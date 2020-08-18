/** \file diag_dnx_fc.c
 * 
 * DNX TM Flow Control diagnostics 
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COSQ

/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fc.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>

#include "diag_dnx_fc.h"

/* 
 * --------------------------------------------------------------------------
 * dnx tm flow control
 * --------------------------------------------------------------------------
 */

/*
 * Define the short strings for each FC Type.
 */
/* *INDENT-OFF* }*/
static sh_sand_enum_t sand_diag_fc_type_enum[] = {
    /**String         Value*/
    {"GLB_RSC",       DNX_FC_TYPE_GLB_RSC,         "Generation: Present signals from Global Resources' thresholds to CFC indicating to generate FC.In this case index is not used. Instead the parameter 'resource' is used to represent the type of Global Resource to be presented."},
    {"AF_NIF",        DNX_FC_TYPE_AF_NIF,          "Generation: Present signals from  Almost full NIF FIFO's thresholds to CFC indicating to generate FC. In this case index is used to represent the NIF port, possible indexes 0-(Number of NIF ports)"},
    {"VSQ_ABCD",      DNX_FC_TYPE_VSQ_ABCD,        "Generation: Present signals from VSQ ABCD's thresholds to CFC indicating to generate FC. In this case index is used to represent the VSQ's index, possible indexes 0-(Number of VSQs A + Number of VSQs B + Number of VSQs C + Number of VSQs D)"},
    {"VSQ_LLFC",      DNX_FC_TYPE_VSQ_LLFC,        "Generation: Present signals from VSQ LLFC(VSQ E)'s thresholds to CFC indicating to generate FC. In this case index is used to represent the VSQ's index, possible indexes 0-(Number of VSQs E)"},
    {"VSQ_PFC",       DNX_FC_TYPE_VSQ_PFC,         "Generation: Present signals from VSQ PFC's thresholds to CFC indicating to generate FC. In this case index is used to represent the VSQ's index, possible indexes 0-(Number of VSQs F)"},
    {"PFC_EGQ",       DNX_FC_TYPE_FC_EGQ,          "Reception: Present PFC indications from CFC received by the EGQ. In this case index is used to represent the Q-pair of the EGQ interface, possible indexes 0-(Number of EGQ Q-pairs)"},
    {"NIF_PFC",       DNX_FC_TYPE_NIF_PFC,         "Reception: Present PFC indications from NIF received by the CFC. In this case index is used to represent the priority of the NIF port, possible indexes 0-383"},
    {"OOB_SINGLE",    DNX_FC_TYPE_OOB_SINGLE,      "Reception: Present Q-pair based FC (to stop a single Q-pair) indications from SPI OOB calendar received by the EGQ. In this case index is used to represent the Q-pair of the EGQ interface, possible indexes 0-(Number of EGQ Q-pairs)"},
    {"OOB_PORT_BASED",DNX_FC_TYPE_OOB_PORT_BASED,  "Reception: Present Q-pair based FC (to stop all Q-pairs of a port) indications from SPI OOB calendar received by the EGQ. In this case index is used to represent the Q-pair of the EGQ interface, possible indexes 0-(Number of EGQ Q-pairs)"},
    {"OOB_LLFC",      DNX_FC_TYPE_OOB_LLFC,        "Reception: Present Link level FC (to stop a NIF port)  indications from SPI OOB calendar received by the NIF. In this case index is used to represent the NIF port, possible indexes 0-(Number of NIF ports + CMIC ports)"},
    {"GEN_BMP_OOB",   DNX_FC_TYPE_OOB_GEN_BMP,     "Reception: Present which of the Generic PFC bitmaps are used by the SPI OOB calendar to send FC to the EGQ. In this case index is used to represent the Generic Bitmap index, possible indexes 0-(Number of Generic Bitmaps)"},
    {"ILKN_OOB",      DNX_FC_TYPE_ILKN_OOB,        "Reception: Present FC received by ILKN OOB calendar. In this case index is used to represent the calendar entry, possible indexes 0-(OOB calendar length)"},
    {"PP_COE",        DNX_FC_TYPE_PP_COE,          "Reception: Present FC received by the PP COE calendar. In this case index is used to represent the calendar entry, possible indexes indexes 0-(COE calendar length)"},
    {"ALL",           DNX_FC_TYPE_COUNT,           "All types"},
    {NULL}
};

static sh_sand_enum_t sand_diag_fc_nif_type_enum[] = {
    /**String           Value*/
    {"LLFC_TO_PM",      DNX_FC_NIF_TYPE_LLFC_TO_PM,         "LLFC generation: Present indications that LLFC is generated to Port Macro"},
    {"LLFC_FROM_CFC",   DNX_FC_NIF_TYPE_LLFC_FROM_CFC,      "LLFC generation: Present signals from CFC indicating to generate LLFC"},
    {"PFC_TO_PM",       DNX_FC_NIF_TYPE_PFC_TO_PM,          "PFC generation: Present indications that PFC is generated to Port Macro"},
    {"PFC_FROM_CFC",    DNX_FC_NIF_TYPE_PFC_FROM_CFC,       "PFC generation: Present signals from CFC indicating to generate PFC"},
    {"PFC_FROM_QMLF",   DNX_FC_NIF_TYPE_PFC_FROM_QMLF,      "PFC generation: Present signals from QMLF(Almost full NIF) indicating to generate PFC"},
    {"LLFC_FROM_QMLF",  DNX_FC_NIF_TYPE_LLFC_FROM_QMLF,     "LLFC generation: Present signals from QMLF(Almost full NIF) indicating to generate LLFC"},
    {"STOP_TO_PM",      DNX_FC_NIF_TYPE_STOP_TO_PM,         "LLFC reception: Present indications that LLFC signal to stop transmitting was sent to Port Macro"},
    {"STOP_FROM_CFC",   DNX_FC_NIF_TYPE_STOP_FROM_CFC,      "LLFC reception: Present received LLFC signals from CFC indicating to stop transmitting"},
    {"PFC_FROM_PM",     DNX_FC_NIF_TYPE_PFC_FROM_PM,        "PFC reception: Present received PFC signals from Port Macro"},
    {"PFC_TO_CFC",      DNX_FC_NIF_TYPE_PFC_TO_CFC,         "PFC reception: Present PFC signals sent to CFC"},
    {"ALL",             DNX_FC_NIF_TYPE_COUNT,              "All types"},
    {NULL}
};

static sh_sand_enum_t sand_diag_fc_oob_if_enum[] = {
    /**String    Value*/
    {"0",       DNX_FC_OOB_IF_0,          "OOB Interface 0"},
    {"1",       DNX_FC_OOB_IF_1,          "OOB Interface 1"},
    {"ALL",     DNX_FC_OOB_IF_COUNT,      "All OOB Interfaces"},
    {NULL}
};

static sh_sand_enum_t sand_diag_fc_direction_enum[] = {
    /**String    Value*/
    {"GEN",       DNX_FC_DIRECTION_GENERATION,          "Generation of FC"},
    {"REC",       DNX_FC_DIRECTION_RECEPTION,          "Reception of FC"},
    {NULL}
};

static sh_sand_enum_t     sand_diag_fc_index_to_glb_rsc_enum[] = {
    /**String    Value*/
    {"DRAM_LOW",                  DNX_FC_DRAM_TOTAL_LOW_PRIO,      "DRAM Total Low priority"},
    {"DRAM_HIGH",                 DNX_FC_DRAM_TOTAL_HIGH_PRIO,     "DRAM Total High priority"},
    {"SRAM_LOW",                  DNX_FC_SRAM_TOTAL_LOW_PRIO,      "SRAM Total Low priority"},
    {"SRAM_HIGH",                 DNX_FC_SRAM_TOTAL_HIGH_PRIO,     "SRAM Total High priority"},
    {"DRAM_P0_LOW",               DNX_FC_DRAM_POOL_0_LOW_PRIO,     "DRAM Pool 0 Low priority"},
    {"DRAM_P0_HIGH",              DNX_FC_DRAM_POOL_0_HIGH_PRIO,    "DRAM Pool 0 High priority"},
    {"DRAM_P1_LOW",               DNX_FC_DRAM_POOL_1_LOW_PRIO,     "DRAM Pool 1 Low priority"},
    {"DRAM_P1_HIGH",              DNX_FC_DRAM_POOL_1_HIGH_PRIO,    "DRAM Pool 1 High priority"},
    {"SRAM_P0_LOW",               DNX_FC_SRAM_POOL_0_LOW_PRIO,     "SRAM Pool 0 Low priority"},
    {"SRAM_P0_HIGH",              DNX_FC_SRAM_POOL_0_HIGH_PRIO,    "SRAM Pool 0 High priority"},
    {"SRAM_P1_LOW",               DNX_FC_SRAM_POOL_1_LOW_PRIO,     "SRAM Pool 1 Low priority"},
    {"SRAM_P1_HIGH",              DNX_FC_SRAM_POOL_1_HIGH_PRIO,    "SRAM Pool 1 High priority"},
    {"HDRM",                      DNX_FC_HDRM_TOTAL_HIGH_PRIO,     "HDRM Total High priority (only this priority supported)"},
    {"ALL",                       DNX_FC_GLB_RSC_INVALID,          "All resources"},
    {NULL}
};


static sh_sand_option_t sh_dnx_fc_cfc_options[] = {

    /*Name              Type                  Description                            Default      Extension*/
    {"Direction",       SAL_FIELD_TYPE_ENUM,  "Direction of FC",                                "GEN",       (void *) sand_diag_fc_direction_enum},
    {"Type",            SAL_FIELD_TYPE_ENUM,  "Flow Control type",                              "ALL",       (void *) sand_diag_fc_type_enum},
    {"Index",           SAL_FIELD_TYPE_INT32, "Which index to show, not relevant for GLB_RSC",  "-1",         NULL},
    {"Resource",        SAL_FIELD_TYPE_ENUM,  "Resource type relevant for GLB_RSC only",        "ALL",       (void *) sand_diag_fc_index_to_glb_rsc_enum},
    {"Count",           SAL_FIELD_TYPE_INT32, "How many IDs to show",                           "1",          NULL},
    {"ON",              SAL_FIELD_TYPE_BOOL,  "Show only enabled FC",                           "0",          NULL},
    {"OOB_InterFace",   SAL_FIELD_TYPE_ENUM,  "Select which OOB interface to show",             "ALL",       (void *) sand_diag_fc_oob_if_enum},
    {NULL}
};

static sh_sand_option_t sh_dnx_fc_nif_options[] = {

    /*Name              Type                  Description                            Default      Extension*/
    {"Direction",       SAL_FIELD_TYPE_ENUM,  "Direction of FC",                     "GEN",       (void *) sand_diag_fc_direction_enum},
    {"Type",            SAL_FIELD_TYPE_ENUM,  "Flow Control type",                   "ALL",       (void *) sand_diag_fc_nif_type_enum},
    {"Port",            SAL_FIELD_TYPE_PORT,  "Logical port",                        "all",        NULL},
    {"ON",              SAL_FIELD_TYPE_BOOL,  "Show only enabled FC",                "0",          NULL},
    {NULL}
};
/* *INDENT-ON* */

sh_sand_man_t sh_dnx_fc_man = {
    "Present Flow Control status from either CFC or NIF blocks",
};

static sh_sand_man_t sh_dnx_fc_cfc_man = {
    .brief = "Present Flow Control status from CFC block",
    .full =
        "Present Flow Control status from CFC block for a specified Flow Control type and direction. Directions can be GENeration and RECeption.\n\n"
        "When Generation is chosen this refers to indications from other blocks (CGM, NIF) that will result in generating one of three types of Flow control (LLFC, PFC, OOB).\n"
        "When Reception is chosen this refers to indications from one of three types of Flow control (LLFC, PFC, OOB) to other blocks (EGQ, NIF) that will result in stopping the traffic on a specific target(Interface, Port, Port+COSQ).\n"
        "If no OOB_IF option is specified, all will be shown.\n"
        "If no Index is specified all FC for the specified type will be shown. \n"
        "A range of FC for the specified type will can shown, starting with the one specified by Index with count specified in Count. \n"
        "If ON=1 is set only the indications that are ON in the specified range fill be shown \n",
    .synopsis =
        "Direction=<FC direction> Type=<FC Type> Index=<FC ID> Count=<Range of IDs to be shown> ON=<Show only FC indications that are currently On> Core=<Specify core for the OOB FC> OOB_InterFace=<In case of OOB, choose the OOB interface to be shown>",
    .examples =
        "Direction=REC Type=OOB_SINGLE Index=5 Count=10 ON=1 OOB_InterFace=0\n"
        "Direction=GEN Type=GLB_RSC Resource=DRAM_P1_HIGH"
};

static sh_sand_man_t sh_dnx_fc_nif_man = {
    .brief = "Present Flow Control status from NIF block",
    .full =
        "Present Flow Control status for a specified Flow Control type and direction. Directions can be GENeration and RECeption.\n\n"
        "When Generation is chosen this refers to indications from CFC or QMLF that will result in generating one of two types of Flow control - LLFC or PFC.\n"
        "When Reception is chosen this refers to indications of one of the two types of Flow control LLFC or PFC that will result in stopping the traffic on a specific target(Interface, Port, Port+COSQ).\n"
        "If no Port is specified all will be shown.\n" "If no Type is specified all will be shown. \n"
        "If ON=1 is set only the indications that are ON in the specified range fill be shown \n",
    .synopsis =
        "Direction=<FC direction> Type=<FC Type> Port=<Logical port> ON=<Show only FC indications that are currently On>",
    .examples = "Direction=REC Type=PFC_FROM_PM  Port=13 ON=1"
};

/**
 * \brief - This function presents the information from the NIF block for different types of Flow control
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line
 *        arguments were parsed (partially)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sh_dnx_fc_nif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    bcm_port_t logical_port_i;
    int lane, priority, lane_in_cdu = 0, nof_phys_per_cdu;
    bcm_pbmp_t lanes, logical_ports, logical_ports_nif;
    dnx_algo_port_cdu_access_info_t cdu_access_info;
    uint8 enabled = 0, only_on_indications, direction, is_pfc = 0;
    int type_start, type_end, prio_start, prio_end;
    dnx_fc_nif_type_e type;
    dbal_fields_e field = DBAL_FIELD_EMPTY;
    dbal_tables_e table = DBAL_TABLE_EMPTY;
    uint8 content_exists = FALSE;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get input parameters
     */
    SH_SAND_GET_ENUM("Direction", direction);
    SH_SAND_GET_ENUM("Type", type);
    SH_SAND_GET_PORT("port", logical_ports);
    SH_SAND_GET_BOOL("ON", only_on_indications);

    if ((type != DNX_FC_NIF_TYPE_COUNT) && (type <= DNX_FC_NIF_TYPE_LLFC_FROM_QMLF)
        && (direction == DNX_FC_DIRECTION_RECEPTION))
    {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Reception for the specified Flow Control type is not supported.\n")}

    if ((type != DNX_FC_NIF_TYPE_COUNT) && (type > DNX_FC_NIF_TYPE_LLFC_FROM_QMLF)
        && (direction == DNX_FC_DIRECTION_GENERATION))
    {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Generation for the specified Flow Control type is not supported.\n")}

    /*
     * If specified type is ALL, we need to display all types of FC
     */
    if (type == DNX_FC_NIF_TYPE_COUNT)
    {
        type_start = DNX_FC_NIF_TYPE_FIRST;
        type_end = DNX_FC_NIF_TYPE_COUNT - 1;
    }
    else
    {
        type_start = type;
        type_end = type;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FC_CDU_PFC_DEBUG, &entry_handle_id));
    for (type = type_start; type <= type_end; type++)
    {
        /*
         * Determine if the type is one of the PFC
         */
        if ((type == DNX_FC_NIF_TYPE_PFC_TO_CFC) || (type == DNX_FC_NIF_TYPE_PFC_FROM_PM)
            || (type == DNX_FC_NIF_TYPE_PFC_TO_PM) || (type == DNX_FC_NIF_TYPE_PFC_FROM_CFC)
            || (type == DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
        {
            is_pfc = 1;
        }
        else
        {
            is_pfc = 0;
        }

        /*
         * Print table header
         */
        PRT_TITLE_SET("NIF FLOW CONTROL DEBUG");
        /*
         * Print Flow Control diagnostic
         */
        PRT_COLUMN_ADD("Type");
        PRT_COLUMN_ADD("Logical Port");
        if (is_pfc)
        {
            PRT_COLUMN_ADD("Priority");
        }
        PRT_COLUMN_ADD("CDU");
        if ((!is_pfc) || (type == DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
        {
            PRT_COLUMN_ADD("Lane");
        }
        PRT_COLUMN_ADD("FC Indication");

        /*
         * Switch over the different types to determine the field and table
         */
        switch (type)
        {
            case DNX_FC_NIF_TYPE_PFC_FROM_QMLF:
                table = DBAL_TABLE_NIF_FC_PFC_QMLF_DEBUG;
                field = DBAL_FIELD_SIGNAL_STATUS;
                break;
            case DNX_FC_NIF_TYPE_LLFC_FROM_QMLF:
                table = DBAL_TABLE_NIF_FC_LLFC_QMLF_DEBUG;
                field = DBAL_FIELD_SIGNAL_STATUS;
                break;
            case DNX_FC_NIF_TYPE_LLFC_TO_PM:
                field = DBAL_FIELD_GEN_LLFC_TO_PM;
                table = DBAL_TABLE_NIF_FC_CDU_LLFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_LLFC_FROM_CFC:
                field = DBAL_FIELD_GEN_LLFC_FROM_CFC;
                table = DBAL_TABLE_NIF_FC_CDU_LLFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_STOP_TO_PM:
                field = DBAL_FIELD_LLFC_STOP_TX_TO_PM;
                table = DBAL_TABLE_NIF_FC_CDU_LLFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_STOP_FROM_CFC:
                field = DBAL_FIELD_LLFC_STOP_TX_FROM_CFC;
                table = DBAL_TABLE_NIF_FC_CDU_LLFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_PFC_FROM_CFC:
                field = DBAL_FIELD_GEN_PFC_FROM_CFC;
                table = DBAL_TABLE_NIF_FC_CDU_PFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_PFC_TO_PM:
                field = DBAL_FIELD_GEN_PFC_TO_PM;
                table = DBAL_TABLE_NIF_FC_CDU_PFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_PFC_FROM_PM:
                field = DBAL_FIELD_RX_PFC_FROM_PM;
                table = DBAL_TABLE_NIF_FC_CDU_PFC_DEBUG;
                break;
            case DNX_FC_NIF_TYPE_PFC_TO_CFC:
                field = DBAL_FIELD_RX_PFC_TO_CFC;
                table = DBAL_TABLE_NIF_FC_CDU_PFC_DEBUG;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid Flow Control Type");
        }

        /*
         * Iterate over valid logical ports bitmap
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, 0, &logical_ports_nif));
        BCM_PBMP_AND(logical_ports, logical_ports_nif);
        BCM_PBMP_ITER(logical_ports, logical_port_i)
        {
            /** If type is PFC we also have to iterate over priority */
            if (is_pfc)
            {
                prio_start = 0;
                prio_end = BCM_COS_COUNT - 1;
            }
            else
            {
                prio_start = prio_end = 0;
            }

            /** Determine the CDU access info */
            SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, logical_port_i, &cdu_access_info));
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, logical_port_i, 0, &lanes));
            nof_phys_per_cdu = dnx_data_nif.eth.cdu_lanes_nof_get(unit);

            /** Get the information from the DBAL table */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, cdu_access_info.core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CDU, cdu_access_info.inner_cdu_id);
            for (priority = prio_start; priority <= prio_end; priority++)
            {
                if (is_pfc)
                {
                    /** If type is PFC we also have priority as a key */
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PRIORITY, priority);
                }
                _SHR_PBMP_ITER(lanes, lane)
                {
                    /** If type is PFC we don't have lane as a key, except when generated from QMLF*/
                    if ((!is_pfc) || (type == DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
                    {
                        lane_in_cdu = lane % nof_phys_per_cdu;
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane_in_cdu);
                    }
                    dbal_value_field8_request(unit, entry_handle_id, field, INST_SINGLE, &enabled);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                    if (!(only_on_indications) || (enabled))
                    {
                        /*
                         * Present the information for FC
                         */
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", sand_diag_fc_nif_type_enum[type].string);
                        PRT_CELL_SET("%u", logical_port_i);
                        if (is_pfc)
                        {
                            PRT_CELL_SET("%u", priority);
                        }
                        PRT_CELL_SET("%u", cdu_access_info.inner_cdu_id);
                        /** If type is PFC we don't have lane, except when generated from QMLF*/
                        if ((!is_pfc) || (type == DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
                        {
                            PRT_CELL_SET("%u", lane_in_cdu);
                        }
                        PRT_CELL_SET("%s", (enabled ? "ON" : "OFF"));
                    }

                    /** If type is PFC we don't have lane as a key, except when generated from QMLF.
                      * Because of this we ned to break the loop over the lanes and continue with the next priority. */
                    if (is_pfc && (type != DNX_FC_NIF_TYPE_PFC_FROM_QMLF))
                    {
                        break;
                    }
                }
            }
        }
        if (PRT_ROW_NUM() > 0)
        {
            PRT_COMMITX;
            content_exists = TRUE;
        }
        else
        {
            PRT_FREE;
        }
    }

    /** If no content was found, give a message */
    if (!content_exists)
    {
        cli_out("No content for table NIF FLOW CONTROL DEBUG\n");
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function presents the information from the CFC block for different types of Flow control
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] args - Command line arguments
 * \param [in] sand_control - structure into which command line
 *        arguments were parsed (partially)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sh_dnx_fc_cfc_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    int id, id_min = 0, id_max = 0, index, id_current;
    uint32 select, select_prev, bit_idx, alloc_size;
    uint32 *status = NULL;
    uint8 enabled = 0, only_on_indications, direction;
    uint8 oob_if, oob_if_current;
    int core, core_id;
    uint32 range, max_range, max_id;
    int nof_presented, type_start, type_end;
    int if_start, if_end, is_oob = 0;
    dnx_fc_type_e type;
    dnx_fc_glb_rsc_e resource;
    dbal_fields_e field_select = DBAL_FIELD_EMPTY;
    dbal_tables_e table_select = DBAL_TABLE_EMPTY;
    dbal_fields_e field_status = DBAL_FIELD_EMPTY;
    dbal_tables_e table_status = DBAL_TABLE_EMPTY;
    uint8 content_exists = FALSE;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get input parameters
     */
    SH_SAND_GET_ENUM("Direction", direction);
    SH_SAND_GET_ENUM("Type", type);
    SH_SAND_GET_ENUM("OOB_InterFace", oob_if);
    SH_SAND_GET_ENUM("Resource", resource);
    SH_SAND_GET_INT32("Index", index);
    SH_SAND_GET_INT32("Count", range);
    SH_SAND_GET_BOOL("ON", only_on_indications);
    SH_SAND_GET_INT32("core", core_id);

    if ((type != DNX_FC_TYPE_COUNT) && (type <= DNX_FC_TYPE_VSQ_PFC) && (direction == DNX_FC_DIRECTION_RECEPTION))
    {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Reception for the specified Flow Control type is not supported.\n")}

    if ((type != DNX_FC_TYPE_COUNT) && (type > DNX_FC_TYPE_VSQ_PFC) && (direction == DNX_FC_DIRECTION_GENERATION))
    {
    SHR_ERR_EXIT(_SHR_E_PARAM, "Generation for the specified Flow Control type is not supported.\n")}

    if ((type == DNX_FC_TYPE_GLB_RSC) && (index != -1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Index for type Global resource is not supported. Please use Resource. \n");
    }
    else if ((type != DNX_FC_TYPE_GLB_RSC) && (resource != -1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Resource for the specified type is not supported. Please use Index. \n");
    }

    /*
     * If specified type is ALL, we need to display all types of FC
     */
    if (type == DNX_FC_TYPE_COUNT)
    {
        type_start = DNX_FC_TYPE_FIRST;
        type_end = DNX_FC_TYPE_COUNT - 1;
    }
    else
    {
        type_start = type;
        type_end = type;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_STATUS_SELECT, &entry_handle_id));
    for (type = type_start; type <= type_end; type++)
    {
        /*
         * Determine if the type is one of the OOB
         */
        if ((type >= DNX_FC_TYPE_OOB_SINGLE) && (type != DNX_FC_TYPE_COUNT))
        {
            is_oob = 1;
        }
        else
        {
            is_oob = 0;
        }

        /*
         * For OOB FC if specified OOB interface is ALL, we need to display results for all interfaces
         */
        if (is_oob)
        {
            if (oob_if == DNX_FC_OOB_IF_COUNT)
            {
                if_start = DNX_FC_OOB_IF_FIRST;
                if_end = DNX_FC_OOB_IF_COUNT - 1;
            }
            else
            {
                if_start = oob_if;
                if_end = oob_if;
            }
        }
        else
        {
            /*
             * So that we would only pass through the loop once
             */
            if_start = if_end = 0;
        }

        /*
         * Iterate over calendar interfaces
         */
        for (oob_if_current = if_start; oob_if_current <= if_end; oob_if_current++)
        {
            /*
             * Set the relevant tables for most of the types. For types with different tables the value will be overwritten in the switch
             */
            table_status = DBAL_TABLE_FC_STATUS;
            table_select = DBAL_TABLE_FC_STATUS_SELECT;

            /*
             * Switch over the different threshold types
             */
            switch (type)
            {
                case DNX_FC_TYPE_GLB_RSC:
                    /*
                     * Set maximum ID
                     */
                    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                                    (unit, DBAL_TABLE_FC_GLB_RSC_STATUS, DBAL_FIELD_FC_INDEX, TRUE, 0, 0,
                                     DBAL_PREDEF_VAL_MAX_VALUE, &max_id));
                    max_range = max_id + 1;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_STATUS;
                    table_status = DBAL_TABLE_FC_GLB_RSC_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_AF_NIF:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_nif.phys.nof_phys_per_core_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_AF_NIF_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_FC_EGQ:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_egr_queuing.params.nof_q_pairs_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_PFC_STATUS_SEL;
                    field_status = DBAL_FIELD_PFC_STATUS;
                    break;
                case DNX_FC_TYPE_NIF_PFC:
                    /*
                     * Set maximum ID
                     */
                    max_range =
                        (dnx_data_nif.phys.nof_phys_per_core_get(unit) * BCM_COS_COUNT);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_NIF_PFC_STATUS_SEL;
                    field_status = DBAL_FIELD_NIF_PFC_STATUS;
                    break;
                case DNX_FC_TYPE_VSQ_LLFC:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_ingr_congestion.vsq.vsq_e_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_VSQ_LLFC_STATUS_SEL;
                    field_status = DBAL_FIELD_VSQ_LLFC_STATUS;
                    break;
                case DNX_FC_TYPE_VSQ_ABCD:
                    /*
                     * Set maximum ID
                     */
                    max_range =
                        dnx_data_ingr_congestion.vsq.vsq_a_nof_get(unit) +
                        dnx_data_ingr_congestion.vsq.vsq_b_nof_get(unit) +
                        dnx_data_ingr_congestion.vsq.vsq_c_nof_get(unit) +
                        dnx_data_ingr_congestion.vsq.vsq_d_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_VSQ_ABCD_STATUS_SEL;
                    field_status = DBAL_FIELD_VSQ_ABCD_STATUS;
                    break;
                case DNX_FC_TYPE_VSQ_PFC:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_ingr_congestion.vsq.vsq_f_nof_get(unit);

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_VSQ_PFC_STATUS_SEL;
                    field_status = DBAL_FIELD_VSQ_PFC_STATUS;
                    break;
                case DNX_FC_TYPE_OOB_SINGLE:
                case DNX_FC_TYPE_OOB_PORT_BASED:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_fc.oob.calender_length_get(unit, oob_if_current)->rx;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_select = DBAL_FIELD_OOB_RX_PFC_SEL;
                    field_status = DBAL_FIELD_OOB_RX_PFC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    break;
                case DNX_FC_TYPE_OOB_LLFC:
                    /*
                     * Set maximum ID
                     * (NIF ports + 4 CMIC)
                     */
                    max_range = dnx_data_nif.phys.nof_phys_per_core_get(unit) + 4;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_OOB_RX_LLFC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_OOB_GEN_BMP:
                    /*
                     * Set maximum ID
                     */
                    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                                    (unit, DBAL_TABLE_FC_GENERIC_BITMAP, DBAL_FIELD_BMP_IDX, TRUE, 0, 0,
                                     DBAL_PREDEF_VAL_MAX_VALUE, &max_id));
                    max_range = max_id + 1;
                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_OOB_RX_GEN_PFC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_ILKN_OOB:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_fc.oob.calender_length_get(unit, oob_if_current)->rx;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_ILKN_OOB_RX_FC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                case DNX_FC_TYPE_PP_COE:
                    /*
                     * Set maximum ID
                     */
                    max_range = dnx_data_fc.coe.calender_length_get(unit, oob_if_current)->len;

                    /*
                     * Choose the appropriate fields and tables
                     */
                    field_status = DBAL_FIELD_ILKN_PP_RX_FC_STATUS;
                    table_status = DBAL_TABLE_FC_OOB_STATUS;
                    table_select = DBAL_TABLE_EMPTY;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_FAIL, "Invalid Flow Control Type");
            }

            if (type == DNX_FC_TYPE_OOB_SINGLE || type == DNX_FC_TYPE_OOB_PORT_BASED)
            {
                /*
                 * Select the appropriate target for the Q-pair based FC
                 */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_STATUS_SELECT, entry_handle_id));
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, oob_if_current);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OOB_RX_PFC_SRC_SEL, INST_SINGLE,
                                             (type == DNX_FC_TYPE_OOB_SINGLE ? 0 : 1));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }

            /*
             * Get number of bits presented at once from the DBAL table
             */
            SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_status, field_status, FALSE, 0, 0, &nof_presented));

            /*
             * Get the appropriate size and allocate the needed memory for the status
             */
            if ((nof_presented % SHR_BITWID) == 0)
            {
                alloc_size = nof_presented / SHR_BITWID;
            }
            else
            {
                alloc_size = (nof_presented / SHR_BITWID) + 1;
            }

            /*
             * The max_range is per core, but for some OOB types the status register is with size 2*max_range (each calendar instance can have targets on both cores)
             */
            if ((type == DNX_FC_TYPE_OOB_LLFC) || (type == DNX_FC_TYPE_OOB_PORT_BASED)
                || (type == DNX_FC_TYPE_OOB_SINGLE))
            {
                alloc_size = alloc_size * 2;
            }
            SHR_ALLOC_SET_ZERO(status, sizeof(uint32) * (alloc_size), "status", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

            /*
             * Determine ID max and min for the case where user wants to display more than one FC
             */
            if (type == DNX_FC_TYPE_GLB_RSC)
            {
                /** For Global resource the index is determined by the resource type enumerator */
                index = resource;
            }

            /*
             * Calculate the minimum and maximum IDs to be presented.
             */
            if ((index == -1) && (range == 1))
            {
                id_min = 0;
                id_max = max_range - 1;
            }
            else if ((index != -1) && (range == 1))
            {
                id_min = id_max = index;
            }
            else if ((index != -1) && (range != 1))
            {
                id_min = index;
                id_max = index + range;
            }

            /*
             * In case the specified range is outside the max IDs allowed, display up to max_range
             */
            if (id_max > max_range)
            {
                id_max = max_range;
            }

            /*
             * Iterate over the cores
             */
            DNXCMN_CORES_ITER(unit, core_id, core)
            {

                /*
                 * Set the select_prev to initial value to ensure first iteration will read the HW
                 */
                select_prev = 0xFFFF;

                /*
                 * Print table header
                 */
                PRT_TITLE_SET("CFC FLOW CONTROL DEBUG");
                PRT_INFO_ADD("Flow Control Type %s", sand_diag_fc_type_enum[type].string);
                if (is_oob)
                {
                    PRT_INFO_ADD("Calendar Interface %d", oob_if_current);
                }
                PRT_INFO_ADD("Showing results for core=%d", core);

                /*
                 * Print Flow Control diagnostic
                 */
                if (type == DNX_FC_TYPE_GLB_RSC)
                {
                    PRT_COLUMN_ADD("Resource");
                }
                else
                {
                    PRT_COLUMN_ADD("Index");
                }
                PRT_COLUMN_ADD("FC Indication");

                /*
                 * Iterate over IDs and get the information from the status tables
                 */
                for (id_current = id_min; id_current <= id_max; id_current++)
                {
                    /*
                     * For some OOB types each calendar instance can have targets on both cores
                     */
                    if ((type == DNX_FC_TYPE_OOB_SINGLE) || (type == DNX_FC_TYPE_OOB_PORT_BASED)
                        || (type == DNX_FC_TYPE_OOB_LLFC))
                    {
                        id = id_current + max_range * core;
                    }
                    else
                    {
                        id = id_current;
                    }

                    /*
                     * Determine the value of the select
                     */
                    select = id / nof_presented;

                    /*
                     * Read the status from HW again only is the select is changed.
                     * This means that the current ID is not in the range of the surrent select
                     */
                    if (select != select_prev)
                    {
                        if (table_select != DBAL_TABLE_EMPTY)
                        {
                            /*
                             * Set the appropriate select for the bits to be presented
                             */
                            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FC_STATUS_SELECT, entry_handle_id));
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
                            dbal_entry_value_field32_set(unit, entry_handle_id, field_select, INST_SINGLE, select);
                            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                        }

                        /*
                         * Clear the variable and get the status
                         */
                        sal_memset(status, 0x0, sizeof(uint32) * (alloc_size));
                        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_status, entry_handle_id));
                        if (is_oob)
                        {
                            /*
                             * For calendars the key to the table is the interface
                             */
                            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OOB_IF, oob_if_current);
                        }
                        else if (type == DNX_FC_TYPE_GLB_RSC)
                        {
                            /*
                             * For Global resources the keys to the table are core and resource index
                             */
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
                            dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_FC_INDEX, id);
                        }
                        else
                        {
                            /*
                             * For all other types the key to the table is only core
                             */
                            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
                        }
                        dbal_value_field_arr32_request(unit, entry_handle_id, field_status, INST_SINGLE, status);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

                        /** Registers for EGQ PFC status and NIF PFC status are both 'sticky' and will clear on read only.
                          * Changing the STATUS_SEL for each of them will result with an OR between two different ranges of the PFC vector.
                          * This is wrong behavior and can be avoided by reading the registers twice for the same select value in order to clear them. */
                        if (dnx_data_fc.inband.pfc_status_regs_double_read_get(unit))
                        {
                            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
                        }

                        /*
                         * Set the select_prev
                         */
                        select_prev = select;
                    }

                    /*
                     * Get the bit corresponding to the requested index
                     */
                    bit_idx = (type == DNX_FC_TYPE_GLB_RSC) ? 0 : (id % nof_presented);
                    enabled = SHR_BITGET(status, bit_idx) ? 1 : 0;

                    if (!(only_on_indications) || (enabled))
                    {
                        /*
                         * Present the information for both enabled and disabled FC
                         */
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        if (type == DNX_FC_TYPE_GLB_RSC)
                        {
                            PRT_CELL_SET("%s", sand_diag_fc_index_to_glb_rsc_enum[id_current].string);
                        }
                        else
                        {
                            PRT_CELL_SET("%u", id_current);
                        }
                        PRT_CELL_SET("%s", (enabled ? "ON" : "OFF"));
                    }
                }
                if (PRT_ROW_NUM() > 0)
                {
                    PRT_COMMITX;
                    content_exists = TRUE;
                }
                else
                {
                    PRT_FREE;
                }
            }
            SHR_FREE(status);
        }
    }

    /** If no content was found, give a message */
    if (!content_exists)
    {
        cli_out("No content for table CFC FLOW CONTROL DEBUG\n");
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnx_fc_cmds[] = {
    /*keyword,      action,                command,                     options,                 man                    */
    {"CFC",        sh_dnx_fc_cfc_cmd,      NULL,                        sh_dnx_fc_cfc_options,   &sh_dnx_fc_cfc_man},
    {"NIF",        sh_dnx_fc_nif_cmd,      NULL,                        sh_dnx_fc_nif_options,   &sh_dnx_fc_nif_man},
    {NULL}
};
/* *INDENT-ON* */
