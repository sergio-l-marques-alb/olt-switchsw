/*
 * $Id: esw_switch.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        switch.c
 * Purpose:     BCM definitions  for bcm_switch_control and
 *              bcm_switch_port_control functions
 */

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/hash.h>
#if defined(BCM_EASYRIDER_SUPPORT)
#include <soc/easyrider.h>
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT)
#include <soc/lpm.h>
#endif /* BCM_FIREBOLT2_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#include <bcm/switch.h>
#include <bcm/error.h>

#include <bcm_int/esw/vlan.h>
#include <bcm_int/esw/port.h>

#if defined(INCLUDE_L3)
#include <bcm_int/esw/draco.h>
#endif  /* INCLUDE_L3 */

#if defined(BCM_XGS3_SWITCH_SUPPORT)
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
#if defined(BCM_TRX_SWITCH_SUPPORT)
#include <bcm_int/esw/trx.h>
#endif  /* BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif  /* BCM_TRIUMPH_SUPPORT */

#include <bcm_int/control.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw_dispatch.h>

typedef int (*xlate_arg_f)(int unit, int arg, int set);

typedef struct {
    bcm_switch_control_t	type;
    uint32			chip;
    soc_reg_t			reg;
    soc_field_t			field;
    xlate_arg_f			xlate_arg;
    soc_feature_t               feature;
} bcm_switch_binding_t;

/* chip field */
#define FB      SOC_INFO_FB
#define FB2     SOC_INFO_FIREBOLT2
#define ER      SOC_INFO_EASYRIDER
#define HX      SOC_INFO_HELIX_ANY
#define FX      SOC_INFO_FELIX_ANY
#define HXFX    SOC_INFO_FX_HX
#define BR      SOC_INFO_BRADLEY
#define GW      SOC_INFO_GOLDWING
#define HBGW    SOC_INFO_HB_GW
#define RP      SOC_INFO_RAPTOR
#define TR      SOC_INFO_TRIUMPH
#define RV   	SOC_INFO_RAVEN
#define SC   	SOC_INFO_SCORPION
#define TRX   	SOC_INFO_TRX
#define HK   	SOC_INFO_HAWKEYE
#define TRVL    SOC_INFO_TR_VL
#define SCQ   	SOC_INFO_SC_CQ
#define TR2   	SOC_INFO_TRIUMPH2
#define AP   	SOC_INFO_APOLLO
#define VL2   	SOC_INFO_VALKYRIE2
#define EN      SOC_INFO_ENDURO

/*
 * Function:
 *	    _bcm_esw_switch_control_gport_resolve
 * Description:
 *	    Decodes local physical port from a gport
 * Parameters:
 *	    unit - StrataSwitch PCI device unit number (driver internal).
 *      gport - a gport to decode
 *      port - (Out) Local physical port encoded in gport
 * Returns:
 *      BCM_E_xxxx
 * Note
 *      In case of gport contains other then local port error will be returned.
 */


STATIC int
_bcm_esw_switch_control_gport_resolve(int unit, bcm_gport_t gport, bcm_port_t *port)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    bcm_port_t      local_port;
    int             id, isMymodid;

    if (NULL == port) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, gport, &modid, &local_port, &tgid, &id));

    if ((BCM_TRUNK_INVALID != tgid) || (-1 != id)) {
        return BCM_E_PARAM;
    }
    /* Check if modid is local */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_modid_is_local(unit, modid, &isMymodid));

    if (isMymodid != TRUE) {
        return BCM_E_PARAM;
    }

    return _bcm_gport_modport_api2hw_map(unit, modid, local_port, &modid, port);
}


#ifdef BCM_XGS12_SWITCH_SUPPORT
/*
 * Function:
 *	_bcm_llcmatch_set
 * Description:
 *	Set the LLC_MATCH (SAP_VALUE0 or SAP_VALUE1, depending on the type
 *      parameter). If top byte of an argument is zero, the low byte is
 *      replicated.
 *      All switch chip ports are configured with the same settings.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_llcmatch_set(int unit,
                  bcm_port_t port,
		  bcm_switch_control_t type,
		  int arg)
{
    uint64	llc_match;
    soc_field_t	field;

    if (arg & ~0xffff) {
        return BCM_E_PARAM;
    }
    if (arg < 0x100) {
    	arg |= arg << 8;
    }

    switch (type) {
        case bcmSwitchLLCSAP1:
	    field = SAP_VALUE0f;
	    break;
        case bcmSwitchLLCSAP2:
	    field = SAP_VALUE1f;
	    break;
        default:
	    return BCM_E_PARAM;
    }

    if (SOC_IS_DRACO1(unit) || SOC_IS_LYNX(unit) || SOC_IS_TUCANA(unit)) {
        BCM_IF_ERROR_RETURN(READ_LLC_MATCHr(unit, port, &llc_match));
        soc_reg64_field32_set(unit, LLC_MATCHr, &llc_match,
                              field, arg);
        BCM_IF_ERROR_RETURN(WRITE_LLC_MATCHr(unit, port, llc_match));

	return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	_bcm_llcmatch_get
 * Description:
 *	Get the current LLC_MATCH settings. Value-returns SAP_VALUE0 or
 *      SAP_VALUE1, depending on the type parameter. If both bytes of
 *      the SAP_VALUE are the same, the top byte is masked off.
 *      All switch chip ports are configured with the same settings.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_llcmatch_get(int unit,
                  bcm_port_t port,
		  bcm_switch_control_t type,
		  int *arg)
{
    uint64	llc_match;
    soc_field_t	field;

    switch (type) {
        case bcmSwitchLLCSAP1:
	    field = SAP_VALUE0f;
	    break;
        case bcmSwitchLLCSAP2:
	    field = SAP_VALUE1f;
	    break;
        default:
	    return BCM_E_PARAM;
    }

    if (SOC_IS_DRACO1(unit) || SOC_IS_LYNX(unit) || SOC_IS_TUCANA(unit)) {
        BCM_IF_ERROR_RETURN(READ_LLC_MATCHr(unit, port, &llc_match));
	*arg = soc_reg64_field32_get(unit, LLC_MATCHr, llc_match, field);
	if ((*arg & 0xff) == (*arg >> 8)) {
	    *arg &= 0xff;
	}
	return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	_bcm_hashselect_set
 * Description:
 *	Set the Hash Select for L2. L3 and Multipath types
 *      are not supported on these devices.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - BCM_HASH_*
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_hashselect_set(int unit,
		    bcm_switch_control_t type,
		    int arg)
{
    uint32      hash_control, val=0;

    if (type != bcmSwitchHashL2) {
        return BCM_E_UNAVAIL;
    }

    switch (arg) {
        case BCM_HASH_ZERO:
            val = XGS_HASH_ZERO;
            break;
        case BCM_HASH_LSB:
            val = XGS_HASH_LSB;
            break;
        case BCM_HASH_CRC16L:
            val = XGS_HASH_CRC16_LOWER;
            break;
        case BCM_HASH_CRC16U:
            val = XGS_HASH_CRC16_UPPER;
            break;
        case BCM_HASH_CRC32L:
            val = XGS_HASH_CRC32_LOWER;
            break;
        case BCM_HASH_CRC32U:
            val = XGS_HASH_CRC32_UPPER;
            break;
        default:
            return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      HASH_SELECTf, val);
    BCM_IF_ERROR_RETURN(WRITE_HASH_CONTROLr(unit, hash_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_hashselect_get
 * Description:
 *	Get the current Hash Select settings. Value returned
 *      is of the form BCM_HASH_*.
 *      All switch chip ports are configured with the same settings.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_hashselect_get(int unit,
		    bcm_switch_control_t type,
		    int *arg)
{
    uint32	hash_control, val=0;

    if (type != bcmSwitchHashL2) {
        return BCM_E_UNAVAIL;
    }
    BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
    val = soc_reg_field_get(unit, HASH_CONTROLr,
                            hash_control, HASH_SELECTf);
    switch (val) {
        case XGS_HASH_ZERO:
            *arg = BCM_HASH_ZERO;
            break;
        case XGS_HASH_LSB:
            *arg = BCM_HASH_LSB;
            break;
        case XGS_HASH_CRC16_LOWER:
            *arg = BCM_HASH_CRC16L;
            break;
        case XGS_HASH_CRC16_UPPER:
            *arg = BCM_HASH_CRC16U;
            break;
        case XGS_HASH_CRC32_LOWER:
            *arg = BCM_HASH_CRC32L;
            break;
        case XGS_HASH_CRC32_UPPER:
            *arg = BCM_HASH_CRC32U;
            break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_hashcontrol_set
 * Description:
 *	Set the Hash Control.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      arg  - BCM_HASH_CONTROL_*
 *             Only BCM_HASH_CONTROL_TRUNK_UC_XGS2 (default) and
 *             BCM_HASH_CONTROL_MULTIPATH_DIP are acceptable.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_hashcontrol_set(int unit, int arg)
{
    if (SOC_IS_DRACO15(unit)) {
        uint32	hash_control, val=0;
        int     mask;

        /* Check for invalid parameters */
        mask = (BCM_HASH_CONTROL_MULTIPATH_L4PORTS |
                BCM_HASH_CONTROL_MULTIPATH_USERDEF(0xff) |
                BCM_HASH_CONTROL_TRUNK_UC_SRCPORT |
                BCM_HASH_CONTROL_TRUNK_NUC_DST |
                BCM_HASH_CONTROL_TRUNK_NUC_SRC |
                BCM_HASH_CONTROL_TRUNK_NUC_MODPORT);

        if (arg & mask) {
            return BCM_E_PARAM;
        }

        val = (arg & BCM_HASH_CONTROL_MULTIPATH_DIP) ? 1 : 0;

        BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
        soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          ECMP_HASH_SELECTf, val);
        BCM_IF_ERROR_RETURN(WRITE_HASH_CONTROLr(unit, hash_control));

        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	_bcm_hashcontrol_get
 * Description:
 *	Get the current Hash Control settings. Value returned
 *      is of the form BCM_HASH_CONTROL*.
 *      All switch chip ports are configured with the same settings.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_hashcontrol_get(int unit, int *arg)
{
    if (SOC_IS_DRACO15(unit)) {
        uint32	hash_control, val=0;

        BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
        val = soc_reg_field_get(unit, HASH_CONTROLr,
                                hash_control, ECMP_HASH_SELECTf);

        *arg = BCM_HASH_CONTROL_TRUNK_UC_XGS2;
        if (val == 1) {
            *arg |= BCM_HASH_CONTROL_MULTIPATH_DIP;
        }
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	_bcm_pkt_age_set
 * Description:
 *	Set packet aging for XGS1/XGS2.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal)
 *      arg - Aging time in msec
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_pkt_age_set(int unit, int arg)
{
    if (SOC_IS_DRACO15(unit)) {
        if (!arg) {
            /* Disable packet aging on all COSQs */
            SOC_IF_ERROR_RETURN(WRITE_PKTAGETIMERr(unit, 0));
            SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, 0)); /* Aesthetic */
        } else if ((arg < 0) || (arg > 7339)) {
            return BCM_E_PARAM;
        } else {
            uint32 duration;
            /* Divide by 0.016, then divide by 7 (rounded up) */
            duration = ((((125 * arg) / 2) + 6) / 7) & 0xffff;
            /* Set all COSQs to the same value */
            SOC_IF_ERROR_RETURN(WRITE_PKTAGETIMERr(unit, duration));
            SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, 0));
        }
        return BCM_E_NONE;
    }
    if (SOC_IS_DRACO1(unit) || SOC_IS_LYNX(unit)) {
        if (!arg) {
            SOC_IF_ERROR_RETURN(WRITE_PKTAGETIMERr(unit, 0x1fff));
        } else if ((arg < 0) || (arg > 4095500)) {
            return BCM_E_PARAM;
        } else {
            /* Tick appear to be around 5 msec */
            uint32 duration = (arg + 4) / 5;
            SOC_IF_ERROR_RETURN(WRITE_PKTAGETIMERr(unit, duration));
        }
        return BCM_E_NONE;
    }
    if (SOC_IS_TUCANA(unit)) {
        if (!arg) {
            /* Disable packet aging */
            SOC_IF_ERROR_RETURN(WRITE_AGING_THRESHOLDr(unit, 0));
        } else if ((arg < 0) || (arg > 163830)) {
            return BCM_E_PARAM;
        } else {
            SOC_IF_ERROR_RETURN(WRITE_AGING_THRESHOLDr(unit, (arg + 9) / 10));
        }
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	_bcm_pkt_age_get
 * Description:
 *	Get packet aging for XGS1/XGS2.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      arg - Pointer to where the retrieved value will be written (msec).
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_pkt_age_get(int unit, int *arg)
{
    if (SOC_IS_DRACO15(unit)) {
        uint32 duration;
        SOC_IF_ERROR_RETURN(READ_PKTAGETIMERr(unit, &duration));
        *arg = (duration * 7 * 2) / 125;
        return BCM_E_NONE;
    }
    if (SOC_IS_DRACO1(unit) || SOC_IS_LYNX(unit)) {
        uint32 duration;
        SOC_IF_ERROR_RETURN(READ_PKTAGETIMERr(unit, &duration));
        if (duration == 0x1fff) {
            *arg = 0;
        } else {
            *arg = duration * 5;
        }
        return BCM_E_NONE;
    }
    if (SOC_IS_TUCANA(unit)) {
        uint32 regval;
        SOC_IF_ERROR_RETURN(READ_AGING_THRESHOLDr(unit, &regval));
        *arg = soc_reg_field_get(unit, AGING_THRESHOLDr,
                                 regval, PKTAGETIMERf) * 10;
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

/*
 * CPU control register set/get functions.
 * If port is specified (port < 0), only val or ival is used as appropriate.
 * Skip stack ports if port is not specified.
 */

STATIC int
_bcm_sw_cc_get(int unit, bcm_port_t port, uint64 *val)
{
    if (IS_E_PORT(unit, port)) {
        return READ_CPU_CONTROLr(unit, port, val);
    } else if (IS_HG_PORT(unit, port)) {
        return READ_ICPU_CONTROLr(unit, port, val);
    }

    return BCM_E_PORT;
}

STATIC int
_bcm_sw_cc_set(int unit, bcm_port_t port, uint64 val)
{
    if (IS_E_PORT(unit, port)) {
        return WRITE_CPU_CONTROLr(unit, port, val);
    } else if (IS_HG_PORT(unit, port)) {
        return WRITE_ICPU_CONTROLr(unit, port, val);
    }

    return BCM_E_PORT;
}

/* Map unit/port/type to a field in CPU CONTROL. Returns INVALIDf on error */

STATIC soc_field_t
_bcm_sw_field_get(int unit, bcm_port_t port, bcm_switch_control_t type)
{
    if (SOC_IS_LYNX(unit) && !IS_E_PORT(unit, port)) {
        switch (type) { /* Check for invalid ops for HG ports */
        case bcmSwitchUnknownIpmcToCpu:
        case bcmSwitchUnknownMcastToCpu:
        case bcmSwitchUnknownUcastToCpu:
        case bcmSwitchL3HeaderErrToCpu:
        case bcmSwitchUnknownL3SrcToCpu:
        case bcmSwitchUnknownL3DestToCpu:
        case bcmSwitchIpmcPortMissToCpu:
        case bcmSwitchVCLabelMissToCpu:
        case bcmSwitchSourceRouteToCpu:
        case bcmSwitchIpmcErrorToCpu:
            return INVALIDf;
        default:
            break;
        }
    }

    /* Choose the field */
    switch(type) {
    case bcmSwitchCpuProtocolPrio:
        return CPU_PROTOCOL_PRIORITYf;
        break;
    case bcmSwitchCpuUnknownPrio:
        return CPU_LKUPFAIL_PRIORITYf;
        break;
    case bcmSwitchCpuDefaultPrio:
        return CPU_DEFAULT_PRIORITYf;
        break;
    case bcmSwitchL2StaticMoveToCpu:
        return STATICMOVE_TOCPUf;
        break;
    case bcmSwitchUnknownVlanToCpu:
        return UVLAN_TOCPUf;
        break;
    case bcmSwitchUnknownIpmcToCpu:
        return UIPMC_TOCPUf;
        break;
    case bcmSwitchUnknownMcastToCpu:
        return UMC_TOCPUf;
        break;
    case bcmSwitchUnknownUcastToCpu:
        return UUCAST_TOCPUf;
        break;
    case bcmSwitchL3HeaderErrToCpu:
        return L3ERR_TOCPUf;
        break;
    case bcmSwitchUnknownL3SrcToCpu:
        return UNRESOLVEDL3SRC_TOCPUf;
        break;
    case bcmSwitchUnknownL3DestToCpu:
        return L3DSTMISS_TOCPUf;
        break;
    case bcmSwitchIpmcPortMissToCpu:
        return IPMCPORTMISS_TOCPUf;
        break;
    case bcmSwitchVCLabelMissToCpu:
        if (SOC_IS_TUCANA(unit)) {
            return VCLABELMISS_TOCPUf;
        }
        break;
    case bcmSwitchSourceRouteToCpu:
        if (SOC_IS_DRACO15(unit)) {
            return SRCROUTE_TOCPUf;
        }
        break;
    case bcmSwitchIpmcErrorToCpu:
        if (SOC_IS_DRACO15(unit)) {
            return IPMCERR_TOCPUf;
        }
        break;
    default:
        break;
    }

    return INVALIDf;
}
#endif

#ifdef BCM_XGS3_SWITCH_SUPPORT

/*
 * Function:
 *	_bcm_vrf_max_get
 * Description:
 *	Get the current maximum vrf value.
 * Parameters:
 *	unit - (IN) BCM device number.
 *  arg -  (OUT)Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_vrf_max_get(int unit, int *arg)
{
    *arg = SOC_VRF_MAX(unit);
    return (BCM_E_NONE);
}


/* Helper routines for argument translation */

STATIC int
_bool_invert(int unit, int arg, int set)
{
    /* Same for both set/get */
    return !arg;
}

/* Helper routines for "chip-wide" packet aging set/get */

STATIC int
_bcm_fb_pkt_age_set(int unit, int arg)
{
    if (!arg) {
        /* Disable packet aging on all COSQs */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, 0));
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, 0)); /* Aesthetic */
    } else if ((arg < 0) || (arg > 7162)) {
        return BCM_E_PARAM;
    } else {
        /* Set all COSQs to the same value */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, ((8 * arg) + 6) / 7));
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMITr(unit, 0));
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_er_pkt_age_set(int unit, int arg)
{
    if (!arg) {
        /* Disable packet aging */
        SOC_IF_ERROR_RETURN(WRITE_AGING_THRESHOLDr(unit, 0));
    } else if ((arg < 0) || (arg > 163830)) {
        return BCM_E_PARAM;
    } else {
        SOC_IF_ERROR_RETURN(WRITE_AGING_THRESHOLDr(unit, (arg + 9) / 10));
    }

    return BCM_E_NONE;
}

#ifdef BCM_TRX_SUPPORT
STATIC int
_bcm_ts_pkt_age_set(int unit, int arg)
{
    if (!arg) {
        /* Disable packet aging on all COSQs */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, 0));
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMIT0r(unit, 0)); /* Aesthetic */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMIT1r(unit, 0)); /* Aesthetic */
    } else if ((arg < 0) || (arg > 7162)) {
        return BCM_E_PARAM;
    } else {
        /* Set all COSQs to the same value */
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGTIMERr(unit, ((8 * arg) + 6) / 7));
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMIT0r(unit, 0));
        SOC_IF_ERROR_RETURN(WRITE_PKTAGINGLIMIT1r(unit, 0));
    }

    return BCM_E_NONE;
}
#endif

STATIC int
_bcm_fb_pkt_age_get(int unit, int *arg)
{
    uint32 timerval, limitval, mask;
    int i, loopcount;

    /* Check that all COSQs have the same value */
    SOC_IF_ERROR_RETURN(READ_PKTAGINGLIMITr(unit, &limitval));
    limitval &= 0xFFFFFF; /* AGINGLIMITCOSn fields only */
    loopcount = mask = soc_reg_field_get(unit, PKTAGINGLIMITr,
					 limitval, AGINGLIMITCOS0f);
    /* Make mask of eight identical AGINGLIMITCOSn fields */
    for (i = 1; i < 8; i++) {
        mask = (mask << 3) | loopcount;
    }

    if (mask != limitval) {
        /* COSQs currently are not programmed identically */
        return BCM_E_CONFIG;
    }

    /* Return the (common) setting */
    loopcount = 7 - loopcount;
    SOC_IF_ERROR_RETURN(READ_PKTAGINGTIMERr(unit, &timerval));

    *arg =
      (loopcount *
       soc_reg_field_get(unit, PKTAGINGTIMERr, timerval, DURATIONSELECTf)) / 8;

    return BCM_E_NONE;
}

STATIC int
_bcm_er_pkt_age_get(int unit, int *arg)
{
    uint32 regval;
    SOC_IF_ERROR_RETURN(READ_AGING_THRESHOLDr(unit, &regval));
    *arg = soc_reg_field_get(unit, AGING_THRESHOLDr,
			     regval, PKTAGETIMERf) * 10;
    return BCM_E_NONE;
}

#ifdef BCM_TRX_SUPPORT
STATIC int
_bcm_ts_pkt_age_get(int unit, int *arg)
{
    uint32 timerval, limitval, mask;
    int i, loopcount;

    /* Check that all COSQs have the same value */
    SOC_IF_ERROR_RETURN(READ_PKTAGINGLIMIT0r(unit, &limitval));
    limitval &= 0xFFFFFF; /* AGINGLIMITPRIn fields only */
    loopcount = mask = soc_reg_field_get(unit, PKTAGINGLIMIT0r,
					 limitval, AGINGLIMITPRI0f);
    /* Make mask of eight identical AGINGLIMITPRIn fields */
    for (i = 1; i < 8; i++) {
        mask = (mask << 3) | loopcount;
    }

    if (mask != limitval) {
        /* COSQs currently are not programmed identically */
        return BCM_E_CONFIG;
    }

    /* Check second register */
    SOC_IF_ERROR_RETURN(READ_PKTAGINGLIMIT1r(unit, &limitval));
    limitval &= 0xFFFFFF; /* AGINGLIMITPRIn fields only */
    if (mask != limitval) {
        /* COSQs currently are not programmed identically */
        return BCM_E_CONFIG;
    }

    /* Return the (common) setting */
    loopcount = 7 - loopcount;
    SOC_IF_ERROR_RETURN(READ_PKTAGINGTIMERr(unit, &timerval));

    *arg =
      (loopcount *
       soc_reg_field_get(unit, PKTAGINGTIMERr, timerval, DURATIONSELECTf)) / 8;

    return BCM_E_NONE;
}
#endif

STATIC int
_bcm_fb_er_color_mode_set(int unit, bcm_port_t port, int arg)
{
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        uint32               val, oval, rval, cfi_as_cng;

        BCM_IF_ERROR_RETURN
            (READ_EGR_VLAN_CONTROL_1r(unit, port, &val));

        oval = val;

        switch (arg) {
        case BCM_COLOR_PRIORITY:
            cfi_as_cng = 0;
            break;
        case BCM_COLOR_OUTER_CFI:
            cfi_as_cng = 0xf;
            break;
        case BCM_COLOR_INNER_CFI:
            cfi_as_cng = 0x1;
            break;
        default:
            return BCM_E_PARAM;
        }

        soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &val,
                          CFI_AS_CNGf, cfi_as_cng);
        if (oval != val) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_port_config_set(unit, port,
                                          _bcmPortCfiAsCng,
                                          cfi_as_cng));
            /* Workaround for FB2 HW bug. For ports with DT_MODE = 0,
             * CNG is incorrectly picked from CNG_MAPr
             * instead of ING_PRI_CNG_MAP table.
             * GNATS 9151 */
            if (SOC_IS_FIREBOLT2(unit)) {
                BCM_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &rval));
                if((soc_reg_field_get(unit, ING_CONFIGr, rval,
                                      DT_MODEf) == 0) &&
                   (arg == BCM_COLOR_OUTER_CFI ||
                    arg == BCM_COLOR_INNER_CFI)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_esw_port_config_set(unit, port,
                                                  _bcmPortNni,
                                                  1));
                }
            }
            BCM_IF_ERROR_RETURN
                (WRITE_EGR_VLAN_CONTROL_1r(unit, port, val));
        }
    } else
#endif
    if (SOC_IS_HUMV(unit)) {
        return BCM_E_UNAVAIL;
    } else {
        uint32 val, oval, eval = 0, oeval;
        int inner_cfg = soc_feature(unit, soc_feature_color_inner_cfi);
        soc_reg_t cfg_reg, egr_cfg_reg;
#if defined(BCM_EASYRIDER_SUPPORT)
        if (SOC_IS_EASYRIDER(unit)) {
            cfg_reg =  SEER_CONFIGr;
            egr_cfg_reg = INVALIDr;
        } else
#endif /* BCM_EASYRIDER_SUPPORT */
        {
            cfg_reg = ING_CONFIGr;
            egr_cfg_reg = EGR_CONFIGr;
        }

        SOC_IF_ERROR_RETURN(soc_reg_read_any_block(unit, cfg_reg, &val));
        oval = val;
        if (egr_cfg_reg != INVALIDr) {
            SOC_IF_ERROR_RETURN
                (soc_reg_read_any_block(unit, egr_cfg_reg, &eval));
        }
        oeval = eval;
        switch (arg) {
        case BCM_COLOR_PRIORITY:
            soc_reg_field_set(unit, cfg_reg, &val, CFI_AS_CNGf, 0);
            if (inner_cfg) {
                 soc_reg_field_set(unit, cfg_reg, &val, CVLAN_CFI_AS_CNGf, 0);
            }
            if (egr_cfg_reg != INVALIDr) {
                soc_reg_field_set(unit, egr_cfg_reg, &eval, CFI_AS_CNGf, 0);
            }
            break;
        case BCM_COLOR_OUTER_CFI:
            soc_reg_field_set(unit, cfg_reg, &val, CFI_AS_CNGf, 1);
            if (inner_cfg) {
                soc_reg_field_set(unit, cfg_reg, &val, CVLAN_CFI_AS_CNGf, 0);
            }
            if (egr_cfg_reg != INVALIDr) {
                soc_reg_field_set(unit, egr_cfg_reg, &eval, CFI_AS_CNGf, 1);
            }
            break;
        case BCM_COLOR_INNER_CFI:
            if (inner_cfg) {
                soc_reg_field_set(unit, cfg_reg, &val, CFI_AS_CNGf, 0);
                soc_reg_field_set(unit, cfg_reg, &val, CVLAN_CFI_AS_CNGf, 1);
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        default:
            return BCM_E_PARAM;
        }
        if (oval != val) {
            SOC_IF_ERROR_RETURN(soc_reg_write_all_blocks(unit, cfg_reg, val));
        }
        if ((egr_cfg_reg != INVALIDr) && (oeval != eval)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_write_all_blocks(unit, egr_cfg_reg, eval));
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_fb_er_color_mode_get(int unit, bcm_port_t port, int *arg)
{

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) \
 || defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_TRX(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        uint32               val, cfi_as_cng;

        BCM_IF_ERROR_RETURN
            (READ_EGR_VLAN_CONTROL_1r(unit, port, &val));

        cfi_as_cng = soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r, val,
                                       CFI_AS_CNGf);
        if (cfi_as_cng == 0) {
            *arg = BCM_COLOR_PRIORITY;
        } else if (cfi_as_cng == 1) {
            *arg = BCM_COLOR_INNER_CFI;
        } else {
            *arg = BCM_COLOR_OUTER_CFI;
        }
    } else
#endif
    {
        uint32 val;
        soc_reg_t cfg_reg;
#if defined(BCM_EASYRIDER_SUPPORT)
        if (SOC_IS_EASYRIDER(unit)) {
            cfg_reg =  SEER_CONFIGr;
        } else
#endif /* BCM_EASYRIDER_SUPPORT */
        {
            cfg_reg = ING_CONFIGr;
        }

        SOC_IF_ERROR_RETURN(soc_reg_read_any_block(unit, cfg_reg, &val));

        
        if (soc_reg_field_get(unit, cfg_reg, val, CFI_AS_CNGf)) {
            *arg = BCM_COLOR_OUTER_CFI;
        } else if (soc_feature(unit, soc_feature_color_inner_cfi) &&
                   soc_reg_field_get(unit, cfg_reg, val, CVLAN_CFI_AS_CNGf)) {
            *arg = BCM_COLOR_INNER_CFI;
        } else {
            *arg = BCM_COLOR_PRIORITY;
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb_er_hashselect_set
 * Description:
 *      Set the Hash Select for L2, L3 or Multipath type.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - BCM_HASH_*
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_fb_er_hashselect_set(int unit, bcm_switch_control_t type, int arg)
{
    uint32      hash_control, val=0;
    soc_field_t field;
    soc_reg_t   hash_reg = HASH_CONTROLr;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
    int         dual = FALSE;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

    switch (type) {
        case bcmSwitchHashL2:
            field = L2_AND_VLAN_MAC_HASH_SELECTf;
            break;
        case bcmSwitchHashL3:
            if (soc_reg_field_valid(unit, HASH_CONTROLr, L3_HASH_SELECTf) &&
                soc_feature(unit, soc_feature_l3)) {
                field = L3_HASH_SELECTf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashMultipath:
            if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_SELf) &&
                soc_feature(unit, soc_feature_l3)) {
                field = ECMP_HASH_SELf;
                if ((arg == BCM_HASH_CRC16L) || (arg == BCM_HASH_CRC16U)) {
                    return BCM_E_PARAM;
                }
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
        case bcmSwitchHashL2Dual:
            dual = TRUE;
            hash_reg = L2_AUX_HASH_CONTROLr;
            field = HASH_SELECTf;
            break;
        case bcmSwitchHashL3Dual:
            if (soc_reg_field_valid(unit, L3_AUX_HASH_CONTROLr, HASH_SELECTf) &&
                soc_feature(unit, soc_feature_l3)) {
                dual = TRUE;
                hash_reg = L3_AUX_HASH_CONTROLr;
                field = HASH_SELECTf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT)
        case bcmSwitchHashIpfixIngress:
	    if (soc_reg_field_valid(unit, ING_IPFIX_HASH_CONTROLr, HASH_SELECT_Af)) {
                hash_reg = ING_IPFIX_HASH_CONTROLr;
                field = HASH_SELECT_Af;
	    } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashIpfixEgress:
	    if (soc_reg_field_valid(unit, EGR_IPFIX_HASH_CONTROLr, HASH_SELECT_Af)) {
	        hash_reg = EGR_IPFIX_HASH_CONTROLr;
	        field = HASH_SELECT_Af;
	    } else {
	        return BCM_E_UNAVAIL;
	    }
            break;
        case bcmSwitchHashIpfixIngressDual:
	    if (soc_reg_field_valid(unit, ING_IPFIX_HASH_CONTROLr, HASH_SELECT_Bf)) {
	        hash_reg = ING_IPFIX_HASH_CONTROLr;
	        field = HASH_SELECT_Bf;
	    } else {
	        return BCM_E_UNAVAIL;
	    }
            break;
        case bcmSwitchHashIpfixEgressDual:
	    if (soc_reg_field_valid(unit, EGR_IPFIX_HASH_CONTROLr, HASH_SELECT_Bf)) {
	        hash_reg = EGR_IPFIX_HASH_CONTROLr;
	        field = HASH_SELECT_Af;
	    } else {
	        return BCM_E_UNAVAIL;
	    }
            break;
#else
        case bcmSwitchHashIpfixIngress:
        case bcmSwitchHashIpfixEgress:
        case bcmSwitchHashIpfixIngressDual:
        case bcmSwitchHashIpfixEgressDual:
            return BCM_E_UNAVAIL;
#endif
#if defined(BCM_TRX_SUPPORT)
        case bcmSwitchHashVlanTranslate:
	    if (soc_reg_field_valid(unit, VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Af)) {
                hash_reg = VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Af;
	    } else {
	        return BCM_E_UNAVAIL;
	    }
            break;
        case bcmSwitchHashVlanTranslateDual:
	    if (soc_reg_field_valid(unit, VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Bf)) {
                hash_reg = VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Bf;
	    } else {
	        return BCM_E_UNAVAIL;
	    }
            break;
#else
        case bcmSwitchHashVlanTranslate:
        case bcmSwitchHashVlanTranslateDual:
            return BCM_E_UNAVAIL;
#endif /* BCM_TRX_SUPPORT */
        default:
            return BCM_E_PARAM;
    }

    switch (arg) {
        case BCM_HASH_ZERO:
            val = FB_HASH_ZERO;
            break;
        case BCM_HASH_LSB:
            val = FB_HASH_LSB;
            break;
        case BCM_HASH_CRC16L:
            val = FB_HASH_CRC16_LOWER;
            break;
        case BCM_HASH_CRC16U:
            val = FB_HASH_CRC16_UPPER;
            break;
        case BCM_HASH_CRC32L:
            val = FB_HASH_CRC32_LOWER;
            break;
        case BCM_HASH_CRC32U:
            val = FB_HASH_CRC32_UPPER;
            break;
        default:
            return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_read_any_block(unit, hash_reg, &hash_control));
    soc_reg_field_set(unit, hash_reg, &hash_control, field, val);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
    if (dual) {
        uint32 base_hash_control, base_hash;
        soc_field_t base_field;

        base_field = (type == bcmSwitchHashL2Dual) ?
            L2_AND_VLAN_MAC_HASH_SELECTf : L3_HASH_SELECTf;

        BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &base_hash_control));
        base_hash = soc_reg_field_get(unit, HASH_CONTROLr,
                                      base_hash_control, base_field);
        soc_reg_field_set(unit, hash_reg, &hash_control,
                          ENABLEf, (base_hash == val) ? 0 : 1);
#if defined (BCM_RAVEN_SUPPORT)
	/* Also write to VLAN_MAC_AUX_HASH_CONTROL for bcmSwitchHashL2Dual */
	if (type == bcmSwitchHashL2Dual &&
            SOC_REG_IS_VALID(unit, VLAN_MAC_AUX_HASH_CONTROLr)) {
    	    SOC_IF_ERROR_RETURN
                (READ_VLAN_MAC_AUX_HASH_CONTROLr(unit, &hash_control));
    	    soc_reg_field_set(unit, VLAN_MAC_AUX_HASH_CONTROLr, &hash_control,
				HASH_SELECTf, val);
            soc_reg_field_set(unit, VLAN_MAC_AUX_HASH_CONTROLr, &hash_control,
                              ENABLEf, (base_hash == val) ? 0 : 1);
   	    SOC_IF_ERROR_RETURN
                (WRITE_VLAN_MAC_AUX_HASH_CONTROLr(unit, hash_control));
	}
#endif /* BCM_RAVEN_SUPPORT */
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
    SOC_IF_ERROR_RETURN
        (soc_reg_write_all_blocks(unit, hash_reg, hash_control));

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit) && (type == bcmSwitchHashL2)) {
        if ((arg != BCM_HASH_CRC16L) && (arg != BCM_HASH_CRC16U)) {
            BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
            soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                              L2_EXT_HASH_SELECTf, val);
            BCM_IF_ERROR_RETURN(WRITE_HASH_CONTROLr(unit, hash_control));
        }
    }
#endif /* BCM_EASYRIDER_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_fb_er_hashselect_get
 * Description:
 *      Get the current Hash Select settings. Value returned
 *      is of the form BCM_HASH_*.
 *      All switch chip ports are configured with the same settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_fb_er_hashselect_get(int unit, bcm_switch_control_t type, int *arg)
{
    uint32      hash_control, val=0;
    soc_reg_t   hash_reg = HASH_CONTROLr;
    soc_field_t field;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
    int         dual = FALSE;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

    switch (type) {
        case bcmSwitchHashL2:
            field = L2_AND_VLAN_MAC_HASH_SELECTf;
            break;
        case bcmSwitchHashL3:
            if (soc_reg_field_valid(unit, HASH_CONTROLr, L3_HASH_SELECTf) &&
                soc_feature(unit, soc_feature_l3)) {
                field = L3_HASH_SELECTf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashMultipath:
            if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_SELf) &&
                soc_feature(unit, soc_feature_l3)) {
                field = ECMP_HASH_SELf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashIpfixIngress:
            if (soc_reg_field_valid(unit, ING_IPFIX_HASH_CONTROLr, HASH_SELECT_Af) ) {
                hash_reg = ING_IPFIX_HASH_CONTROLr;
                field = HASH_SELECT_Af;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashIpfixEgress:
            if (soc_reg_field_valid(unit, EGR_IPFIX_HASH_CONTROLr, HASH_SELECT_Af) ) {
                hash_reg = EGR_IPFIX_HASH_CONTROLr;
                field = HASH_SELECT_Af;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashIpfixIngressDual:
            if (soc_reg_field_valid(unit, ING_IPFIX_HASH_CONTROLr, HASH_SELECT_Bf) ) {
                hash_reg = ING_IPFIX_HASH_CONTROLr;
                field = HASH_SELECT_Bf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchHashIpfixEgressDual:
            if (soc_reg_field_valid(unit, EGR_IPFIX_HASH_CONTROLr, HASH_SELECT_Bf) ) {
                hash_reg = EGR_IPFIX_HASH_CONTROLr;
                field = HASH_SELECT_Bf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
        case bcmSwitchHashL2Dual:
            dual = TRUE;
            hash_reg = L2_AUX_HASH_CONTROLr;
            field = HASH_SELECTf;
            break;
        case bcmSwitchHashL3Dual:
            if (soc_reg_field_valid(unit, L3_AUX_HASH_CONTROLr, HASH_SELECTf) &&
                soc_feature(unit, soc_feature_l3)) {
                dual = TRUE;
                hash_reg = L3_AUX_HASH_CONTROLr;
                field = HASH_SELECTf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
        case bcmSwitchHashVlanTranslate:
	    if (soc_reg_field_valid(unit, VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Af)) {
                hash_reg = VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Af;
	    } else {
	        return BCM_E_UNAVAIL;
	    }
            break;
        case bcmSwitchHashVlanTranslateDual:
	    if (soc_reg_field_valid(unit, VLAN_XLATE_HASH_CONTROLr, HASH_SELECT_Bf)) {
                hash_reg = VLAN_XLATE_HASH_CONTROLr;
                field = HASH_SELECT_Bf;
	    } else {
	        return BCM_E_UNAVAIL;
	    }
            break;
#else
        case bcmSwitchHashVlanTranslate:
        case bcmSwitchHashVlanTranslateDual:
            return BCM_E_UNAVAIL;
#endif /* BCM_TRX_SUPPORT */
        default:
            return BCM_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (soc_reg_read_any_block(unit, hash_reg, &hash_control));
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
    if (dual &&
        !soc_reg_field_get(unit, hash_reg, hash_control, ENABLEf)) {
        /* Dual hash not enabled, just return primary hash */
        field = (type == bcmSwitchHashL2Dual) ?
            L2_AND_VLAN_MAC_HASH_SELECTf : L3_HASH_SELECTf;
        hash_reg = HASH_CONTROLr;
        BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
    val = soc_reg_field_get(unit, hash_reg, hash_control, field);

    switch (val) {
        case FB_HASH_ZERO:
            *arg = BCM_HASH_ZERO;
            break;
        case FB_HASH_LSB:
            *arg = BCM_HASH_LSB;
            break;
        case FB_HASH_CRC16_LOWER:
            *arg = BCM_HASH_CRC16L;
            break;
        case FB_HASH_CRC16_UPPER:
            *arg = BCM_HASH_CRC16U;
            break;
        case FB_HASH_CRC32_LOWER:
            *arg = BCM_HASH_CRC32L;
            break;
        case FB_HASH_CRC32_UPPER:
            *arg = BCM_HASH_CRC32U;
            break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_hashcontrol_set
 * Description:
 *      Set the Hash Control for L2, L3 or Multipath type.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_hashcontrol_set(int unit, int arg)
{
    uint32      hash_control, val=0;

    if (!soc_feature(unit, soc_feature_l3)) {
        if ((arg & BCM_HASH_CONTROL_MULTIPATH_L4PORTS) ||
            (arg & BCM_HASH_CONTROL_MULTIPATH_DIP)) {
            return BCM_E_UNAVAIL;
        }
    }

    BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));

#if defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_EASYRIDER(unit)) {
        val = (arg & BCM_HASH_CONTROL_MULTIPATH_L4PORTS) ? 0 : 1;
        soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          ECMP_HASH_NO_TCP_UDP_PORTSf, val);
    } else
#endif /* BCM_EASYRIDER_SUPPORT */
    {
        val = (arg & BCM_HASH_CONTROL_MULTIPATH_L4PORTS) ? 1 : 0;
        soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          USE_TCP_UDP_PORTSf, val);
    }

    if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_USE_DIPf)) {
        val = (arg & BCM_HASH_CONTROL_MULTIPATH_DIP) ? 1 : 0;
        soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          ECMP_HASH_USE_DIPf, val);

        val = BCM_HASH_CONTROL_MULTIPATH_USERDEF_VAL(arg);
        soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          ECMP_HASH_UDFf, val);
    }

    val = (arg & BCM_HASH_CONTROL_TRUNK_UC_XGS2) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      ENABLE_DRACO1_5_HASHf, val);

    val = (arg & BCM_HASH_CONTROL_TRUNK_UC_SRCPORT) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      UC_TRUNK_HASH_USE_SRC_PORTf, val);

    val = (arg & BCM_HASH_CONTROL_TRUNK_NUC_DST) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      NON_UC_TRUNK_HASH_DST_ENABLEf, val);

    val = (arg & BCM_HASH_CONTROL_TRUNK_NUC_SRC) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      NON_UC_TRUNK_HASH_SRC_ENABLEf, val);

    val = (arg & BCM_HASH_CONTROL_TRUNK_NUC_MODPORT) ? 1 : 0;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      NON_UC_TRUNK_HASH_MOD_PORT_ENABLEf, val);

#ifdef BCM_BRADLEY_SUPPORT
    if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
        val = (arg & BCM_HASH_CONTROL_ECMP_ENHANCE) ? 1 : 0;
        soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          ECMP_HASH_USE_RTAG7f, val);

        val = (arg & BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE) ? 1 : 0;
        soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                          NON_UC_TRUNK_HASH_USE_RTAG7f, val);
    }
#endif /* BCM_BRADLEY_SUPPORT */

    BCM_IF_ERROR_RETURN(WRITE_HASH_CONTROLr(unit, hash_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_hashcontrol_get
 * Description:
 *      Get the current Hash Control settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_hashcontrol_get(int unit, int *arg)
{
    uint32      hash_control, val=0;

    *arg = 0;
    BCM_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));


    if (soc_feature(unit, soc_feature_l3)) {
#if defined(BCM_EASYRIDER_SUPPORT)
        if (SOC_IS_EASYRIDER(unit)) {
            val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                    ECMP_HASH_NO_TCP_UDP_PORTSf);
            if (!val) *arg |= BCM_HASH_CONTROL_MULTIPATH_L4PORTS;
        } else
#endif /* BCM_EASYRIDER_SUPPORT */
        {
            if (soc_reg_field_valid(unit, HASH_CONTROLr, USE_TCP_UDP_PORTSf)) {
                val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                        USE_TCP_UDP_PORTSf);
                if (val) *arg |= BCM_HASH_CONTROL_MULTIPATH_L4PORTS;
            }
        }
        if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_USE_DIPf)) {
            val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                    ECMP_HASH_USE_DIPf);
            if (val) *arg |= BCM_HASH_CONTROL_MULTIPATH_DIP;
        }

        if (soc_reg_field_valid(unit, HASH_CONTROLr, ECMP_HASH_UDFf)) {
            val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                    ECMP_HASH_UDFf);
            *arg |= BCM_HASH_CONTROL_MULTIPATH_USERDEF(val);
        }
    }

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            ENABLE_DRACO1_5_HASHf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_UC_XGS2;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            UC_TRUNK_HASH_USE_SRC_PORTf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_UC_SRCPORT;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            NON_UC_TRUNK_HASH_DST_ENABLEf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_NUC_DST;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            NON_UC_TRUNK_HASH_SRC_ENABLEf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_NUC_SRC;

    val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                            NON_UC_TRUNK_HASH_MOD_PORT_ENABLEf);
    if (val) *arg |= BCM_HASH_CONTROL_TRUNK_NUC_MODPORT;

#ifdef BCM_BRADLEY_SUPPORT
    if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
        val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                ECMP_HASH_USE_RTAG7f);
        if (val) *arg |= BCM_HASH_CONTROL_ECMP_ENHANCE;

        val = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                NON_UC_TRUNK_HASH_USE_RTAG7f);
        if (val) *arg |= BCM_HASH_CONTROL_TRUNK_NUC_ENHANCE;
    }
#endif /* BCM_BRADLEY_SUPPORT */

    return BCM_E_NONE;
}

/* IGMP/MLD values */
#define BCM_SWITCH_FORWARD_VALUE        0
#define BCM_SWITCH_DROP_VALUE           1
#define BCM_SWITCH_FLOOD_VALUE          2
#define BCM_SWITCH_RESERVED_VALUE       3

/*
 * Function:
 *      _bcm_xgs3_igmp_action_set
 * Description:
 *      Set the IGMP/MLD registers
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - IGMP / MLD actions
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_igmp_action_set(int unit,
                         bcm_port_t port,
                         bcm_switch_control_t type,
                         int arg)
{
    uint32      values[3];
    soc_field_t fields[3];
    int         idx;
    soc_reg_t   reg = INVALIDr ;
    int         fcount = 1;
    int         value = (arg) ? 1 : 0;

    for (idx = 0; idx < COUNTOF(values); idx++) {
        values[idx] = value;
        fields[idx] = INVALIDf;
    }
#if defined (BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)
    if (soc_feature(unit, soc_feature_igmp_mld_support)) {
        reg = IGMP_MLD_PKT_CONTROLr;
        /* Given control type select register field. */
        switch (type) {
            case bcmSwitchIgmpPktToCpu:
                fields[0] = IGMP_REP_LEAVE_TO_CPUf;
                fields[1] = IGMP_QUERY_TO_CPUf;
                fields[2] = IGMP_UNKNOWN_MSG_TO_CPUf;
                fcount = 3;
                break;
            case bcmSwitchIgmpPktDrop:
                fields[0] = IGMP_REP_LEAVE_FWD_ACTIONf;
                fields[1] = IGMP_QUERY_FWD_ACTIONf;
                fields[2] = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                fcount = 3;
                values[0] = values[1] = values[2] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldPktToCpu:
                fields[0] = MLD_REP_DONE_TO_CPUf;
                fields[1] = MLD_QUERY_TO_CPUf;
                fcount = 2;
                break;
            case bcmSwitchMldPktDrop:
                fields[0] = MLD_REP_DONE_FWD_ACTIONf;
                fields[1] = MLD_QUERY_FWD_ACTIONf;
                fcount = 2;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchV4ResvdMcPktToCpu:
                fields[0] = IPV4_RESVD_MC_PKT_TO_CPUf;
                break;
            case bcmSwitchV4ResvdMcPktFlood:
                fields[0] = IPV4_RESVD_MC_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchV4ResvdMcPktDrop:
                fields[0] = IPV4_RESVD_MC_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchV6ResvdMcPktToCpu:
                fields[0] = IPV6_RESVD_MC_PKT_TO_CPUf;
                break;
            case bcmSwitchV6ResvdMcPktFlood:
                fields[0] = IPV6_RESVD_MC_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchV6ResvdMcPktDrop:
                fields[0] = IPV6_RESVD_MC_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveDrop:
                fields[0] = IGMP_REP_LEAVE_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveFlood:
                fields[0] = IGMP_REP_LEAVE_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveToCpu:
                fields[0] = IGMP_REP_LEAVE_TO_CPUf;
                break;
            case bcmSwitchIgmpQueryDrop:
                fields[0] = IGMP_QUERY_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpQueryFlood:
                fields[0] = IGMP_QUERY_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpQueryToCpu:
                fields[0] = IGMP_QUERY_TO_CPUf;
                break;
            case bcmSwitchIgmpUnknownDrop:
                fields[0] = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpUnknownFlood:
                fields[0] = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIgmpUnknownToCpu:
                fields[0] = IGMP_UNKNOWN_MSG_TO_CPUf;
                break;
            case bcmSwitchMldReportDoneDrop:
                fields[0] = MLD_REP_DONE_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldReportDoneFlood:
                fields[0] = MLD_REP_DONE_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldReportDoneToCpu:
                fields[0] = MLD_REP_DONE_TO_CPUf;
                break;
            case bcmSwitchMldQueryDrop:
                fields[0] = MLD_QUERY_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldQueryFlood:
                fields[0] = MLD_QUERY_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchMldQueryToCpu:
                fields[0] = MLD_QUERY_TO_CPUf;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryDrop:
                fields[0] = IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryFlood:
                fields[0] = IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryToCpu:
                fields[0] = IPV4_MC_ROUTER_ADV_PKT_TO_CPUf;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryDrop:
                fields[0] = IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryFlood:
                fields[0] = IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                values[0] =
                    (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryToCpu:
                fields[0] = IPV6_MC_ROUTER_ADV_PKT_TO_CPUf;
                break;
            default:
                return (BCM_E_UNAVAIL);
        }
    } else
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    if (soc_feature(unit, soc_feature_proto_pkt_ctrl)){
        reg = PROTOCOL_PKT_CONTROLr;
        switch (type) {
            case bcmSwitchIgmpPktToCpu:
                fields[0] = IGMP_PKT_TO_CPUf;
                break;
            case bcmSwitchIgmpPktDrop:
                fields[0] = IGMP_PKT_DROPf;
                break;
#ifdef LVL7_FIXUP
            /* See #77807 for details */
            case bcmSwitchMldPktToCpu:
                fields[0] = MLD_PKT_TO_CPUf;
                break;
            case bcmSwitchMldPktDrop:
                fields[0] = MLD_PKT_DROPf;
                break;
#endif
            case bcmSwitchV4ResvdMcPktToCpu:
                fields[0] = IPV4_RESVD_MC_PKT_TO_CPUf;
                break;
            case bcmSwitchV4ResvdMcPktDrop:
                fields[0] = IPV4_RESVD_MC_PKT_DROPf;
                break;
            case bcmSwitchV6ResvdMcPktToCpu:
                fields[0] = IPV6_RESVD_MC_PKT_TO_CPUf;
                break;
            case bcmSwitchV6ResvdMcPktDrop:
                fields[0] = IPV6_RESVD_MC_PKT_DROPf;
                break;
            default:
                return (BCM_E_UNAVAIL);
        }
    } else {
        return (BCM_E_UNAVAIL);
    }
    return soc_reg_fields32_modify(unit, reg, port, fcount, fields, values);
}

/*
 * Function:
 *      _bcm_xgs3_igmp_action_get
 * Description:
 *      Get the IGMP/MLD registers
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      *arg  - IGMP / MLD actions
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_igmp_action_get(int unit,
                         bcm_port_t port,
                         bcm_switch_control_t type,
                         int *arg)
{
    uint32      igmp;
    soc_field_t field = INVALIDf;
    soc_reg_t   reg = INVALIDr;
    int         act_value = BCM_SWITCH_RESERVED_VALUE;
    int         hw_value;

#if defined (BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)
    if (soc_feature(unit, soc_feature_igmp_mld_support)) {
        reg = IGMP_MLD_PKT_CONTROLr;
        /* Given control type select register field. */
        switch (type) {
            case bcmSwitchIgmpPktToCpu:
                field = IGMP_REP_LEAVE_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIgmpPktDrop:
                field = IGMP_REP_LEAVE_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchMldPktToCpu:
                field = MLD_REP_DONE_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchMldPktDrop:
                field = MLD_REP_DONE_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchV4ResvdMcPktToCpu:
                field = IPV4_RESVD_MC_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchV4ResvdMcPktFlood:
                field = IPV4_RESVD_MC_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchV4ResvdMcPktDrop:
                field = IPV4_RESVD_MC_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchV6ResvdMcPktToCpu:
                field = IPV6_RESVD_MC_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchV6ResvdMcPktFlood:
                field = IPV6_RESVD_MC_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchV6ResvdMcPktDrop:
                field = IPV6_RESVD_MC_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveDrop:
                field = IGMP_REP_LEAVE_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveFlood:
                field = IGMP_REP_LEAVE_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIgmpReportLeaveToCpu:
                field = IGMP_REP_LEAVE_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIgmpQueryDrop:
                field = IGMP_QUERY_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIgmpQueryFlood:
                field = IGMP_QUERY_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIgmpQueryToCpu:
                field = IGMP_QUERY_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIgmpUnknownDrop:
                field = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIgmpUnknownFlood:
                field = IGMP_UNKNOWN_MSG_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIgmpUnknownToCpu:
                field = IGMP_UNKNOWN_MSG_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchMldReportDoneDrop:
                field = MLD_REP_DONE_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchMldReportDoneFlood:
                field = MLD_REP_DONE_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchMldReportDoneToCpu:
                field = MLD_REP_DONE_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchMldQueryDrop:
                field = MLD_QUERY_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchMldQueryFlood:
                field = MLD_QUERY_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchMldQueryToCpu:
                field = MLD_QUERY_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryDrop:
                field = IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryFlood:
                field = IPV4_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIpmcV4RouterDiscoveryToCpu:
                field = IPV4_MC_ROUTER_ADV_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryDrop:
                field = IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_DROP_VALUE;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryFlood:
                field = IPV6_MC_ROUTER_ADV_PKT_FWD_ACTIONf;
                act_value = BCM_SWITCH_FLOOD_VALUE;
                break;
            case bcmSwitchIpmcV6RouterDiscoveryToCpu:
                field = IPV6_MC_ROUTER_ADV_PKT_TO_CPUf;
                act_value = 1;
                break;
            default:
                return (BCM_E_UNAVAIL);
        }
    } else
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    if (soc_feature(unit, soc_feature_proto_pkt_ctrl)){
        reg = PROTOCOL_PKT_CONTROLr;
        switch (type) {
            case bcmSwitchIgmpPktToCpu:
                field = IGMP_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchIgmpPktDrop:
                field = IGMP_PKT_DROPf;
                act_value = 1;
                break;
#ifdef LVL7_FIXUP
            /* See #77807 for details */
            case bcmSwitchMldPktToCpu: 	 
                field = MLD_PKT_TO_CPUf; 	 
                act_value = 1; 	 
                break; 	 
            case bcmSwitchMldPktDrop: 	 
                field = MLD_PKT_DROPf; 	 
                act_value = 1; 	 
                break;
#endif
            case bcmSwitchV4ResvdMcPktToCpu:
                field = IPV4_RESVD_MC_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchV4ResvdMcPktDrop:
                field = IPV4_RESVD_MC_PKT_DROPf;
                act_value = 1;
                break;
            case bcmSwitchV6ResvdMcPktToCpu:
                field = IPV6_RESVD_MC_PKT_TO_CPUf;
                act_value = 1;
                break;
            case bcmSwitchV6ResvdMcPktDrop:
                field = IPV6_RESVD_MC_PKT_DROPf;
                act_value = 1;
                break;
            default:
                return (BCM_E_UNAVAIL);
        }
    } else {
        return (BCM_E_UNAVAIL);
    }

    if (soc_reg_field_valid(unit, reg, field)) {
#if defined (BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
        defined (BCM_TRX_SUPPORT)
        if (IGMP_MLD_PKT_CONTROLr == reg) {
            BCM_IF_ERROR_RETURN(READ_IGMP_MLD_PKT_CONTROLr(unit, port, &igmp));
        } else
#endif /* BCM_RAPTOR_SUPPORT  || BCM_TRX_SUPPORT || BCM_FIREBOLT2_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(READ_PROTOCOL_PKT_CONTROLr(unit, port, &igmp));
        }
        hw_value = soc_reg_field_get(unit, reg, igmp, field);
        *arg = (act_value == hw_value) ? 1 : 0;
        return (BCM_E_NONE);
    }

    return (BCM_E_UNAVAIL);
}

#if defined(BCM_TRIUMPH2_SUPPORT)

/*
 * Function:
 *      _bcm_tr2_ep_redirect_action_set
 * Description:
 *      Programs action on EP, for EP redirect feature
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - Port number
 *      type - The desired action to set.
 *      arg  - Action value
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_tr2_ep_redirect_action_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg)
{

    uint32      values[2];
    soc_field_t fields[2];
    int         idx;
    soc_reg_t   reg = INVALIDr ;
    int         fcount = 1;
    int         value = (arg) ? 1 : 0;

    
    if (!SOC_IS_TRIUMPH2(unit) && !SOC_IS_APOLLO(unit) &&
        !SOC_IS_VALKYRIE2(unit)) {
        return BCM_E_UNAVAIL;
    }

    for (idx = 0; idx < COUNTOF(values); idx++) {
        values[idx] = value;
        fields[idx] = INVALIDf;
    }

    reg = EGR_CPU_CONTROLr;
    switch (type) {
        case bcmSwitchUnknownVlanToCpu:
            fields[0] = VLAN_TOCPUf;
            break;
        case bcmSwitchL3HeaderErrToCpu:
            fields[0] = L3ERR_TOCPUf;
            break;
        case bcmSwitchIpmcTtlErrToCpu:
            fields[0] = TTL_DROP_TOCPUf;
            break;
        case bcmSwitchHgHdrErrToCpu:
            fields[0] = HIGIG_TOCPUf;
            break;
        case bcmSwitchStgInvalidToCpu:
            fields[0] = STG_TOCPUf;
            break;
        case bcmSwitchVlanTranslateEgressMissToCpu:
            fields[0] = VXLT_TOCPUf;
            break;
        case bcmSwitchTunnelErrToCpu:
            fields[0] = TUNNEL_TOCPUf;
            break;
        case bcmSwitchL3PktErrToCpu:
            fields[0] = L3PKT_ERR_TOCPUf;
            break;
        case bcmSwitchMtuFailureToCpu:
            fields[0] = MTU_TOCPUf;
            break;
        case bcmSwitchSrcKnockoutToCpu:
            fields[0] = PRUNE_TOCPUf;
            break;
        case bcmSwitchWlanTunnelMismatchToCpu:
            fields[0] = WLAN_MOVE_TOCPUf;
            break;
        case bcmSwitchWlanTunnelMismatchDrop:
            fields[0] = WLAN_MOVE_DROPf;
            break;
        case bcmSwitchWlanPortMissToCpu:
            fields[0] = WLAN_SVP_MISS_TOCPUf;
            break;
        default:
            return(BCM_E_UNAVAIL);
    }

    return soc_reg_fields32_modify(unit, reg, port, fcount, fields, values);
}


/*
 * Function:
 *      _bcm_tr2_ep_redirect_action_get
 * Description:
 *      Gets action on EP, for EP redirect feature
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - Port number
 *      type - The desired action to set.
 *      arg  - (OUT) Action value programmed
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_tr2_ep_redirect_action_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg)
{
    int         egr_arg, ing_arg;
    uint64      ing_reg_val, egr_reg_val;
    uint32      ing_reg, egr_reg, ing_field, egr_field;

    
    if (!SOC_IS_TRIUMPH2(unit) && !SOC_IS_APOLLO(unit) &&
        !SOC_IS_VALKYRIE2(unit)) {
        return BCM_E_UNAVAIL;
    }

    ing_reg = INVALIDr;
    ing_field = INVALIDf;
    egr_reg = EGR_CPU_CONTROLr;

    switch (type) {
        case bcmSwitchUnknownVlanToCpu:
            /* Egress vs ingress comparison necessary only for overwrriten */
            /* switch controls */
            if (SOC_IS_TR_VL(unit)) {
                ing_reg = CPU_CONTROL_0r;
            } else {
                ing_reg = CPU_CONTROL_1r;
            }
            ing_field = UVLAN_TOCPUf;
            egr_field = VLAN_TOCPUf;
            break;
        case bcmSwitchL3HeaderErrToCpu:
            /* Egress vs ingress comparison necessary only for overwrriten */
            /* switch controls */
            ing_reg = CPU_CONTROL_1r;
            ing_field = V4L3ERR_TOCPUf;
            egr_field = L3ERR_TOCPUf;
            break;
        case bcmSwitchIpmcTtlErrToCpu:
            /* Egress vs ingress comparison necessary only for overwrriten */
            /* switch controls */
            ing_reg = CPU_CONTROL_1r;
            ing_field = IPMC_TTL_ERR_TOCPUf;
            egr_field = TTL_DROP_TOCPUf;
            break;
        case bcmSwitchHgHdrErrToCpu:
            /* Egress vs ingress comparison necessary only for overwrriten */
            /* switch controls */
            ing_reg = CPU_CONTROL_1r;
            ing_field = HG_HDR_ERROR_TOCPUf;
            egr_field = HIGIG_TOCPUf;
            break;
        case bcmSwitchTunnelErrToCpu:
            /* Egress vs ingress comparison necessary only for overwrriten */
            /* switch controls */
            ing_reg = CPU_CONTROL_1r;
            ing_field = TUNNEL_ERR_TOCPUf;
            egr_field = TUNNEL_TOCPUf;
            break;
        case bcmSwitchStgInvalidToCpu:
            egr_field = STG_TOCPUf;
            break;
        case bcmSwitchVlanTranslateEgressMissToCpu:
            egr_field = VXLT_TOCPUf;
            break;
        case bcmSwitchL3PktErrToCpu:
            egr_field = L3PKT_ERR_TOCPUf;
            break;
        case bcmSwitchMtuFailureToCpu:
            egr_field = MTU_TOCPUf;
            break;
        case bcmSwitchSrcKnockoutToCpu:
            egr_field = PRUNE_TOCPUf;
            break;
        case bcmSwitchWlanTunnelMismatchToCpu:
            egr_field = WLAN_MOVE_TOCPUf;
            break;
        case bcmSwitchWlanTunnelMismatchDrop:
            egr_field = WLAN_MOVE_DROPf;
            break;
        case bcmSwitchWlanPortMissToCpu:
            egr_field = WLAN_SVP_MISS_TOCPUf;
            break;

        default:
            return(BCM_E_UNAVAIL);
    }

    SOC_IF_ERROR_RETURN(soc_reg_read(unit, egr_reg,
                                     soc_reg_addr(unit, egr_reg, port, 0),
                                     &egr_reg_val));
    egr_arg = soc_reg64_field32_get(unit, egr_reg, egr_reg_val,
                                    egr_field);
    if (ing_reg != INVALIDr) {
        SOC_IF_ERROR_RETURN(soc_reg_read(unit, ing_reg,
                                         soc_reg_addr(unit, ing_reg, port, 0),
                                         &ing_reg_val));
        ing_arg = soc_reg64_field32_get(unit, ing_reg, ing_reg_val,
                                        ing_field);
        if (egr_arg != ing_arg) {
            return(BCM_E_CONFIG);
        }
    }

    *arg = egr_arg;
    return(BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_ep_redirect_action_cosq_set
 * Description:
 *      Programs CPU COSQ for EP redirect actions
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - Port number
 *      type - The desired action to set.
 *      arg  - Action value
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_tr2_ep_redirect_action_cosq_set(int unit,
                                     bcm_port_t port,
                                     bcm_switch_control_t type,
                                     int arg)
{
    uint32      values[2];
    soc_field_t fields[2];
    int         idx;
    soc_reg_t   reg = INVALIDr ;
    int         fcount = 1;

    
    if (!SOC_IS_TRIUMPH2(unit) && !SOC_IS_APOLLO(unit) &&
        !SOC_IS_VALKYRIE2(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (arg < 0 || arg > NUM_CPU_COSQ(unit)) {
        return BCM_E_PARAM;
    }

    for (idx = 0; idx < COUNTOF(values); idx++) {
        values[idx] = arg;
        fields[idx] = INVALIDf;
    }

    reg = EGR_CPU_COS_CONTROL_1_64r;
    switch (type) {
        case bcmSwitchUnknownVlanToCpuCosq:
            fields[0] = VLAN_CPU_COSf;
            break;
        case bcmSwitchStgInvalidToCpuCosq:
            fields[0] = STG_CPU_COSf;
            break;
        case bcmSwitchVlanTranslateEgressMissToCpuCosq:
            fields[0] = VXLT_CPU_COSf;
            break;
        case bcmSwitchTunnelErrToCpuCosq:
            fields[0] = TUNNEL_CPU_COSf;
            break;
        case bcmSwitchL3HeaderErrToCpuCosq:
            fields[0] = L3ERR_CPU_COSf;
            break;
        case bcmSwitchL3PktErrToCpuCosq:
            fields[0] = L3PKT_ERR_CPU_COSf;
            break;
        case bcmSwitchIpmcTtlErrToCpuCosq:
            fields[0] = TTL_DROP_CPU_COSf;
            break;
        case bcmSwitchMtuFailureToCpuCosq:
            fields[0] = MTU_CPU_COSf;
            break;
        case bcmSwitchHgHdrErrToCpuCosq:
            fields[0] = HIGIG_CPU_COSf;
            break;
        case bcmSwitchSrcKnockoutToCpuCosq:
            fields[0] = PRUNE_CPU_COSf;
            break;
        case bcmSwitchWlanTunnelMismatchToCpuCosq:
            reg = EGR_CPU_COS_CONTROL_2r;
            fields[0] = WLAN_MOVE_CPU_COSf;
            break;
        case bcmSwitchWlanPortMissToCpuCosq:
            reg = EGR_CPU_COS_CONTROL_2r;
            fields[0] = WLAN_SVP_MISS_CPU_COSf;
            break;
        default:
            return(BCM_E_UNAVAIL);
    }

    return soc_reg_fields32_modify(unit, reg, port, fcount, fields, values);
}

/*
 * Function:
 *      _bcm_tr2_ep_redirect_action_cosq_get
 * Description:
 *      Gets action on EP, for EP redirect feature
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      port - Port number
 *      type - The desired action to set.
 *      arg  - (OUT) Action value programmed
 * Returns:
 *      BCM_E_xxxx
 */
STATIC int
_bcm_tr2_ep_redirect_action_cosq_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg)
{
    uint64      reg_val;
    uint32      reg, field;

    
    if (!SOC_IS_TRIUMPH2(unit) && !SOC_IS_APOLLO(unit) &&
        !SOC_IS_VALKYRIE2(unit)) {
        return BCM_E_UNAVAIL;
    }

    reg = EGR_CPU_COS_CONTROL_1_64r;
    switch (type) {
        case bcmSwitchUnknownVlanToCpuCosq:
            field = VLAN_CPU_COSf;
            break;
        case bcmSwitchStgInvalidToCpuCosq:
            field = STG_CPU_COSf;
            break;
        case bcmSwitchVlanTranslateEgressMissToCpuCosq:
            field = VXLT_CPU_COSf;
            break;
        case bcmSwitchTunnelErrToCpuCosq:
            field = TUNNEL_CPU_COSf;
            break;
        case bcmSwitchL3HeaderErrToCpuCosq:
            field = L3ERR_CPU_COSf;
            break;
        case bcmSwitchL3PktErrToCpuCosq:
            field = L3PKT_ERR_CPU_COSf;
            break;
        case bcmSwitchIpmcTtlErrToCpuCosq:
            field = TTL_DROP_CPU_COSf;
            break;
        case bcmSwitchMtuFailureToCpuCosq:
            field = MTU_CPU_COSf;
            break;
        case bcmSwitchHgHdrErrToCpuCosq:
            field = HIGIG_CPU_COSf;
            break;
        case bcmSwitchSrcKnockoutToCpuCosq:
            field = PRUNE_CPU_COSf;
            break;
        case bcmSwitchWlanTunnelMismatchToCpuCosq:
            reg = EGR_CPU_COS_CONTROL_2r;
            field = WLAN_MOVE_CPU_COSf;
            break;
        case bcmSwitchWlanPortMissToCpuCosq:
            reg = EGR_CPU_COS_CONTROL_2r;
            field = WLAN_SVP_MISS_CPU_COSf;
            break;
        default:
            return(BCM_E_UNAVAIL);
    }

    SOC_IF_ERROR_RETURN(
        soc_reg_read(unit, reg, soc_reg_addr(unit, reg, port, 0), &reg_val));
    *arg = soc_reg64_field32_get(unit, reg, reg_val, field);
    return(BCM_E_NONE);
}

STATIC int
_bcm_tr2_layered_qos_resolution_set(int unit,
                                    bcm_port_t port,
                                    bcm_switch_control_t type,
                                    int arg)
{
    uint64 rval64;
    uint32 rval, value;

    
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        value = arg ? 0 : 1; /* layered resolution  : serial resolution */
        BCM_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                              IGNORE_PPD0_PRESERVE_QOSf, value);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                              IGNORE_PPD2_PRESERVE_QOSf, value);
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                              IGNORE_PPD3_PRESERVE_QOSf, value);
        SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));
        BCM_IF_ERROR_RETURN(READ_EGR_CONFIG_1r(unit, &rval));
        soc_reg_field_set(unit, EGR_CONFIG_1r, &rval,
                          DISABLE_PPD0_PRESERVE_QOSf, value);
        soc_reg_field_set(unit, EGR_CONFIG_1r, &rval,
                          DISABLE_PPD2_PRESERVE_QOSf, value);
        soc_reg_field_set(unit, EGR_CONFIG_1r, &rval,
                          DISABLE_PPD3_PRESERVE_QOSf, value);
        SOC_IF_ERROR_RETURN(WRITE_EGR_CONFIG_1r(unit, rval));

        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_tr2_layered_qos_resolution_get(int unit,
                                    bcm_port_t port,
                                    bcm_switch_control_t type,
                                    int *arg)
{
    uint64 rval64;

    
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        BCM_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
        *arg = soc_reg64_field32_get(unit, ING_CONFIG_64r, rval64,
                                     IGNORE_PPD0_PRESERVE_QOSf) ? 0 : 1;
        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_tr2_ehg_error2cpu_set(int unit, bcm_port_t port, int arg)
{
    if (soc_feature(unit, soc_feature_embedded_higig)) {
        uint32  rval;

        if (!IS_ST_PORT(unit, port)) {
            return (BCM_E_CONFIG);
        }
        BCM_IF_ERROR_RETURN(
            READ_CPU_CONTROL_0r(unit, &rval));
        soc_reg_field_set(unit, CPU_CONTROL_0r, &rval,
                          EHG_NONHG_TOCPUf, arg ? 1 : 0);
        BCM_IF_ERROR_RETURN(
            WRITE_CPU_CONTROL_0r(unit, rval));

        return (BCM_E_NONE);
    }

    return (BCM_E_UNAVAIL);
}

STATIC int
_bcm_tr2_ehg_error2cpu_get(int unit, bcm_port_t port, int *arg)
{
    if (soc_feature(unit, soc_feature_embedded_higig)) {
        uint32  rval;

        if (!IS_ST_PORT(unit, port)) {
            return (BCM_E_CONFIG);
        }
        BCM_IF_ERROR_RETURN(
            READ_CPU_CONTROL_0r(unit, &rval));
        *arg = soc_reg_field_get(unit, CPU_CONTROL_0r, rval, EHG_NONHG_TOCPUf);

        return (BCM_E_NONE);
    }

    return (BCM_E_UNAVAIL);
}

STATIC int
_bcm_tr2_mirror_egress_true_set(int unit, bcm_port_t port,
                                bcm_switch_control_t type, int arg)
{
    uint32      values[2];
    soc_field_t fields[2];

    if (soc_feature(unit, soc_feature_egr_mirror_true)) {
        if (type == bcmSwitchMirrorEgressTrueColorSelect) {
            fields[0] = REDIRECT_DROP_PRECEDENCEf;
            switch (arg) {
            case bcmColorGreen:
                values[0] = 1;
                break;
            case bcmColorYellow:
                values[0] = 2;
                break;
            case bcmColorRed:
                values[0] = 3;
                break;
            default:
                values[0] = 0; /* No change */
                break;
            }
            return soc_reg_fields32_modify(unit, EGR_PORT_64r, port, 1,
                                           fields, values);
        } else if (type == bcmSwitchMirrorEgressTruePriority) {
            fields[0] = REDIRECT_INT_PRI_SELf;
            fields[1] = REDIRECT_INT_PRIf;
            if ((arg < 0) || (arg > 0xf)) {
                values[0] = 0;
                values[1] = 0;
            } else {
                values[0] = 1;
                values[1] = arg & 0xf;
            }
            return soc_reg_fields32_modify(unit, EGR_PORT_64r, port, 2,
                                           fields, values);
        }
    }
    return (BCM_E_UNAVAIL);
}

STATIC int
_bcm_tr2_mirror_egress_true_get(int unit, bcm_port_t port,
                                bcm_switch_control_t type, int *arg)
{
    uint64 rval64;

    if (soc_feature(unit, soc_feature_egr_mirror_true)) {
        BCM_IF_ERROR_RETURN(READ_EGR_PORT_64r(unit, port, &rval64));
        if (type == bcmSwitchMirrorEgressTrueColorSelect) {
            switch (soc_reg64_field32_get(unit, EGR_PORT_64r, rval64,
                              REDIRECT_DROP_PRECEDENCEf)) {
            case 1:
                *arg = bcmColorGreen;
                break;
            case 2:
                *arg = bcmColorYellow;
                break;
            case 3:
                *arg = bcmColorRed;
                break;
            case 0:
            default:
                *arg = -1; /* No change */
                break;
            }
            return BCM_E_NONE;
        } else if (type == bcmSwitchMirrorEgressTruePriority) {
            if (soc_reg64_field32_get(unit, EGR_PORT_64r, rval64,
                              REDIRECT_INT_PRI_SELf)) {
                *arg = soc_reg64_field32_get(unit, EGR_PORT_64r, rval64,
                                             REDIRECT_INT_PRIf);
            } else {
                *arg = -1;
            }
            return BCM_E_NONE;
        }
    }
    return (BCM_E_UNAVAIL);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT)
/*
 * Function:
 *      _bcm_xgs3_switch_ethertype_set
 * Description:
 *      Set the ethertype field for MMRP, and SRP registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control to set the ethertype for
 *      arg         - argument to set as Ethrtype
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_switch_ethertype_set(int unit, bcm_port_t port, 
                               bcm_switch_control_t type,
                               int arg)
{
    uint32      values[2];
    soc_field_t fields[2];
    soc_reg_t   reg = INVALIDr ;
    int         idx, fcount = 1;
    uint16      ethertype;
    
    if (!soc_feature(unit, soc_feature_timesync_support)) {
        return BCM_E_UNAVAIL;
    }
    ethertype = (uint16)arg;

    for (idx = 0; idx < COUNTOF(values); idx++) {
        values[idx] = ethertype;
        fields[idx] = ETHERTYPEf;
    }

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchMMRPEthertype:
            reg = MMRP_CONTROL_2r;
            break;
        case bcmSwitchTimeSyncEthertype: 
            reg = TS_CONTROLr;
            break;
        case bcmSwitchSRPEthertype:
            reg = SRP_CONTROL_2r;
            break;
        default:
            return BCM_E_PARAM;
    }

    return soc_reg_fields32_modify(unit, reg, port, fcount, fields, values);
}

/*
 * Function:
 *      _bcm_xgs3_switch_ethertype_get
 * Description:
 *      Get the ethertype field for MMRP, and SRP registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control to get the ethertype for
 *      arg         - Ethrtype to get
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_switch_ethertype_get(int unit, bcm_port_t port, 
                               bcm_switch_control_t type,
                               int *arg)
{
    soc_reg_t   reg = INVALIDr ;
    uint16      ethertype;
    uint32      regval;

    if (!soc_feature(unit, soc_feature_timesync_support)) {
        return BCM_E_UNAVAIL;
    }

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchMMRPEthertype:
            reg = MMRP_CONTROL_2r;
            break;
        case bcmSwitchTimeSyncEthertype: 
            reg = TS_CONTROLr;
            break;
        case bcmSwitchSRPEthertype:
            reg = SRP_CONTROL_2r;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!soc_reg_field_valid(unit, reg, ETHERTYPEf)) {
        return BCM_E_UNAVAIL;
    }
    SOC_IF_ERROR_RETURN(
        soc_reg32_read(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), &regval));
    ethertype = soc_reg_field_get(unit, reg, regval, ETHERTYPEf);

    *arg = (int)ethertype;

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_xgs3_switch_mac_lo_set
 * Description:
 *      Set the low 24 bits of MAC address field for MMRP, and SRP registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to set the mac for
 *      mac_lo      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_switch_mac_lo_set(int unit, bcm_port_t port, 
                            bcm_switch_control_t type,
                            uint32 mac_lo)
{
    uint32      regval, fieldval;
    soc_reg_t   reg = INVALIDr ;

    if (!soc_feature(unit, soc_feature_timesync_support)) {
        return BCM_E_UNAVAIL;
    }

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchMMRPDestMacNonOui:
            reg = MMRP_CONTROL_1r;
            break;
        case bcmSwitchTimeSyncDestMacNonOui: 
            reg = TS_CONTROL_1r;
            break;
        case bcmSwitchSRPDestMacNonOui:
            reg = SRP_CONTROL_1r;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!soc_reg_field_valid(unit, reg, MAC_DA_LOWERf)) {
        return BCM_E_UNAVAIL;
    }
    SOC_IF_ERROR_RETURN(
        soc_reg32_read(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), &regval));
    fieldval = soc_reg_field_get(unit, reg, regval, MAC_DA_LOWERf);
    fieldval |= ((mac_lo << 8) >> 8);
    soc_reg_field_set(unit, reg, &regval, MAC_DA_LOWERf, fieldval);
    SOC_IF_ERROR_RETURN(
        soc_reg32_write(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), regval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_switch_mac_hi_set
 * Description:
 *      Set the upper 24 bits of MAC address field for MMRP, and SRP registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to set upper MAC for
 *      mac_hi      - MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_switch_mac_hi_set(int unit, bcm_port_t port, 
                            bcm_switch_control_t type,
                            uint32 mac_hi)
{
    soc_reg_t   reg1, reg2;
    uint32      fieldval, regval1, regval2;

    reg1 = reg2 = INVALIDr;

    if (!soc_feature(unit, soc_feature_timesync_support)) {
        return BCM_E_UNAVAIL;
    }

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchMMRPDestMacOui:
            reg1 = MMRP_CONTROL_1r;
            reg2 = MMRP_CONTROL_2r;
            break;
        case bcmSwitchTimeSyncDestMacOui: 
            reg1 = TS_CONTROL_1r;
            reg2 = TS_CONTROL_2r;
            break;
        case bcmSwitchSRPDestMacOui:
            reg1 = SRP_CONTROL_1r;
            reg2 = SRP_CONTROL_2r;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!soc_reg_field_valid(unit, reg1, MAC_DA_LOWERf)) {
        return BCM_E_UNAVAIL;
    }
    if (!soc_reg_field_valid(unit, reg2, MAC_DA_UPPERf)) {
        return BCM_E_UNAVAIL;
    }
    
    SOC_IF_ERROR_RETURN(
        soc_reg32_read(unit, soc_reg_addr(unit, reg1, REG_PORT_ANY, 0), 
                       &regval1));
    fieldval = soc_reg_field_get(unit, reg1, regval1, MAC_DA_LOWERf);
    fieldval |= (mac_hi << 24);
    soc_reg_field_set(unit, reg1, &regval1, MAC_DA_LOWERf, fieldval);
    SOC_IF_ERROR_RETURN(
        soc_reg32_write(unit, soc_reg_addr(unit, reg1, REG_PORT_ANY, 0), 
                        regval1));
    SOC_IF_ERROR_RETURN(
        soc_reg32_read(unit, soc_reg_addr(unit, reg2, REG_PORT_ANY, 0), 
                       &regval2));
    fieldval = (mac_hi >> 8) & 0xffff;
    soc_reg_field_set(unit, reg2, &regval2, MAC_DA_UPPERf, fieldval);
    SOC_IF_ERROR_RETURN(
        soc_reg32_write(unit, soc_reg_addr(unit, reg2, REG_PORT_ANY, 0), 
                        regval2));
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_xgs3_switch_mac_lo_get
 * Description:
 *      Get the lower 24 bits of MAC address field for MMRP, and SRP registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - arg to get the lower MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_switch_mac_lo_get(int unit, bcm_port_t port, 
                            bcm_switch_control_t type,
                            int *arg)
{
    soc_reg_t   reg = INVALIDr ;
    uint32      regval, mac;

    if (!soc_feature(unit, soc_feature_timesync_support)) {
        return BCM_E_UNAVAIL;
    }

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchMMRPDestMacNonOui:
            reg = MMRP_CONTROL_1r;
            break;
        case bcmSwitchTimeSyncDestMacNonOui: 
            reg = TS_CONTROL_1r;
            break;
        case bcmSwitchSRPDestMacNonOui:
            reg = SRP_CONTROL_1r;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!soc_reg_field_valid(unit, reg, MAC_DA_LOWERf)) {
        return BCM_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(
        soc_reg32_read(unit, soc_reg_addr(unit, reg, REG_PORT_ANY, 0), &regval));
    mac = soc_reg_field_get(unit, reg, regval, MAC_DA_LOWERf);

    *arg = (mac << 8) >> 8;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_switch_mac_hi_get
 * Description:
 *      Get the upper 24 bits of MAC address field for MMRP, and SRP registers
 * Parameters:
 *      unit        - StrataSwitch PCI device unit number (driver internal).
 *      port        - port number.
 *      type        - The required switch control type to get MAC for
 *      arg         - arg to get the upper MAC address
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_switch_mac_hi_get(int unit, bcm_port_t port, 
                            bcm_switch_control_t type,
                         int *arg)
{
    soc_reg_t   reg1, reg2;
    uint32      mac, regval1, regval2;

    reg1 = reg2 = INVALIDr;

    if (!soc_feature(unit, soc_feature_timesync_support)) {
        return BCM_E_UNAVAIL;
    }

    /* Given control type select register. */
    switch (type) {
        case bcmSwitchMMRPDestMacOui:
            reg1 = MMRP_CONTROL_1r;
            reg2 = MMRP_CONTROL_2r;
            break;
        case bcmSwitchTimeSyncDestMacOui: 
            reg1 = TS_CONTROL_1r;
            reg2 = TS_CONTROL_2r;
            break;
        case bcmSwitchSRPDestMacOui:
            reg1 = SRP_CONTROL_1r;
            reg2 = SRP_CONTROL_2r;
            break;
        default:
            return BCM_E_PARAM;
    }

    if (!soc_reg_field_valid(unit, reg1, MAC_DA_LOWERf)) {
        return BCM_E_UNAVAIL;
    }
    if (!soc_reg_field_valid(unit, reg2, MAC_DA_UPPERf)) {
        return BCM_E_UNAVAIL;
    }
    SOC_IF_ERROR_RETURN(
        soc_reg32_read(unit, soc_reg_addr(unit, reg1, REG_PORT_ANY, 0), 
                       &regval1));
    mac = (soc_reg_field_get(unit, reg1, regval1, MAC_DA_LOWERf) >> 24);

    SOC_IF_ERROR_RETURN(
        soc_reg32_read(unit, soc_reg_addr(unit, reg2, REG_PORT_ANY, 0), 
                       &regval2));
    mac |= (soc_reg_field_get(unit, reg2, regval2, MAC_DA_UPPERf) << 8);

    *arg = (int)mac;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_xgs3_eav_action_set
 * Description:
 *      Set the Time-Sync, MMRP, and SRP registers
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - EAV actions
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_eav_action_set(int unit,
                         bcm_port_t port,
                         bcm_switch_control_t type,
                         int arg)
{
    uint32      values[3];
    soc_field_t fields[3];
    int         idx;
    soc_reg_t   reg = INVALIDr ;
    int         fcount = 1;
    int         value = (arg) ? 1 : 0;

    if (!soc_feature(unit, soc_feature_timesync_support)) {
        return BCM_E_UNAVAIL;
    }

    for (idx = 0; idx < COUNTOF(values); idx++) {
        values[idx] = value;
        fields[idx] = INVALIDf;
    }

    reg = PROTOCOL_PKT_CONTROLr;
    /* Given control type select register field. */
    switch (type) {
        case bcmSwitchTimeSyncPktToCpu:
            if (soc_reg_field_valid(unit, reg, TS_PKT_TO_CPUf)) {
            fields[0] = TS_PKT_TO_CPUf;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchTimeSyncPktDrop:
            if (soc_reg_field_valid(unit, reg, TS_FWD_ACTIONf)) {
            fields[0] = TS_FWD_ACTIONf;
            values[0] = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchTimeSyncPktFlood:
            if (soc_reg_field_valid(unit, reg, TS_FWD_ACTIONf)) {
            fields[0] = TS_FWD_ACTIONf;
            values[0] = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
            } else {
                return BCM_E_UNAVAIL;
            }
            break;
        case bcmSwitchMmrpPktToCpu:
            fields[0] = MMRP_PKT_TO_CPUf;
            break;
        case bcmSwitchMmrpPktDrop:
            fields[0] = MMRP_FWD_ACTIONf;
            values[0] = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
            break;
        case bcmSwitchMmrpPktFlood:
            fields[0] = MMRP_FWD_ACTIONf;
            values[0] = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
            break;
        case bcmSwitchSrpPktToCpu:
            fields[0] = SRP_PKT_TO_CPUf;
            break;
        case bcmSwitchSrpPktDrop:
            fields[0] = SRP_FWD_ACTIONf;
            values[0] = (arg) ? BCM_SWITCH_DROP_VALUE : BCM_SWITCH_FORWARD_VALUE;
            break;
        case bcmSwitchSrpPktFlood:
            fields[0] = SRP_FWD_ACTIONf;
            values[0] = (arg) ? BCM_SWITCH_FLOOD_VALUE : BCM_SWITCH_FORWARD_VALUE;
            break;
        default:
                return (BCM_E_UNAVAIL);
    }
    return soc_reg_fields32_modify(unit, reg, port, fcount, fields, values);
}

/*
 * Function:
 *      _bcm_xgs3_eav_action_get
 * Description:
 *      Get the Time-Sync, MMRP, and SRP registers
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      *arg  - EAV actions
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_eav_action_get(int unit,
                         bcm_port_t port,
                         bcm_switch_control_t type,
                         int *arg)
{
    uint32      eav;
    soc_field_t field = INVALIDf;
    soc_reg_t   reg = INVALIDr;
    int         act_value = BCM_SWITCH_RESERVED_VALUE;
    int         hw_value = -1;

    reg = PROTOCOL_PKT_CONTROLr;
    /* Given control type select register field. */
    switch (type) {
        case bcmSwitchTimeSyncPktToCpu:
            field = TS_PKT_TO_CPUf;
            act_value = 1;
            break;
        case bcmSwitchTimeSyncPktDrop:
            field = TS_FWD_ACTIONf;
            act_value = BCM_SWITCH_DROP_VALUE;
            break;
        case bcmSwitchTimeSyncPktFlood:
            field = TS_FWD_ACTIONf;
            act_value = BCM_SWITCH_FLOOD_VALUE;
            break;
        case bcmSwitchMmrpPktToCpu:
            field = MMRP_PKT_TO_CPUf;
            act_value = 1;
            break;
        case bcmSwitchMmrpPktDrop:
            field = MMRP_FWD_ACTIONf;
            act_value = BCM_SWITCH_DROP_VALUE;
            break;
        case bcmSwitchMmrpPktFlood:
            field = MMRP_FWD_ACTIONf;
            act_value = BCM_SWITCH_FLOOD_VALUE;
            break;
        case bcmSwitchSrpPktToCpu:
            field = SRP_PKT_TO_CPUf;
            act_value = 1;
            break;
        case bcmSwitchSrpPktDrop:
            field = SRP_FWD_ACTIONf;
            act_value = BCM_SWITCH_DROP_VALUE;
            break;
        case bcmSwitchSrpPktFlood:
            field = SRP_FWD_ACTIONf;
            act_value = BCM_SWITCH_FLOOD_VALUE;
            break;
        default:
                return (BCM_E_UNAVAIL);
    }

    if (soc_reg_field_valid(unit, reg, field)) {
        BCM_IF_ERROR_RETURN(READ_PROTOCOL_PKT_CONTROLr(unit, port, &eav));
        hw_value = soc_reg_field_get(unit, reg, eav, field);
    } else {
        return BCM_E_UNAVAIL;
    }
    
    *arg = (act_value == hw_value) ? 1 : 0;
    return (BCM_E_NONE);
}
#endif /* HAWKEYE, TRIUMPH2, APOLLO */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
/*
 * Function:
 *      _bcm_xgs3_selectcontrol_set
 * Description:
 *      Set the enhanced (aka rtag 7) hash select control.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_selectcontrol_set(int unit, int arg)
{
    uint32      hash_control, val=0;

    BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &hash_control));

    val = (arg & BCM_HASH_FIELD0_DISABLE_IP4 ) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_IPV4_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_IP4) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_IPV4_Bf, val);

    val = (arg & BCM_HASH_FIELD0_DISABLE_IP6) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_IPV6_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_IP6) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_IPV6_Bf, val);

    val = (arg & BCM_HASH_FIELD0_DISABLE_MPLS) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_MPLS_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_MPLS) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_MPLS_Bf, val);

    val = (arg & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_INNER_IPV4_OVER_IPV4_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_INNER_IPV4_OVER_IPV4_Bf, val);

    val = (arg & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_INNER_IPV6_OVER_IPV4_Af, val);

    val = (arg & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP) ? 1 : 0;
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      DISABLE_HASH_INNER_IPV6_OVER_IPV4_Bf, val);

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        val = (arg & BCM_HASH_FIELD0_DISABLE_MIM) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_MIM_Af, val);

        val = (arg & BCM_HASH_FIELD1_DISABLE_MIM) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_MIM_Bf, val);
    }
#endif

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        val = (arg & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP6) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV4_OVER_IPV6_Af, val);

        val = (arg & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP6) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV4_OVER_IPV6_Bf, val);

        val = (arg & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP6) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV6_OVER_IPV6_Af, val);

        val = (arg & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP6) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV6_OVER_IPV6_Bf, val);

        val = (arg & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV4_OVER_GRE_IPV4_Af, val);
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV4_OVER_GRE_IPV6_Af, val);

        val = (arg & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV4_OVER_GRE_IPV4_Bf, val);
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV4_OVER_GRE_IPV6_Bf, val);

        val = (arg & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV6_OVER_GRE_IPV4_Af, val);
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV6_OVER_GRE_IPV6_Af, val);

        val = (arg & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV6_OVER_GRE_IPV4_Bf, val);
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV6_OVER_GRE_IPV6_Bf, val);
    } else
#endif
    {
        val = (arg & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV4_OVER_GRE_Af, val);

        val = (arg & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV4_OVER_GRE_Bf, val);

        val = (arg & BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV6_OVER_GRE_Af, val);

        val = (arg & BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE) ? 1 : 0;
        soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                          DISABLE_HASH_INNER_IPV6_OVER_GRE_Bf, val);
    }

    BCM_IF_ERROR_RETURN(WRITE_RTAG7_HASH_CONTROLr(unit, hash_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_selectcontrol_get
 * Description:
 *      Get the current enhanced (aka rtag 7) hash control settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_selectcontrol_get(int unit, int *arg)
{
    uint32      hash_control, val=0;

    *arg = 0;
    BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &hash_control));

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_IPV4_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_IP4;

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_IPV4_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_IP4;

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_IPV6_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_IP6;

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_IPV6_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_IP6;

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_MPLS_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_MPLS;

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_MPLS_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_MPLS;

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_INNER_IPV4_OVER_IPV4_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP;

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_INNER_IPV4_OVER_IPV4_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP;

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_INNER_IPV6_OVER_IPV4_Af);
    if (val) *arg |= BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP;

    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            DISABLE_HASH_INNER_IPV6_OVER_IPV4_Bf);
    if (val) *arg |= BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP;

#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_MIM_Af);
        if (val) *arg |= BCM_HASH_FIELD0_DISABLE_MIM;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_MIM_Bf);
        if (val) *arg |= BCM_HASH_FIELD1_DISABLE_MIM;
    }
#endif

#ifdef BCM_TRX_SUPPORT
    if (SOC_IS_TRX(unit)) {
        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV4_OVER_IPV6_Af);
        if (val) *arg |= BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_IP6;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV4_OVER_IPV6_Bf);
        if (val) *arg |= BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_IP6;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV6_OVER_IPV6_Af);
        if (val) *arg |= BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_IP6;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV6_OVER_IPV6_Af);
        if (val) *arg |= BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_IP6;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV4_OVER_GRE_IPV4_Af);
        if (val) *arg |= BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV4_OVER_GRE_IPV4_Bf);
        if (val) *arg |= BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV6_OVER_GRE_IPV4_Af);
        if (val) *arg |= BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV6_OVER_GRE_IPV4_Bf);
        if (val) *arg |= BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE;
    } else
#endif
    {
        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV4_OVER_GRE_Af);
        if (val) *arg |= BCM_HASH_FIELD0_DISABLE_TUNNEL_IP4_GRE;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV4_OVER_GRE_Bf);
        if (val) *arg |= BCM_HASH_FIELD1_DISABLE_TUNNEL_IP4_GRE;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV6_OVER_GRE_Af);
        if (val) *arg |= BCM_HASH_FIELD0_DISABLE_TUNNEL_IP6_GRE;

        val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                                DISABLE_HASH_INNER_IPV6_OVER_GRE_Bf);
        if (val) *arg |= BCM_HASH_FIELD1_DISABLE_TUNNEL_IP6_GRE;
    }

    return BCM_E_NONE;
}

enum ENHANCE_XGS3_HASH {
     /* WARNING: values given match hardware register; do not modify */
     ENHANCE_XGS3_HASH_CRC16XOR8 = 7,
     ENHANCE_XGS3_HASH_CRC16XOR4 = 6,
     ENHANCE_XGS3_HASH_CRC16XOR2 = 5,
     ENHANCE_XGS3_HASH_CRC16XOR1 = 4,
     ENHANCE_XGS3_HASH_CRC16 = 3
};

/*
 * Function:
 *      _bcm_xgs3_fieldconfig_set
 * Description:
 *      Set the enhanced (aka rtag 7) field config settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_fieldconfig_set(int unit,
                          bcm_switch_control_t type, int arg)
{
    uint32      hash_control, val=0;

    switch (arg) {
        case BCM_HASH_FIELD_CONFIG_CRC16XOR8:
            val = ENHANCE_XGS3_HASH_CRC16XOR8;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC16XOR4:
            val = ENHANCE_XGS3_HASH_CRC16XOR4;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC16XOR2:
            val = ENHANCE_XGS3_HASH_CRC16XOR2;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC16XOR1:
            val = ENHANCE_XGS3_HASH_CRC16XOR1;
            break;
        case BCM_HASH_FIELD_CONFIG_CRC16:
            val = ENHANCE_XGS3_HASH_CRC16;
            break;
        default:
            return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &hash_control));
    soc_reg_field_set(unit, RTAG7_HASH_CONTROLr, &hash_control,
                      (type == bcmSwitchHashField0Config) ?
                      HASH_MODE_Af : HASH_MODE_Bf, val);
    BCM_IF_ERROR_RETURN(WRITE_RTAG7_HASH_CONTROLr(unit, hash_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_fieldconfig_get
 * Description:
 *      Get the current enhanced (aka rtag 7) field config settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_fieldconfig_get(int unit,
                          bcm_switch_control_t type, int *arg)
{
    uint32      hash_control, val=0;

    BCM_IF_ERROR_RETURN(READ_RTAG7_HASH_CONTROLr(unit, &hash_control));
    val = soc_reg_field_get(unit, RTAG7_HASH_CONTROLr, hash_control,
                            (type == bcmSwitchHashField0Config) ?
                            HASH_MODE_Af : HASH_MODE_Bf);
    switch (val) {
        case ENHANCE_XGS3_HASH_CRC16XOR8:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16XOR8;
            break;
        case ENHANCE_XGS3_HASH_CRC16XOR4:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16XOR4;
            break;
        case ENHANCE_XGS3_HASH_CRC16XOR2:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16XOR2;
            break;
        case ENHANCE_XGS3_HASH_CRC16XOR1:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16XOR1;
            break;
        case ENHANCE_XGS3_HASH_CRC16:
            *arg = BCM_HASH_FIELD_CONFIG_CRC16;
            break;
    }

    return BCM_E_NONE;
}

typedef struct {
   uint32     flag;
   uint32     hw_map;
} _hash_bmap_t;

typedef struct {
    bcm_switch_control_t        type;
    _hash_bmap_t                *map;
    int                         size;
    soc_reg_t                   reg;
    soc_field_t                 field;
} _hash_fieldselect_t;

_hash_bmap_t hash_select_ip4[] =
{
    { BCM_HASH_FIELD_DSTMOD,    0x0001 },
    { BCM_HASH_FIELD_DSTPORT,   0x0002 },
    { BCM_HASH_FIELD_SRCMOD,    0x0004 },
    { BCM_HASH_FIELD_SRCPORT,   0x0008 },
    { BCM_HASH_FIELD_PROTOCOL,  0x0010 },
    { BCM_HASH_FIELD_DSTL4,     0x0020 },
    { BCM_HASH_FIELD_SRCL4,     0x0040 },
    { BCM_HASH_FIELD_VLAN,      0x0080 },
    { BCM_HASH_FIELD_IP4DST_LO, 0x0100 },
    { BCM_HASH_FIELD_IP4DST_HI, 0x0200 },
    { BCM_HASH_FIELD_IP4SRC_LO, 0x0400 },
    { BCM_HASH_FIELD_IP4SRC_HI, 0x0800 }
};

_hash_bmap_t hash_select_ip6[] =
{
    { BCM_HASH_FIELD_DSTMOD,    0x0001 },
    { BCM_HASH_FIELD_DSTPORT,   0x0002 },
    { BCM_HASH_FIELD_SRCMOD,    0x0004 },
    { BCM_HASH_FIELD_SRCPORT,   0x0008 },
    { BCM_HASH_FIELD_NXT_HDR,   0x0010 },
    { BCM_HASH_FIELD_DSTL4,     0x0020 },
    { BCM_HASH_FIELD_SRCL4,     0x0040 },
    { BCM_HASH_FIELD_VLAN,      0x0080 },
    { BCM_HASH_FIELD_IP6DST_LO, 0x0100 },
    { BCM_HASH_FIELD_IP6DST_HI, 0x0200 },
    { BCM_HASH_FIELD_IP6SRC_LO, 0x0400 },
    { BCM_HASH_FIELD_IP6SRC_HI, 0x0800 }
};

_hash_bmap_t hash_select_l2[] =
{
    { BCM_HASH_FIELD_DSTMOD,    0x0001 },
    { BCM_HASH_FIELD_DSTPORT,   0x0002 },
    { BCM_HASH_FIELD_SRCMOD,    0x0004 },
    { BCM_HASH_FIELD_SRCPORT,   0x0008 },
    { BCM_HASH_FIELD_VLAN,      0x0010 },
    { BCM_HASH_FIELD_ETHER_TYPE,0x0020 },
    { BCM_HASH_FIELD_MACDA_LO,  0x0040 },
    { BCM_HASH_FIELD_MACDA_MI,  0x0080 },
    { BCM_HASH_FIELD_MACDA_HI,  0x0100 },
    { BCM_HASH_FIELD_MACSA_LO,  0x0200 },
    { BCM_HASH_FIELD_MACSA_MI,  0x0400 },
    { BCM_HASH_FIELD_MACSA_HI,  0x0800 }
};

_hash_bmap_t hash_select_mpls[] =
{
    { BCM_HASH_FIELD_DSTMOD,     0x0001 },
    { BCM_HASH_FIELD_DSTPORT,    0x0002 },
    { BCM_HASH_FIELD_SRCMOD,     0x0004 },
    { BCM_HASH_FIELD_SRCPORT,    0x0008 },
    { BCM_HASH_FIELD_VLAN,       0x0010 },
    { BCM_HASH_FIELD_IP4DST_LO,  0x0020 },
    { BCM_HASH_FIELD_IP4DST_HI,  0x0040 },
    { BCM_HASH_FIELD_IP4SRC_LO,  0x0080 },
    { BCM_HASH_FIELD_IP4SRC_HI,  0x0100 },
    { BCM_HASH_FIELD_IP6DST_LO,  0x0020 },
    { BCM_HASH_FIELD_IP6DST_HI,  0x0040 },
    { BCM_HASH_FIELD_IP6SRC_LO,  0x0080 },
    { BCM_HASH_FIELD_IP6SRC_HI,  0x0100 },
    { BCM_HASH_FIELD_RSVD_LABELS,0x0200 },
    { BCM_HASH_FIELD_2ND_LABEL,  0x0400 },
    { BCM_HASH_FIELD_TOP_LABEL,  0x0800 }
};

_hash_bmap_t hash_select_unknown[] =
{
    { BCM_HASH_FIELD_DSTMOD,      0x0001 },
    { BCM_HASH_FIELD_DSTPORT,     0x0002 },
    { BCM_HASH_FIELD_SRCMOD,      0x0004 },
    { BCM_HASH_FIELD_SRCPORT,     0x0008 },
    { BCM_HASH_FIELD_LOAD_BALANCE,0x0010 }
};

_hash_fieldselect_t hash_select_info[] =
{
    { bcmSwitchHashIP4Field0,
        hash_select_ip4, COUNTOF(hash_select_ip4),
        RTAG7_HASH_FIELD_BMAP_1r, IPV4_FIELD_BITMAP_Af},
    { bcmSwitchHashIP4Field1,
        hash_select_ip4, COUNTOF(hash_select_ip4),
        RTAG7_HASH_FIELD_BMAP_1r, IPV4_FIELD_BITMAP_Bf},
    { bcmSwitchHashIP6Field0,
        hash_select_ip6, COUNTOF(hash_select_ip6),
        RTAG7_HASH_FIELD_BMAP_2r, IPV6_FIELD_BITMAP_Af},
    { bcmSwitchHashIP6Field1,
        hash_select_ip6, COUNTOF(hash_select_ip6),
        RTAG7_HASH_FIELD_BMAP_2r, IPV6_FIELD_BITMAP_Bf},
    { bcmSwitchHashL2Field0,
        hash_select_l2, COUNTOF(hash_select_l2),
        RTAG7_HASH_FIELD_BMAP_3r, L2_FIELD_BITMAP_Af},
    { bcmSwitchHashL2Field1,
        hash_select_l2, COUNTOF(hash_select_l2),
        RTAG7_HASH_FIELD_BMAP_3r, L2_FIELD_BITMAP_Bf},
    { bcmSwitchHashMPLSField0,
        hash_select_mpls, COUNTOF(hash_select_mpls),
        RTAG7_HASH_FIELD_BMAP_4r, MPLS_FIELD_BITMAP_Af},
    { bcmSwitchHashMPLSField1,
        hash_select_mpls, COUNTOF(hash_select_mpls),
        RTAG7_HASH_FIELD_BMAP_4r, MPLS_FIELD_BITMAP_Bf},
    { bcmSwitchHashHG2UnknownField0,
        hash_select_unknown, COUNTOF(hash_select_unknown),
        RTAG7_HASH_FIELD_BMAP_5r, UNKNOWN_PPD_FIELD_BITMAP_Af},
    { bcmSwitchHashHG2UnknownField1,
        hash_select_unknown, COUNTOF(hash_select_unknown),
        RTAG7_HASH_FIELD_BMAP_5r, UNKNOWN_PPD_FIELD_BITMAP_Bf}

};

#if defined(BCM_TRIUMPH2_SUPPORT)
_hash_bmap_t hash_select_mpls_l2[] =
{
    { BCM_HASH_FIELD_DSTMOD,    0x0001 },
    { BCM_HASH_FIELD_DSTPORT,   0x0002 },
    { BCM_HASH_FIELD_SRCMOD,    0x0004 },
    { BCM_HASH_FIELD_SRCPORT,   0x0008 },
    { BCM_HASH_FIELD_VLAN,      0x0010 },
    { BCM_HASH_FIELD_ETHER_TYPE,0x0020 },
    { BCM_HASH_FIELD_MACDA_LO,  0x0040 },
    { BCM_HASH_FIELD_MACDA_MI,  0x0080 },
    { BCM_HASH_FIELD_MACDA_HI,  0x0100 },
    { BCM_HASH_FIELD_MACSA_LO,  0x0200 },
    { BCM_HASH_FIELD_MACSA_MI,  0x0400 },
    { BCM_HASH_FIELD_MACSA_HI,  0x0800 }
};

_hash_bmap_t hash_select_mpls_l3[] =
{
    { BCM_HASH_FIELD_DSTMOD,    0x0001 },
    { BCM_HASH_FIELD_DSTPORT,   0x0002 },
    { BCM_HASH_FIELD_SRCMOD,    0x0004 },
    { BCM_HASH_FIELD_SRCPORT,   0x0008 },
    { BCM_HASH_FIELD_PROTOCOL,  0x0010 },
    { BCM_HASH_FIELD_DSTL4,     0x0020 },
    { BCM_HASH_FIELD_SRCL4,     0x0040 },
    { BCM_HASH_FIELD_VLAN,      0x0080 },
    { BCM_HASH_FIELD_IP4DST_LO, 0x0100 },
    { BCM_HASH_FIELD_IP4DST_HI, 0x0200 },
    { BCM_HASH_FIELD_IP4SRC_LO, 0x0400 },
    { BCM_HASH_FIELD_IP4SRC_HI, 0x0800 }
};

_hash_bmap_t hash_select_mpls_tunnel[] =
{
    { BCM_HASH_FIELD_DSTMOD,     0x0001 },
    { BCM_HASH_FIELD_DSTPORT,    0x0002 },
    { BCM_HASH_FIELD_SRCMOD,     0x0004 },
    { BCM_HASH_FIELD_SRCPORT,    0x0008 },
    { BCM_HASH_FIELD_VLAN,       0x0010 },
    { BCM_HASH_FIELD_IP4DST_LO,  0x0020 },
    { BCM_HASH_FIELD_IP4DST_HI,  0x0040 },
    { BCM_HASH_FIELD_IP4SRC_LO,  0x0080 },
    { BCM_HASH_FIELD_IP4SRC_HI,  0x0100 },
    { BCM_HASH_FIELD_IP6DST_LO,  0x0020 },
    { BCM_HASH_FIELD_IP6DST_HI,  0x0040 },
    { BCM_HASH_FIELD_IP6SRC_LO,  0x0080 },
    { BCM_HASH_FIELD_IP6SRC_HI,  0x0100 },
    { BCM_HASH_FIELD_RSVD_LABELS,0x0200 },
    { BCM_HASH_FIELD_2ND_LABEL,  0x0400 },
    { BCM_HASH_FIELD_TOP_LABEL,  0x0800 }
};

_hash_bmap_t hash_select_mim[] =
{
    { BCM_HASH_FIELD_DSTMOD,    0x0001 },
    { BCM_HASH_FIELD_DSTPORT,   0x0002 },
    { BCM_HASH_FIELD_SRCMOD,    0x0004 },
    { BCM_HASH_FIELD_SRCPORT,   0x0008 },
    { BCM_HASH_FIELD_VLAN,      0x0010 },
    { BCM_HASH_FIELD_ETHER_TYPE,0x0020 },
    { BCM_HASH_FIELD_MACDA_LO,  0x0040 },
    { BCM_HASH_FIELD_MACDA_MI,  0x0080 },
    { BCM_HASH_FIELD_MACDA_HI,  0x0100 },
    { BCM_HASH_FIELD_MACSA_LO,  0x0200 },
    { BCM_HASH_FIELD_MACSA_MI,  0x0400 },
    { BCM_HASH_FIELD_MACSA_HI,  0x0800 }
};

_hash_bmap_t hash_select_mim_tunnel[] =
{
    { BCM_HASH_FIELD_DSTMOD,       0x0001 },
    { BCM_HASH_FIELD_DSTPORT,      0x0002 },
    { BCM_HASH_FIELD_SRCMOD,       0x0004 },
    { BCM_HASH_FIELD_SRCPORT,      0x0008 },
    { BCM_HASH_FIELD_LOOKUP_ID_LO, 0x0010 },
    { BCM_HASH_FIELD_LOOKUP_ID_HI, 0x0020 },
    { BCM_HASH_FIELD_MACDA_LO,     0x0040 },
    { BCM_HASH_FIELD_MACDA_MI,     0x0080 },
    { BCM_HASH_FIELD_MACDA_HI,     0x0100 },
    { BCM_HASH_FIELD_MACSA_LO,     0x0200 },
    { BCM_HASH_FIELD_MACSA_MI,     0x0400 },
    { BCM_HASH_FIELD_MACSA_HI,     0x0800 }
};

_hash_fieldselect_t hash_select_info_tr2[] =
{
    { bcmSwitchHashIP4Field0,
        hash_select_ip4, COUNTOF(hash_select_ip4),
        RTAG7_HASH_FIELD_BMAP_1r, IPV4_FIELD_BITMAP_Af},
    { bcmSwitchHashIP4Field1,
        hash_select_ip4, COUNTOF(hash_select_ip4),
        RTAG7_HASH_FIELD_BMAP_1r, IPV4_FIELD_BITMAP_Bf},
    { bcmSwitchHashIP6Field0,
        hash_select_ip6, COUNTOF(hash_select_ip6),
        RTAG7_HASH_FIELD_BMAP_2r, IPV6_FIELD_BITMAP_Af},
    { bcmSwitchHashIP6Field1,
        hash_select_ip6, COUNTOF(hash_select_ip6),
        RTAG7_HASH_FIELD_BMAP_2r, IPV6_FIELD_BITMAP_Bf},
    { bcmSwitchHashL2Field0,
        hash_select_l2, COUNTOF(hash_select_l2),
        RTAG7_HASH_FIELD_BMAP_3r, L2_FIELD_BITMAP_Af},
    { bcmSwitchHashL2Field1,
        hash_select_l2, COUNTOF(hash_select_l2),
        RTAG7_HASH_FIELD_BMAP_3r, L2_FIELD_BITMAP_Bf},
    { bcmSwitchHashL2MPLSField0,
        hash_select_mpls_l2, COUNTOF(hash_select_mpls_l2),
        RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, MPLS_L2_PAYLOAD_BITMAP_Af},
    { bcmSwitchHashL2MPLSField1,
        hash_select_mpls_l2, COUNTOF(hash_select_mpls_l2),
        RTAG7_MPLS_L2_PAYLOAD_HASH_FIELD_BMAPr, MPLS_L2_PAYLOAD_BITMAP_Bf},
    { bcmSwitchHashL3MPLSField0,
        hash_select_mpls_l3, COUNTOF(hash_select_mpls_l3),
        RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, MPLS_L3_PAYLOAD_BITMAP_Af},
    { bcmSwitchHashL3MPLSField1,
        hash_select_mpls_l3, COUNTOF(hash_select_mpls_l3),
        RTAG7_MPLS_L3_PAYLOAD_HASH_FIELD_BMAPr, MPLS_L3_PAYLOAD_BITMAP_Bf},
    { bcmSwitchHashMPLSTunnelField0,
        hash_select_mpls_tunnel, COUNTOF(hash_select_mpls_tunnel),
        RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, MPLS_OUTER_BITMAP_Af},
    { bcmSwitchHashMPLSTunnelField1,
        hash_select_mpls_tunnel, COUNTOF(hash_select_mpls_tunnel),
        RTAG7_MPLS_OUTER_HASH_FIELD_BMAPr, MPLS_OUTER_BITMAP_Bf},
    { bcmSwitchHashMIMField0,
        hash_select_mim, COUNTOF(hash_select_mim),
        RTAG7_MIM_PAYLOAD_HASH_FIELD_BMAPr, MIM_PAYLOAD_BITMAP_Af},
    { bcmSwitchHashMIMField1,
        hash_select_mim, COUNTOF(hash_select_mim),
        RTAG7_MIM_PAYLOAD_HASH_FIELD_BMAPr, MIM_PAYLOAD_BITMAP_Bf},
    { bcmSwitchHashMIMTunnelField0,
        hash_select_mim_tunnel, COUNTOF(hash_select_mim_tunnel),
        RTAG7_MIM_OUTER_HASH_FIELD_BMAPr, MIM_OUTER_BITMAP_Af},
    { bcmSwitchHashMIMTunnelField1,
        hash_select_mim_tunnel, COUNTOF(hash_select_mim_tunnel),
        RTAG7_MIM_OUTER_HASH_FIELD_BMAPr, MIM_OUTER_BITMAP_Bf},
    { bcmSwitchHashHG2UnknownField0,
        hash_select_unknown, COUNTOF(hash_select_unknown),
        RTAG7_HASH_FIELD_BMAP_5r, UNKNOWN_PPD_FIELD_BITMAP_Af},
    { bcmSwitchHashHG2UnknownField1,
        hash_select_unknown, COUNTOF(hash_select_unknown),
        RTAG7_HASH_FIELD_BMAP_5r, UNKNOWN_PPD_FIELD_BITMAP_Bf}
};
#endif

/*
 * Function:
 *      _bcm_xgs3_fieldselect_set
 * Description:
 *      Set the enhanced (aka rtag 7) field select settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_fieldselect_set(int unit,
                          bcm_switch_control_t type, int arg)
{
    uint32   hash_control;
    int      i, j, count;
    uint32   flags = 0;
    _hash_fieldselect_t *info, *base_info = hash_select_info;
    count = COUNTOF(hash_select_info);
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        base_info = hash_select_info_tr2;
        count = COUNTOF(hash_select_info_tr2);
    }
#endif

    for (i = 0; i < count; i++) {

         info = base_info + i;
         if (info->type != type) {
             continue;
         }

         for (j = 0; j < info->size; j++) {
              if (info->map[j].flag & arg) {
                  flags |= info->map[j].hw_map;
              }
         }

         SOC_IF_ERROR_RETURN
             (soc_reg32_read(unit, soc_reg_addr(unit, info->reg,
                             REG_PORT_ANY, 0), &hash_control));
         soc_reg_field_set(unit, info->reg,
                           &hash_control, info->field, flags);
         SOC_IF_ERROR_RETURN
             (soc_reg32_write(unit, soc_reg_addr (unit, info->reg,
                              REG_PORT_ANY, 0), hash_control));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_fieldselect_get
 * Description:
 *      Get the current enhanced (aka rtag 7) field select settings.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_fieldselect_get(int unit,
                          bcm_switch_control_t type, int *arg)
{
    uint32   hash_control;
    int      i, j, count;
    uint32   flags;
    _hash_fieldselect_t *info, *base_info = hash_select_info;
    count = COUNTOF(hash_select_info);
#if defined(BCM_TRIUMPH2_SUPPORT)
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
        base_info = hash_select_info_tr2;
        count = COUNTOF(hash_select_info_tr2);
    }
#endif

    for (i = 0; i < count; i++) {

         info = base_info + i;
         if (info->type != type) {
             continue;
         }

         SOC_IF_ERROR_RETURN
             (soc_reg32_read(unit, soc_reg_addr(unit, info->reg,
                             REG_PORT_ANY, 0), &hash_control));
         flags = soc_reg_field_get(unit, info->reg,
                                   hash_control, info->field);
         *arg = 0;
         for (j = 0; j < info->size; j++) {
              if (info->map[j].hw_map & flags) {
                  *arg |= info->map[j].flag;
              }
         }
    }

    return BCM_E_NONE;
}

typedef struct {
    int         idx;
    soc_reg_t   reg;
    soc_field_t sub_f;
    soc_field_t offset_f;
} hash_offset_info_t;

/*
 * Enhanced hash bit selection/offset helper function
 */

STATIC int
_bcm_hash_offset(bcm_switch_control_t type,
                 hash_offset_info_t *info)
{
    switch (type) {
        case bcmSwitchTrunkHashSet0UnicastOffset:
            info->idx = 0;
            info->sub_f = SUB_SEL_UCf;
            info->offset_f = OFFSET_UCf;
            info->reg = RTAG7_HASH_TRUNKr;
            break;
        case bcmSwitchTrunkHashSet0NonUnicastOffset:
            info->idx = 0;
            info->sub_f = SUB_SEL_NONUCf;
            info->offset_f = OFFSET_NONUCf;
            info->reg = RTAG7_HASH_TRUNKr;
            break;
        case bcmSwitchTrunkHashSet1NonUnicastOffset:
            info->idx = 1;
            info->sub_f = SUB_SEL_NONUCf;
            info->offset_f = OFFSET_NONUCf;
            info->reg = RTAG7_HASH_TRUNKr;
            break;
        case bcmSwitchTrunkHashSet1UnicastOffset:
            info->idx = 1;
            info->sub_f = SUB_SEL_UCf;
            info->offset_f = OFFSET_UCf;
            info->reg = RTAG7_HASH_TRUNKr;
            break;
        case bcmSwitchTrunkFailoverHashOffset:
            info->idx = 0;
            info->sub_f = SUB_SELf;
            info->offset_f = OFFSETf;
            info->reg = RTAG7_HASH_PLFSr;
            break;
        case bcmSwitchFabricTrunkHashSet0UnicastOffset:
            info->idx = 0;
            info->sub_f = SUB_SEL_UCf;
            info->offset_f = OFFSET_UCf;
            info->reg = RTAG7_HASH_HG_TRUNKr;
            break;
        case bcmSwitchFabricTrunkHashSet0NonUnicastOffset:
            info->idx = 0;
            info->sub_f = SUB_SEL_NONUCf;
            info->offset_f = OFFSET_NONUCf;
            info->reg = RTAG7_HASH_HG_TRUNKr;
            break;
        case bcmSwitchFabricTrunkHashSet1UnicastOffset:
            info->idx = 1;
            info->sub_f = SUB_SEL_UCf;
            info->offset_f = OFFSET_UCf;
            info->reg = RTAG7_HASH_HG_TRUNKr;
            break;
        case bcmSwitchFabricTrunkHashSet1NonUnicastOffset:
            info->idx = 1;
            info->sub_f = SUB_SEL_NONUCf;
            info->offset_f = OFFSET_NONUCf;
            info->reg = RTAG7_HASH_HG_TRUNKr;
            break;
        case bcmSwitchFabricTrunkFailoverHashOffset:
            info->idx = 0;
            info->sub_f = SUB_SELf;
            info->offset_f = OFFSETf;
            info->reg = RTAG7_HASH_HG_TRUNK_FAILOVERr;
            break;
        case bcmSwitchLoadBalanceHashSet0UnicastOffset:
            info->idx = 0;
            info->sub_f = SUB_SEL_UCf;
            info->offset_f = OFFSET_UCf;
            info->reg = RTAG7_HASH_LBIDr;
            break;
        case bcmSwitchLoadBalanceHashSet0NonUnicastOffset:
            info->idx = 0;
            info->sub_f = SUB_SEL_NONUCf;
            info->offset_f = OFFSET_NONUCf;
            info->reg = RTAG7_HASH_LBIDr;
            break;
        case bcmSwitchLoadBalanceHashSet1UnicastOffset:
            info->idx = 1;
            info->sub_f = SUB_SEL_UCf;
            info->offset_f = OFFSET_UCf;
            info->reg = RTAG7_HASH_LBIDr;
            break;
        case bcmSwitchLoadBalanceHashSet1NonUnicastOffset:
            info->idx = 1;
            info->sub_f = SUB_SEL_NONUCf;
            info->offset_f = OFFSET_NONUCf;
            info->reg = RTAG7_HASH_LBIDr;
            break;
        case bcmSwitchECMPHashSet0Offset:
            info->idx = 0;
            info->sub_f = SUB_SELf;
            info->offset_f = OFFSETf;
            info->reg = RTAG7_HASH_ECMPr;
            break;
        case bcmSwitchECMPHashSet1Offset:
            info->idx = 1;
            info->sub_f = SUB_SELf;
            info->offset_f = OFFSETf;
            info->reg = RTAG7_HASH_ECMPr;
            break;
        default:
	    return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_xgs3_fieldoffset_set
 * Description:
 *      Set the enhanced (aka rtag 7) bits selection offset.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to set.
 *      arg  - BCM_HASH_CONTROL*
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_fieldoffset_set(int unit,
                          bcm_switch_control_t type, int arg)
{
    uint32      hash_control;
    int         sub_field[5] = {16, 16, 4, 5, 8};
    int         offset, i;
    hash_offset_info_t info;

    BCM_IF_ERROR_RETURN(_bcm_hash_offset(type, &info));

    offset = arg % 49;
    for (i = 0; i < 5; i++) {
         offset -= sub_field[i];
         if (offset < 0) {
             offset += sub_field[i];
             break;
         }
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_read(unit, soc_reg_addr(unit, info.reg,
                        REG_PORT_ANY, info.idx), &hash_control));
    soc_reg_field_set(unit, info.reg, &hash_control, info.sub_f, i);
    soc_reg_field_set(unit, info.reg, &hash_control, info.offset_f, offset);
    SOC_IF_ERROR_RETURN
        (soc_reg32_write(unit, soc_reg_addr (unit, info.reg,
                         REG_PORT_ANY, info.idx), hash_control));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_xgs3_fieldoffset_get
 * Description:
 *      Get the current enhanced (aka rtag 7) bit selection offset.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxxx
 */

STATIC int
_bcm_xgs3_fieldoffset_get(int unit,
                          bcm_switch_control_t type, int *arg)
{
    uint32      hash_control;
    int         sub_field[5] = {16, 16, 4, 5, 8};
    int         idx, i;
    hash_offset_info_t info;

    BCM_IF_ERROR_RETURN(_bcm_hash_offset(type, &info));

    SOC_IF_ERROR_RETURN
        (soc_reg32_read(unit, soc_reg_addr
                        (unit, info.reg, REG_PORT_ANY, info.idx), &hash_control));
    idx = soc_reg_field_get(unit, info.reg, hash_control, info.sub_f);
    *arg  = soc_reg_field_get(unit, info.reg, hash_control, info.offset_f);

    for (i = 0; i < idx; i++) {
         *arg += sub_field[i];
    }

    return BCM_E_NONE;
}
#endif /*  BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

STATIC int
_bcm_fb_mod_lb_set(int unit, bcm_port_t port, int arg)
{
    if (soc_feature(unit, soc_feature_module_loopback)) {
        return _bcm_esw_port_config_set(unit, port, _bcmPortModuleLoopback,
                                      (arg ? 1 : 0));
    }
    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_fb_mod_lb_get(int unit, bcm_port_t port, int *arg)
{
    if (soc_feature(unit, soc_feature_module_loopback)) {
        return _bcm_esw_port_config_get(unit, port, _bcmPortModuleLoopback,
                                        arg);
    }
    return BCM_E_UNAVAIL;
}

#define BCM_RAPTOR_BMC_IFG_0    0
#define BCM_RAPTOR_BMC_IFG_16   1
#define BCM_RAPTOR_BMC_IFG_20   2
#define BCM_RAPTOR_BMC_IFG_24   3

#define BCM_FB2_BMC_IFG_MAX     0x3f


#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
STATIC int
_bcm_xgs3_ing_rate_limit_ifg_set(int unit, bcm_port_t port, int arg)
{
#if defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_RAPTOR(unit)) {
        int ifg_sel = 0;
        if (arg >= 20) {
            ifg_sel = BCM_RAPTOR_BMC_IFG_24;
        } else if (arg >= 16) {
            ifg_sel = BCM_RAPTOR_BMC_IFG_20;
        } else if (arg > 0) {
            ifg_sel = BCM_RAPTOR_BMC_IFG_16;
        } else if (arg == 0) {
            ifg_sel = BCM_RAPTOR_BMC_IFG_0;
        } else { /* arg < 0 */
            return BCM_E_PARAM;
        }
        return soc_reg_field32_modify(unit, BKPMETERINGCONFIGr, port,
                                      IFG_ACCT_SELf, ifg_sel);
    } else
#endif /* BCM_RAPTOR_SUPPORT */
#if defined (BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
    if (SOC_REG_IS_VALID(unit, BKPMETERINGCONFIG_EXTr)) {
        if (arg > BCM_FB2_BMC_IFG_MAX) {
            arg = BCM_FB2_BMC_IFG_MAX;
        } else if (arg < 0) {
            return BCM_E_PARAM;
        }
        return soc_reg_field32_modify(unit, BKPMETERINGCONFIG_EXTr, port,
                                      IFG_ACCT_SELf, arg);
    }
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT */
    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_xgs3_ing_rate_limit_ifg_get(int unit, bcm_port_t port, int *arg)
{
    uint32 bmc_reg;

#if defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_RAPTOR(unit)) {
        int ifg_sel;
        BCM_IF_ERROR_RETURN(READ_BKPMETERINGCONFIGr(unit, port, &bmc_reg));
        ifg_sel = soc_reg_field_get(unit, BKPMETERINGCONFIGr, bmc_reg,
                                    IFG_ACCT_SELf);
        switch (ifg_sel) {
        case BCM_RAPTOR_BMC_IFG_24:
            *arg = 24;
            break;
        case BCM_RAPTOR_BMC_IFG_20:
            *arg = 20;
            break;
        case BCM_RAPTOR_BMC_IFG_16:
            *arg = 16;
            break;
        case BCM_RAPTOR_BMC_IFG_0:
            *arg = 0;
            break;
        }
        return BCM_E_NONE;
    }  else
#endif /* BCM_RAPTOR_SUPPORT */
#if defined (BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
    if (SOC_REG_IS_VALID(unit, BKPMETERINGCONFIG_EXTr)) {
        BCM_IF_ERROR_RETURN
            (READ_BKPMETERINGCONFIG_EXTr(unit, port, &bmc_reg));
        *arg = soc_reg_field_get(unit, BKPMETERINGCONFIG_EXTr, bmc_reg,
                                 IFG_ACCT_SELf);
        return BCM_E_NONE;
    }
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT */
    return BCM_E_UNAVAIL;
}
#endif /*  BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT */



#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
STATIC int
_bcm_xgs3_urpf_port_mode_set(int unit, bcm_port_t port, int arg)
{
    if (!soc_feature(unit, soc_feature_urpf)) {
        return (BCM_E_UNAVAIL);
    }

    switch (arg){
      case BCM_SWITCH_URPF_DISABLE:
      case BCM_SWITCH_URPF_LOOSE:
      case BCM_SWITCH_URPF_STRICT:
          return _bcm_esw_port_config_set(unit, port, _bcmPortL3UrpfMode, arg);
      default:
          return (BCM_E_PARAM);
    }
    return (BCM_E_PARAM);
}

STATIC int
_bcm_xgs3_urpf_def_gw_enable(int unit, bcm_port_t port, int arg)
{
    int enable = (arg) ? TRUE : FALSE;

    if (!soc_feature(unit, soc_feature_urpf)) {
        return (BCM_E_UNAVAIL);
    }

    return _bcm_esw_port_config_set(unit, port,
				    _bcmPortL3UrpfDefaultRoute, enable);
}

STATIC int
_bcm_xgs3_urpf_route_enable(int unit,  int arg)
{
#ifdef INCLUDE_L3
    int orig_val;
    uint32 reg_val;

    if (!SOC_REG_IS_VALID(unit, L3_DEFIP_RPF_CONTROLr)) {
        return (BCM_E_UNAVAIL);
    }

    if (!soc_feature(unit, soc_feature_urpf)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(unit, &reg_val));

    /* Handle nothing changed case. */
    orig_val = soc_reg_field_get(unit, L3_DEFIP_RPF_CONTROLr,
                                 reg_val, DEFIP_RPF_ENABLEf);
    if (orig_val == (arg ? 1 : 0)) {
        return (BCM_E_NONE);
    }

    /* State changed -> Delete all the routes. */
    BCM_IF_ERROR_RETURN(bcm_xgs3_defip_del_all(unit));

    /* Destroy Hash/Avl quick lookup structure. */
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_fbx_defip_deinit(unit));

    /* Set urpf route enable bit. */
    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, L3_DEFIP_RPF_CONTROLr, REG_PORT_ANY,
                                DEFIP_RPF_ENABLEf, (arg ? 1 : 0)));


    /* Reinit Hash/Avl quick lookup structure. */
    BCM_IF_ERROR_RETURN(bcm_xgs3_l3_fbx_defip_init(unit));

    return (BCM_E_NONE);
#endif /* INCLUDE_L3 */
    return (BCM_E_UNAVAIL);
}
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#ifdef VENDOR_XYZ
STATIC int
_bcm_fb_l2_src_drop_set(int unit, int arg)
{
    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_fb_l2_src_drop_get(int unit, int *arg)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(BCM_SCORPION_SUPPORT)
STATIC int
_bcm_sc_pfc_sctype_to_class(bcm_switch_control_t type)
{
    int class = 0;

    switch (type) {
    case bcmSwitchPFCClass7Queue:
        class = 7;
        break;
    case bcmSwitchPFCClass6Queue:
        class = 6;
        break;
    case bcmSwitchPFCClass5Queue:
        class = 5;
        break;
    case bcmSwitchPFCClass4Queue:
        class = 4;
        break;
    case bcmSwitchPFCClass3Queue:
        class = 3;
        break;
    case bcmSwitchPFCClass2Queue:
        class = 2;
        break;
    case bcmSwitchPFCClass1Queue:
        class = 1;
        break;
    case bcmSwitchPFCClass0Queue:
        class = 0;
        break;
    default:
        class = -1;
        break;
    }
    return class;
}

STATIC int
_bcm_sc_pfc_priority_to_cos_set(int unit, bcm_switch_control_t type, int arg)
{
    uint32 rval;
    int class = 0;

    if (soc_feature(unit, soc_feature_priority_flow_control)) {
        class = _bcm_sc_pfc_sctype_to_class(type);
        if (class < 0) {
            return BCM_E_INTERNAL;
        }
        if ((arg < 0) || (arg > 7)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN(READ_PRIO2COSr(unit, class, &rval));
        soc_reg_field_set(unit, PRIO2COSr, &rval, COS0_7_BMPf,
                          (1 << arg));
        BCM_IF_ERROR_RETURN(WRITE_PRIO2COSr(unit, class, rval));
        return BCM_E_NONE;
    }
    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_sc_pfc_priority_to_cos_get(int unit, bcm_switch_control_t type,
                                int *arg)
{
    uint32 rval;
    int cosbmp, cos, class = 0;

    if (soc_feature(unit, soc_feature_priority_flow_control)) {
        if (arg == NULL) {
            return BCM_E_PARAM;
        }
        class = _bcm_sc_pfc_sctype_to_class(type);
        if (class < 0) {
            return BCM_E_INTERNAL;
        }

        BCM_IF_ERROR_RETURN(READ_PRIO2COSr(unit, class, &rval));
        cosbmp = soc_reg_field_get(unit, PRIO2COSr, rval, COS0_7_BMPf);
        for (cos = 0; cos < 8; cos++) {
            if (cosbmp & (1 << cos)) {
                *arg = cos;
                return BCM_E_NONE;
            }
        }
        return BCM_E_INTERNAL;
    }
    return BCM_E_UNAVAIL;
}
#endif /* BCM_SCORPION_SUPPORT */

bcm_switch_binding_t xgs3_bindings[] =
{
#if defined (BCM_RAVEN_SUPPORT)
    { bcmSwitchSTPBlockedFieldBypass, RV | HK,
        ING_CONFIGr, BLOCKED_PORTS_FP_DISABLEf,
        NULL, 0},
#endif /* BCM_RAVEN_SUPPORT */

#if defined (BCM_SCORPION_SUPPORT)
    { bcmSwitchSharedVlanMismatchToCpu, SCQ,
        CPU_CONTROL_1r, PVLAN_VID_MISMATCH_TOCPUf,
        NULL, 0},
    { bcmSwitchForceForwardFabricTrunk, SCQ,
        ING_MISC_CONFIGr, LOCAL_SW_DISABLE_HGTRUNK_RES_ENf,
        NULL, 0},
#endif /* BCM_SCORPION_SUPPORT */

#if defined(BCM_EASYRIDER_SUPPORT)
    { bcmSwitchMplsErrToCpu, ER,
    	CPU_CONTROL_2r, MPLSERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchGreSourceRoutingToCpu, ER,
    	CPU_CONTROL_2r, GRE_SOURCE_ROUTING_TOCPUf,
        NULL, 0 },
    { bcmSwitchEnifilterDropToCpu, ER,
    	CPU_CONTROL_2r, ENIFILTER_DROP_TOCPUf,
        NULL, 0 },
    { bcmSwitchMplsPerVlan, ER,
        DEF_VLAN_CONTROLr, MPLS_PER_VLAN_ENABLEf,
        NULL, soc_feature_mpls_per_vlan },
    { bcmSwitchDosAttackIcmpPktOversize, ER,
        DOS_CONTROLr, BIG_ICMP_PKTf,
        NULL, 0 },
    { bcmSwitchMplsPerVlan, ER,
        EGR_CONFIGr, MPLS_PER_VLAN_ENABLEf,
        NULL, soc_feature_mpls_per_vlan },
    { bcmSwitchHgHdrMcastFloodOverride, ER,
        EGR_CONFIGr, STATIC_MH_PFMf,
        NULL, soc_feature_static_pfm },
    { bcmSwitchMirrorUnmarked, ER,
        EGR_CONFIGr, RING_MODEf,
        _bool_invert, 0 },
    { bcmSwitchHgHdrIpMcastFloodOverride, ER,
        EGR_CONFIGr, STATIC_L3MC_PFMf,
        NULL, soc_feature_static_pfm },
    { bcmSwitchRemoteLearnTrust, ER,
        EGR_CONFIG2r, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, soc_feature_remote_learn_trust },
    { bcmSwitchDirectedMirroring, ER,
        IGR_CONFIGr, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchDirectedMirroring, ER,
        MISCCONFIG_2r, DRACO_1_5_MIRRORING_MODE_ENf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchDirectedMirroring, ER,
        SEER_CONFIGr, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchRemoteLearnTrust, ER,
        SEER_CONFIGr, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, soc_feature_remote_learn_trust},
    { bcmSwitchRemoteL2Lookup, ER,
        SEER_CONFIGr, HG_DA_LOOKUP_ENABLEf,
        NULL, 0 },
    { bcmSwitchPortEgressBlockL3, ER,
        SEER_CONFIGr, APPLY_EGR_MASK_ON_L3f,
        NULL, soc_feature_port_egr_block_ctl },
    { bcmSwitchPortEgressBlockL2, ER,
        SEER_CONFIGr, APPLY_EGR_MASK_ON_L2f,
        NULL, soc_feature_port_egr_block_ctl },
    { bcmSwitchIpmcSameVlanL3Route, ER,
        EGR_CONFIGr, IPMC_ROUTE_SAME_VLANf,
        NULL, soc_feature_l3},
#endif /* BCM_EASYRIDER_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchLearnLimitPriority, RV | HK,
        CPU_CONTROL_3r, CPU_MAC_LIMIT_PRIORITYf,
        NULL, soc_feature_mac_learn_limit },
    { bcmSwitchIpmcTunnelToCpu, RP,
        CPU_CONTROL_1r, IPMC_TUNNEL_TO_CPUf,
        NULL,0},
    { bcmSwitchCpuToCpuEnable, RP | RV | HK,
        ING_CONFIGr, DISABLE_COPY_TO_CPU_FOR_CPU_PORTf,
        _bool_invert, 0 },
#endif /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
    { bcmSwitchWlanClientUnauthToCpu, TR2 | AP | VL2,
        CPU_CONTROL_Mr, WLAN_DOT1X_DROPf,
        NULL, 0},
    { bcmSwitchWlanClientRoamedOutErrorToCpu, TR2 | AP | VL2,
        CPU_CONTROL_Mr, WLAN_ROAM_ERRORf,
        NULL, 0},
    { bcmSwitchWlanClientSrcMacMissToCpu, TR2 | AP | VL2,
        CPU_CONTROL_Mr, WCD_SA_MISSf,
        NULL, 0},
    { bcmSwitchWlanClientDstMacMissToCpu, TR2 | AP | VL2,
        CPU_CONTROL_Mr, WCD_DA_MISSf,
        NULL, 0},
    { bcmSwitchOamHeaderErrorToCpu, TR2 | AP | VL2 | EN,
        CPU_CONTROL_Mr, OAM_HEADER_ERROR_TOCPUf,
        NULL, 0},
    { bcmSwitchOamUnknownVersionToCpu, TR2 | AP | VL2 | EN,
        CPU_CONTROL_Mr, OAM_UNKNOWN_OPCODE_VERSION_TOCPUf,
        NULL, 0},
    { bcmSwitchL3SrcBindFailToCpu, TR2 | AP | VL2,
        CPU_CONTROL_Mr, MAC_BIND_FAILf,
        NULL, 0},
    { bcmSwitchTunnelIp4IdShared, TR2 | AP | VL2,
        EGR_TUNNEL_ID_MASKr, SHARED_FRAG_ID_ENABLEf,
        NULL, 0},
    { bcmSwitchIpfixRateViolationDataInsert, TR2 | AP | VL2,
        ING_IPFIX_FLOW_RATE_CONTROLr, SUSPECT_FLOW_INSERT_DISABLEf,
        _bool_invert, soc_feature_ipfix_rate },
    { bcmSwitchIpfixRateViolationPersistent, TR2 | AP | VL2,
        ING_IPFIX_FLOW_RATE_CONTROLr, SUSPECT_FLOW_CONVERT_DISABLEf,
        NULL, 0},
    { bcmSwitchCustomerQueuing, TR2 | AP | VL2,
        ING_MISC_CONFIGr, PHB2_COS_MODEf,
        NULL, soc_feature_vlan_queue_map},
    { bcmSwitchOamUnknownVersionDrop, TR2 | AP | VL2 | EN,
        OAM_DROP_CONTROLr, OAM_UNKNOWN_OPCODE_VERSION_DROPf,
        NULL, 0},
    { bcmSwitchDirectedMirroring, TR2 | AP | VL2,
        EGR_PORT_64r, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchDirectedMirroring, TR2 | AP | VL2,
        IEGR_PORT_64r, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
#endif

#if defined(BCM_ENDURO_SUPPORT)
    { bcmSwitchOamUnexpectedPktToCpu, EN,
        CPU_CONTROL_Mr, OAM_UNEXPECTED_PKT_TOCPUf,
        NULL, 0},
#endif

#if defined(BCM_TRX_SUPPORT)
    { bcmSwitchClassBasedMoveFailPktToCpu, TRX,
        CPU_CONTROL_1r, CLASS_BASED_SM_PREVENTED_TOCPUf,
        NULL, soc_feature_class_based_learning },
    { bcmSwitchL3UrpfFailToCpu,            TRX,
        CPU_CONTROL_1r, URPF_MISS_TOCPUf,
        NULL, soc_feature_urpf },
    { bcmSwitchClassBasedMoveFailPktDrop,  TRX,
        ING_MISC_CONFIG2r, CLASS_BASED_SM_PREVENTED_DROPf,
        NULL, soc_feature_class_based_learning },
    { bcmSwitchSTPBlockedFieldBypass,      TRX,
        ING_MISC_CONFIG2r, BLOCKED_PORTS_FP_DISABLEf,
        NULL, 0},
    { bcmSwitchRateLimitLinear,            TRX,
        MISCCONFIGr, ITU_MODE_SELf,
        _bool_invert, 0},
    { bcmSwitchRemoteLearnTrust,           TRX,
        ING_CONFIG_64r, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, 0 },
    { bcmSwitchMldDirectAttachedOnly,      TRX,
        ING_CONFIG_64r, MLD_CHECKS_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchSharedVlanL2McastEnable,    TRX,
        ING_CONFIG_64r, LOOKUP_L2MC_WITH_FID_IDf,
        NULL, 0},
    { bcmSwitchMldUcastEnable,             TRX,
        ING_CONFIG_64r, MLD_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchSharedVlanEnable, TRX,
        ING_CONFIG_64r, SVL_ENABLEf,
        NULL, 0},
    { bcmSwitchIgmpReservedMcastEnable,    TRX,
        ING_CONFIG_64r, IPV4_RESERVED_MC_ADDR_IGMP_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMldReservedMcastEnable,     TRX,
        ING_CONFIG_64r, IPV6_RESERVED_MC_ADDR_MLD_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchIgmpUcastEnable,            TRX,
        ING_CONFIG_64r, IGMP_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchPortEgressBlockL3,          TRX,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L3f,
        NULL, 0},
    { bcmSwitchPortEgressBlockL2,          TRX,
        ING_CONFIG_64r, APPLY_EGR_MASK_ON_L2f,
        NULL, 0},
    { bcmSwitchHgHdrExtLengthEnable,       TRX,
        ING_CONFIG_64r, IGNORE_HG_HDR_HDR_EXT_LENf,
        _bool_invert, 0 },
    { bcmSwitchIp4McastL2DestCheck,        TRX,
        ING_CONFIG_64r, IPV4_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
    { bcmSwitchIp6McastL2DestCheck,        TRX,
        ING_CONFIG_64r, IPV6_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
    { bcmSwitchL3TunnelUrpfMode,           TRX,
        ING_CONFIG_64r, TUNNEL_URPF_MODEf,
        NULL, soc_feature_urpf },
    { bcmSwitchL3TunnelUrpfDefaultRoute,   TRX,
        ING_CONFIG_64r, TUNNEL_URPF_DEFAULTROUTECHECKf,
        _bool_invert, soc_feature_urpf },
    { bcmSwitchMirrorStackMode, TRX,
        ING_CONFIG_64r, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchDirectedMirroring, TRX,
        ING_CONFIG_64r, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchMirrorSrcModCheck, TRX,
        ING_CONFIG_64r, FB_A0_COMPATIBLEf,
        NULL, soc_feature_src_modid_blk },
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
    { bcmSwitchDosAttackToCpu, TRVL,
        CPU_CONTROL_0r, DOSATTACK_TOCPUf,
        NULL, 0 },
    { bcmSwitchParityErrorToCpu, TRVL,
        CPU_CONTROL_0r, PARITY_ERR_TOCPUf,
        NULL, soc_feature_parity_err_tocpu },
    { bcmSwitchUnknownVlanToCpu, TRVL,
    	CPU_CONTROL_0r, UVLAN_TOCPUf,
        NULL, 0 },
    { bcmSwitchSourceMacZeroDrop, TRVL,
        CPU_CONTROL_0r, MACSA_ALL_ZERO_DROPf,
        NULL, 0},
    { bcmSwitchMplsSequenceErrToCpu, TRVL,
        ING_MISC_CONFIGr, MPLS_SEQ_NUM_FAIL_TOCPUf,
        NULL, 0},
    { bcmSwitchMplsLabelMissToCpu, TRVL,
        CPU_CONTROL_Mr, MPLS_LABEL_MISSf,
        NULL, 0},
    { bcmSwitchMplsTtlErrToCpu, TRVL,
        CPU_CONTROL_Mr, MPLS_TTL_CHECK_FAILf,
        NULL, 0},
    { bcmSwitchMplsInvalidL3PayloadToCpu, TRVL,
        CPU_CONTROL_Mr, MPLS_INVALID_PAYLOADf,
        NULL, 0},
    { bcmSwitchMplsInvalidActionToCpu, TRVL,
        CPU_CONTROL_Mr, MPLS_INVALID_ACTIONf,
        NULL, 0},
    { bcmSwitchSharedVlanMismatchToCpu, TRVL,
        CPU_CONTROL_0r, PVLAN_VID_MISMATCH_TOCPUf,
        NULL, 0},
    { bcmSwitchForceForwardFabricTrunk, TRVL,
        ING_MISC_CONFIGr, LOCAL_SW_DISABLE_HGTRUNK_RES_ENf,
        NULL, 0},
    { bcmSwitchFieldMultipathHashSeed, TRVL,
        FP_ECMP_HASH_CONTROLr, ECMP_HASH_SALTf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange1, TRVL,
        GLOBAL_MPLS_RANGE_1_LOWERr, LABELf,
        NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange1, TRVL,
	GLOBAL_MPLS_RANGE_1_UPPERr, LABELf,
	NULL, 0},
    { bcmSwitchMplsPortIndependentLowerRange2, TRVL,
       GLOBAL_MPLS_RANGE_2_LOWERr, LABELf,
       NULL, 0},
    { bcmSwitchMplsPortIndependentUpperRange2, TRVL,
	GLOBAL_MPLS_RANGE_2_UPPERr, LABELf,
	NULL, 0},
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT)
    { bcmSwitchL3UrpfFailToCpu, FB2,
        CPU_CONTROL_1r, URPF_MISS_TOCPUf,
        NULL, soc_feature_urpf },
    { bcmSwitchCpuLookupFpCopyPrio, FB2,
        CPU_CONTROL_2r, CPU_VFPCOPY_PRIORITYf,
        NULL, 0 },
    { bcmSwitchL3TunnelUrpfMode, FB2,
        ING_CONFIGr, TUNNEL_URPF_MODEf,
        NULL, soc_feature_urpf },
    { bcmSwitchL3TunnelUrpfDefaultRoute, FB2,
        ING_CONFIGr, TUNNEL_URPF_DEFAULTROUTECHECKf,
        _bool_invert, soc_feature_urpf },
    { bcmSwitchHgHdrExtLengthEnable, FB2,
        ING_CONFIGr, IGNORE_HG_HDR_HDR_EXT_LENf,
        _bool_invert, 0 },
    { bcmSwitchUniformFabricTrunkDistribution, FB2,
        ING_MISC_CONFIGr, UNIFORM_HG_TRUNK_DIST_ENABLEf,
        NULL, soc_feature_trunk_group_size },
    { bcmSwitchRateLimitLinear, FB2,
        MISCCONFIGr, ITU_MODE_SELf,
        _bool_invert, 0},
#endif /* BCM_FIREBOLT2_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchCpuToCpuEnable, FB2 | TRX,
        ING_MISC_CONFIGr, DO_NOT_COPY_FROM_CPU_TO_CPUf,
        _bool_invert, 0 },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
    { bcmSwitchCpuProtocolPrio, FB | HX | FX | ER,
        CPU_CONTROL_2r, CPU_PROTOCOL_PRIORITYf,
        NULL, 0 },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_EASYRIDER_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT)
    { bcmSwitchAlternateStoreForward, FB | HXFX,
        ASFCONFIGr, ASF_ENf,
        NULL, soc_feature_asf },
   { bcmSwitchRemoteLearnTrust, FB | HX | FX,
        ING_CONFIGr, DO_NOT_LEARN_ENABLEf,
        NULL, soc_feature_remote_learn_trust },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchIgmpUcastEnable, FB2 | RP | RV | HK,
        ING_CONFIGr, IGMP_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchMldUcastEnable, FB2 | RP | RV | HK,
        ING_CONFIGr, MLD_PKTS_UNICAST_IGNOREf,
        _bool_invert, soc_feature_igmp_mld_support },
    { bcmSwitchIgmpReservedMcastEnable, FB2 | RP | RV | HK,
        ING_CONFIGr, IPV4_RESERVED_MC_ADDR_IGMP_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMldReservedMcastEnable, FB2 | RP | RV | HK,
        ING_CONFIGr, IPV6_RESERVED_MC_ADDR_MLD_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMldDirectAttachedOnly, FB2 | RP | RV | HK,
        ING_CONFIGr, MLD_CHECKS_ENABLEf,
        NULL, soc_feature_igmp_mld_support },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)
    { bcmSwitchIp4McastL2DestCheck, FB2 | RV | HK,
        ING_CONFIGr, IPV4_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
    { bcmSwitchIp6McastL2DestCheck, FB2 | RV | HK,
        ING_CONFIGr, IPV6_MC_MACDA_CHECK_ENABLEf,
        NULL, 0},
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchUniformUcastTrunkDistribution, FB | TRX,
        ING_MISC_CONFIGr, UNIFORM_TRUNK_DIST_ENABLEf,
        NULL, soc_feature_trunk_group_size },
    { bcmSwitchL3UrpfRouteEnable, FB2 | TRX,
        L3_DEFIP_RPF_CONTROLr, DEFIP_RPF_ENABLEf,
        NULL, soc_feature_urpf},
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchStationMoveOverLearnLimitToCpu, RV | TRX | HK,
        CPU_CONTROL_1r, MACLMT_STNMV_TOCPUf,
        NULL, soc_feature_mac_learn_limit},
#endif /* BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchControlOpcodePriority, HBGW | RP | RV | HK,
        CPU_CONTROL_2r, CPU_MH_CONTROL_PRIORITYf,
        NULL, 0 },
#endif /* BCM_BRADLEY_SUPPORT || BCM_RAPTOR_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    { bcmSwitchAlternateStoreForward, HBGW | SCQ,
        OP_THR_CONFIGr, ASF_ENABLEf,
        NULL, soc_feature_asf },
#endif /* BCM_BRADLEY_SUPPORT  || BCM_SCORPION_SUPPORT */

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchHashSeed0, HBGW | TRX,
        RTAG7_HASH_SEED_Ar, HASH_SEED_Af,
        NULL, 0 },
    { bcmSwitchHashSeed1, HBGW | TRX,
        RTAG7_HASH_SEED_Br, HASH_SEED_Bf,
        NULL, 0 },
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchBpduInvalidVlanDrop, FB2 | RP | RV | TRX | HK,
        EGR_CONFIG_1r, BPDU_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMirrorInvalidVlanDrop, FB2 | RP | RV | TRX | HK,
        EGR_CONFIG_1r, MIRROR_INVALID_VLAN_DROPf,
        NULL, soc_feature_igmp_mld_support },
    { bcmSwitchMcastFloodBlocking, FB2 | RP | RV | TRX | HK,
        IGMP_MLD_PKT_CONTROLr, PFM_RULE_APPLYf,
        NULL, soc_feature_igmp_mld_support },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchL3SlowpathToCpu, FB2 | RV | TRX | HK,
        CPU_CONTROL_1r, IPMC_TTL1_ERR_TOCPUf,
        NULL, 0},
    { bcmSwitchL3SlowpathToCpu, FB2 | RV | TRX | HK,
        CPU_CONTROL_1r, L3UC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcTtl1ToCpu, FB2 | RV | TRX | HK,
        CPU_CONTROL_1r, IPMC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3UcastTtl1ToCpu, FB2 | RV | TRX | HK,
        CPU_CONTROL_1r, L3UC_TTL1_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchDhcpLearn, FB2 | RV | TRX | HK,
        ING_MISC_CONFIG2r, DO_NOT_LEARN_DHCPf,
        NULL, 0 },
    { bcmSwitchUnknownIpmcAsMcast, FB2 | RV | TRX | HK,
        ING_MISC_CONFIG2r, IPMC_MISS_AS_L2MCf,
        NULL, 0 },
    { bcmSwitchTunnelUnknownIpmcDrop, FB2 | RV | TRX | HK,
        ING_MISC_CONFIG2r, UNKNOWN_TUNNEL_IPMC_DROPf,
        NULL, 0 },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchMcastUnknownErrToCpu, BR | GW | RP | TRX | RV | HK,
        CPU_CONTROL_1r, MC_INDEX_ERROR_TOCPUf,
        NULL,0},
#endif /* BCM_RAPTOR_SUPPORT || BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT) ||\
    defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchDosAttackMACSAEqualMACDA, FB2 | RP | TRX | RV | HK,
        DOS_CONTROLr, MACSA_EQUALS_MACDA_DROPf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackFlagZeroSeqZero, FB2 | RP | TRX | RV | HK,
        DOS_CONTROLr, TCP_FLAGS_CTRL0_SEQ0_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackSynFrag, FB2 | RP | TRX | RV | HK,
        DOS_CONTROLr, TCP_FLAGS_SYN_FRAG_ENABLEf,
        NULL, 0},
    { bcmSwitchDosAttackIcmp, FB2 | RP | TRX | RV,
        DOS_CONTROL_2r, ICMP_V4_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmp, FB2 | RP | TRX | RV,
        DOS_CONTROL_2r, ICMP_V6_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpFragments, FB2 | RP | TRX | RV,
        DOS_CONTROL_2r, ICMP_FRAG_PKTS_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpOffset, FB2 | RP | TRX | RV,
        DOS_CONTROL_2r, TCP_HDR_OFFSET_EQ1_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackUdpPortsEqual, FB2 | RP | TRX | RV,
        DOS_CONTROL_2r, UDP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpPortsEqual, FB2 | RP | TRX | RV,
        DOS_CONTROL_2r, TCP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsSF, FB2 | RP | TRX | RV,
        DOS_CONTROL_2r, TCP_FLAGS_SYN_FIN_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsFUP, FB2 | RP | TRX | RV,
        DOS_CONTROL_2r, TCP_FLAGS_FIN_URG_PSH_SEQ0_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpHdrPartial, FB2 | RP | TRX | RV,
        DOS_CONTROL_2r, TCP_HDR_PARTIAL_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },


#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT || BCM_RAPTOR_SUPPORT */

#if defined(BCM_HAWKEYE_SUPPORT)
    { bcmSwitchDosAttackIcmp, HK,
        DOS_CONTROLr, ICMP_V4_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmp, HK,
        DOS_CONTROLr, ICMP_V6_PING_SIZE_ENABLEf,
        NULL, 0 },
    { bcmSwitchDosAttackIcmpFragments, HK,
        DOS_CONTROLr, ICMP_FRAG_PKTS_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpOffset, HK,
        DOS_CONTROLr, TCP_HDR_OFFSET_EQ1_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackUdpPortsEqual, HK,
        DOS_CONTROLr, UDP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpPortsEqual, HK,
        DOS_CONTROLr, TCP_SPORT_EQ_DPORT_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsSF, HK,
        DOS_CONTROLr, TCP_FLAGS_SYN_FIN_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpFlagsFUP, HK,
        DOS_CONTROLr, TCP_FLAGS_FIN_URG_PSH_SEQ0_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackTcpHdrPartial, HK,
        DOS_CONTROLr, TCP_HDR_PARTIAL_ENABLEf,
        NULL, soc_feature_enhanced_dos_ctrl },
    { bcmSwitchDosAttackIcmpPktOversize, HK,
        DOS_CONTROL_2r, BIG_ICMP_PKT_SIZEf,
        NULL, 0 },

#endif /* BCM_HAWKEYE_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_SCORPION_SUPPORT) ||\
    defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchSourceMacZeroDrop, FB2 | RP | RV | SCQ | HK,
        ING_MISC_CONFIG2r, MACSA_ALL_ZERO_DROPf,
        NULL, 0},
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_SCORPION_SUPPORT || BCM_RAPTOR_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) ||\
    defined(BCM_BRADLEY_SUPPORT)
    { bcmSwitchRemoteLearnTrust, BR | GW | RP | FB2 | RV | HK,
        ING_CONFIGr, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, 0 },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_BRADLEY_SUPPORT || BCM_RAPTOR_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) ||\
    defined(BCM_BRADLEY_SUPPORT)
    { bcmSwitchDirectedMirroring, FB | HBGW | HX | FX,
        ING_CONFIGr, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchDirectedMirroring, FB | HBGW | HX | FX | TRX,
        EGR_PORTr, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchDirectedMirroring, FB | HBGW | HX | FX | TRX,
        IEGR_PORTr, EM_SRCMOD_CHANGEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorStackMode, FB | HBGW | HXFX,
        ING_CONFIGr, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorStackMode, FB | HBGW | HX | FX | TRVL,
        MISCCONFIGr, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMirrorSrcModCheck, FB | HBGW | HXFX,
        ING_CONFIGr, FB_A0_COMPATIBLEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchSrcModBlockMirrorCopy, FB | HBGW | HXFX,
        MIRROR_CONTROLr, SRC_MODID_BLOCK_MIRROR_COPYf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchSrcModBlockMirrorOnly, FB | HBGW | HXFX,
        MIRROR_CONTROLr, SRC_MODID_BLOCK_MIRROR_ONLY_PKTf,
        NULL, soc_feature_src_modid_blk },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT || BCM_BRADLEY_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) ||\
    defined(BCM_RAVEN_SUPPORT)
    { bcmSwitchDestPortHGTrunk, FB | HXFX | RV | HK,
        ING_MISC_CONFIGr, USE_DEST_PORTf,
        NULL, soc_feature_port_trunk_index },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT || BCM_RAVEN_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_HELIX_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchDosAttackIcmpPktOversize, FB | BR | GW | HXFX | TRX,
        DOS_CONTROLr, BIG_ICMP_PKT_SIZEf,
        NULL, 0 },
    { bcmSwitchDirectedMirroring, FB | HBGW | HX | FX | TRX,
        MISCCONFIGr, DRACO_1_5_MIRRORING_MODE_ENf,
        _bool_invert, soc_feature_xgs1_mirror },
    { bcmSwitchDosAttackIcmpV6PingSize, FB | HXFX | BR | GW | TRX,
        DOS_CONTROL_2r, BIG_ICMPV6_PKT_SIZEf,
        NULL, soc_feature_big_icmpv6_ping_check },
    { bcmSwitchMirrorUnmarked, FB | HBGW | HXFX | TRX,
        EGR_CONFIG_1r, RING_MODEf,
        _bool_invert, 0 },
    { bcmSwitchMirrorStackMode, FB | HBGW | HX | FX | TRX,
        EGR_CONFIG_1r, STACK_MODEf,
        NULL, soc_feature_src_modid_blk },
    { bcmSwitchMeterAdjust, FB | HBGW | HXFX | TRX,
        EGR_SHAPING_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
#ifndef LVL7_FIXUP
    /* Need to open a GNATs to provide an additional control here. We want to
       be able to configure the IFG for IFP and EFP meters separately 
       from egress BW meters. For now, don't configure IFP and EFP
       meters when bcmSwitchMeterAdjust is configured. */
    { bcmSwitchMeterAdjust, FB | HBGW | HXFX | TRX,
        FP_METER_CONTROLr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchMeterAdjust, TRVL,
        EFP_METER_CONTROL_2r, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
#endif
    { bcmSwitchMeterAdjust, TRVL,
        MTRI_IFGr, PACKET_IFG_BYTESf,
        NULL, soc_feature_meter_adjust },
    { bcmSwitchEgressBlockUcast, FB | HBGW | HXFX | TRX,
        ING_MISC_CONFIGr, APPLY_EGR_MASK_ON_UC_ONLYf,
        NULL, soc_feature_egress_blk_ucast_override },
    { bcmSwitchSourceModBlockUcast, FB | HBGW | HXFX | TRX,
        ING_MISC_CONFIGr, APPLY_SRCMOD_BLOCK_ON_UC_ONLYf,
        NULL, soc_feature_src_modid_blk_ucast_override },
    { bcmSwitchHgHdrMcastFloodOverride, FB | BR | GW | HXFX | TRX,
        EGR_CONFIG_1r, FORCE_STATIC_MH_PFMf,
        NULL, soc_feature_static_pfm },
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_BRADLEY_SUPPORT ||
          BCM_HELIX_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    { bcmSwitchHgHdrErrToCpu, FB2 | HBGW | RP | TRX | RV | HK,
        CPU_CONTROL_1r, HG_HDR_ERROR_TOCPUf,
        NULL, soc_feature_cpu_proto_prio},
    { bcmSwitchClassTagPacketsToCpu, FB2 | HBGW | RP | TRX | RV | HK,
        CPU_CONTROL_1r, HG_HDR_TYPE1_TOCPUf,
        NULL, soc_feature_cpu_proto_prio},
    { bcmSwitchRemoteLearnTrust, BR | GW | RP | RV | FB2 | TRX | HK,
        EGR_CONFIG_1r, IGNORE_HG_HDR_DONOT_LEARNf,
        _bool_invert, 0 },
    { bcmSwitchSourceModBlockControlOpcode, FB2 | HBGW | RP | TRX | RV | HK,
        ING_MISC_CONFIGr, DO_NOT_APPLY_SRCMOD_BLOCK_ON_SCf,
        _bool_invert, soc_feature_src_modid_blk_opcode_override },
#endif /* BCM_RAPTOR_SUPPORT || BCM_BRADLEY_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
    { bcmSwitchCpuProtoBpduPriority, FB2 | ER | BR | GW | RP | RV | HK,
        CPU_CONTROL_3r, CPU_PROTO_BPDU_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoArpPriority, FB2 | ER | BR | GW | RP | RV | HK,
        CPU_CONTROL_3r, CPU_PROTO_ARP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoIgmpPriority, FB2 | ER | BR | GW | RP | RV | HK,
        CPU_CONTROL_3r, CPU_PROTO_IGMP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoDhcpPriority, FB2 | ER | BR | GW | RP | RV | HK,
        CPU_CONTROL_3r, CPU_PROTO_DHCP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoIpmcReservedPriority, FB2 | ER | BR | GW | RP | RV | HK,
        CPU_CONTROL_3r, CPU_PROTO_IPMC_RESERVED_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoIpOptionsPriority, FB2 | ER | BR | GW | RP | RV | HK,
        CPU_CONTROL_3r, CPU_PROTO_IP_OPTIONS_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoExceptionsPriority, FB2 | ER | BR | GW | RP | RV | HK,
        CPU_CONTROL_3r, CPU_PROTO_EXCEPTIONS_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
#endif /* BCM_RAPTOR_SUPPORT || BCM_BRADLEY_SUPPORT ||
          BCM_FIREBOLT2_SUPPORT || BCM_EASYRIDER_SUPPORT */


#if defined(BCM_HELIX_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT) || \
    defined(BCM_EASYRIDER_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    { bcmSwitchUnknownIpmcToCpu, FB | BR | GW | HXFX | ER,
    	CPU_CONTROL_1r, UIPMC_TOCPUf,
        NULL, soc_feature_unknown_ipmc_tocpu },
    { bcmSwitchIcmpRedirectToCpu, FB | BR | GW | HXFX | ER,
        CPU_CONTROL_1r, ICMP_REDIRECT_TOCPUf,
        NULL, 0 },
    { bcmSwitchCpuUnknownPrio, FB | BR | GW | HXFX | ER,
	    CPU_CONTROL_2r, CPU_LKUPFAIL_PRIORITYf,
        NULL, 0 },
    { bcmSwitchCpuSamplePrio, FB | BR | GW | HXFX | ER,
    	CPU_CONTROL_2r, CPU_SFLOW_PRIORITYf,
        NULL, 0 },
    { bcmSwitchCpuDefaultPrio, FB | BR | GW | HXFX | ER,
    	CPU_CONTROL_2r, CPU_DEFAULT_PRIORITYf,
        NULL, 0 },
    { bcmSwitchCpuMtuFailPrio, FB | BR | GW | HXFX | ER,
	    CPU_CONTROL_2r, CPU_MTUFAIL_PRIORITYf,
        NULL, 0 },
    { bcmSwitchCpuMirrorPrio, FB | BR | GW | HXFX | ER,
	    CPU_CONTROL_2r, CPU_MIRROR_PRIORITYf,
        NULL, 0 },
    { bcmSwitchCpuIcmpRedirectPrio, FB | BR | GW | HXFX | ER,
	    CPU_CONTROL_2r, CPU_ICMP_REDIRECT_PRIORITYf,
        NULL, 0 },
     { bcmSwitchCpuFpCopyPrio, FB | HBGW | HXFX | ER,
    	CPU_CONTROL_2r, CPU_FPCOPY_PRIORITYf,
        NULL, 0 },
    { bcmSwitchDosAttackTcpFlags, FB | BR | GW | HX | FX | ER,
        DOS_CONTROLr, TCP_FLAGS_CHECK_ENABLEf,
        NULL, soc_feature_basic_dos_ctrl },
    { bcmSwitchDosAttackL4Port,   FB | BR | GW | HX | FX | ER,
        DOS_CONTROLr, L4_PORT_CHECK_ENABLEf,
        NULL, soc_feature_basic_dos_ctrl },
    { bcmSwitchDosAttackTcpFrag,  FB | BR | GW | HX | FX | ER,
        DOS_CONTROLr, TCP_FRAG_CHECK_ENABLEf,
        NULL, soc_feature_basic_dos_ctrl },
    { bcmSwitchDosAttackIcmp,     FB | BR | GW | HX | FX | ER,
        DOS_CONTROLr, ICMP_CHECK_ENABLEf,
        NULL, soc_feature_basic_dos_ctrl },
#endif /*  BCM_HELIX_SUPPORT || BCM_FIREBOLT_SUPPORT ||
           BCM_EASYRIDER_SUPPORT || BCM_BRADLEY_SUPPORT */

#if defined(BCM_HELIX_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT) || \
    defined(BCM_EASYRIDER_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchUnknownMcastToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, UMC_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownUcastToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, UUCAST_TOCPUf,
        NULL, 0 },
    { bcmSwitchL2StaticMoveToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, STATICMOVE_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3HeaderErrToCpu, FB | BR | GW | HXFX | ER | TRX, /* compat */
    	CPU_CONTROL_1r, V4L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownL3SrcToCpu, FB | BR | GW | HXFX | ER | TRX,
	    CPU_CONTROL_1r, UNRESOLVEDL3SRC_TOCPUf,
        NULL, 0 },
    { bcmSwitchUnknownL3DestToCpu, FB | BR | GW | HXFX | ER | TRX, /* compat */
    	CPU_CONTROL_1r, V4L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcPortMissToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, IPMCPORTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcErrorToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, IPMCERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL2NonStaticMoveToCpu, FB | BR | GW | HXFX | ER | TRX,
	    CPU_CONTROL_1r, NONSTATICMOVE_TOCPUf,
        NULL, 0 },
    { bcmSwitchV6L3ErrToCpu, FB | BR | GW | HXFX | ER | TRX,
     	CPU_CONTROL_1r, V6L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchV6L3DstMissToCpu, FB | BR | GW | HXFX | ER | TRX,
	    CPU_CONTROL_1r, V6L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchV4L3ErrToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, V4L3ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchV4L3DstMissToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, V4L3DSTMISS_TOCPUf,
        NULL, 0 },
    { bcmSwitchTunnelErrToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, TUNNEL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchMartianAddrToCpu, FB | BR | GW | HXFX | ER | TRX,
     	CPU_CONTROL_1r, MARTIAN_ADDR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3UcTtlErrToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, L3UC_TTL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchL3SlowpathToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, L3_SLOWPATH_TOCPUf,
        NULL, 0 },
    { bcmSwitchIpmcTtlErrToCpu, FB | BR | GW | HXFX | ER | TRX,
    	CPU_CONTROL_1r, IPMC_TTL_ERR_TOCPUf,
        NULL, 0 },
    { bcmSwitchSampleIngressRandomSeed, FB | BR | GW | HXFX | ER | TRX,
	    SFLOW_ING_RAND_SEEDr, SEEDf,
        NULL, 0 },
    { bcmSwitchSampleEgressRandomSeed, FB | BR | GW | HXFX | ER | TRX,
    	SFLOW_EGR_RAND_SEEDr, SEEDf,
        NULL, 0 },
    { bcmSwitchArpReplyToCpu, FB | BR | GW | HXFX | ER | TRX,
	    PROTOCOL_PKT_CONTROLr, ARP_REPLY_TO_CPUf,
        NULL, 0 },
    { bcmSwitchArpReplyDrop, FB | BR | GW | HXFX | ER | TRX,
    	PROTOCOL_PKT_CONTROLr, ARP_REPLY_DROPf,
        NULL, 0 },
    { bcmSwitchArpRequestToCpu, FB | BR | GW | HXFX | ER | TRX,
    	PROTOCOL_PKT_CONTROLr, ARP_REQUEST_TO_CPUf,
        NULL, 0 },
    { bcmSwitchArpRequestDrop, FB | BR | GW | HXFX | ER | TRX,
    	PROTOCOL_PKT_CONTROLr, ARP_REQUEST_DROPf,
        NULL, 0 },
    { bcmSwitchNdPktToCpu, FB | BR | GW | HXFX | ER | TRX,
    	PROTOCOL_PKT_CONTROLr, ND_PKT_TO_CPUf,
        NULL, 0 },
    { bcmSwitchNdPktDrop, FB | BR | GW | HXFX | ER | TRX,
    	PROTOCOL_PKT_CONTROLr, ND_PKT_DROPf,
        NULL, 0 },
    { bcmSwitchDhcpPktToCpu, FB | BR | GW | HXFX | ER | TRX,
        PROTOCOL_PKT_CONTROLr, DHCP_PKT_TO_CPUf,
        NULL, 0 },
    { bcmSwitchDhcpPktDrop, FB | BR | GW | HXFX | ER | TRX,
        PROTOCOL_PKT_CONTROLr, DHCP_PKT_DROPf,
        NULL, 0 },
    { bcmSwitchDosAttackSipEqualDip, FB | BR | GW | HXFX | ER | TRX,
        DOS_CONTROLr, DROP_IF_SIP_EQUALS_DIPf,
        NULL, 0 },
    { bcmSwitchDosAttackMinTcpHdrSize, FB | BR | GW | HXFX | ER | TRX,
        DOS_CONTROLr, MIN_TCPHDR_SIZEf,
        NULL, 0 },
    { bcmSwitchDosAttackV4FirstFrag, FB | BR | GW | HXFX | ER | TRX,
        DOS_CONTROLr, IPV4_FIRST_FRAG_CHECK_ENABLEf,
        NULL, 0 },
    { bcmSwitchNonIpL3ErrToCpu, FB | BR | GW |  HXFX | ER | TRX,
        CPU_CONTROL_1r, NIP_L3ERR_TOCPUf,
        NULL, soc_feature_nip_l3_err_tocpu },
    { bcmSwitchSourceRouteToCpu, FB | BR | GW | HXFX | ER | TRX,
	    CPU_CONTROL_1r, SRCROUTE_TOCPUf,
        NULL, 0 },
    { bcmSwitchParityErrorToCpu, ER | FB | HBGW | HXFX | SCQ,
        CPU_CONTROL_1r, PARITY_ERR_TOCPUf,
        NULL, soc_feature_parity_err_tocpu },
    { bcmSwitchDirectedMirroring, ER | FB | HBGW | HX | FX | TRX,
        EGR_CONFIGr, DRACO1_5_MIRRORf,
        _bool_invert, soc_feature_xgs1_mirror},
#endif /* BCM_HELIX_SUPPORT || BCM_FIREBOLT_SUPPORT ||
          BCM_EASYRIDER_SUPPORT || BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_HELIX_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT) || \
    defined(BCM_EASYRIDER_SUPPORT) || defined(BCM_BRADLEY_SUPPORT) || \
    defined(BCM_SCORPION_SUPPORT)
    { bcmSwitchDosAttackToCpu, FB | BR | GW | HXFX | ER | SCQ,
    	CPU_CONTROL_1r, DOSATTACK_TOCPUf,
        NULL, 0 },
#endif /* BCM_HELIX_SUPPORT || BCM_FIREBOLT_SUPPORT || BCM_EASYRIDER_SUPPORT ||
          BCM_BRADLEY_SUPPORT || BCM_SCORPION_SUPPORT*/

#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT) || \
    defined(BCM_RAPTOR_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
    { bcmSwitchUnknownVlanToCpu, HBGW | ER | RP | RV | SCQ | HK,
    	CPU_CONTROL_1r, UVLAN_TOCPUf,
        NULL, 0 },
        
#endif /* BCM_BRADLEY_SUPPORT || BCM_EASYRIDER_SUPPORT ||
          BCM_RAPTOR_SUPPORT   || BCM_SCORPION_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) || \
    defined(BCM_BRADLEY_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
    { bcmSwitchPortEgressBlockL3, FB | HXFX | HBGW | RP | RV | HK,
        ING_CONFIGr, APPLY_EGR_MASK_ON_L3f,
        NULL, soc_feature_port_egr_block_ctl },
    { bcmSwitchPortEgressBlockL2, FB | HXFX | HBGW | RP | RV | HK,
        ING_CONFIGr, APPLY_EGR_MASK_ON_L2f,
        NULL, soc_feature_port_egr_block_ctl },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT ||
          BCM_BRADLEY_SUPPORT || BCM_RAPTOR_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_HELIX_SUPPORT) || \
    defined(BCM_BRADLEY_SUPPORT) || defined(BCM_RAVEN_SUPPORT) ||  \
    defined(BCM_TRX_SUPPORT)
    { bcmSwitchL3MtuFailToCpu, ER | FB | BR | GW | HXFX | TRX | RV | HK,
        CPU_CONTROL_1r, L3_MTU_FAIL_TOCPUf,
        NULL, soc_feature_l3mtu_fail_tocpu },
    { bcmSwitchIpmcSameVlanL3Route, FB | BR | GW | HXFX | TRX | RV | HK,
        EGR_CONFIG_1r, IPMC_ROUTE_SAME_VLANf,
        NULL, soc_feature_l3},
    { bcmSwitchIpmcSameVlanPruning, FB | BR | GW | HXFX | TRX | RV | HK,
    	MISCCONFIGr, IPMC_IND_MODEf,
        _bool_invert, 0 },
#endif /* BCM_FIREBOLT_SUPPORT || BCM_HELIX_SUPPORT ||
          BCM_BRADLEY_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */

#if defined(BCM_HAWKEYE_SUPPORT)
    { bcmSwitchCpuProtoTimeSyncPrio, HK,
        CPU_CONTROL_4r, CPU_PROTO_TS_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoMmrpPrio, HK,
        CPU_CONTROL_4r, CPU_PROTO_MMRP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },
    { bcmSwitchCpuProtoSrpPrio, HK,
        CPU_CONTROL_4r, CPU_PROTO_SRP_PRIORITYf,
        NULL, soc_feature_cpu_proto_prio },

#endif /* BCM_HAWKEYE_SUPPORT */

};
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#define BCM_MODID_MAX   128
static uint8 _mod_type_table[BCM_LOCAL_UNITS_MAX][BCM_MODID_MAX];

STATIC int
_bcm_switch_module_type_set(int unit, bcm_module_t mod,
                                       uint32 mod_type)
{
    if (mod >= BCM_MODID_MAX) {
        return BCM_E_PARAM;
    } else {
        _mod_type_table[unit][mod] = mod_type;
    }
    return BCM_E_NONE;
}

int
_bcm_switch_module_type_get(int unit, bcm_module_t mod, uint32 *mod_type)
{
    *mod_type = BCM_SWITCH_MODULE_UNKNOWN;
    if (mod >= BCM_MODID_MAX) {
        return BCM_E_PARAM;
    } else {
        *mod_type = _mod_type_table[unit][mod];
    }
    return BCM_E_NONE;
}
#undef BCM_MODID_MAX

#define CLEARMEM(_unit, _mem) \
	if (SOC_MEM_IS_VALID(_unit, _mem) && SOC_MEM_SIZE(_unit, _mem) > 0) {\
	    SOC_IF_ERROR_RETURN(soc_mem_clear(_unit, _mem, MEM_BLOCK_ALL, 0));\
	}
/*
 * Bulk clear of various hit bit tables
 */
STATIC int
_bcm_esw_switch_hit_clear_set(int unit, bcm_switch_control_t type, int arg)
{
    if (arg != 1) {
	return BCM_E_PARAM;
    }

    switch (type) {
    case bcmSwitchL2HitClear:
	if (SOC_IS_DRACO(unit)) {
	    CLEARMEM(unit, L2X_HITm);
	    return BCM_E_NONE;
	}
	if (SOC_IS_FBX(unit)) {
	    CLEARMEM(unit, L2_HITSA_ONLYm);
	    CLEARMEM(unit, L2_HITDA_ONLYm);
	    if (soc_feature(unit, soc_feature_esm_support)) {
		CLEARMEM(unit, EXT_SRC_HIT_BITS_L2m);
		CLEARMEM(unit, EXT_DST_HIT_BITS_L2m);
	    }
	    return BCM_E_NONE;
	}
	break;
    case bcmSwitchL2SrcHitClear:
	if (SOC_IS_FBX(unit)) {
	    CLEARMEM(unit, L2_HITSA_ONLYm);
	    if (soc_feature(unit, soc_feature_esm_support)) {
		CLEARMEM(unit, EXT_SRC_HIT_BITS_L2m);
	    }
	    return BCM_E_NONE;
	}
	break;
    case bcmSwitchL2DstHitClear:
	if (SOC_IS_DRACO(unit)) {
	    CLEARMEM(unit, L2X_HITm);
	    return BCM_E_NONE;
	}
	if (SOC_IS_FBX(unit)) {
	    CLEARMEM(unit, L2_HITDA_ONLYm);
	    if (soc_feature(unit, soc_feature_esm_support)) {
		CLEARMEM(unit, EXT_DST_HIT_BITS_L2m);
	    }
	    return BCM_E_NONE;
	}
	break;
    case bcmSwitchL3HostHitClear:
	if (!soc_feature(unit, soc_feature_l3) ||
	    soc_feature(unit, soc_feature_fp_based_routing)) {
	    break;
	}
	if (SOC_IS_DRACO(unit)) {
	    CLEARMEM(unit, L3X_HITm);
	    return BCM_E_NONE;
	}
	if (SOC_IS_FBX(unit)) {
	    CLEARMEM(unit, L3_ENTRY_HIT_ONLYm);
	    return BCM_E_NONE;
	}
	break;
    case bcmSwitchL3RouteHitClear:
	if (!soc_feature(unit, soc_feature_l3) ||
	    soc_feature(unit, soc_feature_fp_based_routing)) {
	    break;
	}
	if (SOC_IS_DRACO(unit)) {
	    CLEARMEM(unit, DEFIP_HIT_HIm);
	    CLEARMEM(unit, DEFIP_HIT_LOm);
	    return BCM_E_NONE;
	}
	if (SOC_IS_FBX(unit)) {
	    CLEARMEM(unit, L3_DEFIP_HIT_ONLYm);
	    CLEARMEM(unit, L3_DEFIP_128_HIT_ONLYm);
	    if (soc_feature(unit, soc_feature_esm_support)) {
		CLEARMEM(unit, EXT_SRC_HIT_BITS_IPV4m);
		CLEARMEM(unit, EXT_SRC_HIT_BITS_IPV6_64m);
		CLEARMEM(unit, EXT_SRC_HIT_BITS_IPV6_128m);
		CLEARMEM(unit, EXT_DST_HIT_BITS_IPV4m);
		CLEARMEM(unit, EXT_DST_HIT_BITS_IPV6_64m);
		CLEARMEM(unit, EXT_DST_HIT_BITS_IPV6_128m);
	    }
	    return BCM_E_NONE;
	}
	break;
    default:
	break;
    }

    return BCM_E_UNAVAIL;
}

#undef CLEARMEM

/*
 * Return an error if a hit clear is not supportable on this device
 */
STATIC int
_bcm_esw_switch_hit_clear_get(int unit, bcm_switch_control_t type, int *arg)
{
    int		rv;

    rv = BCM_E_UNAVAIL;

    switch (type) {
    case bcmSwitchL2HitClear:
	if (SOC_IS_DRACO(unit)) {
	    rv = BCM_E_NONE;
	    break;
	}
	if (SOC_IS_FBX(unit)) {
	    rv = BCM_E_NONE;
	    break;
	}
	break;
    case bcmSwitchL2SrcHitClear:
	if (SOC_IS_FBX(unit)) {
	    rv = BCM_E_NONE;
	    break;
	}
	break;
    case bcmSwitchL2DstHitClear:
	if (SOC_IS_DRACO(unit)) {
	    rv = BCM_E_NONE;
	    break;
	}
	if (SOC_IS_FBX(unit)) {
	    rv = BCM_E_NONE;
	    break;
	}
	break;
    case bcmSwitchL3HostHitClear:
	if (!soc_feature(unit, soc_feature_l3) ||
	    soc_feature(unit, soc_feature_fp_based_routing)) {
	    break;
	}
	if (SOC_IS_DRACO(unit)) {
	    rv = BCM_E_NONE;
	    break;
	}
	if (SOC_IS_FBX(unit)) {
	    rv = BCM_E_NONE;
	    break;
	}
	break;
    case bcmSwitchL3RouteHitClear:
	if (!soc_feature(unit, soc_feature_l3) ||
	    soc_feature(unit, soc_feature_fp_based_routing)) {
	    break;
	}
	if (SOC_IS_DRACO(unit)) {
	    rv = BCM_E_NONE;
	    break;
	}
	if (SOC_IS_FBX(unit)) {
	    rv = BCM_E_NONE;
	    break;
	}
	break;
    default:
	break;
    }
    if (rv == BCM_E_NONE && arg != NULL) {
	*arg = 0;
    }
    return rv;
}

/*
 * Function:
 *	bcm_switch_control_port_set
 * Description:
 *	Specify general switch behaviors on a per-port basis.
 * Parameters:
 *	unit - Device unit number
 *	port - Port to affect
 *      type - The desired configuration parameter to modify
 *      arg - The value with which to set the parameter
 * Returns:
 *      BCM_E_xxx
 */

int
bcm_esw_switch_control_port_set(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int arg)
{
    if ((type == bcmSwitchFailoverStackTrunk) ||
        (type == bcmSwitchFailoverEtherTrunk)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_switch_control_gport_resolve(unit, port, &port));
    }

#ifdef BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        uint64 cpu_control;
        soc_reg_t reg;
        soc_field_t field = INVALIDf;

        COMPILER_64_ZERO(cpu_control);

        if ((type == bcmSwitchLLCSAP1) || (type == bcmSwitchLLCSAP2)) {
            return _bcm_llcmatch_set(unit, port, type, arg);
        }

        if ((type == bcmSwitchHashL2) || (type == bcmSwitchHashL3) ||
            (type == bcmSwitchHashMultipath)) {
            return _bcm_hashselect_set(unit, type, arg);
        }

        if (type == bcmSwitchHashControl) {
            return _bcm_hashcontrol_set(unit, arg);
        }

        if (type == bcmSwitchPktAge) {
            return _bcm_pkt_age_set(unit, arg);
        }

        if (type == bcmSwitchIpmcReplicationSharing) {
            if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
                SOC_IPMCREPLSHR_SET(unit, arg);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
        }
#if defined(INCLUDE_L3)
        if ((type == bcmSwitchUnknownL3DestToCpu) && (SOC_IS_DRACO15(unit))) {
            return _bcm_draco_l3_dlf_action_set(unit, arg);
        }
#endif /* INCLUDE_L3 */

        BCM_IF_ERROR_RETURN(_bcm_sw_cc_get(unit, port, &cpu_control));

        reg = IS_E_PORT(unit, port) ? CPU_CONTROLr : ICPU_CONTROLr;

        /* Clean up arg for boolean fields */
        switch(type) {
        case bcmSwitchCpuProtocolPrio:
        case bcmSwitchCpuUnknownPrio:
        case bcmSwitchCpuDefaultPrio:
            break;
        default:  /* All others, force arg to 0 or 1 */
            arg = arg ? 1 : 0;
            break;
        }

        field = _bcm_sw_field_get(unit, port, type);
        if ((field == INVALIDf) || 
            (0 == SOC_REG_FIELD_VALID(unit, reg, field))) {
            return BCM_E_UNAVAIL;
        }

        soc_reg64_field32_set(unit, reg, &cpu_control, field, arg);
        BCM_IF_ERROR_RETURN(_bcm_sw_cc_set(unit, port, cpu_control));

        return BCM_E_NONE;
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint64 oval64, val64;
        int i, found;

        switch(type) {
        case bcmSwitchPktAge:
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                return _bcm_ts_pkt_age_set(unit, arg);
            } else
#endif
            if (SOC_IS_FBX(unit)) {
                return _bcm_fb_pkt_age_set(unit, arg);
            } else if (SOC_IS_EASYRIDER(unit)) {
                return _bcm_er_pkt_age_set(unit, arg);
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchHashL2:
        case bcmSwitchHashL3:
        case bcmSwitchHashMultipath:
        case bcmSwitchHashIpfixIngress:
        case bcmSwitchHashIpfixEgress:
        case bcmSwitchHashIpfixIngressDual:
        case bcmSwitchHashIpfixEgressDual:
        case bcmSwitchHashVlanTranslate:
            return _bcm_fb_er_hashselect_set(unit, type, arg);
        case bcmSwitchHashControl:
            return _bcm_xgs3_hashcontrol_set(unit, arg);
#if defined(BCM_TRIUMPH_SUPPORT)
        case bcmSwitchFieldMultipathHashSelect:
            return _bcm_field_tr_multipath_hashcontrol_set(unit, arg);
#endif  /* BCM_TRIUMPH_SUPPORT */
        case bcmSwitchColorSelect:
            return _bcm_fb_er_color_mode_set(unit, port, arg);
        case bcmSwitchModuleLoopback:
            return _bcm_fb_mod_lb_set(unit, port, arg);
        case bcmSwitchMplsIngressPortCheck:
            return bcm_esw_port_control_set(unit, port,
                                            bcmPortControlMplsIngressPortCheck,
                                            arg);
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
        case bcmSwitchIngressRateLimitIncludeIFG:
            return _bcm_xgs3_ing_rate_limit_ifg_set(unit, port, arg);
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT */
        case bcmSwitchVrfMax:
            return soc_max_vrf_set(unit, arg);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        case bcmSwitchL3UrpfRouteEnable:
            if (soc_feature(unit, soc_feature_urpf)) {
                return _bcm_xgs3_urpf_route_enable(unit, arg);
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchL3UrpfMode:
            if (soc_feature(unit, soc_feature_urpf)) {
                return _bcm_xgs3_urpf_port_mode_set(unit, port, arg);
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchL3UrpfDefaultRoute:
            if (soc_feature(unit, soc_feature_urpf)) {
                return _bcm_xgs3_urpf_def_gw_enable(unit, port, arg);
            }
            return BCM_E_UNAVAIL;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
        case bcmSwitchHashL2Dual:
        case bcmSwitchHashL3Dual:
        case bcmSwitchHashVlanTranslateDual:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                return _bcm_fb_er_hashselect_set(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
        case bcmSwitchHashSelectControl:
            if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                return _bcm_xgs3_selectcontrol_set(unit, arg);
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchHashIP4Field0:
        case bcmSwitchHashIP4Field1:
        case bcmSwitchHashIP6Field0:
        case bcmSwitchHashIP6Field1:
        case bcmSwitchHashL2Field0:
        case bcmSwitchHashL2Field1:
        case bcmSwitchHashMPLSField0:
        case bcmSwitchHashMPLSField1:
        case bcmSwitchHashHG2UnknownField0:
        case bcmSwitchHashHG2UnknownField1:
	    if ((type == bcmSwitchHashMPLSField0 || type == bcmSwitchHashMPLSField1)
                && (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH2(unit)
                 || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit))) {
                return BCM_E_UNAVAIL;
            }
            if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                return _bcm_xgs3_fieldselect_set(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
        case bcmSwitchHashL2MPLSField0:
        case bcmSwitchHashL2MPLSField1:
        case bcmSwitchHashL3MPLSField0:
        case bcmSwitchHashL3MPLSField1:
        case bcmSwitchHashMPLSTunnelField0:
        case bcmSwitchHashMPLSTunnelField1:
        case bcmSwitchHashMIMTunnelField0:
        case bcmSwitchHashMIMTunnelField1:
        case bcmSwitchHashMIMField0:
        case bcmSwitchHashMIMField1:
            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
                SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
                return _bcm_xgs3_fieldselect_set(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
#endif
        case bcmSwitchHashField0Config:
        case bcmSwitchHashField1Config:
            if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                return _bcm_xgs3_fieldconfig_set(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchTrunkHashSet0UnicastOffset:
        case bcmSwitchTrunkHashSet1UnicastOffset:
        case bcmSwitchTrunkHashSet0NonUnicastOffset:
        case bcmSwitchTrunkHashSet1NonUnicastOffset:
        case bcmSwitchTrunkFailoverHashOffset:
        case bcmSwitchFabricTrunkHashSet0UnicastOffset:
        case bcmSwitchFabricTrunkHashSet1UnicastOffset:
        case bcmSwitchFabricTrunkHashSet0NonUnicastOffset:
        case bcmSwitchFabricTrunkHashSet1NonUnicastOffset:
        case bcmSwitchFabricTrunkFailoverHashOffset:
        case bcmSwitchLoadBalanceHashSet0UnicastOffset:
        case bcmSwitchLoadBalanceHashSet1UnicastOffset:
        case bcmSwitchLoadBalanceHashSet0NonUnicastOffset:
        case bcmSwitchLoadBalanceHashSet1NonUnicastOffset:
        case bcmSwitchECMPHashSet0Offset:
        case bcmSwitchECMPHashSet1Offset:
            if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                return _bcm_xgs3_fieldoffset_set(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRIUMPH_SUPPORT */
        case bcmSwitchIgmpPktToCpu:
        case bcmSwitchIgmpPktDrop:
        case bcmSwitchMldPktToCpu:
        case bcmSwitchMldPktDrop:
        case bcmSwitchV4ResvdMcPktToCpu:
        case bcmSwitchV4ResvdMcPktFlood:
        case bcmSwitchV4ResvdMcPktDrop:
        case bcmSwitchV6ResvdMcPktToCpu:
        case bcmSwitchV6ResvdMcPktFlood:
        case bcmSwitchV6ResvdMcPktDrop:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
            defined (BCM_TRX_SUPPORT)
        case bcmSwitchIgmpReportLeaveDrop:
        case bcmSwitchIgmpReportLeaveFlood:
        case bcmSwitchIgmpReportLeaveToCpu:
        case bcmSwitchIgmpQueryDrop:
        case bcmSwitchIgmpQueryFlood:
        case bcmSwitchIgmpQueryToCpu:
        case bcmSwitchIgmpUnknownDrop:
        case bcmSwitchIgmpUnknownFlood:
        case bcmSwitchIgmpUnknownToCpu:
        case bcmSwitchMldReportDoneDrop:
        case bcmSwitchMldReportDoneFlood:
        case bcmSwitchMldReportDoneToCpu:
        case bcmSwitchMldQueryDrop:
        case bcmSwitchMldQueryFlood:
        case bcmSwitchMldQueryToCpu:
        case bcmSwitchIpmcV4RouterDiscoveryDrop:
        case bcmSwitchIpmcV4RouterDiscoveryFlood:
        case bcmSwitchIpmcV4RouterDiscoveryToCpu:
        case bcmSwitchIpmcV6RouterDiscoveryDrop:
        case bcmSwitchIpmcV6RouterDiscoveryFlood:
        case bcmSwitchIpmcV6RouterDiscoveryToCpu:
#endif /* defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) ||
          defined(BCM_TRX_SUPPORT) */
            return _bcm_xgs3_igmp_action_set(unit, port, type, arg);

        case bcmSwitchDirectedMirroring:
            if (!soc_feature(unit, soc_feature_xgs1_mirror)) {
                /* Directed mirroring cannot be turned off */
                return (arg == 0) ? BCM_E_PARAM : BCM_E_NONE;
            }
            break;
        case bcmSwitchL3EgressMode:
#ifdef INCLUDE_L3
            if (soc_feature(unit, soc_feature_l3)) {
                return bcm_xgs3_l3_egress_mode_set(unit, arg);
            }
#endif /* INCLUDE_L3 */
            return BCM_E_UNAVAIL;
        case bcmSwitchIpmcReplicationSharing:
#ifdef INCLUDE_L3
            if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
                SOC_IPMCREPLSHR_SET(unit, arg);
                return BCM_E_NONE;
            }
#endif /* INCLUDE_L3 */
            return BCM_E_UNAVAIL;
        case bcmSwitchHashDualMoveDepth:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                if (arg >= 0) {
                    SOC_DUAL_HASH_MOVE_MAX(unit) = arg;
                    return BCM_E_NONE;
                } else {
                    return BCM_E_PARAM;
                }
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchL2PortBlocking:
            if (soc_feature(unit, soc_feature_src_mac_group)) {
                SOC_L2X_GROUP_ENABLE_SET(unit, _bool_invert(unit, arg, 0));
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
#ifdef VENDOR_XYZ
        case bcmSwitchL2SrcDropOnPortMatchOnly:
            return _bcm_fb_l2_src_drop_set(unit, arg);
#endif
#if defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT)
        case bcmSwitchTimeSyncPktDrop:
        case bcmSwitchTimeSyncPktFlood:
        case bcmSwitchTimeSyncPktToCpu:
        case bcmSwitchMmrpPktDrop:
        case bcmSwitchMmrpPktFlood:
        case bcmSwitchMmrpPktToCpu:
        case bcmSwitchSrpPktDrop:
        case bcmSwitchSrpPktFlood:
        case bcmSwitchSrpPktToCpu:
            return _bcm_xgs3_eav_action_set(unit, port, type, arg);
        case bcmSwitchSRPEthertype:
        case bcmSwitchMMRPEthertype:
        case bcmSwitchTimeSyncEthertype: 
            return _bcm_xgs3_switch_ethertype_set(unit, port, type, arg); 
        case bcmSwitchSRPDestMacOui:
        case bcmSwitchMMRPDestMacOui:
        case bcmSwitchTimeSyncDestMacOui:
            return _bcm_xgs3_switch_mac_hi_set(unit, port, type, arg); 
        case bcmSwitchSRPDestMacNonOui:
        case bcmSwitchMMRPDestMacNonOui:
        case bcmSwitchTimeSyncDestMacNonOui:
            return _bcm_xgs3_switch_mac_lo_set(unit, port, type, arg); 
        case bcmSwitchTimeSyncMessageTypeBitmap:
            if (soc_reg_field_valid(unit, TS_CONTROLr, TS_MSG_BITMAPf)) {
                return soc_reg_field32_modify(unit, TS_CONTROLr, port, 
                                              TS_MSG_BITMAPf, arg);
            } else {
                return BCM_E_UNAVAIL;
            }

#endif /* defined(BCM_HAWKEYE_SUPPORT)*/
#if defined(BCM_TRIUMPH2_SUPPORT)
        case bcmSwitchUnknownVlanToCpu:
        case bcmSwitchL3HeaderErrToCpu:
        case bcmSwitchIpmcTtlErrToCpu:
        case bcmSwitchHgHdrErrToCpu:
        case bcmSwitchTunnelErrToCpu:
            if (SOC_REG_IS_VALID(unit, EGR_CPU_CONTROLr)) {
                BCM_IF_ERROR_RETURN(
                    _bcm_tr2_ep_redirect_action_set(unit, port, type, arg));
            }
            break;
        case bcmSwitchStgInvalidToCpu:
        case bcmSwitchVlanTranslateEgressMissToCpu:
        case bcmSwitchL3PktErrToCpu:
        case bcmSwitchMtuFailureToCpu:
        case bcmSwitchSrcKnockoutToCpu:
        case bcmSwitchWlanTunnelMismatchToCpu:
        case bcmSwitchWlanTunnelMismatchDrop:
        case bcmSwitchWlanPortMissToCpu:
            return _bcm_tr2_ep_redirect_action_set(unit, port, type, arg);
        case bcmSwitchUnknownVlanToCpuCosq:
        case bcmSwitchStgInvalidToCpuCosq:
        case bcmSwitchVlanTranslateEgressMissToCpuCosq:
        case bcmSwitchTunnelErrToCpuCosq:
        case bcmSwitchL3HeaderErrToCpuCosq:
        case bcmSwitchL3PktErrToCpuCosq:
        case bcmSwitchIpmcTtlErrToCpuCosq:
        case bcmSwitchMtuFailureToCpuCosq:
        case bcmSwitchHgHdrErrToCpuCosq:
        case bcmSwitchSrcKnockoutToCpuCosq:
        case bcmSwitchWlanTunnelMismatchToCpuCosq:
        case bcmSwitchWlanPortMissToCpuCosq:
            return _bcm_tr2_ep_redirect_action_cosq_set(unit, port, type, arg);
        case bcmSwitchLayeredQoSResolution:
            return _bcm_tr2_layered_qos_resolution_set(unit, port, type, arg);
        case bcmSwitchEncapErrorToCpu:
            return _bcm_tr2_ehg_error2cpu_set(unit, port, arg);
        case bcmSwitchMirrorEgressTrueColorSelect:
        case bcmSwitchMirrorEgressTruePriority:
            return _bcm_tr2_mirror_egress_true_set(unit, port, type, arg);

#endif /* BCM_TRIUMPH2_SUPPORT */

        default:
            break;
        }

        found = 0;		/* True if one or more matches found */

        for (i = 0; i < COUNTOF(xgs3_bindings); i++) {
            bcm_switch_binding_t *b = &xgs3_bindings[i];
            uint32 addr, max;
            int fval, fwidth;

            if (b->type == type && (SOC_INFO(unit).chip & b->chip) != 0) {
                if (b->feature && !soc_feature(unit, b->feature)) {
                    continue;
                }

                if (!SOC_REG_IS_VALID(unit, b->reg)) {
                    continue;
                }

                if (!soc_reg_field_valid(unit, b->reg, b->field)) {
                    continue;
                }

                if (b->xlate_arg) {
                    if ((fval = (b->xlate_arg)(unit, arg, 1)) < 0) {
                        return fval;
                    }
                } else {
                    fval = arg;
                }

                if (SOC_REG_INFO(unit, b->reg).regtype == soc_portreg) {
                    addr = soc_reg_addr(unit, b->reg, port, 0);
                } else {
                    addr = soc_reg_addr(unit, b->reg, REG_PORT_ANY, 0);
                }

                fwidth = soc_reg_field_length(unit, b->reg, b->field);
                if (fwidth < 32) {
                    max = (1 << fwidth) - 1;
                } else {
                    max = -1;
                }
                SOC_IF_ERROR_RETURN(soc_reg_read(unit, b->reg, addr, &val64));
                oval64 = val64;
                soc_reg64_field32_set(unit, b->reg, &val64, b->field,
                                         (fval < 0) ? 0 :  ((uint32)fval > max) ?
                                         max : (uint32)fval);
                if (COMPILER_64_NE(val64, oval64)) {
                    SOC_IF_ERROR_RETURN(soc_reg_write(unit, b->reg, addr, val64));
                }

                found = 1;
            }
        }

        return (found ? BCM_E_NONE : BCM_E_UNAVAIL);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_switch_control_port_get
 * Description:
 *	Retrieve general switch behaviors on a per-port basis
 * Parameters:
 *	unit - Device unit number
 *	port - Port to check
 *      type - The desired configuration parameter to retrieve
 *      arg - Pointer to where the retrieved value will be written
 * Returns:
 *      BCM_E_xxx
 */

int
bcm_esw_switch_control_port_get(int unit,
                                bcm_port_t port,
                                bcm_switch_control_t type,
                                int *arg)
{
    if ((type == bcmSwitchFailoverStackTrunk) ||
        (type == bcmSwitchFailoverEtherTrunk)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_switch_control_gport_resolve(unit, port, &port));
    }

#ifdef BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
        uint64 cpu_control;
        soc_reg_t reg;
        soc_field_t field = INVALIDf;

        COMPILER_64_ZERO(cpu_control);

        if ((type == bcmSwitchLLCSAP1) || (type == bcmSwitchLLCSAP2)) {
            return _bcm_llcmatch_get(unit, port, type, arg);
        }

        if ((type == bcmSwitchHashL2) || (type == bcmSwitchHashL3) ||
            (type == bcmSwitchHashMultipath)) {
            return _bcm_hashselect_get(unit, type, arg);
        }

        if (type == bcmSwitchHashControl) {
            return _bcm_hashcontrol_get(unit, arg);
        }

        if (type == bcmSwitchPktAge) {
            return _bcm_pkt_age_get(unit, arg);
        }
        if (type == bcmSwitchIpmcReplicationSharing) {
            if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
                *arg = SOC_IPMCREPLSHR_GET(unit);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
        }
#if defined(INCLUDE_L3)
        if ((type == bcmSwitchUnknownL3DestToCpu) && (SOC_IS_DRACO15(unit))) {
            return _bcm_draco_l3_dlf_action_get(unit, arg);
        }
#endif /* INCLUDE_L3 */

        reg = IS_E_PORT(unit, port) ? CPU_CONTROLr : ICPU_CONTROLr;

        field = _bcm_sw_field_get(unit, port, type);
        if ((field == INVALIDf) || 
            (0 == SOC_REG_FIELD_VALID(unit, reg, field))) {
            return BCM_E_UNAVAIL;
        }

        BCM_IF_ERROR_RETURN(_bcm_sw_cc_get(unit, port, &cpu_control));
        /* Extract and return the designated field */
        *arg = soc_reg64_field32_get(unit, reg, cpu_control, field);

        return BCM_E_NONE;
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        uint64 val64;
        int i;

        switch(type) {
        case bcmSwitchPktAge:
#ifdef BCM_TRX_SUPPORT
            if (SOC_IS_TRX(unit)) {
                return _bcm_ts_pkt_age_get(unit, arg);
            } else
#endif
            if (SOC_IS_FBX(unit)) {
                return _bcm_fb_pkt_age_get(unit, arg);
            } else if (SOC_IS_EASYRIDER(unit)) {
                return _bcm_er_pkt_age_get(unit, arg);
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchHashL2:
        case bcmSwitchHashL3:
        case bcmSwitchHashMultipath:
        case bcmSwitchHashIpfixIngress:
        case bcmSwitchHashIpfixEgress:
        case bcmSwitchHashIpfixIngressDual:
        case bcmSwitchHashIpfixEgressDual:
        case bcmSwitchHashVlanTranslate:
            return _bcm_fb_er_hashselect_get(unit, type, arg);
        case bcmSwitchHashControl:
            return _bcm_xgs3_hashcontrol_get(unit, arg);
#if defined(BCM_TRIUMPH_SUPPORT)
        case bcmSwitchFieldMultipathHashSelect:
            return _bcm_field_tr_multipath_hashcontrol_get(unit, arg);
#endif  /* BCM_TRIUMPH_SUPPORT */
        case bcmSwitchColorSelect:
            return _bcm_fb_er_color_mode_get(unit, port, arg);
        case bcmSwitchModuleLoopback:
            return _bcm_fb_mod_lb_get(unit, port, arg);
        case bcmSwitchMplsIngressPortCheck:
            return bcm_esw_port_control_get(unit, port,
                                            bcmPortControlMplsIngressPortCheck,
                                            arg);
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
        case bcmSwitchIngressRateLimitIncludeIFG:
            return _bcm_xgs3_ing_rate_limit_ifg_get(unit, port, arg);
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT */
        case bcmSwitchVrfMax:
            return _bcm_vrf_max_get(unit, arg);
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
        case bcmSwitchL3UrpfMode:
            if (soc_feature(unit, soc_feature_urpf)) {
                return _bcm_esw_port_config_get(unit, port,
                                            _bcmPortL3UrpfMode, arg);
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchL3UrpfDefaultRoute:
            if (soc_feature(unit, soc_feature_urpf)) {
                return _bcm_esw_port_config_get(unit, port,
                                            _bcmPortL3UrpfDefaultRoute, arg);
            }
            return BCM_E_UNAVAIL;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) || \
        defined(BCM_TRX_SUPPORT)
        case bcmSwitchHashL2Dual:
        case bcmSwitchHashL3Dual:
        case bcmSwitchHashVlanTranslateDual:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                return _bcm_fb_er_hashselect_get(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAPTOR_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_BRADLEY_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT)
        case bcmSwitchHashSelectControl:
            if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                return _bcm_xgs3_selectcontrol_get(unit, arg);
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchHashIP4Field0:
        case bcmSwitchHashIP4Field1:
        case bcmSwitchHashIP6Field0:
        case bcmSwitchHashIP6Field1:
        case bcmSwitchHashL2Field0:
        case bcmSwitchHashL2Field1:
        case bcmSwitchHashMPLSField0:
        case bcmSwitchHashMPLSField1:
        case bcmSwitchHashHG2UnknownField0:
        case bcmSwitchHashHG2UnknownField1:
	    if ((type == bcmSwitchHashMPLSField0 || type == bcmSwitchHashMPLSField1)
                && (SOC_IS_ENDURO(unit) || SOC_IS_TRIUMPH2(unit)
                 || SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit))) {
                return BCM_E_UNAVAIL;
            }
            if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                return _bcm_xgs3_fieldselect_get(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
        case bcmSwitchHashL2MPLSField0:
        case bcmSwitchHashL2MPLSField1:
        case bcmSwitchHashL3MPLSField0:
        case bcmSwitchHashL3MPLSField1:
        case bcmSwitchHashMPLSTunnelField0:
        case bcmSwitchHashMPLSTunnelField1:
        case bcmSwitchHashMIMTunnelField0:
        case bcmSwitchHashMIMTunnelField1:
        case bcmSwitchHashMIMField0:
        case bcmSwitchHashMIMField1:
            if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
                SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) {
                return _bcm_xgs3_fieldselect_get(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
#endif
        case bcmSwitchHashField0Config:
        case bcmSwitchHashField1Config:
            if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                return _bcm_xgs3_fieldconfig_get(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchTrunkHashSet0UnicastOffset:
        case bcmSwitchTrunkHashSet1UnicastOffset:
        case bcmSwitchTrunkHashSet0NonUnicastOffset:
        case bcmSwitchTrunkHashSet1NonUnicastOffset:
        case bcmSwitchTrunkFailoverHashOffset:
        case bcmSwitchFabricTrunkHashSet0UnicastOffset:
        case bcmSwitchFabricTrunkHashSet1UnicastOffset:
        case bcmSwitchFabricTrunkHashSet0NonUnicastOffset:
        case bcmSwitchFabricTrunkHashSet1NonUnicastOffset:
        case bcmSwitchFabricTrunkFailoverHashOffset:
        case bcmSwitchLoadBalanceHashSet0UnicastOffset:
        case bcmSwitchLoadBalanceHashSet1UnicastOffset:
        case bcmSwitchLoadBalanceHashSet0NonUnicastOffset:
        case bcmSwitchLoadBalanceHashSet1NonUnicastOffset:
        case bcmSwitchECMPHashSet0Offset:
        case bcmSwitchECMPHashSet1Offset:
            if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                return _bcm_xgs3_fieldoffset_get(unit, type, arg);
            }
            return BCM_E_UNAVAIL;
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRIUMPH_SUPPORT */
        case bcmSwitchIgmpPktToCpu:
        case bcmSwitchIgmpPktDrop:
        case bcmSwitchMldPktToCpu:
        case bcmSwitchMldPktDrop:
        case bcmSwitchV4ResvdMcPktToCpu:
        case bcmSwitchV4ResvdMcPktFlood:
        case bcmSwitchV4ResvdMcPktDrop:
        case bcmSwitchV6ResvdMcPktToCpu:
        case bcmSwitchV6ResvdMcPktFlood:
        case bcmSwitchV6ResvdMcPktDrop:
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAPTOR_SUPPORT) ||\
            defined (BCM_TRX_SUPPORT)
        case bcmSwitchIgmpReportLeaveDrop:
        case bcmSwitchIgmpReportLeaveFlood:
        case bcmSwitchIgmpReportLeaveToCpu:
        case bcmSwitchIgmpQueryDrop:
        case bcmSwitchIgmpQueryFlood:
        case bcmSwitchIgmpQueryToCpu:
        case bcmSwitchIgmpUnknownDrop:
        case bcmSwitchIgmpUnknownFlood:
        case bcmSwitchIgmpUnknownToCpu:
        case bcmSwitchMldReportDoneDrop:
        case bcmSwitchMldReportDoneFlood:
        case bcmSwitchMldReportDoneToCpu:
        case bcmSwitchMldQueryDrop:
        case bcmSwitchMldQueryFlood:
        case bcmSwitchMldQueryToCpu:
        case bcmSwitchIpmcV4RouterDiscoveryDrop:
        case bcmSwitchIpmcV4RouterDiscoveryFlood:
        case bcmSwitchIpmcV4RouterDiscoveryToCpu:
        case bcmSwitchIpmcV6RouterDiscoveryDrop:
        case bcmSwitchIpmcV6RouterDiscoveryFlood:
        case bcmSwitchIpmcV6RouterDiscoveryToCpu:
#endif /* defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) ||
          defined(BCM_TRX_SUPPORT) */
            return _bcm_xgs3_igmp_action_get(unit, port, type, arg);

#if defined(BCM_RAPTOR_SUPPORT)
        case bcmSwitchDirectedMirroring:
            if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
                *arg = 1;
                return BCM_E_NONE;
            }
            break;
#endif
        case bcmSwitchL3EgressMode:
#ifdef INCLUDE_L3
            if (soc_feature(unit, soc_feature_l3)) {
                return bcm_xgs3_l3_egress_mode_get(unit, arg);
            }
#endif /* INCLUDE_L3 */
            return BCM_E_UNAVAIL;
        case bcmSwitchIpmcReplicationSharing:
#ifdef INCLUDE_L3
            if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
                *arg = SOC_IPMCREPLSHR_GET(unit);
                return BCM_E_NONE;
            }
#endif /* INCLUDE_L3 */
            return BCM_E_UNAVAIL;
        case bcmSwitchHashDualMoveDepth:
            if (soc_feature(unit, soc_feature_dual_hash)) {
                *arg = SOC_DUAL_HASH_MOVE_MAX(unit);
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
        case bcmSwitchL2PortBlocking:
            if (soc_feature(unit, soc_feature_src_mac_group)) {
                *arg = (!SOC_L2X_GROUP_ENABLE_GET(unit));
                return BCM_E_NONE;
            }
            return BCM_E_UNAVAIL;
#ifdef VENDOR_XYZ
        case bcmSwitchL2SrcDropOnPortMatchOnly:
            return _bcm_fb_l2_src_drop_get(unit, arg);
#endif
#if defined(BCM_HAWKEYE_SUPPORT)|| defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT)
        case bcmSwitchTimeSyncPktDrop:
        case bcmSwitchTimeSyncPktFlood:
        case bcmSwitchTimeSyncPktToCpu:
        case bcmSwitchMmrpPktDrop:
        case bcmSwitchMmrpPktFlood:
        case bcmSwitchMmrpPktToCpu:
        case bcmSwitchSrpPktDrop:
        case bcmSwitchSrpPktFlood:
        case bcmSwitchSrpPktToCpu:
            return _bcm_xgs3_eav_action_get(unit, port, type, arg);
        case bcmSwitchSRPEthertype:
        case bcmSwitchMMRPEthertype:
        case bcmSwitchTimeSyncEthertype: 
            return _bcm_xgs3_switch_ethertype_get(unit, port, type, arg); 
        case bcmSwitchSRPDestMacOui:
        case bcmSwitchMMRPDestMacOui:
        case bcmSwitchTimeSyncDestMacOui:
            return _bcm_xgs3_switch_mac_hi_get(unit, port, type, arg); 
        case bcmSwitchSRPDestMacNonOui:
        case bcmSwitchMMRPDestMacNonOui:
        case bcmSwitchTimeSyncDestMacNonOui:
            return _bcm_xgs3_switch_mac_lo_get(unit, port, type, arg); 
        case bcmSwitchTimeSyncMessageTypeBitmap:
            if (soc_reg_field_valid(unit, TS_CONTROLr, TS_MSG_BITMAPf)) {
                uint32 rval; 
                SOC_IF_ERROR_RETURN(READ_TS_CONTROLr(unit, &rval)); 
                *arg = soc_reg_field_get(unit, TS_CONTROLr, rval, 
                                         TS_MSG_BITMAPf);
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }

#endif /* defined(BCM_HAWKEYE_SUPPORT)*/
#if defined(BCM_TRIUMPH2_SUPPORT)
        case bcmSwitchUnknownVlanToCpu:
        case bcmSwitchL3HeaderErrToCpu:
        case bcmSwitchIpmcTtlErrToCpu:
        case bcmSwitchHgHdrErrToCpu:
        case bcmSwitchTunnelErrToCpu:
             if (SOC_REG_IS_VALID(unit, EGR_CPU_CONTROLr)) {
                 return _bcm_tr2_ep_redirect_action_get(unit, port, type, arg);
             }
             break;
        case bcmSwitchStgInvalidToCpu:
        case bcmSwitchVlanTranslateEgressMissToCpu:
        case bcmSwitchL3PktErrToCpu:
        case bcmSwitchMtuFailureToCpu:
        case bcmSwitchSrcKnockoutToCpu:
        case bcmSwitchWlanTunnelMismatchToCpu:
        case bcmSwitchWlanTunnelMismatchDrop:
        case bcmSwitchWlanPortMissToCpu:
            return _bcm_tr2_ep_redirect_action_get(unit, port, type, arg);
        case bcmSwitchUnknownVlanToCpuCosq:
        case bcmSwitchStgInvalidToCpuCosq:
        case bcmSwitchVlanTranslateEgressMissToCpuCosq:
        case bcmSwitchTunnelErrToCpuCosq:
        case bcmSwitchL3HeaderErrToCpuCosq:
        case bcmSwitchL3PktErrToCpuCosq:
        case bcmSwitchIpmcTtlErrToCpuCosq:
        case bcmSwitchMtuFailureToCpuCosq:
        case bcmSwitchHgHdrErrToCpuCosq:
        case bcmSwitchSrcKnockoutToCpuCosq:
        case bcmSwitchWlanTunnelMismatchToCpuCosq:
        case bcmSwitchWlanPortMissToCpuCosq:
            return _bcm_tr2_ep_redirect_action_cosq_get(unit, port, type, arg);
        case bcmSwitchLayeredQoSResolution:
            return _bcm_tr2_layered_qos_resolution_get(unit, port, type, arg);
        case bcmSwitchEncapErrorToCpu:
            return _bcm_tr2_ehg_error2cpu_get(unit, port, arg);
        case bcmSwitchMirrorEgressTrueColorSelect:
        case bcmSwitchMirrorEgressTruePriority:
            return _bcm_tr2_mirror_egress_true_get(unit, port, type, arg);
#endif /* BCM_TRIUMPH2_SUPPORT */

        default :
            break;
        }

        for (i = 0; i < COUNTOF(xgs3_bindings); i++) {
            bcm_switch_binding_t *b = &xgs3_bindings[i];
            uint32 addr;

            if (b->type == type && (SOC_INFO(unit).chip & b->chip) != 0) {
                if (b->feature && !soc_feature(unit, b->feature)) {
                    continue;
                }
                if (!SOC_REG_IS_VALID(unit, b->reg)) {
                    continue;
                }

                if (!soc_reg_field_valid(unit, b->reg, b->field)) {
                    continue;
                }
                if (SOC_REG_INFO(unit, b->reg).regtype == soc_portreg) {
                    addr = soc_reg_addr(unit, b->reg, port, 0);
                } else {
                    addr = soc_reg_addr(unit, b->reg, REG_PORT_ANY, 0);
                }

                SOC_IF_ERROR_RETURN(soc_reg_read(unit, b->reg, addr, &val64));
                *arg = soc_reg64_field32_get(unit, b->reg, val64, b->field);

                if (b->xlate_arg) {
                    *arg = (b->xlate_arg)(unit, *arg, 0);
                }
                return BCM_E_NONE;
            }
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_switch_control_set
 * Description:
 *	Specify general switch behaviors.
 * Parameters:
 *	unit - Device unit number
 *      type - The desired configuration parameter to modify
 *      arg - The value with which to set the parameter
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      For behaviors which are port-specific, all non-stack ports
 *	will be set.
 */

int
bcm_esw_switch_control_set(int unit,
                           bcm_switch_control_t type,
                           int arg)
{
    bcm_port_t		port;
    int			rv, found;

    /* Check for device wide, non port specific controls */
    switch (type) {
    case bcmSwitchWarmBoot:
        /* If true, set the Warm Boot state; clear otherwise */
        if (arg) {
#ifdef BCM_WARM_BOOT_SUPPORT
            SOC_WARM_BOOT_START(unit);
#else
            return SOC_E_UNAVAIL;
#endif
        } else {
#ifdef BCM_WARM_BOOT_SUPPORT
            SOC_WARM_BOOT_DONE(unit);
#else
            return SOC_E_UNAVAIL;
#endif
        }
        return BCM_E_NONE;
	break;

    case bcmSwitchModuleType:
        return(_bcm_switch_module_type_set(unit,
                                           BCM_SWITCH_CONTROL_MODID_UNPACK(arg),
                                           BCM_SWITCH_CONTROL_MODTYPE_UNPACK(arg)));
	break;

    case bcmSwitchIpmcGroupMtu:
        if (soc_feature(unit, soc_feature_ipmc_group_mtu)) {
            uint32 val;

            if (arg <= 0 || arg > 0x3fff) {
                return BCM_E_PARAM;
            }

            rv = READ_IPMC_L3_MTUr(unit, 0, &val);
            if (rv >= 0) {
                soc_reg_field_set(unit, IPMC_L3_MTUr, &val, MTU_LENf, arg);
                rv = WRITE_IPMC_L3_MTUr(unit, 0, val);
            }
            return rv;
        }
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchMcastFloodDefault:
        return(_bcm_esw_vlan_flood_default_set(unit, arg));
	break;

    case bcmSwitchUseGport:
        SOC_USE_GPORT_SET(unit, arg);
        return BCM_E_NONE;
	break;

    case bcmSwitchHgHdrMcastFlood:
        return(_bcm_esw_higig_flood_l2_set(unit, arg));
	break;

    case bcmSwitchHgHdrIpMcastFlood:
        return(_bcm_esw_higig_flood_l3_set(unit, arg));
	break;

    case bcmSwitchExternalL3:
#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit) && (arg != 0)) {
            return(soc_easyrider_external_l3_enable(unit));
        }
#endif /* BCM_EASYRIDER_SUPPORT */
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchFailoverStackTrunk:
    case bcmSwitchFailoverEtherTrunk:
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchMcastL2Range:
#ifdef BCM_BRADLEY_SUPPORT
        if (SOC_IS_HBX(unit) || SOC_IS_SC_CQ(unit)) {
            return(soc_hbx_mcast_size_set(unit, arg));
        }
#endif
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchMcastL3Range:
#ifdef BCM_BRADLEY_SUPPORT
        if (SOC_IS_HBX(unit) || SOC_IS_SC_CQ(unit)) {
            return(soc_hbx_ipmc_size_set(unit, arg));
        }
#endif
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchHgHdrMcastVlanRange:
    case bcmSwitchHgHdrMcastL2Range:
    case bcmSwitchHgHdrMcastL3Range:
#ifdef BCM_BRADLEY_SUPPORT
        if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            int bc_size, mc_size, ipmc_size;
            BCM_IF_ERROR_RETURN
                (soc_hbx_higig2_mcast_sizes_get(unit, &bc_size,
                                                &mc_size, &ipmc_size));
            switch (type) {
            case bcmSwitchHgHdrMcastVlanRange:
                bc_size = arg;
                break;
            case bcmSwitchHgHdrMcastL2Range:
                mc_size = arg;
                break;
            case bcmSwitchHgHdrMcastL3Range:
                ipmc_size = arg;
                break;
            default:
                return BCM_E_PARAM;
            }
            return soc_hbx_higig2_mcast_sizes_set(unit, bc_size,
                                                  mc_size, ipmc_size);
        }
#endif
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchL2McastAllRouterPortsAddEnable:
        SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit) = arg ? 1 : 0;
        return BCM_E_NONE;
	break;

    case bcmSwitchPFCClass0Queue:
    case bcmSwitchPFCClass1Queue:
    case bcmSwitchPFCClass2Queue:
    case bcmSwitchPFCClass3Queue:
    case bcmSwitchPFCClass4Queue:
    case bcmSwitchPFCClass5Queue:
    case bcmSwitchPFCClass6Queue:
    case bcmSwitchPFCClass7Queue:
#if defined(BCM_SCORPION_SUPPORT)
        return _bcm_sc_pfc_priority_to_cos_set(unit, type, arg);
#endif /* BCM_SCORPION_SUPPORT */
	return BCM_E_UNAVAIL;
	break;

    case bcmSwitchL2HitClear:
    case bcmSwitchL2SrcHitClear:
    case bcmSwitchL2DstHitClear:
    case bcmSwitchL3HostHitClear:
    case bcmSwitchL3RouteHitClear:
	return _bcm_esw_switch_hit_clear_set(unit, type, arg);
	break;

    /* This swithc control should be supported only per port. */
    case bcmSwitchEncapErrorToCpu:
        return BCM_E_UNAVAIL;

    case bcmSwitchSnapNonZeroOui:
#if defined(BCM_TRX_SUPPORT)
        if (SOC_IS_TRX(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ING_CONFIG_64r, REG_PORT_ANY,
                                        SNAP_OTHER_DECODE_ENABLEf,
                                        arg ? 1 : 0));
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, EGR_CONFIGr, REG_PORT_ANY,
                                        SNAP_OTHER_DECODE_ENABLEf,
                                        arg ? 1 : 0));
            return BCM_E_NONE;
        } else
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT)
        if (SOC_IS_FB(unit) || SOC_IS_FX_HX(unit) || SOC_IS_HB_GW(unit)) {
            SOC_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ING_CONFIGr, REG_PORT_ANY,
                                        SNAP_OTHER_DECODE_ENABLEf,
                                        arg ? 1 : 0));
            if (SOC_IS_FIREBOLT2(unit)) {
                SOC_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, EGR_CONFIGr, REG_PORT_ANY,
                                            SNAP_OTHER_DECODE_ENABLEf,
                                            arg ? 1 : 0));
            }
            return BCM_E_NONE;
        }
#endif /* BCM_FIREBOLT_SUPPORT */

        return BCM_E_UNAVAIL;

    default:
	break;
    }

    /* Iterate over all ports for port specific controls */
    found = 0;

    PBMP_E_ITER(unit, port) {
        if (SOC_IS_STACK_PORT(unit, port)) {
            continue;
        }
        rv = bcm_esw_switch_control_port_set(unit, port, type, arg);
        if (rv == BCM_E_NONE) {
	    found = 1;
	} else if (rv != BCM_E_UNAVAIL) {
            return rv;
        }
    }

    PBMP_HG_ITER(unit, port) {
        if (SOC_IS_STACK_PORT(unit, port) &&
            (type != bcmSwitchDirectedMirroring)) {
            continue;
        }
        rv = bcm_esw_switch_control_port_set(unit, port, type, arg);
	if (rv == BCM_E_NONE) {
	    found = 1;
	} else if (rv != BCM_E_UNAVAIL) {
            return rv;
        }
    }

    return (found ? BCM_E_NONE : BCM_E_UNAVAIL);
}

/*
 * Function:
 *	bcm_switch_control_get
 * Description:
 *	Retrieve general switch behaviors.
 * Parameters:
 *	unit - Device unit number
 *      type - The desired configuration parameter to retrieve.
 *      arg - Pointer to where the retrieved value will be written.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      For behaviors which are port-specific, retrieves the setting
 *	for one port arbitrarily chosen from those which support the
 *	setting and are non-stack.
 */

int
bcm_esw_switch_control_get(int unit,
                           bcm_switch_control_t type,
                           int *arg)
{
    bcm_port_t		port;
    int 		rv;

    /* Check for device wide, non port specific controls */
    switch (type) {
    case bcmSwitchModuleType: {
        bcm_module_t modid;
        uint32 mod_type;

        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));
        BCM_IF_ERROR_RETURN(_bcm_switch_module_type_get(unit, modid,
                                                           &mod_type));
        *arg = BCM_SWITCH_CONTROL_MOD_TYPE_PACK(modid, mod_type);
        return BCM_E_NONE;
    }
	break;

    case bcmSwitchIpmcGroupMtu:
        if (soc_feature(unit, soc_feature_ipmc_group_mtu)) {
            uint32 val;

            rv = READ_IPMC_L3_MTUr(unit, 0, &val);
            if (rv >= 0) {
                *arg = soc_reg_field_get(unit, IPMC_L3_MTUr, val, MTU_LENf);
            }
            return rv;
        }
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchMcastFloodDefault: {
        bcm_vlan_mcast_flood_t mode;
        BCM_IF_ERROR_RETURN(_bcm_esw_vlan_flood_default_get(unit, &mode));
        *arg = mode;
        return BCM_E_NONE;
    }
	break;

    case bcmSwitchHgHdrMcastFlood: {
        bcm_vlan_mcast_flood_t mode;
        BCM_IF_ERROR_RETURN(_bcm_esw_higig_flood_l2_get(unit, &mode));
        *arg = mode;
        return BCM_E_NONE;
    }
	break;

    case bcmSwitchHgHdrIpMcastFlood: {
        bcm_vlan_mcast_flood_t mode;
        BCM_IF_ERROR_RETURN(_bcm_esw_higig_flood_l3_get(unit, &mode));
        *arg = mode;
        return BCM_E_NONE;
    }
	break;

    case bcmSwitchUseGport:
        *arg = SOC_USE_GPORT(unit);
        return BCM_E_NONE;
	break;

    case bcmSwitchExternalL3:
#ifdef BCM_EASYRIDER_SUPPORT
        if (SOC_IS_EASYRIDER(unit)) {
            *arg = soc_easyrider_is_external_l3_enabled(unit);
            return BCM_E_NONE;
        }
#endif /* BCM_EASYRIDER_SUPPORT */
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchFailoverStackTrunk:
        *arg = (SOC_IS_XGS3_SWITCH(unit) || SOC_IS_XGS3_FABRIC(unit)) ? 1 : 0;
        return BCM_E_NONE;
	break;

    case bcmSwitchFailoverEtherTrunk:
        *arg = 0;
        return BCM_E_NONE;
	break;

    case bcmSwitchMcastL2Range:
#ifdef BCM_BRADLEY_SUPPORT
        if (SOC_IS_HBX(unit) || SOC_IS_SC_CQ(unit)) {
            int mc_base, mc_size;
            BCM_IF_ERROR_RETURN
                (soc_hbx_mcast_size_get(unit, &mc_base, &mc_size));
            *arg = mc_size;
            return BCM_E_NONE;
        }
#endif
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchMcastL3Range:
#ifdef BCM_BRADLEY_SUPPORT
        if (SOC_IS_HBX(unit) || SOC_IS_SC_CQ(unit)) {
            int ipmc_base, ipmc_size;
            BCM_IF_ERROR_RETURN
                (soc_hbx_ipmc_size_get(unit, &ipmc_base, &ipmc_size));
            *arg = ipmc_size;
            return BCM_E_NONE;
        }
#endif
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchHgHdrMcastVlanRange:
    case bcmSwitchHgHdrMcastL2Range:
    case bcmSwitchHgHdrMcastL3Range:
#ifdef BCM_BRADLEY_SUPPORT
        if (SOC_IS_HBX(unit) || SOC_IS_TRX(unit)) {
            int bc_size, mc_size, ipmc_size;
            BCM_IF_ERROR_RETURN
                (soc_hbx_higig2_mcast_sizes_get(unit, &bc_size,
                                                &mc_size, &ipmc_size));
            switch (type) {
            case bcmSwitchHgHdrMcastVlanRange:
                *arg = bc_size;
                break;
            case bcmSwitchHgHdrMcastL2Range:
                *arg = mc_size;
                break;
            case bcmSwitchHgHdrMcastL3Range:
                *arg = ipmc_size;
                break;
            default:
                return BCM_E_PARAM;
            }
            return BCM_E_NONE;
        }
#endif
        return BCM_E_UNAVAIL;
	break;

    case bcmSwitchL2McastAllRouterPortsAddEnable:
        *arg = SOC_MCAST_ADD_ALL_ROUTER_PORTS(unit);
        return BCM_E_NONE;
	break;

    case bcmSwitchBypassMode:
        *arg = SOC_SWITCH_BYPASS_MODE(unit);
        return BCM_E_NONE;
	break;

    case bcmSwitchPFCClass0Queue:
    case bcmSwitchPFCClass1Queue:
    case bcmSwitchPFCClass2Queue:
    case bcmSwitchPFCClass3Queue:
    case bcmSwitchPFCClass4Queue:
    case bcmSwitchPFCClass5Queue:
    case bcmSwitchPFCClass6Queue:
    case bcmSwitchPFCClass7Queue:
#if defined(BCM_SCORPION_SUPPORT)
        return _bcm_sc_pfc_priority_to_cos_get(unit, type, arg);
#endif /* BCM_SCORPION_SUPPORT */
	return BCM_E_UNAVAIL;
	break;

    case bcmSwitchL2HitClear:
    case bcmSwitchL2SrcHitClear:
    case bcmSwitchL2DstHitClear:
    case bcmSwitchL3HostHitClear:
    case bcmSwitchL3RouteHitClear:
	return _bcm_esw_switch_hit_clear_get(unit, type, arg);
	break;

    /* This swithc control should be supported only per port. */
    case bcmSwitchEncapErrorToCpu:
        return BCM_E_UNAVAIL;

    case bcmSwitchSnapNonZeroOui:
#if defined(BCM_TRX_SUPPORT)
        if (SOC_IS_TRX(unit)) {
            uint32 rval;
            uint64 rval64;
            SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
            *arg = soc_reg64_field32_get(unit, ING_CONFIG_64r, rval64,
                                         SNAP_OTHER_DECODE_ENABLEf);
            SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
            *arg = soc_reg_field_get(unit, EGR_CONFIGr, rval,
                                     SNAP_OTHER_DECODE_ENABLEf);
            return BCM_E_NONE;
        } else
#endif /* BCM_TRX_SUPPORT */
#if defined(BCM_FIREBOLT_SUPPORT)
        if (SOC_IS_FB(unit) || SOC_IS_FX_HX(unit) || SOC_IS_HB_GW(unit)) {
            uint32 rval;
            SOC_IF_ERROR_RETURN(READ_ING_CONFIGr(unit, &rval));
            *arg = soc_reg_field_get(unit, ING_CONFIGr, rval,
                                     SNAP_OTHER_DECODE_ENABLEf);
            if (SOC_IS_FIREBOLT2(unit)) {
                SOC_IF_ERROR_RETURN(READ_EGR_CONFIGr(unit, &rval));
                *arg = soc_reg_field_get(unit, EGR_CONFIGr, rval,
                                         SNAP_OTHER_DECODE_ENABLEf);
            }
            return BCM_E_NONE;
        }
#endif /* BCM_FIREBOLT_SUPPORT */
    default:
	break;
    }

    /* Stop on first supported port for port specific controls */
    PBMP_E_ITER(unit, port) {
        if (SOC_IS_STACK_PORT(unit, port)) {
            continue;
        }
        rv = bcm_esw_switch_control_port_get(unit, port, type, arg);
        if (rv != BCM_E_UNAVAIL) { /* Found one, or real error */
            return rv;
        }
    }

    PBMP_HG_ITER(unit, port) {
        if (SOC_IS_STACK_PORT(unit, port)) {
            continue;
        }
        rv = bcm_esw_switch_control_port_get(unit, port, type, arg);
        if (rv != BCM_E_UNAVAIL) { /* Found one, or real error */
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	_bcm_switch_init
 * Description:
 *	Initialize switch controls to their different values.
 * Parameters:
 *	unit        - Device unit number
 * Returns:
 *      BCM_E_xxx
 */
int 
_bcm_esw_switch_init(int unit)
{
    /* Unit validation check */
    if (!BCM_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

#if defined(BCM_HAWKEYE_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_APOLLO_SUPPORT)
    /* Initialize tymesync MMRP and SRP switch controls to default values */
    if (soc_feature(unit, soc_feature_timesync_support)) {
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchMMRPEthertype, 
                                   MMRP_ETHERTYPE_DEFAULT)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchSRPEthertype, 
                                   SRP_ETHERTYPE_DEFAULT)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncEthertype, 
                                   TS_ETHERTYPE_DEFAULT)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchMMRPDestMacOui, 
                                   MMRP_MAC_OUI_DEFAULT)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchSRPDestMacOui, 
                                   SRP_MAC_OUI_DEFAULT)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncDestMacOui, 
                                   TS_MAC_OUI_DEFAULT)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchMMRPDestMacNonOui, 
                                   MMRP_MAC_NONOUI_DEFAULT)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchSRPDestMacNonOui, 
                                   SRP_MAC_NONOUI_DEFAULT)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncDestMacNonOui, 
                                   TS_MAC_NONOUI_DEFAULT)); 
    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_set(unit, bcmSwitchTimeSyncMessageTypeBitmap, 
                                   TS_MAC_MSGBITMAP_DEFAULT)); 
    }
#endif
    return BCM_E_NONE;
}

