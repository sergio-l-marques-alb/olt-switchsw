/*
 * $Id: tdm_ap_vec.h$
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * TDM vector library for BCM56860
 */

#ifndef TDM_AP_VECTOR_LIBRARY_H
#define TDM_AP_VECTOR_LIBRARY_H
/**
@name: tdm_ap_vector_load
@param:

Generate vector at index in TDM vector map
**/
int
tdm_ap_vec_load(unsigned short **map, int bw, int port_speed, int yy, int len, int num_ext_ports)
{
	int i, n=0;
	
	switch (bw)
	{
/* ################################################################################################################## */
    case 933: /*NAK AP*/
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
				/*		n=0;
						for (i=0; i<len; i+=(len/4)) {
							map[yy][i] = TOKEN_10G;
							if ((++n)==4) {
								break;
							}
						}*/
							map[yy][0] = TOKEN_10G;
							map[yy][58] = TOKEN_10G;
							map[yy][116] = TOKEN_10G;
							map[yy][175] = TOKEN_10G;
						break;
					case 12:
						n=0;
						for (i=0; i<len; i+=(len/5)) {
							map[yy][i] = TOKEN_12G;
							if ((++n)==5) {
								break;
							}
						}
						break;
					case 20:
					case 21:
						n=0;
						for (i=0; i<len; i+=(len/8)) {
							map[yy][i] = TOKEN_20G;
							if ((++n)==8) {
								break;
							}	
						}
						break;					
					case 40:
					case 41:
					case 42:
					/*	n=0;
						for (i=0; i<len; i+=(len/16)) {
							map[yy][i] = TOKEN_40G;
							if ((++n)==16) {
								break;
							}
						}*/
							map[yy][0] = TOKEN_40G;
							map[yy][15] = TOKEN_40G;
							map[yy][29] = TOKEN_40G;
							map[yy][44] = TOKEN_40G;
							map[yy][58] = TOKEN_40G;
							map[yy][73] = TOKEN_40G;
							map[yy][87] = TOKEN_40G;
							map[yy][102] = TOKEN_40G;
							map[yy][116] = TOKEN_40G;
							map[yy][131] = TOKEN_40G;
							map[yy][145] = TOKEN_40G;
							map[yy][160] = TOKEN_40G;
							map[yy][174] = TOKEN_40G;
							map[yy][189] = TOKEN_40G;
							map[yy][203] = TOKEN_40G;
							map[yy][218] = TOKEN_40G;
						break;
					case 100:
					case 106:
					case 107:
						n=0;
						for (i=0; i<len; i+=(len/40)) {
							map[yy][i] = TOKEN_100G;
							if ((++n)==40) {
								break;
							}
						}
						break;					
					case 120:
						n=0;
						for (i=0; i<len; i+=(len/48)) {
							map[yy][i] = TOKEN_120G;
							if ((++n)==48) {
								break;
							}
						}
						break;
				case 150: /* for ancl slots*/
						map[yy][0] = AP_ANCL_TOKEN;
						map[yy][26] = AP_ANCL_TOKEN;
						map[yy][52] = AP_ANCL_TOKEN;
						map[yy][78] = AP_ANCL_TOKEN;
						map[yy][104] = AP_ANCL_TOKEN;
						map[yy][129] = AP_ANCL_TOKEN;
						map[yy][155] = AP_ANCL_TOKEN;
						map[yy][181] = AP_ANCL_TOKEN;						
						map[yy][207] = AP_ANCL_TOKEN;						
						break;
					default:
						return 0;
						break;
			}
			break;
		case 793: /* TD2/+ */
		case 794: 
			switch (port_speed)
			{
				case 1:
					map[yy][0] = TOKEN_1G;
					break;
				case 10:
				case 11:
					map[yy][0] = TOKEN_10G;
					map[yy][50] = TOKEN_10G;
					map[yy][100] = TOKEN_10G;
					map[yy][150] = TOKEN_10G;
					break;
				case 12:
					map[yy][0] = TOKEN_12G;
					map[yy][36] = TOKEN_12G;
					map[yy][72] = TOKEN_12G;
					map[yy][120] = TOKEN_12G;
					map[yy][156] = TOKEN_12G;
					break;
				case 20:
				case 21:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = TOKEN_20G;
					}
					break;
				case 25:
					map[yy][0] = TOKEN_25G;
					map[yy][20] = TOKEN_25G;
					map[yy][40] = TOKEN_25G;
					map[yy][60] = TOKEN_25G;
					map[yy][80] = TOKEN_25G;
					map[yy][100] = TOKEN_25G;
					map[yy][120] = TOKEN_25G;
					map[yy][140] = TOKEN_25G;
					map[yy][160] = TOKEN_25G;
					map[yy][180] = TOKEN_25G;
					break;
				case 40:
				case 41:
				case 42:
				/*	for (i=0; i<len; i+=(VECTOR_QUANTA_S*3)) {
						map[yy][i] = TOKEN_40G;
					}*/			
		      map[yy][0] = TOKEN_40G;
		      map[yy][12] = TOKEN_40G;
		      map[yy][25] = TOKEN_40G;
		      map[yy][37] = TOKEN_40G;
		      map[yy][50] = TOKEN_40G;
		      map[yy][62] = TOKEN_40G;
		      map[yy][75] = TOKEN_40G;
		      map[yy][87] = TOKEN_40G;
		      map[yy][100] = TOKEN_40G;
		      map[yy][112] = TOKEN_40G;
		      map[yy][125] = TOKEN_40G;
		      map[yy][137] = TOKEN_40G;
		      map[yy][150] = TOKEN_40G;
		      map[yy][162] = TOKEN_40G;
		      map[yy][175] = TOKEN_40G;
		      map[yy][187] = TOKEN_40G;
					break;
				case 100:
				case 106:
				case 107:
			/*for (i=0; i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = TOKEN_100G;
						map[yy][i+5] = TOKEN_100G;
						map[yy][i+10] = TOKEN_100G;
						map[yy][i+15] = TOKEN_100G;
						map[yy][i+19] = TOKEN_100G;
					}
					break;*/
						n=0;
						for (i=0; i<len; i+=(len/40)) {
							map[yy][i] = TOKEN_100G;
							if ((++n)==40) {
								break;
							}
						}
          break;
				case 108:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S)) {
						map[yy][i] = TOKEN_100G;
					}
					break;
				case 109:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S)) {
						map[yy][i] = TOKEN_100G;
					}
					for (i=(VECTOR_QUANTA_S*5); i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = num_ext_ports;
					}
					break;				
				case 120:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S)) {
						map[yy][i] = TOKEN_120G;
					}
					break;
				case 150: /* for ancl slots*/
						map[yy][0] = AP_ANCL_TOKEN;
						map[yy][25] = AP_ANCL_TOKEN;
						map[yy][50] = AP_ANCL_TOKEN;
						map[yy][75] = AP_ANCL_TOKEN;
						map[yy][100] = AP_ANCL_TOKEN;
						map[yy][150] = AP_ANCL_TOKEN;
						map[yy][175] = AP_ANCL_TOKEN;						
						map[yy][199] = AP_ANCL_TOKEN;						
						break;
				default:
					return 0;
					break;
			}
			break;			
		case 575:
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
						map[yy][0] = TOKEN_10G;
						map[yy][36] = TOKEN_10G;
						map[yy][72] = TOKEN_10G;
						map[yy][108] = TOKEN_10G;
						break;
					case 12:
						n=0;
						for (i=0; i<len; i+=(len/5)) {
							map[yy][i] = TOKEN_40G;
							if ((++n)==5) {
								break;
							}	
						}
						break;
					case 20:
					case 21:
						map[yy][0] = TOKEN_20G;
						map[yy][36] = TOKEN_20G;
						map[yy][72] = TOKEN_20G;
						map[yy][109] = TOKEN_20G;
						map[yy][18] = TOKEN_20G;
						map[yy][54] = TOKEN_20G;
						map[yy][90] = TOKEN_20G;
						map[yy][127] = TOKEN_20G;
						break;					
					case 40:
					case 41:
					case 42:
					/*n=0;
						for (i=0; i<len; i+=(len/16)) {
							map[yy][i] = TOKEN_40G;
							if ((++n)==16) {
								break;
							}	
						}*/
						map[yy][0] = TOKEN_40G; /*use 9 and 10*/
						map[yy][9] = TOKEN_40G;
						map[yy][18] = TOKEN_40G;
						map[yy][27] = TOKEN_40G;
						map[yy][36] = TOKEN_40G;
						map[yy][45] = TOKEN_40G;
						map[yy][54] = TOKEN_40G;
						map[yy][63] = TOKEN_40G;
						map[yy][72] = TOKEN_40G;
						map[yy][81] = TOKEN_40G;
						map[yy][90] = TOKEN_40G;
						map[yy][99] = TOKEN_40G;
						map[yy][108] = TOKEN_40G;
						map[yy][117] = TOKEN_40G;
						map[yy][126] = TOKEN_40G;
						map[yy][135] = TOKEN_40G;
						break;
					case 100:
						n=0;
						for (i=0; i<len; i+=(len/40)) {
							map[yy][i] = TOKEN_100G;
							if ((++n)==40) {
								break;
							}
						}
						break;					
					case 120:
						n=0;
						for (i=0; i<len; i+=(len/48)) {
							map[yy][i] = TOKEN_120G;
							if ((++n)==48) {
								break;
							}
						}
						break;
					case 150: /* for ancl slots*/
						map[yy][17] = AP_ANCL_TOKEN;
						map[yy][26] = AP_ANCL_TOKEN;
						map[yy][53] = AP_ANCL_TOKEN;
						map[yy][62] = AP_ANCL_TOKEN;
						map[yy][89] = AP_ANCL_TOKEN;						
						map[yy][98] = AP_ANCL_TOKEN;						
						map[yy][125] = AP_ANCL_TOKEN;						
						map[yy][134] = AP_ANCL_TOKEN;						
						map[yy][144] = AP_ANCL_TOKEN;						
						break;
					default:
						return 0;
						break;
			}
			break;
		case 510:
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
						n=0;
						for (i=0; i<len; i+=(len/4)) {
							map[yy][i] = TOKEN_10G;
							if ((++n)==4) {
								break;
							}
						}
						break;
					case 12:
						n=0;
						for (i=0; i<len; i+=(len/5)) {
							map[yy][i] = TOKEN_12G;
							if ((++n)==5) {
								break;
							}
						}
						break;
					case 20:
					case 21:
						n=0;
						for (i=0; i<len; i+=(len/8)) {
							map[yy][i] = TOKEN_20G;
							if ((++n)==8) {
								break;
							}	
						}
						break;					
					case 40:
					case 41:
					case 42:
					case 43:
					case 44:
					case 45:
						n=0;
						for (i=0; i<len; i+=(len/16)) {
							map[yy][i] = TOKEN_40G;
							if ((++n)==16) {
								break;
							}
						}
						break;
					default:
						return 0;
						break;
			}
			break;
		case 435:
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
						map[yy][0] = TOKEN_10G;
						map[yy][27] = TOKEN_10G;
						map[yy][55] = TOKEN_10G;
						map[yy][82] = TOKEN_10G;
						break;
					case 12:
						map[yy][0] = TOKEN_12G;
						map[yy][19] = TOKEN_12G;
						map[yy][39] = TOKEN_12G;
						map[yy][58] = TOKEN_12G;
						map[yy][78] = TOKEN_12G;
						break;
					case 20:
					case 21:
						map[yy][0] = TOKEN_20G;
						map[yy][14] = TOKEN_20G;
						map[yy][28] = TOKEN_20G;
						map[yy][41] = TOKEN_20G;
						map[yy][55] = TOKEN_20G;
						map[yy][69] = TOKEN_20G;
						map[yy][82] = TOKEN_20G;
						map[yy][96] = TOKEN_20G;						
						break;
					case 40:
					case 41:
					case 42:
						map[yy][0] = TOKEN_40G; /*use 7 and 5*/
						map[yy][7] = TOKEN_40G;
						map[yy][14] = TOKEN_40G;
						map[yy][21] = TOKEN_40G;
						map[yy][28] = TOKEN_40G;
						map[yy][35] = TOKEN_40G;
						map[yy][42] = TOKEN_40G;
						map[yy][49] = TOKEN_40G;
						map[yy][56] = TOKEN_40G;
						map[yy][63] = TOKEN_40G;
						map[yy][70] = TOKEN_40G;
						map[yy][77] = TOKEN_40G;
						map[yy][82] = TOKEN_40G;
						map[yy][89] = TOKEN_40G;
						map[yy][96] = TOKEN_40G;
						map[yy][103] = TOKEN_40G;						
						break;
					case 150: /* for ancl slots*/
						map[yy][6] = AP_ANCL_TOKEN;
						map[yy][13] = AP_ANCL_TOKEN;
						map[yy][33] = AP_ANCL_TOKEN;
						map[yy][40] = AP_ANCL_TOKEN;
						map[yy][54] = AP_ANCL_TOKEN;
						map[yy][61] = AP_ANCL_TOKEN;
						map[yy][68] = AP_ANCL_TOKEN;
						map[yy][88] = AP_ANCL_TOKEN;						
						map[yy][95] = AP_ANCL_TOKEN;						
						map[yy][109] = AP_ANCL_TOKEN;						
						break;
					default:
						return 0;
						break;
			}
			break;			
		default:
			switch (port_speed)
			{
				default:
					return 0;
					break;
			}
	}
	
	return 1;
	
}
#endif /* TDM_AP_VECTOR_LIBRARY_H */
