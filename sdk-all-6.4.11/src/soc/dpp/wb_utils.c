/*
 * $Id: common.c,v 1.66 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * File:        wb_utils.h
 * Purpose:     warmboot utilities
 * Requires:
 */

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/dpp/wb_utils.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/soc_sw_db.h>

#ifdef BCM_WARM_BOOT_SUPPORT

CONST char* CONST soc_dpp_sdk_ver_names[SOC_DPP_SDK_VER_COUNT] = {
    SOC_DPP_SDK_VER_NAMES_INIT
};

int
soc_warmboot_is_allowed(soc_chip_groups_t chip, soc_dpp_sdk_ver_t from, soc_dpp_sdk_ver_t to, uint8 *is_allowed) {
    int rv = SOC_E_NONE;

    *is_allowed = 0;

    if ((from < SOC_DPP_SDK_VER_FIRST) ||
        (from >= SOC_DPP_SDK_VER_COUNT)) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META("'from' value is out of range. from=%d, min=%d, max=%d"),
                   from,
                   SOC_DPP_SDK_VER_FIRST,
                   SOC_DPP_SDK_VER_COUNT));
        rv = SOC_E_PARAM;
        goto exit;
    }

    if ((to < SOC_DPP_SDK_VER_FIRST) ||
        (to >= SOC_DPP_SDK_VER_COUNT)) {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META("'to' value is out of range. to=%d, min=%d, max=%d"),
                   to,
                   SOC_DPP_SDK_VER_FIRST,
                   SOC_DPP_SDK_VER_COUNT));
        rv = SOC_E_PARAM;
        goto exit;
    }

    *is_allowed = 1;

exit:
    return rv;
}

int 
soc_sdk_version_get(soc_dpp_sdk_ver_t *ver) {
    *ver = SOC_DPP_SDK_VER_LATEST;

    return SOC_E_NONE;
}

int
soc_scache_sdk_version_get(int unit, soc_dpp_sdk_ver_t *ver) {
    int rv = SOC_E_NONE;
    soc_scache_handle_t wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = 0, recovered_ver;
    uint8                   *scache_ptr;

    SOCDNX_INIT_FUNC_DEFS

    SOC_SCACHE_HANDLE_SET(wb_handle, 0, SOC_MODULE_SDK_VER, 0);

    rv = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheRetrieve, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
    SOCDNX_IF_ERR_EXIT(rv);

    sal_memcpy(ver, scache_ptr, sizeof(*ver));

exit:
    SOCDNX_FUNC_RETURN;
}

int
_soc_scache_sdk_version_set(int unit) {
    int rv = SOC_E_NONE;
    soc_scache_handle_t wb_handle;
    int                      flags = SOC_DPP_SCACHE_DEFAULT, already_exists;
    uint32                   size;
    uint16                   version = 0, recovered_ver;
    uint8                   *scache_ptr;
    soc_dpp_sdk_ver_t       ver;

    SOCDNX_INIT_FUNC_DEFS

    SOC_SCACHE_HANDLE_SET(wb_handle, 0, SOC_MODULE_SDK_VER, 0);

    size = sizeof(soc_dpp_sdk_ver_t);

    rv = soc_dpp_scache_ptr_get(unit, wb_handle, socDppScacheCreate, flags,
                                    &size, &scache_ptr, version, &recovered_ver, &already_exists);
    SOCDNX_IF_ERR_EXIT(rv);

    ver = SOC_DPP_SDK_VER_LATEST;
    sal_memcpy(scache_ptr, &ver, sizeof(soc_dpp_sdk_ver_t));

    

exit:
    SOCDNX_FUNC_RETURN;
}

CONST char*
soc_sdk_version_to_str(soc_dpp_sdk_ver_t scache_ver) {
    if ((scache_ver >= SOC_DPP_SDK_VER_FIRST) ||
        (scache_ver < SOC_DPP_SDK_VER_COUNT)) {
        return soc_dpp_sdk_ver_names[scache_ver];
    } else {
        LOG_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META("'scache_ver' is out of range. scache_ver=%d, min=%d, max=%d"),
                   scache_ver,
                   SOC_DPP_SDK_VER_FIRST,
                   SOC_DPP_SDK_VER_COUNT));
        return "Unknown version";
    }

}

int
soc_warmboot_is_allowed_verify(int unit) {
    int rv = SOC_E_NONE;
    soc_dpp_sdk_ver_t curr_ver, scache_ver;
    uint8 is_allowed=0;

    SOCDNX_INIT_FUNC_DEFS

    rv = soc_sdk_version_get(&curr_ver);
    SOCDNX_IF_ERR_EXIT(rv);

    LOG_VERBOSE(BSL_LS_SOC_INIT,
                (BSL_META_U(unit,
                            "Current SDK version:     %s\n"), soc_sdk_version_to_str(curr_ver)));

    rv = soc_scache_sdk_version_get(unit, &scache_ver);
    SOCDNX_IF_ERR_EXIT(rv);

    LOG_VERBOSE(BSL_LS_SOC_INIT,
                (BSL_META_U(unit,
                            "SDK version from scache: %s\n"), soc_sdk_version_to_str(scache_ver)));

    rv = soc_warmboot_is_allowed(soc_chip_type_map[SOC_DRIVER(unit)->type], scache_ver, curr_ver, &is_allowed);
    LOG_VERBOSE(BSL_LS_SOC_INIT,
                (BSL_META_U(unit,
                            "Warmboot is allowed:     %s\n"), (is_allowed ? "Yes" : "No")));
    SOCDNX_IF_ERR_EXIT(rv);

    if (!is_allowed) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR("warmboot is not allowed\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

