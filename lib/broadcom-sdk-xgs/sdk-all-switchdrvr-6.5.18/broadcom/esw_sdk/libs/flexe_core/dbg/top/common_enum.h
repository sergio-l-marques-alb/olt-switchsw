
#ifndef COMMON_ENUM_H
#define COMMON_ENUM_H

/******************************************************************************
*INCLUDE FILES
******************************************************************************/




/*Structure for enumerated type for OTN segment type.*/
typedef enum 
{
    OTN_SEG_PM   = 0,
    OTN_SEG_TCM1 = 1,
    OTN_SEG_TCM2 = 2, 
    OTN_SEG_TCM3 = 3,
    OTN_SEG_TCM4 = 4,
    OTN_SEG_TCM5 = 5,
    OTN_SEG_TCM6 = 6,
    OTN_SEG_SM   = 7,  
    LAST_OTN_SEG     /* this is for out of range values error handling */
}otn_seg_t;

/*Structure for enumerated type for OTN alm type.*/
typedef enum
{
    OTN_ALM_AIS = 0,
    OTN_ALM_OCI = 1,
    OTN_ALM_LCK = 2,
    OTN_ALM_LTC = 3,
    OTN_ALM_TIM = 4,
    OTN_ALM_BDI = 5,
    OTN_ALM_IAE = 6,
    OTN_ALM_BIAE = 7,
    OTN_ALM_ATSF = 8,
    OTN_ALM_SD = 9,
    OTN_ALM_SF = 10,
    OTN_ALM_SD_ALL = 11,
    OTN_ALM_FRAME_SLIP = 12,
    OTN_ALM_OTUC_AIS = 13,
    OTN_ALM_PLM = 14,
    OTN_ALM_RSSF = 15,
    OTN_ALM_LOF = 16,
    OTN_ALM_LOM = 17,
    OTN_ALM_OOM = 18,
    OTN_ALM_LOFLOM = 19,
    OTN_ALM_LAST    /* this is for out of range values error handling */
}otn_alm_t;

/*Structure for enumerated type for res type.*/
typedef enum
{
    OTN_RES_1R14C = 0,    
    OTN_RES_2R1C = 1,
    OTN_RES_2R2C = 2,    
    OTN_RES_2R4C = 3,
    OTN_EXP_2R14C = 4,
    OTN_EXP_3R13C = 5,    
    OTN_EXP_3R14C = 6,
    OTN_RES_4R9C = 7,    
    OTN_RES_4R10C = 8,
    OTN_RES_4R11C = 9,    
    OTN_RES_4R12C = 10,
    OTN_RES_4R13C = 11,
    OTN_RES_4R14C = 12,
    OTN_RES_LAST
}otn_res_t;

/*Structure for enumerated type for src sel type.*/
typedef enum 
{
    src_ttisel = 0,
    src_bip8sel = 1,
    src_bip8_debug = 2,
    src_bdisel = 3,
    src_bei_biaesel = 4,
    src_statsel = 5,
    src_apspccsel = 6,
    src_expsel_r2c4 = 7,
    src_expsel_r2c14 = 8,
    src_expsel_r3c13 = 9,
    src_expsel_r3c14 = 10,
    src_ressel_r1c3 = 11,
    src_ressel__r1c14 = 12,
    src_ressel_r2c1 = 13,
    src_ressel_r2c2 = 14,
    src_ressel_r4c9 = 15,
    src_ressel_r4c10 = 16,
    src_ressel_r4c11 = 17,
    src_ressel_r4c12 = 18,
    src_ressel_r4c13l = 19,
    src_ressel_r4c14 = 20,
    src_gcc1sel = 21,
    src_gcc2sel = 22,
    src_fassel = 23,
    src_mfassel = 24,
    src_psisel = 25,
    src_osmcsel = 26,
    src_gcc0sel = 27,
    src_smiaesel = 28,
    src_smressel = 29,
    src_dmsel = 30,
    src_sel_last
}src_sel_t;

/*Structure for enumerated type for src sel type.*/
typedef enum 
{
    tti_oh = 0,
    bip8_oh = 1,
    bei_oh = 2,
    bdi_oh = 3,
    iae_oh = 4,
    stat_oh = 5,
    oh_last
}oh_type_t;

/* enum device's state */
typedef enum 
{
    DEV_SUCCESS          = 0,  /* dev operate success */
    DEV_FAIL             = 1,  /* dev operate fail */
    DEV_INVALID          = 2,  /* dev's unit invalid */
    DEV_INITIALIZED      = 3,  /* dev have already initialized */
    DEV_DEINITIALIZED    = 4,  /* dev not initialized */
    DEV_POINTER_NULL     = 5,  /* pointer null */
    DEV_MM_ALLOC_FAIL    = 6,  /* malloc memory fail */
    DEV_PARA_ERR         = 7,  /* parameter error */
    DEV_CH_UNAVAILABLE   = 8,  /* unavailable ch */
    DEV_TS_UNAVAILABLE   = 9,  /* unavailable ts */
    DEV_DEFAULT                /* default value */
}dev_err_t;

/* enum memory alloc type */
typedef enum 
{
    FLEXE_MM_CH    = 0,       /* malloc memory for ch_info */
    FLEXE_MM_TS    = 1,       /* malloc memory for ts_info */
    FLEXE_MM_CH_B  = 2,       /* malloc memory for ch_info_b */
    FLEXE_MM_TS_B  = 3,       /* malloc memory for ts_info_b */
    FLEXE_DEFAULT             /* default value */
}flexe_mm_type;

/* enum phy_mode */
typedef enum 
{
    FLEXE_PHY_50G  = 0,       /* phy's flexe_50g working mode */
    FLEXE_PHY_100G = 1,       /* phy's flexe_100g working mode */
    PCS_PHY_50G    = 2,       /* phy's pcs_50g working mode */
    PCS_PHY_100G   = 3,       /* phy's pcs_100g working mode */
    PCS_PHY_200G   = 4,       /* phy's pcs_200g working mode */
	PCS_PHY_400G   = 5,
    FLEXE_PHY_DEFAULT         /* default value */
}flexe_phy_mode;

/* enum client_state */
typedef enum 
{
    CLIENT_INITIALIZED    = 0,        /* client_initialized state */
    CLIENT_TRAFFIC_ADD    = 1,        /* client_traffic_add state */
    CLIENT_TRAFFIC_REMOVE = 2,        /* client_traffic_remove state */
    CLIENT_DEFAULT                    /* default value */
}client_st;
#endif
