

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int channelize_para_debug = 0;
unsigned long int g_channelize_para_debug = 0;



const CHIP_REG levelclr_channelize_reg[]=
{
	{LEVELCLR_CHANNELIZE,CHANNELIZE_BASE_ADDR,0x0,0,0},
};
const CHIP_REG bypass_en_reg[]=
{
	{BYPASS_EN,CHANNELIZE_BASE_ADDR,0x1,7,0},
	{B66REPLACE_EN,CHANNELIZE_BASE_ADDR,0x1,15,8},
	{B66REPLACE_PATTERN,CHANNELIZE_BASE_ADDR,0x1,31,16},
};
const CHIP_REG pcschid_cfg_reg[]=
{
	{PCSCHID_CFG_0,CHANNELIZE_BASE_ADDR,0x2,31,0},
	{PCSCHID_CFG_1,CHANNELIZE_BASE_ADDR,0x3,31,0},
};
const CHIP_REG maxlevel_channelize_reg[]=
{
	{MAXLEVEL_CHANNELIZE_0,CHANNELIZE_BASE_ADDR,0x4,31,0},
	{MAXLEVEL_CHANNELIZE_1,CHANNELIZE_BASE_ADDR,0x5,7,0},	
};
const CHIP_REG full_channelize_reg[]=
{
	{FULL_CHANNELIZE,CHANNELIZE_BASE_ADDR,0x6,4,0},
};

/******************************************************************************
*
* FUNCTION
*
*     channelize_levelclr_set
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
*    dingyi    2018-1-3    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_levelclr_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0x1))
	{
	   if (channelize_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n parameter = %d\r\n",
													  __FUNCTION__,
													  chip_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,levelclr_channelize_reg[LEVELCLR_CHANNELIZE],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_levelclr_get
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
*    dingyi    2018-1-3    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_levelclr_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (channelize_para_debug)
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
        if (channelize_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,levelclr_channelize_reg[LEVELCLR_CHANNELIZE],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_bypass_en_set
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
*	  channel_id:channel select 0~7 
*     parameter: value to set 0~1 0:FLEXE PHY 1:ETH
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
*    dingyi    2018-1-3    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_bypass_en_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV)|| (channel_id > 7) || (parameter > 0x1))
	{
	   if (channelize_para_debug)
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

	regp_bit_write(chip_id,bypass_en_reg[BYPASS_EN].base_addr,bypass_en_reg[BYPASS_EN].offset_addr,channel_id,channel_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_bypass_en_get
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
*	  channel_id:channel select 0~7 
*     parameter*: value to get 0~1 0:FLEXE PHY 1:ETH
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
*    dingyi    2018-1-3    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_bypass_en_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV)|| (channel_id > 7))
	{
	   if (channelize_para_debug)
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
        if (channelize_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,bypass_en_reg[BYPASS_EN].base_addr,
					bypass_en_reg[BYPASS_EN].offset_addr,channel_id,channel_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_b66replace_en_set
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
*	  channel_id:channel select 0~7 
*     parameter: value to set 0~1 
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
*    dingyi    2018-11-27    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_b66replace_en_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 7) || (parameter > 0x1))
	{
	   if (channelize_para_debug)
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

	if (B66REPLACE_EN > MAX_INDEX(bypass_en_reg))
	{
		if (g_channelize_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,bypass_en_reg[B66REPLACE_EN].base_addr,
					bypass_en_reg[B66REPLACE_EN].offset_addr,channel_id + 8,channel_id + 8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_b66replace_en_get
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
*	  channel_id:channel select 0~7 
*     parameter*: value to get 0~1 
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
*    dingyi    2018-11-27    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_b66replace_en_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 7))
	{
	   if (channelize_para_debug)
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
        if (channelize_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (B66REPLACE_EN > MAX_INDEX(bypass_en_reg))
	{
		if (channelize_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,bypass_en_reg[B66REPLACE_EN].base_addr,
					bypass_en_reg[B66REPLACE_EN].offset_addr,channel_id + 8,channel_id + 8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_b66replace_pattern_set
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
*	  channel_id:channel select 0~7 
*     parameter: value to set 0~3 
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
*    dingyi    2018-11-27    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_b66replace_pattern_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 7) || (parameter > 0x3))
	{
	   if (channelize_para_debug)
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

	if (B66REPLACE_PATTERN > MAX_INDEX(bypass_en_reg))
	{
		if (channelize_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,bypass_en_reg[B66REPLACE_PATTERN].base_addr,
					bypass_en_reg[B66REPLACE_PATTERN].offset_addr,channel_id * 2 + 17,channel_id * 2 + 16,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_b66replace_pattern_get
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
*	  channel_id:channel select 0~7 
*     parameter*: value to get 0~3 
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
*    dingyi    2018-11-27    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_b66replace_pattern_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 7))
	{
	   if (channelize_para_debug)
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
        if (channelize_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (B66REPLACE_PATTERN > MAX_INDEX(bypass_en_reg))
	{
		if (channelize_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,bypass_en_reg[B66REPLACE_PATTERN].base_addr,
					bypass_en_reg[B66REPLACE_PATTERN].offset_addr,channel_id * 2 + 17,channel_id * 2 + 16,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_pcschid_cfg_set
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
*	  channel_id:channel select 0~7 
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
*    dingyi    2018-1-3    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_pcschid_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV)|| (channel_id > 7) || (parameter > 0xff))
	{
	   if (channelize_para_debug)
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

	if (channel_id/4 > MAX_INDEX(pcschid_cfg_reg))
	{
		if (channelize_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,pcschid_cfg_reg[channel_id/4].base_addr,pcschid_cfg_reg[channel_id/4].offset_addr,(channel_id%4)*8+7,(channel_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_pcschid_cfg_get
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
*	  channel_id:channel select 0~7 
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
*    dingyi    2018-1-3    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_pcschid_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV)|| (channel_id > 7))
	{
	   if (channelize_para_debug)
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
        if (channelize_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (channel_id/4 > MAX_INDEX(pcschid_cfg_reg))
	{
		if (channelize_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,pcschid_cfg_reg[channel_id/4].base_addr,pcschid_cfg_reg[channel_id/4].offset_addr,(channel_id%4)*8+7,(channel_id%4)*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_maxlevel_get
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
*	  channel_id:channel select 0~3 
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
*    dingyi    2018-1-3    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_maxlevel_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV)|| (channel_id > 4))
	{
	   if (channelize_para_debug)
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
        if (channelize_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (MAXLEVEL_CHANNELIZE_1 > MAX_INDEX(maxlevel_channelize_reg))
	{
		if (channelize_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (channel_id < 4)
	{
		regp_bit_read(chip_id,maxlevel_channelize_reg[MAXLEVEL_CHANNELIZE_0].base_addr,maxlevel_channelize_reg[MAXLEVEL_CHANNELIZE_0].offset_addr,channel_id*8+7,channel_id*8,parameter);
	}
	else if (4 == channel_id)
	{
		regp_bit_read(chip_id,maxlevel_channelize_reg[MAXLEVEL_CHANNELIZE_1].base_addr,maxlevel_channelize_reg[MAXLEVEL_CHANNELIZE_1].offset_addr,7,0,parameter);
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     channelize_full_get
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
*	  channel_id:channel select 0~3 
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
*    dingyi    2018-1-3    1.0           initial
*
******************************************************************************/
RET_STATUS channelize_full_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV)|| (channel_id > 3))
	{
	   if (channelize_para_debug)
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
        if (channelize_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,full_channelize_reg[FULL_CHANNELIZE].base_addr,full_channelize_reg[FULL_CHANNELIZE].offset_addr,channel_id,channel_id,parameter);
	
	return RET_SUCCESS;
}
