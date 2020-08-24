
#ifndef MODULE_MCMAC_H
#define MODULE_MCMAC_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "global_macro.h"
#include "datatype.h"


/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/

/* define mcmac macro */
#define MCMAC_CH_NUM     (80)
#define MCMAC_TS_NUM     (80)

#define RATEADP_BLOCK_NUM  (80)
#define RATEADP_CH_NUM     (80)
#define RATEADP_TS_NUM	   (80)
#define RATEADP_TS_MAX     (16)

/*#define MCMAC_CH_NUM     CH_NUM*/
/*#define MCMAC_TS_NUM     TS_NUM*/

#ifdef __cplusplus
extern "C"
{
#endif

extern RET_STATUS mcmac_init(UINT_8 chip_id);
extern RET_STATUS mcmac_tx_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_32 tx_max_pkt);
extern RET_STATUS mcmac_rx_traffic_cfg(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_32 rx_max_pkt);
extern RET_STATUS flexe_macrx_traffic_cfg(UINT_8 chip_id,UINT_8 add_del,UINT_8 ch_id,UINT_32 max_pkt);
extern RET_STATUS mcmac_diag(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode);
extern RET_STATUS mcmac_diag_clear(UINT_8 chip_id, UINT_8 ch_id);
extern void flexe_macrx_init(UINT_8 chip_id);
extern RET_STATUS flexe_macrx_diag_clear(UINT_8 chip_id, UINT_8 ch_id);
extern RET_STATUS mcmac_debug(void);

#ifdef __cplusplus
}
#endif
#endif

