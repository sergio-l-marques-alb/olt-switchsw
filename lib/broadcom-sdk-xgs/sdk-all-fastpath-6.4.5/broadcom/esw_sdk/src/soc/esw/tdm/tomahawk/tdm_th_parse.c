/*
 * $Id: tdm_th_parse.c.$
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
 * TDM chip based printing and parsing functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_th_parse_pipe
@param:

Parses pipe config from class structs
**/
void
tdm_th_parse_pipe( tdm_mod_t *_tdm )
{
	int iter, iter2;

	/* Summarize the port config in this quadrant */
	TDM_PRINT0("TDM: --- Pipe Config ---: ");
	for (iter=_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_start; iter<=_tdm->_chip_data.soc_pkg.soc_vars.th.pipe_end; iter++) {
		if ((((iter-1)%16)==0)) {
			TDM_PRINT0("\nTDM: ");
		}
		TDM_PRINT1("{%03d}\t",iter);
		if (iter%16==0) {
			TDM_PRINT0("\nTDM: ");
			for (iter2=(iter-16); iter2<iter; iter2++) {
				TDM_PRINT1("%d\t",_tdm->_chip_data.soc_pkg.speed[iter2+1]);
			}
			TDM_PRINT0("\nTDM: ");
			for (iter2=(iter-16); iter2<iter; iter2++) {
				if ((iter2-1)%TH_NUM_PM_LNS==0) {
					switch (_tdm->_chip_data.soc_pkg.state[iter2-1]) {
						case PORT_STATE__LINERATE:
						case PORT_STATE__LINERATE_HG:
							TDM_PRINT0("LINE\t---\t---\t---\t");
							break;
						case PORT_STATE__OVERSUB:
						case PORT_STATE__OVERSUB_HG:
							TDM_PRINT0("OVSB\t---\t---\t---\t");
							break;
						default:
							break;
					}
				}
			}
			TDM_PRINT0("\nTDM: ");
			for (iter2=(iter-16); iter2<iter; iter2++) {
				if ((iter2-1)%TH_NUM_PM_LNS==0) {
					switch (_tdm->_chip_data.soc_pkg.state[iter2-1]) {
						case PORT_STATE__LINERATE_HG: case PORT_STATE__OVERSUB_HG:
							TDM_PRINT0("HIGIG2\t---\t---\t---\t");
							break;
						case PORT_STATE__LINERATE: case PORT_STATE__OVERSUB:
							TDM_PRINT0("ETHRNT\t---\t---\t---\t");
							break;
						default:
							break;
					}
				}
			}
		}
	}
	TDM_PRINT0("\n");
	TDM_BIG_BAR
}


/**
@name: tdm_th_parse_tdm_tbl
@param:

Postprocesses TDM calendar tokenized entries
**/
int
tdm_th_parse_tdm_tbl( tdm_mod_t *_tdm )
{
	int i, j, m;
	tdm_calendar_t cal_idb;
	tdm_calendar_t cal_mmu;
	
	switch(_tdm->_core_data.vars_pkg.cal_id) {
		case 0: cal_idb=_tdm->_chip_data.cal_0; cal_mmu=_tdm->_chip_data.cal_4; break;
		case 1: cal_idb=_tdm->_chip_data.cal_1; cal_mmu=_tdm->_chip_data.cal_5; break;
		case 2: cal_idb=_tdm->_chip_data.cal_2; cal_mmu=_tdm->_chip_data.cal_6; break;
		case 3: cal_idb=_tdm->_chip_data.cal_3; cal_mmu=_tdm->_chip_data.cal_7; break;
		default:
			TDM_PRINT1("Invalid calendar ID - %0d\n",_tdm->_core_data.vars_pkg.cal_id);
			return (TDM_EXEC_CORE_SIZE+1);
	}
	for (j=0; j<256; j++) {
		cal_mmu.cal_main[j]=cal_idb.cal_main[j];
	}
	for (i=0; i<TH_OS_VBS_GRP_NUM; i++) {
		for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
			cal_mmu.cal_grp[i][j]=cal_idb.cal_grp[i][j];
		}
	}
	m=0;
	for (j=0; j<256; j++) {
		if (cal_idb.cal_main[j]!=TH_ANCL_TOKEN) {
			TDM_PRINT3("TDM: PIPE %d, IDB TDM TABLE, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_main[j]);
		}
		else {
			TDM_PRINT2("TDM: PIPE %d, IDB TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ",_tdm->_core_data.vars_pkg.pipe,j);
			m++;
			switch (_tdm->_core_data.vars_pkg.pipe) {
				case 0:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_CMIC(j,cal_idb.cal_main)
						case 2: case 6:
							TH_LPB0(j,cal_idb.cal_main)
						case 4: case 10:
							TH_OPRT(j,cal_idb.cal_main)
						case 5:
							TH_NULL(j,cal_idb.cal_main)
						case 8:
							TH_SBUS(j,cal_idb.cal_main)
					}
					break;
				case 1:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_MGM1(j,cal_idb.cal_main)
						case 2: case 6:
							TH_LPB1(j,cal_idb.cal_main)
						case 4: case 10:
							TH_OPRT(j,cal_idb.cal_main)
						case 5:
							TH_NULL(j,cal_idb.cal_main)
						case 8:
							TH_SBUS(j,cal_idb.cal_main)
					}
					break;
				case 2:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_MGM2(j,cal_idb.cal_main)
						case 2: case 6:
							TH_LPB2(j,cal_idb.cal_main)
						case 4: case 10:
							TH_OPRT(j,cal_idb.cal_main)
						case 5:
							TH_NULL(j,cal_idb.cal_main)
						case 8:
							TH_SBUS(j,cal_idb.cal_main)
					}
					break;
				case 3:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_DOT3(j,cal_idb.cal_main)
						case 2: case 6:
							TH_LPB3(j,cal_idb.cal_main)
						case 4: case 10:
							TH_OPRT(j,cal_idb.cal_main)
						case 5:
							TH_NULL(j,cal_idb.cal_main)
						case 8:
							TH_SBUS(j,cal_idb.cal_main)
					}
					break;
			}
		}
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 0, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[0][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 1, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[1][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 2, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[2][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 3, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[3][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 4, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[4][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 5, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[5][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 6, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[6][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, IDB OVS BUCKET 7, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_idb.cal_grp[7][j]);
	}
	m=0;
	for (j=0; j<256; j++) {
		if (cal_mmu.cal_main[j]!=TH_ANCL_TOKEN) {
			TDM_PRINT3("TDM: PIPE %d, MMU TDM TABLE, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_main[j]);
		}
		else {
			TDM_PRINT2("TDM: PIPE %d, MMU TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ",_tdm->_core_data.vars_pkg.pipe,j);
			m++;
			switch (_tdm->_core_data.vars_pkg.pipe) {
				case 0:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_CMIC(j,cal_mmu.cal_main)
						case 2: case 6:
							TH_LPB0(j,cal_mmu.cal_main)
						case 4: case 5: case 8:
							TH_NULL(j,cal_mmu.cal_main)
						case 10:
							TH_OPRT(j,cal_mmu.cal_main)
					}
					break;
				case 1:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_MGM1(j,cal_mmu.cal_main)
						case 2: case 6:
							TH_LPB1(j,cal_mmu.cal_main)
						case 4: case 5: case 8:
							TH_NULL(j,cal_mmu.cal_main)
						case 10:
							TH_OPRT(j,cal_mmu.cal_main)
					}
					break;
				case 2:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_MGM2(j,cal_mmu.cal_main)
						case 2: case 6:
							TH_LPB2(j,cal_mmu.cal_main)
						case 4: case 5: case 8:
							TH_NULL(j,cal_mmu.cal_main)
						case 10:
							TH_OPRT(j,cal_mmu.cal_main)
					}
					break;
				case 3:
					switch (m) {
						case 1: case 3: case 7: case 9:
							TH_DOT3(j,cal_mmu.cal_main)
						case 2: case 6:
							TH_LPB3(j,cal_mmu.cal_main)
						case 4: case 5: case 8:
							TH_NULL(j,cal_mmu.cal_main)
						case 10:
							TH_OPRT(j,cal_mmu.cal_main)
					}
					break;
			}
		}
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 0, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[0][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 1, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[1][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 2, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[2][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 3, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[3][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 4, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[4][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 5, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[5][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 6, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[6][j]);
	}
	for (j=0; j<TH_OS_VBS_GRP_LEN; j++) {
		TDM_PRINT3("TDM: PIPE %d, MMU OVS BUCKET 7, element #%0d, contains physical port #%0d\n",_tdm->_core_data.vars_pkg.pipe,j,cal_mmu.cal_grp[7][j]);
	}
	
	return ( _tdm->_chip_exec[TDM_CHIP_EXEC__POST]( _tdm ) );
}
