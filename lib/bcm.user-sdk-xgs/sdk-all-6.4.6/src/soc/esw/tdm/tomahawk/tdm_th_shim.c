/*
 * $Id: tdm_th_shim.c.$
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
 * $All Rights Reserved.$
 *
 * TDM chip to core API shim layer
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_chip_th_shim__check_ethernet_d
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
**/
tdm_mod_t*
tdm_chip_th_shim__check_ethernet_d( int port, enum port_speed_e speed[TH_NUM_EXT_PORTS], enum port_state_e state[TH_NUM_EXT_PORTS], int **tsc, int traffic[TH_NUM_PM_MOD] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}
	
	_tdm_s->_chip_data.soc_pkg.speed=TDM_ALLOC(TH_NUM_EXT_PORTS*sizeof(int), "port speed list");
	for (idx1=0; idx1<TH_NUM_EXT_PORTS; idx1++) {
		_tdm_s->_chip_data.soc_pkg.speed[idx1] = speed[idx1];
	}
	_tdm_s->_chip_data.soc_pkg.state=TDM_ALLOC(TH_NUM_EXT_PORTS*sizeof(int), "port state list");
	for (idx1=0; idx1<TH_NUM_EXT_PORTS; idx1++) {
		_tdm_s->_chip_data.soc_pkg.state[idx1] = state[idx1];
	}
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = TH_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = TH_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<TH_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TH_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	/* _tdm_s->_chip_data.soc_pkg.soc_vars.th.pm_encap_type=sal_alloc(TH_NUM_PM_MOD*sizeof(int *), "traffic encap"); */	
	for (idx1=0; idx1<TH_NUM_PM_MOD; idx1++) {
		_tdm_s->_chip_data.soc_pkg.soc_vars.th.pm_encap_type[idx1] = traffic[idx1];
	}	
	
	_tdm_s->_core_data.vars_pkg.port = port;
	return _tdm_s;
	
}


/**
@name: tdm_chip_th_shim__core_vbs_scheduler_ovs
@param:

Passthru function for oversub scheduling request from TD2/TD2+
**/
int
tdm_chip_th_shim__core_vbs_scheduler_ovs( tdm_mod_t *_tdm )
{
	return tdm_core_vbs_scheduler_ovs(_tdm);
}
