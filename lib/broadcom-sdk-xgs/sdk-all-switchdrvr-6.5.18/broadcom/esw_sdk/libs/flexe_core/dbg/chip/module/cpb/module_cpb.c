

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "sal.h"
#include "reg_operate.h"
#include "cpb_egress.h"
#include "module_cpb.h"
#include "mcmactx.h"

/* define struct array */
CPB_EGR_INFO_T g_cpb_egr_info[DEV_NUM];


/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_ch_depth_para_set
*
* DESCRIPTION
*
*     Cpb egress ch depth param cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch: channel id, 0~CH_NUM - 1
*     ts_num: channel's ts number
* 
* RETURNS
* 
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS cpb_egress_ch_depth_para_set(UINT_8 chip_id,UINT_8 ch,UINT_16 ts_num)
{
    UINT_32 full_depth_cfg = 0;
    UINT_32 bp_level_cfg = 0;
    UINT_32 rd_level_cfg = 0;
    UINT_32 high_depth_cfg = 0;
    UINT_32 low_depth_cfg = 0;
    UINT_32 bp_low_level_cfg = 0;
    
    /* calc ch depth param */
    full_depth_cfg = 128 * ts_num;
    bp_level_cfg   = CPB_EGR_BP_LEVEL_OFFSET * ts_num;
    bp_low_level_cfg = CPB_EGR_BP_LEVEL_OFFSET * ts_num - 4;
    rd_level_cfg   = CPB_EGR_RD_LEVEL_BASE * ts_num;
    high_depth_cfg = CPB_EGR_HIGH_DEPTH_BASE * ts_num;
    low_depth_cfg  = CPB_EGR_LOW_DEPTH_BASE * ts_num;
    
    /* set cpb egress ch depth param value */
    /** coverity[callee_ptr_arith] */
    cpb_egress_full_depth_egress_set(chip_id,ch,&full_depth_cfg);
    /** coverity[callee_ptr_arith] */
    cpb_egress_bp_level_egress_set(chip_id,ch,&bp_level_cfg);
    /** coverity[callee_ptr_arith] */
    cpb_egress_bp_low_egress_set(chip_id,ch,&bp_low_level_cfg);
    /** coverity[callee_ptr_arith] */
    cpb_egress_rd_level_egress_set(chip_id,ch,&rd_level_cfg);
    /** coverity[callee_ptr_arith] */
    cpb_egress_high_depth_eg_set(chip_id,ch,&high_depth_cfg);
    /** coverity[callee_ptr_arith] */
    cpb_egress_low_depth_eg_set(chip_id,ch,&low_depth_cfg);

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_ch_depth_para_set
*
* DESCRIPTION
*
*     Cpb egress ch depth param cfg.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch: channel id, 0~CH_NUM - 1
*     ts_num: channel's ts number
*      mode:1~10:1~10us(mode>10 mode=10) 
* 
* RETURNS
* 
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS cpb_egress_ch_depth_para_set_for_performance(UINT_8 chip_id,UINT_8 ch,UINT_16 ts_num,UINT_8 mode)
{
    UINT_32 rd_level_cfg = 0;
    UINT_32 high_depth_cfg = 0;
    UINT_32 low_depth_cfg = 0;
	UINT_32 full_depth_cfg = 0;

    if(mode > 10)
    {
        mode = 10;
    }
    /* calc ch depth param */
    rd_level_cfg   = 5.5 * mode * ts_num + (ts_num > 8?8:ts_num);
    if((mode*ts_num) % 2 != 0)
    {
        rd_level_cfg ++;
    }

    high_depth_cfg = rd_level_cfg + 5*ts_num;
    low_depth_cfg  = rd_level_cfg - 5*ts_num;
    full_depth_cfg = 128 * ts_num;
	
    /* set cpb egress ch depth param value */
    /** coverity[callee_ptr_arith] */
    cpb_egress_rd_level_egress_set(chip_id,ch,&rd_level_cfg);
    /** coverity[callee_ptr_arith] */
    cpb_egress_high_depth_eg_set(chip_id,ch,&high_depth_cfg);
    /** coverity[callee_ptr_arith] */
    cpb_egress_low_depth_eg_set(chip_id,ch,&low_depth_cfg);
    /** coverity[callee_ptr_arith] */
	cpb_egress_full_depth_egress_set(chip_id,ch,&full_depth_cfg);

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_init
*
* DESCRIPTION
*
*     Initial cpb egress regs, rams and struct array.
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
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS cpb_egress_init(UINT_8 chip_id)
{
    UINT_8  ch_idx = 0;
    UINT_16 blk_idx = 0;
    UINT_32 parameter = 0;
    
    if (chip_id > DEV_NUM - 1)
    {
        printf("[%s] invalid chip_id = %d\r\n", __FUNCTION__, chip_id);

        return RET_PARAERR;
    }

    /* check chip whether initialized */
    if (true == g_cpb_egr_info[chip_id].init_done)
    {
        printf("[%s] Because of reinit, all regs & rams (chip_id=%d) will be set to default value. \r\n", 
                __FUNCTION__,
                chip_id
              );
        
        /* in order to support reinit, don't return */
    }

    /* init cur block struct array and ch enable*/
    for (ch_idx = 0; ch_idx < CPB_EGR_CH_NUM; ch_idx++)
    {
        /* disable cpb egress channel */
        cpb_egress_mon_en_eg_set(chip_id,ch_idx,0);

        /* set cpb egress ch start blk to default value */
        parameter = 0x7f;
        cpb_egress_star_blk_eg_set(chip_id,ch_idx,&parameter);

        /* init cur block */
        g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_idx].ch = 0x7f;
        g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_idx].index = 0x7f;
        g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_idx].idle = true;

        /* set cpb egress channel depth para to default value */
        cpb_egress_ch_depth_para_set(chip_id,ch_idx,1);
        
        /* set cpb egress sar ind to default value */
        /*cpb_egress_sar_ind_eg_set(chip_id,ch_idx,0);*/
        
        /* set mac_rdy_depth_eg ram to default value */
        /*parameter = 0xa0;*/
       /* cpb_egress_mac_rdy_depth_eg_set(chip_id,ch_idx,&parameter);*/
    }

    /* initial next block struct array */
    for (blk_idx = 0; blk_idx < CPB_EGR_BLK_NUM; blk_idx++)
    {
        /* set cpb egress ch_nextblkram to default value */
        parameter = 0x407f;
        /** coverity[callee_ptr_arith] */
        cpb_egress_next_blk_eg_set(chip_id,blk_idx,&parameter);
        
        /* initial next block struct array */
        g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].ch = 0x7f;
        g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].index = 0x7f;
        g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].idle = true;

    }

    /* initial next block idle num */
    g_cpb_egr_info[chip_id].next_blk_idle_num = CPB_EGR_BLK_NUM;
    
    /* clr cpb egress cnt */
    cpb_egress_clr_eg_set(chip_id,1);
    cpb_egress_clr_eg_set(chip_id,0);

    /* set init done */
    g_cpb_egr_info[chip_id].init_done = true;

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_next_block_get
*
* DESCRIPTION
*
*     Get one ts's next block.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     start_index: search start index
*     ts_next_block: ts's next block pointer 
* 
* RETURNS
* 
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS cpb_egress_next_block_get(UINT_8 chip_id,UINT_16 start_index,UINT_16 *ts_next_block)
{
    UINT_16 block_index = 0;

    if (NULL == ts_next_block)
    {
        printf("[%s] invalid pointer!!\r\n", __FUNCTION__);

        return RET_PARAERR;
    }

    /* config each ts's next block */
    for (block_index = start_index; block_index < CPB_EGR_BLK_NUM; block_index++)
    {
        if (true == g_cpb_egr_info[chip_id].cpb_egr_next_blk[block_index].idle)
        {
            /* search the available block for ts's next block */
            *ts_next_block = block_index;
            g_cpb_egr_info[chip_id].cpb_egr_next_blk[block_index].idle = false;
            g_cpb_egr_info[chip_id].next_blk_idle_num--;

            break;
        }
    }
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_ch_config
*
* DESCRIPTION
*
*     Cpb egress config.
*
* NOTE
*
*     chip_id: chip number used
*     add_del_sel: 1-add, 0-delete
*     ch_id: channel id, 0~CH_NUM - 1
*     ts_num: channel's ts number
*     sar_ind: channel sar ind, 0-mac, 1-sar
* PARAMETERS
*
*     RESERVED 
* 
* RETURNS
* 
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS cpb_egress_ch_config(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_16 ts_num)
{
    UINT_16 blk_idx = 0;
    UINT_16 start_blk = 0;
    UINT_16 ts_pre_blk = 0;
    UINT_16 ts_next_blk = 0;
    UINT_32 parameter = 0;
    
    if ((chip_id > DEV_NUM - 1) || 
        (add_del_sel > 1) ||
        (ch_id > CPB_EGR_CH_NUM - 1) ||
        ((ts_num < 1) && (1 == add_del_sel)) || (ts_num > CPB_EGR_BLK_NUM))
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
                ts_num
              );

        return RET_PARAERR;
    }

    /* Check the chip's initialization */
    if (false == g_cpb_egr_info[chip_id].init_done)
    {
        /* The chip has not been initialized */
        printf("[%s] The chip(id = %d) has not been initialized. \r\n", __FUNCTION__, chip_id);
    
        return RET_PARAERR;
    }

    /* delete ch's cur block and next block config */
    if (0 == add_del_sel)
    {
        /* check ch whether configed */
        if (true == g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_id].idle)
        {
            printf("[%s] ch was not configed, delete fail!! \
                    \r\n\t chip_id = %d, \
                    \r\n\t add_del_sel = %d, \
                    \r\n\t ch_id = %d, \
                    \r\n\t ts_num = %d\r\n", 
                    __FUNCTION__,
                    chip_id, 
                    add_del_sel, 
                    ch_id, 
                    ts_num);

            return RET_PARAERR;
        }

        /* disable cpb egress channel */
        cpb_egress_mon_en_eg_set(chip_id,ch_id,0);

        /* set cpb egress channel current_blk to default value */
        parameter = 0x7f;
        cpb_egress_star_blk_eg_set(chip_id,ch_id,&parameter);

        /*clr bp level egress config*/
        parameter = 0;
        cpb_egress_bp_level_egress_set(chip_id,ch_id,&parameter);

        /*clr mac rdy depth config*/
        /*parameter = 0;*/
        /*cpb_egress_mac_rdy_depth_eg_set(chip_id,ch_id,&parameter);*/

        /*clr bp low egress config*/
        parameter = 0;
        cpb_egress_bp_low_egress_set(chip_id,ch_id,&parameter);
        /* set cpb egress channel current_blk struct */
        g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_id].ch = 0x7f;
        g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_id].index = 0x7f;
        g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_id].idle = true;

        /* set cpb egress ch_depthparamram to default value */
        cpb_egress_ch_depth_para_set_for_performance(chip_id,ch_id,1,2);

		parameter = 0x8;
		cpb_egress_aempty_depth_set(chip_id,ch_id,&parameter);
		
        /* delete next block */
        for (blk_idx = 0; blk_idx < CPB_EGR_BLK_NUM; blk_idx++)
        {
            if ((ch_id == g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].ch) &&
                (false == g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].idle))
            {
                /* set cpb egress ch_nextblkram to default value */
                parameter = 0x407f;
                cpb_egress_next_blk_eg_set(chip_id,blk_idx,&parameter);

                /* set cpb egress ch_nextblkram struct to default value */
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].ch = 0x7f;
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].index = 0x7f;
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].idle = true;
                g_cpb_egr_info[chip_id].next_blk_idle_num++;
            }
        }

    }
    /* add ch & config */
    else
    {
        /* check ch whether configed */
        if (false == g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_id].idle)
        {
            /* channel has been configed */
            printf("[%s] the channel has been configed!! \
                    \r\n if you want to config it again,please first delete it!! \
                    \r\n\t chip_id = %d, \
                    \r\n\t add_del_sel = %d, \
                    \r\n\t ch_id = %d, \
                    \r\n\t ts_num = %d\r\n", 
                    __FUNCTION__,
                    chip_id, 
                    add_del_sel, 
                    ch_id, 
                    ts_num);

            return RET_PARAERR;
        }

        /* invalid channel and ts_num */
        if (g_cpb_egr_info[chip_id].next_blk_idle_num < ts_num)
        {
            printf("[%s] invalid config, not enought next blk to be used!! \
                    \r\n\t chip_id = %d, \
                    \r\n\t add_del_sel = %d, \
                    \r\n\t ch_id = %d, \
                    \r\n\t ts_num = %d,\r\n", 
                    __FUNCTION__,
                    chip_id, 
                    add_del_sel, 
                    ch_id, 
                    ts_num);

            return RET_FAIL;
        }

        /* search the start blk */
        for (blk_idx = 0; blk_idx < CPB_EGR_BLK_NUM; blk_idx++)
        {
            if (true == g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].idle)
            {
                start_blk = blk_idx;

                /* set cpb egress channel current_blk */
                parameter = start_blk;
                cpb_egress_star_blk_eg_set(chip_id,ch_id,&parameter);
                
                /* set cpb egress channel current_blk struct */
                g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_id].ch = ch_id;
                g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_id].index = start_blk;
                g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_id].idle = false;

                break;
            }
        }

        /* initial it for used times */
        ts_pre_blk  = 0;
        ts_next_blk = 0;
            
        /* valid next blk,config it */
        for (blk_idx = 0; blk_idx < ts_num; blk_idx++)
        {
            
            /* get the ts's next block */
            cpb_egress_next_block_get(chip_id,ts_pre_blk,&ts_next_blk);

            /* check whether ts is the first, config next block*/
            if (0 != blk_idx)
            {
                /* set cpb egress ch_nextblkram */
                parameter =  ((0 << SHIFT_BIT_14) | (blk_idx << SHIFT_BIT_7) | ts_next_blk) & MASK_15_BIT;
                cpb_egress_next_blk_eg_set(chip_id,ts_pre_blk,&parameter);

                /* set cpb egress next blk struct */
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[ts_pre_blk].ch = ch_id;
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[ts_pre_blk].index = ts_next_blk;
            }
            
            ts_pre_blk = ts_next_blk;

            /* the last ts */
            if ((ts_num - 1) == blk_idx)
            {
                /* set cpb egress ch_nextblkram */
                parameter =  ((1 << SHIFT_BIT_14) | (0 << SHIFT_BIT_7) | start_blk) & MASK_15_BIT;
                cpb_egress_next_blk_eg_set(chip_id,ts_pre_blk,&parameter);

                /* set cpb egress next blk struct */
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[ts_pre_blk].ch = ch_id;
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[ts_pre_blk].index = start_blk;
            }           
        }
           
        /* set cpb egress channel depth para */
        /*cpb_egress_ch_depth_para_set(chip_id,ch_id,ts_num);*/
        cpb_egress_ch_depth_para_set_for_performance(chip_id,ch_id,ts_num,2);
		
		/*cpb config aempty_depth*/
        if(ts_num >= 8)
        {
            parameter = 0x8;
        }
        else
        {
            parameter = ts_num;
        }
        /** coverity[callee_ptr_arith] */
        cpb_egress_aempty_depth_set(chip_id,ch_id,&parameter);
		
        /* delay 1 us */
        time_delay(125);

        /* enable cpb egress channel */
        cpb_egress_mon_en_eg_set(chip_id,ch_id,1);

    }
    
    /* clr cpb egress cnt */
    cpb_egress_sta_clr_ch_eg_set(chip_id,ch_id);
    cpb_egress_sta_clr_eg_pls_set(chip_id,STA_CLR_EG);

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_egress_debug_print
*
* DESCRIPTION
*
*     Print cpb_egress debug info.
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
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS cpb_egress_debug_print(UINT_8 chip_id)
{
    UINT_8  ch_idx = 0;
    UINT_16 blk_idx = 0;

    if (chip_id > DEV_NUM - 1)
    {
        printf("[%s] invalid parameter!! \
                \r\n\t chip_id = %d\r\n", 
                __FUNCTION__,
                chip_id
              );

        return RET_PARAERR;
    }

    /* Check the chip's initialization */
    if (false == g_cpb_egr_info[chip_id].init_done)
    {
        /* The chip has not been initialized */
        printf("[%s] The chip(id = %d) has not been initialized. \r\n", __FUNCTION__, chip_id);
    
        return RET_PARAERR;
    }

    printf("#### [%s] chip_id = %d, print start/next block info ####\r\n", __FUNCTION__, chip_id);
    
    /* cpb egress start block info */
    printf("#### start_block[0_39] chip_id = %d,cpb_egress_block result test start ####\r\n", chip_id);
    printf("%s\t%s\t%s\t%s\r\n", "pos", "ch", "index", "idle");
    for (ch_idx = 0; ch_idx < CPB_EGR_CH_NUM; ch_idx++)
    {
        printf("%2d\t%2d\t%2d\t%2d\r\n",
                ch_idx,
                g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_idx].ch,
                g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_idx].index,
                g_cpb_egr_info[chip_id].cpb_egr_cur_blk[ch_idx].idle
              );
    }
    printf("#### start_block[0_39] chip_id = %d,cpb_egress_block result test end ####\r\n", chip_id);
    printf("\r\n");

    /* cpb egress next block info */
    printf("#### next_block[0_39] chip_id = %d,cpb_egress_block result test start ####\r\n", chip_id);
    printf("%s\t%s\t%s\t%s\r\n", "pos", "ch", "index", "idle");
    for (blk_idx = 0; blk_idx < CPB_EGR_BLK_NUM; blk_idx++)
    {
        printf("%2d\t%2d\t%2d\t%2d\r\n",
                blk_idx,
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].ch,
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].index,
                g_cpb_egr_info[chip_id].cpb_egr_next_blk[blk_idx].idle
              );
    }
    printf("#### next_block[0_39] chip_id = %d,cpb_egress_block result test end ####\r\n", chip_id);
    printf("\r\n");

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_diag
*
* DESCRIPTION
*
*     Print cpb egress cnt, status & alm.
*
* NOTE
*
*     RESERVED
*
* PARAMETERS
*
*     chip_id: chip number used
*     ch_id: 0~CH_NUM - 1, -1 is all ch
*     mode: 0-simple diag, 1-detail diag
* 
* RETURNS
* 
*    0: success
*    1: fail
* 
******************************************************************************/
RET_STATUS cpb_diag(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode)
{
#ifndef SV_TEST
    UINT_8  idx = 0;
    UINT_8  idx_start = 0;
    UINT_8  idx_end = 0;
    UINT_32 val[2] = {0};
    char cntbuf[2][21] = {{0}};
    
    if ((chip_id > DEV_NUM - 1) || ((ch_id > CPB_EGR_CH_NUM - 1) && (ch_id < 255)) || (mode > 1))
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

    /* Check the chip's initialization */
    if (false == g_cpb_egr_info[chip_id].init_done)
    {
        /* The chip has not been initialized */
        printf("[%s] The chip(id = %d) has not been initialized. \r\n", __FUNCTION__, chip_id);
    
        return RET_PARAERR;
    }

    /* Check whether print all ch */
    if (255 == ch_id)
    {
        idx_start = 0;
        idx_end = CPB_EGR_CH_NUM;
    }
    else
    {
        idx_start = ch_id;
        idx_end = ch_id + 1;
    }



    /* Print diag diagram head */
    printf("\n                           |             I L K N    I N T E R F A C E            |  \n");
    printf("                          \\|/                                                   \\|/\n");

    
    for (idx = idx_start; idx < idx_end; idx++)
    {

        /* Print cpb adp module  */
        diag_print_line();
        diag_print_title("CPB EGR");
        diag_print_title("CH%d",idx);

        diag_print_line();
        /* Print cpb ing/erg max cnt */
        cpb_egress_cnt_max_eg_get(chip_id,idx,&val[0]);
        diag_cnt_val_to_str(&val[0],1,0,cntbuf[0],21);
        printf("| %56s = %-46s |\n", "CNT_MAX", cntbuf[0]);

        /* Print cpb erg cell_in_cnt & cpb ing tx_pkt_num */
        cpb_egress_cell_in_cnt_egress_get(chip_id,idx,val);
        diag_cnt_val_to_str(val,2,0,cntbuf[0],21);
        printf("| %56s = %-46s |\n", "CELL_IN_CNT", cntbuf[0]);

        /* Print cpb erg cell_out_cnt & cpb ing rx_pkt_num */
        cpb_egress_cell_out_cnt_egress_get(chip_id,idx,val);
        diag_cnt_val_to_str(val,2,0,cntbuf[0],21);
        printf("| %56s = %-46s |\n", "CELL_OUT_CNT", cntbuf[0]);

        /* Print cpb erg in err cnt & cpb ing rx_err_num */
        cpb_egress_err_cnt_eg_tb_get(chip_id,idx,val);
        diag_cnt_val_to_str(val,2,0,cntbuf[0],21);
        printf("| %56s = %-46s |\n", "IN_ERR_CNT", cntbuf[0]);

        if(1 == mode)
        {
            /* Print cpb erg sop_in_cnt & cpb ing rx_sop_num */
            cpb_egress_all_cnt_ram_get(chip_id,3,idx,val);
            diag_cnt_val_to_str(val,2,0,cntbuf[0],21);
            printf("| %56s = %-46s |\n", "IN_SOP_CNT", cntbuf[0]);
            
            /* Print cpb erg sop_out_cnt & cpb ing tx_sop_num */
            cpb_egress_all_cnt_ram_get(chip_id,4,idx,val);
            diag_cnt_val_to_str(val,2,0,cntbuf[0],21);
            printf("| %56s = %-46s |\n", "OUT_SOP_CNT", cntbuf[0]);
        }

        /* Print cpb ing/erg full alm */
        cpb_egress_link_full_eg_get(chip_id,idx,&val[0]);
        diag_cnt_val_to_str(&val[0],1,0,cntbuf[0],21);
        printf("| %56s = %-46s |\n", "LINK_FULL", cntbuf[0]);

        if(1 == mode)
        {
            /* Print cpb ing/erg empty alm */
            cpb_egress_link_empty_eg_get(chip_id,idx,&val[0]);
            diag_cnt_val_to_str(&val[0],1,0,cntbuf[0],21);
            printf("| %56s = %-46s |\n", "LINK_EMPTY", cntbuf[0]);
        }
    }
    
    /* Print last line */
    diag_print_line();

    /* Print normal tail */
    printf("                           |                                                     |\n");
    printf("                          \\|/              M A C    I N T E R F A C E           \\|/\n");

#endif
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     cpb_diag_clear
*
* DESCRIPTION
*
*     Clear cpb cnt.
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
RET_STATUS cpb_diag_clear(UINT_8 chip_id, UINT_8 ch_id)
{
    if ((chip_id > DEV_NUM - 1) || ((ch_id > CPB_EGR_CH_NUM - 1) && (ch_id < 255)))
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

    /* Check the chip's initialization */
    if (false == g_cpb_egr_info[chip_id].init_done)
    {
        /* The chip has not been initialized */
        printf("[%s] The chip(id = %d) has not been initialized. \r\n", __FUNCTION__, chip_id);

        return RET_PARAERR;
    }

    /* Check whether clear all ch cnt */
    if (255 == ch_id)
    {
        /* clear cpb egress ch cnt */
        cpb_egress_clr_eg_set(chip_id,1);
        cpb_egress_clr_eg_set(chip_id,0);
        printf("Cpb egress all ch cnt clear. \r\n");
    }
    else
    {
        /* clear cpb egress ch cnt */
        cpb_egress_sta_clr_ch_eg_set(chip_id,ch_id);
        cpb_egress_sta_clr_eg_pls_set(chip_id,STA_CLR_EG);
        printf("Cpb egress ch=%d cnt clear. \r\n", ch_id);
    }

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     cpb debug function.
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      kejian        2018-03-29         1.0            initial
*
******************************************************************************/
RET_STATUS cpb_debug(void)
{
    printf("+----------------------cpb_ingress initialized function---------------------------------------------+\r\n");
    printf("cpb_ingress_init(UINT_8 chip_id)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    
    printf("+----------------------cpb_ingress_sch state check function-----------------------------------------+\r\n");
    printf("cpb_ingress_sch_st_chk(UINT_8 chip_id,UINT_8 chk_times,UINT_32 delay)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("chk_times: check times.\r\n");
    printf("delay: delay some ns.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------cpb_ingress_channel_config function------------------------------------------+\r\n");
    printf("cpb_ingress_ch_config(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_16 ts_num,UINT_32 fifo_fc_en)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("add_del_sel: add or del,0---delete,1---select.\r\n");
    printf("ch_id: channel id,range,0-CHN_SUM-1.\r\n");
    printf("ts_num:channel's ts numbers.\r\n");
    printf("fifo_fc_en:channel fifo fc enable .\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------cpb_ingress_debug/dump cpb ingress info  function----------------------------+\r\n");
    printf("cpb_ingress_debug_print(UINT_8 chip_id)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------set cpb egress ch depth parameter function-----------------------------------+\r\n");
    printf("cpb_egress_ch_depth_para_set(UINT_8 chip_id,UINT_8 ch,UINT_16 ts_num)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("ch: channel_id, range is 0~CH_NUM - 1.\r\n");
    printf("ts_num: channel's ts number.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");
    
    printf("+----------------------set cpb egress ch depth parameter function(for performance)-----------------------------------+\r\n");
    printf("RET_STATUS cpb_egress_ch_depth_para_set_for_performance(UINT_8 chip_id,UINT_8 ch,UINT_16 ts_num,UINT_8 mode)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("ch: channel_id, range is 0~CH_NUM - 1.\r\n");
    printf("ts_num: channel's ts number.\r\n");
    printf("mode:1~10:1~10us(mode>10 mode=10).\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------cpb egress initiation function-----------------------------------------------+\r\n");
    printf("cpb_egress_init(UINT_8 chip_id)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------get cpb egress next block function-------------------------------------------+\r\n");
    printf("cpb_egress_next_block_get(UINT_8 chip_id,UINT_16 start_index,UINT_16 *ts_next_block)\r\n");
    printf("\r\n");
    
    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("start_index: search start index.\r\n");
    printf("ts_next_block: ts's next block pointer.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------cpb_egress_channel_config function-------------------------------------------+\r\n");
    printf("cpb_egress_ch_config(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_16 ts_num,UINT_32 sar_ind)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("add_del_sel: add or del,0---delete,1---select.\r\n");
    printf("ch_id: channel id,range,0-CHN_SUM-1.\r\n");
    printf("ts_num:channel's ts numbers.\r\n");
    printf("sar_ind:channel sar ind, 0-mac, 1-sar.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------cpb_egress_debug/dump cpb egress info  function------------------------------+\r\n");
    printf("cpb_egress_debug_print(UINT_8 chip_id)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------print cpb ingress/egress cnt,status & alm function---------------------------+\r\n");
    printf("cpb_diag(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("ch_id: 0~CH_NUM - 1, -1 is all ch\r\n");
    printf("mode: 0---simple diag,1---detail diag.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------clear cpb count function-----------------------------------------------------+\r\n");
    printf("cpb_diag_clear(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("ch_id: 0~CH_NUM - 1, -1 is clear all ch cnt\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------Cpb ing to egr loopback set function-----------------------------------------+\r\n");
    printf("cpb_diag_lpbk_set(UINT_8 chip_id,UINT_8 mode)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("mode: 0-none, 1-shallow loopback enable, 2-deep loopback enable\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    return RET_SUCCESS;
}
