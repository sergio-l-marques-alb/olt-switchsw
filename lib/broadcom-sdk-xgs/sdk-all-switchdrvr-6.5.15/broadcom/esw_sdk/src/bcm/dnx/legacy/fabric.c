/*

 * $Id: fabric.c,v 1.96 Broadcom SDK $

 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Soc_dnx-B COSQ
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <shared/bsl.h>
#include <shared/gport.h>
#include <shared/shrextend/shrextend_debug.h>

#include "bcm_int/common/debug.h"


#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/fabric.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/control.h>

#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/legacy/utils.h>
#include <bcm_int/dnx/legacy/gport_mgmt.h>
#include <bcm_int/dnx/legacy/utils.h>
#include <bcm_int/dnx/legacy/fabric.h>

#include <bcm_int/common/multicast.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_ipq_alloc_mngr_types.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/scheduler/scheduler.h>
#include <bcm_int/dnx/stk/stk_sys.h>

#include <soc/dnxc/legacy/error.h>
#include <soc/dnx/legacy/mbcm.h>
#include <soc/dnx/legacy/TMC/tmc_api_multicast_fabric.h>
#include <soc/dnx/legacy/TMC/tmc_api_ingress_traffic_mgmt.h>


#include <soc/dnx/legacy/fabric.h>
#include <soc/dnx/legacy/drv.h>
#include <soc/dnxc/legacy/fabric.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnx/legacy/ARAD/arad_mgmt.h>
#include <soc/dnx/legacy/ARAD/arad_fabric.h> 
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ipq_alloc_mngr_access.h>

#include <bcm_int/dnx/fabric/fabric_rci_throttling.h>

/* Number of Ports for Enhance application */
#define DNX_FABRIC_ENHANCE_NOF_PORTS    (4)
#define DNX_FABRIC_ENHANCE_NOF_BE_PORTS (DNX_FABRIC_ENHANCE_NOF_PORTS-1)
#define DNX_SAND_TRUE  1
#define DNX_SAND_FALSE 0

extern int bcm_dnx_stk_my_modid_get(int,int *);



int
bcm_dnx_fabric_init(int unit)
{   
    return BCM_E_UNAVAIL;
}                                        
    
int
bcm_dnx_fabric_port_get(int unit,
                          bcm_gport_t child_port,
                          uint32 flags,
                          bcm_gport_t *parent_port)
{
    int modid = 0, base_modid=0;
    bcm_port_t port = 0, tm_port;
    int    core;
    int    rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(parent_port, _SHR_E_PARAM, "parent_port");

    if (!BCM_GPORT_IS_LOCAL(child_port)) {
        rv = bcm_dnx_stk_my_modid_get(unit, &base_modid);
        SHR_IF_ERR_EXIT(rv);
    }

    if (BCM_GPORT_IS_LOCAL(child_port)) {
        /** IN Local gport -> OUT Local Interface gport  */
        port = BCM_GPORT_LOCAL_GET(child_port);
        BCM_GPORT_LOCAL_INTERFACE_SET(*parent_port, port);

    } else if (BCM_GPORT_IS_MODPORT(child_port)) {

        modid = BCM_GPORT_MODPORT_MODID_GET(child_port);
        tm_port = BCM_GPORT_MODPORT_PORT_GET(child_port);

        if (SOC_DNX_IS_MODID_AND_BASE_MODID_ON_SAME_FAP(unit, modid, base_modid)){
            core = SOC_DNX_MODID_TO_CORE(unit, base_modid, modid);

            SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, (soc_port_t *)&port));

            BCM_GPORT_LOCAL_INTERFACE_SET(*parent_port, port);

        } else {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Gport type unsupported (0x%08x)", child_port);
        }

    } else if (BCM_COSQ_GPORT_IS_E2E_PORT(child_port)) {
        /** IN E2E gport -> OUT E2E Interface gport  */
        port = BCM_COSQ_GPORT_E2E_PORT_GET(child_port);
        BCM_COSQ_GPORT_E2E_INTERFACE_SET(*parent_port, port);
    } else {
        port = child_port;
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
        BCM_GPORT_LOCAL_INTERFACE_SET(*parent_port, port);
    }

exit:
    SHR_FUNC_EXIT;
}

int
legacy_bcm_dnx_fabric_control_set(int unit,
                             bcm_fabric_control_t type,
                             int arg)
{
    uint32 dnx_rv;   

    SHR_FUNC_INIT_VARS(unit);
    BCM_DNX_UNIT_CHECK(unit);

    switch (type) {
        case bcmFabricShaperQueueMin:            
        case bcmFabricShaperQueueMax:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ISQs are not supported.");
            break;
        case bcmFabricQueueMin:
        case bcmFabricQueueMax:
        case bcmFabricMulticastQueueMin:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Use bcmFabricMulticastQueueMax to set max FMQ range (min FMQ range is 0).");
            break;
        case bcmFabricTrafficManagementCosMode:
            break;

        case bcmFabricCellSizeFixed:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported on this device.\n");
            break;
        case bcmFabricCellSizeMin:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported on this device.\n");
            break;

        case bcmFabricCellSizeMax:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported on this device.\n");
            break;

        case bcmFabricVsqCategory:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unit %d, type %d is not supported on this device.\n", unit, type);
            break;
        case bcmFabricRecycleQueueMin:
        case bcmFabricRecycleQueueMax:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unit %d, type %d is not supported on this device.\n", unit, type);
            break;
        
       case bcmFabricRCIControlSource:
           dnx_rv = MBCM_DNX_DRIVER_CALL(unit,mbcm_dnx_fabric_rci_enable_set,(unit, arg));
           SHR_IF_ERR_EXIT(dnx_rv);
           break;

       case bcmFabricRCIIncrementValue:
           dnx_rv = MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_rci_config_set, (unit, SOC_DNX_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE, arg));
           SHR_IF_ERR_EXIT(dnx_rv);
           break;

       case bcmFabricForceTdmBypassTrafficToFabric:
           SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported on this device.\n");
           break;
       default:
           SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d", type);
    }

exit:
    SHR_FUNC_EXIT;
}



int
legacy_bcm_dnx_fabric_control_get(int unit,
                             bcm_fabric_control_t type,
                             int *arg)
{
    uint32 dnx_rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);
    switch (type) {
        case bcmFabricRecycleQueueMin:
        case bcmFabricRecycleQueueMax:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unit %d, type %d is not supported on this device.\n", unit, type);
            break;
        case bcmFabricTrafficManagementCosMode:
            break;
        case bcmFabricCellSizeFixed:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported on this device.\n");
            break;
        case bcmFabricCellSizeMin:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported on this device.\n");
            break;
        case bcmFabricCellSizeMax:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported on this device.\n");
            break;

        case bcmFabricVsqCategory:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "unit %d, type %d is not supported on this device.\n", unit, type);
            break;
        case bcmFabricRCIControlSource:
           dnx_rv = MBCM_DNX_DRIVER_CALL(unit,mbcm_dnx_fabric_rci_enable_get,(unit, (soc_dnxc_fabric_control_source_t*) arg));
           SHR_IF_ERR_EXIT(dnx_rv);
           break;

       case bcmFabricRCIIncrementValue:
           dnx_rv = MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_rci_config_get, (unit, SOC_DNX_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE, arg));
           SHR_IF_ERR_EXIT(dnx_rv);
           break;

       case bcmFabricForceTdmBypassTrafficToFabric:
           SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported on this device.\n");
           break;

       default:
           SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d", type);
    }


exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_bandwidth_core_profile_set(int unit, int core, uint32 flags,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is deprecatated. Use bcm_cosq_bandwidth_fabric_adjust_set instead\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_bandwidth_core_profile_get(int unit, int core, uint32 flags,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is deprecatated. Use bcm_cosq_bandwidth_fabric_adjust_get instead\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_bandwidth_profile_set(int unit,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is deprecatated. Use bcm_cosq_bandwidth_fabric_adjust_set instead\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_bandwidth_profile_get(int unit,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "This API is deprecatated. Use bcm_cosq_bandwidth_fabric_adjust_get instead\n");

exit:
    SHR_FUNC_EXIT;
}


/*
 * Purpose: Set Enhance mode
 */
int
dnx_fabric_multicast_scheduler_mode_set(int unit,
                                        int mode)
{
    shr_error_e rc = _SHR_E_NONE;
    uint32 hr_id[DNX_FABRIC_ENHANCE_NOF_PORTS] = {0};
    int index, set_required = 0, core;
    int is_fmq_allocated = 0;
    SOC_DNX_MULT_FABRIC_INFO fabric_info;    

    SHR_FUNC_INIT_VARS(unit);
    if (mode < 0 || mode > 1) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "mode %d invalid",mode);
    }

    /* MC Scheduler Mode should be configured in all active cores */
    DNXCMN_CORES_ITER(unit,_SHR_CORE_ALL, core) {

        /** get current info */
        rc = (MBCM_DNX_DRIVER_CALL(unit,mbcm_dnx_mult_fabric_credit_source_get,(unit, core, &fabric_info)));
        SHR_IF_ERR_EXIT(rc);

        if (mode == 0) {
            /** Simple mode -- 4 queues connected directly to credit generator  - no HRs */
            if (fabric_info.credits_via_sch == DNX_SAND_TRUE) {
                /* Apply changes, enhance was enabled */
                /* 1. Disable enhance */
                /* 2. Disalloc HRs */
                /* 3. Set ports configuration to be invalid */

                /* Disable enhance */
                fabric_info.credits_via_sch = DNX_SAND_FALSE;  

                hr_id[0] = fabric_info.guaranteed.gr_sch.mcast_class_hr_id;
                for (index = 0; index < DNX_FABRIC_ENHANCE_NOF_BE_PORTS; index++) {
                    hr_id[index+1] = fabric_info.best_effort.be_sch[index].be_sch.mcast_class_hr_id;            
                }

                /* Dealloc HRs of FMQ scheduling hierarchy */
                rc = dnx_scheduler_fmq_hr_deallocate(unit, core, DNX_FABRIC_ENHANCE_NOF_PORTS, hr_id);
                SHR_IF_ERR_EXIT(rc);
    
                /* Disable ports configuration */
                fabric_info.guaranteed.gr_sch.mcast_class_hr_id = 0;
                fabric_info.guaranteed.gr_sch.multicast_class_valid = DNX_SAND_FALSE;
                for (index = 0; index < DNX_FABRIC_ENHANCE_NOF_PORTS-1; index++) {
                    fabric_info.best_effort.be_sch[index].be_sch.mcast_class_hr_id = 0;
                    fabric_info.best_effort.be_sch[index].be_sch.multicast_class_valid = DNX_SAND_FALSE;
                }     
                
                set_required = 1;   
            }          
        } else {
            /** Advanced mode -- credit generator generates credits for 4 HRs in scheduler */
            if (fabric_info.credits_via_sch == DNX_SAND_FALSE) {
                /* Apply changes , enhance was disabled */
                /* 1. Enable enhance */
                /* 2. Alloc HRs */
                /* 3. Set ports configuration */

                /* Enable enhance */
                fabric_info.credits_via_sch = DNX_SAND_TRUE;
        
                /* Obtain  HRs for FMQ scheduling hierarchy */
                rc = dnx_scheduler_fmq_hr_allocate(unit, core, DNX_FABRIC_ENHANCE_NOF_PORTS, hr_id);
                SHR_IF_ERR_EXIT(rc);
        
                /* Set ports configruation */
                fabric_info.guaranteed.gr_sch.mcast_class_hr_id = hr_id[0];
                fabric_info.guaranteed.gr_sch.multicast_class_valid = DNX_SAND_TRUE;
                for (index = 0; index < DNX_FABRIC_ENHANCE_NOF_PORTS-1; index++) {
                    fabric_info.best_effort.be_sch[index].be_sch.mcast_class_hr_id = hr_id[index+1];
                    fabric_info.best_effort.be_sch[index].be_sch.multicast_class_valid = DNX_SAND_TRUE;
                }

                set_required = 1;
            }
        }
          
        /* Commit configuration */
        if(set_required) {
            rc = (MBCM_DNX_DRIVER_CALL(unit,mbcm_dnx_mult_fabric_credit_source_set,(unit, core, &fabric_info)));
            SHR_IF_ERR_EXIT(rc);    

            /** Update sw_state */
            SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.is_enhance_scheduler_mode.set(unit, fabric_info.credits_via_sch));
        }
    }
            
    /*
     * Update FMQ 0 allocation if mode has been changed.
     */
    if (set_required)
    {
        if (mode == 0)
        {
            int base_queue = 0;
            /*
             * When switching back to simple scheduling mode, need to make sure
             * that the first queue quartet is saved for FMQ.
             */
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_allocate
                    (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, DNX_ALGO_IPQ_QUEUE_TYPE_UNICAST,
                     DNX_IPQ_FMQ_BUNDLE_SIZE(unit), &base_queue));
        }
        else
        {
            /*
             * On init we allocated dummy queue 0 as unicast, even though it is really multicast queue.
             * Now we need to deallocate this dummy queue in order to be able to allocate multicast
             * queues in this region.
             */
            SHR_IF_ERR_EXIT(dnx_ipq_alloc_mngr_db.fmq.is_fmq_allocated.get(unit, &is_fmq_allocated));
            if (is_fmq_allocated == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_RESOURCE, "Need to deallocate FMQ 0 in order to change multicast scheduler mode.");
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_cosq_ipq_queue_deallocate(unit, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Purpose: Get Enhance mode
 */
int
dnx_fabric_multicast_scheduler_mode_get(int unit,
                                        int* mode)
{
    int rc;
    SOC_DNX_MULT_FABRIC_INFO fabric_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(mode, _SHR_E_PARAM, "mode");    

    SOC_DNX_MULT_FABRIC_INFO_clear(&fabric_info);
    
    rc = (MBCM_DNX_DRIVER_CALL(unit,mbcm_dnx_mult_fabric_credit_source_get,(unit, BCM_CORE_ALL, &fabric_info)));
    SHR_IF_ERR_EXIT(rc);

    *mode = (fabric_info.credits_via_sch == DNX_SAND_TRUE) ? 1:0;

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_fabric_link_connectivity_status_get(
    int unit, 
    int link_partner_max, 
    bcm_fabric_link_connectivity_t *link_partner_array, 
    int *link_partner_count)
{
    int i, port_i;
    uint32 dnx_rv;
    int nof_links = 0;
    SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR connectivity_map;
    
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(link_partner_array, _SHR_E_PARAM, "link_partner_array");
    SHR_NULL_CHECK(link_partner_count, _SHR_E_PARAM, "link_partner_count");
    
    BCM_DNX_UNIT_CHECK(unit);       
    
    SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(&connectivity_map);
    
    nof_links = dnx_data_fabric.links.nof_links_get(unit);

    dnx_rv = (MBCM_DNX_DRIVER_CALL(unit,mbcm_dnx_fabric_topology_status_connectivity_get,
        (unit, 0, nof_links - 1, &connectivity_map)));

    SHR_IF_ERR_EXIT(dnx_rv);
    
    *link_partner_count = 0;
    

    PBMP_SFI_ITER(unit, port_i)
    {
        i = SOC_DNX_FABRIC_PORT_TO_LINK(unit, port_i);
        if(*link_partner_count >= link_partner_max)
        {
            *link_partner_count = 0;
            SHR_ERR_EXIT(_SHR_E_PARAM, "link_partner_max %d is too small",link_partner_max);
        }

        if(connectivity_map.link_info[i].far_unit != SOC_DNX_DEVICE_ID_IRRELEVANT
           && connectivity_map.link_info[i].is_logically_connected)
        {                      
            link_partner_array[*link_partner_count].module_id   = connectivity_map.link_info[i].far_unit;
            link_partner_array[*link_partner_count].link_id     = connectivity_map.link_info[i].far_link_id;
            switch(connectivity_map.link_info[i].far_dev_type)
            {
                  case SOC_DNX_FAR_DEVICE_TYPE_FE1:
                  case SOC_DNX_FAR_DEVICE_TYPE_FE3:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeFE13;
                  break;
                
                  case SOC_DNX_FAR_DEVICE_TYPE_FE2:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeFE2;
                  break;
                
                  case SOC_DNX_FAR_DEVICE_TYPE_FAP:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeFAP;
                  break;
                
                  default:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeUnknown;
                    break;
            }                        
        }
        else
        {
            link_partner_array[*link_partner_count].link_id = BCM_FABRIC_LINK_NO_CONNECTIVITY;
        }
        (*link_partner_count)++; 
    }
    
exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_fabric_link_connectivity_status_single_get(
    int unit, 
    bcm_port_t link_id, 
    bcm_fabric_link_connectivity_t *link_partner_info)
{
    uint32 dnx_rv;
    int link_index;
    SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR connectivity_map;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(link_partner_info, _SHR_E_PARAM, "link_partner_info");

    if(!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), link_id)){
        SHR_ERR_EXIT(_SHR_E_PORT, "unit %d, Port %d is out-of-range", unit, link_id);
    }       

    SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(&connectivity_map);

    link_index = SOC_DNX_FABRIC_PORT_TO_LINK(unit, link_id);
    dnx_rv = (MBCM_DNX_DRIVER_CALL(unit,mbcm_dnx_fabric_topology_status_connectivity_get,(unit, link_index, link_index, &connectivity_map)));
    SHR_IF_ERR_EXIT(dnx_rv);

    if(connectivity_map.link_info[link_index].far_unit != SOC_DNX_DEVICE_ID_IRRELEVANT
            && connectivity_map.link_info[link_index].is_logically_connected)
    {                      
        link_partner_info->module_id   = connectivity_map.link_info[link_index].far_unit;
        link_partner_info->link_id     = connectivity_map.link_info[link_index].far_link_id;
        switch(connectivity_map.link_info[link_index].far_dev_type)
        {
            case SOC_DNX_FAR_DEVICE_TYPE_FE1:
            case SOC_DNX_FAR_DEVICE_TYPE_FE3:
            {
                link_partner_info->device_type = bcmFabricDeviceTypeFE13;
                break;
            }
            case SOC_DNX_FAR_DEVICE_TYPE_FE2:
            {
                link_partner_info->device_type = bcmFabricDeviceTypeFE2;
                break;
            }
            case SOC_DNX_FAR_DEVICE_TYPE_FAP:
            {
                link_partner_info->device_type = bcmFabricDeviceTypeFAP;
                break;
            }
            default:
            {
                link_partner_info->device_type = bcmFabricDeviceTypeUnknown;
                break;
            }
        }                        
    }
    else
    {
        link_partner_info->link_id = BCM_FABRIC_LINK_NO_CONNECTIVITY;
    }

exit:
    SHR_FUNC_EXIT;
}


int 
bcm_dnx_fabric_link_status_get(
    int unit, 
    bcm_port_t link_id, 
    uint32 *link_status, 
    uint32 *errored_token_count)
{
    uint32 link_status_tmp;
    bcm_port_t link_idx;
    SHR_FUNC_INIT_VARS(unit);

    if(!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), link_id)){
        SHR_ERR_EXIT(_SHR_E_PORT, "unit %d, Port %d is out-of-range", unit, link_id);
    }

    link_idx = SOC_DNX_FABRIC_PORT_TO_LINK(unit, link_id);
    
    SHR_NULL_CHECK(link_status, _SHR_E_PARAM, "link_status");
    SHR_NULL_CHECK(errored_token_count, _SHR_E_PARAM, "errored_token_count");

    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_link_status_get, (unit, link_idx, &link_status_tmp, errored_token_count)));
    
    *link_status = 0;
    if(link_status_tmp & DNXC_FABRIC_LINK_STATUS_CRC_ERROR) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_CRC_ERROR;
    }
    if(link_status_tmp & DNXC_FABRIC_LINK_STATUS_SIZE_ERROR) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_SIZE_ERROR;
    }
    if(link_status_tmp & DNXC_FABRIC_LINK_STATUS_MISALIGN) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_MISALIGN;
    }
    if(link_status_tmp & DNXC_FABRIC_LINK_STATUS_CODE_GROUP_ERROR) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR;
    }
    if(*errored_token_count < 63) {
      (*link_status) |= BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS; 
    }
    
exit:
    SHR_FUNC_EXIT;
}


int
bcm_dnx_fabric_tdm_editing_set(int unit,
                                 bcm_gport_t gport,
                                 bcm_fabric_tdm_editing_t *editing)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported by device.\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_tdm_editing_get(int unit,
                                 bcm_gport_t gport,
                                 bcm_fabric_tdm_editing_t *editing)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported by device.\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_tdm_direct_routing_set(int unit,
                                  bcm_gport_t gport,
                                  bcm_fabric_tdm_direct_routing_t *routing_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "TDM is not supported by device.\n");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_tdm_direct_routing_get(int unit,
                                  bcm_gport_t gport,
                                  bcm_fabric_tdm_direct_routing_t *routing_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API is not supported.\n");

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *      bcm_dnx_fabric_reachability_status_get
 * Purpose:
 *      Retrieves the links through which a remote module ID is
 *      reachable
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      moduleid    - (IN)  Module Id 
 *      links_max   - (IN)  Max size of links_array. If the size doesn't correlate to the device's number of links, 
                            the function returns with the error _SHR_E_FULL
 *      links_array - (OUT) bitmap of the links from which 'moduleid' is reachable 
 *      links_count - (OUT) Size of links_array 
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 *      No support for Soc_dnx-B.  
 */
int 
bcm_dnx_fabric_reachability_status_get(
    int unit, 
    int moduleid, 
    int links_max, 
    uint32 *links_array, 
    int *links_count)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(links_array, _SHR_E_PARAM, "links_array");
    SHR_NULL_CHECK(links_count, _SHR_E_PARAM, "links_count");

    SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit,mbcm_dnx_fabric_reachability_status_get,(unit, moduleid, links_max, links_array, links_count)));

exit:     
    SHR_FUNC_EXIT; 
}

int bcm_dnx_fabric_link_thresholds_pipe_set(
    int unit, 
    int fifo_type, 
    bcm_fabric_pipe_t pipe, 
    uint32 flags, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Function is not supported. Please read User Manual and use bcm_dnx_fabric_cgm_control_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

int bcm_dnx_fabric_link_thresholds_pipe_get(
    int unit, 
    int fifo_type, 
    bcm_fabric_pipe_t pipe, 
    uint32 flags, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Function is not supported. Please read User Manual and use bcm_dnx_fabric_cgm_control_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_fabric_destination_link_min_set(
      int unit,
      uint32 flags,
      bcm_module_t module_id,
      int num_of_links)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_DNX_IS_MESH(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "minimum number of links configuration not supported in mesh");
    }

    SHR_MASK_VERIFY(flags, BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE, _SHR_E_PARAM, "Unrecognized flags");

    if (!(BCM_FABRIC_NUM_OF_LINKS_IS_VALID(unit, num_of_links)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "number of links is invalid");
    }

    if (flags & BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE)
    {
        if (module_id != BCM_MODID_ALL && module_id != -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Please set module_id to be -1 or BCM_MODID_ALL, "
                    "since the configuration is done for all module ids when configuring the all-reachable-vector.");
        }

        SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_minimal_links_all_reachable_set, 
                                                 (unit, num_of_links)));
    }
    else
    {
        if ((module_id != BCM_MODID_ALL) && !(BCM_FABRIC_MODID_IS_VALID(unit, module_id)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "module id %d is invalid", module_id);
        }

        SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_minimal_links_to_dest_set, 
                                                     (unit, module_id, num_of_links)));
    }

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_fabric_destination_link_min_get(
      int unit,
      uint32 flags,
      bcm_module_t module_id,
      int* num_of_links)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(num_of_links, _SHR_E_PARAM, "num_of_links");

    if (SOC_DNX_IS_MESH(unit))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "minimum number of links configuration not supported in mesh");
    }

    SHR_MASK_VERIFY(flags, BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE, _SHR_E_PARAM, "Unrecognized flags");

    if (flags & BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE)
    {
        if (module_id != BCM_MODID_ALL && module_id != -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Please set module_id to be -1 or BCM_MODID_ALL, "
                    "since the configuration is done for all module ids when configuring the all-reachable-vector.");
        }

        SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_minimal_links_all_reachable_get,
                                                 (unit, num_of_links)));
    }
    else
    {
        if ((module_id != BCM_MODID_ALL) && !(BCM_FABRIC_MODID_IS_VALID(unit, module_id)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "module id %d is invalid", module_id);
        }

        SHR_IF_ERR_EXIT(MBCM_DNX_DRIVER_CALL(unit, mbcm_dnx_fabric_minimal_links_to_dest_get, 
                                                 (unit, module_id, num_of_links)));
    }

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_fabric_rci_config_set(
      int unit,
      bcm_fabric_rci_config_t rci_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Function is not supported. Please read User Manual and use bcm_dnx_fabric_cgm_control_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_fabric_rci_config_get(
      int unit,
      bcm_fabric_rci_config_t* rci_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Function is not supported. Please read User Manual and use bcm_dnx_fabric_cgm_control_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_static_replication_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    uint32 destid_count,
    bcm_module_t * destid_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Function is not supported. Please read User Manual and use bcm_fabric_multicast_set() instead.");

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
