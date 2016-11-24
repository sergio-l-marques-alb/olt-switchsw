/*
 * $Id: esw_mirror.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * Mirror - Broadcom StrataSwitch Mirror API.
 *
 * The mirroring code has become more complex after the introduction
 * of XGS3 devices, which support multiple MTPs (mirror-to ports) as
 * well as directed mirroring. When directed mirroring is enabled
 * it is also possible to mirror to a trunk.
 *
 * Non-directed mirroring (aka. Draco1.5-style mirroring and XGS2-style 
 * mirroring) only allows for a single MTP in a system (which can be
 * either a single device or a stack.) In order to mirror a packet to 
 * a remote module in non-directed mode, the local MTP must be the 
 * appropriate stacking port and all modules traversed from the 
 * mirrored port to the MTP need to have mirroring configured to 
 * reflect the desired path for mirror packets.
 *
 * Directed mirroring means that the MTP info includes a module ID,
 * which allows mirror packets to follow the normal path of switched
 * packets, i.e. when mirroring to a remote MTP there is no need to 
 * configure the mirror path separately.
 *
 * Since the original mirror API did not support module IDs in the MTP
 * definition, a new API was introduced to handle this. The new API is
 * called bcm_mirror_port_set/get and allows the application to 
 * configure mirroring with a single API call, whereas the the old API
 * would require two (and in most cases three or more) API calls.
 *
 * For compatibility, the original API will also work on XGS3 devices,
 * and in this case the MTP module ID is automatically set to be the
 * local module ID. Likewise, the new API will also work on pre-XGS3
 * devices as long as the MTP module ID is specified as the local
 * module ID.
 *
 * In addition to normal ingress and egress mirroring, the FP (field
 * processor) can specify actions that include ingress and egress 
 * mirroring. This feature uses the same hardware MTP resources as
 * the mirror API, so in order to coordinate this, the FP APIs must
 * allocate MTP resources through internal reserve/unreserve 
 * functions. Since multiple FP rules can use the same MTP resource
 * the reserve/unreserve functions maintain a reference count for
 * each MTP resource. In the software MTP structure this reference
 * counter is called 'reserved'. Within the same structure, the 
 * mirror API uses the 'pbmp' to indicate whether this MTP resource
 * is being used by the mirror API.
 *
 * Note that the MTP resource management code allows resources to be 
 * shared between the mirror API and the FP whenever the requested 
 * MTP is identical.
 *
 */

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/port.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/trunk.h>

#include <bcm_int/esw_dispatch.h>

/* STATIC FUNCTIONS DECLARATION. */
STATIC int _bcm_esw_mirror_enable(int unit);
STATIC int _bcm_esw_mirror_egress_set(int unit, bcm_port_t port, int enable);
STATIC int _bcm_esw_mirror_egress_get(int unit, bcm_port_t port, int *enable);
STATIC int _bcm_esw_mirror_enable_set(int unit, int port, int enable);
STATIC int _bcm_esw_mirror_egr_dest_get (int unit, bcm_port_t port, uint32
                                         flags, bcm_pbmp_t *dest_bitmap);
STATIC int _bcm_esw_directed_mirroring_get(int unit, int *enable);

/* LOCAL VARIABLES DECLARATION. */
static _bcm_mirror_config_p _bcm_mirror_config[BCM_MAX_NUM_UNITS];


/*
 * Function:
 *	  _bcm_esw_local_modid_get
 * Purpose:
 *	  Get local unit module id. 
 * Parameters:
 *    unit    - (IN) BCM device number.
 *    modid   - (OUT)module id. 
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int
_bcm_esw_local_modid_get(int unit, int *modid)
{
    int  rv;      /* Operation return status. */

    /* Input parameters check. */
    if (NULL == modid) {
        return (BCM_E_PARAM);
    }

    /* Get local module id. */
    rv = bcm_esw_stk_my_modid_get(unit, modid);
    if (BCM_E_UNAVAIL == rv) {
        *modid = 0;
        rv = (BCM_E_NONE);
    }
    return (rv);
}

/*
 * Function:
 *      _bcm_mirror_gport_adapt
 * Description:
 *      Adapts gport encoding for dual mode devices
 * Parameters:
 *      unit        - BCM device number
 *      gport       (IN/OUT)- gport to adapt
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_mirror_gport_adapt(int unit, bcm_gport_t *gport)
{
    bcm_module_t    modid;
    bcm_port_t      port;
    bcm_trunk_t     tgid;
    int             id;
    bcm_gport_t     gport_out;

    if (NULL == gport) {
        return BCM_E_PARAM;
    }

    /* Adaptation is needed only for dual mod devices */
    if ((NUM_MODID(unit) == 2)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, *gport, &modid, 
                                   &port, &tgid, &id));
        if (-1 != id) {
            return BCM_E_PARAM;
        }
    
        if (BCM_TRUNK_INVALID != tgid) {
            BCM_GPORT_TRUNK_SET(gport_out, tgid);
        } else {
            BCM_GPORT_MODPORT_SET(gport_out, modid, port);
        }
    
        *gport = gport_out;
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_mirror_gport_resolve
 * Description:
 *      Resolves gport for mirror module for local ports
 * Parameters:
 *      unit        - (IN) BCM device number
 *      gport       - (IN)- gport to to resolve
 *      port        - (OUT)- port encoded to gport
 *      modid       - (OUT)- modid encoded to gport
 * Returns:
 *      BCM_E_XXX
 * Note :
 *      if modid == NULL port must be local port
 */
STATIC int 
_bcm_mirror_gport_resolve(int unit, bcm_gport_t gport, bcm_port_t *port, 
                          bcm_module_t *modid)
{
    bcm_module_t    lmodid;
    bcm_trunk_t     tgid;
    bcm_port_t      lport;
    int             id, islocal;

    if (NULL == port) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit,gport, &lmodid, &lport, &tgid, &id));

    if (-1 != id || BCM_TRUNK_INVALID != tgid) {
        return BCM_E_PARAM;
    }

    if (NULL == modid) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_modid_is_local(unit, lmodid, &islocal));
        if (islocal != TRUE) {
            return BCM_E_PARAM;
        }
    } else {
        *modid = lmodid;
    }
    *port = lport;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_mirror_gport_construct
 * Description:
 *      Constructs gport for mirror module
 * Parameters:
 *      unit        - (IN) BCM device number
 *      port_tgid   - (IN) port or trunk id to construct into a gprot
 *      modid       - (IN) module id to construct into a gport
 *      flags       - (IN) Mirror trunk flag
 *      gport       - (OUT)- gport to to construct
 * Returns:
 *      BCM_E_XXX
 */
STATIC int 
_bcm_mirror_gport_construct(int unit, int port_tgid, int modid, uint32 flags, 
                            bcm_gport_t *gport)
{
    _bcm_gport_dest_t   dest;
    bcm_module_t        mymodid;
    int                 rv;

    _bcm_gport_dest_t_init(&dest);
    if (flags & BCM_MIRROR_PORT_DEST_TRUNK) {
        dest.tgid = port_tgid;
        dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
    } else {
        dest.port = port_tgid;
        if (IS_ST_PORT(unit, port_tgid)) {
            rv = bcm_esw_stk_my_modid_get(unit, &mymodid);
            if (BCM_E_UNAVAIL == rv) {
                dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
            } else {
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                dest.modid = modid;
            }
        } else {
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            dest.modid = modid;
        }
    }
    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_construct(unit, &dest, gport));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_mirror_destination_gport_parse
 * Description:
 *      Parse mirror destinations gport.
 * Parameters:
 *      unit      - BCM device number
 *      mirror_dest_id - mirror destination id. 
 *      dest_mod  - (OUT) module id of mirror-to port
 *      dest_port - (OUT) mirror-to port
 *      flags     - (OUT) Trunk flag
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_mirror_destination_gport_parse(int unit, bcm_gport_t mirror_dest_id,
                                    bcm_module_t *dest_mod, bcm_port_t *dest_port,
                                    uint32 *flags)
{
    bcm_mirror_destination_t mirror_dest;
    bcm_module_t             modid;
    bcm_port_t               port;
    bcm_trunk_t              tgid;
    int                      id;


    BCM_IF_ERROR_RETURN
        (bcm_esw_mirror_destination_get(unit, mirror_dest_id, &mirror_dest));

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, mirror_dest.gport, &modid, &port, 
                               &tgid, &id));

    if (-1 != id) {
        return BCM_E_PARAM;
    }

    if (BCM_TRUNK_INVALID != tgid) {
        if (NULL != dest_mod) {
            *dest_mod  = -1;
        }
        if (NULL != dest_port) { 
            *dest_port = tgid;
        }
        if (NULL != flags) {
            *flags |= BCM_MIRROR_PORT_DEST_TRUNK;
        }
    } else {
        if (NULL != dest_mod) {
            *dest_mod = modid;
        }
        if (NULL != dest_port) {
            *dest_port = port;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_mirror_destination_match
 * Description:
 *      Limited match utility used to identify mirror destination
 *      with identical gport. 
 * Parameters:
 *      unit           - (IN) BCM device number
 *      mirror_dest    - (IN) Mirror destination. 
 *      mirror_dest_id - (OUT)Matching mirror destination id. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_mirror_destination_match(int unit, bcm_mirror_destination_t *mirror_dest,
                              bcm_gport_t *mirror_dest_id) 
                        
{
    int idx;                         /* Mirror destinations iteration index.*/
    _bcm_mirror_dest_config_p  mdest;/* Mirror destination description.     */
    bcm_module_t mymodid;            /* Local module id.              */
    int          directed;           /* Directed mirroring indicator. */
    bcm_module_t dest_mod;           /* Destination module id.        */
    bcm_port_t   dest_port;          /* Destination port number.      */

    /* Input parameters check. */
    if ((NULL == mirror_dest_id) || (NULL == mirror_dest)) {
        return (BCM_E_PARAM);
    }

    /* Get local modid. */
    BCM_IF_ERROR_RETURN(_bcm_esw_local_modid_get(unit, &mymodid));

    /* Check if directed mirroring is enabled. */
    BCM_IF_ERROR_RETURN(_bcm_esw_directed_mirroring_get(unit, &directed));

    /* Directed  mirroring support check. */
    if ((0 == directed)){
        /* Set mirror destination to outgoing port on local module. */
        dest_mod = BCM_GPORT_MODPORT_MODID_GET(mirror_dest->gport);
        if (mymodid != dest_mod) {
            if (!SOC_IS_TUCANA(unit) || mymodid + 1 != dest_mod) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_topo_port_get(unit, dest_mod, &dest_port));
                if (SOC_IS_TUCANA(unit) && dest_port >= 32) {
                    mymodid +=1;
                    dest_port -=32;
                }
                BCM_GPORT_MODPORT_SET(mirror_dest->gport, mymodid, dest_port);
            }
        }
    }

    /* Find unused mirror destination & allocate it. */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->dest_count; idx++) {
        mdest = MIRROR_CONFIG(unit)->dest_arr + idx;
        /* Skip unused entries. */
        if (0 == mdest->ref_count) {
            continue;
        }

        /* Skip tunnel destinations. */
        if ((mdest->mirror_dest.flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) || 
            (mdest->mirror_dest.flags & BCM_MIRROR_DEST_TUNNEL_L2)) { 
            continue;
        }

        if (mdest->mirror_dest.gport == mirror_dest->gport) {
            /* Matching mirror destination found. */
            *mirror_dest_id = mdest->mirror_dest.mirror_dest_id;
            return (BCM_E_NONE);
        }
    }
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *      _bcm_esw_mirror_ingress_mtp_match
 * Description:
 *      Match a mirror-to port with one of the mtp indexes.
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      dest_port   - (IN)  Mirror destination gport.
 *      match_idx   - (OUT) MTP index matching destination. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_ingress_mtp_match(int unit, bcm_gport_t gport, int *match_idx)
{
    int idx;                                 /* Mtp iteration index. */

    /* Input parameters check. */
    if (NULL == match_idx) {
        return (BCM_E_PARAM);
    }

    /* Look for existing MTP in use */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->ing_mtp_count; idx++) {
        if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, idx)) {
            if (gport == MIRROR_CONFIG_ING_MTP_DEST(unit, idx)) {
                *match_idx = idx;
                return (BCM_E_NONE);
            }
        } 
    }
    return (BCM_E_NOT_FOUND);
}

/*
 * Function:
 *      _bcm_esw_mirror_egress_mtp_match
 * Description:
 *      Match a mirror-to port with one of the mtp indexes.
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      dest_port   - (IN)  Mirror destination gport.
 *      match_idx   - (OUT) MTP index matching destination. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_egress_mtp_match(int unit, bcm_gport_t gport, int *match_idx)
{
    int idx;                                 /* Mtp iteration index. */

    /* Input parameters check. */
    if (NULL == match_idx) {
        return (BCM_E_PARAM);
    }

    /* Look for existing MTP in use */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->egr_mtp_count; idx++) {
        if (MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, idx)) {
            if (gport == MIRROR_CONFIG_EGR_MTP_DEST(unit, idx)) {
                *match_idx = idx;
                return (BCM_E_NONE);
            }
        }
    }
    return (BCM_E_NOT_FOUND);
}

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *      _bcm_esw_mirror_egress_true_mtp_match
 * Description:
 *      Match a mirror-to port with one of the mtp indexes.
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      dest_port   - (IN)  Mirror destination gport.
 *      match_idx   - (OUT) MTP index matching destination. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_egress_true_mtp_match(int unit, bcm_gport_t gport,
                                      int *match_idx)
{
    int idx;                                 /* Mtp iteration index. */

    /* Input parameters check. */
    if (NULL == match_idx) {
        return (BCM_E_PARAM);
    }

    /* Look for existing MTP in use */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->egr_true_mtp_count; idx++) {
        if (MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, idx)) {
            if (gport == MIRROR_CONFIG_EGR_TRUE_MTP_DEST(unit, idx)) {
                *match_idx = idx;
                return (BCM_E_NONE);
            }
        }
    }
    return (BCM_E_NOT_FOUND);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *	  _bcm_esw_mirror_destination_find
 * Purpose:
 *	  Find mirror destination for all gport types. 
 * Parameters:
 *    unit    - (IN) BCM device number.
 *    port    - (IN) port, gport or mirror gport
 *    modid   - (IN) module id.
 *    flags   - (IN) BCM_MIRROR_PORT_DEST_* flags 
 *    mirror_dest - (OUT) mirror destination 
 * Returns:
 *	  BCM_E_XXX
 */

STATIC int 
_bcm_esw_mirror_destination_find(int unit, bcm_port_t port, bcm_module_t modid, 
                                 uint32 flags, bcm_mirror_destination_t *mirror_dest)
{
    if (NULL == mirror_dest) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        /* If gport passed, work with it directly */
        mirror_dest->gport = port;
    } else {
        /* If not gport then construct the gport from given parameters.*/
        if (flags & BCM_MIRROR_PORT_DEST_TRUNK) {
            /* Mirror destination is a trunk. */
            BCM_GPORT_TRUNK_SET(mirror_dest->gport, port);
        } else {
            /* Convert port + mod to GPORT format. No trunking destination support. */
            if (-1 == modid) { 
                /* Get local modid. */
                BCM_IF_ERROR_RETURN(
                    _bcm_esw_local_modid_get(unit, &modid));
            } else if (!SOC_MODID_ADDRESSABLE(unit, modid)){
                return BCM_E_PARAM;
            }

            BCM_GPORT_MODPORT_SET(mirror_dest->gport, modid, port);
        }            
    }

    /* Adapt miror destination gport */
    BCM_IF_ERROR_RETURN(
        _bcm_mirror_gport_adapt(unit, &(mirror_dest->gport)));

    /* Find matching mirror destination */
    BCM_IF_ERROR_RETURN(
        _bcm_mirror_destination_match(unit, mirror_dest,
                                      &(mirror_dest->mirror_dest_id)));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_mirror_destination_alloc
 * Purpose:
 *     Allocate mirror destination description.
 * Parameters:
 *      unit           - (IN) BCM device number. 
 *      mirror_dest_id - (OUT) Mirror destination id.
 * Returns:
 *      BCM_X_XXX
 */

/* Create a mirror (destination, encapsulation) pair. */
STATIC int 
_bcm_mirror_destination_alloc(int unit, bcm_gport_t *mirror_dest_id) 
{
    int idx;                          /* Mirror destinations iteration index.*/
    _bcm_mirror_dest_config_p  mdest; /* Mirror destination description.     */

    /* Input parameters check. */
    if (NULL == mirror_dest_id) {
        return (BCM_E_PARAM);
    }

    /* Find unused mirror destination & allocate it. */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->dest_count; idx++) {
        mdest = MIRROR_CONFIG(unit)->dest_arr + idx;
        if (mdest->ref_count) {
            continue;
        }
        mdest->ref_count++;
        *mirror_dest_id = mdest->mirror_dest.mirror_dest_id;
        return (BCM_E_NONE);
    }

    /* All mirror destinations are used. */
    return (BCM_E_FULL);
}

/*
 * Function:
 *     _bcm_mirror_destination_free
 * Purpose:
 *     Free mirror destination description.
 * Parameters:
 *      unit           - (IN) BCM device number. 
 *      mirror_dest_id - (IN) Mirror destination id.
 * Returns:
 *      BCM_X_XXX
 */

/* Create a mirror (destination, encapsulation) pair. */
STATIC int 
_bcm_mirror_destination_free(int unit, bcm_gport_t mirror_dest_id) 
{
    _bcm_mirror_dest_config_p  mdest_cfg; /* Mirror destination config.*/

    mdest_cfg = &MIRROR_DEST_CONFIG(unit, mirror_dest_id); 

    if (mdest_cfg->ref_count > 0) {
        mdest_cfg->ref_count--;
    }

    if (0 == mdest_cfg->ref_count) {
        sal_memset(&mdest_cfg->mirror_dest, 0, sizeof(bcm_mirror_destination_t));
        mdest_cfg->mirror_dest.mirror_dest_id = mirror_dest_id;
        mdest_cfg->mirror_dest.gport = BCM_GPORT_INVALID;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_esw_mirror_destination_create
 * Purpose:
 *     Helper function to API that creates mirror destination description.
 * Parameters:
 *      unit         - (IN) BCM device number. 
 *      mirror_dest  - (IN) Mirror destination description.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int 
_bcm_esw_mirror_destination_create(int unit, bcm_mirror_destination_t *mirror_dest)
{
    if (mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID) {
        /* Check mirror destination id */
        if (0 == BCM_GPORT_IS_MIRROR(mirror_dest->mirror_dest_id)) {
            return (BCM_E_BADID);
        }

        /* Check if mirror destination is being updated  */
        if (0 != MIRROR_DEST_REF_COUNT(unit, mirror_dest->mirror_dest_id)) { 
            if (0 == (mirror_dest->flags & BCM_MIRROR_DEST_REPLACE)) {
                return (BCM_E_EXISTS);
            }
        } else {
            MIRROR_DEST_REF_COUNT(unit, mirror_dest->mirror_dest_id) = 1;
        }
    } else {
        /* Allocate new mirror destination. */
        BCM_IF_ERROR_RETURN(
            _bcm_mirror_destination_alloc(unit, &mirror_dest->mirror_dest_id));
    }

    /* Set mirror destination configuration. */
    *(MIRROR_DEST(unit, mirror_dest->mirror_dest_id)) = *mirror_dest; 

    return (BCM_E_NONE);
}

#if defined(BCM_TUCANA_SUPPORT)
/*
 * Function:
 *  	_bcm_tucana_mirror_egr_dest_get 
 * Purpose:
 *  	Get destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)port number.
 *      dest_bitmap - (OUT) destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_tucana_mirror_egr_dest_get(int unit, bcm_port_t port, 
                                bcm_pbmp_t *dest_bitmap)
{
    uint64 mirror;

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

    if (IS_E_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(READ_MIRROR_DEST_BITMAPr(unit, port, &mirror));
        SOC_PBMP_WORD_SET(*dest_bitmap, 0, 
                          soc_reg64_field32_get(unit, MIRROR_DEST_BITMAPr,
                                                mirror, DEST_BITMAP_0f));
        SOC_PBMP_WORD_SET(*dest_bitmap, 1, 
                          soc_reg64_field32_get(unit, MIRROR_DEST_BITMAPr,
                                                mirror, DEST_BITMAP_1f));
    } else if (IS_HG_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(READ_IMIRROR_DEST_BITMAPr(unit, port, &mirror));
        SOC_PBMP_WORD_SET(*dest_bitmap, 0, 
                          soc_reg64_field32_get(unit, IMIRROR_DEST_BITMAPr,
                                                mirror, DEST_BITMAP_0f));
        SOC_PBMP_WORD_SET(*dest_bitmap, 1, 
                          soc_reg64_field32_get(unit, IMIRROR_DEST_BITMAPr,
                                                mirror, DEST_BITMAP_1f));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *  	_bcm_tucana_mirror_egr_dest_set 
 * Purpose:
 *  	Set destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)Port number.
 *      dest_bitmap - (IN)Destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_tucana_mirror_egr_dest_set (int unit, bcm_port_t port, 
                                 bcm_pbmp_t *dest_bitmap)
{
    uint32 values[2];
    soc_reg_t reg;
    soc_field_t fields[] = {DEST_BITMAP_0f, DEST_BITMAP_1f};

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }
    values[0] = SOC_PBMP_WORD_GET(*dest_bitmap, 0);
    values[1] = SOC_PBMP_WORD_GET(*dest_bitmap, 1);

    if (IS_E_PORT(unit, port)) {
        reg =  MIRROR_DEST_BITMAPr;
    } else if (IS_HG_PORT(unit, port)) {
        reg =  IMIRROR_DEST_BITMAPr;
    } else if (IS_CPU_PORT(unit, port)) {
        return (BCM_E_NONE);
    } else { 
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, reg, port, 
                                 COUNTOF(values), fields, values));

    return (BCM_E_NONE);
}

#endif /* BCM_TUCANA_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
/*
 * Function:
 *  	_bcm_tr2_mirror_egr_dest_get 
 * Purpose:
 *  	Get destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)port number.
 *      flags       - (IN) Destination flags.(BCM_MIRROR_MTP_ONE/TWO)
 *      dest_bitmap - (OUT) destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_tr2_mirror_egr_dest_get(int unit, bcm_port_t port, uint32 flags, 
                             bcm_pbmp_t *dest_bitmap)
{
    soc_reg_t  reg;              /* Egress mirror control register.  */
    uint64 mirror;               /* Egress mirror control reg value. */ 

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

    switch(flags) {
    case BCM_MIRROR_MTP_ONE:
        reg = EMIRROR_CONTROL_64r;
        break;
    case BCM_MIRROR_MTP_TWO:
        reg = EMIRROR_CONTROL1_64r;
        break;
    case BCM_MIRROR_MTP_THREE:
        reg = EMIRROR_CONTROL2_64r;
        break;
    case BCM_MIRROR_MTP_FOUR:
        reg = EMIRROR_CONTROL3_64r;
        break;
    default:
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN 
        (soc_reg_read(unit, reg, soc_reg_addr(unit, reg, port, 0), &mirror));

    SOC_PBMP_WORD_SET(*dest_bitmap, 0,
                      soc_reg64_field32_get(unit, reg, mirror, BITMAP_LOf));
    SOC_PBMP_WORD_SET(*dest_bitmap, 1, 
                      soc_reg64_field32_get(unit, reg, mirror, BITMAP_HIf));
    return (BCM_E_NONE);
}

/*
 * Function:
 *  	_bcm_tr2_mirror_egr_dest_set 
 * Purpose:
 *  	Set destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)Port number.
 *      flags       - (IN) Destination flags.(BCM_MIRROR_MTP_ONE/TWO)
 *      dest_bitmap - (IN)Destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_tr2_mirror_egr_dest_set (int unit, bcm_port_t port, 
                              uint32 flags, bcm_pbmp_t *dest_bitmap)
{
    soc_reg_t  reg;              /* Egress mirror control register.  */
    uint32 values[2];
    soc_field_t fields[] = {BITMAP_LOf, BITMAP_HIf};

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

    switch(flags) {
    case BCM_MIRROR_MTP_ONE:
        reg = EMIRROR_CONTROL_64r;
        break;
    case BCM_MIRROR_MTP_TWO:
        reg = EMIRROR_CONTROL1_64r;
        break;
    case BCM_MIRROR_MTP_THREE:
        reg = EMIRROR_CONTROL2_64r;
        break;
    case BCM_MIRROR_MTP_FOUR:
        reg = EMIRROR_CONTROL3_64r;
        break;
    default:
        return BCM_E_PARAM;
    }

    values[0] = SOC_PBMP_WORD_GET(*dest_bitmap, 0);
    values[1] = SOC_PBMP_WORD_GET(*dest_bitmap, 1);

    BCM_IF_ERROR_RETURN 
        (soc_reg_fields32_modify(unit, reg, port, 
                                 COUNTOF(values), fields, values));

    /* Enable mirroring of CPU Higig packets as well */
    if (IS_CPU_PORT(unit, port)) {
        reg = (flags & BCM_MIRROR_MTP_ONE) ? IEMIRROR_CONTROL_64r : \
                     IEMIRROR_CONTROL1_64r; 

        if(SOC_IS_ENDURO(unit)) {
            /* There is no BITMAP_HIf in XXXX_64r for Enduro */
            BCM_IF_ERROR_RETURN 
                (soc_reg_fields32_modify(unit, reg, port, 
                                         1, fields, values));
        } else{
            BCM_IF_ERROR_RETURN 
                (soc_reg_fields32_modify(unit, reg, port, 
                                         COUNTOF(values), fields, values));
        }

    }
    return (BCM_E_NONE);
}

#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
/*
 * Function:
 *  	_bcm_triumph_mirror_egr_dest_get 
 * Purpose:
 *  	Get destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)port number.
 *      flags       - (IN) Destination flags.(BCM_MIRROR_MTP_ONE/TWO)
 *      dest_bitmap - (OUT) destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_triumph_mirror_egr_dest_get(int unit, bcm_port_t port, uint32 flags, 
                                  bcm_pbmp_t *dest_bitmap)
{
    soc_reg_t  reg;              /* Egress mirror control register.  */
    uint64 mirror;               /* Egress mirror control reg value. */ 

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

    reg = (flags & BCM_MIRROR_MTP_ONE) ? EMIRROR_CONTROL_64r : \
        EMIRROR_CONTROL1_64r; 

    BCM_IF_ERROR_RETURN 
        (soc_reg_read(unit, reg, soc_reg_addr(unit, reg, port, 0), &mirror));

    SOC_PBMP_WORD_SET(*dest_bitmap, 0,
                      soc_reg64_field32_get(unit, reg, mirror, BITMAP_LOf));
    if(!SOC_IS_ENDURO(unit)) {
        /* There is no BITMAP_HIf in XXXX_64r for Enduro */
        SOC_PBMP_WORD_SET(*dest_bitmap, 1, 
                          soc_reg64_field32_get(unit, reg, mirror, BITMAP_HIf));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *  	_bcm_triumph_mirror_egr_dest_set 
 * Purpose:
 *  	Set destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)Port number.
 *      flags       - (IN) Destination flags.(BCM_MIRROR_MTP_ONE/TWO)
 *      dest_bitmap - (IN)Destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_triumph_mirror_egr_dest_set (int unit, bcm_port_t port, 
                                  uint32 flags, bcm_pbmp_t *dest_bitmap)
{
    soc_reg_t  reg;              /* Egress mirror control register.  */
    uint32 values[2];
    soc_field_t fields[] = {BITMAP_LOf, BITMAP_HIf};

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

    reg = (flags & BCM_MIRROR_MTP_ONE) ? EMIRROR_CONTROL_64r : \
        EMIRROR_CONTROL1_64r; 


    values[0] = SOC_PBMP_WORD_GET(*dest_bitmap, 0);
    values[1] = SOC_PBMP_WORD_GET(*dest_bitmap, 1);

    if(SOC_IS_ENDURO(unit)) {
        /* There is no BITMAP_HIf in XXXX_64r for Enduro */
        BCM_IF_ERROR_RETURN 
            (soc_reg_fields32_modify(unit, reg, port, 
                                     1, fields, values));
    } else {
        BCM_IF_ERROR_RETURN 
            (soc_reg_fields32_modify(unit, reg, port, 
                                     COUNTOF(values), fields, values));
    }

    /* Enable mirroring of CPU Higig packets as well */
    if (IS_CPU_PORT(unit, port)) {
        reg = (flags & BCM_MIRROR_MTP_ONE) ? IEMIRROR_CONTROL_64r : \
                     IEMIRROR_CONTROL1_64r; 

        if(SOC_IS_ENDURO(unit)) {
            /* There is no BITMAP_HIf in XXXX_64r for Enduro */
            BCM_IF_ERROR_RETURN 
                (soc_reg_fields32_modify(unit, reg, port, 
                                         1, fields, values));
        } else{
            BCM_IF_ERROR_RETURN 
                (soc_reg_fields32_modify(unit, reg, port, 
                                         COUNTOF(values), fields, values));
        }

    }
    return (BCM_E_NONE);
}

#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT)
/*
 * Function:
 *  	_bcm_raptor_mirror_egr_dest_get 
 * Purpose:
 *  	Get destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)port number.
 *      dest_bitmap - (OUT) destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_raptor_mirror_egr_dest_get(int unit, bcm_port_t port, 
                                bcm_pbmp_t *dest_bitmap)
{
    uint32 mirror;

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(READ_EMIRROR_CONTROLr(unit, port, &mirror));
    SOC_PBMP_WORD_SET(*dest_bitmap, 0, 
        soc_reg_field_get(unit, EMIRROR_CONTROLr, mirror, BITMAPf));
    
    BCM_IF_ERROR_RETURN(READ_EMIRROR_CONTROL_HIr(unit, port, &mirror));
    SOC_PBMP_WORD_SET(*dest_bitmap, 1, 
        soc_reg_field_get(unit, EMIRROR_CONTROL_HIr, mirror, BITMAPf));

    return (BCM_E_NONE);
}


/*
 * Function:
 *  	_bcm_raptor_mirror_egr_dest_set 
 * Purpose:
 *  	Set destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)Port number.
 *      dest_bitmap - (IN)Destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_raptor_mirror_egr_dest_set(int unit, bcm_port_t port, 
                                bcm_pbmp_t *dest_bitmap)
{
    uint32 value;
    soc_field_t field = BITMAPf;

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }
    value = SOC_PBMP_WORD_GET(*dest_bitmap, 0);

    BCM_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, EMIRROR_CONTROLr, port, 
                                 1, &field, &value));

    /* Enable mirroring of CPU Higig packets as well */
    if (IS_CPU_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, IEMIRROR_CONTROLr, port, 
                                     1, &field, &value));

    }

    value = SOC_PBMP_WORD_GET(*dest_bitmap, 1);

    BCM_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, EMIRROR_CONTROL_HIr, port, 
                                 1, &field, &value));

    /* Enable mirroring of CPU Higig packets as well */
    if (IS_CPU_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, IEMIRROR_CONTROL_HIr, port, 
                                     1, &field, &value));

    }
    return (BCM_E_NONE);
}

#endif /* BCM_RAPTOR_SUPPORT */

#if defined(BCM_XGS12_SWITCH_SUPPORT)
/*
 * Function:
 *	   _bcm_xgs_mirror_enable_set 
 * Purpose:
 *  	Enable/disable mirroring on a port & set mirror-to port.
 * Parameters:
 *	    unit - BCM device number
 *  	port - port number
 *   	enable - enable mirroring if non-zero
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_xgs_mirror_enable_set(int unit, int port, int enable)
{
    int mstack, mport; 
    bcm_module_t    mmod, mymodid;
    uint32 values[4]; 
    soc_field_t fields[4] = {M_PORTf, M_ENABLEf, M_ON_PORTf, M_STACKf}; 

    /* Get my module id - to support Tucana */
    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &mymodid));
    /* Clear port when disabling */
    if (enable && MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)) {
       BCM_IF_ERROR_RETURN(_bcm_mirror_destination_gport_parse(unit,
                                           MIRROR_CONFIG_ING_MTP_DEST(unit, 0),
                                           &mmod , &mport, NULL));
       /* Tucana case */
       if (mymodid != mmod) {
           mport += 32;
       }
    } else {
        mport = 0;
    }

    /* Check if stack port is mirror to */
    if (enable && SOC_PBMP_MEMBER(SOC_PBMP_STACK_CURRENT(unit), mport)) {
        mstack = 1;
    } else {
        mstack = 0;
    }

    values[0] = mport;
    values[1] = enable;
    values[2] = ((port == mport) && enable) ? 1 : 0;
    values[3] = mstack;

    if (IS_E_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, MIRROR_CONTROLr, port, 
                                     (SOC_IS_LYNX(unit) ? 3 : 4),
                                     fields, values));

    } else if (IS_HG_PORT(unit, port)) {
        fields[2] = M_STACKf;
        values[2] = mstack;
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, IMIRROR_CONTROLr, port, 
                                     (SOC_IS_LYNX(unit) ? 2 : 3),
                                     fields, values));
    } else if (0 == IS_CPU_PORT(unit, port)) {
        return (BCM_E_UNAVAIL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *  	_bcm_xgs_mirror_egr_dest_get 
 * Purpose:
 *  	Get destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)port number.
 *      dest_bitmap - (OUT) destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_xgs_mirror_egr_dest_get(int unit, bcm_port_t port, bcm_pbmp_t *dest_bitmap)
{
    uint64 mirror;

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

    if (IS_E_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &mirror));
        SOC_PBMP_WORD_SET(*dest_bitmap, 0, 
            soc_reg64_field32_get(unit, MIRROR_CONTROLr, mirror, DEST_BITMAPf));
    } else if (IS_HG_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(READ_IMIRROR_CONTROLr(unit, port, &mirror));
        SOC_PBMP_WORD_SET(*dest_bitmap, 0, 
            soc_reg64_field32_get(unit, IMIRROR_CONTROLr, mirror, DEST_BITMAPf));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *  	_bcm_xgs_mirror_egr_dest_set 
 * Purpose:
 *  	Set destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)Port number.
 *      dest_bitmap - (IN)Destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_xgs_mirror_egr_dest_set(int unit, bcm_port_t port, 
                              bcm_pbmp_t *dest_bitmap)
{
    uint32 value;
    soc_reg_t reg;
    soc_field_t field;

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

    value = SOC_PBMP_WORD_GET(*dest_bitmap, 0);
    field = DEST_BITMAPf;

    if (IS_E_PORT(unit, port)) {
        reg =  MIRROR_CONTROLr;
    } else if (IS_HG_PORT(unit, port)) {
        reg =  IMIRROR_CONTROLr;
    } else if (IS_CPU_PORT(unit, port)) {
        return (BCM_E_NONE);
    } else { 
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN (soc_reg_fields32_modify(unit, reg, port, 
                                                 1, &field, &value));

    return (BCM_E_NONE);
}

/*
 * Function:
 *  	bcm_xgs_mirror_pfmt_set
 * Description:
 *  	Set the mirroring preserve format field
 * Parameters:
 *	    unit   - (IN)BCM device number
 *  	enable - (IN)Value for preserve format on/off
 * Returns:
 *	    BCM_E_XXX
 */
int
bcm_xgs_mirror_pfmt_set(int unit, int enable)
{
    bcm_port_t	port;
    int value =  enable ? 1 : 0;

    PBMP_E_ITER(unit, port) {
        BCM_IF_ERROR_RETURN 
            (soc_reg_field32_modify(unit, MIRROR_CONTROLr, port, 
                                    M_PRESERVE_FMTf, value));

    }

    PBMP_HG_ITER(unit, port) {
        BCM_IF_ERROR_RETURN 
            (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, port, 
                                    M_PRESERVE_FMTf, value));
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *  	bcm_xgs_mirror_pfmt_get
 * Description:
 *  	Get the mirroring preserve format field
 * Parameters:
 *	    unit   - (IN)BCM device number
 *  	enable - (OUT)Value for preserve format on/off
 * Returns:
 *	    BCM_E_XXX
 */
int
bcm_xgs_mirror_pfmt_get(int unit, int *enable)
{
    uint64	mirror;
    bcm_port_t	port;

    /* Input parameters check. */
    if (NULL == enable) {
        return (BCM_E_PARAM);
    }

	COMPILER_64_ZERO(mirror);

    /* Read status from the first valid port. */
    PBMP_E_ITER(unit, port) {
        BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &mirror));
        break;
    }
    *enable = soc_reg64_field32_get(unit, MIRROR_CONTROLr, mirror,
                                    M_PRESERVE_FMTf);
    return (BCM_E_NONE);
}

#endif /* BCM_XGS12_SWITCH_SUPPORT */

#if defined(BCM_XGS12_FABRIC_SUPPORT)
/*
 * Function:
 *	   _bcm_xgs_fabric_mirror_enable_set 
 * Purpose:
 *  	Enable/disable mirroring on a port & set mirror-to port.
 * Parameters:
 *	    unit - BCM device number
 *  	port - port number
 *   	enable - enable mirroring if non-zero
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_xgs_fabric_mirror_enable_set(int unit, int port, int enable)
{
    pbmp_t ppbm;
    int mport;

    if (!IS_HG_PORT(unit, port)) {
        return (BCM_E_UNAVAIL);
    }

    /* Clear port when disabling */
    if (enable && MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)) {
       BCM_IF_ERROR_RETURN(_bcm_mirror_destination_gport_parse(unit,
                                           MIRROR_CONFIG_ING_MTP_DEST(unit, 0),
                                           NULL , &mport, NULL));
    } else {
        mport = 0;
    }

    SOC_PBMP_CLEAR(ppbm);
    if (enable) {
        SOC_PBMP_PORT_ADD(ppbm, mport);
    }

#ifdef	BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        int    m5670;

        m5670 = soc_property_get(unit, spn_MIRROR_5670, 0);
        BCM_IF_ERROR_RETURN 
            (soc_reg_field32_modify(unit, ING_CTRLr, port,
                                    DISABLE_MIRROR_CHANGEf,
                                    (m5670 | !enable) ? 1 : 0));
    }
#endif	/* BCM_HERCULES15_SUPPORT */
    BCM_IF_ERROR_RETURN
        (WRITE_ING_MIRTOBMAPr(unit, port,
                              SOC_PBMP_WORD_GET(ppbm, 0)));
    return (BCM_E_NONE);
}
#endif /* BCM_XGS12_FABRIC_SUPPORT */
/*
 * Function:
 *     _bcm_mirror_dest_get_all
 * Purpose:
 *     Get all mirroring destinations.   
 * Parameters:
 *     unit             - (IN) BCM device number. 
 *     flags            - (IN) BCM_MIRROR_PORT_XXX flags.
 *     mirror_dest_size - (IN) Preallocated mirror_dest array size.
 *     mirror_dest      - (OUT)Filled array of port mirroring destinations
 *     mirror_dest_count - (OUT)Actual number of mirroring destinations filled.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_mirror_dest_get_all(int unit, uint32 flags, int mirror_dest_size,
                         bcm_gport_t *mirror_dest, int *mirror_dest_count)
{
    int idx = 0;
    int index = 0;  

    /* Input parameters check. */
    if ((NULL == mirror_dest) || (NULL == mirror_dest_count)) {
        return (BCM_E_PARAM);
    }


    /* Copy all used ingress mirror destinations. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        for (idx = 0; idx < MIRROR_CONFIG(unit)->ing_mtp_count; idx++) {
            if ((index < mirror_dest_size) && 
                (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, idx))) {
                mirror_dest[index] = MIRROR_CONFIG_ING_MTP_DEST(unit, idx);
                index++;
            }
        }
    } 

    /* Copy all used egress mirror destinations. */
    if (flags & BCM_MIRROR_PORT_EGRESS) {
        for (idx = 0; idx < MIRROR_CONFIG(unit)->egr_mtp_count; idx++) {
            if ((index < mirror_dest_size) && 
                (MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, idx))) {
                mirror_dest[index] = MIRROR_CONFIG_EGR_MTP_DEST(unit, idx);
                index++;
            }
        }
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    /* Copy all used egress mirror destinations. */
    if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
        for (idx = 0; idx < MIRROR_CONFIG(unit)->egr_true_mtp_count; idx++) {
            if ((index < mirror_dest_size) && 
                (MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, idx))) {
                mirror_dest[index] =
                    MIRROR_CONFIG_EGR_TRUE_MTP_DEST(unit, idx);
                index++;
            }
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
    *mirror_dest_count = index;
    return (BCM_E_NONE);
}

#if defined(BCM_XGS3_SWITCH_SUPPORT)

#if defined(BCM_EASYRIDER_SUPPORT) 
/*
 * Function:
 *	    _bcm_er_mtp_init
 * Purpose:
 *	   Initialize mirror target port for Easyrider devices. 
 * Parameters:
 *	   unit       - (IN)BCM device number
 *     index      - (IN)Mtp index.
 *     trunk_arr  - (IN)Trunk members array. 
 *     flags      - (IN)Filled entry flags(BCM_MIRROR_PORT_INGRESS/EGRESS
 *                    or both. In case both flags are specied
 *                    ingress & egress configuration is assumed to be
 *                    idential.
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_er_mtp_init(int unit, int index, bcm_gport_t *trunk_arr, int flags)
{
    bcm_gport_t       mirror_dest;
    _bcm_mtp_config_p mtp_cfg;
    int               offset;
    bcm_module_t      modid;
    bcm_port_t        port;
    uint32            mtp;
    int               idx;

    /* Input parameters check */ 
    if (NULL == trunk_arr) {
        return (BCM_E_PARAM);
    }

    /* Get mtp configuration structure by direction & index. */
    mtp_cfg = (flags & BCM_MIRROR_PORT_INGRESS) ? \
        &MIRROR_CONFIG_ING_MTP(unit, index) : \
        &MIRROR_CONFIG_EGR_MTP(unit, index);

    /* Parse destination port & module. */
    mirror_dest = MIRROR_DEST_GPORT(unit, mtp_cfg->dest_id);
    modid = BCM_GPORT_MODPORT_MODID_GET(mirror_dest);
    port = BCM_GPORT_MODPORT_PORT_GET(mirror_dest);
    
    /* Hw buffer preparation. */
    mtp = 0;
    soc_reg_field_set(unit, IM_MTP_INDEXr, &mtp, PORT_TGIDf, port);
    soc_reg_field_set(unit, IM_MTP_INDEXr, &mtp, MODULE_IDf, modid);

    /* HW write. based on mirrored traffic direction. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        BCM_IF_ERROR_RETURN(WRITE_IM_MTP_INDEXr(unit, index, mtp));
    }

    /* EM_MTP_INDEX has same layout as IM_MTP_INDEX */
    if (flags & BCM_MIRROR_PORT_EGRESS) {
        BCM_IF_ERROR_RETURN(WRITE_EM_MTP_INDEXr(unit, index, mtp));
    }

    /* Program trunk member ports. */
    offset = index * BCM_SWITCH_TRUNK_MAX_PORTCNT;
    for (idx = 0; idx < BCM_SWITCH_TRUNK_MAX_PORTCNT; idx++, offset++) {
        mtp = 0;
        modid = BCM_GPORT_MODPORT_MODID_GET(trunk_arr[idx]);
        port = BCM_GPORT_MODPORT_PORT_GET(trunk_arr[idx]);
        soc_reg_field_set(unit, EGR_IM_MTP_INDEXr, &mtp, MTP_DST_PORTf, port);
        soc_reg_field_set(unit, EGR_IM_MTP_INDEXr, &mtp, MTP_DST_MODIDf, modid);

        /* HW write. based on mirrored traffic direction. */
        if (flags & BCM_MIRROR_PORT_INGRESS) {
            BCM_IF_ERROR_RETURN (WRITE_EGR_IM_MTP_INDEXr(unit, offset, mtp));
        }

        /* EGR_EM_MTP_INDEX has same layout as EGR_IM_MTP_INDEX */
        if (flags & BCM_MIRROR_PORT_EGRESS) {
            BCM_IF_ERROR_RETURN(WRITE_EGR_EM_MTP_INDEXr(unit, offset, mtp));
        } 
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *	    _bcm_er_mtp_reset
 * Purpose:
 *	   Reset mirror target port for Easyrider devices. 
 * Parameters:
 *	   unit       - (IN)BCM device number
 *     index      - (IN)Mtp index.
 *     flags      - (IN)Filled entry flags(BCM_MIRROR_PORT_INGRESS/EGRESS
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_er_mtp_reset(int unit, int index, int flags)
{
    
    int              offset;
    int              idx;
    uint32           mtp ;

    
    /* Hw buffer preparation. */
    mtp = 0;

    /* HW write. based on mirrored traffic direction. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        BCM_IF_ERROR_RETURN(WRITE_IM_MTP_INDEXr(unit, index, mtp));
    }

    /* EM_MTP_INDEX has same layout as IM_MTP_INDEX */
    if (flags & BCM_MIRROR_PORT_EGRESS) {
        BCM_IF_ERROR_RETURN(WRITE_EM_MTP_INDEXr(unit, index, mtp));
    }

    /* Program trunk member ports. */
    offset = index * BCM_SWITCH_TRUNK_MAX_PORTCNT;
    for (idx = 0; idx < BCM_SWITCH_TRUNK_MAX_PORTCNT; idx++, offset++) {
        mtp = 0;

        /* HW write. based on mirrored traffic direction. */
        if (flags & BCM_MIRROR_PORT_INGRESS) {
            BCM_IF_ERROR_RETURN (WRITE_EGR_IM_MTP_INDEXr(unit, offset, mtp));
        }

        /* EGR_EM_MTP_INDEX has same layout as EGR_IM_MTP_INDEX */
        if (flags & BCM_MIRROR_PORT_EGRESS) {
            BCM_IF_ERROR_RETURN(WRITE_EGR_EM_MTP_INDEXr(unit, offset, mtp));
        } 
    }
    return (BCM_E_NONE);
}
#endif /* BCM_EASYRIDER_SUPPORT */

#if defined(BCM_TRX_SUPPORT)
/*
 * Function:
 *	    _bcm_trx_mirror_egr_erspan_write
 * Purpose:
 *	   Program HW buffer.  
 * Parameters:
 *	   unit     - (IN) BCM device number.
 *     index    - (IN) Mtp index.
 *     buffer   - (IN) Tunnel encapsulation buffer.
 *     flags    - (IN) Mirror direction flags.  
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_trx_mirror_egr_erspan_write(int unit, int index, uint32 *buffer, int flags)
{
    bcm_mirror_destination_t *mirror_dest;/* Destination & encapsulation. */ 
    _bcm_mtp_config_p        mtp_cfg;     /* MTP configuration.           */
    egr_erspan_entry_t       hw_buf;      /* Hw table buffer              */

    /* Get mtp configuration structure by direction & index. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        mtp_cfg = &MIRROR_CONFIG_ING_MTP(unit, index);
    } else if (flags & BCM_MIRROR_PORT_EGRESS) {
        mtp_cfg = &MIRROR_CONFIG_EGR_MTP(unit, index);
        index += 4;
    } else { /* True Egress */
        mtp_cfg = &MIRROR_CONFIG_EGR_TRUE_MTP(unit, index);
        index += 8;
    }

    mirror_dest = MIRROR_DEST(unit, mtp_cfg->dest_id);

    /* Reset hw buffer. */
    sal_memset(&hw_buf, 0, sizeof(egr_erspan_entry_t));

    /* Enable tunneling for mtp . */
    soc_mem_field32_set(unit, EGR_ERSPANm, &hw_buf, ERSPAN_ENABLEf, 1);

    /* Set untag payload flag. */
    if (mirror_dest->flags & BCM_MIRROR_DEST_PAYLOAD_UNTAGGED) {
        soc_EGR_ERSPANm_field32_set(unit, &hw_buf, UNTAG_PAYLOADf, 1);
    }

    /* Set tunnel header.. */
    if (BCM_VLAN_VALID(mirror_dest->vlan_id)) {
        soc_EGR_ERSPANm_field32_set(unit, &hw_buf, USE_TAGGED_HEADERf, 1);
        soc_EGR_ERSPANm_field_set(unit, &hw_buf, HEADER_TAGGEDf, buffer);
    } else {
        soc_EGR_ERSPANm_field_set(unit, &hw_buf, HEADER_UNTAGGEDf, buffer);
    }

    /* Write buffer to hw. */
    BCM_IF_ERROR_RETURN 
        (soc_mem_write(unit, EGR_ERSPANm, MEM_BLOCK_ALL, index, &hw_buf));

    return (BCM_E_NONE);
}

/*
 * Function:
 *	    _bcm_trx_mirror_ipv4_gre_tunnel_set
 * Purpose:
 *	   Prepare IPv4 mirror tunnel encapsulation.
 * Parameters:
 *	   unit       - (IN) BCM device number
 *     index      - (IN) Mtp index.
 *     flags      - (IN) Mirror direction flags. 
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_trx_mirror_ipv4_gre_tunnel_set(int unit, int index, int flags)
{
    bcm_mirror_destination_t *mirror_dest;/* Destination & encapsulation.   */
    uint32 buffer[_BCM_TRX_MIRROR_TUNNEL_BUFFER_SZ];/*SW tunnel encap buffer.*/
    _bcm_mtp_config_p   mtp_cfg;       /* Mtp configuration.              . */
    int                 idx;           /* Headers offset iterator.          */

    /* Get mtp configuration structure by direction & index. */
    mtp_cfg = MIRROR_CONFIG_MTP(unit, index, flags);

    mirror_dest = MIRROR_DEST(unit, mtp_cfg->dest_id);

    sal_memset(buffer, 0, _BCM_TRX_MIRROR_TUNNEL_BUFFER_SZ * sizeof(uint32));

    /*
     *   L2 Header. 
     */
    idx = BCM_VLAN_VALID(mirror_dest->vlan_id) ? 10 : 9;
      
    /* Destination mac address. */
    buffer[idx--] = (((uint32)(mirror_dest->dst_mac)[0]) << 8 | \
                     ((uint32)(mirror_dest->dst_mac)[1]));

    buffer[idx--] = (((uint32)(mirror_dest->dst_mac)[2]) << 24 | \
                     ((uint32)(mirror_dest->dst_mac)[3]) << 16 | \
                     ((uint32)(mirror_dest->dst_mac)[4]) << 8  | \
                     ((uint32)(mirror_dest->dst_mac)[5])); 

    /* Source mac address. */
    buffer[idx--] = (((uint32)(mirror_dest->src_mac)[0]) << 24 | \
                     ((uint32)(mirror_dest->src_mac)[1]) << 16 | \
                     ((uint32)(mirror_dest->src_mac)[2]) << 8  | \
                     ((uint32)(mirror_dest->src_mac)[3])); 

    buffer[idx] = (((uint32)(mirror_dest->src_mac)[4]) << 24 | \
                   ((uint32)(mirror_dest->src_mac)[5]) << 16); 

    /* Set tpid & vlan id. */
    if (BCM_VLAN_VALID(mirror_dest->vlan_id)) {
        /* Tpid. */
        buffer[idx--] |= (((uint32)(mirror_dest->tpid >> 8)) << 8 | \
                          ((uint32)(mirror_dest->tpid & 0xff)));

        /* Priority,  Cfi, Vlan id. */
        buffer[idx] = (((uint32)(mirror_dest->vlan_id >> 8)) << 24 | \
                       ((uint32)(mirror_dest->vlan_id & 0xff) << 16));
    }

    /* Set ether type to ip. 0x800  */
    buffer[idx--] |= (uint32)(0x08 << 8);

    /*
     *   IPv4 header. 
     */
    /* Version + 5 word no options length.  + Tos */
    /* Length, Id, Flags, Fragmentation offset. */
    buffer[idx--] |= ((uint32)(0x45 << 24) | \
                      (uint32)(mirror_dest->tos) << 16);

    idx--;
    /* Ttl, Protocol (GRE 0x2f)*/
    buffer[idx--] = (((uint32)mirror_dest->ttl << 24) | (0x2f << 16));

    /* Src Ip. */
    buffer[idx--] = mirror_dest->src_addr;

    /* Dst Ip. */
    buffer[idx--] = mirror_dest->dst_addr;

    /*
     *   Gre header. 
     */

    /* Protocol. 0x88be */
    buffer[idx] = 0x88be;

    /* swap byte in tunnel buffer. */
    /*  _shr_bit_rev8(buffer[idx]); */

    BCM_IF_ERROR_RETURN
        (_bcm_trx_mirror_egr_erspan_write(unit, index, (uint32 *)buffer, flags));

    return (BCM_E_NONE);
}

/*
 * Function:
 *	    _bcm_trx_mirror_rspan_write
 * Purpose:
 *	   Prepare & write L2 mirror tunnel encapsulation.
 * Parameters:
 *	   unit       - (IN) BCM device number
 *     index      - (IN) Mtp index.
 *     port- (IN) 
 *     flags      - (IN) Mirror direction flags.
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_trx_mirror_rspan_write(int unit, int index, bcm_port_t port, int flags)
{
    bcm_mirror_destination_t *mirror_dest;/* Destination & encapsulation. */
    _bcm_mtp_config_p   mtp_cfg;          /* Mtp configuration.           */
    uint32              hw_buffer;        /* HW buffer.                   */


    /* Get mtp configuration structure by direction & index. */
    mtp_cfg = MIRROR_CONFIG_MTP(unit, index, flags);

    mirror_dest = MIRROR_DEST(unit, mtp_cfg->dest_id);

    /* Outer vlan tag. */
    hw_buffer = (((uint32)mirror_dest->tpid << 16) | 
                  (uint32)mirror_dest->vlan_id);

    BCM_IF_ERROR_RETURN (soc_reg_field32_modify(unit, EGR_RSPAN_VLAN_TAGr, 
                                                port, TAGf, hw_buffer));
    return (BCM_E_NONE);
}

/*
 * Function:
 *	    _bcm_trx_mirror_l2_tunnel_set
 * Purpose:
 *	   Programm mirror L2 tunnel 
 * Parameters:
 *	   unit       - (IN) BCM device number
 *     index      - (IN) Mtp index.
 *     trunk_arr  - (IN) Mirror destinations array.
 *     flags      - (IN) Mirror direction flags.
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_trx_mirror_l2_tunnel_set(int unit, int index, 
                              bcm_gport_t *trunk_arr, int flags)
{
    bcm_module_t  my_modid;    /* Local modid.                   */
    int           idx;         /* Trunk members iteration index. */
    bcm_module_t  mod_out;     /* Hw mapped modid.               */
    bcm_port_t    port_out;    /* Hw mapped port number.         */
    bcm_module_t  modid;       /* Application space modid.       */
    bcm_port_t    port;        /* Application space port number. */

    /* Input parameters check. */ 
    if (NULL == trunk_arr) {
        return (BCM_E_PARAM);
    }

    /* Get local base module id. */
    BCM_IF_ERROR_RETURN (bcm_esw_stk_my_modid_get(unit, &my_modid));

    for (idx = 0; idx < BCM_SWITCH_TRUNK_MAX_PORTCNT; idx++) {
        modid = BCM_GPORT_MODPORT_MODID_GET(trunk_arr[idx]);
        port = BCM_GPORT_MODPORT_PORT_GET(trunk_arr[idx]);
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, modid, port, 
                                    &mod_out, &port_out));
        if (mod_out != my_modid) {
            continue;
        }

        if (0 == IS_E_PORT(unit, port_out)) {
            continue;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_trx_mirror_rspan_write(unit, index, port_out, flags));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *	    _bcm_trx_mirror_tunnel_set
 * Purpose:
 *	   Initialize mirror tunnel 
 * Parameters:
 *	   unit       - (IN) BCM device number
 *     index      - (IN) Mtp index.
 *     trunk_arr  - (IN) 
 *     flags      - (IN) Mirror direction flags.
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_trx_mirror_tunnel_set(int unit, int index,
                           bcm_gport_t *trunk_arr, int flags)
{
    bcm_mirror_destination_t *mirror_dest; /* Destination & Encapsulation.*/
    _bcm_mtp_config_p   mtp_cfg;           /* MTP configuration .         */
    int rv = BCM_E_NONE;                   /* Operation return status.    */

    /* Get mtp configuration structure by direction & index. */
    mtp_cfg = MIRROR_CONFIG_MTP(unit, index, flags);

    mirror_dest = MIRROR_DEST(unit, mtp_cfg->dest_id);
    if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) {
        if (4 == mirror_dest->version) {
            rv = _bcm_trx_mirror_ipv4_gre_tunnel_set(unit, index, flags);
        } else {
            rv = (BCM_E_UNAVAIL);
        }
    }

    if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_L2) {
        rv = _bcm_trx_mirror_l2_tunnel_set(unit, index, trunk_arr, flags);
    }

    return (rv);
}
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_FIREBOLT_SUPPORT) 
/*
 * Function:
 *	    _bcm_fbx_mtp_init
 * Purpose:
 *	   Initialize mirror target port for FBX devices. 
 * Parameters:
 *	   unit       - (IN)BCM device number
 *     index      - (IN)Mtp index.
 *     trunk_arr  - (IN)Trunk members array. 
 *     flags      - (IN)Filled entry flags(BCM_MIRROR_PORT_INGRESS/EGRESS
 *                    or both. In case both flags are specied
 *                    ingress & egress configuration is assumed to be
 *                    idential.
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_fbx_mtp_init(int unit, int index, bcm_gport_t *trunk_arr, int flags)
{
    bcm_gport_t         mirror_dest;
    bcm_port_t          port_out;
    bcm_module_t        mod_out;
    _bcm_mtp_config_p   mtp_cfg;
    int                 offset;
    int                 idx;
    bcm_trunk_t         trunk = BCM_TRUNK_INVALID;
    bcm_module_t        modid = 0;
    bcm_port_t          port = -1;
    uint32              mtp = 0;

    /* Input parameters check */ 
    if (NULL == trunk_arr) {
        return (BCM_E_PARAM);
    }

    /* Get mtp configuration structure by direction & index. */
    mtp_cfg = MIRROR_CONFIG_MTP(unit, index, flags);

    /* Parse destination trunk / port & module. */
    mirror_dest = MIRROR_DEST_GPORT(unit, mtp_cfg->dest_id);
    if (BCM_GPORT_IS_TRUNK(mirror_dest)) {
        trunk = BCM_GPORT_TRUNK_GET(mirror_dest);
    } else {
        modid = BCM_GPORT_MODPORT_MODID_GET(mirror_dest);
        port  = BCM_GPORT_MODPORT_PORT_GET(mirror_dest);
    }

    /* Hw buffer preparation. */
    if (soc_feature(unit, soc_feature_trunk_group_overlay)) {
        if (BCM_GPORT_IS_TRUNK(mirror_dest)) {
            soc_IM_MTP_INDEXm_field32_set(unit, &mtp, Tf, 1);
            soc_IM_MTP_INDEXm_field32_set(unit, &mtp, TGIDf, trunk);
        } else {
            soc_IM_MTP_INDEXm_field32_set(unit, &mtp, MODULE_IDf, modid);
            soc_IM_MTP_INDEXm_field32_set(unit, &mtp, PORT_NUMf, port);
        }
    } else {
        if (BCM_GPORT_IS_TRUNK(mirror_dest)) {
            modid = BCM_TRUNK_TO_MODIDf(unit, trunk);
            port  = BCM_TRUNK_TO_TGIDf(unit, trunk);
        }
        soc_IM_MTP_INDEXm_field32_set(unit, &mtp, MODULE_IDf, modid);
        soc_IM_MTP_INDEXm_field32_set(unit, &mtp, PORT_TGIDf, port);
    }

    /* HW write. based on mirrored traffic direction. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        BCM_IF_ERROR_RETURN 
            (soc_mem_write(unit, IM_MTP_INDEXm, MEM_BLOCK_ALL, index, &mtp));
    }

    /* EM_MTP_INDEX has same layout as IM_MTP_INDEX */
    if (flags & BCM_MIRROR_PORT_EGRESS) {
        BCM_IF_ERROR_RETURN 
            (soc_mem_write(unit, EM_MTP_INDEXm, MEM_BLOCK_ALL, index, &mtp));
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    /* EP_REDIRECT_EM_MTP_INDEX has same layout as IM_MTP_INDEX */
    if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
        BCM_IF_ERROR_RETURN 
            (soc_mem_write(unit, EP_REDIRECT_EM_MTP_INDEXm,
                           MEM_BLOCK_ALL, index, &mtp));
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    offset = index * BCM_SWITCH_TRUNK_MAX_PORTCNT;
    for (idx = 0; idx < BCM_SWITCH_TRUNK_MAX_PORTCNT; idx++, offset++) {
        mtp = 0;
        modid = BCM_GPORT_MODPORT_MODID_GET(trunk_arr[idx]);
        port = BCM_GPORT_MODPORT_PORT_GET(trunk_arr[idx]);
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, modid, port, 
                                    &mod_out, &port_out));
         
        soc_EGR_IM_MTP_INDEXm_field32_set(unit, &mtp, MTP_DST_PORTf, port_out);
        soc_EGR_IM_MTP_INDEXm_field32_set(unit, &mtp, MTP_DST_MODIDf, mod_out);

        /* HW write. based on mirrored traffic direction. */
        if (flags & BCM_MIRROR_PORT_INGRESS) {
            BCM_IF_ERROR_RETURN 
                (soc_mem_write(unit, EGR_IM_MTP_INDEXm, MEM_BLOCK_ALL, offset, &mtp));
        }

        /* EGR_EM_MTP_INDEX has same layout as EGR_IM_MTP_INDEX */
        if (flags & BCM_MIRROR_PORT_EGRESS) {
            BCM_IF_ERROR_RETURN
                (soc_mem_write(unit, EGR_EM_MTP_INDEXm, MEM_BLOCK_ALL, offset, &mtp));
        }

#ifdef BCM_TRIUMPH2_SUPPORT
        /* EGR_EP_REDIRECT_EM_MTP_INDEX has same layout as others */
        if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
            BCM_IF_ERROR_RETURN
                (soc_mem_write(unit, EGR_EP_REDIRECT_EM_MTP_INDEXm,
                               MEM_BLOCK_ALL, offset, &mtp));
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
    }

#if defined(BCM_TRX_SUPPORT)
    if((MIRROR_DEST(unit, mtp_cfg->dest_id))->flags) {
        BCM_IF_ERROR_RETURN(_bcm_trx_mirror_tunnel_set(unit, index, 
                                                       trunk_arr, flags));
    }
#endif /* BCM_TRX_SUPPORT */

    return (BCM_E_NONE);
}

/*
 * Function:
 *	    _bcm_fbx_mtp_reset
 * Purpose:
 *	   Reset mirror target port for FBX devices. 
 * Parameters:
 *	   unit       - (IN)BCM device number
 *     index      - (IN)Mtp index.
 *     flags      - (IN)Filled entry flags(BCM_MIRROR_PORT_INGRESS/EGRESS
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_fbx_mtp_reset(int unit, int index, int flags)
{
    int             offset;
    int             idx;
    uint32          mtp = 0;

    /* HW write. based on mirrored traffic direction. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        BCM_IF_ERROR_RETURN 
            (soc_mem_write(unit, IM_MTP_INDEXm, MEM_BLOCK_ALL, index, &mtp));
    }

    /* EM_MTP_INDEX has same layout as IM_MTP_INDEX */
    if (flags & BCM_MIRROR_PORT_EGRESS) {
        BCM_IF_ERROR_RETURN 
            (soc_mem_write(unit, EM_MTP_INDEXm, MEM_BLOCK_ALL, index, &mtp));
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    /* EP_REDIRECT_EM_MTP_INDEX has same layout as IM_MTP_INDEX */
    if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
        BCM_IF_ERROR_RETURN 
            (soc_mem_write(unit, EP_REDIRECT_EM_MTP_INDEXm,
                           MEM_BLOCK_ALL, index, &mtp));
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

#ifdef BCM_TRX_SUPPORT
        if (SOC_MEM_IS_VALID(unit, EGR_ERSPANm)) {
            egr_erspan_entry_t      hw_buf;
            int                     egr_idx;

            /* Reset hw buffer. */
            sal_memset(&hw_buf, 0, sizeof(egr_erspan_entry_t));
            
            /* Get egr_erspan index by direction */
            if (flags & BCM_MIRROR_PORT_INGRESS) {
                egr_idx = index;
            } else if (flags & BCM_MIRROR_PORT_EGRESS) {
                egr_idx = index + 4;
            } else { /* True Egress */
                egr_idx = index + 8;
            }
            BCM_IF_ERROR_RETURN(
               soc_mem_write(unit, EGR_ERSPANm, MEM_BLOCK_ALL, egr_idx, &hw_buf));
        }
#endif /* BCM_TRX_SUPPORT */

    offset = index * BCM_SWITCH_TRUNK_MAX_PORTCNT;
    for (idx = 0; idx < BCM_SWITCH_TRUNK_MAX_PORTCNT; idx++, offset++) {
        mtp = 0;

        /* HW write. based on mirrored traffic direction. */
        if (flags & BCM_MIRROR_PORT_INGRESS) {
            BCM_IF_ERROR_RETURN 
                (soc_mem_write(unit, EGR_IM_MTP_INDEXm, MEM_BLOCK_ALL, offset, &mtp));
        }

        /* EGR_EM_MTP_INDEX has same layout as EGR_IM_MTP_INDEX */
        if (flags & BCM_MIRROR_PORT_EGRESS) {
            BCM_IF_ERROR_RETURN
                (soc_mem_write(unit, EGR_EM_MTP_INDEXm, MEM_BLOCK_ALL, offset, &mtp));
        }

#ifdef BCM_TRIUMPH2_SUPPORT
        /* EGR_EP_REDIRECT_EM_MTP_INDEX has same layout as others */
        if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
            BCM_IF_ERROR_RETURN
                (soc_mem_write(unit, EGR_EP_REDIRECT_EM_MTP_INDEXm,
                               MEM_BLOCK_ALL, offset, &mtp));
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
    }
    return (BCM_E_NONE);
}
#endif /* BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *	    _bcm_xgs3_mtp_reset
 * Purpose:
 *	   Reset mirror target port for XGS3 devices. 
 * Parameters:
 *	   unit     - (IN)BCM device number
 *     index    - (IN)Mtp index.
 *     flags    - (IN)Filled entry flags(BCM_MIRROR_PORT_INGRESS/EGRESS
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mtp_reset(int unit, int index, int flags)
{
    int rv = BCM_E_UNAVAIL;      /* Operation return status. */
#if defined(BCM_FIREBOLT_SUPPORT) 
    if (SOC_IS_FBX(unit)) {
       rv = _bcm_fbx_mtp_reset(unit, index, flags);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_EASYRIDER_SUPPORT) 
    if (SOC_IS_EASYRIDER(unit)) {
       rv = _bcm_er_mtp_reset(unit, index, flags);
    }
#endif /* BCM_EASYRIDER_SUPPORT */
    return rv;
}


/*
 * Function:
 *	    _bcm_xgs3_mtp_init
 * Purpose:
 *	   Initialize mirror target port for XGS3 devices. 
 * Parameters:
 *	   unit     - (IN)BCM device number
 *     index    - (IN)Mtp index.
 *     flags    - (IN)Filled entry flags(BCM_MIRROR_PORT_INGRESS/EGRESS
 *                    or both. In case both flags are specied
 *                    ingress & egress configuration is assumed to be
 *                    idential.
 * Returns:
 *	   BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mtp_init(int unit, int index, int flags)
{
    _bcm_mtp_config_p    mtp_cfg;
    bcm_gport_t          gport[BCM_SWITCH_TRUNK_MAX_PORTCNT];
    bcm_gport_t          mirror_dest;
    bcm_trunk_add_info_t tinfo;
    int                  idx, isGport = 0;
    int                  rv = BCM_E_UNAVAIL;
    
    mtp_cfg = MIRROR_CONFIG_MTP(unit, index, flags);

    /* Destination port/trunk id validation. */
    mirror_dest = MIRROR_DEST_GPORT(unit, mtp_cfg->dest_id);
    if (BCM_GPORT_IS_TRUNK(mirror_dest)) {
        rv = _bcm_trunk_id_validate(unit, BCM_GPORT_TRUNK_GET(mirror_dest));
        if (BCM_FAILURE(rv)) {
            return (BCM_E_PORT);
        }
        rv = bcm_esw_trunk_get(unit, BCM_GPORT_TRUNK_GET(mirror_dest), &tinfo);
        if (BCM_FAILURE(rv) || (tinfo.num_ports <= 0)) {
            return (BCM_E_PORT);
        }

        /* Fill gport array with trunk member ports. */
        for (idx = 0; idx < BCM_SWITCH_TRUNK_MAX_PORTCNT; idx++) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
            if (isGport) {
                gport[idx] = tinfo.tp[idx % tinfo.num_ports] ;
            } else {
                BCM_GPORT_MODPORT_SET(gport[idx], tinfo.tm[idx % tinfo.num_ports],
                                      tinfo.tp[idx % tinfo.num_ports]);
            }
        }
    } else {
        if (!SOC_MODID_ADDRESSABLE(unit, BCM_GPORT_MODPORT_MODID_GET(mirror_dest))) {
            return (BCM_E_BADID);
        }
        if (!SOC_PORT_ADDRESSABLE(unit, BCM_GPORT_MODPORT_PORT_GET(mirror_dest))) {
            return (BCM_E_PORT);
        }
        /* Fill gport array with destination port only. */
        for (idx = 0; idx < BCM_SWITCH_TRUNK_MAX_PORTCNT; idx++) {
            gport[idx] = mirror_dest;
        }
    }
#if defined(BCM_FIREBOLT_SUPPORT) 
    if (SOC_IS_FBX(unit)) {
       rv = _bcm_fbx_mtp_init(unit, index, gport, flags);
    }
#endif /* BCM_FIREBOLT_SUPPORT */
#if defined(BCM_EASYRIDER_SUPPORT) 
    if (SOC_IS_EASYRIDER(unit)) {
       rv = _bcm_er_mtp_init(unit, index, gport, flags);
    }
#endif /* BCM_EASYRIDER_SUPPORT */
    return rv;
}

/*
 * Function:
 *      _bcm_xgs3_mirror_trunk_update
 * Description:
 *      Update mtp programming based on trunk port membership.
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      tid        - (IN)  Trunk id. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_xgs3_mirror_trunk_update(int unit, bcm_trunk_t tid)
{
    int idx;                        /* Mtp iteration index.     */
    bcm_gport_t  gport;             /* Mirror destination.      */
    bcm_gport_t  mirror_dest_id;    /* Mirror destination.      */
    int rv = BCM_E_NONE;            /* Operation return status. */

    /* Check if mirroring enabled on the device. */
    if (!MIRROR_INIT(unit)) {
        return (rv);
    }

    /* Initilize mirror destination. */
    BCM_GPORT_TRUNK_SET(gport, tid);

    MIRROR_LOCK(unit);
    /* Ingress mirroring destions update */

    for (idx = 0; idx < MIRROR_CONFIG(unit)->ing_mtp_count; idx++) {
        if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, idx)) { 
            mirror_dest_id = MIRROR_CONFIG_ING_MTP_DEST(unit, idx);
            if (MIRROR_DEST_GPORT(unit, mirror_dest_id) == gport) {
                rv = _bcm_xgs3_mtp_init(unit, idx, BCM_MIRROR_PORT_INGRESS);
                if (BCM_FAILURE(rv)) {
                    MIRROR_UNLOCK(unit);
                    return (rv);
                }
            }
        }
    }

    /* Egress mirroring destinations update */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->egr_mtp_count; idx++) {
        if (MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, idx)) { 
            mirror_dest_id = MIRROR_CONFIG_EGR_MTP_DEST(unit, idx);
            if (MIRROR_DEST_GPORT(unit, mirror_dest_id) == gport) {
                rv = _bcm_xgs3_mtp_init(unit, idx, BCM_MIRROR_PORT_EGRESS);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }
        }
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    /* True egress mirroring destinations update */
    if (soc_feature(unit, soc_feature_egr_mirror_true)) {
        for (idx = 0; idx < MIRROR_CONFIG(unit)->egr_true_mtp_count; idx++) {
            if (MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, idx)) { 
                mirror_dest_id = MIRROR_CONFIG_EGR_TRUE_MTP_DEST(unit, idx);
                if (MIRROR_DEST_GPORT(unit, mirror_dest_id) == gport) {
                    rv = _bcm_xgs3_mtp_init(unit, idx,
                                            BCM_MIRROR_PORT_EGRESS_TRUE);
                    if (BCM_FAILURE(rv)) {
                        break;
                    }
                }
            }
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    MIRROR_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      _bcm_xgs3_mirror_ingress_mtp_reserve
 * Description:
 *      Reserve a mirror-to port
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      dest_id    - (IN)  Mirror destination id.
 *      index_used - (OUT) MTP index reserved
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the a mirror-to port is reserved more than once
 *      (without being unreserved) then the same MTP index 
 *      will be returned for each call.
 */
STATIC int
_bcm_xgs3_mirror_ingress_mtp_reserve(int unit, bcm_gport_t dest_id, 
                                     int *index_used)
{
    int rv;                                  /* Operation return status. */
    int idx = _BCM_MIRROR_INVALID_MTP;       /* Mtp iteration index.     */

    /* Input parameters check. */
    if (NULL == index_used) {
        return (BCM_E_PARAM);
    }

    /* Look for existing MTP in use */
    rv = _bcm_esw_mirror_ingress_mtp_match(unit, dest_id, &idx);
    if (BCM_SUCCESS(rv)) {
        MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, idx)++;
        *index_used = idx;
        return (rv);
    }

    /* Reserve free index */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->ing_mtp_count; idx++) {
        if (0 == MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, idx)) {
            break;
        }
    }

    if (idx < MIRROR_CONFIG(unit)->ing_mtp_count) {
        /* Mark mtp as used. */
        MIRROR_CONFIG_ING_MTP_DEST(unit, idx) = dest_id;
        MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, idx)++;
        MIRROR_DEST_REF_COUNT(unit, dest_id)++;

        /* Write MTP registers */
        rv = _bcm_xgs3_mtp_init(unit, idx, BCM_MIRROR_PORT_INGRESS);
        if (BCM_FAILURE(rv)) {
            MIRROR_CONFIG_ING_MTP_DEST(unit, idx) = BCM_GPORT_INVALID;
            MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, idx) = 0;
            if (MIRROR_DEST_REF_COUNT(unit, dest_id) > 0) {
                MIRROR_DEST_REF_COUNT(unit, dest_id)--;
            }
        }
        *index_used = idx;
    } else {
        rv = BCM_E_RESOURCE;
    } 

    return (rv);
}

/*
 * Function:
 *      _bcm_xgs3_mirror_egress_mtp_reserve
 * Description:
 *      Reserve a mirror-to port
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      dest_id    - (IN)  Mirror destination id.
 *      index_used - (OUT) MTP index reserved
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the a mirror-to port is reserved more than once
 *      (without being unreserved) then the same MTP index 
 *      will be returned for each call.
 */
STATIC int
_bcm_xgs3_mirror_egress_mtp_reserve(int unit, bcm_gport_t dest_id,
                                     int *index_used)
{
    int rv;                                  /* Operation return status.*/
    int idx = _BCM_MIRROR_INVALID_MTP;       /* Mtp iteration index.    */

    /* Input parameters check. */
    if (NULL == index_used) {
        return (BCM_E_PARAM);
    }

    /* Look for existing MTP in use */
    rv = _bcm_esw_mirror_egress_mtp_match(unit, dest_id, &idx);
    if (BCM_SUCCESS(rv)) {
        MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, idx)++;
        *index_used = idx;
        return (rv);
    }

    /* Reserve free index */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->egr_mtp_count; idx++) {
        if (0 == MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, idx)) {
            break;
        }
    }

    if (idx < MIRROR_CONFIG(unit)->egr_mtp_count) {
        /* Mark mtp as used. */
        MIRROR_CONFIG_EGR_MTP_DEST(unit, idx) = dest_id;
        MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, idx)++;
        MIRROR_DEST_REF_COUNT(unit, dest_id)++;

        /* Write MTP registers */
        rv = _bcm_xgs3_mtp_init(unit, idx, BCM_MIRROR_PORT_EGRESS);
        if (BCM_FAILURE(rv)) {
            MIRROR_CONFIG_EGR_MTP_DEST(unit, idx) = BCM_GPORT_INVALID;
            MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, idx) = 0;
            if (MIRROR_DEST_REF_COUNT(unit, dest_id) > 0) {
                MIRROR_DEST_REF_COUNT(unit, dest_id)--;
            }
        }
        *index_used = idx;
    } else {
        rv = BCM_E_RESOURCE;
    } 

    return (rv);
}

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *      _bcm_xgs3_mirror_egress_true_mtp_reserve
 * Description:
 *      Reserve a mirror-to port
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      dest_id    - (IN)  Mirror destination id.
 *      index_used - (OUT) MTP index reserved
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the a mirror-to port is reserved more than once
 *      (without being unreserved) then the same MTP index 
 *      will be returned for each call.
 */
STATIC int
_bcm_xgs3_mirror_egress_true_mtp_reserve(int unit, bcm_gport_t dest_id,
                                     int *index_used)
{
    int rv;                                  /* Operation return status.*/
    int idx = _BCM_MIRROR_INVALID_MTP;       /* Mtp iteration index.    */

    /* Input parameters check. */
    if (NULL == index_used) {
        return (BCM_E_PARAM);
    }

    /* Look for existing MTP in use */
    rv = _bcm_esw_mirror_egress_true_mtp_match(unit, dest_id, &idx);
    if (BCM_SUCCESS(rv)) {
        MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, idx)++;
        *index_used = idx;
        return (rv);
    }

    /* Reserve free index */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->egr_true_mtp_count; idx++) {
        if (0 == MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, idx)) {
            break;
        }
    }

    if (idx < MIRROR_CONFIG(unit)->egr_true_mtp_count) {
        /* Mark mtp as used. */
        MIRROR_CONFIG_EGR_TRUE_MTP_DEST(unit, idx) = dest_id;
        MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, idx)++;
        MIRROR_DEST_REF_COUNT(unit, dest_id)++;

        /* Write MTP registers */
        rv = _bcm_xgs3_mtp_init(unit, idx, BCM_MIRROR_PORT_EGRESS_TRUE);
        if (BCM_FAILURE(rv)) {
            MIRROR_CONFIG_EGR_TRUE_MTP_DEST(unit, idx) = BCM_GPORT_INVALID;
            MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, idx) = 0;
            if (MIRROR_DEST_REF_COUNT(unit, dest_id) > 0) {
                MIRROR_DEST_REF_COUNT(unit, dest_id)--;
            }
        }
        *index_used = idx;
    } else {
        rv = BCM_E_RESOURCE;
    } 

    return (rv);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      _bcm_xgs3_mirror_ingress_mtp_unreserve
 * Description:
 *      Free ingress  mirror-to port
 * Parameters:
 *      unit       - (IN) BCM device number
 *      mtp_index  - (IN) MTP index. 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Mtp index completely freed only when reference count gets to 0. 
 */
STATIC int
_bcm_xgs3_mirror_ingress_mtp_unreserve(int unit, int mtp_index)
{
    int rv = BCM_E_NONE;                     /* Operation return status.*/
    bcm_gport_t mirror_dest;                 /* Mirror destination id.  */

    /* Input parameters check. */
    if (mtp_index >= MIRROR_CONFIG(unit)->ing_mtp_count) {
        return (BCM_E_PARAM);
    }

    if (0 == MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, mtp_index)) {
        return (rv);
    }


    /* Decrement mtp index reference count. */
    if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, mtp_index) > 0) {
        MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, mtp_index)--;
    }

    if (0 == MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, mtp_index)) {
        /* Write MTP registers */
        mirror_dest = MIRROR_CONFIG_ING_MTP_DEST(unit, mtp_index);
        rv = _bcm_xgs3_mtp_reset(unit, mtp_index, BCM_MIRROR_PORT_INGRESS);
        MIRROR_CONFIG_ING_MTP_DEST(unit, mtp_index) = BCM_GPORT_INVALID;
        if (MIRROR_DEST_REF_COUNT(unit, mirror_dest) > 0) {
            MIRROR_DEST_REF_COUNT(unit, mirror_dest)--;
        }
    }
    return (rv);
}

/*
 * Function:
 *      _bcm_xgs3_mirror_egress_mtp_unreserve
 * Description:
 *      Free egress  mirror-to port
 * Parameters:
 *      unit       - (IN) BCM device number
 *      mtp_index  - (IN) MTP index. 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Mtp index completely freed only when reference count gets to 0. 
 */
STATIC int
_bcm_xgs3_mirror_egress_mtp_unreserve(int unit, int mtp_index)
{
    int rv = BCM_E_NONE;                     /* Operation return status.*/
    bcm_gport_t mirror_dest;                 /* Mirror destination id.  */

    /* Input parameters check. */
    if (mtp_index >= MIRROR_CONFIG(unit)->egr_mtp_count) {
        return (BCM_E_PARAM);
    }

    if (0 == MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, mtp_index)) {
        return (rv);
    }

    /* Decrement mtp index reference count. */
    if (MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, mtp_index) > 0) {
        MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, mtp_index)--;
    }


    if (0 == MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, mtp_index)) {
        /* Write MTP registers */
        mirror_dest = MIRROR_CONFIG_EGR_MTP_DEST(unit, mtp_index);
        rv = _bcm_xgs3_mtp_reset(unit, mtp_index, BCM_MIRROR_PORT_EGRESS);
        MIRROR_CONFIG_EGR_MTP_DEST(unit, mtp_index) = BCM_GPORT_INVALID;
        if (MIRROR_DEST_REF_COUNT(unit, mirror_dest) > 0) { 
            MIRROR_DEST_REF_COUNT(unit, mirror_dest)--;
        }
    }
    return (rv);
}

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *      _bcm_xgs3_mirror_egress_true_mtp_unreserve
 * Description:
 *      Free egress  mirror-to port
 * Parameters:
 *      unit       - (IN) BCM device number
 *      mtp_index  - (IN) MTP index. 
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Mtp index completely freed only when reference count gets to 0. 
 */
STATIC int
_bcm_xgs3_mirror_egress_true_mtp_unreserve(int unit, int mtp_index)
{
    int rv = BCM_E_NONE;                     /* Operation return status.*/
    bcm_gport_t mirror_dest;                 /* Mirror destination id.  */

    /* Input parameters check. */
    if (mtp_index >= MIRROR_CONFIG(unit)->egr_true_mtp_count) {
        return (BCM_E_PARAM);
    }

    if (0 == MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, mtp_index)) {
        return (rv);
    }

    /* Decrement mtp index reference count. */
    if (MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, mtp_index) > 0) {
        MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, mtp_index)--;
    }


    if (0 == MIRROR_CONFIG_EGR_TRUE_MTP_REF_COUNT(unit, mtp_index)) {
        /* Write MTP registers */
        mirror_dest = MIRROR_CONFIG_EGR_TRUE_MTP_DEST(unit, mtp_index);
        rv = _bcm_xgs3_mtp_reset(unit, mtp_index, BCM_MIRROR_PORT_EGRESS_TRUE);
        MIRROR_CONFIG_EGR_TRUE_MTP_DEST(unit, mtp_index) = BCM_GPORT_INVALID;
        if (MIRROR_DEST_REF_COUNT(unit, mirror_dest) > 0) { 
            MIRROR_DEST_REF_COUNT(unit, mirror_dest)--;
        }
    }
    return (rv);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      _bcm_xgs3_mirror_mtp_reserve 
 * Description:
 *      Reserve a mirror-to port
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      dest_port  - (IN)  Mirror destination gport.
 *      flags      - (IN)  Mirrored traffic direction. 
 *      index_used - (OUT) MTP index reserved
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the a mirror-to port is reserved more than once
 *      (without being unreserved) then the same MTP index 
 *      will be returned for each call.
 *      Direction should be either INGRESS, EGRESS, or EGRESS_TRUE.
 */
STATIC int
_bcm_xgs3_mirror_mtp_reserve(int unit, bcm_gport_t gport, 
                            uint32 flags, int *index_used)
{
    /* Input parameters check. */
    if (NULL == index_used) {
        return (BCM_E_PARAM);
    }

    /* Allocate & initialize mtp based on mirroring direction. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        return _bcm_xgs3_mirror_ingress_mtp_reserve(unit, gport, index_used);
    } else if (flags & BCM_MIRROR_PORT_EGRESS) {
        return _bcm_xgs3_mirror_egress_mtp_reserve(unit, gport, index_used);
#ifdef BCM_TRIUMPH2_SUPPORT
    } else if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
        return _bcm_xgs3_mirror_egress_true_mtp_reserve(unit, gport,
                                                        index_used);
#endif /* BCM_TRIUMPH2_SUPPORT */
    } 
    return (BCM_E_PARAM);
}


/*
 * Function:
 *      _bcm_xgs3_mirror_mtp_unreserve 
 * Description:
 *      Free a mirror-to port
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      mtp_index  - (IN)  MTP index. 
 *      flags      - (IN)  Mirrored traffic direction. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mirror_mtp_unreserve(int unit, int mtp_index, uint32 flags)
{

    /* Free & reset mtp based on mirroring direction. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        return _bcm_xgs3_mirror_ingress_mtp_unreserve(unit, mtp_index);
    } else if (flags & BCM_MIRROR_PORT_EGRESS) {
        return _bcm_xgs3_mirror_egress_mtp_unreserve(unit, mtp_index);
#ifdef BCM_TRIUMPH2_SUPPORT
    } else if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
        return _bcm_xgs3_mirror_egress_true_mtp_unreserve(unit, mtp_index);
#endif /* BCM_TRIUMPH2_SUPPORT */
    } 
    return (BCM_E_PARAM);
}

#ifdef BCM_TRIUMPH2_SUPPORT
static soc_field_t _tr2_mtp_index_f[] = { MTP_INDEX0f, MTP_INDEX1f,
                                          MTP_INDEX2f, MTP_INDEX3f };

/*
 * Function:
 *      _bcm_tr2_mirror_ipipe_mtp_install
 * Description:
 *      Install IPIPE ingress/egress reserved mtp index into 
 *      mirror control register. 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      mtp_index  - (IN)  Mirror to port index.
 *      egress     - (IN)  (TRUE/FALSE) Egress mirror.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mirror_ipipe_mtp_install(int unit, bcm_port_t port,
                                  int mtp_index, int egress)
{
    uint64 reg_val;            /* MTP control register value. */
    uint32 ms_reg;             /* MTP mode register value     */
    int enable = 0;            /* Used mtp bit map.           */
    int hw_mtp;                /* Hw installed mtp index.     */
    int mtp_slot, mtp_bit, free_ptr = -1, free_slot = -1;

    /* Read mtp ingress mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN(_bcm_port_mirror_enable_get(unit, port, &enable));

    /* Read mirror control register to compare programmed mtp indexes. */
    BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &reg_val));

    for (mtp_slot = 0, mtp_bit = BCM_MIRROR_MTP_ONE;
         (mtp_bit & MIRROR_CONFIG_MTP_DEV_MASK(unit)) != 0;
         mtp_bit <<= 1, mtp_slot++) {
        if (egress) {
            if (!(MIRROR_CONFIG_MTP_MODE_BMP(unit) & mtp_bit)) {
                if (MIRROR_CONFIG_MTP_MODE_REF_COUNT(unit, mtp_slot) == 0) {
                    /* MTP Container is undecided, note for later */
                    if (free_ptr < 0) {
                        /* Record unallocated MTP container */
                        free_ptr = mtp_slot;
                    }
                } /* Else, container already used for ingress mirroring */
                continue;
            }
        } else {
            if (MIRROR_CONFIG_MTP_MODE_BMP(unit) & mtp_bit) {
                /* Slot configured for egress mirroring, skip */
                continue;
            }
        }
        if (!(enable & mtp_bit)) { /* Slot unused on this port */
            if (free_slot < 0) {
                /* Record free slot */
                free_slot = mtp_slot;
            }
        } else {
            /* Check if mtp is already installed. */
            hw_mtp = soc_reg64_field32_get(unit, MIRROR_CONTROLr, reg_val,
                                           _tr2_mtp_index_f[mtp_slot]);
            if (mtp_index == hw_mtp) {
                /* Nothing to do */
                return BCM_E_NONE;
            }
        }
    }

    /* Use previous allocated slot if available. Otherwise use unallocated
     * MTP continaner.  If neither, we're out of resources. */
    if (free_slot < 0) {
        if (free_ptr < 0) {
            return BCM_E_FULL;
        } else {
            free_slot = free_ptr;
        }
    }

    mtp_slot = free_slot;
    mtp_bit = 1 << free_slot;

    soc_reg64_field32_set(unit, MIRROR_CONTROLr, &reg_val,
                          _tr2_mtp_index_f[mtp_slot], mtp_index);

    /* Set mtp index in Mirror control. */
    BCM_IF_ERROR_RETURN(WRITE_MIRROR_CONTROLr(unit, port, reg_val));

    if (IS_HG_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, 
                                    port, _tr2_mtp_index_f[mtp_slot],
                                    mtp_index));
    }

    /* Enable ingress mirroring on the port. */
    enable |= mtp_bit;
    BCM_IF_ERROR_RETURN(_bcm_port_mirror_enable_set(unit, port, enable));

    if (egress && !(MIRROR_CONFIG_MTP_MODE_BMP(unit) & mtp_bit)) {
        /* Update MTP_MODE */
        MIRROR_CONFIG_MTP_MODE_BMP(unit) |= mtp_bit;

        BCM_IF_ERROR_RETURN(READ_MIRROR_SELECTr(unit, &ms_reg));
        soc_reg_field_set(unit, MIRROR_SELECTr, &ms_reg, MTP_TYPEf,
                          MIRROR_CONFIG_MTP_MODE_BMP(unit));
        BCM_IF_ERROR_RETURN(WRITE_MIRROR_SELECTr(unit, ms_reg));
        BCM_IF_ERROR_RETURN(WRITE_EGR_MIRROR_SELECTr(unit, ms_reg));
    }
    MIRROR_CONFIG_MTP_MODE_REF_COUNT(unit, mtp_slot)++;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_mirror_egress_true_mtp_install
 * Description:
 *      Install egress true mirroring reserved mtp index into 
 *      mirror control register. 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      mtp_index  - (IN)  Mirror to port index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mirror_egress_true_mtp_install(int unit, bcm_port_t port,
                                        int mtp_index)
{
    int enable;                /* Used mtp bit map.           */

    /* Read mtp egress true mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN
        (_bcm_port_mirror_egress_true_enable_get(unit, port, &enable));

    if (!(enable & (1 << mtp_index))) {
        enable |= (1 << mtp_index);
        BCM_IF_ERROR_RETURN
            (_bcm_port_mirror_egress_true_enable_set(unit, port, enable));
    } else {
        /* GNATS: Nothing to do?  Ref counts? */
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_tr2_mirror_port_ipipe_dest_get
 * Description:
 *      Get IPIPE ingress/egress mirroring destinations for the
 *      specific port.
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      array_sz   - (IN)  Sizeof dest_array parameter.
 *      dest_array - (OUT) Mirror to port array.
 *      egress     - (IN)  (TRUE/FALSE) Egress mirror.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mirror_port_ipipe_dest_get(int unit, bcm_port_t port,
                                    int array_sz, bcm_gport_t *dest_array,
                                    int egress)
{
    uint32 mtp_value;          /* MTP index value.            */
    uint64 reg_val;            /* MTP control register value. */ 
    int enable;                /* Mirror enable bitmap.       */
    int index;                 /* Destination iteration index.*/
    int mtp_slot, mtp_bit;

    /* Input parameters check. */
    if ((NULL == dest_array) || (0 == array_sz)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination array. */
    for (index = 0; index < array_sz; index ++) {
        dest_array[index] = BCM_GPORT_INVALID;
    }

    /* Read mtp ingress mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN(_bcm_port_mirror_enable_get(unit, port, &enable));

    if (egress) {
        enable &= MIRROR_CONFIG_MTP_MODE_BMP(unit); /* Only egress slots */
    } else {
        enable &= ~MIRROR_CONFIG_MTP_MODE_BMP(unit); /* Only ingress slots */
    }

    if (!enable) {
        return (BCM_E_NONE);
    }

    /* Read mirror control register to compare programmed mtp indexes. */
    BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &reg_val));

    index = 0;

    for (mtp_slot = 0, mtp_bit = BCM_MIRROR_MTP_ONE;
         (mtp_bit & MIRROR_CONFIG_MTP_DEV_MASK(unit)) != 0;
         mtp_bit <<= 1, mtp_slot++) {
        if (enable & mtp_bit) {
            mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr,
                                  reg_val, _tr2_mtp_index_f[mtp_slot]);

            dest_array[index] = egress ?
                MIRROR_CONFIG_EGR_MTP_DEST(unit, mtp_value) :
                MIRROR_CONFIG_ING_MTP_DEST(unit, mtp_value);
            index++;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_mirror_port_egress_true_dest_get
 * Description:
 *      Get IP ingress/egress mirroring destinations for the specific port.
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      array_sz   - (IN)  Sizeof dest_array parameter.
 *      dest_array - (OUT) Mirror to port array.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mirror_port_egress_true_dest_get(int unit, bcm_port_t port,
                                          int array_sz,
                                          bcm_gport_t *dest_array)
{
    int enable;                /* Mirror enable bitmap.       */
    int index;                 /* Destination iteration index.*/
    int mtp_slot, mtp_bit;

    /* Input parameters check. */
    if ((NULL == dest_array) || (0 == array_sz)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination array. */
    for (index = 0; index < array_sz; index ++) {
        dest_array[index] = BCM_GPORT_INVALID;
    }

    /* Read mtp egress true mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN
        (_bcm_port_mirror_egress_true_enable_get(unit, port, &enable));

    if (!enable) {
        return (BCM_E_NONE);
    }

    index = 0;

    /* Egress true mirroring uses 1-1 MTP index to slot mapping */
    for (mtp_slot = 0, mtp_bit = BCM_MIRROR_MTP_ONE;
         (mtp_bit & MIRROR_CONFIG_MTP_DEV_MASK(unit)) != 0;
         mtp_bit <<= 1, mtp_slot++) {
        if (enable & mtp_bit) {
            dest_array[index] =
                MIRROR_CONFIG_EGR_TRUE_MTP_DEST(unit, mtp_slot);
            index++;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_mirror_ipipe_mtp_uninstall
 * Description:
 *      Reset ingress reserved mtp index from 
 *      mirror control register. 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      mtp_index  - (IN)  Mirror to port index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mirror_ipipe_mtp_uninstall(int unit, bcm_port_t port,
                                     int mtp_index, int egress)
{
    uint64 reg_val;            /* MTP control register value. */
    uint32 ms_reg;             /* MTP mode register value     */
    int mtp_value;             /* MTP index value.            */
    int comb_enable, enable;   /* Mirror enable bitmaps.      */
    int rv = BCM_E_NOT_FOUND;  /* Operation return status.    */
    int mtp_slot, mtp_bit;     /* MTP iteration values        */

    /* Read mtp ipipe mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN(_bcm_port_mirror_enable_get(unit, port, &comb_enable));

    if (egress) {
        enable = comb_enable & MIRROR_CONFIG_MTP_MODE_BMP(unit);
        /* Only egress slots */
    } else {
        enable = comb_enable & ~MIRROR_CONFIG_MTP_MODE_BMP(unit);
        /* Only ingress slots */
    }

    if (enable) {
        /* Read mirror control register to compare programmed mtp indexes. */
        BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &reg_val));
    }

    for (mtp_slot = 0, mtp_bit = BCM_MIRROR_MTP_ONE;
         (mtp_bit & MIRROR_CONFIG_MTP_DEV_MASK(unit)) != 0;
         mtp_bit <<= 1, mtp_slot++) {
        if (enable & mtp_bit) {
            mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr,
                                  reg_val, _tr2_mtp_index_f[mtp_slot]);
            if (mtp_value == mtp_index) {
                /* Removed mtp was found -> disable it. */

                /* Disable ipipe mirroring on port. */
                comb_enable &= ~mtp_bit;
                BCM_IF_ERROR_RETURN 
                    (_bcm_port_mirror_enable_set(unit, port, comb_enable));

                /* Reset ipipe mirroring mtp index. */
                BCM_IF_ERROR_RETURN 
                    (soc_reg_field32_modify(unit, MIRROR_CONTROLr,
                                  port, _tr2_mtp_index_f[mtp_slot], 0));

                if (IS_HG_PORT(unit, port)) {
                    BCM_IF_ERROR_RETURN 
                        (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, 
                                      port, _tr2_mtp_index_f[mtp_slot], 0));
                }

                if (MIRROR_CONFIG_MTP_MODE_REF_COUNT(unit, mtp_slot) > 0) {
                    MIRROR_CONFIG_MTP_MODE_REF_COUNT(unit, mtp_slot)--;
                }
                if (egress &&
                    !(MIRROR_CONFIG_MTP_MODE_REF_COUNT(unit, mtp_slot))) {
                    /* Free MTP_MODE */
                    MIRROR_CONFIG_MTP_MODE_BMP(unit) &= ~mtp_bit;

                    BCM_IF_ERROR_RETURN(READ_MIRROR_SELECTr(unit, &ms_reg));
                    soc_reg_field_set(unit, MIRROR_SELECTr, &ms_reg,
                                      MTP_TYPEf,
                                      MIRROR_CONFIG_MTP_MODE_BMP(unit));
                    BCM_IF_ERROR_RETURN(WRITE_MIRROR_SELECTr(unit, ms_reg));
                    BCM_IF_ERROR_RETURN(WRITE_EGR_MIRROR_SELECTr(unit, ms_reg));
                }
                
                rv = (BCM_E_NONE);
                break;
            }
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_mirror_egress_true_mtp_uninstall
 * Description:
 *      Uninstall egress true mirroring reserved mtp index from 
 *      mirror control register. 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      mtp_index  - (IN)  Mirror to port index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_tr2_mirror_egress_true_mtp_uninstall(int unit, bcm_port_t port,
                                          int mtp_index)
{
    int enable;                /* Used mtp bit map.           */

    /* Read mtp egress true mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN
        (_bcm_port_mirror_egress_true_enable_get(unit, port, &enable));

    if ((enable & (1 << mtp_index))) {
        enable &= ~(1 << mtp_index);
        BCM_IF_ERROR_RETURN
            (_bcm_port_mirror_egress_true_enable_set(unit, port, enable));
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *      _bcm_xgs3_mirror_ingress_mtp_install
 * Description:
 *      Install ingress reserved mtp index into 
 *      mirror control register. 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      mtp_index  - (IN)  Mirror to port index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mirror_ingress_mtp_install(int unit, bcm_port_t port, int mtp_index)
{
    uint64 reg_val;            /* MTP control register value. */
    int enable = 0;            /* Used mtp bit map.           */
    int rv = BCM_E_FULL;       /* Operation return status.    */
    int hw_mtp;                /* Hw installed mtp index.     */

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        return _bcm_tr2_mirror_ipipe_mtp_install(unit, port,
                                                 mtp_index, FALSE);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* Read mtp ingress mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN(_bcm_port_mirror_enable_get(unit, port, &enable));

    /* Read mirror control register to compare programmed mtp indexes. */
    BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &reg_val));

    if (!(enable & BCM_MIRROR_MTP_ONE)) {
        /* Mtp one is available */
        soc_reg64_field32_set(unit, MIRROR_CONTROLr, &reg_val,
                              IM_MTP_INDEXf, mtp_index);

        /* Set mtp index in Mirror control. */
        BCM_IF_ERROR_RETURN(WRITE_MIRROR_CONTROLr(unit, port, reg_val));

        /* Enable ingress mirroring on the port. */
        enable |= BCM_MIRROR_MTP_ONE;
        BCM_IF_ERROR_RETURN
            (_bcm_port_mirror_enable_set(unit, port, enable));
        if (IS_HG_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, 
                                        port, IM_MTP_INDEXf,
                                        mtp_index));
        }
        rv = (BCM_E_NONE);
    } else {
        /* Mtp one is in use */
        /* Check if mtp is already installed. */
        hw_mtp = soc_reg64_field32_get(unit, MIRROR_CONTROLr, reg_val,
                                       IM_MTP_INDEXf);
        if (mtp_index == hw_mtp) {
            rv = (BCM_E_NONE);
        }
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && BCM_FAILURE(rv)){
        if (!(enable & BCM_MIRROR_MTP_TWO)) {
            /* Mtp two is availbale */
            soc_reg64_field32_set(unit, MIRROR_CONTROLr, &reg_val,
                                  IM_MTP_INDEX1f, mtp_index);

            /* Set mtp index in Mirror control. */
            BCM_IF_ERROR_RETURN(WRITE_MIRROR_CONTROLr(unit, port, reg_val));

            /* Enable ingress mirroring on the port. */
            enable |= BCM_MIRROR_MTP_TWO;
            BCM_IF_ERROR_RETURN
                (_bcm_port_mirror_enable_set(unit, port, enable));
            if (IS_HG_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, 
                                            port, IM_MTP_INDEX1f,
                                            mtp_index));
            }
            rv = (BCM_E_NONE);
        } else {
            /* Mtp two is in use */
            /* Check if mtp is already installed. */
            hw_mtp = soc_reg64_field32_get(unit, MIRROR_CONTROLr,reg_val,
                                           IM_MTP_INDEX1f);
            if (mtp_index == hw_mtp) {
                rv = (BCM_E_NONE);
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}

/*
 * Function:
 *      _bcm_xgs3_mirror_egress_mtp_install
 * Description:
 *      Install egress reserved mtp index into 
 *      mirror control register. 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      mtp_index  - (IN)  Mirror to port index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mirror_egress_mtp_install(int unit, bcm_port_t port, int mtp_index)
{
    uint64 reg_val;            /* MTP control register value. */
    int enable = 0;            /* Used mtp bit map.           */
    int hw_mtp;                /* Hw installed mtp index.     */
    int rv = BCM_E_FULL;       /* Operation return status.    */

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        return _bcm_tr2_mirror_ipipe_mtp_install(unit, port,
                                                 mtp_index, TRUE);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* Read mtp egress mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN(_bcm_esw_mirror_egress_get(unit, port, &enable));

    /* Read mirror control register to compare programmed mtp indexes. */
    BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &reg_val));

    if (!(enable & BCM_MIRROR_MTP_ONE)) {
        /* Mtp one is availbale */
        soc_reg64_field32_set(unit, MIRROR_CONTROLr, &reg_val, EM_MTP_INDEXf, mtp_index);

        /* Set mtp index in Mirror control. */
        BCM_IF_ERROR_RETURN(WRITE_MIRROR_CONTROLr(unit, port, reg_val));

        /* Set multicast mtp index in Mirror control. */
        BCM_IF_ERROR_RETURN (soc_reg_field32_modify(unit, MIRROR_CONTROLr, 
                                                    port, NON_UC_EM_MTP_INDEXf,
                                                    mtp_index));
        /* Enable egress mirroring . */
        enable |= BCM_MIRROR_MTP_ONE;
        BCM_IF_ERROR_RETURN(_bcm_esw_mirror_egress_set(unit, port, enable));
         

        if (IS_HG_PORT(unit, port)) {
            BCM_IF_ERROR_RETURN 
                (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, port, 
                                        EM_MTP_INDEXf, mtp_index));
        }
        rv = (BCM_E_NONE);
    } else {
        /* Mtp one is in use */
        /* Check if mtp is already installed. */
        hw_mtp = soc_reg64_field32_get (unit, MIRROR_CONTROLr, reg_val,
                                        EM_MTP_INDEXf);

        if (mtp_index == hw_mtp) {
            rv = (BCM_E_NONE);
        }
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && BCM_FAILURE(rv)){
        if (!(enable & BCM_MIRROR_MTP_TWO)) {
            /* Mtp two is availbale */
            soc_reg64_field32_set(unit, MIRROR_CONTROLr, &reg_val, EM_MTP_INDEX1f, mtp_index);

            /* Set mtp index in Mirror control. */
            BCM_IF_ERROR_RETURN(WRITE_MIRROR_CONTROLr(unit, port, reg_val));

            /* Enable ingress mirroring on the port. */
            enable |= BCM_MIRROR_MTP_TWO;
            BCM_IF_ERROR_RETURN(_bcm_esw_mirror_egress_set(unit, port, enable));
            if (IS_HG_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, 
                                                            port, EM_MTP_INDEX1f,
                                                            mtp_index));
            }
            rv = (BCM_E_NONE);
        } else {
            /* Mtp one is in use */
            /* Check if mtp is already installed. */
            hw_mtp = soc_reg64_field32_get(unit, MIRROR_CONTROLr, reg_val,
                                           EM_MTP_INDEX1f);
            if (mtp_index == hw_mtp) {
                rv = (BCM_E_NONE);
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}

/*
 * Function:
 *      _bcm_xgs3_mirror_port_ingress_dest_get
 * Description:
 *      Get ingress mirroring destinations for the specific port 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      array_sz   - (IN)  Sizeof dest_array parameter.
 *      dest_array - (OUT) Mirror to port array.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mirror_port_ingress_dest_get(int unit, bcm_port_t port,
                                       int array_sz, bcm_gport_t *dest_array)
{
    uint32 mtp_value;          /* MTP index value.            */
    uint64 reg_val;            /* MTP control register value. */ 
    int enable;                /* Mirror enable bitmap.       */
    int index;                 /* Destination iteration index.*/

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        return _bcm_tr2_mirror_port_ipipe_dest_get(unit, port, array_sz,
                                                   dest_array, FALSE);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* Input parameters check. */
    if ((NULL == dest_array) || (0 == array_sz)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination array. */
    for (index = 0; index < array_sz; index ++) {
        dest_array[index] = BCM_GPORT_INVALID;
    }

    /* Read mtp ingress mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN(_bcm_port_mirror_enable_get(unit, port, &enable));

    if (!enable) {
        return (BCM_E_NONE);
    }

    /* Read mirror control register to compare programmed mtp indexes. */
    BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &reg_val));

    index = 0;

    if (enable & BCM_MIRROR_MTP_ONE) {
        /* Mtp one is in use */
        mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr, 
                                          reg_val, IM_MTP_INDEXf);

        dest_array[index] = MIRROR_CONFIG_ING_MTP_DEST(unit, mtp_value);
        index++;
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && (index < array_sz) ){
        if (enable & BCM_MIRROR_MTP_TWO) {
            /* Mtp one is in use */
            mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr, 
                                              reg_val, IM_MTP_INDEX1f);

            dest_array[index] = MIRROR_CONFIG_ING_MTP_DEST(unit, mtp_value);
            index++;
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _bcm_xgs3_mirror_port_egress_dest_get
 * Description:
 *      Get egress mirroring  destinations for the specific port 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      array_sz   - (IN)  Sizeof dest_array parameter.
 *      dest_array - (OUT) Mirror to port array.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mirror_port_egress_dest_get(int unit, bcm_port_t port, int array_sz,
                                      bcm_gport_t *dest_array)
{
    uint32 mtp_value;          /* MTP index value.            */
    uint64 reg_val;            /* MTP control register value. */ 
    int enable;                /* Mirror enable bitmap.       */
    int index;                 /* Destination iteration index.*/

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        return _bcm_tr2_mirror_port_ipipe_dest_get(unit, port, array_sz,
                                                   dest_array, TRUE);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* Input parameters check. */
    if ((NULL == dest_array) || (0 == array_sz)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination array. */
    for (index = 0; index < array_sz; index ++) {
        dest_array[index] = BCM_GPORT_INVALID;
    }

    /* Read mtp ingress mtp enable enable bitmap for source port. */
    BCM_IF_ERROR_RETURN(_bcm_esw_mirror_egress_get(unit, port, &enable));

    if (!enable) {
        return (BCM_E_NONE);
    }

    /* Read mirror control register to compare programmed mtp indexes. */
    BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &reg_val));

    index = 0;

    if (enable & BCM_MIRROR_MTP_ONE) {
        /* Mtp one is in use */
        mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr, 
                                          reg_val, EM_MTP_INDEXf);

        dest_array[index] = MIRROR_CONFIG_EGR_MTP_DEST(unit, mtp_value);
        index++;
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && (index < array_sz) ){
        if (enable & BCM_MIRROR_MTP_TWO) {
            /* Mtp one is in use */
            mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr, 
                                              reg_val, EM_MTP_INDEX1f);

            dest_array[index] = MIRROR_CONFIG_EGR_MTP_DEST(unit, mtp_value);
            index++;
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_xgs3_mirror_ingress_mtp_uninstall
 * Description:
 *      Reset ingress reserved mtp index from 
 *      mirror control register. 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      mtp_index  - (IN)  Mirror to port index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mirror_ingress_mtp_uninstall(int unit, bcm_port_t port, int mtp_index)
{
    int mtp_value;             /* MTP index value.            */
    uint64 reg_val;            /* MTP control register value. */ 
    int enable;                /* Mirror enable bitmap.       */
    int rv = BCM_E_NOT_FOUND;  /* Operation return status.    */

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        return _bcm_tr2_mirror_ipipe_mtp_uninstall(unit, port,
                                                   mtp_index, FALSE);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* Read mtp ingress mtp enable enable bitmap for source port. */
    BCM_IF_ERROR_RETURN(_bcm_port_mirror_enable_get(unit, port, &enable));

    if (enable) {
        /* Read mirror control register to compare programmed mtp indexes. */
        BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &reg_val));
    }

    if (enable & BCM_MIRROR_MTP_ONE) {
        /* Mtp one is in use */
        mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr, 
                                          reg_val, IM_MTP_INDEXf);

        if (mtp_value == mtp_index) {
            /* Removed mtp was found -> disable it. */

            /* Disable ingress mirroring on port. */
            enable &= ~BCM_MIRROR_MTP_ONE;
            BCM_IF_ERROR_RETURN 
                (_bcm_port_mirror_enable_set(unit, port, enable));

            /* Reset ingress mirroring mtp index. */
            BCM_IF_ERROR_RETURN 
                (soc_reg_field32_modify(unit, MIRROR_CONTROLr,
                                        port, IM_MTP_INDEXf, 0));

            if (IS_HG_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN 
                    (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, 
                                            port, IM_MTP_INDEXf, 0));
            }
            rv = (BCM_E_NONE);
        }
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && BCM_FAILURE(rv)){
        if (enable & BCM_MIRROR_MTP_TWO) {
            /* Mtp one is in use */
            mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr, 
                                              reg_val, IM_MTP_INDEX1f);

            if (mtp_value == mtp_index) {
                /* Removed mtp was found -> disable it. */

                /* Disable ingress mirroring on port. */
                enable &= ~BCM_MIRROR_MTP_TWO;
                BCM_IF_ERROR_RETURN 
                    (_bcm_port_mirror_enable_set(unit, port, enable));

                /* Reset ingress mirroring mtp index. */
                BCM_IF_ERROR_RETURN 
                    (soc_reg_field32_modify(unit, MIRROR_CONTROLr,
                                            port, IM_MTP_INDEX1f, 0));

                if (IS_HG_PORT(unit, port)) {
                    BCM_IF_ERROR_RETURN 
                        (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, 
                                                port, IM_MTP_INDEX1f, 0));
                }
                rv = (BCM_E_NONE);
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}

/*
 * Function:
 *      _bcm_xgs3_mirror_egress_mtp_uninstall
 * Description:
 *      Reset egress reserved mtp index from 
 *      mirror control register. 
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      port       - (IN)  Mirror source gport.
 *      mtp_index  - (IN)  Mirror to port index.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mirror_egress_mtp_uninstall(int unit, bcm_port_t port, int mtp_index)
{
    int mtp_value;             /* MTP index value.            */
    uint64 reg_val;            /* MTP control register value. */ 
    int enable;                /* Mirror enable bitmap.       */
    int rv =BCM_E_NOT_FOUND;   /* Operation return status.    */

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        return _bcm_tr2_mirror_ipipe_mtp_uninstall(unit, port,
                                                   mtp_index, TRUE);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* Read mtp egress mtp enable bitmap for source port. */
    BCM_IF_ERROR_RETURN (_bcm_esw_mirror_egress_get(unit, port, &enable));

    if (enable) {
        /* Read mirror control register to compare programmed mtp indexes. */
        BCM_IF_ERROR_RETURN(READ_MIRROR_CONTROLr(unit, port, &reg_val));
    }

    if (enable & BCM_MIRROR_MTP_ONE) {
        /* Mtp one is in use */
        mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr,
                                          reg_val, EM_MTP_INDEXf);

        if (mtp_value == mtp_index) {
            /* Removed mtp was found -> disable it. */

            /* Disable egress mirroring. */
            enable &= ~BCM_MIRROR_MTP_ONE;
            BCM_IF_ERROR_RETURN(_bcm_esw_mirror_egress_set(unit, port, enable));

            /* Reset ingress mirroring mtp index. */
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MIRROR_CONTROLr,
                                        port, EM_MTP_INDEXf, 0));

            /* Reset multicast mtp index in mirror control. */
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, MIRROR_CONTROLr,
                                        port, NON_UC_EM_MTP_INDEXf, 0));

            if (IS_HG_PORT(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, IMIRROR_CONTROLr,
                                            port, EM_MTP_INDEXf, 0));
            }
            rv = (BCM_E_NONE);
        }
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && BCM_FAILURE(rv)){
        if (enable & BCM_MIRROR_MTP_TWO) {
            /* Mtp two is in use */
            mtp_value = soc_reg64_field32_get(unit, MIRROR_CONTROLr,
                                              reg_val, EM_MTP_INDEX1f);

            if (mtp_value == mtp_index) {
                /* Removed mtp was found -> disable it. */

                /* Disable ingress mirroring. */
                enable &= ~BCM_MIRROR_MTP_TWO;
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_mirror_egress_set(unit, port, enable));

                /* Reset ingress mirroring mtp index. */
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, MIRROR_CONTROLr,
                                            port, EM_MTP_INDEX1f, 0));

                /* Reset multicast mtp index in mirror control. */
                BCM_IF_ERROR_RETURN
                    (soc_reg_field32_modify(unit, MIRROR_CONTROLr,
                                            port, NON_UC_EM_MTP_INDEX1f, 0));

                if (IS_HG_PORT(unit, port)) {
                    BCM_IF_ERROR_RETURN 
                        (soc_reg_field32_modify(unit, IMIRROR_CONTROLr,
                                                port, EM_MTP_INDEX1f, 0));
                }
                rv = (BCM_E_NONE);
            }
        }
    }
#endif /* BCM_TRX_SUPPORT */
    return (rv);
}


/*
 * Function:
 *	   _bcm_xgs3_mirror_enable_set 
 * Purpose:
 *  	Enable/disable mirroring on a port. 
 * Parameters:
 *	    unit - BCM device number
 *  	port - port number
 *   	enable - enable mirroring if non-zero
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int 
_bcm_xgs3_mirror_enable_set(int unit, int port, int enable)
{
    int directed;        /* Directed mirroring indicator. */

    /* Check if directed mirroring is enabled. */
    BCM_IF_ERROR_RETURN(_bcm_esw_directed_mirroring_get(unit, &directed));
    
    /* Higig port should never drop directed mirror packets */
    if (IS_ST_PORT(unit, port) && directed) {
        enable = 1;
    }

    BCM_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, MIRROR_CONTROLr, port, M_ENABLEf,
                                enable));
    /* Enable mirroring of CPU Higig packets as well */
    if (IS_CPU_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IMIRROR_CONTROLr, port,
                                    M_ENABLEf, enable));
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *      bcm_xgs3_mirror_egress_path_set
 * Description:
 *      Set egress mirror packet path for stack ring
 * Parameters:
 *      unit    - (IN) BCM device number
 *      modid   - (IN) Destination module ID (of mirror-to port)
 *      port    - (IN) Stack port for egress mirror packet
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This function should only be used for XGS3 devices stacked
 *      in a ring configuration with fabric devices that may block
 *      egress mirror packets when the mirror-to port is on a 
 *      different device than the egress port being mirrored.
 *      Currently the only such fabric device is BCM5675 rev A0.
 */
int
bcm_xgs3_mirror_egress_path_set(int unit, bcm_module_t modid, bcm_port_t port)
{
    alternate_emirror_bitmap_entry_t egr_bmp;

    if (!soc_feature(unit, soc_feature_egr_mirror_path)) {
        return (BCM_E_UNAVAIL);
    }

    if ( !SOC_MODID_ADDRESSABLE(unit, modid)){
        return (BCM_E_BADID);
    }
    if (!IS_ST_PORT(unit, port)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ALTERNATE_EMIRROR_BITMAPm,
                                     MEM_BLOCK_ANY, modid, &egr_bmp));
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        soc_field_t bmapf = (port < 32) ? BITMAP_LOf : BITMAP_HIf;
        uint32 shift = (port < 32) ? port : (port - 32);
        soc_ALTERNATE_EMIRROR_BITMAPm_field32_set(unit, &egr_bmp,
                                                  bmapf, 1 << shift);
    } else
#endif /* BCM_TRIUMPH2_SUPPORT */
    {
#if defined(BCM_FIREBOLT_SUPPORT)
        if (SOC_IS_FBX(unit)) {
            port -= SOC_HG_OFFSET(unit);
        }
#endif /* BCM_FIREBOLT_SUPPORT */
        soc_ALTERNATE_EMIRROR_BITMAPm_field32_set(unit, &egr_bmp,
                                                  BITMAPf, 1 << port);
    } 
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ALTERNATE_EMIRROR_BITMAPm,
                                      MEM_BLOCK_ALL, modid, &egr_bmp));
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_xgs3_mirror_egress_path_get
 * Description:
 *      Get egress mirror packet path for stack ring
 * Parameters:
 *      unit    - (IN) BCM device number
 *      modid   - (IN) Destination module ID (of mirror-to port)
 *      port    - (OUT)Stack port for egress mirror packet
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      See bcm_mirror_alt_egress_pbmp_set for more details.
 */
int
bcm_xgs3_mirror_egress_path_get(int unit, bcm_module_t modid, bcm_port_t *port)
{
    alternate_emirror_bitmap_entry_t egr_bmp;
    uint32 val, p, start = 0;

    if (NULL == port) {
        return (BCM_E_PARAM);
    }

    if (!soc_feature(unit, soc_feature_egr_mirror_path)) {
        return (BCM_E_UNAVAIL);
    }
    if (!SOC_MODID_ADDRESSABLE(unit, modid)) {
        return (BCM_E_BADID);
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ALTERNATE_EMIRROR_BITMAPm,
                                     MEM_BLOCK_ANY, modid, &egr_bmp));
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) ||
        SOC_IS_VALKYRIE2(unit)) {
        val = soc_ALTERNATE_EMIRROR_BITMAPm_field32_get(unit, &egr_bmp, BITMAP_LOf);
        if (val == 0) {
            val = soc_ALTERNATE_EMIRROR_BITMAPm_field32_get(unit, &egr_bmp, 
                                                            BITMAP_HIf);
            start = 32;
        }
        start--;
    } else
#endif /* BCM_TRIUMPH2_SUPPORT */
    {
        val = soc_ALTERNATE_EMIRROR_BITMAPm_field32_get(unit, &egr_bmp, BITMAPf);
    }
    if (val == 0) {
        /* Return default egress port */
        return bcm_esw_topo_port_get(unit, modid, port);
    }
    for (p = start; val; p++) {
        val >>= 1;
    }
    if (SOC_IS_FBX(unit) && !SOC_IS_TRIUMPH2(unit) && 
        !SOC_IS_APOLLO(unit) && !SOC_IS_VALKYRIE2(unit)) {
        p += SOC_HG_OFFSET(unit);
    }
    *port = p;

    return (BCM_E_NONE);
}

/*
 * Function:
 *  	_bcm_xgs3_mirror_egr_dest_get 
 * Purpose:
 *  	Get destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)port number.
 *      flags       - (IN) Destination flags.(BCM_MIRROR_MTP_ONE/TWO)
 *      dest_bitmap - (OUT) destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mirror_egr_dest_get(int unit, bcm_port_t port, uint32 flags,
                                bcm_pbmp_t *dest_bitmap)
{
    uint32 mirror;
    soc_reg_t reg = INVALIDr;
    uint32 rv = BCM_E_UNAVAIL; 

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && (flags & BCM_MIRROR_MTP_TWO)) {
        reg = EMIRROR_CONTROL1r;
    } else 
#endif /* BCM_TRX_SUPPORT */
    {
        reg = EMIRROR_CONTROLr;
    }

    rv = soc_reg32_read(unit, soc_reg_addr(unit, reg, port, 0), &mirror);
    SOC_PBMP_WORD_SET(*dest_bitmap, 0, 
                      soc_reg_field_get(unit, reg, mirror, BITMAPf));

    return (rv);
}

/*
 * Function:
 *  	_bcm_xgs3_mirror_egr_dest_set 
 * Purpose:
 *  	Set destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN)BCM device number.
 *	    port        - (IN)Port number.
 *      flags       - (IN) Destination flags.(BCM_MIRROR_MTP_ONE/TWO)
 *      dest_bitmap - (IN)Destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_xgs3_mirror_egr_dest_set(int unit, bcm_port_t port,
                              uint32 flags, bcm_pbmp_t *dest_bitmap)
{
    soc_reg_t   reg;
    uint32      value;                  
    soc_field_t field = BITMAPf;

    /* Input parameters check. */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && (flags & BCM_MIRROR_MTP_TWO)) {
        reg = EMIRROR_CONTROL1r;
    } else 
#endif /* BCM_TRX_SUPPORT */
    {
        reg = EMIRROR_CONTROLr;
    }

    value = SOC_PBMP_WORD_GET(*dest_bitmap, 0);

    BCM_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit, reg, port, 1, &field, &value));

    /* Enable mirroring of CPU Higig packets as well */
    if (IS_CPU_PORT(unit, port)) {
#if defined(BCM_TRX_SUPPORT)
        if (SOC_IS_TRX(unit) && (flags & BCM_MIRROR_MTP_TWO)) {
            reg = IEMIRROR_CONTROL1r;
        } else 
#endif /* BCM_TRX_SUPPORT */
        {
            reg = IEMIRROR_CONTROLr;
        }
        BCM_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, reg, port, 1, &field, &value));
    }
    return (BCM_E_NONE);
}

#endif /* BCM_XGS3_SWITCH_SUPPORT */

/*
 * Function:
 *  	_bcm_esw_mirror_egr_dest_set
 * Purpose:
 *  	Set destination port bitmap for egress mirroring.
 * Parameters:
 *	    unit        - (IN) BCM device number
 *	    port        - (IN) Port number
 *      flags       - (IN) Destination flags.(BCM_MIRROR_MTP_ONE/TWO)
 *      dest_bitmap - (IN) destination port bitmap
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_egr_dest_set(int unit, bcm_port_t port, 
                             uint32 flags, bcm_pbmp_t *dest_bitmap)
{
    int rv;

    /* Input parameters check */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_TUCANA_SUPPORT)
    if (SOC_IS_TUCANA(unit)) {
        rv = _bcm_tucana_mirror_egr_dest_set(unit, port, dest_bitmap);
    } else 
#endif /* BCM_TUCANA_SUPPORT */

#if defined(BCM_XGS12_SWITCH_SUPPORT)
    if (SOC_IS_XGS12_SWITCH(unit)) {
        rv = _bcm_xgs_mirror_egr_dest_set(unit, port, dest_bitmap);
    } else 
#endif /* BCM_XGS12_SWITCH_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        rv = _bcm_tr2_mirror_egr_dest_set(unit, port, flags, dest_bitmap);
    } else
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        rv = _bcm_triumph_mirror_egr_dest_set(unit, port, flags, dest_bitmap);
    } else
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) 
    if (SOC_IS_RAPTOR(unit)) {
        rv = _bcm_raptor_mirror_egr_dest_set(unit, port, dest_bitmap);
    } else 
#endif /* BCM_RAPTOR_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        rv = _bcm_xgs3_mirror_egr_dest_set(unit, port, flags, dest_bitmap);
    } else
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return (rv);
}


/*
 * Function:
 *  	_bcm_esw_mirror_egr_dest_get
 * Purpose:
 *      Get destination port bitmap for egress mirroring.
 * Parameters:
 *  	unit        - (IN) BCM device number.
 *  	port        - (IN) Port number.
 *      flags       - (IN) Destination flags.(BCM_MIRROR_MTP_ONE/TWO)
 *      dest_bitmap - (OUT) destination port bitmap.
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_egr_dest_get(int unit, bcm_port_t port, uint32 flags, 
                              bcm_pbmp_t *dest_bitmap)
{
    int rv;

    /* Input parameters check */
    if (NULL == dest_bitmap) {
        return (BCM_E_PARAM);
    }

    SOC_PBMP_CLEAR(*dest_bitmap);

#if defined(BCM_TUCANA_SUPPORT)
    if (SOC_IS_TUCANA(unit)) {
        rv = _bcm_tucana_mirror_egr_dest_get(unit, port, dest_bitmap);
    } else 
#endif /* BCM_TUCANA_SUPPORT */

#if defined(BCM_XGS12_SWITCH_SUPPORT)
    if (SOC_IS_XGS12_SWITCH(unit)) {
        rv = _bcm_xgs_mirror_egr_dest_get(unit, port, dest_bitmap);
    } else 
#endif /* BCM_XGS12_SWITCH_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        rv = _bcm_tr2_mirror_egr_dest_get(unit, port, flags, dest_bitmap);
    } else
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit)) {
        rv = _bcm_triumph_mirror_egr_dest_get(unit, port, flags, dest_bitmap);
    } else
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) 
    if (SOC_IS_RAPTOR(unit)) {
        rv = _bcm_raptor_mirror_egr_dest_get(unit, port, dest_bitmap);
    } else 
#endif /* BCM_RAPTOR_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        rv = _bcm_xgs3_mirror_egr_dest_get(unit, port, flags, dest_bitmap);
    } else
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }

    return (rv);
}

/*
 * Function:
 *	    _bcm_esw_mirror_egress_get
 * Description:
 * 	    Get the mirroring per egress enabled/disabled status
 * Parameters:
 *  	unit -   (IN)  BCM device number
 *  	port -   (IN)  The port to check
 *  	enable - (OUT) Place to store boolean return value for on/off
 * Returns:
 *  	BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_egress_get(int unit, bcm_port_t port, int *enable)
{
    bcm_port_t port_iterator;
    bcm_pbmp_t dest_bitmap;
    int rv = BCM_E_UNAVAIL; 
    int value = 0;
    int mtp_bit;

    BCM_PBMP_CLEAR(dest_bitmap);

    /* Get destination port bitmap from first valid port. */
    PBMP_ALL_ITER(unit, port_iterator) {
        rv = BCM_E_NONE;
        for (mtp_bit = BCM_MIRROR_MTP_ONE;
             (mtp_bit & MIRROR_CONFIG_MTP_DEV_MASK(unit)) != 0;
             mtp_bit <<= 1) {
            rv = _bcm_esw_mirror_egr_dest_get(unit, port_iterator, 
                                              mtp_bit, &dest_bitmap);
            if (SOC_PBMP_MEMBER(dest_bitmap, port)) {
                value |= mtp_bit;
            }
        }
        /* Only care about finding one valid port */
        break;
    }

    *enable = value;
    return (rv);
}


/*
 * Function:
 * 	   _bcm_esw_mirror_egress_set
 * Description:
 *  	Enable or disable mirroring per egress
 * Parameters:
 *  	unit   - (IN) BCM device number
 *	    port   - (IN) The port to affect
 *	    enable - (IN) Boolean value for on/off
 * Returns:
 *	    BCM_E_XXX
 * Notes:
 *  	Mirroring must also be globally enabled.
 */
STATIC int
_bcm_esw_mirror_egress_set(int unit, bcm_port_t port, int enable)
{
    bcm_port_t port_iterator;
    bcm_pbmp_t dest_bitmap;
    int rv = BCM_E_UNAVAIL;
    int mtp_bit;

    PBMP_ALL_ITER(unit, port_iterator) {
        for (mtp_bit = BCM_MIRROR_MTP_ONE;
             (mtp_bit & MIRROR_CONFIG_MTP_DEV_MASK(unit)) != 0;
             mtp_bit <<= 1) {
            /* Read egress destination bitmap from each local port. */
            rv = _bcm_esw_mirror_egr_dest_get(unit, port_iterator,
                                              mtp_bit, &dest_bitmap);
            if (BCM_FAILURE(rv)) {
                break;
            }

            /* Update egress destination bitmap. */
            if (enable & mtp_bit) {
                SOC_PBMP_PORT_ADD(dest_bitmap, port);
            } else {
                SOC_PBMP_PORT_REMOVE(dest_bitmap, port);
            }

            /* Write egress destination bitmap from each local port. */
            rv = _bcm_esw_mirror_egr_dest_set(unit, port_iterator,
                                              mtp_bit, &dest_bitmap);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }
    return (rv);
}

/*
 * Function:
 *	  _bcm_esw_directed_mirroring_get
 * Purpose:
 *	  Check if  directed mirroring is enabled on the chip.
 * Parameters:
 *    unit    - (IN) BCM device number.
 *    enable  - (OUT)Directed mirror enabled.
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int
_bcm_esw_directed_mirroring_get(int unit, int *enable)
{
    int  rv = BCM_E_NONE;      /* Operation return status. */

    /* Input parameters check. */
    if (NULL == enable) {
        return (BCM_E_PARAM);
    }

    /* Raptor supports ONLY directed Mirroring */
#if defined(BCM_RAPTOR_SUPPORT)
    if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit) || SOC_IS_HAWKEYE(unit)) {
        *enable = TRUE;
    } else 
#endif  /* BCM_RAPTOR_SUPPORT */
    {
        /* Read switch control to check if directed mirroring is enabled.*/
        rv = bcm_esw_switch_control_get(unit, bcmSwitchDirectedMirroring, enable);
        if (BCM_FAILURE(rv) && (rv == BCM_E_UNAVAIL)) {
            *enable = FALSE;
            rv = BCM_E_NONE;
        }
    }
    return (rv);
}


/*
 * Function:
 *      _bcm_esw_mirror_mtp_reserve 
 * Description:
 *      Reserve a mirror-to port
 * Parameters:
 *      unit       - (IN)  BCM device number
 *      dest_id    - (IN)  Mirror destination id.
 *      flags      - (IN)  Mirrored traffic direction. 
 *      index_used - (OUT) MTP index reserved
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the a mirror-to port is reserved more than once
 *      (without being unreserved) then the same MTP index 
 *      will be returned for each call.
 *      Direction should be either INGRESS or EGRESS
 */
STATIC int
_bcm_esw_mirror_mtp_reserve(int unit, bcm_gport_t dest_id, 
                            uint32 flags, int *index_used)
{
    int rv = BCM_E_RESOURCE;            /* Operation return status. */

    /* Input parameters check. */
    if (NULL == index_used) {
        return (BCM_E_PARAM);
    }

    /* Allocate MTP index for mirror destination. */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        rv = _bcm_xgs3_mirror_mtp_reserve(unit, dest_id, flags, index_used);
    } else
#endif
    {
        *index_used = 0;
        /*  If mirroring is already in use -> 
            make sure destination is identical, increment reference count.*/
        if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)) {
            /* Mirror destination match. check. */
            if (MIRROR_CONFIG_ING_MTP_DEST(unit, 0) == dest_id) {
                MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)++;
                rv = BCM_E_NONE;
            }
        } else { /* Mirroring not in use. */
            MIRROR_CONFIG_ING_MTP_DEST(unit, 0) = dest_id;
            MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)++;
            MIRROR_DEST_REF_COUNT(unit, dest_id)++;
            rv = BCM_E_NONE;
        }

        /* Ingress & Egress mtp are identical for xgs devices. */
        if (BCM_SUCCESS(rv)) {
            MIRROR_CONFIG_EGR_MTP(unit, 0) = MIRROR_CONFIG_ING_MTP(unit, 0);
        }
    }
    return (rv);
}


/*
 * Function:
 *      _bcm_esw_mirror_mtp_unreserve 
 * Description:
 *      Free  mirror-to port
 * Parameters:
 *      unit       - (IN)  BCM device number.
 *      mtp_index  - (IN)  MTP index. 
 *      flags      - (IN)  Mirrored traffic direction. 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_mtp_unreserve(int unit, int mtp_index, uint32 flags)
{
    bcm_gport_t  mirror_dest;

    /* Free MTP index for mirror destination. */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_mirror_mtp_unreserve(unit, mtp_index, flags));
    } else
#endif
    {
        /* Decrement reference counter & reset dest port    */
        /* if destination is no longer in use.              */
        if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0) > 0) {
            MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)--;
            if (0 == MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)) {
                mirror_dest = MIRROR_CONFIG_ING_MTP_DEST(unit, 0);
                MIRROR_CONFIG_ING_MTP_DEST(unit, 0)= BCM_GPORT_INVALID;
                if (MIRROR_DEST_REF_COUNT(unit, mirror_dest) > 0) {
                    MIRROR_DEST_REF_COUNT(unit, mirror_dest)--;
                }
            }
            MIRROR_CONFIG_EGR_MTP(unit, 0) = MIRROR_CONFIG_ING_MTP(unit, 0);
        }
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *	  _bcm_esw_mirror_deinit
 * Purpose:
 *	  Internal routine used to free mirror software module.
 *        control structures. 
 * Parameters:
 *        unit     - (IN) BCM device number.
 *        cfg_ptr  - (IN) Pointer to config structure.
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_deinit(int unit, _bcm_mirror_config_p *cfg_ptr)
{
    _bcm_mirror_config_p ptr;
    int mtp_type;

    /* Sanity checks. */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    /* If meter config was not allocated we are done. */
    if (NULL == cfg_ptr) {
        return (BCM_E_PARAM);
    }

    ptr = *cfg_ptr;
    if (NULL == ptr) {
        return (BCM_E_NONE);
    }

    /* Free mirror destination information. */
    if (NULL != ptr->dest_arr) {
        sal_free(ptr->dest_arr);
    }

    /* Free egress true mtp information. */
    if (NULL != ptr->egr_true_mtp) {
        sal_free(ptr->egr_true_mtp);
    }

    /* Free MTP types records. */
    for (mtp_type = BCM_MTP_SLOT_TYPE_FP;
         mtp_type < BCM_MTP_SLOT_TYPES; mtp_type++) {
        if (NULL != ptr->mtp_slot[mtp_type]) {
            sal_free(ptr->mtp_slot[mtp_type]);
        }
    }

    /* Free egress mtp information. */
    if (NULL != ptr->egr_mtp) {
        sal_free(ptr->egr_mtp);
    }

    /* Free ingress mtp information. */
    if (NULL != ptr->ing_mtp) {
        sal_free(ptr->ing_mtp);
    }

    /* Destroy protection mutex. */
    if (NULL != ptr->mutex) {
        sal_mutex_destroy(ptr->mutex);
    }

    /* Free module configuration structue. */
    sal_free(ptr);
    *cfg_ptr = NULL;
    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_mirror_enable_set 
 * Purpose:
 *	Enable/disable mirroring on a port
 * Parameters:
 *	unit - BCM device number
 *	port - port number
 *	enable - enable mirroring if non-zero
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *      For non-XGS3 devices this function will also set the
 *      mirror-to port.
 */
STATIC int
_bcm_esw_mirror_enable_set(int unit, int port, int enable)
{
#if defined(BCM_XGS12_SWITCH_SUPPORT)
    if (SOC_IS_XGS12_SWITCH(unit)) {
        return _bcm_xgs_mirror_enable_set(unit, port, enable);
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        return _bcm_xgs3_mirror_enable_set(unit, port, enable);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(BCM_XGS12_FABRIC_SUPPORT)
    if (SOC_IS_XGS12_FABRIC(unit)) {
        return _bcm_xgs_fabric_mirror_enable_set(unit, port, enable);
    }
#endif /* BCM_XGS12_FABRIC_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	  _bcm_esw_mirror_mode_set
 * Description:
 *	  Enable or disable mirroring.  Will wait for bcm_esw_mirror_to_set
 *        to be called to actually do the enable if needed.
 * Parameters:
 *        unit            - (IN)     BCM device number
 *	  mode            - (IN)     One of BCM_MIRROR_(DISABLE|L2|L2_L3)
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_mode_set(int unit, int mode)
{
    int    menable;            /* Enable mirroring flag.      */
    int	      port;            /* Port iterator.              */
    int      omode;            /* Original mirroring mode.    */
#if defined (BCM_XGS3_SWITCH_SUPPORT)
    int      enable;            /* By direction mirror enable. */
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    int  rv = BCM_E_UNAVAIL;   /* Operation return status.    */

    /* Preserve original module configuration. */
    omode = MIRROR_CONFIG_MODE(unit);  

    /* Update module mode. */
    MIRROR_CONFIG_MODE(unit) = mode;
    menable = (BCM_MIRROR_DISABLE != mode) ? TRUE : FALSE;

    if (!menable) {
        /* If mirroring was originally off - we are done. */
        if (!SOC_IS_XGS12_FABRIC(unit) && (omode == BCM_MIRROR_DISABLE)) {
            return (BCM_E_NONE);
        }
    }

    /* Wait for mirror_to_set() */
    if ((BCM_GPORT_INVALID == MIRROR_CONFIG_ING_MTP_DEST(unit, 0)) &&
        !SOC_IS_XGS12_FABRIC(unit)) {
        return (BCM_E_NONE);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        PBMP_PORT_ITER(unit, port) {
            rv = bcm_esw_mirror_ingress_get(unit, port, &enable);
            if (BCM_FAILURE(rv)) {
                break;
            }
              
            if (enable) {
                rv = _bcm_xgs3_mirror_ingress_mtp_install(unit, port, 0);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }

            rv = bcm_esw_mirror_egress_get(unit, port, &enable);
            if (BCM_FAILURE(rv)) {
                break;
            }

            if (enable) {
                rv = _bcm_xgs3_mirror_egress_mtp_install(unit, port, 0);
                if (BCM_FAILURE(rv)) {
                    break;
                }
            }

            rv = _bcm_esw_mirror_enable_set(unit, port, menable);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else 
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(BCM_XGS_SWITCH_SUPPORT)
    if (SOC_IS_XGS_SWITCH(unit)) {
        PBMP_PORT_ITER(unit, port) {
            rv = _bcm_esw_mirror_enable_set(unit, port, menable);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else 
#endif /* BCM_XGS_SWITCH_SUPPORT */

#if defined(BCM_XGS_FABRIC_SUPPORT)
    if (SOC_IS_XGS_FABRIC(unit)) {
        PBMP_ST_ITER(unit, port) {
            rv = _bcm_esw_mirror_enable_set(unit, port, menable);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else
#endif /* BCM_XGS_FABRIC_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }
    return (rv);
}

/*
 * Function:
 *	  _bcm_esw_mirror_hw_clear
 * Purpose:
 *	  Clear hw registers/tables & disable mirroring on the device.
 * Parameters:
 *    unit - (IN) BCM device number.
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int
_bcm_esw_mirror_hw_clear(int unit)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int port;   /* Port iteration index. */
    soc_field_t fields[2];
    uint32      values[2] = {0, 0};

    if (SOC_IS_XGS3_SWITCH(unit)) {
        /* Stacking ports should never drop directed mirror packets */
        /* Other ports should default to no mirroring */
        PBMP_ALL_ITER(unit, port) {

            /* Reset global mirror enable bit. */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_mirror_enable_set(unit, port, 
                                            IS_ST_PORT(unit, port) ? 1 : 0));
            /* Disable ingress mirroring. */
            BCM_IF_ERROR_RETURN
                (_bcm_port_mirror_enable_set(unit, port, values[0]));

#if defined(BCM_TRIUMPH2_SUPPORT)
            if (soc_feature(unit, soc_feature_egr_mirror_true) &&
                IS_LB_PORT(unit, port)) {
                uint64 reg_val;
                int mtp_slot, mtp_bit;
                
                /* Set LB port MTP mapping to 1-1 for egress true mirror */
                BCM_IF_ERROR_RETURN
                    (READ_MIRROR_CONTROLr(unit, port, &reg_val));
                for (mtp_slot = 0, mtp_bit = BCM_MIRROR_MTP_ONE;
                     (mtp_bit & MIRROR_CONFIG_MTP_DEV_MASK(unit)) != 0;
                     mtp_bit <<= 1, mtp_slot++) {
                    soc_reg64_field32_set(unit, MIRROR_CONTROLr, &reg_val,
                                          _tr2_mtp_index_f[mtp_slot],
                                          mtp_slot);
                }
                BCM_IF_ERROR_RETURN
                    (WRITE_MIRROR_CONTROLr(unit, port, reg_val));
            }
#endif /* BCM_TRIUMPH2_SUPPORT */

            /* Disable egress mirroring. */
#if defined(BCM_TRIUMPH_SUPPORT)
            if (SOC_REG_IS_VALID(unit, EMIRROR_CONTROL_64r)) {
                fields[0] = BITMAP_LOf;
                fields[1] = BITMAP_HIf;
                if(SOC_IS_ENDURO(unit)) {
                    /* There is no BITMAP_HIf in XXXX_64r for Enduro */
                    BCM_IF_ERROR_RETURN
                        (soc_reg_fields32_modify(unit, EMIRROR_CONTROL_64r,
                                                 port, 1, fields, values));
                } else {
                    BCM_IF_ERROR_RETURN
                        (soc_reg_fields32_modify(unit, EMIRROR_CONTROL_64r,
                                                 port, COUNTOF(values),
                                                 fields, values));
                    BCM_IF_ERROR_RETURN
                        (soc_reg_fields32_modify(unit, EMIRROR_CONTROL1_64r,
                                                 port, COUNTOF(values),
                                                 fields, values));
#if defined(BCM_TRIUMPH2_SUPPORT)
                    if (SOC_REG_IS_VALID(unit, EMIRROR_CONTROL2_64r)) {
                        BCM_IF_ERROR_RETURN
                            (soc_reg_fields32_modify(unit,
                                          EMIRROR_CONTROL2_64r, port,
                                          COUNTOF(values), fields, values));
                        BCM_IF_ERROR_RETURN
                            (soc_reg_fields32_modify(unit,
                                          EMIRROR_CONTROL3_64r, port,
                                          COUNTOF(values), fields, values));
                    }
#endif /* BCM_TRIUMPH2_SUPPORT */
                }
            } else
#endif /* BCM_TRIUMPH_SUPPORT */
            {
                fields[0] = BITMAPf; 
                BCM_IF_ERROR_RETURN
                    (soc_reg_fields32_modify(unit, EMIRROR_CONTROLr, port,
                                             1, fields, values));
#if defined(BCM_RAPTOR_SUPPORT)
                if (SOC_REG_IS_VALID(unit, EMIRROR_CONTROL_HIr)) {
                    BCM_IF_ERROR_RETURN
                        (soc_reg_fields32_modify(unit, EMIRROR_CONTROL_HIr, port,
                                                 1, fields, values));
                }
#endif /* BCM_RAPTOR_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
                if (SOC_REG_IS_VALID(unit, EMIRROR_CONTROL1r)) {
                    BCM_IF_ERROR_RETURN
                        (soc_reg_fields32_modify(unit, EMIRROR_CONTROL1r, port,
                                                 1, fields, values));
                }
#endif /* BCM_SCORPION_SUPPORT */
            }
        }

        if (SOC_MEM_IS_VALID(unit, IM_MTP_INDEXm)) {
            BCM_IF_ERROR_RETURN
                (soc_mem_clear(unit, IM_MTP_INDEXm, COPYNO_ALL, 0));
        }
        if (SOC_MEM_IS_VALID(unit, EM_MTP_INDEXm)) {
            BCM_IF_ERROR_RETURN
                (soc_mem_clear(unit, EM_MTP_INDEXm, COPYNO_ALL, 0));
        }
#if defined(BCM_TRIUMPH2_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, EP_REDIRECT_EM_MTP_INDEXm)) {
            BCM_IF_ERROR_RETURN
                (soc_mem_clear(unit, EP_REDIRECT_EM_MTP_INDEXm,
                               COPYNO_ALL, 0));
        }
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_TRX_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, EGR_ERSPANm)) {
            BCM_IF_ERROR_RETURN
                ( soc_mem_clear(unit, EGR_ERSPANm, COPYNO_ALL, 0));
        }
#endif /* BCM_TRX_SUPPORT */

        /* Mirror is disabled by default on the switch. */
        MIRROR_CONFIG_MODE(unit) = BCM_MIRROR_DISABLE;
       
    } else 
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    { 
        BCM_IF_ERROR_RETURN(_bcm_esw_mirror_mode_set(unit, BCM_MIRROR_DISABLE));
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *   	_bcm_esw_mirror_stk_update
 * Description:
 *	Stack callback to re-program path for mirror-to-port when
 *      there is an alternate path available to the unit on which 
 *      MTP is present.
 * Parameters:
 *	unit   - (IN)BCM device number
 *      modid  - (IN)Module id. 
 *      port   - (IN)
 *      pbmp   - (IN)
 * Returns:
 *	    BCM_E_XXX
 */
int
_bcm_esw_mirror_stk_update(int unit, bcm_module_t modid, bcm_port_t port,
                           bcm_pbmp_t pbmp)
{
    /* Initialization check. */
    if (!MIRROR_INIT(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if (!SOC_PORT_VALID(unit, port)) {
        return (BCM_E_PORT);
    }

    if (SOC_PBMP_IS_NULL(pbmp)) {
        return (BCM_E_NONE);
    }

#ifdef BCM_XGS_FABRIC_SUPPORT
    if (SOC_IS_HERCULES(unit) || SOC_IS_HERCULES15(unit)) {
        bcm_port_t hg_port;
        bcm_pbmp_t uc_pbmp, mir_pbmp;

        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_ucbitmap_get(unit, port, modid, &uc_pbmp));

        PBMP_HG_ITER(unit, hg_port) {
            uint32 mirbmap, old_bmap;

            BCM_IF_ERROR_RETURN (READ_ING_MIRTOBMAPr(unit, hg_port,  &mirbmap));
            old_bmap = mirbmap;
            SOC_PBMP_CLEAR(mir_pbmp);
            SOC_PBMP_WORD_SET(mir_pbmp, 0, mirbmap);

            if (SOC_PBMP_EQ(mir_pbmp, uc_pbmp)) {
                soc_reg_field_set(unit, ING_MIRTOBMAPr, &mirbmap, BMAPf,
                                  SOC_PBMP_WORD_GET(pbmp, 0));
                if (old_bmap != mirbmap) {
                    BCM_IF_ERROR_RETURN
                        (WRITE_ING_MIRTOBMAPr(unit, hg_port, mirbmap));
                }
            }
        }
    }
#endif
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_esw_mirror_port_ingress_dest_add 
 * Purpose:
 *      Add ingress mirroring destination to a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_esw_mirror_port_ingress_dest_add(int unit, bcm_port_t port,
                                      bcm_gport_t mirror_dest)
{
    int mtp_index;           /* Mirror to port index.    */
    int rv;                  /* Operation return status. */

    /* Allocate MTP index for mirror destination. */
    rv = _bcm_esw_mirror_mtp_reserve(unit, mirror_dest,
                                     BCM_MIRROR_PORT_INGRESS, &mtp_index);
    /* Check for mtp allocation failure. */
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Enable MTP index on mirror source port */
    if ((-1 != port) && SOC_PORT_VALID(unit, port)) {
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        if (SOC_IS_XGS3_SWITCH(unit)) {
            rv = _bcm_xgs3_mirror_ingress_mtp_install(unit, port, mtp_index);
        } else 
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        {
            rv = bcm_esw_mirror_ingress_set(unit, port, TRUE);
        }

        /* Check for mtp enable failure. */
        if (BCM_FAILURE(rv)) {
            _bcm_esw_mirror_mtp_unreserve(unit, mtp_index, 
                                          BCM_MIRROR_PORT_INGRESS);
        }
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_esw_mirror_port_egress_dest_add 
 * Purpose:
 *      Add egress mirroring destination to a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_esw_mirror_port_egress_dest_add(int unit, bcm_port_t port, 
                                     bcm_gport_t mirror_dest)
{
    int mtp_index;           /* Mirror to port index.    */
    int rv;                  /* Operation return status. */

    /* Allocate MTP index for mirror destination. */
    rv = _bcm_esw_mirror_mtp_reserve(unit, mirror_dest,
                                     BCM_MIRROR_PORT_EGRESS, &mtp_index);
    /* Check for mtp allocation failure. */
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Enable MTP index on mirror source port */
    if ((-1 != port) && SOC_PORT_VALID(unit, port)) {
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        if (SOC_IS_XGS3_SWITCH(unit)) {
            /* Enable MTP index on mirror source port */
            rv = _bcm_xgs3_mirror_egress_mtp_install(unit, port, mtp_index);
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        {
            rv = bcm_esw_mirror_egress_set(unit, port, TRUE);
        }

        /* Check for mtp enable failure. */
        if (BCM_FAILURE(rv)) {
            _bcm_esw_mirror_mtp_unreserve(unit, mtp_index, BCM_MIRROR_PORT_EGRESS);
        }
    }

    return (rv);
}

#if defined(BCM_TRIUMPH2_SUPPORT)
/*
 * Function:
 *     _bcm_esw_mirror_port_egress_true_dest_add 
 * Purpose:
 *      Add egress_true mirroring destination to a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_esw_mirror_port_egress_true_dest_add(int unit, bcm_port_t port, 
                                          bcm_gport_t mirror_dest)
{
    int mtp_index;           /* Mirror to port index.    */
    int rv;                  /* Operation return status. */

    /* Allocate MTP index for mirror destination. */
    rv = _bcm_esw_mirror_mtp_reserve(unit, mirror_dest,
                                     BCM_MIRROR_PORT_EGRESS_TRUE, 
                                     &mtp_index);
    /* Check for mtp allocation failure. */
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    /* Enable MTP index on mirror source port */
    if ((-1 != port) && SOC_PORT_VALID(unit, port)) {
        /* Enable MTP index on mirror source port */
        rv = _bcm_tr2_mirror_egress_true_mtp_install(unit, port, mtp_index);

        /* Check for mtp enable failure. */
        if (BCM_FAILURE(rv)) {
            _bcm_esw_mirror_mtp_unreserve(unit, mtp_index,
                                          BCM_MIRROR_PORT_EGRESS_TRUE);
        }
    }

    return (rv);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *     _bcm_esw_mirror_stacking_dest_update
 * Purpose:
 *      Update mirror_to bitmap for a system when stacking is enabled 
 * Parameters:
 *      unit         -  (IN) BCM device number.
 *      port         -  (IN) Port mirrored port.
 *      mirror_dest  -  (IN) Mirroring destination gport.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_esw_mirror_stacking_dest_update(int unit, bcm_port_t port, 
                                     bcm_gport_t mirror_dest)
{
    bcm_trunk_add_info_t tinfo; /* Destination trunk information.       */ 
    bcm_module_t mymodid;       /* Local module id.                     */
    bcm_module_t rem_modid;     /* Remote module id.                    */
    bcm_port_t port_num;        /* Port number to get to rem_modid.     */
    bcm_pbmp_t pbmp;            /* Mirror destination bitmap.           */
    uint32 mirbmap;             /* Word 0 of mirror destination bitmap. */
    int directed;               /* Draco 1.5 mirroring mode.            */
    int idx;                    /* Trunk members iterator.              */
    int isGport = 0;            /* Gport support indicator, FALSE by default */
    int is_local_modid;         /* Check for local trunk port */

    /* Check if directed mirroring is enabled. */
    BCM_IF_ERROR_RETURN(_bcm_esw_directed_mirroring_get(unit, &directed));

    /* Clear destination pbmp. */
    BCM_PBMP_CLEAR(pbmp);
    mirbmap = 0;

    /* 
     * Clear mirrorto bitmap if devices are not in draco mode 
     * or mirroring is off. 
     */ 
    if ((0 == directed) && (BCM_GPORT_INVALID != mirror_dest)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_local_modid_get(unit, &mymodid));

        if (BCM_GPORT_IS_TRUNK(mirror_dest)) {
            /* Get trunk member port/module pairs. */
            BCM_IF_ERROR_RETURN
                (bcm_esw_trunk_get(unit, BCM_GPORT_TRUNK_GET(mirror_dest),
                                   &tinfo));

             BCM_IF_ERROR_RETURN
                 (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
             if (isGport) {
                 BCM_IF_ERROR_RETURN(
                     _bcm_esw_trunk_gport_resolve(unit,
                          BCM_GPORT_TRUNK_GET(mirror_dest), &tinfo));
             }
             /* Fill pbmp with trunk members from other modules . */
             for (idx = 0; idx < tinfo.num_ports; idx++) {
                 BCM_IF_ERROR_RETURN
                     (_bcm_esw_modid_is_local(unit, tinfo.tm[idx],
                                              &is_local_modid));
                 if (!is_local_modid) {
                     BCM_IF_ERROR_RETURN
                         (bcm_esw_stk_modport_get(unit, tinfo.tm[idx],
                                                  &port_num));
                     /* Set local port used to reach remote module to pbmp. */
                     BCM_PBMP_PORT_SET(pbmp, port_num);
                 }
             }
        } else {
            rem_modid = BCM_GPORT_MODPORT_MODID_GET(mirror_dest);
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, rem_modid, &is_local_modid));
            if (!is_local_modid) {
                BCM_IF_ERROR_RETURN(bcm_esw_stk_modport_get(unit, rem_modid,
                                                            &port_num));
                /* Set local port used to reach remote module to pbmp. */
                BCM_PBMP_PORT_SET(pbmp, port_num);
            }
        }

        mirbmap = SOC_PBMP_WORD_GET(pbmp, 0);
        if (SOC_IS_FBX(unit)) {
            mirbmap >>= SOC_HG_OFFSET(unit);
        }
    }
#if defined(BCM_HERCULES_SUPPORT)
    if (SOC_IS_HERCULES(unit)) {
        return WRITE_ING_MIRTOBMAPr(unit, port, mirbmap);
    }
#endif /* BCM_HERCULES_SUPPORT */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_mirror_path)) {
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            uint32 values[2];
            soc_field_t fields[] = {BITMAP_LOf, BITMAP_HIf};
            values[0] = SOC_PBMP_WORD_GET(pbmp, 0);
            values[1] = SOC_PBMP_WORD_GET(pbmp, 1);
            BCM_IF_ERROR_RETURN
                (soc_reg_fields32_modify(unit, IMIRROR_BITMAP_64r, port,
                                         2, fields, values));
        } else
#endif /* BCM_TRIUMPH2_SUPPORT */
        {
            return WRITE_IMIRROR_BITMAPr(unit, port, mirbmap);
        } 
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_esw_mirror_port_ingress_dest_delete
 * Purpose:
 *      Delete ingress mirroring destination from a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_esw_mirror_port_ingress_dest_delete(int unit, bcm_port_t port, 
                                         bcm_gport_t mirror_dest) 
{
    int enable;              /* Mirror enable check.     */
    int mtp_index;           /* Mirror to port index.    */
    int rv;                  /* Operation return status. */

    /* Look for used MTP index */
    rv = _bcm_esw_mirror_ingress_mtp_match(unit, mirror_dest, &mtp_index);
    if (BCM_FAILURE(rv)) {
        return (BCM_E_NOT_FOUND);
    }

    /* Disable  MTP index on mirror source port */
    if ((-1 != port) && SOC_PORT_VALID(unit, port)) {
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        if (SOC_IS_XGS3_SWITCH(unit)) {
            /* Enable MTP index on mirror source port */
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_mirror_ingress_mtp_uninstall(unit, port, mtp_index));
        } else 
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        {

            BCM_IF_ERROR_RETURN(bcm_esw_mirror_ingress_get(unit, port, &enable));
            if (!enable) {
                return (BCM_E_NONE);
            }
            BCM_IF_ERROR_RETURN(bcm_esw_mirror_ingress_set(unit, port, FALSE));
        }
    }

    /* Free MTP index for mirror destination. */
    rv =  _bcm_esw_mirror_mtp_unreserve(unit, mtp_index, BCM_MIRROR_PORT_INGRESS);

    return (rv);
}

/*
 * Function:
 *     _bcm_esw_mirror_port_egress_dest_delete
 * Purpose:
 *      Delete egress mirroring destination from a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_esw_mirror_port_egress_dest_delete(int unit, bcm_port_t port, 
                                        bcm_gport_t mirror_dest) 
{
    int enable;              /* Mirror enable check.     */
    int mtp_index;           /* Mirror to port index.    */
    int rv;                  /* Operation return status. */

    /* Look for used MTP index */
    rv = _bcm_esw_mirror_egress_mtp_match(unit, mirror_dest, &mtp_index);
    if (BCM_FAILURE(rv)) {
        return (BCM_E_NOT_FOUND);
    }

    /* Disable  MTP index on mirror source port */
    if ((-1 != port) && SOC_PORT_VALID(unit, port)) {
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        if (SOC_IS_XGS3_SWITCH(unit)) {
            /* Enable MTP index on mirror source port */
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_mirror_egress_mtp_uninstall(unit, port, mtp_index));
        } else 
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        {
            BCM_IF_ERROR_RETURN(bcm_esw_mirror_egress_get(unit, port, &enable));
            if (!enable) {
                return (BCM_E_NONE);
            }
            BCM_IF_ERROR_RETURN(bcm_esw_mirror_egress_set(unit, port, FALSE));
        }
    }

    /* Free MTP index for mirror destination. */
    rv =  _bcm_esw_mirror_mtp_unreserve(unit, mtp_index, BCM_MIRROR_PORT_EGRESS);

    return (rv);
}

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *     _bcm_esw_mirror_port_egress_true_dest_delete
 * Purpose:
 *      Delete egress true mirroring destination from a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_esw_mirror_port_egress_true_dest_delete(int unit, bcm_port_t port, 
                                        bcm_gport_t mirror_dest) 
{
    int mtp_index;           /* Mirror to port index.    */
    int rv;                  /* Operation return status. */

    /* Look for used MTP index */
    rv = _bcm_esw_mirror_egress_true_mtp_match(unit, mirror_dest,
                                               &mtp_index);
    if (BCM_FAILURE(rv)) {
        return (BCM_E_NOT_FOUND);
    }

    /* Disable MTP index on mirror source port */
    if ((-1 != port) && SOC_PORT_VALID(unit, port)) {
        /* Enable MTP index on mirror source port */
        BCM_IF_ERROR_RETURN
            (_bcm_tr2_mirror_egress_true_mtp_uninstall(unit, port,
                                                        mtp_index));
    }

    /* Free MTP index for mirror destination. */
    rv =  _bcm_esw_mirror_mtp_unreserve(unit, mtp_index,
                                        BCM_MIRROR_PORT_EGRESS_TRUE);

    return (rv);
}
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_FIELD_SUPPORT) && defined(BCM_XGS3_SWITCH_SUPPORT)
#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *     _bcm_xgs3_mtp_type_slot_reserve
 * Purpose:
 *      Record used MTP slots for FP/IPFIX usage. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      flags        -  (IN) BCM_MIRROR_PORT_XXX flags.
 *      mtp_type     -  (IN) FP/IPFIX.
 *      mtp_index    -  (IN) Allocated hw mtp index.
 *      mtp_slot_p   -  (OUT) MTP slot in which to install the MTP index.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_xgs3_mtp_type_slot_reserve(int unit, uint32 flags, int mtp_type,
                                int mtp_index, int *mtp_slot_p)
{
    uint32 ms_reg;             /* MTP mode register value     */
    int mtp_slot, mtp_bit, free_ptr = -1, free_slot = -1;
    int egress = FALSE;

    if (flags & BCM_MIRROR_PORT_EGRESS) {
        egress = TRUE;
    }

    for (mtp_slot = 0, mtp_bit = BCM_MIRROR_MTP_ONE;
         (mtp_bit & MIRROR_CONFIG_MTP_DEV_MASK(unit)) != 0;
         mtp_bit <<= 1, mtp_slot++) {
        if (egress) {
            if (!(MIRROR_CONFIG_MTP_MODE_BMP(unit) & mtp_bit)) {
                if (MIRROR_CONFIG_MTP_MODE_REF_COUNT(unit,
                                                     mtp_slot) == 0) {
                    /* MTP Container is undecided, note for later */
                    if (free_ptr < 0) {
                        /* Record unallocated MTP container */
                        free_ptr = mtp_slot;
                    }
                } /* Else, container already used for ingress mirrors */
                continue;
            }
        } else {
            if (MIRROR_CONFIG_MTP_MODE_BMP(unit) & mtp_bit) {
                /* Slot configured for egress mirroring, skip */
                continue;
            }
        }
        if (MIRROR_CONFIG_TYPE_MTP_REF_COUNT(unit, mtp_slot, mtp_type)) {
            if (MIRROR_CONFIG_TYPE_MTP_SLOT(unit, mtp_slot,
                                            mtp_type) == mtp_index) {
                /* Match - return mtp_slot */
                *mtp_slot_p = mtp_slot;
                MIRROR_CONFIG_TYPE_MTP_REF_COUNT(unit, mtp_slot, mtp_type)++;
                return BCM_E_NONE;
            }
        } else { /* Slot unused on this port */
            if (free_slot < 0) {
                /* Record free slot */
                free_slot = mtp_slot;
            }
        }
    }

    /* Use previous allocated slot if available. Otherwise use unallocated
     * MTP continaner.  If neither, we're out of resources. */
    if (free_slot < 0) {
        if (free_ptr < 0) {
            return BCM_E_FULL;
        } else {
            free_slot = free_ptr;
        }
    }

    mtp_slot = free_slot;
    mtp_bit = 1 << free_slot;

    /* Record references and new MTP mode allocation if necessary */
    if (egress && !(MIRROR_CONFIG_MTP_MODE_BMP(unit) & mtp_bit)) {
        /* Update MTP_MODE */
        MIRROR_CONFIG_MTP_MODE_BMP(unit) |= mtp_bit;

        BCM_IF_ERROR_RETURN(READ_MIRROR_SELECTr(unit, &ms_reg));
        soc_reg_field_set(unit, MIRROR_SELECTr, &ms_reg, MTP_TYPEf,
                          MIRROR_CONFIG_MTP_MODE_BMP(unit));
        BCM_IF_ERROR_RETURN(WRITE_MIRROR_SELECTr(unit, ms_reg));
        BCM_IF_ERROR_RETURN(WRITE_EGR_MIRROR_SELECTr(unit, ms_reg));
    }
    MIRROR_CONFIG_MTP_MODE_REF_COUNT(unit, mtp_slot)++;

    MIRROR_CONFIG_TYPE_MTP_SLOT(unit, mtp_slot, mtp_type) = mtp_index;
    MIRROR_CONFIG_TYPE_MTP_REF_COUNT(unit, mtp_slot, mtp_type)++;
    *mtp_slot_p = mtp_slot;

    return BCM_E_NONE;
}     
#endif /* BCM_TRIUMPH2_SUPPORT */

/*
 * Function:
 *     _bcm_esw_mirror_fp_dest_add 
 * Purpose:
 *      Add mirroring destination to field processor module. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      modid        -  (IN) Mirroring destination module.
 *      port         -  (IN) Mirroring destination port or GPORT. 
 *      flags        -  (IN) BCM_MIRROR_PORT_XXX flags.
 *      mtp_index    -  (OUT) Allocated hw mtp index.
 * Returns:
 *      BCM_X_XXX
 */
int
_bcm_esw_mirror_fp_dest_add(int unit, int modid, int port, 
                            uint32 flags, int *mtp_index) 
{
    int             directed;        /* Directed mirroring indicator.*/
    bcm_mirror_destination_t mirror_dest;  /* Mirror destination.          */
    bcm_gport_t     mirror_dest_id;  /* Mirror destination id.       */
    int             rv = BCM_E_NONE; /* Operation return status.     */
    uint32          destroy_flag = FALSE; /* mirror destination destroy */
#ifdef BCM_TRIUMPH2_SUPPORT
    int   mtp_slot, mtp_slot_type = BCM_MTP_SLOT_TYPE_FP;          
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* At least one packet direction must be specified. */
    if (!(flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS |
                   BCM_MIRROR_PORT_EGRESS_TRUE))) {
        return (BCM_E_PARAM);
    }

    /* Can't reserve multiple types of mtp in 1 shot. */
    if (((flags & BCM_MIRROR_PORT_INGRESS) &&
        (flags & (BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_TRUE))) ||
        ((flags & BCM_MIRROR_PORT_EGRESS) &&
         (flags & BCM_MIRROR_PORT_EGRESS_TRUE))) {
        return (BCM_E_PARAM);
    }

    if (!soc_feature(unit, soc_feature_egr_mirror_true) &&
        (flags & BCM_MIRROR_PORT_EGRESS_TRUE)) {
        return (BCM_E_PARAM);
    }

    /* Initialization check */
    if (!MIRROR_INIT(unit)) {
        return (BCM_E_INIT);
    }

    /* Create traditional mirror destination. */
    bcm_mirror_destination_t_init(&mirror_dest);

    MIRROR_LOCK(unit);

    /* Check if directed mirroring is enabled. */
    rv = _bcm_esw_directed_mirroring_get(unit, &directed);
    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        return (rv);
    }
    if (BCM_GPORT_IS_MIRROR(port)) {
        rv = bcm_esw_mirror_destination_get(unit, port, &mirror_dest);
    } else {
        rv = _bcm_esw_mirror_destination_find(unit, port, modid, flags, &mirror_dest); 
        if (BCM_E_NOT_FOUND == rv) {
            mirror_dest.flags |= _BCM_MIRROR_DESTINATION_LOCAL;
            rv = _bcm_esw_mirror_destination_create(unit, &mirror_dest);
            destroy_flag = TRUE;
        }       
    }
    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        return rv;
    }
    mirror_dest_id = mirror_dest.mirror_dest_id;
    /* Single mirroring destination for ingress & egress. */
    if (!directed) {
        if (BCM_GPORT_IS_TRUNK(mirror_dest.gport)) {
            if (destroy_flag) {
               (void)bcm_esw_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id); 
            }
            MIRROR_UNLOCK(unit);
            return (BCM_E_UNAVAIL);
        }

        if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)) {
            if ((MIRROR_CONFIG_ING_MTP_DEST(unit, 0) != mirror_dest_id) ||
                (MIRROR_CONFIG_EGR_MTP_DEST(unit, 0) != mirror_dest_id)) {
                if (destroy_flag) {
                   (void)bcm_esw_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id); 
                }
                MIRROR_UNLOCK(unit);
                return (BCM_E_RESOURCE);
            }
        }
    }
     

    /* Reserve & initialize mtp index based on traffic direction. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        rv = _bcm_xgs3_mirror_ingress_mtp_reserve(unit, mirror_dest_id, 
                                                  mtp_index);
    } else if (flags & BCM_MIRROR_PORT_EGRESS) {
        rv = _bcm_xgs3_mirror_egress_mtp_reserve(unit, mirror_dest_id,
                                                 mtp_index);
#ifdef BCM_TRIUMPH2_SUPPORT
    } else if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
        rv = _bcm_xgs3_mirror_egress_true_mtp_reserve(unit, mirror_dest_id,
                                                      mtp_index);
        mtp_slot_type = BCM_MTP_SLOT_TYPES;
#endif /* BCM_TRIUMPH2_SUPPORT */
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        if (mtp_slot_type < BCM_MTP_SLOT_TYPES) {
            /* Record-keeping for used slots of mtp_index for FP */
            rv = _bcm_xgs3_mtp_type_slot_reserve(unit, flags, mtp_slot_type,
                                                 *mtp_index, &mtp_slot);
            if (BCM_SUCCESS(rv)) {
                *mtp_index |= (mtp_slot << BCM_MIRROR_MTP_FLEX_SLOT_SHIFT);
            }    
        } else {
            /* Egress true uses 1-1 mapping */
            *mtp_index |= (*mtp_index << BCM_MIRROR_MTP_FLEX_SLOT_SHIFT);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* Enable mirroring on a port.  */
    if (BCM_SUCCESS(rv)) { 
        if(!SOC_IS_XGS3_SWITCH(unit) || 
           (BCM_MIRROR_DISABLE == MIRROR_CONFIG_MODE(unit))) {
            rv = _bcm_esw_mirror_enable(unit);
            MIRROR_CONFIG_MODE(unit) = BCM_MIRROR_L2;
        }
    } 

    if (BCM_FAILURE(rv) && destroy_flag) {
        (void)bcm_esw_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id); 
    }
    MIRROR_UNLOCK(unit);
    return (rv);
}

#ifdef BCM_TRIUMPH2_SUPPORT
/*
 * Function:
 *     _bcm_xgs3_mtp_type_slot_unreserve
 * Purpose:
 *      Clear a used MTP slot for FP/IPFIX usage. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      flags        -  (IN) BCM_MIRROR_PORT_XXX flags.
 *      mtp_type     -  (IN) FP/IPFIX.
 *      mtp_index    -  (IN) Allocated hw mtp index.
 * Returns:
 *      BCM_X_XXX
 */
STATIC int
_bcm_xgs3_mtp_type_slot_unreserve(int unit, uint32 flags, int mtp_type,
                                  int mtp_index)
{
    uint32 ms_reg;             /* MTP mode register value     */
    int mtp_slot, mtp_bit;
    int egress = FALSE;

    if (flags & BCM_MIRROR_PORT_EGRESS) {
        egress = TRUE;
    }

    for (mtp_slot = 0, mtp_bit = BCM_MIRROR_MTP_ONE;
         (mtp_bit & MIRROR_CONFIG_MTP_DEV_MASK(unit)) != 0;
         mtp_bit <<= 1, mtp_slot++) {
        if (egress) {
            if (!(MIRROR_CONFIG_MTP_MODE_BMP(unit) & mtp_bit)) {
                continue;
            }
        } else {
            if (MIRROR_CONFIG_MTP_MODE_BMP(unit) & mtp_bit) {
                continue;
            }
        }
        if (MIRROR_CONFIG_TYPE_MTP_REF_COUNT(unit, mtp_slot, mtp_type)) {
            if (MIRROR_CONFIG_TYPE_MTP_SLOT(unit, mtp_slot,
                                            mtp_type) == mtp_index) {
                /* Found! */
                if (MIRROR_DEST_REF_COUNT(unit, mtp_slot) > 0) {
                    MIRROR_CONFIG_MTP_MODE_REF_COUNT(unit, mtp_slot)--;
                }
                if (egress &&
                    !(MIRROR_CONFIG_MTP_MODE_REF_COUNT(unit,
                                                       mtp_slot))) {
                    /* Free MTP_MODE */
                    MIRROR_CONFIG_MTP_MODE_BMP(unit) &= ~mtp_bit;

                    BCM_IF_ERROR_RETURN
                        (READ_MIRROR_SELECTr(unit, &ms_reg));
                    soc_reg_field_set(unit, MIRROR_SELECTr, &ms_reg,
                                      MTP_TYPEf,
                                      MIRROR_CONFIG_MTP_MODE_BMP(unit));
                    BCM_IF_ERROR_RETURN
                        (WRITE_MIRROR_SELECTr(unit, ms_reg));
                    BCM_IF_ERROR_RETURN
                        (WRITE_EGR_MIRROR_SELECTr(unit, ms_reg));
                }

                MIRROR_CONFIG_TYPE_MTP_SLOT(unit, mtp_slot, mtp_type) = 0;
                if (MIRROR_CONFIG_TYPE_MTP_REF_COUNT(unit, mtp_slot, mtp_type) > 0) { 
                    MIRROR_CONFIG_TYPE_MTP_REF_COUNT(unit, mtp_slot, mtp_type)--;
                }
            }
        }
    }
    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH2_SUPPORT */  

/*
 * Function:
 *     _bcm_esw_mirror_fp_dest_delete
 * Purpose:
 *      Delete fp mirroring destination.
 * Parameters:
 *      unit         -  (IN) BCM device number.
 *      mtp_index    -  (IN) Mirror destination index.
 *      flags        -  (IN) Mirror direction flags.
 * Returns:
 *      BCM_X_XXX
 * Notes: 
 */
int
_bcm_esw_mirror_fp_dest_delete(int unit, int mtp_index, uint32 flags)
{
    int rv = BCM_E_NONE;                      /* Operation return status. */
    bcm_mirror_destination_t mirror_dest;     /* Mirror destination.       */
    bcm_gport_t              mirror_dest_id;  /* Mirror destination id.    */
#ifdef BCM_TRIUMPH2_SUPPORT
    int   mtp_slot_type = BCM_MTP_SLOT_TYPE_FP;          
#endif /* BCM_TRIUMPH2_SUPPORT */ 

    /* Input parameters check. */
    /* At least one packet direction must be specified. */
    if (!(flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS |
                   BCM_MIRROR_PORT_EGRESS_TRUE))) {
        return (BCM_E_PARAM);
    }

    /* Can't reserve multiple types of mtp in 1 shot. */
    if (((flags & BCM_MIRROR_PORT_INGRESS) &&
        (flags & (BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_TRUE))) ||
        ((flags & BCM_MIRROR_PORT_EGRESS) &&
         (flags & BCM_MIRROR_PORT_EGRESS_TRUE))) {
        return (BCM_E_PARAM);
    }

    if (!soc_feature(unit, soc_feature_egr_mirror_true) &&
        (flags & BCM_MIRROR_PORT_EGRESS_TRUE)) {
        return (BCM_E_PARAM);
    }

    MIRROR_LOCK(unit);

    if (flags & BCM_MIRROR_PORT_EGRESS) {
        mirror_dest_id = MIRROR_CONFIG_EGR_MTP_DEST(unit, mtp_index);
#ifdef BCM_TRIUMPH2_SUPPORT
    } else if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
        mirror_dest_id = MIRROR_CONFIG_EGR_TRUE_MTP_DEST(unit, mtp_index);
        mtp_slot_type = BCM_MTP_SLOT_TYPES;
#endif /* BCM_TRIUMPH2_SUPPORT */  
    }  else {
        mirror_dest_id = MIRROR_CONFIG_ING_MTP_DEST(unit, mtp_index);
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mirror_flexible) && 
        (mtp_slot_type < BCM_MTP_SLOT_TYPES)) {
        rv = _bcm_xgs3_mtp_type_slot_unreserve(unit, flags, mtp_slot_type,
                                               mtp_index);
    }
#endif /* BCM_TRIUMPH2_SUPPORT */  

    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        return (rv);
    }

    /* Get mirror destination descriptor. */
    rv = bcm_esw_mirror_destination_get(unit, mirror_dest_id, &mirror_dest);
    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        return (rv);
    }

    /* Free MTP index for mirror destination. */
    rv = _bcm_esw_mirror_mtp_unreserve(unit, mtp_index, flags);
    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        return (rv);
    }

    /* Destroy mirror destination if it was created by fp action add. */ 
    if ((mirror_dest.flags & _BCM_MIRROR_DESTINATION_LOCAL) && 
        (1 >= MIRROR_DEST_REF_COUNT(unit, mirror_dest.mirror_dest_id))) {
        rv = bcm_esw_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
    }
    MIRROR_UNLOCK(unit);
    return(rv);
}

#endif /* BCM_FIELD_SUPPORT  && BCM_XGS3_SWITCH_SUPPORT */

/*
 * Function:
 *	    _bcm_esw_mirror_enable
 * Purpose:
 *	    Set mirror enable = TRUE on all ports.
 * Parameters:
 *	    unit - (IN) BCM device number
 * Returns:
 *   	BCM_E_XXX
 * Notes:
 *      When egress or fp mirroring is enabled, we need to enable 
 *      mirroring on all ports even if mirroring is only explicitely
 *      enabled on a single port. This function ensures that the mirror
 *      enable bit is toggled correctly on all ports, when
 *      bcm_mirror_port_dest_xxx style apis are used by application.
 */
STATIC int
_bcm_esw_mirror_enable(int unit)
{
    bcm_port_t port;
    PBMP_ALL_ITER(unit, port) {
        BCM_IF_ERROR_RETURN(_bcm_esw_mirror_enable_set(unit, port, TRUE));
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *	  bcm_esw_mirror_deinit
 * Purpose:
 *	  Deinitialize mirror software module.
 * Parameters:
 *    unit - (IN) BCM device number.
 * Returns:
 *	  BCM_E_XXX
 */
int
bcm_esw_mirror_deinit(int unit)
{
    /* Call internal sw structures clean up routine. */
    return _bcm_esw_mirror_deinit(unit, &MIRROR_CONFIG(unit));
}

/*
 * Function:
 *	  bcm_esw_mirror_init
 * Purpose:
 *	  Initialize mirror software system.
 * Parameters:
 *    unit - (IN) BCM device number.
 * Returns:
 *	  BCM_E_XXX
 */
int
bcm_esw_mirror_init(int unit)
{
    _bcm_mirror_config_p mirror_cfg_ptr;/* Mirror module config structue. */
    bcm_mirror_destination_t *mdest;    /* Mirror destinations iterator.  */
    int directed;                       /* Directed mirroring enable.     */
    int alloc_sz;                       /* Memory allocation size.        */
    int idx;                            /* MTP iteration index.           */
    int rv;                             /* Operation return status.       */

    /* Deinitialize the module if it was previously initialized.*/
    if (NULL != MIRROR_CONFIG(unit)) {
        _bcm_esw_mirror_deinit(unit, &MIRROR_CONFIG(unit));
    }

    /* Allocate meter config structure. */
    alloc_sz = sizeof(_bcm_mirror_config_t);
    mirror_cfg_ptr = sal_alloc(alloc_sz, "Meter module");
    if (NULL == mirror_cfg_ptr) {
        return (BCM_E_MEMORY);
    }
    sal_memset(mirror_cfg_ptr, 0, alloc_sz);

    rv = _bcm_esw_directed_mirroring_get(unit, &directed);
    if (BCM_FAILURE(rv)) {
        _bcm_esw_mirror_deinit(unit, &mirror_cfg_ptr);
        return(BCM_E_INTERNAL); 
    }

    if (directed) {
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_egr_mirror_true)) {
            mirror_cfg_ptr->egr_true_mtp_count = 4;
        }
        if (soc_feature(unit, soc_feature_mirror_flexible)) {
            mirror_cfg_ptr->egr_mtp_count = 4;
            mirror_cfg_ptr->ing_mtp_count = 4;
        } else 
#endif /* BCM_TRIUMPH2_SUPPORT */
        {
        mirror_cfg_ptr->egr_mtp_count = SOC_IS_TRX(unit) ? 2 : 4;
        mirror_cfg_ptr->ing_mtp_count = 4;
        }
    }  else {
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_egr_mirror_true)) {
            mirror_cfg_ptr->egr_true_mtp_count = 1;
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
        mirror_cfg_ptr->egr_mtp_count = 1;
        mirror_cfg_ptr->ing_mtp_count = 1;
    }

    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        mirror_cfg_ptr->mtp_dev_mask = BCM_TR2_MIRROR_MTP_MASK;
    } else if (SOC_IS_TRX(unit)) {
        mirror_cfg_ptr->mtp_dev_mask = BCM_TRX_MIRROR_MTP_MASK;
    } else {
        /* Pre-XGS3 devices don't use this value */
        mirror_cfg_ptr->mtp_dev_mask = BCM_XGS3_MIRROR_MTP_MASK;
    }

    /* Allocate mirror destinations structure. */
    mirror_cfg_ptr->dest_count = 
        mirror_cfg_ptr->egr_mtp_count + mirror_cfg_ptr->ing_mtp_count;
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_egr_mirror_true)) {
        mirror_cfg_ptr->dest_count += mirror_cfg_ptr->egr_true_mtp_count;
    }
#endif /* BCM_TRIUMPH2_SUPPORT */
    alloc_sz = 
        mirror_cfg_ptr->dest_count * sizeof(_bcm_mirror_dest_config_t);

    mirror_cfg_ptr->dest_arr = sal_alloc(alloc_sz, "Mirror destinations");
    if (NULL == mirror_cfg_ptr->dest_arr) {
        _bcm_esw_mirror_deinit(unit, &mirror_cfg_ptr);
        return (BCM_E_MEMORY);
    }
    sal_memset(mirror_cfg_ptr->dest_arr, 0, alloc_sz);
    for (idx = 0; idx < mirror_cfg_ptr->dest_count; idx++) {
        mdest = &mirror_cfg_ptr->dest_arr[idx].mirror_dest;
        BCM_GPORT_MIRROR_SET(mdest->mirror_dest_id, idx);
    }

    /* Allocate egress mirror destinations structure. */
    alloc_sz = mirror_cfg_ptr->egr_mtp_count * sizeof(_bcm_mtp_config_t);
    mirror_cfg_ptr->egr_mtp  = sal_alloc(alloc_sz, "Egress MTP indexes");
    if (NULL == mirror_cfg_ptr->egr_mtp) {
        _bcm_esw_mirror_deinit(unit, &mirror_cfg_ptr);
        return (BCM_E_MEMORY);
    }
    sal_memset(mirror_cfg_ptr->egr_mtp, 0, alloc_sz);

    /* Allocate ingress mirror destinations structure. */
    alloc_sz = mirror_cfg_ptr->ing_mtp_count * sizeof(_bcm_mtp_config_t);
    mirror_cfg_ptr->ing_mtp  = sal_alloc(alloc_sz, "Ingress MTP indexes");
    if (NULL == mirror_cfg_ptr->ing_mtp) {
        _bcm_esw_mirror_deinit(unit, &mirror_cfg_ptr);
        return (BCM_E_MEMORY);
    }
    sal_memset(mirror_cfg_ptr->ing_mtp, 0, alloc_sz);

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_egr_mirror_true)) {
        /* Allocate egress true mirror destinations structure. */
        alloc_sz =
            mirror_cfg_ptr->egr_true_mtp_count * sizeof(_bcm_mtp_config_t);
        mirror_cfg_ptr->egr_true_mtp  = sal_alloc(alloc_sz,
                                                  "Egress true MTP indexes");
        if (NULL == mirror_cfg_ptr->egr_true_mtp) {
            _bcm_esw_mirror_deinit(unit, &mirror_cfg_ptr);
            return (BCM_E_MEMORY);
        }
        sal_memset(mirror_cfg_ptr->egr_true_mtp, 0, alloc_sz);
    }

    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        int mtp_type;
        for (mtp_type = BCM_MTP_SLOT_TYPE_FP;
             mtp_type < BCM_MTP_SLOT_TYPES; mtp_type++) {
            /* Allocate MTP types records. */
            mirror_cfg_ptr->mtp_slot_count[mtp_type] = 4;

            alloc_sz = mirror_cfg_ptr->mtp_slot_count[mtp_type] *
                sizeof(_bcm_mtp_config_t);
            mirror_cfg_ptr->mtp_slot[mtp_type]  = sal_alloc(alloc_sz,
                                                      "Typed MTP indexes");
            if (NULL == mirror_cfg_ptr->mtp_slot[mtp_type]) {
                _bcm_esw_mirror_deinit(unit, &mirror_cfg_ptr);
                return (BCM_E_MEMORY);
            }
            sal_memset(mirror_cfg_ptr->mtp_slot[mtp_type], 0, alloc_sz);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* Create protection mutex. */
    mirror_cfg_ptr->mutex = sal_mutex_create("Meter module mutex");
    if (NULL == mirror_cfg_ptr->mutex) {
        _bcm_esw_mirror_deinit(unit, &mirror_cfg_ptr);
        return (BCM_E_MEMORY);
    } 

    /* Take protection mutex for initial state setting & hw clear. */
    sal_mutex_take(mirror_cfg_ptr->mutex, sal_mutex_FOREVER);

    MIRROR_CONFIG(unit) = mirror_cfg_ptr;

    /* Clear memories/registers. */
    rv  = _bcm_esw_mirror_hw_clear(unit);
    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        _bcm_esw_mirror_deinit(unit, &mirror_cfg_ptr);
        MIRROR_CONFIG(unit) = NULL;
        return (BCM_E_FAIL);
    }

    MIRROR_UNLOCK(unit);
    return (BCM_E_NONE);
}



/*
 * Function:
 *	  bcm_esw_mirror_mode_set
 * Description:
 *	  Enable or disable mirroring.  Will wait for bcm_esw_mirror_to_set
 *        to be called to actually do the enable if needed.
 * Parameters:
 *        unit - (IN) BCM device number
 *	  mode - (IN) One of BCM_MIRROR_(DISABLE|L2|L2_L3)
 * Returns:
 *	  BCM_E_XXX
 */
int
bcm_esw_mirror_mode_set(int unit, int mode)
{
    int      rv = BCM_E_UNAVAIL;   /* Operation return status.    */

    /* Initialization check */
    if (0 == MIRROR_INIT(unit)) {
        return (BCM_E_INIT);
    }

    /* Input parameters check. */
    if ((BCM_MIRROR_L2 != mode) && 
        (BCM_MIRROR_L2_L3 != mode) && 
        (BCM_MIRROR_DISABLE != mode)) {
          return (BCM_E_PARAM);
    }

    MIRROR_LOCK(unit);
    rv = _bcm_esw_mirror_mode_set(unit, mode);
    MIRROR_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *   	bcm_esw_mirror_mode_get
 * Description:
 *	    Get mirror mode. (L2/L2_L3/DISABLED).
 * Parameters:
 *	    unit - BCM device number
 *	    mode - (OUT) One of BCM_MIRROR_(DISABLE|L2|L2_L3)
 * Returns:
 *	    BCM_E_XXX
 */
int
bcm_esw_mirror_mode_get(int unit, int *mode)
{
    /* Initialization check */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if (NULL == mode) {
        return BCM_E_PARAM;
    }
    MIRROR_LOCK(unit);
    *mode = MIRROR_CONFIG_MODE(unit);
    MIRROR_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *	   bcm_esw_mirror_to_set
 * Description:
 *	   Set the mirror-to port for all mirroring, enabling mirroring
 *	   if a mode has previously been set.
 * Parameters:
 *	   unit - (IN) BCM device number
 *	   port - (IN) The port to mirror all ingress/egress selections to
 * Returns:
 *	   BCM_E_XXX
 * Notes:
 *     When mirroring to a remote unit, the mirror-to port
 *     should be the appropriate stack port on the local unit.
 *     This will return BCM_E_CONFIG if the unit is configured for,
 *     or only supports directed mirroring.
 */
int
bcm_esw_mirror_to_set(int unit, bcm_port_t port)
{
    bcm_mirror_destination_t mirror_dest;    /* Destination port/trunk.       */
    int directed;                            /* Directed mirroring indicator. */
    int rv;                                  /* Operation return status.      */
    int mod_out, port_out;                   /* Module and port for mapping   */
    bcm_gport_t gport;                       /* Local gport operations        */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    gport = port;
    if (BCM_GPORT_IS_SET(gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport, &port));
    }
    /* Input parameters check. */
    if (!SOC_PORT_VALID(unit, port)) {
        return (BCM_E_PORT);
    }

    bcm_mirror_destination_t_init(&mirror_dest);

    /* Check if directed mirroring is enabled. */
    BCM_IF_ERROR_RETURN(_bcm_esw_directed_mirroring_get(unit, &directed));
    
    if (directed) {
        return (BCM_E_CONFIG);
    }

    if (BCM_GPORT_IS_SET(gport)) {
        mirror_dest.gport = gport;
    } else {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_gport_get(unit, port, &gport));
        BCM_IF_ERROR_RETURN(
            bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                   SOC_GPORT_MODPORT_MODID_GET(gport),
                                   SOC_GPORT_MODPORT_PORT_GET(gport),
                                   &mod_out, &port_out));
        BCM_IF_ERROR_RETURN(
            _bcm_mirror_gport_construct(unit, port_out, mod_out, 0, 
                                        &(mirror_dest.gport)));
    }

    /* Create traditional mirror destination. */
    BCM_GPORT_MIRROR_SET(mirror_dest.mirror_dest_id, 0);
    mirror_dest.flags = BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_REPLACE;
    
    MIRROR_LOCK(unit);

    rv = bcm_esw_mirror_destination_create(unit, &mirror_dest);
    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        return (rv);
    }

    MIRROR_CONFIG_ING_MTP_DEST(unit, 0) = mirror_dest.mirror_dest_id;
    MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0) = 1;

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        /* Ingress & Egress configuration is identical. */
        MIRROR_CONFIG_EGR_MTP(unit, 0) = MIRROR_CONFIG_ING_MTP(unit, 0);

        /* Write MTP registers */
        rv = _bcm_xgs3_mtp_init(unit, 0, (BCM_MIRROR_PORT_EGRESS |
                                          BCM_MIRROR_PORT_INGRESS));
        if (BCM_FAILURE(rv)) {
            MIRROR_UNLOCK(unit);
            return (rv);
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    rv = bcm_esw_mirror_mode_set(unit, MIRROR_CONFIG_MODE(unit));
    MIRROR_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *	bcm_esw_mirror_to_get
 * Description:
 *	Get the mirror-to port for all mirroring
 * Parameters:
 *	unit - (IN)  BCM device number
 *	port - (OUT) The port to mirror all ingress/egress selections to
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_esw_mirror_to_get(int unit, bcm_port_t *port)
{
    uint32          flags;      /* Mirror destination flags. */
    int             rv;         /* Operation return status  */
    int             isGport;    /* Indicator on which format to return port */
    bcm_module_t    mymodid, modid;    /* module id to construct a gport */
    bcm_gport_t     gport;
    int             mod_out, port_out; /* To do a modmap mapping */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if (NULL == port) {
        return (BCM_E_PARAM);
    }

    flags = 0;
    BCM_IF_ERROR_RETURN(
        bcm_esw_stk_my_modid_get(unit, &mymodid));

    MIRROR_LOCK(unit);

    if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)) {
         rv = _bcm_mirror_destination_gport_parse(unit,
                            MIRROR_CONFIG_ING_MTP_DEST(unit, 0),
                            &modid, port, &flags);
    } else {
        *port = -1;
        modid = mymodid;
        rv = BCM_E_NONE;
    }
    MIRROR_UNLOCK(unit);
    if (BCM_FAILURE(rv)) {
        return (rv);
    }

    if (flags & BCM_MIRROR_PORT_DEST_TRUNK) { 
        return (BCM_E_CONFIG);
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
    if (isGport) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mymodid, *port, 
                                   &mod_out, &port_out));
        BCM_IF_ERROR_RETURN
            (_bcm_mirror_gport_construct(unit, port_out, mod_out,
                                         flags, port)); 
    } else if (*port != -1) {
        BCM_GPORT_MODPORT_SET(gport, modid, *port);
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_local_get(unit, gport, port));
    }
    
    return (BCM_E_NONE);
}

/*
 * Function:
 *   	bcm_esw_mirror_ingress_set
 * Description:
 *	    Enable or disable mirroring per ingress
 * Parameters:
 *   	unit   - (IN) BCM device number
 *	    port   - (IN) The port to affect
 *   	enable - (IN) Boolean value for on/off
 * Returns:
 *	    BCM_E_XXX
 * Notes:
 *	    Mirroring must also be globally enabled.
 */
int
bcm_esw_mirror_ingress_set(int unit, bcm_port_t port, int enable)
{
    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    /* Input parameters check. */
    if (!SOC_PORT_VALID(unit, port)) {
        return (BCM_E_PORT);
    }

    if (IS_CPU_PORT(unit, port) && 
        !soc_feature(unit, soc_feature_cpuport_mirror)) {
        return (BCM_E_PORT);
    }

    /* Set ingress mirroring enable in port table. */
    return _bcm_port_mirror_enable_set(unit, port, 
                                       ((enable) ?  BCM_MIRROR_MTP_ONE : (0)));

}

/*
 * Function:
 * 	    bcm_esw_mirror_ingress_get
 * Description:
 * 	    Get the mirroring per ingress enabled/disabled status
 * Parameters:
 *	    unit   - (IN)  BCM device number
 *   	port   - (IN)  The port to check
 *	    enable - (OUT) Place to store boolean return value for on/off
 * Returns:
 *	    BCM_E_XXX
 */
int
bcm_esw_mirror_ingress_get(int unit, bcm_port_t port, int *enable)
{
    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if (NULL == enable) {
        return (BCM_E_PARAM);
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return (BCM_E_PORT);
    }

    if (IS_CPU_PORT(unit, port) &&
        !soc_feature(unit, soc_feature_cpuport_mirror)) {
        return (BCM_E_PORT);
    }

    /* Get ingress mirroring enable from  port table. */
    return _bcm_port_mirror_enable_get(unit, port, enable);
}

/*
 * Function:
 * 	   bcm_esw_mirror_egress_set
 * Description:
 *  	Enable or disable mirroring per egress
 * Parameters:
 *  	unit   - (IN) BCM device number
 *	    port   - (IN) The port to affect
 *	    enable - (IN) Boolean value for on/off
 * Returns:
 *	    BCM_E_XXX
 * Notes:
 *  	Mirroring must also be globally enabled.
 */
int
bcm_esw_mirror_egress_set(int unit, bcm_port_t port, int enable)
{

    int rv;

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    /* Input parameters check. */
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (IS_CPU_PORT(unit, port) &&
        !soc_feature(unit, soc_feature_cpuport_mirror)) {
        return BCM_E_PORT;
    }

    MIRROR_LOCK(unit);

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_mirror_flexible)) {
        /* Turn on all MTPs since this is for single MTP mode */
        rv = _bcm_esw_mirror_egress_set(unit, port, 
                            (enable) ?  BCM_TR2_MIRROR_MTP_MASK : (0));
    } else 
#endif /* BCM_TRIUMPH2_SUPPORT */
    {
    rv = _bcm_esw_mirror_egress_set(unit, port, 
                                    (enable) ?  BCM_MIRROR_MTP_ONE : (0));
    }

    MIRROR_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *	    bcm_esw_mirror_egress_get
 * Description:
 * 	    Get the mirroring per egress enabled/disabled status
 * Parameters:
 *  	unit -   (IN)  BCM device number
 *  	port -   (IN)  The port to check
 *  	enable - (OUT) Place to store boolean return value for on/off
 * Returns:
 *  	BCM_E_XXX
 */
int
bcm_esw_mirror_egress_get(int unit, bcm_port_t port, int *enable)
{
    int rv; 

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if (NULL == enable) {
        return (BCM_E_PARAM);
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return (BCM_E_PORT);
    }

    if (IS_CPU_PORT(unit, port) &&
        !soc_feature(unit, soc_feature_cpuport_mirror)) {
        return (BCM_E_PORT);
    }

    MIRROR_LOCK(unit); 
    rv = _bcm_esw_mirror_egress_get(unit, port, enable);
    MIRROR_UNLOCK(unit);
    *enable = *enable ? 1 : 0;
    return (rv);
}

/*
 * Function:
 *  	bcm_esw_mirror_pfmt_set
 * Description:
 *  	Set the mirroring preserve format field
 * Parameters:
 *	    unit   - (IN)BCM device number
 *  	enable - (IN)Value for preserve format on/off
 * Returns:
 *	    BCM_E_XXX
 */
int
bcm_esw_mirror_pfmt_set(int unit, int enable)
{
    int rv = BCM_E_UNAVAIL;

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    MIRROR_LOCK(unit);
#if defined(BCM_XGS12_SWITCH_SUPPORT)
    if (SOC_IS_XGS12_SWITCH(unit)) {
        rv = bcm_xgs_mirror_pfmt_set(unit, enable);
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */
    MIRROR_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *	    bcm_esw_mirror_pfmt_get
 * Description:
 *	    Set the mirroring preserve format field
 * Parameters:
 *	    unit   - (IN)  BCM device number
 *	    enable - (OUT) Value for preserve format on/off
 * Returns:
 *	    BCM_E_XXX
 */
int
bcm_esw_mirror_pfmt_get(int unit, int *enable)
{
    int rv = BCM_E_UNAVAIL;

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    MIRROR_LOCK(unit);
#if defined(BCM_XGS12_SWITCH_SUPPORT)
    if (SOC_IS_XGS12_SWITCH(unit)) {
        rv = bcm_xgs_mirror_pfmt_get(unit, enable);
    }
#endif /* BCM_XGS12_SWITCH_SUPPORT */
    MIRROR_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *      bcm_esw_mirror_to_pbmp_set
 * Description:
 *  	Set the mirror-to port bitmap for mirroring on a given port.
 * Parameters:
 *  	unit - (IN) BCM device number
 *  	port - (IN) The port to affect
 *      pbmp - (IN) The port bitmap of mirrored to ports for this port.
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	This API interface is only supported on XGS fabric devices and
 *      production versions of XGS3 switch devices. For XGS3 devices
 *      this function is normally only used when the XGS3 device is
 *      stacked in a ring configuration with BCM567x fabric devices.
 */
int
bcm_esw_mirror_to_pbmp_set(int unit, bcm_port_t port, pbmp_t pbmp)
{

#if defined(BCM_HERCULES_SUPPORT) 
    bcm_mirror_destination_t    mirror_dest;    /* Mirror destination.*/
    int                         rv;             /* Return Value */
#endif /* BCM_HERCULES_SUPPORT */


    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }
    /* Input parameters check. */
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

#if defined(BCM_HERCULES_SUPPORT)
    if (SOC_IS_HERCULES(unit)) {
        int mport;
        bcm_module_t mymodid;

        MIRROR_CONFIG_MODE(unit) = BCM_MIRROR_L2;
        mport = -1;
        PBMP_ITER(pbmp, mport) {
            break;
        }

        /* Get local modid. */
        BCM_IF_ERROR_RETURN(_bcm_esw_local_modid_get(unit, &mymodid));

        /* Create traditional mirror destination. */
        BCM_IF_ERROR_RETURN(
            _bcm_mirror_gport_construct(unit, mport, mymodid, 0, 
                                        &(mirror_dest.gport)));
        BCM_GPORT_MIRROR_SET(mirror_dest.mirror_dest_id, 0);
        mirror_dest.flags = BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_REPLACE;

        MIRROR_LOCK(unit);

        rv = bcm_esw_mirror_destination_create(unit, &mirror_dest);
        if (BCM_FAILURE(rv)) {
            MIRROR_UNLOCK(unit);
            return (rv);
        }

        MIRROR_CONFIG_ING_MTP_DEST(unit, 0) = mirror_dest.mirror_dest_id;
        MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0) = 1;

        MIRROR_UNLOCK(unit);

        return WRITE_ING_MIRTOBMAPr(unit, port, SOC_PBMP_WORD_GET(pbmp, 0));
    }
#endif /* BCM_HERCULES_SUPPORT */
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (soc_feature(unit, soc_feature_egr_mirror_path)) {
        int mport;

        /* Both ingress and egress ports must be stack ports */
        if (!IS_ST_PORT(unit, port)) {
            return (BCM_E_PARAM);
        }
        
        PBMP_ITER(pbmp, mport) {
            if (!IS_ST_PORT(unit, mport)) {
                return (BCM_E_PARAM);
            }
        }
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_REG_IS_VALID(unit, IMIRROR_BITMAP_64r)) {
            uint32 values[2];
            soc_field_t fields[] = {BITMAP_LOf, BITMAP_HIf};

            values[0] = SOC_PBMP_WORD_GET(pbmp, 0);
            values[1] = SOC_PBMP_WORD_GET(pbmp, 1);
            return soc_reg_fields32_modify(unit, IMIRROR_BITMAP_64r, port,
                                           2, fields, values);
        } else 
#endif /* BCM_TRIUMPH2_SUPPORT */
        {
            uint32 mirbmap;

            mirbmap = SOC_PBMP_WORD_GET(pbmp, 0);
            if (SOC_IS_FBX(unit)) {
                mirbmap >>= SOC_HG_OFFSET(unit);
            }
            return WRITE_IMIRROR_BITMAPr(unit, port, mirbmap);
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *   	bcm_esw_mirror_to_pbmp_get
 * Description:
 *	    Get the mirror-to port bitmap for mirroring on the
 *	    specified port.
 * Parameters:
 *	    unit - (IN) BCM device number
 *	    port - (IN) The port to mirror all ingress/egress selections to
 *      pbmp - (OUT) The port bitmap of mirror-to ports for this port.
 * Returns:
 *	    BCM_E_XXX
 */
int
bcm_esw_mirror_to_pbmp_get(int unit, bcm_port_t port, pbmp_t *pbmp)
{

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    /* Input parameters check. */
    if (!SOC_PORT_VALID(unit, port) || !IS_PORT(unit, port)) {
        return BCM_E_PORT;
    }

#if defined(BCM_HERCULES_SUPPORT)
    if (SOC_IS_HERCULES(unit)) {
        int rv;
        uint32 mirbmap;

        rv = READ_ING_MIRTOBMAPr(unit, port, &mirbmap);
        SOC_PBMP_CLEAR(*pbmp);
        SOC_PBMP_WORD_SET(*pbmp, 0, mirbmap);
        return rv;
    }
#endif /* BCM_HERCULES_SUPPORT */

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (soc_feature(unit, soc_feature_egr_mirror_path)) {
        int rv;
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_REG_IS_VALID(unit, IMIRROR_BITMAP_64r)) {
            uint64 mirbmap64;
            rv = READ_IMIRROR_BITMAP_64r(unit, port, &mirbmap64);
            SOC_PBMP_CLEAR(*pbmp);
            SOC_PBMP_WORD_SET(*pbmp, 0,
                soc_reg64_field32_get(unit, IMIRROR_BITMAP_64r,
                                      mirbmap64, BITMAP_LOf));
            SOC_PBMP_WORD_SET(*pbmp, 1,
                soc_reg64_field32_get(unit, IMIRROR_BITMAP_64r,
                                      mirbmap64, BITMAP_HIf));
        } else 
#endif /* BCM_TRIUMPH2_SUPPORT */
        {
            uint32 mirbmap;

            rv = READ_IMIRROR_BITMAPr(unit, port, &mirbmap);
            if (SOC_IS_FBX(unit)) {
                mirbmap <<= SOC_HG_OFFSET(unit);
            }
            SOC_PBMP_CLEAR(*pbmp);
            SOC_PBMP_WORD_SET(*pbmp, 0, mirbmap);
        }
        return rv;
    }
#endif
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *      bcm_esw_mirror_vlan_set
 * Description:
 *      Set VLAN for egressing mirrored packets on a port (RSPAN)
 * Parameters:
 *      unit    - (IN) Bcm device number.
 *      port    - (IN) Mirror-to port to set (-1 for all ports).
 *      tpid    - (IN) Tag protocol id (0 to disable).
 *      vlan    - (IN) Virtual lan number (0 to disable).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mirror_vlan_set(int unit, bcm_port_t port,
                        uint16 tpid, uint16 vlan)
{
    int rv;

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Vlan id range check. */ 
    if (vlan > BCM_VLAN_INVALID) {
        return (BCM_E_PARAM);
    } 

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    if(!SOC_PORT_VALID(unit, port)) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        rv = WRITE_EGR_RSPAN_VLAN_TAGr(unit, port, (tpid << 16) | vlan);
    } else 
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        rv = WRITE_EGR_RSPANr(unit, port, (tpid << 16) | vlan);
    } else 
#endif /* BCM_EASYRIDER_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }
    return (rv);
}

/*
 * Function:
 *      bcm_esw_mirror_vlan_get
 * Description:
 *      Get VLAN for egressing mirrored packets on a port (RSPAN)
 * Parameters:
 *      unit    - (IN) BCM device number
 *      port    - (IN) Mirror-to port for which to get tag info
 *      tpid    - (OUT) tag protocol id
 *      vlan    - (OUT) virtual lan number
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mirror_vlan_get(int unit, bcm_port_t port,
                        uint16 *tpid, uint16 *vlan)
{
    int rv;

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if ((NULL == tpid) || (NULL == vlan)) {
        return (BCM_E_PARAM);
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

#ifdef BCM_FIREBOLT_SUPPORT
    if (SOC_IS_FBX(unit)) {
        uint32 rspan;

        BCM_IF_ERROR_RETURN(READ_EGR_RSPAN_VLAN_TAGr(unit, port, &rspan));
        *tpid = (rspan >> 16);
        *vlan = (rspan & 0xFFF);

        rv = BCM_E_NONE;
    } else 
#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT
    if (SOC_IS_EASYRIDER(unit)) {
        uint32 rspan;

        BCM_IF_ERROR_RETURN(READ_EGR_RSPANr(unit, port, &rspan));

        *tpid = (rspan >> 16);
        *vlan = (rspan & 0xFFF);

        rv = BCM_E_NONE;
    } else 
#endif /* BCM_EASYRIDER_SUPPORT */
    {
        rv = BCM_E_UNAVAIL;
    }
    return (rv);
}

/*
 * Function:
 *      bcm_esw_mirror_egress_path_set
 * Description:
 *      Set egress mirror packet path for stack ring
 * Parameters:
 *      unit    - (IN) BCM device number
 *      modid   - (IN) Destination module ID (of mirror-to port)
 *      port    - (IN) Stack port for egress mirror packet
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      This function should only be used for XGS3 devices stacked
 *      in a ring configuration with fabric devices that may block
 *      egress mirror packets when the mirror-to port is on a 
 *      different device than the egress port being mirrored.
 *      Currently the only such fabric device is BCM5675 rev A0.
 */
int
bcm_esw_mirror_egress_path_set(int unit, bcm_module_t modid, bcm_port_t port)
{
    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_mirror_gport_resolve(unit, port, &port, &modid));
    } else {
        bcm_module_t    mod_out;
        bcm_port_t      port_out;

        PORT_DUALMODID_VALID(unit, port);
        BCM_IF_ERROR_RETURN
            (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                    modid, port,
                                    &mod_out, &port_out));
        if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
            return BCM_E_BADID;
        }
        if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
            return BCM_E_PORT;
        }
        port = port_out;
        modid = mod_out;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) { 
        return bcm_xgs3_mirror_egress_path_set(unit, modid, port);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *      bcm_esw_mirror_egress_path_get
 * Description:
 *      Get egress mirror packet path for stack ring
 * Parameters:
 *      unit    - (IN) BCM device number
 *      modid   - (IN) Destination module ID (of mirror-to port)
 *      port    - (OUT)Stack port for egress mirror packet
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      See bcm_mirror_alt_egress_pbmp_set for more details.
 */
int
bcm_esw_mirror_egress_path_get(int unit, bcm_module_t modid, bcm_port_t *port)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    int             rv, isGport;
    bcm_module_t    mod_out;
    bcm_port_t      port_out;
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if (NULL == port) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) { 
        rv = bcm_xgs3_mirror_egress_path_get(unit, modid, port);

        if (BCM_FAILURE(rv)) {
            return rv;
        }
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        if (isGport) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, modid, *port, 
                                       &mod_out, &port_out));
            BCM_IF_ERROR_RETURN(
                _bcm_mirror_gport_construct(unit, port_out, mod_out, 0, port)); 
        } else {
            BCM_IF_ERROR_RETURN(
                bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, modid, *port, 
                                       &mod_out, port));
        }

        return (BCM_E_NONE);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return (BCM_E_UNAVAIL);
}


/*
 * Function:
 *      bcm_esw_mirror_port_set
 * Description:
 *      Set mirroring configuration for a port
 * Parameters:
 *      unit      - BCM device number
 *      port      - port to configure
 *      dest_mod  - module id of mirror-to port
 *                  (-1 for local port)
 *      dest_port - mirror-to port ( can be gport or mirror_gport)
 *      flags     - BCM_MIRROR_PORT_* flags
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Setting BCM_MIRROR_PORT_ENABLE without setting _INGRESS or
 *      _EGRESS allows the port to participate in bcm_l2_cache matches
 *      with the BCM_L2_CACHE_MIRROR bit set, and to participate in
 *      bcm_field lookups with the mirror action set.
 *
 *      If bcmSwitchDirectedMirroring is disabled for the unit and
 *      dest_mod is non-negative, then the dest_mod path is looked
 *      up using bcm_topo_port_get.
 *      If bcmSwitchDirectedMirroring is enabled for the unit and
 *      dest_mod is negative, then the local unit's modid is used
 *      as the dest_mod.
 */
int
bcm_esw_mirror_port_set(int unit, bcm_port_t port,
                        bcm_module_t dest_mod, bcm_port_t dest_port,
                        uint32 flags)
{
    int         rv;                        /* Operation return status.        */
    bcm_mirror_destination_t mirror_dest;  /* Mirror destination.             */
    uint32                   destroy_flag = FALSE; /* mirror dest destroy     */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    /* If mirroring is completely disabled, remove all mirror destinations */
    if (flags == 0 && dest_mod == -1 && dest_port == -1) {
        flags = BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS |
            BCM_MIRROR_PORT_EGRESS_TRUE;
        return bcm_esw_mirror_port_dest_delete_all(unit, port, flags);
    }

    /* Create traditional mirror destination. */
    bcm_mirror_destination_t_init(&mirror_dest);

    MIRROR_LOCK(unit);
    if (BCM_GPORT_IS_MIRROR(dest_port)) {
        rv = bcm_esw_mirror_destination_get(unit, dest_port, &mirror_dest);
    } else {
        rv = _bcm_esw_mirror_destination_find(unit, dest_port, dest_mod, flags, &mirror_dest); 
        if (BCM_E_NOT_FOUND == rv) {
            if ((flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) |
                (soc_feature(unit, soc_feature_egr_mirror_true) &&
                 (flags & BCM_MIRROR_PORT_EGRESS_TRUE))) {
                rv = _bcm_esw_mirror_destination_create(unit, &mirror_dest);
                destroy_flag = TRUE;
            } else {
                MIRROR_UNLOCK(unit); 
                return (BCM_E_NONE);
            }
        }       
    }
    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        return rv;
    }

    /* Enable/Disable ingress mirroring. */
    if (flags & BCM_MIRROR_PORT_INGRESS) {
        rv = bcm_esw_mirror_port_dest_add(unit, port, BCM_MIRROR_PORT_INGRESS,
                                          mirror_dest.mirror_dest_id); 
    } else {
        rv = bcm_esw_mirror_port_dest_delete(unit, port, BCM_MIRROR_PORT_INGRESS,
                                             mirror_dest.mirror_dest_id); 
        if (BCM_E_NOT_FOUND == rv) {
            /* There is no clean way to identify delete. -> 
               if destination is not found assume success. */ 
            rv = BCM_E_NONE;
        } 
    }

    if (BCM_FAILURE(rv)) {
        /* Delete unused mirror destination. */
        if (destroy_flag) {
            (void)bcm_esw_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
        }
        MIRROR_UNLOCK(unit); 
        return (rv);
    }

    /* Enable/Disable egress mirroring. */
    if (flags & BCM_MIRROR_PORT_EGRESS) {
        rv = bcm_esw_mirror_port_dest_add(unit, port, BCM_MIRROR_PORT_EGRESS,
                                          mirror_dest.mirror_dest_id); 
    } else {
        rv = bcm_esw_mirror_port_dest_delete(unit, port, BCM_MIRROR_PORT_EGRESS,
                                             mirror_dest.mirror_dest_id); 
        if (BCM_E_NOT_FOUND == rv) {
            /* There is no clean way to identify delete. -> 
               if destination is not found assume success. */ 
            rv = BCM_E_NONE;
        } 
    }

    if (BCM_FAILURE(rv)) {
        /* Delete unused mirror destination. */
        if (destroy_flag) {
            (void)bcm_esw_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
        }
        MIRROR_UNLOCK(unit); 
        return (rv);
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_egr_mirror_true)) {
        /* Enable/Disable egress true mirroring. */
        if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
            rv = bcm_esw_mirror_port_dest_add(unit, port,
                                              BCM_MIRROR_PORT_EGRESS_TRUE,
                                              mirror_dest.mirror_dest_id); 
        } else {
            rv = bcm_esw_mirror_port_dest_delete(unit, port,
                                                 BCM_MIRROR_PORT_EGRESS_TRUE,
                                                 mirror_dest.mirror_dest_id); 
            if (BCM_E_NOT_FOUND == rv) {
                /* There is no clean way to identify delete. -> 
                   if destination is not found assume success. */ 
                rv = BCM_E_NONE;
            } 
        }

        if (BCM_FAILURE(rv)) {
            /* Delete unused mirror destination. */
            if (destroy_flag) {
                (void)bcm_esw_mirror_destination_destroy(unit,
                                                 mirror_dest.mirror_dest_id);
            }
            MIRROR_UNLOCK(unit); 
            return (rv);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */  

    /* Delete unused mirror destination. */
    if (1 >= MIRROR_DEST_REF_COUNT(unit, mirror_dest.mirror_dest_id)) {
        rv = bcm_esw_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
    }

    MIRROR_UNLOCK(unit); 
    return (rv);
}
/*
 * Function:
 *      bcm_esw_mirror_port_get
 * Description:
 *      Get mirroring configuration for a port
 * Parameters:
 *      unit      - BCM device number
 *      port      - port to get configuration for
 *      dest_mod  - (OUT) module id of mirror-to port
 *      dest_port - (OUT) mirror-to port
 *      flags     - (OUT) BCM_MIRROR_PORT_* flags
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_mirror_port_get(int unit, bcm_port_t port,
                        bcm_module_t *dest_mod, bcm_port_t *dest_port,
                        uint32 *flags)
{
    int directed;                             /* Directed mirroring mode.  */
    bcm_gport_t mirror_dest_id;               /* Mirror destination  id.   */
    int enable;                               /* Egress mirror is enabled. */
    int rv;                                   /* Operation return status.  */
    int mirror_dest_count = 0;                /* Mirror destination found. */
    int isGport;                              /* gport indicator */  
    bcm_mirror_destination_t    mirror_dest;  /* mirror destination struct */       

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if ((NULL == flags) || (NULL == dest_mod) || (NULL == dest_port)) {
        return (BCM_E_PARAM);
    }

    bcm_mirror_destination_t_init(&mirror_dest);

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    *flags = 0;

    /* Check if directed mirroring is enabled and gport required. */
    BCM_IF_ERROR_RETURN(_bcm_esw_directed_mirroring_get(unit, &directed));
    BCM_IF_ERROR_RETURN(bcm_esw_switch_control_get(unit, bcmSwitchUseGport, 
                                                   &isGport));
    
    MIRROR_LOCK(unit);

    /* Read port ingress mirroring destination ports. */
    rv = bcm_esw_mirror_port_dest_get(unit, port, BCM_MIRROR_PORT_INGRESS, 
                                      1, &mirror_dest_id, &mirror_dest_count);

    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        return (rv);
    }

    if (mirror_dest_count) {
        rv = bcm_esw_mirror_destination_get(unit, mirror_dest_id, &mirror_dest);

        if (BCM_FAILURE(rv)) {
            MIRROR_UNLOCK(unit);
            return (rv);
        }
        *flags |= BCM_MIRROR_PORT_INGRESS;

        if (0 == directed) {
            /* Read mtp egress enable bitmap for source port. */
            rv = _bcm_esw_mirror_egress_get(unit, port, &enable);
            if (BCM_FAILURE(rv)) {
                MIRROR_UNLOCK(unit);
                return (rv);
            }
            if (enable) {
                *flags |= BCM_MIRROR_PORT_EGRESS;
            }
        }
        MIRROR_UNLOCK(unit);

        if (isGport) {
            *dest_port = mirror_dest.gport;
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_mirror_gport_resolve(unit, mirror_dest.gport,
                                          dest_port, dest_mod));
            BCM_IF_ERROR_RETURN(
                _bcm_gport_modport_hw2api_map(unit, *dest_mod, *dest_port, 
                                              dest_mod, dest_port));
        }

        return (BCM_E_NONE);
    }

    /* Read port ingress mirroring destination ports. */
    rv = bcm_esw_mirror_port_dest_get(unit, port, BCM_MIRROR_PORT_EGRESS, 
                                      1, &mirror_dest_id, &mirror_dest_count);

    if (BCM_FAILURE(rv)) {
        MIRROR_UNLOCK(unit);
        return (rv);
    }

    if (mirror_dest_count) {
        rv = bcm_esw_mirror_destination_get(unit, mirror_dest_id, &mirror_dest);

        if (BCM_FAILURE(rv)) {
            MIRROR_UNLOCK(unit);
            return (rv);
        }
        *flags |= BCM_MIRROR_PORT_EGRESS;

        MIRROR_UNLOCK(unit);

        if (isGport) {
            *dest_port = mirror_dest.gport;
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_mirror_gport_resolve(unit, mirror_dest.gport,
                                          dest_port, dest_mod));
            BCM_IF_ERROR_RETURN(
                _bcm_gport_modport_hw2api_map(unit, *dest_mod, *dest_port, 
                                              dest_mod, dest_port));
        }

        return (BCM_E_NONE);
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_egr_mirror_true)) {
        /* Read port ingress mirroring destination ports. */
        rv = bcm_esw_mirror_port_dest_get(unit, port,
                                          BCM_MIRROR_PORT_EGRESS_TRUE, 
                                          1, &mirror_dest_id,
                                          &mirror_dest_count);

        if (BCM_FAILURE(rv)) {
            MIRROR_UNLOCK(unit);
            return (rv);
        }

        if (mirror_dest_count) {
            rv = bcm_esw_mirror_destination_get(unit, mirror_dest_id,
                                                &mirror_dest);

            if (BCM_FAILURE(rv)) {
                MIRROR_UNLOCK(unit);
                return (rv);
            }
            *flags |= BCM_MIRROR_PORT_EGRESS_TRUE;

            MIRROR_UNLOCK(unit);

            if (isGport) {
                *dest_port = mirror_dest.gport;
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_mirror_gport_resolve(unit, mirror_dest.gport,
                                               dest_port, dest_mod));
                BCM_IF_ERROR_RETURN
                    (_bcm_gport_modport_hw2api_map(unit, *dest_mod, *dest_port, 
                                                   dest_mod, dest_port));
            }

            return (BCM_E_NONE);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */  

    MIRROR_UNLOCK(unit);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     bcm_esw_mirror_port_dest_add 
 * Purpose:
 *      Add mirroring destination to a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      flags        -  (IN) BCM_MIRROR_PORT_XXX flags.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_esw_mirror_port_dest_add(int unit, bcm_port_t port, 
                              uint32 flags, bcm_gport_t mirror_dest) 
{
    int          directed;          /* Directed mirroring indicator. */
    int          rv = BCM_E_NONE;   /* Operation return status.      */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */

    if (-1 != port) { 
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
        }

        if (!SOC_PORT_VALID(unit, port)) {
            return (BCM_E_PORT);
        }

        if (IS_CPU_PORT(unit, port) &&
            !soc_feature(unit, soc_feature_cpuport_mirror)) {
            return (BCM_E_PORT);
        }
    }

    if (!soc_feature(unit, soc_feature_egr_mirror_true) &&
        (flags & BCM_MIRROR_PORT_EGRESS_TRUE)) {
        return (BCM_E_PARAM);
    }

    if (!(flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS |
                   BCM_MIRROR_PORT_EGRESS_TRUE))) { 
        return (BCM_E_PARAM);
    }

    /* Check if directed mirroring is enabled. */
    BCM_IF_ERROR_RETURN(_bcm_esw_directed_mirroring_get(unit, &directed));

    /* Directed  mirroring support check. */
    if (0 == directed) {

        /* No mirrorint to a trunk. */
        if (BCM_GPORT_IS_TRUNK(MIRROR_DEST_GPORT(unit, mirror_dest))) {
            return (BCM_E_UNAVAIL);
        } 

        /* Single mirroring destination for ingress & egress. */
        if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)) {
            if (MIRROR_CONFIG_ING_MTP_DEST(unit, 0) != mirror_dest) {
                return (BCM_E_RESOURCE);
            }
        }
        if (MIRROR_CONFIG_EGR_MTP_REF_COUNT(unit, 0)) {
            if (MIRROR_CONFIG_EGR_MTP_DEST(unit, 0) != mirror_dest) {
                return (BCM_E_RESOURCE);
            }
        }
    }

    MIRROR_LOCK(unit);

    if (flags & BCM_MIRROR_PORT_INGRESS) {
        rv = _bcm_esw_mirror_port_ingress_dest_add(unit, port, mirror_dest);
    }

    if (BCM_SUCCESS(rv) && (flags & BCM_MIRROR_PORT_EGRESS)) {
        rv = _bcm_esw_mirror_port_egress_dest_add(unit, port, mirror_dest);

        /* Check for operation failure. */
        if (BCM_FAILURE(rv)) {
            if (flags & BCM_MIRROR_PORT_INGRESS) {
                _bcm_esw_mirror_port_ingress_dest_delete(unit, port,
                                                         mirror_dest);
            }
        }
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if (BCM_SUCCESS(rv) && (flags & BCM_MIRROR_PORT_EGRESS_TRUE)) {
        rv = _bcm_esw_mirror_port_egress_true_dest_add(unit, port,
                                                       mirror_dest);

        /* Check for operation failure. */
        if (BCM_FAILURE(rv)) {
            if (flags & BCM_MIRROR_PORT_INGRESS) {
                _bcm_esw_mirror_port_ingress_dest_delete(unit, port,
                                                         mirror_dest);
            }
            if (flags & BCM_MIRROR_PORT_EGRESS) {
                _bcm_esw_mirror_port_egress_dest_delete(unit, port,
                                                        mirror_dest);
            }
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */  
  

    /* Update stacking mirror destination bitmap. */
    if (BCM_SUCCESS(rv) && (-1 != port) && IS_ST_PORT(unit, port)) {
        rv = _bcm_esw_mirror_stacking_dest_update
                             (unit, port, MIRROR_DEST_GPORT(unit, mirror_dest));
        /* Check for operation failure. */
        if (BCM_FAILURE(rv)) {
            if (flags & BCM_MIRROR_PORT_INGRESS) {
                _bcm_esw_mirror_port_ingress_dest_delete(unit, port,
                                                         mirror_dest);
            }
            if (flags & BCM_MIRROR_PORT_EGRESS) {
                _bcm_esw_mirror_port_egress_dest_delete(unit, port,
                                                        mirror_dest);
            }
#ifdef BCM_TRIUMPH2_SUPPORT
            if (flags & BCM_MIRROR_PORT_EGRESS_TRUE) {
                _bcm_esw_mirror_port_egress_true_dest_delete(unit, port,
                                                             mirror_dest);
            }
#endif /* BCM_TRIUMPH2_SUPPORT */  
        }
    }

    /* Enable mirroring on a port.  */
    if (BCM_SUCCESS(rv)) { 
        if(!SOC_IS_XGS3_SWITCH(unit) || 
           (BCM_MIRROR_DISABLE == MIRROR_CONFIG_MODE(unit))) {
            rv = _bcm_esw_mirror_enable(unit);
            MIRROR_CONFIG_MODE(unit) = BCM_MIRROR_L2;
        }
    }

    MIRROR_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *     bcm_esw_mirror_port_dest_delete
 * Purpose:
 *      Remove mirroring destination from a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      flags        -  (IN) BCM_MIRROR_PORT_XXX flags.
 *      mirror_dest  -  (IN) Mirroring destination gport. 
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_esw_mirror_port_dest_delete(int unit, bcm_port_t port, 
                                uint32 flags, bcm_gport_t mirror_dest) 
{
    int final_rv = BCM_E_NONE;      /* Operation return status. */
    int rv = BCM_E_NONE;            /* Operation return status. */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */

    if (-1 != port) { 
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
        }

        if (!SOC_PORT_VALID(unit, port)) {
            return (BCM_E_PORT);
        }

        if (IS_CPU_PORT(unit, port) &&
            !soc_feature(unit, soc_feature_cpuport_mirror)) {
            return (BCM_E_PORT);
        }
    }

    if (!soc_feature(unit, soc_feature_egr_mirror_true) &&
        (flags & BCM_MIRROR_PORT_EGRESS_TRUE)) {
        return (BCM_E_PARAM);
    }

    if (!(flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS |
                   BCM_MIRROR_PORT_EGRESS_TRUE))) { 
        return (BCM_E_PARAM);
    }

    MIRROR_LOCK(unit);

    if ((flags & BCM_MIRROR_PORT_INGRESS) &&
        (BCM_GPORT_INVALID != mirror_dest)) {
        final_rv = _bcm_esw_mirror_port_ingress_dest_delete(unit, port,
                                                            mirror_dest);
    }

    if ((flags & BCM_MIRROR_PORT_EGRESS) &&
        (BCM_GPORT_INVALID != mirror_dest)) {
        rv = _bcm_esw_mirror_port_egress_dest_delete(unit, port,
                                                     mirror_dest);
        if (!BCM_FAILURE(final_rv)) {
            final_rv = rv;
        }
    }

#ifdef BCM_TRIUMPH2_SUPPORT
    if ((flags & BCM_MIRROR_PORT_EGRESS_TRUE) &&
        (BCM_GPORT_INVALID != mirror_dest)) {
        rv = _bcm_esw_mirror_port_egress_true_dest_delete(unit, port,
                                                          mirror_dest);
        if (!BCM_FAILURE(final_rv)) {
            final_rv = rv;
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT */

    /* Update stacking mirror destination bitmap. */
    if ((-1 != port) && (IS_ST_PORT(unit, port))) {
        rv = _bcm_esw_mirror_stacking_dest_update(unit, port, BCM_GPORT_INVALID);
        if (!BCM_FAILURE(final_rv)) {
            final_rv = rv;
        }
    }

    MIRROR_UNLOCK(unit);
    return (final_rv);
}

/*
 * Function:
 *     bcm_esw_mirror_port_dest_get
 * Purpose:
 *     Get port mirroring destinations.   
 * Parameters:
 *     unit             - (IN) BCM device number. 
 *     port             - (IN) Port mirrored port.
 *     flags            - (IN) BCM_MIRROR_PORT_XXX flags.
 *     mirror_dest_size - (IN) Preallocated mirror_dest array size.
 *     mirror_dest      - (OUT)Filled array of port mirroring destinations
 *     mirror_dest_count - (OUT)Actual number of mirroring destinations filled.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_esw_mirror_port_dest_get(int unit, bcm_port_t port, uint32 flags, 
                         int mirror_dest_size, bcm_gport_t *mirror_dest,
                         int *mirror_dest_count)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    int         idx;                    /* Mirror to port iteration index.  */
    int         index = 0;              /* Filled destinations index.       */
    bcm_gport_t mtp_dest[BCM_MIRROR_MTP_MAX]; /* Mirror destinations array. */
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    int         rv = BCM_E_NONE;        /* Operation return status.         */
    int         directed = 0;           /* Indication if directed mirroring */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    if (-1 == port) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_directed_mirroring_get(unit, &directed));
        if (directed) {
            return (BCM_E_UNAVAIL);
        }
        MIRROR_LOCK(unit);
        if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)) {
            if(NULL != mirror_dest) {
                mirror_dest[0] = MIRROR_CONFIG_ING_MTP_DEST(unit, 0);
            }
            *mirror_dest_count = 1;    
        } else {
            if(NULL != mirror_dest) {
                mirror_dest[0] = BCM_GPORT_INVALID;
            }
            *mirror_dest_count = 0;    
        }
        MIRROR_UNLOCK(unit);
        return BCM_E_NONE;
    }

    /* Input parameters check. */
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port));
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return (BCM_E_PORT);
    }

    if (IS_CPU_PORT(unit, port) &&
        !soc_feature(unit, soc_feature_cpuport_mirror)) {
        return (BCM_E_PORT);
    }

    if ((0 != mirror_dest_size) && (NULL == mirror_dest)) {
        return (BCM_E_PARAM);
    }

    if (NULL == mirror_dest_count) {
        return (BCM_E_PARAM);
    }

    MIRROR_LOCK(unit);

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit))  {
        if (flags & BCM_MIRROR_PORT_INGRESS) {
            rv = _bcm_xgs3_mirror_port_ingress_dest_get(unit, port, 
                                                        BCM_MIRROR_MTP_MAX,
                                                        mtp_dest);
            for (idx = 0; idx < BCM_MIRROR_MTP_MAX; idx++) { 
                if ((index < mirror_dest_size) && 
                    (BCM_GPORT_INVALID != mtp_dest[idx])) {
                    if(NULL != (mirror_dest + index)) {
                        mirror_dest[index] = mtp_dest[idx];
                    }
                    index++;
                }
            }
        }
        if ((flags & BCM_MIRROR_PORT_EGRESS) &&
            (index <  mirror_dest_size)) {
            rv = _bcm_xgs3_mirror_port_egress_dest_get(unit, port, 
                                                       BCM_MIRROR_MTP_MAX,
                                                       mtp_dest);
            for (idx = 0; idx < BCM_MIRROR_MTP_MAX; idx++) { 
                if ((index < mirror_dest_size) && 
                    (BCM_GPORT_INVALID != mtp_dest[idx])) {
                    if(NULL != (mirror_dest + index)) {
                        mirror_dest[index] = mtp_dest[idx];
                    }
                    index++;
                }
            }
        }
#ifdef BCM_TRIUMPH2_SUPPORT
        if (soc_feature(unit, soc_feature_egr_mirror_true) &&
            (flags & BCM_MIRROR_PORT_EGRESS_TRUE) &&
            (index <  mirror_dest_size)) {
            rv = _bcm_tr2_mirror_port_egress_true_dest_get(unit, port, 
                                                       BCM_MIRROR_MTP_MAX,
                                                       mtp_dest);
            for (idx = 0; idx < BCM_MIRROR_MTP_MAX; idx++) { 
                if ((index < mirror_dest_size) && 
                    (BCM_GPORT_INVALID != mtp_dest[idx])) {
                    if(NULL != (mirror_dest + index)) {
                        mirror_dest[index] = mtp_dest[idx];
                    }
                    index++;
                }
            }
        }
#endif /* BCM_TRIUMPH2_SUPPORT */
        *mirror_dest_count = index;
    } else 
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        if (MIRROR_CONFIG_ING_MTP_REF_COUNT(unit, 0)) {
            if(NULL != mirror_dest) {
                mirror_dest[0] = MIRROR_CONFIG_ING_MTP_DEST(unit, 0);
            }
            *mirror_dest_count = 1;    
        } else {
            if(NULL != mirror_dest) {
                mirror_dest[0] = BCM_GPORT_INVALID;
            }
            *mirror_dest_count = 0;    
        }

    MIRROR_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *     bcm_esw_mirror_port_dest_delete_all
 * Purpose:
 *      Remove all mirroring destinations from a port. 
 * Parameters:
 *      unit         -  (IN) BCM device number. 
 *      port         -  (IN) Port mirrored port.
 *      flags        -  (IN) BCM_MIRROR_PORT_XXX flags.
 * Returns:
 *      BCM_X_XXX
 */
int
bcm_esw_mirror_port_dest_delete_all(int unit, bcm_port_t port, uint32 flags) 
{
    bcm_gport_t mirror_dest[BCM_MIRROR_MTP_MAX];
    int         mirror_dest_count;
    int         index;
    int         rv; 

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    if (!(flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS |
                   BCM_MIRROR_PORT_EGRESS_TRUE))) { 
        return (BCM_E_PARAM);
    }

    MIRROR_LOCK(unit);

    if (flags & BCM_MIRROR_PORT_INGRESS) {
        if (-1 != port) {
            if (BCM_GPORT_IS_SET(port)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, port, &port));
            }
            /* Read port ingress mirroring destination ports. */
            rv = bcm_esw_mirror_port_dest_get(unit, port, BCM_MIRROR_PORT_INGRESS,
                                              BCM_MIRROR_MTP_MAX, mirror_dest,
                                              &mirror_dest_count);
        } else {
            /* Get all ingress mirror destinations. */
            rv = _bcm_mirror_dest_get_all(unit, BCM_MIRROR_PORT_INGRESS, 
                                         BCM_MIRROR_MTP_MAX, mirror_dest, 
                                         &mirror_dest_count);
        }
        if (BCM_FAILURE(rv)) {
            MIRROR_UNLOCK(unit);
            return (rv);
        }

        /* Remove all ingress mirroring destination ports. */
        for (index = 0; index < mirror_dest_count; index++) {
            rv = bcm_esw_mirror_port_dest_delete(unit, port,
                                                 BCM_MIRROR_PORT_INGRESS,
                                                 mirror_dest[index]); 
            if (BCM_FAILURE(rv)) {
                MIRROR_UNLOCK(unit);
                return (rv);
            }
        }
    }

    if (flags & BCM_MIRROR_PORT_EGRESS) {
        /* Read port egress mirroring destination ports. */
        if (-1 != port) {
            if (BCM_GPORT_IS_SET(port)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, port, &port));
            }
            rv = bcm_esw_mirror_port_dest_get(unit, port, BCM_MIRROR_PORT_EGRESS, 
                                              BCM_MIRROR_MTP_MAX, mirror_dest, 
                                              &mirror_dest_count);
        } else {
            /* Get all egress mirror destinations. */
            rv = _bcm_mirror_dest_get_all(unit, BCM_MIRROR_PORT_EGRESS, 
                                          BCM_MIRROR_MTP_MAX, mirror_dest, 
                                          &mirror_dest_count);
        }
        if (BCM_FAILURE(rv)) {
            MIRROR_UNLOCK(unit);
            return (rv);
        }

        /* Remove all egress mirroring destination ports. */
        for (index = 0; index < mirror_dest_count; index++) {
            rv = bcm_esw_mirror_port_dest_delete(unit, port,
                                                 BCM_MIRROR_PORT_EGRESS,
                                                 mirror_dest[index]); 
            if (BCM_FAILURE(rv)) {
                MIRROR_UNLOCK(unit);
                return (rv);
            }
        }
    }

    if (soc_feature(unit, soc_feature_egr_mirror_true) &&
        (flags & BCM_MIRROR_PORT_EGRESS_TRUE)) {
        /* Read port egress true mirroring destination ports. */
        if (-1 != port) {
            if (BCM_GPORT_IS_SET(port)) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, port, &port));
            }
            rv = bcm_esw_mirror_port_dest_get(unit, port,
                                              BCM_MIRROR_PORT_EGRESS_TRUE, 
                                              BCM_MIRROR_MTP_MAX,
                                              mirror_dest, 
                                              &mirror_dest_count);
        } else {
            /* Get all egress mirror destinations. */
            rv = _bcm_mirror_dest_get_all(unit, BCM_MIRROR_PORT_EGRESS_TRUE, 
                                          BCM_MIRROR_MTP_MAX, mirror_dest, 
                                          &mirror_dest_count);
        }
        if (BCM_FAILURE(rv)) {
            MIRROR_UNLOCK(unit);
            return (rv);
        }

        /* Remove all egress mirroring destination ports. */
        for (index = 0; index < mirror_dest_count; index++) {
            rv = bcm_esw_mirror_port_dest_delete(unit, port,
                                                 BCM_MIRROR_PORT_EGRESS_TRUE,
                                                 mirror_dest[index]); 
            if (BCM_FAILURE(rv)) {
                MIRROR_UNLOCK(unit);
                return (rv);
            }
        }
    }

    MIRROR_UNLOCK(unit);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     bcm_esw_mirror_destination_create
 * Purpose:
 *     Create mirror destination description.
 * Parameters:
 *      unit         - (IN) BCM device number. 
 *      mirror_dest  - (IN) Mirror destination description.
 * Returns:
 *      BCM_X_XXX
 */
int 
bcm_esw_mirror_destination_create(int unit, bcm_mirror_destination_t *mirror_dest) 
{
    bcm_module_t mymodid;           /* Local module id.              */
    int          directed;          /* Directed mirroring indicator. */
    bcm_module_t dest_mod;          /* Destination module id.        */
    bcm_port_t   dest_port;         /* Destination port number.      */
    int rv;   /* Operation return status. */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if (NULL == mirror_dest) {
        return (BCM_E_PARAM);
    }
    

    /* Check if device supports advanced mirroring mode. */
    if (mirror_dest->flags & (BCM_MIRROR_DEST_TUNNEL_IP_GRE |
                              BCM_MIRROR_DEST_PAYLOAD_UNTAGGED)) {
        if (0 == SOC_MEM_IS_VALID(unit, EGR_ERSPANm)) {
            return (BCM_E_UNAVAIL);
        }

        /* Bypass mode is enabled check. */
        if (0 == soc_mem_index_count(unit, EGR_ERSPANm)) {
            return (BCM_E_UNAVAIL);
        } 
    }

    /* Untagging payload supported only on IP tunnels. */
    if ((0 == (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE)) && 
        (mirror_dest->flags & BCM_MIRROR_DEST_PAYLOAD_UNTAGGED)) {
        return (BCM_E_UNAVAIL);
    }

    /* Can't do IP-GRE & L3 tunnel simultaneously. */
    if ((mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) && 
        (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_L2)) {
        return (BCM_E_CONFIG);
    }

    /* Resolve miror destination gport */
    BCM_IF_ERROR_RETURN(
        _bcm_mirror_gport_adapt(unit, &(mirror_dest->gport)));

    /* Verify mirror destination port/trunk. */
    if ((0 == BCM_GPORT_IS_MODPORT(mirror_dest->gport)) && 
        (0 == BCM_GPORT_IS_TRUNK(mirror_dest->gport)) &&
        (0 == BCM_GPORT_IS_DEVPORT(mirror_dest->gport))) {
        return (BCM_E_PARAM);
    }

    /* Get local modid. */
    BCM_IF_ERROR_RETURN(_bcm_esw_local_modid_get(unit, &mymodid));


    /* Check if directed mirroring is enabled. */
    BCM_IF_ERROR_RETURN(_bcm_esw_directed_mirroring_get(unit, &directed));

    /* Directed  mirroring support check. */
    if (0 == directed) {
        /* No mirroring to a trunk. */
        if (BCM_GPORT_IS_TRUNK(mirror_dest->gport)) {
            return (BCM_E_UNAVAIL);
        } 

        /* Set mirror destination to outgoing port on local module. */
        dest_mod = BCM_GPORT_IS_DEVPORT(mirror_dest->gport) ? 
            mymodid : BCM_GPORT_MODPORT_MODID_GET(mirror_dest->gport);
        if (mymodid != dest_mod) {
            if (!SOC_IS_TUCANA(unit) || mymodid + 1 != dest_mod) {
                BCM_IF_ERROR_RETURN
                    (bcm_esw_topo_port_get(unit, dest_mod, &dest_port));
                if (SOC_IS_TUCANA(unit) && dest_port >= 32) {
                    mymodid +=1;
                    dest_port -=32;
                }
                BCM_GPORT_MODPORT_SET(mirror_dest->gport, mymodid, dest_port);
            }
        }
    }

    MIRROR_LOCK(unit);
    rv = _bcm_esw_mirror_destination_create(unit, mirror_dest);
    MIRROR_UNLOCK(unit);
    return (rv);
}


/*
 * Function:
 *     bcm_esw_mirror_destination_destroy
 * Purpose:
 *     Destroy mirror destination description.
 * Parameters:
 *      unit            - (IN) BCM device number. 
 *      mirror_dest_id  - (IN) Mirror destination id.
 * Returns:
 *      BCM_X_XXX
 */
int 
bcm_esw_mirror_destination_destroy(int unit, bcm_gport_t mirror_dest_id) 
{
    int rv;   /* Operation return status. */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    if (0 == BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        return (BCM_E_PARAM);
    }

    MIRROR_LOCK(unit);

    /* If destination stil in use - > E_BUSY */
    if (1 < MIRROR_DEST_REF_COUNT(unit, mirror_dest_id)) {
        MIRROR_UNLOCK(unit);
        return (BCM_E_BUSY);
    }

    rv = _bcm_mirror_destination_free(unit, mirror_dest_id); 

    MIRROR_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *     bcm_esw_mirror_destination_get
 * Purpose:
 *     Get mirror destination description.
 * Parameters:
 *      unit            - (IN) BCM device number. 
 *      mirror_dest_id  - (IN) Mirror destination id.
 *      mirror_dest     - (OUT)Mirror destination description.
 * Returns:
 *      BCM_X_XXX
 */
int 
bcm_esw_mirror_destination_get(int unit, bcm_gport_t mirror_dest_id, 
                                   bcm_mirror_destination_t *mirror_dest)
{
    bcm_mirror_destination_t    mirror_destination;
    bcm_port_t                  port, port_out;
    bcm_module_t                modid, modid_out;


    bcm_mirror_destination_t_init(&mirror_destination);

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    if (0 == BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        return (BCM_E_PARAM);
    }

    if (NULL == mirror_dest) {
        return (BCM_E_PARAM);
    }

    MIRROR_LOCK(unit);

    /* If destination is not valid */ 
    if (0 == MIRROR_DEST_REF_COUNT(unit, mirror_dest_id)) {
        MIRROR_UNLOCK(unit);
        return (BCM_E_NOT_FOUND);
    }

    mirror_destination  = *(MIRROR_DEST(unit, mirror_dest_id)); 
    if (BCM_GPORT_IS_MODPORT(mirror_destination.gport)) {
        port = BCM_GPORT_MODPORT_PORT_GET(mirror_destination.gport);
        modid = BCM_GPORT_MODPORT_MODID_GET(mirror_destination.gport);
        if (NUM_MODID(unit) == 2 && port> 31) {
            BCM_IF_ERROR_RETURN(
                bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, modid, port, 
                                       &modid_out, &port_out));
            if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
                return BCM_E_PORT;
            }
            if (!SOC_MODID_ADDRESSABLE(unit, modid_out)) {
                return BCM_E_PARAM;
            }
            port = port_out;
            modid = modid_out;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_mirror_gport_construct(unit, port,modid, 0, 
                                        &(mirror_destination.gport)));
    }
    *mirror_dest = mirror_destination; 
    MIRROR_UNLOCK(unit);
    return (BCM_E_NONE);
}


/*
 * Function:
 *     bcm_esw_mirror_destination_traverse
 * Purpose:
 *     Traverse installed mirror destinations
 * Parameters:
 *      unit      - (IN) BCM device number. 
 *      cb        - (IN) Mirror destination traverse callback.         
 *      user_data - (IN) User cookie
 * Returns:
 *      BCM_X_XXX
 */
int 
bcm_esw_mirror_destination_traverse(int unit, bcm_mirror_destination_traverse_cb cb, 
                                    void *user_data) 
{
    int idx;                                 /* Mirror destinations index.     */
    _bcm_mirror_dest_config_p  mdest;        /* Mirror destination description.*/
    bcm_mirror_destination_t   mirror_dest;  /* User cb mirror destination.    */

    /* Initialization check. */
    if (0 == MIRROR_INIT(unit)) {
        return BCM_E_INIT;
    }

    /* Input parameters check. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    MIRROR_LOCK(unit);
    /* Iterate mirror destinations & call user callback for valid ones. */
    for (idx = 0; idx < MIRROR_CONFIG(unit)->dest_count; idx++) {
        mdest = &MIRROR_CONFIG(unit)->dest_arr[idx];
        if (0 == mdest->ref_count) {
            continue;
        }

        mirror_dest = mdest->mirror_dest;
        (*cb)(unit, &mirror_dest, user_data);
    }
    MIRROR_UNLOCK(unit);
    return (BCM_E_NONE);
}
