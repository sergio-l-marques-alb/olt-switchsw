/*
 * $Id: mirror.c,v 1.53 Broadcom SDK $
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
 * Mirror - Broadcom Mirror API.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MIRROR

#include <shared/bsl.h>

#include "bcm_int/common/debug.h"

#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/stack.h>
#include <shared/shr_template.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/PPD/ppd_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/Petra/PB_TM/pb_api_action_cmd.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/policer.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/dpp/wb_db_mirror.h>
#include <bcm_int/dpp/state.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/switch.h>

/*
 *  Ingress mirroring is done using a mirror action table.  This table allows
 *  control over not only the destination for the mirror, but traffic class,
 *  drop probability, metering, and counting.  The mirror action table is used
 *  by several APIs, including the mirroring APIs and the mirror field action.
 *
 *  In general, though, the BCM APIs don't provide a way to get to all of these
 *  extra controls, so most of it will be left in a state indicating these
 *  attributes of the mirror copies will remain unchanged from the original.
 *  We represent the internal features here, in the expectation that some
 *  customer will want them and so an appropriate API adjustment will be made
 *  later to take advantage.
 *
 *  Ingress mirroring is done through templates.  This means that if more than
 *  one thing wants a mirror with the same parameters, they will use the same
 *  template.  Templates (and the underlying resources) are recovered when
 *  nothing else wants to use a particular one.  This means that anything
 *  wanting a mirror should alloc it, and free it after use.  If the mirror
 *  needs to be changed while in use, the change API will try to do this
 *  in-place (if there is only one reference to the mirror) or will try to
 *  allocate a new mirror if it is already shared.  Template zero is reserved
 *  during init for use a 'no mirroring', and will not be edited.
 */




/******************************************************************************
 *
 *  Local functions and data
 */

/* Global lock, for protecting unit init and detach functions */
_bcm_petra_mirror_unit_data_t *_bcm_petra_mirror_unit_data[BCM_MAX_NUM_UNITS] = {0};

/*
 *  Some basic macros
 */

/* non recursive safe MACROS for give & take */
#define SOC_PETRA_MIRROR_UNIT_CHECK(_unit,_unitData) \
    if (!((_unit) >= 0 && ((_unit) < (BCM_MAX_NUM_UNITS)))) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("unit %d invalid"), _unit)); \
    } \
    if (!_bcm_petra_mirror_unit_data[_unit]) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("unit %d mirror not initialised"), _unit)); \
    } \
    (_unitData) = (_bcm_petra_mirror_unit_data[unit])
#define SOC_PETRA_MIRROR_LOCK_TAKE(_unitData) \
    if (sal_mutex_take((_unitData)->lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unable to take unit %d mirror lock"), (_unitData)->unit)); \
    }
#define SOC_PETRA_MIRROR_LOCK_GIVE(_unitData) \
    if (sal_mutex_give((_unitData)->lock)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unable to release unit %d mirror lock"), (_unitData)->unit)); \
    }

#define MIRROR_LOCK_DEFS _bcm_petra_mirror_unit_data_t *unitData = 0; int _in_lock = 0
#define MIRROR_LOCK_TAKE do { \
                             if (!_in_lock) { \
                                 SOC_PETRA_MIRROR_UNIT_CHECK(unit, unitData); \
                                 SOC_PETRA_MIRROR_LOCK_TAKE(unitData); \
                                 _in_lock = 1; \
                             } \
                         } while (0)
#define MIRROR_LOCK_GIVE do { \
                             if (_in_lock) { \
                                 SOC_PETRA_MIRROR_LOCK_GIVE(unitData); \
                                 _in_lock = 0;\
                             } \
                         } while (0)


#define SOC_PETRA_GET_BCM_SWITCH_USE_GPORT(unit) FALSE

#define _NON_LOCAL_PORT ((SOC_PPD_PORT)(-1))

/* mirror probability encoding constants */
#define ARAD_MIRROR_PROBABILITY_BITS ((SOC_IS_JERICHO(unit)) ? 24 : 16)
#define ARAD_MIRROR_PROBABILITY_DIVISOR (1 << ARAD_MIRROR_PROBABILITY_BITS)
#define ARAD_MIRROR_PROBABILITY_ALLWAYS (ARAD_MIRROR_PROBABILITY_DIVISOR - 1)

/*
 * For local ports, returns the local port number in out_loc_port and fail if the port is not valid.
 * For valid non local ports, return _NON_LOCAL_PORT.
 * For invalid non local ports, either return _NON_LOCAL_PORT or fail.
 */

STATIC int
_bcm_petra_get_local_valid_port(int unit,
                               bcm_port_t port,
                               SOC_PPD_PORT *out_loc_port)
{
    SOC_PPD_PORT loc_port;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    *out_loc_port = _NON_LOCAL_PORT;

    /* check if port is local and get the port. If so, does not check its validaty properly */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info)); 
    if (_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) {

        BCM_PBMP_ITER(gport_info.pbmp_local_ports,loc_port) {
            *out_loc_port = loc_port;
            break; /* If for some reason the bitmap contains multiple ports, use only the first one */
        }
        if (*out_loc_port == _NON_LOCAL_PORT) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("error getting local port")));
        }
        /* The Macro SOC_PORT_VALID checkes that port >= 0 it may be needed for signed varibles */
        /* coverity[unsigned_compare : FALSE] */
        if (!SOC_PORT_VALID(unit, loc_port)) { 
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("local port %lu is not valid"),(unsigned long)loc_port));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_petra_mirror_detach
 *  Purpose
 *     De-initialise the field APIs.
 *  Parameters
 *     (in) int unit = the unit number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
STATIC int
_bcm_petra_mirror_detach(int unit)
{
    _bcm_petra_mirror_unit_data_t *tempUnit = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    /* tear down this unit */
    if (_bcm_petra_mirror_unit_data[unit]) {
        /* copy pointer to unit data & zero it to keep other callers out */
        tempUnit = _bcm_petra_mirror_unit_data[unit];
        _bcm_petra_mirror_unit_data[unit] = NULL;
        /* make sure nobody else is using it right now */
        if (sal_mutex_take(tempUnit->lock, sal_mutex_FOREVER)) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "unable to take unit lock\n")));
            /* can't destroy it now, so put it back */
            _bcm_petra_mirror_unit_data[unit] = tempUnit;
        } else {
            /* took unit lock, destroy unit information */
            sal_mutex_destroy(tempUnit->lock);
           BCM_FREE(tempUnit);
        }
        BCM_EXIT;
    } else {
        BCM_EXIT;
    }
exit:
        BCMDNX_FUNC_RETURN;
}

#ifdef BCM_PETRAB_SUPPORT
/*
 *  Function
 *    _bcm_petra_mirror_port_check
 *  Purpose
 *    Verify that a port argument is valid for mirroring, translate that port
 *    to the proper form, and look up the local module ID.
 *  Arguments
 *    (in) int unit = unit number
 *    (in) bcm_port_t port = port to verify and translate
 *    (out) bcm_module_t *myModule = where to put local module ID
 *    (out) bcm_port_t *localPort = translated port ID
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 */
STATIC int
_bcm_petra_mirror_port_check(int unit,
                             bcm_port_t port,
                             bcm_module_t *myModule,
                             bcm_port_t *localPort)
{
    bcm_module_t mod;
    bcm_port_t lport;
    int result, core;
    pbmp_t ports_map;

    BCMDNX_INIT_FUNC_DEFS;
    result = bcm_petra_stk_my_modid_get(unit, &mod);
    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG("unable to get unit %d module ID: %d (%s)"), unit, result, _SHR_ERRMSG(result)));
    }
    if (BCM_GPORT_IS_LOCAL(port)) {
        lport = BCM_GPORT_LOCAL_GET(port);
    } else if (BCM_GPORT_IS_MODPORT(port)) {
        lport = BCM_GPORT_MODPORT_PORT_GET(port);
        if (mod != BCM_GPORT_MODPORT_MODID_GET(port)) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "unit %d module ID %d unable to access"
                                  " mirroring for port %08X because it is"
                                  " on another module (%d)\n"),
                       unit,
                       mod,
                       port,
                       BCM_GPORT_MODPORT_MODID_GET(port)));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }
    } else {
        /* just copy it */
        lport = port;
    }
    if (!BCM_GPORT_IS_SET(lport)) {
        BCM_PBMP_ASSIGN(ports_map, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(ports_map, PBMP_SFI_ALL(unit));
        if (!SOC_PBMP_MEMBER(ports_map, lport)) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "unit %d port %d(%08X) is not valid"
                                  " for mirroring\n"),
                       unit,
                       port,
                       port));
            BCM_RETURN_VAL_EXIT(BCM_E_PORT);
        }
    } 
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, lport, (uint32*)&lport, &core)));

    *myModule = mod;
    *localPort = lport;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_PETRAB_SUPPORT */

/*
 *  Function
 *    _bcm_petra_ingress_mirror_alloc
 *  Purpose
 *    Allocate an ingress mirror template according to the parameters
 *  Arguments
 *    (in) _bcm_petra_mirror_unit_data_t *unitData = unit information
 *    (in) bcm_module_t destMod = destination module
 *    (in) bcm_port_t destPort = destination port
 *    (out) int *mirrorId = where to put allocated ingress mirror ID
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Builds a mirror descriptor and uses the template manager to allocate a
 *    mirror template according to that decriptor.
 *
 *    Right now, only configures destination in mirror action so that is all it
 *    compares when looking for a matching template.
 */
STATIC int
_bcm_petra_ingress_mirror_alloc(_bcm_petra_mirror_unit_data_t* unitData,
                                bcm_module_t myModule,
                                bcm_module_t destMod,
                                bcm_port_t destPort,int* mirrorId)
{
    SOC_PB_ACTION_CMD_SNOOP_MIRROR_INFO mirrorInfo;
    unsigned int index;
    unsigned int free;
    int result;
    uint32 ppdr;
    bcm_gport_t dest;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (-1 == destMod) {
        /* -1 == destMod means use local module ID */
        destMod = myModule;
    }
    if (BCM_GPORT_IS_SET(destPort)) {
        /* it is a GPORT, so use that */
        dest = destPort;
    } else {
        /* it is not a GPORT, so it's a local module+port */
        BCM_GPORT_MODPORT_SET(dest, destMod, destPort);
    }
    
    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&mirrorInfo);
    /* always mirror */
    mirrorInfo.prob = 0x3FF; /* 1023/1023 probability of mirroring */
    /* fill in forwarding decision */
    result = _bcm_dpp_gport_to_tm_dest_info(unit,
                                            dest,
                                            &mirrorInfo.cmd.dest_id);
    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to compute dest info for unit"
                                " %d module %d port %d (GPORT %08X):"
                                " %d (%s)\n"),
                    unit,
                    destMod,
                    destPort,
                    dest,
                    result,
                    _SHR_ERRMSG(result)));
    }
    for (index = 0, free = unitData->ingressCount;
         index < unitData->ingressCount;
         index++) {
        if (unitData->ingress[index].refCount) {
            /* this template is in use */
            if (0 == sal_memcmp(&(unitData->ingress[index].mirrorInfo.cmd.dest_id),
                                &(mirrorInfo.cmd.dest_id),
                                sizeof(mirrorInfo.cmd.dest_id))) {
                /* this template is in use and points to the same place */
                break;
            }
        } else { /* if (unitData->ingress[index].refCount) */
            /* this template is not in use */
            if (unitData->ingressCount <= free) {
                /* we have not found a free one yet, keep this one */
                free = index;
            }
        } /* if (unitData->ingress[index].refCount) */
    } /* for (all ingress templates) */
    if (unitData->ingressCount > index) {
        /* found one that matches */
        *mirrorId = index;
        unitData->ingress[index].refCount++;
#ifdef BCM_WARM_BOOT_SUPPORT
        result =_bcm_dpp_wb_mirror_update_refCount_state(unit,index);
        if (BCM_E_NONE != result) {
            BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to update WB refCount unit"
                                                       " %d index %d : %d (%s)\n"),
                                      unit,
                                      index,
                                      result,
                                      _SHR_ERRMSG(result)));
        }
#endif
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d reused ingress mirror"
                              " template %d\n"),
                   unit,
                   index));
    } else if (unitData->ingressCount > free) {
        /* found a free one */
        ppdr = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_set,(unit, free, &mirrorInfo));
        result = handle_sand_result(ppdr);
        if (BCM_E_NONE == result) {
            LOG_DEBUG(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "unit %d allocated ingress mirror"
                                  " template %d\n"),
                       unit,
                       free));
            *mirrorId = free;
            unitData->ingress[free].refCount++;
#ifdef BCM_WARM_BOOT_SUPPORT
        result = _bcm_dpp_wb_mirror_update_refCount_state(unit, free);
        if (BCM_E_NONE != result) {
            BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to update WB refCount unit"
                                                       " %d free %d : %d (%s)\n"),
                                      unit,
                                      free,
                                      result,
                                      _SHR_ERRMSG(result)));
        }
#endif
            /* set template local copy */
            sal_memcpy(&(unitData->ingress[free].mirrorInfo),
                       &mirrorInfo,
                       sizeof(unitData->ingress[free].mirrorInfo));
        } else {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unit %d unable to set ingress mirror"
                                    " template %d: %d (%s)\n"),
                        unit,
                        free,
                        result,
                        _SHR_ERRMSG(result)));
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d has no available ingress mirror"
                              " template\n"),
                   unit));
        BCM_RETURN_VAL_EXIT(BCM_E_RESOURCE);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_petra_ingress_mirror_reference
 *  Purpose
 *    Add a reference to an ingress mirror template
 *  Arguments
 *    (in) _bcm_petra_mirror_unit_data_t *unitData = unit information
 *    (in) int mirrorId = mirror template ID for the reference
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This is intended to be used as part of warm boot or similar.  When other
 *    modules are rebuilding their state, they call this function as they
 *    encounter references to mirror templates.  This allows the mirror module
 *    to know how many outstanding references there are to each template.
 *
 *    Template 0 is reserved (for no-mirror) and should never be freed.
 */
STATIC int
_bcm_petra_ingress_mirror_reference(_bcm_petra_mirror_unit_data_t *unitData,
                                    int mirrorId)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int result = BCM_E_NONE;
#endif
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 == mirrorId) {
        /* template zero is reserved and must not be freed */
        BCM_EXIT;
    }
    unitData->ingress[mirrorId].refCount++;
#ifdef BCM_WARM_BOOT_SUPPORT
    result = _bcm_dpp_wb_mirror_update_refCount_state(unit, mirrorId);
    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to update WB refCount unit"
                                                   " %d mirrorId %d : %d (%s)\n"),
                                  unit,
                                  mirrorId,
                                  result,
                                  _SHR_ERRMSG(result)));
    }
#endif

    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "unit %d add one user to ingress mirror"
                          " template %d\n"),
               unit,
               mirrorId));
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_petra_ingress_mirror_free
 *  Purpose
 *    Free a specified ingress mirror
 *  Arguments
 *    (in) _bcm_petra_mirror_unit_data_t *unitData = unit information
 *    (in) int mirrorId = ingress mirror ID to free
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Frees (one reference to) an ingress mirror template.
 *
 *    Template 0 is reserved (for no-mirror) and should never be freed.
 */
STATIC int
_bcm_petra_ingress_mirror_free(_bcm_petra_mirror_unit_data_t *unitData,
                               int mirrorId)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int result = BCM_E_NONE;
#endif
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (0 == mirrorId) {
        /* template zero is reserved and must not be freed */
        BCM_EXIT;
    }
    if (unitData->ingress[mirrorId].refCount) {
        unitData->ingress[mirrorId].refCount--;
#ifdef BCM_WARM_BOOT_SUPPORT
        result = _bcm_dpp_wb_mirror_update_refCount_state(unit, mirrorId);
        if (BCM_E_NONE != result) {
            BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to update WB refCount unit"
                                                       " %d mirrorId %d : %d (%s)\n"),
                                      unit,
                                      mirrorId,
                                      result,
                                      _SHR_ERRMSG(result)));
        }
#endif

        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d remove %s user from ingress mirror"
                              " template %d\n"),
                   unit,
                   unitData->ingress[mirrorId].refCount?"one":"last",
                   mirrorId));
        
        BCM_EXIT;
    } else {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror template %d is not"
                              " currently in use\n"),
                   unit,
                   mirrorId));
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }
exit:
        BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_petra_ingress_mirror_change
 *  Purpose
 *    Change an ingress mirror
 *  Arguments
 *    (in) _bcm_petra_mirror_unit_data_t *unitData = unit information
 *    (in) int oldMirrorId = old ingress mirror ID
 *    (in) bcm_module_t destMod = destination module
 *    (in) bcm_port_t destPort = destination port
 *    (out) int *newMirrorId = where to put new ingress mirror ID
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This will only edit the template in-place if there is exactly one
 *    reference to it.  If there is more than one reference to the template, it
 *    will allocate a replacement template and provide that, releasing one
 *    reference to the old template.  If success and the oldMirrorId and
 *    newMirrorId values are the same, the template was edited in place and
 *    there should be no need to make changes to the reference.
 *
 *    Once the other table (field entry, port descriptor, &c) has been updated
 *    to use the new mirror template, the old one must be freed (this is not
 *    done automatically here).  If such edits fail, the new mirror template
 *    should be freed instead of the old one.  This is done so that a template
 *    will not be overwritten if there are other things happening or more than
 *    one subsystem updates mirror targets concurrently.
 */
STATIC int
_bcm_petra_ingress_mirror_change(_bcm_petra_mirror_unit_data_t* unitData,
                                 int oldMirrorId,
                                 bcm_module_t myModule,
                                 bcm_module_t destMod,
                                 bcm_port_t destPort,
                                 int *newMirrorId)
{
    SOC_PB_ACTION_CMD_MIRROR_INFO mirrorInfo;
    unsigned int index;
    unsigned int free;
    int result;
    uint32 ppdr;
    bcm_gport_t dest;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (!(unitData->ingress[oldMirrorId].refCount)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d can't change non-used mirror template"
                              " %d\n"),
                   unit,
                   oldMirrorId));
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }
    /* at least one reference, so old template in use */
    if (-1 == destMod) {
        /* -1 == destMod means use local module ID */
        destMod = myModule;
    }
    if (BCM_GPORT_IS_SET(destPort)) {
        /* it is a GPORT, so use that */
        dest = destPort;
    } else {
        /* it is not a GPORT, so it's a local module+port */
        BCM_GPORT_MODPORT_SET(dest, destMod, destPort);
    }
    
    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&mirrorInfo);
    /* always mirror */
    mirrorInfo.prob = 0x3FF; /* 1023/1023 probability of mirroring */
    /* fill in forwarding decision */
    result = _bcm_dpp_gport_to_tm_dest_info(unit,
                                            dest,
                                            &mirrorInfo.cmd.dest_id);
    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to compute dest info for unit"
                                " %d module %d port %d (GPORT %08X):"
                                " %d (%s)\n"),
                    unit,
                    destMod,
                    destPort,
                    dest,
                    result,
                    _SHR_ERRMSG(result)));
    }
    /* see if there is a match for the new settings */
    for (index = 0, free = unitData->ingressCount;
         index < unitData->ingressCount;
         index++) {
        if (unitData->ingress[index].refCount) {
            /* this template is in use */
            if (0 == sal_memcmp(&(unitData->ingress[index].mirrorInfo.cmd.dest_id),
                                &(mirrorInfo.cmd.dest_id),
                                sizeof(mirrorInfo.cmd.dest_id))) {
                /* this template is in use and points to same place */
                break;
            }
        } else { /* if (unitData->ingress[index].refCount) */
            /* this template is not in use */
            if (unitData->ingressCount <= free) {
                /* we have not found a free one yet, keep this one */
                free = index;
            }
        } /* if (unitData->ingress[index].refCount) */
    } /* for (all ingress templates) */
    if (unitData->ingressCount > index) {
        /* found one that matches, use it */
        *newMirrorId = index;
        /* shift reference to new template */
        unitData->ingress[index].refCount++;
#ifdef BCM_WARM_BOOT_SUPPORT
        result = _bcm_dpp_wb_mirror_update_refCount_state(unit, index);
        if (BCM_E_NONE != result) {
            BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to update WB refCount unit"
                                                       " %d index %d : %d (%s)\n"),
                                      unit,
                                      index,
                                      result,
                                      _SHR_ERRMSG(result)));
        }
#endif

        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d reused ingress mirror"
                              " template %d\n"),
                   unit,
                   index));
    } else if ((1 == unitData->ingress[oldMirrorId].refCount) ||
               (unitData->ingressCount > free)) {
        /* no existing match, == 1 reference or free template found */
        if (1 == unitData->ingress[oldMirrorId].refCount) {
            /* can update in place, so use that instead of free */
            free = oldMirrorId;
        }
        ppdr = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_set,(unit, free, &mirrorInfo));
        result = handle_sand_result(ppdr);
        if (BCM_E_NONE != result) {
            BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unit %d unable to set ingress"
                                    " mirror template %d: %d (%s)\n"),
                        unit,
                        free,
                        result,
                        _SHR_ERRMSG(result)));
        }
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d %s ingress mirror"
                              " template %d\n"),
                   unit,
                   (free == oldMirrorId)?"updated":"allocated",
                   free));
        *newMirrorId = free;
        /* add reference to new template */
        unitData->ingress[free].refCount++;
#ifdef BCM_WARM_BOOT_SUPPORT
        result = _bcm_dpp_wb_mirror_update_refCount_state(unit, free);
        if (BCM_E_NONE != result) {
            BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to update WB refCount unit"
                                                       " %d free %d : %d (%s)\n"),
                                      unit,
                                      free,
                                      result,
                                      _SHR_ERRMSG(result)));
        }
#endif
        /* set new template local copy */
        sal_memcpy(&(unitData->ingress[free].mirrorInfo),
                   &mirrorInfo,
                   sizeof(unitData->ingress[free].mirrorInfo));
    } else {
        /* no existing match, >1 reference and no free templates */
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d has no available ingress"
                              " mirror template\n"),
                   unit));
        BCM_RETURN_VAL_EXIT(BCM_E_RESOURCE);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_petra_ingress_mirror_get
 *  Purpose
 *    Get information about an ingress mirror
 *  Arguments
 *    (in) _bcm_petra_mirror_unit_data_t *unitData = unit information
 *    (in) int mirrorId = ingress mirror ID
 *    (out) bcm_module_t *destMod = where to put destination module
 *    (out) bcm_port_t *destPort = where to put destination port
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This gets the cached copy, not the hardware state.
 */
STATIC int
_bcm_petra_ingress_mirror_get(_bcm_petra_mirror_unit_data_t *unitData,
                              int mirrorId,
                              bcm_module_t *destMod,
                              bcm_port_t *destPort)
{
    SOC_PB_ACTION_CMD_MIRROR_INFO *mirrorInfo;
    int result;
    bcm_gport_t dest;
    bcm_module_t myModule;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    result = bcm_petra_stk_my_modid_get(unit, &myModule);
    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to get unit %d module ID: %d (%s)\n"),
                    unit,
                    result,
                    _SHR_ERRMSG(result)));
    }
    if (!(unitData->ingress[mirrorId].refCount)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d mirror template %d not in use\n"),
                   unit,
                   mirrorId));
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }
    /* at least one reference, so template in use */
    mirrorInfo = &(unitData->ingress[mirrorId].mirrorInfo);
    result = _bcm_dpp_gport_from_tm_dest_info(unit,
                                              &dest,
                                              &(mirrorInfo->cmd.dest_id));

    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG_NO_UNIT("unable to compute GPORT from unit %d"
                                " ingress template %d dest info"
                                " %d.%d(%08X): %d (%s)\n"),
                    unit,
                    mirrorId,
                    mirrorInfo->cmd.dest_id.type,
                    mirrorInfo->cmd.dest_id.id,
                    mirrorInfo->cmd.dest_id.id,
                    result,
                    _SHR_ERRMSG(result)));
    }
    if (SOC_PETRA_GET_BCM_SWITCH_USE_GPORT(unit)) {
        
    if (BCM_GPORT_IS_MODPORT(dest)) {
        *destMod = BCM_GPORT_MODPORT_MODID_GET(dest);
    } else {
        *destMod = myModule;
    }
        /* in GPORT mode, always provide GPORT as-is */
    *destPort = dest;
    } else { /* if (SOC_PETRA_GET_BCM_SWITCH_USE_GPORT(unit)) */
        /* Convert anything mappable as such to module,port */
        
        if (BCM_GPORT_IS_LOCAL(dest)) {
            *destMod = myModule;
            *destPort = BCM_GPORT_LOCAL_GET(dest);
        } else if (BCM_GPORT_IS_MODPORT(dest)) {
            *destMod = BCM_GPORT_MODPORT_MODID_GET(dest);
            *destPort = BCM_GPORT_MODPORT_PORT_GET(dest);
        } else if (BCM_GPORT_IS_EGRESS_MODPORT(dest)) {
            *destMod = BCM_GPORT_EGRESS_MODPORT_MODID_GET(dest);
            *destPort = BCM_GPORT_EGRESS_MODPORT_PORT_GET(dest);
        } else {
            *destMod = myModule;
            *destPort = dest;
        }
    } /* if (SOC_PETRA_GET_BCM_SWITCH_USE_GPORT(unit)) */

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_is_mirror_dest_created
 * Purpose:
 *      Check if given mirror destination is created (allocated by the allocation manager)
 * Parameters:
 *      unit                   (IN) Deivce id 
 *      mirr_action_profile_id (IN) mirror destination/profile ID.
 *      is_created            (OUT) is destination created (allocated)
 */
int
 _bcm_petra_is_mirror_dest_created(int unit, uint32 mirr_action_profile_id, int *is_created)
{
	uint32 ref_count;
    /* Check if this mirror destination exists */
    int res; 
    BCMDNX_INIT_FUNC_DEFS;

	res = _bcm_dpp_am_template_mirror_action_profile_ref_get(unit, mirr_action_profile_id, &ref_count); /* easy - get the ref count*/
    BCMDNX_IF_ERR_EXIT(res);
	*is_created= ref_count>0; 

exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_PETRAB_SUPPORT
/*
 *  Function
 *     _bcm_petrab_mirror_port_outbound_get
 *  Purpose
 *     De-initialise the field APIs.
 *  Parameters
 *     (in) unit = the unit number
 *     (in) port = port to verify and translate
 *     (out) mirror_profile = mirror profile
 *     (out) mirror_port = whether the mirror is enabled on port
 *     (out) mirror_port_vlan = the number that mirror is enabled on port-vlan
 *     (out) mirror_enable = whether mirror is enabled
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
STATIC int 
_bcm_petrab_mirror_port_outbound_get(int unit,
                                    bcm_port_t port,
                                    uint8 *mirror_profile,
                                    uint8 *mirror_port,
                                    uint8 *mirror_port_vlan,
                                    uint8 *mirror_enable)
{
    int rv = 0;
    uint32 result;
    uint32 vid_idx;
    uint8 mirror_num = 0;
    uint8 is_port_vlan_exists;
    
    SOC_TMC_PORT_OUTBOUND_MIRROR_INFO outbound_mirror_info;    
    SOC_PPD_EG_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;

    BCMDNX_INIT_FUNC_DEFS; 
    *mirror_profile = *mirror_port = *mirror_port_vlan = *mirror_enable = 0;
    result = soc_petra_ports_mirror_outbound_get(
          unit,
          port,
          &outbound_mirror_info);
    BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));
    *mirror_enable = outbound_mirror_info.enable; 
    
    rv = _bcm_sw_db_outbound_mirror_port_profile_get(
          unit,
          port,
          mirror_profile
          );
    BCMDNX_IF_ERR_EXIT(rv);  

    /* scan whether the mirror profile used by the port-vlan */
    for (vid_idx = 0; vid_idx < SOC_PB_PORT_EG_MIRROR_NOF_VID_MIRROR_INDICES; vid_idx++) {
        result = soc_pb_sw_db_outbound_mirror_port_vlan_is_exist_get(
                unit,
                port,
                vid_idx,
                &is_port_vlan_exists
              );
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));  
        if (is_port_vlan_exists) {
            mirror_num++;
        }
    }
    *mirror_port_vlan = mirror_num;
    /* scan whether the mirror profile used by the port */
    SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_dflt_get(unit, port, &dflt_mirroring_info));
    if (dflt_mirroring_info.enable_mirror) {
        *mirror_port = 1;
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

} 


/*
 *  Function
 *     _bcm_petrab_mirror_port_outbound_set
 *  Purpose
 *     De-initialise the field APIs.
 *  Parameters
 *     (in) unit = the unit number
 *     (in) port = port to verify and translate
 *     (in) mirror_profile = mirror profile
 *     (in) mirror_enable = mirror enable
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
STATIC int 
_bcm_petrab_mirror_port_outbound_set(int unit,
                                    bcm_port_t port,
                                    uint8 mirror_profile,
                                    uint8 mirror_enable)
{   
    int rv = 0;
    uint32 result;
    
    uint32 recycle_port;
    uint32 recycle_channel;
    SOC_SAND_SUCCESS_FAILURE failure = SOC_SAND_SUCCESS;
    SOC_TMC_PORT2IF_MAPPING_INFO port2if_mapping_info;
    SOC_TMC_PORT_PP_PORT_INFO pp_port_info;    
    SOC_TMC_PORTS_FORWARDING_HEADER_INFO forwarding_header_info;
    SOC_TMC_PORT_OUTBOUND_MIRROR_INFO outbound_mirror_info;
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO mirror_info;

    BCMDNX_INIT_FUNC_DEFS;
    
    BCM_DPP_UNIT_CHECK(unit);  

    if (mirror_enable) {    
        /* Step 1: Alloc the context and channel */
        result = soc_pb_ports_reassembly_context_and_recycle_channel_alloc_unsafe(unit, &recycle_port, &recycle_channel);
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));  
        /* Step 2: port to interface map set */       
        SOC_TMC_PORT2IF_MAPPING_INFO_clear(&port2if_mapping_info);
        port2if_mapping_info.if_id = SOC_TMC_IF_ID_RCY;
        port2if_mapping_info.channel_id = recycle_channel;    
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_to_interface_map_set,(unit, recycle_port, SOC_TMC_PORT_DIRECTION_INCOMING, &port2if_mapping_info));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));
            
        /* Step 3: port header type set */
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_header_type_set,(unit, recycle_port, SOC_TMC_PORT_DIRECTION_INCOMING, SOC_TMC_PORT_HEADER_TYPE_RAW));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));

        /* Step 4: pb port to pp port map set */
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_to_pp_port_map_set,(unit, (soc_port_t)recycle_port, SOC_TMC_PORT_DIRECTION_INCOMING));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));        
        
        /* Step 5: pb port pp port set */
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_pp_port_get,(unit, recycle_port, &pp_port_info));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));  
        
        pp_port_info.header_type = SOC_TMC_PORT_HEADER_TYPE_RAW;
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_pp_port_set,(unit, recycle_port, 0 /*core*/, &pp_port_info, &failure));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));        

        /* Step 6: get the mirror profile */
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_get,(unit, mirror_profile, &mirror_info));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));            

        /* Step 7: pb port forwarding header get */
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_forwarding_header_get,(unit, recycle_port, &forwarding_header_info));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));         
        
        forwarding_header_info.destination.type = SOC_TMC_DEST_TYPE_SYS_PHY_PORT;
        forwarding_header_info.destination.id = mirror_info.cmd.dest_id.id;
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_port_forwarding_header_set,(unit, recycle_port, &forwarding_header_info));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result)); 

        /* Step 8: port mirror outbound enable */
        SOC_TMC_PORT_OUTBOUND_MIRROR_INFO_clear(&outbound_mirror_info);
        outbound_mirror_info.ifp_id = recycle_port;
        outbound_mirror_info.enable = mirror_enable;
        outbound_mirror_info.skip_port_deafult_enable = TRUE;    
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_mirror_outbound_set,(unit, port, &outbound_mirror_info));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));        
    } else {
        mirror_profile = 0;
        /* Step 1: Get the IFP */
        result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_mirror_outbound_get,(unit, port, &outbound_mirror_info));
        BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));
        
        /* the calling to mbcm_dpp_ports_mirror_outbound_get filling the data of outbound_mirror_info*/
        /* coverity[uninit_use:FALSE] */
        if (outbound_mirror_info.enable) {
            /* Step 2: Free the context and channel */

            /* the calling to mbcm_dpp_ports_mirror_outbound_get filling the data of outbound_mirror_info*/
            /* coverity[uninit_use_in_call:FALSE] */
            result = soc_pb_ports_reassembly_context_and_recycle_channel_free_unsafe(unit, outbound_mirror_info.ifp_id);
            BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));              

            /* Step 3: port mirror outbound disable */
            outbound_mirror_info.enable = FALSE;
            outbound_mirror_info.skip_port_deafult_enable = TRUE;    
            result = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ports_mirror_outbound_set,(unit, port, &outbound_mirror_info));
            BCMDNX_IF_ERR_EXIT(soc_sand_get_error_code_from_error_word(result));            
        }
    }
    
    /* Add to SW */
    rv = _bcm_sw_db_outbound_mirror_port_profile_set(
          unit,
          port,
          mirror_profile
          );
    BCMDNX_IF_ERR_EXIT(rv);   
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;

}  
#endif /* BCM_PETRAB_SUPPORT */


/******************************************************************************
 *
 *  Exposed helper functions (see function description comments in .h file)
 */

int
bcm_petra_ingress_mirror_alloc(int unit,
                               bcm_module_t myModule,
                               bcm_module_t destMod,
                               bcm_port_t destPort,
                               int *mirrorId)
{
    _bcm_petra_mirror_unit_data_t *unitData;
    int result;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PETRA_MIRROR_UNIT_CHECK(unit, unitData);
    SOC_PETRA_MIRROR_LOCK_TAKE(unitData);
    result = _bcm_petra_ingress_mirror_alloc(unitData,
                                             myModule,
                                             destMod,
                                             destPort,
                                             mirrorId);
    SOC_PETRA_MIRROR_LOCK_GIVE(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ingress_mirror_reference(int unit,
                                   int mirrorId)
{
    _bcm_petra_mirror_unit_data_t *unitData;
    int result;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PETRA_MIRROR_UNIT_CHECK(unit, unitData);
    SOC_PETRA_MIRROR_LOCK_TAKE(unitData);
    if ((0 > mirrorId) || (unitData->ingressCount <= mirrorId)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror %d is not valid\n"),
                   unit,
                   mirrorId));
        result = BCM_E_NOT_FOUND;
    } else {
        result = _bcm_petra_ingress_mirror_reference(unitData,
                                                     mirrorId);
    }
    SOC_PETRA_MIRROR_LOCK_GIVE(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ingress_mirror_free(int unit,
                              int mirrorId)
{
    _bcm_petra_mirror_unit_data_t *unitData;
    int result = BCM_E_NONE; BCMDNX_INIT_FUNC_DEFS;
                             /* ah, optimism */

    SOC_PETRA_MIRROR_UNIT_CHECK(unit, unitData);
    SOC_PETRA_MIRROR_LOCK_TAKE(unitData);
    if ((0 > mirrorId) || (unitData->ingressCount <= mirrorId)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror %d is not valid\n"),
                   unit,
                   mirrorId));
        result = BCM_E_NOT_FOUND;
    } else {
        result = _bcm_petra_ingress_mirror_free(unitData,
                                                mirrorId);
    }
    SOC_PETRA_MIRROR_LOCK_GIVE(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ingress_mirror_change(int unit,
                                int oldMirrorId,
                                bcm_module_t myModule,
                                bcm_module_t destMod,
                                bcm_port_t destPort,
                                int *newMirrorId)
{
    _bcm_petra_mirror_unit_data_t *unitData;
    int result = BCM_E_NONE; BCMDNX_INIT_FUNC_DEFS;
                             /* ah, optimism */

    SOC_PETRA_MIRROR_UNIT_CHECK(unit, unitData);
    SOC_PETRA_MIRROR_LOCK_TAKE(unitData);
    if ((0 > oldMirrorId) || (unitData->ingressCount <= oldMirrorId)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror %d is not valid\n"),
                   unit,
                   oldMirrorId));
        result = BCM_E_NOT_FOUND;
    } else {
        result = _bcm_petra_ingress_mirror_change(unitData,
                                                  oldMirrorId,
                                                  myModule,
                                                  destMod,
                                                  destPort,
                                                  newMirrorId);
    }
    SOC_PETRA_MIRROR_LOCK_GIVE(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ingress_mirror_get(int unit,
                             int mirrorId,
                             bcm_module_t *destMod,
                             bcm_port_t *destPort)
{
    _bcm_petra_mirror_unit_data_t *unitData;
    int result = BCM_E_NONE; BCMDNX_INIT_FUNC_DEFS;
                             /* ah, optimism */

    SOC_PETRA_MIRROR_UNIT_CHECK(unit, unitData);
    SOC_PETRA_MIRROR_LOCK_TAKE(unitData);
    if ((1 > mirrorId) || (unitData->ingressCount <= mirrorId)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror %d is not valid\n"),
                   unit,
                   mirrorId));
        result = BCM_E_NOT_FOUND;
    } else {
        result = _bcm_petra_ingress_mirror_get(unitData,
                                               mirrorId,
                                               destMod,
                                               destPort);
    }
    SOC_PETRA_MIRROR_LOCK_GIVE(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


/******************************************************************************
 *
 *  API functions
 */

/*
 *  Function
 *     bcm_petra_mirror_init
 *  Purpose
 *     Initialise the mirror APIs.
 *  Parameters
 *     (in) int unit = the unit number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_init(int unit)
{
    int result = BCM_E_NONE;
    size_t allocSize = 0;
    _bcm_petra_mirror_unit_data_t *tempUnit = NULL;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "(%d) enter\n"), unit));
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    /* If the unit is already initialised, tear it down */
    if (_bcm_petra_mirror_unit_data[unit]) {
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit already init; detach first\n")));
                   result = _bcm_petra_mirror_detach(unit);
        if (BCM_E_NONE == result) {
            if (_bcm_petra_mirror_unit_data[unit]) {
                /* should not see this */
                result = BCM_E_INTERNAL;
            }
        }
    } else {
        /* nothing to destroy, so successfully didn't do any destruction */
        result = BCM_E_NONE;
    }

    if (BCM_E_NONE == result) {
        /* compute the size of the unit data */
        allocSize = sizeof(*(_bcm_petra_mirror_unit_data[0]));
        allocSize += (sizeof(*(tempUnit->ingress)) * SOC_PETRA_IGR_MIRRORS(unit));
        /* allocate the unit data */
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "allocate %u bytes for unit %d mirror state\n"),
                   (uint32)allocSize,
                   unit));
        BCMDNX_ALLOC(tempUnit, allocSize, "soc_petra_mirror_unit_data");
        if (tempUnit) {
            /* initialise the unit data */
            sal_memset(tempUnit, 0x00, allocSize);
            tempUnit->ingress = (_bcm_petra_mirror_data_t*)(&(tempUnit[1]));
            tempUnit->ingressCount = SOC_PETRA_IGR_MIRRORS(unit);
            tempUnit->unit = unit;
            tempUnit->lock = sal_mutex_create("soc_petra_mirror_unit_lock");
            if (!tempUnit->lock) {
                LOG_ERROR(BSL_LS_BCM_MIRROR,
                          (BSL_META_U(unit,
                                      "could not create unit %d"
                                      " mirror lock\n"),
                           unit));
                result = BCM_E_RESOURCE;
            } else { /* if (!tempUnit->lock) */
                /* reserve one action for not mirroring */
                SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&(tempUnit->ingress[0].mirrorInfo));
                tempUnit->ingress[0].mirrorInfo.prob = 0;
                tempUnit->ingress[0].refCount = 2;
            } /* if (!tempUnit->lock) */
        } else {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "could not allocate %u bytes for unit %d"
                                  " mirror state\n"),
                       (uint32)allocSize,
                       unit));
            result = BCM_E_MEMORY;
        }
    }

    /* commit if success, clean up if failure */
    if (BCM_E_NONE == result) {
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "commit unit %d init\n"), unit));
        SOC_DPP_STATE(unit)->mirror_state =_bcm_petra_mirror_unit_data[unit] = tempUnit;
    } else {
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "back out unit %d init\n"), unit));
        if (tempUnit) {
            if (tempUnit->lock) {
                LOG_DEBUG(BSL_LS_BCM_MIRROR,
                          (BSL_META_U(unit,
                                      "destroy lock\n")));
                sal_mutex_destroy(tempUnit->lock);
            }
            LOG_DEBUG(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "free working unit descriptor\n")));
           BCM_FREE(tempUnit);
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_mirror_state_init(unit));
#endif

    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "(%d) return %d (%s)\n"),
               unit,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_detach
 *  Purpose
 *     Initialise the mirror APIs.
 *  Parameters
 *     (in) int unit = the unit number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_detach(int unit)
{
    int result;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "(%d) enter\n"), unit));
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    result = _bcm_petra_mirror_detach(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_mirror_state_deinit(unit));
#endif


    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "(%d) return %d (%s)\n"),
               unit,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_set
 *  Purpose
 *     Set mirroring mode and destination for a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) bcm_module_t dest_mod = destination module for mirror frames
 *     (in) bcm_port_t dest_port = destination port for mirror frames
 *     (in) uint32 flags = mirror control flags
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Ingress mirroring sets the default mirror profile for both tagged and
 *     untagged frames on the port, to the same value, so only one of these
 *     values is used when deciding (for example) whether mirroring is active
 *     on the specified port.
 */

int
bcm_petra_mirror_port_set(int unit,
                          bcm_port_t port,
                          bcm_module_t dest_mod,
                          bcm_port_t dest_port,
                          uint32 flags)
{
#ifdef BCM_PETRAB_SUPPORT
    _bcm_petra_mirror_unit_data_t *unitData;
    bcm_module_t myModule;
    int result;
    int auxRes;
    int mirrorId;
    int mirrorIdOld;
    int mirrorMode;
    bcm_port_t localPort;
    uint32 ppdr;
    SOC_PPD_LLP_MIRROR_PORT_DFLT_INFO defaultMirror;
#endif

    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_mode_get(unit, &mirrorMode));
        if (mirrorMode != BCM_PETRA_MIRROR_MODE_PORT_SET) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "The current mirror mode is bcm_petra_mirror_destination_create\n")));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }        
        
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "(%d,%d(%08X),%d,%d(%08X),%08X) enter\n"),
                   unit,
                   port,
                   port,
                   dest_mod,
                   dest_port,
                   dest_port,
                   flags));
        _BCM_DPP_SWITCH_API_START(unit);
        SOC_PETRA_MIRROR_UNIT_CHECK(unit, unitData);
        if (flags & (~(BCM_MIRROR_PORT_INGRESS))) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "unit %d does not support mirror_port flags"
                                  " %08X\n"),
                       unit,
                       flags & (~(BCM_MIRROR_PORT_INGRESS))));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }
        result = _bcm_petra_mirror_port_check(unit, port, &myModule, &localPort);
        BCMDNX_IF_ERR_EXIT(result);
        SOC_PETRA_MIRROR_LOCK_TAKE(unitData);

        /* deal with ingress mirorring first */
        ppdr = soc_ppd_llp_mirror_port_dflt_get((unit),
                                            localPort,
                                            &defaultMirror);
        result = handle_sand_result(ppdr);
        if (BCM_E_NONE == result) {
            mirrorIdOld = defaultMirror.tagged_dflt;
            /* decide on new mirror ID to use */
            if (flags & BCM_MIRROR_PORT_INGRESS) {
                /* ingress mirroring enabled */
                if (0 == mirrorIdOld) {
                    /* port was not ingress mirroring */
                    result = _bcm_petra_ingress_mirror_alloc(unitData,
                                                             myModule,
                                                             dest_mod,
                                                             dest_port,
                                                             &mirrorId);
                } else {
                    /* port was already ingress mirroring */
                    result = _bcm_petra_ingress_mirror_change(unitData,
                                                              mirrorIdOld,
                                                              myModule,
                                                              dest_mod,
                                                              dest_port,
                                                              &mirrorId);
                }
            } else {
                /* ingress mirroring disabled */
                mirrorId = 0;
            }
            /* write the new mirror ID to the port */
            if (BCM_E_NONE == result) {
                defaultMirror.tagged_dflt = mirrorId;
                defaultMirror.untagged_dflt = mirrorId;
                ppdr = soc_ppd_llp_mirror_port_dflt_set((unit),
                                                    localPort,
                                                    &defaultMirror);
                result = handle_sand_result(ppdr);
                if (BCM_E_NONE == result) {
                    /* get rid of old mirror ID */
                    mirrorId = mirrorIdOld;
                    auxRes = _bcm_petra_ingress_mirror_free(unitData, mirrorId);
                    if(auxRes != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_MIRROR,
                                  (BSL_META_U(unit,
                                              "_bcm_petra_ingress_mirror_free failed. unit %d port %d(%08X)"
                                              " ingress profile %d"
                                              ": %d (%s)\n"),
                                   unit,
                                   port,
                                   port,
                                   mirrorId,
                                   auxRes,
                                   _SHR_ERRMSG(auxRes)));
                    }
                } else { /* if (BCM_E_NONE == result) */
                    /* get rid of new mirror ID */
                    auxRes = _bcm_petra_ingress_mirror_free(unitData, mirrorId);
                    if(auxRes != BCM_E_NONE) {
                        LOG_ERROR(BSL_LS_BCM_MIRROR,
                                  (BSL_META_U(unit,
                                              "_bcm_petra_ingress_mirror_free failed. unit %d port %d(%08X)"
                                              " ingress profile %d"
                                              ": %d (%s)\n"),
                                   unit,
                                   port,
                                   port,
                                   mirrorId,
                                   auxRes,
                                   _SHR_ERRMSG(auxRes)));
                    }
                    LOG_ERROR(BSL_LS_BCM_MIRROR,
                              (BSL_META_U(unit,
                                          "unable to set unit %d port %d(%08X)"
                                          " to use ingress profile %d"
                                          ": %d (%s)\n"),
                               unit,
                               port,
                               port,
                               mirrorId,
                               result,
                               _SHR_ERRMSG(result)));
                } /* if (BCM_E_NONE == result) */
            } /* if (BCM_E_NONE == result) */
        } else { /* if (BCM_E_NONE == result) */
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "unable to read ingress mirorring state"
                                  " for unit %d port %d(%08X): %d (%s)\n"),
                       unit,
                       port,
                       port,
                       result,
                       _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
        

        SOC_PETRA_MIRROR_LOCK_GIVE(unitData);
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "(%d,%d(%08X),%d,%d(%08X),%08X) return %d (%s)\n"),
                   unit,
                   port,
                   port,
                   dest_mod,
                   dest_port,
                   dest_port,
                   flags,
                   result,
                   _SHR_ERRMSG(result)));
        BCMDNX_IF_ERR_EXIT(result);
    }
    else
#endif
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
    }
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_get
 *  Purpose
 *     Get mirroring mode and destination for a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (out) bcm_module_t *dest_mod = where to put destination module
 *     (out) bcm_port_t *dest_port = where to put destination port
 *     (out) uint32 *flags = where to put mirror control flags
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Ingress mirroring sets the default mirror profile for both tagged and
 *     untagged frames on the port, to the same value, so only one of these
 *     values is used when deciding (for example) whether mirroring is active
 *     on the specified port.
 */
int
bcm_petra_mirror_port_get(int unit,
                          bcm_port_t port,
                          bcm_module_t *dest_mod,
                          bcm_port_t *dest_port,
                          uint32 *flags)
{
#ifdef BCM_PETRAB_SUPPORT
    _bcm_petra_mirror_unit_data_t *unitData;
    int result;
    int mirrorMode;
    uint32 cFlags = 0;
    bcm_module_t cDestMod = -1;
    bcm_module_t myModule;
    bcm_port_t localPort;
    bcm_port_t cDestPort = BCM_GPORT_INVALID;
    uint32 ppdr;
    SOC_PPD_LLP_MIRROR_PORT_DFLT_INFO defaultMirror;
#endif

    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_mode_get(unit, &mirrorMode));
        if (mirrorMode != BCM_PETRA_MIRROR_MODE_PORT_SET) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "The current mirror mode is bcm_petra_mirror_destination_create\n")));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }  
        
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "(%d,%d(%08X),*,*,*) enter\n"),
                   unit,
                   port,
                   port));
        SOC_PETRA_MIRROR_UNIT_CHECK(unit, unitData);
        result = _bcm_petra_mirror_port_check(unit, port, &myModule, &localPort);
        BCMDNX_IF_ERR_EXIT(result);
        if ((!dest_mod) || (!dest_port) || (!flags)) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "obligatory OUT argument mut not be NULL\n")));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        }

        SOC_PETRA_MIRROR_LOCK_TAKE(unitData);

        /* deal with ingress mirorring first */
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "get unit %d port %d(%08X) ingress mirror\n"),
                   unit,
                   port,
                   port));
        ppdr = soc_ppd_llp_mirror_port_dflt_get((unit),
                                            localPort,
                                            &defaultMirror);
        result = handle_sand_result(ppdr);
        if (BCM_E_NONE == result) {
            if (defaultMirror.tagged_dflt) {
                /* ingress mirror enabled */
                cFlags |= BCM_MIRROR_PORT_INGRESS;
                LOG_DEBUG(BSL_LS_BCM_MIRROR,
                          (BSL_META_U(unit,
                                      "get unit %d mirror %d info\n"),
                           unit,
                           defaultMirror.tagged_dflt));
                result = _bcm_petra_ingress_mirror_get(unitData,
                                                       defaultMirror.tagged_dflt,
                                                       &cDestMod,
                                                       &cDestPort);
            }
        } else {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "unable to read ingress mirorring state"
                                  " for unit %d port %d(%08X): %d (%s)\n"),
                       unit,
                       port,
                       port,
                       result,
                       _SHR_ERRMSG(result)));
        }
        

        if (BCM_E_NONE == result) {
            /* commit data to caller */
            *flags = cFlags;
            *dest_mod = cDestMod;
            *dest_port = cDestPort;
        }

        SOC_PETRA_MIRROR_LOCK_GIVE(unitData);
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "(%d,%d(%08X),&(%d),&(%d(%08X)),&(%08X))"
                              " return %d (%s)\n"),
                   unit,
                   port,
                   port,
                   *dest_mod,
                   *dest_port,
                   *dest_port,
                   *flags,
                   result,
                   _SHR_ERRMSG(result)));
        BCMDNX_IF_ERR_EXIT(result);
    }
    else
#endif
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
    }
exit:
    BCMDNX_FUNC_RETURN;
}




/*
 *  Function
 *     bcm_petra_mirror_destination_tunnel_create
 *  Purpose
 *     In case mirror destination is over ip tunnel with/out ERSPAN.
 *     build ERSPAN tunnel if needed, and make mirror profile points to erspan outlif
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_mirror_destination_t *mirror_dest: mirror destination as given by BCM call
 *     (in) uint32  action_profile_id: mirror profile used
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */

int
bcm_petra_mirror_destination_tunnel_create(int unit,
                                    bcm_mirror_destination_t *mirror_dest, uint32  action_profile_id,uint32 *outlif, uint8 *outlif_valid, uint8 update)
{
#ifdef BCM_ARAD_SUPPORT
    uint32 soc_sand_rv;
    uint32 alloc_flags=0; 
    int rv = BCM_E_NONE;
    uint32 ip_tunnel_id;    
    SOC_PPD_EG_ENCAP_DATA_INFO
        data_encap_entry;
    SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO
        mirror_encap_info;
    uint32 next_eep, nof_entries;
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    int mc_support;

    int soc_sand_dev_id;
#endif
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    *outlif = 0;
    *outlif_valid = 0;

    /* tunnel build */
#ifdef BCM_ARAD_SUPPORT
    soc_sand_dev_id = (unit);
    if (SOC_IS_ARAD(unit)) {

        /* encap_id to point to egres LIF: can be supplied without setting IP_GRE or RSPAN, which already build using ip tunnel API */
        if(mirror_dest->encap_id != 0) {
            *outlif = mirror_dest->encap_id;
            alloc_flags = SHR_RES_ALLOC_WITH_ID;
            if (update) {
                alloc_flags |= SHR_RES_ALLOC_REPLACE;
            }
            *outlif_valid = 1;
        }
        
        /* check if destination include IP tunnel encapsulation */
        if(mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) {
            *outlif_valid = 1;
            if (!_BCM_PETRA_L3_ITF_IS_ENCAP(mirror_dest->tunnel_id)) {/* IP tunnel id  */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_TUNNEL_IP_GRE flag is set then tunnel_id has to point to ip-tunnel")));
            }
            ip_tunnel_id = _BCM_PETRA_L3_ITF_VAL_GET(mirror_dest->tunnel_id);

            /* update IP tunnel to include gre of 8 bits type */
            /* get entry */
            soc_sand_rv =
              soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                     SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                     ip_tunnel_id, 1,
                                     encap_entry_info, &next_eep,
                                     &nof_entries);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            /* check that it's really IP tunnel */
            if (encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_TUNNEL_IP_GRE flag is and pointed encap is not an IP tunnel ")));
            }

            /* 
             * ERSPAN multicast: IPV4_tunnel.VSI = encap_id
             */
            if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
                mc_support = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "custom_feature_erspan_mc_support ", 0) ;
            } else {
                mc_support = 0;
            }

            /* update ip tunnel to include enhanced GRE */
            if(encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode != SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE ||
               (mc_support && encap_entry_info[0].entry_val.ipv4_encap_info.out_vsi != mirror_dest->encap_id))
            {
                encap_entry_info[0].entry_val.ipv4_encap_info.dest.encapsulation_mode = SOC_PPD_EG_ENCAP_ENCAPSULATION_MODE_ENHANCE_GRE;
                if (mc_support) 
                {
                    encap_entry_info[0].entry_val.ipv4_encap_info.out_vsi = mirror_dest->encap_id;
                }

                soc_sand_rv =
                  soc_ppd_eg_encap_ipv4_entry_add(soc_sand_dev_id,
                                         ip_tunnel_id, 
                                         &(encap_entry_info[0].entry_val.ipv4_encap_info),
                                         next_eep);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }


            /* check if need to add ERSPAN encapsulation */
            if(mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID) {/*span_id*/

                if (SOC_IS_ARAD_B1_AND_BELOW(unit))
                {
                    if (0 == (mirror_dest->flags & (BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID))) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID is set but BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID is not set")));
                    }

                    if (0 == (mirror_dest->flags & (BCM_MIRROR_DEST_TUNNEL_L2))) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID is set but SBCM_MIRROR_DEST_TUNNEL_L2 is not set")));
                    }

                    if (DPP_MIRROR_ACTION_NDX_MAX < mirror_dest->encap_id ||
                        0 > mirror_dest->encap_id) { 
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id is out of range range is %d - %d"), 0, DPP_MIRROR_ACTION_NDX_MAX));
                    }
                    
                    if(mirror_dest->encap_id != (ip_tunnel_id & 0xf)) { 
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id is different from tunnel_id[0:3]. encap_id = %d, tunnel_id[0:3] = %d "), 
                                                       mirror_dest->encap_id, (ip_tunnel_id & 0xf)));
                    }
                    

                    SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO_clear(&mirror_encap_info);

                    /* L2 source mac address. */
                    rv = _bcm_petra_mac_to_sand_mac(mirror_dest->dst_mac, &(mirror_encap_info.tunnel.dest));
                    BCMDNX_IF_ERR_EXIT(rv);
    
                    /* L2 destination mac address. */
                    rv = _bcm_petra_mac_to_sand_mac(mirror_dest->src_mac, &(mirror_encap_info.tunnel.src));
                    BCMDNX_IF_ERR_EXIT(rv);
    
                    mirror_encap_info.tunnel.tpid       = mirror_dest->tpid;
                    mirror_encap_info.tunnel.vid        = mirror_dest->vlan_id;
                    mirror_encap_info.tunnel.pcp        = mirror_dest->pkt_prio;
                    mirror_encap_info.tunnel.ether_type = 0x0800; /* ipv4 ethertype */
                    mirror_encap_info.tunnel.erspan_id  = mirror_dest->span_id;
                    mirror_encap_info.tunnel.encap_id   = mirror_dest->encap_id;


                    /* add enry to allocated place */
                    soc_sand_rv = soc_ppd_eg_encap_mirror_entry_set(soc_sand_dev_id, mirror_dest->encap_id, &mirror_encap_info);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    *outlif       = ip_tunnel_id;
                    *outlif_valid = 1;                    

                }
                else /* ARAD+ or above*/
                {
                    /* Can't replace non - existing ERSPAN entry */
                    if (update) {
                        rv = bcm_dpp_am_eg_data_erspan_is_alloced(unit,*outlif);
                        if (rv != BCM_E_EXISTS) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ERSPAN id %d to be replaced does not exist"),*outlif));
                        }

                        /* get entry */
                        soc_sand_rv =
                          soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                                 SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                                 *outlif, 1,
                                                 encap_entry_info, &next_eep,
                                                 &nof_entries);
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                    } else {
                        /* allocate entry to store the ERSPAN entry */
                        
                        
                        rv = bcm_dpp_am_eg_data_erspan_alloc(unit,alloc_flags,outlif);
                        BCMDNX_IF_ERR_EXIT(rv);
                    }

                    /* build data entry and fill with ERSPAN info */
                    SOC_PPD_EG_ENCAP_DATA_INFO_clear(&data_encap_entry);
                    SOC_PPD_EG_ENCAP_EEDB_DATA_ERSPAN_FORMAT_SET(soc_sand_dev_id, SOC_PPD_EG_ENCAP_ERSPAN_PRIO_VAL, SOC_PPD_EG_ENCAP_ERSPAN_TRUNC_VAL, 
                                                                 mirror_dest->span_id, &data_encap_entry);

                    if (update) {
                        data_encap_entry.oam_lif_set = encap_entry_info[0].entry_val.data_info.oam_lif_set;
                        data_encap_entry.drop = encap_entry_info[0].entry_val.data_info.drop;                        
                    }

                    /* add enry to allocated place */
                    soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id,*outlif,&data_encap_entry,TRUE,ip_tunnel_id);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                }

            }
        }
        else if(mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_RSPAN){ /* RSPAN */
            *outlif_valid = 1;

            /* Can't replace non - existing ERSPAN entry */
            if (update) {
                rv = bcm_dpp_am_eg_data_erspan_is_alloced(unit,*outlif);
                if (rv != BCM_E_EXISTS) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ERSPAN id %d to be replaced does not exist"),*outlif));
                }
                
                /* get entry */
                soc_sand_rv =
                  soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                         SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                         *outlif, 1,
                                         encap_entry_info, &next_eep,
                                         &nof_entries);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            } else {
                /* allocate entry to store the ERSPAN entry */
                
                
                rv = bcm_dpp_am_eg_data_erspan_alloc(unit,alloc_flags,outlif);
                BCMDNX_IF_ERR_EXIT(rv);
            }

            /* build data entry and fill with RSPAN info */
            SOC_PPD_EG_ENCAP_DATA_INFO_clear(&data_encap_entry);
            SOC_PPD_EG_ENCAP_DATA_RSPAN_FORMAT_SET(soc_sand_dev_id,mirror_dest->tpid,mirror_dest->pkt_prio,mirror_dest->vlan_id,&data_encap_entry);

            if (update) {
                data_encap_entry.oam_lif_set = encap_entry_info[0].entry_val.data_info.oam_lif_set;
                data_encap_entry.drop = encap_entry_info[0].entry_val.data_info.drop;
            }

            /* add enry to allocated place including vlan-tag */
            soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id,*outlif,&data_encap_entry,TRUE,mirror_dest->vlan_id); /* FIX need to point to RIF entry to update VSI for processing */
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        /* if out-lif not valid then reset outlif value */
        if(*outlif_valid == 0) {
            *outlif = 0;
        }
        else{
            mirror_dest->encap_id = *outlif;
        }
    }
    else
#endif /* SOC_IS_ARAD */
    {
    }
    BCM_EXIT;


exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


int
bcm_petra_mirror_destination_tunnel_destroy(int unit, uint32 outlif)
{
#ifdef BCM_ARAD_SUPPORT
    uint32 soc_sand_rv;
    uint32 dealloc_flags=0; 
    int rv = BCM_E_NONE;
    SOC_PPD_EG_ENCAP_DATA_INFO
        data_encap_entry;
    int soc_sand_dev_id;
#endif
    
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    /* tunnel destroy */
#ifdef BCM_ARAD_SUPPORT
    soc_sand_dev_id = (unit);
    if (SOC_IS_ARAD(unit)) {
        rv = bcm_dpp_am_eg_data_erspan_dealloc(unit,dealloc_flags,outlif);
        BCMDNX_IF_ERR_EXIT(rv);
        /* clear data entry and fill  */
        SOC_PPD_EG_ENCAP_DATA_INFO_clear(&data_encap_entry);
        /* add enry to allocated place */
        soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id,outlif,&data_encap_entry,FALSE,0);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
    else
#endif /* SOC_IS_ARAD */
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


/*
 *  Function
 *     bcm_petra_mirror_destination_create
 *  Purpose
 *     Create mirror destination description.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in/out) bcm_mirror_destination_t *mirror_dest = dest description
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Creates a mirror destination for use with bcm_mirror_port{_vlan}_dest_*
 *     The returned destination is specific to the unit and can not be used with other units.
 */
int
bcm_petra_mirror_destination_create(int unit,
                                    bcm_mirror_destination_t *mirror_dest)
{
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO info;
    uint32 action_profile_id = 0;
    uint32 soc_sand_rv;
    uint32 flags; /* hold flags for dpp function */
    uint32 prob_field; /* the value for the HW field of the mirror probability */
    int do_alloc = 1, is_created, failed = 1;
#ifdef BCM_PETRAB_SUPPORT
    int mirror_mode;
#endif
    unsigned int eproc;
    unsigned int ecntr;

    SOC_TMC_CNT_MODE_EG_TYPE                eg_type;
    SOC_TMC_CNT_SRC_TYPE                    src_type;
    MIRROR_LOCK_DEFS;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(mirror_dest);

#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_mode_get(unit, &mirror_mode));
        if (mirror_mode != BCM_PETRA_MIRROR_MODE_DESTINATION_CREATE) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "The current mirror mode is bcm_petra_mirror_port_set\n")));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        } 
    }
#endif

    /* set input data for mbcm_dpp_action_cmd_mirror_set and parse input flags */
    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&info);
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        /*
         * The probability of mirroring a packet is:
         * sample_rate_dividend >= sample_rate_divisor ? 1 : sample_rate_dividend / sample_rate_divisor
         * The field should be probability ? probability * 2^16 - 1 : 0
         * The actual formula for the field with rounding is:
         * sample_rate_dividend * 2^16 / sample_rate_divisor - 1/2
         */
        if (mirror_dest->sample_rate_dividend >= mirror_dest->sample_rate_divisor) {
            prob_field = ARAD_MIRROR_PROBABILITY_ALLWAYS; /* 100% probability of mirror execution in Arad. */
        } else {
#if defined(COMPILER_HAS_LONGLONG_MUL) && !defined(__KERNEL__)
            /* Calculate using 64 bit multiplication and division */
            uint64 val64 = mirror_dest->sample_rate_dividend;
            val64 = (val64 << ARAD_MIRROR_PROBABILITY_BITS) + mirror_dest->sample_rate_divisor / 2;
            val64 /= mirror_dest->sample_rate_divisor;
            prob_field = val64;
#else /* COMPILER_HAS_LONGLONG_MUL */
            /* Calculate using 32 bit arithmetic division, result may be off by one */
            uint32 val32 = mirror_dest->sample_rate_dividend; /* val32 contains the dividend here */
            unsigned shift_left = ARAD_MIRROR_PROBABILITY_BITS;
            /* multiply (shift) the dividend, but stop before overflowing */
            while ((val32 & (((uint32)1)<<31)) == 0 && shift_left) {
              --shift_left;
              val32 <<=1;
            }
            prob_field = val32 + ((mirror_dest->sample_rate_divisor/2) >> shift_left); /* add rounding (0.5 to field value) */
            if (prob_field < val32) {
                 prob_field = (uint32)(-1); /* handle overflow */
            }
            prob_field /= shift_left ?
              ((mirror_dest->sample_rate_divisor >> (shift_left - 1)) + 1) >> 1 : /* adjust the divisor to the shift, with rounding */
              mirror_dest->sample_rate_divisor;
#endif /* COMPILER_HAS_LONGLONG_MUL */

            if (prob_field <= 1) {
                if (mirror_dest->sample_rate_dividend) {
                    prob_field = 1; /* do not disable mirroring with non zero probability */
                }
            } else if (prob_field > ARAD_MIRROR_PROBABILITY_ALLWAYS) {
                prob_field = ARAD_MIRROR_PROBABILITY_ALLWAYS; /* fix roundup error */
            } else {
                --prob_field;
            }
        }
    } else
#endif
    {
        prob_field = 0x3ff; /* 100% probability of mirror execution in Soc_petra. */
    }
    info.prob = prob_field;


    /* Translate the input gport to a SOC_TMC_DEST_INFO */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_tm_dest_info(unit, mirror_dest->gport, &info.cmd.dest_id));

    if ((flags = mirror_dest->flags & ~(BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_REPLACE | BCM_MIRROR_DEST_UPDATE_POLICER | 
                                        BCM_MIRROR_DEST_UPDATE_COUNTER | BCM_MIRROR_DEST_TUNNEL_IP_GRE | BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID | 
                                        BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID | BCM_MIRROR_DEST_TUNNEL_RSPAN | BCM_MIRROR_DEST_TUNNEL_L2))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror destination flag was specified : 0x%lx"), (unsigned long)flags));
    }
    /* not supporting these flags: BCM_MIRROR_DEST_TUNNEL_L2 BCM_MIRROR_DEST_PAYLOAD_UNTAGGED BCM_MIRROR_DEST_TUNNEL_TRILL BCM_MIRROR_DEST_TUNNEL_NIV */


    /*  Override up meter pointer, if enabled. Value Range: 0 - 8K-1. Petra-B only since Meter block is after mirroring in Arad */
    if (SOC_IS_PETRAB(unit)) {
        if (mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_POLICER) {
            if (_DPP_POLICER_ID_TO_GROUP(unit, mirror_dest->policer_id)) { /* group 1 */
                info.cmd.meter_ptr_up.value = _DPP_POLICER_ID_TO_METER(unit, mirror_dest->policer_id);
                info.cmd.meter_ptr_up.enable = TRUE; /* set upper bank pointer */
            } else { /* group 0 */
                info.cmd.meter_ptr_low.value = _DPP_POLICER_ID_TO_METER(unit, mirror_dest->policer_id);
                info.cmd.meter_ptr_low.enable = TRUE; /* set lower bank pointer */
            }
        }
    }  else {
        if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_RSPAN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RSPAN flag unavailable through mirror_destination_create(). For RSPAN, create the tunnel with bcm_tunnel_initiator_create()  and create mirror with "
                                                           "encap_id set to the tunnel_id returned from above API, using BCM_GPORT_TUNNEL_ID_GET(). See mirror_with_rspan_example() in cint_mirror_erspan.c for further detail.")));
        }

        if (SOC_IS_ARADPLUS(unit)) {
            if (mirror_dest->flags & (BCM_MIRROR_DEST_TUNNEL_IP_GRE |BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID) ) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERSPN flags unavailable through mirror_destination_create(). For ERSPAN, create the tunnel with bcm_tunnel_initiator_create()  and create mirror with "
                                                               "encap_id set to the tunnel_id returned from above API, using BCM_GPORT_TUNNEL_ID_GET(). See mirror_with_erspan_example() in cint_mirror_erspan.c for further detail.")));

            }
        }
    } 

    if (mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_COUNTER) {
        _BCM_PETRA_PROC_CNTR_FROM_STAT(eproc, ecntr, mirror_dest->stat_id);
        BCMDNX_IF_ERR_EXIT(_soc_dpp_str_prop_parse_counter_source (unit, eproc, &src_type, &eg_type));

        if ((src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB) || (src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB)) { /* group 1 */
            info.cmd.counter_ptr_2.value = ecntr;
            info.cmd.counter_ptr_2.enable = TRUE; /* set lower bank pointer */
        } else { /* group 0 */
            info.cmd.counter_ptr_1.value = ecntr;
            info.cmd.counter_ptr_1.enable = TRUE; /* set upper bank pointer */
        }
    }

    flags = 0; /* flags will now hold the input flags for mbcm_dpp_action_cmd_mirror_set */
    MIRROR_LOCK_TAKE;
    if (mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID) { /* is the destination id (mirror action profile id) specified by the user? */
        /* verify that the given mirror destination is legal */
        if (!BCM_GPORT_IS_MIRROR(mirror_dest->mirror_dest_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
        }
        action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
        if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
        }

        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        if (mirror_dest->flags & BCM_MIRROR_DEST_REPLACE) { /* replace an existing destination */
            if (!is_created) { /* verify that the destination already exists */
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not in used"), (unsigned)action_profile_id));
            }
            do_alloc = 0;
        } else { /* create a new destination */
            if (is_created) { /* verify that the destination does not already exists */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirroring destination ID %u is already in use"), (unsigned)action_profile_id));
            }
        }
        flags = SHR_TEMPLATE_MANAGE_SET_WITH_ID;
    } else { /* the destination was not specified, need to allocate a free destination */
        action_profile_id = 0;
        if (mirror_dest->flags & BCM_MIRROR_DEST_REPLACE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_REPLACE must be used with BCM_MIRROR_DEST_WITH_ID")));
        }
    }

    /* allocate mirror profile */
    if (do_alloc) {
        if (_bcm_dpp_am_template_mirror_action_profile_alloc(unit, flags | SHR_TEMPLATE_MANAGE_IGNORE_DATA,NULL,NULL, (int*) &action_profile_id) != BCM_E_NONE) { 
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Failed to allocate mirroring destination")));
        }
        if (flags) {
            do_alloc = 9;
        }
        if (flags ?
          action_profile_id != BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id) :
          action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("internal mirror profile allocation error")));
        }
    }
    /* allocate tunnel if needed, return out lif */
    BCMDNX_IF_ERR_EXIT( bcm_petra_mirror_destination_tunnel_create(
      unit, mirror_dest, action_profile_id, &info.cmd.outlif.value, &info.cmd.outlif.enable,0));

    /* call dpp to set/create the mirror action profile */
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_set,(unit, action_profile_id, &info));
    if (soc_sand_get_error_code_from_error_word(soc_sand_rv) != SOC_SAND_OK) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed setting mirror action profile")));
    }

    if (!flags) { /* return the newly allocated profile */
        BCM_GPORT_MIRROR_SET(mirror_dest->mirror_dest_id, action_profile_id);
    }

    failed = 0;
exit:
    if (do_alloc == 9 && failed) {
		int is_last;
        /* Failure. no need to check the returned value */
	/* coverity[unchecked_value] */
	_bcm_dpp_am_template_mirror_action_profile_free(unit,  action_profile_id,  &is_last);
}
    MIRROR_LOCK_GIVE;
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_destination_destroy
 *  Purpose
 *     Destroy mirror destination description.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Destroyes a mirror destination for use with bcm_mirror_port{_vlan}_dest_*
 */
int
bcm_petra_mirror_destination_destroy(int unit,
                                     bcm_gport_t mirror_dest_id)
{
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO info;
    uint32 action_profile_id;
    uint32 soc_sand_rv;
    int is_created;
	int is_last;
#ifdef BCM_PETRAB_SUPPORT
    int mirror_mode;
#endif
    MIRROR_LOCK_DEFS;

    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);
    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_mode_get(unit, &mirror_mode));
        if (mirror_mode != BCM_PETRA_MIRROR_MODE_DESTINATION_CREATE) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "The current mirror mode is bcm_petra_mirror_port_set\n")));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        } 
    }
#endif

    if (!BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
    if (!is_created) { /* verify that the destination already exists */
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not created"), (unsigned)action_profile_id));
    }

    /* The code does not check if the destination is in use in all the possible places, as a requirements/design decision */

    /* get old info of the mirror profile to clean it up */
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_get,(unit, action_profile_id, &info));
    if (soc_sand_get_error_code_from_error_word(soc_sand_rv) != SOC_SAND_OK) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed resetting mirror action profile")));
    }

    /* set input data for mbcm_dpp_action_cmd_mirror_set and parse input flags */
    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&info);
    info.cmd.dest_id.id = -1; /* mark destination as drop */
    info.cmd.dest_id.type = SOC_PETRA_DEST_TYPE_QUEUE;
    info.prob = 0; /* 0% probability of mirror execution. */

    /* deallocate mirror profile */
    if (_bcm_dpp_am_template_mirror_action_profile_free(unit,  action_profile_id,  &is_last)!= BCM_E_NONE) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to deallocate mirroring destination")));
    }


    /* call dpp to set the mirror action profile to not used */
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_set,(unit, action_profile_id, &info));
    if (soc_sand_get_error_code_from_error_word(soc_sand_rv) != SOC_SAND_OK) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed resetting mirror action profile")));
    }

exit:
    MIRROR_LOCK_GIVE;
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}


/*
 *  Function
 *     _bcm_petra_mirror_destination_erspan_get
 *  Purpose
 *     get ERSPAN data from soc level (stopred in 2 prge memory table entries).
 *  Parameters
 *     (in) unit = the unit number
 *     (out) bcm_mirror_destination_t *mirror_dest = dest description
 *     (in) mirror_index = mirror index in preg memory
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
STATIC int
_bcm_petra_mirror_destination_erspan_get(int unit,
                                         bcm_mirror_destination_t *mirror_dest, uint32 mirror_index)
{
    uint32 soc_sand_rv;
    SOC_PPD_EG_ENCAP_MIRROR_ENCAP_INFO
        mirror_encap_info;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(mirror_dest);
  
    /* get the data ERSPAN from soc lovel */
    soc_sand_rv = soc_ppd_eg_encap_mirror_entry_get(unit, mirror_index, &mirror_encap_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
    mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_L2;
    mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_IP_GRE;
    mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID;

    mirror_dest->encap_id = mirror_index;

    mirror_dest->tpid     = mirror_encap_info.tunnel.tpid;
    mirror_dest->vlan_id  = mirror_encap_info.tunnel.vid;
    mirror_dest->pkt_prio = mirror_encap_info.tunnel.pcp;
    mirror_dest->span_id  = mirror_encap_info.tunnel.erspan_id;


    /* L2 source mac address. */
    rv = _bcm_petra_mac_from_sand_mac(mirror_dest->dst_mac, &(mirror_encap_info.tunnel.dest));
    BCMDNX_IF_ERR_EXIT(rv);
    
    /* L2 destination mac address. */
    rv = _bcm_petra_mac_from_sand_mac(mirror_dest->src_mac, &(mirror_encap_info.tunnel.src));
    BCMDNX_IF_ERR_EXIT(rv);
    
 
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_destination_get
 *  Purpose
 *     Get a mirror destination description.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *     (out) bcm_mirror_destination_t *mirror_dest = dest description
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Gets information about a mirror destination for use with
 *     bcm_mirror_port{_vlan}_dest_*
 */
int
bcm_petra_mirror_destination_get(int unit,
                                 bcm_gport_t mirror_dest_id,
                                 bcm_mirror_destination_t *mirror_dest)
{
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO info;
    SOC_TMC_DEST_INFO internal_dest;
    uint32 action_profile_id = 0;
    int is_created;
#ifdef BCM_PETRAB_SUPPORT
    int mirror_mode;
#endif
    unsigned int proc;
    unsigned int cntr;
    bcm_gport_t internal_gport;
    int internal_ports_count;
    SOC_TMC_CNT_MODE_EG_TYPE                eg_type;
    SOC_TMC_CNT_SRC_TYPE                    src_type;
    uint32 soc_sand_rv;
#ifdef BCM_ARAD_SUPPORT
    SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
    uint32 next_eep, nof_entries;
#endif
    MIRROR_LOCK_DEFS;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(mirror_dest);
    
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_mode_get(unit, &mirror_mode));
        if (mirror_mode != BCM_PETRA_MIRROR_MODE_DESTINATION_CREATE) {
            LOG_ERROR(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "The current mirror mode is bcm_petra_mirror_port_set\n")));
            BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
        } 
    }
#endif


    /* verify that the given mirror destination is legal */
    if (!BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }

    MIRROR_LOCK_TAKE;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
    if (!is_created) { /* verify that the destination already exists */
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not in created"), (unsigned)action_profile_id));
    }

    /* call dpp to get the mirror action profile */
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_get,(unit, action_profile_id, &info));
    MIRROR_LOCK_GIVE;
    if (soc_sand_get_error_code_from_error_word(soc_sand_rv) != SOC_SAND_OK) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed getting mirror action profile %u information"), (unsigned)action_profile_id));
    }

    /* check if destination is ERP port / outLIF = Multicast */
    BCMDNX_IF_ERR_EXIT(bcm_petra_port_internal_get(unit,BCM_PORT_INTERNAL_EGRESS_REPLICATION,1,&internal_gport,&internal_ports_count));
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_tm_dest_info(unit, internal_gport, &internal_dest));

    if(info.cmd.dest_id.id == internal_dest.id && info.cmd.dest_id.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT){
        bcm_mirror_destination_t_init(mirror_dest);
        mirror_dest->mirror_dest_id = mirror_dest_id;
        BCM_GPORT_MCAST_SET((mirror_dest->gport), info.cmd.outlif.value);
    }
    else{
        /* set the returned information */
        bcm_mirror_destination_t_init(mirror_dest);
        mirror_dest->mirror_dest_id = mirror_dest_id;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_from_tm_dest_info(unit, &mirror_dest->gport, &info.cmd.dest_id));
    }
#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        /* return the sample probability */
        mirror_dest->sample_rate_dividend = info.prob ? info.prob + 1 : 0;
        mirror_dest->sample_rate_divisor = ARAD_MIRROR_PROBABILITY_DIVISOR;

       /* in case i ERSPAN mirroring initialize the ERSPAN related data */
        if (SOC_IS_ARAD_B1_AND_BELOW(unit) &&
            1 == info.cmd.outlif.enable) {

            /* get entry */
            SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_encap_entry_get(unit,
                                                              SOC_PPD_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                                              info.cmd.outlif.value, 1,
                                                              encap_entry_info, &next_eep,
                                                              &nof_entries));

            /* when entry type is IPV4 it is for sure ERSPAN mirroring */
            if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
                _BCM_PETRA_L3_ITF_SET(mirror_dest->tunnel_id, _BCM_PETRA_L3_ITF_ENCAP, info.cmd.outlif.value);
                mirror_dest->encap_id = mirror_dest->tunnel_id & 0xf;
                BCMDNX_IF_ERR_EXIT(_bcm_petra_mirror_destination_erspan_get(unit, 
                                                                         mirror_dest, 
                                                                         info.cmd.outlif.value & 0xf));
            }
        }

    } else
#endif /* BCM_ARAD_SUPPORT */
    { /* return the sample probability */
        mirror_dest->sample_rate_divisor = mirror_dest->sample_rate_dividend = 0;
    }

    /*  Petra-B only since Meter block is after mirroring in Arad */
    if (SOC_IS_PETRAB(unit)) {
        if (info.cmd.meter_ptr_low.enable) { /* lower bank meter is used */
            mirror_dest->policer_id = _DPP_POLICER_ID_FROM_METER_GROUP(unit, info.cmd.meter_ptr_low.value, 0);
        } else if (info.cmd.meter_ptr_up.enable) { /* upper bank meter is used */
            mirror_dest->policer_id = _DPP_POLICER_ID_FROM_METER_GROUP(unit, info.cmd.meter_ptr_up.value, 1);
        }
    }

    if (info.cmd.counter_ptr_1.enable || info.cmd.counter_ptr_2.enable) {
        /* Find the Counter processor and get counter Id */
        cntr = 0;
        for (proc = 0; proc < SOC_DPP_DEFS_GET(unit, nof_counter_processors); proc++) {
            BCMDNX_IF_ERR_EXIT(_soc_dpp_str_prop_parse_counter_source(unit, proc, &src_type, &eg_type));

            if (info.cmd.counter_ptr_2.enable && 
                ((src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB) || (src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB))) { /* group 1 */
                cntr = info.cmd.counter_ptr_2.value;
                break;
            } 
            else if (info.cmd.counter_ptr_1.enable && 
                ((src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP) || (src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB))) { /* group 1 */
                cntr = info.cmd.counter_ptr_1.value;
                break;
            }
        }
        _BCM_PETRA_STAT_FROM_PROC_CNTR(mirror_dest->stat_id, proc, cntr);
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_destination_traverse
 *  Purpose
 *     Traverse existing mirror destination descriptions.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_mirror_destination_traverse_cb cb = ptr to callback handler
 *     (in) void *user_data = user data to pass to callback handler
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Traverses all mirror destinations for use with bcm_mirror_port{_vlan}_dest_*
 */
int
bcm_petra_mirror_destination_traverse(int unit,
                                      bcm_mirror_destination_traverse_cb cb,
                                      void *user_data)
{
    uint32 soc_sand_rv;
    uint32 action_profile_id = 0;
    int is_created, ret;
    bcm_mirror_destination_t mirror_dest;
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO info;
    unsigned int proc;
    unsigned int cntr;
    SOC_TMC_CNT_MODE_EG_TYPE                eg_type;
    SOC_TMC_CNT_SRC_TYPE                    src_type;
    MIRROR_LOCK_DEFS;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(cb);

    for (action_profile_id = DPP_MIRROR_ACTION_NDX_MIN; action_profile_id <= DPP_MIRROR_ACTION_NDX_MAX; ++action_profile_id) {
        MIRROR_LOCK_TAKE;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        if (is_created) {
            /* call dpp to get the mirror action profile */
            soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_get,(unit, action_profile_id, &info));
            if (soc_sand_get_error_code_from_error_word(soc_sand_rv) != SOC_SAND_OK) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed getting mirror destination %u information, stopping"), (unsigned)action_profile_id));
            }
            /* set the destination information for the callback */
            bcm_mirror_destination_t_init(&mirror_dest);
            BCM_GPORT_MIRROR_SET(mirror_dest.mirror_dest_id, action_profile_id);
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_from_tm_dest_info(unit, &mirror_dest.gport, &info.cmd.dest_id));

            if (info.cmd.meter_ptr_low.enable) { /* lower bank meter is used */
                mirror_dest.policer_id = _DPP_POLICER_ID_FROM_METER_GROUP(unit, info.cmd.meter_ptr_low.value, 0);
            } else if (info.cmd.meter_ptr_up.enable) { /* upper bank meter is used */
                mirror_dest.policer_id = _DPP_POLICER_ID_FROM_METER_GROUP(unit, info.cmd.meter_ptr_up.value, 1);
            }

            if (info.cmd.counter_ptr_1.enable || info.cmd.counter_ptr_2.enable) {
                /* Find the Counter processor and get counter Id */
                cntr = 0;
                for (proc = 0; proc < SOC_DPP_DEFS_GET(unit, nof_counter_processors); proc++) {
                    BCMDNX_IF_ERR_EXIT(_soc_dpp_str_prop_parse_counter_source(unit, proc, &src_type, &eg_type));

                    if (info.cmd.counter_ptr_2.enable && 
                        ((src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_2_LSB) || (src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_2_MSB))) { /* group 1 */
                        cntr = info.cmd.counter_ptr_2.value;
                        break;
                    } 
                    else if (info.cmd.counter_ptr_1.enable && 
                        ((src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP) || (src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP_MSB))) { /* group 1 */
                        cntr = info.cmd.counter_ptr_1.value;
                        break;
                    }
                }
                _BCM_PETRA_STAT_FROM_PROC_CNTR(mirror_dest.stat_id, proc, cntr);
            }

            MIRROR_LOCK_GIVE;
            ret = cb(unit, &mirror_dest, user_data);
            if (ret) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("callback function returned error value %d for mirror destination %u"),ret, (unsigned)action_profile_id));
            }
        }
    }
exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_dest_add
 *  Purpose
 *     Add a mirroring destination to a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_port_dest_add(int unit,
                               bcm_port_t port,
                               uint32 flags,
                               bcm_gport_t mirror_dest)
{
    uint32 action_profile_id = 0, found_profile;
    SOC_PPD_PORT loc_port;
    int is_created, is_destination;
    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);
    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS
#ifdef BCM_ARAD_SUPPORT
        | (SOC_IS_ARAD(unit) ? BCM_MIRROR_PORT_EGRESS_ACL : 0)
#endif
        )) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    is_destination = BCM_MIRROR_PORT_EGRESS_ACL != (flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL));
    /* check destination validity */
    if (is_destination && !BCM_GPORT_IS_MIRROR(mirror_dest)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest);
    if (is_destination && (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    if (is_destination) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        if (!is_created) { /* verify that the destination already exists */
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not created"), (unsigned)action_profile_id));
        }
    }

    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        SOC_PPD_LLP_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;
        /* check that the port is not already mirrored */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_dflt_get((unit), loc_port, &dflt_mirroring_info));
        if ((found_profile = dflt_mirroring_info.tagged_dflt) != dflt_mirroring_info.untagged_dflt) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Inconsistent destination action profiles %u, %u for the port"),
              (unsigned)found_profile, (unsigned)dflt_mirroring_info.untagged_dflt));
        }
        if (found_profile) { /* the port is mirrored or internal error */
            if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
            }
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The inbound port is already mirrored")));
        }

        /* call dpp to set the port mirroring to the given profile */
        SOC_PPD_LLP_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info);
        dflt_mirroring_info.tagged_dflt = action_profile_id;
        dflt_mirroring_info.untagged_dflt = action_profile_id;
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_dflt_set((unit), loc_port, &dflt_mirroring_info));
    }

    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        SOC_PPD_EG_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;
        /* check that the port is not already mirrored */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_dflt_get((unit), loc_port, &dflt_mirroring_info));
        found_profile = dflt_mirroring_info.dflt_profile;
        
        if (dflt_mirroring_info.enable_mirror) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port is already mirrored")));
        } else if (found_profile > DPP_MIRROR_ACTION_NDX_MAX) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }

#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            uint8 mirr_profile, mirr_port, mirr_port_vlan, mirr_enable;
            
            BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_get(unit, port, &mirr_profile, &mirr_port, &mirr_port_vlan, &mirr_enable)); 
            if (mirr_port_vlan != 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port mirror is already mirrored by port-vlan")));
            } else {
                BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_set(unit, port, action_profile_id, TRUE));
            }
        }
#endif
        /* call dpp to set the port mirroring to the given profile */
        SOC_PPD_EG_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info);
        dflt_mirroring_info.enable_mirror = TRUE;
        dflt_mirroring_info.dflt_profile = action_profile_id;
        
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_dflt_set((unit), loc_port, &dflt_mirroring_info));
    }

    if (flags & BCM_MIRROR_PORT_EGRESS_ACL) { /* enable of outbound mirroring (reserve reassembly context and recycle channel) for other applications */
        uint8 recycle_enabled;
        /* check if mirroring is already enabled */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_appl_get((unit), loc_port, &recycle_enabled));
        if (recycle_enabled) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port is already enabled for mirroring by other applications like acl")));
        }

        /* enable mirroring */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_appl_set((unit), loc_port, 1));
    }


exit:
    MIRROR_LOCK_GIVE;
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_dest_delete
 *  Purpose
 *     Remove a mirroring destination from a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_port_dest_delete(int unit,
                                  bcm_port_t port,
                                  uint32 flags,
                                  bcm_gport_t mirror_dest)
{
    uint32 action_profile_id, found_profile;
    SOC_PPD_PORT loc_port;
    int is_created, is_destination;
    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS
#ifdef BCM_ARAD_SUPPORT
        | (SOC_IS_ARAD(unit) ? BCM_MIRROR_PORT_EGRESS_ACL : 0)
#endif
        )) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    is_destination = BCM_MIRROR_PORT_EGRESS_ACL != (flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL));
    /* check destination validity */
    if (is_destination && !BCM_GPORT_IS_MIRROR(mirror_dest)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest);
    if (is_destination && (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    if (is_destination) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        if (!is_created) { /* verify that the destination already exists */
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not created"), (unsigned)action_profile_id));
        }
    }

    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        SOC_PPD_LLP_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;
        /* check that the destination profile is the specified one */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_dflt_get((unit), loc_port, &dflt_mirroring_info));
        if ((found_profile = dflt_mirroring_info.tagged_dflt) != dflt_mirroring_info.untagged_dflt) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Inconsistent destination action profiles %u, %u for the port"),
              (unsigned)found_profile, (unsigned)dflt_mirroring_info.untagged_dflt));
        }
        /* check retrieved destination validity */
        if (!found_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("inbound port was not added to mirror")));
        } else if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }
        if (found_profile != action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The inbound port has destination profile %u, and not the specified one"),
              (unsigned)found_profile));
        }

        /* call dpp to set the port mirroring to the given profile */
        SOC_PPD_LLP_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info); /* mark as not mirrored */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_dflt_set((unit), loc_port, &dflt_mirroring_info));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        SOC_PPD_EG_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;

        /* check that the destination profile is the specified one */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_dflt_get((unit), loc_port, &dflt_mirroring_info));
        /* check retrieved destination validity */
        found_profile = dflt_mirroring_info.dflt_profile;        
        if (!dflt_mirroring_info.enable_mirror) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outbound port was not added to mirror")));
        }
#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            uint8 mirr_profile, mirr_port, mirr_port_vlan, mirr_enable;
            
            BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_get(unit, port, &mirr_profile, &mirr_port, &mirr_port_vlan, &mirr_enable)); 
            if ((mirr_port == 1) && (mirr_port_vlan == 0)){
                if (action_profile_id != mirr_profile) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The outbound port mirror has destination profile %u, and not the specified one"), (unsigned)mirr_profile));
                } else {
                    BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_set(unit, port, mirr_profile, FALSE)); 
                }
            }    
        }   
#else
        else if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }
        if (found_profile != action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The outbound port has destination profile %u, and not the specified one"),
              (unsigned)found_profile));
        }   
#endif
        /* call dpp to set the port mirroring to the given profile */
        SOC_PPD_EG_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info); /* mark as not mirrored */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_dflt_set((unit), loc_port, &dflt_mirroring_info));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS_ACL) { /* remove reservation of reassembly context and recycle channel for port for other applications */
        uint8 recycle_enabled;
        /* check if mirroring is already enabled */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_appl_get((unit), loc_port, &recycle_enabled));
        if (!recycle_enabled) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port is not enabled for mirroring by other applications like acl")));
        }

        /* disable mirroring */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_appl_set((unit), loc_port, 0));
    }

exit:
    MIRROR_LOCK_GIVE;
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_dest_delete_all
 *  Purpose
 *     Remove all mirroring destinations from a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_port_dest_delete_all(int unit,
                                      bcm_port_t port,
                                      uint32 flags)
{
    SOC_PPD_PORT loc_port;
    uint32 found_profile;
    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS
#ifdef BCM_ARAD_SUPPORT
        | (SOC_IS_ARAD(unit) ? BCM_MIRROR_PORT_EGRESS_ACL : 0)
#endif
        )) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    MIRROR_LOCK_TAKE;
    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        SOC_PPD_LLP_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;
        /* check that the port was added */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_dflt_get((unit), loc_port, &dflt_mirroring_info));
        if ((found_profile = dflt_mirroring_info.tagged_dflt) != dflt_mirroring_info.untagged_dflt) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Inconsistent destination action profiles %u, %u for the port"),
              (unsigned)found_profile, (unsigned)dflt_mirroring_info.untagged_dflt));
        }
        /* check retrieved destination validity */
        if (!found_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("inbound port was not added to mirror")));
        } else if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }

        /* call dpp to set the port mirroring to the given profile */
        SOC_PPD_LLP_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info); /* mark as not mirrored */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_dflt_set((unit), loc_port, &dflt_mirroring_info));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        SOC_PPD_EG_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;        

        /* check that the port was added */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_dflt_get((unit), loc_port, &dflt_mirroring_info));
        /* check retrieved destination validity */
        found_profile = dflt_mirroring_info.dflt_profile;
        if (!dflt_mirroring_info.enable_mirror) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outbound port was not added to mirror")));
        } 
#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            uint8 mirr_profile, mirr_port, mirr_port_vlan, mirr_enable;
            
            BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_get(unit, port, &mirr_profile, &mirr_port, &mirr_port_vlan, &mirr_enable)); 
            if ((mirr_port == 1) && (mirr_port_vlan == 0)) {
                BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_set(unit, port, mirr_profile, FALSE)); 
            }    
        } 
#else
        else if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }
#endif
        /* call dpp to set the port mirroring to the given profile */
        SOC_PPD_EG_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info); /* mark as not mirrored */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_dflt_set((unit), loc_port, &dflt_mirroring_info));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS_ACL) { /* remove reservation of reassembly context and recycle channel for port for other applications */
        uint8 recycle_enabled;
        /* check if mirroring is already enabled */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_appl_get((unit), loc_port, &recycle_enabled));
        if (!recycle_enabled) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port is not enabled for mirroring by other applications like acl")));
        }

        /* disable mirroring */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_appl_set((unit), loc_port, 0));
    }

exit:
    MIRROR_LOCK_GIVE;
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_dest_get
 *  Purpose
 *     Get (all?) mirroring destinations on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) int mirror_dest_size = size of provided buffer
 *     (out) bcm_gport_t *mirror_dest = pointer to buffer for destinations
 *     (out) int *mirror_dest_count = number of mirror dests
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     The value written to mirror_dest_count will be the number of mirror
 *     destinations actually filled in mirror_dest, except if the value of
 *     mirror_dest_size is zero.  In this case, nothing is written to
 *     mirror_dest and mirror_dest_count is populated with the actual number of
 *     mirror destinations on the port.
 */
int
bcm_petra_mirror_port_dest_get(int unit,
                               bcm_port_t port,
                               uint32 flags,
                               int mirror_dest_size,
                               bcm_gport_t *mirror_dest,
                               int *mirror_dest_count)
{
    SOC_PPD_PORT loc_port;
    int is_created;
    uint32 found_profile;
    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS
#ifdef BCM_ARAD_SUPPORT
        | (SOC_IS_ARAD(unit) ? BCM_MIRROR_PORT_EGRESS_ACL : 0)
#endif
        )) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    *mirror_dest_count = 0;
    MIRROR_LOCK_TAKE;
    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        SOC_PPD_LLP_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;
        /* get the destination profile */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_dflt_get((unit), loc_port, &dflt_mirroring_info));
        if ((found_profile = dflt_mirroring_info.tagged_dflt) != dflt_mirroring_info.untagged_dflt) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Inconsistent destination action profiles %u, %u for the port"),
              (unsigned)found_profile, (unsigned)dflt_mirroring_info.untagged_dflt));
        }
        /* check retrieved destination validity */
        if (!found_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("inbound port was not added to mirror")));
        } else if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, found_profile, &is_created));
        if (!is_created) { /* verify that the destination already exists */
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirroring destination ID %u is not created"), (unsigned)found_profile));
        }
        if (*mirror_dest_count >= mirror_dest_size) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("output size is too small")));
        }
        BCM_GPORT_MIRROR_SET(*mirror_dest, found_profile);
        ++*mirror_dest_count;
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        SOC_PPD_EG_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;
        /* get the destination profile */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_dflt_get((unit), loc_port, &dflt_mirroring_info));
        found_profile = dflt_mirroring_info.dflt_profile;
        /* check retrieved destination validity */
        if (!dflt_mirroring_info.enable_mirror) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outbound port was not added to mirror")));
        } 
#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            uint8 mirr_profile, mirr_port, mirr_port_vlan, mirr_enable;
            
            BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_get(unit, port, &mirr_profile, &mirr_port, &mirr_port_vlan, &mirr_enable)); 
            found_profile = mirr_profile;
        }            
#else
        else if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }
#endif
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, found_profile, &is_created));
        if (!is_created) { /* verify that the destination already exists */
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirroring destination ID %u is not created"), (unsigned)found_profile));
        }
        if (*mirror_dest_count >= mirror_dest_size) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("output size is too small")));
        }
        BCM_GPORT_MIRROR_SET(*mirror_dest, found_profile);
        ++*mirror_dest_count;
    }
    if (flags & BCM_MIRROR_PORT_EGRESS_ACL) { /* check if reassembly context and recycle channel are reserved for port for other applications */
        uint8 recycle_enabled;
        /* check if mirroring is already enabled */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_appl_get((unit), loc_port, &recycle_enabled));
        if (recycle_enabled) {
          BCM_GPORT_MIRROR_SET(*mirror_dest, 0);
          ++*mirror_dest_count;
        }

    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}



/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_add
 *  Purpose
 *     Add a mirroring destination to a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *  Assume only ingress mirroring.
 */
int
bcm_petra_mirror_port_vlan_dest_add(int unit, 
                                    bcm_port_t port, 
                                    bcm_vlan_t vlan, 
                                    uint32 flags, 
                                    bcm_gport_t mirror_dest)
{
    uint32 action_profile_id;
    SOC_PPD_PORT loc_port;
    int is_created;
    SOC_SAND_SUCCESS_FAILURE success;
    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    /* check destination validity */
    if (!BCM_GPORT_IS_MIRROR(mirror_dest)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest);
    if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
    if (!is_created) { /* verify that the destination already exists */
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not created"), (unsigned)action_profile_id));
    }

    if (vlan > SOC_SAND_PP_VLAN_ID_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN ID is out of range")));
    }

    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        uint32 mirror_profile;
        /* check that the port+vlan is not already mirrored */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_vlan_get((unit), loc_port, vlan, &mirror_profile));
        if (mirror_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The inbound port-vlan combination is already mirrored")));
        }

        /* call dpp to set the port mirroring to the given profile */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_vlan_add((unit),
          loc_port, vlan, action_profile_id, &success));
        if (success != SOC_SAND_SUCCESS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("Failed adding an inbound port-vlan mirror destination")));
        }
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        uint8 mirror_profile;
        /* check that the port+vlan is not already mirrored */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_vlan_get((unit), loc_port, vlan, &mirror_profile));
        if (mirror_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port-vlan combination is already mirrored")));
        }
        
#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            uint8 mirr_profile, mirr_port, mirr_port_vlan, mirr_enable;
            
            BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_get(unit, port, &mirr_profile, &mirr_port, &mirr_port_vlan, &mirr_enable)); 
            if (mirr_port == 1) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port-vlan mirror is already mirrored by port")));
            } else {
                if (mirr_port_vlan == 0) {
                    BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_set(unit, port, action_profile_id, TRUE));
                } else if (action_profile_id != mirr_profile) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port-vlan mirror has destination profile %u, and can not add new one"), (unsigned)mirr_profile));
                } 
            }             
        }
#endif
        /* call dpp to set the port mirroring to the given profile */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_vlan_add((unit),
          loc_port, vlan, action_profile_id, &success));
        if (success != SOC_SAND_SUCCESS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("Failed adding an outbound port-vlan mirror destination, perhaps all outbound mirror vlans are used")));
        }
    }

exit:
    MIRROR_LOCK_GIVE;
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_delete
 *  Purpose
 *     Remove a mirroring destination from a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
/* bcm_petra_mirror_port_vlan_dest_delete */
int
bcm_petra_mirror_port_vlan_dest_delete(int unit, 
                                       bcm_port_t port, 
                                       bcm_vlan_t vlan, 
                                       uint32 flags, 
                                       bcm_gport_t mirror_dest)
{
    uint32 action_profile_id;
    SOC_PPD_PORT loc_port;
    int is_created;
    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    /* check destination validity */
    if (!BCM_GPORT_IS_MIRROR(mirror_dest)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest);
    if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
    if (!is_created) { /* verify that the destination already exists */
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not created"), (unsigned)action_profile_id));
    }

    if (vlan > SOC_SAND_PP_VLAN_ID_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN ID is out of range")));
    }

    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        uint32 mirror_profile;
        /* check that the destination profile is the specified one */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_vlan_get((unit), loc_port, vlan, &mirror_profile));
        if (!mirror_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified inbound port-vlan was not added")));
        }
        if (mirror_profile > DPP_MIRROR_ACTION_NDX_MAX || mirror_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)mirror_profile));
        }
        if (mirror_profile != action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The inbound port-vlan combination has destination profile %u, and not the specified one"),
              (unsigned)mirror_profile));
        }

        /* call dpp to remove the port-vlan mirroring */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_vlan_remove((unit), loc_port, vlan));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */       
        uint8 mirror_profile;

        /* check that the destination profile is the specified one */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_vlan_get((unit), loc_port, vlan, &mirror_profile));
        if (!mirror_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified outbound port-vlan was not added")));
        }
#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            uint8 mirr_profile, mirr_port, mirr_port_vlan, mirr_enable;
            
            BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_get(unit, port, &mirr_profile, &mirr_port, &mirr_port_vlan, &mirr_enable)); 
            if ((mirr_port == 0) && (mirr_port_vlan == 1)) {
                if (action_profile_id != mirr_profile) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The outbound port-vlan mirror has destination profile %u, and not the specified one"), (unsigned)mirr_profile));
                } else {
                    BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_set(unit, port, mirr_profile, FALSE)); 
                }
            }    
        }            
#else
        if (mirror_profile > DPP_MIRROR_ACTION_NDX_MAX || mirror_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)mirror_profile));
        }
        if (mirror_profile != action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The outbound port-vlan combination has destination profile %u, and not the specified one"),
              (unsigned)mirror_profile));
        }
#endif
        /* call dpp to remove the port-vlan mirroring */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_vlan_remove((unit), loc_port, vlan));
    }

exit:
    MIRROR_LOCK_GIVE;
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_delete_all
 *  Purpose
 *     Remove all mirroring destinations from a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_port_vlan_dest_delete_all(int unit, 
                                           bcm_port_t port, 
                                           bcm_vlan_t vlan, 
                                           uint32 flags)
{
    SOC_PPD_PORT loc_port;
    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      
    _BCM_DPP_SWITCH_API_START(unit);

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    if (vlan > SOC_SAND_PP_VLAN_ID_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN ID is out of range")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    MIRROR_LOCK_TAKE;
    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        uint32 mirror_profile;
        /* check that the port-vlan was added */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_vlan_get((unit), loc_port, vlan, &mirror_profile));
        if (!mirror_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified inbound port-vlan was not added")));
        }
        if (mirror_profile > DPP_MIRROR_ACTION_NDX_MAX || mirror_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)mirror_profile));
        }
        /* call dpp to remove the port-vlan mirroring */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_vlan_remove((unit), loc_port, vlan));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */     
        uint8 mirror_profile;
        /* check that the port-vlan was added */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_vlan_get((unit), loc_port, vlan, &mirror_profile));
        if (!mirror_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified outbound port-vlan was not added")));
        }
#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            uint8 mirr_profile, mirr_port, mirr_port_vlan, mirr_enable;
            
            BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_get(unit, port, &mirr_profile, &mirr_port, &mirr_port_vlan, &mirr_enable)); 
            if ((mirr_port == 0) && (mirr_port_vlan == 1)){
                BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_set(unit, port, mirr_profile, FALSE)); 
            }    
        }            
#else
        if (mirror_profile > DPP_MIRROR_ACTION_NDX_MAX || mirror_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)mirror_profile));
        }
#endif
        /* call dpp to remove the port-vlan mirroring */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_vlan_remove((unit), loc_port, vlan));
    }

exit:
    MIRROR_LOCK_GIVE;
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_get
 *  Purpose
 *     Get (all?) mirroring destinations on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) int mirror_dest_size = size of provided buffer
 *     (out) bcm_gport_t *mirror_dest = pointer to buffer for destinations
 *     (out) int *mirror_dest_count = number of mirror dests
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     The value written to mirror_dest_count will be the number of mirror
 *     destinations actually filled in mirror_dest, except if the value of
 *     mirror_dest_size is zero.  In this case, nothing is written to
 *     mirror_dest and mirror_dest_count is populated with the actual number of
 *     mirror destinations on the port.
 */
int
bcm_petra_mirror_port_vlan_dest_get(int unit, 
                                    bcm_port_t port, 
                                    bcm_vlan_t vlan, 
                                    uint32 flags, 
                                    uint32 mirror_dest_size, 
                                    bcm_gport_t *mirror_dest, 
                                    uint32 *mirror_dest_count)
{
    SOC_PPD_PORT loc_port;
    int is_created;
    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    if (vlan > SOC_SAND_PP_VLAN_ID_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN ID is out of range")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    *mirror_dest_count = 0;
    MIRROR_LOCK_TAKE;
    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        uint32 action_profile_id;
        /* get the destination profile */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_llp_mirror_port_vlan_get((unit), loc_port, vlan, &action_profile_id));
        /* check retrieved destination validity */
        if (!action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified inbound port-vlan was not added")));
        }
        if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)action_profile_id));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        if (!is_created) { /* verify that the destination already exists */
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirroring destination ID %u is not created"), (unsigned)action_profile_id));
        }
        if (*mirror_dest_count >= mirror_dest_size) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("output size is too small")));
        }
        BCM_GPORT_MIRROR_SET(*mirror_dest, action_profile_id);
        ++*mirror_dest_count;
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        uint8 action_profile_id;
        /* get the destination profile */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_vlan_get((unit), loc_port, vlan, &action_profile_id));
        /* check retrieved destination validity */
        if (!action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified outbound port-vlan was not added")));
        }
#ifdef BCM_PETRAB_SUPPORT
        if (SOC_IS_PETRAB(unit)) {
            uint8 mirr_profile, mirr_port, mirr_port_vlan, mirr_enable;
            
            BCMDNX_IF_ERR_EXIT(_bcm_petrab_mirror_port_outbound_get(unit, port, &mirr_profile, &mirr_port, &mirr_port_vlan, &mirr_enable)); 
            action_profile_id = mirr_profile;
        }            
#else
        if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)action_profile_id));
        }
#endif
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        if (!is_created) { /* verify that the destination already exists */
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirroring destination ID %u is not created"), (unsigned)action_profile_id));
        }
        if (*mirror_dest_count >= mirror_dest_size) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("output size is too small")));
        }
        BCM_GPORT_MIRROR_SET(*mirror_dest, (bcm_gport_t)action_profile_id);
        ++*mirror_dest_count;
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}


/******************************************************************************
 *
 *  Unimplemented API functions
 */



/*
 *  Function
 *     bcm_petra_mirror_mode_set
 *  Purpose
 *     Set mirroring mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) int mode = the mirroring mode to use
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_mode_set(int unit,
                          int mode)
{
    int rv = 0;
    int mirror_id, is_created;
    int mirror_mode;
    _bcm_petra_mirror_unit_data_t *unitData;
    
    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_sw_db_petra_mirror_mode_get(unit, &mirror_mode);
    BCMDNX_IF_ERR_EXIT(rv);
    if ((mirror_mode != mode) && (mode == BCM_PETRA_MIRROR_MODE_DESTINATION_CREATE)){
        SOC_PETRA_MIRROR_UNIT_CHECK(unit, unitData);

        for (mirror_id = 1; mirror_id < 16; mirror_id++) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, mirror_id, &is_created));
            if (is_created) {
                BCMDNX_IF_ERR_EXIT(_bcm_petra_ingress_mirror_free(unitData, mirror_id));
            }
        }
    }           
        
    rv = _bcm_sw_db_petra_mirror_mode_set(unit, mode);
    BCMDNX_IF_ERR_EXIT(rv);
        
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_mode_get
 *  Purpose
 *     Set mirroring mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (out) int *mode = where to put the mirroring mode
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_mode_get(int unit,
                          int *mode)
{
    int rv = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_sw_db_petra_mirror_mode_get(unit, mode);
    BCMDNX_IF_ERR_EXIT(rv);  

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_to_set
 *  Purpose
 *     Set mirroring mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to which mirror frames are to be sent
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_to_set(int unit,
                        bcm_port_t port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_to_get
 *  Purpose
 *     Set mirroring mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t *port = where to put mirror dest port
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_to_get(int unit,
                        bcm_port_t *port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_ingress_set
 *  Purpose
 *     Enable or disable ingress mirroring on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port on which to set ingress mirroring
 *     (in) int val = nonzero to enable ingress mirroring
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_ingress_set(int unit,
                             bcm_port_t port,
                             int val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_ingress_set
 *  Purpose
 *     Get ingress mirroring state on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port on which to get ingress mirroring
 *     (out) int *val = where to put ingress mirror flag for this port
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_ingress_get(int unit,
                             bcm_port_t port,
                             int *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_egress_set
 *  Purpose
 *     Enable or disable egress mirroring on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port on which to set egress mirroring
 *     (in) int val = nonzero to enable egress mirroring
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_egress_set(int unit,
                            bcm_port_t port,
                            int val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_egress_get
 *  Purpose
 *     Get egress mirroring state on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port on which to set egress mirroring
 *     (out) int *val = where to put port's egress mirroring state
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_egress_get(int unit,
                            bcm_port_t port,
                            int *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_pfmt_set
 *  Purpose
 *     Set mirroring format preserve mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) int mode = nonzero to preserve mirrored frame format
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */

int
bcm_petra_mirror_pfmt_set(int unit,
                          int val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_pfmt_get
 *  Purpose
 *     Get mirroring format preserve mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (out) int *mode = where to put format preserve setting
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */

int
bcm_petra_mirror_pfmt_get(int unit,
                          int *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_to_pbmp_set
 *  Purpose
 *     Set a port to mirror to a set of ports
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) bcm_pbmp_t pbmp = the mirror-to ports for the specified port
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Sets up so a port is mirrored to a *set* of ports.
 */

int
bcm_petra_mirror_to_pbmp_set(int unit,
                             bcm_port_t port,
                             bcm_pbmp_t pbmp)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_to_pbmp_get
 *  Purpose
 *     Get the set of ports to which a port is mirroring
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (out) bcm_pbmp_t *pbmp = where to put the port's the mirror-to ports
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Sets up so a port is mirrored to a *set* of ports.
 */

int
bcm_petra_mirror_to_pbmp_get(int unit,
                             bcm_port_t port,
                             bcm_pbmp_t *pbmp)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_vlan_set
 *  Purpose
 *     Set VLAN tag for mirror frames egressing a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint16 tpid = TPID to use (full 16b)
 *     (in) uint16 vlan = VLAN tag (full 16b)
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     This affects how mirrored frames are encapsulated on a port -- if VLAN
 *     or TPID is zero, mirror frames egressing the port will not have a tag
 *     inserted.  If VLAN and TPID are nonzero, mirror frames egressing the
 *     port will have the specified bytes inserted as outer VLAN tag.
 */

int
bcm_petra_mirror_vlan_set(int unit,
                          bcm_port_t port,
                          uint16 tpid,
                          uint16 vlan)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_vlan_get
 *  Purpose
 *     Get VLAN tag for mirror frames egressing a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (out) uint16 *tpid = where to put TPID to use (full 16b)
 *     (out) uint16 *vlan = where to put VLAN tag (full 16b)
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     This affects how mirrored frames are encapsulated on a port -- if VLAN
 *     or TPID is zero, mirror frames egressing the port will not have a tag
 *     inserted.  If VLAN and TPID are nonzero, mirror frames egressing the
 *     port will have the specified bytes inserted as outer VLAN tag.
 */

int
bcm_petra_mirror_vlan_get(int unit,
                          bcm_port_t port,
                          uint16 *tpid,
                          uint16 *vlan)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_egress_path_set
 *  Purpose
 *     Configure specific propagation path for mirror frames in stack rings.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_module_t modid = control mirror frames to this module
 *     (in) bcm_port_t port = which port to take to get to this module
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only applicable to devices that operate in stack-ring mode.
 */
int
bcm_petra_mirror_egress_path_set(int unit,
                                 bcm_module_t modid,
                                 bcm_port_t port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_egress_path_get
 *  Purpose
 *     Get specific propagation path for mirror frames in stack rings.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_module_t modid = control mirror frames to this module
 *     (out) bcm_port_t *port = where to put port to take to get to module
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only applicable to devices that operate in stack-ring mode.
 */
int
bcm_petra_mirror_egress_path_get(int unit,
                                 bcm_module_t modid,
                                 bcm_port_t *port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_info_set
 *  Purpose
 *     Set mirror information per port. Set system port ID per outbound mirror.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = ingress mirror port or egress mirror port
 *     (in) bcm_mirror_port_info_t *info = port mirror information
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only applicable ARAD. Used only for egress mirroring.
 *     The use must set the API after setting an outbound mirror, and set it again if stopping and starting again the mirroring.
 *     Future consideration:
 *     Storing the system port per output port would allow the user to make the configuration only once and possibly before setting the outbound mirror.
 */
int
bcm_petra_mirror_port_info_set(int unit,
                               bcm_port_t port,
                               uint32 flags,
                               bcm_mirror_port_info_t *info)
{
    SOC_PPD_PORT loc_port;
    SOC_PPD_EG_MIRROR_PORT_INFO ppd_port_info;
    
    int rv = 0;
    bcm_gport_t sysport;
    bcm_gport_t local_port;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_PETRAB(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
    }

    if (flags & ~(BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    if (flags & (BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        SOC_PPD_EG_MIRROR_PORT_INFO_clear(&ppd_port_info);
        /* Return connected In-Port */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_info_get(unit, loc_port, &ppd_port_info));

        if (ppd_port_info.outbound_mirror_enable) {
            BCM_GPORT_LOCAL_SET(local_port,ppd_port_info.outbound_port_ndx);
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport, info->mirror_system_id);
            
            rv = bcm_petra_stk_sysport_gport_set(unit, sysport, local_port);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Outbound mirror is not enabled")));
        }

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No acceptable flag combination was specified")));
    }



exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_info_get
 *  Purpose
 *     Get mirror information per port. Retreive system port ID per outbound mirror.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = ingress mirror port or egress mirror port
 *     (in) bcm_mirror_port_info_t *info = port mirror information
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only applicable for ARAD. Used only for egress mirroring. 
 */
int
bcm_petra_mirror_port_info_get(int unit,
                               bcm_port_t port,
                               uint32 flags,
                               bcm_mirror_port_info_t *info)
{
    SOC_PPD_PORT loc_port;
    SOC_PPD_EG_MIRROR_PORT_INFO ppd_port_info;
    
    int rv = 0;
    bcm_gport_t sysport;
    bcm_gport_t local_port;

    BCMDNX_INIT_FUNC_DEFS;
    
    if (SOC_IS_PETRAB(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
    }

    if (flags & ~(BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    if (flags & (BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        SOC_PPD_EG_MIRROR_PORT_INFO_clear(&ppd_port_info);
        /* Return connected In-Port */
        SOC_SAND_IF_ERR_RETURN(soc_ppd_eg_mirror_port_info_get(unit, loc_port, &ppd_port_info));

        if (ppd_port_info.outbound_mirror_enable) {
            BCM_GPORT_LOCAL_SET(local_port,ppd_port_info.outbound_port_ndx);

            rv = bcm_petra_stk_gport_sysport_get(unit, local_port, &sysport);
            BCMDNX_IF_ERR_EXIT(rv);

            info->mirror_system_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Outbound mirror is not enabled")));
        }

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No acceptable flag combination was specified")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_destination_add
 *  Purpose
 *     extended versions of existing bcm_petra_mirror_port_destination_add API using a bcm_mirror_options_t argument 
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *     (in) bcm_mirror_options_t options = input the options for the mirroring of packets 
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_port_destination_add(int unit, 
                                      bcm_port_t port, 
                                      uint32 flags, 
                                      bcm_gport_t mirror_dest, 
                                      bcm_mirror_options_t options)
{

    int _rv = BCM_E_UNAVAIL;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 *  Function
 *     bcm_petra_mirror_port_destination_get
 *  Purpose
 *     extended versions of existing bcm_petra_mirror_port_destination_get API using a bcm_mirror_options_t argument 
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) int mirror_dest_size = size of provided buffer
 *     (out) bcm_gport_t *mirror_dest = pointer to buffer for destinations
 *     (out) int *mirror_dest_count = number of mirror dests
 *     (out) bcm_mirror_options_t *options = get the options for the mirroring of packets 
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     The value written to mirror_dest_count will be the number of mirror
 *     destinations actually filled in mirror_dest, except if the value of
 *     mirror_dest_size is zero.  In this case, nothing is written to
 *     mirror_dest and mirror_dest_count is populated with the actual number of
 *     mirror destinations on the port.
 */
int
bcm_petra_mirror_port_destination_get(int unit, 
                                      bcm_port_t port, 
                                      uint32 flags, 
                                      int mirror_dest_size, 
                                      bcm_gport_t *mirror_dest, 
                                      int *mirror_dest_count, 
                                      bcm_mirror_options_t *options)
{
    int _rv = BCM_E_UNAVAIL;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;

}



/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_add
 *  Purpose
 *     extended versions of existing bcm_petra_mirror_port_vlan_dest_add API using a bcm_mirror_options_t argument 
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *     (in) bcm_mirror_options_t options = input the options for the mirroring of packets 
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *  Assume only ingress mirroring.
 */
int
bcm_petra_mirror_port_vlan_destination_add(int unit, 
                                           bcm_port_t port, 
                                           bcm_vlan_t vlan, 
                                           uint32 flags, 
                                           bcm_gport_t mirror_dest,
                                           bcm_mirror_options_t options)
{

    int _rv = BCM_E_UNAVAIL;

    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;

}


/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_get
 *  Purpose
 *     extended versions of existing bcm_petra_mirror_port_vlan_dest_get API using a bcm_mirror_options_t argument 
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) int mirror_dest_size = size of provided buffer
 *     (out) bcm_gport_t *mirror_dest = pointer to buffer for destinations
 *     (out) int *mirror_dest_count = number of mirror dests
 *     (out) bcm_mirror_options_t *options = get the options for the mirroring of packets 
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     The value written to mirror_dest_count will be the number of mirror
 *     destinations actually filled in mirror_dest, except if the value of
 *     mirror_dest_size is zero.  In this case, nothing is written to
 *     mirror_dest and mirror_dest_count is populated with the actual number of
 *     mirror destinations on the port.
 */
int
bcm_petra_mirror_port_vlan_destination_get(int unit, 
                                    bcm_port_t port, 
                                    bcm_vlan_t vlan, 
                                    uint32 flags, 
                                    uint32 mirror_dest_size, 
                                    bcm_gport_t *mirror_dest, 
                                    uint32 *mirror_dest_count,
                                    bcm_mirror_options_t *options)
{

    int _rv = BCM_E_UNAVAIL;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

    exit:
        BCMDNX_FUNC_RETURN;

}

