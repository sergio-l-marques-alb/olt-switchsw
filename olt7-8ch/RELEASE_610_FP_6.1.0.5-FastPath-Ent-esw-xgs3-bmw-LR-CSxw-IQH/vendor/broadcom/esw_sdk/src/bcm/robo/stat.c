/*
 * $Id: stat.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * Broadcom RoboSwitch SNMP Statistics API.
 */

#include <sal/types.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#include <soc/counter.h>
#include <soc/ll.h>

#include <bcm/port.h>
#include <bcm/error.h>

#include <bcm/stat.h>

#define COUNTER_FLAGS_DEFAULT	0

/*
 * Function:
 *	bcm_robo_stat_init
 * Description:
 *	Initializes the BCM stat module.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_INTERNAL - Chip access failure.
 */
int bcm_robo_stat_init(int unit)
{
    pbmp_t          pbmp;
    sal_usecs_t     interval;
    uint32          flags;

    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_stat_init()..\n");
    if (soc_property_get_str(unit, spn_BCM_STAT_PBMP) == NULL) {
        SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    } else {
        pbmp = soc_property_get_pbmp(unit, spn_BCM_STAT_PBMP, 0);
    }
    interval = soc_property_get(unit, spn_BCM_STAT_INTERVAL, 1000000);
    flags = soc_property_get(unit, spn_BCM_STAT_FLAGS, COUNTER_FLAGS_DEFAULT);

    SOC_PBMP_CLEAR(pbmp);
    BCM_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->counter_reset)(unit));
    
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->counter_thread_set)
                (unit, DRV_COUNTER_THREAD_START, flags, 
                interval, pbmp));

    return BCM_E_NONE;
}				


/*
 * Function:
 *	bcm_robo_stat_clear
 * Description:
 *	Clear the port based statistics from the RoboSwitch port.
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_INTERNAL - Chip access failure.
 */
int bcm_robo_stat_clear(int unit, bcm_port_t port)
{
    pbmp_t	pbm;

    if (!SOC_PORT_VALID(unit, port)){
        return BCM_E_PORT;
    }

    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_stat_clear()..\n");
    SOC_PBMP_CLEAR(pbm);
    SOC_PBMP_PORT_ADD(pbm, port);

    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->counter_set)
                (unit, pbm, 
                DRV_SNMP_COUNTER_COUNT, 0));

    return(BCM_E_NONE);
}				
	
/*
 * Function:
 *	bcm_robo_stat_sync
 * Description:
 *	Synchronize software counters with hardware
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *	Makes sure all counter hardware activity prior to the call to
 *	bcm_stat_sync is reflected in all bcm_stat_get calls that come
 *	after the call to bcm_stat_sync.
 */

int bcm_robo_stat_sync(int unit)
{
    pbmp_t null_pbmp;
    
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_stat_sync()..\n");

    SOC_PBMP_CLEAR(null_pbmp);
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->counter_thread_set)
                (unit, DRV_COUNTER_THREAD_SYNC, 0, 0, null_pbmp));

    return BCM_E_NONE;
}				

/*
 * Function:
 *	bcm_robo_stat_get
 * Description:
 *	Get the specified statistic from the RoboSwitch
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 *	type - SNMP statistics type (see stat.h)
 *      val - (OUT) 64-bit counter value.
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_PARAM - Illegal parameter.
 *	BCM_E_BADID - Illegal port number.
 *	BCM_E_INTERNAL - Chip access failure.
 *	BCM_E_UNAVAIL - Counter/variable is not implemented
 *				on this current chip.
 * Notes:
 *	Some counters are implemented on a given port only when it is
 *	operating in a specific mode, for example, 10 or 100, and not 
 *	1000. If the counter is not implemented on a given port, OR, 
 *	on the port given its current operating mode, BCM_E_UNAVAIL
 *	is returned.
 */
int bcm_robo_stat_get(int unit, bcm_port_t port, 
			bcm_stat_val_t type, uint64 *value)
{
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_stat_get()..\n");
    BCM_IF_ERROR_RETURN((DRV_SERVICES(unit)->counter_get)
                (unit, port, type, value));

    return BCM_E_NONE;
}				
			
/*
 * Function:
 *	bcm_robo_stat_get32
 * Description:
 *	Get the specified statistic from the RoboSwitch
 * Parameters:
 *	unit - RoboSwitch PCI device unit number (driver internal).
 *	port - zero-based port number
 *	type - SNMP statistics type (see stat.h)
 *      val - (OUT) 32-bit counter value.
 * Returns:
 *	BCM_E_NONE - Success.
 *	BCM_E_PARAM - Illegal parameter.
 *	BCM_E_BADID - Illegal port number.
 *	BCM_E_INTERNAL - Chip access failure.
 * Notes:
 *	Same as bcm_stat_get, except converts result to 32-bit.
 */
int bcm_robo_stat_get32(int unit, bcm_port_t port, 
			  bcm_stat_val_t type, uint32 *value)
{
    int		rv;
    uint64	val64;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    soc_cm_debug(DK_VERBOSE, 
            "BCM API : bcm_robo_stat_get32()..\n");
    rv = bcm_stat_get(unit, port, type, &val64);

    COMPILER_64_TO_32_LO(*value, val64);

    return(rv);
}				

/*
 * Function:
 *      bcm_robo_stat_custom_get 
 * Description:
 *      Get debug counter select value.
 * Parameters:
 *      unit  - RoboSwitch PCI device unit number.
 *      port  - Port number
 *      type  - SNMP statistics type.
 *      flags - (OUT) The counter select value
 *		      (see stat.h for bit definitions).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_robo_stat_custom_get(int unit, bcm_port_t port,
			   bcm_stat_val_t type, uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_robo_stat_custom_set 
 * Description:
 *      Set debug counter to count certain packet types.
 * Parameters:
 *      unit  - RoboSwitch PCI device unit number.
 *      port  - Port number, -1 to set all ports. 
 *      type  - SNMP statistics type.
 *      flags - The counter select value (see stat.h for bit definitions). 
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_robo_stat_custom_set(int unit, bcm_port_t port,
			   bcm_stat_val_t type, uint32 flags)
{
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_stat_custom_add 
 * Description:
 *      Add a certain packet type to debug counter to count 
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (only applicable to Easyrider TDBGC_SELECT).
 *      type  - SNMP statistics type.
 *      trigger - The counter select value
 *		      (see stat.h for bit definitions).
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_stat_custom_add(int unit, bcm_port_t port, bcm_stat_val_t type,
                    bcm_custom_stat_trigger_t trigger)
{
   return BCM_E_UNAVAIL;
}
/*
 * Function:
 *      bcm_stat_custom_delete 
 * Description:
 *      Deletes a certain packet type from debug counter  
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (only applicable to Easyrider TDBGC_SELECT).
 *      type  - SNMP statistics type.
 *      trigger - The counter select value
 *		      (see stat.h for bit definitions).
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_stat_custom_delete(int unit, bcm_port_t port,bcm_stat_val_t type, 
                       bcm_custom_stat_trigger_t trigger)
{
   return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_stat_custom_delete_all
 * Description:
 *      Deletes a all packet types from debug counter  
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (only applicable to Easyrider TDBGC_SELECT).
 *      type  - SNMP statistics type.
 *
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_stat_custom_delete_all(int unit, bcm_port_t port,bcm_stat_val_t type)
{
   return BCM_E_UNAVAIL;

}

/*
 * Function:
 *      bcm_stat_custom_check
 * Description:
 *      Check if certain packet types is part of debug counter  
 * Parameters:
 *      unit  - StrataSwitch PCI device unit number.
 *      port  - Port number (only applicable to Easyrider TDBGC_SELECT).
 *      type  - SNMP statistics type.
 *      trigger - The counter select value
 *		      (see stat.h for bit definitions).
 *      result - [OUT] result of a query. 0 if positive , -1 if negative
 *
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_robo_stat_custom_check(int unit, bcm_port_t port, bcm_stat_val_t type, 
                      bcm_custom_stat_trigger_t trigger, int *result)
{
   return BCM_E_UNAVAIL;
}

