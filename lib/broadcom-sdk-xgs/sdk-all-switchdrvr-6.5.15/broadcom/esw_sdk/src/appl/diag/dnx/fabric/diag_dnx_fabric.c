/** \file diag_dnx_fabric.c
 * 
 * diagnostic pack for fabric
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>

/*dnx-data*/
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fabric.h>

/*bcm*/
#include <bcm/fabric.h>

/*soc*/
#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnxc/legacy/fabric.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnx/legacy/mbcm.h>

/*sal*/
#include <sal/appl/sal.h>

/*
 * }
 */

 /*
  * LOCAL DEFINEs:
  * {
  */

#define DIAG_DNX_PORT_RANGE_STR_LENGTH      (SOC_MAX_NUM_PORTS + 3)

 /*
  * LOCAL DEFINEs:
  * }
  */

 /*
  * LOCAL TYPEs:
  * }
  */

 /*
  * LOCAL TYPEs:
  * }
  */

/*
 * LOCAL FUNCTIONs:
 * {
 */

/*
 * LOCAL FUNCTIONs:
 * }
 */

/*
 * LOCAL DIAG PACK:
 * {
 */

/*
 * Reachability
 */

/* *INDENT-OFF* */
sh_sand_man_t dnx_fabric_mesh_man = {
    .brief =    "Diagnostic for mesh mode",
    .full =     "Diagnostic for mesh mode",
    .synopsis = "fabric mesh",
    .examples = "mesh\n",
};
/* *INDENT-ON* */

shr_error_e
sh_cmd_is_dnx_mesh(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNXF(unit))      /* DNXF - not supported */
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }
    else if (SOC_IS_DNX(unit))
    {
        char *connect_mode = soc_property_get_str(unit, "fabric_connect_mode");
        /** Should return success only if the mode is defined and is MESH. otherwise ERROR */
        if ((connect_mode == NULL) || sal_strcasecmp(connect_mode, "MESH"))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        }
    }
    else
    {   /** Should not reach this choice - just in case */
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
sh_cmd_is_dnx_clos(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNXF(unit))      /* DNXF - not supported */
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }
    else if (SOC_IS_DNX(unit))
    {
        char *connect_mode = soc_property_get_str(unit, "fabric_connect_mode");
        /** Should return success only if the mode is defined and is FE(clos) otherwise ERROR */
        if ((connect_mode == NULL) || sal_strcasecmp(connect_mode, "FE"))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        }
    }
    else
    {   /** Should not reach this choice - just in case */
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - display fabric mesh info
 */
shr_error_e
cmd_dnx_fabric_mesh(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i, j, dest;
    char *mesh_type = NULL;
    int is_mesh_mc;
    int fap_id_group_count;
    soc_module_t fap_id_group_array[3];
    int links_group_count;
    soc_module_t links_group_array[DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS];
    _shr_pbmp_t group_bmp, link_bmp;
    char range_format_buf[DIAG_DNX_PORT_RANGE_STR_LENGTH];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    is_mesh_mc = (dnx_data_fabric.mesh.multicast_enable_get(unit) == 1);
    mesh_type = is_mesh_mc ? "Mesh_MC" : "Mesh";

    /*
     * Print mesh diag topic
     */
    LOG_CLI(("MESH DIAG\n"));

    /*
     * Print fabric mesh table
     */
    PRT_TITLE_SET("Fabric Mesh Type: %s", mesh_type);
    PRT_COLUMN_ADD("Group Dest");
    PRT_COLUMN_ADD("FAP IDs");
    PRT_COLUMN_ADD("Links");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    for (i = 0; i < dnx_data_fabric.mesh.nof_max_dest_get(unit); ++i)
    {
        PRT_CELL_SET("%d", i);
        dest = BCM_FABRIC_GROUP_MODID_SET(i);
        SHR_IF_ERR_EXIT(bcm_fabric_modid_group_get(unit, dest,
                                                   dnx_data_fabric.mesh.nof_max_dest_get(unit),
                                                   fap_id_group_array, &fap_id_group_count));
        SHR_IF_ERR_EXIT(bcm_fabric_link_topology_get(unit, dest,
                                                     DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS,
                                                     &links_group_count, links_group_array));

        _SHR_PBMP_CLEAR(group_bmp);
        _SHR_PBMP_CLEAR(link_bmp);
        if (fap_id_group_count)
        {
            for (j = 0; j < fap_id_group_count; ++j)
            {
                _SHR_PBMP_PORT_ADD(group_bmp, fap_id_group_array[j]);
            }
            shr_pbmp_range_format(group_bmp, range_format_buf, DIAG_DNX_PORT_RANGE_STR_LENGTH);
            PRT_CELL_SET("%s", range_format_buf);
        }
        else
        {
            PRT_CELL_SET("-");
        }
        if (links_group_count)
        {
            for (j = 0; j < links_group_count; ++j)
            {
                _SHR_PBMP_PORT_ADD(link_bmp, links_group_array[j]);
            }
            shr_pbmp_range_format(link_bmp, range_format_buf, DIAG_DNX_PORT_RANGE_STR_LENGTH);
            PRT_CELL_SET("%s", range_format_buf);
        }
        else
        {
            PRT_CELL_SET("-");
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/* Link the values in the two enums*/
sh_sand_enum_t dnx_fabric_force_enum_table[] = {
    {"FABRIC", socDnxFabricForceFabric, "Force Fabric"},
    {"CORE0", socDnxFabricForceLocal0, "Force Local Core 0"},
    {"CORE1", socDnxFabricForceLocal1, "Force Local Core 1"},
    {"DEFAULT", socDnxFabricForceRestore, "Restore default"},
    {NULL}
};

sh_sand_man_t dnx_fabric_force_man = {
    .brief    = "Diagnostic for fabric force.",
    .full     = "Map packets with all destination IDs to fabric/local core 0/local core 1.\n"
                "Not to be done when traffic is enabled.\n"
                "To disable traffic, use bcm_stk_module_enable and bcm_fabric_control_set with bcmFabricControlCellsEnable parameter.",
    .synopsis = "[fabric/core0/core1/default]",
    .examples = "fabric\n"
                "core0\n"
                "default\n",
};
sh_sand_option_t dnx_fabric_force_options[] = {
    /*name   type                 desc          default ext*/
    {"type", SAL_FIELD_TYPE_ENUM, "force type", NULL,   (void *)dnx_fabric_force_enum_table, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/* *INDENT-ON* */

/**
 * \brief - fabric force command
 */
shr_error_e
cmd_dnx_fabric_force(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int force;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("type", force);

    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_force_set, (unit, force)));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
