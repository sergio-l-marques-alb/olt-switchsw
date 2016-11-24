/*
 * $Id: bcmx_custom.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * Custom API callouts
 */

#include <bcm/types.h>

#include <bcmx/custom.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>

#include "bcmx_int.h"


/*
 * Notes:
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_custom_port_set(bcmx_lport_t port, int type,
                     uint32 args[BCM_CUSTOM_ARGS_MAX])
{
    int         rv = BCM_E_NONE, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        if (BCM_FAILURE(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
            return BCM_E_PORT;
        }
        return bcm_custom_port_set(bcm_unit, bcm_port, type, args);

    } else if (BCMX_LPORT_IS_VIRTUAL(port)) {
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_custom_port_set(bcm_unit, port, type, args);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
    }

    return BCM_E_PORT;
}

int
bcmx_custom_port_get(bcmx_lport_t port, int type,
                     uint32 args[BCM_CUSTOM_ARGS_MAX])
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    if (BCMX_LPORT_IS_PHYSICAL(port)) {
        if (BCM_FAILURE(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
            return BCM_E_PORT;
        }
        return bcm_custom_port_get(bcm_unit, bcm_port, type, args);

    } else if (BCMX_LPORT_IS_VIRTUAL(port)) {
    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_custom_port_get(bcm_unit, port, type, args);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }
    return BCM_E_NOT_FOUND;
    }

    return BCM_E_PORT;
}

int
bcmx_custom_unit_set(int type, uint32 args[BCM_CUSTOM_ARGS_MAX])
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_custom_port_set(bcm_unit, -1, type, args);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_custom_unit_get(int type, uint32 args[BCM_CUSTOM_ARGS_MAX])
{
    int rv = BCM_E_UNAVAIL;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_custom_port_get(bcm_unit, -1, type, args);
        if (rv >= 0) {
            return rv;
        }
    }

    return rv;
}
