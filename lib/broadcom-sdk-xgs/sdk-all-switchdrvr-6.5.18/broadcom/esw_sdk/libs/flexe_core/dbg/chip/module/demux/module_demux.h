

#ifndef _MODULE_DEMUX_H_
#define _MODULE_DEMUX_H_

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "struct.h"
#include "demux_algorithm.h"

#ifdef __cplusplus
        extern "C"
        {
#endif
extern dev_err_t demux_hw_init(UINT_8 unit);
extern dev_err_t demux_init(UINT_8 unit);
extern dev_err_t demux_uninit(UINT_8 unit);
extern dev_err_t demux_debug(void);
extern dev_err_t demux_phy_group_cfg(UINT_8 unit,
                                     UINT_8 group_id,
                                     UINT_8 phy_index,
                                     UINT_8 *phy_id,
                                     flexe_phy_mode phy_mode,
                                     UINT_8 enable);
extern dev_err_t demux_traffic_flexe_cfg(UINT_8 unit,
                                         UINT_8 ch,
                                         UINT_8 *phy_index,
                                         UINT_8 phy_num,
                                         UINT_32 *ts_mask,
                                         UINT_8 enable);
extern dev_err_t demux_traffic_flexe_b_cfg(UINT_8 unit,
                                           UINT_8 ch,
                                           UINT_8 *phy_index,
                                           UINT_8 phy_num,
                                           UINT_32 *ts_mask,
                                           UINT_8 enable);
extern dev_err_t demux_chip_info_get(UINT_8 unit, flexe_demux_info_t **flexe_demux_chip_info);
extern dev_err_t demux_phy_group_cfg_debug(char *string);
extern dev_err_t demux_traffic_flexe_cfg_debug(char *string);
extern dev_err_t demux_traffic_flexe_b_cfg_debug(char *string);
#ifdef __cplusplus
}
#endif
#endif
