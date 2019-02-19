/* 
 * $Id:$
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * File:        portctrl.c
 * Purpose:     SDK Port Control Glue Layer
 *
 *              The purpose is to encapsulate port functionality
 *              related to the xxPORT block (i.e. XLPORT, CPORT)
 *              MAC and PHY.
 *
 *              Currently, only the PortMod library is being supported.
 *              The PortMod library provides support for the MAC, PHY,
 *              and xxPORT registers.
 *
 *              Callers of the Port Control routines should check
 *              before calling into them with the macro:
 *                  SOC_USE_PORTCTRL()
 *
 */

#include <bcm_int/esw/portctrl.h>
#include <bcm_int/esw/port.h>

#include <bcm/error.h>


/* Indicates if the Port Control module has been initalized */
static int portctrl_init[BCM_MAX_NUM_UNITS];

/*
 * Define:
 *      PORTCTRL_INIT_CHECK
 * Purpose:
 *      Checks that Port Control module has been initialized for given
 *      unit.  If failure, it causes routine to return with BCM_E_INIT.
 */
#define PORTCTRL_INIT_CHECK(unit)    \
    if (!portctrl_init[unit]) { return BCM_E_INIT; }


/*
 * Function:
 *      bcmi_esw_portctrl_init
 * Purpose:
 *      Initialize the Port Control component and
 *      corresponding library (PortMod library).
 *
 *      This is to be called by the BCM Port module initialization
 *      function bcm_esw_port_init().
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_esw_portctrl_init(int unit)
{
#ifdef PORTMOD_SUPPORT

    /* Call PortMod library initialization */

    /* Successful */
    portctrl_init[unit] = 1;
    return BCM_E_NONE;

#else   /* PORTMOD_SUPPORT */

    /* Prevents compiler warning: unused variable */
    COMPILER_REFERENCE(portctrl_init);

    return BCM_E_UNAVAIL;
#endif  /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      bcmi_esw_portctrl_deinit
 * Purpose:
 *      Uninitialize the Port Control component and
 *      corresponding library (PortMod library).
 *
 *      This function must be called by the BCM Port module
 *      deinit (or detach) function.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_esw_portctrl_deinit(int unit)
{
#ifdef PORTMOD_SUPPORT

    /*
      Possible common calls:
      PORT_LOCK/UNLOCK()
      _bcm_esw_port_gport_validate()
      _bcm_esw_gport_resolve()
    */

    portctrl_init[unit] = 0;
    return BCM_E_NONE;

#else   /* PORTMOD_SUPPORT */
    return BCM_E_UNAVAIL;
#endif  /* PORTMOD_SUPPORT */
}

/*
 * Function:
 *      bcmi_esw_portctrl_enable_set
 * Purpose:
 *      Physically enable/disable the MAC/PHY on this port.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Port number.
 *      enable - (IN) TRUE, port is enabled, FALSE port is disabled.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If linkscan is running, it also controls the MAC enable state.
 */
int
bcmi_esw_portctrl_enable_set(int unit, bcm_gport_t port, int enable)
{
#ifdef PORTMOD_SUPPORT
    int rv = BCM_E_NONE;

    PORTCTRL_INIT_CHECK(unit);

    /*
      Possible common calls:
      PORT_LOCK/UNLOCK()
      _bcm_esw_port_gport_validate()
      _bcm_esw_gport_resolve()
    */

    return rv;

#else   /* PORTMOD_SUPPORT */
    return BCM_E_UNAVAIL;
#endif  /* PORTMOD_SUPPORT */
}
