/*
 * $Id: int.c,v 1.0 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file contains INT functions.
 * INT(Inband Network Telemetry) provides a framework to help detecting 
 * and isolating network faults. This is achieved by sending a INT probe packet
 * from a source node to the destination node. Along the path, every INT-awared 
 * switch device appends the requested data to the probe packet. 
 * The requested data includes switch device's real-time states and should be 
 * processed in the data plane by hardware.
 * This module supports INT probe packet detection and processing configuration 
 * in the INT-awared switch device. This basically is done with various  
 * switch control types and per-port controls.
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/scache.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/switch.h>
#include <bcm_int/esw/switch.h>

#if defined(INCLUDE_L3)
#include <bcm_int/esw/trx.h>
#endif

#define LB_PORT_LPORT_IDX   0
#define INT_MAX_LEN_OFFSET_VALUE    32

#if defined(BCM_TRIDENT3_SUPPORT)
 /*
  * Function:
  *      _bcm_td3_int_loopback_enable
  * Purpose:
  *      Enable IPv4 L3 attribute of the loopback port for INT reply packet
  *   When sending packet to the internal loopback port, the ingress
  *   pp_port(packet process port) will be one of the front panel port. 
  *   Need to enable L3 on the pp_port. In TD3, it references the 
  *   SOURCE_TRUNK_MAP for the port attributes. By default, it uses LPORT 
  *   profile 0.
  * Parameters:
  *   IN : unit
  * Returns:
  *      BCM_E_XXX
  */

STATIC int
_bcm_td3_int_misc_enable(int unit)
{
    soc_field_t reg_fld = HIGIG2_ECN_IN_CC_ENABLEf;
    uint32 fld_value    = 1;

    /* enable IPv4 L3 on loopback port  */
    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, LPORT_TABm, 0,
                                V4L3_ENABLEf, 1));

    /* populate INT_CN in the egress object bus. Editor uses egress object
     * bus to fetch INT_CN from Egress object bus and put into INT metadata.
     */
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, EGR_CONFIG_1_64r, 
               REG_PORT_ANY, 1, &reg_fld, &fld_value));
  
    return SOC_E_NONE;
}

 /*
  * Function:
  *      _bcm_td3_int_loopback_disable
  * Purpose:
  *      Disable L3 on pp_port 
  * Parameters:
  *   IN : unit
  * Returns:
  *      BCM_E_XXX
  */

STATIC int
_bcm_td3_int_misc_disable(int unit)
{
    soc_field_t reg_fld = HIGIG2_ECN_IN_CC_ENABLEf;
    uint32 fld_value    = 0;

    SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit, LPORT_TABm, 0,
                                V4L3_ENABLEf, 0));
    SOC_IF_ERROR_RETURN(soc_reg_fields32_modify(unit, EGR_CONFIG_1_64r, 
               REG_PORT_ANY, 1, &reg_fld, &fld_value));
    return SOC_E_NONE;
}
#endif

/*
 * Function:
 *    bcmi_esw_int_switch_control_set
 * Purpose:
 *    Implement various INT switch control types to perform
 *  system wide configuration to enable INT packet detection 
 *  and processing.
 *
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to modify
 *      arg - The value with which to set the parameter
 * Returns:
 *    BCM_E_XXX
 */

int
bcmi_esw_int_switch_control_set(int unit, 
                           bcm_switch_control_t type,
                           int arg)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3(unit)) {
        uint32 reg_value;
        soc_field_t field;
        int rv;

        switch (type) {
            case bcmSwitchIntL4DestPort1:
            case bcmSwitchIntMaxPayloadLength:
                if (type == bcmSwitchIntL4DestPort1) {
                    field = UDP_DEST_PORTf; 
                    if (arg) {
                        rv = _bcm_td3_int_misc_enable(unit);
                    } else {
                        rv = _bcm_td3_int_misc_disable(unit);
                    }
                    SOC_IF_ERROR_RETURN(rv);
                } else {
                    field = MAX_PAYLOAD_LENGTHf; 
                    if (arg < INT_MAX_LEN_OFFSET_VALUE) {
                        return BCM_E_UNAVAIL;
                    }
                    arg -= INT_MAX_LEN_OFFSET_VALUE;
                }
                /* call cancun cch interface function */
                if (SOC_REG_FIELD_VALID(unit, INT_CONTROLr, field)) {
                    rv = soc_reg32_get(unit, INT_CONTROLr, REG_PORT_ANY,0,
                                             &reg_value);
                    SOC_IF_ERROR_RETURN(rv);
                    soc_reg_field_set(unit,INT_CONTROLr,&reg_value,
                               field,arg);
                    rv = soc_reg32_set(unit, INT_CONTROLr, REG_PORT_ANY, 0, 
                               reg_value);
                    SOC_IF_ERROR_RETURN(rv);
                    return SOC_E_NONE;
                } 
                break;
            case bcmSwitchIntL4DestPort2:
            case bcmSwitchIntProbeMarker1:
            case bcmSwitchIntProbeMarker2:
            case bcmSwitchIntVersion:
            case bcmSwitchIntRequestVectorValue:
            case bcmSwitchIntRequestVectorMask:
            case bcmSwitchIntSwitchId:
            case bcmSwitchIntEgressTimeDelta:
            case bcmSwitchIntHopLimitCpuEnable:
            case bcmSwitchIntTurnAroundCpuEnable:
            default:
                return BCM_E_UNAVAIL;
                break;
        }
        return BCM_E_UNAVAIL;
    }
#endif  /* defined(BCM_TRIDENT3_SUPPORT) */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *    bcmi_esw_int_control_get
 * Purpose:
 *    Get the value of various INT switch control types.
 *
 * Parameters:
 *      unit - Device unit number
 *      type - The desired configuration parameter to modify
 *      arg - The value with which to set the parameter
 * Returns:
 *    BCM_E_XXX
 */

int
bcmi_esw_int_switch_control_get(int unit, 
                           bcm_switch_control_t type,
                           int *arg)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3(unit)) {
        uint32 reg_value;
        soc_field_t field;
        int rv;

        switch (type) {
            case bcmSwitchIntL4DestPort1:
            case bcmSwitchIntMaxPayloadLength:
                /* call cancun cch interface function */
                if (type == bcmSwitchIntL4DestPort1) {
                    field = UDP_DEST_PORTf;
                } else {
                    field = MAX_PAYLOAD_LENGTHf;
                }
                /* call cancun cch interface function */
                if (SOC_REG_FIELD_VALID(unit, INT_CONTROLr, field)) {
                    rv = soc_reg32_get(unit, INT_CONTROLr, REG_PORT_ANY,0,
                                             &reg_value);
                    SOC_IF_ERROR_RETURN(rv);
                    *arg = soc_reg_field_get(unit,INT_CONTROLr,reg_value,
                               field);
                    if (type == bcmSwitchIntMaxPayloadLength) {
                        *arg += INT_MAX_LEN_OFFSET_VALUE;
                    }
                    return SOC_E_NONE;
                }
                break;

            case bcmSwitchIntL4DestPort2:
            case bcmSwitchIntProbeMarker1:
            case bcmSwitchIntProbeMarker2:
            case bcmSwitchIntVersion:
            case bcmSwitchIntRequestVectorValue:
            case bcmSwitchIntRequestVectorMask:
            case bcmSwitchIntSwitchId:
            case bcmSwitchIntEgressTimeDelta:
            case bcmSwitchIntHopLimitCpuEnable:
            case bcmSwitchIntTurnAroundCpuEnable:
            default:
                return BCM_E_UNAVAIL;
        }
        return BCM_E_UNAVAIL;
    }
#endif  /* defined(BCM_TRIDENT3_SUPPORT) */


    return BCM_E_UNAVAIL;
}
