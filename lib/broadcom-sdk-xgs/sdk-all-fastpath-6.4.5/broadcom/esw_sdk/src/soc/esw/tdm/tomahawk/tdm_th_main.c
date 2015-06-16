/*
 * $Id: tdm_th_main.c.$
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
 * TDM chip main functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_th_corereq
@param:

Allocate memory for core data execute request to core executive
**/
int
tdm_th_corereq( tdm_mod_t *_tdm )
{
	int idx1;
	
	_tdm->_core_data.vars_pkg.cal_id=_tdm->_core_data.vars_pkg.pipe;
	if (_tdm->_core_data.vars_pkg.pipe==0) {
		_tdm->_core_data.vmap=(unsigned char **) TDM_ALLOC((_tdm->_core_data.vmap_max_wid)*sizeof(unsigned char *), "vector_map_l1");
		for (idx1=0; idx1<(_tdm->_core_data.vmap_max_wid); idx1++) {
			_tdm->_core_data.vmap[idx1]=(unsigned char *) TDM_ALLOC((_tdm->_core_data.vmap_max_len)*sizeof(unsigned char), "vector_map_l2");
		}
	}
	return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}


/**
@name: tdm_th_scheduler_wrap
@param:

Code wrapper for ingress TDM scheduling
**/
int
tdm_th_scheduler_wrap( tdm_mod_t *_tdm )
{
	int iter, idx1=0, idx2=0, ethernet_encap=BOOL_TRUE;
	
	_tdm->_core_data.vars_pkg.pipe=(_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start/32);
	if (_tdm->_core_data.vars_pkg.pipe>3) {
		TDM_ERROR1("Invalid pipe ID - %0d\n",_tdm->_core_data.vars_pkg.pipe);
		return (TDM_EXEC_CHIP_SIZE+1);
	}
	tdm_th_parse_pipe(_tdm);
	for (iter=0; iter<TDM_AUX_SIZE; iter++) {
		_tdm->_core_data.vars_pkg.lr_buffer[iter]=TH_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.os_buffer[iter]=TH_NUM_EXT_PORTS;
	}
	for (iter=(_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start-1); iter<(_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end); iter++) {
		if ( (_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__LINERATE)||(_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__LINERATE_HG) ) {
			_tdm->_core_data.vars_pkg.lr_buffer[idx1]=(iter+1);
			if (idx1<32) {
				idx1++;
			}
			else {
				TDM_PRINT0("Line rate queue overflow, ports may have been skipped.\n");
			}
		}
		else if ( (_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__OVERSUB)||(_tdm->_chip_data.soc_pkg.state[iter]==PORT_STATE__OVERSUB_HG) ) {
			_tdm->_core_data.vars_pkg.os_buffer[idx2]=(iter+1);
			if (idx2<32) {
				idx2++;
			}
			else {
				TDM_PRINT0("Oversub queue overflow, ports may have been skipped.\n");
			}
		}
	}
	if ( (_tdm->_chip_data.soc_pkg.soc_vars.th.mgmt_pm_hg==BOOL_TRUE) && 
		 (_tdm->_core_data.vars_pkg.pipe==1||_tdm->_core_data.vars_pkg.pipe==2) &&
		 ( (_tdm->_core_data.vars_pkg.lr_buffer[0]!=TH_NUM_EXT_PORTS&&_tdm->_chip_data.soc_pkg.clk_freq>=MIN_HG_FREQ)||
		   (_tdm->_core_data.vars_pkg.lr_buffer[0]==TH_NUM_EXT_PORTS) ) ) {
		_tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt=BOOL_TRUE;
	}
	for (iter=0; iter<TDM_AUX_SIZE; iter++) {
		if (_tdm->_core_data.vars_pkg.lr_buffer[iter]!=TH_NUM_EXT_PORTS) {
			TDM_PUSH(_tdm->_core_data.vars_pkg.lr_buffer[iter],TDM_CORE_EXEC__ENCAP_SCAN,ethernet_encap);
			if (!ethernet_encap) {
				break;
			}
		}
		if (_tdm->_core_data.vars_pkg.os_buffer[iter]!=TH_NUM_EXT_PORTS) {
			TDM_PUSH(_tdm->_core_data.vars_pkg.os_buffer[iter],TDM_CORE_EXEC__ENCAP_SCAN,ethernet_encap);
			if (!ethernet_encap) {
				break;
			}
		}
	}
	if ((_tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt==BOOL_FALSE)&&(ethernet_encap)) {
		switch (_tdm->_chip_data.soc_pkg.clk_freq) {
			case 850: _tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_850MHZ_EN-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 850MHz (214+10)*2.65G */
			case 766:
			case 765:
				_tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_765MHZ_EN-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 765MHz (192+10)*2.65G */
			case 672: _tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_672MHZ_EN-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 672MHz (167+10)*2.65G */
			case 645: _tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_645MHZ_EN-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 645MHz (160+10)*2.65G */
			case 545: _tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_545MHZ_EN-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 545MHz (133+10)*2.65G */	
			default:
				TDM_PRINT1("Invalid frequency - %0dMHz not supported\n",_tdm->_chip_data.soc_pkg.clk_freq);
				return 0;
				break;
		}		
	}
	else {
		switch (_tdm->_chip_data.soc_pkg.clk_freq) {
			case 850: _tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_850MHZ_HG-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 850MHz (205+10)*2.65G */
			case 766:
			case 765: 
				_tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_765MHZ_HG-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 765MHz (184+10)*2.65G */
			case 672: _tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_672MHZ_HG-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 672MHz (160+10)*2.65G */
			case 645: _tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_645MHZ_HG-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 645MHz (153+10)*2.65G */
			case 545: _tdm->_chip_data.soc_pkg.lr_idx_limit=(LEN_545MHZ_HG-_tdm->_chip_data.soc_pkg.tvec_size); break; /* 545MHz (128+10)*2.65G */
			default:
				TDM_PRINT1("Invalid frequency - %0dMHz not supported\n",_tdm->_chip_data.soc_pkg.clk_freq);
				return 0;
				break;
		}	
	}
	_tdm->_core_data.vmap_max_wid=TH_VMAP_MAX_WID;
	_tdm->_core_data.vmap_max_len=TH_VMAP_MAX_LEN;

	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ]( _tdm ) );
}


/**
@name: tdm_th_pmap_transcription
@param:

For Tomahawk BCM56960
Transcription algorithm for generating port module mapping
**/
int
tdm_th_pmap_transcription( tdm_mod_t *_tdm )
{
	int i, j, last_port=TH_NUM_EXT_PORTS;
	/*char tmp_str[16];*/
	/*sprintf (cmd_str, "soc setmem -index %x %s {", index->d, addr);*/
	
	for (i=0; i<TH_NUM_PHY_PM; i++) {
		for (j=0; j<TH_NUM_PM_LNS; j++) {
			_tdm->_chip_data.soc_pkg.pmap[i][j]=TH_NUM_EXT_PORTS;
		}
	}
	for (i=1; i<=TH_NUM_PHY_PORTS; i+=TH_NUM_PM_LNS) {
		if (_tdm->_chip_data.soc_pkg.speed[i]>SPEED_0) {
			for (j=0; j<TH_NUM_PM_LNS; j++) {
				switch (_tdm->_chip_data.soc_pkg.state[i+j]) {
					case PORT_STATE__LINERATE: case PORT_STATE__OVERSUB:
					case PORT_STATE__LINERATE_HG: case PORT_STATE__OVERSUB_HG:
						_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][j]=(i+j);
						last_port=(i+j);
						break;
					case PORT_STATE__COMBINE:
						_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][j]=last_port;
						break;
					default:
						_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][j]=TH_NUM_EXT_PORTS;
						break;
				}
			}
			/*tri mode x_xx*/
			if (_tdm->_chip_data.soc_pkg.speed[i]>_tdm->_chip_data.soc_pkg.speed[i+2]&&_tdm->_chip_data.soc_pkg.speed[i+2]==_tdm->_chip_data.soc_pkg.speed[i+3]&&_tdm->_chip_data.soc_pkg.speed[i+2]!=SPEED_0) {
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][2];
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][0];
			}
			/*tri mode xxx_*/
			else if (_tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+1]&&_tdm->_chip_data.soc_pkg.speed[i]<_tdm->_chip_data.soc_pkg.speed[i+2]&&_tdm->_chip_data.soc_pkg.speed[i]!=SPEED_0)  {
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][1];
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][3];
			}
			/*dual mode*/
			else if (_tdm->_chip_data.soc_pkg.speed[i]!=_tdm->_chip_data.soc_pkg.speed[i+1]&&_tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+2]&&_tdm->_chip_data.soc_pkg.speed[i]!=SPEED_0) {
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][3];
				_tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TH_NUM_PM_LNS][0];
			}
		}
	}
	tdm_print_stat( _tdm );
	for (i=0; i<(TH_NUM_EXT_PORTS-1); i++) {
		_tdm->_chip_data.soc_pkg.state[i]=_tdm->_chip_data.soc_pkg.state[i+1];
	}
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP]( _tdm ) );
}


/**
@name: tdm_th_init
@param:

**/
int
tdm_th_init( tdm_mod_t *_tdm )
{
	int index;
	
	_tdm->_chip_data.soc_pkg.pmap_num_modules = TH_NUM_PM_MOD;
	_tdm->_chip_data.soc_pkg.pmap_num_lanes = TH_NUM_PM_LNS;
	_tdm->_chip_data.soc_pkg.pm_num_phy_modules = TH_NUM_PHY_PM;
	
	_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = TH_OVSB_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl1_token = TH_IDL1_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl2_token = TH_IDL2_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.ancl_token = TH_ANCL_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = TH_NUM_PHY_PORTS;
	
	_tdm->_chip_data.cal_0.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_0.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_0.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_1.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_1.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_1.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_2.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_2.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_2.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_3.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_3.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_3.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_4.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_4.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_4.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_5.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_5.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_5.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_6.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_6.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_6.grp_len = TH_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_7.cal_len = TH_LR_VBS_LEN;
	_tdm->_chip_data.cal_7.grp_num = TH_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_7.grp_len = TH_OS_VBS_GRP_LEN;	
	
	_tdm->_core_data.vars_pkg.pipe=0;
	_tdm->_chip_data.soc_pkg.soc_vars.th.higig_mgmt=BOOL_FALSE;
	_tdm->_chip_data.soc_pkg.lr_idx_limit=LEN_850MHZ_EN;
	_tdm->_chip_data.soc_pkg.tvec_size = TH_ACC_PORT_NUM;
	_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start=1;
	_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end=32;
	
	_tdm->_core_data.rule__same_port_min = TH_LR_VBS_LEN;
	_tdm->_core_data.rule__prox_port_min = VBS_MIN_SPACING;
	
	for (index=0; index<TH_NUM_PM_MOD; index++) {
		_tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type[index] = (_tdm->_chip_data.soc_pkg.state[index*TH_NUM_PM_LNS]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[index]==PORT_STATE__OVERSUB_HG)?(999):(998);
	}
	_tdm->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
		_tdm->_chip_data.soc_pkg.pmap[index]=(int *) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm->_chip_data.soc_pkg.pmap[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.soc_pkg.pmap_num_lanes);
	}
	/* IDB Pipe 0 calendar group */
	_tdm->_chip_data.cal_0.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.cal_len)*sizeof(int), "TDM inst 0 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_0.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.cal_len);
	_tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num)*sizeof(int *), "TDM inst 0 groups");
	for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
		_tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
	}
	/* IDB Pipe 1 calendar group */
	_tdm->_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_1.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.cal_len);
	_tdm->_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_num)*sizeof(int *), "TDM inst 1 groups");
	for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
		_tdm->_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_len)*sizeof(int), "TDM inst 1 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.grp_len);
	}
	/* IDB Pipe 2 calendar group */
	_tdm->_chip_data.cal_2.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.cal_len)*sizeof(int), "TDM inst 2 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_2.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.cal_len);
	_tdm->_chip_data.cal_2.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_num)*sizeof(int *), "TDM inst 2 groups");
	for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
		_tdm->_chip_data.cal_2.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_len)*sizeof(int), "TDM inst 2 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_2.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.grp_len);
	}
	/* IDB Pipe 3 calendar group */
	_tdm->_chip_data.cal_3.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.cal_len)*sizeof(int), "TDM inst 3 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_3.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.cal_len);
	_tdm->_chip_data.cal_3.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_num)*sizeof(int *), "TDM inst 3 groups");
	for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
		_tdm->_chip_data.cal_3.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_len)*sizeof(int), "TDM inst 3 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_3.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.grp_len);
	}
	/* MMU Pipe 0 calendar group */
	_tdm->_chip_data.cal_4.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.cal_len)*sizeof(int), "TDM inst 0 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_4.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.cal_len);
	_tdm->_chip_data.cal_4.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_num)*sizeof(int *), "TDM inst 0 groups");
	for (index=0; index<(_tdm->_chip_data.cal_4.grp_num); index++) {
		_tdm->_chip_data.cal_4.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_len)*sizeof(int), "TDM inst 0 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_4.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.grp_len);
	}
	/* MMU Pipe 1 calendar group */
	_tdm->_chip_data.cal_5.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_5.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.cal_len);
	_tdm->_chip_data.cal_5.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_num)*sizeof(int *), "TDM inst 1 groups");
	for (index=0; index<(_tdm->_chip_data.cal_5.grp_num); index++) {
		_tdm->_chip_data.cal_5.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_len)*sizeof(int), "TDM inst 1 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_5.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.grp_len);
	}
	/* MMU Pipe 2 calendar group */
	_tdm->_chip_data.cal_6.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_6.cal_len)*sizeof(int), "TDM inst 2 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_6.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_6.cal_len);
	_tdm->_chip_data.cal_6.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_6.grp_num)*sizeof(int *), "TDM inst 2 groups");
	for (index=0; index<(_tdm->_chip_data.cal_6.grp_num); index++) {
		_tdm->_chip_data.cal_6.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_6.grp_len)*sizeof(int), "TDM inst 2 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_6.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_6.grp_len);
	}
	/* MMU Pipe 3 calendar group */
	_tdm->_chip_data.cal_7.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_7.cal_len)*sizeof(int), "TDM inst 3 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_7.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_7.cal_len);
	_tdm->_chip_data.cal_7.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_7.grp_num)*sizeof(int *), "TDM inst 3 groups");
	for (index=0; index<(_tdm->_chip_data.cal_7.grp_num); index++) {
		_tdm->_chip_data.cal_7.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_7.grp_len)*sizeof(int), "TDM inst 3 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_7.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_7.grp_len);
	}
	
	for (index=(TH_NUM_EXT_PORTS-1); index>0; index--) {
		_tdm->_chip_data.soc_pkg.state[index]=_tdm->_chip_data.soc_pkg.state[index-1];
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION]( _tdm ) );
}


/**
@name: tdm_th_post
@param:

**/
int
tdm_th_post( tdm_mod_t *_tdm )
{
	_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start+=32;
	_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end+=32;
	
	return (_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end>128)?(PASS):( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP]( _tdm ) );
}
