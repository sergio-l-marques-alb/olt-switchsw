/*
 * $Id: instr.c,v 1.26 Broadcom SDK $
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
 * Tomahawk Instrumentation API
 */

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/switch.h>
#include <bcm_int/esw/instrumentation.h>
#include <bcm/l2.h>


#include <soc/l2u.h>
#include <soc/mem.h>

#include <appl/diag/system.h>

/* when visibility packet is sent
   following datastructure will be set */
_bcm_switch_pkt_trace_port_info_t pkt_trace_port_info;

/*
 * Function:
 *      bcm_th_pkt_trace_hw_reset
 * Purpose:
 *      clean PTR_RESULTS_BUFFER_IVP/ISW1/ISW2 registers
 *      call this functio before sending a 
 *      visibilty packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_hw_reset(int unit)
{
    uint32                          result_index;
    ptr_results_buffer_ivp_entry_t  ivp_entry;
    ptr_results_buffer_isw1_entry_t isw1_entry;
    ptr_results_buffer_isw2_entry_t isw2_entry;

    memset(&ivp_entry, 0, sizeof(ptr_results_buffer_ivp_entry_t));
    memset(&isw1_entry, 0, sizeof(ptr_results_buffer_isw1_entry_t));
    memset(&isw2_entry, 0, sizeof(ptr_results_buffer_isw2_entry_t));
     
    for (result_index = 0; result_index < TH_PTR_RESULTS_IVP_MAX_INDEX; result_index++) {  
        WRITE_PTR_RESULTS_BUFFER_IVPm(unit, MEM_BLOCK_ALL, result_index, &ivp_entry);
    }

    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW1_MAX_INDEX; result_index++) {  
        WRITE_PTR_RESULTS_BUFFER_ISW1m(unit, MEM_BLOCK_ALL, result_index,&isw1_entry);
    }

    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW2_MAX_INDEX; result_index++) {  
        WRITE_PTR_RESULTS_BUFFER_ISW2m(unit, MEM_BLOCK_ALL, result_index,&isw2_entry);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_src_port_set 
 * Purpose:
 *      set the source port of 
 *      visiblity packet
 *      this must be called before visibilty packet 
 *      is sent to set the pipe to read the resuls 
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_src_port_set(int unit, uint32 logical_src_port) 
{
    soc_info_t  *si;
    si = &SOC_INFO(unit);

    pkt_trace_port_info.pkt_trace_src_logical_port = logical_src_port;
    pkt_trace_port_info.pkt_trace_src_pipe =  
                                      si->port_pipe[logical_src_port];

    return 0;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_src_port_get 
 * Purpose:
 *      get the source port of 
 *      visiblity packet
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_th_pkt_trace_src_port_get(int unit) 
{
    return pkt_trace_port_info.pkt_trace_src_logical_port;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_info_get
 * Purpose:
 *      read results ptr buffer for
 *      visibility packet process and
 *      store into pkt_trace_info
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_info_get(int unit,
                              bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    ptr_results_buffer_ivp_entry_t        ivp_entry;
    ptr_results_buffer_isw1_entry_t       isw1_entry; 
    ptr_results_buffer_isw2_entry_t       isw2_entry;
    
    uint8    result_index = 0, pipe = 0;
    uint32  *dw_first, *dw_second;

    soc_mem_t mem;
    pipe =  pkt_trace_port_info.pkt_trace_src_pipe;

    for (result_index = 0; result_index < TH_PTR_RESULTS_IVP_MAX_INDEX; result_index++) {

        if (SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_IVPm) == NULL) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_IVPm)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, result_index, &ivp_entry)); 

        dw_first = (uint32*)&ivp_entry;
        dw_second = dw_first+1; 

        sal_memcpy(&(pkt_trace_info->ivp_raw_data[result_index *2]),
                   dw_first, sizeof(uint32));
        sal_memcpy(&(pkt_trace_info->ivp_raw_data[(result_index *2) + 1]),
                   dw_second, sizeof(uint32));
    }/*end of ivp_parsing*/

    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW1_MAX_INDEX; result_index++) {

        if (SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW1m) == NULL) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW1m)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }
 
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, result_index, &isw1_entry)); 

        dw_first = (uint32*)&isw1_entry;
        dw_second = dw_first+1; 

        sal_memcpy(&(pkt_trace_info->isw1_raw_data[result_index *2]),
                   dw_first, sizeof(uint32));
        sal_memcpy(&(pkt_trace_info->isw1_raw_data[(result_index *2) + 1]),
                   dw_second, sizeof(uint32));

    }/*end of isw1_parsing*/

    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW2_MAX_INDEX; result_index++) {
        if (SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW2m) == NULL) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW2m)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, result_index, &isw2_entry)); 

        dw_first = (uint32*)&isw2_entry;
        dw_second = dw_first+1; 

        sal_memcpy(&(pkt_trace_info->isw2_raw_data[result_index *2]),
                   dw_first, sizeof(uint32));
        sal_memcpy(&(pkt_trace_info->isw2_raw_data[(result_index *2) + 1]),
                   dw_second, sizeof(uint32));

    }/*end of isw2_parsing*/

    return BCM_E_NONE;
}


