

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "global_macro.h"
#include <string.h>
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int ccu_rx_tx_para_debug = 0;

const CHIP_REG CCU_RX_FRAME_CNT_REG[]=
{
	{CCU_RX_FRAME_CNT,CCU_RX_BASE_ADDR,0x0,31,0},
};
const CHIP_REG CCU_RX_DROP_CNT_REG[]=
{
	{CCU_RX_DROP_CNT,CCU_RX_BASE_ADDR,0x1,31,0},
};
const CHIP_REG CCU_RX_FILTER_CNT_REG[]=
{
	{CCU_RX_FILTER_CNT,CCU_RX_BASE_ADDR,0x2,31,0},
};
const CHIP_REG CCU_RX_FRM_ERR_CNT_REG[]=
{
	{CCU_RX_FRM_ERR_CNT,CCU_RX_BASE_ADDR,0x3,31,0},
};
const CHIP_REG CCU_RX_LEN_ERR_CNT_REG[]=
{
	{CCU_RX_LEN_ERR_CNT,CCU_RX_BASE_ADDR,0x4,31,0},
};
const CHIP_REG CCU_RX_PKT_IN_NUM_REG[]=
{
	{CCU_RX_PKT_IN_NUM,CCU_RX_BASE_ADDR,0x8,31,0},
};
const CHIP_REG CCU_RX_SYNC_CNT_REG[]=
{
	{CCU_RX_SYNC_CNT,CCU_RX_BASE_ADDR,0xd,31,0},
};
const CHIP_REG CCU_RX_DELAY_REQ_CNT_REG[]=
{
	{CCU_RX_DELAY_REQ_CNT,CCU_RX_BASE_ADDR,0xe,31,0},
};
const CHIP_REG CCU_RX_PDELAY_REQ_CNT_REG[]=
{
	{CCU_RX_PDELAY_REQ_CNT,CCU_RX_BASE_ADDR,0xf,31,0},
};
const CHIP_REG CCU_RX_PDELAY_RESP_CNT_REG[]=
{
	{CCU_RX_PDELAY_RESP_CNT,CCU_RX_BASE_ADDR,0x10,31,0},
};
const CHIP_REG CCU_RX_FOLLOW_UP_CNT_REG[]=
{
	{CCU_RX_FOLLOW_UP_CNT,CCU_RX_BASE_ADDR,0x11,31,0},
};
const CHIP_REG CCU_RX_DELAY_RESP_CNT_REG[]=
{
	{CCU_RX_DELAY_RESP_CNT,CCU_RX_BASE_ADDR,0x12,31,0},
};
const CHIP_REG CCU_RX_PDELAY_FOLLOW_CNT_REG[]=
{
	{CCU_RX_PDELAY_FOLLOW_CNT,CCU_RX_BASE_ADDR,0x13,31,0},
};
const CHIP_REG CCU_RX_ANNOUNCE_CNT_REG[]=
{
	{CCU_RX_ANNOUNCE_CNT,CCU_RX_BASE_ADDR,0x14,31,0},
};
const CHIP_REG CCU_RX_SIGNALING_CNT_REG[]=
{
	{CCU_RX_SIGNALING_CNT,CCU_RX_BASE_ADDR,0x15,31,0},
};
const CHIP_REG CCU_RX_MANAGEMENT_CNT_REG[]=
{
	{CCU_RX_MANAGEMENT_CNT,CCU_RX_BASE_ADDR,0x16,31,0},
};
const CHIP_REG CCU_RX_SSM_CNT_REG[]=
{
	{CCU_RX_SSM_CNT,CCU_RX_BASE_ADDR,0x17,31,0},
};
const CHIP_REG CCU_RX_SSM_MESSAGE0_REG[]=
{
	{CCU_RX_SSM_MESSAGE0,CCU_RX_BASE_ADDR,0x18,31,0},
};
const CHIP_REG CCU_RX_SSM_REQ_REG[]=
{
	{CCU_RX_SSM_REQ,CCU_RX_BASE_ADDR,0x19,0,0},
};
const CHIP_REG CCU_RX_SSM_MESSAGE1_REG[]=
{
	{CCU_RX_SSM_MESSAGE1,CCU_RX_BASE_ADDR,0x1a,31,0},
};
const CHIP_REG CCU_RX_RESV_CNT_REG[]=
{
	{CCU_RX_RESV_CNT,CCU_RX_BASE_ADDR,0x1b,31,0},
};
const CHIP_REG CCU_RX_DECODE_ERR_CNT_REG[]=
{
	{CCU_RX_DECODE_ERR_CNT,CCU_RX_BASE_ADDR,0x1c,31,0},
};
const CHIP_REG CCU_RX_DECODE_SPKT_BYTE_CNT_REG[]=
{
	{CCU_RX_DECODE_SPKT_BYTE_CNT,CCU_RX_BASE_ADDR,0x1d,31,0},
};
const CHIP_REG CCU_RX_DECODE_SPKT_NUM_CNT_REG[]=
{
	{CCU_RX_DECODE_SPKT_NUM_CNT,CCU_RX_BASE_ADDR,0x1e,31,0},
};
const CHIP_REG CCU_RX_ALARM_REG[]=
{
	{CCU_RX_INPORT_FIFO_FULL,CCU_RX_BASE_ADDR,0x5,0,0},
	{CCU_RX_OVER_LENGTH,CCU_RX_BASE_ADDR,0x5,1,1},
	{CCU_RX_DECODE_ERR,CCU_RX_BASE_ADDR,0x5,2,2},
	{CCU_RX_OH_BUFF_FULL,CCU_RX_BASE_ADDR,0x5,3,3},
	{CCU_RX_DECODE_EOP_ABNOR,CCU_RX_BASE_ADDR,0x5,4,4},
	{CCU_RX_DECODE_REMOTE_FAULT,CCU_RX_BASE_ADDR,0x5,5,5},
	{CCU_RX_DECODE_LOCAL_FAULT,CCU_RX_BASE_ADDR,0x5,6,6},
	{CCU_RX_LEN_MISMATCH,CCU_RX_BASE_ADDR,0x5,7,7},
	{CCU_RX_INPUT_NO_EOP,CCU_RX_BASE_ADDR,0x5,8,8},
};
const CHIP_REG CCU_RX_INT_REG[]=
{
	{CCU_RX_INPORT_FIFO_FULL_INT,CCU_RX_BASE_ADDR,0x6,1,0},
	{CCU_RX_OVER_LENGTH_INT,CCU_RX_BASE_ADDR,0x6,3,2},
	{CCU_RX_DECODE_ERR_INT,CCU_RX_BASE_ADDR,0x6,5,4},
	{CCU_RX_OH_BUFF_FULL_INT,CCU_RX_BASE_ADDR,0x6,7,6},
	{CCU_RX_DECODE_EOP_ABNOR_INT,CCU_RX_BASE_ADDR,0x6,9,8},
	{CCU_RX_INPUT_NO_EOP_INT,CCU_RX_BASE_ADDR,0x6,11,10},
};
const CHIP_REG CCU_RX_INT_MASK_REG[]=
{
	{CCU_RX_INPORT_FIFO_FULL_INT_MASK,CCU_RX_BASE_ADDR,0x7,1,0},
	{CCU_RX_OVER_LENGTH_INT_MASK,CCU_RX_BASE_ADDR,0x7,3,2},
	{CCU_RX_DECODE_ERR_INT_MASK,CCU_RX_BASE_ADDR,0x7,5,4},
	{CCU_RX_OH_BUFF_FULL_INT_MASK,CCU_RX_BASE_ADDR,0x7,7,6},
	{CCU_RX_DECODE_EOP_ABNOR_INT_MASK,CCU_RX_BASE_ADDR,0x7,9,8},
	{CCU_RX_INPUT_NO_EOP_INT_MASK,CCU_RX_BASE_ADDR,0x7,11,10},
};

const CHIP_REG CCU_TX_FRAME_CNT_REG[]=
{
	{CCU_TX_FRAME_CNT,CCU_TX_BASE_ADDR,0x0,31,0},
};
const CHIP_REG CCU_TX_DROP_CNT_REG[]=
{
	{CCU_TX_DROP_CNT,CCU_TX_BASE_ADDR,0x1,31,0},
};
const CHIP_REG CCU_TX_FILTER_CNT_REG[]=
{
	{CCU_TX_FILTER_CNT,CCU_TX_BASE_ADDR,0x2,31,0},
};
const CHIP_REG CCU_TX_ERR_CNT_REG[]=
{
	{CCU_TX_ERR_CNT,CCU_TX_BASE_ADDR,0x3,31,0},
};
const CHIP_REG CCU_TX_LEN_ERR_CNT_REG[]=
{
	{CCU_TX_LEN_ERR_CNT,CCU_TX_BASE_ADDR,0x4,31,0},
};
const CHIP_REG CCU_TX_PTP_CNT_REG[]=
{
	{CCU_TX_PTP_CNT,CCU_TX_BASE_ADDR,0x5,31,0},
};
const CHIP_REG CCU_TX_SSM_CNT_REG[]=
{
	{CCU_TX_SSM_CNT,CCU_TX_BASE_ADDR,0x6,31,0},
};
const CHIP_REG CCU_TX_SSM_MESSAGE0_REG[]=
{
	{CCU_TX_SSM_MESSAGE0,CCU_TX_BASE_ADDR,0x7,31,0},
};
const CHIP_REG CCU_TX_SSM_MESSAGE1_REG[]=
{
	{CCU_TX_SSM_MESSAGE1,CCU_TX_BASE_ADDR,0x8,31,0},
};
const CHIP_REG CCU_TX_SSM_REQ_REG[]=
{
	{CCU_TX_SSM_REQ,CCU_TX_BASE_ADDR,0x9,0,0},
};
const CHIP_REG CCU_TX_SYNC_PORT_CNT_REG[]=
{
	{CCU_TX_SYNC_PORT_CNT,CCU_TX_BASE_ADDR,0xa,31,0},
};
const CHIP_REG CCU_TX_DELAY_REQ_PORT_CNT_REG[]=
{
	{CCU_TX_DELAY_REQ_PORT_CNT,CCU_TX_BASE_ADDR,0x12,31,0},
};
const CHIP_REG CCU_TX_PDELAY_REQ_PORT_CNT_REG[]=
{
	{CCU_TX_PDELAY_REQ_PORT_CNT,CCU_TX_BASE_ADDR,0x1a,31,0},
};
const CHIP_REG CCU_TX_PDELAY_RESP_PORT_CNT_REG[]=
{
	{CCU_TX_PDELAY_RESP_PORT_CNT,CCU_TX_BASE_ADDR,0x22,31,0},
};
const CHIP_REG CCU_TX_FOLLOW_UP_PORT_CNT_REG[]=
{
	{CCU_TX_FOLLOW_UP_PORT_CNT,CCU_TX_BASE_ADDR,0x2a,31,0},
};
const CHIP_REG CCU_TX_DELAY_RESP_PORT_CNT_REG[]=
{
	{CCU_TX_DELAY_RESP_PORT_CNT,CCU_TX_BASE_ADDR,0x32,31,0},
};
const CHIP_REG CCU_TX_PDELAY_FOLLOW_UP_PORT_CNT_REG[]=
{
	{CCU_TX_PDELAY_FOLLOW_UP_PORT_CNT,CCU_TX_BASE_ADDR,0x3a,31,0},
};
const CHIP_REG CCU_TX_ANNOUNCE_PORT_CNT_REG[]=
{
	{CCU_TX_ANNOUNCE_PORT_CNT,CCU_TX_BASE_ADDR,0x42,31,0},
};
const CHIP_REG CCU_TX_SIGNALING_PORT_CNT_REG[]=
{
	{CCU_TX_SIGNALING_PORT_CNT,CCU_TX_BASE_ADDR,0x4a,31,0},
};
const CHIP_REG CCU_TX_MANAGEMENT_PORT_CNT_REG[]=
{
	{CCU_TX_MANAGEMENT_PORT_CNT,CCU_TX_BASE_ADDR,0x52,31,0},
};
const CHIP_REG CCU_TX_RESV_PORT_CNT_REG[]=
{
	{CCU_TX_RESV_PORT_CNT,CCU_TX_BASE_ADDR,0x5a,31,0},
};
const CHIP_REG CCU_TX_SSM_PORT_CNT_REG[]=
{
	{CCU_TX_SSM_PORT_CNT,CCU_TX_BASE_ADDR,0x62,31,0},
};
const CHIP_REG CCU_TX_ENCODE_ERR_CNT_REG[]=
{
	{CCU_TX_ENCODE_ERR_CNT,CCU_TX_BASE_ADDR,0x70,31,0},
};
const CHIP_REG CCU_TX_ENCODE_ERR_ALM_REG[]=
{
	{CCU_TX_ENCODE_ERR_ALM,CCU_TX_BASE_ADDR,0x71,0,0},
	{CCU_TX_64_TO_640_FIFO_FULL,CCU_TX_BASE_ADDR,0x71,1,1},
	{CCU_TX_64_TO_640_ERR_ALM,CCU_TX_BASE_ADDR,0x71,2,2},
	{CCU_TX_OH_BUFF_ERR_ALM,CCU_TX_BASE_ADDR,0x71,3,3},
};
const CHIP_REG CCU_TX_INT_REG[]=
{
	{CCU_TX_ENCODE_ERR_INT,CCU_TX_BASE_ADDR,0x72,1,0},
	{CCU_TX_64_TO_640_FIFO_FULL_INT,CCU_TX_BASE_ADDR,0x72,3,2},
	{CCU_TX_64_TO_640_ERR_INT,CCU_TX_BASE_ADDR,0x72,5,4},
	{CCU_TX_OH_BUFF_ERR_INT,CCU_TX_BASE_ADDR,0x72,7,6},
};
const CHIP_REG CCU_TX_INT_MASK_REG[]=
{
	{CCU_TX_ENCODE_ERR_INT_MASK,CCU_TX_BASE_ADDR,0x73,1,0},
	{CCU_TX_64_TO_640_FIFO_FULL_INT_MASK,CCU_TX_BASE_ADDR,0x73,3,2},
	{CCU_TX_64_TO_640_ERR_INT_MASK,CCU_TX_BASE_ADDR,0x73,5,4},
	{CCU_TX_OH_BUFF_ERR_INT_MASK,CCU_TX_BASE_ADDR,0x73,7,6},
};

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_ssm_req_set
*
* DESCRIPTION
*
*	  ge rx ssm req cfg
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_ssm_req_set(UINT_8 chip_id)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_SSM_REQ_SET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    rv = regp_write(chip_id,CCU_RX_SSM_REQ_REG[CCU_RX_SSM_REQ].base_addr,CCU_RX_SSM_REQ_REG[CCU_RX_SSM_REQ].offset_addr,1);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_frame_cnt_get
*
* DESCRIPTION
*
*	  Ge rx frame cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_frame_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_FRAME_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_FRAME_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_FRAME_CNT_REG[CCU_RX_FRAME_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_drop_cnt_get
*
* DESCRIPTION
*
*	  Ge rx drop cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_drop_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_DROP_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_DROP_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_DROP_CNT_REG[CCU_RX_DROP_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_filter_cnt_get
*
* DESCRIPTION
*
*	  Ge rx filter cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_filter_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_FILTER_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_FILTER_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_FILTER_CNT_REG[CCU_RX_FILTER_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_frm_err_cnt_get
*
* DESCRIPTION
*
*	  Ge rx frm_err cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_frm_err_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_FRM_ERR_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_FRM_ERR_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_FRM_ERR_CNT_REG[CCU_RX_FRM_ERR_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_len_err_cnt_get
*
* DESCRIPTION
*
*	  Ge rx len_err cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_len_err_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_LEN_ERR_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_LEN_ERR_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_LEN_ERR_CNT_REG[CCU_RX_LEN_ERR_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_pkt_in_num_get
*
* DESCRIPTION
*
*	  Ge rx pkt_in_num cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_pkt_in_num_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_PKT_IN_NUM_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_PKT_IN_NUM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_PKT_IN_NUM_REG[CCU_RX_PKT_IN_NUM],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_sync_cnt_get
*
* DESCRIPTION
*
*	  Ge rx sync frame cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_sync_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_SYNC_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_SYNC_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_SYNC_CNT_REG[CCU_RX_SYNC_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_delay_req_cnt_get
*
* DESCRIPTION
*
*	  Ge rx delay req cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_delay_req_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_DELAY_REQ_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_DELAY_REQ_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_DELAY_REQ_CNT_REG[CCU_RX_DELAY_REQ_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_pdelay_req_cnt_get
*
* DESCRIPTION
*
*	  Ge rx pdelay req cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_pdelay_req_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_PDELAY_REQ_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_PDELAY_REQ_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_PDELAY_REQ_CNT_REG[CCU_RX_PDELAY_REQ_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_pdelay_resp_cnt_get
*
* DESCRIPTION
*
*	  Ge rx pdelay resp cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_pdelay_resp_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_PDELAY_RESP_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_PDELAY_RESP_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_PDELAY_RESP_CNT_REG[CCU_RX_PDELAY_RESP_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_follow_up_cnt_get
*
* DESCRIPTION
*
*	  Ge rx follow up cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_follow_up_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_FOLLOW_UP_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_FOLLOW_UP_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_FOLLOW_UP_CNT_REG[CCU_RX_FOLLOW_UP_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_delay_resp_cnt_get
*
* DESCRIPTION
*
*	  Ge rx delay resp cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_delay_resp_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_DELAY_RESP_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_DELAY_RESP_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_DELAY_RESP_CNT_REG[CCU_RX_DELAY_RESP_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_pdelay_follow_cnt_get
*
* DESCRIPTION
*
*	  Ge rx pdelay follow cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_pdelay_follow_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_PDELAY_FOLLOW_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_PDELAY_FOLLOW_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_PDELAY_FOLLOW_CNT_REG[CCU_RX_PDELAY_FOLLOW_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_announce_cnt_get
*
* DESCRIPTION
*
*	  Ge rx announce cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_announce_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_ANNOUNCE_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_ANNOUNCE_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_ANNOUNCE_CNT_REG[CCU_RX_ANNOUNCE_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_signaling_cnt_get
*
* DESCRIPTION
*
*	  Ge rx signaling cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_signaling_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_SIGNALING_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_SIGNALING_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_SIGNALING_CNT_REG[CCU_RX_SIGNALING_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_management_cnt_get
*
* DESCRIPTION
*
*	  Ge rx management cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_management_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_MANAGEMENT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_MANAGEMENT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_MANAGEMENT_CNT_REG[CCU_RX_MANAGEMENT_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_ssm_cnt_get
*
* DESCRIPTION
*
*	  Ge rx ssm cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_ssm_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_SSM_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_SSM_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_SSM_CNT_REG[CCU_RX_SSM_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_resv_cnt_get
*
* DESCRIPTION
*
*	  Ge rx resv cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_resv_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_RESV_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_RESV_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_RESV_CNT_REG[CCU_RX_RESV_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_decode_err_cnt_get
*
* DESCRIPTION
*
*	  Ge rx decode err cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_decode_err_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_DECODE_ERR_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_DECODE_ERR_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_DECODE_ERR_CNT_REG[CCU_RX_DECODE_ERR_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_decode_spkt_byte_cnt_get
*
* DESCRIPTION
*
*	  Ge rx decode spkt byte cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_decode_spkt_byte_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_DECODE_SPKT_BYTE_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_DECODE_SPKT_BYTE_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_DECODE_SPKT_BYTE_CNT_REG[CCU_RX_DECODE_SPKT_BYTE_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_decode_spkt_num_cnt_get
*
* DESCRIPTION
*
*	  Ge rx decode spkt num cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_decode_spkt_num_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_DECODE_SPKT_NUM_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_DECODE_SPKT_NUM_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_RX_DECODE_SPKT_NUM_CNT_REG[CCU_RX_DECODE_SPKT_NUM_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_ssm_message_get
*
* DESCRIPTION
*
*	  Ge rx ssm message
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to rx ssm message
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_ssm_message_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(port_id > 7))
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_SSM_MESSAGE_GET] invalid parameter!! \
													  \r\n chip_id = %d  ,\
													  \r\n port_id = %d \r\n",   
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_SSM_MESSAGE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if(port_id < 4)
	{
		rv = regp_bit_read(chip_id,CCU_RX_SSM_MESSAGE0_REG[CCU_RX_SSM_MESSAGE0].base_addr,CCU_RX_SSM_MESSAGE0_REG[CCU_RX_SSM_MESSAGE0].offset_addr,
			(port_id*8 + 7),port_id*8,parameter);

	}
	else
	{
		port_id -= 4;
		rv = regp_bit_read(chip_id,CCU_RX_SSM_MESSAGE1_REG[CCU_RX_SSM_MESSAGE1].base_addr,CCU_RX_SSM_MESSAGE1_REG[CCU_RX_SSM_MESSAGE1].offset_addr,
			(port_id*8 + 7),port_id*8,parameter);

	}
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_alarm_get
*
* DESCRIPTION
*
*	  Ge rx alarm
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3 0:CCU_RX_INPORT_FIFO_FULL 1:CCU_RX_OVER_LENGTH
*			       2:CCU_RX_DECODE_ERR 3:CCU_RX_OH_BUFF_FULL
*				   4:CCU_RX_DECODE_EOP_ABNOR   5:CCU_RX_DECODE_REMOTE_FAULT 
*				   6:CCU_RX_DECODE_LOCAL_FAULT 7:CCU_RX_LEN_MISMATCH
*				   8:CCU_RX_INPUT_NO_EOP
**	  parameter*: pointer to rx ssm message
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_alarm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_ALARM_GET] invalid parameter!! \
													  \r\n chip_id = %d\r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_ALARM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_ALARM_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_ALARM_GET] out of array range\r\n");
		}
	
		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_ALARM_REG[field_id],parameter);

	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_alarm_all_get
*
* DESCRIPTION
*
*	  Ge rx alarm all
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to rx ssm message
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_alarm_all_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_ALARM_ALL_GET] invalid parameter!! \
													  \r\n chip_id = %d\r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_ALARM_ALL_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
	
	rv = regp_read(chip_id,CCU_RX_ALARM_REG[CCU_RX_INPORT_FIFO_FULL].base_addr,CCU_RX_ALARM_REG[CCU_RX_INPORT_FIFO_FULL].offset_addr,parameter);

	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_int_set
*
* DESCRIPTION
*
*	  ge rx int cfg
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3 0:CCU_RX_INPORT_FIFO_FULL_INT 1:CCU_RX_OVER_LENGTH_INT
*				   2:CCU_RX_DECODE_ERR_INT       3:CCU_RX_OH_BUFF_FULL_INT 4:CCU_RX_DECODE_EOP_ABNOR_INT
*				   5:CCU_RX_INPUT_NO_EOP_INT
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_INT_SET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_INT_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_INT_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
    rv = regp_field_write(chip_id,CCU_RX_INT_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_int_get
*
* DESCRIPTION
*
*	  Ge rx int
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3 0:CCU_RX_INPORT_FIFO_FULL_INT 1:CCU_RX_OVER_LENGTH_INT
*				   2:CCU_RX_DECODE_ERR_INT       3:CCU_RX_OH_BUFF_FULL_INT 4:CCU_RX_DECODE_EOP_ABNOR_INT
*				   5:CCU_RX_INPUT_NO_EOP_INT
**	  parameter*: pointer to rx ssm message
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_INT_GET] invalid parameter!! \
													  \r\n chip_id = %d  ,\
													  \r\n field_id = %d \r\n",   
													  chip_id,
													  field_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_INT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_INT_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_INT_GET] out of array range\r\n");
		}
	
		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_INT_REG[field_id],parameter);

	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_int_mask_set
*
* DESCRIPTION
*
*	  ge rx int mask cfg
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3 0:CCU_RX_INPORT_FIFO_FULL_INT_MASK 1:CCU_RX_OVER_LENGTH_INT_MASK
*				   2:CCU_RX_DECODE_ERR_INT_MASK       3:CCU_RX_OH_BUFF_FULL_INT_MASK 4:CCU_RX_DECODE_EOP_ABNOR_INT_MASK
*				   5:CCU_RX_INPUT_NO_EOP_INT_MASK
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_INT_MASK_SET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_INT_MASK_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_INT_MASK_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
    rv = regp_field_write(chip_id,CCU_RX_INT_MASK_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_int_mask_get
*
* DESCRIPTION
*
*	  Ge rx int_mask
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~3 0:CCU_RX_INPORT_FIFO_FULL_INT_MASK 1:CCU_RX_OVER_LENGTH_INT_MASK
*				   2:CCU_RX_DECODE_ERR_INT_MASK       3:CCU_RX_OH_BUFF_FULL_INT_MASK 4:CCU_RX_DECODE_EOP_ABNOR_INT_MASK
*				   5:CCU_RX_INPUT_NO_EOP_INT_MASK
**	  parameter*: pointer to rx ssm message
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_RX_INT_MASK_GET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_INT_MASK_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_RX_INT_MASK_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_INT_MASK_GET] out of array range\r\n");
		}
	
		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_RX_INT_MASK_REG[field_id],parameter);

	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_ssm_req_set
*
* DESCRIPTION
*
*	  ge tx ssm req cfg
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_ssm_req_set(UINT_8 chip_id)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_SSM_REQ_SET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

    rv = regp_write(chip_id,CCU_TX_SSM_REQ_REG[CCU_TX_SSM_REQ].base_addr,CCU_TX_SSM_REQ_REG[CCU_TX_SSM_REQ].offset_addr,1);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_frame_cnt_get
*
* DESCRIPTION
*
*	  Ge tx frame cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_frame_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_FRAME_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_FRAME_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_FRAME_CNT_REG[CCU_TX_FRAME_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_drop_cnt_get
*
* DESCRIPTION
*
*	  Ge tx drop cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_drop_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_DROP_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_DROP_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_DROP_CNT_REG[CCU_TX_DROP_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_filter_cnt_get
*
* DESCRIPTION
*
*	  Ge tx filter cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_filter_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_FILTER_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_FILTER_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_FILTER_CNT_REG[CCU_TX_FILTER_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_err_cnt_get
*
* DESCRIPTION
*
*	  Ge tx err_cnt cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_err_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_ERR_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ERR_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_ERR_CNT_REG[CCU_TX_ERR_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_len_err_cnt_get
*
* DESCRIPTION
*
*	  Ge tx len_err_cnt cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_len_err_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_LEN_ERR_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_LEN_ERR_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_LEN_ERR_CNT_REG[CCU_TX_LEN_ERR_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_ptp_cnt_get
*
* DESCRIPTION
*
*	  Ge tx ptp cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_ptp_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_PTP_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_PTP_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_PTP_CNT_REG[CCU_TX_PTP_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_ssm_cnt_get
*
* DESCRIPTION
*
*	  Ge tx ssm cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_ssm_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_SSM_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_SSM_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_SSM_CNT_REG[CCU_TX_SSM_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_sync_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx sync frame cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_sync_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_SYNC_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_SYNC_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_SYNC_PORT_CNT_REG[CCU_TX_SYNC_PORT_CNT].base_addr,(CCU_TX_SYNC_PORT_CNT_REG[CCU_TX_SYNC_PORT_CNT].offset_addr + port_id),parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_delay_req_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx delay req cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_delay_req_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_DELAY_REQ_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_DELAY_REQ_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_DELAY_REQ_PORT_CNT_REG[CCU_TX_DELAY_REQ_PORT_CNT].base_addr,(CCU_TX_DELAY_REQ_PORT_CNT_REG[CCU_TX_DELAY_REQ_PORT_CNT].offset_addr + port_id),parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_pdelay_req_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx pdelay req cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_pdelay_req_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_PDELAY_REQ_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_PDELAY_REQ_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_PDELAY_REQ_PORT_CNT_REG[CCU_TX_PDELAY_REQ_PORT_CNT].base_addr,CCU_TX_PDELAY_REQ_PORT_CNT_REG[CCU_TX_PDELAY_REQ_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_pdelay_resp_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx pdelay resp cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_pdelay_resp_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_PDELAY_RESP_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_PDELAY_RESP_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_PDELAY_RESP_PORT_CNT_REG[CCU_TX_PDELAY_RESP_PORT_CNT].base_addr,CCU_TX_PDELAY_RESP_PORT_CNT_REG[CCU_TX_PDELAY_RESP_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_follow_up_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx follow up cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_follow_up_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_FOLLOW_UP_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_FOLLOW_UP_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_FOLLOW_UP_PORT_CNT_REG[CCU_TX_FOLLOW_UP_PORT_CNT].base_addr,CCU_TX_FOLLOW_UP_PORT_CNT_REG[CCU_TX_FOLLOW_UP_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_delay_resp_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx delay resp cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_delay_resp_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_DELAY_RESP_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_DELAY_RESP_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_DELAY_RESP_PORT_CNT_REG[CCU_TX_DELAY_RESP_PORT_CNT].base_addr,CCU_TX_DELAY_RESP_PORT_CNT_REG[CCU_TX_DELAY_RESP_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_pdelay_follow_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx pdelay follow cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_pdelay_follow_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_PDELAY_FOLLOW_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_PDELAY_FOLLOW_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_PDELAY_FOLLOW_UP_PORT_CNT_REG[CCU_TX_PDELAY_FOLLOW_UP_PORT_CNT].base_addr,CCU_TX_PDELAY_FOLLOW_UP_PORT_CNT_REG[CCU_TX_PDELAY_FOLLOW_UP_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_announce_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx announce cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_announce_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_ANNOUNCE_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ANNOUNCE_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_ANNOUNCE_PORT_CNT_REG[CCU_TX_ANNOUNCE_PORT_CNT].base_addr,CCU_TX_ANNOUNCE_PORT_CNT_REG[CCU_TX_ANNOUNCE_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_signaling_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx signaling cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_signaling_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_SIGNALING_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_SIGNALING_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_SIGNALING_PORT_CNT_REG[CCU_TX_SIGNALING_PORT_CNT].base_addr,CCU_TX_SIGNALING_PORT_CNT_REG[CCU_TX_SIGNALING_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_management_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx management cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_management_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_MANAGEMENT_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_MANAGEMENT_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_MANAGEMENT_PORT_CNT_REG[CCU_TX_MANAGEMENT_PORT_CNT].base_addr,CCU_TX_MANAGEMENT_PORT_CNT_REG[CCU_TX_MANAGEMENT_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_resv_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx_resv_port_cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_resv_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_RESV_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_RESV_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_RESV_PORT_CNT_REG[CCU_TX_RESV_PORT_CNT].base_addr,CCU_TX_RESV_PORT_CNT_REG[CCU_TX_RESV_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_ssm_port_cnt_get
*
* DESCRIPTION
*
*	  Ge tx_ssm_port_cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_ssm_port_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_SSM_PORT_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_SSM_PORT_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_read(chip_id,CCU_TX_SSM_PORT_CNT_REG[CCU_TX_SSM_PORT_CNT].base_addr,CCU_TX_SSM_PORT_CNT_REG[CCU_TX_SSM_PORT_CNT].offset_addr + port_id,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_encode_err_cnt_get
*
* DESCRIPTION
*
*	  Ge tx encode err cnt
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to cnt
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_encode_err_cnt_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_ENCODE_ERR_CNT_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	rv = regp_field_read(chip_id,CCU_TX_ENCODE_ERR_CNT_REG[CCU_TX_ENCODE_ERR_CNT],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_ssm_message_get
*
* DESCRIPTION
*
*	  Ge tx ssm message
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  port_id:0~7
**	  parameter*: pointer to tx ssm message
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_ssm_message_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(port_id > 7))
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_SSM_MESSAGE_GET] invalid parameter!! \
													  \r\n chip_id = %d  ,\
													  \r\n port_id = %d \r\n",   
													  chip_id,
													  port_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_SSM_MESSAGE_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if(port_id < 4)
	{
		rv = regp_bit_read(chip_id,CCU_TX_SSM_MESSAGE0_REG[CCU_TX_SSM_MESSAGE0].base_addr,CCU_TX_SSM_MESSAGE0_REG[CCU_TX_SSM_MESSAGE0].offset_addr,
			(port_id*8 + 7),port_id*8,parameter);

	}
	else
	{
		port_id -= 4;
		rv = regp_bit_read(chip_id,CCU_TX_SSM_MESSAGE1_REG[CCU_TX_SSM_MESSAGE1].base_addr,CCU_TX_SSM_MESSAGE1_REG[CCU_TX_SSM_MESSAGE1].offset_addr,
			(port_id*8 + 7),port_id*8,parameter);

	}
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_encode_err_alm_get
*
* DESCRIPTION
*
*	  Ge tx encode err alm
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id:0~1  0:CCU_TX_ENCODE_ERR_ALM  1:CCU_TX_64_TO_640_FIFO_FULL 
*					2:CCU_TX_64_TO_640_ERR_ALM 3:CCU_TX_OH_BUFF_ERR_ALM
**	  parameter*: pointer to alm
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_encode_err_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_ENCODE_ERR_ALM_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_ALM_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_ENCODE_ERR_ALM_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_ALM_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_ENCODE_ERR_ALM_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_encode_err_alm_all_get
*
* DESCRIPTION
*
*	  Ge tx encode err alm all
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
**	  parameter*: pointer to alm
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_encode_err_alm_all_get(UINT_8 chip_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_ENCODE_ERR_ALM_ALL_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_ALM_ALL_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
	
	rv = regp_read(chip_id,CCU_TX_ENCODE_ERR_ALM_REG[CCU_TX_ENCODE_ERR_ALM].base_addr,CCU_TX_ENCODE_ERR_ALM_REG[CCU_TX_ENCODE_ERR_ALM].offset_addr,parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_encode_err_int_set
*
* DESCRIPTION
*
*	  ge tx encode err int  cfg
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~1 0:CCU_TX_ENCODE_ERR_INT  1:CCU_TX_64_TO_640_FIFO_FULL_INT
*					2:CCU_TX_64_TO_640_ERR_INT 3:CCU_TX_OH_BUFF_ERR_INT
*
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_encode_err_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_ENCODE_ERR_INT_SET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_INT_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_INT_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
    rv = regp_field_write(chip_id,CCU_TX_INT_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_encode_err_int_get
*
* DESCRIPTION
*
*	  Ge tx encode err int cfg
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~1 0:CCU_TX_ENCODE_ERR_INT  1:CCU_TX_64_TO_640_FIFO_FULL_INT
*					2:CCU_TX_64_TO_640_ERR_INT 3:CCU_TX_OH_BUFF_ERR_INT
**	  parameter*: pointer 
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_encode_err_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_ENCODE_ERR_INT_GET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_INT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_INT_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_INT_MASK_GET] out of array range\r\n");
		}
	
		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_INT_REG[field_id],parameter);

	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_encode_err_int_mask_set
*
* DESCRIPTION
*
*	  ge tx encode err int mask cfg
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~1  0:CCU_TX_ENCODE_ERR_INT_MASK  1:CCU_TX_64_TO_640_FIFO_FULL_INT_MASK
*					2:CCU_TX_64_TO_640_ERR_INT_MASK 3:CCU_TX_OH_BUFF_ERR_INT_MASK
*
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_encode_err_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_ENCODE_ERR_INT_MASK_SET] invalid parameter!! \
													  \r\n chip_id = %d \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_INT_MASK_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_INT_MASK_SET] out of array range\r\n");
		}

		return RET_PARAERR;
	}
	
    rv = regp_field_write(chip_id,CCU_TX_INT_MASK_REG[field_id],parameter);
	
	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_encode_err_int_mask_get
*
* DESCRIPTION
*
*	  Ge tx encode err int mask cfg
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  field_id: 0~1  0:CCU_TX_ENCODE_ERR_INT_MASK  1:CCU_TX_64_TO_640_FIFO_FULL_INT_MASK
*					2:CCU_TX_64_TO_640_ERR_INT_MASK 3:CCU_TX_OH_BUFF_ERR_INT_MASK
**	  parameter*: pointer to rx ssm message
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 dengj       2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_encode_err_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

	/* check parameter whether valid */
	if (chip_id > MAX_DEV)
	{
	   if (ccu_rx_tx_para_debug)
	   {
		   printf("[CCU_TX_ENCODE_ERR_INT_MASK_GET] invalid parameter!! \
													  \r\n chip_id = %d  \r\n",   
													  chip_id);
	   }

	   return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_INT_MASK_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(CCU_TX_INT_MASK_REG))
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_ENCODE_ERR_INT_MASK_GET] out of array range\r\n");
		}
	
		return RET_PARAERR;
	}
	
	rv = regp_field_read(chip_id,CCU_TX_INT_MASK_REG[field_id],parameter);

	return rv;
}

/******************************************************************************
*
* FUNCTION
*
*	  ccu_rx_cnt_get
*
* DESCRIPTION
*
*	  Ccu rx cnt.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  cnt_type: 0~16
*	  parameter: vid value
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 taos    2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_rx_cnt_get(UINT_8 chip_id,UINT_8 cnt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;
	
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(cnt_type > 16))
	{
	    if (ccu_rx_tx_para_debug)
	    {
		    printf("[CCU_RX_CNT_GET] invalid parameter!! \
		 											  \r\n chip_id = %d, \
		 											  \r\n cnt_type = %d\r\n",   
		 											  chip_id,
		 											  cnt_type);
	    }
        
	    return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_RX_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}
    memset(&p, 0, sizeof(CHIP_REG));
    switch(cnt_type)
    {
        case 0 : field_id = CCU_RX_FRAME_CNT;       p = CCU_RX_FRAME_CNT_REG[field_id];   break;
        case 1 : field_id = CCU_RX_DROP_CNT;        p = CCU_RX_DROP_CNT_REG[field_id];   break;
        case 2 : field_id = CCU_RX_FILTER_CNT;      p = CCU_RX_FILTER_CNT_REG[field_id];  break;
        case 3 : field_id = CCU_RX_FRM_ERR_CNT;     p = CCU_RX_FRM_ERR_CNT_REG[field_id];  break;
        case 4 : field_id = CCU_RX_LEN_ERR_CNT;     p = CCU_RX_LEN_ERR_CNT_REG[field_id];  break;
        case 5 : field_id = CCU_RX_SYNC_CNT;        p = CCU_RX_SYNC_CNT_REG[field_id];  break;
        case 6 : field_id = CCU_RX_DELAY_REQ_CNT;   p = CCU_RX_DELAY_REQ_CNT_REG[field_id];  break;
        case 7 : field_id = CCU_RX_PDELAY_REQ_CNT;  p = CCU_RX_PDELAY_REQ_CNT_REG[field_id];  break;
		case 8 : field_id = CCU_RX_PDELAY_RESP_CNT; p = CCU_RX_PDELAY_RESP_CNT_REG[field_id];   break;
        case 9 : field_id = CCU_RX_FOLLOW_UP_CNT;   p = CCU_RX_FOLLOW_UP_CNT_REG[field_id];   break;
        case 10 : field_id = CCU_RX_DELAY_RESP_CNT; p = CCU_RX_DELAY_RESP_CNT_REG[field_id];  break;
        case 11 : field_id = CCU_RX_PDELAY_FOLLOW_CNT;  p = CCU_RX_PDELAY_FOLLOW_CNT_REG[field_id];  break;
        case 12 : field_id = CCU_RX_ANNOUNCE_CNT;       p = CCU_RX_ANNOUNCE_CNT_REG[field_id];  break;
        case 13 : field_id = CCU_RX_SIGNALING_CNT;      p = CCU_RX_SIGNALING_CNT_REG[field_id];  break;
        case 14 : field_id = CCU_RX_MANAGEMENT_CNT;     p = CCU_RX_MANAGEMENT_CNT_REG[field_id];  break;
        case 15 : field_id = CCU_RX_SSM_CNT;            p = CCU_RX_SSM_CNT_REG[field_id];  break;
		case 16 : field_id = CCU_RX_RESV_CNT;           p = CCU_RX_RESV_CNT_REG[field_id];  break;
        default: break;
    }

    rv = regp_field_read(chip_id,p,parameter);
    
    return rv;
}

#if 0
/******************************************************************************
*
* FUNCTION
*
*	  ccu_tx_cnt_get
*
* DESCRIPTION
*
*	  Ccu tx cnt.
*
* NOTE
*
*	  RESERVED
*
* PARAMETERS
*
*	  chip_id: chip number used
*	  cnt_type: 0~16
*	  parameter: vid value
* RETURNS
*
*	  0: success
*	  1: fail
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
*	 taos    2017-10-25	 1.0		   initial
*
******************************************************************************/
RET_STATUS ccu_tx_cnt_get(UINT_8 chip_id,UINT_8 cnt_type,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;
	CHIP_REG p;
	UINT_8 field_id = 0;
	
	/* check parameter whether valid */
	if ((chip_id > MAX_DEV) ||
		(cnt_type > 16))
	{
	    if (ccu_rx_tx_para_debug)
	    {
		    printf("[CCU_TX_CNT_GET] invalid parameter!! \
		 											  \r\n chip_id = %d, \
		 											  \r\n cnt_type = %d\r\n",   
		 											  chip_id,
		 											  cnt_type);
	    }
        
	    return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_CNT_GET] invalid parameter pointer\r\n");
		}
		return RET_PARAERR;
	}

    switch(cnt_type)
    {
        case 0 : field_id = CCU_TX_FRAME_CNT;       p = CCU_TX_FRAME_CNT_REG[field_id];   break;
        case 1 : field_id = CCU_TX_DROP_CNT;        p = CCU_TX_DROP_CNT_REG[field_id];   break;
        case 2 : field_id = CCU_TX_FILTER_CNT;      p = CCU_TX_FILTER_CNT_REG[field_id];  break;
        case 3 : field_id = CCU_TX_ERR_CNT;         p = CCU_TX_ERR_CNT_REG[field_id];  break;
        case 4 : field_id = CCU_TX_LEN_ERR_CNT;     p = CCU_TX_LEN_ERR_CNT_REG[field_id];  break;
        case 5 : field_id = CCU_TX_SYNC_CNT;        p = CCU_TX_SYNC_CNT_REG[field_id];  break;
        case 6 : field_id = CCU_TX_DELAY_REQ_CNT;   p = CCU_TX_DELAY_REQ_CNT_REG[field_id];  break;
        case 7 : field_id = CCU_TX_PDELAY_REQ_CNT;  p = CCU_TX_PDELAY_REQ_CNT_REG[field_id];  break;
		case 8 : field_id = CCU_TX_PDELAY_RESP_CNT; p = CCU_TX_PDELAY_RESP_CNT_REG[field_id];   break;
        case 9 : field_id = CCU_TX_FOLLOW_UP_CNT;   p = CCU_TX_FOLLOW_UP_CNT_REG[field_id];   break;
        case 10 : field_id = CCU_TX_DELAY_RESP_CNT; p = CCU_TX_DELAY_RESP_CNT_REG[field_id];  break;
        case 11 : field_id = CCU_TX_PDELAY_FOLLOW_UP_CNT;  p = CCU_TX_PDELAY_FOLLOW_UP_CNT_REG[field_id];  break;
        case 12 : field_id = CCU_TX_ANNOUNCE_CNT;          p = CCU_TX_ANNOUNCE_CNT_REG[field_id];  break;
        case 13 : field_id = CCU_TX_SIGNALING_CNT;         p = CCU_TX_SIGNALING_CNT_REG[field_id];  break;
        case 14 : field_id = CCU_TX_MANAGEMENT_CNT;        p = CCU_TX_MANAGEMENT_CNT_REG[field_id];  break;
        case 15 : field_id = CCU_TX_SSM_CNT;               p = CCU_TX_SSM_CNT_REG[field_id];  break;
		case 16 : field_id = CCU_TX_RESV_CNT;              p = CCU_TX_RESV_CNT_REG[field_id];  break;
        default: break;
    }

	if (field_id > 1)
	{
		if (ccu_rx_tx_para_debug)
		{
			printf("[CCU_TX_CNT_GET] out of array range\r\n");
		}

		return RET_PARAERR;
	}

    rv = regp_field_read(chip_id,p,parameter);
    
    return rv;
}
#endif
