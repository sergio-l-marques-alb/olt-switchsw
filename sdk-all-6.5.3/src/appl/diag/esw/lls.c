/*
 * $Id: lls.c,v 1.8 Broadcom SDK $
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
 * Diag CLI Buffer command
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <bcm/error.h>

#include <soc/triumph3.h>
#include <soc/trident2.h>
#include <soc/katana2.h>
#include <soc/katana.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/katana2.h>
#if defined(BCM_APACHE_SUPPORT)
#include <bcm_int/esw/apache.h>
#endif
#include <soc/apache.h>

char cmd_lls_usage[] =
    "Usages:\n\t"
    "  lls [PortBitMap=<pbm>]\n"
    "        - Show the schedule tree(s) for the LLS port(s)\n";

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
char cmd_hsp_usage[] =
    "Usages:\n\t"
    "  hsp [PortBitMap=<pbm>]\n"
    "        - Show the schedule tree(s) for the HSP port(s)\n";
#endif /* BCM_TRIDENT2_SUPPORT */

cmd_result_t
cmd_lls(int unit, args_t *a)
{
    int p;
#if defined(BCM_APACHE_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    int subcmd = 0;
#endif
    soc_pbmp_t		pbmp;
    parse_table_t	pt;
#ifdef BCM_KATANA2_SUPPORT
    soc_info_t *si = &SOC_INFO(unit);
    int pp_port;
    int start_pp_port, end_pp_port;
#endif

    SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP,
		    (void *)(0), &pbmp, NULL);

#if defined(BCM_APACHE_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_APACHE(unit) || SOC_IS_SABER2(unit)) {
        parse_table_add(&pt, "sw", PQ_DFL | PQ_NO_EQ_OPT | PQ_BOOL, 0,
                            (void *)&subcmd, 0);
    }
#endif
    if (parse_arg_eq(a, &pt) < 0) {
	cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
	parse_arg_eq_done(&pt);
	return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    PBMP_ITER(pbmp, p) {
        if (SOC_IS_TRIUMPH3(unit)) {
            soc_tr3_dump_port_lls(unit, p);
        } else if (SOC_IS_TOMAHAWK(unit)) {
            cli_out("LLS command not supported on this device\n");
            return CMD_FAIL;
#if defined(BCM_APACHE_SUPPORT)
        } else if (SOC_IS_APACHE(unit)) {
            if (subcmd) {
                bcm_apache_dump_port_lls_sw(unit, p);
            } else {
                soc_apache_dump_port_lls(unit, p);
            }
#endif
        } else if (SOC_IS_TD2_TT2(unit)) {
            soc_td2_dump_port_lls(unit, p);
        }
#ifdef BCM_KATANA2_SUPPORT
        else if (SOC_IS_KATANA2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit) && (subcmd)) {
                /* Display LLS details from SW */
                bcm_kt2_dump_port_lls(unit, p);
            } else
#endif
            if (SOC_PBMP_MEMBER(si->linkphy_pbm, p)) {
                bcm_kt2_dump_port_lls(unit, p);
            } else {
                soc_kt2_dump_port_lls(unit, p, 0);
                if (si->port_num_subport[p] > 0) {
                    start_pp_port = si->port_subport_base[p];
                    end_pp_port = si->port_subport_base[p] +
                          si->port_num_subport[p];
                    for (pp_port = start_pp_port; pp_port <
                                   end_pp_port; pp_port++) {
                         soc_kt2_dump_port_lls(unit, p, pp_port);
                    }
                }
            }
        }
#endif
#ifdef BCM_KATANA_SUPPORT
        else if (SOC_IS_KATANA(unit)) {
            bcm_kt_dump_port_lls(unit, p);
        }
#endif
    }

    return CMD_OK;
}

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
cmd_result_t
cmd_hsp(int unit, args_t *a)
{
    int p;
    soc_pbmp_t          pbmp;
    parse_table_t       pt;

    SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap",  PQ_DFL|PQ_PBMP,
                    (void *)(0), &pbmp, NULL);

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    PBMP_ITER(pbmp, p) {
        if (SOC_IS_TOMAHAWK(unit)) {
            cli_out("HSP command not supported on this device\n");
            return CMD_FAIL;
        } else if (SOC_IS_APACHE(unit)) {
            soc_apache_dump_port_hsp(unit, p);
        } else if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))) {
            soc_td2_dump_port_hsp(unit, p);
        }  else if (SOC_IS_TRIUMPH3(unit)) {
            soc_tr3_dump_port_hsp(unit, p);
        }
    }

    return CMD_OK;
}
#endif /* BCM_TRIDENT2_SUPPORT */

#endif /* (BCM_TRIUMPH3_SUPPORT | BCM_KATANA_SUPPORT) */

