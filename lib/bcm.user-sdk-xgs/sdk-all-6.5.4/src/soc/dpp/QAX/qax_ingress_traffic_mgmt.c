/*
 * $Id: $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * File: qax_ingress_traffic_mgmt.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/QAX/qax_ingress_traffic_mgmt.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>

int
  qax_itm_init(
    SOC_SAND_IN  int  unit
  )
{
    uint32                 data = 0;
    soc_reg_above_64_val_t above64;
    uint64                 val64;
    int                    res;

    SOCDNX_INIT_FUNC_DEFS;

    data = 0x0;
    if (!SOC_IS_QUX(unit)) {
    soc_mem_field32_set(unit, IPS_CRVSm, &data, CR_VAL_BMPf, 0x0); 
    res = arad_fill_table_with_entry(unit, IPS_CRVSm, MEM_BLOCK_ANY, &data);
    SOCDNX_IF_ERR_EXIT(res);

    data = 0;
    res = READ_IPS_CREDIT_CONFIGr(unit, &data); 
    SOCDNX_IF_ERR_EXIT(res);
    soc_reg_field_set(unit, IPS_CREDIT_CONFIGr, &data, CR_VAL_SEL_ENABLEf, TRUE);
    res = WRITE_IPS_CREDIT_CONFIGr(unit, data); 
    SOCDNX_IF_ERR_EXIT(res);
    }

    SOC_REG_ABOVE_64_CLEAR(above64); /* set IPS_STORED_CREDITS_USAGE_CONFIGURATIONr */
    soc_reg_above_64_field32_set(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, above64, MUL_PKT_DEQf, 0xf0f0);
    soc_reg_above_64_field32_set(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, above64, MUL_PKT_DEQ_BYTESf, 0x10);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, REG_PORT_ANY, 0, above64));

    /* init CGM counters */
    COMPILER_64_SET(val64, 0xFFFFF, 0xFFFFFFFF);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_PRG_CTR_SETTINGSr(unit, val64));

    /* Enable stamping Fabric Header */
    SOCDNX_IF_ERR_EXIT(READ_ITE_STAMPING_FABRIC_HEADER_ENABLEr(unit, &val64));
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_FAP_PORTf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_FWD_ACTIONf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_LB_KEY_EXT_ENf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_FWDACTION_TDMf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_DP_ENf, 0xff);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, SNP_STAMP_TRAP_CODEf, 1);
    soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_CNI_BITf, 1);
    if (SOC_DPP_CONFIG(unit)->arad->init.mirror_stamp_sys_dsp_ext) 
    {
        /* Enable DSP-Ext stamping for mirrored/snooped packets */
        soc_reg64_field32_set(unit, ITE_STAMPING_FABRIC_HEADER_ENABLEr, &val64, STAMP_DSP_EXT_ENf, 0);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_ITE_STAMPING_FABRIC_HEADER_ENABLEr(unit, val64));

    /* 
     * Read CGM minimum occupancy registers (watermark) in order for 
     * future reads to be correct (the registers set to maximum on read, 
     * but first reads are always 0).
     */
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_BUFFERS_FREE_MIN_STATUSr(unit, &data));
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_PDBS_FREE_MIN_STATUSr(unit, &data));
    SOCDNX_IF_ERR_EXIT(READ_CGM_DRAM_BDBS_FREE_MIN_STATUSr(unit, &data));

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_per_queue_info_set(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          queue,
    SOC_SAND_IN   ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  )
{
    soc_reg_above_64_val_t data_above_64, data2_above_64;
    uint32 offset;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOC_REG_ABOVE_64_CLEAR(data2_above_64);

    offset = queue / 4; /* each entry handles 4 queues */
    /* read */
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_PROFILESm(unit, CGM_BLOCK(unit, core), offset, &data_above_64));
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_VSQS_PRMSm(unit, CGM_BLOCK(unit, core), offset, &data2_above_64));

    switch (queue % 4) {
        /* modify */
        case 0:
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, RATE_CLASSf, IQM_static_tbl_data->rate_class);
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, CREDIT_CLASSf, IQM_static_tbl_data->credit_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, TRAFFIC_CLASSf, IQM_static_tbl_data->traffic_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, CONNECTION_CLASSf, IQM_static_tbl_data->connection_class);
            break;
        case 1:
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, RATE_CLASS_1f, IQM_static_tbl_data->rate_class);
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, CREDIT_CLASS_1f, IQM_static_tbl_data->credit_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, TRAFFIC_CLASS_1f, IQM_static_tbl_data->traffic_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, CONNECTION_CLASS_1f, IQM_static_tbl_data->connection_class);
            break;
        case 2:
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, RATE_CLASS_2f, IQM_static_tbl_data->rate_class);
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, CREDIT_CLASS_2f, IQM_static_tbl_data->credit_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, TRAFFIC_CLASS_2f, IQM_static_tbl_data->traffic_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, CONNECTION_CLASS_2f, IQM_static_tbl_data->connection_class);
            break;
        case 3:
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, RATE_CLASS_3f, IQM_static_tbl_data->rate_class);
            soc_CGM_VOQ_PROFILESm_field32_set(unit, &data_above_64, CREDIT_CLASS_3f, IQM_static_tbl_data->credit_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, TRAFFIC_CLASS_3f, IQM_static_tbl_data->traffic_class);
            soc_CGM_VOQ_VSQS_PRMSm_field32_set(unit, &data2_above_64, CONNECTION_CLASS_3f, IQM_static_tbl_data->connection_class);
            break;
    }
    /* write */
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_PROFILESm(unit, CGM_BLOCK(unit, core), offset, &data_above_64));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_VSQS_PRMSm(unit, CGM_BLOCK(unit, core), offset, &data2_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_per_queue_info_get(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          queue,
    SOC_SAND_OUT   ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  )
{
    soc_reg_above_64_val_t data_above_64, data2_above_64;
    uint32 offset;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOC_REG_ABOVE_64_CLEAR(data2_above_64);
    IQM_static_tbl_data->que_signature = 0; /* no signature in QAX */

    offset = queue / 4; /* each entry handles 4 queues */
    /* read */
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_PROFILESm(unit, CGM_BLOCK(unit, core), offset, &data_above_64));
    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_VSQS_PRMSm(unit, CGM_BLOCK(unit, core), offset, &data2_above_64));

    switch (queue % 4) {
        /* modify */
        case 0:
            IQM_static_tbl_data->rate_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, RATE_CLASSf);
            IQM_static_tbl_data->credit_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, CREDIT_CLASSf);
            IQM_static_tbl_data->traffic_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, TRAFFIC_CLASSf);
            IQM_static_tbl_data->connection_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, CONNECTION_CLASSf);
            break;
        case 1:
            IQM_static_tbl_data->rate_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, RATE_CLASS_1f);
            IQM_static_tbl_data->credit_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, CREDIT_CLASS_1f);
            IQM_static_tbl_data->traffic_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, TRAFFIC_CLASS_1f);
            IQM_static_tbl_data->connection_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, CONNECTION_CLASS_1f);
            break;
        case 2:
            IQM_static_tbl_data->rate_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, RATE_CLASS_2f);
            IQM_static_tbl_data->credit_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, CREDIT_CLASS_2f);
            IQM_static_tbl_data->traffic_class  = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, TRAFFIC_CLASS_2f);
            IQM_static_tbl_data->connection_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, CONNECTION_CLASS_2f);
            break;
        case 3:
            IQM_static_tbl_data->rate_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, RATE_CLASS_3f);
            IQM_static_tbl_data->credit_class = soc_CGM_VOQ_PROFILESm_field32_get(unit, &data_above_64, CREDIT_CLASS_3f);
            IQM_static_tbl_data->traffic_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, TRAFFIC_CLASS_3f);
            IQM_static_tbl_data->connection_class = soc_CGM_VOQ_VSQS_PRMSm_field32_get(unit, &data2_above_64, CONNECTION_CLASS_3f);
            break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_profile_ocb_only_set(
    SOC_SAND_IN   int unit,
    SOC_SAND_IN   int rate_class,
    SOC_SAND_IN   int is_ocb_only
  )
{
    uint32 data;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_SRAM_DRAM_ONLY_MODEm(unit, MEM_BLOCK_ALL, rate_class, &data));
    /* 0 - regular mode (mix), 1 - ocb only mode */
    soc_mem_field32_set(unit, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, &data, SRAM_DRAM_ONLY_MODEf, is_ocb_only);
     
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_VOQ_SRAM_DRAM_ONLY_MODEm(unit, MEM_BLOCK_ALL, rate_class, &data));         
        
exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_itm_profile_ocb_only_get(
    SOC_SAND_IN   int unit,
    SOC_SAND_IN   int rate_class,
    SOC_SAND_OUT  int *is_ocb_only
  )
{
    uint32 data, field;


    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_VOQ_SRAM_DRAM_ONLY_MODEm(unit, MEM_BLOCK_ALL, rate_class, &data));
    /* 0 - regular mode (mix), 1 - ocb only mode */
    field = soc_mem_field32_get(unit, CGM_VOQ_SRAM_DRAM_ONLY_MODEm, &data, SRAM_DRAM_ONLY_MODEf);
    
    *is_ocb_only = (int)field;        
        
exit:
    SOCDNX_FUNC_RETURN;
}

/*
Get QAX ingress congestion statistics.
*/
int qax_itm_congestion_statistics_get(
  SOC_SAND_IN int unit,
  SOC_SAND_IN int core,
  SOC_SAND_OUT ARAD_ITM_CGM_CONGENSTION_STATS *stats /* place current statistics output here */
  )
{
    uint32 val;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(stats);

    if (!SOC_UNIT_NUM_VALID(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNIT);
    } 
    if (((core < 0) || (core > SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores)) && (core != SOC_CORE_ALL)) { 
        SOCDNX_IF_ERR_EXIT(SOC_E_PARAM); 
    }

    /* collect current value statistics */  

    /* Instantaneous SRAM-Buffers free counter */
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_BUFFERS_FREE_STATUSr(unit, &val));
    stats->sram_buf_free = val; /* place the value into the 32 bits integer */	

    /* Minimal SRAM-Buffers free counter */
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_BUFFERS_FREE_MIN_STATUSr(unit, &val));
    stats->sram_buf_min_free = val; /* place the value into the 32 bits integer */		

    /* Instantaneous SRAM-PDBs free counter */
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_PDBS_FREE_STATUSr(unit, &val));
    stats->sram_pdbs_free = val; /* place the value into the 32 bits integer */	

    /* Minimal SRAM-PDBs free counter */
    SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_PDBS_FREE_MIN_STATUSr(unit, &val));
    stats->sram_pdbs_min_free = val; /* place the value into the 32 bits integer */		

    /* Instantaneous DRAM-BDBs free counter */
    SOCDNX_IF_ERR_EXIT(READ_CGM_DRAM_BDBS_FREE_STATUSr(unit, &val));
    stats->bdb_free = val; /* place the value into the 32 bits integer */			

    /* Minimal DRAM-BDBs free counter */
    SOCDNX_IF_ERR_EXIT(READ_CGM_DRAM_BDBS_FREE_MIN_STATUSr(unit, &val));
    stats->min_bdb_free = val; /* place the value into the 32 bits integer */

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Get QAX ingress minimal free resources.
 * The counters clear on read.
 */
int qax_itm_min_free_resources_stat_get(
  SOC_SAND_IN int unit,
  SOC_SAND_IN int core,
  SOC_SAND_IN SOC_TMC_ITM_CGM_RSRC_STAT_TYPE type,
  SOC_SAND_OUT uint64 *value
  )
{
    uint32 val32;

    SOCDNX_INIT_FUNC_DEFS;

    switch (type) {
        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_BDB:
            /* Minimal DRAM-BDBs free counter */
            SOCDNX_IF_ERR_EXIT(READ_CGM_DRAM_BDBS_FREE_MIN_STATUSr(unit, &val32));
            break;
        
        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_SRAM_BUFFERS:
            /* Minimal SRAM-Buffers free counter */
            SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_BUFFERS_FREE_MIN_STATUSr(unit, &val32));
            break;

        case SOC_TMC_ITM_CGM_RSRC_STAT_MIN_FREE_SRAM_PDB:
            /* Minimal SRAM-PDBs free counter */
            SOCDNX_IF_ERR_EXIT(READ_CGM_SRAM_PDBS_FREE_MIN_STATUSr(unit, &val32));
            break;

        default:
            SOCDNX_IF_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_SOC_MSG("Resource statistic type not supported for this device.")));
    }

    COMPILER_64_SET(*value, 0, val32);

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Set the drop precedence value above which 
*     all packets will always be discarded.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int
  qax_itm_dp_discard_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  discard_dp
  )
{     
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_itm_dp_discard_set_verify(unit, discard_dp));

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, CGM_GLBL_RJCT_PRMSr, SOC_CORE_ALL, 0, DP_LEVEL_RJCT_THf, discard_dp));

exit:
    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
*     Get the drop precedence value above which 
*     all packets will always be discarded.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int
  qax_itm_dp_discard_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  )
{     
    uint32 fld_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(discard_dp);

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_read(unit, CGM_GLBL_RJCT_PRMSr, SOC_CORE_ALL, 0, DP_LEVEL_RJCT_THf, &fld_val));

    *discard_dp = fld_val;

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

