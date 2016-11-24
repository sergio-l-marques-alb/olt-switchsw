/*
 * $Id: bcmx_stat.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:    bcmx/stat.c
 * Purpose: BCMX Port Statistics APIs
 */

#include <bcm/types.h>

#include <bcmx/stat.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>

#include "bcmx_int.h"


/*
 * Function:
 *      bcmx_stat_init
 */

int
bcmx_stat_init(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stat_init(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_stat_sync(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_stat_sync(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_stat_clear
 */

int
bcmx_stat_clear(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_clear(bcm_unit, bcm_port);
}

/*
 * Function:
 *      bcmx_stat_get
 */

int
bcmx_stat_get(bcmx_lport_t port,
              bcm_stat_val_t type,
              uint64 *value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_get(bcm_unit, bcm_port,
                        type, value);
}


/*
 * Function:
 *      bcmx_stat_get32
 */

int
bcmx_stat_get32(bcmx_lport_t port,
                bcm_stat_val_t type,
                uint32 *value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_get32(bcm_unit, bcm_port,
                          type, value);
}


/*
 * Function:
 *      bcmx_stat_custom_set
 */

int
bcmx_stat_custom_set(bcmx_lport_t port,
                     bcm_stat_val_t type,
                     uint32 flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_set(bcm_unit, bcm_port,
                               type, flags);
}


/*
 * Function:
 *      bcmx_stat_custom_get
 */

int
bcmx_stat_custom_get(bcmx_lport_t port,
                     bcm_stat_val_t type,
                     uint32 *flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_get(bcm_unit, bcm_port,
                               type, flags);
}


/*
 * Function:
 *      bcmx_stat_custom_add
 */

int
bcmx_stat_custom_add(bcmx_lport_t port, bcm_stat_val_t type, 
                     bcm_custom_stat_trigger_t trigger)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_add(bcm_unit, bcm_port,
                               type, trigger);
}


/*
 * Function:
 *      bcmx_stat_custom_delete
 */

int
bcmx_stat_custom_delete(bcmx_lport_t port, bcm_stat_val_t type, 
                        bcm_custom_stat_trigger_t trigger)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_delete(bcm_unit, bcm_port,
                                  type, trigger);
}


/*
 * Function:
 *      bcmx_stat_custom_delete_all
 */

int
bcmx_stat_custom_delete_all(bcmx_lport_t port, bcm_stat_val_t type)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_delete_all(bcm_unit, bcm_port,
                                      type);
}


/*
 * Function:
 *      bcmx_stat_custom_check
 */

int
bcmx_stat_custom_check(bcmx_lport_t port, bcm_stat_val_t type, 
                       bcm_custom_stat_trigger_t trigger, int *result)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_stat_custom_check(bcm_unit, bcm_port,
                                 type, trigger, result);
}
