/*
 * $Id: fabric.c Exp $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/dcmn/fabric.h>
#include <soc/dcmn/error.h>

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/mbcm.h>
#endif


int
soc_dcmn_drv_fabric_pipe_map_initalize_valid_configurations(int unit, int min_tdm_priority, soc_dcmn_fabric_pipe_map_t* fabric_pipe_map_valid_config)
{
    int *config_uc;
    int *config_mc;
    int i;
    SOCDNX_INIT_FUNC_DEFS;

    /* UC, MC, TDM configuration - uc={0,0,0,2} mc={1,1,1,2}  ([uc/mc x priority]-> pipe_num )*/
    sal_strncpy(fabric_pipe_map_valid_config[0].name ,"UC,MC,TDM", sizeof(fabric_pipe_map_valid_config[0].name));
    fabric_pipe_map_valid_config[0].nof_pipes=3;
    fabric_pipe_map_valid_config[0].mapping_type = soc_dcmn_drv_uc_mc_tdm_config_triple_pipe;
    config_uc = fabric_pipe_map_valid_config[0].config_uc;
    config_mc = fabric_pipe_map_valid_config[0].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=config_mc[3]=1;
    for (i = min_tdm_priority; i < SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES; i++)
    {
        config_uc[i] = config_mc[i] = 2;
    }
    
    /* UC, HP MC, LP MC Configuration -  uc={0,0,0,0} mc={1,1,2,2} ([uc/mc x priority] -> pipe_num )*/
    sal_strncpy(fabric_pipe_map_valid_config[1].name,"UC,HP MC,LP MC", sizeof(fabric_pipe_map_valid_config[1].name));
    fabric_pipe_map_valid_config[1].nof_pipes=3;
    fabric_pipe_map_valid_config[1].mapping_type = soc_dcmn_drv_uc_hp_mc_lp_mc_config_triple_pipe;
    config_uc = fabric_pipe_map_valid_config[1].config_uc;
    config_mc = fabric_pipe_map_valid_config[1].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=1;config_mc[2]=config_mc[3]=2;

    /* NON_TDM , TDM Configuration - uc={0,0,0,1} mc={0,0,0,1} ([uc/mc x priority] -> pipe_num )*/
    sal_strncpy(fabric_pipe_map_valid_config[2].name, "NON_TDM,TDM", sizeof(fabric_pipe_map_valid_config[2].name));
    fabric_pipe_map_valid_config[2].nof_pipes=2;
    fabric_pipe_map_valid_config[2].mapping_type = soc_dcmn_drv_tdm_non_tdm_config_dual_pipe;
    config_uc = fabric_pipe_map_valid_config[2].config_uc;
    config_mc = fabric_pipe_map_valid_config[2].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=config_mc[3]=0;
    for (i = min_tdm_priority; i < SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES; i++)
    {
        config_uc[i] = config_mc[i] = 1;
    }

    /* UC,MC Configuration - uc={0,0,0,0} mc={1,1,1,1} ([uc/mc x priority] -> pipe_num) */
    sal_strncpy(fabric_pipe_map_valid_config[3].name, "UC,MC", sizeof(fabric_pipe_map_valid_config[3].name));
    fabric_pipe_map_valid_config[3].nof_pipes=2;
    fabric_pipe_map_valid_config[3].mapping_type = soc_dcmn_drv_uc_mc_config_dual_pipe;
    config_uc = fabric_pipe_map_valid_config[3].config_uc;
    config_mc = fabric_pipe_map_valid_config[3].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=config_mc[3]=1;

    SOCDNX_FUNC_RETURN;
}

int
soc_dcmn_fabric_mesh_topology_diag_get(int unit, soc_dcmn_fabric_mesh_topology_diag_t *mesh_topology_diag)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit))
    {

        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_diag_mesh_topology_get, (unit, mesh_topology_diag)));
    } else
#endif /*BCM_DFE_SUPPORT*/
#ifdef BCM_DPP_SUPPORT
    if (SOC_IS_DPP(unit))
    {

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_mesh_topology_get, (unit, mesh_topology_diag)));
    } else
#endif /*BCM_DPP_SUPPORT*/
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("feature unavail")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

