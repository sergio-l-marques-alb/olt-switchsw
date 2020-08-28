
#ifndef _DEMUX_ALGORITHM_H_
#define _DEMUX_ALGORITHM_H_

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "struct.h"

/* struct for flexe_demux input structure */
typedef struct 
{
    UINT_8 unit;              /* supported multi_chips, dev's index */
    UINT_8 ch;                /* input ts's ch */
    UINT_32 ts[TS_FIELD];     /* input ts */
    UINT_8 enable;            /* add or remove traffic state */
}flexe_demux_traffic_info_t;


/* struct for traffic's demux ch info */
typedef struct 
{
    UINT_8 unit;            /* dev's index */
    UINT_8 ch_id;           /* ch_id */
    UINT_32 ts[TS_FIELD];   /* ch's ts pos */
    UINT_8 ts_num;          /* ch's ts num */
    UINT_8 ts_min;          /* ch's min ts */
    UINT_8 enable;          /* ch's state */
}flexe_demux_ch_info_t;


/* struct for ts's info */
typedef struct 
{
    UINT_8 unit;            /* dev's index */
    UINT_8 ch_id;           /* ts belong to which ch */
    UINT_8 ts_id;           /* ts_id */
    UINT_8 w_col[TCYCLE];   /* ts's write_calculation result */
    UINT_8 r_col[TCYCLE];   /* ts's read_calculation result */
}flexe_demux_ts_info_t;


/* struct for phy info */
typedef struct 
{
    UINT_8 unit;                  /* dev's index */
    UINT_8 group_id;              /* phy's group */
    UINT_8 phy_index;             /* phy's index */
    UINT_8 phy_id;                /* phy's id */
    UINT_8 sou_index;             /* phy's sou_index */
    UINT_8 des_index;             /* phy's des_index */
    UINT_8 phy_div_des_index[2];  /* 100g division to 50g phy index */
    UINT_32 phy_div_ts_mask;      /* 100g division to 50g's ts position */
    UINT_8 phy_div_ts_num;        /* 100g division to 50g's ts_num */
	UINT_8 phy_div_ts_num_b;      /* 100g division to 50g's ts_num */
    flexe_phy_mode phy_mode;      /* phy's working mode */
    UINT_32 ts_mask;              /* phy's ts position */
    UINT_8 ts_num;                /* phy's payload ts */
    UINT_8 enable;                /* phy's add or remove state */
    UINT_8 available;             /* phy whether available */
}flexe_demux_phy_info_t;


/* struct for phy_group info */
typedef struct 
{
    UINT_8 unit;
    UINT_8 group_id;
    UINT_8 phy_index[PHY_NUM];
    UINT_8 phy_id[PHY_NUM];
    flexe_phy_mode group_phy_mode;
    flexe_phy_mode phy_mode[PHY_NUM];
    UINT_8 phy_num;
}flexe_demux_group_info_t;


/* struct for chip's demux info */
typedef struct 
{
     UINT_8 unit;                                                             /* dev's index */
     
     UINT_8 group_num;                                                        /* group num */
     flexe_demux_group_info_t group_info[PHY_NUM];                            /* group info */

     UINT_8 phy_mask;                                                         /* dev's phy mask */
     flexe_demux_phy_info_t sour_phy[PHY_NUM];                                /* source_phy's info */
     flexe_demux_phy_info_t des_phy[PHY_NUM];                                 /* des_phy's info */

     UINT_32 ch_mask[CH_FIELD];                                               /* dev's ch mask */
     UINT_32 ts_mask[TS_FIELD];                                               /* dev's ts mask */
     flexe_demux_ch_info_t *flexe_ch_info[CH_NUM];                            /* demux module's ch_info */
     flexe_demux_ts_info_t *flexe_ts_info[TS_NUM];                            /* demux module's ts info */
     
     UINT_32 ch_mask_b[CH_FIELD];                                             /* dev's ch mask_b */
     UINT_32 ts_mask_b[TS_FIELD];                                             /* dev's ts mask_b */
      flexe_demux_ch_info_t *flexe_ch_info_b[CH_NUM];                          /* demux module's ch_info_b */
     flexe_demux_ts_info_t *flexe_ts_info_b[TS_NUM];                          /* demux module's ts info_b */

     dev_err_t (*mm_alloc)(UINT_8 unit, flexe_mm_type mm_type, UINT_8 size);  /* funcptr for channel mm_alloc */
}flexe_demux_info_t;


/* function declare */
#ifdef __cplusplus
        extern "C"
        {
#endif
extern dev_err_t flexe_demux_init(UINT_8 unit);
extern dev_err_t flexe_demux_uninit(UINT_8 unit);
extern dev_err_t flexe_demux_divisor_data_get(UINT_8 input_data_1, UINT_8 input_data_2, UINT_8 *result_out);
extern dev_err_t flexe_demux_phy_map_out(UINT_8 unit,
                                         flexe_phy_mode *phy_mode,
                                         UINT_32 *phy_id,
                                         UINT_32 *phy_map,
                                         UINT_8 *grp_num,
                                         flexe_demux_phy_info_t *des_phy);
extern dev_err_t flexe_demux_phy_info_cfg(UINT_8 unit,
                                          UINT_8 *phy_index,
                                          UINT_32 *phy_ts_mask,
                                          UINT_8 *phy_ts_num,
                                          UINT_8 phy_num,
                                          UINT_8 enable,
                                          UINT_8 indicate);
extern dev_err_t flexe_demux_phy_info_out(UINT_8 unit,
                                          UINT_8 *phy_index,
                                          UINT_8 phy_num,
                                          flexe_demux_phy_info_t *des_phy);
extern dev_err_t flexe_demux_mm_alloc(UINT_8 unit, flexe_mm_type mm_type, UINT_8 size);
extern dev_err_t flexe_demux_ch_mm_alloc(UINT_8 unit, UINT_8 size);
extern dev_err_t flexe_demux_ch_b_mm_alloc(UINT_8 unit, UINT_8 size);
extern dev_err_t flexe_demux_ts_mm_alloc(UINT_8 unit, UINT_8 size);
extern dev_err_t flexe_demux_ts_b_mm_alloc(UINT_8 unit, UINT_8 size);
extern dev_err_t flexe_demux_traffic_cfg(flexe_demux_traffic_info_t traffic_info);
extern dev_err_t flexe_demux_traffic_b_cfg(flexe_demux_traffic_info_t traffic_info);
extern dev_err_t flexe_demux_algorithm_cfg(flexe_demux_traffic_info_t traffic_info);
extern dev_err_t flexe_demux_algorithm_b_cfg(flexe_demux_traffic_info_t traffic_info);
extern dev_err_t flexe_demux_traffic_out(flexe_demux_traffic_info_t traffic_info,
                                         flexe_demux_ch_info_t *p_flexe_ch_info_out,
                                         flexe_demux_ts_info_t *p_flexe_ts_info_out);
extern dev_err_t flexe_demux_traffic_b_out(flexe_demux_traffic_info_t traffic_info,
                                           flexe_demux_ch_info_t *p_flexe_ch_info_out,
                                           flexe_demux_ts_info_t *p_flexe_ts_info_out);
extern dev_err_t flexe_demux_dump(UINT_8 unit);
extern dev_err_t flexe_demux_ch_dump(UINT_8 unit, UINT_8 ch);
extern dev_err_t flexe_demux_ts_dump(UINT_8 unit, UINT_8 ts);
extern dev_err_t flexe_demux_debug(void);
extern dev_err_t flexe_demux_phy_group_cfg(UINT_8 unit,
                                           UINT_8 group_id,
                                           UINT_8 phy_index,
                                           UINT_8 *phy_id,
                                           flexe_phy_mode phy_mode,
                                           UINT_8 enable);
extern dev_err_t flexe_demux_phy_group_out(UINT_8 unit,
                                           UINT_8 group_id,
                                           UINT_8 *phy_num,
                                           flexe_demux_phy_info_t *sour_phy,
                                           flexe_demux_phy_info_t *des_phy);
extern dev_err_t flexe_demux_chip_info_get(UINT_8 unit, flexe_demux_info_t **flexe_demux_chip_info);
extern dev_err_t flexe_demux_phy_group_info_get(UINT_8 unit,
                                                UINT_8 group_id,
                                                flexe_demux_group_info_t *group_info);
#ifdef __cplusplus
}
#endif
#endif
