
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int b66sar_rx_tx_para_debug = 0;


const CHIP_REG sar_rx_glb_cfg_reg[]=
{
	{RX_LOOPBACK,B66SAR_RX_BASE_ADDR,0x0,0,0},
	{B66SIZE_29B,B66SAR_RX_BASE_ADDR,0x0,1,1},
	{RX_PKT_CNT_PROB,B66SAR_RX_BASE_ADDR,0x0,14,8},
	{RX_PKT_CNT_256_PROB,B66SAR_RX_BASE_ADDR,0x0,23,15},
	{BDCOME_FC_EN,B66SAR_RX_BASE_ADDR,0x0,24,24},
};
const CHIP_REG sar_rx_glb_alm_reg[]=
{
	{BSIZE_ERR,B66SAR_RX_BASE_ADDR,0x1,4,2},
	{ADJ_IN_ALM,B66SAR_RX_BASE_ADDR,0x1,5,5},
	{I1_OVERFLOW,B66SAR_RX_BASE_ADDR,0x1,6,6},
	{I1_UNDERFLOW,B66SAR_RX_BASE_ADDR,0x1,7,7},
	{I2_OVERFLOW,B66SAR_RX_BASE_ADDR,0x1,8,8},
	{I2_UNDERFLOW,B66SAR_RX_BASE_ADDR,0x1,9,9},
	{BT_SOP_ERR,B66SAR_RX_BASE_ADDR,0x1,16,16},
	{BT_CHAN_ERR,B66SAR_RX_BASE_ADDR,0x1,17,17},
};

const CHIP_REG sar_rx_ctrl_cfg_reg[]=
{
	{MON_EN,B66SAR_RX_BASE_ADDR,0x2,0,0},
	{CPB2SAR_CH_EN,B66SAR_RX_BASE_ADDR,0x2,1,1},
	{BRCM_FC_CH_EN,B66SAR_RX_BASE_ADDR,0x2,2,2},
	{RX_CHAN_MAP,B66SAR_RX_BASE_ADDR,0x2,17,8},
	{RX_FIFO_HIGH_CFG,B66SAR_RX_BASE_ADDR,0x2,23,20},
	{RX_FIFO_LOW_CFG,B66SAR_RX_BASE_ADDR,0x2,27,24},
	{RX_FIFO_MID_CFG,B66SAR_RX_BASE_ADDR,0x2,31,28},
};
const CHIP_REG cpb2sar_cfg4_reg[]=
{
	{CPB2SAR_FIFO_ADJ_PERIOD,B66SAR_RX_BASE_ADDR,0x52,14,0},
	{SAR_FIFO_ADJ_PERIOD,B66SAR_RX_BASE_ADDR,0x52,30,16},
};
const CHIP_REG cpb2sar_cfg1_reg[]=
{
	{CPB2SAR_CFG_PERIOD,B66SAR_RX_BASE_ADDR,0x53,12,0},
	{CPB2SAR_NOM_CNT,B66SAR_RX_BASE_ADDR,0x53,25,13},
	{CPB2SAR_FIFO_ADJ_EN,B66SAR_RX_BASE_ADDR,0x53,26,26},
	{SAR_FIFO_ADJ_EN,B66SAR_RX_BASE_ADDR,0x53,27,27},
};
const CHIP_REG sar_rx_pkt_cnt_reg[]=
{
	{RX_PKT_CNT,B66SAR_RX_BASE_ADDR,0x54,31,0},
};
const CHIP_REG sar_rx_pkt_cnt_256_reg[]=
{
	{RX_PKT_CNT_256,B66SAR_RX_BASE_ADDR,0xb1,31,0},
};
const CHIP_REG cpb2sar_cfg2_reg[]=
{
	{CPB2SAR_Q,B66SAR_RX_BASE_ADDR,0x55,12,0},
	{CPB2SAR_ADJ_01EN,B66SAR_RX_BASE_ADDR,0x55,13,13},
	{CPB2SAR_CALENDAR_EN,B66SAR_RX_BASE_ADDR,0x55,14,14},
	{CPB2SAR_ADJ_IN_CHECK_VALUE,B66SAR_RX_BASE_ADDR,0x55,24,15},
	{CPB2SAR_ADJ_IN_CORRECT_EN,B66SAR_RX_BASE_ADDR,0x55,25,25},
};
const CHIP_REG sar_rx_state_reg[]=
{
	{RX_PATH_DELAY,B66SAR_RX_BASE_ADDR,0x56,15,0},
	{CSI,B66SAR_RX_BASE_ADDR,0x56,17,16},
	{CPB2SAR_CURRENT_TABLE,B66SAR_RX_BASE_ADDR,0x56,18,18},
	{CPB2SAR_BUSY,B66SAR_RX_BASE_ADDR,0x56,19,19},
	{RX_JOINT_INIT_DONE,B66SAR_RX_BASE_ADDR,0x56,20,20},
	{CPB2SAR_FIFO_ADJ_INIT_DONE,B66SAR_RX_BASE_ADDR,0x56,21,21},
	{SAR_FIFO_ADJ_INIT_DONE,B66SAR_RX_BASE_ADDR,0x56,22,22},
	{CPB2SAR_DECIMATE_INIT_DONE,B66SAR_RX_BASE_ADDR,0x56,23,23},
};
const CHIP_REG cpb2sar_table_switch_reg[]=
{
	{CPB2SAR_TABLE_SWITCH,B66SAR_RX_BASE_ADDR,0x57,0,0},
	{RX_JOINT_INFO_INIT,B66SAR_RX_BASE_ADDR,0x57,1,1},
	{CPB2SAR_FIFO_ADJ_INIT,B66SAR_RX_BASE_ADDR,0x57,2,2},
	{SAR_FIFO_ADJ_INIT,B66SAR_RX_BASE_ADDR,0x57,3,3},
	{CPB2SAR_DECIMATE_INIT,B66SAR_RX_BASE_ADDR,0x57,4,4},
};
const CHIP_REG cpb2sar_cfg3_reg[]=
{
	{CPB2SAR_LOCAL_EN,B66SAR_RX_BASE_ADDR,0x59,0,0},
	{CPB2SAR_LOCAL_M,B66SAR_RX_BASE_ADDR,0x59,10,1},
	{DELAY_CYCLES,B66SAR_RX_BASE_ADDR,0x59,16,11},
};
const CHIP_REG sar_rx_ch_alm_reg[]=
{
	{RX_PAR_ERR,B66SAR_RX_BASE_ADDR,0x60,0,0},
	{RX_DROP_ONE_ERR,B66SAR_RX_BASE_ADDR,0x60,1,1},
	{RX_DROP_TWO_ERR,B66SAR_RX_BASE_ADDR,0x60,2,2},
	{RX_FIFO_HIGH_IND,B66SAR_RX_BASE_ADDR,0x60,3,3},
	{RX_FIFO_LOW_IND,B66SAR_RX_BASE_ADDR,0x60,4,4},
	{RX_FIFO_FULL,B66SAR_RX_BASE_ADDR,0x60,6,6},
	{RX_FIFO_EMPTY,B66SAR_RX_BASE_ADDR,0x60,7,7},
};
const CHIP_REG b66sar_rx_INT_REG[]=
{
	{BT_SOP_ERR_INT,B66SAR_RX_BASE_ADDR,0xb2,3,2},
	{BT_CHAN_ERR_INT,B66SAR_RX_BASE_ADDR,0xb2,5,4},
};
const CHIP_REG b66sar_rx_INT_MASK_REG[]=
{
	{BT_SOP_ERR_INT_MASK,B66SAR_RX_BASE_ADDR,0xb3,3,2},
	{BT_CHAN_ERR_INT_MASK,B66SAR_RX_BASE_ADDR,0xb3,5,4},
};
const CHIP_REG rx_fifo_err_INT_REG[]=
{
	{RX_FIFO_ERR_0_INT,B66SAR_RX_BASE_ADDR,0xc0,1,0},
	{RX_FIFO_ERR_1_INT,B66SAR_RX_BASE_ADDR,0xc0,3,2},
	{RX_FIFO_ERR_2_INT,B66SAR_RX_BASE_ADDR,0xc0,5,4},
	{RX_FIFO_ERR_3_INT,B66SAR_RX_BASE_ADDR,0xc0,7,6},
	{RX_FIFO_ERR_4_INT,B66SAR_RX_BASE_ADDR,0xc0,9,8},
	{RX_FIFO_ERR_5_INT,B66SAR_RX_BASE_ADDR,0xc0,11,10},
	{RX_FIFO_ERR_6_INT,B66SAR_RX_BASE_ADDR,0xc0,13,12},
	{RX_FIFO_ERR_7_INT,B66SAR_RX_BASE_ADDR,0xc0,15,14},
	{RX_FIFO_ERR_8_INT,B66SAR_RX_BASE_ADDR,0xc0,17,16},
	{RX_FIFO_ERR_9_INT,B66SAR_RX_BASE_ADDR,0xc0,19,18},
	{RX_FIFO_ERR_10_INT,B66SAR_RX_BASE_ADDR,0xc0,21,20},
	{RX_FIFO_ERR_11_INT,B66SAR_RX_BASE_ADDR,0xc0,23,22},
	{RX_FIFO_ERR_12_INT,B66SAR_RX_BASE_ADDR,0xc0,25,24},
	{RX_FIFO_ERR_13_INT,B66SAR_RX_BASE_ADDR,0xc0,27,26},
	{RX_FIFO_ERR_14_INT,B66SAR_RX_BASE_ADDR,0xc0,29,28},
	{RX_FIFO_ERR_15_INT,B66SAR_RX_BASE_ADDR,0xc0,31,30},
};
const CHIP_REG rx_fifo_err_INT_MASK_REG[]=
{
	{RX_FIFO_ERR_0_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,1,0},
	{RX_FIFO_ERR_1_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,3,2},
	{RX_FIFO_ERR_2_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,5,4},
	{RX_FIFO_ERR_3_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,7,6},
	{RX_FIFO_ERR_4_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,9,8},
	{RX_FIFO_ERR_5_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,11,10},
	{RX_FIFO_ERR_6_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,13,12},
	{RX_FIFO_ERR_7_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,15,14},
	{RX_FIFO_ERR_8_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,17,16},
	{RX_FIFO_ERR_9_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,19,18},
	{RX_FIFO_ERR_10_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,21,20},
	{RX_FIFO_ERR_11_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,23,22},
	{RX_FIFO_ERR_12_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,25,24},
	{RX_FIFO_ERR_13_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,27,26},
	{RX_FIFO_ERR_14_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,29,28},
	{RX_FIFO_ERR_15_INT_MASK,B66SAR_RX_BASE_ADDR,0xc5,31,30},
};
const CHIP_RAM cpb2sar_ram = 
{
	TABLE_BASE_ADDR,B66SAR_RX_BASE_ADDR+0x58,7,
};

/*********************TX*******************/
const CHIP_REG sar_tx_glb_cfg_reg[]=
{
	{TX_LOOPBACK,B66SAR_TX_BASE_ADDR,0x0,0,0},
	{TX_B66SIZE_29B,B66SAR_TX_BASE_ADDR,0x0,1,1},
	{TX_CNT_PROB,B66SAR_TX_BASE_ADDR,0x0,14,8},
	{TX_BT256_CNT_PROB,B66SAR_TX_BASE_ADDR,0x0,25,16},
	{BDCOME_LOOPBACK,B66SAR_TX_BASE_ADDR,0x0,31,31},
};
const CHIP_REG ra2sar_cfg1_reg[]=
{
	{RA2SAR_CFG_PERIOD,B66SAR_TX_BASE_ADDR,0x1,12,0},
	{RA2SAR_NOM_CNT,B66SAR_TX_BASE_ADDR,0x1,25,13},
};
const CHIP_REG sar_tx_ctrl_cfg_reg[]=
{
	{SEG_EN,B66SAR_TX_BASE_ADDR,0x2,0,0},
	{RA2SAR_CH_EN,B66SAR_TX_BASE_ADDR,0x2,1,1},
	{TX_CHAN_MAP,B66SAR_TX_BASE_ADDR,0x2,17,8},
};
const CHIP_REG ra2sar_cfg2_reg[]=
{
	{RA2SAR_Q,B66SAR_TX_BASE_ADDR,0x53,12,0},
	{RA2SAR_ADJ_01EN,B66SAR_TX_BASE_ADDR,0x53,13,13},
	{RA2SAR_CALENDAR_EN,B66SAR_TX_BASE_ADDR,0x53,14,14},
	{RA2SAR_ADJ_IN_CHECK_VALUE,B66SAR_TX_BASE_ADDR,0x53,24,15},
	{RA2SAR_ADJ_IN_CORRECT_EN,B66SAR_TX_BASE_ADDR,0x53,25,25},
};
const CHIP_REG sar_tx_pkt_cnt_reg[]=
{
	{TX_PKT_CNT,B66SAR_TX_BASE_ADDR,0x54,31,0},
};
const CHIP_REG sar_tx_pkt_cnt_256_reg[]=
{
	{TX_PKT_CNT_256,B66SAR_TX_BASE_ADDR,0x59,31,0},
};
const CHIP_REG ra2sar_table_switch_reg[]=
{
	{RA2SAR_TABLE_SWITCH,B66SAR_TX_BASE_ADDR,0x55,0,0},
};
const CHIP_REG sar_tx_state_reg[]=
{
	{SEG_FSM_STATE,B66SAR_TX_BASE_ADDR,0x56,3,0},
	{RA2SAR_CURRENT_TABLE,B66SAR_TX_BASE_ADDR,0x56,4,4},
	{RA2SAR_BUSY,B66SAR_TX_BASE_ADDR,0x56,5,5},
};
const CHIP_REG ra2sar_env_alm_reg[]=
{
	{TX_ADJ_IN_ALM,B66SAR_TX_BASE_ADDR,0x57,0,0},
	{TX_I1_OVERFLOW,B66SAR_TX_BASE_ADDR,0x57,1,1},
	{TX_I1_UNDERFLOW,B66SAR_TX_BASE_ADDR,0x57,2,2},
	{TX_I2_OVERFLOW,B66SAR_TX_BASE_ADDR,0x57,3,3},
	{TX_I2_UNDERFLOW,B66SAR_TX_BASE_ADDR,0x57,4,4},
	{TX_BTFIFO_FULL,B66SAR_TX_BASE_ADDR,0x57,5,5},
};
const CHIP_REG b66sar_tx_INT_REG[]=
{
	{TX_BTFIFO_FULL_INT,B66SAR_TX_BASE_ADDR,0x60,1,0},
};
const CHIP_REG b66sar_tx_INT_MASK_REG[]=
{
	{TX_BTFIFO_FULL_INT_MASK,B66SAR_TX_BASE_ADDR,0x61,1,0},
};
const CHIP_REG ra2sar_cfg3_reg[]=
{
	{RA2SAR_LOCAL_EN,B66SAR_TX_BASE_ADDR,0x58,0,0},
	{RA2SAR_LOCAL_M,B66SAR_TX_BASE_ADDR,0x58,10,1},
};
const CHIP_RAM ra2sar_ram = 
{
	TABLE_BASE_ADDR,B66SAR_TX_BASE_ADDR+0x52,7,
};


/*****************sar_rx_311m**************/
const CHIP_REG cpb2sar_m_1st_reg[]=
{
	{CPB2SAR_M_1ST,B66SAR_RX_311M_BASE_ADDR,0x0,31,0},
};
const CHIP_REG cpb2sar_base_m_1st_reg[]=
{
	{CPB2SAR_BASE_M_1ST,B66SAR_RX_311M_BASE_ADDR,0x1,31,0},
};
const CHIP_REG cpb2sar_cfg_311m_reg[]=
{
	{CPB2SAR_MULTIPLIER,B66SAR_RX_311M_BASE_ADDR,0x2,1,0},
	{CPB2SAR_BYPASS_M_N_2ND,B66SAR_RX_311M_BASE_ADDR,0x2,2,2},
};
const CHIP_REG cpb2sar_m_2nd_reg[]=
{
	{CPB2SAR_M_2ND,B66SAR_RX_311M_BASE_ADDR,0x3,31,0},
};
const CHIP_REG cpb2sar_base_m_2nd_reg[]=
{
	{CPB2SAR_BASE_M_2ND,B66SAR_RX_311M_BASE_ADDR,0x4,31,0},
};
/******************sar_tx_311m***************/
const CHIP_REG ra2sar_m_1st_reg[]=
{
	{RA2SAR_M_1ST,B66SAR_TX_311M_BASE_ADDR,0x0,31,0},
};
const CHIP_REG ra2sar_base_m_1st_reg[]=
{
	{RA2SAR_BASE_M_1ST,B66SAR_TX_311M_BASE_ADDR,0x1,31,0},
};
const CHIP_REG ra2sar_cfg_311m_reg[]=
{
	{RA2SAR_MULTIPLIER,B66SAR_TX_311M_BASE_ADDR,0x2,1,0},
	{RA2SAR_BYPASS_M_N_2ND,B66SAR_TX_311M_BASE_ADDR,0x2,2,2},
};
const CHIP_REG ra2sar_m_2nd_reg[]=
{
	{RA2SAR_M_2ND,B66SAR_TX_311M_BASE_ADDR,0x3,31,0},
};
const CHIP_REG ra2sar_base_m_2nd_reg[]=
{
	{RA2SAR_BASE_M_2ND,B66SAR_TX_311M_BASE_ADDR,0x4,31,0},
};


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_tx_loopback_set 
*
* DESCRIPTION
*
*	  0 is disable,1 is enable.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  direction:0:tx 1:rx
*	  parameter:1:enable or 0:disable loopback
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_loopback_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,sar_rx_glb_cfg_reg[RX_LOOPBACK],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_loopback_get
*
* DESCRIPTION
*
*	  0 is disable,1 is enable.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter*:pointer to loopback status
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_loopback_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,sar_rx_glb_cfg_reg[RX_LOOPBACK],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_glb_probe_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  direction:0:rx 1:tx
*	  parameter:
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_tx_b66size_29b_set(UINT_8 chip_id,UINT_8 direction,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || direction > 1 || parameter > 1)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,direction=%d,parameter=%d\r\n",__FUNCTION__,chip_id,direction,parameter);
		}

		return RET_PARAERR;
	}


	if (0 == direction)
	{
		if (B66SIZE_29B > MAX_INDEX(sar_rx_glb_cfg_reg))
		{
			if (b66sar_rx_tx_para_debug)
			{
				printf("[%s] out of array range!!\r\n",__FUNCTION__);
			}

			return RET_PARAERR;
		}
		regp_field_write(chip_id,sar_rx_glb_cfg_reg[B66SIZE_29B],parameter);
	}
	else
	{
		if (TX_B66SIZE_29B > MAX_INDEX(sar_tx_glb_cfg_reg))
		{
			if (b66sar_rx_tx_para_debug)
			{
				printf("[%s] out of array range!!\r\n",__FUNCTION__);
			}

			return RET_PARAERR;
		}
		regp_field_write(chip_id,sar_tx_glb_cfg_reg[TX_B66SIZE_29B],parameter);
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_tx_b66size_29b_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  direction:0:tx 1:rx
*	  parameter:glb probe value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_tx_b66size_29b_get(UINT_8 chip_id,UINT_8 direction,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || direction > 1)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,direction=%d\r\n",__FUNCTION__,chip_id,direction);
		}

		return RET_PARAERR;
	}


	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (0 == direction)
	{
		if (B66SIZE_29B > MAX_INDEX(sar_rx_glb_cfg_reg))
		{
			if (b66sar_rx_tx_para_debug)
			{
				printf("[%s] out of array range!!\r\n",__FUNCTION__);
			}

			return RET_PARAERR;
		}
		regp_field_read(chip_id,sar_rx_glb_cfg_reg[B66SIZE_29B],parameter);
	}
	else
	{
		if (TX_B66SIZE_29B > MAX_INDEX(sar_tx_glb_cfg_reg))
		{
			if (b66sar_rx_tx_para_debug)
			{
				printf("[%s] out of array range!!\r\n",__FUNCTION__);
			}

			return RET_PARAERR;
		}
		regp_field_read(chip_id,sar_tx_glb_cfg_reg[TX_B66SIZE_29B],parameter);
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_pkt_cnt_probe_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  direction:0:tx 1:rx
*	  parameter:glb probe value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_pkt_cnt_probe_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (RX_PKT_CNT_PROB > MAX_INDEX(sar_rx_glb_cfg_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,sar_rx_glb_cfg_reg[RX_PKT_CNT_PROB],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_pkt_cnt_probe_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  *parameter:glb probe value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_pkt_cnt_probe_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (RX_PKT_CNT_PROB > MAX_INDEX(sar_rx_glb_cfg_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,sar_rx_glb_cfg_reg[RX_PKT_CNT_PROB],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_cnt_probe_256_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  direction:0:tx 1:rx
*	  parameter:glb probe value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_pkt_cnt_probe_256_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (RX_PKT_CNT_256_PROB > MAX_INDEX(sar_rx_glb_cfg_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,sar_rx_glb_cfg_reg[RX_PKT_CNT_256_PROB],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_pkt_cnt_probe_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  *parameter:glb probe value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_pkt_cnt_probe_256_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (RX_PKT_CNT_256_PROB > MAX_INDEX(sar_rx_glb_cfg_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,sar_rx_glb_cfg_reg[RX_PKT_CNT_256_PROB],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_bdcome_fc_en_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter:set value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-6-6	     1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_bdcome_fc_en_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (BDCOME_FC_EN > MAX_INDEX(sar_rx_glb_cfg_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,sar_rx_glb_cfg_reg[BDCOME_FC_EN],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_bdcome_fc_en_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  *parameter:get value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_bdcome_fc_en_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (BDCOME_FC_EN > MAX_INDEX(sar_rx_glb_cfg_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,sar_rx_glb_cfg_reg[BDCOME_FC_EN],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION b66sar_rx_pkt_cnt_probe_256_get
*
*	 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  *parameter:glb probe value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_pkt_cnt_256_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,sar_rx_pkt_cnt_256_reg[RX_PKT_CNT_256],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_tx_ctrl_cfg_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  channel_id:0~79
*	  field_id: 0:MON_EN  1:CPB2SAR_CH_EN  2:BRCM_FC_CH_EN 3:RX_CHAN_MAP
*			    4:RX_FIFO_HIGH_CFG  5:RX_FIFO_LOW_CFG  6:RX_FIFO_MID_CFG
*	  parameter:0: disable 1:enable
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_ctrl_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (CH_NUM-1)) || (field_id > 6))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,channel_id = %d,field_id=%d,parameter = %d\r\n",__FUNCTION__,chip_id,channel_id,field_id,parameter);
	   }

	   return RET_PARAERR;
	}
	   
	if (field_id > MAX_INDEX(sar_rx_ctrl_cfg_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,sar_rx_ctrl_cfg_reg[field_id].base_addr,sar_rx_ctrl_cfg_reg[field_id].offset_addr+channel_id,
		sar_rx_ctrl_cfg_reg[field_id].end_bit,sar_rx_ctrl_cfg_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_ctrl_cfg_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  channel_id:0~79
*	  field_id: 0:MON_EN  1:CPB2SAR_CH_EN  2:BRCM_FC_CH_EN 3:RX_CHAN_MAP
*			    4:RX_FIFO_HIGH_CFG  5:RX_FIFO_LOW_CFG  6:RX_FIFO_MID_CFG
*	  parameter*:pointer to ctrl cfg status
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_ctrl_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (CH_NUM-1)) || field_id > 6)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,channel_id = %d,field_id=%d\r\n",__FUNCTION__,chip_id,channel_id,field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(sar_rx_ctrl_cfg_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_bit_read(chip_id,sar_rx_ctrl_cfg_reg[field_id].base_addr,sar_rx_ctrl_cfg_reg[field_id].offset_addr+channel_id,
		sar_rx_ctrl_cfg_reg[field_id].end_bit,sar_rx_ctrl_cfg_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_cpb2sar_cfg4_set
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:CPB2SAR_FIFO_ADJ_PERIOD  1:SAR_FIFO_ADJ_PERIOD  
*	  parameter:set value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-9-12	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_cfg4_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV )
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(cpb2sar_cfg4_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,cpb2sar_cfg4_reg[field_id],parameter);	
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_cpb2sar_cfg4_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:CPB2SAR_FIFO_ADJ_PERIOD  1:SAR_FIFO_ADJ_PERIOD  
*	  parameter:get value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-9-12	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_cfg4_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV )
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(cpb2sar_cfg4_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_read(chip_id,cpb2sar_cfg4_reg[field_id],parameter);	
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_cpb2sar_cfg_set
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  channel_id:0~79
*	  field_id:0~4
*	  parameter*:pointer to ctrl cfg status
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
    if (chip_id > MAX_DEV || field_id > 8)
    {
        if (b66sar_rx_tx_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
        }
        return RET_PARAERR;
    }

    if (field_id < 4)
    {
        regp_field_write(chip_id,cpb2sar_cfg1_reg[field_id],parameter);
    }
    else if (field_id >= 4 && field_id <= 8)
    {
        regp_field_write(chip_id,cpb2sar_cfg2_reg[field_id-4],parameter);
    }

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_cpb2sar_cfg_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~4
*	  parameter*:pointer to ctrl cfg status
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    if (chip_id > MAX_DEV || field_id > 8)
    {
        if (b66sar_rx_tx_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (b66sar_rx_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    if (field_id < 4)
    {
        regp_field_read(chip_id,cpb2sar_cfg1_reg[field_id],parameter);
    }
    else if (field_id >= 4 && field_id <= 8)
    {
        regp_field_read(chip_id,cpb2sar_cfg2_reg[field_id-4],parameter);
    }

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_tx_pkt_cnt_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter*:pointer to pkt count number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_pkt_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,sar_rx_pkt_cnt_reg[RX_PKT_CNT],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_path_delay_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0 RX_PATH_DELAY
*	  1 CSI
*	  2 CPB2SAR_CURRENT_TABLE
*	  3 CPB2SAR_BUSY
*	  4 RX_JOINT_INIT_DONE
*	  5 CPB2SAR_FIFO_ADJ_INIT_DONE
*	  6 SAR_FIFO_ADJ_INIT_DONE
*	  7 CPB2SAR_DECIMATE_INIT_DONE
*	  parameter*:pointer to path delay
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV || field_id > MAX_INDEX(sar_rx_state_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,sar_rx_state_reg[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_sar_rx_ch_alm_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  channel_id:0~39
*	  alm_id:0 sar_error 1:drop one 2:drop two  3:RX_FIFO_HIGH_IND  4:RX_FIFO_LOW_IND
*			   5:RX_FIFO_FULL  6:RX_FIFO_EMPTY
*	  parameter*:pointer to sar channel alm
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_sar_rx_ch_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 alm_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}
	
	if (alm_id > MAX_INDEX(sar_rx_ch_alm_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_bit_read(chip_id,sar_rx_ch_alm_reg[alm_id].base_addr,sar_rx_ch_alm_reg[alm_id].offset_addr+channel_id,
		sar_rx_ch_alm_reg[alm_id].end_bit,sar_rx_ch_alm_reg[alm_id].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_sar_rx_glb_alm_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  channel_id:0~39
*	  alm_id:0~max
*	  parameter*:pointer to sar channel alm
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_sar_rx_glb_alm_get(UINT_8 chip_id,UINT_8 alm_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}
	
	if (alm_id > MAX_INDEX(sar_rx_glb_alm_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_read(chip_id,sar_rx_glb_alm_reg[alm_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_cpb2sar_table_switch_set
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0: CPB2SAR_TABLE_SWITCH 
* 				1: RX_JOINT_INFO_INIT 
*				2: CPB2SAR_FIFO_ADJ_INIT 
*				3: SAR_FIFO_ADJ_INIT 
*				4: CPB2SAR_DECIMATE_INIT 
*	  parameter*:pointer to SEG_FSM_STATE
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_table_switch_set(UINT_8 chip_id,UINT_8 field_id)
{
	if (chip_id > MAX_DEV)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(cpb2sar_table_switch_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_write(chip_id,cpb2sar_table_switch_reg[field_id].base_addr,cpb2sar_table_switch_reg[field_id].offset_addr,1<<field_id);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_cpb2sar_cfg3_set
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:CPB2SAR_LOCAL_EN;1:CPB2SAR_LOCAL_M 2:DELAY_CYCLES
*	  parameter*:pointer to SEG_FSM_STATE
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_cfg3_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(cpb2sar_cfg3_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,cpb2sar_cfg3_reg[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_cpb2sar_cfg3_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:CPB2SAR_LOCAL_EN;1:CPB2SAR_LOCAL_M  2:DELAY_CYCLES
*	  parameter*:pointer to SEG_FSM_STATE
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_cfg3_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s]invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(cpb2sar_cfg3_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,cpb2sar_cfg3_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_sar_rx_cpb2sar_ram_set
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  offset:0~159
*	  parameter*:
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_ram_set(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || offset > 159)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,offset=%d\r\n",__FUNCTION__,chip_id,offset);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	ram_write_single(chip_id,cpb2sar_ram,offset,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_sar_rx_cpb2sar_ram_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  offset:0~159
*	  parameter*:
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_ram_get(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || offset > 159)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,offset=%d\r\n",__FUNCTION__,chip_id,offset);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	ram_read_single(chip_id,cpb2sar_ram,offset,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_int_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:BT_SOP_ERR_INT  1:BT_CHAN_ERR_INT
*	  parameter:set value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-6-6	     1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) || (parameter > 3))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!! chip_id = %d,\
											 parameter = %d \r\n",
											 __FUNCTION__,
											 chip_id,
											 parameter);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(b66sar_rx_INT_REG))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,b66sar_rx_INT_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_int_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:BT_SOP_ERR_INT  1:BT_CHAN_ERR_INT
*	  parameter:get value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-6-6	     1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!! chip_id = %d \r\n",
											 __FUNCTION__,
											 chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(b66sar_rx_INT_REG))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_read(chip_id,b66sar_rx_INT_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_int_mask_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:BT_SOP_ERR_INT  1:BT_CHAN_ERR_INT
*	  parameter:set value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-6-6	     1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) || (parameter > 3))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!! chip_id = %d,\
											 parameter = %d \r\n",
											 __FUNCTION__,
											 chip_id,
											 parameter);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(b66sar_rx_INT_MASK_REG))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,b66sar_rx_INT_MASK_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_int_mask_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0:BT_SOP_ERR_INT  1:BT_CHAN_ERR_INT
*	  parameter:get value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-6-6	     1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!! chip_id = %d \r\n",
											 __FUNCTION__,
											 chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(b66sar_rx_INT_MASK_REG))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_read(chip_id,b66sar_rx_INT_MASK_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_fifo_err_int_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  ch_id: 0~79
*	  parameter:set value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-11-1	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_fifo_err_int_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1) || (parameter > 3))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!! \
											 \r\n chip_id = %d,\
											 \r\n ch_id = %d,\
											 \r\n parameter = %d \r\n",
											 __FUNCTION__,
											 chip_id,
											 ch_id,
											 parameter);
		}

		return RET_PARAERR;
	}

	if (ch_id%16 > MAX_INDEX(rx_fifo_err_INT_REG))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,rx_fifo_err_INT_REG[ch_id%16].base_addr,rx_fifo_err_INT_REG[ch_id%16].offset_addr+ch_id/16,
		rx_fifo_err_INT_REG[ch_id%16].end_bit,rx_fifo_err_INT_REG[ch_id%16].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_fifo_err_int_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  ch_id: 0~79
*	  parameter:get value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-11-1	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_fifo_err_int_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!! \
											 \r\n chip_id = %d,\
											 \r\n ch_id = %d \r\n",
											 __FUNCTION__,
											 chip_id,
											 ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (ch_id%16 > MAX_INDEX(rx_fifo_err_INT_REG))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_bit_read(chip_id,rx_fifo_err_INT_REG[ch_id%16].base_addr,rx_fifo_err_INT_REG[ch_id%16].offset_addr+ch_id/16,
		rx_fifo_err_INT_REG[ch_id%16].end_bit,rx_fifo_err_INT_REG[ch_id%16].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_fifo_err_int_mask_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  ch_id: 0~79
*	  parameter:set value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-11-1	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_fifo_err_int_mask_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1) || (parameter > 3))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!! \
											 \r\n chip_id = %d,\
											 \r\n ch_id = %d,\
											 \r\n parameter = %d \r\n",
											 __FUNCTION__,
											 chip_id,
											 ch_id,
											 parameter);
		}

		return RET_PARAERR;
	}

	if (ch_id%16 > MAX_INDEX(rx_fifo_err_INT_MASK_REG))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,rx_fifo_err_INT_MASK_REG[ch_id%16].base_addr,rx_fifo_err_INT_MASK_REG[ch_id%16].offset_addr+ch_id/16,
		rx_fifo_err_INT_MASK_REG[ch_id%16].end_bit,rx_fifo_err_INT_MASK_REG[ch_id%16].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_fifo_err_int_mask_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  ch_id: 0~79
*	  parameter:get value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi    2018-11-1	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_fifo_err_int_mask_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!! \
											 \r\n chip_id = %d,\
											 \r\n ch_id = %d \r\n",
											 __FUNCTION__,
											 chip_id,
											 ch_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	if (ch_id%16 > MAX_INDEX(rx_fifo_err_INT_MASK_REG))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_bit_read(chip_id,rx_fifo_err_INT_MASK_REG[ch_id%16].base_addr,rx_fifo_err_INT_MASK_REG[ch_id%16].offset_addr+ch_id/16,
		rx_fifo_err_INT_MASK_REG[ch_id%16].end_bit,rx_fifo_err_INT_MASK_REG[ch_id%16].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_loopback_set 
*
* DESCRIPTION
*
*	  0 is disable,1 is enable.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter:1:enable or 0:disable loopback
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_loopback_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,sar_tx_glb_cfg_reg[TX_LOOPBACK],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_loopback_get
*
* DESCRIPTION
*
*	  0 is disable,1 is enable.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter*:pointer to loopback status
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_loopback_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,sar_tx_glb_cfg_reg[TX_LOOPBACK],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_cnt_probe_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0 cnt probe;1 bt256 cnt probe
*	  parameter:set value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_cnt_probe_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
        if (b66sar_rx_tx_para_debug)
        {
            printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
        }

        return RET_PARAERR;
    }

    if ((field_id+2) > MAX_INDEX(sar_tx_glb_cfg_reg))
    {
        if (b66sar_rx_tx_para_debug)
        {
            printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    /* coverity[overrun-local : FALSE] */
    regp_field_write(chip_id,sar_tx_glb_cfg_reg[field_id+2],parameter);

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_cnt_probe_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0 cnt probe;1 bt256 cnt probe
*	  *parameter:value to get
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_cnt_probe_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}

    if ((field_id+2) > MAX_INDEX(sar_tx_glb_cfg_reg) - 2)
    {
        if (b66sar_rx_tx_para_debug)
        {
            printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    /* coverity[overrun-local : FALSE] */
    regp_field_read(chip_id,sar_tx_glb_cfg_reg[field_id+2],parameter);

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_bdcome_loopback_set 
*
* DESCRIPTION
*
*	  0 is disable,1 is enable.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter:1:enable or 0:disable loopback
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi     2018-6-6	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_bdcome_loopback_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
	   }

	   return RET_PARAERR;
	}
	   
	if (BDCOME_LOOPBACK > MAX_INDEX(sar_tx_glb_cfg_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,sar_tx_glb_cfg_reg[BDCOME_LOOPBACK],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_bdcome_loopback_get
*
* DESCRIPTION
*
*	  0 is disable,1 is enable.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter*:pointer to loopback status
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi     2018-6-6	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_bdcome_loopback_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}
	
	if (BDCOME_LOOPBACK > MAX_INDEX(sar_tx_glb_cfg_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_read(chip_id,sar_tx_glb_cfg_reg[BDCOME_LOOPBACK],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_ra2sar_cfg_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~4
*	  parameter:block number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
    if (chip_id > MAX_DEV || field_id > 6)
    {
        if (b66sar_rx_tx_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
        }

        return RET_PARAERR;
    }

    if (field_id < 2)
    {
        regp_field_write(chip_id,ra2sar_cfg1_reg[field_id],parameter);
    }
    else if (field_id >= 2 && field_id <= 6)
    {
        regp_field_write(chip_id,ra2sar_cfg2_reg[field_id-2],parameter);
    }

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_ra2sar_cfg_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~4
*	  parameter:block number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    if (chip_id > MAX_DEV || field_id > 6)
    {
        if (b66sar_rx_tx_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (b66sar_rx_tx_para_debug)
        {
            printf("[%s] invalid parameter pointer!!",__FUNCTION__);
        }

        return RET_PARAERR;
    }

    if (field_id < 2)
    {
        regp_field_read(chip_id,ra2sar_cfg1_reg[field_id],parameter);
    }
    else if (field_id >= 2 && field_id <= 6)
    {
        regp_field_read(chip_id,ra2sar_cfg2_reg[field_id-2],parameter);
    }


    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_ctrl_cfg_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  channel_id:0~79
*	  field_id:0~2
*	  parameter:block number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ctrl_cfg_set(UINT_8 chip_id,UINT_8 channel_id,UINT_8 field_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (CH_NUM-1)) || (field_id > 2))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,channel_id = %d,field_id=%d,parameter = %d\r\n",__FUNCTION__,chip_id,channel_id,field_id,parameter);
	   }

	   return RET_PARAERR;
	}
	   
	if (field_id > MAX_INDEX(sar_tx_ctrl_cfg_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_bit_write(chip_id,sar_tx_ctrl_cfg_reg[field_id].base_addr,sar_tx_ctrl_cfg_reg[field_id].offset_addr+channel_id,
		sar_tx_ctrl_cfg_reg[field_id].end_bit,sar_tx_ctrl_cfg_reg[field_id].start_bit,parameter);	
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_ctrl_cfg_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  channel_id:0~39
*	  parameter*:pointer to ctrl cfg status
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ctrl_cfg_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (channel_id > (CH_NUM-1)) || (field_id > 2))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,channel_id = %d,field_id=%d\r\n",__FUNCTION__,chip_id,channel_id,field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(sar_tx_ctrl_cfg_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_bit_read(chip_id,sar_tx_ctrl_cfg_reg[field_id].base_addr,sar_tx_ctrl_cfg_reg[field_id].offset_addr+channel_id,
		sar_tx_ctrl_cfg_reg[field_id].end_bit,sar_tx_ctrl_cfg_reg[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_pkt_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  channel_id:0~39
*	  parameter*:pointer to rx ts number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_pkt_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,sar_tx_pkt_cnt_reg[TX_PKT_CNT],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_pkt_256_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  channel_id:0~39
*	  parameter*:pointer to rx ts number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_pkt_256_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,sar_tx_pkt_cnt_256_reg[TX_PKT_CNT_256],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_ra2sar_table_switch_set
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:RA2SAR_TABLE_SWITCH 1:sar_rx_joint_info_init
*	  parameter:
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_table_switch_set(UINT_8 chip_id,UINT_8 field_id)
{
	if (chip_id > MAX_DEV)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(ra2sar_table_switch_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,ra2sar_table_switch_reg[field_id],1);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_rx_tx_seg_fsm_state_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~max_index
*	  parameter*:pointer to SEG_FSM_STATE
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_state_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV || field_id > MAX_INDEX(sar_tx_state_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,sar_tx_state_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_ra2sar_env_alm_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~max_index
*	  parameter*:pointer to get value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_env_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(ra2sar_env_alm_reg))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] out of array range\r\n",__FUNCTION__);
	   }

	   return RET_PARAERR;
	}

	regp_field_read(chip_id,ra2sar_env_alm_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_bdcome_loopback_set 
*
* DESCRIPTION
*
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter:set value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi     2018-6-6	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_int_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,b66sar_tx_INT_REG[TX_BTFIFO_FULL_INT],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_bdcome_loopback_get
*
* DESCRIPTION
*
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter*:get value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi     2018-6-6	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_int_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,b66sar_tx_INT_REG[TX_BTFIFO_FULL_INT],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_int_mask_set 
*
* DESCRIPTION
*
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter:set value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi     2018-6-6	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_int_mask_set(UINT_8 chip_id,UINT_32 parameter)
{
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) || (parameter > 1))
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d,parameter = %d\r\n",__FUNCTION__,chip_id,parameter);
	   }

	   return RET_PARAERR;
	}

	regp_field_write(chip_id,b66sar_tx_INT_MASK_REG[TX_BTFIFO_FULL_INT_MASK],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_int_mask_get
*
* DESCRIPTION
*
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  parameter*:get value
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 dingyi     2018-6-6	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_int_mask_get(UINT_8 chip_id,UINT_32* parameter)
{
	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (b66sar_rx_tx_para_debug)
	   {
		   printf("[%s] invalid parameter!! chip_id = %d\r\n",__FUNCTION__,chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);				
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,b66sar_tx_INT_MASK_REG[TX_BTFIFO_FULL_INT_MASK],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_ra2sar_cfg3_set
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:RA2SAR_LOCAL_EN;1:RA2SAR_LOCAL_M
*	  parameter*:pointer to seg_fsm_state
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_cfg3_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(ra2sar_cfg3_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,ra2sar_cfg3_reg[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_tx_ra2sar_cfg3_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0:RA2SAR_LOCAL_EN;1:RA2SAR_LOCAL_M
*	  parameter*:pointer to seg_fsm_state
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_cfg3_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s]invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(ra2sar_cfg3_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] out of array range!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	regp_field_read(chip_id,ra2sar_cfg3_reg[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*	  b66sar_sar_tx_ra2sar_ram_set
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  offset:0~159
*	  parameter*:
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_ram_set(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || offset > 159)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,offset=%d\r\n",__FUNCTION__,chip_id,offset);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	ram_write_single(chip_id,ra2sar_ram,offset,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	  b66sar_sar_tx_ra2sar_ram_get
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  offset:0~159
*	  parameter*:
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_ram_get(UINT_8 chip_id,UINT_8 offset,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || offset > 159)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,offset=%d\r\n",__FUNCTION__,chip_id,offset);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	ram_read_single(chip_id,ra2sar_ram,offset,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_cpb_m_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3
*	  parameter:block number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb_m_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || field_id > 3)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
		}

		return RET_PARAERR;
	}

	switch(field_id)
	{
		case 0:
			regp_field_write(chip_id,cpb2sar_m_1st_reg[0],parameter);
			break;
		case 1:
			regp_field_write(chip_id,cpb2sar_base_m_1st_reg[0],parameter);
			break;
		case 2:
			regp_field_write(chip_id,cpb2sar_m_2nd_reg[0],parameter);
			break;
		case 3:
			regp_field_write(chip_id,cpb2sar_base_m_2nd_reg[0],parameter);
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
*	 b66sar_rx_cpb_m_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3
*	  parameter:block number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb_m_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || field_id > 3)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	switch(field_id)
	{
		case 0:
			regp_field_read(chip_id,cpb2sar_m_1st_reg[0],parameter);
			break;
		case 1:
			regp_field_read(chip_id,cpb2sar_base_m_1st_reg[0],parameter);
			break;
		case 2:
			regp_field_read(chip_id,cpb2sar_m_2nd_reg[0],parameter);
			break;
		case 3:
			regp_field_read(chip_id,cpb2sar_base_m_2nd_reg[0],parameter);
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
*	 b66sar_rx_cpb2sar_cfg_311m_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1
*	  parameter:block number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_cfg_311m_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || field_id > MAX_INDEX(cpb2sar_cfg_311m_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);		
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,cpb2sar_cfg_311m_reg[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_rx_cpb2sar_cfg_311m_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1
*	  *parameter:
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_rx_cpb2sar_cfg_311m_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || field_id > MAX_INDEX(cpb2sar_cfg_311m_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);		
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
		
	regp_field_read(chip_id,cpb2sar_cfg_311m_reg[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_ra_m_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3
*	  parameter:block number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra_m_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || field_id > 3)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
		}

		return RET_PARAERR;
	}

	switch(field_id)
	{
		case 0:
			regp_field_write(chip_id,ra2sar_m_1st_reg[0],parameter);
			break;
		case 1:
			regp_field_write(chip_id,ra2sar_base_m_1st_reg[0],parameter);
			break;
		case 2:
			regp_field_write(chip_id,ra2sar_m_2nd_reg[0],parameter);
			break;
		case 3:
			regp_field_write(chip_id,ra2sar_base_m_2nd_reg[0],parameter);
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
*	 b66sar_tx_ra_m_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3
*	  parameter:block number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra_m_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || field_id > 3)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	switch(field_id)
	{
		case 0:
			regp_field_read(chip_id,ra2sar_m_1st_reg[0],parameter);
			break;
		case 1:
			regp_field_read(chip_id,ra2sar_base_m_1st_reg[0],parameter);
			break;
		case 2:
			regp_field_read(chip_id,ra2sar_m_2nd_reg[0],parameter);
			break;
		case 3:
			regp_field_read(chip_id,ra2sar_base_m_2nd_reg[0],parameter);
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
*	 b66sar_tx_ra2sar_cfg_311m_set 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1
*	  parameter:block number
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_cfg_311m_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || field_id > MAX_INDEX(ra2sar_cfg_311m_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);		
		}

		return RET_PARAERR;
	}

	regp_field_write(chip_id,ra2sar_cfg_311m_reg[field_id],parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*	 b66sar_tx_ra2sar_cfg_311m_get 
*
* DESCRIPTION
*
*	  
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1
*	  *parameter:
* RETURNS
*
*	  RET_SUCCESS: success
*	  RET_FAIL: fail
*	  RET_PARAERR: parameter error	
*
* CALLS
*
*	  RESERVED
*
* CALLED BY
*
*	  RESERVED
*
* VERSION
*
*	<author>	 <data>    <CR_ID>		<DESCRIPTION>
*	 jxma    2015-10-22	 1.0		   initial
*
******************************************************************************/
RET_STATUS b66sar_tx_ra2sar_cfg_311m_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || field_id > MAX_INDEX(ra2sar_cfg_311m_reg))
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,field_id=%d\r\n",__FUNCTION__,chip_id,field_id);		
		}

		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (b66sar_rx_tx_para_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}
		
	regp_field_read(chip_id,ra2sar_cfg_311m_reg[field_id],parameter);
	
	return RET_SUCCESS;
}
