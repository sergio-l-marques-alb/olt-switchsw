/*
 * $Id: fe3200_fabric_topology.c,v 1.16.24.1 Broadcom SDK $
 *
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * SOC FE3200 FABRIC TOPOLOGY
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC 
#include <shared/bsl.h>

#include <soc/dfe/cmn/dfe_drv.h>

#include <soc/dfe/fe3200/fe3200_fabric_topology.h>
#include <soc/dcmn/error.h>
#include <soc/defs.h>
#include <soc/error.h>

/*
 * Function:
 *      soc_fe3200_fabric_topology_reachabilty_mask_set
 * Purpose:
 *      Isolate / Acrivate fabric
 * Parameters:
 *      unit - (IN) Unit number.
 *      active_links - (IN)
 *      val  - (IN) Link Isolation Status
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe3200_fabric_topology_reachability_mask_set(int unit, soc_pbmp_t active_links, soc_dcmn_isolation_status_t val)
{
    soc_port_t port;
    soc_reg_above_64_val_t reg_val;
    uint32 reg32_val = 0;
    SOCDNX_INIT_FUNC_DEFS;
    
    if (val == soc_dcmn_isolation_status_isolated)
    {
        /*Get automatic RTP update*/
        SOCDNX_IF_ERR_EXIT(READ_RTP_RESERVED_10r(unit, &reg32_val));

        /*Disable automatic update of RTP*/    
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit, 0x0));
    }

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_val));
    SOC_PBMP_ITER(active_links, port) {
        if (val == soc_dcmn_isolation_status_isolated) {
            SHR_BITCLR(reg_val, port);
        } else {
            SHR_BITSET(reg_val, port);
        }
    }
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_val));
      
    if (val == soc_dcmn_isolation_status_isolated)
    {
        /*Wait to be sure all other devices stopped sending traffic*/
        sal_usleep(30000);

        /*Renable automatic update of RTP*/
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit, reg32_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
/*
 * Function:
 *      soc_fe3200_fabric_topology_nof_links_to_min_nof_links_default
 * Purpose:
 *      Retrive link default value for min number of links
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      nof_links      - (IN)
 *      min_nof_links - (OUT)
 * Returns:
 *      SOC_E_xxx
 */
int
soc_fe3200_fabric_topology_nof_links_to_min_nof_links_default(int unit, int nof_links, int *min_nof_links)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    *min_nof_links = 0;

    SOCDNX_FUNC_RETURN; 
}

/*
 * Function:
 *      soc_fe3200_fabric_topology_min_nof_links_set
 * Purpose:
 *      Setting min number of links
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      min_nof_links - (IN)
 * Returns:
 *      SOC_E_xxx
 */
int
soc_fe3200_fabric_topology_min_nof_links_set(int unit, int min_nof_links)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    if (min_nof_links != 0)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("FE3200 do not support min number of links feature")));
    }
exit:
    SOCDNX_FUNC_RETURN; 
}


/*
 * Function:
 *      soc_fe3200_fabric_topology_min_nof_links_get
 * Purpose:
 *      Getting min number of links
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      min_nof_links - (OUT)
 * Returns:
 *      SOC_E_xxx
 */
int
soc_fe3200_fabric_topology_min_nof_links_get(int unit, int *min_nof_links)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    *min_nof_links = 0;

    SOCDNX_FUNC_RETURN; 
}
/*
 * Function:
 *      soc_fe3200_fabric_topology_isolate_set
 * Purpose:
 *      Isolate / Acrivate fabric
 * Parameters:
 *      unit - (IN) Unit number.
 *      val  - (IN) Link Isolation Status
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe3200_fabric_topology_isolate_set(int unit, soc_dcmn_isolation_status_t val)
{
    uint32 reg_val32;

    SOCDNX_INIT_FUNC_DEFS;

    if(soc_dcmn_isolation_status_active == val) {

        sal_usleep(2000);

        SOCDNX_IF_ERR_EXIT(soc_fe3200_fabric_topology_mesh_topology_reset(unit));
                   
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_topology_rmgr_set, (unit, 1)));
    } else {
        /*Get automatic RTP update*/
        SOCDNX_IF_ERR_EXIT(READ_RTP_RESERVED_10r(unit,&reg_val32));

        /*Set automatic RTP update*/
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit,0x0));
               
        /*Disable reachability messages*/
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_topology_rmgr_set, (unit, 0)));

        /*Wait to be sure all other devices stopped sending traffic*/
        sal_usleep(30000);

        /*Re-enable automatic update of RTP*/
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit,reg_val32));        
    } 
      
    sal_usleep(20000);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function:
 *      soc_fe3200_fabric_topology_mesh_topology_reset
 * Purpose:
 *      Rest MESH TOPOLOGY block
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SOC_E_xxx
 */
soc_error_t 
soc_fe3200_fabric_topology_mesh_topology_reset(int unit)
{
    uint32 reg_val32;
    int ccs_index,nof_instances_ccs;
    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_ccs = SOC_DFE_DEFS_GET(unit, nof_instances_ccs);

    /*MESH_TOPOLOGY reset sequence*/
    for (ccs_index = 0; ccs_index < nof_instances_ccs ; ccs_index++)
    {
        SOCDNX_IF_ERR_EXIT(READ_CCS_FILTERED_CELLS_CONFIGURATIONSr(unit,ccs_index,&reg_val32));
        soc_reg_field_set(unit,CCS_FILTERED_CELLS_CONFIGURATIONSr,&reg_val32,FILTER_CELLSf,1);
        SOCDNX_IF_ERR_EXIT(WRITE_CCS_FILTERED_CELLS_CONFIGURATIONSr(unit,ccs_index,reg_val32));
    }

    sal_usleep(1000);

    /*reset MESH_TOPOLOGY*/ 
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));
    /* Enable back */
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));
    

    for (ccs_index = 0; ccs_index < nof_instances_ccs ; ccs_index++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_CCS_FILTERED_CELLS_CONFIGURATIONSr(unit,ccs_index,&reg_val32));
        soc_reg_field_set(unit,CCS_FILTERED_CELLS_CONFIGURATIONSr,&reg_val32,FILTER_CELLSf,0);
        SOCDNX_IF_ERR_EXIT(WRITE_CCS_FILTERED_CELLS_CONFIGURATIONSr(unit,ccs_index,reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
