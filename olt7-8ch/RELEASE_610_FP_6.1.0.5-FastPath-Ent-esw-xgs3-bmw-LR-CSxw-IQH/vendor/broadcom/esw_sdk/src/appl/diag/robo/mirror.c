/*
 * $Id: mirror.c,v 1.1 2011/04/18 17:11:00 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/stack.h>
#include <bcm/debug.h>
#include <bcm_int/control.h>


cmd_result_t
if_robo_mirror(int unit, args_t *a)
{
    int rv;
    soc_port_t port;

    static char *mode_list[] = {"Off", "L2", NULL};

    static int arg_mode = 0;     /* Off*/
    static int arg_port = -1;
    static pbmp_t arg_ing_bmp, arg_eng_bmp;
    

    parse_table_t pt;
    cmd_result_t ret_code;

    if (! sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }
    rv = bcm_mirror_to_get(unit, &arg_port);
	if (rv < 0) {
	    printk("%s: bcm_mirror_to_get failed: %s\n", ARG_CMD(a),
		   bcm_errmsg(rv));
	    return CMD_FAIL;
	}

    rv = bcm_mirror_mode_get(unit, &arg_mode);
    if (rv < 0) {
        printk("%s: bcm_mirror_mode_get failed: %s\n", ARG_CMD(a),
            bcm_errmsg(rv));
        return CMD_FAIL;
    }

    BCM_PBMP_CLEAR(arg_ing_bmp);
    BCM_PBMP_CLEAR(arg_eng_bmp);

    PBMP_ALL_ITER(unit, port) {
        int enable = 0;

        rv = bcm_mirror_ingress_get(unit, port, &enable);
        if (rv < 0 && !IS_CPU_PORT(unit, port)) {
		printk("%s: bcm_mirror_ingress_get failed: %s\n", ARG_CMD(a),
		       bcm_errmsg(rv));
		return CMD_FAIL;
	 }

	 if (enable) {
            BCM_PBMP_PORT_ADD(arg_ing_bmp, port);
	 }

        rv = bcm_mirror_egress_get(unit, port, &enable);
	 if (rv < 0 && !IS_CPU_PORT(unit, port)) {
            printk("%s: bcm_mirror_egress_get failed: %s\n", ARG_CMD(a),
                bcm_errmsg(rv));
            return CMD_FAIL;
        }

        if (enable) {
		BCM_PBMP_PORT_ADD(arg_eng_bmp, port);
        }
    }
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Mode"         , \
                    PQ_DFL|PQ_MULTI,0, &arg_mode, mode_list);
    parse_table_add(&pt, "Port"         , \
                    PQ_DFL|PQ_PORT,(void *)( 0), &arg_port, NULL);
    parse_table_add(&pt, "IngressBitMap", \
                    PQ_DFL|PQ_PBMP,(void *)(0), &arg_ing_bmp, NULL);
    parse_table_add(&pt, "EgressBitMap" , \
                    PQ_DFL|PQ_PBMP,(void *)(0), &arg_eng_bmp, NULL);    
    
    if (!parseEndOk( a, &pt, &ret_code)) {
        return ret_code;
    }
    rv =  bcm_mirror_to_set(unit, arg_port);
    
    if (rv < 0) {
        printk("%s: bcm_mirror_to_set failed: %s\n", ARG_CMD(a),
           bcm_errmsg(rv));
        if (arg_port == -1) {
            printk("mirror to port should be set\n");
        }
        return CMD_FAIL;
    }

    switch (arg_mode) {
        case 0:
            rv = bcm_mirror_mode(unit, BCM_MIRROR_DISABLE);
            break;
        case 1:
            rv = bcm_mirror_mode(unit, BCM_MIRROR_L2);
            break;
        default:
            break;
    }
    

    
    if (rv < 0) {
        printk("%s: bcm_mirror_mode failed: %s\n", ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }
    if (arg_mode == 0) {/* skip ingress/egress for disable case */
        return CMD_OK;
    }

    PBMP_FE_ITER(unit, port) {
        rv = bcm_mirror_ingress_set(unit, port,\
                                    PBMP_MEMBER(arg_ing_bmp, port) ? 1 : 0);
        if (rv < 0) {
            printk("%s: bcm_mirror_ingress_set failed: %s\n", \
                    ARG_CMD(a),bcm_errmsg(rv));
            return CMD_FAIL;
        }
        rv = bcm_mirror_egress_set(unit, port,\
                                   PBMP_MEMBER(arg_eng_bmp, port) ? 1 : 0);
        if (rv < 0) {
            printk("%s: bcm_mirror_egress_set failed: %s\n", \
                    ARG_CMD(a),bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    PBMP_GE_ITER(unit, port) {
        rv = bcm_mirror_ingress_set(unit, port,\
                                    PBMP_MEMBER(arg_ing_bmp, port) ? 1 : 0);
        if (rv < 0) {
            printk("%s: bcm_mirror_ingress_set failed: %s\n", \
                    ARG_CMD(a),bcm_errmsg(rv));
            return CMD_FAIL;
        }
        rv = bcm_mirror_egress_set(unit, port,\
                                   PBMP_MEMBER(arg_eng_bmp, port) ? 1 : 0);
        if (rv < 0) {
            printk("%s: bcm_mirror_egress_set failed: %s\n", \
                    ARG_CMD(a),bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }
                    
    return CMD_OK;
}

static void
_robo_dmirror_show(int unit, int port, int dstmod, int dstport, uint32 flags, int mymodid)
{
    int rv, show_dest;
    char *mstr;
    uint16 tpid, vlan;

    show_dest = 1;
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        if (flags & BCM_MIRROR_PORT_EGRESS) {
            mstr = "Mirror all";
        } else {
            mstr = "Mirror ingress";
        }
    } else if (flags & BCM_MIRROR_PORT_EGRESS) {
        mstr = "Mirror egress";
    } else if (flags & BCM_MIRROR_PORT_ENABLE) {
        mstr = "Mirror";
    } else {
        mstr = "-";
        show_dest = 0;
    }
    printk("%4s: %s", SOC_PORT_NAME(unit, port), mstr);
    if (show_dest) {
        if (flags & BCM_MIRROR_PORT_DEST_TRUNK) {
            printk(" to trunk %d", dstport);
        } else if (dstmod == mymodid) {
            printk(" to local port %s", SOC_PORT_NAME(unit, dstport));
        } else {
            printk(" to module %d, port %d", dstmod, dstport+1);
        }
        if (dstmod == mymodid) {
            rv = bcm_mirror_vlan_get(unit, port, &tpid, &vlan);
            if (rv == BCM_E_NONE && vlan > 0) {
                printk(" (TPID=%d, VLAN=%d)", tpid, vlan);
            }
        }
    }
    printk("\n");
}

char if_robo_dmirror_usage[] =
    "Parameters: <ports> [Mode=<Off|On|Ingress|Egress|All>]\n"
    "\t[DestPort=<port>] [DestModule=<modid>] [DestTrunk=<tgid>]\n"
    "\t[MirrorTagProtocolID=<tpid>] [MirrorVlanID=<vid>\n"
    "\tDestTrunk overrides DestModule/DestPort.\n"
    "\tTPID and VLAN are set only if DestPort is a local port.\n";

cmd_result_t
if_robo_dmirror(int unit, args_t *a)
{
    int	rv;
    int port, dstmod, dstport;
    int mymodid;
    uint32 flags;
    parse_table_t pt;
    cmd_result_t retCode;
    bcm_pbmp_t pbm, tmp_pbm;
    char *mstr;
    int argDestPort = -1, argDestMod = -1, argDestTrunk = -1;
    int argMode = -1, argTpid = -1, argVlan = -1;
    int mirror_mode = 0;

    char *modeList[] = {"OFF", "ON", "Ingress", "Egress", "All", NULL};

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    rv = bcm_stk_my_modid_get(unit, &mymodid);
    if (rv < 0) {
        printk("%s: bcm_stk_my_modid_get: %s\n", ARG_CMD(a),
               bcm_errmsg(rv));
        return CMD_FAIL;
    }

    if ((mstr = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (parse_pbmp(unit, mstr, &pbm) < 0) {
        printk("%s: Error: unrecognized port bitmap: %s\n",
               ARG_CMD(a), mstr);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, PBMP_E_ALL(unit));
    if (BCM_PBMP_IS_NULL(pbm)) {
        printk("%s: Error: unsupported port bitmap: %s\n",
               ARG_CMD(a), mstr);
        return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {
        /* Show FE ports separately (for Tucana) */
        tmp_pbm = pbm;
        BCM_PBMP_AND(tmp_pbm, PBMP_FE_ALL(unit));
        PBMP_ITER(tmp_pbm, port) {
            rv = bcm_mirror_port_get(unit, port, &dstmod, &dstport, &flags);
            if (rv < 0) {
                printk("%s: bcm_mirror_port_get: %s\n", ARG_CMD(a),
                       bcm_errmsg(rv));
                return CMD_FAIL;
            }
            _robo_dmirror_show(unit, port, dstmod, dstport, flags, mymodid);
        }
        /* Show non-FE ports */
        tmp_pbm = pbm;
        BCM_PBMP_REMOVE(tmp_pbm, PBMP_FE_ALL(unit));
        PBMP_ITER(tmp_pbm, port) {
            rv = bcm_mirror_port_get(unit, port, &dstmod, &dstport, &flags);
            if (rv < 0) {
                printk("%s: bcm_mirror_port_get: %s\n", ARG_CMD(a),
                       bcm_errmsg(rv));
                return CMD_FAIL;
            }
            _robo_dmirror_show(unit, port, dstmod, dstport, flags, mymodid);
        }
        return CMD_OK;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "DestPort", PQ_DFL|PQ_PORT,
                    (void *)(0), &argDestPort, NULL);
    parse_table_add(&pt, "DestModule", PQ_DFL|PQ_INT,
                    (void *)(0), &argDestMod, NULL);
    parse_table_add(&pt, "DestTrunk", PQ_DFL|PQ_INT,
                    (void *)(0), &argDestTrunk, NULL);
    parse_table_add(&pt, "MirrorTagProtocolID", PQ_DFL|PQ_INT,
                    (void *)(0), &argTpid, NULL);
    parse_table_add(&pt, "MirrorVlanID", PQ_DFL|PQ_INT,
                    (void *)(0), &argVlan, NULL);
    parse_table_add(&pt, "Mode", PQ_DFL|PQ_MULTI,
                    (void *)(0), &argMode, modeList);
    if (!parseEndOk( a, &pt, &retCode)) {
        return retCode;
    }

    flags = 0;

    if (argMode < 0) {
        return CMD_USAGE;
    }

    if (argMode > 0 && argDestPort < 0) {
        return CMD_USAGE;
    }

    if (argDestTrunk < 0) {
        dstport = argDestPort;
    } else {
        dstport = argDestTrunk;
        flags |= BCM_MIRROR_PORT_DEST_TRUNK;
    }

    switch (argMode) {
    case 0:
        flags |= BCM_MIRROR_DISABLE;
        break;
    case 1:
        flags |= BCM_MIRROR_PORT_ENABLE;
        break;
    case 2:
        flags |= BCM_MIRROR_PORT_INGRESS;
        break;
    case 3:
        flags |= BCM_MIRROR_PORT_EGRESS;
        break;
    case 4:
        flags |= (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS);
        break;
    default:
        break;
    }

    if (flags & BCM_MIRROR_DISABLE) {
        mirror_mode = BCM_MIRROR_DISABLE;
    } else {
        mirror_mode = BCM_MIRROR_L2;
    }

    rv = bcm_mirror_mode_set(unit, mirror_mode);
    if (rv < 0) {
        printk("%s: bcm_mirror_mode_set failed: %s\n", ARG_CMD(a),
               bcm_errmsg(rv));
        return CMD_FAIL;
    }
    
    PBMP_ITER(pbm, port) {
        rv = bcm_mirror_port_set(unit, port, argDestMod, dstport, flags);

        if (rv < 0) {
            printk("%s: bcm_mirror_port_set: %s\n", ARG_CMD(a),
                   bcm_errmsg(rv));
            return CMD_FAIL;
        }

        if (argDestMod < 0 || argDestMod == mymodid) {
            if (argTpid >= 0 && argVlan >= 0 && argDestTrunk < 0) {
                rv = bcm_mirror_vlan_set(unit, port, argTpid, argVlan);

                if (rv < 0) {
                    printk("%s: bcm_mirror_vlan_set failed: %s\n", ARG_CMD(a),
                           bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
        }
    }

    return CMD_OK;
}

