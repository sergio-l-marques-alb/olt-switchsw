
#ifndef _MUX_ALGORITHM_H_
#define _MUX_ALGORITHM_H_

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "struct.h"


/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/
/* struct for mux_phy_logic info */
typedef struct 
{
    UINT_8 unit;                  /* dev's index */
    UINT_8 group_id;              /* logic_phy's group_id */
    UINT_8 phy_index;              /* logic_phy's index */
    UINT_8 phy_master;            /* logic_phy's master info */
    UINT_8 phy_comb_index[2];      /* logic_phy's combine index */
    UINT_8 phy_comb_ts_mask[4];   /* logic_phy's combine ts_mask */
    UINT_8 phy_comb_ts_num;       /* logic_phy's combine ts_num */
    UINT_8 enable;                  /* logic_phy's add or remove state */
    UINT_8 available;              /* logic_phy whether available */
}flexe_mux_logic_phy_info_t;


/* struct for phy info */
typedef struct 
{
    UINT_8 unit;                              /* dev's index */
    UINT_8 group_id;                          /* phy's group_id */
    UINT_8 phy_index;                         /* phy's index */
    flexe_phy_mode phy_mode;                  /* phy's working mode */
    UINT_32 ts_mask;                          /* phy's ts position */
    UINT_8 ts_num;                            /* phy's payload ts */
    UINT_8 enable;                            /* phy's add or remove state */
    UINT_8 available;                         /* phy whether available */
    UINT_8 phy_out_index;		  /* xc to phy_out_index*/
}flexe_mux_phy_info_t;


/* struct for flexe_mux input structure */
typedef struct 
{
    UINT_8 unit;                 /* supported multi_chips, dev's index */
    UINT_8 ch;                   /* input ts's ch */
    UINT_32 ts[TS_FIELD];        /* input ts */
    UINT_8 ts_sort[TS_NUM];      /* ts's sort index */
    UINT_8 enable;               /* add or remove traffic state */
	UINT_8 group_id;
}flexe_mux_traffic_info_t;


/* struct for traffic's mux ch info */
typedef struct 
{
    UINT_8 unit;            /* dev's index */
    UINT_8 ch_id;           /* ch_id */
    flexe_phy_mode mode;    /* ch's traffic type */
    UINT_32 ts[TS_FIELD];   /* ch's ts pos */
    UINT_8 ts_num;          /* ch's ts num */
    UINT_8 ts_min;          /* ch's min ts */
    UINT_8 enable;          /* ch's state */
	UINT_8 group_id;
}flexe_mux_ch_info_t;


/* struct for ts's info */
typedef struct 
{
    UINT_8 unit;           /* dev's index */
    UINT_8 ch_id;          /* ts belong to which ch */
    UINT_8 ts_id;          /* ts_id */
    UINT_8 w_col[TCYCLE];  /* ts's write_calculation result */
    UINT_8 r_col[TCYCLE];  /* ts's read_calculation result */
}flexe_mux_ts_info_t;

/* struct for phy_group info */
typedef struct 
{
    UINT_8 unit;
    UINT_8 group_id;
    UINT_8 phy_index[PHY_NUM];
    flexe_phy_mode group_phy_mode;
    UINT_8 phy_num;
}flexe_mux_group_info_t;


/* struct for pcs info */
typedef struct 
{
    UINT_8 unit;
    UINT_8 ch[PHY_NUM];
    UINT_8 phy_index[PHY_NUM];
    flexe_phy_mode phy_mode[PHY_NUM];
    UINT_8 pcs_num;
}pcs_info_t;


/* struct for chip's mux info */
typedef struct 
{
     UINT_8 unit;                                                             /* dev's index */
     
     UINT_8 group_num;                                                        /* group num */
     flexe_mux_group_info_t group_info[PHY_NUM];                              /* group info */

     pcs_info_t pcs_info;                                                     /* all pcs_phy_info */

      UINT_8 phy_mask;                                                         /* dev's phy mask */
     flexe_mux_phy_info_t phy[PHY_NUM];                                       /* phy's info */
     flexe_mux_logic_phy_info_t logic_phy[PHY_NUM];                           /* logic_phy info */

     UINT_32 ch_mask[CH_FIELD];                                               /* dev's configed ch */
     UINT_32 ts_mask[TS_FIELD];                                               /* dev's configed ts */
     flexe_mux_ch_info_t *flexe_ch_info[CH_NUM];                              /* mux module's ch_info */
     flexe_mux_ts_info_t *flexe_ts_info[TS_NUM];                              /* mux module's ts info */
     
     UINT_32 ch_mask_b[CH_FIELD];                                             /* dev's calendar_b configed ch */
     UINT_32 ts_mask_b[TS_FIELD];                                             /* dev's calendar_b configed ts */
      flexe_mux_ch_info_t *flexe_ch_info_b[CH_NUM];                            /* mux module's ch_info_b */
     flexe_mux_ts_info_t *flexe_ts_info_b[TS_NUM];                            /* mux module's ts info_b */

     dev_err_t (*mm_alloc)(UINT_8 unit, flexe_mm_type mm_type, UINT_8 size);  /* funcptr for channel mm_alloc */
}flexe_mux_info_t;


/* function declare */
#ifdef __cplusplus
        extern "C"
        {
#endif
extern dev_err_t flexe_mux_init(UINT_8 unit);
extern dev_err_t flexe_mux_uninit(UINT_8 unit);
extern dev_err_t flexe_mux_phy_mode_cfg(UINT_8 unit, flexe_phy_mode *phy_mode);
extern dev_err_t flexe_mux_phy_mode_out(UINT_8 unit, flexe_phy_mode *phy_mode);
extern dev_err_t flexe_mux_phy_pcs_cfg(UINT_8 unit,
                                       UINT_8 ch,
                                       UINT_8 phy_index,
                                       flexe_phy_mode phy_mode,
                                       UINT_8 enable);
extern dev_err_t flexe_mux_phy_pcs_out(UINT_8 unit,
                                       UINT_8 *pcs_num,
                                       flexe_mux_phy_info_t *phy_pcs_info);
extern dev_err_t flexe_mux_divisor_data_get(UINT_8 input_data_1, UINT_8 input_data_2, UINT_8 *result_out);
extern dev_err_t flexe_mux_mm_alloc(UINT_8 unit, flexe_mm_type mm_type, UINT_8 size);
extern dev_err_t flexe_mux_ch_mm_alloc(UINT_8 unit, UINT_8 size);
extern dev_err_t flexe_mux_ch_b_mm_alloc(UINT_8 unit, UINT_8 size);
extern dev_err_t flexe_mux_ts_mm_alloc(UINT_8 unit, UINT_8 size);
extern dev_err_t flexe_mux_ts_b_mm_alloc(UINT_8 unit, UINT_8 size);
extern dev_err_t flexe_mux_algorithm_cfg(flexe_mux_traffic_info_t traffic_info);
extern dev_err_t flexe_mux_algorithm_b_cfg(flexe_mux_traffic_info_t traffic_info);
extern dev_err_t flexe_mux_traffic_cfg(flexe_mux_traffic_info_t traffic_info);
extern dev_err_t flexe_mux_traffic_b_cfg(flexe_mux_traffic_info_t traffic_info);
extern dev_err_t flexe_mux_traffic_out(flexe_mux_traffic_info_t traffic_info,
                                       flexe_mux_ch_info_t *p_flexe_ch_info_out,
                                       flexe_mux_ts_info_t *p_flexe_ts_info_out);
extern dev_err_t flexe_mux_traffic_b_out(flexe_mux_traffic_info_t traffic_info,
                                         flexe_mux_ch_info_t *p_flexe_ch_info_out,
                                         flexe_mux_ts_info_t *p_flexe_ts_info_out);

extern dev_err_t flexe_mux_dump(UINT_8 unit);
extern dev_err_t flexe_mux_ch_dump(UINT_8 unit, UINT_8 ch);
extern dev_err_t flexe_mux_ts_dump(UINT_8 unit, UINT_8 ts);
extern dev_err_t flexe_mux_debug(void);
extern dev_err_t flexe_mux_phy_group_cfg(UINT_8 unit,
                                         UINT_8 group_id,
                                         UINT_8 phy_index,
                                         flexe_phy_mode phy_mode,
                                         UINT_8 enable);
extern dev_err_t flexe_mux_phy_group_info_get(UINT_8 unit,
                                              UINT_8 group_id,
                                              flexe_mux_group_info_t *group_info);
extern dev_err_t flexe_mux_phy_group_info_check(UINT_8 unit,
                                                UINT_8 *phy_index,
                                                UINT_8 phy_num,
                                                flexe_mux_phy_info_t *phy_info);
extern dev_err_t flexe_mux_phy_info_cfg(UINT_8 unit, flexe_mux_phy_info_t phy_info);
extern dev_err_t flexe_mux_phy_info_get(UINT_8 unit, UINT_8 phy_index, flexe_mux_phy_info_t *phy_info);
extern dev_err_t flexe_mux_chip_info_get(UINT_8 unit, flexe_mux_info_t **flexe_mux_chip_info);
extern dev_err_t flexe_mux_logic_phy_group_info_get(UINT_8 unit,
                                                    UINT_8 *phy_index,
                                                    UINT_8 phy_num,
                                                    flexe_mux_logic_phy_info_t *phy_info);
extern dev_err_t flexe_mux_logic_phy_info_cfg(UINT_8 unit, flexe_mux_logic_phy_info_t phy_info);

extern dev_err_t flexe_mux_logic_phy_out(UINT_8 unit, UINT_8 mux_logic_phy_info[]);

extern dev_err_t flexe_mux_ch_belong_group(UINT_8 unit,UINT_8 ch,UINT_8 group_id,UINT_8* status);
extern dev_err_t flexe_mux_group_ch_delete(UINT_8 unit,UINT_8 group_id);
#ifdef __cplusplus
}
#endif
#endif
