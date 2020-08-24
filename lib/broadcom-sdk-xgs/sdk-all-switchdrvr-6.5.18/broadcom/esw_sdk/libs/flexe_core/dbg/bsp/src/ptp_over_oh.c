

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
unsigned long int ptp_over_oh_debug = 0;

const CHIP_REG IEEE1588_GLOBAL_CFG_REG[]=
{
	{PTP_1588_MODE,GLOBAL_IEEE1588_BASE_ADDR,0x0,1,0},
	{RX_TIMESTAMP_MODE,GLOBAL_IEEE1588_BASE_ADDR,0x0,2,2},
	{RX_TS_SRC_MODE,GLOBAL_IEEE1588_BASE_ADDR,0x0,3,3},
	{RX_TS_SAMPLE_MODE,GLOBAL_IEEE1588_BASE_ADDR,0x0,4,4},
	{TX_TS_SRC_MODE,GLOBAL_IEEE1588_BASE_ADDR,0x0,5,5},
	{TX_TS_SAMPLE_MODE,GLOBAL_IEEE1588_BASE_ADDR,0x0,6,6},
	{CF_THRESHOLD_EN,GLOBAL_IEEE1588_BASE_ADDR,0x0,7,7},
	{IEEE1588_GLOBAL_TX_TEST_EN,GLOBAL_IEEE1588_BASE_ADDR,0x0,8,8},
	{ENABLE_RX_FAULT,GLOBAL_IEEE1588_BASE_ADDR,0x0,9,9},
	{IEEE1588_GLOBAL_REMOTE_FAULT_CONFIG,GLOBAL_IEEE1588_BASE_ADDR,0x0,11,10},
	{IEEE1588_GLOBAL_LOCAL_FAULT_CONFIG,GLOBAL_IEEE1588_BASE_ADDR,0x0,13,12},
	{IEEE1588_GLOBAL_MANUAL_RF_INSERT,GLOBAL_IEEE1588_BASE_ADDR,0x0,14,14},
	{IEEE1588_GLOBAL_MANUAL_LF_INSERT,GLOBAL_IEEE1588_BASE_ADDR,0x0,15,15},
	{IEEE1588_GLOBAL_MANUAL_IDLE_INSERT,GLOBAL_IEEE1588_BASE_ADDR,0x0,16,16},
	{IEEE1588_GLOBAL_DECODE_SEL,GLOBAL_IEEE1588_BASE_ADDR,0x0,17,17},
};
const CHIP_REG PTP_TS_CFG_REG[]=
{
	{RX_PORT_EN,GLOBAL_IEEE1588_BASE_ADDR,0x1,7,0},
	{TX_PORT_EN,GLOBAL_IEEE1588_BASE_ADDR,0x1,15,8},
	{TX2RX_PORT_EN,GLOBAL_IEEE1588_BASE_ADDR,0x1,23,16},
};
const CHIP_REG PTP_GLB_INT1_INT_REG[]=
{
	{PTP_GLB_INT1_INT,GLOBAL_IEEE1588_BASE_ADDR,0x2,25,0},
};
const CHIP_REG PTP_GLB_INT1_INT_MASK_REG[]=
{
	{PTP_GLB_INT1_INT_MASK,GLOBAL_IEEE1588_BASE_ADDR,0x3,25,0},
};
const CHIP_REG PTP_TX_PKTS_EN_REG[]=
{
	{RECALC_CRC_EN,PTP_TX_OH_BASE_ADDR,0x0,0,0},
	{RECALC_CKSUM_EN,PTP_TX_OH_BASE_ADDR,0x0,1,1},
	{TX_LEN_CTRL_EN,PTP_TX_OH_BASE_ADDR,0x0,3,3},
	{SAMPLE_TX_TS_EN,PTP_TX_OH_BASE_ADDR,0x0,4,4},
	{TX2RX_LOOP_EN,PTP_TX_OH_BASE_ADDR,0x0,5,5},
	{CF_FILED_SIGN,PTP_TX_OH_BASE_ADDR,0x0,6,6},
	{TX_ELASTIC_EN,PTP_TX_OH_BASE_ADDR,0x0,7,7},
};
const CHIP_REG TX_PORT_ASYM_COMP_REG[]=
{
	{TX_ASYM_COMP_DATA,PTP_TX_OH_BASE_ADDR,0x1,15,0},
	{TX_ASYM_COMP_SIGN,PTP_TX_OH_BASE_ADDR,0x1,16,16},
	{TX_ASYM_COMP_FRAC,PTP_TX_OH_BASE_ADDR,0x1,20,17},
};
const CHIP_REG PTPTX_CF_CALC_CTRL_REG[]=
{
	{CF_SET_THRESHOLD,PTP_TX_OH_BASE_ADDR,0x6,15,0},
};
const CHIP_REG PORT_TX_FRM_CNT_REG[]=
{
	{PORT_TX_FRM_CNT,PTP_TX_OH_BASE_ADDR,0x7,31,0},
};
const CHIP_REG PORT_TX_PTP_CNT_REG[]=
{
	{PORT_TX_PTP_CNT,PTP_TX_OH_BASE_ADDR,0x8,31,0},
};
const CHIP_REG PORT_TX_SSM_CNT_REG[]=
{
	{PORT_TX_SSM_CNT,PTP_TX_OH_BASE_ADDR,0x9,31,0},
};
const CHIP_REG PORT_TX_DROP_CNT_REG[]=
{
	{PBUFF_DROP_CNT,PTP_TX_OH_BASE_ADDR,0xc,31,0},
};
const CHIP_REG PTPTX_2STEP_TS_CAP_REG[]=
{
	{PORT_2STEP_TIMESTAMP_0,PTP_TX_OH_BASE_ADDR,0xa,31,0},
	{PORT_2STEP_TIMESTAMP_1,PTP_TX_OH_BASE_ADDR,0xb,19,0},
};
const CHIP_REG PTP_TXOH_ALM_REG[]=
{
	{TX_2STEP_CAP_READY,PTP_TX_OH_BASE_ADDR,0xe,0,0},
	{CF_CORRECT_ALM,PTP_TX_OH_BASE_ADDR,0xe,1,1},
	{PTP_TX_OH_ERR,PTP_TX_OH_BASE_ADDR,0xe,2,2},
	{TX_66B_CODE_ALM,PTP_TX_OH_BASE_ADDR,0xe,3,3},
	{PBUFF_DROP_ALM,PTP_TX_OH_BASE_ADDR,0xe,4,4},
	{RD_LENGTH_ALM,PTP_TX_OH_BASE_ADDR,0xe,5,5},
};
const CHIP_REG TX_PORT_BUF_TS_STATE_REG[]=
{
	{PORT_BUFF_FULL,PTP_TX_OH_BASE_ADDR,0xf,0,0},
	{PORT_BUFF_EMPTY,PTP_TX_OH_BASE_ADDR,0xf,1,1},
	{TX_BBUFF_BANK_ST,PTP_TX_OH_BASE_ADDR,0xf,6,2},
};
const CHIP_REG PTP_TXOH_INT_REG[]=
{
	{TX_2STEP_CAP_READY_INT,PTP_TX_OH_BASE_ADDR,0x2,1,0},
	{CF_CORRECT_ALM_INT,PTP_TX_OH_BASE_ADDR,0x2,3,2},
	{PTP_TX_OH_ERR_INT,PTP_TX_OH_BASE_ADDR,0x2,5,4},
	{TX_66B_CODE_ALM_INT,PTP_TX_OH_BASE_ADDR,0x2,7,6},
	{PBUFF_DROP_ALM_INT,PTP_TX_OH_BASE_ADDR,0x2,9,8},
	{RD_LENGTH_ALM_INT,PTP_TX_OH_BASE_ADDR,0x2,11,10},
};
const CHIP_REG PTP_TXOH_INT_MASK_REG[]=
{
	{TX_2STEP_CAP_READY_INT_MASK,PTP_TX_OH_BASE_ADDR,0x3,1,0},
	{CF_CORRECT_ALM_INT_MASK,PTP_TX_OH_BASE_ADDR,0x3,3,2},
	{PTP_TX_OH_ERR_INT_MASK,PTP_TX_OH_BASE_ADDR,0x3,5,4},
	{TX_66B_CODE_ALM_INT_MASK,PTP_TX_OH_BASE_ADDR,0x3,7,6},
	{PBUFF_DROP_ALM_INT_MASK,PTP_TX_OH_BASE_ADDR,0x3,9,8},
	{RD_LENGTH_ALM_INT_MASK,PTP_TX_OH_BASE_ADDR,0x3,11,10},
};
const CHIP_REG PTP_RX_PKTS_EN_REG[]=
{
	{ERROR_DROP_EN,PTP_RX_OH_BASE_ADDR,0x0,1,1},
};
const CHIP_REG RX_PORT_COMP_REG[]=
{
	{RX_COMP_FRAC,PTP_RX_OH_BASE_ADDR,0x1,3,0},
	{RX_COMP_NS,PTP_RX_OH_BASE_ADDR,0x1,19,4},
	{RX_COMP_SIGN,PTP_RX_OH_BASE_ADDR,0x1,20,20},
};

const CHIP_REG RX_FRM_CNT_REG[]=
{
	{RX_FRM_CNT,PTP_RX_OH_BASE_ADDR,0x2,31,0},
};
const CHIP_REG RX_PTP_PORT_BUF_REG[]=
{
	{CTC_PORT_BUFF_FULL,PTP_RX_OH_BASE_ADDR,0x3,0,0},
	{CTC_PORT_BUFF_EMPTY,PTP_RX_OH_BASE_ADDR,0x3,1,1},
	{RX_BBUFF_BANK_ST,PTP_RX_OH_BASE_ADDR,0x3,6,2},
};
const CHIP_REG BANK_FRM_ALL_CNT_REG[]=
{
	{BK_FRM_ALL_CNT,PTP_RX_OH_BASE_ADDR,0x4,31,0},
};
const CHIP_REG BANK_FRM_ERR_CNT_REG[]=
{
	{BK_FRM_ERR_CNT,PTP_RX_OH_BASE_ADDR,0x5,31,0},
};
const CHIP_REG BANK_DROP_FRM_CNT_REG[]=
{
	{BK_DROP_FRM_CNT,PTP_RX_OH_BASE_ADDR,0x6,31,0},
};
const CHIP_REG PTP_RXOH_ALM_REG[]=
{
	{RX_PB_BUFF_FULL_ALM,PTP_RX_OH_BASE_ADDR,0x7,0,0},
	{PTP_RX_OH_ERR,PTP_RX_OH_BASE_ADDR,0x7,1,1},
	{RX_66B_CODE_ALM,PTP_RX_OH_BASE_ADDR,0x7,2,2},
	{RX_CAP_TS_READY,PTP_RX_OH_BASE_ADDR,0x7,3,3},
	{PRD_LENGTH_ALM,PTP_RX_OH_BASE_ADDR,0x7,4,4},
};
const CHIP_REG PTP_RXOH_INT_REG[]=
{
	{RX_PB_BUFF_FULL_ALM_INT,PTP_RX_OH_BASE_ADDR,0x8,1,0},
	{PTP_RX_OH_ERR_INT,PTP_RX_OH_BASE_ADDR,0x8,3,2},
	{RX_66B_CODE_ALM_INT,PTP_RX_OH_BASE_ADDR,0x8,5,4},
	{RX_CAP_TS_READY_INT,PTP_RX_OH_BASE_ADDR,0x8,7,6},
	{PRD_LENGTH_ALM_INT,PTP_RX_OH_BASE_ADDR,0x8,9,8},
};
const CHIP_REG PTP_RXOH_INT_MASK_REG[]=
{
	{RX_PB_BUFF_FULL_ALM_INT_MASK,PTP_RX_OH_BASE_ADDR,0x9,1,0},
	{PTP_RX_OH_ERR_INT_MASK,PTP_RX_OH_BASE_ADDR,0x9,3,2},
	{RX_66B_CODE_ALM_INT_MASK,PTP_RX_OH_BASE_ADDR,0x9,5,4},
	{RX_CAP_TS_READY_INT_MASK,PTP_RX_OH_BASE_ADDR,0x9,7,6},
	{PRD_LENGTH_ALM_INT_MASK,PTP_RX_OH_BASE_ADDR,0x9,9,8},
};
const CHIP_REG PORTRX_CAP_TS_REG[]=
{
	{PORTRX_CAP_TIMESTAMP_0,PTP_RX_OH_BASE_ADDR,0xa,31,0},
	{PORTRX_CAP_TIMESTAMP_1,PTP_RX_OH_BASE_ADDR,0xb,19,0},
};

/******************************************************************************
*
* FUNCTION
*
*     global_ieee1588_global_cfg_set
*
* DESCRIPTION
*
*     IEEE1588 GLOBAL CFG.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id:0~14
**    parameter: field cfg value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS global_ieee1588_global_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (ptp_over_oh_debug)
       {
           printf("[GLOBAL_IEEE1588_GLOBAL_CFG_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n field_id = %d\r\n",   
                                                      chip_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(IEEE1588_GLOBAL_CFG_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[GLOBAL_IEEE1588_GLOBAL_CFG_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_write(chip_id,IEEE1588_GLOBAL_CFG_REG[field_id],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     global_ieee1588_global_cfg_get
*
* DESCRIPTION
*
*     IEEE1588 GLOBAL CFG.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id:0~14
**    parameter*: pointer to field value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS global_ieee1588_global_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (ptp_over_oh_debug)
       {
           printf("[GLOBAL_IEEE1588_GLOBAL_CFG_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[GLOBAL_IEEE1588_GLOBAL_CFG_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(IEEE1588_GLOBAL_CFG_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[GLOBAL_IEEE1588_GLOBAL_CFG_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,IEEE1588_GLOBAL_CFG_REG[field_id],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     global_ieee1588_ptp_ts_cfg_set
*
* DESCRIPTION
*
*     IEEE1588 PTP TS CFG.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id: 0~1 0:RX_PORT_EN 1:TX_PORT_EN 2:TX2RX_PORT_EN
**    parameter: field cfg value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS global_ieee1588_ptp_ts_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (field_id > TX2RX_PORT_EN) ||
        (parameter > 0xff))
    {
       if (ptp_over_oh_debug)
       {
           printf("[GLOBAL_IEEE1588_PTP_TS_CFG_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n field_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      field_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_TS_CFG_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[GLOBAL_IEEE1588_PTP_TS_CFG_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_write(chip_id,PTP_TS_CFG_REG[field_id],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     global_ieee1588_ptp_ts_cfg_get
*
* DESCRIPTION
*
*     IEEE1588 PTP TS CFG.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id: 0~1 0:RX_PORT_EN 1:TX_PORT_EN 2:TX2RX_PORT_EN
**    parameter*: pointer to field value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS global_ieee1588_ptp_ts_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (field_id > TX2RX_PORT_EN))
    {
       if (ptp_over_oh_debug)
       {
           printf("[GLOBAL_IEEE1588_PTP_TS_CFG_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[GLOBAL_IEEE1588_PTP_TS_CFG_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_TS_CFG_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[GLOBAL_IEEE1588_PTP_TS_CFG_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,PTP_TS_CFG_REG[field_id],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     global_ieee1588_ptp_glb_int1_int_set
*
* DESCRIPTION
*
*     IEEE1588 PTP GLB INT1 INT .
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id: 0~25
**    parameter: field cfg value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS global_ieee1588_ptp_glb_int1_int_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (field_id > 25) ||
        (parameter > 0x1))
    {
       if (ptp_over_oh_debug)
       {
           printf("[GLOBAL_IEEE1588_PTP_GLB_INT1_INT_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n field_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      field_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,PTP_GLB_INT1_INT_REG[PTP_GLB_INT1_INT].base_addr,PTP_GLB_INT1_INT_REG[PTP_GLB_INT1_INT].offset_addr,
		field_id,field_id,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     global_ieee1588_ptp_glb_int1_int_get
*
* DESCRIPTION
*
*     IEEE1588 PTP  GLB INT1 INT .
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id: 0~25
**    parameter*: pointer to field value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS global_ieee1588_ptp_glb_int1_int_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (field_id > 25))
    {
       if (ptp_over_oh_debug)
       {
           printf("[GLOBAL_IEEE1588_PTP_GLB_INT1_INT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[GLOBAL_IEEE1588_PTP_GLB_INT1_INT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_GLB_INT1_INT_REG[PTP_GLB_INT1_INT].base_addr,PTP_GLB_INT1_INT_REG[PTP_GLB_INT1_INT].offset_addr,
		field_id,field_id,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     global_ieee1588_ptp_glb_int1_int_mask_set
*
* DESCRIPTION
*
*     IEEE1588 PTP GLB INT1 INT MASK.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id: 0~25
**    parameter: field cfg value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS global_ieee1588_ptp_glb_int1_int_mask_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (field_id > 25) ||
        (parameter > 0x1))
    {
       if (ptp_over_oh_debug)
       {
           printf("[GLOBAL_IEEE1588_PTP_GLB_INT1_INT_MASK_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n field_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      field_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,PTP_GLB_INT1_INT_MASK_REG[PTP_GLB_INT1_INT_MASK].base_addr,PTP_GLB_INT1_INT_MASK_REG[PTP_GLB_INT1_INT_MASK].offset_addr,
		field_id,field_id,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     global_ieee1588_ptp_glb_int1_int_mask_get
*
* DESCRIPTION
*
*     IEEE1588 PTP TS CFG.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id: 0~25
**    parameter*: pointer to field value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS global_ieee1588_ptp_glb_int1_int_mask_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (field_id > 25))
    {
       if (ptp_over_oh_debug)
       {
           printf("[GLOBAL_IEEE1588_PTP_GLB_INT1_INT_MASK_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[GLOBAL_IEEE1588_PTP_GLB_INT1_INT_MASK_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_GLB_INT1_INT_MASK_REG[PTP_GLB_INT1_INT_MASK].base_addr,PTP_GLB_INT1_INT_MASK_REG[PTP_GLB_INT1_INT_MASK].offset_addr,
		field_id,field_id,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_pkts_en_set
*
* DESCRIPTION
*
*     Ptp tx pkts enable cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
*     field_id: 0~4 0:RECALC_CRC_EN 1:RECALC_CKSUM_EN
*				    2:TX_LEN_CTRL_EN 3:SAMPLE_TX_TS_EN 4:TX2RX_LOOP_EN
*					5:CF_FILED_SIGN 6:TX_ELASTIC_EN
**    parameter: 1 is enabel, 0 is disable
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_pkts_en_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7) ||
        (field_id > TX_ELASTIC_EN) ||
        (parameter > 0x1))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PKTS_EN_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n field_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_TX_PKTS_EN_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PKTS_EN_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,PTP_TX_PKTS_EN_REG[field_id].base_addr+port_id*PTP_TX_OH_OFFSET,PTP_TX_PKTS_EN_REG[field_id].offset_addr,
        PTP_TX_PKTS_EN_REG[field_id].end_bit,PTP_TX_PKTS_EN_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_pkts_en_get
*
* DESCRIPTION
*
*     Ptp tx pkts enable cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
*     field_id: 0~4 0:RECALC_CRC_EN 1:RECALC_CKSUM_EN
*				    2:TX_LEN_CTRL_EN 3:SAMPLE_TX_TS_EN 4:TX2RX_LOOP_EN
*					5:CF_FILED_SIGN 6:TX_ELASTIC_EN
**    parameter*: pointer to enable status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_pkts_en_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7) ||
        (field_id > TX_ELASTIC_EN))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PKTS_EN_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n field_id = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PKTS_EN_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_TX_PKTS_EN_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PKTS_EN_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_TX_PKTS_EN_REG[field_id].base_addr+port_id*PTP_TX_OH_OFFSET,PTP_TX_PKTS_EN_REG[field_id].offset_addr,
        PTP_TX_PKTS_EN_REG[field_id].end_bit,PTP_TX_PKTS_EN_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_TX_ASYM_COMP_DATA_set
*
* DESCRIPTION
*
*     Ptp tx TX_ASYM_COMP_DATA cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter: 
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_asym_comp_data_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7) ||
        (parameter > 0xffff))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_ASYM_COMP_DATA_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_DATA].base_addr+port_id*PTP_TX_OH_OFFSET,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_DATA].offset_addr,
        TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_DATA].end_bit,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_DATA].start_bit,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_TX_ASYM_COMP_DATA_get
*
* DESCRIPTION
*
*     Ptp tx TX_ASYM_COMP_DATA cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter*: pointer to enable status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_asym_comp_data_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_ASYM_COMP_DATA_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d\r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_ASYM_COMP_DATA_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_DATA].base_addr+port_id*PTP_TX_OH_OFFSET,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_DATA].offset_addr,
        TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_DATA].end_bit,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_DATA].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_TX_ASYM_COMP_SIGN_set
*
* DESCRIPTION
*
*     Ptp tx asym_comp_sign cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter: 
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_asym_comp_sign_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7) ||
        (parameter > 0x1))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_ASYM_COMP_SIGN_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (TX_ASYM_COMP_SIGN > MAX_INDEX(TX_PORT_ASYM_COMP_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_ASYM_COMP_SIGN_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_SIGN].base_addr+port_id*PTP_TX_OH_OFFSET,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_SIGN].offset_addr,
        TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_SIGN].end_bit,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_SIGN].start_bit,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_TX_ASYM_COMP_SIGN_get
*
* DESCRIPTION
*
*     Ptp tx TX_ASYM_COMP_SIGN cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter*: pointer to enable status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_asym_comp_sign_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_ASYM_COMP_SIGN_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d\r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_ASYM_COMP_SIGN_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (TX_ASYM_COMP_SIGN > MAX_INDEX(TX_PORT_ASYM_COMP_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_ASYM_COMP_SIGN_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_SIGN].base_addr+port_id*PTP_TX_OH_OFFSET,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_SIGN].offset_addr,
        TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_SIGN].end_bit,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_SIGN].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_asym_comp_frac_set
*
* DESCRIPTION
*
*     Ptp Tx_asym_comp_frac cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter: 
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_asym_comp_frac_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7) ||
        (parameter > 0xf))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_ASYM_COMP_FRAC_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (TX_ASYM_COMP_FRAC > MAX_INDEX(TX_PORT_ASYM_COMP_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_ASYM_COMP_FRAC_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_FRAC].base_addr+port_id*PTP_TX_OH_OFFSET,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_FRAC].offset_addr,
        TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_FRAC].end_bit,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_FRAC].start_bit,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_TX_ASYM_COMP_frac_get
*
* DESCRIPTION
*
*     Ptp tx TX_ASYM_COMP_frac cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter*: pointer to enable status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_asym_comp_frac_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_ASYM_COMP_FRAC_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d\r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_ASYM_COMP_FRAC_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (TX_ASYM_COMP_FRAC > MAX_INDEX(TX_PORT_ASYM_COMP_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_ASYM_COMP_FRAC_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_FRAC].base_addr+port_id*PTP_TX_OH_OFFSET,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_FRAC].offset_addr,
        TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_FRAC].end_bit,TX_PORT_ASYM_COMP_REG[TX_ASYM_COMP_FRAC].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_CF_SET_THRESHOLD_set
*
* DESCRIPTION
*
*     Ptp tx CF_SET_THRESHOLD cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter: 
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_cf_set_threshold_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7) ||
        (parameter > 0xffff))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_CF_SET_THRESHOLD_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,PTPTX_CF_CALC_CTRL_REG[CF_SET_THRESHOLD].base_addr+port_id*PTP_TX_OH_OFFSET,PTPTX_CF_CALC_CTRL_REG[CF_SET_THRESHOLD].offset_addr,
        PTPTX_CF_CALC_CTRL_REG[CF_SET_THRESHOLD].end_bit,PTPTX_CF_CALC_CTRL_REG[CF_SET_THRESHOLD].start_bit,parameter);

    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_CF_SET_THRESHOLD_get
*
* DESCRIPTION
*
*     Ptp tx CF_SET_THRESHOLD cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter*: pointer to enable status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_cf_set_threshold_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_CF_SET_THRESHOLD_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d\r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_CF_SET_THRESHOLD_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTPTX_CF_CALC_CTRL_REG[CF_SET_THRESHOLD].base_addr+port_id*PTP_TX_OH_OFFSET,PTPTX_CF_CALC_CTRL_REG[CF_SET_THRESHOLD].offset_addr,
        PTPTX_CF_CALC_CTRL_REG[CF_SET_THRESHOLD].end_bit,PTPTX_CF_CALC_CTRL_REG[CF_SET_THRESHOLD].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_port_tx_frm_cnt_get
*
* DESCRIPTION
*
*     Ptp tx port_tx_frm_cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter*: pointer to alm
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_port_tx_frm_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PORT_TX_FRM_CNT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d,  \
                                                      \r\n port_id = %d\r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PORT_TX_FRM_CNT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PORT_TX_FRM_CNT_REG[PORT_TX_FRM_CNT].base_addr+port_id*PTP_TX_OH_OFFSET,PORT_TX_FRM_CNT_REG[PORT_TX_FRM_CNT].offset_addr,
        PORT_TX_FRM_CNT_REG[PORT_TX_FRM_CNT].end_bit,PORT_TX_FRM_CNT_REG[PORT_TX_FRM_CNT].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_port_tx_ptp_cnt_get
*
* DESCRIPTION
*
*     Ptp tx port_tx_ptp_cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter*: pointer to alm
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_port_tx_ptp_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PORT_TX_PTP_CNT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d,  \
                                                      \r\n port_id = %d\r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PORT_TX_PTP_CNT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PORT_TX_PTP_CNT_REG[PORT_TX_PTP_CNT].base_addr+port_id*PTP_TX_OH_OFFSET,PORT_TX_PTP_CNT_REG[PORT_TX_PTP_CNT].offset_addr,
        PORT_TX_PTP_CNT_REG[PORT_TX_PTP_CNT].end_bit,PORT_TX_PTP_CNT_REG[PORT_TX_PTP_CNT].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_port_tx_ssm_cnt_get
*
* DESCRIPTION
*
*     Ptp tx port_tx_ssm_cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter*: pointer to alm
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_port_tx_ssm_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PORT_TX_SSM_CNT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d,  \
                                                      \r\n port_id = %d\r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PORT_TX_SSM_CNT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PORT_TX_SSM_CNT_REG[PORT_TX_SSM_CNT].base_addr+port_id*PTP_TX_OH_OFFSET,PORT_TX_SSM_CNT_REG[PORT_TX_SSM_CNT].offset_addr,
        PORT_TX_SSM_CNT_REG[PORT_TX_SSM_CNT].end_bit,PORT_TX_SSM_CNT_REG[PORT_TX_SSM_CNT].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_port_tx_drop_cnt_get
*
* DESCRIPTION
*
*     Ptp tx port_tx_drop_cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter*: 
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_port_tx_drop_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PORT_TX_DROP_CNT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d,  \
                                                      \r\n port_id = %d\r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PORT_TX_DROP_CNT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    rv = regp_bit_read(chip_id,PORT_TX_DROP_CNT_REG[PBUFF_DROP_CNT].base_addr+port_id*PTP_TX_OH_OFFSET,PORT_TX_DROP_CNT_REG[PBUFF_DROP_CNT].offset_addr,
        PORT_TX_DROP_CNT_REG[PBUFF_DROP_CNT].end_bit,PORT_TX_DROP_CNT_REG[PBUFF_DROP_CNT].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_port_2step_timestamp_get
*
* DESCRIPTION
*
*     Ptp tx port_2step_timestamp.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
*     field_id:0~1:port_2step_timestamp0~1
**    parameter*: pointer to alm
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_port_2step_timestamp_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7) ||
        (field_id > 1))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PORT_2STEP_TIMESTAMP_GET] invalid parameter!! \
                                                      \r\n chip_id = %d,  \
                                                      \r\n port_id = %d, \
                                                      \r\n field_id = %d\r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PORT_2STEP_TIMESTAMP_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTPTX_2STEP_TS_CAP_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PORT_2STEP_TIMESTAMP_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTPTX_2STEP_TS_CAP_REG[field_id].base_addr+port_id*PTP_TX_OH_OFFSET,PTPTX_2STEP_TS_CAP_REG[field_id].offset_addr,
        PTPTX_2STEP_TS_CAP_REG[field_id].end_bit,PTPTX_2STEP_TS_CAP_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_oh_alm_get
*
* DESCRIPTION
*
*     Ptp tx ptp_tx_oh_alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
*     field_id: 0~4 0:TX_2STEP_CAP_READY 1:CF_CORRECT_ALM 2:PTP_TX_OH_ERR
*					3:TX_66B_CODE_ALM  4:PBUFF_DROP_ALM 5:RD_LENGTH_ALM
**    parameter*: pointer to buf status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_oh_alm_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_OH_ALM_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n field_id = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_OH_ALM_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_TXOH_ALM_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_OH_ALM_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_TXOH_ALM_REG[field_id].base_addr+port_id*PTP_TX_OH_OFFSET,PTP_TXOH_ALM_REG[field_id].offset_addr,
        PTP_TXOH_ALM_REG[field_id].end_bit,PTP_TXOH_ALM_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_oh_alm_all_get
*
* DESCRIPTION
*
*     Ptp tx ptp_tx_oh_alm all reg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
**    parameter*: pointer to buf status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_oh_alm_all_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_OH_ALM_ALL_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d \r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_OH_ALM_ALL_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_read(chip_id,PTP_TXOH_ALM_REG[TX_2STEP_CAP_READY].base_addr+port_id*PTP_TX_OH_OFFSET,PTP_TXOH_ALM_REG[TX_2STEP_CAP_READY].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_ptp_port_buf_get
*
* DESCRIPTION
*
*     Ptp tx pkts port buf status.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~7
*     field_id: 0~2 0:PORT_BUFF_FULL 1:PORT_BUFF_EMPTY  2:TX_BBUFF_BANK_ST
**    parameter*: pointer to buf status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_ptp_port_buf_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 7) ||
        (field_id > TX_BBUFF_BANK_ST))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PTP_PORT_BUF_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n field_id = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PTP_PORT_BUF_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(TX_PORT_BUF_TS_STATE_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[TX_PORT_BUF_TS_STATE_REG] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,TX_PORT_BUF_TS_STATE_REG[field_id].base_addr+port_id*PTP_TX_OH_OFFSET,TX_PORT_BUF_TS_STATE_REG[field_id].offset_addr,
        TX_PORT_BUF_TS_STATE_REG[field_id].end_bit,TX_PORT_BUF_TS_STATE_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_ptp_txoh_int_set
*
* DESCRIPTION
*
*     IEEE1588 PTP txoh INT .
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  port_id:0~7
*     field_id: 0~5 0:TX_2STEP_CAP_READY_INT 1:CF_CORRECT_ALM_INT 2:PTP_TX_OH_ERR_INT 3:TX_66B_CODE_ALM_INT 
*                   4:PBUFF_DROP_ALM_INT 5:RD_LENGTH_ALM_INT
**    parameter: field cfg value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_ptp_txoh_int_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
		(port_id > 7) ||
        (parameter > 0x3))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PTP_TXOH_INT_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n field_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_TXOH_INT_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PTP_TXOH_INT_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,PTP_TXOH_INT_REG[field_id].base_addr+port_id*PTP_TX_OH_OFFSET,PTP_TXOH_INT_REG[field_id].offset_addr,
		PTP_TXOH_INT_REG[field_id].end_bit,PTP_TXOH_INT_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_ptp_txoh_int_get
*
* DESCRIPTION
*
*     IEEE1588 PTP txoh INT  .
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  port_id:0~7
*     field_id: 0~3 0:TX_2STEP_CAP_READY_INT 1:CF_CORRECT_ALM_INT 2:PTP_TX_OH_ERR_INT 3:TX_66B_CODE_ALM_INT 
*                   4:PBUFF_DROP_ALM_INT 5:RD_LENGTH_ALM_INT
**    parameter*: pointer to field value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_ptp_txoh_int_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
		(port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PTP_TXOH_INT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PTP_TXOH_INT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_TXOH_INT_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PTP_TXOH_INT_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_TXOH_INT_REG[field_id].base_addr+port_id*PTP_TX_OH_OFFSET,PTP_TXOH_INT_REG[field_id].offset_addr,
		PTP_TXOH_INT_REG[field_id].end_bit,PTP_TXOH_INT_REG[field_id].start_bit,parameter);;
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_ptp_txoh_int_mask_set
*
* DESCRIPTION
*
*     IEEE1588 PTP txoh INT  MASK.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  port_id:0~7
*     field_id: 0~5 0:TX_2STEP_CAP_READY_INT_MASK 1:CF_CORRECT_ALM_INT_MASK 2:PTP_TX_OH_ERR_INT_MASK 3:TX_66B_CODE_ALM_INT_MASK 
*					4:PBUFF_DROP_ALM_INT_MASK   5:RD_LENGTH_ALM_INT_MASK
**    parameter: field cfg value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_ptp_txoh_int_mask_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
		(port_id > 7) ||
        (parameter > 0x3))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PTP_TXOH_INT_MASK_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n field_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_TXOH_INT_MASK_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PTP_TXOH_INT_MASK_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,PTP_TXOH_INT_MASK_REG[field_id].base_addr+port_id*PTP_TX_OH_OFFSET,PTP_TXOH_INT_MASK_REG[field_id].offset_addr,
		PTP_TXOH_INT_MASK_REG[field_id].end_bit,PTP_TXOH_INT_MASK_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_tx_ptp_txoh_int_mask_get
*
* DESCRIPTION
*
*     IEEE1588 PTP txoh INT  CFG.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  port_id:0~7
*     field_id: 0~5 0:TX_2STEP_CAP_READY_INT_MASK 1:CF_CORRECT_ALM_INT_MASK 2:PTP_TX_OH_ERR_INT_MASK 3:TX_66B_CODE_ALM_INT_MASK 
*					4:PBUFF_DROP_ALM_INT_MASK   5:RD_LENGTH_ALM_INT_MASK
**    parameter*: pointer to field value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_tx_ptp_txoh_int_mask_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
		(port_id > 7))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_TX_PTP_TXOH_INT_MASK_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PTP_TXOH_INT_MASK_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_TXOH_INT_MASK_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_TX_PTP_TXOH_INT_MASK_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_TXOH_INT_MASK_REG[field_id].base_addr+port_id*PTP_TX_OH_OFFSET,PTP_TXOH_INT_MASK_REG[field_id].offset_addr,
		PTP_TXOH_INT_MASK_REG[field_id].end_bit,PTP_TXOH_INT_MASK_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_pkts_en_set
*
* DESCRIPTION
*
*     Ptp rx pkts enable cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter: 1 is enabel, 0 is disable
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_pkts_en_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15) ||
        (parameter > 0x1))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PKTS_EN_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,PTP_RX_PKTS_EN_REG[ERROR_DROP_EN].base_addr+port_id*PTP_RX_OH_OFFSET,PTP_RX_PKTS_EN_REG[ERROR_DROP_EN].offset_addr,
        PTP_RX_PKTS_EN_REG[ERROR_DROP_EN].end_bit,PTP_RX_PKTS_EN_REG[ERROR_DROP_EN].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_pkts_en_get
*
* DESCRIPTION
*
*     Ptp rx pkts enable cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter*: pointer to field enable status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_pkts_en_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15) )
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PKTS_EN_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d \r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PKTS_EN_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_RX_PKTS_EN_REG[ERROR_DROP_EN].base_addr+port_id*PTP_RX_OH_OFFSET,PTP_RX_PKTS_EN_REG[ERROR_DROP_EN].offset_addr,
        PTP_RX_PKTS_EN_REG[ERROR_DROP_EN].end_bit,PTP_RX_PKTS_EN_REG[ERROR_DROP_EN].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_comp_frac_set
*
* DESCRIPTION
*
*     Ptp rx_comp_frac cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter:0~0xf PTPfraction ns1/16ns
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_comp_frac_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15) ||
        (parameter > 0xf))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_COMP_FRAC_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,RX_PORT_COMP_REG[RX_COMP_FRAC].base_addr+port_id*PTP_RX_OH_OFFSET,RX_PORT_COMP_REG[RX_COMP_FRAC].offset_addr,
        RX_PORT_COMP_REG[RX_COMP_FRAC].end_bit,RX_PORT_COMP_REG[RX_COMP_FRAC].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_comp_frac_get
*
* DESCRIPTION
*
*     Ptp rx_comp_frac cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter*: pointer to field 
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_comp_frac_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15) )
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_COMP_FRAC_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d \r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_COMP_FRAC_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,RX_PORT_COMP_REG[RX_COMP_FRAC].base_addr+port_id*PTP_RX_OH_OFFSET,RX_PORT_COMP_REG[RX_COMP_FRAC].offset_addr,
        RX_PORT_COMP_REG[RX_COMP_FRAC].end_bit,RX_PORT_COMP_REG[RX_COMP_FRAC].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_comp_ns_set
*
* DESCRIPTION
*
*     Ptp rx_comp_ns cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter:0~0xffff PTPns
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_comp_ns_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15) ||
        (parameter > 0xffff))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_COMP_NS_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (RX_COMP_NS > MAX_INDEX(RX_PORT_COMP_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_COMP_NS_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,RX_PORT_COMP_REG[RX_COMP_NS].base_addr+port_id*PTP_RX_OH_OFFSET,RX_PORT_COMP_REG[RX_COMP_NS].offset_addr,
        RX_PORT_COMP_REG[RX_COMP_NS].end_bit,RX_PORT_COMP_REG[RX_COMP_NS].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_comp_ns_get
*
* DESCRIPTION
*
*     Ptp rx_comp_ns cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter*: pointer to field 
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_comp_ns_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15) )
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_COMP_NS_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d \r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_COMP_NS_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (RX_COMP_NS > MAX_INDEX(RX_PORT_COMP_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_COMP_NS_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,RX_PORT_COMP_REG[RX_COMP_NS].base_addr+port_id*PTP_RX_OH_OFFSET,RX_PORT_COMP_REG[RX_COMP_NS].offset_addr,
        RX_PORT_COMP_REG[RX_COMP_NS].end_bit,RX_PORT_COMP_REG[RX_COMP_NS].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_comp_sign_set
*
* DESCRIPTION
*
*     Ptp rx_comp_sign cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter:0~0xffff PTPsign
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_comp_sign_set(UINT_8 chip_id,UINT_8 port_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15) ||
        (parameter > 0x1))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_COMP_SIGN_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (RX_COMP_SIGN > MAX_INDEX(RX_PORT_COMP_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_COMP_SIGN_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,RX_PORT_COMP_REG[RX_COMP_SIGN].base_addr+port_id*PTP_RX_OH_OFFSET,RX_PORT_COMP_REG[RX_COMP_SIGN].offset_addr,
        RX_PORT_COMP_REG[RX_COMP_SIGN].end_bit,RX_PORT_COMP_REG[RX_COMP_SIGN].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_comp_sign_get
*
* DESCRIPTION
*
*     Ptp rx_comp_sign cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter*: pointer to field 
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_comp_sign_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15) )
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_COMP_SIGN_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d \r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_COMP_SIGN_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (RX_COMP_SIGN > MAX_INDEX(RX_PORT_COMP_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_COMP_SIGN_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,RX_PORT_COMP_REG[RX_COMP_SIGN].base_addr+port_id*PTP_RX_OH_OFFSET,RX_PORT_COMP_REG[RX_COMP_SIGN].offset_addr,
        RX_PORT_COMP_REG[RX_COMP_SIGN].end_bit,RX_PORT_COMP_REG[RX_COMP_SIGN].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_pkts_rx_frm_cnt_get
*
* DESCRIPTION
*
*     ptp rx pkts rx frm cnt cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter*: pointer to cnt value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_pkts_rx_frm_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PKTS_RX_FRM_CNT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d,  \
                                                      \r\n port_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PKTS_RX_FRM_CNT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,RX_FRM_CNT_REG[RX_FRM_CNT].base_addr+port_id*PTP_RX_OH_OFFSET,RX_FRM_CNT_REG[RX_FRM_CNT].offset_addr,
        RX_FRM_CNT_REG[RX_FRM_CNT].end_bit,RX_FRM_CNT_REG[RX_FRM_CNT].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_pkts_bank_frm_all_cnt_get
*
* DESCRIPTION
*
*     ptp rx pkts bank frm all cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter*: pointer to cnt value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_pkts_bank_frm_all_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PKTS_BANK_FRM_ALL_CNT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d,  \
                                                      \r\n port_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PKTS_BANK_FRM_ALL_CNT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,BANK_FRM_ALL_CNT_REG[BK_FRM_ALL_CNT].base_addr+port_id*PTP_RX_OH_OFFSET,BANK_FRM_ALL_CNT_REG[BK_FRM_ALL_CNT].offset_addr,
        BANK_FRM_ALL_CNT_REG[BK_FRM_ALL_CNT].end_bit,BANK_FRM_ALL_CNT_REG[BK_FRM_ALL_CNT].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_pkts_bank_frm_err_cnt_get
*
* DESCRIPTION
*
*     ptp rx pkts bank frm err cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter*: pointer to cnt value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_pkts_bank_frm_err_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PKTS_BANK_FRM_ERR_CNT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d,  \
                                                      \r\n port_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PKTS_BANK_FRM_ERR_CNT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,BANK_FRM_ERR_CNT_REG[BK_FRM_ERR_CNT].base_addr+port_id*PTP_RX_OH_OFFSET,BANK_FRM_ERR_CNT_REG[BK_FRM_ERR_CNT].offset_addr,
        BANK_FRM_ERR_CNT_REG[BK_FRM_ERR_CNT].end_bit,BANK_FRM_ERR_CNT_REG[BK_FRM_ERR_CNT].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_pkts_bank_drop_frm_cnt_get
*
* DESCRIPTION
*
*     ptp rx pkts bank drop frm cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter*: pointer to cnt value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_pkts_bank_drop_frm_cnt_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PKTS_BANK_DROP_FRM_CNT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d,  \
                                                      \r\n port_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PKTS_BANK_DROP_FRM_CNT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,BANK_DROP_FRM_CNT_REG[BK_DROP_FRM_CNT].base_addr+port_id*PTP_RX_OH_OFFSET,BANK_DROP_FRM_CNT_REG[BK_DROP_FRM_CNT].offset_addr,
        BANK_DROP_FRM_CNT_REG[BK_DROP_FRM_CNT].end_bit,BANK_DROP_FRM_CNT_REG[BK_DROP_FRM_CNT].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_pkts_rx_ptp_port_buf_get
*
* DESCRIPTION
*
*     Ptp rx pkts rx ptp port buf status.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
*     field_id: 0~1 0:CTC_PORT_BUFF_FULL 1:CTC_PORT_BUFF_EMPTY 2:RX_BBUFF_BANK_ST
**    parameter*: pointer to field buf status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_pkts_rx_ptp_port_buf_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PKTS_RX_PTP_PORT_BUF_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PKTS_RX_PTP_PORT_BUF_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(RX_PTP_PORT_BUF_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PKTS_RX_PTP_PORT_BUF_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,RX_PTP_PORT_BUF_REG[field_id].base_addr+port_id*PTP_RX_OH_OFFSET,RX_PTP_PORT_BUF_REG[field_id].offset_addr,
        RX_PTP_PORT_BUF_REG[field_id].end_bit,RX_PTP_PORT_BUF_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_portrx_cap_ts_get
*
* DESCRIPTION
*
*     Ptp rx portrx_cap_ts status.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
*     field_id: 0~1 0:bit0-31 1:bit32-51
**    parameter*: pointer to field buf status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_portrx_cap_ts_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PORTRX_CAP_TS_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PORTRX_CAP_TS_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PORTRX_CAP_TS_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PORTRX_CAP_TS_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PORTRX_CAP_TS_REG[field_id].base_addr+port_id*PTP_RX_OH_OFFSET,PORTRX_CAP_TS_REG[field_id].offset_addr,
        PORTRX_CAP_TS_REG[field_id].end_bit,PORTRX_CAP_TS_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_ptp_rxoh_alm_get
*
* DESCRIPTION
*
*     Ptp rx ptp_rxoh_alm_get status.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
*     field_id: 0~4 0:RX_PB_BUFF_FULL_ALM 1:PTP_RX_OH_ERR 2:RX_66B_CODE_ALM 3:RX_CAP_TS_READY 4:PRD_LENGTH_ALM
**    parameter*: pointer to field buf status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_ptp_rxoh_alm_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PTP_RXOH_ALM_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d,  \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PTP_RXOH_ALM_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_RXOH_ALM_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PTP_RXOH_ALM_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_RXOH_ALM_REG[field_id].base_addr+port_id*PTP_RX_OH_OFFSET,PTP_RXOH_ALM_REG[field_id].offset_addr,
        PTP_RXOH_ALM_REG[field_id].end_bit,PTP_RXOH_ALM_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_ptp_rxoh_alm_all_get
*
* DESCRIPTION
*
*     Ptp rx ptp_rxoh_alm all status.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     port_id: 0~15
**    parameter*: pointer to field buf status
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_ptp_rxoh_alm_all_get(UINT_8 chip_id,UINT_8 port_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PTP_RXOH_ALM_ALL_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d\r\n",   
                                                      chip_id,
                                                      port_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PTP_RXOH_ALM_ALL_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    rv = regp_read(chip_id,PTP_RXOH_ALM_REG[RX_PB_BUFF_FULL_ALM].base_addr+port_id*PTP_RX_OH_OFFSET,PTP_RXOH_ALM_REG[RX_PB_BUFF_FULL_ALM].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_ptp_rxoh_int_set
*
* DESCRIPTION
*
*     IEEE1588 PTP rxoh INT .
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  port_id:0~15
*     field_id: 0~2 0:RX_PB_BUFF_FULL_ALM_INT 1:PTP_RX_OH_ERR_INT 2:RX_66B_CODE_ALM_INT
*					3:RX_CAP_TS_READY_INT 4:PRD_LENGTH_ALM_INT
**    parameter: field cfg value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_ptp_rxoh_int_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
		(port_id > 15) ||
        (parameter > 0x3))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PTP_RXOH_INT_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n field_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_RXOH_INT_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PTP_RXOH_INT_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,PTP_RXOH_INT_REG[field_id].base_addr+port_id*PTP_RX_OH_OFFSET,PTP_RXOH_INT_REG[field_id].offset_addr,
		PTP_RXOH_INT_REG[field_id].end_bit,PTP_RXOH_INT_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_ptp_rxoh_int_get
*
* DESCRIPTION
*
*     IEEE1588 PTP rxoh INT  .
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  port_id:0~15
*     field_id: 0~2 0:RX_PB_BUFF_FULL_ALM_INT 1:PTP_RX_OH_ERR_INT 2:RX_66B_CODE_ALM_INT 
*					3:RX_CAP_TS_READY_INT 4:PRD_LENGTH_ALM_INT
**    parameter*: pointer to field value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_ptp_rxoh_int_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
		(port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PTP_RXOH_INT_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PTP_RXOH_INT_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_RXOH_INT_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PTP_RXOH_INT_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_RXOH_INT_REG[field_id].base_addr+port_id*PTP_RX_OH_OFFSET,PTP_RXOH_INT_REG[field_id].offset_addr,
		PTP_RXOH_INT_REG[field_id].end_bit,PTP_RXOH_INT_REG[field_id].start_bit,parameter);;
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_ptp_rxoh_int_mask_set
*
* DESCRIPTION
*
*     IEEE1588 PTP rxoh INT  MASK.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  port_id:0~15
*     field_id: 0~2 0:RX_PB_BUFF_FULL_ALM_INT_MASK 1:PTP_RX_OH_ERR_INT_MASK 2:RX_66B_CODE_ALM_INT_MASK
*					3:RX_CAP_TS_READY_INT_MASK 4:PRD_LENGTH_ALM_INT_MASK
**    parameter: field cfg value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_ptp_rxoh_int_mask_set(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
		(port_id > 15) ||
        (parameter > 0x3))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PTP_RXOH_INT_MASK_SET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n field_id = %d, \
                                                      \r\n parameter = %d \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id,
                                                      parameter);
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_RXOH_INT_MASK_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PTP_RXOH_INT_MASK_SET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,PTP_RXOH_INT_MASK_REG[field_id].base_addr+port_id*PTP_RX_OH_OFFSET,PTP_RXOH_INT_MASK_REG[field_id].offset_addr,
		PTP_RXOH_INT_MASK_REG[field_id].end_bit,PTP_RXOH_INT_MASK_REG[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     ptp_rx_ptp_rxoh_int_mask_get
*
* DESCRIPTION
*
*     IEEE1588 PTP rxoh INT  CFG.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  port_id:0~15
*     field_id: 0~2 0:RX_PB_BUFF_FULL_ALM_INT_MASK 1:PTP_RX_OH_ERR_INT_MASK 2:RX_66B_CODE_ALM_INT_MASK
*					3:RX_CAP_TS_READY_INT_MASK 4:PRD_LENGTH_ALM_INT_MASK
**    parameter*: pointer to field value
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
*    taos       2017-10-24  1.0           initial
*
******************************************************************************/
RET_STATUS ptp_rx_ptp_rxoh_int_mask_get(UINT_8 chip_id,UINT_8 port_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
		(port_id > 15))
    {
       if (ptp_over_oh_debug)
       {
           printf("[PTP_RX_PTP_RXOH_INT_MASK_GET] invalid parameter!! \
                                                      \r\n chip_id = %d, \
                                                      \r\n port_id = %d, \
                                                      \r\n field_id = %d  \r\n",   
                                                      chip_id,
                                                      port_id,
                                                      field_id);
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PTP_RXOH_INT_MASK_GET] invalid parameter pointer\r\n");
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(PTP_RXOH_INT_MASK_REG))
    {
        if (ptp_over_oh_debug)
        {
            printf("[PTP_RX_PTP_RXOH_INT_MASK_GET] out of array range\r\n");
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,PTP_RXOH_INT_MASK_REG[field_id].base_addr+port_id*PTP_RX_OH_OFFSET,PTP_RXOH_INT_MASK_REG[field_id].offset_addr,
		PTP_RXOH_INT_MASK_REG[field_id].end_bit,PTP_RXOH_INT_MASK_REG[field_id].start_bit,parameter);
    
    return rv;
}


