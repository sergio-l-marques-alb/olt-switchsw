/*
 * $Id:$
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        port.c
 * Purpose:     MV2 SOC Port driver.
 *
 */

#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/defs.h>
#include <soc/tdm/core/tdm_top.h>

#if defined(BCM_FIREBOLT6_SUPPORT)
/* #include <soc/tdm/maverick2/tdm_fb6_defines.h> */
#include <soc/maverick2.h>
#include <soc/scache.h>

#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_FLEXPORT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_FLEXPORT_WB_DEFAULT_VERSION            SOC_FLEXPORT_WB_VERSION_1_0

extern int
_soc_fb6_port_speed_cap[SOC_MAX_NUM_DEVICES][132];
#endif
/*
 * Function:
 *      soc_fb6_max_lr_bandwidth
 * Purpose:
 *      Get the max line rate bandwidth per pipeline
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      max_lr_bw            - (OUT) Max line rate bandwidth (Gbps).
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
soc_fb6_max_lr_bandwidth(int unit, uint32 *max_lr_bw)
{
    soc_info_t *si = &SOC_INFO(unit);
    int cal_universal;

    cal_universal = si->fabric_port_enable ? 1 : 0;

    switch (si->frequency) {
    case 1700:
        *max_lr_bw = cal_universal ? 1050 : 1095;
        break;
    case 1625:
        *max_lr_bw = cal_universal ? 1002 : 1047;
        break;
    case 1525:
        *max_lr_bw = cal_universal ? 940 : 980;
        break;
    case 1425:
        *max_lr_bw = cal_universal ? 877 : 915;
        break;
    case 1325:
        *max_lr_bw = cal_universal ? 812 : 847;
        break;
    case 1275:
        *max_lr_bw = cal_universal ? 782 : 815;
        break;
    case 1225:
        *max_lr_bw = cal_universal ? 750 : 782;
        break;
    case 1125:
        *max_lr_bw = cal_universal ? 687 : 717;
        break;
    case 1050:
        *max_lr_bw = cal_universal ? 640 : 667;
        break;
    case 950:
        *max_lr_bw = cal_universal ? 575 : 600;
        break;
    case 850:
        *max_lr_bw = cal_universal ? 512 : 535;
        break;
    default:
        *max_lr_bw = 0;
        break;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_fb6_support_speeds
 * Purpose:
 *      Get the supported speed of port for specified lanes
 * Parameters:
 *      unit                 - (IN)  Unit number.
 *      lanes                - (IN)  Number of Lanes for the port.
 *      speed_mask           - (IN)  Bitmap for supported speed.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
soc_fb6_support_speeds(int unit, int lanes, uint32 *speed_mask)
{
    soc_info_t *si = &SOC_INFO(unit);
    uint32 speed_valid;

    switch(lanes) {
    case 1:
        /* Single Lane Port Speeds */
        speed_valid = SOC_PA_SPEED_1000MB | SOC_PA_SPEED_10GB |
                        SOC_PA_SPEED_11GB | SOC_PA_SPEED_25GB |
                        SOC_PA_SPEED_27GB;
        break;
    case 2:
        /* Dual Lane Port Speeds */
        speed_valid = SOC_PA_SPEED_20GB | SOC_PA_SPEED_21GB |
                        SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB |
                        SOC_PA_SPEED_50GB | SOC_PA_SPEED_53GB;
        break;
    case 4:
        /* Quad Lane Port Speeds */
        speed_valid = SOC_PA_SPEED_40GB | SOC_PA_SPEED_42GB |
                        SOC_PA_SPEED_100GB | SOC_PA_SPEED_106GB;
        break;
    default:
        return SOC_E_PARAM;
    }

    if (FALSE == si->fabric_port_enable) {
        /* Ethernet-optimized Calendar doesn't support any HG speed */
        speed_valid &= ~(SOC_PA_SPEED_11GB | SOC_PA_SPEED_21GB |
                            SOC_PA_SPEED_27GB | SOC_PA_SPEED_42GB |
                            SOC_PA_SPEED_53GB | SOC_PA_SPEED_106GB);
    } else if (si->frequency < 1125) {
        speed_valid &= ~(SOC_PA_SPEED_27GB | SOC_PA_SPEED_53GB |
                            SOC_PA_SPEED_106GB);
    }

    if (si->frequency < 1050) {
        speed_valid &= ~(SOC_PA_SPEED_25GB | SOC_PA_SPEED_50GB |
                            SOC_PA_SPEED_100GB);
    }

    *speed_mask = speed_valid;

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int soc_fb6_flexport_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int stable_size;
    int default_ver = SOC_FLEXPORT_WB_DEFAULT_VERSION;

    alloc_size =  (sizeof(int) * _TD3_PORTS_PER_DEV(unit))  + /* phy to logical*/
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* logical to phy */
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* max port speed */
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* init port speed */
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* num of lanes */
                   (sizeof(pbmp_t));                        /* Disabled bitmap */
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          TRUE, &alloc_get,
                                          &flexport_scache_ptr,
                                          default_ver,
                                          NULL);

    if (rv  == SOC_E_CONFIG) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory for flexport size mismatch"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory not allocated for flexport"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_fb6_flexport_scache_sync
 * Purpose:
 *      Record Port info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    port_p2l_mapping
 *    port_l2p_mapping
 *    port_speed_max
 *    port_init_speed
 *    port_num_lanes
 *    disabled_bitmap
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int soc_fb6_flexport_scache_sync(int unit)
{
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 scache_offset=0;
    int rv = 0;

    alloc_size =  (sizeof(int) * _TD3_PORTS_PER_DEV(unit))  + /* phy to logical*/
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* logical to phy */
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* max port speed */
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* init port speed */
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* num of lanes */
                   (sizeof(pbmp_t));                        /* Disabled bitmap */

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &flexport_scache_ptr,
                        SOC_FLEXPORT_WB_DEFAULT_VERSION,
                        NULL);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory for flexport size mismatch"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }
    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory not allocated for flexport"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* Physical to logical port mapping */
    var_size = sizeof(int) * _TD3_PORTS_PER_DEV(unit);
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2l_mapping, var_size);
    scache_offset += var_size;

    var_size = sizeof(int) * _TD3_PORTS_PER_DEV(unit);

    /* Logical to Physical port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_l2p_mapping, var_size);
    scache_offset += var_size;

    /* Max port speed */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_speed_max, var_size);
    scache_offset += var_size;

    /* Init port speed */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_init_speed, var_size);
    scache_offset += var_size;

    /* Num of lanes */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_num_lanes, var_size);
    scache_offset += var_size;

    /* Disabled Port Bitmap */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->all.disabled_bitmap,
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_fb6_flexport_scache_recovery
 * Purpose:
 *      Recover Port info that maybe changed during flexport for Level 2 WB
 *
 * Warm Boot Version Map:
 *
 *  BCM_WB_VERSION_1_0
 *    port_p2l_mapping
 *    port_l2p_mapping
 *    port_speed_max
 *    port_init_speed
 *    port_num_lanes
 *    disabled_bitmap
 *
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      SOC_E_XXX
 */
int soc_fb6_flexport_scache_recovery(int unit)
{
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size = 0;
    uint16 recovered_ver = 0;
    soc_info_t *si = &SOC_INFO(unit);

    alloc_size =  (sizeof(int) * _TD3_PORTS_PER_DEV(unit))  + /* phy to logical*/
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* logical to phy */
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* max port speed */
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* init port speed */
                   (sizeof(int) * _TD3_PORTS_PER_DEV(unit)) + /* num of lanes */
                   (sizeof(pbmp_t));                        /* Disabled bitmap */

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &flexport_scache_ptr,
                        SOC_FLEXPORT_WB_DEFAULT_VERSION,
                        &recovered_ver);
    if (SOC_FAILURE(rv)) {
        if ((rv == SOC_E_CONFIG) ||
            (rv == SOC_E_NOT_FOUND)) {
            /* warmboot file does not contain this
            * module, or the warmboot state does not exist.
            * in this case return SOC_E_NOT_FOUND
            */
            return SOC_E_NOT_FOUND;
        } else {
            /* Only Level2 - flexport treat this as a error */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                     "Failed to recover scache data - %s\n"),soc_errmsg(rv)));
            return rv;
        }
    }
    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache recovery for flexport"
                 "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache recovery for flexport"
                 "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /*Physical to logical port mapping */
    var_size = (sizeof(int) * _TD3_PORTS_PER_DEV(unit));
    sal_memcpy(si->port_p2l_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    var_size = (sizeof(int) * _TD3_PORTS_PER_DEV(unit));

    /*Logical to Physical port mapping*/
    sal_memcpy(si->port_l2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Max port speed */
    sal_memcpy(_soc_fb6_port_speed_cap[unit],
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Init port speed */
    sal_memcpy(si->port_speed_max,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Num of lanes */
    sal_memcpy(si->port_num_lanes,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Disabled Port Bitmap */
    sal_memcpy(&si->all.disabled_bitmap,
           &flexport_scache_ptr[scache_offset],
           sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
void
soc_fb6_flexport_sw_dump(int unit)
{
    int port, phy_port, mmu_port, pipe, pm, cosq, numq, uc_cosq, uc_numq;
    int max_speed, init_speed, num_lanes;
    char  pfmt[SOC_PBMP_FMT_LEN];

    LOG_CLI((BSL_META_U(unit,
                            "  port(log/phy/mmu)  pipe  pm  lanes    "
                            "speed(Max)    uc_Qbase/Numq mc_Qbase/Numq\n")));
    PBMP_ALL_ITER(unit, port) {
        pipe = SOC_INFO(unit).port_pipe[port];
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        num_lanes = SOC_INFO(unit).port_num_lanes[port];
        pm = SOC_INFO(unit).port_serdes[port];
        max_speed = SOC_INFO(unit).port_speed_max[port];
        init_speed = SOC_INFO(unit).port_init_speed[port];
        cosq = SOC_INFO(unit).port_cosq_base[port];
        numq = SOC_INFO(unit).port_num_cosq[port];
        uc_cosq = SOC_INFO(unit).port_uc_cosq_base[port];
        uc_numq = SOC_INFO(unit).port_num_uc_cosq[port];

        LOG_CLI((BSL_META_U(unit,
                            "  %4s(%3d/%3d/%3d)  %4d  %2d  %5d %7d(%7d) "
                            "%6d/%-6d  %6d/%-6d\n"),
                SOC_INFO(unit).port_name[port], port, phy_port, mmu_port,
                pipe, pm, num_lanes, init_speed, max_speed,
                uc_cosq, uc_numq, cosq, numq));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n    Oversub Bitmap: %s"),
       SOC_PBMP_FMT(SOC_INFO(unit).oversub_pbm, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "\n    Disabled Bitmap: %s \n"),
       SOC_PBMP_FMT(SOC_INFO(unit).all.disabled_bitmap, pfmt)));
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif /* BCM_FIREBOLT6_SUPPORT */
