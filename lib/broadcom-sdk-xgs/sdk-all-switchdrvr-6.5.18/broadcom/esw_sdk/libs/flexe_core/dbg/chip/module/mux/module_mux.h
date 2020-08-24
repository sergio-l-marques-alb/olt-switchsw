

#ifndef _MODULE_MUX_H_
#define _MODULE_MUX_H_

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "struct.h"
#include "mux_algorithm.h"


#ifdef __cplusplus
        extern "C"
        {
#endif
extern dev_err_t mux_init(UINT_8 unit);
extern dev_err_t mux_uninit(UINT_8 unit);
extern dev_err_t mux_hw_env_init(UINT_8 unit);
extern dev_err_t mux_hw_init(UINT_8 unit);
extern dev_err_t mux_traffic_pcs_cfg(UINT_8 unit,
                                     UINT_8 ch,
                                     UINT_8 phy_index,
                                     flexe_phy_mode phy_mode,
                                     UINT_8 enable);
extern dev_err_t mux_debug(void);
extern dev_err_t mux_chip_info_get(UINT_8 unit, flexe_mux_info_t **flexe_mux_chip_info);
extern dev_err_t mux_phy_group_c_cfg(UINT_8 unit, UINT_8 group_id, UINT_8 c_value);
extern dev_err_t mux_phy_group_cfg(UINT_8 unit,
                                   UINT_8 group_id,
                                   UINT_8 phy_index,
                                   flexe_phy_mode phy_mode,
                                   UINT_8 enable);
extern UINT_8 min_element(UINT_8 *start, UINT_8 length);
extern dev_err_t mux_traffic_flexe_cfg(UINT_8 unit,
                                       UINT_8 ch,
                                       UINT_8 *phy_index,
                                       UINT_8 phy_num,
                                       UINT_32 *ts_mask,
                                       UINT_8 enable);
extern dev_err_t mux_traffic_flexe_cfg_debug(char *string);
extern dev_err_t mux_traffic_flexe_b_cfg(UINT_8 unit,
                                         UINT_8 ch,
                                         UINT_8 *phy_index,
                                         UINT_8 phy_num,
                                         UINT_32 *ts_mask,
                                         UINT_8 enable);
extern dev_err_t mux_traffic_flexe_b_cfg_debug(char *string);
extern dev_err_t mux_phy_xc_cfg(UINT_8 unit,
                         UINT_8 phy_index, /*flexe phy inst process index*/
                         UINT_8 phy_out_index); /*pcs index*/
extern dev_err_t flexe_default_config(UINT_8 unit,UINT_8 instance_id,UINT_8 mode);
#ifdef __cplusplus
}
#endif
#endif
