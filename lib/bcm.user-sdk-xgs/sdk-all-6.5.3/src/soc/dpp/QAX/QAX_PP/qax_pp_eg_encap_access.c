
/* $Id: qax_pp_eg_encap_access.c,v 1.20 Broadcom SDK $
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
*/
#include <soc/mcm/memregs.h>

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS



/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap_access.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_eg_encap_access.h>
#include <soc/dpp/drv.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* Each out rif table row has 8 entries */
#define QAX_PP_OUT_RIF_TBL_NUM_OF_ENTRIES_PER_ROW              (8)

#define QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS                   (8)

#define QAX_PP_OUT_RIF_TBL_NUM_OF_ROWS                         (4096)

#define QAX_PP_OUT_RIF_TBL_ENTRY_SIZE                          (3)

/* Convert from outlif to entry offset */
#define QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_INTERNAL_OFFSET(_offset) \
  (_offset / QAX_PP_OUT_RIF_TBL_NUM_OF_ENTRIES_PER_ROW)

/* Convert from outlif to offset within a row */
#define QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_ROW_OFFSET(_offset) \
  (_offset - (_offset / QAX_PP_OUT_RIF_TBL_NUM_OF_ENTRIES_PER_ROW) * QAX_PP_OUT_RIF_TBL_NUM_OF_ENTRIES_PER_ROW)

#define QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_ARRAY_INDEX(_offset) \
  ((_offset * QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS) / SOC_SAND_NOF_BITS_IN_UINT32)

#define QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_BIT_OFFSET(_offset) \
  ((_offset * QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS) % SOC_SAND_NOF_BITS_IN_UINT32);




/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

soc_error_t
  soc_qax_pp_eg_rif_profile_set(
    int                 unit,
    uint32              rif_profile_index,
    QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT  *tbl_data
  )
{
    uint32 rif_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;

    soc_mem_field32_set(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, DROPf, tbl_data->drop);
    soc_mem_field32_set(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, OAM_LIF_SETf, tbl_data->oam_lif_set);
    soc_mem_field32_set(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, OUTLIF_PROFILEf, tbl_data->outlif_profile);

    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EDB_RIF_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, rif_profile_index, &rif_profile));

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_qax_pp_eg_rif_profile_get(
    int                 unit,
    uint32              rif_profile_index,
    QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT  *tbl_data
  )
{
    uint32 rif_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_SAND_IF_ERR_EXIT(READ_EDB_RIF_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, rif_profile_index, &rif_profile));

    tbl_data->drop = soc_mem_field32_get(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, DROPf);
    tbl_data->oam_lif_set = soc_mem_field32_get(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, OAM_LIF_SETf);
    tbl_data->outlif_profile = soc_mem_field32_get(unit, EDB_RIF_PROFILE_TABLEm, &rif_profile, OUTLIF_PROFILEf);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_qax_pp_eg_rif_profile_delete(
    int                 unit,
    uint32              rif_profile_index
  )
{
    uint32 rif_profile = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EDB_RIF_PROFILE_TABLEm(unit, MEM_BLOCK_ANY, rif_profile_index, &rif_profile));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  qax_pp_eg_encap_access_out_rif_entry_format_tbl_get_unsafe(
    int                 unit,
    uint32              outlif,
    ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  )
{
    uint32
        entry_offset,
        internal_row_offset,
        data[QAX_PP_OUT_RIF_TBL_ENTRY_SIZE],
        entry = 0,
        rif_profile_index,
        data_index,
        data_inner_offset;
    QAX_PP_EG_ENCAP_ACCESS_OUT_RIF_PROFILE_ENTRY_FORMAT out_rif_profile;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*QAX_PP_OUT_RIF_TBL_ENTRY_SIZE);

    entry_offset = QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_INTERNAL_OFFSET(outlif);

    if (entry_offset >= QAX_PP_OUT_RIF_TBL_NUM_OF_ROWS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("Entry offset: %d, is out of range: %d.\n"), entry_offset, QAX_PP_OUT_RIF_TBL_NUM_OF_ROWS - 1));
    }

    SOCDNX_SAND_IF_ERR_EXIT(READ_EDB_OUTRIF_TABLEm(unit, MEM_BLOCK_ANY, entry_offset, data));

    internal_row_offset = QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_ROW_OFFSET(outlif);

    /* Each row is 8 entries of 8 bits each in QAX */
    data_index = QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_ARRAY_INDEX(internal_row_offset);
    data_inner_offset = QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_BIT_OFFSET(internal_row_offset);

    /* First, retrieve the 8 bits associated with our entry */
    entry = (data[data_index] >> data_inner_offset) & ((1 << QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS) - 1);

    /* The remark profile is located in the bottom 4 bits in QAX */
    tbl_data->remark_profile = entry & ((1 << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) - 1);

    /* The rif profile is located in the top 4 bits in QAX */
    rif_profile_index = (entry >> (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) & ((1 << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) - 1);

    SOCDNX_SAND_IF_ERR_EXIT(soc_qax_pp_eg_rif_profile_get(unit, rif_profile_index, &out_rif_profile));

    tbl_data->drop = out_rif_profile.drop;
    tbl_data->oam_lif_set = out_rif_profile.oam_lif_set;
    tbl_data->outlif_profile = out_rif_profile.outlif_profile;
    tbl_data->next_vsi_lsb = outlif; /* In Qax, vsi = outrif. Therefore, this field is not actually a part of the entry, and we fill it manually. */

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  qax_pp_eg_encap_access_out_rif_entry_format_tbl_set_unsafe(
    int                 unit,
    uint32              outlif,
    ARAD_PP_EG_ENCAP_ACCESS_OUT_RIF_ENTRY_FORMAT  *tbl_data
  )
{
    uint32
        entry_offset,
        internal_row_offset,
        data[QAX_PP_OUT_RIF_TBL_ENTRY_SIZE],
        entry = 0,
        data_index,
        data_inner_offset;

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*QAX_PP_OUT_RIF_TBL_ENTRY_SIZE);

    entry_offset = QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_INTERNAL_OFFSET(outlif);

    if (entry_offset >= QAX_PP_OUT_RIF_TBL_NUM_OF_ROWS) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("Entry offset: %d, is out of range: %d.\n"), entry_offset, QAX_PP_OUT_RIF_TBL_NUM_OF_ROWS - 1));
    }

    SOCDNX_SAND_IF_ERR_EXIT(READ_EDB_OUTRIF_TABLEm(unit, MEM_BLOCK_ANY, entry_offset, data));

    internal_row_offset = QAX_PP_OUT_RIF_TBL_ACCESS_OFFSET_TO_ROW_OFFSET(outlif);

    /* Each row is 8 entries of 8 bits each in QAX */
    data_index = QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_ARRAY_INDEX(internal_row_offset);
    data_inner_offset = QAX_PP_OUT_RIF_TBL_ACCESS_ROW_OFFSET_TO_UINT32_BIT_OFFSET(internal_row_offset);
    /* First, zero the 8 bits associated with our entry */
    entry = SOC_SAND_U32_MAX - (((1 << QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS) - 1) << data_inner_offset);
    data[data_index] &= entry;

    /* The remark profile is located in the bottom 4 bits in QAX */
    entry = tbl_data->remark_profile & ((1 << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) - 1);

    /* The rif profile is located in the top 4 bits in QAX */
    entry |= (tbl_data->outrif_profile_index & ((1 << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2)) - 1)) << (QAX_PP_OUT_RIF_TBL_ENTRY_LENGTH_BITS / 2);

    /* Update data with the new rif entry */
    entry <<= data_inner_offset;
    data[data_index] |= entry;

    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EDB_OUTRIF_TABLEm(unit, MEM_BLOCK_ANY, entry_offset, data));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  qax_pp_eg_encap_access_init_outrif_max(
    int                 unit
  )
{
    uint32 nof_outrifs = SOC_DPP_CONFIG(unit)->l3.nof_rifs;

    SOCDNX_INIT_FUNC_DEFS;

    /* These three registers logically signify the same thing (maximum number of outrifs), so they should have the same value. */
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EDB_CFG_OUTRIF_MAXr(unit, nof_outrifs));
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EPNI_OUTRIF_MAX_VALUEr(unit, REG_PORT_ANY, nof_outrifs));
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_EGQ_OUTRIF_MAX_VALUEr(unit, REG_PORT_ANY, nof_outrifs));
    SOCDNX_SAND_IF_ERR_EXIT(
        soc_reg_above_64_field32_modify(unit, ITE_ITPP_GENERAL_CFGr, REG_PORT_ANY, 0, ITPP_OUTRIF_RANGEf, nof_outrifs - 1 /* This register does <= instead of < */));

exit:
    SOCDNX_FUNC_RETURN;
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


