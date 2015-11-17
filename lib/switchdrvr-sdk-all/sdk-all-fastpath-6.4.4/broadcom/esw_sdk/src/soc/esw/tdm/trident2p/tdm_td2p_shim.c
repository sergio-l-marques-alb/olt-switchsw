/*
 * $Id: tdm_td2p_shim.c.$
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
@name: tdm_chip_td2p_shim_core
@param:

Downward abstraction layer between TDM.4 and TDM.5 API core data
**/
tdm_chip_legacy_t
tdm_chip_td2p_shim_core( tdm_mod_t *_tdm )
{
	int idx1, idx2;
	tdm_chip_legacy_t _tdm_td2p;
	
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_td2p.pmap[idx1][idx2] = _tdm->_chip_data.soc_pkg.pmap[idx1][idx2];
		}
	}
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		_tdm_td2p.tdm_globals.speed[idx1] = _tdm->_chip_data.soc_pkg.speed[idx1];
	}
	_tdm_td2p.tdm_globals.clk_freq = _tdm->_chip_data.soc_pkg.clk_freq;
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		_tdm_td2p.tdm_globals.port_rates_array[idx1] = _tdm->_chip_data.soc_pkg.state[idx1];
	}
	/* Realign port state array to old specification */
	for (idx1=0; idx1<(TD2P_NUM_EXT_PORTS-1); idx1++) {
		_tdm_td2p.tdm_globals.port_rates_array[idx1] = _tdm_td2p.tdm_globals.port_rates_array[idx1+1];
	}
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		if (idx1%TD2P_NUM_PM_LNS==0) {
			_tdm_td2p.tdm_globals.pm_encap_type[idx1/TD2P_NUM_PM_LNS] = (_tdm->_chip_data.soc_pkg.state[idx1]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[idx1]==PORT_STATE__OVERSUB_HG)?(999):(998);
		}
	}
	
	return _tdm_td2p;
	
}


/**
@name: tdm_chip_td2p_shim__ing_wrap
@param:

Downward abstraction layer between TDM.4 and TDM.5 API ingress scheduler
**/
tdm_chip_legacy_t
tdm_chip_td2p_shim__ing_wrap( tdm_mod_t *_tdm )
{
	tdm_chip_legacy_t _tdm_td2p = tdm_chip_td2p_shim_core(_tdm);

	TDM_MSET(_tdm_td2p.tdm_pgw.pgw_tdm_tbl_x0,TD2P_NUM_EXT_PORTS,TD2P_LR_LLS_LEN);
	TDM_MSET(_tdm_td2p.tdm_pgw.ovs_tdm_tbl_x0,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_td2p.tdm_pgw.ovs_spacing_x0,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pgw.pgw_tdm_tbl_x1,TD2P_NUM_EXT_PORTS,TD2P_LR_LLS_LEN);
	TDM_MSET(_tdm_td2p.tdm_pgw.ovs_tdm_tbl_x1,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_td2p.tdm_pgw.ovs_spacing_x1,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pgw.pgw_tdm_tbl_y0,TD2P_NUM_EXT_PORTS,TD2P_LR_LLS_LEN);
	TDM_MSET(_tdm_td2p.tdm_pgw.ovs_tdm_tbl_y0,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_td2p.tdm_pgw.ovs_spacing_y0,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pgw.pgw_tdm_tbl_y1,TD2P_NUM_EXT_PORTS,TD2P_LR_LLS_LEN);
	TDM_MSET(_tdm_td2p.tdm_pgw.ovs_tdm_tbl_y1,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	TDM_MSET(_tdm_td2p.tdm_pgw.ovs_spacing_y1,TD2P_NUM_EXT_PORTS,TD2P_OS_LLS_GRP_LEN);	
	
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_pipe_main,TD2P_NUM_EXT_PORTS,TD2P_LR_VBS_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_0,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_1,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_2,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_3,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_4,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_5,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_6,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_7,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_pipe_main,TD2P_NUM_EXT_PORTS,TD2P_LR_VBS_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_0,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_1,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_2,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_3,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_4,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_5,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_6,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_7,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);	
	
	return _tdm_td2p;
	
}


/**
@name: tdm_chip_td2p_shim__egr_wrap
@param:

Downward abstraction layer between TDM.4 and TDM.5 API egress scheduler
**/
tdm_chip_legacy_t
tdm_chip_td2p_shim__egr_wrap( tdm_mod_t *_tdm )
{
	tdm_chip_legacy_t _tdm_td2p = tdm_chip_td2p_shim_core(_tdm);

	TDM_COPY(_tdm_td2p.tdm_pgw.pgw_tdm_tbl_x0,_tdm->_chip_data.cal_0.cal_main,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm_td2p.tdm_pgw.ovs_tdm_tbl_x0,_tdm->_chip_data.cal_0.cal_grp[0],sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm_td2p.tdm_pgw.pgw_tdm_tbl_x1,_tdm->_chip_data.cal_1.cal_main,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm_td2p.tdm_pgw.ovs_tdm_tbl_x1,_tdm->_chip_data.cal_1.cal_grp[0],sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm_td2p.tdm_pgw.pgw_tdm_tbl_y0,_tdm->_chip_data.cal_2.cal_main,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm_td2p.tdm_pgw.ovs_tdm_tbl_y0,_tdm->_chip_data.cal_2.cal_grp[0],sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm_td2p.tdm_pgw.pgw_tdm_tbl_y1,_tdm->_chip_data.cal_3.cal_main,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm_td2p.tdm_pgw.ovs_tdm_tbl_y1,_tdm->_chip_data.cal_3.cal_grp[0],sizeof(int)*TD2P_OS_LLS_GRP_LEN);	
	
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_pipe_main,TD2P_NUM_EXT_PORTS,TD2P_LR_VBS_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_0,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_1,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_2,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_3,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_4,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_5,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_6,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_x.tdm_ovs_grp_7,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_pipe_main,TD2P_NUM_EXT_PORTS,TD2P_LR_VBS_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_0,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_1,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_2,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_3,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_4,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_5,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_6,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);
	TDM_MSET(_tdm_td2p.tdm_pipe_table_y.tdm_ovs_grp_7,TD2P_NUM_EXT_PORTS,TD2P_OS_VBS_GRP_LEN);	
	
	
	return _tdm_td2p;
	
}	


/**
@name: tdm_chip_td2p_shim__which_tsc
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
Only returns enough of TDM.5 style struct to drive scan functions, do not use as class
**/
tdm_mod_t*
tdm_chip_td2p_shim__which_tsc( int port, int tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}

	_tdm_s->_chip_data.soc_pkg.num_ext_ports = TD2P_NUM_EXT_PORTS;
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	
	_tdm_s->_core_data.vars_pkg.port = port;
	
	return _tdm_s;
	
}


/**
@name: tdm_chip_td2p_shim__check_ethernet
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
**/
tdm_mod_t*
tdm_chip_td2p_shim__check_ethernet( int port, enum port_speed_e speed[TD2P_NUM_EXT_PORTS], int tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS], int traffic[TD2P_NUM_PM_MOD] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}
	
	_tdm_s->_chip_data.soc_pkg.speed=TDM_ALLOC(TD2P_NUM_EXT_PORTS*sizeof(int *), "port speed list");
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		_tdm_s->_chip_data.soc_pkg.speed[idx1] = speed[idx1];
	}
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	/* _tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type=sal_alloc(TD2P_NUM_PM_MOD*sizeof(int *), "traffic encap"); */	
	for (idx1=0; idx1<TD2P_NUM_PM_MOD; idx1++) {
		_tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[idx1] = traffic[idx1];
	}
	
	_tdm_s->_core_data.vars_pkg.port = port;
	return _tdm_s;
	
}


/**
@name: tdm_chip_td2p_shim__check_ethernet
@param:

Upward abstraction layer between TDM.4 and TDM.5 API
**/
tdm_mod_t*
tdm_chip_td2p_shim__check_ethernet_d( int port, enum port_speed_e speed[TD2P_NUM_EXT_PORTS], int **tsc, int traffic[TD2P_NUM_PM_MOD] )
{
	int idx1, idx2;
	tdm_mod_t *_tdm_s;
	
	_tdm_s = TDM_ALLOC(sizeof(tdm_mod_t),"TDM shim allocation");
	if (!_tdm_s) {
		return NULL;
	}
	
	_tdm_s->_chip_data.soc_pkg.speed=TDM_ALLOC(TD2P_NUM_EXT_PORTS*sizeof(int *), "port speed list");
	for (idx1=0; idx1<TD2P_NUM_EXT_PORTS; idx1++) {
		_tdm_s->_chip_data.soc_pkg.speed[idx1] = speed[idx1];
	}
	_tdm_s->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PHY_PM;
	_tdm_s->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm_s->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (idx1=0; idx1<(_tdm_s->_chip_data.soc_pkg.pmap_num_modules); idx1++) {
		_tdm_s->_chip_data.soc_pkg.pmap[idx1]=(int *) TDM_ALLOC((_tdm_s->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm_s->_chip_data.soc_pkg.pmap[idx1],(_tdm_s->_chip_data.soc_pkg.num_ext_ports),_tdm_s->_chip_data.soc_pkg.pmap_num_lanes);
	}
	for (idx1=0; idx1<TD2P_NUM_PHY_PM; idx1++) {
		for (idx2=0; idx2<TD2P_NUM_PM_LNS; idx2++) {
			_tdm_s->_chip_data.soc_pkg.pmap[idx1][idx2] = tsc[idx1][idx2];
		}
	}
	/* _tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type=sal_alloc(TD2P_NUM_PM_MOD*sizeof(int *), "traffic encap"); */	
	for (idx1=0; idx1<TD2P_NUM_PM_MOD; idx1++) {
		_tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[idx1] = traffic[idx1];
	}	
	
	_tdm_s->_core_data.vars_pkg.port = port;
	return _tdm_s;
	
}


/**
@name: tdm_chip_td2p_shim__core_vbs_scheduler_ovs
@param:

Passthru function for oversub scheduling request from TD2/TD2+
**/
int
tdm_chip_td2p_shim__core_vbs_scheduler_ovs( tdm_mod_t *_tdm )
{
	/* TD2/TD2+ doesn't use 25G and 50G */
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5=0;
	_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6=0;
	
	return tdm_core_vbs_scheduler_ovs(_tdm);
}
