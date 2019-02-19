/*
 * $Id: fe3200_fabric_links.c,v 1.21.20.1 Broadcom SDK $
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
 * SOC FE3200 FABRIC LINKS
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/mbcm.h>

#include <soc/dfe/fe3200/fe3200_fabric_links.h>

 
soc_error_t
soc_fe3200_fabric_links_flow_status_control_cell_format_set(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val) 
{
    uint64 reg64_val,
        bitmap;
    int blk,
        inner_link,
        nof_links_in_ccs;

    SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_ccs = SOC_DFE_DEFS_GET(unit, nof_links_in_ccs);

    blk = link / nof_links_in_ccs;
    inner_link = link % nof_links_in_ccs;

    switch (val)
    {
       case soc_dfe_fabric_link_cell_size_VSC256_V1:
           SOCDNX_IF_ERR_EXIT(READ_CCS_ARAD_BMPr(unit, blk, &reg64_val));
           bitmap = soc_reg64_field_get(unit, CCS_ARAD_BMPr, reg64_val, ARAD_BMPf);
           COMPILER_64_BITSET(bitmap, inner_link);
           soc_reg64_field_set(unit, CCS_ARAD_BMPr, &reg64_val, ARAD_BMPf, bitmap);
           SOCDNX_IF_ERR_EXIT(WRITE_CCS_ARAD_BMPr(unit, blk, reg64_val));
           break;

       case soc_dfe_fabric_link_cell_size_VSC256_V2:
           SOCDNX_IF_ERR_EXIT(READ_CCS_ARAD_BMPr(unit, blk, &reg64_val));
           bitmap = soc_reg64_field_get(unit, CCS_ARAD_BMPr, reg64_val, ARAD_BMPf);
           COMPILER_64_BITCLR(bitmap, inner_link);
           soc_reg64_field_set(unit, CCS_ARAD_BMPr, &reg64_val, ARAD_BMPf, bitmap);
           SOCDNX_IF_ERR_EXIT(WRITE_CCS_ARAD_BMPr(unit, blk, reg64_val));        
           break;

       default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported  cell format val %d"), val));
           break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_links_flow_status_control_cell_format_get(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t *val)
{
    uint64 reg64_val,
        bitmap;
    int blk,
        inner_link,
        is_vsc256_v1,
        nof_links_in_ccs;

    SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_ccs = SOC_DFE_DEFS_GET(unit, nof_links_in_ccs);

    blk = link / nof_links_in_ccs;
    inner_link = link % nof_links_in_ccs;


    SOCDNX_IF_ERR_EXIT(READ_CCS_ARAD_BMPr(unit, blk, &reg64_val));
    bitmap = soc_reg64_field_get(unit, CCS_ARAD_BMPr, reg64_val, ARAD_BMPf);
    is_vsc256_v1 = COMPILER_64_BITTEST(bitmap, inner_link);
    *val = is_vsc256_v1 ? 
        soc_dfe_fabric_link_cell_size_VSC256_V1 : soc_dfe_fabric_link_cell_size_VSC256_V2;                     

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fabric_links_weight_validate(int unit, int val) 
{
   SOCDNX_INIT_FUNC_DEFS;
   
   SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_links_weight_set(int unit, soc_port_t link, int is_prim, int val)
{
   SOCDNX_INIT_FUNC_DEFS;
   
   SOCDNX_FUNC_RETURN;
}



soc_error_t
soc_fe3200_fabric_links_weight_get(int unit, soc_port_t link, int is_prim, int *val)
{
   SOCDNX_INIT_FUNC_DEFS;
   
   SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_links_secondary_only_set(int unit, soc_port_t link, int val)
{
   SOCDNX_INIT_FUNC_DEFS;
   
   SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_links_secondary_only_get(int unit, soc_port_t link, int *val)
{
   SOCDNX_INIT_FUNC_DEFS;
   
   SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe3200_fabric_link_repeater_enable_set
 * Purpose:
 *      Enable/Disable link connected to repeater
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  port number 
 *      enable          - (IN) enable/disable
 *      empty_cell_size - (IN) releavant empty cell size
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */

soc_error_t
soc_fe3200_fabric_link_repeater_enable_set(int unit, soc_port_t port, int enable, int empty_cell_size)
{
    int nof_links_in_mac,
        fmac_instance,
        inner_link, inner_link_enable,
        include_empties;
    uint32 reg32_val;
    uint64 reg64_val;
   SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_mac = SOC_DFE_DEFS_GET(unit, nof_links_in_mac);

    fmac_instance = port / nof_links_in_mac;
    inner_link = port % nof_links_in_mac;


    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_TX_LLFC_CELL_SIZEf, empty_cell_size); 
    soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_TX_LLFC_CELL_SIZE_OVERRIDEf, enable); 
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link, reg32_val));


    /*Shaper should include empty cells iff at least single link (in the quad) connected to a repeater*/
   
   if (enable)
   {
       include_empties = 1;
   } else {
       include_empties = 0;
       /* Check if one of the links in the quad is connected to repeater*/
       for (inner_link = 0; inner_link < nof_links_in_mac; inner_link++)
       {
           SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link, &reg32_val));
           inner_link_enable = soc_reg_field_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg32_val, FMAL_TX_LLFC_CELL_SIZE_OVERRIDEf);

           if (inner_link_enable)
           {
               include_empties = 1;
               break;
           }
       }
   }

   SOCDNX_IF_ERR_EXIT(READ_FMAC_TX_CELL_LIMITr(unit, fmac_instance, &reg64_val));
   soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_LLFC_CELLS_GENf, include_empties); 
   SOCDNX_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_instance, reg64_val));


exit:
   SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe3200_fabric_link_repeater_enable_get
 * Purpose:
 *      Enable/Disable link connected to repeater
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      port            - (IN)  port number 
 *      enable          - (OUT) enable/disable
 *      empty_cell_size - (OUT) releavant empty cell size
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */

soc_error_t
soc_fe3200_fabric_link_repeater_enable_get(int unit, soc_port_t port, int *enable, int *empty_cell_size)
{
    int nof_links_in_mac,
        fmac_instance,
        inner_link;
    uint32 reg32_val;

   SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_mac = SOC_DFE_DEFS_GET(unit, nof_links_in_mac);

    fmac_instance = port / nof_links_in_mac;
    inner_link = port % nof_links_in_mac;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_instance, inner_link, &reg32_val));
    *empty_cell_size = soc_reg_field_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg32_val, FMAL_TX_LLFC_CELL_SIZEf); 
    *enable = soc_reg_field_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg32_val, FMAL_TX_LLFC_CELL_SIZE_OVERRIDEf);

exit:
   SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe3200_fabric_links_pcp_enable_set
 * Purpose:
 *      Enable\Disable Packet-Cell-Packing 
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  port number 
 *      enable  - (IN)  
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
soc_error_t 
soc_fe3200_fabric_links_pcp_enable_set(int unit, soc_port_t port, int enable)
{
    int dch_instance, dcl_instance, fmac_instance;
    int dch_inner_link, dcl_inner_link, fmac_inner_link;
    uint64 reg64_val;
    uint32 reg32_val;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SOCDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
    SOCDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &fmac_instance, &fmac_inner_link, SOC_BLK_FMAC));
    SOCDNX_IF_ERR_EXIT(rv);

    /*Enable/Disable PCP RX*/
    SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_CELL_PACKING_ENr(unit, dch_instance, &reg64_val));
    if (enable)
    {
        COMPILER_64_BITSET(reg64_val, dch_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dch_inner_link);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_CELL_PACKING_ENr(unit, dch_instance, reg64_val));

    /*Enable/Disable PCP TX*/
    SOCDNX_IF_ERR_EXIT(READ_DCL_LINK_CELL_PACKING_ENr(unit, dcl_instance, &reg64_val));
    if (enable)
    {
        COMPILER_64_BITSET(reg64_val, dcl_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dcl_inner_link);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_LINK_CELL_PACKING_ENr(unit, dcl_instance, reg64_val));

    /*Enable/Disable PCP MAC*/
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PCP_ENABLEDf, enable ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe3200_fabric_links_pcp_enable_get
 * Purpose:
 *      Get Packet-Cell-Packing enable
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  port number 
 *      enable  - (OUT) 
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
soc_error_t 
soc_fe3200_fabric_links_pcp_enable_get(int unit, soc_port_t port, int *enable)
{
    int dch_instance;
    int dch_inner_link;
    uint64 reg64_val;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_CELL_PACKING_ENr(unit, dch_instance, &reg64_val));
    *enable = COMPILER_64_BITTEST(reg64_val, dch_inner_link);
    
exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_fe3200_fabric_links_pipe_map_set
 * Purpose:
 *      Set pipe mapping 
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  port number 
 *      pipe_map  - (IN)  
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
#define _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE      (2) /*nof bits*/
#define _SOC_FE3200_REMOTE_PIPE_MAPPING_MAX     (2)
soc_error_t 
soc_fe3200_fabric_links_pipe_map_set(int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t pipe_map)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val;
    int dch_instance,
           dch_inner_link,
           dcl_instance,
           dcl_inner_link,
           fmac_instance,
           fmac_inner_link;
    int rv;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    /*Verify*/
    if (pipe_map.num_of_remote_pipes > _SOC_FE3200_REMOTE_PIPE_MAPPING_MAX)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_BCM_MSG("Max of supported number of pipe mapping is 2")));
    }
    
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port,&dch_instance,&dch_inner_link, SOC_BLK_DCH)); 
    SOCDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
    SOCDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &fmac_instance, &fmac_inner_link, SOC_BLK_FMAC));
    SOCDNX_IF_ERR_EXIT(rv);

    /*
     * nof remote link pipes
     */
    /*DCH*/
    rv = READ_DCH_TWO_PIPES_BMPr(unit, dch_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes == 2 || 
        (pipe_map.num_of_remote_pipes == 0 && SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 2))
    {
        COMPILER_64_BITSET(reg64_val, dch_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dch_inner_link);
    }
    rv = WRITE_DCH_TWO_PIPES_BMPr(unit, dch_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_DCH_THREE_PIPES_BMPr(unit, dch_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes == 0 && SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 3)
    {
        COMPILER_64_BITSET(reg64_val, dch_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dch_inner_link);
    }
    rv = WRITE_DCH_THREE_PIPES_BMPr(unit, dch_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    /*DCL*/
    rv = READ_DCL_TWO_PIPES_BMPr(unit, dcl_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes == 2 ||
        (pipe_map.num_of_remote_pipes == 0 && SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 2))
    {
        COMPILER_64_BITSET(reg64_val, dcl_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dcl_inner_link);
    }
    rv = WRITE_DCL_TWO_PIPES_BMPr(unit, dcl_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_DCL_THREE_PIPES_BMPr(unit, dcl_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes == 0 && SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 3)
    {
        COMPILER_64_BITSET(reg64_val, dcl_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dcl_inner_link);
    }
    rv = WRITE_DCL_THREE_PIPES_BMPr(unit, dcl_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    /*FMAC*/
    rv  = READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link , &reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PARALLEL_DATA_PATHf, 
                      pipe_map.num_of_remote_pipes == 0 ? SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes - 1  : pipe_map.num_of_remote_pipes - 1);
    rv  = WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link , reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);

    /* 
     *remote pipe 0 map
     */
    if (pipe_map.num_of_remote_pipes >= 1)
    {
        /*DCH*/
        rv = READ_DCH_REMOTE_PRI_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, dch_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[0], 0, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
        rv = WRITE_DCH_REMOTE_PRI_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);

        /*DCL*/
        rv = READ_DCL_REMOTE_PRI_PIPE_IDXr(unit, dcl_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, dcl_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[0], 0, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
        rv = WRITE_DCL_REMOTE_PRI_PIPE_IDXr(unit, dcl_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);

    }

    /* 
     *remote pipe 1 map
     */
    if (pipe_map.num_of_remote_pipes >= 2)
    {
        /*DCH*/
        rv = READ_DCH_REMOTE_SEC_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, dch_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[1], 0, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
        rv = WRITE_DCH_REMOTE_SEC_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);

        /*DCL*/
        rv = READ_DCL_REMOTE_SEC_PIPE_IDXr(unit, dcl_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, dcl_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[1], 0, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
        rv = WRITE_DCL_REMOTE_SEC_PIPE_IDXr(unit, dcl_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    /* 
     *Enable/Disable pipe mapping
     */

    /*DCH*/
    rv = READ_DCH_PIPES_MAP_ENr(unit, dch_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes != 0 /*enable*/)
    {
        COMPILER_64_BITSET(reg64_val, dch_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dch_inner_link);
    }
    rv = WRITE_DCH_PIPES_MAP_ENr(unit, dch_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    /*DCL*/
    rv = READ_DCL_PIPES_MAP_ENr(unit, dcl_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes != 0 /*enable*/)
    {
        COMPILER_64_BITSET(reg64_val, dcl_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dcl_inner_link);
    }
    rv = WRITE_DCL_PIPES_MAP_ENr(unit, dcl_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe3200_fabric_links_pipe_map_get
 * Purpose:
 *      Get pipe mapping 
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  port number 
 *      pipe_map  - (IN)  
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
soc_error_t 
soc_fe3200_fabric_links_pipe_map_get(int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t *pipe_map)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val_2_pipes,
           reg64_val;
    int dch_instance,
           dch_inner_link;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SOCDNX_IF_ERR_EXIT(rv);

   /* 
    * Enable/Disable pipe mapping
    */


    /*DCH*/
    rv = READ_DCH_PIPES_MAP_ENr(unit, dch_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (!COMPILER_64_BITTEST(reg64_val, dch_inner_link))
    {
        /*pipe mapping disabled*/
        pipe_map->num_of_remote_pipes= 0;
        SOC_EXIT;
    }

    /*
     * nof remote link pipes
     */
    /*DCH*/
    rv = READ_DCH_TWO_PIPES_BMPr(unit, dch_instance, &reg64_val_2_pipes);
    SOCDNX_IF_ERR_EXIT(rv);
    if (COMPILER_64_BITTEST(reg64_val_2_pipes, dch_inner_link)) {
        pipe_map->num_of_remote_pipes = 2;
    } else {
        pipe_map->num_of_remote_pipes = 1;
    }


    /* 
     *remote pipe 0 map
     */
    if (pipe_map->num_of_remote_pipes >= 1)
    {
        /*DCH*/
        rv = READ_DCH_REMOTE_PRI_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(&pipe_map->remote_pipe_mapping[0], 0, reg_above_64_val, dch_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
    }

    /* 
     *remote pipe 1 map
     */
    if (pipe_map->num_of_remote_pipes >= 2)
    {
        /*DCH*/
        rv = READ_DCH_REMOTE_SEC_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(&pipe_map->remote_pipe_mapping[1], 0, reg_above_64_val, dch_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe3200_fabric_links_cell_format_verify
 * Purpose:
 *      Verify cell format val
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  port number 
 *      val       - (IN)  Cell fotmat
 * Returns:
 *      SOC_E_xxx
 * Notes:
 */
soc_error_t
soc_fe3200_fabric_links_cell_format_verify(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val) 
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The cell format is automatically according to PCP configuration and number pipes.\n")));

exit:
    SOCDNX_FUNC_RETURN;
}


/*
 * Function:
 *      soc_fe1600_fabric_links_cell_format_get
 * Purpose:
 *      Get link cell format
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      link  - (IN)  Link
 *      val   - (OUT) Cell format
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t
soc_fe3200_fabric_links_cell_format_get(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t *val)
{
    SOCDNX_INIT_FUNC_DEFS;

    *val = soc_dfe_fabric_link_cell_size_VSC256_V2;

    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
