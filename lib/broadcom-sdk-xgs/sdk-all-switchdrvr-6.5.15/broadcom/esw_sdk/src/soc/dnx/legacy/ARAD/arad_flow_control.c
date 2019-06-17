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
#include <shared/swstate/access/sw_state_access.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/mem.h>
/* { */

#include <soc/dnxc/legacy/error.h>


#include <soc/dnx/legacy/drv.h>

#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

#include <soc/dnx/legacy/ARAD/arad_flow_control.h>
#include <soc/dnx/legacy/ARAD/arad_reg_access.h>
#include <soc/dnx/legacy/ARAD/arad_tbl_access.h>
#include <soc/dnx/legacy/ARAD/arad_mgmt.h>
#include <soc/dnx/legacy/TMC/tmc_api_egr_queuing.h>
#include <soc/dnx/legacy/mbcm.h>

#include <soc/mcm/allenum.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define JER2_ARAD_NIF_ILKN_ID_A -1
#define JER2_ARAD_NIF_ILKN_ID_B -1

#define JER2_ARAD_ILKN_NDX_MIN                                        (JER2_ARAD_NIF_ILKN_ID_A)
#define JER2_ARAD_ILKN_NDX_MAX                                        (JER2_ARAD_NIF_ILKN_ID_B)
#define JER2_ARAD_FC_CLASS_MAX                                        7
#define JER2_ARAD_CAL_MODE_NDX_MAX                                    (SOC_DNX_FC_NOF_CAL_MODES-1)

#define JER2_ARAD_EGQ_CNM_CPID_TO_FC_TYPE_REG_NOF_FLDS 3

#define JER2_ARAD_FC_REC_CAL_DEST_ARR_SIZE   7
#define JER2_ARAD_FC_GEN_CAL_SRC_ARR_SIZE    8

#define JER2_ARAD_FC_PFC_GENERIC_BITMAP_SIZE SOC_DNX_FC_PFC_GENERIC_BITMAP_SIZE
#define JER2_ARAD_FC_HCFC_BITMAPS 8

/* Retransmit values for RX/TX calendars */
#define JER2_ARAD_FC_RETRANSMIT_DEST_ILKN_A_VAL 0x0 
#define JER2_ARAD_FC_RETRANSMIT_DEST_ILKN_B_VAL 0x2 

/* ILKN Multi-use TX CAL */
#define JER2_ARAD_FC_ILKN_MUB_TX_CAL_SOURCE_ENTRY_OFFSET 9


/* (2^10 - 1) */
#define JER2_ARAD_FC_RX_MLF_MAX           ((1 << soc_reg_field_length(unit, NBI_RX_MLF_LLFC_THRESHOLDS_CONFIGr, RX_N_LLFC_THRESHOLD_SETf)) - 1)
/* (2^11 - 1) */
#define JER2_ARAD_FC_RX_MLF_HRF_MAX       ((1 << soc_reg_field_length(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, HRF_RX_N_LLFC_THRESHOLD_SETf)) - 1)

#define JER2_ARAD_FC_RX_MLF_HRF_NOF_BITS  (soc_reg_field_length(unit, NBI_HRF_RX_CONTROLLER_CONFIG_1r, HRF_RX_N_LLFC_THRESHOLD_SETf))

#define JER2_ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MIN 1
#define JER2_ARAD_FC_ILKN_RETRANSMIT_CAL_LEN_MAX 2


/* Ardon FCV (flow control vector) input bit length */
#define ARDON_FC_FCV_BIT_LEN 256


#define FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_MAX               SOC_DNX_EGR_NOF_Q_PRIO_JER2_ARAD
#define FC_NOF_INDICATION_IN_NIF_PFC_MAP_ENTRY_JER2_ARAD              4

/* } */
/*************
 * MACROS    *
 *************/
/* { */
#define JER2_ARAD_FC_CAL_MODE_IS_ILKN(c_mode) \
  DNX_SAND_NUM2BOOL((c_mode == SOC_DNX_FC_CAL_MODE_ILKN_INBND) || (c_mode ==SOC_DNX_FC_CAL_MODE_ILKN_OOB))


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

#define FC_ENABLE_NOT_SUPPORT_JER2_ARAD {    \
    SOC_DNX_FC_GLB_RSC_TO_NIF_LLFC_EN,  \
    SOC_DNX_FC_GLB_RSC_TO_NIF_PFC_EN,   \
    SOC_DNX_FC_NIF_TO_GEN_PFC_EN,       \
    SOC_DNX_FC_STAT_VSQ_TO_HCFC_EN      \
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
  jer2_arad_fc_gen_cal_src_arr[JER2_ARAD_FC_GEN_CAL_SRC_ARR_SIZE] = 
    { SOC_DNX_FC_GEN_CAL_SRC_STE, 
      SOC_DNX_FC_GEN_CAL_SRC_LLFC_VSQ,
      SOC_DNX_FC_GEN_CAL_SRC_PFC_VSQ,
      SOC_DNX_FC_GEN_CAL_SRC_GLB_RCS,
      SOC_DNX_FC_GEN_CAL_SRC_HCFC,
      SOC_DNX_FC_GEN_CAL_SRC_LLFC,
      SOC_DNX_FC_GEN_CAL_SRC_RETRANSMIT,
      SOC_DNX_FC_GEN_CAL_SRC_CONST
    };

static SOC_DNX_FC_REC_CAL_DEST 
  jer2_arad_fc_rec_cal_dest_arr[JER2_ARAD_FC_REC_CAL_DEST_ARR_SIZE] = 
    { SOC_DNX_FC_REC_CAL_DEST_PFC, 
      SOC_DNX_FC_REC_CAL_DEST_NIF_LL,
      SOC_DNX_FC_REC_CAL_DEST_PORT_2_PRIORITY,
      SOC_DNX_FC_REC_CAL_DEST_PORT_8_PRIORITY,
      SOC_DNX_FC_REC_CAL_DEST_GENERIC_PFC,
      SOC_DNX_FC_REC_CAL_DEST_RETRANSMIT,
      SOC_DNX_FC_REC_CAL_DEST_NONE
    };

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */






int
jer2_arad_fc_rec_cal_dest_type_to_val_internal(
    DNX_SAND_IN SOC_DNX_FC_REC_CAL_DEST dest_type
  )
{
    uint32 ret;

    for(ret = 0; ret < JER2_ARAD_FC_REC_CAL_DEST_ARR_SIZE; ret++)
    {
        if(jer2_arad_fc_rec_cal_dest_arr[ret] == dest_type)
        {
            return ret;
        }
    }

    return -1;
}

int
jer2_arad_fc_gen_cal_src_type_to_val_internal(
    DNX_SAND_IN  int                        unit,
    DNX_SAND_IN  SOC_DNX_FC_GEN_CAL_SRC     src_type
  )
{
    uint32 ret;

    for(ret = 0; ret < JER2_ARAD_FC_GEN_CAL_SRC_ARR_SIZE; ret++) {
        if(jer2_arad_fc_gen_cal_src_arr[ret] == src_type) {
            return ret;
        }
    }

    return -1;
}


