/*
 * $Id: vlan.c,v 1.2 Broadcom SDK $
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
 * COSQ CLI commands
 */

#include <shared/bsl.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/types.h>
#include <bcm/cosq.h>

#include <shared/bsl.h>

#include <soc/dpp/mbcm.h>
#define DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID 0xfffffff


char cmd_dpp_cosq_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  cosq <option> [args...]\n"
#else
    "  cosq comp ing voq=<id>                         - show ingress compensation\n\t"
    "  cosq comp egr port=<id>                        - show egress compensation\n\t"
    "  cosq comp ing [voq=<id>] Compensation=<value>  - set ingress compensation\n\t"
    "  cosq comp egr [port=<id>] Compensation=<value> - set egress compensation\n\t"
    "  cosq comp egr [port=<id>] cast=<UC/MC>         - set egress unicast/multicast\n\t"
    "  cosq flush enable port=<id>                    - flush port queues\n\t"
    "  cosq flush disable port=<id>                   - flush port queues\n"
#endif
    ;




int cosq_packet_length_adjust_cb (
    int unit, 
    bcm_gport_t port, 
    int numq, 
    uint32 flags, 
    bcm_gport_t gport, 
    void *user_data)
{
     int rv;
     int* ingress_compensation_ptr = user_data;
     if(BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
         rv = bcm_cosq_control_set(unit, gport, 0, bcmCosqControlPacketLengthAdjust, *ingress_compensation_ptr); 
         if(rv<0) {
             return rv;
         }
     }
     
     return 0;
}

cmd_result_t
cmd_dpp_cosq(int unit, args_t *a)
{
    char                *subcmd;
    uint32              voq, port;
    int                 comp;
    parse_table_t       pt;
    int                 rv = BCM_E_NONE;
    int                 rc;
    bcm_gport_t         gport[2] = {BCM_GPORT_INVALID};
    bcm_port_config_t   config;
    int                 cast;
    char                *cast_list[] = {"ALL", "UC", "MC"};
    int                 start = 0, count = 2;
    int                 i;

    rc=0;
    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    /* sub compensation command */
    if (sal_strcasecmp(subcmd, "comp") == 0) {

        if ((subcmd = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        /* parse values */
        parse_table_init(unit, &pt);
        voq = DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID;
        parse_table_add(&pt, "voq", PQ_DFL | PQ_INT, &voq, &voq, NULL);
        port = DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID;
        parse_table_add(&pt, "port", PQ_DFL | PQ_INT, &port, &port, NULL);
        comp = DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID;
        parse_table_add(&pt, "Compensation", PQ_DFL | PQ_INT, &comp, &comp, NULL);
        cast = 0;
        parse_table_add(&pt, "cast", PQ_DFL | PQ_MULTI, &cast, &cast, cast_list);

        if (parse_arg_eq(a, &pt) < 0 ) {
            cli_out("%s: ERROR: Unknown option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);


        /* ingress compenstation */
        if (sal_strcasecmp(subcmd, "ing") == 0) 
        {
            /* no voq specified, set all voqs */
            if (voq == DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID) {
                rv = bcm_cosq_gport_traverse(unit, cosq_packet_length_adjust_cb, &comp);
                if (rv != BCM_E_NONE) {
                    return CMD_FAIL;
                    }
            } else {
                /* specific voq */
                 BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport[0], voq);
                 /* get */
                 if (comp == DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID) {
                     rv = bcm_cosq_control_get(unit, gport[0], 0, bcmCosqControlPacketLengthAdjust, &comp); 
                     if(rv<0) {
                         return CMD_FAIL;
                     }
                  cli_out("Voq %d compensation is %d\n", voq, comp);
                 } else {
                     /* set */
                     rv = bcm_cosq_control_set(unit, gport[0], 0, bcmCosqControlPacketLengthAdjust, comp);
                     if(rv<0) {
                         return CMD_FAIL;
                     }
                   
                }
            }
        }

        /* egress compensation */
        if (sal_strcasecmp(subcmd, "egr") == 0) {

            if (cast == 0) {
                /* configure both gports (UC and MC) */
                start = 0;
                count = 2;
            } else if (cast == 1) {
                /* configure only UC gport (index 0 in gport array) */
                start = 0;
                count = 1;
            } else if (cast == 2) {
                /* configure only MC gport (index 1 in gport array) */
                start = 1;
                count = 1;
            } else {
                cli_out("ERROR: Unknown cast option\n");
                return CMD_FAIL;
            }

            /* no port specified, set all ports */
            if (port == DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID) {
                rv = bcm_port_config_get(unit, &config);
                if (rv != BCM_E_NONE) {
                    cli_out("failed to get port bmps in cint_compensation_set");
                    return rv;
                }

                BCM_PBMP_ITER(config.all, port) {
                    if(!BCM_PBMP_MEMBER(config.sfi, port)) {
                        BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport[0], port); 
                        BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(gport[1], port); 
                        
                        for (i = 0; i < count; ++i, ++start) {
                            rv = bcm_cosq_control_set(unit, gport[start], 0, bcmCosqControlPacketLengthAdjust, comp);
                            if(rv<0) {
                                return CMD_FAIL;
                            }
                        }
                    }
                }

            } else {
                /* specific port */
                BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport[0], port); 
                BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_SET(gport[1], port); 
                
                for (i = 0; i < count; ++i, ++start) {
                    /* get */
                    if (comp == DIAG_DCMN_COSQ_COMPENSTATION_DEFAULT_ID) {
                        rv = bcm_cosq_control_get(unit, gport[start], 0, bcmCosqControlPacketLengthAdjust, &comp); 
                        if(rv<0) {
                            return CMD_FAIL;
                        }
                        cli_out("Port %d compensation is %d\n", port, comp);
                        cli_out("Port %d cast is %s\n", port, cast_list[cast]);
                    } else {
                        /* set */
                        rv = bcm_cosq_control_set(unit, gport[start], 0, bcmCosqControlPacketLengthAdjust, comp);
                        if(rv<0) {
                            return CMD_FAIL;
                        }
                    }
                }
            }

        }
    } else {

            /* sub compensation command */
        if (sal_strcasecmp(subcmd, "flush") == 0) {

            if ((subcmd = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }

            /* parse values */
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "port", PQ_DFL | PQ_INT, 0, &port, NULL);

            if (parse_arg_eq(a, &pt) < 0 ) {
                cli_out("%s: ERROR: Unknown option: %s\n",
                        ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            parse_arg_eq_done(&pt);

            /* Enable flush */
            if (sal_strcasecmp(subcmd, "Enable") == 0) 
            {
                rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_cosq_flush,(unit,port,TRUE)));
                if (SOC_SAND_FAILURE(rc)) {
                    cli_out("\nFailed to enable port\n\n");
                    return CMD_FAIL;
                } 
            }

            /* Disable flush  */
            if (sal_strcasecmp(subcmd, "Disable") == 0) 
            { 
                rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_cosq_flush,(unit,port,FALSE)));
                if (SOC_SAND_FAILURE(rc)) {
                    cli_out("\nFailed to disable port\n\n");
                    return CMD_FAIL;
                } 
            }
        }

    }

    return CMD_OK;
}

