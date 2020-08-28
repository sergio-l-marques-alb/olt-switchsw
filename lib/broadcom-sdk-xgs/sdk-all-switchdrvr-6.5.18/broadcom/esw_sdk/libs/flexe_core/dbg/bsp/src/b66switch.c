
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include "oh_rx.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int b66switch_para_debug = 0;

const CHIP_REG levelclr_sch_reg[]=
{
	{LEVELCLR_SCH,B66SWITCH_BASE_ADDR,0x40,0,0},
};

const CHIP_REG maxlevel_sch_reg[]=
{
	{MAXLEVEL_SCH_0,B66SWITCH_BASE_ADDR,0x41,31,0},
	{MAXLEVEL_SCH_1,B66SWITCH_BASE_ADDR,0x42,31,0},
	{MAXLEVEL_SCH_2,B66SWITCH_BASE_ADDR,0x43,31,0},
	{MAXLEVEL_SCH_3,B66SWITCH_BASE_ADDR,0x44,31,0},
	{MAXLEVEL_SCH_4,B66SWITCH_BASE_ADDR,0x45,31,0},
};

const CHIP_REG full_sch_reg[]=
{
	{FULL_SCH_0,B66SWITCH_BASE_ADDR,0x47,3,0},
	{FULL_SCH_1,B66SWITCH_BASE_ADDR,0x47,7,4},
	{FULL_SCH_2,B66SWITCH_BASE_ADDR,0x47,11,8},
	{FULL_SCH_3,B66SWITCH_BASE_ADDR,0x47,15,12},
	{FULL_SCH_4,B66SWITCH_BASE_ADDR,0x47,19,16},
};

const CHIP_REG protect_en_reg[]=
{
	{PROTECT_EN,B66SWITCH_BASE_ADDR,0x6,0,0},
};
const CHIP_REG chid_cfg_reg[]=
{
	{CHID_CFG,B66SWITCH_BASE_ADDR,0x7,31,0},
};
const CHIP_REG protect_info_reg[]=
{
	{PROTECT_INFO_0,B66SWITCH_BASE_ADDR,0x10,31,0},
	{PROTECT_INFO_1,B66SWITCH_BASE_ADDR,0x11,31,0},
	{PROTECT_INFO_2,B66SWITCH_BASE_ADDR,0x12,31,0},
	{PROTECT_INFO_3,B66SWITCH_BASE_ADDR,0x13,31,0},
	{PROTECT_INFO_4,B66SWITCH_BASE_ADDR,0x14,31,0},
	{PROTECT_INFO_5,B66SWITCH_BASE_ADDR,0x15,31,0},
	{PROTECT_INFO_6,B66SWITCH_BASE_ADDR,0x16,31,0},
	{PROTECT_INFO_7,B66SWITCH_BASE_ADDR,0x17,31,0},
	{PROTECT_INFO_8,B66SWITCH_BASE_ADDR,0x18,31,0},
	{PROTECT_INFO_9,B66SWITCH_BASE_ADDR,0x19,31,0},
	{PROTECT_INFO_10,B66SWITCH_BASE_ADDR,0x1a,31,0},
	{PROTECT_INFO_11,B66SWITCH_BASE_ADDR,0x1b,31,0},
	{PROTECT_INFO_12,B66SWITCH_BASE_ADDR,0x1c,31,0},
	{PROTECT_INFO_13,B66SWITCH_BASE_ADDR,0x1d,31,0},
	{PROTECT_INFO_14,B66SWITCH_BASE_ADDR,0x1e,31,0},
	{PROTECT_INFO_15,B66SWITCH_BASE_ADDR,0x1f,11,0},
};
const CHIP_RAM swram[] = 
{
	{TABLE_BASE_ADDR,B66SWITCH_BASE_ADDR+0x0,24},
	{TABLE_BASE_ADDR,B66SWITCH_BASE_ADDR+0x1,24},
	{TABLE_BASE_ADDR,B66SWITCH_BASE_ADDR+0x2,24},
};


/******************************************************************************
*
* FUNCTION
*
*     b66switch_levelclr_set
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
RET_STATUS b66switch_levelclr_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0x1))
	{
	   if (b66switch_para_debug)
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

	regp_field_write(chip_id,levelclr_sch_reg[LEVELCLR_SCH],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     b66switch_levelclr_get
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
RET_STATUS b66switch_levelclr_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66switch_para_debug)
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
        if (b66switch_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,levelclr_sch_reg[LEVELCLR_SCH],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     b66switch_maxlevel_get
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
*	  field_id:0~1 0:FLEXE schedule 1:MAC schedule
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
RET_STATUS b66switch_maxlevel_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (field_id > MAX_INDEX(maxlevel_sch_reg)))
	{
	   if (b66switch_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",
													  __FUNCTION__,
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (b66switch_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,maxlevel_sch_reg[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     b66switch_full_get
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
*	  field_id:0~1 0:FLEXE schedule 1:MAC schedule
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
RET_STATUS b66switch_full_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (field_id > MAX_INDEX(full_sch_reg)))
	{
	   if (b66switch_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d  \r\n",
													  __FUNCTION__,
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

    if (NULL == parameter)
    {
        if (b66switch_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,full_sch_reg[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     b66switch_swram_set
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
*	  channel_id: channel select 0~1
*	  offset:0~79
*	  ram_id:0~1 0:flexe direction 1:mac direction
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
*    dingyi    2018-1-3    1.0           initial
*
******************************************************************************/
RET_STATUS b66switch_swram_set(UINT_8 chip_id,UINT_8 channel_id,UINT_8 offset,UINT_8 ram_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
    UINT_32 rd_val[128] = {0};
	UINT_32 wr_val[128] = {0};
    
    if ((chip_id > MAX_DEV) || (channel_id > 2) || (ram_id > 2))
	{
		if (b66switch_para_debug)
		{
			printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n channel_id = %d, \
													  \r\n offset_id = %d, \
													  \r\n ram_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  channel_id,
													  offset,
                                                      ram_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66switch_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		return RET_PARAERR;
	}

	rv = ram_read_single(chip_id,swram[ram_id],offset,rd_val);
    
    if (RET_SUCCESS != rv)
    {
		if (b66switch_para_debug)
		{
			printf("[%s] table access failure\r\n",__FUNCTION__);
		}
		return rv;
    }
    
    switch(channel_id)
    {
        case 0:
            wr_val[0] = (rd_val[0] & 0xffffff00) | ((*parameter) << SHIFT_BIT_0);
            break;
        case 1:
            wr_val[0] = (rd_val[0] & 0xffff00ff) | ((*parameter) << SHIFT_BIT_8);
            break;
	case 2:
	     wr_val[0] = (rd_val[0] & 0xff00ffff) | ((*parameter & 0xff) << SHIFT_BIT_16);
	     break;
        default:
            return RET_PARAERR;
    }
		
	rv = ram_write_single(chip_id,swram[ram_id],offset,wr_val);

	return rv;
}


/******************************************************************************
*
* FUNCTION
*
*     b66switch_swram_get
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
*	  channel_id: channel select 0~1
*	  offset:0~79
*	  ram_id:0~1 0:FLEXE direction 1:mac direction
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
RET_STATUS b66switch_swram_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 offset,UINT_8 ram_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
    UINT_32 rd_val = 0;
    
    if ((chip_id > MAX_DEV) || (channel_id > 1) || (offset > (CH_NUM-1)) || (ram_id > 2))
	{
		if (b66switch_para_debug)
		{
			printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n channel_id = %d, \
													  \r\n offset_id = %d, \
													  \r\n ram_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  channel_id,
													  offset,
                                                      ram_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66switch_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		return RET_PARAERR;
	}

	rv = ram_read_single(chip_id,swram[ram_id],offset,&rd_val);
    
    if (RET_SUCCESS != rv)
    {
		if (b66switch_para_debug)
		{
			printf("[%s] table access failure\r\n",__FUNCTION__);
		}
		return rv;
    }
    
    switch(channel_id)
    {
        case 0:
            *parameter = (rd_val & 0x000000ff) >> SHIFT_BIT_0;
            break;
        case 1:
            *parameter = (rd_val & 0x0000ff00) >> SHIFT_BIT_8;
            break;
        default:
            return RET_PARAERR;
    }

	return rv;
}


/******************************************************************************
*
* FUNCTION
*
*     b66switch_protect_en_set
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
RET_STATUS b66switch_protect_en_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0x1))
	{
	   if (b66switch_para_debug)
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

	regp_field_write(chip_id,protect_en_reg[0],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     b66switch_protect_en_get
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
*	  field_id:0 
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
RET_STATUS b66switch_protect_en_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66switch_para_debug)
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
        if (b66switch_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,protect_en_reg[0],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION b66switch_chid_cfg_set
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
*	  ch_id:0~3 
*     parameter: 1588/oam oh belong to which channel
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
RET_STATUS b66switch_chid_cfg_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (ch_id > 3) || (parameter > 0xff))
	{
	   if (b66switch_para_debug)
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

  	regp_bit_write(chip_id,chid_cfg_reg[0].base_addr,chid_cfg_reg[0].offset_addr,ch_id*8+7,ch_id*8,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION b66switch_chid_cfg_get
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
*	  ch_id:0~3 
*     parameter: oam oh belong to which channel
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
RET_STATUS b66switch_chid_cfg_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (ch_id > 3))
	{
	   if (b66switch_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n ch_id = %d, \r\n",
													  __FUNCTION__,
													  chip_id,
													  ch_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66switch_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
  	regp_bit_read(chip_id,chid_cfg_reg[0].base_addr,chid_cfg_reg[0].offset_addr,ch_id*8+7,ch_id*8,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     b66switch_protect_info_get
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
*	  size: size of array
*     parameter: get value  parameter[16]
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
*    dingyi    2018-9-30    1.0           initial
*
******************************************************************************/
RET_STATUS b66switch_protect_info_get(UINT_8 chip_id,UINT_8 size,UINT_32* parameter)
{
	UINT_8 i = 0;
	
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (size > 16))
	{
	   if (b66switch_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d,\
													  \r\n size = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  size);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66switch_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	for (i = 0; i < size; i++)
	{
		regp_field_read(chip_id, protect_info_reg[i], &parameter[i]);
	}
		
	return RET_SUCCESS;
}
