/*
 * $Id: jer_tdm.c, v1 18/11/2014 09:55:39 azarrin $
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
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TDM
/*************
 * INCLUDES  *
 *************/
#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_tdm.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_sw_db.h>

/*********************************************************************
* NAME:
*     jer_tdm_init
* FUNCTION:
*     Initialization of the TDM configuration depends on the tdm mode.
* INPUT:
*    int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
int
  jer_tdm_init(int unit)
{
    uint32
        fld_val,
        tdm_found,
        tdm_egress_priority,
        tm_port;
    ARAD_MGMT_TDM_MODE
        tdm_mode;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
        ilkn_tdm_dedicated_queuing;
        uint8
        is_local;
    soc_reg_above_64_val_t
        data;
    int fabric_priority;
    int core, port_i;
    uint32 is_valid;
    SOC_TMC_EGR_OFP_SCH_INFO ofp_sch_info;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);

    SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_tdm_unit_has_tdm(unit,&tdm_found)));
    tdm_mode = SOC_DPP_CONFIG(unit)->arad->init.tdm_mode;
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;  
    tdm_egress_priority = SOC_DPP_CONFIG(unit)->arad->init.tdm_egress_priority;
    fabric_priority = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_tdm_priority_min;

    arad_sw_db_tdm_mode_set(unit, tdm_mode);
    arad_sw_db_ilkn_tdm_dedicated_queuing_set(unit, ilkn_tdm_dedicated_queuing);

    /* TDM packet size limit range 65-254 */
    fld_val = ARAD_TDM_CELL_SIZE_MIN;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IRE_TDM_SIZEr, REG_PORT_ANY, 0, TDM_MIN_SIZEf,  fld_val));

    fld_val = ARAD_TDM_CELL_SIZE_MAX;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IRE_TDM_SIZEr, REG_PORT_ANY, 0, TDM_MAX_SIZEf,  fld_val));    

    /* IRE FTMH version for TDM packet to identify the packets as TDM flows. */
    fld_val = ARAD_TDM_VERSION_ID;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FTMH_VERSIONf, fld_val));

    /* IPT TDM enable */
    fld_val = 0x1;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IPT_ENABLESr, REG_PORT_ANY, 0, TDM_ENf,  fld_val));

    /* Enable push queue for TDM packets */
    fld_val = ARAD_TDM_PUSH_QUEUE_TYPE;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPS_PUSH_QUEUE_TYPES_CONFIGr, SOC_CORE_ALL, 0, PUSH_QUEUE_TYPEf,  fld_val));

    fld_val = 0x1;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPS_PUSH_QUEUE_TYPES_CONFIGr, SOC_CORE_ALL, 0, PUSH_QUEUE_TYPE_ENf,  fld_val));

    /*
    * In NON Fabric mode, enable traffic tdm local only.
    * Note that fabric module must be initialize before TDM module
    */
    if (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_STA)
    {
        is_local = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP)? TRUE:FALSE;

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, FORCE_ALL_LOCALf,  is_local));
    }

    /* 
    * MODE #2 does not read from RTP - only looks at the link status. 
    * Used for TDM static routing.
    */
    fld_val = 0x2;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_LINK_BITMAP_CONFIGURATIONr, REG_PORT_ANY, 0, IRE_TDM_MASK_MODEf,  fld_val));

    /* Fabric MC from TDM at mesh mode is enabled */
    fld_val = 1;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, TDM_FMC_ENf,  fld_val));

    /* Mesh MC replication bitmap from TDM will be taken from MeshMc table in FDT. Note that at this mode the number of remote replications is limited to 2.*/
    fld_val = 1;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, TDM_MESH_MC_BMP_SRCf,  fld_val));

    /* FDA TDM priority */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDA_FDA_ENABLERSr, REG_PORT_ANY, 0, TDM_HEADER_PRIORITYf,  fabric_priority));

    /* TDM SP MODE CONFIGURATION*/
    if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
        /*Enable ilkn_tdm_dedicated_queueing*/
        fld_val = 0x1;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));
    }

    /* TDM general configuration */  
    if (tdm_found) /* TDM bypass is enabled */
    {
        /* Enable tdm cell mode only */
        fld_val = 0x1;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, EGRESS_TDM_MODEf,  fld_val));

        /* Enable 2 bytes ftmh only in optimize ftmh mode */
        fld_val = (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT)?0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_2_BYTES_FTMHf,  fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_2_BYTES_FTMHf,  fld_val));        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_TDM_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_FTMH_OPTIMIZEDf,  fld_val));
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_CONTEXT_MODEf, tdm_found));

    /* TDM MC use only VLAN membership table (i.e. no need for TDM special format) */
    fld_val = 0;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, SOC_CORE_ALL, 0, TDM_REP_FORMAT_ENf,  fld_val));

    /* TDM egress priority configuration */
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_PKT_TCf,  tdm_egress_priority));

    /* TDM Always High Priority Scheduling */
    SOC_TMC_EGR_OFP_SCH_INFO_clear(&ofp_sch_info);

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
        /* Invalid port */
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
        if (!is_valid) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));

        if (IS_TDM_PORT(unit, port_i)) {
            SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_egr_ofp_scheduling_get(unit, core, tm_port, &ofp_sch_info)));
            ofp_sch_info.nif_priority = SOC_TMC_EGR_OFP_INTERFACE_PRIO_HIGH;
            SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_egr_ofp_scheduling_set(unit, core, tm_port, &ofp_sch_info)));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

