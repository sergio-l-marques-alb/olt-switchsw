/*
 * $Id: bcmx_vlan.c,v 1.1 2011/04/18 17:11:03 mruas Exp $
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
 * File:        src/bcmx/vlan.c
 * Purpose:     BCMX Virtual LAN (VLAN) APIs
 */

#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/vlan.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

int 
bcmx_vlan_create(bcm_vlan_t vid)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_create(bcm_unit, vid);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_EXISTS);
    }

    return rv;
}


int 
bcmx_vlan_destroy(bcm_vlan_t vid)
{
    int tmp_rv, rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_destroy(bcm_unit, vid);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_NOT_FOUND);
    }

    return rv;
}


int
bcmx_vlan_destroy_all(void)
{
    int tmp_rv, rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_destroy_all(bcm_unit);
        BCMX_RV_REPLACE(tmp_rv, rv);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_vlan_port_add
 * Notes:
 *      Currently assumes stack ports are added by BCM on vlan create.
 */

int
bcmx_vlan_port_add(bcm_vlan_t vid, 
                   bcmx_lplist_t lplist,
                   bcmx_lplist_t ut_lplist)
{
    int tmp_rv, rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, upbmp;
    int bcm_unit;
    int i;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        BCMX_LPLIST_TO_PBMP(ut_lplist, bcm_unit, upbmp);
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            tmp_rv = bcm_vlan_port_add(bcm_unit, vid, pbmp, upbmp);
            BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_EXISTS);
        }
    }

    return rv;
}


int
bcmx_vlan_port_remove(bcm_vlan_t vid, bcmx_lplist_t lplist)
{
    int tmp_rv, rv = BCM_E_NONE;
    bcm_pbmp_t pbmp;
    int bcm_unit;
    int i;

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            tmp_rv = bcm_vlan_port_remove(bcm_unit, vid, pbmp);
            BCMX_RV_REPLACE(tmp_rv, rv);
        }
    }

    return rv;
}


int
bcmx_vlan_port_get(bcm_vlan_t vid,
                   bcmx_lplist_t *lplist,
                   bcmx_lplist_t *ut_lplist)
{
    int tmp_rv, rv = BCM_E_NONE;
    bcm_pbmp_t pbmp, upbmp;
    int bcm_unit;
    int i;

    BCMX_PARAM_NULL_CHECK(lplist);
    BCMX_PARAM_NULL_CHECK(ut_lplist);
    
    bcmx_lplist_clear(lplist);
    bcmx_lplist_clear(ut_lplist);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_port_get(bcm_unit, vid, &pbmp, &upbmp);
        if (BCM_FAILURE(tmp_rv)) {
            rv = tmp_rv;
        } else {
            tmp_rv = BCMX_LPLIST_PBMP_ADD(lplist, bcm_unit, pbmp);
            BCMX_RV_REPLACE(tmp_rv, rv);
            tmp_rv = BCMX_LPLIST_PBMP_ADD(ut_lplist, bcm_unit, upbmp);
            BCMX_RV_REPLACE(tmp_rv, rv);
        }
    }

    return rv;
}


int
bcmx_vlan_default_get(bcm_vlan_t *vid_ptr)
{
    int rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(vid_ptr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_default_get(bcm_unit, vid_ptr);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}


int
bcmx_vlan_default_set(bcm_vlan_t vid)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_default_set(bcm_unit, vid);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_stg_get(bcm_vlan_t vid, bcm_stg_t *stg_ptr)
{
    int rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(stg_ptr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (BCM_IS_FABRIC(bcm_unit)) {
            continue;
        }

        rv = bcm_vlan_stg_get(bcm_unit, vid, stg_ptr);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}


int
bcmx_vlan_stg_set(bcm_vlan_t vid, bcm_stg_t stg)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stg_set(bcm_unit, vid, stg);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_stp_set(bcm_vlan_t vid, bcmx_lport_t port, int stp_state)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_stp_set(bcm_unit,
                            vid,
                            bcm_port,
                            stp_state);
}


int
bcmx_vlan_stp_get(bcm_vlan_t vid, bcmx_lport_t port, int *stp_state)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(stp_state);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_stp_get(bcm_unit,
                            vid,
                            bcm_port,
                            stp_state);
}


int
bcmx_vlan_port_default_action_set(bcmx_lport_t port,
                                  bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_default_action_set(bcm_unit, bcm_port,
                                            action);
}

int
bcmx_vlan_port_default_action_get(bcmx_lport_t port,
                                  bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_default_action_get(bcm_unit, bcm_port,
                                            action);
}

int
bcmx_vlan_port_default_action_delete(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_default_action_delete(bcm_unit, bcm_port);
}

int
bcmx_vlan_port_egress_default_action_set(bcmx_lport_t port,
                                         bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_egress_default_action_set(bcm_unit, bcm_port,
                                                   action);
}
 
int
bcmx_vlan_port_egress_default_action_get(bcmx_lport_t port,
                                         bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_egress_default_action_get(bcm_unit, bcm_port,
                                                   action);
}   

int
bcmx_vlan_port_egress_default_action_delete(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_egress_default_action_delete(bcm_unit, bcm_port);
}

int
bcmx_vlan_port_protocol_action_add(bcmx_lport_t port,
                                   bcm_port_frametype_t frame,
                                   bcm_port_ethertype_t ether,
                                   bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_protocol_action_add(bcm_unit, bcm_port,
                                             frame, ether, action);
}

int
bcmx_vlan_port_protocol_action_get(bcmx_lport_t port, 
                                   bcm_port_frametype_t frame, 
                                   bcm_port_ethertype_t ether, 
                                   bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_protocol_action_get(bcm_unit, bcm_port,
                                             frame, ether, action);
}

int
bcmx_vlan_port_protocol_action_delete(bcmx_lport_t port, 
                                      bcm_port_frametype_t frame, 
                                      bcm_port_ethertype_t ether)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_protocol_action_delete(bcm_unit, bcm_port,
                                                frame, ether);
}

int
bcmx_vlan_port_protocol_action_delete_all(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_port_protocol_action_delete_all(bcm_unit, bcm_port);
}


int
bcmx_vlan_mac_add(bcm_mac_t mac, bcm_vlan_t vid, int prio)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_add(bcm_unit, mac, vid, prio);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_mac_delete(bcm_mac_t mac)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_delete(bcm_unit, mac);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_mac_delete_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int bcmx_vlan_mac_action_add(bcm_mac_t mac,
                             bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_action_add(bcm_unit, mac, action);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_mac_action_delete(bcm_mac_t mac)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_action_delete(bcm_unit, mac);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_mac_action_delete_all(void)
{
    int tmp_rv, rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mac_action_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_mac_action_get(bcm_mac_t mac, 
                         bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_mac_action_get(bcm_unit, mac, action);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}


int
bcmx_vlan_translate_add(bcmx_lport_t port,
                        bcm_vlan_t old_vid,
                        bcm_vlan_t new_vid,
                        int prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_add(bcm_unit, bcm_port,
                                  old_vid, new_vid, prio);
}

int
bcmx_vlan_translate_delete(bcmx_lport_t port, bcm_vlan_t old_vid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_delete(bcm_unit, bcm_port,
                                     old_vid);
}

int
bcmx_vlan_translate_delete_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_translate_get(bcmx_lport_t port, bcm_vlan_t old_vid,
                        bcm_vlan_t *new_vid, int *prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(prio);
    
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_get(bcm_unit, bcm_port,
                                  old_vid, new_vid, prio);
}

int
bcmx_vlan_translate_action_add(bcm_gport_t port, 
                               bcm_vlan_translate_key_t key_type, 
                               bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                               bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;
#ifdef LVL7_FIXUP
    int rv = BCM_E_NONE, tmp_rv;
    int i;
#endif



    BCMX_PARAM_NULL_CHECK(action);


#ifdef LVL7_FIXUP
    if ( BCM_GPORT_IS_TRUNK(port))
    {
      BCMX_UNIT_ITER(bcm_unit, i) {
       bcm_unit = i;
       bcm_port = port;
       tmp_rv = bcm_vlan_translate_action_add(bcm_unit, port,
                                         key_type, outer_vlan, inner_vlan,
                                         action);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
      }
    }
    else
#endif
    {
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_action_add(bcm_unit, port,
                                         key_type, outer_vlan, inner_vlan,
                                         action);


    }
  #ifdef LVL7_FIXUP
  return rv;
 #endif
}

int
bcmx_vlan_translate_action_delete(bcm_gport_t port,
                                  bcm_vlan_translate_key_t key_type,
                                  bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

#ifdef LVL7_FIXUP
    int rv = BCM_E_NONE, tmp_rv;
    int i;
#endif


#ifdef LVL7_FIXUP
    if ( BCM_GPORT_IS_TRUNK(port))
    {
       BCMX_UNIT_ITER(bcm_unit, i) {
       bcm_unit = i;
       bcm_port = port;
       tmp_rv =  bcm_vlan_translate_action_delete(bcm_unit, port,
                                            key_type, outer_vlan, inner_vlan);
       BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
      }
    }
    else
#endif
    {
      BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));
    return bcm_vlan_translate_action_delete(bcm_unit, port,
                                            key_type, outer_vlan, inner_vlan);
    }

  #ifdef LVL7_FIXUP
  return rv;
 #endif

}

int
bcmx_vlan_translate_action_get(bcm_gport_t port, 
                               bcm_vlan_translate_key_t key_type, 
                               bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                               bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_action_get(bcm_unit, port, key_type,
                                         outer_vlan, inner_vlan,
                                         action);
}


int
bcmx_vlan_translate_egress_add(bcmx_lport_t port,
                               bcm_vlan_t old_vid,
                               bcm_vlan_t new_vid,
                               int prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_egress_add(bcm_unit, bcm_port,
                                         old_vid, new_vid, prio);
}

int
bcmx_vlan_translate_egress_delete(bcmx_lport_t port, bcm_vlan_t old_vid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_egress_delete(bcm_unit, bcm_port,
                                            old_vid);
}

int
bcmx_vlan_translate_egress_delete_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_translate_egress_get(bcmx_lport_t port, bcm_vlan_t old_vid, 
                               bcm_vlan_t *new_vid, int *prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(prio);
    
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_egress_get(bcm_unit, bcm_port, old_vid,
                                         new_vid, prio);
}


int
bcmx_vlan_translate_egress_action_add(int port_class,
                                      bcm_vlan_t outer_vlan,
                                      bcm_vlan_t inner_vlan,
                                      bcm_vlan_action_set_t *action)
{
    int tmp_rv, rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_action_add(bcm_unit, port_class,
                                                      outer_vlan, inner_vlan,
                                                      action);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_translate_egress_action_delete(int port_class,
                                         bcm_vlan_t outer_vlan,
                                         bcm_vlan_t inner_vlan)
{
    int tmp_rv, rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_egress_action_delete(bcm_unit, port_class,
                                                         outer_vlan, inner_vlan);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_translate_egress_action_get(int port_class, 
                                      bcm_vlan_t outer_vlan, 
                                      bcm_vlan_t inner_vlan, 
                                      bcm_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_translate_egress_action_get(bcm_unit, port_class,
                                                  outer_vlan, inner_vlan,
                                                  action);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}


int
bcmx_vlan_dtag_add(bcmx_lport_t port,
                   bcm_vlan_t old_vid,
                   bcm_vlan_t new_vid,
                   int prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_dtag_add(bcm_unit, bcm_port,
                             old_vid, new_vid, prio);
}

int
bcmx_vlan_dtag_delete(bcmx_lport_t port, bcm_vlan_t old_vid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_dtag_delete(bcm_unit, bcm_port,
                                old_vid);
}

int
bcmx_vlan_dtag_delete_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_dtag_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_dtag_get(bcmx_lport_t port, bcm_vlan_t old_vid, 
                   bcm_vlan_t *new_vid, int *prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(prio);
    
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_dtag_get(bcm_unit, bcm_port, old_vid, new_vid, prio);
}


int
bcmx_vlan_translate_range_add(bcmx_lport_t port,
                              bcm_vlan_t old_vid_low, bcm_vlan_t old_vid_high,
                              bcm_vlan_t new_vid, int int_prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_range_add(bcm_unit, bcm_port,
                                        old_vid_low, old_vid_high,
                                        new_vid, int_prio);
}

int
bcmx_vlan_translate_range_delete(bcmx_lport_t port,
                                 bcm_vlan_t old_vid_low,
                                 bcm_vlan_t old_vid_high)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_range_delete(bcm_unit, bcm_port,
                                           old_vid_low, old_vid_high);
}

int
bcmx_vlan_translate_range_delete_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_range_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_translate_range_get(bcmx_lport_t port, 
                              bcm_vlan_t old_vid_low, bcm_vlan_t old_vid_high, 
                              bcm_vlan_t *new_vid, int *int_prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(int_prio);
    
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_range_get(bcm_unit, bcm_port,
                                        old_vid_low, old_vid_high,
                                        new_vid, int_prio);
}


int
bcmx_vlan_dtag_range_add(bcmx_lport_t port,
                         bcm_vlan_t old_vid_low, bcm_vlan_t old_vid_high,
                         bcm_vlan_t new_vid, int int_prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_dtag_range_add(bcm_unit, bcm_port,
                                   old_vid_low, old_vid_high,
                                   new_vid, int_prio);
}

int
bcmx_vlan_dtag_range_delete(bcmx_lport_t port, 
                            bcm_vlan_t old_vid_low,
                            bcm_vlan_t old_vid_high)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_dtag_range_delete(bcm_unit, bcm_port,
                                      old_vid_low, old_vid_high);
}

int
bcmx_vlan_dtag_range_delete_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_dtag_range_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_dtag_range_get(bcmx_lport_t port,
                         bcm_vlan_t old_vid_low, bcm_vlan_t old_vid_high, 
                         bcm_vlan_t *new_vid, int *prio)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(new_vid);
    BCMX_PARAM_NULL_CHECK(prio);
    
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_dtag_range_get(bcm_unit, bcm_port,
                                   old_vid_low, old_vid_high,
                                   new_vid, prio);
}


int
bcmx_vlan_translate_action_range_add(bcm_gport_t port,
                                     bcm_vlan_t outer_vlan_low, 
                                     bcm_vlan_t outer_vlan_high, 
                                     bcm_vlan_t inner_vlan_low, 
                                     bcm_vlan_t inner_vlan_high, 
                                     bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_action_range_add(bcm_unit, port,
                                               outer_vlan_low,
                                               outer_vlan_high,
                                               inner_vlan_low, 
                                               inner_vlan_high, 
                                               action);
}

int
bcmx_vlan_translate_action_range_delete(bcm_gport_t port,
                                        bcm_vlan_t outer_vlan_low, 
                                        bcm_vlan_t outer_vlan_high, 
                                        bcm_vlan_t inner_vlan_low, 
                                        bcm_vlan_t inner_vlan_high)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_action_range_delete(bcm_unit, port,
                                                  outer_vlan_low,
                                                  outer_vlan_high,
                                                  inner_vlan_low, 
                                                  inner_vlan_high);
}

int
bcmx_vlan_translate_action_range_delete_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_translate_action_range_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_translate_action_range_get(bcm_gport_t port, 
                                     bcm_vlan_t outer_vlan_low, 
                                     bcm_vlan_t outer_vlan_high, 
                                     bcm_vlan_t inner_vlan_low, 
                                     bcm_vlan_t inner_vlan_high, 
                                     bcm_vlan_action_set_t *action)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(action);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_translate_action_range_get(bcm_unit, port,
                                               outer_vlan_low,
                                               outer_vlan_high,
                                               inner_vlan_low,
                                               inner_vlan_high,
                                               action);
}


int
bcmx_vlan_ip4_add(bcm_ip_t ipaddr,
                  bcm_ip_t netmask,
                  bcm_vlan_t vid,
                  int prio)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip4_add(bcm_unit, ipaddr,
                                  netmask, vid, prio);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_ip4_delete(bcm_ip_t ipaddr, bcm_ip_t netmask)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip4_delete(bcm_unit, ipaddr, netmask);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_ip4_delete_all(void)
{
    int tmp_rv, rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip4_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_ip_add(bcm_vlan_ip_t *vlan_ip)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(vlan_ip);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_add(bcm_unit, vlan_ip);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_ip_delete(bcm_vlan_ip_t *vlan_ip)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(vlan_ip);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_delete(bcm_unit, vlan_ip);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_ip_delete_all(void)
{
    int tmp_rv, rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_ip_action_add(bcm_vlan_ip_t *vlan_ip,
                        bcm_vlan_action_set_t *action)
{
    int tmp_rv, rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(vlan_ip);
    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_action_add(bcm_unit, vlan_ip, action);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;

}

int
bcmx_vlan_ip_action_get(bcm_vlan_ip_t *vlan_ip,
                        bcm_vlan_action_set_t *action)
{
    int  rv = BCM_E_NONE;
    int  i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(vlan_ip);
    BCMX_PARAM_NULL_CHECK(action);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_ip_action_get(bcm_unit, vlan_ip, action);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}

int
bcmx_vlan_ip_action_delete(bcm_vlan_ip_t *vlan_ip)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(vlan_ip);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_action_delete(bcm_unit, vlan_ip);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;

}

int
bcmx_vlan_ip_action_delete_all(void)
{
    int  tmp_rv, rv = BCM_E_NONE;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_ip_action_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_control_set(bcm_vlan_control_t type, int arg)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_control_set(bcm_unit, type, arg);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


int
bcmx_vlan_control_get(bcm_vlan_control_t type, int *arg)
{
    int rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(arg);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_control_get(bcm_unit, type, arg);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}


/*
 * Function:
 *      bcmx_vlan_control_port_set
 * Notes:
 *      Exposing the ability of the BCM API to set all Ethernet ports
 *      the BCMX layer provides an special value defined for this purpose:
 *          BCMX_LPORT_ETHER_ALL  --> All Ethernet ports (FE, GE, XE)
 */

int
bcmx_vlan_control_port_set(bcmx_lport_t port, 
                           bcm_vlan_control_port_t type,
                           int arg)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;
    int bcm_port_all = -1; /* This value indicates all ether ports in BCM */

    if (port == BCMX_LPORT_ETHER_ALL) {
        BCMX_UNIT_ITER(bcm_unit, i) {
            tmp_rv = bcm_vlan_control_port_set(bcm_unit, bcm_port_all, 
                                               type, arg);
            BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
        }
    } else {
        int         bcm_unit;
        bcm_port_t  bcm_port;

        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));

        return bcm_vlan_control_port_set(bcm_unit, bcm_port,
                                         type, arg);
    }

    return rv;
}


int
bcmx_vlan_control_port_get(bcmx_lport_t port,
                           bcm_vlan_control_port_t type,
                           int *arg)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PARAM_NULL_CHECK(arg);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_vlan_control_port_get(bcm_unit, bcm_port,
                                     type, arg);
}


int
bcmx_vlan_mcast_flood_set(bcm_vlan_t vlan,
                          bcm_vlan_mcast_flood_t mode)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_mcast_flood_set(bcm_unit, vlan, mode);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

int
bcmx_vlan_mcast_flood_get(bcm_vlan_t vlan,
                          bcm_vlan_mcast_flood_t *mode)
{
    int rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(mode);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_mcast_flood_get(bcm_unit, vlan, mode);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_vlan_control_vlan_set
 */
int 
bcmx_vlan_control_vlan_set(bcm_vlan_t vlan,
                           bcm_vlan_control_vlan_t control) 
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_control_vlan_set(bcm_unit, vlan, control);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_control_vlan_get
 */
int
bcmx_vlan_control_vlan_get(bcm_vlan_t vlan,
                           bcm_vlan_control_vlan_t *control) 
{
    int rv = BCM_E_NONE;
    int i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(control);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_control_vlan_get(bcm_unit, vlan, control);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcmx_vlan_cross_connect_add
 */
int
bcmx_vlan_cross_connect_add(bcm_vlan_t outer_vlan, bcm_vlan_t inner_vlan, 
                            bcm_gport_t port_1, bcm_gport_t port_2)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_cross_connect_add(bcm_unit,
                                            outer_vlan, inner_vlan,
                                            port_1, port_2);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_cross_connect_delete
 */
int bcmx_vlan_cross_connect_delete(bcm_vlan_t outer_vlan, 
                                   bcm_vlan_t inner_vlan)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_cross_connect_delete(bcm_unit,
                                               outer_vlan, inner_vlan);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_cross_connect_delete_all
 */
int
bcmx_vlan_cross_connect_delete_all(void)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_cross_connect_delete_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

void bcmx_vlan_block_t_init(bcmx_vlan_block_t *data)
{
    bcmx_lplist_t_init(&data->known_multicast);
    bcmx_lplist_t_init(&data->unknown_multicast);
    bcmx_lplist_t_init(&data->unknown_unicast);
    bcmx_lplist_t_init(&data->broadcast);
}

int bcmx_vlan_block_get(bcm_vlan_t vlan, bcmx_vlan_block_t *vlan_block)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;
    bcm_vlan_block_t bcm_vlb;

    bcmx_vlan_block_t_init(vlan_block);
    BCMX_UNIT_ITER(bcm_unit, i) {
        bcm_vlan_block_t_init(&bcm_vlb);
        tmp_rv = bcm_vlan_block_set(bcm_unit, vlan, &bcm_vlb);
        if (BCM_SUCCESS(tmp_rv)) {
            BCM_IF_ERROR_RETURN
                (bcmx_lplist_pbmp_add(&vlan_block->known_multicast,
                                      bcm_unit, &bcm_vlb.known_multicast));
            BCM_IF_ERROR_RETURN
                (bcmx_lplist_pbmp_add(&vlan_block->unknown_multicast,
                                      bcm_unit, &bcm_vlb.unknown_multicast));
            BCM_IF_ERROR_RETURN
                (bcmx_lplist_pbmp_add(&vlan_block->unknown_unicast,
                                      bcm_unit, &bcm_vlb.unknown_unicast));
            BCM_IF_ERROR_RETURN
                (bcmx_lplist_pbmp_add(&vlan_block->broadcast,
                                      bcm_unit, &bcm_vlb.broadcast));
        }
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_EXISTS);
    }

    return rv;
}

int bcmx_vlan_block_set(bcm_vlan_t vlan, bcmx_vlan_block_t *vlan_block)
{
    int rv = BCM_E_NONE, tmp_rv;
    int i, bcm_unit;
    bcm_vlan_block_t bcm_vlb;

    BCMX_UNIT_ITER(bcm_unit, i) {
        bcm_vlan_block_t_init(&bcm_vlb);
        BCMX_LPLIST_TO_PBMP(vlan_block->known_multicast,
                            bcm_unit, bcm_vlb.known_multicast);
        BCMX_LPLIST_TO_PBMP(vlan_block->unknown_multicast,
                            bcm_unit, bcm_vlb.unknown_multicast);
        BCMX_LPLIST_TO_PBMP(vlan_block->unknown_unicast,
                            bcm_unit, bcm_vlb.unknown_unicast);
        BCMX_LPLIST_TO_PBMP(vlan_block->broadcast,
                            bcm_unit, bcm_vlb.broadcast);
        if (BCM_PBMP_NOT_NULL(bcm_vlb.known_multicast) ||
            BCM_PBMP_NOT_NULL(bcm_vlb.unknown_multicast) ||
            BCM_PBMP_NOT_NULL(bcm_vlb.unknown_unicast) ||
            BCM_PBMP_NOT_NULL(bcm_vlb.broadcast)) {
            tmp_rv = bcm_vlan_block_set(bcm_unit, vlan, &bcm_vlb);
            BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_EXISTS);
        }
    }
    
    return rv;
}


/*
 * Function:
 *      bcmx_vlan_stat_enable_set
 * Purpose:
 *      Enable/disable collection of statistics on the indicated VLAN.
 * Parameters:
 *      vlan   - VLAN Id
 *      enable - Non-zero to enable counter collection, zero to disable
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_enable_set(bcm_vlan_t vlan, int enable)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_enable_set(bcm_unit, vlan, enable);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_get
 * Purpose:
 *      Get 64-bit counter value for specified VLAN and COS statistic type.
 * Parameters:
 *      vlan - VLAN Id
 *      cos  - COS or priority
 *      stat - Type of the counter to retrieve
 *      val  - (OUT) Pointer to a counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_get(bcm_vlan_t vlan, bcm_cos_t cos,
                   bcm_vlan_stat_t stat, uint64 *val)
{
    int     rv = BCM_E_NONE, tmp_rv;
    int     i, bcm_unit;
    uint64  tmp_val;

    BCMX_PARAM_NULL_CHECK(val);

    COMPILER_64_ZERO(*val);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_get(bcm_unit, vlan, cos, stat, &tmp_val);
        if (BCM_SUCCESS(tmp_rv)) {
            COMPILER_64_ADD_64(*val, tmp_val);
        }
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}
     
/*
 * Function:
 *      bcmx_vlan_stat_get32
 * Purpose:
 *      Get lower 32-bit counter value for specified VLAN and
 *      COS statistic type.
 * Parameters:
 *      vlan - VLAN Id
 *      cos  - COS or priority
 *      stat - Type of the counter to retrieve
 *      val  - (OUT) Pointer to a counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_get32(bcm_vlan_t vlan, bcm_cos_t cos, 
                     bcm_vlan_stat_t stat, uint32 *val)
{
    int     rv = BCM_E_NONE, tmp_rv;
    int     i, bcm_unit;
    uint32  tmp_val;

    BCMX_PARAM_NULL_CHECK(val);

    *val = 0;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_get32(bcm_unit, vlan, cos, stat, &tmp_val);
        if (BCM_SUCCESS(tmp_rv)) {
            *val += tmp_val;
        }
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_set
 * Purpose:
 *      Set 64-bit counter value for specified VLAN and COS statistic type.
 * Parameters:
 *      vlan - VLAN Id
 *      cos  - COS or priority
 *      stat - Type of the counter to set
 *      val  - New counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_set(bcm_vlan_t vlan, bcm_cos_t cos, 
                   bcm_vlan_stat_t stat, uint64 val)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_set(bcm_unit, vlan, cos, stat, val);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}
     
/*
 * Function:
 *      bcmx_vlan_stat_set32
 * Purpose:
 *      Set lower 32-bit counter value for specified VLAN and
 *      COS statistic type.
 * Parameters:
 *      vlan - VLAN Id
 *      cos  - COS or priority
 *      stat - Type of the counter to set
 *      val  - New counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_set32(bcm_vlan_t vlan, bcm_cos_t cos, 
                     bcm_vlan_stat_t stat, uint32 val)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_set32(bcm_unit, vlan, cos, stat, val);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      vlan      - VLAN Id
 *      cos       - COS or priority
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_multi_get(bcm_vlan_t vlan, bcm_cos_t cos, int nstat, 
                         bcm_vlan_stat_t *stat_arr, uint64 *value_arr)
{
    int     rv = BCM_E_NONE, tmp_rv;
    int     i, bcm_unit;
    uint64  *tmp_val;

    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    tmp_val = sal_alloc(sizeof(uint64) * nstat, "bcmx vlan stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        COMPILER_64_ZERO(value_arr[i]);
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_multi_get(bcm_unit, vlan, cos, nstat,
                                         stat_arr, tmp_val);
        if (BCM_SUCCESS(tmp_rv)) {
            for (i = 0; i < nstat; i++) {
                COMPILER_64_ADD_64(value_arr[i], tmp_val[i]);
            }
        }
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    sal_free(tmp_val);

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      vlan      - VLAN Id
 *      cos       - COS or priority
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_multi_get32(bcm_vlan_t vlan, bcm_cos_t cos, int nstat,
                           bcm_vlan_stat_t *stat_arr, uint32 *value_arr)
{
    int     rv = BCM_E_NONE, tmp_rv;
    int     i, bcm_unit;
    uint32  *tmp_val;

    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    tmp_val = sal_alloc(sizeof(uint32) * nstat, "bcmx vlan stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        value_arr[i] = 0;
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_multi_get32(bcm_unit, vlan, cos, nstat,
                                           stat_arr, tmp_val);
        if (BCM_SUCCESS(tmp_rv)) {
            for (i = 0; i < nstat; i++) {
                value_arr[i] += tmp_val[i];
            }
        }
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    sal_free(tmp_val);

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      vlan      - VLAN Id
 *      cos       - COS or priority
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_multi_set(bcm_vlan_t vlan, bcm_cos_t cos, int nstat, 
                         bcm_vlan_stat_t *stat_arr, uint64 *value_arr)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_multi_set(bcm_unit, vlan, cos, nstat,
                                         stat_arr, value_arr);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple VLAN statistic types.
 * Parameters:
 *      vlan      - VLAN Id
 *      cos       - COS or priority
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_stat_multi_set32(bcm_vlan_t vlan, bcm_cos_t cos, int nstat, 
                           bcm_vlan_stat_t *stat_arr, uint32 *value_arr)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_stat_multi_set32(bcm_unit, vlan, cos, nstat,
                                           stat_arr, value_arr);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_vlan_queue_map_create
 * Purpose:
 *      Create a VLAN queue map entry.
 * Parameters:
 *      flags - Control flags
 *      qmid  - (IN/OUT) Queue map identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_create(uint32 flags, int *qmid)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_PARAM_NULL_CHECK(qmid);

    BCMX_UNIT_ITER(bcm_unit, i) {

        tmp_rv = bcm_vlan_queue_map_create(bcm_unit, flags, qmid);
        /*
         * Use the ID from first successful 'create' if group ID
         * is not specified.
         */
        if (!(flags & BCM_VLAN_QUEUE_MAP_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                flags |= BCM_VLAN_QUEUE_MAP_WITH_ID;
            }
        }

        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_destroy
 * Purpose:
 *      Delete a VLAN queue map entry.
 * Parameters:
 *      qmid  - Queue map identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_destroy(int qmid)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_destroy(bcm_unit, qmid);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_destroy_all
 * Purpose:
 *      Delete all VLAN queue map entries.
 * Parameters:
 *      None
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_destroy_all(void)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_destroy_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_set
 * Purpose:
 *      Set packet queue and color for a VLAN queue map entry.
 * Parameters:
 *      qmid    - Queue map identifier
 *      pkt_pri - Packet priority
 *      cfi     - Packet CFI value
 *      queue   - Assigned COS queue number
 *      color   - Assigned color value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_set(int qmid, int pkt_pri, int cfi,
                        int queue, int color)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_set(bcm_unit, qmid, pkt_pri, cfi,
                                        queue, color);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_get
 * Purpose:
 *      Get packet queue and color for a VLAN queue map entry.
 * Parameters:
 *      qmid    - Queue map identifier
 *      pkt_pri - Packet priority
 *      cfi     - Packet CFI value
 *      queue   - (OUT) Assigned COS queue number
 *      color   - (OUT) Assigned color value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_get(int qmid, int pkt_pri, int cfi, 
                        int *queue, int *color)
{
    int  rv;
    int  i;
    int  bcm_unit;

    BCMX_PARAM_NULL_CHECK(queue);
    BCMX_PARAM_NULL_CHECK(color);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_queue_map_get(bcm_unit, qmid, pkt_pri, cfi,
                                    queue, color);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_attach
 * Purpose:
 *      Attach a queue map object to a VLAN or VFI.
 * Parameters:
 *      vlan  - VLAN identifier
 *      qmid  - Queue map identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_attach(bcm_vlan_t vlan, int qmid)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_attach(bcm_unit, vlan, qmid);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_attach_get
 * Purpose:
 *      Get attached queue map object for given VLAN or VFI.
 * Parameters:
 *      vlan  - VLAN identifier
 *      qmid  - (OUT) Queue map identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_attach_get(bcm_vlan_t vlan, int *qmid)
{
    int  rv;
    int  i;
    int  bcm_unit;

    BCMX_PARAM_NULL_CHECK(qmid);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_queue_map_attach_get(bcm_unit, vlan, qmid);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmx_vlan_queue_map_detach
 * Purpose:
 *      Dettached queue map object for given VLAN or VFI.
 * Parameters:
 *      vlan  - VLAN identifier
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_detach(bcm_vlan_t vlan)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_detach(bcm_unit, vlan);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}
    
/*
 * Function:
 *      bcmx_vlan_queue_map_detach_all
 * Purpose:
 *      Dettached queue map objects from all VLAN or VFI.
 * Parameters:
 *      None
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_queue_map_detach_all(void)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_queue_map_detach_all(bcm_unit);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}


/*
 * Function:
 *      bcmx_vlan_gport_add
 * Purpose:
 *      Add a gport (virtual or physical port) to the specified VLAN.
 * Parameters:
 *      vlan        - VLAN ID to add port to as a member
 *      port        - Gport ID
 *      is_untagged - untagged or not
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_add(bcm_vlan_t vlan, bcm_gport_t port, int is_untagged)
{
    int          rv = BCM_E_NONE, tmp_rv;
    int          i, bcm_unit;
    bcm_port_t   bcm_port;

    /*
     * If gport is:
     *    - physical port, apply to unit where port resides
     *    - trunk, apply to first successful unit
     *    - otherwise, apply to all units
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_gport_add(bcm_unit, vlan, port, is_untagged);
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        BCMX_UNIT_ITER(bcm_unit, i) {
            tmp_rv = bcm_vlan_gport_add(bcm_unit, vlan, port, is_untagged);
            if (BCM_SUCCESS(tmp_rv)) {
                return tmp_rv;
            }
            BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
        }

        return rv;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_gport_add(bcm_unit, vlan, port, is_untagged);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_gport_delete
 * Purpose:
 *      Delete a gport (virtual or physical port) from the specified VLAN.
 * Parameters:
 *      vlan - VLAN ID
 *      port - Gport ID
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_delete(bcm_vlan_t vlan, bcm_gport_t port)
{
    int          rv = BCM_E_NONE, tmp_rv;
    int          i, bcm_unit;
    bcm_port_t   bcm_port;

    /*
     * If gport is:
     *    - physical port, apply to unit where port resides
     *    - trunk, apply to first successful unit
     *    - otherwise, apply to all units
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_gport_delete(bcm_unit, vlan, port);
    }

    if (BCM_GPORT_IS_TRUNK(port)) {
        BCMX_UNIT_ITER(bcm_unit, i) {
            tmp_rv = bcm_vlan_gport_delete(bcm_unit, vlan, port);
            if (BCM_SUCCESS(tmp_rv)) {
                return tmp_rv;
            }
            BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
        }

        return rv;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_gport_delete(bcm_unit, vlan, port);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_gport_delete_all
 * Purpose:
 *      Delete all gports (virtual and physical port) from the specified VLAN.
 * Parameters:
 *      vlan - VLAN ID
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_delete_all(bcm_vlan_t vlan)
{
    int  rv = BCM_E_NONE, tmp_rv;
    int  i, bcm_unit;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_vlan_gport_delete_all(bcm_unit, vlan);
        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    return rv;
}

/*
 * Function:
 *      bcmx_vlan_gport_get
 * Purpose:
 *      Get tagged/untagged status for given gport (virtual or physical port)
 *      on the specified VLAN.
 * Parameters:
 *      vlan        - VLAN ID
 *      port        - Gport ID
 *      is_untagged - (OUT) TRUE if port is an untagged member of the VLAN
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_get(bcm_vlan_t vlan, bcm_gport_t port, int *is_untagged)
{
    int          rv;
    int          i, bcm_unit;
    bcm_port_t   bcm_port;

    BCMX_PARAM_NULL_CHECK(is_untagged);

    /*
     * If gport is:
     *    - physical port, get from unit where port resides
     *    - otherwise (including trunk), return from first successful get
     */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_vlan_gport_get(bcm_unit, vlan, port, is_untagged);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_vlan_gport_get(bcm_unit, vlan, port, is_untagged);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function:
 *      bcmx_vlan_gport_get_all
 * Purpose:
 *      Get all gports (virtual and physical port) for the specified VLAN.
 * Parameters:
 *      vlan        - VLAN ID
 *      array_max   - Maximum number of elements in arrays
 *      gport_array - (OUT) Array of gports
 *      is_untagged - (OUT) Array of tagged/untagged status for gports
 *      array_size  - (OUT) Actual number of returned values in array
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_vlan_gport_get_all(bcm_vlan_t vlan,
                        int array_max, bcm_gport_t *gport_array, 
                        int *is_untagged, int *array_size)
{
    int          rv = BCM_E_NONE, tmp_rv;
    int          i, bcm_unit;
    int          count, total_count, array_left;
    bcm_gport_t  *gport_tmp;
    int          *is_untagged_tmp;
    int          j, k;    

    BCMX_PARAM_NULL_CHECK(gport_array);
    BCMX_PARAM_NULL_CHECK(is_untagged);
    BCMX_PARAM_NULL_CHECK(array_size);

    if ((gport_tmp = sal_alloc(sizeof(bcm_gport_t) * array_max,
                               "bcmx vlan gport get")) == NULL) {
        return BCM_E_MEMORY;
    }
    if ((is_untagged_tmp = sal_alloc(sizeof(int) * array_max,
                                     "bcmx vlan untagged get")) == NULL) {
        sal_free(gport_tmp);
        return BCM_E_MEMORY;
    }

    /*
     * Collect gports from all units
     */
    *array_size = 0;
    total_count = 0;
    array_left  = array_max;

    BCMX_UNIT_ITER(bcm_unit, i) {
        count = 0;
        tmp_rv = bcm_vlan_gport_get_all(bcm_unit, vlan, array_left,
                                        gport_tmp, is_untagged_tmp, &count);
        if (BCM_SUCCESS(tmp_rv)) {

            /* Add ports that are not yet in the list */
            for (j = 0; j < count; j++) {
                for (k = 0; k < total_count; k++) {
                    if (gport_tmp[j] == gport_array[k]) {
                        break;
                    }
                }
                if (k == total_count) {
                    gport_array[total_count] = gport_tmp[j];
                    is_untagged[total_count] = is_untagged_tmp[j];
                    array_left--;
                    total_count++;
                }
            }
        }

        BCMX_RV_REPLACE_OK(tmp_rv, rv, BCM_E_UNAVAIL);
    }

    *array_size = total_count;

    sal_free(gport_tmp);
    sal_free(is_untagged_tmp);

    return rv;
}

