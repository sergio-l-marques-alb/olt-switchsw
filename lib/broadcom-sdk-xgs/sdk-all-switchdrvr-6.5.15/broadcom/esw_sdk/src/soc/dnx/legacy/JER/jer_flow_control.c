#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
/* $Id: jer2_arad_flow_control.c,v 1.59 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FLOWCONTROL

/*************
 * INCLUDES  *
 *************/
#include <shared/shrextend/shrextend_debug.h>
#include <soc/mem.h>
/* { */

#include <soc/dnxc/legacy/error.h>

#include <soc/dnx/legacy/drv.h>

#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnx/legacy/ARAD/arad_flow_control.h>
#include <soc/dnx/legacy/JER/jer_flow_control.h>
#include <soc/dnx/legacy/ARAD/arad_reg_access.h>
#include <soc/dnx/legacy/ARAD/arad_tbl_access.h>
#include <soc/dnx/legacy/ARAD/arad_mgmt.h>
#include <soc/dnx/legacy/TMC/tmc_api_egr_queuing.h>
#include <soc/dnx/legacy/mbcm.h>
#include <soc/portmod/portmod.h>

#if (0)
/* { */
#include <soc/dnx/legacy/JER/jer_egr_queuing.h>
/* } */
#endif

#include <soc/mcm/allenum.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define JER2_JER_FC_REC_CAL_DEST_ARR_SIZE   7
#define JER2_JER_FC_GEN_CAL_SRC_ARR_SIZE    8


#define NUM_OF_LANES_IN_PM                4


#define GLB_RSC_MAP_TBL_NIF_PFC_OFFSET    0
#define GLB_RSC_MAP_TBL_RCY_PFC_OFFSET    8
#define GLB_RSC_MAP_TBL_HCFC_OFFSET       16

/* (2^10 - 1) */
#define JER2_JER_FC_RX_MLF_MAX           ((1 << soc_reg_field_length(unit, \
                                                                 SOC_IS_QUX(unit) ? NIF_RX_MLF_LLFC_THRESHOLDS_CONFIGr : \
                                                                 NBIH_RX_MLF_LLFC_THRESHOLDS_CONFIGr, RX_LLFC_THRESHOLD_SET_QMLF_Nf)) - 1)
/* (2^11 - 1) */
#define JER2_JER_FC_RX_MLF_HRF_MAX       ((1 << soc_reg_field_length(unit, NBIH_HRF_RX_CONFIG_HRFr, HRF_RX_LLFC_THRESHOLD_SET_HRF_Nf)) - 1)

/* JER2_JER: (2^4  - 1) ,  JER2_QAX (2^3  - 1) */
#define NIF_PFC_STATUS_SEL_MAX       ((1 << soc_reg_field_length(unit, CFC_NIF_PFC_STATUS_SELr, NIF_PFC_STATUS_SELf)) - 1)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SET_FC_ENABLE(__FC_ENA, __FIELD) {                                            \
    if (SHR_BITGET(cfc_enables->bmp, __FC_ENA)) {                                     \
        if (soc_reg_field_valid(unit, reg, __FIELD)) {                                \
            field_value = (SHR_BITGET(ena_info->bmp, __FC_ENA))? 1: 0;                \
            soc_reg_above_64_field32_set(unit, reg, reg_data, __FIELD, field_value);  \
        }                                                                             \
    }                                                                                 \
}

#define GET_FC_ENABLE(__FC_ENA, __FIELD) {                                            \
    if (SHR_BITGET(cfc_enables->bmp, __FC_ENA)) {                                     \
        if (soc_reg_field_valid(unit, reg, __FIELD)) {                                \
            field_value = soc_reg_above_64_field32_get(unit, reg, reg_data, __FIELD); \
            if (field_value) {                                                        \
                SHR_BITSET(ena_info->bmp, __FC_ENA);                                  \
            }                                                                         \
        }                                                                             \
        else {                                                                        \
            SHR_BITCLR(ena_info->bmp, __FC_ENA);                                      \
        }                                                                             \
    }                                                                                 \
}

#define FC_ENABLE_NOT_SUPPORT_JER2_JER {     \
    SOC_DNX_FC_ILKN_RX_TO_RET_REQ_EN,   \
    SOC_DNX_FC_SPI_OOB_RX_TO_RET_REQ_EN \
}

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_DNX_FC_GEN_CAL_SRC 
  jer2_jer_fc_gen_cal_src_arr[JER2_JER_FC_GEN_CAL_SRC_ARR_SIZE] = 
    { SOC_DNX_FC_GEN_CAL_SRC_STE, 
      SOC_DNX_FC_GEN_CAL_SRC_STTSTCS_TAG,
      SOC_DNX_FC_GEN_CAL_SRC_LLFC_VSQ,
      SOC_DNX_FC_GEN_CAL_SRC_PFC_VSQ,
      SOC_DNX_FC_GEN_CAL_SRC_GLB_RCS,
      SOC_DNX_FC_GEN_CAL_SRC_HCFC,
      SOC_DNX_FC_GEN_CAL_SRC_LLFC,
      SOC_DNX_FC_GEN_CAL_SRC_CONST
    };

static SOC_DNX_FC_REC_CAL_DEST 
  jer2_jer_fc_rec_cal_dest_arr[JER2_JER_FC_REC_CAL_DEST_ARR_SIZE] = 
    { SOC_DNX_FC_REC_CAL_DEST_PFC, 
      SOC_DNX_FC_REC_CAL_DEST_NIF_LL,
      SOC_DNX_FC_REC_CAL_DEST_PORT_2_PRIORITY,
      SOC_DNX_FC_REC_CAL_DEST_PORT_8_PRIORITY,
      SOC_DNX_FC_REC_CAL_DEST_GENERIC_PFC,
      SOC_DNX_FC_REC_CAL_DEST_RETRANSMIT,
      SOC_DNX_FC_REC_CAL_DEST_NONE
    };



/*************
 * FUNCTIONS *
 *************/
/* { */


int
jer2_jer_fc_rec_cal_dest_type_to_val_internal(
    DNX_SAND_IN SOC_DNX_FC_REC_CAL_DEST dest_type
  )
{
    uint32 ret = 0;

    for(ret = 0; ret < JER2_JER_FC_REC_CAL_DEST_ARR_SIZE; ret++)
    {
        if(jer2_jer_fc_rec_cal_dest_arr[ret] == dest_type)
        {
            return ret;
        }
    }

    return -1;
}

int
jer2_jer_fc_gen_cal_src_type_to_val_internal(
    DNX_SAND_IN  SOC_DNX_FC_GEN_CAL_SRC     src_type
  )
{
    uint32 ret = 0;

    for(ret = 0; ret < JER2_JER_FC_GEN_CAL_SRC_ARR_SIZE; ret++) {
        if(jer2_jer_fc_gen_cal_src_arr[ret] == src_type) {
            return ret;
        }
    }

    return -1;
}



