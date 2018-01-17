/*
 * $Id: fe3200_multicast.c,v 1.6.34.1 Broadcom SDK $
 *
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
 * SOC FE3200 MULTICAST
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MCAST

#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dfe/fe3200/fe3200_multicast.h>

#include <soc/dfe/cmn/dfe_drv.h>

soc_error_t
soc_fe3200_multicast_mode_get(int unit, soc_dfe_multicast_table_mode_t* multicast_mode)
{
	SOCDNX_INIT_FUNC_DEFS;
	*multicast_mode = soc_dfe_multicast_table_mode_128k_half; /* default */
	switch(SOC_DFE_CONFIG(unit).fe_mc_id_range)
	{
		case soc_dfe_multicast_table_mode_64k:
		case soc_dfe_multicast_table_mode_128k:
		case soc_dfe_multicast_table_mode_128k_half:
			*multicast_mode =  SOC_DFE_CONFIG(unit).fe_mc_id_range;
			break;
		default:
			SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Wrong mc_table_mode value %d"), SOC_DFE_CONFIG(unit).fe_mc_id_range));
	}
exit:
	SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_multicast_table_size_get(int unit, uint32* mc_table_size)
{
	soc_dfe_multicast_table_mode_t multicast_mode;
	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_IF_ERR_EXIT(soc_fe3200_multicast_mode_get(unit, &multicast_mode));
	switch (multicast_mode)
	{
		case soc_dfe_multicast_table_mode_64k:
			*mc_table_size = 64*1024;
			break;
		case soc_dfe_multicast_table_mode_128k:
			*mc_table_size = 128*1024;
			break;
		case soc_dfe_multicast_table_mode_128k_half:
			*mc_table_size = 128*1024;
			break;
		default:
			SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("wrong mc_table_mode value %d"),SOC_DFE_CONFIG(unit).fe_mc_id_range));
            break;
	}
exit:
	SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_multicast_table_entry_size_get(int unit, uint32* entry_size)
{
	soc_dfe_multicast_table_mode_t multicast_mode;
	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_IF_ERR_EXIT(soc_fe3200_multicast_mode_get(unit, &multicast_mode));
	switch (multicast_mode)
	{
		case soc_dfe_multicast_table_mode_64k:
			*entry_size = 144;
			break;
		case soc_dfe_multicast_table_mode_128k:
			*entry_size = 144;
			break;
		case soc_dfe_multicast_table_mode_128k_half:
			*entry_size = 72;
			break;
		default:
			SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("wrong mc_table_mode value %d"),SOC_DFE_CONFIG(unit).fe_mc_id_range));
	}
exit:
	SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
