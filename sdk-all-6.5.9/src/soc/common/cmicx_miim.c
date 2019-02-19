/*
 * $Id: cmix_misc.c,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2016 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * CMICx MIIM (MDIO) functions
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <sal/core/sync.h>
#include <soc/util.h>
#include <soc/cmic.h>

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_MIIM

#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/iproc.h>
#endif

#include <soc/phyreg.h>

#ifdef BCM_CMICX_SUPPORT

/** see .h file   */
int
soc_cmicx_miim_init(int unit)
{
    uint32 reg_val;

    /* Give MDIO control to IPROC */
    SOC_IF_ERROR_RETURN(READ_MIIM_COMMON_CONTROLr(unit, &reg_val));
    soc_reg_field_set(unit, MIIM_COMMON_CONTROLr, &reg_val, OVRD_EXT_MDIO_MSTR_CNTRLf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MIIM_COMMON_CONTROLr(unit, reg_val));

    return SOC_E_NONE;
}


/** see .h file   */
int
soc_cmicx_miim_divider_set(int unit, int int_divider, int ext_divider, int out_delay)
{
    soc_reg_t ring_control_reg[] = {MIIM_RING0_CONTROLr, 
                                    MIIM_RING1_CONTROLr, 
                                    MIIM_RING2_CONTROLr, 
                                    MIIM_RING3_CONTROLr, 
                                    MIIM_RING4_CONTROLr, 
                                    MIIM_RING5_CONTROLr, 
                                    MIIM_RING6_CONTROLr,
                                    MIIM_RING7_CONTROLr,
                                    INVALIDr};
    uint32 reg_val, reg_addr;

    int ring_idx = 0;
    while (ring_control_reg[ring_idx] != INVALIDr) {
        reg_addr = soc_reg_addr(unit, ring_control_reg[ring_idx], REG_PORT_ANY, 0);
        SOC_IF_ERROR_RETURN(soc_pci_getreg(unit, reg_addr, &reg_val));

        if (int_divider != -1) {
            soc_reg_field_set(unit, ring_control_reg[ring_idx], &reg_val, CLOCK_DIVIDER_INTf, int_divider);
        }

        if (ext_divider != -1) {
            soc_reg_field_set(unit, ring_control_reg[ring_idx], &reg_val, CLOCK_DIVIDER_EXTf, ext_divider);
        }

        if (out_delay != -1) {
            soc_reg_field_set(unit, ring_control_reg[ring_idx], &reg_val, MDIO_OUT_DELAYf, out_delay);
        }
         
        SOC_IF_ERROR_RETURN(soc_pci_write(unit, reg_addr, reg_val));

        ring_idx++;
    }

    return SOC_E_NONE;
}

/** see .h file   */
int
soc_cmicx_miim_cl45_operation(int unit, int is_write, uint32 phy_id,
                  uint32 phy_reg_addr, uint16 *phy_data)
{
    uint32 reg_val, internal_select, real_phy_id, real_bus_id, is_done, is_error;
    uint32 devad = 0;
    soc_timeout_t to;

    SOC_INIT_FUNC_DEFS;

    /* parse phy_id to bus, phy, and internal indication
    *   phy_id encoding:
    *       bit7, 1: internal MDIO bus, 0: external MDIO bus
    *       bit9,8,6,5, mdio bus number
    *       bit4-0,   mdio addresses
    */
    internal_select = (phy_id & 0x80) ? 1 : 0;
    real_phy_id = phy_id & ~(PHY_ID_BUS_UPPER_MASK | PHY_ID_BUS_LOWER_MASK);
    real_phy_id &= ~(0x80); /*clear internal / external bit*/ 
    real_bus_id = PHY_ID_BUS_NUM(phy_id);

    /* cleanup of devad flags */
    devad = SOC_PHY_CLAUSE45_DEVAD(phy_reg_addr) & 0x1f;

    MIIM_LOCK(unit);

    /* preper transaction data */
    _SOC_IF_ERR_EXIT(READ_MIIM_CH0_ADDRESSr(unit, &reg_val));
    soc_reg_field_set(unit, MIIM_CH0_ADDRESSr, &reg_val, CLAUSE_45_REG_ADRRf, SOC_PHY_CLAUSE45_REGAD(phy_reg_addr));
    soc_reg_field_set(unit, MIIM_CH0_ADDRESSr, &reg_val, CLAUSE_45_DTYPEf, devad);
    soc_reg_field_set(unit, MIIM_CH0_ADDRESSr, &reg_val, PHY_IDf, real_phy_id);
    _SOC_IF_ERR_EXIT(WRITE_MIIM_CH0_ADDRESSr(unit, reg_val));

    _SOC_IF_ERR_EXIT(READ_MIIM_CH0_PARAMSr(unit, &reg_val));
    soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, SEL_INT_PHYf, internal_select);
    soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, SEL_CL45f, 1); /* this function handles only CL45*/
    soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, RING_MAPf, (1 << real_bus_id));
    soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, RD_ENf, is_write ? 0 : 1);
    soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, WR_ENf, is_write ? 1 : 0);

    if(is_write) {
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, PHY_WR_DATAf, *phy_data);
    } else {
        soc_reg_field_set(unit, MIIM_CH0_PARAMSr, &reg_val, PHY_WR_DATAf, 0x0);
    }

    _SOC_IF_ERR_EXIT(WRITE_MIIM_CH0_PARAMSr(unit, reg_val));

    /* start transaction */
    reg_val = 0;
    soc_reg_field_set(unit, MIIM_CH0_CONTROLr, &reg_val, STARTf, 0x1);
    _SOC_IF_ERR_EXIT(WRITE_MIIM_CH0_CONTROLr(unit, reg_val));

    /* poll for DONE bit*/
    soc_timeout_init(&to, SOC_CONTROL(unit)->miimTimeout, 2500);

    while (1){
        _SOC_IF_ERR_EXIT(READ_MIIM_CH0_STATUSr(unit, &reg_val));
        is_done = soc_reg_field_get(unit, MIIM_CH0_STATUSr, reg_val, DONEf);
        if (is_done) {
            break; /* MIIM operation is done */
        }

        if (soc_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_SOC_MIIM,
                      (BSL_META_U(unit,
                                  "MiimTimeOut: %s, "
                                  "timeout (id=0x%02x addr=0x%02x)\n"),
                                    is_write ? "write" : "read" , 
                                    phy_id, phy_reg_addr));
            _SOC_IF_ERR_EXIT(SOC_E_TIMEOUT);
        }
    }

    LOG_DEBUG(BSL_LS_SOC_MIIM,
                (BSL_META_U(unit,
                            "  Done in %d polls\n"), to.polls));

    /* check for transaction error */
    is_error = soc_reg_field_get(unit, MIIM_CH0_STATUSr, reg_val, ERRORf);
    if (is_error) {
        LOG_ERROR(BSL_LS_SOC_MIIM,
                    (BSL_META_U(unit,
                                "  MDIO transaction Error\n")));
        _SOC_IF_ERR_EXIT(SOC_E_FAIL);
    }

    /* in case of read - get data */
    if (!is_write) {
        *phy_data = (uint16)soc_reg_field_get(unit, MIIM_CH0_STATUSr, reg_val, PHY_RD_DATAf);
        LOG_VERBOSE(BSL_LS_SOC_MIIM,
                     (BSL_META_U(unit,
                                 "soc_dcmn_miim_operation read data: %d \n"),*phy_data)); 
    }

    /* cleanup */
    reg_val = 0;
    soc_reg_field_set(unit, MIIM_CH0_CONTROLr, &reg_val, STARTf, 0x0);
    _SOC_IF_ERR_EXIT(WRITE_MIIM_CH0_CONTROLr(unit, reg_val));

exit:
    /* in case we failed - try to cleanup */
    if (SOC_FUNC_ERROR) {
        reg_val = 0;
        soc_reg_field_set(unit, MIIM_CH0_CONTROLr, &reg_val, STARTf, 0x0);
        WRITE_MIIM_CH0_CONTROLr(unit, reg_val); /* no need to catch error in case of failrue */
    }

    /* lock release */
    MIIM_UNLOCK(unit);

    SOC_FUNC_RETURN

}



#endif /* BCM_CMICX_SUPPORT  */
