/*
 * $Id:$
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT
#include <shared/bsl.h>
#ifdef PORTMOD_SUPPORT

#include <soc/dnxc/legacy/error.h>
#include <soc/dnxc/legacy/dnxc_port.h>
#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <sal/core/sync.h>
#include <soc/phyreg.h>
#include <shared/shrextend/shrextend_debug.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#endif /* BCM_DNX_SUPPORT */

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>
#endif /* BCM_DNXF_SUPPORT */

#include <soc/phy/phymod_port_control.h>
#include <phymod/phymod_acc.h>
#include <phymod/phymod.h>

/*simulator includes*/
#include <soc/phy/phymod_sim.h>

/* CMICx includes */
#include <soc/cmicx_miim.h>

/*******************
 * Static variables
 ******************/

typedef struct dnxc_port_user_access_s {
    int unit; 
    int fsrd_blk_id; 
    sal_mutex_t mutex; 
} dnxc_port_user_access_t;

/** information needed for access */
dnxc_port_user_access_t *g_port_user_access[SOC_MAX_NUM_DEVICES] = {NULL};

/*******************
 * Defines
 ******************/

#define DNXC_PORT_TX_TAP_UNSIGNED_GET(tx_tap) \
    (tx_tap) & 0x000000FF


/******* for soc_dnxc_port_speed_config_lane_config_default_get function ********/

#define DNXC_PORT_RESOURCE_LANE_CONFIG_CLEAR_ALL(lane_config) \
    (lane_config = 0)

/*******************************************************************************/


/****************************************************************************** 
 DNXC MDIO access
*******************************************************************************/

static
int cl45_bus_read(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t* val){

    dnxc_port_user_access_t *cl45_user_data;
    uint16 val16;
    int rv = 0;

    if(user_acc == NULL){
        return _SHR_E_PARAM;
    }
    cl45_user_data = user_acc;

    rv = soc_cmicx_miim_operation(cl45_user_data->unit, 0 /* is_write */, 45, core_addr,
                  reg_addr, &val16);
    (*val) = val16;

    return rv;

}


static
int cl45_bus_write(void* user_acc, uint32_t core_addr, uint32_t reg_addr, uint32_t val){

    uint16 val16;
    dnxc_port_user_access_t *cl45_user_data;

    if(user_acc == NULL){
        return _SHR_E_PARAM;
    }
    cl45_user_data = user_acc;

    val16 = (uint16)val;
    return soc_cmicx_miim_operation(cl45_user_data->unit, 1 /* is_write */, 45, core_addr,
                  reg_addr, &val16);

}


static
int mdio_bus_mutex_take(void* user_acc){
    dnxc_port_user_access_t *user_data;

    if(user_acc == NULL){
        return _SHR_E_PARAM;
    }
    user_data = (dnxc_port_user_access_t *) user_acc;

    return sal_mutex_take(user_data->mutex, sal_mutex_FOREVER); 
}

static
int mdio_bus_mutex_give(void* user_acc){
    dnxc_port_user_access_t *user_data;

    if(user_acc == NULL){
        return _SHR_E_PARAM;
    }
    user_data = (dnxc_port_user_access_t *) user_acc;

    return sal_mutex_give(user_data->mutex); 
}

static int soc_dnxc_port_bus_write_disabled(void* user_acc, uint32_t* val) {
#if defined(BCM_EASY_RELOAD_SUPPORT) || defined(BCM_EASY_RELOAD_WB_COMPAT_SUPPORT)
    dnxc_port_user_access_t *user_data;
    if(user_acc == NULL){
        return _SHR_E_PARAM;
    }

    user_data = (dnxc_port_user_access_t *) user_acc;
    *val = 0;
    if (SOC_IS_RELOADING(user_data->unit)) {
        *val = 1;
    }

#else
    *val = 0;
#endif

    return _SHR_E_NONE;
}

static phymod_bus_t cl45_bus = {
    "dnxc_cl45_with_mutex",
    cl45_bus_read,
    cl45_bus_write,
    soc_dnxc_port_bus_write_disabled,
    mdio_bus_mutex_take,
    mdio_bus_mutex_give,
    0
};

static phymod_bus_t cl45_no_mutex_bus = {
    "dnxc_cl45",
    cl45_bus_read,
    cl45_bus_write,
    soc_dnxc_port_bus_write_disabled,
    NULL,
    NULL,
    0
};


/** 
 *  @brief read SoC properties to get extenal phy chain of
 *         specified phy.
 *  @param unit-
 *  @param phy - lane number
 *  @param addresses_array_size - the maximum number of phy
 *                              addresses that allowed
 *  @param addresses - (output)core addresses
 *  @param phys_in_chain - (output) number of phys read from the
 *                       SoC properties
 *  @note assume that all phys in the chain work with same MDIO
 *        type.
 *  
 */
shr_error_e
soc_dnxc_external_phy_chain_info_get(int unit, int phy, int addresses_array_size, int *addresses, int *phys_in_chain)
{
    int clause;
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

    clause = soc_property_port_get(unit, phy, spn_PORT_PHY_CLAUSE, 45);
    if(clause != 45){
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid Clause value %d", clause);
    }
    *phys_in_chain = soc_property_port_get_csv(unit, phy, spn_PORT_PHY_ADDR, addresses_array_size, addresses);

exit:
    SHR_FUNC_EXIT; 
}



/** 
 *  @brief get phymod access structure for mdio core
 *  @param unit-
 *  @param acc_data - access structure allocated in the caller
 *  @param addr - core address
 *  @param access - function output. the phymod core access
 */
static shr_error_e
soc_dnxc_mdio_phy_access_get(int unit, dnxc_port_user_access_t *acc_data, uint16 addr, phymod_access_t *access, int *is_sim)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(acc_data, _SHR_E_PARAM, "acc_data");
    SHR_NULL_CHECK(access, _SHR_E_PARAM, "access");
    phymod_access_t_init(access);

    PHYMOD_ACC_USER_ACC(access) = acc_data;
    PHYMOD_ACC_ADDR(access) = addr;
    if(acc_data->mutex != NULL){
        PHYMOD_ACC_BUS(access) = &cl45_bus;
    }else{
        PHYMOD_ACC_BUS(access) = &cl45_no_mutex_bus;
    }

    PHYMOD_ACC_F_CLAUSE45_SET(access);

    SHR_IF_ERR_EXIT(soc_physim_check_sim(unit, phymodDispatchTypeBlackhawk, access, 0, is_sim)); 


exit:
    SHR_FUNC_EXIT;  
}


/****************************************************************************** 
 DNXC to portmod mapping
*******************************************************************************/

shr_error_e
dnxc_soc_to_phymod_ref_clk(int unit, int  ref_clk, phymod_ref_clk_t *phymod_ref_clk){
    SHR_FUNC_INIT_VARS(unit);

    *phymod_ref_clk = phymodRefClkCount;
    switch(ref_clk){
        case soc_dnxc_init_serdes_ref_clock_disable: break;
        case soc_dnxc_init_serdes_ref_clock_125:
        case 125:
            *phymod_ref_clk = phymodRefClk125Mhz;
            break;
        case soc_dnxc_init_serdes_ref_clock_156_25:
        case 156:
            *phymod_ref_clk = phymodRefClk156Mhz;
            break;
        case soc_dnxc_init_serdes_ref_clock_312_5:
        case 312:
            *phymod_ref_clk = phymodRefClk312Mhz;
            break;
        default:
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ref clk %d", ref_clk);
    }
exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT
static shr_error_e
dnx_to_phymod_ref_clk(int unit, int ref_clk, phymod_ref_clk_t *phymod_ref_clk)
{
    SHR_FUNC_INIT_VARS(unit);

    *phymod_ref_clk = phymodRefClkCount;
    switch(ref_clk)
    {
        case DNX_SERDES_REF_CLOCK_DISABLE:
            break;
        case DNX_SERDES_REF_CLOCK_125:
            *phymod_ref_clk = phymodRefClk125Mhz;
            break;
        case DNX_SERDES_REF_CLOCK_156_25:
            *phymod_ref_clk = phymodRefClk156Mhz;
            break;
        case DNX_SERDES_REF_CLOCK_312_5:
            *phymod_ref_clk = phymodRefClk312Mhz;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid ref clk %d", ref_clk);
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

/****************************************************************************** 
 External Firmware loader
*******************************************************************************/
#define UC_TABLE_ENTRY_SIZE (4)


/** 
 *  @brief This function loads firmware to all Fabric cores. IF
 *         firmware already loaded this function will do
 *         nothing.
 *  @param length -  the firmware length
 *  @param data - the uCode
 *  @note assumes that all cores use the same firmware
 */
int
soc_dnxc_fabric_broadcast_firmware_loader(int unit,  uint32_t length, const uint8_t* data)
{
    soc_reg_above_64_val_t wr_data;
    int i=0;
    int word_index = 0;

    for (i = 0 ; i < length ; i+= UC_TABLE_ENTRY_SIZE){
        SOC_REG_ABOVE_64_CLEAR(wr_data);
        if(i + UC_TABLE_ENTRY_SIZE < length){
            sal_memcpy((uint8 *)wr_data, data + i, UC_TABLE_ENTRY_SIZE);
        }else{ /*last time*/
            sal_memcpy((uint8 *)wr_data, data + i, length - i);
        }
        /*swap every 4 bytes in case of big endian*/
        for(word_index = 0 ; word_index < sizeof(soc_reg_above_64_val_t)/4; word_index++) {
            wr_data[word_index] = _shr_uint32_read((uint8 *)&wr_data[word_index]);
        }
        /*we write to index 0 always*/
        PHYMOD_IF_ERR_RETURN(WRITE_BRDC_FSRD_FSRD_PROM_MEMm(unit, MEM_BLOCK_ALL, 0, wr_data));
        
    }

    return _SHR_E_NONE;
}


/**
 *  @brief This function load firmware to a Fabric core. IF
 *         firmware already loaded this function will do
 *         nothing.
 *         This is callback function that called during phymod
 *         core_init
 *  @param core- phymod core access
 *  @param length -  the firmware length
 *  @param data - the uCode
 *  @note assumes that all cores use the same firmware
 */
static int
soc_dnxc_fabric_firmware_loader_callback(const phymod_core_access_t* core,  uint32_t length, const uint8_t* data)
{
    dnxc_port_user_access_t *user_data;
    soc_reg_above_64_val_t wr_data;
    int i=0;
    int word_index = 0;

    user_data = core->access.user_acc;

    for (i = 0 ; i < length ; i+= UC_TABLE_ENTRY_SIZE){
        SOC_REG_ABOVE_64_CLEAR(wr_data);
        if(i + UC_TABLE_ENTRY_SIZE < length){
            sal_memcpy((uint8 *)wr_data, data + i, UC_TABLE_ENTRY_SIZE);
        }else{ /*last time*/
            sal_memcpy((uint8 *)wr_data, data + i, length - i);
        }
        /*swap every 4 bytes in case of big endian*/
        for(word_index = 0 ; word_index < sizeof(soc_reg_above_64_val_t)/4; word_index++) {
            wr_data[word_index] = _shr_uint32_read((uint8 *)&wr_data[word_index]);
        }  
        /*we write to index 0 always*/
        PHYMOD_IF_ERR_RETURN(WRITE_FSRD_FSRD_PROM_MEMm(user_data->unit, user_data->fsrd_blk_id, 0, wr_data));
    }

    return PHYMOD_E_NONE;
}

/****************************************************************************** 
 Fabric PMs init
*******************************************************************************/

/**
 * \brief
 *   Add Fabric PM database.
 * \param [in] unit -
 *   The unit id.
 * \param [in] core -
 *   The PM core id.
 * \param [in] cores_num -
 *   Number of PM cores.
 * \param [in] phy_offset -
 *   The start offset of fabric phys.
 * \param [in] use_mutex -
 *   Use (1) / don't use (0) mutex.
 * \param [in] address_get_func -
 *   MDIO address calculation function.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
soc_dnxc_fabric_single_pm_add(int unit, int core, int cores_num, int phy_offset, int use_mutex, dnxc_core_address_get_f address_get_func)
{
    portmod_pm_create_info_t pm_info;
    int cl = -1;
    int phy = -1;
    int core_mac_index = 0;
    int mac_core = -1;
    int fmac_block_id = -1;
    int fsrd_block_id = -1;
    uint16 addr = 0;
    int is_sim = 0;
    soc_dnxc_init_serdes_ref_clock_t ref_clk = phymodRefClkCount;
    int core_port_index = 0;
    uint32 rx_polarity = 0;
    uint32 tx_polarity = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*Clause*/
    cl = soc_property_suffix_num_get(unit, -1, spn_PORT_PHY_CLAUSE  , "fabric", 45);
    if (cl != 45) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid clause %d", cl);
    }

    if (g_port_user_access[unit] == NULL)
    {
        SHR_ALLOC_SET_ZERO(g_port_user_access[unit], sizeof(dnxc_port_user_access_t)*cores_num, "g_port_user_access", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    }

    g_port_user_access[unit][core].unit = unit;
    g_port_user_access[unit][core].mutex = (use_mutex) ? sal_mutex_create("core mutex") : NULL;

    SHR_IF_ERR_EXIT(portmod_pm_create_info_t_init(unit, &pm_info));

    pm_info.pm_specific_info.pm8x50_fabric.core_index = core;
    pm_info.type = portmodDispatchTypePm8x50_fabric;
    pm_info.pm_specific_info.pm8x50_fabric.first_phy_offset = 0;
    pm_info.pm_specific_info.pm8x50_fabric.fw_load_method = soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, "fabric", phymodFirmwareLoadMethodExternal);
    pm_info.pm_specific_info.pm8x50_fabric.fw_load_method &= 0xff;
    if( pm_info.pm_specific_info.pm8x50_fabric.fw_load_method == phymodFirmwareLoadMethodExternal){
        pm_info.pm_specific_info.pm8x50_fabric.external_fw_loader =  soc_dnxc_fabric_firmware_loader_callback;
    }

    phy = phy_offset + core*DNXC_PORT_FABRIC_LANES_PER_CORE;

    for(core_mac_index = 0; core_mac_index< PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC; ++core_mac_index) {
        mac_core = core*PORTMOD_NOF_FMACS_PER_PM8X50_FABRIC + core_mac_index;
        fmac_block_id = FMAC_BLOCK(unit, mac_core);
        pm_info.pm_specific_info.pm8x50_fabric.fmac_schan_id[core_mac_index] = SOC_BLOCK_INFO(unit, fmac_block_id).schan;
    }
    fsrd_block_id = FSRD_BLOCK(unit, core);
    g_port_user_access[unit][core].fsrd_blk_id = fsrd_block_id;
    pm_info.pm_specific_info.pm8x50_fabric.fsrd_schan_id = SOC_BLOCK_INFO(unit, fsrd_block_id).schan;

    SHR_IF_ERR_EXIT(address_get_func(unit, core, &addr));
    SHR_IF_ERR_EXIT(soc_dnxc_mdio_phy_access_get(unit, &g_port_user_access[unit][core], addr, &pm_info.pm_specific_info.pm8x50_fabric.access, &is_sim));

    if(is_sim) {
        pm_info.pm_specific_info.pm8x50_fabric.fw_load_method = phymodFirmwareLoadMethodNone;
    }

    if (SOC_IS_DNX(unit)) {
#ifdef BCM_DNX_SUPPORT
        int pll_index = -1;

        pll_index = (core*DNXC_PORT_FABRIC_LANES_PER_CORE) / (dnx_data_fabric.links.nof_links_get(unit) / 2);
        ref_clk = dnx_data_pll.general.fabric_pll_cfg_get(unit, pll_index)->out_freq;
        SHR_IF_ERR_EXIT(dnx_to_phymod_ref_clk(unit, ref_clk, &pm_info.pm_specific_info.pm8x50_fabric.ref_clk));
#endif
    }
    else
    {
        ref_clk = SOC_INFO(unit).port_refclk_int[phy];
        SHR_IF_ERR_EXIT(dnxc_soc_to_phymod_ref_clk(unit, ref_clk, &pm_info.pm_specific_info.pm8x50_fabric.ref_clk));
    }

    /*polarity*/
    PORTMOD_PBMP_CLEAR(pm_info.phys);
    for(core_port_index = 0 ;  core_port_index < DNXC_PORT_FABRIC_LANES_PER_CORE ; core_port_index++){
        if (!SOC_WARM_BOOT(unit))
        {
            rx_polarity = soc_property_port_get(unit, phy, spn_PHY_RX_POLARITY_FLIP, 0);
            tx_polarity = soc_property_port_get(unit, phy, spn_PHY_TX_POLARITY_FLIP, 0);
            pm_info.pm_specific_info.pm8x50_fabric.polarity.rx_polarity |= ((rx_polarity & 0x1) << core_port_index);
            pm_info.pm_specific_info.pm8x50_fabric.polarity.tx_polarity |= ((tx_polarity & 0x1) << core_port_index);

        } 

        PORTMOD_PBMP_PORT_ADD(pm_info.phys, phy);
        phy++;
    }

    pm_info.pm_specific_info.pm8x50_fabric.force_single_pll = (SOC_IS_RAMON_A0(unit))? 1 : 0;

    SHR_IF_ERR_EXIT(portmod_port_macro_add(unit, &pm_info));

exit:
    if (_func_rv != _SHR_E_NONE)
    {
        soc_dnxc_fabric_pms_destroy(unit, cores_num);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Add all Fabric PMs.
 *   See soc_dnxc_fabric_pms_add.
 */
shr_error_e
soc_dnxc_fabric_pms_add(int unit, int cores_num, int phy_offset, int use_mutex, dnxc_core_address_get_f address_get_func)
{
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);

    /* Assuming one PM per one SerDes core */
    for (core = 0; core < cores_num ; core++)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_fabric_single_pm_add(unit, core, cores_num, phy_offset, use_mutex, address_get_func));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Destory all Fabric PMs.
 */
void
soc_dnxc_fabric_pms_destroy(int unit, int cores_num)
{
    int core = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (g_port_user_access[unit] != NULL)
    {
        for(core = 0 ; core < cores_num ; core++)
        {
            if(g_port_user_access[unit][core].mutex != NULL)
            {
                sal_mutex_destroy(g_port_user_access[unit][core].mutex);
            }
        }
        SHR_FREE(g_port_user_access[unit]);
    }

    SHR_VOID_FUNC_EXIT;
}

shr_error_e
soc_dnxc_fabric_port_probe(int unit, int port, dnxc_port_init_stage_t init_stage, int fw_verify, dnxc_port_fabric_init_config_t* port_config)
{
    portmod_port_add_info_t info;
    int phy = 0;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit)) {

        SHR_IF_ERR_EXIT(portmod_port_add_info_t_init(unit, &info));

        if (soc_feature(unit, soc_feature_logical_port_num)) {
            phy = SOC_INFO(unit).port_l2p_mapping[port];
        } else{
            phy = port;
        }
        PORTMOD_PBMP_PORT_ADD(info.phys, phy);
        info.interface_config.interface = SOC_PORT_IF_SFI;

        /* pass to pm an array where all lanes are mapped (even lanes defined as unmapped by user) - to configure FMAC and phymod without holes */
        for (i = 0; i < DNXC_PORT_FABRIC_LANES_PER_CORE; ++i)
        {
            info.lane_map.lane_map_rx[i] = port_config->lane2serdes[i].rx_id;
            info.lane_map.lane_map_tx[i] = port_config->lane2serdes[i].tx_id;
        }
        info.lane_map.num_of_lanes = DNXC_PORT_FABRIC_LANES_PER_CORE;

        if(init_stage == dnxc_port_init_pass1) {
            PORTMOD_PORT_ADD_F_INIT_CORE_PROBE_SET(&info);
            PORTMOD_PORT_ADD_F_INIT_PASS1_SET(&info);
        }

        if(init_stage == dnxc_port_init_pass2) {
            PORTMOD_PORT_ADD_F_INIT_PASS2_SET(&info);
        }

        if(fw_verify) {
            PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_SET(&info);
        } else {
            PORTMOD_PORT_ADD_F_FIRMWARE_LOAD_VERIFY_CLR(&info);
        }


        SHR_IF_ERR_EXIT(portmod_port_add(unit, port, &info));

        if(init_stage == dnxc_port_init_pass1) {
            SHR_EXIT();
        }

        if (SOC_IS_DNX(unit)) {
            
            
            /* tx_params_set function moved to dnxf_data for Ramon. Currently this SoC property isn't read for J2 */
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_cl72_set(int unit, soc_port_t port, int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_cl72_set(unit, port, (enable ? 1 : 0))); 

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_cl72_get(int unit, soc_port_t port, int *enable)
{
    uint32 local_enable;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_cl72_get(unit, port, &local_enable)); 
    *enable = (local_enable ? 1 : 0);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_dnxc_port_portmod_polarity_db_set
 * Purpose:
 *      Set polarity for a port in portmod WB engine database
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  Port number
 *      is_rx     - (IN)  0 - tx polarity, 1 - rx polarity
 *      value     - (IN)  0 - polarity is straight, 1 - flip polarity
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
static shr_error_e
soc_dnxc_port_portmod_polarity_db_set(int unit, soc_port_t port, int is_rx, uint32 value)
{
    phymod_polarity_t polarity;
    int core_port_index;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit)) {
        DNXC_LEGACY_FIXME_ASSERT;
    }

    SHR_IF_ERR_EXIT(portmod_port_polarity_get(unit, port, &polarity));
    core_port_index = port % DNXC_PORT_FABRIC_LANES_PER_CORE;
    if (is_rx) {
        SHR_BITWRITE(&polarity.rx_polarity, core_port_index, value);
    } else { /* tx */
        SHR_BITWRITE(&polarity.tx_polarity, core_port_index, value);
    }
    SHR_IF_ERR_EXIT(portmod_port_polarity_set(unit, port, &polarity));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_phy_control_is_supported_validate(int unit, soc_port_t port, soc_phy_control_t type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        /* supported controls */
        case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_VGA:
        case SOC_PHY_CONTROL_RX_TAP1:
        case SOC_PHY_CONTROL_RX_TAP2:
        case SOC_PHY_CONTROL_RX_TAP3:
        case SOC_PHY_CONTROL_RX_TAP4:
        case SOC_PHY_CONTROL_RX_TAP5:
        case SOC_PHY_CONTROL_RX_ADAPTATION_RESUME:
        case SOC_PHY_CONTROL_PHASE_INTERP:
        case SOC_PHY_CONTROL_RX_POLARITY:
        case SOC_PHY_CONTROL_TX_POLARITY:
        case SOC_PHY_CONTROL_DUMP:
        case SOC_PHY_CONTROL_CL72_STATUS:
        case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        case SOC_PHY_CONTROL_TX_PATTERN_GEN_ENABLE:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        case SOC_PHY_CONTROL_TX_PPM_ADJUST:
        case SOC_PHY_CONTROL_RX_SIGNAL_DETECT:
        case SOC_PHY_CONTROL_RX_SEQ_DONE:
        case SOC_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE:
        case SOC_PHY_CONTROL_LP_TX_PRECODER_ENABLE:
        case SOC_PHY_CONTROL_RX_LANE_SQUELCH:
            break;
        
        case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
        case SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91:
        case SOC_PHY_CONTROL_AUTONEG_MODE:
        case SOC_PHY_CONTROL_TX_FIR_DRIVERMODE:
        case SOC_PHY_CONTROL_DRIVER_CURRENT:
            if (IS_SFI_PORT(unit, port))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported.");
            }
            else
            {
                DNXC_LEGACY_FIXME_ASSERT; 
            }
            break;
        case SOC_PHY_CONTROL_SCRAMBLER:
            if (IS_SFI_PORT(unit, port))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported.");
            }
            break;
        /* unsupported controls */
        /* resource API */
        case SOC_PHY_CONTROL_CL72:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_CL72 is no longer supported, to control link training please use bcm_port_resource_set/get.");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE is no longer supported, to control BR DFE please use bcm_port_resource_set/get.");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE is no longer supported, to control LP DFE please use bcm_port_resource_set/get.");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_FIRMWARE_DFE_ENABLE is no longer supported, to control DFE please use bcm_port_resource_set/get.");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_FIRMWARE_CL72_RESTART_TIMEOUT_ENABLE is no longer supported, to control CL72 restart timeout please use bcm_port_resource_set/get.");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_FIRMWARE_CL72_AUTO_POLARITY_ENABLE is no longer supported, to control CL72 auto polarity please use bcm_port_resource_set/get.");
            break;
        case SOC_PHY_CONTROL_UNRELIABLE_LOS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_UNRELIABLE_LOS is no longer supported, to control unreliable los please use bcm_port_resource_set/get.");
            break;
        case SOC_PHY_CONTROL_MEDIUM_TYPE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_CONTROL_MEDIUM_TYPE is no longer supported, bcm_port_resource_set/get can be used to set the medium.");
            break;
        case SOC_PHY_CONTROL_FIRMWARE_MODE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_FIRMWARE_MODE is no longer supported, bcm_port_resource_set/get can be used to set firmware configurations.");
            break;
        case SOC_PHY_CONTROL_INTERFACE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_INTERFACE is no longer supported, to align interface properties please use bcm_port_resource_set/get.");
            break;
        case SOC_PHY_CONTROL_PREEMPHASIS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_PREEMPHASIS is no longer supported, please use bcm_port_phy_tx_set/get APIs instead.");
            break;
        case SOC_PHY_CONTROL_TX_FIR_PRE:
        case SOC_PHY_CONTROL_TX_FIR_PRE2:
        case SOC_PHY_CONTROL_TX_FIR_MAIN:
        case SOC_PHY_CONTROL_TX_FIR_POST:
        case SOC_PHY_CONTROL_TX_FIR_POST2:
        case SOC_PHY_CONTROL_TX_FIR_POST3:
        case SOC_PHY_CONTROL_TX_FIR_MODE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_TX_FIR controls are no longer supported, please use bcm_port_phy_tx_set/get APIs instead.");
            break;
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE0:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE1:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE2:
        case SOC_PHY_CONTROL_PREEMPHASIS_LANE3:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_PREEMPHASIS_LANE<> is no longer supported, please use bcm_port_phy_tx_set/get APIs instead.");
            break;
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE0:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE1:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE2:
        case SOC_PHY_CONTROL_DRIVER_CURRENT_LANE3:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_DRIVER_CURRENT_LANE<> is not supported.");
            break;
        case SOC_PHY_CONTROL_RX_VGA_RELEASE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_RX_VGA_RELEASE is no longer supported. Please use BCM_PORT_PHY_CONTROL_RX_ADAPTATION_RESUME instead.");
            break;
        case SOC_PHY_CONTROL_RX_TAP1_RELEASE:
        case SOC_PHY_CONTROL_RX_TAP2_RELEASE:
        case SOC_PHY_CONTROL_RX_TAP3_RELEASE:
        case SOC_PHY_CONTROL_RX_TAP4_RELEASE:
        case SOC_PHY_CONTROL_RX_TAP5_RELEASE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_RX_TAP<>_RELEASE controls are no longer supported. Please use BCM_PORT_PHY_CONTROL_RX_ADAPTATION_RESUME instead.");
            break;
        case SOC_PHY_CONTROL_TX_LANE_SQUELCH:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_TX_LANE_SQUELCH is no longer supported, please use bcm_port_enable_set/get instead.");
            break;
        case SOC_PHY_CONTROL_TX_RESET:
        case SOC_PHY_CONTROL_RX_RESET:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Reset RX or TX only using BCM_PORT_PHY_CONTROL_RX/TX_RESET is no longer supported. Please use bcm_port_enable_set/get to enable/disable both RX and TX.");
            break;
        case SOC_PHY_CONTROL_POWER:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_POWER is no longer supported. Please use bcm_port_enable_set/get instead.");
            break;
        case SOC_PHY_CONTROL_LANE_SWAP:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_LANE_SWAP is no longer supported, please use bcm_port_lane_to_serdes_map_set/get instead.");
            break;
        case SOC_PHY_CONTROL_PRBS_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_TX_ENABLE:
        case SOC_PHY_CONTROL_PRBS_RX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_RX_ENABLE:
        case SOC_PHY_CONTROL_PRBS_RX_STATUS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_PRBS_ controls are no longer supported. To enable PRBS please refer to PRBS section in UM.");
            break;
        case SOC_PHY_CONTROL_LOOPBACK_INTERNAL:
        case SOC_PHY_CONTROL_LOOPBACK_PMD:
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE:
        case SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_LOOPBACK_ controls are no longer supported. For loopback configuration please use bcm_port_loopback_set/get API.");
            break;
        case SOC_PHY_CONTROL_RX_SEQ_TOGGLE:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_RX_SEQ_TOGGLE is no longer supported. To reset the RX path, use BCM_PORT_PHY_CONTROL_RX_LANE_SQUELCH.");
            break;
        case SOC_PHY_CONTROL_GPIO_CONFIG:
        case SOC_PHY_CONTROL_GPIO_VALUE:
        case SOC_PHY_CONTROL_INTR_MASK:
        case SOC_PHY_CONTROL_INTR_STATUS:
        case SOC_PHY_CONTROL_LINKDOWN_TRANSMIT:
        case SOC_PHY_CONTROL_HG2_BCM_CODEC_ENABLE:
        case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE:
        case SOC_PHY_CONTROL_SHORT_CHANNEL_MODE_STATUS:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported.");
            break;
        case SOC_PHY_CONTROL_EEE:
            if (IS_SFI_PORT(unit, port))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported.");
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "BCM_PORT_PHY_CONTROL_EEE is not required to enable EEE. Please refer to EEE documentation for correct sequence.");
            }
            break;
        /* EYE controls- no problem to support, but they are not implemented for blackhawk */
        case SOC_PHY_CONTROL_EYE_VAL_HZ_L:
        case SOC_PHY_CONTROL_EYE_VAL_HZ_R:
        case SOC_PHY_CONTROL_EYE_VAL_VT_U:
        case SOC_PHY_CONTROL_EYE_VAL_VT_D:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported.");
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This control is not supported.");
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_dnxc_port_phy_control_set_to_disable_port_get(int unit, soc_port_t port, soc_phy_control_t type, int* to_disable)
{
    SHR_FUNC_INIT_VARS(unit);

    /* except for the exception cases, all phy controls are set under disabled port */
    switch (type)
    {
        case SOC_PHY_CONTROL_DUMP:
        case SOC_PHY_CONTROL_TX_PATTERN_LENGTH:
        case SOC_PHY_CONTROL_TX_PATTERN_GEN_ENABLE:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA0:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA1:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA2:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA3:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA4:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA5:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA6:
        case SOC_PHY_CONTROL_TX_PATTERN_DATA7:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_TX_ENABLE:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_POLYNOMIAL:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_INVERT_DATA:
        case SOC_PHY_CONTROL_PRBS_DECOUPLED_RX_ENABLE:
        case SOC_PHY_CONTROL_RX_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_LOW_FREQ_PEAK_FILTER:
        case SOC_PHY_CONTROL_RX_VGA:
        case SOC_PHY_CONTROL_RX_TAP1:
        case SOC_PHY_CONTROL_RX_TAP2:
        case SOC_PHY_CONTROL_RX_TAP3:
        case SOC_PHY_CONTROL_RX_TAP4:
        case SOC_PHY_CONTROL_RX_TAP5:
        case SOC_PHY_CONTROL_RX_ADAPTATION_RESUME:
        case SOC_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE:
        case SOC_PHY_CONTROL_LP_TX_PRECODER_ENABLE:
        case SOC_PHY_CONTROL_RX_LANE_SQUELCH:
            *to_disable = 0;
            break;
        default:
            *to_disable = 1;
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_control_set(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 value)
{
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    int is_lane_control, rv;
    phymod_ref_clk_t ref_clk;
    int is_legacy_ext_phy = 0;
    uint32_t port_dynamic_state = 0;
    int orig_enabled = 0;
    int to_disable = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_is_supported_validate(unit, port, type));

    SHR_IF_ERR_EXIT(soc_dnxc_port_enable_get(unit, port, &orig_enabled));
    /* if to disable port when setting this PHY control*/
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_set_to_disable_port_get(unit, port, type, &to_disable));
    if (orig_enabled && to_disable)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 0));
    }

    rv = dnxc_soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk);
    SHR_IF_ERR_EXIT(rv);

    portmod_access_get_params_t_init(unit, &params);
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;

    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));
    switch (type)
    {
        /*not per lane control*/
        case SOC_PHY_CONTROL_LANE_SWAP:
            is_lane_control = 0;
            break;
        default:
            is_lane_control = 1;
    }

    if( params.phyn != 0) { /* only check if phy is legacy when phyn is not 0 (in portmod internal phy is always not legacy) */
        SHR_IF_ERR_EXIT(portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_ext_phy));
    }

    if (!is_legacy_ext_phy) { 
        if(type != SOC_PHY_CONTROL_AUTONEG_MODE){
            SHR_IF_ERR_EXIT(soc_port_control_set_wrapper(unit, ref_clk, is_lane_control, phys, phys_returned, type, value));
        }
        else{
            port_dynamic_state |= 0x2;
            port_dynamic_state |= value << 16;
            portmod_port_update_dynamic_state(unit, port, port_dynamic_state);
        }
    } else {
        SHR_IF_ERR_EXIT(portmod_port_ext_phy_control_set(unit, port, phyn, lane, is_sys_side, type, value));
    }

    if (type == SOC_PHY_CONTROL_RX_POLARITY || type == SOC_PHY_CONTROL_TX_POLARITY) {
        int is_rx = (type == SOC_PHY_CONTROL_RX_POLARITY)? 1 : 0;
        SHR_IF_ERR_EXIT(soc_dnxc_port_portmod_polarity_db_set(unit, port, is_rx, value));
    }

    if (orig_enabled && to_disable)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_dnxc_port_phy_control_get(int unit, soc_port_t port, int phyn, int lane, int is_sys_side, soc_phy_control_t type, uint32 *value)
{
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params; 
    int is_lane_control, rv;
    phymod_ref_clk_t ref_clk;
    int is_legacy_ext_phy = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_control_is_supported_validate(unit, port, type));

    rv = dnxc_soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk);
    SHR_IF_ERR_EXIT(rv);

    portmod_access_get_params_t_init(unit, &params);
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;

    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));
    switch (type)
    {
        /*not per lane control*/
        case SOC_PHY_CONTROL_LANE_SWAP:
            is_lane_control = 0;
            break;
        default:
            is_lane_control = 1;
    }

    if( params.phyn != 0) { /* only check if phy is legacy when phyn is not 0 (in portmod internal phy is always not legacy) */
        SHR_IF_ERR_EXIT(portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_ext_phy));
    }

    if (!is_legacy_ext_phy) {
        SHR_IF_ERR_EXIT(soc_port_control_get_wrapper(unit, ref_clk, is_lane_control, phys, phys_returned, type, value));

        if (type == SOC_PHY_CONTROL_TX_FIR_MAIN)
        {
            *value = DNXC_PORT_TX_TAP_UNSIGNED_GET(*value);
        }
    } else {
        SHR_IF_ERR_EXIT(portmod_port_ext_phy_control_get(unit, port, phyn, lane, is_sys_side, type, value));
    }

exit:
    SHR_FUNC_EXIT;
}

/* legality validation of a single bcm_port_resource_t entry */
shr_error_e
soc_dnxc_port_resource_validate(int unit, const bcm_port_resource_t *resource)
{
    uint32 pam4_channel_loss;
    SHR_FUNC_INIT_VARS(unit);

     if ((resource->flags != 0) || (resource->physical_port != 0) || (resource->lanes != 0) || (resource->encap != 0))
     {
         SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: flags, physical_port, lanes and encap fields of resource are not in use. They should all be initialized to 0", resource->port);
     }

     if ( (resource->speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST) ||
             (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST) ||
             (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST) ||
             (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST) )
     {
         SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: speed, fec_type, link_training and phy_lane_config fields of resource are mandatory as input to this API.\n Call bcm_port_resource_default_get to fill them", resource->port);
     }

     if (PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_GET(resource->phy_lane_config))
     {
         SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: bit 14 of phy_lane_config is internal. Do not set it.", resource->port);
     }

     if (PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_GET(resource->phy_lane_config))
     {
         SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: bit 15 of phy_lane_config is internal. Do not set it.", resource->port);
     }

     if ((resource->link_training == 1) && !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(resource->phy_lane_config))
     {
         SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: DFE is mandatory for link training", resource->port);
     }

     if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_GET(resource->phy_lane_config) && !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(resource->phy_lane_config))
     {
         SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: when LP_DFE is on, DFE must be on as well", resource->port);
     }

     /* FORCE_NS, FORCE_ES configuration */
     if (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(resource->phy_lane_config) && BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource->phy_lane_config))
     {
         SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: setting both FORCE_NS and FORCE_ES is not a valid option. Only one can be set.", resource->port);
     }

     if (DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed) && (resource->link_training == 0) &&
             !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(resource->phy_lane_config) && !BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource->phy_lane_config))
     {
         SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: if mode is PAM4 and link training is disabled, either FORCE_NS or FORCE_ES has to be set. They are both 0.", resource->port);
     }

     if (!DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed) &&
             (BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(resource->phy_lane_config) || BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource->phy_lane_config)))
     {
         SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: if mode is NRZ, both FORCE_NS and FORCE_ES have to be 0. FORCE_NS is %d, FORCE_ES is %d.", resource->port,
                 BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(resource->phy_lane_config), BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource->phy_lane_config));
     }

     if (!DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed))
     {
         pam4_channel_loss = PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_GET(resource->phy_lane_config);
         if (pam4_channel_loss)
         {
             SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: pam4 channel loss configuration - bits [16:21] of phy_lane_config - is expected to be 0 when mode is nrz. speed is %d, pam4 channel loss is %u.", resource->port, resource->speed, pam4_channel_loss);
         }
     }

     if (resource->link_training == 1)
     {
         pam4_channel_loss = PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_GET(resource->phy_lane_config);
         if (pam4_channel_loss)
         {
             SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: pam4 channel loss configuration - bits [16:21] of phy_lane_config - is expected to be 0 when link training is enabled. pam4 channel loss is %u.", resource->port, pam4_channel_loss);
         }
     }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_resource_get(int unit, bcm_gport_t port, bcm_port_resource_t *resource)
{
    portmod_speed_config_t port_speed_config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    SHR_IF_ERR_EXIT(portmod_speed_config_t_init(unit, &port_speed_config));
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &port_speed_config));

    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(port_speed_config.lane_config);
    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(port_speed_config.lane_config);
    /* LP precoder FW bit is not configured by resource API but by bcm_port_phy_control API, so here we mask-out this bit */
    PORTMOD_PORT_PHY_LANE_CONFIG_LP_PREC_EN_CLEAR(port_speed_config.lane_config);

    bcm_port_resource_t_init(resource);

    resource->speed = port_speed_config.speed;
    resource->fec_type = (bcm_port_phy_fec_t)port_speed_config.fec;
    resource->link_training = port_speed_config.link_training;
    resource->phy_lane_config = port_speed_config.lane_config;

    resource->port = port;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Get recommended default values of port_speed_config->lane_config.
 * This function assumes resource->speed is initialized to correct value.
 */
static shr_error_e
soc_dnxc_port_fabric_resource_lane_config_default_get(int unit, bcm_gport_t port, bcm_port_resource_t *resource) {
    SHR_FUNC_INIT_VARS(unit);

    DNXC_PORT_RESOURCE_LANE_CONFIG_CLEAR_ALL(resource->phy_lane_config);

    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_LP_DFE_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_MEDIUM_SET(resource->phy_lane_config, SOC_DNXC_PORT_LANE_CONFIG_MEDIA_TYPE_BACKPLANE_COPPER_TRACE);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_UNRELIABLE_LOS_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_SCRAMBLING_DISABLE_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_POLARITY_AUTO_EN_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_CL72_RESTART_TIMEOUT_EN_SET(resource->phy_lane_config);

    /* FORCE_ES, FORCE_NS must be clear for {NRZ} and for {PAM4 with link training} */
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_CLEAR(resource->phy_lane_config);
    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_CLEAR(resource->phy_lane_config);
    if (DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed) && (resource->link_training == 0))
    {
        BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(resource->phy_lane_config);
    }

    /* internal fields that will be set in resource_set before calling portmod. Not exposed to user. */
    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_PAM4_CLEAR(resource->phy_lane_config);
    PORTMOD_PORT_PHY_LANE_CONFIG_FORCE_NRZ_CLEAR(resource->phy_lane_config);
    /* this field should always be 0 in normal operation, can be changed only for debug. */
    PORTMOD_PORT_PHY_LANE_CONFIG_PAM4_CHANNEL_LOSS_SET(resource->phy_lane_config, 0);

    SHR_FUNC_EXIT;
}

/*
 * Get recommended default values of resource members: fec_type, link training (CL72) and phy_lane_config.
 */
shr_error_e
soc_dnxc_port_fabric_resource_default_get(int unit, bcm_gport_t port, uint32 flags, bcm_port_resource_t *resource)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(resource, _SHR_E_PARAM, "resource");

    if ((resource->flags != 0) || (resource->physical_port != 0) || (resource->lanes != 0) || (resource->encap != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: flags, physical_port, lanes and encap fields of bcm_port_resource_t are not in use. They should all be initialized to 0", port);
    }

    if ((resource->port != 0) && (resource->port != port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "port %d: resource->port should be 0 or equal to port passed to the API", port);
    }

    if (resource->speed == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "port %d: there is no given default value for speed. speed is mandatory as input to this API", port);
    }

    /* in case user didn't configure the FEC already - give default value */
    if (resource->fec_type == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        if (DNXC_PORT_PHY_SPEED_IS_PAM4(resource->speed))
        {
            /* 15T RS FEC */
            resource->fec_type = bcmPortPhyFecRs545;
        }
        else
        {
            /* 5T RS FEC */
            resource->fec_type = bcmPortPhyFecRs206;
        }
    }
    /* in case user didn't configure CL72 - by default it is enabled */
    if (resource->link_training == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        resource->link_training = 1;
    }

    /* get default lane_config */
    if (resource->phy_lane_config == BCM_PORT_RESOURCE_DEFAULT_REQUEST)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_fabric_resource_lane_config_default_get(unit, port, resource));
    }

    resource->port = port;

exit:
    SHR_FUNC_EXIT;
}

/*
 * Set TX FIR parameters per port.
 * This function can be called both in init sequence and via the tx_set API.
 * if it is called in init sequence:
 *      the port will not be disabled inside the function, since it was not enabled yet.
 *      - if tx->main is INVALID, this means SoC property was not configured. In this case phymod TX FIR default will be configured, according to port speed.
 *      - otherwise, tx struct contains the SoC property values, and they will be configured.
 * if it is called via tx_set API:
 *      the port will be disabled inside the function.
 *      values given in tx struct will be configured.
 */
shr_error_e
soc_dnxc_port_phy_tx_set(int unit, bcm_port_t port, int phyn, int lane, int is_sys_side, int is_init_sequence, bcm_port_phy_tx_t* tx)
{
    int orig_enabled = 0;
    phymod_tx_t phymod_tx;
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    int phy_index;
    bcm_port_resource_t resource;
    phymod_phy_signalling_method_t signalling_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tx, _SHR_E_PARAM, "tx");

    /* disable port if not in init sequence. In init seqence port shouldn't be enabled yet. */
    if (!is_init_sequence)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_enable_get(unit, port, &orig_enabled));
        if (orig_enabled)
        {
            SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 0));
        }
    }

    portmod_access_get_params_t_init(unit, &params);
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;

    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));

    SHR_IF_ERR_EXIT(phymod_tx_t_init(&phymod_tx));

    /* in init sequence: if the SoC property was not configured - set phymod defaults according to the configured speed - PAM4/NRZ */
    if (is_init_sequence && (tx->main == DNXC_PORT_TX_FIR_INVALID_MAIN_TAP))
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_resource_get(unit, port, &resource));
        signalling_mode = (DNXC_PORT_PHY_SPEED_IS_PAM4(resource.speed))? phymodSignallingMethodPAM4 : phymodSignallingMethodNRZ;

        for (phy_index = 0 ; phy_index < phys_returned; phy_index++)
        {
            /* if lane mask is 0, skip it. */
            if(phys[phy_index].access.lane_mask)
            {
                SHR_IF_ERR_EXIT(phymod_phy_tx_taps_default_get(&phys[phy_index], signalling_mode, &phymod_tx));
                phymod_tx.signallingMode = signalling_mode;
                break;
            }
        }
    }
    else /* configure Tx params as given to the function */
    {
        phymod_tx.pre2 = tx->pre2;
        phymod_tx.pre = tx->pre;
        phymod_tx.main = tx->main;
        phymod_tx.post = tx->post;
        phymod_tx.post2 = tx->post2;
        phymod_tx.post3 = tx->post3;
        phymod_tx.txTapMode = (tx->tx_tap_mode == bcmPortPhyTxTapMode3Tap)? phymodTxTapMode3Tap : phymodTxTapMode6Tap;
        phymod_tx.signallingMode = (tx->signalling_mode == bcmPortPhySignallingModeNRZ)? phymodSignallingMethodNRZ : phymodSignallingMethodPAM4;
    }

    /* set TX FIR parameters on all phys of the port */
    for (phy_index = 0 ; phy_index < phys_returned; phy_index++){
        /* if lane mask is 0, skip it. */
        if(phys[phy_index].access.lane_mask) {
            SHR_IF_ERR_EXIT(phymod_phy_tx_set(&phys[phy_index], &phymod_tx));
        }
    }

    /* in init sequence port shouldn't be enabled yet. */
    if (!is_init_sequence && orig_enabled)
    {
        SHR_IF_ERR_EXIT(soc_dnxc_port_enable_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_tx_get(int unit, bcm_port_t port, int phyn, int lane, int is_sys_side, bcm_port_phy_tx_t* tx)
{
    phymod_tx_t phymod_tx;
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    int phy_index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(tx, _SHR_E_PARAM, "tx");

    portmod_access_get_params_t_init(unit, &params);
    params.lane = lane;
    params.phyn = (phyn == -1 ? PORTMOD_PHYN_LAST_ONE : phyn);
    params.sys_side = is_sys_side ? PORTMOD_SIDE_SYSTEM : PORTMOD_SIDE_LINE;

    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));

    bcm_port_phy_tx_t_init(tx);

    for (phy_index = 0 ; phy_index < phys_returned; phy_index++){
        /* if lane mask is 0, skip it. */
        if(phys[phy_index].access.lane_mask) {
            SHR_IF_ERR_EXIT(phymod_phy_tx_get(&phys[phy_index], &phymod_tx));

            tx->pre2 = phymod_tx.pre2;
            tx->pre = phymod_tx.pre;
            tx->main = phymod_tx.main & 0xFF;
            tx->post = phymod_tx.post;
            tx->post2 = phymod_tx.post2;
            tx->post3 = phymod_tx.post3;
            tx->tx_tap_mode = (phymod_tx.txTapMode == phymodTxTapMode3Tap)? bcmPortPhyTxTapMode3Tap : bcmPortPhyTxTapMode6Tap;
            tx->signalling_mode = (phymod_tx.signallingMode == phymodSignallingMethodNRZ)? bcmPortPhySignallingModeNRZ : bcmPortPhySignallingModePAM4;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_reg_get(int unit, soc_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 *phy_data)
{
    uint16 phy_rd_data = 0;
    uint32 reg_flag, phy_id, phy_reg;
    int    rv;
    int nof_phys_structs = 0;
    phymod_phy_access_t phy_access[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    portmod_access_get_params_t params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if(flags & SOC_PHY_INTERNAL){
        params.phyn = 0;
    }

    if ((flags & SOC_PHY_NOMAP) == 0){
        rv = portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phy_access, &nof_phys_structs, NULL);
        SHR_IF_ERR_EXIT(rv);
    }

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16)) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags for ramon");
    } 

    reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & SOC_PHY_REG_INDIRECT) {
        if (flags & BCM_PORT_PHY_NOMAP) {
            /* Indirect register access is performed through PHY driver.
             * Therefore, indirect register access is not supported if
             * BCM_PORT_PHY_NOMAP flag is set.
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "indirect register access is not supported if SOC_PHY_NOMAP flag is set");
        }
        phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
        rv = phymod_phy_reg_read(phy_access, phy_reg_addr, phy_data);
        SHR_IF_ERR_EXIT(rv);
    } else {
        if (flags & SOC_PHY_NOMAP) {
            phy_id = port;
            phy_reg = phy_reg_addr;
            rv = soc_cmicx_miim_operation(unit, 0 /* is_write */, 45, phy_id, phy_reg, &phy_rd_data);
            SHR_IF_ERR_EXIT(rv);
            *phy_data = phy_rd_data;
        } else{
            PHYMOD_LOCK_TAKE(phy_access);
            rv = PHYMOD_BUS_READ(&phy_access[0].access, phy_reg_addr, phy_data);
            PHYMOD_LOCK_GIVE(phy_access);
            SHR_IF_ERR_EXIT(rv);
        }
    }
    

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_reg_set(int unit, soc_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data)
{ 
    uint16 phy_wr_data;
    uint32 reg_flag, phy_id, phy_reg;
    int    rv;
    int nof_phys_structs = 0;
    phymod_phy_access_t phy_access[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    portmod_access_get_params_t params;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    if(flags & SOC_PHY_INTERNAL){
        params.phyn = 0;
    }

    if ((flags & SOC_PHY_NOMAP) == 0){
        rv = portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phy_access, &nof_phys_structs, NULL);
        SHR_IF_ERR_EXIT(rv);
    }

    if (flags & (SOC_PHY_I2C_DATA8 | SOC_PHY_I2C_DATA16)) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported flags for ramon");
    } 

    reg_flag = SOC_PHY_REG_FLAGS(phy_reg_addr);
    if (reg_flag & SOC_PHY_REG_INDIRECT) {
        if (flags & BCM_PORT_PHY_NOMAP) {
            /* Indirect register access is performed through PHY driver.
             * Therefore, indirect register access is not supported if
             * SOC_PHY_NOMAP flag is set.
             */
            SHR_ERR_EXIT(_SHR_E_PARAM, "indirect register access is not supported if SOC_PHY_NOMAP flag is set");
        }
        phy_reg_addr &= ~SOC_PHY_REG_INDIRECT;
        rv = phymod_phy_reg_write(phy_access, phy_reg_addr, phy_data);
        SHR_IF_ERR_EXIT(rv);
    } else {
        if (flags & BCM_PORT_PHY_NOMAP) {
            phy_id = port;
            phy_reg = phy_reg_addr;
            phy_wr_data = phy_data & 0xffff;
            rv = soc_cmicx_miim_operation(unit, 1 /* is_write */, 45, phy_id, phy_reg, &phy_wr_data);
            SHR_IF_ERR_EXIT(rv);
        } else{
            PHYMOD_LOCK_TAKE(phy_access);
            rv = PHYMOD_BUS_WRITE(&phy_access[0].access, phy_reg_addr, phy_data);
            PHYMOD_LOCK_GIVE(phy_access);
            SHR_IF_ERR_EXIT(rv);
        }
    }
    

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_phy_reg_modify(int unit, soc_port_t port, uint32 flags,
                 uint32 phy_reg_addr, uint32 phy_data, uint32 phy_mask)
{
    uint32 phy_rd_data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_reg_get(unit, port, flags, phy_reg_addr, &phy_rd_data));
    phy_data |= (phy_rd_data & ~phy_mask);
    SHR_IF_ERR_EXIT(soc_dnxc_port_phy_reg_set(unit, port, flags, phy_reg_addr, phy_data));

exit:
    SHR_FUNC_EXIT;
}

/****************************************************************************** 
 Fabric Port Controls
*******************************************************************************/

shr_error_e 
soc_dnxc_port_control_pcs_set(int unit, soc_port_t port, soc_dnxc_port_pcs_t pcs)
{
    uint32 properties = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (pcs == soc_dnxc_port_pcs_64_66_fec)
    {
        PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_SET(properties); /*CIG From llfc is enabled by default*/
    } else if (pcs == soc_dnxc_port_pcs_64_66_rs_fec || pcs == soc_dnxc_port_pcs_64_66_ll_rs_fec
               || pcs == soc_dnxc_port_pcs_64_66_15t_rs_fec || pcs == soc_dnxc_port_pcs_64_66_15t_ll_rs_fec)
    {
        PORTMOD_ENCODING_LOW_LATENCY_LLFC_SET(properties);
    }
    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, (_shr_port_pcs_t) pcs));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_control_pcs_get(int unit, soc_port_t port, soc_dnxc_port_pcs_t* pcs)
{
    uint32 properties = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, (portmod_port_pcs_t *) pcs));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_low_latency_llfc_set(int unit, soc_port_t port, int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));

    if ((encoding != PORTMOD_PCS_64B66B_FEC)
        && (encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)
        && (encoding != PORTMOD_PCS_64B66B_RS_FEC)
        && (encoding != PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC)
        && (encoding != PORTMOD_PCS_64B66B_15T_RS_FEC))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Low latency LLFC control is supported only by KR FEC and RS FECs");
    }
    if(value){
        properties |= PORTMOD_ENCODING_LOW_LATENCY_LLFC;
    } else{
        properties &= ~PORTMOD_ENCODING_LOW_LATENCY_LLFC;
    }
    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_low_latency_llfc_get(int unit, soc_port_t port, int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if ((encoding != PORTMOD_PCS_64B66B_FEC)
        && (encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC)
        && (encoding != PORTMOD_PCS_64B66B_RS_FEC)
        && (encoding != PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC)
        && (encoding != PORTMOD_PCS_64B66B_15T_RS_FEC))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Low latency LLFC control is supported only by KR FEC and RS FECs");
    }
    *value = PORTMOD_ENCODING_LOW_LATENCY_LLFC_GET(properties);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_fec_error_detect_set(int unit, soc_port_t port, int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC && encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_RS_FEC
            && encoding != PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_15T_RS_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Error detect control is supported only by KR FEC and RS FECs");
    }

    if(value){
        properties |= PORTMOD_ENCODING_FEC_ERROR_DETECT;
    } else{
        properties &= ~PORTMOD_ENCODING_FEC_ERROR_DETECT;
    }

    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_fec_error_detect_get(int unit, soc_port_t port, int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC && encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_RS_FEC
            && encoding != PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_15T_RS_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Error detect control is supported only by KR FEC and RS FECs");
    }

    *value = PORTMOD_ENCODING_FEC_ERROR_DETECT_GET(properties);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnxc_port_control_llfc_after_fec_enable_set(int unit, soc_port_t port, int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_RS_FEC
            && encoding != PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_15T_RS_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "LLFC after FEC control is supported only by RS FECs");
    }

    if(value){
        properties |= PORTMOD_ENCODING_LLFC_AFTER_FEC;
    } else{
        properties &= ~PORTMOD_ENCODING_LLFC_AFTER_FEC;
    }

    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_control_llfc_after_fec_enable_get(int unit, soc_port_t port, int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_RS_FEC
            && encoding != PORTMOD_PCS_64B66B_15T_LOW_LATENCY_RS_FEC && encoding != PORTMOD_PCS_64B66B_15T_RS_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "LLFC after FEC control is supported only by RS FECs");
    }

    *value = PORTMOD_ENCODING_LLFC_AFTER_FEC_GET(properties);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_dnxc_port_control_power_set(int unit, soc_port_t port, uint32 flags, soc_dnxc_port_power_t power)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, soc_dnxc_port_power_on == power ? 1 : 0));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_control_power_get(int unit, soc_port_t port, soc_dnxc_port_power_t* power)
{
    int enable;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, &enable));
    *power =  enable ? soc_dnxc_port_power_on : soc_dnxc_port_power_off;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_control_rx_enable_set(int unit, soc_port_t port, uint32 flags, int enable)
{
    /*Maybe has to set PORTMOD_PORT_ENABLE_MAC flag*/
    int portmod_enable_flags = PORTMOD_PORT_ENABLE_RX;
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

    if(flags & SOC_DNXC_PORT_CONTROL_FLAGS_RX_SERDES_IGNORE){
        portmod_enable_flags |= PORTMOD_PORT_ENABLE_MAC ;
    }
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, portmod_enable_flags, enable));

exit:
    SHR_FUNC_EXIT; 
}

shr_error_e 
soc_dnxc_port_control_tx_enable_set(int unit, soc_port_t port, int enable)
{
    int portmod_enable_flags = PORTMOD_PORT_ENABLE_TX;
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, portmod_enable_flags, enable));
exit:
    SHR_FUNC_EXIT; 
}

shr_error_e 
soc_dnxc_port_control_rx_enable_get(int unit, soc_port_t port, int* enable)
{
    int portmod_enable_flags = PORTMOD_PORT_ENABLE_RX | PORTMOD_PORT_ENABLE_MAC;
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, portmod_enable_flags, enable));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_control_tx_enable_get(int unit, soc_port_t port, int* enable)
{
    int portmod_enable_flags = PORTMOD_PORT_ENABLE_TX | PORTMOD_PORT_ENABLE_MAC;
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, portmod_enable_flags, enable));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_prbs_tx_enable_set(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);

    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_set(unit, port, portmod_mode, flags, value));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_dnxc_port_prbs_rx_enable_set(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);

    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_set(unit, port, portmod_mode, flags, value));

exit:
    SHR_FUNC_EXIT;
    
}

shr_error_e 
soc_dnxc_port_prbs_tx_enable_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int *value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);

    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_get(unit, port, portmod_mode, flags, value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_prbs_rx_enable_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int *value)
{
    int flags = 0;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);

    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_RX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_enable_get(unit, port, portmod_mode, flags, value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_prbs_rx_status_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int* value)
{
    phymod_prbs_status_t status;
    int flags = PHYMOD_PRBS_STATUS_F_CLEAR_ON_READ;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);

    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    SHR_IF_ERR_EXIT(phymod_prbs_status_t_init(&status));
    SHR_IF_ERR_EXIT(portmod_port_prbs_status_get(unit, port, portmod_mode, flags, &status));
    if(status.prbs_lock_loss) {
        *value = -2;
    } else if(!status.prbs_lock) {
        *value = -1;
    } else {
        *value = status.error_count;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_prbs_tx_invert_data_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int *invert)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);

    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    *invert = config.invert ? 1: 0;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_dnxc_port_prbs_tx_invert_data_set(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int invert)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);

    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    PHYMOD_PRBS_DIRECTION_TX_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    config.invert = invert ? 1: 0;
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_set(unit, port, portmod_mode, flags, &config));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_dnxc_port_prbs_polynomial_set(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int value)
{
    int flags = 0;
    phymod_prbs_poly_t poly;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);

    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    SHR_IF_ERR_EXIT(soc_prbs_poly_to_phymod(value, &poly));
    config.poly = poly;
    SHR_IF_ERR_EXIT(portmod_port_prbs_config_set(unit, port, portmod_mode, flags, &config));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_prbs_polynomial_get(int unit, soc_port_t port, soc_dnxc_port_prbs_mode_t mode, int *value)
{
    int flags = 0;
    phymod_prbs_t config;
    portmod_prbs_mode_t portmod_mode;
    SHR_FUNC_INIT_VARS(unit);

    portmod_mode = (mode == soc_dnxc_port_prbs_mode_phy ? portmodPrbsModePhy : portmodPrbsModeMac);

    SHR_IF_ERR_EXIT(portmod_port_prbs_config_get(unit, port, portmod_mode, flags, &config));
    SHR_IF_ERR_EXIT(phymod_prbs_poly_to_soc(config.poly, (uint32*) value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t loopback)
{
    portmod_loopback_mode_t current_lb = portmodLoopbackCount; /* no loopback */
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxc_port_loopback_get(unit, port, &current_lb));
    if(current_lb == loopback){
        /*nothing to do*/
        SHR_EXIT();
    }
    if(current_lb != portmodLoopbackCount){
        /*open existing loopback*/
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, current_lb, 0));
    }
    if(loopback != portmodLoopbackCount){
        /*define the new loopback*/
        SHR_IF_ERR_EXIT(portmod_port_loopback_set(unit, port, loopback, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t* loopback)
{
    portmod_loopback_mode_t supported_lb_modes[] = { portmodLoopbackMacAsyncFifo, portmodLoopbackMacOuter, portmodLoopbackMacPCS, portmodLoopbackMacRloop,
        portmodLoopbackPhyGloopPMD,portmodLoopbackPhyGloopPCS,portmodLoopbackPhyRloopPMD};
    int i = 0, rv;
    int enable = 0;
    SHR_FUNC_INIT_VARS(unit);

    *loopback = portmodLoopbackCount; /* no loopback */
    for(i = 0 ; i < COUNTOF(supported_lb_modes); i++){

        if(IS_SFI_PORT(unit, port) || IS_IL_PORT(unit, port)) {
            if(supported_lb_modes[i] == portmodLoopbackPhyGloopPCS) {
                continue;
            }
        }
        rv = portmod_port_loopback_get(unit, port, supported_lb_modes[i], &enable);
        if(rv == _SHR_E_UNAVAIL) {
            /* loopback type is not supported for the PM */
            continue; 
        }
        SHR_IF_ERR_EXIT(rv);
        if(enable){
            *loopback = supported_lb_modes[i];
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_enable_set(int unit, soc_port_t port, int enable)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, enable? 1 : 0));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_enable_get(int unit, soc_port_t port, int *enable)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, port, 0, enable));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_dnxc_port_rx_locked_get(int unit, soc_port_t port, uint32 *rx_locked)
{
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    portmod_access_get_params_t params;
    SHR_FUNC_INIT_VARS(unit);

    portmod_access_get_params_t_init(unit, &params);
    params.lane = -1;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;
    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));

    SHR_IF_ERR_EXIT(phymod_phy_rx_pmd_locked_get(phys, rx_locked));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_dnxc_port_extract_cig_from_llfc_enable_set(int unit, soc_port_t port, int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Extract Congestion Ind from LLFC cells control is supported only by KR_FEC");
    }
    if(value){
        properties |= PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC;
    } else{
        properties &= ~PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC;
    }
    SHR_IF_ERR_EXIT(portmod_port_encoding_set(unit, port, properties, encoding));
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_dnxc_port_extract_cig_from_llfc_enable_get(int unit, soc_port_t port, int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Extract Congestion Ind from LLFC cells control is supported only by KR_FEC");
    }
    *value = PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties);
exit:
    SHR_FUNC_EXIT;
}


#endif /*PORTMOD_SUPPORT*/

#undef BSL_LOG_MODULE

