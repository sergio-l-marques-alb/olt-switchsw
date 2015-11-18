/*
* $Id: dpp_fabric_cell.c,v 1.4 Broadcom SDK $
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
* SOC DFE FABRIC CELL
*/

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/dpp_fabric_cell.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_fabric_cell.h>

static
void clear_data_out_strcute (soc_dcmn_captured_control_cell_t* data_out)
{
    data_out->dest_device = -1;
    data_out->source_device = -1;
    data_out->dest_port = -1;
    data_out->src_queue_num = -1;
    data_out->dest_queue_num = -1;
    data_out->sub_flow_id = -1;
    data_out->flow_id = -1;
    data_out->reachability_bitmap = -1;
    data_out->base_index = -1;
    data_out->source_link_number = -1;
}

int 
soc_dpp_cell_filter_set(int unit, uint32 array_size, soc_dcmn_filter_type_t* filter_type_arr, uint32* filter_type_val) 
{

#ifdef BCM_ARAD_SUPPORT
    int rc;
#endif /* BCM_ARAD_SUPPORT */

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    if(!SOC_IS_ARAD(unit)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }
    SOCDNX_NULL_CHECK(filter_type_arr);
    SOCDNX_NULL_CHECK(filter_type_val);

#ifdef BCM_ARAD_SUPPORT
    
    rc = soc_arad_cell_filter_set(unit, array_size, filter_type_arr, filter_type_val);
    SOCDNX_IF_ERR_EXIT(rc);
#endif /* BCM_ARAD_SUPPORT */

exit:
    
    SOCDNX_FUNC_RETURN;

}

int 
soc_dpp_cell_filter_clear(int unit)
{

#ifdef BCM_ARAD_SUPPORT
    int rc;
#endif /* BCM_ARAD_SUPPORT */

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    if(!SOC_IS_ARAD(unit)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    

#ifdef BCM_ARAD_SUPPORT
     rc = soc_arad_cell_filter_clear(unit);
    SOCDNX_IF_ERR_EXIT(rc);
#endif /* BCM_ARAD_SUPPORT */

exit:
    
    SOCDNX_FUNC_RETURN;
}

int 
soc_dpp_cell_filter_receive(int unit, dcmn_captured_cell_t* data_out)
{

#ifdef BCM_ARAD_SUPPORT
    int rc;
#endif /* BCM_ARAD_SUPPORT */

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    if(!SOC_IS_ARAD(unit)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    SOCDNX_NULL_CHECK(data_out);

    

#ifdef BCM_ARAD_SUPPORT
    rc = soc_arad_cell_filter_receive(unit, data_out);
    SOCDNX_IF_ERR_EXIT(rc);
#endif /* BCM_ARAD_SUPPORT */

exit:
    
    SOCDNX_FUNC_RETURN;

}

/*
* Function:
*      soc_dpp_control_cell_filter_set
* Purpose:
*      Set filter in order to capture a control cell (credit, flow status, reachability)
* Parameters:
*      unit                            - (IN)  Unit number
*      cell_type                       - (IN)  Control cell type (credit, flow status or reachability cell
*      array_size                      - (IN)  Number of filters
*      control_cell_filter_type_arr    - (IN)  The filters array
*      filter_type_val                 - (OUT) The filter that we should us to capture the control cell
*  
* Returns:
*      SOC_E_xxx
*/
int soc_dpp_control_cell_filter_set(int unit, soc_dcmn_control_cell_types_t cell_type, uint32 array_size, soc_dcmn_control_cell_filter_type_t* control_cell_filter_type_arr, uint32* filter_type_val) 
{

#ifdef BCM_ARAD_SUPPORT
    int rc;
#endif /* BCM_ARAD_SUPPORT */

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    if(!SOC_IS_ARAD(unit)){
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    SOCDNX_NULL_CHECK(control_cell_filter_type_arr);
    SOCDNX_NULL_CHECK(filter_type_val);

    

#ifdef BCM_ARAD_SUPPORT
    rc = soc_arad_control_cell_filter_set(unit, cell_type, array_size, control_cell_filter_type_arr, filter_type_val);
    SOCDNX_IF_ERR_EXIT(rc);
#endif /* BCM_ARAD_SUPPORT */

exit:
    
    SOCDNX_FUNC_RETURN;
}

/*
* Function:
*      soc_dpp_control_cell_filter_clear
* Purpose:
*      Clear the control cell of filter and mask
* Parameters:
*      unit                            - (IN)  Unit number
*  
* Returns:
*      SOC_E_xxx
*/
int 
soc_dpp_control_cell_filter_clear(int unit)
{
 
#ifdef BCM_ARAD_SUPPORT
   int rc;
#endif /* BCM_ARAD_SUPPORT */

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    if (!SOC_IS_ARAD(unit))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    

#ifdef BCM_ARAD_SUPPORT
    rc = soc_arad_control_cell_filter_clear(unit);
    SOCDNX_IF_ERR_EXIT(rc);
#endif /* BCM_ARAD_SUPPORT */

exit:
    
    SOCDNX_FUNC_RETURN;
}

/*
* Function:
*      soc_dpp_control_cell_filter_receive
* Purpose:
*      Recieve the content of the capture control cell. The capture cell can be one of the following:
*      1. Credit
*      2. FSM (Flow Status Message)
*      3. Reachability cell
*  
* Parameters:
*      unit                            - (IN)  Unit number
*      data_out                       - (OUT) The parsing of the control cell that match the filter
*  
* Comments: 
*      # The data_out strcture includes valid bit that indicate if we were able to capture a cell
*      For each control cell different fields are relevant:
*      1. Credit - dest_queue_num, sub_flow_id, flow_id, source_device, dest_device
*      2. FSM - dest_port, src_queue_num, flow_id, source_device, dest_device
*      3. Reachability cell - reachability_bitmap, base_index, source_link_number, source_device
*  
* Returns:
*      SOC_E_xxx
*/

int 
soc_dpp_control_cell_filter_receive(int unit, soc_dcmn_captured_control_cell_t* data_out)
{

#ifdef BCM_ARAD_SUPPORT
    int rc;
#endif /* BCM_ARAD_SUPPORT */

	SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_UNIT_VALID(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    if (!SOC_IS_ARAD(unit)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNIT, (_BSL_SOCDNX_MSG("Invalid unit")));
    }

    SOCDNX_NULL_CHECK(data_out);

    /* Clear the strcure, if the output is "-1" this field is not avaialble on this specific control cell type */
    clear_data_out_strcute(data_out);

    

#ifdef BCM_ARAD_SUPPORT
    rc = soc_arad_control_cell_filter_receive(unit, data_out);
    SOCDNX_IF_ERR_EXIT(rc);
#endif /* BCM_ARAD_SUPPORT */

exit:
    
    SOCDNX_FUNC_RETURN;

}

#undef _ERR_MSG_MODULE_NAME
