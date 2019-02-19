/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/cpu.h>
#include <asm/io.h>
#include <linux/kobject.h>
#include <asm/processor.h>
#include <asm/cpu.h>

#include <mach/iproc_regs.h>
#include "pae_regs.h"
#include "pae_shared.h"
#include "pae_fw.h"
#include "paemsg_int.h"

#define SREC_INIT (1)
#define SREC_DONE (2)

int pae_hex_to_int(char c)
{
    if (c >= '0' && c <= '9') {
        return (c - '0');
    }
    if (c >= 'a' && c <= 'f') {
        return (c - 'a' + 10);
    }
    if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    }
    return 0;
}



static int pae_r5_go(void)
{
    u32 regval;

    RETURN_ON_FAIL(pae_register_get(R5_CONFIG0, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, SYS_PORESET,             0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, RESET_N,                 0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_RESET_N,           0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, CPU_HALT,                0);
    RETURN_ON_FAIL(pae_register_set(R5_CONFIG0, regval), NO_MSG);

    RETURN_ON_FAIL(pae_register_get(R5_CONFIG0, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, SYS_PORESET,             1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, RESET_N,                 1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_RESET_N,           1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, CPU_HALT,                1);
    RETURN_ON_FAIL(pae_register_set(R5_CONFIG0, regval), NO_MSG);

    printk(KERN_INFO "PAE firmware started\n");
    return 0;
}


unsigned 
int pae_parse_srec_line (char *line, int line_len)
{
    int count;
    u32 address;
    int datapos;
    int i;

    if (line_len < 8) { /* minimal length */
        printk(KERN_ERR "Bad SREC length in PAE FW: %d\n", line_len);
        return -EINVAL;
    }

    switch (line[1]) {      /* Record Type */
    case '0':           /* Header record.  Tell caller to initialize things */
        return SREC_INIT;
            
    case '1' :          /* Data Record with 2 byte address */
        count = (pae_hex_to_int(line[2]) << 4) |
            (pae_hex_to_int(line[3]));
        count -= 3; /* 2 address + 1 checksum */
        datapos = 8;

        address = (pae_hex_to_int(line[4]) << 12) |
            (pae_hex_to_int(line[5]) << 8) |
            (pae_hex_to_int(line[6]) << 4) |
            (pae_hex_to_int(line[7]));
        break;

    case '2' :          /* Data Record with 3 byte address */
        count = (pae_hex_to_int(line[2]) << 4) |
            (pae_hex_to_int(line[3]));
        count -= 4; /* 3 address + 1 checksum */
        datapos = 10;

        address = (pae_hex_to_int(line[4]) << 20) |
            (pae_hex_to_int(line[5]) << 16) |
            (pae_hex_to_int(line[6]) << 12) |
            (pae_hex_to_int(line[7]) << 8) |
            (pae_hex_to_int(line[8]) << 4) |
            (pae_hex_to_int(line[9]));
        break;

    case '3' :          /* Data Record with 4 byte address */
        count = (pae_hex_to_int(line[2]) << 4) |
            (pae_hex_to_int(line[3]));
        count -= 5; /* 4 address + 1 checksum */
        datapos = 12;

        address = (pae_hex_to_int(line[4]) << 28) |
            (pae_hex_to_int(line[5]) << 24) |
            (pae_hex_to_int(line[6]) << 20) |
            (pae_hex_to_int(line[7]) << 16) |
            (pae_hex_to_int(line[8]) << 12) |
            (pae_hex_to_int(line[9]) << 8) |
            (pae_hex_to_int(line[10]) << 4) |
            (pae_hex_to_int(line[11]));
        break;
            
    case '5':         /* Record count - ignore */
        return 0;

    case '6':         /* End of block*/
    case '7':         /* End of block */
    case '8':         /* End of block */
    case '9':         /* End of block */
        return SREC_DONE;
            
    default :       /* We don't parse all other records */
        printk(KERN_ERR "Unsupported Record S%c\n", line[1]);
        return 0;
    }
    if (line_len < datapos + count * 2 + 2) {
        printk(KERN_ERR "Bad SREC length in PAE FW: %d\n", line_len);
        return -EINVAL;
    }
    
    while (count > 0) {
        u32 word = 0;
        for (i = 0; i < 4; ++i) {
            if (count > 0) {
                u8 byte = (pae_hex_to_int(line[datapos]) << 4) | pae_hex_to_int(line[datapos + 1]);
                datapos += 2;
                word |= ((u32)byte << (8 * i));
                count--;
            }
        }

        pae_set_tcm_u32(address, word);

        address += 4;
    }

    return 0;
}


int pae_add_srec_character(char c)
{
    static char buffer[128];
    static int buf_pos = 0;
    
    if (c == '\n') {
        if (buf_pos == sizeof(buffer)) {
            /* We are full, and have a newline.  Reset and return the error */
            buf_pos = 0;
            return -ENOMEM;
        } else {
            /* Not full, and have a newline.  Parse the line and reset */
            int rv = pae_parse_srec_line(buffer, buf_pos);
            buf_pos = 0;
            return rv;
        }
    } else {
        if (buf_pos == sizeof(buffer)) {
            /* We are full.  Just eat the character and wait for newline */
            return 0;
        } else {
            /* add the character, wait for newline */
            buffer[buf_pos++] = c;
            return 0;
        }
    }
}


ssize_t fw_write(struct file *f, struct kobject *kobj, struct bin_attribute *bin_attr,
                 char *buf, loff_t offset, size_t count)
{
    int rv;
    int num_left = count;

    if (pae_status == PAE_STOPPED) {
        pae_init();
    }

    while (num_left--) {
        rv = pae_add_srec_character(*buf++);

        switch (rv) {
        case 0:
            break;
        case SREC_INIT:
            // should stop R5, then
            pae_clear_tcms();
            pae_status = PAE_NO_GO;
            break;
        case SREC_DONE:
            {
                u32 regval;
                u32 version_word;
                u8 compat_vers;

                RETURN_ON_FAIL(pae_register_get(R5_CONFIG0, &regval), NO_MSG);
                /* printk("\nOn Go: R5_CONFIG0: %08x (%d, %d)\n", regval, REG_FIELD_GET_BIT(regval, R5_CONFIG0, INITRAMA), REG_FIELD_GET_BIT(regval, R5_CONFIG0, INITRAMB)); */
                /* pae_register_get(0x2a000000, &regval); */
                /* printk("%08x: %08x (%08x)\n", 0, pae_get_tcm_u32(0), regval); */

                version_word = pae_get_tcm_u32(R5_VERSION);
                compat_vers = ((version_word>>24) & 0xff);
                printk(KERN_INFO "PAE FW %u.%u.%u; compat vers: %u\n",
                       ((version_word>>16) & 0xff), ((version_word>>8) & 0xff), ((version_word>>0) & 0xff),
                       compat_vers);

                if (compat_vers != PAE_COMPAT_VERSION) {
                    printk(KERN_ERR "Incompatible PAE FW level (%d, expected %d)\n",
                           compat_vers, PAE_COMPAT_VERSION);
                    break;
                }
				// Write the current IPsec version and it compatibility version
				version_word = PAE_IPSEC_VERSION | (PAE_IPSEC_COMPAT_VERSION << 24); 
				pae_set_tcm_u32(R5_IPSEC_VERSION, version_word);
            }
            rv = pae_r5_go();
            pae_status = PAE_RUNNING;
            if (rv) {
                return rv;
            }
            break;
        default:
            return rv;
        }
    }

    return count;
}
