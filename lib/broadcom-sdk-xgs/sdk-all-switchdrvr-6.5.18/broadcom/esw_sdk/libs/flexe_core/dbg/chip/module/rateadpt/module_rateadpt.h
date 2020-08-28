
#ifndef MODULE_RATEADPT_H
#define MODULE_RATEADPT_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "global_macro.h"
#include "datatype.h"


/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
#define RATEADP_BLOCK_NUM  (80)
#define RATEADP_CH_NUM     (80)
#define RATEADP_TS_NUM	   (80)
#define RATEADP_TS_MAX     (16)

/* define rateadp info struct*/
typedef struct RATEADP_INFO
{
	UINT_8			rateadp_blk_index[RATEADP_BLOCK_NUM];
	UINT_8			rateadp_blk_ch[RATEADP_BLOCK_NUM];
	bool			rateadp_blk_flag[RATEADP_BLOCK_NUM];
	UINT_8			rateadp_idle_num;
}RATEADP_INFO_T;
typedef struct MAC_RATEADP_INFO
{
	UINT_8			mac_rateadp_blk_index[RATEADP_BLOCK_NUM];
	UINT_8			mac_rateadp_blk_ch[RATEADP_BLOCK_NUM];
	bool			mac_rateadp_blk_flag[RATEADP_BLOCK_NUM];
	UINT_8			mac_rateadp_idle_num;
}MAC_RATEADP_INFO_T;


#ifdef __cplusplus
extern "C"
{
#endif
	
extern RET_STATUS rateadp_init(UINT_8 chip_id);
extern RET_STATUS rateadp_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_8 ts_num);
extern RET_STATUS rateadp_traffic_cfg_in_chip(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_8 ts_num);
extern RET_STATUS mac_rateadp_init(UINT_8 chip_id);
extern RET_STATUS mac_rateadp_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_8 ts_num);

#ifdef __cplusplus
}
#endif
#endif

