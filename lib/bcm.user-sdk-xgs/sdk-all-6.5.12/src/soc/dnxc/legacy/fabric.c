/*
 * $Id: fabric.c Exp $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_INIT
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/dnxc/legacy/fabric.h>
#include <soc/dnxc/legacy/error.h>
#include <shared/shrextend/shrextend_debug.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/legacy/mbcm.h>
#endif


int
soc_dnxc_fabric_pipe_map_initalize_valid_configurations(int unit, int min_tdm_priority, soc_dnxc_fabric_pipe_map_t* fabric_pipe_map_valid_config)
{
    int *config_uc;
    int *config_mc;
    int i, index = 0;
    SHR_FUNC_INIT_VARS(unit);

    /* UC, MC, TDM configuration - uc={0,0,0,2} mc={1,1,1,2}  ([uc/mc x priority]-> pipe_num )*/
    sal_strncpy(fabric_pipe_map_valid_config[index].name ,"UC,MC,TDM", sizeof(fabric_pipe_map_valid_config[0].name));
    fabric_pipe_map_valid_config[index].nof_pipes=3;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=config_mc[3]=1;
    for (i = min_tdm_priority; i < SOC_DNXC_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES; i++)
    {
        config_uc[i] = config_mc[i] = 2;
    }
    
    index++;

    /* UC, HP MC (3), LP MC (0, 1, 2) Configuration -  uc={0,0,0,0} mc={2,2,2,1} ([uc/mc x priority] -> pipe_num )*/
    sal_strncpy(fabric_pipe_map_valid_config[index].name,"UC,HP MC (3),LP MC", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=3;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=2; config_mc[3]=1;

    index++;

    /* UC, HP MC (2, 3), LP MC (0, 1) (Configuration -  uc={0,0,0,0} mc={2,2,1,1} ([uc/mc x priority] -> pipe_num )*/
    sal_strncpy(fabric_pipe_map_valid_config[index].name,"UC,HP MC (2,3),LP MC", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=3;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=2;config_mc[2]=config_mc[3]=1;

    index++;

    /* UC, HP MC (1, 2, 3), LP MC (0) Configuration -  uc={0,0,0,0} mc={2,1,1,1} ([uc/mc x priority] -> pipe_num )*/
    sal_strncpy(fabric_pipe_map_valid_config[index].name,"UC,HP MC (1,2,3),LP MC", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=3;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=2;config_mc[1]=config_mc[2]=config_mc[3]=1;

    index++;

    /* NON_TDM , TDM Configuration - uc={0,0,0,1} mc={0,0,0,1} ([uc/mc x priority] -> pipe_num )*/
    sal_strncpy(fabric_pipe_map_valid_config[index].name, "NON_TDM,TDM", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=2;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=config_mc[3]=0;
    for (i = min_tdm_priority; i < SOC_DNXC_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES; i++)
    {
        config_uc[i] = config_mc[i] = 1;
    }

    index++;

    /* UC,MC Configuration - uc={0,0,0,0} mc={1,1,1,1} ([uc/mc x priority] -> pipe_num) */
    sal_strncpy(fabric_pipe_map_valid_config[index].name, "UC,MC", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=2;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dnxc_fabric_pipe_map_dual_uc_mc;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=config_mc[3]=1;

    SHR_FUNC_EXIT;
}

int
soc_dnxc_fabric_mesh_topology_diag_get(int unit, soc_dnxc_fabric_mesh_topology_diag_t *mesh_topology_diag)
{
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {

        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_diag_mesh_topology_get, (unit, mesh_topology_diag)));
    } else
#endif /*BCM_DNXF_SUPPORT*/
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        if (!soc_feature(unit, soc_feature_no_fabric)) {
            SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_mesh_topology_get, (unit, mesh_topology_diag)));
        }
    } else
#endif /*BCM_DNX_SUPPORT*/
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "feature unavail");
    }

exit:
    SHR_FUNC_EXIT;
}

