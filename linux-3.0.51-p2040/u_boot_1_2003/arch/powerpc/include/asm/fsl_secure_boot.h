/*
 * Copyright 2010-2012 Freescale Semiconductor, Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __FSL_SECURE_BOOT_H
#define __FSL_SECURE_BOOT_H

#if defined(CONFIG_FSL_CORENET)
#define CONFIG_SYS_PBI_FLASH_BASE		0xc0000000
#else
#define CONFIG_SYS_PBI_FLASH_BASE		0xce000000
#endif
#define CONFIG_SYS_PBI_FLASH_WINDOW		0xcff80000

/*
 * Define the key hash for boot script here if public/private key pair used to
 * sign bootscript are different from the SRK hash put in the fuse
 * Example of defining KEY_HASH is
 * #define CONFIG_BOOTSCRIPT_KEY_HASH \
 *	 "41066b564c6ffcef40ccbc1e0a5d0d519604000c785d97bbefd25e4d288d1c8b"
 */

#define CONFIG_CMD_ESBC_VALIDATE

#if defined(CONFIG_B4860QDS)
#define CONFIG_SECURE_HKAREA_CPC
#undef CONFIG_SYS_INIT_L3_ADDR
#define CONFIG_SYS_INIT_L3_ADDR		0xbff00000
#define CONFIG_BOOTSCRIPT_HDR_ADDR	0xecc00000
#elif defined(CONFIG_FSL_CORENET)
#define CONFIG_BOOTSCRIPT_HDR_ADDR	0xe8e00000
#else
#define CONFIG_BOOTSCRIPT_HDR_ADDR	0xee020000
#endif

/*
 * Control should not reach back to uboot after validation of images
 * for secure boot flow and therefore bootscript should have
 * the bootm command. If control reaches back to uboot anyhow
 * after validating images, core should just spin.
 */
#ifdef CONFIG_BOOTSCRIPT_KEY_HASH
#define CONFIG_SECBOOT \
	"setenv bs_hdraddr " __stringify(CONFIG_BOOTSCRIPT_HDR_ADDR)";"	   \
	"setenv bootargs \"root=/dev/ram rw console=ttyS0,115200 ramdisk_size=600000\";"	\
	"esbc_validate $bs_hdraddr "  __stringify(CONFIG_BOOTSCRIPT_KEY_HASH)";" \
	"source $img_addr;"					\
	"esbc_halt;"
#else
#define CONFIG_SECBOOT \
	"setenv bs_hdraddr " __stringify(CONFIG_BOOTSCRIPT_HDR_ADDR)";"	 \
	"setenv bootargs \"root=/dev/ram rw console=ttyS0,115200 ramdisk_size=600000\";"	\
	"esbc_validate $bs_hdraddr;"			\
	"source $img_addr;"				\
	"esbc_halt;"
#endif

/* For secure boot flow, default environment used will be used */
#if defined(CONFIG_SYS_RAMBOOT)
#if defined(CONFIG_RAMBOOT_SPIFLASH)
#undef CONFIG_ENV_IS_IN_SPI_FLASH
#elif defined(CONFIG_NAND)
#undef CONFIG_ENV_IS_IN_NAND
#endif
#else /*CONFIG_SYS_RAMBOOT*/
#undef CONFIG_ENV_IS_IN_FLASH
#endif

#define CONFIG_ENV_IS_NOWHERE

/*
 * We don't want boot delay for secure boot flow
 * before autoboot starts
 */
#undef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	0
#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND		CONFIG_SECBOOT

/*
 * CONFIG_ZERO_BOOTDELAY_CHECK should not be defined for
 * secure boot flow as defining this would enable a user to
 * reach uboot prompt by pressing some key before start of
 * autoboot
 */
#undef CONFIG_ZERO_BOOTDELAY_CHECK

#endif
