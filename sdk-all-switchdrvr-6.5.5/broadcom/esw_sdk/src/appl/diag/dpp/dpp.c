/* 
 * $Id: dpp.c,v 1.8 Broadcom SDK $
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
 * File:        dpp.c
 * Purpose:     Other CLI commands
 *
 */

#include <shared/bsl.h>
#include <appl/diag/diag.h>

#if  defined(INCLUDE_DUNE_UI)
#include <appl/dpp/UserInterface/ui_pure_defi.h>
#include <appl/dpp/UserInterface/ui_ram_defi.h>
#endif /* INCLUDE_DUNE_UI */

#include <bcm/error.h>
#include <bcm/stack.h>


#if defined(INCLUDE_DUNE_UI)

char cmd_dpp_ui_usage[] =
    "Parameters: <command>\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tEnter the Dune debug CLI. \n"
    "\tParameters:\n"
    "\t\tcommand: Optional. Dune CLI command to run.\n"
    "\t\t         If not given - enters interactive UI.\n"
#endif
    ;

cmd_result_t
cmd_dpp_ui(int unit, args_t* a)
{
    char        *c;

    static int first_call = 1;

    if (first_call) {
        init_ui_module();

        first_call = 0;
    }    
    
    if (ARG_CNT(a) > 0) {
        c = ARG_GET(a);
        do {            
            if (*c == '\0') {
                handle_next_char(&Current_line_ptr,'\n') ;
                cli_out("\n");
                break;
            } else {
                handle_next_char(&Current_line_ptr,*c) ;
                ++c;
            }
        } while (1);
    } else {    
        
        cli_out("Entering Dune UI shell. Type '`' (above the tab key) to quit.\n\n"); 
        ui_proc(); 
    }

    return 0; 
}

#endif /* INCLUDE_DUNE_UI */




