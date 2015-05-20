/*
 * $Id: fe1600_property.c,v 1.8.64.2 Broadcom SDK $
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
 * SOC FE1600 PROPERTY
 */
#include <shared/bsl.h>
#include <soc/dfe/fe1600/fe1600_property.h>
#include <soc/dfe/cmn/dfe_property.h>
#include <soc/dfe/cmn/dfe_drv.h>



#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_STAT

soc_dfe_property_info_t soc_fe1600_property_info[]={

	{
		spn_MIIM_TIMEOUT_USEC,
		NULL,
		2000,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_MIIM_INTR_ENABLE,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_FABRIC_DEVICE_MODE,
		"SINGLE_STAGE_FE2",
		-1,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_STRING,
	},
	{
		spn_IS_DUAL_MODE,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_SYSTEM_IS_VCS_128_IN_SYSTEM,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_SYSTEM_IS_DUAL_MODE_IN_SYSTEM,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_SYSTEM_IS_SINGLE_MODE_IN_SYSTEM,
		NULL,
		1,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
    {
		spn_SYSTEM_CONTAINS_MULTIPLE_PIPE_DEVICE,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_SYSTEM_IS_FE600_IN_SYSTEM,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_FABRIC_MERGE_CELLS,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},

	{
		spn_SERDES_MIXED_RATE_ENABLE,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_FABRIC_MULTICAST_MODE,
		"DIRECT",
		-1,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_STRING
	},
	{
		spn_FABRIC_LOAD_BALANCING_MODE,
		"NORMAL_LOAD_BALANCE",
		-1,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_STRING
	},
    {
		spn_FABRIC_CELL_FORMAT,
		"VSC256",
		FALSE,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_STRING
	},
	{
		spn_FABRIC_TDM_FRAGMENT,
		NULL,
		0x181,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_FABRIC_TDM_OVER_PRIMARY_PIPE,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_BIST_ENABLE,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_FABRIC_OPTIMIZE_PARTIAL_LINKS,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_VCS128_UNICAST_PRIORITY,
		NULL,
		-1 , /* get default value from soc_properties_init */
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_FABRIC_MAC_BUCKET_FILL_RATE,
		NULL,
		DFE_FABRIC_MAC_BUCKET_FILL_RATE_DEFAULT,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_SCHAN_TIMEOUT_USEC,
		NULL,
		-1, /* get default value from soc_properties_init */
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_SCHAN_INTR_ENABLE,
		NULL,
		0,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
	{
		spn_SYSTEM_REF_CORE_CLOCK,
		NULL,
		300,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
    {
		spn_CORE_CLOCK_SPEED,
		NULL,
		533,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
    {
		spn_REPEATER_LINK_DEST,
		NULL,
		-1,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_INT
	},
    {
		spn_BACKPLANE_SERDES_ENCODING,
		"KR_FEC",
		-1,
		SOC_DFE_PROPERTY_DEFAULT_TYPE_STRING,
	},
	{
		NULL,
		NULL,
		-1,
		-1,
	}

};

void
soc_fe1600_soc_properties_array_get(int unit,soc_dfe_property_info_t** soc_dfe_property_info_array )
{
	*soc_dfe_property_info_array=soc_fe1600_property_info;
}
#undef _ERR_MSG_MODULE_NAME
