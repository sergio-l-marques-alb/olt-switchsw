

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hal.h"
#include "sal.h"
#include "reg_operate.h"
#include "global_macro.h"
#include "top.h"
#include "ccu_cfg.h"
#include "ptp_over_oh.h"
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int module_1588_para_debug = 0;

/******************************************************************************
*
* FUNCTION ieee1588_mode_set
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
*	  chip_id:chip number used
*     mode:0:bypass 1:CBC 2:CTC 3:BC
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
RET_STATUS ieee1588_mode_set(UINT_8 chip_id,UINT_8 mode)
{
	UINT_8 i = 0;
	UINT_32 reg_data = 0;
	if((chip_id > MAX_DEV) || (mode > 3))
	{
	   if (module_1588_para_debug)
       {
           printf("[PTP1588_MODE_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n mode = %d \r\n",   
                                                      chip_id,
                                                      mode);
       }
	   return RET_FAIL;
	}

	if(0 == mode)
	{		
		for(i = 0;i<(TX2RX_PORT_EN+1);i++)
		{
			global_ieee1588_ptp_ts_cfg_set(chip_id,i,0);
		}
		ccu_cfg_rx_ptp_en_set(chip_id,CCU_RX_PARSE_EN,0);
		global_ieee1588_global_cfg_set(chip_id,PTP_1588_MODE,mode);
		
		printf("[%s]mode=%d \r\n",__FUNCTION__,mode);
	}
	else
	{
		global_ieee1588_global_cfg_get(chip_id,PTP_1588_MODE,&reg_data);
		if(reg_data != mode)
		{
			global_ieee1588_global_cfg_set(chip_id,PTP_1588_MODE,mode);
			/*reset 1588 module*/
			for(i = 0;i<(RST_CPU_TX_N+1);i++)
			{
				top_rst_ieee1588_1_reset(chip_id,i,0);
			}
			for(i = 0;i<(RST_UPI_CPU_N+1);i++)
			{
				top_rst_ieee1588_2_reset(chip_id,i,0);
			}
			
			for(i = 0;i<(RST_CPU_TX_N+1);i++)
			{
				top_rst_ieee1588_1_reset(chip_id,i,1);
			}
			for(i = 0;i<(RST_UPI_CPU_N+1);i++)
			{
				top_rst_ieee1588_2_reset(chip_id,i,1);
			}
			for(i = 0;i<(TX2RX_PORT_EN+1);i++)
			{
				global_ieee1588_ptp_ts_cfg_set(chip_id,i,0xff);
			}
			ccu_cfg_rx_ptp_en_set(chip_id,CCU_RX_PARSE_EN,1);
			
			printf("[%s]mode=%d \r\n",__FUNCTION__,mode);
		}
		else
		{
			printf("[%s]mode=%d has been configured!config not be configured!\r\n",__FUNCTION__,mode);
		}
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION ieee1588_port_en_set
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
*	  chip_id:chip number used
*     port:
*	  enable:0:disable 1:enable
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
RET_STATUS ieee1588_port_en_set(UINT_8 chip_id,UINT_8 port,UINT_8 enable)
{
	UINT_32 reg_data = 0;
	if((chip_id > MAX_DEV) || (port > 7) || (enable > 1))
	{
	   if (module_1588_para_debug)
       {
           printf("[PTP1588_MODE_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port = %d, \
                                                      \r\n enable = %d \r\n",   
                                                      chip_id,
                                                      port,
                                                      enable);
       }
	   return RET_FAIL;
	}

	global_ieee1588_ptp_ts_cfg_get(chip_id,RX_PORT_EN,&reg_data);
	reg_data |= (enable&MASK_1_BIT)<< port;	
	global_ieee1588_ptp_ts_cfg_set(chip_id,RX_PORT_EN,reg_data);

	global_ieee1588_ptp_ts_cfg_get(chip_id,TX_PORT_EN,&reg_data);
	reg_data |= (enable&MASK_1_BIT)<< port;	
	global_ieee1588_ptp_ts_cfg_set(chip_id,TX_PORT_EN,reg_data);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION ieee1588_port_en_set
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
*	  chip_id:chip number used
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
RET_STATUS ieee1588_init(UINT_8 chip_id)
{
	ccu_cfg_rx_ccu_leth_type_set(chip_id,CCU_RX_LETH_TYPE,0x88f7);
	ccu_cfg_tx_type_set(chip_id,CCU_TX_ETH_TYPE,0x88f7);
        ccu_cfg_tx_type_set(chip_id,CCU_TX_VLAN_TAG,0x8100);
	return RET_SUCCESS;
}


