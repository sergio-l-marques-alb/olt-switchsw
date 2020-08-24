

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int inf_ch_adp_tx_para_debug = 0;

const CHIP_REG port_fifo_cfg1_reg[]=
{
	{START_LEVEL,INF_CH_ADP_TX_BASE_ADDR,0x2,6,0},
	{HIGH_LEVEL,INF_CH_ADP_TX_BASE_ADDR,0x2,14,8},
	{LOW_LEVEL,INF_CH_ADP_TX_BASE_ADDR,0x2,22,16},
};
const CHIP_REG max_min_clr_reg[]=
{
	{INF_MAX_MIN_CLR,INF_CH_ADP_TX_BASE_ADDR,0xa,7,0},
};
const CHIP_REG port_protect_en_reg[]=
{
	{PROTECT_EN_0,INF_CH_ADP_TX_BASE_ADDR,0xb,31,0},
	{PROTECT_EN_1,INF_CH_ADP_TX_BASE_ADDR,0xc,31,0},
};
const CHIP_REG port_restart_reg[]=
{
	{PORT_RESTART,INF_CH_ADP_TX_BASE_ADDR,0xd,7,0},
};
const CHIP_REG tx_map_table_reg[]=
{
	{TX_MAP_TABLE,INF_CH_ADP_TX_BASE_ADDR,0xe,31,0},
};
const CHIP_REG map_en_reg[]=
{
	{MAP_EN,INF_CH_ADP_TX_BASE_ADDR,0xf,0,0},
};
const CHIP_REG external_cfg_reg[]=
{
	{RA_IS_400G,INF_CH_ADP_TX_BASE_ADDR,0x1c,0,0},
	{RA_400G_CH,INF_CH_ADP_TX_BASE_ADDR,0x1c,14,8},
};
const CHIP_REG tx_mode_cfg_reg[]=
{
	{TX_IS_400G,INF_CH_ADP_TX_BASE_ADDR,0x1d,0,0},
	{TX_IS_200G,INF_CH_ADP_TX_BASE_ADDR,0x1d,4,1},
	{TX_IS_BYPASS,INF_CH_ADP_TX_BASE_ADDR,0x1d,12,5},
	{TX_IS_PORT6,INF_CH_ADP_TX_BASE_ADDR,0x1d,13,13},
	{TX_IS_ASYM,INF_CH_ADP_TX_BASE_ADDR,0x1d,14,14},
	{TX_ASYM_INST_SEL,INF_CH_ADP_TX_BASE_ADDR,0x1d,15,15},
};
const CHIP_REG port_fifo_cfg2_reg[]=
{
	{PROTECT_UP_LEVEL,INF_CH_ADP_TX_BASE_ADDR,0x1e,6,0},
	{PROTECT_DOWN_LEVEL,INF_CH_ADP_TX_BASE_ADDR,0x1e,14,8},
};
const CHIP_REG port_fifo_state_reg[]=
{
	{TX_MAX_LEVEL,INF_CH_ADP_TX_BASE_ADDR,0x11,6,0},
	{TX_MIN_LEVEL,INF_CH_ADP_TX_BASE_ADDR,0x11,14,8},
};
const CHIP_REG tx_gearbox_alm_reg[]=
{
	{FULL_GB_400G,INF_CH_ADP_TX_BASE_ADDR,0x19,0,0},
	{EMPTY_GB_400G,INF_CH_ADP_TX_BASE_ADDR,0x19,1,1},
	{FULL_GB_200G,INF_CH_ADP_TX_BASE_ADDR,0x19,5,2},
	{EMPTY_GB_200G,INF_CH_ADP_TX_BASE_ADDR,0x19,9,6},
	{FULL_GB_RA,INF_CH_ADP_TX_BASE_ADDR,0x19,17,10},
	{EMPTY_GB_RA,INF_CH_ADP_TX_BASE_ADDR,0x19,25,18},
};
const CHIP_REG port_fifo_alm_reg[]=
{
	{HIGH_ALM,INF_CH_ADP_TX_BASE_ADDR,0x1a,7,0},
	{LOW_ALM,INF_CH_ADP_TX_BASE_ADDR,0x1a,15,8},
	{PROTECT_UP_ALM,INF_CH_ADP_TX_BASE_ADDR,0x1a,23,16},
	{PROTECT_DOWN_ALM,INF_CH_ADP_TX_BASE_ADDR,0x1a,31,24},
	{PORT_FIFO_FULL,INF_CH_ADP_TX_BASE_ADDR,0x1b,7,0},
	{PORT_FIFO_EMPTY,INF_CH_ADP_TX_BASE_ADDR,0x1b,15,8},
};
const CHIP_REG port_buffer0_INT_REG[]=
{
	{PORT_FIFO_FULL0_INT,INF_CH_ADP_TX_BASE_ADDR,0x26,1,0},
	{PORT_FIFO_EMPTY0_INT,INF_CH_ADP_TX_BASE_ADDR,0x26,3,2},
	{PROTECT_UP_ALM0_INT,INF_CH_ADP_TX_BASE_ADDR,0x26,5,4},
	{PROTECT_DOWN_ALM0_INT,INF_CH_ADP_TX_BASE_ADDR,0x26,7,6},
};
const CHIP_REG port_buffer0_INT_MASK_REG[]=
{
	{PORT_FIFO_FULL0_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x27,1,0},
	{PORT_FIFO_EMPTY0_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x27,3,2},
	{PROTECT_UP_ALM0_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x27,5,4},
	{PROTECT_DOWN_ALM0_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x27,7,6},
};
const CHIP_REG port_buffer1_INT_REG[]=
{
	{PORT_FIFO_FULL1_INT,INF_CH_ADP_TX_BASE_ADDR,0x28,1,0},
	{PORT_FIFO_EMPTY1_INT,INF_CH_ADP_TX_BASE_ADDR,0x28,3,2},
	{PROTECT_UP_ALM1_INT,INF_CH_ADP_TX_BASE_ADDR,0x28,5,4},
	{PROTECT_DOWN_ALM1_INT,INF_CH_ADP_TX_BASE_ADDR,0x28,7,6},
};
const CHIP_REG port_buffer1_INT_MASK_REG[]=
{
	{PORT_FIFO_FULL1_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x29,1,0},
	{PORT_FIFO_EMPTY1_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x29,3,2},
	{PROTECT_UP_ALM1_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x29,5,4},
	{PROTECT_DOWN_ALM1_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x29,7,6},
};
const CHIP_REG port_buffer2_INT_REG[]=
{
	{PORT_FIFO_FULL2_INT,INF_CH_ADP_TX_BASE_ADDR,0x2a,1,0},
	{PORT_FIFO_EMPTY2_INT,INF_CH_ADP_TX_BASE_ADDR,0x2a,3,2},
	{PROTECT_UP_ALM2_INT,INF_CH_ADP_TX_BASE_ADDR,0x2a,5,4},
	{PROTECT_DOWN_ALM2_INT,INF_CH_ADP_TX_BASE_ADDR,0x2a,7,6},
};
const CHIP_REG port_buffer2_INT_MASK_REG[]=
{
	{PORT_FIFO_FULL2_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2b,1,0},
	{PORT_FIFO_EMPTY2_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2b,3,2},
	{PROTECT_UP_ALM2_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2b,5,4},
	{PROTECT_DOWN_ALM2_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2b,7,6},
};
const CHIP_REG port_buffer3_INT_REG[]=
{
	{PORT_FIFO_FULL3_INT,INF_CH_ADP_TX_BASE_ADDR,0x2c,1,0},
	{PORT_FIFO_EMPTY3_INT,INF_CH_ADP_TX_BASE_ADDR,0x2c,3,2},
	{PROTECT_UP_ALM3_INT,INF_CH_ADP_TX_BASE_ADDR,0x2c,5,4},
	{PROTECT_DOWN_ALM3_INT,INF_CH_ADP_TX_BASE_ADDR,0x2c,7,6},
};
const CHIP_REG port_buffer3_INT_MASK_REG[]=
{
	{PORT_FIFO_FULL3_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2d,1,0},
	{PORT_FIFO_EMPTY3_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2d,3,2},
	{PROTECT_UP_ALM3_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2d,5,4},
	{PROTECT_DOWN_ALM3_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2d,7,6},
};
const CHIP_REG port_buffer4_INT_REG[]=
{
	{PORT_FIFO_FULL4_INT,INF_CH_ADP_TX_BASE_ADDR,0x2e,1,0},
	{PORT_FIFO_EMPTY4_INT,INF_CH_ADP_TX_BASE_ADDR,0x2e,3,2},
	{PROTECT_UP_ALM4_INT,INF_CH_ADP_TX_BASE_ADDR,0x2e,5,4},
	{PROTECT_DOWN_ALM4_INT,INF_CH_ADP_TX_BASE_ADDR,0x2e,7,6},
};
const CHIP_REG port_buffer4_INT_MASK_REG[]=
{
	{PORT_FIFO_FULL4_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2f,1,0},
	{PORT_FIFO_EMPTY4_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2f,3,2},
	{PROTECT_UP_ALM4_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2f,5,4},
	{PROTECT_DOWN_ALM4_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x2f,7,6},
};
const CHIP_REG port_buffer5_INT_REG[]=
{
	{PORT_FIFO_FULL5_INT,INF_CH_ADP_TX_BASE_ADDR,0x30,1,0},
	{PORT_FIFO_EMPTY5_INT,INF_CH_ADP_TX_BASE_ADDR,0x30,3,2},
	{PROTECT_UP_ALM5_INT,INF_CH_ADP_TX_BASE_ADDR,0x30,5,4},
	{PROTECT_DOWN_ALM5_INT,INF_CH_ADP_TX_BASE_ADDR,0x30,7,6},
};
const CHIP_REG port_buffer5_INT_MASK_REG[]=
{
	{PORT_FIFO_FULL5_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x31,1,0},
	{PORT_FIFO_EMPTY5_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x31,3,2},
	{PROTECT_UP_ALM5_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x31,5,4},
	{PROTECT_DOWN_ALM5_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x31,7,6},
};
const CHIP_REG port_buffer6_INT_REG[]=
{
	{PORT_FIFO_FULL6_INT,INF_CH_ADP_TX_BASE_ADDR,0x32,1,0},
	{PORT_FIFO_EMPTY6_INT,INF_CH_ADP_TX_BASE_ADDR,0x32,3,2},
	{PROTECT_UP_ALM6_INT,INF_CH_ADP_TX_BASE_ADDR,0x32,5,4},
	{PROTECT_DOWN_ALM6_INT,INF_CH_ADP_TX_BASE_ADDR,0x32,7,6},
};
const CHIP_REG port_buffer6_INT_MASK_REG[]=
{
	{PORT_FIFO_FULL6_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x33,1,0},
	{PORT_FIFO_EMPTY6_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x33,3,2},
	{PROTECT_UP_ALM6_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x33,5,4},
	{PROTECT_DOWN_ALM6_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x33,7,6},
};
const CHIP_REG port_buffer7_INT_REG[]=
{
	{PORT_FIFO_FULL7_INT,INF_CH_ADP_TX_BASE_ADDR,0x34,1,0},
	{PORT_FIFO_EMPTY7_INT,INF_CH_ADP_TX_BASE_ADDR,0x34,3,2},
	{PROTECT_UP_ALM7_INT,INF_CH_ADP_TX_BASE_ADDR,0x34,5,4},
	{PROTECT_DOWN_ALM7_INT,INF_CH_ADP_TX_BASE_ADDR,0x34,7,6},
};
const CHIP_REG port_buffer7_INT_MASK_REG[]=
{
	{PORT_FIFO_FULL7_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x35,1,0},
	{PORT_FIFO_EMPTY7_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x35,3,2},
	{PROTECT_UP_ALM7_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x35,5,4},
	{PROTECT_DOWN_ALM7_INT_MASK,INF_CH_ADP_TX_BASE_ADDR,0x35,7,6},
};
const CHIP_RAM map_ram = 
{
	TABLE_BASE_ADDR,INF_CH_ADP_TX_BASE_ADDR+0x10,4,
};

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_fifo_cfg2_set
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
*	  field_id: 0:PROTECT_UP_LEVEL  1:PROTECT_DOWN_LEVEL
*	  grp_id: 0~7
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
*    dingyi    2018-6-14    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_fifo_cfg2_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7) || (parameter > 0x7f))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n grp_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  grp_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(port_fifo_cfg2_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id, port_fifo_cfg2_reg[field_id].base_addr, port_fifo_cfg2_reg[field_id].offset_addr+grp_id, 
		port_fifo_cfg2_reg[field_id].end_bit, port_fifo_cfg2_reg[field_id].start_bit, parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_fifo_cfg2_get
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
*	  field_id: 0:PROTECT_UP_LEVEL  1:PROTECT_DOWN_LEVEL
*	  grp_id: 0~7
*     parameter: value to get 
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
*    dingyi    2018-6-14    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_fifo_cfg2_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n grp_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  grp_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(port_fifo_cfg2_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id, port_fifo_cfg2_reg[field_id].base_addr, port_fifo_cfg2_reg[field_id].offset_addr+grp_id, 
		port_fifo_cfg2_reg[field_id].end_bit, port_fifo_cfg2_reg[field_id].start_bit, parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_fifo_cfg_set
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
*	  field_id: 0~4  0:START_LEVEL  1:HIGH_LEVEL  2:LOW_LEVEL
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_fifo_cfg1_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7) || (parameter > 0x7f))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n grp_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  grp_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(port_fifo_cfg1_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,port_fifo_cfg1_reg[field_id].base_addr,port_fifo_cfg1_reg[field_id].offset_addr+grp_id,
		port_fifo_cfg1_reg[field_id].end_bit,port_fifo_cfg1_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_fifo_cfg_get
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
*	  field_id: 0~4  0:START_LEVEL  1:HIGH_LEVEL  2:LOW_LEVEL
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_fifo_cfg1_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n grp_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  grp_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }
	
	if (field_id > MAX_INDEX(port_fifo_cfg1_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,port_fifo_cfg1_reg[field_id].base_addr,port_fifo_cfg1_reg[field_id].offset_addr+grp_id,
		port_fifo_cfg1_reg[field_id].end_bit,port_fifo_cfg1_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_max_min_clr_set
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
*	  port_id: 0-7:port 0-7
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_max_min_clr_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7) || (parameter > 0x1))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,max_min_clr_reg[INF_MAX_MIN_CLR].base_addr,max_min_clr_reg[INF_MAX_MIN_CLR].offset_addr,
		port_id,port_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_max_min_clr_get
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
*	  port_id: 0-7:port 0-7
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_max_min_clr_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,max_min_clr_reg[INF_MAX_MIN_CLR].base_addr,max_min_clr_reg[INF_MAX_MIN_CLR].offset_addr,
		port_id,port_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_protect_en_set
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
*	  port_id: 0-7:port 0-7
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_protect_en_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7) || (parameter > 0xff))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (port_id/4 > MAX_INDEX(port_protect_en_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,port_protect_en_reg[port_id/4].base_addr,port_protect_en_reg[port_id/4].offset_addr,
		(port_id%4)*8+7,(port_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_protect_en_get
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
*	  port_id: 0-7:port 0-7
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_protect_en_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (port_id/4 > MAX_INDEX(port_protect_en_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,port_protect_en_reg[port_id/4].base_addr,port_protect_en_reg[port_id/4].offset_addr,
		(port_id%4)*8+7,(port_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_restart_set
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
*	  port_id: 0-7:port 0-7
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_restart_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7) || (parameter > 0x1))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,port_restart_reg[PORT_RESTART].base_addr,port_restart_reg[PORT_RESTART].offset_addr,
		port_id,port_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_restart_get
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
*	  port_id: 0-7:port 0-7
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_restart_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,port_restart_reg[PORT_RESTART].base_addr,port_restart_reg[PORT_RESTART].offset_addr,
		port_id,port_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_map_table_set
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
*	  ch_id: 0-7 channel select
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_map_table_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (ch_id > 7) || (parameter > 0xf))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n ch_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  ch_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,tx_map_table_reg[TX_MAP_TABLE].base_addr,tx_map_table_reg[TX_MAP_TABLE].offset_addr,
		ch_id*4+3,ch_id*4,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_map_table_get
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
*	  ch_id: 0-7 channel select
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_map_table_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (ch_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n ch_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  ch_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,tx_map_table_reg[TX_MAP_TABLE].base_addr,tx_map_table_reg[TX_MAP_TABLE].offset_addr,
		ch_id*4+3,ch_id*4,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_map_en_set
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_map_en_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0x1))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,map_en_reg[MAP_EN],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_map_en_get
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
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_map_en_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_tx_para_debug)
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
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,map_en_reg[MAP_EN],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_external_cfg_set
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
*	  field_id: 0:RA_IS_400G  1:RA_400G_CH
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_external_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(external_cfg_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,external_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_external_cfg_get
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
*	  field_id: 0:RA_IS_400G  1:RA_400G_CH
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_external_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_tx_para_debug)
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
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(external_cfg_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,external_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_fifo_state_get
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
*	  field_id: 0~1  0:TX_MAX_LEVEL  1:TX_MIN_LEVEL
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_fifo_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n grp_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  grp_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }
	
	if (field_id > MAX_INDEX(port_fifo_state_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,port_fifo_state_reg[field_id].base_addr,port_fifo_state_reg[field_id].offset_addr+grp_id,
		port_fifo_state_reg[field_id].end_bit,port_fifo_state_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_gearbox_alm_get
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
*	  field_id: 0-5 0:FULL_GB_400G  1:EMPTY_GB_400G  2:FULL_GB_200G
*					3:EMPTY_GB_200G 4:FULL_GB_RA     5:EMPTY_GB_RA
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_gearbox_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_tx_para_debug)
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
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(tx_gearbox_alm_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,tx_gearbox_alm_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_fifo_alm_get
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
*	  field_id: 0-5 0:HIGH_ALM  1:LOW_ALM  2:PROTECT_UP_ALM
*					3:PROTECT_DOWN_ALM 4:PORT_FIFO_FULL  5:PORT_FIFO_EMPTY
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_fifo_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_tx_para_debug)
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
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(port_fifo_alm_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,port_fifo_alm_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_map_ram_set
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
*	  ch_id: 0-79 channel select
*     *parameter: value to set 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_map_ram_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (ch_id > (CH_NUM -1)))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n ch_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  ch_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	ram_write_single(chip_id,map_ram,ch_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_map_ram_get
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
*	  ch_id: 0-79 channel select
*     *parameter: value to get 
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_map_ram_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (ch_id > (CH_NUM -1)))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n ch_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  ch_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	ram_read_single(chip_id,map_ram,ch_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_mode_cfg_set
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
*	  field_id: 0:TX_IS_400G  1:TX_IS_200G  2:TX_IS_BYPASS 3:TX_IS_PORT6
*				4:TX_IS_ASYM  5:TX_ASYM_INST_SEL
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
*    dingyi    2018-6-14    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_mode_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  field_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(tx_mode_cfg_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,tx_mode_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_mode_cfg_get
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
*	  field_id: 0:TX_IS_400G  1:TX_IS_200G  2:TX_IS_BYPASS 3:TX_IS_PORT6
*				4:TX_IS_ASYM  5:TX_ASYM_INST_SEL
*     parameter: value to get 
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
*    dingyi    2018-6-14    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_mode_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(tx_mode_cfg_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,tx_mode_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_mode_cfg_400g_set
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
*    dingyi    2018-6-15    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_mode_cfg_400g_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,tx_mode_cfg_reg[TX_IS_400G],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_mode_cfg_400g_get
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
*     parameter: value to get 
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
*    dingyi    2018-6-15    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_mode_cfg_400g_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_tx_para_debug)
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
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,tx_mode_cfg_reg[TX_IS_400G],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_mode_cfg_200g_set
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
*	  port_id: 0:port0 1:port2 2:port4 3:port6
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
*    dingyi    2018-6-15    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_mode_cfg_200g_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 3) || (parameter > 1))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (TX_IS_200G > MAX_INDEX(tx_mode_cfg_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,tx_mode_cfg_reg[TX_IS_200G].base_addr,tx_mode_cfg_reg[TX_IS_200G].offset_addr,port_id+1,port_id+1,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_mode_cfg_200g_get
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
*	  port_id: 0:port0 1:port2 2:port4 3:port6
*     parameter: value to get 
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
*    dingyi    2018-6-15    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_mode_cfg_200g_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 3))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (TX_IS_200G > MAX_INDEX(tx_mode_cfg_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,tx_mode_cfg_reg[TX_IS_200G].base_addr,tx_mode_cfg_reg[TX_IS_200G].offset_addr,port_id+1,port_id+1,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_mode_cfg_bypass_set
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
*	  port_id: 0-7:port 0-7
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
*    dingyi    2018-6-15    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_mode_cfg_bypass_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7) || (parameter > 1))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (TX_IS_BYPASS > MAX_INDEX(tx_mode_cfg_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,tx_mode_cfg_reg[TX_IS_BYPASS].base_addr,tx_mode_cfg_reg[TX_IS_BYPASS].offset_addr,port_id+5,port_id+5,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_mode_cfg_bypass_get
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
*	  port_id: 0-7:port 0-7
*     parameter: value to get 
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
*    dingyi    2018-6-15    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_mode_cfg_bypass_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (TX_IS_BYPASS > MAX_INDEX(tx_mode_cfg_reg))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,tx_mode_cfg_reg[TX_IS_BYPASS].base_addr,tx_mode_cfg_reg[TX_IS_BYPASS].offset_addr,port_id+5,port_id+5,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_buffer_int_set
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
*	  port_id: 0-7:port 0-7
*	  field_id: 0:PORT_FIFO_FULL_INT  1:PORT_FIFO_EMPTY_INT  2:PROTECT_UP_ALM_INT  3:PROTECT_DOWN_ALM_INT
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
*    dingyi    2018-11-5    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_buffer_int_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7) || (parameter > 0x3))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(port_buffer0_INT_REG))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	switch (port_id)
	{
		case 0:
			regp_field_write(chip_id,port_buffer0_INT_REG[field_id],parameter);
			break;
		case 1:
			regp_field_write(chip_id,port_buffer1_INT_REG[field_id],parameter);
			break;
		case 2:
			regp_field_write(chip_id,port_buffer2_INT_REG[field_id],parameter);
			break;
		case 3:
			regp_field_write(chip_id,port_buffer3_INT_REG[field_id],parameter);
			break;
		case 4:
			regp_field_write(chip_id,port_buffer4_INT_REG[field_id],parameter);
			break;
		case 5:
			regp_field_write(chip_id,port_buffer5_INT_REG[field_id],parameter);
			break;
		case 6:
			regp_field_write(chip_id,port_buffer6_INT_REG[field_id],parameter);
			break;
		case 7:
			regp_field_write(chip_id,port_buffer7_INT_REG[field_id],parameter);
			break;
		default:
			break;
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_buffer_int_get
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
*	  port_id: 0-7:port 0-7
*	  field_id: 0:PORT_FIFO_FULL_INT  1:PORT_FIFO_EMPTY_INT  2:PROTECT_UP_ALM_INT  3:PROTECT_DOWN_ALM_INT
*     parameter: value to get 
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
*    dingyi    2018-11-5    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_buffer_int_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(port_buffer0_INT_REG))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	switch (port_id)
	{
		case 0:
			regp_field_read(chip_id,port_buffer0_INT_REG[field_id],parameter);
			break;
		case 1:
			regp_field_read(chip_id,port_buffer1_INT_REG[field_id],parameter);
			break;
		case 2:
			regp_field_read(chip_id,port_buffer2_INT_REG[field_id],parameter);
			break;
		case 3:
			regp_field_read(chip_id,port_buffer3_INT_REG[field_id],parameter);
			break;
		case 4:
			regp_field_read(chip_id,port_buffer4_INT_REG[field_id],parameter);
			break;
		case 5:
			regp_field_read(chip_id,port_buffer5_INT_REG[field_id],parameter);
			break;
		case 6:
			regp_field_read(chip_id,port_buffer6_INT_REG[field_id],parameter);
			break;
		case 7:
			regp_field_read(chip_id,port_buffer7_INT_REG[field_id],parameter);
			break;
		default:
			break;
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_buffer_int_mask_set
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
*	  port_id: 0-7:port 0-7
*	  field_id: 0:PORT_FIFO_FULL_INT_MASK  1:PORT_FIFO_EMPTY_INT_MASK  2:PROTECT_UP_ALM_INT_MASK  
*				3:PROTECT_DOWN_ALM_INT_MASK
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
*    dingyi    2018-11-5    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_buffer_int_mask_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7) || (parameter > 0x3))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d, \
													  \r\n parameter = 0x%x \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(port_buffer0_INT_MASK_REG))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	switch (port_id)
	{
		case 0:
			regp_field_write(chip_id,port_buffer0_INT_MASK_REG[field_id],parameter);
			break;
		case 1:
			regp_field_write(chip_id,port_buffer1_INT_MASK_REG[field_id],parameter);
			break;
		case 2:
			regp_field_write(chip_id,port_buffer2_INT_MASK_REG[field_id],parameter);
			break;
		case 3:
			regp_field_write(chip_id,port_buffer3_INT_MASK_REG[field_id],parameter);
			break;
		case 4:
			regp_field_write(chip_id,port_buffer4_INT_MASK_REG[field_id],parameter);
			break;
		case 5:
			regp_field_write(chip_id,port_buffer5_INT_MASK_REG[field_id],parameter);
			break;
		case 6:
			regp_field_write(chip_id,port_buffer6_INT_MASK_REG[field_id],parameter);
			break;
		case 7:
			regp_field_write(chip_id,port_buffer7_INT_MASK_REG[field_id],parameter);
			break;
		default:
			break;
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_tx_port_buffer_int_get
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
*	  port_id: 0-7:port 0-7
*	  field_id: 0:PORT_FIFO_FULL_INT_MASK  1:PORT_FIFO_EMPTY_INT_MASK  2:PROTECT_UP_ALM_INT_MASK  
*				3:PROTECT_DOWN_ALM_INT_MASK
*     parameter: value to get 
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
*    dingyi    2018-11-5    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_port_buffer_int_mask_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7))
	{
	   if (inf_ch_adp_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(port_buffer0_INT_MASK_REG))
	{
		if (inf_ch_adp_tx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	switch (port_id)
	{
		case 0:
			regp_field_read(chip_id,port_buffer0_INT_MASK_REG[field_id],parameter);
			break;
		case 1:
			regp_field_read(chip_id,port_buffer1_INT_MASK_REG[field_id],parameter);
			break;
		case 2:
			regp_field_read(chip_id,port_buffer2_INT_MASK_REG[field_id],parameter);
			break;
		case 3:
			regp_field_read(chip_id,port_buffer3_INT_MASK_REG[field_id],parameter);
			break;
		case 4:
			regp_field_read(chip_id,port_buffer4_INT_MASK_REG[field_id],parameter);
			break;
		case 5:
			regp_field_read(chip_id,port_buffer5_INT_MASK_REG[field_id],parameter);
			break;
		case 6:
			regp_field_read(chip_id,port_buffer6_INT_MASK_REG[field_id],parameter);
			break;
		case 7:
			regp_field_read(chip_id,port_buffer7_INT_MASK_REG[field_id],parameter);
			break;
		default:
			break;
	}
	
	return RET_SUCCESS;
}

