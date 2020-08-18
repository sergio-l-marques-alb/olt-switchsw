/*
 * $Id: ramon_fabric_cell_snake_test.c,v 1.30 Broadcom SDK $
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC RAMON FABRIC CELL SNAKE TEST
 */
 
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/mcm/memregs.h>
#include <soc/mcm/allenum.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <shared/bitop.h>

#include <sal/core/boot.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell_snake_test.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

#include <soc/dnxf/ramon/ramon_fabric_cell_snake_test.h>
#include <soc/dnxf/ramon/ramon_drv.h>
#include <soc/dnxf/ramon/ramon_fabric_links.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

/*
 * Local functions
 */
STATIC int soc_ramon_cell_snake_test_reg_reset(int unit);
STATIC int soc_ramon_cell_snake_test_sw_configuration_set(int unit);
STATIC int soc_ramon_cell_snake_test_interrupts_clear(int unit);
STATIC int soc_ramon_cell_snake_test_mac_loopback_set(int unit, int port);
STATIC int soc_ramon_cell_snake_test_post_init_ports(int unit, int nof_cores,
            int nof_links_in_core, int *ref_ports, uint32 *uninitialized_pbmp);


/*
 * Function:
 *      soc_ramon_cell_snake_test_prepare
 * Purpose:
 *      Prepare system to cell snake test.
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      flags    - (IN)  Configuration parameters
 *      config   - (IN)  Cell snake test configuration
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
int
  soc_ramon_cell_snake_test_prepare(
    
    int unit, 
    uint32 flags)
{
    
    uint32 reg_val, field_val[1];
    soc_reg_above_64_val_t reg_val_above_64, 
                           block_bitmap_default;
    bcm_port_t port;
    int rv;
    int nof_links;
    int nof_links_in_core, link_idx_in_core, link;
    int nof_cores, core_idx;
    uint32 core_pbmp[1], curr_core_pbmp[1];
    uint32 uninitialized_pbmp[32];
    int ref_ports[32];
    SHR_FUNC_INIT_VARS(unit);

    sal_usleep(100*10000); /* sleep 1 s */

    nof_links = dnxf_data_port.general.nof_links_get(unit);
    nof_links_in_core = dnxf_data_device.blocks.nof_links_in_phy_core_get(unit);
    nof_cores = nof_links / nof_links_in_core;

    /* go over all phy cores of the device and check that either all ports of the core are attached or all ports of the core are detached */
    *core_pbmp = 0;
    SHR_BITSET_RANGE(core_pbmp, 0, nof_links_in_core);
    for (core_idx = 0; core_idx < nof_cores; ++core_idx)
    {
        *curr_core_pbmp = 0;
        uninitialized_pbmp[core_idx] = 0;
        ref_ports[core_idx] = 0;
        for(link_idx_in_core = 0; link_idx_in_core < nof_links_in_core; ++link_idx_in_core)
        {
            link = core_idx*nof_links_in_core + link_idx_in_core;
            if(SOC_PBMP_MEMBER(PBMP_SFI_ALL(unit), link))
            {
                SHR_BITSET(curr_core_pbmp, link_idx_in_core);

                /* Because dnxc_data_mgmt_table_data_internal_get doesn't allow to get
                 * "port_init_speed" after system has been intialized, the only way to get its value here is to use
                 * soc_property_port_get.
                 */
                if (soc_property_port_get(unit, link, spn_PORT_INIT_SPEED, -1) == -1)
                {
                    SHR_BITSET(&uninitialized_pbmp[core_idx], link_idx_in_core);
                }
                else
                {
                    ref_ports[core_idx] = link;
                }
            }
        }
        if ((*curr_core_pbmp != *core_pbmp) && (*curr_core_pbmp != 0))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "the test is supported only if ALL ports in a phy core are active (probed) or ALL ports in the core are deactivated (detached).\n"
                    "in core %d PART of the ports are active and PART deactivated.\n", core_idx);
        }
    }

    SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_post_init_ports(unit, nof_cores, nof_links_in_core, ref_ports, uninitialized_pbmp));

    /*
     * enable FEC on control cells to avoid snake test failure in case of links which have poor performance due to CRC on control cells
     */
    if(flags & SOC_DNXF_ENABLE_EXTERNAL_LOOPBACK)
    {
        PBMP_SFI_ITER(unit, port)
        {
            rv = soc_dnxc_port_control_cells_fec_bypass_enable_set(unit, port, 0);
            SHR_IF_ERR_EXIT(rv);
        }
    }

    /* 
     *Disable interrupts
     */

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_interrupt_all_enable_set, (unit, 0));
    SHR_IF_ERR_EXIT(rv);

    /* 
     * Loopback configuration
     */
    if (!(flags & SOC_DNXF_ENABLE_MAC_LOOPBACK)) 
	{

        PBMP_SFI_ITER(unit, port)
        {
                if(flags & SOC_DNXF_ENABLE_EXTERNAL_LOOPBACK) {
                    rv = soc_dnxf_port_loopback_set(unit, port, portmodLoopbackCount);
                    SHR_IF_ERR_EXIT(rv);
                } else {
                    rv = soc_dnxf_port_loopback_set(unit, port, portmodLoopbackPhyGloopPMD);
                    SHR_IF_ERR_EXIT(rv);
                }

                sal_usleep(20*1000); /* sleep 20ms */  
        }

        sal_usleep(100*1000); /* sleep 100ms */
    }

    /* 
     * Blocks reset
     * perform a blocks reset as a preparation for the test
     */
   
    if (flags & SOC_DNXF_ENABLE_MAC_LOOPBACK)
    {
        /*For MAC loopback - reset all blocks */
        SOC_REG_ABOVE_64_ALLONES(block_bitmap_default);
    }
    else
    {
        /*For PHY, External loopback - reset all blocks except FMAC & FSRD */
        SOC_REG_ABOVE_64_CLEAR(block_bitmap_default);
        SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_0, 0);
        SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_1, 1);
        SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_2, 2);
        SOC_REG_ABOVE_64_WORD_SET(block_bitmap_default, SOC_RAMON_DRV_BLOCKS_RESET_WITHOUT_FMAC_FSRD_WORD_3, 3);
    }

    SOC_REG_ABOVE_64_COPY(reg_val_above_64, block_bitmap_default);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_blocks_reset, (unit, 1 /*force blocks*/, &reg_val_above_64));
    SHR_IF_ERR_EXIT(rv);

    sal_usleep(200);

    SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_reg_reset(unit));

    if (dnxf_data_fabric.hw_snake.feature_get(unit, dnxf_data_fabric_hw_snake_is_sw_config_required) && !(flags & SOC_DNXF_ENABLE_MAC_LOOPBACK))
    {
        SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_sw_configuration_set(unit));
    }

    /* 
     *set back default statistics counters control
     */
    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stat_init, (unit));
    SHR_IF_ERR_EXIT(rv);

    /*
     * Test mode settings
     */
    SHR_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_REGr(unit, &reg_val));

    if (dnxf_data_fabric.hw_snake.feature_get(unit, dnxf_data_fabric_hw_snake_is_sw_config_required))
    {
        *field_val = ((flags & SOC_DNXF_ENABLE_MAC_LOOPBACK) ? 0x1:0x0);
        soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_0_0f, *field_val);
        soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_12f, *field_val);
    }
    else
    {
        soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_12f, 1);
    }

    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_2_10f, 0);

    /*Loopback*/
    *field_val = ((!(flags & SOC_DNXF_ENABLE_MAC_LOOPBACK)) ? 0x1:0x0);
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_13f, *field_val); 
          
    soc_reg_field_set(unit, OCCG_TEST_MODE_CMD_REGr, &reg_val, FIELD_1f, 0); 
 
    SHR_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_REGr(unit, reg_val));

    SHR_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));  
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, START_GEN_CELLf, 0);
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 0);
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_FILTER_PROPf, 0);
    SHR_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));

    /* clear RTP interrupts */
    SHR_IF_ERR_EXIT(WRITE_RTP_GENERAL_INTERRUPT_REGISTERr(unit, -1));
    SHR_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));
    soc_reg_above_64_field32_set(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_TRGf, 1);
    SHR_IF_ERR_EXIT(WRITE_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));

    /*
     * set MAC loopback and activate MAC on ports that are detached.
     * no need to do this if the test is in MAC loopback mode, since in this case OCCG sets MAC loopabck on all ports (regrdless of their attach state)
     */
    if (!(flags & SOC_DNXF_ENABLE_MAC_LOOPBACK))
    {
        PBMP_ITER(SOC_PORT_DISABLED_BITMAP(unit, sfi), port)
        {
            rv = soc_ramon_cell_snake_test_mac_loopback_set(unit, port);
            SHR_IF_ERR_EXIT(rv);
        }
    }

    sal_usleep(100*10000); /* sleep 1 s */



exit:
    SHR_FUNC_EXIT;
}
    
    
/*
 * Function:
 *      soc_ramon_cell_snake_test_run
 * Purpose:
 *      Run cell snake test.
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      flags     - (IN)  Configuration parameters
 *      usec      - (IN)  Timeout
 *      results   - (IN)  Cell snake test results
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
int
  soc_ramon_cell_snake_test_run(
    int unit, 
    uint32 flags, 
    soc_dnxf_fabric_cell_snake_test_results_t* results)
{  
    soc_timeout_t to;
    uint32 reg_val, field_val;
    soc_reg_above_64_val_t reg_val_above_64, field_val_above_64;
 
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_ramon_cell_snake_test_interrupts_clear(unit));


    sal_memset(results, 0, sizeof(soc_dnxf_fabric_cell_snake_test_results_t));

    /* clear finish interrupt*/
    SHR_IF_ERR_EXIT(READ_OCCG_INTERRUPT_REGISTERr(unit, &reg_val));
    soc_reg_field_set(unit,OCCG_INTERRUPT_REGISTERr,&reg_val,TEST_MODE_CMD_FINISH_INTf,0x1);
    SHR_IF_ERR_EXIT(WRITE_OCCG_INTERRUPT_REGISTERr(unit, reg_val));

    /* Verify that STOP run will be done only when test is on */
    if (flags & SOC_DNXF_SNAKE_STOP_TEST)
    {
        SHR_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));
        field_val = soc_reg_field_get(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, reg_val, EXT_WAIT_CELL_PROPf);
        if (field_val != 0x0)
        {
          SHR_ERR_EXIT(_SHR_E_PARAM, "test is not on %d",field_val); 
        }
    }

    /*Start test*/
    if (!(flags & SOC_DNXF_SNAKE_STOP_TEST)) {  
        SHR_IF_ERR_EXIT(READ_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, &reg_val));
        soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, START_GEN_CELLf, 1);
        soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 0);
        SHR_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));
    }
  
          
    if (flags & SOC_DNXF_SNAKE_INFINITE_RUN)
    {
          /* Keep test on and exit */
          SHR_EXIT();
    }
    else
    {
          /*Wait 1 sec*/
          sal_usleep(1000 * 1000);
    }

    /* Stop Test */
    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_CELL_PROPf, 1);
    SHR_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));

    /* Wait to finish filtering */
    sal_usleep(1000 * 1000);

    soc_reg_field_set(unit, OCCG_SYNC_TEST_MODE_OPERATIONr, &reg_val, EXT_WAIT_FILTER_PROPf, 1);
    SHR_IF_ERR_EXIT(WRITE_OCCG_SYNC_TEST_MODE_OPERATIONr(unit, reg_val));

    /*Poll for the results*/
    soc_timeout_init(&to, 1000000 /*1 sec*/, 100);
  
    while(1)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);
        SHR_IF_ERR_EXIT(READ_OCCG_TEST_MODE_CMD_RESULT_REGr(unit, reg_val_above_64));
        soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_TRGf, field_val_above_64); 
        if(SOC_REG_ABOVE_64_IS_ZERO(field_val_above_64) || SAL_BOOT_PLISIM) {
            /*test is done*/            
            break;
        }
        
        if (soc_timeout_check(&to)) {
            SHR_ERR_EXIT(_SHR_E_TIMEOUT, "timeout - test was not triggered"); 
        }
    }


    /*
     * Parse results
     */
  
    results->test_failed = soc_reg_above_64_field32_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_FAILf);
     
    if (results->test_failed)
    {
        soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_INT_STATUSf, results->interrupts_status); 
    }
    
    SOC_REG_ABOVE_64_CLEAR(field_val_above_64);
    soc_reg_above_64_field_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, TEST_MODE_STAGE_STATUSf, field_val_above_64);
    field_val = field_val_above_64[0];


    results->failure_stage_flags = 0;

    /*Config stage failure*/
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG_POS 0
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONFIG;
    }

    /*Get out of reset failure*/
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_GET_OUT_OF_RESET_POS 1
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_GET_OUT_OF_RESET_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_GET_OUT_OF_RESET;
    }

    /*Data ell generation failure*/
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_GENERATION_POS 2
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_GENERATION_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_GENERATION;
    }

    /*Control cell generation failure*/
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_GENERATION_POS 3
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_GENERATION_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_GENERATION;
    }

    /*Data cell filter write command failure*/
    #define _SOC_RAMON_SNAKE_FABRIC_CELL_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_WRITE_COMMAND_POS 4
    if (field_val & (0x1 << _SOC_RAMON_SNAKE_FABRIC_CELL_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_WRITE_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_WRITE_COMMAND;      
    }
    /*Control cell filter write command failure*/
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_WRITE_COMMAND_POS 5
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_WRITE_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_WRITE_COMMAND;
    }

    /*Data cell filter read command failure*/
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_READ_COMMAND_POS 6
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_DATA_CELL_FILTER_READ_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_DATA_CELL_FILTER_READ_COMMAND;
    }

    /*Control cell filter read command failure*/
    #define _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_READ_COMMAND_POS 7
    if (field_val & (0x1 << _SOC_RAMON_FABRIC_CELL_SNAKE_TEST_FAILURE_STAGE_REGISTER_CONTROL_CELL_FILTER_READ_COMMAND_POS))
    {
      results->failure_stage_flags |= SOC_DNXF_SNAKE_TEST_FAILURE_STAGE_CONTROL_CELL_FILTER_READ_COMMAND;
    }

    /*LFSRs*/
    results->lfsr_per_pipe[0] = soc_reg_above_64_field32_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, LFSR_VALUE_P_0f);
    results->lfsr_per_pipe[1] = soc_reg_above_64_field32_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, LFSR_VALUE_P_1f);
    results->lfsr_per_pipe[2] = soc_reg_above_64_field32_get(unit, OCCG_TEST_MODE_CMD_RESULT_REGr, reg_val_above_64, LFSR_VALUE_P_2f);

    
    /* verify that the test was performed */
    SHR_IF_ERR_EXIT(READ_OCCG_INTERRUPT_REGISTERr(unit, &reg_val));
    field_val = soc_reg_field_get(unit,OCCG_INTERRUPT_REGISTERr,reg_val,TEST_MODE_CMD_FINISH_INTf);
    if (field_val != 1) 
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "ERROR - configuration error. test did not run\n");
    }
	
exit:

    cli_out("Warning: Device reset is required for full functionality\n");
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_cell_snake_test_reg_reset(
    int unit)
{

    int i;
    uint32 reg_val32;
    bcm_port_t port;
    int port_enable_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    /* disable FMACs */
    PORTMOD_PORT_ENABLE_MAC_SET(port_enable_flags);
    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, port_enable_flags, 0));
    }

    /*Enable interleaving*/
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_AUTO_DOC_NAME_12r(unit, 0xf));

    /*Enable RX_LOS_SYNC interrupt*/
    for(i=0; i<dnxf_data_device.blocks.nof_links_in_fmac_get(unit); i++) {
        SHR_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
    }

    /*Disable Link-Level Flow-control*/
    SHR_IF_ERR_EXIT(READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, REG_PORT_ANY, &reg_val32));
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32 ,LNK_LVL_FC_TX_ENf, 0);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32 ,LNK_LVL_FC_RX_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, reg_val32));

    /* enable FMACs */
    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, port_enable_flags, 1));
    }


exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_cell_snake_test_sw_configuration_set
 * Purpose:
 *      Prepare blocks by SW to snake test
 *      Workaround for BCM88790 OCCG Errata
 * Parameters:
 *      unit     - (IN)  Unit number.
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
STATIC int
soc_ramon_cell_snake_test_sw_configuration_set(int unit)
{
    int index;
    uint64 reg64_val;
    soc_reg_above_64_val_t reg_above64_val;
    bcm_port_t port;
    int port_enable_flags = 0;
    SHR_FUNC_INIT_VARS(unit);

    /* DCH  configuration */
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_INTERRUPT_MASK_REGISTERr(unit,  0x3EF7B87F));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_DCH_ENABLERS_REGISTER_1r(unit, 0x7C00038));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_PIPES_SEPARATION_REGISTERr(unit, 0xA4A4));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_AUTO_DOC_NAME_0r(unit, 0x3));

    for (index = 0 ; index < dnxf_data_fabric.general.max_nof_pipes_get(unit) ; index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_DCH_DCH_ENABLERS_REGISTER_2_Pr, REG_PORT_ANY, index, 0x7E00002));
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_DCH_DCH_ENABLERS_REGISTER_3_Pr, REG_PORT_ANY, index, 0x1));
    }

    COMPILER_64_ALLONES(reg64_val);
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, reg64_val));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, reg64_val));

    /* RTP  configuration */
    SHR_IF_ERR_EXIT(WRITE_RTP_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_RTP_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_RTP_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_RTP_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_RTP_GENERAL_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));

    COMPILER_64_SET(reg64_val, 0x00000004, 0x00000000);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr(unit, reg64_val));

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOC_REG_ABOVE_64_WORD_SET(reg_above64_val, 0x00000000, 0);
    SOC_REG_ABOVE_64_WORD_SET(reg_above64_val, 0x003FC000, 1);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, REG_PORT_ANY, 0, reg_above64_val));

    /* QRH  configuration */
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit,  0x0));

    COMPILER_64_SET(reg64_val, 0x0000ffff, 0x00000038);
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_DRH_LOAD_BALANCING_GENERAL_CONFIGr(unit, reg64_val));
    COMPILER_64_SET(reg64_val, 0x00000000, 0x50029FF0);
    SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr(unit, reg64_val));

    /* MCT  configuration */
    SHR_IF_ERR_EXIT(WRITE_MCT_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_MCT_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_MCT_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_MCT_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));

    /* DCML configuration */
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCML_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCML_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCML_FPC_FREE_ERROR_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_DCML_FPC_ALLOC_ERROR_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));

    SOC_REG_ABOVE_64_ALLONES(reg_above64_val);
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, BRDC_DCML_ECC_ERR_1B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, reg_above64_val));
    SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, BRDC_DCML_ECC_ERR_2B_MONITOR_MEM_MASKr, REG_PORT_ANY, 0, reg_above64_val));

    /* LCM  configuration */
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_INTERRUPT_MASK_REGISTERr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_LCM_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_DTML_ENABLERSr(unit, 0x100001));

    /* CCH  configuration */
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_INTERRUPT_MASK_REGISTERr(unit, 0x5F9));
    SHR_IF_ERR_EXIT(WRITE_BRDC_CCH_CCH_CONFIGURATIONSr(unit, 0xe));

    /* FMAC configuration */
    /* disable FMACs */
    PORTMOD_PORT_ENABLE_MAC_SET(port_enable_flags);
    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, port_enable_flags, 0));
    }

    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_ECC_ERR_1B_MONITOR_MEM_MASKr(unit, 0xFFFFFEFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_ECC_ERR_2B_MONITOR_MEM_MASKr(unit, 0xFFFFFEFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_ECC_INTERRUPT_REGISTER_MASKr(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAC_INTERRUPT_MASK_REGISTER_1r(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAC_INTERRUPT_MASK_REGISTER_8r(unit, 0xFFFFFFFF));
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_LINK_TOPO_MODE_REG_0r(unit, 0xff));

    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_RECEIVE_RESET_REGISTERr(unit,  0x0));

    for (index = 0 ; index < dnxf_data_device.blocks.nof_links_in_fmac_get(unit) ; index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, REG_PORT_ANY, index, 0x80400));
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_FMAC_FMAL_GENERAL_CONFIGURATIONr, REG_PORT_ANY, index, 0x236c));
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, REG_PORT_ANY, index, 0x80000E09));
        SHR_IF_ERR_EXIT(soc_reg_set(unit, BRDC_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, REG_PORT_ANY, index,  0x11));
    }

    /* enable FMACs */
    SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, port, port_enable_flags, 1));
    }

    COMPILER_64_SET(reg64_val, 0x001FFFE0, 0x0001FF03);
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_INTERRUPT_MASK_REGISTERr(unit, reg64_val));
    COMPILER_64_SET(reg64_val, 0x00001800, 0x18999086);
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, reg64_val));



exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_cell_snake_test_interrupts_clear(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    
    /* if necessary clear interrupt after OCCG trigger */

    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_cell_snake_test_mac_lb_set
 * Purpose:
 *      For a case where some of the MAC lanes are detached -
 *      configure MAC loopback on the disabled MAC lanes and activate them.
 *      It is not possible to use the regular loopback API on detached links.
 *      This is done because the snake test will generate cells in all the DCMLs, even the DCMLs that are connected to disabled links.
 *      If we configure the MAC loopback, these cells will be captured and counted and test will succeed.
 * Parameters:
 *      unit     - (IN)  Unit number.
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
STATIC int
soc_ramon_cell_snake_test_mac_loopback_set(int unit, int port)
{
    uint32 reg_val;
    uint32 field_val[1];
    int fmac_block, fmac_inner_link;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_ramon_drv_link_to_block_mapping(unit, port, &fmac_block, &fmac_inner_link, SOC_BLK_FMAC));

    /* set MAC loopback and disable back-pressure from SerDes Tx */
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, &reg_val));
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMALN_CORE_LOOPBACKf, 1);
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg_val, FMAL_N_TX_SRD_BACKPRESSURE_ENf, 0);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_block, fmac_inner_link, reg_val));

    /* enable FMAC TX */
    SHR_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_block, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_TX_RST_Nf);
    SHR_BITCLR(field_val, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_TX_RST_Nf, *field_val);
    SHR_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_block, reg_val));

    /* enable FMAC RX */
    SHR_IF_ERR_EXIT(READ_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_block, &reg_val));
    *field_val = soc_reg_field_get(unit, FMAC_RECEIVE_RESET_REGISTERr, reg_val, FMAC_RX_RST_Nf);
    SHR_BITCLR(field_val, fmac_inner_link);
    soc_reg_field_set(unit, FMAC_RECEIVE_RESET_REGISTERr, &reg_val, FMAC_RX_RST_Nf, *field_val);
    SHR_IF_ERR_EXIT(WRITE_FMAC_RECEIVE_RESET_REGISTERr(unit, fmac_block, reg_val));

exit:
    SHR_FUNC_EXIT;
}

extern int bcm_dnxf_port_resource_set(int,bcm_gport_t,bcm_port_resource_t *);
extern int bcm_dnxf_port_resource_get(int,bcm_gport_t,bcm_port_resource_t *);
STATIC int
soc_ramon_cell_snake_test_post_init_ports(int unit, int nof_cores,
int nof_links_in_core, int *ref_ports, uint32 *uninitialized_pbmp)
{
    int core_idx;
    int uninitialized_port, link_idx_in_core;
    bcm_port_resource_t port_resource;
    SHR_FUNC_INIT_VARS(unit);

    for (core_idx = 0; core_idx < nof_cores; ++core_idx)
    {
        if (ref_ports[core_idx] == 0)
        {
            continue;
        }

        if (SHR_BITNULL_RANGE(&uninitialized_pbmp[core_idx], 0, nof_links_in_core) == 1)
        {
            continue;
        }

        bcm_port_resource_t_init(&port_resource);
        SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_get(unit, ref_ports[core_idx], &port_resource));

        SHR_BIT_ITER(&uninitialized_pbmp[core_idx], nof_links_in_core, link_idx_in_core)
        {
            uninitialized_port = core_idx * nof_links_in_core + link_idx_in_core;
            port_resource.port = uninitialized_port;
            SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_set(unit, uninitialized_port, &port_resource));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

