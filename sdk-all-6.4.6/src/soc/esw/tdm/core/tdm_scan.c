/*
 * $Id: tdm_parse.c.$
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
 * All Rights Reserved.$
 *
 * TDM core parsing functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_find_pm
@param:

Returns the TSC to which the input port belongs given pointer to transcribed pmap
**/
int
tdm_find_pm( tdm_mod_t *_tdm )
{
	int i, j, which=_tdm->_chip_data.soc_pkg.num_ext_ports;
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_modules; i++) {
		for (j=0; j<_tdm->_chip_data.soc_pkg.pmap_num_lanes; j++) {
			if (_tdm->_chip_data.soc_pkg.pmap[i][j]==_tdm->_core_data.vars_pkg.port) {
				which=i;
			}
		}
		if (which!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
			break;
		}
	}
	
	return which;
	
}


/**
@name: tdm_type_chk
@param:

Bubble sorts port module mapping slice and returns # of transitions
**/
int
tdm_type_chk( tdm_mod_t *_tdm )
{
	int i, j, id=_tdm->_chip_data.soc_pkg.num_ext_ports, cnt=1,
		tsc_arr[PM_SORT_STACK_SIZE],
		tsc_inst=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
		
	TDM_COPY(tsc_arr,_tdm->_chip_data.soc_pkg.pmap[tsc_inst],sizeof(int)*_tdm->_chip_data.soc_pkg.pmap_num_lanes);
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		for (j=0; j<_tdm->_chip_data.soc_pkg.pmap_num_lanes-i; j++) {
			if ((j+1)<_tdm->_chip_data.soc_pkg.pmap_num_lanes) {
				if (tsc_arr[j] > tsc_arr[j+1]) {
					id=tsc_arr[j];
					tsc_arr[j]=tsc_arr[j+1];
					tsc_arr[j+1]=id;
				}
			}
		}
	}
	for (i=1; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		if (tsc_arr[i]!=_tdm->_chip_data.soc_pkg.num_ext_ports && tsc_arr[i]!=tsc_arr[i-1]) {
			cnt++;
		}
	}
	
	return cnt;

}


/**
@name: tdm_find_fastest_port
@param:

Returns the fastest port number from within a tsc
**/
int
tdm_find_fastest_port( tdm_mod_t *_tdm )
{
	int i, port=_tdm->_chip_data.soc_pkg.num_ext_ports, tsc_id=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
	unsigned int spd=SPEED_0;
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		if (_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {	
			if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]]>spd) {
				port=_tdm->_chip_data.soc_pkg.pmap[tsc_id][i];
				spd=_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]];
			}
		}
	}
	
	return port;
	
}


/**
@name: tdm_find_fastest_spd
@param:

Returns the speed of the fastest lane from within a tsc
**/
int
tdm_find_fastest_spd( tdm_mod_t *_tdm )
{
	int i, tsc_id=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
	unsigned int spd=SPEED_0;
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		if (_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
			if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]]>spd) {
				spd=_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]];
			}
		}
	}
	
	return spd;	
}


/**
@name: tdm_find_fastest_triport
@param:

Returns whether the current port is the fastest port from within a triport
**/
int
tdm_find_fastest_triport( tdm_mod_t *_tdm )
{
	int i, tsc_id=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.pmap_num_lanes; i++) {
		if (_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]!=_tdm->_core_data.vars_pkg.port) {
			if (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.port]<_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[tsc_id][i]]) {
				return BOOL_FALSE;
			}
		}
	}
	
	return BOOL_TRUE;
	
}


/**
@name: tdm_pick_vec
@param:

Select vector index on x axis to rotate based on priority of TSC pipeline
**/
int
tdm_pick_vec( tdm_mod_t *_tdm )
{
	int i, vec_sel=1, port=_tdm->_core_data.vars_pkg.port;
	
	for (i=_tdm->_core_data.vars_pkg.m_tdm_pick_vec.prev_vec; i<_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.lr_vec_cnt; i++) {
		_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[i][0];
		if ( (_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)==_tdm->_core_data.vars_pkg.m_tdm_pick_vec.tsc_dq) ) {
			if (_tdm->_core_data.vars_pkg.m_tdm_pick_vec.triport_priority) {
				if (tdm_find_fastest_triport(_tdm)) {
					vec_sel=i;
					_tdm->_core_data.vars_pkg.m_tdm_pick_vec.triport_priority=BOOL_FALSE;
					break;
				}
				else {
					continue;
				}
			}
			else {
				vec_sel=i;
				break;
			}
		}
	}
	
	_tdm->_core_data.vars_pkg.port=port;
	return vec_sel;
	
}


/**
@name: tdm_map_find_y_indx
@param:

Scans vector map Y axis for Y index of any non-principle node
**/
int
tdm_map_find_y_indx( tdm_mod_t *_tdm )
{
	int i, id=_tdm->_chip_data.soc_pkg.num_ext_ports;
	
	if (_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.principle!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx<_tdm->_core_data.vmap_max_wid) {
		for (i=0; i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
			if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx][i]!=_tdm->_chip_data.soc_pkg.num_ext_ports && _tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.idx][i]!=_tdm->_core_data.vars_pkg.m_tdm_map_find_y_indx.principle) {
				id = i;
				break;
			}
		}
	}
	
	return id;
	
}


/**
@name: tdm_nsin_row
@param:

Checks if current row is singular
**/
int
tdm_nsin_row( tdm_mod_t *_tdm )
{
	int i, found_port=_tdm->_chip_data.soc_pkg.num_ext_ports, check=PASS;
	
	for (i=0; i<_tdm->_core_data.vmap_max_wid; i++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_nsin_row.y_idx]) {
			if (found_port==_tdm->_chip_data.soc_pkg.num_ext_ports) {
				found_port=_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_nsin_row.y_idx];
			}
			else {
				check=FAIL;
				break;
			}
		}
	}
	
	return check;
	
}


/**
@name: tdm_check_blank
@param:

Checks if current row is blank
**/
int
tdm_empty_row(unsigned char **map, unsigned char y_idx, int num_ext_ports, int vec_map_wid)
{
	int i, check=PASS;
	
	for (i=0; i<vec_map_wid; i++) {
		if (map[i][y_idx]!=num_ext_ports) {
			check=FAIL;
			break;
		}
	}
	
	return check;
	
}


/**
@name: tdm_slice_size_2d
@param:

Returns size of proximate 2-D slice in vector map or 0 if the passed row was blank
**/
int
tdm_slice_size_2d(unsigned char **map, unsigned char y_idx, int lim, int num_ext_ports, int vec_map_wid)
{
	int i, slice_size=1;
	
	if (tdm_empty_row(map,y_idx,num_ext_ports,vec_map_wid)) {
		return 0;
	}
	else {
		for (i=1; (y_idx-i)>=0; i++) {
			if (tdm_empty_row(map,i,num_ext_ports,vec_map_wid)) {
				break;
			}
			slice_size++;
		}
		for (i=1; (y_idx+i)<lim; i++) {
			if (tdm_empty_row(map,i,num_ext_ports,vec_map_wid)) {
				break;
			}
			slice_size++;
		}
	}
	
	return slice_size;	
}


/**
@name: tdm_fit_singular_cnt
@param:

Given y index, count number of nodes
**/
int
tdm_fit_singular_cnt(unsigned char **map, int node_y, int vec_map_wid, int num_ext_ports)
{
	int v, cnt=0;
	
	for (v=0; v<vec_map_wid; v++) {
		if (map[v][node_y]!=num_ext_ports) {
			++cnt;
		}
	}
	
	return cnt;
}


/**
@name: tdm_map_cadence_count
@param:

Returns size of port sequence at index
**/
int
tdm_map_cadence_count(unsigned char *vector, int idx, int vec_map_len)
{
	int i=idx, cnt=0;
	unsigned char port=vector[idx];
	
	while (vector[++i]!=port && i<vec_map_len) {
		cnt++;
	}
	
	return cnt;	
}


/**
@name: tdm_map_retrace_count
@param:

Returns number of retraceable slots within cadence at index
**/
int
tdm_map_retrace_count(unsigned char **map, int x_idx, int y_idx, int vec_map_len, int vec_map_wid, int num_ext_ports)
{
	int i=y_idx, cnt=0;
	unsigned char port=map[x_idx][y_idx];
	
	while (map[x_idx][++i]!=port && i<vec_map_len) {
		if ( tdm_fit_singular_cnt(map,i,vec_map_wid,num_ext_ports)==0 ) {
			cnt++;
		}
	}
	
	return cnt;
}


/**
@name: tdm_fit_singular_col
@param:

Given x index, determines fit based on if current column is reducible
**/
int
tdm_fit_singular_col( tdm_mod_t *_tdm )
{
	int v, result=PASS;

	for (v=0; v<_tdm->_chip_data.soc_pkg.lr_idx_limit; v++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_fit_singular_col.node_x][v]) {
			if ( tdm_fit_singular_cnt(_tdm->_core_data.vmap,v,_tdm->_core_data.vmap_max_wid,_tdm->_chip_data.soc_pkg.num_ext_ports)>1 ) {
				result=FAIL;
				break;
			}
		}
	}
	
	return result;
}


/**
@name: tdm_count_nonsingular
@param:

Counts number of unreduced rows at current rotation
**/
int
tdm_count_nonsingular( tdm_mod_t *_tdm )
{
	int i, j, ns_cnt=0;
	
	for (i=0; i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx][i]) {
			for (j=0; j<_tdm->_core_data.vmap_max_wid; j++) {
				if ( (j!=_tdm->_core_data.vars_pkg.m_tdm_count_nonsingular.x_idx) && (_tdm->_core_data.vmap[j][i]!=_tdm->_chip_data.soc_pkg.num_ext_ports) ) {
					ns_cnt++;
				}
			}
		}
	}
	
	return ns_cnt;
}


/**
@name: tdm_fit_row_min
@param:

Checks if current row in vmap has a min spacing violation
**/
int
tdm_fit_row_min( tdm_mod_t *_tdm )
{
	int i, j, port=_tdm->_chip_data.soc_pkg.num_ext_ports,
		pm_0, pm_1;

	for (i=0; i<_tdm->_core_data.vmap_max_wid; i++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx]) {
			port=_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx];
			break;
		}
	}
	TOKEN_CHECK(port) {
		for (i=0; i<_tdm->_core_data.vmap_max_wid; i++) {
			for (j=0; j<VBS_MIN_SPACING; j++) {
				TOKEN_CHECK(_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx+j]) {
					_tdm->_core_data.vars_pkg.port = _tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx+j];
					pm_0 = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
					_tdm->_core_data.vars_pkg.port = port;
					pm_1 = _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
					if (_tdm->_core_data.vmap[i][_tdm->_core_data.vars_pkg.m_tdm_fit_row_min.y_idx+j]!=port && pm_0==pm_1) {
						return FAIL;
					}
				}
			}
		}
	}

	return PASS;
	
}


/**
@name: tdm_fit_prox
@param:

Given x index, determines fit for current vector based on sister port (VBS) spacing rule
**/
int
tdm_fit_prox( tdm_mod_t *_tdm )
{
	int i, j, v, check=PASS, wc=_tdm->_chip_data.soc_pkg.num_ext_ports, _port=_tdm->_core_data.vars_pkg.port;
		
	for (i=(_tdm->_core_data.rule__prox_port_min-1); i<_tdm->_chip_data.soc_pkg.lr_idx_limit; i++) {
		TOKEN_CHECK(_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x][i]) {
			_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x][i];
			wc=_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm);
			for (v=0; v<_tdm->_core_data.vars_pkg.m_tdm_fit_prox.wid; v++) {
				if (v!=_tdm->_core_data.vars_pkg.m_tdm_fit_prox.node_x) {	
					for (j=0; j<_tdm->_core_data.rule__prox_port_min; j++) {
						_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[v][i+j];
						if (wc==_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)) {
							check=FAIL;
							break;
						}
						_tdm->_core_data.vars_pkg.port=_tdm->_core_data.vmap[v][i-j];
						if (wc==_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](_tdm)) {
							check=FAIL;
							break;
						}
					}
				}
			}			
		}
		if (!check) {
			break;
		}
	}
	
	_tdm->_core_data.vars_pkg.port=_port;
	return check;
}


/**
@name: tdm_count_param_spd
@param:

Returns parameterized speed of a vector on the vector map by counting the number of nodes
**/
int
tdm_count_param_spd( tdm_mod_t *_tdm )
{
	int k=0, v, param_spd=_tdm->_chip_data.soc_pkg.num_ext_ports, y_pos=0, port=_tdm->_core_data.vars_pkg.port;
	
	for (v=0; v<_tdm->_chip_data.soc_pkg.lr_idx_limit; v++) {
		if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos][v]!=_tdm->_chip_data.soc_pkg.num_ext_ports) {
			y_pos = (y_pos==0) ? (v):(y_pos);
			k++;
		}
	}
	_tdm->_core_data.vars_pkg.port = _tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.x_pos][y_pos];
	switch(k) {
		case 1: param_spd = 1; break;
		case 4: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 10;} else {param_spd = 11;} break;
		case 8: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 20;} else {param_spd = 21;} break;
		case 10: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 25;} else {param_spd = 27;} break;
		case 16: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 40;} else {param_spd = 42;} break;
		case 20: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 50;} else {param_spd = 53;} break;
		case 39: param_spd = 107; break;
		case 40: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 100;} else {param_spd = 106;} break;
		case 48: if (_tdm->_core_exec[TDM_CORE_EXEC__ENCAP_SCAN](_tdm)) {param_spd = 120;} else {param_spd = 124;} break;
	}
	_tdm->_core_data.vars_pkg.port = port;

	if (param_spd>=10) {
		while (_tdm->_core_data.vars_pkg.m_tdm_count_param_spd.round && param_spd%5!=0) {
			param_spd--;
		}
	}
	
	return param_spd;

}
