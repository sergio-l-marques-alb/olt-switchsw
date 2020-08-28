
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int flexe_env_para_debug = 0;

const CHIP_REG flexe_env_cfg1_reg[]=
{
	{CFG_PERIOD,FLEXE_ENV_BASE_ADDR,0x0,14,0},
	{MULTIPLIER,FLEXE_ENV_BASE_ADDR,0x0,15,15},
	{NOM_CNT,FLEXE_ENV_BASE_ADDR,0x0,30,16},
};
const CHIP_REG flexe_env_cfg2_reg[]=
{
	{FLEXE_ENV_Q,FLEXE_ENV_BASE_ADDR,0x8,14,0},
	{ADJ_01EN,FLEXE_ENV_BASE_ADDR,0x8,15,15},
	{FIFO_ADJ_EN,FLEXE_ENV_BASE_ADDR,0x8,16,16},
	{FIFO_ADJ_PERIOD,FLEXE_ENV_BASE_ADDR,0x8,24,17},
	{FLEXE_ENV_EN,FLEXE_ENV_BASE_ADDR,0x8,25,25},
	{FIFO_ADJ_MAX_EN,FLEXE_ENV_BASE_ADDR,0x8,26,26},
	{IS_LOCAL,FLEXE_ENV_BASE_ADDR,0x8,27,27},
};
const CHIP_REG flexe_env_cfg3_reg[]=
{
	{ADJ_IN_CHECK_VALUE,FLEXE_ENV_BASE_ADDR,0x10,9,0},
	{ADJ_IN_CORRECT_EN,FLEXE_ENV_BASE_ADDR,0x10,10,10},
	{FLEXE_LOCAL_EN,FLEXE_ENV_BASE_ADDR,0x10,11,11},
	{FLEXE_LOCAL_M,FLEXE_ENV_BASE_ADDR,0x10,21,12},
};
const CHIP_REG m_cfg_reg[]=
{
	{FLEXE_ENV_M,FLEXE_ENV_BASE_ADDR,0x18,31,0},
};
const CHIP_REG base_m_cfg_reg[]=
{
	{FLEXE_ENV_BASE_M,FLEXE_ENV_BASE_ADDR,0x20,31,0},
};
const CHIP_REG adj_in_alm_reg[]=
{
	{FLEXE_ADJ_IN_ALM,FLEXE_ENV_BASE_ADDR,0x28,7,0},
};
const CHIP_REG integral_alm_reg[]=
{
	{FLEXE_I1_OVERFLOW,FLEXE_ENV_BASE_ADDR,0x29,7,0},
	{FLEXE_I1_UNDERFLOW,FLEXE_ENV_BASE_ADDR,0x29,15,8},
	{FLEXE_I2_OVERFLOW,FLEXE_ENV_BASE_ADDR,0x29,23,16},
	{FLEXE_I2_UNDERFLOW,FLEXE_ENV_BASE_ADDR,0x29,31,24},
};

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_cfg1_set
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
*	  field_id: 0:CFG_PERIOD  1:MULTIPLIER  2:NOM_CNT
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_cfg1_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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

	if (field_id > MAX_INDEX(flexe_env_cfg1_reg))
	{
		if (flexe_env_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,flexe_env_cfg1_reg[field_id].base_addr,flexe_env_cfg1_reg[field_id].offset_addr+grp_id,
		flexe_env_cfg1_reg[field_id].end_bit,flexe_env_cfg1_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_cfg1_get
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
*	  field_id: 0:CFG_PERIOD  1:MULTIPLIER  2:NOM_CNT
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_cfg1_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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
        if (flexe_env_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(flexe_env_cfg1_reg))
	{
		if (flexe_env_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,flexe_env_cfg1_reg[field_id].base_addr,flexe_env_cfg1_reg[field_id].offset_addr+grp_id,
		flexe_env_cfg1_reg[field_id].end_bit,flexe_env_cfg1_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_cfg2_set
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
*	  field_id: 0:FLEXE_ENV_Q  1:ADJ_01EN  2:FIFO_ADJ_EN  3:FIFO_ADJ_PERIOD
*				4:FLEXE_ENV_EN  5:FIFO_ADJ_MAX_EN  6:IS_LOCAL
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_cfg2_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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

	if (field_id > MAX_INDEX(flexe_env_cfg2_reg))
	{
		if (flexe_env_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,flexe_env_cfg2_reg[field_id].base_addr,flexe_env_cfg2_reg[field_id].offset_addr+grp_id,
		flexe_env_cfg2_reg[field_id].end_bit,flexe_env_cfg2_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_cfg2_get
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
*	  field_id: 0:FLEXE_ENV_Q  1:ADJ_01EN  2:FIFO_ADJ_EN  3:FIFO_ADJ_PERIOD
*				4:FLEXE_ENV_EN  5:FIFO_ADJ_MAX_EN  6:IS_LOCAL
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_cfg2_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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
        if (flexe_env_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(flexe_env_cfg2_reg))
	{
		if (flexe_env_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,flexe_env_cfg2_reg[field_id].base_addr,flexe_env_cfg2_reg[field_id].offset_addr+grp_id,
		flexe_env_cfg2_reg[field_id].end_bit,flexe_env_cfg2_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_cfg3_set
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
*	  field_id: 0:ADJ_IN_CHECK_VALUE  1:ADJ_IN_CORRECT_EN  2:FLEXE_LOCAL_EN  3:FLEXE_LOCAL_M
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_cfg3_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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

	if (field_id > MAX_INDEX(flexe_env_cfg3_reg))
	{
		if (flexe_env_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,flexe_env_cfg3_reg[field_id].base_addr,flexe_env_cfg3_reg[field_id].offset_addr+grp_id,
		flexe_env_cfg3_reg[field_id].end_bit,flexe_env_cfg3_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_cfg3_get
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
*	  field_id: 0:ADJ_IN_CHECK_VALUE  1:ADJ_IN_CORRECT_EN  2:FLEXE_LOCAL_EN  3:FLEXE_LOCAL_M
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_cfg3_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 grp_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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
        if (flexe_env_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(flexe_env_cfg3_reg))
	{
		if (flexe_env_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,flexe_env_cfg3_reg[field_id].base_addr,flexe_env_cfg3_reg[field_id].offset_addr+grp_id,
		flexe_env_cfg3_reg[field_id].end_bit,flexe_env_cfg3_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_m_cfg_set
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
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_m_cfg_set(UINT_8 chip_id,UINT_8 grp_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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

	regp_bit_write(chip_id,m_cfg_reg[FLEXE_ENV_M].base_addr,m_cfg_reg[FLEXE_ENV_M].offset_addr+grp_id,
		m_cfg_reg[FLEXE_ENV_M].end_bit,m_cfg_reg[FLEXE_ENV_M].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_m_cfg_get
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
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_m_cfg_get(UINT_8 chip_id,UINT_8 grp_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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
        if (flexe_env_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,m_cfg_reg[FLEXE_ENV_M].base_addr,m_cfg_reg[FLEXE_ENV_M].offset_addr+grp_id,
		m_cfg_reg[FLEXE_ENV_M].end_bit,m_cfg_reg[FLEXE_ENV_M].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_base_m_cfg_set
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
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_base_m_cfg_set(UINT_8 chip_id,UINT_8 grp_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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

	regp_bit_write(chip_id,base_m_cfg_reg[FLEXE_ENV_BASE_M].base_addr,base_m_cfg_reg[FLEXE_ENV_BASE_M].offset_addr+grp_id,
		base_m_cfg_reg[FLEXE_ENV_BASE_M].end_bit,base_m_cfg_reg[FLEXE_ENV_BASE_M].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_base_m_cfg_get
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
*	  grp_id: grp select 0~7 
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
RET_STATUS flexe_env_base_m_cfg_get(UINT_8 chip_id,UINT_8 grp_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > 7))
	{
	   if (flexe_env_para_debug)
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
        if (flexe_env_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,base_m_cfg_reg[FLEXE_ENV_BASE_M].base_addr,base_m_cfg_reg[FLEXE_ENV_BASE_M].offset_addr+grp_id,
		base_m_cfg_reg[FLEXE_ENV_BASE_M].end_bit,base_m_cfg_reg[FLEXE_ENV_BASE_M].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     flexe_env_adj_in_alm_get
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
RET_STATUS flexe_env_adj_in_alm_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (flexe_env_para_debug)
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
        if (flexe_env_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,adj_in_alm_reg[FLEXE_ADJ_IN_ALM],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_env_integral_alm_get
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
*	  field_id: 0:FLEXE_I1_OVERFLOW  1:FLEXE_I1_UNDERFLOW  2:FLEXE_I2_OVERFLOW  3:FLEXE_I2_UNDERFLOW
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
RET_STATUS flexe_env_integral_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (flexe_env_para_debug)
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
        if (flexe_env_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(integral_alm_reg))
	{
		if (flexe_env_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,integral_alm_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

