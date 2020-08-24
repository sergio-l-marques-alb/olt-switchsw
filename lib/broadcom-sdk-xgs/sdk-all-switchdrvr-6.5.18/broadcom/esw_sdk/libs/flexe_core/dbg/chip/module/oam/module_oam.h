
#ifndef MODULE_OAM_H
#define MODULE_OAM_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "global_macro.h"
#include "datatype.h"


/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/

typedef enum 
{
	base_los_alm_en 	= 0,
	bas_cs_lf_alm_en	= 1,
	bas_cs_rf_alm_en	= 2,
	bas_csf_lpi_alm_en	= 3,
	sdbip_alm_en		= 4,
	sdbei_alm_en		= 5,
	client_lf_alm_en	= 6,
	client_rf_alm_en	= 7,
	client_lpi_alm_en	= 8,
	client_fail_alm_en	= 9,
	tx_sdbip_alm_en 	= 10,
	tx_sdbei_alm_en 	= 11,
	tx_base_los_alm_en	= 12,
	tx_client_lf_alm_en = 13,
	tx_client_rf_alm_en = 14,
	tx_client_lpi_alm_en = 15,
	bas_rdi_alm_en		= 16,
	oam_alm_last
}oam_alm_t;

#define INSTANCE_NUM (8)

#ifdef __cplusplus
extern "C"
{
#endif
	
extern RET_STATUS flexe_oam_init(UINT_8 chip_id);
extern RET_STATUS sar_oam_init(UINT_8 chip_id);
extern RET_STATUS sar_oam_channel_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_16* status);
extern RET_STATUS flexe_oam_channel_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_16* status);
extern RET_STATUS sar_oam_per_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bip8_cnt,UINT_32* bei_cnt);
extern RET_STATUS flexe_oam_per_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bip8_cnt,UINT_32* bei_cnt);
extern RET_STATUS sar_oam_bas_en_set (UINT_8 chip_id,UINT_8 channel_id,UINT_32  bas_period,UINT_8 bas_enable);
extern RET_STATUS flexe_oam_bas_en_set (UINT_8 chip_id,UINT_8 channel_id,UINT_32  bas_period,UINT_8 bas_enable);
extern RET_STATUS sar_oam_alm_enable_set (UINT_8 chip_id,oam_alm_t alm_type,UINT_8 status); 
extern RET_STATUS flexe_oam_alm_enable_set (UINT_8 chip_id,oam_alm_t alm_type,UINT_8 status); 
extern RET_STATUS sar_oam_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* packet_cnt);
extern RET_STATUS flexe_oam_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* packet_cnt);
extern RET_STATUS sar_oam_bas_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* packet_cnt);
extern RET_STATUS flexe_oam_bas_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* packet_cnt);
extern RET_STATUS sar_oam_count_clr(UINT_8 chip_id,UINT_8 ch_id);
extern RET_STATUS flexe_oam_count_clr(UINT_8 chip_id,UINT_8 ch_id);
extern RET_STATUS sar_oam_count_query(UINT_8 chip_id,UINT_8 ch_id);
extern RET_STATUS flexe_oam_count_query(UINT_8 chip_id,UINT_8 ch_id);
extern RET_STATUS sar_oam_bypass_cfg(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction,UINT_8 mode);
extern RET_STATUS flexe_oam_bypass_cfg(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction,UINT_8 mode);
extern RET_STATUS flexe_oam_localfault_insert(UINT_8 chip_id,UINT_8 channel_id,UINT_8 status);
extern RET_STATUS flexe_oam_remotefault_insert(UINT_8 chip_id,UINT_8 channel_id,UINT_8 status);
extern RET_STATUS sar_oam_localfault_insert(UINT_8 chip_id,UINT_8 channel_id,UINT_8 status);
extern RET_STATUS sar_oam_remotefault_insert(UINT_8 chip_id,UINT_8 channel_id,UINT_8 status);
extern RET_STATUS sar_oam_sdsf_alm_threshold_cfg(UINT_8 chip_id,UINT_8 channel_id,UINT_8 type,UINT_32 block_num,UINT_32 set_thresh,UINT_32 clr_thresh);
extern RET_STATUS flexe_oam_sdsf_alm_threshold_cfg(UINT_8 chip_id,UINT_8 channel_id,UINT_8 type,UINT_32 block_num,UINT_32 set_thresh,UINT_32 clr_thresh);
extern RET_STATUS oam_lf_rf_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 *status);
extern RET_STATUS flexe_oam_lf_rf_alm_get(UINT_8 chip_id,UINT_8 channel_id,UINT_8 *status);
extern RET_STATUS sar_oam_sdbip_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bip8_cnt);
extern RET_STATUS flexe_oam_sdbip_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bip8_cnt);
extern RET_STATUS sar_oam_sdbei_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bei_cnt);
extern RET_STATUS flexe_oam_sdbei_cnt_get(UINT_8 chip_id,UINT_8 channel_id,UINT_32* bei_cnt);
extern void module_oam_debug_set(unsigned long int parameter);

#ifdef __cplusplus
}
#endif
#endif

