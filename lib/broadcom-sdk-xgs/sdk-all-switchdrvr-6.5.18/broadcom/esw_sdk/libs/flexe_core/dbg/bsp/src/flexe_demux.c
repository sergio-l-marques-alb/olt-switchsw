

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int flexe_demux_para_debug = 0;

const CHIP_REG ready_ts_reg[]=
{
	{READY_TS_0,FLEXE_DEMUX_BASE_ADDR,0x0,31,0},
	{READY_TS_1,FLEXE_DEMUX_BASE_ADDR,0x1,31,0},
	{READY_TS_2,FLEXE_DEMUX_BASE_ADDR,0x2,15,0},
};
const CHIP_REG flexegroup_info_reg[]=
{
	{FLEXEGROUP_INFO,FLEXE_DEMUX_BASE_ADDR,0x3,31,0},
};
const CHIP_REG group_cfg_reg[]=
{
	{GROUP_CFG_0,FLEXE_DEMUX_BASE_ADDR,0x4,31,0},
	{GROUP_CFG_1,FLEXE_DEMUX_BASE_ADDR,0x5,31,0},
};
const CHIP_REG reorder_asic_reg[]=
{
	{REORDER_ASIC_0,FLEXE_DEMUX_BASE_ADDR,0x6,31,0},
	{REORDER_ASIC_1,FLEXE_DEMUX_BASE_ADDR,0x7,31,0},
};
const CHIP_REG autoset_en_reg[]=
{
	{AUTOSET_EN,FLEXE_DEMUX_BASE_ADDR,0xe,31,0},
};
const CHIP_REG mode_info_reg[]=
{
	{MODE_INFO,FLEXE_DEMUX_BASE_ADDR,0xf,7,0},
};
const CHIP_REG reorder_info_reg[]=
{
	{REORDER_INFO,FLEXE_DEMUX_BASE_ADDR,0x10,31,0},
};
const CHIP_REG calendar_cfg_reg[]=
{
	{CALENDAR_CFG_0,FLEXE_DEMUX_BASE_ADDR,0x11,31,0},
};
const CHIP_REG channel_own_tsgroup_reg[]=
{
	{CHANNEL_OWN_TSGROUP,FLEXE_DEMUX_BASE_ADDR,0x40,31,0},
};
const CHIP_REG maxlevel_clr_reg[]=
{
	{MAXLEVEL_CLR,FLEXE_DEMUX_BASE_ADDR,0x55,0,0},
};
const CHIP_REG ready_ts_b_reg[]=
{
	{READY_TS_B_0,FLEXE_DEMUX_BASE_ADDR,0x80,31,0},
	{READY_TS_B_1,FLEXE_DEMUX_BASE_ADDR,0x81,31,0},
	{READY_TS_B_2,FLEXE_DEMUX_BASE_ADDR,0x82,15,0},
};
const CHIP_REG calendar_cfg_b_reg[]=
{
	{CALENDAR_CFG_B_0,FLEXE_DEMUX_BASE_ADDR,0x91,31,0},
};
const CHIP_REG channel_own_tsgroup_b_reg[]=
{
	{CHANNEL_OWN_TSGROUP_B_0,FLEXE_DEMUX_BASE_ADDR,0xc0,31,0},
};
const CHIP_REG enable_ab_reg[]=
{
	{ENABLE_AB,FLEXE_DEMUX_BASE_ADDR,0xdd,0,0},
};
const CHIP_REG verify_cfg_reg[]=
{
	{VERIFY_CFG_0,FLEXE_DEMUX_BASE_ADDR,0xde,3,0},
	{VERIFY_CFG_1,FLEXE_DEMUX_BASE_ADDR,0xde,15,4},
};
const CHIP_REG localfault_en_reg[]=
{
	{LOCALFAULT_EN,FLEXE_DEMUX_BASE_ADDR,0xdf,0,0},
};
const CHIP_REG threshold_reg[]=
{	
	{THRESHOLD,FLEXE_DEMUX_BASE_ADDR,0xe0,31,0},
};
const CHIP_REG threshold_full_reg[]=
{
	{THRESHOLD_FULL_0,FLEXE_DEMUX_BASE_ADDR,0xe1,31,0},
	{THRESHOLD_FULL_1,FLEXE_DEMUX_BASE_ADDR,0xe2,31,0},
};
const CHIP_REG threshold_empty_reg[]=
{
	{THRESHOLD_EMPTY_0,FLEXE_DEMUX_BASE_ADDR,0xe3,31,0},
	{THRESHOLD_EMPTY_1,FLEXE_DEMUX_BASE_ADDR,0xe4,31,0},
};
const CHIP_REG maxlevel_interleave_reg[]=
{
	{MAXLEVEL_INTERLEAVE_0,FLEXE_DEMUX_BASE_ADDR,0x56,31,0},
	{MAXLEVEL_INTERLEAVE_1,FLEXE_DEMUX_BASE_ADDR,0x57,31,0},
};
const CHIP_REG interleave_rst_reg[]=
{
	{INTERLEAVE_RST,FLEXE_DEMUX_BASE_ADDR,0x54,7,0}
};
const CHIP_REG flexedemux_ram_full_reg[]=
{
	{FLEXEDEMUX_RAM_FULL_0,FLEXE_DEMUX_BASE_ADDR,0x25,31,0},
	{FLEXEDEMUX_RAM_FULL_1,FLEXE_DEMUX_BASE_ADDR,0x26,31,0},
	{FLEXEDEMUX_RAM_FULL_2,FLEXE_DEMUX_BASE_ADDR,0x27,31,0},
	{FLEXEDEMUX_RAM_FULL_3,FLEXE_DEMUX_BASE_ADDR,0x28,3,0},
};
const CHIP_REG flexedemux_fifo_full_reg[]=
{
	{FLEXEDEMUX_FIFO_FULL,FLEXE_DEMUX_BASE_ADDR,0x29,7,0},
};
const CHIP_REG flexedemux_fifo_empty_reg[]=
{
	{FLEXEDEMUX_FIFO_EMPTY,FLEXE_DEMUX_BASE_ADDR,0x39,7,0},
};
const CHIP_REG fp_check_alm_reg[]=
{
	{FP_CHECK_ALM,FLEXE_DEMUX_BASE_ADDR,0xe5,7,0},
};
const CHIP_REG head_check_phy_alm_reg[]=
{
	{HEAD_CHECK_PHY_ALM,FLEXE_DEMUX_BASE_ADDR,0xe6,7,0},
};
const CHIP_REG head_check_channel_alm_reg[]=
{
	{HEAD_CHECK_CHANNEL_ALM_0,FLEXE_DEMUX_BASE_ADDR,0xe7,31,0},
	{HEAD_CHECK_CHANNEL_ALM_1,FLEXE_DEMUX_BASE_ADDR,0xe8,31,0},
	{HEAD_CHECK_CHANNEL_ALM_2,FLEXE_DEMUX_BASE_ADDR,0xe9,15,0},
};
const CHIP_RAM wcfgram[] = 
{
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x2a,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x2b,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x2c,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x2d,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x2e,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x2f,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x30,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x31,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x32,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x33,4},	
};
const CHIP_RAM rdrule = 
{
	TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x36,80,
};
const CHIP_RAM property = 
{
	TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0x37,20,
};
const CHIP_RAM wcfgram_b[] = 
{
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xaa,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xab,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xac,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xad,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xae,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xaf,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xb0,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xb1,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xb2,4},
	{TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xb3,4},
};
const CHIP_RAM rdrule_b = 
{
	TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xb6,80,
};
const CHIP_RAM chproperty_b = 
{
	TABLE_BASE_ADDR,FLEXE_DEMUX_BASE_ADDR+0xb7,20,
};


/******************************************************************************
*
* FUNCTION flexe_demux_ts_en_set
*
*     
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
*     ts_id:0~79
*	  parameter: 0:enable 1:disable
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_ts_en_set(UINT_8 chip_id,UINT_8 ts_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ts_id > (TS_NUM-1) || parameter > 1)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ts_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,ts_id,parameter);
        }
		
		return RET_PARAERR;
	}

	if (ts_id/32 > MAX_INDEX(ready_ts_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,ready_ts_reg[ts_id/32].base_addr,ready_ts_reg[ts_id/32].offset_addr,ts_id%32,ts_id%32,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_ts_en_get
*
*     
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
*     ts_id:0~79
*	  *parameter:pointer to ts enable status 
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_ts_en_get(UINT_8 chip_id,UINT_8 ts_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ts_id > (TS_NUM - 1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ts_id = %d\r\n",__FUNCTION__,chip_id,ts_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (ts_id/32 > MAX_INDEX(ready_ts_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,ready_ts_reg[ts_id/32].base_addr,ready_ts_reg[ts_id/32].offset_addr,ts_id%32,ts_id%32,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_group_info_set
*
*     
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
*     phy_id:0~7
*	  parameter: 0~7 phy belong to which group
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_group_info_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1) || parameter > 0xf)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,phy_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,flexegroup_info_reg[0].base_addr,flexegroup_info_reg[0].offset_addr,
	phy_id*4+3,phy_id*4,parameter);
	
	return RET_SUCCESS;
}
RET_STATUS flexe_demux_group_info_sync_set(UINT_8 chip_id, UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,flexegroup_info_reg[0].base_addr,flexegroup_info_reg[0].offset_addr,31,0,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_demux_group_info_get
*
*     
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
*     phy_id:0~7
*	  *parameter: pointer to group info
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_group_info_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,flexegroup_info_reg[0].base_addr,flexegroup_info_reg[0].offset_addr,
	phy_id*4+3,phy_id*4,parameter);
	
	return RET_SUCCESS;
}
RET_STATUS flexe_demux_group_info_sync_get(UINT_8 chip_id, UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,flexegroup_info_reg[0].base_addr,flexegroup_info_reg[0].offset_addr,31,0,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*	flexe_demux_group_cfg_set
*     
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
*     phy_id:0~7
*	  parameter: set value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
RET_STATUS flexe_demux_group_cfg_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1) || parameter > 0xf)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,phy_id,parameter);
        }
		
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,group_cfg_reg[GROUP_CFG_0].base_addr,group_cfg_reg[GROUP_CFG_0].offset_addr,
		phy_id*4+3,phy_id*4,parameter);
	
	return RET_SUCCESS;
}

RET_STATUS flexe_demux_group_cfg_sync_set(UINT_8 chip_id, UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, parameter = %d\r\n",__FUNCTION__,chip_id, parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,group_cfg_reg[GROUP_CFG_0].base_addr,group_cfg_reg[GROUP_CFG_0].offset_addr,31,0,parameter);
	
	return RET_SUCCESS;
}
/******************************************************************************
*
* FUNCTION 
*
*	flexe_demux_group_cfg_get
*     
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
*     phy_id:0~7
*	  parameter: get value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
RET_STATUS flexe_demux_group_cfg_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,group_cfg_reg[GROUP_CFG_0].base_addr,group_cfg_reg[GROUP_CFG_0].offset_addr,
		phy_id*4+3,phy_id*4,parameter);
	
	return RET_SUCCESS;
}

RET_STATUS flexe_demux_group_cfg_sync_get(UINT_8 chip_id, UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,group_cfg_reg[GROUP_CFG_0].base_addr,group_cfg_reg[GROUP_CFG_0].offset_addr,31,0,parameter);
	
	return RET_SUCCESS;
}
/******************************************************************************
*
* FUNCTION 
*
*	flexe_demux_reorder_asic_set
*     
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
*     phy_id:0~7
*	  parameter: set value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-7-3   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_reorder_asic_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1) || parameter > 0xff)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,phy_id,parameter);
        }
		
		return RET_PARAERR;
	}

	if (phy_id/4 > MAX_INDEX(reorder_asic_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,reorder_asic_reg[phy_id/4].base_addr,reorder_asic_reg[phy_id/4].offset_addr,
		(phy_id%4)*8+7,(phy_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*	flexe_demux_reorder_asic_get
*     
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
*     phy_id:0~7
*	  parameter: get value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-7-3   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_reorder_asic_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d \r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (phy_id/4 > MAX_INDEX(reorder_asic_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,reorder_asic_reg[phy_id/4].base_addr,reorder_asic_reg[phy_id/4].offset_addr,
		(phy_id%4)*8+7,(phy_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_demux_autoset_en_set
*
*     
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
*	  parameter: 0:disable 1:enable
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_autoset_en_set(UINT_8 chip_id,UINT_32 parameter)
{	
	if (chip_id > MAX_DEV || parameter > 0xffffffff)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
        }
		
		return RET_PARAERR;
	}

	regp_field_write(chip_id,autoset_en_reg[AUTOSET_EN],parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_autoset_en_get
*
*     
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
*	  *parameter: 0:disable 1:enable
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_autoset_en_get(UINT_8 chip_id,UINT_32* parameter)
{	
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,autoset_en_reg[AUTOSET_EN],parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_demux_mode_info_set
*
*     
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
*     phy_id:0 phy 1 1:phy2
*	  parameter: mode 0:100g mode  1:01 50g mode 2 100g and 50g mode
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_mode_info_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1) || parameter > 0x3)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,phy_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,mode_info_reg[0].base_addr,mode_info_reg[0].offset_addr,phy_id,phy_id,parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_mode_info_get
*
*     
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
*     phy_id:0 phy 1 1:phy2
*	  *parameter: pointer to mode 
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_mode_info_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,mode_info_reg[0].base_addr,mode_info_reg[0].offset_addr,phy_id,phy_id,parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_reorder_info_set
*
*     
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
*     phy_id:0~7
*	  parameter: phy source after reorder
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_reorder_info_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,phy_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,reorder_info_reg[0].base_addr,reorder_info_reg[0].offset_addr,
	phy_id*4+3,phy_id*4,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_reorder_info_get
*
*     
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
*     phy_id:0~7
*	  *parameter:pointer to phy source after reorder
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_reorder_info_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,reorder_info_reg[0].base_addr,reorder_info_reg[0].offset_addr,
	phy_id*4+3,phy_id*4,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_calendar_cfg_set
*
*     
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
*     ts_id:0~79
*	  parameter: ts is on which channel
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_calendar_cfg_set(UINT_8 chip_id,UINT_8 ts_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ts_id > (TS_NUM - 1) || parameter > 0xff)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ts_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,ts_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,calendar_cfg_reg[0].base_addr,calendar_cfg_reg[0].offset_addr+ts_id/4,
	(ts_id%4)*8+7,(ts_id%4)*8,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_calendar_cfg_get
*
*     
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
*     ts_id:0~79
*	  *parameter:pointer to ts is on which channel
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_calendar_cfg_get(UINT_8 chip_id,UINT_8 ts_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ts_id > (TS_NUM - 1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ts_id = %d\r\n",__FUNCTION__,chip_id,ts_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,calendar_cfg_reg[0].base_addr,calendar_cfg_reg[0].offset_addr+ts_id/4,
	(ts_id%4)*8+7,(ts_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_demux_channel_own_tsgroup_set
*
*     
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
*     ch_id:0~79
*	  parameter: ch own tsgroup cfg
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_channel_own_tsgroup_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1) || parameter > 0xff)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ch_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,ch_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,channel_own_tsgroup_reg[CHANNEL_OWN_TSGROUP].base_addr,channel_own_tsgroup_reg[CHANNEL_OWN_TSGROUP].offset_addr+ch_id/4,
	(ch_id%4)*8+7,(ch_id%4)*8,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_channel_own_tsgroup_get
*
*     
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
*     ch_id:0~79
*	 *parameter: pointer to ch own tsgroup cfg
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_channel_own_tsgroup_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ch_id = %d \r\n",__FUNCTION__,chip_id,ch_id);
        }
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,channel_own_tsgroup_reg[CHANNEL_OWN_TSGROUP].base_addr,channel_own_tsgroup_reg[CHANNEL_OWN_TSGROUP].offset_addr+ch_id/4,
	(ch_id%4)*8+7,(ch_id%4)*8,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_ram_full_alm_get
*
*     
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
*     ram_id:0~79
*	  *parameter:pointer to ram full alm
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_ram_full_alm_get(UINT_8 chip_id,UINT_8 ram_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ram_id > 99)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ram_id = %d\r\n",__FUNCTION__,chip_id,ram_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,flexedemux_ram_full_reg[0].base_addr,flexedemux_ram_full_reg[0].offset_addr+ram_id/32,
	ram_id%32,ram_id%32,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_fifo_full_alm_get
*
*     
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
*     phy_id:0~7
*	  *parameter:pointer to ram full alm
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_fifo_full_alm_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,flexedemux_fifo_full_reg[0].base_addr,flexedemux_fifo_full_reg[0].offset_addr,
	phy_id,phy_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_demux_fifo_empty_alm_get
*
*     
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
*     phy_id:0~7
*	  *parameter:pointer to ram empty alm
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-8-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_fifo_empty_alm_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,flexedemux_fifo_empty_reg[0].base_addr,flexedemux_fifo_empty_reg[0].offset_addr,
	phy_id,phy_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_demux_fp_check_alm_get
*
*     
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
*     phy_id:0~7
*	  *parameter:pointer to ram empty alm
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-8-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_fp_check_alm_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,fp_check_alm_reg[0].base_addr,fp_check_alm_reg[0].offset_addr,
	phy_id,phy_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_demux_head_check_phy_alm_get
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
*     phy_id:0~7
*	  *parameter:pointer to ram empty alm
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-11-10   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_head_check_phy_alm_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,head_check_phy_alm_reg[HEAD_CHECK_PHY_ALM].base_addr,head_check_phy_alm_reg[HEAD_CHECK_PHY_ALM].offset_addr,
	phy_id,phy_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_demux_head_check_channel_alm_get
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
*     ch_id:0~79
*	  *parameter:pointer to ram empty alm
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-11-10   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_head_check_channel_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ch_id = %d\r\n",__FUNCTION__,chip_id,ch_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,head_check_channel_alm_reg[ch_id/32].base_addr,head_check_channel_alm_reg[ch_id/32].offset_addr,
	ch_id%32,ch_id%32,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_demux_wcfgram_set
*
*     
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
*	  table_id:0~9
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_wcfgram_set(UINT_8 chip_id,UINT_8 table_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || table_id > 9 || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,table_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,table_id,channel_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	ram_write_single(chip_id,wcfgram[table_id],channel_id,parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_wcfgram_get
*
*     
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
*	  table_id:0~9
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_wcfgram_get(UINT_8 chip_id,UINT_8 table_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || table_id > 9 || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,table_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,table_id,channel_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	ram_read_single(chip_id,wcfgram[table_id],channel_id,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION flexe_demux_rdrule_set
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to rdrule value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_rdrule_set(UINT_8 chip_id,UINT_32 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > 6399)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	ram_write_single(chip_id,rdrule,channel_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_rdrule_get
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_rdrule_get(UINT_8 chip_id,UINT_32 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > 6399)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	ram_read_single(chip_id,rdrule,channel_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_property_set
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_ts_num_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	UINT_32 read_value = 0;
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	ram_read_single(chip_id,property,channel_id,&read_value);

	*parameter = ((*parameter & MASK_8_BIT) << SHIFT_BIT_12) | (read_value & MASK_12_BIT);
	
	ram_write_single(chip_id,property,channel_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_property_get
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_ts_num_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	ram_read_single(chip_id,property,channel_id,parameter);
	*parameter = (*parameter >> SHIFT_BIT_12) & MASK_8_BIT;

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_property_blk_set
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_blk_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	UINT_32 read_value = 0;
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	ram_read_single(chip_id,property,channel_id,&read_value);

	*parameter = ((*parameter & MASK_4_BIT) << SHIFT_BIT_8) | (read_value & 0xff0ff);
	
	ram_write_single(chip_id,property,channel_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_property_blk_get
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_blk_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	ram_read_single(chip_id,property,channel_id,parameter);
	*parameter = (*parameter >> SHIFT_BIT_8) & MASK_4_BIT;
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_property_cycle_set
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_cycle_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	UINT_32 read_value = 0;
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	ram_read_single(chip_id,property,channel_id,&read_value);

	*parameter = ((*parameter & MASK_8_BIT) ) | (read_value & 0xfff00);
	
	ram_write_single(chip_id,property,channel_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_property_cycle_get
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_cycle_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	ram_read_single(chip_id,property,channel_id,parameter);
	*parameter = *parameter & MASK_8_BIT;
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_property_set
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to property
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	ram_write_single(chip_id,property,channel_id,parameter);
		
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_property_get
*
*     
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
*     channel_id:0~79
*	  *parameter:pointer to property
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	ram_read_single(chip_id,property,channel_id,parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_maxlevel_interleave_get
*
*     
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
*     fifo_id:0~7
*	  *parameter:pointer to property
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_maxlevel_interleave_get(UINT_8 chip_id,UINT_8 fifo_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || fifo_id > 7)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, fifo_id = %d\r\n",__FUNCTION__,chip_id,fifo_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,maxlevel_interleave_reg[fifo_id/4].base_addr,maxlevel_interleave_reg[fifo_id/4].offset_addr,
		(fifo_id%4)*8+7,(fifo_id%4)*8,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_maxlevel_clr_set
*
*     
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
*	  parameter:0:enable 1:disable
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_maxlevel_clr_set(UINT_8 chip_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || parameter > 1)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
        }
		
		return RET_PARAERR;
	}

	regp_field_write(chip_id,maxlevel_clr_reg[MAXLEVEL_CLR],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_maxlevel_clr_get
*
*     
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
*	  *parameter:0:enable 1:disable
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_maxlevel_clr_get(UINT_8 chip_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,maxlevel_clr_reg[MAXLEVEL_CLR],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION flexe_demux_interleave_rst_set
*
*     
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
*	  fifo_id: 0~7
*	  parameter:0:enable 1:disable
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_interleave_rst_set(UINT_8 chip_id,UINT_8 fifo_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || fifo_id > 7 || parameter > 1)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,fifo_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,fifo_id,parameter);
        }
		
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,interleave_rst_reg[INTERLEAVE_RST].base_addr,interleave_rst_reg[INTERLEAVE_RST].offset_addr,
		fifo_id,fifo_id,parameter);
	
	return RET_SUCCESS;
}
RET_STATUS flexe_demux_interleave_rst_sync_set(UINT_8 chip_id, UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
        }
		
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,interleave_rst_reg[INTERLEAVE_RST].base_addr,interleave_rst_reg[INTERLEAVE_RST].offset_addr,7,0,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_ts_en_b_set   
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
*     ts_id:0~79
*	  parameter: 0:enable 1:disable
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_ts_en_b_set(UINT_8 chip_id,UINT_8 ts_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ts_id > (TS_NUM-1) || parameter > 1)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ts_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,ts_id,parameter);
        }
		
		return RET_PARAERR;
	}

	if (ts_id/32 > MAX_INDEX(ready_ts_b_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,ready_ts_b_reg[ts_id/32].base_addr,ready_ts_b_reg[ts_id/32].offset_addr,ts_id%32,ts_id%32,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_ts_en_b_get   
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
*     ts_id:0~79
*	  parameter: 0:enable 1:disable
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_ts_en_b_get(UINT_8 chip_id,UINT_8 ts_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ts_id > (TS_NUM - 1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ts_id = %d\r\n",__FUNCTION__,chip_id,ts_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (ts_id/32 > MAX_INDEX(ready_ts_b_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,ready_ts_b_reg[ts_id/32].base_addr,ready_ts_b_reg[ts_id/32].offset_addr,ts_id%32,ts_id%32,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_calendar_cfg_b_set   
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
*     ts_id:0~79
*	  parameter: set value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_calendar_cfg_b_set(UINT_8 chip_id,UINT_8 ts_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ts_id > (TS_NUM - 1) || parameter > 0xff)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ts_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,ts_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,calendar_cfg_b_reg[0].base_addr,calendar_cfg_b_reg[0].offset_addr+ts_id/4,
	(ts_id%4)*8+7,(ts_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_calendar_cfg_b_get   
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
*     ts_id:0~79
*	  parameter: get value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_calendar_cfg_b_get(UINT_8 chip_id,UINT_8 ts_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ts_id > (TS_NUM - 1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ts_id = %d\r\n",__FUNCTION__,chip_id,ts_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,calendar_cfg_b_reg[0].base_addr,calendar_cfg_b_reg[0].offset_addr+ts_id/4,
	(ts_id%4)*8+7,(ts_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_channel_own_tsgroup_b_set   
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
*     ch_id:0~79
*	  parameter: set value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_channel_own_tsgroup_b_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1) || parameter > 0xff)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ch_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,ch_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,channel_own_tsgroup_b_reg[CHANNEL_OWN_TSGROUP_B_0].base_addr,channel_own_tsgroup_b_reg[CHANNEL_OWN_TSGROUP_B_0].offset_addr+ch_id/4,
	(ch_id%4)*8+7,(ch_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_channel_own_tsgroup_b_get   
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
*     ch_id:0~79
*	  parameter: get value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_channel_own_tsgroup_b_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, ch_id = %d \r\n",__FUNCTION__,chip_id,ch_id);
        }
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,channel_own_tsgroup_b_reg[CHANNEL_OWN_TSGROUP_B_0].base_addr,channel_own_tsgroup_b_reg[CHANNEL_OWN_TSGROUP_B_0].offset_addr+ch_id/4,
	(ch_id%4)*8+7,(ch_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_enable_ab_set   
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
*	  parameter: 0: B disable  1: B able
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_enable_ab_set(UINT_8 chip_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || parameter > 1)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
        }
		
		return RET_PARAERR;
	}

	regp_field_write(chip_id,enable_ab_reg[ENABLE_AB],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_enable_ab_get   
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
*	  parameter: 0: B disable  1: B able
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_enable_ab_get(UINT_8 chip_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	regp_field_read(chip_id,enable_ab_reg[ENABLE_AB],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_verify_cfg_set   
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
*	  phy_sel: 0: phy1-phy4  1: phy5-phy8
*	  parameter: set value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_verify_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,parameter = %d\r\n",
		   	__FUNCTION__,chip_id,parameter);
        }
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(verify_cfg_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,verify_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_verify_cfg_get   
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
*	  phy_sel: 0: phy1-phy4  1: phy5-phy8
*	  parameter: get value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_verify_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(verify_cfg_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,verify_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_localfault_en_set   
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
*	  parameter: 0: not replace  1: replace
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_localfault_en_set(UINT_8 chip_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || parameter > 1)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,localfault_en_reg[LOCALFAULT_EN],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_localfault_en_get   
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
*	  parameter: 0: not replace  1: replace
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_localfault_en_get(UINT_8 chip_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	regp_field_read(chip_id,localfault_en_reg[LOCALFAULT_EN],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_threshold_set   
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
*	  parameter: set value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-29   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_threshold_set(UINT_8 chip_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || parameter > 0xffffffff)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
        }
		
		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,threshold_reg[THRESHOLD],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_threshold_get   
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
*	  parameter: get value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-29   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_threshold_get(UINT_8 chip_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,threshold_reg[THRESHOLD],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_threshold_full_set   
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
*	  phy_id:0~7
*	  parameter: set value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-8-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_threshold_full_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1) || parameter > 0xff)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,phy_id = %d,parameter = %d\r\n",
		   	__FUNCTION__,chip_id,phy_id,parameter);
        }
		
		return RET_PARAERR;
	}

	if (phy_id/4 > MAX_INDEX(threshold_full_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,threshold_full_reg[phy_id/4].base_addr,threshold_full_reg[phy_id/4].offset_addr,
		(phy_id%4)*8+7,(phy_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_threshold_full_get   
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
*	  phy_id:0~7
*	  parameter: get value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-8-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_threshold_full_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	if (phy_id/4 > MAX_INDEX(threshold_full_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,threshold_full_reg[phy_id/4].base_addr,threshold_full_reg[phy_id/4].offset_addr,
		(phy_id%4)*8+7,(phy_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_threshold_empty_set   
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
*	  phy_id:0~7
*	  parameter: set value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-8-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_threshold_empty_set(UINT_8 chip_id,UINT_8 phy_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1) || parameter > 0xff)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,phy_id = %d,parameter = %d\r\n",
		   	__FUNCTION__,chip_id,phy_id,parameter);
        }
		
		return RET_PARAERR;
	}

	if (phy_id/4 > MAX_INDEX(threshold_empty_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,threshold_empty_reg[phy_id/4].base_addr,threshold_empty_reg[phy_id/4].offset_addr,
		(phy_id%4)*8+7,(phy_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*  flexe_demux_threshold_empty_get   
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
*	  phy_id:0~7
*	  parameter: get value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-8-7   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_threshold_empty_get(UINT_8 chip_id,UINT_8 phy_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || phy_id > (PHY_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,phy_id = %d\r\n",__FUNCTION__,chip_id,phy_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}

	if (phy_id/4 > MAX_INDEX(threshold_empty_reg))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,threshold_empty_reg[phy_id/4].base_addr,threshold_empty_reg[phy_id/4].offset_addr,
		(phy_id%4)*8+7,(phy_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*   flexe_demux_wcfgram_b_set  
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
*	  table_id:0~9
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_wcfgram_b_set(UINT_8 chip_id,UINT_8 table_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || table_id > 9 || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,table_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,table_id,channel_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	ram_write_single(chip_id,wcfgram_b[table_id],channel_id,parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION 
*
*     flexe_demux_wcfgram_b_get
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
*	  table_id:0~9
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_wcfgram_b_get(UINT_8 chip_id,UINT_8 table_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || table_id > 9 || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,table_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,table_id,channel_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	ram_read_single(chip_id,wcfgram_b[table_id],channel_id,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_demux_rdrule_b_set
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
*     channel_id:0~79
*	  *parameter:pointer to rdrule value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_rdrule_b_set(UINT_8 chip_id,UINT_32 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > 6399)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	ram_write_single(chip_id,rdrule_b,channel_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION 
*
*     flexe_demux_rdrule_b_get
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
*     channel_id:0~79
*	  *parameter:pointer to tsqueue value 640bit width
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_rdrule_b_get(UINT_8 chip_id,UINT_32 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > 6399)
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	ram_read_single(chip_id,rdrule_b,channel_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_demux_property_b_set
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
*     channel_id:0~79
*	  *parameter:pointer to property
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_b_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	ram_write_single(chip_id,chproperty_b,channel_id,parameter);
		
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION 
*
*     flexe_demux_property_b_get
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
*     channel_id:0~79
*	  *parameter:pointer to property
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    dingyi      2018-6-12   1.0           initial
*
******************************************************************************/
RET_STATUS flexe_demux_property_b_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (flexe_demux_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, channel_id = %d\r\n",__FUNCTION__,chip_id,channel_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_demux_para_debug)
		{			
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}
	
		return RET_PARAERR;
	}
	
	ram_read_single(chip_id,chproperty_b,channel_id,parameter);

	return RET_SUCCESS;
}

