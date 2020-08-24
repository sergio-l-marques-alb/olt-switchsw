/*
 * $Id: hla.c,v 1.0 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File: 	hla.c
 * Purpose: 	Hardware License Authenticator(HLA) command Support
 */

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>

#if defined(BCM_ESW_SUPPORT) && defined(BCM_HLA_SUPPORT)

#include <shared/hla/hla.h>

#define HLA_LIC_FILENAME_SIZE            (256)

char hla_usage[] =
    "Parameters: <command> [<address>] [<file>]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tcommand = [load/status]\n"
    "\tfile    = License (CCF) binary file name\n"
#endif
    ;

cmd_result_t
hla_cmd(int unit, args_t *a)
/*
 * Function: 	hla_cmd
 * Purpose:	Load feature license file (CCF file from BP3 server) into 
 *              HLA Cortex-M0 SRAM.
 *              Displays HLA current status.
 * Parameters:	unit - unit
 *		a - args, sub_command and license file name, including path
 * Returns:	CMD_OK/CMD_FAIL/CMD_USAGE
 */
{
    cmd_result_t rv = CMD_OK;
    char *c;
    uint32 fw_ver;
    hla_status_t hla_status;

#ifndef NO_FILEIO
    uint8 *buf = NULL;
    int fsize = 0, size = 0;
    uint32 max_allowed_len;
    char filename[HLA_LIC_FILENAME_SIZE] = {0};
    FILE * volatile fp = NULL;
#ifndef NO_CTRL_C
    jmp_buf ctrl_c;
#endif
#endif

    /* check for simulation*/
    if (SAL_BOOT_BCMSIM) {
        return(rv);
    }

    if (!soc_feature(unit, soc_feature_cmicx)) {
        return (CMD_FAIL);
    }

    if (!sh_check_attached("hla", unit)) {
        return(CMD_FAIL);
    }

    if (!ARG_CNT(a)) {
        return(CMD_USAGE);
    }

    c = ARG_GET(a);

    if (!sal_strcasecmp(c, "load")) {
#ifdef NO_FILEIO
        cli_out("no filesystem\n");
        rv = CMD_FAIL;
#else
        c = ARG_GET(a);

        if (c == NULL) {
            cli_out("%s: Error: No file specified\n", ARG_CMD(a));
            return(CMD_USAGE);
        }

        if (sal_strlen(c) > HLA_LIC_FILENAME_SIZE) {
            cli_out("Filename cannot be longer than %d\n", HLA_LIC_FILENAME_SIZE);
            return(CMD_FAIL);
        }

        sal_strncpy(filename, c, sal_strlen(c));

        cli_out("Loading M0 Firmware located at %s\n", filename);

#ifndef NO_CTRL_C
        if (!setjmp(ctrl_c)) {
             sh_push_ctrl_c(&ctrl_c);
#endif
             /* See if we can open the file before doing anything else */
            fp = sal_fopen(filename, "rb");
            if (!fp) {
                cli_out("%s: Error: Unable to open file: %s\n",
                        ARG_CMD(a), filename);
                rv = CMD_FAIL;
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return rv;
            }

            fsize = sal_fsize(fp);

            if ((fsize <= 0) || ((fsize + sizeof(scha_hdr_t)) > HLA_AUTH_DATA_SIZE_MAX)) {
                max_allowed_len = HLA_AUTH_DATA_SIZE_MAX - sizeof(scha_hdr_t);
                cli_out("%s: Error: file size is not valid, size = %d, max allowed = %u\n",
                        ARG_CMD(a), fsize, max_allowed_len);
                sal_fclose((FILE *)fp);
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return CMD_FAIL;
            }

            buf = sal_alloc(fsize, "HLA License");
            if (buf == NULL) {
                cli_out("%s: Memory allocate fail\n", ARG_CMD(a));
                sal_fclose((FILE *)fp);
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return CMD_FAIL;
            }

            size = sal_fread(buf, 1, fsize, fp);
            if (size != fsize) {
                cli_out("%s: Reading file %s has failed: expected: %d, read %d \
                                        \n", ARG_CMD(a),filename, fsize, size);
                /* coverity[tainted_data] */
                sal_free(buf);
                sal_fclose((FILE *)fp);
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return CMD_FAIL;
            }


            rv = soc_hla_ccf_load(unit, buf, size);

            if (SOC_FAILURE(rv)) {
                cli_out("%s: Error: License file loading failed\n", ARG_CMD(a));
                /* coverity[tainted_data] */
                sal_free(buf);
                sal_fclose((FILE *)fp);
                fp = NULL;
                buf = NULL;
#ifndef NO_CTRL_C
                sh_pop_ctrl_c();
#endif
                return CMD_FAIL;
            }

            /* coverity[tainted_data] */
            sal_free(buf);
            sal_fclose((FILE *)fp);
            fp = NULL;
            buf = NULL;

#ifndef NO_CTRL_C
        } else if (fp) {
            sal_fclose((FILE *)fp);
            fp = NULL;
            rv = CMD_INTR;
        }
        sh_pop_ctrl_c();
#endif
#endif /* NO_FILEIO */
        sal_usleep(10000);
    } else if (!sal_strcasecmp(c, "status")) {
        rv = soc_hla_status(unit, &hla_status);
        if (SOC_FAILURE(rv)) {
            cli_out("%s: Error: Reading HLA status failed\n", ARG_CMD(a));
            return CMD_FAIL;
        }
        /* Print status */
        cli_out("HLA Firmware:\n");
        fw_ver = hla_status.fw_ver;
        cli_out("\tVersion: %s%d.%d\n", ((fw_ver >> 24) & 0xff) ? "E" :"", (fw_ver >> 16) & 0xff, fw_ver & 0xffff);
        cli_out("\tCore State: %s\n", (hla_status.reset) ? "reset" : "running");
        cli_out("\tFirmware State: %s\n", (hla_status.busy) ? "busy" : "waiting");
        cli_out("\tCurrent run: %d\n", hla_status.cur_run);
        cli_out("\tCurrent run state: 0x%08x\n", hla_status.cur_state);
        cli_out("\tCurrent run state: 0x%08x\n", hla_status.prev_state);
        cli_out("\tError: 0x%08x\n", hla_status.error);
        cli_out("\tSecondary Error: 0x%08x\n", hla_status.error2);
        cli_out("\tChipID: 0x%08x:0x%08x\n", hla_status.chipid_h,hla_status.chipid_l);
    }

    return(rv);
}
#endif /* BCM_ESW_SUPPORT && BCM_HLA_SUPPORT */
