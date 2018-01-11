/*
 * $Id: //depot/tomahawk2/dv/tdm/chip/include/tdm_th2_vec.h#7 $
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
 * TDM vector library for BCM56970
 */

#ifndef TDM_TH2_VECTOR_LIBRARY_H
#define TDM_TH2_VECTOR_LIBRARY_H
/**
@name: tdm_th2_vector_load
@param:

Generate vector at index in TDM vector map
**/
int
tdm_th2_vec_load(unsigned short **map, int bw, int port_speed, int yy, int len, int num_ext_ports)
{
	int i, n=0;
	
	switch (bw) {
		case 1700:
		case 1275:
		case 1133:
		case 850:
			switch (port_speed) {
				case 100:
					for (i=0; i<len; i+=(len/40)) {
						map[yy][i] = TOKEN_100G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 106:
					for (i=0; i<len; i+=(len/40)) {
						map[yy][i] = TOKEN_100G;
						n++;
						if (n==((100*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 50:
					for (i=0; i<len; i+=(len/20)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 53:
					for (i=0; i<len; i+=(len/20)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if (n==((50*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 40:
					for (i=0; i<len; i+=(len/16)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 42:
					for (i=0; i<len; i+=(len/16)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((40*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 25:
					for (i=0; i<len; i+=(len/10)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 27:
					for (i=0; i<len; i+=(len/10)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if (n==((25*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 20:
					for (i=0; i<len; i+=(len/8)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 21:
					for (i=0; i<len; i+=(len/8)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((20*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 10:
					for (i=0; i<len; i+=(len/4)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 11:
					for (i=0; i<len; i+=(len/4)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((10*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 1:
					map[yy][0] = TOKEN_1G;
					break;
			}
			break;
		default:
			switch (port_speed) {
				default:
					return 0;
					break;
			}
	}
	
	return 1;
	
}
#endif /* TDM_TH2_VECTOR_LIBRARY_H */
