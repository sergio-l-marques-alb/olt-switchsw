/*
 * $Id: tdm_th_main.c.$
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
 * TDM chip main functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_th_chk_print_config
@param:

**/
void tdm_th_chk_print_config(enum port_speed_e speed[TH_NUM_EXT_PORTS], enum port_state_e state[TH_NUM_EXT_PORTS], int traffic[TH_NUM_PHY_PM+1])
{
	int i, tsc_idx, lgc_idx=0, config_pass=BOOL_TRUE;
	TDM_SML_BAR
	TDM_SML_BAR
	TDM_PRINT0("Port Info based on config.bcm (SelfCheck) \n");
	TDM_SML_BAR
	TDM_PRINT6("%4s%4s%8s%8s%8s%8s\n", "lgc", "phy", "spd","state", "encap", "traffic");
	
	for (i=0;i<TH_NUM_EXT_PORTS;i++){
			if (speed[i]>0) {lgc_idx++;}
			TDM_PRINT1("%4d", lgc_idx);
			TDM_PRINT1("%4d", i);
			TDM_PRINT1("%8d", speed[i]/1000);
			if (i>0){
				switch (state[i-1]){
					case PORT_STATE__LINERATE:
						TDM_PRINT2("%8s%8s","[LNRT]","[ETHN]");
						break;
					case PORT_STATE__OVERSUB:
						TDM_PRINT2("%8s%8s","[OVSB]", "[ETHN]");
						break;
					case PORT_STATE__LINERATE_HG:
						TDM_PRINT2("%8s%8s","[LNRT]", "[HIG2]");
						break;
					case PORT_STATE__OVERSUB_HG:
						TDM_PRINT2("%8s%8s","[OVSB]", "[HIG2]");
						break;
					default:
						TDM_PRINT2("%8s%8s","[----]","[----]");
						break;
				}
				tsc_idx = (i-1)/4;
				if (tsc_idx <=TH_NUM_PHY_PM){
					switch(traffic[tsc_idx]){
						case PM_ENCAP__ETHRNT:
							TDM_PRINT1("%8s", "[ETHN]");
							break;
						case PM_ENCAP__HIGIG2:
							TDM_PRINT1("%8s", "[HIG2]");
							break;
						default:
							TDM_PRINT1("%8s", "[----]");
							break;
					}
					
					if ( (speed[i]/1000>0) &&
					     ((traffic[tsc_idx]==PM_ENCAP__HIGIG2 && state[i-1]!=PORT_STATE__LINERATE_HG && state[i-1]!=PORT_STATE__OVERSUB_HG) ||
					      (traffic[tsc_idx]==PM_ENCAP__ETHRNT && state[i-1]!=PORT_STATE__LINERATE    && state[i-1]!=PORT_STATE__OVERSUB   ) ) ){
						TDM_PRINT1("\t%s", "[XXXX]");
						config_pass = BOOL_FALSE;
					}
				}
			}
			TDM_PRINT0("\n");
		/* } */
		if(i>0 && i%4==0){TDM_PRINT0("\n");}
	}
	TDM_PRINT0("\n");
	if (config_pass==BOOL_FALSE){TDM_PRINT0("TDM ERROR: config.bcm failed, encap type violation\n");}
	TDM_BIG_BAR
}


/**
@name: tdm_th_chk_tdm_shim_which_tsc
@param:

Returns the TSC to which the input port belongs given pointer to transcribed pmap
**/
int
tdm_th_chk_tdm_shim_which_tsc(int port, int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS])
{
	int i, j, which=TH_NUM_EXT_PORTS;
	
	TH_TOKEN_CHECK(port) {
		for (i=0; i<TH_NUM_PHY_PM; i++) {
			for (j=0; j<TH_NUM_PM_LNS; j++) {
				if (tsc[i][j]==port) {
					which=i;
				}
			}
			if (which!=TH_NUM_EXT_PORTS) {
				break;
			}
		}
	}
	
	return which;
	
}


/**
@name: tdm_th_chk_tdm_shim_check_ethernet
@param: int, enum[], int[][], int[]

Returns BOOL_TRUE or BOOL_FALSE depending on if pipe of the given port has traffic entirely Ethernet.
**/
int tdm_th_chk_tdm_shim_check_ethernet(int port, enum port_speed_e speed[TH_NUM_EXT_PORTS], int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS], int traffic[TH_NUM_PHY_PM+1]) {

	int i, idx_s=0, idx_e=0, tsc_idx, type=BOOL_TRUE;
	
	if      (port<=32)  {idx_s = 1; idx_e =32;}
	else if (port<=64)  {idx_s =33; idx_e =64;}
	else if (port<=96)  {idx_s =65; idx_e =96;}
	else if (port<=128) {idx_s =97;	idx_e =128;}
	
	for (i=idx_s; i<=idx_e; i++) {
		tsc_idx = tdm_th_chk_tdm_shim_which_tsc(i,tsc);
		if (tsc_idx <= TH_NUM_PHY_PM){
			if (speed[i]!=SPEED_0 && traffic[tsc_idx]!=PM_ENCAP__ETHRNT) {
				type=BOOL_FALSE;
				break;
			}
		}
	}

	return type;
	
}


/**
@name: tdm_th_chk_tdm_lr_jitter
@param:

Checks jitter of line rate ports
**/
void tdm_th_chk_tdm_lr_jitter(int *tdm, int *tracker, int freq, int len, enum port_speed_e speed[TH_NUM_EXT_PORTS], int id, int fail[7])
{
	int i, j, up, down, sum=0, num=1, avg=0, rate=0, ports_done[128], yy=0, exists, jitter=0, spd_norm=0;
	const char *table;
	
	for (i=0; i<128; i++){ports_done[i]=-1;}
	
	fail[6] = PASS;
	fail[5] = (fail[5]==UNDEF || fail[5]==PASS) ? (PASS):(fail[5]);
	
	switch (id) {
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;
		case 2: table = "IDB Pipe 2"; break;
		case 3: table = "IDB Pipe 3"; break;
		case 4: table = "MMU Pipe 0"; break;
		case 5: table = "MMU Pipe 1"; break;
		case 6: table = "MMU Pipe 2"; break;
		case 7: table = "MMU Pipe 3"; break;	
		default:
			table = "Unrecognized Pipe ID";
			fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(UNDEF);
			fail[6] = (fail[6]==FAIL || fail[6]==UNDEF) ? (fail[6]):(UNDEF);
			TDM_ERROR1("Line rate jitter check failed, unrecognized table ID %0d\n",id);
			break;
	}
		
	for (i=len; i<(2*len); i++) {
		if (tracker[i-len]!=0) {
			exists=BOOL_FALSE;
			for (j=0; j<=yy; j++) {
				if (ports_done[j]==tdm[i]) {
					exists=BOOL_TRUE;
				}
			}
			if (exists==BOOL_FALSE) {
				ports_done[yy]=tdm[i]; yy++;
				sum=0; num=0;
				for (j=0; j<len; j++) {
					if (tdm[j]==tdm[i]) {
						++num;
						sum+=tracker[j];
					}
				}
				switch((speed[tdm[i]]/1000)) {
					case 10:
					case 11:
						jitter = JITTER_THRESH_HI;
						break;
					case 20:
					case 21:
					case 25:
					case 27:
						jitter = JITTER_THRESH_MH;
						break;
					case 40:
					case 42:
					case 50:
					case 53:
						jitter = JITTER_THRESH_ML;
						break;
					case 100:
					case 106:
						jitter = JITTER_THRESH_LO;
						break;
					default:
						fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(UNDEF);
						fail[6] = (fail[6]==FAIL || fail[6]==UNDEF) ? (fail[6]):(UNDEF);
						TDM_ERROR3("Line rate jitter check failed in %s, unrecognized port speed %0dG at port %0d\n",table,speed[tdm[i]],tdm[i]);
						break;
				}
				switch((speed[tdm[i]]/1000)) {
					case 10:
					case 11:
						spd_norm = 10;
						break;
					case 20:
					case 21:
						spd_norm = speed[tdm[i]]/1000; /* spd_norm = 20; */
					case 25:
					case 27:
						spd_norm = speed[tdm[i]]/1000; /* spd_norm = 25; */
						break;
					case 40:
					case 42:
						spd_norm = speed[tdm[i]]/1000; /* spd_norm = 40; */
						break;
					case 50:
					case 53:
						spd_norm = speed[tdm[i]]/1000; /* spd_norm = 50; */
						break;
					case 100:
					case 106:
						spd_norm = speed[tdm[i]]/1000; /* spd_norm = 100; */
						break;
					default:
						spd_norm = 0;
						break;
				}
				avg = (num==0)? (0): (sum/num);
				rate = (((PKT_SIZE+PKT_PREAMBLE+PKT_IPG)*800)/(2*avg*(100000/freq)));
				if (rate < spd_norm) {
					fail[6] = (fail[6]==FAIL || fail[6]==UNDEF) ? (fail[6]):(FAIL);
					TDM_WARN4("TDM: _____WARNING: Line rate checker %s, port %0d projected rate %0dG to operate under line rate %0dG\n",table,tdm[i],rate,(speed[tdm[i]]/1000));
				}
				up=0; down=0;
				for (j=1; j<len; j++) {
					up++;
					if (tdm[i+j]==tdm[i]) {
						break;
					}
				}
				for (j=1; j<len; j++) {
					down++;
					if (tdm[i-j]==tdm[i]) {
						break;
					}
				}
				if (up > (avg+1+jitter) ) {
					fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(FAIL);
					TDM_WARN6("TDM: _____WARNING: Possible underrun %s, %0dG port %0d index %0d past threshold %0d in up direction %0d\n",table,(speed[tdm[i]]/1000),tdm[i],(i-len),(avg+1+jitter),up);
				}
				if (down > (avg+1+jitter) ) {
					fail[5] = (fail[5]==FAIL || fail[5]==UNDEF) ? (fail[5]):(FAIL);
					TDM_WARN6("TDM: _____WARNING: Possible underrun %s, %0dG port %0d index %0d past threshold %0d in down direction %0d\n",table,(speed[tdm[i]]/1000),tdm[i],(i-len),(avg+1+jitter),down);
				}
			}
		}
	}
	
}


/**
@name: tdm_th_chk_tdm_os_jitter
@param:

Checks distribution of oversub tokens to maximize performance of oversub scheduler
**/
int tdm_th_chk_tdm_os_jitter(int *tdm, int lim, int num_os, int id)
{
	int i, j, k=0, lr_cnt=0, os_cnt=0, os_burst=0, os_burst_idx=0, result=PASS, os_burst_max_threshold;
	const char *table;
	
	switch (id) {
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;
		case 2: table = "IDB Pipe 2"; break;
		case 3: table = "IDB Pipe 3"; break;
		case 4: table = "MMU Pipe 0"; break;
		case 5: table = "MMU Pipe 1"; break;
		case 6: table = "MMU Pipe 2"; break;
		case 7: table = "MMU Pipe 3"; break;	
		default:
			table = "Unrecognized Pipe ID";
			TDM_ERROR1("OS jitter check failed, unrecognized table ID %0d\n",id);
			return UNDEF; 
			break;
	}
	
	for (i=0; i<lim; i++) {
		TH_TOKEN_CHECK(tdm[i]) {
			lr_cnt++;
		}
		if (tdm[i]==TH_OVSB_TOKEN) {
			os_cnt++;
		}
	}
	if (lr_cnt==0 || os_cnt==0) {
		return PASS;
	}
	os_burst_max_threshold=(num_os>((lim/lr_cnt)+1))?(((lim/lr_cnt)+1)):(num_os-1);
	for (i=0; i<lim; i++) {
		if (tdm[i]==TH_OVSB_TOKEN) {
			k=1;
			for (j=1; j<lim; j++) {
				if (tdm[i+j]==TH_OVSB_TOKEN) {k++;}
				else {break;}
			}
			if (k>os_burst) {
				os_burst = k;
				os_burst_idx = i;
			}
		}
	}
	if ( os_burst>os_burst_max_threshold ) {
		TDM_WARN4("TDM: _____WARNING: %s OS token cluster above projected tolerance %0d, cluster size %0d found at %d\n",table,os_burst_max_threshold,os_burst,os_burst_idx);
		result = FAIL;
	}

	return result;
	
}


/**
@name: tdm_th_chk_tdm_sub_lr
@param:

Checks that all line rate nodes were extracted from solution matrix
**/
int tdm_th_chk_tdm_sub_lr(int port, int freq, int lim, int *tdm, enum port_speed_e speed[TH_NUM_EXT_PORTS], enum port_state_e state[TH_NUM_EXT_PORTS], int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS], int traffic[TH_NUM_PHY_PM+1], int id)
{
	int v, y, result=PASS, tmp_length, tmp_width, spd, found_nodes;
	int **temp_map;
	const char *table;
	
	switch (id) {
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;
		case 2: table = "IDB Pipe 2"; break;
		case 3: table = "IDB Pipe 3"; break;
		case 4: table = "MMU Pipe 0"; break;
		case 5: table = "MMU Pipe 1"; break;
		case 6: table = "MMU Pipe 2"; break;
		case 7: table = "MMU Pipe 3"; break;	
		default:
			TDM_ERROR1("Line rate subscription check failed, unrecognized table ID %0d\n",id);
			return UNDEF; 
			break;
	}
	
	if (freq>850) {
		TDM_ERROR2("Subscription check in %s failed in pipe %0d, unrecognized chip frequency\n",table,(port/32));
		return UNDEF;
	}
	
	/* Find speed of input port */
	spd = (speed[port]/1000);
/* 	if (spd!=10 && spd!=11 && spd!=20 && spd!=21 && spd!=25 && spd!=27 && spd!=40 && spd!=42 && spd!=50 && spd!=53 && spd!=100 && spd!=106) {
		TDM_ERROR3("Subscription check in %s failed, unrecognized speed %0dG for port %0d\n",table,spd,port);
		return UNDEF;
	} */
	switch (spd){
		case 10:  
		case 11:  
			spd = 10; break;
		case 20:  
		case 21: 
			spd = 20; break;
		case 25:  
		case 27:  
			spd = 25; break;
		case 40:  
		case 42:  
			spd = 40; break;
		case 50:  
		case 53:  
			spd = 50; break;
		case 100: 
		case 106: 
			spd = 100; break;
		default:
			TDM_ERROR3("Subscription check in %s failed, unrecognized speed %0dG for port %0d\n",table,spd,port);
			return UNDEF;
			break;
	}
	
	/* Allocate small vector map */
	tmp_length=VEC_MAP_LEN;
	tmp_width=1;
	temp_map=(int **) TDM_ALLOC(tmp_width*sizeof(int *), "temp_map_l1");
	for (v=0; v<tmp_width; v++) {
		temp_map[v]=(int *) TDM_ALLOC(tmp_length*sizeof(int), "temp_map_l2");
	}
	for (v=0; v<tmp_length; v++) {
		temp_map[0][v]=TH_NUM_EXT_PORTS;
	}
	/* Extract vector from TDM */
	for (v=0; v<lim; v++) {
		temp_map[0][v] = (tdm[v]==port) ? (port):(TH_NUM_EXT_PORTS);
	}
	/* Count how many nodes */
	found_nodes=0; y=(VEC_MAP_LEN+1);
	for (v=0; v<lim; v++) {
		if (temp_map[0][v]!=TH_NUM_EXT_PORTS) {
			if (y>VEC_MAP_LEN) {y=v;}
			found_nodes++;
		}
	}
	
	found_nodes=0;
	for (v=0; v<lim; v++) {
		if (tdm[v]==port) {
			found_nodes++;
		}
	}
	if ( found_nodes < (((spd*10)/(BW_QUANTA))-1) ) {
		result = FAIL;
		TDM_ERROR5("Subscription error in %s, %0dG vector for port %0d expected %0d nodes (found %0d)\n",table,spd,port,((spd*10)/(BW_QUANTA)),found_nodes);
	}
	
	for (v=0; v<tmp_width; v++) {
		TDM_FREE(temp_map[v]);
	}
	TDM_FREE(temp_map);	
	
	return result;
	
}


/**
@name: tdm_th_chk_tdm_sub_os
@param:

Checks that all oversub nodes were sorted into oversub speed groups
**/
int tdm_th_chk_tdm_sub_os(int port, enum port_speed_e speed[TH_NUM_EXT_PORTS], int os_spds[4][8], int *bucket0, int *bucket1, int *bucket2, int *bucket3, int *bucket4, int *bucket5, int *bucket6, int *bucket7, int id)
{
	int i, result=PASS, pipe;
	const char *table;
	
	result = FAIL;
	for (i=0; i<TH_OS_VBS_GRP_LEN; i++) {
		if (bucket0[i]==port || bucket1[i]==port || bucket2[i]==port || bucket3[i]==port || bucket4[i]==port || bucket5[i]==port || bucket6[i]==port || bucket7[i]==port) {
			result = PASS;
			break;
		}
	}
	if (result == FAIL) {
		return result;
	}

	switch (id) {
		case 0: table = "IDB Pipe 0"; pipe=0; break;
		case 1: table = "IDB Pipe 1"; pipe=1; break;
		case 2: table = "IDB Pipe 2"; pipe=2; break;
		case 3: table = "IDB Pipe 3"; pipe=3; break;
		case 4: table = "MMU Pipe 0"; pipe=0; break;
		case 5: table = "MMU Pipe 1"; pipe=1; break;
		case 6: table = "MMU Pipe 2"; pipe=2; break;
		case 7: table = "MMU Pipe 3"; pipe=3; break;	
		default:
			TDM_ERROR1("Oversub subscription check failed, unrecognized table ID %0d\n",id);
			return UNDEF; 
			break;
	}
	
	for (i=1; i<TH_OS_VBS_GRP_LEN; i++) {
		if (bucket0[i]!=TH_NUM_EXT_PORTS) {
			if (speed[bucket0[i]]>=SPEED_10G && (speed[bucket0[i]]>(os_spds[pipe][0]+1010) || speed[bucket0[i]]<(os_spds[pipe][0]-1010))) {
				result = FAIL; TDM_ERROR4("%s OS bin 0 slot #%0d speed %0dG mismatches %0dG group assginment\n",table,i,speed[bucket0[i]]/1000,os_spds[pipe][0]/1000);
			}
		}
		if (bucket1[i]!=TH_NUM_EXT_PORTS) {
			if (speed[bucket1[i]]>=SPEED_10G && (speed[bucket1[i]]>(os_spds[pipe][1]+1010) || speed[bucket1[i]]<(os_spds[pipe][1]-1010))) {
				result = FAIL; TDM_ERROR4("%s OS bin 1 slot #%0d speed %0dG mismatches %0dG group assginment\n",table,i,speed[bucket1[i]]/1000,os_spds[pipe][0]/1000);
			}
		}
		if (bucket2[i]!=TH_NUM_EXT_PORTS) {
			if (speed[bucket2[i]]>=SPEED_10G && (speed[bucket2[i]]>(os_spds[pipe][2]+1010) || speed[bucket2[i]]<(os_spds[pipe][2]-1010))) {
				result = FAIL; TDM_ERROR4("%s OS bin 2 slot #%0d speed %0dG mismatches %0dG group assginment\n",table,i,speed[bucket2[i]]/1000,os_spds[pipe][0]/1000);
			}
		}
		if (bucket3[i]!=TH_NUM_EXT_PORTS) {
			if (speed[bucket3[i]]>=SPEED_10G && (speed[bucket3[i]]>(os_spds[pipe][3]+1010) || speed[bucket3[i]]<(os_spds[pipe][3]-1010))) {
				result = FAIL; TDM_ERROR4("%s OS bin 3 slot #%0d speed %0dG mismatches %0dG group assginment\n",table,i,speed[bucket3[i]]/1000,os_spds[pipe][0]/1000);
			}
		}
		if (bucket4[i]!=TH_NUM_EXT_PORTS) {
			if (speed[bucket4[i]]>=SPEED_10G && (speed[bucket4[i]]>(os_spds[pipe][4]+1010) || speed[bucket4[i]]<(os_spds[pipe][4]-1010))) {
				result = FAIL; TDM_ERROR4("%s OS bin 4 slot #%0d speed %0dG mismatches %0dG group assginment\n",table,i,speed[bucket4[i]]/1000,os_spds[pipe][0]/1000);
			}
		}
		if (bucket5[i]!=TH_NUM_EXT_PORTS) {
			if (speed[bucket5[i]]>=SPEED_10G && (speed[bucket5[i]]>(os_spds[pipe][5]+1010) || speed[bucket5[i]]<(os_spds[pipe][5]-1010))) {
				result = FAIL; TDM_ERROR4("%s OS bin 5 slot #%0d speed %0dG mismatches %0dG group assginment\n",table,i,speed[bucket5[i]]/1000,os_spds[pipe][0]/1000);
			}
		}
		if (bucket6[i]!=TH_NUM_EXT_PORTS) {
			if (speed[bucket6[i]]>=SPEED_10G && (speed[bucket6[i]]>(os_spds[pipe][6]+1010) || speed[bucket6[i]]<(os_spds[pipe][6]-1010))) {
				result = FAIL; TDM_ERROR4("%s OS bin 6 slot #%0d speed %0dG mismatches %0dG group assginment\n",table,i,speed[bucket6[i]]/1000,os_spds[pipe][0]/1000);
			}
		}
		if (bucket7[i]!=TH_NUM_EXT_PORTS) {
			if (speed[bucket7[i]]>=SPEED_10G && (speed[bucket7[i]]>(os_spds[pipe][7]+1010) || speed[bucket7[i]]<(os_spds[pipe][7]-1010))) {
				result = FAIL; TDM_ERROR4("%s OS bin 7 slot #%0d speed %0dG mismatches %0dG group assginment\n",table,i,speed[bucket7[i]]/1000,os_spds[pipe][0]/1000);
			}
		}
	}
	
	return result;

}


/**
@name: tdm_th_chk_tdm_min
@param:

Checks min spacing in TDM (TSC sister port spacing)
**/
int tdm_th_chk_tdm_min(int *tdm, int len, int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS], int id)
{
	int i, j, tsc_i, tsc_j_1, tsc_j_2, result=PASS, num=len, idx=len;
	const char *table;
	
	switch (id) {
		case 0: table = "IDB Pipe 0"; break;
		case 1: table = "IDB Pipe 1"; break;		
		case 2: table = "IDB Pipe 2"; break;		
		case 3: table = "IDB Pipe 3"; break;
		case 4: table = "MMU Pipe 0"; break;
		case 5: table = "MMU Pipe 1"; break;		
		case 6: table = "MMU Pipe 2"; break;		
		case 7: table = "MMU Pipe 3"; break;
		default: 
			TDM_ERROR1("Min spacing check failed, unrecognized table ID %0d\n",id);
			return UNDEF; 
			break;
	}
	for (i=0; i<=len; i++) {
		TH_TOKEN_CHECK(tdm[len-i]) {
			idx = (len-i);
			break;
		}
	}
	for (i=0; i<60; i++) {
		if (tdm[idx+i]==tdm[idx]) {
			num = ((idx+i+1)>len) ? (idx+i+1):(len);
			break;
		}
	}
	for (i=3; i<num; i++){
		TH_TOKEN_CHECK(tdm[i]) {
			tsc_i = tdm_th_chk_tdm_shim_which_tsc(tdm[i],tsc);
			for (j=1; j<4; j++) {
				tsc_j_1 = tdm_th_chk_tdm_shim_which_tsc(tdm[i-j],tsc);
				tsc_j_2 = tdm_th_chk_tdm_shim_which_tsc(tdm[i+j],tsc);
				if (tsc_i==tsc_j_1 && tsc_i <TH_NUM_PHY_PM){
						result = FAIL;
						TDM_ERROR8("%s min spacing violation port %0d, index (#%0d | #%0d), TSC: [%0d, %0d, %0d, %0d]\n",table,tdm[i],i,(i-j),tsc[tsc_i][0],tsc[tsc_i][1],tsc[tsc_i][2],tsc[tsc_i][3]);
				}
				if (tsc_i==tsc_j_2 && tsc_i <TH_NUM_PHY_PM){
						result = FAIL;
						TDM_ERROR8("%s min spacing violation port %0d, index (#%0d | #%0d), TSC: [%0d, %0d, %0d, %0d]\n",table,tdm[i],i,(i+j),tsc[tsc_i][0],tsc[tsc_i][1],tsc[tsc_i][2],tsc[tsc_i][3]);
				}
			}
		}
	}
	
	return result;
	
}


/**
@name: tdm_th_chk_tdm_tsc
@param:

Verifies TSC transcription caught all ports indexed by port_state_array
**/
int tdm_th_chk_tdm_tsc(int port, enum port_speed_e speed[TH_NUM_EXT_PORTS], enum port_state_e state[TH_NUM_EXT_PORTS], int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS])
{
	int num_lanes=0, i, tsc_idx, result=PASS;

	if (tdm_th_chk_tdm_shim_which_tsc(port,tsc)==TH_NUM_EXT_PORTS) {
		result = FAIL;
		TDM_ERROR1("TSC transcription failed to transcribe port %0d\n",port);
	}
	else {
		for (i=0; i<4; i++) {
			tsc_idx = tdm_th_chk_tdm_shim_which_tsc(port,tsc);
			if (tsc_idx <= TH_NUM_PHY_PM){
				if (tsc[tsc_idx][i]==port) {
					++num_lanes;
				}	
			}
		}
		/* Assay all legal lane configs these speeds can fit */
		switch (speed[port]) {
			case SPEED_1G:
			case SPEED_10G: 
			case SPEED_11G:
			case SPEED_20G: 
			case SPEED_21G: 
			case SPEED_25G: 
			case SPEED_27G:
				if (num_lanes>1) {result = FAIL;} break;
			case SPEED_40G:
			case SPEED_42G:
			case SPEED_42G_HG2:
			case SPEED_50G:
			case SPEED_53G:
				if (num_lanes<2) { result = FAIL;} break;
			case SPEED_100G: 
			case SPEED_106G: 
				if (num_lanes<4) {result = FAIL;} break;
			default:
				TDM_ERROR2("TDM TSC check found unrecognized speed %0dG for port %0d\n",(speed[port]/1000),port);
				return UNDEF;
				break;
		}
		if (result==FAIL) {
			TDM_ERROR2("Port %0d illegal TSC configuration for %0dG\n",port,(speed[port]/1000));
		}
	}
	
	return result;
	
}


/**
@name: tdm_th_chk_tdm_struct
@param:

Checks length, overflow, and symmetry (determinism between IDB and MMU tables
**/
int tdm_th_chk_tdm_struct(int *idb_tdm, int *mmu_tdm, int i_len, int m_len, int pipe, enum port_speed_e speed[TH_NUM_EXT_PORTS], int freq, int traffic[TH_NUM_PHY_PM+1], int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS])
{
	int result=PASS, max_len;
	
	if ( tdm_th_chk_tdm_shim_check_ethernet(idb_tdm[0],speed,tsc,traffic) ) {
		switch (freq) {
			case 850: max_len=LEN_850MHZ_EN; break;
			case 765: max_len=LEN_765MHZ_EN; break;
			case 672: max_len=LEN_672MHZ_EN; break;
			case 645: max_len=LEN_645MHZ_EN; break;
			case 545: max_len=LEN_545MHZ_EN; break;
			default:
				TDM_ERROR0("TDM structure check received unrecognized frequency\n");
				return UNDEF;
				break;
		}
	}
	else {
		switch (freq) {
			case 850: max_len=LEN_850MHZ_HG; break;
			case 765: max_len=LEN_765MHZ_HG; break;
			case 672: max_len=LEN_672MHZ_HG; break;
			case 645: max_len=LEN_645MHZ_HG; break;
			case 545: max_len=LEN_545MHZ_HG; break;
			default:
				TDM_ERROR0("TDM structure check received unrecognized frequency\n");
				return UNDEF;
				break;
		}
	}
	
	if (i_len>max_len) {result=FAIL; TDM_ERROR3("IDB pipe %0d TDM length %0d slots, spec %0d slots\n",pipe,i_len,max_len);}
	if (m_len>max_len) {result=FAIL; TDM_ERROR3("MMU pipe %0d TDM length %0d slots, spec %0d slots\n",pipe,m_len,max_len);}
	
	return result;

}


/**
@name: tdm_th_chk_tdm
@param: 

**/
void
tdm_th_chk_tdm( tdm_mod_t *_tdm)
{	
	int i, j, k, result=PASS, empty;
	int accessories=10;
	
	/* table lengths */
	int mmu_0_len=256, mmu_1_len=256, mmu_2_len=256, mmu_3_len=256,
		idb_0_len=256, idb_1_len=256, idb_2_len=256, idb_3_len=256,
		num_os_0=0, num_os_1=0, num_os_2=0, num_os_3=0;
	int mmu_tdm_len=256, idb_tdm_len=256;
	int om_spds[4][8], oi_spds[4][8];
	int wc_checker[TH_NUM_PHY_PM][TH_NUM_PM_LNS];
	int *mmu_tdm_tbl_0,   *mmu_tdm_tbl_1,   *mmu_tdm_tbl_2,   *mmu_tdm_tbl_3,
	    *mmu_tdm_ovs_0_a, *mmu_tdm_ovs_1_a, *mmu_tdm_ovs_2_a, *mmu_tdm_ovs_3_a,
		*mmu_tdm_ovs_0_b, *mmu_tdm_ovs_1_b, *mmu_tdm_ovs_2_b, *mmu_tdm_ovs_3_b,
		*mmu_tdm_ovs_0_c, *mmu_tdm_ovs_1_c, *mmu_tdm_ovs_2_c, *mmu_tdm_ovs_3_c,
		*mmu_tdm_ovs_0_d, *mmu_tdm_ovs_1_d, *mmu_tdm_ovs_2_d, *mmu_tdm_ovs_3_d,
		*mmu_tdm_ovs_0_e, *mmu_tdm_ovs_1_e, *mmu_tdm_ovs_2_e, *mmu_tdm_ovs_3_e,
		*mmu_tdm_ovs_0_f, *mmu_tdm_ovs_1_f, *mmu_tdm_ovs_2_f, *mmu_tdm_ovs_3_f,
		*mmu_tdm_ovs_0_g, *mmu_tdm_ovs_1_g, *mmu_tdm_ovs_2_g, *mmu_tdm_ovs_3_g,
		*mmu_tdm_ovs_0_h, *mmu_tdm_ovs_1_h, *mmu_tdm_ovs_2_h, *mmu_tdm_ovs_3_h;
	int *idb_tdm_tbl_0,   *idb_tdm_tbl_1,   *idb_tdm_tbl_2,   *idb_tdm_tbl_3,
	    *idb_tdm_ovs_0_a, *idb_tdm_ovs_1_a, *idb_tdm_ovs_2_a, *idb_tdm_ovs_3_a,
		*idb_tdm_ovs_0_b, *idb_tdm_ovs_1_b, *idb_tdm_ovs_2_b, *idb_tdm_ovs_3_b,
		*idb_tdm_ovs_0_c, *idb_tdm_ovs_1_c, *idb_tdm_ovs_2_c, *idb_tdm_ovs_3_c,
		*idb_tdm_ovs_0_d, *idb_tdm_ovs_1_d, *idb_tdm_ovs_2_d, *idb_tdm_ovs_3_d,
		*idb_tdm_ovs_0_e, *idb_tdm_ovs_1_e, *idb_tdm_ovs_2_e, *idb_tdm_ovs_3_e,
		*idb_tdm_ovs_0_f, *idb_tdm_ovs_1_f, *idb_tdm_ovs_2_f, *idb_tdm_ovs_3_f,
		*idb_tdm_ovs_0_g, *idb_tdm_ovs_1_g, *idb_tdm_ovs_2_g, *idb_tdm_ovs_3_g,
		*idb_tdm_ovs_0_h, *idb_tdm_ovs_1_h, *idb_tdm_ovs_2_h, *idb_tdm_ovs_3_h;
	int *mmu_0_mtbl, *idb_0_mtbl, *mmu_0_tracker, *idb_0_tracker,
	    *mmu_1_mtbl, *idb_1_mtbl, *mmu_1_tracker, *idb_1_tracker,
		*mmu_2_mtbl, *idb_2_mtbl, *mmu_2_tracker, *idb_2_tracker,
		*mmu_3_mtbl, *idb_3_mtbl, *mmu_3_tracker, *idb_3_tracker;
	
	/* To support TDM-5 */
	enum port_speed_e speed[TH_NUM_EXT_PORTS];
	enum port_state_e state[TH_NUM_EXT_PORTS];
	int traffic[TH_NUM_PHY_PM+1];
	int freq, fail[7];

	/* MMU Pipe 0 */
	mmu_tdm_tbl_0   = (int *) TDM_ALLOC(mmu_tdm_len*sizeof(int), "TDM MMU Calendar 0, main");
	mmu_tdm_ovs_0_a = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 0, ovs a");
	mmu_tdm_ovs_0_b = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 0, ovs b");
	mmu_tdm_ovs_0_c = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 0, ovs c");
	mmu_tdm_ovs_0_d = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 0, ovs d");
	mmu_tdm_ovs_0_e = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 0, ovs e");
	mmu_tdm_ovs_0_f = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 0, ovs f");
	mmu_tdm_ovs_0_g = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 0, ovs g");
	mmu_tdm_ovs_0_h = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 0, ovs h");
	/* MMU Pipe 1 */
	mmu_tdm_tbl_1   = (int *) TDM_ALLOC(mmu_tdm_len*sizeof(int), "TDM MMU Calendar 1, main");
	mmu_tdm_ovs_1_a = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 1, ovs a");
	mmu_tdm_ovs_1_b = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 1, ovs b");
	mmu_tdm_ovs_1_c = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 1, ovs c");
	mmu_tdm_ovs_1_d = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 1, ovs d");
	mmu_tdm_ovs_1_e = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 1, ovs e");
	mmu_tdm_ovs_1_f = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 1, ovs f");
	mmu_tdm_ovs_1_g = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 1, ovs g");
	mmu_tdm_ovs_1_h = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 1, ovs h");
	/* MMU Pipe 2 */
	mmu_tdm_tbl_2   = (int *) TDM_ALLOC(mmu_tdm_len*sizeof(int), "TDM MMU Calendar 2, main");
	mmu_tdm_ovs_2_a = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 2, ovs a");
	mmu_tdm_ovs_2_b = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 2, ovs b");
	mmu_tdm_ovs_2_c = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 2, ovs c");
	mmu_tdm_ovs_2_d = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 2, ovs d");
	mmu_tdm_ovs_2_e = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 2, ovs e");
	mmu_tdm_ovs_2_f = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 2, ovs f");
	mmu_tdm_ovs_2_g = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 2, ovs g");
	mmu_tdm_ovs_2_h = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 2, ovs h");
	/* MMU Pipe 3 */
	mmu_tdm_tbl_3   = (int *) TDM_ALLOC(mmu_tdm_len*sizeof(int), "TDM MMU Calendar 3, main");
	mmu_tdm_ovs_3_a = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 3, ovs a");
	mmu_tdm_ovs_3_b = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 3, ovs b");
	mmu_tdm_ovs_3_c = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 3, ovs c");
	mmu_tdm_ovs_3_d = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 3, ovs d");
	mmu_tdm_ovs_3_e = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 3, ovs e");
	mmu_tdm_ovs_3_f = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 3, ovs f");
	mmu_tdm_ovs_3_g = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 3, ovs g");
	mmu_tdm_ovs_3_h = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM MMU Calendar 3, ovs h");
	/* IDB Pipe 0 */
	idb_tdm_tbl_0   = (int *) TDM_ALLOC(idb_tdm_len*sizeof(int), "TDM IDB Calendar 0, main");
	idb_tdm_ovs_0_a = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 0, ovs a");
	idb_tdm_ovs_0_b = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 0, ovs b");
	idb_tdm_ovs_0_c = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 0, ovs c");
	idb_tdm_ovs_0_d = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 0, ovs d");
	idb_tdm_ovs_0_e = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 0, ovs e");
	idb_tdm_ovs_0_f = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 0, ovs f");
	idb_tdm_ovs_0_g = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 0, ovs g");
	idb_tdm_ovs_0_h = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 0, ovs h");
	/* IDB Pipe 1 */
	idb_tdm_tbl_1   = (int *) TDM_ALLOC(idb_tdm_len*sizeof(int), "TDM IDB Calendar 1, main");
	idb_tdm_ovs_1_a = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 1, ovs a");
	idb_tdm_ovs_1_b = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 1, ovs b");
	idb_tdm_ovs_1_c = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 1, ovs c");
	idb_tdm_ovs_1_d = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 1, ovs d");
	idb_tdm_ovs_1_e = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 1, ovs e");
	idb_tdm_ovs_1_f = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 1, ovs f");
	idb_tdm_ovs_1_g = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 1, ovs g");
	idb_tdm_ovs_1_h = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 1, ovs h");
	/* IDB Pipe 2 */
	idb_tdm_tbl_2   = (int *) TDM_ALLOC(idb_tdm_len*sizeof(int), "TDM IDB Calendar 2, main");
	idb_tdm_ovs_2_a = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 2, ovs a");
	idb_tdm_ovs_2_b = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 2, ovs b");
	idb_tdm_ovs_2_c = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 2, ovs c");
	idb_tdm_ovs_2_d = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 2, ovs d");
	idb_tdm_ovs_2_e = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 2, ovs e");
	idb_tdm_ovs_2_f = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 2, ovs f");
	idb_tdm_ovs_2_g = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 2, ovs g");
	idb_tdm_ovs_2_h = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 2, ovs h");
	/* IDB Pipe 3 */
	idb_tdm_tbl_3   = (int *) TDM_ALLOC(idb_tdm_len*sizeof(int), "TDM IDB Calendar 3, main");
	idb_tdm_ovs_3_a = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 3, ovs a");
	idb_tdm_ovs_3_b = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 3, ovs b");
	idb_tdm_ovs_3_c = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 3, ovs c");
	idb_tdm_ovs_3_d = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 3, ovs d");
	idb_tdm_ovs_3_e = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 3, ovs e");
	idb_tdm_ovs_3_f = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 3, ovs f");
	idb_tdm_ovs_3_g = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 3, ovs g");
	idb_tdm_ovs_3_h = (int *) TDM_ALLOC(TH_OS_VBS_GRP_LEN*sizeof(int), "TDM IDB Calendar 3, ovs h");
	
	/* MMU Mirror Calendar*/
	mmu_0_mtbl = (int *) TDM_ALLOC(2*mmu_tdm_len*sizeof(int), "TDM MMU Mirror Calendar 0, main");
	mmu_1_mtbl = (int *) TDM_ALLOC(2*mmu_tdm_len*sizeof(int), "TDM MMU Mirror Calendar 1, main");
	mmu_2_mtbl = (int *) TDM_ALLOC(2*mmu_tdm_len*sizeof(int), "TDM MMU Mirror Calendar 2, main");
	mmu_3_mtbl = (int *) TDM_ALLOC(2*mmu_tdm_len*sizeof(int), "TDM MMU Mirror Calendar 3, main");
	idb_0_mtbl = (int *) TDM_ALLOC(2*idb_tdm_len*sizeof(int), "TDM IDB Mirror Calendar 0, main");
	idb_1_mtbl = (int *) TDM_ALLOC(2*idb_tdm_len*sizeof(int), "TDM IDB Mirror Calendar 1, main");
	idb_2_mtbl = (int *) TDM_ALLOC(2*idb_tdm_len*sizeof(int), "TDM IDB Mirror Calendar 2, main");
	idb_3_mtbl = (int *) TDM_ALLOC(2*idb_tdm_len*sizeof(int), "TDM IDB Mirror Calendar 3, main");
	/* Tracker of MMU/IDB main Calendar*/
	mmu_0_tracker = (int *) TDM_ALLOC(mmu_tdm_len*sizeof(int), "TDM MMU Tracker 0");
	mmu_1_tracker = (int *) TDM_ALLOC(mmu_tdm_len*sizeof(int), "TDM MMU Tracker 1");
	mmu_2_tracker = (int *) TDM_ALLOC(mmu_tdm_len*sizeof(int), "TDM MMU Tracker 2");
	mmu_3_tracker = (int *) TDM_ALLOC(mmu_tdm_len*sizeof(int), "TDM MMU Tracker 3");
	idb_0_tracker = (int *) TDM_ALLOC(idb_tdm_len*sizeof(int), "TDM IDB Tracker 0");
	idb_1_tracker = (int *) TDM_ALLOC(idb_tdm_len*sizeof(int), "TDM IDB Tracker 1");
	idb_2_tracker = (int *) TDM_ALLOC(idb_tdm_len*sizeof(int), "TDM IDB Tracker 2");
	idb_3_tracker = (int *) TDM_ALLOC(idb_tdm_len*sizeof(int), "TDM IDB Tracker 3");
	
	
	/* initialize */
	freq = _tdm->_chip_data.soc_pkg.clk_freq;
	for (i=0; i<7; i++) {fail[i]=2;}
	for (i=0; i<TH_NUM_EXT_PORTS; i++) {
		speed[i] = _tdm->_chip_data.soc_pkg.speed[i];
		state[i] = _tdm->_chip_data.soc_pkg.state[i];
	}
	for (i=0; i<(TH_NUM_PHY_PM+1); i++){
		traffic[i] = _tdm->_chip_data.soc_pkg.soc_vars.th.pm_encap_type[i];
	}
	for (i=0; i<mmu_tdm_len; i++){
		mmu_tdm_tbl_0[i] = _tdm->_chip_data.cal_4.cal_main[i];
		mmu_tdm_tbl_1[i] = _tdm->_chip_data.cal_5.cal_main[i];
		mmu_tdm_tbl_2[i] = _tdm->_chip_data.cal_6.cal_main[i];
		mmu_tdm_tbl_3[i] = _tdm->_chip_data.cal_7.cal_main[i];
	}
	for (i=0; i<idb_tdm_len; i++){
		idb_tdm_tbl_0[i] = _tdm->_chip_data.cal_0.cal_main[i];
		idb_tdm_tbl_1[i] = _tdm->_chip_data.cal_1.cal_main[i];
		idb_tdm_tbl_2[i] = _tdm->_chip_data.cal_2.cal_main[i];
		idb_tdm_tbl_3[i] = _tdm->_chip_data.cal_3.cal_main[i];
	}
	for (i=0; i<TH_OS_VBS_GRP_LEN; i++){
		/* IDB Pipe 0 */
		idb_tdm_ovs_0_a[i] = _tdm->_chip_data.cal_0.cal_grp[0][i];
		idb_tdm_ovs_0_b[i] = _tdm->_chip_data.cal_0.cal_grp[1][i];
		idb_tdm_ovs_0_c[i] = _tdm->_chip_data.cal_0.cal_grp[2][i];
		idb_tdm_ovs_0_d[i] = _tdm->_chip_data.cal_0.cal_grp[3][i];
		idb_tdm_ovs_0_e[i] = _tdm->_chip_data.cal_0.cal_grp[4][i];
		idb_tdm_ovs_0_f[i] = _tdm->_chip_data.cal_0.cal_grp[5][i];
		idb_tdm_ovs_0_g[i] = _tdm->_chip_data.cal_0.cal_grp[6][i];
		idb_tdm_ovs_0_h[i] = _tdm->_chip_data.cal_0.cal_grp[7][i];
		/* IDB Pipe 1 */
		idb_tdm_ovs_1_a[i] = _tdm->_chip_data.cal_1.cal_grp[0][i];
		idb_tdm_ovs_1_b[i] = _tdm->_chip_data.cal_1.cal_grp[1][i];
		idb_tdm_ovs_1_c[i] = _tdm->_chip_data.cal_1.cal_grp[2][i];
		idb_tdm_ovs_1_d[i] = _tdm->_chip_data.cal_1.cal_grp[3][i];
		idb_tdm_ovs_1_e[i] = _tdm->_chip_data.cal_1.cal_grp[4][i];
		idb_tdm_ovs_1_f[i] = _tdm->_chip_data.cal_1.cal_grp[5][i];
		idb_tdm_ovs_1_g[i] = _tdm->_chip_data.cal_1.cal_grp[6][i];
		idb_tdm_ovs_1_h[i] = _tdm->_chip_data.cal_1.cal_grp[7][i];
		/* IDB Pipe 2 */
		idb_tdm_ovs_2_a[i] = _tdm->_chip_data.cal_2.cal_grp[0][i];
		idb_tdm_ovs_2_b[i] = _tdm->_chip_data.cal_2.cal_grp[1][i];
		idb_tdm_ovs_2_c[i] = _tdm->_chip_data.cal_2.cal_grp[2][i];
		idb_tdm_ovs_2_d[i] = _tdm->_chip_data.cal_2.cal_grp[3][i];
		idb_tdm_ovs_2_e[i] = _tdm->_chip_data.cal_2.cal_grp[4][i];
		idb_tdm_ovs_2_f[i] = _tdm->_chip_data.cal_2.cal_grp[5][i];
		idb_tdm_ovs_2_g[i] = _tdm->_chip_data.cal_2.cal_grp[6][i];
		idb_tdm_ovs_2_h[i] = _tdm->_chip_data.cal_2.cal_grp[7][i];
		/* IDB Pipe 3 */
		idb_tdm_ovs_3_a[i] = _tdm->_chip_data.cal_3.cal_grp[0][i];
		idb_tdm_ovs_3_b[i] = _tdm->_chip_data.cal_3.cal_grp[1][i];
		idb_tdm_ovs_3_c[i] = _tdm->_chip_data.cal_3.cal_grp[2][i];
		idb_tdm_ovs_3_d[i] = _tdm->_chip_data.cal_3.cal_grp[3][i];
		idb_tdm_ovs_3_e[i] = _tdm->_chip_data.cal_3.cal_grp[4][i];
		idb_tdm_ovs_3_f[i] = _tdm->_chip_data.cal_3.cal_grp[5][i];
		idb_tdm_ovs_3_g[i] = _tdm->_chip_data.cal_3.cal_grp[6][i];
		idb_tdm_ovs_3_h[i] = _tdm->_chip_data.cal_3.cal_grp[7][i];
		/* MMU Pipe 0 */
		mmu_tdm_ovs_0_a[i] = _tdm->_chip_data.cal_4.cal_grp[0][i];
		mmu_tdm_ovs_0_b[i] = _tdm->_chip_data.cal_4.cal_grp[1][i];
		mmu_tdm_ovs_0_c[i] = _tdm->_chip_data.cal_4.cal_grp[2][i];
		mmu_tdm_ovs_0_d[i] = _tdm->_chip_data.cal_4.cal_grp[3][i];
		mmu_tdm_ovs_0_e[i] = _tdm->_chip_data.cal_4.cal_grp[4][i];
		mmu_tdm_ovs_0_f[i] = _tdm->_chip_data.cal_4.cal_grp[5][i];
		mmu_tdm_ovs_0_g[i] = _tdm->_chip_data.cal_4.cal_grp[6][i];
		mmu_tdm_ovs_0_h[i] = _tdm->_chip_data.cal_4.cal_grp[7][i];
		/* MMU Pipe 1 */
		mmu_tdm_ovs_1_a[i] = _tdm->_chip_data.cal_5.cal_grp[0][i];
		mmu_tdm_ovs_1_b[i] = _tdm->_chip_data.cal_5.cal_grp[1][i];
		mmu_tdm_ovs_1_c[i] = _tdm->_chip_data.cal_5.cal_grp[2][i];
		mmu_tdm_ovs_1_d[i] = _tdm->_chip_data.cal_5.cal_grp[3][i];
		mmu_tdm_ovs_1_e[i] = _tdm->_chip_data.cal_5.cal_grp[4][i];
		mmu_tdm_ovs_1_f[i] = _tdm->_chip_data.cal_5.cal_grp[5][i];
		mmu_tdm_ovs_1_g[i] = _tdm->_chip_data.cal_5.cal_grp[6][i];
		mmu_tdm_ovs_1_h[i] = _tdm->_chip_data.cal_5.cal_grp[7][i];
		/* MMU Pipe 2 */
		mmu_tdm_ovs_2_a[i] = _tdm->_chip_data.cal_6.cal_grp[0][i];
		mmu_tdm_ovs_2_b[i] = _tdm->_chip_data.cal_6.cal_grp[1][i];
		mmu_tdm_ovs_2_c[i] = _tdm->_chip_data.cal_6.cal_grp[2][i];
		mmu_tdm_ovs_2_d[i] = _tdm->_chip_data.cal_6.cal_grp[3][i];
		mmu_tdm_ovs_2_e[i] = _tdm->_chip_data.cal_6.cal_grp[4][i];
		mmu_tdm_ovs_2_f[i] = _tdm->_chip_data.cal_6.cal_grp[5][i];
		mmu_tdm_ovs_2_g[i] = _tdm->_chip_data.cal_6.cal_grp[6][i];
		mmu_tdm_ovs_2_h[i] = _tdm->_chip_data.cal_6.cal_grp[7][i];
		/* MMU Pipe 3 */
		mmu_tdm_ovs_3_a[i] = _tdm->_chip_data.cal_7.cal_grp[0][i];
		mmu_tdm_ovs_3_b[i] = _tdm->_chip_data.cal_7.cal_grp[1][i];
		mmu_tdm_ovs_3_c[i] = _tdm->_chip_data.cal_7.cal_grp[2][i];
		mmu_tdm_ovs_3_d[i] = _tdm->_chip_data.cal_7.cal_grp[3][i];
		mmu_tdm_ovs_3_e[i] = _tdm->_chip_data.cal_7.cal_grp[4][i];
		mmu_tdm_ovs_3_f[i] = _tdm->_chip_data.cal_7.cal_grp[5][i];
		mmu_tdm_ovs_3_g[i] = _tdm->_chip_data.cal_7.cal_grp[6][i];
		mmu_tdm_ovs_3_h[i] = _tdm->_chip_data.cal_7.cal_grp[7][i];		
	}
	
	
	TDM_BIG_BAR
	TDM_PRINT0("TDM: TDM Self Check\n");
	for (i=0; i<TH_NUM_PHY_PM; i++){
		for (j=0; j<TH_NUM_PM_LNS; j++){
			wc_checker[i][j] = _tdm->_chip_data.soc_pkg.pmap[i][j];
		}
	}
	
	TDM_PRINT2("TDM: (clk - %0d MHz) (accessories - %0d)\n", freq, accessories);
	idb_0_len = idb_tdm_len; 
	idb_1_len = idb_tdm_len; 
	idb_2_len = idb_tdm_len; 
	idb_3_len = idb_tdm_len;
	mmu_0_len = mmu_tdm_len; 
	mmu_1_len = mmu_tdm_len; 
	mmu_2_len = mmu_tdm_len; 
	mmu_3_len = mmu_tdm_len;
	
	for (i=idb_tdm_len-1; i>0; i--) {if (idb_tdm_tbl_0[i]==TH_NUM_EXT_PORTS) {idb_0_len--;} else {break;}}
	for (i=idb_tdm_len-1; i>0; i--) {if (idb_tdm_tbl_1[i]==TH_NUM_EXT_PORTS) {idb_1_len--;} else {break;}}
	for (i=idb_tdm_len-1; i>0; i--) {if (idb_tdm_tbl_2[i]==TH_NUM_EXT_PORTS) {idb_2_len--;} else {break;}}
	for (i=idb_tdm_len-1; i>0; i--) {if (idb_tdm_tbl_3[i]==TH_NUM_EXT_PORTS) {idb_3_len--;} else {break;}}
	for (i=mmu_tdm_len-1; i>0; i--) {if (mmu_tdm_tbl_0[i]==TH_NUM_EXT_PORTS) {mmu_0_len--;} else {break;}}
	for (i=mmu_tdm_len-1; i>0; i--) {if (mmu_tdm_tbl_1[i]==TH_NUM_EXT_PORTS) {mmu_1_len--;} else {break;}}
	for (i=mmu_tdm_len-1; i>0; i--) {if (mmu_tdm_tbl_2[i]==TH_NUM_EXT_PORTS) {mmu_2_len--;} else {break;}}
	for (i=mmu_tdm_len-1; i>0; i--) {if (mmu_tdm_tbl_3[i]==TH_NUM_EXT_PORTS) {mmu_3_len--;} else {break;}}	
	TDM_PRINT4("TDM: (idb0 - %0d)(idb1 - %0d)(idb2 - %0d)(idb3 - %0d)\n",idb_0_len,idb_1_len,idb_2_len,idb_3_len);
	TDM_PRINT4("TDM: (mmu0 - %0d)(mmu1 - %0d)(mmu2 - %0d)(mmu3 - %0d)\n",mmu_0_len,mmu_1_len,mmu_2_len,mmu_3_len);	
	
	/* Mirror the tables to make loopback easier to investigate */
	for (i=0; i<mmu_0_len; i++) {mmu_0_mtbl[i]=mmu_tdm_tbl_0[i]; mmu_0_mtbl[i+mmu_0_len]=mmu_tdm_tbl_0[i];}
	for (i=0; i<mmu_1_len; i++) {mmu_1_mtbl[i]=mmu_tdm_tbl_1[i]; mmu_1_mtbl[i+mmu_1_len]=mmu_tdm_tbl_1[i];}
	for (i=0; i<mmu_2_len; i++) {mmu_2_mtbl[i]=mmu_tdm_tbl_2[i]; mmu_2_mtbl[i+mmu_2_len]=mmu_tdm_tbl_2[i];}
	for (i=0; i<mmu_3_len; i++) {mmu_3_mtbl[i]=mmu_tdm_tbl_3[i]; mmu_3_mtbl[i+mmu_3_len]=mmu_tdm_tbl_3[i];}
	for (i=0; i<idb_0_len; i++) {idb_0_mtbl[i]=idb_tdm_tbl_0[i]; idb_0_mtbl[i+idb_0_len]=idb_tdm_tbl_0[i];}
	for (i=0; i<idb_1_len; i++) {idb_1_mtbl[i]=idb_tdm_tbl_1[i]; idb_1_mtbl[i+idb_1_len]=idb_tdm_tbl_1[i];}
	for (i=0; i<idb_2_len; i++) {idb_2_mtbl[i]=idb_tdm_tbl_2[i]; idb_2_mtbl[i+idb_2_len]=idb_tdm_tbl_2[i];}
	for (i=0; i<idb_3_len; i++) {idb_3_mtbl[i]=idb_tdm_tbl_3[i]; idb_3_mtbl[i+idb_3_len]=idb_tdm_tbl_3[i];}

	for (i=0; i<TH_OS_VBS_GRP_LEN; i++) {
		if (mmu_tdm_ovs_0_a[i]!=TH_NUM_EXT_PORTS) {++num_os_0;}
		if (mmu_tdm_ovs_1_a[i]!=TH_NUM_EXT_PORTS) {++num_os_1;}
		if (mmu_tdm_ovs_2_a[i]!=TH_NUM_EXT_PORTS) {++num_os_2;}
		if (mmu_tdm_ovs_3_a[i]!=TH_NUM_EXT_PORTS) {++num_os_3;}
	}

	om_spds[0][0]=(mmu_tdm_ovs_0_a[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_0_a[0]];
	om_spds[0][1]=(mmu_tdm_ovs_0_b[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_0_b[0]];
	om_spds[0][2]=(mmu_tdm_ovs_0_c[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_0_c[0]];
	om_spds[0][3]=(mmu_tdm_ovs_0_d[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_0_d[0]];
	om_spds[0][4]=(mmu_tdm_ovs_0_e[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_0_e[0]];
	om_spds[0][5]=(mmu_tdm_ovs_0_f[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_0_f[0]];
	om_spds[0][6]=(mmu_tdm_ovs_0_g[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_0_g[0]];
	om_spds[0][7]=(mmu_tdm_ovs_0_h[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_0_h[0]];	
	om_spds[1][0]=(mmu_tdm_ovs_1_a[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_1_a[0]];
	om_spds[1][1]=(mmu_tdm_ovs_1_b[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_1_b[0]];
	om_spds[1][2]=(mmu_tdm_ovs_1_c[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_1_c[0]];
	om_spds[1][3]=(mmu_tdm_ovs_1_d[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_1_d[0]];
	om_spds[1][4]=(mmu_tdm_ovs_1_e[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_1_e[0]];
	om_spds[1][5]=(mmu_tdm_ovs_1_f[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_1_f[0]];
	om_spds[1][6]=(mmu_tdm_ovs_1_g[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_1_g[0]];
	om_spds[1][7]=(mmu_tdm_ovs_1_h[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_1_h[0]];
	om_spds[2][0]=(mmu_tdm_ovs_2_a[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_2_a[0]];
	om_spds[2][1]=(mmu_tdm_ovs_2_b[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_2_b[0]];
	om_spds[2][2]=(mmu_tdm_ovs_2_c[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_2_c[0]];
	om_spds[2][3]=(mmu_tdm_ovs_2_d[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_2_d[0]];
	om_spds[2][4]=(mmu_tdm_ovs_2_e[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_2_e[0]];
	om_spds[2][5]=(mmu_tdm_ovs_2_f[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_2_f[0]];
	om_spds[2][6]=(mmu_tdm_ovs_2_g[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_2_g[0]];
	om_spds[2][7]=(mmu_tdm_ovs_2_h[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_2_h[0]];
	om_spds[3][0]=(mmu_tdm_ovs_3_a[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_3_a[0]];
	om_spds[3][1]=(mmu_tdm_ovs_3_b[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_3_b[0]];
	om_spds[3][2]=(mmu_tdm_ovs_3_c[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_3_c[0]];
	om_spds[3][3]=(mmu_tdm_ovs_3_d[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_3_d[0]];
	om_spds[3][4]=(mmu_tdm_ovs_3_e[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_3_e[0]];
	om_spds[3][5]=(mmu_tdm_ovs_3_f[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_3_f[0]];
	om_spds[3][6]=(mmu_tdm_ovs_3_g[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_3_g[0]];
	om_spds[3][7]=(mmu_tdm_ovs_3_h[0]==TH_NUM_EXT_PORTS) ? (0) : speed[mmu_tdm_ovs_3_h[0]];
	oi_spds[0][0]=(idb_tdm_ovs_0_a[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_0_a[0]];
	oi_spds[0][1]=(idb_tdm_ovs_0_b[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_0_b[0]];
	oi_spds[0][2]=(idb_tdm_ovs_0_c[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_0_c[0]];
	oi_spds[0][3]=(idb_tdm_ovs_0_d[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_0_d[0]];
	oi_spds[0][4]=(idb_tdm_ovs_0_e[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_0_e[0]];
	oi_spds[0][5]=(idb_tdm_ovs_0_f[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_0_f[0]];
	oi_spds[0][6]=(idb_tdm_ovs_0_g[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_0_g[0]];
	oi_spds[0][7]=(idb_tdm_ovs_0_h[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_0_h[0]];
	oi_spds[1][0]=(idb_tdm_ovs_1_a[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_1_a[0]];
	oi_spds[1][1]=(idb_tdm_ovs_1_b[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_1_b[0]];
	oi_spds[1][2]=(idb_tdm_ovs_1_c[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_1_c[0]];
	oi_spds[1][3]=(idb_tdm_ovs_1_d[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_1_d[0]];
	oi_spds[1][4]=(idb_tdm_ovs_1_e[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_1_e[0]];
	oi_spds[1][5]=(idb_tdm_ovs_1_f[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_1_f[0]];
	oi_spds[1][6]=(idb_tdm_ovs_1_g[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_1_g[0]];
	oi_spds[1][7]=(idb_tdm_ovs_1_h[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_1_h[0]];
	oi_spds[2][0]=(idb_tdm_ovs_2_a[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_2_a[0]];
	oi_spds[2][1]=(idb_tdm_ovs_2_b[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_2_b[0]];
	oi_spds[2][2]=(idb_tdm_ovs_2_c[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_2_c[0]];
	oi_spds[2][3]=(idb_tdm_ovs_2_d[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_2_d[0]];
	oi_spds[2][4]=(idb_tdm_ovs_2_e[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_2_e[0]];
	oi_spds[2][5]=(idb_tdm_ovs_2_f[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_2_f[0]];
	oi_spds[2][6]=(idb_tdm_ovs_2_g[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_2_g[0]];
	oi_spds[2][7]=(idb_tdm_ovs_2_h[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_2_h[0]];	
	oi_spds[3][0]=(idb_tdm_ovs_3_a[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_3_a[0]];
	oi_spds[3][1]=(idb_tdm_ovs_3_b[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_3_b[0]];
	oi_spds[3][2]=(idb_tdm_ovs_3_c[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_3_c[0]];
	oi_spds[3][3]=(idb_tdm_ovs_3_d[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_3_d[0]];
	oi_spds[3][4]=(idb_tdm_ovs_3_e[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_3_e[0]];
	oi_spds[3][5]=(idb_tdm_ovs_3_f[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_3_f[0]];
	oi_spds[3][6]=(idb_tdm_ovs_3_g[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_3_g[0]];
	oi_spds[3][7]=(idb_tdm_ovs_3_h[0]==TH_NUM_EXT_PORTS) ? (0) : speed[idb_tdm_ovs_3_h[0]];
	
	TDM_SML_BAR
	TDM_PRINT0("\tIDB oversub speed matrix: \n\t");
	TDM_PRINT0("\t------------------------- \n\t");
	for (i=0; i<4; i++) {
		for (j=0; j<8; j++) {
			TDM_PRINT1(" %8dG\t", oi_spds[i][j]/1000);
		}
		TDM_PRINT0("\n");
		if (i<3) {TDM_PRINT0("\t");}
	}
	TDM_PRINT0("\tMMU oversub speed matrix: \n\t");
	TDM_PRINT0("\t------------------------- \n\t");
	for (i=0; i<4; i++) {
		for (j=0; j<8; j++) {
			TDM_PRINT1(" %8dG\t", om_spds[i][j]/1000);
		}
		TDM_PRINT0("\n");
		if (i<3) {TDM_PRINT0("\t");}
	}
	TDM_PRINT0("\n");
	
	/* For a port in idb_x_tracker[i], value is the distance to next identical slot */
	empty=BOOL_TRUE;
	for (i=0; i<idb_0_len; i++) {
		TH_TOKEN_CHECK(idb_0_mtbl[i]) {
			empty=BOOL_FALSE;
			break;
		}
	}
	if (!empty) {
		for (i=0; i<idb_0_len; i++) {
			if (idb_0_mtbl[i]!=TH_NUM_EXT_PORTS && idb_0_mtbl[i]!=TH_OVSB_TOKEN) {
				k=0;
				for (j=1; j<idb_0_len; j++) {
					k++;
					if (idb_0_mtbl[i+j]==idb_0_mtbl[i]) {idb_0_tracker[i]=k; break;}
				}
			}
			else {idb_0_tracker[i]=0;}
		}
	}
	else {
		for (i=0; i<idb_0_len; i++) {
			idb_0_tracker[i]=0;
		}
	}
	empty=BOOL_TRUE;
	for (i=0; i<idb_1_len; i++) {
		TH_TOKEN_CHECK(idb_1_mtbl[i]) {
			empty=BOOL_FALSE;
			break;
		}
	}
	if (!empty) {	
		for (i=0; i<idb_1_len; i++) {
			if (idb_1_mtbl[i]!=TH_NUM_EXT_PORTS && idb_1_mtbl[i]!=TH_OVSB_TOKEN) {
				k=0;
				for (j=1; j<idb_1_len; j++) {
					k++;
					if (idb_1_mtbl[i+j]==idb_1_mtbl[i]) {idb_1_tracker[i]=k; break;}
				}
			}
			else {idb_1_tracker[i]=0;}
		}
	}
	else {
		for (i=0; i<idb_1_len; i++) {
			idb_1_tracker[i]=0;
		}
	}
	empty=BOOL_TRUE;
	for (i=0; i<idb_2_len; i++) {
		TH_TOKEN_CHECK(idb_2_mtbl[i]) {
			empty=BOOL_FALSE;
			break;
		}
	}
	if (!empty) {	
		for (i=0; i<idb_2_len; i++) {
			if (idb_2_mtbl[i]!=TH_NUM_EXT_PORTS && idb_2_mtbl[i]!=TH_OVSB_TOKEN) {
				k=0;
				for (j=1; j<idb_2_len; j++) {
					k++;
					if (idb_2_mtbl[i+j]==idb_2_mtbl[i]) {idb_2_tracker[i]=k; break;}
				}
			}
			else {idb_2_tracker[i]=0;}
		}
	}
	else {
		for (i=0; i<idb_2_len; i++) {
			idb_2_tracker[i]=0;
		}
	}
	
	empty=BOOL_TRUE;
	for (i=0; i<idb_3_len; i++) {
		TH_TOKEN_CHECK(idb_3_mtbl[i]) {
			empty=BOOL_FALSE;
			break;
		}
	}
	if (!empty) {		
		for (i=0; i<idb_3_len; i++) {
			if (idb_3_mtbl[i]!=TH_NUM_EXT_PORTS && idb_3_mtbl[i]!=TH_OVSB_TOKEN) {
				k=0;
				for (j=1; j<idb_3_len; j++) {
					k++;
					if (idb_3_mtbl[i+j]==idb_3_mtbl[i]) {idb_3_tracker[i]=k; break;}
				}
			}
			else {idb_3_tracker[i]=0;}
		}
	}
	else {
		for (i=0; i<idb_3_len; i++) {
			idb_3_tracker[i]=0;
		}
	}
	empty=BOOL_TRUE;
	for (i=0; i<mmu_0_len; i++) {
		TH_TOKEN_CHECK(mmu_0_mtbl[i]) {
			empty=BOOL_FALSE;
			break;
		}
	}
	if (!empty) {		
		for (i=0; i<mmu_0_len; i++) {
			if (mmu_0_mtbl[i]!=TH_NUM_EXT_PORTS && mmu_0_mtbl[i]!=TH_OVSB_TOKEN) {
				k=0;
				for (j=1; j<mmu_0_len; j++) {
					k++;
					if (mmu_0_mtbl[i+j]==mmu_0_mtbl[i]) {mmu_0_tracker[i]=k; break;}
				}
			}
			else {mmu_0_tracker[i]=0;}
		}
	}
	else {
		for (i=0; i<mmu_0_len; i++) {
			mmu_0_tracker[i]=0;
		}
	}
	
	empty=BOOL_TRUE;
	for (i=0; i<mmu_1_len; i++) {
		TH_TOKEN_CHECK(mmu_1_mtbl[i]) {
			empty=BOOL_FALSE;
			break;
		}
	}
	if (!empty) {			
		for (i=0; i<mmu_1_len; i++) {
			if (mmu_1_mtbl[i]!=TH_NUM_EXT_PORTS && mmu_1_mtbl[i]!=TH_OVSB_TOKEN) {
				k=0;
				for (j=1; j<mmu_1_len; j++) {
					k++;
					if (mmu_1_mtbl[i+j]==mmu_1_mtbl[i]) {mmu_1_tracker[i]=k; break;}
				}
			}
			else {mmu_1_tracker[i]=0;}
		}
	}
	else {
		for (i=0; i<mmu_1_len; i++) {
			mmu_1_tracker[i]=0;
		}
	}
	empty=BOOL_TRUE;
	for (i=0; i<mmu_2_len; i++) {
		TH_TOKEN_CHECK(mmu_2_mtbl[i]) {
			empty=BOOL_FALSE;
			break;
		}
	}
	if (!empty) {
		for (i=0; i<mmu_2_len; i++) {
			if (mmu_2_mtbl[i]!=TH_NUM_EXT_PORTS && mmu_2_mtbl[i]!=TH_OVSB_TOKEN) {
				k=0;
				for (j=1; j<mmu_2_len; j++) {
					k++;
					if (mmu_2_mtbl[i+j]==mmu_2_mtbl[i]) {mmu_2_tracker[i]=k; break;}
				}
			}
			else {mmu_2_tracker[i]=0;}
		}	
	}
	else {
		for (i=0; i<mmu_2_len; i++) {
			mmu_2_tracker[i]=0;
		}
	}
	empty=BOOL_TRUE;
	for (i=0; i<mmu_3_len; i++) {
		TH_TOKEN_CHECK(mmu_3_mtbl[i]) {
			empty=BOOL_FALSE;
			break;
		}
	}
	if (!empty) {
		for (i=0; i<mmu_3_len; i++) {
			if (mmu_3_mtbl[i]!=TH_NUM_EXT_PORTS && mmu_3_mtbl[i]!=TH_OVSB_TOKEN) {
				k=0;
				for (j=1; j<mmu_3_len; j++) {
					k++;
					if (mmu_3_mtbl[i+j]==mmu_3_mtbl[i]) {mmu_3_tracker[i]=k; break;}
				}
			}
			else {mmu_3_tracker[i]=0;}
		}
	}
	else {
		for (i=0; i<mmu_3_len; i++) {
			mmu_3_tracker[i]=0;
		}
	}
	
	{
/* 		int tracker_stop_print[4];
		TDM_SML_BAR
		TDM_PRINT0("TDM Spacing Map\n");
		TDM_PRINT0("\t\tPipe0\tPipe1\tPipe2\tPipe3\n");
		TDM_PRINT0("\t\t-----\t-----\t-----\t-----\n");
		tracker_stop_print[0]=BOOL_FALSE; tracker_stop_print[1]=BOOL_FALSE; tracker_stop_print[2]=BOOL_FALSE; tracker_stop_print[3]=BOOL_FALSE;
		for (i=0; i<224; i++) {
			if (tracker_stop_print[0]==BOOL_TRUE && tracker_stop_print[1]==BOOL_TRUE && tracker_stop_print[2]==BOOL_TRUE && tracker_stop_print[3]==BOOL_TRUE) {break;}
			TDM_PRINT1("\t[%d]",i);
			if (i<idb_0_len) {if (idb_0_tracker[i]!=0) {TDM_PRINT1("\t%02d",idb_0_tracker[i]);} else {TDM_PRINT0("\t--");}} else {TDM_PRINT0("\t"); tracker_stop_print[0]=BOOL_TRUE;}
			if (i<idb_1_len) {if (idb_1_tracker[i]!=0) {TDM_PRINT1("\t%02d",idb_1_tracker[i]);} else {TDM_PRINT0("\t--");}} else {TDM_PRINT0("\t"); tracker_stop_print[1]=BOOL_TRUE;}
			if (i<idb_2_len) {if (idb_2_tracker[i]!=0) {TDM_PRINT1("\t%02d",idb_2_tracker[i]);} else {TDM_PRINT0("\t--");}} else {TDM_PRINT0("\t"); tracker_stop_print[2]=BOOL_TRUE;}
			if (i<idb_3_len) {if (idb_3_tracker[i]!=0) {TDM_PRINT1("\t%02d",idb_3_tracker[i]);} else {TDM_PRINT0("\t--");}} else {TDM_PRINT0("\t"); tracker_stop_print[3]=BOOL_TRUE;}
			TDM_PRINT0("\n");
		} */
	}
	
	for (i=0; i<idb_0_len; i++) {
		if (idb_0_mtbl[i]<1 || idb_0_mtbl[i]>128) {
			idb_0_tracker[i]=0;
		}		
		if (mmu_0_mtbl[i]<1 || mmu_0_mtbl[i]>128) {
			mmu_0_tracker[i]=0;
		}				
	}
	for (i=0; i<idb_1_len; i++) {
		if (idb_1_mtbl[i]<1 || idb_1_mtbl[i]>128) {
			idb_1_tracker[i]=0;
		}		
		if (mmu_1_mtbl[i]<1 || mmu_1_mtbl[i]>128) {
			mmu_1_tracker[i]=0;
		}				
	}	
	for (i=0; i<idb_2_len; i++) {
		if (idb_2_mtbl[i]<1 || idb_2_mtbl[i]>128) {
			idb_2_tracker[i]=0;
		}		
		if (mmu_2_mtbl[i]<1 || mmu_2_mtbl[i]>128) {
			mmu_2_tracker[i]=0;
		}				
	}	
	for (i=0; i<idb_3_len; i++) {
		if (idb_3_mtbl[i]<1 || idb_3_mtbl[i]>128) {
			idb_3_tracker[i]=0;
		}		
		if (mmu_3_mtbl[i]<1 || mmu_3_mtbl[i]>128) {
			mmu_3_tracker[i]=0;
		}				
	}	

	TDM_PRINT0("\n");
	/* [0]: Check length (overflow and symmetry) */
	if (fail[0]==2) {
		tdm_th_chk_print_config(speed, state, traffic);
		TDM_PRINT0("TDM: Checking length (overflow and symmetry)\n"); TDM_SML_BAR
		result = tdm_th_chk_tdm_struct(idb_0_mtbl, mmu_0_mtbl, idb_0_len, mmu_0_len, 0, speed, freq, traffic, wc_checker);
		fail[0] = (fail[0]==FAIL || fail[0]==UNDEF) ? (fail[0]):(result);
		result = tdm_th_chk_tdm_struct(idb_1_mtbl, mmu_1_mtbl, idb_1_len, mmu_1_len, 1, speed, freq, traffic, wc_checker);
		fail[0] = (fail[0]==FAIL || fail[0]==UNDEF) ? (fail[0]):(result);
		result = tdm_th_chk_tdm_struct(idb_2_mtbl, mmu_2_mtbl, idb_2_len, mmu_2_len, 2, speed, freq, traffic, wc_checker);
		fail[0] = (fail[0]==FAIL || fail[0]==UNDEF) ? (fail[0]):(result);
		result = tdm_th_chk_tdm_struct(idb_3_mtbl, mmu_3_mtbl, idb_3_len, mmu_3_len, 3, speed, freq, traffic, wc_checker);
		fail[0] = (fail[0]==FAIL || fail[0]==UNDEF) ? (fail[0]):(result);
	}

	/* [1]: Check TSC transcription */
	if (fail[1]==2) {
		TDM_PRINT0("\nTDM: Checking TSC transcription\n"); TDM_SML_BAR
		for (i=0; i<128; i++) {
			if (state[i]!=0 && state[i]!=3 && speed[i+1]>SPEED_0) {
				result = tdm_th_chk_tdm_tsc((i+1),speed,state,wc_checker);
				fail[1] = (fail[1]==FAIL || fail[1]==UNDEF) ? (fail[1]):(result);
			}
		}		
	}
	
	/* [2]: Check sister port spacing */
	if (fail[2]==2) {
		TDM_PRINT0("\nTDM: Checking min spacing\n"); TDM_SML_BAR
		result = tdm_th_chk_tdm_min(idb_0_mtbl,idb_0_len,wc_checker,0);
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);
		result = tdm_th_chk_tdm_min(idb_1_mtbl,idb_1_len,wc_checker,1);
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);
		result = tdm_th_chk_tdm_min(idb_2_mtbl,idb_2_len,wc_checker,2);
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);
		result = tdm_th_chk_tdm_min(idb_3_mtbl,idb_3_len,wc_checker,3);
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);
		result = tdm_th_chk_tdm_min(mmu_0_mtbl,mmu_0_len,wc_checker,4);
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);
		result = tdm_th_chk_tdm_min(mmu_1_mtbl,mmu_1_len,wc_checker,5);
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);
		result = tdm_th_chk_tdm_min(mmu_2_mtbl,mmu_2_len,wc_checker,6);
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);
		result = tdm_th_chk_tdm_min(mmu_3_mtbl,mmu_3_len,wc_checker,7);
		fail[2] = (fail[2]==FAIL || fail[2]==UNDEF) ? (fail[2]):(result);		
	}
	
	/* [3]: Check subcription */
	if (fail[3]==2) {
		TDM_PRINT0("\nTDM: Checking per port subscription stats\n"); TDM_SML_BAR
		/* CPU port */
		k=0;
		for (j=0; j<idb_0_len; j++) {
			if (idb_0_mtbl[j]==0) {
				k++;
			}
		}
		if (k<4) {
			fail[3] = FAIL;
			TDM_ERROR1("Subscription error in Pipe 0, CPU port bandwidth is insufficient in this configuration - %0d slots\n",k);
		}
		/* Loopback port 0 */
		k=0;
		for (j=0; j<idb_0_len; j++) {
			if (idb_0_mtbl[j]==TH_LPB0_TOKEN) {
				k++;
			}
		}
		if (k<2) {
			fail[3] = FAIL;
			TDM_ERROR1("Subscription error in Pipe 0, loopback port bandwidth is insufficient in this configuration - %0d slots\n",k);
		}
		/* Loopback port 1 */
		k=0;
		for (j=0; j<idb_1_len; j++) {
			if (idb_1_mtbl[j]==TH_LPB1_TOKEN) {
				k++;
			}
		}
		if (k<2) {
			fail[3] = FAIL;
			TDM_ERROR1("Subscription error in Pipe 1, loopback port bandwidth is insufficient in this configuration - %0d slots\n",k);
		}
		/* Loopback port 2 */
		k=0;
		for (j=0; j<idb_2_len; j++) {
			if (idb_2_mtbl[j]==TH_LPB2_TOKEN) {
				k++;
			}
		}
		if (k<2) {
			fail[3] = FAIL;
			TDM_ERROR1("Subscription error in Pipe 2, loopback port bandwidth is insufficient in this configuration - %0d slots\n",k);
		}		
		/* Loopback port 3 */
		k=0;
		for (j=0; j<idb_3_len; j++) {
			if (idb_3_mtbl[j]==TH_LPB3_TOKEN) {
				k++;
			}
		}
		if (k<2) {
			fail[3] = FAIL;
			TDM_ERROR1("Subscription error in Pipe 3, loopback port bandwidth is insufficient in this configuration - %0d slots\n",k);
		}
		for (i=1; i<33; i++) {
			if (state[i-1]==1 || state[i-1]==5) {
				k=0;
				for (j=0; j<idb_0_len; j++) {
					if (idb_0_mtbl[j]==i) {
						k++;
					}
				}
				if (k<4) {
					fail[3] = FAIL;
					TDM_ERROR2("Subscription error in IDB pipe 0, port %0d is enabled but not scheduled in calender, %0d slots\n",i,k);
				}
				else {
					result=tdm_th_chk_tdm_sub_lr(i,freq,idb_0_len,idb_0_mtbl,speed,state,wc_checker,traffic,0);
					fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				}
			}
			else if (state[i-1]==2 || state[i-1]==6) {
				result = tdm_th_chk_tdm_sub_os(i,speed,oi_spds,idb_tdm_ovs_0_a,idb_tdm_ovs_0_b,idb_tdm_ovs_0_c,idb_tdm_ovs_0_d,idb_tdm_ovs_0_e,idb_tdm_ovs_0_f,idb_tdm_ovs_0_g,idb_tdm_ovs_0_h,0);
				fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				if (fail[3]==FAIL || fail[3]==UNDEF) {break;}
			}
		}
		for (i=33; i<65; i++) {
			if (state[i-1]==1 || state[i-1]==5) {
				k=0;
				for (j=0; j<idb_1_len; j++) {
					if (idb_1_mtbl[j]==i) {
						k++;
					}
				}
				if (k<4) {
					fail[3] = FAIL;
					TDM_ERROR2("Subscription error in IDB pipe 1, port %0d is enabled but not scheduled in calender, %0d slots\n",i,k);
				}
				else {
					result = tdm_th_chk_tdm_sub_lr(i,freq,idb_1_len,idb_1_mtbl,speed,state,wc_checker,traffic,1);
					fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				}
			}
			else if (state[i-1]==2 || state[i-1]==6) {
				result = tdm_th_chk_tdm_sub_os(i,speed,oi_spds,idb_tdm_ovs_1_a,idb_tdm_ovs_1_b,idb_tdm_ovs_1_c,idb_tdm_ovs_1_d,idb_tdm_ovs_1_e,idb_tdm_ovs_1_f,idb_tdm_ovs_1_g,idb_tdm_ovs_1_h,1);
				fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				if (fail[3]==FAIL || fail[3]==UNDEF) {break;}
			}
		}
		for (i=65; i<97; i++) {
			if (state[i-1]==1 || state[i-1]==5) {
				k=0;
				for (j=0; j<idb_2_len; j++) {
					if (idb_2_mtbl[j]==i) {
						k++;
					}
				}
				if (k<4) {
					fail[3] = FAIL;
					TDM_ERROR2("Subscription error in IDB pipe 2, port %0d is enabled but not scheduled in calender, %0d slots\n",i,k);
				}
				else {
					result = tdm_th_chk_tdm_sub_lr(i,freq,idb_2_len,idb_2_mtbl,speed,state,wc_checker,traffic,2);
					fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				}
			}
			else if (state[i-1]==2 || state[i-1]==6) {
				result = tdm_th_chk_tdm_sub_os(i,speed,oi_spds,idb_tdm_ovs_2_a,idb_tdm_ovs_2_b,idb_tdm_ovs_2_c,idb_tdm_ovs_2_d,idb_tdm_ovs_2_e,idb_tdm_ovs_2_f,idb_tdm_ovs_2_g,idb_tdm_ovs_2_h,2);
				fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				if (fail[3]==FAIL || fail[3]==UNDEF) {break;}
			}
		}
		for (i=97; i<129; i++) {
			if (state[i-1]==1 || state[i-1]==5) {
				k=0;
				for (j=0; j<idb_3_len; j++) {
					if (idb_3_mtbl[j]==i) {
						k++;
					}
				}
				if (k<4) {
					fail[3] = FAIL;
					TDM_ERROR2("Subscription error in IDB pipe 3, port %0d is enabled but not scheduled in calender, %0d slots\n",i,k);
				}
				else {
					tdm_th_chk_tdm_sub_lr(i,freq,idb_3_len,idb_3_mtbl,speed,state,wc_checker,traffic,3);
					fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				}
			}
			else if (state[i-1]==2 || state[i-1]==6) {
				tdm_th_chk_tdm_sub_os(i,speed,oi_spds,idb_tdm_ovs_3_a,idb_tdm_ovs_3_b,idb_tdm_ovs_3_c,idb_tdm_ovs_3_d,idb_tdm_ovs_3_e,idb_tdm_ovs_3_f,idb_tdm_ovs_3_g,idb_tdm_ovs_3_h,3);
				fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				if (fail[3]==FAIL || fail[3]==UNDEF) {break;}
			}
		}
		for (i=1; i<33; i++) {
			if (state[i-1]==1 || state[i-1]==5) {
				k=0;
				for (j=0; j<mmu_0_len; j++) {
					if (mmu_0_mtbl[j]==i) {
						k++;
					}
				}
				if (k<4) {
					fail[3] = FAIL;
					TDM_ERROR2("Subscription error in MMU pipe 0, port %0d is enabled but not scheduled in calender, %0d slots\n",i,k);
				}
				else {
					result = tdm_th_chk_tdm_sub_lr(i,freq,mmu_0_len,mmu_0_mtbl,speed,state,wc_checker,traffic,4);
					fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				}
			}
			else if (state[i-1]==2 || state[i-1]==6) {
				result = tdm_th_chk_tdm_sub_os(i,speed,om_spds,mmu_tdm_ovs_0_a,mmu_tdm_ovs_0_b,mmu_tdm_ovs_0_c,mmu_tdm_ovs_0_d,mmu_tdm_ovs_0_e,mmu_tdm_ovs_0_f,mmu_tdm_ovs_0_g,mmu_tdm_ovs_0_h,4);
				fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				if (fail[3]==FAIL || fail[3]==UNDEF) {break;}
			}
		}
		for (i=33; i<65; i++) {
			if (state[i-1]==1 || state[i-1]==5) {
				k=0;
				for (j=0; j<mmu_1_len; j++) {
					if (mmu_1_mtbl[j]==i) {
						k++;
					}
				}
				if (k<4) {
					fail[3] = FAIL;
					TDM_ERROR2("Subscription error in MMU pipe 1, port %0d is enabled but not scheduled in calender, %0d slots\n",i,k);
				}
				else {
					result = tdm_th_chk_tdm_sub_lr(i,freq,mmu_1_len,mmu_1_mtbl,speed,state,wc_checker,traffic,5);
					fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				}
			}
			else if (state[i-1]==2 || state[i-1]==6) {
				result = tdm_th_chk_tdm_sub_os(i,speed,om_spds,mmu_tdm_ovs_1_a,mmu_tdm_ovs_1_b,mmu_tdm_ovs_1_c,mmu_tdm_ovs_1_d,mmu_tdm_ovs_1_e,mmu_tdm_ovs_1_f,mmu_tdm_ovs_1_g,mmu_tdm_ovs_1_h,5);
				fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				if (fail[3]==FAIL || fail[3]==UNDEF) {break;}
			}
		}
		for (i=65; i<97; i++) {
			if (state[i-1]==1 || state[i-1]==5) {
				k=0;
				for (j=0; j<mmu_2_len; j++) {
					if (mmu_2_mtbl[j]==i) {
						k++;
					}
				}
				if (k<4) {
					fail[3] = FAIL;
					TDM_ERROR2("Subscription error in MMU pipe 2, port %0d is enabled but not scheduled in calender, %0d slots\n",i,k);
				}
				else {
					result = tdm_th_chk_tdm_sub_lr(i,freq,mmu_2_len,mmu_2_mtbl,speed,state,wc_checker,traffic,6);
					fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				}
			}
			else if (state[i-1]==2 || state[i-1]==6) {
				result = tdm_th_chk_tdm_sub_os(i,speed,om_spds,mmu_tdm_ovs_2_a,mmu_tdm_ovs_2_b,mmu_tdm_ovs_2_c,mmu_tdm_ovs_2_d,mmu_tdm_ovs_2_e,mmu_tdm_ovs_2_f,mmu_tdm_ovs_2_g,mmu_tdm_ovs_2_h,6);
				fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				if (fail[3]==FAIL || fail[3]==UNDEF) {break;}
			}
		}
		for (i=97; i<129; i++) {
			if (state[i-1]==1 || state[i-1]==5) {
				k=0;
				for (j=0; j<mmu_3_len; j++) {
					if (mmu_3_mtbl[j]==i) {
						k++;
					}
				}
				if (k<4) {
					fail[3] = FAIL;
					TDM_ERROR2("Subscription error in MMU pipe 3, port %0d is enabled but not scheduled in calender, %0d slots\n",i,k);
				}
				else {
					result = tdm_th_chk_tdm_sub_lr(i,freq,mmu_3_len,mmu_3_mtbl,speed,state,wc_checker,traffic,7);
					fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				}
			}
			else if (state[i-1]==2 || state[i-1]==6) {
				result = tdm_th_chk_tdm_sub_os(i,speed,om_spds,mmu_tdm_ovs_3_a,mmu_tdm_ovs_3_b,mmu_tdm_ovs_3_c,mmu_tdm_ovs_3_d,mmu_tdm_ovs_3_e,mmu_tdm_ovs_3_f,mmu_tdm_ovs_3_g,mmu_tdm_ovs_3_h,7);
				fail[3] = (fail[3]==FAIL || fail[3]==UNDEF) ? (fail[3]):(result);
				if (fail[3]==FAIL || fail[3]==UNDEF) {break;}
			}
		}	
	}
	
	/* [4]: This should be a warning in wrapper */
	if (fail[4]==2) {
		TDM_PRINT0("\nTDM: Checking OS jitter\n"); TDM_SML_BAR
		result = tdm_th_chk_tdm_os_jitter(idb_0_mtbl,idb_0_len,num_os_0,0);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
		result = tdm_th_chk_tdm_os_jitter(idb_1_mtbl,idb_1_len,num_os_1,1);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
		result = tdm_th_chk_tdm_os_jitter(idb_2_mtbl,idb_2_len,num_os_2,2);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
		result = tdm_th_chk_tdm_os_jitter(idb_3_mtbl,idb_3_len,num_os_3,3);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
		result = tdm_th_chk_tdm_os_jitter(mmu_0_mtbl,mmu_0_len,num_os_0,4);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
		result = tdm_th_chk_tdm_os_jitter(mmu_1_mtbl,mmu_1_len,num_os_1,5);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
		result = tdm_th_chk_tdm_os_jitter(mmu_2_mtbl,mmu_2_len,num_os_2,6);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
		result = tdm_th_chk_tdm_os_jitter(mmu_3_mtbl,mmu_3_len,num_os_3,7);
		fail[4] = (fail[4]==FAIL || fail[4]==UNDEF) ? (fail[4]):(result);
	}
	/*  [5] & [6] */
	if (fail[5]==2 || fail[6]==2) {
		TDM_PRINT0("\nTDM: Checking line rate jitter\n"); TDM_SML_BAR
		tdm_th_chk_tdm_lr_jitter(idb_0_mtbl,idb_0_tracker,freq,idb_0_len,speed,0,fail);
		tdm_th_chk_tdm_lr_jitter(idb_1_mtbl,idb_1_tracker,freq,idb_1_len,speed,1,fail);
		tdm_th_chk_tdm_lr_jitter(idb_2_mtbl,idb_2_tracker,freq,idb_2_len,speed,2,fail);
		tdm_th_chk_tdm_lr_jitter(idb_3_mtbl,idb_3_tracker,freq,idb_3_len,speed,3,fail);
		tdm_th_chk_tdm_lr_jitter(mmu_0_mtbl,mmu_0_tracker,freq,mmu_0_len,speed,4,fail);
		tdm_th_chk_tdm_lr_jitter(mmu_1_mtbl,mmu_1_tracker,freq,mmu_1_len,speed,5,fail);
		tdm_th_chk_tdm_lr_jitter(mmu_2_mtbl,mmu_2_tracker,freq,mmu_2_len,speed,6,fail);
		tdm_th_chk_tdm_lr_jitter(mmu_3_mtbl,mmu_3_tracker,freq,mmu_3_len,speed,7,fail);
	}
	
	TDM_PRINT0("\n"); TDM_SML_BAR
	if (fail[0]==FAIL ||
	    fail[1]==FAIL ||
		/* fail[4]==1 || */
		fail[2]==FAIL ||
		fail[3]==FAIL /* ||
		fail[5]==FAIL ||
		fail[6]==FAIL */
		) {
		TDM_PRINT0("TDM: *** FAILED ***\n");
		TDM_SML_BAR
		TDM_PRINT0("TDM: Fail vector: [");
		for (i=0; i<7; i++) {
			TDM_PRINT1(" %0d ",fail[i]);
		}
		TDM_PRINT0("]\n");
	}
	else {
		TDM_PRINT0("TDM: *** PASSED ***\n");
	}
	TDM_SML_BAR
	TDM_PRINT0("TDM: TDM Self Check Complete.\n");
	TDM_BIG_BAR	
	
	TDM_FREE(mmu_tdm_tbl_0);   TDM_FREE(mmu_tdm_tbl_1);   TDM_FREE(mmu_tdm_tbl_2);   TDM_FREE(mmu_tdm_tbl_3);
	TDM_FREE(mmu_tdm_ovs_0_a); TDM_FREE(mmu_tdm_ovs_1_a); TDM_FREE(mmu_tdm_ovs_2_a); TDM_FREE(mmu_tdm_ovs_3_a);
	TDM_FREE(mmu_tdm_ovs_0_b); TDM_FREE(mmu_tdm_ovs_1_b); TDM_FREE(mmu_tdm_ovs_2_b); TDM_FREE(mmu_tdm_ovs_3_b);
	TDM_FREE(mmu_tdm_ovs_0_c); TDM_FREE(mmu_tdm_ovs_1_c); TDM_FREE(mmu_tdm_ovs_2_c); TDM_FREE(mmu_tdm_ovs_3_c);
	TDM_FREE(mmu_tdm_ovs_0_d); TDM_FREE(mmu_tdm_ovs_1_d); TDM_FREE(mmu_tdm_ovs_2_d); TDM_FREE(mmu_tdm_ovs_3_d);
	TDM_FREE(mmu_tdm_ovs_0_e); TDM_FREE(mmu_tdm_ovs_1_e); TDM_FREE(mmu_tdm_ovs_2_e); TDM_FREE(mmu_tdm_ovs_3_e);
	TDM_FREE(mmu_tdm_ovs_0_f); TDM_FREE(mmu_tdm_ovs_1_f); TDM_FREE(mmu_tdm_ovs_2_f); TDM_FREE(mmu_tdm_ovs_3_f);
	TDM_FREE(mmu_tdm_ovs_0_g); TDM_FREE(mmu_tdm_ovs_1_g); TDM_FREE(mmu_tdm_ovs_2_g); TDM_FREE(mmu_tdm_ovs_3_g);
	TDM_FREE(mmu_tdm_ovs_0_h); TDM_FREE(mmu_tdm_ovs_1_h); TDM_FREE(mmu_tdm_ovs_2_h); TDM_FREE(mmu_tdm_ovs_3_h);
	
	TDM_FREE(idb_tdm_tbl_0);   TDM_FREE(idb_tdm_tbl_1);   TDM_FREE(idb_tdm_tbl_2);   TDM_FREE(idb_tdm_tbl_3);
	TDM_FREE(idb_tdm_ovs_0_a); TDM_FREE(idb_tdm_ovs_1_a); TDM_FREE(idb_tdm_ovs_2_a); TDM_FREE(idb_tdm_ovs_3_a);
	TDM_FREE(idb_tdm_ovs_0_b); TDM_FREE(idb_tdm_ovs_1_b); TDM_FREE(idb_tdm_ovs_2_b); TDM_FREE(idb_tdm_ovs_3_b);
	TDM_FREE(idb_tdm_ovs_0_c); TDM_FREE(idb_tdm_ovs_1_c); TDM_FREE(idb_tdm_ovs_2_c); TDM_FREE(idb_tdm_ovs_3_c);
	TDM_FREE(idb_tdm_ovs_0_d); TDM_FREE(idb_tdm_ovs_1_d); TDM_FREE(idb_tdm_ovs_2_d); TDM_FREE(idb_tdm_ovs_3_d);
	TDM_FREE(idb_tdm_ovs_0_e); TDM_FREE(idb_tdm_ovs_1_e); TDM_FREE(idb_tdm_ovs_2_e); TDM_FREE(idb_tdm_ovs_3_e);
	TDM_FREE(idb_tdm_ovs_0_f); TDM_FREE(idb_tdm_ovs_1_f); TDM_FREE(idb_tdm_ovs_2_f); TDM_FREE(idb_tdm_ovs_3_f);
	TDM_FREE(idb_tdm_ovs_0_g); TDM_FREE(idb_tdm_ovs_1_g); TDM_FREE(idb_tdm_ovs_2_g); TDM_FREE(idb_tdm_ovs_3_g);
	TDM_FREE(idb_tdm_ovs_0_h); TDM_FREE(idb_tdm_ovs_1_h); TDM_FREE(idb_tdm_ovs_2_h); TDM_FREE(idb_tdm_ovs_3_h);
	
	TDM_FREE(mmu_0_mtbl); TDM_FREE(idb_0_mtbl); TDM_FREE(mmu_0_tracker); TDM_FREE(idb_0_tracker);
	TDM_FREE(mmu_1_mtbl); TDM_FREE(idb_1_mtbl); TDM_FREE(mmu_1_tracker); TDM_FREE(idb_1_tracker);
	TDM_FREE(mmu_2_mtbl); TDM_FREE(idb_2_mtbl); TDM_FREE(mmu_2_tracker); TDM_FREE(idb_2_tracker);
	TDM_FREE(mmu_3_mtbl); TDM_FREE(idb_3_mtbl); TDM_FREE(mmu_3_tracker); TDM_FREE(idb_3_tracker);
}
