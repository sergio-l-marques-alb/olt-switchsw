

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int mcmacrx_para_debug = 0;


const CHIP_REG macrx_cfg_reg[]=
{
    {WATCHDOG_EN,MCMACRX_BASE_ADDR,0x0,15,15},
    {WATCHDOG_TIME,MCMACRX_BASE_ADDR,0x0,23,16},
};
const CHIP_REG macrx_enable_reg[]=
{
    {MACRX_CHAN_EN_0,MCMACRX_BASE_ADDR,0x1,31,0},
    {MACRX_CHAN_EN_1,MCMACRX_BASE_ADDR,0x2,31,0},
	{MACRX_CHAN_EN_2,MCMACRX_BASE_ADDR,0x3,15,0},
};
const CHIP_REG macrx_stat_enable_reg[]=
{
    {MACRX_STAT_EN_0,MCMACRX_BASE_ADDR,0x4,31,0},
    {MACRX_STAT_EN_1,MCMACRX_BASE_ADDR,0x5,31,0},
    {MACRX_STAT_EN_2,MCMACRX_BASE_ADDR,0x6,15,0},
};
const CHIP_REG macrx_crc_cutoff_en_reg[]=
{
	{CRC_CUTOFF_EN_0,MCMACRX_BASE_ADDR,0x7,31,0},
	{CRC_CUTOFF_EN_1,MCMACRX_BASE_ADDR,0x8,31,0},
	{CRC_CUTOFF_EN_2,MCMACRX_BASE_ADDR,0x9,15,0},
};
const CHIP_REG macrx_statistic_clear_chan_reg[]=
{
    {STA_CLR_CHAN,MCMACRX_BASE_ADDR,0xb,6,0},
};
const CHIP_REG macrx_statistic_clear_reg[]=
{
    {STA_CLR_PLS,MCMACRX_BASE_ADDR,0xc,0,0},
};
const CHIP_REG macrx_statistic_all_clr_reg[]=
{
	{STA_ALL_CLR,MCMACRX_BASE_ADDR,0xf,1,1},
};
const CHIP_REG macrx_crc_errdrop_en_reg[]=
{
	{CRC_ERRDROP_EN_0,MCMACRX_BASE_ADDR,0x10,31,0},
	{CRC_ERRDROP_EN_1,MCMACRX_BASE_ADDR,0x11,31,0},
	{CRC_ERRDROP_EN_2,MCMACRX_BASE_ADDR,0x12,15,0},
};
const CHIP_REG macrx_decode_sel_reg[]=
{
	{DECODE_SEL,MCMACRX_BASE_ADDR,0x13,0,0},
};
const CHIP_REG macrx_out_sop_reg[]=
{
    {SOP_CNT,MCMACRX_BASE_ADDR,0x20,31,0},
};
const CHIP_REG macrx_out_eop_reg[]=
{
    {EOP_CNT,MCMACRX_BASE_ADDR,0x21,31,0},
};
const CHIP_REG macrx_sta_ram_init_done_reg[]=
{
	{RX_STA_RAM_INIT_DONE,MCMACRX_BASE_ADDR,0x30,0,0},
};
const CHIP_REG macrx_error_reg[]=
{
	{SOP_WARNING,MCMACRX_BASE_ADDR,0x40,0,0},
	{EOP_WARNING,MCMACRX_BASE_ADDR,0x40,1,1},
	{SW_RAM_FULL,MCMACRX_BASE_ADDR,0x40,2,2},
};
const CHIP_REG macrx_fault_alm_reg[]=
{
    {ARM_LF_0,MCMACRX_BASE_ADDR,0x53,31,0},
    {ARM_LF_1,MCMACRX_BASE_ADDR,0x54,31,0},
    {ARM_LF_2,MCMACRX_BASE_ADDR,0x55,15,0},
    {ARM_RF_0,MCMACRX_BASE_ADDR,0x55,31,16},
    {ARM_RF_1,MCMACRX_BASE_ADDR,0x56,31,0},
    {ARM_RF_2,MCMACRX_BASE_ADDR,0x57,31,0},
    {ARM_DECODE_ERR_0,MCMACRX_BASE_ADDR,0x58,31,0},
    {ARM_DECODE_ERR_1,MCMACRX_BASE_ADDR,0x59,31,0},
    {ARM_DECODE_ERR_2,MCMACRX_BASE_ADDR,0x5a,15,0},
};
const CHIP_REG macrx_sta_ram_ecc_correct_alm_reg[]=
{
	{ENCODE_ERR_CODE_ECC_ERR_CORRECT1,MCMACRX_BASE_ADDR,0x60,0,0},
	{DECODE_ERR_CODE_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,1,1},
	{TOTAL_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,2,2},
	{GOOD_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,3,3},
	{BAD_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,4,4},
	{FRAGMENT_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,5,5},
	{JABBER_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,6,6},
	{OVERSIZE_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,7,7},
	{UNICAST_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,8,8},
	{MULTICAST_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,9,9},
	{BROADCAST_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,10,10},
	{B64_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,11,11},
	{B65_MPL_FRAME_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,12,12},
	{TOTAL_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,13,13},
	{GOOD_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,14,14},
	{BAD_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,15,15},
	{FRAGMENT_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,16,16},
	{JABBER_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,17,17},
	{OVERSIZE_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,18,18},
	{UNICAST_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,19,19},
	{MULTICAST_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,20,20},
	{BROADCAST_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,21,21},
	{B64_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,22,22},
	{B65_MPL_BYTES_ECC_ERR_CORRECT,MCMACRX_BASE_ADDR,0x60,23,23},
};
const CHIP_REG macrx_sta_ram_ecc_uncorrect_alm_reg[]=
{
	{ENCODE_ERR_CODE_ECC_ERR_UNCORRECT1,MCMACRX_BASE_ADDR,0x61,0,0},
	{DECODE_ERR_CODE_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,1,1},
	{TOTAL_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,2,2},
	{GOOD_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,3,3},
	{BAD_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,4,4},
	{FRAGMENT_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,5,5},
	{JABBER_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,6,6},
	{OVERSIZE_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,7,7},
	{UNICAST_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,8,8},
	{MULTICAST_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,9,9},
	{BROADCAST_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,10,10},
	{B64_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,11,11},
	{B65_MPL_FRAME_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,12,12},
	{TOTAL_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,13,13},
	{GOOD_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,14,14},
	{BAD_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,15,15},
	{FRAGMENT_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,16,16},
	{JABBER_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,17,17},
	{OVERSIZE_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,18,18},
	{UNICAST_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,19,19},
	{MULTICAST_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,20,20},
	{BROADCAST_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,21,21},
	{B64_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,22,22},
	{B65_MPL_BYTES_ECC_ERR_UNCORRECT,MCMACRX_BASE_ADDR,0x61,23,23},
};
const CHIP_REG macrx_eop_abnor_alm_reg[]=
{
	{EOP_ABNOR_ALM_0,MCMACRX_BASE_ADDR,0x62,31,0},
	{EOP_ABNOR_ALM_1,MCMACRX_BASE_ADDR,0x63,31,0},
	{EOP_ABNOR_ALM_2,MCMACRX_BASE_ADDR,0x64,15,0},
};
const CHIP_RAM macrx_max_pkt = 
{
	TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xa0,14,
};
const CHIP_RAM macrx_decode_err_code = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xb0,64,
};
const CHIP_RAM macrx_total_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc0,64,
};
const CHIP_RAM macrx_good_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc1,64,
};
const CHIP_RAM macrx_bad_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc2,64,
};
const CHIP_RAM macrx_fragment_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc3,64,
};
const CHIP_RAM macrx_jabber_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc4,64,
};
const CHIP_RAM macrx_oversize_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc5,64,
};
const CHIP_RAM macrx_unicast_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc6,64,
};
const CHIP_RAM macrx_multicast_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc7,64,
};
const CHIP_RAM macrx_broadcast_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc8,64,
};
const CHIP_RAM macrx_b64_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xc9,64,
};
const CHIP_RAM macrx_b65_mpl_bytes = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xca,64,
};
const CHIP_RAM macrx_total_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd0,64,
};
const CHIP_RAM macrx_good_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd1,64,
};
const CHIP_RAM macrx_bad_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd2,64,
};
const CHIP_RAM macrx_fragment_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd3,64,
};
const CHIP_RAM macrx_jabber_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd4,64,
};
const CHIP_RAM macrx_oversize_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd5,64,
};
const CHIP_RAM macrx_unicast_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd6,64,
};
const CHIP_RAM macrx_multicast_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd7,64,
};
const CHIP_RAM macrx_broadcast_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd8,64,
};
const CHIP_RAM macrx_b64_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xd9,64,
};
const CHIP_RAM macrx_b65_mpl_frame = 
{
    TABLE_BASE_ADDR,MCMACRX_BASE_ADDR+0xda,64,
};


/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_cfg_set
*
* DESCRIPTION
*
*     Mac rx cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id: MAX_PKT~WATCHDOG_TIME
*               0-WATCHDOG_EN
*               1-WATCHDOG_TIME
*               
**    parameter: field value
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
RET_STATUS mcmacrx_cfg_set(UINT_8 chip_id,UINT_8 field_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (field_id > WATCHDOG_TIME))
    {
       if (mcmacrx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n field_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   field_id
                 );
       }

       return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(macrx_cfg_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_write(chip_id,macrx_cfg_reg[field_id],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_cfg_get
*
* DESCRIPTION
*
*     Mac rx cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     field_id: MAX_PKT~WATCHDOG_TIME
*               0-WATCHDOG_EN
*               1-WATCHDOG_TIME
**   *parameter: pointer to field value
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
RET_STATUS mcmacrx_cfg_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (field_id > WATCHDOG_TIME))
    {
       if (mcmacrx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d, \
                   \r\n field_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id,
                   field_id
                 );
       }

       return RET_PARAERR;
    }

    if (NULL == parameter)
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(macrx_cfg_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,macrx_cfg_reg[field_id],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_enable_set
*
* DESCRIPTION
*
*     Mac rx enable.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: 1 is MAC, 0 is SAR
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
RET_STATUS mcmacrx_enable_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1) ||
        (parameter > 1))
    {
       if (mcmacrx_para_debug)
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

    if ((ch_id/32) > MAX_INDEX(macrx_enable_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,macrx_enable_reg[ch_id/32].base_addr,macrx_enable_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_enable_get
*
* DESCRIPTION
*
*     Mac rx enable.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**   *parameter: pointer to enable cfg, 1 is MAC, 0 is SAR
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
RET_STATUS mcmacrx_enable_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(macrx_enable_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,macrx_enable_reg[ch_id/32].base_addr,macrx_enable_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_statistic_clear_chan_set
*
* DESCRIPTION
*
*     Mac rx statistic_clear_chan cfg.
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
RET_STATUS mcmacrx_statistic_clear_chan_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (parameter > 0x7f))
    {
       if (mcmacrx_para_debug)
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
   
    rv = regp_write(chip_id,macrx_statistic_clear_chan_reg[STA_CLR_CHAN].base_addr,macrx_statistic_clear_chan_reg[STA_CLR_CHAN].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_statistic_clear_chan_get
*
* DESCRIPTION
*
*     Mac rx statistic_clear_chan cfg.
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
RET_STATUS mcmacrx_statistic_clear_chan_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
    rv = regp_read(chip_id,macrx_statistic_clear_chan_reg[STA_CLR_CHAN].base_addr,macrx_statistic_clear_chan_reg[STA_CLR_CHAN].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_stat_enable_set
*
* DESCRIPTION
*
*     Mac rx stat enable.
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
RET_STATUS mcmacrx_stat_enable_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1) ||
        (parameter > 1))
    {
       if (mcmacrx_para_debug)
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

    if ((ch_id/32) > MAX_INDEX(macrx_stat_enable_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,macrx_stat_enable_reg[ch_id/32].base_addr,macrx_stat_enable_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_stat_enable_get
*
* DESCRIPTION
*
*     Mac rx stat enable.
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
RET_STATUS mcmacrx_stat_enable_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(macrx_stat_enable_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,macrx_stat_enable_reg[ch_id/32].base_addr,macrx_stat_enable_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_crc_cutoff_en_set
*
* DESCRIPTION
*
*     mcmacrx_crc_cutoff_en_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: 0~1
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
*    dingyi    2018-4-25    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_crc_cutoff_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1) ||
        (parameter > 1))
    {
       if (mcmacrx_para_debug)
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

    if ((ch_id/32) > MAX_INDEX(macrx_crc_cutoff_en_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,macrx_crc_cutoff_en_reg[ch_id/32].base_addr,macrx_crc_cutoff_en_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_crc_cutoff_en_get
*
* DESCRIPTION
*
*     mcmacrx_crc_cutoff_en_get
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
*    dingyi    2018-4-25    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_crc_cutoff_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(macrx_crc_cutoff_en_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,macrx_crc_cutoff_en_reg[ch_id/32].base_addr,macrx_crc_cutoff_en_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_statistic_clear_pls_set
*
* DESCRIPTION
*
*     Mac rx statistic clear pls.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
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
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_statistic_clear_pls_set(UINT_8 chip_id)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmacrx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }
    
    rv = regp_write(chip_id,macrx_statistic_clear_reg[STA_CLR_PLS].base_addr,macrx_statistic_clear_reg[STA_CLR_PLS].offset_addr,1);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_statistic_all_clr_pls_set
*
* DESCRIPTION
*
*     Mac rx statistic all clear pls.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
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
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_statistic_all_clr_pls_set(UINT_8 chip_id)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmacrx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }
   
    rv = regp_write(chip_id,macrx_statistic_all_clr_reg[STA_ALL_CLR].base_addr,macrx_statistic_all_clr_reg[STA_ALL_CLR].offset_addr,2);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_crc_errdrop_en_set
*
* DESCRIPTION
*
*     mcmacrx_crc_errdrop_en_set
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
**    parameter: 0~1
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
*    dingyi    2018-4-25    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_crc_errdrop_en_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1) ||
        (parameter > 1))
    {
       if (mcmacrx_para_debug)
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

    if ((ch_id/32) > MAX_INDEX(macrx_crc_errdrop_en_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,macrx_crc_errdrop_en_reg[ch_id/32].base_addr,macrx_crc_errdrop_en_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_crc_errdrop_en_get
*
* DESCRIPTION
*
*     mcmacrx_crc_errdrop_en_get
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
*    dingyi    2018-4-25    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_crc_errdrop_en_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(macrx_crc_errdrop_en_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,macrx_crc_errdrop_en_reg[ch_id/32].base_addr,macrx_crc_errdrop_en_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_decode_sel_set
*
* DESCRIPTION
*
*     mcmacrx_decode_sel_set
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
RET_STATUS mcmacrx_decode_sel_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (parameter > 1))
    {
       if (mcmacrx_para_debug)
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
   
    rv = regp_field_write(chip_id, macrx_decode_sel_reg[DECODE_SEL], parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_decode_sel_get
*
* DESCRIPTION
*
*     mcmacrx_decode_sel_get
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
RET_STATUS mcmacrx_decode_sel_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
    rv = regp_field_read(chip_id, macrx_decode_sel_reg[DECODE_SEL], parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_sta_ram_init_done_get
*
* DESCRIPTION
*
*     Mac rx lf alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
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
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_sta_ram_init_done_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
    rv = regp_field_read(chip_id,macrx_sta_ram_init_done_reg[RX_STA_RAM_INIT_DONE],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_arm_lf_alm_get
*
* DESCRIPTION
*
*     Mac rx lf alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
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
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_arm_lf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
   
    rv = regp_bit_read(chip_id,macrx_fault_alm_reg[ARM_LF_0].base_addr,macrx_fault_alm_reg[ARM_LF_0].offset_addr+ch_id/32,
        ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_arm_rf_alm_get
*
* DESCRIPTION
*
*     Mac rx rf alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
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
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_arm_rf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (ARM_RF_0 > MAX_INDEX(macrx_fault_alm_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,macrx_fault_alm_reg[ARM_RF_0].base_addr,macrx_fault_alm_reg[ARM_RF_0].offset_addr+(ch_id+16)/32,
        (ch_id+16)%32,(ch_id+16)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_arm_decode_err_alm_get
*
* DESCRIPTION
*
*     Mac rx decode err alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1
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
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_arm_decode_err_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (ARM_DECODE_ERR_0 > MAX_INDEX(macrx_fault_alm_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,macrx_fault_alm_reg[ARM_DECODE_ERR_0].base_addr,macrx_fault_alm_reg[ARM_DECODE_ERR_0].offset_addr+ch_id/32,
        ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_error_alm_get
*
* DESCRIPTION
*
*     Mac rx err alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
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
*    dengjie    2018-1-11    1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_error_alm_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }
    
    rv = regp_read(chip_id,macrx_error_reg[SOP_WARNING].base_addr,macrx_error_reg[SOP_WARNING].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_sta_ram_ecc_correct_alm_get
*
* DESCRIPTION
*
*     mcmacrx_sta_ram_ecc_correct_alm_get
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
*				5:FRAGMENT_FRAME  6:JABBER_FRAME  7:OVERSIZE_FRAME  8:UNICAST_FRAME
*				9:MULTICAST_FRAME  10:BROADCAST_FRAME  11:B64_FRAME  12:B65_MPL_FRAME
*				13:TOTAL_BYTES  14:GOOD_BYTES  15:BAD_BYTES  16:FRAGMENT_BYTES
*				17:JABBER_BYTES  18:OVERSIZE_BYTES  19:UNICAST_BYTES  20:MULTICAST_BYTES
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
*    dingyi   2018-5-10     1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_sta_ram_ecc_correct_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(macrx_sta_ram_ecc_correct_alm_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,macrx_sta_ram_ecc_correct_alm_reg[field_id],parameter);
    
    return rv;
	
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_sta_ram_ecc_uncorrect_alm_get
*
* DESCRIPTION
*
*     mcmacrx_sta_ram_ecc_uncorrect_alm_get
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
*				5:FRAGMENT_FRAME  6:JABBER_FRAME  7:OVERSIZE_FRAME  8:UNICAST_FRAME
*				9:MULTICAST_FRAME  10:BROADCAST_FRAME  11:B64_FRAME  12:B65_MPL_FRAME
*				13:TOTAL_BYTES  14:GOOD_BYTES  15:BAD_BYTES  16:FRAGMENT_BYTES
*				17:JABBER_BYTES  18:OVERSIZE_BYTES  19:UNICAST_BYTES  20:MULTICAST_BYTES
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
*    dingyi   2018-5-10     1.0           initial
*
******************************************************************************/
RET_STATUS mcmacrx_sta_ram_ecc_uncorrect_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(macrx_sta_ram_ecc_uncorrect_alm_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,macrx_sta_ram_ecc_uncorrect_alm_reg[field_id],parameter);
    
    return rv;
	
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_eop_abnor_alm_get
*
* DESCRIPTION
*
*     mcmacrx_eop_abnor_alm_get
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
RET_STATUS mcmacrx_eop_abnor_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1))
    {
       if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (ch_id/32 > MAX_INDEX(macrx_eop_abnor_alm_reg))
    {
        if (mcmacrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id, macrx_eop_abnor_alm_reg[ch_id/32].base_addr, macrx_eop_abnor_alm_reg[ch_id/32].offset_addr, 
		ch_id%32, ch_id%32, parameter);
    
    return rv;
	
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_max_pkt_ram_set
*
* DESCRIPTION 
*
*     Mac rx max pkt ram.
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
RET_STATUS mcmacrx_max_pkt_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > CH_NUM - 1))
    {
        if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,macrx_max_pkt,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_all_ram_get
*
* DESCRIPTION 
*
*     Get mac rx all ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ram_sel: 0 -macrx_decode_err_code     20-macrx_total_frame
*              1 -macrx_total_bytes         21-macrx_good_frame
*              2 -macrx_good_bytes          22-macrx_bad_frame
*              3 -macrx_bad_bytes           23-macrx_fragment_frame
*              4 -macrx_fragment_bytes      24-macrx_jabber_frame
*              5 -macrx_jabber_bytes        25-macrx_oversize_frame
*              6 -macrx_oversize_bytes      26-macrx_unicast_frame
*              7 -macrx_unicast_bytes       27-macrx_multicast_frame
*              8 -macrx_multicast_bytes     28-macrx_broadcast_frame
*              9 -macrx_broadcast_bytes     29-macrx_64b_frame
*              10-macrx_64b_bytes           30-macrx_65_127b_frame
*              11-macrx_65_127b_bytes       31-macrx_128_255b_frame
*              12-macrx_128_255b_bytes      32-macrx_256_511b_frame
*              13-macrx_256_511b_bytes      33-macrx_512_1023b_frame
*              14-macrx_512_1023b_bytes     34-macrx_1024_1518b_frame
*              15-macrx_1024_1518b_bytes    35-macrx_1519_1522b_frame
*              16-macrx_1519_1522b_bytes    36-macrx_1523_1548b_frame
*              17-macrx_1523_1548b_bytes    37-macrx_1549_2000b_frame
*              18-macrx_1549_2000b_bytes    38-macrx_2001b_mpl_frame
*              19-macrx_2000b_mpl_bytes
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
RET_STATUS mcmacrx_all_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    UINT_8 field_num = 0;
    UINT_8 field_id = 0;
    const CHIP_RAM *p = &macrx_decode_err_code;
    
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ram_sel > 38) || (index > CH_NUM - 1))
    {
        if (mcmacrx_para_debug)
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
        if (mcmacrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    switch (ram_sel)
    {
        case 0 : p = &macrx_decode_err_code       ; field_num = 1; field_id = 0; break;
        case 1 : p = &macrx_total_bytes           ; field_num = 1; field_id = 0; break;
        case 2 : p = &macrx_good_bytes            ; field_num = 1; field_id = 0; break;
        case 3 : p = &macrx_bad_bytes             ; field_num = 1; field_id = 0; break;
        case 4 : p = &macrx_fragment_bytes        ; field_num = 1; field_id = 0; break;
        case 5 : p = &macrx_jabber_bytes          ; field_num = 1; field_id = 0; break;
        case 6 : p = &macrx_oversize_bytes        ; field_num = 1; field_id = 0; break;
        case 7 : p = &macrx_unicast_bytes         ; field_num = 1; field_id = 0; break;
        case 8 : p = &macrx_multicast_bytes       ; field_num = 1; field_id = 0; break;
        case 9 : p = &macrx_broadcast_bytes       ; field_num = 1; field_id = 0; break;
        case 10: p = &macrx_b64_bytes             ; field_num = 1; field_id = 0; break;
        case 11: p = &macrx_b65_mpl_bytes         ; field_num = 9; field_id = 0; break;
        case 12: p = &macrx_b65_mpl_bytes         ; field_num = 9; field_id = 1; break;
        case 13: p = &macrx_b65_mpl_bytes         ; field_num = 9; field_id = 2; break;
        case 14: p = &macrx_b65_mpl_bytes         ; field_num = 9; field_id = 3; break;
        case 15: p = &macrx_b65_mpl_bytes         ; field_num = 9; field_id = 4; break;
        case 16: p = &macrx_b65_mpl_bytes         ; field_num = 9; field_id = 5; break;
        case 17: p = &macrx_b65_mpl_bytes         ; field_num = 9; field_id = 6; break;
        case 18: p = &macrx_b65_mpl_bytes         ; field_num = 9; field_id = 7; break;
        case 19: p = &macrx_b65_mpl_bytes         ; field_num = 9; field_id = 8; break;
        case 20: p = &macrx_total_frame           ; field_num = 1; field_id = 0; break;
        case 21: p = &macrx_good_frame            ; field_num = 1; field_id = 0; break;
        case 22: p = &macrx_bad_frame             ; field_num = 1; field_id = 0; break;
        case 23: p = &macrx_fragment_frame        ; field_num = 1; field_id = 0; break;
        case 24: p = &macrx_jabber_frame          ; field_num = 1; field_id = 0; break;
        case 25: p = &macrx_oversize_frame        ; field_num = 1; field_id = 0; break;
        case 26: p = &macrx_unicast_frame         ; field_num = 1; field_id = 0; break;
        case 27: p = &macrx_multicast_frame       ; field_num = 1; field_id = 0; break;
        case 28: p = &macrx_broadcast_frame       ; field_num = 1; field_id = 0; break;
        case 29: p = &macrx_b64_frame             ; field_num = 1; field_id = 0; break;
        case 30: p = &macrx_b65_mpl_frame         ; field_num = 9; field_id = 0; break;
        case 31: p = &macrx_b65_mpl_frame         ; field_num = 9; field_id = 1; break;
        case 32: p = &macrx_b65_mpl_frame         ; field_num = 9; field_id = 2; break;
        case 33: p = &macrx_b65_mpl_frame         ; field_num = 9; field_id = 3; break;
        case 34: p = &macrx_b65_mpl_frame         ; field_num = 9; field_id = 4; break;
        case 35: p = &macrx_b65_mpl_frame         ; field_num = 9; field_id = 5; break;
        case 36: p = &macrx_b65_mpl_frame         ; field_num = 9; field_id = 6; break;
        case 37: p = &macrx_b65_mpl_frame         ; field_num = 9; field_id = 7; break;
        case 38: p = &macrx_b65_mpl_frame         ; field_num = 9; field_id = 8; break;
        default: break;
    }
        
    rv = ram_read_single(chip_id,*p,index*field_num+field_id,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_config
*
* DESCRIPTION
*
*     mac rx ch config.
*
* NOTE
*
*     chip_id: chip number used
*     add_del_sel: 1-add,0-delete
*     ch_id: channel id, 0~CH_NUM - 1
*     mode: 1-mac, 0-sar
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
RET_STATUS mcmacrx_config(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_32 mode)
{
    UINT_32 parameter[1];

    if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > CH_NUM - 1) ||
        (mode > 1))
    {
        printf("[%s] invalid parameter!! \
                \r\n\t chip_id = %d, \
                \r\n\t add_del_sel = %d, \
                \r\n\t ch_id = %d, \
                \r\n\t mode = %d\r\n", 
                __FUNCTION__,
                chip_id, 
                add_del_sel, 
                ch_id, 
                mode
              );

        return RET_PARAERR;
    }

    /* set max pkt */
    parameter[0] = 9600;
    mcmacrx_max_pkt_ram_set(chip_id,ch_id,parameter);
    
    /* set chan enable */
    mcmacrx_enable_set(chip_id,ch_id,add_del_sel & mode);        

    /* set chan stat enable */
    mcmacrx_stat_enable_set(chip_id,ch_id,add_del_sel);
        
    /* clear ch statistic */
    mcmacrx_statistic_clear_chan_set(chip_id,ch_id);
    mcmacrx_statistic_clear_pls_set(chip_id);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_config1
*
* DESCRIPTION
*
*     mac rx ch config.
*
* NOTE
*
*     chip_id: chip number used
*     add_del_sel: 1-add,0-delete
*     ch_id: channel id, 0~CH_NUM - 1
*     mode: 1-mac, 0-sar
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
RET_STATUS mcmacrx_config1(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_32 mode,UINT_32 rx_max_pkt)
{
    UINT_32 val[1];
    if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > CH_NUM - 1) ||
        (mode > 1))
    {
        printf("[%s] invalid parameter!! \
                \r\n\t chip_id = %d, \
                \r\n\t add_del_sel = %d, \
                \r\n\t ch_id = %d, \
                \r\n\t mode = %d\r\n", 
                __FUNCTION__,
                chip_id, 
                add_del_sel, 
                ch_id, 
                mode
              );

        return RET_PARAERR;
    }

    /* set max pkt */
    val[0] = rx_max_pkt;
    mcmacrx_max_pkt_ram_set(chip_id,ch_id,val);
    
    /* set chan enable */
    mcmacrx_enable_set(chip_id,ch_id,add_del_sel & mode);        

    /* set chan stat enable */
    mcmacrx_stat_enable_set(chip_id,ch_id,add_del_sel);
        
    /* clear ch statistic */
    mcmacrx_statistic_clear_chan_set(chip_id,ch_id);
    mcmacrx_statistic_clear_pls_set(chip_id);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmacrx_out_cnt_print
*
* DESCRIPTION
*
*     mac rx out sop/eop print, only for debug.
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
RET_STATUS mcmacrx_out_cnt_print(UINT_8 chip_id)
{
    UINT_32 cnt_val[2] = {0};
    
    if (chip_id > DEV_NUM - 1)
    {
        printf("[%s] invalid parameter!! \
                \r\n\t chip_id = %d  \r\n", 
                __FUNCTION__,
                chip_id
              );

        return RET_PARAERR;
    }

    /* get cnt val */
    regp_read(chip_id,macrx_out_sop_reg[SOP_CNT].base_addr,macrx_out_sop_reg[SOP_CNT].offset_addr,&cnt_val[0]);
    regp_read(chip_id,macrx_out_eop_reg[EOP_CNT].base_addr,macrx_out_eop_reg[EOP_CNT].offset_addr,&cnt_val[1]);

    /* Print cnt val */
    printf("Mcmac rx out_sop cnt = %d\n", cnt_val[0]);
    printf("Mcmac rx out_eop cnt = %d\n", cnt_val[1]);
    
    return RET_SUCCESS;
}

