

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include "sal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "oh_tx.h"
#include "flexe_oam_rx.h"
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
#define SOH_DCI 0x0
#define SOH_FUCI_125M 0x10
#define SOH_SGMIITX_125M 0x20
#define SOH_SGMIIRX_125M 0x30
#define SOH_EXT_125M 0x40
#define SOH_INS_125M 0x50
#define OHIF_ASIC 0x60
#define OHIF_ASIC1 0x70

unsigned long int ohoam_ef_para_debug = 0;

const CHIP_REG SOH_INS_GETPKT1_REG[]=
{
	{SOH_INS_DRAM_INIT_EN,OHOAM_EF_BASE_ADDR,0x0 + SOH_DCI,9,9},
	{SOH_INST_MODE,OHOAM_EF_BASE_ADDR,0x0 + SOH_DCI,17,10},
	{SOH_INS_CHKSH_MFI,OHOAM_EF_BASE_ADDR,0x0 + SOH_DCI,20,18},
	{SOH_INS_DBUFF_LVL_CLR,OHOAM_EF_BASE_ADDR,0x0 + SOH_DCI,21,21},	
	{SOH_INS_DBUFF_LVL_RPT_SEL,OHOAM_EF_BASE_ADDR,0x0 + SOH_DCI,22,22},
        {SOAM_NO_OVERTIME_PRO,OHOAM_EF_BASE_ADDR,0x0 + SOH_DCI,8,8},
        {SOAM_OVERTIME,OHOAM_EF_BASE_ADDR,0x0 + SOH_DCI,7,0},
};
const CHIP_REG SOAM_LOOPBACK_REG[] =
{
	{SOAM_FLEXE_LOOPBACK,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,0,0},
	{SOAM_MAC_LOOPBACK,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,1,1},
	{MAC_INTE_RAM_INIT_EN,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,2,2},
	{FLEXE_INTE_RAM_INIT_EN,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,3,3},
	{SOAM_FLEXE_CFG_DONE,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,4,4},
	{SOAM_MAC_CFG_DONE,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,5,5},
	{ASIC_SOAM_FLEXE_ALM_EXT_EN,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,6,6},
	{ASIC_SOAM_MAC_ALM_EXT_EN,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,7,7},
	{SOH_LOOPBACK,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,8,8},
	{SOAM_FLEXE_BAS_EXT_EN,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,9,9},
	{SOAM_MAC_BAS_EXT_EN,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,10,10},
	{SOAM_FLEXE_FLVL_CLR,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,11,11},
	{SOAM_MAC_FLVL_CLR,OHOAM_EF_BASE_ADDR,0x9 + SOH_DCI,12,12},
};
const CHIP_REG SOAM_ALM_COLECTION_REG[] =
{
	{SOAM_FLEXE_ALM_PRD,OHOAM_EF_BASE_ADDR,0xa + SOH_DCI,3,0},
	{SOAM_MAC_ALM_PRD,OHOAM_EF_BASE_ADDR,0xa + SOH_DCI,7,4},
	{SOAM_TIMER_STEP_CFG,OHOAM_EF_BASE_ADDR,0xa + SOH_DCI,13,8},
};
const CHIP_REG ASIC_OHIF_DASA_REG[] =
{
	{ASIC_OHIF_LOCAL_ADDR_0,OHOAM_EF_BASE_ADDR,0xb + SOH_DCI,31,0},
	{ASIC_OHIF_LOCAL_ADDR_1,OHOAM_EF_BASE_ADDR,0xc + SOH_DCI,15,0},
	{ASIC_OHIF_DEST_ADDR_0,OHOAM_EF_BASE_ADDR,0xc + SOH_DCI,31,16},
	{ASIC_OHIF_DEST_ADDR_1,OHOAM_EF_BASE_ADDR,0xd + SOH_DCI,31,0},
};
const CHIP_REG SOAM_EXT_DSCF_LVL_STAT_REG[]=
{
	{SOAM_FLEXE_DSCFLVL_STAT,OHOAM_EF_BASE_ADDR,0x4 + SOH_DCI,8,0},
	{SOAM_MAC_DSCFLVL_STAT,OHOAM_EF_BASE_ADDR,0x4 + SOH_DCI,17,9},
	{SOH_INS_DBUFF_MAX_LVL,OHOAM_EF_BASE_ADDR,0x4 + SOH_DCI,25,18},
};
const CHIP_REG ins_pkt_port_id_err_reg[]=
{
	{SOH_INS_PIDERR_ALARM,OHOAM_EF_BASE_ADDR,0x1 + SOH_DCI,0,0},
	{SOH_INS_REQ_FIFO_FULL_ALAM,OHOAM_EF_BASE_ADDR,0x1 + SOH_DCI,1,1},	
	{SOH_INS_DA_ERR,OHOAM_EF_BASE_ADDR,0x1 + SOH_DCI,2,2},
	{SOHSH_ERR,OHOAM_EF_BASE_ADDR,0x1 + SOH_DCI,10,3},
	{INS_MFI_CON_ERR,OHOAM_EF_BASE_ADDR,0x1 + SOH_DCI,18,11},
	{INS_CBAS_PLN_ERR,OHOAM_EF_BASE_ADDR,0x1 + SOH_DCI,19,19},
	{INS_ETHID_ERR,OHOAM_EF_BASE_ADDR,0x1 + SOH_DCI,20,20},
};
const CHIP_REG SOH_EXT_CFFULL_ALARM_REG[]=
{
	{SOH_EXT_CFFULL_ALARM,OHOAM_EF_BASE_ADDR,0x3 + SOH_DCI,0,0},
	{SOH_INS_DBUFF_EMPTY,OHOAM_EF_BASE_ADDR,0x3 + SOH_DCI,8,1},
	{SOH_INS_DBUFF_FULL,OHOAM_EF_BASE_ADDR,0x3 + SOH_DCI,16,9},
};
const CHIP_REG SOAM_EXT_FLEXE_ALAM_REG[] =
{
	{FLEXE_EXT_CHID_ERR_ALAM,OHOAM_EF_BASE_ADDR,0x5 + SOH_DCI,0,0},
	{FLEXE_EXT_SEQ_ILGL_ALAM,OHOAM_EF_BASE_ADDR,0x5 + SOH_DCI,1,1},
	{FLEXE_EXT_SOP_ONLY_ALAM,OHOAM_EF_BASE_ADDR,0x5 + SOH_DCI,2,2},
	{FLEXE_EXT_EOP_ONLY_ALAM,OHOAM_EF_BASE_ADDR,0x5 + SOH_DCI,3,3},
	{FLEXE_EXT_DSC_FIFO_FULL,OHOAM_EF_BASE_ADDR,0x5 + SOH_DCI,4,4},
	{FLEXE_SOAM_OVERTIME_ALM,OHOAM_EF_BASE_ADDR,0x5 + SOH_DCI,5,5},
};
const CHIP_REG SOAM_EXT_MAC_ALAM_REG[] =
{
	{MAC_EXT_CHID_ERR_ALAM,OHOAM_EF_BASE_ADDR,0x6 + SOH_DCI,0,0},
	{MAC_EXT_SEQ_ILGL_ALAM,OHOAM_EF_BASE_ADDR,0x6 + SOH_DCI,1,1},
	{MAC_EXT_SOP_ONLY_ALAM,OHOAM_EF_BASE_ADDR,0x6 + SOH_DCI,2,2},
	{MAC_EXT_EOP_ONLY_ALAM,OHOAM_EF_BASE_ADDR,0x6 + SOH_DCI,3,3},
	{MAC_EXT_DSC_FIFO_FULL,OHOAM_EF_BASE_ADDR,0x6 + SOH_DCI,4,4},
	{MAC_SOAM_OVERTIME_ALM,OHOAM_EF_BASE_ADDR,0x6 + SOH_DCI,5,5},
};	
const CHIP_RAM soam_flexe_type_tbl_ram =
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x7 + SOH_DCI,8,
};
const CHIP_RAM soam_mac_type_tbl_ram = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x8 + SOH_DCI,8,
};
const CHIP_REG ext_gbfifo_lvl_REG[]=
{
	{EXT_GBF_BACKPRE_LVL,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,7,0},
	{EXT_GBF_IDLEADD_LVL,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,15,8},
	{B66ENCD_TESTEN,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,16,16},
	{B66ENCD_BKFTEN,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,17,17},
	{B66ENCD_RFCFG,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,19,18},
	{B66ENCD_LFCFG,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,21,20},
	{B66ENCD_MANRF,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,22,22},
	{B66ENCD_MANLF,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,23,23},
	{B66ENCD_MANIDL,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,24,24},
	{B66DECODE_SEL,OHOAM_EF_BASE_ADDR,0x0+ OHIF_ASIC,25,25},
};
const CHIP_REG b66_encode_decode_cnt_REG[]=
{
	{ENCODE_ERR_CNT,OHOAM_EF_BASE_ADDR,0x2+ OHIF_ASIC,7,0},
	{DECODE_ERR_CNT,OHOAM_EF_BASE_ADDR,0x2+ OHIF_ASIC,15,8},
	{B66_INS_BYTES_CNT,OHOAM_EF_BASE_ADDR,0x2+ OHIF_ASIC,31,16},
};
const CHIP_REG pkt_cnt_REG[]=
{
	{B66_DECODE_PKT_CNT,OHOAM_EF_BASE_ADDR,0x3+ OHIF_ASIC,15,0},
	{EXT_REQ_PKT_CNT,OHOAM_EF_BASE_ADDR,0x3+ OHIF_ASIC,31,16},
};
const CHIP_REG ins_crc_err_cnt_REG[]=
{
	{INS_PKT_CRC_ERR_CNT,OHOAM_EF_BASE_ADDR,0x6+ OHIF_ASIC,7,0},
	{INS_SHORT_PKT_CNT,OHOAM_EF_BASE_ADDR,0x6+ OHIF_ASIC,15,8},
};
const CHIP_REG ext_pkt_cnt_REG[]=
{
	{EXT_PKT_SOP_CNT,OHOAM_EF_BASE_ADDR,0x7+ OHIF_ASIC,15,0},
	{EXT_OAM_REQ_CNT,OHOAM_EF_BASE_ADDR,0x7+ OHIF_ASIC,31,16},
};
const CHIP_REG ohif_asic_alm_REG[]=
{
	{EXT_GBF_FULL_ALM,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,0,0},
	{EXT_GBF_EMPTY_ALM,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,1,1},
	{B66_ENCODE_ERR_ALM,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,2,2},
	{INS_GBF_FULL_ALM,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,3,3},
	{INS_PREF_FULL_ALM,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,4,4},
	{FLEXE_SOAM_REQF_FULL_ALM,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,5,5},
	{MAC_SOAM_REQF_FULL_ALM,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,6,6},
	{B66_DECODE_ERR_ALM,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,7,7},
	{B66_DECODE_EOP_ABNOR_ALM,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,8,8},
	{DE2EN_RF_STATUS,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,9,9},
	{DE2EN_LF_STATUS,OHOAM_EF_BASE_ADDR,0x1+ OHIF_ASIC,10,10},
};
const CHIP_RAM SNAP_EXT = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x8+ OHIF_ASIC,64,
};
const CHIP_RAM snap_ins = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x9+ OHIF_ASIC,64,
};
const CHIP_REG test_cfg_REG[]=
{
	{OAM_SEQ_ERR_CHID,OHOAM_EF_BASE_ADDR,0x2 + OHIF_ASIC1,6,0},
	{OAM_SEQ_ERR_EN,OHOAM_EF_BASE_ADDR,0x2 + OHIF_ASIC1,7,7},
	{OAM_INS_MODE,OHOAM_EF_BASE_ADDR,0x2 + OHIF_ASIC1,8,8},
	{SELF_MAX_CHID,OHOAM_EF_BASE_ADDR,0x2 + OHIF_ASIC1,15,9},
	{SET_CRC_ERR,OHOAM_EF_BASE_ADDR,0x2 + OHIF_ASIC1,16,16},
};
const CHIP_REG test_oh_ext_err_REG[]=
{
	{TEST_OH_EXT_ERR_0,OHOAM_EF_BASE_ADDR,0x6 + OHIF_ASIC1,31,0},
	{TEST_OH_EXT_ERR_1,OHOAM_EF_BASE_ADDR,0x7 + OHIF_ASIC1,31,0},
};
const CHIP_REG test_alm_REG[]=
{
	{TEST_TOLIUF_FULL_ALM,OHOAM_EF_BASE_ADDR,0xc + OHIF_ASIC1,0,0},
	{EXT_ETH_DNT_SQT,OHOAM_EF_BASE_ADDR,0xc + OHIF_ASIC1,1,1},
	{EXT_OAMREQ_DNT_SQT,OHOAM_EF_BASE_ADDR,0xc + OHIF_ASIC1,2,2},
	{REQ_MFI_CON_ERR,OHOAM_EF_BASE_ADDR,0xc + OHIF_ASIC1,15,8},
	{EXT_MFI_CON_ERR,OHOAM_EF_BASE_ADDR,0xc + OHIF_ASIC1,23,16},
	{EXT_SSF_ALM,OHOAM_EF_BASE_ADDR,0xc + OHIF_ASIC1,31,24},
};
const CHIP_RAM test_flexe_oam_ins = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x0 + OHIF_ASIC1,64,
};
const CHIP_RAM test_mac_oam_ins = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x1 + OHIF_ASIC1,64,
};
const CHIP_RAM test_oh_ins = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x3 + OHIF_ASIC1,66,
};
const CHIP_RAM test_oh_exp = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x4 + OHIF_ASIC1,66,
};
const CHIP_RAM test_oh_ext = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x5 + OHIF_ASIC1,66,
};
const CHIP_RAM test_flexe_oam_exp = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x8 + OHIF_ASIC1,64,
};
const CHIP_RAM test_flexe_oam_ext = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0x9 + OHIF_ASIC1,64,
};
const CHIP_RAM test_mac_oam_exp = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0xa + OHIF_ASIC1,64,
};
const CHIP_RAM test_mac_oam_ext = 
{
	TABLE_BASE_ADDR,OHOAM_EF_BASE_ADDR+0xb + OHIF_ASIC1,64,
};
const CHIP_REG ext_col_gen_m_REG[]=
{
	{EXT_COL_GEN_M,OHOAM_EF_ASIC_BASE_ADDR,0x9,31,0},
};
const CHIP_REG ext_col_gen_base_m_REG[]=
{
	{EXT_COL_GEN_BASE_M,OHOAM_EF_ASIC_BASE_ADDR,0xa,31,0},
};
const CHIP_REG MAC_CTRL_INSERT_OAM_REG[]=
{
	{SOAM_FLEXE_SELF_PLS,OHOAM_EF_ASIC_BASE_ADDR,0x6,0,0},
	{SOAM_MAC_SELF_PLS,OHOAM_EF_ASIC_BASE_ADDR,0x6,1,1},
};
const CHIP_REG flexe_oam_cmp_alm_REG[]=
{
	{FLEXE_OAM_CMP_ALM_0,OHOAM_EF_ASIC_BASE_ADDR,0x0,31,0},
	{FLEXE_OAM_CMP_ALM_1,OHOAM_EF_ASIC_BASE_ADDR,0x1,31,0},
	{FLEXE_OAM_CMP_ALM_2,OHOAM_EF_ASIC_BASE_ADDR,0x2,15,0},
};
const CHIP_REG mac_oam_cmp_alm_REG[]=
{
	{MAC_OAM_CMP_ALM_0,OHOAM_EF_ASIC_BASE_ADDR,0x3,31,0},
	{MAC_OAM_CMP_ALM_1,OHOAM_EF_ASIC_BASE_ADDR,0x4,31,0},
	{MAC_OAM_CMP_ALM_2,OHOAM_EF_ASIC_BASE_ADDR,0x5,15,0},
};
const CHIP_RAM FLEXE_1DM_TEST = 
{
	TABLE_BASE_ADDR,OHOAM_EF_ASIC_BASE_ADDR+0x7,64,
};
const CHIP_RAM MAC_1DM_TEST = 
{
	TABLE_BASE_ADDR,OHOAM_EF_ASIC_BASE_ADDR+0x8,64,
};

/******************************************************************************
*
* FUNCTION	ohoam_ef_soh_ins_getpkt1_set
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
*		type:0:SOH_INS_DRAM_INIT_EN 1:SOH_INST_MODE 
*			 2:SOH_INS_CHKSH_MFI 3:SOH_INS_DBUFF_LVL_CLR 4:SOH_INS_DBUFF_LVL_RPT_SEL
*			 5:SOAM_NO_OVERTIME_PRO 6:SOAM_OVERTIME
*		parameter:type~0:soh ins ram init cfg en 0~0x1 
*				  type~1:soh ins mode cfg.1:100G mode, 0:50G mode
*				  type~2:soh ins chksh mfi
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
RET_STATUS ohoam_ef_soh_ins_getpkt1_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_GETPKT1_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOH_INS_GETPKT1_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_GETPKT1_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,SOH_INS_GETPKT1_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soh_ins_getpkt1_get
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
*		type:0:SOH_INS_DRAM_INIT_EN 1:SOH_INST_MODE 
*			 2:SOH_INS_CHKSH_MFI 3:SOH_INS_DBUFF_LVL_CLR 4:SOH_INS_DBUFF_LVL_RPT_SEL
*		parameter:type~0:soh ins ram init cfg en 0~0x1 
*				  type~1:soh ins mode cfg.1:100G mode, 0:50G mode
*				  type~2:soh ins chksh mfi
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
RET_STATUS ohoam_ef_soh_ins_getpkt1_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if ( chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_GETPKT1_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_GETPKT1_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOH_INS_GETPKT1_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_GETPKT1_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,SOH_INS_GETPKT1_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_loopback_set
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
*		type:0:SOAM_FLEXE_LOOPBACK,flexe oam remote loopback 1:SOAM_MAC_LOOPBACK,mac oam remote loopback
*			 2:MAC_INTE_RAM_INIT_EN,mac oam ram init cfg     3:FLEXE_INTE_RAM_INIT_EN,flexe oam ram init cfg
*			 4:SOAM_FLEXE_CFG_DONE,flexe_oam_cfg_done        5:SOAM_MAC_CFG_DONE,mac_oam_cfg_done
*			 6:ASIC_SOAM_FLEXE_ALM_EXT_EN                    7:ASIC_SOAM_MAC_ALM_EXT_EN
*			 8:SOH_LOOPBACK									 9:SOAM_FLEXE_BAS_EXT_EN
*			 10:SOAM_MAC_BAS_EXT_EN							 11:SOAM_FLEXE_FLVL_CLR
*			 12:SOAM_MAC_FLVL_CLR
*		parameter:0~0x1 
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
RET_STATUS ohoam_ef_soam_loopback_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) ||
		(type > SOAM_MAC_FLVL_CLR) ||
		(parameter > 0x1))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_LOOPBACK_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   type,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOAM_LOOPBACK_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_LOOPBACK_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,SOAM_LOOPBACK_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_loopback_get
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
*		type:0:SOAM_FLEXE_LOOPBACK,flexe oam remote loopback 1:SOAM_MAC_LOOPBACK,mac oam remote loopback
*			 1:MAC_INTE_RAM_INIT_EN,mac oam ram init cfg     3:FLEXE_INTE_RAM_INIT_EN,flexe oam ram init cfg
*			 4:SOAM_FLEXE_CFG_DONE,flexe_oam_cfg_done        5:SOAM_MAC_CFG_DONE,mac_oam_cfg_done
*			 6:ASIC_SOAM_FLEXE_ALM_EXT_EN                    7:ASIC_SOAM_MAC_ALM_EXT_EN
*			 8:SOH_LOOPBACK									 9:SOAM_FLEXE_BAS_EXT_EN
*			 10:SOAM_MAC_BAS_EXT_EN							 11:SOAM_FLEXE_FLVL_CLR
*			 12:SOAM_MAC_FLVL_CLR
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
RET_STATUS ohoam_ef_soam_loopback_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(type > SOAM_MAC_FLVL_CLR))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_LOOPBACK_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_LOOPBACK_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOAM_LOOPBACK_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_LOOPBACK_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,SOAM_LOOPBACK_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_alm_colection_set
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
*		type:0:SOAM_FLEXE_ALM_PRD,flexe,65us 1:SOAM_MAC_ALM_PRD,,65us
*			 2:SOAM_TIMER_STEP_CFG 
*		parameter:0~0xf 
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
RET_STATUS ohoam_ef_soam_alm_colection_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) ||
		(type > SOAM_TIMER_STEP_CFG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_ALM_COLECTION_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOAM_ALM_COLECTION_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_ALM_COLECTION_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,SOAM_ALM_COLECTION_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_alm_colection_get
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
*		type:type:0:SOAM_FLEXE_ALM_PRD,flexe,65us 1:SOAM_MAC_ALM_PRD,,65us
*			 2:SOAM_TIMER_STEP_CFG 
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
RET_STATUS ohoam_ef_soam_alm_colection_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(type > SOAM_TIMER_STEP_CFG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_ALM_COLECTION_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_ALM_COLECTION_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOAM_ALM_COLECTION_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_ALM_COLECTION_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,SOAM_ALM_COLECTION_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_asic_ohif_dasa_set
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
*		type:0:asic_ohif_local_addr 1:asic_ohif_dest_addr
*		field_id:0:bits0~31 1:bits32~47
*		parameter:
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
RET_STATUS ohoam_ef_asic_ohif_dasa_set(UINT_8 chip_id ,UINT_8 type ,UINT_8 field_id,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) ||
		(type > 1)||
		(field_id > 1))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_ASIC_OHIF_DASA_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d ,\
													   \r\n field_id = %d \r\n",
													   chip_id,
													   type,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if(0 == type)
	{
		regp_field_write(chip_id,ASIC_OHIF_DASA_REG[field_id],parameter);
	}
	else
	{
		if(0 == field_id)
		{
			regp_field_write(chip_id,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_0],(parameter&0xffff));
			regp_bit_write(chip_id,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_1].base_addr,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_1].offset_addr,
				15,0,((parameter >> 16)&0xffff));
		}
		else
		{
			regp_bit_write(chip_id,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_1].base_addr,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_1].offset_addr,
				31,16,(parameter&0xffff));
		}
	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_asic_ohif_dasa_get
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
*		type:0:asic_ohif_local_addr 1:asic_ohif_dest_addr
*		field_id:0:bits0~31 1:bits32~47
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
RET_STATUS ohoam_ef_asic_ohif_dasa_get(UINT_8 chip_id ,UINT_8 type ,UINT_8 field_id, UINT_32* parameter)
{
	UINT_32 data = 0;
	if ((chip_id > MAX_DEV) ||
		(type > 1) ||
		(field_id > 1))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_ASIC_OHIF_DASA_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_ASIC_OHIF_DASA_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if(0 == type)
	{
		regp_field_read(chip_id,ASIC_OHIF_DASA_REG[field_id],parameter);
	}
	else
	{
		if(0 == field_id)
		{
			regp_field_read(chip_id,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_0],&data);
			*parameter = data & 0xffff;
			regp_bit_read(chip_id,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_1].base_addr,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_1].offset_addr,
				15,0,&data);
			*parameter |= ((data & 0xffff) << 16);
		}
		else
		{
			regp_bit_read(chip_id,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_1].base_addr,ASIC_OHIF_DASA_REG[ASIC_OHIF_DEST_ADDR_1].offset_addr,
				31,16,parameter);
		}

	}
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	
*
*    	ohoam_ef_soam_ext_dscf_lvl_stat_get
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
*		type:0:SOAM_FLEXE_DSCFLVL_STAT 1:SOAM_MAC_DSCFLVL_STAT 2:SOH_INS_DBUFF_MAX_LVL
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
*  	 dingyi   	    2018-10-18		initial
*
******************************************************************************/
RET_STATUS ohoam_ef_soam_ext_dscf_lvl_stat_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
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
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOAM_EXT_DSCF_LVL_STAT_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,SOAM_EXT_DSCF_LVL_STAT_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soh_ins_alarm_bit_get
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
*		type:0:SOH_INS_PIDERR_ALARM,ins port id err alarm 1:SOH_INS_REQ_FIFO_FULL_ALAM,ins req fifo full alarm 
*			 2:SOH_INS_DA_ERR,ins da err  3:SOHSH_ERR  4:INS_MFI_CON_ERR 5:INS_CBAS_PLN_ERR 6:INS_ETHID_ERR
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
RET_STATUS ohoam_ef_soh_ins_alarm_bit_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_ALARM_BIT_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_ALARM_BIT_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(ins_pkt_port_id_err_reg))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_ALARM_BIT_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,ins_pkt_port_id_err_reg[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soh_ins_alarm_get
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
RET_STATUS ohoam_ef_soh_ins_alarm_get(UINT_8 chip_id ,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_ALARM_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_ALARM_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_read(chip_id,ins_pkt_port_id_err_reg[0].base_addr,ins_pkt_port_id_err_reg[0].offset_addr,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soh_ext_cffull_alarm_bit_get
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
*		type:0~2;0:SOH_EXT_CFFULL_ALARM 1:SOH_INS_DBUFF_EMPTY 2:SOH_INS_DBUFF_FULL
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
RET_STATUS ohoam_ef_soh_ext_cffull_alarm_bit_get(UINT_8 chip_id ,UINT_8 type,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_EXT_CFFULL_ALARM_BIT_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_EXT_CFFULL_ALARM_BIT_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOH_EXT_CFFULL_ALARM_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_EXT_CFFULL_ALARM_BIT_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,SOH_EXT_CFFULL_ALARM_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soh_ext_cffull_alarm_get
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
RET_STATUS ohoam_ef_soh_ext_cffull_alarm_get(UINT_8 chip_id ,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_EXT_CFFULL_ALARM_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_EXT_CFFULL_ALARM_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_read(chip_id,SOH_EXT_CFFULL_ALARM_REG[0].base_addr,SOH_EXT_CFFULL_ALARM_REG[0].offset_addr,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_ext_flexe_alam_bit_get
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
*		type:0:FLEXE_EXT_CHID_ERR_ALAM  1:FLEXE_EXT_SEQ_ILGL_ALAM
*			 2:FLEXE_EXT_SOP_ONLY_ALAM  3:FLEXE_EXT_EOP_ONLY_ALAM
*			 4:FLEXE_EXT_DSC_FIFO_FULL
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
RET_STATUS ohoam_ef_soam_ext_flexe_alam_bit_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(type > FLEXE_SOAM_OVERTIME_ALM))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_FLEXE_ALM_BIT_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_FLEXE_ALM_BIT_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOAM_EXT_FLEXE_ALAM_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_FLEXE_ALM_BIT_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,SOAM_EXT_FLEXE_ALAM_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_ext_flexe_alam_get
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
RET_STATUS ohoam_ef_soam_ext_flexe_alam_get(UINT_8 chip_id ,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_FLEXE_ALM_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_FLEXE_ALM_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_read(chip_id,SOAM_EXT_FLEXE_ALAM_REG[0].base_addr,SOAM_EXT_FLEXE_ALAM_REG[0].offset_addr,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_ext_mac_alam_bit_get
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
*		type:0:MAC_EXT_CHID_ERR_ALAM  1:MAC_EXT_SEQ_ILGL_ALAM
*			 2:MAC_EXT_SOP_ONLY_ALAM  3:MAC_EXT_EOP_ONLY_ALAM
*			 4:MAC_EXT_DSC_FIFO_FULL
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
RET_STATUS ohoam_ef_soam_ext_mac_alam_bit_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(type > MAC_SOAM_OVERTIME_ALM))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_MAC_ALM_BIT_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_MAC_ALM_BIT_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(SOAM_EXT_MAC_ALAM_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_MAC_ALM_BIT_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,SOAM_EXT_MAC_ALAM_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	
*
*    	ohoam_ef_soam_ext_mac_alam_get
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
RET_STATUS ohoam_ef_soam_ext_mac_alam_get(UINT_8 chip_id ,UINT_32* parameter)
{
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_MAC_ALM_GET] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_EXT_MAC_ALM_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_read(chip_id,SOAM_EXT_MAC_ALAM_REG[0].base_addr,SOAM_EXT_MAC_ALAM_REG[0].offset_addr,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_flexe_type_tbl_set
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
*		offset:0~63
*		parameter:0~0xff
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
RET_STATUS ohoam_ef_soam_flexe_type_tbl_set(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV) || 
		(offset > 63))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_FLEXE_TYPE_TBL_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n offset = %d\r\n",
													   chip_id,
													   offset);

		}
		
		return RET_PARAERR;
	}

	if(NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_FLEXE_TYPE_TBL_SET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;

	}
	
	ram_write_single(chip_id,soam_flexe_type_tbl_ram, offset ,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_flexe_type_tbl_get
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
*		offset:0~63
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
RET_STATUS ohoam_ef_soam_flexe_type_tbl_get(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV) || 
		(offset > 63))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_FLEXE_TYPE_TBL_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n offset = %d\r\n",
													   chip_id,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_FLEXE_TYPE_TBL_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	ram_read_single(chip_id,soam_flexe_type_tbl_ram, offset ,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_mac_type_tbl_set
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
*		offset:0~63
*		*parameter:write number
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
RET_STATUS ohoam_ef_soam_mac_type_tbl_set(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV) || 
		(offset > 63))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_MAC_TYPE_TBL_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n offset = %d\r\n",
													   chip_id,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if(NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_MAC_TYPE_TBL_SET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;

	}

	ram_write_single(chip_id,soam_mac_type_tbl_ram, offset ,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_mac_type_tbl_get
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
*		offset:0~63
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
RET_STATUS ohoam_ef_soam_mac_type_tbl_get(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV) || 
		(offset > 63))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_MAC_TYPE_TBL_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n offset = %d\r\n",
													   chip_id,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_MAC_TYPE_TBL_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	ram_read_single(chip_id,soam_mac_type_tbl_ram, offset ,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_ext_gbfifo_lvl_set
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
*		field_id:0~9:0:EXT_GBF_BACKPRE_LVL,ext_gbf_backpre_lvl 1:EXT_GBF_IDLEADD_LVL,ext_gbf_idleadd_lvl
*					 2:B66ENCD_TESTEN  3:B66ENCD_BKFTEN  4:B66ENCD_RFCFG  5:B66ENCD_LFCFG  6:B66ENCD_MANRF
*					 7:B66ENCD_MANLF   8:B66ENCD_MANIDL  9:B66DECODE_SEL
*		parameter:0~0xff cfg value
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
RET_STATUS ohoam_ef_ext_gbfifo_lvl_set(UINT_8 chip_id ,UINT_8 field_id,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) ||
		(field_id > B66DECODE_SEL) ||
		(parameter > 0xff))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_EXT_GBFIFO_LVL_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   field_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(ext_gbfifo_lvl_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_EXT_GBFIFO_LVL_SET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,ext_gbfifo_lvl_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_ext_gbfifo_lvl_get
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
*		field_id:0~9:0:EXT_GBF_BACKPRE_LVL,ext_gbf_backpre_lvl 1:EXT_GBF_IDLEADD_LVL,ext_gbf_idleadd_lvl
*					 2:B66ENCD_TESTEN  3:B66ENCD_BKFTEN  4:B66ENCD_RFCFG  5:B66ENCD_LFCFG  6:B66ENCD_MANRF
*					 7:B66ENCD_MANLF   8:B66ENCD_MANIDL  9:B66DECODE_SEL
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
RET_STATUS ohoam_ef_ext_gbfifo_lvl_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(field_id > B66DECODE_SEL))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_EXT_GBFIFO_LVL_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d\r\n",
													   chip_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_EXT_GBFIFO_LVL_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if (field_id > MAX_INDEX(ext_gbfifo_lvl_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_EXT_GBFIFO_LVL_GET] out of array range\r\n");
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,ext_gbfifo_lvl_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_ext_col_gen_m_set
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
*		field_id:0~1:0:ext_col_gen_m 1:ext_col_gen_base_m
*		parameter:0~0xffffffff cfg value
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
RET_STATUS ohoam_ef_ext_col_gen_m_set(UINT_8 chip_id ,UINT_8 field_id,UINT_32 parameter)
{
	CHIP_REG p;
	if ((chip_id > MAX_DEV) ||
		(field_id > 1) ||
		(parameter > 0xffffffff))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_EXT_COL_GEN_M_SET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d, \
													   \r\n parameter = 0x%x\r\n",
													   chip_id,
													   field_id,
													   parameter);
		}
		
		return RET_PARAERR;
	}

	if(0 == field_id)
	{
		p = ext_col_gen_m_REG[EXT_COL_GEN_M];
	}
	else
	{
		p = ext_col_gen_base_m_REG[EXT_COL_GEN_BASE_M];
	}
	
	regp_field_write(chip_id,p,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_ext_col_gen_m_get
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
*		field_id:0~1:0:ext_col_gen_m 1:ext_col_gen_base_m
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
RET_STATUS ohoam_ef_ext_col_gen_m_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter)
{
	CHIP_REG p;
	if ((chip_id > MAX_DEV) ||
		(field_id > 1))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_EXT_COL_GEN_M_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d\r\n",
													   chip_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_EXT_COL_GEN_M_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if(0 == field_id)
	{
		p = ext_col_gen_m_REG[EXT_COL_GEN_M];
	}
	else
	{
		p = ext_col_gen_base_m_REG[EXT_COL_GEN_BASE_M];
	}
	
	regp_field_read(chip_id,p,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_b66_encode_decode_cnt_get
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
*		field_id:0~2:0:ENCODE_ERR_CNT 1:DECODE_ERR_CNT 2:B66_INS_BYTES_CNT
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
RET_STATUS ohoam_ef_b66_encode_decode_cnt_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(field_id > B66_INS_BYTES_CNT))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_B66_ENCODE_DECODE_CNT_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d\r\n",
													   chip_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_B66_ENCODE_DECODE_CNT_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if(field_id > MAX_INDEX(b66_encode_decode_cnt_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_B66_ENCODE_DECODE_CNT_GET] out of array range\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,b66_encode_decode_cnt_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_pkt_cnt_get
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
*		field_id:0~1:0:B66_DECODE_PKT_CNT 1:EXT_REQ_PKT_CNT
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
RET_STATUS ohoam_ef_pkt_cnt_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(field_id > EXT_REQ_PKT_CNT))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_PKT_CNT_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d\r\n",
													   chip_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_PKT_CNT_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if(field_id > MAX_INDEX(pkt_cnt_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_PKT_CNT_GET] out of array range\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,pkt_cnt_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_ins_crc_err_cnt_get
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
*		field_id:0~1:0:INS_PKT_CRC_ERR_CNT 1:INS_SHORT_PKT_CNT
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
RET_STATUS ohoam_ef_ins_crc_err_cnt_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(field_id > INS_SHORT_PKT_CNT))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if(field_id > MAX_INDEX(ins_crc_err_cnt_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,ins_crc_err_cnt_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_ext_pkt_cnt_get
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
*		field_id:0~1:0:EXT_PKT_SOP_CNT 1:EXT_OAM_REQ_CNT
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
RET_STATUS ohoam_ef_ext_pkt_cnt_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(field_id > EXT_OAM_REQ_CNT))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if(field_id > MAX_INDEX(ext_pkt_cnt_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,ext_pkt_cnt_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_ohif_asic_alm_get
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
*		field_id:0~8:0:EXT_GBF_FULL_ALM 1:EXT_GBF_EMPTY_ALM 2:B66_ENCODE_ERR_ALM
*					 3:INS_GBF_FULL_ALM 4:INS_PREF_FULL_ALM 5:FLEXE_SOAM_REQF_FULL_ALM 
*					 6:MAC_SOAM_REQF_FULL_ALM   7:B66_DECODE_ERR_ALM  8:B66_DECODE_EOP_ABNOR_ALM
*					 9:DE2EN_RF_STATUS 10:DE2EN_LF_STATUS
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
RET_STATUS ohoam_ef_ohif_asic_alm_get(UINT_8 chip_id ,UINT_8 field_id,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV) ||
		(field_id > DE2EN_LF_STATUS))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_OHIF_ASIC_ALM_GET] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n field_id = %d\r\n",
													   chip_id,
													   field_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_OHIF_ASIC_ALM_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}

	if(field_id > MAX_INDEX(ohif_asic_alm_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_OHIF_ASIC_ALM_GET] out of array range\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,ohif_asic_alm_REG[field_id],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_snap_ext_ram_get
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
*		offset:0~63
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
RET_STATUS ohoam_ef_snap_ext_ram_get(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV) || 
		(offset > 63))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n offset = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	ram_read_single(chip_id,SNAP_EXT, offset ,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_snap_ext_ram_print
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
RET_STATUS ohoam_ef_snap_ext_ram_print(UINT_8 chip_id)
{	
	UINT_8 i_cnt;
	UINT_32 u32_data[2] = {0};
	UINT_8 num = 64;
	UINT_8 i = 0; 
	
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   __FUNCTION__,
													   chip_id);

		}
		
		return RET_PARAERR;
	}

	printf("+-------------------+\n");
	for(i_cnt = 0 ; i_cnt < num ; i_cnt++)
	{	
		ohoam_ef_snap_ext_ram_get(chip_id,i_cnt,u32_data);
		
		for(i = 0; i < 2;i++)
		{
			printf("|");
			printf("snap_ext_ram:%d",i_cnt);
			printf("\t");
			printf(" |");
			printf("0x%08x", u32_data[i]);
			printf("|");
			printf("\n");
		}
	}
	printf("+-------------------+\n");

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_snap_ins_ram_get
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
*		offset:0~63
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
RET_STATUS ohoam_ef_snap_ins_ram_get(UINT_8 chip_id ,UINT_8 offset ,UINT_32* parameter)
{	
	if ((chip_id > MAX_DEV) || 
		(offset > 63))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n offset = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	ram_read_single(chip_id,snap_ins, offset ,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_snap_ins_ram_print
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
RET_STATUS ohoam_ef_snap_ins_ram_print(UINT_8 chip_id)
{	
	UINT_8 i_cnt;
	UINT_32 u32_data[2] = {0};
	UINT_8 num = 64;
	UINT_8 i = 0; 

	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   __FUNCTION__,
													   chip_id);

		}
		
		return RET_PARAERR;
	}
	
	printf("+-------------------+\n");
	for(i_cnt = 0 ; i_cnt < num ; i_cnt++)
	{	
		ohoam_ef_snap_ins_ram_get(chip_id,i_cnt,u32_data);
		
		for(i = 0; i < 2;i++)
		{
			printf("|");
			printf("snap_ins_ram:%d",i_cnt);
			printf("\t");
			printf(" |");
			printf("0x%08x", u32_data[i]);
			printf("|");
			printf("\n");
		}
	}
	printf("+-------------------+\n");

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_test_cfg_set
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
*		type:0~4;0:OAM_SEQ_ERR_CHID 1:OAM_SEQ_ERR_EN 2:OAM_INS_MODE 3:SELF_MAX_CHID 4:SET_CRC_ERR
*		parameter:
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
RET_STATUS ohoam_ef_test_cfg_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) ||
		(type > SET_CRC_ERR))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(test_cfg_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
				
		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,test_cfg_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_test_cfg_get
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
*		type:0~4;0:OAM_SEQ_ERR_CHID 1:OAM_SEQ_ERR_EN 2:OAM_INS_MODE 3:SELF_MAX_CHID 4:SET_CRC_ERR
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
RET_STATUS ohoam_ef_test_cfg_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||
		(type > SET_CRC_ERR))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(test_cfg_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,test_cfg_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_test_oh_ext_err_set
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
*		type:0~1;0:TEST_OH_EXT_ERR_0 1:TEST_OH_EXT_ERR_1
*		parameter:
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
RET_STATUS ohoam_ef_test_oh_ext_err_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) ||
		(type > TEST_OH_EXT_ERR_1))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(test_oh_ext_err_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
				
		return RET_PARAERR;
	}
	
	regp_field_write(chip_id,test_oh_ext_err_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_test_oh_ext_err_get
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
*		type:0~1;0:TEST_OH_EXT_ERR_0 1:TEST_OH_EXT_ERR_1
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
RET_STATUS ohoam_ef_test_oh_ext_err_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||
		(type > TEST_OH_EXT_ERR_1))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(test_oh_ext_err_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,test_oh_ext_err_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_test_alm_get
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
*		type:0~5;0:TEST_TOLIUF_FULL_ALM 1:EXT_ETH_DNT_SQT 2:EXT_OAMREQ_DNT_SQT
*				 3:REQ_MFI_CON_ERR  4:EXT_MFI_CON_ERR  5:EXT_SSF_ALM
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
RET_STATUS ohoam_ef_test_alm_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if ((chip_id > MAX_DEV)||
		(type > EXT_SSF_ALM))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(test_alm_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
				
		return RET_PARAERR;
	}
	
	regp_field_read(chip_id,test_alm_REG[type],parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     ohoam_ef_test_all_ram_set
*
* DESCRIPTION 
*
*     ohoam_ef test all ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ram_sel: 0-test_flexe_oam_ins 
*              1-test_mac_oam_ins     
*              2-test_oh_ins      
*              3-test_oh_exp       
*              4-test_flexe_oam_exp    
*              5-test_mac_oam_exp 
*
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
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
*   <author>     <date>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS ohoam_ef_test_all_ram_set(UINT_8 chip_id,UINT_8 ram_sel,UINT_16 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    const CHIP_RAM *p = &test_flexe_oam_ins;
    
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ram_sel > 5) || (index > 5119))
    {
        if (ohoam_ef_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n ram_sel = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    ram_sel,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ohoam_ef_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    switch (ram_sel)
    {
        case 0 : p = &test_flexe_oam_ins; break;
        case 1 : p = &test_mac_oam_ins; break;
        case 2 : 
			p = &test_oh_ins;

			if(index > 2047)
			{
				if (ohoam_ef_para_debug)
				{
					printf("[%s] invalid parameter!! \
							\r\n chip_id = %d, \
							\r\n ram_sel = %d, \
							\r\n index = %d  \r\n",
							__FUNCTION__,
							chip_id,
							ram_sel,
							index
						  );
				}

			}
			
			break;
        case 3 : 
			p = &test_oh_exp; 
			
			if(index > 2047)
			{
				if (ohoam_ef_para_debug)
				{
					printf("[%s] invalid parameter!! \
							\r\n chip_id = %d, \
							\r\n ram_sel = %d, \
							\r\n index = %d  \r\n",
							__FUNCTION__,
							chip_id,
							ram_sel,
							index
						  );
				}

			}
			
			break;

        case 4 : p = &test_flexe_oam_exp; break;
		case 5 : p = &test_mac_oam_exp; break;

        default: break;
    }
        
    rv = ram_write_single(chip_id,*p,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ohoam_ef_test_all_ram_get
*
* DESCRIPTION 
*
*     ohoam_ef test all ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ram_sel: 0-test_flexe_oam_ins 
*              1-test_mac_oam_ins     
*              2-test_oh_ins      
*              3-test_oh_exp       
*              4-test_flexe_oam_exp    
*              5-test_mac_oam_exp 
*              6-test_oh_ext 
*              7-test_flexe_oam_ext  
*              8-test_mac_oam_ext 
*
*     index: 0~CH_NUM - 1
*    *parameter: pointer to value
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
*   <author>     <date>    <CR_ID>      <DESCRIPTION>
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS ohoam_ef_test_all_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_16 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    const CHIP_RAM *p = &test_flexe_oam_ins;
    
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ram_sel > 8) || (index > 5119))
    {
        if (ohoam_ef_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n ram_sel = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    ram_sel,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ohoam_ef_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    switch (ram_sel)
    {
        case 0 : p = &test_flexe_oam_ins; break;
        case 1 : p = &test_mac_oam_ins; break;
        case 2 : 
			p = &test_oh_ins;

			if(index > 2047)
			{
				if (ohoam_ef_para_debug)
				{
					printf("[%s] invalid parameter!! \
							\r\n chip_id = %d, \
							\r\n ram_sel = %d, \
							\r\n index = %d  \r\n",
							__FUNCTION__,
							chip_id,
							ram_sel,
							index
						  );
				}

			}
			
			break;
        case 3 : 
			p = &test_oh_exp; 
			
			if(index > 2047)
			{
				if (ohoam_ef_para_debug)
				{
					printf("[%s] invalid parameter!! \
							\r\n chip_id = %d, \
							\r\n ram_sel = %d, \
							\r\n index = %d  \r\n",
							__FUNCTION__,
							chip_id,
							ram_sel,
							index
						  );
				}

			}
			
			break;

        case 4 : p = &test_flexe_oam_exp; break;
		case 5 : p = &test_mac_oam_exp; break;
		case 6 :
			p = &test_oh_ext; 

			if(index > 2047)
			{
				if (ohoam_ef_para_debug)
				{
					printf("[%s] invalid parameter!! \
							\r\n chip_id = %d, \
							\r\n ram_sel = %d, \
							\r\n index = %d  \r\n",
							__FUNCTION__,
							chip_id,
							ram_sel,
							index
						  );
				}

			}
			
			break;
		case 7 : p = &test_flexe_oam_ext; break;
        case 8 : p = &test_mac_oam_ext; break;
        default: break;
    }
        
    rv = ram_read_single(chip_id,*p,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION	
*
*    	ohoam_ef_asic_test_flexe_oam_cmp_alm_get
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
*		ch_id:0~CH_NUM - 1
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
*  	  dingyi   	    2018-09-19		   initial
*
******************************************************************************/
RET_STATUS ohoam_ef_asic_test_flexe_oam_cmp_alm_get(UINT_8 chip_id ,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n ch_id = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   ch_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id, flexe_oam_cmp_alm_REG[ch_id/32].base_addr, flexe_oam_cmp_alm_REG[ch_id/32].offset_addr,
		ch_id%32, ch_id%32, parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	
*
*    	ohoam_ef_asic_test_mac_oam_cmp_alm_get
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
*		ch_id:0~CH_NUM - 1
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
*  	  dingyi   	    2018-09-19		   initial
*
******************************************************************************/
RET_STATUS ohoam_ef_asic_test_mac_oam_cmp_alm_get(UINT_8 chip_id ,UINT_8 ch_id,UINT_32* parameter)
{
	if (chip_id > MAX_DEV || ch_id > (CH_NUM - 1))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n ch_id = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   ch_id);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id, mac_oam_cmp_alm_REG[ch_id/32].base_addr, mac_oam_cmp_alm_REG[ch_id/32].offset_addr,
		ch_id%32, ch_id%32, parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_asic_test_mac_ctrl_insert_oam_set
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
*		type:0~1;0:SOAM_FLEXE_SELF_PLS 1:SOAM_MAC_SELF_PLS 
*		parameter:
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
RET_STATUS ohoam_ef_asic_test_mac_ctrl_insert_oam_set(UINT_8 chip_id ,UINT_8 type )
{
	if ((chip_id > MAX_DEV) ||
		(type > SOAM_MAC_SELF_PLS))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (type > MAX_INDEX(MAC_CTRL_INSERT_OAM_REG))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] out of array range\r\n",__FUNCTION__);
		}
				
		return RET_PARAERR;
	}
	
	regp_write(chip_id,MAC_CTRL_INSERT_OAM_REG[type].base_addr,MAC_CTRL_INSERT_OAM_REG[type].offset_addr,1<<type);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_asic_test_1dm_test_get
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
*		type:0~1 0:flexe_1dm_test 1:mac_1dm_test
*		offset:0~63
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
RET_STATUS ohoam_ef_asic_test_1dm_test_get(UINT_8 chip_id ,UINT_8 type,UINT_8 offset ,UINT_32* parameter)
{	
    const CHIP_RAM *p = &FLEXE_1DM_TEST;

	if ((chip_id > MAX_DEV) || 
		(type >1)||
		(offset > 63))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d, \
													   \r\n offset = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   type,
													   offset);

		}
		
		return RET_PARAERR;
	}
	
	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
		}
		
		return RET_PARAERR;
	}

	switch(type)
	{
		case 0:
			p = &FLEXE_1DM_TEST;
			break;
		case 1:
			p = &MAC_1DM_TEST;
			break;
		default:
			break;
	}

	ram_read_single(chip_id,*p, offset ,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	ohoam_ef_soam_init
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
RET_STATUS ohoam_ef_soam_init(UINT_8 chip_id )
{	
	UINT_8 i = 0;
	UINT_32 reg[7] = {0x10,0x27,0x43,0x63,0x87,0x30,0xef};
			
	if (chip_id > MAX_DEV)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOAM_INIT] invalid parameter!! \
													   \r\n chip_id = %d\r\n",
													   chip_id);

		}
		
		return RET_PARAERR;
	}

	ohoam_ef_soh_ins_getpkt1_set(chip_id,SOH_INS_DRAM_INIT_EN,1);/*set done*/

	/*set MAC_INTE_RAM_INIT_EN & FLEXE_INTE_RAM_INIT_EN */
	ohoam_ef_soam_loopback_set(chip_id,MAC_INTE_RAM_INIT_EN,0); /*first clear*/
	ohoam_ef_soam_loopback_set(chip_id,FLEXE_INTE_RAM_INIT_EN,0);

	ohoam_ef_soam_loopback_set(chip_id,MAC_INTE_RAM_INIT_EN,1); /*set enable*/
	ohoam_ef_soam_loopback_set(chip_id,FLEXE_INTE_RAM_INIT_EN,1);

        ohoam_ef_soh_ins_getpkt1_set(chip_id,SOAM_OVERTIME,0xa);

	for(i = 0; i <= 63 ;i++)
	{
		switch(i)
		{
			case 0x1:
			case 0x2:
				ohoam_ef_soam_flexe_type_tbl_set(chip_id,i,&reg[0]);
				ohoam_ef_soam_mac_type_tbl_set(chip_id,i,&reg[0]);
				break;
			case 0x11:
				ohoam_ef_soam_flexe_type_tbl_set(chip_id,i,&reg[1]);
				ohoam_ef_soam_mac_type_tbl_set(chip_id,i,&reg[1]);
				break;
			case 0x12:
				ohoam_ef_soam_flexe_type_tbl_set(chip_id,i,&reg[2]);
				ohoam_ef_soam_mac_type_tbl_set(chip_id,i,&reg[2]);
				break;
			case 0x13:
				ohoam_ef_soam_flexe_type_tbl_set(chip_id,i,&reg[3]);
				ohoam_ef_soam_mac_type_tbl_set(chip_id,i,&reg[3]);
				break;
			case 0x14:
				ohoam_ef_soam_flexe_type_tbl_set(chip_id,i,&reg[4]);
				ohoam_ef_soam_mac_type_tbl_set(chip_id,i,&reg[4]);
				break;
			case 0x15:
				ohoam_ef_soam_flexe_type_tbl_set(chip_id,i,&reg[5]);
				ohoam_ef_soam_mac_type_tbl_set(chip_id,i,&reg[5]);
				break;
			default:
				ohoam_ef_soam_flexe_type_tbl_set(chip_id,i,&reg[6]);
				ohoam_ef_soam_mac_type_tbl_set(chip_id,i,&reg[6]);
				break;
		}
	}

	ohoam_ef_soam_loopback_set(chip_id,SOAM_FLEXE_CFG_DONE,1); /*set done*/
	ohoam_ef_soam_loopback_set(chip_id,SOAM_MAC_CFG_DONE,1);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soh_inst_mode_set
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
*		type:0~7
*		parameter:soh ins mode cfg.1:100G mode, 0:50G mode
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
RET_STATUS ohoam_ef_soh_inst_mode_set(UINT_8 chip_id ,UINT_8 type ,UINT_32 parameter)
{
	if ((chip_id > MAX_DEV) || (type > (PHY_NUM - 1)))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_write(chip_id,SOH_INS_GETPKT1_REG[SOH_INST_MODE].base_addr,SOH_INS_GETPKT1_REG[SOH_INST_MODE].offset_addr,
	SOH_INS_GETPKT1_REG[SOH_INST_MODE].start_bit+type,SOH_INS_GETPKT1_REG[SOH_INST_MODE].start_bit+type,parameter);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	ohoam_ef_soh_inst_mode_get
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
*		type:0~7
*		parameter:soh ins mode cfg.1:100G mode, 0:50G mode
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
RET_STATUS ohoam_ef_soh_inst_mode_get(UINT_8 chip_id ,UINT_8 type ,UINT_32* parameter)
{
	if (( chip_id > MAX_DEV) || (type > (PHY_NUM - 1)))
	{
		if (ohoam_ef_para_debug)
		{
			printf("[%s] invalid parameter!! \
													   \r\n chip_id = %d, \
													   \r\n type = %d\r\n",
													   __FUNCTION__,
													   chip_id,
													   type);
		}
		
		return RET_PARAERR;
	}

	if (NULL == parameter)
	{
		if (ohoam_ef_para_debug)
		{
			printf("[OHOAM_EF_SOH_INS_GETPKT1_GET] invalid parameter pointer\r\n");
		}
		
		return RET_PARAERR;
	}
	
	regp_bit_read(chip_id,SOH_INS_GETPKT1_REG[SOH_INST_MODE].base_addr,SOH_INS_GETPKT1_REG[SOH_INST_MODE].offset_addr,
	SOH_INS_GETPKT1_REG[SOH_INST_MODE].start_bit+type,SOH_INS_GETPKT1_REG[SOH_INST_MODE].start_bit+type,parameter);
	
	return RET_SUCCESS;
}
/******************************************************************************
*
* FUNCTION	ohoam_ef_oam_flexe_set_debug
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
*   	start_chid:
*   	end_chid:
*   	loop_num:
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
RET_STATUS ohoam_ef_oam_flexe_set_debug(UINT_8 start_chid,UINT_8 end_chid,UINT_8 loop_num)
{	
	UINT_32 reg_data = 0;
	UINT_32 ins_cnt = 0;
	UINT_32 ext_cnt = 0;
	UINT_8 type = 0;
	UINT_8 ch_id = 0;
	UINT_32 cnt = 0;
	UINT_32 i = 0;
	UINT_8 j = 1;
	srand(time(NULL));
	
	while(loop_num)
	{
		
		for(ch_id = start_chid;ch_id<end_chid+1;ch_id++)
		{
			regp_bit_write(0,0xe070,0xf,12,6,ch_id);
			regp_bit_write(0,0xe070,0xf,25,19,ch_id);
			
			for(;type < 32;type ++)
			{				
				regp_bit_write(0,0xe070,0xf,5,0,type);
				regp_bit_write(0,0xe070,0xf,18,13,type);
				
				regp_read(0,0xe070,0xd,&reg_data);
				regp_read(0,0xe070,0xe,&reg_data);
                /* coverity[dont_call : FALSE] */
				cnt = rand()%65535+1;
				for(i = 0;i<cnt;i++)
				{
					usleep(400);
					regp_write(0,0xe000,0x86,0x3);
					usleep(400);
				}

				usleep(400);
				regp_read(0,0xe070,0xd,&reg_data);
				ins_cnt = reg_data&MASK_16_BIT;
				ext_cnt = (reg_data >> SHIFT_BIT_16)&MASK_16_BIT;
				if((ins_cnt != cnt) || (ext_cnt != cnt))
				{
					printf("soam_flexe_err!loop_num = %d,ch_id= %d,type = %d,cnt = %d,ins_cnt = %d,ext_cnt = %d\r\n",
						loop_num,ch_id,type,cnt,ins_cnt,ext_cnt);
					goto err;
				}

				regp_read(0,0xe070,0xe,&reg_data);
				ins_cnt = reg_data&MASK_16_BIT;
				ext_cnt = (reg_data >> SHIFT_BIT_16)&MASK_16_BIT;
				if((ins_cnt != cnt) || (ext_cnt != cnt))
				{
					printf("soam_mac_err!loop time:%d,ch_id= %d,type = %d,cnt = %d,ins_cnt = %d,ext_cnt = %d\r\n",
						j,ch_id,type,cnt,ins_cnt,ext_cnt);
					goto err;
				}
				
			}
		}
		j++;
		loop_num --;
	}
	printf("SUCCESS!\r\n");
	
	return RET_SUCCESS;

	err:
		return RET_FAIL;

}

/******************************************************************************
*
* FUNCTION	ohoam_ef_sw66_cmd_debug
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
*   	loop_num:
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
RET_STATUS ohoam_ef_sw66_cmd_debug(UINT_8 loop_num)
{	
	UINT_32 reg_data[16][2] = {{0}};
	UINT_32 parameter = 0;
	UINT_8 i = 0;
	UINT_8 j = 1;
	srand(time(NULL));
	
	while(loop_num)
	{
		for(i = 0;i < 16;i++)
		{
            /* coverity[dont_call : FALSE] */
			reg_data[i][0] = rand()%0xffffffff;
            /* coverity[dont_call : FALSE] */
			reg_data[i][1] = rand()%0xffffffff;
			ram_write(0,0x20000,0xe08c,i,0x40,reg_data[i]);
			ram_write(0,0x20000,0xe08c,i + 16,0x40,reg_data[i]);
		}

		usleep(50);
		regp_bit_write(0,0xe000,0xa,7,0,0);

		usleep(30);
		regp_bit_write(0,0xe070,0x2,21,21,1);

		usleep(30);
		regp_bit_write(0,0xe070,0x2,23,23,0);
		regp_bit_write(0,0xe070,0x2,23,23,1);

		usleep(30);
		regp_bit_write(0,0xe070,0x2,22,22,0);
		regp_bit_write(0,0xe070,0x2,22,22,1);


		for(i = 0;i < 16;i++)
		{
			regp_read(0,0x4a210,i,&parameter);
			if(parameter != reg_data[i/2][i%2])
			{
				printf("ERROR!loop time:%d,0x4a21%x = 0x%x,reg_data[%x] = 0x%x\r\n",
						j,i,parameter,i,reg_data[i/2][i%2]);
				goto err;
			}
		}
		j++;
		loop_num --;
	}
	printf("SUCCESS!\r\n");
	
	return RET_SUCCESS;

	err:
		return RET_FAIL;

}

UINT_32 mask_set(UINT_8 num)
{
	if(num == 1)
	{
		return 1;
	}
	else if(num == 0)
	{
		return 0;
	}
	else
	{
		return (((mask_set(num-1) << 1) | 0x1) );
	}
}

void diag_print_cnt_1(const char *cnt1, char *val1, const char *cnt2, char *val2,UINT_8 cnt)
{
    if (NULL != cnt1 && NULL != cnt2)
        printf("| %26s%2d = %-20s | %26s%2d = %-20s |\n", cnt1,cnt, val1, cnt2,cnt ,val2);
    else if (NULL == cnt1 && NULL != cnt2)
        printf("| %26s%2d   %-20s | %26s%2d = %-20s |\n", " ", cnt," ", cnt2, cnt,val2);
    else if (NULL != cnt1 && NULL == cnt2)
        printf("| %26s%2d = %-20s | %26s%2d   %-20s |\n", cnt1,cnt, val1, " ",cnt, " ");
    else
        printf("| %26s%2d   %-20s | %26s%2d   %-20s |\n", " ",cnt, " ", " ",cnt, " ");
}

void binary(UINT_32 val,char* str,UINT_8 len)
{
	UINT_8 i = 0;
	UINT_8 blank_num = (len-1)/4;
	str[len + blank_num] = '\0';
	for(i = 0; i < len; i++)
	{
		str[len-1-i + blank_num] = ((val >> (i)) & 1) + ('1' - 1);
		/*printf("str[%d] = %c\r\n",len-1-i + blank_num,str[len-1-i + blank_num]);*/
		if((i+1)%4 == 0)
		{
			str[len-1-i + blank_num-1] = '_';
			/*printf("   str[%d] = %c\r\n",len-1-i + blank_num-1,str[len-1-i + blank_num-1]);*/
			blank_num --;
		}
	}
}

UINT_8 alm_rpt_debug = 0;
void ohoam_ef_alm_rpt_get(UINT_8 chip_id)
{
	UINT_32 reg_data[41][2] = {{0}};
	UINT_8 i = 0;
	UINT_32 val[2] = {0};
	char cntbuf[2][36] = {{0}};

	UINT_8 start_num = 0;
	UINT_8 start_offset = 0;
	UINT_8 start_val = 0;

	for(i = 0; i < 20; i++)
	{
        ram_read(0x0,0x20000,0xe08b,i,0x40,reg_data[i]);
        ram_read(0x0,0x20000,0xe08b,i+32,0x40,reg_data[i+20]);
		if(alm_rpt_debug)
		{
			printf("reg_data[%d][0]=0x%x,reg_data[%d][1]=0x%x\r\n",i+20,reg_data[i+20][0],i+20,reg_data[i+20][1]);
		}	
	}

	diag_print_line();
	diag_print_2_title("FLEXE", "MAC");
	/*diag_print_title("grp_alm");*/
	diag_print_line();


	val[0] = (reg_data[0][1] >> 24) & 0xff;
	val[1] = (reg_data[20][1] >> 24) & 0xff;
	/*diag_cnt_val_to_str(&val[0],1,1,cntbuf[0],32);*/
	/*diag_cnt_val_to_str(&val[1],1,1,cntbuf[1],32);*/
	binary(val[0],cntbuf[0],8);
	binary(val[1],cntbuf[1],8);
	diag_print_cnt("flexe_grp_alm", cntbuf[0], "mac_grp_alm", cntbuf[1]);
	diag_print_line();

	start_num = 24;
	start_offset = 0;
	start_val = 1;

	diag_print_title("OH_ALM");
	for(i = 0; i < 8; i++)
	{
		if(start_num >= 28)
		{
			val[0] = (reg_data[start_offset][start_val] >> (start_num - 28)) & 0xfffffff;
			val[1] = (reg_data[start_offset+20][start_val] >> (start_num - 28)) & 0xfffffff;
			start_num = start_num - 28;
		}
		else
		{
			val[0] = ((reg_data[start_offset][start_val] & mask_set(start_num)) << (28 - start_num)) |
					((reg_data[start_val?start_offset:(start_offset+1)][start_val?0:1] >> (4 + start_num)) & mask_set(28-start_num));
			val[1] = ((reg_data[start_offset+20][start_val] & mask_set(start_num)) << (28 - start_num)) |
					((reg_data[start_val?(start_offset+20):(start_offset+21)][start_val?0:1] >> (4 + start_num)) & mask_set(28-start_num));
			start_num = start_num + 4;
			start_offset = start_val?start_offset:(start_offset+1);
			start_val = start_val?0:1;
		}

		/*diag_cnt_val_to_str(&val[0],1,1,cntbuf[0],32);*/
		/*diag_cnt_val_to_str(&val[1],1,1,cntbuf[1],32);*/
		binary(val[0],cntbuf[0],28);
		binary(val[1],cntbuf[1],28);
		diag_print_cnt_1("flexe_alm", cntbuf[0], "mac_alm",cntbuf[1], 7-i);

		/*printf("id= %d,start_offset = %d,start_val = %d,start_num = %d\r\n",79-i,start_offset,start_val,start_num);*/
	}

	diag_print_line();
	diag_print_title("OAM_CLIENT_ALM");
	/*printf("0x%lx",mask_set(13));*/

	start_num = 24;
	start_offset = 3;
	start_val = 0;

	for(i = 0; i < 80; i++)
	{
		if(start_num >= 13)
		{
			val[0] = (reg_data[start_offset][start_val] >> (start_num - 13)) & 0x1fff;
			val[1] = (reg_data[start_offset+20][start_val] >> (start_num - 13)) & 0x1fff;
			start_num = start_num - 13;
		}
		else
		{
			val[0] = ((reg_data[start_offset][start_val] & mask_set(start_num)) << (13 - start_num)) |
					((reg_data[start_val?start_offset:(start_offset+1)][start_val?0:1] >> (19 + start_num)) & mask_set(13-start_num));
            /* coverity[large_shift : FALSE] */
			val[1] = ((reg_data[start_offset+20][start_val] & mask_set(start_num)) << (13 - start_num)) |
					((reg_data[start_val?(start_offset+20):(start_offset+21)][start_val?0:1] >> (19 + start_num)) & mask_set(13-start_num));
			start_num = start_num + 19;
			start_offset = start_val?start_offset:(start_offset+1);
			start_val = start_val?0:1;
		}

		/*diag_cnt_val_to_str(&val[0],1,1,cntbuf[0],32);*/
		/*diag_cnt_val_to_str(&val[1],1,1,cntbuf[1],32);*/
		binary(val[0],cntbuf[0],13);
		binary(val[1],cntbuf[1],13);
		diag_print_cnt_1("flexe_alm", cntbuf[0], "mac_alm",cntbuf[1], 79-i);

		/*printf("id= %d,start_offset = %d,start_val = %d,start_num = %d\r\n",79-i,start_offset,start_val,start_num);*/
	}
	diag_print_line();

}


void ohoam_ef_flexe_oam_rx_alm_get(UINT_8 chip_id)
{
	UINT_8 ch_index = 0;
	UINT_8 alm_status = 0;
	UINT_32 status = 0;
	UINT_8 start_num = 0;
	UINT_8 start_offset = 0;
	UINT_8 start_val = 0;
	UINT_32 val[2] = {0};
	UINT_8 i = 0;
	char cntbuf[2][21] = {{0}};
	UINT_32 reg_data[21][2] = {{0}};
	/*UINT_8 flag = 0;*/

	start_num = 24;
	start_offset = 3;
	start_val = 0;

	for(i = 0; i < 20; i++)
	{
		/*ohoam_ef_asic_test_alm_pkt_rpt_get(chip_id,i,reg_data[i]);*/
		ram_read(0x0,0x20000,0xe08b,i,0x40,reg_data[i]);
		printf("reg_data[%d][0]=0x%x,reg_data[%d][1]=0x%x\r\n",i,reg_data[i][0],i,reg_data[i][1]);
	}

	flexe_oam_rx_ch_alm_init(chip_id);

	diag_print_line();
	diag_print_2_title("OHOAM_EF", "FLEXE_OAM_RX");
	/*diag_print_title("grp_alm");*/
	diag_print_line();

	for(ch_index = 0;ch_index < CH_NUM;ch_index++)
	{
		status = 0;
		/*flexe_oam_rx_lpi_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_lpi[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_lpi[chip_id].p_ch_alm, &alm_status);
		status |= (alm_status&MASK_1_BIT);

		/*flexe_oam_rx_rf_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rf_alm[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rf_alm[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 1);

		/*flexe_oam_rx_rdi_lf_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rdi_lf_alm[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rdi_lf_alm[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 2);

		/*flexe_oam_rx_sdbei_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_sdbei[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_sdbei[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 3);

		/*flexe_oam_rx_sdbip_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_sdbip[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_sdbip[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 4);

		/*flexe_oam_rx_bas_rdi_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_rdi[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_rdi[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 5);

		/*flexe_oam_rx_bas_csf_lpi_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_csf_lpi[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_csf_lpi[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 6);

		/*flexe_oam_rx_bas_cs_rf_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_cs_rf[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_cs_rf[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 7);

		/*flexe_oam_rx_bas_cs_lf_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_cs_lf[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_cs_lf[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 8);

		/*flexe_oam_rx_bas_no_receive_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_no_receive_alm[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_no_receive_alm[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 9);

		/*flexe_oam_rx_sfbei_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_sfbei[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_sfbei[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 10);

		/*flexe_oam_rx_sfbip_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_sfbip[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_sfbip[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 11);

		/*flexe_oam_rx_bas_period_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_period_alm[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_period_alm[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 12);

		val[1] = status;

		if(start_num/13)
		{
			val[0] = (reg_data[start_offset][start_val] >> (start_num - 13)) & 0x1fff;
			start_num = start_num - 13;
		}
		else
		{
			val[0] = 79-ch_index;
		}

		/*binary(val[0],cntbuf[0],13);*/
		diag_cnt_val_to_str(&val[0],1,0,cntbuf[0],21);
		binary(val[1],cntbuf[1],13);
		diag_print_cnt_1("flexe_oam_alm", cntbuf[0], "flexe_oam_alm ",cntbuf[1], 79-ch_index);
	}

	diag_print_line();
}

void ohoam_ef_alm_rpt_compare_get(UINT_8 chip_id)
{
	UINT_8 ch_index = 0;
	UINT_8 alm_status = 0;
	UINT_32 status = 0;
	UINT_8 start_num = 0;
	UINT_8 start_offset = 0;
	UINT_8 start_val = 0;
	UINT_32 val[2] = {0};
	UINT_8 i = 0;
	char cntbuf[2][21] = {{0}};
	UINT_32 reg_data[21][2] = {{0}};
	UINT_8 flag = 0;

	start_num = 24;
	start_offset = 3;
	start_val = 0;

	for(i = 0; i < 20; i++)
	{
		ram_read(0x0,0x20000,0xe08b,i,0x40,reg_data[i]);
	}

	
	diag_print_line();
	diag_print_2_title("OHOAM_EF", "FLEXE_OAM_RX");
	/*diag_print_title("grp_alm");*/
	diag_print_line();

	for(ch_index = 0;ch_index < CH_NUM;ch_index++)
	{
		status = 0;
		/*flexe_oam_rx_lpi_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_lpi[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_lpi[chip_id].p_ch_alm, &alm_status);
		status |= (alm_status&MASK_1_BIT);

		/*flexe_oam_rx_rf_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rf_alm[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rf_alm[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 1);

		/*flexe_oam_rx_rdi_lf_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rdi_lf_alm[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rdi_lf_alm[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 2);

		/*flexe_oam_rx_sdbei_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_sdbei[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_sdbei[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 3);

		/*flexe_oam_rx_sdbip_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_sdbip[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_sdbip[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 4);

		/*flexe_oam_rx_bas_rdi_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_rdi[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_rdi[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 5);

		/*flexe_oam_rx_bas_csf_lpi_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_csf_lpi[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_csf_lpi[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 6);

		/*flexe_oam_rx_bas_cs_rf_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_cs_rf[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_cs_rf[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 7);

		/*flexe_oam_rx_bas_cs_lf_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_cs_lf[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_cs_lf[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 8);

		/*flexe_oam_rx_bas_no_receive_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_no_receive_alm[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_no_receive_alm[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 9);

		/*flexe_oam_rx_sfbei_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_sfbei[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_sfbei[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 10);

		/*flexe_oam_rx_sfbip_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_rx_sfbip[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_rx_sfbip[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 11);

		/*flexe_oam_rx_bas_period_alm_get(chip_id,79-ch_index,&alm_status);*/
		flexe_bas_period_alm[chip_id].p_ch_alm_report(chip_id, 79-ch_index, flexe_bas_period_alm[chip_id].p_ch_alm, &alm_status);
		status |= ((alm_status&MASK_1_BIT) << 12);

		val[1] = status;

        /* coverity[cond_at_most : FALSE] */
		if(start_num >= 13)
		{
			val[0] = (reg_data[start_offset][start_val] >> (start_num - 13)) & 0x1fff;
			start_num = start_num - 13;
		}
		else
		{
            /* coverity[large_shift : FALSE] */
			val[0] = ((reg_data[start_offset][start_val] & mask_set(start_num)) << (13 - start_num)) |
					((reg_data[start_val?start_offset:(start_offset+1)][start_val?0:1] >> (19 + start_num)) & mask_set(13-start_num));
			start_num = start_num + 19;
			start_offset = start_val?start_offset:(start_offset+1);
			start_val = start_val?0:1;
		}

		if(val[0] != val[1])
		{
			/*diag_cnt_val_to_str(&val[0],1,1,cntbuf[0],32);*/
			/*diag_cnt_val_to_str(&val[1],1,1,cntbuf[1],32);*/
			binary(val[0],cntbuf[0],13);
			binary(val[1],cntbuf[1],13);
			diag_print_cnt_1("ohoam_ef_alm", cntbuf[0], "flexe_oam_alm",cntbuf[1], 79-ch_index);
			if(flag == 0)
			{
				flag = 1;
			}
		}

	}
	if(flag == 0)
	{
		diag_print_title("SUCCESS!");
	}
	diag_print_line();
}

/******************************************************************************
*
* FUNCTION	set_mblk2sblk_debug
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
RET_STATUS set_mblk2sblk_debug (UINT_8 min_chid,UINT_8 max_chid,UINT_8 times)
{
	UINT_8 i = 0;
	UINT_32 delay_time = 0;
	UINT_32 soam_flexe_cnt = 0;
	UINT_16 flexe_ins_cnt = 0;
	UINT_16 flexe_ext_cnt = 0;
	UINT_32 soam_mac_cnt = 0;
	UINT_16 mac_ins_cnt = 0;
	UINT_16 mac_ext_cnt = 0;

	srand(time(NULL));

	diag_print_line();
	diag_print_2_title("SOAM_FLEXE_CNT", "SOAM_MAC_CNT");
	diag_print_line();

	for(i = min_chid; i <= max_chid; i++)
	{
	    regp_bit_write(0,0xe000,0x72,18,17,0x3);
	    regp_bit_write(0,0xe070,0xf,12, 6,i);
	    regp_bit_write(0,0xe070,0xf,25,19,i);
	    regp_bit_write(0,0xe000,0x72,18,17,0x0);

	    /*random time 1ms-1s*/
        /* coverity[dont_call : FALSE] */
	    delay_time = rand()%1000+1;
	    usleep(delay_time*1000);
	    regp_bit_write(0,0xe000,0x72,18,17,0x3);

	    regp_read(0x0,0xe070,0xd,&soam_flexe_cnt);
	    flexe_ins_cnt = soam_flexe_cnt & 0xffff;
	    flexe_ext_cnt = (soam_flexe_cnt >> SHIFT_BIT_16) & MASK_16_BIT;

	    regp_read(0x0,0xe070,0xe,&soam_mac_cnt);
	    mac_ins_cnt = soam_mac_cnt & 0xffff;
	    mac_ext_cnt = (soam_mac_cnt >> SHIFT_BIT_16) & MASK_16_BIT;

	    if((flexe_ext_cnt == (UINT_16)(flexe_ins_cnt*times))&&(mac_ext_cnt == (UINT_16)(mac_ins_cnt*times)))
	    {
		    printf("| %12s%2d = 0x%-6x %9s%2d = 0x%-5x | %12s%2d = 0x%-6x %9s%2d = 0x%-5x |\n", "PASS!INS_CNT CH", i,flexe_ins_cnt, "EXT_CNT CH" ,i,flexe_ext_cnt,"PASS!INS_CNT CH", i,mac_ins_cnt, "EXT_CNT CH" ,i,mac_ext_cnt);

		    diag_print_line();

	    }
	    else if(flexe_ext_cnt == (UINT_16)(flexe_ins_cnt*times))
	    {
	    	printf("| %12s%2d = 0x%-6x %12s%2d = 0x%-6x | %12s%2d = 0x%-6x %10s%2d = 0x%-6x |\n", "INS_CNT CH", i,flexe_ins_cnt, "EXT_CNT CH" ,i,flexe_ext_cnt,"ERR!INS_CNT CH", i,mac_ins_cnt, "EXT_CNT CH" ,i,mac_ext_cnt);
	    	diag_print_line();
	    	diag_print_title("CH = %2d FAIL!!!",i);
	    	diag_print_line();
	    	return RET_FAIL;
	    }
	    else if(mac_ext_cnt == (UINT_16)(mac_ins_cnt*times))
	    {
	    	printf("| %12s%2d = 0x%-6x %10s%2d = 0x%-6x | %12s%2d = 0x%-6x %12s%2d = 0x%-6x |\n", "ERR!INS_CNT CH", i,flexe_ins_cnt, "EXT_CNT CH" ,i,flexe_ext_cnt,"INS_CNT CH", i,mac_ins_cnt, "EXT_CNT CH" ,i,mac_ext_cnt);
	    	diag_print_line();
	    	diag_print_title("CH = %2d FAIL!!!",i);
	    	diag_print_line();
	    	return RET_FAIL;
	    }
	    else
	    {
	    	printf("| %12s%2d = 0x%-6x %10s%2d = 0x%-6x | %12s%2d = 0x%-6x %10s%2d = 0x%-6x |\n", "ERR!INS_CNT CH", i,flexe_ins_cnt, "EXT_CNT CH" ,i,flexe_ext_cnt,"ERR!INS_CNT CH", i,mac_ins_cnt, "EXT_CNT CH" ,i,mac_ext_cnt);
	    	diag_print_line();
	    	diag_print_title("CH = %2d FAIL!!!",i);
	    	diag_print_line();
	    	return RET_FAIL;
	    }
	}

	diag_print_title("PASS all channel!");
	diag_print_line();
	return RET_SUCCESS;
}
