

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "sal.h"
#include "reg_operate.h"
#include "mcmactx.h"
#include "mcmacrx.h"
#include "module_mcmac.h"
#include "flexe_macrx.h"

/******************************************************************************
*
* FUNCTION
*
*     mcmac_init
*
* DESCRIPTION
*
*     mac ch config.
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
RET_STATUS mcmac_init(UINT_8 chip_id)
{
    UINT_8  idx = 0;
        
    /* init mcmac tx/rx work mode/stat enable */
    for (idx = 0; idx < MCMAC_CH_NUM; idx++)
    {
        /* disable mcmac tx stat */
        mcmactx_stat_enable_set(chip_id,idx,0);
        
        /* disable mcmac rx chan stat */
        mcmacrx_stat_enable_set(chip_id,idx,0);
    }
    	
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     mcmac_tx_traffic_cfg
*
* DESCRIPTION
*
*     mac tx ch config.
*
* NOTE
*
*     chip_id: chip number used
*     add_del_sel: 1-add,0-delete
*     ch_id: 0~CH_NUM - 1
*     ts_num: 1~40
*     mode: 0-mac, 1-sar
*     tx_max_pkt: MAX_PKT
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
RET_STATUS mcmac_tx_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_32 tx_max_pkt)
{
    if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > MCMAC_CH_NUM - 1) ||
        (tx_max_pkt > 0x3ffff))
    {
        printf("[%s] invalid parameter!! \
                \r\n\t chip_id = %d, \
                \r\n\t add_del_sel = %d, \
                \r\n\t ch_id = %d, \
                \r\n\t tx_max_pkt = %d\r\n", 
                __FUNCTION__,
                chip_id, 
                add_del_sel, 
                ch_id, 
                tx_max_pkt);

        return RET_PARAERR;
    }

    /* delete ch & config */
    if (0 == add_del_sel)
    {
        /* disable mcmac tx stat */
        mcmactx_stat_enable_set(chip_id,ch_id,0);
    }
    /* add ch & config */
    else
    {
        /* set max pkt */
        /** coverity[ARRAY_VS_SINGLETON:FALSE] */
        mcmactx_max_pkt_ram_set(chip_id,ch_id,&tx_max_pkt);
       
        /* enable mcmac tx ch stat */
        mcmactx_stat_enable_set(chip_id,ch_id,1);
    }

    /* clear mcmac tx ch statistic */
    mcmactx_statistic_clear_chan_set(chip_id,ch_id);
    mcmactx_all_pls_set(chip_id, 1);
    
    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mcmac_rx_traffic_cfg
*
* DESCRIPTION
*
*     mac rx ch config.
*
* NOTE
*
*     chip_id: chip number used
*     add_del_sel: 1-add,0-delete
*     ch_id: 0~CH_NUM - 1
*     mode: 0-mac, 1-sar
*     rx_max_pkt: max_pkt
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
RET_STATUS mcmac_rx_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_32 rx_max_pkt)
{
    if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > MCMAC_CH_NUM - 1) ||
        (rx_max_pkt > 0x3ffff))
    {
        printf("[%s] invalid parameter!! \
                \r\n\t chip_id = %d, \
                \r\n\t add_del_sel = %d, \
                \r\n\t ch_id = %d, \
                \r\n\t rx_max_pkt = %d\r\n", 
                __FUNCTION__,
                chip_id, 
                add_del_sel, 
                ch_id, 
                rx_max_pkt
              );

        return RET_PARAERR;
    }

 
    /* delete ch & config */
    if (0 == add_del_sel)
    {
        /* set mcmac rx chan stat enable */
        mcmacrx_stat_enable_set(chip_id,ch_id,0);
    }
    /* add ch & config */
    else
    {
        /* set mcmac rx max pkt */
        /** coverity[ARRAY_VS_SINGLETON:FALSE] */
        mcmacrx_max_pkt_ram_set(chip_id,ch_id,&rx_max_pkt);

        /* set mcmac rx chan stat enable */
        mcmacrx_stat_enable_set(chip_id,ch_id,1);
    }
    
    /* clear mcmac rx ch statistic */
    mcmacrx_statistic_clear_chan_set(chip_id,ch_id);
    mcmacrx_statistic_clear_pls_set(chip_id);

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_init
*
* DESCRIPTION
*
*     init the flexe macrx
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
void flexe_macrx_init(UINT_8 chip_id)
{
	UINT_8 index = 0;
	UINT_32 parameter = 10000;

	for (index = 0;index < CH_NUM;index++)
	{
        /** coverity[ARRAY_VS_SINGLETON:FALSE] */
		flexe_macrx_max_pkt_ram_set(chip_id,index,&parameter);
		flexe_macrx_stat_enable_set(chip_id,index,1);
	}
}

/******************************************************************************
*
* FUNCTION
*
*     flexe_macrx_traffic_cfg
*
* DESCRIPTION
*
*     flexe macrx ch config.
*
* NOTE
*
*     chip_id: chip number used
*	  add_del: 0:del,1:add
*     ch_id: 0~CH_NUM - 1
*     rx_max_pkt: max_pkt
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
RET_STATUS flexe_macrx_traffic_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 ch_id,UINT_32 max_pkt)
{
	if (chip_id > MAX_DEV || add_del> 1 || ch_id >= CH_NUM)
	{
		printf("[%s]:invalid parameter!!chip_id=%d,add_del=%d,ch_id=%d,max_pkt=%d\r\n",__FUNCTION__,chip_id,add_del,ch_id,max_pkt);

		return RET_PARAERR;
	}

	if (1 == add_del)
	{
        /** coverity[ARRAY_VS_SINGLETON:FALSE] */
		flexe_macrx_max_pkt_ram_set(chip_id,ch_id,&max_pkt);
	
		flexe_macrx_stat_enable_set(chip_id,ch_id,1);
	}
	else
	{
		flexe_macrx_stat_enable_set(chip_id,ch_id,0);
	}
	
	return RET_SUCCESS;
}
/******************************************************************************
*
* FUNCTION
*
*     mcmac_diag
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
*     mode: 0-simple diag, 1-all frame diag, 2-all bytes diag
* 
* RETURNS
* 
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS mcmac_diag(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode)
{
#ifndef SV_TEST
    UINT_8  idx = 0;
    UINT_8  idx_start = 0;
    UINT_8  idx_end = 0;
    UINT_32 val[2][2] = {{0}};
    char cntbuf[2][21] = {{0}};
    
    if ((chip_id > DEV_NUM - 1) || ((ch_id > MCMAC_CH_NUM - 1) && (ch_id < 255)) || (mode > 2))
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
        idx_end = MCMAC_CH_NUM;
    }
    else
    {
        idx_start = ch_id;
        idx_end = ch_id + 1;
    }
    
    /* Print diag diagram head */
    printf("\n                           |              C P B    I N T E R F A C E            /|\\\n");
    printf("                          \\|/                                                    |\n");

    for (idx = idx_start; idx < idx_end; idx++)
    {
        /* Print mcmac tx/rx title */
        diag_print_line();
        diag_print_2_title("SAR RX", "SAR TX");
        
        /* Print ch id */
        diag_print_title("CH%d",idx);
        
        if (0 == mode || 1 == mode)
        {
            /* Print mcmac tx/rx total_frame */
            mcmacrx_all_ram_get(chip_id,20,idx,val[0]);
            mcmactx_all_ram_get(chip_id,24,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("TOTAL_FRAME", cntbuf[1], "TOTAL_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx good_frame */
            mcmacrx_all_ram_get(chip_id,21,idx,val[0]);
            mcmactx_all_ram_get(chip_id,25,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("GOOD_FRAME", cntbuf[1], "GOOD_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx bad_frame */
            mcmacrx_all_ram_get(chip_id,22,idx,val[0]);
            mcmactx_all_ram_get(chip_id,26,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("BAD_FRAME", cntbuf[1], "BAD_FRAME", cntbuf[0]);
        }

        if (1 == mode)
        {
            /* Print mcmac tx/rx fragment_frame */
            mcmacrx_all_ram_get(chip_id,23,idx,val[0]);
            mcmactx_all_ram_get(chip_id,28,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("FRAGMENT_FRAME", cntbuf[1], "FRAGMENT_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx jabber_frame */
            mcmacrx_all_ram_get(chip_id,24,idx,val[0]);
            mcmactx_all_ram_get(chip_id,30,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("JABBER_FRAME", cntbuf[1], "JABBER_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx oversize_frame */
            mcmacrx_all_ram_get(chip_id,25,idx,val[0]);
            mcmactx_all_ram_get(chip_id,31,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("OVERSIZE_FRAME", cntbuf[1], "OVERSIZE_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx unicast_frame */
            mcmacrx_all_ram_get(chip_id,26,idx,val[0]);
            mcmactx_all_ram_get(chip_id,32,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("UNICAST_FRAME", cntbuf[1], "UNICAST_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx multicast_frame */
            mcmacrx_all_ram_get(chip_id,27,idx,val[0]);
            mcmactx_all_ram_get(chip_id,33,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("MULTICAST_FRAME", cntbuf[1], "MULTICAST_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx broadcast_frame */
            mcmacrx_all_ram_get(chip_id,28,idx,val[0]);
            mcmactx_all_ram_get(chip_id,34,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("BROADCAST_FRAME", cntbuf[1], "BROADCAST_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 64b_frame */
            mcmacrx_all_ram_get(chip_id,29,idx,val[0]);
            mcmactx_all_ram_get(chip_id,35,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("64B_FRAME", cntbuf[1], "64B_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 65_127b_frame */
            mcmacrx_all_ram_get(chip_id,30,idx,val[0]);
            mcmactx_all_ram_get(chip_id,36,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("65_127B_FRAME", cntbuf[1], "65_127B_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 128_255b_frame */
            mcmacrx_all_ram_get(chip_id,31,idx,val[0]);
            mcmactx_all_ram_get(chip_id,37,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("128_255B_FRAME", cntbuf[1], "128_255B_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 256_511b_frame */
            mcmacrx_all_ram_get(chip_id,32,idx,val[0]);
            mcmactx_all_ram_get(chip_id,38,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("256_511B_FRAME", cntbuf[1], "256_511B_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 512_1023b_frame */
            mcmacrx_all_ram_get(chip_id,33,idx,val[0]);
            mcmactx_all_ram_get(chip_id,39,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("512_1023B_FRAME", cntbuf[1], "512_1023B_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 1024_1518b_frame */
            mcmacrx_all_ram_get(chip_id,34,idx,val[0]);
            mcmactx_all_ram_get(chip_id,40,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("1024_1518B_FRAME", cntbuf[1], "1024_1518B_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 1519_1522b_frame */
            mcmacrx_all_ram_get(chip_id,35,idx,val[0]);
            mcmactx_all_ram_get(chip_id,41,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("1519_2047B_FRAME", cntbuf[1], "1519_2047B_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 1523_1548b_frame */
            mcmacrx_all_ram_get(chip_id,36,idx,val[0]);
            mcmactx_all_ram_get(chip_id,42,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("2048_4095B_FRAME", cntbuf[1], "2048_4095B_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 1549_2000b_frame */
            mcmacrx_all_ram_get(chip_id,37,idx,val[0]);
            mcmactx_all_ram_get(chip_id,43,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("4096_9216B_FRAME", cntbuf[1], "4096_9216B_FRAME", cntbuf[0]);

            /* Print mcmac tx/rx 2000b_mpl_frame */
            mcmacrx_all_ram_get(chip_id,38,idx,val[0]);
            mcmactx_all_ram_get(chip_id,44,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("9217B_MPL_FRAME", cntbuf[1], "9217B_MPL_FRAME", cntbuf[0]);
        }
        
        if (2 == mode)
        {
            /* Print mcmac tx/rx total_bytes */
            mcmacrx_all_ram_get(chip_id,1,idx,val[0]);
            mcmactx_all_ram_get(chip_id,3,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("TOTAL_BYTES", cntbuf[1], "TOTAL_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx good_bytes */
            mcmacrx_all_ram_get(chip_id,2,idx,val[0]);
            mcmactx_all_ram_get(chip_id,4,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("GOOD_BYTES", cntbuf[1], "GOOD_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx bad_bytes */
            mcmacrx_all_ram_get(chip_id,3,idx,val[0]);
            mcmactx_all_ram_get(chip_id,5,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("BAD_BYTES", cntbuf[1], "BAD_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx fragment_bytes */
            mcmacrx_all_ram_get(chip_id,4,idx,val[0]);
            mcmactx_all_ram_get(chip_id,7,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("FRAGMENT_BYTES", cntbuf[1], "SAR_FRAGMENT_BYTES/MAC_ERR_CNT", cntbuf[0]);

            /* Print mcmac tx/rx jabber_bytes */
            mcmacrx_all_ram_get(chip_id,5,idx,val[0]);
            mcmactx_all_ram_get(chip_id,9,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("JABBER_BYTES", cntbuf[1], "JABBER_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx oversize_bytes */
            mcmacrx_all_ram_get(chip_id,6,idx,val[0]);
            mcmactx_all_ram_get(chip_id,10,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("OVERSIZE_BYTES", cntbuf[1], "OVERSIZE_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx unicast_bytes */
            mcmacrx_all_ram_get(chip_id,7,idx,val[0]);
            mcmactx_all_ram_get(chip_id,11,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("UNICAST_BYTES", cntbuf[1], "UNICAST_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx multicast_bytes */
            mcmacrx_all_ram_get(chip_id,8,idx,val[0]);
            mcmactx_all_ram_get(chip_id,12,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("MULTICAST_BYTES", cntbuf[1], "MULTICAST_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx broadcast_bytes */
            mcmacrx_all_ram_get(chip_id,9,idx,val[0]);
            mcmactx_all_ram_get(chip_id,13,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("BROADCAST_BYTES", cntbuf[1], "BROADCAST_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 64b_bytes */
            mcmacrx_all_ram_get(chip_id,10,idx,val[0]);
            mcmactx_all_ram_get(chip_id,14,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("64B_BYTES", cntbuf[1], "64B_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 65_127b_bytes */
            mcmacrx_all_ram_get(chip_id,11,idx,val[0]);
            mcmactx_all_ram_get(chip_id,15,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("65_127B_BYTES", cntbuf[1], "65_127B_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 128_255b_bytes */
            mcmacrx_all_ram_get(chip_id,12,idx,val[0]);
            mcmactx_all_ram_get(chip_id,16,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("128_255B_BYTES", cntbuf[1], "128_255B_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 256_511b_bytes */
            mcmacrx_all_ram_get(chip_id,13,idx,val[0]);
            mcmactx_all_ram_get(chip_id,17,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("256_511B_BYTES", cntbuf[1], "256_511B_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 512_1023b_bytes */
            mcmacrx_all_ram_get(chip_id,14,idx,val[0]);
            mcmactx_all_ram_get(chip_id,18,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("512_1023B_BYTES", cntbuf[1], "512_1023B_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 1024_1518b_bytes */
            mcmacrx_all_ram_get(chip_id,15,idx,val[0]);
            mcmactx_all_ram_get(chip_id,19,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("1024_1518B_BYTES", cntbuf[1], "1024_1518B_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 1519_1522b_bytes */
            mcmacrx_all_ram_get(chip_id,16,idx,val[0]);
            mcmactx_all_ram_get(chip_id,20,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("1519_2047B_BYTES", cntbuf[1], "1519_2047B_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 1523_1548b_bytes */
            mcmacrx_all_ram_get(chip_id,17,idx,val[0]);
            mcmactx_all_ram_get(chip_id,21,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("2048_4095B_BYTES", cntbuf[1], "2048_4095B_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 1549_2000b_bytes */
            mcmacrx_all_ram_get(chip_id,18,idx,val[0]);
            mcmactx_all_ram_get(chip_id,22,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("4096_9216B_BYTES", cntbuf[1], "4096_9216B_BYTES", cntbuf[0]);

            /* Print mcmac tx/rx 2000b_mpl_bytes */
            mcmacrx_all_ram_get(chip_id,19,idx,val[0]);
            mcmactx_all_ram_get(chip_id,23,idx,val[1]);
            diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
            diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
            diag_print_cnt("9217B_MPL_BYTES", cntbuf[1], "9217B_MPL_BYTES", cntbuf[0]);
        }
        
        
        
        /* Print blank line */
        diag_print_cnt(NULL, NULL, NULL, NULL);

        /* Print mcmac tx/rx err alm */
        mcmacrx_error_alm_get(chip_id,&val[0][0]);
        mcmactx_bussw_underflow_alm_get(chip_id,idx,&val[1][0]);
        diag_cnt_val_to_str(&val[0][0],1,1,cntbuf[0],21);
        diag_cnt_val_to_str(&val[1][0],1,0,cntbuf[1],21);
        diag_print_cnt("UNDERFLOW_ALM", cntbuf[1], "{SW_RAM_FULL,EOP,SOP}ALM", cntbuf[0]);

        /* Print encode/decode title */
        diag_print_line();
        diag_print_2_title("ENCODE", "DECODE");
            
        /* Print ch id */
        diag_print_title("CH%d",idx);
        
        /* Print encode/decode err cnt */
	
		mcmacrx_all_ram_get(chip_id,0,idx,val[0]);
        mcmactx_all_ram_get(chip_id,1,idx,val[1]);
        diag_cnt_val_to_str(val[0],2,0,cntbuf[0],21);
        diag_cnt_val_to_str(val[1],2,0,cntbuf[1],21);
        diag_print_cnt("TX_DECODE_ERR_CODE", cntbuf[1], "RX_DECODE_ERR_CODE", cntbuf[0]);
		
        
        /* Print blank line */
        diag_print_cnt(NULL, NULL, NULL, NULL);

        /* Print encode/decode err alm */
        mcmacrx_arm_decode_err_alm_get(chip_id,idx,&val[0][0]);
        mcmactx_decode_err_alm_get(chip_id,idx,&val[1][0]);
        diag_cnt_val_to_str(&val[0][0],1,0,cntbuf[0],21);
        diag_cnt_val_to_str(&val[1][0],1,0,cntbuf[1],21);
        diag_print_cnt("DECODE_ERR_ALM", cntbuf[1], "DECODE_ERR_ALM", cntbuf[0]);
 
        /* sar alm*/
        
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

/******************************************************************************
*
* FUNCTION
*
*     mcmac_diag_clear
*
* DESCRIPTION
*
*     Clear mcmac cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1, -1 is clear all ch cnt
* 
* RETURNS
* 
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS mcmac_diag_clear(UINT_8 chip_id, UINT_8 ch_id)
{
    if ((chip_id > DEV_NUM - 1) || ((ch_id > MCMAC_CH_NUM - 1) && (ch_id < 255)))
    {
        printf("[%s] invalid parameter! \
                \r\n\t chip_id = %d, \
                \r\n\t ch_id = %d\r\n", 
                __FUNCTION__,
                chip_id, 
                ch_id
              );

        return RET_PARAERR;
    }

    /* Check whether clear all ch cnt */
    if (255 == ch_id)
    {
        /* clear mcmac rx ch cnt */
        mcmacrx_statistic_all_clr_pls_set(chip_id);
        printf("Mcmac rx all ch cnt clear. \r\n");

        /* clear mcmac tx ch cnt */
        mcmactx_all_pls_set(chip_id, 0);
        printf("Mcmac tx all ch cnt clear. \r\n");
    }
    else
    {
        /* clear mcmac rx ch cnt */
        mcmacrx_statistic_clear_chan_set(chip_id,ch_id);
        mcmacrx_statistic_clear_pls_set(chip_id);
        printf("Mcmac rx ch=%d cnt clear. \r\n", ch_id);

        /* clear mcmac tx ch cnt */
        mcmactx_statistic_clear_chan_set(chip_id,ch_id);
        mcmactx_all_pls_set(chip_id,1);
        printf("Mcmac tx ch=%d cnt clear. \r\n", ch_id);
    }

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*      flexe_macrx_diag_clear
*
* DESCRIPTION
*
*     Clear mcmac cnt.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1, -1 is clear all ch cnt
* 
* RETURNS
* 
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS flexe_macrx_diag_clear(UINT_8 chip_id, UINT_8 ch_id)
{
    if ((chip_id > DEV_NUM - 1) || ((ch_id > MCMAC_CH_NUM - 1) && (ch_id < 255)))
    {
        printf("[%s] invalid parameter! \
                \r\n\t chip_id = %d, \
                \r\n\t ch_id = %d\r\n", 
                __FUNCTION__,
                chip_id, 
                ch_id
              );

        return RET_PARAERR;
    }

    /* Check whether clear all ch cnt */
    if (255 == ch_id)
    {
        /* clear mcmac rx ch cnt */
        flexe_macrx_statistic_all_clr_pls_set(chip_id);
        printf("Mcmac rx all ch cnt clear. \r\n");
    }
    else
    {
        /* clear mcmac rx ch cnt */
        flexe_macrx_statistic_clear_chan_set(chip_id,ch_id);
        flexe_macrx_statistic_clear_pls_set(chip_id);
        printf("Mcmac rx ch=%d cnt clear. \r\n", ch_id);
    }

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mcmac debug function.
*
* DESCRIPTION
*
*     RESERVED
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     RESERVED
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
*	<author>		<date>			<CR_ID>		 <DESCRIPTION>
*  	kejian  	  2018-04-03		 1.0        	initial
*
******************************************************************************/
RET_STATUS mcmac_debug(void)
{
	printf("+----------------------mcmac initialized function---------------------------------------------------+\r\n");
	printf("mcmac_init(UINT_8 chip_id)\r\n");
	printf("\r\n");

	printf("parameter description:\r\n");
	printf("chip_id: chip number used.\r\n");
	printf("+---------------------------------------------------------------------------------------------------+\r\n");
	printf("\r\n\r\n");

	printf("+----------------------mcmac_tx_traffic_cfg function------------------------------------------------+\r\n");
	printf("mcmac_tx_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_16 ts_num,UINT_32 mode,UINT_32 tx_max_pkt)\r\n");
	printf("\r\n");

	printf("parameter description:\r\n");
	printf("chip_id: chip number used.\r\n");
	printf("add_del_sel: delete/add operation select,0---delete ts,1---add.\r\n");
	printf("ch_id: channel_id, range is 0---MCMAC_CH_NUM-1.\r\n");
	printf("ts_num:number of ts,if add_del_sel was 1,indicate the number of ts will be configured for the channel.\r\n");
	printf("mode: 0-mac, 1-sar.\r\n");
	printf("tx_max_pkt: max_pkt.\r\n");
	printf("+----------------------------------------------------------------------------------- ---------------+\r\n");
	printf("\r\n\r\n");

	printf("+----------------------mcmac_rx_traffic_cfg function------------------------------------------------+\r\n");
	printf("mcmac_rx_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_32 mode,UINT_32 rx_max_pkt)\r\n");
	printf("\r\n");

	printf("parameter description:\r\n");
	printf("chip_id: chip number used.\r\n");
	printf("add_del_sel: delete/add operation select,0---delete,1---add.\r\n");
	printf("ch_id: channel_id, range is 0---MCMAC_CH_NUM-1.\r\n");
	printf("mode: 0-mac, 1-sar.\r\n");
	printf("rx_max_pkt: max_pkt.\r\n");
	printf("+---------------------------------------------------------------------------------------------------+\r\n");
	printf("\r\n\r\n");

	printf("+----------------------mcmac_adp_sch_cfg_debug function---------------------------------------------+\r\n");
	printf("mcmac_adp_sch_cfg_debug(UINT_8 chip_id,UINT_8 add_del,UINT_8 ch_id,UINT_32 ts_mask_l,UINT_32 ts_mask_h)\r\n");
	printf("\r\n");

	printf("parameter description:\r\n");
	printf("chip_id: chip number used.\r\n");
	printf("add_del: 0---delete,1---add.\r\n");
	printf("ch_id: channel_id, range is 0---MCMAC_CH_NUM-1.\r\n");
	printf("ts_mask_l: .\r\n");
	printf("ts_mask_h: .\r\n");
	printf("+---------------------------------------------------------------------------------------------------+\r\n");
	printf("\r\n\r\n");

	printf("+----------------------mcmac_diag function----------------------------------------------------- ----+\r\n");
	printf("mcmac_diag(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode)\r\n");
	printf("\r\n");

	printf("parameter description:\r\n");
	printf("chip_id: chip number used.\r\n");
	printf("ch_id: channel_id, 0---MCMAC_CH_NUM-1.\r\n");
	printf("mode: 0--simple diag, 1--all frame diag, 2--all bytes diag.\r\n");
	printf("+---------------------------------------------------------------------------------------------------+\r\n");
	printf("\r\n\r\n");

	printf("+----------------------mcmac_diag_clear function----------------------------------------------------+\r\n");
	printf("mcmac_diag_clear(UINT_8 chip_id, UINT_8 ch_id)\r\n");
	printf("\r\n");

	printf("parameter description:\r\n");
	printf("chip_id: chip number used.\r\n");
	printf("ch_id: channel_id, 0---MCMAC_CH_NUM-1.\r\n");
	printf("+---------------------------------------------------------------------------------------------------+\r\n");
	printf("\r\n\r\n");

	printf("+----------------------mcmac_info dump function----------------------------------------------------------+\r\n");
	printf("mcmac_dump(UINT_8 chip_id)\r\n");
	printf("\r\n");

	printf("parameter description:\r\n");
	printf("chip_id: chip number used.\r\n");
	printf("+---------------------------------------------------------------------------------------------------+\r\n");
	printf("\r\n\r\n");

	return RET_SUCCESS;
}

