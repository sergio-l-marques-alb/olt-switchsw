
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int rateadp_para_debug = 0;

const UINT_32 rateadpt_fifo_level_old[80]=
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
const UINT_32 rateadpt_fifo_level[80]=
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
const CHIP_REG FIFO_LEVEL_CFG_REG[]=
{
	{LOW_LVL,RATEADP_BASE_ADDR,0x0,7,0},
	{STA_LVL,RATEADP_BASE_ADDR,0x0,15,8},
	{HIG_LVL,RATEADP_BASE_ADDR,0x0,23,16},
	{RSK_LVL,RATEADP_BASE_ADDR,0x0,31,24},
};
const CHIP_REG BLOCK_TYPE_REG[]=
{
	{BLOCK_TYPE,RATEADP_BASE_ADDR,0x56,1,0},
};
const CHIP_REG IND_SEL_REG[]=
{
	{RATEADP_IND_SEL,RATEADP_BASE_ADDR,0x59,0,0},
};
const CHIP_REG INS_DEL_GAP_REG[]=
{
	{RATEADP_INS_GAP,RATEADP_BASE_ADDR,0x5a,8,0},
	{RATEADP_DEL_GAP,RATEADP_BASE_ADDR,0x5a,24,16},
};
const CHIP_REG RATEADP_FULL_REG[]=
{
	{RATEADP_FULL_0,RATEADP_BASE_ADDR,0x50,31,0},
	{RATEADP_FULL_1,RATEADP_BASE_ADDR,0x51,31,0},
	{RATEADP_FULL_2,RATEADP_BASE_ADDR,0x52,15,0},
};
const CHIP_REG RATEADP_EMPTY_REG[]=
{
	{RATEADP_EMPTY_0,RATEADP_BASE_ADDR,0x53,31,0},
	{RATEADP_EMPTY_1,RATEADP_BASE_ADDR,0x54,31,0},
	{RATEADP_EMPTY_2,RATEADP_BASE_ADDR,0x55,15,0},
};
const CHIP_REG RAM_OVERFLOW_REG[]=
{
	{RATEADP_OVERFLOW_0,RATEADP_BASE_ADDR,0xfb,31,0},
	{RATEADP_OVERFLOW_1,RATEADP_BASE_ADDR,0xfc,31,0},
	{RATEADP_OVERFLOW_2,RATEADP_BASE_ADDR,0xfd,15,0},
};
const CHIP_REG CLR_HIST_REG[]=
{
	{RATEADP_CHID_LVL_HIST,RATEADP_BASE_ADDR,0x57,6,0},
	{RATEADP_CLR_HIST,RATEADP_BASE_ADDR,0x57,8,8},
};
const CHIP_REG LVL_HIST_REG[]=
{
	{RATEADP_LOW_HIST,RATEADP_BASE_ADDR,0x58,6,0},
	{RATEADP_HIG_HIST,RATEADP_BASE_ADDR,0x58,14,8},
};

/*adj*/
const CHIP_REG ADJ_CFG_REG[]=
{
	{I_NOMINAL_VAL,ADJ_BASE_ADDR,0x0,12,0},
	{I_ADJ_P_I,ADJ_BASE_ADDR,0x0,28,16},
	{I_ADJ_P_N,ADJ_BASE_ADDR,0x1,7,0},
	{I_ADJ_P_D,ADJ_BASE_ADDR,0x1,15,8},
	{I_ADJ_ONLY,ADJ_BASE_ADDR,0x1,16,16},
	{I_FIFO_RESTART_EN,ADJ_BASE_ADDR,0x1,17,17},
	{I_ADJ_ALM_RESTART_EN,ADJ_BASE_ADDR,0x1,18,18},
	{I_PLL_COEFF,ADJ_BASE_ADDR,0x1,20,19},
	{I_ADJ_P_D_EN,ADJ_BASE_ADDR,0x1,21,21},
	{I_CFG_FINISH,ADJ_BASE_ADDR,0x1,22,22},
};
const CHIP_REG CNT_ALM_REG[]=
{
	{O_ADJ_CNT_ALM_0,ADJ_BASE_ADDR,0xa0,31,0},
	{O_ADJ_CNT_ALM_1,ADJ_BASE_ADDR,0xa1,31,0},
	{O_ADJ_CNT_ALM_2,ADJ_BASE_ADDR,0xa2,15,0},
};
const CHIP_REG CORE_ALM_REG[]=
{
	{O_ADJ_CORE_ALM_0,ADJ_BASE_ADDR,0xa3,31,0},
	{O_ADJ_CORE_ALM_1,ADJ_BASE_ADDR,0xa4,31,0},
	{O_ADJ_CORE_ALM_2,ADJ_BASE_ADDR,0xa5,15,0},
};
const CHIP_REG BLK_NUM_CFG[]=
{
	{RATEADP_BLK_NUM,RATEADP_BASE_ADDR,0x5b,4,0},
	{CHAN_EN,RATEADP_BASE_ADDR,0x5b,5,5},
};

const CHIP_REG MAP_REG[]=
{
	{MAP_SEQ,RATEADP_BASE_ADDR,0xab,3,0},
	{MAP_CHID,RATEADP_BASE_ADDR,0xab,14,8},
	{RATEADP_MAP_EN,RATEADP_BASE_ADDR,0xab,16,16},
};
/******************************************************************************
*
* FUNCTION rateadp_alm_get
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
*	  alm_id:0~3
*	  *parameter_h:high 7~0bit
*     *parameter_l:low 31~0bit
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
RET_STATUS rateadp_alm_get(UINT_8 chip_id,UINT_8 alm_id,UINT_8 chnl,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || alm_id > 3 || chnl > (CH_NUM-1))
	{
		if(rateadp_para_debug)
		{
			printf("[%s] invalide parameter!!chip_id = %d,alm_id=%d,chnl=%d\r\n",__FUNCTION__,chip_id,alm_id,chnl);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	switch(alm_id)
	{
		case 0:
			regp_bit_read(chip_id,RATEADP_FULL_REG[chnl/32].base_addr,RATEADP_FULL_REG[chnl/32].offset_addr,chnl%32,chnl%32,parameter);
			break;
		case 1:
			regp_bit_read(chip_id,RATEADP_EMPTY_REG[chnl/32].base_addr,RATEADP_EMPTY_REG[chnl/32].offset_addr,chnl%32,chnl%32,parameter);
			break;
		default:
			return RET_PARAERR;
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION rateadp_fifo_level_cfg_set
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
*	  field_id:0~3
*	  parameter:fifo_level
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
RET_STATUS rateadp_fifo_level_cfg_set(UINT_8 chip_id,UINT_8 chnl_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV ||chnl_id > (CH_NUM-1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,chnl_id=%d\r\n",__FUNCTION__,chip_id,chnl_id);
		}

		return RET_PARAERR;
	}

	regp_write(chip_id,FIFO_LEVEL_CFG_REG[0].base_addr,FIFO_LEVEL_CFG_REG[0].offset_addr+chnl_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION rateadp_fifo_level_cfg_get
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
*	  field_id:0~3
*	  *parameter:pointer to fifo_level
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
RET_STATUS rateadp_fifo_level_cfg_get(UINT_8 chip_id,UINT_8 chnl_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV ||chnl_id > (CH_NUM-1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,chnl_id=%d\r\n",__FUNCTION__,chip_id,chnl_id);
		}

		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_read(chip_id,FIFO_LEVEL_CFG_REG[0].base_addr,FIFO_LEVEL_CFG_REG[0].offset_addr+chnl_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION rateadp_block_type_set
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
*	  parameter:0:IDLE 1:LF 2/3:ERR_BLOCK
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
RET_STATUS rateadp_block_type_set(UINT_8 chip_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || parameter > 3)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,parameter=%d\r\n",__FUNCTION__,chip_id,parameter);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,BLOCK_TYPE_REG[BLOCK_TYPE],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION rateadp_block_type_get
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
*	  *parameter:0:IDLE 1:LF 2/3:ERR_BLOCK
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
RET_STATUS rateadp_block_type_get(UINT_8 chip_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,BLOCK_TYPE_REG[BLOCK_TYPE],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
* 
*
*	  rateadp_clr_hist_set
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
* 	  field_id:0 :chid_lvl_hist;1:clear_hist
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
*    dingyi    	2018-4-11    1.0           initial
*
******************************************************************************/
RET_STATUS rateadp_clr_hist_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || parameter > 1)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,parameter=%d\r\n",__FUNCTION__,chip_id,parameter);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CLR_HIST_REG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,CLR_HIST_REG[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION 
*
* 	  rateadp_clr_hist_get    
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
*	  field_id:0 :chid_lvl_hist;1:clear_hist
*	  *parameter:get value
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
*    dingyi    	2018-4-11    1.0           initial
*
******************************************************************************/
RET_STATUS rateadp_clr_hist_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CLR_HIST_REG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,CLR_HIST_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
* 	  rateadp_lvl_hist_get    
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
*	  *parameter: get value
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
*    dingyi    	2018-4-11    1.0           initial
*
******************************************************************************/
RET_STATUS rateadp_lvl_hist_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(LVL_HIST_REG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,LVL_HIST_REG[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION 
*
*     rateadp_ind_sel_set
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
RET_STATUS adj_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 ch_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(ADJ_CFG_REG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,ADJ_CFG_REG[field_id].base_addr,ADJ_CFG_REG[field_id].offset_addr+ch_id*2,ADJ_CFG_REG[field_id].end_bit,
		ADJ_CFG_REG[field_id].start_bit,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     rateadp_ind_sel_get
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
RET_STATUS adj_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(ADJ_CFG_REG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,ADJ_CFG_REG[field_id].base_addr,ADJ_CFG_REG[field_id].offset_addr+ch_id*2,ADJ_CFG_REG[field_id].end_bit,
		ADJ_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
* 
*
*	  adj_cnt_alm_get
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
RET_STATUS adj_cnt_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (ch_id/32 > MAX_INDEX(CNT_ALM_REG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,CNT_ALM_REG[ch_id/32].base_addr,CNT_ALM_REG[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
* 
*
*	  adj_core_alm_get
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
RET_STATUS adj_core_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (ch_id/32 > MAX_INDEX(CORE_ALM_REG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,CORE_ALM_REG[ch_id/32].base_addr,CORE_ALM_REG[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION rateadp_block_num_set
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
*	  parameter:ts num
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
RET_STATUS rateadp_block_num_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,BLK_NUM_CFG[RATEADP_BLK_NUM].base_addr,BLK_NUM_CFG[RATEADP_BLK_NUM].offset_addr+ch_id,
		BLK_NUM_CFG[RATEADP_BLK_NUM].end_bit,BLK_NUM_CFG[RATEADP_BLK_NUM].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION rateadp_block_num_get
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
*	  *parameter:ts num
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
RET_STATUS rateadp_block_num_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,BLK_NUM_CFG[RATEADP_BLK_NUM].base_addr,BLK_NUM_CFG[RATEADP_BLK_NUM].offset_addr+ch_id,
		BLK_NUM_CFG[RATEADP_BLK_NUM].end_bit,BLK_NUM_CFG[RATEADP_BLK_NUM].start_bit,parameter);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION rateadp_chan_en_set
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
*	  parameter:0:disable,1:enable
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
RET_STATUS rateadp_chan_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (CHAN_EN > MAX_INDEX(BLK_NUM_CFG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,BLK_NUM_CFG[CHAN_EN].base_addr,BLK_NUM_CFG[CHAN_EN].offset_addr+ch_id,
		BLK_NUM_CFG[CHAN_EN].end_bit,BLK_NUM_CFG[CHAN_EN].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION rateadp_chan_en_get
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
*	  *parameter:0:disable,1:enable
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
RET_STATUS rateadp_chan_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (CHAN_EN > MAX_INDEX(BLK_NUM_CFG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,BLK_NUM_CFG[CHAN_EN].base_addr,BLK_NUM_CFG[CHAN_EN].offset_addr+ch_id,
		BLK_NUM_CFG[CHAN_EN].end_bit,BLK_NUM_CFG[CHAN_EN].start_bit,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION rateadp_map_reg_set
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
RET_STATUS rateadp_map_reg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32 parameter)
{
	CHIP_REG reg_to_write={field_id,MAP_REG[field_id].base_addr,MAP_REG[field_id].offset_addr+ch_id,MAP_REG[field_id].end_bit,MAP_REG[field_id].start_bit};

	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(MAP_REG))
	{
		if (rateadp_para_debug)
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
* FUNCTION rateadp_map_reg_get
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
*	  *parameter:get value
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
RET_STATUS rateadp_map_reg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32* parameter)
{
	CHIP_REG reg_to_read={field_id,MAP_REG[field_id].base_addr,MAP_REG[field_id].offset_addr+ch_id,MAP_REG[field_id].end_bit,MAP_REG[field_id].start_bit};

	if (chip_id > MAX_DEV || ch_id > (CH_NUM-1))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d,ch_id=%d\r\n",__FUNCTION__,chip_id,ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(MAP_REG))
	{
		if (rateadp_para_debug)
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
*     rateadp_ind_sel_set
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
RET_STATUS rateadp_ind_sel_set(UINT_8 chip_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,IND_SEL_REG[RATEADP_IND_SEL],parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     rateadp_ind_sel_get
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
RET_STATUS rateadp_ind_sel_get(UINT_8 chip_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,IND_SEL_REG[RATEADP_IND_SEL],parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     rateadp_ins_del_gap_set
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
RET_STATUS rateadp_ins_del_gap_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(INS_DEL_GAP_REG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,INS_DEL_GAP_REG[field_id],parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     rateadp_ins_del_gap_get
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
RET_STATUS rateadp_ins_del_gap_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (rateadp_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(INS_DEL_GAP_REG))
	{
		if (rateadp_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,INS_DEL_GAP_REG[field_id],parameter);

	return RET_SUCCESS;
}
