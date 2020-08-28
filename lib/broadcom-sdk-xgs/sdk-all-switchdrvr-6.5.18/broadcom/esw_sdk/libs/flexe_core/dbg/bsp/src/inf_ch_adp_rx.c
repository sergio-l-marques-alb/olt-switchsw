

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int inf_ch_adp_rx_para_debug = 0;

const CHIP_REG rx_map_table_reg[]=
{
	{RX_MAP_TABLE,INF_CH_ADP_RX_BASE_ADDR,0x0,31,0},
};
const CHIP_REG ts_delta_reg[]=
{
	{TS_DELTA,INF_CH_ADP_RX_BASE_ADDR,0x1,31,0},
};
const CHIP_REG mode_cfg_reg[]=
{
	{IS_400G,INF_CH_ADP_RX_BASE_ADDR,0x9,0,0},
	{IS_200G,INF_CH_ADP_RX_BASE_ADDR,0x9,4,1},
	{IS_PORT6,INF_CH_ADP_RX_BASE_ADDR,0x9,5,5},
	{IS_ASYM,INF_CH_ADP_RX_BASE_ADDR,0x9,6,6},
	{ASYM_INST_SEL,INF_CH_ADP_RX_BASE_ADDR,0x9,7,7},
};
const CHIP_REG lf_sel_reg[]=
{
	{LF_SEL,INF_CH_ADP_RX_BASE_ADDR,0xa,23,0},
};
const CHIP_REG rx_port_restart_reg[]=
{
	{RX_PORT_RESTART,INF_CH_ADP_RX_BASE_ADDR,0xc,7,0},
};
const CHIP_REG rx_gearbox_alm_reg[]=
{
	{FULL,INF_CH_ADP_RX_BASE_ADDR,0xb,7,0},
	{FIELD_EMPTY,INF_CH_ADP_RX_BASE_ADDR,0xb,15,8},
};

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_map_table_set
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_rx_map_table_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 7) || (parameter > 0xf))
	{
	   if (inf_ch_adp_rx_para_debug)
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

	regp_bit_write(chip_id,rx_map_table_reg[RX_MAP_TABLE].base_addr,rx_map_table_reg[RX_MAP_TABLE].offset_addr,
		channel_id*4+3,channel_id*4,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_map_table_get
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_rx_map_table_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 7))
	{
	   if (inf_ch_adp_rx_para_debug)
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
        if (inf_ch_adp_rx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,rx_map_table_reg[RX_MAP_TABLE].base_addr,rx_map_table_reg[RX_MAP_TABLE].offset_addr,
		channel_id*4+3,channel_id*4,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_ts_delta_set
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
*	  low_high: 0:low  1:high
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
RET_STATUS inf_ch_adp_rx_ts_delta_set(UINT_8 chip_id,UINT_8 low_high,UINT_8 grp_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (low_high > 1) || (grp_id > 7) || (parameter > 0xffff))
	{
	   if (inf_ch_adp_rx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n low_high = %d, \
													  \r\n grp_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  low_high,
													  grp_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,ts_delta_reg[TS_DELTA].base_addr,ts_delta_reg[TS_DELTA].offset_addr+grp_id,
		low_high*16+15,low_high*16,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_map_table_get
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
*	  low_high: 0:low  1:high
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
RET_STATUS inf_ch_adp_rx_ts_delta_get(UINT_8 chip_id,UINT_8 low_high,UINT_8 grp_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (low_high > 1) || (grp_id > 7))
	{
	   if (inf_ch_adp_rx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n low_high = %d, \
													  \r\n grp_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  low_high,
													  grp_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_rx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,ts_delta_reg[TS_DELTA].base_addr,ts_delta_reg[TS_DELTA].offset_addr+grp_id,
		low_high*16+15,low_high*16,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_mode_cfg_set
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
*	  field_id: 0:IS_400G 1:IS_200G 2:IS_PORT6 3:IS_ASYM 4:ASYM_INST_SEL
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
RET_STATUS inf_ch_adp_rx_mode_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_rx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(mode_cfg_reg))
	{
		if (inf_ch_adp_rx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,mode_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_mode_cfg_get
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
*	  field_id: 0:IS_400G 1:IS_200G 2:IS_PORT6 3:IS_ASYM 4:ASYM_INST_SEL
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
RET_STATUS inf_ch_adp_rx_mode_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (inf_ch_adp_rx_para_debug)
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
        if (inf_ch_adp_rx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(mode_cfg_reg))
	{
		if (inf_ch_adp_rx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,mode_cfg_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_mode_cfg_400g_set
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
RET_STATUS inf_ch_adp_rx_mode_cfg_400g_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV))
	{
	   if (inf_ch_adp_rx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d \r\n",
													  __FUNCTION__,
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,mode_cfg_reg[IS_400G],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_mode_cfg_400g_get
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
RET_STATUS inf_ch_adp_rx_mode_cfg_400g_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV))
	{
	   if (inf_ch_adp_rx_para_debug)
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
        if (inf_ch_adp_rx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_field_read(chip_id,mode_cfg_reg[IS_400G],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_mode_cfg_200g_set
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_rx_mode_cfg_200g_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 3) || (parameter > 1))
	{
	   if (inf_ch_adp_rx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d,\
													  \r\n port_id = %d,\
													  \r\n parameter = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	if (IS_200G > MAX_INDEX(mode_cfg_reg))
	{
		if (inf_ch_adp_rx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_write(chip_id,mode_cfg_reg[IS_200G].base_addr,mode_cfg_reg[IS_200G].offset_addr,port_id+1,port_id+1,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_mode_cfg_200g_get
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_rx_mode_cfg_200g_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 3))
	{
	   if (inf_ch_adp_rx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d,\
													  \r\n port_id = %d \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_rx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (IS_200G > MAX_INDEX(mode_cfg_reg))
	{
		if (inf_ch_adp_rx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_bit_read(chip_id,mode_cfg_reg[IS_200G].base_addr,mode_cfg_reg[IS_200G].offset_addr,port_id+1,port_id+1,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_lf_sel_set
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
*    dingyi    2018-5-23    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_rx_lf_sel_set(UINT_8 chip_id,UINT_8 channel_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 7) || (parameter > 0x7))
	{
	   if (inf_ch_adp_rx_para_debug)
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

	regp_bit_write(chip_id,lf_sel_reg[LF_SEL].base_addr,lf_sel_reg[LF_SEL].offset_addr,
		channel_id*3+2,channel_id*3,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_lf_sel_get
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
RET_STATUS inf_ch_adp_rx_lf_sel_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > 7))
	{
	   if (inf_ch_adp_rx_para_debug)
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
        if (inf_ch_adp_rx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,lf_sel_reg[LF_SEL].base_addr,lf_sel_reg[LF_SEL].offset_addr,
		channel_id*3+2,channel_id*3,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_port_restart_set
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
*	  port_id:port select 0~7 
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
*    dingyi    2018-9-27    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_rx_port_restart_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7) || (parameter > 0x1))
	{
	   if (inf_ch_adp_rx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d, \
													  \r\n parameter = 0x%x\r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id,
													  parameter);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,rx_port_restart_reg[RX_PORT_RESTART].base_addr,rx_port_restart_reg[RX_PORT_RESTART].offset_addr,
		port_id,port_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_port_restart_get
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
*	  port_id:port select 0~7 
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
*    dingyi    2018-9-27    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_rx_port_restart_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (port_id > 7))
	{
	   if (inf_ch_adp_rx_para_debug)
	   {
		   printf("[%s] invalid parameter!! \
													  \r\n chip_id = %d, \
													  \r\n port_id = %d  \r\n",
													  __FUNCTION__,
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
    {
        if (inf_ch_adp_rx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	regp_bit_read(chip_id,rx_port_restart_reg[RX_PORT_RESTART].base_addr,rx_port_restart_reg[RX_PORT_RESTART].offset_addr,
		port_id,port_id,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     inf_ch_adp_rx_gearbox_alm_get
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
*	  field_id: 0:FULL  1:EMPTY
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
RET_STATUS inf_ch_adp_rx_gearbox_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV))
	{
	   if (inf_ch_adp_rx_para_debug)
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
        if (inf_ch_adp_rx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
		
        return RET_PARAERR;
    }

	if (field_id > MAX_INDEX(rx_gearbox_alm_reg))
	{
		if (inf_ch_adp_rx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,rx_gearbox_alm_reg[field_id],parameter);
	
	return RET_SUCCESS;
}
