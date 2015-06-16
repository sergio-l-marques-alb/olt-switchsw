/*
 * $Id: tdm_th_proc.c.$
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
	int j, v, w, load_status=FAIL;
	unsigned char lr1[TDM_AUX_SIZE], lr10[TDM_AUX_SIZE], lr20[TDM_AUX_SIZE], lr25[TDM_AUX_SIZE], lr40[TDM_AUX_SIZE], lr50[TDM_AUX_SIZE], lr100[TDM_AUX_SIZE], lr120[TDM_AUX_SIZE];
		
	for (j=0; j<(TDM_AUX_SIZE); j++) {
		lr1[j] = TH_NUM_EXT_PORTS; lr120[j] = TH_NUM_EXT_PORTS;
		lr100[j] = TH_NUM_EXT_PORTS; lr50[j] = TH_NUM_EXT_PORTS; lr40[j] = TH_NUM_EXT_PORTS;
		lr25[j] = TH_NUM_EXT_PORTS; lr20[j] = TH_NUM_EXT_PORTS; lr10[j] = TH_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os1[j] = TH_NUM_EXT_PORTS; _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120[j] = TH_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10[j] = TH_NUM_EXT_PORTS; _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20[j] = TH_NUM_EXT_PORTS; _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25[j] = TH_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40[j] = TH_NUM_EXT_PORTS; _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50[j] = TH_NUM_EXT_PORTS; _tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100[j] = TH_NUM_EXT_PORTS;
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
					tdm_core_prealloc(lr10, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1), &(_tdm->_core_data.vars_pkg.lr_10), j);
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					tdm_core_prealloc(lr20, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2), &(_tdm->_core_data.vars_pkg.lr_20), j);
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					tdm_core_prealloc(lr40, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3), &(_tdm->_core_data.vars_pkg.lr_40), j);
					break;
				case SPEED_100G:
				case SPEED_106G:
					tdm_core_prealloc(lr100, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4), &(_tdm->_core_data.vars_pkg.lr_100), j);
					break;
				case SPEED_50G:
				case SPEED_53G:
					tdm_core_prealloc(lr50, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5), &(_tdm->_core_data.vars_pkg.lr_50), j);
					break;
				case SPEED_25G:
				case SPEED_27G:
					tdm_core_prealloc(lr25, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6), &(_tdm->_core_data.vars_pkg.lr_25), j);
					break;
				case SPEED_120G:
				case SPEED_126G:
					tdm_core_prealloc(lr120, _tdm->_core_data.vars_pkg.lr_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7), &(_tdm->_core_data.vars_pkg.lr_120), j);
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
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os10, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z1), &(_tdm->_core_data.vars_pkg.os_10), j);
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os20, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z2), &(_tdm->_core_data.vars_pkg.os_20), j);
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os40, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z3), &(_tdm->_core_data.vars_pkg.os_40), j);
					break;
				case SPEED_100G:
				case SPEED_106G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os100, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z4), &(_tdm->_core_data.vars_pkg.os_100), j);
					break;
				case SPEED_50G:
				case SPEED_53G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os50, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z5), &(_tdm->_core_data.vars_pkg.os_50), j);
					break;
				case SPEED_25G:
				case SPEED_27G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os25, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z6), &(_tdm->_core_data.vars_pkg.os_25), j);
					break;
				case SPEED_120G:
				case SPEED_126G:
					tdm_core_prealloc(_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.os120, _tdm->_core_data.vars_pkg.os_buffer, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.z7), &(_tdm->_core_data.vars_pkg.os_120), j);
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
	
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7 > 0) {
		if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 120, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &(_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y7), _tdm->_chip_data.soc_pkg.lr_idx_limit, lr120, TOKEN_120G, "120", TH_NUM_EXT_PORTS)!=PASS) {
			TDM_ERROR0("Critical error in 120G time vector preallocation\n");
			return 0;
		}
	}
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4 > 0) {
		TDM_BIG_BAR
		TDM_PRINT0("100G scheduling pass\n");
		TDM_SML_BAR
		TDM_PRINT0("Stack contains: [ ");
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4; v>0; v--) {
			if (lr100[v]!=TH_NUM_EXT_PORTS) {
				TDM_PRINT1(" %0d ",lr100[v]);
			}
		}
		TDM_PRINT0(" ]\n");
		TDM_SML_BAR
		while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4 > 0) {
			if (_tdm->_core_data.vars_pkg.HG4X106G_3X40G) {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 107, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			else if (_tdm->_core_data.vars_pkg.HG1X106G_xX40G_OVERSUB) {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 108, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			else if (_tdm->_core_data.vars_pkg.HGXx120G_Xx100G) {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 109, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			else {
				if (tdm_th_check_ethernet(tdm_chip_th_shim__check_ethernet_d(lr100[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4],_tdm->_chip_data.soc_pkg.speed,_tdm->_chip_data.soc_pkg.state,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type)) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE) {
					load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 100, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
				}
				else {
					load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 106, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
				}
			}
			if (load_status==FAIL) {
				TDM_ERROR1("Failed to load 100G/106G vector for port %0d\n",lr100[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]==TOKEN_100G || _tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]==TOKEN_106G) {
						_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]=lr100[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4];
					}
				}
				TDM_PRINT3("Loaded 100G port %d vector from stack pointer %d into map at index %0d\n", lr100[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4], _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
			}
			_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4--; _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy++;
			if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4>0) {
				TDM_SML_BAR
				TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y4);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_PRINT1("Done loading 100G vectors, map pointer floating at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5 > 0) {
		TDM_BIG_BAR
		TDM_PRINT0("50G scheduling pass\n");
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
		TDM_SML_BAR
		TDM_PRINT0("Stack contains: [ ");
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			if (lr50[v]!=TH_NUM_EXT_PORTS) {
				TDM_PRINT1(" %0d ",lr50[v]);
			}
		}
		TDM_PRINT0(" ]\n");
		TDM_SML_BAR
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_th_legacy_which_tsc(lr50[v],_tdm->_chip_data.soc_pkg.pmap);
			for (w=0; w<4; w++) {
				if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]]!=SPEED_50G) {
					_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_50=BOOL_TRUE;
					break;
				}
			}
			if (_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_50==BOOL_TRUE) {
				TDM_PRINT0("50G triport detected\n");
				break;
			}
		}
		while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5 > 0) {
			if (tdm_th_check_ethernet(tdm_chip_th_shim__check_ethernet_d(lr50[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5],_tdm->_chip_data.soc_pkg.speed,_tdm->_chip_data.soc_pkg.state,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type)) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE) {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 50, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			else {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 53, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			if (load_status==FAIL)
			{
				TDM_ERROR1("Failed to load 50G vector for port %0d\n",lr50[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]==TOKEN_50G) {
						_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]=lr50[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5];
					}
				}			
				TDM_PRINT3("Loaded 50G port %d vector from stack pointer %d into map at index %0d\n", lr50[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5], _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
			}			
			_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5--; _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy++;
			if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5>0) {
				TDM_SML_BAR
				TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y5);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_PRINT1("Done scheduling 50G, map pointer floating at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3 > 0) {
		TDM_BIG_BAR
		TDM_PRINT0("40G scheduling pass\n");
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
		TDM_SML_BAR
		TDM_PRINT0("Stack contains: [ ");
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			if (lr40[v]!=TH_NUM_EXT_PORTS) {
				TDM_PRINT1(" %0d ",lr40[v]);
			}
		}
		TDM_PRINT0(" ]\n");
		TDM_SML_BAR
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_th_legacy_which_tsc(lr40[v],_tdm->_chip_data.soc_pkg.pmap);
			for (w=0; w<4; w++) {
				if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]]!=SPEED_40G && _tdm->_chip_data.soc_pkg.state[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]-1]!=PORT_STATE__DISABLED) {
					_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40=BOOL_TRUE;
					break;
				}
			}
			if (_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_40==BOOL_TRUE) {
				TDM_PRINT0("40G triport detected\n");
				break;
			}
		}
		while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3 > 0) {
			if (_tdm->_core_data.vars_pkg.HG4X106G_3X40G==BOOL_TRUE) {
				if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3==3) {
					load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 43, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
				}
				else if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3==2) {
					load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 44, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
				}
				else if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3==1) {
					load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 45, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
				}				
			}
			else {
				if (tdm_th_check_ethernet(tdm_chip_th_shim__check_ethernet_d(lr40[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3],_tdm->_chip_data.soc_pkg.speed,_tdm->_chip_data.soc_pkg.state,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type)) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE) {
					load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 40, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
				}
				else {
					load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 42, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
				}
			}
			if (load_status==FAIL) {
				TDM_ERROR1("Failed to load 40G vector for port %0d\n",lr40[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]==TOKEN_40G) {
						_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]=lr40[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3];
					}
				}			
				TDM_PRINT3("Loaded 40G port %d vector from stack pointer %d into map at index %0d\n", lr40[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3], _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
			}			
			_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3--; _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy++;
			if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3>0) {
				TDM_SML_BAR
				TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y3);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_PRINT1("Done scheduling 40G, map pointer floating at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}	
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6 > 0) {
		TDM_BIG_BAR
		TDM_PRINT0("25G scheduling pass\n");
		TDM_SML_BAR
		TDM_PRINT0("Stack contains: [ ");
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6; v>0; v--) {
			if (lr25[v]!=TH_NUM_EXT_PORTS) {
				TDM_PRINT1(" %0d ",lr25[v]);
			}
		}
		TDM_PRINT0(" ]\n");
		TDM_SML_BAR
		while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6 > 0) {
			if (tdm_th_check_ethernet(tdm_chip_th_shim__check_ethernet_d(lr25[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6],_tdm->_chip_data.soc_pkg.speed,_tdm->_chip_data.soc_pkg.state,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type)) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE) {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 25, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			else {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 27, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			if (load_status==FAIL) {
				TDM_ERROR1("Failed to load 25G vector for port %0d\n",lr25[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]==TOKEN_25G) {
						_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]=lr25[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6];
					}
				}			
				TDM_PRINT3("Loaded 25G port %d vector from stack pointer %d into map at index %0d\n", lr25[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6], _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
			}			
			_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6--; _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy++;
			if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6>0) {
				TDM_SML_BAR
				TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y6);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_PRINT1("Done scheduling 25G, map pointer floating at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2 > 0) {
		TDM_BIG_BAR
		TDM_PRINT0("20G scheduling pass\n");
		TDM_SML_BAR
		TDM_PRINT0("Stack contains: [ ");
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2; v>0; v--) {
			if (lr20[v]!=TH_NUM_EXT_PORTS) {
				TDM_PRINT1(" %0d ",lr20[v]);
			}
		}
		TDM_PRINT0(" ]\n");
		TDM_SML_BAR
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2; v>0; v--) {
			_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk = tdm_th_legacy_which_tsc(lr20[v],_tdm->_chip_data.soc_pkg.pmap);
			for (w=0; w<4; w++) {
				if (_tdm->_chip_data.soc_pkg.speed[_tdm->_chip_data.soc_pkg.pmap[_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_chk][w]]!=SPEED_20G) {
					_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_20=BOOL_TRUE;
					break;
				}
			}
			if (_tdm->_core_data.vars_pkg.m_tdm_vmap_alloc.tri_en_20==BOOL_TRUE) {
				TDM_PRINT0("20G triport detected\n");
				break;
			}
		}
		while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2 > 0) {
			if (tdm_th_check_ethernet(tdm_chip_th_shim__check_ethernet_d(lr20[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2],_tdm->_chip_data.soc_pkg.speed,_tdm->_chip_data.soc_pkg.state,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type)) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE) {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 20, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			else {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 21, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			if (load_status==FAIL)
			{
				TDM_ERROR1("Failed to load 20G vector for port %0d\n",lr20[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]==TOKEN_20G) {
						_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]=lr20[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2];
					}
				}
				TDM_PRINT3("Loaded 20G port %d vector from stack pointer %d into map at index %0d\n", lr20[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2], _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
			}			
			_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2--; _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy++;
			if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2>0) {
				TDM_SML_BAR
				TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y2);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_PRINT1("Done scheduling 20G, map pointer floating at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1 > 0) {
		TDM_BIG_BAR
		TDM_PRINT0("10G scheduling pass\n");
		TDM_SML_BAR
		TDM_PRINT0("Stack contains: [ ");
		for (v=_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1; v>0; v--) {
			if (lr10[v]!=TH_NUM_EXT_PORTS) {
				TDM_PRINT1(" %0d ",lr10[v]);
			}
		}
		TDM_PRINT0(" ]\n");
		TDM_SML_BAR
		while (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1 > 0) {
			if (tdm_th_check_ethernet(tdm_chip_th_shim__check_ethernet_d(lr10[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1],_tdm->_chip_data.soc_pkg.speed,_tdm->_chip_data.soc_pkg.state,_tdm->_chip_data.soc_pkg.pmap,_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type)) && _tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE) {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 10, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			else {
				load_status = tdm_core_vec_load(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 11, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_chip_data.soc_pkg.lr_idx_limit, TH_NUM_EXT_PORTS);
			}
			if (load_status==FAIL)
			{
				TDM_ERROR1("Failed to load 10G vector for port %0d\n",lr10[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1]);
				TDM_ERROR0("Critical error in 10G time vector preallocation\n");
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]==TOKEN_10G) {
						_tdm->_core_data.vmap[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy][v]=lr10[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1];
					}
				}
				TDM_PRINT3("Loaded 10G port %d vector from stack pointer %d into map at index %0d\n", lr10[_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1], _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
			}			
			_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1--; _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy++;
			if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1>0) {
				TDM_SML_BAR
				TDM_PRINT2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y1);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_PRINT1("Done scheduling 10G, map pointer floating at: %0d\n", _tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	if (_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8 > 0) {
		if (tdm_core_postalloc(_tdm->_core_data.vmap, _tdm->_chip_data.soc_pkg.clk_freq, 1, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.yy, &_tdm->_core_data.vars_pkg.m_tdm_core_vbs_scheduler.y8, _tdm->_chip_data.soc_pkg.lr_idx_limit, lr1, TOKEN_1G, "1", TH_NUM_EXT_PORTS)!=PASS) {
			TDM_ERROR0("Critical error in 1G time vector preallocation\n");
			return 0;
		}
	}
	
	return ( _tdm->_core_exec[TDM_CORE_EXEC__SCHEDULER]( _tdm ) );
}
