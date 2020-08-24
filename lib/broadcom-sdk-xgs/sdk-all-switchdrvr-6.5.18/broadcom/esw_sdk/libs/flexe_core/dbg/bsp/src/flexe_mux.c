
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int flexe_mux_para_debug = 0;

const CHIP_REG flexe_mode_reg[]=
{
	{FLEXE_MODE,FLEXE_MUX_BASE_ADDR,0x0,15,0},
	{FLEXE_ENV_MODE,FLEXE_MUX_BASE_ADDR,0x0,16,16},
};
const CHIP_REG flexe_group_cfg_reg[]=
{
	{FLEXE_GROUP_CFG,FLEXE_MUX_BASE_ADDR,0x1,31,0},
};
const CHIP_REG flexe_sch_cfg_reg[]=
{
	{FLEXE_SCH_CFG,FLEXE_MUX_BASE_ADDR,0x2,31,0},
};
const CHIP_REG flexe_msi_cfg_reg[]=
{
	{FLEXE_MSI_CFG_0,FLEXE_MUX_BASE_ADDR,0x3,6,0},
	{FLEXE_MSI_CFG_1,FLEXE_MUX_BASE_ADDR,0x3,14,8},
	{FLEXE_MSI_CFG_2,FLEXE_MUX_BASE_ADDR,0x3,22,16},
	{FLEXE_MSI_CFG_3,FLEXE_MUX_BASE_ADDR,0x3,30,24},
};
const CHIP_REG tsnum_cfg_reg[]=
{
	{TSNUM_CFG_0,FLEXE_MUX_BASE_ADDR,0x17,6,0},
	{TSNUM_CFG_1,FLEXE_MUX_BASE_ADDR,0x17,14,8},
	{TSNUM_CFG_2,FLEXE_MUX_BASE_ADDR,0x17,22,16},
	{TSNUM_CFG_3,FLEXE_MUX_BASE_ADDR,0x17,30,24},
};
const CHIP_REG cycle_cfg_reg[]=
{
	{CYCLE_CFG_0,FLEXE_MUX_BASE_ADDR,0x2b,6,0},
	{CYCLE_CFG_1,FLEXE_MUX_BASE_ADDR,0x2b,14,8},
	{CYCLE_CFG_2,FLEXE_MUX_BASE_ADDR,0x2b,22,16},
	{CYCLE_CFG_3,FLEXE_MUX_BASE_ADDR,0x2b,30,24},
};
const CHIP_REG block_cfg_reg[]=
{
	{BLOCK_CFG_0,FLEXE_MUX_BASE_ADDR,0x3f,31,0},
	{BLOCK_CFG_1,FLEXE_MUX_BASE_ADDR,0x40,31,0},
	{BLOCK_CFG_2,FLEXE_MUX_BASE_ADDR,0x41,31,0},
	{BLOCK_CFG_3,FLEXE_MUX_BASE_ADDR,0x42,31,0},
	{BLOCK_CFG_4,FLEXE_MUX_BASE_ADDR,0x43,31,0},
	{BLOCK_CFG_5,FLEXE_MUX_BASE_ADDR,0x44,31,0},
	{BLOCK_CFG_6,FLEXE_MUX_BASE_ADDR,0x45,31,0},
	{BLOCK_CFG_7,FLEXE_MUX_BASE_ADDR,0x46,31,0},
	{BLOCK_CFG_8,FLEXE_MUX_BASE_ADDR,0x47,31,0},
	{BLOCK_CFG_9,FLEXE_MUX_BASE_ADDR,0x48,31,0},
};
const CHIP_REG ch_belong_flexe_reg[]=
{
	{CH_BELONG_FLEXE_0,FLEXE_MUX_BASE_ADDR,0x49,31,0},
	{CH_BELONG_FLEXE_1,FLEXE_MUX_BASE_ADDR,0x4a,31,0},
	{CH_BELONG_FLEXE_2,FLEXE_MUX_BASE_ADDR,0x4b,31,0},
	{CH_BELONG_FLEXE_3,FLEXE_MUX_BASE_ADDR,0x4c,31,0},
	{CH_BELONG_FLEXE_4,FLEXE_MUX_BASE_ADDR,0x4d,31,0},
	{CH_BELONG_FLEXE_5,FLEXE_MUX_BASE_ADDR,0x4e,31,0},
	{CH_BELONG_FLEXE_6,FLEXE_MUX_BASE_ADDR,0x4f,31,0},
	{CH_BELONG_FLEXE_7,FLEXE_MUX_BASE_ADDR,0x50,31,0},
	{CH_BELONG_FLEXE_8,FLEXE_MUX_BASE_ADDR,0x51,31,0},
	{CH_BELONG_FLEXE_9,FLEXE_MUX_BASE_ADDR,0x52,31,0},
};
const CHIP_REG flexe_pcs_enable_reg[]=
{
	{FLEXE_ENABLE,FLEXE_MUX_BASE_ADDR,0x53,7,0},
	{PCS_ENABLE,FLEXE_MUX_BASE_ADDR,0x53,15,8},
	{PCS_NUM,FLEXE_MUX_BASE_ADDR,0x53,18,16},
};
const CHIP_REG ts_enable_reg[]=
{
	{TS_ENABLE_0,FLEXE_MUX_BASE_ADDR,0x54,31,0},
	{TS_ENABLE_1,FLEXE_MUX_BASE_ADDR,0x55,31,0},
	{TS_ENABLE_2,FLEXE_MUX_BASE_ADDR,0x56,15,0},
};
const CHIP_REG pcs_sch_cfg_reg[]=
{
	{PCS_SCH_CFG,FLEXE_MUX_BASE_ADDR,0x57,31,0},
};
const CHIP_REG pcs_chid_cfg_reg[]=
{
	{PCS_CHID_CFG_0,FLEXE_MUX_BASE_ADDR,0x58,6,0},
	{PCS_CHID_CFG_1,FLEXE_MUX_BASE_ADDR,0x58,14,8},
	{PCS_CHID_CFG_2,FLEXE_MUX_BASE_ADDR,0x58,22,16},
	{PCS_CHID_CFG_3,FLEXE_MUX_BASE_ADDR,0x58,30,24},
};
const CHIP_REG reqcnt_slvl_cfg_reg[]=
{
	{REQCNT_SLVL_CFG,FLEXE_MUX_BASE_ADDR,0x6b,10,0},
	{PCSREQCNT_SLVL_CFG,FLEXE_MUX_BASE_ADDR,0x6b,23,16},
};
const CHIP_REG c_cfg_reg[]=
{
	{C_CFG,FLEXE_MUX_BASE_ADDR,0x6c,7,0},
};
const CHIP_REG flexe_msi_cfg_b_reg[]=
{
	{FLEXE_MSI_CFG_B0,FLEXE_MUX_BASE_ADDR,0x6d,6,0},
	{FLEXE_MSI_CFG_B1,FLEXE_MUX_BASE_ADDR,0x6d,14,8},
	{FLEXE_MSI_CFG_B2,FLEXE_MUX_BASE_ADDR,0x6d,22,16},
	{FLEXE_MSI_CFG_B3,FLEXE_MUX_BASE_ADDR,0x6d,30,24},
};
const CHIP_REG tsnum_cfg_b_reg[]=
{
	{TSNUM_CFG_B0,FLEXE_MUX_BASE_ADDR,0x81,6,0},
	{TSNUM_CFG_B1,FLEXE_MUX_BASE_ADDR,0x81,14,8},
	{TSNUM_CFG_B2,FLEXE_MUX_BASE_ADDR,0x81,22,16},
	{TSNUM_CFG_B3,FLEXE_MUX_BASE_ADDR,0x81,30,24},
};
const CHIP_REG cycle_cfg_b_reg[]=
{
	{CYCLE_CFG_B0,FLEXE_MUX_BASE_ADDR,0x95,6,0},
	{CYCLE_CFG_B1,FLEXE_MUX_BASE_ADDR,0x95,14,8},
	{CYCLE_CFG_B2,FLEXE_MUX_BASE_ADDR,0x95,22,16},
	{CYCLE_CFG_B3,FLEXE_MUX_BASE_ADDR,0x95,30,24},
};
const CHIP_REG block_cfg_b_reg[]=
{
	{BLOCK_CFG_B_0,FLEXE_MUX_BASE_ADDR,0xa9,31,0},
	{BLOCK_CFG_B_1,FLEXE_MUX_BASE_ADDR,0xaa,31,0},
	{BLOCK_CFG_B_2,FLEXE_MUX_BASE_ADDR,0xab,31,0},
	{BLOCK_CFG_B_3,FLEXE_MUX_BASE_ADDR,0xac,31,0},
	{BLOCK_CFG_B_4,FLEXE_MUX_BASE_ADDR,0xad,31,0},
	{BLOCK_CFG_B_5,FLEXE_MUX_BASE_ADDR,0xae,31,0},
	{BLOCK_CFG_B_6,FLEXE_MUX_BASE_ADDR,0xaf,31,0},
	{BLOCK_CFG_B_7,FLEXE_MUX_BASE_ADDR,0xb0,31,0},
	{BLOCK_CFG_B_8,FLEXE_MUX_BASE_ADDR,0xb1,31,0},
	{BLOCK_CFG_B_9,FLEXE_MUX_BASE_ADDR,0xb2,31,0},
};
const CHIP_REG ts_enable_b_reg[]=
{
	{TS_ENABLE_B_0,FLEXE_MUX_BASE_ADDR,0xb3,31,0},
	{TS_ENABLE_B_1,FLEXE_MUX_BASE_ADDR,0xb4,31,0},
	{TS_ENABLE_B_2,FLEXE_MUX_BASE_ADDR,0xb5,15,0},
};
const CHIP_REG cphy2_cfg_reg[]=
{
	{CPHY2_CFG_0,FLEXE_MUX_BASE_ADDR,0xc1,2,0},
	{CPHY2_CFG_1,FLEXE_MUX_BASE_ADDR,0xc1,6,4},
	{CPHY2_CFG_2,FLEXE_MUX_BASE_ADDR,0xc1,10,8},
	{CPHY2_CFG_3,FLEXE_MUX_BASE_ADDR,0xc1,14,12},
	{CPHY2_CFG_4,FLEXE_MUX_BASE_ADDR,0xc1,18,16},
	{CPHY2_CFG_5,FLEXE_MUX_BASE_ADDR,0xc1,22,20},
	{CPHY2_CFG_6,FLEXE_MUX_BASE_ADDR,0xc1,26,24},
	{CPHY2_CFG_7,FLEXE_MUX_BASE_ADDR,0xc1,30,28},
};
const CHIP_REG cnt8_sch_cfg_reg[]=
{
	{CNT8_SCH_CFG_0,FLEXE_MUX_BASE_ADDR,0xc2,2,0},
	{CNT8_SCH_CFG_1,FLEXE_MUX_BASE_ADDR,0xc2,6,4},
	{CNT8_SCH_CFG_2,FLEXE_MUX_BASE_ADDR,0xc2,10,8},
	{CNT8_SCH_CFG_3,FLEXE_MUX_BASE_ADDR,0xc2,14,12},
	{CNT8_SCH_CFG_4,FLEXE_MUX_BASE_ADDR,0xc2,18,16},
	{CNT8_SCH_CFG_5,FLEXE_MUX_BASE_ADDR,0xc2,22,20},
	{CNT8_SCH_CFG_6,FLEXE_MUX_BASE_ADDR,0xc2,26,24},
	{CNT8_SCH_CFG_7,FLEXE_MUX_BASE_ADDR,0xc2,30,28},
};
const CHIP_REG m_cfg_mux_reg[]=
{
	{M_CFG_MUX,FLEXE_MUX_BASE_ADDR,0xc3,31,0},
};
const CHIP_REG base_m_cfg_mux_reg[]=
{
	{BASE_M_CFG_MUX,FLEXE_MUX_BASE_ADDR,0xc4,31,0},
};
const CHIP_REG en_sel_cfg_reg[]=
{
	{EN_SEL_CFG_0,FLEXE_MUX_BASE_ADDR,0xc5,2,0},
	{EN_SEL_CFG_1,FLEXE_MUX_BASE_ADDR,0xc5,6,4},
	{EN_SEL_CFG_2,FLEXE_MUX_BASE_ADDR,0xc5,10,8},
	{EN_SEL_CFG_3,FLEXE_MUX_BASE_ADDR,0xc5,14,12},
	{EN_SEL_CFG_4,FLEXE_MUX_BASE_ADDR,0xc5,18,16},
	{EN_SEL_CFG_5,FLEXE_MUX_BASE_ADDR,0xc5,22,20},
	{EN_SEL_CFG_6,FLEXE_MUX_BASE_ADDR,0xc5,26,24},
	{EN_SEL_CFG_7,FLEXE_MUX_BASE_ADDR,0xc5,30,28},
};
const CHIP_REG inst_sel_cfg_reg[]=
{
	{INST_SEL_CFG_0,FLEXE_MUX_BASE_ADDR,0xc6,2,0},
	{INST_SEL_CFG_1,FLEXE_MUX_BASE_ADDR,0xc6,6,4},
	{INST_SEL_CFG_2,FLEXE_MUX_BASE_ADDR,0xc6,10,8},
	{INST_SEL_CFG_3,FLEXE_MUX_BASE_ADDR,0xc6,14,12},
	{INST_SEL_CFG_4,FLEXE_MUX_BASE_ADDR,0xc6,18,16},
	{INST_SEL_CFG_5,FLEXE_MUX_BASE_ADDR,0xc6,22,20},
	{INST_SEL_CFG_6,FLEXE_MUX_BASE_ADDR,0xc6,26,24},
	{INST_SEL_CFG_7,FLEXE_MUX_BASE_ADDR,0xc6,30,28},
};
const CHIP_REG ram_conflict_alm_reg[]=
{
	{RAM_CONFLICT_ALM_0,FLEXE_MUX_BASE_ADDR,0x65,31,0},
	{RAM_CONFLICT_ALM_1,FLEXE_MUX_BASE_ADDR,0x66,31,0},
	{RAM_CONFLICT_ALM_2,FLEXE_MUX_BASE_ADDR,0x67,31,0},
	{RAM_CONFLICT_ALM_3,FLEXE_MUX_BASE_ADDR,0x68,3,0},
};
const CHIP_REG sys_signal_fifo_alm_reg[]=
{
	{SYS_SGL_FIFO_EMPTY,FLEXE_MUX_BASE_ADDR,0x69,7,0},
	{SYS_SGL_FIFO_FULL,FLEXE_MUX_BASE_ADDR,0x69,15,8},
};
const CHIP_REG cfg_pls_reg[]=
{
	{CFG_PLS,FLEXE_MUX_BASE_ADDR,0x6a,0,0},
};
const CHIP_RAM wr_cfg = 
{
	TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x5a,80,
};
const CHIP_RAM rd_cfg[] = 
{
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x5b,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x5c,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x5d,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x5e,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x5f,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x60,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x61,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x62,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x63,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0x64,4},
};
const CHIP_RAM wr_cfg_b = 
{
	TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xb6,80,
};
const CHIP_RAM rd_cfg_b[] = 
{
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xb7,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xb8,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xb9,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xba,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xbb,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xbc,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xbd,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xbe,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xbf,4},
	{TABLE_BASE_ADDR,FLEXE_MUX_BASE_ADDR+0xc0,4},
};

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_mode_set
*
* DESCRIPTION
*
*     Flexe mux mode cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     flexe_sel: 0-FlexE1, 1-FlexE2, 2-FlexE3, 3-FlexE4
**    parameter: 1 is 50G-FlexE, 0 is 100G-FlexE
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_mode_set(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (flexe_sel > 7) ||
        (parameter > 3))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n flexe_sel = %d, \
                   \r\n parameter = %d \r\n",   
                   __FUNCTION__,
                   chip_id,
                   flexe_sel,
                   parameter
                 );
       }

       return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,flexe_mode_reg[FLEXE_MODE].base_addr,flexe_mode_reg[FLEXE_MODE].offset_addr,flexe_sel*2+1,flexe_sel*2,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_mode_get
*
* DESCRIPTION
*
*     Flexe mux mode cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     flexe_sel: 0-FlexE1, 1-FlexE2, 2-FlexE3, 3-FlexE4
**    *parameter: pointer to mode, 1 is 50G-FlexE, 0 is 100G-FlexE
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_mode_get(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (flexe_sel > 7))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n flexe_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   flexe_sel
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
    rv = regp_bit_read(chip_id,flexe_mode_reg[FLEXE_MODE].base_addr,flexe_mode_reg[FLEXE_MODE].offset_addr,flexe_sel*2+1,flexe_sel*2,parameter);
    
    return rv;
}


RET_STATUS flexe_mux_env_mode_set(UINT_8 chip_id, UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (parameter > 1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n parameter = %d \r\n",   
                   __FUNCTION__,
                   chip_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if (FLEXE_ENV_MODE > MAX_INDEX(flexe_mode_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,flexe_mode_reg[FLEXE_ENV_MODE].base_addr,flexe_mode_reg[FLEXE_ENV_MODE].offset_addr,
		                flexe_mode_reg[FLEXE_ENV_MODE].end_bit,flexe_mode_reg[FLEXE_ENV_MODE].start_bit,parameter);
    
    return rv;
}

RET_STATUS flexe_mux_env_mode_get(UINT_8 chip_id, UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (FLEXE_ENV_MODE > MAX_INDEX(flexe_mode_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,flexe_mode_reg[FLEXE_ENV_MODE].base_addr,flexe_mode_reg[FLEXE_ENV_MODE].offset_addr,
		               flexe_mode_reg[FLEXE_ENV_MODE].end_bit,flexe_mode_reg[FLEXE_ENV_MODE].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_group_cfg_set
*
* DESCRIPTION
*
*     Flexe mux group cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     flexe_sel: 0-FlexE1, 1-FlexE2, 2-FlexE3, ..., 7-FlexE8
**    parameter: group cfg value
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_group_cfg_set(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (flexe_sel > 7))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n flexe_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   flexe_sel
                 );
       }

       return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,flexe_group_cfg_reg[FLEXE_GROUP_CFG].base_addr,flexe_group_cfg_reg[FLEXE_GROUP_CFG].offset_addr,(flexe_sel*4+3)%32,(flexe_sel*4)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_group_cfg_get
*
* DESCRIPTION
*
*     Flexe mux group cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     flexe_sel: 0-FlexE1, 1-FlexE2, 2-FlexE3, ..., 7-FlexE8
**    *parameter: pointer to group cfg value
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_group_cfg_get(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (flexe_sel > 7))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n flexe_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   flexe_sel
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,flexe_group_cfg_reg[FLEXE_GROUP_CFG].base_addr,flexe_group_cfg_reg[FLEXE_GROUP_CFG].offset_addr,(flexe_sel*4+3)%32,(flexe_sel*4)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_sch_cfg_set
*
* DESCRIPTION
*
*     Flexe mux sch cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     sch_opp: Select which scheduling opportunity, 0~7
**    parameter: sch indication
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_sch_cfg_set(UINT_8 chip_id,UINT_8 sch_opp,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (sch_opp > 7) ||
        (parameter > 0xf))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n sch_opp = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   sch_opp,
                   parameter
                 );
       }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,flexe_sch_cfg_reg[FLEXE_SCH_CFG].base_addr,flexe_sch_cfg_reg[FLEXE_SCH_CFG].offset_addr,
        sch_opp*4+3,sch_opp*4,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_sch_cfg_get
*
* DESCRIPTION
*
*     Flexe mux sch cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     sch_opp: Select which scheduling opportunity, 0~7
**    *parameter: pointer to sch indication
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_sch_cfg_get(UINT_8 chip_id,UINT_8 sch_opp,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (sch_opp > 7))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n sch_opp = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   sch_opp
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
    rv = regp_bit_read(chip_id,flexe_sch_cfg_reg[FLEXE_SCH_CFG].base_addr,flexe_sch_cfg_reg[FLEXE_SCH_CFG].offset_addr,
        sch_opp*4+3,sch_opp*4,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_msi_cfg_set
*
* DESCRIPTION
*
*     Flexe mux msi cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ts_sel: 0-ts0, 1-ts1, 2-ts2, ..., 79-ts79
**    parameter: channel id cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_msi_cfg_set(UINT_8 chip_id,UINT_8 ts_sel,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ts_sel > (TS_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ts_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ts_sel
                 );
       }

       return RET_PARAERR;
    }

    if ((ts_sel/20) > MAX_INDEX(flexe_msi_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,flexe_msi_cfg_reg[ts_sel/20].base_addr,flexe_msi_cfg_reg[ts_sel/20].offset_addr+ts_sel%20,
        flexe_msi_cfg_reg[ts_sel/20].end_bit,flexe_msi_cfg_reg[ts_sel/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_msi_cfg_get
*
* DESCRIPTION
*
*     Flexe mux msi cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ts_sel: 0-ts0, 1-ts1, 2-ts2, ..., 79-ts79
**    *parameter: pointer to channel id cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_msi_cfg_get(UINT_8 chip_id,UINT_8 ts_sel,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ts_sel > (TS_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ts_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ts_sel
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ts_sel/20) > MAX_INDEX(flexe_msi_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,flexe_msi_cfg_reg[ts_sel/20].base_addr,flexe_msi_cfg_reg[ts_sel/20].offset_addr+ts_sel%20,
        flexe_msi_cfg_reg[ts_sel/20].end_bit,flexe_msi_cfg_reg[ts_sel/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_tsnum_cfg_set
*
* DESCRIPTION
*
*     Flexe mux tsnum cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    parameter: channel tsnum
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_tsnum_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/20) > MAX_INDEX(tsnum_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,tsnum_cfg_reg[ch_id/20].base_addr,tsnum_cfg_reg[ch_id/20].offset_addr+ch_id%20,
        tsnum_cfg_reg[ch_id/20].end_bit,tsnum_cfg_reg[ch_id/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_tsnum_cfg_get
*
* DESCRIPTION
*
*     Flexe mux tsnum cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    *parameter: pointer to channel tsnum
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_tsnum_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/20) > MAX_INDEX(tsnum_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,tsnum_cfg_reg[ch_id/20].base_addr,tsnum_cfg_reg[ch_id/20].offset_addr+ch_id%20,
        tsnum_cfg_reg[ch_id/20].end_bit,tsnum_cfg_reg[ch_id/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_cycle_cfg_set
*
* DESCRIPTION
*
*     Flexe mux cycle cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    parameter: cycle cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_cycle_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/20) > MAX_INDEX(cycle_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,cycle_cfg_reg[ch_id/20].base_addr,cycle_cfg_reg[ch_id/20].offset_addr+ch_id%20,
        cycle_cfg_reg[ch_id/20].end_bit,cycle_cfg_reg[ch_id/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_cycle_cfg_get
*
* DESCRIPTION
*
*     Flexe mux cycle cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    *parameter: pointer to cycle cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_cycle_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/20) > MAX_INDEX(cycle_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,cycle_cfg_reg[ch_id/20].base_addr,cycle_cfg_reg[ch_id/20].offset_addr+ch_id%20,
        cycle_cfg_reg[ch_id/20].end_bit,cycle_cfg_reg[ch_id/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_block_cfg_set
*
* DESCRIPTION
*
*     Flexe mux block cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    parameter: block cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_block_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/8) > MAX_INDEX(block_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,block_cfg_reg[ch_id/8].base_addr,block_cfg_reg[ch_id/8].offset_addr,
        (ch_id*4+3)%32,(ch_id*4)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_block_cfg_get
*
* DESCRIPTION
*
*     Flexe mux block cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    *parameter: pointer to block cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_block_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/8) > MAX_INDEX(block_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,block_cfg_reg[ch_id/8].base_addr,block_cfg_reg[ch_id/8].offset_addr,
        (ch_id*4+3)%32,(ch_id*4)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_ch_belong_flexe_set
*
* DESCRIPTION
*
*     Flexe mux ch belong flexe cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    parameter: ch belong flexe cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_ch_belong_flexe_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/8) > MAX_INDEX(ch_belong_flexe_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,ch_belong_flexe_reg[ch_id/8].base_addr,ch_belong_flexe_reg[ch_id/8].offset_addr,
        (ch_id*4+3)%32,(ch_id*4)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_ch_belong_flexe_get
*
* DESCRIPTION
*
*     Flexe mux ch belong flexe cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    *parameter: pointer to ch belong flexe cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_ch_belong_flexe_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/8) > MAX_INDEX(ch_belong_flexe_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,ch_belong_flexe_reg[ch_id/8].base_addr,ch_belong_flexe_reg[ch_id/8].offset_addr,
        (ch_id*4+3)%32,(ch_id*4)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_flexe_enable_set
*
* DESCRIPTION
*
*     Flexe mux flexe enable.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     flexe_sel: 0-FlexE1, 1-FlexE2, 2-FlexE3, ..., 7-FlexE8
**    parameter: enable cfg, 0-diable, 1-enable
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_flexe_enable_set(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (flexe_sel > (PHY_NUM-1)) ||
        (parameter > 1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n flexe_sel = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   flexe_sel,
                   parameter
                 );
       }

       return RET_PARAERR;
    }
   
    rv = regp_bit_write(chip_id,flexe_pcs_enable_reg[FLEXE_ENABLE].base_addr,flexe_pcs_enable_reg[FLEXE_ENABLE].offset_addr,
        flexe_sel,flexe_sel,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_flexe_enable_get
*
* DESCRIPTION
*
*     Flexe mux flexe enable.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     flexe_sel: 0-FlexE1, 1-FlexE2, 2-FlexE3, ..., 7-FlexE8
**   *parameter: pointer to enable cfg, 0-diable, 1-enable
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_flexe_enable_get(UINT_8 chip_id,UINT_8 flexe_sel,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (flexe_sel > (PHY_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n flexe_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   flexe_sel
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,flexe_pcs_enable_reg[FLEXE_ENABLE].base_addr,flexe_pcs_enable_reg[FLEXE_ENABLE].offset_addr,
        flexe_sel,flexe_sel,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_pcs_enable_set
*
* DESCRIPTION
*
*     Flexe mux pcs enable.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: enable cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_pcs_enable_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (parameter > 0xff))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n parameter = 0x%x  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if (PCS_ENABLE > MAX_INDEX(flexe_pcs_enable_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_write(chip_id,flexe_pcs_enable_reg[PCS_ENABLE],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_pcs_enable_get
*
* DESCRIPTION
*
*     Flexe mux pcs enable.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**   *parameter: pointer to enable cfg
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_pcs_enable_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (PCS_ENABLE > MAX_INDEX(flexe_pcs_enable_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,flexe_pcs_enable_reg[PCS_ENABLE],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_pcs_num_set
*
* DESCRIPTION
*
*     Flexe mux pcs num.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: pcs num cfg, 0-one pcs, 1-two pcs, ..., 7-eight pcs
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_pcs_num_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (parameter > 7))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if (PCS_NUM > MAX_INDEX(flexe_pcs_enable_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,flexe_pcs_enable_reg[PCS_NUM].base_addr,flexe_pcs_enable_reg[PCS_NUM].offset_addr,
        flexe_pcs_enable_reg[PCS_NUM].end_bit,flexe_pcs_enable_reg[PCS_NUM].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_pcs_num_get
*
* DESCRIPTION
*
*     Flexe mux pcs num.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**   *parameter: pointer to pcs num cfg, 0-one pcs, 1-two pcs, ..., 7-eight pcs
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_pcs_num_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (PCS_NUM > MAX_INDEX(flexe_pcs_enable_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,flexe_pcs_enable_reg[PCS_NUM].base_addr,flexe_pcs_enable_reg[PCS_NUM].offset_addr,
        flexe_pcs_enable_reg[PCS_NUM].end_bit,flexe_pcs_enable_reg[PCS_NUM].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_ts_enable_set
*
* DESCRIPTION
*
*     Flexe mux ts enable.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ts_sel: 0-ts0, 1-ts1, 2-ts2, ..., 79-ts79
**    parameter: ts enable, 0-diable, 1-enable
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_ts_enable_set(UINT_8 chip_id,UINT_8 ts_sel,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ts_sel > (TS_NUM-1)) ||
        (parameter > 1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ts_sel = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ts_sel,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if ((ts_sel/32) > MAX_INDEX(ts_enable_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,ts_enable_reg[ts_sel/32].base_addr,ts_enable_reg[ts_sel/32].offset_addr,
        ts_sel%32,ts_sel%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_ts_enable_get
*
* DESCRIPTION
*
*     Flexe mux ts enable.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ts_sel: 0-ts0, 1-ts1, 2-ts2, ..., 79-ts79
**    *parameter: pointer to ts enable, 0-diable, 1-enable
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_ts_enable_get(UINT_8 chip_id,UINT_8 ts_sel,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ts_sel > (TS_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ts_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ts_sel
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ts_sel/32) > MAX_INDEX(ts_enable_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,ts_enable_reg[ts_sel/32].base_addr,ts_enable_reg[ts_sel/32].offset_addr,
        ts_sel%32,ts_sel%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_pcs_sch_cfg_set
*
* DESCRIPTION
*
*     Flexe mux pcs sch cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     sch_opp: Select which scheduling opportunity, 0~7
**    parameter: sch for which pcs, 1~8
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_pcs_sch_cfg_set(UINT_8 chip_id,UINT_8 sch_opp,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (sch_opp > 7) ||
        (parameter < 1 || parameter > 8))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n sch_opp = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   sch_opp,
                   parameter
                 );
       }

       return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,pcs_sch_cfg_reg[PCS_SCH_CFG].base_addr,pcs_sch_cfg_reg[PCS_SCH_CFG].offset_addr,
        sch_opp*4+3,sch_opp*4,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_pcs_sch_cfg_get
*
* DESCRIPTION
*
*     Flexe mux pcs sch cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     sch_opp: Select which scheduling opportunity, 0~7
**   *parameter: pointer to sch for which pcs, 1~8
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_pcs_sch_cfg_get(UINT_8 chip_id,UINT_8 sch_opp,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (sch_opp > 7))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n sch_opp = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   sch_opp
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
    rv = regp_bit_read(chip_id,pcs_sch_cfg_reg[PCS_SCH_CFG].base_addr,pcs_sch_cfg_reg[PCS_SCH_CFG].offset_addr,
        sch_opp*4+3,sch_opp*4,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_pcs_chid_cfg_set
*
* DESCRIPTION
*
*     Flexe mux pcs chid cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     pcs_sel: 0-PCS1, 1-PCS2, 2-PCS3, ..., 7-PCS8
**    parameter: chid cfg, 0~79
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_pcs_chid_cfg_set(UINT_8 chip_id,UINT_8 pcs_sel,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (pcs_sel > (PHY_NUM-1)) ||
        (parameter > 0x7f))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n pcs_sel = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   pcs_sel,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if ((pcs_sel/2) > MAX_INDEX(pcs_chid_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
	if (0 == (pcs_sel%2))
	{
		regp_bit_write(chip_id,pcs_chid_cfg_reg[pcs_sel/2].base_addr,pcs_chid_cfg_reg[pcs_sel/2].offset_addr,
			pcs_chid_cfg_reg[pcs_sel/2].end_bit,pcs_chid_cfg_reg[pcs_sel/2].start_bit,parameter);
	}
	else 
	{
		regp_bit_write(chip_id,pcs_chid_cfg_reg[pcs_sel/2].base_addr,pcs_chid_cfg_reg[pcs_sel/2].offset_addr+1,
			pcs_chid_cfg_reg[pcs_sel/2].end_bit,pcs_chid_cfg_reg[pcs_sel/2].start_bit,parameter);
	}
	    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_pcs_chid_cfg_get
*
* DESCRIPTION
*
*     Flexe mux pcs chid cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     pcs_sel: 0-PCS1, 1-PCS2, 2-PCS3, ..., 7-PCS8
**   *parameter: pointer to chid cfg, 0~79
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_pcs_chid_cfg_get(UINT_8 chip_id,UINT_8 pcs_sel,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (pcs_sel > (PHY_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n pcs_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   pcs_sel
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((pcs_sel/2) > MAX_INDEX(pcs_chid_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
	if (0 == (pcs_sel%2))
	{
		regp_bit_read(chip_id,pcs_chid_cfg_reg[pcs_sel/2].base_addr,pcs_chid_cfg_reg[pcs_sel/2].offset_addr,
			pcs_chid_cfg_reg[pcs_sel/2].end_bit,pcs_chid_cfg_reg[pcs_sel/2].start_bit,parameter);
	}
	else 
	{
		regp_bit_read(chip_id,pcs_chid_cfg_reg[pcs_sel/2].base_addr,pcs_chid_cfg_reg[pcs_sel/2].offset_addr+1,
			pcs_chid_cfg_reg[pcs_sel/2].end_bit,pcs_chid_cfg_reg[pcs_sel/2].start_bit,parameter);
	}
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_reqcnt_slvl_cfg_set
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
*     field_id: 0:REQCNT_SLVL_CFG  1:PCSREQCNT_SLVL_CFG
**    parameter: set value
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
*    dingyi     2018-6-7     1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_reqcnt_slvl_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! chip_id = %d \r\n",__FUNCTION__,chip_id);
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(reqcnt_slvl_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
	regp_field_write(chip_id,reqcnt_slvl_cfg_reg[field_id],parameter);
	    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_reqcnt_slvl_cfg_get
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
*     field_id: 0:REQCNT_SLVL_CFG  1:PCSREQCNT_SLVL_CFG
**    parameter: get value
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
*    dingyi     2018-6-7     1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_reqcnt_slvl_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! chip_id = %d \r\n",__FUNCTION__,chip_id);
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(reqcnt_slvl_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
	regp_field_read(chip_id,reqcnt_slvl_cfg_reg[field_id],parameter);
	    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_c_cfg_set
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
*     phy_id: 0~7
**    parameter: set value
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
*    dingyi     2018-6-7     1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_c_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM - 1)) || (parameter > 1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! chip_id = %d, phy_id = %d, parameter = %d \r\n",
		   	__FUNCTION__,chip_id,phy_id,parameter);
       }

       return RET_PARAERR;
    }
    
	regp_bit_write(chip_id,c_cfg_reg[C_CFG].base_addr,c_cfg_reg[C_CFG].offset_addr,phy_id,phy_id,parameter);
	    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_c_cfg_get
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
*     phy_id: 0~7
**    parameter: get value
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
*    dingyi     2018-6-7     1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_c_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM - 1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! chip_id = %d, phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
	regp_bit_read(chip_id,c_cfg_reg[C_CFG].base_addr,c_cfg_reg[C_CFG].offset_addr,phy_id,phy_id,parameter);
	    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_msi_cfg_b_set
*
* DESCRIPTION
*
*     flexe_mux_msi_cfg_b_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ts_sel: 0-ts0, 1-ts1, 2-ts2, ..., 79-ts79
**    parameter: channel id cfg
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_msi_cfg_b_set(UINT_8 chip_id,UINT_8 ts_sel,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ts_sel > (TS_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ts_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ts_sel
                 );
       }

       return RET_PARAERR;
    }

    if ((ts_sel/20) > MAX_INDEX(flexe_msi_cfg_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,flexe_msi_cfg_b_reg[ts_sel/20].base_addr,flexe_msi_cfg_b_reg[ts_sel/20].offset_addr+ts_sel%20,
        flexe_msi_cfg_b_reg[ts_sel/20].end_bit,flexe_msi_cfg_b_reg[ts_sel/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_msi_cfg_b_get
*
* DESCRIPTION
*
*     flexe_mux_msi_cfg_b_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ts_sel: 0-ts0, 1-ts1, 2-ts2, ..., 79-ts79
**    *parameter: pointer to channel id cfg
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_msi_cfg_b_get(UINT_8 chip_id,UINT_8 ts_sel,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ts_sel > (TS_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ts_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ts_sel
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ts_sel/20) > MAX_INDEX(flexe_msi_cfg_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,flexe_msi_cfg_b_reg[ts_sel/20].base_addr,flexe_msi_cfg_b_reg[ts_sel/20].offset_addr+ts_sel%20,
        flexe_msi_cfg_b_reg[ts_sel/20].end_bit,flexe_msi_cfg_b_reg[ts_sel/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_tsnum_cfg_b_set
*
* DESCRIPTION
*
*     flexe_mux_tsnum_cfg_b_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    parameter: channel tsnum
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_tsnum_cfg_b_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/20) > MAX_INDEX(tsnum_cfg_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,tsnum_cfg_b_reg[ch_id/20].base_addr,tsnum_cfg_b_reg[ch_id/20].offset_addr+ch_id%20,
        tsnum_cfg_b_reg[ch_id/20].end_bit,tsnum_cfg_b_reg[ch_id/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_tsnum_cfg_b_get
*
* DESCRIPTION
*
*     flexe_mux_tsnum_cfg_b_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    *parameter: pointer to channel tsnum
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_tsnum_cfg_b_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/20) > MAX_INDEX(tsnum_cfg_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,tsnum_cfg_b_reg[ch_id/20].base_addr,tsnum_cfg_b_reg[ch_id/20].offset_addr+ch_id%20,
        tsnum_cfg_b_reg[ch_id/20].end_bit,tsnum_cfg_b_reg[ch_id/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_cycle_cfg_b_set
*
* DESCRIPTION
*
*     flexe_mux_cycle_cfg_b_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    parameter: cycle cfg
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_cycle_cfg_b_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/20) > MAX_INDEX(cycle_cfg_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,cycle_cfg_b_reg[ch_id/20].base_addr,cycle_cfg_b_reg[ch_id/20].offset_addr+ch_id%20,
        cycle_cfg_b_reg[ch_id/20].end_bit,cycle_cfg_b_reg[ch_id/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_cycle_cfg_b_get
*
* DESCRIPTION
*
*     flexe_mux_cycle_cfg_b_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    *parameter: pointer to cycle cfg
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_cycle_cfg_b_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/20) > MAX_INDEX(cycle_cfg_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,cycle_cfg_b_reg[ch_id/20].base_addr,cycle_cfg_b_reg[ch_id/20].offset_addr+ch_id%20,
        cycle_cfg_b_reg[ch_id/20].end_bit,cycle_cfg_b_reg[ch_id/20].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_block_cfg_b_set
*
* DESCRIPTION
*
*     flexe_mux_block_cfg_b_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    parameter: block cfg
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_block_cfg_b_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/8) > MAX_INDEX(block_cfg_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,block_cfg_b_reg[ch_id/8].base_addr,block_cfg_b_reg[ch_id/8].offset_addr,
        (ch_id*4+3)%32,(ch_id*4)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_block_cfg_b_get
*
* DESCRIPTION
*
*    flexe_mux_block_cfg_b_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~79
**    *parameter: pointer to block cfg
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_block_cfg_b_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > (CH_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/8) > MAX_INDEX(block_cfg_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,block_cfg_b_reg[ch_id/8].base_addr,block_cfg_b_reg[ch_id/8].offset_addr,
        (ch_id*4+3)%32,(ch_id*4)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_ts_enable_b_set
*
* DESCRIPTION
*
*     flexe_mux_ts_enable_b_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ts_sel: 0-ts0, 1-ts1, 2-ts2, ..., 79-ts79
**    parameter: ts enable, 0-diable, 1-enable
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_ts_enable_b_set(UINT_8 chip_id,UINT_8 ts_sel,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ts_sel > (TS_NUM-1)) ||
        (parameter > 1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ts_sel = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ts_sel,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if ((ts_sel/32) > MAX_INDEX(ts_enable_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,ts_enable_b_reg[ts_sel/32].base_addr,ts_enable_b_reg[ts_sel/32].offset_addr,
        ts_sel%32,ts_sel%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_ts_enable_b_get
*
* DESCRIPTION
*
*     flexe_mux_ts_enable_b_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ts_sel: 0-ts0, 1-ts1, 2-ts2, ..., 79-ts79
**    *parameter: pointer to ts enable, 0-diable, 1-enable
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_ts_enable_b_get(UINT_8 chip_id,UINT_8 ts_sel,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ts_sel > (TS_NUM-1)))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ts_sel = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ts_sel
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ts_sel/32) > MAX_INDEX(ts_enable_b_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,ts_enable_b_reg[ts_sel/32].base_addr,ts_enable_b_reg[ts_sel/32].offset_addr,
        ts_sel%32,ts_sel%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_cfg_pls_set
*
* DESCRIPTION
*
*     Flexe mux cfg pls.
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_cfg_pls_set(UINT_8 chip_id)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }
   
    rv = regp_write(chip_id,cfg_pls_reg[CFG_PLS].base_addr,cfg_pls_reg[CFG_PLS].offset_addr,1);
    
    return rv;
}


/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_ram_conflict_alm_get
*
* DESCRIPTION
*
*     Flexe mux ram conflict alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id: RAM_CONFLICT_ALM_0~RAM_CONFLICT_ALM_3
**    *parameter: pointer to alm
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_ram_conflict_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (field_id > RAM_CONFLICT_ALM_3))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \
                   \r\n field_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   field_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(ram_conflict_alm_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_read(chip_id,ram_conflict_alm_reg[field_id].base_addr,ram_conflict_alm_reg[field_id].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_sys_signal_fifo_alm_get
*
* DESCRIPTION
*
*     Flexe mux sys signal fifo alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    *parameter: pointer to alm
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
*    dengj      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_sys_signal_fifo_alm_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
  
    rv = regp_read(chip_id,sys_signal_fifo_alm_reg[SYS_SGL_FIFO_EMPTY].base_addr,
        sys_signal_fifo_alm_reg[SYS_SGL_FIFO_EMPTY].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_wr_cfg_ram_set
*
* DESCRIPTION
*
*     Flexe mux wr_cfg ram cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~6399
**    parameter*: pointer to value
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
*    dengj      2017-12-21  1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_wr_cfg_ram_set(UINT_8 chip_id,UINT_16 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    
    if ((chip_id > MAX_DEV) ||
        (index > 6399))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d \r\n",    
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = ram_write_single(chip_id,wr_cfg,index,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_wr_cfg_ram_get
*
* DESCRIPTION
*
*     Flexe mux wr_cfg ram cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~6399
**    parameter*: pointer to value
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
*    dengj      2017-12-21  1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_wr_cfg_ram_get(UINT_8 chip_id,UINT_16 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    
    if ((chip_id > MAX_DEV) ||
        (index > 6399))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d \r\n",    
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = ram_read_single(chip_id,wr_cfg,index,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_rd_cfg_ram_set
*
* DESCRIPTION
*
*     Flexe mux rd cfg ram cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     line_sel: 0~9
*     index: 0~79
**    parameter*: pointer to value
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
*    dengj      2017-12-21  1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_rd_cfg_ram_set(UINT_8 chip_id,UINT_8 line_sel,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    
    if ((chip_id > MAX_DEV) ||
        (line_sel > 9) ||
        (index > 79))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n line_sel = %d, \
                    \r\n index = %d \r\n",    
                    __FUNCTION__,
                    chip_id,
                    line_sel,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = ram_write_single(chip_id,rd_cfg[line_sel],index,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_rd_cfg_ram_get
*
* DESCRIPTION
*
*     Flexe mux rd cfg ram cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     line_sel: 0~9
*     index: 0~79
**    parameter*: pointer to value
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
*    dengj      2017-12-21  1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_rd_cfg_ram_get(UINT_8 chip_id,UINT_8 line_sel,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    
    if ((chip_id > MAX_DEV) ||
        (line_sel > 9) ||
        (index > 79))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n line_sel = %d, \
                    \r\n index = %d \r\n",    
                    __FUNCTION__,
                    chip_id,
                    line_sel,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = ram_read_single(chip_id,rd_cfg[line_sel],index,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_wr_cfg_b_ram_set
*
* DESCRIPTION
*
*     flexe_mux_wr_cfg_b_ram_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~6399
**    parameter*: pointer to value
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_wr_cfg_b_ram_set(UINT_8 chip_id,UINT_16 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    
    if ((chip_id > MAX_DEV) ||
        (index > 6399))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d \r\n",    
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = ram_write_single(chip_id,wr_cfg_b,index,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_wr_cfg_b_ram_get
*
* DESCRIPTION
*
*     flexe_mux_wr_cfg_b_ram_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~6399
**    parameter*: pointer to value
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_wr_cfg_b_ram_get(UINT_8 chip_id,UINT_16 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    
    if ((chip_id > MAX_DEV) ||
        (index > 6399))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d \r\n",    
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = ram_read_single(chip_id,wr_cfg_b,index,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_rd_cfg_b_ram_set
*
* DESCRIPTION
*
*     flexe_mux_rd_cfg_b_ram_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     line_sel: 0~9
*     index: 0~79
**    parameter*: pointer to value
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_rd_cfg_b_ram_set(UINT_8 chip_id,UINT_8 line_sel,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    
    if ((chip_id > MAX_DEV) ||
        (line_sel > 9) ||
        (index > 79))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n line_sel = %d, \
                    \r\n index = %d \r\n",    
                    __FUNCTION__,
                    chip_id,
                    line_sel,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = ram_write_single(chip_id,rd_cfg_b[line_sel],index,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_rd_cfg_b_ram_get
*
* DESCRIPTION
*
*     flexe_mux_rd_cfg_b_ram_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     line_sel: 0~9
*     index: 0~79
**    parameter*: pointer to value
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
*    dingyi      2018-6-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_rd_cfg_b_ram_get(UINT_8 chip_id,UINT_8 line_sel,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    
    if ((chip_id > MAX_DEV) ||
        (line_sel > 9) ||
        (index > 79))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n line_sel = %d, \
                    \r\n index = %d \r\n",    
                    __FUNCTION__,
                    chip_id,
                    line_sel,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = ram_read_single(chip_id,rd_cfg_b[line_sel],index,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_cphy2_cfg_set
*
* DESCRIPTION
*
*     flexe_mux_cphy2_cfg_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     phy_id: 0~7
**    parameter: set value
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
*    dingyi     2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_cphy2_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > PHY_NUM -1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n phy_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   phy_id
                 );
       }

       return RET_PARAERR;
    }

    if (phy_id > MAX_INDEX(cphy2_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    regp_field_write(chip_id,cphy2_cfg_reg[phy_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_cphy2_cfg_get
*
* DESCRIPTION
*
*     flexe_mux_cphy2_cfg_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     phy_id: 0~7
**    parameter: get value
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
*    dingyi     2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_cphy2_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > PHY_NUM -1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n phy_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   phy_id
                 );
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (phy_id > MAX_INDEX(cphy2_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    regp_field_read(chip_id,cphy2_cfg_reg[phy_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_cnt8_sch_cfg_set
*
* DESCRIPTION
*
*     flexe_mux_cnt8_sch_cfg_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     phy_id: 0~7
**    parameter: set value
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
*    dingyi     2018-8-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_cnt8_sch_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > PHY_NUM -1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n phy_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   phy_id
                 );
       }

       return RET_PARAERR;
    }

    if (phy_id > MAX_INDEX(cnt8_sch_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    regp_field_write(chip_id,cnt8_sch_cfg_reg[phy_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_cnt8_sch_cfg_get
*
* DESCRIPTION
*
*     flexe_mux_cnt8_sch_cfg_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     phy_id: 0~7
**    parameter: get value
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
*    dingyi     2018-8-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_cnt8_sch_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > PHY_NUM -1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n phy_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   phy_id
                 );
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (phy_id > MAX_INDEX(cnt8_sch_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    regp_field_read(chip_id,cnt8_sch_cfg_reg[phy_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_m_cfg_set
*
* DESCRIPTION
*
*     flexe_mux_m_cfg_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: set value
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
*    dingyi     2018-10-9   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_m_cfg_set(UINT_8 chip_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! chip_id = %d \r\n",__FUNCTION__,chip_id);
       }

       return RET_PARAERR;
    }
   
    regp_field_write(chip_id,m_cfg_mux_reg[M_CFG_MUX],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_m_cfg_get
*
* DESCRIPTION
*
*     flexe_mux_m_cfg_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: get value
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
*    dingyi     2018-10-9   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_m_cfg_get(UINT_8 chip_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! chip_id = %d \r\n",__FUNCTION__,chip_id);
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    regp_field_read(chip_id,m_cfg_mux_reg[M_CFG_MUX],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_base_m_cfg_set
*
* DESCRIPTION
*
*     flexe_mux_base_m_cfg_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: set value
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
*    dingyi     2018-10-9   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_base_m_cfg_set(UINT_8 chip_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! chip_id = %d \r\n",__FUNCTION__,chip_id);
       }

       return RET_PARAERR;
    }
   
    regp_field_write(chip_id,base_m_cfg_mux_reg[BASE_M_CFG_MUX],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_base_m_cfg_get
*
* DESCRIPTION
*
*     flexe_mux_base_m_cfg_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: get value
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
*    dingyi     2018-10-9   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_base_m_cfg_get(UINT_8 chip_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! chip_id = %d \r\n",__FUNCTION__,chip_id);
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    regp_field_read(chip_id,base_m_cfg_mux_reg[BASE_M_CFG_MUX],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_en_sel_cfg_set
*
* DESCRIPTION
*
*     flexe_mux_en_sel_cfg_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     phy_id: 0~7
**    parameter: set value
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
*    dingyi     2018-11-10   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_en_sel_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > PHY_NUM -1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n phy_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   phy_id
                 );
       }

       return RET_PARAERR;
    }

    if (phy_id > MAX_INDEX(en_sel_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    regp_field_write(chip_id,en_sel_cfg_reg[phy_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_en_sel_cfg_get
*
* DESCRIPTION
*
*     flexe_mux_en_sel_cfg_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     phy_id: 0~7
**    parameter: get value
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
*    dingyi     2018-11-10   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_en_sel_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > PHY_NUM -1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n phy_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   phy_id
                 );
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (phy_id > MAX_INDEX(en_sel_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    regp_field_read(chip_id,en_sel_cfg_reg[phy_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_inst_sel_cfg_set
*
* DESCRIPTION
*
*     flexe_mux_inst_sel_cfg_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     phy_id: 0~7
**    parameter: set value
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
*    dingyi     2018-11-10   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_inst_sel_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > PHY_NUM -1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n phy_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   phy_id
                 );
       }

       return RET_PARAERR;
    }

    if (phy_id > MAX_INDEX(inst_sel_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    regp_field_write(chip_id,inst_sel_cfg_reg[phy_id],parameter);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_mux_inst_sel_cfg_get
*
* DESCRIPTION
*
*     flexe_mux_inst_sel_cfg_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     phy_id: 0~7
**    parameter: get value
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
*    dingyi     2018-11-10   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_mux_inst_sel_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (phy_id > PHY_NUM -1))
    {
       if (flexe_mux_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n phy_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   phy_id
                 );
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (phy_id > MAX_INDEX(inst_sel_cfg_reg))
    {
        if (flexe_mux_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    regp_field_read(chip_id,inst_sel_cfg_reg[phy_id],parameter);
    
    return RET_SUCCESS;
}
