

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "sal.h"
#include "reg_operate.h"
#include "oh_rx.h"
#include "oh_tx.h"
#include "flexe_multi_deskew.h"
#include "oam_rx.h"
#include "oam_tx.h"
#include "flexe_oam_rx.h"
#include "flexe_oam_tx.h"
#include "ohoam_ef.h"
#include "module_sar.h"
#include "module_oam.h"

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
unsigned long int module_oam_debug = 0;
/*unsigned int g_flexe_oh_alm_enable[DEV_NUM][INSTANCE_NUM][flexe_oh_alm_last] = {0};*/

/*extern traffic_group_info_t g_group_info[DEV_NUM][PHY_NUM];*/

UINT_32 g_oam_count_l[DEV_NUM][CH_NUM] = {{0}};
UINT_32 g_oam_count_h[DEV_NUM][CH_NUM] = {{0}};
UINT_32 g_flexe_oam_count_l[DEV_NUM][CH_NUM] = {{0}};
UINT_32 g_flexe_oam_count_h[DEV_NUM][CH_NUM] = {{0}};

/******************************************************************************
*
* FUNCTION	flexe_oam_rx_init
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
*
*     chip_id: chip number used
*	  
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oam_init(UINT_8 chip_id)
{
	UINT_8 ch_id = 0;
	UINT_32 parameter[2] = {0};
	/*UINT_8 ch_max = 0;*/
	
	/*ch_max = CH_NUM;*/
	
	for (ch_id = 0; ch_id < CH_NUM; ch_id++)
	{
		/*modify by taos 1218 for sd/sf thresh set*/
		/*0xcc0c */
		parameter[0] = 0xb0002;
		parameter[1] = 0xc8;
		flexe_oam_rx_sfcfg_set(chip_id,ch_id,parameter);
		/*0xcc5a */
		flexe_oam_rx_sfbeicfg_ram_set(chip_id,ch_id,parameter);
		
		/*0xcc0e */
		parameter[0] = 0xb0002;
		parameter[1] = 0x4e20;
		flexe_oam_rx_sdcfg_ram_set(chip_id,ch_id,parameter);
		/*0xcc64 */
		flexe_oam_rx_sdbeicfg_ram_set(chip_id,ch_id,parameter);

		
		/*0xcc65 */
		parameter[0] = ch_id;
		parameter[1] = 0;
		flexe_oam_rx_reixc_cfg_ram_set(chip_id,ch_id,parameter);

		flexe_oam_tx_bas_en_cfg_asic_set(chip_id,ch_id,0);
		flexe_oam_rx_period_sel_set(chip_id,ch_id,0);

		
	}

	/* accord to xulei 2018-05-07*/
	ohoam_ef_soam_init(chip_id);

        flexe_oam_tx_ts_1dm_insert_en_set(chip_id,1);
        flexe_oam_tx_ts_2dmm_insert_en_set(chip_id,1);
        flexe_oam_tx_ts_2dmr_insert_en_set(chip_id,1);

	/*add by taos 0813 */
	flexe_oam_rx_alm_en_set(chip_id,TX_SDBIP_ALM_EN,0);
	flexe_oam_rx_alm_en_set(chip_id,TX_SDBEI_ALM_EN,0);
	/*add by taos 0820 	*/
	flexe_oam_tx_bas_period_cfg1_set(chip_id,0,0xf);
	flexe_oam_rx_bas_period_ab_set(chip_id,0,0x18);

	flexe_oam_tx_bas_period_cfg1_set(chip_id,1,0x1f);
	flexe_oam_rx_bas_period_ab_set(chip_id,1,0x28);

	flexe_oam_tx_bas_period_cfg1_set(chip_id,2,0x3f);
	flexe_oam_rx_bas_period_ab_set(chip_id,2,0x48);

	flexe_oam_tx_bas_period_cfg1_set(chip_id,3,0x1ff);
	flexe_oam_rx_bas_period_ab_set(chip_id,3,0x208);

	flexe_oam_rx_ram_init(chip_id);

	flexe_oam_rx_ch_alm_init(chip_id);

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	sar_oam_rx_init
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
*
*     chip_id: chip number used
*	  
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS sar_oam_init(UINT_8 chip_id)
{
	UINT_8 ch_id = 0;
	UINT_32 parameter[2] = {0};
	/*UINT_8 ch_max = 0;*/
	
	/*ch_max = CH_NUM;*/
	
	for (ch_id = 0; ch_id < CH_NUM; ch_id++)
	{
		/*modify by taos 1218 for sd/sf thresh set*/
		/*0x8c0c */
		parameter[0] = 0xb0002;
		parameter[1] = 0xc8;
		oam_rx_sfcfg_set(chip_id,ch_id,parameter);
		/*0x8c5a */
		oam_rx_sfbeicfg_ram_set(chip_id,ch_id,parameter);
		
		/*0x8c0e */
		parameter[0] = 0xb0002;
		parameter[1] = 0x4e20;
		oam_rx_sdcfg_ram_set(chip_id,ch_id,parameter);
		/*0x8c64 */
		oam_rx_sdbeicfg_ram_set(chip_id,ch_id,parameter);
		
		/*0x8c65 */
		parameter[0] = ch_id;
		parameter[1] = 0;
		oam_rx_reixc_cfg_ram_set(chip_id,ch_id,parameter);
		
		oam_tx_bas_en_cfg_asic_set(chip_id,ch_id,0);
		oam_rx_period_sel_set(chip_id,ch_id,0);
	}

        oam_tx_ts_1dm_insert_en_set(chip_id,1);
        oam_tx_ts_2dmm_insert_en_set(chip_id,1);
        oam_tx_ts_2dmr_insert_en_set(chip_id,1);

	/*add by taos 0813 */
	oam_rx_alm_en_set(chip_id,TX_SDBIP_ALM_EN,0);
	oam_rx_alm_en_set(chip_id,TX_SDBEI_ALM_EN,0);
	
	/*add by taos 0820 	*/
	oam_tx_bas_period_cfg1_set(chip_id,0,0xf);
	oam_rx_bas_period_ab_set(chip_id,0,0x18);

	oam_tx_bas_period_cfg1_set(chip_id,1,0x1f);
	oam_rx_bas_period_ab_set(chip_id,1,0x28);

	oam_tx_bas_period_cfg1_set(chip_id,2,0x3f);
	oam_rx_bas_period_ab_set(chip_id,2,0x48);

	oam_tx_bas_period_cfg1_set(chip_id,3,0x1ff);
	oam_rx_bas_period_ab_set(chip_id,3,0x208);
	
	oam_rx_ram_init(chip_id);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	sar_oam_client_alm_get
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
*
*     chip_id: chip number used
*	  channel_idhannel_id
*	  *status:bit0:lpi;bit1:lf;bit2:rf;bit3:BAS_REV_ALM;bit4:sd alm;bit5:crc;bit6:bas_rdi;bit7:cc
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS sar_oam_channel_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_16* status)
{
	RET_STATUS rt = RET_SUCCESS;
	UINT_32 alm_status = 0;
    
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,channel_id);
		}

		return RET_PARAERR;
	}

	if (NULL == status)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	*status = 0;

	oam_rx_bas_csf_lpi_alm_get(chip_id,channel_id,&alm_status);

	*status |= (alm_status & MASK_1_BIT);

	oam_rx_bas_cs_lf_alm_get(chip_id,channel_id,&alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_1);

	oam_rx_bas_cs_rf_alm_get(chip_id,channel_id,&alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_2);

	oam_rx_bas_no_receive_alm_get(chip_id,channel_id,&alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_3);

	oam_rx_sdbip_alm_get(chip_id,channel_id,&alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_4);

	oam_rx_bas_crc_err_alm_get(chip_id,channel_id,&alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_5);

	oam_rx_bas_rdi_alm_get(chip_id,channel_id,&alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_6);

	oam_rx_bas_period_alm_get(chip_id,channel_id,&alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_7);
	oam_rx_sdbei_alm_get(chip_id,channel_id,&alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_8);
	oam_rx_sfbip_alm_get(chip_id,channel_id,&alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_9);
	oam_rx_sfbei_alm_get(chip_id,channel_id,&alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_10);
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_channel_alm_get
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  *status:bit0:lpi;bit1:lf;bit2:rf;bit3:BAS_REV_ALM;bit4:sd alm;bit5:crc;bit6:bas_rdi;bit7:cc
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oam_channel_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_16* status)
{
	RET_STATUS rt = RET_SUCCESS;
	UINT_8 alm_status = 0;
    
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,channel_id);
		}

		return RET_PARAERR;
	}

	if (NULL == status)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	*status = 0;
	
	/*flexe_oam_rx_bas_csf_lpi_alm_get(chip_id,channel_id,&alm_status);*/
	flexe_bas_csf_lpi[chip_id].p_ch_alm_report(chip_id,channel_id, flexe_bas_csf_lpi[chip_id].p_ch_alm, &alm_status);
	*status |= (alm_status & MASK_1_BIT);

	/*flexe_oam_rx_bas_cs_lf_alm_get(chip_id,channel_id,&alm_status);*/
	flexe_bas_cs_lf[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_bas_cs_lf[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_1);

	/*flexe_oam_rx_bas_cs_rf_alm_get(chip_id,channel_id,&alm_status);*/
	flexe_bas_cs_rf[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_bas_cs_rf[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_2);

	/*flexe_oam_rx_bas_no_receive_alm_get(chip_id,channel_id,&alm_status);*/
	flexe_bas_no_receive_alm[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_bas_no_receive_alm[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_3);

	/*flexe_oam_rx_sdbip_alm_get(chip_id,channel_id,&alm_status);*/
	flexe_rx_sdbip[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_rx_sdbip[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_4);

	/*flexe_oam_rx_bas_crc_err_alm_get(chip_id,channel_id,&alm_status);*/
	flexe_bas_crc_err[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_bas_crc_err[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_5);

	/*flexe_oam_rx_bas_rdi_alm_get(chip_id,channel_id,&alm_status);*/
	flexe_bas_rdi[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_bas_rdi[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_6);

	/*flexe_oam_rx_bas_period_alm_get(chip_id,channel_id,&alm_status);*/
	flexe_bas_period_alm[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_bas_period_alm[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_7);

	flexe_rx_sdbei[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_rx_sdbei[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_8);
	flexe_rx_sfbip[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_rx_sfbip[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_9);
	flexe_rx_sfbei[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_rx_sfbei[chip_id].p_ch_alm, &alm_status);
	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_10);

	if(module_oam_debug)
	{
		printf("[%s] chip_id=%d,channel_id=%d,status = 0x%x\r\n",__FUNCTION__,chip_id,channel_id,*status);
	}

	return rt;
}

/******************************************************************************
*
* FUNCTION	sar_oam_per_cnt_get
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  *bip8_cnt:
*	  *bei_cnt:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS sar_oam_per_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bip8_cnt,UINT_32* bei_cnt)
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,channel_id);
		}

		return RET_PARAERR;
	}

	if (NULL == bip8_cnt || NULL == bei_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	oam_rx_biperr_cnt_clr_pls_set(chip_id,channel_id);
	oam_rx_biperr_ram_get(chip_id,channel_id,bip8_cnt);

	oam_rx_beierr_cnt_clr_pls_set(chip_id,channel_id);
	oam_rx_beierr_ram_get(chip_id,channel_id,bei_cnt);
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_per_cnt_get
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  *bip8_cnt:
*	  *bei_cnt:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oam_per_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bip8_cnt,UINT_32* bei_cnt)
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,channel_id);
		}

		return RET_PARAERR;
	}


	if (NULL == bip8_cnt || NULL == bei_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	flexe_oam_rx_biperr_cnt_clr_pls_set(chip_id,channel_id);
	flexe_oam_rx_biperr_ram_get(chip_id,channel_id,bip8_cnt);

	flexe_oam_rx_beierr_cnt_clr_pls_set(chip_id,channel_id);
	flexe_oam_rx_beierr_ram_get(chip_id,channel_id,bei_cnt);
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	sar_oam_bas_en_set
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  bas_period:period 0~3 0:16K 1:64K 2:256K 3:512K
*	  bas_enable:enable
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS sar_oam_bas_en_set (UINT_8 chip_id,UINT_8 channel_id,UINT_32  bas_period,UINT_8 bas_enable)
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1) || bas_period > 3 || bas_enable > 1)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d,bas_period=%d,bas_enable=%d\r\n",__FUNCTION__,chip_id,channel_id,bas_period,bas_enable);
		}

		return RET_PARAERR;
	}

	
	oam_tx_bas_period_cfg_set(chip_id,channel_id,bas_period);
	oam_rx_period_cfg_set(chip_id,channel_id,bas_period);

	oam_tx_bas_en_cfg_asic_set(chip_id,channel_id,bas_enable);
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_bas_en_set
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  bas_period:period 0~3 0:16K 1:64K 2:256K 3:512K
*	  bas_enable:enable
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oam_bas_en_set (UINT_8 chip_id,UINT_8 channel_id,UINT_32  bas_period,UINT_8 bas_enable)
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1) || bas_period > 3 || bas_enable > 1)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d,bas_period=%d,bas_enable=%d\r\n",__FUNCTION__,chip_id,channel_id,bas_period,bas_enable);
		}

		return RET_PARAERR;
	}
	
	flexe_oam_tx_bas_period_cfg_set(chip_id,channel_id,bas_period);
	flexe_oam_rx_period_cfg_set(chip_id,channel_id,bas_period);

	flexe_oam_tx_bas_en_cfg_asic_set(chip_id,channel_id,bas_enable);
		
	return rt;
}


/******************************************************************************
*
* FUNCTION	sar_oam_alm_enable_set
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  alm_type:
*	  status:enable
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS sar_oam_alm_enable_set (UINT_8 chip_id,oam_alm_t alm_type,UINT_8 status) 
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || alm_type > bas_rdi_alm_en || status > 1)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,alm_type=%d,status=%d\r\n",__FUNCTION__,chip_id,alm_type,status);
		}

		return RET_PARAERR;
	}

	rt = oam_rx_alm_en_set(chip_id,alm_type,status);
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_alm_enable_set
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  alm_type:
*	  status:enable
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oam_alm_enable_set (UINT_8 chip_id,oam_alm_t alm_type,UINT_8 status) 
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || alm_type > bas_rdi_alm_en || status > 1)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,alm_type=%d,status=%d\r\n",__FUNCTION__,chip_id,alm_type,status);
		}

		return RET_PARAERR;
	}

	rt = flexe_oam_rx_alm_en_set(chip_id,alm_type,status);
	
	return rt;
}


/******************************************************************************
*
* FUNCTION	sar_oam_cnt_get
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
*
*     chip_id: chip number used
*	  group_id:the group to query
*	  client_id:the client to qurey
*	  *packet_cnt:the pointer to oam rx packet cnt
*	  
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS sar_oam_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* packet_cnt)
{
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == packet_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	oam_rx_oam_cnt_ram_get(chip_id,channel_id,packet_cnt);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_cnt_get
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
*
*     chip_id: chip number used
*	  group_id:the group to query
*	  client_id:the client to qurey
*	  *packet_cnt:the pointer to oam rx packet cnt
*	  
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oam_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* packet_cnt)
{
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == packet_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	flexe_oam_rx_oam_cnt_ram_get(chip_id,channel_id,packet_cnt);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	sar_oam_bas_cnt_get
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
*
*     chip_id: chip number used
*	  group_id:the group to query
*	  client_id:the client to qurey
*	  *packet_cnt:the pointer to oam rx packet cnt
*	  
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS sar_oam_bas_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* packet_cnt)
{
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == packet_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	oam_rx_base_oam_cnt_get(chip_id,channel_id,packet_cnt);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_bas_cnt_get
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
*
*     chip_id: chip number used
*	  group_id:the group to query
*	  client_id:the client to qurey
*	  *packet_cnt:the pointer to oam rx packet cnt
*	  
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oam_bas_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* packet_cnt)
{
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	if (NULL == packet_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!\r\n",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	flexe_oam_rx_base_oam_get(chip_id,channel_id,packet_cnt);
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	sar_oam_count_clr
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
*
*     chip_id: chip number used
*	  
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS sar_oam_count_clr(UINT_8 chip_id,UINT_8 ch_id)
{
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
        return RET_PARAERR;
	}
	
	g_oam_count_l[chip_id][ch_id] = 0;
	g_oam_count_h[chip_id][ch_id] = 0;

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	flexe_oam_count_clr
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
*
*     chip_id: chip number used
*	  
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS flexe_oam_count_clr(UINT_8 chip_id,UINT_8 ch_id)
{
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
        return RET_PARAERR;

	}
	
	g_flexe_oam_count_l[chip_id][ch_id] = 0;
	g_flexe_oam_count_h[chip_id][ch_id] = 0;

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	sar_oam_count_query
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
*
*     chip_id: chip number used
*	  ch_id:0~CH_NUM-1
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS sar_oam_count_query(UINT_8 chip_id,UINT_8 ch_id)
{
	UINT_32 parameter[2] = {0};
	
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
        return RET_PARAERR;

	}

	oam_rx_oam_cnt_ram_get(chip_id,ch_id,parameter);

	printf("CH_ID[%d]:oam cnt query bit31-0:%d\r\n",ch_id,parameter[0] - g_oam_count_l[chip_id][ch_id]);

	printf("CH_ID[%d]:oam cnt query bit63-32:%d\r\n",ch_id,parameter[1] - g_oam_count_h[chip_id][ch_id]);

	g_oam_count_l[chip_id][ch_id] = parameter[0];

	g_oam_count_h[chip_id][ch_id] = parameter[1];
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	flexe_oam_count_query
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
*
*     chip_id: chip number used
*	  ch_id:0~CH_NUM-1
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS flexe_oam_count_query(UINT_8 chip_id,UINT_8 ch_id)
{
	UINT_32 parameter[2] = {0};
	
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id = %d\r\n",__FUNCTION__,chip_id);
        }
        return RET_PARAERR;

	}

	flexe_oam_rx_oam_cnt_ram_get(chip_id,ch_id,parameter);

	printf("CH_ID[%d]:oam cnt query bit31-0:%d\r\n",ch_id,parameter[0] - g_flexe_oam_count_l[chip_id][ch_id]);

	printf("CH_ID[%d]:oam cnt query bit63-32:%d\r\n",ch_id,parameter[1] - g_flexe_oam_count_h[chip_id][ch_id]);

	g_flexe_oam_count_l[chip_id][ch_id] = parameter[0];

	g_flexe_oam_count_h[chip_id][ch_id] = parameter[1];
	
	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	sar_oam_bypass_cfg
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
*
*     chip_id: chip number used
*	  ch_id:0~CH_NUM-1
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS sar_oam_bypass_cfg(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction,UINT_8 mode)
{	
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	switch(direction)
	{
		case 0:
	/*flexe_oam_rx*/
	oam_rx_bypass_chnl_set(chip_id,channel_id,mode);
			oam_rx_idle_rpl_en_set(chip_id,channel_id,(1-mode));
			break;
		case 1:
	/*flexe_oam_tx*/
	oam_tx_bypass_client_set(chip_id,channel_id,mode);

	/*flexe_oam_rx*/
	/*flexe_oam_tx*/
			oam_tx_idle_rpl_en_set(chip_id,channel_id,(1-mode));
			break;
		default:
			printf("[%s]:%d the direction is out of range!!direction=%d\r\n",__FUNCTION__,__LINE__,direction);
			return RET_PARAERR;
	}

	return RET_SUCCESS;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_bypass_cfg
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
*
*     chip_id: chip number used
*	  ch_id:0~CH_NUM-1
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS flexe_oam_bypass_cfg(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction,UINT_8 mode)
{
	
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	switch(direction)
	{
		case 0:
	/*flexe_oam_rx*/
	flexe_oam_rx_bypass_chnl_set(chip_id,channel_id,mode);
			flexe_oam_rx_idle_rpl_en_set(chip_id,channel_id,(1-mode));
			break;
		case 1:
	/*flexe_oam_tx*/
	flexe_oam_tx_bypass_client_set(chip_id,channel_id,mode);

	/*flexe_oam_rx*/
	/*flexe_oam_tx*/
			flexe_oam_tx_idle_rpl_en_set(chip_id,channel_id,(1-mode));
			break;
		default:
			printf("[%s]:%d the direction is out of range!!direction=%d\r\n",__FUNCTION__,__LINE__,direction);
			return RET_PARAERR;
	}

	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	flexe_oam_localfault_insert
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
*
*     chip_id: chip number used
*	  group_id:
*	  client_id:
*	  status:0:enable insert;1:disable insert
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS flexe_oam_localfault_insert(UINT_8 chip_id,UINT_8 channel_id,UINT_8 status)
{
	UINT_32 parameter = status;
	
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	flexe_oam_tx_manual_insert_csf_lf_set(chip_id,channel_id,parameter);
	
	return RET_SUCCESS;
}



/******************************************************************************
*
* FUNCTION	flexe_oam_remotefault_insert
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
*
*     chip_id: chip number used
*	  group_id:
*	  client_id:
*	  status:0:enable insert;1:disable insert
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS flexe_oam_remotefault_insert(UINT_8 chip_id,UINT_8 channel_id,UINT_8 status)
{
	UINT_32 parameter = status;
	
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	flexe_oam_tx_manual_insert_csf_rf_set(chip_id,channel_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	sar_oam_localfault_insert
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
*
*     chip_id: chip number used
*	  group_id:
*	  client_id:
*	  status:0:enable insert;1:disable insert
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS sar_oam_localfault_insert(UINT_8 chip_id,UINT_8 channel_id,UINT_8 status)
{
	UINT_32 parameter = status;
	
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	oam_tx_manual_insert_csf_lf_set(chip_id,channel_id,parameter);
		
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	sar_oam_remotefault_insert
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
*
*     chip_id: chip number used
*	  group_id:
*	  client_id:
*	  status:0:enable insert;1:disable insert
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS sar_oam_remotefault_insert(UINT_8 chip_id,UINT_8 channel_id,UINT_8 status)
{
	UINT_32 parameter = status;
	if (chip_id > MAX_DEV)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);
		}

		return RET_PARAERR;
	}

	oam_tx_manual_insert_csf_rf_set(chip_id,channel_id,parameter);
	
	return RET_SUCCESS;
}


/******************************************************************************
*
* FUNCTION	sar_oam_sd_alm_threshold_cfg
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
*
*     chip_id: chip number used
*	  channel_id:channel number
*	  mode:

* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS sar_oam_sdsf_alm_threshold_cfg(UINT_8 chip_id,UINT_8 channel_id,UINT_8 type,UINT_32 block_num,UINT_32 set_thresh,UINT_32 clr_thresh)
{
	RET_STATUS rt = RET_SUCCESS;
	UINT_32 reg_data[2] = {0};
	
	if (chip_id > MAX_DEV )
	{
		printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);

		return RET_PARAERR;
	}

	if (channel_id >= CH_NUM)
	{
	    printf("[%s]:%d the channel id is out of range!!channel_id=%d\r\n",__FUNCTION__,__LINE__,channel_id);

        return RET_FAIL;
	}
	
	reg_data[0] = (clr_thresh & MASK_16_BIT) | ((set_thresh & MASK_16_BIT ) << SHIFT_BIT_16);
	reg_data[1] = ((set_thresh >> SHIFT_BIT_16) & MASK_1_BIT) | ((block_num & MASK_27_BIT) << SHIFT_BIT_1);

	switch(type)
	{
		case 0:
	rt = oam_rx_sdcfg_ram_set(chip_id,channel_id,reg_data);
			break;
		case 1:
			rt = oam_rx_sdbeicfg_ram_set(chip_id,channel_id,reg_data);
			break;
		case 2:
			rt = oam_rx_sfcfg_set(chip_id,channel_id,reg_data);
			break;
		case 3:
			rt = oam_rx_sfbeicfg_ram_set(chip_id,channel_id,reg_data);
			break;
		default:
			printf("[%s]:%d the type is out of range!!type=%d\r\n",__FUNCTION__,__LINE__,type);
			return RET_PARAERR;
	}	
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_sd_alm_threshold_cfg
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
*
*     chip_id: chip number used
*	channel_id:channel number
*	  mode:

* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS flexe_oam_sdsf_alm_threshold_cfg(UINT_8 chip_id,UINT_8 channel_id,UINT_8 type,UINT_32 block_num,UINT_32 set_thresh,UINT_32 clr_thresh)
{
	RET_STATUS rt = RET_SUCCESS;
	UINT_32 reg_data[2] = {0};
		
	if (chip_id > MAX_DEV )
	{
		printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);

		return RET_PARAERR;
	}

	if (channel_id >= CH_NUM)
	{
	    printf("[%s]:%d the channel id is out of range!!channel_id=%d\r\n",__FUNCTION__,__LINE__,channel_id);

        return RET_FAIL;
	}
	
	reg_data[0] = (clr_thresh & MASK_16_BIT) | ((set_thresh & MASK_16_BIT ) << SHIFT_BIT_16);
	reg_data[1] = ((set_thresh >> SHIFT_BIT_16) & MASK_1_BIT) | ((block_num & MASK_27_BIT) << SHIFT_BIT_1);

	switch(type)
	{
		case 0:
	rt = flexe_oam_rx_sdcfg_ram_set(chip_id,channel_id,reg_data);
			break;
		case 1:
			rt = flexe_oam_rx_sdbeicfg_ram_set(chip_id,channel_id,reg_data);
			break;
		case 2:
			rt = flexe_oam_rx_sfcfg_set(chip_id,channel_id,reg_data);
			break;
		case 3:
			rt = flexe_oam_rx_sfbeicfg_ram_set(chip_id,channel_id,reg_data);
			break;
		default:
			printf("[%s]:%d the type is out of range!!type=%d\r\n",__FUNCTION__,__LINE__,type);
			return RET_PARAERR;
	}	
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	oam_lf_rf_alm_get
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
*
*     chip_id: chip number used
*	channel_id:channel number
*	*status:bit0:lf;bit1:rf
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS oam_lf_rf_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 *status)
{
	RET_STATUS rt = RET_SUCCESS;
	UINT_32 alm_status = 0;
	
	if (chip_id > MAX_DEV )
	{
		printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);

		return RET_PARAERR;
	}

	if (channel_id >= CH_NUM)
	{
	    printf("[%s]:%d the channel id is out of range!!channel_id=%d\r\n",__FUNCTION__,__LINE__,channel_id);

        return RET_FAIL;
	}
	
	*status = 0;
	
	oam_rx_rdi_lf_alm_get(chip_id,channel_id,&alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_0);

	oam_rx_rf_alm_get(chip_id,channel_id,&alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_1);	
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_lf_rf_alm_get
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
*
*     chip_id: chip number used
*	channel_id:channel number
*	*status:bit0:lf;bit1:rf
* 
* RETURNS
* 
*   
*    
*    
*******************************************************************************/
RET_STATUS flexe_oam_lf_rf_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 *status)
{
	RET_STATUS rt = RET_SUCCESS;
	UINT_8 alm_status = 0;
	
	if (chip_id > MAX_DEV )
	{
		printf("[%s] invalid parameter!!chip_id=%d\r\n",__FUNCTION__,chip_id);

		return RET_PARAERR;
	}

	if (channel_id >= CH_NUM)
	{
	    printf("[%s]:%d the channel id is out of range!!channel_id=%d\r\n",__FUNCTION__,__LINE__,channel_id);

        return RET_FAIL;
	}

	*status = 0;
	
	flexe_rdi_lf_alm[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_rdi_lf_alm[chip_id].p_ch_alm, &alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_0);

	flexe_rf_alm[chip_id].p_ch_alm_report(chip_id, channel_id, flexe_rf_alm[chip_id].p_ch_alm, &alm_status);

	*status |= ((alm_status & MASK_1_BIT)<<SHIFT_BIT_1);

	if(module_oam_debug)
	{
		printf("[%s] chip_id=%d,channel_id=%d,status = 0x%x\r\n",__FUNCTION__,chip_id,channel_id,*status);
	}
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	sar_oam_sdbip_cnt_get
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  *bip8_cnt:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS sar_oam_sdbip_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bip8_cnt)
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,channel_id);
		}

		return RET_PARAERR;
	}

	if (NULL == bip8_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}
	
	oam_rx_biperr_cnt_clr_pls_set(chip_id,channel_id);
	oam_rx_biperr_ram_get(chip_id,channel_id,bip8_cnt);
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_sdbip_cnt_get
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  *bip8_cnt:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oam_sdbip_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bip8_cnt)
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,channel_id);
		}

		return RET_PARAERR;
	}


	if (NULL == bip8_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	flexe_oam_rx_biperr_cnt_clr_pls_set(chip_id,channel_id);
	flexe_oam_rx_biperr_ram_get(chip_id,channel_id,bip8_cnt);

	
	return rt;
}

/******************************************************************************
*
* FUNCTION	sar_oam_sdbei_cnt_get
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  *bei_cnt:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS sar_oam_sdbei_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bei_cnt)
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,channel_id);
		}

		return RET_PARAERR;
	}

	if (NULL == bei_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	oam_rx_beierr_cnt_clr_pls_set(chip_id,channel_id);
	oam_rx_beierr_ram_get(chip_id,channel_id,bei_cnt);
	
	return rt;
}

/******************************************************************************
*
* FUNCTION	flexe_oam_sdbei_cnt_get
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
*
*     chip_id: chip number used
*	  channel_id:channel_id
*	  *bei_cnt:
* 
* RETURNS
* 
*    RET_SUCCESS: success
*    RET_FAIL: fail
*    RET_PARAERR:parameter error
******************************************************************************/
RET_STATUS flexe_oam_sdbei_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bei_cnt)
{
	RET_STATUS rt = RET_SUCCESS;
	
	if (chip_id > MAX_DEV || channel_id > (CH_NUM-1))
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter!!chip_id=%d,channel_id=%d\r\n",__FUNCTION__,chip_id,channel_id);
		}

		return RET_PARAERR;
	}


	if (NULL == bei_cnt)
	{
		if (module_oam_debug)
		{
			printf("[%s] invalid parameter pointer!!",__FUNCTION__);
		}

		return RET_PARAERR;
	}

	flexe_oam_rx_beierr_cnt_clr_pls_set(chip_id,channel_id);
	flexe_oam_rx_beierr_ram_get(chip_id,channel_id,bei_cnt);
	
	return rt;
}

void module_oam_debug_set(unsigned long int parameter)
{
	module_oam_debug = parameter;
	printf("module_oam_debug = %ld\r\n",module_oam_debug);
}
