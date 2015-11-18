/*
 * $Id: tdm_td2p_scan.c.$
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
 * TDM chip data structure scanning functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_td2p_which_tsc
@param:

Returns the TSC to which the input port belongs given pointer to transcribed pmap
**/
int
tdm_td2p_which_tsc( tdm_mod_t *_tdm_s )
{
	TD2P_TOKEN_CHECK(_tdm_s->_core_data.vars_pkg.port) {
		return tdm_find_pm( _tdm_s );
	}

	return TD2P_NUM_EXT_PORTS;
	
}


/**
@name: tdm_td2p_legacy_which_tsc
@param:

Returns the TSC to which the input port belongs given pointer to transcribed pmap
**/
int
tdm_td2p_legacy_which_tsc(unsigned char port, int **tsc)
{
	int i, j, which=TD2P_NUM_EXT_PORTS;
	
	TD2P_TOKEN_CHECK(port) {
		for (i=0; i<TD2P_NUM_PHY_PM; i++) {
			for (j=0; j<TD2P_NUM_PM_LNS; j++) {
				if (tsc[i][j]==port) {
					which=i;
				}
			}
			if (which!=TD2P_NUM_EXT_PORTS) {
				break;
			}
		}
	}
	
	return which;
	
}


/**
@name: tdm_td2p_check_ethernet
@param:

Returns BOOL_TRUE or BOOL_FALSE depending on if pipe of the given port has traffic entirely Ethernet
**/
int
tdm_td2p_check_ethernet( tdm_mod_t *_tdm_s )
{
	int i, j, type=BOOL_TRUE, tsc[TD2P_NUM_PHY_PM][TD2P_NUM_PM_LNS];
	
	for (i=0; i<TD2P_NUM_PHY_PM; i++) {
		for (j=0; j<TD2P_NUM_PM_LNS; j++) {
			tsc[i][j]=_tdm_s->_chip_data.soc_pkg.pmap[i][j];
		}
	}
	if (_tdm_s->_core_data.vars_pkg.port<33) {
		for (i=1; i<33; i++) {
			if (_tdm_s->_chip_data.soc_pkg.speed[i]!=SPEED_0 && _tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[tdm_td2p_which_tsc(tdm_chip_td2p_shim__which_tsc(i,tsc))]==PM_ENCAP__HIGIG2) {
				type=BOOL_FALSE;
				break;
			}
		}
	}
	else if (_tdm_s->_core_data.vars_pkg.port<65) {
		for (i=33; i<65; i++) {
			if (_tdm_s->_chip_data.soc_pkg.speed[i]!=SPEED_0 && _tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[tdm_td2p_which_tsc(tdm_chip_td2p_shim__which_tsc(i,tsc))]==PM_ENCAP__HIGIG2) {
				type=BOOL_FALSE;
				break;
			}
		}
	}
	else if (_tdm_s->_core_data.vars_pkg.port<97) {
		for (i=65; i<97; i++) {
			if (_tdm_s->_chip_data.soc_pkg.speed[i]!=SPEED_0 && _tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[tdm_td2p_which_tsc(tdm_chip_td2p_shim__which_tsc(i,tsc))]==PM_ENCAP__HIGIG2) {
				type=BOOL_FALSE;
				break;
			}
		}	
	}
	else {
		for (i=97; i<129; i++) {
			if (_tdm_s->_chip_data.soc_pkg.speed[i]!=SPEED_0 && _tdm_s->_chip_data.soc_pkg.soc_vars.td2p.pm_encap_type[tdm_td2p_which_tsc(tdm_chip_td2p_shim__which_tsc(i,tsc))]==PM_ENCAP__HIGIG2) {
				type=BOOL_FALSE;
				break;
			}
		}	
	}

	return type;
	
}


/**
@name: tdm_td2p_check_same_port_dist_dn
@param:

Returns distance to next index with same port number, in down direction
Wraparound without mirroring
**/
int
tdm_td2p_check_same_port_dist_dn(int idx, int *tdm_tbl, int lim)
{
	int j, dist=1, slot;

	slot=idx;
	for (j=1; j<lim; j++) {
		if (++slot==lim) {slot=0;}
		if (tdm_tbl[slot]==tdm_tbl[idx]) {
			break;
		}
		dist++;
	}

	return dist;

}


/**
@name: tdm_td2p_check_same_port_dist_up
@param:

Returns distance to next index with same port number, in down direction
Wraparound without mirroring
**/
int
tdm_td2p_check_same_port_dist_up(int idx, int *tdm_tbl, int lim)
{
	int j, dist=1, slot;

	slot=idx;
	for (j=1; j<lim; j++) {
		if (--slot<=0) {slot=(lim-1);}
		if (tdm_tbl[slot]==tdm_tbl[idx]) {
			break;
		}
		dist++;
	}

	return dist;

}


/**
@name: tdm_td2p_check_same_port_dist_dn_port
@param:

Returns distance to next index with same port number, in down direction
Wraparound without mirroring
**/
int
tdm_td2p_check_same_port_dist_dn_port(int port, int idx, int *tdm_tbl, int lim)
{
	int j, dist=1, slot;

	slot=idx;
	for (j=1; j<lim; j++) {
		if (++slot==lim) {slot=0;}
		if (tdm_tbl[slot]==port) {
			break;
		}
		dist++;
	}

	return dist;
}


/**
@name: tdm_td2p_check_same_port_dist_up_port
@param:

Returns distance to next index with same port number, in down direction
Wraparound without mirroring
**/
int
tdm_td2p_check_same_port_dist_up_port(int port, int idx, int *tdm_tbl, int lim)
{
	int j, dist=1, slot;

	slot=idx;
	for (j=1; j<lim; j++) {
		if (--slot<=0) {slot=(lim-1);}
		if (tdm_tbl[slot]==port) {
			break;
		}
		dist++;
	}

	return dist;
}


/**
@name: tdm_td2p_slice_size_local
@param:

Given index, returns size of largest contiguous slice
**/
int
tdm_td2p_slice_size_local(unsigned char idx, int *tdm, int lim)
{
	int i, slice_size=(-1);
	
	if (tdm[idx]!=TD2P_OVSB_TOKEN && tdm[idx]!=TD2P_NUM_EXT_PORTS) {
		for (i=idx; i>=0; i--) {
			if (tdm[i]!=TD2P_OVSB_TOKEN && tdm[i]!=TD2P_NUM_EXT_PORTS) {
				slice_size++;
			}
			else {
				break;
			}
		}
		for (i=idx; i<lim; i++) {
			if (tdm[i]!=TD2P_OVSB_TOKEN && tdm[i]!=TD2P_NUM_EXT_PORTS) {
				slice_size++;
			}
			else {
				break;
			}
		}
	}
	else if (tdm[idx]==TD2P_OVSB_TOKEN) {
		for (i=idx; i>=0; i--) {
			if (tdm[i]==TD2P_OVSB_TOKEN) {
				slice_size++;
			}
			else {
				break;
			}
		}
		for (i=idx; i<lim; i++) {
			if (tdm[i]==TD2P_OVSB_TOKEN) {
				slice_size++;
			}
			else {
				break;
			}
		}
	}

	return slice_size;
}


/**
@name: tdm_td2p_slice_size
@param:

Given port number, returns size of largest slice
**/
int
tdm_td2p_slice_size(unsigned char port, int *tdm, int lim)
{
	int i, j, k=0, slice_size=0;
	
	if (port<129 && port>0) {
		for (i=0; i<lim; i++) {
			TD2P_TOKEN_CHECK(tdm[i]) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					TD2P_TOKEN_CHECK(tdm[j]) {k++;}
					else {break;}
				}
			}
			slice_size = (k>slice_size)?(k):(slice_size);
		}
	}
	else {
		for (i=2; i<lim; i++) {
			if (tdm[i]==port) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					if (tdm[j]==port) {k++;}
					else {break;}
				}
			}
			slice_size = (k>slice_size)?(k):(slice_size);
		}
	}

	return slice_size;
}


/**
@name: tdm_td2p_slice_idx
@param:

Given port number, returns index of largest slice
**/
int
tdm_td2p_slice_idx(unsigned char port, int *tdm, int lim)
{
	int i, j, k=0, slice_size=0, slice_idx=0;
	
	if (port<129 && port>0) {
		for (i=0; i<lim; i++) {
			TD2P_TOKEN_CHECK(tdm[i]) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					TD2P_TOKEN_CHECK(tdm[j]) {k++;}
					else {break;}
				}
			}
			if (k>slice_size) {
				slice_idx=i;
				slice_size=k;
			}
		}
	}
	else {
		for (i=2; i<lim; i++) {
			if (tdm[i]==port) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					if (tdm[j]==port) {k++;}
					else {break;}
				}
			}
			if (k>slice_size) {
				slice_idx=i;
				slice_size=k;
			}
		}
	}
	
	return slice_idx;
}


/**
@name: tdm_td2p_slice_prox_dn
@param:

Given port number, checks min spacing in a slice in down direction
**/
int
tdm_td2p_slice_prox_dn(int slot, int *tdm, int lim, int **tsc, enum port_speed_e *speed)
{
	int i, cnt=0, wc, idx=(slot+1), slice_prox=PASS;
	
	wc=(tdm[slot]==TD2P_ANCL_TOKEN)?(tdm[slot]):(tdm_td2p_legacy_which_tsc(tdm[slot],tsc));
	if (slot<=(lim-5)) {
		if ( wc==tdm_td2p_legacy_which_tsc(tdm[slot+1],tsc) ||
		     wc==tdm_td2p_legacy_which_tsc(tdm[slot+2],tsc) ||
		     wc==tdm_td2p_legacy_which_tsc(tdm[slot+3],tsc) ||
			 wc==tdm_td2p_legacy_which_tsc(tdm[slot+4],tsc) ) {
			slice_prox=FAIL;
		}
	}
	else {
		while (idx<lim) {
			if (wc==tdm_td2p_legacy_which_tsc(tdm[idx],tsc)) {
				slice_prox=FAIL;
				break;
			}
			idx++; cnt++;
		}
		for (i=(lim-slot-cnt-1); i>=0; i--) {
			if (wc==tdm_td2p_legacy_which_tsc(tdm[i],tsc)) {
				slice_prox=FAIL;
				break;
			}
		}
	}
/* #ifdef _LLS_SCHEDULER */
	{
		int i=slot, j;
		if (speed[tdm[i]]<=SPEED_42G_HG2) {
			if (i<(TD2P_VMAP_MAX_LEN-1)) {
				for (j=1; j<11; j++) {
					if (tdm[i+j]==tdm[i]) {
						slice_prox=FAIL;
						break;
					}
				}
			}
		}
	}
/* #endif */

	return slice_prox;
}


/**
@name: tdm_td2p_slice_prox_up
@param:

Given port number, checks min spacing in a slice in up direction
**/
int
tdm_td2p_slice_prox_up(int slot, int *tdm, int **tsc, enum port_speed_e *speed)
{
	int wc, slice_prox=PASS;
	
	wc=(tdm[slot]==TD2P_ANCL_TOKEN)?(tdm[slot]):(tdm_td2p_legacy_which_tsc(tdm[slot],tsc));
	if (slot>=4) {
		if ( wc==tdm_td2p_legacy_which_tsc(tdm[slot-1],tsc) ||
		     wc==tdm_td2p_legacy_which_tsc(tdm[slot-2],tsc) ||
		     wc==tdm_td2p_legacy_which_tsc(tdm[slot-3],tsc) ||
			 wc==tdm_td2p_legacy_which_tsc(tdm[slot-4],tsc) ) {
			slice_prox=FAIL;
		}
	}
/* #ifdef _LLS_SCHEDULER */
	{
		int i=slot, j;
		if (speed[tdm[i]]<=SPEED_42G_HG2) {
			if (i>=1) {
				for (j=1; j<11; j++) {
					if (tdm[i-j]==tdm[i]) {
						slice_prox=FAIL;
						break;
					}
				}
			}
		}
	}
/* #endif */

	return slice_prox;
}


/**
@name: tdm_td2p_check_fit_smooth
@param:

Inside of table array, returns number of nodes inside a port vector that clump with other nodes of the same type
**/
int
tdm_td2p_check_fit_smooth(int *tdm_tbl, int port, int lr_idx_limit, int clump_thresh)
{
	int i, cnt=0;

	for (i=0; i<lr_idx_limit; i++) {
		if ( (tdm_tbl[i]==port) && (tdm_td2p_slice_size_local(i,tdm_tbl,lr_idx_limit)>=clump_thresh) ) {
			cnt++;
		}
	}

	return cnt;

}


/**
@name: tdm_td2p_check_lls_flat_up
@param:

Checks LLS scheduler min spacing in tdm array, up direction only, returns dist
**/
int
tdm_td2p_check_lls_flat_up(int idx, int *tdm_tbl, enum port_speed_e *speed)
{
	int lls_prox=TD2P_VMAP_MAX_LEN;

/* #ifdef _LLS_SCHEDULER */
	{
		int i=idx, j;
		lls_prox=1;
		if (i>=11 && tdm_tbl[idx]<=SPEED_42G_HG2) {
			for (j=1; j<11; j++) {
				if (tdm_tbl[i-j]==tdm_tbl[i]) {
					break;
				}
				lls_prox++;
			}
		}
	}
/* #endif */

	return lls_prox;

}


/**
@name: tdm_td2p_slice_prox_local
@param:

Given index, checks min spacing of two nearest non-token ports
**/
int
tdm_td2p_slice_prox_local(unsigned char idx, int *tdm, int lim, int **tsc)
{
	int i, prox_len=0, wc=TD2P_NUM_EXT_PORTS;
	
	/* Nearest non-token port */
	TD2P_TOKEN_CHECK(tdm[idx]) {
		wc=tdm_td2p_legacy_which_tsc(tdm[idx],tsc);
	}
	else {
		for (i=1; (idx-i)>=0; i++) {
			TD2P_TOKEN_CHECK(tdm[i]) {
				wc=tdm_td2p_legacy_which_tsc(tdm[idx-i],tsc);
				break;
			}
		}
	}
	for (i=1; (idx+i)<lim; i++) {
		if (tdm_td2p_legacy_which_tsc(tdm[idx+i],tsc)!=wc) {
			prox_len++;
		}
		else {
			break;
		}
	}

	return prox_len;
}


/**
@name: tdm_td2p_num_lr_slots
@param:

**/
int
tdm_td2p_num_lr_slots(int *tdm_tbl)
{
	int i, cnt=0;
	
	for (i=0; i<TD2P_VMAP_MAX_LEN; i++) {
		TD2P_TOKEN_CHECK(tdm_tbl[i]) {
			cnt++;
		}
	}
	
	return cnt;
}
