/*
 * $Id: drv.h,v 1.0 Broadcom SDK $
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
 */

#ifdef _ERR_MSG_MODULE_NAME 
    #error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

/* 
 * Includes
 */ 
#include <shared/bsl.h>
#include <shared/bitop.h>

/* SAL includes */
#include <sal/appl/sal.h>

/* SOC includes */
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/iproc.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>

/* SOC DPP includes */
#include <soc/dpp/drv.h>
#include <soc/dcmn/dcmn_mem.h>

/* SOC DPP JER includes */
#include <soc/dpp/JER/jer_tbls.h>
#include <soc/dpp/JER/jer_fabric.h>

/* SOC DPP Arad includes */ 
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>


/* 
 * Defines
 */

#define JER_MBMP_SET_EXCLUDED(_mem)      SHR_BITSET(jer_excluded_mems_bmap_p, _mem)
#define JER_MBMP_IS_EXCLUDED(_mem)       SHR_BITGET( jer_excluded_mems_bmap_p, _mem)

static soc_mem_t jer_tbls_88375_excluded_mem_list[] = {

    /* Has to be last memory in array */
    INVALIDm
};

static soc_mem_t jer_tbls_88670_excluded_mem_list[] = {

    /* Has to be last memory in array */
    INVALIDm
};



/* 
 * Tables Init Functions
 */ 



uint32 soc_jer_sch_tbls_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

/*exit:*/
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_irr_tbls_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

/*exit:*/
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_ire_tbls_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

/*exit:*/
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_ihb_tbls_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

/*exit:*/
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_iqm_tbls_init( int unit)
{
    uint32 table_default[SOC_MAX_MEM_WORDS] = {0xaaaaaaaa};
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize IQM_PACKING_MODE_TABLE*/
    /* each line in the table configures fabric-pcp mode of 16 devices*/
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, IQM_PACK_MODEm, MEM_BLOCK_ALL, table_default));
    
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_ips_tbls_init(int unit)
{

    SOCDNX_INIT_FUNC_DEFS;

/* exit: */
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_ipt_tbls_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(arad_mgmt_ipt_tbls_init(unit, 1));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_fdt_tbls_init(int unit)
{
    uint32 table_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 pcp_config_data = 2;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_DPP_IS_MESH(unit)) {
        /* 
         * initialize table for fabric configuration
         */

        /* fabric-pcp is configured in bits 6:7 of each line */
        SHR_BITCOPY_RANGE(table_default, 6, &pcp_config_data, 0, SOC_JER_FABRIC_PCP_LENGTH);
    }

    /* Initialize FDT_IPT_MESH_MC table */
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, FDT_IPT_MESH_MCm, MEM_BLOCK_ALL, table_default));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_egq_tbls_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

/*exit:*/
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_epni_tbls_init(int unit)
{
    uint32 res = 0;
    uint32 tx_tag_table_entry[8] = {0};
    uint32 table_default[3] = {0x10040100, 0x04010040, 0x4010};
    SOCDNX_INIT_FUNC_DEFS;

    /* initialize EPNI_MIRROR_PROFILE_TABLE*/
    SOCDNX_IF_ERR_EXIT(dcmn_fill_table_with_entry(unit, EPNI_MIRROR_PROFILE_TABLEm, MEM_BLOCK_ALL, table_default));
    

    res = soc_sand_bitstream_fill(tx_tag_table_entry, 8);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_fill_table_with_entry(unit, EPNI_TX_TAG_TABLEm, MEM_BLOCK_ANY, &tx_tag_table_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res);


exit:
    SOCDNX_FUNC_RETURN;
}



/*
 * Function:
 *      soc_jer_excluded_tbls_list_set
 * Purpose:
 *      sets the excluded memory list with the relevant memories
 * Parameters:
 *      unit    - Device Number 
 * Returns:
 *      SOC_E_XXX 
 * Note:
 *      to insert a memory to excluded list write the memory's name in the relevant exclude list above
 */
int soc_jer_excluded_tbls_list_set(int unit) 
{
    SHR_BITDCL *jer_excluded_mems_bmap_p = NULL;
    int mem_iter = 0;
    soc_mem_t* excluded_list;

    SOCDNX_INIT_FUNC_DEFS;

    /* get relevant exclude mems bmap of wanted device and define jer_excluded_mems_bmap_p for MACROs */
    jer_excluded_mems_bmap_p = SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap;

    /* get relevant exclude mems list of wanted device type */
    if (SOC_IS_JERICHO_A0(unit)) {
        excluded_list = jer_tbls_88670_excluded_mem_list;
    } else if(/* SOC_IS_QMX_A0(unit) */ 0){ 
        excluded_list = jer_tbls_88375_excluded_mem_list;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unknown Device Type\n")));
    }

    /* set exclude bmap to zero */
    sal_memset(jer_excluded_mems_bmap_p, 0, SHR_BITALLOCSIZE(NUM_SOC_MEM));

    /* iterate over exclude list to set bmap */
    mem_iter = 0;
    while (excluded_list[mem_iter] != INVALIDm) {
        JER_MBMP_SET_EXCLUDED(excluded_list[mem_iter]);
        ++mem_iter;
    }

exit:
    SOCDNX_FUNC_RETURN;     
}

/*
 * Function:
 *      soc_jer_static_tbls_reset
 * Purpose:
 *      iterates over all memories and resets the static ones
 * Parameters:
 *      unit    - Device Number
 * Returns:
 *      SOC_E_XXX
 */
int soc_jer_static_tbls_reset (int unit)
{
    SHR_BITDCL *jer_excluded_mems_bmap_p = NULL;
    int mem_iter = 0;
    uint32 reset_value[SOC_JER_TBLS_MAX_WORDS_FOR_REGULAR_STATIC_TABLE_ENTRY] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    
    /* set excluded mem list */
    SOCDNX_IF_ERR_EXIT(soc_jer_excluded_tbls_list_set(unit));

    /* define jer_excluded_mems_bmap_p for MACROs */
    jer_excluded_mems_bmap_p = SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap;

    /* iterate over all mems */
    for (mem_iter = 0; mem_iter < NUM_SOC_MEM; mem_iter++) 
    {
        /* if not a valid mem */
        if (!SOC_MEM_IS_VALID(unit, mem_iter)) {
            continue;
        }

        /* if table is a static table */
        if (soc_mem_flags(unit, mem_iter) & SOC_MEM_FLAG_SIGNAL) {
            continue;
        }
        /* if mem is read only */
        if (soc_mem_flags(unit, mem_iter) & SOC_MEM_FLAG_READONLY) {
            continue;
        }
        
        /* if entry is bigger than max entry size and this is a wide mem that should be ignored */
        if (soc_mem_entry_words(unit, mem_iter) > SOC_JER_TBLS_MAX_WORDS_FOR_REGULAR_STATIC_TABLE_ENTRY) {
            continue;
        }

        /* if mem is in excluded list */
        if(JER_MBMP_IS_EXCLUDED(mem_iter)) {
            continue;
        }

        /* reset memory - set all values to 0 */
        LOG_VERBOSE(BSL_LS_SOC_INIT,(BSL_META_U(unit,"Reseting static memory # %d - %s\n"),mem_iter, SOC_MEM_NAME(unit, mem_iter)));
        SOCDNX_IF_ERR_EXIT( dcmn_fill_table_with_entry(unit, mem_iter, MEM_BLOCK_ALL, reset_value));
    }

exit:
    SOCDNX_FUNC_RETURN;     
}
