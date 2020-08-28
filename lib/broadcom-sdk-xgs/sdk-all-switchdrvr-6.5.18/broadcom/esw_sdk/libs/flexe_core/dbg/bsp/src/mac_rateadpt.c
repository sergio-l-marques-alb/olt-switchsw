

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int mac_rateadpt_para_debug = 0;

const UINT_32 mac_rateadpt_fifo_level_old[80]=
{
	0x0d0b0702,
	0x19140c03,
	0x22180e04,
	0x2c221405,
	0x3c2d1a06,
	0x4c382007,
	0x5c482808,
	0x6c583109,
	0x7864370a,
	0x8c78440f,
	0x9c884c0f,
	0xa08c4e0f,
	0xa08c4e0f,
	0xa08c4e0f,
	0xa08c4e0f,
	0xa08c4e0f,
	0xa08c4e0f,
	0xa08c5014,
	0xa08c5014,
	0xa08c5014,
	0xa08c5014,
	0xa08c5014,
	0xa08c5014,
	0xa08c5014,
	0xa08c5014,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c5319,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
	0xa08c551e,
};
const UINT_32 mac_rateadpt_fifo_level[80]=
{
	0x0d0b0702,
	0x19140c03,
	0x22180f04,
	0x2c221405,
	0x3c2d1a06,
	0x4c382007,
	0x5c482908,
	0x6c583109,
	0x7864370a,
	0x8c78410a,
	0x9c88490a,
	0xa08c4b0a,
	0xa08c4b0a,
	0xa08c4b0a,
	0xa08c4b0a,
	0xa08c4b0a,
	0xa08c4b0a,
	0xa08c4b0a,
	0xa08c4b0a,
	0xa08c4b0a,
	0xa08c4b0f,
	0xa08c4b0f,
	0xa08c4b0f,
	0xa08c4b0f,
	0xa08c4b0f,
	0xa08c4b0f,
	0xa08c4b0f,
	0xa08c4b0f,
	0xa08c4b0f,
	0xa08c4b0f,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
	0xa08c4b14,
};

const CHIP_REG MAC_FIFO_LEVEL_CFG_REG[]=
{
	{MAC_LOW_LVL,MAC_ADAPT_BASE_ADDR,0x0,7,0},
	{MAC_STA_LVL,MAC_ADAPT_BASE_ADDR,0x0,15,8},
	{MAC_HIG_LVL,MAC_ADAPT_BASE_ADDR,0x0,23,16},
	{MAC_RSK_LVL,MAC_ADAPT_BASE_ADDR,0x0,31,24},
};
const CHIP_REG MAC_RATEADP_FULL_REG[]=
{
	{MAC_RATEADP_FULL_0,MAC_ADAPT_BASE_ADDR,0x50,31,0},
	{MAC_RATEADP_FULL_1,MAC_ADAPT_BASE_ADDR,0x51,31,0},
	{MAC_RATEADP_FULL_2,MAC_ADAPT_BASE_ADDR,0x52,15,0},
};
const CHIP_REG MAC_RATEADP_EMPTY_REG[]=
{
	{MAC_RATEADP_EMPTY_0,MAC_ADAPT_BASE_ADDR,0x53,31,0},
	{MAC_RATEADP_EMPTY_1,MAC_ADAPT_BASE_ADDR,0x54,31,0},
	{MAC_RATEADP_EMPTY_2,MAC_ADAPT_BASE_ADDR,0x55,15,0},
};
const CHIP_REG MAC_RAM_OVERFLOW_REG[]=
{
	{OVERFLOW_0,MAC_ADAPT_BASE_ADDR,0xfb,31,0},
	{OVERFLOW_1,MAC_ADAPT_BASE_ADDR,0xfc,31,0},
	{OVERFLOW_2,MAC_ADAPT_BASE_ADDR,0xfd,15,0},
};
const CHIP_REG MAC_BLOCK_TYPE_REG[]=
{
	{BLOCK_TYPE,MAC_ADAPT_BASE_ADDR,0x56,1,0},
};
const CHIP_REG MAC_CLR_HIST_REG[]=
{
	{MAC_RATEADP_CHID_LVL_HIST,MAC_ADAPT_BASE_ADDR,0x57,6,0},
	{MAC_RATEADP_CLR_HIST,MAC_ADAPT_BASE_ADDR,0x57,8,8},
};
const CHIP_REG MAC_IND_SEL_REG[]=
{
	{IND_SEL,MAC_ADAPT_BASE_ADDR,0x59,0,0},
};
const CHIP_REG MAC_INS_DEL_GAP_REG[]=
{
	{INS_GAP,MAC_ADAPT_BASE_ADDR,0x5a,8,0},
	{DEL_GAP,MAC_ADAPT_BASE_ADDR,0x5a,24,16},
};
const CHIP_REG MAC_LVL_HIST_REG[]=
{
	{MAC_RATEADP_LOW_HIST,MAC_ADAPT_BASE_ADDR,0x58,7,0},
	{MAC_RATEADP_HIG_HIST,MAC_ADAPT_BASE_ADDR,0x58,15,8},
};

/*mac_adj*/
const CHIP_REG MAC_ADJ_CFG_REG[]=
{
	{MAC_I_NOMINAL_VAL,MAC_ADJ_BASE_ADDR,0x0,12,0},
	{MAC_I_ADJ_P_I,MAC_ADJ_BASE_ADDR,0x0,28,16},
	{MAC_I_ADJ_P_N,MAC_ADJ_BASE_ADDR,0x1,7,0},
	{MAC_I_ADJ_P_D,MAC_ADJ_BASE_ADDR,0x1,15,8},
	{MAC_I_ADJ_ONLY,MAC_ADJ_BASE_ADDR,0x1,16,16},
	{MAC_I_FIFO_RESTART_EN,MAC_ADJ_BASE_ADDR,0x1,17,17},
	{MAC_I_ADJ_ALM_RESTART_EN,MAC_ADJ_BASE_ADDR,0x1,18,18},
	{MAC_I_PLL_COEFF,MAC_ADJ_BASE_ADDR,0x1,20,19},
	{MAC_I_ADJ_P_D_EN,MAC_ADJ_BASE_ADDR,0x1,21,21},
	{MAC_I_CFG_FINISH,MAC_ADJ_BASE_ADDR,0x1,22,22},
};
const CHIP_REG MAC_CNT_ALM_REG[]=
{
	{MAC_O_ADJ_CNT_ALM_0,MAC_ADJ_BASE_ADDR,0xa0,31,0},
	{MAC_O_ADJ_CNT_ALM_1,MAC_ADJ_BASE_ADDR,0xa1,31,0},
	{MAC_O_ADJ_CNT_ALM_2,MAC_ADJ_BASE_ADDR,0xa2,15,0},
};
const CHIP_REG MAC_CORE_ALM_REG[]=
{
	{MAC_O_ADJ_CORE_ALM_0,MAC_ADJ_BASE_ADDR,0xa3,31,0},
	{MAC_O_ADJ_CORE_ALM_1,MAC_ADJ_BASE_ADDR,0xa4,31,0},
	{MAC_O_ADJ_CORE_ALM_2,MAC_ADJ_BASE_ADDR,0xa5,15,0},
};
const CHIP_REG MAC_BLK_NUM_CFG[]=
{
	{MAC_RATEADP_BLK_NUM,MAC_ADAPT_BASE_ADDR,0x5b,4,0},
	{MAC_CHAN_EN,MAC_ADAPT_BASE_ADDR,0x5b,5,5},
};

const CHIP_REG MAC_MAP_REG[]=
{
	{MAC_MAP_SEQ,MAC_ADAPT_BASE_ADDR,0xab,3,0},
	{MAC_MAP_CHID,MAC_ADAPT_BASE_ADDR,0xab,14,8},
	{MAC_RATEADP_MAP_EN,MAC_ADAPT_BASE_ADDR,0xab,16,16},
};
/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_fifo_level_cfg_set
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
*      channel_id: channel select 0~79
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
*    dingyi    2018-1-17    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadpt_fifo_level_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (CH_NUM-1)))
	{
	   if (mac_rateadpt_para_debug)
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

	regp_write(chip_id,MAC_FIFO_LEVEL_CFG_REG[0].base_addr,MAC_FIFO_LEVEL_CFG_REG[0].offset_addr+channel_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_fifo_level_cfg_get
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
*	  field_id: field select 0~3  0:MAC_LOW_LVL  1:MAC_STA_LVL  2:MAC_HIG_LVL  3:MAC_RSK_LVL
*	  channel_id: channel select 0~79
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
*    dingyi    2018-1-17    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadpt_fifo_level_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (CH_NUM-1)))
	{
	   if (mac_rateadpt_para_debug)
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
        if (mac_rateadpt_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_read(chip_id,MAC_FIFO_LEVEL_CFG_REG[0].base_addr,MAC_FIFO_LEVEL_CFG_REG[0].offset_addr+channel_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_full_get
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
*	  channel_id: channel select 0~79
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
*    dingyi    2018-1-17    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadpt_full_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (CH_NUM-1)))
	{
	   if (mac_rateadpt_para_debug)
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
        if (mac_rateadpt_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (channel_id/32 > MAX_INDEX(MAC_RATEADP_FULL_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,MAC_RATEADP_FULL_REG[channel_id/32].base_addr,MAC_RATEADP_FULL_REG[channel_id/32].offset_addr,channel_id%32,channel_id%32,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_empty_get
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
*	  channel_id: channel select 0~79
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
*    dingyi    2018-1-17    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadpt_empty_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (CH_NUM-1)))
	{
	   if (mac_rateadpt_para_debug)
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
        if (mac_rateadpt_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (channel_id/32 > MAX_INDEX(MAC_RATEADP_EMPTY_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,MAC_RATEADP_EMPTY_REG[channel_id/32].base_addr,MAC_RATEADP_EMPTY_REG[channel_id/32].offset_addr,channel_id%32,channel_id%32,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_ram_overflow_get
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
*	  channel_id: channel select 0~79
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
RET_STATUS mac_rateadpt_ram_overflow_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (CH_NUM-1)))
	{
	   if (mac_rateadpt_para_debug)
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
        if (mac_rateadpt_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (channel_id/32 > MAX_INDEX(MAC_RAM_OVERFLOW_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,MAC_RAM_OVERFLOW_REG[channel_id/32].base_addr,MAC_RAM_OVERFLOW_REG[channel_id/32].offset_addr,channel_id%32,channel_id%32,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_block_type_set
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
*    dingyi    2018-1-17    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadpt_block_type_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 3))
	{
	   if (mac_rateadpt_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}


	regp_field_write(chip_id,MAC_BLOCK_TYPE_REG[BLOCK_TYPE],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_block_type_get
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
*    dingyi    2018-1-17    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadpt_block_type_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (mac_rateadpt_para_debug)
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
        if (mac_rateadpt_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,MAC_BLOCK_TYPE_REG[BLOCK_TYPE],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_clr_hist_set
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
*    dingyi    2018-1-17    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadpt_clr_hist_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (mac_rateadpt_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(MAC_CLR_HIST_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,MAC_CLR_HIST_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_block_type_get
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
*    dingyi    2018-1-17    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadpt_clr_hist_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (mac_rateadpt_para_debug)
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
        if (mac_rateadpt_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(MAC_CLR_HIST_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,MAC_CLR_HIST_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mac_rateadpt_lvl_hist_get
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
*	  field_id: 0~1  0:LOW_HIST  1:HIG_HIST
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
*    dingyi    2018-1-17    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadpt_lvl_hist_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (mac_rateadpt_para_debug)
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
        if (mac_rateadpt_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(MAC_LVL_HIST_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,MAC_LVL_HIST_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
* 
*
*	  mac_adj_cfg_set
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
*	  field_id:0-9 0:I_NOMINAL_VAL  1:I_ADJ_P_I  2:I_ADJ_P_N  3:I_ADJ_P_D  4:I_ADJ_ONLY
*				   5:I_FIFO_RESTART_EN  6:I_ADJ_ALM_RESTART_EN  7:I_PLL_COEFF
*				   8:I_ADJ_P_D_EN  9:I_CFG_FINISH
*	  ch_id:0-79
*	  parameter:set value
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
*    dingyi    	2018-11-10    1.0           initial
*
******************************************************************************/
RET_STATUS mac_adj_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 ch_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(MAC_ADJ_CFG_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,MAC_ADJ_CFG_REG[field_id].base_addr,MAC_ADJ_CFG_REG[field_id].offset_addr+ch_id*2,MAC_ADJ_CFG_REG[field_id].end_bit,
		MAC_ADJ_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
* 
*
*     mac_rateadp_ind_sel_get
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
*	  field_id:0-9 0:I_NOMINAL_VAL  1:I_ADJ_P_I  2:I_ADJ_P_N  3:I_ADJ_P_D  4:I_ADJ_ONLY
*				   5:I_FIFO_RESTART_EN  6:I_ADJ_ALM_RESTART_EN  7:I_PLL_COEFF
*				   8:I_ADJ_P_D_EN  9:I_CFG_FINISH
*	  ch_id:0-79
*	  parameter:get value
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
*    dingyi    	2018-11-10    1.0           initial
*
******************************************************************************/
RET_STATUS mac_adj_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(MAC_ADJ_CFG_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,MAC_ADJ_CFG_REG[field_id].base_addr,MAC_ADJ_CFG_REG[field_id].offset_addr+ch_id*2,MAC_ADJ_CFG_REG[field_id].end_bit,
		MAC_ADJ_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
* 
*
*	  mac_adj_cnt_alm_get
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
*	  ch_id:0-79
*	  parameter:get value
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
*    dingyi    	2018-9-27    1.0           initial
*
******************************************************************************/
RET_STATUS mac_adj_cnt_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (ch_id/32 > MAX_INDEX(MAC_CNT_ALM_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,MAC_CNT_ALM_REG[ch_id/32].base_addr,MAC_CNT_ALM_REG[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
* 
*
*	  mac_adj_core_alm_get
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
*	  ch_id:0-79
*	  parameter:get value
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
*    dingyi    	2018-9-27    1.0           initial
*
******************************************************************************/
RET_STATUS mac_adj_core_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (ch_id/32 > MAX_INDEX(MAC_CORE_ALM_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,MAC_CORE_ALM_REG[ch_id/32].base_addr,MAC_CORE_ALM_REG[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION mac_rateadp_block_num_set
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
*	  ch_id:0~CH_NUM-1
*	  parameter:ts_num
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
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_block_num_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,MAC_BLK_NUM_CFG[MAC_RATEADP_BLK_NUM].base_addr,MAC_BLK_NUM_CFG[MAC_RATEADP_BLK_NUM].offset_addr+ch_id,MAC_BLK_NUM_CFG[MAC_RATEADP_BLK_NUM].end_bit,
		MAC_BLK_NUM_CFG[MAC_RATEADP_BLK_NUM].start_bit,parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION mac_rateadp_block_num_get
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
*	  ch_id:0~CH_NUM-1
*	  *parameter
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
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_block_num_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,MAC_BLK_NUM_CFG[MAC_RATEADP_BLK_NUM].base_addr,MAC_BLK_NUM_CFG[MAC_RATEADP_BLK_NUM].offset_addr+ch_id,MAC_BLK_NUM_CFG[MAC_RATEADP_BLK_NUM].end_bit,
		MAC_BLK_NUM_CFG[MAC_RATEADP_BLK_NUM].start_bit,parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION mac_rateadp_chan_en_set
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
*	  ch_id:0~CH_NUM-1
*	  parameter:ts_num
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
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_chan_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (MAC_CHAN_EN > MAX_INDEX(MAC_BLK_NUM_CFG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,MAC_BLK_NUM_CFG[MAC_CHAN_EN].base_addr,MAC_BLK_NUM_CFG[MAC_CHAN_EN].offset_addr+ch_id,MAC_BLK_NUM_CFG[MAC_CHAN_EN].end_bit,
		MAC_BLK_NUM_CFG[MAC_CHAN_EN].start_bit,parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION mac_rateadp_chan_en_get
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
*	  ch_id:0~CH_NUM-1
*	  *parameter
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
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_chan_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (MAC_CHAN_EN > MAX_INDEX(MAC_BLK_NUM_CFG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,MAC_BLK_NUM_CFG[MAC_CHAN_EN].base_addr,MAC_BLK_NUM_CFG[MAC_CHAN_EN].offset_addr+ch_id,MAC_BLK_NUM_CFG[MAC_CHAN_EN].end_bit,
		MAC_BLK_NUM_CFG[MAC_CHAN_EN].start_bit,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION mac_rateadp_map_reg_set
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
*	  ch_id:0~CH_NUM
*	  field_id:MAP_SEQ;MAP_CHID;MAP_EN
*	  parameter:value
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
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_map_reg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32 parameter)
{  
	CHIP_REG reg_to_write={field_id,MAC_MAP_REG[field_id].base_addr,MAC_MAP_REG[field_id].offset_addr+ch_id,MAC_MAP_REG[field_id].end_bit,MAC_MAP_REG[field_id].start_bit};

	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(MAC_MAP_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	

	regp_field_write(chip_id,reg_to_write,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION mac_rateadp_map_reg_get
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
*	  ch_id:0~CH_NUM
*	  field_id:MAP_SEQ;MAP_CHID;MAP_EN
*	  *parameter:
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
*    jxma    	2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_map_reg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32* parameter)
{
	CHIP_REG reg_to_read={field_id,MAC_MAP_REG[field_id].base_addr,MAC_MAP_REG[field_id].offset_addr+ch_id,MAC_MAP_REG[field_id].end_bit,MAC_MAP_REG[field_id].start_bit};

	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(MAC_MAP_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	

	regp_field_read(chip_id,reg_to_read,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     mac_rateadp_ind_sel_set
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
*	  parameter:set value
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
*    dingyi    	2018-11-10    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_ind_sel_set(UINT_8 chip_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,MAC_IND_SEL_REG[IND_SEL],parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     mac_rateadp_ind_sel_get
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
*	  parameter:get value
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
*    dingyi    	2018-11-10    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_ind_sel_get(UINT_8 chip_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,MAC_IND_SEL_REG[IND_SEL],parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     mac_rateadp_ins_del_gap_set
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
*	  field_id: 0:INS_GAP  1:DEL_GAP
*	  parameter:set value
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
*    dingyi    	2018-11-10    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_ins_del_gap_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(MAC_INS_DEL_GAP_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,MAC_INS_DEL_GAP_REG[field_id],parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     mac_rateadp_ins_del_gap_get
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
*	  field_id: 0:INS_GAP  1:DEL_GAP
*	  parameter:get value
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
*    dingyi    	2018-11-10    1.0           initial
*
******************************************************************************/
RET_STATUS mac_rateadp_ins_del_gap_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(MAC_INS_DEL_GAP_REG))
	{
		if (mac_rateadpt_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,MAC_INS_DEL_GAP_REG[field_id],parameter);

	return RET_SUCCESS;
}
