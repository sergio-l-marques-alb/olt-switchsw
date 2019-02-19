/*
 * $Id: tdm_th_proc.c.$
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
 * TDM chip operations
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
	#include <tdm_th_vec.h>
#else
	#include <soc/tdm/core/tdm_top.h>
	#include <soc/tdm/tomahawk/tdm_th_vec.h>
#endif

int tdm_core_vec_load(unsigned char **map, int bw, int port_speed, int yy, int len, int num_ext_ports);


/**
@name: tdm_th_vmap_alloc
@param:

**/
int
tdm_th_vmap_alloc( tdm_mod_t *_tdm )
{
	int j, v, w;
	unsigned char lr1[TDM_AUX_SIZE], lr10[TDM_AUX_SIZE], lr20[TDM_AUX_SIZE], lr25[TDM_AUX_SIZE], lr40[TDM_AUX_SIZE], lr50[TDM_AUX_SIZE], lr100[TDM_AUX_SIZE];
		
	for (j=0; j<(TDM_AUX_SIZE); j++) {
        lr1[j]   = TH_NUM_EXT_PORTS;
        lr10[j]  = TH_NUM_EXT_PORTS;
        lr20[j]  = TH_NUM_EXT_PORTS;
        lr25[j]  = TH_NUM_EXT_PORTS;
        lr40[j]  = TH_NUM_EXT_PORTS;
        lr50[j]  = TH_NUM_EXT_PORTS;
        lr100[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1[j]  = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50[j] = TH_NUM_EXT_PORTS;
        _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100[j]= TH_NUM_EXT_PORTS;
	}	
	
	for (j=0; j<TDM_AUX_SIZE; j++) {
		if ((_tdm->_core_data.vars_pkg.lr_buffer[j] != TH_NUM_EXT_PORTS) && ( (_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1] == PORT_STATE__LINERATE)||(_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1] == PORT_STATE__LINERATE_HG) )) {
			switch (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.lr_buffer[j]]) {
				case SPEED_1G:
				case SPEED_2p5G:
					tdm_core_prealloc(lr1, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8), &(_tdm->_core_data.vars_pkg.lr_1), j);
					break;
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
				case SPEED_11G:
					tdm_core_prealloc(lr10, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), &(_tdm->_core_data.vars_pkg.lr_10), j);
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					tdm_core_prealloc(lr20, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), &(_tdm->_core_data.vars_pkg.lr_20), j);
					break;
				case SPEED_25G:
				case SPEED_27G:
					tdm_core_prealloc(lr25, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), &(_tdm->_core_data.vars_pkg.lr_25), j);
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					tdm_core_prealloc(lr40, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), &(_tdm->_core_data.vars_pkg.lr_40), j);
					break;
				case SPEED_50G:
				case SPEED_53G:
					tdm_core_prealloc(lr50, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), &(_tdm->_core_data.vars_pkg.lr_50), j);
					break;
				case SPEED_100G:
				case SPEED_106G:
					tdm_core_prealloc(lr100, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), &(_tdm->_core_data.vars_pkg.lr_100), j);
					break;
				default:
					TDM_ERROR2("Speed %0d port %0d was skipped in linerate preallocation\n",_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.lr_buffer[j]],_tdm->_core_data.vars_pkg.lr_buffer[j]);
			}
		}
		else if (_tdm->_core_data.vars_pkg.lr_buffer[j] != TH_NUM_EXT_PORTS && _tdm->_core_data.vars_pkg.lr_buffer[j] != TH_OVSB_TOKEN) {
			TDM_ERROR2("Failed to presort linerate port %0d with state mapping %0d\n", _tdm->_core_data.vars_pkg.lr_buffer[j], _tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.lr_buffer[j]-1]);
		}
		if ((_tdm->_core_data.vars_pkg.os_buffer[j] != TH_NUM_EXT_PORTS) && ( (_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1] == PORT_STATE__OVERSUB)||(_tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1] == PORT_STATE__OVERSUB_HG) )) {
			switch (_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.os_buffer[j]]) {
				case SPEED_1G:
				case SPEED_2p5G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8), &(_tdm->_core_data.vars_pkg.os_1), j);
					break;
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
				case SPEED_11G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1), &(_tdm->_core_data.vars_pkg.os_10), j);
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2), &(_tdm->_core_data.vars_pkg.os_20), j);
					break;
				case SPEED_25G:
				case SPEED_27G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6), &(_tdm->_core_data.vars_pkg.os_25), j);
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3), &(_tdm->_core_data.vars_pkg.os_40), j);
					break;
				case SPEED_50G:
				case SPEED_53G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5), &(_tdm->_core_data.vars_pkg.os_50), j);
					break;
				case SPEED_100G:
				case SPEED_106G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4), &(_tdm->_core_data.vars_pkg.os_100), j);
					break;
				default:
					TDM_ERROR2("Speed %0d port %0d was skipped in oversub preallocation\n",_tdm->_chip_data.soc_pkg.speed[_tdm->_core_data.vars_pkg.os_buffer[j]],_tdm->_core_data.vars_pkg.os_buffer[j]);
			}
		}
		else if (_tdm->_core_data.vars_pkg.os_buffer[j] != TH_NUM_EXT_PORTS) {
			TDM_ERROR2("Failed to presort oversub port %0d with state mapping %0d\n", _tdm->_core_data.vars_pkg.os_buffer[j], _tdm->_chip_data.soc_pkg.state[_tdm->_core_data.vars_pkg.os_buffer[j]-1]);
		}
	}   
	_tdm->_core_data.vars_pkg.os_enable = (_tdm->_core_data.vars_pkg.os_1||_tdm->_core_data.vars_pkg.os_10||_tdm->_core_data.vars_pkg.os_20||_tdm->_core_data.vars_pkg.os_25||_tdm->_core_data.vars_pkg.os_40||_tdm->_core_data.vars_pkg.os_50||_tdm->_core_data.vars_pkg.os_100||_tdm->_core_data.vars_pkg.os_120);
	_tdm->_core_data.vars_pkg.lr_enable = (_tdm->_core_data.vars_pkg.lr_1||_tdm->_core_data.vars_pkg.lr_10||_tdm->_core_data.vars_pkg.lr_20||_tdm->_core_data.vars_pkg.lr_25||_tdm->_core_data.vars_pkg.lr_40||_tdm->_core_data.vars_pkg.lr_50||_tdm->_core_data.vars_pkg.lr_100||_tdm->_core_data.vars_pkg.lr_120);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_lr = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_os = (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z8+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6+_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7);
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4;
	_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g = _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3;
	/* 4x106HG+3x40G uses a unique TDM in Tomahawk */
	if (_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_40g==3 && _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g==4 && _tdm->_chip_data.soc_pkg.clk_freq==850) {
		_tdm->_chip_data.soc_pkg.tvec_size=9;
		_tdm->_chip_data.soc_pkg.lr_idx_limit=201;
		_tdm->_core_data.vars_pkg.HG4X106G_3X40G=BOOL_TRUE;
	}
	/* To guarantee oversub smoothness in nonscaling 100G+40G case, algorithm must sort with a 120G port vector (bw will be reclaimed) */
	if ( ( (_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.num_100g==1) && (_tdm->_core_data.vars_pkg.lr_40) && (!_tdm->_core_data.vars_pkg.lr_120) && (_tdm->_core_data.vars_pkg.os_enable) ) ) {
		_tdm->_core_data.vars_pkg.HG1X106G_xX40G_OVERSUB=BOOL_TRUE;
	}
	/* To sort 120G and 100G vectors together, load a special 100G vector */
	if ( (_tdm->_core_data.vars_pkg.lr_100 && _tdm->_core_data.vars_pkg.lr_120) ) {
		_tdm->_core_data.vars_pkg.HGXx120G_Xx100G=BOOL_TRUE;
	}
	TDM_PRINT9("(1G - %0d) (10G - %0d) (20G - %0d) (25G - %0d) (40G - %0d) (50G - %0d) (100G - %0d) (120G - %0d) (Number of Line Rate Types - %0d)\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7, (_tdm->_core_data.vars_pkg.lr_1+_tdm->_core_data.vars_pkg.lr_10+_tdm->_core_data.vars_pkg.lr_20+_tdm->_core_data.vars_pkg.lr_25+_tdm->_core_data.vars_pkg.lr_40+_tdm->_core_data.vars_pkg.lr_50+_tdm->_core_data.vars_pkg.lr_100+_tdm->_core_data.vars_pkg.lr_120));
	TDM_BIG_BAR
	/* 100G */
	if(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4 > 0){
		if (_tdm->_core_data.vars_pkg.HG4X106G_3X40G) {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 107, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 100G time vector preallocation\n");
				return 0;
			}
		}
		else if (_tdm->_core_data.vars_pkg.HG1X106G_xX40G_OVERSUB) {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 108, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 100G time vector preallocation\n");
				return 0;
			}
		}
		else if (_tdm->_core_data.vars_pkg.HGXx120G_Xx100G) {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 109, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 100G time vector preallocation\n");
				return 0;
			}
		}
		else{
			
			if (tdm_th_check_pipe_ethernet(lr100[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
				if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 100, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS)!=PASS) {
					TDM_ERROR0("Critical error in 100G time vector preallocation\n");
					return 0;
				}
			}
			else {
				if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 106, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr100, TOKEN_100G, "100", TH_NUM_EXT_PORTS)!=PASS) {
					TDM_ERROR0("Critical error in 100G time vector preallocation\n");
					return 0;
				}
			}
		}
	}
	/* 50G */
	if(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5 > 0){
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			if (tdm_th_legacy_which_tsc(lr50[v+1],_tdm->_chip_data.soc_pkg.pmap)==tdm_th_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap) ||
				tdm_th_legacy_which_tsc(lr50[v-1],_tdm->_chip_data.soc_pkg.pmap)==tdm_th_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap)) {
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap=lr50[v];
				for (w=v; w>0; w--) {
					lr50[w]=lr50[w-1];
				}
				lr50[1]=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap;
			}
		}
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_th_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap);
			if ((_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ||
			    (_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ){
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_50=BOOL_TRUE;
				TDM_PRINT0("50G triport detected\n");
				break;
			}
		}
		if (tdm_th_check_pipe_ethernet(lr50[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 50, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr50, TOKEN_50G, "50", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 50G time vector preallocation\n");
				return 0;
			}
		}
		else {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 53, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr50, TOKEN_50G, "50", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 50G time vector preallocation\n");
				return 0;
			}
		}
	}
	/* 40G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3 > 0) {
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			if (tdm_th_legacy_which_tsc(lr40[v+1],_tdm->_chip_data.soc_pkg.pmap)==tdm_th_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap) ||
				tdm_th_legacy_which_tsc(lr40[v-1],_tdm->_chip_data.soc_pkg.pmap)==tdm_th_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap)) {
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap=lr40[v];
				for (w=v; w>0; w--) {
					lr40[w]=lr40[w-1];
				}
				lr40[1]=_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.prox_swap;
			}
		}
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_th_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap);
			if ((_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ||
			    (_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ){
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40=BOOL_TRUE;
				TDM_PRINT0("40G triport detected\n");
				break;
			}
		}
		if (_tdm->_core_data.vars_pkg.HG4X106G_3X40G==BOOL_TRUE) {
			if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3==3) {
				if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 43, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
			else if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3==2) {
				if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 44, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
			else if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3==1) {
				if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 45, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
		}
		else {
			if (tdm_th_check_pipe_ethernet(lr40[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
				if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 40, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
			else {
				if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 42, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr40, TOKEN_40G, "40", TH_NUM_EXT_PORTS)!=PASS) {
					TDM_ERROR0("Critical error in 40G time vector preallocation\n");
					return 0;
				}
			}
		}
	}
	/* 25G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6 > 0) {
		if (tdm_th_check_pipe_ethernet(lr25[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 25, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr25, TOKEN_25G, "25", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 25G time vector preallocation\n");
				return 0;
			}
		}
		else {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 27, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr25, TOKEN_25G, "25", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 25G time vector preallocation\n");
				return 0;
			}
		}
	}
	/* 20G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2 > 0) {
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_th_legacy_which_tsc(lr20[v],_tdm->_chip_data.soc_pkg.pmap);
			if ((_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ||
			    (_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][0]==_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][2] && _tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][1]!=_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][3]) ){
				_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_20=BOOL_TRUE;
				TDM_PRINT0("20G triport detected\n");
				break;
			}
		}
		if (tdm_th_check_pipe_ethernet(lr20[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE) {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 20, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr20, TOKEN_20G, "20", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 20G time vector preallocation\n");
				return 0;
			}
		}
		else {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 21, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr20, TOKEN_20G, "20", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 20G time vector preallocation\n");
				return 0;
			}
		}
	}
	/* 10G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1 > 0) {
		if (tdm_th_check_pipe_ethernet(lr10[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1],_tdm) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE && _tdm->_chip_data.soc_pkg.soc_vars.th.cal_hg_en==BOOL_FALSE) {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 10, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr10, TOKEN_10G, "10", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 10G time vector preallocation\n");
				return 0;
			}
		}
		else {
			if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 11, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr10, TOKEN_10G, "10", TH_NUM_EXT_PORTS)!=PASS) {
				TDM_ERROR0("Critical error in 10G time vector preallocation\n");
				return 0;
			}
		}
	}
	/* 1G */
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8 > 0) {
		if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 1, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8, _tdm->_chip_data.soc_pkg.lr_idx_limit, lr1, TOKEN_1G, "1", TH_NUM_EXT_PORTS)!=PASS) {
			TDM_ERROR0("Critical error in 1G time vector preallocation\n");
			return 0;
		}
	}
	
	return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER]( _tdm ) );
}
