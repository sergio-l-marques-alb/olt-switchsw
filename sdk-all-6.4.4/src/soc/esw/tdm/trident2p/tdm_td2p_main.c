/*
 * $Id: tdm_td2p_main.c.$
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
@name: tdm_td2p_corereq
@param:

Allocate memory for core data execute request to core executive
**/
int
tdm_td2p_corereq( tdm_mod_t *_tdm )
{
	int idx1;

	if (_tdm->_core_data.vars_pkg.pipe==TD2P_PIPE_Y_ID) {
		_tdm->_core_data.vars_pkg.cal_id=5;
		_tdm->_core_exec[TDM_CORE_EXEC__POST]=&tdm_core_post;
	}
	else if (_tdm->_core_data.vars_pkg.pipe==TD2P_PIPE_X_ID) {
		_tdm->_core_data.vmap=(unsigned char **) TDM_ALLOC((_tdm->_core_data.vmap_max_wid)*sizeof(unsigned char *), "vector_map_l1");
		for (idx1=0; idx1<(_tdm->_core_data.vmap_max_wid); idx1++) {
			_tdm->_core_data.vmap[idx1]=(unsigned char *) TDM_ALLOC((_tdm->_core_data.vmap_max_len)*sizeof(unsigned char), "vector_map_l2");
		}
		_tdm->_core_data.vars_pkg.cal_id=4;
		_tdm->_core_exec[TDM_CORE_EXEC__POST]=&tdm_td2p_vbs_wrapper;
	}
	else {
		return (TDM_EXEC_CORE_SIZE+1);
	}
	
	return ( _tdm->_core_exec[TDM_CORE_EXEC__INIT]( _tdm ) );
}


/**
@name: tdm_td2p_vbs_wrapper
@param:

Code wrapper for egress TDM scheduling
**/
int
tdm_td2p_vbs_wrapper( tdm_mod_t *_tdm )
{
	int idx1, idx2, acc, clk, lr_buffer_idx=0, os_buffer_idx=0, pipe, higig_mgmt=BOOL_FALSE, lr_idx_limit=LEN_760MHZ_EN,
		*pgw_tdm_tbl_0, *pgw_tdm_tbl_1, *ovs_tdm_tbl_0, *ovs_tdm_tbl_1;
	tdm_chip_legacy_t td2p_chip = tdm_chip_td2p_shim__egr_wrap(_tdm);
	
	_tdm->_core_data.vmap_max_len = TD2P_VMAP_MAX_LEN;
	_tdm->_core_data.vmap_max_wid = TD2P_VMAP_MAX_WID;
	
	_tdm->_core_data.rule__same_port_min = LLS_MIN_SPACING;
	_tdm->_core_data.rule__prox_port_min = VBS_MIN_SPACING;
	
	if (_tdm->_core_data.vars_pkg.pipe>TD2P_PIPE_X_ID) {
		pgw_tdm_tbl_0 = td2p_chip.tdm_pgw.pgw_tdm_tbl_y0;
		pgw_tdm_tbl_1 = td2p_chip.tdm_pgw.pgw_tdm_tbl_y1;
		ovs_tdm_tbl_0 = td2p_chip.tdm_pgw.ovs_tdm_tbl_y0;
		ovs_tdm_tbl_1 = td2p_chip.tdm_pgw.ovs_tdm_tbl_y1;
	}
	else {		
		pgw_tdm_tbl_0 = td2p_chip.tdm_pgw.pgw_tdm_tbl_x0;
		pgw_tdm_tbl_1 = td2p_chip.tdm_pgw.pgw_tdm_tbl_x1;
		ovs_tdm_tbl_0 = td2p_chip.tdm_pgw.ovs_tdm_tbl_x0;
		ovs_tdm_tbl_1 = td2p_chip.tdm_pgw.ovs_tdm_tbl_x1;
	}
	switch (td2p_chip.tdm_globals.clk_freq) {
		case 608:
		case 609: 
			clk=609; acc=TD2P_ACC_PORT_NUM; break;
		case 517:
		case 518: 
			clk=518; acc=(TD2P_ACC_PORT_NUM-2); break;
		case 416:
		case 415:
			clk=415; acc=TD2P_ACC_PORT_NUM; break;
		default:
			clk=760; acc=(TD2P_ACC_PORT_NUM-2); break;
	}
	
	for (idx1=0; idx1<TDM_AUX_SIZE; idx1++) {
		_tdm->_core_data.vars_pkg.lr_buffer[idx1]=TD2P_NUM_EXT_PORTS;
		_tdm->_core_data.vars_pkg.os_buffer[idx1]=TD2P_NUM_EXT_PORTS;
	}
	for (idx1=0; idx1<TD2P_LR_LLS_LEN; idx1++) {
		TD2P_TOKEN_CHECK(pgw_tdm_tbl_0[idx1]) {
			_tdm->_core_data.vars_pkg.lr_buffer[lr_buffer_idx++]=pgw_tdm_tbl_0[idx1];
		}
	}
	for (idx1=0; idx1<TD2P_LR_LLS_LEN; idx1++) {
		TD2P_TOKEN_CHECK(pgw_tdm_tbl_1[idx1]) {
			_tdm->_core_data.vars_pkg.lr_buffer[lr_buffer_idx++]=pgw_tdm_tbl_1[idx1];
		}
	}
	for (idx1=0; idx1<TD2P_OS_LLS_GRP_LEN; idx1++) {
		TD2P_TOKEN_CHECK(ovs_tdm_tbl_0[idx1]) {
			_tdm->_core_data.vars_pkg.os_buffer[os_buffer_idx++]=ovs_tdm_tbl_0[idx1];
		}
	}
	for (idx1=0; idx1<TD2P_OS_LLS_GRP_LEN; idx1++) {
		TD2P_TOKEN_CHECK(ovs_tdm_tbl_1[idx1]) {
			_tdm->_core_data.vars_pkg.os_buffer[os_buffer_idx++]=ovs_tdm_tbl_1[idx1];
		}
	}
	
	if (_tdm->_core_data.vars_pkg.pipe>TD2P_PIPE_X_ID) {
		pipe=1;
		tdm_td2p_parse_quad(td2p_chip.tdm_globals.speed, td2p_chip.tdm_globals.port_rates_array, td2p_chip.pmap, td2p_chip.tdm_globals.pm_encap_type, 65, 128);
	}
	else {
		pipe=0;
		tdm_td2p_parse_quad(td2p_chip.tdm_globals.speed, td2p_chip.tdm_globals.port_rates_array, td2p_chip.pmap, td2p_chip.tdm_globals.pm_encap_type, 1, 64);
	}

	for (idx1=0; idx1<TD2P_NUM_PHY_PORTS; idx1++) {
		if (td2p_chip.tdm_globals.port_rates_array[idx1]==PORT_STATE__LINERATE) {
			for (idx2=0; idx2<(TD2P_NUM_PHY_PORTS/2); idx2++) {
				if (_tdm->_core_data.vars_pkg.os_buffer[idx2]==(idx1+1)) {
					td2p_chip.tdm_globals.port_rates_array[idx1]=PORT_STATE__OVERSUB;
				}
			}
		}
	}

	if ( (td2p_chip.tdm_globals.pm_encap_type[TD2P_NUM_PHY_PM]==PM_ENCAP__HIGIG2 && (pipe==1||pipe==2)) && ((_tdm->_core_data.vars_pkg.lr_buffer[0]!=TD2P_NUM_EXT_PORTS && td2p_chip.tdm_globals.clk_freq>=MIN_HG_FREQ)||(_tdm->_core_data.vars_pkg.lr_buffer[0]==TD2P_NUM_EXT_PORTS)) ) {
		higig_mgmt=BOOL_TRUE;
	}
	if ( (tdm_td2p_check_ethernet(tdm_chip_td2p_shim__check_ethernet(_tdm->_core_data.vars_pkg.lr_buffer[0], td2p_chip.tdm_globals.speed, td2p_chip.pmap, td2p_chip.tdm_globals.pm_encap_type))) && (higig_mgmt==BOOL_FALSE) ) {
		switch (td2p_chip.tdm_globals.clk_freq) {
			case 760: lr_idx_limit=(LEN_760MHZ_EN-acc); break; /* 760MHz (192+8)*2.65G */
			case 609: case 608: lr_idx_limit=(LEN_608MHZ_EN-acc); break; /* 608.333MHz (160+10)*2.65G */
			case 518: case 517: lr_idx_limit=(LEN_517MHZ_EN-acc); break; /* 517.857MHz (128+8)*2.65G */
			case 415: case 416: lr_idx_limit=(LEN_415MHZ_EN-acc); break; /* 415.625MHz (106+10)*2.65G */
			default:
				TDM_ERROR0("Invalid frequency\n");
				return 0;
				break;
		}
	}
	else {
		switch (td2p_chip.tdm_globals.clk_freq) {
			case 760: lr_idx_limit=(LEN_760MHZ_HG-acc); break; /* 760MHz (192+8)*2.65G */
			case 609: case 608: lr_idx_limit=(LEN_608MHZ_HG-acc); break; /* 608.333MHz (160+10)*2.65G */
			case 518: case 517: lr_idx_limit=(LEN_517MHZ_HG-acc); break; /* 517.857MHz (128+8)*2.65G */
			case 415: case 416: lr_idx_limit=(LEN_415MHZ_HG-acc); break; /* 415.625MHz (106+10)*2.65G */
			default:
				TDM_ERROR0("Invalid frequency\n");
				return 0;
				break;
		}
	}
	_tdm->_chip_data.soc_pkg.clk_freq=(_tdm->_chip_data.soc_pkg.clk_freq==clk)?(_tdm->_chip_data.soc_pkg.clk_freq):(clk);
	_tdm->_chip_data.soc_pkg.tvec_size = acc;
	_tdm->_chip_data.soc_pkg.soc_vars.td2p.higig_mgmt = higig_mgmt;
	_tdm->_chip_data.soc_pkg.lr_idx_limit = lr_idx_limit;

	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__COREREQ]( _tdm ) );
}


/**
@name: tdm_td2p_lls_wrapper
@param:

Code wrapper for ingress TDM scheduling
**/
int
tdm_td2p_lls_wrapper( tdm_mod_t *_tdm )
{
	int idx;
	tdm_chip_legacy_t td2p_chip = tdm_chip_td2p_shim__ing_wrap(_tdm);

	TDM_PRINT0("TDM: Linked list round robin\n");
	
	for (idx=0; idx<NUM_OP_FLAGS; idx++) {
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.op_flags_0[idx]=0;
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.op_flags_1[idx]=0;
	}
	for (idx=0; idx<TD2P_NUM_QUAD; idx++) {
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.pgw_num = idx;
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.start_port = (idx*(TD2P_NUM_PHY_PORTS/TD2P_NUM_QUAD));
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.stop_port = ((idx+1)*(TD2P_NUM_PHY_PORTS/TD2P_NUM_QUAD));
		{
			int i, j, first_wc=0, swap_array[TDM_AUX_SIZE], scheduler_state, op_flags_str[2], relink;
			struct ll_node llist;
			td2p_pgw_pntrs_t td2p_pntr_pkg;
			td2p_pgw_scheduler_vars_t td2p_vars_pkg;
			int *pgw_tdm_tbl, *ovs_tdm_tbl, *ovs_spacing;
			
			llist.port=0;
			llist.next=NULL;
			switch(_tdm->_chip_data.soc_pkg.soc_vars.td2p.start_port) {
				case 0:
					pgw_tdm_tbl = td2p_chip.tdm_pgw.pgw_tdm_tbl_x0;
					ovs_tdm_tbl = td2p_chip.tdm_pgw.ovs_tdm_tbl_x0;
					ovs_spacing = td2p_chip.tdm_pgw.ovs_spacing_x0;
					break;
				case 32:
					pgw_tdm_tbl = td2p_chip.tdm_pgw.pgw_tdm_tbl_x1;
					ovs_tdm_tbl = td2p_chip.tdm_pgw.ovs_tdm_tbl_x1;
					ovs_spacing = td2p_chip.tdm_pgw.ovs_spacing_x1;
					break;		
				case 64:
					pgw_tdm_tbl = td2p_chip.tdm_pgw.pgw_tdm_tbl_y0;
					ovs_tdm_tbl = td2p_chip.tdm_pgw.ovs_tdm_tbl_y0;
					ovs_spacing = td2p_chip.tdm_pgw.ovs_spacing_y0;
					break;		
				default:
					pgw_tdm_tbl = td2p_chip.tdm_pgw.pgw_tdm_tbl_y1;
					ovs_tdm_tbl = td2p_chip.tdm_pgw.ovs_tdm_tbl_y1;
					ovs_spacing = td2p_chip.tdm_pgw.ovs_spacing_y1;
					break;
			}
			td2p_pntr_pkg.pgw_tdm_idx=0;
			td2p_pntr_pkg.ovs_tdm_idx=0;
			td2p_pntr_pkg.tdm_stk_idx=0;
			
			for (i=0; i<TDM_AUX_SIZE; i++) {
				swap_array[i] = 0;
			}
			switch(_tdm->_chip_data.soc_pkg.soc_vars.td2p.pgw_num) {
				case 0: first_wc = 0; break;
				case 1: first_wc = 8; break;
				case 2: first_wc = 16; break;
				case 3: first_wc = 24; break;
			}
			td2p_vars_pkg.first_port=_tdm->_chip_data.soc_pkg.soc_vars.td2p.start_port;
			td2p_vars_pkg.last_port=_tdm->_chip_data.soc_pkg.soc_vars.td2p.stop_port;
			/* memcpy(&op_flags, op_flags_str, 1* sizeof(int) );*/
			if (idx<(TD2P_NUM_QUAD/2)) {
				op_flags_str[0]=_tdm->_chip_data.soc_pkg.soc_vars.td2p.op_flags_0[0];
			}
			else {
				op_flags_str[0]=_tdm->_chip_data.soc_pkg.soc_vars.td2p.op_flags_1[0];
			}
			for (j=0; j<4; j++) {
				td2p_vars_pkg.subport=j;
				td2p_pntr_pkg.cur_idx=first_wc;
				td2p_vars_pkg.cur_idx_max=(first_wc+8);
				scheduler_state = tdm_td2p_lls_scheduler(&llist, &td2p_chip, &td2p_pntr_pkg, &td2p_vars_pkg, &pgw_tdm_tbl, &ovs_tdm_tbl, op_flags_str);
				op_flags_str[1]=scheduler_state;
				while (td2p_pntr_pkg.tdm_stk_idx>0) {
					tdm_td2p_ll_append(&llist,swap_array[td2p_pntr_pkg.tdm_stk_idx--],&scheduler_state);
					if (td2p_pntr_pkg.tdm_stk_idx<=0) {
						tdm_td2p_ll_print(&llist);
					}
				}
			}
			td2p_pntr_pkg.pgw_tdm_idx=tdm_td2p_ll_len(&llist);
			relink=BOOL_FALSE;
			for (i=1; i<tdm_td2p_ll_len(&llist); i++) {
				TD2P_TOKEN_CHECK(tdm_td2p_ll_get(&llist,i)) {
					if ( _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](tdm_chip_td2p_shim__which_tsc(tdm_td2p_ll_get(&llist,i),td2p_chip.pmap))==_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](tdm_chip_td2p_shim__which_tsc(tdm_td2p_ll_get(&llist,(i-1)),td2p_chip.pmap)) ) {
						relink=BOOL_TRUE;
						break;
					}
				}
			}
			if (relink) {
				TDM_PRINT0("TDM: Retracing calendar\n");
				tdm_td2p_ll_retrace(&llist,td2p_chip.pmap);
				tdm_td2p_ll_print(&llist);
			}
			relink=BOOL_FALSE;
			for (i=1; i<tdm_td2p_ll_len(&llist); i++) {
				TD2P_TOKEN_CHECK(tdm_td2p_ll_get(&llist,i)) {
					if ( _tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](tdm_chip_td2p_shim__which_tsc(tdm_td2p_ll_get(&llist,i),td2p_chip.pmap))==_tdm->_core_exec[TDM_CORE_EXEC__PM_SCAN](tdm_chip_td2p_shim__which_tsc(tdm_td2p_ll_get(&llist,(i-1)),td2p_chip.pmap)) ) {
						relink=BOOL_TRUE;
						break;
					}
				}
			}
			if ( (tdm_td2p_ll_count(&llist,1)>tdm_td2p_ll_count(&llist,TD2P_OVSB_TOKEN)) || (tdm_td2p_ll_single_100(&llist)) || (relink) ) {
				TDM_PRINT0("TDM: Reweaving calendar\n");
				tdm_td2p_ll_weave(&llist,td2p_chip.pmap,TD2P_OVSB_TOKEN);
				tdm_td2p_ll_print(&llist);
			}
			tdm_td2p_ll_deref(&llist,&pgw_tdm_tbl,64);
			if (idx<(TD2P_NUM_QUAD/2)) {
				if (_tdm->_chip_data.soc_pkg.soc_vars.td2p.op_flags_0[0]==1) {
					for (i=0; i<(td2p_chip.tdm_globals.clk_freq/40); i++) {
						if (pgw_tdm_tbl[i]==TD2P_NUM_EXT_PORTS) {
							pgw_tdm_tbl[i]=TD2P_OVSB_TOKEN;
						}
					}
				}
			}
			else {
				if (_tdm->_chip_data.soc_pkg.soc_vars.td2p.op_flags_1[0]==1) {
					for (i=0; i<(td2p_chip.tdm_globals.clk_freq/40); i++) {
						if (pgw_tdm_tbl[i]==TD2P_NUM_EXT_PORTS) {
							pgw_tdm_tbl[i]=TD2P_OVSB_TOKEN;
						}
					}
				}			
			}
			TDM_SML_BAR
			tdm_td2p_print_tbl(pgw_tdm_tbl,TD2P_LR_LLS_LEN,"PGW Main Calendar",idx);
			if (ovs_tdm_tbl[0]!=TD2P_NUM_EXT_PORTS) {
				tdm_td2p_ovs_spacer(&td2p_chip.pmap,ovs_tdm_tbl,ovs_spacing);
				tdm_td2p_print_tbl_ovs(ovs_tdm_tbl,ovs_spacing,TD2P_OS_LLS_GRP_LEN,"PGW Oversub Calendar",idx);
				for (i=0; i<TD2P_OS_LLS_GRP_LEN; i++) {
					if (ovs_tdm_tbl[i]!=TD2P_NUM_EXT_PORTS && _tdm->_chip_data.soc_pkg.state[ovs_tdm_tbl[i]]==PORT_STATE__LINERATE) {
						_tdm->_chip_data.soc_pkg.state[ovs_tdm_tbl[i]]=PORT_STATE__OVERSUB;
					}
				}
				tdm_print_stat( _tdm );
			}
		}
	}
	
	/* Bind to TDM class object */
	TDM_COPY(_tdm->_chip_data.cal_0.cal_main,td2p_chip.tdm_pgw.pgw_tdm_tbl_x0,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_0.cal_grp[0],td2p_chip.tdm_pgw.ovs_tdm_tbl_x0,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_0.cal_grp[1],td2p_chip.tdm_pgw.ovs_spacing_x0,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_1.cal_main,td2p_chip.tdm_pgw.pgw_tdm_tbl_x1,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_1.cal_grp[0],td2p_chip.tdm_pgw.ovs_tdm_tbl_x1,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_1.cal_grp[1],td2p_chip.tdm_pgw.ovs_spacing_x1,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_2.cal_main,td2p_chip.tdm_pgw.pgw_tdm_tbl_y0,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_2.cal_grp[0],td2p_chip.tdm_pgw.ovs_tdm_tbl_y0,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_2.cal_grp[1],td2p_chip.tdm_pgw.ovs_spacing_y0,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_3.cal_main,td2p_chip.tdm_pgw.pgw_tdm_tbl_y1,sizeof(int)*TD2P_LR_LLS_LEN);
	TDM_COPY(_tdm->_chip_data.cal_3.cal_grp[0],td2p_chip.tdm_pgw.ovs_tdm_tbl_y1,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	TDM_COPY(_tdm->_chip_data.cal_3.cal_grp[1],td2p_chip.tdm_pgw.ovs_spacing_y1,sizeof(int)*TD2P_OS_LLS_GRP_LEN);
	
	/* Realign port state array to old specification */	
	for (idx=0; idx<((_tdm->_chip_data.soc_pkg.num_ext_ports)-1); idx++) {
		_tdm->_chip_data.soc_pkg.state[idx] = _tdm->_chip_data.soc_pkg.state[idx+1];
	}		
	
	_tdm->_core_data.vars_pkg.pipe=TD2P_PIPE_X_ID;
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__EGRESS_WRAP]( _tdm ) );
}


/**
@name: tdm_td2p_pmap_transcription
@param:

For Trident2+ BCM56850
Transcription algorithm for generating port module mapping

	40G 	- xxxx
      	20G 	- xx
	      	    xx
      	10G	- x
		   x
		    x
		     x
     	1X0G 	- xxxx_xxxx_xxxx
**/
int
tdm_td2p_pmap_transcription( tdm_mod_t *_tdm )
{
	int i, j, last_port=TD2P_NUM_EXT_PORTS, tsc_active;
	/*sprintf (cmd_str, "soc setmem -index %x %s {", index->d, addr);*/
	
	for (i=1; i<(TD2P_NUM_EXT_PORTS-1); i+=TD2P_NUM_PM_LNS) {
		tsc_active=BOOL_FALSE;
		for (j=0; j<TD2P_NUM_PM_LNS; j++) {
			if ( _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__LINERATE || _tdm->_chip_data.soc_pkg.state[i+j]==PORT_STATE__OVERSUB ) {
				tsc_active=BOOL_TRUE;
				break;
			}
		}
		if ( ((i-1)/TD2P_NUM_PM_LNS==3) && _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][0]==TD2P_MGMT_TOKEN ) {
			if ( _tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+1] ) {
				TDM_MSET(_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS],TD2P_MGMT_TOKEN,TD2P_NUM_PM_LNS);
			}
		}
		else if (tsc_active) {
			if ( _tdm->_chip_data.soc_pkg.speed[i]<=SPEED_42G_HG2 || _tdm->_chip_data.soc_pkg.state[i-1]==PORT_STATE__DISABLED ) {
				for (j=0; j<TD2P_NUM_PM_LNS; j++) {
					switch (_tdm->_chip_data.soc_pkg.state[i+j]) {
						case PORT_STATE__LINERATE: case PORT_STATE__OVERSUB:
							_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][j]=(i+j);
							last_port=(i+j);
							break;
						case PORT_STATE__COMBINE:
							_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][j]=last_port;
							break;
						default:
							_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][j]=TD2P_NUM_EXT_PORTS;
							break;
					}
				}
				if ( _tdm->_chip_data.soc_pkg.speed[i]>_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i+2]==_tdm->_chip_data.soc_pkg.speed[i+3] && _tdm->_chip_data.soc_pkg.speed[i+2]!=SPEED_0 ) {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2];
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][0];
				}
				else if ( _tdm->_chip_data.soc_pkg.speed[i]==_tdm->_chip_data.soc_pkg.speed[i+1] && _tdm->_chip_data.soc_pkg.speed[i]<_tdm->_chip_data.soc_pkg.speed[i+2] && _tdm->_chip_data.soc_pkg.speed[i]!=SPEED_0 )  {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1];
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = _tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][3];
				}
				if (_tdm->_chip_data.soc_pkg.speed[i]==SPEED_20G&&_tdm->_chip_data.soc_pkg.speed[i+2]==SPEED_0&&_tdm->_chip_data.soc_pkg.state[i+2]==3) {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = TD2P_NUM_EXT_PORTS;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][3] = TD2P_NUM_EXT_PORTS;
				}
			}
			else {
				if ( _tdm->_chip_data.soc_pkg.speed[i]==SPEED_100G && _tdm->_chip_data.soc_pkg.state[i+10]!=PORT_STATE__COMBINE ) {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][2] = TD2P_NUM_EXT_PORTS;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][3] = TD2P_NUM_EXT_PORTS;
				}
				else if ( _tdm->_chip_data.soc_pkg.speed[i]==SPEED_100G || _tdm->_chip_data.soc_pkg.speed[i]==SPEED_120G ) {
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][0] = i;	
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i-1)/TD2P_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+3)/TD2P_NUM_PM_LNS][3] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][0] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][1] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][2] = i;
					_tdm->_chip_data.soc_pkg.pmap[(i+7)/TD2P_NUM_PM_LNS][3] = i;
				}
				i+=8;
			}
		}
	}
	tdm_print_stat( _tdm );
	_tdm->_chip_data.soc_pkg.soc_vars.td2p.mgmtbw=0;
	for (i=0; i<TD2P_NUM_PM_LNS; i++) {
		if (_tdm->_chip_data.soc_pkg.pmap[TD2P_MGMT_TSC_ID][i]==TD2P_MGMT_TOKEN) {
			((_tdm->_chip_data.soc_pkg.soc_vars.td2p.mgmtbw)++);
		}
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__INGRESS_WRAP]( _tdm ) );
}


/**
@name: tdm_chip_td2p_init
@param:

**/
int
tdm_td2p_init( tdm_mod_t *_tdm )
{
	int index;
	
	_tdm->_chip_data.soc_pkg.pmap_num_modules = TD2P_NUM_PM_MOD;
	_tdm->_chip_data.soc_pkg.pmap_num_lanes = TD2P_NUM_PM_LNS;
	_tdm->_chip_data.soc_pkg.pm_num_phy_modules = TD2P_NUM_PHY_PM;
	
	_tdm->_chip_data.soc_pkg.soc_vars.ovsb_token = TD2P_OVSB_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl1_token = TD2P_IDL1_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.idl2_token = TD2P_IDL2_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.ancl_token = TD2P_ANCL_TOKEN;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo = 1;
	_tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi = TD2P_NUM_PHY_PORTS;
	
	_tdm->_chip_data.cal_0.cal_len = TD2P_LR_LLS_LEN;
	_tdm->_chip_data.cal_0.grp_num = TD2P_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_0.grp_len = TD2P_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_1.cal_len = TD2P_LR_LLS_LEN;
	_tdm->_chip_data.cal_1.grp_num = TD2P_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_1.grp_len = TD2P_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_2.cal_len = TD2P_LR_LLS_LEN;
	_tdm->_chip_data.cal_2.grp_num = TD2P_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_2.grp_len = TD2P_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_3.cal_len = TD2P_LR_LLS_LEN;
	_tdm->_chip_data.cal_3.grp_num = TD2P_OS_LLS_GRP_NUM;
	_tdm->_chip_data.cal_3.grp_len = TD2P_OS_LLS_GRP_LEN;
	_tdm->_chip_data.cal_4.cal_len = TD2P_LR_VBS_LEN;
	_tdm->_chip_data.cal_4.grp_num = TD2P_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_4.grp_len = TD2P_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_5.cal_len = TD2P_LR_VBS_LEN;
	_tdm->_chip_data.cal_5.grp_num = TD2P_OS_VBS_GRP_NUM;
	_tdm->_chip_data.cal_5.grp_len = TD2P_OS_VBS_GRP_LEN;
	_tdm->_chip_data.cal_6.cal_len = TD2P_LR_IARB_STATIC_LEN;
	_tdm->_chip_data.cal_6.grp_num = 0;
	_tdm->_chip_data.cal_6.grp_len = 0;
	_tdm->_chip_data.cal_7.cal_len = TD2P_LR_IARB_STATIC_LEN;
	_tdm->_chip_data.cal_7.grp_num = 0;
	_tdm->_chip_data.cal_7.grp_len = 0;
	
	for (index=0; index<TD2P_NUM_PM_MOD; index++) {
		_tdm->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[index] = (_tdm->_chip_data.soc_pkg.state[index*4]==PORT_STATE__LINERATE_HG||_tdm->_chip_data.soc_pkg.state[index]==PORT_STATE__OVERSUB_HG)?(999):(998);
	}
	_tdm->_chip_data.soc_pkg.pmap=(int **) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_modules)*sizeof(int *), "portmod_map_l1");
	for (index=0; index<(_tdm->_chip_data.soc_pkg.pmap_num_modules); index++) {
		_tdm->_chip_data.soc_pkg.pmap[index]=(int *) TDM_ALLOC((_tdm->_chip_data.soc_pkg.pmap_num_lanes)*sizeof(int), "portmod_map_l2");
		TDM_MSET(_tdm->_chip_data.soc_pkg.pmap[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.soc_pkg.pmap_num_lanes);
	}
	/* PGW x0 calendar group */
	_tdm->_chip_data.cal_0.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.cal_len)*sizeof(int), "TDM inst 0 main calendar");
	TDM_MSET(_tdm->_chip_data.cal_0.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.cal_len);
	_tdm->_chip_data.cal_0.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_num)*sizeof(int *), "TDM inst 0 groups");
	for (index=0; index<(_tdm->_chip_data.cal_0.grp_num); index++) {
		_tdm->_chip_data.cal_0.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_0.grp_len)*sizeof(int), "TDM inst 0 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_0.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_0.grp_len);
	}
	/* PGW x1 calendar group */
	_tdm->_chip_data.cal_1.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.cal_len)*sizeof(int), "TDM inst 1 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_1.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.cal_len);
	_tdm->_chip_data.cal_1.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_num)*sizeof(int *), "TDM inst 1 groups");
	for (index=0; index<(_tdm->_chip_data.cal_1.grp_num); index++) {
		_tdm->_chip_data.cal_1.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_1.grp_len)*sizeof(int), "TDM inst 1 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_1.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_1.grp_len);
	}
	/* PGW y0 calendar group */
	_tdm->_chip_data.cal_2.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.cal_len)*sizeof(int), "TDM inst 2 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_2.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.cal_len);
	_tdm->_chip_data.cal_2.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_num)*sizeof(int *), "TDM inst 2 groups");
	for (index=0; index<(_tdm->_chip_data.cal_2.grp_num); index++) {
		_tdm->_chip_data.cal_2.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_2.grp_len)*sizeof(int), "TDM inst 2 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_2.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_2.grp_len);
	}
	/* PGW y1 calendar group */
	_tdm->_chip_data.cal_3.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.cal_len)*sizeof(int), "TDM inst 3 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_3.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.cal_len);
	_tdm->_chip_data.cal_3.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_num)*sizeof(int *), "TDM inst 3 groups");
	for (index=0; index<(_tdm->_chip_data.cal_3.grp_num); index++) {
		_tdm->_chip_data.cal_3.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_3.grp_len)*sizeof(int), "TDM inst 3 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_3.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_3.grp_len);
	}
	/* MMU x pipe calendar group */
	_tdm->_chip_data.cal_4.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.cal_len)*sizeof(int), "TDM inst 4 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_4.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.cal_len);
	_tdm->_chip_data.cal_4.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_num)*sizeof(int *), "TDM inst 4 groups");
	for (index=0; index<(_tdm->_chip_data.cal_4.grp_num); index++) {
		_tdm->_chip_data.cal_4.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_4.grp_len)*sizeof(int), "TDM inst 4 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_4.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_4.grp_len);
	}
	/* MMU y pipe calendar group */
	_tdm->_chip_data.cal_5.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.cal_len)*sizeof(int), "TDM inst 5 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_5.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.cal_len);
	_tdm->_chip_data.cal_5.cal_grp=(int **) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_num)*sizeof(int *), "TDM inst 5 groups");
	for (index=0; index<(_tdm->_chip_data.cal_5.grp_num); index++) {
		_tdm->_chip_data.cal_5.cal_grp[index]=(int *) TDM_ALLOC((_tdm->_chip_data.cal_5.grp_len)*sizeof(int), "TDM inst 5 group calendars");
		TDM_MSET(_tdm->_chip_data.cal_5.cal_grp[index],(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_5.grp_len);
	}
	/* IARB static calendar group */
	_tdm->_chip_data.cal_6.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_6.cal_len)*sizeof(int), "TDM inst 6 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_6.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_6.cal_len);
	_tdm->_chip_data.cal_7.cal_main=(int *) TDM_ALLOC((_tdm->_chip_data.cal_7.cal_len)*sizeof(int), "TDM inst 7 main calendar");	
	TDM_MSET(_tdm->_chip_data.cal_7.cal_main,(_tdm->_chip_data.soc_pkg.num_ext_ports),_tdm->_chip_data.cal_7.cal_len);

	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__TRANSCRIPTION]( _tdm ) );
}


/**
@name: tdm_td2p_post
@param:

**/
int
tdm_td2p_post( tdm_mod_t *_tdm )
{
	return PASS;
}
