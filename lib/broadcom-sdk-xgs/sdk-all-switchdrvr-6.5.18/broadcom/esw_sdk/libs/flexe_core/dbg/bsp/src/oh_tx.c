
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int flexe_oh_tx_para_debug = 0;

const CHIP_REG FLEXE_TX_MODE_CFG_REG[]=
{
	{FLEXE_TX_MODE,FLEXE_OH_TX_BASE_ADDR,0x1,1,0},
	{CRC_TX_MODE,FLEXE_OH_TX_BASE_ADDR,0x1,2,2},
	{UNEQUIPPED_CFG,FLEXE_OH_TX_BASE_ADDR,0x1,3,3},
	{B66_ERR_RPL,FLEXE_OH_TX_BASE_ADDR,0x1,4,4},
	{MF16T32_TX_CFG,FLEXE_OH_TX_BASE_ADDR,0x1,5,5},
	{PHY_INST_TX_RI_SW_CFG,FLEXE_OH_TX_BASE_ADDR,0x1,10,8},
	{SMC_REQ_CFG,FLEXE_OH_TX_BASE_ADDR,0x1,11,11},
};
const CHIP_REG OH_INS_SRC_CFG_REG[]=
{
	{FAS_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,1,0},
	{CC_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,3,2},
	{OMF_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,5,4},
	{RPF_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,7,6},
	{GID_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,9,8},
	{PYHMAP_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,11,10},
	{PID_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,13,12},
	{CCA_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,15,14},
	{CCB_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,17,16},
	{CR_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,19,18},
	{CA_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,21,20},
	{CRC16_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,23,22},
	{SECTION_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,25,24},
	{SHIM2SHIM_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,27,26},
	{SMC_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,29,28},
	{SC_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x2,31,30},
};
const CHIP_REG RESERVED_INS_SRC_CFG_REG[]=
{
	{RESERVED1_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x3,3,2},
	{RESERVED2_SRC_CFG,FLEXE_OH_TX_BASE_ADDR,0x3,5,4},
};
const CHIP_REG OH_INS_CFG0_REG[]=
{
	{CCC_INS,FLEXE_OH_TX_BASE_ADDR,0x4,0,0},
	{CR_INS,FLEXE_OH_TX_BASE_ADDR,0x4,1,1},
	{CA_INS,FLEXE_OH_TX_BASE_ADDR,0x4,2,2},
	{RPF_INS,FLEXE_OH_TX_BASE_ADDR,0x4,3,3},
	{PID_INS,FLEXE_OH_TX_BASE_ADDR,0x4,11,4},
	{GID_INS,FLEXE_OH_TX_BASE_ADDR,0x4,31,12},
};
const CHIP_REG OH_INS_CFG1_REG[]=
{
	{MC_SECTION_INS_0,FLEXE_OH_TX_BASE_ADDR,0x5,31,0},
	{MC_SECTION_INS_1,FLEXE_OH_TX_BASE_ADDR,0x6,31,0},
	{MC_SECTION_INS_2,FLEXE_OH_TX_BASE_ADDR,0x7,31,0},
	{MC_SECTION_INS_3,FLEXE_OH_TX_BASE_ADDR,0x8,31,0},
};
const CHIP_REG OH_INS_CFG2_REG[]=
{
	{SMC_INS_0,FLEXE_OH_TX_BASE_ADDR,0x9,31,0},
	{SMC_INS_1,FLEXE_OH_TX_BASE_ADDR,0xa,31,0},
	{MC_S2S_INS_0,FLEXE_OH_TX_BASE_ADDR,0xb,31,0},
	{MC_S2S_INS_1,FLEXE_OH_TX_BASE_ADDR,0xc,31,0},
	{MC_S2S_INS_2,FLEXE_OH_TX_BASE_ADDR,0xd,31,0},
	{MC_S2S_INS_3,FLEXE_OH_TX_BASE_ADDR,0xe,31,0},
};
const CHIP_REG RESERVED_INS_CFG_REG[]=
{
	{SC_INS,FLEXE_OH_TX_BASE_ADDR,0x10,0,0},
	{RESERVED1_INS_0,FLEXE_OH_TX_BASE_ADDR,0x10,31,4},
	{RESERVED1_INS_1,FLEXE_OH_TX_BASE_ADDR,0x11,18,0},
	{RESERVED2_INS,FLEXE_OH_TX_BASE_ADDR,0x11,31,19},
};
const CHIP_REG OH_SH_CFG_REG[]=
{
	{OH_SH,FLEXE_OH_TX_BASE_ADDR,0xf,11,0},
};
const CHIP_REG b66code_err_REG[]=
{
	{B66CODE_ERR,FLEXE_OH_TX_BASE_ADDR,0x19,0,0},
};
const CHIP_RAM PHYMAP_INS_CFG = 
{
	TABLE_BASE_ADDR,FLEXE_OH_TX_BASE_ADDR+0x20,8,
};
const CHIP_RAM CCA_INS_CFG = 
{
	TABLE_BASE_ADDR,FLEXE_OH_TX_BASE_ADDR+0x21,16,
};
const CHIP_RAM CCB_INS_CFG = 
{
	TABLE_BASE_ADDR,FLEXE_OH_TX_BASE_ADDR+0x22,16,
};


/******************************************************************************
*
* FUNCTION oh_tx_mode_cfg_set
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
*     field_id: 0:FLEXE_TX_MODE 1:CRC_TX_MODE 2:UNEQUIPPED_CFG 3:B66_ERR_RPL 4:MF16T32_TX_CFG 5:PHY_INST_TX_RI_SW_CFG
*     parameter: oh ins src
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_mode_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,flexe_id=%d \r\n",__FUNCTION__,chip_id,flexe_id);
        }
		
		return RET_PARAERR;
	}
		
	if (field_id > MAX_INDEX(FLEXE_TX_MODE_CFG_REG))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,FLEXE_TX_MODE_CFG_REG[field_id].base_addr,FLEXE_TX_MODE_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		FLEXE_TX_MODE_CFG_REG[field_id].end_bit,FLEXE_TX_MODE_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_mode_cfg_get
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
*     field_id: 0:FLEXE_TX_MODE 1:CRC_TX_MODE 2:UNEQUIPPED_CFG 3:B66_ERR_RPL 4:MF16T32_TX_CFG 5:PHY_INST_TX_RI_SW_CFG
*     *parameter: pointer to oh ins src
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_mode_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d,flexe_id=%d \r\n",__FUNCTION__,chip_id,flexe_id);
        }
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(FLEXE_TX_MODE_CFG_REG))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,FLEXE_TX_MODE_CFG_REG[field_id].base_addr,FLEXE_TX_MODE_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		FLEXE_TX_MODE_CFG_REG[field_id].end_bit,FLEXE_TX_MODE_CFG_REG[field_id].start_bit,parameter);
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION oh_tx_oh_ins_src_cfg_set
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
*     field_id: oh id
*     parameter: oh ins src
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_oh_ins_src_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, \r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}
	
	
	
	if (field_id > MAX_INDEX(OH_INS_SRC_CFG_REG))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,OH_INS_SRC_CFG_REG[field_id].base_addr,OH_INS_SRC_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		OH_INS_SRC_CFG_REG[field_id].end_bit,OH_INS_SRC_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_oh_ins_src_cfg_get
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
*     field_id: oh id
*     *parameter: pointer to oh ins src
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_oh_ins_src_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, \r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(OH_INS_SRC_CFG_REG))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,OH_INS_SRC_CFG_REG[field_id].base_addr,OH_INS_SRC_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		OH_INS_SRC_CFG_REG[field_id].end_bit,OH_INS_SRC_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_res_ins_src_cfg_set
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
*     field_id:res id
*     parameter: res ins src
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_res_ins_src_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, \r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}
	
	
	
	if (field_id > MAX_INDEX(RESERVED_INS_SRC_CFG_REG))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,RESERVED_INS_SRC_CFG_REG[field_id].base_addr,RESERVED_INS_SRC_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		RESERVED_INS_SRC_CFG_REG[field_id].end_bit,RESERVED_INS_SRC_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_res_ins_src_cfg_get
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
*     field_id:res id
*     *parameter: pointer to res ins src
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_res_ins_src_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, \r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	if (field_id > MAX_INDEX(RESERVED_INS_SRC_CFG_REG))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,RESERVED_INS_SRC_CFG_REG[field_id].base_addr,RESERVED_INS_SRC_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		RESERVED_INS_SRC_CFG_REG[field_id].end_bit,RESERVED_INS_SRC_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_oh_ins_cfg_set
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
*     field_id:res id
*	  0:CCC_INS 1:CR_INS 2:CA_INS 3:RPF_INS 4:PID_INS 5:GID_INS 
*	  6:MC_SECTION_INS_0 7:MC_SECTION_INS_1 8:MC_SECTION_INS_2 9:MC_SECTION_INS_3
*	  10:SMC_INS_0 11:SMC_INS_1 12:MC_S2S_INS_0 13:MC_S2S_INS_1 14:MC_S2S_INS_2 15:MC_S2S_INS_3
*     parameter: pointer to oh ins value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_oh_ins_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, \r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}
	
	if (field_id <= 5)
	{	
		regp_bit_write(chip_id,OH_INS_CFG0_REG[field_id].base_addr,OH_INS_CFG0_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
			OH_INS_CFG0_REG[field_id].end_bit,OH_INS_CFG0_REG[field_id].start_bit,parameter);
		
	}
	else if (field_id >= 6 && field_id <= 9)
	{
		regp_bit_write(chip_id,OH_INS_CFG1_REG[field_id-6].base_addr,OH_INS_CFG1_REG[field_id-6].offset_addr+FLEXE_OFFSET*flexe_id,
			OH_INS_CFG1_REG[field_id-6].end_bit,OH_INS_CFG1_REG[field_id-6].start_bit,parameter);
	}
	else if (field_id >= 10 && field_id <= 15)
	{
		regp_bit_write(chip_id,OH_INS_CFG2_REG[field_id-10].base_addr,OH_INS_CFG2_REG[field_id-10].offset_addr+FLEXE_OFFSET*flexe_id,
			OH_INS_CFG2_REG[field_id-10].end_bit,OH_INS_CFG2_REG[field_id-10].start_bit,parameter);
	}
	else
	{
		return RET_PARAERR;
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_oh_ins_cfg_get
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
*     field_id:res id
*	  0:CCC_INS 1:CR_INS 2:CA_INS 3:RPF_INS 4:PID_INS 5:GID_INS 
*	  6:MC_SECTION_INS_0 7:MC_SECTION_INS_1 8:MC_SECTION_INS_2 9:MC_SECTION_INS_3
*	  10:SMC_INS_0 11:SMC_INS_1 12:MC_S2S_INS_0 13:MC_S2S_INS_1 14:MC_S2S_INS_2 15:MC_S2S_INS_3
*     *parameter: pointer to oh ins value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_oh_ins_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, \r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	if (field_id <= 5)
	{	
		regp_bit_read(chip_id,OH_INS_CFG0_REG[field_id].base_addr,OH_INS_CFG0_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
			OH_INS_CFG0_REG[field_id].end_bit,OH_INS_CFG0_REG[field_id].start_bit,parameter);
		
	}
	else if (field_id >= 6 && field_id <= 9)
	{
		regp_bit_read(chip_id,OH_INS_CFG1_REG[field_id-6].base_addr,OH_INS_CFG1_REG[field_id-6].offset_addr+FLEXE_OFFSET*flexe_id,
			OH_INS_CFG1_REG[field_id-6].end_bit,OH_INS_CFG1_REG[field_id-6].start_bit,parameter);
	}
	else if (field_id >= 10 && field_id <= 15)
	{
		regp_bit_read(chip_id,OH_INS_CFG2_REG[field_id-10].base_addr,OH_INS_CFG2_REG[field_id-10].offset_addr+FLEXE_OFFSET*flexe_id,
			OH_INS_CFG2_REG[field_id-10].end_bit,OH_INS_CFG2_REG[field_id-10].start_bit,parameter);
	}
	else
	{
		return RET_PARAERR;
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_res_ins_cfg_set
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
*     field_id:res id
*	  0:SC_INS 1:reserved1_0 0~27bit 2:reserved1_1 28~47bit  3:reserved2 
*     parameter: res ins value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_res_ins_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, \r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(RESERVED_INS_CFG_REG))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,RESERVED_INS_CFG_REG[field_id].base_addr,RESERVED_INS_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		RESERVED_INS_CFG_REG[field_id].end_bit,RESERVED_INS_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_res_ins_cfg_get
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
*     field_id:res id
*	  0:SC_INS 1:reserved1_0 0~27bit 2:reserved1_1 28~47bit  3:reserved2 
*     *parameter: pointer to oh ins value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_res_ins_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 field_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, \r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(RESERVED_INS_CFG_REG))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] out of array range!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,RESERVED_INS_CFG_REG[field_id].base_addr,RESERVED_INS_CFG_REG[field_id].offset_addr+FLEXE_OFFSET*flexe_id,
		RESERVED_INS_CFG_REG[field_id].end_bit,RESERVED_INS_CFG_REG[field_id].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION oh_tx_res_ins_cfg_set
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
*     field_id:BLOCK id
*	  0~5:BLOCK0,3,4,5,6,7 
*     parameter: res ins value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_oh_sh_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_32 parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, flexe_id = %d\r\n",
		   	__FUNCTION__,chip_id,flexe_id);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,OH_SH_CFG_REG[OH_SH].base_addr,OH_SH_CFG_REG[OH_SH].offset_addr+FLEXE_OFFSET*flexe_id,
		OH_SH_CFG_REG[OH_SH].end_bit,OH_SH_CFG_REG[OH_SH].start_bit,parameter);
	
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_res_ins_cfg_get
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
*     field_id:res id
*	  0:SC_INS 1:reserved1_0 0~27bit 2:reserved1_1 28~47bit  3:reserved2 
*     *parameter: pointer to oh ins value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_oh_sh_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, flexe_id = %d\r\n",
		   	__FUNCTION__,chip_id,flexe_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,OH_SH_CFG_REG[OH_SH].base_addr,OH_SH_CFG_REG[OH_SH].offset_addr+FLEXE_OFFSET*flexe_id,
		OH_SH_CFG_REG[OH_SH].end_bit,OH_SH_CFG_REG[OH_SH].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION oh_tx_b66code_err_get
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
*     field_id:res id
*     *parameter: pointer to oh ins value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_b66code_err_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, \r\n",__FUNCTION__,chip_id);
        }
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}

	regp_bit_read(chip_id,b66code_err_REG[B66CODE_ERR].base_addr,b66code_err_REG[B66CODE_ERR].offset_addr+FLEXE_OFFSET*flexe_id,
		b66code_err_REG[B66CODE_ERR].end_bit,b66code_err_REG[B66CODE_ERR].start_bit,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION oh_tx_phymap_cca_ccb_ins_cfg_set
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
*     oh_id:res id
*	  0:PHYMAP 1:CCA 2:CCB  
*	  offset:ram offset
*     *parameter: pointer to ins value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_phymap_cca_ccb_ins_cfg_set(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 oh_id,UINT_8 offset,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || oh_id > 2 || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, oh_id = %d\r\n",__FUNCTION__,chip_id,oh_id);
        }
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter pointer!!",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	switch(oh_id)
	{
		case 0:
			ram_write(chip_id,PHYMAP_INS_CFG.base_addr,PHYMAP_INS_CFG.ram_addr+FLEXE_OFFSET*flexe_id,offset,PHYMAP_INS_CFG.width,parameter);
		break;
		case 1:
			ram_write(chip_id,CCA_INS_CFG.base_addr,CCA_INS_CFG.ram_addr+FLEXE_OFFSET*flexe_id,offset,CCA_INS_CFG.width,parameter);
		break;
		case 2:
			ram_write(chip_id,CCB_INS_CFG.base_addr,CCB_INS_CFG.ram_addr+FLEXE_OFFSET*flexe_id,offset,CCB_INS_CFG.width,parameter);
		break;
		default:
		return RET_PARAERR;
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION oh_tx_phymap_cca_ccb_ins_cfg_get
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
*     oh_id:res id
*	  0:PHYMAP 1:CCA 2:CCB  
*	  offset:ram offset
*     *parameter: pointer to ins value
* RETURNS
*
*     0: success
*     1: fail
*     2: parameter error
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
*    jxma      2017-12-26   1.0           initial
*
******************************************************************************/
RET_STATUS oh_tx_phymap_cca_ccb_ins_cfg_get(UINT_8 chip_id,UINT_8 flexe_id,UINT_8 oh_id,UINT_8 offset,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || oh_id > 2 || flexe_id > (PHY_NUM-1))
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter!!\r\n chip_id = %d, oh_id = %d\r\n",__FUNCTION__,chip_id,oh_id);
        }
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (flexe_oh_tx_para_debug)
        {
           printf("[%s] invalid parameter pointer!!",__FUNCTION__);
        }
		
		return RET_PARAERR;
	}
	
	switch(oh_id)
	{
		case 0:
			ram_read(chip_id,PHYMAP_INS_CFG.base_addr,PHYMAP_INS_CFG.ram_addr+FLEXE_OFFSET*flexe_id,offset,PHYMAP_INS_CFG.width,parameter);
		break;
		case 1:
			ram_read(chip_id,CCA_INS_CFG.base_addr,CCA_INS_CFG.ram_addr+FLEXE_OFFSET*flexe_id,offset,CCA_INS_CFG.width,parameter);
		break;
		case 2:
			ram_read(chip_id,CCB_INS_CFG.base_addr,CCB_INS_CFG.ram_addr+FLEXE_OFFSET*flexe_id,offset,CCB_INS_CFG.width,parameter);
		break;
		default:
		return RET_PARAERR;
	}
	
	return RET_SUCCESS;
}

