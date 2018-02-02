/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    cdmac.c
 * Purpose: 
 *         
 *         
 *
 */

#include <soc/portmod/portmod.h>
#include <soc/portmod/cdmac.h>

#if defined(PORTMOD_PM8X50_SUPPORT)

#ifdef _ERR_MSG_MODULE_NAME 
#error "_ERR_MSG_MODULE_NAME redefined" 
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#define CDMAC_SHORT_INIT
int cdmac_init(int unit, soc_port_t port, uint32 init_flags)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

#ifdef CDMAC_SHORT_INIT
    /* RX Control */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, STRIP_CRCf, 0);
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, RX_PASS_PAUSEf, 0);
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, RX_PASS_PFCf, 0);
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, RX_PASS_CTRLf, 1);
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, RUNT_THRESHOLDf, 64);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

    /* TX Control */
    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, DISCARDf, 0);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, PAD_ENf, 0);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, PAD_THRESHOLDf, 64);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, AVERAGE_IPGf, 12);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, TX_THRESHOLDf, 1);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval,
                          TX_CRC_CORRUPT_ENf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_TX_CTRLr(unit, port, rval));

    /* RX Max size */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_MAX_SIZEr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_MAX_SIZEr, &rval,
                      RX_MAX_SIZEf, CDMAC_JUMBO_MAXSZ);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_MAX_SIZEr(unit, port, rval));

    /* TX CDC ecc control */
    _SOC_IF_ERR_EXIT(READ_CDMAC_ECC_CTRLr(unit, &rval));
    soc_reg_field_set(unit, CDMAC_ECC_CTRLr, &rval,
                          TX_CDC_ECC_CTRL_ENf, 1);
    soc_reg_field_set(unit, CDMAC_ECC_CTRLr, &rval,
                          TX_CDC_FORCE_SINGLE_BIT_ERRf, 1);
    soc_reg_field_set(unit, CDMAC_ECC_CTRLr, &rval,
                          TX_CDC_FORCE_DOUBLE_BIT_ERRf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_ECC_CTRLr(unit,  rval));

    /*MAC control*/
    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LOCAL_LPBKf, 0);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, 1);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 1);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, 1);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));
#else /* full init */


#endif 

exit:
    SOC_FUNC_RETURN;
}

int cdmac_speed_set(int unit, soc_port_t port, int flags, int speed)
{
    /*
     * There is nothing to be programmed in CDMAC for Speed.
     * Speed setting is handled in the serdes.
     */
    return (SOC_E_NONE);
}

int cdmac_speed_get(int unit, soc_port_t port, int *speed)
{
    /*
     * There is nothing to be programmed in CDMAC for Speed.
     * Speed setting is handled in the serdes.
     */
    return (SOC_E_NONE);
}

int cdmac_encap_set(int unit, soc_port_t port, int flags,
                    portmod_encap_t encap)
{

    uint32 val = 0;
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * Only IEEE encap support, HIGIG not supported.
     */
    switch(encap){
        case SOC_ENCAP_IEEE:
            val = 0;
            break;
        case SOC_ENCAP_SOP_ONLY:
        /* no preamble or sfd, 0xFB is followed by mac da */
            val = 5;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               (_SOC_MSG("illegal encap mode %d"), encap));
            break;
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_MODEr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_MODEr, &rval, HDR_MODEf, val);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_MODEr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_encap_get(int unit, soc_port_t port, int *flags,
                    portmod_encap_t *encap)
{
    uint32 rval;
    uint32 fld_val;
    SOC_INIT_FUNC_DEFS;

    (*flags) = 0;

    _SOC_IF_ERR_EXIT(READ_CDMAC_MODEr(unit, port, &rval));
    fld_val = soc_reg_field_get(unit, CDMAC_MODEr, rval, HDR_MODEf);

    switch(fld_val){
        case 0:
            *encap = SOC_ENCAP_IEEE;
            break;
        case 5:
            *encap = SOC_ENCAP_SOP_ONLY;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                              (_SOC_MSG("unknown encap mode %d"), fld_val));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_enable_set(int unit, soc_port_t port, int flags, int enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, 1);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, enable? 1: 0);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, enable? 0: 1);

    return (SOC_E_NONE);
}

int cdmac_enable_get(int unit, soc_port_t port, int flags, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));

    if (flags & CDMAC_ENABLE_SET_FLAGS_TX_EN) {
        *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, TX_ENf);
    } else {
        *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, RX_ENf);
    }

    return (SOC_E_NONE);
}

/*
 * This function can be called to either set/reset
 * the TX/RX or put the MAC in reset/bring the MAC
 * out of reset.
 * Only one operation allowed in a single call.
 */
int cdmac_enable_selective_set(int unit, soc_port_t port,
                               int flags, int enable)
{
    uint32 rval, orig_rval;
    SOC_INIT_FUNC_DEFS;


    if (!(flags == CDMAC_ENABLE_SET_FLAGS_TX_EN) &&
        !(flags == CDMAC_ENABLE_SET_FLAGS_RX_EN) &&
        !(flags == CDMAC_ENABLE_SET_FLAGS_SOFT_RESET)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("unknown control flag - %x"), flags));
    }

    /*
     * based on the flags passed the required fields are set/reset
     */

    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
    COMPILER_64_COPY(orig_rval, rval);

    if (flags == CDMAC_ENABLE_SET_FLAGS_TX_EN) {
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, enable? 1: 0);
    }

    if (flags == CDMAC_ENABLE_SET_FLAGS_RX_EN) {
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, enable? 1: 0);
    }

    /*
     * if enable = 0, bring mac out of reset, else mac in reset.
     */
    if (flags == CDMAC_ENABLE_SET_FLAGS_SOFT_RESET) {
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, enable? 1: 0);
    }

    /* write only if value changed */
    if(COMPILER_64_NE(rval, orig_rval)) { /* write only if value changed */
       _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_duplex_set(int unit, soc_port_t port, int duplex)
{

    /* Only duplex support, nothing to be done here */
    return (SOC_E_NONE);
}

int cdmac_duplex_get(int unit, soc_port_t port, int *duplex)
{

    /* Only duplex support, nothing to be done here */
    return (SOC_E_NONE);
}


int cdmac_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t lb,
                       int enable)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /* only line side loopback supported */
    if (lb == portmodLoopbackMacOuter) {
        _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LOCAL_LPBKf,
                          enable? 1: 0);
        _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("unsupported loopback type %d"), lb));
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t lb,
                       int *enable)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    if (lb == portmodLoopbackMacOuter) {
        _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
        *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, LOCAL_LPBKf);
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("unsupported loopback type %d"), lb));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_tx_mac_enable_set is the caller function
 */
int cdmac_tx_enable_set (int unit, soc_port_t port, int enable)
{
    return(cdmac_enable_selective_set(unit, port,
                        CDMAC_ENABLE_SET_FLAGS_TX_EN, enable));
}

/*
 * portmod_port_tx_mac_enable_get is the caller function
 */
int cdmac_tx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, TX_ENf);

    return (SOC_E_NONE);
}

/*
 * portmod_port_rx_mac_enable_set is the caller function
 */
int cdmac_rx_enable_set (int unit, soc_port_t port, int enable)
{

    return(cdmac_enable_selective_set(unit, port,
                        CDMAC_ENABLE_SET_FLAGS_RX_EN, enable));
}

/*
 * portmod_port_rx_mac_enable_get is the caller function
 */
int cdmac_rx_enable_get (int unit, soc_port_t port, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, RX_ENf);

    return (SOC_E_NONE);
}

/*
 * portmod_port_mac_reset_set is the caller function
 */
int cdmac_soft_reset_set(int unit, soc_port_t port, int enable)
{
    return(cdmac_enable_selective_set(unit, port,
                        CDMAC_ENABLE_SET_FLAGS_SOFT_RESET, enable));
}

/*
 * portmod_port_mac_reset_set is the caller function
 */
int cdmac_soft_reset_get(int unit, soc_port_t port, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, SOFT_RESETf);

    return (SOC_E_NONE);
}

int cdmac_tx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    /*
     * Write bytes in the following order.
     * mac[0] -> BYTE 5(MSB) of the MAC address in HW
     * mac[1] -> BYTE 4
     * mac[2] -> BYTE 3
     * mac[3] -> BYTE 2
     * mac[4] -> BYTE 1
     * mac[5] -> BYTE 0(LSB)
     */
    COMPILER_64_ZERO(rval64);
    COMPILER_64_SET(rval64, _shr_uint16_read(&mac[0]),
                    _shr_uint32_read(&mac[2]));

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_TX_MAC_SAr(unit, port, rval64));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_tx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64, field64;
    uint32 mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_MAC_SAr(unit, port, &rval64));
    field64 = soc_reg64_field_get(unit, CDMAC_TX_MAC_SAr, rval64, CTRL_SAf);

    mac_addr[0] = COMPILER_64_HI(field64);
    mac_addr[1] = COMPILER_64_LO(field64);

    /*
     * Extract bytes in the following order
     * BYTE 5(MSB) -> mac[0]
     * BYTE 4 -> mac[1]
     * BYTE 3 -> mac[2]
     * BYTE 2 -> mac[3]
     * BYTE 1 -> mac[4]
     * BYTE 0(LSB) -> mac[5]
     */
    mac[0] = (mac_addr[0] & 0x0000ff00) >> 8;
    mac[1] = (mac_addr[0] & 0x000000ff);
    mac[2] = (mac_addr[1] & 0xff000000) >> 24;
    mac[3] = (mac_addr[1] & 0x00ff0000) >> 16;
    mac[4] = (mac_addr[1] & 0x0000ff00) >> 8;
    mac[5] = (mac_addr[1] & 0x000000ff);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_mac_sa_set(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(rval64);
    COMPILER_64_SET(rval64, _shr_uint16_read(&mac[0]),
                    _shr_uint32_read(&mac[2]));

    /*
     * Write bytes in the following order.
     * mac[0] -> BYTE 5(MSB) of the MAC address in HW
     * mac[1] -> BYTE 4
     * mac[2] -> BYTE 3
     * mac[3] -> BYTE 2
     * mac[4] -> BYTE 1
     * mac[5] -> BYTE 0(LSB)
     */
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_MAC_SAr(unit, port, rval64));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_mac_sa_get(int unit, soc_port_t port, sal_mac_addr_t mac)
{
    uint64 rval64, field64;
    uint32 mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_MAC_SAr(unit, port, &rval64));
    field64 = soc_reg64_field_get(unit, CDMAC_RX_MAC_SAr, rval64, RX_SAf);

    mac_addr[0] = COMPILER_64_HI(field64);
    mac_addr[1] = COMPILER_64_LO(field64);

    /*
     * Extract bytes in the following order
     * BYTE 5(MSB) -> mac[0]
     * BYTE 4 -> mac[1]
     * BYTE 3 -> mac[2]
     * BYTE 2 -> mac[3]
     * BYTE 1 -> mac[4]
     * BYTE 0(LSB) -> mac[5]
     */
    mac[0] = (mac_addr[0] & 0x0000ff00) >> 8;
    mac[1] = (mac_addr[0] & 0x000000ff);
    mac[2] = (mac_addr[1] & 0xff000000) >> 24;
    mac[3] = (mac_addr[1] & 0x00ff0000) >> 16;
    mac[4] = (mac_addr[1] & 0x0000ff00) >> 8;
    mac[5] = (mac_addr[1] & 0x000000ff);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_vlan_tag_set(int unit, soc_port_t port, int outer_vlan_tag,
                          int inner_vlan_tag)
{
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_VLAN_TAGr(unit, port, &rval64));

    if(inner_vlan_tag == -1) {
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             INNER_VLAN_TAG_ENABLEf, 0);
    } else {
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             INNER_VLAN_TAGf, inner_vlan_tag);
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             INNER_VLAN_TAG_ENABLEf, 1);
    }

    if(outer_vlan_tag == -1) {
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             OUTER_VLAN_TAG_ENABLEf, 0);
    } else {
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             OUTER_VLAN_TAGf, outer_vlan_tag);
       soc_reg64_field32_set(unit, CDMAC_RX_VLAN_TAGr, &rval64,
                             OUTER_VLAN_TAG_ENABLEf, 1);
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_vlan_tag_get(int unit, soc_port_t port, int *outer_vlan_tag,
                          int *inner_vlan_tag)
{
    uint64 rval64;
    uint32 is_enabled = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_VLAN_TAGr(unit, port, &rval64));

    is_enabled = soc_reg64_field32_get(unit, CDMAC_RX_VLAN_TAGr, rval64,
                                       INNER_VLAN_TAG_ENABLEf);
    if(is_enabled == 0) {
       *inner_vlan_tag = -1;
    } else {
       *inner_vlan_tag = soc_reg64_field32_get(unit, CDMAC_RX_VLAN_TAGr,
                                               rval64, INNER_VLAN_TAGf);
    }

    is_enabled = soc_reg64_field32_get(unit, CDMAC_RX_VLAN_TAGr, rval64,
                                       OUTER_VLAN_TAG_ENABLEf);
    if(is_enabled == 0) {
       *outer_vlan_tag = -1;
    } else {
       *outer_vlan_tag = soc_reg64_field32_get(unit, CDMAC_RX_VLAN_TAGr,
                                               rval64, OUTER_VLAN_TAGf);
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_max_size_set(int unit, soc_port_t port, int value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * Maximum packet size in receive direction, exclusive of preamble
     * and  CRC in strip mode. Packets greater than this size are
     * truncated to this value.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_MAX_SIZEr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_MAX_SIZEr, &rval, RX_MAX_SIZEf, value);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_MAX_SIZEr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_rx_max_size_get(int unit, soc_port_t port, int *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_MAX_SIZEr(unit, port, &rval));
     *value = soc_reg_field_get(unit, CDMAC_RX_MAX_SIZEr, rval, RX_MAX_SIZEf);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pad_size_set(int unit, soc_port_t port, int value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    if ((value < CDMAC_MIN_PAD_THRESHOLD_SIZE) ||
        (value > CDMAC_MAX_PAD_THRESHOLD_SIZE)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                   (_SOC_MSG("unsupported pad threshold size %d"), value));
    }

    /*
     * If PAD_EN is set, packets smaller than PAD_THRESHOLD are padded
     * to this size. PAD_THRESHOLD must be >=64 and <= 96.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, PAD_ENf, value? 1: 0);
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, PAD_THRESHOLDf, value);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_TX_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pad_size_get(int unit, soc_port_t port, int *value)
{
    uint32 rval;
    uint32 pad_en;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    pad_en = soc_reg_field_get(unit, CDMAC_TX_CTRLr, rval, PAD_ENf);
    if(!pad_en) {
        *value = 0;
    } else {
        *value = soc_reg_field_get(unit, CDMAC_TX_CTRLr, rval, PAD_THRESHOLDf);
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_tx_average_ipg_set(int unit, soc_port_t port, int val)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * Average inter packet gap can be in the range 8 to 56 or 60.
     * should be 56 for XLGMII, 60 for XGMII,
     * default is 12.
     * Granularity is in bytes.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, AVERAGE_IPGf, val);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_TX_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_tx_average_ipg_get(int unit, soc_port_t port, int *val)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_TX_CTRLr(unit, port, &rval));

    *val = soc_reg_field_get(unit, CDMAC_TX_CTRLr, rval, AVERAGE_IPGf);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_runt_threshold_set(int unit, soc_port_t port, int value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * The threshold, below which the packets are dropped or
     * marked as runt. Should be programmed >=64 and <= 96 bytes.
     */
    if ((value < CDMAC_MIN_RUNT_THRESHOLD) ||
        (value > CDMAC_MAX_RUNT_THRESHOLD)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
            (_SOC_MSG("runt size should be greater than %d and "
            "smaller than %d. got %d"), CDMAC_MIN_RUNT_THRESHOLD,
            CDMAC_MAX_RUNT_THRESHOLD,  value));
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval, RUNT_THRESHOLDf, value);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_runt_threshold_get(int unit, soc_port_t port, int *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    *value = soc_reg_field_get(unit, CDMAC_RX_CTRLr, rval, RUNT_THRESHOLDf);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_remote_fault_control_set is the caller of this function.
 */
int cdmac_remote_fault_control_set(int unit, soc_port_t port,
                          const portmod_remote_fault_control_t *control)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * REMOTE_FAULT_DISABLE determines the transmit response during remote
     * fault state. The REMOTE_FAULT_STATUS bit is always updated irrespective
     * of this configuration.
     * If set, MAC will continue to transmit data irrespective of
     *  REMOTE_FAULT_STATUS.
     * If reset, MAC transmit behavior is governed by
     * DROP_TX_DATA_ON_REMOTE_FAULT configuration.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval, REMOTE_FAULT_DISABLEf,
                      control->enable? 0: 1); /* flip */
    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval,
                      DROP_TX_DATA_ON_REMOTE_FAULTf,
                      control->drop_tx_on_fault? 1: 0);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_LSS_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_remote_fault_control_get is the caller of this function.
 */
int cdmac_remote_fault_control_get(int unit, soc_port_t port,
                                   portmod_remote_fault_control_t *control)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));

    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             REMOTE_FAULT_DISABLEf);
    /* if fval is reset, indicates enable */
    control->enable = (fval? 0: 1); 

    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             DROP_TX_DATA_ON_REMOTE_FAULTf);
    control->drop_tx_on_fault = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_remote_fault_status_get is the caller of this function.
 * The status bit is clear on read, no seperate api for status_clear  
 * required.
 */
int cdmac_remote_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &rval));
    /* fault status bits clear on read */
    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                             REMOTE_FAULT_STATUSf);
    *status = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_local_fault_control_set is the caller of this function.
 */
int cdmac_local_fault_control_set(int unit, soc_port_t port,
                                  const portmod_local_fault_control_t *control)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * LOCALFAULT_DISABLE determines the transmit response during remote
     * fault state. The LOCAL_FAULT_STATUS bit is always updated irrespective
     * of this configuration.
     * If set, MAC will continue to transmit data irrespective of
     *  REMOTE_FAULT_STATUS.
     * If reset, MAC transmit behavior is governed by 
     * DROP_TX_DATA_ON_LOCAL_FAULT configuration.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval, LOCAL_FAULT_DISABLEf,
                      control->enable? 0: 1); /* flip */
    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval,
                      DROP_TX_DATA_ON_LOCAL_FAULTf,
                      control->drop_tx_on_fault? 1: 0);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RX_LSS_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_local_fault_control_get is the caller of this function.
 */
int cdmac_local_fault_control_get(int unit, soc_port_t port,
                                  portmod_local_fault_control_t *control)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));

    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             LOCAL_FAULT_DISABLEf);
    control->enable = (fval? 0: 1);

    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             DROP_TX_DATA_ON_LOCAL_FAULTf);
    control->drop_tx_on_fault = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_local_fault_status_get is the caller of this function.
 * The status bit is clear on read, no seperate api for status_clear  
 * required.
 */
int cdmac_local_fault_status_get(int unit, soc_port_t port, int *status)
{
    uint32 rval;
    uint32 fval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_RX_LSS_STATUSr(unit, port, &rval));
    /* fault status bits clear on read */
    fval = soc_reg_field_get(unit, CDMAC_RX_LSS_STATUSr, rval,
                             LOCAL_FAULT_STATUSf);
    *status = (fval? 1: 0);

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_pfc_control_set is the caller of this function.
 */
int cdmac_pfc_control_set(int unit, soc_port_t port,
                         const portmod_pfc_control_t *control)
{
    uint64 rval64;
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));

    if(control->refresh_timer > 0 ) {
       soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                         PFC_REFRESH_TIMERf, control->refresh_timer);
       soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                             PFC_REFRESH_ENf, 1);
    } else {
       soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                             PFC_REFRESH_ENf, 0);
    }
    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                      PFC_XOFF_TIMERf, control->xoff_timer);

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_PAUSE_CTRLr(unit, port, rval64));


    _SOC_IF_ERR_EXIT(READ_CDMAC_PFC_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, PFC_STATS_ENf,
                      control->stats_en);
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, FORCE_PFC_XONf,
                      control->force_xon);
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, TX_PFC_ENf,
                      control->tx_enable);
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, RX_PFC_ENf,
                      control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_PFC_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pfc_control_get(int unit, soc_port_t port,
                          portmod_pfc_control_t *control)
{
    uint64 rval64;
    uint32 rval;
    uint32 refresh_enable = 0;
    uint32 refresh_timer = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));

    refresh_enable = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                       PFC_REFRESH_ENf);
    refresh_timer = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                      PFC_REFRESH_TIMERf);

    control->refresh_timer = (refresh_enable? refresh_timer: 0);
    control->xoff_timer = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr,
                                                rval64, PFC_XOFF_TIMERf);

    _SOC_IF_ERR_EXIT(READ_CDMAC_PFC_CTRLr(unit, port, &rval));

    control->stats_en = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                           PFC_STATS_ENf);
    control->force_xon = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                           FORCE_PFC_XONf);
    control->tx_enable = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                           TX_PFC_ENf);
    control->rx_enable = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                           RX_PFC_ENf);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pause_control_set(int unit, soc_port_t port,
                            const portmod_pause_control_t *control)
{
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));

    if(control->rx_enable || control->tx_enable) {
       if(control->refresh_timer > 0 ) {
          soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                            PAUSE_REFRESH_TIMERf, control->refresh_timer);
          soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                            PAUSE_REFRESH_ENf, 1);
       } else {
          soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                            PAUSE_REFRESH_ENf, 0);
       }
       soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64,
                         PAUSE_XOFF_TIMERf, control->xoff_timer);
    }

    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, TX_PAUSE_ENf,
                      control->tx_enable);
    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, RX_PAUSE_ENf,
                      control->rx_enable);

    _SOC_IF_ERR_EXIT(WRITE_CDMAC_PAUSE_CTRLr(unit, port, rval64));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pause_control_get(int unit, soc_port_t port,
                            portmod_pause_control_t *control)
{
    uint64 rval64;
    uint32 refresh_enable;
    uint32 refresh_timer;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));

    refresh_enable = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                       PAUSE_REFRESH_ENf);
    refresh_timer = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                      PAUSE_REFRESH_TIMERf);

    control->refresh_timer = (refresh_enable? refresh_timer: -1);
    control->xoff_timer = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr,
                                                rval64, PAUSE_XOFF_TIMERf);

    control->tx_enable = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                           TX_PAUSE_ENf);
    control->rx_enable = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr, rval64,
                                           RX_PAUSE_ENf);

exit:
    SOC_FUNC_RETURN;
}

int cdmac_pfc_config_set (int unit, int port,
                          const portmod_pfc_config_t* pfc_cfg)
{
    uint32 rval;
    uint32 fval[2] = {};
    uint64 rval64;
    uint64 fval64;

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_TYPEr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_PFC_TYPEr, &rval, PFC_ETH_TYPEf,
                      pfc_cfg->type);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_TYPEr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_OPCODEr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_PFC_OPCODEr, &rval, PFC_OPCODEf,
                      pfc_cfg->opcode);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_OPCODEr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_DAr(unit, port, &rval64));
    fval[0] |= pfc_cfg->da_nonoui;
    fval[0] |= (pfc_cfg->da_oui & 0xff) << 24;
    fval[1] |= (pfc_cfg->da_oui & 0xffff00) >> 8;

    COMPILER_64_SET(fval64, fval[1], fval[0]);
    soc_reg64_field_set(unit, CDMAC_PFC_DAr, &rval64, PFC_MACDAf, fval64);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_DAr(unit, port, rval64));

    return (SOC_E_NONE);
}

int cdmac_pfc_config_get (int unit, int port, portmod_pfc_config_t* pfc_cfg)
{
    uint32 rval;
    uint32 fval[2] = {};
    uint64 rval64;
    uint64 fval64;

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_TYPEr(unit, port, &rval));
    pfc_cfg->type = soc_reg_field_get(unit, CDMAC_PFC_TYPEr, rval,
                                      PFC_ETH_TYPEf);

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_OPCODEr(unit, port, &rval));
    pfc_cfg->opcode = soc_reg_field_get(unit, CDMAC_PFC_OPCODEr, rval,
                                        PFC_OPCODEf);

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_DAr(unit, port, &rval64));
    fval64 = soc_reg64_field_get(unit, CDMAC_PFC_DAr, rval64, PFC_MACDAf);
    fval[0] = COMPILER_64_LO(fval64);
    fval[1] = COMPILER_64_HI(fval64);

    pfc_cfg->da_nonoui = fval[0] & 0xffffff;
    pfc_cfg->da_oui = (fval[1] << 8) | (fval[0] >> 24);

    return (SOC_E_NONE);
}


int cdmac_pass_control_frame_set(int unit, int port, int value)
{
    uint32 rval;

    /* 
     * This configuration is used to drop or pass all control frames
     * (with ether type 0x8808) except pause packets.
     * If set, all control frames are passed to system side.
     * if reset, control frames (including pfc frames wih ether type 0x8808)i
     * are dropped in CDMAC.
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval,
                      RX_PASS_CTRLf, (value? 1: 0));
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_pass_control_frame_get(int unit, int port, int *value)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    *value = soc_reg_field_get(unit, CDMAC_RX_CTRLr, rval, RX_PASS_CTRLf);

    return (SOC_E_NONE);
}


int cdmac_pass_pfc_frame_set(int unit, int port, int value)
{
    uint32 rval;

    /*
     * This configuration is used to pass or drop PFC packets when 
     * PFC_ETH_TYPE is not equal to 0x8808.
     * If set, PFC frames are passed to system side.
     * If reset, PFC frames are dropped in CDMAC.
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval,
                      RX_PASS_PFCf, (value? 1: 0));
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_pass_pfc_frame_get(int unit, int port, int *value)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    *value = soc_reg_field_get(unit, CDMAC_RX_CTRLr, rval, RX_PASS_PFCf);

    return (SOC_E_NONE);
}


int cdmac_pass_pause_frame_set(int unit, int port, int value)
{
    uint32 rval;

    /*
     * If set, PAUSE frames are passed to sytem side.
     * If reset, PAUSE frames are dropped in CDMAC
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_CTRLr, &rval,
                      RX_PASS_PAUSEf, (value? 1: 0));
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_RX_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_pass_pause_frame_get(int unit, int port, int *value)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_CTRLr(unit, port, &rval));
    *value = soc_reg_field_get(unit, CDMAC_RX_CTRLr, rval, RX_PASS_PAUSEf);

    return (SOC_E_NONE);
}

/*
 * LAG FAILOVER
 * Following routines are used and invoked for the LAG
 * failover feature.
 * When a LAG(Trunk) member port goes down, if LAG failover
 * is enabled all the tx packets are loopback to RX path.
 * This is done by enabling loopback on the port which went
 * down, when the port comes back up, the software detects
 * the LINK UP event and removes the loopback.
 * Programming Sequence:
 * CDMAC_CTRL.LAG_FAILOVER_ENf 1/0, enable/disable feature.
 *                             If set, enable LAG Failover.
 * This bit has priority over LOCAL_LPBK. The lag failover
 * kicks in when the link status selected by LINK_STATUS_SELECT
 * transitions from 1 to 0. TSC clock and TSC credits must
 * be active for lag failover.
 *
 * portmod_port_trunk_hwfailover_config_set is the caller
 * of this function, which in turn is called by the bcm
 * application layer function calls handling the lag  hardware
 * failover feature.
 */
int cdmac_lag_failover_en_set(int unit, int port, int val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LAG_FAILOVER_ENf, val);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

/*
 * portmod_port_trunk_hwfailover_config_get is the caller
 * of this function, returns if the hw lag failover feature
 * is enabled on a port.
 */
int cdmac_lag_failover_en_get(int unit, int port, int *val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *val = soc_reg_field_get(unit, CDMAC_CTRLr, rval, LAG_FAILOVER_ENf);

    return (SOC_E_NONE);
}

/*
 * This function is used to disable the LAG failover feature
 * on a port.
 * portmod_port_lag_failover_disable is the caller
 * of this function, which in turn is called by the bcm
 * application layer function calls handling the lag hardware
 * failover feature.
 */
int cdmac_lag_failover_disable(int unit, int port)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));

    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LAG_FAILOVER_ENf, 0);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LINK_STATUS_SELECTf, 0);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, REMOVE_FAILOVER_LPBKf, 0);

    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

/*
 * portmod_port_lag_failover_loopback_set is the caller
 * of this function.
 */
int cdmac_lag_failover_loopback_set(int unit, int port, int val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_LAG_FAILOVER_STATUSr(unit, port, &rval));
    /* If set, indicates that the port is in lag failover state */
    soc_reg_field_set(unit, CDMAC_LAG_FAILOVER_STATUSr, &rval,
                      LAG_FAILOVER_LOOPBACKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_LAG_FAILOVER_STATUSr(unit, port, rval));

    return (SOC_E_NONE);
}

/*
 * portmod_port_lag_failover_loopback_get is the caller
 * of this function. This function is invoked as part of
 * LINK events from LINKSCAN thread
 */
int cdmac_lag_failover_loopback_get(int unit, int port, int *val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_LAG_FAILOVER_STATUSr(unit, port, &rval));
    /*
     * The LAG_FAILOVER_LOOPBACK is SET by the hardware if
     * the LINK goes down and LAG_FAILOVER_ENf is set on the port.
     */
    *val = soc_reg_field_get(unit, CDMAC_LAG_FAILOVER_STATUSr, rval,
                             LAG_FAILOVER_LOOPBACKf);

    return (SOC_E_NONE);
}

/*
 * portmod_port_lag_remove_failover_lpbk_set is the caller
 * of this function. This function is invoked as part of
 * LINK events from LINKSCAN thread
 */
int cdmac_lag_remove_failover_lpbk_set(int unit, int port, int val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    /*
     * If set, CDMAC port will move from lag failover state to
     * normal operation. This bit should be set after link is up.
     * A transition from 0 to 1 is required, so first a write with
     * 0 should be followed by a write with 1.
     * The BCM layer should take care of the calling sequnce as
     * part of LINK events.
     */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, REMOVE_FAILOVER_LPBKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

/*
 * portmod_port_lag_remove_failover_lpbk_get is the caller
 * of this function.
 */
int cdmac_lag_remove_failover_lpbk_get(int unit, int port, int *val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *val = soc_reg_field_get(unit, CDMAC_CTRLr, rval, REMOVE_FAILOVER_LPBKf);

    return (SOC_E_NONE);
}

int cdmac_reset_fc_timers_on_link_dn_get(int unit, soc_port_t port, int *val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));
    *val = soc_reg_field_get(unit, CDMAC_RX_LSS_CTRLr, rval,
                             RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf);

    return (SOC_E_NONE);
}

/*
 * portmod_port_trunk_hwfailover_config_set is the caller of this
 * function. This function is invoked during lag failover config
 * on a port.
 */
int cdmac_reset_fc_timers_on_link_dn_set(int unit, soc_port_t port, int val)
{
    uint32 rval;

    /*
     * If set, the receive pause and PFC timers are reset whenever the link
     * status is down, or local or remote faults are received.
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_RX_LSS_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_RX_LSS_CTRLr, &rval,
                      RESET_FLOW_CONTROL_TIMERS_ON_LINK_DOWNf, val);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_RX_LSS_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_mac_ctrl_set(int unit, int port, uint64 ctrl)
{
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, ctrl));
    return (SOC_E_NONE);
}

int cdmac_drain_cell_get(int unit, int port,
                         portmod_drain_cells_t *drain_cells)
{
    uint32 rval;
    uint64 rval64;

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_CTRLr(unit, port, &rval));
    drain_cells->rx_pfc_en = soc_reg_field_get(unit, CDMAC_PFC_CTRLr, rval,
                                               RX_PFC_ENf);

    SOC_IF_ERROR_RETURN(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));
    drain_cells->rx_pause = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr,
                                                  rval64, RX_PAUSE_ENf);
    drain_cells->tx_pause = soc_reg64_field32_get(unit, CDMAC_PAUSE_CTRLr,
                                                  rval64, TX_PAUSE_ENf);

    return (SOC_E_NONE);
}


int cdmac_discard_set(int unit, soc_port_t port, int discard)
{
    uint32 rval;

    /* Clear Discard fields */
    SOC_IF_ERROR_RETURN(READ_CDMAC_TX_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_TX_CTRLr, &rval, DISCARDf, discard);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_TX_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}


int cdmac_drain_cell_stop(int unit, int port,
                           const portmod_drain_cells_t *drain_cells)
{
    uint32 rval;
    uint64 rval64;

    /* Clear Discard fields */
    SOC_IF_ERROR_RETURN(cdmac_discard_set(unit, port, 0));

    /* set pause fields */
    SOC_IF_ERROR_RETURN(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));
    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, RX_PAUSE_ENf,
                      drain_cells->rx_pause);
    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, TX_PAUSE_ENf,
                      drain_cells->tx_pause);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PAUSE_CTRLr(unit, port, rval64));

    /* set pfc rx_en fields */
    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, RX_PFC_ENf,
                      drain_cells->rx_pfc_en);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

int cdmac_drain_cell_start(int unit, int port)
{
    uint32 rval;
    uint64 rval64;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, 1);
    /* Disable RX */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, 0);

    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CDMAC_PAUSE_CTRLr(unit, port, &rval64));
    soc_reg64_field32_set(unit, CDMAC_PAUSE_CTRLr, &rval64, RX_PAUSE_ENf,0);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PAUSE_CTRLr(unit, port, rval64));

    SOC_IF_ERROR_RETURN(READ_CDMAC_PFC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_PFC_CTRLr, &rval, RX_PFC_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_PFC_CTRLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    SOC_IF_ERROR_RETURN(cdmac_discard_set(unit, port, 1));

    return (SOC_E_NONE);
}


int cdmac_txfifo_cell_cnt_get(int unit, int port, uint32* val)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_TXFIFO_CELL_CNTr(unit, port, &rval));
    *val = soc_reg_field_get(unit, CDMAC_TXFIFO_CELL_CNTr, rval, CELL_CNTf);

    return (SOC_E_NONE);
}

int cdmac_egress_queue_drain_get(int unit, int port, uint64 *mac_ctrl,
                                 int *rx_en)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *mac_ctrl = rval;

    *rx_en = soc_reg_field_get(unit, CDMAC_CTRLr, rval, RX_ENf);

    return (SOC_E_NONE);
}


int cdmac_drain_cells_rx_enable(int unit, int port, int rx_en)
{
    uint32 rval, orig_rval;
    uint32 soft_reset = 0;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    orig_rval = rval;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, 1);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, rx_en ? 1: 0);

    if (COMPILER_64_EQ(rval, orig_rval)) {
        /*
         *  To avoid the unexpected early return to prevent this problem.
         *  1. Problem occurred for disabling process only.
         *  2. To comply original designing scenario, CDMAC_CTRLr.SOFT_RESETf
         *     is used to early check to see if this port is at disabled state
         *     already.
         */
        soft_reset = soc_reg_field_get(unit, CDMAC_CTRLr, rval, SOFT_RESETf);
        if ((rx_en) || (!rx_en && soft_reset)){
            return SOC_E_NONE;
        }
    }

    if (rx_en) {
        soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SOFT_RESETf, 0);
    }

    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

int cdmac_egress_queue_drain_rx_en(int unit, int port, int rx_en)
{
    uint32 rval;

    /* Enable RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, rx_en? 1: 0);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));
    /*Bring mac out of reset */
    SOC_IF_ERROR_RETURN(cdmac_soft_reset_set(unit, port, 0));

    return (SOC_E_NONE);
}

int cdmac_reset_check(int unit, int port, int enable, int *reset)
{
    uint32 rval, orig_rval;

    *reset = 1;

    /* Enable both TX and RX, de-assert SOFT_RESET */
    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    orig_rval = rval;

    /* Don't disable TX since it stops egress and hangs if CPU sends */
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, TX_ENf, 1);
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, RX_ENf, enable? 1: 0);

    if (COMPILER_64_EQ(rval, orig_rval)) {
        if (enable) {
            *reset = 0;
        } else {
            if (soc_reg_field_get(unit, CDMAC_CTRLr, rval, SOFT_RESETf)) {
                *reset = 0;
            }
        }
    }

    return (SOC_E_NONE);
}

/*
 * portmod_port_trunk_hwfailover_config_set is the caller of this
 * function. This function is invoked during lag failover config
 * on a port.
 */
int cdmac_sw_link_status_select_set(int unit, soc_port_t port, int enable)
{
    uint32 rval;

    /*
     * This configuration chooses between link status indication from software
     * (SW_LINK_STATUSf) or the hardware link status indication from the PCS i
     * to MAC. If set, hardware link status is used.
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, LINK_STATUS_SELECTf, enable);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_CTRLr(unit, port, rval));

    return (SOC_E_NONE);
}

int cdmac_sw_link_status_select_get(int unit, soc_port_t port, int *enable)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_CDMAC_CTRLr(unit, port, &rval));
    *enable = soc_reg_field_get(unit, CDMAC_CTRLr, rval, LINK_STATUS_SELECTf);

    return (SOC_E_NONE);
}

int cdmac_sw_link_status_set (int unit, soc_port_t port, int link)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    /*
     * This is valid only if LINK_STATUS_SELECT is 0, which means the 
     * software drives the link status. If SW_LINK_STATUS is set,
     * it indicates that the link is active. The use case is if there is  
     * some other mechanism used for the link status determination other
     * than hardware.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_CTRLr, &rval, SW_LINK_STATUSf, link);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_sw_link_status_get (int unit, soc_port_t port, int *link)
{
    uint32 rval;
    int link_status_sel = 0;
    SOC_INIT_FUNC_DEFS;

    /*
     * check if link status selection is software based, if yes return 
     * sw_link_status.
     */
    _SOC_IF_ERR_EXIT(
        cdmac_sw_link_status_select_get(unit, port, &link_status_sel));

    /* link status selection is software based */
    if (!link_status_sel) {
        _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
        *link = soc_reg_field_get(unit, CDMAC_CTRLr, rval, SW_LINK_STATUSf);
    }

exit:
    SOC_FUNC_RETURN;
}

int cdmac_mib_counter_control_set(int unit, soc_port_t port,
                                  int enable, int clear)
{
    uint32 rval;

    /* 
     * For enabling the MIB statistics counters for a port, the ENABLEf 
     * must be set.
     * To reset/clear MIB statistics counters, CLEARf should be set 1.
     * A low-to-high(0->1) transition on this bit(CLEARf) will trigger
     * the counter-clear operation.
     * Please Note:
     * If a subsequent counter-clear operation is required, this bit has to 
     * be first written to 0 and then written to 1. Instead of doing this
     * setting CLEARf to 0 after setting to 1.
     */
    SOC_IF_ERROR_RETURN(READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_MIB_COUNTER_CTRLr, &rval,
                      ENABLEf, enable);
    soc_reg_field_set(unit, CDMAC_MIB_COUNTER_CTRLr, &rval, CLEARf, clear);
    SOC_IF_ERROR_RETURN(WRITE_CDMAC_MIB_COUNTER_CTRLr(unit, port, rval));

    /*
     * set CLEARf to 0, this operation is not mandatory, adding to 
     * remove ambiguity in interpretation if a read on this register
     */
    if (clear) {
        SOC_IF_ERROR_RETURN(READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &rval));
        soc_reg_field_set(unit, CDMAC_MIB_COUNTER_CTRLr, &rval, CLEARf, 0);
        SOC_IF_ERROR_RETURN(WRITE_CDMAC_MIB_COUNTER_CTRLr(unit, port, rval));
    }

    return (SOC_E_NONE);
}

/*
 * portmod_port_cntmaxsize_set is caller of this function
 */
int cdmac_cntmaxsize_set(int unit, int port, int val)
{
    uint32 rval;
    int field_len = 0;
    SOC_INIT_FUNC_DEFS;

    field_len = soc_reg_field_length(unit, CDMAC_MIB_COUNTER_CTRLr,
                                     CNTMAXSIZEf);

    if ((!field_len) || (val > ((1 << field_len) - 1))) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                          (_SOC_MSG("invalid field length(%d) or value(%d)"),
                          field_len, val));
    }
    /*
     * The maximum packet size that is used in statistics counter updates.
     * default size is 1518. Note if RX_MAX_SIZE(max frame size received)
     * is greater than CNTMAXSIZEf, a good packet that is valid CRC and
     * contains no other errors, will increment the OVR(oversize) counters
     * if the length of the packet > CNTMAXSIZE < RX_MAX_SIZE values.
     * Having CNTMAXSIZE > RX_MAX_SIZE is not recommended.
     * This is generally taken care in the statistics module while
     * accumulating the counts.
     */
    _SOC_IF_ERR_EXIT(READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &rval));
    soc_reg_field_set(unit, CDMAC_MIB_COUNTER_CTRLr, &rval, CNTMAXSIZEf, val);
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_MIB_COUNTER_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

/*
 * portmod_port_cntmaxsize_get is caller of this function
 */
int cdmac_cntmaxsize_get(int unit, int port, int *val)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_MIB_COUNTER_CTRLr(unit, port, &rval));
    *val = soc_reg_field_get(unit, CDMAC_MIB_COUNTER_CTRLr, rval, CNTMAXSIZEf);

exit:
    SOC_FUNC_RETURN;
}

/*
 * This function controls which RSV(Receive statistics vector) event
 * causes a purge event that triggers RXERR to be set for the packet
 * sent by the MAC to the IP. These bits are used to mask RSV[34:16]
 * for CDMAC; bit[18] of MASK maps to bit[34] of RSV, bit[0] of MASK
 * maps to bit[16] of RSV.
 */
int cdmac_rsv_mask_set(int unit, int port, uint32 flags)
{

    int mask = 0;
    uint32 rsv_mask = 0;
    SOC_INIT_FUNC_DEFS;

    if (flags > CDMAC_RSV_MASK_INVALID_VAL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("invalid mask %x"), flags));
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_RSV_MASKr(unit, port, &rsv_mask));
    for(mask = 0; mask <= CDMAC_RSV_MASK_MAX; mask++) {
        if (flags & mask) {
            SHR_BITSET(&rsv_mask, mask);
        }
    }
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RSV_MASKr(unit, port, rsv_mask));

exit:
   SOC_FUNC_RETURN;
}

int cdmac_rsv_mask_clear(int unit, int port, uint32 flags)
{

    int mask = 0;
    uint32 rsv_mask = 0;
    SOC_INIT_FUNC_DEFS;

    if (flags > CDMAC_RSV_MASK_INVALID_VAL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("invalid mask %x"), flags));
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_RSV_MASKr(unit, port, &rsv_mask));
    for(mask = 0; mask <= CDMAC_RSV_MASK_MAX; mask++) {
        if (flags & mask) {
            SHR_BITCLR(&rsv_mask, mask);
        }
    }
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_RSV_MASKr(unit, port, rsv_mask));

exit:
   SOC_FUNC_RETURN;
}

int cdmac_rsv_mask_get(int unit, int port, uint32 flags, int *rsv_mask)
{

    int mask = 0;
    uint32 rval = 0;
    SOC_INIT_FUNC_DEFS;

    if (flags > CDMAC_RSV_MASK_INVALID_VAL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                           (_SOC_MSG("invalid mask %x"), flags));
    }

    _SOC_IF_ERR_EXIT(READ_CDMAC_RSV_MASKr(unit, port, &rval));
    for(mask = 0; mask <= CDMAC_RSV_MASK_MAX; mask++) {
        if (flags & mask) {
            if(SHR_BITGET(&rval, mask)) {
               SHR_BITSET(rsv_mask, mask);
            }
        }
    }

exit:
   SOC_FUNC_RETURN;
}

int cdmac_link_down_sequence_enable_set(int unit, int port, uint32 value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
    /*soc_reg_field_set(unit, CDMAC_CTRLr, &rval, MAC_LINK_DOWN_SEQ_ENf,
                      (value)? 1: 0);*/
    _SOC_IF_ERR_EXIT(WRITE_CDMAC_CTRLr(unit, port, rval));

exit:
    SOC_FUNC_RETURN;
}

int cdmac_link_down_sequence_enable_get(int unit, int port, uint32 *value)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CDMAC_CTRLr(unit, port, &rval));
    /* *value = soc_reg_field_get(unit, CDMAC_CTRLr, rval,
                               MAC_LINK_DOWN_SEQ_ENf);*/

exit:
    SOC_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

#endif
