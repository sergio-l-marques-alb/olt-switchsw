

/*
 * $Id: dnxf_port.c,v 1.13 Broadcom SDK $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXF PORT
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC

#ifdef BCM_DNXF_SUPPORT
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/drv.h>

#include <bcm_int/dnxf_dispatch.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/mbcm.h>

/*
 * Function:
 *      soc_dnxf_is_loopback_type_local_get
 * Purpose:
 *      query if loopback type is local
 * Parameters:
 *      unit              - (IN)   unit number.
 *      loopback_type     - (IN)   loopback type
 *      is_local          - (OUT)  is loopback type local
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */

static int
soc_dnxf_is_loopback_type_local_get(int unit, portmod_loopback_mode_t loopback_type, uint32_t *is_local)
{
    SHR_FUNC_INIT_VARS(unit);

    *is_local=FALSE;

    switch (loopback_type){
    case portmodLoopbackMacOuter:
    case portmodLoopbackMacCore:
    case portmodLoopbackMacPCS:
    case portmodLoopbackMacAsyncFifo:
    case portmodLoopbackPhyGloopPCS:
    case portmodLoopbackPhyGloopPMD:
        *is_local=TRUE;
        break;
    default:
        break;
    }

    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_dnxf_port_loopback_set
 * Purpose:
 *      Set port loopback
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  port number
 *      loopback  - (IN)  portmod_loopback_mode_t
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
shr_error_e 
soc_dnxf_port_loopback_set(int unit, soc_port_t port, portmod_loopback_mode_t loopback)
{
    int rc;
    portmod_loopback_mode_t lb_start;
    uint32 cl72_start;
    int phy_lane_config_start;
    uint32 is_lb_start_local=FALSE;
    uint32 is_lb_local=FALSE;
    bcm_port_resource_t resource;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit) || !SOC_IS_DNXF(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_LINK_INPUT_CHECK_DNXC(unit, port);

    DNXF_UNIT_LOCK_TAKE(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_port_loopback_get(unit, port, &lb_start));

    SHR_IF_ERR_EXIT(soc_dnxf_is_loopback_type_local_get(unit, lb_start, &is_lb_start_local));
    SHR_IF_ERR_EXIT(soc_dnxf_is_loopback_type_local_get(unit, loopback, &is_lb_local));

    /* if local loopback enabled ==> store CL72 mode into SW state and disable CL72.
     * Also store the phy_lane_config and configure phy_lane_config suitable to CL72 disabled */
    if ( (is_lb_start_local == FALSE) && (is_lb_local == TRUE) ) {
        SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_get(unit, port, &resource));
        cl72_start = resource.link_training;
        rc = dnxf_state.port.cl72_conf.set(unit, port, cl72_start);
        SHR_IF_ERR_EXIT(rc);

        if (cl72_start == 1) {
            phy_lane_config_start = resource.phy_lane_config;
            rc = dnxf_state.port.phy_lane_config.set(unit, port, phy_lane_config_start);
            SHR_IF_ERR_EXIT(rc);

            resource.link_training = 0;
            resource.phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
            SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_default_get(unit, port, 0, &resource));

            SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_set(unit, port, &resource));
        }
    }

    /* Link integrity must be down when setting MAC remote loopback */
    if (loopback == portmodLoopbackMacRloop)
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_links_link_integrity_down_set,(unit, port, TRUE)));
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_loopback_set,(unit, port, loopback));
    SHR_IF_ERR_EXIT(rc);

    /* if not using MAC remote loopback, don't force link integrity to be down */
    if (loopback != portmodLoopbackMacRloop)
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_links_link_integrity_down_set,(unit, port, FALSE)));
    }

    /* if local loopback disabled ==> restore CL72 mode and phy_lane_config from SW state */
    if ( (is_lb_start_local == TRUE) && (is_lb_local == FALSE) ) {
        rc = dnxf_state.port.cl72_conf.get(unit, port, &cl72_start);
        SHR_IF_ERR_EXIT(rc);

        if (cl72_start == 1) {
            rc = dnxf_state.port.phy_lane_config.get(unit, port, &phy_lane_config_start);
            SHR_IF_ERR_EXIT(rc);

            SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_get(unit, port, &resource));
            resource.link_training = 1;
            resource.phy_lane_config = phy_lane_config_start;
            SHR_IF_ERR_EXIT(bcm_dnxf_port_resource_set(unit, port, &resource));
        }
    }

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_burst_control_set,(unit, port, loopback));
    SHR_IF_ERR_EXIT(rc);
    
    /* additional calibration is needed for MAC remote loopback */
    if (loopback == portmodLoopbackMacRloop)
    {
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_retimer_calibrate,(unit, port, port)));
    }

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      soc_dnxf_port_loopback_get
 * Purpose:
 *      Get port loopback
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      port      - (IN)  port number 
 *      loopback  - (OUT) portmod_loopback_mode_t
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
shr_error_e 
soc_dnxf_port_loopback_get(int unit, soc_port_t port, portmod_loopback_mode_t* loopback)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_UNIT_VALID(unit) || !SOC_IS_DNXF(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }

    DNXF_LINK_INPUT_CHECK_DNXC(unit, port);
    SHR_NULL_CHECK(loopback, _SHR_E_PARAM, "loopback");

    DNXF_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_port_loopback_get,(unit, port, loopback));
    SHR_IF_ERR_EXIT(rc);

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;

}

#endif /* BCM_DNXF_SUPPORT */

#undef BSL_LOG_MODULE

