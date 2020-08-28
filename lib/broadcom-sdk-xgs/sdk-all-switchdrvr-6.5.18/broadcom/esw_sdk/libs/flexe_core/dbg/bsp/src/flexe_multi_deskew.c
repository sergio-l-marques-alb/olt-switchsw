

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int flexe_multi_deskew_para_debug = 0;

const CHIP_REG FLEXE_INSTANCE_GRP_CFG_0_REG[]=
{
	{GRP_SORT_CFG0,FLEXE_MULTI_DESKEW_BASE_ADDR,0x0,7,0},
	{GRP_SORT_CFG1,FLEXE_MULTI_DESKEW_BASE_ADDR,0x0,15,8},
	{GRP_SORT_CFG2,FLEXE_MULTI_DESKEW_BASE_ADDR,0x0,23,16},
	{GRP_SORT_CFG3,FLEXE_MULTI_DESKEW_BASE_ADDR,0x0,31,24},
};
const CHIP_REG FLEXE_INSTANCE_GRP_CFG_1_REG[]=
{
	{GRP_SORT_CFG4,FLEXE_MULTI_DESKEW_BASE_ADDR,0x1,7,0},
	{GRP_SORT_CFG5,FLEXE_MULTI_DESKEW_BASE_ADDR,0x1,15,8},
	{GRP_SORT_CFG6,FLEXE_MULTI_DESKEW_BASE_ADDR,0x1,23,16},
	{GRP_SORT_CFG7,FLEXE_MULTI_DESKEW_BASE_ADDR,0x1,31,24},
};
const CHIP_REG FLEXE_DSW_MARGIN_CFG_0_REG[]=
{
	{DSW_MARGIN_CFG0,FLEXE_MULTI_DESKEW_BASE_ADDR,0x2,11,0},
	{DSW_MARGIN_CFG1,FLEXE_MULTI_DESKEW_BASE_ADDR,0x2,23,12},
};
const CHIP_REG FLEXE_DSW_MARGIN_CFG_1_REG[]=
{
	{DSW_MARGIN_CFG2,FLEXE_MULTI_DESKEW_BASE_ADDR,0x3,11,0},
	{DSW_MARGIN_CFG3,FLEXE_MULTI_DESKEW_BASE_ADDR,0x3,23,12},
};
const CHIP_REG FLEXE_DSW_MARGIN_CFG_2_REG[]=
{
	{DSW_MARGIN_CFG4,FLEXE_MULTI_DESKEW_BASE_ADDR,0x4,11,0},
	{DSW_MARGIN_CFG5,FLEXE_MULTI_DESKEW_BASE_ADDR,0x4,23,12},
};
const CHIP_REG FLEXE_DSW_MARGIN_CFG_3_REG[]=
{
	{DSW_MARGIN_CFG6,FLEXE_MULTI_DESKEW_BASE_ADDR,0x5,11,0},
	{DSW_MARGIN_CFG7,FLEXE_MULTI_DESKEW_BASE_ADDR,0x5,23,12},
};
const CHIP_REG FLEXE_GRP_SSF_CFG_REG[]=
{
	{GRP_SSF_EN,FLEXE_MULTI_DESKEW_BASE_ADDR,0x6,7,0},
	{PHY_SSF_EN,FLEXE_MULTI_DESKEW_BASE_ADDR,0x6,31,24},
};
const CHIP_REG FLEXE_GRP_INI_CTL_REG[]=
{
	{GRP_INI_SEL,FLEXE_MULTI_DESKEW_BASE_ADDR,0x7,7,0},
	{GRP_INI_CFG,FLEXE_MULTI_DESKEW_BASE_ADDR,0x7,15,8},
	{FIFO_ADDR_OFFSET,FLEXE_MULTI_DESKEW_BASE_ADDR,0x7,25,16},
};
const CHIP_REG FlEXE_GRP_MODE_REG[]=
{
	{GRP_MODE,FLEXE_MULTI_DESKEW_BASE_ADDR,0x8,7,0},
	{BYPASS_50G,FLEXE_MULTI_DESKEW_BASE_ADDR,0x8,8,8},
	{BYPASS_100G,FLEXE_MULTI_DESKEW_BASE_ADDR,0x8,9,9},
};
const CHIP_REG DESKEW_RATE_M_50G_REG[]=
{
	{DESKEW_RATE_M_50G,FLEXE_MULTI_DESKEW_BASE_ADDR,0xa,31,0},
};
const CHIP_REG DESKEW_RATE_BASE_M_50G_REG[]=
{
	{DESKEW_RATE_BASE_M_50G,FLEXE_MULTI_DESKEW_BASE_ADDR,0xb,31,0},
};
const CHIP_REG DESKEW_RATE_M_100G_REG[]=
{
	{DESKEW_RATE_M_100G,FLEXE_MULTI_DESKEW_BASE_ADDR,0xc,31,0},
};
const CHIP_REG DESKEW_RATE_BASE_M_100G_REG[]=
{
	{DESKEW_RATE_BASE_M_100G,FLEXE_MULTI_DESKEW_BASE_ADDR,0xd,31,0},
};
const CHIP_REG FLEXE_GRP_DSW_EN_REG[]=
{
	{DESKEW_ALM_EN0,FLEXE_MULTI_DESKEW_BASE_ADDR,0xe,7,0},
	{DESKEW_ALM_EN1,FLEXE_MULTI_DESKEW_BASE_ADDR,0xe,15,8},
	{DESKEW_ALM_EN2,FLEXE_MULTI_DESKEW_BASE_ADDR,0xe,23,16},
};
const CHIP_REG FLEXE_DSW_MEASURE_0_REG[]=
{
	{DSW_MEASURE_GRP0,FLEXE_MULTI_DESKEW_BASE_ADDR,0x10,11,0},
	{DSW_MEASURE_GRP1,FLEXE_MULTI_DESKEW_BASE_ADDR,0x10,23,12},
};
const CHIP_REG FLEXE_DSW_MEASURE_1_REG[]=
{
	{DSW_MEASURE_GRP2,FLEXE_MULTI_DESKEW_BASE_ADDR,0x11,11,0},
	{DSW_MEASURE_GRP3,FLEXE_MULTI_DESKEW_BASE_ADDR,0x11,23,12},
};
const CHIP_REG FLEXE_DSW_MEASURE_2_REG[]=
{
	{DSW_MEASURE_GRP4,FLEXE_MULTI_DESKEW_BASE_ADDR,0x12,11,0},
	{DSW_MEASURE_GRP5,FLEXE_MULTI_DESKEW_BASE_ADDR,0x12,23,12},
};
const CHIP_REG FLEXE_DSW_MEASURE_3_REG[]=
{
	{DSW_MEASURE_GRP6,FLEXE_MULTI_DESKEW_BASE_ADDR,0x13,11,0},
	{DSW_MEASURE_GRP7,FLEXE_MULTI_DESKEW_BASE_ADDR,0x13,23,12},
};
const CHIP_REG FLEXE_DSW_ALM_REG[]=
{
	{GRP_DSW_FIFO_EMPTY,FLEXE_MULTI_DESKEW_BASE_ADDR,0x15,7,0},
	{GRP_DSW_FIFO_FULL,FLEXE_MULTI_DESKEW_BASE_ADDR,0x15,15,8},
	{GRP_DSW_OVERMARGIN,FLEXE_MULTI_DESKEW_BASE_ADDR,0x15,23,16},
	{GRP_DSW_ALM,FLEXE_MULTI_DESKEW_BASE_ADDR,0x15,31,24},
};
const CHIP_REG FLEXE_GRP_SSF_ALM_REG[]=
{
	{GRP_SSF_ALM,FLEXE_MULTI_DESKEW_BASE_ADDR,0x16,7,0},
	{PHY200G_INST_NUM_ALM,FLEXE_MULTI_DESKEW_BASE_ADDR,0x16,11,8},
	{PHY400G_INST_NUM_ALM,FLEXE_MULTI_DESKEW_BASE_ADDR,0x16,12,12},
};

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_grp_cfg_set
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
*	  group_id:0~7
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
*    dingyi    2017-12-26    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_grp_cfg_set(UINT_8 chip_id,UINT_8 group_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (group_id > (MAX_GROUP-1)) || (parameter > 0xff))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n group_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  group_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (group_id < 4)
	{
		regp_bit_write(chip_id,FLEXE_INSTANCE_GRP_CFG_0_REG[group_id].base_addr,FLEXE_INSTANCE_GRP_CFG_0_REG[group_id].offset_addr,
			FLEXE_INSTANCE_GRP_CFG_0_REG[group_id].end_bit,FLEXE_INSTANCE_GRP_CFG_0_REG[group_id].start_bit,parameter);
	}
	else
	{
		regp_bit_write(chip_id,FLEXE_INSTANCE_GRP_CFG_1_REG[group_id-4].base_addr,FLEXE_INSTANCE_GRP_CFG_1_REG[group_id-4].offset_addr,
			FLEXE_INSTANCE_GRP_CFG_1_REG[group_id-4].end_bit,FLEXE_INSTANCE_GRP_CFG_1_REG[group_id-4].start_bit,parameter);
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_grp_cfg_get
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
*	  group_id:0~7
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
*    dingyi    2017-12-26    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_grp_cfg_get(UINT_8 chip_id,UINT_8 group_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV))
	{
	   if (flexe_multi_deskew_para_debug)
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
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (group_id < 4)
	{
		regp_bit_read(chip_id,FLEXE_INSTANCE_GRP_CFG_0_REG[group_id].base_addr,FLEXE_INSTANCE_GRP_CFG_0_REG[group_id].offset_addr,
			FLEXE_INSTANCE_GRP_CFG_0_REG[group_id].end_bit,FLEXE_INSTANCE_GRP_CFG_0_REG[group_id].start_bit,parameter);
	}
	else
	{
		regp_bit_read(chip_id,FLEXE_INSTANCE_GRP_CFG_1_REG[group_id-4].base_addr,FLEXE_INSTANCE_GRP_CFG_1_REG[group_id-4].offset_addr,
			FLEXE_INSTANCE_GRP_CFG_1_REG[group_id-4].end_bit,FLEXE_INSTANCE_GRP_CFG_1_REG[group_id-4].start_bit,parameter);
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_dsw_margin_cfg_set
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
*	  grp_id:0~3
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
*    dingyi    2017-12-26    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_dsw_margin_cfg_set(UINT_8 chip_id,UINT_8 grp_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (grp_id > (MAX_GROUP-1)) || (parameter > 0xfff))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n grp_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  grp_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	switch(grp_id)
	{
		case 0:
		case 1:
			if (grp_id > MAX_INDEX(FLEXE_DSW_MARGIN_CFG_0_REG))
			{
				if (flexe_multi_deskew_para_debug)
				{
					printf("[%s] out of array range\r\n",__FUNCTION__);
				}

				return RET_PARAERR;
			}

			regp_field_write(chip_id,FLEXE_DSW_MARGIN_CFG_0_REG[grp_id],parameter);
			break;
		case 2:
		case 3:
			if ((grp_id-2) > MAX_INDEX(FLEXE_DSW_MARGIN_CFG_1_REG))
			{
				if (flexe_multi_deskew_para_debug)
				{
					printf("[%s] out of array range\r\n",__FUNCTION__);
				}

				return RET_PARAERR;
			}

			regp_field_write(chip_id,FLEXE_DSW_MARGIN_CFG_1_REG[grp_id-2],parameter);
			break;
		case 4:
		case 5:
			if ((grp_id-4) > MAX_INDEX(FLEXE_DSW_MARGIN_CFG_2_REG))
			{
				if (flexe_multi_deskew_para_debug)
				{
					printf("[%s] out of array range\r\n",__FUNCTION__);
				}

				return RET_PARAERR;
			}

			regp_field_write(chip_id,FLEXE_DSW_MARGIN_CFG_2_REG[grp_id-4],parameter);
			break;
		case 6:
		case 7:
			if ((grp_id-6) > MAX_INDEX(FLEXE_DSW_MARGIN_CFG_3_REG))
			{
				if (flexe_multi_deskew_para_debug)
				{
					printf("[%s] out of array range\r\n",__FUNCTION__);
				}

				return RET_PARAERR;
			}

			regp_field_write(chip_id,FLEXE_DSW_MARGIN_CFG_3_REG[grp_id-6],parameter);
			break;
		default:
			return RET_PARAERR;
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_dsw_margin_cfg_get
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
*	  grp_id:0~3
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
*    dingyi    2017-12-26    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_dsw_margin_cfg_get(UINT_8 chip_id,UINT_8 grp_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV))
    {
        if (flexe_multi_deskew_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n grp_id = %d   \r\n",
                    __FUNCTION__,
                    chip_id,
                    grp_id);
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_multi_deskew_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    switch(grp_id)
    {
        case 0:
        case 1:
            if (grp_id > MAX_INDEX(FLEXE_DSW_MARGIN_CFG_0_REG))
            {
                if (flexe_multi_deskew_para_debug)
                {
                    printf("[%s] out of array range\r\n",__FUNCTION__);
                }
                return RET_PARAERR;
            }

            regp_field_read(chip_id,FLEXE_DSW_MARGIN_CFG_0_REG[grp_id],parameter);
            break;
        case 2:
        case 3:
            if ((grp_id-2) > MAX_INDEX(FLEXE_DSW_MARGIN_CFG_1_REG))
            {
                if (flexe_multi_deskew_para_debug)
                {
                    printf("[%s] out of array range\r\n",__FUNCTION__);
                }
                return RET_PARAERR;
            }
    
            regp_field_read(chip_id,FLEXE_DSW_MARGIN_CFG_1_REG[grp_id-2],parameter);
            break;
        case 4:
        case 5:
            if ((grp_id-4) > MAX_INDEX(FLEXE_DSW_MARGIN_CFG_2_REG))
            {
                if (flexe_multi_deskew_para_debug)
                {
                    printf("[%s] out of array range\r\n",__FUNCTION__);
                }
                return RET_PARAERR;
            }
            regp_field_read(chip_id,FLEXE_DSW_MARGIN_CFG_2_REG[grp_id-4],parameter);
            break;
        case 6:
        case 7:
            if ((grp_id-6) > MAX_INDEX(FLEXE_DSW_MARGIN_CFG_3_REG))
            {
                if (flexe_multi_deskew_para_debug)
                {
                    printf("[%s] out of array range\r\n",__FUNCTION__);
                }
                return RET_PARAERR;
            }
            regp_field_read(chip_id,FLEXE_DSW_MARGIN_CFG_3_REG[grp_id-6],parameter);
            break;
        default:
            return RET_PARAERR;
    }

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_ssf_cfg_set
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
*	  group_id:0~7
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
*    dingyi    2017-12-26    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_ssf_cfg_set(UINT_8 chip_id,UINT_8 group_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (group_id > (MAX_GROUP-1)) || (parameter > 1))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n group_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  group_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,FLEXE_GRP_SSF_CFG_REG[GRP_SSF_EN].base_addr,FLEXE_GRP_SSF_CFG_REG[GRP_SSF_EN].offset_addr,
		group_id,group_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_ssf_cfg_get
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
*	  group_id:0~7
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
*    dingyi    2017-12-26    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_ssf_cfg_get(UINT_8 chip_id,UINT_8 group_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (group_id > (MAX_GROUP-1)))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n group_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  group_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,FLEXE_GRP_SSF_CFG_REG[GRP_SSF_EN].base_addr,FLEXE_GRP_SSF_CFG_REG[GRP_SSF_EN].offset_addr,
		group_id,group_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_phy_ssf_en_set
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
*	  group_id:0~7
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_phy_ssf_en_set(UINT_8 chip_id,UINT_8 group_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (group_id > (MAX_GROUP-1)) || (parameter > 1))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n group_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  group_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (PHY_SSF_EN > MAX_INDEX(FLEXE_GRP_SSF_CFG_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,FLEXE_GRP_SSF_CFG_REG[PHY_SSF_EN].base_addr,FLEXE_GRP_SSF_CFG_REG[PHY_SSF_EN].offset_addr,
		group_id+24,group_id+24,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_phy_ssf_en_get
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
*	  group_id:0~7
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_phy_ssf_en_get(UINT_8 chip_id,UINT_8 group_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (group_id > (MAX_GROUP-1)))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n group_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  group_id);
	   }

	   return RET_PARAERR;
	}

	if (PHY_SSF_EN > MAX_INDEX(FLEXE_GRP_SSF_CFG_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,FLEXE_GRP_SSF_CFG_REG[PHY_SSF_EN].base_addr,FLEXE_GRP_SSF_CFG_REG[PHY_SSF_EN].offset_addr,
		group_id+24,group_id+24,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_grp_ini_ctl_set
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
*	  field_id:0~1 0:GRP_INI_SEL  1:GRP_INI_CFG
*	  group_id:0~7
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_grp_ini_ctl_set(UINT_8 chip_id,UINT_8 field_id,UINT_8 group_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (group_id > (MAX_GROUP-1)) || (parameter > 1))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n group_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  group_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_GRP_INI_CTL_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,FLEXE_GRP_INI_CTL_REG[field_id].base_addr,FLEXE_GRP_INI_CTL_REG[field_id].offset_addr,
		FLEXE_GRP_INI_CTL_REG[field_id].start_bit+group_id,FLEXE_GRP_INI_CTL_REG[field_id].start_bit+group_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_grp_ini_ctl_get
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
*	  field_id:0~1 0:GRP_INI_SEL  1:GRP_INI_CFG
*	  group_id:0~7
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_grp_ini_ctl_get(UINT_8 chip_id,UINT_8 field_id,UINT_8 group_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (group_id > (MAX_GROUP-1)))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n group_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  group_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_GRP_INI_CTL_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,FLEXE_GRP_INI_CTL_REG[field_id].base_addr,FLEXE_GRP_INI_CTL_REG[field_id].offset_addr,
		FLEXE_GRP_INI_CTL_REG[field_id].start_bit+group_id,FLEXE_GRP_INI_CTL_REG[field_id].start_bit+group_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_fifo_addr_offset_set
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_fifo_addr_offset_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 0x3ff))
	{
	   if (flexe_multi_deskew_para_debug)
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

	if (FIFO_ADDR_OFFSET > MAX_INDEX(FLEXE_GRP_INI_CTL_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,FLEXE_GRP_INI_CTL_REG[FIFO_ADDR_OFFSET],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_fifo_addr_offset_get
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_fifo_addr_offset_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (flexe_multi_deskew_para_debug)
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
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (FIFO_ADDR_OFFSET > MAX_INDEX(FLEXE_GRP_INI_CTL_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,FLEXE_GRP_INI_CTL_REG[FIFO_ADDR_OFFSET],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_grp_mode_set
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
*	  group_id:0~7
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_grp_mode_set(UINT_8 chip_id,UINT_8 group_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (group_id > (MAX_GROUP-1)) || (parameter > 1))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n group_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  group_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,FlEXE_GRP_MODE_REG[GRP_MODE].base_addr,FlEXE_GRP_MODE_REG[GRP_MODE].offset_addr,
		group_id,group_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_grp_mode_get
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
*	  group_id:0~7
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_grp_mode_get(UINT_8 chip_id,UINT_8 group_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (group_id > (MAX_GROUP-1)))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n group_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  group_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,FlEXE_GRP_MODE_REG[GRP_MODE].base_addr,FlEXE_GRP_MODE_REG[GRP_MODE].offset_addr,
		group_id,group_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_bypass_50g_set
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_bypass_50g_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (flexe_multi_deskew_para_debug)
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

	if (BYPASS_50G > MAX_INDEX(FlEXE_GRP_MODE_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,FlEXE_GRP_MODE_REG[BYPASS_50G],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_bypass_50g_get
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_bypass_50g_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (flexe_multi_deskew_para_debug)
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
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (BYPASS_50G > MAX_INDEX(FlEXE_GRP_MODE_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,FlEXE_GRP_MODE_REG[BYPASS_50G],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_bypass_100g_set
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_bypass_100g_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (flexe_multi_deskew_para_debug)
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

	if (BYPASS_100G > MAX_INDEX(FlEXE_GRP_MODE_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,FlEXE_GRP_MODE_REG[BYPASS_100G],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_bypass_100g_get
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_bypass_100g_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (flexe_multi_deskew_para_debug)
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
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (BYPASS_100G > MAX_INDEX(FlEXE_GRP_MODE_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,FlEXE_GRP_MODE_REG[BYPASS_100G],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_grp_dsw_en_set
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
*	  field_id: 0:demux 1:OAM rdi  2:local fault
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
RET_STATUS flexe_multi_deskew_grp_dsw_en_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_GRP_DSW_EN_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,FLEXE_GRP_DSW_EN_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_grp_dsw_en_get
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
*	  field_id: 0:demux 1:OAM rdi  2:local fault
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
*    dingyi    2018-11-10    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_grp_dsw_en_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (flexe_multi_deskew_para_debug)
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
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_GRP_DSW_EN_REG))
	{
		if (flexe_multi_deskew_para_debug)
	    {
	    	printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,FLEXE_GRP_DSW_EN_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_rate_set
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
*	  field_id: 0~3 0:DESKEW_RATE_M_50G_REG   1:DESKEW_RATE_BASE_M_50G_REG
*					2:DESKEW_RATE_M_100G_REG  3:DESKEW_RATE_BASE_M_100G_REG
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_rate_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (field_id > 3)|| (parameter > 0xffffffff))
	{
	   if (flexe_multi_deskew_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n field_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  field_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	switch (field_id)
	{
		case 0:
			regp_field_write(chip_id,DESKEW_RATE_M_50G_REG[DESKEW_RATE_M_50G],parameter);
			break;
		case 1:
			regp_field_write(chip_id,DESKEW_RATE_BASE_M_50G_REG[DESKEW_RATE_BASE_M_50G],parameter);
			break;
		case 2:
			regp_field_write(chip_id,DESKEW_RATE_M_100G_REG[DESKEW_RATE_M_100G],parameter);
			break;
		case 3:
			regp_field_write(chip_id,DESKEW_RATE_BASE_M_100G_REG[DESKEW_RATE_BASE_M_100G],parameter);
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
*     flexe_multi_deskew_rate_get
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
*	  field_id: 0~3 0:DESKEW_RATE_M_50G_REG   1:DESKEW_RATE_BASE_M_50G_REG
*					2:DESKEW_RATE_M_100G_REG  3:DESKEW_RATE_BASE_M_100G_REG
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_rate_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (field_id > 3))
	{
	   if (flexe_multi_deskew_para_debug)
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
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	switch (field_id)
	{
		case 0:
			regp_field_read(chip_id,DESKEW_RATE_M_50G_REG[DESKEW_RATE_M_50G],parameter);
			break;
		case 1:
			regp_field_read(chip_id,DESKEW_RATE_BASE_M_50G_REG[DESKEW_RATE_BASE_M_50G],parameter);
			break;
		case 2:
			regp_field_read(chip_id,DESKEW_RATE_M_100G_REG[DESKEW_RATE_M_100G],parameter);
			break;
		case 3:
			regp_field_read(chip_id,DESKEW_RATE_BASE_M_100G_REG[DESKEW_RATE_BASE_M_100G],parameter);
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
*     flexe_multi_deskew_dsw_measure_get
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
*	  grp_id:0~3
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
*    dingyi    2017-12-26    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_dsw_measure_get(UINT_8 chip_id,UINT_8 grp_id,UINT_32* parameter)
{
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV))
    {
        if (flexe_multi_deskew_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n grp_id = %d   \r\n",
                    __FUNCTION__,
                    chip_id,
                    grp_id);
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (flexe_multi_deskew_para_debug)
                        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    switch(grp_id)
    {
        case 0:
        case 1:
            if (grp_id > MAX_INDEX(FLEXE_DSW_MEASURE_0_REG))
            {
                if (flexe_multi_deskew_para_debug)
                {
                    printf("[%s] out of array range\r\n",__FUNCTION__);
                }

                return RET_PARAERR;
            }

            regp_field_read(chip_id,FLEXE_DSW_MEASURE_0_REG[grp_id],parameter);
            break;
        case 2:
        case 3:
            if ((grp_id-2) > MAX_INDEX(FLEXE_DSW_MEASURE_1_REG))
            {
                if (flexe_multi_deskew_para_debug)
                {
                    printf("[%s] out of array range\r\n",__FUNCTION__);
                }

                return RET_PARAERR;
            }

            regp_field_read(chip_id,FLEXE_DSW_MEASURE_1_REG[grp_id-2],parameter);
            break;
        case 4:
        case 5:
            if ((grp_id-4) > MAX_INDEX(FLEXE_DSW_MEASURE_2_REG))
            {
                if (flexe_multi_deskew_para_debug)
                {
                    printf("[%s] out of array range\r\n",__FUNCTION__);
                }

                return RET_PARAERR;
            }

            regp_field_read(chip_id,FLEXE_DSW_MEASURE_2_REG[grp_id-4],parameter);
            break;
        case 6:
        case 7:
            if ((grp_id-6) > MAX_INDEX(FLEXE_DSW_MEASURE_3_REG))
            {
                if (flexe_multi_deskew_para_debug)
                {
                    printf("[%s] out of array range\r\n",__FUNCTION__);
                }

                return RET_PARAERR;
            }
            regp_field_read(chip_id,FLEXE_DSW_MEASURE_3_REG[grp_id-6],parameter);
            break;
        default:
            return RET_PARAERR;
    }

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_dsw_alm_get
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
*    dingyi    2017-12-26    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_dsw_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV))
	{
	   if (flexe_multi_deskew_para_debug)
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
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_DSW_ALM_REG))
	{
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,FLEXE_DSW_ALM_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_multi_deskew_grp_ssf_alm_get
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
*	  field_id:0~2  0:GRP_SSF_ALM  1:PHY200G_INST_NUM_ALM  2:PHY400G_INST_NUM_ALM
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
*    dingyi    2018-8-8    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_multi_deskew_grp_ssf_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV))
	{
	   if (flexe_multi_deskew_para_debug)
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
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_GRP_SSF_ALM_REG))
	{
		if (flexe_multi_deskew_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,FLEXE_GRP_SSF_ALM_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

