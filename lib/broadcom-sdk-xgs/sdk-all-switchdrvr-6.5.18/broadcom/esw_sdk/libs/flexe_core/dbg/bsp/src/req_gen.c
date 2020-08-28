
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int req_gen_para_debug = 0;

const CHIP_REG req_gen_m_reg[]=
{
	{REQ_GEN_M,REQ_GEN_BASE_ADDR,0x0,31,0},
};
const CHIP_REG base_m_reg[]=
{
	{BASE_M,REQ_GEN_BASE_ADDR,0x1,31,0},
};
const CHIP_REG req_gen_cfg_reg[]=
{
	{CALENDAR_EN,REQ_GEN_BASE_ADDR,0x2,0,0},
	{OHIF_1588_RATE_LIMIT_EN,REQ_GEN_BASE_ADDR,0x2,1,1},
};
const CHIP_REG ohif_1588_src_dst_reg[]=
{
	{EXTRA_SRC,REQ_GEN_BASE_ADDR,0x6,6,0},
	{EXTRA_DST0,REQ_GEN_BASE_ADDR,0x6,14,8},
	{EXTRA_DST1,REQ_GEN_BASE_ADDR,0x6,22,16},
};
const CHIP_REG ohif_1588_ratio_reg[]=
{
	{OHIF_1588_RATIO,REQ_GEN_BASE_ADDR,0x7,9,0},
};
const CHIP_REG ohif_1588_rate_limit_m_reg[]=
{
	{OHIF_1588_RATE_LIMIT_M,REQ_GEN_BASE_ADDR,0x8,31,0},
};
const CHIP_REG ohif_1588_rate_limit_base_m_reg[]=
{
	{OHIF_1588_RATE_LIMIT_BASE_M,REQ_GEN_BASE_ADDR,0x9,31,0},
};
const CHIP_REG ohif_1588_rate_limit_ratio_reg[]=
{
	{OHIF_1588_RATE_LIMIT_RATIO,REQ_GEN_BASE_ADDR,0xa,9,0},
};
const CHIP_REG extra_m_n_reg[]=
{
	{EXTRA_M,REQ_GEN_BASE_ADDR,0xb,15,0},
	{EXTRA_BASE_M,REQ_GEN_BASE_ADDR,0xb,31,16},
};
const CHIP_REG req_gen_pls_reg[]=
{
	{MAC_ENV_INIT,REQ_GEN_BASE_ADDR,0x3,0,0},
	{TABLE_SWITCH,REQ_GEN_BASE_ADDR,0x3,1,1},
};
const CHIP_REG req_gen_state_reg[]=
{
	{MAC_ENV_INIT_DONE,REQ_GEN_BASE_ADDR,0x4,0,0},
	{CURRENT_TABLE,REQ_GEN_BASE_ADDR,0x4,1,1},
	{BUSY,REQ_GEN_BASE_ADDR,0x4,2,2},
};
const CHIP_RAM req_gen_ram = 
{
	TABLE_BASE_ADDR,REQ_GEN_BASE_ADDR+0x5,7,
};

/******************************************************************************
*
* FUNCTION
*
*     req_gen_set
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
RET_STATUS req_gen_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0xffffffff))
	{
	   if (req_gen_para_debug)
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

	regp_field_write(chip_id,req_gen_m_reg[REQ_GEN_M],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_get
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
RET_STATUS req_gen_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,req_gen_m_reg[REQ_GEN_M],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_base_set
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
RET_STATUS req_gen_base_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0xffffffff))
	{
	   if (req_gen_para_debug)
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

	regp_field_write(chip_id,base_m_reg[BASE_M],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_base_get
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
RET_STATUS req_gen_base_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,base_m_reg[BASE_M],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_cfg_set
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
*	  field_id: 0:CALENDAR_EN  1:OHIF_1588_RATE_LIMIT_EN
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
RET_STATUS req_gen_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0x1))
	{
	   if (req_gen_para_debug)
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

	if (field_id > MAX_INDEX(req_gen_cfg_reg))
	{
		if (req_gen_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,req_gen_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_cfg_get
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
*	  field_id: 0:CALENDAR_EN  1:OHIF_1588_RATE_LIMIT_EN
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
RET_STATUS req_gen_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(req_gen_cfg_reg))
	{
		if (req_gen_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,req_gen_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_src_dst_set
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
*	  field_id: 0:EXTRA_SRC  1:EXTRA_DST0  2:EXTRA_DST1
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_src_dst_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0x7f))
	{
	   if (req_gen_para_debug)
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

	if (field_id > MAX_INDEX(ohif_1588_src_dst_reg))
	{
		if (req_gen_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,ohif_1588_src_dst_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_src_dst_get
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
*	  field_id: 0:EXTRA_SRC  1:EXTRA_DST0  2:EXTRA_DST1
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_src_dst_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(ohif_1588_src_dst_reg))
	{
		if (req_gen_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,ohif_1588_src_dst_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_ratio_set
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_ratio_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0x3ff))
	{
	   if (req_gen_para_debug)
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

	regp_field_write(chip_id,ohif_1588_ratio_reg[OHIF_1588_RATIO],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_ratio_get
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_ratio_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,ohif_1588_ratio_reg[OHIF_1588_RATIO],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_rate_limit_m_set
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_rate_limit_m_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0xffffffff))
	{
	   if (req_gen_para_debug)
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

	regp_field_write(chip_id,ohif_1588_rate_limit_m_reg[OHIF_1588_RATE_LIMIT_M],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_rate_limit_m_get
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_rate_limit_m_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,ohif_1588_rate_limit_m_reg[OHIF_1588_RATE_LIMIT_M],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_rate_limit_base_m_set
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_rate_limit_base_m_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0xffffffff))
	{
	   if (req_gen_para_debug)
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

	regp_field_write(chip_id,ohif_1588_rate_limit_base_m_reg[OHIF_1588_RATE_LIMIT_BASE_M],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_rate_limit_base_m_get
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_rate_limit_base_m_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,ohif_1588_rate_limit_base_m_reg[OHIF_1588_RATE_LIMIT_BASE_M],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_rate_limit_ratio_set
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_rate_limit_ratio_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0x3ff))
	{
	   if (req_gen_para_debug)
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

	regp_field_write(chip_id,ohif_1588_rate_limit_ratio_reg[OHIF_1588_RATE_LIMIT_RATIO],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ohif_1588_rate_limit_ratio_get
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
*    dingyi    2018-10-30    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_ohif_1588_rate_limit_ratio_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,ohif_1588_rate_limit_ratio_reg[OHIF_1588_RATE_LIMIT_RATIO],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_extra_m_n_set
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
*	  field_id: 0:EXTRA_M  1:EXTRA_BASE_M
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
*    dingyi    2018-11-21    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_extra_m_n_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(extra_m_n_reg))
	{
		if (req_gen_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,extra_m_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_extra_m_n_get
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
*	  field_id: 0:EXTRA_M  1:EXTRA_BASE_M
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
*    dingyi    2018-11-21    1.0           initial
*
******************************************************************************/
RET_STATUS req_gen_extra_m_n_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(extra_m_n_reg))
	{
		if (req_gen_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,extra_m_n_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_plus_set
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
*     field_id: 0: MAC_ENV_INIT  1:TABLE_SWITCH
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
RET_STATUS req_gen_plus_set(UINT_8 chip_id,UINT_8 field_id)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(req_gen_pls_reg))
	{
		if (req_gen_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_write(chip_id,req_gen_pls_reg[MAC_ENV_INIT].base_addr,req_gen_pls_reg[MAC_ENV_INIT].offset_addr,1 << field_id);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_state_get
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
*	  field_id: 0:MAC_ENV_INIT_DONE  1:CURRENT_TABLE  2:BUSY
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
RET_STATUS req_gen_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (req_gen_para_debug)
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
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(req_gen_state_reg))
	{
		if (req_gen_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,req_gen_state_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ram_set
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
*	  offset: depth 168
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
RET_STATUS req_gen_ram_set(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (offset > 161))
	{
	   if (req_gen_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n offset = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  offset);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	ram_write_single(chip_id,req_gen_ram,offset,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     req_gen_ram_get
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
*	  offset: depth 168
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
RET_STATUS req_gen_ram_get(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (offset > 161))
	{
	   if (req_gen_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n offset = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  offset);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (req_gen_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	ram_read_single(chip_id,req_gen_ram,offset,parameter);
	
	return RET_SUCCESS;
}
