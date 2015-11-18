/*

 * $Id: fabric.c,v 1.96 Broadcom SDK $

 *
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
 * Soc_petra-B COSQ
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FABRIC

#include <shared/bsl.h>

#include "bcm_int/common/debug.h"

#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/fabric.h>
#include <shared/gport.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/control.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_cosq.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/fabric.h>
#include <bcm_int/dpp/port.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/wb_db_cosq.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/TMC/tmc_api_multicast_fabric.h>
#include <soc/dpp/TMC/tmc_api_ingress_traffic_mgmt.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/fabric.h>
#include <soc/dpp/drv.h>
#include <soc/dcmn/fabric.h>
#ifdef BCM_ARAD_SUPPORT
    #include <soc/dpp/ARAD/arad_api_mgmt.h>
    #include <soc/dpp/ARAD/arad_fabric.h> 
    #include <soc/dpp/ARAD/arad_ports.h>
    #include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>
#endif
/* Number of Ports for Enhance application */
#define DPP_FABRIC_ENHANCE_NOF_PORTS    (4)
#define DPP_FABRIC_ENHANCE_NOF_BE_PORTS (DPP_FABRIC_ENHANCE_NOF_PORTS-1)
#define SOC_SAND_TRUE  1
#define SOC_SAND_FALSE 0

#define DPP_TDM_DIRECT_ROUTING_DEFAULT_PROFILE (0)




STATIC int 
_bcm_petra_fabric_vsq_category_set(int unit,                                   
                                   bcm_fabric_control_t control_type,
                                   int queue_id_in,
                                   int *queue_id_out);
STATIC int 
_bcm_petra_fabric_vsq_category_get(int unit,                                   
                                   bcm_fabric_control_t control_type,
                                   int *queue_id);
STATIC int
_bcm_petra_fabric_multicast_queue_range_set(int unit,
                                            bcm_fabric_control_t control_type,
                                            int queue_id);
STATIC int
_bcm_petra_fabric_multicast_queue_range_get(int unit,
                                            bcm_fabric_control_t control_type,
                                            int* queue_id);
STATIC int
_bcm_petra_fabric_unicast_queue_range_set(int unit,
                                          bcm_fabric_control_t type,
                                          int queue_id);
STATIC int
_bcm_petra_fabric_unicast_queue_range_get(int unit,
                                          bcm_fabric_control_t control_type,
                                          int* queue_id);
STATIC int
_bcm_petra_fabric_ingress_shaper_queue_range_set(int unit,
                         bcm_fabric_control_t type,
                         int queue_id);
STATIC int
_bcm_petra_fabric_ingress_shaper_queue_range_get(int unit,
                         bcm_fabric_control_t control_type,
                         int* queue_id);
STATIC int
_bcm_petra_fabric_egress_queue_range_set(int unit,
                                          bcm_fabric_control_t type,
                                          int queue_id);
STATIC int
_bcm_petra_fabric_egress_queue_range_get(int unit,
                                         bcm_fabric_control_t control_type,
                                         int* queue_id);
STATIC int
_bcm_petra_fabric_multicast_scheduler_mode_set(int unit,
                                               int mode);
STATIC int
_bcm_petra_fabric_multicast_scheduler_mode_get(int unit,
                                               int* mode);
STATIC int
_bcm_petra_fabric_credit_watchdog_range_get(int unit,
                                            bcm_fabric_control_t control_type,
                                            int *queue_id);
STATIC int
_bcm_petra_fabric_enhance_ports_alloc(int unit,
                                    int *ports,
                                    int nof_ports);
STATIC int
_bcm_petra_fabric_enhance_ports_disalloc(int unit,
                                    int *ports,
                                    int nof_ports);
STATIC int
_bcm_petra_fabric_tdm_range_size_set(int unit,
                                     int minimum_size,
                                     int maximum_size); 

STATIC int
_bcm_petra_fabric_tdm_range_size_get(int unit,
                                     int *minimum_size,
                                     int *maximum_size);

STATIC int
_bcm_petra_fabric_tdm_from_action_to_editing_type(int unit,
                                                  int is_ingress,
                                                  SOC_TMC_TDM_FTMH_INFO *ftmh_info,
                                                  bcm_fabric_tdm_editing_type_t *editing_type);

STATIC int
_bcm_petra_fabric_tdm_to_action_from_editing_type(int unit,
                                                  int is_ingress,
                                                  SOC_TMC_TDM_FTMH_INFO *ftmh_info,
                                                  bcm_fabric_tdm_editing_type_t *editing_type);
int
bcm_petra_fabric_init(int unit)
{   
    bcm_error_t rc = BCM_E_NONE;
    uint8 enable_rpt_reachable = FALSE;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (SOC_IS_JERICHO(unit)) { 
        bcm_port_t link;
        int repeater_link_enable;

        /*links that connected to a repeater*/
        PBMP_SFI_ITER(unit, link) {
            repeater_link_enable = soc_property_port_get(unit, link, spn_REPEATER_LINK_ENABLE, 0);
            if (repeater_link_enable) {
                BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_link_control_set(unit, link, bcmFabricLinkRepeaterEnable, 1));
            }
        }
        
        BCMDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_JER_MODID_GROUP_MAP_TO_DEST)); /*should be placed at the end of jer fabric_init*/
        BCM_EXIT;
    }

    /* Initalize TDM fabric direct routing */
    if (SOC_DPP_CONFIG(unit)->tdm.is_bypass && !SOC_WARM_BOOT(unit)) {
      SOC_TMC_TDM_DIRECT_ROUTING_INFO direct_routing;
      int template_init_id;
      uint32 soc_sand_rc;
      SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(&direct_routing);

      /* By default enable all links */
      direct_routing.link_bitmap.arr[0] = 0xffffffff;
      direct_routing.link_bitmap.arr[1] = 0xf;

      /* Set HW direct routing */
      template_init_id = DPP_TDM_DIRECT_ROUTING_DEFAULT_PROFILE;

      soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_direct_routing_set,(unit, template_init_id, &direct_routing, enable_rpt_reachable)));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

      /* Init template management */
      rc = _bcm_dpp_am_template_fabric_tdm_link_ptr_init(unit,template_init_id,&direct_routing);
      BCMDNX_IF_ERR_EXIT(rc);
    }
#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit)){

        /* If credit watchdog configuration is not set, set or restore it from hardware */
        if (IS_CREDIT_WATCHDOG_UNINITIALIZED(unit)) {
            SOC_TMC_ITM_CR_WD_INFO crwd_info;
            SOC_TMC_ITM_CR_WD_INFO_clear(&crwd_info);
            crwd_info.top_queue = crwd_info.bottom_queue = ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP;
            SOCDNX_SAND_IF_ERR_RETURN(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_cr_wd_set, (unit, &crwd_info, &crwd_info)));
        }
        if (!SOC_WARM_BOOT(unit)) {  /* if cold boot, initially turn on credit watchdog */
            BCMDNX_IF_ERR_EXIT(bcm_petra_fabric_control_set(unit, bcmFabricWatchdogQueueEnable, GET_CREDIT_WATCHDOG_MODE(unit)));
        }

    }
#endif
exit:
    SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX(rc);
    if(rc != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_FABRIC,
                  (BSL_META_U(unit,
                              " Failed while executing the macro SOC_DPP_WARMBOOT_RELEASE_HW_MUTEX.\n")));
    }
    BCMDNX_FUNC_RETURN;
}                                        
    
int
bcm_petra_fabric_port_get(int unit,
                          bcm_gport_t child_port,
                          uint32 flags,
                          bcm_gport_t *parent_port)
{
    int modid = 0;
    bcm_port_t port = 0;
    bcm_port_t interface = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(parent_port);

    if (BCM_GPORT_IS_LOCAL(child_port)) {

    port = BCM_GPORT_LOCAL_GET(child_port);
    interface = port + BCM_DPP_PORT_INTERFACE_START;
    BCM_GPORT_LOCAL_SET(*parent_port, interface);

    } else if (BCM_GPORT_IS_MODPORT(child_port)) {

    modid = BCM_GPORT_MODPORT_MODID_GET(child_port);
    port = BCM_GPORT_MODPORT_PORT_GET(child_port);
    interface = port + BCM_DPP_PORT_INTERFACE_START;
    BCM_GPORT_MODPORT_SET(*parent_port, modid, interface);

    } else if ((SOC_PORT_VALID(unit, child_port)) && (IS_PORT(unit, child_port))) {

      interface = port + BCM_DPP_PORT_INTERFACE_START;
      BCM_GPORT_MODPORT_SET(*parent_port, modid, interface);

    } else if (BCM_COSQ_GPORT_IS_E2E_PORT(child_port)) {

    port = BCM_COSQ_GPORT_E2E_PORT_GET(child_port);
    interface = port + BCM_DPP_PORT_INTERFACE_START;
    BCM_COSQ_GPORT_E2E_PORT_SET(*parent_port, interface);

    } else {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Gport type unsupported (0x%08x)"), child_port));
    }

    if (port >= BCM_DPP_PORT_INTERFACE_END) {
        *parent_port = -1;
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Gport interface parent currently unsupported interface_id(0x%08x)"), port));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_control_set(int unit,
                             bcm_fabric_control_t type,
                             int arg)
{
    bcm_error_t rc = BCM_E_NONE;    
    uint32 soc_sand_rv;   
    int minimum_size = 0;
    int maximum_size = 0;
    int queue_id; 

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    switch (type) {
        case bcmFabricQueueMin:
        case bcmFabricQueueMax:
            /* Set queue range for VOQs */
            rc = _bcm_petra_fabric_unicast_queue_range_set(unit, type, arg);
            break;
        case bcmFabricShaperQueueMin:            
        case bcmFabricShaperQueueMax:
            /* Set queue range for Ingress shaping queues */
            rc = _bcm_petra_fabric_ingress_shaper_queue_range_set(unit, type, arg);
            break;
        case bcmFabricMulticastQueueMin:
            /* Set queue range and handle FMQ IDs queues */
            rc = _bcm_petra_fabric_multicast_queue_range_set(unit,bcmFabricMulticastQueueMin,arg);                        
            break;
        case bcmFabricMulticastQueueMax:
            /* Set queue range and handle FMQ IDs queues */
            rc = _bcm_petra_fabric_multicast_queue_range_set(unit,bcmFabricMulticastQueueMax,arg);
            break;

        case bcmFabricTrafficManagementCosMode:
            break;

        case bcmFabricCellSizeFixed:
            /* Cell fixed size */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);
            
            minimum_size = maximum_size = arg;            
            rc = _bcm_petra_fabric_tdm_range_size_set(unit,minimum_size,maximum_size);
            
            break;
        case bcmFabricCellSizeMin:
            /* Cell size Minimum */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);
            
            minimum_size = arg;            
            rc = _bcm_petra_fabric_tdm_range_size_set(unit,minimum_size,maximum_size);
            break;
        case bcmFabricCellSizeMax:
            /* Cell size Maximum */   
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);
            
            maximum_size = arg;            
            rc = _bcm_petra_fabric_tdm_range_size_set(unit,minimum_size,maximum_size);         
            break;

        case bcmFabricCreditSize:
            /* one of the first configurations that should be done. Also set via SOC property */
            /* "credit_size" .                                                                */
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_set,(unit, arg)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            break;
#ifdef BCM_ARAD_SUPPORT
        case bcmFabricCreditSizeRemoteDefault:   /* Default remote credit size */
            if (!SOC_IS_ARADPLUS(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcmFabricCreditSizeRemoteDefault is not supported on this device")));
            } else if (arg < ARAD_CREDIT_SIZE_BYTES_MIN || arg > ARAD_CREDIT_SIZE_BYTES_MAX) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Remote size %d is not between %u..%u"), arg, ARAD_CREDIT_SIZE_BYTES_MIN, ARAD_CREDIT_SIZE_BYTES_MAX));
            } else {
                uint16 credit_worth_local, credit_worth_remote;
                BCM_SAND_IF_ERR_EXIT(arad_plus_mgmt_credit_worth_get_unsafe(unit, &credit_worth_local, &credit_worth_remote));
                if (credit_worth_remote != arg) { /* are we changing the value? */
                    if (SOC_DPP_CONFIG(unit)->arad_plus->nof_remote_faps_with_remote_credit_value) { /* is the current value being used (by remote FAPs)? */
                        if (credit_worth_remote != credit_worth_local) {
                            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The Remote credit value is assigned to remote devices. To change the value you must first assign the local credit value to these devices.")));
                        } else { /* the local and (previous) remote values are equal, so we can just mark all FAPs as using the local value */
                            BCM_SAND_IF_ERR_EXIT(arad_plus_mgmt_change_all_faps_credit_worth_unsafe(unit, ARAD_PLUS_FAP_CREDIT_VALUE_LOCAL));
                        }
                    }
                    credit_worth_remote = arg;
                    BCM_SAND_IF_ERR_EXIT(arad_plus_mgmt_credit_worth_set_unsafe(unit, credit_worth_local, credit_worth_remote));
                }
            }
            break;
#endif
        case bcmFabricVsqCategory:
            rc = bcm_petra_cosq_fmq_vsq_category_mode_set(unit,arg);
            BCMDNX_IF_ERR_EXIT(rc);
            /* Set all VOQs to category 2 when mode is None. */
            if (arg == bcmFabricVsqCatagoryModeNone) {
                queue_id = DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit);
                rc = _bcm_petra_fabric_vsq_category_set(unit, bcmFabricQueueMax, queue_id, &queue_id);
                BCMDNX_IF_ERR_EXIT(rc);
            }                       

#ifdef BCM_WARM_BOOT_SUPPORT
            rc = _bcm_dpp_wb_cosq_update_vsq_data(unit, arg);
            BCMDNX_IF_ERR_EXIT(rc);
#endif /* BCM_WARM_BOOT_SUPPORT */

            break;
        case bcmFabricMulticastSchedulerMode:
            rc = _bcm_petra_fabric_multicast_scheduler_mode_set(unit,arg);
            break;
        case bcmFabricIsolate:
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_all_ctrl_cells_enable_set,(unit, arg ? FALSE : TRUE)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);  
            break;
        case bcmFabricRecycleQueueMin:
        case bcmFabricRecycleQueueMax:
            /* Set queue range for egress queues */
            rc = _bcm_petra_fabric_egress_queue_range_set(unit, type, arg);        
            break;
        case bcmFabricOcbMulticastId1Min:
        case bcmFabricOcbMulticastId1Max:
        case bcmFabricOcbMulticastId2Min:
        case bcmFabricOcbMulticastId2Max:
#ifdef BCM_ARAD_SUPPORT
            if(SOC_IS_ARAD(unit)){
                ARAD_MGMT_OCB_MC_RANGE range;
                uint32 range_ndx;
                if(type == bcmFabricOcbMulticastId1Min || type == bcmFabricOcbMulticastId1Max)
                {
                    range_ndx = 0;
                }
                else
                {
                    range_ndx = 1;
                } 
                soc_sand_rv = arad_mgmt_ocb_mc_range_get(
                                unit,
                                range_ndx,
                                &range
                              );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if(type == bcmFabricOcbMulticastId1Min || type == bcmFabricOcbMulticastId2Min)
                { 
                    range.min = arg;
                }
                else
                {
                    range.max = arg;
                }
                soc_sand_rv = arad_mgmt_ocb_mc_range_set(
                                unit,
                                range_ndx,
                                &range
                              );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                break;
            }
#endif            
            rc = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_FABRIC,
                      (BSL_META_U(unit,
                                  "unit %d, Unsupported Type %d\n"), unit, type));
            break;

        case bcmFabricWatchdogQueueMin:    /* Watchdog QID start index */
        case bcmFabricWatchdogQueueMax:    /* Watchdog QID end index */
        case bcmFabricWatchdogQueueEnable: /* Queue Watchdog Functionality Enable/Disable setting (True/False) */
            /* Set queue range and enable/disable for credit watchdog */
            rc = _bcm_petra_fabric_credit_watchdog_range_set(unit, type, arg, -1);
            break;

       case bcmFabricRCIControlSource:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_rci_enable_set,(unit, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

    case bcmFabricRCIIncrementValue:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_config_set, (unit, SOC_TMC_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricGciLeakyBucketEnable:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_enable_set, (unit, SOC_TMC_FABRIC_GCI_TYPE_LEAKY_BUCKET, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricGciBackoffEnable:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_enable_set, (unit, SOC_TMC_FABRIC_GCI_TYPE_RANDOM_BACKOFF, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricMinimalLinksToDestination:
           if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit))
           {
               soc_sand_rv= MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_minimal_links_to_dest_set, (unit, BCM_MODID_ALL, arg));
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
               
           } else{
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcmFabricMinimalLinksToDestination is not supported on this device")));
              
           }
           break;
        case bcmFabricControlCellsEnable:
              soc_sand_rv= MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_all_ctrl_cells_enable_set, (unit,arg));
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;
        case bcmFabricForceTdmBypassTrafficToFabric:
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_force_tdm_bypass_traffic_to_fabric_set, (unit, arg)));
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"), type));
    }

    BCMDNX_IF_ERR_EXIT(rc);
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}



int
bcm_petra_fabric_control_get(int unit,
                             bcm_fabric_control_t type,
                             int *arg)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv = SOC_SAND_OK;
    uint8 ret_bool = FALSE;
    int minimum_size = 0;
    int maximum_size = 0; 

    BCMDNX_INIT_FUNC_DEFS;
    switch (type) {
        case bcmFabricQueueMin:                                    
        case bcmFabricQueueMax:
            rc = _bcm_petra_fabric_unicast_queue_range_get(unit,type,arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricShaperQueueMin:                        
        case bcmFabricShaperQueueMax:
            rc = _bcm_petra_fabric_ingress_shaper_queue_range_get(unit,type,arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricMulticastQueueMin:
        case bcmFabricMulticastQueueMax:
            rc = _bcm_petra_fabric_multicast_queue_range_get(unit,type,arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricRecycleQueueMin:
        case bcmFabricRecycleQueueMax:
            /* Set queue range for egress queues */
            rc = _bcm_petra_fabric_egress_queue_range_get(unit, type, arg);        
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricTrafficManagementCosMode:
            break;
        case bcmFabricCellSizeFixed:
            /* Cell fixed size */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);

            if (minimum_size != maximum_size) {
                LOG_ERROR(BSL_LS_BCM_FABRIC,
                          (BSL_META_U(unit,
                                      "unit %d, Asking for fixed size, while size is a range, %d - %d\n"), unit, minimum_size,maximum_size));
            }

            *arg = minimum_size;
            break;
        case bcmFabricCellSizeMin:
            /* Minimum cell size */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);

            *arg = minimum_size;
            break;
        case bcmFabricCellSizeMax:
            /* Maximum cell size */
            rc = _bcm_petra_fabric_tdm_range_size_get(unit,&minimum_size,&maximum_size);
            BCMDNX_IF_ERR_EXIT(rc);
            *arg = maximum_size;
            break;

        case bcmFabricCreditSize:
            rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_credit_worth_get,(unit, (uint32 *)arg)));
            BCM_SAND_IF_ERR_EXIT(rc);
            break;
        case bcmFabricCreditSizeRemoteDefault:   /* Default remote credit size */
            if (SOC_IS_ARADPLUS(unit)) {
                uint16 credit_size_local, credit_size_remote;
                BCM_SAND_IF_ERR_EXIT(arad_plus_mgmt_credit_worth_get_unsafe(unit, &credit_size_local, &credit_size_remote));
                *arg = credit_size_remote;
            } else {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bcmFabricCreditSizeRemoteDefault is not supported on this device")));
            }
            break;
        case bcmFabricVsqCategory:
            {
                bcm_fabric_vsq_category_mode_t vsq_category_mode;
                rc = bcm_petra_cosq_fmq_vsq_category_mode_get(unit,&vsq_category_mode);
                BCMDNX_IF_ERR_EXIT(rc);
                *arg = vsq_category_mode;
            }
            break;
        case bcmFabricMulticastSchedulerMode:
            rc = _bcm_petra_fabric_multicast_scheduler_mode_get(unit,arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricIsolate:
            rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_all_ctrl_cells_enable_get,(unit, &ret_bool)));
            BCM_SAND_IF_ERR_EXIT(rc);
            (*arg) = (ret_bool == FALSE ? 1 : 0);
             break;
        case bcmFabricOcbMulticastId1Min:
        case bcmFabricOcbMulticastId1Max:
        case bcmFabricOcbMulticastId2Min:
        case bcmFabricOcbMulticastId2Max:
#ifdef BCM_ARAD_SUPPORT
            if(SOC_IS_ARAD(unit)){
                ARAD_MGMT_OCB_MC_RANGE range;
                uint32 range_ndx;
                if(type == bcmFabricOcbMulticastId1Min || type == bcmFabricOcbMulticastId1Max)
                {
                    range_ndx = 0;
                }
                else
                {
                    range_ndx = 1;
                } 
                soc_sand_rv = arad_mgmt_ocb_mc_range_get(
                            unit,
                            range_ndx,
                            &range
                          );
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                if(type == bcmFabricOcbMulticastId1Min || type == bcmFabricOcbMulticastId2Min)
                { 
                    *arg = range.min;
                }
                else
                {
                    *arg = range.max;
                }
                break;
            }
#endif            
            rc = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_FABRIC,
                      (BSL_META_U(unit,
                                  "unit %d, Unsupported Type %d\n"), unit, type));
            break;

        case bcmFabricWatchdogQueueMin:    /* Watchdog QID start index */
        case bcmFabricWatchdogQueueMax:    /* Watchdog QID end index */
        case bcmFabricWatchdogQueueEnable: /* Queue Watchdog Functionality Enable/Disable setting (True/False) */
            /* Set queue range and enable/disable for credit watchdog */
            rc = _bcm_petra_fabric_credit_watchdog_range_get(unit, type, arg);
            break;

       case bcmFabricRCIControlSource:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_rci_enable_get,(unit, (soc_dcmn_fabric_control_source_t*) arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricRCIIncrementValue:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_config_get, (unit, SOC_TMC_FABRIC_RCI_CONFIG_TYPE_INCREMENT_VALUE, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricGciLeakyBucketEnable:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_enable_get, (unit, SOC_TMC_FABRIC_GCI_TYPE_LEAKY_BUCKET, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricGciBackoffEnable:
           soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_enable_get, (unit, SOC_TMC_FABRIC_GCI_TYPE_RANDOM_BACKOFF, arg));
           BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
           break;

       case bcmFabricMinimalLinksToDestination:
          if (SOC_IS_ARADPLUS(unit) || SOC_IS_JERICHO(unit))
           {
               soc_sand_rv= MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_minimal_links_to_dest_get, (unit, SOC_MODID_ALL, arg));
               BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
               
           } else{
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcmFabricMinimalLinksToDestination is not supported on this device")));
              
           }
           break;

        case bcmFabricForceTdmBypassTrafficToFabric:
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_force_tdm_bypass_traffic_to_fabric_get, (unit, arg)));
            break;

        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"), type));
    }


exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_bandwidth_profile_set(int unit,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{
    uint32 soc_sand_rv;
    uint32 exact_rate;
    int i;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    BCMDNX_NULL_CHECK(profile_array);       

    if (profile_count > SOC_TMC_SCH_NOF_LINKS * SOC_TMC_SCH_NOF_RCI_LEVELS || profile_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("profile_count invalid")));
    }
    
    for(i=0 ; i<profile_count ; i++)
    {
      soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_device_rate_entry_set,(unit, profile_array[i].rci, profile_array[i].num_links, profile_array[i].max_kbps, &exact_rate)));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }
     
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_bandwidth_profile_get(int unit,
                                       int profile_count,
                                       bcm_fabric_bandwidth_profile_t *profile_array)
{
    uint32 soc_sand_rv;
    int i;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    
    BCMDNX_NULL_CHECK(profile_array);    
    
    if (profile_count > SOC_TMC_SCH_NOF_LINKS * SOC_TMC_SCH_NOF_RCI_LEVELS || profile_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("profile_count invalid")));
    }
    
    for(i=0 ; i<profile_count ; i++)
    {  
      soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sch_device_rate_entry_get,(unit, profile_array[i].rci, profile_array[i].num_links, (uint32*)&(profile_array[i].max_kbps))));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * external functions
 */
int
bcm_petra_fabric_enhance_ports_get(int unit, int *nbr_ports, int *ports)
{
    bcm_error_t           rc = BCM_E_NONE;
    uint32                soc_sand_rv;
    SOC_SAND_U32_RANGE    queue_range;
    SOC_TMC_MULT_FABRIC_INFO  fabric_info;
    int                   be_class;


    BCMDNX_INIT_FUNC_DEFS;

    (*nbr_ports) = 0;

    /* check if in enhanced mode */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_enhanced_get,(unit, &queue_range)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
    /* coverity[uninit_use:FALSE] */
    if ( (queue_range.start == DPP_DEVICE_COSQ_FMQ_NON_ENHANCED_QID_MIN) && (queue_range.end == DPP_DEVICE_COSQ_FMQ_NON_ENHANCED_QID_MAX) ) {
        BCM_EXIT;
    }

    /* determine associated ports */
    SOC_TMC_MULT_FABRIC_INFO_clear(&fabric_info);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_credit_source_get,(unit, &fabric_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    /* consistency checks */
    if (fabric_info.credits_via_sch != SOC_SAND_TRUE) {
        BCM_EXIT;
    }

    if (fabric_info.guaranteed.gr_sch_port.multicast_class_valid) {
        ports[(*nbr_ports)++] = fabric_info.guaranteed.gr_sch_port.mcast_class_port_id;
    }
    for (be_class = 0; be_class < SOC_TMC_MULT_FABRIC_NOF_BE_CLASSES; be_class++) {
        if (fabric_info.best_effort.be_sch_port[be_class].be_sch_port.multicast_class_valid) {
            ports[(*nbr_ports)++] = fabric_info.best_effort.be_sch_port[be_class].be_sch_port.mcast_class_port_id;
        }
    }

    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Set VOQ range
 */
STATIC int
_bcm_petra_fabric_unicast_queue_range_set(int unit,
                      bcm_fabric_control_t control_type,
                      int queue_id)
{
    bcm_error_t rc = BCM_E_NONE;
    int *queue_region_config = NULL;
    int voq_1k_start, voq_1k_end;
    int have_first = FALSE;
    int have_last = FALSE;
    int region;
    int current_voq_1k_start = -1, current_voq_1k_end = -1;
    int qid_start = -1, qid_end = -1;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* Validate queue id */
    if (queue_id > DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit) || queue_id < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id %d invalid"),queue_id));
    }
    if ((queue_id % 1024) && (queue_id != 4) && (control_type == bcmFabricQueueMin)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id min %d must be a multiple of 1024"),queue_id));
    }
    if (((queue_id+1) % 1024) && (control_type == bcmFabricQueueMax)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id max %d must be a multiple of 1024 then -1"),queue_id));
    }

    /* Special case: queue_id is 4, in that case we treat it as queue_id 0. */
    if (queue_id == 4 && (control_type == bcmFabricQueueMin)) {
        queue_id = 0;
    }

    /* set VSQ categories - also updates queue_id according to the other queue ranges settings (multicast etc.)*/
    rc = _bcm_petra_fabric_vsq_category_set(unit,control_type, queue_id, &queue_id);
    BCMDNX_IF_ERR_EXIT(rc);

    /* Allocation manager configuration */
    BCMDNX_ALLOC(queue_region_config, sizeof(int) * DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit), "queue_region");

    if (queue_region_config == NULL) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("esource memory allocation failure")));
    }

    rc = bcm_dpp_am_cosq_queue_region_config_get(unit, queue_region_config, DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST);
    BCMDNX_IF_ERR_EXIT(rc);

    for (region = 0; region < (DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)); region++) {
        if ((queue_region_config[region] == DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST) && (have_first == FALSE)) {

            have_first = TRUE;
            current_voq_1k_start = region;

        } else if ((queue_region_config[region] != DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST) 
               && (have_first == TRUE) 
               && (have_last == FALSE)){
             current_voq_1k_end = region;
        }
    }
 
    if (control_type == bcmFabricQueueMin) {
        qid_start = queue_id;
        voq_1k_start = queue_id/1024;
        if (current_voq_1k_end == -1) {
            current_voq_1k_end = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
        }
        voq_1k_end = current_voq_1k_end;
    }
    else {
        qid_end = queue_id;
        if (current_voq_1k_end == -1) {
            current_voq_1k_end = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
        }
        voq_1k_start = current_voq_1k_start;
        voq_1k_end = (queue_id + 1)/1024;
    }

    for (region = voq_1k_start; region < voq_1k_end; region++) {
        queue_region_config[region] = DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST;
    }
    
    rc = _bcm_dpp_am_cosq_process_queue_region(unit, queue_region_config);
    BCMDNX_IF_ERR_EXIT(rc);

    /* update state */
    rc = _bcm_petra_cosq_ucast_qid_range_set(unit, qid_start, qid_end);
    BCMDNX_IF_ERR_EXIT(rc);

#ifdef BCM_WARM_BOOT_SUPPORT
    rc = _bcm_dpp_wb_cosq_update_ucast_data(unit, qid_start, qid_end);
    BCMDNX_IF_ERR_EXIT(rc);
#endif /* BCM_WARM_BOOT_SUPPORT */

exit:
    if (queue_region_config != NULL) {
       BCM_FREE(queue_region_config);
    }
    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: Set FMQ range
 */
STATIC int
_bcm_petra_fabric_multicast_queue_range_set(int unit,
                                            bcm_fabric_control_t control_type,
                                            int queue_id)
{
    bcm_error_t rc = BCM_E_NONE;    
    uint32 soc_sand_rv;
    SOC_SAND_U32_RANGE queue_range,queue_range_new;    
    int schedule_mode;
    int *queue_region_config = NULL;
    int voq_1k_start, voq_1k_end;
    int have_first = FALSE;
    int have_last = FALSE;
    int region;
    int current_voq_1k_start = -1, current_voq_1k_end = -1;
    int is_run_allocation;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    /* Allocation manager configuration */
    /* Validate queue id */
    if (queue_id > DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit) || queue_id < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id %d invalid"),queue_id));
    }    
    
    /* Get queue id range */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_enhanced_get,(unit,&queue_range)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    
    /* Allocation succeeded, Set enhance range queues */
    /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
    /* coverity[uninit_use:FALSE] */
    queue_range_new = queue_range;

    if (control_type == bcmFabricMulticastQueueMin) {
        queue_range_new.start = queue_id;
    } else {
        queue_range_new.end = queue_id;
    }
    
    rc = _bcm_petra_fabric_multicast_scheduler_mode_get(unit,&schedule_mode);
    BCMDNX_IF_ERR_EXIT(rc);

    /*  Check if enhance is disable, if it is and queue range defined is the
     *  same as before, Skip allocation management. Since this is the default
     *  range of multicast fabric which assumed to be already allocated 0-3 or
     *  new range that the allocation management already allocated.
     */
    if (schedule_mode == 0) {
        is_run_allocation = 0;
    } else {
        is_run_allocation = 1;
    }

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_enhanced_set,(unit,&queue_range_new)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    /* Set new VSQ category 0 range - Same as FMQ range */
    /* This applies only if vsq categories is bcmFabricVsqCatagoryMode1 */
    rc = _bcm_petra_fabric_vsq_category_set(unit,control_type, queue_id, &queue_id);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_run_allocation) {
        
        if ((queue_id % 1024) && (control_type == bcmFabricMulticastQueueMin)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id min %d must be a multiple of 1024"),queue_id));
        }
        if (((queue_id+1) % 1024) && (control_type == bcmFabricMulticastQueueMax)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id min %d must be a multiple of 1024 then -1"),queue_id));
        }        
    
        BCMDNX_ALLOC(queue_region_config, sizeof(int) * DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit), "queue_region");
    
        if (queue_region_config == NULL) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("resource memory allocation failur")));
        }
    
        rc = bcm_dpp_am_cosq_queue_region_config_get(unit, queue_region_config, DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST);
        BCMDNX_IF_ERR_EXIT(rc);
    
        for (region = 0;
         region < (DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit));
         region++) {
        if ((queue_region_config[region] == DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST) 
            && (have_first == FALSE)) {
    
            have_first = TRUE;
            current_voq_1k_start = region;
    
        } else if ((queue_region_config[region] != DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST) 
               && (have_first == TRUE) 
               && (have_last == FALSE)){
             current_voq_1k_end = region;
        }
        }
    
    
        if (control_type == bcmFabricMulticastQueueMin) {
        voq_1k_start = queue_id/1024;
        if (current_voq_1k_end == -1) {
          current_voq_1k_end = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
        }
        voq_1k_end = current_voq_1k_end;
    
        } else {
        if (current_voq_1k_start == -1) {
          current_voq_1k_start = 0;
        }
        voq_1k_start = current_voq_1k_start;
        voq_1k_end = (queue_id + 1)/1024;
        }
    
        for (region = voq_1k_start;
         region < voq_1k_end;
         region++) {
        queue_region_config[region] = DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST;
        }
        
        rc = _bcm_dpp_am_cosq_process_queue_region(unit, queue_region_config);
        BCMDNX_IF_ERR_EXIT(rc);
    
    }
    
exit:
    if (queue_region_config != NULL) {
       BCM_FREE(queue_region_config);
    }

    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: Get FMQ range
 */
STATIC int
_bcm_petra_fabric_multicast_queue_range_get(int unit,
                                            bcm_fabric_control_t control_type,
                                            int* queue_id)
{
    uint32 soc_sand_rv;
    SOC_SAND_U32_RANGE queue_range;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(queue_id);    

    /* Get queue id range */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_enhanced_get,(unit,&queue_range)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (control_type == bcmFabricMulticastQueueMin) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *queue_id = queue_range.start;
    }

    if (control_type == bcmFabricMulticastQueueMax) {
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *queue_id = queue_range.end;
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Purpose: Set Enhance mode
 */
STATIC int
_bcm_petra_fabric_multicast_scheduler_mode_set(int unit,
                                               int mode)
{
    bcm_error_t rc = BCM_E_NONE;
    int ports[DPP_FABRIC_ENHANCE_NOF_PORTS];
    int index;
    uint32 soc_sand_rv;
    SOC_TMC_MULT_FABRIC_INFO fabric_info, exact_fabric_info;    

    BCMDNX_INIT_FUNC_DEFS;
    if (mode < 0 || mode > 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("mode %d invalid"),mode));
    }

    SOC_TMC_MULT_FABRIC_INFO_clear(&fabric_info);
    SOC_TMC_MULT_FABRIC_INFO_clear(&exact_fabric_info);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_credit_source_get,(unit,&fabric_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (mode == 0) {
        if (fabric_info.credits_via_sch == SOC_SAND_TRUE) {
            /* Apply changes, enhance was enabled */
            /* 1. Disable enhance */
            /* 2. Disalloc HRs */
            /* 3. Set ports configuration to be invalid */

            /* Disable enhance */
            fabric_info.credits_via_sch = SOC_SAND_FALSE;  
            
            ports[0] = fabric_info.guaranteed.gr_sch_port.mcast_class_port_id;
            for (index = 0; index < DPP_FABRIC_ENHANCE_NOF_BE_PORTS; index++) {
                ports[index+1] = fabric_info.best_effort.be_sch_port[index].be_sch_port.mcast_class_port_id;            
            }
    
            /* Disalloc dummy HR "hard wire" ports */
            rc = _bcm_petra_fabric_enhance_ports_disalloc(unit,ports,DPP_FABRIC_ENHANCE_NOF_PORTS);
            BCMDNX_IF_ERR_EXIT(rc);
    
            /* Disable ports configuration */
            fabric_info.guaranteed.gr_sch_port.mcast_class_port_id = 0;
            fabric_info.guaranteed.gr_sch_port.multicast_class_valid = SOC_SAND_FALSE;
            for (index = 0; index < DPP_FABRIC_ENHANCE_NOF_PORTS-1; index++) {
                fabric_info.best_effort.be_sch_port[index].be_sch_port.mcast_class_port_id = 0;
                fabric_info.best_effort.be_sch_port[index].be_sch_port.multicast_class_valid = SOC_SAND_FALSE;
            }        
        }
                      
    } else {
        if (fabric_info.credits_via_sch == SOC_SAND_FALSE) {
            /* Apply changes , enhance was disabled */
            /* 1. Enable enhance */
            /* 2. Alloc HRs */
            /* 3. Set ports configuration */

            /* Enable enhance */
            fabric_info.credits_via_sch = SOC_SAND_TRUE;
    
            /* Obtain Dummy HR "hard wire" Port for scheduelr properties */
            rc = _bcm_petra_fabric_enhance_ports_alloc(unit,ports,DPP_FABRIC_ENHANCE_NOF_PORTS);
            BCMDNX_IF_ERR_EXIT(rc);
    
            /* Set ports configruation */
            fabric_info.credits_via_sch = 1;
            fabric_info.guaranteed.gr_sch_port.mcast_class_port_id = ports[0];
            fabric_info.guaranteed.gr_sch_port.multicast_class_valid = SOC_SAND_TRUE;
            for (index = 0; index < DPP_FABRIC_ENHANCE_NOF_PORTS-1; index++) {
                fabric_info.best_effort.be_sch_port[index].be_sch_port.mcast_class_port_id = ports[index+1];
                fabric_info.best_effort.be_sch_port[index].be_sch_port.multicast_class_valid = SOC_SAND_TRUE;
            }
        }
    }
      
    /* Commit configuration */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_credit_source_set,(unit,&fabric_info,&exact_fabric_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);    

exit:
    BCMDNX_FUNC_RETURN;
}
/* wrapper function for external call */
int
bcm_petra_fabric_multicast_scheduler_mode_get(int unit, int *mode)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_petra_fabric_multicast_scheduler_mode_get(unit, mode);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Purpose: Get Enhance mode
 */
STATIC int
_bcm_petra_fabric_multicast_scheduler_mode_get(int unit,
                                               int* mode)
{
    uint32 soc_sand_rv;
    SOC_TMC_MULT_FABRIC_INFO fabric_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(mode);    

    SOC_TMC_MULT_FABRIC_INFO_clear(&fabric_info);
    
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_fabric_credit_source_get,(unit,&fabric_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *mode = (fabric_info.credits_via_sch == SOC_SAND_TRUE) ? 1:0;

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: Alloc ports-HR for enhance scheduler
 */
STATIC int
_bcm_petra_fabric_enhance_ports_alloc(int unit,
                                    int *ports,
                                    int nof_ports)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(ports);

    rc = bcm_petra_cosq_fmq_hr_allocate(unit, nof_ports, ports);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Purpose: Disalloc ports-HR for enhance scheduler
 */
STATIC int
_bcm_petra_fabric_enhance_ports_disalloc(int unit,
                                         int *ports,
                                         int nof_ports)
{
    bcm_error_t rc = BCM_E_NONE;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(ports);

    rc = bcm_petra_cosq_fmq_hr_deallocate(unit, nof_ports, ports);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_connectivity_status_get(
    int unit, 
    int link_partner_max, 
    bcm_fabric_link_connectivity_t *link_partner_array, 
    int *link_partner_count)
{
    int i, port_i;
    uint32 soc_sand_rv;
    int nof_links = 0;
    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR connectivity_map;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(link_partner_array);
    BCMDNX_NULL_CHECK(link_partner_count);
    
    BCM_DPP_UNIT_CHECK(unit);       
    
    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(&connectivity_map);
    
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_nof_links_get,(unit, &nof_links)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_topology_status_connectivity_get,(unit, 0, nof_links - 1, &connectivity_map)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    *link_partner_count = 0;
    

    PBMP_SFI_ITER(unit, port_i)
    {
        i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port_i);
        if(*link_partner_count >= link_partner_max)
        {
            *link_partner_count = 0;
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("link_partner_max %d is too small"),link_partner_max));
        }

        if(connectivity_map.link_info[i].far_unit != SOC_TMC_DEVICE_ID_IRRELEVANT
           && connectivity_map.link_info[i].is_logically_connected)
        {                      
            link_partner_array[*link_partner_count].module_id   = connectivity_map.link_info[i].far_unit;
            link_partner_array[*link_partner_count].link_id     = connectivity_map.link_info[i].far_link_id;
            switch(connectivity_map.link_info[i].far_dev_type)
            {
                  case SOC_TMC_FAR_DEVICE_TYPE_FE1:
                  case SOC_TMC_FAR_DEVICE_TYPE_FE3:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeFE13;
                  break;
                
                  case SOC_TMC_FAR_DEVICE_TYPE_FE2:
                    link_partner_array[*link_partner_count].device_type = bcmFabricDeviceTypeFE2;
                  break;
                
                  case SOC_TMC_FAR_DEVICE_TYPE_FAP:
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
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_connectivity_status_single_get(
    int unit, 
    bcm_port_t link_id, 
    bcm_fabric_link_connectivity_t *link_partner_info)
{
    uint32 soc_sand_rv;
    int link_index;
    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR connectivity_map;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(link_partner_info);
    BCM_DPP_UNIT_CHECK(unit);
    if(!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), link_id)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("unit %d, Port %d is out-of-range"), unit, link_id));
    }       
    
    SOC_TMC_FABRIC_LINKS_CON_STAT_INFO_ARR_clear(&connectivity_map);

    link_index = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link_id);
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_topology_status_connectivity_get,(unit, link_index, link_index, &connectivity_map)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    if(connectivity_map.link_info[link_index].far_unit != SOC_TMC_DEVICE_ID_IRRELEVANT
       && connectivity_map.link_info[link_index].is_logically_connected)
    {                      
    link_partner_info->module_id   = connectivity_map.link_info[link_index].far_unit;
    link_partner_info->link_id     = connectivity_map.link_info[link_index].far_link_id;
    switch(connectivity_map.link_info[link_index].far_dev_type)
    {
        case SOC_TMC_FAR_DEVICE_TYPE_FE1:
        case SOC_TMC_FAR_DEVICE_TYPE_FE3:
            link_partner_info->device_type = bcmFabricDeviceTypeFE13;
            break;
        
        case SOC_TMC_FAR_DEVICE_TYPE_FE2:
            link_partner_info->device_type = bcmFabricDeviceTypeFE2;
            break;
        
        case SOC_TMC_FAR_DEVICE_TYPE_FAP:
            link_partner_info->device_type = bcmFabricDeviceTypeFAP;
            break;
        
        default:
            link_partner_info->device_type = bcmFabricDeviceTypeUnknown;
            break;
    }                        
    }
    else
    {
        link_partner_info->link_id = BCM_FABRIC_LINK_NO_CONNECTIVITY;
    }

    
exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_ARAD_SUPPORT
int 
bcm_petra_arad_fabric_link_status_get(
    int unit, 
    bcm_port_t link_id, 
    uint32 *link_status, 
    uint32 *errored_token_count)
{
    uint32 soc_rv = 0;
    uint32 link_status_tmp;
    bcm_port_t link_i;
    BCMDNX_INIT_FUNC_DEFS;

    if(!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), link_id)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("unit %d, Port %d is out-of-range"), unit, link_id));
    }

    link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link_id);
    
    BCMDNX_NULL_CHECK(link_status);
    BCMDNX_NULL_CHECK(errored_token_count);

    soc_rv = soc_dpp_fabric_link_status_get(unit, link_i, &link_status_tmp, errored_token_count);
    BCMDNX_IF_ERR_EXIT(soc_rv);

    
    *link_status = 0;
    if(link_status_tmp & DCMN_FABRIC_LINK_STATUS_CRC_ERROR) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_CRC_ERROR;
    }
    if(link_status_tmp & DCMN_FABRIC_LINK_STATUS_SIZE_ERROR) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_SIZE_ERROR;
    }
    if(link_status_tmp & DCMN_FABRIC_LINK_STATUS_MISALIGN) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_MISALIGN;
    }
    if(link_status_tmp & DCMN_FABRIC_LINK_STATUS_CODE_GROUP_ERROR) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR;
    }
    if(*errored_token_count < 63) {
      (*link_status) |= BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS; 
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}
#endif

#ifdef BCM_PETRAB_SUPPORT
int 
bcm_petra_pb_fabric_link_status_get(
    int unit, 
    bcm_port_t link_id, 
    uint32 *link_status, 
    uint32 *errored_token_count)
{
    uint32 soc_rv = 0;
    SOC_TMC_FABRIC_LINKS_STATUS_SINGLE    link_status_single;

    bcm_port_t link_i;
    
    BCMDNX_INIT_FUNC_DEFS;
    link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link_id);
    
    BCM_DPP_UNIT_CHECK(unit);
    if(!SOC_PORT_VALID_RANGE(unit, link_i))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("unit %d, Link %d is out-of-range"), unit, link_i));
    }
    BCMDNX_NULL_CHECK(link_status);
    BCMDNX_NULL_CHECK(errored_token_count);
    
 

    soc_rv = soc_fabric_link_status_get(unit, link_i, &link_status_single);
    BCMDNX_IF_ERR_EXIT(soc_rv);
 
		
    if(!link_status_single.valid)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Invalid link"), unit));
    }
		
    *link_status = 0;
		 
    if(link_status_single.errors_bm & SOC_TMC_LINK_ERR_TYPE_CRC) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_CRC_ERROR;
    }
		
    if(link_status_single.errors_bm & SOC_TMC_LINK_ERR_TYPE_SIZE) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_SIZE_ERROR;    
    }

    if(link_status_single.errors_bm & SOC_TMC_LINK_ERR_TYPE_CODE_GROUP) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR; 
    }

    if(link_status_single.errors_bm & SOC_TMC_LINK_ERR_TYPE_MISALIGN) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_MISALIGN; 
    }

    if(!(link_status_single.indications_bm & SOC_TMC_LINK_INDICATE_TYPE_SIG_LOCK)) {
         (*link_status) |= BCM_FABRIC_LINK_STATUS_NO_SIG_LOCK; 
    }

    if(!(link_status_single.indications_bm & SOC_TMC_LINK_INDICATE_TYPE_ACCEPT_CELL)) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP; 
    }
										
    (*errored_token_count) = link_status_single.leaky_bucket_counter;
		
    if(*errored_token_count < 63) {
        (*link_status) |= BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS; 
    }
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BCM_PETRAB_SUPPORT */

int 
bcm_petra_fabric_link_status_get(
    int unit, 
    bcm_port_t link_id, 
    uint32 *link_status, 
    uint32 *errored_token_count)
{
    bcm_error_t rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        rc = bcm_petra_arad_fabric_link_status_get(unit, link_id, link_status, errored_token_count);
    }
#endif /* BCM_ARAD_SUPPORT */

#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        rc = bcm_petra_pb_fabric_link_status_get(unit, link_id, link_status, errored_token_count);
    }
#endif /* BCM_PETRAB_SUPPORT*/
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_tdm_editing_set(int unit,
                                 bcm_gport_t gport,
                                 bcm_fabric_tdm_editing_t *editing)
{
    bcm_error_t rc = BCM_E_NONE;
    bcm_port_t local_port = 0;
    bcm_module_t modid = 0, my_mod;
    SOC_TMC_TDM_FTMH_INFO ftmh_info;
    uint32 soc_sand_rv;
    int is_ingress;
    int core;
    int index,nof_bytes;
    uint32 tmp, p_fap_port;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);
    BCMDNX_NULL_CHECK(editing);

    SOC_TMC_TDM_FTMH_INFO_clear(&ftmh_info);

    /* Check if TDM is supported */
    if (!SOC_IS_DPP_TDM_OPTIMIZE(unit) && !SOC_IS_DPP_TDM_STANDARD(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("TDM is not configured")));
    }
    
    /* Validate Gport, retreive port id */
    rc = _bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rc);    

    /* Validate port is TDM port */
    if(!IS_TDM_PORT(unit, gport_info.local_port)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT,(_BSL_BCM_MSG("given gport: 0x%x is not a tdm port"),gport));
    }
    /* Check CRC is added*/
    if(!editing->add_packet_crc && !SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("crc must be added to a system with no petra")));
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, gport_info.local_port, &p_fap_port, &core)));

    /* Retrieve port TDM configuration */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_ftmh_get,(unit, p_fap_port,&ftmh_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (ftmh_info.action_ing == SOC_TMC_TDM_NOF_ING_ACTIONS) {
        ftmh_info.action_ing = SOC_TMC_TDM_ING_ACTION_NO_CHANGE;
    }

    if (ftmh_info.action_eg == SOC_TMC_TDM_NOF_EG_ACTIONS) {
        ftmh_info.action_eg = SOC_TMC_TDM_EG_ACTION_NO_CHANGE;
    }

    /* Set packet crc. (only if feature avaialable) */
    if (SOC_DPP_CONFIG(unit)->tdm.is_bypass) { 
        uint8 add_crc = editing->add_packet_crc, configure_ingress = TRUE, configure_egress = TRUE;
        /* validate parameter */
        if (editing->add_packet_crc < 0 || editing->add_packet_crc > 1) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid add_packet_crc parameter: %d"),editing->add_packet_crc));
        }
        if (editing->flags & BCM_FABRIC_TDM_EDITING_NON_SYMMETRIC_CRC) {
            configure_ingress = (editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS) ? TRUE : FALSE;
            configure_egress  = (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS)  ? TRUE : FALSE;
        }

        /* Add packet CRC should be done when sending to ARAD or Soc_petra-B packet mode */
        /* Soc_petra-B ingress can not add CRC in case of Bypass mode */
        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_port_packet_crc_set,(unit, p_fap_port, add_crc, configure_ingress, configure_egress)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        
    }
    
    /* Set Ingress configuration */
    if (editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS) {
        
        /* 
         * Ingress configuration includes: 
         * 1. Set Action ingress 
         * 2. Set destinatino if action ingress is append or custom external
         * 3. Set user defined if action ingress is append
         */

        /* 1. Set action ingress */
        is_ingress = 1;
        rc = _bcm_petra_fabric_tdm_to_action_from_editing_type(unit,is_ingress,&ftmh_info,&(editing->type));
        BCMDNX_IF_ERR_EXIT(rc);

        /* 2. Set destination only if tdm type is append or custom external */
        rc = bcm_petra_stk_my_modid_get(unit, &my_mod);
        BCMDNX_IF_ERR_EXIT(rc);

        if (editing->type == bcmFabricTdmEditingAppend || editing->type == bcmFabricTdmEditingCustomExternal) {
        
            if (editing->flags & BCM_FABRIC_TDM_EDITING_MULTICAST && editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("given invalid falgs: 0x%x unicast and multicast flags cant be toghther"),editing->flags));
            }
            
            if (editing->flags & BCM_FABRIC_TDM_EDITING_MULTICAST) {
                ftmh_info.is_mc = SOC_SAND_TRUE;
                /* Multicast destination */
                if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {
                    ftmh_info.ftmh.opt_mc.mc_id = _BCM_MULTICAST_ID_GET(editing->multicast_id);
                } else {
                    ftmh_info.ftmh.standard_mc.mc_id = _BCM_MULTICAST_ID_GET(editing->multicast_id);
                }            
            } else if (editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {
                /* Unicast destination */                            
                uint32  sys_port = 0;
                bcm_error_t rv;
                int core;
                uint32 tm_port = 0;
    
                ftmh_info.is_mc = SOC_SAND_FALSE;

                if (SOC_IS_PETRAB(unit) || BCM_GPORT_IS_SYSTEM_PORT(editing->destination_port)) {
                    rc = _bcm_dpp_gport_to_phy_port(unit, editing->destination_port, _BCM_DPP_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info);
                    BCMDNX_IF_ERR_EXIT(rc);              
                    
                    sys_port = gport_info.sys_port;  
                } else {                    
                    /* Retreive directly from MODPORT or LOCAL */
                    if (BCM_GPORT_IS_MODPORT(editing->destination_port)) {
                        modid = BCM_GPORT_MODPORT_MODID_GET(editing->destination_port);
                        local_port = BCM_GPORT_MODPORT_PORT_GET(editing->destination_port);
                    } else if (BCM_GPORT_IS_LOCAL(editing->destination_port)){
                        modid = my_mod;
                        local_port = BCM_GPORT_LOCAL_GET(editing->destination_port);                          
                    } else {
                        modid = my_mod;
                    }

                    
                    rv = soc_port_sw_db_local_to_tm_port_get(unit, local_port, &tm_port, &core);
                    if(BCM_FAILURE(rv)) {
                      /* In case TM port is not configured assume 1:1 mapping */
                      tm_port = local_port;
                    }
                }

                if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {                                  
                    ftmh_info.ftmh.opt_uc.dest_fap_id = modid;
                    ftmh_info.ftmh.opt_uc.dest_if = tm_port;                
                } else {                
                    ftmh_info.ftmh.standard_uc.sys_phy_port = sys_port;

                    if (SOC_IS_ARAD(unit)) {                                                
                        ftmh_info.ftmh.standard_uc.dest_fap_port = tm_port;
                        ftmh_info.ftmh.standard_uc.dest_fap_id = modid;
                    }
                }
            }                            
        }
        
        /* 3. User define settings */
        /* Adding user define only if editing type is add */
        if (editing->type == bcmFabricTdmEditingAppend && SOC_IS_DPP_TDM_STANDARD(unit)) {
            if (editing->user_field_count > SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid max user define bits %d. max allowed %d"),
                                               editing->user_field_count,SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits));
            }

            if (editing->user_field_count % 8 != 0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid user define bits %d. must be in a multipies of 8"), editing->user_field_count));
            }

            if (editing->user_field_count != 0) {
                if (editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {  
                    nof_bytes = editing->user_field_count >> 3;
                    
                    for (index = 0; index < nof_bytes; index++) {
                        tmp = 0;
                        tmp = editing->user_field[index];
                        if (index < 4) {
                          soc_sand_rv = soc_sand_bitstream_set_any_field(
                            &tmp,                
                            index*8,
                            (index+1)*8-1,
                            &(ftmh_info.ftmh.standard_uc.user_def)
                            );            
                          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }                        
                        else {
                          /* User define for more than 32bits. */
                          soc_sand_rv = soc_sand_bitstream_set_any_field(
                            &tmp,                
                            index*8,
                            (index+1)*8-1,
                            &(ftmh_info.ftmh.standard_uc.user_def_2)
                            );            
                          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                        }
                    }
                } else {
                    nof_bytes = editing->user_field_count >> 3;
                    for (index = 0; index <  nof_bytes; index++) {
                        tmp = 0;
                        tmp = editing->user_field[index];
                        
                        soc_sand_rv = soc_sand_bitstream_set_any_field(
                            &tmp,                
                            index*8,
                            (index+1)*8-1,
                            &(ftmh_info.ftmh.standard_mc.user_def)
                            );            
                        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    }
                }
            }
        }                      
    } else if (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS) {
        /* 
         * Egress configuration includes: 
         * 1. Set Action Egress         
         */
        is_ingress = 0;
        rc = _bcm_petra_fabric_tdm_to_action_from_editing_type(unit,is_ingress,&ftmh_info,&(editing->type));
        BCMDNX_IF_ERR_EXIT(rc);         


    } else {
        /* No ask for either egress or ingress configuration */
        BCM_EXIT;
    }


    /* Set tdm ftmh settings */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_ftmh_set,(unit, p_fap_port,&ftmh_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_tdm_editing_get(int unit,
                                 bcm_gport_t gport,
                                 bcm_fabric_tdm_editing_t *editing)
{
    bcm_error_t rc = BCM_E_NONE;
    bcm_module_t modid;
    SOC_TMC_TDM_FTMH_INFO ftmh_info;
    uint32 soc_sand_rv;
    int is_ingress;
    int core;
    int index, nof_bytes;
    uint32 tmp, p_fap_port;
    bcm_port_t local_port;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(editing);

    SOC_TMC_TDM_FTMH_INFO_clear(&ftmh_info);

    /* Check TDM is supported */
    if (!SOC_IS_DPP_TDM_OPTIMIZE(unit) && !SOC_IS_DPP_TDM_STANDARD(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM")));
    }

    /* Validate Gport, retreive port id */
    rc = _bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
    BCMDNX_IF_ERR_EXIT(rc);

    /* Validate port is TDM port */
    if(!IS_TDM_PORT(unit, gport_info.local_port)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PORT,(_BSL_BCM_MSG("given gport: 0x%x is not a tdm port"),gport));
    }
    
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, gport_info.local_port, &p_fap_port, &core)));

    /*Set default user defined values to be zero*/
    for (index = 0; index <  BCM_FABRIC_TDM_USER_FIELD_MAX_SIZE; index++) {
        editing->user_field[0] = 0;
    }
    editing->user_field_count = 0;

    /* Retrieve port TDM configuration */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_ftmh_get,(unit, p_fap_port,&ftmh_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
    /* Get packet crc. (only if feature avaialable) */
    if (SOC_DPP_CONFIG(unit)->tdm.is_bypass) { 
      uint8 is_ingress_enabled = 0, is_egress_enabled = 0;

      /* validate parameter */
      soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_port_packet_crc_get,(unit, p_fap_port, &is_ingress_enabled, &is_egress_enabled)));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      editing->add_packet_crc = (editing->flags & BCM_FABRIC_TDM_EDITING_NON_SYMMETRIC_CRC) ?
          ((editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS ? is_ingress_enabled : FALSE) |
           (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS ? is_egress_enabled : FALSE)) :
        (is_ingress_enabled & is_egress_enabled);
    }

    /* Get ingress configuration */
    if (editing->flags & BCM_FABRIC_TDM_EDITING_INGRESS) {
        is_ingress = 1;
        rc = _bcm_petra_fabric_tdm_from_action_to_editing_type(unit,is_ingress,&ftmh_info,&(editing->type));
        BCMDNX_IF_ERR_EXIT(rc);

        /* Get destination only if ftmh is appended */
        if (ftmh_info.action_ing != SOC_TMC_TDM_ING_ACTION_NO_CHANGE) {
            if (ftmh_info.is_mc) {
                editing->flags |= BCM_FABRIC_TDM_EDITING_MULTICAST;
                /* Multicast destination */
                if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {
                    _BCM_MULTICAST_GROUP_SET(editing->multicast_id,_BCM_PETRA_MULTICAST_TM_TYPE,ftmh_info.ftmh.opt_mc.mc_id);
                } else {
                    _BCM_MULTICAST_GROUP_SET(editing->multicast_id,_BCM_PETRA_MULTICAST_TM_TYPE,ftmh_info.ftmh.standard_mc.mc_id);
                }            
            } else {
                /* Unicast destination */
                editing->flags |= BCM_FABRIC_TDM_EDITING_UNICAST;
                if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {                                        
                    
                    
                    modid = ftmh_info.ftmh.opt_uc.dest_fap_id;
                    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_tm_to_local_port_get,(unit, ftmh_info.ftmh.opt_uc.dest_if, 0, &local_port)));

                    BCM_GPORT_MODPORT_SET(editing->destination_port,modid,local_port);
                } else {
                    if (SOC_IS_PETRAB(unit)) {
                      BCM_GPORT_SYSTEM_PORT_ID_SET(editing->destination_port,ftmh_info.ftmh.standard_uc.sys_phy_port);
                    } else {
                      rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_tm_to_local_port_get,(unit, ftmh_info.ftmh.standard_uc.dest_fap_port, 0, &local_port));
                      BCMDNX_IF_ERR_EXIT(rc);

                      modid = ftmh_info.ftmh.standard_uc.dest_fap_id;
                      BCM_GPORT_MODPORT_SET(editing->destination_port,modid,local_port);
                    }
                }
            }
        }

        /* Get user defined only if action is add */        
        if (ftmh_info.action_ing == SOC_TMC_TDM_ING_ACTION_ADD && SOC_IS_DPP_TDM_STANDARD(unit)) {
            /* 3. User define settings */            
            /* Soc_petra-B can handle only 32 bits */
            editing->user_field_count = SOC_DPP_CONFIG(unit)->tdm.max_user_define_bits;
            
            if (editing->flags & BCM_FABRIC_TDM_EDITING_UNICAST) {  
                nof_bytes = editing->user_field_count >> 3;
                for (index = 0; index <  nof_bytes; index++) {
                    tmp = 0;
                    
                    soc_sand_rv = soc_sand_bitstream_get_any_field(
                        &(ftmh_info.ftmh.standard_uc.user_def),
                        index*8,
                        (index+1)*8-1,
                        &tmp
                        );            
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    editing->user_field[index] = tmp;
                }
            } else {
                nof_bytes = editing->user_field_count >> 3;
                for (index = 0; index <  nof_bytes; index++) {
                    tmp = 0;
                    
                    soc_sand_rv = soc_sand_bitstream_get_any_field(
                        &(ftmh_info.ftmh.standard_mc.user_def),
                        index*8,
                        (index+1)*8-1,
                        &tmp
                        );            
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
                    editing->user_field[index] = tmp;
                }
            }
        }
    } else if (editing->flags & BCM_FABRIC_TDM_EDITING_EGRESS) {
        is_ingress = 0;
        rc = _bcm_petra_fabric_tdm_from_action_to_editing_type(unit,is_ingress,&ftmh_info,&(editing->type));
        BCMDNX_IF_ERR_EXIT(rc);
    } else {
        /* No ask for Ingress or Egress flags */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid flags parameters 0x%x, should includ ingress or egress flag"),editing->flags));
    }
   
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_tdm_direct_routing_set(int unit,
                                  bcm_gport_t gport,
                                  bcm_fabric_tdm_direct_routing_t *routing_info)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rc;
    bcm_port_t link_i;
    bcm_port_t port_i;
    int index, old_link_ptr, is_last, new_link_ptr, is_allocated;
    SOC_TMC_TDM_DIRECT_ROUTING_INFO direct_routing_information, cleaned_direct_routing_information;
    _bcm_dpp_gport_info_t gport_info;
    uint8 enable_rpt_reachable = TRUE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

    /* The API is useful only if direct routing feature is avaliable. i.e. working in bypass mode */
    if (!(SOC_DPP_CONFIG(unit)->tdm.is_bypass)) { 
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only in bypass mode is supported")));
    }

    SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(&direct_routing_information);
    SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(&cleaned_direct_routing_information);

    /* Verify routing information */
    BCMDNX_NULL_CHECK(routing_info);

    if (routing_info->links_count < 0 || routing_info->links_count > SOC_DPP_DEFS_GET(unit, nof_fabric_links)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid links count, links_count %d"), routing_info->links_count));
    }

    /* Verify links and set accordingly link bitmap */
    for (index = 0; index < routing_info->links_count; index++)
    {
      link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, routing_info->links[index]);
    
      if(!SOC_PORT_VALID_RANGE(unit, link_i))
      {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Link 0x%x is out-of-range"), unit, routing_info->links[index]));
      }
      
      /* Set link_i in bitmap */
      soc_sand_rc = soc_sand_bitstream_set_bit(&(direct_routing_information.link_bitmap.arr[0]),link_i);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
    }

    /* setting a link count of zero implies no direct routing - hence seeting all links to all-ones */
    if(routing_info->links_count == 0) {
        direct_routing_information.link_bitmap.arr[0] = 0xffffffff;
        direct_routing_information.link_bitmap.arr[1] = 0xf;
    }

    /* Retreive Port from gport */
    rc = _bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info);
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
      /* Validate port is TDM port */
      if(!IS_TDM_PORT(unit, port_i)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PORT,(_BSL_BCM_MSG("given gport: 0x%x is not a tdm port"),gport));
      }

      /* Set all ports with the same routing information */
      rc = _bcm_dpp_am_template_fabric_tdm_link_ptr_exchange(unit,port_i,&direct_routing_information,&old_link_ptr, &is_last, &new_link_ptr,&is_allocated);
      BCMDNX_IF_ERR_EXIT(rc);

      /* Commit Changes in HW according to routing information */
      if (is_allocated) {
          soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_direct_routing_set,(unit, new_link_ptr, &direct_routing_information, enable_rpt_reachable)));
          BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
      }

      /* Set new mapping */
      soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_direct_routing_profile_map_set,(unit, port_i, new_link_ptr)));
      BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

      /* last mapped, remove routing information */
      if (is_last && (new_link_ptr != old_link_ptr)) {
          soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_direct_routing_set,(unit, old_link_ptr, &cleaned_direct_routing_information, enable_rpt_reachable)));
          BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
      }
    }
exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_fabric_tdm_direct_routing_get(int unit,
                                  bcm_gport_t gport,
                                  bcm_fabric_tdm_direct_routing_t *routing_info)
{
    bcm_error_t rc = BCM_E_NONE;
    bcm_port_t link_i;
    bcm_port_t port_i;
    SOC_TMC_TDM_DIRECT_ROUTING_INFO direct_routing_information;
    _bcm_dpp_gport_info_t gport_info;

    BCMDNX_INIT_FUNC_DEFS;
    /* The API is useful only if direct routing feature is avaliable */
    if (!(SOC_DPP_CONFIG(unit)->tdm.is_bypass)) { 
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Only bypass mode is supported")));
    }

    BCMDNX_NULL_CHECK(routing_info);

    SOC_TMC_TDM_DIRECT_ROUTING_INFO_clear(&direct_routing_information);

    /* Retreive Port from gport */
    rc = _bcm_dpp_gport_to_phy_port(unit, gport, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info); 
    BCMDNX_IF_ERR_EXIT(rc);

    BCM_PBMP_ITER(gport_info.pbmp_local_ports, port_i) {
      /* Validate port is TDM port */
      if(!IS_TDM_PORT(unit, port_i)) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PORT,(_BSL_BCM_MSG("given gport: 0x%x is not a tdm port"),gport));
      }
      /* Retreive routing information from port */
      rc = _bcm_dpp_am_template_fabric_tdm_link_ptr_data_get(unit, port_i ,&direct_routing_information);
      BCMDNX_IF_ERR_EXIT(rc);

      /* Fill routing information, all valid links */
      routing_info->links_count = 0;
      PBMP_SFI_ITER(unit, port_i) {
        link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port_i);
        if (soc_sand_bitstream_test_bit(direct_routing_information.link_bitmap.arr,link_i)) {
          routing_info->links[routing_info->links_count++] = port_i;
        }
      }
      
      /* Retreive only first port */
      break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_petra_fabric_tdm_range_size_set(int unit,
                                     int minimum_size,
                                     int maximum_size)
{
    uint32 soc_sand_rv = 0;    

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (SOC_IS_DPP_TDM_STANDARD(unit) || !SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size) { /* Standard or not fixed Optimize size (not Petra-B in system) */       
        SOC_SAND_U32_RANGE size_range;

        /* TDM standard validate size range */
        
        if (minimum_size < SOC_DPP_CONFIG(unit)->tdm.min_cell_size || maximum_size > SOC_DPP_CONFIG(unit)->tdm.max_cell_size
            || minimum_size > maximum_size) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid cell size range: Minimum size: %d, Maximum size: %d"),minimum_size,maximum_size));
        }

        /* TDM standard set size range*/
        size_range.start = minimum_size;
        size_range.end = maximum_size;

        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_stand_size_range_set,(unit, &size_range)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    } else if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {
        /* TDM optimize */

        /* In Soc_petra-B cell size is fixed */
        
        if (SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size) {
            uint32 cell_size;

            cell_size = minimum_size;

            /* TDM optimize validate size range */
            
            if (cell_size < SOC_DPP_CONFIG(unit)->tdm.min_cell_size ||  maximum_size > SOC_DPP_CONFIG(unit)->tdm.max_cell_size
                || minimum_size != maximum_size) {
                LOG_ERROR(BSL_LS_BCM_FABRIC,
                          (BSL_META_U(unit,
                                      "unit %d, invalid cell size: Minimum size: %d, Maximum size: %d \n"), unit,minimum_size,maximum_size));
            }
    
            /* TDM optimize set size range*/              
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_opt_size_set,(unit, cell_size)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {       
            /* Not TDM Mode */
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM"))); 
        }
    } else {
       /* Not TDM Mode */
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM"))); 
    }
        
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_tdm_range_size_get(int unit,
                                     int *minimum_size,
                                     int *maximum_size)
{
    uint32 soc_sand_rv = 0;    

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(minimum_size);
    BCMDNX_NULL_CHECK(maximum_size);

    if (SOC_IS_DPP_TDM_STANDARD(unit) || !SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size) { /* Standard or Not fixed Optimize size (not Petra-B in system) */  
        SOC_SAND_U32_RANGE size_range;
        /* TDM standard size get */

        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_stand_size_range_get,(unit, &size_range)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);        

        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *minimum_size = size_range.start;
        /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
        /* coverity[uninit_use:FALSE] */
        *maximum_size = size_range.end;
    } else if (SOC_IS_DPP_TDM_OPTIMIZE(unit)) {
        /* TDM optimize size get */
        /* In Soc_petra-B cell size is fixed */
        
        if (SOC_DPP_CONFIG(unit)->tdm.is_fixed_opt_cell_size) {
            uint32 cell_size = 0;
            soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_tdm_opt_size_get,(unit, &cell_size)));
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 
            *minimum_size = *maximum_size = cell_size;
        } else {            
           /* Not TDM Mode */
           BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM"))); 
        }
    } else {
       /* Not TDM Mode */
       BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("not supported TDM"))); 
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_tdm_from_action_to_editing_type(int unit,
                                                  int is_ingress,
                                                  SOC_TMC_TDM_FTMH_INFO *ftmh_info,
                                                  bcm_fabric_tdm_editing_type_t *editing_type)
{

   BCMDNX_INIT_FUNC_DEFS;
   BCMDNX_NULL_CHECK(editing_type);
   BCMDNX_NULL_CHECK(ftmh_info);

   if (is_ingress) {
       /* get ingress action */
       switch(ftmh_info->action_ing) {
            case SOC_TMC_TDM_ING_ACTION_ADD:
                *editing_type = bcmFabricTdmEditingAppend;
                break;
            case SOC_TMC_TDM_ING_ACTION_NO_CHANGE:
                *editing_type = bcmFabricTdmEditingNoChange;
                break;
            case SOC_TMC_TDM_ING_ACTION_CUSTOMER_EMBED:
                *editing_type = bcmFabricTdmEditingCustomExternal;
                break;
           default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported ingress action type %d"), ftmh_info->action_ing));
        }
   } else {
       /* get egress action */
       switch(ftmh_info->action_eg) {
            case SOC_TMC_TDM_EG_ACTION_REMOVE:
                *editing_type = bcmFabricTdmEditingRemove;
                break;
            case SOC_TMC_TDM_EG_ACTION_NO_CHANGE:
                *editing_type = bcmFabricTdmEditingNoChange;
                break;
            case SOC_TMC_TDM_EG_ACTION_CUSTOMER_EXTRACT:
                *editing_type = bcmFabricTdmEditingCustomExternal;
                break;
           default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported egress action type %d"), ftmh_info->action_eg));
        }       
   }
   
   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_tdm_to_action_from_editing_type(int unit,
                                                  int is_ingress,
                                                  SOC_TMC_TDM_FTMH_INFO *ftmh_info,
                                                  bcm_fabric_tdm_editing_type_t *editing_type)
{

   BCMDNX_INIT_FUNC_DEFS;
   BCMDNX_NULL_CHECK(editing_type);
   BCMDNX_NULL_CHECK(ftmh_info);

   if (is_ingress) {
       /* get ingress action */
       switch(*editing_type) {
            case bcmFabricTdmEditingAppend:
                ftmh_info->action_ing = SOC_TMC_TDM_ING_ACTION_ADD;
                break;
            case bcmFabricTdmEditingNoChange:
                ftmh_info->action_ing = SOC_TMC_TDM_ING_ACTION_NO_CHANGE;
                break;
           case bcmFabricTdmEditingCustomExternal:
                ftmh_info->action_ing = SOC_TMC_TDM_ING_ACTION_CUSTOMER_EMBED;
                break;
           default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported ingress editing type %d"), *editing_type));
        }
   } else {
       /* get egress action */
       switch(*editing_type) {
            case bcmFabricTdmEditingRemove:
                ftmh_info->action_eg = SOC_TMC_TDM_EG_ACTION_REMOVE;
                break;
            case bcmFabricTdmEditingNoChange:
                ftmh_info->action_eg = SOC_TMC_TDM_EG_ACTION_NO_CHANGE;
                break;
            case bcmFabricTdmEditingCustomExternal:
                ftmh_info->action_eg = SOC_TMC_TDM_EG_ACTION_CUSTOMER_EXTRACT;
                break;
           default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("not supported egress editing type %d"),*editing_type));
        }       
   }
   
   BCM_EXIT;
exit:
   BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_petra_fabric_vsq_category_set(int unit,                                   
                                   bcm_fabric_control_t control_type,
                                   int queue_id_in,
                                   int *queue_id_out)
{
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_sand_rv;
    int vsq_category_id;
    SOC_TMC_ITM_CATEGORY_RNGS info;
    int mode;
    SOC_SAND_U32_RANGE queue_range;
    int is_min = 0;
    int *queue_id = queue_id_out;

    BCMDNX_INIT_FUNC_DEFS;

    *queue_id = queue_id_in;

    /* Clear */
    SOC_TMC_ITM_CATEGORY_RNGS_clear(&info);

    /* validate parameters */        
    if (*queue_id < 0 || *queue_id > DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id given %d is out of range"),*queue_id));
    }

    switch (control_type) {
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricMulticastQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricMulticastQueueMax:
        vsq_category_id = 0;
        break;
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricShaperQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricShaperQueueMax:
        vsq_category_id = 1;
        break;
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricQueueMax:
        vsq_category_id = 2;
        break;
    /* coverity[unterminated_case:FALSE] */
    case bcmFabricRecycleQueueMin:
        is_min = 1;
        /* coverity[fallthrough:FALSE] */
    case bcmFabricRecycleQueueMax:
        vsq_category_id = 3;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fabric control mode %d invalid for vsq categories"),control_type));
    }
    
    /* Set new VSQ category range */ 
    rc = bcm_petra_fabric_control_get(unit,bcmFabricVsqCategory,&mode);
    BCMDNX_IF_ERR_EXIT(rc); 

    if (mode == bcmFabricVsqCatagoryModeNone) {
        /* All VOQs are in the same category 2 except VOQs 0-3. */
        info.vsq_ctgry0_end = 3;
        info.vsq_ctgry1_end = 3;
        info.vsq_ctgry2_end = DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit);        
    } else if (mode == bcmFabricVsqCatagoryMode1 || mode == bcmFabricVsqCatagoryMode2) {    

        soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_category_rngs_get,(unit,&info)));
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
        if (mode == bcmFabricVsqCatagoryMode2 && vsq_category_id != 2 && vsq_category_id != 0) {
            /* Nothing to do */
            BCM_EXIT;
        }

        switch (vsq_category_id) {
        case 0:
            queue_range.start = 0;
            queue_range.end = info.vsq_ctgry0_end;
            break;
        case 1:
            queue_range.start = SOC_SAND_MIN(info.vsq_ctgry1_end,info.vsq_ctgry0_end+1);
            queue_range.end = info.vsq_ctgry1_end; 
            break;           
        case 2:
            queue_range.start = SOC_SAND_MIN(info.vsq_ctgry2_end,info.vsq_ctgry1_end+1);
            queue_range.end = info.vsq_ctgry2_end; 
            break;
        case 3:
            queue_range.start = SOC_SAND_MIN(SOC_SAND_MIN(info.vsq_ctgry2_end,DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit)),info.vsq_ctgry2_end+1);
            queue_range.end = DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit); 
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("fabric control mode %d invalid vsq category id %d "), control_type,vsq_category_id));
        }

        if (is_min) {
            queue_range.start = *queue_id;
            if (queue_range.end < queue_range.start) {
                queue_range.end = *queue_id;
            }
        } else {
            queue_range.end = *queue_id;
            if (queue_range.end < queue_range.start) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fabric control mode %d invalid vsq range given, range end is lower than start %d"),control_type,*queue_id));
            }
        }

        switch (vsq_category_id) {
        case 0:
            info.vsq_ctgry0_end = queue_range.end;
            /* Align other categories accordingly */
            info.vsq_ctgry1_end = SOC_SAND_MAX(info.vsq_ctgry0_end,info.vsq_ctgry1_end);
            info.vsq_ctgry2_end = SOC_SAND_MAX(info.vsq_ctgry1_end,info.vsq_ctgry2_end);
            break;
        case 1:
            /* Take as previous end the range.start-1 only if different to zero, otherwise start-1 returns -1 */
            info.vsq_ctgry0_end = SOC_SAND_MAX(SOC_SAND_MAX(queue_range.start,1)-1,0);
            info.vsq_ctgry1_end = queue_range.end;
            /* Align other categories accordingly */            
            info.vsq_ctgry2_end = SOC_SAND_MAX(info.vsq_ctgry1_end,info.vsq_ctgry2_end);   
            /* update new range */
            queue_range.start = info.vsq_ctgry0_end + 1; 
            queue_range.end = info.vsq_ctgry1_end; 
            break;        
        case 2:
            info.vsq_ctgry1_end = SOC_SAND_MAX(SOC_SAND_MAX(queue_range.start,1)-1,info.vsq_ctgry0_end);
            info.vsq_ctgry2_end = queue_range.end;
            /* update new range */
            queue_range.start = info.vsq_ctgry1_end + 1; 
            break;
        case 3:
            info.vsq_ctgry2_end = SOC_SAND_MAX(SOC_SAND_MAX(queue_range.start,1)-1,info.vsq_ctgry1_end);            
            /* update new range */
            queue_range.start = info.vsq_ctgry2_end + 1; 
            break;
        /* coverity[dead_error_begin:FALSE] */
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("fabric control mode %d invalid vsq category id %d "), control_type,vsq_category_id));
        }
        *queue_id = is_min? queue_range.start :queue_range.end;
    } else {
        /* Not valid category modes */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VSQ Category invalid %d"),mode)); 
    }

    /* Set new ranges */
    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_category_rngs_set,(unit,&info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    
exit:
    BCMDNX_FUNC_RETURN;
}                                                                    
 
/* Get vsq categories */
STATIC int 
_bcm_petra_fabric_vsq_category_get(int unit,                                   
                                   bcm_fabric_control_t control_type,
                                   int *queue_id)
{
    bcm_error_t rc = BCM_E_NONE;
    int mode;
    uint32 soc_sand_rv;
    SOC_TMC_ITM_CATEGORY_RNGS info;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    /* Clear */
    SOC_TMC_ITM_CATEGORY_RNGS_clear(&info);

    BCMDNX_NULL_CHECK(queue_id);

    rc = bcm_petra_fabric_control_get(unit,bcmFabricVsqCategory,&mode);
    BCMDNX_IF_ERR_EXIT(rc); 

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_category_rngs_get,(unit,&info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    if (mode == bcmFabricVsqCatagoryModeNone) {
        /* All VOQs are in the same category 2 and being mapped to 4-96K. Multicast Queues 0-3 */
        switch (control_type) {
          case bcmFabricMulticastQueueMin:
            *queue_id = 0;
            break;
          case bcmFabricMulticastQueueMax:
            *queue_id = 3;
            break;
          case bcmFabricQueueMin:
            *queue_id = 4;
            break;
          case bcmFabricQueueMax:
            *queue_id = DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit);
            break;
        default:
            *queue_id = 0;
        }
        /* Nothing to do */        
        BCM_EXIT;   
    }
     
    switch (control_type) {
    case bcmFabricMulticastQueueMin:
        *queue_id = 0;
        break;
    case bcmFabricMulticastQueueMax:
        *queue_id = info.vsq_ctgry0_end;
        break;
    case bcmFabricShaperQueueMin:
        *queue_id = SOC_SAND_MIN(info.vsq_ctgry0_end+1,info.vsq_ctgry1_end);
        break;
    case bcmFabricShaperQueueMax:
        *queue_id = info.vsq_ctgry1_end;
        break;
    case bcmFabricQueueMin:
        *queue_id = SOC_SAND_MIN(info.vsq_ctgry1_end+1,info.vsq_ctgry2_end);
        break;
    case bcmFabricQueueMax:
        *queue_id = info.vsq_ctgry2_end;
        break;
    case bcmFabricRecycleQueueMin:
        *queue_id = SOC_SAND_MIN(info.vsq_ctgry2_end+1,DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit));
        break;
    case bcmFabricRecycleQueueMax:
        *queue_id = DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit);
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fabric control mode %d invalid for vsq categories"),control_type));
    }

    if (mode == bcmFabricVsqCatagoryMode2 && (control_type != bcmFabricQueueMax) && (control_type != bcmFabricQueueMin)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fabric control mode %d invalid for vsq categories mode %d"), control_type, mode));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_unicast_queue_range_get(int unit,
                                          bcm_fabric_control_t control_type,
                                          int* queue_id)
{
    bcm_error_t rc;
    
    BCMDNX_INIT_FUNC_DEFS;
    /* Currently get from VSQ categories */
    rc = _bcm_petra_fabric_vsq_category_get(unit,control_type,queue_id);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Purpose: Ingress shaper queue range */
STATIC int
_bcm_petra_fabric_ingress_shaper_queue_range_set(int unit,
                                                  bcm_fabric_control_t type,
                                                  int queue_id)
{
    SOC_TMC_ITM_INGRESS_SHAPE_INFO isp_info;
    uint32 soc_sand_rc;
    bcm_error_t rc = BCM_E_NONE;
    int *queue_region_config = NULL;
    int voq_1k_start, voq_1k_end;
    int have_first = FALSE;
    int have_last = FALSE;
    int region;
    int current_voq_1k_start = -1, current_voq_1k_end = -1;
    int hr_port;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
 
    /* Validate queue id */
    if (queue_id > DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit)) {
        LOG_ERROR(BSL_LS_BCM_FABRIC,
                  (BSL_META_U(unit,
                              "unit %d, queue_id %d invalid\n"), unit, queue_id));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, queue_id %d invalid\n"), unit, queue_id));
    }
    if ((queue_id % 1024) && (queue_id != 4) && (type == bcmFabricShaperQueueMin)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("queue_id min %d must be a multiple of 1024"),queue_id));
    }    
    if (((queue_id+1) % 1024) && (type == bcmFabricShaperQueueMax)) {
        LOG_ERROR(BSL_LS_BCM_FABRIC,
                  (BSL_META_U(unit,
                              "unit %d, shaper queue_id min %d must be a multiple of 1024 then -1\n"), unit, queue_id));
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, shaper queue_id min %d must be a multiple of 1024 then -1\n"), unit, queue_id));
    }

    /* Special case: queue_id is 4, in that case we treat it as queue_id 0. */
    if (queue_id == 4 && (type == bcmFabricShaperQueueMin)) {
        queue_id = 0;
    }

    /* Allocation manager configuration */
    queue_region_config = sal_alloc(sizeof(int) * DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit), "queue_region");

    if (queue_region_config == NULL) {
         LOG_ERROR(BSL_LS_BCM_FABRIC,
                   (BSL_META_U(unit,
                               "unit %d, resource memory allocation failure\n"), unit));
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, resource memory allocation failure\n"), unit));
    }

    rc = bcm_dpp_am_cosq_queue_region_config_get(unit, queue_region_config, DPP_DEVICE_COSQ_QUEUE_REGION_ISQ);
    BCMDNX_IF_ERR_EXIT(rc);

    for (region = 0;
         region < (DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit));
         region++) {
        if ((queue_region_config[region] == DPP_DEVICE_COSQ_QUEUE_REGION_ISQ) 
            && (have_first == FALSE)) {

            have_first = TRUE;
            current_voq_1k_start = region;

        } else if ((queue_region_config[region] != DPP_DEVICE_COSQ_QUEUE_REGION_ISQ) 
                   && (have_first == TRUE) 
                   && (have_last == FALSE)){
                   current_voq_1k_end = region;
        }
    }
 
    if (type == bcmFabricShaperQueueMin) {
        voq_1k_start = queue_id/1024;
        if (current_voq_1k_end == -1) {
             current_voq_1k_end = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
        }
        voq_1k_end = current_voq_1k_end;
    } else {
        if (current_voq_1k_end == -1) {
             current_voq_1k_end = DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit);
        }
        voq_1k_start = current_voq_1k_start;
        voq_1k_end = (queue_id + 1)/1024;
    }

    for (region = voq_1k_start;
         region < voq_1k_end;
         region++) {
        queue_region_config[region] = DPP_DEVICE_COSQ_QUEUE_REGION_ISQ;
    }
    
    rc = _bcm_dpp_am_cosq_process_queue_region(unit, queue_region_config);
    BCMDNX_IF_ERR_EXIT(rc);

    /* Set ISQ range in hardware */
    SOC_TMC_ITM_INGRESS_SHAPE_INFO_clear(&isp_info);
    
    soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_ingress_shape_get,(unit, &isp_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    hr_port = isp_info.sch_port;

    if (voq_1k_start < voq_1k_end) {

        /* Allocate hr resource only in case of MIN */
        if (type == bcmFabricShaperQueueMin) {
            rc = bcm_petra_cosq_isq_hr_allocate(unit, &hr_port);
        }
        
    } else {
        /* deallocate hr resource */
      hr_port = (int)isp_info.sch_port;
      rc = bcm_petra_cosq_isq_hr_deallocate(unit, &hr_port);
    }

    isp_info.q_range.q_num_low = voq_1k_start * 1024;
    isp_info.q_range.q_num_high = (voq_1k_end *1024) - 1;
    if (SOC_IS_PETRAB(unit)) {
        /* in case of Petra-B must be in valid range 0-32K-2 */
        isp_info.q_range.q_num_high = (isp_info.q_range.q_num_high > (32*1024-2) ? (32*1024-2):isp_info.q_range.q_num_high);
    }
    isp_info.sch_port = hr_port;
    /* must be enabled to set q range */
    isp_info.enable = TRUE;

    soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_ingress_shape_set,(unit, &isp_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    /* Set VSQ categories */
    rc = _bcm_petra_fabric_vsq_category_set(unit, type, queue_id, &queue_id);
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    if (queue_region_config != NULL) {
        sal_free(queue_region_config);
    }
    BCMDNX_FUNC_RETURN;

}

STATIC int
_bcm_petra_fabric_ingress_shaper_queue_range_get(int unit,
                         bcm_fabric_control_t control_type,
                                                 int* queue_id)
{
    SOC_TMC_ITM_INGRESS_SHAPE_INFO isp_info;
    uint32 soc_sand_rc;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(queue_id);    

    /* Get ISQ range in hardware */
    SOC_TMC_ITM_INGRESS_SHAPE_INFO_clear(&isp_info);
    
    soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_ingress_shape_get,(unit, &isp_info)));
    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);

    if (control_type == bcmFabricShaperQueueMin) {
        *queue_id = isp_info.q_range.q_num_low;
    }

    if (control_type == bcmFabricShaperQueueMax) {
        *queue_id = isp_info.q_range.q_num_high;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 *  Purpose: set Egress queue ranges 
 *  Egress TM queues are defined Queues for the process of enable the user to
 *  have some sort of scheduling for egress TM queues.
 *  This is done by:
 *  1. Define egress queue ranges (see 4.).
 *  2. Define MC egress group for certain ports.
 *  3. Set recycle process for specific ports (in the MC egress group).  
 *  4. Set ingress queues to receive the packets that coming from recycle, so it enables for the user to decide on scheduling for these egress queues.
 */ 
STATIC int
_bcm_petra_fabric_egress_queue_range_set(int unit,
                                          bcm_fabric_control_t type,
                                          int queue_id)
{
    bcm_error_t rc;

    BCMDNX_INIT_FUNC_DEFS;
    

    /* Currently set only VSQ categories */
    rc = _bcm_petra_fabric_vsq_category_set(unit,type, queue_id, &queue_id);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}
STATIC int
_bcm_petra_fabric_egress_queue_range_get(int unit,
                                         bcm_fabric_control_t control_type,
                                         int* queue_id)
{
    bcm_error_t rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    /* Currently get from VSQ categories */
    rc = _bcm_petra_fabric_vsq_category_get(unit,control_type,queue_id);
    BCMDNX_IF_ERR_EXIT(rc);
exit:
    BCMDNX_FUNC_RETURN;
}

/* System red */
/* 
 * set discard configuration. Configuration to enable, disable aging and
 * its settings
 */
int bcm_petra_fabric_config_discard_set(
    int unit, 
    bcm_fabric_config_discard_t *discard)
{

    SOC_TMC_ITM_SYS_RED_EG_INFO info, exact_info;

    BCMDNX_INIT_FUNC_DEFS;

    SOC_TMC_ITM_SYS_RED_EG_INFO_clear(&info);
    SOC_TMC_ITM_SYS_RED_EG_INFO_clear(&exact_info);

    info.enable = discard->enable;
    info.aging_timer = discard->age;
    if (discard->age_mode == bcmFabricAgeModeReset) {
        info.reset_expired_q_size = 0x1;
    } 
    if (discard->age_mode == bcmFabricAgeModeDecrement) {
        info.aging_only_dec_q_size = 0x1;
    }

    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_sys_red_eg_set,(unit, &info, &exact_info)));

exit:
  BCMDNX_FUNC_RETURN;
}

/* get discard configuration. */
int bcm_petra_fabric_config_discard_get(
    int unit, 
    bcm_fabric_config_discard_t *discard)
{
    SOC_TMC_ITM_SYS_RED_EG_INFO info;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(&info, 0x0, sizeof(info));

    BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_sys_red_eg_get,(unit, &info)));

    discard->enable = info.enable;
    discard->age = info.aging_timer;
    if (info.reset_expired_q_size == 0x1) {
        discard->age_mode = bcmFabricAgeModeReset;
    } 
    if (info.aging_only_dec_q_size == 0x1) {
        discard->age_mode = bcmFabricAgeModeDecrement;
    }

exit:
  BCMDNX_FUNC_RETURN;
}

#ifdef BCM_ARAD_SUPPORT
STATIC int
_bcm_petra_fabric_link_control_set_llfc_control_source(int unit, bcm_port_t link, int arg)
{
    uint32 soc_sand_rv, mask;
    bcm_port_t link_i;
    int nof_pipes;
    BCMDNX_INIT_FUNC_DEFS;

    nof_pipes = SOC_DPP_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link);
    
    BCM_DPP_UNIT_CHECK(unit);
    if(!SOC_PORT_VALID_RANGE(unit, link_i))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Link %d is out-of-range"), unit, link_i));
    }

    SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(&mask, nof_pipes);

    /*validation*/
    if ( (!(SOC_DCMN_FABRIC_PIPE_ALL_PIPES_IS_CLEAR(arg))) &&  (!(SOC_DCMN_FABRIC_PIPE_ALL_PIPES_IS_SET(arg, mask))) ) /* if enable, then all bits must be on */
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("LLFC must be for all pipes")));
    }

    /*set registers*/
    soc_sand_rv = arad_fabric_links_llf_control_source_set(unit, link_i, arg);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:   
    BCMDNX_FUNC_RETURN;
}
#endif

STATIC int
_bcm_petra_fabric_link_repeater_enable_set(int unit, bcm_port_t port, int enable)
{
    int rv;
    uint32 burst, pps;
    int speed, empty_cell_size;
    uint64 empty_cell_size64, pps64;
    BCMDNX_INIT_FUNC_DEFS;

    if (enable)
    {
        /* 
         * Repeater link enable -
         *  if cell shaper is enabled set empty cell size accordingly.                      -
         *  Else set to repeater link default cell size.                                                                                -
         */
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_rate_egress_pps_get,(unit, port, &pps, &burst)));

        rv = bcm_petra_port_speed_get(unit, SOC_DPP_FABRIC_LINK_TO_PORT(unit, port), &speed);
        BCMDNX_IF_ERR_EXIT(rv);

        if (pps == 0)
        {
            rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_link_repeater_enable_set,(unit, port, 1, SOC_DPP_DEFS_GET(unit, repeater_default_empty_cell_size)));
            BCMDNX_IF_ERR_EXIT(rv); 
        } else {
            /*empty_cell_size [bytes/cell] = speed [bytes/sec] / pps [cells/sec]*/
            COMPILER_64_SET(empty_cell_size64, 0, speed);
            COMPILER_64_UMUL_32(empty_cell_size64, (1000000 / 8)); /*Mega-bits to bytes*/

            COMPILER_64_SET(pps64, 0, pps);
            COMPILER_64_UDIV_64(empty_cell_size64, pps64); 

            empty_cell_size = COMPILER_64_LO(empty_cell_size64);

            rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_link_repeater_enable_set,(unit, port, 1, empty_cell_size));
            BCMDNX_IF_ERR_EXIT(rv); 
        }

    } else {
        /*Disable repeater link and reset empty cell size*/
        rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_repeater_enable_set,(unit, port, 0, SOC_DPP_DEFS_GET(unit, repeater_none_empty_cell_size)));
        BCMDNX_IF_ERR_EXIT(rv);           
    }
   
exit:   
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_fabric_link_repeater_enable_get(int unit, bcm_port_t port, int *enable)
{
    int rv;
    int empty_cell_size;
    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_link_repeater_enable_get,(unit, port, enable, &empty_cell_size));
    BCMDNX_IF_ERR_EXIT(rv); 

exit:   
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_fabric_link_control_set
 * Purpose:
 *      Set fabric-link attribute (fifo ype) per link. Each link might
 *      have two fifo types; one per RX and one per TX
 * Parameters:
 *      unit - (IN) Unit number.
 *      link - (IN) Link Number 
 *      type - (IN) Control type
 *      arg  - (IN) Value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_fabric_link_control_set(
                                int unit, 
                                bcm_port_t link, 
                                bcm_fabric_link_control_t type, 
                                int arg)
{
#ifdef BCM_ARAD_SUPPORT
    bcm_error_t rc = BCM_E_NONE;
    uint32 soc_rv;
    bcm_port_t link_i;
#endif
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    _BCM_DPP_SWITCH_API_START(unit);

#ifdef BCM_PETRAB_SUPPORT
    if SOC_IS_PETRAB(unit){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_fabric_link_control_set is not supported in Soc_petra-B")));
    }
#endif
    /*General validation*/
    /*DPP_LINK_INPUT_CHECK(unit, link);*/

#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit)){
        link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link);
    
        BCM_DPP_UNIT_CHECK(unit);
        if(!SOC_PORT_VALID_RANGE(unit, link_i))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Link %d is out-of-range"), unit, link_i));
        }
        if(arg!=0 && arg!=1 ) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported arg %d"), arg));
        }
        switch(type)
        {
            case bcmFabricLinkCellInterleavingEnable:
                soc_rv = arad_fabric_links_cell_interleaving_set(unit, link_i, arg ? 1 : 0);
                BCMDNX_IF_ERR_EXIT(soc_rv);
                break;
            case bcmFabricLLFControlSource:
                rc = _bcm_petra_fabric_link_control_set_llfc_control_source(unit, link, arg);
                BCMDNX_IF_ERR_EXIT(rc);
                break;       
            case bcmFabricLinkIsolate:
                soc_rv =  arad_fabric_links_isolate_set(unit, link_i, arg ? soc_dcmn_isolation_status_isolated : soc_dcmn_isolation_status_active);
                BCMDNX_IF_ERR_EXIT(soc_rv);
                break;
            case bcmFabricLinkTxTrafficDisable:
                soc_rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_fabric_link_tx_traffic_disable_set, (unit, link_i, arg));
                BCMDNX_IF_ERR_EXIT(soc_rv);
                break;
            case bcmFabricLinkRepeaterEnable:
                soc_rv = _bcm_petra_fabric_link_repeater_enable_set(unit, link_i, arg);
                BCMDNX_IF_ERR_EXIT(soc_rv);
                break; 
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"),type)); 
        }  
        BCM_EXIT;
    }
#endif
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_fabric_link_control_set is not supported")));
exit: 
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN; 
}
/*
* Function:
*      bcm_petra_fabric_link_control_get
* Purpose:
*       Get fabric-link attribute (fifo type) per link and direction
* Parameters:
*      unit - (IN)  Unit number.
*      link - (IN)  Link Number 
*      type - (IN)  Control type 
*      arg  - (OUT) Value
* Returns:
*      BCM_E_xxx
* Notes:
*/
int 
bcm_petra_fabric_link_control_get(
                                int unit, 
                                bcm_port_t link, 
                                bcm_fabric_link_control_t type, 
                                int *arg)
{
#ifdef BCM_ARAD_SUPPORT
    uint32 soc_rv;
    bcm_port_t link_i;
#endif
    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_PETRAB_SUPPORT
    if SOC_IS_PETRAB(unit){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_fabric_link_control_get is not supported in Soc_petra-B")));
    }
#endif
#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit))
    {
        /*General validation*/
        BCMDNX_NULL_CHECK(arg);
        link_i = SOC_DPP_FABRIC_PORT_TO_LINK(unit, link);
    
        BCM_DPP_UNIT_CHECK(unit);
        if(!SOC_PORT_VALID_RANGE(unit, link_i))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unit %d, Link %d is out-of-range"), unit, link_i));
        }

        switch(type)
        {
            case bcmFabricLinkCellInterleavingEnable:
                soc_rv = arad_fabric_links_cell_interleaving_get(unit, link_i, arg); 
                BCMDNX_IF_ERR_EXIT(soc_rv);
                break;
            case bcmFabricLLFControlSource:
                soc_rv = arad_fabric_links_llf_control_source_get(unit, link_i, (soc_dcmn_fabric_control_source_t*)arg);
                BCMDNX_IF_ERR_EXIT(soc_rv);
                break;   
            case bcmFabricLinkIsolate:
                soc_rv = arad_fabric_links_isolate_get(unit, link_i, (soc_dcmn_isolation_status_t*)arg); 
                BCMDNX_IF_ERR_EXIT(soc_rv);
                break;
            case bcmFabricLinkTxTrafficDisable:
               soc_rv = MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_fabric_link_tx_traffic_disable_get, (unit, link_i, arg));
               BCMDNX_IF_ERR_EXIT(soc_rv);
               break;
            case bcmFabricLinkRepeaterEnable:
               soc_rv = _bcm_petra_fabric_link_repeater_enable_get(unit, link_i, arg);
               BCMDNX_IF_ERR_EXIT(soc_rv);
               break;
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"),type)); 
        }  
        BCM_EXIT;
    }
#endif
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_fabric_link_control_get is not supported")));
exit: 
    BCMDNX_FUNC_RETURN;    
}

/*
 * Function:
 *      bcm_petra_fabric_reachability_status_get
 * Purpose:
 *      Retrieves the links through which a remote module ID is
 *      reachable
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      moduleid    - (IN)  Module Id 
 *      links_max   - (IN)  Max size of links_array. If the size doesn't correlate to the device's number of links, 
                            the function returns with the error BCM_E_FULL
 *      links_array - (OUT) bitmap of the links from which 'moduleid' is reachable 
 *      links_count - (OUT) Size of links_array 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      No support for Soc_petra-B.  
 */
int 
bcm_petra_fabric_reachability_status_get(
    int unit, 
    int moduleid, 
    int links_max, 
    uint32 *links_array, 
    int *links_count)
{    
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(links_array);
    BCMDNX_NULL_CHECK(links_count);
#ifdef BCM_PETRAB_SUPPORT
    if(SOC_IS_PETRAB(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_fabric_reachability_status_get is not supported in Soc_petra-B")));
    }
#endif

#ifdef BCM_ARAD_SUPPORT
    if(SOC_IS_ARAD(unit)){
        uint32 soc_rv;

        soc_rv = soc_dpp_fabric_reachability_status_get(unit, moduleid, links_max, links_array, links_count);  
        BCMDNX_IF_ERR_EXIT(soc_rv);
    }
#endif
    BCM_EXIT;
exit:     
    BCMDNX_FUNC_RETURN; 
}

/* test a credit request profile for conditions needed for moving to a different credit request mode */
STATIC int
_bcm_petra_fabric_credit_watchdog_test_profile(int unit, int profile, int cr_wd_mode)
{
    bcm_cosq_delay_tolerance_t profile_data;
    int rc = bcm_petra_cosq_delay_tolerance_level_get(unit, profile, &profile_data);
    BCMDNX_INIT_FUNC_DEFS;
    if (rc != BCM_E_UNAVAIL) {
        BCMDNX_IF_ERR_EXIT(rc);
        if (profile_data.credit_request_watchdog_delete_queue_thresh) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Can not enter the %s credit watchdog mode when a credit watchdog delete queue threshold is set.\n"
              "You may want to disable it for user defined credit request profile number %d (tolerance level %d)"),
              cr_wd_mode == BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE ? "common status message" : "aggressive status message",
              profile - (BCM_COSQ_DELAY_TOLERANCE_02 - 2), profile));
        } else if (cr_wd_mode == BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE &&
                   profile_data.credit_request_watchdog_status_msg_gen) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("Can not enter the common status message credit watchdog mode when a credit watchdog status message threshold is set.\n"
              "You may want to disable it for user defined credit request profile number %d (tolerance level %d)"),
              profile - (BCM_COSQ_DELAY_TOLERANCE_02 - 2), profile));
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

 /*
  * Credit watchdog:
  *  1. retransmit flow control messages for active queues that haven't get Credits from
  *     schedule after some time.
  *  2. free "stuck" queues after time threshold to keep DRAM resources.
  */

/*
 * Set queue range or enable/disable for credit watchdog
 * Set VOQ range
 */
int
_bcm_petra_fabric_credit_watchdog_range_set(int unit,
                      bcm_fabric_control_t control_type,
                      int queue_id,
                      int common_message_time) /* if >= 0, this is the new common message time, for the common message time mode */
{
#ifdef BCM_ARAD_SUPPORT
    int8 orig_crwd_mode = 0;
    uint32 orig_scan_time_ns = 0;
    int fail = 0;
#endif /* BCM_ARAD_SUPPORT */
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        SOC_TMC_ITM_CR_WD_INFO info, info2;
        int enabled, profile;
        int changed_mode = 0;
        if (control_type == bcmFabricWatchdogQueueEnable) {
            if (queue_id < BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_MIN || queue_id > BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_MAX) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid enabling number: %d, should be between %d - %d"), queue_id,
                  BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_MIN, BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_MAX));
            } else if (queue_id == BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE && !SOC_IS_ARADPLUS(unit)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("common FSM mode is only supported in 86660\n")));
            } else if (common_message_time >= 0 && (!IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit) ||
                       queue_id != CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("common message time can only be set in common message time mode")));
            }
        } else if (queue_id > DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit) || queue_id < 0) { /* Validate queue id */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid queue number: %d, is not between 0 - %d"), queue_id, DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit)));
        } else if (common_message_time >= 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("can only set common message time using bcmFabricWatchdogQueueEnable")));
        }
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_cr_wd_get, (unit, &info)));
        fail = 1; /* get credit watchdog configuration if error happens from here on */
        orig_crwd_mode = SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode;
        orig_scan_time_ns = SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano;

        /* The MBCM_DPP_DRIVER_CALL initializing the required variable */ 
        /* coverity[uninit_use:FALSE] */
        enabled = info.min_scan_cycle_period_micro;
        if (common_message_time >= 0) {
            if (common_message_time == 2 * ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS / 1000) {
                /* If over max scan time, use max scan time and mark mode to use two scans */
                queue_id = CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE + 1;
                common_message_time = ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS / 1000;
            }
            if (!enabled) { /* If changing common message time, do not change enabled or not */
                SET_CREDIT_WATCHDOG_MODE(unit, queue_id); /* update the stored mode */
                queue_id = BCM_FABRIC_WATCHDOG_QUEUE_DISABLE;
            }
        }

        if (control_type == bcmFabricWatchdogQueueEnable) {
            if (queue_id == BCM_FABRIC_WATCHDOG_QUEUE_DISABLE) {
                enabled = 0;
            } else {
                changed_mode = !IS_CREDIT_WATCHDOG_MODE(unit, queue_id);
                if (GET_CREDIT_WATCHDOG_MODE_EXACT(unit) == CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE + 1 &&
                    queue_id == CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE && common_message_time < 0) {
                    queue_id = CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE + 1; /* do not change common FSM sub mode if not requested to do so */
                }
                enabled = 1;
                    /* if moving to a different credit watchdog mode, check if allowed to move to the new mode */
                if (changed_mode &&
                   (queue_id != BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_NORMAL || IS_CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE(unit))) {
                    /* check that no profile with aging exists */
                    for (profile = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED; profile <= BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY; ++profile) {
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_fabric_credit_watchdog_test_profile(unit, profile, queue_id));
                    }
                    for (profile = BCM_COSQ_DELAY_TOLERANCE_02; profile <= BCM_COSQ_DELAY_TOLERANCE_14; ++profile) {
                        BCMDNX_IF_ERR_EXIT(_bcm_petra_fabric_credit_watchdog_test_profile(unit, profile, queue_id));
                    }
                }
                SET_CREDIT_WATCHDOG_MODE(unit, queue_id); /* update the stored mode */
            }
        } else if (control_type == bcmFabricWatchdogQueueMin) {
            info.bottom_queue = queue_id;
            if (info.top_queue < queue_id) {
                info.top_queue = queue_id;
            }
        } else if (control_type == bcmFabricWatchdogQueueMax) {
            info.top_queue = queue_id;
            if (info.bottom_queue > queue_id) {
                info.bottom_queue = queue_id;
            }
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Wrong control_type argument")));
        }

        if (enabled) {
            /* The MBCM_DPP_DRIVER_CALL initializing the required variable */ 
            /* coverity[uninit_use:FALSE] */
            uint32 queue_number = info.top_queue + 1 - info.bottom_queue;
            uint32 nof_ns_in_20_ticks, scan_time_ns;
            BCM_SAND_IF_ERR_EXIT(arad_ticks_to_time((unit), 20, TRUE, 1, &nof_ns_in_20_ticks));
            /* Get the default watchdog scan time for the mode (or the one given in common_message_time) and set it in SW */
            SOC_TMC_ITM_CR_WD_INFO_clear(&info2);
            info2.min_scan_cycle_period_micro = common_message_time >= 0 ? common_message_time :
              (changed_mode || !IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit) ? 0 :
               SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano / 1000);
            info2.top_queue = info2.bottom_queue = info2.max_flow_msg_gen_rate_nano = ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP;
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_cr_wd_set, (unit, &info2, &info2)));
            scan_time_ns = info2.min_scan_cycle_period_micro; /* exact scan time */
            info.min_scan_cycle_period_micro = SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano / 1000; /* rounded scan time in common FSM mode */

            /* Set the time in nanoseconds to scan each queue, may be later limited by the range of the hardware field.
             * time = max{0.5*full_scan/nof_queues, full_scan/nof_queues - 20*clock_cycle}
             * We try to be precise (not have scans longer than the declared scan time, and not have burstiness.
             */
            info.max_flow_msg_gen_rate_nano = (scan_time_ns / 2) / queue_number;
            queue_number = scan_time_ns / queue_number;
            if (queue_number > nof_ns_in_20_ticks) {
                queue_number -= nof_ns_in_20_ticks;
                if (info.max_flow_msg_gen_rate_nano < queue_number ) {
                    info.max_flow_msg_gen_rate_nano = queue_number;
                }
            }

            /* if moving to a different credit watchdog mode, adjust existing profiles */
            if (changed_mode && !IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit)) {
                bcm_cosq_delay_tolerance_t profile_data;
                int status_msg_threshold = queue_id == BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_FAST_STATUS_MESSAGE ?
                  SOC_TMC_ITM_CREDIT_WATCHDOG_AGGRESSIVE_WD_STATUS_MSG_MESSAGE_THRESHOLD :
                  SOC_TMC_ITM_CREDIT_WATCHDOG_NORMAL_STATUS_MSG_THRESHOLD;
                /* Change existing credit watchdog status message thresholds to the default for the new mode */
                for (profile = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED; profile <= BCM_COSQ_DELAY_TOLERANCE_200G_LOW_DELAY; ++profile) {
                    BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_delay_tolerance_level_get(unit, profile, &profile_data));
                    if (profile_data.credit_request_watchdog_status_msg_gen && 
                        profile_data.credit_request_watchdog_status_msg_gen != status_msg_threshold) {
                        profile_data.credit_request_watchdog_status_msg_gen = status_msg_threshold;
                        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_delay_tolerance_level_set(unit, profile, &profile_data));
                    }
                }
                for (profile = BCM_COSQ_DELAY_TOLERANCE_02; profile <= BCM_COSQ_DELAY_TOLERANCE_14; ++profile) {
                    int rc = bcm_petra_cosq_delay_tolerance_level_get(unit, profile, &profile_data);
                    if (rc == BCM_E_UNAVAIL) {
                        continue;
                    }
                    BCMDNX_IF_ERR_EXIT(rc);
                    if (profile_data.credit_request_watchdog_status_msg_gen && 
                        profile_data.credit_request_watchdog_status_msg_gen != status_msg_threshold) {
                        profile_data.credit_request_watchdog_status_msg_gen = status_msg_threshold;
                        BCMDNX_IF_ERR_EXIT(bcm_petra_cosq_delay_tolerance_level_set(unit, profile, &profile_data));
                    }
                }
            }

        } else {
            if (common_message_time >= 0) {
                /* Set the scan time for the common mode in SW, and not in HW */
                SOC_TMC_ITM_CR_WD_INFO_clear(&info2);
                info2.min_scan_cycle_period_micro = common_message_time;
                info2.top_queue = info2.bottom_queue = info2.max_flow_msg_gen_rate_nano = ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP;
                BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_cr_wd_set, (unit, &info2, &info2)));
            }
            info.min_scan_cycle_period_micro = 0;
            if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit)) {
                uint32 exp = 0; /* find the (negative) exponent of the scan time compared to the max scan time */
                for (; exp <= ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES &&
                     SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano != ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS << exp;
                     ++exp);
                if  (exp > ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid scan time for common message time mode: %uns"),
                      SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano));
                }
                /* store data the (disabled) credit watchdog mode and scan time in HW for warm boot */
                info.max_flow_msg_gen_rate_nano = SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode + exp;
            } else {
                /* store data the (disabled) credit watchdog mode in HW for warm boot */
                info.max_flow_msg_gen_rate_nano = SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode;
            }
        }

        /* If the scan time of the common message time mode was changed, change the delete time exponents to match new scan time */
        if (common_message_time >= 0 && !changed_mode && orig_scan_time_ns !=
           SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano) {
            uint32 bigger, smaller, *shift;
            info2.min_scan_cycle_period_micro = info2.max_flow_msg_gen_rate_nano = 0;
            info2.top_queue = info2.bottom_queue = ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP;
            if (SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano > orig_scan_time_ns) {
                bigger = SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano;
                smaller = orig_scan_time_ns;
                shift = &info2.min_scan_cycle_period_micro;
            } else {
                bigger = orig_scan_time_ns;
                smaller = SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano;
                shift = &info2.max_flow_msg_gen_rate_nano;
            }
            for (*shift = 1; smaller << *shift != bigger; ++*shift) {
                if (*shift >= ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Can not adjust delete times moving from scan time %u to scan time %u"),
                      (unsigned)orig_scan_time_ns, (unsigned)SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano));
                }
            }

            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_cr_wd_set, (unit, &info2, &info2)));
        }

        /* Change the credit watchdog scan configuration */
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_itm_cr_wd_set, (unit, &info, &info2)));
        fail = 0;

    } else
#endif
    {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported for this device")));
    }

exit:
#ifdef BCM_ARAD_SUPPORT
    if (fail) { /* If failed, restore the original credit watchdog mode */
        SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode = orig_crwd_mode;
        SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano = orig_scan_time_ns;
    }
#endif /* BCM_ARAD_SUPPORT */
    BCMDNX_FUNC_RETURN;
}

/*
 * Get queue range or enable/disable for credit watchdog
 * Set VOQ range
 */
STATIC int
_bcm_petra_fabric_credit_watchdog_range_get(int unit,
                      bcm_fabric_control_t control_type,
                      int* queue_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(queue_id);       

#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        SOC_TMC_ITM_CR_WD_INFO info;

        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_itm_cr_wd_get,(unit, &info)));
        if (control_type == bcmFabricWatchdogQueueEnable) {
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            *queue_id = (info.min_scan_cycle_period_micro) ? 
              GET_CREDIT_WATCHDOG_MODE(unit) :
              BCM_FABRIC_WATCHDOG_QUEUE_DISABLE;
        } else if (control_type == bcmFabricWatchdogQueueMin) {
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            *queue_id = info.bottom_queue;
        } else if (control_type == bcmFabricWatchdogQueueMax) {
            /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
            /* coverity[uninit_use:FALSE] */
            *queue_id = info.top_queue;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Wrong control_type argument")));
        }
    } else
#endif
    {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not supported for this device")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_priority_set(
    int unit,
    uint32 flags, 
    bcm_cos_t ingress_pri, 
    bcm_color_t color, 
    int fabric_priority)
{
    BCMDNX_INIT_FUNC_DEFS;
    _BCM_DPP_SWITCH_API_START(unit);

    /*arguments check*/
    BCM_DPP_UNIT_CHECK(unit);

    if (ingress_pri < 0 || ingress_pri >= DPP_DEVICE_COSQ_ING_NOF_TC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress_pri parameter %d"), ingress_pri));
    }
    if (color < 0 || color >= DPP_DEVICE_COSQ_ING_NOF_DP) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress_dp parameter %d"), color));
    }

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) &&  (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags includes both  BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY and BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY")));
    }

    unit = (unit);
    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) == 0 &&  (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY) == 0) {
        /*set both hp and lp*/
        uint32 flags_high, flags_low;

        flags_high = flags | BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY;
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_set,(unit, ingress_pri, color, flags_high, fabric_priority)));

        flags_low = flags | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY;
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_set,(unit, ingress_pri, color, flags_low, fabric_priority)));
    } else {
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_set,(unit, ingress_pri, color, flags, fabric_priority)));
    }

exit:
    _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_priority_get(
    int unit,
    uint32 flags, 
    bcm_cos_t ingress_pri, 
    bcm_color_t color,  
    int *fabric_priority)
{
    BCMDNX_INIT_FUNC_DEFS;

    /*arguments check*/
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_NULL_CHECK(fabric_priority);

    if (ingress_pri < 0 || ingress_pri >= DPP_DEVICE_COSQ_ING_NOF_TC) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress_pri parameter %d"), ingress_pri));
    }
    if (color < 0 || color >= DPP_DEVICE_COSQ_ING_NOF_DP) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress_dp parameter %d"), color));
    }

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) &&  (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags includes both  BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY and BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY")));
    }

    unit = (unit);
    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) == 0 &&  (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY) == 0) {
        /*set both hp and lp*/
        uint32 flags_high, flags_low;
        int fabric_priority_low = 0, fabric_priority_high = 0;

        flags_high = flags | BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY;
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_get,(unit, ingress_pri, color, flags_high, &fabric_priority_low)));

        flags_low = flags | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY;
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_get,(unit, ingress_pri, color, flags_low, &fabric_priority_high)));

        if (fabric_priority_low != fabric_priority_high) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("fabric priority is different for QUEUE_PRIORITY_LOW and QUEUE_PRIORITY_HIGH. use flags BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY or BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY.")));
        }
        *fabric_priority = fabric_priority_high;
    } else {
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_priority_get,(unit, ingress_pri, color, flags, fabric_priority)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_thresholds_set(
    int unit, 
    int fifo_type, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    int i;
    uint32 soc_sand_rc;
    BCMDNX_INIT_FUNC_DEFS;

    if (fifo_type == -1)
    {
        /*special functionality - no fifo_type required*/

        for(i = 0 ; i<count; i++)
        {
            switch(type[i])
            {
                /*
                 * GCI Backoff related thresholds configuraion
                 */
                case bcmFabricLinkTxGciLvl1FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0, value[i]));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
                case bcmFabricLinkTxGciLvl2FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1, value[i]));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
                case bcmFabricLinkTxGciLvl3FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2, value[i]));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;

                /*
                 * GCI Leaky bucket thresholds
                 */
               case bcmFabricLinkGciLeakyBucket1Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket2Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket3Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket4Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;

               case bcmFabricLinkGciLeakyBucket1Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket2Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket3Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket4Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_set, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL, value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;


                /*
                 * Link level thresholds configuration
                 */
                case bcmFabricLinkRxFifoLLFC:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_llfc_threshold_set,(unit, value[i]));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;

                /*
                 * Local RCI thresholds
                 */

               case bcmFabricLinkRciFC:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_config_set, (unit, SOC_TMC_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH,value[i]));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;


                default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Threshold type not supported: %d"), type[i]));
                    break;
                
            }
         }
        
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Fifo type not supported: %d"), fifo_type));
    }



exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_thresholds_get(
    int unit, 
    int fifo_type, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    int i;
    uint32 soc_sand_rc;
    BCMDNX_INIT_FUNC_DEFS;

    if (fifo_type == -1)
    {
        /*special functionality - no fifo_type required*/


        for(i = 0 ; i<count; i++)
        {
            switch(type[i])
            {
                /*
                 * GCI Backoff related thresholds configuraion
                 */
                case bcmFabricLinkTxGciLvl1FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_0, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
                case bcmFabricLinkTxGciLvl2FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_1, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
                case bcmFabricLinkTxGciLvl3FC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_BACKOFF_TH_2, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;
               /*
                * GCI Leaky bucket thresholds
                */
               case bcmFabricLinkGciLeakyBucket1Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_CONGESTION_TH, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket2Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_CONGESTION_TH, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket3Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_CONGESTION_TH, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket4Congestion:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_CONGESTION_TH, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;

               case bcmFabricLinkGciLeakyBucket1Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_1_FULL, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket2Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_2_FULL, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket3Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_3_FULL, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;
               case bcmFabricLinkGciLeakyBucket4Full:
                   soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_gci_config_get, (unit, SOC_TMC_FABRIC_GCI_CONFIG_TYPE_LB_4_FULL, &(value[i])));
                   BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                   break;

                /*
                 * Link level thresholds configuration
                 */
                case bcmFabricLinkRxFifoLLFC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_fabric_llfc_threshold_get,(unit, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;

                /* 
                 *Local RCI thresholds
                 */
                case bcmFabricLinkRciFC:
                    soc_sand_rc = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_config_get, (unit, SOC_TMC_FABRIC_RCI_CONFIG_TYPE_LOCAL_RX_TH, &(value[i])));
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rc);
                    break;

               default:
                    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Threshold type not supported: %d"), type[i]));
                    break;
            }
         }
        
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Fifo type not supported: %d"), fifo_type));
    }



exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_fabric_link_thresholds_pipe_set(
    int unit, 
    int fifo_type, 
    bcm_fabric_pipe_t pipe, 
    uint32 flags, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    soc_dpp_fabric_pipe_t soc_pipe;
    int i;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(type);
    BCMDNX_NULL_CHECK(value);

    if (pipe == bcmFabricPipe0) {
        soc_pipe = socDppFabricPipe0;
    } else if (pipe == bcmFabricPipe1) {
        soc_pipe = socDppFabricPipe1;
    } else if (pipe == bcmFabricPipe2) {
        soc_pipe = socDppFabricPipe2;
    } else {
        soc_pipe = socDppFabricPipeAll;
    }

    for (i = 0; i < count; ++i) {
        switch (type[i]) {
        case bcmFabricLinkRxFifoLLFC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_set, 
                                                (unit, soc_pipe, socDppFabricLinkRxFifoLLFC, value[i])));
                break;
        case bcmFabricLinkRxRciLvl1FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_set, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl1FC, value[i])));
                break;
        case bcmFabricLinkRxRciLvl2FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_set, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl2FC, value[i])));
                break;
        case bcmFabricLinkRxRciLvl3FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_set, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl3FC, value[i])));
                break;
        default:
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_BCM_MSG("threshold type is not supported")));
                break;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_fabric_link_thresholds_pipe_get(
    int unit, 
    int fifo_type, 
    bcm_fabric_pipe_t pipe, 
    uint32 flags, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    int i;
    soc_dpp_fabric_pipe_t soc_pipe;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(type);
    BCMDNX_NULL_CHECK(value);

    if (pipe == bcmFabricPipe0) {
        soc_pipe = socDppFabricPipe0;
    } else if (pipe == bcmFabricPipe1) {
        soc_pipe = socDppFabricPipe1;
    } else if (pipe == bcmFabricPipe2) {
        soc_pipe = socDppFabricPipe2;
    } else {
        soc_pipe = socDppFabricPipeAll;
    }

    for (i = 0; i < count; ++i) {
        switch (type[i]) {
        case bcmFabricLinkRxFifoLLFC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_get, 
                                                (unit, soc_pipe, socDppFabricLinkRxFifoLLFC, value+i)));
                break;
        case bcmFabricLinkRxRciLvl1FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_get, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl1FC, value+i)));
                break;
        case bcmFabricLinkRxRciLvl2FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_get, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl2FC, value+i)));
                break;
        case bcmFabricLinkRxRciLvl3FC:
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_thresholds_pipe_get, 
                                                (unit, soc_pipe, socDppFabricLinkRxRciLvl3FC, value+i)));
                break;
        default:
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_BCM_MSG("threshold type is not supported")));
                break;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * 
 * Function:
 *      bcm_fabric_route_rx
 * Purpose:
 *      Receive Source routed data cells
 * Parameters:
 *      unit                (IN) Unit number.
 *      flags               (IN) flags
 *      data_out_max_size   (IN) max "data_out" size
 *      data_out            (OUT) the received data.
 *      data_out_size       (OUT) "data_out" actual size.
 */
int 
bcm_petra_fabric_route_rx(
    int unit, 
    uint32 flags, 
    uint32 data_out_max_size, 
    uint32 *data_out, 
    uint32 *data_out_size)
{
    uint32 data_out_size_in_bytes;
    BCMDNX_INIT_FUNC_DEFS;

    /*Verify*/
    BCMDNX_NULL_CHECK(data_out);
    BCMDNX_NULL_CHECK(data_out_size);
    if (data_out_max_size == 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("buffer is too small")));
    }

    
    BCMDNX_IF_ERR_EXIT(soc_dpp_fabric_receive_sr_cell(unit, flags, (data_out_max_size * 4), &data_out_size_in_bytes, data_out));
    if(data_out_size_in_bytes % 4)
    {
      /* There is another data in no more then 3 bytes, which don't fits whole 32-bits unit */
      *data_out_size = (data_out_size_in_bytes / 4) + 1;
    }
    else
    {
      *data_out_size = data_out_size_in_bytes / 4;
    }

exit: 
    BCMDNX_FUNC_RETURN;
}
/*
 * 
 * Function:
 *      bcm_fabric_route_tx
 * Purpose:
 *      Send Source routed data cells
 * Parameters:
 *      unit         (IN) Unit number.
 *      flags        (IN) flags
 *      route        (IN) Specify the path for the generated cells.
 *      data_in_size (IN) "data_in" size (uint32 units).
 *      data_in      (IN) The data to send.
 */
int 
bcm_petra_fabric_route_tx(
    int unit, 
    uint32 flags, 
    bcm_fabric_route_t *route, 
    uint32 data_in_size, 
    uint32 *data_in)
{
    soc_fabric_inband_route_t soc_route;
    BCMDNX_INIT_FUNC_DEFS;

    /*verify*/
    BCMDNX_NULL_CHECK(route);
    BCMDNX_NULL_CHECK(route->hop_ids);
    BCMDNX_NULL_CHECK(data_in);
    if (data_in_size == 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("no data to send")));
    }


    soc_route.number_of_hops = route->number_of_hops;
    if (soc_route.number_of_hops > FABRIC_CELL_NOF_LINKS_IN_PATH_LINKS)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("number_of_hops is too long\n")));
    }
    sal_memcpy(soc_route.link_ids ,route->hop_ids, sizeof(int) * soc_route.number_of_hops);
    
    BCMDNX_IF_ERR_EXIT(soc_dpp_fabric_send_sr_cell(unit, flags, &soc_route, sizeof(uint32) * data_in_size, data_in));

exit: 
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_pcp_dest_mode_config_set(
    int 				unit, 
    uint32  			flags, 
    bcm_module_t 			modid, 
    bcm_fabric_pcp_mode_config_t  	*pcp_config)
{
    ARAD_INIT_FABRIC *fabric = NULL;
    BCMDNX_INIT_FUNC_DEFS;
    
    fabric = &(SOC_DPP_CONFIG(unit)->arad->init.fabric);   
    if (fabric->fabric_pcp_enable) {
        if (pcp_config == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("pcp_config argument is invalid")));
        }
        if (!(BCM_FABRIC_MODID_IS_VALID(unit, modid))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("modid argument is invalid")));
        }
        if (!(BCM_FABRIC_PCP_MODE_IS_VALID(pcp_config->pcp_mode))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("pcp_mode is invalid")));
        }

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_pcp_dest_mode_config_set, 
                                                 (unit, flags, modid, pcp_config->pcp_mode)));

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,(_BSL_BCM_MSG("fabric_pcp is disabled")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_fabric_pcp_dest_mode_config_get(
    int 				unit, 
    uint32  			flags, 
    bcm_module_t 			modid, 
    bcm_fabric_pcp_mode_config_t  	*pcp_config)
{
    ARAD_INIT_FABRIC *fabric = NULL;
    uint32 pcp_mode = 0;
    BCMDNX_INIT_FUNC_DEFS;
    
    fabric = &(SOC_DPP_CONFIG(unit)->arad->init.fabric); 
    if (fabric->fabric_pcp_enable) {
        if (pcp_config == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("pcp_config argument is invalid")));
        }
        if (!(BCM_FABRIC_MODID_IS_VALID(unit, modid))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("modid argument is invalid")));
        }

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_pcp_dest_mode_config_get, 
                                                 (unit, flags, modid, &pcp_mode)));
        pcp_config->pcp_mode = pcp_mode;
         
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("fabric_pcp is disabled")));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_destination_link_min_set(
      int unit,
      uint32 flags,
      bcm_module_t module_id,
      int num_of_links)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (!(BCM_FABRIC_NUM_OF_LINKS_IS_VALID(unit, num_of_links))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("number of links is invalid")));
    }
    if (SOC_DPP_IS_MESH(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("minimum number of links configuration not supported in mesh")));
    }
    if (flags & BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_minimal_links_all_reachable_set, 
                                                 (unit, num_of_links)));
    } else {
        if ((module_id != BCM_MODID_ALL) && !(BCM_FABRIC_MODID_IS_VALID(unit, module_id))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("module id is invalid")));
        }
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_minimal_links_to_dest_set, 
                                                     (unit, module_id, num_of_links)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_destination_link_min_get(
      int unit,
      uint32 flags,
      bcm_module_t module_id,
      int* num_of_links)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(num_of_links);

    if (SOC_DPP_IS_MESH(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("minimum number of links configuration not supported in mesh")));
    }

    if (flags & BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_minimal_links_all_reachable_get, 
                                                 (unit, num_of_links)));
    } else {
        if ((module_id != BCM_MODID_ALL) && !(BCM_FABRIC_MODID_IS_VALID(unit, module_id))) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("module id is invalid")));
        }
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_minimal_links_to_dest_get, 
                                                 (unit, module_id, num_of_links)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_topology_set(
    int unit, 
    bcm_module_t destination, 
    int links_count, 
    bcm_port_t *links_array)
{
    int nof_fabric_links, is_mesh, i;
    bcm_module_t local_dest_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(links_array);

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    /* validate mesh mode */
    if (!is_mesh) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_modid_group_get is only valid in mesh mode"))); 
    }

    /* validate links_count */
    if(links_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("links_count should be 0 and up"))); 
    }
    nof_fabric_links = SOC_DPP_DEFS_GET(unit, nof_fabric_links);
    if(links_count >= nof_fabric_links) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("links_count value too big"))); 
    }
    /* validate links in links_array are valid */
    for (i = 0; i < links_count; ++i) {
        if (!BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), links_array[i])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("link %d is invalid"), links_array[i])); 
        }
    }
    /* validate destination */
    if (!BCM_FABRIC_MODID_IS_GROUP(destination)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("destination should be a group"))); 
    }
    local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(destination);
    /*validate local_dest fits configuration*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_local_dest_id_verify, 
                                                 (unit, local_dest_id)));
    if (links_count == 0) { 
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_topology_unset, 
                                                     (unit, local_dest_id)));
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_topology_set, 
                                                     (unit, local_dest_id, links_count, links_array)));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_link_topology_get(
    int unit, 
    bcm_module_t destination, 
    int max_links_count, 
    int *links_count, 
    bcm_port_t *links_array)
{
    int is_mesh;
    bcm_module_t local_dest_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(links_array);
    BCMDNX_NULL_CHECK(links_count);

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    /* validate mesh mode */
    if (!is_mesh) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_modid_group_get is only valid in mesh mode"))); 
    }

    /* validate max_links_count */
    if(max_links_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal max_links_count"))); 
    }
    /*validate destination */
    if (!BCM_FABRIC_MODID_IS_GROUP(destination)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("destination should be a group"))); 
    }
    local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(destination);
    /*validate local_dest fits configuration*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_local_dest_id_verify, 
                                                 (unit, local_dest_id)));

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_link_topology_get, 
                                                 (unit, local_dest_id, max_links_count, links_count, links_array)));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_multicast_set(
    int unit, 
    bcm_multicast_t mc_id, 
    uint32 flags, 
    uint32 destid_count, 
    bcm_module_t *destid_array)
{
    int i;
    int is_mesh_mc;
    uint32 my_core0_fap_id, my_core1_fap_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(destid_array);

    /* validate mc_id */
    if (!BCM_FABRIC_MC_ID_IS_VALID(unit, mc_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal mc_id"))); 
    }
    /* validate destid count */
    if(destid_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal destid_count"))); 
    }
    if(destid_count > SOC_DPP_FABRIC_MAX_MESH_REPLICATION) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("too many destinations"))); 
    }
    /* validate destinatins in dest_array */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mgmt_system_fap_id_get,(unit, &my_core0_fap_id)));
    my_core1_fap_id = my_core0_fap_id + 1;
    for (i = 0; i < destid_count; ++i) {
        if ((!SOC_DPP_FABRIC_MODID_IS_GROUP(destid_array[i]))
            && (destid_array[i] != my_core0_fap_id)
            && (destid_array[i] != my_core1_fap_id)) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_BCM_MSG("invalid dest_id %d"), destid_array[i]));
        }
    }
    /* vaidate MESH_MC mode */
    is_mesh_mc = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable;

    if (is_mesh_mc) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_multicast_set, 
                                                 (unit, mc_id, destid_count, destid_array)));
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_multicast_set is supported in MESH_MC mode only")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_multicast_get(
    int unit, 
    bcm_multicast_t mc_id, 
    uint32 flags, 
    int destid_count_max, 
    int *destid_count, 
    bcm_module_t *destid_array)
{
    int is_mesh_mc;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(destid_array);
    BCMDNX_NULL_CHECK(destid_count);

    /* validate mc_id (up to 64k)*/
    if (!BCM_FABRIC_MC_ID_IS_VALID(unit, mc_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal mc_id"))); 
    }
    /* validate destid_count_max */
    if(destid_count_max <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal destid_count_max"))); 
    }
    /* validate MESH_MC mode */
    is_mesh_mc = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable;

    if (is_mesh_mc) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_multicast_get, 
                                                 (unit, mc_id, destid_count_max, destid_count, destid_array)));
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_multicast_get is supported in MESH_MC mode only")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_fabric_modid_group_set(
    int unit, 
    bcm_module_t group, 
    int modid_count, 
    bcm_module_t *modid_array)
{
    int is_mesh, i;
    bcm_module_t local_dest_id;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(modid_array);

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    /* validate mesh mode */
    if (!is_mesh) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_modid_group_get is only valid in mesh mode"))); 
    }

    /* validate modid_count */
    if(modid_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("modid_count should be greater than 0"))); 
    }
    if (modid_count > SOC_DPP_MAX_NOF_FAP_ID_MAPPED_TO_DEST_LIMIT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("only 2 fAPs can be mapped to a single destination"))); 
    }
    /* validate faps in array are valid */
    for (i = 0; i < modid_count; ++i) {
        if (!BCM_FABRIC_MODID_IS_VALID(unit, modid_array[i])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fap id %d invalid"), modid_array[i])); 
        }
    }
    /* validate group */
    if (!BCM_FABRIC_MODID_IS_GROUP(group)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("destination should be a group"))); 
    }
    local_dest_id = BCM_FABRIC_GROUP_MODID_UNSET(group);

    /*validate local_dest fits configuration*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_local_dest_id_verify, 
                                                 (unit, local_dest_id)));

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_modid_group_set, 
                                                 (unit, local_dest_id, modid_count, modid_array)));
exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_fabric_modid_group_get(
    int unit, 
    bcm_module_t group, 
    int modid_max_count, 
    bcm_module_t *modid_array, 
    int *modid_count)
{
    int is_mesh;
    soc_module_t local_dest;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(modid_array);
    BCMDNX_NULL_CHECK(modid_count);

    is_mesh = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == SOC_TMC_FABRIC_CONNECT_MODE_MESH);
    /* validate mesh mode */
    if (!is_mesh) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_fabric_modid_group_get is only valid in mesh mode"))); 
    }

    /* validate modid_max_count */
    if(modid_max_count <= 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal modid_max_count"))); 
    }
    
    /* validate group */
    if (!BCM_FABRIC_MODID_IS_GROUP(group)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("destination should be a group"))); 
    }
    local_dest = BCM_FABRIC_GROUP_MODID_UNSET(group);

    /*validate local_dest fits configuration*/
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_local_dest_id_verify, 
                                                 (unit, local_dest)));

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_modid_group_get, 
                                                 (unit, local_dest, modid_max_count, modid_array, modid_count)));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_rci_config_set(
      int unit,
      bcm_fabric_rci_config_t rci_config)
{
    int i;
    soc_dpp_fabric_rci_config_t soc_rci_config;
    BCMDNX_INIT_FUNC_DEFS;

    /*validate struct fields are valid and fill soc-layer struct*/
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_LEVELS; ++i) {
        if (!BCM_FABRIC_RCI_THRESHOLD_IS_VALID(rci_config.rci_core_level_thresholds[i])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("threshold %d is invalid"), rci_config.rci_core_level_thresholds[i]));
        }
        if (!BCM_FABRIC_RCI_THRESHOLD_IS_VALID(rci_config.rci_device_level_thresholds[i])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("threshold %d is invalid"), rci_config.rci_device_level_thresholds[i]));
        }
        soc_rci_config.soc_dpp_fabric_rci_core_level_thresholds[i] = rci_config.rci_core_level_thresholds[i];
        soc_rci_config.soc_dpp_fabric_rci_device_level_thresholds[i] = rci_config.rci_device_level_thresholds[i];
    }
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_SEVERITIES; ++i) {
        if (!BCM_FABRIC_RCI_SEVERITY_FACTOR_IS_VALID(rci_config.rci_severity_factors[i])) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("severity factor %d is invalid"), rci_config.rci_severity_factors[i]));
        }
        soc_rci_config.soc_dpp_fabric_rci_severity_factors[i] = rci_config.rci_severity_factors[i];
    }

    if (!BCM_FABRIC_RCI_HIGH_SCORE_IS_VALID(rci_config.rci_high_score_fabric_rx_queue)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("rci_high_score_fabric_rx_queue is invalid")));
    }
    if (!BCM_FABRIC_RCI_HIGH_SCORE_IS_VALID(rci_config.rci_high_score_fabric_rx_local_queue)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("rci_high_score_fabric_rx_local_queue is invalid")));
    }
    if (!BCM_FABRIC_RCI_HIGH_SCORE_IS_VALID(rci_config.rci_high_score_congested_links)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("rci_high_score_congested_links is invalid")));
    }
    if (!BCM_FABRIC_RCI_CONGESTED_LINKS_THRESHOLD_IS_VALID(rci_config.rci_threshold_num_of_congested_links)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("rci_threshold_num_of_congested_links is invalid")));
    }
    
    soc_rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_queue = rci_config.rci_high_score_fabric_rx_queue;
    soc_rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_local_queue = rci_config.rci_high_score_fabric_rx_local_queue;
    soc_rci_config.soc_dpp_fabric_rci_threshold_num_of_congested_links = rci_config.rci_threshold_num_of_congested_links;
    soc_rci_config.soc_dpp_fabric_rci_high_score_congested_links = rci_config.rci_high_score_congested_links;

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_thresholds_config_set, (unit, soc_rci_config)));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_fabric_rci_config_get(
      int unit,
      bcm_fabric_rci_config_t* rci_config)
{
    int i;
    soc_dpp_fabric_rci_config_t soc_rci_config;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(rci_config);

    /*initialize struct to invalid values*/
    bcm_fabric_rci_config_t_init(rci_config);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_rci_thresholds_config_get, (unit, &soc_rci_config)));

    /*fill rci_config struct with fields*/
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_LEVELS; ++i) {
        rci_config->rci_core_level_thresholds[i] = soc_rci_config.soc_dpp_fabric_rci_core_level_thresholds[i];
        rci_config->rci_device_level_thresholds[i]= soc_rci_config.soc_dpp_fabric_rci_device_level_thresholds[i];
    }
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_SEVERITIES; ++i) {
        rci_config->rci_severity_factors[i] = soc_rci_config.soc_dpp_fabric_rci_severity_factors[i];
    }
    rci_config->rci_high_score_fabric_rx_queue = soc_rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_queue;
    rci_config->rci_high_score_fabric_rx_local_queue = soc_rci_config.soc_dpp_fabric_rci_high_score_fabric_rx_local_queue;
    rci_config->rci_threshold_num_of_congested_links = soc_rci_config.soc_dpp_fabric_rci_threshold_num_of_congested_links;
    rci_config->rci_high_score_congested_links = soc_rci_config.soc_dpp_fabric_rci_high_score_congested_links;

exit:
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
