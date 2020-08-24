

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "sal.h"
#include "reg_operate.h"
#include "mac_rateadpt.h"
#include "rateadpt.h"
#include "module_rateadpt.h"

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
RATEADP_INFO_T g_rateadp_info[DEV_NUM];
MAC_RATEADP_INFO_T g_mac_rateadp_info[DEV_NUM];
UINT_8 g_rateadapt_ts_of_channel[DEV_NUM][RATEADP_CH_NUM]={{0, 0}};
UINT_8 g_mac_rateadapt_ts_of_channel[DEV_NUM][RATEADP_CH_NUM]={{0, 0}};
extern UINT_32 mac_rateadpt_fifo_level_old[80];
extern UINT_32 rateadpt_fifo_level_old[80];
extern UINT_32 mac_rateadpt_fifo_level[80];
extern UINT_32 rateadpt_fifo_level[80];


/******************************************************************************
*
* FUNCTION
*
*     rateadp_init
*
* DESCRIPTION
*
*     rateadp init.
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
RET_STATUS rateadp_init(UINT_8 chip_id)
{
	UINT_8  idx = 0;
    
    for(idx = 0;idx < RATEADP_BLOCK_NUM;idx++)
	{
		rateadp_block_num_set(chip_id,idx,1);
		rateadp_map_reg_set(chip_id,idx,MAP_SEQ,0xf);
		rateadp_map_reg_set(chip_id,idx,MAP_CHID,0x7f);
		rateadp_map_reg_set(chip_id,idx,RATEADP_MAP_EN,0);
		adj_cfg_set(chip_id,I_CFG_FINISH,idx,1);
		g_rateadp_info[chip_id].rateadp_blk_index[idx] = 0xf;
		g_rateadp_info[chip_id].rateadp_blk_ch[idx] = 0x7f;
		g_rateadp_info[chip_id].rateadp_blk_flag[idx] = false;
		g_rateadapt_ts_of_channel[chip_id][idx] = 0;
	}

	rateadp_ind_sel_set(chip_id,1);

	g_rateadp_info[chip_id].rateadp_idle_num = RATEADP_BLOCK_NUM;
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     rateadp_traffic_cfg
*
* DESCRIPTION
*
*     rateadp_traffic_cfg.
*
* NOTE
*
*     chip_id: chip number used
*	  add_del_sel:1:add;0:delete
*	  ch_id:0~(CH_NUM-1)
*	  ts_num:the number of ts 
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
RET_STATUS rateadp_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_8 ts_num)
{

	if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > RATEADP_CH_NUM - 1) ||
        (((ts_num < 1)&&(1 == add_del_sel)) || ts_num > RATEADP_TS_NUM))
	{
		 printf("[%s] invalid parameter!! \
	            \r\n\t chip_id = %d, \
	            \r\n\t add_del_sel = %d, \
	            \r\n\t ch_id = %d, \
	            \r\n\t ts_num = %d \r\n", 
	            __FUNCTION__,
	            chip_id, 
	            add_del_sel, 
	            ch_id, 
	            ts_num);

		return RET_PARAERR;
	}

	if (0 == add_del_sel)
	{
		ts_num = g_rateadapt_ts_of_channel[chip_id][ch_id];
		if (0 == ts_num)
		{
			printf("[%s] ch_id=%d has been delete\r\n",__FUNCTION__,ch_id);

			return RET_SUCCESS;
		}
	}

	if((1 == add_del_sel)&&(ts_num == g_rateadapt_ts_of_channel[chip_id][ch_id]))
	{
		printf("[%s] ch_id=%d ts number don't changed\r\n",__FUNCTION__,ch_id);

		return RET_SUCCESS;
	}

	if (0 == add_del_sel)
	{
		rateadp_traffic_cfg_in_chip(chip_id,add_del_sel, ch_id, ts_num);
		
		g_rateadapt_ts_of_channel[chip_id][ch_id] = 0;
	}
	else
	{
		if ((1 == add_del_sel)
			&&(ts_num != g_rateadapt_ts_of_channel[chip_id][ch_id])
			&&(0!=g_rateadapt_ts_of_channel[chip_id][ch_id]))
		{
			rateadp_traffic_cfg_in_chip(chip_id,0, ch_id, g_rateadapt_ts_of_channel[chip_id][ch_id]);
		}
			
		rateadp_traffic_cfg_in_chip(chip_id,add_del_sel, ch_id, ts_num);

		g_rateadapt_ts_of_channel[chip_id][ch_id] = ts_num;
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     rateadp_traffic_cfg
*
* DESCRIPTION
*
*     rateadp_traffic_cfg.
*
* NOTE
*
*     chip_id: chip number used
*	  add_del_sel:1:add;0:delete
*	  ch_id:0~(CH_NUM-1)
*	  ts_num:the number of ts 
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
RET_STATUS rateadp_traffic_cfg_in_chip(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_8 ts_num)
{
        UINT_8 ts_cnt = 0;
        UINT_8 idx = 0;
        UINT_8 blk_cnt = 0;
        UINT_32 rate_ind = 0;

	if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > RATEADP_CH_NUM - 1) ||
        (ts_num < 1 || ts_num > RATEADP_TS_NUM))
	{
		 printf("[%s] invalid parameter!! \
	            \r\n\t chip_id = %d, \
	            \r\n\t add_del_sel = %d, \
	            \r\n\t ch_id = %d, \
	            \r\n\t ts_num = %d \r\n", 
	            __FUNCTION__,
	            chip_id, 
	            add_del_sel, 
	            ch_id, 
	            ts_num);

		return RET_PARAERR;
	}
	
	rateadp_ind_sel_get(chip_id,&rate_ind);
	if (0 == rate_ind)
	{
		rateadp_fifo_level_cfg_set(chip_id,ch_id,rateadpt_fifo_level_old[ts_num-1]);
	}
	else
	{
		rateadp_fifo_level_cfg_set(chip_id,ch_id,rateadpt_fifo_level[ts_num-1]);
	}
	
	
	ts_cnt = (ts_num < RATEADP_TS_MAX) ? ts_num : RATEADP_TS_MAX;
	
	if (1 == add_del_sel)
	{
		blk_cnt = 0;
		if (ts_num > g_rateadp_info[chip_id].rateadp_idle_num)
		{
			printf("[%s] there is not enough ts in rateadp!!idle_num = %d\r\n",__FUNCTION__,g_rateadp_info[chip_id].rateadp_idle_num);

			return RET_PARAERR;
		}
		
		rateadp_chan_en_set(chip_id,ch_id,0);		
		rateadp_block_num_set(chip_id,ch_id,ts_cnt);

		for (idx = 0;idx < RATEADP_BLOCK_NUM;idx++)
		{
			if (false == g_rateadp_info[chip_id].rateadp_blk_flag[idx])
			{
				rateadp_map_reg_set(chip_id,idx,MAP_SEQ,blk_cnt);
				rateadp_map_reg_set(chip_id,idx,MAP_CHID,ch_id);
				rateadp_map_reg_set(chip_id,idx,RATEADP_MAP_EN,1);

				g_rateadp_info[chip_id].rateadp_blk_ch[idx] = ch_id;
				g_rateadp_info[chip_id].rateadp_blk_index[idx] = blk_cnt;
				g_rateadp_info[chip_id].rateadp_blk_flag[idx] = true;
				g_rateadp_info[chip_id].rateadp_idle_num--;
				blk_cnt++;
			}

			if (blk_cnt >= ts_cnt)
			{
				break;
			}
		}
		
		rateadp_chan_en_set(chip_id,ch_id,1);
		
	}
	else
	{
		blk_cnt = ts_cnt;
		
		rateadp_chan_en_set(chip_id,ch_id,0);		
		rateadp_block_num_set(chip_id,ch_id,1);
		
		for (idx = 0;idx < RATEADP_BLOCK_NUM;idx++)
		{
			if ((ch_id == g_rateadp_info[chip_id].rateadp_blk_ch[idx])&&(true == g_rateadp_info[chip_id].rateadp_blk_flag[idx]))
			{
				rateadp_map_reg_set(chip_id,idx,MAP_SEQ,0xf);
				rateadp_map_reg_set(chip_id,idx,MAP_CHID,0x7f);
				rateadp_map_reg_set(chip_id,idx,RATEADP_MAP_EN,0);

				g_rateadp_info[chip_id].rateadp_blk_ch[idx] = 0x7f;
				g_rateadp_info[chip_id].rateadp_blk_index[idx] = 0xf;
				g_rateadp_info[chip_id].rateadp_blk_flag[idx] = false;
				g_rateadp_info[chip_id].rateadp_idle_num++;
				blk_cnt--;
			}

			if (0 == blk_cnt)
			{
				break;
			}
		}
				
		if (0 != blk_cnt)
		{
			printf("[%s] there is an error!!blk_cnt =%d\r\n",__FUNCTION__,blk_cnt);

			return RET_PARAERR;
		}
		
	}
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mac_rateadp_init
*
* DESCRIPTION
*
*     rateadp init.
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
RET_STATUS mac_rateadp_init(UINT_8 chip_id)
{
	UINT_8  idx = 0;
    
    for(idx = 0;idx < RATEADP_BLOCK_NUM;idx++)
	{
		mac_rateadp_block_num_set(chip_id,idx,1);
		mac_rateadp_map_reg_set(chip_id,idx,MAC_MAP_SEQ,0xf);
		mac_rateadp_map_reg_set(chip_id,idx,MAC_MAP_CHID,0x7f);
		mac_rateadp_map_reg_set(chip_id,idx,MAC_RATEADP_MAP_EN,0);
		mac_adj_cfg_set(chip_id,I_CFG_FINISH,idx,1);
		g_mac_rateadp_info[chip_id].mac_rateadp_blk_index[idx] = 0xf;
		g_mac_rateadp_info[chip_id].mac_rateadp_blk_ch[idx] = 0x7f;
		g_mac_rateadp_info[chip_id].mac_rateadp_blk_flag[idx] = false;
		g_mac_rateadapt_ts_of_channel[chip_id][idx] = 0;
	}
	
	mac_rateadp_ind_sel_set(chip_id,1);

	g_mac_rateadp_info[chip_id].mac_rateadp_idle_num = RATEADP_BLOCK_NUM;
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     mac_rateadp_traffic_cfg
*
* DESCRIPTION
*
*     mac_rateadp_traffic_cfg.
*
* NOTE
*
*     chip_id: chip number used
*	  add_del_sel:1:add;0:delete
*	  ch_id:0~(CH_NUM-1)
*	  ts_num:the number of ts 
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
RET_STATUS mac_rateadp_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_8 ts_num)
{

        UINT_8 ts_cnt = 0;
        UINT_8 idx = 0;
        UINT_8 blk_cnt = 0;
        UINT_32 rate_ind = 0;

	if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > RATEADP_CH_NUM - 1) ||
        (((ts_num < 1)&&(1 == add_del_sel)) || ts_num > RATEADP_TS_NUM))
	{
		 printf("[%s] invalid parameter!! \
	            \r\n\t chip_id = %d, \
	            \r\n\t add_del_sel = %d, \
	            \r\n\t ch_id = %d, \
	            \r\n\t ts_num = %d \r\n", 
	            __FUNCTION__,
	            chip_id, 
	            add_del_sel, 
	            ch_id, 
	            ts_num);

		return RET_PARAERR;
	}

	
	mac_rateadp_ind_sel_get(chip_id,&rate_ind);

	if (0 == add_del_sel)
	{
		ts_num = g_mac_rateadapt_ts_of_channel[chip_id][ch_id];
	}

	if (0 == rate_ind)
	{
		mac_rateadpt_fifo_level_cfg_set(chip_id,ch_id,mac_rateadpt_fifo_level_old[ts_num-1]);
	}
	else
	{
		mac_rateadpt_fifo_level_cfg_set(chip_id,ch_id,mac_rateadpt_fifo_level[ts_num-1]);
	}
	printf("[%s]%d ch_id=%d,ts_num =%d\r\n",__FUNCTION__,__LINE__,ch_id,ts_num);
	
	ts_cnt = (ts_num < RATEADP_TS_MAX) ? ts_num : RATEADP_TS_MAX;
	
	if (1 == add_del_sel)
	{
		blk_cnt = 0;
		if (ts_num > g_mac_rateadp_info[chip_id].mac_rateadp_idle_num)
		{
			printf("[%s] there is not enough ts in rateadp!!idle_num = %d\r\n",__FUNCTION__,g_mac_rateadp_info[chip_id].mac_rateadp_idle_num);

			return RET_PARAERR;
		}
		
		mac_rateadp_chan_en_set(chip_id,ch_id,0);						
		mac_rateadp_block_num_set(chip_id,ch_id,ts_cnt);

		for (idx = 0;idx < RATEADP_BLOCK_NUM;idx++)
		{
			if (false == g_mac_rateadp_info[chip_id].mac_rateadp_blk_flag[idx])
			{
				mac_rateadp_map_reg_set(chip_id,idx,MAC_MAP_SEQ,blk_cnt);
				mac_rateadp_map_reg_set(chip_id,idx,MAC_MAP_CHID,ch_id);
				mac_rateadp_map_reg_set(chip_id,idx,MAC_RATEADP_MAP_EN,1);

				g_mac_rateadp_info[chip_id].mac_rateadp_blk_ch[idx] = ch_id;
				g_mac_rateadp_info[chip_id].mac_rateadp_blk_index[idx] = blk_cnt;
				g_mac_rateadp_info[chip_id].mac_rateadp_blk_flag[idx] = true;
				g_mac_rateadp_info[chip_id].mac_rateadp_idle_num--;
				blk_cnt++;
			}

			if (blk_cnt >= ts_cnt)
			{
				break;
			}
		}
		
		mac_rateadp_chan_en_set(chip_id,ch_id,1);
		g_mac_rateadapt_ts_of_channel[chip_id][ch_id] = ts_num;
	}
	else
	{
		blk_cnt = ts_cnt;
		
		mac_rateadp_chan_en_set(chip_id,ch_id,0);						
		mac_rateadp_block_num_set(chip_id,ch_id,1);
		
		for (idx = 0;idx < RATEADP_BLOCK_NUM;idx++)
		{
			if ((ch_id == g_mac_rateadp_info[chip_id].mac_rateadp_blk_ch[idx])&&(true == g_mac_rateadp_info[chip_id].mac_rateadp_blk_flag[idx]))
			{
				mac_rateadp_map_reg_set(chip_id,idx,MAC_MAP_SEQ,0xf);
				mac_rateadp_map_reg_set(chip_id,idx,MAC_MAP_CHID,0x7f);
				mac_rateadp_map_reg_set(chip_id,idx,MAC_RATEADP_MAP_EN,0);

				g_mac_rateadp_info[chip_id].mac_rateadp_blk_ch[idx] = 0x7f;
				g_mac_rateadp_info[chip_id].mac_rateadp_blk_index[idx] = 0xf;
				g_mac_rateadp_info[chip_id].mac_rateadp_blk_flag[idx] = false;
				g_mac_rateadp_info[chip_id].mac_rateadp_idle_num++;
				blk_cnt--;
			}

			if (0 == blk_cnt)
			{
				break;
			}
		}
		
		if (0 != blk_cnt)
		{
			printf("[%s] there is an error!!blk_cnt =%d\r\n",__FUNCTION__,blk_cnt);

			return RET_PARAERR;
		}
		g_mac_rateadapt_ts_of_channel[chip_id][ch_id] = 0;
	}
	
	return RET_SUCCESS;
}


