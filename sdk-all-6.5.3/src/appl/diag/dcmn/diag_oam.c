/*
 * $Id: diag_oam.c,v 1.4 Broadcom SDK $
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
 *
 * File:    diag_oam.c
 * Purpose: Manages oam diagnostics functions 
 */


#include <shared/bsl.h>
#include <shared/swstate/sw_state_hash_tbl.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <appl/diag/diag_oam.h>
#include <appl/diag/diag.h>

#ifdef BCM_DPP_SUPPORT

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oamp_pe.h>

#include <bcm_int/dpp/oam.h>
#include <bcm_int/dpp/utils.h>


/*
 * Dispalay oam endpoints.
*/
cmd_result_t
cmd_ppd_api_diag_oam_endpoint(int unit, args_t *a) {
    char *next_arg;
    int mepID;

    next_arg = ARG_GET(a);
    if (next_arg) {
        if (!sal_strncasecmp(next_arg, "id=", 3)) {
            mepID = sal_strtoul(next_arg + 3, NULL, 0);

            if ((_bcm_dpp_oam_bfd_diagnostics_endpoint_by_id(unit, mepID) != 0)) {
                cli_out("Diag failed. No endpoint with id %d found\n", mepID);
            }
        } else { /*Invalid parameter given*/
            return CMD_USAGE;
        }
    } else { /* no parameter given. print all endpoints*/
        if (_bcm_dpp_oam_bfd_diagnostics_endpoints(unit) != 0) {
            cli_out("Diag failed. Was OAM/BFD initialized?\n");
            return CMD_FAIL;
        }
    }
    return 0;

}



/*
 * Display oam lookup
*/
cmd_result_t
cmd_ppd_api_diag_oam_lookup(int unit, args_t *a) {
#ifdef LINK_ARAD_LIBRARIES
    char *next_arg= ARG_GET(a);

    if (next_arg) {
       return CMD_USAGE;
    }

    return soc_ppd_oam_diag_print_lookup(unit);

    return 0;
#else /*LINK_ARAD_LIBRARIES*/
    cli_out("Diag unsupported when compiling without LINK_ARAD_LIBRARIES");
    return 0;
#endif /*LINK_ARAD_LIBRARIES*/

}

/*
 * Display oam  counters
*/
cmd_result_t
cmd_ppd_api_diag_oam_counters(int unit, args_t *a) {
    char *next_arg= ARG_GET(a);
    int mepID;

    if (next_arg && (!sal_strncasecmp(next_arg, "id=", 3))) {
         mepID = sal_strtoul(next_arg + 3, NULL, 0);
         if (_bcm_dpp_oam_bfd_diagnostics_LM_counters(unit,mepID) !=0 ) {
             cli_out("Diag failed. No OAM endpoint with id %d found\n", mepID);
         }
    } else if (next_arg && (!sal_strncasecmp(next_arg, "oamp", 4))){
        return soc_ppd_oam_diag_print_oamp_counter(unit);
    } else {
        return CMD_USAGE;
    }

    return 0;
}

cmd_result_t
cmd_ppd_api_diag_oam_rx_packet(int unit, args_t *a) {
#ifdef LINK_ARAD_LIBRARIES
    char *next_arg= ARG_GET(a);
    int core;

    if (next_arg && (!sal_strncasecmp(next_arg, "core=", 5))) {
         core = sal_strtoul(next_arg + 5, NULL, 0);
    } else {
        return CMD_USAGE;
    }

	if ((core < 0 )  
        || 
      (core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
         cli_out("error ilegal core ID for device\n");
         return CMD_FAIL;
    } 


    return soc_ppd_oam_diag_print_rx(unit, core);

    return 0;
#else /*LINK_ARAD_LIBRARIES*/
    cli_out("Diag unsupported when compiling without LINK_ARAD_LIBRARIES");
    return 0;
#endif /*LINK_ARAD_LIBRARIES*/
}

cmd_result_t
cmd_ppd_api_diag_oam_em(int unit, args_t *a) {
#ifdef LINK_ARAD_LIBRARIES
    char *next_arg= ARG_GET(a);
    int lif;

    if (next_arg && (!sal_strncasecmp(next_arg, "lif=", 4))) {
        lif = sal_strtoul(next_arg + 4, NULL, 0);
        if (lif < 0 || lif > OAM_LIF_MAX_VALUE(unit)) {
           cli_out("lif is out of range\n");
           return CMD_OK;
        }
        if (soc_ppd_oam_diag_print_em(unit,lif) !=0 ) {
            cli_out("Diag failed.\n");
        }
    } else {
        return CMD_USAGE;
    }

    return 0;
#else /*LINK_ARAD_LIBRARIES*/
    cli_out("Diag unsupported when compiling without LINK_ARAD_LIBRARIES");
    return 0;
#endif /*LINK_ARAD_LIBRARIES*/
}


cmd_result_t
cmd_ppd_api_diag_oam_ak(int unit, args_t *a) {
#ifdef LINK_ARAD_LIBRARIES
    SOC_PPC_OAM_ACTION_KEY_PARAMS key_params = {0};
    parse_table_t pt;

    key_params.lif = -1;
    key_params.level = 0;
    key_params.opcode = 1;
    key_params.ing = 1;
    key_params.inj = 0;
    key_params.mymac = 0;
    key_params.bfd = 0;
    key_params.your_disc = 0;

    /* Parse input */
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "lif",   PQ_DFL | PQ_INT, &key_params.lif,       &key_params.lif,       NULL);
    parse_table_add(&pt, "mdl",   PQ_DFL | PQ_INT, &key_params.level,     &key_params.level,     NULL);
    parse_table_add(&pt, "op",    PQ_DFL | PQ_INT, &key_params.opcode,    &key_params.opcode,    NULL);
    parse_table_add(&pt, "ing",   PQ_DFL | PQ_INT, &key_params.ing,       &key_params.ing,       NULL);
    parse_table_add(&pt, "inj",   PQ_DFL | PQ_INT, &key_params.inj,       &key_params.inj,       NULL);
    parse_table_add(&pt, "mymac", PQ_DFL | PQ_INT, &key_params.mymac,     &key_params.mymac,     NULL);
    parse_table_add(&pt, "bfd",   PQ_DFL | PQ_INT, &key_params.bfd,       &key_params.bfd,       NULL);
    parse_table_add(&pt, "ydv",   PQ_DFL | PQ_INT, &key_params.your_disc, &key_params.your_disc, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        return CMD_USAGE;
    }

    if (key_params.lif < 0 || key_params.lif > OAM_LIF_MAX_VALUE(unit)) {
        cli_out("lif is not in range\n");
        return CMD_OK;
    }

    if (key_params.your_disc != 0 && !SOC_IS_JERICHO(unit)) {
        cli_out("ydv is only available in Jericho devices or above\n");
        return CMD_OK;
    }

    if (key_params.bfd) {
        if (key_params.level) {
            cli_out("mdl should not be set if packet is a BFD packet\n");
            return CMD_OK;
        }
        if (key_params.opcode) {
            cli_out("op should not be set if packet is a BFD packet\n");
            return CMD_OK;
        }
        if (!key_params.ing) {
            cli_out("ing should be 1 if packet is a BFD packet\n");
            return CMD_OK;
        }
    } else {
        if (key_params.your_disc) {
            cli_out("ydv should not be set if packet is not a BFD packet\n");
            return CMD_OK;
        }
    }

    if (key_params.level < 0 || key_params.level > 7) {
        cli_out("mdl is out of range (valid range is 0-7)\n");
        return CMD_OK;
    }

    if (key_params.opcode < 0 || key_params.opcode > 55) {
        cli_out("op is out of range (valid range is 1-55, 0 for bfd)\n");
        return CMD_OK;
    }

    if (key_params.ing < 0 || key_params.ing > 1) {
        cli_out("ing is out of range (use 0 for ingress and 1 for egress)\n");
        return CMD_OK;
    }

    if (key_params.inj < 0 || key_params.inj > 1) {
        cli_out("inj is out of range (use 1 for injected packets, otherwise use 0)\n");
        return CMD_OK;
    }

    if (key_params.mymac < 0 || key_params.mymac > 1) {
        cli_out("mymac is out of range (use 1 if DA on packet equals MEP's address, otherwise use 0)\n");
        return CMD_OK;
    }

    if (key_params.bfd < 0 || key_params.bfd > 1) {
        cli_out("bfd is out of range (use 1 for BFD packet, otherwise use 0)\n");
        return CMD_OK;
    }

    if (key_params.your_disc < 0 || key_params.your_disc > 1) {
        cli_out("ydv is out of range (use 1 if Your-Discriminator on packet equals MEP's My-Discriminator, otherwise use 0)\n");
        return CMD_OK;
    }

    cli_out("\nFull command issued: diag oam cl lif=0x%0x mdl=%d op=0x%0x ing=%d inj=%d mymac=%d bfd=%d ydv=%d\n",\
            key_params.lif, key_params.level, key_params.opcode, key_params.ing, key_params.inj, key_params.mymac,\
            key_params.bfd, key_params.your_disc);
    if (soc_ppd_oam_diag_print_ak(unit, &key_params) != 0) {
        cli_out("Diag failed.\n");
    }

    return 0;
#else /*LINK_ARAD_LIBRARIES*/
    cli_out("Diag unsupported when compiling without LINK_ARAD_LIBRARIES");
    return 0;
#endif /*LINK_ARAD_LIBRARIES*/
}

cmd_result_t
cmd_ppd_api_diag_oam_prge(int unit, args_t *a) {
#ifdef LINK_ARAD_LIBRARIES

    parse_table_t    pt;
    uint32         default_val = 0;
    uint32         last_program = 0;
    uint32         res = SOC_SAND_OK;

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "last", PQ_DFL|PQ_INT, &default_val,  &last_program, NULL);

    if (0 > parse_arg_eq(a, &pt)) {
        return CMD_USAGE;
    }

    /* last_program = 0, dump the programs
     * last_program = 1, only dump last program invoked
     */
    if (last_program) {
        res = arad_pp_oamp_pe_print_last_program_data(unit);
    } else {
        res = arad_pp_oamp_pe_print_all_programs_data(unit);
    }    
    /* check if previous function return an error value */
    if (soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK) { 
        return CMD_FAIL; 
    }
	
	return 0;    
#else /*LINK_ARAD_LIBRARIES*/
    cli_out("Diag unsupported when compiling without LINK_ARAD_LIBRARIES");
    return 0;
#endif /*LINK_ARAD_LIBRARIES*/
}

cmd_result_t
cmd_ppd_api_diag_oam_debug(int unit, args_t *a) {
#ifdef LINK_ARAD_LIBRARIES
    parse_table_t    pt;
    uint32         default_mode = 0;
    uint32         mode = 0;
    uint32         is_cfg = 0;

    if (ARG_CNT(a) != 0) {
        is_cfg = 1;
    }

    /* Get parameters */ 
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "mode", PQ_DFL|PQ_INT, &default_mode,  &mode, NULL);  
    
    if (0 > parse_arg_eq(a, &pt)) {
        return CMD_USAGE;
    }

    return soc_ppd_oam_diag_print_debug(unit, is_cfg, mode);
#else /*LINK_ARAD_LIBRARIES*/
    cli_out("Diag unsupported when compiling without LINK_ARAD_LIBRARIES");
    return 0;
#endif /*LINK_ARAD_LIBRARIES*/
}

cmd_result_t
cmd_ppd_api_diag_oam_oam_id(int unit, args_t *a) {
#ifdef LINK_ARAD_LIBRARIES
    char *next_arg= ARG_GET(a);
    int core;

    if (next_arg && (!sal_strncasecmp(next_arg, "core=", 5))) {
         core = sal_strtoul(next_arg + 5, NULL, 0);
    } else {
        return CMD_USAGE;
    }
	if ((core < 0 )  
        || 
      (core >= SOC_DPP_DEFS_GET(unit, nof_cores))) {
         cli_out("error ilegal core ID for device\n");
         return CMD_FAIL;
    } 

    return soc_ppd_oam_diag_print_oam_id(unit, core);

    return 0;
#else /*LINK_ARAD_LIBRARIES*/
    cli_out("Diag unsupported when compiling without LINK_ARAD_LIBRARIES");
    return 0;
#endif /*LINK_ARAD_LIBRARIES*/
}


/*
 * Print diag oam usage
*/
void print_oam_usage(int unit) {
    char cmd_dpp_diag_oam_usage[] =
       "Usage (DIAG oam):"
       "\n\tDIAGnotsics OAM commands\n\t"
       "Usages:\n\t"
       "DIAG oam <OPTION> <parameters> ..."
#ifdef __PEDANTIC__
       "\nFull documentation cannot be displayed with -pendantic compiler"
#else
       "OPTION can be:"
       "\n\nEndPoints- \tDisplay information on all endpoints and their remote endpoints"
       "\n\tOptional parameters:"
       "\n\t\tID=<endpoint id> - Display an OAM endpoint according to the endpoint ID."
       "\n\nLookUps- \tDisplay lookup information from OAM exact matches."
       "\n\nCOUNTers- \tDisplay OAM LM counters for a given endpoint ID."
       "\n\tParameters:"
       "\n\t\tID=<endpoint id> - endpoint for which counters are to be displayed"
	   "\n\t\tOAMP - Display OAMP counters"
       "\n\nRX_packet- \tDisplay information about the last RX OAM packet."
       "\n\nExact_Match-\tDisplay O-EM-1 entries by LIF."
       "\n\tParameters:"
       "\n\t\tLIF=<LIF> Key for O-EM-1 entry"
       "\n\nCLassification-\tSimulates OAM classification based on O-EM-1/2 inputs and OAM-1/2 inputs."
           "\n\t\tRetrieves OAM-1/2 action-key & entry."
       "\n\tParameters:"
       "\n\t\tLIF   = <LIF> Key for O-EM-1 entry (mandatory value, must be set, use global lif in Jericho)"
       "\n\t\tMDL   = <Level> Level of the packet                     (default is 0)"
       "\n\t\tOP    = <op-code> OAM Op-Code                           (default is 1 = CCM)"
       "\n\t\tING   = <yes/no> Is classification in Ingress?          (default is 1)"
       "\n\t\tINJ   = <yes/no> Is inject bit is on?                   (default is 0)"
       "\n\t\tMYMAC = <yes/no> Is packet DA equals MEP's MAC address? (default is 0)"
       "\n\t\tBFD   = <yes/no> Is packet is a BFD packet?             (default is 0)"
       "%s" /* For adding line dynamically (for example based on device type) */
       "\n\nPRoGram_Editor-\tDisplay program editor."
       "\n\tParameters:"
       "\n\t\tlast=<0 or 1> Is last invoked PE program."
       "\n\nDeBuG-\tDebug for ARAD mode OAM ID."
       "\n\tParameters:"
       "\n\t\tmode=<0 or 1> Configure ARAD mode OAM ID."
       "\nOAM_ID -\tDisplay for OAM ID."
#endif
           "\n";

    cli_out(cmd_dpp_diag_oam_usage,
            SOC_IS_JERICHO(unit) ? "\n\t\tYDV   = <yes/no> Is Your-Disc-Valid set?                (default is 0)" : ""
            );
}

/*
 * "Driver" for the diag oam functionalities.
*/
cmd_result_t
cmd_dpp_diag_oam(int unit, args_t *a) {
    char      *function;

    function = ARG_GET(a);
    if (!function) {
        return CMD_USAGE;
    } else if (DIAG_FUNC_STR_MATCH(function, "endpoints", "ep")) {
        return cmd_ppd_api_diag_oam_endpoint(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "lookups", "lu")) {
        return cmd_ppd_api_diag_oam_lookup(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "counters", "count")) {
        return cmd_ppd_api_diag_oam_counters(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "rx_packet", "rx")) {
        return cmd_ppd_api_diag_oam_rx_packet(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "exact_match", "em")) {
        return cmd_ppd_api_diag_oam_em(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "action_key", "ak")) {
        cli_out("Please use 'diag oam cl' instead\n"); return 0;
    } else if (DIAG_FUNC_STR_MATCH(function, "classification", "cl")) {
        return cmd_ppd_api_diag_oam_ak(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "program_editor", "prge")) {
        return cmd_ppd_api_diag_oam_prge(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "debug", "dbg")) {
        return cmd_ppd_api_diag_oam_debug(unit, a);
    } else if (DIAG_FUNC_STR_MATCH(function, "oam_id", "oamid")) {
        return cmd_ppd_api_diag_oam_oam_id(unit, a);;
    } else {
        return CMD_USAGE;
    }

}

#endif /* BCM_DPP_SUPPORT */
