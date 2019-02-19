
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

// #define DDR_1_CS0_BNDS_VAL         0xFF         
// #define DDR_1_CS1_BNDS_VAL         0xFF         
// #define DDR_1_CS2_BNDS_VAL         0x00         
// #define DDR_1_CS3_BNDS_VAL         0x00         
// #define DDR_1_CS0_CONFIG_VAL       0x80044302   
// #define DDR_1_CS1_CONFIG_VAL       0x80004302   
// #define DDR_1_CS2_CONFIG_VAL       0x00         
// #define DDR_1_CS3_CONFIG_VAL       0x00         
// #define DDR_1_CS0_CONFIG_2_VAL     0x00         
// #define DDR_1_CS1_CONFIG_2_VAL     0x00         
// #define DDR_1_CS2_CONFIG_2_VAL     0x00         
// #define DDR_1_CS3_CONFIG_2_VAL     0x00   
// #define DDR_1_TIMING_CFG_0_VAL     0x40110104   
// #define DDR_1_TIMING_CFG_1_VAL     0x5C5BE544   
// #define DDR_1_TIMING_CFG_2_VAL     0x0FA888CA   
// #define DDR_1_TIMING_CFG_3_VAL     0x00020000
// #define DDR_1_TIMING_CFG_4_VAL     0x00000001   
// #define DDR_1_TIMING_CFG_5_VAL     0x02401400   
// #define DDR_1_SDRAM_CFG_VAL        0xC7044000   
// #define DDR_1_SDRAM_CFG_2_VAL      0x24401100   
// #define DDR_1_SDRAM_INTERVAL_VAL   0x0A280100   
// #define DDR_1_SDRAM_CLK_CNTL_VAL   0x01800000   
// #define DDR_1_ZQ_CNTL_VAL          0x89080600   
// #define DDR_1_WRLVL_CNTL_VAL       0x8675F605   
// #define DDR_1_WRLVL_CNTL_2_VAL     0x00         
// #define DDR_1_WRLVL_CNTL_3_VAL     0x00         
// #define DDR_1_SDRAM_MODE_VAL       0x00441220   
// #define DDR_1_SDRAM_MODE_2_VAL     0x00         
// #define DDR_1_SDRAM_MODE_3_VAL     0x00001220   
// #define DDR_1_SDRAM_MODE_4_VAL     0x00         
// #define DDR_1_SDRAM_MODE_5_VAL     0x00001220   
// #define DDR_1_SDRAM_MODE_6_VAL     0x00         
// #define DDR_1_SDRAM_MODE_7_VAL     0x00001220   
// #define DDR_1_SDRAM_MODE_8_VAL     0x00         
// #define DDR_1_INIT_ADDR_VAL        0x00000000   
// #define DDR_1_INIT_EXT_ADDR_VAL    0x00000000   
// #define DDR_1_SDRAM_RCW_1_VAL      0x00000000   
// #define DDR_1_SDRAM_RCW_2_VAL      0x00000000   
// #define DDR_1_DATA_INIT_VAL        0xDEADBEEF   
// #define DDR_1_SDRAM_MD_CNTL_VAL    0x00000000   
// #define DDR_1_CONFIG_DDR1_RCW_1    0x00000000
// #define DDR_1_CONFIG_DDR1_RCW_2    0x00000000
// anterior

/*DDR Controller 1 register values */             
                                                                              /* DDR Controller 1 register values */                                                           
#define DDR_1_CS0_BNDS_VAL        0x7f/* 0xFF        */                      
#define DDR_1_CS1_BNDS_VAL        0x00/* 0xFF        */                      
#define DDR_1_CS2_BNDS_VAL        0x00/* 0x00        */                      
#define DDR_1_CS3_BNDS_VAL        0x00/* 0x00        */                      
#define DDR_1_CS0_CONFIG_VAL      0x80014302/* 0x80044302  */                
#define DDR_1_CS1_CONFIG_VAL      0x00 /* 0x80004302  */                     
#define DDR_1_CS2_CONFIG_VAL      0x00/* 0x00        */                      
#define DDR_1_CS3_CONFIG_VAL      0x00/* 0x00        */                      
#define DDR_1_CS0_CONFIG_2_VAL    0x00/* 0x00        */                      
#define DDR_1_CS1_CONFIG_2_VAL    0x00/* 0x00        */                      
#define DDR_1_CS2_CONFIG_2_VAL    0x00/* 0x00        */                      
#define DDR_1_CS3_CONFIG_2_VAL    0x00/* 0x00        */                      
#define DDR_1_TIMING_CFG_0_VAL    0x40330104/*0x00220104 /* 0x40110104  */    
#define DDR_1_TIMING_CFG_1_VAL    0x6E6B0644/*0x5d5bd546/* 0x5C5BE544  */                 
#define DDR_1_TIMING_CFG_2_VAL    0x0FA89110/*0x0fa8d10b/* 0x0FA888CA  */                 
#define DDR_1_TIMING_CFG_3_VAL    0x00070000/*0x00050000/* 0x00020000  */     
#define DDR_1_TIMING_CFG_4_VAL    0x01      /*0x00220001/* 0x00000001  */                 
#define DDR_1_TIMING_CFG_5_VAL    0x02401400/* 0x02401400  */                 
#define DDR_1_SDRAM_CFG_VAL       0xC7040008/*0xc7040000/* 0xC7044000  */     
#define DDR_1_SDRAM_CFG_2_VAL     0x24401010/*0x24401050/* 0x24401100  */     
#define DDR_1_SDRAM_INTERVAL_VAL  0x0C30030C/*0x0a28028a/* 0x0A280100  */                 
#define DDR_1_SDRAM_CLK_CNTL_VAL  0x02800000/* 0x01800000  */                 
#define DDR_1_ZQ_CNTL_VAL         0x89080600/* 0x89080600  */                 
#define DDR_1_WRLVL_CNTL_VAL      0x8655f607/* 0x8675F605  */                 
#define DDR_1_WRLVL_CNTL_2_VAL    0x00000000/* 0x00        */                 
#define DDR_1_WRLVL_CNTL_3_VAL    0x00000000/* 0x00        */                 
#define DDR_1_SDRAM_MODE_VAL      0x00061420/*0x00061221/* 0x00441220  */     
#define DDR_1_SDRAM_MODE_2_VAL    0x00000000/* 0x00        */                 
#define DDR_1_SDRAM_MODE_3_VAL    0x00000000/* 0x00001220  */                 
#define DDR_1_SDRAM_MODE_4_VAL    0x00000000/* 0x00        */                 
#define DDR_1_SDRAM_MODE_5_VAL    0x00000000/* 0x00001220  */                 
#define DDR_1_SDRAM_MODE_6_VAL    0x00000000/* 0x00        */                 
#define DDR_1_SDRAM_MODE_7_VAL    0x00000000/* 0x00001220  */                 
#define DDR_1_SDRAM_MODE_8_VAL    0x00000000/* 0x00        */                 
#define DDR_1_INIT_ADDR_VAL       0x00000000/* 0x00000000  */                                                                                                                  
#define DDR_1_INIT_EXT_ADDR_VAL   0x00000000/* 0x00000000  */                                                          
#define DDR_1_SDRAM_RCW_1_VAL     0x00000000/* 0x00000000  */                                                          
#define DDR_1_SDRAM_RCW_2_VAL     0x00000000/* 0x00000000  */                                                          
#define DDR_1_DATA_INIT_VAL       0xDEADBEEF/* 0xDEADBEEF  */                                                          
#define DDR_1_SDRAM_MD_CNTL_VAL   0x00000000/* 0x00000000  */                                                          
#define DDR_1_cdr_1               0x80040000/* 0x00000000  */                                                          
#define DDR_1_cdr_2               0x00000000/* 0x00000000  */                                                          
                                                                                                                       
                                                                                                                       
                                                                                                                       
                                                                                                                       
/* DDR Controller 1 configuration global structures */                                                                 
fsl_ddr_cfg_regs_t ddr_cfg_regs_0 = {                                                                                  
  .cs[0].bnds = DDR_1_CS0_BNDS_VAL,                                                                                    
  .cs[1].bnds = DDR_1_CS1_BNDS_VAL,                                                                                    
  .cs[2].bnds = DDR_1_CS2_BNDS_VAL,                                                                                    
  .cs[3].bnds = DDR_1_CS3_BNDS_VAL,                                                                                    
  .cs[0].config = DDR_1_CS0_CONFIG_VAL,                                                                                
  .cs[1].config = DDR_1_CS1_CONFIG_VAL,                                                                                
  .cs[2].config = DDR_1_CS2_CONFIG_VAL,                                                                                
  .cs[3].config = DDR_1_CS3_CONFIG_VAL,                                                                                
  .cs[0].config_2 = DDR_1_CS0_CONFIG_2_VAL,                                                                            
  .cs[1].config_2 =  DDR_1_CS1_CONFIG_2_VAL,                                                                           
  .cs[2].config_2 =  DDR_1_CS2_CONFIG_2_VAL,                                                                           
  .cs[3].config_2 =  DDR_1_CS3_CONFIG_2_VAL,                                                                           
  .timing_cfg_3 = DDR_1_TIMING_CFG_3_VAL,                                                                              
  .timing_cfg_0 = DDR_1_TIMING_CFG_0_VAL,                                                                              
  .timing_cfg_1 = DDR_1_TIMING_CFG_1_VAL,                                                                              
  .timing_cfg_2 = DDR_1_TIMING_CFG_2_VAL,                                                                              
  .ddr_sdram_cfg = DDR_1_SDRAM_CFG_VAL,                                                                                
  .ddr_sdram_cfg_2 = DDR_1_SDRAM_CFG_2_VAL,                                                                            
  .ddr_sdram_mode = DDR_1_SDRAM_MODE_VAL,                                                                              
  .ddr_sdram_mode_2 = DDR_1_SDRAM_MODE_2_VAL,                                                                          
  .ddr_sdram_md_cntl = DDR_1_SDRAM_MD_CNTL_VAL,                                                                        
  .ddr_sdram_interval = DDR_1_SDRAM_INTERVAL_VAL,                                                                      
  .ddr_data_init = DDR_1_DATA_INIT_VAL,                                                                                
  .ddr_sdram_clk_cntl = DDR_1_SDRAM_CLK_CNTL_VAL,                                                                      
  .ddr_init_addr = DDR_1_INIT_ADDR_VAL,                                                                                
  .ddr_init_ext_addr = DDR_1_INIT_EXT_ADDR_VAL,                                                                        
  .timing_cfg_4 = DDR_1_TIMING_CFG_4_VAL,                                                                              
  .timing_cfg_5 = DDR_1_TIMING_CFG_5_VAL,                                                                              
  .ddr_zq_cntl = DDR_1_ZQ_CNTL_VAL,                                                                                    
  .ddr_wrlvl_cntl = DDR_1_WRLVL_CNTL_VAL,                                                                              
  .ddr_sdram_rcw_1 = DDR_1_SDRAM_RCW_1_VAL,                                                                            
  .ddr_sdram_rcw_2 = DDR_1_SDRAM_RCW_1_VAL,
  .ddr_cdr1 = DDR_1_cdr_1, 
  .ddr_cdr2 = DDR_1_cdr_2
};

fixed_ddr_parm_t fixed_ddr_parm_0[] = {
  { DATARATE_500MHZ, DATARATE_1200MHZ, &ddr_cfg_regs_0 },
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
	ulong ddr_freq;

	ddr_freq = get_ddr_freq(0);
	/*ddr_freq_mhz = ddr_freq / 1000000;*/

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

