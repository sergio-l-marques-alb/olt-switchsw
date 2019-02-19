/* $Id: chipsim_appl.c,v 1.17 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:        chipsim_appl.c
 * Purpose:     main() for DPP Chipsim application integrated via bde interface
 */

#include <unistd.h>
#include <stdlib.h>

#include <appl/diag/system.h>
#if defined(INCLUDE_DUNE_UI) && defined (BCM_PETRAB_SUPPORT)
#include <appl/dpp/UserInterface/ui_pure_defi.h>
#endif

#include <sal/core/boot.h>
#include <sal/appl/sal.h>

#include <ibde.h>
#include "chipsim_bde.h"

ibde_t *bde;

int
bde_create(void)
{
    chipsim_bde_bus_t   bus;

    bus.base_addr_start = 0x40000000;
    bus.int_line = 2;
    bus.be_pio = 1;
    bus.be_packet = 0;
    bus.be_other = 1;

    return chipsim_bde_create(&bus, &bde);
}

int
main(int argc, char *argv[])
{
    int     use_sand_sim = 0;
    char    *use_sand_sim_str;

    use_sand_sim_str = getenv("USE_SAND_SIM");
    if (use_sand_sim_str != NULL) {
        use_sand_sim = atoi(use_sand_sim_str);
        if (use_sand_sim) {
            printf("Using SOC_SAND sim \n");
        }
    }
    
    /* initialize SDK */    
    if (sal_core_init() < 0) {
        printf("sal_core_init failed, exiting chipsim application\n");
        return (-1);
    }
    if (sal_appl_init() < 0) {
        printf("sal_appl_init failed, exiting chipsim application\n");
        return (-1);
    }

    if(use_sand_sim) {
#ifdef BCM_PETRAB_SUPPORT
        /* initialize chipsim */
        if (soc_pb_sim_main(0,0,0,0,0,1)) {
            printf("soc_pb_sim_main failed, exiting chipsim application\n");
            return (-1);
        }
 #endif
   }

#if defined(INCLUDE_DUNE_UI) && defined (BCM_PETRAB_SUPPORT)
    /* Init Dune UI */
    init_ui_module();
#endif

    /* launch BCM shell */
    diag_shell();

    return 0; /* only when diag_shell exits */
}

