
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/


/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/
unsigned long int cpb_egress_para_debug = 0;

const CHIP_REG sel_tb_en_reg[]=
{
	{SEL_TB_EN,CPB_EGRESS_BASE_ADDR,0xf,0,0},
};
const CHIP_REG clr_eg_reg[]=
{
	{CLR_EG,CPB_EGRESS_BASE_ADDR,0x0,0,0},
};
const CHIP_REG sq_err_eg_REG[]=
{
	{SQ_ERR_EG_0,CPB_EGRESS_BASE_ADDR,0x10,31,0},
	{SQ_ERR_EG_1,CPB_EGRESS_BASE_ADDR,0x11,31,0},
	{SQ_ERR_EG_2,CPB_EGRESS_BASE_ADDR,0x12,15,0},
};
const CHIP_REG mon_en_eg_reg[]=
{
	{MON_EN_EG_0,CPB_EGRESS_BASE_ADDR,0x13,31,0},
	{MON_EN_EG_1,CPB_EGRESS_BASE_ADDR,0x14,31,0},
	{MON_EN_EG_2,CPB_EGRESS_BASE_ADDR,0x15,15,0},
};
const CHIP_REG sta_clr_ch_eg_reg[]=
{
	{STA_CLR_CH_EG,CPB_EGRESS_BASE_ADDR,0x74,6,0},
};
const CHIP_REG sar_pathdelay_chan_reg[]=
{
	{DEBUG_CHAN,CPB_EGRESS_BASE_ADDR,0x7c,6,0},
};
const CHIP_REG sta_clr_eg_reg[]=
{
	{STA_CLR_EG,CPB_EGRESS_BASE_ADDR,0x7d,0,0},
	{DEBUG_CLR,CPB_EGRESS_BASE_ADDR,0x7d,1,1},
};
const CHIP_REG cnt_max_eg_reg[]=
{
	{CNT_MAX_EG,CPB_EGRESS_BASE_ADDR,0x20,15,0},
};
const CHIP_REG cnt_depth_eg_reg[]=
{
	{CNT_DEPTH_EG,CPB_EGRESS_BASE_ADDR,0x70,15,0},
	{CNT_MIN_EG,CPB_EGRESS_BASE_ADDR,0x70,31,16},	
};
const CHIP_REG sar_pathdelay_stat_reg[]=
{
	{MIN_PATHDELAY,CPB_EGRESS_BASE_ADDR,0x7e,15,0},
	{MAX_PATHDELAY,CPB_EGRESS_BASE_ADDR,0x7e,31,16},
};
const CHIP_REG sar_pathdelay_real_reg[]=
{
	{REAL_PATHDELAY,CPB_EGRESS_BASE_ADDR,0x7f,15,0},
	{STA_CLR_DONE,CPB_EGRESS_BASE_ADDR,0x7f,31,31},
};
const CHIP_REG cpb_ilk_bp_REG[]=
{
	{CPB_ILK_BP_0,CPB_EGRESS_BASE_ADDR,0x71,31,0},
	{CPB_ILK_BP_1,CPB_EGRESS_BASE_ADDR,0x72,31,0},
	{CPB_ILK_BP_2,CPB_EGRESS_BASE_ADDR,0x73,15,0},
};
const CHIP_REG dly_fifo_state_reg[]=
{
	{DLY_FIFO_STATE,CPB_EGRESS_BASE_ADDR,0x75,1,0},
	{DATA_RAM_ADDR_ALM,CPB_EGRESS_BASE_ADDR,0x75,2,2},
};
const CHIP_REG link_full_eg_reg[]=
{
	{LINK_FULL_EG_0,CPB_EGRESS_BASE_ADDR,0x76,31,0},
	{LINK_FULL_EG_1,CPB_EGRESS_BASE_ADDR,0x77,31,0},
	{LINK_FULL_EG_2,CPB_EGRESS_BASE_ADDR,0x78,15,0},
};
const CHIP_REG link_empty_eg_reg[]=
{
	{LINK_EMPTY_EG_0,CPB_EGRESS_BASE_ADDR,0x79,31,0},
	{LINK_EMPTY_EG_1,CPB_EGRESS_BASE_ADDR,0x7a,31,0},
	{LINK_EMPTY_EG_2,CPB_EGRESS_BASE_ADDR,0x7b,15,0},
};
const CHIP_REG cpb_eg0_INT_REG[]=
{
	{CH0_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,1,0},
	{CH1_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,3,2},
	{CH2_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,5,4},
	{CH3_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,7,6},
	{CH4_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,9,8},
	{CH5_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,11,10},
	{CH6_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,13,12},
	{CH7_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,15,14},
	{CH8_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,17,16},
	{CH9_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,19,18},
	{CH10_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,21,20},
	{CH11_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,23,22},
	{CH12_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,25,24},
	{CH13_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,27,26},
	{CH14_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,29,28},
	{CH15_FULL_INT,CPB_EGRESS_BASE_ADDR,0x16,31,30},
};
const CHIP_REG cpb_eg0_INT_MASK_REG[]=
{
	{CH0_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,1,0},
	{CH1_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,3,2},
	{CH2_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,5,4},
	{CH3_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,7,6},
	{CH4_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,9,8},
	{CH5_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,11,10},
	{CH6_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,13,12},
	{CH7_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,15,14},
	{CH8_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,17,16},
	{CH9_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,19,18},
	{CH10_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,21,20},
	{CH11_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,23,22},
	{CH12_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,25,24},
	{CH13_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,27,26},
	{CH14_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,29,28},
	{CH15_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x17,31,30},
};
const CHIP_REG cpb_eg1_INT_REG[]=
{
	{CH16_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,1,0},
	{CH17_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,3,2},
	{CH18_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,5,4},
	{CH19_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,7,6},
	{CH20_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,9,8},
	{CH21_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,11,10},
	{CH22_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,13,12},
	{CH23_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,15,14},
	{CH24_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,17,16},
	{CH25_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,19,18},
	{CH26_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,21,20},
	{CH27_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,23,22},
	{CH28_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,25,24},
	{CH29_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,27,26},
	{CH30_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,29,28},
	{CH31_FULL_INT,CPB_EGRESS_BASE_ADDR,0x18,31,30},
};
const CHIP_REG cpb_eg1_INT_MASK_REG[]=
{
	{CH16_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,1,0},
	{CH17_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,3,2},
	{CH18_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,5,4},
	{CH19_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,7,6},
	{CH20_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,9,8},
	{CH21_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,11,10},
	{CH22_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,13,12},
	{CH23_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,15,14},
	{CH24_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,17,16},
	{CH25_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,19,18},
	{CH26_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,21,20},
	{CH27_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,23,22},
	{CH28_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,25,24},
	{CH29_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,27,26},
	{CH30_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,29,28},
	{CH31_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x19,31,30},
};
const CHIP_REG cpb_eg2_INT_REG[]=
{
	{CH32_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,1,0},
	{CH33_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,3,2},
	{CH34_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,5,4},
	{CH35_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,7,6},
	{CH36_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,9,8},
	{CH37_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,11,10},
	{CH38_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,13,12},
	{CH39_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,15,14},
	{CH40_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,17,16},
	{CH41_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,19,18},
	{CH42_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,21,20},
	{CH43_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,23,22},
	{CH44_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,25,24},
	{CH45_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,27,26},
	{CH46_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,29,28},
	{CH47_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1a,31,30},
};
const CHIP_REG cpb_eg2_INT_MASK_REG[]=
{
	{CH32_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,1,0},
	{CH33_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,3,2},
	{CH34_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,5,4},
	{CH35_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,7,6},
	{CH36_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,9,8},
	{CH37_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,11,10},
	{CH38_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,13,12},
	{CH39_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,15,14},
	{CH40_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,17,16},
	{CH41_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,19,18},
	{CH42_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,21,20},
	{CH43_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,23,22},
	{CH44_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,25,24},
	{CH45_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,27,26},
	{CH46_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,29,28},
	{CH47_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1b,31,30},
};
const CHIP_REG cpb_eg3_INT_REG[]=
{
	{CH48_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,1,0},
	{CH49_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,3,2},
	{CH50_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,5,4},
	{CH51_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,7,6},
	{CH52_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,9,8},
	{CH53_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,11,10},
	{CH54_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,13,12},
	{CH55_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,15,14},
	{CH56_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,17,16},
	{CH57_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,19,18},
	{CH58_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,21,20},
	{CH59_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,23,22},
	{CH60_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,25,24},
	{CH61_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,27,26},
	{CH62_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,29,28},
	{CH63_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1c,31,30},
};
const CHIP_REG cpb_eg3_INT_MASK_REG[]=
{
	{CH48_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,1,0},
	{CH49_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,3,2},
	{CH50_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,5,4},
	{CH51_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,7,6},
	{CH52_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,9,8},
	{CH53_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,11,10},
	{CH54_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,13,12},
	{CH55_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,15,14},
	{CH56_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,17,16},
	{CH57_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,19,18},
	{CH58_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,21,20},
	{CH59_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,23,22},
	{CH60_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,25,24},
	{CH61_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,27,26},
	{CH62_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,29,28},
	{CH63_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1d,31,30},
};
const CHIP_REG cpb_eg4_INT_REG[]=
{
	{CH64_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,1,0},
	{CH65_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,3,2},
	{CH66_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,5,4},
	{CH67_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,7,6},
	{CH68_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,9,8},
	{CH69_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,11,10},
	{CH70_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,13,12},
	{CH71_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,15,14},
	{CH72_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,17,16},
	{CH73_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,19,18},
	{CH74_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,21,20},
	{CH75_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,23,22},
	{CH76_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,25,24},
	{CH77_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,27,26},
	{CH78_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,29,28},
	{CH79_FULL_INT,CPB_EGRESS_BASE_ADDR,0x1e,31,30},
};
const CHIP_REG cpb_eg4_INT_MASK_REG[]=
{
	{CH64_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,1,0},
	{CH65_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,3,2},
	{CH66_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,5,4},
	{CH67_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,7,6},
	{CH68_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,9,8},
	{CH69_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,11,10},
	{CH70_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,13,12},
	{CH71_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,15,14},
	{CH72_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,17,16},
	{CH73_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,19,18},
	{CH74_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,21,20},
	{CH75_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,23,22},
	{CH76_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,25,24},
	{CH77_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,27,26},
	{CH78_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,29,28},
	{CH79_FULL_INT_MASK,CPB_EGRESS_BASE_ADDR,0x1f,31,30},
};
const CHIP_RAM err_cnt_eg_tb = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0x1,64,
};
const CHIP_RAM sop_in_cnt_egress = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0x2,64,
};
const CHIP_RAM sop_out_cnt_egress = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0x3,64,
};
const CHIP_RAM bp_low_egress = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0x4,16,
};
const CHIP_RAM start_blk_tb = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0x5,7,
};
const CHIP_RAM next_blk_tb = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0x6,15,
};
const CHIP_RAM full_depth_egress = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0x7,16,
};
const CHIP_RAM bp_level_egress = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0x8,16,
};
const CHIP_RAM rd_level_egress = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0x9,16,
};
const CHIP_RAM cell_in_cnt_egress = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0xa,64,
};
const CHIP_RAM cell_out_cnt_egress = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0xb,64,
};
const CHIP_RAM high_depth_eg = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0xc,16,
};
const CHIP_RAM low_depth_eg = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0xd,16,
};
const CHIP_RAM aempty_depth = 
{
	TABLE_BASE_ADDR,CPB_EGRESS_BASE_ADDR+0xe,16,
};

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sel_tb_en_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-11-10    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sel_tb_en_set(UINT_8 chip_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (parameter > 0x1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n parameter = 0x%x\r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    regp_field_write(chip_id,sel_tb_en_reg[SEL_TB_EN],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sel_tb_en_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-11-10    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sel_tb_en_get(UINT_8 chip_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    regp_field_read(chip_id,sel_tb_en_reg[SEL_TB_EN],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sq_err_eg_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sq_err_eg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1) || (parameter > 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d, \
                                                      \r\n parameter = 0x%x\r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    regp_bit_write(chip_id,sq_err_eg_REG[SQ_ERR_EG_0].base_addr,sq_err_eg_REG[SQ_ERR_EG_0].offset_addr+channel_id/32,
		channel_id%32,channel_id%32,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sq_err_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sq_err_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    regp_bit_read(chip_id,sq_err_eg_REG[SQ_ERR_EG_0].base_addr,sq_err_eg_REG[SQ_ERR_EG_0].offset_addr+channel_id/32,
		channel_id%32,channel_id%32,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_mon_en_eg_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_mon_en_eg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1) || (parameter > 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d, \
                                                      \r\n parameter = 0x%x\r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    regp_bit_write(chip_id,mon_en_eg_reg[MON_EN_EG_0].base_addr,mon_en_eg_reg[MON_EN_EG_0].offset_addr+channel_id/32,
		channel_id%32,channel_id%32,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_mon_en_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_mon_en_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    regp_bit_read(chip_id,mon_en_eg_reg[MON_EN_EG_0].base_addr,mon_en_eg_reg[MON_EN_EG_0].offset_addr+channel_id/32,
		channel_id%32,channel_id%32,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_clr_eg_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_clr_eg_set(UINT_8 chip_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (parameter > 0x1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n parameter = 0x%x\r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    /*regp_field_write(chip_id,clr_eg_reg[CLR_EG],parameter);*/
    regp_write(chip_id,clr_eg_reg[CLR_EG].base_addr,clr_eg_reg[CLR_EG].offset_addr,1);

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_clr_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_clr_eg_get(UINT_8 chip_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    regp_field_read(chip_id,clr_eg_reg[CLR_EG],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_cnt_max_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_cnt_max_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    regp_bit_read(chip_id,cnt_max_eg_reg[CNT_MAX_EG].base_addr,cnt_max_eg_reg[CNT_MAX_EG].offset_addr+channel_id,
        cnt_max_eg_reg[CNT_MAX_EG].end_bit,cnt_max_eg_reg[CNT_MAX_EG].start_bit,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_cpb_ilk_bp_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-8-7    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_cpb_ilk_bp_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    regp_bit_read(chip_id,cpb_ilk_bp_REG[CPB_ILK_BP_0].base_addr,cpb_ilk_bp_REG[CPB_ILK_BP_0].offset_addr+channel_id/32,
        channel_id%32,channel_id%32,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_link_full_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_link_full_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    regp_bit_read(chip_id,link_full_eg_reg[LINK_FULL_EG_0].base_addr,link_full_eg_reg[LINK_FULL_EG_0].offset_addr+channel_id/32,
        channel_id%32,channel_id%32,parameter);
    
    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_link_empty_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_link_empty_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    regp_bit_read(chip_id,link_empty_eg_reg[LINK_EMPTY_EG_0].base_addr,link_empty_eg_reg[LINK_EMPTY_EG_0].offset_addr+channel_id/32,
        channel_id%32,channel_id%32,parameter);
    
    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sar_pathdelay_chan_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-10   1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sar_pathdelay_chan_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (parameter > 0x7f))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n parameter = 0x%x\r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    regp_field_write(chip_id,sar_pathdelay_chan_reg[DEBUG_CHAN],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sar_pathdelay_chan_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-10   1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sar_pathdelay_chan_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d\r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
	{
		if (cpb_egress_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

    regp_field_read(chip_id,sar_pathdelay_chan_reg[DEBUG_CHAN],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_dly_fifo_state_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id:0:DLY_FIFO_STATE 1:DATA_RAM_ADDR_ALM
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-10   1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_dly_fifo_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d  r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(dly_fifo_state_reg))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    regp_field_read(chip_id,dly_fifo_state_reg[field_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sta_clr_eg_pls_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-10   1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sta_clr_eg_pls_set(UINT_8 chip_id,UINT_8 filed_id)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d  r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

    if (filed_id > MAX_INDEX(sta_clr_eg_reg))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    /*regp_write(chip_id,sta_clr_eg_reg[filed_id].base_addr,sta_clr_eg_reg[filed_id].offset_addr,1);*/
    regp_write(chip_id,sta_clr_eg_reg[filed_id].base_addr,sta_clr_eg_reg[filed_id].offset_addr,1<<filed_id);

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_cnt_depth_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id:CNT_DEPTH_EG,CNT_MIN_EG
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_cnt_depth_eg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d\r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(cnt_depth_eg_reg))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    regp_field_read(chip_id,cnt_depth_eg_reg[field_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sar_pathdelay_stat_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id:MIN_PATHDELAY,MAX_PATHDELAY
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sar_pathdelay_stat_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d\r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(sar_pathdelay_stat_reg))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    regp_field_read(chip_id,sar_pathdelay_stat_reg[field_id],parameter);
    
    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sar_pathdelay_real_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 0:REAL_PATHDELAY  1:STA_CLR_DONE
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sar_pathdelay_real_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d\r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(sar_pathdelay_real_reg))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    regp_field_read(chip_id,sar_pathdelay_real_reg[field_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sta_clr_ch_eg_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-10   1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sta_clr_ch_eg_set(UINT_8 chip_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d  r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

    regp_field_write(chip_id,sta_clr_ch_eg_reg[STA_CLR_CH_EG],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_sta_clr_ch_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-10   1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_sta_clr_ch_eg_get(UINT_8 chip_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d  r\n",
                                                      __FUNCTION__,
                                                      chip_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }

    regp_field_read(chip_id,sta_clr_ch_eg_reg[STA_CLR_CH_EG],parameter);
    
    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_star_blk_eg_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_star_blk_eg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d, \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    ram_write_single(chip_id,start_blk_tb,channel_id,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_star_blk_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_star_blk_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d, \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    ram_read_single(chip_id,start_blk_tb,channel_id,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_next_blk_eg_set
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to set
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_next_blk_eg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d, \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    ram_write_single(chip_id,next_blk_tb,channel_id,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_next_blk_eg_get
*
* DESCRIPTION 
*
*     
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     channel_id:0~CH_NUM - 1
*     parameter*: value to get
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2017-12-21    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_next_blk_eg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d, \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    ram_read_single(chip_id,next_blk_tb,channel_id,parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_full_depth_egress_set
*
* DESCRIPTION 
*
*     Cpb egress full_depth_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_full_depth_egress_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,full_depth_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_full_depth_egress_get
*
* DESCRIPTION 
*
*     Cpb egress full_depth_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_full_depth_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,full_depth_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_bp_level_egress_set
*
* DESCRIPTION 
*
*     Cpb egress bp_level_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_bp_level_egress_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,bp_level_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_bp_level_egress_get
*
* DESCRIPTION 
*
*     Cpb egress bp_level_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_bp_level_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,bp_level_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_rd_level_egress_set
*
* DESCRIPTION 
*
*     Cpb egress rd_level_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-10    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_rd_level_egress_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,rd_level_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_rd_level_egress_get
*
* DESCRIPTION 
*
*     Cpb egress rd_level_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_rd_level_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,rd_level_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_cell_in_cnt_egress_get
*
* DESCRIPTION 
*
*     Cpb egress cell_in_cnt_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_cell_in_cnt_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,cell_in_cnt_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_cell_out_cnt_egress_get
*
* DESCRIPTION 
*
*     Cpb egress cell_out_cnt_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_cell_out_cnt_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,cell_out_cnt_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_high_depth_eg_set
*
* DESCRIPTION 
*
*     Cpb egress cell_out_cnt_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_high_depth_eg_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,high_depth_eg,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_high_depth_eg_get
*
* DESCRIPTION 
*
*     Cpb egress cell_out_cnt_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_high_depth_eg_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,high_depth_eg,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_low_depth_eg_set
*
* DESCRIPTION 
*
*     Cpb egress cell_out_cnt_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_low_depth_eg_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,low_depth_eg,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_low_depth_eg_get
*
* DESCRIPTION 
*
*     Cpb egress cell_out_cnt_egress ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_low_depth_eg_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,low_depth_eg,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_aempty_depth_set
*
* DESCRIPTION 
*
*     Cpb egress aempty_depth.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_aempty_depth_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,aempty_depth,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_aempty_depth_get
*
* DESCRIPTION 
*
*     Cpb egress aempty_depth.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_aempty_depth_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,aempty_depth,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_bp_low_egress_set
*
* DESCRIPTION 
*
*     cpb_egress_bp_low_egress_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-4-24   1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_bp_low_egress_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,bp_low_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_mac_rdy_depth_eg_get
*
* DESCRIPTION 
*
*     Cpb egress mac rdy depth eg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi    2018-4-24   1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_bp_low_egress_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,bp_low_egress,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_err_cnt_eg_tb_get
*
* DESCRIPTION 
*
*     Cpb egress err_cnt_eg_tb ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-9     1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_err_cnt_eg_tb_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,err_cnt_eg_tb,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_all_cnt_ram_get
*
* DESCRIPTION 
*
*     Get cpb egress all cnt ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ram_sel: 0-cell_in_cnt_egress 
*              1-cell_out_cnt_egress     
*              2-err_cnt_eg_tb      
*              3-sop_in_cnt_egress       
*              4-sop_out_cnt_egress  
*
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
*
* RETURNS
*
*     0: success
*     1: fail
*
* CALLS
*
*     RESERVED
*
* CALLED BY
*
*     RESERVED
*
* VERSION
*
*   <author>     <date>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS cpb_egress_all_cnt_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    const CHIP_RAM *p = &cell_in_cnt_egress;
    
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ram_sel > 4) || (index > CH_NUM - 1))
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    switch (ram_sel)
    {
        case 0 : p = &cell_in_cnt_egress ; break;
        case 1 : p = &cell_out_cnt_egress; break;
        case 2 : p = &err_cnt_eg_tb      ; break;
        case 3 : p = &sop_in_cnt_egress  ; break;
        case 4 : p = &sop_out_cnt_egress ; break;
        default: break;
    }
        
    rv = ram_read_single(chip_id,*p,index,parameter);
    
    return rv;
}

RET_STATUS cpb_egress_eg_int_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1) || (parameter > 3))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d, \
                                                      \r\n parameter = 0x%x\r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    switch (channel_id/16)
    {
    	case 0:
			regp_field_write(chip_id,cpb_eg0_INT_REG[channel_id%16],parameter);
			break;
		case 1:
			regp_field_write(chip_id,cpb_eg1_INT_REG[channel_id%16],parameter);
			break;
		case 2:
			regp_field_write(chip_id,cpb_eg2_INT_REG[channel_id%16],parameter);
			break;
		case 3:
			regp_field_write(chip_id,cpb_eg3_INT_REG[channel_id%16],parameter);
			break;
		case 4:
			regp_field_write(chip_id,cpb_eg4_INT_REG[channel_id%16],parameter);
			break;
		default:
			return RET_FAIL;   	
    }
    
    return RET_SUCCESS;
}

RET_STATUS cpb_egress_eg_int_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    } 

    switch (channel_id/16)
    {
    	case 0:
			regp_field_read(chip_id,cpb_eg0_INT_REG[channel_id%16],parameter);
			break;
		case 1:
			regp_field_read(chip_id,cpb_eg1_INT_REG[channel_id%16],parameter);
			break;
		case 2:
			regp_field_read(chip_id,cpb_eg2_INT_REG[channel_id%16],parameter);
			break;
		case 3:
			regp_field_read(chip_id,cpb_eg3_INT_REG[channel_id%16],parameter);
			break;
		case 4:
			regp_field_read(chip_id,cpb_eg4_INT_REG[channel_id%16],parameter);
			break;
		default:
			return RET_FAIL;   	
    }
    
    return RET_SUCCESS;
}

RET_STATUS cpb_egress_eg_int_mask_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1) || (parameter > 3))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d, \
                                                      \r\n parameter = 0x%x\r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    switch (channel_id/16)
    {
    	case 0:
			regp_field_write(chip_id,cpb_eg0_INT_MASK_REG[channel_id%16],parameter);
			break;
		case 1:
			regp_field_write(chip_id,cpb_eg1_INT_MASK_REG[channel_id%16],parameter);
			break;
		case 2:
			regp_field_write(chip_id,cpb_eg2_INT_MASK_REG[channel_id%16],parameter);
			break;
		case 3:
			regp_field_write(chip_id,cpb_eg3_INT_MASK_REG[channel_id%16],parameter);
			break;
		case 4:
			regp_field_write(chip_id,cpb_eg4_INT_MASK_REG[channel_id%16],parameter);
			break;
		default:
			return RET_FAIL;   	
    }
    
    return RET_SUCCESS;
}

RET_STATUS cpb_egress_eg_int_mask_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (channel_id > CH_NUM - 1))
    {
       if (cpb_egress_para_debug)
       {
           printf("[%s] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n channel_id = %d \r\n",
                                                      __FUNCTION__,
                                                      chip_id,
                                                      channel_id);
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (cpb_egress_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    } 

    switch (channel_id/16)
    {
    	case 0:
			regp_field_read(chip_id,cpb_eg0_INT_MASK_REG[channel_id%16],parameter);
			break;
		case 1:
			regp_field_read(chip_id,cpb_eg1_INT_MASK_REG[channel_id%16],parameter);
			break;
		case 2:
			regp_field_read(chip_id,cpb_eg2_INT_MASK_REG[channel_id%16],parameter);
			break;
		case 3:
			regp_field_read(chip_id,cpb_eg3_INT_MASK_REG[channel_id%16],parameter);
			break;
		case 4:
			regp_field_read(chip_id,cpb_eg4_INT_MASK_REG[channel_id%16],parameter);
			break;
		default:
			return RET_FAIL;   	
    }
    
    return RET_SUCCESS;
}

