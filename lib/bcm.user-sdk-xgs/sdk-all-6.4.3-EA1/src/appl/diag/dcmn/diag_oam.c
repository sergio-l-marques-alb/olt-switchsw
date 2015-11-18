/*
 * $Id: diag_oam.c,v 1.4 Broadcom SDK $
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
 * File:    diag_oam.c
 * Purpose: Manages oam diagnostics functions 
 */


#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>

#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_oam.h>
#endif

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <appl/diag/diag_oam.h>
#include <appl/diag/diag.h>

#ifdef BCM_DPP_SUPPORT
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

    return arad_pp_oam_diag_print_lookup(unit);

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
    } else {
        return CMD_USAGE;
    }

    return 0;
}

cmd_result_t
cmd_ppd_api_diag_oam_rx_packet(int unit, args_t *a) {
#ifdef LINK_ARAD_LIBRARIES
    char *next_arg= ARG_GET(a);

    if (next_arg) {
       return CMD_USAGE;
    }

    return arad_pp_oam_diag_print_rx(unit);

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
       "\nFull documentation cannot be displayed with -pendantic compiler\n";
#else 
       "OPTION can be:"
       "\nEndPoints - \tDisplay information on all endpoints and their remote endpoints"
       "\n\tOptional parameters:"
       "\n\t\tID=<endpoint id> - Display an OAM endpoint according to the endpoint ID."
       "\nLookUps - \tDisplay lookup information from OAM exact matches."
       "\nCOUNTers - \tDisplay OAM LM counters for a given endpoint ID."
       "\n\tParameters:"
       "\n\t\tID=<endpoint id> - endpoint for which counters are to be displayed"
       "\nRX_packet - \tDisplay information about the last RX OAM packet."
       "\n";
#endif
           


    cli_out(cmd_dpp_diag_oam_usage);
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
    } else {
        return CMD_USAGE;
    }

}

#endif /* BCM_DPP_SUPPORT */
