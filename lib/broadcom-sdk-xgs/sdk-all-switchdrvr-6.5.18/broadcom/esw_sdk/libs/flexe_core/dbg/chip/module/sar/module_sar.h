
#ifndef MODULE_SAR_H
#define MODULE_SAR_H


/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "datatype.h"

/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
typedef enum 
{
    _50GE_50INSTANCE   = 0,
    _100GE_50INSTANCE  = 1,
    _200GE_50INSTANCE  = 2,
    _400GE_50INSTANCE  = 3,
    _50GE_50GE         = 4,
    _100GE_100GE       = 5,
    _200GE_200GE       = 6,
    _400GE_400GE       = 7,
    _50GE_LOCAL           = 8,
    _100GE_LOCAL       = 9,
    _200GE_LOCAL       = 10,
    _400GE_LOCAL       = 11,
    _50G_INSTANCE_LOCAL = 12,
}flexe_env_mode;

typedef struct
{
    UINT_8 chip_id;            /*chip_id*/
    UINT_8 local_port;        /*local_port 0~7*/
    UINT_8 bcm_port;         /*bcm_port 0~7*/
    UINT_8 rate;            /*0:50g 1:100g 2:200g 3:400g*/
    UINT_8 instance[4];        /*instance id*/
}busa_info_t;


/******************************************************************************
*VARIABLE DEFINE
******************************************************************************/



/******************************************************************************
*LOCAL FUNCTION PROTOTYPE DECLARATION
******************************************************************************/


/******************************************************************************
*GLOBAL FUNCTION DEFINITION
******************************************************************************/
#ifdef __cplusplus
        extern "C"
        {
#endif

extern RET_STATUS sar_init(UINT_8 chip_id);
extern RET_STATUS sar_rx_init(UINT_8 chip_id);
extern RET_STATUS sar_tx_init(UINT_8 chip_id);
extern RET_STATUS sar_rx_ts_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num);
extern RET_STATUS sar_rx_ts_cfg_asic(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num);
extern RET_STATUS sar_rx_ts_cfg_asic_in_chip(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num);
extern RET_STATUS sar_tx_ts_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num);
extern RET_STATUS sar_tx_ts_cfg_asic(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num);
extern RET_STATUS sar_tx_ts_cfg_asic_in_chip(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_32 b_mode,UINT_8 ts_num);
extern RET_STATUS sar_switch_table(UINT_8 chip_id);
extern RET_STATUS sar_ts_allocate(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction,UINT_8 ts_num);
extern RET_STATUS sar_ts_free(UINT_8 chip_id,UINT_8 channel_id,UINT_8 direction);
extern RET_STATUS sar_rx_chan_map_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 ch_id,UINT_32 bcm_id);
extern RET_STATUS sar_tx_chan_map_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 ch_id,UINT_32 bcm_id);
extern RET_STATUS sar_loopback_mode_cfg(UINT_8 chip_id,UINT_8 mode);
extern RET_STATUS sar_debug(void);
extern RET_STATUS sar_dump(UINT_8 chip_id);
extern RET_STATUS flexe_env_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 grp_id,flexe_env_mode mode);
extern RET_STATUS client_env_init(UINT_8 chip_id);
extern RET_STATUS client_ts_allocate(UINT_8 chip_id,UINT_8 channel_id,UINT_8 ts_num);
extern RET_STATUS client_env_ts_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 channel_id,UINT_8 ts_num);
extern RET_STATUS inf_ch_adp_rx_init(UINT_8 chip_id);
extern RET_STATUS inf_ch_adp_rx_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_32 bcm_port,UINT_32 local_port,UINT_32 ins_1,UINT_32 ins_2,UINT_8 rate,UINT_8 serdes_rate);
extern RET_STATUS inf_ch_adp_tx_init(UINT_8 chip_id);
extern RET_STATUS inf_ch_adp_tx_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_32 bcm_port,UINT_32 local_port,UINT_32 ins_1,UINT_32 ins_2,UINT_8 rate,UINT_8 bypass,UINT_8 client_id);
extern RET_STATUS inf_ch_adp_dump(UINT_8 chip_id);
#ifdef __cplusplus
}
#endif
#endif
