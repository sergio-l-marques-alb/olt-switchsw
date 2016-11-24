/*
 * $Id: port.c,v 1.1 2011/04/18 17:10:59 mruas Exp $
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
 *
 * File:        port.c
 * Purpose:     Functions to support CLI port commands
 * Requires:    
 */


#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/core/libc.h>

#include <assert.h>

#include <soc/debug.h>
#include <soc/xaui.h>
#include <soc/phyctrl.h>
#include <soc/phy.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>

#include <bcm/init.h>
#include <bcm/port.h>
#include <bcm/stg.h>
#include <bcm/error.h>

#include <bcm_int/esw/port.h>

/*
 * Function:
 *	_if_fmt_speed
 * Purpose:
 *	Format a speed as returned from bcm_xxx for display.
 * Parameters:
 *	b     - buffer to format into.
 *	speed - speed as returned from bcm_port_speed_get
 * Returns:
 *	Pointer to buffer (b).
 */
static char *
_if_fmt_speed(char *b, int speed)
{
    if (speed >= 1000) {                /* Use Gb */
        if (speed % 1000) {             /* Use Decimal */
            sal_sprintf(b, "%d.%dG", speed / 1000, (speed % 1000) / 100);
        } else {
            sal_sprintf(b, "%dG", speed / 1000);
        }
    } else if (speed == 0) {
	sal_sprintf(b, "-");
    } else {                            /* Use Mb */
        sal_sprintf(b, "%dM", speed);
    }
    return(b);
}

/*
 * These are ordered according the corresponding enumerated types.
 * See soc/portmode.h, bcm/port.h and bcm/link.h for more information
 */

/* Note:  See port.h, bcm_port_discard_e */
char    *discard_mode[] = {
    "None", "All", "Untag", "Tag", NULL
};
/* Note:  See link.h, bcm_linkscan_mode_e */
char            *linkscan_mode[] = {
    "None", "SW", "HW", "OFF", "ON", NULL
};
/* Note:  See portmode.h, soc_port_if_e */
char            *interface_mode[] = {
    "NONE", "NULL", "MII", "GMII", "SGMII", "TBI", "XGMII", "RGMII","RvMII", NULL
};
/* Note:  See portmode.h, soc_port_ms_e */
char            *phymaster_mode[] = {
    "Slave", "Master", "Auto", "None", NULL
};
/* Note:  See port.h, bcm_port_loopback_e */
char            *loopback_mode[] = {
    "NONE", "MAC", "PHY", "RMT", "C57", NULL
};
/* Note:  See port.h, bcm_port_stp_e */
char            *forward_mode[] = {
    "Disable", "Block", "LIsten", "LEarn", "Forward", NULL
};
/* Note: See port.h, bcm_port_encap_e */
char            *encap_mode[] = {
    "IEEE", "HIGIG", "B5632", "HIGIG2", NULL
};
/* Note: See port.h, bcm_port_mdix_e */
char            *mdix_mode[] = {
    "Auto", "ForcedAuto", "ForcedNormal", "ForcedXover", NULL
};
/* Note: See port.h, bcm_port_mdix_status_e */
char            *mdix_status[] = {
    "Normal", "Xover", NULL
};
/* Note: See port.h, bcm_port_medium_t */
char           *medium_status[] = {
    "None", "Copper", "Fiber", NULL
};
/* Note: See port.h, bcm_port_mcast_flood_t */
char           *mcast_flood[] = {
    "FloodAll", "FloodUnknown", "FloodNone", NULL
};
/* Note: See port.h, bcm_port_phy_control_t */
char           *phy_control[] = {
    "WAN                     ", 
    "Preemphasis             ", 
    "DriverCurrent           ", 
    "PreDriverCurrent        ", 
    "Interface               ",
    "InterfaceMAX            ",
    "MacsecSwitchFixed       ", 
    "MacsecSwitchFixedSpeed  ", 
    "MacsecSwitchFixedDuplex ", 
    "MacsecSwitchFixedPause  ", 
    "MacsecPauseRxForward    ", 
    "MacsecPauseTxForward    ", 
    "MacsecLineIPG           ", 
    "MacsecSwitchIPG         ", 
    "LongreachSpeed          ",
    "LongreachPairs          ",
    "LongreachGain           ",
    "LongreachAutoneg        ",
    "LongreachLocalAbility   ",
    "LongreachRemoteAbility  ",
    "LongreachCurrentAbility ",
    "LongreachMaster         ", 
    "LongreachActive         ", 
    "LongreachEnable         ", 
    "PrePremphasis           ",
    "Encoding                ",
    "Scrambler               ",
    "PrbsPolynomial          ",
    "PrbsTxInvertData        ",
    "PrbsTxEnable            ",
    "PrbsRxEnable            ",
    "PrbsRxStatus            ",
    "SerdesDriverTune        ",
    "SerdesDriverEqualTuneStatus",
    "8b10b                   ",
    NULL
};

void
brief_port_info_header(int unit)
{
    char *disp_str =
	"%7s  "          /* port number */
	"%4s "           /* enable/link state */
	"%7s "           /* speed/duplex */
	"%4s "           /* link scan mode */
	"%4s "           /* auto negotiate? */
	"%7s   "         /* spantree state */
	"%5s  "          /* pause tx/rx */
	"%6s "           /* discard mode */
	"%3s "           /* learn to CPU, ARL, FWD or discard */
	"%6s "           /* interface */
	"%5s "           /* max frame */
	"%5s\n";         /* loopback */

    printk(disp_str,
	   " ",          /* port number */
	   "ena/",       /* enable/link state */
	   "speed/",     /* speed/duplex */
	   "link",       /* link scan mode */
	   "auto",       /* auto negotiate? */
	   " STP ",      /* spantree state */
	   " ",          /* pause tx/rx */
	   " ",          /* discard mode */
	   "lrn",        /* learn to CPU, ARL, FWD or discard */
	   "inter",      /* interface */
	   "max",        /* max frame */
	   "loop");      /* loopback */
    printk(disp_str,
	   "port",       /* port number */
	   "link",       /* enable/link state */
	   "duplex",     /* speed/duplex */
	   "scan",       /* link scan mode */
	   "neg?",       /* auto negotiate? */
	   "state",      /* spantree state */
	   "pause",      /* pause tx/rx */
	   "discrd",     /* discard mode */
	   "ops",        /* learn to CPU, ARL, FWD or discard */
	   "face",       /* interface */
	   "frame",      /* max frame */
	   "back");      /* loopback */
}

#define _CHECK_PRINT(flags, mask, str, val) \
    if ((flags) & (mask)) printk(str, val); \
    else printk(str, "")

int
brief_port_info(char *port_ref, bcm_port_info_t *info, uint32 flags)
{
    char        *spt_str, *discrd_str;
    char        sbuf[6];
    int         lrn_ptr;
    char        lrn_str[4];

    spt_str = FORWARD_MODE(info->stp_state);
    discrd_str = DISCARD_MODE(info->discard);

    /* port number (7)
     * enable/link state (4)
     * speed/duplex (6)
     * link scan mode (4)
     * auto negotiate? (4)
     * spantree state (7)
     * pause tx/rx (5)
     * discard mode (6)
     * learn to CPU, ARL, FWD or discard (3)
     */
    printk("%7s  %4s ", port_ref,
	   !info->enable ? "!ena" :
           (info->linkstatus== BCM_PORT_LINK_STATUS_FAILED) ? "fail" :
           (info->linkstatus ? "up  " : "down"));
    _CHECK_PRINT(flags, BCM_PORT_ATTR_SPEED_MASK,
                 "%4s ", _if_fmt_speed(sbuf, info->speed));
    _CHECK_PRINT(flags, BCM_PORT_ATTR_DUPLEX_MASK,
                 "%2s ", info->speed == 0 ? "" : info->duplex ? "FD" : "HD");
    _CHECK_PRINT(flags, BCM_PORT_ATTR_LINKSCAN_MASK,
                 "%4s ", LINKSCAN_MODE(info->linkscan));
    _CHECK_PRINT(flags, BCM_PORT_ATTR_AUTONEG_MASK,
                 "%4s ", info->autoneg ? " Yes" : " No ");
    _CHECK_PRINT(flags, BCM_PORT_ATTR_STP_STATE_MASK,
                 " %7s  ", spt_str);
    _CHECK_PRINT(flags, BCM_PORT_ATTR_PAUSE_TX_MASK,
                 "%2s ", info->pause_tx ? "TX" : "");
    _CHECK_PRINT(flags, BCM_PORT_ATTR_PAUSE_RX_MASK,
                 "%2s ", info->pause_rx ? "RX" : "");
    _CHECK_PRINT(flags, BCM_PORT_ATTR_DISCARD_MASK,
                 "%6s  ", discrd_str);

    lrn_ptr = 0;
    sal_memset(lrn_str, 0, sizeof(lrn_str));
    lrn_str[0] = 'D';
    if (info->learn & BCM_PORT_LEARN_FWD) {
        lrn_str[lrn_ptr++] = 'F';
    }
    if (info->learn & BCM_PORT_LEARN_ARL) {
        lrn_str[lrn_ptr++] = 'A';
    }
    if (info->learn & BCM_PORT_LEARN_CPU) {
        lrn_str[lrn_ptr++] = 'C';
    }
    _CHECK_PRINT(flags, BCM_PORT_ATTR_LEARN_MASK,
                 "%3s ", lrn_str);
    _CHECK_PRINT(flags, BCM_PORT_ATTR_INTERFACE_MASK,
		 "%6s ", INTERFACE_MODE(info->interface));
    if (flags & BCM_PORT_ATTR_FRAME_MAX_MASK) {
	printk("%5d ", info->frame_max);
    } else {
	printk("%5s ", "");
    }
    _CHECK_PRINT(flags, BCM_PORT_ATTR_LOOPBACK_MASK,
		 "%5s",
		 info->loopback != BCM_PORT_LOOPBACK_NONE ?
		 LOOPBACK_MODE(info->loopback) : "");

    printk("\n");
    return 0;
}

/*
 * Function:
 *	if_port_stat
 * Purpose:
 *	Table display of port information
 * Parameters:
 *	unit - SOC unit #
 *	a - pointer to args
 * Returns:
 *	CMD_OK/CMD_FAIL
 */
cmd_result_t
if_esw_port_stat(int unit, args_t *a)
{
    pbmp_t              pbm, tmp_pbm;
    bcm_port_info_t    *info_all;
    bcm_port_config_t   pcfg;
    soc_port_t          p, dport;
    int                 r;
    char               *c;
    uint32              mask;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        printk("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        BCM_PBMP_ASSIGN(pbm, pcfg.port);
    } else if (parse_bcm_pbmp(unit, c, &pbm) < 0) {
        printk("%s: Error: unrecognized port bitmap: %s\n",
               ARG_CMD(a), c);
        return CMD_FAIL;
    }
    BCM_PBMP_AND(pbm, pcfg.port);
    if (BCM_PBMP_IS_NULL(pbm)) {
        printk("No ports specified.\n");
        return CMD_OK;
    }
    
    mask = BCM_PORT_ATTR_ALL_MASK;

    info_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t), 
                         "if_port_stat");
    if (info_all == NULL) {
        printk("Insufficient memory.\n");
        return CMD_FAIL;
    }

    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        port_info_init(unit, p, &info_all[p], mask);
        if ((r = bcm_port_selective_get(unit, p, &info_all[p])) < 0) {
            printk("%s: Error: Could not get port %s information: %s\n",
                   ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(r));
            sal_free(info_all);
            return (CMD_FAIL);
        }
    }

    brief_port_info_header(unit);

#define _call_bpi(pbm, pbm_mask) \
    tmp_pbm = pbm_mask; \
    BCM_PBMP_AND(tmp_pbm, pbm); \
    DPORT_BCM_PBMP_ITER(unit, tmp_pbm, dport, p) { \
        brief_port_info(BCM_PORT_NAME(unit, p), &info_all[p], mask); \
    }

    if (soc_property_get(unit, spn_DPORT_MAP_ENABLE, TRUE)) {
        /* If port mapping is enabled, then use port order */
        _call_bpi(pbm, pcfg.port);
    } else {
        /* If no port mapping, ensure that ports are grouped by type */
        _call_bpi(pbm, pcfg.fe);
        _call_bpi(pbm, pcfg.ge);
        _call_bpi(pbm, pcfg.xe);
        _call_bpi(pbm, pcfg.hg);
    }

    sal_free(info_all);

    return CMD_OK;
}

/*
 * Function:
 *	if_port_rate
 * Purpose:
 *	Set/display of port rate metering characteristics
 * Parameters:
 *	unit - SOC unit #
 *	a - pointer to args
 * Returns:
 *	CMD_OK/CMD_FAIL
 */
cmd_result_t
if_esw_port_rate(int unit, args_t *a)
{
    pbmp_t          pbm;
    bcm_port_config_t pcfg;
    soc_port_t      p, dport;
    int             operation = 0;
    int             rv;
    int             header;
    uint32          rate = 0xFFFFFFFF;
    uint32          burst = 0xFFFFFFFF;
    char           *c;

#define SHOW    1
#define INGRESS 2
#define EGRESS  4
#define PAUSE   8

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    /* Check for metering capabilities */
    if (! soc_feature(unit, soc_feature_ingress_metering) &&
	! soc_feature(unit, soc_feature_egress_metering) ) {
        printk("%s: Error: metering unavailable for this device\n", 
	       ARG_CMD(a));
        return CMD_OK;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        printk("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        BCM_PBMP_ASSIGN(pbm, pcfg.port);
    } else if (parse_bcm_pbmp(unit, c, &pbm) < 0) {
        printk("%s: Error: unrecognized port bitmap: %s\n",
               ARG_CMD(a), c);
        return CMD_FAIL;
    }

    /* Apply PortRate only to those ports which support it */
    BCM_PBMP_AND(pbm, pcfg.e);
    if (BCM_PBMP_IS_NULL(pbm)) {
        printk("No ports specified.\n");
        return CMD_OK;
    }
    
    /* Ingress, egress or show both */
    if ((c = ARG_GET(a)) == NULL) {
        operation = SHOW;
	if (soc_feature(unit, soc_feature_ingress_metering)) {
	    operation |= INGRESS;
	}
	if (soc_feature(unit, soc_feature_egress_metering)) {
	    operation |= EGRESS;
	}
    }
    else if (!sal_strncasecmp(c, "ingress", strlen(c))) {
	if (soc_feature(unit, soc_feature_ingress_metering)) {
	    operation = INGRESS;
	} else {
	    printk("%s: Error: ingress metering unavailable for "
		   "this device\n", ARG_CMD(a));
	    return CMD_OK;
	}
    }
    else if (!sal_strncasecmp(c, "egress", strlen(c))) {
	if (soc_feature(unit, soc_feature_egress_metering)) {
	    operation = EGRESS;
	} else {
	    printk("%s: Error: egress metering unavailable for "
		   "this device\n", ARG_CMD(a));
	    return CMD_OK;
	}
    }
    else if (SOC_IS_TUCANA(unit) && !sal_strncasecmp(c, "pause", strlen(c))) {
        operation = PAUSE;
    }
    else {
        printk("%s: Error: unrecognized port rate type: %s\n",
               ARG_CMD(a), c);
        return CMD_FAIL;
    }

    /* Set or get */
    if ((c = ARG_GET(a)) != NULL) {
        rate = parse_integer(c);
        if ((c = ARG_GET(a)) != NULL) {
            burst = parse_integer(c);
        }
        else {
            printk("%s: Error: missing port burst size\n",
                   ARG_CMD(a));
            return CMD_FAIL;
        }
    }
    else {
        operation |= SHOW;
    }

    DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
        if (operation & SHOW) {
            /* Display current setting */
            header = 0;
            if (operation & (INGRESS | PAUSE)) {
                rv = bcm_port_rate_ingress_get(unit, p, &rate, &burst); 
                if (rv < 0) {
                    printk("%s port %s: ERROR: bcm_port_rate_ingress_get: "
                           "%s\n",
                           ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                if (rate) {
                    printk("%4s:", BCM_PORT_NAME(unit, p));
                    header = 1;
                    if (rate < 64) { 
                        printk("\tIngress meter: ? kbps ? kbits max burst.\n");
                    }
                    else {
                        printk("\tIngress meter: "
                               "%8d kbps %8d kbits max burst.\n",
                               rate, burst);
                    }
                    if (SOC_IS_TUCANA(unit)) {
                        rv = bcm_port_rate_pause_get(unit, p, &rate, &burst);
                        if (rv < 0) {
                            printk("%s port %s: ERROR: "
                                   "bcm_port_rate_pause_get: %s\n",
                                   ARG_CMD(a), BCM_PORT_NAME(unit, p),
                                   bcm_errmsg(rv));
                            return CMD_FAIL;
                        }
                        if (rate) { 
                            printk("\tPause frames: Pause = %8d kbits, "
                                   "Resume = %8d kbits.\n", rate, burst);
                        }
                    }
                }
            }
            if (operation & EGRESS) { 
                rv = bcm_port_rate_egress_get(unit, p, &rate, &burst);
                if (rv < 0) {
                    printk("%s port %s: ERROR: bcm_port_rate_egress_get: %s\n",
                           ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                if (rate) { 
                    if (!header)
                        printk("%4s:", BCM_PORT_NAME(unit, p));
                    printk("\tEgress meter:  %8d kbps %8d kbits max burst.\n",
                           rate, burst);
                }
            }
        }
        else {
            /* New setting */
            if (!rate || !burst)
                rate = burst = 0; /* Disable port metering */
            if (operation & INGRESS) {
                rv = bcm_port_rate_ingress_set(unit, p, rate, burst); 
                if (rv < 0) {
                    printk("%s: ERROR: bcm_port_rate_ingress_set: %s\n",
                           ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
            else if (operation & PAUSE) {
                rv = bcm_port_rate_pause_set(unit, p, rate, burst); 
                if (rv < 0) {
                    printk("%s: ERROR: bcm_port_rate_pause_set: %s\n",
                           ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
            else if (operation & EGRESS) {
                rv = bcm_port_rate_egress_set(unit, p, rate, burst);
                if (rv < 0) {
                    printk("%s: ERROR: bcm_port_rate_egress_set: %s\n",
                           ARG_CMD(a), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            }
        }
    }
#undef SHOW 
#undef INGRESS
#undef EGRESS
#undef PAUSE

    return CMD_OK;
}

/*
 * Function:
 *      if_port_samp_rate
 * Purpose:
 *      Set/display of sflow port sampling rates.
 * Parameters:
 *      unit - SOC unit #
 *      args - pointer to comand line arguments
 * Returns:
 *      CMD_OK/CMD_FAIL
 */
char if_port_samp_rate_usage[] =
    "Set/Display port sampling rate characteristics.\n"
    "Parameters: <pbm> [ingress_rate] [egress_rate]\n"
    "\tOn average, every 1/ingress_rate packets will be sampled.\n"
    "\tA rate of 0 indicates no sampling.\n"
    "\tA rate of 1 indicates all packets sampled.\n";

cmd_result_t
if_port_samp_rate(int unit, args_t *args)
{
#define SHOW    0x01
#define SET     0x02
    pbmp_t          pbm;
    bcm_port_config_t pcfg;
    char           *ch;
    int             operation    = SET; /* Set or Show */
    int             ingress_rate = -1;
    int             egress_rate  = -1;
    soc_port_t      soc_port, dport;
    int             retval;

    if (!sh_check_attached(ARG_CMD(args), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        printk("%s: Error: bcm ports not initialized\n", ARG_CMD(args));
        return CMD_FAIL;
    }

    /* get port bitmap */
    if ((ch = ARG_GET(args)) == NULL) {
        BCM_PBMP_ASSIGN(pbm, pcfg.port);
    } else if (parse_bcm_pbmp(unit, ch, &pbm) < 0) {
        printk("%s: Error: unrecognized port bitmap: %s\n",
               ARG_CMD(args), ch);
        return CMD_FAIL;
    }

    /* read in ingress_rate and egress_rate if given */
    if ((ch = ARG_GET(args)) != NULL) {
        ingress_rate = parse_integer(ch);
        if ((ch = ARG_GET(args)) != NULL) {
            egress_rate = parse_integer(ch);
        }
        else {
            printk("%s: Error: missing egress rate \n", ARG_CMD(args));
            return CMD_FAIL;
        }
    }
    else {
        operation = SHOW;
    }

    /* Iterate through port bitmap and perform 'operation' on them. */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, soc_port) {
        if (operation == SHOW) {
        /* Show port sflow sample rate(s) */
            retval = bcm_port_sample_rate_get(unit, soc_port, &ingress_rate,
                                              &egress_rate);
            if (retval != BCM_E_NONE) {
                printk("%s port %s: ERROR: bcm_port_sample_rate_get: "
                       "%s\n", ARG_CMD(args),
                       BCM_PORT_NAME(unit, soc_port), bcm_errmsg(retval));
                return CMD_FAIL;
            }

            printk("%4s:", BCM_PORT_NAME(unit, soc_port));
 
            if ( ingress_rate == 0 ) {
                printk("\tingress: not sampling,");
            }
            else {
                printk("\tingress: 1 out of %d packets,", ingress_rate);
            }
            if ( egress_rate == 0 ) {
                printk("\tegress: not sampling,");
            }
            else {
                printk("\tegress: 1 out of %d packets,", egress_rate);
            }
            printk("\n");
        }
        else {
        /* Set port sflow sample rate(s) */
            retval = bcm_port_sample_rate_set(unit, soc_port, ingress_rate, egress_rate);
            if (retval != BCM_E_NONE) {
                printk("%s port %s: ERROR: bcm_port_sample_rate_set: "
                       "%s\n", ARG_CMD(args),
                       BCM_PORT_NAME(unit, soc_port), bcm_errmsg(retval));
                return CMD_FAIL;
            }
        }
    }

#undef SHOW
#undef SET
    return CMD_OK;
}
 
/*
 * Function:
 *	disp_port_info
 * Purpose:
 *	Display selected port information
 * Parameters:
 *	info	    - pointer to structure with info to display
 *	port_ref    - Port reference to print
 *	st_port     - Is the port a hi-gig port?
 * Returns:     
 *	Nothing
 * Notes:
 *	Assumes link status info always valid
 */

void
disp_port_info(char *port_ref, bcm_port_info_t *info, 
               int st_port, uint32 flags)
{
    int 		r;
    int 		no_an_props = 0;   /* Do not show AN props */
    bcm_port_ability_t *local = &info->port_ability;
    bcm_port_ability_t *remote = &info->remote_ability;
    bcm_port_ability_t *advert = &info->local_ability;

    /* Assume link status always available. */
    printk(" %c%-7s ", info->linkstatus ? '*' : ' ', port_ref);

    if (info->linkstatus == BCM_PORT_LINK_STATUS_FAILED) {
        printk("%s", "FAILED ");
    }

    if (flags & BCM_PORT_ATTR_ENABLE_MASK) {
        printk("%s", info->enable ? "" : "DISABLED ");
    }

    if (flags & BCM_PORT_ATTR_LINKSCAN_MASK) {
        if (info->linkscan) {
            printk("LS(%s) ", LINKSCAN_MODE(info->linkscan));
        }
    }
    if (st_port) {
        printk("%s(", ENCAP_MODE(info->encap_mode & 0x3));
    } else if (flags & BCM_PORT_ATTR_AUTONEG_MASK) {
        if (info->autoneg) {
            if (!info->linkstatus) {
                printk("Auto(no link) ");
                no_an_props = 1;
            } else {
                printk("Auto(");
            }
        } else {
            printk("Forced(");
        }
    } else {
        printk("AN?(");
    }

    /* If AN is enabled, but not complete, don't show port settings */
    if (!no_an_props) {
        if (flags & BCM_PORT_ATTR_SPEED_MASK) {
	    char	buf[6];
            printk("%s", _if_fmt_speed(buf, info->speed));
        }
        if (flags & BCM_PORT_ATTR_DUPLEX_MASK) {
            printk("%s", info->speed == 0 ? "" : info->duplex ? "FD" : "HD");
        }
        if (flags & BCM_PORT_ATTR_PAUSE_MASK) {
	    if (info->pause_tx && info->pause_rx) {
		printk(",pause");
	    } else if (info->pause_tx) {
		printk(",pause_tx");
	    } else if (info->pause_rx) {
		printk(",pause_rx");
	    }
        }
	printk(") ");
    }

    if (flags & BCM_PORT_ATTR_AUTONEG_MASK) {
        if (info->autoneg) {
	    char	buf[80];

            if (flags & BCM_PORT_ATTR_ABILITY_MASK) {
                format_port_speed_ability(buf, sizeof (buf), local->speed_full_duplex);
                printk("Ability (fd = %s ", buf);
                format_port_speed_ability(buf, sizeof (buf), local->speed_half_duplex);
                printk("hd = %s ", buf);
                format_port_intf_ability(buf, sizeof (buf), local->interface);
                printk("intf = %s ", buf);
                format_port_medium_ability(buf, sizeof (buf), local->medium);
                printk("medium = %s ", buf);
                format_port_pause_ability(buf, sizeof (buf), local->pause);
                printk("pause = %s ", buf);
                format_port_lb_ability(buf, sizeof (buf), local->loopback);
                printk("lb = %s ", buf);
                format_port_flags_ability(buf, sizeof (buf), local->flags);
                printk("flags = %s )", buf);
            }

            if (flags & BCM_PORT_ATTR_LOCAL_ADVERT_MASK) {
                format_port_speed_ability(buf, sizeof (buf), advert->speed_full_duplex);
                printk("Local (fd = %s ", buf);
                format_port_speed_ability(buf, sizeof (buf), advert->speed_half_duplex);
                printk("hd = %s ", buf);
                format_port_intf_ability(buf, sizeof (buf), advert->interface);
                printk("intf = %s", buf);
                format_port_medium_ability(buf, sizeof (buf), advert->medium);
                printk("medium = %s ", buf);
                format_port_pause_ability(buf, sizeof (buf), advert->pause);
                printk("pause = %s ", buf);
                format_port_lb_ability(buf, sizeof (buf), advert->loopback);
                printk("lb = %s ", buf);
                format_port_flags_ability(buf, sizeof (buf), advert->flags);
                printk("flags = %s )", buf);
            }

            if ((flags & BCM_PORT_ATTR_REMOTE_ADVERT_MASK) &&
                info->remote_advert_valid && info->linkstatus) {
                format_port_speed_ability(buf, sizeof (buf), remote->speed_full_duplex);
                printk("Remote (fd = %s ", buf);
                format_port_speed_ability(buf, sizeof (buf), remote->speed_half_duplex);
                printk("hd = %s ", buf);
                format_port_intf_ability(buf, sizeof (buf), remote->interface);
                printk("intf = %s ", buf);
                format_port_medium_ability(buf, sizeof (buf), remote->medium);
                printk("medium = %s ", buf);
                format_port_pause_ability(buf, sizeof (buf), remote->pause);
                printk("pause = %s ", buf);
                format_port_lb_ability(buf, sizeof (buf), remote->loopback);
                printk("lb = %s ", buf);
                format_port_flags_ability(buf, sizeof (buf), remote->flags);
                printk("flags = %s )", buf);
            }
        }
    }

    if (flags & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
	if ((info->pause_mac[0] | info->pause_mac[1] |
	     info->pause_mac[2] | info->pause_mac[3] |
	     info->pause_mac[4] | info->pause_mac[5]) != 0) {
	    printk("Stad(%02x:%02x:%02x:%02x:%02x:%02x) ",
		   info->pause_mac[0], info->pause_mac[1],
		   info->pause_mac[2], info->pause_mac[3],
		   info->pause_mac[4], info->pause_mac[5]);
	}
    }

    if (flags & BCM_PORT_ATTR_STP_STATE_MASK) {
        printk("STP(%s) ", FORWARD_MODE(info->stp_state));
    }

    if (!st_port) {
        if (flags & BCM_PORT_ATTR_DISCARD_MASK) {
            switch (info->discard) {
            case BCM_PORT_DISCARD_NONE:
                break;
            case BCM_PORT_DISCARD_ALL:
                printk("Disc(all) ");
                break;
            case BCM_PORT_DISCARD_UNTAG:
                printk("Disc(untagged) ");
                break;
            case BCM_PORT_DISCARD_TAG:
                printk("Disc(tagged) ");
                break;
            default:
                printk("Disc(?) ");
                break;
            }
        }

        if (flags & BCM_PORT_ATTR_LEARN_MASK) {
            printk("Lrn(");

            r = 0;

            if (info->learn & BCM_PORT_LEARN_ARL) {
                printk("ARL");
                r = 1;
            }

            if (info->learn & BCM_PORT_LEARN_CPU) {
                printk("%sCPU", r ? "," : "");
                r = 1;
            }

            if (info->learn & BCM_PORT_LEARN_FWD) {
                printk("%sFWD", r ? "," : "");
                r = 1;
            }

            if (!r) {
                printk("disc");
            }
            printk(") ");
        }

        if (flags & BCM_PORT_ATTR_UNTAG_PRI_MASK) {
            printk("UtPri(");

            if (info->untagged_priority < 0) {
                printk("off");
            } else {
                printk("%d", info->untagged_priority);
            }
            printk(") ");
        }

        if (flags & BCM_PORT_ATTR_PFM_MASK) {
            printk("Pfm(%s) ",  MCAST_FLOOD(info->pfm));
        }
    } /* !st_port */
            
    if (flags & BCM_PORT_ATTR_INTERFACE_MASK) {
        if (info->interface >= 0 && info->interface < SOC_PORT_IF_COUNT) {
            printk("IF(%s) ", INTERFACE_MODE(info->interface));
        }
    }

    if (flags & BCM_PORT_ATTR_PHY_MASTER_MASK) {
        if (info->phy_master >= 0 &&
	    info->phy_master < SOC_PORT_MS_COUNT &&
	    info->phy_master != SOC_PORT_MS_NONE) {
            printk("PH(%s) ", PHYMASTER_MODE(info->phy_master));
        }
    }

    if (flags & BCM_PORT_ATTR_LOOPBACK_MASK) {
        if (info->loopback == BCM_PORT_LOOPBACK_PHY) {
            printk("LB(PHY) ");
        } else if (info->loopback == BCM_PORT_LOOPBACK_MAC) {
            printk("LB(MAC) ");
        }
    }

    if (flags & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        printk("Max_frame(%d) ", info->frame_max);
    }

    if ((flags & BCM_PORT_ATTR_MDIX_MASK) &&
        (0 <= info->mdix) && 
        (info->mdix < BCM_PORT_MDIX_COUNT)) {
        printk("MDIX(%s", MDIX_MODE(info->mdix));

        if ((flags & BCM_PORT_ATTR_MDIX_STATUS_MASK) &&
            (0 <= info->mdix_status) && 
            (info->mdix_status < BCM_PORT_MDIX_STATUS_COUNT)) {
            printk(", %s", MDIX_STATUS(info->mdix_status));
        }

        printk(") ");
    }

    if ((flags & BCM_PORT_ATTR_MEDIUM_MASK) &&
        (0 <= info->medium) && (info->medium < BCM_PORT_MEDIUM_COUNT)) {
        printk("Medium(%s) ", MEDIUM_STATUS(info->medium));
    }

    if ((flags & BCM_PORT_ATTR_FAULT_MASK) && (info->fault)) { 
        printk("Fault(%s%s) ", (info->fault & BCM_PORT_FAULT_LOCAL) ? "Local" : "", 
                               (info->fault & BCM_PORT_FAULT_REMOTE) ? "Remote" : "");
    }

    printk("\n");
}

/* This maps the above list to the masks for the proper attributes
 * Note that the order of this attribute map should match that of
 * the parse-table entry/creation below.
 */
static int port_attr_map[] = {
    BCM_PORT_ATTR_ENABLE_MASK,       /* Enable */
    BCM_PORT_ATTR_AUTONEG_MASK,      /* AutoNeg */
    BCM_PORT_ATTR_LOCAL_ADVERT_MASK, /* ADVert */
    BCM_PORT_ATTR_SPEED_MASK,        /* SPeed */
    BCM_PORT_ATTR_DUPLEX_MASK,       /* FullDuplex */
    BCM_PORT_ATTR_LINKSCAN_MASK,     /* LinkScan */
    BCM_PORT_ATTR_LEARN_MASK,        /* LeaRN */
    BCM_PORT_ATTR_DISCARD_MASK,      /* DISCard */
    BCM_PORT_ATTR_VLANFILTER_MASK,   /* VlanFilter */
    BCM_PORT_ATTR_UNTAG_PRI_MASK,    /* PRIOrity */
    BCM_PORT_ATTR_PFM_MASK,          /* PortFilterMode */
    BCM_PORT_ATTR_PHY_MASTER_MASK,   /* PHymaster */
    BCM_PORT_ATTR_INTERFACE_MASK,    /* InterFace */
    BCM_PORT_ATTR_LOOPBACK_MASK,     /* LoopBack */
    BCM_PORT_ATTR_STP_STATE_MASK,    /* SpanningTreeProtocol */
    BCM_PORT_ATTR_PAUSE_MAC_MASK,    /* STationADdress */
    BCM_PORT_ATTR_PAUSE_TX_MASK,     /* TxPAUse */
    BCM_PORT_ATTR_PAUSE_RX_MASK,     /* RxPAUse */
    BCM_PORT_ATTR_ENCAP_MASK,        /* Port encapsulation mode */
    BCM_PORT_ATTR_FRAME_MAX_MASK,    /* Max receive frame size */
    BCM_PORT_ATTR_MDIX_MASK,         /* MDIX mode */
    BCM_PORT_ATTR_MEDIUM_MASK,       /* port MEDIUM */
};

/*
 * Function:
 *	port_parse_setup
 * Purpose:
 *	Setup the parse table for a port command
 * Parameters:
 *	pt	- the table
 *	info	- port info structure to hold parse results
 * Returns:
 *	Nothing
 */

void
port_parse_setup(int unit, parse_table_t *pt, bcm_port_info_t *info)
{
    int i;

    /*
     * NOTE: ENTRIES IN THIS TABLE ARE POSITION-DEPENDENT!
     * See references to PQ_PARSED below.
     */
    parse_table_init(unit, pt);
    parse_table_add(pt, "Enable", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->enable, 0);
    parse_table_add(pt, "AutoNeg", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->autoneg, 0);
    parse_table_add(pt, "ADVert", PQ_PORTMODE | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->local_advert, 0);
    parse_table_add(pt, "SPeed",       PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->speed, 0);
    parse_table_add(pt, "FullDuplex",  PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->duplex, 0);
    parse_table_add(pt, "LinkScan",    PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->linkscan, linkscan_mode);
    parse_table_add(pt, "LeaRN",   PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->learn, 0);
    parse_table_add(pt, "DISCard", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->discard, discard_mode);
    parse_table_add(pt, "VlanFilter", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->vlanfilter, 0);
    parse_table_add(pt, "PRIOrity", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->untagged_priority, 0);
    parse_table_add(pt, "PortFilterMode", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->pfm, 0);
    parse_table_add(pt, "PHymaster", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->phy_master, phymaster_mode);
    parse_table_add(pt, "InterFace", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->interface, interface_mode);
    parse_table_add(pt, "LoopBack", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->loopback, loopback_mode);
    parse_table_add(pt, "SpanningTreeProtocol",
                    PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->stp_state, forward_mode);
    parse_table_add(pt, "STationADdress", PQ_MAC | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->pause_mac, 0);
    parse_table_add(pt, "TxPAUse",     PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->pause_tx, 0);
    parse_table_add(pt, "RxPAUse",     PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                    0, &info->pause_rx, 0);
    parse_table_add(pt, "ENCapsulation", PQ_MULTI | PQ_DFL,
                    0, &info->encap_mode, encap_mode);
    parse_table_add(pt, "FrameMax", PQ_INT | PQ_DFL,
                    0, &info->frame_max, 0);
    parse_table_add(pt, "MDIX", PQ_MULTI | PQ_DFL,
                    0, &info->mdix, mdix_mode);
    parse_table_add(pt, "Medium", PQ_MULTI | PQ_DFL,
                    0, &info->medium, medium_status);
    
    if (SOC_IS_XGS12_FABRIC(unit)) {
        /* For Hercules, ignore some StrataSwitch attributes */
        for (i = 0; i < pt->pt_cnt; i++) {
            if (~BCM_PORT_HERC_ATTRS & port_attr_map[i]) {
                pt->pt_entries[i].pq_type |= PQ_IGNORE;
            }
        }
    } else if (!SOC_IS_XGS(unit) && !SOC_IS_SIRIUS(unit)) {
	/* For all non-XGS chips, ignore special XGS attributes */
        for (i = 0; i < pt->pt_cnt; i++) {
            if (BCM_PORT_XGS_ATTRS & port_attr_map[i]) {
                pt->pt_entries[i].pq_type |= PQ_IGNORE;
            }
        }
    }
}


/*
 * Function:
 *	port_parse_mask_get
 * Purpose:
 *	Given PT has been parsed, set seen and parsed flags
 * Parameters:
 *	pt	- the table
 *	seen	- which parameters occurred w/o =
 *	parsed	- which parameters occurred w =
 * Returns:
 *	Nothing
 */

void
port_parse_mask_get(parse_table_t *pt, uint32 *seen, uint32 *parsed)
{
    uint32		were_parsed = 0;
    uint32		were_seen = 0;
    int			i;

    /* Check that either all parameters are parsed or are seen (no =) */

    for (i = 0; i < pt->pt_cnt; i++) {
        if (pt->pt_entries[i].pq_type & PQ_SEEN) {
            were_seen |= port_attr_map[i];
        }

        if (pt->pt_entries[i].pq_type & PQ_PARSED) {
            were_parsed |= port_attr_map[i];
        }
    }

    *seen = were_seen;
    *parsed = were_parsed;
}

/* Invalid unit number ( < 0) is permitted */
void
port_info_init(int unit, int port, bcm_port_info_t *info, uint32 actions)
{
    info->action_mask = actions;

    /* We generally need to get link state */
    info->action_mask |= BCM_PORT_ATTR_LINKSTAT_MASK;

    /* Add the autoneg and advert masks if any of actions is possibly 
     * related to the autoneg. 
     */ 
    if (actions & (BCM_PORT_AN_ATTRS | 
                   BCM_PORT_ATTR_AUTONEG_MASK |
                   BCM_PORT_ATTR_LOCAL_ADVERT_MASK)) {     
        info->action_mask |= BCM_PORT_ATTR_LOCAL_ADVERT_MASK;
        info->action_mask |= BCM_PORT_ATTR_REMOTE_ADVERT_MASK;
        info->action_mask |= BCM_PORT_ATTR_AUTONEG_MASK;
    } 

    if (unit >= 0) {
        
        if (unit >= 0 && SOC_IS_XGS12_FABRIC(unit)) {
            info->action_mask |= BCM_PORT_ATTR_ENCAP_MASK;
        }

        /* Clear rate for HG/HL ports */
        if (IS_ST_PORT(unit, port)) {
            info->action_mask &= ~(BCM_PORT_ATTR_RATE_MCAST_MASK |
                                   BCM_PORT_ATTR_RATE_BCAST_MASK |
                                   BCM_PORT_ATTR_RATE_DLFBC_MASK);
        }

        if (SOC_IS_SC_CQ(unit) || SOC_IS_TRIUMPH2(unit) || 
            SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) {
            info->action_mask2 |= BCM_PORT_ATTR2_PORT_ABILITY;
        }
    }
}

/* Given a maximum speed, return the mask of bcm_port_ability_t speeds
 * while are less than or equal to the given speed. */
bcm_port_abil_t
port_speed_max_mask(bcm_port_abil_t max_speed)
{
    bcm_port_abil_t speed_mask = 0;
    /* This is a giant fall through switch */
    switch (max_speed) {
        
    case 25000:
        speed_mask |= BCM_PORT_ABILITY_25GB;
    case 24000:
        speed_mask |= BCM_PORT_ABILITY_24GB;
    case 21000:
        speed_mask |= BCM_PORT_ABILITY_21GB;
    case 20000:
        speed_mask |= BCM_PORT_ABILITY_20GB;
    case 16000:
        speed_mask |= BCM_PORT_ABILITY_16GB;
    case 15000:
        speed_mask |= BCM_PORT_ABILITY_15GB;
    case 13000:
        speed_mask |= BCM_PORT_ABILITY_13GB;
    case 12500:
        speed_mask |= BCM_PORT_ABILITY_12P5GB;
    case 12000:
        speed_mask |= BCM_PORT_ABILITY_12GB;
    case 10000:
        speed_mask |= BCM_PORT_ABILITY_10GB;
    case 6000:
        speed_mask |= BCM_PORT_ABILITY_6000MB;
    case 5000:
        speed_mask |= BCM_PORT_ABILITY_5000MB;
    case 3000:
        speed_mask |= BCM_PORT_ABILITY_3000MB;
    case 2500:
        speed_mask |= BCM_PORT_ABILITY_2500MB;
    case 1000:
        speed_mask |= BCM_PORT_ABILITY_1000MB;
    case 100:
        speed_mask |= BCM_PORT_ABILITY_100MB;
    case 10:
        speed_mask |= BCM_PORT_ABILITY_10MB;
    default:
        break;
    }
    return speed_mask;
}

/*
 * Function:
 *	port_parse_port_info_set
 * Purpose:
 *	Set/change values in a destination according to parsing
 * Parameters:
 *	flags	- What values to change
 *	src	- Where to get info from
 *	dest	- Where to put info
 * Returns:
 *	-1 on error.  0 on success
 * Notes:
 *	The speed_max and abilities values must be
 *      set in the src port info structure before this is called.
 *
 *      Assumes linkstat and autoneg are valid in the dest structure
 *      If autoneg is specified in flags, assumes local advert
 *      is valid in the dest structure.
 */

int
port_parse_port_info_set(uint32 flags,
			 bcm_port_info_t *src,
                         bcm_port_info_t *dest)
{
    int info_speed_adj;

    if (flags & BCM_PORT_ATTR_AUTONEG_MASK) {
        dest->autoneg = src->autoneg;
    }

    if (flags & BCM_PORT_ATTR_ENABLE_MASK) {
        dest->enable = src->enable;
    }

    if (flags & BCM_PORT_ATTR_STP_STATE_MASK) {
        dest->stp_state = src->stp_state;
    }

    /*
     * info_speed_adj is the same as src->speed except a speed of 0
     * is replaced by the maximum speed supported by the port.
     */

    info_speed_adj = src->speed;

    if ((flags & BCM_PORT_ATTR_SPEED_MASK) && (info_speed_adj == 0)) {
        info_speed_adj = src->speed_max;
    }

    /*
     * If local_advert was parsed, use it.  Otherwise, calculate a
     * reasonable local advertisement from the given values and current
     * values of speed/duplex.
     */

    if ((flags & BCM_PORT_ATTR_LOCAL_ADVERT_MASK) != 0) {
        if (dest->action_mask2 & BCM_PORT_ATTR2_PORT_ABILITY) {
            /* Copy source advert info to destination, converting the 
             * format in the process */
            BCM_IF_ERROR_RETURN
                (soc_port_mode_to_ability(src->local_advert,
                                          &(dest->local_ability)));
        } else {
            dest->local_advert = src->local_advert;
        }
    } else if (dest->autoneg) {
        int                 cur_speed, cur_duplex;
        int                 cur_pause_tx, cur_pause_rx;
        int                 new_speed, new_duplex;
        int                 new_pause_tx, new_pause_rx;
        bcm_port_abil_t     mode;

        /*
         * Update link advertisements for speed/duplex/pause.  All
         * speeds less than or equal to the requested speed are
         * advertised.
         */

        if (dest->action_mask2 & BCM_PORT_ATTR2_PORT_ABILITY) {
            bcm_port_ability_t  *dab, *sab;

            dab = &(dest->local_ability);
            sab = &(src->port_ability);

            cur_speed =
                BCM_PORT_ABILITY_SPEED_MAX(dab->speed_full_duplex |
                                           dab->speed_half_duplex);
            cur_duplex = (dab->speed_full_duplex ?
                          SOC_PORT_DUPLEX_FULL : SOC_PORT_DUPLEX_HALF);
            cur_pause_tx = (dab->pause & BCM_PORT_ABILITY_PAUSE_TX) != 0;
            cur_pause_rx = (dab->pause & BCM_PORT_ABILITY_PAUSE_RX) != 0;

            new_speed = (flags & BCM_PORT_ATTR_SPEED_MASK ?
                         info_speed_adj : cur_speed);
            new_duplex = (flags & BCM_PORT_ATTR_DUPLEX_MASK ?
                          src->duplex : cur_duplex);
            new_pause_tx = (flags & BCM_PORT_ATTR_PAUSE_TX_MASK ?
                            src->pause_tx : cur_pause_tx);
            new_pause_rx = (flags & BCM_PORT_ATTR_PAUSE_RX_MASK ?
                            src->pause_rx : cur_pause_rx);

            if (new_duplex != SOC_PORT_DUPLEX_HALF) {
                mode = sab->speed_full_duplex;
                mode &= port_speed_max_mask(new_speed);
                dab->speed_full_duplex = mode;
            } else {
                dab->speed_full_duplex = 0;
            }
            
            mode = sab->speed_half_duplex;
            mode &= port_speed_max_mask(new_speed);
            dab->speed_half_duplex = mode;

            if (!(sab->pause &
                  BCM_PORT_ABILITY_PAUSE_ASYMM) &&
                (new_pause_tx != new_pause_rx)) {
                printk("port parse: Error: Asymmetrical pause not available\n");
                return -1;
            }

            if (!new_pause_tx) {
                dab->pause &= ~BCM_PORT_ABILITY_PAUSE_TX;
            }

            if (!new_pause_rx) {
                dab->pause &= ~BCM_PORT_ABILITY_PAUSE_RX;
            }
        } else {
            mode = dest->local_advert;

            cur_speed = BCM_PORT_ABIL_SPD_MAX(mode);
            cur_duplex = ((mode & BCM_PORT_ABIL_FD) ?
                          SOC_PORT_DUPLEX_FULL : SOC_PORT_DUPLEX_HALF);
            cur_pause_tx = (mode & BCM_PORT_ABIL_PAUSE_TX) != 0;
            cur_pause_rx = (mode & BCM_PORT_ABIL_PAUSE_RX) != 0;

            new_speed = (flags & BCM_PORT_ATTR_SPEED_MASK ?
                         info_speed_adj : cur_speed);
            new_duplex = (flags & BCM_PORT_ATTR_DUPLEX_MASK ?
                          src->duplex : cur_duplex);
            new_pause_tx = (flags & BCM_PORT_ATTR_PAUSE_TX_MASK ?
                            src->pause_tx : cur_pause_tx);
            new_pause_rx = (flags & BCM_PORT_ATTR_PAUSE_RX_MASK ?
                            src->pause_rx : cur_pause_rx);

            /* Start with maximum ability and cut down */

            mode = src->ability;

            if (new_duplex == SOC_PORT_DUPLEX_HALF) {
                mode &= ~BCM_PORT_ABIL_FD;
            }

            if (new_speed < 2500) {
                mode &= ~BCM_PORT_ABIL_2500MB;
            }

            if (new_speed < 1000) {
                mode &= ~BCM_PORT_ABIL_1000MB;
            }

            if (new_speed < 100) {
                mode &= ~BCM_PORT_ABIL_100MB;
            }

            if (!(mode & BCM_PORT_ABIL_PAUSE_ASYMM) &&
                (new_pause_tx != new_pause_rx)) {
                printk("port parse: Error: Asymmetrical pause not available\n");
                return -1;
            }

            if (!new_pause_tx) {
                mode &= ~BCM_PORT_ABIL_PAUSE_TX;
            }

            if (!new_pause_rx) {
                mode &= ~BCM_PORT_ABIL_PAUSE_RX;
            }

            dest->local_advert = mode;
        }
    } else {
        /* Update forced values for speed/duplex/pause */

        if (flags & BCM_PORT_ATTR_SPEED_MASK) {
            dest->speed = info_speed_adj;
        }

        if (flags & BCM_PORT_ATTR_DUPLEX_MASK) {
            dest->duplex = src->duplex;
        }

        if (flags & BCM_PORT_ATTR_PAUSE_TX_MASK) {
            dest->pause_tx = src->pause_tx;
        }

        if (flags & BCM_PORT_ATTR_PAUSE_RX_MASK) {
            dest->pause_rx = src->pause_rx;
        }
    }

    if (flags & BCM_PORT_ATTR_PAUSE_MAC_MASK) {
        sal_memcpy(dest->pause_mac, src->pause_mac, sizeof (mac_addr_t));
    }

    if (flags & BCM_PORT_ATTR_LINKSCAN_MASK) {
        dest->linkscan = src->linkscan;
    }

    if (flags & BCM_PORT_ATTR_LEARN_MASK) {
        dest->learn = src->learn;
    }

    if (flags & BCM_PORT_ATTR_DISCARD_MASK) {
        dest->discard = src->discard;
    }

    if (flags & BCM_PORT_ATTR_VLANFILTER_MASK) {
        dest->vlanfilter = src->vlanfilter;
    }

    if (flags & BCM_PORT_ATTR_UNTAG_PRI_MASK) {
        dest->untagged_priority = src->untagged_priority;
    }

    if (flags & BCM_PORT_ATTR_PFM_MASK) {
        dest->pfm = src->pfm;
    }

    if (flags & BCM_PORT_ATTR_PHY_MASTER_MASK) {
        dest->phy_master = src->phy_master;
    }

    if (flags & BCM_PORT_ATTR_INTERFACE_MASK) {
        dest->interface = src->interface;
    }

    if (flags & BCM_PORT_ATTR_LOOPBACK_MASK) {
        dest->loopback = src->loopback;
    }

    if (flags & BCM_PORT_ATTR_ENCAP_MASK) {
        dest->encap_mode = src->encap_mode;
    }

    if (flags & BCM_PORT_ATTR_FRAME_MAX_MASK) {
        dest->frame_max = src->frame_max;
    }

    if (flags & BCM_PORT_ATTR_MDIX_MASK) {
        dest->mdix = src->mdix;
    }

    return 0;
}

/* Iterate thru a port bitmap with the given mask; display info */
STATIC int
_port_disp_iter(int unit, pbmp_t pbm, pbmp_t pbm_mask, uint32 seen)
{
    bcm_port_info_t info;
    soc_port_t port, dport;
    int r;

    BCM_PBMP_AND(pbm, pbm_mask);
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {

        sal_memset(&info, 0, sizeof(bcm_port_info_t));
        port_info_init(unit, port, &info, seen);

        if ((r = bcm_port_selective_get(unit, port, &info)) < 0) {
            printk("Error: Could not get port %s information: %s\n",
                   BCM_PORT_NAME(unit, port), bcm_errmsg(r));
            return (CMD_FAIL);
        }

        disp_port_info(BCM_PORT_NAME(unit, port), &info, 
                       IS_ST_PORT(unit, port), seen);
    }

    return CMD_OK;
}

/*
 * Function:
 *	if_port
 * Purpose:
 *	Configure port specific parameters.
 * Parameters:
 *	u	- SOC unit #
 *	a	- pointer to args
 * Returns:
 *	CMD_OK/CMD_FAIL
 */
cmd_result_t
if_esw_port(int u, args_t *a)
{
    pbmp_t              pbm;
    bcm_port_config_t   pcfg;
    bcm_port_info_t    *info_all;
    bcm_port_info_t     info_given;
    bcm_port_ability_t  *ability_all;       /* Abilities for all ports */
    bcm_port_ability_t  ability_port;       /* Ability for current port */
    bcm_port_ability_t  ability_given;  
    char                *c;
    int                 r, rv = 0, cmd_rv = CMD_OK;
    soc_port_t          p, dport;
    parse_table_t       pt;
    uint32		seen = 0;
    uint32		parsed = 0;
    char		pfmt[SOC_PBMP_FMT_LEN];

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(u, &pcfg) != BCM_E_NONE) {
        printk("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return(CMD_USAGE);
    }

    info_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t), 
                         "if_port");

    if (info_all == NULL) {
        printk("Insufficient memory.\n");
        return CMD_FAIL;
    }

    ability_all = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(bcm_port_ability_t), 
        "if_port");

    if (ability_all == NULL) {
        printk("Insufficient memory.\n");
        sal_free(info_all);
        return CMD_FAIL;
    }

    sal_memset(&info_given, 0, sizeof(bcm_port_info_t));
    sal_memset(info_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_info_t));
    sal_memset(&ability_given, 0, sizeof(bcm_port_ability_t));
    sal_memset(&ability_port, 0, sizeof(bcm_port_ability_t));
    sal_memset(ability_all, 0, SOC_MAX_NUM_PORTS * sizeof(bcm_port_ability_t));

    if (parse_bcm_pbmp(u, c, &pbm) < 0) {
        printk("%s: Error: unrecognized port bitmap: %s\n",
               ARG_CMD(a), c);
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, pcfg.port);

    if (BCM_PBMP_IS_NULL(pbm)) {
	ARG_DISCARD(a);
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_OK;
    }

    if (ARG_CNT(a) == 0) {
        seen = BCM_PORT_ATTR_ALL_MASK;
    } else {
        /*
         * Otherwise, arguments are given.  Use them to determine which
         * properties need to be gotten/set.
	 *
         * Probe and detach, hidden commands.
         */
        if (!sal_strcasecmp(_ARG_CUR(a), "detach")) {
            pbmp_t detached;
            bcm_port_detach(u, pbm, &detached);
            printk("Detached port bitmap %s\n", SOC_PBMP_FMT(detached, pfmt));
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            return CMD_OK;
        } else if ((!sal_strcasecmp(_ARG_CUR(a), "probe")) ||
                   (!sal_strcasecmp(_ARG_CUR(a), "attach"))) {
            pbmp_t probed;
            bcm_port_probe(u, pbm, &probed);
            printk("Probed port bitmap %s\n", SOC_PBMP_FMT(probed, pfmt));
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            return CMD_OK;
        } else if (!sal_strcasecmp(_ARG_CUR(a), "lanes")) {
            int lanes;
            ARG_GET(a);
            if ((c = ARG_GET(a)) == NULL) {
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_USAGE;
            }
   	    lanes = sal_ctoi(c, 0);
            DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
                rv = bcm_port_control_set(u, p, bcmPortControlLanes, lanes);
                if (rv < 0) {
                    break;
                }
            }
            ARG_GET(a);
            sal_free(info_all);
            sal_free(ability_all);
            if (rv < 0) {
                return CMD_FAIL;
            } else {
                return CMD_OK;
            }
        }

        if (!sal_strcmp(_ARG_CUR(a), "=")) {
            /*
             * For "=" where the user is prompted to enter all the parameters,
             * use the parameters from the first selected port as the defaults.
             */
            if (ARG_CNT(a) != 1) {
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_USAGE;
            }
            DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
                break;    /* Find first port in bitmap */
            }
            if ((rv = bcm_port_info_get(u, p, &info_given)) < 0) {
                printk("%s: Error: Failed to get port info\n", ARG_CMD(a));
                sal_free(info_all);
                sal_free(ability_all);
                return CMD_FAIL;
            }
        }

        /*
         * Parse the arguments.  Determine which ones were actually given.
         */
        port_parse_setup(u, &pt, &info_given);
        
        if (parse_arg_eq(a, &pt) < 0) {
	    parse_arg_eq_done(&pt);
            sal_free(info_all);
            sal_free(ability_all);
            return(CMD_FAIL);
        }

        /* Translate port_info into port abilities. */

        if (ARG_CNT(a) > 0) {
            printk("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
	    parse_arg_eq_done(&pt);
            sal_free(info_all);
            sal_free(ability_all);
            return(CMD_FAIL);
        }

        /*
         * Find out what parameters specified.  Record values specified.
         */
        port_parse_mask_get(&pt, &seen, &parsed);
        parse_arg_eq_done(&pt);
    }

    if (seen && parsed) {
        soc_cm_print("%s: Cannot get and set "
		     "port properties in one command\n",
                     ARG_CMD(a));
        sal_free(info_all);
        sal_free(ability_all);
        return CMD_FAIL;
    } else if (seen) { /* Show selected information */
        printk("%s: Status (* indicates PHY link up)\n", ARG_CMD(a));
        /* Display the information by port type */
#define _call_pdi(u, p, mp, s) \
        if (_port_disp_iter(u, p, mp, s) != CMD_OK) { \
             sal_free(info_all); \
             sal_free(ability_all); \
             return CMD_FAIL; \
        }
        _call_pdi(u, pbm, pcfg.fe, seen);
        _call_pdi(u, pbm, pcfg.ge, seen);
        _call_pdi(u, pbm, pcfg.xe, seen);
        _call_pdi(u, pbm, pcfg.hg, seen);
        sal_free(info_all);
        sal_free(ability_all);
        return(CMD_OK);
    }

    /* Some set information was given */

    if (parsed & BCM_PORT_ATTR_LINKSCAN_MASK) {
        /* Map ON --> S/W, OFF--> None */
        if (info_given.linkscan > 2) {
            info_given.linkscan -= 3;
        }
    }

    /*
     * Retrieve all requested port information first, then later write
     * back all port information.  That prevents a problem with loopback
     * cables where setting one port's info throws another into autoneg
     * causing it to return info in flux (e.g. suddenly go half duplex).
     */

    DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
        port_info_init(u, p, &info_all[p], parsed);
        if ((r = bcm_port_selective_get(u, p, &info_all[p])) < 0) {
            printk("%s: Error: Could not get port %s information: %s\n",
                   ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
            sal_free(info_all);
            sal_free(ability_all);
            return (CMD_FAIL);
        }
    }

    /*
     * Loop through all the specified ports, changing whatever field
     * values were actually given.  This avoids copying unaffected
     * information from one port to another and prevents having to
     * re-parse the arguments once per port.
     */

    DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
        if ((rv = bcm_port_speed_max(u, p, &info_given.speed_max)) < 0) {
            printk("port parse: Error: Could not get port %s max speed: %s\n",
                   BCM_PORT_NAME(u, p), bcm_errmsg(rv));
            continue;
        }

        if ((rv = bcm_port_ability_local_get(u, p, &info_given.port_ability)) < 0) {
            printk("port parse: Error: Could not get port %s ability: %s\n",
                   BCM_PORT_NAME(u, p), bcm_errmsg(rv));
            continue;
        }
        if ((rv = soc_port_ability_to_mode(&info_given.port_ability,
                                           &info_given.ability)) < 0) {
            printk("port parse: Error: Could not transform port %s ability to mode: %s\n",
                   BCM_PORT_NAME(u, p), bcm_errmsg(rv));
            continue;
        }

        if ((r = port_parse_port_info_set(parsed,
                                          &info_given, &info_all[p])) < 0) {
            printk("%s: Error: Could not parse port %s info: %s\n",
                   ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
            cmd_rv = CMD_FAIL;
            continue;
        }

        /* If AN is on, do not set speed, duplex, pause */
        if (info_all[p].autoneg) {
            info_all[p].action_mask &= ~BCM_PORT_AN_ATTRS;
        }

        if ((r = bcm_port_selective_set(u, p, &info_all[p])) < 0) {
            printk("%s: Error: Could not set port %s information: %s\n",
                   ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
            cmd_rv = CMD_FAIL;
            continue;
        }
    }

    sal_free(info_all);
    sal_free(ability_all);

    return(cmd_rv);
}

char if_egress_usage[] =
    "Usages:\n\t"
    "  egress set [<Port=port#>] [<Modid=modid>] <PBmp=val>\n\t"
    "        - Set allowed egress bitmap for (modid,port) or all\n\t"
    "  egress show [<Port=port#>] [<Module=modid>]\n\t"
    "        - Show allowed egress bitmap for (modid,port)\n";

/*
 * Note:
 *
 * Since these port numbers are likely on different modules, we cannot
 * use PBMP qualifiers by this unit.
 */

cmd_result_t
if_egress(int unit, args_t *a)
{
    char 		*subcmd, *c;
    int                 port, arg_port = -1, min_port = 0,
                        max_port = 31;
    int		        modid, arg_modid = -1, mod_min = 0,
                        mod_max = SOC_MODID_MAX(unit);
    bcm_pbmp_t 		pbmp;
    int 		r;
    bcm_pbmp_t 	arg_pbmp;
    parse_table_t	pt;
    cmd_result_t	retCode;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(arg_pbmp);

    /* Egress show command */
    if (sal_strcasecmp(subcmd, "show") == 0) {

	if ((c = ARG_CUR(a)) != NULL) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_DFL|PQ_INT, 0, &arg_port, NULL);
            parse_table_add(&pt, "Modid",   PQ_DFL|PQ_INT,  0, &arg_modid,
                            NULL);
            if (!parseEndOk(a, &pt, &retCode)) {
                return retCode;
            }

            if (arg_modid >= 0) {
                mod_min = mod_max = arg_modid;
            }
            if (arg_port >= 0) {
                min_port = max_port = arg_port;
            }
	}

	for (modid = mod_min; modid <= mod_max; modid++) {
            for (port = min_port; port <= max_port; port++) {
                r = bcm_port_egress_get(unit, port, modid, &pbmp);
                if (r < 0) {
                    printk("%s: egress (modid=%d, port=%d) get failed: %s\n",
                           ARG_CMD(a), modid, port, bcm_errmsg(r));
                    return CMD_FAIL;
                }

                if (BCM_PBMP_NEQ(pbmp, PBMP_ALL(unit))) {
		    char	buf[FORMAT_PBMP_MAX];
                    format_bcm_pbmp(unit, buf, sizeof (buf), pbmp);
                    printk("Module %d, port %d:  Enabled egress ports %s\n",
                           modid, port, buf);
                }
            }
        }

	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "set") == 0) {
	parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL|PQ_INT, 0, &arg_port, NULL);
        parse_table_add(&pt, "Modid",   PQ_DFL|PQ_INT,  0, &arg_modid,  NULL);
	parse_table_add(&pt, "Pbmp", PQ_DFL|PQ_PBMP|PQ_BCM, 0, &arg_pbmp, NULL);
	if (!parseEndOk(a, &pt, &retCode)) {
	    return retCode;
        }

        SOC_PBMP_ASSIGN(pbmp, arg_pbmp);

	r = bcm_port_egress_set(unit, arg_port, arg_modid, pbmp);

    } else {
	return CMD_USAGE;
    }

    if (r < 0) {
	printk("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
	return CMD_FAIL;
    }

    return CMD_OK;
}

cmd_result_t
if_esw_dscp(int unit, args_t *a)
{
    int         rv, port, dport, srccp, mapcp, prio, cng, mode, count, i;
    bcm_port_config_t pcfg;
    bcm_pbmp_t	pbm;
    bcm_pbmp_t  tpbm;
    int         use_global = 0;
    char	*s;
    parse_table_t pt;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        printk("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((s = ARG_GET(a)) == NULL) {
	printk("%s: ERROR: missing port bitmap\n", ARG_CMD(a));
	return CMD_FAIL;
    }
    if (parse_bcm_pbmp(unit, s, &pbm) < 0) {
	printk("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), s);
	return CMD_FAIL;
    }

    BCM_PBMP_ASSIGN(tpbm, pbm);
    BCM_PBMP_XOR(tpbm, pcfg.e);
    if (BCM_PBMP_IS_NULL(tpbm)) {
        /* global table */
        use_global = 1;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Mode", PQ_INT, (void *)-1, &mode, NULL);
    rv = parse_arg_eq(a, &pt);
    parse_arg_eq_done(&pt);
    if (BCM_FAILURE(rv)) {
        return CMD_FAIL;
    }

    srccp = -1;
    s = ARG_GET(a);
    if (s) {
        srccp = parse_integer(s);
    }

    if ((s = ARG_GET(a)) == NULL) {
        if (mode != -1) {
            printk("%s: WARNING: ignore mode argument\n", ARG_CMD(a));
        }
        if (srccp < 0) {
            srccp = 0;
            count = 64;
        } else {
            count = 1;
        }
        if (BCM_PBMP_IS_NULL(pbm) && 
            !soc_feature(unit, soc_feature_dscp_map_per_port) ) {
            port = -1;
            rv = bcm_port_dscp_map_get(unit, port, 0, &mapcp, &prio);
            if (rv == BCM_E_PORT) {
                port = -1;
                printk("%d: dscp map:\n", unit);
            }
            for (i = 0; i < count; i++) {
                rv = bcm_port_dscp_map_get(unit, port, srccp + i, &mapcp,
                                           &prio);
                if (rv < 0) {
                    printk("ERROR: dscp map get %d failed: %s\n",
                           srccp + i, bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                if (srccp + i != mapcp || count == 1) {
                    printk(" %d->%d prio=%d cng=%d\n",
                           srccp + i, mapcp,
                           prio & BCM_PRIO_MASK,
                           (prio & BCM_PRIO_DROP_FIRST) ? 1 : 0);

                }
            }
            printk("\n");
        } else {
        DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
            if (use_global) {
                rv = bcm_port_dscp_map_get(unit, port, 0, &mapcp, &prio);
                if (rv == BCM_E_PORT) {
                    port = -1;
                    printk("%d: dscp map:\n", unit);
                }
            } else {
                printk("%d:%s dscp map:\n", unit, BCM_PORT_NAME(unit, port));
            }
	    for (i = 0; i < count; i++) {
		rv = bcm_port_dscp_map_get(unit, port, srccp + i, &mapcp,
                                           &prio);
		if (rv < 0) {
		    printk("ERROR: dscp map get %d failed: %s\n",
			   srccp + i, bcm_errmsg(rv));
		    return CMD_FAIL;
		}
		if (srccp + i != mapcp || count == 1) {
		    printk(" %d->%d prio=%d cng=%d\n",
			   srccp + i, mapcp,
			   prio & BCM_PRIO_MASK,
			   (prio & BCM_PRIO_DROP_FIRST) ? 1 : 0);

                    }
                }
	    printk("\n");
            if (port == -1) {
                break;
            }
	}
        }
	return CMD_OK;
    }

    mapcp = parse_integer(s);
    prio = -1;
    cng = 0;

    if ((s = ARG_GET(a)) != NULL) {
	prio = parse_integer(s);
	if ((s = ARG_GET(a)) != NULL) {
	    cng = parse_integer(s);
	}
    }
    if (cng) prio |= BCM_PRIO_DROP_FIRST;

    /* Allow empty pbmp to configure devices that don't support per port */
    /*  dscp mapping */
    if (BCM_PBMP_IS_NULL(pbm) && 
        !soc_feature(unit, soc_feature_dscp_map_per_port) ) {
        port = -1;

        if (mode != -1) {
            rv = bcm_port_dscp_map_mode_set(unit, port, mode);
            if (rv < 0) {
                printk("%d:%s ERROR: dscp mode set mode=%d: %s\n",
                       unit, (port == -1) ? "" : BCM_PORT_NAME(unit, port),
                       mode, bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }

        rv = bcm_port_dscp_map_set(unit, port, srccp, mapcp, prio);
        if (rv < 0) {
            printk("%d:%s ERROR: "
               "dscp map set %d->%d prio=%d cng=%d failed: %s\n",
               unit, (port == -1) ? "" : BCM_PORT_NAME(unit, port),
               srccp, mapcp, prio, cng, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    } else {
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
        if (use_global) {
            port = -1;
        }

        if (mode != -1) {
            rv = bcm_port_dscp_map_mode_set(unit, port, mode);
            if (rv < 0) {
                printk("%d:%s ERROR: dscp mode set mode=%d: %s\n",
                       unit, (port == -1) ? "" : BCM_PORT_NAME(unit, port),
                       mode, bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }

	rv = bcm_port_dscp_map_set(unit, port, srccp, mapcp, prio);
	if (rv < 0) {
	    printk("%d:%s ERROR: "
		   "dscp map set %d->%d prio=%d cng=%d failed: %s\n",
		   unit, (port == -1) ? "" : BCM_PORT_NAME(unit, port),
		   srccp, mapcp, prio, cng, bcm_errmsg(rv));
	    return CMD_FAIL;
	}
        if (port == -1) {
            break;
        }
    }
    }
    return CMD_OK;
}

cmd_result_t
if_esw_ipg(int unit, args_t *a)
/*
 * Function: 	if_ipg
 * Purpose:	Configure default IPG values.
 * Parameters:	unit - SOC unit #
 *		a - arguments
 * Returns:	CMD_OK/CMD_FAIL
 */
{
    parse_table_t      pt;
    pbmp_t             arg_pbmp;
    int                arg_speed, speed;
    bcm_port_duplex_t  arg_duplex, duplex;
    int                arg_gap;
    cmd_result_t       retCode;
    int                real_ifg;
    int                rv;
    int                i;
    bcm_port_config_t  pcfg;
    bcm_port_t         port, dport;

    const char *header = "        "
                         "    10HD"
                         "    10FD"
                         "   100HD"
                         "   100FD"
                         "  1000HD"
                         "  1000FD"
                         "  2500HD"
                         "  2500FD"
                         " 10000FD";

    const int speeds[] = {10, 100, 1000, 2500, 10000};
    const int num_speeds = sizeof(speeds) / sizeof(int);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        printk("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    /*
     * Assign the defaults
     */
    BCM_PBMP_ASSIGN(arg_pbmp, pcfg.port);
    arg_speed  = 0;
    arg_duplex = BCM_PORT_DUPLEX_COUNT; 
    arg_gap    = 0;

    /*
     * Parse the arguments
     */
    if (ARG_CNT(a)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL | PQ_PBMP | PQ_BCM,
                        0, &arg_pbmp, NULL);
        parse_table_add(&pt, "SPeed", PQ_DFL | PQ_INT,
                        0, &arg_speed, NULL);
        parse_table_add(&pt, "FullDuplex", PQ_DFL | PQ_BOOL,
                        0, &arg_duplex, NULL);
        parse_table_add(&pt, "Gap", PQ_DFL | PQ_INT,
                        0, &arg_gap, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }
    }

    printk("%s\n", header);
    /*
     * Display IPG settings for all the specified ports
     */
    DPORT_BCM_PBMP_ITER(unit, arg_pbmp, dport, port) {
        printk("%-8.8s", BCM_PORT_NAME(unit, port));
        for (i = 0; i < num_speeds; i++) {
            speed = speeds[i];
            
            for (duplex = BCM_PORT_DUPLEX_HALF; 
                 duplex < BCM_PORT_DUPLEX_COUNT;
                 duplex++) {
                /*
                 * Skip the illegal 10000HD combination
                 */
                if (speed == 10000 && duplex == BCM_PORT_DUPLEX_HALF) {
                    continue;
                }
                
                /*
                 * Skip an entry if the speed has been explicitly specified
                 */
                if (arg_speed != 0 && speed != arg_speed) {
                    printk("%8.8s", " ");
                    continue;
                }
            
                /*
                 * Skip an entry if duplex has been explicitly specified
                 * and the entry doesn't match
                 */
                if (arg_duplex != BCM_PORT_DUPLEX_COUNT &&
                    arg_duplex != duplex) {
                    printk("%8.8s", " ");
                    continue;
                }
                
                if (arg_gap != 0) {
                    rv = bcm_port_ifg_set(unit, port, speed, duplex, arg_gap);
                }
                
                rv = bcm_port_ifg_get(unit, port, speed, duplex, &real_ifg);
                
                if (rv == BCM_E_NONE) {
                    printk("%8d", real_ifg);
                } else {
                    printk("%8.8s", "n/a");
                }
            }
        }
        printk("\n");
    }

    return(CMD_OK);
}

cmd_result_t
if_esw_dtag(int unit, args_t *a)
{
    char		*subcmd, *c;
    bcm_port_config_t   pcfg;
    bcm_pbmp_t		pbmp;
    bcm_port_t		port, dport;
    int			mode, r;
    uint16		tpid;
    int                 dt_mode_mask;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        printk("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    dt_mode_mask = BCM_PORT_DTAG_MODE_INTERNAL |
                   BCM_PORT_DTAG_MODE_EXTERNAL;

    if ((subcmd = ARG_GET(a)) == NULL) {
	subcmd = "show";
    }

    c = ARG_GET(a);
    if (c == NULL) {
	BCM_PBMP_ASSIGN(pbmp, pcfg.e);
    } else {
	if (parse_bcm_pbmp(unit, c, &pbmp) < 0) {
	    printk("%s: ERROR: unrecognized port bitmap: %s\n",
		   ARG_CMD(a), c);
	    return CMD_FAIL;
	}
    }

    r = 0;
    if (sal_strcasecmp(subcmd, "show") == 0) {
        char *mode_flag; 

        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
	    r = bcm_port_dtag_mode_get(unit, port, &mode);
	    if (r < 0) {
		goto bcm_err;
	    }
	    r = bcm_port_tpid_get(unit, port, &tpid);
	    if (r < 0) {
		goto bcm_err;
	    }
	    switch (mode & dt_mode_mask) {
	    case BCM_PORT_DTAG_MODE_NONE:
		c = "none (disabled)";
		break;
	    case BCM_PORT_DTAG_MODE_INTERNAL:
		c = "internal (service provider)";
		break;
	    case BCM_PORT_DTAG_MODE_EXTERNAL:
		c = "external (customer)";
		break;
	    default:
		c = "unknown";
		break;
	    }
 
            mode_flag = "";
            if (mode & BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG) {
                mode_flag = " remove customer tag";
            } else if (mode & BCM_PORT_DTAG_ADD_EXTERNAL_TAG) {
                mode_flag = " add customer tag";
            }

	    printk("port %d:%s\tdouble tag mode %s%s, tpid 0x%x\n",
		   unit, BCM_PORT_NAME(unit, port), c, mode_flag, tpid);
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "mode") == 0) {
	c = ARG_GET(a);
	if (c == NULL) {
            char *mode_flag;
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {

		r = bcm_port_dtag_mode_get(unit, port, &mode);
		if (r < 0) {
		    goto bcm_err;
		}
		switch (mode & dt_mode_mask) {
		case BCM_PORT_DTAG_MODE_NONE:
		    c = "none (disabled)";
		    break;
		case BCM_PORT_DTAG_MODE_INTERNAL:
		    c = "internal (service provider)";
		    break;
		case BCM_PORT_DTAG_MODE_EXTERNAL:
		    c = "external (customer)";
		    break;
		default:
		    c = "unknown";
		    break;
		}
              
                mode_flag = "";
                if (mode & BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG) {
                    mode_flag = " remove customer tag";
                } else if (mode & BCM_PORT_DTAG_ADD_EXTERNAL_TAG) {
                    mode_flag = " add customer tag";
                }

		printk("port %d:%s\tdouble tag mode %s%s\n",
		       unit, BCM_PORT_NAME(unit, port), c, mode_flag);
	    }
	    return CMD_OK;
	}
	if (sal_strcasecmp(c, "none") == 0) {
	    mode = BCM_PORT_DTAG_MODE_NONE;
	} else if (sal_strcasecmp(c, "internal") == 0) {
	    mode = BCM_PORT_DTAG_MODE_INTERNAL;
	} else if (sal_strcasecmp(c, "external") == 0) {
	    mode = BCM_PORT_DTAG_MODE_EXTERNAL;
	} else {
	    return CMD_USAGE;
	}
        c = ARG_GET(a);
        if (c != NULL) {
            if (sal_strcasecmp(c, "addInnerTag") == 0) {
                mode |= BCM_PORT_DTAG_ADD_EXTERNAL_TAG;
            } else if (sal_strcasecmp(c, "removeInnerTag") == 0) {
                mode |= BCM_PORT_DTAG_REMOVE_EXTERNAL_TAG;
            } else {
                return CMD_OK;
            }
        }
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
	    r = bcm_port_dtag_mode_set(unit, port, mode);
	    if (r < 0) {
		goto bcm_err;
	    }
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "tpid") == 0) {
	c = ARG_GET(a);
	if (c == NULL) {
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		r = bcm_port_tpid_get(unit, port, &tpid);
		if (r < 0) {
		    goto bcm_err;
		}
		printk("port %d:%s\ttpid 0x%x\n",
		       unit, BCM_PORT_NAME(unit, port), tpid);
	    }
	} else {
	    tpid = parse_integer(c);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		r = bcm_port_tpid_set(unit, port, tpid);
		if (r < 0) {
		    goto bcm_err;
		}
	    }
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "addTpid") == 0) {
	c = ARG_GET(a);
	if (c == NULL) {
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		r = bcm_port_tpid_get(unit, port, &tpid);
		if (r < 0) {
		    goto bcm_err;
		}
		printk("port %d:%s\ttpid 0x%x\n",
		       unit, BCM_PORT_NAME(unit, port), tpid);
	    }
	} else {
	    tpid = parse_integer(c);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		r = bcm_port_tpid_add(unit, port, tpid, BCM_COLOR_PRIORITY);
		if (r < 0) {
		    goto bcm_err;
		}
	    }
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "deleteTpid") == 0) {
	c = ARG_GET(a);
	if (c == NULL) {
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		r = bcm_port_tpid_get(unit, port, &tpid);
		if (r < 0) {
		    goto bcm_err;
		}
		printk("port %d:%s\ttpid 0x%x\n",
		       unit, BCM_PORT_NAME(unit, port), tpid);
	    }
	} else {
            if (sal_strcasecmp(c, "all") == 0) {
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                    r = bcm_port_tpid_delete_all(unit, port);
                    if (r < 0) {
                        goto bcm_err;
                    } 
                } 
            } else {
	        tpid = parse_integer(c);
                DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
		    r = bcm_port_tpid_delete(unit, port, tpid);
		    if (r < 0) {
		        goto bcm_err;
		    }
                }
	    }
	}
	return CMD_OK;
    }

    return CMD_USAGE;

 bcm_err:
    printk("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));
    return CMD_FAIL;
}


cmd_result_t
if_esw_linkscan(int unit, args_t *a)
{
    parse_table_t	pt;
    char		*c;
    int			us, rv;
    pbmp_t		pbm_sw, pbm_hw, pbm_none, pbm_force;
    pbmp_t		pbm_temp;
    bcm_port_config_t   pcfg;
    soc_port_t		port, dport;
    char		pfmt[SOC_PBMP_FMT_LEN];

    
    /*
     * Workaround that allows "linkscan off" at the beginning of rc.soc
     */

    if (ARG_CNT(a) == 1 && sal_strcasecmp(_ARG_CUR(a), "off") == 0) {
	rv = bcm_init_check(unit);
	if (rv == BCM_E_UNIT || !(rv & BCM_INIT_SEL_LINKSCAN)) {
	    (void) ARG_GET(a);
	    return(CMD_OK);
	}
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        printk("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    /*
     * First get current linkscan state.  (us == 0 if disabled).
     */

    if ((rv = bcm_linkscan_enable_get(unit, &us)) < 0) {
	printk("%s: Error: Failed to recover enable status: %s\n",
	       ARG_CMD(a), bcm_errmsg(rv));
	return(CMD_FAIL);
    }

    BCM_PBMP_CLEAR(pbm_sw);
    BCM_PBMP_CLEAR(pbm_hw);
    BCM_PBMP_CLEAR(pbm_force);

    DPORT_BCM_PBMP_ITER(unit, pcfg.port, dport, port) {
	int		mode;

	if ((rv = bcm_linkscan_mode_get(unit, port, &mode)) < 0) {
	    printk("%s: Error: Could not get linkscan state for port %s\n",
		   ARG_CMD(a), BCM_PORT_NAME(unit, port));
	} else {
	    switch (mode) {
	    case BCM_LINKSCAN_MODE_SW:
		BCM_PBMP_PORT_ADD(pbm_sw, port);
		break;
	    case BCM_LINKSCAN_MODE_HW:
		BCM_PBMP_PORT_ADD(pbm_hw, port);
		break;
	    default:
		break;
	    }
	}
    }

    /*
     * If there are no arguments, just display the status.
     */

    if (ARG_CNT(a) == 0) {
	char		buf[FORMAT_PBMP_MAX];
	pbmp_t		pbm;

	if (us) {
	    printk("%s: Linkscan enabled\n", ARG_CMD(a));
	    printk("%s:   Software polling interval: %d usec\n",
		   ARG_CMD(a), us);
	    format_bcm_pbmp(unit, buf, sizeof (buf), pbm_sw);
	    printk("%s:   Software Port BitMap %s: %s\n",
		   ARG_CMD(a), SOC_PBMP_FMT(pbm_sw, pfmt), buf);
	    format_bcm_pbmp(unit, buf, sizeof (buf), pbm_hw);
	    printk("%s:   Hardware Port BitMap %s: %s\n",
		   ARG_CMD(a), SOC_PBMP_FMT(pbm_hw, pfmt), buf);
	    BCM_PBMP_ASSIGN(pbm_temp, pbm_sw);
	    BCM_PBMP_OR(pbm_temp, pbm_hw);
	    BCM_PBMP_ASSIGN(pbm, pcfg.port);
	    BCM_PBMP_XOR(pbm, pbm_temp);
	    format_bcm_pbmp(unit, buf, sizeof (buf), pbm);
	    printk("%s:   Disabled Port BitMap %s: %s\n",
		   ARG_CMD(a), SOC_PBMP_FMT(pbm, pfmt), buf);
	} else {
	    printk("%s: Linkscan disabled\n", ARG_CMD(a));
	}

	return(CMD_OK);
    }

    us = soc_property_get(unit, spn_BCM_LINKSCAN_INTERVAL, 250000);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "SwPortBitMap", PQ_PBMP|PQ_DFL|PQ_BCM, 0, &pbm_sw, 0);
    parse_table_add(&pt, "HwPortBitMap", PQ_PBMP|PQ_DFL|PQ_BCM, 0, &pbm_hw, 0);
    parse_table_add(&pt, "Force", PQ_PBMP|PQ_DFL|PQ_BCM, 0, &pbm_force, 0);
    parse_table_add(&pt, "Interval", PQ_INT|PQ_DFL, 0, &us, 0);

    if (parse_arg_eq(a, &pt) < 0) {
	printk("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
	parse_arg_eq_done(&pt);
	return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    /*
     * Handle backward compatibility, allowing a raw interval to be
     * specified directly on the command line, as well as "on" or "off".
     */

    if (ARG_CUR(a) != NULL) {
	c = ARG_GET(a);

	if (!sal_strcasecmp(c, "off") ||
	    !sal_strcasecmp(c, "disable") ||
	    !sal_strcasecmp(c, "no")) {
	    us = 0;
	} else if (!sal_strcasecmp(c, "on") ||
		   !sal_strcasecmp(c, "enable") ||
		   !sal_strcasecmp(c, "yes")) {
	    us = soc_property_get(unit, spn_BCM_LINKSCAN_INTERVAL, 250000);
	} else if (isint(c)) {
	    us = parse_integer(c);
	} else {
	    return(CMD_USAGE);
	}
    }

    if (us == 0) {
	/* Turn off linkscan */

	if ((rv = bcm_linkscan_enable_set(unit, 0)) < 0) {
	    printk("%s: Error: Failed to disable linkscan: %s\n",
		   ARG_CMD(a), bcm_errmsg(rv));
	    return(CMD_FAIL);
	}

	return(CMD_OK);
    }

    BCM_PBMP_AND(pbm_sw, pcfg.port);
    BCM_PBMP_AND(pbm_hw, pcfg.port);
    BCM_PBMP_ASSIGN(pbm_none, pcfg.port);
    BCM_PBMP_ASSIGN(pbm_temp, pbm_sw);
    BCM_PBMP_OR(pbm_temp, pbm_hw);
    BCM_PBMP_XOR(pbm_none, pbm_temp);
    BCM_PBMP_AND(pbm_force, pcfg.port);

    BCM_PBMP_ASSIGN(pbm_temp, pbm_sw);
    BCM_PBMP_AND(pbm_temp, pbm_hw);
    if (BCM_PBMP_NOT_NULL(pbm_temp)) {
	printk("%s: Error: Same port can't use both "
	       "software and hardware linkscan\n",
	       ARG_CMD(a));
	return(CMD_FAIL);
    }

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_sw,
					BCM_LINKSCAN_MODE_SW)) < 0) {
	printk("%s: Failed to set software link scanning: PBM=%s: %s\n",
	       ARG_CMD(a), SOC_PBMP_FMT(pbm_sw, pfmt), bcm_errmsg(rv));
	return(CMD_FAIL);
    }

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_hw,
					BCM_LINKSCAN_MODE_HW)) < 0) {
	printk("%s: Failed to set hardware link scanning: PBM=%s: %s\n",
	       ARG_CMD(a), SOC_PBMP_FMT(pbm_hw, pfmt), bcm_errmsg(rv));
	return(CMD_FAIL);
    }

    if ((rv = bcm_linkscan_mode_set_pbm(unit, pbm_none,
					BCM_LINKSCAN_MODE_NONE)) < 0) {
	printk("%s: Failed to disable link scanning: PBM=%s: %s\n",
	       ARG_CMD(a), SOC_PBMP_FMT(pbm_none, pfmt), bcm_errmsg(rv));
	return(CMD_FAIL);
    }

    if ((rv = bcm_linkscan_enable_set(unit, us)) < 0) {
	printk("%s: Error: Failed to enable linkscan: %s\n",
	       ARG_CMD(a), bcm_errmsg(rv));
	return(CMD_FAIL);
    }

    if ((rv = bcm_link_change(unit, pbm_force)) < 0) {
	printk("%s: Failed to force link scan: PBM=%s: %s\n",
	       ARG_CMD(a), SOC_PBMP_FMT(pbm_force, pfmt), bcm_errmsg(rv));
	return(CMD_FAIL);
    }

    return(CMD_OK);
}

#define DUMP_PHY_COLS	4
#define PHY_UPDATE(_flags, _control) ((_flags) & (1 << (_control))) 

cmd_result_t
port_phy_control_update(int u, bcm_port_t p, bcm_port_phy_control_t type,
                        uint32 val, uint32 flags, int *print_header)
{
    int    rv;
    uint32 oval; 

    oval = 0;
    rv = bcm_port_phy_control_get(u, p, type, &oval);
    if (BCM_FAILURE(rv) && BCM_E_UNAVAIL != rv) {
        printk("%s\n", bcm_errmsg(rv));
        return CMD_FAIL;
    } else if (BCM_SUCCESS(rv)) {
        if ((val != oval) && PHY_UPDATE(flags, type)) {
            if (BCM_FAILURE(bcm_port_phy_control_set(u, p, type, val))) {
                printk("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            oval = val;
        }
        if (*print_header) { 
            printk("Current PHY control settings of %s ->\n", 
                    BCM_PORT_NAME(u, p)); 
            *print_header = FALSE;
        }
        printk("%s = 0x%0x\n", phy_control[type], oval);
    }
    return CMD_OK;
}

/* definition for phy low power control command */

typedef struct {
    bcm_pbmp_t pbm;
    int registered;
} phy_power_ctrl_t; 

static phy_power_ctrl_t phy_pctrl_desc[SOC_MAX_NUM_DEVICES];

STATIC void
_phy_power_linkscan_cb (int unit, soc_port_t port, bcm_port_info_t *info)
{
    int found = FALSE;
    soc_port_t p, dport;
    phy_power_ctrl_t *pDesc;
    bcm_port_cable_diag_t cds;
    int rv;

    pDesc = &phy_pctrl_desc[unit];

    DPORT_BCM_PBMP_ITER(unit, pDesc->pbm, dport, p) {
        if (p == port) {
            found = TRUE;
            break;
        }
    }
   
    if (found == TRUE) {
        if (info->linkstatus) {
            /* link down->up transition */

            /* Check if giga link */ 
            if (info->speed != 1000) {
                return;
            }

            /* Run cable diag if giga link*/
            sal_memset(&cds, 0, sizeof(bcm_port_cable_diag_t));
            rv = bcm_port_cable_diag(unit, p, &cds);
            if (SOC_FAILURE(rv)) {
                return;
            }

            if (cds.pair_len[0] >= 0 && cds.pair_len[0] < 10) {
                /* Enable low power mode */
                (void)bcm_port_phy_control_set(unit,p,
                                           BCM_PORT_PHY_CONTROL_POWER,
                                           BCM_PORT_PHY_CONTROL_POWER_LOW);
            }
        } else {
            /* link up->down transition */
            /* disable low-power mode */
            (void)bcm_port_phy_control_set(unit,p,
                                           BCM_PORT_PHY_CONTROL_POWER,
                                           BCM_PORT_PHY_CONTROL_POWER_FULL);

        } 
    }
}

STATIC int
_phy_auto_low_start (int unit,bcm_pbmp_t pbm,int enable)
{
    soc_port_t p, dport;
    phy_power_ctrl_t *pDesc;

    pDesc = &phy_pctrl_desc[unit];

    if (enable) {
        if (!pDesc->registered) {
            pDesc->pbm = pbm;
            (void)bcm_linkscan_register(unit, _phy_power_linkscan_cb);
            pDesc->registered = TRUE;
        }   
    } else {
        if (pDesc->registered) {
            (void)bcm_linkscan_unregister(unit, _phy_power_linkscan_cb);
            pDesc->registered = FALSE;
            DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
                (void)bcm_port_phy_control_set(unit,p,
                                          BCM_PORT_PHY_CONTROL_POWER,
                                          BCM_PORT_PHY_CONTROL_POWER_FULL);
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function: 	if_phy
 * Purpose:	Show/configure phy registers.
 * Parameters:	u - SOC unit #
 *		a - pointer to args
 * Returns:	CMD_OK/CMD_FAIL/
 */
cmd_result_t
if_esw_phy(int u, args_t *a)
{
    soc_pbmp_t pbm, pbm_phys, pbm_temp;
    soc_port_t p, dport;
    char *c, drv_name[64];
    uint16 phy_data, phy_reg, phy_devad = 0;
    uint8 phy_addr;
    int intermediate = 0;
    int is_c45 = 0;
    int rv = CMD_OK;
    char pfmt[SOC_PBMP_FMT_LEN];
    int i;
    int p_devad[] = {PHY_C45_DEV_PMA_PMD,
                     PHY_C45_DEV_WIS,
                     PHY_C45_DEV_PCS,
                     PHY_C45_DEV_PHYXS,
                     PHY_C45_DEV_DTEXS,
                     PHY_C45_DEV_AN};
    char *p_devad_str[] = {"DEV_PMA_PMD",
                           "DEV_WIS",
                           "DEV_PCS",
                           "DEV_PHYXS",
                           "DEV_DTEXS",
                           "DEV_AN"};

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    c = ARG_GET(a);

    if (c != NULL && sal_strcasecmp(c, "info") == 0) {
        SOC_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(u));
        printk("Phy mapping dump:\n");
        printk("%10s %5s %5s %5s %5s %16s %10s\n",
               "port", "id0", "id1", "addr", "iaddr", "name", "timeout");
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            printk("%5s(%3d) %5x %5x %5x %5x %16s %10d\n",
                   SOC_PORT_NAME(u, p), p,
                   soc_phy_id0reg_get(u, p),
                   soc_phy_id1reg_get(u, p),
                   soc_phy_addr_of_port(u, p),
                   soc_phy_addr_int_of_port(u, p),
                   soc_phy_name_get(u, p),
                   soc_phy_an_timeout_get(u, p));
        }

        return CMD_OK;
    }

    if (c != NULL && sal_strcasecmp(c, "firmware") == 0) {
#ifdef  NO_FILEIO
        printk("This command is not supported without file I/O\n");
        return CMD_FAIL;
#else
        parse_table_t    pt;
        int count;
        uint32       flags;
        FILE      *fp = NULL;
        uint8 *buf;
        int len;
        int offset;
        char    input[32];
        #define FIRMWARE_BUF_LEN   0x80000

        char *filename = NULL;

        if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
            printk("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
            return CMD_FAIL;
        }

        SOC_PBMP_COUNT(pbm, count);

        if (count > 1) { 
            printk("ERROR: too many ports specified : %d\n", count);
            return CMD_FAIL;
        }
        flags = 0;
        if ((c = ARG_CUR(a)) != NULL) {

            if (c[0] == '=') {
                return CMD_USAGE;        /* '=' unsupported */
            }

            parse_table_init(u, &pt);
            parse_table_add(&pt, "set", PQ_DFL | PQ_STRING,0,
                    &filename, NULL);

            if (parse_arg_eq(a, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if (ARG_CNT(a) > 0) {
                printk("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
        }
        
        if (!filename) {
            printk("ERROR: file name %s not found\n", filename);
            return CMD_FAIL;
        }

        if ((fp = sal_fopen(filename, "rb")) == NULL) {
            parse_arg_eq_done(&pt);
            printk("ERROR: Can't open the file : %s\n", filename);
            return CMD_FAIL;
        }

        /* filename points to a allocated memory from arg parsing, calling 
         * the routine below frees it
         */ 
        parse_arg_eq_done(&pt);

        /* prompt user for confirmation */
        printk("Warning!!!\n"
                 "The PHY device will become un-usable if the power is off\n"
                 "during this process or a wrong file is given. The file must\n"
                 "be in BINARY format. The only way to recover is to program\n"
                 "the non-volatile storage device with a rom burner\n");

        if ((NULL == sal_readline("Are you sure you want to continue(yes/no)?",
                                 input, sizeof(input), "no")) || 
            (strlen(input) != strlen("yes")) ||
            (sal_strncasecmp("yes", input, strlen(input)))) {
            sal_fclose(fp);
            printk("Firmware updating aborted. No writes to the PHY device's "
                   "non-volatile storage\n");
            return CMD_FAIL;
        }

        buf = sal_alloc(FIRMWARE_BUF_LEN,"temp_buf");
        if (buf == NULL) {
            sal_fclose(fp);
            printk("ERROR: Can't allocate enough buffer : 0x%x\n", 
                    FIRMWARE_BUF_LEN);
            return CMD_FAIL;
        }

        printk("Firmware updating in prograss. ");
        offset = 0;
        len = 0;
        DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
            do {
                offset += len;
                len = fread(buf, 1, FIRMWARE_BUF_LEN, fp);
                printk("Data length: %d\n",len);
                printk("Please wait ....\n");
                
                /* for now, only allow external phy. If internal phy is
                 * intended use the flag BCM_PORT_PHY_INTERNAL
                 */
                rv = bcm_port_phy_firmware_set(u,p,0,offset,buf,len);
            } while (len >= FIRMWARE_BUF_LEN);
            break;
        }
        sal_fclose(fp);
        sal_free(buf);
        if (rv == SOC_E_NONE) {
            printk("Successfully Done!!!\n");
        } else if (rv == SOC_E_UNAVAIL) {
            printk("Exit. The feature is not available to this phy device\n");
        } else {
            printk("Failed. Phy device may not be usable\n");
        }
        return CMD_OK;
#endif
    }
    if (c != NULL && sal_strcasecmp(c, "power") == 0) {
        parse_table_t    pt;
        char *mode_type = 0;
        uint32 mode_value;

        if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
            printk("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
            return CMD_FAIL;
        }

        if ((c = ARG_CUR(a)) != NULL) {

            if (c[0] == '=') {
                return CMD_USAGE;        /* '=' unsupported */
            }

            parse_table_init(u, &pt);
            parse_table_add(&pt, "mode", PQ_DFL | PQ_STRING,0,
                    &mode_type, NULL);

            if (parse_arg_eq(a, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if (ARG_CNT(a) > 0) {
                printk("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
        }

        if (!mode_type) {
            char * str;
            printk("Phy Power Mode dump:\n");
            printk("%10s %16s %14s\n",
               "port", "name", "power_mode");
            DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
                mode_value = 0;
                rv = bcm_port_phy_control_get(u,p,BCM_PORT_PHY_CONTROL_POWER,
                                          &mode_value);
                if (rv == SOC_E_NONE) {
                    str = mode_value? "low":"full";
                } else {
                    str = "unavail";
                }                
                printk("%5s(%3d) %16s %14s\n",
                       SOC_PORT_NAME(u, p), p,
                       soc_phy_name_get(u, p),
                       str);
            }
            return CMD_OK;
        }
        if (sal_strcasecmp(mode_type, "auto_low") == 0) {
            (void)_phy_auto_low_start(u,pbm,1);
        } else if (sal_strcasecmp(mode_type, "auto_off") == 0) {
            (void)_phy_auto_low_start(u,pbm,0);
        } else if (sal_strcasecmp(mode_type, "low") == 0) {
            DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
                (void)bcm_port_phy_control_set(u,p,BCM_PORT_PHY_CONTROL_POWER,
                                          BCM_PORT_PHY_CONTROL_POWER_LOW);
            }
        } else if (sal_strcasecmp(mode_type, "full") == 0) {
            DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
                (void)bcm_port_phy_control_set(u,p,BCM_PORT_PHY_CONTROL_POWER,
                                          BCM_PORT_PHY_CONTROL_POWER_FULL);
            }
        }

        /* free allocated memory from arg parsing */
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (c != NULL && sal_strcasecmp(c, "wr") == 0) {
        cmd_result_t cmd_rv; 
        bcm_port_t   port;
        uint32       block;
        uint32       address;
        uint32       value;

        /* Get port */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	port = sal_ctoi(c, 0);
        if (!SOC_PORT_VALID(u, port)) {
            printk("%s: Invalid port\n", ARG_CMD(a));
            return CMD_FAIL;
        }
        
        /* Get block */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	block = sal_ctoi(c, 0);

        /* Get address */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	address = sal_ctoi(c, 0);

        /* Get value */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	value = sal_ctoi(c, 0);

        /* Write the phy register */
        cmd_rv = bcm_port_phy_set(u, port, BCM_PORT_PHY_INTERNAL, 
                                  BCM_PORT_PHY_REG_INDIRECT_ADDR
                                  (0, block, address), value);
        return cmd_rv;
    }

    if (c != NULL && sal_strcasecmp(c, "rd_cp") == 0) {
        cmd_result_t cmd_rv; 
        bcm_port_t   port;
        uint32       block;
        uint32       address;
        uint32       value;
        uint32       rval;

        /* Get port */
	c = ARG_GET(a);
	port = sal_ctoi(c, 0);
        if (!SOC_PORT_VALID(u, port)) {
            printk("%s: Invalid port\n", ARG_CMD(a));
            return CMD_FAIL;
        }
        
        /* Get block */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	block = sal_ctoi(c, 0);

        /* Get address */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	address = sal_ctoi(c, 0);

        /* Get compare value */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	value = sal_ctoi(c, 0);

        /* Read the phy register */
        cmd_rv = bcm_port_phy_get(u, port, BCM_PORT_PHY_INTERNAL, 
                                  BCM_PORT_PHY_REG_INDIRECT_ADDR
                                  (0, block, address), &rval);

        if (value != rval) {
            printk("Error: block %x, register %x expected %x, got %x\n", 
                   block, address, value, rval);
        } else {
            printk("Pass\n");
        }
        return cmd_rv;
    }

    if (c != NULL && sal_strcasecmp(c, "rd_cp2") == 0) {
        cmd_result_t cmd_rv; 
        bcm_port_t   port;
        uint32       block;
        uint32       address;
        uint32       value;
        uint32       mask;
        uint32       rval;

        /* Get port */
	c = ARG_GET(a);
	port = sal_ctoi(c, 0);
        if (!SOC_PORT_VALID(u, port)) {
            printk("%s: Invalid port\n", ARG_CMD(a));
            return CMD_FAIL;
        }
        
        /* Get block */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	block = sal_ctoi(c, 0);

        /* Get address */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	address = sal_ctoi(c, 0);

        /* Get compare value */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	value = sal_ctoi(c, 0);

        /* Get mask */
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
	mask = sal_ctoi(c, 0);

        /* Read the phy register */
        cmd_rv = bcm_port_phy_get(u, port, BCM_PORT_PHY_INTERNAL, 
                                  BCM_PORT_PHY_REG_INDIRECT_ADDR
                                  (0, block, address), &rval);

        if ((value & mask) != (rval & mask)) {
            printk("Error: block %x, register %x expected %x, got %x\n", 
                   block, address, (value & mask), (rval & mask));
        } else {
            printk("Pass\n");
        }
        return cmd_rv;
    }

    if (c != NULL && sal_strcasecmp(c, "control") == 0) {
        uint32       wan_mode, preemphasis, predriver_current, driver_current;
        uint32       interface;
        uint32       interfacemax;
        uint32       flags;
        int          print_header;
        cmd_result_t cmd_rv; 
        bcm_port_config_t pcfg;
        uint32 macsec_switch_fixed, macsec_switch_fixed_speed;
        uint32 macsec_switch_fixed_duplex, macsec_switch_fixed_pause;
        uint32 macsec_pause_rx_fwd, macsec_pause_tx_fwd;
        uint32 macsec_line_ipg, macsec_switch_ipg;
        uint32 longreach_speed, longreach_pairs;
        uint32 longreach_gain, longreach_autoneg;
        uint32 longreach_local_ability, longreach_remote_ability;
        uint32 longreach_current_ability, longreach_master;
        uint32 longreach_active, longreach_enable;

        macsec_switch_fixed = 0;
        macsec_switch_fixed_speed = 0;
        macsec_switch_fixed_duplex = 0;
        macsec_switch_fixed_pause = 0;
        macsec_pause_rx_fwd = 0;
        macsec_pause_tx_fwd = 0;
        macsec_line_ipg = 0;
        macsec_switch_ipg = 0;

        longreach_speed = 0;
        longreach_pairs = 0;
        longreach_gain = 0;
        longreach_autoneg = 0;
        longreach_local_ability = 0;
        longreach_remote_ability = 0;
        longreach_current_ability = 0;
        longreach_master = 0;
        longreach_active = 0;
        longreach_enable = 0;

        if (bcm_port_config_get(u, &pcfg) != BCM_E_NONE) {
            printk("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
            return CMD_FAIL;
        }

        wan_mode          = 0;
        preemphasis       = 0;
        predriver_current = 0;
        driver_current    = 0;
        interface         = 0;
        interfacemax      = 0;
        if ((c = ARG_GET(a)) == NULL) {
            SOC_PBMP_ASSIGN(pbm, pcfg.port);
        } else if (parse_bcm_pbmp(u, c, &pbm) < 0) {
            printk("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
            return CMD_FAIL;
        }

	BCM_PBMP_AND(pbm, pcfg.port);

        flags = 0;
        if ((c = ARG_CUR(a)) != NULL) {
            parse_table_t    pt;
            int              i;

            if (c[0] == '=') {
                return CMD_USAGE;        /* '=' unsupported */
            }

            parse_table_init(u, &pt);
            parse_table_add(&pt, "WanMode", PQ_DFL|PQ_BOOL, 
                            0, &wan_mode, 0);
            parse_table_add(&pt, "Preemphasis", PQ_DFL|PQ_INT, 
                            0, &preemphasis, 0);
            parse_table_add(&pt, "DriverCurrent", PQ_DFL|PQ_INT, 
                            0, &driver_current, 0);
            parse_table_add(&pt, "PreDriverCurrent", PQ_DFL|PQ_INT, 
                            0, &predriver_current, 0);
            parse_table_add(&pt, "Interface", PQ_DFL|PQ_INT, 
                            0, &interface, 0);
            parse_table_add(&pt, "InterfaceMax", PQ_DFL|PQ_INT, 
                            0, &interfacemax, 0);

            parse_table_add(&pt, "MacsecSwitchFixed", PQ_DFL|PQ_BOOL, 
                            0, &macsec_switch_fixed, 0);
            parse_table_add(&pt, "MacsecSwitchFixedSpeed", PQ_DFL|PQ_INT, 
                            0, &macsec_switch_fixed_speed, 0);
            parse_table_add(&pt, "MacsecSwitchFixedDuplex", PQ_DFL|PQ_BOOL, 
                            0, &macsec_switch_fixed_duplex, 0);
            parse_table_add(&pt, "MacsecSwitchFixedPause", PQ_DFL|PQ_BOOL, 
                            0, &macsec_switch_fixed_pause, 0);
            parse_table_add(&pt, "MacsecPauseRXForward", PQ_DFL|PQ_BOOL, 
                            0, &macsec_pause_rx_fwd, 0);
            parse_table_add(&pt, "MacsecPauseTXForward", PQ_DFL|PQ_BOOL, 
                            0, &macsec_pause_tx_fwd, 0);
            parse_table_add(&pt, "MacsecLineIPG", PQ_DFL|PQ_INT, 
                            0, &macsec_line_ipg, 0);
            parse_table_add(&pt, "MacsecSwitchIPG", PQ_DFL|PQ_INT, 
                            0, &macsec_switch_ipg, 0);

            parse_table_add(&pt, "LongreachSpeed", PQ_DFL|PQ_INT, 
                            0, &longreach_speed, 0);
            parse_table_add(&pt, "LongreachPairs", PQ_DFL|PQ_INT, 
                            0, &longreach_pairs, 0);
            parse_table_add(&pt, "LongreachGain", PQ_DFL|PQ_INT, 
                            0, &longreach_gain, 0);
            parse_table_add(&pt, "LongreachAutoneg", PQ_DFL|PQ_BOOL, 
                            0, &longreach_autoneg, 0);
            parse_table_add(&pt, "LongreachLocalAbility", PQ_DFL|PQ_INT, 
                            0, &longreach_local_ability, 0);
            parse_table_add(&pt, "LongreachRemoteAbility", PQ_DFL|PQ_INT, 
                            0, &longreach_remote_ability, 0);
            parse_table_add(&pt, "LongreachCurrentAbility", PQ_DFL|PQ_INT, 
                            0, &longreach_current_ability, 0);
            parse_table_add(&pt, "LongreachMaster", PQ_DFL|PQ_BOOL, 
                            0, &longreach_master, 0);
            parse_table_add(&pt, "LongreachActive", PQ_DFL|PQ_BOOL, 
                            0, &longreach_active, 0);
            parse_table_add(&pt, "LongreachEnable", PQ_DFL|PQ_BOOL, 
                            0, &longreach_enable, 0);

            if (parse_arg_eq(a, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if (ARG_CNT(a) > 0) {
                printk("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }

            for (i = 0; i < pt.pt_cnt; i++) {
                if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                    flags |= (1 << i);
                }
            }
            parse_arg_eq_done(&pt);
        }
 
        DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
            print_header = TRUE;

            /* Read and set WAN mode */
            cmd_rv = port_phy_control_update(u, p, BCM_PORT_PHY_CONTROL_WAN,
                                             wan_mode, flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set Preemphasis */
            cmd_rv = port_phy_control_update(u, p, 
                                             BCM_PORT_PHY_CONTROL_PREEMPHASIS,
                                             preemphasis, flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set Driver Current */ 
            cmd_rv = port_phy_control_update(u, p, 
                                          BCM_PORT_PHY_CONTROL_DRIVER_CURRENT,
                                          driver_current, flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set Pre-driver Current */
            cmd_rv = port_phy_control_update(u, p, 
                                    BCM_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT,
                                    predriver_current, flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the interface */
            cmd_rv = port_phy_control_update(u, p,
                                    BCM_PORT_PHY_CONTROL_INTERFACE,
                                    interface, flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            /* Read and set(is noop) the interface */
            cmd_rv = port_phy_control_update(u, p,
                                    SOC_PHY_CONTROL_INTERFACE_MAX,
                                    interfacemax, flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
#ifdef INCLUDE_MACSEC

            /* Read and set the Switch fixed */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED,
                               macsec_switch_fixed, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the Switch fixed Speed */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED,
                               macsec_switch_fixed_speed, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the Switch fixed Duplex */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX,
                               macsec_switch_fixed_duplex, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the Switch fixed Pause */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE,
                               macsec_switch_fixed_pause, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set Pause Receive Forward */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD,
                               macsec_pause_rx_fwd, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set Pause transmit Forward */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD,
                               macsec_pause_tx_fwd, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set Line Side IPG */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_MACSEC_LINE_IPG,
                               macsec_line_ipg, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set Switch Side IPG */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_IPG,
                               macsec_switch_ipg, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
#endif
#ifdef INCLUDE_LONGREACH

            /* Read and set the longreach speed */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_SPEED,
                               longreach_speed, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach pairs */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_PAIRS,
                               longreach_pairs, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach gain */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_GAIN,
                               longreach_gain, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach autoneg (LDS) */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_AUTONEG,
                               longreach_autoneg, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach local ability */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_LOCAL_ABILITY,
                               longreach_local_ability, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read the longreach remote ability */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_REMOTE_ABILITY,
                               longreach_remote_ability, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read the longreach current ability (GCD - read only) */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_CURRENT_ABILITY,
                               longreach_current_ability, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach master (when no LDS) */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_MASTER,
                               longreach_master, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            /* Read and set the longreach active (LR is active - read only) */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_ACTIVE,
                               longreach_active, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            /* Read and set the longreach active (Enable LR) */
            cmd_rv = port_phy_control_update(u, p,
                               BCM_PORT_PHY_CONTROL_LONGREACH_ENABLE,
                               longreach_enable, 
                               flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
#endif
        }
        return CMD_OK;
    }
    /* All access to an MII register */
    if (c != NULL && sal_strcasecmp(c, "dumpall") == 0) {
        uint8 phy_addr_start = 0, phy_addr_end = 0x7F;
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        is_c45 = 0;
        if (soc_feature(u, soc_feature_phy_cl45)) {
            if (sal_strcasecmp(c, "c45") == 0) {
                is_c45 = 1;
            }
        }
        if (sal_strcasecmp(c, "c22") == 0) {
            is_c45 = 0;
        }
        if ((c = ARG_GET(a)) != NULL) {
            phy_addr_start = strtoul(c, NULL, 0) & 0x7F;
            if ((c = ARG_GET(a)) != NULL) {
            }
            phy_addr_end = strtoul(c, NULL, 0) & 0x7F;
        }
        if (is_c45) {
            printk("%s\t%s:%s %s : %s\n", "", "PRTAD", "DEVAD", "REG", "VALUE");
            for(phy_addr = phy_addr_start;phy_addr <= phy_addr_end;phy_addr++) {
                for(phy_devad = 0; phy_devad <= 0x1f; phy_devad++) {
                    for(phy_reg = 2; phy_reg <= 3; phy_reg++) {
                        rv = soc_miimc45_read(u, phy_addr, phy_devad, 
                                              phy_reg, &phy_data);
                        if (rv < 0) {
                            printk("ERROR: MII Addr %d: soc_miim_read failed: %s\n",
                                   phy_addr, soc_errmsg(rv));
                            return CMD_FAIL;
                        }
                        if ((phy_data != 0xFFFF) && (phy_data != 0x0000)) {
                            printk("%s\t0x%02X:0x%02X\t0x%02x: 0x%04x\n",
                                   "", phy_addr, phy_devad, phy_reg, phy_data);
                        }
                    }
                }
            }
        } else {
            printk("%s\t%s %s : %s\n", "", "PRTAD", "REG", "VALUE");
            for(phy_addr = phy_addr_start;phy_addr <= phy_addr_end;phy_addr++) {
                for(phy_reg = 2; phy_reg <= 3; phy_reg++) {
                    rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
                    if (rv < 0) {
                        printk("ERROR: MII Addr %d: soc_miim_read failed: %s\n",
                               phy_addr, soc_errmsg(rv));
                        return CMD_FAIL;
                    }
                    if ((phy_data != 0xFFFF) && (phy_data != 0x0000)) {
                        printk("%s\t0x%02X\t0x%02x: 0x%04x\n",
                               "", phy_addr, phy_reg, phy_data);
                    }
                }
            }
        }
        return CMD_OK;
    }

    /* Raw access to an MII register */
    if (c != NULL && sal_strcasecmp(c, "raw") == 0) {
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        if (soc_feature(u, soc_feature_phy_cl45)) {
            if (sal_strcasecmp(c, "c45") == 0) {
                is_c45 = 1;
                if ((c = ARG_GET(a)) == NULL) {
                    return CMD_USAGE;
                }
            }
        }
        phy_addr = strtoul(c, NULL, 0);
        if ((c = ARG_GET(a)) == NULL) { /* Get register number */
            return CMD_USAGE;
        }
        if (is_c45) {
            phy_devad = strtoul(c, NULL, 0);
            if ((c = ARG_GET(a)) == NULL) { /* Get register number */
                return CMD_USAGE;
            }
        }
        phy_reg = strtoul(c, NULL, 0);
        if ((c = ARG_GET(a)) == NULL) { /* Read register */
            if (is_c45) {
                rv = soc_miimc45_read(u, phy_addr, phy_devad, 
                                      phy_reg, &phy_data);
                if (rv < 0) {
                    printk("ERROR: MII Addr %d: soc_miim_read failed: %s\n",
                           phy_addr, soc_errmsg(rv));
                    return CMD_FAIL;
                }
            } else {
                rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
                if (rv < 0) {
                    printk("ERROR: MII Addr %d: soc_miim_read failed: %s\n",
                           phy_addr, soc_errmsg(rv));
                    return CMD_FAIL;
                }
            }
            printk("%s\t0x%02x: 0x%04x\n",
                   "", phy_reg, phy_data);
        } else { /* write */
            phy_data = strtoul(c, NULL, 0);
            if (is_c45) {
                rv = soc_miimc45_write(u, phy_addr, phy_devad, 
                                       phy_reg, phy_data);
            } else {
                rv = soc_miim_write(u, phy_addr, phy_reg, phy_data);
            }
            if (rv < 0) {
                printk("ERROR: MII Addr %d: soc_miim_write failed: %s\n",
                       phy_addr, soc_errmsg(rv));
                return CMD_FAIL;
            }
        }
        return CMD_OK;
    }

    if (c != NULL && sal_strcasecmp(c, "int") == 0) {
	intermediate = 1;
	c = ARG_GET(a);
    }

    if (c == NULL) {
	return(CMD_USAGE);
    }

    /* Parse the bitmap. */
    if (parse_pbmp(u, c, &pbm) < 0) {
        printk("%s: ERROR: unrecognized port bitmap: %s\n",
               ARG_CMD(a), c);
        return CMD_FAIL;
    }

    SOC_PBMP_ASSIGN(pbm_phys, pbm);
    SOC_PBMP_AND(pbm_phys, PBMP_PORT_ALL(u));
    if (SOC_PBMP_IS_NULL(pbm_phys)) {
        printk("Ports specified do not have PHY drivers.\n");
    } else {
	SOC_PBMP_ASSIGN(pbm_temp, pbm);
	SOC_PBMP_REMOVE(pbm_temp, PBMP_PORT_ALL(u));
	if (SOC_PBMP_NOT_NULL(pbm_temp)) {
        printk("Not all ports given have PHY drivers.  Using %s\n",
               SOC_PBMP_FMT(pbm_phys, pfmt));
	}
    }
    SOC_PBMP_ASSIGN(pbm, pbm_phys);

    if (ARG_CNT(a) == 0) {	/*  show information for all registers */
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
	    phy_addr = (intermediate ?
			PORT_TO_PHY_ADDR_INT(u, p) :
			PORT_TO_PHY_ADDR(u, p));
	    if (phy_addr == 0xff) {
		printk("Port %s: No %sPHY address assigned\n",
		       SOC_PORT_NAME(u, p),
		       intermediate ? "intermediate " : "");
		continue;
	    }
	    if (intermediate) {
		printk("Port %s (intermediate PHY addr 0x%02x):",
		       SOC_PORT_NAME(u, p), phy_addr);
	    } else {
                int ap = p;
                BCM_API_XLATE_PORT_P2A(u, &ap); /* Use BCM API port */
                BCM_IF_ERROR_RETURN(bcm_port_phy_drv_name_get(u, ap, drv_name, 64));
                printk("Port %s (PHY addr 0x%02x): %s (%s)",
		       SOC_PORT_NAME(u, p), phy_addr,
		       soc_phy_name_get(u, p), drv_name);
	    }
            if ((!intermediate) && (soc_phy_is_c45_miim(u, p))) {
                for(i = 0; i < COUNTOF(p_devad); i++) {
                    phy_devad = p_devad[i];
                    printk("\nDevAd = %d(%s)", phy_devad, p_devad_str[i]);
                    for (phy_reg = PHY_MIN_REG; phy_reg <= PHY_MAX_REG; phy_reg++) {
                        rv = soc_miimc45_read(u, phy_addr,
                                              phy_devad, phy_reg, &phy_data);
                        if (rv < 0) {
                            printk("\nERROR: Port %s: soc_miim_read failed: %s\n",
                                   SOC_PORT_NAME(u, p), soc_errmsg(rv));
                            rv = CMD_FAIL;
                            goto done;
                        }
                        printk("%s\t0x%04x: 0x%04x",
                               ((phy_reg % DUMP_PHY_COLS) == 0) ? "\n" : "",
                               phy_reg, phy_data);
                    }
                }
            } else {
                for (phy_reg = PHY_MIN_REG; phy_reg <= PHY_MAX_REG; phy_reg++) {
                    rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
                    if (rv < 0) {
                        printk("\nERROR: Port %s: soc_miim_read failed: %s\n",
                               SOC_PORT_NAME(u, p), soc_errmsg(rv));
                        rv = CMD_FAIL;
                        goto done;
                    }
                    printk("%s\t0x%02x: 0x%04x",
                           ((phy_reg % DUMP_PHY_COLS) == 0) ? "\n" : "",
                           phy_reg, phy_data);
                }
            }
	    printk("\n");
        }
    } else {				/* get register argument */
	c = ARG_GET(a);
	phy_reg = sal_ctoi(c, 0);

	if (ARG_CNT(a) == 0) {		/* no more args; show this register */
            DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
		phy_addr = (intermediate ?
			    PORT_TO_PHY_ADDR_INT(u, p) :
			    PORT_TO_PHY_ADDR(u, p));
		if (phy_addr == 0xff) {
		    printk("Port %s: No %sPHY address assigned\n",
			   SOC_PORT_NAME(u, p),
			   intermediate ? "intermediate " : "");
		    continue;
		}
        if ((!intermediate) && (soc_phy_is_c45_miim(u, p))) {
            for(i = 0; i < COUNTOF(p_devad); i++) {
                phy_devad = p_devad[i];
                printk("Port %s (PHY addr 0x%02x) DevAd %d(%s) Reg 0x%04x: ",
                       SOC_PORT_NAME(u, p), phy_addr, phy_devad,
                       p_devad_str[i], phy_reg);
                rv = soc_miimc45_read(u, phy_addr,
                        phy_devad, phy_reg, &phy_data);
                if (rv < 0) {
                    printk("\nERROR: Port %s: soc_miim_read failed: %s\n",
                       SOC_PORT_NAME(u, p), soc_errmsg(rv));
                    rv = CMD_FAIL;
                    goto done;
                }
                printk("0x%04x\n", phy_data);
            }
        } else {
            printk("Port %s (PHY addr 0x%02x) Reg %d: ",
                       SOC_PORT_NAME(u, p), phy_addr, phy_reg);
            rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
            if (rv < 0) {
                printk("\nERROR: Port %s: soc_miim_read failed: %s\n",
                   SOC_PORT_NAME(u, p), soc_errmsg(rv));
                rv = CMD_FAIL;
                goto done;
            }
            printk("0x%04x\n", phy_data);
        }
        }
        } else {	/* set the reg to given value for the indicated phys */
            c = ARG_GET(a);
            phy_data = sal_ctoi(c, 0);

            DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
		phy_addr = (intermediate ?
			    PORT_TO_PHY_ADDR_INT(u, p) :
			    PORT_TO_PHY_ADDR(u, p));
		if (phy_addr == 0xff) {
		    printk("Port %s: No %sPHY address assigned\n",
			   SOC_PORT_NAME(u, p),
			   intermediate ? "intermediate " : "");
		    rv = CMD_FAIL;
		    goto done;
		}
        if ((!intermediate) && (soc_phy_is_c45_miim(u, p))) {
            phy_devad = phy_data;
            for(i = 0; i < COUNTOF(p_devad); i++) {
                if (phy_devad == p_devad[i]) {
                    break;
                }
            }
            if (i >= COUNTOF(p_devad)) {
                printk("\nERROR: Port %s: Invalid DevAd %d\n",
                   SOC_PORT_NAME(u, p), phy_devad);
                rv = CMD_FAIL;
                continue;
            }
            if (ARG_CNT(a) == 0) {	/* no more args; show this register */
            printk("Port %s (PHY addr 0x%02x) DevAd %d(%s) Reg 0x%04x: ",
                   SOC_PORT_NAME(u, p), phy_addr, phy_devad,
                   p_devad_str[i], phy_reg);
            rv = soc_miimc45_read(u, phy_addr,
                    phy_devad, phy_reg, &phy_data);
            if (rv < 0) {
                printk("\nERROR: Port %s: soc_miim_read failed: %s\n",
                   SOC_PORT_NAME(u, p), soc_errmsg(rv));
                rv = CMD_FAIL;
                goto done;
            }
            printk("0x%04x\n", phy_data);
            } else { /* write */
                c = ARG_GET(a);
                phy_data = sal_ctoi(c, 0);
                rv = soc_miimc45_write(u, phy_addr,
                        phy_devad, phy_reg, phy_data);
                if (rv < 0) {
                    printk("ERROR: Port %s: soc_miim_write failed: %s\n",
                       SOC_PORT_NAME(u, p), soc_errmsg(rv));
                    rv = CMD_FAIL;
                    goto done;
                }
            }
        } else {
		rv = soc_miim_write(u, phy_addr, phy_reg, phy_data);
		if (rv < 0) {
		    printk("ERROR: Port %s: soc_miim_write failed: %s\n",
			   SOC_PORT_NAME(u, p), soc_errmsg(rv));
		    rv = CMD_FAIL;
		    goto done;
		}
	    }
        }
	}
    }

 done:
    return rv;
}

/***********************************************************************
 *
 * Combo port support
 *
 ***********************************************************************/


/*
 * Function:	if_combo_dump
 * Purpose:	Dump the contents of a bcm_phy_config_t
 */

STATIC int
if_combo_dump(args_t *a, int u, int p, int medium)
{
    char		pm_str[80];
    bcm_port_medium_t   active_medium;
    int			r;
    bcm_phy_config_t	cfg;

    /*
     * Get active medium so we can put an asterisk next to the status if
     * it is active.
     */

    if ((r = bcm_port_medium_get(u, p, &active_medium)) < 0) {
	return r;
    }
    if ((r = bcm_port_medium_config_get(u, p, medium, &cfg)) < 0) {
	return r;
    }

    printk("%s:\t%s medium%s\n",
	   BCM_PORT_NAME(u, p),
	   MEDIUM_STATUS(medium),
	   (medium == active_medium) ? " (active)" : "");

    format_port_mode(pm_str, sizeof (pm_str), cfg.autoneg_advert, TRUE);

    printk("\tenable=%d preferred=%d "
	   "force_speed=%d force_duplex=%d master=%s\n",
	   cfg.enable, cfg.preferred,
	   cfg.force_speed, cfg.force_duplex,
	   PHYMASTER_MODE(cfg.master));
    printk("\tautoneg_enable=%d autoneg_advert=%s(0x%x)\n",
	   cfg.autoneg_enable, pm_str, cfg.autoneg_advert);
    printk("\tMDIX=%s\n",
	   MDIX_MODE(cfg.mdix));

    return BCM_E_NONE;
}

static int combo_watch[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];

static void
if_combo_watch(int unit, bcm_port_t port, bcm_port_medium_t medium, void *arg) 
{
    printk("Unit %d: %s: Active medium switched to %s\n",
           unit, BCM_PORT_NAME(unit, port), MEDIUM_STATUS(medium));

    /* 
     * Increment the number of medium changes. Remember, that we pass the 
     * address of combo_watch[unit][port] when we register this callback
     */
    (*((int *)arg))++; 
}

/*
 * Function: 	if_combo
 * Purpose:	Control combo ports
 * Parameters:	u - SOC unit #
 *		a - pointer to args
 * Returns:	CMD_OK/CMD_FAIL/
 */
cmd_result_t
if_esw_combo(int u, args_t *a)
{
    pbmp_t		pbm;
    soc_port_t		p, dport;
    int			specified_medium = BCM_PORT_MEDIUM_COUNT;
    int			r, rc, rf;
    char		*c;
    parse_table_t	pt;
    bcm_phy_config_t	cfg, cfg_opt;

    enum if_combo_cmd_e {
        COMBO_CMD_DUMP,
        COMBO_CMD_SET,
        COMBO_CMD_WATCH,
        CONBO_CMD_COUNT
    } cmd;

    enum if_combo_watch_arg_e {
        COMBO_CMD_WATCH_SHOW,
        COMBO_CMD_WATCH_ON,
        COMBO_CMD_WATCH_OFF,
        COMBO_CMD_WATCH_COUNT
    } watch_arg = COMBO_CMD_WATCH_SHOW;

    cfg_opt.enable = -1;
    cfg_opt.preferred = -1;
    cfg_opt.autoneg_enable = -1;
    cfg_opt.autoneg_advert = 0xffffffff;
    cfg_opt.force_speed = -1;
    cfg_opt.force_duplex = -1;
    cfg_opt.master = -1;
    cfg_opt.mdix = -1;

    if (!sh_check_attached(ARG_CMD(a), u)) {
	return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    if (parse_bcm_pbmp(u, c, &pbm) < 0) {
	printk("%s: ERROR: unrecognized port bitmap: %s\n",
	       ARG_CMD(a), c);
	return CMD_FAIL;
    }

    SOC_PBMP_AND(pbm, PBMP_PORT_ALL(u));

    c = ARG_GET(a);		/* NULL or media type or command */

    if (c == NULL) {
        cmd = COMBO_CMD_DUMP;
        specified_medium = BCM_PORT_MEDIUM_COUNT;
    } else if (sal_strcasecmp(c, "copper") == 0 || 
               sal_strcasecmp(c, "c") == 0) {
        cmd = COMBO_CMD_SET;
        specified_medium = BCM_PORT_MEDIUM_COPPER;
    } else if (sal_strcasecmp(c, "fiber") == 0 || 
               sal_strcasecmp(c, "f") == 0) {
        cmd = COMBO_CMD_SET;
        specified_medium = BCM_PORT_MEDIUM_FIBER;
    } else if (sal_strcasecmp(c, "watch") == 0 || 
               sal_strcasecmp(c, "w") == 0) {
        cmd = COMBO_CMD_WATCH;
    } else {
	    return CMD_USAGE;
    }

    switch (cmd) {
    case COMBO_CMD_SET:
	if ((c = ARG_CUR(a)) != NULL) {
	    if (c[0] == '=') {
		return CMD_USAGE;	/* '=' unsupported */
	    }

	    parse_table_init(u, &pt);
	    parse_table_add(&pt, "Enable", PQ_DFL|PQ_BOOL, 0,
			    &cfg_opt.enable, 0);
	    parse_table_add(&pt, "PREFerred", PQ_DFL|PQ_BOOL, 0,
			    &cfg_opt.preferred, 0);
	    parse_table_add(&pt, "Autoneg_Enable", PQ_DFL|PQ_BOOL, 0,
			    &cfg_opt.autoneg_enable, 0);
	    parse_table_add(&pt, "Autoneg_Advert", PQ_DFL|PQ_PORTMODE, 0,
			    &cfg_opt.autoneg_advert, 0);
	    parse_table_add(&pt, "Force_Speed", PQ_DFL|PQ_INT, 0,
			    &cfg_opt.force_speed, 0);
	    parse_table_add(&pt, "Force_Duplex", PQ_DFL|PQ_BOOL, 0,
			    &cfg_opt.force_duplex, 0);
	    parse_table_add(&pt, "MAster", PQ_DFL|PQ_BOOL, 0,
			    &cfg_opt.master, 0);
            parse_table_add(&pt, "MDIX", PQ_DFL|PQ_MULTI, 0,
                            &cfg_opt.mdix, mdix_mode);

	    if (parse_arg_eq(a, &pt) < 0) {
		parse_arg_eq_done(&pt);
		return CMD_USAGE;
	    }
	    parse_arg_eq_done(&pt);

	    if (ARG_CUR(a) != NULL) {
		return CMD_USAGE;
	    }
	} else {
            cmd = COMBO_CMD_DUMP;
        }

        break;
        
    case COMBO_CMD_WATCH:
        c = ARG_GET(a);

        if (c == NULL) {
            watch_arg = COMBO_CMD_WATCH_SHOW;
        } else if (sal_strcasecmp(c, "on") == 0) {
            watch_arg = COMBO_CMD_WATCH_ON;
        } else if (sal_strcasecmp(c, "off") == 0) {
            watch_arg = COMBO_CMD_WATCH_OFF;
        } else {
            return CMD_USAGE;
        }
        break;

    default:
        break;
    }

    DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
	switch (cmd) {
        case COMBO_CMD_DUMP:
	    printk("Port %s:\n", BCM_PORT_NAME(u, p));
            
            rc = rf = BCM_E_UNAVAIL;
            if (specified_medium == BCM_PORT_MEDIUM_COPPER ||
                specified_medium == BCM_PORT_MEDIUM_COUNT) {
                rc = if_combo_dump(a, u, p, BCM_PORT_MEDIUM_COPPER);
                if (rc != BCM_E_NONE && rc != BCM_E_UNAVAIL) {
                    printk("%s:\tERROR(copper): %s\n",
                           BCM_PORT_NAME(u, p),
                           bcm_errmsg(rc));
                }
            } 

            if (specified_medium == BCM_PORT_MEDIUM_FIBER ||
                specified_medium == BCM_PORT_MEDIUM_COUNT) {
                rf = if_combo_dump(a, u, p, BCM_PORT_MEDIUM_FIBER);
                if (rf != BCM_E_NONE && rf != BCM_E_UNAVAIL) {
                    printk("%s:\tERROR(fiber): %s\n",
                           BCM_PORT_NAME(u, p),
                           bcm_errmsg(rf));
                }
            }

            /*
             * If there were problems getting medium-specific info on 
             * individual mediums, then they will be printed above. However,
             * if BCM_E_UNAVAIL is returned for both copper and fiber mediums
             * we'll print only one error message
             */
            if (rc == BCM_E_UNAVAIL && rf == BCM_E_UNAVAIL) {
                printk("%s:\tmedium info unavailable\n",
                       BCM_PORT_NAME(u, p));
            } 
           break;

        case COMBO_CMD_SET:
            /*
             * Update the medium operating mode.
             */
            r = bcm_port_medium_config_get(u, p,
                                           specified_medium,
                                           &cfg);

            if (r < 0) {
                printk("%s: port %s: Error getting medium config: %s\n",
                       ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
                return CMD_FAIL;
            }

            if (cfg_opt.enable != -1) {
                cfg.enable = cfg_opt.enable;
            }
            
            if (cfg_opt.preferred != -1) {
                cfg.preferred = cfg_opt.preferred;
            }
            
            if (cfg_opt.autoneg_enable != -1) {
                cfg.autoneg_enable = cfg_opt.autoneg_enable;
            }
            
            if (cfg_opt.autoneg_advert != 0xffffffff) {
                cfg.autoneg_advert = cfg_opt.autoneg_advert;
            }
            
            if (cfg_opt.force_speed != -1) {
                cfg.force_speed = cfg_opt.force_speed;
            }
            
            if (cfg_opt.force_duplex != -1) {
                cfg.force_duplex = cfg_opt.force_duplex;
            }
            
            if (cfg_opt.master != -1) {
                cfg.master = cfg_opt.master;
            }
            
            if (cfg_opt.mdix != -1) {
                cfg.mdix = cfg_opt.mdix;
            }
            
            r = bcm_port_medium_config_set(u, p,
                                           specified_medium,
                                           &cfg);
        
            if (r < 0) {
                printk("%s: port %s: Error setting medium config: %s\n",
                       ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
                return CMD_FAIL;
            }

            break;

        case COMBO_CMD_WATCH:
            switch (watch_arg) {
            case COMBO_CMD_WATCH_SHOW:
                if (combo_watch[u][p]) {
                    printk("Port %s: Medium status change watch is  ON. "
                           "Medim changed %d times\n",
                           BCM_PORT_NAME(u, p),
                           combo_watch[u][p] - 1);
                } else {
                    printk("Port %s: Medium status change watch is OFF.\n",
                            BCM_PORT_NAME(u, p));
                }
                break;

            case COMBO_CMD_WATCH_ON:
                if (!combo_watch[u][p]) {
                    r = bcm_port_medium_status_register(u, p, 
                                                        if_combo_watch,
                                                        &combo_watch[u][p]);
                    if (r < 0) {
                        printk("Error registerinig medium status change "
                               "callback for %s: %s\n",
                               BCM_PORT_NAME(u, p), soc_errmsg(r));
                        return (CMD_FAIL);
                    }

                    combo_watch[u][p] = 1;
                }

                printk("Port %s: Medium change watch is ON\n",
                       BCM_PORT_NAME(u, p));

                break;

            case COMBO_CMD_WATCH_OFF:
                if (combo_watch[u][p]) {
                    r = bcm_port_medium_status_unregister(u, p, 
                                                          if_combo_watch,
                                                          &combo_watch[u][p]);
                    if (r < 0) {
                        printk("Error unregisterinig medium status change "
                               "callback for %s: %s\n",
                               BCM_PORT_NAME(u, p), soc_errmsg(r));
                        return (CMD_FAIL);
                    }

                    combo_watch[u][p] = 0;
                }

                printk("Port %s: Medium change watch is OFF\n",
                       BCM_PORT_NAME(u, p));

                break;

            default:
                return CMD_FAIL;
            }
           
            break;

        default:
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

/*
 * Function:
 *	cmd_cablediag
 * Purpose:
 *	Run cable diagnostics (if available)
 */

cmd_result_t
cmd_esw_cablediag(int unit, args_t *a)
{
    char	*s;
    bcm_pbmp_t	pbm;
    bcm_port_t	port, dport;
    int		rv, i;
    bcm_port_cable_diag_t	cd;
    char	*statename[] = _SHR_PORT_CABLE_STATE_NAMES_INITIALIZER;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if ((s = ARG_GET(a)) == NULL) {
	return CMD_USAGE;
    }

    if (parse_bcm_pbmp(unit, s, &pbm) < 0) {
        printk("%s: ERROR: unrecognized port bitmap: %s\n",
               ARG_CMD(a), s);
        return CMD_FAIL;
    }

    DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
	rv = bcm_port_cable_diag(unit, port, &cd);
	if (rv < 0) {
	    printk("%s: ERROR: port %s: %s\n",
		   ARG_CMD(a), BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
	    continue;
	}
	if (cd.fuzz_len == 0) {
	    printk("port %s: cable %s (%d pairs)\n",
		   BCM_PORT_NAME(unit, port), statename[cd.state],
		   cd.npairs);
	} else {
	    printk("port %s: cable %s (%d pairs, length +/- %d meters)\n",
		   BCM_PORT_NAME(unit, port), statename[cd.state],
		   cd.npairs, cd.fuzz_len);
	}
	for (i = 0; i < cd.npairs; i++) {
		printk("\tpair %c %s, length %d meters\n",
		       'A' + i, statename[cd.pair_state[i]], cd.pair_len[i]);
	}
    }

    return CMD_OK;
}

/* Must stay in sync with bcm_color_t enum (bcm/types.h) */
const char *diag_parse_color[] = {
    "Green",
    "Yellow",
    "Red",
    NULL
};

char cmd_color_usage[] =
    "Usages:\n\t"
    "  color set Port=<port> Prio=<prio> CFI=<cfi>\n\t"
    "        Color=<Green|Yellow|Red>\n\t"
    "  color show Port=<port>\n\t"
    "  color map Port=<port> PktPrio=<prio> CFI=<cfi>\n\t"
    "        IntPrio=<prio> Color=<Green|Yellow|Red>\n\t"
    "  color unmap Port=<port> IntPrio=<prio> Color=<Green|Yellow|Red>\n\t"
    "        PktPrio=<prio> CFI=<cfi>\n";

cmd_result_t
cmd_color(int unit, args_t *a)
{
    int                 port = 0, prio = -1, cfi = -1, color_parse = bcmColorRed;
    bcm_color_t         color;
    char 		*subcmd;
    int 		r;
    parse_table_t	pt;
    cmd_result_t	retCode;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "set") == 0) {

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "PRio", PQ_INT|PQ_DFL, 0, &prio, NULL);
        parse_table_add(&pt, "CFI", PQ_INT|PQ_DFL, 0, &cfi, NULL);
        parse_table_add(&pt, "Color", PQ_MULTI|PQ_DFL, 0,
                        &color_parse, diag_parse_color);

        if (!parseEndOk( a, &pt, &retCode)) {
            return retCode;
        }
        if (!SOC_PORT_VALID(unit,port)) {
            printk("%s: ERROR: Invalid port selection %d\n",
                   ARG_CMD(a), port);
            return CMD_FAIL;
        }

        color = (bcm_color_t) color_parse;

        if (prio < 0) {
            if (cfi < 0) {
                /* No selection to assign color */
                printk("%s: ERROR: No parameter to assign color\n",
                       ARG_CMD(a));
                return CMD_FAIL;
            } else {
                if ((r = bcm_port_cfi_color_set(unit, port,
                                                cfi, color)) < 0) {
                    goto bcm_err;
                }
            }
        } else {
            if (cfi < 0) {
                if (prio > BCM_PRIO_MAX) {
                    printk("%s: ERROR: Priority %d exceeds maximum\n",
                           ARG_CMD(a), prio);
                    return CMD_FAIL;
                } else {
                    if ((r = bcm_port_priority_color_set(unit, port, prio,
                                                         color)) < 0) {
                        goto bcm_err;
                    }
                }
            }

        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &port, NULL);
        if (!parseEndOk( a, &pt, &retCode)) {
            return retCode;
        }
        if (!SOC_PORT_VALID(unit,port)) {
            printk("%s: ERROR: Invalid port selection %d\n",
                   ARG_CMD(a), port);
            return CMD_FAIL;
        }

        printk("Color settings for port %s\n", BCM_PORT_NAME(unit, port));
        for (prio = BCM_PRIO_MIN; prio <= BCM_PRIO_MAX; prio++) {
            if ((r = bcm_port_priority_color_get(unit, port, prio,
                                                 &color)) < 0) {
                goto bcm_err;
            }
            printk("Priority %d\t%s\n", prio, diag_parse_color[color]);
        }

        if ((r = bcm_port_cfi_color_get(unit, port, FALSE, &color)) < 0) {
            goto bcm_err;
        }
        printk("No CFI     \t%s\n", diag_parse_color[color]);

        if ((r = bcm_port_cfi_color_get(unit, port, TRUE, &color)) < 0) {
            goto bcm_err;
        }
        printk("CFI        \t%s\n", diag_parse_color[color]);
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "map") == 0) {
        int pkt_prio, int_prio;

        pkt_prio = int_prio = -1;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "PktPrio", PQ_INT|PQ_DFL, 0, &pkt_prio, NULL);
        parse_table_add(&pt, "CFI", PQ_INT|PQ_DFL, 0, &cfi, NULL);
        parse_table_add(&pt, "IntPrio", PQ_INT|PQ_DFL, 0, &int_prio, NULL);
        parse_table_add(&pt, "Color", PQ_MULTI|PQ_DFL, 0,
                        &color_parse, diag_parse_color);

        if (!parseEndOk( a, &pt, &retCode)) {
            return retCode;
        }

        if (!SOC_PORT_VALID(unit,port)) {
            printk("%s: ERROR: Invalid port selection %d\n",
                   ARG_CMD(a), port);
            return CMD_FAIL;
        }

        if (pkt_prio < 0 || cfi < 0 || int_prio < 0) {
            printk("Color map settings for port %s\n", 
                                      BCM_PORT_NAME(unit, port));

            for (prio = BCM_PRIO_MIN; prio <= BCM_PRIO_MAX; prio++) {
                for (cfi = 0; cfi <= 1; cfi++) {
                    if ((r = bcm_port_vlan_priority_map_get(unit, port, 
                              prio, cfi, &int_prio, &color)) < 0) {
                        goto bcm_err; 
                    }
                    printk("Packet Prio=%d, CFI=%d, Internal Prio=%d, "
                           "Color=%s\n",
                            prio, cfi, int_prio, diag_parse_color[color]);
                } 
            }
         } else {
             color = (bcm_color_t) color_parse;
             if ((r = bcm_port_vlan_priority_map_set(unit, port, pkt_prio, cfi,
                                                int_prio, color)) < 0) {
                 goto bcm_err;
             }
         }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "unmap") == 0) {
        int pkt_prio, int_prio;

        pkt_prio = int_prio = -1;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, &port, NULL);
        parse_table_add(&pt, "PktPrio", PQ_INT|PQ_DFL, 0, &pkt_prio, NULL);
        parse_table_add(&pt, "CFI", PQ_INT|PQ_DFL, 0, &cfi, NULL);
        parse_table_add(&pt, "IntPrio", PQ_INT|PQ_DFL, 0, &int_prio, NULL);
        parse_table_add(&pt, "Color", PQ_MULTI|PQ_DFL, 0,
                        &color_parse, diag_parse_color);

        if (!parseEndOk( a, &pt, &retCode)) {
            return retCode;
        }

        if (!SOC_PORT_VALID(unit,port)) {
            printk("%s: ERROR: Invalid port selection %d\n",
                   ARG_CMD(a), port);
            return CMD_FAIL;
        }

        if (pkt_prio < 0 || cfi < 0 || int_prio < 0) {
            printk("Color unmap settings for port %s\n", 
                                      BCM_PORT_NAME(unit, port));

            for (prio = BCM_PRIO_MIN; prio <= BCM_PRIO_MAX; prio++) {
                for (color = bcmColorGreen; 
                     color <= bcmColorRed; 
                     color++) {
                    if ((r = bcm_port_vlan_priority_unmap_get(unit, port, 
                              prio, color, &pkt_prio, &cfi)) < 0) {
                        goto bcm_err; 
                    }
                    printk("Internal Prio=%d, Color=%s, Packet Prio=%d, "
                           "CFI=%d\n",
                            prio, diag_parse_color[color], pkt_prio, cfi);
                } 
            }
         } else {
             color = (bcm_color_t) color_parse;
             if ((r = bcm_port_vlan_priority_unmap_set(unit, port, int_prio, 
                                             color, pkt_prio, cfi)) < 0) {
                 goto bcm_err;
             }
         }

        return CMD_OK;
 
    }

    printk("%s: ERROR: Unknown color subcommand: %s\n",
           ARG_CMD(a), subcmd);

    return CMD_USAGE;

 bcm_err:

    printk("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));

    return CMD_FAIL;
}

#ifdef BCM_XGS3_SWITCH_SUPPORT
/* XAUI BERT Test */

char cmd_xaui_usage[] =
    "Run XAUI BERT.\n"
    "Usages:\n\t"
    "  xaui bert SrcPort=<port> DestPort=<port> Duration=<usec> Verb=0/1\n";

#define XAUI_PREEMPHASIS_MIN  (0)
#define XAUI_PREEMPHASIS_MAX  (15)
#define XAUI_IDRIVER_MIN      (0)
#define XAUI_IDRIVER_MAX      (15)
#define XAUI_EQUALIZER_MIN    (0)
#define XAUI_EQUALIZER_MAX    (7)

typedef struct _xaui_bert_info_s {
    bcm_port_t          src_port;
    bcm_port_t          dst_port;
    soc_xaui_config_t   src_config;
    soc_xaui_config_t   dst_config;
    soc_xaui_config_t   test_config;
    bcm_port_info_t     src_info;
    bcm_port_info_t     dst_info;
    int                 duration;
    int                 linkscan_us;
    int                 verbose;
} _xaui_bert_info_t;

/*
 * Function:
 *      _xaui_bert_counter_check 
 * Purpose:
 *      Check BERT counters after the test. 
 * Parameters:
 *      (IN) unit       - BCM unit number
 *      (IN) test_info - Test configuration 
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
static int
_xaui_bert_counter_check(int unit, _xaui_bert_info_t *test_info) 
{
    bcm_port_t src_port, dst_port;
    uint32 tx_pkt, tx_byte, rx_pkt, rx_byte, bit_err, byte_err, pkt_err;
    int    prbs_lock, lock;

    src_port = test_info->src_port;
    dst_port = test_info->dst_port;

    /* Read Tx counters */
    SOC_IF_ERROR_RETURN
        (soc_xaui_txbert_pkt_count_get(unit, src_port, &tx_pkt));
    SOC_IF_ERROR_RETURN
        (soc_xaui_txbert_byte_count_get(unit, src_port, &tx_byte));

    lock = 1;
    /* Read Rx counters */
    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_pkt_count_get(unit, dst_port, 
                                       &rx_pkt, &prbs_lock));
    lock &= prbs_lock;

    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_byte_count_get(unit, dst_port, 
                                        &rx_byte, &prbs_lock));
    lock &= prbs_lock;

    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_bit_err_count_get(unit, dst_port, 
                                           &bit_err, &prbs_lock));
    lock &= prbs_lock;

    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_byte_err_count_get(unit, dst_port, 
                                            &byte_err, &prbs_lock));
    lock &= prbs_lock;

    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_pkt_err_count_get(unit, dst_port, 
                                           &pkt_err, &prbs_lock));
    lock &= prbs_lock;

    if (test_info->verbose) {
        printk(" %4s->%4s, 0x%08x, 0x%08x, 0x%08x, %s, ", 
               BCM_PORT_NAME(unit, src_port), BCM_PORT_NAME(unit, dst_port),
               tx_byte, rx_byte, bit_err, 
               lock? "       OK": "      !OK");
    }

    /* Check TX/RX counters */
    if ((tx_byte == 0) || (tx_pkt == 0) ||
        (tx_byte != rx_byte) || (tx_pkt != rx_pkt) || !lock) {
        return BCM_E_FAIL;
    }

   /* Check error counters */
    if ((bit_err != 0) || (byte_err != 0) || (pkt_err != 0)) {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _xaui_bert_test 
 * Purpose:
 *      Run BERT test with requested port configuration. 
 * Parameters:
 *      (IN) unit       - BCM unit number
 *      (IN) test_info - Test configuration 
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
static int
_xaui_bert_test(int unit, _xaui_bert_info_t *test_info)
{
    int            result1, result2;
    bcm_port_t     src_port, dst_port;

    src_port = test_info->src_port;
    dst_port = test_info->dst_port;

    BCM_IF_ERROR_RETURN
        (bcm_port_speed_set(unit, src_port, 10000));
    BCM_IF_ERROR_RETURN
        (bcm_port_speed_set(unit, dst_port, 10000));

    BCM_IF_ERROR_RETURN
        (soc_xaui_config_set(unit, src_port, &test_info->test_config));
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_set(unit, dst_port, &test_info->test_config));

    /* Wait up to 0.1 sec for TX PLL lock */ 
    sal_usleep(100000); 

    /* Enable RX BERT on both ports first */
    BCM_IF_ERROR_RETURN(soc_xaui_rxbert_enable(unit, dst_port, TRUE));
    if (src_port != dst_port) {
        BCM_IF_ERROR_RETURN
            (soc_xaui_rxbert_enable(unit, src_port, TRUE));
    }

    /* Enable TX BERT on both ports */
    BCM_IF_ERROR_RETURN(soc_xaui_txbert_enable(unit, src_port, TRUE));
    if (src_port != dst_port) {
        BCM_IF_ERROR_RETURN
            (soc_xaui_txbert_enable(unit, dst_port, TRUE));
    }

    /* Run test for requested duration */
    sal_usleep(test_info->duration);

    /* Disable TX BERT */
    BCM_IF_ERROR_RETURN
       (soc_xaui_txbert_enable(unit, src_port, FALSE));
    if (src_port != dst_port) {
        BCM_IF_ERROR_RETURN
            (soc_xaui_txbert_enable(unit, dst_port, FALSE));
    }

    /* Give enough time to complete tx/rx */ 
    sal_usleep(500);
    result1 = _xaui_bert_counter_check(unit, test_info);
    result2 = _xaui_bert_counter_check(unit, test_info);
    if (BCM_SUCCESS(result1) && BCM_SUCCESS(result2)) {
        printk(" ( P ) "); 
    } else {
        printk(" ( F ) "); 
    }

    if (test_info->verbose) {
        printk("\n");
    }

    /* Disable RX BERT only after reading the counter. 
     * Otherwise, counters always read zero.
     */
    BCM_IF_ERROR_RETURN
        (soc_xaui_rxbert_enable(unit, src_port, FALSE));
    if (src_port != dst_port) {
        BCM_IF_ERROR_RETURN
            (soc_xaui_rxbert_enable(unit, dst_port, FALSE));
    }

    if ((BCM_E_NONE != result1) && (BCM_E_FAIL != result1)) {
        return result1;
    }
    if ((BCM_E_NONE != result2) && (BCM_E_FAIL != result2)) {
        return result2;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _xaui_bert_save_config 
 * Purpose:
 *      Disable linkscan and save current port configuration. 
 * Parameters:
 *      (IN) unit       - BCM unit number
 *      (OUT) test_info - Current port configuration 
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
static int
_xaui_bert_save_config(int unit, _xaui_bert_info_t *test_info) {

    /* If linkscan is enabled, disable linkscan */
    BCM_IF_ERROR_RETURN
        (bcm_linkscan_enable_get(unit, &test_info->linkscan_us));
    if (test_info->linkscan_us != 0) {
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
        /* Give enough time for linkscan task to exit. */
        sal_usleep(test_info->linkscan_us * 5); 
    }

    /* Save current settings */ 
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_get(unit, test_info->src_port, 
                             &test_info->src_config));
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_get(unit, test_info->dst_port, 
                             &test_info->dst_config));

    /* Save original speed settings */
    BCM_IF_ERROR_RETURN
        (bcm_port_info_save(unit, test_info->src_port, &test_info->src_info));
    BCM_IF_ERROR_RETURN
        (bcm_port_info_save(unit, test_info->dst_port, &test_info->dst_info));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _xaui_bert_restore_config 
 * Purpose:
 *      Restore original port configuration. 
 * Parameters:
 *      (IN) unit      - BCM unit number
 *      (IN) test_info - Port configuration to be restored 
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
static int
_xaui_bert_restore_config(int unit, _xaui_bert_info_t *test_info) {
 
    BCM_IF_ERROR_RETURN
        (bcm_port_info_restore(unit, test_info->src_port, 
                               &test_info->src_info));
    BCM_IF_ERROR_RETURN
        (bcm_port_info_restore(unit, test_info->dst_port, 
                               &test_info->dst_info));

#if 0
    /* Restore original configuration */
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_set(unit, test_info->src_port, 
                             &test_info->src_config));
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_set(unit, test_info->dst_port, 
                             &test_info->dst_config));
#endif

    if (test_info->linkscan_us != 0) {
        BCM_IF_ERROR_RETURN
            (bcm_linkscan_enable_set(unit, test_info->linkscan_us));
    }  

    return BCM_E_NONE;
}

char bert_header[] =
     "                                   Equalizer\n"
     "I_Driver     0      1      2      3      4      5      6      7\n";

char bert_header_v[] =
     "\n Preemph, I_Driver, Equalizer,   Src->Des,"
     "    tx_byte,    rx_byte,    bit_err, PRBS Lock,"
     "    Des->Src,    tx_byte,    rx_byte,    bit_err,"
     " PRBS Lock\n";

/*
 * Function:
 *      cmd_xaui 
 * Purpose:
 *      Entry point to XAUI related CLI commands.
 * Parameters:
 *      (IN) unit - BCM unit number
 *      (IN) a    - Arguments for the command 
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
cmd_result_t
cmd_xaui(int unit, args_t *a)
{
    char          *subcmd;
    parse_table_t  pt;
    int            rv;
    cmd_result_t   retCode;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    rv = BCM_E_NONE;
    if (sal_strcasecmp(subcmd, "bert") == 0) {
        uint32            preemphasis, idriver, equalizer;
        _xaui_bert_info_t test_info;

        sal_memset(&test_info, 0, sizeof(test_info));
        test_info.duration  = 10;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "SrcPort", PQ_DFL|PQ_PORT, 0, 
                        &test_info.src_port, NULL);
        parse_table_add(&pt, "DestPort", PQ_DFL|PQ_PORT, 0, 
                        &test_info.dst_port, NULL);
        parse_table_add(&pt, "Duration", PQ_INT|PQ_DFL, 0, 
                        &test_info.duration, NULL);
        parse_table_add(&pt, "Verbose", PQ_BOOL|PQ_DFL, 0, 
                        &test_info.verbose, NULL);

        if (!parseEndOk( a, &pt, &retCode)) {
            return retCode;
        }

        /* Run only on HG and XE port */
        if ((!IS_HG_PORT(unit, test_info.src_port) && 
             !IS_XE_PORT(unit, test_info.src_port)) || 
            (!IS_HG_PORT(unit, test_info.dst_port) && 
             !IS_XE_PORT(unit, test_info.dst_port))) {
            printk("%s: ERROR: Invalid port selection %d, %d\n",
                   ARG_CMD(a), test_info.src_port, test_info.dst_port);
            return CMD_FAIL;
        }
        
        if ((rv = _xaui_bert_save_config(unit, &test_info)) < 0) {
            goto cmd_xaui_err;
        }

        test_info.test_config = test_info.src_config;
        for (preemphasis = XAUI_PREEMPHASIS_MIN; 
             preemphasis <= XAUI_PREEMPHASIS_MAX;
             preemphasis++) {
            test_info.test_config.preemphasis = preemphasis;
             
            if (!test_info.verbose) {
                printk("\nPreemphasis = %d\n", preemphasis);
            } 

            printk("%s", test_info.verbose? bert_header_v: bert_header);

            for (idriver = XAUI_IDRIVER_MIN;
                 idriver <= XAUI_IDRIVER_MAX;
                 idriver++) {
                test_info.test_config.idriver = idriver;
                if (!test_info.verbose) {
                    printk("%8d  ", idriver);
                }
                for (equalizer = XAUI_EQUALIZER_MIN;
                     equalizer <= XAUI_EQUALIZER_MAX;
                     equalizer++) {
                   
                    if (test_info.verbose) {
                        printk("%8d, %8d, %9d,", preemphasis, idriver,
                               equalizer);
                    }

                    test_info.test_config.equalizer_ctrl = equalizer;
                    if ((rv = _xaui_bert_test(unit, &test_info)) < 0) {
                        _xaui_bert_restore_config(unit, &test_info);
                         goto cmd_xaui_err;
                    }
                }
                printk("\n");
            }
        }

        if ((rv = _xaui_bert_restore_config(unit, &test_info)) < 0) {
            goto cmd_xaui_err;
        }
        return CMD_OK;
    }
    printk("%s: ERROR: Unknown xaui subcommand: %s\n",
           ARG_CMD(a), subcmd);
    return CMD_USAGE;

cmd_xaui_err:
    printk("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
    return CMD_FAIL;
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */
