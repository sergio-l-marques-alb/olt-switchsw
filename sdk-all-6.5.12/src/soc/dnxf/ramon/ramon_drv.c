/*
 * $Id: ramon_drv.c,v 1.1.2.1 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC RAMON DRV
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/mem.h>

#include <soc/dnxc/drv.h>
#include <soc/dnxc/legacy/dnxc_cmic.h>
#include <soc/dnxc/legacy/dnxc_iproc.h>
#include <soc/dnxc/legacy/dnxc_ser_correction.h>
#include <soc/dnxc/legacy/dnxc_dev_feature_manager.h>
#include <soc/sand/sand_mem.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/mbcm.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

#include <soc/dnxf/ramon/ramon_drv.h>
#include <soc/dnxf/ramon/ramon_defs.h>
#include <soc/dnxf/ramon/ramon_intr.h>
#include <soc/dnxf/ramon/ramon_stack.h>
#include <soc/dnxf/ramon/ramon_port.h>
#include <soc/dnxf/ramon/ramon_fabric_links.h>
#include <soc/dnxf/ramon/ramon_fabric_topology.h>

/* CMICx includes */
#include <soc/cmicx_miim.h>

extern char *_build_release;

/* 
 * SBUS rings mapping
 */
#define _SOC_RAMON_DRV_SBUS_RING_MAP_0_VAL                 (0x02222227)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_1_VAL                 (0x33333222)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_2_VAL                 (0X55333333)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_3_VAL                 (0x65555555)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_4_VAL                 (0x66666666)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_5_VAL                 (0x55555555)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_6_VAL                 (0x66666665)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_7_VAL                 (0x44444466)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_8_VAL                 (0x10444444)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_9_VAL                 (0x22040065)
#define _SOC_RAMON_DRV_SBUS_RING_MAP_10_VAL                (0x00000033)

#define _SOC_RAMON_DRV_BRDC_FMAC_AC_ID                     (72)
#define _SOC_RAMON_DRV_BRDC_FMAC_BD_ID                     (73)
#define _SOC_RAMON_DRV_BRDC_DCH                            (78)
#define _SOC_RAMON_DRV_BRDC_DCM                            (80)
#define _SOC_RAMON_DRV_BRDC_DCL                            (79)
#define _SOC_RAMON_DRV_BRDC_CCH                            (81)
#define _SOC_RAMON_DRV_BRDC_FSRD                           (76)

/*
 * PVT monitor
 */
#define _SOC_RAMON_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT   (32)
#define _SOC_RAMON_ECI_PVT_MON_CONTROL_REG_RESET_BIT       (33)

/* WFQ config */
#define _SOC_RAMON_WFQ_PIPES_PRIORITY_INIT_VALUE           (0x7)

/* FIFO Profile bitmaps */
#define SOC_RAMON_DRV_DTM_FIFO_PROFILE_BMP_LOW  (0xF0)
#define SOC_RAMON_DRV_DTM_FIFO_PROFILE_BMP_HIGH (0x0F)

/* Link hysteresis */
#define SOC_RAMON_DRV_LINK_LOAD_SAT_VAL         (0x7ff)
#define SOC_RAMON_DRV_LINK_LOAD_HYST_TYPE_BYTE  (0)     /* bytes counter */
#define SOC_RAMON_DRV_LINK_LOAD_HYST_TYPE_FRAG  (1)     /* fragment counter */
#define SOC_RAMON_DRV_LINK_LOAD_HYST_LOW_TH     (0)
#define SOC_RAMON_DRV_LINK_LOAD_HYST_HIGH_TH    (0xffff)

/* DFL config */
#define SOC_RAMON_DRV_DFL_AUTO_DOC_NAME_33      (0x6fd)

/* FMAC config */
#define SOC_RAMON_DRV_ASYNC_FIFO_AUTO_DOC_NAME_59   (5)
#define SOC_RAMON_DRV_BURST_PERIOD                  (0xd)

/* Dynamic Memory Access Enable */
#define SOC_RAMON_DRV_NOF_DYN_MEM_ACCESS_REGS       (6)

STATIC int
soc_ramon_reset_cmic_iproc_regs(int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    /*Configure PAXB, enabling the access of CMIC*/
    SHR_IF_ERR_EXIT(soc_dnxc_iproc_config_paxb(unit));
    sal_usleep(10*1000); /*wait 10 mili sec*/
    SHR_IF_ERR_EXIT(soc_dnxc_cmicx_device_hard_reset(unit, SOC_DNXC_RESET_ACTION_INOUT_RESET));
    SHR_IF_ERR_EXIT(soc_dnxc_iproc_config_paxb(unit));

    /* Config Endianess */
    soc_endian_config(unit);
    soc_pci_ep_config(unit,0);

    /*SBUS rings map configuration*/
    SHR_IF_ERR_EXIT(soc_ramon_drv_rings_map_set(unit));
    SHR_IF_ERR_EXIT(soc_dnxc_cmic_sbus_timeout_set(unit, dnxf_data_device.general.core_clock_khz_get(unit) /* KHz */ ,
                                                   SOC_CONTROL(unit)->schanTimeout));

    /* Clear SCHAN_ERR */
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH0_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH1_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH2_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH3_ERRr(unit, 0));
    SHR_IF_ERR_EXIT(WRITE_CMIC_COMMON_POOL_SCHAN_CH4_ERRr(unit, 0));

    /*MDIO configuration*/
    SHR_IF_ERR_EXIT(soc_ramon_drv_mdio_config_set(unit));
    
exit:
    SHR_FUNC_EXIT;
}


/*********************************************************************
*     Enable Dynamic Memory Access
*********************************************************************/
STATIC int
soc_ramon_drv_dynamic_mem_access_enable(int unit)
{
    int reg_index;
    uint32 reg_val32;

    soc_reg_t dyn_mem_access_reg[SOC_RAMON_DRV_NOF_DYN_MEM_ACCESS_REGS] = {BRDC_DCML_ENABLE_DYNAMIC_MEMORY_ACCESSr, BRDC_FSRD_ENABLE_DYNAMIC_MEMORY_ACCESSr, BRDC_QRH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
                                                                           ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,       MCT_ENABLE_DYNAMIC_MEMORY_ACCESSr,       RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr };

    SHR_FUNC_INIT_VARS(unit);

    for (reg_index = 0 ; reg_index < SOC_RAMON_DRV_NOF_DYN_MEM_ACCESS_REGS ; reg_index++)
    {
        reg_val32 = 0;
        soc_reg_field_set(unit, dyn_mem_access_reg[reg_index], &reg_val32, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, dyn_mem_access_reg[reg_index], REG_PORT_ANY, 0, reg_val32));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_drv_blocks_reset
 * Purpose:
 *      RAMON reset blocks
 * Parameters:
 *      unit                          - (IN)  Unit number.
 *      force_blocks_reset_value      - (IN)  if 0 - reset all blocks. otherwise use  block_bitmap
 *      block_bitmap                  - (IN)  which blocks to reset.
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
int
soc_ramon_drv_blocks_reset(int unit, int force_blocks_reset_value, soc_reg_above_64_val_t *block_bitmap) 
{
    soc_reg_above_64_val_t reg_above_64;
    SHR_FUNC_INIT_VARS(unit);

    if (force_blocks_reset_value && (block_bitmap != NULL))
    {
        SOC_REG_ABOVE_64_COPY(reg_above_64, *block_bitmap);
    } else {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64);
    }
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64));
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64));
    SOC_REG_ABOVE_64_CLEAR(reg_above_64);
    sal_usleep(40000);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SBUS_RESETr(unit, reg_above_64));
    sal_usleep(40000);
    SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_RESETr(unit, reg_above_64));
    sal_usleep(40000);

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_device_mode_set(int unit, soc_dnxf_fabric_device_mode_t fabric_device_mode)
{
    uint32 reg32_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_ECI_FE_GLOBAL_GENERAL_CFG_1r(unit, &reg32_val));
        
    /*FE13*/
    soc_reg_field_set(unit, ECI_FE_GLOBAL_GENERAL_CFG_1r, &reg32_val, FE_13_MODEf,
                          (fabric_device_mode == soc_dnxf_fabric_device_mode_multi_stage_fe13) ? 1 : 0);

    /*Multistage*/
    soc_reg_field_set(unit, ECI_FE_GLOBAL_GENERAL_CFG_1r, &reg32_val, MULTISTAGE_MODEf,
                           (fabric_device_mode == soc_dnxf_fabric_device_mode_multi_stage_fe2 ||
                           fabric_device_mode == soc_dnxf_fabric_device_mode_multi_stage_fe13) ? 1 : 0);

    SHR_IF_ERR_EXIT(WRITE_ECI_FE_GLOBAL_GENERAL_CFG_1r(unit, reg32_val));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_load_balancing_set(int unit, soc_dnxf_load_balancing_mode_t load_balancing)
{
    uint64 reg_val64;
    soc_reg_above_64_val_t reg_val_above_64;
    uint32 load_balancing_val;
    int i, nof_qrh;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val_above_64));

    load_balancing_val = soc_dnxf_load_balancing_mode_normal == load_balancing ? 1 : 0;
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val_above_64, DISABLE_RCG_LOAD_BALANCINGf, load_balancing_val);

    load_balancing_val = (soc_dnxf_load_balancing_mode_destination_unreachable == load_balancing) ? 0 : 1;
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val_above_64, SCT_SCRUB_ENABLEf, load_balancing_val);
                            
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_val_above_64));

    /*link load load-balancing*/
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (i = 0; i < nof_qrh; ++i) {
        SHR_IF_ERR_EXIT(soc_reg_get(unit, QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr, i, 0, &reg_val64));
        soc_reg64_field32_set(unit, QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr, &reg_val64, LOAD_BALANCE_LEVELS_IGNOREf, 0);
        SHR_IF_ERR_EXIT(soc_reg_set(unit, QRH_DRH_LOAD_BALANCING_LEVEL_CONFIGr, i, 0, reg_val64));
    }
exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_multicast_config_set(int unit, soc_dnxf_multicast_mode_t mc_mode, int fe_mc_priority_map_enable)
{
    int i, nof_qrh;
    uint32 reg_val32;
    uint32 multicast_mode_field_val;
    soc_dnxf_multicast_table_mode_t multicast_mode;
    uint64 reg_val64;
    SHR_FUNC_INIT_VARS(unit);

    for (i=0 ; i < dnxf_data_device.blocks.nof_instances_dch_get(unit) ; i++)
    {
        SHR_IF_ERR_EXIT(READ_DCH_PRIORITY_TRANSLATIONr(unit, i, &reg_val32));
        soc_reg_field_set(unit, DCH_PRIORITY_TRANSLATIONr, &reg_val32, LOW_PRE_MUL_SETf,
                          dnxf_data_fabric.multicast.priority_map_enable_get(unit) ? 1 : 0);
        soc_reg_field_set(unit, DCH_PRIORITY_TRANSLATIONr, &reg_val32, MID_PRE_MUL_SETf,
                          dnxf_data_fabric.multicast.priority_map_enable_get(unit) ? 1 : 0);
        SHR_IF_ERR_EXIT(WRITE_DCH_PRIORITY_TRANSLATIONr(unit, i, reg_val32));

    }
    if (dnxf_data_fabric.multicast.priority_map_enable_get(unit))
    {
        COMPILER_64_ZERO(reg_val64);
        SHR_IF_ERR_EXIT(WRITE_BRDC_DCH_USE_MC_CELL_PRIO_BMPr(unit, reg_val64));
    }
    

    /*MC mode: direct / indirect*/
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (i = 0; i < nof_qrh; i++) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_MULTICAST_MODE_SELECTIONr, i, 0, &reg_val32));
        soc_reg_field_set(unit, QRH_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_INDIRECT_LIST_OF_FAPS_MODEf,
                                soc_dnxf_multicast_mode_indirect == mc_mode ? 1 : 0);
        soc_reg_field_set(unit, QRH_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_MULTIPLE_PIPESf,
                                soc_dnxf_multicast_mode_indirect == mc_mode ? 1 : 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_MULTICAST_MODE_SELECTIONr, i, 0, reg_val32));
    }
    
    /* configure according to multicast table mode */
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_mode_get, (unit, &multicast_mode)));
    switch (multicast_mode)
    {
        case soc_dnxf_multicast_table_mode_64k:
            multicast_mode_field_val = 0; /* 000 */
            break;
        case soc_dnxf_multicast_table_mode_128k:
            multicast_mode_field_val = 1; /* 001 */
            break;
        case soc_dnxf_multicast_table_mode_128k_half:
            multicast_mode_field_val = 2; /* 010 */
            break;
        case soc_dnxf_multicast_table_mode_256k:
            multicast_mode_field_val = 3 ; /* 011 */
            break;
        case soc_dnxf_multicast_table_mode_256k_half:
            multicast_mode_field_val = 4 ; /* 100 */
            break;
        case soc_dnxf_multicast_table_mode_512k_half:
            multicast_mode_field_val = 5 ;/* 101 */
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong mc_table_mode value %d",
                         dnxf_data_fabric.multicast.id_range_get(unit));
    }

    SHR_IF_ERR_EXIT(READ_RTP_MULTICAST_MODE_SELECTIONr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_MULTICAST_MODE_SELECTIONr, &reg_val32, MC_TABLE_MODEf, multicast_mode_field_val);
    SHR_IF_ERR_EXIT(WRITE_RTP_MULTICAST_MODE_SELECTIONr(unit, reg_val32));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_local_routing_set(int unit, int local_routing_enable)
{
    int i, nof_qrh, nof_lcm;
    uint32 reg_val32;

    SHR_FUNC_INIT_VARS(unit);

    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for (i = 0; i < nof_qrh/2; i++) {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, i, 0, &reg_val32));
        /* Unicast */
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, ENABLE_LOCAL_FE_1_ROUTINGf, local_routing_enable == 1 ? 0xf : 0);
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, ENABLE_CTRL_LOCAL_FE_1_ROUTINGf, 0x1); /*Always allow local routing for control cells*/
        /* Multicast */
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, ENABLE_MC_LOCAL_FE_1_ROUTINGf, 1);
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, MC_FORCE_LOCAL_ENf, 1);
        soc_reg_field_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, MC_LOCAL_ONLYf, 0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_LOCAL_ROUTE_CONFIGURATIONSr, i, 0, reg_val32));
    }

    SHR_IF_ERR_EXIT(READ_RTP_LOCAL_ROUTE_CONFIGURATIONSr(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_LOCAL_ROUTE_CONFIGURATIONSr, &reg_val32, ENABLE_REACH_LOCAL_FE_1_ROUTINGf, local_routing_enable == 1 ? 0x1 : 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_LOCAL_ROUTE_CONFIGURATIONSr(unit, reg_val32));

    nof_lcm = dnxf_data_device.blocks.nof_instances_lcm_get(unit);
    for (i=0 ; i < nof_lcm/2 ; i++ )
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_DTML_ENABLERSr, i, 0, &reg_val32));
        soc_reg_field_set(unit, LCM_DTML_ENABLERSr, &reg_val32, LCLRT_ENf, local_routing_enable == 1 ? 1:0);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_DTML_ENABLERSr, i, 0, reg_val32));
    }

    /* Enable GCI generation from DFL, DTM & DTL for MC local-route. Used only in FE13, LCM0-3 (lower links) */
    SHR_IF_ERR_EXIT(READ_LCM_LCM_GCI_ENABLERSr(unit, 0, &reg_val32));
    soc_reg_field_set(unit, LCM_LCM_GCI_ENABLERSr, &reg_val32, GCI_LOCAL_MC_ENf, 1);
    for (i=0 ; i < nof_lcm/2 ; i++)
    {
        SHR_IF_ERR_EXIT(WRITE_LCM_LCM_GCI_ENABLERSr(unit, i, reg_val32));
    }

    /* Enable generation of RTP-copies-FC for MC-local-route. Used only in FE13, LCM0-3 (lower links) */
    SHR_IF_ERR_EXIT(READ_LCM_LCM_FC_ENABLERSr(unit, 0, &reg_val32));
    soc_reg_field_set(unit, LCM_LCM_FC_ENABLERSr, &reg_val32, RTP_COPIES_FC_LCL_RT_MC_ENf, 1);
    for (i=0 ; i < nof_lcm/2 ; i++)
    {
        SHR_IF_ERR_EXIT(WRITE_LCM_LCM_FC_ENABLERSr(unit, i, reg_val32));
    }


exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_cell_priority_config_set(int unit, uint32 min_tdm_priority)
{
    uint32 reg32_val;
    uint32 tdm_bmp;
    int dch_instance;
    SHR_FUNC_INIT_VARS(unit);

    /*TDM priority*/
    for (dch_instance = 0; dch_instance < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_instance++)
    {
        tdm_bmp = 0;
        SHR_IF_ERR_EXIT(READ_DCH_TDM_PRIORITYr(unit, dch_instance, &reg32_val));
        if (min_tdm_priority == SOC_DNXF_FABRIC_TDM_PRIORITY_NONE) 
        {
            /*Disable tdm priority*/
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMP_ENf, 0x0); 
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMPf, tdm_bmp);
        } else {
            /*Mark tdm priorties*/
            SHR_BITSET_RANGE(&tdm_bmp, min_tdm_priority, soc_dnxf_fabric_priority_nof - min_tdm_priority);
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMP_ENf, 0x1); 
            soc_reg_field_set(unit, DCH_TDM_PRIORITYr, &reg32_val, TDM_PRI_BMPf, tdm_bmp);
        }
        SHR_IF_ERR_EXIT(WRITE_DCH_TDM_PRIORITYr(unit, dch_instance, reg32_val));
    }


exit:
    SHR_FUNC_EXIT;
}

STATIC int
_soc_ramon_drv_fabric_pipes_config_set(int unit)
{
    uint32 reg32_val;
    int rv, link;
    int dch_instance, dcml_instance, lcm_instance;
    int fmac_instance, inner_link;
    int pipe_index, prio_index;
    int cast_max, priority_max;
    uint64 reg64_val;
    soc_field_t dch_cast_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {AUTO_DOC_NAME_18f, AUTO_DOC_NAME_20f, AUTO_DOC_NAME_22f};
    soc_field_t dch_priority_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {AUTO_DOC_NAME_19f, AUTO_DOC_NAME_21f, AUTO_DOC_NAME_23f};
    soc_field_t lcm_cast_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {FIELD_0_0f, FIELD_3_3f, FIELD_6_6f};
    soc_field_t lcm_priority_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {FIELD_1_2f, FIELD_4_5f, FIELD_7_8f};
    soc_field_t dcml_cast_fields[DNXF_DATA_MAX_FABRIC_PIPES_MAX_NOF_PIPES] = {FIELD_0_0f, FIELD_1_1f, FIELD_2_2f};
    SHR_FUNC_INIT_VARS(unit);

    /* 
     *Set number of pipes 
     */

    /*ECI*/
    rv = READ_ECI_FE_GLOBAL_GENERAL_CFG_1r(unit,&reg32_val);
    SHR_IF_ERR_EXIT(rv);
    soc_reg_field_set(unit, ECI_FE_GLOBAL_GENERAL_CFG_1r, &reg32_val, PIPE_1_ENf,
                      dnxf_data_fabric.pipes.nof_pipes_get(unit) > 1);
    soc_reg_field_set(unit, ECI_FE_GLOBAL_GENERAL_CFG_1r, &reg32_val, PIPE_2_ENf,
                      dnxf_data_fabric.pipes.nof_pipes_get(unit) > 2);
    rv = WRITE_ECI_FE_GLOBAL_GENERAL_CFG_1r(unit, reg32_val);
    SHR_IF_ERR_EXIT(rv);

    /*FMAC*/
    PBMP_SFI_ITER(unit, link)
    {
        fmac_instance = link / dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
        inner_link = link % dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
        rv  = READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link , &reg32_val);
        SHR_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PARALLEL_DATA_PATHf,
                          dnxf_data_fabric.pipes.nof_pipes_get(unit) - 1);
        rv = WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link, reg32_val);
        SHR_IF_ERR_EXIT(rv);
    }

     /*DCH*/ COMPILER_64_SET(reg64_val, 0, 0);
    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 2)
    {
        COMPILER_64_MASK_CREATE(reg64_val, dnxf_data_device.blocks.nof_links_in_dch_get(unit), 0);
    }
    rv = WRITE_BRDC_DCH_TWO_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);

    COMPILER_64_SET(reg64_val, 0, 0);
    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 3)
    {
        COMPILER_64_MASK_CREATE(reg64_val, dnxf_data_device.blocks.nof_links_in_dch_get(unit), 0);
    }
    rv = WRITE_BRDC_DCH_THREE_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);

    /*DCML / LCM*/
    COMPILER_64_SET(reg64_val, 0, 0);
    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 2)
    {
        COMPILER_64_MASK_CREATE(reg64_val, dnxf_data_device.blocks.nof_links_in_dcml_get(unit), 0);
    }
    rv = WRITE_BRDC_DCML_TWO_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);	
    rv = WRITE_BRDC_LCM_TWO_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);

    COMPILER_64_SET(reg64_val, 0, 0);
    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 3)
    {
        COMPILER_64_MASK_CREATE(reg64_val, dnxf_data_device.blocks.nof_links_in_dcml_get(unit), 0);
    }
    rv = WRITE_BRDC_DCML_THREE_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);	
    rv = WRITE_BRDC_LCM_THREE_PIPES_BMPr(unit, reg64_val);
    SHR_IF_ERR_EXIT(rv);

    /* 
     * Pipe Mapping
     */
    /*DCH*/
    for (dch_instance = 0; dch_instance < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_instance++)
    {
        rv = READ_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, &reg32_val);
        SHR_IF_ERR_EXIT(rv);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_0_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[0]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_1_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[1]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_2_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[2]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_3_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[3]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_0_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[0]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_1_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[1]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_2_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[2]);
        soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_3_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[3]);
        rv = WRITE_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, reg32_val);
        SHR_IF_ERR_EXIT(rv);
    }

    /*DCML (Registers in LCM block)*/
    for (lcm_instance = 0; lcm_instance < dnxf_data_device.blocks.nof_instances_lcm_get(unit); lcm_instance++)
    {

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_PIPES_SEPARATION_REGISTERr, lcm_instance, 0, &reg32_val));
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_0_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[0]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_1_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[1]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_2_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[2]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, UC_PRI_3_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->uc[3]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_0_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[0]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_1_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[1]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_2_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[2]);
        soc_reg_field_set(unit, LCM_PIPES_SEPARATION_REGISTERr, &reg32_val, MC_PRI_3_PIPEf,
                          dnxf_data_fabric.pipes.map_get(unit)->mc[3]);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_PIPES_SEPARATION_REGISTERr, lcm_instance, 0, reg32_val));
    }

    /*
     *  configure cast/priority per pipe for source routed cells to be considered in CGM
     *  cast/priority is the highest available in pipe
     */
    for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
    {

        priority_max = cast_max = 0;

        for (prio_index=(SOC_DNXF_MAX_NUM_OF_PRIORITIES - 1); prio_index >= 0  ; prio_index--)
        {
            if (pipe_index == dnxf_data_fabric.pipes.map_get(unit)->mc[prio_index])
            {
                cast_max = 1;
                priority_max = prio_index;
                break;
            }
            else if (pipe_index == dnxf_data_fabric.pipes.map_get(unit)->uc[prio_index])
            {
                cast_max = 0;
                priority_max = prio_index;
                break;
            }
        }

        for (dch_instance = 0; dch_instance < dnxf_data_device.blocks.nof_instances_dch_get(unit); dch_instance++)
        {
            rv = READ_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, &reg32_val);
            SHR_IF_ERR_EXIT(rv);
            soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, dch_cast_fields[pipe_index], cast_max);
            soc_reg_field_set(unit, DCH_PIPES_SEPARATION_REGISTERr, &reg32_val, dch_priority_fields[pipe_index], priority_max);
            rv = WRITE_DCH_PIPES_SEPARATION_REGISTERr(unit, dch_instance, reg32_val);
            SHR_IF_ERR_EXIT(rv);
        }

        for (lcm_instance = 0; lcm_instance < dnxf_data_device.blocks.nof_instances_lcm_get(unit); lcm_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_DCML_SEPARATION_REGISTERr, lcm_instance, 0, &reg32_val));
            soc_reg_field_set(unit, LCM_DCML_SEPARATION_REGISTERr, &reg32_val, lcm_cast_fields[pipe_index], cast_max);
            soc_reg_field_set(unit, LCM_DCML_SEPARATION_REGISTERr, &reg32_val, lcm_priority_fields[pipe_index], priority_max);
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_DCML_SEPARATION_REGISTERr, lcm_instance, 0, reg32_val));
        }

        for (dcml_instance = 0; dcml_instance < dnxf_data_device.blocks.nof_instances_dcml_get(unit); dcml_instance++)
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, DCML_DCML_SEPARATION_REGISTERr, dcml_instance, 0, &reg32_val));
            soc_reg_field_set(unit, DCML_DCML_SEPARATION_REGISTERr, &reg32_val, dcml_cast_fields[pipe_index], cast_max);
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, DCML_DCML_SEPARATION_REGISTERr, dcml_instance, 0, reg32_val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_operation_mode(int unit)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    /* 
     * Device Mode Configuration
     */
    rv = _soc_ramon_drv_fabric_device_mode_set(unit, dnxf_data_fabric.general.device_mode_get(unit));
    SHR_IF_ERR_EXIT(rv);

     /*
     * Fabric pipes configuration
     */
    rv = _soc_ramon_drv_fabric_cell_priority_config_set(unit, dnxf_data_fabric.pipes.system_tdm_priority_get(unit));
    SHR_IF_ERR_EXIT(rv);

    /*
     * Fabric pipes configuration
     */
    rv = _soc_ramon_drv_fabric_pipes_config_set(unit);
    SHR_IF_ERR_EXIT(rv);

    /*
     * Multicast mode configuration
     */
    rv = _soc_ramon_drv_fabric_multicast_config_set(unit, dnxf_data_fabric.multicast.mode_get(unit),
                                                    dnxf_data_fabric.multicast.priority_map_enable_get(unit));
    SHR_IF_ERR_EXIT(rv);

    /*
     * Load Balancing
     */
    rv = _soc_ramon_drv_fabric_load_balancing_set(unit, dnxf_data_fabric.topology.load_balancing_mode_get(unit));
    SHR_IF_ERR_EXIT(rv);

    if (SOC_DNXF_IS_FE13(unit))
    {
        rv = _soc_ramon_drv_fabric_local_routing_set(unit, dnxf_data_fabric.general.local_routing_enable_get(unit));
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

#define _SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS                  (96)

#define _SOC_RAMON_DRV_MULTIPLIER_T1_LENGTH                        (64)
#define _SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH                         (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T1_MAX_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH)

#define _SOC_RAMON_DRV_MULTIPLIER_T1_AND_T2_SIZE                   (_SOC_RAMON_DRV_MULTIPLIER_T1_LENGTH * _SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH)

#define _SOC_RAMON_DRV_MULTIPLIER_T2_LENGTH                        (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T2_WIDTH                         (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T2_MIN_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T1_MAX_ACTIVE_LINKS + 1)
#define _SOC_RAMON_DRV_MULTIPLIER_T2_MAX_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T2_MIN_ACTIVE_LINKS + _SOC_RAMON_DRV_MULTIPLIER_T2_WIDTH - 1)
#define _SOC_RAMON_DRV_MULTIPLIER_T2_MAX_TOTAL_LINKS               (_SOC_RAMON_DRV_MULTIPLIER_T2_MAX_ACTIVE_LINKS)

#define _SOC_RAMON_DRV_MULTIPLIER_T3_LENGTH                        (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T3_WIDTH                         (64)
#define _SOC_RAMON_DRV_MULTIPLIER_T3_MIN_TOTAL_LINKS               (_SOC_RAMON_DRV_MULTIPLIER_T2_MAX_TOTAL_LINKS + 1)
#define _SOC_RAMON_DRV_MULTIPLIER_T3_MAX_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T2_MAX_ACTIVE_LINKS)
#define _SOC_RAMON_DRV_MULTIPLIER_T3_SIZE                          (_SOC_RAMON_DRV_MULTIPLIER_T3_LENGTH * _SOC_RAMON_DRV_MULTIPLIER_T3_WIDTH)

#define _SOC_RAMON_DRV_MULTIPLIER_T4_LENGTH                        (32)
#define _SOC_RAMON_DRV_MULTIPLIER_T4_WIDTH                         (16)
#define _SOC_RAMON_DRV_MULTIPLIER_T4_MIN_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T3_MAX_ACTIVE_LINKS + 1)
#define _SOC_RAMON_DRV_MULTIPLIER_T4_MAX_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T4_MIN_ACTIVE_LINKS + _SOC_RAMON_DRV_MULTIPLIER_T4_WIDTH)

#define _SOC_RAMON_DRV_MULTIPLIER_T5_LENGTH                        (16)
#define _SOC_RAMON_DRV_MULTIPLIER_T5_WIDTH                         (16)
#define _SOC_RAMON_DRV_MULTIPLIER_T5_MIN_ACTIVE_LINKS              (_SOC_RAMON_DRV_MULTIPLIER_T4_MAX_ACTIVE_LINKS + 1)

/* Number of offsets in RTP_MULTI_TB entry */
#define _SOC_RAMON_DRV_MULTIPLIER_NOF_OFFSETS_IN_ENTRY             (4)

#define _SOC_RAMON_MULTIPLIER_TABLE_DUMP 0

STATIC int
soc_ramon_clean_rtp_table_array(int unit, soc_mem_t mem, soc_reg_above_64_val_t data)
{
    uint32 reg_val32;
    unsigned i, elem_num;
    uint32 current_address, element_skip;        /* current address in the array, and its skip */
    uint64  val64;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_MEM_IS_ARRAY_SAFE(unit,mem))
    {
        elem_num = SOC_MEM_NUMELS(unit, mem);
        element_skip = SOC_MEM_ELEM_SKIP(unit, mem);
    }
    else
    {
        elem_num = 1;
        element_skip = 0;
    }
    current_address = SOC_MEM_INFO(unit, mem).base;

    for (i = 0; i < elem_num; ++i) { /* loop over all the elements of the array */
        SHR_IF_ERR_EXIT(WRITE_RTP_INDIRECT_COMMAND_WR_DATAr(unit, data));
        COMPILER_64_ZERO(val64);
        SHR_IF_ERR_EXIT(WRITE_RTP_INDIRECT_COMMAND_DATA_INCREMENTr(unit, val64));

        reg_val32 = 0;
        soc_reg_field_set(unit, RTP_INDIRECT_COMMAND_ADDRESSr, &reg_val32, INDIRECT_COMMAND_ADDRf, current_address);

        soc_reg_field_set(unit, RTP_INDIRECT_COMMAND_ADDRESSr, &reg_val32, INDIRECT_COMMAND_TYPEf, 0 /*write command*/);
        SHR_IF_ERR_EXIT(WRITE_RTP_INDIRECT_COMMAND_ADDRESSr(unit, reg_val32));

        reg_val32 = 0;
        soc_reg_field_set(unit, RTP_INDIRECT_COMMANDr, &reg_val32, INDIRECT_COMMAND_COUNTf, SOC_MEM_INFO(unit, mem).index_max+1);
        soc_reg_field_set(unit, RTP_INDIRECT_COMMANDr, &reg_val32, INDIRECT_COMMAND_TRIGGERf, 1);
        SHR_IF_ERR_EXIT(WRITE_RTP_INDIRECT_COMMANDr(unit, reg_val32));

        current_address += element_skip;
    }

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_clean_qrh_table_array(int unit, soc_mem_t mem, soc_reg_above_64_val_t data)
{
    uint32 reg_val32;
    unsigned i, j, elem_num;
    uint32 current_address, element_skip;        /* current address in the array, and its skip */
    uint64  val64;
    int nof_qrh;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_MEM_IS_ARRAY_SAFE(unit,mem))
    {
        elem_num = SOC_MEM_NUMELS(unit, mem);
        element_skip = SOC_MEM_ELEM_SKIP(unit, mem);
    }
    else
    {
        elem_num = 1;
        element_skip = 0;
    }
    current_address = SOC_MEM_INFO(unit, mem).base;
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);

    for (i = 0; i < nof_qrh; ++i) {
        /* loop over all the elements of the array - NUMELS */
        for (j = 0; j < elem_num; ++j) {
            /* Data to write in case IndirectCommand is triggered */
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, QRH_INDIRECT_COMMAND_WR_DATAr, i, 0, data));

            /* IndirectCommandWrData is incremented by this value after every indirect write - set to 0, e.g. all entries will have the same value */
            COMPILER_64_ZERO(val64);
            SHR_IF_ERR_EXIT(soc_reg_set(unit, QRH_INDIRECT_COMMAND_DATA_INCREMENTr, i, 0, val64));

            reg_val32 = 0;
            /* Indirect access address - Defines which indirect object is accessed */
            soc_reg_field_set(unit, QRH_INDIRECT_COMMAND_ADDRESSr, &reg_val32, INDIRECT_COMMAND_ADDRf, current_address);

            /* write command */
            soc_reg_field_set(unit, QRH_INDIRECT_COMMAND_ADDRESSr, &reg_val32, INDIRECT_COMMAND_TYPEf, 0);
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_INDIRECT_COMMAND_ADDRESSr, i, 0, reg_val32));

            reg_val32 = 0;
            /* Each write command is executed IndirectCommandCount number of times - MAXIDX+1 */
            soc_reg_field_set(unit, QRH_INDIRECT_COMMANDr, &reg_val32, INDIRECT_COMMAND_COUNTf, SOC_MEM_INFO(unit, mem).index_max+1);
            /* If set indirect access is triggered */
            soc_reg_field_set(unit, QRH_INDIRECT_COMMANDr, &reg_val32, INDIRECT_COMMAND_TRIGGERf, 1);
            SHR_IF_ERR_EXIT(soc_reg32_set(unit, QRH_INDIRECT_COMMANDr, i, 0, reg_val32));

            current_address += element_skip;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_clean_table_slow(int unit, soc_mem_t mem, soc_reg_above_64_val_t data)
{
    int index, index_max;
    SHR_FUNC_INIT_VARS(unit);

    index_max = soc_mem_index_max(unit, mem);

    for (index = 0; index <= index_max; index++) {
        SHR_IF_ERR_EXIT(soc_mem_write(unit, mem, MEM_BLOCK_ALL, index, data));
    }

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_reset_tables(int unit)
{
    soc_reg_above_64_val_t data;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 total_links, active_links, score, entry;
    uint32 bmp[DNXF_DATA_MAX_FABRIC_REACHABILITY_TABLE_ROW_SIZE_IN_UINT32];
    uint32 totsf_val, slsct_val, score_slsct, links_count, sctinc_val, sctinc;
    soc_field_t scrub_en;
    soc_field_t calc_result_field_arr[_SOC_RAMON_DRV_MULTIPLIER_NOF_OFFSETS_IN_ENTRY] = {CALC_RESULT_0f, CALC_RESULT_1f, CALC_RESULT_2f, CALC_RESULT_3f};
    int link, nof_qrh;
    uint64 data64;
    int tbl_index = 0;
    int field_offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    scrub_en = soc_reg_above_64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, SCT_SCRUB_ENABLEf);
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, SCT_SCRUB_ENABLEf, 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

    SOC_REG_ABOVE_64_CLEAR(data);
    /*MCLBT*/
    SHR_IF_ERR_EXIT(soc_ramon_clean_qrh_table_array(unit, QRH_MCLBTm, data));
    /*MNOL*/
    SHR_IF_ERR_EXIT(soc_ramon_clean_qrh_table_array(unit, QRH_MNOLm, data));
    /*RCGLBT*/
    SHR_IF_ERR_EXIT(soc_ramon_clean_rtp_table_array(unit, RTP_RCGLBTm, data));
    /*TOTSF*/
    SHR_IF_ERR_EXIT(soc_ramon_clean_rtp_table_array(unit, RTP_TOTSFm, data));
    /*SLSCT*/
    SHR_IF_ERR_EXIT(soc_ramon_clean_rtp_table_array(unit, RTP_SLSCTm, data));
    /*SCTINC*/
    SHR_IF_ERR_EXIT(soc_ramon_clean_rtp_table_array(unit, RTP_SCTINCm, data));

    /*MNOLG*/
    SHR_IF_ERR_EXIT(soc_ramon_clean_table_slow(unit, RTP_MNOLGm, data));
    /*FLGM*/
    SHR_IF_ERR_EXIT(soc_ramon_clean_table_slow(unit, RTP_FLGMm, data));
    /*GCILBT*/
    SHR_IF_ERR_EXIT(soc_ramon_clean_table_slow(unit, RTP_GCILBTm, data));

    /* nof links towards the same FAP/group */
    totsf_val = 0;
    links_count = 1;
    soc_mem_field_set(unit, RTP_TOTSFm, &totsf_val, TOTAL_LINKSf, (uint32*)&links_count);

    /* score of each link */
    slsct_val = 0;
    score_slsct = 0;
    soc_mem_field_set(unit, RTP_SLSCTm, &slsct_val, SCORE_OF_LINKf, &score_slsct);

    /* delta between consequent link scores */
    sctinc_val = 0;
    sctinc = 0;
    soc_mem_field_set(unit, RTP_SCTINCm, &sctinc_val, SCORE_TO_INCf, &sctinc);
    
    nof_qrh = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
    for(link = 0 ; link < dnxf_data_port.general.nof_links_get(unit) ; link++) {
        /*build bitmap*/
        sal_memset(bmp, 0, sizeof(bmp));
        SHR_BITSET(bmp,link);

        SHR_IF_ERR_EXIT(WRITE_RTP_RCGLBTm(unit, MEM_BLOCK_ALL, link, bmp));
        SHR_IF_ERR_EXIT(WRITE_RTP_TOTSFm(unit, MEM_BLOCK_ALL, link, &totsf_val));
        SHR_IF_ERR_EXIT(WRITE_RTP_SLSCTm(unit, MEM_BLOCK_ALL, link, &slsct_val));
        SHR_IF_ERR_EXIT(WRITE_RTP_SCTINCm(unit, MEM_BLOCK_ALL, link, &sctinc_val));
    }

    /* 
     * 96X96 Triangle folded to table at size 1152:
     * Triangle size = 96*96/2
     * Each entry in table holds 4 offsets --> result table = (triangle size) / 4 = 1152
     *      +
     *      |\
     *      | \
     *      |  \
     *      |   \
     *      |   |\
     *      |   | \
     *      |   |  \
     *      |T1 |T2 \
     *      +---+----+
     *      |        |\
     *      |        | \
     *      |  T3    |  \
     *      |        |T4|\ 
     *      |        |  | \ <- T5
     *      +---+---------+
     *  
     *  
     */
    /*
     * Original calc
     * Folded address = 
     *  (Num of active links > 80) && (total links to same fap > 80) ?
     *      2048+(96%64)*64 + (96-total links to same fap)%32)*16 + ((96-Num of active links)%16) :
     *      (Num of active links > 64) && (total links to same fap > 64) ?
     *          2048 + (96%64)*64 + ((total links to same fap - 1)%32)*16 + ((Num of active links - 1)%16) :
     *          (Num of active links <= 64) && (total links to same fap > 64) ?
     *              (2048 + ((total links to same fap - 1)%32)*64 + (Num of active links - 1) :
     *              (33 <= Num of active links <= 64) && (33 <= total links to same fap <= 64) ?
     *                  (((64- total_links_to_same_fap )*32) + (64 - num_of_active_links)) :
     *                  (Num of active links < 33) && (total links to same fap <= 64) ?
     *                      (((total links to same fap - 1)*32 + (Num of active links - 1)
     */

#if _SOC_RAMON_MULTIPLIER_TABLE_DUMP
    LOG_CLI((BSL_META_U(unit,
                        "MUL TABLE\n\n")));

    LOG_CLI((BSL_META_U(unit,
                        "%02d || "), 0));
    for(active_links = 1 ; active_links <= _SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS ; active_links++) {
        LOG_CLI((BSL_META_U(unit,
                            "%04d | "), active_links));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n\n")));
#endif
    for(total_links = 1 ; total_links <= _SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS ; total_links++) {

#if _SOC_RAMON_MULTIPLIER_TABLE_DUMP
        LOG_CLI((BSL_META_U(unit,
                            "%02d || "), total_links));
#endif
        for(active_links = 1 ; active_links <= total_links ; active_links++) {
            /* Calculate Score */
            score = UTILEX_DIV_ROUND_UP((SOC_DNXF_DRV_MULTIPLIER_MAX_LINK_SCORE * active_links), total_links);

            /* Calculate Folded Address */
            /* 81 <= active_links < 97 && 81 <= total_links < 97 */
            if (active_links >= _SOC_RAMON_DRV_MULTIPLIER_T5_MIN_ACTIVE_LINKS)
            {
                /* T5 entries */
                entry = _SOC_RAMON_DRV_MULTIPLIER_T1_AND_T2_SIZE
                    + _SOC_RAMON_DRV_MULTIPLIER_T3_SIZE
                    + ((_SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS - total_links) * _SOC_RAMON_DRV_MULTIPLIER_T5_WIDTH)
                    + (_SOC_RAMON_DRV_MULTIPLIER_TABLE_MAX_LINKS - active_links);
            }
            /* 65 <= active_links < 81 && 65 <= total_links < 97 */
            else if (active_links >= _SOC_RAMON_DRV_MULTIPLIER_T4_MIN_ACTIVE_LINKS)
            {
                /* T4 entries */
                entry = _SOC_RAMON_DRV_MULTIPLIER_T1_AND_T2_SIZE
                    + _SOC_RAMON_DRV_MULTIPLIER_T3_SIZE
                    + ((total_links - 1) % _SOC_RAMON_DRV_MULTIPLIER_T4_LENGTH) * _SOC_RAMON_DRV_MULTIPLIER_T4_WIDTH
                    + ((active_links - 1) % _SOC_RAMON_DRV_MULTIPLIER_T4_WIDTH);
            }
            /* active_links < 65 && 65 <= total_links < 97 */
            else if (total_links >= _SOC_RAMON_DRV_MULTIPLIER_T3_MIN_TOTAL_LINKS)
            {
                /* T3 entries */
                entry = _SOC_RAMON_DRV_MULTIPLIER_T1_AND_T2_SIZE
                    + ((total_links - 1) % _SOC_RAMON_DRV_MULTIPLIER_T3_LENGTH) * _SOC_RAMON_DRV_MULTIPLIER_T3_WIDTH
                    + (active_links - 1);
            }
            /* 33 <= active_links < 65 && 33 <= total_links < 65 */
            else if (active_links >= _SOC_RAMON_DRV_MULTIPLIER_T2_MIN_ACTIVE_LINKS)
            {
                /* T2 entries */
                entry = (_SOC_RAMON_DRV_MULTIPLIER_T1_LENGTH - total_links) * _SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH
                    + (_SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH  + _SOC_RAMON_DRV_MULTIPLIER_T2_WIDTH - active_links);
            }
            /* active_links < 33 && total_links < 65 */
            else
            {
                /* T1 entries */
                entry = (total_links - 1) * _SOC_RAMON_DRV_MULTIPLIER_T1_WIDTH + (active_links - 1);
            }

#if _SOC_RAMON_MULTIPLIER_TABLE_DUMP
            LOG_CLI((BSL_META_U(unit,
                                "%04d | "), entry));
#endif
            COMPILER_64_ZERO(data64);
            tbl_index = entry / _SOC_RAMON_DRV_MULTIPLIER_NOF_OFFSETS_IN_ENTRY;
            field_offset = entry % _SOC_RAMON_DRV_MULTIPLIER_NOF_OFFSETS_IN_ENTRY;

            SHR_IF_ERR_EXIT(READ_RTP_MULTI_TBm(unit, MEM_BLOCK_ANY, tbl_index, &data64));
            soc_mem_field32_set(unit, RTP_MULTI_TBm, &data64, calc_result_field_arr[field_offset], score);
            SHR_IF_ERR_EXIT(WRITE_RTP_MULTI_TBm(unit, MEM_BLOCK_ALL, tbl_index, &data64));
        }
#if _SOC_RAMON_MULTIPLIER_TABLE_DUMP
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
#endif
    }
    
    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, SCT_SCRUB_ENABLEf, scrub_en);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit, reg_above64_val));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_fmac_config(int unit)
{
    uint32 reg_val32, field[1];
    uint64  reg_val64;
    soc_reg_above_64_val_t reg_above64_val;
    int i;
    int link, blk, inner_link;
    int fmac_index;
    soc_dnxf_fabric_link_device_mode_t link_mode;
    int nof_links_in_fmac;
    int nof_instances_fmac;
    SHR_FUNC_INIT_VARS(unit);

    nof_instances_fmac = dnxf_data_device.blocks.nof_instances_fmac_get(unit);
    nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);

    /*FMAC Leaky bucket configuration*/
    SHR_IF_ERR_EXIT(soc_reg32_get(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, 0, 0, &reg_val32));
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_FILL_RATE_Nf,
                      dnxf_data_port.general.mac_bucket_fill_rate_get(unit));
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_LINK_UP_TH_Nf, 0x20);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, BKT_LINK_DN_TH_Nf, 0x10);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, SIG_DET_BKT_RST_ENA_Nf, 0x1);
    soc_reg_field_set(unit, FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, &reg_val32, ALIGN_LCK_BKT_RST_ENA_Nf, 0x1);
    for (i = 0 ; i < nof_links_in_fmac ; i++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, BRDC_FMAC_LEAKY_BUCKET_CONTROL_REGISTERr, REG_PORT_ANY, i, reg_val32));
    }

    /*Comma burst configuration*/
    if (SOC_DNXF_IS_FE13(unit))
    {
       PBMP_SFI_ITER(unit, link)
       {
           SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_device_mode_get,(unit, link, 0/*tx*/, &link_mode)));
           SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, link, &blk, &inner_link, SOC_BLK_FMAC)));

           SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, blk, inner_link, &reg_val32));
           soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
           if (link_mode == soc_dnxf_fabric_link_device_mode_multi_stage_fe1)
           {
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE1);
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE1);
           } else {
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE3);
               soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE3);
           }
           SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, blk, inner_link, reg_val32));
       }
    } else {
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, 0, 0, &reg_val32));
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_BYTE_MODEf, 0x1);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_TX_PERIODf, SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE2);
        soc_reg_field_set(unit, FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CM_BRST_SIZEf, SOC_RAMON_PORT_COMMA_BURST_SIZE_FE2);

        for (i = 0 ; i < nof_links_in_fmac ; i++)
        {
            SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAL_TX_COMMA_BURST_CONFIGURATIONr(unit, i, reg_val32));
        }

    }
    
    for (i = 0 ; i < nof_links_in_fmac ; i++)
    {
        SHR_IF_ERR_EXIT(READ_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, REG_PORT_ANY, i, &reg_val64));
        soc_reg64_field_set(unit, FMAC_ASYNC_FIFO_CONFIGURATIONr, &reg_val64 ,AUTO_DOC_NAME_59f, SOC_RAMON_DRV_ASYNC_FIFO_AUTO_DOC_NAME_59);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, i, reg_val64));

        if (SAL_BOOT_PLISIM)
        {
            for (fmac_index = 0 ; fmac_index < nof_instances_fmac ; fmac_index++)
            {
                SHR_IF_ERR_EXIT(WRITE_FMAC_ASYNC_FIFO_CONFIGURATIONr(unit, fmac_index, i, reg_val64));
            }
        }
    }

    for (i = 0 ; i < nof_links_in_fmac ; i++)
    {
        SHR_IF_ERR_EXIT(READ_FMAC_RETIMERr(unit, REG_PORT_ANY, i, reg_above64_val));
        soc_reg_above_64_field32_set(unit, FMAC_RETIMERr, reg_above64_val, FMAL_N_RX_RETIMER_RS_FEC_FEEDBACK_PROCESSOR_ENf, 0);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_RETIMERr(unit, i, reg_above64_val));

        if (SAL_BOOT_PLISIM)
        {
            for (fmac_index = 0 ; fmac_index < nof_instances_fmac ; fmac_index++)
            {
                SHR_IF_ERR_EXIT(WRITE_FMAC_RETIMERr(unit, fmac_index, i, reg_above64_val));
            }
        }
    }

    for (i = 0 ; i < nof_links_in_fmac ; i++)
    {
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr, &reg_val32, FMAL_N_CNTRL_BURST_PERIODf, SOC_RAMON_DRV_BURST_PERIOD);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr(unit, i, reg_val32));

        if (SAL_BOOT_PLISIM)
        {
            for (fmac_index = 0 ; fmac_index < nof_instances_fmac ; fmac_index++)
            {
                SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_CONTROL_BURST_CONFIGURATIONr(unit, fmac_index, i, reg_val32));
            }
        }
    }

    if(SOC_DNXF_IS_FE13(unit)) {

        PBMP_SFI_ITER(unit, link)
        {

            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_device_mode_get,(unit, link, 0/*tx*/, &link_mode)));
            if (link_mode == soc_dnxf_fabric_link_device_mode_multi_stage_fe1)
            {
                SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, link, &blk, &inner_link, SOC_BLK_FMAC)));

                SHR_IF_ERR_EXIT(READ_FMAC_LINK_TOPO_MODE_REG_0r(unit, blk, &reg_val32));
                *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg_val32, LINK_TOPO_MODE_0f);
                SHR_BITSET(field, inner_link);
                soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg_val32, LINK_TOPO_MODE_0f, *field);

                *field = soc_reg_field_get(unit, FMAC_LINK_TOPO_MODE_REG_0r, reg_val32, LINK_TOPO_MODE_1f);
                SHR_BITCLR(field, inner_link);
                soc_reg_field_set(unit, FMAC_LINK_TOPO_MODE_REG_0r, &reg_val32, LINK_TOPO_MODE_1f, *field);

                SHR_IF_ERR_EXIT(WRITE_FMAC_LINK_TOPO_MODE_REG_0r(unit, blk, reg_val32));
            }
        }
    }

    /*Enable llfc by default*/
    reg_val32 = 0;
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32, LNK_LVL_FC_RX_ENf, 0xf);
    soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val32, LNK_LVL_FC_TX_ENf, 0xf);
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, reg_val32));

    /*Enable RX_LOS_SYNC interrupt*/
    for(i=0; i<nof_links_in_fmac; i++) {
        SHR_IF_ERR_EXIT(READ_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, REG_PORT_ANY, i, &reg_val32));
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_LCK_ENf, 0);
        soc_reg_field_set(unit, FMAC_FPS_CONFIGURATION_RX_SYNCr, &reg_val32 ,FPS_N_RX_SYNC_FORCE_SLP_ENf, 0);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FPS_CONFIGURATION_RX_SYNCr(unit, i, reg_val32));
    }

    /*Enable Mac-Tx pump when leaky bucket is down*/
    for(i=0; i<nof_links_in_fmac; i++) {
        SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, REG_PORT_ANY, i, &reg_val64));
        soc_reg64_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg_val64 ,FMAL_N_TX_PUMP_WHEN_LB_DNf, 1);
        SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, i, reg_val64));
    }

    /*Enable interleaving*/
    
    SHR_IF_ERR_EXIT(WRITE_BRDC_FMAC_AUTO_DOC_NAME_12r(unit, 0xf));

exit:
    SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_fsrd_config(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

        
        SHR_FUNC_EXIT;
}

STATIC int
soc_ramon_set_ccs_config(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_set_rtp_config(int unit)
{
    uint32 reg_val32;
    uint64 reg_val64;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 core_clock_speed;
    uint32 rtpwp;
    uint32 gpd_rmgr_global_time = 0;
    uint32 gpd_rmgr_global_in_clocks = 0;
    uint32 gpd_rmgr_global_in_units_clocks = 0;
    uint32 gpd_protect_cnt_th = 0;
    uint32 gpd_protect_cnt_th_in_clocks = 0;
    uint32 gpd_cnt_th = 0;
    uint32 gpd_cnt_th_in_clocks = 0;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg_val64);

    core_clock_speed = dnxf_data_device.general.core_clock_khz_get(unit);

     /*RTPWP CALC*/
    rtpwp = ((dnxf_data_fabric.reachability.watchdog_rate_get(unit)/ 1000)/*micro sec*/ * core_clock_speed /*KHz*/) / 1000 /*convert micro sec to mili */;

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr, &reg_val64, RTPWPf, rtpwp);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_PROCESSOR_CONFIGURATIONr(unit, reg_val64));

    if (SOC_DNXF_IS_FE13(unit)){
        SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_CFGr(unit, &reg_val32));
        soc_reg_field_set(unit, RTP_ALL_REACHABLE_CFGr, &reg_val32, ALRC_ENABLE_SLOW_LINK_DOWNf, 1);
        SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_CFGr(unit, reg_val32));
    }

    SHR_IF_ERR_EXIT(soc_ramon_stk_module_max_fap_set(unit, SOC_RAMON_STK_MAX_MODULE));

    /* Stop reachability generation */
    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit,reg_above64_val));
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGRf, 0);
    soc_reg_above_64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_above64_val, RMGR_GLOBALf, 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr(unit,reg_above64_val));

    /* 
     * Graceful Power Down configurations
     */
    COMPILER_64_ZERO(reg_val64);

    /*
     * full cycle = RMGRGlobal * 32 clocks * clock period
     * => GPD_RMGR = full_cycle / (32 clocks * clock period)
     *
     * The result is multiply by 10^-6 in 2 steps, in order to prevent
     * register overflow.
     */
    gpd_rmgr_global_time = dnxf_data_fabric.reachability.gpd_gen_rate_full_cycle_get(unit);
    gpd_rmgr_global_in_clocks = ((gpd_rmgr_global_time / 100) * core_clock_speed ) / 10000;
    gpd_rmgr_global_in_units_clocks = gpd_rmgr_global_in_clocks / dnxf_data_fabric.reachability.rmgr_units_get(unit);

    /*
     * time[usec] = (threshold[clocks] / core_clock_speed[khz=kclocks/sec]) * 1000
     * => threshold = (time[usec] * core_clock_speed[khz]) / 1000
     */
    gpd_protect_cnt_th = dnxf_data_fabric.gpd.in_time_get(unit);
    gpd_protect_cnt_th_in_clocks = (gpd_protect_cnt_th * core_clock_speed) / 1000;

    /*
     * time[msec] = threshold[clocks] / core_clock_speed[khz=kclocks/sec]
     * => threshold = time[msec] * core_clock_speed[khz]
     */
    gpd_cnt_th = dnxf_data_fabric.gpd.out_time_get(unit);
    gpd_cnt_th_in_clocks = gpd_cnt_th * core_clock_speed;

    SHR_IF_ERR_EXIT(READ_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_ENf, 1);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_RMGRf, gpd_rmgr_global_in_units_clocks);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_PROTECT_ENf, 1);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_PROTECT_CNT_THf, gpd_protect_cnt_th_in_clocks);
    soc_reg64_field32_set(unit, RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr, &reg_val64, GRACEFUL_PD_CNT_THf, gpd_cnt_th_in_clocks);
    SHR_IF_ERR_EXIT(WRITE_RTP_GRACEFUL_POWER_DOWN_CONFIGURATIONr(unit, reg_val64));

exit:
    SHR_FUNC_EXIT;
}

#define SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE3_DEF          (0x1200)
#define SOC_RAMON_DCH_AUTO_DOC_NAME_35_MULTI_FE2_DEF    (0x800)
#define SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE1_FE2_DEF      (0x400)

STATIC int
soc_ramon_set_dch_config(int unit)
{
    uint64 reg_val64;
    uint32 reg_val32;
    int  dch_index, nof_instances_dch;
    int  pipe_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_instances_dch = dnxf_data_device.blocks.nof_instances_dch_get(unit);

    /*ALUWP config*/
    for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
    {
        for (dch_index=0 ; dch_index < nof_instances_dch ; dch_index++) {
            SHR_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_3_Pr(unit, dch_index, pipe_index, &reg_val32));
            soc_reg_field_set(unit, DCH_DCH_ENABLERS_REGISTER_3_Pr, &reg_val32, AUTO_DOC_NAME_12f, 0Xfd);
            SHR_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_3_Pr(unit, dch_index, pipe_index, reg_val32));
        }
    }


    /* set "WdeifmenP0" bit in LLFC registers */
    for (dch_index=0 ; dch_index < nof_instances_dch ; dch_index++)
    {
        SHR_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, dch_index, &reg_val64));
        soc_reg64_field32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_0r, &reg_val64, AUTO_DOC_NAME_26f, 1);
        SHR_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_0r(unit, dch_index, reg_val64));

        SHR_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, dch_index, &reg_val64));
        soc_reg64_field32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_1r, &reg_val64, AUTO_DOC_NAME_27f, 1);
        SHR_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_1r(unit, dch_index, reg_val64));

        SHR_IF_ERR_EXIT(READ_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, dch_index, &reg_val64));
        soc_reg64_field32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_P_2r, &reg_val64, AUTO_DOC_NAME_28f, 1);
        SHR_IF_ERR_EXIT(WRITE_DCH_LINK_LEVEL_FLOW_CONTROL_P_2r(unit, dch_index, reg_val64));
    }
   
    /* set wfq priorities */
    for (dch_index=0 ; dch_index < nof_instances_dch ; dch_index++)
    {
        SHR_IF_ERR_EXIT(READ_DCH_PIPES_WEIGHTS_REGISTERr(unit, dch_index, &reg_val32));
        soc_reg_field_set(unit, DCH_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_RST_CFGf, _SOC_RAMON_WFQ_PIPES_PRIORITY_INIT_VALUE);
        SHR_IF_ERR_EXIT(WRITE_DCH_PIPES_WEIGHTS_REGISTERr(unit, dch_index, reg_val32));
    }

     /* Internal thresholds*/
    if (SOC_DNXF_IS_MULTISTAGE_FE2(unit))
    {
        for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
        {
            for (dch_index = 0; dch_index < nof_instances_dch; dch_index++)
            {
                SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, &reg_val32));
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_0_0f, 0X1); /*enable internal threshold*/
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_4_22f, SOC_RAMON_DCH_AUTO_DOC_NAME_35_MULTI_FE2_DEF);
                SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, reg_val32));
            }
        }
    }
    else if (SOC_DNXF_IS_SINGLESTAGE_FE2(unit))
    {
        for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
        {
            for (dch_index = 0; dch_index < nof_instances_dch; dch_index++)
            {
                SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, &reg_val32));
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_0_0f, 0X1); /*enable internal threshold*/
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_4_22f, SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE1_FE2_DEF);
                SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, reg_val32));
            }
        }
    }
    else if (SOC_DNXF_IS_FE13(unit))
    {
        for (pipe_index = 0; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit); pipe_index++)
        {
            /*FE1 threshold*/
            for (dch_index=0 ; dch_index < nof_instances_dch/2; dch_index++) {
                SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, &reg_val32));
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_0_0f, 0X1); /*enable internal threshold*/
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_4_22f, SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE1_FE2_DEF);
                SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, reg_val32));
            }

            /*FE3 threshold*/
            for (dch_index=nof_instances_dch/2 ; dch_index < nof_instances_dch; dch_index++) {
                SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, &reg_val32));
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_0_0f, 0X1); /*enable internal threshold*/
                soc_reg_field_set(unit, DCH_AUTO_DOC_NAME_35r, &reg_val32, FIELD_4_22f, SOC_RAMON_DCH_AUTO_DOC_NAME_35_FE3_DEF);
                SHR_IF_ERR_EXIT(WRITE_DCH_AUTO_DOC_NAME_35r(unit, dch_index, pipe_index, reg_val32));
            }
        }

    }

           
exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_set_dcml_config(int unit)
{
    int dcml_index, nof_instances_dcml;
    uint32 reg_val32;
    soc_reg_above_64_val_t reg_above64_val;

    SHR_FUNC_INIT_VARS(unit);

    nof_instances_dcml = dnxf_data_device.blocks.nof_instances_dcml_get(unit);

    /* set wfq priorities */
    for (dcml_index=0 ; dcml_index < nof_instances_dcml ; dcml_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_PIPES_WEIGHTS_REGISTERr, dcml_index, 0, &reg_val32));
        soc_reg_field_set(unit, LCM_PIPES_WEIGHTS_REGISTERr, &reg_val32, WFQ_RST_CFGf, _SOC_RAMON_WFQ_PIPES_PRIORITY_INIT_VALUE);
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_PIPES_WEIGHTS_REGISTERr, dcml_index, 0, reg_val32));
    }

    for (dcml_index=0 ; dcml_index < nof_instances_dcml ; dcml_index++)
    {
        SHR_IF_ERR_EXIT(READ_DCML_AUTO_DOC_NAME_1r(unit, dcml_index, reg_above64_val));
        soc_reg_above_64_field32_set(unit, DCML_AUTO_DOC_NAME_1r, reg_above64_val, AUTO_DOC_NAME_33f, SOC_RAMON_DRV_DFL_AUTO_DOC_NAME_33);
        SHR_IF_ERR_EXIT(WRITE_DCML_AUTO_DOC_NAME_1r(unit, dcml_index, reg_above64_val));
    }

exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_set_lcm_config(int unit)
{
    int block_idx, nof_instances_lcm;
    uint32 reg_val32;
    uint64 reg_val64;
    soc_reg_above_64_val_t reg_above64_val;
    uint32 pipes_uc, pipes_mc;
    SHR_FUNC_INIT_VARS(unit);

    nof_instances_lcm = dnxf_data_device.blocks.nof_instances_lcm_get(unit);

    /* ================ */
    /* GCI/RCI Enablers */
    /* ================ */

    /* Enable RCI for UC pipes ; Enable GCI for MC pipes */
    pipes_uc = 0;
    pipes_mc = 0;

    SHR_IF_ERR_EXIT(READ_LCM_LCM_RCI_ENABLERSr(unit, 0, &reg_val64));
    SHR_IF_ERR_EXIT(READ_LCM_LCM_GCI_ENABLERSr(unit, 0, &reg_val32));

    if (dnxf_data_fabric.pipes.nof_pipes_get(unit) == 1)
    {
        SHR_BITSET(&pipes_uc, 0);
        SHR_BITSET(&pipes_mc, 0);
    }
    else
    {
        switch (dnxf_data_fabric.pipes.map_get(unit)->type)
        {
        case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET(&pipes_mc, 1);
            break;
        case soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET_RANGE(&pipes_mc, 1, 2);
            break;
        case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET(&pipes_mc, 0);
            break;
        case soc_dnxc_fabric_pipe_map_dual_uc_mc:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET(&pipes_mc, 1);
            break;
        case soc_dnxc_fabric_pipe_map_single:
            SHR_BITSET(&pipes_uc, 0);
            SHR_BITSET(&pipes_mc, 0);
            break;
        default:
            break;
        };
    }

    soc_reg64_field_set(unit, LCM_LCM_RCI_ENABLERSr, &reg_val64, PIPE_RCI_ENf, pipes_uc);
    soc_reg_field_set(unit, LCM_LCM_GCI_ENABLERSr, &reg_val32, PIPE_GCI_ENf, pipes_mc);

    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_LCM_RCI_ENABLERSr(unit, reg_val64));
    SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_LCM_GCI_ENABLERSr(unit, reg_val32));

    /* ======================= */
    /* GCI/RCI Method2 Disable */
    /* ======================= */

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    for (block_idx=0 ; block_idx < nof_instances_lcm ; block_idx++)
    {
         SHR_IF_ERR_EXIT(WRITE_LCM_DFL_GCI_TH_METHOD_2r(unit, block_idx, reg_above64_val));
         SHR_IF_ERR_EXIT(WRITE_LCM_DFL_RCI_TH_METHOD_2r(unit, block_idx, reg_above64_val));
    }

    /* =========== */
    /* FIFOs Depth */
    /* =========== */


    if(SOC_DNXF_IS_FE13(unit)) {

        /*
         * DTM Depth Profiles
         *
         * For FE13
         * Profile 0: used for non-local-route fifos: FE13-DTM4-7_DCH0-3, FE13-DTM0-3_DCH4-7.
         * Profile 1: used for local-route fifos:     FE13-DTM0-3_DCH0-3
         *
         * For FE2 Use only Profile 0
         */

        for (block_idx=0 ; block_idx < nof_instances_lcm/2 ; block_idx++)
        {
            SHR_IF_ERR_EXIT(WRITE_LCM_DTM_FIFO_TYPE_BMPr(unit, block_idx, SOC_RAMON_DRV_DTM_FIFO_PROFILE_BMP_LOW));
        }

        for (block_idx=nof_instances_lcm/2 ; block_idx < nof_instances_lcm ; block_idx++)
        {
            SHR_IF_ERR_EXIT(WRITE_LCM_DTM_FIFO_TYPE_BMPr(unit, block_idx, SOC_RAMON_DRV_DTM_FIFO_PROFILE_BMP_HIGH));
        }
    }


    /* =============== */
    /* Link Hysteresis */
    /* =============== */

    for (block_idx=0 ; block_idx < nof_instances_lcm ; block_idx++)
    {
        SHR_IF_ERR_EXIT(READ_LCM_LINK_LOAD_COUNT_CFGr(unit, block_idx, &reg_val32));
        soc_reg_field_set(unit, LCM_LINK_LOAD_COUNT_CFGr, &reg_val32, LINK_LOAD_SAT_VALf, SOC_RAMON_DRV_LINK_LOAD_SAT_VAL);
        SHR_IF_ERR_EXIT(WRITE_LCM_LINK_LOAD_COUNT_CFGr(unit, block_idx, reg_val32));

        SHR_IF_ERR_EXIT(READ_LCM_LINK_LOAD_HYST_TYPEr(unit, block_idx, &reg_val32));
        soc_reg_field_set(unit, LCM_LINK_LOAD_HYST_TYPEr, &reg_val32, LINK_LOAD_HYST_TYPEf, SOC_RAMON_DRV_LINK_LOAD_HYST_TYPE_BYTE);
        SHR_IF_ERR_EXIT(WRITE_LCM_LINK_LOAD_HYST_TYPEr(unit, block_idx, reg_val32));

        SHR_IF_ERR_EXIT(READ_LCM_LINK_LOAD_HYST_THr(unit, block_idx, &reg_val32));
        soc_reg_field_set(unit, LCM_LINK_LOAD_HYST_THr, &reg_val32, LINK_LOAD_HYST_LOW_THf, SOC_RAMON_DRV_LINK_LOAD_HYST_LOW_TH);
        soc_reg_field_set(unit, LCM_LINK_LOAD_HYST_THr, &reg_val32, LINK_LOAD_HYST_HIGH_THf, SOC_RAMON_DRV_LINK_LOAD_HYST_HIGH_TH);
        SHR_IF_ERR_EXIT(WRITE_LCM_LINK_LOAD_HYST_THr(unit, block_idx, reg_val32));
    }

    exit:
    SHR_FUNC_EXIT;
}


STATIC int
soc_ramon_set_eci_config(int unit)
{
    uint32 reg32_val;
    int rv;
    SHR_FUNC_INIT_VARS(unit);
    
    

    rv = READ_ECI_FAP_GLOBAL_GENERAL_CFG_1r(unit, &reg32_val);
    SHR_IF_ERR_EXIT(rv);
    if (dnxf_data_fabric.pipes.system_contains_multiple_pipe_device_get(unit)) {
        soc_reg_field_set(unit, ECI_FAP_GLOBAL_GENERAL_CFG_1r, &reg32_val, RESERVED_QTSf, 2);
    } else {
        soc_reg_field_set(unit, ECI_FAP_GLOBAL_GENERAL_CFG_1r, &reg32_val, RESERVED_QTSf, 0);
    }
    rv = WRITE_ECI_FAP_GLOBAL_GENERAL_CFG_1r(unit, reg32_val);
    SHR_IF_ERR_EXIT(rv);
    
    /*PVT configuration*/
    SHR_IF_ERR_EXIT(soc_ramon_drv_pvt_monitor_enable(unit));

exit:
    SHR_FUNC_EXIT;
}

int
soc_ramon_reset_device(int unit)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    /* cmic regs reset function */
    rc =soc_ramon_reset_cmic_iproc_regs(unit); 
    SHR_IF_ERR_EXIT(rc);

    /*PLL configuration*/
    rc = soc_ramon_drv_pll_config_set(unit);
    SHR_IF_ERR_EXIT(rc);

    /* blocks reset*/
    rc = soc_ramon_drv_blocks_reset(unit, 0 /*full reset*/ , NULL);
    SHR_IF_ERR_EXIT(rc);

    /*
     * Memory bist
     */
    if (dnxf_data_device.access.bist_enable_get(unit))
    {
        rc = soc_bist_all_ramon(unit, dnxf_data_device.access.bist_enable_get(unit) - 1);
        SHR_IF_ERR_EXIT(rc);

        /* Rerun init sequence */

        /* cmic regs reset function */
        rc =soc_ramon_reset_cmic_iproc_regs(unit); 
        SHR_IF_ERR_EXIT(rc);

        /*PLL configuration*/
        rc = soc_ramon_drv_pll_config_set(unit);
        SHR_IF_ERR_EXIT(rc);
        
        /*blocks reset*/
        rc =soc_ramon_drv_blocks_reset(unit, 0 /*full reset*/ , NULL);
        SHR_IF_ERR_EXIT(rc);
    }

    /* interrupt init*/
     rc = soc_ramon_interrupts_init(unit);
     SHR_IF_ERR_EXIT(rc);

    /*Blocks broadcast*/
    rc = soc_ramon_drv_sbus_broadcast_config(unit);
    SHR_IF_ERR_EXIT(rc);

    /* Enable dynamic memory access */
    rc = soc_ramon_drv_dynamic_mem_access_enable(unit);
    SHR_IF_ERR_EXIT(rc);

    /*soft reset - into reset*/
    rc = soc_ramon_drv_soft_init(unit, SOC_DNXC_RESET_ACTION_IN_RESET);
    SHR_IF_ERR_EXIT(rc);

    /* interrupt init*/
    rc = soc_ramon_interrupts_disable(unit);
    SHR_IF_ERR_EXIT(rc);

    /* set operation mode function */
    rc = soc_ramon_set_operation_mode(unit);
    SHR_IF_ERR_EXIT(rc);
    
    /* reset tables function*/
    rc = soc_ramon_reset_tables(unit);
    SHR_IF_ERR_EXIT(rc);

    /* set fmac config function*/
    rc = soc_ramon_set_fmac_config(unit);
    SHR_IF_ERR_EXIT(rc);

    /* set fsrd config function */
    rc = soc_ramon_set_fsrd_config(unit);
    SHR_IF_ERR_EXIT(rc);

    /* set ccs config function */
    rc = soc_ramon_set_ccs_config(unit);
    SHR_IF_ERR_EXIT(rc); 

    /* set mesh topology config function */
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_set_mesh_topology_config ,(unit));
    SHR_IF_ERR_EXIT(rc);    

    /* set rtp config function */
    rc = soc_ramon_set_rtp_config(unit);
    SHR_IF_ERR_EXIT(rc);

    /* set dch config function */
    rc = soc_ramon_set_dch_config(unit);
    SHR_IF_ERR_EXIT(rc);    

    /* set dcml config function */
    rc = soc_ramon_set_dcml_config(unit);
    SHR_IF_ERR_EXIT(rc);

    /* set lcm config function */
    rc = soc_ramon_set_lcm_config(unit);
    SHR_IF_ERR_EXIT(rc);

    /*set eci config function*/
    rc = soc_ramon_set_eci_config(unit);
    SHR_IF_ERR_EXIT(rc);

     /*Set default low priority drop select*/    
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_multicast_low_prio_drop_select_priority_set,(unit, soc_dnxf_fabric_priority_0));
    SHR_IF_ERR_EXIT(rc);

    /*soft reset - out of reset*/
    rc = soc_ramon_drv_soft_init(unit, SOC_DNXC_RESET_ACTION_OUT_RESET);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}


int
soc_ramon_drv_pll_config_set(int unit)
{

    soc_reg_above_64_val_t puc;
    uint32 puc_lcpll_in_sel=0;
    uint32 pll_status_val32, pll_locked;
    int lcpll, config_index;
    soc_reg_above_64_val_t reg_above_64_val;
    soc_reg_above_64_val_t pll_config_above_64_val;
    int lcpll_in, lcpll_out;
    soc_reg_t   pll_config_reg[]        = {ECI_MISC_PLL_0_CONFIGr,   ECI_MISC_PLL_1_CONFIGr,   ECI_MISC_PLL_2_CONFIGr,   ECI_MISC_PLL_3_CONFIGr};
    soc_field_t pll_config_field[]      = {MISC_PLL_0_CONFIGf,       MISC_PLL_1_CONFIGf,       MISC_PLL_2_CONFIGf,       MISC_PLL_3_CONFIGf};
    soc_field_t pll_reset_field[]       = {MISC_PLL_0_RESET_Bf,      MISC_PLL_1_RESET_Bf,      MISC_PLL_2_RESET_Bf,      MISC_PLL_3_RESET_Bf};
    soc_field_t pll_post_reset_field[]  = {MISC_PLL_0_POST_RESET_Bf, MISC_PLL_1_POST_RESET_Bf, MISC_PLL_2_POST_RESET_Bf, MISC_PLL_3_POST_RESET_Bf};
    soc_field_t pll_sw_ovrd_field[]     = {MISC_PLL_0_SW_OVERRIDEf,  MISC_PLL_1_SW_OVERRIDEf,  MISC_PLL_2_SW_OVERRIDEf,  MISC_PLL_3_SW_OVERRIDEf};

    soc_reg_t   pll_stat_reg[]          = {ECI_MISC_PLL_0_STATUSr,   ECI_MISC_PLL_1_STATUSr,   ECI_MISC_PLL_2_STATUSr,   ECI_MISC_PLL_3_STATUSr};
    soc_field_t pll_locked_field[]      = {MISC_PLL_0_LOCKEDf,       MISC_PLL_1_LOCKEDf,       MISC_PLL_2_LOCKEDf,       MISC_PLL_3_LOCKEDf};

    SHR_FUNC_INIT_VARS(unit);


    /*Check if PLL configuration is possible*/
    SHR_IF_ERR_EXIT( soc_reg_above_64_get(unit, ECI_POWERUP_CONFIGr, REG_PORT_ANY, 0, puc) );
    SHR_BITCOPY_RANGE(&puc_lcpll_in_sel, 0, puc, SOC_RAMON_PORT_PUC_LCPLL_IN_SEL, SOC_RAMON_PORT_PUC_LCPLL_IN_SEL_NOF_BITS);

    if ( !(puc_lcpll_in_sel ==  SOC_RAMON_PORT_PUC_LCPLL_IN_DISABLE_POWERUP) && !SAL_BOOT_PLISIM )
    {
        /*PLL static configuration - the PLL was set at power up*/
        SHR_EXIT();
    }

    for(lcpll=0 ; lcpll<dnxf_data_port.pll.nof_lcpll_get(unit); lcpll++) 
    {
        lcpll_in = dnxf_data_port.pll.info_get(unit, lcpll)->fabric_ref_clk_in;
        lcpll_out = dnxf_data_port.pll.info_get(unit, lcpll)->fabric_ref_clk_out;

        if (dnxf_data_port.pll.config_get(unit,lcpll_in,lcpll_out)->valid == 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,"Illegal reference clock configuration IN=%d, OUT=%d", lcpll_in, lcpll_out);
        }

        SOC_REG_ABOVE_64_CLEAR(pll_config_above_64_val);

        for (config_index=0 ; config_index < dnxf_data_port.pll.nof_config_words_get(unit) ; config_index++)
        {
            SOC_REG_ABOVE_64_WORD_SET(pll_config_above_64_val, dnxf_data_port.pll.config_get(unit,lcpll_in,lcpll_out)->data[config_index], config_index);
        }

        SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

        /* set LCPLL configuration */
        soc_reg_above_64_field_set(unit, pll_config_reg[lcpll], reg_above_64_val, pll_config_field[lcpll], pll_config_above_64_val);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));

        /* set SW override */
        soc_reg_above_64_field32_set(unit, pll_config_reg[lcpll], reg_above_64_val, pll_sw_ovrd_field[lcpll], 1);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));

        /* take LCPLL out of reset */
        soc_reg_above_64_field32_set(unit, pll_config_reg[lcpll], reg_above_64_val, pll_reset_field[lcpll], 1);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));

        sal_usleep(100);

        /* validate LCPLL locked */
        if (!SAL_BOOT_PLISIM)
        {
            SHR_IF_ERR_EXIT(soc_reg32_get(unit, pll_stat_reg[lcpll], REG_PORT_ANY, 0, &pll_status_val32));
            pll_locked = soc_reg_field_get(unit, pll_stat_reg[lcpll], pll_status_val32, pll_locked_field[lcpll]);
            if (pll_locked != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INIT,"Failed to init LCPLL %d", lcpll);
            }
        }

        /* post reset */
        soc_reg_above_64_field32_set(unit, pll_config_reg[lcpll], reg_above_64_val, pll_post_reset_field[lcpll], 1);
        SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, pll_config_reg[lcpll], REG_PORT_ANY, 0, reg_above_64_val));

    }

exit:
    SHR_FUNC_EXIT; 
}

int
soc_ramon_drv_mdio_config_set(int unit)
{

    int divisor, ext_divisor, mdio_delay;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Mdio - internal
     */
    divisor = dnxf_data_device.access.mdio_int_divisor_get(unit);
    ext_divisor = dnxf_data_device.access.mdio_ext_divisor_get(unit);
    mdio_delay = dnxf_data_device.access.mdio_int_out_delay_default_get(unit);

    SHR_IF_ERR_EXIT(soc_cmicx_miim_init(unit));
    SHR_IF_ERR_EXIT(soc_cmicx_miim_divider_set_all(unit, divisor, ext_divisor, mdio_delay));
exit:
    SHR_FUNC_EXIT;

}

int
soc_ramon_drv_pvt_monitor_enable(int unit)
{
    uint64 reg64_val;
    soc_reg_t pvt_monitors[] = {ECI_PVT_MON_A_CONTROL_REGr, ECI_PVT_MON_B_CONTROL_REGr, ECI_PVT_MON_C_CONTROL_REGr, ECI_PVT_MON_D_CONTROL_REGr};
    int pvt_index;
    SHR_FUNC_INIT_VARS(unit);

    /*Init*/
    COMPILER_64_ZERO(reg64_val);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    /*Powerdown*/
    COMPILER_64_BITSET(reg64_val, _SOC_RAMON_ECI_PVT_MON_CONTROL_REG_POWERDOWN_BIT);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    /*Powerup*/
    COMPILER_64_ZERO(reg64_val);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

    /*Reset*/
    COMPILER_64_BITSET(reg64_val, _SOC_RAMON_ECI_PVT_MON_CONTROL_REG_RESET_BIT);
    for (pvt_index = 0; pvt_index < (sizeof(pvt_monitors) / sizeof(soc_reg_t)); pvt_index++)
    {
        SHR_IF_ERR_EXIT(soc_reg_set(unit, pvt_monitors[pvt_index], REG_PORT_ANY, 0, reg64_val));
    }

exit:
    SHR_FUNC_EXIT; 
}

/*
 * SBUS ring map:
 * Ring 0: AVS(2)
 * Ring 2: DCH(96-103), CCH(104-111), QRH(88-95), MCT(87), RTP(86), MESH_TOPOLOGY(85), BRDC_DCH(122), BRDC_CCH(123), BRDC_QRH(126)
 * Ring 3: DCML(3-6, 8-11), BRDC_DCML(124)
 * Ring 4: FMAC(12-59), FSRD(60-83), BRDC_MAC(120), BRDC_FSRD(121)
 * Ring 5: LCM(112-119), BRDC_LCM(125)
 * Ring 7: ECI(0), CMIC(127)
 */
int
soc_ramon_drv_rings_map_set(int unit)
{
	SHR_FUNC_INIT_VARS(unit);

	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_0_7r(unit, 0x73333027));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_8_15r(unit, 0x44443333));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_16_23r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_24_31r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_32_39r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_40_47r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_48_55r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_56_63r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_64_71r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_72_79r(unit, 0x44444444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_80_87r(unit, 0x22204444));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_88_95r(unit, 0x22222222));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_96_103r(unit, 0x22222222));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_104_111r(unit, 0x22222222));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_112_119r(unit, 0x55555555));
	SHR_IF_ERR_EXIT(WRITE_CMIC_TOP_SBUS_RING_MAP_120_127r(unit, 0x72532244));

	exit:
	SHR_FUNC_EXIT;
}

int
soc_ramon_drv_sbus_broadcast_config(int unit)
{
    int block_idx;
    int is_enabled;
    soc_info_t *si;

    SHR_FUNC_INIT_VARS(unit);

    si = &SOC_INFO(unit);

    /*FMAC broadcast*/
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_fmac_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_FMAC_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_FMAC_BLOCK(unit)).schan));
    }
    
    /*FSRD broadcast*/
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_fsrd_get(unit); block_idx++)
    {
        /*Check if FSRD block is enabled*/
        is_enabled = si->block_valid[block_idx];
        if (is_enabled)
        {
            SHR_IF_ERR_EXIT(WRITE_FSRD_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_FSRD_BLOCK(unit)).schan));
        }
    }

    /*DCH broadcast*/
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_dch_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_DCH_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_DCH_BLOCK(unit)).schan));
    }

    /*DCML broadcast*/
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_dcml_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_DCML_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_DCML_BLOCK(unit)).schan));
    }

    /*CCH broadcast*/
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_cch_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_SBUS_BROADCAST_IDr, block_idx, 0, SOC_BLOCK_INFO(unit,BRDC_CCH_BLOCK(unit)).schan));
    }

    /*LCM broadcast*/
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_lcm_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_LCM_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit,BRDC_LCM_BLOCK(unit)).schan));
    }

    /*QRH broadcast*/
    for (block_idx=0; block_idx<dnxf_data_device.blocks.nof_instances_qrh_get(unit); block_idx++)
    {
        SHR_IF_ERR_EXIT(WRITE_QRH_SBUS_BROADCAST_IDr(unit, block_idx, SOC_BLOCK_INFO(unit, BRDC_QRH_BLOCK(unit)).schan));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_drv_soft_init
 * Purpose:
 *      Run blocks soft init
 * Parameters:
 *      unit  -                     (IN)     Unit number.
 *      soft_reset_mode_flags -     (IN)     SOC_DNXC_RESET_ACTION_IN_RESET, SOC_DNXC_RESET_ACTION_OUT_RESET, SOC_DNXC_RESET_ACTION_INOUT_RESET
 * Returns:
 *      _SHR_E_NONE     No Error  
 *      _SHR_E_UNAVAIL  Feature unavailable  
 *      _SHR_E_XXX      Error occurred  
 */

int 
soc_ramon_drv_soft_init(int unit, uint32 soft_reset_mode_flags)
{
    soc_reg_above_64_val_t reg_above_64;
    SHR_FUNC_INIT_VARS(unit);

    if (soft_reset_mode_flags == SOC_DNXC_RESET_ACTION_IN_RESET || soft_reset_mode_flags == SOC_DNXC_RESET_ACTION_INOUT_RESET)
    {
        SOC_REG_ABOVE_64_ALLONES(reg_above_64);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64));
    }

    if (soft_reset_mode_flags == SOC_DNXC_RESET_ACTION_OUT_RESET || soft_reset_mode_flags == SOC_DNXC_RESET_ACTION_INOUT_RESET)
    {
        SOC_REG_ABOVE_64_CLEAR(reg_above_64);
        SHR_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_above_64));
    }

exit:
    SHR_FUNC_EXIT;
}

int soc_ramon_drv_reg_access_only_reset(int unit)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    /* cmic regs reset function */
    rc =soc_ramon_reset_cmic_iproc_regs(unit);
    SHR_IF_ERR_EXIT(rc);

    /* blocks reset*/
    rc = soc_ramon_drv_blocks_reset(unit, 0 /*full reset*/ , NULL);
    SHR_IF_ERR_EXIT(rc);
    rc = soc_ramon_drv_pll_config_set(unit);
    SHR_IF_ERR_EXIT(rc);
    /* interrupt init*/
     rc = soc_ramon_interrupts_init(unit);
     SHR_IF_ERR_EXIT(rc);
    /*Blocks broadcast*/
    rc = soc_ramon_drv_sbus_broadcast_config(unit);
    SHR_IF_ERR_EXIT(rc);

    /* Enable dynamic memory access */
    rc = soc_ramon_drv_dynamic_mem_access_enable(unit);
    SHR_IF_ERR_EXIT(rc);
    /* interrupt init*/
    rc = soc_ramon_interrupts_disable(unit);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_ramon_drv_test_brdc_blk_info_get
 * Purpose:
 *      Returns necessary info on device broadcast blocks
 * Parameters:
 *      unit                        - (IN)  Unit number.
 *      max_size                    - (IN)  max number of broadcast blocks
 *      brdc_info                   - (OUT) structure which holds the required info about each broadcast block
 *      actual_size                 - (OUT) number of broadcast blocks
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
int 
soc_ramon_drv_test_brdc_blk_info_get(int unit, int max_size, soc_reg_brdc_block_info_t *brdc_info, int *actual_size)
{
    int instance;
    int i;
    SHR_FUNC_INIT_VARS(unit);
    DNXC_LEGACY_FIXME_ASSERT;
    *actual_size = 0;

    /*FMAC_AC*/
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_FMAC_AC;
        for (i = 0, instance = 0; instance < dnxf_data_device.blocks.nof_instances_fmac_get(unit) / 4; instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        for (instance = dnxf_data_device.blocks.nof_instances_fmac_get(unit) / 2;
             instance < 3 * dnxf_data_device.blocks.nof_instances_fmac_get(unit) / 4; instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SHR_ERR_EXIT(_SHR_E_FULL, "Test does not support all device block ids");
    }

    /*FMAC_BD*/
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_FMAC_BD;
        for (i = 0, instance = dnxf_data_device.blocks.nof_instances_fmac_get(unit) / 4;
             instance < dnxf_data_device.blocks.nof_instances_fmac_get(unit) / 2; instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        for (instance = 3 * dnxf_data_device.blocks.nof_instances_fmac_get(unit) / 4;
             instance < dnxf_data_device.blocks.nof_instances_fmac_get(unit); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = FMAC_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SHR_ERR_EXIT(_SHR_E_FULL, "Test does not support all device block ids");
    }

    /*FSRD*/
    if (!(dnxc_device_block_for_feature(unit,DNXC_FABRIC_12_QUADS_FEATURE)) &&
        !(dnxc_device_block_for_feature(unit,DNXC_FABRIC_24_QUADS_FEATURE)) &&
        !(dnxc_device_block_for_feature(unit,DNXC_FABRIC_18_QUADS_FEATURE))) /*SKUs do not support broadcast fsrd*/
    {
        if (max_size > *actual_size)
        {
            brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_FSRD;
            for (i = 0, instance = 0; instance < dnxf_data_device.blocks.nof_instances_fsrd_get(unit); instance++, i++)
            {
                brdc_info[*actual_size].blk_ids[i] = FSRD_BLOCK(unit, instance); 
            }
            brdc_info[*actual_size].blk_ids[i] = -1;

            (*actual_size)++;
        } else {
            SHR_ERR_EXIT(_SHR_E_FULL, "Test does not support all device block ids");
        }

    }
    /*DCH*/
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_DCH;
        for (i = 0, instance = 0; instance < dnxf_data_device.blocks.nof_instances_fsrd_get(unit); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = DCH_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SHR_ERR_EXIT(_SHR_E_FULL, "Test does not support all device block ids");
    }

    /*DCM*/
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_DCM;
        for (i = 0, instance = 0; instance < dnxf_data_device.blocks.nof_instances_fsrd_get(unit); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = DCM_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SHR_ERR_EXIT(_SHR_E_FULL, "Test does not support all device block ids");
    }

    /*DCL*/
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_DCL;
        for (i = 0, instance = 0; instance < dnxf_data_device.blocks.nof_instances_fsrd_get(unit); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = DCL_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SHR_ERR_EXIT(_SHR_E_FULL, "Test does not support all device block ids");
    }

    /*CCS*/
    if (max_size > *actual_size)
    {
        brdc_info[*actual_size].blk_type = SOC_BLK_BRDC_CCS;
        for (i = 0, instance = 0; instance < dnxf_data_device.blocks.nof_instances_fsrd_get(unit); instance++, i++)
        {
            brdc_info[*actual_size].blk_ids[i] = CCS_BLOCK(unit, instance); 
        }
        brdc_info[*actual_size].blk_ids[i] = -1;

        (*actual_size)++;
    } else {
        SHR_ERR_EXIT(_SHR_E_FULL, "Test does not support all device block ids");
    }

exit:
    SHR_FUNC_EXIT; 
}

int
soc_ramon_nof_block_instances(int unit, soc_block_types_t block_types, int *nof_block_instances) 
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(nof_block_instances, _SHR_E_PARAM, "nof_block_instances");
    SHR_NULL_CHECK(block_types, _SHR_E_PARAM, "block_types");

    switch(block_types[0]) {
        case SOC_BLK_FMAC:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_fmac_get(unit); 
            break;
        case SOC_BLK_FSRD:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_fsrd_get(unit);
            break;
        case SOC_BLK_DCH:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_dch_get(unit);
            break;
        case SOC_BLK_CCH:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_cch_get(unit);
            break;
        case SOC_BLK_DCL:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_dcml_get(unit);
            break;
        case SOC_BLK_RTP:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_rtp_get(unit);
            break;
        case SOC_BLK_OCCG:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_occg_get(unit);
            break;
        case SOC_BLK_ECI:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_eci_get(unit);
            break;
        case SOC_BLK_DCM:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_dcml_get(unit);
            break;
        case SOC_BLK_CMIC:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_cmic_get(unit);
            break;
        case SOC_BLK_DCML:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_dcml_get(unit);
            break;
        case SOC_BLK_MCT:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_mct_get(unit);
            break;
        case SOC_BLK_QRH:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_qrh_get(unit);
            break;
        case SOC_BLK_LCM:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_lcm_get(unit);
            break;
        case SOC_BLK_MESH_TOPOLOGY:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_mesh_topology_get(unit);
            break;
        case SOC_BLK_BRDC_FMACH:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_brdc_fmach_get(unit);
            break;
        case SOC_BLK_BRDC_FMACL:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_brdc_fmacl_get(unit);
            break;
        case SOC_BLK_BRDC_FSRD:
            *nof_block_instances = dnxf_data_device.blocks.nof_instances_brdc_fsrd_get(unit);
            break;
            
        case SOC_BLK_BRDC_CCH:
        case SOC_BLK_BRDC_DCML:
        case SOC_BLK_BRDC_LCM:
        case SOC_BLK_BRDC_QRH:
            *nof_block_instances = 1;
            break;
        default:
            *nof_block_instances = 0;
            break;
    }

exit:
    SHR_FUNC_EXIT;
}


/*
 * Memory bist test
 */
int
soc_ramon_drv_mbist(int unit, int skip_errors)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);

    SOC_DNXF_DRV_INIT_LOG(unit, "Memory Bist");
    rc = soc_bist_all_ramon(unit, skip_errors);
    SHR_IF_ERR_EXIT(rc);
        
exit:
    SHR_FUNC_EXIT;   
}

int
soc_ramon_drv_block_pbmp_get(int unit, int block_type, int blk_instance, soc_pbmp_t *pbmp)
{   
    int first_link = 0,range = 0;
    SHR_FUNC_INIT_VARS(unit);

    SOC_PBMP_CLEAR(*pbmp);

    switch (block_type)
    {
       case SOC_BLK_DCH:
       case SOC_BLK_DCML:
           first_link = dnxf_data_device.blocks.nof_links_in_dcq_get(unit) * blk_instance;
           range = dnxf_data_device.blocks.nof_links_in_dcq_get(unit);
           break;
       default:
           SHR_ERR_EXIT(_SHR_E_INTERNAL, "block (%d) - block pbmp is not supported", block_type);
           break;
    }

    SOC_PBMP_PORTS_RANGE_ADD(*pbmp, first_link, range);
    SOC_PBMP_AND(*pbmp, PBMP_SFI_ALL(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Returns TRUE if the memory is dynamic
 */
int soc_ramon_tbl_is_dynamic(int unit, soc_mem_t mem) {

    if (soc_mem_is_readonly(unit, mem) || soc_mem_is_writeonly(unit, mem) || soc_mem_is_signal(unit, mem)) {
        return TRUE;
    }

    switch (mem)
    {
        case RTP_MULTI_CAST_TABLE_UPDATEm:
            return TRUE;
        case RTP_SLSCTm:
            if (soc_dnxf_load_balancing_mode_destination_unreachable !=
                dnxf_data_fabric.topology.load_balancing_mode_get(unit))
            {
                return TRUE;
            }
            else
            {
                return FALSE;
            }

        default:
            return FALSE;
    }
}

int
soc_ramon_drv_block_valid_get(int unit, int blktype, int blockid, char *valid)
{

    int enabled = 1;
    soc_pbmp_t pbmp_supported;
    soc_pbmp_t pbmp_fsrd_tmp;

    SHR_FUNC_INIT_VARS(unit);
    SOC_PBMP_CLEAR(pbmp_fsrd_tmp);
    pbmp_supported = dnxf_data_port.general.supported_phys_get(unit)->pbmp;

    if (blktype == SOC_BLK_FSRD) {
        SOC_PBMP_PORTS_RANGE_ADD(pbmp_fsrd_tmp, blockid * dnxf_data_device.blocks.nof_links_in_fsrd_get(unit), dnxf_data_device.blocks.nof_links_in_fsrd_get(unit));
        SOC_PBMP_AND(pbmp_fsrd_tmp, pbmp_supported);
        if (SOC_PBMP_IS_NULL(pbmp_fsrd_tmp))
        {
            enabled = 0;
        }

        *valid = ((enabled == 1) ? 1 : 0);
    } else {
        *valid  = 1;
    }

    SHR_FUNC_EXIT; 
}

int
soc_ramon_drv_link_to_block_mapping(int unit, int link, int* block_id,int* inner_link, int block_type)
{   
    int nof_links_in_blk;

    SHR_FUNC_INIT_VARS(unit);

    switch (block_type)
    {
       case SOC_BLK_DCH:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_dch_get(unit);
           break;

       case SOC_BLK_DCML:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_dcml_get(unit);
           break;

       case SOC_BLK_LCM:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_lcm_get(unit);
           break;

       case SOC_BLK_QRH:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_qrh_get(unit);
           break;

       case SOC_BLK_FMAC:
           nof_links_in_blk = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
           break;

       default:
           SHR_ERR_EXIT(_SHR_E_INTERNAL, "block (%d) - link to block mapiing is not supported", block_type);
           break;
    }

   *block_id=INT_DEVIDE(link,nof_links_in_blk);
   *inner_link=link % nof_links_in_blk;

exit:
    SHR_FUNC_EXIT;
}


int
soc_ramon_set_mesh_topology_config(int unit) {

    uint32 reg_val32, intg, frac; 
    uint64 frac64;
    soc_reg_t mesh_topology_reg_0107;
    int gt_size = -1;
    SHR_FUNC_INIT_VARS(unit);
    /*Should be replaced by alias feature*/
    mesh_topology_reg_0107 = MESH_TOPOLOGY_MESH_TOPOLOGY_REG_0107r;


    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 0);
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, FIELD_27_27f, 1);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGY_2r(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGY_2r, &reg_val32, FIELD_4_17f, 2049);    
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGY_2r(unit, reg_val32));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_INITr(unit, &reg_val32));
    if(SOC_DNXF_IS_FE13(unit)) {
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, INITf, 10);
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, CONFIG_1f, 2);
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, CONFIG_2f, 0xe);
    } else {
        soc_reg_field_set(unit, MESH_TOPOLOGY_INITr, &reg_val32, CONFIG_2f, 1);
    }
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_INITr(unit, reg_val32));

    intg =
        dnxf_data_device.general.system_ref_core_clock_khz_get(unit) /
        dnxf_data_device.general.core_clock_khz_get(unit);
    COMPILER_64_SET(frac64, 0, dnxf_data_device.general.system_ref_core_clock_khz_get(unit));
    COMPILER_64_SUB_32(frac64, dnxf_data_device.general.core_clock_khz_get(unit) * intg);
    COMPILER_64_SHL(frac64, 19);
    SHR_IF_ERR_EXIT(soc_dnxf_compiler_64_div_32(frac64, dnxf_data_device.general.core_clock_khz_get(unit), &frac));

    soc_reg32_get(unit, mesh_topology_reg_0107, REG_PORT_ANY, 0, &reg_val32);

    soc_reg_field_set(unit, mesh_topology_reg_0107, &reg_val32, REG_107_CONFIG_1f, frac);
    soc_reg_field_set(unit, mesh_topology_reg_0107, &reg_val32, REG_107_CONFIG_2f, intg);

    soc_reg32_set(unit, mesh_topology_reg_0107, REG_PORT_ANY, 0, reg_val32);

    
    gt_size = soc_property_suffix_num_get(unit,0,spn_CUSTOM_FEATURE, "mesh_topology_size", -1);

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    if (gt_size == -1)
    {
        if (dnxf_data_fabric.pipes.system_contains_multiple_pipe_device_get(unit))
        {
            soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, 2);
        }
        else
        {
            soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, 0);
        }
    } else {
        soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_2f, gt_size);
    }
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_1f, 15);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_REG_011Br(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_011Br, &reg_val32, REG_11B_CONFIG_0f, 5); /* nof cells */

    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_011Br, &reg_val32, REG_11B_CONFIG_1f, 0xc);

    soc_reg_field_set(unit, MESH_TOPOLOGY_REG_011Br, &reg_val32, REG_11B_CONFIG_2f,
                      dnxf_data_device.custom_features.mesh_topology_fast_get(unit) ? 1 : 0);

    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_REG_011Br(unit, reg_val32));

    sal_usleep(20);

    /* Enable back */
    SHR_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 1);
    SHR_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
