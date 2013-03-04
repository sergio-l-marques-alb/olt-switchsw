/*
 * $Id: ledproc.c 1.24.20.2 Broadcom SDK $
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
 * File: 	ledproc.c
 * Purpose: 	LED Processor Support
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm/link.h>
#include <soc/types.h>
#include <appl/diag/system.h>

#ifdef BCM_TRIDENT_SUPPORT
#define TRIDENT_LEDUP_MAX              2
#define TRIDENT_LEDUP0_PORT_MAX        36
#endif

typedef struct led_info_s {
    uint32 ctrl;
    uint32 status;
    uint32 pram_base;
    uint32 dram_base;
} led_info_t;

static led_info_t led_info_gen[] = {
    {CMIC_LED_CTRL,CMIC_LED_STATUS,CMIC_LED_PROGRAM_RAM_BASE,
     CMIC_LED_DATA_RAM_BASE}
};

#ifdef BCM_TRIDENT_SUPPORT
static led_info_t led_info_tri[] = {
    {CMICE_LEDUP0_CTRL,CMICE_LEDUP0_STATUS,CMICE_LEDUP0_PROGRAM_RAM_BASE,
     CMICE_LEDUP0_DATA_RAM_BASE},
    {CMICE_LEDUP1_CTRL,CMICE_LEDUP1_STATUS,CMICE_LEDUP1_PROGRAM_RAM_BASE,
     CMICE_LEDUP1_DATA_RAM_BASE}
};
#endif
#ifdef BCM_KATANA_SUPPORT
static led_info_t led_info_kt[] = {
    {CMIC_LEDUP0_CTRL,CMIC_LEDUP0_STATUS,CMIC_LEDUP0_PROGRAM_RAM_BASE,
     CMIC_LEDUP0_DATA_RAM_BASE},
    {CMIC_LEDUP0_CTRL,CMIC_LEDUP0_STATUS,CMIC_LEDUP0_PROGRAM_RAM_BASE,
     CMIC_LEDUP0_DATA_RAM_BASE} /* added to remove coverity error */
};
#endif
/*
 * Function:	ledproc_load
 * Purpose:	Load a program into the LED microprocessor from a buffer
 * Parameters:	unit - StrataSwitch unit #
 *		program - Array of up to 256 program bytes
 *		bytes - Number of bytes in array
 * Notes:	Also clears the LED processor data RAM from 0x80-0xff
 *		so the LED program has a known state at startup.
 */

STATIC void
ledproc_load(int unit, uint8 *program, int bytes, led_info_t *led_ptr)
{
    int		offset;

    for (offset = 0; offset < CMIC_LED_PROGRAM_RAM_SIZE; offset++) {
	soc_pci_write(unit,
		      led_ptr->pram_base + CMIC_LED_REG_SIZE * offset,
		      (offset < bytes) ? (uint32) program[offset] : 0);
    }

    for (offset = 0x80; offset < CMIC_LED_DATA_RAM_SIZE; offset++) {
	soc_pci_write(unit,
		      led_ptr->dram_base + CMIC_LED_REG_SIZE * offset,
		      0);
    }
}

#ifndef NO_FILEIO
STATIC cmd_result_t
ledproc_load_fp(int unit, char *cmd, char *file, FILE *f, 
                led_info_t *led_ptr)
/*
 * Function:	ledproc_load_fp
 * Purpose:	Load a program into the LED microprocessor from a file
 * Parameters:	unit - StrataSwitch unit #
 *		cmd - Name of command for error printing
 *		file - Name of file for error printing
 *		f - Open stdio file file pointer containing program
 * Returns:	CMD_XXX
 *
 * Notes:	File format (ASCII FILE)
 *	00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
 *	10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
 *	... for a total of 256 bytes, white space ignored.
 */
{
    uint8	program[CMIC_LED_PROGRAM_RAM_SIZE];
    char        input[256], *cp = NULL, *error = NULL;
    int         bytes = 0, line = 0;
    int         offset_size = 0;

    while (!error && (cp = fgets(input, sizeof(input) - 1, f))) {
        line++;

        while (*cp) {
            if (isspace((unsigned)(*cp))) { /* Skip whitespace */
                cp++;
            } else {
                if (!isxdigit((unsigned)*cp) ||
                    !isxdigit((unsigned)*(cp+1))) {
                    error = "Invalid hex digit";
                    break;
                }
                offset_size = sizeof(program);
                if (bytes >= offset_size) {
                    error = "Program memory exceeded";
                    break;
                }
                program[bytes++] = (xdigit2i(*cp) << 4) | xdigit2i(*(cp + 1));
                cp += 2;
            }
        }
    }

    if (error) {
        printk("%s: Error loading file %s line %d: %s\n",
               cmd, file, line, error);
        return(CMD_FAIL);
    }

    printk("%s: Loading %d bytes into LED program memory\n",
           cmd, bytes);

    ledproc_load(unit, program, bytes,led_ptr);

    return(CMD_OK);
}
#endif /* NO_FILEIO */
/*
 * Function:	ledproc_load_args
 * Purpose:	Load a program into the LED microprocessor from an args_t
 * Parameters:	unit - StrataSwitch unit #
 *		a - Remainder of command line containing raw hex data
 * Returns:	CMD_XXX
 * Notes:	Input strings are concatenated and parsed the
 *		same way as ledproc_load_fp.
 */
STATIC cmd_result_t
ledproc_load_args(int unit, args_t *a, led_info_t *led_ptr)
/*
 * Notes:	Args format
 *	00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
 *	10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
 *	...
 * White space optional.
 */
{
    uint8	program[CMIC_LED_PROGRAM_RAM_SIZE];
    int		bytes = 0;
    char	*cp;
    int         offset_size = 0;

    memset(program, 0, sizeof (program));

    while ((cp = ARG_GET(a)) != NULL) {
	while (*cp) {
	    if (isspace((int)(*cp))) {
		cp++;
	    } else {
		if (!isxdigit((unsigned)*cp) ||
		    !isxdigit((unsigned)*(cp+1))) {
		    printk("%s: Invalid character\n", ARG_CMD(a));
		    return(CMD_FAIL);
		}

                offset_size = sizeof(program);
		if (bytes >= offset_size) {
		    printk("%s: Program memory exceeded\n", ARG_CMD(a));
		    return(CMD_FAIL);
		}
		program[bytes++] = (xdigit2i(*cp) << 4) | xdigit2i(*(cp + 1));
		cp += 2;
	    }
	}
    }

    ledproc_load(unit, program, bytes,led_ptr);

    return(CMD_OK);
}

/*
 * Function: 	ledproc_linkscan_cb
 * Purpose:	Call back function for LEDs on link change.
 * Parameters:	unit - unit number
 *              port - callback from this port
 *              info - pointer to structure giving status
 * Returns:	nothing
 * Notes:	Each port has one byte of data at address (0x80 + portnum).
 *		In each byte, bit 0 is used for link status.
 *		In each byte, bit 7 is used for turbo mode indication.
 */
STATIC void
ledproc_linkscan_cb(int unit, soc_port_t port, bcm_port_info_t *info)
{
    uint32	portdata;
    int		byte = LS_LED_DATA_OFFSET + port;
    led_info_t *led_info;
    int led_ix;

    led_info = led_info_gen;
    led_ix = 0;

#ifdef BCM_TUCANA_SUPPORT
    if (SOC_IS_TUCANA(unit)) {
        byte = LS_TUCANA_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_RAVEN_SUPPORT
    if (SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        byte = LS_RAVEN_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_RAPTOR_SUPPORT
    if (SOC_IS_RAPTOR(unit)) {
        byte = LS_RAPTOR_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_IS_TRIUMPH(unit)) {
        byte = LS_TR_VL_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_VALKYRIE_SUPPORT
    if (SOC_IS_VALKYRIE(unit)) {
        byte = LS_TR_VL_LED_DATA_OFFSET + port;
    }
#endif

#ifdef BCM_SCORPION_SUPPORT
    if (SOC_IS_SC_CQ(unit)) {
        byte = LS_SC_CQ_LED_DATA_OFFSET + port;
    }
#endif

    if (soc_feature(unit, soc_feature_led_data_offset_a0)) {
        byte = LS_LED_DATA_OFFSET_A0 + port;
    }

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        soc_info_t *si;
        int phy_port;  /* physical port number */

        led_info = led_info_tri;
        si = &SOC_INFO(unit);
        phy_port = si->port_l2p_mapping[port];

        /* trident first 36 ports in ledproc0, the other 36 ports in ledproc1*/
        if (phy_port > TRIDENT_LEDUP0_PORT_MAX) {
            phy_port -= TRIDENT_LEDUP0_PORT_MAX;
            led_ix = 1;
        }
        byte = LS_LED_DATA_OFFSET + phy_port;
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        led_info = led_info_kt;
    }
#endif

    portdata = soc_pci_read(unit, 
                     led_info[led_ix].dram_base + CMIC_LED_REG_SIZE * byte);

    if (info->linkstatus) {
        portdata |= 0x01;
    } else {
        portdata &= ~0x01;
    }

    portdata &= ~0x80;

    soc_pci_write(unit, led_info[led_ix].dram_base + CMIC_LED_REG_SIZE * byte,
                      portdata);
}

char ledproc_usage[] =
    "Parameters: [num] [status | start | stop | load <file.hex> |\n\t"
    "            [auto [on | off]] | prog <hexdata> | dump\n\t"
    "Load the LED Microprocessor with the program specified in <file.hex>.\n\t"
    "Option 'auto' turns on (by default) or off automatic linkscan\n\t"
    "    updates to the LED processor.\n\t"
    "Option 'num' selects the specific LED processor. The default is 0\n";

cmd_result_t
ledproc_cmd(int unit, args_t *a)
/*
 * Function: 	sh_led
 * Purpose:	Load a led program and start it.
 * Parameters:	unit - unit
 *		a - args, each of the files to be displayed.
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    VOL cmd_result_t		rv = CMD_OK;
#ifndef NO_FILEIO
    jmp_buf			ctrl_c;
    FILE * volatile		f = NULL;
#endif
    char			*c;
    volatile uint32		led_ctrl, led_status;
    volatile int		auto_on;
    int led_max;
    led_info_t *led_info;
    led_info_t *led_info_cur;
    int ix;

    if (!sh_check_attached("ledproc", unit)) {
	return(CMD_FAIL);
    }

    if (!soc_feature(unit, soc_feature_led_proc)) {
	printk("%s: LED Micro-controller not available on %s\n",
	       ARG_CMD(a),
	       soc_dev_name(unit));
	return(CMD_FAIL);
    }

    led_info = led_info_gen;
    led_max = 1;
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        led_info = led_info_tri;
        led_max = TRIDENT_LEDUP_MAX;
    }
#endif
#ifdef BCM_KATANA_SUPPORT
    if (SOC_IS_KATANA(unit)) {
        led_info = led_info_kt;
    }
#endif

    if (bcm_linkscan_unregister(unit, ledproc_linkscan_cb) < 0) {
	auto_on = FALSE;
    } else {
	(void)bcm_linkscan_register(unit, ledproc_linkscan_cb);
	auto_on = TRUE;
    }

    if (ARG_CNT(a) == 0) {
        for (ix = 0; ix < led_max; ix++) {
            led_ctrl = soc_pci_read(unit, led_info[ix].ctrl);
            led_status = soc_pci_read(unit, led_info[ix].status);
	    printk("%s: LED%d Micro-controller is %s (PC=0x%x State=%s%s); "
	       "auto updating is %s.\n",
	       ARG_CMD(a),
               ix,
	       (led_ctrl & LC_LED_ENABLE) ? "ON" : "OFF",
	       (led_status & LS_LED_PC),
	       (led_status & LS_LED_INIT) ? "INIT," : "",
	       (led_status & LS_LED_RUN) ? "RUN" : "SLEEP",
	       auto_on ? "ON" : "OFF");
        }
	return(CMD_OK);
    }

    ix = 0;
    c = ARG_GET(a);

    /* check if command has processor number specified */
    if (isint(c)) {
        ix = parse_integer(c);
        if (ix > (led_max - 1)) {
	    printk("Invalid uProcessor number: %d, set to 0\n",ix); 
            ix = led_max - 1;
        }
        c = ARG_GET(a);
    }

    led_info_cur = led_info+ix;
    led_ctrl = soc_pci_read(unit, led_info_cur->ctrl);

    if (!sal_strcasecmp(c, "status")) {
        int     port;
        uint32	portdata;
        int phy_port;

        /* coverity[overrun-local] */
        PBMP_PORT_ITER(unit, port) {
            if (soc_feature(unit, soc_feature_logical_port_num)) {
                if (port >= SOC_MAX_NUM_PORTS) { /* safety check */
                    break;
                }
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            } else {
                phy_port = port;
            }
#ifdef BCM_TRIDENT_SUPPORT
           if (SOC_IS_TD_TT(unit)) {
               /* trident first 36 ports in ledproc0, the other 36 ports in ledproc1*/
               if (phy_port > TRIDENT_LEDUP0_PORT_MAX) {
                   phy_port -= TRIDENT_LEDUP0_PORT_MAX;
                   led_info_cur = led_info+1;
               } else {
                   led_info_cur = led_info;
               }
            }
#endif

            portdata = soc_pci_read(unit, 
                     led_info_cur->dram_base + CMIC_LED_REG_SIZE * (phy_port * 2 + 1)); 
            portdata &= 0xFF;
            portdata <<= 8;
            portdata |= 0xFF & soc_pci_read(unit, 
                         led_info_cur->dram_base + CMIC_LED_REG_SIZE * (phy_port * 2));
#ifdef BCM_TRIDENT_SUPPORT
            if (SOC_IS_TD_TT(unit)) {
                uint32 byte;
                uint32 portdata1;

                byte = LS_LED_DATA_OFFSET + phy_port;
                /* read tx/rx activity */
                portdata1 = soc_pci_read(unit, 
                     led_info_cur->dram_base + CMIC_LED_REG_SIZE * byte); 
                if (portdata1 & 0x10) {
                    portdata |= 0x2;
                } else {
                    portdata &= ~0x2;
                }
                if (portdata1 & 0x20) {
                    portdata |= 0x1;
                } else {
                    portdata &= ~0x1;
                }
            }
#endif
            if (soc_feature(unit, soc_feature_unimac) &&
                ((IS_FE_PORT(unit, port)) || (IS_GE_PORT(unit, port)))) {
                printk("%2d %4s %s %s %s %s %s %s %s %s\n",
                        port,
                        SOC_PORT_VALID(unit, port) ?
                                SOC_PORT_NAME(unit, port) :"    ",
                        (portdata & 0x0001) ? "RX"  : "  " ,
                        (portdata & 0x0002) ? "TX"  : "  " ,
                        (portdata & 0x0080) ? "LI"  : "  " ,
                        (portdata & 0x0100) ? "LE"  : "  " ,
                        (portdata & 0x0004) ? "COL" : "   ",
                        (portdata & 0x0008) ? "100" : "   ",
                        (portdata & 0x0010) ? "1G"  : "  " ,
                        (portdata & 0x0020) ? "HD"  : "FD"
                        );

            } else {
                printk("%2d %4s %s %s %s %s %s %s %s %s\n",
                        port,
                        SOC_PORT_VALID(unit, port) ?
                                SOC_PORT_NAME(unit, port) :"    ",
                        (portdata & 0x0001) ? "RX"  : "  " ,
                        (portdata & 0x0002) ? "TX"  : "  " ,
                        (portdata & 0x0080) ? "LI"  : "  " ,
                        (portdata & 0x0100) ? "LE"  : "  " ,
                        (portdata & 0x0004) ? "COL" : "   ",
                        (portdata & 0x0008) ? "100" : "   ",
                        (portdata & 0x0010) ? "1G"  : "  " ,
                        (portdata & 0x0020) ? "FD"  : "HD"
                        );

            }
        }
    } else if (!sal_strcasecmp(c, "start")) {
	int		offset;

	/* The LED data area should be clear whenever program starts */

	soc_pci_write(unit, led_info_cur->ctrl, led_ctrl & ~LC_LED_ENABLE);

	for (offset = 0x80; offset < CMIC_LED_DATA_RAM_SIZE; offset++) {
	    soc_pci_write(unit,
			  led_info_cur->dram_base + CMIC_LED_REG_SIZE * offset,
			  0);
	}

	led_ctrl |= LC_LED_ENABLE;

	soc_pci_write(unit, led_info_cur->ctrl, led_ctrl);

	if (auto_on) {
	    (void)bcm_link_change(unit, PBMP_PORT_ALL(unit));
	}

	return(CMD_OK);
    } else if (!sal_strcasecmp(c, "stop")) {

	led_ctrl &= ~LC_LED_ENABLE;
	soc_pci_write(unit, led_info_cur->ctrl, led_ctrl);

	return(CMD_OK);
    } else if (!sal_strcasecmp(c, "load")) {
        led_info_t *led_info_saved;
        led_info_saved = led_info_cur;
	/*
	 * Try to catch ^C to avoid leaving file open if ^C'd.
	 * There are still a number of unlikely race conditions here.
	 * Temporarily stop LED processor if it is currently running.
	 */
#ifdef NO_FILEIO
	printk("no filesystem\n");
#else	
	soc_pci_write(unit, led_info_saved->ctrl, led_ctrl & ~LC_LED_ENABLE);
	if (!setjmp(ctrl_c)) {
	    sh_push_ctrl_c(&ctrl_c);

	    if ((c = ARG_GET(a)) != NULL) {
		f = sal_fopen(c, "r");
		if (!f) {
		    printk("%s: Error: Unable to open file: %s\n",
			   ARG_CMD(a), c);
		    rv = CMD_FAIL;
		} else {
		    rv = ledproc_load_fp(unit, ARG_CMD(a), c, 
                                (FILE *)f,led_info_saved);
		    sal_fclose((FILE *)f); /* Cast for un-volatile */
		    f = NULL;
		}
	    } else {
		printk("%s: Error: No file specified\n", ARG_CMD(a));
		rv = CMD_USAGE;
	    }
	} else if (f) {
	    sal_fclose((FILE *)f);
	    f = NULL;
	    rv = CMD_INTR;
	}
	/* Restore LED processor if it was running */
	soc_pci_write(unit, led_info_saved->ctrl, led_ctrl);

	if (auto_on) {
	    (void)bcm_link_change(unit, PBMP_PORT_ALL(unit));
	}

	sh_pop_ctrl_c();
#endif /* NO_FILEIO */
    } else if (!sal_strcasecmp(c, "prog")) {
	if (ARG_CUR(a) == NULL) {
	    return(CMD_USAGE);
	}

	led_ctrl &= ~LC_LED_ENABLE;
	soc_pci_write(unit, led_info_cur->ctrl, led_ctrl);

	return ledproc_load_args(unit, a, led_info_cur);
    } else if (!sal_strcasecmp(c, "auto")) {
	int new_auto = 1;

	if ((c = ARG_GET(a)) != NULL) {
	    if (!sal_strcasecmp(c, "off")) {
		new_auto = 0;
	    } else if (!sal_strcasecmp(c, "on")) {
		new_auto = 1;
	    } else {
		return CMD_USAGE;
	    }
	}

	if (new_auto && !auto_on) {
	    (void)bcm_linkscan_register(unit, ledproc_linkscan_cb);
	}

	if (!new_auto && auto_on) {
	    (void)bcm_linkscan_unregister(unit, ledproc_linkscan_cb);
	}

	if (new_auto) {
	    (void)bcm_link_change(unit, PBMP_PORT_ALL(unit));
	}
    } else if (!sal_strcasecmp(c, "dump")) {
	int		offset, max;

	for (max = CMIC_LED_PROGRAM_RAM_SIZE - 1; max >= 0; max--) {
	    if (soc_pci_read(unit, 
                       led_info_cur->pram_base + CMIC_LED_REG_SIZE * max) != 0) {
		break;
	    }
	}

	for (offset = 0; offset <= max; offset++) {
	    printk(" %02x", soc_pci_read(unit, 
                          led_info_cur->pram_base + CMIC_LED_REG_SIZE * offset));
	    if ((offset % 16) == 15) {
		printk("\n");
	    }
	}

	if (offset % 16 != 0) {
	    printk("\n");
	}
    } else {
	return(CMD_USAGE);
    }

    return(rv);
}
