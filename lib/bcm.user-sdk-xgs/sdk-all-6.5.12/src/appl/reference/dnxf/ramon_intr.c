/*
 * $Id: ramon_intr.c, v1 Broadcom SDK $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:    Implements application interrupt lists for RAMON.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <soc/dnxf/ramon/ramon_intr.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm_int/common/debug.h>
#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/system.h>
#include <appl/dcmn/interrupts/dcmn_intr.h>

/*************
 * DEFINES   *
 *************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_INTR

/*************
 * TYPE DEFS *
 *************/
ramon_interrupt_type ramon_int_disable_on_init[] = {
    INVALIDr
};

ramon_interrupt_type ramon_int_active_on_init[] = {
    /*
     * list interrupt vector 
     */
    RAMON_INT_CCH_ERROR_ECC,
    RAMON_INT_LCM_ERROR_ECC,
    RAMON_INT_LCM_LCM_INT_REG,
    RAMON_INT_RTP_ERROR_ECC,
    RAMON_INT_RTP_GENERAL_INT_REG,
    RAMON_INT_MCT_ERROR_ECC,
    RAMON_INT_QRH_ERROR_ECC,
    RAMON_INT_QRH_DRH_INT_REG,
    RAMON_INT_QRH_CRH_INT_REG,
    RAMON_INT_DCML_ERROR_ECC,
    RAMON_INT_DCML_ERROR_FPC_FREE,
    RAMON_INT_DCML_ERROR_FPC_ALLOC,
    RAMON_INT_DCH_ERROR_ECC,
    RAMON_INT_FSRD_ERROR_ECC,
    RAMON_INT_FMAC_ERROR_ECC,
    RAMON_INT_FMAC_INT_REG_1,
    RAMON_INT_FMAC_INT_REG_2,
    RAMON_INT_FMAC_INT_REG_3,
    RAMON_INT_FMAC_INT_REG_4,
    RAMON_INT_FMAC_INT_REG_5,
    RAMON_INT_FMAC_INT_REG_6,
    RAMON_INT_FMAC_INT_REG_7,
    RAMON_INT_FMAC_INT_REG_8,
    RAMON_INT_ECI_ERROR_ECC,
    /*
     * ecc/parity interrupts list 
     */
    RAMON_INT_CCH_ECC_PARITY_ERR_INT,
    RAMON_INT_CCH_ECC_ECC_1B_ERR_INT,
    RAMON_INT_CCH_ECC_ECC_2B_ERR_INT,
    RAMON_INT_LCM_ECC_PARITY_ERR_INT,
    RAMON_INT_LCM_ECC_ECC_1B_ERR_INT,
    RAMON_INT_LCM_ECC_ECC_2B_ERR_INT,
    RAMON_INT_RTP_ECC_PARITY_ERR_INT,
    RAMON_INT_RTP_ECC_ECC_1B_ERR_INT,
    RAMON_INT_RTP_ECC_ECC_2B_ERR_INT,
    RAMON_INT_MCT_ECC_PARITY_ERR_INT,
    RAMON_INT_MCT_ECC_ECC_1B_ERR_INT,
    RAMON_INT_MCT_ECC_ECC_2B_ERR_INT,
    RAMON_INT_QRH_ECC_PARITY_ERR_INT,
    RAMON_INT_QRH_ECC_ECC_1B_ERR_INT,
    RAMON_INT_QRH_ECC_ECC_2B_ERR_INT,
    RAMON_INT_DCML_ECC_PARITY_ERR_INT,
    RAMON_INT_DCML_ECC_ECC_1B_ERR_INT,
    RAMON_INT_DCML_ECC_ECC_2B_ERR_INT,
    RAMON_INT_DCH_ECC_PARITY_ERR_INT,
    RAMON_INT_DCH_ECC_ECC_1B_ERR_INT,
    RAMON_INT_DCH_ECC_ECC_2B_ERR_INT,
    RAMON_INT_FSRD_ECC_PARITY_ERR_INT,
    RAMON_INT_FSRD_ECC_ECC_1B_ERR_INT,
    RAMON_INT_FSRD_ECC_ECC_2B_ERR_INT,
    RAMON_INT_FMAC_ECC_PARITY_ERR_INT,
    RAMON_INT_FMAC_ECC_ECC_1B_ERR_INT,
    RAMON_INT_FMAC_ECC_ECC_2B_ERR_INT,
    RAMON_INT_ECI_ECC_PARITY_ERR_INT,
    RAMON_INT_ECI_ECC_ECC_1B_ERR_INT,
    RAMON_INT_ECI_ECC_ECC_2B_ERR_INT,

    INVALIDr
};

ramon_interrupt_type ramon_int_disable_print_on_init[] = {
    INVALIDr
};

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/*************
 * FUNCTIONS *
 *************/
int
ramon_interrupt_cmn_param_init(
    int unit,
    intr_common_params_t * common_params)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(common_params, _SHR_E_PARAM, "common_params");

    common_params->int_disable_on_init = ramon_int_disable_on_init;
    common_params->int_disable_print_on_init = ramon_int_disable_print_on_init;
    common_params->int_active_on_init = ramon_int_active_on_init;

exit:
    SHR_FUNC_EXIT;

}

#undef BSL_LOG_MODULE
