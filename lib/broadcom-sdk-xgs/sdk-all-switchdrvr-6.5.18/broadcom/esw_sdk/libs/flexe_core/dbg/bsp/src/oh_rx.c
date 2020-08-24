

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
#include "oh_rx.h"
#include "oh_tx.h"
#include "flexe_multi_deskew.h"

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int flexe_oh_rx_para_debug = 0;

const CHIP_REG FLEXE_OH_LOOPBACK_CFG_REG[]=
{
	{TEST_EN,FLEXE_OH_RX_BASE_ADDR,0x0,0,0},
	{OHRX_DIN_SRC_CFG,FLEXE_OH_RX_BASE_ADDR,0x0,1,1},
	{MUX2OHTX_CP_CFG,FLEXE_OH_RX_BASE_ADDR,0x0,2,2},
};
const CHIP_REG FLEXE_RX_MODE_CFG_REG[]=
{
	{FLEXE_RX_MODE,FLEXE_OH_RX_BASE_ADDR,0x1,1,0},
	{MF16T32_RX_CFG,FLEXE_OH_RX_BASE_ADDR,0x1,2,2},
	{OMF_CHK_CTRL,FLEXE_OH_RX_BASE_ADDR,0x1,3,3},
	{OMF_CRC_CTRL,FLEXE_OH_RX_BASE_ADDR,0x1,4,4},
};
const CHIP_REG ALM_EN_CFG_REG[]=
{
	{ALM_EN,FLEXE_OH_RX_BASE_ADDR,0x2,30,0},
	{CRC_RX_MODE,FLEXE_OH_RX_BASE_ADDR,0x2,31,31},
};
const CHIP_REG EXCC_OH_CFG_REG[]=
{
	{EXCCC,FLEXE_OH_RX_BASE_ADDR,0x3,0,0},
	{EXSC,FLEXE_OH_RX_BASE_ADDR,0x3,1,1},
};
const CHIP_REG EXGID_OH_CFG_REG[]=
{
	{EXGID,FLEXE_OH_RX_BASE_ADDR,0x4,19,0},
	{EXPID,FLEXE_OH_RX_BASE_ADDR,0x4,27,20},
};
const CHIP_REG FAS_CHK_CFG_REG[]=
{
	{OOF_SET_CFG,FLEXE_OH_RX_BASE_ADDR,0x5,3,0},
	{OOF_CLR_CFG,FLEXE_OH_RX_BASE_ADDR,0x5,7,4},
	{LOF_SET_CFG,FLEXE_OH_RX_BASE_ADDR,0x5,12,8},
	{LOF_CLR_CFG,FLEXE_OH_RX_BASE_ADDR,0x5,20,16},
	{FP_PATTERN_CFG,FLEXE_OH_RX_BASE_ADDR,0x5,22,21},
};
const CHIP_REG OMF_CHK_CFG_REG[]=
{
	{OOM_SET_CFG,FLEXE_OH_RX_BASE_ADDR,0x6,4,0},
	{OOM_CLR_CFG,FLEXE_OH_RX_BASE_ADDR,0x6,9,5},
	{LOM_SET_CFG,FLEXE_OH_RX_BASE_ADDR,0x6,14,10},
	{LOM_CLR_CFG,FLEXE_OH_RX_BASE_ADDR,0x6,20,16},
};
const CHIP_REG PAD_CHK_CFG_REG[]=
{
	{OOP_SET_CFG,FLEXE_OH_RX_BASE_ADDR,0x7,3,0},
	{OOP_CLR_CFG,FLEXE_OH_RX_BASE_ADDR,0x7,7,4},
	{LOP_SET_CFG,FLEXE_OH_RX_BASE_ADDR,0x7,12,8},
	{LOP_CLR_CFG,FLEXE_OH_RX_BASE_ADDR,0x7,20,16},
};
const CHIP_REG PAD_ALM_EN_REG[]=
{
	{PAD_ALM_EN,FLEXE_OH_RX_BASE_ADDR,0x8,9,0},
	{PCS_LF_EN,FLEXE_OH_RX_BASE_ADDR,0x8,10,10},
	{CRC_ALM_EN,FLEXE_OH_RX_BASE_ADDR,0x8,11,11},
};
const CHIP_REG CC_Rx_VALUE_REG[]=
{
	{ACCCC,FLEXE_OH_RX_BASE_ADDR,0x10,0,0},
	{ACCR,FLEXE_OH_RX_BASE_ADDR,0x10,1,1},
	{ACCA,FLEXE_OH_RX_BASE_ADDR,0x10,2,2},
	{ACSC,FLEXE_OH_RX_BASE_ADDR,0x10,3,3},
};
const CHIP_REG ID_Rx_VALUE_REG[]=
{
	{ACGID,FLEXE_OH_RX_BASE_ADDR,0x11,19,0},
	{ACPID,FLEXE_OH_RX_BASE_ADDR,0x11,27,20},
};
const CHIP_REG MANAGEMENT_CHANNEL_SECTION_REG[]=
{
	{ACMC_SETION_0,FLEXE_OH_RX_BASE_ADDR,0x12,31,0},
	{ACMC_SETION_1,FLEXE_OH_RX_BASE_ADDR,0x13,31,0},
	{ACMC_SETION_2,FLEXE_OH_RX_BASE_ADDR,0x14,31,0},
	{ACMC_SETION_3,FLEXE_OH_RX_BASE_ADDR,0x15,31,0},
};
const CHIP_REG MANAGEMENT_CHANNEL_S2S_REG[]=
{
	{AC_SHIM2SHIM_0,FLEXE_OH_RX_BASE_ADDR,0x16,31,0},
	{AC_SHIM2SHIM_1,FLEXE_OH_RX_BASE_ADDR,0x17,31,0},
	{AC_SHIM2SHIM_2,FLEXE_OH_RX_BASE_ADDR,0x18,31,0},
	{AC_SHIM2SHIM_3,FLEXE_OH_RX_BASE_ADDR,0x19,31,0},
	{AC_SHIM2SHIM_4,FLEXE_OH_RX_BASE_ADDR,0x1a,31,0},
	{AC_SHIM2SHIM_5,FLEXE_OH_RX_BASE_ADDR,0x1b,31,0},
};
const CHIP_REG RESERVED_Rx_VALUE_REG[]=
{
	{AC_RESERVED1_0,FLEXE_OH_RX_BASE_ADDR,0x1e,31,4},
	{AC_RESERVED1_1,FLEXE_OH_RX_BASE_ADDR,0x1f,18,0},	
	{AC_RESERVED2,FLEXE_OH_RX_BASE_ADDR,0x1f,31,19},
};
const CHIP_REG OH_Rx_ALM_REG[]=
{
	{OOF_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,0,0},
	{LOF_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,1,1},
	{OOM_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,2,2},
	{LOM_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,3,3},
	{CRC_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,4,4},
	{GIDM_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,5,5},
	{PMM_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,7,7},
	{RPF_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,8,8},
	{SCM_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,9,9},
	{OOP_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,10,10},
	{LOP_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,11,11},
	{PIDM_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,6,6},
	{OH1_BLOCK_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,12,12},
	{C_BIT_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,13,13},
	{GID_UNEQ_ALM,FLEXE_OH_RX_BASE_ADDR,0x20,14,14},
};
const CHIP_REG CCM_Rx_ALM_REG[]=
{
	{CCM,FLEXE_OH_RX_BASE_ADDR,0x21,19,0},
};
const CHIP_REG oh_alm_INT_REG[]=
{
	{LOF_ALM_INT,FLEXE_OH_RX_BASE_ADDR,0x30,1,0},
	{LOM_ALM_INT,FLEXE_OH_RX_BASE_ADDR,0x30,3,2},
	{GIDM_ALM_INT,FLEXE_OH_RX_BASE_ADDR,0x30,5,4},
	{PIDM_ALM_INT,FLEXE_OH_RX_BASE_ADDR,0x30,7,6},
	{PMM_ALM_INT,FLEXE_OH_RX_BASE_ADDR,0x30,9,8},
	{RPF_ALM_INT,FLEXE_OH_RX_BASE_ADDR,0x30,11,10},
	{DESKEW_ALM_INT,FLEXE_OH_RX_BASE_ADDR,0x30,13,12},
	{CCM_ALM0_INT,FLEXE_OH_RX_BASE_ADDR,0x30,15,14},
	{CCM_ALM1_INT,FLEXE_OH_RX_BASE_ADDR,0x30,17,16},
	{CCM_ALM2_INT,FLEXE_OH_RX_BASE_ADDR,0x30,19,18},
	{CCM_ALM3_INT,FLEXE_OH_RX_BASE_ADDR,0x30,21,20},
	{CCM_ALM4_INT,FLEXE_OH_RX_BASE_ADDR,0x30,23,22},
	{CCM_ALM5_INT,FLEXE_OH_RX_BASE_ADDR,0x30,25,24},
	{CCM_ALM6_INT,FLEXE_OH_RX_BASE_ADDR,0x30,27,26},
	{CCM_ALM7_INT,FLEXE_OH_RX_BASE_ADDR,0x30,29,28},
	{CCM_ALM8_INT,FLEXE_OH_RX_BASE_ADDR,0x30,31,30},
	{CCM_ALM9_INT,FLEXE_OH_RX_BASE_ADDR,0x31,1,0},
	{CCM_ALM10_INT,FLEXE_OH_RX_BASE_ADDR,0x31,3,2},
	{CCM_ALM11_INT,FLEXE_OH_RX_BASE_ADDR,0x31,5,4},
	{CCM_ALM12_INT,FLEXE_OH_RX_BASE_ADDR,0x31,7,6},
	{CCM_ALM13_INT,FLEXE_OH_RX_BASE_ADDR,0x31,9,8},
	{CCM_ALM14_INT,FLEXE_OH_RX_BASE_ADDR,0x31,11,10},
	{CCM_ALM15_INT,FLEXE_OH_RX_BASE_ADDR,0x31,13,12},
	{CCM_ALM16_INT,FLEXE_OH_RX_BASE_ADDR,0x31,15,14},
	{CCM_ALM17_INT,FLEXE_OH_RX_BASE_ADDR,0x31,17,16},
	{CCM_ALM18_INT,FLEXE_OH_RX_BASE_ADDR,0x31,19,18},
	{CCM_ALM19_INT,FLEXE_OH_RX_BASE_ADDR,0x31,21,20},
	{CR_STA_INT,FLEXE_OH_RX_BASE_ADDR,0x31,23,22},
	{CA_STA_INT,FLEXE_OH_RX_BASE_ADDR,0x31,25,24},
};
const CHIP_REG oh_alm_INT_MASK_REG[]=
{
	{LOF_ALM_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,1,0},
	{LOM_ALM_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,3,2},
	{GIDM_ALM_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,5,4},
	{PIDM_ALM_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,7,6},
	{PMM_ALM_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,9,8},
	{RPF_ALM_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,11,10},
	{DESKEW_ALM_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,13,12},
	{CCM_ALM0_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,15,14},
	{CCM_ALM1_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,17,16},
	{CCM_ALM2_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,19,18},
	{CCM_ALM3_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,21,20},
	{CCM_ALM4_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,23,22},
	{CCM_ALM5_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,25,24},
	{CCM_ALM6_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,27,26},
	{CCM_ALM7_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,29,28},
	{CCM_ALM8_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x32,31,30},
	{CCM_ALM9_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,1,0},
	{CCM_ALM10_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,3,2},
	{CCM_ALM11_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,5,4},
	{CCM_ALM12_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,7,6},
	{CCM_ALM13_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,9,8},
	{CCM_ALM14_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,11,10},
	{CCM_ALM15_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,13,12},
	{CCM_ALM16_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,15,14},
	{CCM_ALM17_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,17,16},
	{CCM_ALM18_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,19,18},
	{CCM_ALM19_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,21,20},
	{CR_STA_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,23,22},
	{CA_STA_INT_MASK,FLEXE_OH_RX_BASE_ADDR,0x33,25,24},
};
const CHIP_RAM PHYMAP_Rx = 
{
	TABLE_BASE_ADDR,FLEXE_OH_RX_BASE_ADDR+0x25,8,
};
const CHIP_RAM PHYMAP_Rx_EXP = 
{
	TABLE_BASE_ADDR,FLEXE_OH_RX_BASE_ADDR+0x26,8,
};
const CHIP_RAM CCAB_Rx = 
{
	TABLE_BASE_ADDR,FLEXE_OH_RX_BASE_ADDR+0x27,32,
};
const CHIP_RAM CCA_Rx_EXP = 
{
	TABLE_BASE_ADDR,FLEXE_OH_RX_BASE_ADDR+0x28,16,
};
const CHIP_RAM CCB_Rx_EXP = 
{
	TABLE_BASE_ADDR,FLEXE_OH_RX_BASE_ADDR+0x29,16,
};

/******************************************************************************
*
* FUNCTION	oh_rx_flexe_oh_loopback_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		flexe_id:0~7
*		field_id:0~MUX2OHTX_CP_CFG
*		parameter:flexe instance select 0:50G  1:100G 2:200G 3:400G
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_flexe_oh_loopback_cfg_set(UINT_8 chip_id ,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) ||
		(flexe_id > (PHY_NUM-1))||
		(parameter > 0x1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n flexe_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   __FUNCTION__,
													   chip_id,
													   flexe_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_OH_LOOPBACK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
				
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,FLEXE_OH_LOOPBACK_CFG_REG[field_id].base_addr,FLEXE_OH_LOOPBACK_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		FLEXE_OH_LOOPBACK_CFG_REG[field_id].end_bit,FLEXE_OH_LOOPBACK_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_flexe_oh_loopback_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		flexe_id:0~7
*		field_id:0~MUX2OHTX_CP_CFG
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_flexe_oh_loopback_cfg_get(UINT_8 chip_id ,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d,flexe_id=%d\r\n",
													   __FUNCTION__,
													   chip_id,
													   flexe_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_OH_LOOPBACK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,FLEXE_OH_LOOPBACK_CFG_REG[field_id].base_addr,FLEXE_OH_LOOPBACK_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		FLEXE_OH_LOOPBACK_CFG_REG[field_id].end_bit,FLEXE_OH_LOOPBACK_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_flexe_rx_mode_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		field_id:0:FLEXE_RX_MODE 1:MF16T32_RX_CFG 2:OMF_CHK_CTRL 3:OMF_CRC_CTRL
*		parameter:FLEXE_RX_MODE 0:50G  1:100G 2:200G 3:400G  MF16T32_RX_CFG:0,16 1:32
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_flexe_rx_mode_set(UINT_8 chip_id ,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) ||
		(flexe_id > (PHY_NUM-1))||
		(parameter > 0x3))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FLEXE_RX_MODE_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n flexe_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   flexe_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_RX_MODE_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FLEXE_RX_MODE_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,FLEXE_RX_MODE_CFG_REG[field_id].base_addr,FLEXE_RX_MODE_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		FLEXE_RX_MODE_CFG_REG[field_id].end_bit,FLEXE_RX_MODE_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_flexe_rx_mode_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		field_id:0:FLEXE_RX_MODE 1:MF16T32_RX_CFG 2:OMF_CHK_CTRL 3:OMF_CRC_CTRL
*		*parameter:FLEXE_RX_MODE 0:50G  1:100G 2:200G 3:400G  MF16T32_RX_CFG:0,16 1:32
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_flexe_rx_mode_get(UINT_8 chip_id ,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FLEXE_RX_MODE_GET] invalid parameter!! \
													   \r\n chip_id = %d,flexe_id=%d\r\n",
													   chip_id,flexe_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FLEXE_RX_MODE_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(FLEXE_RX_MODE_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FLEXE_RX_MODE_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,FLEXE_RX_MODE_CFG_REG[field_id].base_addr,FLEXE_RX_MODE_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		FLEXE_RX_MODE_CFG_REG[field_id].end_bit,FLEXE_RX_MODE_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_crc_rx_mode_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0:normal order 1:bit - reserve order 
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_crc_rx_mode_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)) ||
		(parameter > 0x1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CRC_RX_MODE_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (CRC_RX_MODE > MAX_INDEX(ALM_EN_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CRC_RX_MODE_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,ALM_EN_CFG_REG[CRC_RX_MODE].base_addr,ALM_EN_CFG_REG[CRC_RX_MODE].offset_addr+FLEXE_OFFSET*phy_id,
		ALM_EN_CFG_REG[CRC_RX_MODE].end_bit,ALM_EN_CFG_REG[CRC_RX_MODE].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_crc_rx_mode_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_crc_rx_mode_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CRC_RX_MODE_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CRC_RX_MODE_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (CRC_RX_MODE > MAX_INDEX(ALM_EN_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CRC_RX_MODE_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,ALM_EN_CFG_REG[CRC_RX_MODE].base_addr,ALM_EN_CFG_REG[CRC_RX_MODE].offset_addr+FLEXE_OFFSET*phy_id,
		ALM_EN_CFG_REG[CRC_RX_MODE].end_bit,ALM_EN_CFG_REG[CRC_RX_MODE].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_alm_en_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type: ,[5~0]flexe_fail [0]:LOF;[1]:LOM;[2]:GIDM;[3]:PMM;[4]: PCS_LF;[5]:OOF
*							   [11~6]RPF [6]:LOF;[7]:LOM;[8]:GIDM;[9]:PMM;[10]:PCS_LF;[11]:OOF; 
*	                           [17~12]deskew [12]:LOF [13]:LOM [14]:PCS_LF [15]:PMM [16]:PCS_LF;[17]:OOF
*							   [23~18]oam_rdi [18]:LOF [19]:LOM [20]:PCS_LF [21]:PMM [22]:PCS_LF;[23]:OOF
*							   [30~24]soh [24]:LOF [25]:LOM [26]:PCS_LF [27]:PMM [28]:PCS_LF;[29]:OOF;[30]:LOM
*		parameter:0:disable  1:enable 
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_alm_en_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)) ||
		(type > 30) ||
		(parameter > 0x1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ALM_EN_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   type,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,ALM_EN_CFG_REG[ALM_EN].base_addr,ALM_EN_CFG_REG[ALM_EN].offset_addr+FLEXE_OFFSET*phy_id,
		type,type,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_alm_en_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type: ,[5~0]flexe_fail [0]:LOF;[1]:LOM;[2]:GIDM;[3]:PMM;[4]: PCS_LF;[5]:OOF
*							   [11~6]RPF [6]:LOF;[7]:LOM;[8]:GIDM;[9]:PMM;[10]:PCS_LF;[11]:OOF; 
*	                           [17~12]deskew [12]:LOF [13]:LOM [14]:PCS_LF [15]:PMM [16]:PCS_LF;[17]:OOF
*							   [23~18]oam_rdi [18]:LOF [19]:LOM [20]:PCS_LF [21]:PMM [22]:PCS_LF;[23]:OOF
*							   [30~24]soh [24]:LOF [25]:LOM [26]:PCS_LF [27]:PMM [28]:PCS_LF;[29]:OOF;[30]:LOM
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_alm_en_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)) ||
		(type > 30))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ALM_EN_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ALM_EN_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,ALM_EN_CFG_REG[ALM_EN].base_addr,ALM_EN_CFG_REG[ALM_EN].offset_addr+FLEXE_OFFSET*phy_id,
		type,type,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_exccc_exsc_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0:ccc 1:sc
*		parameter:reserve exp value 
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_exccc_exsc_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32 parameter)
{	
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)) ||
		(type > MAX_INDEX(EXCC_OH_CFG_REG)) ||
		(parameter > 0x1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_EXCCC_EXSC_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   type,
													   parameter);
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,EXCC_OH_CFG_REG[type].base_addr,EXCC_OH_CFG_REG[type].offset_addr+FLEXE_OFFSET*phy_id,
		EXCC_OH_CFG_REG[type].end_bit,EXCC_OH_CFG_REG[type].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_exccc_exsc_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0:ccc 1:sc
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_exccc_exsc_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)|| (phy_id > (PHY_NUM-1)) ||
		(type > MAX_INDEX(EXCC_OH_CFG_REG)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_EXCCC_EXSC_GET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n type = %d,\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_EXCCC_EXSC_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,EXCC_OH_CFG_REG[type].base_addr,EXCC_OH_CFG_REG[type].offset_addr+FLEXE_OFFSET*phy_id,
		EXCC_OH_CFG_REG[type].end_bit,EXCC_OH_CFG_REG[type].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_exgid_expid_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		field_id:0~1,0:exgid,1:expid
*		parameter:0~0xfffff:reserve gid exp value
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_exgid_expid_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 field_id,UINT_32 parameter)
{	
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)) ||(field_id > 1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_EXGID_EXPID_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n phy_id = %d, \
													   \r\n field_id = %d \r\n",
													   chip_id,
													   phy_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(EXGID_OH_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_EXGID_EXPID_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,EXGID_OH_CFG_REG[field_id].base_addr,EXGID_OH_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*phy_id,
		EXGID_OH_CFG_REG[field_id].end_bit,EXGID_OH_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_exgid_expid_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		field_id:0~1,0:exgid,1:expid
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_exgid_expid_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 field_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)) || (field_id > 1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_EXGID_EXPID_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n phy_id = %d ,\
													   \r\n field_id = %d \r\n",
													   chip_id,
													   phy_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_EXGID_EXPID_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(EXGID_OH_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_EXGID_EXPID_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,EXGID_OH_CFG_REG[field_id].base_addr,EXGID_OH_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*phy_id,
		EXGID_OH_CFG_REG[field_id].end_bit,EXGID_OH_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_fp_pattern_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0x3 length of Pattern  0:14 bit  1:22 bit 2:30 bit 3 38 bit
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_fp_pattern_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)|| (phy_id > (PHY_NUM-1)) ||
		(parameter > 0x3))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FP_PATTERN_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (FP_PATTERN_CFG > MAX_INDEX(FAS_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FP_PATTERN_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,FAS_CHK_CFG_REG[FP_PATTERN_CFG].base_addr,FAS_CHK_CFG_REG[FP_PATTERN_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[FP_PATTERN_CFG].end_bit,FAS_CHK_CFG_REG[FP_PATTERN_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_fp_pattern_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_fp_pattern_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||(phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FP_PATTERN_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FP_PATTERN_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (FP_PATTERN_CFG > MAX_INDEX(FAS_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_FP_PATTERN_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,FAS_CHK_CFG_REG[FP_PATTERN_CFG].base_addr,FAS_CHK_CFG_REG[FP_PATTERN_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[FP_PATTERN_CFG].end_bit,FAS_CHK_CFG_REG[FP_PATTERN_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lof_clr_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0x1f clear lof alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lof_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||(phy_id > (PHY_NUM-1)) ||
		(parameter > 0x1f))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_CLR_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (LOF_CLR_CFG > MAX_INDEX(FAS_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_CLR_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,FAS_CHK_CFG_REG[LOF_CLR_CFG].base_addr,FAS_CHK_CFG_REG[LOF_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[LOF_CLR_CFG].end_bit,FAS_CHK_CFG_REG[LOF_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lof_clr_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lof_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||(phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_CLR_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_CLR_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (LOF_CLR_CFG > MAX_INDEX(FAS_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_CLR_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,FAS_CHK_CFG_REG[LOF_CLR_CFG].base_addr,FAS_CHK_CFG_REG[LOF_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[LOF_CLR_CFG].end_bit,FAS_CHK_CFG_REG[LOF_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lof_set_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0x1f generate lof alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lof_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0x1f))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_SET_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (LOF_SET_CFG > MAX_INDEX(FAS_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_SET_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,FAS_CHK_CFG_REG[LOF_SET_CFG].base_addr,FAS_CHK_CFG_REG[LOF_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[LOF_SET_CFG].end_bit,FAS_CHK_CFG_REG[LOF_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lof_set_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lof_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_SET_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_SET_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (LOF_SET_CFG > MAX_INDEX(FAS_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOF_SET_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,FAS_CHK_CFG_REG[LOF_SET_CFG].base_addr,FAS_CHK_CFG_REG[LOF_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[LOF_SET_CFG].end_bit,FAS_CHK_CFG_REG[LOF_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oof_clr_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0xf clear oof alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oof_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0xf))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOF_CLR_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (OOF_CLR_CFG > MAX_INDEX(FAS_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOF_CLR_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,FAS_CHK_CFG_REG[OOF_CLR_CFG].base_addr,FAS_CHK_CFG_REG[OOF_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[OOF_CLR_CFG].end_bit,FAS_CHK_CFG_REG[OOF_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oof_clr_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oof_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOF_CLR_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOF_CLR_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (OOF_CLR_CFG > MAX_INDEX(FAS_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOF_CLR_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,FAS_CHK_CFG_REG[OOF_CLR_CFG].base_addr,FAS_CHK_CFG_REG[OOF_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[OOF_CLR_CFG].end_bit,FAS_CHK_CFG_REG[OOF_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oof_set_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0xf generate oof alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oof_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0xf))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOF_SET_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,FAS_CHK_CFG_REG[OOF_SET_CFG].base_addr,FAS_CHK_CFG_REG[OOF_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[OOF_SET_CFG].end_bit,FAS_CHK_CFG_REG[OOF_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oof_set_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oof_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||(phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOF_SET_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOF_SET_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,FAS_CHK_CFG_REG[OOF_SET_CFG].base_addr,FAS_CHK_CFG_REG[OOF_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		FAS_CHK_CFG_REG[OOF_SET_CFG].end_bit,FAS_CHK_CFG_REG[OOF_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lom_clr_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0x1f clear lom alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lom_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0x1f))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_CLR_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (LOM_CLR_CFG > MAX_INDEX(OMF_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_CLR_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,OMF_CHK_CFG_REG[LOM_CLR_CFG].base_addr,OMF_CHK_CFG_REG[LOM_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		OMF_CHK_CFG_REG[LOM_CLR_CFG].end_bit,OMF_CHK_CFG_REG[LOM_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lom_clr_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lom_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||(phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_CLR_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_CLR_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (LOM_CLR_CFG > MAX_INDEX(OMF_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_CLR_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,OMF_CHK_CFG_REG[LOM_CLR_CFG].base_addr,OMF_CHK_CFG_REG[LOM_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		OMF_CHK_CFG_REG[LOM_CLR_CFG].end_bit,OMF_CHK_CFG_REG[LOM_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lom_set_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0x1f generate lom alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lom_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0x1f))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_SET_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (LOM_SET_CFG > MAX_INDEX(OMF_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_SET_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,OMF_CHK_CFG_REG[LOM_SET_CFG].base_addr,OMF_CHK_CFG_REG[LOM_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		OMF_CHK_CFG_REG[LOM_SET_CFG].end_bit,OMF_CHK_CFG_REG[LOM_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lom_set_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lom_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_SET_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_SET_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (LOM_SET_CFG > MAX_INDEX(OMF_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOM_SET_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,OMF_CHK_CFG_REG[LOM_SET_CFG].base_addr,OMF_CHK_CFG_REG[LOM_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		OMF_CHK_CFG_REG[LOM_SET_CFG].end_bit,OMF_CHK_CFG_REG[LOM_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oom_clr_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0x1f clear oom alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oom_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0x1f))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOM_CLR_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (OOM_CLR_CFG > MAX_INDEX(OMF_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOM_CLR_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,OMF_CHK_CFG_REG[OOM_CLR_CFG].base_addr,OMF_CHK_CFG_REG[OOM_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		OMF_CHK_CFG_REG[OOM_CLR_CFG].end_bit,OMF_CHK_CFG_REG[OOM_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oom_clr_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oom_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOM_CLR_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOM_CLR_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (OOM_CLR_CFG > MAX_INDEX(OMF_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOM_CLR_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,OMF_CHK_CFG_REG[OOM_CLR_CFG].base_addr,OMF_CHK_CFG_REG[OOM_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		OMF_CHK_CFG_REG[OOM_CLR_CFG].end_bit,OMF_CHK_CFG_REG[OOM_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oom_set_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0x1f generate oom alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oom_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0x1f))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOM_SET_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,OMF_CHK_CFG_REG[OOM_SET_CFG].base_addr,OMF_CHK_CFG_REG[OOM_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		OMF_CHK_CFG_REG[OOM_SET_CFG].end_bit,OMF_CHK_CFG_REG[OOM_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oom_set_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oom_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||(phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOM_SET_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOM_SET_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,OMF_CHK_CFG_REG[OOM_SET_CFG].base_addr,OMF_CHK_CFG_REG[OOM_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		OMF_CHK_CFG_REG[OOM_SET_CFG].end_bit,OMF_CHK_CFG_REG[OOM_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lop_clr_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0x1f clear lop alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lop_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0x1f))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_CLR_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (LOP_CLR_CFG > MAX_INDEX(PAD_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_CLR_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,PAD_CHK_CFG_REG[LOP_CLR_CFG].base_addr,PAD_CHK_CFG_REG[LOP_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		PAD_CHK_CFG_REG[LOP_CLR_CFG].end_bit,PAD_CHK_CFG_REG[LOP_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lop_clr_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lop_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||(phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_CLR_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_CLR_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (LOP_CLR_CFG > MAX_INDEX(PAD_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_CLR_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,PAD_CHK_CFG_REG[LOP_CLR_CFG].base_addr,PAD_CHK_CFG_REG[LOP_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		PAD_CHK_CFG_REG[LOP_CLR_CFG].end_bit,PAD_CHK_CFG_REG[LOP_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lop_set_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0x1f generate lop alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lop_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0x1f))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_SET_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (LOP_SET_CFG > MAX_INDEX(PAD_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_SET_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,PAD_CHK_CFG_REG[LOP_SET_CFG].base_addr,PAD_CHK_CFG_REG[LOP_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		PAD_CHK_CFG_REG[LOP_SET_CFG].end_bit,PAD_CHK_CFG_REG[LOP_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_lop_set_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_lop_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_SET_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_SET_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (LOP_SET_CFG > MAX_INDEX(PAD_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_LOP_SET_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,PAD_CHK_CFG_REG[LOP_SET_CFG].base_addr,PAD_CHK_CFG_REG[LOP_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		PAD_CHK_CFG_REG[LOP_SET_CFG].end_bit,PAD_CHK_CFG_REG[LOP_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oop_clr_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0xf clear oop alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oop_clr_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0xf))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOP_CLR_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (OOP_CLR_CFG > MAX_INDEX(PAD_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOP_CLR_CFG_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,PAD_CHK_CFG_REG[OOP_CLR_CFG].base_addr,PAD_CHK_CFG_REG[OOP_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		PAD_CHK_CFG_REG[OOP_CLR_CFG].end_bit,PAD_CHK_CFG_REG[OOP_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oop_clr_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oop_clr_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || (phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOP_CLR_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOP_CLR_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (OOP_CLR_CFG > MAX_INDEX(PAD_CHK_CFG_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOP_CLR_CFG_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,PAD_CHK_CFG_REG[OOP_CLR_CFG].base_addr,PAD_CHK_CFG_REG[OOP_CLR_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		PAD_CHK_CFG_REG[OOP_CLR_CFG].end_bit,PAD_CHK_CFG_REG[OOP_CLR_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oop_set_cfg_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		parameter:0~0xf generate oop alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oop_set_cfg_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_32 parameter)
{		
	if ((chip_id > MAX_DEV)||
        (phy_id > (PHY_NUM-1)) ||
		(parameter > 0xf))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOP_SET_CFG_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,PAD_CHK_CFG_REG[OOP_SET_CFG].base_addr,PAD_CHK_CFG_REG[OOP_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		PAD_CHK_CFG_REG[OOP_SET_CFG].end_bit,PAD_CHK_CFG_REG[OOP_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oop_set_cfg_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oop_set_cfg_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||(phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOP_SET_CFG_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OOP_SET_CFG_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,PAD_CHK_CFG_REG[OOP_SET_CFG].base_addr,PAD_CHK_CFG_REG[OOP_SET_CFG].offset_addr+FLEXE_OFFSET*phy_id,
		PAD_CHK_CFG_REG[OOP_SET_CFG].end_bit,PAD_CHK_CFG_REG[OOP_SET_CFG].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_pad_alm_en_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type: ,[1:0]  Local fault  [0]:OOP; [1]:LOP 
*							   [3:2]  RPF  [2]:OOP; [3]:LOP
*							   [5:4]  deskew  [4]:OOP; [5]:LOP
*							   [7:6]  oam_rdi  [6]:OOP; [7]:LOP 
*							   [9:8]  soh   [8]:OOP; [9]:LOP
*							   [10] pcs_lf  '0': '1'OOF/OOP
*							   [11] CRC_ALM_EN
*		parameter:0:disable  1:enable 
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_pad_alm_en_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) || 
        (phy_id > (PHY_NUM-1)) ||
		(type > 11) ||
		(parameter > 0x1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_PAD_ALM_EN_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   type,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	regp_bit_write(chip_id,PAD_ALM_EN_REG[PAD_ALM_EN].base_addr,PAD_ALM_EN_REG[PAD_ALM_EN].offset_addr+FLEXE_OFFSET*phy_id,
		type,type,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_pad_alm_en_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type: ,[1:0]  Local fault  [0]:OOP; [1]:LOP 
*							   [3:2]  RPF  [2]:OOP; [3]:LOP
*							   [5:4]  deskew  [4]:OOP; [5]:LOP
*							   [7:6]  oam_rdi  [6]:OOP; [7]:LOP 
*							   [9:8]  soh   [8]:OOP; [9]:LOP
*							   [10] pcs_lf  '0': '1'OOF/OOP
*							   [11] CRC_ALM_EN
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_pad_alm_en_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || 
        (phy_id > (PHY_NUM-1)) ||
		(type > 11))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_PAD_ALM_EN_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_PAD_ALM_EN_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,PAD_ALM_EN_REG[PAD_ALM_EN].base_addr,PAD_ALM_EN_REG[PAD_ALM_EN].offset_addr+FLEXE_OFFSET*phy_id,
		type,type,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_cc_rx_value_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0~3 0:ACCCC  1:ACCR  2:ACCA 3:ACSC
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_cc_rx_value_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
        (phy_id > (PHY_NUM-1)) ||
		(type > 3))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CC_RX_VALUE_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CC_RX_VALUE_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(CC_Rx_VALUE_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CC_RX_VALUE_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,CC_Rx_VALUE_REG[type].base_addr,CC_Rx_VALUE_REG[type].offset_addr+FLEXE_OFFSET*phy_id,
		CC_Rx_VALUE_REG[type].end_bit,CC_Rx_VALUE_REG[type].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_id_rx_value_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0~1 0: ACGID  1:ACPID 
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_id_rx_value_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
        (phy_id > (PHY_NUM-1)) ||
		(type > 1))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ID_RX_VALUE_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ID_RX_VALUE_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(ID_Rx_VALUE_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ID_RX_VALUE_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,ID_Rx_VALUE_REG[type].base_addr,ID_Rx_VALUE_REG[type].offset_addr+FLEXE_OFFSET*phy_id,
		ID_Rx_VALUE_REG[type].end_bit,ID_Rx_VALUE_REG[type].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_acmc_setion_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		field_id:0:bit0~31 1:bit32~63 2:bit64~95 3:bit96~127
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_acmc_setion_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 field_id , UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV) ||
        (phy_id > (PHY_NUM-1)) ||
		(field_id> 3))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ACMC_SETION_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d\r\n",
													   chip_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ACMC_SETION_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,MANAGEMENT_CHANNEL_SECTION_REG[field_id].base_addr,MANAGEMENT_CHANNEL_SECTION_REG[field_id].offset_addr+FLEXE_OFFSET*phy_id,
		MANAGEMENT_CHANNEL_SECTION_REG[field_id].end_bit,MANAGEMENT_CHANNEL_SECTION_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_ac_shim2shim_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		field_id:0:bit0~31 1:bit32~63 2:bit64~95 3:bit96~127 4:bit128~159 5:bit160~191
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_ac_shim2shim_get(UINT_8 chip_id , UINT_8 phy_id,UINT_8 field_id ,UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV) ||
        (phy_id > (PHY_NUM-1)) ||
		(field_id > 5))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_AC_SHIM2SHIM_GET] invalid parameter!! \
				 									   \r\n chip_id = %d, \
													   \r\n field_id = %d\r\n",
													   chip_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_AC_SHIM2SHIM_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}


	regp_bit_read(chip_id,MANAGEMENT_CHANNEL_S2S_REG[field_id].base_addr,MANAGEMENT_CHANNEL_S2S_REG[field_id].offset_addr+FLEXE_OFFSET*phy_id,
		MANAGEMENT_CHANNEL_S2S_REG[field_id].end_bit,MANAGEMENT_CHANNEL_S2S_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_ac_reserved1_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		field:0:bit0~31 1:bit32~46
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_ac_reserved1_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 field_id, UINT_32* parameter)
{	
	UINT_32 data[2] = {0,0};
	UINT_32 i = 0;
	
	if ((chip_id > MAX_DEV) ||(phy_id > (PHY_NUM-1)) || field_id > 1)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_AC_RESERVED1_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_AC_RESERVED1_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	for(i = 0; i < 2; i++)
	{
		regp_bit_read(chip_id,RESERVED_Rx_VALUE_REG[AC_RESERVED1_0 + i].base_addr,RESERVED_Rx_VALUE_REG[AC_RESERVED1_0 + i].offset_addr+FLEXE_OFFSET*phy_id,
			RESERVED_Rx_VALUE_REG[AC_RESERVED1_0 + i].end_bit,RESERVED_Rx_VALUE_REG[AC_RESERVED1_0 + i].start_bit,&data[i]);
	}

	data[1] = data[1] & 0x7ffff;
	*parameter = data[field_id];
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_ac_reserved2_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_ac_reserved2_get(UINT_8 chip_id , UINT_8 phy_id,UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV)||(phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_AC_RESERVED2_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_AC_RESERVED2_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if(AC_RESERVED2 > MAX_INDEX(RESERVED_Rx_VALUE_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_AC_RESERVED2_GET] out of array range\r\n");
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,RESERVED_Rx_VALUE_REG[AC_RESERVED2].base_addr,RESERVED_Rx_VALUE_REG[AC_RESERVED2].offset_addr+FLEXE_OFFSET*phy_id,
		RESERVED_Rx_VALUE_REG[AC_RESERVED2].end_bit,RESERVED_Rx_VALUE_REG[AC_RESERVED2].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_alm_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0:oof  1:lof  2:oom 3:lom  4: crc16
*			 5:gid mismatch  6:phymap mismatch   7:rpf 8:sc mismatch  9:oop  10:lop 11:pid mistach
*			 12:OH1_BLOCK_ALM  13:C_BIT_ALM  14:GID_UNEQ_ALM
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_alm_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
        (phy_id > (PHY_NUM-1)) ||
		(type > GID_UNEQ_ALM))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ALM_GET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ALM_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}


	if(type > MAX_INDEX(OH_Rx_ALM_REG))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_ALM_GET] out of array range\r\n");
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,OH_Rx_ALM_REG[type].base_addr,OH_Rx_ALM_REG[type].offset_addr+FLEXE_OFFSET*phy_id,
		OH_Rx_ALM_REG[type].end_bit,OH_Rx_ALM_REG[type].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	oh_rx_alm_debug
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0:oof  1:lof  2:oom 3:lom  4: crc16
*			 5:gid mismatch  6:phymap mismatch   7:rpf 8:sc mismatch  9:oop  10:lop 11:pid mistach
*			 12:OH1_BLOCK_ALM  13:C_BIT_ALM  14:GID_UNEQ_ALM
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>		 	<CR_ID>		<DESCRIPTION>
*  yanghongliang  2019-03-28		              initial
*
******************************************************************************/

RET_STATUS oh_rx_alm_debug(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type)
{
	RET_STATUS result=RET_SUCCESS;
	UINT_32 para=0;
	result=oh_rx_alm_get(chip_id ,phy_id,type,&para);
	printf("alarm value is=%d\n",para);
	return result;
}



/******************************************************************************
*
* FUNCTION	oh_rx_ccm_rx_alm_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_ccm_rx_alm_get(UINT_8 chip_id , UINT_8 phy_id,UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV) ||(phy_id > (PHY_NUM-1)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CCM_RX_ALM_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CCM_RX_ALM_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,CCM_Rx_ALM_REG[CCM].base_addr,CCM_Rx_ALM_REG[CCM].offset_addr+FLEXE_OFFSET*phy_id,
		CCM_Rx_ALM_REG[CCM].end_bit,CCM_Rx_ALM_REG[CCM].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oh_alm_int_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0~28
*		parameter:reserve exp value 
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oh_alm_int_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32 parameter)
{	
	if ((chip_id > MAX_DEV) || 
        (phy_id > (PHY_NUM-1)) ||
		(type > MAX_INDEX(oh_alm_INT_REG)) ||
		(parameter > 0x3))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OH_ALM_INT_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   type,
													   parameter);
		}
		
		return RET_PARAERR;
	}


	regp_bit_write(chip_id,oh_alm_INT_REG[type].base_addr,oh_alm_INT_REG[type].offset_addr+FLEXE_OFFSET*phy_id,
				oh_alm_INT_REG[type].end_bit,oh_alm_INT_REG[type].start_bit,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oh_alm_int_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0~28
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oh_alm_int_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)|| 
        (phy_id > (PHY_NUM-1)) ||
		(type > MAX_INDEX(oh_alm_INT_REG)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OH_ALM_INT_GET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n type = %d,\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OH_ALM_INT_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,oh_alm_INT_REG[type].base_addr,oh_alm_INT_REG[type].offset_addr+FLEXE_OFFSET*phy_id,
		oh_alm_INT_REG[type].end_bit,oh_alm_INT_REG[type].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oh_alm_int_mask_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0~28
*		parameter:reserve exp value 
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oh_alm_int_mask_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32 parameter)
{	
	if ((chip_id > MAX_DEV) || 
        (phy_id > (PHY_NUM-1)) ||
		(type > MAX_INDEX(oh_alm_INT_MASK_REG)) ||
		(parameter > 0x3))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OH_ALM_INT_MASK_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   type,
													   parameter);
		}
		
		return RET_PARAERR;
	}


	regp_bit_write(chip_id,oh_alm_INT_MASK_REG[type].base_addr,oh_alm_INT_MASK_REG[type].offset_addr+FLEXE_OFFSET*phy_id,
				oh_alm_INT_MASK_REG[type].end_bit,oh_alm_INT_MASK_REG[type].start_bit,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_oh_alm_int_mask_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0~28
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_oh_alm_int_mask_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type, UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) || 
        (phy_id > (PHY_NUM-1)) ||
		(type > MAX_INDEX(oh_alm_INT_MASK_REG)))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OH_ALM_INT_MASK_GET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n type = %d,\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_OH_ALM_INT_MASK_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,oh_alm_INT_MASK_REG[type].base_addr,oh_alm_INT_MASK_REG[type].offset_addr+FLEXE_OFFSET*phy_id,
		oh_alm_INT_MASK_REG[type].end_bit,oh_alm_INT_MASK_REG[type].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_phymap_rx_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		offset:0~31
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_phymap_rx_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 offset ,UINT_32* parameter)
{		
	if ((chip_id > MAX_DEV) || 
        (phy_id > (PHY_NUM-1)) ||
		(offset > 31))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_PHYMAP_RX_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n offset = %d\r\n",
													   chip_id,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_PHYMAP_RX_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	ram_read(chip_id,PHYMAP_Rx.base_addr,PHYMAP_Rx.ram_addr+FLEXE_OFFSET*phy_id,offset,PHYMAP_Rx.width,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_phymap_rx_exp_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		offset:0~31
*		parameter:0~0x7 clear oom alm more than n frame
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_phymap_rx_exp_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 offset ,UINT_32* parameter)
{		
	if ((chip_id > MAX_DEV) ||
        (phy_id > (PHY_NUM-1)) ||
		(offset > 31))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_PHYMAP_RX_EXP_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n offset = %d\r\n",
													   chip_id,
													   offset);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_PHYMAP_RX_EXP_SET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	ram_write(chip_id,PHYMAP_Rx_EXP.base_addr,PHYMAP_Rx_EXP.ram_addr+FLEXE_OFFSET*phy_id,offset,PHYMAP_Rx_EXP.width,parameter);

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_phymap_rx_exp_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		offset:0~31
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_phymap_rx_exp_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 offset ,UINT_32* parameter)
{		
	if ((chip_id > MAX_DEV) || 
        (phy_id > (PHY_NUM-1)) ||
		(offset > 31))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_PHYMAP_RX_EXP_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n offset = %d\r\n",
													   chip_id,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_PHYMAP_RX_EXP_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	ram_read(chip_id,PHYMAP_Rx_EXP.base_addr,PHYMAP_Rx_EXP.ram_addr+FLEXE_OFFSET*phy_id,offset,PHYMAP_Rx_EXP.width,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_calendar_rx_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0:calendarA 1:calendarB 
*		offset:0~19
*		*parameter:read number,CalendarA&CalendarB rx value
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_calendar_rx_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_8 offset ,UINT_32* parameter)
{		
	UINT_32 data[1] = {0};
	
	if ((chip_id > MAX_DEV) || 
        (phy_id > (PHY_NUM-1)) ||
		(type > 1) ||
		(offset > 19))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CALENDAR_RX_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d, \
													   \r\n offset = %d\r\n",
													   chip_id,
													   type,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CALENDAR_RX_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	ram_read(chip_id,CCAB_Rx.base_addr,CCAB_Rx.ram_addr+FLEXE_OFFSET*phy_id,offset,CCAB_Rx.width,data);
	
	*parameter = (data[0] >> (16*type)) & 0xffff;
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_ccab_rx_exp_set
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0:cca 1:ccb
*		offset:0~19
*		*parameter:0~0xffff
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_ccab_rx_exp_set(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_8 offset ,UINT_32* parameter)
{	
	CHIP_RAM reg[2] = {CCA_Rx_EXP,CCB_Rx_EXP};
	
	if ((chip_id > MAX_DEV) ||
        (phy_id > (PHY_NUM-1)) ||
		(type > 1) || 
		(offset > 19))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CCAB_RX_EXP_SET] invalid parameter!! \
													   \r\n chip_id = %d,\
													   \r\n type = %d,\
													   \r\n offset = %d\r\n",
													   chip_id,
													   type,
													   offset);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CCAB_RX_EXP_SET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	ram_write(chip_id,reg[type].base_addr,reg[type].ram_addr+FLEXE_OFFSET*phy_id,offset,reg[type].width,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	oh_rx_ccab_rx_exp_get
*
*    	RESERVED
*
* DESCRIPTION
*
*   	RESERVED
*
* NOTE
*
*		RESERVED
*
* PARAMETERS
*
*   	chip_id:0~max_dev
*		type:0:cca 1:ccb
*		offset:0~31
*		*parameter:read number
*
* RETURNS
*
*   	RET_SUCCESS:success
*		RET_PARAERR:parameter error
*
* CALLS
*
*   	RESERVED
*
* CALLED BY
*
*    	RESERVED
*
* VERSION
*
*	<author>		<data>			<CR_ID>		<DESCRIPTION>
*  	  xinle   	    2017-09-25		   initial
*
******************************************************************************/
RET_STATUS oh_rx_ccab_rx_exp_get(UINT_8 chip_id ,UINT_8 phy_id,UINT_8 type ,UINT_8 offset ,UINT_32* parameter)
{		
	CHIP_RAM reg[2] = {CCA_Rx_EXP,CCB_Rx_EXP};

	if ((chip_id > MAX_DEV) ||
        (phy_id > (PHY_NUM-1)) ||
		(type > 1) || 
		(offset > 31))
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CCAB_RX_EXP_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d, \
													   \r\n offset = %d\r\n",
													   chip_id,
													   type,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_oh_rx_para_debug)
		{
			printf("[OH_RX_CCAB_RX_EXP_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	ram_read(chip_id,reg[type].base_addr,reg[type].ram_addr+FLEXE_OFFSET*phy_id,offset,reg[type].width,parameter);
	
	return RET_SUCCESS;
}
#if 0
/******************************************************************************
*
* FUNCTION flexe_oh_init
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
RET_STATUS flexe_oh_init(UINT_8 chip_id)
{
	UINT_8 i = 0;

	/*rdi insert cfg init*/
	for(i = 0;i < PHY_NUM; i++)
	{
		oh_tx_mode_cfg_set(chip_id,i,PHY_INST_TX_RI_SW_CFG,i);

		oh_tx_oh_ins_src_cfg_set(chip_id,i,SECTION_SRC_CFG,0x1);  /**/
		oh_tx_oh_ins_src_cfg_set(chip_id,i,SHIM2SHIM_SRC_CFG,0x1);  /**/
		oh_tx_oh_ins_src_cfg_set(chip_id,i,SMC_SRC_CFG,0x1);  /**/

		oh_tx_oh_ins_src_cfg_set(chip_id,i,GID_SRC_CFG,0);
		oh_tx_oh_ins_cfg_set(chip_id,i,GID_INS,0x1);
		oh_rx_exgid_expid_set(chip_id,i,EXGID,0x1);
	}

	flexe_multi_deskew_rate_set(chip_id,0,804591503);
	flexe_multi_deskew_rate_set(chip_id,1,825337743);
	flexe_multi_deskew_rate_set(chip_id,2,1609183006);
	flexe_multi_deskew_rate_set(chip_id,3,1629929246);
	
	return RET_SUCCESS;
}
#endif
