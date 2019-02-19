/*
 * $Id: pcie.c,v 1.00 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * 
 * Purpose: PCIe Diagnostics utilities
 */
#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT) || (defined(BCM_ESW_SUPPORT) && defined(PCIEG3_DIAG_SUPPORT))
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <sal/compiler.h>
#include <sal/core/thread.h>
#include <sal/core/libc.h>
#include <soc/error.h>
#include <soc/cm.h>
#include <soc/iproc.h>
#include <soc/mcm/memregs.h>
#include <shared/bsl.h>
#include <sdk_config.h>
#include <soc/soc_flash.h>
#include <pcig3_phy_acc.h>

# define SPI_SPEED    62500000 /* BSPI */
# define SPI_MODE     3 /* (CPOL|CPHA) */

#define SWAP_ENDIAN(x) ( \
    (((x) << 24) & 0xff000000) | \
    (((x) <<  8) & 0x00ff0000) | \
    (((x) >>  8) & 0x0000ff00) | \
    (((x) >> 24) & 0x000000ff))

#ifndef NO_FILEIO
static int
_pcie_phy_fw_flash_init(int unit,
                        soc_flash_conf_t *flash)
{
    unsigned int speed = SPI_SPEED;
    unsigned int mode = SPI_MODE;
    int rv = SOC_E_NONE;

    rv = soc_flash_init(unit, speed, mode, flash);
    if (SOC_FAILURE(rv)) {
        cli_out("Error: Init SPI flash speed = %u, mode =%u\n", speed, mode);
    }
    return rv;
}

STATIC int
_pcie_phy_fw_write_block(int unit,
                         uint32 offset,
                         size_t len,
                         const uint8 *buf)
{
    int rv = SOC_E_NONE;

    rv = soc_flash_erase(unit, offset, len);

    if (SOC_SUCCESS(rv)) {
        rv = soc_flash_write(unit, offset, len, buf);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to write = 0x%x\n", offset);
        }
    } else {
        cli_out("Failed to erase = 0x%x\n", offset);
    }

    return rv;
}
#endif

STATIC cmd_result_t
_pcie_phy_fw_load(int unit, args_t *args)
{
#ifndef NO_FILEIO
    int rv = SOC_E_NONE;
    char *file_str;
    long len;
    uint32 offset = 0;
    uint8 *wbuf = NULL;
    uint8 *rbuf = NULL;
    soc_flash_conf_t flash;
    FILE *in = NULL;

    /* Parse command */
    if ((file_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    /* initialize the flash driver */
    rv = _pcie_phy_fw_flash_init(unit, &flash);
    if (SOC_FAILURE(rv)) {
        goto error_init;
    }
    wbuf = sal_alloc(flash.sector_size, "FLASH_W");
    if (!wbuf) {
        rv = SOC_E_MEMORY;
        goto error;
    }

    rbuf = sal_alloc(flash.sector_size, "FLASH_R");
    if (!rbuf) {
        rv = SOC_E_MEMORY;
        goto error;
    }
    /* Open file and get the file size */
    cli_out("Opening file: %s\n", file_str);

    in = sal_fopen(file_str, "r");
    if (!in) {
        /* failure to open existing file is error */
        rv = _SHR_E_EXISTS;
        goto error;
    }

    sal_fseek(in, 0, SEEK_END);
    if((len = sal_ftell(in)) == -1) {
        rv = _SHR_E_INTERNAL;
        goto error;
    }
    if ((sal_fseek(in, 0, SEEK_SET) != 0) || (len == 0)) {
        rv = _SHR_E_INTERNAL;
        goto error;
    }
    cli_out("Updating PCIE firmware\n");
    while (len) {
        size_t size = (flash.sector_size > len) ? len : flash.sector_size;
        int i;
        /* Read the sector data */
        if (sal_fread(wbuf, sizeof(uint8), size, in) != size) {
            rv = _SHR_E_INTERNAL;
            goto error;
        }

        rv = soc_flash_cmd_write_status(unit, 0);
        if (SOC_FAILURE(rv)) {
            cli_out("Failed to unlock all sectors\n");
            break;
        }
        rv = _pcie_phy_fw_write_block(unit, offset, size, wbuf);
        if (SOC_FAILURE(rv)) {
            break;
        }

        /* Read back to verify */
        rv = soc_flash_read(unit, offset, size, rbuf);
        if (SOC_FAILURE(rv)) {
            break;
        }
        for (i = 0; i < size; i++) {
            if (wbuf[i] != rbuf[i]) {
                cli_out("Data Mismatch: offset= 0x%x, write = 0x%x, read = 0x%x\n",
                        i, wbuf[i], rbuf[i]);
                rv = SOC_E_FAIL;
                break;
            }
        }
        len = len - size;
        offset += size;
    }
    if (SOC_SUCCESS(rv)) {
        cli_out("PCIE firmware updated successfully. Please reset the system... \n");
    }
error:
    /*  Close the driver */
    soc_flash_cleanup(unit);
    if (in) {
        sal_fclose(in);
    }
error_init:
    if (rbuf) {
        sal_free(rbuf);
    }
    if (wbuf) {
        sal_free(wbuf);
    }
    if (SOC_FAILURE(rv)) {
        return CMD_FAIL;
    } else {
        return CMD_OK;
    }
#else
    cli_out("File system not supported\n");
    return CMD_FAIL;
#endif
}

STATIC cmd_result_t
_pcie_phy_fw_dump(int unit)
{
    cli_out("Command NOT supported in this release\n");
    return CMD_OK;
}

#define PCIE_G3_FW_MAGIC_NUM  (0x50434549)
#define PCIE_GE_FW_HDR_OFFSET (0x2000)

typedef struct soc_pcieg3_fw_hdr_s{
    uint32 magic; /* Magic number to verify if FW is programmed into flash */
    uint32 ldrver; /* FW loader version, major version: bits[31-16], minor version: bits[15-0] */
    uint32 size; /* FW image size */
    uint32 crc;  /* FW image CRC */
    char   version[12]; /* FW version string */
}soc_pcieg3_fw_hdr_t;

STATIC cmd_result_t
_pcie_phy_fw_version(int unit)
{
    soc_pcieg3_fw_hdr_t fwhdr;
    uint32 size, saddr, *daddr;
    uint32 endian = 1;
    int i;

    if(*(uint8 *)(&endian) == 1)
        endian = 0;
    saddr = (IPROC14_QSPI_MEM_ADDR + PCIE_GE_FW_HDR_OFFSET);
    daddr = (uint32 *) &fwhdr;
    size = sizeof(soc_pcieg3_fw_hdr_t) / 4;
    for(i = 0; i < size; i++) {
       *daddr = soc_cm_iproc_read(unit, saddr);
       saddr += 4;
       daddr++;
    }

    if(endian) {
       size  = sizeof(fwhdr.version) / 4;
       daddr = (uint32 *) (&fwhdr.version[0]);
       for(i = 0; i < size; i++) {
          *daddr = SWAP_ENDIAN(*daddr);
          daddr++;
       }
    }
    if(fwhdr.magic == PCIE_G3_FW_MAGIC_NUM) {
        cli_out("\tPCIe FW loader version: %d.%d\n\tPCIe FW version: %s\n",
                                          fwhdr.ldrver >>16,
                                          fwhdr.ldrver & 0xffff,
                                          fwhdr.version);
    }
    else {
        cli_out("Valid firmware not found\n");
    }
    return CMD_OK;
}

STATIC cmd_result_t
_pcie_phy_fw(int unit, args_t *args)
{
    int rv = CMD_OK;
    char *cmd_str;

    /* Parse command */
    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(cmd_str, "load") == 0) {
        rv = _pcie_phy_fw_load(unit, args);
    } else if (sal_strcasecmp(cmd_str, "dump") == 0) {
        rv = _pcie_phy_fw_dump(unit);
    } else if (sal_strcasecmp(cmd_str, "version") == 0) {
        rv = _pcie_phy_fw_version(unit);
    }
    else {
        rv = CMD_FAIL;
    }

    return rv;
}

STATIC cmd_result_t
_pcie_serdes_pram_read(int unit, args_t *args)
{
    char *c;
    uint32_t address;
    uint32_t size;
    srds_access_t sa;

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    size = sal_ctoi(c, 0);

    sa.unit = unit;

    (void)pcie_phy_diag_pram_read(&sa, address, size);

    return CMD_OK;
}

STATIC cmd_result_t
_pcie_get_reg(int unit, args_t *args)
{
    char *c;
    uint16_t address;
    uint32_t val;

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    val= 0;
    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, address);
    sal_udelay(1000);
    READ_PAXB_0_CONFIG_IND_DATAr(unit, &val);
    cli_out("\nPCIe getepreg: address: 0x%x,  data = 0x%x\n", address, val);

    return CMD_OK;
}

STATIC cmd_result_t
_pcie_set_reg(int unit, args_t *args)
{
    char *c;
    uint16_t address;
    uint32_t val;
    uint32_t data;

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    val= 0;

    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, address);
    sal_udelay(1000);
    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    data = sal_ctoi(c, 0);
    WRITE_PAXB_0_CONFIG_IND_DATAr(unit, data);
    sal_udelay(1000);

    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, address);
    sal_udelay(1000);
    READ_PAXB_0_CONFIG_IND_DATAr(unit, &val);
    cli_out("\nPCIe setepreg: address: 0x%x,  data = 0x%x\n", address, val);

    return CMD_OK;
}

/* Arguments: address, field data, field mask, field bit shift(left) */
STATIC cmd_result_t
_pcie_rmw_reg(int unit, args_t *args)
{
    char *c;
    uint16_t address, fdata, fmask, fshift;
    uint32_t iaddr, data;

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    fdata = sal_ctoi(c, 0);

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    fmask = sal_ctoi(c, 0);

    if ((c= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    fshift = sal_ctoi(c, 0);

    iaddr = address;
    /* Read register */
    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, iaddr);
    sal_udelay(1000);
    READ_PAXB_0_CONFIG_IND_DATAr(unit, &data);

    /* modify data */
    data &= fmask << fshift;
    data |= (fdata & fmask) << fshift;

    /* Write data */
    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, iaddr);
    sal_udelay(1000);
    WRITE_PAXB_0_CONFIG_IND_DATAr(unit, data);

    /* Read back data */
    WRITE_PAXB_0_CONFIG_IND_ADDRr(unit, iaddr);
    sal_udelay(1000);
    READ_PAXB_0_CONFIG_IND_DATAr(unit, &data);

    cli_out("\nPCIe modepreg: address: 0x%x,  data = 0x%x\n", address, data);

    return CMD_OK;
}

/*
 * Diagnostic utilities for serdes and PHY devices.
 *
 * Command format used in BCM diag shell:
 * phy diag <pbm> <sub_cmd> [sub cmd parameters]
 * The list of sub commands:
 *   dsc - display tx/rx equalization information.
 *   dsc - display Diag information
 *            BCM.0> pciephy diag <lane bit map> dsc
 *            BCM.0> pciephy diag <lane bit map> state
 *            BCM.0> pciephy diag <lane bit map> eyescan
*/
STATIC cmd_result_t
_pcie_phy_diag(int unit, args_t *args)
{
    int rv = CMD_OK;
    char *cmd_str, *lane_str;
    soc_pbmp_t pbmp;
    soc_port_t port;
    srds_access_t *sa, *sa_l = NULL;
    int core = 0, prev_core = -1;
    int lane = 0, count = 0;

    /* Parse lane bitmap */
    if ((lane_str= ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    (void)parse_pbmp(unit, lane_str, &pbmp);

    /* Parse command */
    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    sa = sal_alloc(sizeof(srds_access_t) * PCIE_MAX_CORES, "access array");
    if (NULL == sa) {
        cli_out("_pcie_phy_diag: memory allocation failed/n");
        return CMD_FAIL;
    }

    SOC_PBMP_ITER(pbmp, port) {
        core = port / 4;
        lane = port % 4;
        if (core != prev_core) {
            prev_core = core;
            if(sa_l == NULL) {
                sa_l = sa;
            }
            else {
                sa_l++;
            }
            sa_l->unit = unit;
            sa_l->core = core;
            count++;
        }
        if(sa_l != NULL) {
            sa_l->lane_mask = 0x1 << lane;
        }
    }

    if (sal_strcasecmp(cmd_str, "dsc") == 0) {
        rv = pcie_phy_diag_dsc(sa, count);
        if(rv != SOC_E_NONE) {
            cli_out("pcie_phy_diag_dsc() failed, Error: 0x%x\n", rv);
        }
    } else if (sal_strcasecmp(cmd_str, "eyescan") == 0) {
        rv = pcie_phy_diag_eyescan(sa, count);
        if(rv != SOC_E_NONE) {
            cli_out("pcie_phy_diag_eyescan() failed, Error: 0x%x\n", rv);
        }
    } else if (sal_strcasecmp(cmd_str, "state") == 0) {
        rv = pcie_phy_diag_state(sa, count);
        if(rv != SOC_E_NONE) {
            cli_out("pcie_phy_diag_state() failed, Error: 0x%x\n", rv);
        }
    }
    else {
        rv = CMD_FAIL;
    }
    sal_free(sa);
    return rv;
}



char pciephy_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "pciephy <sub_cmd> [sub cmd parameters]\n\t"
    "Default: None\n"
    "Subcommands: diag, fw\n";
#else
    "Parameters: <sub_cmd> [sub cmd parameters]\n\t"
    "The list of sub commands:\n\t"
    "   getreg - displays value of given phy register\n\t"
    "          Syntax: pciephy getreg <PHY register address>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy getreg 0xd230\n\t"
    "   setreg - writes given value into given phy register\n\t"
    "          Syntax: pciephy setreg <PHY register address> <value>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy setreg 0xd230 0xa555\n\t"
    "   getepreg - displays value of given pcie core(endpoint) register\n\t"
    "          Syntax: pciephy getepreg <pcie core register address>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy getepreg 0xbc\n\t"
    "   setepreg - writes given value into given pcie core(endpoint) register\n\t"
    "          Syntax: pciephy setepreg <pcie core register address> <value>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy setepreg 0xbc 0x12345678\n\t"
    "   pramread - Reads and displays pcie phy pram content at given address\n\t"
    "          Syntax: pciephy pramread <pram address offset> <number of 16-bit words to read>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy pramread 0x0 0x20\n\t"
    "   diag - display SerDes lane information.\n\t"
    "          Syntax: pciephy diag <lane bit map> <diag sub-command>\n\t"
    "          Example:\n\t"
    "            BCM.0> pciephy diag 0x1 dsc\n\t"
    "            BCM.0> pciephy diag 0x1 state\n\t"
    "            BCM.0> pciephy diag 0x1 eyescan\n\t"
    "          diag - sub commands:\n\t"
    "            dsc - Display core state and lane state\n\t"
    "            state - Discplay core state, lane state for all lanes and event log\n\t"
    "            eyescan - Display eyescan for selected lane\n\t"
    "   fw - Load/dump SerDes(PCIe Gen3) firmware, display firmware version\n\t"
    "        Syntax: pciephy fw <fw sub-command> [fw sub cmd parameters]\n\t"
    "        Example:\n\t"
    "          BCM.0> pciephy fw load <firmware file>\n\t"
    "        fw - sub commands:\n\t"
    "          load - programs contents of given firmware file to QSPI flash\n\t"
    "          dump - Displays contents of PCIe Gen3 firmware from QSPI flash\n\t"
    "          version - displays PCIe Gen3 fw loader and SerDes fw versions\n"
    ;
#endif

/*
 * Function:    cmd_pciephy
 * Purpose:     Show/configure PCIe phy registers.
 * Parameters:  u - SOC unit #
 *              a - pointer to args
 * Returns:     CMD_OK/CMD_FAIL/
 */
cmd_result_t cmd_pciephy(int u, args_t *a)
{
    char *c;
    uint16_t address, data;
    srds_access_t sa;
    int rv = 0;

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    c = ARG_GET(a);

    if (c != NULL && sal_strcasecmp(c, "fw") == 0) {
        return _pcie_phy_fw(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "diag") == 0) {
        return _pcie_phy_diag(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "getreg") == 0) {
        if ((c= ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        address = sal_ctoi(c, 0);
        sa.unit = u;
        rv = pcie_phy_diag_reg_read(&sa, address, &data);
        if(rv != 0) {
            cli_out("pcie_phy_diag_reg_read failed: %d\n", rv);
        }
        else {
            cli_out("\naddess: 0x%x,  data = 0x%x\n", address, data);
        }
        return CMD_OK;
    }
    if (c != NULL && sal_strcasecmp(c, "setreg") == 0) {
        sa.unit = u;
        if ((c= ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        address = sal_ctoi(c, 0);
        if ((c= ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
        data = sal_ctoi(c, 0);
        rv = pcie_phy_diag_reg_write(&sa, address, data);
        if(rv != 0) {
            cli_out("ERROR: pcie_diag_reg_write failed: %d\n", rv);
        }
        else {
            cli_out("\naddess: 0x%x,  data = 0x%x\n", address, data);
        }
        return CMD_OK;
    }
    if (c != NULL && sal_strcasecmp(c, "getepreg") == 0) {
        return _pcie_get_reg(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "setepreg") == 0) {
        return _pcie_set_reg(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "modepreg") == 0) {
        return _pcie_rmw_reg(u, a);
    }
    if (c != NULL && sal_strcasecmp(c, "pramread") == 0) {
        return _pcie_serdes_pram_read(u, a);
    }
    if (c == NULL) {
        return (CMD_USAGE);
    }
    else {
        cli_out("Command NOT supported\n");
    }
    return CMD_OK;
}

#else
/* To avoid empty file warning in not supported architectures */
int appl_diag_diag_anchor;
#endif /* #if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT) || defined(BCM_ESW_SUPPORT) */

