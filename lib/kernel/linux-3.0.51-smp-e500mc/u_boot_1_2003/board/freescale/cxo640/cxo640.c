/*
 * Copyright 2011,2012 Freescale Semiconductor, Inc.
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

#include <common.h>
#include <command.h>
#include <netdev.h>
#include <linux/compiler.h>
#include <asm/mmu.h>
#include <asm/processor.h>
#include <asm/cache.h>
#include <asm/immap_85xx.h>
#include <asm/fsl_law.h>
#include <asm/fsl_serdes.h>
#include <asm/fsl_portals.h>
#include <asm/fsl_liodn.h>
#include <fm_eth.h>


extern void pci_of_setup(void *blob, bd_t *bd);


DECLARE_GLOBAL_DATA_PTR;

int checkboard(void)
{
	struct cpu_type *cpu = gd->cpu;
	ccsr_gur_t *gur = (void *)CONFIG_SYS_MPC85xx_GUTS_ADDR;
	unsigned int i;

	printf("Board: %sRDB, ", cpu->name);
	/*printf("HUGO - Ja nao ha CPLD version\n\r");*/

	/*printf("HUGO - Banco da memorioa nor e sempre o 0\n\r");*/

	/*
	 * Display the RCW, so that no one gets confused as to what RCW
	 * we're actually using for this boot.
	 */
	puts("Reset Configuration Word (RCW):");
	for (i = 0; i < ARRAY_SIZE(gur->rcwsr); i++) {
		u32 rcw = in_be32(&gur->rcwsr[i]);
		if ((i % 4) == 0)
			printf("\n       %08x:", i * 4);
		printf(" %08x", rcw);
	}
	puts("\n");

	/*
	 * Display the actual SERDES reference clocks as configured by the
	 * dip switches on the board.  Note that the SWx registers could
	 * technically be set to force the reference clocks to match the
	 * values that the SERDES expects (or vice versa).  For now, however,
	 * we just display both values and hope the user notices when they
	 * don't match.
	 */
	
    printf("******************************************************************** \n\r"); /* ja nao vai ler a cpld...valores colados*/
    printf("***                    CXO640 V2: RCW CONFIG.                   **** \n\r"); 
    printf("***        00000000: 50140000 00000000 54540000 00000000        **** \n\r"); 
    printf("***        00000010: 4c8090c3 c3c02000 de800000 40000000        **** \n\r"); 
    printf("***        00000020: 00000000 00000000 00000000 d0030700        **** \n\r"); 
    printf("***        00000030: 00000000 00000000 00000000 00000000        **** \n\r"); 
    printf("******************************************************************** \n\r");
    printf("***                      SERDES PROTOCOL 0x13                   **** \n\r"); 
	printf("***    SERDES BANK1 CLK= 100Mhz      SERDES BANK2 CLK= 100Mhz   **** \n\r"); 
    printf("***                 LANE C-SGMII1  LANE D-SGMII2                **** \n\r"); 
    printf("******************************************************************** \n\r"); 

	return 0;
}

int board_early_init_f(void)
{
	ccsr_gur_t *gur = (void *)(CONFIG_SYS_MPC85xx_GUTS_ADDR);

	/* board only uses the DDR_MCK0/1, so disable the DDR_MCK2/3 */
	setbits_be32(&gur->ddrclkdr, 0x000f000f);

	return 0;
}

#define CPLD_LANE_A_SEL	0x1
#define CPLD_LANE_G_SEL	0x2
#define CPLD_LANE_C_SEL	0x4
#define CPLD_LANE_D_SEL	0x8

void board_config_lanes_mux(void)
{
	ccsr_gur_t *gur = (void *)CONFIG_SYS_MPC85xx_GUTS_ADDR;
	int srds_prtcl = (in_be32(&gur->rcwsr[4]) &
				FSL_CORENET_RCWSR4_SRDS_PRTCL) >> 26;

	u8 mux = 0;
	switch (srds_prtcl) {
	case 0x2:
	case 0x5:
	case 0x9:
	case 0xa:
	case 0xf:
		break;
	case 0x8:
		mux |= CPLD_LANE_C_SEL | CPLD_LANE_D_SEL;
		break;
    case 0x13: /*HUGO configuraçao da CXO640 */
		break;
	case 0x14:
		mux |= CPLD_LANE_A_SEL;
		break;
	case 0x17:
		mux |= CPLD_LANE_G_SEL;
		break;
	case 0x16:
	case 0x19:
	case 0x1a:
		mux |= CPLD_LANE_G_SEL | CPLD_LANE_C_SEL | CPLD_LANE_D_SEL;
		break;
	case 0x1c:
		mux |= CPLD_LANE_G_SEL | CPLD_LANE_A_SEL;
		break;
	default:
		printf("Fman:Unsupported SerDes Protocol 0x%02x\n", srds_prtcl);
		break;
	}
	
}

int board_early_init_r(void)
{
	const unsigned int flashbase = CONFIG_SYS_FLASH_BASE;
	const u8 flash_esel = find_tlb_idx((void *)flashbase, 1);

	/*
	 * Remap Boot flash + PROMJET region to caching-inhibited
	 * so that flash can be erased properly.
	 */

	/* Flush d-cache and invalidate i-cache of any FLASH data */
	flush_dcache();
	invalidate_icache();

	/* invalidate existing TLB entry for flash + promjet */
	disable_tlb(flash_esel);

	set_tlb(1, flashbase, CONFIG_SYS_FLASH_BASE_PHYS,
			MAS3_SX|MAS3_SW|MAS3_SR, MAS2_I|MAS2_G,
			0, flash_esel, BOOKE_PAGESZ_256M, 1);

	set_liodns();
	setup_portals();
	board_config_lanes_mux();

	return 0;
}

unsigned long get_board_sys_clk(unsigned long dummy)
{
    printf("SYSCLCK = 66Mhz\n\r"); /*...valores colados*/

		return 66666666;

}

static const char *serdes_clock_to_string(u32 clock)
{
	switch (clock) {
	case SRDS_PLLCR0_RFCK_SEL_100:
		return "100";
	case SRDS_PLLCR0_RFCK_SEL_125:
		return "125";
	case SRDS_PLLCR0_RFCK_SEL_156_25:
		return "156.25";
	default:
		return "150";
	}
}

#define NUM_SRDS_BANKS	2

int misc_init_r(void)
{
	serdes_corenet_t *regs = (void *)CONFIG_SYS_FSL_CORENET_SERDES_ADDR;
	u32 actual[NUM_SRDS_BANKS];
	unsigned int i;
       
			/*actual[0] = freq[0][1]=0; */ /* valores anteriormente lidos pela CPLD*/
            /*actual[1] = freq[1][1]=0; */
            actual[0]=0;
            actual[1]=0;

	for (i = 0; i < NUM_SRDS_BANKS; i++) {
		u32 expected = in_be32(&regs->bank[i].pllcr0);
		expected &= SRDS_PLLCR0_RFCK_SEL_MASK;
        printf("SERDES bank %u expects reference clock"
			       " %sMHz, and actual is %sMHz\n", i + 1,
			       serdes_clock_to_string(expected),
			       serdes_clock_to_string(actual[i]));
		if (expected != actual[i]) {
			printf("Warning: SERDES bank %u expects reference clock"
			       " %sMHz, but actual is %sMHz\n", i + 1,
			       serdes_clock_to_string(expected),
			       serdes_clock_to_string(actual[i]));
		}
	}

	return 0;
}

void ft_board_setup(void *blob, bd_t *bd)
{
	phys_addr_t base;
	phys_size_t size;

	ft_cpu_setup(blob, bd);

	base = getenv_bootm_low();
	size = getenv_bootm_size();

	fdt_fixup_memory(blob, (u64)base, (u64)size);

#if defined(CONFIG_HAS_FSL_DR_USB) || defined(CONFIG_HAS_FSL_MPH_USB)
	fdt_fixup_dr_usb(blob, bd);
#endif

#ifdef CONFIG_PCI
	pci_of_setup(blob, bd);
#endif

	fdt_fixup_liodn(blob);
#ifdef CONFIG_SYS_DPAA_FMAN
	fdt_fixup_fman_ethernet(blob);
#endif
}
