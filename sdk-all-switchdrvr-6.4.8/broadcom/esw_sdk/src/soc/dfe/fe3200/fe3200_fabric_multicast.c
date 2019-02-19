/*
 * $Id: fe3200_fabric_multicast.c,v 1.7.48.1 Broadcom SDK $
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
 * SOC FE3200 FABRIC MULTICAST
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/mem.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <shared/bitop.h>

#if defined(BCM_88950_A0)

#include <soc/dfe/fe3200/fe3200_fabric_multicast.h>

#define _SOC_FE3200_FABRIC_MULTICAST_TABLE_SIZE (64 * 1024)

STATIC soc_dfe_multicast_read_range_info_t _soc_fe3200_fabric_multicast_read_range_info_standard[] = {
    {
        0,
        _SOC_FE3200_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        1
    },
    {
        _SOC_FE3200_FABRIC_MULTICAST_TABLE_SIZE,
        2 * _SOC_FE3200_FABRIC_MULTICAST_TABLE_SIZE - 1,
        1,
        1
    },
    {
        2 * _SOC_FE3200_FABRIC_MULTICAST_TABLE_SIZE,
        3 * _SOC_FE3200_FABRIC_MULTICAST_TABLE_SIZE - 1,
        2,
        1
    }
};
STATIC soc_dfe_multicast_read_range_info_t _soc_fe3200_fabric_multicast_read_range_info_128k_half[] = {
    {
        0,
        _SOC_FE3200_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        1
    },
    {
        _SOC_FE3200_FABRIC_MULTICAST_TABLE_SIZE,
        2 * _SOC_FE3200_FABRIC_MULTICAST_TABLE_SIZE - 1,
        0,
        0
    }
};

soc_error_t
soc_fe3200_fabric_multicast_multi_read_info_get(int unit, soc_dfe_multicast_read_range_info_t **info, int *info_size)
{
    SOCDNX_INIT_FUNC_DEFS;

    /*get mc table read info*/
    switch (SOC_DFE_CONFIG(unit).fe_mc_id_range)
    {
       case soc_dfe_multicast_table_mode_128k_half:
           *info = _soc_fe3200_fabric_multicast_read_range_info_128k_half;
           *info_size = sizeof(_soc_fe3200_fabric_multicast_read_range_info_128k_half) / sizeof(soc_dfe_multicast_read_range_info_t);
           break;
       default:
           *info = _soc_fe3200_fabric_multicast_read_range_info_standard;
           *info_size = sizeof(_soc_fe3200_fabric_multicast_read_range_info_standard) / sizeof(soc_dfe_multicast_read_range_info_t);
           break;
    }

    SOCDNX_FUNC_RETURN;
}
 
/* 
 *write range of multicast groups - the data can be sperated per instance of RTP_MCTm 
 */
soc_error_t
soc_fe3200_fabric_multicast_multi_write_range(int unit, int mem_flags, soc_multicast_t group_min, soc_multicast_t group_max, uint32 *entry_array)
{
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = soc_mem_array_write_range(unit, mem_flags, RTP_MULTI_CAST_TABLE_UPDATEm, 0, MEM_BLOCK_ALL, group_min, group_max, entry_array);
    SOCDNX_IF_ERR_EXIT(rv);


exit:
    SOCDNX_FUNC_RETURN;
}

#endif
