

#ifndef MODULE_CPB_H
#define MODULE_CPB_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/
#include "global_macro.h"
#include "datatype.h"


/******************************************************************************
*CONSTANT AND MACRO DEFINE
******************************************************************************/

/* define cpb ingress macro */
/*#define CPB_ING_CH_NUM     CH_NUM*/
/*#define CPB_ING_SCH_DEPTH  TS_NUM*/
#define CPB_ING_CH_NUM     (40)
#define CPB_ING_SCH_DEPTH  (40)
#define CPB_ING_CHK_TIMES  (5)
#define CPB_ING_CHK_DELAY  (20)
#define CPB_ING_FULL_DEPTH (128)
#define CPB_ING_FC_HIGH    (45)
#define CPB_ING_FC_LOW     (0)
#define CPB_ING_ALM_CFG    (7)

/* define cpb egress macro */
#define CPB_EGR_CH_NUM          (80)
#define CPB_EGR_BLK_NUM         (80)

/*#define CPB_EGR_CH_NUM          CH_NUM*/
/*#define CPB_EGR_BLK_NUM         TS_NUM*/
#define CPB_EGR_BP_LEVEL_OFFSET (80)
#define CPB_EGR_RD_LEVEL_BASE   (56)
#define CPB_EGR_HIGH_DEPTH_BASE (64)
#define CPB_EGR_LOW_DEPTH_BASE  (48)

/* define cpb ingress sch ram struct */
typedef struct CPB_ING_SCH_RAM
{
    UINT_8  ch;           /* ch id */
    bool    idle;         /* idle status */
    bool    pre_cfg;      /* pre cfg status */
}CPB_ING_SCH_RAM_T;

/* define cpb ingress info struct */
typedef struct CPB_ING_INFO
{
    CPB_ING_SCH_RAM_T cpb_ing_sch_ram[CPB_ING_SCH_DEPTH];   /* sch calendar ram */
    UINT_8            cpb_ing_sch_st;                       /* sch calendar state*/
    UINT_8            ts_idle_num;                          /* ts idle num */
    bool              init_done;                            /* init done */
}CPB_ING_INFO_T;

/* define cpb egress start block struct */
typedef struct CPB_EGR_CUR_BLK
{
    UINT_8  ch;           /* ch index */
    UINT_16 index;        /* ch's start block index */
    bool    idle;         /* idle state */
}CPB_EGR_CUR_BLK_T;

/* define cpb egress next block struct */
typedef struct CPB_EGR_NEXT_BLK
{
    UINT_8  ch;           /* block's ch */
    UINT_16 index;        /* ch's next block index */
    bool    idle;         /* idle status */
}CPB_EGR_NEXT_BLK_T;

/* define cpb egress info struct */
typedef struct CPB_EGR_INFO
{
    CPB_EGR_CUR_BLK_T  cpb_egr_cur_blk[CPB_EGR_CH_NUM];     /* ch start block */
    CPB_EGR_NEXT_BLK_T cpb_egr_next_blk[CPB_EGR_BLK_NUM];   /* ch next block */
    UINT_8             next_blk_idle_num;                   /* next block idle num */
    bool               init_done;                           /* init done */
}CPB_EGR_INFO_T;

#ifdef __cplusplus
extern "C"
{
#endif

extern RET_STATUS cpb_egress_ch_depth_para_set(UINT_8 chip_id,UINT_8 ch,UINT_16 ts_num);
extern RET_STATUS cpb_egress_ch_depth_para_set_for_performance(UINT_8 chip_id,UINT_8 ch,UINT_16 ts_num,UINT_8 mode);
extern RET_STATUS cpb_egress_init(UINT_8 chip_id);
extern RET_STATUS cpb_egress_ch_config(UINT_8 chip_id,UINT_8 add_del_sel,UINT_8 ch_id,UINT_16 ts_num);
extern RET_STATUS cpb_egress_debug_print(UINT_8 chip_id);
extern RET_STATUS cpb_diag(UINT_8 chip_id, UINT_8 ch_id, UINT_8 mode);
extern RET_STATUS cpb_diag_clear(UINT_8 chip_id, UINT_8 ch_id);
extern RET_STATUS cpb_debug(void);
extern UINT_8 set_cpb_delay(UINT_8 level);

#ifdef __cplusplus
}
#endif
#endif

