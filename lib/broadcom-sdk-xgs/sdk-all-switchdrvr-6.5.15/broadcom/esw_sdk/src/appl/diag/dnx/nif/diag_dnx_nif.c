/** \file diag_dnx_nif.c
 * 
 * main file for nif diagnostics
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include "diag_dnx_nif.h"

/** local */
#include "diag_dnx_nif_lane_map.h"
#include "diag_dnx_nif_status.h"
#include "diag_dnx_nif_txrx.h"

/*
 * }
 */

/**
 * \brief DNX NIF diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for NIF diagnostic commands 
 */

/* *INDENT-OFF* */

static sh_sand_man_t sh_dnx_nif_ilkn_man = {
    .brief    = "NIF ILKN commands"
};

static sh_sand_cmd_t sh_dnx_nif_ilkn_cmds[] = {
    /*keyword,   action,                       command, options,                             man                     */
    {"lane_map", sh_dnx_nif_ilkn_lane_map_cmd, NULL,    sh_dnx_nif_ilkn_lane_map_options,    &sh_dnx_nif_ilkn_lane_map_man},
    {NULL}
};

sh_sand_cmd_t sh_dnx_nif_cmds[] = {
    /*keyword,   action,                  command,              options,                     man*/
    {"lane_map", sh_dnx_nif_lane_map_cmd, NULL,                 sh_dnx_nif_lane_map_options, &sh_dnx_nif_lane_map_man},
    {"ilkn",     NULL,                    sh_dnx_nif_ilkn_cmds, NULL,                        &sh_dnx_nif_ilkn_man},
    {"status",   sh_dnx_nif_status_cmd,   NULL,                 sh_dnx_nif_status_options,   &sh_dnx_nif_status_man},
    {"tx",       sh_dnx_nif_tx_cmd,       NULL,                 sh_dnx_nif_tx_options,       &sh_dnx_nif_tx_man},
    {"rx",       sh_dnx_nif_rx_cmd,       NULL,                 sh_dnx_nif_rx_options,       &sh_dnx_nif_rx_man},
    {NULL}
};

sh_sand_man_t sh_dnx_nif_man = {
    .brief    = "NIF commands"};

/* *INDENT-ON* */
