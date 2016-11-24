/*
 * $Id: bcmx_filter.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:    bcmx/filter.c
 * Purpose: BCMX Filter Processor APIs
 */

#include <bcm/types.h>

#include <bcmx/filter.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

static uint32  _bcmx_filter_id;

#define FILT_ID_ALLOC  (('X'<<24) + (++_bcmx_filter_id))

#define FILT_RV_REPLACE(_trv, _rv)    \
    if (_trv == BCM_E_INIT) _trv = BCM_E_UNAVAIL;  \
        BCMX_RV_REPLACE_OK(_trv, _rv, BCM_E_UNAVAIL)

/*
 * Function:
 *      bcmx_filter_init
 */

int
bcmx_filter_init(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_init(bcm_unit);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_create
 */

int
bcmx_filter_create(bcm_filterid_t *f_return)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;
    bcm_filterid_t fid;

    fid = FILT_ID_ALLOC;
    *f_return = fid;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_create_id(bcm_unit, fid);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}



/*
 * Function:
 *      bcmx_filter_create_id
 */

int
bcmx_filter_create_id(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_create_id(bcm_unit, f);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_destroy
 */

int
bcmx_filter_destroy(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_destroy(bcm_unit, f);
        if (tmp_rv == BCM_E_NOT_FOUND) {
            tmp_rv = BCM_E_NONE;
        }
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_copy
 */

int
bcmx_filter_copy(bcm_filterid_t f_src,
                 bcm_filterid_t *f_return)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;
    bcm_filterid_t fid;

    fid = FILT_ID_ALLOC;
    *f_return = fid;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_copy_id(bcm_unit, f_src, fid);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_priority
 */

int
bcmx_filter_qualify_priority(bcm_filterid_t f,
                             int prio)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_priority(bcm_unit, f, prio);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_ingress
 */

int
bcmx_filter_qualify_ingress(bcm_filterid_t f,
                            bcmx_lplist_t lplist)
{
    int         rv, tmp_rv, i, bcm_unit;
    bcm_pbmp_t  pbm;

    rv = BCM_E_NONE;
    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbm);
        tmp_rv = bcm_filter_qualify_ingress(bcm_unit, f, pbm);
        FILT_RV_REPLACE(tmp_rv, rv);
    }
    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_egress
 *
 * Notes:
 *     All ports in the list should have the same module-id,
 *     since a filter-id can only be associated to one egress module-id.
 */

int
bcmx_filter_qualify_egress(bcm_filterid_t f,
                           bcmx_lplist_t lplist)
{
    int           rv = BCM_E_NONE;
    int           tmp_rv, i, bcm_unit;
    bcmx_lport_t  lport;
    int           count;
    bcm_module_t  filter_modid = -1;
    bcm_module_t  modid;
    bcm_port_t    modport;
    bcm_pbmp_t    pbmp;


    /*
     * Get module-id and ports
     * NOTE:
     * Allow only one module-id in the port list, since
     * a filter can be set to one egress module-id at a time.
     */

    BCM_PBMP_CLEAR(pbmp);
    BCMX_LPLIST_ITER(lplist, lport, count) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_modid_port(lport,
                                      &modid, &modport,
                                      BCMX_DEST_CONVERT_DEFAULT));
        if (filter_modid == -1) {
            filter_modid = modid;
        } else if (filter_modid != modid) {
            return BCM_E_PORT;
        }

        BCM_PBMP_PORT_ADD(pbmp, modport);
    }

    /* Set egress module-id and egress ports */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_egress_modid(bcm_unit, f, filter_modid);
        FILT_RV_REPLACE(tmp_rv, rv);
        tmp_rv = bcm_filter_qualify_egress(bcm_unit, f, pbmp);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_egress_modid
 */

int
bcmx_filter_qualify_egress_modid(bcm_filterid_t f,
                                 int module_id)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_egress_modid(bcm_unit, f, module_id);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_unknown_ucast
 */

int
bcmx_filter_qualify_unknown_ucast(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_unknown_ucast(bcm_unit, f);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_unknown_mcast
 */

int
bcmx_filter_qualify_unknown_mcast(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_unknown_mcast(bcm_unit, f);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_known_ucast
 */

int
bcmx_filter_qualify_known_ucast(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_known_ucast(bcm_unit, f);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_known_mcast
 */

int
bcmx_filter_qualify_known_mcast(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_known_mcast(bcm_unit, f);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_broadcast
 */

int
bcmx_filter_qualify_broadcast(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_broadcast(bcm_unit, f);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_stop
 */

int
bcmx_filter_qualify_stop(bcm_filterid_t f,
                         int partial_match)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_stop(bcm_unit, f, partial_match);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_format
 */

int
bcmx_filter_qualify_format(bcm_filterid_t f,
                           bcm_filter_format_t format)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_format(bcm_unit, f, format);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_data
 */

int
bcmx_filter_qualify_data(bcm_filterid_t f,
                         int offset,
                         int len,
                         const uint8 * data,
                         const uint8 * mask)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_LOCAL(bcm_unit)) {
            tmp_rv = bcm_filter_qualify_data(bcm_unit, f, offset,
                                             len, data, mask);
            FILT_RV_REPLACE(tmp_rv, rv);
        } else {
            int l;
            for (l = 0; l < len; l++) {
                tmp_rv = bcm_filter_qualify_data8(bcm_unit, f, offset+l,
                                                  data[l], mask[l]);
                if (tmp_rv < 0) {
                    FILT_RV_REPLACE(tmp_rv, rv);
                    break;
                }
            }
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_data8
 */

int
bcmx_filter_qualify_data8(bcm_filterid_t f,
                          int offset,
                          uint8 val,
                          uint8 mask)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_data8(bcm_unit, f, offset, val, mask);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_data16
 */

int
bcmx_filter_qualify_data16(bcm_filterid_t f,
                           int offset,
                           uint16 val,
                           uint16 mask)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_data16(bcm_unit, f, offset, val, mask);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_qualify_data32
 */

int
bcmx_filter_qualify_data32(bcm_filterid_t f,
                           int offset,
                           uint32 val,
                           uint32 mask)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_qualify_data32(bcm_unit, f, offset, val, mask);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_filter_action_match
 */

int
bcmx_filter_action_match(bcm_filterid_t f,
                         bcm_filter_action_t action,
                         uint32 param)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;
    bcmx_lport_t lport;
    int map_local_cpu = FALSE;
    bcm_module_t modid = -1;
    bcm_port_t modport;

    switch (action) {
    case bcmActionSetPortUcast:
    case bcmActionSetPortNonUcast:
    case bcmActionSetPortAll:
        /* For port related actions, param is an LPort. */
        lport = (bcmx_lport_t) param;
        if (lport == BCMX_LPORT_LOCAL_CPU) {
            map_local_cpu = TRUE;
        } else {
            BCM_IF_ERROR_RETURN
                (_bcmx_dest_to_modid_port(lport,
                                          &modid, &modport,
                                          BCMX_DEST_CONVERT_DEFAULT));
            param = (uint32) modport;
        }
        break;
    default:
        modid = -1;
        break;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (map_local_cpu) {
            lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
            if (BCM_FAILURE
                (_bcmx_dest_to_modid_port(lport,
                                          &modid, &modport,
                                          BCMX_DEST_CONVERT_DEFAULT))) {
                continue;
            }
            param = (uint32) modport;
        }
        tmp_rv = bcm_filter_action_match(bcm_unit, f, action, param);
        FILT_RV_REPLACE(tmp_rv, rv);
        if (modid >= 0) {
            tmp_rv = bcm_filter_action_match(bcm_unit, f,
                                             bcmActionSetModule, (uint32)modid);
            FILT_RV_REPLACE(tmp_rv, rv);
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_action_no_match
 */

int
bcmx_filter_action_no_match(bcm_filterid_t f,
                            bcm_filter_action_t action,
                            uint32 param)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;
    bcmx_lport_t lport;
    int map_local_cpu = FALSE;
    bcm_module_t modid = -1;
    bcm_port_t modport;

    switch (action) {
    case bcmActionSetPortUcast:
    case bcmActionSetPortNonUcast:
    case bcmActionSetPortAll:
        /* For port related actions, param is an LPort. */
        lport = (bcmx_lport_t) param;
        if (lport == BCMX_LPORT_LOCAL_CPU) {
            map_local_cpu = TRUE;
        } else {
            BCM_IF_ERROR_RETURN
                (_bcmx_dest_to_modid_port(lport,
                                          &modid, &modport,
                                          BCMX_DEST_CONVERT_DEFAULT));
            param = (uint32) modport;
        }
        break;
    default:
        modid = -1;
        break;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (map_local_cpu) {
            lport = BCMX_LPORT_LOCAL_CPU_GET(bcm_unit);
            if (BCM_FAILURE
                (_bcmx_dest_to_modid_port(lport,
                                          &modid, &modport,
                                          BCMX_DEST_CONVERT_DEFAULT))) {
                continue;
            }
            param = (uint32) modport;
        }
        tmp_rv = bcm_filter_action_no_match(bcm_unit, f, action, param);
        FILT_RV_REPLACE(tmp_rv, rv);
        if (modid >= 0) {
            tmp_rv = bcm_filter_action_match(bcm_unit, f,
                                             bcmActionSetModule, (uint32)modid);
            FILT_RV_REPLACE(tmp_rv, rv);
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_action_out_profile
 */

int
bcmx_filter_action_out_profile(bcm_filterid_t f,
                               bcm_filter_action_t action,
                               uint32 param,
                               int meter_id)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_action_out_profile(bcm_unit, f, action,
                                               param, meter_id);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_install
 */

int
bcmx_filter_install(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_install(bcm_unit, f);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_reinstall
 */

int
bcmx_filter_reinstall(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_reinstall(bcm_unit, f);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_filter_remove
 */

int
bcmx_filter_remove(bcm_filterid_t f)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_remove(bcm_unit, f);
        if (tmp_rv != BCM_E_NOT_FOUND) { /* Ignore not found error */
            FILT_RV_REPLACE(tmp_rv, rv);
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmx_filter_remove_all
 */

int
bcmx_filter_remove_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_filter_remove_all(bcm_unit);
        FILT_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}
