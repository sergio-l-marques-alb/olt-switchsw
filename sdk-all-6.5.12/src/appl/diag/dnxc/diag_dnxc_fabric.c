/** \file diag_dnxc_fabric.c
 * 
 * diagnostic pack for fabric
 */
/*
 * $Copyright: (c) 2017 Broadcom.
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
#include <appl/diag/dnxc/diag_dnxc_fabric.h>
#include <appl/diag/dnxf/diag_dnxf_fabric.h>

/*bcm*/
#include <bcm/fabric.h>
/*soc*/
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_cgm.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#endif /* BCM_DNXF_SUPPORT */
/* Put your new common defines in this file*/
#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnxc/legacy/fabric.h>
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

 /*
  * LOCAL DEFINEs:
  * {
  */
#define DIAG_DNXC_MAX_NOF_REACHABILITY_CLMNS        24
#define DIAG_DNXC_MAX_NOF_MESH_TOPOLOGY_CLMNS        7

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
static sh_sand_option_t dnxc_fabric_reachability_options[] = {
    {"variable",   SAL_FIELD_TYPE_INT32,  "Module ID",    NULL},
    {NULL}
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_reachability_man = {
    "Displays reachable links to modid",
    "Diagnostic to display all reachable links to a given module",
    "fabric reachability <modid>",
    "fabric reachability 3 \n"
    "f reach 2"
};
/* *INDENT-ON* */

/**
 * \brief - display reachable links to modid
 */
static shr_error_e
cmd_dnxc_fabric_reachability(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int modid;
    uint32 links_array[SOC_DNXC_MAX_NOF_FABRIC_LINKS];
    int array_size;
    int ii, row_index, cell_index, nof_clmns = 0;
    int minimum_links = -1;
    int isolate = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("variable", modid);

    /*
     * Retrieve reachability inforamtion for current device
     */
    SHR_IF_ERR_EXIT(bcm_fabric_control_get(unit, bcmFabricIsolate, &isolate));
    SHR_IF_ERR_EXIT(bcm_fabric_reachability_status_get
                    (unit, modid, SOC_DNXC_MAX_NOF_FABRIC_LINKS, links_array, &array_size));

    if (array_size == 0)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Module %d isn't reachable \n", modid);
    }

    if (SOC_IS_JERICHO(unit))   
    {
        SHR_IF_ERR_EXIT(bcm_fabric_destination_link_min_get(unit, 0, modid, &minimum_links));
    }

    PRT_TITLE_SET("Reachability table");

    /*
     * Displays reachability status information per module
     */
    if (isolate)
    {
        PRT_INFO_ADD("This device is isolated");
    }

    PRT_INFO_ADD("Found %d links to module %d:", array_size, modid);

    /*
     * The default behaviour of the diagnostic is to print out the links in rows each containing 24 links * If we don't 
     * have at least 24(MAX_NOF_REACHABILITY_COLS) links we reduce the number of the columns 
     */
    nof_clmns = UTILEX_MIN(array_size, DIAG_DNXC_MAX_NOF_REACHABILITY_CLMNS);

    /*
     * Create the columns
     */
    for (ii = 0; ii < nof_clmns; ii++)
    {
        PRT_COLUMN_ADD("#");
    }

    /*
     * Display all links on which the module is reachable
     */
    /*
     * Iterate over all needed rows, each of them with 24 links or less 
     */
    for (row_index = 0; row_index < array_size; row_index += nof_clmns)
    {

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        /*
         * Iterate over each of the cells in the row and print the link 
         */
        for (cell_index = row_index; cell_index < row_index + nof_clmns; cell_index++)
        {
            

            if (cell_index < array_size)
            {
                PRT_CELL_SET("%d", links_array[cell_index]);
            }
            else
            {
                /*
                 * Once the links that are about to be displayed become less than 24(MAX_NOF_REACHABILITY_COLS) * the
                 * rest of the row is filled with empty cells 
                 */
                PRT_CELL_SKIP(1);
            }
        }
    }

    if (minimum_links != -1)    
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("The minimum number of links to destination is %d", minimum_links);
        if (minimum_links > array_size && minimum_links)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Current number of links is smaller than minimal number of links");
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
/*
 * Connectivity
 */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_connectivity_man = {
    "Display fabric interface connectivity information",
    "Diagnostic displaying all connectivity information for current module",
    "fabric connectivity \n",
    "f con"
};
/* *INDENT-ON* */

/**
 * \brief - display fabric interface connectivity information
 */
static shr_error_e
cmd_dnxc_fabric_connectivity(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_fabric_link_connectivity_t links_connectivity_array[SOC_DNXC_MAX_NOF_FABRIC_LINKS];
    int array_size, object_col_id = 0;
    int link, links_count = 0;
    bcm_port_config_t config;
    bcm_port_t port;
    char *dev_type;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get an array with all ports associated to the passed number of links 
     */
    SHR_IF_ERR_EXIT(bcm_fabric_link_connectivity_status_get
                    (unit, SOC_DNXC_MAX_NOF_FABRIC_LINKS, links_connectivity_array, &array_size));

    if (array_size == 0)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "No links were found for this device \n");
    }

    PRT_TITLE_SET("Connectivity status");
    /*
     * Displays device connectivity information
     */
    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Link");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Logical Port");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Remote Module");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Remote Link");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Remote Device Type");

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.sfi, port)
    {
        /*
         * Get the device type of the connected module on the associated link
         */
        if (BCM_FABRIC_LINK_NO_CONNECTIVITY != links_connectivity_array[links_count].link_id)
        {

            switch (links_connectivity_array[links_count].device_type)
            {
                case bcmFabricDeviceTypeFE1:
                case bcmFabricDeviceTypeFE3:
                case bcmFabricDeviceTypeFE13:
                    dev_type = "FE13";
                    break;
                case bcmFabricDeviceTypeFE2:
                    dev_type = "FE2";
                    break;
                case bcmFabricDeviceTypeFAP:
                case bcmFabricDeviceTypeFIP:
                case bcmFabricDeviceTypeFOP:
                    dev_type = "FAP";
                    break;
                case bcmFabricDeviceTypeUnknown:
                default:
                    dev_type = "Unknown";
                    break;
            }

            
            {
                /*
                 * For FE devices link is always equal to port
                 */
                link = port;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("%d", link);
            PRT_CELL_SET("%d", port);
            PRT_CELL_SET("%d", links_connectivity_array[links_count].module_id);
            PRT_CELL_SET("%d", links_connectivity_array[links_count].link_id);
            PRT_CELL_SET("%s", dev_type);
        }

        links_count++;
    }

    if (array_size != links_count)
    {
        SHR_CLI_EXIT(_SHR_E_CONFIG,
                     "Number of enabled links doesn't match returned value from the 'bcm_fabric_link_connectivity_status_get' API \n");
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Queues diagnostic
 */

/* Queues filters per block*/
static sh_sand_enum_t dnxc_fabric_queues_blocks_enum_table[] = {
    {"DCH", diag_dch_stage_option},
    {"DTM", diag_dtm_stage_option},
    {"DTL", diag_dtl_stage_option},
    {"DFL", diag_dfl_stage_option},
    {"ALL", DIAG_DNXC_OPTION_ALL},
    {NULL}
};

/* *INDENT-OFF* */
static sh_sand_option_t dnxc_fabric_queues_options[] = {
    {"variable",   SAL_FIELD_TYPE_ENUM,    "FE Stage (DCH/DTM/DTL/DFL)",  "ALL", (void *) dnxc_fabric_queues_blocks_enum_table},
    {NULL}
};

static sh_sand_man_t dnxc_fabric_queues_man = {
    "display queues status",
    "Show maximum queues occupancy per block",
    "fabric queues <DCH/DTM/DTL/DFL>\n",
    "f q dch - Show most ocupied link for each DCH link group and the maximum number of cells in its FIFO \n"
    "f q dtm - Shows the maximum cells in each DCH FIFO of each DTM block \n"
    "f q dtl - Show most ocupied link of each DTL and the maximum number of cells in its FIFO \n"
    "f q dfl - Shows the most ocupied DFL Bank and its most occupied Sub-Bank and the maximum number of cells in this Sub-Bank"
};

/* *INDENT-ON* */

/**
 * \brief - display queues status
 */
static shr_error_e
cmd_dnxc_fabric_queues(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#ifdef BCM_DNXF_SUPPORT
    int queue_stage_selected;
#endif
    SHR_FUNC_INIT_VARS(unit);
#ifdef SOC_IS_JERICHO_2
    if (SOC_IS_DNX(unit))
    {
        
        SHR_ERR_EXIT(_SHR_E_UNIT, "This diagnostic is supported only for DNXF devices\n");
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        /*
         * Get filter input if used
         */
        SH_SAND_GET_ENUM("variable", queue_stage_selected);

        /*
         * DNXF device printout
         */
        SHR_IF_ERR_EXIT(diag_dnxf_queues_diag_print(unit, queue_stage_selected, sand_control));
    }
exit:
#endif
    SHR_FUNC_EXIT;
}

/*
 * Property
 */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_property_man = {
    "Please use 'data dump property *' instead",
    "Please use 'data dump property *' instead",
    "Please use 'data dump property *' instead",
    "\n"
};
/* *INDENT-ON* */

/**
 * \brief - display current used fabric property values
 */
static shr_error_e
cmd_dnxc_fabric_property(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        cli_out("%s \n", dnxc_fabric_property_man.brief);
    }
#endif /* BCM_DNXF_SUPPORT */
    SHR_FUNC_EXIT;
}

/*
 * Traffic Profile
 */

/* *INDENT-OFF* */
static sh_sand_option_t dnxc_fabric_traffic_options[] = {
    {"destination",      SAL_FIELD_TYPE_INT32,  "Destination ID",  "-1"},
    {"source",           SAL_FIELD_TYPE_INT32,  "Source ID",       "-1"},
    {NULL}
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_traffic_man = {
    "Display traffic cast and priorities sent through the fabric",
    "Please fill here full description for fabric traffic",
    "fabric traffic_profile [destination=<destnation_id>] [source=<source_id>]",
    "fabric traffic_profile \n"
    "fabric traffic_profile destination=40 \n"
    "fabric traffic_profile dst=40 src=50"
};
/* *INDENT-ON* */

/**
 * \brief - display if there are cells with given cast 
 *        and priority sent through the fabric
 */

static shr_error_e
cmd_dnxc_fabric_traffic(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#ifdef BCM_DNXF_SUPPORT
    soc_dnxf_diag_fabric_traffic_profile_t traffic_profile;
    int destination, source, priority;
    char dest_id_buf[10], source_id_buf[10];
    PRT_INIT_VARS;
#endif
    SHR_FUNC_INIT_VARS(unit);
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        

        SHR_ERR_EXIT(_SHR_E_UNIT, "This diagnostic is supported only for DNXF devices\n");
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        SH_SAND_GET_INT32("destination", destination);
        SH_SAND_GET_INT32("source", source);

        /*
         * Check the validity of the inputs if they are passed
         */
        if (source != DIAG_DNXC_OPTION_ALL)
        {
            if (source < 0 || source > DNXF_MAX_MODULES)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid source parameter specified! MODID should be between 0 and %d \n",
                             DNXF_MAX_MODULES);
            }
            sal_snprintf(source_id_buf, 10, "%d", source);
        }

        if (destination != DIAG_DNXC_OPTION_ALL)
        {
            if (destination < 0 || destination > DNXF_MAX_MODULES)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM,
                             "Invalid destination parameter specified! MODID should be between 0 and %d \n",
                             DNXF_MAX_MODULES);
            }
            sal_snprintf(dest_id_buf, 10, "%d", destination);
        }

        /*
         * Get traffic info
         */
        SHR_IF_ERR_EXIT(soc_dnxf_diag_fabric_traffic_profile_get(unit, source, destination, &traffic_profile));

        /*
         * Print traffic info
         */
        PRT_TITLE_SET("Traffic profile information");

        PRT_INFO_ADD("Checked for cells with source_id = %s and dest_id = %s",
                     source != -1 ? source_id_buf : "any", destination != -1 ? dest_id_buf : "any");

        PRT_COLUMN_ADD("Priority");
        PRT_COLUMN_ADD("UC");
        PRT_COLUMN_ADD("MC");
        for (priority = 0; priority < dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit); priority++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("%d", priority);
            /*
             * Unicast printout
             */
            PRT_CELL_SET("%s", traffic_profile.unicast[priority] ? "Yes" : "No");
            /*
             * Multicast printout
             */
            PRT_CELL_SET("%s", traffic_profile.multicast[priority] ? "Yes" : "No");
        }

        PRT_COMMITX;
    }
    PRT_FREE;
#endif
exit:
    SHR_FUNC_EXIT;
}

/*
 * Thresholds diagnostic
 */

/*
 * LOCAL Thresholds STRUCTUREs:
 * {
 */

static sh_sand_enum_t dnxc_fabric_thresholds_group_enum_table[] = {
    {"GCI", diag_stage_gci_option},
    {"RCI", diag_stage_rci_option},
    {"FC", diag_stage_fc_option},
    {"DROP", diag_stage_drop_option},
    {"SIZE", diag_stage_size_option},
    {NULL}
};

/* Link the values in the two enums*/
static sh_sand_enum_t dnxc_fabric_stage_enum_table[] = {
    {"TX", diag_stage_tx_option},
    {"MIDDLE", diag_stage_middle_option},
    {"RX", diag_stage_rx_option},
    {"SHARED", diag_stage_shared_option},
    {"ALL", DIAG_DNXC_OPTION_ALL},
    {NULL}
};

/*
 * LOCAL Threshold STRUCTUREs:
 * }
 */

/* *INDENT-OFF* */
static sh_sand_option_t dnxc_fabric_thresholds_options[] = {
    {"variable",   SAL_FIELD_TYPE_ENUM,    "Threshold Group (GCI/RCI/FC/DROP/SIZE)",  NULL, (void *) dnxc_fabric_thresholds_group_enum_table},	
    {"pipe",       SAL_FIELD_TYPE_INT32,   "Pipe ID (0/1/2)",                       "-1"},
    {"priority",   SAL_FIELD_TYPE_INT32,   "Priority (0/1/2/3)",                    "-1"},
    {"stage",      SAL_FIELD_TYPE_ENUM,    "TX/MIDDLE/RX/SHARED",                   "ALL",   (void *) dnxc_fabric_stage_enum_table},
    {"link",       SAL_FIELD_TYPE_INT32,   "link ID",                               "-1"},
    {NULL}
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_thresholds_man = {
    "Display the configured FIFO thresholds",
    "Displays all FIFO threshold information per threshold type",
    "fabric thresholds <RCI/GCI/FC/DROP/SIZE>"
    "[stage=TX/MIDDLE/RX/SHARED/ALL] [pipe=<0/1/2>] [priority=<0/1/2/3>] [link=<link_id>]",
    "fabric thresholds RCI"
    "f ths rci priority=2 link=46 \n"
    "f ths drop stage=shared pipe=1"
};
/* *INDENT-ON* */

/**
 * \brief - display the configured FIFO thresholds
 */
static shr_error_e
cmd_dnxc_fabric_thresholds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
#ifdef BCM_DNXF_SUPPORT
    int th_group_selected;
    int pipe_selected, priority_selected, link_selected;
    int th_stage_selected;

    PRT_INIT_VARS;
#endif
    SHR_FUNC_INIT_VARS(unit);
#ifdef SOC_IS_JERICHO_2
    
    if (SOC_IS_DNX(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNIT, "This diagnostic is supported only for DNXF devices\n");
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        /*
         * Get the command inputs
         */
        SH_SAND_GET_ENUM("variable", th_group_selected);
        SH_SAND_GET_ENUM("stage", th_stage_selected);
        SH_SAND_GET_INT32("pipe", pipe_selected);
        SH_SAND_GET_INT32("priority", priority_selected);
        SH_SAND_GET_INT32("link", link_selected);

        /*
         * Print in format for DNXF devices
         */
        SHR_IF_ERR_EXIT(diag_dnxf_thresholds_diag_print
                        (unit, th_group_selected, th_stage_selected, pipe_selected, priority_selected, link_selected,
                         sand_control));
    }
    PRT_FREE;
exit:
#endif
    SHR_FUNC_EXIT;
}

/*
 * Link Status/Config
 */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_link_status_man = {
    "Display fabric link status",
    "Displays the fabric link status for all links or for a specific link",
    "fabric link status [<link_id>] [all]",
    "fabric link status all \n"
    "f link sts all \n"
    "f link sts 45"
};
/* *INDENT-ON* */

/**
 * \brief - display fabric link status
 */
static shr_error_e
cmd_dnxc_fabric_link_status(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int link_idx = 0;
    bcm_port_t link_id;
    int show_all;
    uint32 link_status;
    uint32 errored_token_count;
    int object_col_id = 0;
    uint32 links_status_arr[SOC_DNXC_MAX_NOF_FABRIC_LINKS],
        links_errored_token_count_arr[SOC_DNXC_MAX_NOF_FABRIC_LINKS];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the two command inputs
     */
    SH_SAND_GET_INT32("variable", link_id);
    SH_SAND_GET_BOOL("all", show_all);

    /*
     * If no parameter is passed display all links
     */
    if (!show_all && link_id == DIAG_DNXC_OPTION_ALL)
    {
        show_all = 1;
    }

    if (show_all && link_id != DIAG_DNXC_OPTION_ALL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM,
                     "Passing 'all' together with link_id as parameters is now allowed! Please see 'fabric link status usage' \n");
    }

    PRT_TITLE_SET("Links status");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Link");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "CRC Error");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Size Error");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Code Group Error");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Misalign");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "No Signal Lock");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "No signal accept");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Errored tokens");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Errored tokens count");

    if (show_all)
    {
        /*
         * Show link status information for all links
         */

        if (SOC_IS_DNXF(unit))
        {
            /*
             * Get link status for all FE links
             */
            for (link_id = 0; link_id < SOC_DNXC_MAX_NOF_FABRIC_LINKS; link_id++)
            {
                SHR_IF_ERR_EXIT(bcm_fabric_link_status_get
                                (unit, link_id, &links_status_arr[link_id], &links_errored_token_count_arr[link_id]));
            }
        }
        else
        {


            SHR_ERR_EXIT(BCM_E_UNAVAIL, "Feature not yet supported for FAP devices\n");
        }

        for (link_idx = 0; link_idx < SOC_DNXC_MAX_NOF_FABRIC_LINKS; link_idx++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("%d", link_idx);       /* Link number */

            PRT_CELL_SET("%s", links_status_arr[link_idx] & BCM_FABRIC_LINK_STATUS_CRC_ERROR ? "***" : " - ");
            PRT_CELL_SET("%s", links_status_arr[link_idx] & BCM_FABRIC_LINK_STATUS_SIZE_ERROR ? "***" : " - ");
            PRT_CELL_SET("%s", links_status_arr[link_idx] & BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR ? "***" : " - ");
            PRT_CELL_SET("%s", links_status_arr[link_idx] & BCM_FABRIC_LINK_STATUS_MISALIGN ? "***" : " - ");
            PRT_CELL_SET("%s", links_status_arr[link_idx] & BCM_FABRIC_LINK_STATUS_NO_SIG_LOCK ? "***" : " - ");
            PRT_CELL_SET("%s", links_status_arr[link_idx] & BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP ? "***" : " - ");
            PRT_CELL_SET("%s", links_status_arr[link_idx] & BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS ? "***" : " - ");
            PRT_CELL_SET("%d", links_errored_token_count_arr[link_idx]);
        }

    }
    else
    {
        /*
         * Show diagnostic only for specified link
         */
        SHR_IF_ERR_EXIT(bcm_fabric_link_status_get(unit, link_id, &link_status, &errored_token_count));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%d", link_id);

        PRT_CELL_SET("%s", link_status & BCM_FABRIC_LINK_STATUS_CRC_ERROR ? "***" : " - ");
        PRT_CELL_SET("%s", link_status & BCM_FABRIC_LINK_STATUS_SIZE_ERROR ? "***" : " - ");
        PRT_CELL_SET("%s", link_status & BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR ? "***" : " - ");
        PRT_CELL_SET("%s", link_status & BCM_FABRIC_LINK_STATUS_MISALIGN ? "***" : " - ");
        PRT_CELL_SET("%s", link_status & BCM_FABRIC_LINK_STATUS_NO_SIG_LOCK ? "***" : " - ");
        PRT_CELL_SET("%s", link_status & BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP ? "***" : " - ");
        PRT_CELL_SET("%s", link_status & BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS ? "***" : " - ");
        PRT_CELL_SET("%d", errored_token_count);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - display fabric link configuration
 */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_link_config_man = {
    "Display fabric link config",
    "Displays the most important NIF configuration information for all links or for a specific link",
    "fabric link config [all] [<link_id>]",
    "fabric link config all \n"
    "f link cfg all \n"
    "f link cfg 45"
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
static sh_sand_option_t dnxc_fabric_link_config_options[] = {
    {"variable",         SAL_FIELD_TYPE_INT32,    "link ID",         "-1"},
    {"all",              SAL_FIELD_TYPE_BOOL,     "show all links",  "no"},
    {NULL}
};
/* *INDENT-ON* */

typedef struct diag_dnxc_fabric_link_config_s
{
    int enable;
    int speed;
    bcm_port_phy_fec_t fec_type;
    int fec_llfc_extract_cig;
    int fec_error_detect;
    int fec_llfc_after_fec;
    int fec_llfc_low_latency;
    uint32 cl72;
    int pcp;
    bcm_fabric_link_remote_pipe_mapping_t pipe_mapping;
    uint32 rx_polarity;
    uint32 tx_polarity;
    uint32 lane_swap;
    int clk_freq;
} diag_dnxc_fabric_link_config_t;

static shr_error_e
cmd_dnxc_fabric_link_config(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    diag_dnxc_fabric_link_config_t link_config;
    bcm_port_config_t port_config;
    bcm_pbmp_t diag_info_bmp;
    char fec_str[80], pipe_mapping_buf[20];
    char *fec_type, *pipe_mapping_str;
    bcm_port_t link;
    int show_all, object_col_id = 0;
    bcm_port_lane_to_serdes_map_t serdes_map[SOC_DNXC_MAX_NOF_FABRIC_LINKS];
    bcm_fabric_pipe_t remote_pipe_map[SOC_DNXC_MAX_NOF_PIPES];
    bcm_port_resource_t resource;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the two command inputs
     */
    SH_SAND_GET_INT32("variable", link);
    SH_SAND_GET_BOOL("all", show_all);

    /*
     * If no parameter is passed display all links
     */
    if (!show_all && link == DIAG_DNXC_OPTION_ALL)
    {
        show_all = 1;
    }

    if (show_all && link != DIAG_DNXC_OPTION_ALL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM,
                     "Passing 'all' together with link_id as parameters is now allowed! Please see 'fabric link config usage' \n");
    }

    BCM_PBMP_CLEAR(diag_info_bmp);
    if (show_all)
    {
        SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
        diag_info_bmp = port_config.sfi;
    }
    else
    {
        BCM_PBMP_PORT_SET(diag_info_bmp, link);
    }

    PRT_TITLE_SET("Link Config");

    PRT_COLUMN_ADDX(PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Link");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Enable");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Speed");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Ref clock");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "CL72");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "FEC");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "PCP");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Pipe mapping");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Polarity");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "SerDes");

    /*
     * Get Serdes Mapping
     */
    SHR_IF_ERR_EXIT(bcm_port_lane_to_serdes_map_get
                    (unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, SOC_DNXC_MAX_NOF_FABRIC_LINKS, serdes_map));

    BCM_PBMP_ITER(diag_info_bmp, link)
    {

        /*
         * Get link enable status
         */
        SHR_IF_ERR_EXIT(bcm_port_enable_get(unit, link, &link_config.enable));

        /*
         * Get link resource 
         */
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, link, &resource));

        /*
         * Get link speed
         */
        link_config.speed = resource.speed;

        /*
         * Get Link CL72 status
         */
        link_config.cl72 = resource.link_training;

        /*
         * Get link FEC status
         */
        link_config.fec_type = resource.fec_type;

        link_config.fec_llfc_extract_cig = 0;
        link_config.fec_error_detect = 0;
        link_config.fec_llfc_low_latency = 0;
        link_config.fec_llfc_after_fec = 0;

        if (link_config.fec_type == bcmPortPhyFecBaseR || link_config.fec_type == bcmPortPhyFecRs206
            || link_config.fec_type == bcmPortPhyFecRs108 || link_config.fec_type == bcmPortPhyFecRs545
            || link_config.fec_type == bcmPortPhyFecRs304)
        {
            SHR_IF_ERR_EXIT(bcm_port_control_get
                            (unit, link, bcmPortControlFecErrorDetectEnable, &link_config.fec_error_detect));
        }

        if (link_config.fec_type == bcmPortPhyFecBaseR)
        {
            SHR_IF_ERR_EXIT(bcm_port_control_get
                            (unit, link, bcmPortControlLlfcCellsCongestionIndEnable,
                             &link_config.fec_llfc_extract_cig));
            SHR_IF_ERR_EXIT(bcm_port_control_get
                            (unit, link, bcmPortControlLowLatencyLLFCEnable, &link_config.fec_llfc_low_latency));
        }

        if (link_config.fec_type == bcmPortPhyFecRs206 || link_config.fec_type == bcmPortPhyFecRs108
            || link_config.fec_type == bcmPortPhyFecRs545 || link_config.fec_type == bcmPortPhyFecRs304)
        {
            SHR_IF_ERR_EXIT(bcm_port_control_get
                            (unit, link, bcmPortControlLLFCAfterFecEnable, &link_config.fec_llfc_after_fec));
        }

        /*
         * Get Link TX Polarity status
         */
        SHR_IF_ERR_EXIT(bcm_port_phy_control_get
                        (unit, link, BCM_PORT_PHY_CONTROL_TX_POLARITY, &link_config.tx_polarity));

        /*
         * Get Link RX Polarity status
         */
        SHR_IF_ERR_EXIT(bcm_port_phy_control_get
                        (unit, link, BCM_PORT_PHY_CONTROL_RX_POLARITY, &link_config.rx_polarity));

        /*
         * Get link pipe mapping
         */
        bcm_fabric_link_remote_pipe_mapping_t_init(&link_config.pipe_mapping);
        link_config.pipe_mapping.remote_pipe_mapping = remote_pipe_map;
        link_config.pipe_mapping.remote_pipe_mapping_max_size = SOC_DNXC_MAX_NOF_PIPES;
        link_config.pcp = 0;
        if (SOC_IS_DNXF(unit))
        {
            SHR_IF_ERR_EXIT(bcm_fabric_link_remote_pipe_mapping_get(unit, link, &link_config.pipe_mapping));

            SHR_IF_ERR_EXIT(bcm_fabric_link_control_get(unit, link, bcmFabricLinkPcpEnable, &link_config.pcp));

        }

        /*
         * Get link's ref clk value
         */
        link_config.clk_freq = SOC_INFO(unit).port_refclk_int[link];

        /*
         * Convert link FEC type to string
         */
        /*
         * { FEC info string create
         */
        switch (link_config.fec_type)
        {
            case bcmPortPhyFecNone:
                fec_type = "None";
                break;
            case bcmPortPhyFecBaseR:
                fec_type = "BaseR";
                break;
            case bcmPortPhyFecRs206:
                fec_type = "Rs206";
                break;
            case bcmPortPhyFecRs108:
                fec_type = "Rs108";
                break;
            case bcmPortPhyFecRs545:
                fec_type = "Rs545";
                break;
            case bcmPortPhyFecRs304:
                fec_type = "Rs304";
                break;
            default:
                fec_type = "unknown";
                break;
        }
        sal_sprintf(fec_str, "%s%s%s%s%s",
                    fec_type,
                    link_config.fec_llfc_extract_cig ? " (+LlfcCig)" : "",
                    link_config.fec_error_detect ? " (+ErrInd)" : "",
                    link_config.fec_llfc_low_latency ? " (+LlfcLat)" : "",
                    link_config.fec_llfc_after_fec ? " (+LlfcAfterFec)" : "");

        /*
         * } FEC info string create
         */

        /*
         * Format pipe mapping strings
         */
        if (link_config.pipe_mapping.num_of_remote_pipes == 0)
        {
            /*
             * no mapping
             */
            pipe_mapping_str = "None";
        }
        else if (link_config.pipe_mapping.num_of_remote_pipes == 1)
        {
            sal_sprintf(pipe_mapping_buf, "   0->%d   ", link_config.pipe_mapping.remote_pipe_mapping[0]);
            pipe_mapping_str = pipe_mapping_buf;
        }
        else if (link_config.pipe_mapping.num_of_remote_pipes == 2)
        {
            sal_sprintf(pipe_mapping_buf, "0->%d, 1->%d", link_config.pipe_mapping.remote_pipe_mapping[0],
                        link_config.pipe_mapping.remote_pipe_mapping[1]);
            pipe_mapping_str = pipe_mapping_buf;
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "ERROR: convert pipe mapping to string");
        }

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("%d", link);
        PRT_CELL_SET("%d", link_config.enable);
        PRT_CELL_SET("%d", link_config.speed);
        PRT_CELL_SET("%d ", link_config.clk_freq);
        PRT_CELL_SET("%d", link_config.cl72 ? 1 : 0);
        PRT_CELL_SET("%s", fec_str);
        PRT_CELL_SET("%d", link_config.pcp);
        PRT_CELL_SET("%s", pipe_mapping_str);
        PRT_CELL_SET("rx:%d, tx:%d", link_config.rx_polarity, link_config.tx_polarity);
        PRT_CELL_SET("rx:%d, tx:%d", serdes_map[link].serdes_rx_id, serdes_map[link].serdes_tx_id);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
static sh_sand_option_t dnxc_fabric_link_status_options[] = {
    {"variable",         SAL_FIELD_TYPE_INT32,     "link ID",         "-1"},
    {"all",              SAL_FIELD_TYPE_BOOL,      "show all links",  "no"},
    {NULL}
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_link_man = {
    "display fabric link status and configuration",
    "",
    "",
    "\n"
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnxc_fabric_link_cmds[] = {
    /*keyword,          action,                            command, options,                                                      man                               */
    {"status",          cmd_dnxc_fabric_link_status,       NULL,                        dnxc_fabric_link_status_options,          &dnxc_fabric_link_status_man},
    {"config",          cmd_dnxc_fabric_link_config,       NULL,                        dnxc_fabric_link_config_options,          &dnxc_fabric_link_config_man},
    {NULL}
};

/*
 * Mesh Topology
 */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_mesh_topology_man = {
    "Diagnostic for mesh_topology block",
    "Diagnostic for mesh_topology block",
    "fabric mesh_topology",
    "f mesh_topology"
};
/* *INDENT-ON* */

/**
 * \brief - display fabric mesh_topology configuration
 */
static shr_error_e
cmd_dnxc_fabric_mesh_topology(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int link_idx, col_idx;
    soc_dnxc_fabric_mesh_topology_diag_t mesh_topology_diag;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get traffic info
     */
    SHR_IF_ERR_EXIT(soc_dnxc_fabric_mesh_topology_diag_get(unit, &mesh_topology_diag));

    /*
     * Print mesh topology diag topic
     */
    LOG_CLI(("MESH TOPOLOGY DIAG \n"));

    /*
     * Print mesh topology indications table { 
     */
    PRT_TITLE_SET("Indications:");
    PRT_COLUMN_ADD("Indication");
    PRT_COLUMN_ADD("Value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("1");
    PRT_CELL_SET("%u", mesh_topology_diag.indication_1);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("2");
    PRT_CELL_SET("%u", mesh_topology_diag.indication_2);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("3");
    PRT_CELL_SET("%u", mesh_topology_diag.indication_3);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("4");
    PRT_CELL_SET("%u", mesh_topology_diag.indication_4);

    PRT_COMMITX;

    /*
     * Print mesh topology indications table * } 
     */

    /*
     * Print mesh topology counters table { 
     */
    PRT_TITLE_SET("Counters:");
    PRT_COLUMN_ADD("Control cell type");
    PRT_COLUMN_ADD("Count");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("rx_control_cells_type1");
    PRT_CELL_SET("%u", mesh_topology_diag.rx_control_cells_type1);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("rx_control_cells_type2");
    PRT_CELL_SET("%u", mesh_topology_diag.rx_control_cells_type2);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("control_cells_type3");
    PRT_CELL_SET("%u", mesh_topology_diag.control_cells_type3);

    PRT_COMMITX;
    /*
     * Print mesh topology counters table } 
     */

    /*
     * Print mesh topology link status table { 
     */
    PRT_TITLE_SET("type2_list:");

    for (col_idx = 0; col_idx < DIAG_DNXC_MAX_NOF_MESH_TOPOLOGY_CLMNS; col_idx++)
    {
        PRT_COLUMN_ADD("#%d", col_idx + 1);
        PRT_COLUMN_ADD("Values%d", col_idx + 1);
        /*
         * Empty column to separete each link with its status
         */
        PRT_COLUMN_ADD("");
    }

    for (link_idx = 0; link_idx < mesh_topology_diag.link_list_count; link_idx++)
    {

        if ((link_idx % DIAG_DNXC_MAX_NOF_MESH_TOPOLOGY_CLMNS) == 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }

        PRT_CELL_SET("%d", link_idx);
        if (mesh_topology_diag.type2_list[link_idx] != -1)
        {
            PRT_CELL_SET("0x%x", mesh_topology_diag.type2_list[link_idx]);
        }
        else
        {
            PRT_CELL_SET("N/A");
        }
        /*
         * Empty column to separete each link with its status
         */
        PRT_CELL_SET_SHIFT(0, "");
    }

    PRT_COMMITX;
    /*
     * Print mesh topology link status table } 
     */

    /*
     * Print mesh topology Status table 1 { 
     */
    LOG_CLI(("status 1:\n"));
    LOG_CLI(("status_1 %u \n", mesh_topology_diag.status_1));
    LOG_CLI(("status_1_id1 %u \n", mesh_topology_diag.status_1_id1));
    LOG_CLI(("status_1_id2 %u \n", mesh_topology_diag.status_1_id2));
    /*
     * Print mesh topology Status table 1 } 
     */

    /*
     * Print mesh topology Status table 2 { 
     */
    LOG_CLI(("status 2:\n"));
    LOG_CLI(("status_2 %u \n", mesh_topology_diag.status_2));
    LOG_CLI(("status_2_id1 %u \n", mesh_topology_diag.status_2_id1));
    LOG_CLI(("status_2_id2 %u \n", mesh_topology_diag.status_2_id2));
    /*
     * Print mesh topology Status table 2 } 
     */
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnxc_fabric_cmds[] = {
    /*keyword,          action,                            command,                              options,                                  man                               */
    {"reachability",    cmd_dnxc_fabric_reachability,      NULL,                                 dnxc_fabric_reachability_options,         &dnxc_fabric_reachability_man},
    {"connectivity",    cmd_dnxc_fabric_connectivity,      NULL,                                 NULL,                                     &dnxc_fabric_connectivity_man},
    {"queues",          cmd_dnxc_fabric_queues,            NULL,                                 dnxc_fabric_queues_options,               &dnxc_fabric_queues_man},
    {"property",        cmd_dnxc_fabric_property,          NULL,                                 NULL,                                     &dnxc_fabric_property_man},
    {"traffic_profile", cmd_dnxc_fabric_traffic,           NULL,                                 dnxc_fabric_traffic_options,              &dnxc_fabric_traffic_man},
    {"thresholds",      cmd_dnxc_fabric_thresholds,        NULL,                                 dnxc_fabric_thresholds_options,           &dnxc_fabric_thresholds_man},
    {"link",            NULL,                              sh_dnxc_fabric_link_cmds,             NULL,                                     &dnxc_fabric_link_man},
    {"mesh_topology",   cmd_dnxc_fabric_mesh_topology,     NULL,                                 NULL,                                     &dnxc_fabric_mesh_topology_man},
    {NULL}
};

sh_sand_man_t sh_dnxc_fabric_man = {
    "Misc facilities for displaying dnxc fabric information",
    NULL,
    NULL,
    NULL,
};

const char cmd_dnxc_fabric_usage[] = "Display dnxc fabric information";
/* *INDENT-ON* */

/*
 * }
 */
