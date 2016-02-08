/*
 * $Id: esw_eav.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * Ethernet AV BCM APIs
 */

#include <soc/drv.h>
#include <soc/mem.h>

#include <bcm/error.h>
#include <bcm/eav.h>

#include <bcm_int/esw/firebolt.h>

#include <bcm_int/esw_dispatch.h>

#define PHASE_FREQUENCY_ADJUSTMENT_MODE_SHIFT  20
#define PHASE_FREQUENCY_ADJUSTMENT_MODE_MASK   0x3
#define PHASE_FREQUENCY_ADJUSTMENT_ATTR_MASK   0xFFFFF

#define HK_BURST_GRANULARITY   64 /* 64bytes */
#define HK_BW_FIELD_MAX        0x3FFFF
#define BCM_EAV_CLASS_A_QUEUE  3
#define BCM_EAV_CLASS_B_QUEUE  2

STATIC bcm_mac_t TS_MAC = { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E };
STATIC uint16 TS_ETHERTYPE = 0x88F7;
STATIC bcm_mac_t MMRP_MAC = { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x20 };
STATIC uint16 MMRP_ETHERTYPE = 0x88F6;

/*
* Function:
*      bcm_eav_init
* Description:
*      Initialize the Residential Ethernet module and enable
*      the Ethernet AV (EAV) support.
* Parameters:
*      unit - device unit number.
* Returns:
*      BCM_E_XXX
*      
* Notes:
*      1. This function will enable the global EAV functionality
*      2. Decide the way to report egress timestamp info to CPU
*         Either loopback reporting packets
*         or CPU directly read register later.
*/
int 
bcm_esw_eav_init(int unit)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval, macaddr[2];

    /* Init ether type and DA of TS, SRP and MMRP. */
    if (SOC_IS_HAWKEYE(unit)) {

        SOC_IF_ERROR_RETURN(READ_TS_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, TS_CONTROLr, &rval, ETHERTYPEf, TS_ETHERTYPE);
        SOC_IF_ERROR_RETURN(WRITE_TS_CONTROLr(unit, rval));
        
        SAL_MAC_ADDR_TO_UINT32(TS_MAC, macaddr);
        SOC_IF_ERROR_RETURN(READ_TS_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, TS_CONTROL_1r, &rval, MAC_DA_LOWERf, macaddr[0]);
        SOC_IF_ERROR_RETURN(WRITE_TS_CONTROL_1r(unit, rval));
        SOC_IF_ERROR_RETURN(READ_TS_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, TS_CONTROL_2r, &rval, MAC_DA_UPPERf, macaddr[1]);
        SOC_IF_ERROR_RETURN(WRITE_TS_CONTROL_2r(unit, rval));
        
        SAL_MAC_ADDR_TO_UINT32(MMRP_MAC, macaddr);
        SOC_IF_ERROR_RETURN(READ_MMRP_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, MMRP_CONTROL_1r, &rval, MAC_DA_LOWERf, macaddr[0]);
        SOC_IF_ERROR_RETURN(WRITE_MMRP_CONTROL_1r(unit, rval));
        SOC_IF_ERROR_RETURN(READ_MMRP_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, MMRP_CONTROL_2r, &rval, MAC_DA_UPPERf, macaddr[1]);
        soc_reg_field_set(unit, MMRP_CONTROL_2r, &rval, ETHERTYPEf, MMRP_ETHERTYPE);
        SOC_IF_ERROR_RETURN(WRITE_MMRP_CONTROL_2r(unit, rval));

        /* Enable CPU 802.1as egress capability to get ingress timestamp. */
        BCM_IF_ERROR_RETURN(bcm_esw_port_control_set(unit, CMIC_PORT(unit),
                            bcmPortControlIEEE8021ASEnableEgress,1));
        return BCM_E_NONE;
    }
    return rv;
}

/*
* Function:
*      bcm_eav_port_enable_get
* Description:
*      Get enable status of per port Ethernet AV functionality
* Parameters:
*      unit - device unit number.
*      port - port number
*      enable - (OUT) TRUE, port is enabled for Ethernet AV
*                     FALSE, port is disabled for Ethernet AV
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_port_enable_get(int unit, bcm_port_t port, int *enable)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval;

    if (SOC_IS_HAWKEYE(unit)) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        SOC_IF_ERROR_RETURN(READ_EAV_ENABLE_BMAPr(unit, &rval));
        *enable = (rval & (1 << port))? TRUE:FALSE;
        return BCM_E_NONE;
    }
    return rv;
}

/*
* Function:
*      bcm_eav_port_enable_set
* Description:
*      Enable or disable per port Ethernet AV functionality
* Parameters:
*      unit - device unit number.
*      port - port number
*      enable - TRUE, port is enabled for Ethernet AV
*               FALSE, port is disabled for Ethernet AV
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
*
* Notes:
*      Need to disable the per port flow control
*/

int 
bcm_esw_eav_port_enable_set(int unit, bcm_port_t port, int enable)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval;

    if (SOC_IS_HAWKEYE(unit)) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        /* Enable EAV packets parsing */
        SOC_IF_ERROR_RETURN(READ_EAV_ENABLE_BMAPr(unit, &rval));
        if (enable) {
            rval |= (1 << port);
        } else {
            rval &= ~(1 << port);
        }
        SOC_IF_ERROR_RETURN(WRITE_EAV_ENABLE_BMAPr(unit, rval));
        
        /* Enable MMU */
        SOC_IF_ERROR_RETURN(WRITE_MMUEAVENABLEr(unit, rval));
        
        /* Program EGR_PORT register */
        SOC_IF_ERROR_RETURN(READ_EGR_PORTr(unit, port, &rval));
        soc_reg_field_set(unit, EGR_PORTr, &rval, EAV_CAPABLEf, enable);
        SOC_IF_ERROR_RETURN(WRITE_EGR_PORTr(unit, port, rval));
        
        /* Enable TS protocol packets parsing */
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_control_set(unit, port, 
                                     bcmPortControlIEEE8021ASEnableIngress, 
                                     enable));
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_control_set(unit, port,
                                     bcmPortControlIEEE8021ASEnableEgress,
                                     enable));
        /* Setup switch behavior for TS packets. */
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_port_set(unit, port,
                                            bcmSwitchTimeSyncPktDrop, enable));
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_port_set(unit, port,
                                            bcmSwitchTimeSyncPktToCpu, enable));
        return BCM_E_NONE;
    }
    return rv;
}

/*
* Function:
*      bcm_eav_link_status_get
* Description:
*      Get link status of per port Ethernet AV functionality
* Parameters:
*      unit - device unit number.
*      port - port number
*      link - (OUT) TRUE, Ethernet AV led is light on
*                     FALSE, Ethernet AV led is light off
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_link_status_get(int unit, bcm_port_t port, int *link)
{
    return BCM_E_UNAVAIL;
}

/*
* Function:
*      bcm_eav_link_status_set
* Description:
*      Set the EAV link status of  per port Ethernet AV functionality
* Parameters:
*      unit - device unit number.
*      port - port number
*      link - TRUE, Ethernet AV led is light on
*               FALSE, Ethernet AV led is light off
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
*
* Notes:
*/

int 
bcm_esw_eav_link_status_set(int unit, bcm_port_t port, int link)
{
    return BCM_E_UNAVAIL;
}

/*
* Function:
*      bcm_eav_control_get
* Description:
*      Get the configuration for specific type
* Parameters:
*      unit - device unit number.
*      type - configuration type
*      arg1 - (OUT) the pointer buffer to store the returned configuration
*      arg2 - (OUT) the pointer buffer to store the returned configuration
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_control_get(int unit, bcm_eav_control_t type, 
        uint32 *arg1, uint32 *arg2)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval;

    if (SOC_IS_HAWKEYE(unit)) {
        switch (type) {
            case bcmEAVControlTimeBase:
                SOC_IF_ERROR_RETURN(READ_CMIC_TIMESYNC_TIMERr(unit, arg1));
                return BCM_E_NONE;
            case bcmEAVControlTimeAdjust:
                SOC_IF_ERROR_RETURN(READ_CMIC_TIMESYNC_CONTROLr(unit, &rval));
                *arg1 = (rval >> PHASE_FREQUENCY_ADJUSTMENT_MODE_SHIFT) &
                         PHASE_FREQUENCY_ADJUSTMENT_MODE_MASK;
                *arg2 = (rval & PHASE_FREQUENCY_ADJUSTMENT_ATTR_MASK);
                return BCM_E_NONE;
            default:
                break;
        }
        return BCM_E_PARAM; 
    }
    return rv;
}

/*
* Function:
*      bcm_eav_control_set
* Description:
*      Set the configuration for specific type
* Parameters:
*      unit - device unit number.
*      type - configuration type
*      arg1 - the configuration data to set 
*      arg2 - the configuration data to set 
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_control_set(int unit, bcm_eav_control_t type, 
       uint32 arg1, uint32 arg2)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval = 0;
    if (SOC_IS_HAWKEYE(unit)) {
        switch (type) {
            case bcmEAVControlTimeAdjust:
                /* arg1 = mode, arg2 = phase/freq adjustment value */
                rval = (arg1 << PHASE_FREQUENCY_ADJUSTMENT_MODE_SHIFT) | 
                       (arg2 & PHASE_FREQUENCY_ADJUSTMENT_ATTR_MASK);
                switch (arg1) {
                    case BCM_EAV_TIMEREF_MODE_FREERUN:
                        rval = 0x0;
                    case BCM_EAV_TIMEREF_MODE_PHASE_ADJUSTMENT:
                    case BCM_EAV_TIMEREF_MODE_FREQ_ADJUMENT1:
                    case BCM_EAV_TIMEREF_MODE_FREQ_ADJUMENT2:
                        break;
                    default:
                    return BCM_E_PARAM;
                }
                SOC_IF_ERROR_RETURN(WRITE_CMIC_TIMESYNC_CONTROLr(unit, rval));
                break;
            default:
                return BCM_E_PARAM;
        }
        return BCM_E_NONE; 
    }
    return rv;
}

/*
* Function:
*      bcm_eav_egress_timestamp_get
* Description:
*      Get the per port egress timestamp value of TS protocol packets
* Parameters:
*      unit - device unit number
*      port - port number
*      timestamp - (OUT) the pointer buffer to store the returned timestamp  
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_timestamp_get(int unit, bcm_port_t port, uint32 *timestamp)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval, error;

    if (SOC_IS_HAWKEYE(unit)) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        SOC_IF_ERROR_RETURN(READ_TS_STATUS_CNTRLr(unit, port, &rval));
        error = soc_reg_field_get(unit, TS_STATUS_CNTRLr, rval, TX_TS_FIFO_EMPTYf);
        if (error) {
            return BCM_E_EMPTY;
        }
        SOC_IF_ERROR_RETURN(READ_TX_TS_DATAr(unit, port, timestamp));
        return BCM_E_NONE; 
    }
    return rv;
}

/*
* Function:
*      bcm_eav_timesync_mac_get
* Description:
*      Get the Mac address of Time Sync protocol
* Parameters:
*      unit - device unit number
*      eav_mac - the pointer buffer to restorm the mac addrss  
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_timesync_mac_get(int unit, bcm_mac_t eav_mac)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval[2];
    if (SOC_IS_HAWKEYE(unit)) {
        SOC_IF_ERROR_RETURN(READ_TS_CONTROL_1r(unit, &rval[0]));
        SOC_IF_ERROR_RETURN(READ_TS_CONTROL_2r(unit, &rval[1]));
        SAL_MAC_ADDR_FROM_UINT32(eav_mac, rval);
        return BCM_E_NONE; 
    }
    return rv;
}

/*
* Function:
*      bcm_eav_timesync_mac_set
* Description:
*      Set the Mac address of Time Sync protocol
* Parameters:
*      unit - device unit number
*      eav_mac - the pointer buffer to restorm the mac addrss  
* Returns:
*      BCM_E_NONE
*      BCM_E_XXX
* Notes:
*/

int 
bcm_esw_eav_timesync_mac_set(int unit, bcm_mac_t eav_mac)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval, macaddr[2];
    if (SOC_IS_HAWKEYE(unit)) {
        SAL_MAC_ADDR_TO_UINT32(eav_mac, macaddr);
        SOC_IF_ERROR_RETURN(READ_TS_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, TS_CONTROL_1r, &rval, MAC_DA_LOWERf, macaddr[0]);
        SOC_IF_ERROR_RETURN(WRITE_TS_CONTROL_1r(unit, rval));
        SOC_IF_ERROR_RETURN(READ_TS_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, TS_CONTROL_2r, &rval, MAC_DA_UPPERf, macaddr[1]);
        SOC_IF_ERROR_RETURN(WRITE_TS_CONTROL_2r(unit, rval));
        return BCM_E_NONE; 
    }
    return rv;
}

/*
 * Function:
 *      bcm_eav_srp_mac_ethertype_set
 * Description:
 *      Get the Mac address and Ethertype used to trap SRP protocol packets
 * Parameters:
 *      unit - device unit number
 *      mac  - the mac addrss   
 *      ethertype - the EtherType
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_srp_mac_ethertype_set(int unit, bcm_mac_t mac, bcm_port_ethertype_t ethertype)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval, macaddr[2];
    if (SOC_IS_HAWKEYE(unit)) {
        SAL_MAC_ADDR_TO_UINT32(mac, macaddr);
        SOC_IF_ERROR_RETURN(READ_SRP_CONTROL_1r(unit, &rval));
        soc_reg_field_set(unit, SRP_CONTROL_1r, &rval, MAC_DA_LOWERf, macaddr[0]);
        SOC_IF_ERROR_RETURN(WRITE_SRP_CONTROL_1r(unit, rval));
        SOC_IF_ERROR_RETURN(READ_SRP_CONTROL_2r(unit, &rval));
        soc_reg_field_set(unit, SRP_CONTROL_2r, &rval, MAC_DA_UPPERf, macaddr[1]);
        soc_reg_field_set(unit, SRP_CONTROL_2r, &rval, ETHERTYPEf, ethertype);
        SOC_IF_ERROR_RETURN(WRITE_SRP_CONTROL_2r(unit, rval));
        return BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *      bcm_eav_srp_mac_ethertype_get
 * Description:
 *      Get the Mac address and Ethertype used to trap SRP protocol packets
 * Parameters:
 *      unit - device unit number
 *      mac  - (OUT)the mac addrss   
 *      ethertype - (OUT)the EtherType
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */                               
int 
bcm_esw_eav_srp_mac_ethertype_get(int unit, bcm_mac_t mac, bcm_port_ethertype_t *ethertype)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval;
    if (SOC_IS_HAWKEYE(unit)) {
        SOC_IF_ERROR_RETURN(READ_SRP_CONTROL_1r(unit, &rval));
        mac[2] = (rval >> 24) & 0xFF;
        mac[3] = (rval >> 16) & 0xFF;
        mac[4] = (rval >> 8) & 0xFF;
        mac[5] = (rval & 0xFF);
        SOC_IF_ERROR_RETURN(READ_SRP_CONTROL_2r(unit, &rval));
        mac[0] = (rval >> 24) & 0xFF;
        mac[1] = (rval >> 16) & 0xFF;
        *ethertype = soc_reg_field_get(unit, SRP_CONTROL_2r, rval, ETHERTYPEf);
        return BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *      bcm_eav_pcp_mapping_set
 * Description:
 *      Set the PCP(priority) value mapping for each EAV class packets
 * Parameters:
 *      unit - device unit number
 *      type - Class A or Class B stream
 *      pcp  - Priority for the Class   
 *      rempapped_pcp - For NonEAV traffic with PCP=ClassA_PCP or ClassB_PCP
 *      exiting through an egress port configured in EAV mode must be remapped
 *      to another pcp.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_pcp_mapping_set(
    int unit, 
    bcm_eav_stream_class_t type, 
    int pcp, 
    int remapped_pcp)
{
    int rv = BCM_E_UNAVAIL;
    int    field_width;
    uint32 rval, max_value;

    if (SOC_IS_HAWKEYE(unit)) {
        field_width = soc_reg_field_length(unit, ING_EAV_CLASSr, CLASS_Af);
        max_value = (1 << field_width) - 1;
        if (pcp < 0 || pcp > max_value || 
            remapped_pcp < 0 || remapped_pcp > max_value) {
            return BCM_E_PARAM;
        }
    
        SOC_IF_ERROR_RETURN(READ_ING_EAV_CLASSr(unit, &rval));
        if (type == bcmEAVCStreamClassA) {
            soc_reg_field_set(unit, ING_EAV_CLASSr, &rval, CLASS_Af, pcp);
        } else if (type == bcmEAVCStreamClassB) {
            soc_reg_field_set(unit, ING_EAV_CLASSr, &rval, CLASS_Bf, pcp);
        } else {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(WRITE_ING_EAV_CLASSr(unit, rval));
    
        /* Updaet EGR_EAV_CLASS register */
        SOC_IF_ERROR_RETURN(READ_EGR_EAV_CLASSr(unit, &rval));
        if (type == bcmEAVCStreamClassA) {
            soc_reg_field_set(unit, EGR_EAV_CLASSr, &rval, CLASS_Af, pcp);
            soc_reg_field_set(unit, EGR_EAV_CLASSr, &rval, REMAP_CLASS_Af, remapped_pcp);
        } else if (type == bcmEAVCStreamClassB) {
            soc_reg_field_set(unit, EGR_EAV_CLASSr, &rval, CLASS_Bf, pcp);
            soc_reg_field_set(unit, EGR_EAV_CLASSr, &rval, REMAP_CLASS_Bf, remapped_pcp);
        }
        SOC_IF_ERROR_RETURN(WRITE_EGR_EAV_CLASSr(unit, rval));
        return BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *      bcm_esw_eav_pcp_mapping_get
 * Description:
 *      Set the PCP(priority) value mapping for each EAV class packets
 * Parameters:
 *      unit - device unit number
 *      type - Class A or Class B stream
 *      pcp  - Priority for the Class   
 *      rempapped_pcp - For NonEAV traffic with PCP=ClassA_PCP or ClassB_PCP
 *      exiting through an egress port configured in EAV mode must be remapped
 *      to another pcp.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_pcp_mapping_get(
    int unit, 
    bcm_eav_stream_class_t type, 
    int *pcp, 
    int *remapped_pcp)
{
    int rv = BCM_E_UNAVAIL;
    uint32 rval;
    
    if (SOC_IS_HAWKEYE(unit)) {
        /* Read EGR_EAV_CLASS register */
        SOC_IF_ERROR_RETURN(READ_EGR_EAV_CLASSr(unit, &rval));
        if (type == bcmEAVCStreamClassA) {
            *pcp = (int)soc_reg_field_get(unit, EGR_EAV_CLASSr, rval, CLASS_Af);
            *remapped_pcp = (int)soc_reg_field_get(unit, EGR_EAV_CLASSr, rval, REMAP_CLASS_Af);
        } else if (type == bcmEAVCStreamClassB) {
            *pcp = (int)soc_reg_field_get(unit, EGR_EAV_CLASSr, rval, CLASS_Bf);
            *remapped_pcp = (int)soc_reg_field_get(unit, EGR_EAV_CLASSr, rval, REMAP_CLASS_Bf);
        } else {
            return BCM_E_PARAM;
        }
        return BCM_E_NONE;
    }
    return rv;
}

/*
 * Function:
 *      bcm_esw_eav_bandwidth_set
 * Description:
 *      Set the reserved bandwidth for Class A or B stream traffic
 * Parameters:
 *      unit - device unit number
 *      port - port number   
 *      type - Class A or Class B stream
 *      bytes_sec - maximum bytes per second.
 *      bytes_burst - maximum burst size in bytes. 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_bandwidth_set(
    int unit, 
    bcm_port_t port, 
    bcm_eav_stream_class_t type, 
    uint32 bytes_sec,
    uint32 bytes_burst)
{
	int rv = BCM_E_UNAVAIL;
#if defined(BCM_HAWKEYE_SUPPORT)
    uint32 regval, kbits_sec_max;
    uint32 bucket_val, burst_size;
    bcm_cos_queue_t cosq;
    int    refresh_bitsize, bucket_bitsize;
    uint32 refresh_rate;
    uint32 bucketsize, granularity = 3, meter_flags = 0;

    if (SOC_IS_HAWKEYE(unit)) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        if (type == bcmEAVCStreamClassA) {
            cosq = BCM_EAV_CLASS_A_QUEUE;
        } else if (type == bcmEAVCStreamClassB) {
            cosq = BCM_EAV_CLASS_B_QUEUE;
        } else {
            return BCM_E_PARAM;
        }
        
        /* disable ingress policing and egress metering */
        if (bytes_sec) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_rate_ingress_set(unit, port, 0, 0));
            BCM_IF_ERROR_RETURN(bcm_esw_port_rate_egress_set(unit, port, 0, 0));
        }
        
        /*
         * To set the new Bandwidth settings, the procedure adopted is
         * a. reset MAXBUCKETCONFIG, MINBUCKETCONFIG, MAXBUCKET,MINBUCKET
         * b. update MAXBUCKETCONFIG and MINBUCKETCONFIG with new values passed
         */

        SOC_IF_ERROR_RETURN(READ_MAXBUCKETCONFIGr(unit, port, cosq, &regval));

        /* Disable egress metering for this port */
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval, MAX_REFRESHf, 0);
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval, MAX_THD_SELf, 0);
        SOC_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIGr(unit, port, cosq, regval));
        
        regval = 0;
        if (type == bcmEAVCStreamClassA) {
            SOC_IF_ERROR_RETURN(WRITE_MINBUCKETCOS3CONFIGr(unit, port, regval));
        } else {
            SOC_IF_ERROR_RETURN(WRITE_MINBUCKETCOS2CONFIGr(unit, port, regval));
        }
        
        /* reset the MAXBUCKET register*/
        bucket_val = 0;
        soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, MAX_BUCKETf,0);
        soc_reg_field_set(unit, MAXBUCKETr, &bucket_val, IN_PROFILE_FLAGf,1);
        SOC_IF_ERROR_RETURN(WRITE_MAXBUCKETr(unit, port, cosq, bucket_val));

        /* reset the MINBUCKET register value*/
        soc_reg_field_set(unit, MINBUCKETr, &bucket_val, MIN_BUCKETf,0);
        soc_reg_field_set(unit, MINBUCKETr, &bucket_val, IN_PROFILE_FLAGf,0);
        SOC_IF_ERROR_RETURN(WRITE_MINBUCKETr(unit, port, cosq, bucket_val));
        
        kbits_sec_max = bytes_sec*8/1000;
        refresh_bitsize =
                soc_reg_field_length(unit, MAXBUCKETCONFIGr, MAX_REFRESHf);
        bucket_bitsize = 18;
        
        /* Use existing code to calculate bucket size */ 
        BCM_IF_ERROR_RETURN
                (_bcm_xgs_kbits_to_bucket_encoding(kbits_sec_max,
                                                   kbits_sec_max,
                          meter_flags, refresh_bitsize, bucket_bitsize,
                          &refresh_rate, &bucketsize, &granularity));

        /* regval has MAXBUCKETCONFIGr value */
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval,
                              MAX_REFRESHf, refresh_rate);
        
        /* Increment for EAV bucket is 64 bytes */
        burst_size = (bytes_burst + (HK_BURST_GRANULARITY - 1)) / HK_BURST_GRANULARITY;
        if (burst_size > HK_BW_FIELD_MAX) {
                burst_size = HK_BW_FIELD_MAX;
        }
        /* regval has MAXBUCKETCONFIGr value */
        soc_reg_field_set(unit, MAXBUCKETCONFIGr, &regval,
                              MAX_THD_SELf, (burst_size >> 6) & 0xFFF);
        
        SOC_IF_ERROR_RETURN(WRITE_MAXBUCKETCONFIGr(unit, port, cosq, regval));
        
        SOC_IF_ERROR_RETURN(READ_EAVBUCKETCONFIG_EXTr(unit, port, &regval));
        soc_reg_field_set(unit, EAVBUCKETCONFIG_EXTr, &regval,
                              (type == bcmEAVCStreamClassA)?CLASSA_THD_SEL_6LSBf:
                              CLASSB_THD_SEL_6LSBf, burst_size & 0x3f);
        SOC_IF_ERROR_RETURN(WRITE_EAVBUCKETCONFIG_EXTr(unit, port, regval));
        
        return BCM_E_NONE;
    }
#endif /* BCM_HAWKEYE_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_esw_eav_bandwidth_get
 * Description:
 *      Get the reserved bandwidth for Class A or B stream traffic
 * Parameters:
 *      unit - device unit number
 *      port - port number   
 *      type - Class A or Class B stream
 *      bytes_sec - bytes per second.
 *      bytes_burst - max burst bytes.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_esw_eav_bandwidth_get(
    int unit, 
    bcm_port_t port, 
    bcm_eav_stream_class_t type, 
    uint32 *bytes_sec,
    uint32 *bytes_burst)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_HAWKEYE_SUPPORT)
    uint32 regval, refresh_max, kbits_sec_max, kbits_burst;
    uint32 burst_hi, burst_lo;
    bcm_cos_queue_t cosq;
    uint32 granularity = 3, meter_flags = 0;

    if (SOC_IS_HAWKEYE(unit)) {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
        if (type == bcmEAVCStreamClassA) {
            cosq = BCM_EAV_CLASS_A_QUEUE;
        } else if (type == bcmEAVCStreamClassB) {
            cosq = BCM_EAV_CLASS_B_QUEUE;
        } else {
            return BCM_E_PARAM;
        }
        
        SOC_IF_ERROR_RETURN(READ_MAXBUCKETCONFIGr(unit, port, cosq, &regval));
        refresh_max = 
            soc_reg_field_get(unit, MAXBUCKETCONFIGr, regval, MAX_REFRESHf);
        BCM_IF_ERROR_RETURN
                (_bcm_xgs_bucket_encoding_to_kbits(refresh_max, 0,
                                                   granularity,
                                                   meter_flags,
                                                   &kbits_sec_max,
                                                   &kbits_burst));
        *bytes_sec = (kbits_sec_max*1000)/8;
        burst_hi = soc_reg_field_get(unit, MAXBUCKETCONFIGr, regval, MAX_THD_SELf);
        SOC_IF_ERROR_RETURN(READ_EAVBUCKETCONFIG_EXTr(unit, port, &regval));
        if (type == bcmEAVCStreamClassA) {
            burst_lo = soc_reg_field_get(unit, EAVBUCKETCONFIG_EXTr, regval, CLASSA_THD_SEL_6LSBf);
        } else {
            burst_lo = soc_reg_field_get(unit, EAVBUCKETCONFIG_EXTr, regval, CLASSB_THD_SEL_6LSBf);
        }
        *bytes_burst = ((burst_hi << 6) | burst_lo)*HK_BURST_GRANULARITY;
        return BCM_E_NONE;
    }
#endif /* BCM_HAWKEYE_SUPPORT */
    return rv;
}

