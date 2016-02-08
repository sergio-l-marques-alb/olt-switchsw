/*
 * $Id: eav.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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

#include <bcm/error.h>
#include <bcm/eav.h>
#include <soc/drv.h>
#include <soc/debug.h>



static int
_bcm_robo_eav_mmu_init(int unit)
{
    uint32 reg_val;

    /* 1. MMU settings provided by ASIC*/
    /* Hysteresis threshold */
    reg_val = 110; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa10, &reg_val, 2);
    reg_val = 111;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa12, &reg_val, 2);
    reg_val = 111;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa14, &reg_val, 2);
    reg_val = 112;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa16, &reg_val, 2);
    reg_val = 112;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xab0, &reg_val, 2);
    reg_val = 112;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xab2, &reg_val, 2);

    /* Pause threshold */
    reg_val = 232; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa18, &reg_val, 2);
    reg_val = 233;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa1a, &reg_val, 2);
    reg_val = 234;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa1c, &reg_val, 2);
    reg_val = 235;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa1e, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xab4, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xab6, &reg_val, 2);

    /* Drop threshold */
    reg_val = 500; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa20, &reg_val, 2);
    reg_val = 500;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa22, &reg_val, 2);
    reg_val = 500;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa24, &reg_val, 2);
    reg_val = 500;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa26, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xab8, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xaba, &reg_val, 2);

    /* Total reserved threshold */
    reg_val = 1; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa40, &reg_val, 2);
    reg_val = 1;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa42, &reg_val, 2);
    reg_val = 1;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa44, &reg_val, 2);
    reg_val = 1;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa46, &reg_val, 2);
    reg_val = 18;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa48, &reg_val, 2);
    reg_val = 24;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa4a, &reg_val, 2);

    /* IMP Hysteresis threshold */
    reg_val = 122; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd10, &reg_val, 2);
    reg_val = 123;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd12, &reg_val, 2);
    reg_val = 123;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd14, &reg_val, 2);
    reg_val = 124;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd16, &reg_val, 2);

    /* IMP Pause threshold */
    reg_val = 244; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd18, &reg_val, 2);
    reg_val = 245;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd1a, &reg_val, 2);
    reg_val = 246;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd1c, &reg_val, 2);
    reg_val = 247;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd1e, &reg_val, 2);

    /* IMP Drop threshold */
    reg_val = 511; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd20, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd22, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd24, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd26, &reg_val, 2);

    /* Total Hysteresis threshold */
    reg_val = 108; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa28, &reg_val, 2);
    reg_val = 109;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa2a, &reg_val, 2);
    reg_val = 110;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa2c, &reg_val, 2);
    reg_val = 111;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa2e, &reg_val, 2);
    reg_val = 111;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xabc, &reg_val, 2);
    reg_val = 111;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xabe, &reg_val, 2);

    /* Total Pause threshold */
    reg_val = 246; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa30, &reg_val, 2);
    reg_val = 248;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa32, &reg_val, 2);
    reg_val = 250;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa34, &reg_val, 2);
    reg_val = 252;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa36, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xac0, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xac2, &reg_val, 2);

    /* Total Drop threshold */
    reg_val = 378; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa38, &reg_val, 2);
    reg_val = 380;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa3a, &reg_val, 2);
    reg_val = 382;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa3c, &reg_val, 2);
    reg_val = 384;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xa3e, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xac4, &reg_val, 2);
    reg_val = 511;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xac6, &reg_val, 2);

    /* Total IMP Hysteresis threshold */
    reg_val = 138; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd28, &reg_val, 2);
    reg_val = 139;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd2a, &reg_val, 2);
    reg_val = 140;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd2c, &reg_val, 2);
    reg_val = 141;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd2e, &reg_val, 2);

    /* Total IMP Pause threshold */
    reg_val = 276; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd30, &reg_val, 2);
    reg_val = 278;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd32, &reg_val, 2);
    reg_val = 280;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd34, &reg_val, 2);
    reg_val = 282;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd36, &reg_val, 2);

    /* Total IMP Drop threshold */
    reg_val = 408; 
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd38, &reg_val, 2);
    reg_val = 410;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd3a, &reg_val, 2);
    reg_val = 412;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd3c, &reg_val, 2);
    reg_val = 414;
    (DRV_SERVICES(unit)->reg_write)
        (unit, 0xd3e, &reg_val, 2);

    return BCM_E_NONE;
}

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
bcm_robo_eav_init(int unit)
{
    int rv = BCM_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    
    /* 1. MMU settings provided by ASIC*/
    _bcm_robo_eav_mmu_init(unit);

    /* 2. Enable time stamped to IMP port */
    rv = ((DRV_SERVICES(unit)->eav_control_set)
        (unit, DRV_EAV_CONTROL_TIME_STAMP_TO_IMP, 1));

    if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR, 
            "EAV Error: failure in bcm_robo_eav_init()\n");
        return rv;
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
bcm_robo_eav_port_enable_get(int unit, bcm_port_t port, int *enable)
{
    int rv = BCM_E_NONE;
    uint32  temp;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) { 
        return BCM_E_PORT; 
    }
    
    rv = (DRV_SERVICES(unit)->eav_enable_get)(unit, port, &temp);

    if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR, 
            "EAV Error: failure in bcm_robo_eav_port_enable_get()\n");
        return rv;
    }
    if (temp) {
        *enable = TRUE;
    } else {
        *enable = FALSE;
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
bcm_robo_eav_port_enable_set(int unit, bcm_port_t port, int enable)
{
    int rv = BCM_E_NONE;
    uint32  temp;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) { 
        return BCM_E_PORT; 
    }
    
    if (enable) {
        temp = 1;
    } else {
        temp = 0;
    }
    
    rv = (DRV_SERVICES(unit)->eav_enable_set)(unit, port, temp);

    if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR, 
            "EAV Error: failure in bcm_robo_eav_port_enable_set()\n");
        return rv;
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
bcm_robo_eav_link_status_get(int unit, bcm_port_t port, int *link)
{
    int rv = BCM_E_NONE;
    uint32  temp;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) { 
        return BCM_E_PORT; 
    }
    
    rv = (DRV_SERVICES(unit)->eav_link_status_get)(unit, port, &temp);

    if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR, 
            "EAV Error: failure in bcm_robo_eav_link_status_get()\n");
        return rv;
    }
    if (temp) {
        *link = TRUE;
    } else {
        *link = FALSE;
    }

    return rv;
    
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
bcm_robo_eav_link_status_set(int unit, bcm_port_t port, int link)
{
    int rv = BCM_E_NONE;
    uint32  temp;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) { 
        return BCM_E_PORT; 
    }
    
    if (link) {
        temp = 1;
    } else {
        temp = 0;
    }
    
    rv = (DRV_SERVICES(unit)->eav_link_status_set)(unit, port, temp);

    if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR, 
            "EAV Error: failure in bcm_robo_eav_link_status_set()\n");
        return rv;
    }

    return rv;
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
bcm_robo_eav_control_get(int unit, bcm_eav_control_t type, 
        uint32 *arg1, uint32 *arg2)
{
    int rv = BCM_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    switch (type) {
        case bcmEAVControlMaxFrameSize:
            rv = (DRV_SERVICES(unit)->eav_control_get)
                (unit, DRV_EAV_CONTROL_MAX_AV_SIZE, arg1);
            break;
        case bcmEAVControlTimeBase:
            rv = (DRV_SERVICES(unit)->eav_time_sync_get)
                (unit, DRV_EAV_TIME_SYNC_TIME_BASE, arg1, arg2);
            break;
        case bcmEAVControlTimeAdjust:
            rv = (DRV_SERVICES(unit)->eav_time_sync_get)
                (unit, DRV_EAV_TIME_SYNC_TIME_ADJUST, arg1, arg2);
            break;
        case bcmEAVControlTickCounter:
            rv = (DRV_SERVICES(unit)->eav_time_sync_get)
                (unit, DRV_EAV_TIME_SYNC_TICK_COUNTER, arg1, arg2);
            break;
        case bcmEAVControlSlotNumber:
            rv = (DRV_SERVICES(unit)->eav_time_sync_get)
                (unit, DRV_EAV_TIME_SYNC_SLOT_NUMBER, arg1, arg2);
            break;
        case bcmEAVControlMacroSlotTime:
            rv = (DRV_SERVICES(unit)->eav_time_sync_get)
                (unit, DRV_EAV_TIME_SYNC_MACRO_SLOT_PERIOD, arg1, arg2);
            break;
         case bcmEAVControlSlotAdjust:
            rv = (DRV_SERVICES(unit)->eav_time_sync_get)
                (unit, DRV_EAV_TIME_SYNC_SLOT_ADJUST, arg1, arg2);
            break;
        case bcmEAVControlStreamClassAPCP:
            rv = (DRV_SERVICES(unit)->eav_control_get)
                (unit, DRV_EAV_CONTROL_STREAM_CLASSA_PCP, arg1);
            break;
        case bcmEAVControlStreamClassBPCP:
            rv = (DRV_SERVICES(unit)->eav_control_get)
                (unit, DRV_EAV_CONTROL_STREAM_CLASSB_PCP, arg1);
            break;
        default:
            return BCM_E_PARAM;
    }
    if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR, 
            "EAV Error: failure in bcm_eav_control_get()\n");
        return rv;
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
bcm_robo_eav_control_set(int unit, bcm_eav_control_t type, 
       uint32 arg1, uint32 arg2)
{
    int rv = BCM_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    
    switch (type) {
        case bcmEAVControlMaxFrameSize:
            rv = (DRV_SERVICES(unit)->eav_control_set)
                (unit, DRV_EAV_CONTROL_MAX_AV_SIZE, arg1);
            break;
        case bcmEAVControlTimeBase:
            rv = (DRV_SERVICES(unit)->eav_time_sync_set)
                (unit, DRV_EAV_TIME_SYNC_TIME_BASE, arg1, arg2);
            break;
        case bcmEAVControlTimeAdjust:
            rv = (DRV_SERVICES(unit)->eav_time_sync_set)
                (unit, DRV_EAV_TIME_SYNC_TIME_ADJUST, arg1, arg2);
            break;
        case bcmEAVControlTickCounter:
            rv = (DRV_SERVICES(unit)->eav_time_sync_set)
                (unit, DRV_EAV_TIME_SYNC_TICK_COUNTER, arg1, arg2);
            break;
        case bcmEAVControlSlotNumber:
            rv = (DRV_SERVICES(unit)->eav_time_sync_set)
                (unit, DRV_EAV_TIME_SYNC_SLOT_NUMBER, arg1, arg2);
            break;
        case bcmEAVControlMacroSlotTime:
            rv = (DRV_SERVICES(unit)->eav_time_sync_set)
                (unit, DRV_EAV_TIME_SYNC_MACRO_SLOT_PERIOD, arg1, arg2);
            break;
         case bcmEAVControlSlotAdjust:
            rv = (DRV_SERVICES(unit)->eav_time_sync_set)
                (unit, DRV_EAV_TIME_SYNC_SLOT_ADJUST, arg1, arg2);
            break;
         case bcmEAVControlStreamClassAPCP:
            rv = (DRV_SERVICES(unit)->eav_control_set)
                (unit, DRV_EAV_CONTROL_STREAM_CLASSA_PCP, arg1);
            break;
         case bcmEAVControlStreamClassBPCP:
            rv = (DRV_SERVICES(unit)->eav_control_set)
                (unit, DRV_EAV_CONTROL_STREAM_CLASSB_PCP, arg1);
            break;
        default:
            return BCM_E_PARAM;
    }
    if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR, 
            "EAV Error: failure in bcm_eav_control_set()\n");
        return rv;
    }

    return rv;
}

/*
* Function:
*      bcm_eav_egress_timestamp_get
* Description:
*      Get the per port egress timestamp value
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
bcm_robo_eav_timestamp_get(int unit, bcm_port_t port, uint32 *timestamp)
{
    int rv = BCM_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) { 
        return BCM_E_PORT; 
    }
    
     rv = (DRV_SERVICES(unit)->eav_egress_timestamp_get)
        (unit, port, timestamp);

     if (BCM_FAILURE(rv)) {
        soc_cm_debug(DK_ERR, 
            "EAV Error: failure in bcm_eav_timestamp_get()\n");
        return rv;
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
bcm_robo_eav_timesync_mac_get(int unit, bcm_mac_t eav_mac)
{
    uint16  ethertype;
    int rv = BCM_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    rv = (DRV_SERVICES(unit)->eav_time_sync_mac_get)
        (unit, eav_mac, &ethertype);
    
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
bcm_robo_eav_timesync_mac_set(int unit, bcm_mac_t eav_mac)
{

    uint16  ethertype;
    int rv = BCM_E_NONE;

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    ethertype = 0; /* ignore ethtype value */
    rv = (DRV_SERVICES(unit)->eav_time_sync_mac_set)
        (unit, eav_mac, ethertype);
    
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
bcm_robo_eav_srp_mac_ethertype_set(int unit, bcm_mac_t mac, bcm_port_ethertype_t ethertype)
{
    uint32 reg_addr, reg_val;
    uint64 reg_val64, mac_field;
    uint32 temp;
    int rv =  BCM_E_NONE, reg_len;

    if (!soc_feature(unit, soc_feature_eav_support)) {
        return BCM_E_UNAVAIL;
    }

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    /*
     * For srp, the mac should be set in Multi-address 1 register
     */

    /* 1. Set MAC and Ethertype value */
    SAL_MAC_ADDR_TO_UINT64(mac, mac_field);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MULTIPORT_ADDR1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MULTIPORT_ADDR1r);

    COMPILER_64_ZERO(reg_val64);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                    (unit, MULTIPORT_ADDR1r, (uint32 *)&reg_val64, 
                        MPORT_ADDRf, (uint32 *)&mac_field));
    
    temp = ethertype;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                    (unit, MULTIPORT_ADDR1r, (uint32 *)&reg_val64, 
                        MPORT_E_TYPEf, &temp));
    
    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, (uint32 *)&reg_val64, reg_len)) < 0) {
        return rv;
    }

    /* 2. Set Forward map to CPU only */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MPORTVEC1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MPORTVEC1r);
    temp  = SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0);
    reg_val = 0;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, MPORTVEC1r, &reg_val, PORT_VCTRf, &temp));
    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_val, reg_len)) < 0) {
        return rv;
    }

    /* 3. Enable Multi-address o */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MULTI_PORT_CTLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MULTI_PORT_CTLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_val, reg_len)) < 0) {
        return rv;
    }
    /* Set the match condition are MAC/Ethertype */
    temp = DRV_MULTIPORT_CTRL_MATCH_ETYPE_ADDR;
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, MULTI_PORT_CTLr, &reg_val, MPORT_CTRL1f, &temp));

    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_val, reg_len)) < 0) {
        return rv;
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
bcm_robo_eav_srp_mac_ethertype_get(int unit, bcm_mac_t mac, bcm_port_ethertype_t *ethertype)
{
    uint32 reg_addr, reg_val;
    uint64 reg_val64, mac_field;
    uint32 temp;
    int rv =  BCM_E_NONE, reg_len;

    if (!soc_feature(unit, soc_feature_eav_support)) {
        return BCM_E_UNAVAIL;
    }

    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MULTI_PORT_CTLr, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MULTI_PORT_CTLr);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_val, reg_len)) < 0) {
        return rv;
    }
    /* Get the Multi-address control value */
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, MULTI_PORT_CTLr, &reg_val, MPORT_CTRL1f, &temp));
    if (temp == 0) {
        rv = SOC_E_DISABLED;
        return rv;
    }

    /* Get the MAC and Ethertype value */
    reg_addr = (DRV_SERVICES(unit)->reg_addr)
                (unit, MULTIPORT_ADDR1r, 0, 0);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)
                (unit, MULTIPORT_ADDR1r);

    COMPILER_64_ZERO(reg_val64);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, (uint32 *)&reg_val64, reg_len)) < 0) {
        return rv;
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                    (unit, MULTIPORT_ADDR1r, (uint32 *)&reg_val64, 
                        MPORT_ADDRf, (uint32 *)&mac_field));
    SAL_MAC_ADDR_FROM_UINT64(mac, mac_field);
   
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
                    (unit, MULTIPORT_ADDR1r, (uint32 *)&reg_val64, 
                        MPORT_E_TYPEf, &temp));
    *ethertype = temp;

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
bcm_robo_eav_pcp_mapping_set(
    int unit, 
    bcm_eav_stream_class_t type, 
    int pcp, 
    int remapped_pcp)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_eav_pcp_mapping_get
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
bcm_robo_eav_pcp_mapping_get(
    int unit, 
    bcm_eav_stream_class_t type, 
    int *pcp, 
    int *remapped_pcp)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_eav_bandwidth_set
 * Description:
 *      Set the reserved bandwidth for Class A or B stream traffic
 * Parameters:
 *      unit - device unit number
 *      port - port number   
 *      type - Class A or Class B stream
 *      bytes_sec - bytes per second.
 *      bytes_burst - maximum burst size in bytes.  
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_robo_eav_bandwidth_set(
    int unit, 
    bcm_port_t port, 
    bcm_eav_stream_class_t type, 
    uint32 bytes_sec,
    uint32 bytes_burst)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_eav_bandwidth_get
 * Description:
 *      Get the reserved bandwidth for Class A or B stream traffic
 * Parameters:
 *      unit - device unit number
 *      port - port number   
 *      type - Class A or Class B stream
 *      bytes_sec - bytes per second.
 *      bytes_burst - maximum burst size in bytes.  
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_robo_eav_bandwidth_get(
    int unit, 
    bcm_port_t port, 
    bcm_eav_stream_class_t type, 
    uint32 *bytes_sec,
    uint32 *bytes_burst)
{
    return BCM_E_UNAVAIL;
}



