/*
 * $Id: dcmn_cells_buffer.c,v 1.4 Broadcom SDK $
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
 * SOC FE1600 CELLS BUFFER
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#if defined(BCM_DFE_SUPPORT) || defined (BCM_PETRA_SUPPORT)

#include <soc/defs.h>
#include <soc/error.h>
#include <shared/cyclic_buffer.h>
#include <soc/dcmn/dcmn_cells_buffer.h>


soc_error_t 
dcmn_cells_buffer_create(int unit, cyclic_buffer_t* cells_buffer, int max_buffered_cells)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);

    rc = cyclic_buffer_create(unit, cells_buffer, sizeof(vsc256_sr_cell_t), max_buffered_cells, "cells_buffer->cells"); 
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_destroy(int unit, cyclic_buffer_t* cells_buffer)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);

    rc = cyclic_buffer_destroy(unit, cells_buffer);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_add(int unit, cyclic_buffer_t* buffer, const vsc256_sr_cell_t* new_cell)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_NULL_CHECK(buffer);
    SOCDNX_NULL_CHECK(new_cell);

    rc = cyclic_buffer_add(unit, buffer, (const void*)new_cell);
    SOCDNX_IF_ERR_EXIT(rc);
    
exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_get(int unit, cyclic_buffer_t* cells_buffer, vsc256_sr_cell_t* received_cell)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);
    SOCDNX_NULL_CHECK(received_cell);

    rc = cyclic_buffer_get(unit, cells_buffer, (void*)received_cell);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_is_empty(int unit, const cyclic_buffer_t* cells_buffer, int* is_empty)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);
    SOCDNX_NULL_CHECK(is_empty);

    rc = cyclic_buffer_is_empty(unit, cells_buffer, is_empty);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_is_full(int unit, const cyclic_buffer_t* cells_buffer, int* is_full)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);
    SOCDNX_NULL_CHECK(is_full);

    rc = cyclic_buffer_is_full(unit, cells_buffer, is_full);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}

soc_error_t 
dcmn_cells_buffer_cells_count(int unit, const cyclic_buffer_t* cells_buffer, int* count)
{
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(cells_buffer);
    SOCDNX_NULL_CHECK(count);

    rc = cyclic_buffer_cells_count(unit, cells_buffer, count);
    SOCDNX_IF_ERR_EXIT(rc);

exit:
    SOCDNX_FUNC_RETURN; 
}


#endif /*defined(BCM_DFE_SUPPORT) || defined BCM_PETRA_SUPPORT */

#undef _ERR_MSG_MODULE_NAME

