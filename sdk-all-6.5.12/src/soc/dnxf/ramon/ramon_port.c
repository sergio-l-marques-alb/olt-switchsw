/*
 * $Id: ramon_port.c,v 1.37.20.1 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC RAMON PORT
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif
 
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT
#include <shared/bsl.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/dnxf/ramon/ramon_port.h>
#include <soc/dnxf/ramon/ramon_drv.h>
#include <soc/dnxf/ramon/ramon_defs.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_port.h>

#include <soc/dnxf/cmn/dnxf_config_defs.h>
#include <soc/dnxf/cmn/dnxf_config_imp_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_warm_boot.h>
#include <soc/dnxf/cmn/dnxf_port.h>

#include <soc/dnxc/legacy/dnxc_port.h>
#include <soc/dnxc/legacy/dnxc_dev_feature_manager.h>

#include <soc/portmod/portmod.h>
#include <phymod/phymod_acc.h>
#include <soc/phy/phymod_port_control.h>
#include <soc/cmicx_miim.h>

extern unsigned char  blackhawk_ucode[];
extern unsigned int   blackhawk_ucode_len;

/*
 * Function:
 *      soc_ramon_port_soc_init
 * Purpose:
 *      Initalize port module - read the PLL and config the comma burst
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Returns:
 *      _SHR_E_NONE     No Error  
 *      _SHR_E_UNAVAIL  Feature unavailable  
 *      _SHR_E_XXX      Error occurred  
 */
shr_error_e 
soc_ramon_port_soc_init(int unit)
{

    soc_reg_above_64_val_t puc;
    uint32 puc_lcpll_in_sel=0;
    uint32 puc_lcpll_out_sel=0;
    soc_port_t port_i;
    uint32 lcpll;
    int sync_e_master_port, sync_e_slave_port, sync_e_master_divider; 

    SHR_FUNC_INIT_VARS(unit);

    /*Check if PLL configuration is possible*/
    SHR_IF_ERR_EXIT( soc_reg_above_64_get(unit, ECI_POWERUP_CONFIGr, REG_PORT_ANY, 0, puc) );
    SHR_BITCOPY_RANGE(&puc_lcpll_in_sel, 0, puc, SOC_RAMON_PORT_PUC_LCPLL_IN_SEL, SOC_RAMON_PORT_PUC_LCPLL_IN_SEL_NOF_BITS);

    /*set port refclk*/
    for (port_i = 0; port_i < SOC_RAMON_NOF_LINKS ; ++port_i) {
        lcpll = INT_DEVIDE(port_i, SOC_RAMON_NOF_PORT_IN_LCPLL);

        if( (puc_lcpll_in_sel ==  SOC_RAMON_PORT_PUC_LCPLL_IN_DISABLE_POWERUP) || SAL_BOOT_PLISIM ) {

            switch(dnxf_data_port.pll.info_get(unit, lcpll)->fabric_ref_clk_out)
            {
            case soc_dnxc_init_serdes_ref_clock_312_5:
                SOC_INFO(unit).port_refclk_int[port_i] = 312;
                break;

            /* Bypass mode */
            case soc_dnxc_init_serdes_ref_clock_bypass:

                switch(dnxf_data_port.pll.info_get(unit, lcpll)->fabric_ref_clk_in)
                {
                case soc_dnxc_init_serdes_ref_clock_125:
                      SOC_INFO(unit).port_refclk_int[port_i] = 125;
                      break;

                case soc_dnxc_init_serdes_ref_clock_156_25:
                      SOC_INFO(unit).port_refclk_int[port_i] = 156;
                      break;

                case soc_dnxc_init_serdes_ref_clock_312_5:
                      SOC_INFO(unit).port_refclk_int[port_i] = 312;
                      break;

                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized LC-PLL value");
                    break;
                }

                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized LC-PLL value");
                break;
            }

        } else {

            /* LCPLL01 */
            switch(lcpll) {
            case 0:
            case 1:

                SHR_BITCOPY_RANGE(&puc_lcpll_out_sel, 0, puc, SOC_RAMON_PORT_PUC_LCPLL01_OUT_SEL, SOC_RAMON_PORT_PUC_LCPLL01_OUT_SEL_NOF_BITS);

                break;

            /* LCPLL23 */
            case 2:
            case 3:

                SHR_BITCOPY_RANGE(&puc_lcpll_out_sel, 0, puc, SOC_RAMON_PORT_PUC_LCPLL23_OUT_SEL, SOC_RAMON_PORT_PUC_LCPLL23_OUT_SEL_NOF_BITS);

                break;
            }

            if (puc_lcpll_out_sel == SOC_RAMON_PORT_PUC_LCPLL_OUT_312_5_MHZ)
            {
                SOC_INFO(unit).port_refclk_int[port_i] = 312;
            }
            else if (puc_lcpll_out_sel == SOC_RAMON_PORT_PUC_LCPLL_OUT_156_25_MHZ)
            {
                SOC_INFO(unit).port_refclk_int[port_i] = 156;
            }
            else if (puc_lcpll_out_sel == SOC_RAMON_PORT_PUC_LCPLL_OUT_125_MHZ)
            {
                SOC_INFO(unit).port_refclk_int[port_i] = 125;
            }
            else
            {
                /* Bypass mode */
                if (puc_lcpll_in_sel == SOC_RAMON_PORT_PUC_LCPLL_IN_312_5_MHZ)
                {
                    SOC_INFO(unit).port_refclk_int[port_i] = 312;
                }
                else if (puc_lcpll_in_sel == SOC_RAMON_PORT_PUC_LCPLL_IN_156_25_MHZ)
                {
                    SOC_INFO(unit).port_refclk_int[port_i] = 156;
                }
                else if (puc_lcpll_in_sel == SOC_RAMON_PORT_PUC_LCPLL_IN_125_MHZ)
                {
                    SOC_INFO(unit).port_refclk_int[port_i] = 125;
                }
            }
        }
    }

    if(!SOC_WARM_BOOT(unit)) {

        sync_e_master_port = dnxf_data_port.synce.cfg_get(unit, 0)->source_clock_port;
        sync_e_slave_port  = dnxf_data_port.synce.cfg_get(unit, 1)->source_clock_port;
    
        if (sync_e_master_port != SOC_DNXF_PROPERTY_UNAVAIL)
        {
            SHR_IF_ERR_EXIT(soc_ramon_port_sync_e_link_set(unit, 1, sync_e_master_port));
        }
    
        if (sync_e_slave_port != SOC_DNXF_PROPERTY_UNAVAIL)
        {
            SHR_IF_ERR_EXIT(soc_ramon_port_sync_e_link_set(unit, 0, sync_e_slave_port));
        }

        sync_e_master_divider =  dnxf_data_port.synce.cfg_get(unit, 0)->source_clock_divider;

        SHR_IF_ERR_EXIT(soc_ramon_port_sync_e_divider_set(unit, sync_e_master_divider));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_sync_e_link_set(int unit, int is_master, int port)
{
    int fsrd_block;
    uint32 reg_val,reg_val2; 
    SHR_FUNC_INIT_VARS(unit);

    /* input validation */
    SHR_IF_ERR_EXIT( MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_links_validate_link, (unit, port)) );

    fsrd_block = port/dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);

    SHR_IF_ERR_EXIT(READ_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, &reg_val));
    SHR_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg_val2));

    if (is_master)
    {
        /*update eci register*/
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, FABRIC_MASTER_SYNC_E_SELf, fsrd_block);
        /*update fsrd register*/    
        soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg_val2, MASTER_CLK_SELf,  port % dnxf_data_device.blocks.nof_links_in_fsrd_get(unit));

    }
    else
    {
        /*update eci register*/
        soc_reg_field_set(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, &reg_val, FABRIC_SLAVE_SYNC_E_SELf, fsrd_block);
        /*update fsrd register*/
        soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg_val2, SLAVE_CLK_SELf,  port % dnxf_data_device.blocks.nof_links_in_fsrd_get(unit));
    }
    SHR_IF_ERR_EXIT(WRITE_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, reg_val));
    SHR_IF_ERR_EXIT(WRITE_FSRD_SYNC_E_SELECTr(unit, fsrd_block, reg_val2));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_sync_e_divider_set(int unit, int divider)
{
    uint32 reg_val;
    int phase1_divider, fsrd_idx;
    int is_enabled;
    soc_info_t *si;

    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);

    if (!((divider >= SOC_RAMON_PORT_SYNC_E_MIN_DIVIDER) && (divider <= SOC_RAMON_PORT_SYNC_E_MAX_DIVIDER)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid divider for syncE configuration, must be between 2 to 16");
    }

    phase1_divider = divider-1;
 
    /* update divider for all fsrd blocks */
    for (fsrd_idx = 0 ; fsrd_idx < dnxf_data_device.blocks.nof_instances_fsrd_get(unit); fsrd_idx++)
    {
        /*Check if FSRD block is enabled*/
        is_enabled = si->block_valid[fsrd_idx];

        if (is_enabled)
        {
            SHR_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_idx, &reg_val));
            soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg_val, SYNC_E_CLK_DIV_PHASE_1f, phase1_divider);
            soc_reg_field_set(unit, FSRD_SYNC_E_SELECTr, &reg_val, SYNC_E_CLK_DIV_PHASE_0f, phase1_divider/2);
            SHR_IF_ERR_EXIT(WRITE_FSRD_SYNC_E_SELECTr(unit, fsrd_idx, reg_val));
        }
    }

exit:
    SHR_FUNC_EXIT;    
}

shr_error_e
soc_ramon_port_sync_e_link_get(int unit, int is_master, int *port)
{
    uint32 reg_val, reg_val2; 
    int fsrd_block;
    int inner_fsrd_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, &reg_val));

    if (is_master)
    {
        fsrd_block = soc_reg_field_get(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, reg_val, FABRIC_MASTER_SYNC_E_SELf);  
        SHR_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg_val2));  
        inner_fsrd_port = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg_val2, MASTER_CLK_SELf);   
    }
    else
    {
        fsrd_block = soc_reg_field_get(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, reg_val, FABRIC_SLAVE_SYNC_E_SELf); 
        SHR_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg_val2));  
        inner_fsrd_port = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg_val2, SLAVE_CLK_SELf);   
    }

    *port = fsrd_block*dnxf_data_device.blocks.nof_links_in_fsrd_get(unit) + inner_fsrd_port;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_sync_e_divider_get(int unit, int *divider)
{
    uint32 reg_val, reg_val2;
    int fsrd_block;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr(unit, &reg_val));
    fsrd_block = soc_reg_field_get(unit, ECI_SELECT_OUTPUT_OF_SYNCHRONOUS_ETHERNET_PADSr, reg_val, FABRIC_MASTER_SYNC_E_SELf);
    SHR_IF_ERR_EXIT(READ_FSRD_SYNC_E_SELECTr(unit, fsrd_block, &reg_val2));
    *divider = soc_reg_field_get(unit, FSRD_SYNC_E_SELECTr, reg_val2, SYNC_E_CLK_DIV_PHASE_1f) + 1; /*phase1 is configured as (divider-1)*/

exit:
    SHR_FUNC_EXIT;    

}

#define RAMON_NOF_CORES_IN_RING 6

STATIC
int soc_ramon_core_address_get(int unit, int core_index, uint16 *phy_addr){
    uint16 bus_id = 0;
    uint16 phy_id = 0;

/*
MIIM ring    Port address
             (phy address)    Logical hierarchy / notes    Links
------------------------------------------------------------------ 
------------------------------------------------------------------ 
  0               0                 PCIES                 PCIES 0-2
------------------------------------------------------------------
  1               0                 MAS0 BLHK0            0  -7  
                  1                 MAS0 BLHK1            8  -15 
                  2                 MAS0 BLHK2            16 -23 
                  3                 MAS0 BLHK3            24 -31 
                  4                 MAS0 BLHK4            32 -39 
                  5                 MAS0 BLHK5            40 -47 
                  31                broadcast to MIIM 1   Fabric 0-47
------------------------------------------------------------------  
  2               0                 MAS1 BLHK0            48 -55 
                  1                 MAS1 BLHK1            56 -63 
                  2                 MAS1 BLHK2            64 -71 
                  3                 MAS1 BLHK3            72 -79 
                  4                 MAS1 BLHK4            80 -87 
                  5                 MAS1 BLHK5            88 -95 
                  31                broadcast to MIIM 2   48-95
------------------------------------------------------------------ 
  3               0                 MAS2 BLHK0            96 -103
                  1                 MAS2 BLHK1            104-111
                  2                 MAS2 BLHK2            112-119
                  3                 MAS2 BLHK3            120-127
                  4                 MAS2 BLHK4            128-135
                  5                 MAS2 BLHK5            136-143
                  31                broadcast to MIIM 3   96-143
------------------------------------------------------------------  
  4               0                 MAS3 BLHK0            144-151
                  1                 MAS3 BLHK1            152-159
                  2                 MAS3 BLHK2            160-167
                  3                 MAS3 BLHK3            168-175
                  4                 MAS3 BLHK4            176-183
                  5                 MAS3 BLHK5            184-191
                  31                broadcast to MIIM 4   144-191
*/

    bus_id = (core_index / RAMON_NOF_CORES_IN_RING) + 1 /* +1 as ring 0 is for PCIe */ ; 
    phy_id = core_index % RAMON_NOF_CORES_IN_RING;

    /*encode for MIIM format:
      bits 0-4 for the "port" address 
      bits 5-6 and 8-9 for bus
      bit 7 for internal/external */
    /*0x80 for internal port*/
    *phy_addr = 0x80 | ((bus_id & 0x3)<<PHY_ID_BUS_LOWER_SHIFT) | ((bus_id & 0xc)<<PHY_ID_BUS_UPPER_SHIFT) | phy_id;

    return _SHR_E_NONE;
}

shr_error_e
soc_ramon_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence)
{

    int rc = _SHR_E_NONE, is_first_link, i, fsrd_block, fmac_block;
    soc_port_t port;
    dnxc_port_fabric_init_config_t port_config;
    phymod_firmware_load_method_t fw_load_method = phymodFirmwareLoadMethodNone;
    int broadcast_load = 0;
    dnxc_port_init_stage_t stage;
    int fw_verify = 0;
    int is_lane_mapped;
    int pm_index;
    SHR_FUNC_INIT_VARS(unit);

    if (is_init_sequence) {
        fw_load_method = dnxf_data_port.general.fw_load_method_get(unit);
        if (!SOC_IS_RELOADING(unit)) 
        {
            fw_verify = dnxf_data_port.general.fw_load_verify_get(unit);
        }



        /*broadcast load is supported just for extrnal method and not reduced links SKUs*/
        if(fw_load_method == phymodFirmwareLoadMethodExternal) {
             broadcast_load = 1;
        }

    } else {
        fw_load_method = phymodFirmwareLoadMethodInternal;
        broadcast_load = 0;
        fw_verify = 1;
    }

    /* when adding a port to be active need to make sure it is lane-mapped */
    SOC_PBMP_ITER(pbmp, port) {
        rc = soc_dnxf_lane_map_db_is_lane_mapped_get(unit, port, &is_lane_mapped);
        SHR_IF_ERR_EXIT(rc);
        if(!is_lane_mapped)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "port %d: an active port must be lane-mapped", port);
        }
    }

    /*Initial port speed max*/
    SOC_PBMP_ITER(pbmp, port) {
        /* Check if it is a first link in relevant fmac block , if so, turn it on in ECI register */
        rc = soc_ramon_port_first_link_in_fsrd_get(unit, port , &is_first_link);
        SHR_IF_ERR_EXIT(rc);

        if (is_first_link == 1)
        {
            if (!SOC_WARM_BOOT(unit))
            {
                rc = soc_ramon_port_update_fsrd_block(unit, port, 1);
                SHR_IF_ERR_EXIT(rc);
            }

            fsrd_block = port/dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);

            fmac_block = fsrd_block * dnxf_data_device.blocks.nof_fmacs_in_fsrd_get(unit);

            rc = soc_ramon_port_update_valid_block_database(unit, SOC_BLK_FSRD, fsrd_block, 1);
            SHR_IF_ERR_EXIT(rc);

            for (i = fmac_block; i < fmac_block +  dnxf_data_device.blocks.nof_fmacs_in_fsrd_get(unit) ; i++)
            {
                rc = soc_ramon_port_update_valid_block_database(unit, SOC_BLK_FMAC, i, 1);
                SHR_IF_ERR_EXIT(rc);
            }
        }

        if (!is_init_sequence)
        {
            rc = soc_ramon_port_dynamic_soc_init(unit, port, is_first_link);
            SHR_IF_ERR_EXIT(rc);
        }

        /* Get lane-mapping relevant to all lanes in the pm of the port: lane -> {serdes rx, serdes tx}.
         * Unmapped lanes defined by user will be filled with unused serdeses- so that all lanes are mapped,
         * in order to configure FMAC and phymod without holes */
        pm_index = port / DNXC_PORT_FABRIC_LANES_PER_CORE;
        SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_pm_map_get(unit, pm_index, DNXC_PORT_FABRIC_LANES_PER_CORE, port_config.lane2serdes));

        stage = broadcast_load ? dnxc_port_init_pass1 : dnxc_port_init_full;

        rc= soc_dnxc_fabric_port_probe(unit, port, stage, fw_verify, &port_config);
        SHR_IF_ERR_EXIT(rc);
    }

    if(!SOC_WARM_BOOT(unit) && broadcast_load)
    {
        rc = soc_dnxc_fabric_broadcast_firmware_loader(unit, blackhawk_ucode_len, blackhawk_ucode);
        SHR_IF_ERR_EXIT(rc);
    }
        
    SOC_PBMP_ITER(pbmp, port) {

        if(broadcast_load)
        {
            /* Get lane-mapping relevant to all lanes in the pm of the port: lane -> {serdes rx, serdes tx}.
             * Unmapped lanes defined by user will be filled with unused serdeses- so that all lanes are mapped,
             * in order to configure FMAC and phymod without holes */
            pm_index = port / DNXC_PORT_FABRIC_LANES_PER_CORE;
            SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_pm_map_get(unit, pm_index, DNXC_PORT_FABRIC_LANES_PER_CORE, port_config.lane2serdes));

            rc= soc_dnxc_fabric_port_probe(unit, port, dnxc_port_init_pass2, fw_verify, &port_config);
            SHR_IF_ERR_EXIT(rc);
        }

        SOC_PBMP_PORT_ADD(*okay_pbmp, port);

        /* Update relevant bitmaps - add to enabled bitmaps, remove from disabled bitmaps.
         * Only when port is dynamic- bitmaps are changed by below code. For static port bitmaps remain the same.*/
        rc =  MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_ADD, soc_dnxf_port_update_type_sfi));
        SHR_IF_ERR_EXIT(rc);

        rc =  MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_ADD, soc_dnxf_port_update_type_port));            
        SHR_IF_ERR_EXIT(rc);

        rc =  MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_ADD, soc_dnxf_port_update_type_all));            
        SHR_IF_ERR_EXIT(rc);

        rc =  MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_REMOVE, soc_dnxf_port_update_type_sfi_disabled));            
        SHR_IF_ERR_EXIT(rc);

        rc =  MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_REMOVE, soc_dnxf_port_update_type_port_disabled));            
        SHR_IF_ERR_EXIT(rc);

        rc =  MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_REMOVE, soc_dnxf_port_update_type_all_disabled));            
        SHR_IF_ERR_EXIT(rc); 
    }

exit:
    SHR_FUNC_EXIT; 
}

shr_error_e
soc_ramon_port_update_fsrd_block(int unit, soc_port_t port, int enable)
{
    int fsrd_block;
    uint32 field_val;
    uint64 reg_val64;
    const soc_field_t fields[] = {BLOCKS_POWER_DOWN_0f, BLOCKS_POWER_DOWN_1f, BLOCKS_POWER_DOWN_2f, BLOCKS_POWER_DOWN_3f, BLOCKS_POWER_DOWN_4f, BLOCKS_POWER_DOWN_5f,
            BLOCKS_POWER_DOWN_6f, BLOCKS_POWER_DOWN_7f, BLOCKS_POWER_DOWN_8f, BLOCKS_POWER_DOWN_9f, BLOCKS_POWER_DOWN_10f, BLOCKS_POWER_DOWN_11f,
            BLOCKS_POWER_DOWN_12f, BLOCKS_POWER_DOWN_13f, BLOCKS_POWER_DOWN_14f, BLOCKS_POWER_DOWN_15f, BLOCKS_POWER_DOWN_16f, BLOCKS_POWER_DOWN_17f,
            BLOCKS_POWER_DOWN_18f, BLOCKS_POWER_DOWN_19f, BLOCKS_POWER_DOWN_20f, BLOCKS_POWER_DOWN_21f, BLOCKS_POWER_DOWN_22f, BLOCKS_POWER_DOWN_23f};
    SHR_FUNC_INIT_VARS(unit);

    fsrd_block =  port/dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);

    if (fsrd_block < 0 || fsrd_block >= dnxf_data_device.blocks.nof_instances_fsrd_get(unit)) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid fsrd block");
    }

    SHR_IF_ERR_EXIT(READ_ECI_BLOCKS_POWER_DOWNr(unit, &reg_val64));

    field_val = enable? 0: 1;
    soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, fields[fsrd_block], field_val);

    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_POWER_DOWNr(unit, reg_val64));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_first_link_in_fsrd_get(int unit, soc_port_t port, int *is_first_link)
{
    int fsrd_block, i;
    int counter_mask_fsrd_bmp = 0;
    pbmp_t mask_fsrd_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SOC_PBMP_CLEAR(mask_fsrd_bmp);

    fsrd_block = port/dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);

    for (i=fsrd_block*dnxf_data_device.blocks.nof_links_in_fsrd_get(unit) ; i<fsrd_block*dnxf_data_device.blocks.nof_links_in_fsrd_get(unit) + dnxf_data_device.blocks.nof_links_in_fsrd_get(unit); i++) /* create a mask for all the links in the fsrd*/
    {
        SOC_PBMP_PORT_ADD(mask_fsrd_bmp, i);
    }

    SOC_PBMP_AND(mask_fsrd_bmp, PBMP_SFI_ALL(unit)); /* mask the ports the relevant fmac */

    SOC_PBMP_COUNT(mask_fsrd_bmp, counter_mask_fsrd_bmp);

    /* Count the number of links activated in that bmp */

    if (counter_mask_fsrd_bmp==0) /* all the ports in the fmac are off, and we want to turn on a port */
    {
        *is_first_link = 1;
    }
    else
    {
        *is_first_link = 0;
    }

    SHR_FUNC_EXIT;
}


/*
 * Function:
 *      soc_ramon_dynamic_port_restore
 * Purpose:
 *      Restore SFI bitmap from portmod WB
 * Parameters:
 *      unit  - (IN)     Unit number.
 * Comments:
 *      Should be called after portmod initiation
 */
STATIC shr_error_e
soc_ramon_dynamic_port_restore(int unit)
{
    int rc;
    int port, valid;
    int enabled_pbmp_control, disabled_pbmp_control;
    SHR_FUNC_INIT_VARS(unit);

    for (port = 0; port < dnxf_data_port.general.nof_links_get(unit) ; port++) {

        SHR_IF_ERR_EXIT(portmod_port_is_valid(unit, port, &valid));
        /* 
         * IF Port is valid - add to enabled bmp and remove from disabled bitmap.
         * IF Port is invalid - remove from enabled bmp and add to disabled bitmap.
         */
        if (valid)
        {
            enabled_pbmp_control =  SOC_DNXF_PBMP_PORT_ADD;
            disabled_pbmp_control = SOC_DNXF_PBMP_PORT_REMOVE;
        } else {
            enabled_pbmp_control =  SOC_DNXF_PBMP_PORT_REMOVE;
            disabled_pbmp_control = SOC_DNXF_PBMP_PORT_ADD;
        }

        /* enabled bmp*/
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_sfi));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_port));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, enabled_pbmp_control, soc_dnxf_port_update_type_all));
        SHR_IF_ERR_EXIT(rc);

        /* disabled bmp */
        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_sfi_disabled));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_port_disabled));
        SHR_IF_ERR_EXIT(rc);

        rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, disabled_pbmp_control, soc_dnxf_port_update_type_all_disabled));
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_port_init(int unit)
{
    int rc, nof_links, nof_pms, lane;
    portmod_pm_instances_t pm_types_and_instances[] = {{portmodDispatchTypePm8x50_fabric, SOC_RAMON_NOF_PMS}};
    soc_dnxc_lane_map_db_map_t lane2serdes[DNXF_DATA_MAX_PORT_GENERAL_NOF_LINKS];
    SHR_FUNC_INIT_VARS(unit);
    
    /*portmod create for the unit*/
    nof_links = dnxf_data_port.general.nof_links_get(unit);
    rc = portmod_create(unit, 0, nof_links, nof_links, 1 , pm_types_and_instances);
    SHR_IF_ERR_EXIT(rc);

    nof_pms = dnxf_data_port.general.nof_pms_get(unit);
    rc = soc_dnxc_fabric_pms_add(unit, nof_pms, 0, FALSE, soc_ramon_core_address_get);
    SHR_IF_ERR_EXIT(rc);

    /* lane map (swap) - parse soc property and set the SW state database of the unit */
    if (!SOC_WARM_BOOT(unit))
    {
        for (lane = 0; lane < nof_links; lane++) {
            lane2serdes[lane].rx_id = dnxf_data_port.lane_map.info_get(unit, lane)->serdes_rx_id;
            lane2serdes[lane].tx_id = dnxf_data_port.lane_map.info_get(unit, lane)->serdes_tx_id;
        }
        SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_map_set(unit, nof_links, lane2serdes, 1 /*no need to check if port is in use in init sequence*/));
    }

    if (SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(soc_ramon_dynamic_port_restore(unit));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_port_deinit(int unit)
{
    int rc;
    int nof_pms = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    nof_pms = dnxf_data_port.general.nof_pms_get(unit);
    soc_dnxc_fabric_pms_destroy(unit, nof_pms);

    rc = portmod_destroy(unit);
    SHR_IF_ERR_EXIT(rc);
exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_port_detach(int unit, soc_port_t port)
{

    int rc, is_first_link, i, fsrd_block, fmac_block;
    SHR_FUNC_INIT_VARS(unit);

    /* Update relevant bitmaps - remove from enabled, add to disabled*/
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_REMOVE, soc_dnxf_port_update_type_sfi));    
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_REMOVE, soc_dnxf_port_update_type_port));
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_REMOVE, soc_dnxf_port_update_type_all));    
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_ADD, soc_dnxf_port_update_type_sfi_disabled));    
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_ADD, soc_dnxf_port_update_type_port_disabled));    
    SHR_IF_ERR_EXIT(rc);

    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_dynamic_port_update, (unit, port, SOC_DNXF_PBMP_PORT_ADD, soc_dnxf_port_update_type_all_disabled));    
    SHR_IF_ERR_EXIT(rc);
    
    rc = portmod_port_remove(unit, port);
    SHR_IF_ERR_EXIT(rc);

    rc = soc_ramon_port_first_link_in_fsrd_get(unit, port , &is_first_link);

    if (is_first_link == 1) /* last active link in the fsrd block */
    {
        fsrd_block = port/dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);

        fmac_block = fsrd_block * dnxf_data_device.blocks.nof_fmacs_in_fsrd_get(unit);

        for (i = fmac_block; i < fmac_block +  dnxf_data_device.blocks.nof_fmacs_in_fsrd_get(unit) ; i++)
        {
            rc = soc_ramon_port_update_valid_block_database(unit, SOC_BLK_FMAC, i, 0);
            SHR_IF_ERR_EXIT(rc);
        }

        rc = soc_ramon_port_update_valid_block_database(unit, SOC_BLK_FSRD, fsrd_block, 0);
        SHR_IF_ERR_EXIT(rc);

        rc = soc_ramon_port_update_fsrd_block(unit, port, 0);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_port_speed_max(int unit, soc_port_t port, int *speed)
{
    SHR_FUNC_INIT_VARS(unit);

    *speed=dnxf_data_port.general.speed_max_get(unit);

    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_speed_get(int unit, soc_port_t port, int *speed)
{
    portmod_port_interface_config_t config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_interface_config_get(unit, port, &config, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
    *speed = config.speed;
exit:
    SHR_FUNC_EXIT; 
}

shr_error_e
soc_ramon_port_speed_set(int unit, soc_port_t port, int speed)
{
    portmod_port_interface_config_t config;
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

    SHR_IF_ERR_EXIT(portmod_port_interface_config_get(unit, port, &config, 
                                          PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));
    config.speed = speed;
    SHR_IF_ERR_EXIT(portmod_port_interface_config_set(unit, port, &config, 
                                          PORTMOD_INIT_F_EXTERNAL_MOST_ONLY));

exit:
    SHR_FUNC_EXIT; 
}

shr_error_e
soc_ramon_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf)
{
    phymod_ref_clk_t ref_clk;
    phymod_phy_inf_config_t phy_config;
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    int lane = -1, i;
    portmod_access_get_params_t params;

    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

    portmod_access_get_params_t_init(unit, &params);
    params.lane = lane;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));

    SHR_IF_ERR_EXIT(dnxc_soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk));

    for (i=0 ; i<phys_returned ; i++) {
        SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(&phys[i], 0 /* flags */, ref_clk, &phy_config));
        PHYMOD_INTF_MODES_FIBER_CLR(&phy_config);
        PHYMOD_INTF_MODES_COPPER_CLR(&phy_config);
        switch (intf) {
        case SOC_PORT_IF_SR:
            PHYMOD_INTF_MODES_FIBER_SET(&phy_config);
            break;
        case SOC_PORT_IF_CR:
            PHYMOD_INTF_MODES_COPPER_SET(&phy_config);
            break;
        default:
            break;
        }
        SOC_IF_ERROR_RETURN(phymod_phy_interface_config_set(&phys[i], 0 /* flags */,&phy_config));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf)
{
    phymod_ref_clk_t ref_clk;
    phymod_phy_inf_config_t phy_config;
    phymod_phy_access_t phys[SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT];
    int phys_returned;
    int lane = -1;
    portmod_access_get_params_t params;

    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;

    portmod_access_get_params_t_init(unit, &params);
    params.lane = lane;
    params.phyn = PORTMOD_PHYN_LAST_ONE;
    params.sys_side = PORTMOD_SIDE_LINE;

    SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, SOC_DNXC_PORT_MAX_CORE_ACCESS_PER_PORT, phys, &phys_returned, NULL));

    SHR_IF_ERR_EXIT(dnxc_soc_to_phymod_ref_clk(unit, SOC_INFO(unit).port_refclk_int[port], &ref_clk));

    SOC_IF_ERROR_RETURN(phymod_phy_interface_config_get(&phys[0], 0 /* flags */, ref_clk, &phy_config));

    if (PHYMOD_INTF_MODES_FIBER_GET(&phy_config)) {
        *intf = SOC_PORT_IF_SR;
    } else if (PHYMOD_INTF_MODES_COPPER_GET(&phy_config)) {
        *intf = SOC_PORT_IF_CR;
    } else {
        *intf = SOC_PORT_IF_KR;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_burst_control_set(int unit, soc_port_t port, portmod_loopback_mode_t loopback)
{
    int fmac_block, fmac_inner_link;
    uint32 reg32_val;
    soc_dnxf_fabric_link_device_mode_t link_mode;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &fmac_block, &fmac_inner_link, SOC_BLK_FMAC)));

    /*Comma burst configuration*/
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, &reg32_val));
    if (loopback == portmodLoopbackMacOuter || loopback == portmodLoopbackMacAsyncFifo)
    {
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_MAC_LOOPBACK);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_MAC_LOOPBACK);
    } else {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_device_mode_get,(unit, port, 0/*tx*/, &link_mode)));
        
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
        switch (link_mode)
        {
            case soc_dnxf_fabric_link_device_mode_multi_stage_fe1:
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE1);
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE1);
                break;
            case soc_dnxf_fabric_link_device_mode_multi_stage_fe3:
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE3);
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE3);
                break;
            case soc_dnxf_fabric_link_device_mode_fe2:
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE2);
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE2);
                break;
            case soc_dnxf_fabric_link_device_mode_repeater:
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE3);
                soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE3);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid link mode");
                break;
        }
    }
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, reg32_val));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_port_serdes_power_disable(int unit , soc_pbmp_t disable_pbmp)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_link_status_get(int unit, soc_port_t port, int *link_up)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_link_get(unit, port, PORTMOD_INIT_F_EXTERNAL_MOST_ONLY, link_up));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_extract_cig_from_llfc_enable_set(int unit, soc_port_t port, int value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Extract Congrstion Ind from LLFC cells control is supported only by KR_FEC");
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
soc_ramon_port_extract_cig_from_llfc_enable_get(int unit, soc_port_t port, int *value)
{
    uint32 properties = 0;
    portmod_port_pcs_t encoding;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(portmod_port_encoding_get(unit, port, &properties, &encoding));
    if (encoding != PORTMOD_PCS_64B66B_FEC)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Extract Congrstion Ind from LLFC cells control is supported only by KR_FEC");
    }
    *value = PORTMOD_ENCODING_EXTRCT_CIG_FROM_LLFC_GET(properties);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_update_valid_block_database(int unit, int block_type, int block_number, int enable)
{
    soc_info_t *si;
    int blk;

    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);

    switch (block_type)
    {
        case SOC_BLK_FSRD:
            if(block_number < dnxf_data_device.blocks.nof_instances_fsrd_get(unit)) {
                blk = si->fsrd_block[block_number];
                si->block_valid[blk] = enable;
            }
            else {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_drv_update_block_database: too much FSRD blocks");
            }
            break;

        case SOC_BLK_FMAC:
            if(block_number < dnxf_data_device.blocks.nof_instances_fmac_get(unit)) {
                blk = si->fmac_block[block_number];
                si->block_valid[blk] = enable;
            }
            else {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "soc_dnxf_drv_update_block_database: too much FMAC blocks");
            }
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "soc_dnxf_drv_update_block_database: block not supported/recognized");
            break;

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_port_enable_set(int unit, soc_port_t port, int enable)
{
    uint32 reg_val[1];
    int inner_link, dcml_block;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &dcml_block, &inner_link, SOC_BLK_DCML)));

    if (enable)
    {
        /* When enable port make sure that MAC is enabled and we start sending reachaboility cells*/
        SHR_IF_ERR_EXIT(READ_DCML_CPU_FORCE_MAC_LINK_IREADYr(unit, dcml_block, reg_val));
        SHR_BITCLR(reg_val, inner_link);
        SHR_IF_ERR_EXIT(WRITE_DCML_CPU_FORCE_MAC_LINK_IREADYr(unit, dcml_block, *reg_val));

        SHR_IF_ERR_EXIT(READ_LCM_DISABLE_MACr(unit, dcml_block, reg_val));
        SHR_BITCLR(reg_val, inner_link);
        SHR_IF_ERR_EXIT(WRITE_LCM_DISABLE_MACr(unit, dcml_block, *reg_val));
    } else {
        /* When Disabeling first stop sending reachaboility cells*/
        SHR_IF_ERR_EXIT(READ_DCML_CPU_FORCE_MAC_LINK_IREADYr(unit, dcml_block, reg_val));
        SHR_BITSET(reg_val, inner_link);
        SHR_IF_ERR_EXIT(WRITE_DCML_CPU_FORCE_MAC_LINK_IREADYr(unit, dcml_block, *reg_val));

        SHR_IF_ERR_EXIT(READ_LCM_DISABLE_MACr(unit, dcml_block, reg_val));
        SHR_BITSET(reg_val, inner_link);
        SHR_IF_ERR_EXIT(WRITE_LCM_DISABLE_MACr(unit, dcml_block, *reg_val));
    }

    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, 0, enable));

exit:
    SHR_FUNC_EXIT;
}

/*Turn off link pump when leaky bucket is down for better link utilization*/
shr_error_e 
soc_ramon_port_pump_enable_set(int unit, soc_port_t port, int enable)
{
    int blk, inner_link;
    uint64 reg64_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &blk, &inner_link, SOC_BLK_FMAC)));

    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, blk, inner_link, &reg64_val));
    soc_reg64_field32_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg64_val ,FMAL_N_TX_PUMP_WHEN_LB_DNf, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, blk, inner_link, reg64_val));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_port_dynamic_soc_init(int unit, soc_port_t port, int is_first_link)
{
    uint32 reg32_val, field[1];
    uint64 reg64_val;
    int fmac_block, fmac_inner_link, blk, fmac_first_block, fsrd_block;
    soc_dnxf_fabric_link_device_mode_t link_mode;
    SHR_FUNC_INIT_VARS(unit);

    /* 
     *FMAC init
     */
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &fmac_block, &fmac_inner_link, SOC_BLK_FMAC)));
    fsrd_block = port/dnxf_data_device.blocks.nof_links_in_fsrd_get(unit);
    fmac_first_block = fsrd_block * dnxf_data_device.blocks.nof_fmacs_in_fsrd_get(unit);
    

    /*Leacky Bucket*/
    if (is_first_link)
    {
        for (blk = fmac_first_block; blk < fmac_first_block +  dnxf_data_device.blocks.nof_fmacs_in_fsrd_get(unit) ; blk++)
        {
            SHR_IF_ERR_EXIT(READ_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr(unit, blk, &reg32_val));
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, BKT_FILL_RATE_Nf, dnxf_data_port.general.mac_bucket_fill_rate_get(unit));
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, BKT_LINK_UP_TH_Nf, 0x20);
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, BKT_LINK_DN_TH_Nf, 0x10);
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, SIG_DET_BKT_RST_ENA_Nf, 0x1);
            soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg32_val, ALIGN_LCK_BKT_RST_ENA_Nf, 0x1);
            SHR_IF_ERR_EXIT(WRITE_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr(unit, blk, reg32_val));
        }
    }

    /*Comma burst configuration*/
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_device_mode_get,(unit, port, 0/*tx*/, &link_mode)));
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
    switch (link_mode)
    {
        case soc_dnxf_fabric_link_device_mode_multi_stage_fe1:
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE1);
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE1);
            break;
        case soc_dnxf_fabric_link_device_mode_multi_stage_fe3:
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE3);
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE3);
            break;
        case soc_dnxf_fabric_link_device_mode_fe2:
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE2);
            soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg32_val, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE2);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid link mode");
            break;
    }
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, reg32_val));

    /*Link TOPO mode*/
    if(SOC_DNXF_IS_FE13(unit)) {
        if (link_mode == soc_dnxf_fabric_link_device_mode_multi_stage_fe1)
        {
            SHR_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_block, &reg32_val));
            *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg32_val, LINK_TOPO_MODE_0f);
            SHR_BITSET(field, fmac_inner_link);
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg32_val, LINK_TOPO_MODE_0f, *field);
            *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg32_val, LINK_TOPO_MODE_1f);
            SHR_BITCLR(field, fmac_inner_link);
            soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg32_val, LINK_TOPO_MODE_1f, *field);
            SHR_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, fmac_block, reg32_val));
        }
    }

    /*LLFC enable*/
    SHR_IF_ERR_EXIT(READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, fmac_block, &reg32_val));
    *field = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg32_val, LNK_LVL_FC_TX_ENf);
    SHR_BITSET(field, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg32_val, LNK_LVL_FC_TX_ENf, *field);
    *field = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg32_val, LNK_LVL_FC_RX_ENf);
    SHR_BITSET(field, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg32_val, LNK_LVL_FC_RX_ENf, *field);
    SHR_IF_ERR_EXIT(WRITE_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, fmac_block, reg32_val));

    /*RX LOS interrupt*/
    SHR_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, fmac_block, fmac_inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg32_val ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
    soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg32_val ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, fmac_block, fmac_inner_link, reg32_val));

    /*Link pump*/
    COMPILER_64_ZERO(reg64_val);
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, &reg64_val));
    soc_reg64_field32_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg64_val ,FMAL_N_TX_PUMP_WHEN_LB_DNf, 1);
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, &reg64_val));


    /*Enable interleaving*/
    SHR_IF_ERR_EXIT(READ_FMAC_AUTO_DOC_NAME_12r(unit, fmac_block, &reg32_val));
    *field = soc_reg_field_get(unit, FMAC_AUTO_DOC_NAME_12r, reg32_val, FIELD_0_3f);
    SHR_BITSET(field, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_AUTO_DOC_NAME_12r, &reg32_val, FIELD_0_3f, *field);
    SHR_IF_ERR_EXIT(WRITE_FMAC_AUTO_DOC_NAME_12r(unit, fmac_block, reg32_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   On SKU devices, some of the quads are disabled.
 *   This function checks if Quad is disabled for current device.
 * \param [in] unit -
 *   The unit number.
 * \param [in] quad -
 *   The quad number.
 * \param [out] disabled -
 *   Is the quad disabled.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   This function should be filled accroding to SKUs.
 * \see
 *   None.
 */
shr_error_e
soc_ramon_port_quad_disabled(int unit, int quad, int *disabled)
{
    SHR_FUNC_INIT_VARS(unit);

    *disabled = 0;

    SHR_FUNC_EXIT; 
}

/*
 * Function:
 *      soc_ramon_port_fault_get
 * Purpose:
 *      Get port loopback
 * Parameters:
 *      unit -  (IN)  BCM device number 
 *      port -  (IN)  Device or logical port number .
 *      flags - (OUT) Flags to indicate fault type 
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
shr_error_e 
soc_ramon_port_fault_get(int unit, soc_port_t port, uint32* flags)
{
    soc_reg_above_64_val_t reg_val;
    SHR_FUNC_INIT_VARS(unit);

    (*flags) = 0;
    SHR_IF_ERR_EXIT(READ_RTP_LINK_STATE_VECTORr(unit, reg_val));
    if(!SHR_BITGET(reg_val,port)) {
        (*flags) |= BCM_PORT_FAULT_LOCAL;
    }

    SHR_IF_ERR_EXIT(READ_RTP_ACL_VECTORr(unit, reg_val));
    if(!SHR_BITGET(reg_val,port)) {
        (*flags) |= BCM_PORT_FAULT_REMOTE;
    }  

exit:
    SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_port_bucket_fill_rate_validate(int unit, uint32 bucket_fill_rate)
{

    SHR_FUNC_INIT_VARS(unit);

    if(bucket_fill_rate > SOC_RAMON_MAX_BUCKET_FILL_RATE) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "BUCKET_FILL_RATE: %d is out-of-ranget", bucket_fill_rate);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_port_rate_egress_ppt_set
 * Description:
 *      packets per second shaper
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      nof_packets , nof_tiks (out)- Rate in packet per tiks. A value of zero disables rate limiting (full speed).
 * Returns:
 *      _SHR_E_XXX
 */

shr_error_e
soc_ramon_port_rate_egress_ppt_set(int unit, soc_port_t port, uint32 burst, uint32 nof_tiks)
{
    uint32 limit_max;
    int max_length;
    uint64 reg64_val;
    int fmac_index; 
    SHR_FUNC_INIT_VARS(unit);
    
    /* 
     *Valdiate input
     */
    max_length = soc_reg_field_length(unit, FMAC_TX_CELL_LIMITr,  CELL_LIMIT_COUNTf);
    limit_max = 0;
    SHR_BITSET_RANGE(&limit_max, 0, max_length);
    if (limit_max <  burst) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Burst %u is too big - should be < %u", burst, limit_max); 
    }

    max_length = soc_reg_field_length(unit, FMAC_TX_CELL_LIMITr,  CELL_LIMIT_PERIODf);
    limit_max = 0;
    SHR_BITSET_RANGE(&limit_max, 0, max_length);
    if (limit_max <  nof_tiks) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Burst %u is too big relative to pps %u", burst, nof_tiks); 
    }

    /*disable shaper handle*/
    if (burst == 0)
    {
        fmac_index = port / dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
        COMPILER_64_ZERO(reg64_val);
        SHR_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_index, reg64_val));
    }
    else if (burst < 3)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Burst %u is too small - should be > 2", burst);
    }
    else
    {
        fmac_index = port / dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
        COMPILER_64_ZERO(reg64_val);
        soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_COUNTf, burst - 2 /*-2 for shaper accuracy*/);
        soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_PERIODf, nof_tiks);
        SHR_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_index, reg64_val));
    }

exit:
    SHR_FUNC_EXIT;
}
shr_error_e

/*
 * Function:
 *      soc_ramon_port_rate_egress_ppt_get
 * Description:
 *      packets per second shaper
 * Parameters:
 *      unit - Device number
 *      port - Port number
 *      nof_packets , nof_tiks (out)- Rate in packet per tiks. A value of zero disables rate limiting (full speed).
 * Returns:
 *      _SHR_E_XXX
 */

soc_ramon_port_rate_egress_ppt_get(int unit, soc_port_t port, uint32 *burst, uint32 *nof_tiks)
{
    uint64 reg64_val;
    int fmac_index; 
    SHR_FUNC_INIT_VARS(unit);
    
    fmac_index = port / dnxf_data_device.blocks.nof_links_in_fmac_get(unit);

    SHR_IF_ERR_EXIT(READ_FMAC_TX_CELL_LIMITr(unit, fmac_index, &reg64_val));
    *burst =  soc_reg64_field32_get(unit, FMAC_TX_CELL_LIMITr, reg64_val, CELL_LIMIT_COUNTf) + 2/*+2 for shaper accuracy*/;
    *nof_tiks = soc_reg64_field32_get(unit, FMAC_TX_CELL_LIMITr, reg64_val, CELL_LIMIT_PERIODf);

exit:
    SHR_FUNC_EXIT; 
}
/**
 * /brief 
 * Function to update port bitmaps during init and dynamic 
 * procedures. 
 * Functions should receive one of two flags as a third argument 
 * depending on what is needed. 
 * - SOC_DNXF_PBMP_PORT_ADD - to add port to given pbmp
 * - SOC_DNXF_PBMP_PORT_REMOVE - to remove port to given pbmp
 * 
 * @author db018457 (9/1/2017)
 * 
 * @param unit 
 * @param port 
 * @param add_remove 
 * @param type 
 * 
 * @return shr_error_e 
 */
shr_error_e
soc_ramon_port_dynamic_port_update(int unit, int port, int add_remove, soc_dnxf_port_update_type_t type)
{
    soc_info_t          *si;

    SHR_FUNC_INIT_VARS(unit);

    /* SKU SUPPORT - get all enabled device links*/
    /* SAFETY CHECK
       Practically this branch should not be entered in any case.
       The dynamic port update function should not be called with requests:
       - TO REMOVE not supported port to the DISABLED PBMP
       - TO ADD not supported port to the ENABLED PBMP*/
    if (!(PBMP_MEMBER(dnxf_data_port.general.supported_phys_get(unit)->pbmp, port)) ) {
        if (((add_remove == SOC_DNXF_PBMP_PORT_REMOVE) && (type==soc_dnxf_port_update_type_sfi_disabled)) ||
            ((add_remove == SOC_DNXF_PBMP_PORT_ADD)    && (type==soc_dnxf_port_update_type_sfi)))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Fabric link %d is disabled for device %s", port, soc_dev_name(unit));
        }
    }

    si  = &SOC_INFO(unit);

    if (add_remove == SOC_DNXF_PBMP_PORT_ADD)
    {
        switch (type)
        {
            case soc_dnxf_port_update_type_sfi:
                DNXF_ADD_DYNAMIC_PORT(sfi, port);
                break;
            case soc_dnxf_port_update_type_port:
                DNXF_ADD_DYNAMIC_PORT(port, port);
                break;
            case soc_dnxf_port_update_type_all:
                DNXF_ADD_DYNAMIC_PORT(all, port);
                break;
            case soc_dnxf_port_update_type_sfi_disabled:
                DNXF_ADD_DYNAMIC_DISABLED_PORT(sfi, port);
                break;
            case soc_dnxf_port_update_type_port_disabled:
                DNXF_ADD_DYNAMIC_DISABLED_PORT(port, port);
                break;
            case soc_dnxf_port_update_type_all_disabled:
                DNXF_ADD_DYNAMIC_DISABLED_PORT(all, port);
                break;                      
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown port update type");
                break;
        }
    }
    else if (add_remove == SOC_DNXF_PBMP_PORT_REMOVE)
    {
        switch (type)
        {
            case soc_dnxf_port_update_type_sfi:
                DNXF_REMOVE_DYNAMIC_PORT(sfi, port);
                break;
            case soc_dnxf_port_update_type_port:
                DNXF_REMOVE_DYNAMIC_PORT(port, port);
                break;
            case soc_dnxf_port_update_type_all:
                DNXF_REMOVE_DYNAMIC_PORT(all, port);
                break;
            case soc_dnxf_port_update_type_sfi_disabled:
                DNXF_REMOVE_DYNAMIC_DISABLED_PORT(sfi, port);
                break;
            case soc_dnxf_port_update_type_port_disabled:
                DNXF_REMOVE_DYNAMIC_DISABLED_PORT(port, port);
                break;
            case soc_dnxf_port_update_type_all_disabled:
                DNXF_REMOVE_DYNAMIC_DISABLED_PORT(all, port);
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown port update type");
                break;
        }
    } else {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown control type %d provided", add_remove);
    }
exit:
    SHR_FUNC_EXIT;

}


#undef BSL_LOG_MODULE

