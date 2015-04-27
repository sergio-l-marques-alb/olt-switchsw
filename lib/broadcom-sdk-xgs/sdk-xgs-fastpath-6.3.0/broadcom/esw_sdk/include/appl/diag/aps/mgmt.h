/*
 * $Id: mgmt.h 1.16 Broadcom SDK $
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
 * Arm Processor Subsystem remote management library
 */

/*
 * Warning:
 * This file is included by both uKernel and host code. Be very careful
 * about what goes into this file, esp. with regards to including other
 * files.
 */

#ifndef APPL_DIAG_APS_MGMT_H
#define APPL_DIAG_APS_MGMT_H

#define MGMT_COMMAND_MAGIC      0xa5a5a5a5
#define MGMT_REPLY_MAGIC        0x5a5a5a5a

#define MGMT_SESSION_KEY_LEN            (128/8)     /* AES 128 bits */
#define MGMT_SESSION_KEY_MSG_LEN        (2048/8)    /* pkcs encrypted session key */

/* SPI packet encapsulation */
#define MGMT_SPI_PREAMBLE               0xee
#define MGMT_SPI_PREAMBLE_LEN           16
#define MGMT_SPI_ESCAPE                 0xe0
#define MGMT_SPI_ESCAPE_ESCAPE          0xe1
#define MGMT_SPI_ESCAPE_PREAMBLE        0xe2
#define MGMT_SPI_ESCAPE_DUPLICATE       0xe3
#define MGMT_SPI_DUPLICATE_SEPARATOR    0xe4

/* SPI command codes */
#define MGMT_SPI_CMD_READY              0xa1
#define MGMT_SPI_CMD                    0xa2
#define MGMT_SPI_REPLY                  0xa3

typedef enum {
    MGMT_VERSION_INFO,                  /* Retrieve version info */
    MGMT_REBOOT,                        /* Reboot request */
    MGMT_SESSION_KEY,                   /* Request new session key */
    MGMT_SET_MACADDR,
    MGMT_SET_MACSEC_KEY,
    MGMT_SWITCH_READ_REG,
    MGMT_SWITCH_WRITE_REG,
    MGMT_SWITCH_SPI_CONTROL,            /* Change SPI-MUX to point at the switch */
    MGMT_FLASH_INFO,                    /* Retrieve flash info */
    MGMT_FLASH_ERASE_SECTOR,            /* Erase the specified sector */
    MGMT_FLASH_ERASE_PAGE,              /* Erase the specified page */
    MGMT_FLASH_PAGE_READ,               /* Read a page (256 bytes) */
    MGMT_FLASH_PAGE_WRITE,              /* Write a page (256 bytes) */
    MGMT_FLASH_DUMP_CORE,               /* Write a core file to flash */
    MGMT_READ_MEMORY,                   /* Read target memory */
    MGMT_WRITE_MEMORY,                  /* Write target memory */
    MGMT_EXECUTE,                       /* Execute downloaded image */
    MGMT_RNG_DATA,                      /* Fetch RNG data */
    MGMT_CABLE_DIAG,                    /* Cable diagnostics (Polar) */
    MGMT_PERF_COUNTER,                  /* Read performance counters */
    MGMT_PING,                          /* ping the device */
    MGMT_GET_POWER_STATE,               /* set the power state */
    MGMT_SET_POWER_STATE,               /* get the power state */
    MGMT_PLL_SPREAD,                    /* set PLL spread spectrum mode */
    MGMT_SHELL_CMD,                     /* execute shell command */
    MGMT_BUFFER_INFO,                   /* request buffer info */
    MGMT_READ_BUFFER,                   /* buffer read */
    MGMT_READ_BUFFER_PROT,              /* buffer read (protected) */
    MGMT_WRITE_BUFFER,                  /* buffer write */
    MGMT_WRITE_BUFFER_PROT,             /* buffer write (protected) */
    MGMT_CERT_SESSION_KEY,              /* certificate based management key request */
    MGMT_TRANSMIT_BUFFER,               /* buffer transmit */
    MGMT_ACD,                           /* Automotive cable diagnostics - Polar only */
    MGMT_DMU                            /* Change the DMU settings - Polar B0 only */
    /* No changes above this line - ROM code depends on these values */
} mgmt_cmd_t;

typedef enum {
    MGMT_SUCCESS,                       /* successful operation */
    MGMT_ERROR                          /* generic error */
} mgmt_status_t;

typedef enum {
    MGMT_FLASH_INIT_MODE,               /* ROM flash-init mode, commands/replys not encrypted */
    MGMT_ENGINEERING_MODE,              /* Engineering/test mode */
    MGMT_PRODUCTION_MODE                /* Production mode */
} mgmt_mode_t;


/*
 * Power states
 */

typedef enum {
    POWER_INIT =        0,      /* Default value out of reset */
    POWER_ULTRA_LOW =   1,      /* About to power down */
    POWER_DEEP_SLEEP =  2,      /* ARM at REFCLK, interfaces off */
    POWER_SLEEP =       3,      /* REFCLK no SERDES */
    POWER_REF =         4,      /* ARM at REFCLK */
    POWER_HALF_SPEED =  5,      /* ARM at half full speed */
    POWER_FULL_SPEED =  6       /* Full speed ARM, everything on */
} power_state_t;

#define _POWER_STATE_LIMIT 7


#define _POWER_STATE_STR_INIT        { \
        "default",                     \
        "ultra",                       \
        "deep",                        \
        "sleep",                       \
        "ref",                         \
        "half",                        \
        "full",                        \
        "unknown"                      \
        }

extern char *_power_state_str[];

#define _POWER_STATE_STR(s)          \
        _power_state_str[(((int)s) <= 0 && ((int)s) >= _POWER_STATE_LIMIT) ? _POWER_STATE_LIMIT : s]

#define MGMT_REBOOT_SLOT        0x4a696d42  /* boot specific slot image */
#define MGMT_REBOOT_FLASH_INIT  0x4d61726b  /* reenter flash init mode */

#define MGMT_COMMAND_LEN        12      /* mgmt command w/ no data */
#define MGMT_STATUS_LEN         12      /* mgmt reply w/ no data */

#define MGMT_NUM_ARGS           4
#define MGMT_RNG_DATA_SIZE      32      /* bytes of RNG data to return */

#define MGMT_FLASH_PAGE_SIZE    256     /* Flash page size */
#define MGMT_MEMORY_PAGE_SIZE   256     /* Memory page size */

#define MGMT_NUM_BUFFERS        4       /* Max number of buffers */
#define MGMT_BUFFER_PAGE_SIZE   256     /* Buffer page size */
#define MGMT_BUFFER_FLAG_PROT   1       /* Buffer is protected */

#define MGMT_SHELL_CMD_LEN      256     /* Max length of shell command */

typedef struct mgmt_command_s {
    uint32            cmd;                    /* command */
    uint32            len;                    /* Length of entire packet */
    struct {
        uint32    magic;                      /* MGMT_COMMAND_MAGIC */
        union {
            struct {
                uint32    number;             /* ping number */
            } ping;
            struct {
                uint32    state;              /* power state */
            } power;
            struct {
                uint32    spread;             /* PLL spread-spectrum param */
            } spread;
            struct {
                uint32    args[MGMT_NUM_ARGS];    /* args to reboot with */
            } reboot;
            struct {
                char        cmd[MGMT_SHELL_CMD_LEN];   /* shell command to execute */
            } shell_cmd;
            struct {
                uint32    size;               /* 1, 2, 4 or 8 bytes */
                uint32    address;            /* switch reg addr */
            } switch_read;
            struct {
                uint32    size;               /* 1, 2, 4 or 8 bytes */
                uint32    address;            /* switch reg addr */
                uint32    upper_regval;       /* switch reg value, upper bits */
                uint32    lower_regval;       /* switch reg value, lower bits */
            } switch_write;
            struct {
                uint8     addr[6];            /* MAC address */
            } macaddr;
            struct {
                uint8     data[256];          /* MACSEC key */
            } macsec_key;
            struct {
                uint32    spi;                /* SPI flash # */
            } flash_info;
            struct {
                uint32    spi;                /* SPI flash # */
                uint32    address;            /* address */
            } flash_erase;
            struct {
                uint32    spi;                /* SPI flash # */
                uint32    address;            /* address */
            } flash_read;
            struct {
                uint32    spi;                        /* SPI flash # */
                uint32    address;                    /* address */
                uint8     data[MGMT_FLASH_PAGE_SIZE]; /* Flash read/write */
            } flash_write;
            struct {
                uint32    spi;                        /* SPI flash # */
            } flash_coredump;
            struct {
                uint32    address;            /* address */
            } read_memory;
            struct {
                uint32    address;            /* address */
                uint8     data[MGMT_MEMORY_PAGE_SIZE];
            } write_memory;
            struct {
                uint32    length;
                uint32    cksum;
                uint32    args[MGMT_NUM_ARGS];
                uint8     signature[MGMT_MEMORY_PAGE_SIZE];
            } execute;
            struct {
                uint32    counter;
            } perf_counter;
            struct {
                uint32      buffer;
                uint32      offset;
                uint32      length;
            } read_buffer;
            struct {
                uint32      buffer;
                uint32      offset;
                uint32      length;
                uint8       data[MGMT_BUFFER_PAGE_SIZE];
            } write_buffer;
            struct {
                uint32      buffer;
                uint32      length;
            } transmit_buffer;
            struct {
                uint32      buffer;
                uint32      length;
            } cert_session_key;
            struct {
                uint32      port;
                uint32      command;
            } acd;
            struct {
                uint32      hz;
                uint32      hclk_freq;
                uint32      hclk_sel;
                uint32      pclk_freq;
                uint32      pclk_sel;
                uint32      p1div;
                uint32      p2div;
                uint32      ndiv;
                uint32      m1div;
                uint32      m2div;
                uint32      m3div;
                uint32      m4div;
                uint32      pll_num;
                uint32      frac;
                uint32      bclk_sel;
            } dmu;
        };
    } u;
    char        pad[16];                                /* encryption padding */
} mgmt_command_t;

typedef struct mgmt_reply_version_s {
    uint32    mode;               /* Navigator mode */
    uint32    mos_version;        /* Software version */
    uint32    args[4];            /* args to booted with */
    uint32    model_id;           /* model id */
    uint32    chip_id;            /* chip id */
    uint32    otp_bits;           /* OTP bit values */
} mgmt_reply_version_t;

typedef struct mgmt_reply_buffer_info_s {
    uint32      count;
    struct {
        uint32  flags;
        uint32  length;
    } info[MGMT_NUM_BUFFERS];
} mgmt_reply_buffer_info_t;

#define MGMT_ACD_PAIRS          4
typedef struct mgmt_reply_acd_results_s {
    uint32      fault;                  /* ECD cable fault register */
    uint32      length[MGMT_ACD_PAIRS]; /* pair A, B, C, D lengths */
} mgmt_reply_acd_results_t;


typedef struct mgmt_reply_s {
    uint32            status;                 /* Used in replies */
    uint32            len;                    /* Length of entire packet */
    struct {
        uint32    magic;                      /* MGMT_REPLY_MAGIC */
        union {
            mgmt_reply_version_t    version;    /* version/status information */
            struct {
                uint32      number;             /* ping number */
            } ping;
            struct {
                uint32      state;              /* power state */
            } power;
            struct {
                /* nonce to use as the IV with the encrypted session key */
                uint8     nonce[MGMT_SESSION_KEY_LEN];
                /*
                 * The new AES session key, which is itself encrypted
                 * with the RSA public key embedded in the Navigator device.
                 * The host code will have to have a copy of the corresponding
                 * private key to use this.
                 */
                uint8     message[MGMT_SESSION_KEY_MSG_LEN];
            } session;
            struct {
                uint32    upper_regval;               /* switch reg value, upper bits */
                uint32    lower_regval;               /* switch reg value, lower bits */
            } switch_read;
            struct {
                uint32    rdid;                       /* response to RDID command */
            } flash_info;
            struct {
                /* Flash read data */
                uint8     data[MGMT_FLASH_PAGE_SIZE];
            } flash_read;
            struct {
                /* memory read data */
                uint8     data[MGMT_MEMORY_PAGE_SIZE];
            } read_memory;
            struct {
                uint8     data[MGMT_RNG_DATA_SIZE];
            } rng;
            struct {
                uint32    value;
            } perf_counter;
            mgmt_reply_buffer_info_t buffer_info;
            struct {
                uint8       data[MGMT_BUFFER_PAGE_SIZE];
            } read_buffer;
            mgmt_reply_acd_results_t acd;           /* ACD results */
        };
    } u;
    char        pad[16];                        /* AES encryption padding */
} mgmt_reply_t;

#endif
