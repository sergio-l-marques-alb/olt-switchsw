

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "reg_operate.h"
#include "hal.h"
#include "sal.h"
#include <stdio.h>
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int flexe_macrx_para_debug = 0;


const CHIP_REG flexe_macrx_stat_enable_reg[]=
{
    {MACRX_STAT_EN_0,FLEXE_MACRX_BASE_ADDR,0x4,31,0},
    {MACRX_STAT_EN_1,FLEXE_MACRX_BASE_ADDR,0x5,31,0},
    {MACRX_STAT_EN_2,FLEXE_MACRX_BASE_ADDR,0x6,15,0},
};
const CHIP_REG flexe_macrx_statistic_clear_chan_reg[]=
{
    {STA_CLR_CHAN,FLEXE_MACRX_BASE_ADDR,0xb,6,0},
};
const CHIP_REG flexe_macrx_decode_sel_reg[]=
{
	{DECODE_SEL,FLEXE_MACRX_BASE_ADDR,0xd,0,0},
};
const CHIP_REG flexe_macrx_statistic_clear_reg[]=
{
    {STA_CLR_PLS,FLEXE_MACRX_BASE_ADDR,0xc,0,0},
};
const CHIP_REG flexe_macrx_statistic_all_clr_reg[]=
{
	{STA_ALL_CLR,FLEXE_MACRX_BASE_ADDR,0xf,1,1},
};
const CHIP_REG flexe_macrx_sta_ram_init_done_reg[]=
{
	{RX_STA_RAM_INIT_DONE,FLEXE_MACRX_BASE_ADDR,0x30,0,0},
};
const CHIP_REG flexe_macrx_error_reg[]=
{
	{SOP_WARNING,FLEXE_MACRX_BASE_ADDR,0x40,0,0},
	{EOP_WARNING,FLEXE_MACRX_BASE_ADDR,0x40,1,1},
};
const CHIP_REG flexe_macrx_fault_alm_reg[]=
{
    {ARM_LF_0,FLEXE_MACRX_BASE_ADDR,0x53,31,0},
    {ARM_LF_1,FLEXE_MACRX_BASE_ADDR,0x54,31,0},
    {ARM_LF_2,FLEXE_MACRX_BASE_ADDR,0x55,15,0},
    {ARM_RF_0,FLEXE_MACRX_BASE_ADDR,0x55,31,16},
    {ARM_RF_1,FLEXE_MACRX_BASE_ADDR,0x56,31,0},
    {ARM_RF_2,FLEXE_MACRX_BASE_ADDR,0x57,31,0},
    {ARM_DECODE_ERR_0,FLEXE_MACRX_BASE_ADDR,0x58,31,0},
    {ARM_DECODE_ERR_1,FLEXE_MACRX_BASE_ADDR,0x59,31,0},
    {ARM_DECODE_ERR_2,FLEXE_MACRX_BASE_ADDR,0x5a,15,0},
};
const CHIP_REG flexe_macrx_sta_ram_ecc_correct_alm_reg[]=
{
	{ENCODE_ERR_CODE_ECC_ERR_CORRECT1,FLEXE_MACRX_BASE_ADDR,0x60,0,0},
	{DECODE_ERR_CODE_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,1,1},
	{TOTAL_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,2,2},
	{GOOD_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,3,3},
	{BAD_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,4,4},
	{FRAGMENT_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,5,5},
	{JABBER_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,6,6},
	{OVERSIZE_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,7,7},
	{UNICAST_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,8,8},
	{MULTICAST_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,9,9},
	{BROADCAST_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,10,10},
	{B64_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,11,11},
	{B65_MPL_FRAME_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,12,12},
	{TOTAL_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,13,13},
	{GOOD_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,14,14},
	{BAD_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,15,15},
	{FRAGMENT_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,16,16},
	{JABBER_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,17,17},
	{OVERSIZE_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,18,18},
	{UNICAST_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,19,19},
	{MULTICAST_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,20,20},
	{BROADCAST_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,21,21},
	{B64_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,22,22},
	{B65_MPL_BYTES_ECC_ERR_CORRECT,FLEXE_MACRX_BASE_ADDR,0x60,23,23},
};
const CHIP_REG flexe_macrx_sta_ram_ecc_uncorrect_alm_reg[]=
{
	{B64_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,22,22},
	{B65_MPL_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,23,23},
	{ENCODE_ERR_CODE_ECC_ERR_UNCORRECT1,FLEXE_MACRX_BASE_ADDR,0x61,0,0},
	{DECODE_ERR_CODE_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,1,1},
	{TOTAL_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,2,2},
	{GOOD_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,3,3},
	{BAD_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,4,4},
	{FRAGMENT_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,5,5},
	{JABBER_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,6,6},
	{OVERSIZE_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,7,7},
	{UNICAST_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,8,8},
	{MULTICAST_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,9,9},
	{BROADCAST_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,10,10},
	{B64_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,11,11},
	{B65_MPL_FRAME_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,12,12},
	{TOTAL_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,13,13},
	{GOOD_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,14,14},
	{BAD_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,15,15},
	{FRAGMENT_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,16,16},
	{JABBER_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,17,17},
	{OVERSIZE_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,18,18},
	{UNICAST_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,19,19},
	{MULTICAST_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,20,20},
	{BROADCAST_BYTES_ECC_ERR_UNCORRECT,FLEXE_MACRX_BASE_ADDR,0x61,21,21},
};
const CHIP_REG flexe_macrx_eop_abnor_alm_reg[]=
{
	{EOP_ABNOR_ALM_0,FLEXE_MACRX_BASE_ADDR,0x62,31,0},
	{EOP_ABNOR_ALM_1,FLEXE_MACRX_BASE_ADDR,0x63,31,0},
	{EOP_ABNOR_ALM_2,FLEXE_MACRX_BASE_ADDR,0x64,15,0},
};
const CHIP_RAM flexe_macrx_max_pkt = 
{
	TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xa0,14,
};
const CHIP_RAM flexe_macrx_decode_err_code = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xb0,64,
};
const CHIP_RAM flexe_macrx_total_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc0,64,
};
const CHIP_RAM flexe_macrx_good_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc1,64,
};
const CHIP_RAM flexe_macrx_bad_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc2,64,
};
const CHIP_RAM flexe_macrx_fragment_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc3,64,
};
const CHIP_RAM flexe_macrx_jabber_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc4,64,
};
const CHIP_RAM flexe_macrx_oversize_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc5,64,
};
const CHIP_RAM flexe_macrx_unicast_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc6,64,
};
const CHIP_RAM flexe_macrx_multicast_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc7,64,
};
const CHIP_RAM flexe_macrx_broadcast_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc8,64,
};
const CHIP_RAM flexe_macrx_b64_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xc9,64,
};
const CHIP_RAM flexe_macrx_b65_mpl_bytes = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xca,64,
};
const CHIP_RAM flexe_macrx_total_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd0,64,
};
const CHIP_RAM flexe_macrx_good_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd1,64,
};
const CHIP_RAM flexe_macrx_bad_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd2,64,
};
const CHIP_RAM flexe_macrx_fragment_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd3,64,
};
const CHIP_RAM flexe_macrx_jabber_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd4,64,
};
const CHIP_RAM flexe_macrx_oversize_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd5,64,
};
const CHIP_RAM flexe_macrx_unicast_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd6,64,
};
const CHIP_RAM flexe_macrx_multicast_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd7,64,
};
const CHIP_RAM flexe_macrx_broadcast_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd8,64,
};
const CHIP_RAM flexe_macrx_b64_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xd9,64,
};
const CHIP_RAM flexe_macrx_b65_mpl_frame = 
{
    TABLE_BASE_ADDR,FLEXE_MACRX_BASE_ADDR+0xda,64,
};


/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_statistic_clear_chan_set
*
* DESCRIPTION
*
*     Flexe mac rx statistic_clear_chan cfg.
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
RET_STATUS flexe_macrx_statistic_clear_chan_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (parameter > 0x7f))
    {
       if (flexe_macrx_para_debug)
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

    rv = regp_write(chip_id,flexe_macrx_statistic_clear_chan_reg[STA_CLR_CHAN].base_addr,flexe_macrx_statistic_clear_chan_reg[STA_CLR_CHAN].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_statistic_clear_chan_get
*
* DESCRIPTION
*
*     Flexe mac rx statistic_clear_chan cfg.
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
RET_STATUS flexe_macrx_statistic_clear_chan_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    rv = regp_read(chip_id,flexe_macrx_statistic_clear_chan_reg[STA_CLR_CHAN].base_addr,flexe_macrx_statistic_clear_chan_reg[STA_CLR_CHAN].offset_addr,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_stat_enable_set
*
* DESCRIPTION
*
*     Flexe mac rx stat enable.
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
RET_STATUS flexe_macrx_stat_enable_set(UINT_8 chip_id,UINT_8 ch_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1) ||
        (parameter > 1))
    {
       if (flexe_macrx_para_debug)
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

    if ((ch_id/32) > MAX_INDEX(flexe_macrx_stat_enable_reg))
    {
        if (flexe_macrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_write(chip_id,flexe_macrx_stat_enable_reg[ch_id/32].base_addr,flexe_macrx_stat_enable_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_stat_enable_get
*
* DESCRIPTION
*
*     Flexe mac rx stat enable.
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
RET_STATUS flexe_macrx_stat_enable_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if ((ch_id/32) > MAX_INDEX(flexe_macrx_stat_enable_reg))
    {
        if (flexe_macrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,flexe_macrx_stat_enable_reg[ch_id/32].base_addr,flexe_macrx_stat_enable_reg[ch_id/32].offset_addr,ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_decode_sel_set
*
* DESCRIPTION
*
*     flexe_macrx_decode_sel_set
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
RET_STATUS flexe_macrx_decode_sel_set(UINT_8 chip_id,UINT_32 parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (parameter > 1))
    {
       if (flexe_macrx_para_debug)
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

    rv = regp_field_write(chip_id, flexe_macrx_decode_sel_reg[DECODE_SEL], parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_decode_sel_get
*
* DESCRIPTION
*
*     flexe_macrx_decode_sel_get
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
RET_STATUS flexe_macrx_decode_sel_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    rv = regp_field_read(chip_id, flexe_macrx_decode_sel_reg[DECODE_SEL], parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_statistic_clear_pls_set
*
* DESCRIPTION
*
*     Flexe mac rx statistic clear pls.
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
RET_STATUS flexe_macrx_statistic_clear_pls_set(UINT_8 chip_id)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_macrx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    rv = regp_write(chip_id,flexe_macrx_statistic_clear_reg[STA_CLR_PLS].base_addr,flexe_macrx_statistic_clear_reg[STA_CLR_PLS].offset_addr,1);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_statistic_all_clr_pls_set
*
* DESCRIPTION
*
*     Flexe mac rx statistic all clear pls.
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
RET_STATUS flexe_macrx_statistic_all_clr_pls_set(UINT_8 chip_id)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_macrx_para_debug)
       {
           printf("[%s] invalid parameter!! \
                   \r\n chip_id = %d  \r\n",   
                   __FUNCTION__,
                   chip_id
                 );
       }

       return RET_PARAERR;
    }

    rv = regp_write(chip_id,flexe_macrx_statistic_all_clr_reg[STA_ALL_CLR].base_addr,flexe_macrx_statistic_all_clr_reg[STA_ALL_CLR].offset_addr,2);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_sta_ram_init_done_get
*
* DESCRIPTION
*
*     Flexe mac rx lf alm.
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
RET_STATUS flexe_macrx_sta_ram_init_done_get(UINT_8 chip_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    rv = regp_field_read(chip_id,flexe_macrx_sta_ram_init_done_reg[RX_STA_RAM_INIT_DONE],parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_arm_lf_alm_get
*
* DESCRIPTION
*
*     Flexe mac rx lf alm.
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
RET_STATUS flexe_macrx_arm_lf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    rv = regp_bit_read(chip_id,flexe_macrx_fault_alm_reg[ARM_LF_0].base_addr,flexe_macrx_fault_alm_reg[ARM_LF_0].offset_addr+ch_id/32,
        ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_arm_rf_alm_get
*
* DESCRIPTION
*
*     Flexe mac rx rf alm.
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
RET_STATUS flexe_macrx_arm_rf_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (ARM_RF_0 > MAX_INDEX(flexe_macrx_fault_alm_reg))
    {
        if (flexe_macrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,flexe_macrx_fault_alm_reg[ARM_RF_0].base_addr,flexe_macrx_fault_alm_reg[ARM_RF_0].offset_addr+(ch_id+16)/32,
        (ch_id+16)%32,(ch_id+16)%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_arm_decode_err_alm_get
*
* DESCRIPTION
*
*     Flexe mac rx decode err alm.
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
RET_STATUS flexe_macrx_arm_decode_err_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) ||
        (ch_id > CH_NUM - 1))
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (ARM_DECODE_ERR_0 > MAX_INDEX(flexe_macrx_fault_alm_reg))
    {
        if (flexe_macrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id,flexe_macrx_fault_alm_reg[ARM_DECODE_ERR_0].base_addr,flexe_macrx_fault_alm_reg[ARM_DECODE_ERR_0].offset_addr+ch_id/32,
        ch_id%32,ch_id%32,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_error_alm_get
*
* DESCRIPTION
*
*     Flexe mac rx err alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*	  field_id: 0:SOP_WARNING 1:EOP_WARNING
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
RET_STATUS flexe_macrx_error_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(flexe_macrx_error_reg))
    {
        if (flexe_macrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id, flexe_macrx_error_reg[field_id], parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_sta_ram_ecc_correct_alm_get
*
* DESCRIPTION
*
*     flexe_macrx_sta_ram_ecc_correct_alm_get
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
RET_STATUS flexe_macrx_sta_ram_ecc_correct_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(flexe_macrx_sta_ram_ecc_correct_alm_reg))
    {
        if (flexe_macrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,flexe_macrx_sta_ram_ecc_correct_alm_reg[field_id],parameter);
    
    return rv;
	
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_sta_ram_ecc_uncorrect_alm_get
*
* DESCRIPTION
*
*     flexe_macrx_sta_ram_ecc_uncorrect_alm_get
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
RET_STATUS flexe_macrx_sta_ram_ecc_uncorrect_alm_get(UINT_8 chip_id,UINT_8 field_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if (chip_id > MAX_DEV)
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (field_id > MAX_INDEX(flexe_macrx_sta_ram_ecc_uncorrect_alm_reg))
    {
        if (flexe_macrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_field_read(chip_id,flexe_macrx_sta_ram_ecc_uncorrect_alm_reg[field_id],parameter);
    
    return rv;
	
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_eop_abnor_alm_get
*
* DESCRIPTION
*
*     flexe_macrx_eop_abnor_alm_get
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
RET_STATUS flexe_macrx_eop_abnor_alm_get(UINT_8 chip_id,UINT_8 ch_id,UINT_32* parameter)
{
	RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ch_id > CH_NUM - 1))
    {
       if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        return RET_PARAERR;
    }

    if (ch_id/32 > MAX_INDEX(flexe_macrx_eop_abnor_alm_reg))
    {
        if (flexe_macrx_para_debug)
        {
            printf("[%s] out of array range\r\n",__FUNCTION__);
        }

        return RET_PARAERR;
    }
    
    rv = regp_bit_read(chip_id, flexe_macrx_eop_abnor_alm_reg[ch_id/32].base_addr, flexe_macrx_eop_abnor_alm_reg[ch_id/32].offset_addr, 
		ch_id%32, ch_id%32, parameter);
    
    return rv;
	
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_max_pkt_ram_set
*
* DESCRIPTION 
*
*     Flexe mac rx max pkt ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~79
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
RET_STATUS flexe_macrx_max_pkt_ram_set(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > (CH_NUM - 1)))
    {
        if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_write_single(chip_id,flexe_macrx_max_pkt,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_max_pkt_ram_get
*
* DESCRIPTION 
*
*     Flexe mac rx max pkt ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~79
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
RET_STATUS flexe_macrx_max_pkt_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > (CH_NUM - 1)))
    {
        if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,flexe_macrx_max_pkt,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_decode_err_code_ram_get
*
* DESCRIPTION 
*
*     Flexe mac rx decode err code ram.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     index: 0~79
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
RET_STATUS flexe_macrx_decode_err_code_ram_get(UINT_8 chip_id,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;

    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (index > (CH_NUM - 1)))
    {
        if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    rv = ram_read_single(chip_id,flexe_macrx_decode_err_code,index,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_all_ram_get
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
*     ram_sel: 0 -flexe_macrx_decode_err_code     20-flexe_macrx_total_frame
*              1 -flexe_macrx_total_bytes         21-flexe_macrx_good_frame
*              2 -flexe_macrx_good_bytes          22-flexe_macrx_bad_frame
*              3 -flexe_macrx_bad_bytes           23-flexe_macrx_fragment_frame
*              4 -flexe_macrx_fragment_bytes      24-flexe_macrx_jabber_frame
*              5 -flexe_macrx_jabber_bytes        25-flexe_macrx_oversize_frame
*              6 -flexe_macrx_oversize_bytes      26-flexe_macrx_unicast_frame
*              7 -flexe_macrx_unicast_bytes       27-flexe_macrx_multicast_frame
*              8 -flexe_macrx_multicast_bytes     28-flexe_macrx_broadcast_frame
*              9 -flexe_macrx_broadcast_bytes     29-flexe_macrx_64b_frame
*              10-flexe_macrx_64b_bytes           30-flexe_macrx_65_127b_frame
*              11-flexe_macrx_65_127b_bytes       31-flexe_macrx_128_255b_frame
*              12-flexe_macrx_128_255b_bytes      32-flexe_macrx_256_511b_frame
*              13-flexe_macrx_256_511b_bytes      33-flexe_macrx_512_1023b_frame
*              14-flexe_macrx_512_1023b_bytes     34-flexe_macrx_1024_1518b_frame
*              15-flexe_macrx_1024_1518b_bytes    35-flexe_macrx_1519_1522b_frame
*              16-flexe_macrx_1519_1522b_bytes    36-flexe_macrx_1523_1548b_frame
*              17-flexe_macrx_1523_1548b_bytes    37-flexe_macrx_1549_2000b_frame
*              18-flexe_macrx_1549_2000b_bytes    38-flexe_macrx_2001b_mpl_frame
*              19-flexe_macrx_2000b_mpl_bytes
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
RET_STATUS flexe_macrx_all_ram_get(UINT_8 chip_id,UINT_8 ram_sel,UINT_8 index,UINT_32* parameter)
{
    RET_STATUS rv = RET_SUCCESS;
    UINT_8 field_num = 0;
    UINT_8 field_id = 0;
    const CHIP_RAM *p = &flexe_macrx_decode_err_code;
    
    /* check parameter whether valid */
    if ((chip_id > MAX_DEV) || (ram_sel > 38) || (index > CH_NUM - 1))
    {
        if (flexe_macrx_para_debug)
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
        if (flexe_macrx_para_debug)
        {
            printf("[%s] invalid parameter pointer\r\n",__FUNCTION__);
        }
        
        return RET_PARAERR;
    }   

    switch (ram_sel)
    {
        case 0 : p = &flexe_macrx_decode_err_code       ; field_num = 1; field_id = 0; break;
        case 1 : p = &flexe_macrx_total_bytes           ; field_num = 1; field_id = 0; break;
        case 2 : p = &flexe_macrx_good_bytes            ; field_num = 1; field_id = 0; break;
        case 3 : p = &flexe_macrx_bad_bytes             ; field_num = 1; field_id = 0; break;
        case 4 : p = &flexe_macrx_fragment_bytes        ; field_num = 1; field_id = 0; break;
        case 5 : p = &flexe_macrx_jabber_bytes          ; field_num = 1; field_id = 0; break;
        case 6 : p = &flexe_macrx_oversize_bytes        ; field_num = 1; field_id = 0; break;
        case 7 : p = &flexe_macrx_unicast_bytes         ; field_num = 1; field_id = 0; break;
        case 8 : p = &flexe_macrx_multicast_bytes       ; field_num = 1; field_id = 0; break;
        case 9 : p = &flexe_macrx_broadcast_bytes       ; field_num = 1; field_id = 0; break;
        case 10: p = &flexe_macrx_b64_bytes             ; field_num = 1; field_id = 0; break;
        case 11: p = &flexe_macrx_b65_mpl_bytes         ; field_num = 9; field_id = 0; break;
        case 12: p = &flexe_macrx_b65_mpl_bytes         ; field_num = 9; field_id = 1; break;
        case 13: p = &flexe_macrx_b65_mpl_bytes         ; field_num = 9; field_id = 2; break;
        case 14: p = &flexe_macrx_b65_mpl_bytes         ; field_num = 9; field_id = 3; break;
        case 15: p = &flexe_macrx_b65_mpl_bytes         ; field_num = 9; field_id = 4; break;
        case 16: p = &flexe_macrx_b65_mpl_bytes         ; field_num = 9; field_id = 5; break;
        case 17: p = &flexe_macrx_b65_mpl_bytes         ; field_num = 9; field_id = 6; break;
        case 18: p = &flexe_macrx_b65_mpl_bytes         ; field_num = 9; field_id = 7; break;
        case 19: p = &flexe_macrx_b65_mpl_bytes         ; field_num = 9; field_id = 8; break;
        case 20: p = &flexe_macrx_total_frame           ; field_num = 1; field_id = 0; break;
        case 21: p = &flexe_macrx_good_frame            ; field_num = 1; field_id = 0; break;
        case 22: p = &flexe_macrx_bad_frame             ; field_num = 1; field_id = 0; break;
        case 23: p = &flexe_macrx_fragment_frame        ; field_num = 1; field_id = 0; break;
        case 24: p = &flexe_macrx_jabber_frame          ; field_num = 1; field_id = 0; break;
        case 25: p = &flexe_macrx_oversize_frame        ; field_num = 1; field_id = 0; break;
        case 26: p = &flexe_macrx_unicast_frame         ; field_num = 1; field_id = 0; break;
        case 27: p = &flexe_macrx_multicast_frame       ; field_num = 1; field_id = 0; break;
        case 28: p = &flexe_macrx_broadcast_frame       ; field_num = 1; field_id = 0; break;
        case 29: p = &flexe_macrx_b64_frame             ; field_num = 1; field_id = 0; break;
        case 30: p = &flexe_macrx_b65_mpl_frame         ; field_num = 9; field_id = 0; break;
        case 31: p = &flexe_macrx_b65_mpl_frame         ; field_num = 9; field_id = 1; break;
        case 32: p = &flexe_macrx_b65_mpl_frame         ; field_num = 9; field_id = 2; break;
        case 33: p = &flexe_macrx_b65_mpl_frame         ; field_num = 9; field_id = 3; break;
        case 34: p = &flexe_macrx_b65_mpl_frame         ; field_num = 9; field_id = 4; break;
        case 35: p = &flexe_macrx_b65_mpl_frame         ; field_num = 9; field_id = 5; break;
        case 36: p = &flexe_macrx_b65_mpl_frame         ; field_num = 9; field_id = 6; break;
        case 37: p = &flexe_macrx_b65_mpl_frame         ; field_num = 9; field_id = 7; break;
        case 38: p = &flexe_macrx_b65_mpl_frame         ; field_num = 9; field_id = 8; break;
        default: break;
    }
        
    rv = ram_read_single(chip_id,*p,index*field_num+field_id,parameter);
    
    return rv;
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_diag
*
* DESCRIPTION
*
*     Print subsysmac cnt, status & alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1, -1 is all ch
*     mode: 0-simple diag, 1-all frame diag and all bytes diag
* 
* RETURNS
* 
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS flexe_macrx_diag(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode)
{
#ifndef SV_TEST
    UINT_8  idx = 0;
    UINT_8  idx_start = 0;
    UINT_8  idx_end = 0;
    UINT_32 val[2][2] = {{0}};
    char cntbuf[2][21] = {{0}};
    
    if ((chip_id > DEV_NUM - 1) || ((ch_id > CH_NUM - 1) && (ch_id < 255)) || (mode > 1))
    {
        printf("[%s] invalid parameter! \
                \r\n\t chip_id = %d, \
                \r\n\t ch_id = %d, \
                \r\n\t mode = %d  \r\n", 
                __FUNCTION__,
                chip_id, 
                ch_id,
                mode
              );

        return RET_PARAERR;
    }

    /* Check whether print all ch */
    if (255 == ch_id)
    {
        idx_start = 0;
        idx_end = CH_NUM/2;
    }
    else
    {
        idx_start = ch_id;
        idx_end = ch_id + 1;
    }
    
    for (idx = idx_start; idx < idx_end; idx++)
    {
        /* Print flexe mac rx title */
        diag_print_line();

        diag_print_title("FLEXE MAC RX");
        
        /* Print ch id */
        diag_print_title("CH%d",idx);
        
        if (0 == mode || 1 == mode)
        {
            /* Print flexe mac rx total_frame */
            flexe_macrx_all_ram_get(chip_id,20,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,1,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("TOTAL_BYTES", cntbuf[1], "TOTAL_FRAME", cntbuf[0]);

            /* Print flexe mac rx good_frame */
            flexe_macrx_all_ram_get(chip_id,21,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,2,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("GOOD_BYTES", cntbuf[1], "GOOD_FRAME", cntbuf[0]);

            /* Print flexe mac rx bad_frame */
            flexe_macrx_all_ram_get(chip_id,22,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,3,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("BAD_BYTES", cntbuf[1], "BAD_FRAME", cntbuf[0]);
        }

        if (1 == mode)
        {
            /* Print flexe mac rx fragment_frame */
            flexe_macrx_all_ram_get(chip_id,23,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,4,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("FRAGMENT_BYTES", cntbuf[1], "FRAGMENT_FRAME", cntbuf[0]);

            /* Print flexe mac rx jabber_frame */
            flexe_macrx_all_ram_get(chip_id,24,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,5,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("JABBER_BYTES", cntbuf[1], "JABBER_FRAME", cntbuf[0]);

            /* Print flexe mac rx oversize_frame */
            flexe_macrx_all_ram_get(chip_id,25,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,6,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("OVERSIZE_BYTES", cntbuf[1], "OVERSIZE_FRAME", cntbuf[0]);

            /* Print flexe mac rx unicast_frame */
            flexe_macrx_all_ram_get(chip_id,26,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,7,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("UNICAST_BYTES", cntbuf[1], "UNICAST_FRAME", cntbuf[0]);

            /* Print flexe mac rx multicast_frame */
            flexe_macrx_all_ram_get(chip_id,27,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,8,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("MULTICAST_BYTES", cntbuf[1], "MULTICAST_FRAME", cntbuf[0]);

            /* Print flexe mac rx broadcast_frame */
            flexe_macrx_all_ram_get(chip_id,28,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,9,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("BROADCAST_BYTES", cntbuf[1], "BROADCAST_FRAME", cntbuf[0]);

            /* Print flexe mac rx 64b_frame */
            flexe_macrx_all_ram_get(chip_id,29,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,10,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("64B_BYTES", cntbuf[1], "64B_FRAME", cntbuf[0]);

            /* Print flexe mac rx 65_127b_frame */
            flexe_macrx_all_ram_get(chip_id,30,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,11,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("65_127B_BYTES", cntbuf[1], "65_127B_FRAME", cntbuf[0]);

            /* Print flexe mac rx 128_255b_frame */
            flexe_macrx_all_ram_get(chip_id,31,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,12,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("128_255B_BYTES", cntbuf[1], "128_255B_FRAME", cntbuf[0]);

            /* Print flexe mac rx 256_511b_frame */
            flexe_macrx_all_ram_get(chip_id,32,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,13,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("256_511B_BYTES", cntbuf[1], "256_511B_FRAME", cntbuf[0]);

            /* Print flexe mac rx 512_1023b_frame */
            flexe_macrx_all_ram_get(chip_id,33,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,14,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("512_1023B_BYTES", cntbuf[1], "512_1023B_FRAME", cntbuf[0]);

            /* Print flexe mac rx 1024_1518b_frame */
            flexe_macrx_all_ram_get(chip_id,34,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,15,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("1024_1518B_BYTES", cntbuf[1], "1024_1518B_FRAME", cntbuf[0]);

            /* Print flexe mac rx 1519_1522b_frame */
            flexe_macrx_all_ram_get(chip_id,35,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,16,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("1519_2047B_BYTES", cntbuf[1], "1519_2047B_FRAME", cntbuf[0]);

            /* Print flexe mac rx 1523_1548b_frame */
            flexe_macrx_all_ram_get(chip_id,36,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,17,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("2048_4095B_BYTES", cntbuf[1], "2048_4095B_FRAME", cntbuf[0]);

            /* Print flexe mac rx 1549_2000b_frame */
            flexe_macrx_all_ram_get(chip_id,37,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,18,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("4096_9216B_BYTES", cntbuf[1], "4096_9216B_FRAME", cntbuf[0]);

            /* Print flexe mac rx 2000b_mpl_frame */
            flexe_macrx_all_ram_get(chip_id,38,idx,val[0]);
			flexe_macrx_all_ram_get(chip_id,19,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
			diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("9217B_MPL_BYTES", cntbuf[1], "9217B_MPL_FRAME", cntbuf[0]);
        }
        
        /* Print blank line */
        diag_print_cnt(NULL, NULL, NULL, NULL);

        /* Print flexe mac rx err alm */
        /*flexe_macrx_error_alm_get(chip_id,SOP_WARNING,&val[0][0]);*/
        regp_read(chip_id, FLEXE_MACRX_BASE_ADDR, 0x40, &val[0][0]);
        diag_cnt_val_to_str(&val[0][0],1,1,cntbuf[0],21);
        diag_print_cnt("{EOP,SOP}ALM", cntbuf[0], NULL, NULL);

        /* Print decode title */
        diag_print_line();
        diag_print_title("DECODE");
            
        /* Print ch id */
        diag_print_title("CH%d",idx);
        
        /* Print decode err cnt */
        flexe_macrx_all_ram_get(chip_id,0,idx,val[0]);
        diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
        diag_print_cnt("DECODE_ERR_CODE", cntbuf[0], NULL, NULL);
        
        /* Print blank line */
        diag_print_cnt(NULL, NULL, NULL, NULL);

        /* Print decode err alm */
        flexe_macrx_arm_decode_err_alm_get(chip_id,idx,&val[0][0]);
        diag_cnt_val_to_str(&val[0][0],1,0,cntbuf[0],21);
        diag_print_cnt("DECODE_ERR_ALM", cntbuf[0], NULL, NULL);
        
        /* Print oam rx/oam tx title */
        diag_print_line();
        diag_print_2_title("OAM RX", "OAM TX");
        

    }
    
    /* Print normal tail */
    diag_print_line();
    printf("                           |                                                    /|\\\n");
    printf("                          \\|/           E T H    6 6 B    S W I T C H            |\n");

#endif
    return RET_SUCCESS;
}
