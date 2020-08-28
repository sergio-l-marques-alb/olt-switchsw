/* $Id: arad_kbp_connectivity.c,v 1.50 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0) && defined(INCLUDE_KBP)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TCAM

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>
#include <soc/dcmn/error.h>
#include <soc/i2c.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/dpp/ARAD/arad_kbp_rop.h>
#include <soc/dpp/ARAD/arad_kbp_xpt.h>
#include <soc/dpp/ARAD/arad_kbp_recover.h>

#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_api_nif.h>
#include <soc/dpp/ARAD/arad_api_ports.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ce_instruction.h>


#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/scache.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/ha.h>
#endif

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* KBP mdio */
#define ARAD_KBP_MDIO_CLAUSE 45

/* KBP ROP test */
#define ARAD_KBP_ROP_TEST_REG_ADDR       0x102 
#define ARAD_KBP_ROP_TEST_DATA          {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef, 0x12, 0x35}

extern genericTblMgrAradAppData *AradAppData[SOC_SAND_MAX_DEVICE];

/* Test ROP packet write and read */
int arad_kbp_init_rop_test(int unit, uint32 core){
    uint32
       res,  
       addr, nbo_addr;
    uint8 data[10] = ARAD_KBP_ROP_TEST_DATA;
    arad_kbp_rop_write_t wr_data;
    arad_kbp_rop_read_t rd_data;

    if (ARAD_KBP_IS_OP2) {
    	/* For OP2, we don't need ROP test. */
        return SOC_E_NONE;
    }

    if (ARAD_KBP_IS_OP) {
        addr = 0xE; /* scratch pad register address */
    } else {
        addr = ARAD_KBP_ROP_TEST_REG_ADDR;
    }

    nbo_addr = soc_htonl(addr);
    sal_memcpy(wr_data.addr, &nbo_addr, sizeof(uint32));
    sal_memset(wr_data.mask, 0x0, NLM_DATA_WIDTH_BYTES);
    sal_memset(wr_data.addr_short, 0x0, NLMDEV_REG_ADDR_LEN_IN_BYTES);
    wr_data.writeMode = NLM_ARAD_WRITE_MODE_DATABASE_DM;
    
    /* Set wr_data */
    ARAD_KBP_ROP_REVERSE_DATA(data, wr_data.data, 10);
    
    /* Set rd_data */
    sal_memcpy(rd_data.addr, &nbo_addr, sizeof(uint32));
    rd_data.dataType = NLM_ARAD_READ_MODE_DATA_X;
    
    /* ROP write and ROP read */
    res = arad_kbp_rop_write(unit, core, &wr_data);
    if (res != 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "%s(): arad_kbp_rop_write failed\n"), FUNCTION_NAME()));
        return SOC_E_FAIL;
    }

    res = arad_kbp_rop_write(unit, core, &wr_data);
    if (res != 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "%s(): arad_kbp_rop_write failed\n"), FUNCTION_NAME()));
        return SOC_E_FAIL;
    }

    res = arad_kbp_rop_read(unit, core, &rd_data);
    if (res!=0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "%s(): arad_kbp_rop_read failed\n"), FUNCTION_NAME()));
        return SOC_E_FAIL;
    }
    /* Comparing Read data with Written data */
    if (ARAD_KBP_IS_OP) {
        /* Scratch PD is only 8 bytes in Optimus Prime */
        res = sal_memcmp(rd_data.data+3 ,wr_data.data + 2, 8);
    } else {
        res = sal_memcmp(rd_data.data+1 ,wr_data.data, 10);
    }

    if (res != 0) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "%s(): arad_kbp_rop write-read failed. read data does no match expected data\n"), 
                              FUNCTION_NAME()));
        return SOC_E_FAIL;
    }
    return SOC_E_NONE;
}

int32_t arad_kbp_callback_mdio_read(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t *value){
    uint32 kbp_reg_addr;
    ARAD_NIF_ELK_USER_DATA *user_data = handle;
    int rv;
    
    if(handle == NULL){
        return KBP_INVALID_ARGUMENT;
    }
    kbp_reg_addr = reg + (dev << 16);
    rv = soc_dcmn_miim_read(user_data->device, ARAD_KBP_MDIO_CLAUSE, user_data->kbp_mdio_id, kbp_reg_addr, value);
    if(SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META("%s(): soc_dcmn_miim_read() Failed\n"), FUNCTION_NAME()));
        return KBP_INTERNAL_ERROR;
    }
    return KBP_OK;
}

int32_t arad_kbp_callback_mdio_write(void *handle, int32_t chip_no, uint8_t dev, uint16_t reg, uint16_t value){
    uint32 kbp_reg_addr;
    ARAD_NIF_ELK_USER_DATA *user_data = handle;
    int rv;
    
    if(handle == NULL){
        return KBP_INVALID_ARGUMENT;
    }
    kbp_reg_addr = reg + (dev << 16);
    rv = soc_dcmn_miim_write(user_data->device, ARAD_KBP_MDIO_CLAUSE, user_data->kbp_mdio_id, kbp_reg_addr, value);
    if(SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META("%s(): arad_kbp_callback_mdio_write() Failed\n"), FUNCTION_NAME()));
        return KBP_INTERNAL_ERROR;
    }
    return KBP_OK;   
}

int32_t arad_kbp_usleep_callback(void *handle, uint32_t usec){
    sal_usleep(usec);
    return KBP_OK;
}

uint32 arad_kbp_init_egw_config_set(
    int unit,
    uint32 core,
    int ilkn_num_lanes, 
    int ilkn_rate,
    uint32 meta_frame_len)
{
    uint32
        res,
        reg_val = 0x0,
        fld_val = 0x0,
        ilkn_total_mbits_rate,
        ilkn_total_burst_rate,
        core_clk_ticks,
        spr_dly_mega,
        spr_dly_fld,
        spr_dly_fraction_fld;
    uint64
        reg64_val = 0x0;
    soc_reg_above_64_val_t
        fld_above64_val,
        reg_above64_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_REG_ABOVE_64_CLEAR(reg_above64_val);
    SOC_REG_ABOVE_64_CLEAR(fld_above64_val);

    if(!SAL_BOOT_PLISIM){

    /* Minimum/Maximum packet size in B, up to which packet not closed */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_PACKET_SIZEr(unit, core, &reg_val));
    fld_val = 64;
    soc_reg_field_set(unit, IHB_PACKET_SIZEr, &reg_val, MIN_PKT_SIZEf, fld_val);
    fld_val = 256;
    soc_reg_field_set(unit, IHB_PACKET_SIZEr, &reg_val, MAX_PKT_SIZEf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_PACKET_SIZEr(unit, core, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_TRANSMIT_CFGSr(unit, core, &reg64_val));
    /* EGW will not transmit FLP records if FIFO exceed this threshold */
    fld_val = 0x190;
    soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, FLP_TRANSMITING_THRESHOLDf, fld_val);
    /* congestion identified when FIFO exceed this threshold (enables ROP style packing and/or MaxPktSize reaching) */
    fld_val = 0x14;
    soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, CONGESTION_THRESHOLDf, fld_val);
    /* Enable ROP style packing */
    fld_val = 0x1;
    soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, PACKING_ENABLEf, fld_val);
#ifdef BCM_88660_A0
    if (SOC_IS_ARADPLUS(unit)){
        /* set CPU priority over traffic lookups */
        fld_val = 0x1;
        soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, CPU_RECORD_PRIOf, fld_val);
    }
#endif /* BCM_88660_A0 */
    if (ARAD_KBP_IS_OP_OR_OP2) {
        fld_val = 0x1;
        soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, TIMER_MODEf, fld_val);
        /*Transmit packing delay, Jericho Plus has higher core clock, this vaule should be larger, */
        /*othersize,there may be FLP lookup timeouts and other ELK errors in case of packets burst */
        fld_val = SOC_IS_JERICHO_PLUS_ONLY(unit) ? 0x13 : 0x10;
        soc_reg64_field_set(unit, IHB_TRANSMIT_CFGSr, &reg64_val, TIMER_DELAYf, fld_val);
    }
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_TRANSMIT_CFGSr(unit, core, reg64_val));

    /* Enables Attaching application prefix in packet transmition/reciving */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_APP_PREFIX_CONTROLr(unit, core, &reg_val));
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_APP_PREFIX_CONTROLr, &reg_val, TX_APP_PREFIX_ENABLEf, fld_val);
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_APP_PREFIX_CONTROLr, &reg_val, RX_APP_PREFIX_ENABLEf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_APP_PREFIX_CONTROLr(unit, core, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_PHY_CHANNEL_CFGSr(unit, core, &reg_val));
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_PHY_CHANNEL_CFGSr, &reg_val, TX_CHANNEL_NUMf, fld_val);
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_PHY_CHANNEL_CFGSr, &reg_val, RX_CHANNEL_NUMf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_PHY_CHANNEL_CFGSr(unit, core, reg_val));

    /* 
     * Shaper configuration
     */
    ilkn_total_mbits_rate = ilkn_num_lanes * ilkn_rate;
    ilkn_total_burst_rate = ilkn_total_mbits_rate / (32 * 8);
    core_clk_ticks = arad_chip_ticks_per_sec_get(unit);

    /*
     * The actual ILKN rate should consider:
     *     overhead for metaframe(4B);
     *     64B/67B encoding;
     *     overhead for burst control(8B).
     *
     */
    spr_dly_mega = core_clk_ticks /
                   ((ilkn_total_burst_rate * (meta_frame_len - 4) * 64 * 32) / (meta_frame_len * 67 * 40));

    spr_dly_fld = spr_dly_mega / 1000000;

    /* Need to find the fraction = ((64 * 1024 * dly_mega) / 1000000).
     * since the fraction is = x/64K. 64/1000000 = 1/15625 
     */ 
    spr_dly_fraction_fld = ((spr_dly_mega % 1000000) * 1024) / 15625;
   LOG_VERBOSE(BSL_LS_SOC_TCAM,
               (BSL_META_U(unit,
                           "%s(): Shaper configuration ilkn_total_mbits_rate=%d, ilkn_total_burst_rate=%d, core_clk_ticks=%d\n"), 
                           FUNCTION_NAME(), ilkn_total_mbits_rate, ilkn_total_burst_rate, core_clk_ticks)); 
   LOG_VERBOSE(BSL_LS_SOC_TCAM,
               (BSL_META_U(unit,
                           "%s(): Shaper configuration spr_dly_fld=%d, spr_dly_fraction_fld=%d\n"), 
                           FUNCTION_NAME(), spr_dly_fld, spr_dly_fraction_fld));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_SPR_DLY_CFGSr(unit, core, &reg64_val));
    soc_reg64_field_set(unit, IHB_SPR_DLY_CFGSr, &reg64_val, SPR_DLYf, spr_dly_fld);
    soc_reg64_field_set(unit, IHB_SPR_DLY_CFGSr, &reg64_val, SPR_DLY_FRACTIONf, spr_dly_fraction_fld);
    fld_val = 0x1ff;
    soc_reg64_field_set(unit, IHB_SPR_DLY_CFGSr, &reg64_val, SPR_MAX_BURSTf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_SPR_DLY_CFGSr(unit, core, reg64_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_SPR_CFGSr(unit, core, &reg_val));
    fld_val = 0x3;
    soc_reg_field_set(unit, IHB_SPR_CFGSr, &reg_val, WORD_ALIGNMENTf, fld_val);
    fld_val = 0x0;
    soc_reg_field_set(unit, IHB_SPR_CFGSr, &reg_val, PACKET_GAPf, fld_val);
    fld_val = 0x8;
    soc_reg_field_set(unit, IHB_SPR_CFGSr, &reg_val, PACKET_OVERHEADf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_SPR_CFGSr(unit, core, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_INTERLAKEN_CFGSr(unit, core, &reg_val));
    fld_val = 0x1;
    soc_reg_field_set(unit, IHB_INTERLAKEN_CFGSr, &reg_val, ILKN_MODEf, fld_val);
    fld_val = 0x1;
    soc_reg_field_set(unit, IHB_INTERLAKEN_CFGSr, &reg_val, ILKN_MIN_BURSTf, fld_val);
    fld_val = 0x7;
    soc_reg_field_set(unit, IHB_INTERLAKEN_CFGSr, &reg_val, ILKN_MAX_BURSTf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_INTERLAKEN_CFGSr(unit, core, reg_val));

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHB_GENERAL_CFGSr(unit, core, &reg_val));
    /* ***** Enables lookups to ELK ***** */
    fld_val = 0x1;
    soc_reg_field_set(unit, IHB_GENERAL_CFGSr, &reg_val, ENABLE_ELK_LOOKUPf, fld_val);
    fld_val = 0x0;
    /* PB leftover should be 0x0*/
    soc_reg_field_set(unit, IHB_GENERAL_CFGSr, &reg_val, NIF_TX_INIT_CREDITSf, fld_val);
    /* Error recovery time of ROP packet transmission, Jericho Plus has higher core clock, this vaule should be larger */
    /* Multiply it by 2 to account for unknown factors,for Jericho Plus, the original value is 0x514*/
    fld_val = SOC_IS_JERICHO_PLUS_ONLY(unit) ? 0xA28 : 0x7D0;
    soc_reg_field_set(unit, IHB_GENERAL_CFGSr, &reg_val, TIMEOUT_DLYf, fld_val);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_GENERAL_CFGSr(unit, core, reg_val));
    if (!SOC_IS_JERICHO_PLUS(unit)) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_FLP_GENERAL_CFGr_REG64(unit, core, &reg64_val));
        /* ***** Enables lookups to ELK ***** */
        fld_val = 0x1;
        soc_reg64_field_set(unit, IHP_FLP_GENERAL_CFGr, &reg64_val, ELK_ENABLEf, fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_FLP_GENERAL_CFGr_REG64(unit, core, reg64_val));
    } else {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_IHP_FLP_GENERAL_CFGr(unit, core, reg_above64_val));
        /* ***** Enables lookups to ELK ***** */
        fld_above64_val[0] = 0x1;
        soc_reg_above_64_field_set(unit, IHP_FLP_GENERAL_CFGr, reg_above64_val, ELK_ENABLEf, fld_above64_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHP_FLP_GENERAL_CFGr(unit, core, reg_above64_val));
    }
    /* ***** Enables ELK on ilkn port 10/16 ***** */
    fld_val = 0x1; /*ILKN port 16 val*/
    if (SOC_IS_ARADPLUS_AND_BELOW(unit)){
#ifdef BCM_88660_A0
        if (SOC_IS_ARADPLUS(unit)) {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_ELK_CFG_ON_ILKN_PORT_10r(unit, &reg_val));
            if(SOC_DPP_CONFIG(unit)->arad->init.elk.ext_interface_mode){
                soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10f, 1);
                fld_val = 0; /*change the value of ELK on ILKN port 16*/
                switch(ilkn_num_lanes){
                case 4:
                    soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10_USING_FOUR_LANESf, 1);
                    break;
                case 8:
                    soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10_USING_FOUR_LANESf, 1);
                    soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10_USING_EIGHT_LANESf, 1);
                    break;
                default:
                     LOG_ERROR(BSL_LS_SOC_TCAM,
                               (BSL_META_U(unit,
                                           "Error in %s(): External interface mode unsupported ilkn_num_lanes=%d. (supported num of lanes 4/8)\n"), FUNCTION_NAME(), ilkn_num_lanes));
                     SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 11, exit);
                }
            }
            else{
                soc_reg_field_set(unit, NBI_ELK_CFG_ON_ILKN_PORT_10r, &reg_val, ELK_ENABLE_ON_ILKN_PORT_10f, 0);
            }
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_NBI_ELK_CFG_ON_ILKN_PORT_10r(unit, reg_val));
        }
#endif
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_NBI_ELK_CFGr(unit, &reg_val));
        soc_reg_field_set(unit, NBI_ELK_CFGr, &reg_val, ELK_ENABLE_ON_ILKN_PORT_16f, fld_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_NBI_ELK_CFGr(unit, reg_val));
    }
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_egw_config_set()", ilkn_num_lanes, ilkn_rate);
}

uint32 arad_kbp_init_egw_default_opcode_set(
    int unit)
{
    uint32
        res,
        mem_filed = 0x0;
    uint32
        table_entry[1];
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Configure in the EGW the used OPCODES */
    /* LUT_WR. Opcode ARAD_KBP_CPU_WR_LUT_OPCODE = 255 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_WR_LUT_OPCODE, table_entry));
    /* Opcode 255 is 10B data (80 bit) */
    mem_filed = 9;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 255 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_WR_LUT_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_WR_LUT_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_WR_LUT_OPCODE, table_entry));

    /* LUT_RD. Opcode ARAD_KBP_CPU_RD_LUT_OPCODE = 254 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_RD_LUT_OPCODE, table_entry));
    /* Opcode 254 is 1B data (8 bit) */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 254 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_RD_LUT_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_RD_LUT_OPCODE, table_entry));
    /* Reply is 10B */
    mem_filed = 9;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_RD_LUT_OPCODE, table_entry));

    /* PIOWR. Opcode ARAD_KBP_CPU_PIOWR_OPCODE = 253 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIOWR_OPCODE, table_entry));
    /* Opcode 253 is 24B data */
    mem_filed = 23;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 253 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIOWR_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIOWR_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIOWR_OPCODE, table_entry));

    /* PIORD-X. Opcode ARAD_KBP_CPU_PIORDX_OPCODE = 252 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDX_OPCODE, table_entry));
    /* Opcode 252 is 4B data */
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 252 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDX_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDX_OPCODE, table_entry));
    /* Reply is 11B */
    mem_filed = 10;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDX_OPCODE, table_entry));

    /* PIORD-Y. Opcode ARAD_KBP_CPU_PIORDY_OPCODE = 251 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    /* Opcode 251 is 4B data */
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 251 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    /* Reply is 11B */
    mem_filed = 10;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
  
    /* PIORD-Y. Opcode ARAD_KBP_CPU_CTX_BUFF_WRITE_OPCODE = 251 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    /* Opcode 251 is 4B data */
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 251 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));
    /* Reply is 11B */
    mem_filed = 10;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_PIORDY_OPCODE, table_entry));

    /* Block Copy. Opcode ARAD_KBP_CPU_BLK_COPY_OPCODE = 249 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_COPY_OPCODE, table_entry));
    /* Opcode 249 is 8B data */
    mem_filed = 7;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 249 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_COPY_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_COPY_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_COPY_OPCODE, table_entry));

   /* Block move. Opcode ARAD_KBP_CPU_BLK_MOVE_OPCODE = 248 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_MOVE_OPCODE, table_entry));
    /* Opcode 248 is 8B data */
    mem_filed = 7;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 248 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_MOVE_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_MOVE_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_MOVE_OPCODE, table_entry));

   /* Block clear. Opcode ARAD_KBP_CPU_BLK_CLR_OPCODE = 247 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_CLR_OPCODE, table_entry));
    /* Opcode 247 is 5B data */
    mem_filed = 4;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 247 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_CLR_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_CLR_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 115, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_CLR_OPCODE, table_entry));

   /* Block invalidate. Opcode ARAD_KBP_CPU_BLK_CLR_OPCODE = 246 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_EV_OPCODE, table_entry));
    /* Opcode 246 is 5B data */
    mem_filed = 4;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 246 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 125, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_EV_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 130, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_EV_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 135, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_BLK_EV_OPCODE, table_entry));

    /*AD write Opcode ARAD_KBP_CPU_4B_AD_OPCODE = 220 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 140, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_4B_AD_OPCODE, table_entry));
    /* Opcode 220 is 12B data */
    mem_filed = 11;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 246 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 145, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_4B_AD_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_4B_AD_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 155, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_4B_AD_OPCODE, table_entry));
    
    /*AD write Opcode ARAD_KBP_CPU_8B_AD_OPCODE= 221 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_8B_AD_OPCODE, table_entry));
    /* Opcode 221 is 16B data */
    mem_filed = 15;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 246 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 165, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_8B_AD_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 170, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_8B_AD_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 175, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_8B_AD_OPCODE, table_entry));
    
    /*AD write Opcode ARAD_KBP_CPU_16B_AD_OPCODE= 222 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 180, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_16B_AD_OPCODE, table_entry));
    /* Opcode 222 is 32B data */
    mem_filed = 23;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 246 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 185, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_16B_AD_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 190, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_16B_AD_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 195, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_16B_AD_OPCODE, table_entry));
    
    /*AD write Opcode ARAD_KBP_CPU_32B_AD_OPCODE= 223 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 180, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_32B_AD_OPCODE, table_entry));
    /* Opcode 222 is 32B data */
    mem_filed = 39;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 246 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 185, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_32B_AD_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 190, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_32B_AD_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 195, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_32B_AD_OPCODE, table_entry));
    
    /*UDA Operation1 Opcode ARAD_KBP_CPU_UDA_OPERATION1_OPCODE = 231 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 111, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_UDA_OPERATION1_OPCODE, table_entry));
    /* Opcode 231 is 40B data */
    mem_filed = 39;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 231 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 113, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_UDA_OPERATION1_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 117, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_UDA_OPERATION1_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 119, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_UDA_OPERATION1_OPCODE, table_entry));
    
    /*UDA Operation2 Opcode ARAD_KBP_CPU_UDA_OPERATION2_OPCODE = 233 */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 121, exit, READ_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_UDA_OPERATION2_OPCODE, table_entry));
    /* Opcode 233 is 8B data */
    mem_filed = 7;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, SIZEf, mem_filed);
    /* Opcode 233 is Type request (not info) */
    mem_filed = 1;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_TXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 123, exit, WRITE_IHB_OPCODE_MAP_TXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_UDA_OPERATION2_OPCODE, table_entry));
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 127, exit, READ_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_UDA_OPERATION2_OPCODE, table_entry));
    /* Reply is 1B */
    mem_filed = 0;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, SIZEf, mem_filed);
    mem_filed = 3;
    soc_mem_field32_set(unit, IHB_OPCODE_MAP_RXm, table_entry, TYPEf, mem_filed);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 129, exit, WRITE_IHB_OPCODE_MAP_RXm(unit, SOC_CORE_ALL, ARAD_KBP_CPU_UDA_OPERATION2_OPCODE, table_entry));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_egw_default_opcode_set()", 0x0, 0x0);
}


uint32 arad_kbp_ilkn_interface_param_get( 
    int         unit,
    uint32      core,
    soc_port_t *ilkn_port,
    uint32     *ilkn_num_lanes,
    int        *ilkn_rate,
    uint32     *ilkn_metaframe)
{
    uint32
        res = SOC_SAND_OK,
        offset;
    soc_error_t
        rv;
    soc_port_t
        port;
    soc_pbmp_t 
        ports_bm;
    soc_port_if_t 
        interface_type;
    ARAD_PORTS_ILKN_CONFIG 
        *ilkn_config;
    uint32 flags;
    int core_id;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if(!SAL_BOOT_PLISIM){

    /* Find ILKN1 port number */
    *ilkn_port = -1;
    rv = soc_port_sw_db_valid_ports_get(unit, 0x0, &ports_bm);
    if(SOC_FAILURE(rv)) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 2, exit);
    }
    SOC_PBMP_ITER(ports_bm, port){
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 3, exit, soc_port_sw_db_core_get(unit, port, &core_id));

        if (core != core_id) {
            continue;
        }
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 3, exit, soc_port_sw_db_interface_type_get(unit, port, &interface_type)); 
        if (SOC_PORT_IF_ILKN == interface_type) {
            rv = soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset); 
            ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
            if(SOC_FAILURE(rv)) {
                SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 4, exit);
            }
            if (SOC_IS_JERICHO(unit)) {
                rv = soc_port_sw_db_flags_get(unit, port, &flags);
                if(SOC_FAILURE(rv)) {
                    SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 4, exit);
                }
                if (SOC_PORT_IS_ELK_INTERFACE(flags)) {
                    if (*ilkn_port != -1) {
                        LOG_ERROR(BSL_LS_SOC_TCAM, (BSL_META_U(unit, "Error in %s(): More than 1 ILKN KBP port found.\n"), FUNCTION_NAME()));    
                         SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 5, exit);
                    }
                    *ilkn_port = port;
                }
            } else {
                if (0x1 == offset) {
                    *ilkn_port = port;
                    break;
                }
            }

        }
    }
    if (*ilkn_port == -1) {
        if (!SOC_IS_JERICHO(unit)) {
           LOG_ERROR(BSL_LS_SOC_TCAM, (BSL_META_U(unit, "Error in %s(): No ILKN1 port found.\n"), FUNCTION_NAME()));    
           SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 5, exit);
        } else {
           LOG_VERBOSE(BSL_LS_SOC_TCAM, (BSL_META_U(unit, "%s(): No ILKN port found for core %u\n"), FUNCTION_NAME(), core));    
           soc_sand_set_error_code_into_error_word(SOC_SAND_GEN_ERR, &ex);
           goto exit_wo_print;
        }
    }

    /* get ILKN configuration */
    rv = soc_port_sw_db_num_lanes_get(unit, *ilkn_port, ilkn_num_lanes);
    if(SOC_FAILURE(rv)) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 10, exit);
    }
    rv = soc_port_sw_db_speed_get(unit, *ilkn_port, ilkn_rate);
    if(SOC_FAILURE(rv)) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 20, exit);
    }
    rv = soc_port_sw_db_protocol_offset_get(unit, *ilkn_port, 0, &offset);
    ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[offset];
    if(SOC_FAILURE(rv)) {
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 30, exit);
    }
    *ilkn_metaframe = ilkn_config->metaframe_sync_period;
    LOG_VERBOSE(BSL_LS_SOC_TCAM,
                (BSL_META_U(unit,
                            "%s(): ilkn_port=%d, ilkn_num_lanes=%d, ilkn_rate=%d, ilkn_metaframe=%d\n"), 
                            FUNCTION_NAME(), *ilkn_port, *ilkn_num_lanes, *ilkn_rate, *ilkn_metaframe));    
    }

exit_wo_print:
    return ex;

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_ilkn_interface_param_get()", *ilkn_num_lanes, *ilkn_rate);
}

uint32 arad_kbp_init_kbp_interface( 
    int unit,
    uint32 core)
{
    int rc = 0;
    enum kbp_device_type kbp_type;
    uint32_t flags = KBP_DEVICE_DEFAULT;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!SAL_BOOT_PLISIM) {
        if (SOC_DPP_CONFIG(unit)->arad->init.elk.enable == 0x0) {
           LOG_ERROR(BSL_LS_SOC_TCAM,
                     (BSL_META_U(unit,
                                 "Error in %s(): ELK disabled (ext_tcam_dev_type might be NONE)!!!\n"), FUNCTION_NAME()));    
            SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
        }
        /* set port flags for OP and OP2; skip configuration for core 1 */
        if (ARAD_KBP_IS_OP_OR_OP2) {
            if (core == 1) {
                SOC_EXIT;
            }
            if (ARAD_KBP_OP_IS_DUAL_MODE) {
                flags |= KBP_DEVICE_QUAD_PORT;
            } else {
                flags |= KBP_DEVICE_DUAL_PORT;
            }
        }

        /* select KBP device type */
        if (ARAD_KBP_IS_OP) {
            kbp_type = KBP_DEVICE_OP;
        } else if (ARAD_KBP_IS_OP2) {
            kbp_type = KBP_DEVICE_OP2;
        } else {
            kbp_type = KBP_DEVICE_12K;
        }

        LOG_VERBOSE(BSL_LS_SOC_TCAM,
                    (BSL_META_U(unit,
                                "%s(): Start. kbp_mdio_id=0x%x, kbp_ilkn_rev=%d.\n"),
                                FUNCTION_NAME(), elk->kbp_user_data[core].kbp_mdio_id, AradAppData[unit]->device_config[core].reverse_lanes));

        /*config the kbp*/
        rc = kbp_device_interface_init(kbp_type, flags, &AradAppData[unit]->device_config[core]);
        if (ARAD_KBP_TO_SOC_RESULT(rc) != SOC_SAND_OK) {
            if ((ARAD_KBP_OP_IS_DUAL_MODE) && (elk->is_master)) {
                rc = KBP_OK;
            } else {
                LOG_ERROR(BSL_LS_SOC_TCAM,
                          (BSL_META_U(unit,
                                      "Error in %s(): configuring elk device. %s\n"), FUNCTION_NAME(), kbp_get_status_string(rc)));
                SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
            }
        }
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_kbp_init_kbp_interface()", 0, 0);

}

#if (defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__))
uint32 arad_kbp_init_gpio_config(
    int unit)
{
    int res, kbp_gpio_address, kbp_gpio_mask;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (SOC_IS_QAX(unit)) {
        kbp_gpio_address = 0x3;
        kbp_gpio_mask = 0x11;
    } else if (SOC_IS_JERICHO(unit)) {
        kbp_gpio_address = 0x20;
        kbp_gpio_mask = 0xf;
    } else {
        kbp_gpio_address = 0x3;
        kbp_gpio_mask = 0xc0;
    }
    /* Set kbp gpio's to Output */
    res = cpu_i2c_write(0x40, kbp_gpio_address, CPU_I2C_ALEN_LONG_DLEN_LONG, kbp_gpio_mask);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_gpio_config()", 0, 0);
}
#endif

#if 0

int32 arad_kbp_serdes_pre_enable_link_callback(
        void *misc_handle)
{
    struct kbp_device_config *config = misc_handle;
    uint64_t lane_bitmap = 0ULL;
    uint32_t bitmap;
    int32_t i;
    uint16_t enable_taps;
    int16_t pre2, pre1, txfir_main, post1, post2, post3;
    kbp_status status;

    for (i = 0; i < 4; i++) {
        if (config->port_map[i].num_lanes) {
            bitmap = (1ULL << config->port_map[i].num_lanes) - 1;
            lane_bitmap |= bitmap << config->port_map[i].start_lane;
        }
    }
    /* default values */
    pre2 = 4;
    pre1 = -14;
    enable_taps = 0x1;
    txfir_main = 81;
    post1 = -28;
    post2 = 0;
    post3 = 0;

    status = kbp_device_interface_op2_tx_serdes_emphasize(config, lane_bitmap, enable_taps, pre2, pre1, txfir_main, post1, post2, post3);
    if (status != KBP_OK)
        return 1;

    return KBP_OK;
}
#endif

uint32 arad_kbp_init_device_config_init(
    int unit,
    uint32 core,
    uint32 kbp_ilkn_rev,
    kbp_reset_f kbp_reset)
{
    int res, ilkn_rate;
    struct kbp_device_config *kbp_config;
    uint32 ilkn_num_lanes, ilkn_metaframe, offset;
    soc_port_t ilkn_port;
    ARAD_INIT_ELK *elk = &SOC_DPP_CONFIG(unit)->arad->init.elk;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /*
     * Master device needs to allocate AradAppData for master device and slave device.
     */
    if ((AradAppData[unit] == NULL) && elk->is_master) {
        ARAD_ALLOC_ANY_SIZE(AradAppData[unit], genericTblMgrAradAppData, 1, "AradAppData[unit][core]");
        if (ARAD_KBP_OP_IS_DUAL_MODE) {
            ARAD_ALLOC_ANY_SIZE(AradAppData[unit+1], genericTblMgrAradAppData, 1, "AradAppData[slave_unit][core]");
        }
    }

    /*
     * For OP, core0 and core1 share the same device config info, no need to update again.
     */
    if (ARAD_KBP_IS_OP_OR_OP2) {
        /* Number of cores check added to fix compilation */
        if ((core == 1) && (SOC_DPP_DEFS_MAX(NOF_CORES) == 2)) {
            AradAppData[unit]->device_config[1] = AradAppData[unit]->device_config[0];
            SOC_EXIT;
        }
    }
    kbp_config = &AradAppData[unit]->device_config[core];
    /*
     * Init KBP callback functions
     */
    kbp_config->mdio_read = arad_kbp_callback_mdio_read;
    kbp_config->mdio_write = arad_kbp_callback_mdio_write;
    kbp_config->assert_kbp_resets = kbp_reset;
    kbp_config->usleep = arad_kbp_usleep_callback;
#if 0
    
    if (ARAD_KBP_IS_OP2)
        kbp_config->pre_enable_link_callback = arad_kbp_serdes_pre_enable_link_callback;
    else
#endif
    kbp_config->pre_enable_link_callback = NULL;
    /*
     * Init KBP user data handle
     */
    kbp_config->handle = &elk->kbp_user_data[core];
    /*
     * Init KBP ILKN interface configurations
     */
    res = arad_kbp_ilkn_interface_param_get(unit, core, &ilkn_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    switch (ilkn_rate) {
        case 6250:
            kbp_config->speed = KBP_INIT_LANE_6_25;
            break;
        case 10312:
            kbp_config->speed = KBP_INIT_LANE_10_3;
            break;
        case 12500:
            kbp_config->speed = KBP_INIT_LANE_12_5;
            break;
        case 25781:
            kbp_config->speed = KBP_INIT_LANE_25_78125;
            break;
        case 27343:
            kbp_config->speed = KBP_INIT_LANE_27_34375;
            break;
        case 56250:
            kbp_config->speed = KBP_INIT_LANE_56_25;
            break;
        default:
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                                  "Error in %s(): Error, unsupported ilkn_rate=%d.\n"), FUNCTION_NAME(), ilkn_rate));
            SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 20, exit);
    }


    if (ARAD_KBP_IS_OP_OR_OP2) {
        kbp_config->init_avs = 1;
        kbp_config->port_map[0].start_lane = elk->kbp_start_lane[0];
        kbp_config->port_map[0].num_lanes = ilkn_num_lanes;
        kbp_config->port_map[1].start_lane = elk->kbp_start_lane[1];
        kbp_config->port_map[1].num_lanes = ilkn_num_lanes;
        if (ARAD_KBP_OP_IS_DUAL_MODE) {
            kbp_config->port_map[2].start_lane = elk->kbp_start_lane[2];
            kbp_config->port_map[2].num_lanes = ilkn_num_lanes;
            kbp_config->port_map[3].start_lane = elk->kbp_start_lane[3];
            kbp_config->port_map[3].num_lanes = ilkn_num_lanes;
        }
    } else {
        kbp_config->port_map[0].num_lanes = ilkn_num_lanes;
    }

    kbp_config->burst = KBP_INIT_BURST_SHORT_16_BYTES;
    /*metframe length set*/
    if (ilkn_metaframe == 64 || ilkn_metaframe == 2048) {
        kbp_config->meta_frame_len = ilkn_metaframe;
    } else {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                              "Error in %s(): Unsupported ilkn_meta_data=%d.\n"), FUNCTION_NAME(), ilkn_metaframe));
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 30, exit);
    }

    if (ARAD_KBP_IS_OP2) {
        kbp_config->settings.link_training_enable = soc_property_port_get(unit, ilkn_port, spn_PORT_INIT_CL72, 0);
    }
    res = soc_port_sw_db_protocol_offset_get(unit, ilkn_port, 0, &offset);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    kbp_config->reverse_lanes = soc_property_port_get(unit, offset, spn_EXT_ILKN_REVERSE, kbp_ilkn_rev);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_device_config_init()", 0, 0);
}

uint32 arad_kbp_init_kbp_reset(
    int unit,
    uint32 core)
{
    int rc;
    uint16_t value_temp;
    enum kbp_device_type kbp_type = KBP_DEVICE_UNKNOWN;
    struct kbp_device_config *kbp_config;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* This API is only relevant for OP and OP2 */
    if (ARAD_KBP_IS_OP) {
        kbp_type = KBP_DEVICE_OP;
    } else if (ARAD_KBP_IS_OP2) {
        kbp_type = KBP_DEVICE_OP2;
    } else {
        LOG_ERROR(BSL_LS_SOC_TCAM,
                  (BSL_META_U(unit,
                  "Error in %s(): This API is relevant for OP and OP2\n"), FUNCTION_NAME()));
        SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 100, exit);
    }

    kbp_config = &AradAppData[unit]->device_config[core];

    if (core == 1) {
       SOC_EXIT;
    }

    if (!SAL_BOOT_PLISIM) {
        /* Reset the kbp */
        rc = kbp_device_interface_reset(kbp_type, kbp_config);
        /* dummy read to avoid OP MDIO lock-up */
        arad_kbp_callback_mdio_read(kbp_config->handle, 0, 0, 0, &value_temp);
        if (ARAD_KBP_TO_SOC_RESULT(rc) != SOC_SAND_OK) {
            LOG_ERROR(BSL_LS_SOC_TCAM,
                      (BSL_META_U(unit,
                      "Error in %s(): resetting elk device. %s\n"), FUNCTION_NAME(), kbp_get_status_string(rc)));
            SOC_SAND_CHECK_FUNC_RESULT(SOC_SAND_GEN_ERR, 100, exit);
        }
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_kbp_reset()", 0 , 0);
}

uint32 arad_kbp_blk_lut_set(int unit, uint32 core)
{
    uint32
        res = SOC_SAND_OK;
    arad_kbp_lut_data_t 
        lut_data;
    
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    sal_memset(&lut_data, 0, sizeof(lut_data));

    /* Block copy instruction */
    lut_data.instr = ARAD_KBP_CPU_BLK_COPY_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_BLK_COPY_REC_SIZE;
    lut_data.rec_is_valid = 1;   
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_BLK_COPY_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Block move instruction */
    lut_data.instr = ARAD_KBP_CPU_BLK_MOVE_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_BLK_MOVE_REC_SIZE;
    lut_data.rec_is_valid = 1;   
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_BLK_MOVE_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    /* Block clear instruction */
    lut_data.instr = ARAD_KBP_CPU_BLK_CLR_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_BLK_CLR_REC_SIZE;
    lut_data.rec_is_valid = 1;   
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_BLK_CLR_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    /* Block invalidate instruction */
    lut_data.instr = ARAD_KBP_CPU_BLK_EV_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_BLK_EV_REC_SIZE;
    lut_data.rec_is_valid = 1;   
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_BLK_EV_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    /* 32b AD write instruction */
    lut_data.instr = ARAD_KBP_CPU_AD_WRITE_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_4B_AD_REC_SIZE;
    lut_data.rec_is_valid = 1;
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_4B_AD_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /* 64b AD write instruction */
    lut_data.instr = ARAD_KBP_CPU_AD_WRITE_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_8B_AD_REC_SIZE;
    lut_data.rec_is_valid = 1;
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_8B_AD_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    /* 128b AD write instruction */
    lut_data.instr = ARAD_KBP_CPU_AD_WRITE_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_16B_AD_REC_SIZE;
    lut_data.rec_is_valid = 1;
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_16B_AD_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    /* 256b AD write instruction */
    lut_data.instr = ARAD_KBP_CPU_AD_WRITE_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_32B_AD_REC_SIZE;
    lut_data.rec_is_valid = 1;
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_32B_AD_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    /* UDA Operation1 instruction */
    lut_data.instr = ARAD_KBP_CPU_UDA_OPERATION1_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_UDA_OPERATION1_REC_SIZE;
    lut_data.rec_is_valid = 1;
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_UDA_OPERATION1_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    /* UDA Operation2 instruction */
    lut_data.instr = ARAD_KBP_CPU_UDA_OPERATION2_INSTRUCTION;
    lut_data.rec_size = ARAD_KBP_CPU_UDA_OPERATION2_REC_SIZE;
    lut_data.rec_is_valid = 1;
    res = arad_kbp_lut_write(unit, core, ARAD_KBP_CPU_UDA_OPERATION2_OPCODE, &lut_data, NULL);
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_blk_lut_set()", 0, 0);
}

uint32 arad_kbp_init_arad_interface( 
    int unit)
{
    uint32
        res = SOC_SAND_OK,
        ilkn_num_lanes,
        ilkn_metaframe;
    soc_port_t
        ilkn_port;
    int
        ilkn_rate;
    uint64
        reg_val;
    uint32 core;
    uint32 one_kbp_ilkn_at_least = 0;
   
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_DPP_CORES_ITER(_SHR_CORE_ALL, core) {
        res = arad_kbp_ilkn_interface_param_get(unit, core, &ilkn_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe);
        if (res == 0) {
            one_kbp_ilkn_at_least = 1;
           LOG_VERBOSE(BSL_LS_SOC_TCAM,
                       (BSL_META_U(unit,
                                   "%s(): ilkn_port=%d, ilkn_num_lanes=%d, ilkn_rate=%d, ilkn_metaframe=%d\n"), FUNCTION_NAME(), ilkn_port, ilkn_num_lanes, ilkn_rate, ilkn_metaframe));
       

           if (!SOC_IS_JERICHO(unit)) {
			    uint32 tx_1_burst_short_ilkn_config = soc_property_suffix_num_get(unit, 0, "tx_1_burst_short_ilkn_segment", "", 0);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_NBI_TX_1_ILKN_CONTROLr(unit, &reg_val));
                soc_reg64_field_set(unit, NBI_TX_1_ILKN_CONTROLr, &reg_val, TX_1_BURSTSHORTf, tx_1_burst_short_ilkn_config);
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_NBI_TX_1_ILKN_CONTROLr(unit, reg_val));
           }

           /* EGW config needed, might be according to ilkn configuration */
            res = arad_kbp_init_egw_config_set(unit, core, ilkn_num_lanes, ilkn_rate, ilkn_metaframe);
            SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
        }
    }

    if (one_kbp_ilkn_at_least == 0) {
       SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

    res = arad_kbp_init_egw_default_opcode_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);


	if ( soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ext_ipv4_mc_flexible_fwd_table", 0) &&
		!soc_property_get(unit, spn_EXT_IP4_MC_FWD_TABLE_SIZE, 0x0) ) {
		res=-1;
        _bsl_error (_BSL_SOCDNX_MSG("soc property custom_feature_ext_ipv4_mc_flexible_fwd_table is on! make sure also ext_ip4_mc_fwd_table_size is defined")) ;  
		SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
	}
      
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_kbp_init_arad_interface()", ilkn_num_lanes, ilkn_rate);
}

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif /* #if defined(BCM_88650_A0) && defined(INCLUDE_KBP) */

