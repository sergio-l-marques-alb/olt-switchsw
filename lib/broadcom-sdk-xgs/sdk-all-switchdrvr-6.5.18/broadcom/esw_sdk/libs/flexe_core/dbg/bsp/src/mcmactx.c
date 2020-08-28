
/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int mcmactx_para_debug = 0;


const CHIP_REG mactx_work_mode_reg[]=
{
    {MAC_SAR_MODE_0,MCMACTX_BASE_ADDR,0x0,31,0},
    {MAC_SAR_MODE_1,MCMACTX_BASE_ADDR,0x1,31,0},
    {MAC_SAR_MODE_2,MCMACTX_BASE_ADDR,0x2,15,0},
};
const CHIP_REG mactx_statistic_clr_chan_reg[]=
{
    {STA_CLR_CHAN,MCMACTX_BASE_ADDR,0x6,6,0},
};
const CHIP_REG mactx_encode_config_reg[]=
{
    {LOCAL_FAULT_CONFIG,MCMACTX_BASE_ADDR,0x7,6,5},
    {REMOTE_FAULT_CONFIG,MCMACTX_BASE_ADDR,0x7,8,7},
    {TX_TEST_EN,MCMACTX_BASE_ADDR,0x7,0,0},
    {MANUAL_LF_INSERT,MCMACTX_BASE_ADDR,0x7,1,1},
    {MANUAL_IDLE_INSERT,MCMACTX_BASE_ADDR,0x7,3,3},
    {ENABLE_TX_FAULT,MCMACTX_BASE_ADDR,0x7,4,4},
    {MANUAL_RF_INSERT,MCMACTX_BASE_ADDR,0x7,2,2},
};
const CHIP_REG mactx_stat_enable_reg[]=
{
    {STAT_EN_0,MCMACTX_BASE_ADDR,0x60,31,0},
    {STAT_EN_1,MCMACTX_BASE_ADDR,0x61,31,0},
    {STAT_EN_2,MCMACTX_BASE_ADDR,0x62,15,0},
};
const CHIP_REG mactx_avg_ipg_reg[] =
{
	{AVG_IPG,MCMACTX_BASE_ADDR,0x64,2,0},
	{MINIMUM_MODE,MCMACTX_BASE_ADDR,0x64,3,3},
};
const CHIP_REG mactx_crc_inc_en_reg[]=
{
	{CRC_INC_EN_0,MCMACTX_BASE_ADDR,0x65,31,0},
	{CRC_INC_EN_1,MCMACTX_BASE_ADDR,0x66,31,0},
	{CRC_INC_EN_2,MCMACTX_BASE_ADDR,0x67,15,0},
};
const CHIP_REG mactx_crc_recal_en_reg[]=
{
	{CRC_RECAL_EN_0,MCMACTX_BASE_ADDR,0x69,31,0},
	{CRC_RECAL_EN_1,MCMACTX_BASE_ADDR,0x6a,31,0},
	{CRC_RECAL_EN_2,MCMACTX_BASE_ADDR,0x6b,15,0},
};
const CHIP_REG mactx_decode_sel_reg[]=
{
	{DECODE_SEL,MCMACTX_BASE_ADDR,0x6d,0,0},
};
const CHIP_REG mactx_statistic_all_clr_reg[]=
{
    {STA_ALL_CLR,MCMACTX_BASE_ADDR,0x70,0,0},
};
const CHIP_REG mactx_statistic_clr_reg[]=
{
    {STA_CLR,MCMACTX_BASE_ADDR,0x71,0,0},
};

const CHIP_REG mactx_ts_ram_init_reg[]=
{
    {TS_RAM_INIT,MCMACTX_BASE_ADDR,0x73,0,0},
};
const CHIP_REG mactx_env_ram_init_reg[]=
{
    {ENV_RAM_INIT,MCMACTX_BASE_ADDR,0x74,0,0},
};
const CHIP_REG mactx_ram_init_done_reg[]=
{
	{ENV_RAM_INIT_DONE,MCMACTX_BASE_ADDR,0x7f,0,0},
	{TS_RAM_INIT_DONE,MCMACTX_BASE_ADDR,0x7f,1,1},
	{STA_RAM_INIT_DONE,MCMACTX_BASE_ADDR,0x7f,2,2},
};
const CHIP_REG mactx_bussw_underflow_reg[]=
{
    {UNDERFLOW_0,MCMACTX_BASE_ADDR,0x80,31,0},
    {UNDERFLOW_1,MCMACTX_BASE_ADDR,0x81,31,0},
    {UNDERFLOW_2,MCMACTX_BASE_ADDR,0x82,15,0},
};
const CHIP_REG mactx_sar_lf_alm_reg[]=
{
    {SAR_LF_ALM_0,MCMACTX_BASE_ADDR,0x88,31,0},
    {SAR_LF_ALM_1,MCMACTX_BASE_ADDR,0x89,31,0},
    {SAR_LF_ALM_2,MCMACTX_BASE_ADDR,0x8a,15,0},
};
const CHIP_REG mactx_sar_rf_alm_reg[]=
{
    {SAR_RF_ALM_0,MCMACTX_BASE_ADDR,0x8c,31,0},
    {SAR_RF_ALM_1,MCMACTX_BASE_ADDR,0x8d,31,0},
    {SAR_RF_ALM_2,MCMACTX_BASE_ADDR,0x8e,15,0},
};
const CHIP_REG mactx_encode_err_alm_reg[]=
{
    {ENCODE_ERR_ALM_0,MCMACTX_BASE_ADDR,0x90,31,0},
    {ENCODE_ERR_ALM_1,MCMACTX_BASE_ADDR,0x91,31,0},
    {ENCODE_ERR_ALM_2,MCMACTX_BASE_ADDR,0x92,15,0},
};
const CHIP_REG mactx_decode_err_alm_reg[]=
{
    {DECODE_ERR_ALM_0,MCMACTX_BASE_ADDR,0x94,31,0},
    {DECODE_ERR_ALM_1,MCMACTX_BASE_ADDR,0x95,31,0},
    {DECODE_ERR_ALM_2,MCMACTX_BASE_ADDR,0x96,15,0},
};
const CHIP_REG mactx_sta_ram_ecc_correct_alm_reg[]=
{
	{MACTX_ENCODE_ERR_CODE_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,0,0},
	{MACTX_DECODE_ERR_CODE_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,1,1},
	{MACTX_TOTAL_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,2,2},
	{MACTX_GOOD_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,3,3},
	{MACTX_BAD_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,4,4},
	{MACTX_SAR_FRAGMENT_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,5,5},
	{MACTX_SAR_JABBER_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,6,6},
	{MACTX_SAR_OVERSIZE_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,7,7},
	{MACTX_UNICAST_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,8,8},
	{MACTX_MULTICAST_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,9,9},
	{MACTX_BROADCAST_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,10,10},
	{MACTX_B64_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,11,11},
	{MACTX_B65_MPL_FRAME_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,12,12},
	{MACTX_TOTAL_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,13,13},
	{MACTX_GOOD_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,14,14},
	{MACTX_BAD_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,15,15},
	{MACTX_SAR_FRAGMENT_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,16,16},
	{MACTX_SAR_JABBER_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,17,17},
	{MACTX_SAR_OVERSIZE_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,18,18},
	{MACTX_UNICAST_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,19,19},
	{MACTX_MULTICAST_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,20,20},
	{MACTX_BROADCAST_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,21,21},
	{MACTX_B64_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,22,22},
	{MACTX_B65_MPL_BYTES_ECC_ERR_CORRECT,MCMACTX_BASE_ADDR,0x98,23,23},
};
const CHIP_REG mactx_sta_ram_ecc_uncorrect_alm_reg[]=
{
	{MACTX_ENCODE_ERR_CODE_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,0,0},
	{MACTX_DECODE_ERR_CODE_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,1,1},
	{MACTX_TOTAL_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,2,2},
	{MACTX_GOOD_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,3,3},
	{MACTX_BAD_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,4,4},
	{MACTX_SAR_FRAGMENT_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,5,5},
	{MACTX_SAR_JABBER_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,6,6},
	{MACTX_SAR_OVERSIZE_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,7,7},
	{MACTX_UNICAST_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,8,8},
	{MACTX_MULTICAST_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,9,9},
	{MACTX_BROADCAST_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,10,10},
	{MACTX_B64_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,11,11},
	{MACTX_B65_MPL_FRAME_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,12,12},
	{MACTX_TOTAL_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,13,13},
	{MACTX_GOOD_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,14,14},
	{MACTX_BAD_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,15,15},
	{MACTX_SAR_FRAGMENT_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,16,16},
	{MACTX_SAR_JABBER_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,17,17},
	{MACTX_SAR_OVERSIZE_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,18,18},
	{MACTX_UNICAST_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,19,19},
	{MACTX_MULTICAST_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,20,20},
	{MACTX_BROADCAST_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,21,21},
	{MACTX_B64_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,22,22},
	{MACTX_B65_MPL_BYTES_ECC_ERR_UNCORRECT,MCMACTX_BASE_ADDR,0x99,23,23},
};
const CHIP_REG mactx_eop_abnor_alm_reg[]=
{
	{EOP_ABNOR_ALM_0,MCMACTX_BASE_ADDR,0x9a,31,0},
	{EOP_ABNOR_ALM_1,MCMACTX_BASE_ADDR,0x9b,31,0},
	{EOP_ABNOR_ALM_2,MCMACTX_BASE_ADDR,0x9c,15,0},
};
const CHIP_RAM mactx_ts_config = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xa0,7,
};
const CHIP_RAM mactx_max_pkt = 
{
	TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xa1,14,
};
const CHIP_RAM mactx_decode_err_code = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xb0,64,
};
const CHIP_RAM mactx_encode_err_code = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xb1,64,
};
const CHIP_RAM mactx_total_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xc0,64,
};
const CHIP_RAM mactx_good_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xc1,64,
};
const CHIP_RAM mactx_bad_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xc2,64,
};
const CHIP_RAM mactx_sar_fragment_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xc4,64,
};
const CHIP_RAM mactx_sar_jabber_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xc6,64,
};
const CHIP_RAM mactx_sar_oversize_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xc7,64,
};
const CHIP_RAM mactx_unicast_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xc8,64,
};
const CHIP_RAM mactx_multicast_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xc9,64,
};
const CHIP_RAM mactx_broadcast_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xca,64,
};
const CHIP_RAM mactx_b64_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xcb,64,
};
const CHIP_RAM mactx_b65_mpl_bytes = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xcc,64,
};
const CHIP_RAM mactx_total_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xd0,64,
};
const CHIP_RAM mactx_good_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xd1,64,
};
const CHIP_RAM mactx_bad_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xd2,64,
};
const CHIP_RAM mactx_sar_fragment_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xd4,64,
};
const CHIP_RAM mactx_sar_jabber_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xd6,64,
};
const CHIP_RAM mactx_sar_oversize_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xd7,64,
};
const CHIP_RAM mactx_unicast_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xd8,64,
};
const CHIP_RAM mactx_multicast_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xd9,64,
};
const CHIP_RAM mactx_broadcast_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xda,64,
};
const CHIP_RAM mactx_b64_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xdb,64,
};
const CHIP_RAM mactx_b65_mpl_frame = 
{
    TABLE_BASE_ADDR,MCMACTX_BASE_ADDR+0xdc,64,
};


/******************************************************************************
*
* FUNCTION
*
*     mcmactx_work_mode_set
*
* DESCRIPTION
*
*     Mac tx mac sar work mode cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: 1 is sar, 0 is mac
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
RET_STATUS mcmactx_work_mode_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1) ||
        (parameter > 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d, \
                   \r\n parameter = %d \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_work_mode_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,mactx_work_mode_reg[ch_id/32].base_addr,mactx_work_mode_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_work_mode_get
*
* DESCRIPTION
*
*     Mac tx mac sar work mode cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**   *parameter: pointer to cfg, 1 is sar, 0 is mac
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
RET_STATUS mcmactx_work_mode_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_work_mode_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_work_mode_reg[ch_id/32].base_addr,mactx_work_mode_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_statistic_clear_chan_set
*
* DESCRIPTION
*
*     Mac tx statistic_clear_chan cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: value
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
RET_STATUS mcmactx_statistic_clear_chan_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (parameter > 0x7f))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n parameter = %d \r\n",   
                   __FUNCTION__,
                   chip_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }
   
    rv = regp_write(chip_id,mactx_statistic_clr_chan_reg[STA_CLR_CHAN].base_addr,mactx_statistic_clr_chan_reg[STA_CLR_CHAN].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_statistic_clear_chan_get
*
* DESCRIPTION
*
*     Mac tx statistic_clear_chan cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**   *parameter: pointer to value
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
RET_STATUS mcmactx_statistic_clear_chan_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
    rv = regp_read(chip_id,mactx_statistic_clr_chan_reg[STA_CLR_CHAN].base_addr,mactx_statistic_clr_chan_reg[STA_CLR_CHAN].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_encode_config_set
*
* DESCRIPTION
*
*     Mac tx encode cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
*     field_id: 0-LOCAL_FAULT_CONFIG
*               1-REMOTE_FAULT_CONFIG
*               2-TX_TEST_EN
*               3-MANUAL_LF_INSERT
*               4-MANUAL_IDLE_INSERT
*               5-ENABLE_TX_FAULT
*               6-MANUAL_RF_INSERT
*
*     parameter: field value
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
RET_STATUS mcmactx_encode_config_set(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1) ||
        (field_id > MANUAL_RF_INSERT) ||
        (parameter > 0x3))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d, \
                   \r\n field_id = %d, \
                   \r\n parameter = %d \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id,
                   field_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(mactx_encode_config_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,mactx_encode_config_reg[field_id].base_addr,mactx_encode_config_reg[field_id].offset_addr+ch_id,
        mactx_encode_config_reg[field_id].end_bit,mactx_encode_config_reg[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_encode_config_get
*
* DESCRIPTION
*
*     Mac tx encode cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
*     field_id: 0-LOCAL_FAULT_CONFIG
*               1-REMOTE_FAULT_CONFIG
*               2-TX_TEST_EN
*               3-MANUAL_LF_INSERT
*               4-MANUAL_IDLE_INSERT
*               5-ENABLE_TX_FAULT
*               6-MANUAL_RF_INSERT
*
*    *parameter: pointer to field value
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
RET_STATUS mcmactx_encode_config_get(UINT_8 chip_id,UINT_8 ch_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1) ||
        (field_id > MANUAL_RF_INSERT))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d, \
                   \r\n field_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id,
                   field_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(mactx_encode_config_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_encode_config_reg[field_id].base_addr,mactx_encode_config_reg[field_id].offset_addr+ch_id,
        mactx_encode_config_reg[field_id].end_bit,mactx_encode_config_reg[field_id].start_bit,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_stat_enable_set
*
* DESCRIPTION
*
*     Mac tx stat enable cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: 1 is enable, 0 is disable
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
RET_STATUS mcmactx_stat_enable_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1) || (parameter > 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_stat_enable_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,mactx_stat_enable_reg[ch_id/32].base_addr,mactx_stat_enable_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_stat_enable_get
*
* DESCRIPTION
*
*     Mac tx stat enable cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**   *parameter: pointer to cfg, 1 is enable, 0 is disable
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
RET_STATUS mcmactx_stat_enable_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_stat_enable_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_stat_enable_reg[ch_id/32].base_addr,mactx_stat_enable_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_avg_ipg_set
*
* DESCRIPTION
*
*     Mac tx AVG_IPG cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter:0~2 2avg 10 IPG 1avg 9 IPG  0avg 8 IPG 
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
RET_STATUS mcmactx_avg_ipg_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (parameter > 7))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }
    
    rv = regp_field_write(chip_id,mactx_avg_ipg_reg[AVG_IPG],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_avg_ipg_get
*
* DESCRIPTION
*
*     Mac tx AVG_IPG cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**   *parameter: pointer to cfg, 2avg 10 IPG 1avg 9 IPG  0avg 8 IPG 
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
RET_STATUS mcmactx_avg_ipg_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) )
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,mactx_avg_ipg_reg[AVG_IPG],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_minimum_mode_set
*
* DESCRIPTION
*
*     mcmactx_minimum_mode_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter:0~1
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
*    dingyi    2018-04-18    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_minimum_mode_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (parameter > 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if (MINIMUM_MODE > MAX_INDEX(mactx_avg_ipg_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_write(chip_id,mactx_avg_ipg_reg[MINIMUM_MODE],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_minimum_mode_get
*
* DESCRIPTION
*
*     mcmactx_minimum_mode_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**   *parameter: pointer to cfg
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
*    dingyi    2018-04-18    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_minimum_mode_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) )
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (MINIMUM_MODE > MAX_INDEX(mactx_avg_ipg_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,mactx_avg_ipg_reg[MINIMUM_MODE],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_crc_inc_en_set
*
* DESCRIPTION
*
*     mcmactx_crc_inc_en_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: set value
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
*    dingyi    2018-04-18    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_crc_inc_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1) || (parameter > 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_crc_inc_en_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,mactx_crc_inc_en_reg[ch_id/32].base_addr,mactx_crc_inc_en_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_crc_inc_en_get
*
* DESCRIPTION
*
*     mcmactx_crc_inc_en_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**   *parameter: pointer to cfg
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
*    dingyi    2018-04-18    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_crc_inc_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_crc_inc_en_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_crc_inc_en_reg[ch_id/32].base_addr,mactx_crc_inc_en_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_crc_recal_en_set
*
* DESCRIPTION
*
*     mcmactx_crc_recal_en_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: set value
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
*    dingyi    2018-04-23    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_crc_recal_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1) || (parameter > 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d, \
                   \r\n parameter = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_crc_recal_en_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,mactx_crc_recal_en_reg[ch_id/32].base_addr,mactx_crc_recal_en_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_crc_recal_en_get
*
* DESCRIPTION
*
*     mcmactx_crc_recal_en_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**   *parameter: pointer to cfg
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
*    dingyi    2018-04-18    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_crc_recal_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_crc_recal_en_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_crc_recal_en_reg[ch_id/32].base_addr,mactx_crc_recal_en_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_decode_sel_set
*
* DESCRIPTION
*
*     mcmactx_decode_sel_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: set value
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
*    dingyi    2018-10-26    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_decode_sel_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (parameter > 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d \
                   \r\n parameter = %d\r\n",   
                   __FUNCTION__,
                   chip_id,
                   parameter
                 );
       }

       return RET_PARAERR;
    }
    
    rv = regp_field_write(chip_id, mactx_decode_sel_reg[DECODE_SEL], parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_decode_sel_get
*
* DESCRIPTION
*
*     mcmactx_decode_sel_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     parameter: get value
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
*    dingyi    2018-10-26    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_decode_sel_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

	if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
    rv = regp_field_read(chip_id, mactx_decode_sel_reg[DECODE_SEL], parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_all_pls_set
*
* DESCRIPTION
*
*     Mac tx all clr pls cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     pls_sel: 0-mactx_statistic_all_clr
*              1-mactx_statistic_clr   
*              2-mactx_ts_ram_init
*              3-mactx_env_ram_init
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
RET_STATUS mcmactx_all_pls_set(UINT_8 chip_id, UINT_8 pls_sel)
{
    RET_STATUS rv = RET_SUCCESS;
    const CHIP_REG *p = mactx_statistic_all_clr_reg;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (pls_sel > 3))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n pls_sel = %d  r\n",   
                   __FUNCTION__,
                   chip_id,
                   pls_sel
                 );
       }

       return RET_PARAERR;
    }

    switch (pls_sel)
    {
        case 0: p = mactx_statistic_all_clr_reg ; break;
        case 1: p = mactx_statistic_clr_reg     ; break;
        case 2: p = mactx_ts_ram_init_reg       ; break;
        case 3: p = mactx_env_ram_init_reg      ; break;
        default: break;
    }
    
    rv = regp_write(chip_id,p->base_addr,p->offset_addr,1);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_ts_init_done_get
*
* DESCRIPTION
*
*     Mac tx ts init done.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: pointer to st
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
RET_STATUS mcmactx_ts_init_done_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (TS_RAM_INIT_DONE > MAX_INDEX(mactx_ram_init_done_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,mactx_ram_init_done_reg[TS_RAM_INIT_DONE],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_sta_ram_init_done_get
*
* DESCRIPTION
*
*     Mac tx sta ram init done.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: pointer to st
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
RET_STATUS mcmactx_sta_ram_init_done_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (STA_RAM_INIT_DONE > MAX_INDEX(mactx_ram_init_done_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,mactx_ram_init_done_reg[STA_RAM_INIT_DONE],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_env_ram_init_done_get
*
* DESCRIPTION
*
*     Mac tx env ram init done.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
**    parameter: pointer to st
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
RET_STATUS mcmactx_env_ram_init_done_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,mactx_ram_init_done_reg[ENV_RAM_INIT_DONE],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_bussw_underflow_alm_get
*
* DESCRIPTION
*
*     Mac tx bussw_underflow alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: pointer to alm
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
RET_STATUS mcmactx_bussw_underflow_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_bussw_underflow_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_bussw_underflow_reg[ch_id/32].base_addr,mactx_bussw_underflow_reg[ch_id/32].offset_addr,
        ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_sar_lf_alm_get
*
* DESCRIPTION
*
*     Mac tx sar_lf alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: pointer to alm
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
RET_STATUS mcmactx_sar_lf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_sar_lf_alm_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_sar_lf_alm_reg[ch_id/32].base_addr,mactx_sar_lf_alm_reg[ch_id/32].offset_addr,
        ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_sar_rf_alm_get
*
* DESCRIPTION
*
*     Mac tx sar_rf alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: pointer to alm
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
RET_STATUS mcmactx_sar_rf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_sar_rf_alm_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_sar_rf_alm_reg[ch_id/32].base_addr,mactx_sar_rf_alm_reg[ch_id/32].offset_addr,
        ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_encode_err_alm_get
*
* DESCRIPTION
*
*     Mac tx encode_err alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: pointer to alm
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
RET_STATUS mcmactx_encode_err_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_encode_err_alm_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_encode_err_alm_reg[ch_id/32].base_addr,mactx_encode_err_alm_reg[ch_id/32].offset_addr,
        ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_decode_err_alm_get
*
* DESCRIPTION
*
*     Mac tx decode_err alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: pointer to alm
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
RET_STATUS mcmactx_decode_err_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n ch_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(mactx_decode_err_alm_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,mactx_decode_err_alm_reg[ch_id/32].base_addr,mactx_decode_err_alm_reg[ch_id/32].offset_addr,
        ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_sta_ram_ecc_correct_alm_get
*
* DESCRIPTION
*
*     mcmactx_sta_ram_ecc_correct_alm_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: field select 0~23
*				0:ENCODE  1:DECODE  2:TOTAL_FRAME  3:GOOD_FRAME  4:BAD_FRAME
*				5:SAR_FRAGMENT_FRAME  6:SAR_JABBER_FRAME  7:SAR_OVERSIZE_FRAME  8:UNICAST_FRAME
*				9:MULTICAST_FRAME  10:BROADCAST_FRAME  11:B64_FRAME  12:B65_MPL_FRAME
*				13:TOTAL_BYTES  14:GOOD_BYTES  15:BAD_BYTES  16:SAR_FRAGMENT_BYTES
*				17:SAR_JABBER_BYTES  18:SAR_OVERSIZE_BYTES  19:UNICAST_BYTES  20:MULTICAST_BYTES
*				21:BROADCAST_BYTES  22:B64_BYTES  23:B65_MPL_BYTES
**   *parameter: pointer to alm
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
*    dingyi   2018-5-14     1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_sta_ram_ecc_correct_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(mactx_sta_ram_ecc_correct_alm_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,mactx_sta_ram_ecc_correct_alm_reg[field_id],parameter);
    
    return rv;
	
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_sta_ram_ecc_uncorrect_alm_get
*
* DESCRIPTION
*
*     mcmactx_sta_ram_ecc_uncorrect_alm_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: field select 0~23
*				0:ENCODE  1:DECODE  2:TOTAL_FRAME  3:GOOD_FRAME  4:BAD_FRAME
*				5:SAR_FRAGMENT_FRAME  6:SAR_JABBER_FRAME  7:SAR_OVERSIZE_FRAME  8:UNICAST_FRAME
*				9:MULTICAST_FRAME  10:BROADCAST_FRAME  11:B64_FRAME  12:B65_MPL_FRAME
*				13:TOTAL_BYTES  14:GOOD_BYTES  15:BAD_BYTES  16:SAR_FRAGMENT_BYTES
*				17:SAR_JABBER_BYTES  18:SAR_OVERSIZE_BYTES  19:UNICAST_BYTES  20:MULTICAST_BYTES
*				21:BROADCAST_BYTES  22:B64_BYTES  23:B65_MPL_BYTES
**   *parameter: pointer to alm
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
*    dingyi   2018-5-14     1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_sta_ram_ecc_uncorrect_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(mactx_sta_ram_ecc_uncorrect_alm_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,mactx_sta_ram_ecc_uncorrect_alm_reg[field_id],parameter);
    
    return rv;
	
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_ts_config_ram_set
*
* DESCRIPTION 
*
*     Mac tx ts config ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
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
RET_STATUS mcmactx_ts_config_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,mactx_ts_config,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_ts_config_ram_get
*
* DESCRIPTION 
*
*     Mac tx ts config ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
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
RET_STATUS mcmactx_ts_config_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,mactx_ts_config,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_decode_err_code_ram_get
*
* DESCRIPTION 
*
*     Mac tx decode err code ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
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
RET_STATUS mcmactx_decode_err_code_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,mactx_decode_err_code,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_encode_err_code_ram_get
*
* DESCRIPTION 
*
*     Mac tx encode err code ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
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
RET_STATUS mcmactx_encode_err_code_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,mactx_encode_err_code,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_max_pkt_ram_set
*
* DESCRIPTION 
*
*     Mac tx mactx_max_pkt ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
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
RET_STATUS mcmactx_max_pkt_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,mactx_max_pkt,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_all_ram_get
*
* DESCRIPTION 
*
*     Get mac tx all ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ram_sel: 0 -mactx_ts_config            23-mactx_2001b_mpl_bytes  
*              1 -mactx_decode_err_code      24-mactx_total_frame      
*              2 -mactx_encode_err_code      25-mactx_good_frame       
*              3 -mactx_total_bytes          26-mactx_bad_frame        
*              4 -mactx_good_bytes           27-mactx_mac_fragment_frame
*              5 -mactx_bad_bytes            28-mactx_sar_fragment_frame
*              6 -mactx_mac_fragment_bytes   29-mactx_mac_jabber_frame 
*              7 -mactx_sar_fragment_bytes   30-mactx_sar_jabber_frame 
*              8 -mactx_mac_jabber_bytes     31-mactx_sar_oversize_frame  
*              9 -mactx_sar_jabber_bytes     32-mactx_unicast_frame    
*              10-mactx_sar_oversize_bytes   33-mactx_multicast_frame  
*              11-mactx_unicast_bytes        34-mactx_broadcast_frame  
*              12-mactx_multicast_bytes      35-mactx_64b_frame        
*              13-mactx_broadcast_bytes      36-mactx_65_127b_frame    
*              14-mactx_64b_bytes            37-mactx_128_255b_frame   
*              15-mactx_65_127b_bytes        38-mactx_256_511b_frame   
*              16-mactx_128_255b_bytes       39-mactx_512_1023b_frame  
*              17-mactx_256_511b_bytes       40-mactx_1024_1518b_frame 
*              18-mactx_512_1023b_bytes      41-mactx_1519_1522b_frame 
*              19-mactx_1024_1518b_bytes     42-mactx_1523_1548b_frame 
*              20-mactx_1519_1522b_bytes     43-mactx_1549_2000b_frame 
*              21-mactx_1523_1548b_bytes     44-mactx_2001b_mpl_frame  
*              22-mactx_1549_2000b_bytes 
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
RET_STATUS mcmactx_all_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    UINT_8 field_num = 0;
    UINT_8 field_id = 0;
    const CHIP_RAM *p = &mactx_ts_config;
    
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ram_sel > 44) || (index > CH_NUM - 1))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter!! \
                    \r\n chip_id = %d, \
                    \r\n index = %d  \r\n",
                    __FUNCTION__,
                    chip_id,
                    index
                  );
        }

        return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    switch (ram_sel)
    {
        case 0 : p = &mactx_ts_config         ; field_num = 1; field_id = 0; break;
        case 1 : p = &mactx_decode_err_code   ; field_num = 1; field_id = 0; break;
        case 2 : p = &mactx_encode_err_code   ; field_num = 1; field_id = 0; break;
        case 3 : p = &mactx_total_bytes       ; field_num = 1; field_id = 0; break;
        case 4 : p = &mactx_good_bytes        ; field_num = 1; field_id = 0; break;
        case 5 : p = &mactx_bad_bytes         ; field_num = 1; field_id = 0; break;
        case 6 : break;
        case 7 : p = &mactx_sar_fragment_bytes; field_num = 1; field_id = 0; break;
        case 8 : *parameter = 0; return rv;
        case 9 : p = &mactx_sar_jabber_bytes  ; field_num = 1; field_id = 0; break;
        case 10: p = &mactx_sar_oversize_bytes; field_num = 1; field_id = 0; break;
        case 11: p = &mactx_unicast_bytes     ; field_num = 1; field_id = 0; break;
        case 12: p = &mactx_multicast_bytes   ; field_num = 1; field_id = 0; break;
        case 13: p = &mactx_broadcast_bytes   ; field_num = 1; field_id = 0; break;
        case 14: p = &mactx_b64_bytes         ; field_num = 1; field_id = 0; break;
        case 15: p = &mactx_b65_mpl_bytes     ; field_num = 9; field_id = 0; break;
        case 16: p = &mactx_b65_mpl_bytes     ; field_num = 9; field_id = 1; break;
        case 17: p = &mactx_b65_mpl_bytes     ; field_num = 9; field_id = 2; break;
        case 18: p = &mactx_b65_mpl_bytes     ; field_num = 9; field_id = 3; break;
        case 19: p = &mactx_b65_mpl_bytes     ; field_num = 9; field_id = 4; break;
        case 20: p = &mactx_b65_mpl_bytes     ; field_num = 9; field_id = 5; break;
        case 21: p = &mactx_b65_mpl_bytes     ; field_num = 9; field_id = 6; break;
        case 22: p = &mactx_b65_mpl_bytes     ; field_num = 9; field_id = 7; break;
        case 23: p = &mactx_b65_mpl_bytes     ; field_num = 9; field_id = 8; break;
        case 24: p = &mactx_total_frame       ; field_num = 1; field_id = 0; break;
        case 25: p = &mactx_good_frame        ; field_num = 1; field_id = 0; break;
        case 26: p = &mactx_bad_frame         ; field_num = 1; field_id = 0; break;
        case 27: *parameter = 0; return rv;
        case 28: p = &mactx_sar_fragment_frame; field_num = 1; field_id = 0; break;
        case 29: *parameter = 0; return rv;
        case 30: p = &mactx_sar_jabber_frame  ; field_num = 1; field_id = 0; break;
        case 31: p = &mactx_sar_oversize_frame; field_num = 1; field_id = 0; break;
        case 32: p = &mactx_unicast_frame     ; field_num = 1; field_id = 0; break;
        case 33: p = &mactx_multicast_frame   ; field_num = 1; field_id = 0; break;
        case 34: p = &mactx_broadcast_frame   ; field_num = 1; field_id = 0; break;
        case 35: p = &mactx_b64_frame         ; field_num = 1; field_id = 0; break;
        case 36: p = &mactx_b65_mpl_frame     ; field_num = 9; field_id = 0; break;
        case 37: p = &mactx_b65_mpl_frame     ; field_num = 9; field_id = 1; break;
        case 38: p = &mactx_b65_mpl_frame     ; field_num = 9; field_id = 2; break;
        case 39: p = &mactx_b65_mpl_frame     ; field_num = 9; field_id = 3; break;
        case 40: p = &mactx_b65_mpl_frame     ; field_num = 9; field_id = 4; break;
        case 41: p = &mactx_b65_mpl_frame     ; field_num = 9; field_id = 5; break;
        case 42: p = &mactx_b65_mpl_frame     ; field_num = 9; field_id = 6; break;
        case 43: p = &mactx_b65_mpl_frame     ; field_num = 9; field_id = 7; break;
        case 44: p = &mactx_b65_mpl_frame     ; field_num = 9; field_id = 8; break;
        default: return rv;
    }
        
    rv = ram_read_single(chip_id,*p,index*field_num+field_id,parameter);
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_eop_abnor_alm_get
*
* DESCRIPTION
*
*     mcmactx_eop_abnor_alm_get
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  ch_id: 0-79
**   *parameter: pointer to alm
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
*    dingyi   2018-10-26     1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_eop_abnor_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1))
    {
       if (mcmactx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \
                   \r\n ch_id = %d \r\n",   
                   __FUNCTION__,
                   chip_id,
                   ch_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (ch_id/32 > MAX_INDEX(mactx_eop_abnor_alm_reg))
    {
        if (mcmactx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id, mactx_eop_abnor_alm_reg[ch_id/32].base_addr, mactx_eop_abnor_alm_reg[ch_id/32].offset_addr, 
		ch_id%32, ch_id%32, parameter);
    
    return rv;
	
}

   
UINT_8 g_ts_use_tab[DEV_NUM][TS_NUM] = {{0}};

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_config_init
*
* DESCRIPTION
*
*     mac tx ch config init.
*
* NOTE
*
*     chip_id: chip number used
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
*    dengjie    2018-1-16    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_config_init(UINT_8 chip_id)
{
    UINT_8  idx = 0;
    UINT_32 parameter[1];

    if (chip_id > DEV_NUM - 1)
    {
        printf("[%s] invalid parameter!! \
                \r\n\t chip_id = %d  \r\n", 
                __FUNCTION__,
                chip_id
              );

        return RET_PARAERR;
    }

	 /* init ts ram */
    for (idx = 0; idx < TS_NUM; idx++)
    {
        /* set ts config ram to default value */
        parameter[0] = 0x7f;
        mcmactx_ts_config_ram_set(chip_id,idx,parameter);
        
        /* sync g_ts_use_tab */
        g_ts_use_tab[chip_id][idx] = 0;
    }

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_config
*
* DESCRIPTION
*
*     mac ch config.
*
* NOTE
*
*     chip_id: chip number used
*     add_del_sel: 1-add,0-delete
*     ch_id: channel id, 0~CH_NUM - 1
*     ts_num: channel's ts number, 1~TS_NUM
*     mode: 0-mac, 1-sar
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
*    dengjie    2018-1-16    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_config(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_16 ts_num,UINT_32 mode)
{
    UINT_8  idx = 0;
    UINT_16 ts_cnt = 0;
    UINT_32 parameter[1];

    if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > CH_NUM - 1) ||
        (((ts_num < 1)&&(1 == add_del_sel)) || ts_num > TS_NUM) ||
        (mode > 1))
    {
        printf("[%s] invalid parameter!! \
                \r\n\t chip_id = %d, \
                \r\n\t add_del_sel = %d, \
                \r\n\t ch_id = %d, \
                \r\n\t ts_num = %d, \
                \r\n\t mode = %d\r\n", 
                __FUNCTION__,
                chip_id, 
                add_del_sel, 
                ch_id, 
                ts_num, 
                mode
              );

        return RET_PARAERR;
    }

    /* delete ch & config */
    if (0 == add_del_sel)
    {
        /* delete ch & config */
        for (idx = 0; idx < TS_NUM; idx++)
        {
            if ((ch_id + 1) == g_ts_use_tab[chip_id][idx])
            {
                /* set ts config ram to default value */
                parameter[0] = 0x7f;
                mcmactx_ts_config_ram_set(chip_id,idx,parameter);
                
                /* sync g_ts_use_tab */
                g_ts_use_tab[chip_id][idx] = 0;
            }
        }
        
        /* disable stat */
        mcmactx_stat_enable_set(chip_id,ch_id,0);
    }
    /* add ch & config */
    else
    {
        /* set max pkt */
        parameter[0] = 9600;
        mcmactx_max_pkt_ram_set(chip_id,ch_id,parameter);

        /* add ch & config */
        for (idx = 0, ts_cnt = 0; (idx < TS_NUM) && (ts_cnt < ts_num); idx++)
        {
            if (0 == g_ts_use_tab[chip_id][idx])
            {
                /* set ts config ram */
                parameter[0] = ch_id;
                mcmactx_ts_config_ram_set(chip_id,idx,parameter);
                
                /* sync g_ts_use_tab */
                g_ts_use_tab[chip_id][idx] = ch_id + 1;
                
                /* ts num cnt */
                ts_cnt++;
            }
        }

        /* enable stat */
        mcmactx_stat_enable_set(chip_id,ch_id,1);
    }
    
    /* set work mode */
    mcmactx_work_mode_set(chip_id,ch_id,mode);

    /* clear ch statistic */
    mcmactx_statistic_clear_chan_set(chip_id,ch_id);
    mcmactx_all_pls_set(chip_id, 1);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmactx_config1
*
* DESCRIPTION
*
*     mac ch config.
*
* NOTE
*
*     chip_id: chip number used
*     add_del_sel: 1-add,0-delete
*     ch_id: channel id, 0~CH_NUM - 1
*     ts_num: channel's ts number, 1~TS_NUM
*     mode: 0-mac, 1-sar
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
*    dengjie    2018-1-16    1.0           initial
*
******************************************************************************/
RET_STATUS mcmactx_config1(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_16 ts_num,UINT_32 mode,UINT_32 tx_max_pkt)
{
    UINT_8  idx = 0;
    UINT_16 ts_cnt = 0;
    UINT_32 parameter[1];

    if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > CH_NUM - 1) ||
        (ts_num < 1 || ts_num > TS_NUM) ||
        (mode > 1))
    {
        printf("[%s] invalid parameter!! \
                \r\n\t chip_id = %d, \
                \r\n\t add_del_sel = %d, \
                \r\n\t ch_id = %d, \
                \r\n\t ts_num = %d, \
                \r\n\t mode = %d\r\n", 
                __FUNCTION__,
                chip_id, 
                add_del_sel, 
                ch_id, 
                ts_num, 
                mode
              );

        return RET_PARAERR;
    }

    /* delete ch & config */
    if (0 == add_del_sel)
    {
        /* delete ch & config */
        for (idx = 0; idx < TS_NUM; idx++)
        {
            if ((ch_id + 1) == g_ts_use_tab[chip_id][idx])
            {
                /* set ts config ram to default value */
                parameter[0] = 0x7f;
                mcmactx_ts_config_ram_set(chip_id,idx,parameter);
                
                /* sync g_ts_use_tab */
                g_ts_use_tab[chip_id][idx] = 0;
            }
        }
        
        /* disable stat */
        mcmactx_stat_enable_set(chip_id,ch_id,0);
    }
    /* add ch & config */
    else
    {
        /* set max pkt */
        parameter[0] = tx_max_pkt;
        mcmactx_max_pkt_ram_set(chip_id,ch_id,parameter);

        /* add ch & config */
        for (idx = 0, ts_cnt = 0; (idx < TS_NUM) && (ts_cnt < ts_num); idx++)
        {
            if (0 == g_ts_use_tab[chip_id][idx])
            {
                /* set ts config ram */
                parameter[0] = ch_id;
                mcmactx_ts_config_ram_set(chip_id,idx,parameter);
                
                /* sync g_ts_use_tab */
                g_ts_use_tab[chip_id][idx] = ch_id + 1;
                
                /* ts num cnt */
                ts_cnt++;
            }
        }

        /* enable stat */
        mcmactx_stat_enable_set(chip_id,ch_id,1);
    }
    
    /* set work mode */
    mcmactx_work_mode_set(chip_id,ch_id,mode);

    /* clear ch statistic */
    mcmactx_statistic_clear_chan_set(chip_id,ch_id);
    mcmactx_all_pls_set(chip_id, 1);
    
    return RET_SUCCESS;
}


