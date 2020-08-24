

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
unsigned long int bc_1588_cpu_para_debug = 0;

const CHIP_REG CPU2PTP_WDATA_REG[] =
{
	{C2P_WDATA,CPU_BASE_ADDR,0x0,31,0},
};
const CHIP_REG CPU2PTP_FINISH_REG[] =
{
	{C2P_FINISH,CPU_BASE_ADDR,0x1,0,0},
};
const CHIP_REG PTP2CPU_FINISH_REG[] =
{
	{P2C_FINISH,CPU_BASE_ADDR,0x2,0,0},
};
const CHIP_REG CPU2PTP_FRM_CNT_REG[] =
{
	{C2P_FRM_CNT,CPU_BASE_ADDR,0x3,31,0},
};
const CHIP_REG PTP2CPU_FRM_CNT_REG[] =
{
	{P2C_FRM_CNT,CPU_BASE_ADDR,0x4,31,0},
};
const CHIP_REG CPU2PTP_CSTATE_REG[] =
{
	{C2P_CSTATE,CPU_BASE_ADDR,0x5,1,0},
};
const CHIP_REG CPU2PTP_BANK_FULL_REG[] =
{
	{C2P_BANK_FULL,CPU_BASE_ADDR,0x6,0,0},
};
const CHIP_REG PTP2CPU_RDATA_REG[] =
{
	{P2C_RDATA,CPU_BASE_ADDR,0x7,31,0},
};
const CHIP_REG PTP2CPU_CSTATE_REG[] =
{
	{P2C_CSTATE,CPU_BASE_ADDR,0x8,1,0},
};
const CHIP_REG PTP2CPU_BANK_EMPTY_REG[] =
{
	{P2C_BANK_EMPTY,CPU_BASE_ADDR,0x9,0,0},
};
const CHIP_REG CPU2PTP_LEN_ERR_REG[] =
{
	{C2P_DSCP_ERR,CPU_BASE_ADDR,0xa,0,0},
	{C2P_LEN_ERR,CPU_BASE_ADDR,0xa,1,1},
	{C2P_RAM_READ_PKT_ERR,CPU_BASE_ADDR,0xa,2,2},
};
const CHIP_REG PTP2CPU_LEN_ERR_REG[] =
{
	{P2C_PTP_ERR,CPU_BASE_ADDR,0xb,0,0},
	{P2C_LEN_ERR,CPU_BASE_ADDR,0xb,1,1},
};

/******************************************************************************
*
* FUNCTION	bc_cpu_c2p_wdata_set
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
*   	chip_no:0~max_dev
*		parameter:0:not drop ptp pkt  1:drop ptp pkt
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
RET_STATUS bc_cpu_c2p_wdata_set(UINT_8 chip_no ,UINT_32 parameter)
{
	if ((chip_no > MAX_DEV) || (parameter > 0xffffffff))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_WDATA_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_no,
													   parameter);
		}
		
		return RET_PARAERR;
	}

/********
	if (C2P_WDATA > MAX_INDEX(CPU2PTP_WDATA_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_WDATA_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
*********/
	
	regp_field_write(chip_no,CPU2PTP_WDATA_REG[C2P_WDATA],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_c2p_wdata_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_c2p_wdata_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_WDATA_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_WDATA_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/*******
	if (C2P_WDATA > MAX_INDEX(CPU2PTP_WDATA_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_WDATA_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	*******/
	
	regp_field_read(chip_no,CPU2PTP_WDATA_REG[C2P_WDATA],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_c2p_finish_set
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
*   	chip_no:0~max_dev
*		parameter:0:not drop ptp pkt  1:drop ptp pkt
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
RET_STATUS bc_cpu_c2p_finish_set(UINT_8 chip_no ,UINT_32 parameter)
{
	if ((chip_no > MAX_DEV) || (parameter > 0x1))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_FINISH_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_no,
													   parameter);
		}
		
		return RET_PARAERR;
	}

/*******
	if (C2P_FINISH > MAX_INDEX(CPU2PTP_FINISH_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_FINISH_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
*******/

	regp_write(chip_no,CPU2PTP_FINISH_REG[C2P_FINISH].base_addr,CPU2PTP_FINISH_REG[C2P_FINISH].offset_addr,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_p2c_finish_set
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
*   	chip_no:0~max_dev
*		parameter:0:not drop ptp pkt  1:drop ptp pkt
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
RET_STATUS bc_cpu_p2c_finish_set(UINT_8 chip_no ,UINT_32 parameter)
{
	if ((chip_no > MAX_DEV) || (parameter > 0x1))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_FINISH_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_no,
													   parameter);
		}
		
		return RET_PARAERR;
	}

/********
	if (P2C_FINISH > MAX_INDEX(PTP2CPU_FINISH_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_FINISH_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
********/	

	regp_write(chip_no,PTP2CPU_FINISH_REG[P2C_FINISH].base_addr,PTP2CPU_FINISH_REG[P2C_FINISH].offset_addr,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_c2p_frm_cnt_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_c2p_frm_cnt_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_FRM_CNT_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_FRM_CNT_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/*********
	if (C2P_FRM_CNT > MAX_INDEX(CPU2PTP_FRM_CNT_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_FRM_CNT_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	*********/
	
	regp_field_read(chip_no,CPU2PTP_FRM_CNT_REG[C2P_FRM_CNT],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_p2c_frm_cnt_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_p2c_frm_cnt_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_FRM_CNT_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_FRM_CNT_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/*******
	if (P2C_FRM_CNT > MAX_INDEX(PTP2CPU_FRM_CNT_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_FRM_CNT_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	********/
	
	regp_field_read(chip_no,PTP2CPU_FRM_CNT_REG[P2C_FRM_CNT],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_c2p_cstate_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_c2p_cstate_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_CSTATE_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_CSTATE_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/**********
	if (C2P_CSTATE > MAX_INDEX(CPU2PTP_CSTATE_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_CSTATE_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	***********/
	
	regp_field_read(chip_no,CPU2PTP_CSTATE_REG[C2P_CSTATE],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_c2p_bank_full_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_c2p_bank_full_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_BANK_FULL_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_BANK_FULL_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/*********
	if (C2P_BANK_FULL > MAX_INDEX(CPU2PTP_BANK_FULL_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_BANK_FULL_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	********/
	
	regp_field_read(chip_no,CPU2PTP_BANK_FULL_REG[C2P_BANK_FULL],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_p2c_rdata_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_p2c_rdata_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_RDATA_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_RDATA_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/**********
	if (P2C_RDATA > MAX_INDEX(PTP2CPU_RDATA_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_RDATA_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	*********/
	
	regp_field_read(chip_no,PTP2CPU_RDATA_REG[P2C_RDATA],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_p2c_cstate_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_p2c_cstate_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_CSTATE_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_CSTATE_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/********
	if (P2C_CSTATE > MAX_INDEX(PTP2CPU_CSTATE_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_CSTATE_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	**********/
	
	regp_field_read(chip_no,PTP2CPU_CSTATE_REG[P2C_CSTATE],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_p2c_bank_empty_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_p2c_bank_empty_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_BANK_EMPTY_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_BANK_EMPTY_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/********
	if (P2C_BANK_EMPTY > MAX_INDEX(PTP2CPU_BANK_EMPTY_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_BANK_EMPTY_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	*********/
	
	regp_field_read(chip_no,PTP2CPU_BANK_EMPTY_REG[P2C_BANK_EMPTY],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_c2p_len_err_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_c2p_len_err_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_LEN_ERR_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_LEN_ERR_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/********
	if (C2P_LEN_ERR > MAX_INDEX(CPU2PTP_LEN_ERR_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_LEN_ERR_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	********/
	
	regp_field_read(chip_no,CPU2PTP_LEN_ERR_REG[C2P_LEN_ERR],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_c2p_dscp_err_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_c2p_dscp_err_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_DSCP_ERR_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_DSCP_ERR_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/*********
	if (C2P_DSCP_ERR > MAX_INDEX(CPU2PTP_LEN_ERR_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_DSCP_ERR_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	**********/
	
	regp_field_read(chip_no,CPU2PTP_LEN_ERR_REG[C2P_DSCP_ERR],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_c2p_ram_read_pkt_err_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_c2p_ram_read_pkt_err_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_RAM_READ_PKT_ERR_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_RAM_READ_PKT_ERR_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/*********
	if (C2P_RAM_READ_PKT_ERR > MAX_INDEX(CPU2PTP_LEN_ERR_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_C2P_RAM_READ_PKT_ERR_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	**********/
	
	regp_field_read(chip_no,CPU2PTP_LEN_ERR_REG[C2P_RAM_READ_PKT_ERR],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_p2c_ptp_err_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_p2c_ptp_err_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_PTP_ERR_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_PTP_ERR_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/***********
	if (P2C_PTP_ERR > MAX_INDEX(PTP2CPU_LEN_ERR_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_PTP_ERR_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	**********/
	
	regp_field_read(chip_no,PTP2CPU_LEN_ERR_REG[P2C_PTP_ERR],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_cpu_p2c_len_err_get
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
*   	chip_no:0~max_dev
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
RET_STATUS bc_cpu_p2c_len_err_get(UINT_8 chip_no ,UINT_32 *parameter)
{
	if (chip_no > MAX_DEV)
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_LEN_ERR_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_no);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_LEN_ERR_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
	/***********
	if (P2C_LEN_ERR > MAX_INDEX(PTP2CPU_LEN_ERR_REG))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_CPU_P2C_LEN_ERR_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	**********/
	
	regp_field_read(chip_no,PTP2CPU_LEN_ERR_REG[P2C_LEN_ERR],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	bc_dma_cpu_cnt_get
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
*   	chip_no:0~max_dev
*   	sel:0-CPU2PTP_CNT, 1-PTP2CPU_CNT
*	    *parameter:pointer to cnt
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
RET_STATUS bc_cpu_cnt_get(UINT_8 chip_no, UINT_8 sel, UINT_32 *parameter)
{
	const CHIP_REG *p = NULL;

	if ((chip_no > MAX_DEV) ||
		(sel > 1))
	{
		if (bc_1588_cpu_para_debug)
		{
			printf("[BC_DMA_CPU_CNT_GET] invalid parameter!! \
													   \r\n chip_no = %d \
													   \r\n sel = %d \r\n",
													   chip_no,
                                                       sel);
		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if(bc_1588_cpu_para_debug)
		{
			printf("[BC_DMA_CPU_CNT_GET] invalid parameter pointer\r\n");
		}

		return RET_PARAERR;
	}
	
    switch(sel)
    {
        case 0:  
			sel = C2P_FRM_CNT;  
			p = CPU2PTP_FRM_CNT_REG;  
			break;
        case 1:  
			sel = P2C_FRM_CNT;  
			p = PTP2CPU_FRM_CNT_REG;  
			break;
        default: 
			break;
    }
	
	regp_field_read(chip_no,p[sel],parameter);
	
	return RET_SUCCESS;
}


