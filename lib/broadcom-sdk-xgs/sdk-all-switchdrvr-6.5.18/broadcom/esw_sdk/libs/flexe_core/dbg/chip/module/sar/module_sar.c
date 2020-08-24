

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hal.h"
#include "sal.h"
#include "reg_operate.h"
#include "b66sar_rx_tx.h"
#include "global_macro.h"
#include "module_sar.h"
#include "flexe_env.h"
#include "req_gen.h"
#include "inf_ch_adp_rx.h"
#include "inf_ch_adp_tx.h"
#include "oam_rx.h"
#include "module_cpb.h"
#include "module_mcmac.h"
#include <sal/core/boot.h>
extern RET_STATUS mac_rateadp_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_8 ts_num);
/*#include "module_rateadpt.h"*/
/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int module_sar_para_debug = 0;
UINT_32 g_sar_rx_channel_of_ts[DEV_NUM][SAR_TS_NUM];
UINT_32 g_sar_tx_channel_of_ts[DEV_NUM][SAR_TS_NUM];
UINT_32 g_client_channel_of_ts[DEV_NUM][84];
UINT_32 rx_port_state[DEV_NUM][8];
UINT_32 rx_bcm_port_state[DEV_NUM][8];
/*UINT_32 rx_flexe_oh_state[DEV_NUM][8];*/
UINT_32 tx_port_state[DEV_NUM][8];
UINT_32 tx_bcm_port_state[DEV_NUM][8];
/*UINT_32 tx_flexe_oh_state[DEV_NUM][8];*/
UINT_32 rx_cfg[DEV_NUM];
UINT_32 tx_cfg[DEV_NUM];

busa_info_t busa_rx_info[DEV_NUM][8];
busa_info_t busa_tx_info[DEV_NUM][8];


#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
*
* FUNCTION 
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS sar_init(UINT_8 chip_id)
{
    UINT_8 index = 0;
    UINT_8 ts_id;
    UINT_32 ra2sar_value = 0x7f;
    UINT_32 cpb2sar_value = 0x7f;
    
    /*step1:init g_sar_channel_of_ts*/
    for(ts_id = 0; ts_id < SAR_TS_NUM;ts_id++)
    {
        g_sar_rx_channel_of_ts[chip_id][ts_id] = 0xff;
        g_sar_tx_channel_of_ts[chip_id][ts_id] = 0xff;
    }

    
    /*step2:init cpb2sar and ra2sar*/
    for(index = 0; index < 160; index++)
    {    
        b66sar_rx_cpb2sar_ram_set(chip_id,index,&cpb2sar_value);
        b66sar_tx_ra2sar_ram_set(chip_id,index,&ra2sar_value);
    }
    
    /*step3:calendar_en */
    b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_CALENDAR_EN+6,1);
    b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_CALENDAR_EN+2,1);
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION sar_rx_init
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS sar_rx_init(UINT_8 chip_id)
{
    UINT_8 index = 0;
    UINT_8 ts_id;
    UINT_32 cpb2sar_value = 0x7f;
    UINT_32 sar_alm = 0;
    UINT_32 parameter = 0;

    b66sar_rx_cpb2sar_table_switch_set(chip_id,CPB2SAR_FIFO_ADJ_INIT);
    b66sar_rx_cpb2sar_table_switch_set(chip_id,SAR_FIFO_ADJ_INIT);
    b66sar_rx_cpb2sar_table_switch_set(chip_id,CPB2SAR_DECIMATE_INIT);

    for (index = 0; index < 100; index++)
    {
        parameter = 0;
        b66sar_rx_state_get(chip_id,CPB2SAR_FIFO_ADJ_INIT_DONE,&parameter);
        if (parameter == 1)
        {
            break;
        }
    }
    if (!SAL_BOOT_SIMULATION)
    {
        if (index == 100)
        {
            printf("[%s] error! CPB2SAR_FIFO_ADJ_INIT_DONE can't get 1!\r\n",__FUNCTION__);
            return RET_FAIL;
        }
    }
    
    /*step1:init g_sar_channel_of_ts*/
    for(ts_id = 0; ts_id < SAR_TS_NUM;ts_id++)
    {
        g_sar_rx_channel_of_ts[chip_id][ts_id] = 0xff;
    }

    
    /*step2:init cpb2sar and ra2sar*/
    for(index = 0; index < 160; index++)
    {    
        b66sar_rx_cpb2sar_ram_set(chip_id,index,&cpb2sar_value);
    }
    
    /*step3:calendar_en */
    b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_CALENDAR_EN+4,1);

    /*step4:clear sar rx drop one drop two alm*/
    for(index = 0;index < 80;index++)
    {
        b66sar_sar_rx_ch_alm_get(chip_id,index,RX_PAR_ERR,&sar_alm);
        b66sar_sar_rx_ch_alm_get(chip_id,index,RX_DROP_ONE_ERR,&sar_alm);
        b66sar_sar_rx_ch_alm_get(chip_id,index,RX_DROP_TWO_ERR,&sar_alm);
        b66sar_sar_rx_ch_alm_get(chip_id,index,RX_FIFO_HIGH_IND,&sar_alm);
        b66sar_sar_rx_ch_alm_get(chip_id,index,RX_FIFO_LOW_IND,&sar_alm);
        b66sar_sar_rx_ch_alm_get(chip_id,index,RX_FIFO_FULL,&sar_alm);
        b66sar_sar_rx_ch_alm_get(chip_id,index,RX_FIFO_EMPTY,&sar_alm);
    }

    /*sar_rx_glb_alm*/
    b66sar_sar_rx_glb_alm_get(chip_id,BSIZE_ERR,&sar_alm);
    b66sar_sar_rx_glb_alm_get(chip_id,ADJ_IN_ALM,&sar_alm);
    b66sar_sar_rx_glb_alm_get(chip_id,I1_OVERFLOW,&sar_alm);
    b66sar_sar_rx_glb_alm_get(chip_id,I1_UNDERFLOW,&sar_alm);
    b66sar_sar_rx_glb_alm_get(chip_id,I2_OVERFLOW,&sar_alm);
    b66sar_sar_rx_glb_alm_get(chip_id,I2_UNDERFLOW,&sar_alm);

    /*sar_rx_pkt_cnt*/
    b66sar_rx_pkt_cnt_get(chip_id,&sar_alm);

    /*step5:set cfg reg default*/

    /*sar_rx_glb_cfg_reg*/
    b66sar_rx_loopback_set(chip_id,0);
    b66sar_rx_pkt_cnt_probe_set(chip_id,0);

    /*sar_rx_ctrl_cfg_reg*/
    for(index = 0;index < SAR_CHANNEL_NUM;index++)
    {
        b66sar_rx_ctrl_cfg_set(chip_id,index,MON_EN,0);
        b66sar_rx_ctrl_cfg_set(chip_id,index,CPB2SAR_CH_EN,0);
        b66sar_rx_ctrl_cfg_set(chip_id,index,BRCM_FC_CH_EN,0);
        b66sar_rx_ctrl_cfg_set(chip_id,index,RX_CHAN_MAP,0x200);
        b66sar_rx_ctrl_cfg_set(chip_id,index,RX_FIFO_HIGH_CFG,0xd);
        b66sar_rx_ctrl_cfg_set(chip_id,index,RX_FIFO_LOW_CFG,0x3);
        b66sar_rx_ctrl_cfg_set(chip_id,index,RX_FIFO_MID_CFG,0x7);
    }
    
    b66sar_rx_tx_b66size_29b_set(chip_id,0,0);
    
    /*cpb2sar_cfg1_reg and cpb2sar_cfg2_reg*/
    b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_CFG_PERIOD,2507);
    b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_NOM_CNT,1887);
    b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_FIFO_ADJ_EN,1);
    b66sar_rx_cpb2sar_cfg_set(chip_id,SAR_FIFO_ADJ_EN,1);
    b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_Q+4,32);
    b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_ADJ_01EN+4,0);
    b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_ADJ_IN_CHECK_VALUE+4,100);
    b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_ADJ_IN_CORRECT_EN+4,1);

    /*cpb2sar_cfg3_reg*/
    b66sar_rx_cpb2sar_cfg3_set(chip_id,CPB2SAR_LOCAL_EN,1);
    b66sar_rx_cpb2sar_cfg3_set(chip_id,CPB2SAR_LOCAL_M,426);
    b66sar_rx_cpb2sar_cfg3_set(chip_id,DELAY_CYCLES,20);

    /*cpb2sar_cfg4_reg*/
    b66sar_rx_cpb2sar_cfg4_set(chip_id,CPB2SAR_FIFO_ADJ_PERIOD,32767);
    b66sar_rx_cpb2sar_cfg4_set(chip_id,SAR_FIFO_ADJ_PERIOD,32767);

    /**/
    b66sar_rx_cpb_m_set(chip_id,2+CPB2SAR_M_2ND,0xFEBC44FF);
    b66sar_rx_cpb_m_set(chip_id,3+CPB2SAR_BASE_M_2ND,0xFFF8D4FF);
    b66sar_rx_cpb_m_set(chip_id,0+CPB2SAR_M_1ST,0xAB77B8B0);
    b66sar_rx_cpb_m_set(chip_id,1+CPB2SAR_BASE_M_1ST,0xAB77C2B0);
    b66sar_rx_cpb2sar_cfg_311m_set(chip_id,CPB2SAR_MULTIPLIER,3);
    b66sar_rx_cpb2sar_cfg_311m_set(chip_id,CPB2SAR_BYPASS_M_N_2ND,0);
    
    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION sar_tx_init
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS sar_tx_init(UINT_8 chip_id)
{
    UINT_8 index = 0;
    UINT_8 ts_id;
    UINT_32 ra2sar_value = 0x7f;
    UINT_32 sar_alm = 0;
    
    /*step1:init g_sar_tx_channel_of_ts*/
    for(ts_id = 0; ts_id < SAR_TS_NUM;ts_id++)
    {
        g_sar_tx_channel_of_ts[chip_id][ts_id] = 0xff;
    }

    /*step2:init cpb2sar and ra2sar*/
    for(index = 0; index < 160; index++)
    {    
        b66sar_tx_ra2sar_ram_set(chip_id,index,&ra2sar_value);
    }
    
    /*step3:calendar_en */
    b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_CALENDAR_EN+2,1);

    /*step4:clear alm and cnt reg*/

    /*sar_tx_pkt*/
    b66sar_tx_pkt_get(chip_id,&sar_alm);

    /*ra2sar_env_alm*/
    b66sar_tx_ra2sar_env_alm_get(chip_id,TX_ADJ_IN_ALM,&sar_alm);
    b66sar_tx_ra2sar_env_alm_get(chip_id,TX_I1_OVERFLOW,&sar_alm);
    b66sar_tx_ra2sar_env_alm_get(chip_id,TX_I1_UNDERFLOW,&sar_alm);
    b66sar_tx_ra2sar_env_alm_get(chip_id,TX_I2_OVERFLOW,&sar_alm);
    b66sar_tx_ra2sar_env_alm_get(chip_id,TX_I2_UNDERFLOW,&sar_alm);

    /*step5:set cfg reg default*/

    /*sar_tx_glb_cfg_reg*/
    b66sar_tx_loopback_set(chip_id,0);
    b66sar_rx_tx_b66size_29b_set(chip_id,1,0);
    b66sar_tx_cnt_probe_set(chip_id,0,0);

    /*ra2sar_cfg1_reg and ra2sar_cfg2_reg*/
    b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_CFG_PERIOD,2507);
    b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_NOM_CNT,1887);
    b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_Q+2,32);
    b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_ADJ_01EN+2,0);
    b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_ADJ_IN_CHECK_VALUE+2,100);
    b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_ADJ_IN_CORRECT_EN+2,1);

    /*sar_tx_ctrl_cfg_reg*/
    for(index = 0; index < SAR_CHANNEL_NUM; index++)
    {
        b66sar_tx_ctrl_cfg_set(chip_id,index,SEG_EN,0);
        b66sar_tx_ctrl_cfg_set(chip_id,index,RA2SAR_CH_EN,0);
        b66sar_tx_ctrl_cfg_set(chip_id,index,TX_CHAN_MAP,0x200);
    }

    /*ra2sar_cfg3_reg*/
    b66sar_tx_ra2sar_cfg3_set(chip_id,RA2SAR_LOCAL_EN,1);
    b66sar_tx_ra2sar_cfg3_set(chip_id,RA2SAR_LOCAL_M,426);

    /**/
    b66sar_tx_ra_m_set(chip_id,0+RA2SAR_M_1ST,0xab77b8b0);
    b66sar_tx_ra_m_set(chip_id,1+RA2SAR_BASE_M_1ST,0xab77c2b0);
    b66sar_tx_ra_m_set(chip_id,2+RA2SAR_M_2ND,0xfebc44ff);
    b66sar_tx_ra_m_set(chip_id,3+RA2SAR_BASE_M_2ND,0xfff8d4ff);
    b66sar_tx_ra2sar_cfg_311m_set(chip_id,RA2SAR_MULTIPLIER,3);
    b66sar_tx_ra2sar_cfg_311m_set(chip_id,RA2SAR_BYPASS_M_N_2ND,0);
        
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
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
*      chip_id:chip number used
*      add_del_sel:0:delete 1:add 
*      channel_id:0~79
*      b_mode:0:28*66B,1:29*66B
*      ts_num:number of ts
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS sar_rx_ts_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num)
{
    UINT_8 offset = 0;
    UINT_8 index = 0;
    UINT_32 current_table = 0;
    UINT_32 channel_to_write = channel_id;
    UINT_32 joint_init_done = 0;
    RET_STATUS rt = RET_SUCCESS;
    
    if (chip_id > MAX_DEV || add_del_sel > 1|| channel_id > (CH_NUM-1) || b_mode > 1)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,add_del_sel=%d,channel_id=%d,b_mode=%d\r\n",__FUNCTION__,chip_id,add_del_sel,channel_id,b_mode);
        }

        return RET_PARAERR;
    }

    if ( ts_num > 80)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid ts!!ts_num=%d\r\n",__FUNCTION__,ts_num);
        }

        return RET_PARAERR;
    }
    
    if (0 == add_del_sel)
    {
        b66sar_rx_ctrl_cfg_set(chip_id,channel_id,MON_EN,0);
        b66sar_rx_ctrl_cfg_set(chip_id,channel_id,CPB2SAR_CH_EN,0);        
        for (index = 0; index < SAR_TS_NUM; index++)
        {
            if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
            {
                offset = 2*index;
                channel_to_write = 0x7f;
                b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                offset = 2*index+1;
                b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
            }
        }
        sar_ts_free(chip_id,channel_id,0);
    }
    else
    {    
        /*check channel whether used*/
        for (index = 0; index < SAR_TS_NUM; index++)
        {
            if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
            {
                printf("[%s] channel_id = %d has been used!! \r\n",__FUNCTION__,channel_id);
                return RET_FAIL;
            }
        }
        
        b66sar_rx_state_get(chip_id,CPB2SAR_CURRENT_TABLE,&current_table);
        
        rt = sar_ts_allocate(chip_id,channel_id,0,ts_num);
        
        for (index = 0; index < SAR_TS_NUM; index++)
        {
            if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
            {
                offset = 2*index;
                b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                offset = 2*index+1;
                b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
            }            
        }
        
    
        b66sar_rx_cpb_m_set(chip_id,0+CPB2SAR_M_1ST,0x1312d);
        b66sar_rx_cpb_m_set(chip_id,2+CPB2SAR_M_2ND,0x4feac05);
        b66sar_rx_cpb_m_set(chip_id,3+CPB2SAR_BASE_M_2ND,0xffffdc05);
        b66sar_rx_cpb2sar_cfg_311m_set(chip_id,CPB2SAR_MULTIPLIER,1);
        b66sar_rx_cpb2sar_cfg_311m_set(chip_id,CPB2SAR_BYPASS_M_N_2ND,0);
        b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_Q+4,32);
        b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_ADJ_01EN+4,0);
        b66sar_rx_cpb2sar_cfg3_set(chip_id,CPB2SAR_LOCAL_EN,1);
        b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_ADJ_IN_CHECK_VALUE+4,100);
        b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_ADJ_IN_CORRECT_EN+4,1);
        b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_FIFO_ADJ_EN,1);
        b66sar_rx_cpb2sar_cfg4_set(chip_id,CPB2SAR_FIFO_ADJ_PERIOD,2);
        /*b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_FIFO_ADJ_PERIOD,2);*/
        
        if (0 == b_mode)
        {
            b66sar_rx_cpb_m_set(chip_id,1+CPB2SAR_BASE_M_1ST,0xffff87ed);
            b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_CFG_PERIOD,0xbc0);
            b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_NOM_CNT,0x7c4);
            b66sar_rx_cpb2sar_cfg3_set(chip_id,CPB2SAR_LOCAL_M,555);
            
        }
        else
        {
            b66sar_rx_cpb_m_set(chip_id,1+CPB2SAR_BASE_M_1ST,0xffff78bd);
            b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_CFG_PERIOD,0xbb9);
            b66sar_rx_cpb2sar_cfg_set(chip_id,CPB2SAR_NOM_CNT,0x77b);
            b66sar_rx_cpb2sar_cfg3_set(chip_id,CPB2SAR_LOCAL_M,515);
        }        
        b66sar_rx_tx_b66size_29b_set(chip_id,0,b_mode);

        
        b66sar_rx_ctrl_cfg_set(chip_id,channel_id,CPB2SAR_CH_EN,1);
        b66sar_rx_state_get(chip_id,RX_JOINT_INIT_DONE,&joint_init_done);
        if (1 == joint_init_done)
        {
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,MON_EN,1);
        }
        else
        {
            if (module_sar_para_debug)
            {
                printf("[%s] joint init not done!!\r\n",__FUNCTION__);
            }
        }
    }
    
    return rt;
}

/******************************************************************************
*
* FUNCTION 
*
*     sar_rx_ts_cfg_asic
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
*      chip_id:chip number used
*      add_del_sel:0:delete 1:add 
*      channel_id:0~79
*      b_mode:0:28*66B,1:29*66B
*      ts_num:number of ts
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-5-29    1.0           initial
*
******************************************************************************/
RET_STATUS sar_rx_ts_cfg_asic(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num)
{
    UINT_8 index = 0;
	UINT_8 ts_num_cur = 0;
    RET_STATUS rt = RET_SUCCESS;
    
    if (chip_id > MAX_DEV || add_del_sel > 1|| channel_id > (CH_NUM-1) || b_mode > 1)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,add_del_sel=%d,channel_id=%d,b_mode=%d\r\n",__FUNCTION__,chip_id,add_del_sel,channel_id,b_mode);
        }

        return RET_PARAERR;
    }

    if ((ts_num > 80) || ((ts_num == 0)&& (1 == add_del_sel)))
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid ts!!ts_num=%d\r\n",__FUNCTION__,ts_num);
        }

        return RET_PARAERR;
    }

	/* get the ts number of the channel */
	for (index = 0; index < SAR_TS_NUM; index++)
	{
		if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
		{
			ts_num_cur++;
		}
	}

	if ((0 == add_del_sel)&&(0 == ts_num_cur))
	{
		printf("[%s] channel=%d of sar rx has not been configed\r\n",__FUNCTION__,channel_id);

		return RET_SUCCESS;
	}

	if ((1 == add_del_sel)&&(ts_num == ts_num_cur))
	{
		printf("[%s]sar rx channel=%d has been configed,don't config the same\r\n",__FUNCTION__,channel_id);

		return RET_SUCCESS;
	}

	if (0 == add_del_sel)
	{
		sar_rx_ts_cfg_asic_in_chip( chip_id,  add_del_sel,  channel_id,  b_mode,  ts_num);
	}
    else
    {
    	if (0 != ts_num_cur)
    	{
    		sar_rx_ts_cfg_asic_in_chip(chip_id,  0,  channel_id,  b_mode,  ts_num_cur);
			
			sar_rx_ts_cfg_asic_in_chip(chip_id,  add_del_sel,  channel_id,  b_mode,  ts_num);
    	}
		else
		{
			sar_rx_ts_cfg_asic_in_chip( chip_id,  add_del_sel,  channel_id,  b_mode,  ts_num);
		}
    }
    	
	mcmac_tx_traffic_cfg(chip_id,add_del_sel,channel_id,10000);
	
    return rt;
}


/******************************************************************************
*
* FUNCTION 
*
*     sar_rx_ts_cfg_asic_in_chip
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
*      chip_id:chip number used
*      add_del_sel:0:delete 1:add 
*      channel_id:0~79
*      b_mode:0:28*66B,1:29*66B
*      ts_num:number of ts
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-5-29    1.0           initial
*
******************************************************************************/
RET_STATUS sar_rx_ts_cfg_asic_in_chip(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num)
{
    UINT_8 offset = 0;
    UINT_8 index = 0;
    UINT_32 channel_to_write = channel_id;
    UINT_32 joint_init_done = 0;
    UINT_32 parameter = 0;
    RET_STATUS rt = RET_SUCCESS;
    
    if (chip_id > MAX_DEV || add_del_sel > 1|| channel_id > (CH_NUM-1) || b_mode > 1)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,add_del_sel=%d,channel_id=%d,b_mode=%d\r\n",__FUNCTION__,chip_id,add_del_sel,channel_id,b_mode);
        }

        return RET_PARAERR;
    }

    if ((ts_num > 80) || ((ts_num == 0)&& (1 == add_del_sel)))
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid ts!!ts_num=%d\r\n",__FUNCTION__,ts_num);
        }

        return RET_PARAERR;
    }
    
    if (0 == add_del_sel)
    {
        b66sar_rx_ctrl_cfg_set(chip_id,channel_id,MON_EN,0);
        b66sar_rx_ctrl_cfg_set(chip_id,channel_id,CPB2SAR_CH_EN,0);
        /*b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_CHAN_MAP,0x200);*/
        b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_HIGH_CFG,0xd);
        b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_MID_CFG,0x7);
        b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_LOW_CFG,0x3);
       
        b66sar_rx_state_get(chip_id,CPB2SAR_CURRENT_TABLE,&parameter);

        if (parameter == 0)
        {
            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index+1;
                    b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                }
            }

            b66sar_rx_cpb2sar_table_switch_set(chip_id,CPB2SAR_TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                b66sar_rx_state_get(chip_id,CPB2SAR_BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            if (index == 100)
            {
                printf("[%s] error! CPB2SAR_BUSY can't get 0!\r\n",__FUNCTION__);

                #ifndef SV_TEST
                return RET_FAIL;
                #endif
            }

            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index;
                    channel_to_write = 0x7f;
                    b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                }
            }
        }
        else if (parameter == 1)
        {
            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index;
                    b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                }
            }

            b66sar_rx_cpb2sar_table_switch_set(chip_id,CPB2SAR_TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                b66sar_rx_state_get(chip_id,CPB2SAR_BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            if (index == 100)
            {
                printf("[%s] error! CPB2SAR_BUSY can't get 0!\r\n",__FUNCTION__);

                #ifndef SV_TEST
                return RET_FAIL;
                #endif
            }

            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index+1;
                    channel_to_write = 0x7f;
                    b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                }
            }
        }
		
        sar_ts_free(chip_id,channel_id,0);
				
		oam_rx_ts_del(chip_id,channel_id);
    }
    else
    {    
        /*check channel whether used*/
        for (index = 0; index < SAR_TS_NUM; index++)
        {
            if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
            {
                printf("[%s] channel_id = %d has been used!! \r\n",__FUNCTION__,channel_id);
                return RET_FAIL;
            }
        }
        
        rt = sar_ts_allocate(chip_id,channel_id,0,ts_num);

        if (rt == RET_FAIL)
        {
            printf("[%s] ERROR! ts allocation failed! \r\n",__FUNCTION__);
            return RET_FAIL;
        }

        b66sar_rx_state_get(chip_id,CPB2SAR_CURRENT_TABLE,&parameter);

        if (parameter == 0)
        {
            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index+1;
                    b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                }            
            }

            b66sar_rx_cpb2sar_table_switch_set(chip_id,CPB2SAR_TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                b66sar_rx_state_get(chip_id,CPB2SAR_BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            if (index == 100)
            {
                printf("[%s] error! CPB2SAR_BUSY can't get 0!\r\n",__FUNCTION__);

                #ifndef SV_TEST
                return RET_FAIL;
                #endif
            }

            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index;
                    b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                }            
            }            
        }
        else if (parameter == 1)
        {
            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index;
                    b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                }            
            }

            b66sar_rx_cpb2sar_table_switch_set(chip_id,CPB2SAR_TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                b66sar_rx_state_get(chip_id,CPB2SAR_BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            if (index == 100)
            {
                printf("[%s] error! CPB2SAR_BUSY can't get 0!\r\n",__FUNCTION__);

                #ifndef SV_TEST
                return RET_FAIL;
                #endif
            }

            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index+1;
                    b66sar_rx_cpb2sar_ram_set(chip_id,offset,&channel_to_write);
                }            
            }
        }        
    
        /*b66sar_rx_cpb_m_set(chip_id,0+CPB2SAR_M_1ST,0x1312d);*/
        
        b66sar_rx_tx_b66size_29b_set(chip_id,0,b_mode);
        
        b66sar_rx_ctrl_cfg_set(chip_id,channel_id,CPB2SAR_CH_EN,1);
        b66sar_rx_state_get(chip_id,RX_JOINT_INIT_DONE,&joint_init_done);
        if (1 == joint_init_done)
        {
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,MON_EN,1);
        }
        else
        {
            if (module_sar_para_debug)
            {
                printf("[%s] joint init not done!!\r\n",__FUNCTION__);
            }
        }

        /*config sar rx fifo according to ts_num*/
        if (ts_num == 1)
        {
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_HIGH_CFG,5);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_MID_CFG,2);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_LOW_CFG,1);
        }

        if (ts_num == 2)
        {
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_HIGH_CFG,6);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_MID_CFG,3);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_LOW_CFG,2);
        }

        if ((ts_num > 2) && (ts_num <= 5))
        {
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_HIGH_CFG,7);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_MID_CFG,4);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_LOW_CFG,2);
        }

        if ((ts_num > 5) && (ts_num <= 10))
        {
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_HIGH_CFG,8);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_MID_CFG,5);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_LOW_CFG,2);
        }

        if ((ts_num > 10) && (ts_num <= 20))
        {
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_HIGH_CFG,9);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_MID_CFG,6);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_LOW_CFG,3);
        }

        if (ts_num > 20)
        {
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_HIGH_CFG,11);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_MID_CFG,7);
            b66sar_rx_ctrl_cfg_set(chip_id,channel_id,RX_FIFO_LOW_CFG,3);
        }

				
		oam_rx_ts_add(chip_id,channel_id,ts_num*5);
    }
    
	cpb_egress_ch_config(chip_id,add_del_sel,channel_id,ts_num);
	
    return rt;
}


/******************************************************************************
*
* FUNCTION 
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
*      chip_id:chip number used
*      add_del_sel:0:delete 1:add
*      channel_id:0~79
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS sar_tx_ts_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num)
{
    UINT_8 offset = 0;
    UINT_8 index = 0;
    UINT_32 current_table = 0;
    UINT_32 channel_to_write = channel_id;
    RET_STATUS rt = RET_SUCCESS;
    
    if (chip_id > MAX_DEV || add_del_sel > 1 || channel_id > (CH_NUM-1))
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,add_del_sel=%d,channel_id=%d,b_mode=%d\r\n",__FUNCTION__,chip_id,add_del_sel,channel_id,b_mode);
        }

        return RET_PARAERR;
    }

    if (ts_num > 80)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid ts!!ts_num=%d\r\n",__FUNCTION__,ts_num);
        }

        return RET_PARAERR;
    }
    
    if (0 == add_del_sel)
    {
        b66sar_tx_ctrl_cfg_set(chip_id,channel_id,SEG_EN,0);
        b66sar_tx_ctrl_cfg_set(chip_id,channel_id,RA2SAR_CH_EN,0);
        for (index = 0; index < SAR_TS_NUM; index++)
        {
            if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
            {
                offset = 2*index;
                channel_to_write = 0x7f;
                b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                offset = 2*index+1;
                b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
            }
        }
        sar_ts_free(chip_id,channel_id,1);
    }
    else
    {
        /*check channel whether used*/
        for (index = 0; index < SAR_TS_NUM; index++)
        {
            if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
            {
                printf("[%s] channel_id = %d has been used!! \r\n",__FUNCTION__,channel_id);
                return RET_FAIL;
            }
        }
        
        b66sar_tx_state_get(chip_id,RA2SAR_CURRENT_TABLE,&current_table);
        
        rt = sar_ts_allocate(chip_id,channel_id,1,ts_num);
        
        for (index = 0; index < SAR_TS_NUM; index++)
        {
            if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
            {
                offset = 2*index;
                b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                offset = 2*index+1;
                b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
            }            
        }

        
        b66sar_tx_ra_m_set(chip_id,0+RA2SAR_M_1ST,0x3d09);
        b66sar_tx_ra_m_set(chip_id,1+RA2SAR_BASE_M_1ST,0xffffc389);
        b66sar_tx_ra_m_set(chip_id,2+RA2SAR_M_2ND,0x4feac05);
        b66sar_tx_ra_m_set(chip_id,3+RA2SAR_BASE_M_2ND,0xffffdc05);
        b66sar_tx_ra2sar_cfg_311m_set(chip_id,RA2SAR_MULTIPLIER,2);
        b66sar_tx_ra2sar_cfg_311m_set(chip_id,RA2SAR_BYPASS_M_N_2ND,0);
        b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_CFG_PERIOD,0x7d7);
        b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_NOM_CNT,0x741);
        b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_Q+2,32);
        b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_ADJ_01EN+2,0);
        b66sar_tx_ra2sar_cfg3_set(chip_id,RA2SAR_LOCAL_EN,1);
        b66sar_tx_ra2sar_cfg3_set(chip_id,RA2SAR_LOCAL_M,527);
        b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_ADJ_IN_CHECK_VALUE+2,100);
        b66sar_tx_ra2sar_cfg_set(chip_id,RA2SAR_ADJ_IN_CORRECT_EN+2,1);
        b66sar_rx_tx_b66size_29b_set(chip_id,1,b_mode);
        b66sar_tx_ctrl_cfg_set(chip_id,channel_id,RA2SAR_CH_EN,1);
        b66sar_tx_ctrl_cfg_set(chip_id,channel_id,SEG_EN,1);
    }

    return rt;
}

/******************************************************************************
*
* FUNCTION 
*
*     sar_tx_ts_cfg_asic
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
*      chip_id:chip number used
*      add_del_sel:0:delete 1:add
*      channel_id:0~79
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-5-29    1.0           initial
*
******************************************************************************/
RET_STATUS sar_tx_ts_cfg_asic(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num)
{
    UINT_8 index = 0;
	UINT_8 ts_num_cur = 0;
    RET_STATUS rt = RET_SUCCESS;
    
    if (chip_id > MAX_DEV || add_del_sel > 1 || channel_id > (CH_NUM-1))
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,add_del_sel=%d,channel_id=%d,b_mode=%d\r\n",__FUNCTION__,chip_id,add_del_sel,channel_id,b_mode);
        }

        return RET_PARAERR;
    }

    if ( (ts_num > 80) || ((ts_num == 0) && (add_del_sel == 1)))
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid ts!!ts_num=%d\r\n",__FUNCTION__,ts_num);
        }

        return RET_PARAERR;
    }

	/* get the tx ts number of this channel */
	for (index = 0; index < SAR_TS_NUM; index++)
    {
        if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
        {
        	ts_num_cur++;
        }
	}

	if ((0 == add_del_sel)&&(0 == ts_num_cur))
	{
		printf("[%s] channel=%d of sar tx has not been configed\r\n",__FUNCTION__,channel_id);

		return RET_SUCCESS;
	}

	if ((1 == add_del_sel)&&(ts_num == ts_num_cur))
	{
		printf("[%s] sar tx channel=%d has been configed,don't config the same\r\n",__FUNCTION__,channel_id);

		return RET_SUCCESS;
	}

	if (0 == add_del_sel)
	{
		sar_tx_ts_cfg_asic_in_chip(chip_id,  add_del_sel,  channel_id,  b_mode,  ts_num);
	}
    else
    {
    	if (0 != ts_num_cur)
    	{
    		sar_tx_ts_cfg_asic_in_chip(chip_id,  0,  channel_id,  b_mode,  ts_num_cur);			

			sar_tx_ts_cfg_asic_in_chip(chip_id,  add_del_sel,  channel_id,  b_mode,  ts_num);
    	}
		else
		{
			sar_tx_ts_cfg_asic_in_chip(chip_id,  add_del_sel,  channel_id,  b_mode,  ts_num);
		}
    }

	mcmac_rx_traffic_cfg(chip_id,add_del_sel,channel_id,10000);
	
    return rt;
}


/******************************************************************************
*
* FUNCTION 
*
*     sar_tx_ts_cfg_asic_in_chip
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
*      chip_id:chip number used
*      add_del_sel:0:delete 1:add
*      channel_id:0~79
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-5-29    1.0           initial
*
******************************************************************************/
RET_STATUS sar_tx_ts_cfg_asic_in_chip(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num)
{
    UINT_8 offset = 0;
    UINT_8 index = 0;
    UINT_32 channel_to_write = channel_id;
    UINT_32 parameter = 0;
    RET_STATUS rt = RET_SUCCESS;
    
    if (chip_id > MAX_DEV || add_del_sel > 1 || channel_id > (CH_NUM-1))
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,add_del_sel=%d,channel_id=%d,b_mode=%d\r\n",__FUNCTION__,chip_id,add_del_sel,channel_id,b_mode);
        }

        return RET_PARAERR;
    }

    if ( (ts_num > 80) || ((ts_num == 0) && (add_del_sel == 1)))
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid ts!!ts_num=%d\r\n",__FUNCTION__,ts_num);
        }

        return RET_PARAERR;
    }
    
    if (0 == add_del_sel)
    {
        b66sar_tx_ctrl_cfg_set(chip_id,channel_id,SEG_EN,0);
        b66sar_tx_ctrl_cfg_set(chip_id,channel_id,RA2SAR_CH_EN,0);

        b66sar_tx_state_get(chip_id,RA2SAR_CURRENT_TABLE,&parameter);
        if (parameter == 0)
        {
            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index+1;
                    b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                }
            }

            b66sar_tx_ra2sar_table_switch_set(chip_id,RA2SAR_TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                b66sar_tx_state_get(chip_id,RA2SAR_BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            if (index == 100)
            {
                printf("[%s] error! RA2SAR_BUSY can't get 0!\r\n",__FUNCTION__);

                #ifndef SV_TEST
                return RET_FAIL;
                #endif
            }

            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index;
                    b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                }
            }            
        }
        else if(parameter == 1)
        {
            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index;
                    b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                }
            }

            b66sar_tx_ra2sar_table_switch_set(chip_id,RA2SAR_TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                b66sar_tx_state_get(chip_id,RA2SAR_BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            if (index == 100)
            {
                printf("[%s] error! RA2SAR_BUSY can't get 0!\r\n",__FUNCTION__);

                #ifndef SV_TEST
                return RET_FAIL;
                #endif
            }

            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index+1;
                    b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                }
            }    
        }
        sar_ts_free(chip_id,channel_id,1);
    }
    else
    {
        /*check channel whether used*/
        for (index = 0; index < SAR_TS_NUM; index++)
        {
            if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
            {
                printf("[%s] channel_id = %d has been used!! \r\n",__FUNCTION__,channel_id);
                return RET_FAIL;
            }
        }
        
        rt = sar_ts_allocate(chip_id,channel_id,1,ts_num);

        if (rt == RET_FAIL)
        {
            printf("[%s] ERROR! ts allocation failed! \r\n",__FUNCTION__);
            return RET_FAIL;
        }

        b66sar_tx_state_get(chip_id,RA2SAR_CURRENT_TABLE,&parameter);
        if (parameter == 0)
        {
            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index+1;
                    b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                }            
            }

            b66sar_tx_ra2sar_table_switch_set(chip_id,RA2SAR_TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                b66sar_tx_state_get(chip_id,RA2SAR_BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            if (index == 100)
            {
                printf("[%s] error! RA2SAR_BUSY can't get 0!\r\n",__FUNCTION__);

                #ifndef SV_TEST
                return RET_FAIL;
                #endif
            }

            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index;
                    b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                }            
            }
        }
        else if (parameter == 1)
        {
            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index;
                    b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                }            
            }

            b66sar_tx_ra2sar_table_switch_set(chip_id,RA2SAR_TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                b66sar_tx_state_get(chip_id,RA2SAR_BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            if (index == 100)
            {
                printf("[%s] error! RA2SAR_BUSY can't get 0!\r\n",__FUNCTION__);

                #ifndef SV_TEST
                return RET_FAIL;
                #endif
            }

            for (index = 0; index < SAR_TS_NUM; index++)
            {
                if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
                {
                    offset = 2*index+1;
                    b66sar_tx_ra2sar_ram_set(chip_id,offset,&channel_to_write);
                }            
            }
        }
        
        b66sar_rx_tx_b66size_29b_set(chip_id,1,b_mode);
        b66sar_tx_ctrl_cfg_set(chip_id,channel_id,RA2SAR_CH_EN,1);
        b66sar_tx_ctrl_cfg_set(chip_id,channel_id,SEG_EN,1);
    }

	mac_rateadp_traffic_cfg(chip_id,add_del_sel,channel_id,ts_num);
	
    return rt;
}

/******************************************************************************
*
* FUNCTION 
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS sar_switch_table(UINT_8 chip_id)
{
    UINT_32 busy_state = 0;
    
    b66sar_tx_state_get(chip_id,RA2SAR_BUSY,&busy_state);

    if (0 == busy_state)
    {
        b66sar_tx_ra2sar_table_switch_set(chip_id,0);
    }
    else
    {
        if (module_sar_para_debug)
        {
            printf("[%s] busy now!!\r\n",__FUNCTION__);
        }
        
        return RET_FAIL;
    }

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION 
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS sar_ts_allocate(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction,UINT_8 ts_num)
{
    UINT_8 index = 0;
    UINT_8 ts_cnt = 0;
    RET_STATUS rt = RET_FAIL;
    
    if (chip_id > MAX_DEV || direction > 1 || channel_id > (CH_NUM-1) || ts_num > 80)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d,direction=%d,ts_num=%d",__FUNCTION__,chip_id,channel_id,direction,ts_num);
        }

        return RET_PARAERR;
    }

    if (0 == direction)
    {
        for (index = 0;index < SAR_TS_NUM;index++)
        {
            if (0xff == g_sar_rx_channel_of_ts[chip_id][index])
            {
                g_sar_rx_channel_of_ts[chip_id][index] = channel_id;
                ts_cnt++;
            }
            
            if (ts_cnt == ts_num)
            {    
                rt = RET_SUCCESS;
                break;
            }
        }
    }
    else
    {
        for (index = 0;index < SAR_TS_NUM;index++)
        {
            if (0xff == g_sar_tx_channel_of_ts[chip_id][index])
            {
                g_sar_tx_channel_of_ts[chip_id][index] = channel_id;
                ts_cnt++;
            }
            
            if (ts_cnt == ts_num)
            {    
                rt = RET_SUCCESS;
                break;
            }
        }
    }

    return rt;
}


/******************************************************************************
*
* FUNCTION sar_ts_free
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    jxma        2018-1-2    1.0           initial
*
******************************************************************************/
RET_STATUS sar_ts_free(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction)
{
    UINT_8 index = 0;

    if (chip_id > MAX_DEV || direction > 1 || channel_id > (CH_NUM-1))
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d,direction=%d\r\n",__FUNCTION__,chip_id,channel_id,direction);
        }

        return RET_PARAERR;
    }

    if (0 == direction)
    {
        for (index = 0;index < SAR_TS_NUM;index++)
        {
            if (channel_id == g_sar_rx_channel_of_ts[chip_id][index])
            {
                g_sar_rx_channel_of_ts[chip_id][index] = 0xff;
            }
        }
    }
    else
    {
        for (index = 0;index < SAR_TS_NUM;index++)
        {
            if (channel_id == g_sar_tx_channel_of_ts[chip_id][index])
            {
                g_sar_tx_channel_of_ts[chip_id][index] = 0xff;
            }
        }
    }
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*   sar_rx_chan_map_cfg  
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
*      chip_id:chip number used
*      add_del:0:del 1:add
*      ch_id:0-79
*      bcm_id:0-255
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-6-28    1.0           initial
*
******************************************************************************/
RET_STATUS sar_rx_chan_map_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 ch_id,UINT_32 bcm_id)
{
    if (chip_id > MAX_DEV || add_del > 1 || ch_id > (CH_NUM-1) || bcm_id > 512)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,add_del=%d,ch_id=%d,bcm_id=%d\r\n",
                __FUNCTION__,chip_id,add_del,ch_id,bcm_id);
        }

        return RET_PARAERR;
    }

    if (add_del == 0)
    {
        b66sar_rx_ctrl_cfg_set(chip_id,ch_id,MON_EN,0);
        b66sar_rx_ctrl_cfg_set(chip_id,ch_id,RX_CHAN_MAP,0x200);
    }
    else if (add_del == 1)
    {
        b66sar_rx_ctrl_cfg_set(chip_id,ch_id,RX_CHAN_MAP,bcm_id);
        b66sar_rx_ctrl_cfg_set(chip_id,ch_id,MON_EN,1);
    }

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*   sar_tx_chan_map_cfg  
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
*      chip_id:chip number used
*      add_del:0:del 1:add
*      ch_id:0-79
*      bcm_id:0-255
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-6-28    1.0           initial
*
******************************************************************************/
RET_STATUS sar_tx_chan_map_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 ch_id,UINT_32 bcm_id)
{
    if (chip_id > MAX_DEV || add_del > 1 || ch_id > (CH_NUM-1) || bcm_id > 512)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,add_del=%d,ch_id=%d,bcm_id=%d\r\n",
                __FUNCTION__,chip_id,add_del,ch_id,bcm_id);
        }

        return RET_PARAERR;
    }

    if (add_del == 0)
    {
        b66sar_tx_ctrl_cfg_set(chip_id,ch_id,SEG_EN,0);
        b66sar_tx_ctrl_cfg_set(chip_id,ch_id,TX_CHAN_MAP,0x200);
    }
    else if (add_del == 1)
    {
        b66sar_tx_ctrl_cfg_set(chip_id,ch_id,TX_CHAN_MAP,bcm_id);
        b66sar_tx_ctrl_cfg_set(chip_id,ch_id,SEG_EN,1);
    }

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*   sar_lookback_mode_cfg  
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
*      chip_id:chip number used
*      mode:0:sar_rx_loopback  1:sar_tx_lookback  2:sar_tx_bdcome_loopback
*      loopback_en: 0:no loopback 1:loopback
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-6-28    1.0           initial
*
******************************************************************************/
RET_STATUS sar_loopback_mode_cfg(UINT_8 chip_id,UINT_8 mode)
{
    if (chip_id > MAX_DEV || mode > 3)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,mode=%d\r\n",
                __FUNCTION__,chip_id,mode);
        }

        return RET_PARAERR;
    }

    switch (mode)
    {
        case 0:
            b66sar_rx_loopback_set(chip_id,0);
            b66sar_tx_loopback_set(chip_id,0);
            b66sar_tx_bdcome_loopback_set(chip_id,0);
            
            printf("\r\n[%s] no loopback all \r\n",__FUNCTION__);
            break;
            
        case 1:
            b66sar_rx_loopback_set(chip_id,1);
            b66sar_tx_loopback_set(chip_id,0);
            b66sar_tx_bdcome_loopback_set(chip_id,0);
            printf("\r\n[%s] cfg sar_tx_to_rx_loopback\r\n",__FUNCTION__);
            break;
        case 2:
            b66sar_rx_loopback_set(chip_id,0);
            b66sar_tx_loopback_set(chip_id,1);
            b66sar_tx_bdcome_loopback_set(chip_id,0);
            printf("\r\n[%s] cfg sar_rx_to_tx_loopback\r\n",__FUNCTION__);
            break;
        case 3:
            b66sar_rx_loopback_set(chip_id,0);
            b66sar_tx_loopback_set(chip_id,0);
            b66sar_tx_bdcome_loopback_set(chip_id,1);
            printf("\r\n[%s] cfg sar_tx_bdcome_loopback\r\n",__FUNCTION__);
            break;
            
        default:
            break;
    }

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION
*
*     sar debug function.
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
*      kejian        2018-03-27         1.0            initial
*
******************************************************************************/
RET_STATUS sar_debug(void)
{
    printf("+----------------------sar/sar rx/sar tx initialized function---------------------------------------+\r\n");
    printf("sar_init(UINT_8 chip_id)\r\n");
    printf("sar_rx_init(UINT_8 chip_id)\r\n");
    printf("sar_tx_init(UINT_8 chip_id)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------sar's rx/tx ts_cfg function--------------------------------------------------+\r\n");
    printf("sar_rx_ts_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num)\r\n");
    printf("sar_tx_ts_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("add_del_sel: delete/add ts operation select,0---delete ts,1---add ts.\r\n");
    printf("channel_id: channel_id, range is 0~79.\r\n");
    printf("b_mode: 66b_block mode,0---sar package load 28*66b_blocks,1---29*66b_blocks.\r\n");
    printf("ts_num:number of ts,if add_del_sel was 1,indicate the number of ts will be allocated for the channel.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------sar_switch_table function----------------------------------------------------+\r\n");
    printf("sar_switch_table(UINT_8 chip_id)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------sar_ts_allocate function-----------------------------------------------------+\r\n");
    printf("sar_ts_allocate(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction,UINT_8 ts_num)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("channel_id: channel_id, range is 0~79.\r\n");
    printf("direction: 0---rx,1---tx.\r\n");
    printf("ts_num: number of ts,indicate the number of ts will be allocated for the channel.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------sar_ts_free function---------------------------------------------------------+\r\n");
    printf("sar_ts_free(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("channel_id: channel_id, range is 0~79.\r\n");
    printf("direction: 0---rx,1---tx.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    printf("+----------------------sar_dump function------------------------------------------------------------+\r\n");
    printf("sar_dump(UINT_8 chip_id)\r\n");
    printf("\r\n");

    printf("parameter description:\r\n");
    printf("chip_id: chip number used.\r\n");
    printf("+---------------------------------------------------------------------------------------------------+\r\n");
    printf("\r\n\r\n");

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION
*
*     dump sar'ts info.
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
*    <author>        <date>            <CR_ID>         <DESCRIPTION>
*      kejian        2018-03-28         1.0            initial
*
******************************************************************************/
RET_STATUS sar_dump(UINT_8 chip_id)
{
    UINT_8 ts_index = 0;    
    UINT_8 index = 0;
    
    /* check unit whether normal */
    if (chip_id > (DEV_NUM - 1)) 
    {
        printf("%s: invalid unit = 0x%x...\r\n", __FUNCTION__, chip_id);
        
        return RET_PARAERR;
    }

    
    printf("\r\n");
    printf("********** chip %d's sar_channel_ts info **********\r\n", chip_id);
    printf("\r\n");

    printf("++++++++++++++++++++++ sar_rx_channel_of_ts info ++++++++++++++++++++++\r\n");
    for (ts_index = 0; ts_index < SAR_TS_NUM;) 
    {
        printf("          %s", bit_start);
        for (index = ts_index; index < (((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM) - 2); index++) 
        {
            printf("%s", bit_middle);
        }
        printf("%s\r\n", bit_end);


        printf("ts_index  ");
        for (index = ts_index; index < ((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM); index++) 
        {
            printf("%3d|", (int)index);
        }
        printf("\r\n");
        
        printf("          %s", bit_start);
        for (index = ts_index; index < (((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM) - 2); index++) 
        {
            printf("%s", bit_middle);
        }
        printf("%s\r\n", bit_end);

        printf("chnl_id   ");
        for (index = ts_index; index < ((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM); index++) 
        {
            printf("%3d|",(int)(g_sar_rx_channel_of_ts[chip_id][index]));
        }
        printf("\r\n");

        printf("          %s", bit_start);
        for (index = ts_index; index < (((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM) - 2); index++)
        {
            printf("%s", bit_middle);
        }
        printf("%s\r\n", bit_end);

        if (ts_index + REG_WIDTH > SAR_TS_NUM)
            break;
        
        ts_index += REG_WIDTH;
    }
    printf("\r\n");

    printf("++++++++++++++++++++++ sar_tx_channel_of_ts info ++++++++++++++++++++++\r\n");
    for (ts_index = 0; ts_index < SAR_TS_NUM;) 
    {
        printf("          %s", bit_start);
        for (index = ts_index; index < (((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM) - 2); index++) 
        {
            printf("%s", bit_middle);
        }
        printf("%s\r\n", bit_end);

        printf("ts_index  ");
        for (index = ts_index; index < ((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM); index++) 
        {
            printf("%3d|", (int)index);
        }
        printf("\r\n");
        
        printf("          %s", bit_start);
        for (index = ts_index; index < (((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM) - 2); index++) 
        {
            printf("%s", bit_middle);
        }
        printf("%s\r\n", bit_end);

        printf("chnl_id   ");
        for (index = ts_index; index < ((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM); index++) 
        {
            printf("%3d|",(int)(g_sar_tx_channel_of_ts[chip_id][index]));
        }
        printf("\r\n");

        printf("          %s", bit_start);
        for (index = ts_index; index < (((ts_index + REG_WIDTH) < SAR_TS_NUM ? (ts_index + REG_WIDTH) : SAR_TS_NUM) - 2); index++)
        {
            printf("%s", bit_middle);
        }
        printf("%s\r\n", bit_end);

        if (ts_index + REG_WIDTH > SAR_TS_NUM)
            break;
        
        ts_index += REG_WIDTH;
    }
    printf("\r\n");    

    return RET_SUCCESS;
    
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_env_cfg
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
*      chip_id:chip number used
*      grp_id:0~9
*      flexe_env_mode: 0:50GE_50INSTANCE  1:100GE_50INSTANCE  2:200GE_50INSTANCE
*                      3:400GE_50INSTANCE    4:50GE_50GE
*                      5:100GE_100GE  6:200GE_200GE  7:400GE_400GE  
*                      8:_50GE_LOCAL  9:_100GE_LOCAL  10:_200GE_LOCAL
*                      11:_400GE_LOCAL  12:_50G_INSTANCE_LOCAL
*
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-5-29    1.0           initial
*
******************************************************************************/
RET_STATUS flexe_env_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 grp_id,flexe_env_mode mode)
{
    if (chip_id > MAX_DEV || add_del > 1 || grp_id > 7)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,add_del=%d,grp_id=%d\r\n",__FUNCTION__,chip_id,add_del,grp_id);
        }

        return RET_PARAERR;
    }

    if (add_del == 1)
    {
        flexe_env_cfg2_set(chip_id,FLEXE_ENV_Q,grp_id,32);
        flexe_env_cfg2_set(chip_id,ADJ_01EN,grp_id,0);
        flexe_env_cfg2_set(chip_id,FIFO_ADJ_EN,grp_id,1);
        flexe_env_cfg2_set(chip_id,FIFO_ADJ_PERIOD,grp_id,0);
        flexe_env_cfg2_set(chip_id,FIFO_ADJ_MAX_EN,grp_id,1);
        flexe_env_cfg3_set(chip_id,ADJ_IN_CHECK_VALUE,grp_id,100);
        flexe_env_cfg3_set(chip_id,ADJ_IN_CORRECT_EN,grp_id,1);
        flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_EN,grp_id,1);

        switch (mode)
        {
            case _50GE_50INSTANCE:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,10767);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,1);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,2028);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,499);
                flexe_env_m_cfg_set(chip_id,grp_id,0xC7FCE000);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0xCE3B5F42);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,0);
                break;
            case _100GE_50INSTANCE:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,10767);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,0);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,2028);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,499);
                flexe_env_m_cfg_set(chip_id,grp_id,0xCCFD4000);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0xCCFD8B2F);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,0);
                break;
            case _200GE_50INSTANCE:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,10767);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,0);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,2028);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,499);
                flexe_env_m_cfg_set(chip_id,grp_id,0x63FE7000);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0x6A3CEF42);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,0);
                break;
            case _400GE_50INSTANCE:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,10767);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,0);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,2028);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,499);
                flexe_env_m_cfg_set(chip_id,grp_id,0x31FF3800);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0x383DB742);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,0);
                break;
            case _50GE_50GE:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,10097);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,1);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,1900);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,531);
                flexe_env_m_cfg_set(chip_id,grp_id,0xCCCCCCCC);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0xCCCCCCCD);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,0);
                break;
            case _100GE_100GE:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,5014);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,1);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,1887);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,521);
                flexe_env_m_cfg_set(chip_id,grp_id,0xCCCCCCCC);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0xCCCCCCCD);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,0);
                break;
            case _200GE_200GE:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,2511);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,1);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,1890);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,556);
                flexe_env_m_cfg_set(chip_id,grp_id,0xCCCCCCCC);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0xCCCCCCCD);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,0);
                break;
            case _400GE_400GE:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,2511);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,0);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,1890);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,556);
                flexe_env_m_cfg_set(chip_id,grp_id,0xCCCCCCCC);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0xCCCCCCCD);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,0);
                break;
            case _50GE_LOCAL:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,10097);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,0);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,1900);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,531);
                flexe_env_m_cfg_set(chip_id,grp_id,0x302F55CD);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0x3030AACD);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,1);
                break;
            case _100GE_LOCAL:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,5014);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,0);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,1887);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,521);
                flexe_env_m_cfg_set(chip_id,grp_id,0x605EBE7F);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0x605F147F);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,1);
                break;
            case _200GE_LOCAL:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,2511);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,0);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,1890);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,556);
                flexe_env_m_cfg_set(chip_id,grp_id,0xC0BE4731);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0xC0BE5D71);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,1);
                break;
            case _400GE_LOCAL:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,2511);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,0);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,1890);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,556);
                flexe_env_m_cfg_set(chip_id,grp_id,0xC0BE4731);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0xC0BE5D71);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,1);
                break;
            case _50G_INSTANCE_LOCAL:
                flexe_env_cfg1_set(chip_id,CFG_PERIOD,grp_id,10767);
                flexe_env_cfg1_set(chip_id,MULTIPLIER,grp_id,0);
                flexe_env_cfg1_set(chip_id,NOM_CNT,grp_id,2028);
                flexe_env_cfg3_set(chip_id,FLEXE_LOCAL_M,grp_id,499);
                flexe_env_m_cfg_set(chip_id,grp_id,0x303A3F14);
                flexe_env_base_m_cfg_set(chip_id,grp_id,0x303DA985);
                flexe_env_cfg2_set(chip_id,IS_LOCAL,grp_id,1);
                break;                
            default:
                return RET_PARAERR;
        }

        flexe_env_cfg2_set(chip_id,FLEXE_ENV_EN,grp_id,1);
    }
    else if (add_del == 0)
    {
        flexe_env_cfg2_set(chip_id,FLEXE_ENV_EN,grp_id,0);
    }
    
    return RET_SUCCESS;    
}

/******************************************************************************
*
* FUNCTION 
*
*    client_env_init     
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-5-31    1.0           initial
*
******************************************************************************/
RET_STATUS client_env_init(UINT_8 chip_id)
{
    UINT_8 index = 0;
    UINT_8 ts_id;
    UINT_32 req_gen_value = 0x7f;
    UINT_32 parameter = 0;

    req_gen_plus_set(chip_id,MAC_ENV_INIT);

    for (index = 0; index < 100; index++)
    {
        parameter = 0;
        req_gen_state_get(chip_id,MAC_ENV_INIT_DONE,&parameter);

        if (parameter == 1)
        {
            break;
        }
    }
    if (!SAL_BOOT_SIMULATION)
    {
        if (index == 100)
        {
            printf("[%s] error! MAC_ENV_INIT_DONE can't get 1!\r\n",__FUNCTION__);
            return RET_FAIL;
        }
    }
    
    /*step1:init g_client_channel_of_ts*/
    for(ts_id = 0; ts_id < 81;ts_id++)
    {
        g_client_channel_of_ts[chip_id][ts_id] = 0xff;
    }
    
    /*step2:init req_gen_ram*/
    for(index = 0; index < 162; index++)
    {    
        req_gen_ram_set(chip_id,index,&req_gen_value);
    }
    
	req_gen_value = 0x50;
	req_gen_ram_set(chip_id,160,&req_gen_value);
	req_gen_ram_set(chip_id,161,&req_gen_value);
	
    /*step3:calendar_en */
    req_gen_cfg_set(chip_id,CALENDAR_EN,1);
    req_gen_cfg_set(chip_id,OHIF_1588_RATE_LIMIT_EN,1);
    
    /* req_m base_m*/
    req_gen_set(chip_id,0xC3298F74);
    req_gen_base_set(chip_id,0xC329901F);

    /*ohif_1588_src_dst_reg*/
    req_gen_ohif_1588_src_dst_set(chip_id,EXTRA_SRC,80);
    req_gen_ohif_1588_src_dst_set(chip_id,EXTRA_DST0,80);
    req_gen_ohif_1588_src_dst_set(chip_id,EXTRA_DST1,81);

    /*ohif_1588_ratio_reg limit_ratio*/
    req_gen_ohif_1588_ratio_set(chip_id,853);
    req_gen_ohif_1588_rate_limit_ratio_set(chip_id,853);

    /*ohif_1588_rate_limit_m_reg base_m*/
    req_gen_ohif_1588_rate_limit_m_set(chip_id,0xE74DB38);
    req_gen_ohif_1588_rate_limit_base_m_set(chip_id,0xE74DBE3);

    /*extra_m_n*/
    req_gen_extra_m_n_set(chip_id,EXTRA_M,39321);
    req_gen_extra_m_n_set(chip_id,EXTRA_BASE_M,39322);
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*    client_ts_allocate     
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
*      chip_id:chip number used
*      channel_id: 0~81
*     ts_num:1~84
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-5-31    1.0           initial
*
******************************************************************************/
RET_STATUS client_ts_allocate(UINT_8 chip_id,UINT_8 channel_id,UINT_8 ts_num)
{
    UINT_8 index = 0;
    UINT_8 ts_cnt = 0;
    RET_STATUS rt = RET_FAIL;

    if (chip_id > MAX_DEV || channel_id > 81 || ts_num > 81)
    {
        if (module_sar_para_debug)
        {
            printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d,ts_num=%d",__FUNCTION__,chip_id,channel_id,ts_num);
        }

        return RET_PARAERR;
    }
    
    for (index = 0;index < 81;index++)
    {
        if (0xff == g_client_channel_of_ts[chip_id][index])
        {
            g_client_channel_of_ts[chip_id][index] = channel_id;
            ts_cnt++;
        }
            
        if (ts_cnt == ts_num)
        {    
            rt = RET_SUCCESS;
            break;
        }
    }

    return rt;
}


/******************************************************************************
*
* FUNCTION
*
*    client_env_ts_cfg_del
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
*      chip_id:chip number used
*      channel_id: 0~81
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-5-31    1.0           initial
*
******************************************************************************/
RET_STATUS client_env_ts_cfg_del(UINT_8 chip_id,UINT_8 channel_id)
{
    UINT_8 offset = 0;
    UINT_8 index = 0;
    UINT_32 channel_to_write = channel_id;
    UINT_32 parameter = 2;

    req_gen_state_get(chip_id,CURRENT_TABLE,&parameter);
    if (parameter == 0)
    {
        for (index = 0; index < 81; index++)
        {
            if (channel_id == g_client_channel_of_ts[chip_id][index])
            {
                channel_to_write = 0x7f;
                offset = 2*index+1;
                req_gen_ram_set(chip_id,offset,&channel_to_write);
            }
        }

        req_gen_plus_set(chip_id,TABLE_SWITCH);

        for (index = 0; index < 100; index++)
        {
            parameter = 2;
            req_gen_state_get(chip_id,BUSY,&parameter);
            if (parameter == 0)
            {
                break;
            }
        }

        for (index = 0; index < 81; index++)
        {
            if (channel_id == g_client_channel_of_ts[chip_id][index])
            {
                channel_to_write = 0x7f;
                offset = 2*index;
                req_gen_ram_set(chip_id,offset,&channel_to_write);
            }
        }
    }
    else if (parameter == 1)
    {
        for (index = 0; index < 81; index++)
        {
            if (channel_id == g_client_channel_of_ts[chip_id][index])
            {
                channel_to_write = 0x7f;
                offset = 2*index;
                req_gen_ram_set(chip_id,offset,&channel_to_write);
            }
        }

        req_gen_plus_set(chip_id,TABLE_SWITCH);

        for (index = 0; index < 100; index++)
        {
            parameter = 2;
            req_gen_state_get(chip_id,BUSY,&parameter);
            if (parameter == 0)
            {
                break;
            }
        }

        for (index = 0; index < 81; index++)
        {
            if (channel_id == g_client_channel_of_ts[chip_id][index])
            {
                channel_to_write = 0x7f;
                offset = 2*index+1;
                req_gen_ram_set(chip_id,offset,&channel_to_write);
            }
        }
    }

    for (index = 0;index < 81;index++)
    {
        if (channel_id == g_client_channel_of_ts[chip_id][index])
        {
            g_client_channel_of_ts[chip_id][index] = 0xff;
        }
    }

    return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION 
*
*    client_env_ts_cfg     
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
*      chip_id:chip number used
*      add_del_sel: 0: del  1:add
*      channel_id: 0~81
*     ts_num:1~84
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-5-31    1.0           initial
*
******************************************************************************/
RET_STATUS client_env_ts_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_8 ts_num)
{
    UINT_8 offset = 0;
    UINT_8 index = 0;
    UINT_32 channel_to_write = channel_id;
    UINT_32 parameter = 2;
    UINT_8 ts_num_cur = 0;
    RET_STATUS rt = RET_SUCCESS;

	if (81 == channel_id)
	{
		printf("\r\n [%s] the channel for 1588 does not need to config calendar,it has been configured in init\r\n",__FUNCTION__);
		
		return RET_SUCCESS;
	}
    if (chip_id > MAX_DEV || add_del_sel > 1|| channel_id > 81 || ts_num > 81)
    {        
        printf("\r\n [%s] invalid parameter!!chip_id=%d,add_del_sel=%d,channel_id=%d,ts_num=%d\r\n",
            __FUNCTION__,chip_id,add_del_sel,channel_id,ts_num);

        return RET_PARAERR;
    }
    
    if (0 == add_del_sel)
    {    
        req_gen_state_get(chip_id,CURRENT_TABLE,&parameter);

        if (parameter == 0)
        {
            for (index = 0; index < 81; index++)
            {
                if (channel_id == g_client_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index+1;
                    req_gen_ram_set(chip_id,offset,&channel_to_write);
                }
            }

            req_gen_plus_set(chip_id,TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                parameter = 2;
                req_gen_state_get(chip_id,BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            for (index = 0; index < 81; index++)
            {
                if (channel_id == g_client_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index;
                    req_gen_ram_set(chip_id,offset,&channel_to_write);
                }
            }
        }
        else if (parameter == 1)
        {
            for (index = 0; index < 81; index++)
            {
                if (channel_id == g_client_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index;
                    req_gen_ram_set(chip_id,offset,&channel_to_write);
                }
            }

            req_gen_plus_set(chip_id,TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                parameter = 2;
                req_gen_state_get(chip_id,BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            for (index = 0; index < 81; index++)
            {
                if (channel_id == g_client_channel_of_ts[chip_id][index])
                {
                    channel_to_write = 0x7f;
                    offset = 2*index+1;
                    req_gen_ram_set(chip_id,offset,&channel_to_write);
                }
            }
        }
        
        for (index = 0;index < 81;index++)
        {
            if (channel_id == g_client_channel_of_ts[chip_id][index])
            {
                g_client_channel_of_ts[chip_id][index] = 0xff;
            }
        }
    }
    else if (1 == add_del_sel)
    {    
    if (0 == ts_num)
    {
        printf("\r\n [%s]:%d the ts number equals to zero when add!!chip_id=%d,add_del_sel=%d,channel_id=%d,ts_num=%d\r\n",
         __FUNCTION__,__LINE__,chip_id,add_del_sel,channel_id,ts_num);

        return RET_PARAERR;
    }

    for(index = 0; index < 81;index++)
    {
        if (channel_id == g_client_channel_of_ts[chip_id][index])
        {
            ts_num_cur++;
        }
    }

    if (ts_num_cur != 0)
    {
        if (ts_num == ts_num_cur)
        {
            printf("[%s] channel_id = %d has been used and the same parameter add again!! \r\n",__FUNCTION__,channel_id);
            return RET_SUCCESS;
        }
        else
        {
            client_env_ts_cfg_del(chip_id, channel_id);
        }
    }

        rt = client_ts_allocate(chip_id,channel_id,ts_num);

        if (rt == RET_FAIL)
        {
            printf("[%s] ERROR! ts allocation failed! \r\n",__FUNCTION__);
            return RET_FAIL;
        }

        req_gen_state_get(chip_id,CURRENT_TABLE,&parameter);

        if (parameter == 0)
        {
            for (index = 0; index < 81; index++)
            {
                if (channel_id == g_client_channel_of_ts[chip_id][index])
                {
                    offset = 2*index+1;
                    req_gen_ram_set(chip_id,offset,&channel_to_write);
                }            
            }

            req_gen_plus_set(chip_id,TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                parameter = 2;
                req_gen_state_get(chip_id,BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            for (index = 0; index < 81; index++)
            {
                if (channel_id == g_client_channel_of_ts[chip_id][index])
                {
                    offset = 2*index;
                    req_gen_ram_set(chip_id,offset,&channel_to_write);
                }            
            }            
        }
        else if (parameter == 1)
        {
            for (index = 0; index < 81; index++)
            {
                if (channel_id == g_client_channel_of_ts[chip_id][index])
                {
                    offset = 2*index;
                    req_gen_ram_set(chip_id,offset,&channel_to_write);
                }            
            }

            req_gen_plus_set(chip_id,TABLE_SWITCH);

            for (index = 0; index < 100; index++)
            {
                parameter = 2;
                req_gen_state_get(chip_id,BUSY,&parameter);
                if (parameter == 0)
                {
                    break;
                }
            }

            for (index = 0; index < 81; index++)
            {
                if (channel_id == g_client_channel_of_ts[chip_id][index])
                {
                    offset = 2*index+1;
                    req_gen_ram_set(chip_id,offset,&channel_to_write);
                }            
            }
        }        
    }

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     inf_ch_adp_rx_init
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-6-15    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_rx_init(UINT_8 chip_id)
{
    UINT_8 index = 0;
    UINT_8 port_id = 0;

    /*step1:init port state    */
    rx_cfg[chip_id] = 0;
    
    
    for(port_id = 0; port_id < 8; port_id++)
    {
        rx_port_state[chip_id][port_id] = 0;
    }

    for(port_id = 0; port_id < 8; port_id++)
    {
        rx_bcm_port_state[chip_id][port_id] = 0;
    }
    
    /*step2:init inf_ch_adp_rx_map_table*/
    for(index = 0; index < 8; index++)
    {    
        inf_ch_adp_rx_map_table_set(chip_id,index,0xf);
    }

    /*init inf_ch_adp_rx_mode_cfg*/
    inf_ch_adp_rx_mode_cfg_400g_set(chip_id,0);
    inf_ch_adp_rx_mode_cfg_200g_set(chip_id,0,0);
    inf_ch_adp_rx_mode_cfg_200g_set(chip_id,1,0);
    inf_ch_adp_rx_mode_cfg_200g_set(chip_id,2,0);
    inf_ch_adp_rx_mode_cfg_200g_set(chip_id,3,0);

    /*init inf_ch_adp_rx_lf_sel*/
    regp_write(chip_id,INF_CH_ADP_RX_BASE_ADDR,0xa,0xfac688);

    /*init busa_rx_info*/
    for (index = 0; index < 8; index++)
    {
        busa_rx_info[chip_id][index].chip_id = chip_id;
        busa_rx_info[chip_id][index].local_port = 0xf;
        busa_rx_info[chip_id][index].bcm_port = 0xf;
        busa_rx_info[chip_id][index].rate = 0xf;
        busa_rx_info[chip_id][index].instance[0] = 0xf;
        busa_rx_info[chip_id][index].instance[1] = 0xf;
        busa_rx_info[chip_id][index].instance[2] = 0xf;
        busa_rx_info[chip_id][index].instance[3] = 0xf;
    }
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     inf_ch_adp_rx_cfg
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
*      chip_id:chip number used
*      add_del_sel: 0:del  1:add
*      bcm_port: bcm port 0-7
*      local_port: local port 0-7
*      rate:0-3 0:50g 1:100g 2:200g 3:400g
*      serdes_rate: 0-2  0:25G 1:26G 2:53G
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-9-21    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_rx_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_32 bcm_port,UINT_32 local_port,UINT_32 ins_1,UINT_32 ins_2,UINT_8 rate,UINT_8 serdes_rate)
{
    UINT_32 read_local_port = 0;
    UINT_32 index = 0;
	UINT_32 info_200g[18][3] = {{0,0,1},{2,2,3},{4,4,5},{4,4,6},{4,4,7},{6,5,6},{6,6,7},{7,5,7},{7,6,7},
                                {0,1,0},{2,3,2},{4,5,4},{4,6,4},{4,7,4},{6,6,5},{6,7,6},{7,7,5},{7,7,6}};
    
    if (chip_id > MAX_DEV || add_del_sel > 1 || bcm_port > 7 || local_port > 7 || rate > 3)
    {        
        printf("\r\n [%s] invalid parameter!!chip_id=%d,add_del_sel=%d,bcm_port=%d,local_port=%d,rate=%d\r\n",
            __FUNCTION__,chip_id,add_del_sel,bcm_port,local_port,rate);
        
        return RET_PARAERR;
    }

    if (add_del_sel == 0)
    {
        for (index = 0; index < 8; index++)
        {
            inf_ch_adp_rx_map_table_get(chip_id, index, &read_local_port);
            if (local_port == read_local_port)
            {
                break;
            }
        }

        if (index == 8)
        {
            printf("\r\n [%s] bcm_port = %d is not used! Don't need to delete.\r\n",__FUNCTION__,bcm_port);
            return RET_FAIL;
        }

        rx_cfg[chip_id] = rx_cfg[chip_id] - ((1 << rate) * 50);
        rx_bcm_port_state[chip_id][bcm_port] = 0;
        /** inf_ch_adp_rx_map_table_set(chip_id,bcm_port,0xf); */

        busa_rx_info[chip_id][local_port].chip_id = 0xf;
        busa_rx_info[chip_id][local_port].local_port = 0xf;
        busa_rx_info[chip_id][local_port].bcm_port = 0xf;
        busa_rx_info[chip_id][local_port].rate = 0xf;
        busa_rx_info[chip_id][local_port].instance[0] = 0xf;
        busa_rx_info[chip_id][local_port].instance[1] = 0xf;
        busa_rx_info[chip_id][local_port].instance[2] = 0xf;
        busa_rx_info[chip_id][local_port].instance[3] = 0xf;

        switch (rate)
        {
            case 0:
            case 1:
                rx_port_state[chip_id][local_port] = 0;
                break;
            case 2:
				for (index = 0; index < 18; index++)
                {
                    if((info_200g[index][0] == local_port) && (info_200g[index][1] == ins_1) && (info_200g[index][2] == ins_2))
                    {
                        break;
                    }
                }

				if (index == 18)
                {
                    printf("\r\n [%s] ERROR! No such combination! local_port = %d,ins_1 = %d, ins_2 = %d \r\n",
                        __FUNCTION__,local_port,ins_1,ins_2);
                    return RET_FAIL;
                }
                rx_port_state[chip_id][ins_1] = 0;
                rx_port_state[chip_id][ins_2] = 0;
                /* inf_ch_adp_rx_mode_cfg_200g_set(chip_id,local_port/2,0); */
                break;
            case 3:
                rx_port_state[chip_id][0] = 0;
                rx_port_state[chip_id][1] = 0;
                rx_port_state[chip_id][2] = 0;
                rx_port_state[chip_id][3] = 0;
                break;
            default:
                return RET_PARAERR;
        }
    }
    else
    {    
        if ((rx_cfg[chip_id] + (1 << rate) * 50) > 400)
        {
            printf("\r\n [%s] Error ! Total rate > 400G ! rx_cfg = %d,rate = %d \r\n",__FUNCTION__,rx_cfg[chip_id],rate);

            return RET_FAIL;
        }
        
        if (rx_bcm_port_state[chip_id][bcm_port] == 1)
        {
            printf("\r\n [%s] bcm_port = %d has been used!\r\n",__FUNCTION__,bcm_port);
            return RET_FAIL;
        }

        switch (rate)
        {
            case 0:
                if (rx_port_state[chip_id][local_port] == 1)
                {
                    printf("\r\n [%s] local_port = %d has been used! Config fail! \r\n",__FUNCTION__,local_port);
                    return RET_FAIL;
                }
                
                inf_ch_adp_rx_mode_cfg_400g_set(chip_id,0);
                inf_ch_adp_rx_mode_cfg_200g_set(chip_id,local_port/2,0);

                rx_cfg[chip_id] = rx_cfg[chip_id] + 50;
                rx_port_state[chip_id][local_port] = 1;
                rx_bcm_port_state[chip_id][bcm_port] = 1;
                inf_ch_adp_rx_map_table_set(chip_id,bcm_port,local_port);
                inf_ch_adp_rx_lf_sel_set(chip_id,local_port,bcm_port);
                
                switch (serdes_rate)
                {
                    case 0:
                        inf_ch_adp_rx_ts_delta_set(chip_id,0,local_port,20);
                        inf_ch_adp_rx_ts_delta_set(chip_id,1,local_port,20);
                        break;
                    case 1:
                        inf_ch_adp_rx_ts_delta_set(chip_id,0,local_port,19);
                        inf_ch_adp_rx_ts_delta_set(chip_id,1,local_port,19);
                        break;
                    case 2:
                        inf_ch_adp_rx_ts_delta_set(chip_id,0,local_port,19);
                        inf_ch_adp_rx_ts_delta_set(chip_id,1,local_port,19);
                        break;
                    default:
                        break;
                }

                busa_rx_info[chip_id][local_port].rate = 0;
                busa_rx_info[chip_id][local_port].instance[0] = local_port;

                break;    
            case 1:
                if (rx_port_state[chip_id][local_port] == 1)
                {
                    printf("\r\n [%s] local_port = %d has been used! Config fail! \r\n",__FUNCTION__,local_port);
                    return RET_FAIL;
                }
                
                inf_ch_adp_rx_mode_cfg_400g_set(chip_id,0);
                inf_ch_adp_rx_mode_cfg_200g_set(chip_id,local_port/2,0);

                rx_cfg[chip_id] = rx_cfg[chip_id] + 100;
                rx_port_state[chip_id][local_port] = 1;
                rx_bcm_port_state[chip_id][bcm_port] = 1;
                inf_ch_adp_rx_map_table_set(chip_id,bcm_port,local_port);
                inf_ch_adp_rx_lf_sel_set(chip_id,local_port,bcm_port);

                switch (serdes_rate)
                {
                    case 0:
                        inf_ch_adp_rx_ts_delta_set(chip_id,0,local_port,10);
                        inf_ch_adp_rx_ts_delta_set(chip_id,1,local_port,10);
                        break;
                    case 1:
                        inf_ch_adp_rx_ts_delta_set(chip_id,0,local_port,10);
                        inf_ch_adp_rx_ts_delta_set(chip_id,1,local_port,10);
                        break;
                    case 2:
                        inf_ch_adp_rx_ts_delta_set(chip_id,0,local_port,10);
                        inf_ch_adp_rx_ts_delta_set(chip_id,1,local_port,10);
                        break;
                    default:
                        break;
                }

                busa_rx_info[chip_id][local_port].rate = 1;
                busa_rx_info[chip_id][local_port].instance[0] = local_port;

                break;                
            case 2:
                if ((rx_port_state[chip_id][ins_1] == 1) || (rx_port_state[chip_id][ins_2] == 1))
                {
                    printf("\r\n [%s] ins[%d] = %d, ins[%d] = %d! Config fail! \r\n",__FUNCTION__,ins_1,rx_port_state[chip_id][ins_1],
                        ins_2,rx_port_state[chip_id][ins_2]);
                    return RET_FAIL;
                                    
                }
                
				for (index = 0; index < 18; index++)
                {
                    if((info_200g[index][0] == local_port) && (info_200g[index][1] == ins_1) && (info_200g[index][2] == ins_2))
                    {
                        break;
                    }
                }

				if (index == 18)
                {
                    printf("\r\n [%s] ERROR! No such combination! local_port = %d,ins_1 = %d, ins_2 = %d \r\n",
                        __FUNCTION__,local_port,ins_1,ins_2);
                    return RET_FAIL;
                }

                inf_ch_adp_rx_mode_cfg_400g_set(chip_id,0);
                inf_ch_adp_rx_mode_cfg_200g_set(chip_id,local_port/2,1);

                rx_cfg[chip_id] = rx_cfg[chip_id] + 200;
                rx_port_state[chip_id][ins_1] = 1;
                rx_port_state[chip_id][ins_2] = 1;
                rx_bcm_port_state[chip_id][bcm_port] = 1;
                inf_ch_adp_rx_map_table_set(chip_id,bcm_port,local_port);
                inf_ch_adp_rx_lf_sel_set(chip_id,ins_1,bcm_port);
                inf_ch_adp_rx_lf_sel_set(chip_id,ins_2,bcm_port);
                inf_ch_adp_rx_mode_cfg_set(chip_id,IS_ASYM,0);
                inf_ch_adp_rx_ts_delta_set(chip_id,0,local_port,5);
                inf_ch_adp_rx_ts_delta_set(chip_id,1,local_port,5);

                if (local_port == 6)
                {
                    inf_ch_adp_rx_mode_cfg_set(chip_id,IS_PORT6,1);
                }

                if (local_port == 7)
                {
                    inf_ch_adp_rx_mode_cfg_set(chip_id,IS_PORT6,0);
                }

				if (((ins_1 == 4) && (ins_2 == 6)) || ((ins_1 == 6) && (ins_2 == 4)))
				{
					inf_ch_adp_rx_mode_cfg_set(chip_id,IS_ASYM,1);
					inf_ch_adp_rx_mode_cfg_set(chip_id,ASYM_INST_SEL,0);
				}

				if (((ins_1 == 4) && (ins_2 == 7)) || ((ins_1 == 7) && (ins_2 == 4)))
				{
					inf_ch_adp_rx_mode_cfg_set(chip_id,IS_ASYM,1);
					inf_ch_adp_rx_mode_cfg_set(chip_id,ASYM_INST_SEL,1);
				}

				if (((ins_1 == 5) && (ins_2 == 6)) || ((ins_1 == 6) && (ins_2 == 5)))
				{
					inf_ch_adp_rx_mode_cfg_set(chip_id,IS_ASYM,1);
					inf_ch_adp_rx_mode_cfg_set(chip_id,ASYM_INST_SEL,1);
				}

				if (((ins_1 == 5) && (ins_2 == 7)) || ((ins_1 == 7) && (ins_2 == 5)))
				{
					inf_ch_adp_rx_mode_cfg_set(chip_id,IS_ASYM,1);
					inf_ch_adp_rx_mode_cfg_set(chip_id,ASYM_INST_SEL,0);
				}

                busa_rx_info[chip_id][local_port].rate = 2;
                busa_rx_info[chip_id][local_port].instance[0] = ins_1;
                busa_rx_info[chip_id][local_port].instance[1] = ins_2;
                
                break;
            case 3:
                if (local_port != 0)
                {
                    printf("\r\n [%s] 400G local_port only select 0. local_port = %d\r\n",__FUNCTION__,local_port);
                    return RET_FAIL;
                }

                inf_ch_adp_rx_mode_cfg_400g_set(chip_id,1);
                inf_ch_adp_rx_mode_cfg_200g_set(chip_id,0,0);

                rx_cfg[chip_id] = rx_cfg[chip_id] + 400;
                rx_port_state[chip_id][0] = 1;
                rx_port_state[chip_id][1] = 1;
                rx_port_state[chip_id][2] = 1;
                rx_port_state[chip_id][3] = 1;
                rx_bcm_port_state[chip_id][bcm_port] = 1;
                inf_ch_adp_rx_map_table_set(chip_id,bcm_port,local_port);
                inf_ch_adp_rx_lf_sel_set(chip_id,0,bcm_port);
                inf_ch_adp_rx_lf_sel_set(chip_id,1,bcm_port);
                inf_ch_adp_rx_lf_sel_set(chip_id,2,bcm_port);
                inf_ch_adp_rx_lf_sel_set(chip_id,3,bcm_port);
                inf_ch_adp_rx_ts_delta_set(chip_id,0,local_port,2);
                inf_ch_adp_rx_ts_delta_set(chip_id,1,local_port,2);

                busa_rx_info[chip_id][local_port].rate = 3;
                busa_rx_info[chip_id][local_port].instance[0] = 0;
                busa_rx_info[chip_id][local_port].instance[1] = 1;
                busa_rx_info[chip_id][local_port].instance[2] = 2;
                busa_rx_info[chip_id][local_port].instance[3] = 3;

                break;
            default:
                return RET_PARAERR;
        }

        busa_rx_info[chip_id][local_port].chip_id = chip_id;
        busa_rx_info[chip_id][local_port].local_port = local_port;
        busa_rx_info[chip_id][local_port].bcm_port = bcm_port;
    }

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     inf_ch_adp_tx_init
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-6-15    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_init(UINT_8 chip_id)
{
    UINT_8 index = 0;
    UINT_8 port_id = 0;
    UINT_32 map_ram_value = 0xf;

    /*step1:init port state*/
    tx_cfg[chip_id] = 0;
    
    for(port_id = 0; port_id < 8; port_id++)
    {
        tx_port_state[chip_id][port_id] = 0;
    }

    /*init bcm port state*/
    for(port_id = 0; port_id < 8; port_id++)
    {
        tx_bcm_port_state[chip_id][port_id] = 0;
    }
    
    /*step2:init inf_ch_adp_tx_map_table*/
    for(index = 0; index < 8; index++)
    {    
        inf_ch_adp_tx_map_table_set(chip_id,index,0xf);
    }

    /*init tx_mode_cfg*/
    inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_400G,0);
    inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_200G,0);
    inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_BYPASS,0);
    
    /*init map_ram*/
    for(index = 0; index < 80; index++)
    {
        /** coverity[ARRAY_VS_SINGLETON:FALSE] */
        inf_ch_adp_tx_map_ram_set(chip_id,index,&map_ram_value);
    }

    /*init map_en*/
    inf_ch_adp_tx_map_en_set(chip_id,1);

    /*init external_cfg    */
    inf_ch_adp_tx_external_cfg_set(chip_id,RA_IS_400G,0);
    inf_ch_adp_tx_external_cfg_set(chip_id,RA_400G_CH,0x7f);

    /*init busa_tx_info*/
    for (index = 0; index < 8; index++)
    {
        busa_tx_info[chip_id][index].chip_id = chip_id;
        busa_tx_info[chip_id][index].local_port = 0xf;
        busa_tx_info[chip_id][index].bcm_port = 0xf;
        busa_tx_info[chip_id][index].rate = 0xf;
        busa_tx_info[chip_id][index].instance[0] = 0xf;
        busa_tx_info[chip_id][index].instance[1] = 0xf;
        busa_tx_info[chip_id][index].instance[2] = 0xf;
        busa_tx_info[chip_id][index].instance[3] = 0xf;
    }
    
    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     flexe_inf_ch_adp_tx_cfg
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
*      chip_id:chip number used
*      add_del_sel: 0:del  1:add
*      bcm_port: bcm port 0-7
*      local_port: local port 0-7
*      rate:0-3 0:50g 1:100g 2:200g 3:400g
*      bypass: 0:flexe  1:bypass
*      client_id:0-79
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-9-21    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_tx_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_32 bcm_port,UINT_32 local_port,UINT_32 ins_1,UINT_32 ins_2,UINT_8 rate,UINT_8 bypass,UINT_8 client_id)
{
    UINT_32 read_local_port = 0;
    UINT_32 map_ram_value = 0xf;
    UINT_32 index = 0;
	UINT_32 info_200g[18][3] = {{0,0,1},{2,2,3},{4,4,5},{4,4,6},{4,4,7},{6,5,6},{6,6,7},{7,5,7},{7,6,7},
                                {0,1,0},{2,3,2},{4,5,4},{4,6,4},{4,7,4},{6,6,5},{6,7,6},{7,7,5},{7,7,6}};
    
    if (chip_id > MAX_DEV || add_del_sel > 1 || bcm_port > 7 || local_port > 7 || rate > 3 || bypass > 1)
    {        
        printf("\r\n [%s] invalid parameter!!chip_id=%d,add_del_sel=%d,bcm_port=%d,local_port=%d,rate=%d,bypass=%d\r\n",
            __FUNCTION__,chip_id,add_del_sel,bcm_port,local_port,rate,bypass);
        
        return RET_PARAERR;
    }

    /*flexe*/
    if (bypass == 0)
    {
        if (add_del_sel == 0)
        {
            inf_ch_adp_tx_map_table_get(chip_id,bcm_port,&read_local_port);

            if (read_local_port == 0xf)
            {
                printf("\r\n [%s] bcm_port = %d is not used! Don't need to delete.\r\n",__FUNCTION__,bcm_port);
                return RET_FAIL;
            }

            tx_cfg[chip_id] = tx_cfg[chip_id] - ((1 << rate) * 50);
            tx_bcm_port_state[chip_id][bcm_port] = 0;
            /** inf_ch_adp_tx_map_table_set(chip_id,bcm_port,0xf); */

            busa_tx_info[chip_id][local_port].chip_id = 0xf;
            busa_tx_info[chip_id][local_port].local_port = 0xf;
            busa_tx_info[chip_id][local_port].bcm_port = 0xf;
            busa_tx_info[chip_id][local_port].rate = 0xf;
            busa_tx_info[chip_id][local_port].instance[0] = 0xf;
            busa_tx_info[chip_id][local_port].instance[1] = 0xf;
            busa_tx_info[chip_id][local_port].instance[2] = 0xf;
            busa_tx_info[chip_id][local_port].instance[3] = 0xf;

            switch (rate)
            {
                case 0:
                case 1:
                    tx_port_state[chip_id][local_port] = 0;
                    break;
                case 2:
					for (index = 0; index < 18; index++)
                    {
                        if((info_200g[index][0] == local_port) && (info_200g[index][1] == ins_1) && (info_200g[index][2] == ins_2))
                        {
                            break;
                        }
                    }

					if (index == 18)
                    {
                        printf("\r\n [%s] ERROR! No such combination! local_port = %d,ins_1 = %d, ins_2 = %d \r\n",
                            __FUNCTION__,local_port,ins_1,ins_2);
                        return RET_FAIL;
                    }
                    
                    tx_port_state[chip_id][ins_1] = 0;
                    tx_port_state[chip_id][ins_2] = 0;
                    /* inf_ch_adp_tx_mode_cfg_200g_set(chip_id,local_port/2,0); */
                    break;
                case 3:
                    tx_port_state[chip_id][0] = 0;
                    tx_port_state[chip_id][1] = 0;
                    tx_port_state[chip_id][2] = 0;
                    tx_port_state[chip_id][3] = 0;
                    break;
                default:
                    return RET_PARAERR;
            }
        }
        else
        {
            if ((tx_cfg[chip_id] + (1 << rate) * 50) > 400)
            {
                printf("\r\n [%s] Error ! Total rate > 400G ! tx_cfg = %d,rate = %d \r\n",__FUNCTION__,tx_cfg[chip_id],rate);

                return RET_FAIL;
            }
        
            if (tx_bcm_port_state[chip_id][bcm_port] == 1)
            {
                printf("\r\n [%s] bcm_port = %d has been used!\r\n",__FUNCTION__,bcm_port);
                return RET_FAIL;
            }

            switch (rate)
            {
                case 0:
                    if (tx_port_state[chip_id][local_port] == 1)
                    {
                        printf("\r\n [%s] local_port = %d has been used! Config fail! \r\n",__FUNCTION__,local_port);
                        return RET_FAIL;
                    }
                    
                    inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_400G,0);
                    inf_ch_adp_tx_mode_cfg_200g_set(chip_id,local_port/2,0);
                    inf_ch_adp_tx_mode_cfg_bypass_set(chip_id,local_port,0);
                    /*inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_BYPASS,0);*/

                    tx_cfg[chip_id] = tx_cfg[chip_id] + 50;
                    tx_port_state[chip_id][local_port] = 1;
                    tx_bcm_port_state[chip_id][bcm_port] = 1;
                    inf_ch_adp_tx_map_table_set(chip_id,bcm_port,local_port);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,START_LEVEL,local_port,12);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,HIGH_LEVEL,local_port,16);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,LOW_LEVEL,local_port,8);
                    inf_ch_adp_tx_port_fifo_cfg2_set(chip_id,PROTECT_UP_LEVEL,local_port,21);
                    inf_ch_adp_tx_port_fifo_cfg2_set(chip_id,PROTECT_DOWN_LEVEL,local_port,3);

                    busa_tx_info[chip_id][local_port].rate = 0;
                    busa_tx_info[chip_id][local_port].instance[0] = local_port;

                    break;
                case 1:
                    if (tx_port_state[chip_id][local_port] == 1)
                    {
                        printf("\r\n [%s] local_port = %d has been used! Config fail! \r\n",__FUNCTION__,local_port);
                        return RET_FAIL;
                    }
                    
                    inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_400G,0);
                    inf_ch_adp_tx_mode_cfg_200g_set(chip_id,local_port/2,0);
                    inf_ch_adp_tx_mode_cfg_bypass_set(chip_id,local_port,0);
                    /*inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_BYPASS,0);*/

                    tx_cfg[chip_id] = tx_cfg[chip_id] + 100;
                    tx_port_state[chip_id][local_port] = 1;
                    tx_bcm_port_state[chip_id][bcm_port] = 1;
                    inf_ch_adp_tx_map_table_set(chip_id,bcm_port,local_port);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,START_LEVEL,local_port,14);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,HIGH_LEVEL,local_port,18);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,LOW_LEVEL,local_port,10);
                    inf_ch_adp_tx_port_fifo_cfg2_set(chip_id,PROTECT_UP_LEVEL,local_port,25);
                    inf_ch_adp_tx_port_fifo_cfg2_set(chip_id,PROTECT_DOWN_LEVEL,local_port,3);

                    busa_tx_info[chip_id][local_port].rate = 1;
                    busa_tx_info[chip_id][local_port].instance[0] = local_port;

                    break;
                case 2:
                    if ((tx_port_state[chip_id][ins_1] == 1) || (tx_port_state[chip_id][ins_2] == 1))
                    {
                        printf("\r\n [%s] ins[%d] = %d, ins[%d] = %d! Config fail! \r\n",__FUNCTION__,ins_1,tx_port_state[chip_id][ins_1],
                            ins_2,tx_port_state[chip_id][ins_2]);
                        return RET_FAIL;
                                        
                    }
                    
					for (index = 0; index < 18; index++)
                    {
                        if((info_200g[index][0] == local_port) && (info_200g[index][1] == ins_1) && (info_200g[index][2] == ins_2))
                        {
                            break;
                        }
                    }

					if (index == 18)
                    {
                        printf("\r\n [%s] ERROR! No such combination! local_port = %d,ins_1 = %d, ins_2 = %d \r\n",
                            __FUNCTION__,local_port,ins_1,ins_2);
                        return RET_FAIL;
                    }
                    
                    inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_400G,0);
                    inf_ch_adp_tx_mode_cfg_bypass_set(chip_id,local_port,0);
                    /*inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_BYPASS,0);*/
                    inf_ch_adp_tx_mode_cfg_200g_set(chip_id,local_port/2,1);
                    
                    tx_cfg[chip_id] = tx_cfg[chip_id] + 200;
                    tx_port_state[chip_id][ins_1] = 1;
                    tx_port_state[chip_id][ins_2] = 1;
                    tx_bcm_port_state[chip_id][bcm_port] = 1;
                    inf_ch_adp_tx_map_table_set(chip_id,bcm_port,local_port);
                    inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_ASYM,0);

                    if (local_port == 6)
                    {
                        inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_PORT6,1);
                    }

                    if (local_port == 7)
                    {
                        inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_PORT6,0);
					}

					if (((ins_1 == 4) && (ins_2 == 6)) || ((ins_1 == 6) && (ins_2 == 4)))
					{
						inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_ASYM,1);
						inf_ch_adp_tx_mode_cfg_set(chip_id,TX_ASYM_INST_SEL,0);
					}

					if (((ins_1 == 4) && (ins_2 == 7)) || ((ins_1 == 7) && (ins_2 == 4)))
					{
						inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_ASYM,1);
						inf_ch_adp_tx_mode_cfg_set(chip_id,TX_ASYM_INST_SEL,1);
					}

					if (((ins_1 == 5) && (ins_2 == 6)) || ((ins_1 == 6) && (ins_2 == 5)))
					{
						inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_ASYM,1);
						inf_ch_adp_tx_mode_cfg_set(chip_id,TX_ASYM_INST_SEL,1);
					}

					if (((ins_1 == 5) && (ins_2 == 7)) || ((ins_1 == 7) && (ins_2 == 5)))
					{
						inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_ASYM,1);
						inf_ch_adp_tx_mode_cfg_set(chip_id,TX_ASYM_INST_SEL,0);
                    }

                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,START_LEVEL,local_port,22);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,HIGH_LEVEL,local_port,29);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,LOW_LEVEL,local_port,15);
                    inf_ch_adp_tx_port_fifo_cfg2_set(chip_id,PROTECT_UP_LEVEL,local_port,41);
                    inf_ch_adp_tx_port_fifo_cfg2_set(chip_id,PROTECT_DOWN_LEVEL,local_port,3);

                    busa_tx_info[chip_id][local_port].rate = 2;
                    busa_tx_info[chip_id][local_port].instance[0] = ins_1;
                    busa_tx_info[chip_id][local_port].instance[1] = ins_2;

                    break;
                case 3:
                    if (local_port != 0)
                    {
                        printf("\r\n [%s] 400G local_port only select 0. local_port = %d\r\n",__FUNCTION__,local_port);
                        return RET_FAIL;
                    }
                    
                    inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_400G,1);
                    inf_ch_adp_tx_mode_cfg_200g_set(chip_id,0,0);
                    inf_ch_adp_tx_mode_cfg_bypass_set(chip_id,local_port,0);
                    /*inf_ch_adp_tx_mode_cfg_set(chip_id,TX_IS_BYPASS,0);*/

                    tx_cfg[chip_id] = tx_cfg[chip_id] + 400;
                    tx_port_state[chip_id][0] = 1;
                    tx_port_state[chip_id][1] = 1;
                    tx_port_state[chip_id][2] = 1;
                    tx_port_state[chip_id][3] = 1;
                    tx_bcm_port_state[chip_id][bcm_port] = 1;
                    inf_ch_adp_tx_map_table_set(chip_id,bcm_port,local_port);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,START_LEVEL,local_port,38);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,HIGH_LEVEL,local_port,50);
                    inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,LOW_LEVEL,local_port,26);

                    busa_tx_info[chip_id][local_port].rate = 3;
                    busa_tx_info[chip_id][local_port].instance[0] = 0;
                    busa_tx_info[chip_id][local_port].instance[1] = 1;
                    busa_tx_info[chip_id][local_port].instance[2] = 2;
                    busa_tx_info[chip_id][local_port].instance[3] = 3;

                    break;
                default:
                    return RET_PARAERR;
            }

            busa_tx_info[chip_id][local_port].chip_id = chip_id;
            busa_tx_info[chip_id][local_port].local_port = local_port;
            busa_tx_info[chip_id][local_port].bcm_port = bcm_port;
        }
    }
    /*bypass*/
    else
    {
        if (client_id > 79)
        {
            printf("\r\n [%s] invalid parameter!! client_id=%d\r\n",__FUNCTION__,client_id);

            return RET_PARAERR;
        }
        
        if (add_del_sel == 0)
        {
            inf_ch_adp_tx_map_table_get(chip_id,bcm_port,&read_local_port);

            if (read_local_port == 0xf)
            {
                printf("\r\n [%s] bcm_port = %d is not used! Don't need to delete.\r\n",__FUNCTION__,bcm_port);
                return RET_FAIL;
            }

            tx_cfg[chip_id] = tx_cfg[chip_id] - ((1 << rate) * 50);
            tx_bcm_port_state[chip_id][bcm_port] = 0;
            tx_port_state[chip_id][read_local_port] = 0;
            inf_ch_adp_tx_map_table_set(chip_id,bcm_port,0xf);
            /** coverity[ARRAY_VS_SINGLETON:FALSE] */
            inf_ch_adp_tx_map_ram_set(chip_id,client_id,&map_ram_value);
            inf_ch_adp_tx_mode_cfg_bypass_set(chip_id,read_local_port,0);

            if(rate == 3)
            {
                inf_ch_adp_tx_external_cfg_set(chip_id,RA_IS_400G,0);
                inf_ch_adp_tx_external_cfg_set(chip_id,RA_400G_CH,0x7f);
            }
        }
        else
        {
            if ((tx_cfg[chip_id] + (1 << rate) * 50) > 400)
            {
                printf("\r\n [%s] Error ! Total rate > 400G ! tx_cfg = %d,rate = %d \r\n",__FUNCTION__,tx_cfg[chip_id],rate);

                return RET_FAIL;
            }
        
            if (tx_bcm_port_state[chip_id][bcm_port] == 1)
            {
                printf("\r\n [%s] bcm_port = %d has been used!\r\n",__FUNCTION__,bcm_port);
                return RET_FAIL;
            }

            if (rate == 3)
            {
                if (local_port != 0)
                {
                    printf("\r\n [%s] 400G local_port only select 0. local_port = %d\r\n",__FUNCTION__,local_port);
                    return RET_FAIL;
                }

                inf_ch_adp_tx_mode_cfg_bypass_set(chip_id,local_port,1);

                tx_cfg[chip_id] = tx_cfg[chip_id] + 400;
                tx_port_state[chip_id][local_port] = 1;
                tx_bcm_port_state[chip_id][bcm_port] = 1;
                inf_ch_adp_tx_map_table_set(chip_id,bcm_port,local_port);
                inf_ch_adp_tx_map_ram_set(chip_id,client_id,&local_port);
                inf_ch_adp_tx_external_cfg_set(chip_id,RA_IS_400G,1);
                inf_ch_adp_tx_external_cfg_set(chip_id,RA_400G_CH,client_id);                
            }
            else
            {
                if (tx_port_state[chip_id][local_port] == 1)
                {
                    printf("\r\n [%s] local_port = %d has been used! Config fail! \r\n",__FUNCTION__,local_port);
                    return RET_FAIL;
                }

                inf_ch_adp_tx_mode_cfg_bypass_set(chip_id,local_port,1);

                tx_cfg[chip_id] = tx_cfg[chip_id] + (1 << rate) * 50;
                tx_port_state[chip_id][local_port] = 1;
                tx_bcm_port_state[chip_id][bcm_port] = 1;
                inf_ch_adp_tx_map_table_set(chip_id,bcm_port,local_port);
                /** coverity[ARRAY_VS_SINGLETON:FALSE] */
                inf_ch_adp_tx_map_ram_set(chip_id,client_id,&local_port);
            }

            inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,START_LEVEL,local_port,12);
            inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,HIGH_LEVEL,local_port,16);
            inf_ch_adp_tx_port_fifo_cfg1_set(chip_id,LOW_LEVEL,local_port,8);
        }
    }

    return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION 
*
*     inf_ch_adp_dump
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
*      chip_id:chip number used
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
*   <author>     <data>    <CR_ID>      <DESCRIPTION>
*    dingyi        2018-9-21    1.0           initial
*
******************************************************************************/
RET_STATUS inf_ch_adp_dump(UINT_8 chip_id)
{
    UINT_8 i = 0;
    printf("\r\n------------rx-------------\r\n");
    printf("rx_cfg = %d\r\n",rx_cfg[chip_id]);
    
    for(i = 0; i < 8; i++)
    {
        printf("rx_port_state[%d][%d] = %d\r\n",chip_id,i,rx_port_state[chip_id][i]);
    }
    
    printf("\r\n");
    
    for(i = 0; i < 8; i++)
    {
        printf("rx_bcm_port_state[%d][%d] = %d\r\n",chip_id,i,rx_bcm_port_state[chip_id][i]);
    }
    
    printf("\r\n------------tx-------------\r\n");
    printf("tx_cfg = %d\r\n",tx_cfg[chip_id]);
    
    for(i = 0; i < 8; i++)
    {
        printf("tx_port_state[%d][%d] = %d\r\n",chip_id,i,tx_port_state[chip_id][i]);
    }
    
    printf("\r\n");
    
    for(i = 0; i < 8; i++)
    {
        printf("tx_bcm_port_state[%d][%d] = %d\r\n",chip_id,i,tx_bcm_port_state[chip_id][i]);
    }
    
    return RET_SUCCESS;
}

#ifdef __cplusplus
}
#endif 

