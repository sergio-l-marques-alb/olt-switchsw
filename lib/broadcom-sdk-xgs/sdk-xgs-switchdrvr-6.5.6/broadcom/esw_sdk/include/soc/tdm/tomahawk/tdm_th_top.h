/*
 * $Id: tdm_th_top.h$
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
 * TDM top header for BCM56960
 */

#ifndef TDM_TH_PREPROCESSOR_DIRECTIVES_H
#define TDM_TH_PREPROCESSOR_DIRECTIVES_H

#ifdef _TDM_STANDALONE
	#include <tdm_th_defines.h>
#else
	#include <soc/tdm/tomahawk/tdm_th_defines.h>
#endif

#define TH_TOKEN_CHECK(a)  				\
			if (a!=TH_NUM_EXT_PORTS && 	\
				a!=TH_OVSB_TOKEN && 	\
				a!=TH_NULL_TOKEN &&		\
				a!=TH_RSVD_TOKEN &&		\
				a!=TH_MGM1_TOKEN &&		\
				a!=TH_LPB1_TOKEN && 	\
				a!=TH_MGM2_TOKEN && 	\
				a!=TH_LPB2_TOKEN && 	\
				a!=TH_LPB3_TOKEN && 	\
				a!=TH_CMIC_TOKEN && 	\
				a!=TH_ANCL_TOKEN &&		\
				a!=TH_IDL1_TOKEN &&		\
				a!=TH_IDL2_TOKEN &&		\
				a!=TH_LPB0_TOKEN) 		\

#define TH_CMIC(a,b) {								\
			b[a]=TH_CMIC_TOKEN; 					\
			TDM_PRINT0("CMIC/CPU\n");				\
			break;                                  \
		}
#define TH_LPB0(a,b) {										\
			b[a]=TH_LPB0_TOKEN; 							\
			TDM_PRINT0("TILE 0 LOOPBACK\n");				\
			break;                                          \
		}
#define TH_MGM1(a,b) {										\
			b[a]=TH_MGM1_TOKEN; 							\
			TDM_PRINT0("IDB 1 MANAGEMENT\n");				\
			break;                                          \
		}
#define TH_LPB1(a,b) {										\
			b[a]=TH_LPB1_TOKEN; 							\
			TDM_PRINT0("TILE 1 LOOPBACK\n");				\
			break;                                          \
		}
#define TH_MGM2(a,b) {										\
			b[a]=TH_MGM2_TOKEN; 							\
			TDM_PRINT0("IDB 2 MANAGEMENT\n");				\
			break;                                          \
		}
#define TH_LPB2(a,b) {										\
			b[a]=TH_LPB2_TOKEN; 							\
			TDM_PRINT0("TILE 2 LOOPBACK\n");				\
			break;                                          \
		}
#define TH_OPRT(a,b) {							\
			b[a]=TH_IDL2_TOKEN; 				\
			TDM_PRINT0("PURGE\n");				\
			break;								\
		}
#define TH_SBUS(a,b) {							\
			b[a]=TH_IDL1_TOKEN; 				\
			TDM_PRINT0("SBUS\n");				\
			break;								\
		}		
#define TH_NULL(a,b) {							\
			b[a]=TH_NULL_TOKEN; 				\
			TDM_PRINT0("NULL\n");				\
			break;								\
		}				
#define TH_LPB3(a,b) {							\
			b[a]=TH_LPB3_TOKEN; 				\
			TDM_PRINT0("TILE 3 LOOPBACK\n");	\
			break;								\
		}
#define TH_DOT0(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==TH_OVSB_TOKEN) {			\
					b[a]=TH_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=TH_CMIC_TOKEN;				\
				}									\
			}										\
			if (b[a]==TH_OVSB_TOKEN) {             	\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT CMIC/CPU\n");				\
			}                                               \
			break;                                          \
		}
#define TH_DOT1(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==TH_OVSB_TOKEN) {				\
					b[a]=TH_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=TH_MGM1_TOKEN;					\
				}										\
			}											\
			if (b[a]==TH_OVSB_TOKEN) {					\
				TDM_PRINT0("DOT OVERSUB\n");			\
			}                                               \
			else {                                                  \
				TDM_PRINT0("DOT IDB 1 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}			
#define TH_DOT2(a,b) {									\
			for (i=0; i<256; i++) {						\
				if (b[i]==TH_OVSB_TOKEN) {				\
					b[a]=TH_OVSB_TOKEN;					\
					break;								\
				}										\
				else {									\
					b[a]=TH_MGM2_TOKEN;					\
				}										\
			}											\
			if (b[a]==TH_OVSB_TOKEN) {                    			\
				TDM_PRINT0("DOT OVERSUB\n");						\
			}                                                       \
			else {                                                  \
				TDM_PRINT0("DOT IDB 2 MANAGEMENT\n");				\
			}                                                       \
			break;                                                  \
		}
#define TH_DOT3(a,b) {								\
			for (i=0; i<256; i++) {					\
				if (b[i]==TH_OVSB_TOKEN) {			\
					b[a]=TH_OVSB_TOKEN;				\
					break;							\
				}									\
				else {								\
					b[a]=TH_IDL2_TOKEN;				\
				}									\
			}										\
			if (b[a]==TH_OVSB_TOKEN) {    			\
				TDM_PRINT0("DOT OVERSUB\n");		\
			}                                               \
			else {                                          \
				TDM_PRINT0("DOT PURGE\n");					\
			}                                               \
			break;                                          \
		}

typedef struct th_ll_node {
	unsigned short port;
	struct th_ll_node *next;
} th_ll_node;

#ifdef _TDM_STANDALONE
	#include <tdm_th_methods.h>
#else
	#include <soc/tdm/tomahawk/tdm_th_methods.h>
#endif

/* API shim layer */
LINKER_DECL tdm_mod_t* tdm_chip_th_shim__which_tsc( int port, int tsc[TH_NUM_PHY_PM][TH_NUM_PM_LNS] );
LINKER_DECL tdm_mod_t* tdm_chip_th_shim__check_ethernet_d( int port, enum port_speed_e speed[TH_NUM_EXT_PORTS], enum port_state_e state[TH_NUM_EXT_PORTS], int **tsc, int traffic[TH_NUM_PM_MOD] );

#endif /* TDM_TH_PREPROCESSOR_DIRECTIVES_H */
