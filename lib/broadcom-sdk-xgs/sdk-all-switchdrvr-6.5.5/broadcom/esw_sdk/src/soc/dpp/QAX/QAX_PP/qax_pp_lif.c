/* $Id: qax_pp_lif.c,v 1.29 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LIF

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/register.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/QAX/QAX_PP/qax_pp_lif.h>
#include  <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/PPC/ppc_api_lif.h>


#define QAX_PP_LIF_NOF_DEFAULT_NATIVE_OUTLIFS (4)

soc_error_t qax_pp_lif_default_native_ac_outlif_init(int unit) {
    soc_reg_above_64_val_t reg_above_64_val; 
    int default_outlif_id = 0;

    /* we init default outlif with an invalid AC outlif value: 
       this outlif points to a null entry, allocated at init */
    uint32 out_lif_null_entry = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID); 
    SOCDNX_INIT_FUNC_DEFS;
     
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 

    for (default_outlif_id = 0; default_outlif_id < QAX_PP_LIF_NOF_DEFAULT_NATIVE_OUTLIFS; default_outlif_id++) {
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, default_outlif_id * SOC_DPP_DEFS_GET(unit, out_lif_nof_bits), &out_lif_null_entry, 0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits)); 
    }
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_NATIVE_DEFAULT_AC_POINTERSr(unit, reg_above_64_val)); 
exit:
  SOCDNX_FUNC_RETURN;

}

soc_error_t qax_pp_lif_default_native_ac_outlif_set(int unit, uint32 local_out_lif_id) {

    soc_reg_above_64_val_t reg_above_64_val; 
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 

    /* check that we're not copying above source size */
    SOCDNX_VERIFY(SOC_DPP_DEFS_GET(unit, out_lif_nof_bits) <= 8*sizeof(local_out_lif_id)); 

    SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, 0, &local_out_lif_id, 0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits)); 

    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_CFG_NATIVE_DEFAULT_AC_POINTERSr(unit, reg_above_64_val)); 
                                      

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t qax_pp_lif_default_native_ac_outlif_get(int unit, uint32* local_out_lif_id) {

    soc_reg_above_64_val_t reg_above_64_val; 
    SOCDNX_INIT_FUNC_DEFS;

    /* check that we're not copying above destination size */
    SOCDNX_VERIFY(SOC_DPP_DEFS_GET(unit, out_lif_nof_bits) <= 8*sizeof(*local_out_lif_id)); 

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val); 
    SOCDNX_IF_ERR_EXIT(READ_EPNI_CFG_NATIVE_DEFAULT_AC_POINTERSr(unit, reg_above_64_val)); 

    SOC_REG_ABOVE_64_RANGE_COPY(local_out_lif_id, 0, reg_above_64_val, 0, SOC_DPP_DEFS_GET(unit, out_lif_nof_bits)); 

exit:
  SOCDNX_FUNC_RETURN;
}


