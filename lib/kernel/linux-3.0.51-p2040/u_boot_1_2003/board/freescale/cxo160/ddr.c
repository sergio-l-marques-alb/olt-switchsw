
/*
**     DDR controller(s)  initialization code
**   
**     Copyright : 1997 - 2013 Freescale Semiconductor, Inc. All Rights Reserved.
**    
**     http      : www.freescale.com
**     mail      : support@freescale.com
 */
#include <common.h>
#include <asm/fsl_ddr_sdram.h>
#include <common.h>
#include <i2c.h>
#include <hwconfig.h>
#include <asm/mmu.h>
#include <asm/fsl_ddr_dimm_params.h>
#include <asm/fsl_law.h>

#define DATARATE_300MHZ        300000000
#define DATARATE_400MHZ        400000000
#define DATARATE_500MHZ        500000000
#define DATARATE_600MHZ        666667000
#define DATARATE_700MHZ        700000000
#define DATARATE_800MHZ        800000000
#define DATARATE_900MHZ        900000000
#define DATARATE_1000MHZ      1000000000
#define DATARATE_1100MHZ      1100000000
#define DATARATE_1200MHZ      1200000000
#define DATARATE_1300MHZ      1300000000
#define DATARATE_1400MHZ      1400000000
#define DATARATE_1500MHZ      1500000000
#define DATARATE_1600MHZ      1600000000
#define DATARATE_1700MHZ      1700000000
#define DATARATE_1800MHZ      1800000000
#define DATARATE_1900MHZ      1900000000
#define DATARATE_2000MHZ      2000000000
#define DATARATE_2100MHZ      2100000000
#define DATARATE_2200MHZ      2200000000
#define DATARATE_2300MHZ      2300000000
#define DATARATE_2400MHZ      2400000000

/* DDR Controller 1 register values */

#define SDRAM_CFG_MEM_EN_MASK       0x80000000

/* DDR Controller 1 register values */
#define PEX_CONFIG_DDRmc1_CS0_BNDS         0x7F
#define PEX_CONFIG_DDRmc1_CS1_BNDS         0x00
#define PEX_CONFIG_DDRmc1_CS0_CONFIG       0x80014302
#define PEX_CONFIG_DDRmc1_CS1_CONFIG       0x00
#define PEX_CONFIG_DDRmc1_CS0_CONFIG_2     0x00
#define PEX_CONFIG_DDRmc1_CS1_CONFIG_2     0x00
#define PEX_CONFIG_DDRmc1_CS2_BNDS         0x00
#define PEX_CONFIG_DDRmc1_CS3_BNDS         0x00
#define PEX_CONFIG_DDRmc1_CS2_CONFIG       0x00
#define PEX_CONFIG_DDRmc1_CS3_CONFIG       0x00
#define PEX_CONFIG_DDRmc1_CS2_CONFIG_2     0x00
#define PEX_CONFIG_DDRmc1_CS3_CONFIG_2     0x00
#define PEX_CONFIG_DDRmc1_TIMING_3         0x00090000
#define PEX_CONFIG_DDRmc1_TIMING_0         0x40440104
#define PEX_CONFIG_DDRmc1_TIMING_1         0x838F8844
#define PEX_CONFIG_DDRmc1_TIMING_2         0x0FB09116
#define PEX_CONFIG_DDRmc1_SDRAM_CFG        (0xC7040008 & (~SDRAM_CFG_MEM_EN_MASK))
#define PEX_CONFIG_DDRmc1_SDRAM_CFG2       0x24401010
#define PEX_CONFIG_DDRmc1_MODE_1           0x00061840
#define PEX_CONFIG_DDRmc1_MODE_2           0x00080000
#define PEX_CONFIG_DDRmc1_MODE_CONTROL     0x00000000
#define PEX_CONFIG_DDRmc1_INTERVAL         0x1034040D
#define PEX_CONFIG_DDRmc1_MEM_INIT_VALUE   0xDEADBEEF
#define PEX_CONFIG_DDRmc1_CLK_CTRL         0x02800000
#define PEX_CONFIG_DDRmc1_INIT_ADDR        0x00000000
#define PEX_CONFIG_DDRmc1_INIT_EXT_ADDR    0x00000000
#define PEX_CONFIG_DDRmc1_TIMING_4         0x01
#define PEX_CONFIG_DDRmc1_TIMING_5         0x03401400
#define PEX_CONFIG_DDRmc1_ZQ_CNTL          0x89080600
#define PEX_CONFIG_DDRmc1_WRLVL_CNTL       0x8655F607
#define PEX_CONFIG_DDRmc1_WRLVL_CNTL_2     0x00
#define PEX_CONFIG_DDRmc1_WRLVL_CNTL_3     0x00
#define PEX_CONFIG_DDRmc1_RCW_1            0x00
#define PEX_CONFIG_DDRmc1_RCW_2            0x00


/* DDR Controller 1 configuration global structures */
fsl_ddr_cfg_regs_t ddr_cfg_regs_0 = {
  .cs[0].bnds = PEX_CONFIG_DDRmc1_CS0_BNDS,
  .cs[1].bnds = PEX_CONFIG_DDRmc1_CS1_BNDS,
  .cs[0].config = PEX_CONFIG_DDRmc1_CS0_CONFIG,
  .cs[1].config = PEX_CONFIG_DDRmc1_CS1_CONFIG,
  .cs[0].config_2 = PEX_CONFIG_DDRmc1_CS0_CONFIG_2,
  .cs[1].config_2 =  PEX_CONFIG_DDRmc1_CS1_CONFIG_2,
  .cs[2].bnds = PEX_CONFIG_DDRmc1_CS2_BNDS,
  .cs[3].bnds = PEX_CONFIG_DDRmc1_CS3_BNDS,
  .cs[2].config = PEX_CONFIG_DDRmc1_CS2_CONFIG,
  .cs[3].config = PEX_CONFIG_DDRmc1_CS3_CONFIG,
  .cs[2].config_2 =  PEX_CONFIG_DDRmc1_CS2_CONFIG_2,
  .cs[3].config_2 =  PEX_CONFIG_DDRmc1_CS3_CONFIG_2,
  .timing_cfg_3 = PEX_CONFIG_DDRmc1_TIMING_3,
  .timing_cfg_0 = PEX_CONFIG_DDRmc1_TIMING_0,
  .timing_cfg_1 = PEX_CONFIG_DDRmc1_TIMING_1,
  .timing_cfg_2 = PEX_CONFIG_DDRmc1_TIMING_2,
  .ddr_sdram_cfg = PEX_CONFIG_DDRmc1_SDRAM_CFG,
  .ddr_sdram_cfg_2 = PEX_CONFIG_DDRmc1_SDRAM_CFG2,
  .ddr_sdram_mode = PEX_CONFIG_DDRmc1_MODE_1,
  .ddr_sdram_mode_2 = PEX_CONFIG_DDRmc1_MODE_2,
  .ddr_sdram_md_cntl = PEX_CONFIG_DDRmc1_MODE_CONTROL,
  .ddr_sdram_interval = PEX_CONFIG_DDRmc1_INTERVAL,
  .ddr_data_init = PEX_CONFIG_DDRmc1_MEM_INIT_VALUE,
  .ddr_sdram_clk_cntl = PEX_CONFIG_DDRmc1_CLK_CTRL,
  .ddr_init_addr = PEX_CONFIG_DDRmc1_INIT_ADDR,
  .ddr_init_ext_addr = PEX_CONFIG_DDRmc1_INIT_EXT_ADDR,
  .timing_cfg_4 = PEX_CONFIG_DDRmc1_TIMING_4,
  .timing_cfg_5 = PEX_CONFIG_DDRmc1_TIMING_5,
  .ddr_zq_cntl = PEX_CONFIG_DDRmc1_ZQ_CNTL,
  .ddr_wrlvl_cntl = PEX_CONFIG_DDRmc1_WRLVL_CNTL,
  .ddr_wrlvl_cntl_2 = PEX_CONFIG_DDRmc1_WRLVL_CNTL_2,
  .ddr_wrlvl_cntl_3 = PEX_CONFIG_DDRmc1_WRLVL_CNTL_3,
  .ddr_sdram_rcw_1 = PEX_CONFIG_DDRmc1_RCW_1,
  .ddr_sdram_rcw_2 = PEX_CONFIG_DDRmc1_RCW_2
};

fixed_ddr_parm_t fixed_ddr_parm_0[] = {
  { DATARATE_1000MHZ, DATARATE_1100MHZ, &ddr_cfg_regs_0 },
  {0, 0, NULL}
};



phys_size_t initdram(int board_type)
{
	phys_size_t dram_size;

	puts("Initializing....");
 
    puts("using fixed parameters\n");
 	dram_size = fixed_sdram();
 
   dram_size = setup_ddr_tlbs(dram_size / 0x100000);
   dram_size *= 0x100000;
   debug("    DDR: ");
	return dram_size;
}

phys_size_t fixed_sdram(void)
{
	int i;
	char buf[32];
	fsl_ddr_cfg_regs_t ddr_cfg_regs;
	phys_size_t ddr_size;
	unsigned int lawbar1_target_id;
	ulong ddr_freq, ddr_freq_mhz;

	ddr_freq = get_ddr_freq(0);
	ddr_freq_mhz = ddr_freq / 1000000;

	printf("Configuring DDR for %s MT/s data rate\n",
				strmhz(buf, ddr_freq));

    for (i = 0; fixed_ddr_parm_0[i].max_freq > 0; i++) {
      // printf("fixed_ddr_parm_0[%d].min_freq=%d\n\r",i,(fixed_ddr_parm_0[i].min_freq)/1000000);
      // printf("fixed_ddr_parm_0[%d].max_freq=%d\n\r",i,(fixed_ddr_parm_0[i].max_freq)/1000000);
      // printf("ddr_freq_mhz=%d\n\r",ddr_freq_mhz);
    	if ((ddr_freq > fixed_ddr_parm_0[i].min_freq) && (ddr_freq <= fixed_ddr_parm_0[i].max_freq)) {
         //   printf("HUGO_fixed_sdram -entrou no if\n\r");
    		memcpy(&ddr_cfg_regs,fixed_ddr_parm_0[i].ddr_settings,sizeof(ddr_cfg_regs)); 
            break;   		
    	}
   }
    
	if (fixed_ddr_parm_0[i].max_freq == 0)
		panic("Unsupported DDR data rate %s MT/s data rate\n",strmhz(buf, ddr_freq)); /*panic*/
    
	ddr_size = (phys_size_t) CONFIG_SYS_SDRAM_SIZE * 1024 * 1024;
    
	ddr_cfg_regs.ddr_cdr1 = DDR_CDR1_DHC_EN;
    
	fsl_ddr_set_memctl_regs(&ddr_cfg_regs, 0);

	/*
	 * setup laws for DDR. If not interleaving, presuming half memory on
	 * DDR1 and the other half on DDR2
	 */
	if (fixed_ddr_parm_0[i].ddr_settings->cs[0].config & 0x20000000) {
		if (set_ddr_laws(CONFIG_SYS_DDR_SDRAM_BASE,
				 ddr_size,
				 LAW_TRGT_IF_DDR_INTRLV) < 0) {
			printf("ERROR setting Local Access Windows for DDR\n");
			return 0;
		}
	} else {

		lawbar1_target_id = LAW_TRGT_IF_DDR_1;
		if (set_ddr_laws(CONFIG_SYS_DDR_SDRAM_BASE,
				 ddr_size,
				 lawbar1_target_id) < 0) {
			printf("ERROR setting Local Access Windows for DDR\n");
			return 0;
		}
	}
	return ddr_size;
}

