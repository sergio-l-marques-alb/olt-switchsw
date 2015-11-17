/*
 * $Id: set_tdm.c,v 1.1.10.25 Broadcom SDK $
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
 *
 * File:        set_tdm.c
 * Purpose:
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/tdm_tomahawk.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <sal/core/alloc.h>
typedef int FILE;

#ifndef TDM_PREPROCESSOR_DIRECTIVES
#define TDM_PREPROCESSOR_DIRECTIVES

/*  No variadic macros */
#define TDM_BIG_BAR  LOG_VERBOSE(BSL_LS_SOC_TDM,                        \
                                 (BSL_META("TDM: #################################################################################################################################\n")));
#define TDM_SML_BAR  LOG_VERBOSE(BSL_LS_SOC_TDM,                        \
                                 (BSL_META("TDM: ---------------------------------------------------------------------------------------------------------------------------------\n")));

#ifdef _STD_ALLOC
	#define TDM_ALLOC(a,b) malloc(a)
	#define TDM_FREE(a) free(a)
#else
	#define TDM_ALLOC(_sz,_id) sal_alloc(_sz,_id)
	#define TDM_FREE(_id) sal_free(_id)
#endif


/* Minimum freq in MHz at which all HG speeds are supported */
#define MIN_HG_FREQ 545

/* Max clk frquency in MHz */
#define MAX_FREQ 850

/* Granularity of TDM in 100MBps */
#define BW_QUANTA 25

/* TDM length based on frequency */
/* TH */
#define LEN_850MHZ_HG 215
#define LEN_850MHZ_EN 224
#define LEN_765MHZ_HG 194
#define LEN_765MHZ_EN 202
#define LEN_672MHZ_HG 170
#define LEN_672MHZ_EN 177
#define LEN_645MHZ_HG 163
#define LEN_645MHZ_EN 170
#define LEN_545MHZ_HG 138
#define LEN_545MHZ_EN 143
/* TD2/TD2+ */
#define LEN_760MHZ_HG 200
#define LEN_760MHZ_EN 200
#define LEN_608MHZ_HG 170
#define LEN_608MHZ_EN 170
#define LEN_517MHZ_HG 136
#define LEN_517MHZ_EN 136
#define LEN_415MHZ_HG 126
#define LEN_415MHZ_EN 126

/* Length of TDM vector map - max length of TDM memory */
#define VEC_MAP_LEN 255
/* Width of TDM vector map - max number of vectors to reduce */
#define VEC_MAP_WID 33
/* Scan sees blank row or column */
#define BLANK 12345

/* Port tokens */
#define TOKEN_120G _TH_NUM_EXT_PORTS+119
#define TOKEN_106G _TH_NUM_EXT_PORTS+106
#define TOKEN_100G _TH_NUM_EXT_PORTS+100
#define TOKEN_40G _TH_NUM_EXT_PORTS+40
#define TOKEN_50G _TH_NUM_EXT_PORTS+50
#define TOKEN_25G _TH_NUM_EXT_PORTS+25
#define TOKEN_20G _TH_NUM_EXT_PORTS+20
#define TOKEN_12G _TH_NUM_EXT_PORTS+12
#define TOKEN_10G _TH_NUM_EXT_PORTS+11

/* Number of oversub speed groups */
#define OS_GROUP_NUM 8

/* 850MHz spacing quanta */
#define VECTOR_QUANTA_F 5
#define VECTOR_QUANTA_S 4
#define P_100G_HG_850MHZ 5
#define P_50G_HG_850MHZ 10
#define P_40G_HG_850MHZ 12
#define P_25G_HG_850MHZ 20
#define P_20G_HG_850MHZ 25
#define P_10G_HG_850MHZ 50

/* E2E CT Analysis Jitter Limits */
#define VECTOR_ISOLATION 0
#define JITTER_THRESH_LO 1 /* For 100G */
#define JITTER_THRESH_ML 2 /* For 50G, 40G */
#define JITTER_THRESH_MH 3 /* For 25G, 20G */
#define JITTER_THRESH_HI 5 /* For 10G */

/* Filter sensitivity */
#define DITHER_THRESHOLD 7
#define DITHER_PASS 10

/* C Checker Constants */
#define PKT_SIZE 145
#define PKT_PREAMBLE 8
#define PKT_IPG 12

#define TIMEOUT 1000

#define UP -1
#define DN 1
#define BOOL_TRUE 1
#define PASS 1
#define BOOL_FALSE 0
#define FAIL 0
#define UNDEF 254







#endif
/* Number of ancillary ports in calendar */
#define TH_ANCILLARY_PORTS 10

/* Pipe 0 - CPU slot */
#define CMIC_TOKEN 0
/* Pipe 0 - loopback slot */
#define LPB0_TOKEN (_TH_NUM_EXT_PORTS-4)
/* Pipe 1 - management slot */
#define MGM1_TOKEN (_TH_NUM_EXT_PORTS-7)
/* Pipe 1 - loopback slot */
#define LPB1_TOKEN (_TH_NUM_EXT_PORTS-3)
/* Pipe 2 - management slot */
#define MGM2_TOKEN (_TH_NUM_EXT_PORTS-5)
/* Pipe 2 - loopback slot */
#define LPB2_TOKEN (_TH_NUM_EXT_PORTS-2)
/* Reserved slot */
#define RSVD_TOKEN (_TH_NUM_EXT_PORTS-6)
/* Pipe 3 - loopback slot */
#define LPB3_TOKEN (_TH_NUM_EXT_PORTS-1)

#define TOKEN_CHECK(a)  				\
			if (a!=_TH_NUM_EXT_PORTS && 	\
				a!=OVSB_TOKEN && 		\
				a!=NULL_TOKEN &&		\
				a!=RSVD_TOKEN &&		\
				a!=MGM1_TOKEN &&		\
				a!=LPB1_TOKEN && 		\
				a!=MGM2_TOKEN && 		\
				a!=LPB2_TOKEN && 		\
				a!=LPB3_TOKEN && 		\
				a!=CMIC_TOKEN && 		\
				a!=ACC_TOKEN &&			\
				a!=IDL1_TOKEN &&		\
				a!=IDL2_TOKEN &&		\
				a!=LPB0_TOKEN) 			\

/* TDM table accessory token */
#define ACC_TOKEN 	(_TH_NUM_EXT_PORTS+9)
#define OVF_TOKEN	(_TH_NUM_EXT_PORTS+10)

#define CMIC(a) {						\
			tdm_tbl[a]=CMIC_TOKEN; 		\
			LOG_VERBOSE(BSL_LS_SOC_TDM,             \
                        (BSL_META("CMIC/CPU\n")));  \
			break;                                  \
		}
#define LPB0(a) {								\
			tdm_tbl[a]=LPB0_TOKEN; 				\
			LOG_VERBOSE(BSL_LS_SOC_TDM,                     \
                        (BSL_META("TILE 0 LOOPBACK\n")));   \
			break;                                          \
		}
#define MGM1(a) {								\
			tdm_tbl[a]=MGM1_TOKEN; 				\
			LOG_VERBOSE(BSL_LS_SOC_TDM,                     \
                        (BSL_META("IDB 1 MANAGEMENT\n")));  \
			break;                                          \
		}
#define LPB1(a) {								\
			tdm_tbl[a]=LPB1_TOKEN; 				\
			LOG_VERBOSE(BSL_LS_SOC_TDM,                     \
                        (BSL_META("TILE 1 LOOPBACK\n")));   \
			break;                                          \
		}
#define MGM2(a) {								\
			tdm_tbl[a]=MGM2_TOKEN; 				\
			LOG_VERBOSE(BSL_LS_SOC_TDM,                     \
                        (BSL_META("IDB 2 MANAGEMENT\n")));  \
			break;                                          \
		}
#define LPB2(a) {								\
			tdm_tbl[a]=LPB2_TOKEN; 				\
			LOG_VERBOSE(BSL_LS_SOC_TDM,                     \
                        (BSL_META("TILE 2 LOOPBACK\n")));   \
			break;                                          \
		}
#define OPRT(a) {								\
			tdm_tbl[a]=IDL2_TOKEN; 				\
			LOG_VERBOSE(BSL_LS_SOC_TDM,         \
                        (BSL_META("PURGE\n"))); \
			break;								\
		}
#define SBUS(a) {								\
			tdm_tbl[a]=IDL1_TOKEN; 				\
			LOG_VERBOSE(BSL_LS_SOC_TDM,         \
                        (BSL_META("SBUS\n")));  \
			break;								\
		}		
#define NULL_SLOT(a) {							\
			tdm_tbl[a]=NULL_TOKEN; 				\
			LOG_VERBOSE(BSL_LS_SOC_TDM,         \
                        (BSL_META("NULL\n")));  \
			break;								\
		}				
#define LPB3(a) {								\
			tdm_tbl[a]=LPB3_TOKEN; 				\
			LOG_VERBOSE(BSL_LS_SOC_TDM,                     \
                        (BSL_META("TILE 3 LOOPBACK\n")));   \
			break;                                          \
		}
#define DOT0(a) {									\
			for (i=0; i<256; i++) {					\
				if (tdm_tbl[i]==OVSB_TOKEN) {		\
					tdm_tbl[a]=OVSB_TOKEN;			\
					break;							\
				}									\
				else {								\
					tdm_tbl[a]=CMIC_TOKEN;			\
				}									\
			}										\
			if (tdm_tbl[a]==OVSB_TOKEN) {                   \
				LOG_VERBOSE(BSL_LS_SOC_TDM,                 \
                            (BSL_META("DOT OVERSUB\n")));   \
			}                                               \
			else {                                          \
				LOG_VERBOSE(BSL_LS_SOC_TDM,                 \
                            (BSL_META("DOT CMIC/CPU\n")));  \
			}                                               \
			break;                                          \
		}
#define DOT1(a) {										\
			for (i=0; i<256; i++) {						\
				if (tdm_tbl[i]==OVSB_TOKEN) {			\
					tdm_tbl[a]=OVSB_TOKEN;				\
					break;								\
				}										\
				else {									\
					tdm_tbl[a]=MGM1_TOKEN;				\
				}										\
			}											\
			if (tdm_tbl[a]==OVSB_TOKEN) {				\
				LOG_VERBOSE(BSL_LS_SOC_TDM,                 \
                            (BSL_META("DOT OVERSUB\n")));   \
			}                                               \
			else {                                                  \
				LOG_VERBOSE(BSL_LS_SOC_TDM,                         \
                            (BSL_META("DOT IDB 1 MANAGEMENT\n")));	\
			}                                                       \
			break;                                                  \
		}			
#define DOT2(a) {										\
			for (i=0; i<256; i++) {						\
				if (tdm_tbl[i]==OVSB_TOKEN) {			\
					tdm_tbl[a]=OVSB_TOKEN;				\
					break;								\
				}										\
				else {									\
					tdm_tbl[a]=MGM2_TOKEN;				\
				}										\
			}											\
			if (tdm_tbl[a]==OVSB_TOKEN) {                           \
				LOG_VERBOSE(BSL_LS_SOC_TDM,                         \
                            (BSL_META("DOT OVERSUB\n")));           \
			}                                                       \
			else {                                                  \
				LOG_VERBOSE(BSL_LS_SOC_TDM,                         \
                            (BSL_META("DOT IDB 2 MANAGEMENT\n")));	\
			}                                                       \
			break;                                                  \
		}
#define DOT3(a) {									\
			for (i=0; i<256; i++) {					\
				if (tdm_tbl[i]==OVSB_TOKEN) {		\
					tdm_tbl[a]=OVSB_TOKEN;			\
					break;							\
				}									\
				else {								\
					tdm_tbl[a]=IDL2_TOKEN;			\
				}									\
			}										\
			if (tdm_tbl[a]==OVSB_TOKEN) {                   \
				LOG_VERBOSE(BSL_LS_SOC_TDM,                 \
                            (BSL_META("DOT OVERSUB\n")));   \
			}                                               \
			else {                                          \
				LOG_VERBOSE(BSL_LS_SOC_TDM,                 \
                            (BSL_META("DOT PURGE\n")));     \
			}                                               \
			break;                                          \
		}

static int TDM_scheduler_2d(FILE *file, int freq, enum port_speed speed[_TH_NUM_EXT_PORTS], int port_state_map[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_buffer[64], int os_buffer[64], int lr_idx_limit, int tdm_tbl[256], int bucket1[_TH_OS_GROUP_LEN], int bucket2[_TH_OS_GROUP_LEN], int bucket3[_TH_OS_GROUP_LEN], int bucket4[_TH_OS_GROUP_LEN], int bucket5[_TH_OS_GROUP_LEN], int bucket6[_TH_OS_GROUP_LEN], int bucket7[_TH_OS_GROUP_LEN], int bucket8[_TH_OS_GROUP_LEN], int pipe, unsigned char accessories, unsigned char higig_mgmt);
static int TDM_scheduler_PQ(int f);
static int check_type_2(unsigned char port, int tsc[NUM_TSC][4]);
static int TDM_scheduler_slice_size_local(unsigned char idx, int tdm[256], int lim);
static int TDM_scheduler_slice_prox_local(unsigned char idx, int tdm[256], int lim, int tsc[NUM_TSC][4]);
static int TDM_scheduler_slice_size(unsigned char port, int tdm[256], int lim);
static int TDM_scheduler_slice_idx(unsigned char port, int tdm[256], int lim);
static int TDM_scheduler_slice_prox_dn(int slot, int tdm[256], int lim, int tsc[NUM_TSC][4]);
static int TDM_scheduler_slice_prox_up(int slot, int tdm[256], int tsc[NUM_TSC][4]);
static int TDM_scheduler_exists(int e, int arr[107]);
static int TDM_scheduler_fit_prox(unsigned char **map, int lim, int wid, int node_x, int tsc[NUM_TSC][4]);
static int TDM_scheduler_fit_prox_node(unsigned char **map, int node_y, int wid, int node_x, int tsc[NUM_TSC][4], int new_node_y);
static int TDM_scheduler_fit_singular_cnt(unsigned char **map, int node_y);
static int TDM_scheduler_fit_singular_col(unsigned char **map, int node_x, int lim);
static int TDM_scheduler_map_find_x(unsigned char **map, int limit, int idx, int principle);
static int TDM_scheduler_map_find_y(unsigned char **map, int limit, int idx, int principle);
static int TDM_scheduler_map_search_x(unsigned char **map, int idx);
static int TDM_scheduler_map_shortest_distance(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2);
static int TDM_scheduler_map_shortest_distance_calc(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2, unsigned int start);
static int TDM_scheduler_nodal_slice(unsigned char **map, int freq, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt, int direction);
static int TDM_scheduler_nodal_slice_lr(unsigned char **map, int freq, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt, int direction, int curr_cnt);
static int TDM_scheduler_nodal_transmute_lr(unsigned char **map, int freq, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt);
static int TDM_scheduler_slots(int port_speed);
static int TDM_scheduler_map_transpose(unsigned char **map, int limit, unsigned int xpos_1, unsigned int xpos_2);
static int TDM_scheduler_vector_diff(unsigned char **map, int bw, int lim, int x, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1]);
static void TDM_scheduler_vector_dump(FILE *file, unsigned char **map, int idx);
static int TDM_scheduler_vector_load(unsigned char **map, int bw, int port_speed, int yy, int len);
static void TDM_scheduler_vector_rotate(unsigned char vector[], int size, int step);
static void TDM_scheduler_vector_rotate_step(unsigned char vector[], int size, int step);
static int TDM_scheduler_weight_min(int weights[VEC_MAP_WID], int span);
static void TDM_scheduler_weight_update(unsigned char **map, int lim, int weights[VEC_MAP_WID], int weights_last[VEC_MAP_WID], int weights_cmp[VEC_MAP_WID], int span, int mpass, int lr_slot_cnt);
static int TDM_scheduler_weight_vnum(int weights[VEC_MAP_WID], int span, int threshold);
static int TDM_scheduler_wrap(FILE *file, int bw, enum port_speed speed[_TH_NUM_EXT_PORTS], int portmap[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int pipe_start, int pipe_end, 
					   int idb_tdm_tbl[256], 
					   int idb_tdm_ovs_a[_TH_OS_GROUP_LEN], int idb_tdm_ovs_b[_TH_OS_GROUP_LEN], int idb_tdm_ovs_c[_TH_OS_GROUP_LEN], int idb_tdm_ovs_d[_TH_OS_GROUP_LEN], 
					   int idb_tdm_ovs_e[_TH_OS_GROUP_LEN], int idb_tdm_ovs_f[_TH_OS_GROUP_LEN], int idb_tdm_ovs_g[_TH_OS_GROUP_LEN], int idb_tdm_ovs_h[_TH_OS_GROUP_LEN], 
					   int mmu_tdm_tbl[256], 
					   int mmu_tdm_ovs_a[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_b[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_c[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_d[_TH_OS_GROUP_LEN],
					   int mmu_tdm_ovs_e[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_f[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_g[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_h[_TH_OS_GROUP_LEN]);
static void tsc_port_transcription(int wc_array[NUM_TSC][4], enum port_speed speed[_TH_NUM_EXT_PORTS], int port_state_array[_TH_NUM_EXT_PORTS]);
static int check_ethernet(int port, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1]);
static void parse_tdm_tbl(FILE *file, int mgmt_bw, int tdm_tbl[256], int tdm_ovs_a[_TH_OS_GROUP_LEN], int tdm_ovs_b[_TH_OS_GROUP_LEN], int tdm_ovs_c[_TH_OS_GROUP_LEN], int tdm_ovs_d[_TH_OS_GROUP_LEN], int tdm_ovs_e[_TH_OS_GROUP_LEN], int tdm_ovs_f[_TH_OS_GROUP_LEN], int tdm_ovs_g[_TH_OS_GROUP_LEN], int tdm_ovs_h[_TH_OS_GROUP_LEN], int block, int pipe);
static void print_tsc(FILE *file, int wc_array[NUM_TSC][4]);
static int tdm_abs(int num);
static int tdm_fac(int num_signed);
static int tdm_pow(int num, int pow);
static int tdm_sqrt(int input_signed);
static void tdm_ver(int ver[2]);
static int which_tsc(unsigned char port, int tsc[NUM_TSC][4]);
static void TDM_scheduler_filter_dither(int tdm_tbl[256], int lr_idx_limit, int accessories, int tsc[NUM_TSC][4], int threshold);
static int TDM_scheduler_filter_fine_dither(int port, int tdm_tbl[256], int lr_idx_limit, int accessories, int tsc[NUM_TSC][4]);
static void TDM_scheduler_filter_ancillary_smooth(int port, int tdm_tbl[256], int lr_idx_limit, int accessories);
static void TDM_scheduler_filter_refactor(int tdm_tbl[256], int lr_idx_limit, int accessories, int lr_vec_cnt);
static void TDM_scheduler_filter_local_slice_up(int port, int tdm_tbl[256], int tsc[NUM_TSC][4]);
static void TDM_scheduler_filter_local_slice_dn(int port, int tdm_tbl[256], int tsc[NUM_TSC][4]);
static void TDM_scheduler_filter_local_slice_oversub(int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4]);
static int TDM_scheduler_filter_slice_dn(int port, int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4]);
static int TDM_scheduler_filter_slice_up(int port, int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4]);
#ifndef TDM_VECTOR_LIBRARY
#define TDM_VECTOR_LIBRARY
/**
@name: TDM_scheduler_vector_load
@param:

Generate vector at index in TDM vector map
**/
int TDM_scheduler_vector_load(unsigned char **map, int bw, int port_speed, int yy, int len)
{
	int i, j=0, n=0;
	
	switch (bw)
	{
		case 850: /* TH */
			switch (port_speed)
			{
				case 10:
					map[yy][0] = TOKEN_10G; 
					map[yy][53] = TOKEN_10G; 
					map[yy][107] = TOKEN_10G; 
					map[yy][160] = TOKEN_10G;
					break;
				case 11:
					map[yy][0] = TOKEN_10G; 
					map[yy][51] = TOKEN_10G; 
					map[yy][102] = TOKEN_10G;
					map[yy][153] = TOKEN_10G;
					break;
				case 20:
					map[yy][0] = TOKEN_20G; 
					map[yy][26] = TOKEN_20G; 
					map[yy][53] = TOKEN_20G; 
					map[yy][80] = TOKEN_20G;
					map[yy][107] = TOKEN_20G; 
					map[yy][133] = TOKEN_20G; 
					map[yy][160] = TOKEN_20G; 
					map[yy][187] = TOKEN_20G;
					break;
				case 21:
					map[yy][0] = TOKEN_20G; 
					map[yy][25] = TOKEN_20G; 
					map[yy][51] = TOKEN_20G; 
					map[yy][76] = TOKEN_20G;
					map[yy][102] = TOKEN_20G; 
					map[yy][127] = TOKEN_20G; 
					map[yy][153] = TOKEN_20G; 
					map[yy][179] = TOKEN_20G;
					break;
				case 25:
					map[yy][0] = TOKEN_25G;
					map[yy][20] = TOKEN_25G;
					map[yy][42] = TOKEN_25G;
					map[yy][64] = TOKEN_25G;
					map[yy][86] = TOKEN_25G;
					map[yy][107] = TOKEN_25G;
					map[yy][127] = TOKEN_25G;
					map[yy][149] = TOKEN_25G;
					map[yy][171] = TOKEN_25G;
					map[yy][193] = TOKEN_25G;
					break;
				case 27:
					map[yy][0] = TOKEN_25G;
					map[yy][20] = TOKEN_25G;
					map[yy][41] = TOKEN_25G;
					map[yy][61] = TOKEN_25G;
					map[yy][82] = TOKEN_25G;
					map[yy][102] = TOKEN_25G;
					map[yy][122] = TOKEN_25G;
					map[yy][143] = TOKEN_25G;
					map[yy][164] = TOKEN_25G;
					map[yy][185] = TOKEN_25G;
					break;
				case 40:
					map[yy][0] = TOKEN_40G;
					map[yy][13] = TOKEN_40G;
					map[yy][26] = TOKEN_40G;
					map[yy][39] = TOKEN_40G;
					map[yy][53] = TOKEN_40G;
					map[yy][66] = TOKEN_40G;
					map[yy][80] = TOKEN_40G;
					map[yy][93] = TOKEN_40G;
					map[yy][107] = TOKEN_40G;
					map[yy][120] = TOKEN_40G;
					map[yy][133] = TOKEN_40G;
					map[yy][146] = TOKEN_40G;
					map[yy][160] = TOKEN_40G;
					map[yy][173] = TOKEN_40G;
					map[yy][187] = TOKEN_40G;
					map[yy][200] = TOKEN_40G;
					break;
				case 42:
					map[yy][0] = TOKEN_40G;
					map[yy][12] = TOKEN_40G;
					map[yy][25] = TOKEN_40G;
					map[yy][38] = TOKEN_40G;
					map[yy][51] = TOKEN_40G;
					map[yy][63] = TOKEN_40G;
					map[yy][76] = TOKEN_40G;
					map[yy][89] = TOKEN_40G;
					map[yy][102] = TOKEN_40G;
					map[yy][114] = TOKEN_40G;
					map[yy][127] = TOKEN_40G;
					map[yy][140] = TOKEN_40G;
					map[yy][153] = TOKEN_40G;
					map[yy][166] = TOKEN_40G;
					map[yy][179] = TOKEN_40G;
					map[yy][192] = TOKEN_40G;
					break;
				case 43:
					map[yy][4] = TOKEN_40G;
					map[yy][11] = TOKEN_40G;
					map[yy][18] = TOKEN_40G;
					map[yy][25] = TOKEN_40G;
					map[yy][40] = TOKEN_40G;
					map[yy][55] = TOKEN_40G;
					map[yy][70] = TOKEN_40G;
					map[yy][85] = TOKEN_40G;
					map[yy][100] = TOKEN_40G;
					map[yy][115] = TOKEN_40G;
					map[yy][130] = TOKEN_40G;
					map[yy][145] = TOKEN_40G;
					map[yy][160] = TOKEN_40G;
					map[yy][175] = TOKEN_40G;
					map[yy][190] = TOKEN_40G;
					break;
				case 44:
					map[yy][5] = TOKEN_40G;
					map[yy][12] = TOKEN_40G;
					map[yy][19] = TOKEN_40G;
					map[yy][30] = TOKEN_40G;
					map[yy][45] = TOKEN_40G;
					map[yy][60] = TOKEN_40G;
					map[yy][75] = TOKEN_40G;
					map[yy][90] = TOKEN_40G;
					map[yy][105] = TOKEN_40G;
					map[yy][120] = TOKEN_40G;
					map[yy][135] = TOKEN_40G;
					map[yy][150] = TOKEN_40G;
					map[yy][165] = TOKEN_40G;
					map[yy][180] = TOKEN_40G;
					map[yy][195] = TOKEN_40G;
					break;
				case 45:
					map[yy][6] = TOKEN_40G;
					map[yy][13] = TOKEN_40G;
					map[yy][20] = TOKEN_40G;
					map[yy][35] = TOKEN_40G;
					map[yy][50] = TOKEN_40G;
					map[yy][65] = TOKEN_40G;
					map[yy][80] = TOKEN_40G;
					map[yy][95] = TOKEN_40G;
					map[yy][110] = TOKEN_40G;
					map[yy][125] = TOKEN_40G;
					map[yy][140] = TOKEN_40G;
					map[yy][155] = TOKEN_40G;
					map[yy][170] = TOKEN_40G;
					map[yy][185] = TOKEN_40G;
					map[yy][200] = TOKEN_40G;
					break;
				case 50:
					map[yy][0] = TOKEN_50G;
					map[yy][10] = TOKEN_50G;
					map[yy][20] = TOKEN_50G;
					map[yy][32] = TOKEN_50G;
					map[yy][42] = TOKEN_50G;
					map[yy][53] = TOKEN_50G;
					map[yy][64] = TOKEN_50G;
					map[yy][74] = TOKEN_50G;
					map[yy][86] = TOKEN_50G;
					map[yy][96] = TOKEN_50G;
					map[yy][107] = TOKEN_50G;
					map[yy][117] = TOKEN_50G;
					map[yy][127] = TOKEN_50G;
					map[yy][139] = TOKEN_50G;
					map[yy][149] = TOKEN_50G;
					map[yy][160] = TOKEN_50G;					
					map[yy][171] = TOKEN_50G;
					map[yy][181] = TOKEN_50G;
					map[yy][193] = TOKEN_50G;
					map[yy][203] = TOKEN_50G;
					break;
				case 53:
					map[yy][0] = TOKEN_50G;
					map[yy][10] = TOKEN_50G;
					map[yy][20] = TOKEN_50G;
					map[yy][31] = TOKEN_50G;
					map[yy][41] = TOKEN_50G;
					map[yy][51] = TOKEN_50G;
					map[yy][61] = TOKEN_50G;
					map[yy][71] = TOKEN_50G;
					map[yy][82] = TOKEN_50G;
					map[yy][92] = TOKEN_50G;
					map[yy][102] = TOKEN_50G;
					map[yy][112] = TOKEN_50G;
					map[yy][122] = TOKEN_50G;
					map[yy][133] = TOKEN_50G;
					map[yy][143] = TOKEN_50G;
					map[yy][153] = TOKEN_50G;					
					map[yy][164] = TOKEN_50G;
					map[yy][174] = TOKEN_50G;
					map[yy][185] = TOKEN_50G;
					map[yy][195] = TOKEN_50G;
					break;					
				case 100:
					for (i=0; i<len; i+=VECTOR_QUANTA_F) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ( (j==4)||(j==5)||(j==9)||(j==10)||(j==14)||(j==15)||(j==19)||(j==24)||(j==25)||(j==29)||(j==30)||(j==34)||(j==35) ) {
							i++;
						}
						j++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}				
					break;
				case 106:
					for (i=0; i<len; i+=VECTOR_QUANTA_F) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ( (j==5)||(j==15)||(j==25)||(j==30)||(j==35) ) {
							i++;
						}
						j++;
						if (n==((100*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 107:
					for (i=0; i<len; i+=VECTOR_QUANTA_F) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ( (j==0)||(j==1)||(j==2) ) {
							i+=2;
						}
						j++;
						if (n==39) {
							break;
						}
					}
					break;
				default:
					return 0;
					break;
			}
			break;
		case 766:
		case 765: /* TH */
			switch (port_speed)
			{
				case 10:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*12)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;			
				case 11:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*9)+1)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((10*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 20:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;					
				case 21:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*4)+3)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((20*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 25:
					map[yy][0] = TOKEN_25G;
					map[yy][20] = TOKEN_25G;
					map[yy][39] = TOKEN_25G;
					map[yy][58] = TOKEN_25G;
					map[yy][77] = TOKEN_25G;
					map[yy][96] = TOKEN_25G;
					map[yy][116] = TOKEN_25G;
					map[yy][135] = TOKEN_25G;
					map[yy][154] = TOKEN_25G;
					map[yy][173] = TOKEN_25G;
					break;				
				case 27:
					for (i=0; i<len; i+=((VECTOR_QUANTA_S*4)+2)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if ( (j==1)||(j==2)||(j==6)||(j==7) ) {
							i++;
						}
						j++;						
						if (n==((25*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 40:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*3)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;					
				case 42:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*2)+1)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if ((j%2)==0) {
							i++; j=0;
						}
						j++;						
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 50:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*2)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if ( (j==2)||(j==4)||(j==7)||(j==9)||(j==12)||(j==14)||(j==17) ) {
							i--;
						}
						j++;						
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 53:
					map[yy][0] = TOKEN_50G;
					map[yy][10] = TOKEN_50G;
					map[yy][19] = TOKEN_50G;
					map[yy][28] = TOKEN_50G;
					map[yy][38] = TOKEN_50G;
					map[yy][46] = TOKEN_50G;
					map[yy][56] = TOKEN_50G;
					map[yy][65] = TOKEN_50G;
					map[yy][74] = TOKEN_50G;
					map[yy][84] = TOKEN_50G;
					map[yy][92] = TOKEN_50G;
					map[yy][102] = TOKEN_50G;
					map[yy][111] = TOKEN_50G;
					map[yy][120] = TOKEN_50G;
					map[yy][130] = TOKEN_50G;
					map[yy][138] = TOKEN_50G;
					map[yy][148] = TOKEN_50G;
					map[yy][157] = TOKEN_50G;
					map[yy][166] = TOKEN_50G;
					map[yy][176] = TOKEN_50G;
					break;
				case 100:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F)) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ((((j+1)%5)==0)&&(j<40)) {
							i--;
						}
						j++;						
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;				
				case 106:
					for (i=0; i<len; i+=VECTOR_QUANTA_S) {
						map[yy][i] = TOKEN_100G;
						n++; j++;
						if (j==1||j==2||j==3) {
							i++;
						}
						if (j==5) {
							j=0;
						}
						if (n==((100*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				default:
					return 0;
					break;
			}
			break;
		case 672: /* TH */
			switch (port_speed)
			{
				case 100:
					for (i=0; i<len; i+=VECTOR_QUANTA_S) {
						map[yy][i] = TOKEN_100G;
						n++;
						if ( ((j+1)%5==0)&&(j<40)) {
							i++;
						}
						j++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 106:
					for (i=0; i<len; i+=VECTOR_QUANTA_S) {
						map[yy][i] = TOKEN_100G;
						n++;
						if (n==((100*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 50:
					map[yy][0] = TOKEN_50G;
					map[yy][8] = TOKEN_50G;
					map[yy][16] = TOKEN_50G;
					map[yy][25] = TOKEN_50G;
					map[yy][33] = TOKEN_50G;
					map[yy][42] = TOKEN_50G;
					map[yy][50] = TOKEN_50G;
					map[yy][58] = TOKEN_50G;
					map[yy][67] = TOKEN_50G;
					map[yy][75] = TOKEN_50G;
					map[yy][84] = TOKEN_50G;
					map[yy][92] = TOKEN_50G;
					map[yy][100] = TOKEN_50G;
					map[yy][109] = TOKEN_50G;
					map[yy][117] = TOKEN_50G;
					map[yy][126] = TOKEN_50G;
					map[yy][134] = TOKEN_50G;
					map[yy][142] = TOKEN_50G;
					map[yy][151] = TOKEN_50G;
					map[yy][159] = TOKEN_50G;
					break;
				case 53:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*2)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if (n==((50*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 40:
					map[yy][0] = TOKEN_40G;
					map[yy][10] = TOKEN_40G;
					map[yy][21] = TOKEN_40G;
					map[yy][31] = TOKEN_40G;
					map[yy][42] = TOKEN_40G;
					map[yy][52] = TOKEN_40G;
					map[yy][63] = TOKEN_40G;
					map[yy][73] = TOKEN_40G;
					map[yy][84] = TOKEN_40G;
					map[yy][94] = TOKEN_40G;
					map[yy][105] = TOKEN_40G;
					map[yy][115] = TOKEN_40G;
					map[yy][126] = TOKEN_40G;
					map[yy][136] = TOKEN_40G;
					map[yy][147] = TOKEN_40G;
					map[yy][157] = TOKEN_40G;
					break;
				case 42:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*2)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((40*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 25:
					map[yy][0] = TOKEN_25G;
					map[yy][16] = TOKEN_25G;
					map[yy][33] = TOKEN_25G;
					map[yy][50] = TOKEN_25G;
					map[yy][67] = TOKEN_25G;
					map[yy][84] = TOKEN_25G;
					map[yy][100] = TOKEN_25G;
					map[yy][117] = TOKEN_25G;
					map[yy][134] = TOKEN_25G;
					map[yy][151] = TOKEN_25G;
					break;
				case 27:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*4)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if (n==((25*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 20:
					map[yy][0] = TOKEN_20G;
					map[yy][21] = TOKEN_20G;
					map[yy][42] = TOKEN_20G;
					map[yy][63] = TOKEN_20G;
					map[yy][84] = TOKEN_20G;
					map[yy][105] = TOKEN_20G;
					map[yy][126] = TOKEN_20G;
					map[yy][147] = TOKEN_20G;
					break;					
				case 21:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*4)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((20*10)/BW_QUANTA)) {
							break;
						}
					}
					break;	
				case 10:
					map[yy][0] = TOKEN_10G;
					map[yy][42] = TOKEN_10G;
					map[yy][84] = TOKEN_10G;
					map[yy][126] = TOKEN_10G;
					break;						
				case 11:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*10)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((10*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				default:
					return 0;
					break;
			}
			break;
		case 645: /* TH */
			switch (port_speed)
			{
				case 100:
					for (i=0; i<len; i+=VECTOR_QUANTA_S) {
						map[yy][i] = TOKEN_100G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;	
				case 50:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*2)) {
						map[yy][i] = TOKEN_50G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 42:
					map[yy][0] = TOKEN_40G;
					map[yy][9] = TOKEN_40G;
					map[yy][19] = TOKEN_40G;
					map[yy][28] = TOKEN_40G;
					map[yy][38] = TOKEN_40G;
					map[yy][47] = TOKEN_40G;
					map[yy][57] = TOKEN_40G;
					map[yy][66] = TOKEN_40G;
					map[yy][76] = TOKEN_40G;
					map[yy][85] = TOKEN_40G;
					map[yy][95] = TOKEN_40G;
					map[yy][104] = TOKEN_40G;
					map[yy][114] = TOKEN_40G;
					map[yy][124] = TOKEN_40G;
					map[yy][134] = TOKEN_40G;
					map[yy][144] = TOKEN_40G;
					break;					
				case 40:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*2)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 25:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*4)) {
						map[yy][i] = TOKEN_25G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 21:
					map[yy][0] = TOKEN_20G;
					map[yy][19] = TOKEN_20G;
					map[yy][38] = TOKEN_20G;
					map[yy][57] = TOKEN_20G;
					map[yy][76] = TOKEN_20G;
					map[yy][95] = TOKEN_20G;
					map[yy][114] = TOKEN_20G;
					map[yy][134] = TOKEN_20G;
					break;						
				case 20:
					for (i=0; i<len; i+=(VECTOR_QUANTA_F*4)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 11:
					map[yy][0] = TOKEN_10G;
					map[yy][38] = TOKEN_10G;
					map[yy][76] = TOKEN_10G;
					map[yy][114] = TOKEN_10G;
					break;					
				case 10:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*10)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				default:
					return 0;
					break;					
			}
			break;
		case 545: /* TH */
			switch (port_speed)
			{
				case 10:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*6)+3)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;			
				case 11:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*6)+2)) {
						map[yy][i] = TOKEN_10G;
						n++;
						if (n==((10*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 20:
					map[yy][0] = TOKEN_20G;
					map[yy][16] = TOKEN_20G;
					map[yy][33] = TOKEN_20G;
					map[yy][49] = TOKEN_20G;
					map[yy][66] = TOKEN_20G;
					map[yy][82] = TOKEN_20G;
					map[yy][99] = TOKEN_20G;
					map[yy][116] = TOKEN_20G;
					break;						
				case 21:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F*3)+1)) {
						map[yy][i] = TOKEN_20G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
					break;
				case 40:
					map[yy][0] = TOKEN_40G;
					map[yy][8] = TOKEN_40G;
					map[yy][16] = TOKEN_40G;
					map[yy][24] = TOKEN_40G;
					map[yy][33] = TOKEN_40G;
					map[yy][41] = TOKEN_40G;
					map[yy][49] = TOKEN_40G;
					map[yy][57] = TOKEN_40G;
					map[yy][66] = TOKEN_40G;
					map[yy][74] = TOKEN_40G;
					map[yy][82] = TOKEN_40G;
					map[yy][90] = TOKEN_40G;
					map[yy][99] = TOKEN_40G;
					map[yy][108] = TOKEN_40G;
					map[yy][116] = TOKEN_40G;
					map[yy][125] = TOKEN_40G;
					break;						
				case 42:
					for (i=0; i<len; i+=((VECTOR_QUANTA_F)+3)) {
						map[yy][i] = TOKEN_40G;
						n++;
						if (n==((port_speed*10)/BW_QUANTA)) {
							break;
						}
					}
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
#endif
/* -I  $(VCS_HOME)/include  */


/**
@name: tdm_ver
@param: int[]

Populates manually annotated version number of C code
**/
void tdm_ver(int ver[2]) {
	/* P4 */
	ver[0] = 131;
	/* CVS */
	ver[1] = 0;
}


/**
@name: tdm_abs
@param: int

Returns absolute value of an integer
**/
int tdm_abs(int num) {
	if (num < 0) {
		return (-num);
	}
	else {
		return num;
	}
}


/**
@name: tdm_fac
@param: int

Calculates factorial of an integer
**/
int tdm_fac(int num_signed) {
	int i, product, num;
	num = tdm_abs(num_signed);
	product=num;
	
	if (num==0) {
		return 1;
	}
	else {
		for (i=(num-1); i>0; i--) {
			product *= (num-i);
		}	
		return product;
	}
}


/**
@name: tdm_pow
@param: int

Calculates unsigned power of an integer
**/
int tdm_pow(int num, int pow) {
	int i, product=num;
	
	if (pow==0) {
		return 1;
	}
	else {
		for (i=1; i<pow; i++) {
			product *= num;
		}
		return product;
	}
}


/**
@name: tdm_sqrt
@param: int

Calculates approximate square root of an integer using Taylor series
*** CURRENTLY NOT USING ANY FLOATING POINT ***
**/
int tdm_sqrt(int input_signed) {
	int n, d=0, s=1, approx, input;
	input = tdm_abs(input_signed);
	
	/* We cannot afford a slow rate of convergence, so we will
	 * find the perfect Taylor series coefficients via Bakhshali
	 * Approximation before solving any Taylor terms.
	 */
	do {
		d=(input-tdm_pow(s,2));
		if (d<0) {
			--s;
			break;
		}
		if ( ((1000*tdm_abs(d))/tdm_pow(s,2)) <= ((1000*tdm_abs(d+1))/tdm_pow((s+1),2)) ) {
			break;
		}
		s++;
	} while(s<input);
	d=(input-tdm_pow(s,2));

	/* Sum the Taylor series */
	approx=s; /* Fill in first term */
	/* At near perfect-square values it may converge to ceiling(sqrt(input))
	 * as integer cast sqrt(input) would discard the remainder, so we need 
	 * to clamp to the integer floor at all times.
	 */
	if (d<(2*s)) {
		for (n=1; n<3; n++) {
			approx+=((tdm_pow(-1,n)*tdm_fac(2*n)*tdm_pow(d,n))/(tdm_pow(tdm_fac(n),2)*tdm_pow(4,n)*tdm_pow(s,((2*n)-1))*(1-2*n)));
		}
	}
	
	return approx;
}


/**
@name: block
@param: int[256]

For debugging. Prints TDM table and blocks on keypress before resuming execution
**/
void block(int tdm_tbl[256])
{
#ifdef _SET_TDM_DEV
	int i, j=1;
	
	for (i=255; i>0; i--) {
		if (tdm_tbl[i]!=_TH_NUM_EXT_PORTS) {
			j=(i+1);
			break;
		}
	}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("\n")));
	for (i=0; i<j; i++) {
		if (tdm_tbl[i]!=OVSB_TOKEN) {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("\t[%0d] =\t %03d\n"),
                         i,tdm_tbl[i]));
		}
		else {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("\t[%0d] =\t ---\n"),
                         i));
		}
	}
	while(getchar() != '\n');
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("\n")));
#endif
}


/**
@name: TDM_scheduler_exists
@param: int

Returns boolean on whether an element exists in an array
**/
int TDM_scheduler_exists(int e, int arr[107]) {
	int i, result=FAIL;
	
	for (i=0; i<107; i++) {
		if (arr[i]==e) {
			result=PASS;
			break;
		}
	}
	
	return result;
}


/**
@name: TDM_scheduler_PQ
@param: int

Customizable partial quotient ceiling function
**/
/* int TDM_scheduler_PQ(float f) { return (int)((f < 0.0f) ? f - 0.5f : f + 0.5f); }*/
int TDM_scheduler_PQ(int f) { 
	return ( (int)( ( f+5 )/10) );
}


/**
@name: TDM_scheduler_slots
@param: int, int

Calculates number of slots required for a specific port
**/
int TDM_scheduler_slots(int port_speed) {
	return ( (int)( (port_speed*10)/BW_QUANTA ) ); 
}


/**
@name: TDM_scheduler_2d
@param:

TDM scheduler function
**/
int TDM_scheduler_2d(FILE *file, int freq, enum port_speed speed[_TH_NUM_EXT_PORTS], int port_state_map[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_buffer[64], int os_buffer[64], int lr_idx_limit, int tdm_tbl[256], int bucket1[_TH_OS_GROUP_LEN], int bucket2[_TH_OS_GROUP_LEN], int bucket3[_TH_OS_GROUP_LEN], int bucket4[_TH_OS_GROUP_LEN], int bucket5[_TH_OS_GROUP_LEN], int bucket6[_TH_OS_GROUP_LEN], int bucket7[_TH_OS_GROUP_LEN], int bucket8[_TH_OS_GROUP_LEN], int pipe, unsigned char accessories, unsigned char higig_mgmt)
{
	/* Values */
	int num_40g, num_100g, timeout=TIMEOUT, inner_timeout=TIMEOUT, min_slots=1;
	
	/* Constants */
	unsigned char type_mod=0, type_mod_diff=BOOL_FALSE;
	
	/* Single use variables or loop iterators */
	int a, b, c=0, d, f, g, i=0, j, k, l, m, p, q, v, w, x;
	
	/* Vector TDM variables */
	unsigned short map_width, map_length;
	unsigned char **vector_map;
	unsigned char dim_reduce, dim_reduce_4;
	char HG4X106G_3X40G=BOOL_FALSE;
	int load_status=FAIL, os_param_spd=0, os_param_cnt, vec_slot_cnt=0, lr_vec_cnt=0, num_lr=0, num_os=0, ovs_vec_cnt=0, lr_slot_cnt=0;
	int dr_weights[VEC_MAP_WID], dr_weights_last[VEC_MAP_WID], dr_weight_cmp[VEC_MAP_WID], slice_blacklist[107];
	int slice_row_up=0, slice_row_up_cnt=0, slice_row_dn=0, slice_row_dn_cnt=0, slice_col_up, slice_col_dn, slice_result_up=0, slice_result_dn=0, slice_blacklist_idx=0, os_burst_max_last_idx=0, mpass_backup_node, mpass_weight_min, mpass_weight_vnum, mpass_optimal_node=0, node_transmuted, os_burst_max, os_burst_max_idx, os_burst_max_threshold, lr_burst_max, lr_burst_max_idx, lr_burst_max_threshold, lr_burst_max_last_idx=0, last_rotation_idx;
	
	/* Table post processing variables */
	int tsc_swap[4];
	int cap=0, cap_tsc=_TH_NUM_EXT_PORTS, cap_lpbk=0, cap_lkbk=0;
	
	/* Flags to denote which linerate speeds are active */
	char lr_100=BOOL_FALSE, lr_10=BOOL_FALSE, lr_20=BOOL_FALSE, lr_25=BOOL_FALSE, lr_40=BOOL_FALSE, lr_50=BOOL_FALSE, lr_enable=BOOL_FALSE;
	/* Line rate port lists */
	unsigned char lr10[64], lr20[64], lr25[64], lr40[64], lr50[64], lr100[64];
	/* Flags to denote which oversub speeds are active */
	char os_10=BOOL_FALSE, os_20=BOOL_FALSE, os_25=BOOL_FALSE, os_40=BOOL_FALSE, os_50=BOOL_FALSE, os_100=BOOL_FALSE, os_enable=BOOL_FALSE;
	/* Oversub port lists */
	unsigned char os10[64], os20[64], os25[64], os40[64], os50[64], os100[64];
	
	/* Create local stack pointers for lr - 7 local pointers, 1 schedule pointer */
	unsigned short y1=0, y2=0, y3=0, y4=0, y5=0, y6=0, yy=0;
	
	/* Create local stack pointers for os - 5 local pointers, 4 bucket pointers */
	unsigned char z6=0, z1=0, z2=0, z3=0, z4=0, z5=0, z11=0, z22=0, z33=0, z44=0, z55=0, z66=0, z77=0, z88=0;
	
	/* Sister port proxcheck variables */
	unsigned char prox_swap=_TH_NUM_EXT_PORTS;
	
	/* Triport */
	unsigned char tri_chk, tri_en_50=BOOL_FALSE, tri_en_40=BOOL_FALSE, tri_en_20=BOOL_FALSE;

	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("TDM: _____VERBOSE: index limit set to %d\n"),
                 lr_idx_limit));

	for (j=0; j<64; j++)
	{
		lr100[j] = _TH_NUM_EXT_PORTS; lr50[j] = _TH_NUM_EXT_PORTS; lr40[j] = _TH_NUM_EXT_PORTS;
		lr25[j] = _TH_NUM_EXT_PORTS; lr20[j] = _TH_NUM_EXT_PORTS; lr10[j] = _TH_NUM_EXT_PORTS;
		os10[j] = _TH_NUM_EXT_PORTS; os20[j] = _TH_NUM_EXT_PORTS; os25[j] = _TH_NUM_EXT_PORTS;
		os40[j] = _TH_NUM_EXT_PORTS; os50[j] = _TH_NUM_EXT_PORTS; os100[j] = _TH_NUM_EXT_PORTS;
	}
	for (j=0; j<107; j++) {
		slice_blacklist[j]=0;
	}

	/* Generate the TDM vector map */
	map_length=VEC_MAP_LEN;
	map_width=VEC_MAP_WID;
	vector_map=(unsigned char **) TDM_ALLOC(map_width*sizeof(unsigned char *), "vector_map_l1");
	for (j=0; j<map_width; j++) {
		vector_map[j]=(unsigned char *) TDM_ALLOC(map_length*sizeof(unsigned char), "vector_map_l2");
	}
	for (v=0; v<map_length; v++) {
		for (j=0; j<map_width; j++) {
			vector_map[j][v]=_TH_NUM_EXT_PORTS;
		}
	}
	
	/* Preallocation */
	for (j=0; j<64; j++) {
		if (lr_buffer[j] != _TH_NUM_EXT_PORTS && port_state_map[lr_buffer[j]-1] == 1) {
			switch (speed[lr_buffer[j]]) {
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
					lr_10 = BOOL_TRUE;
					y1++;
					lr10[y1] = lr_buffer[j];
					for (i=0; i<y1; i++) if (lr10[i] == lr_buffer[j]) y1--;
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					lr_20 = BOOL_TRUE;
					y2++;
					lr20[y2] = lr_buffer[j];
					for (i=0; i<y2; i++) if (lr20[i] == lr_buffer[j]) y2--;
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					lr_40 = BOOL_TRUE;
					y3++;
					lr40[y3] = lr_buffer[j];
					for (i=0; i<y3; i++) if (lr40[i] == lr_buffer[j]) y3--;
					break;
				case SPEED_106G:
					speed[lr_buffer[j]]=SPEED_100G;
				case SPEED_100G:	
					lr_100 = BOOL_TRUE;
					y4++;
					lr100[y4] = lr_buffer[j];
					for (i=0; i<y4; i++) if (lr100[i] == lr_buffer[j]) y4--;
					break;
				case SPEED_53G:
					speed[lr_buffer[j]]=SPEED_50G;
				case SPEED_50G:
					lr_50 = BOOL_TRUE;
					y5++;
					lr50[y5] = lr_buffer[j];
					for (i=0; i<y5; i++) if (lr50[i] == lr_buffer[j]) y5--;
					break;
				case SPEED_27G:
					speed[lr_buffer[j]]=SPEED_25G;
				case SPEED_25G:
					lr_25 = BOOL_TRUE;
					y6++;
					lr25[y6] = lr_buffer[j];
					for (i=0; i<y6; i++) if (lr25[i] == lr_buffer[j]) y6--;
					break;				
				default:
                    LOG_ERROR(BSL_LS_SOC_TDM,
                              (BSL_META("speed %0d port %0d was skipped in linerate preallocation\n"),
                               speed[lr_buffer[j]],lr_buffer[j]));
			}
		}
		else if (lr_buffer[j] != _TH_NUM_EXT_PORTS && lr_buffer[j] != OVSB_TOKEN) {
			LOG_ERROR(BSL_LS_SOC_TDM,
                      (BSL_META("failed to presort linerate port %0d with state mapping %0d\n"),
                       lr_buffer[j], port_state_map[lr_buffer[j]-1]));
		}
		if (os_buffer[j] != _TH_NUM_EXT_PORTS && port_state_map[os_buffer[j]-1] == 2) {
			switch (speed[os_buffer[j]]) {
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
					os_10 = BOOL_TRUE;
					z1++;
					os10[z1] = os_buffer[j];
					for (i=0; i<z1; i++) if (os10[i] == os_buffer[j]) z1--;
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					os_20 = BOOL_TRUE;
					z2++;
					os20[z2] = os_buffer[j];
					for (i=0; i<z2; i++) if (os20[i] == os_buffer[j]) z2--;
					break;
				case SPEED_40G:
				case SPEED_42G:
					os_40 = BOOL_TRUE;
					z3++;
					os40[z3] = os_buffer[j];
					for (i=0; i<z3; i++) if (os40[i] == os_buffer[j]) z3--;				
					break;
				case SPEED_106G:
					speed[os_buffer[j]]=SPEED_100G;
				case SPEED_100G:
					os_100 = BOOL_TRUE;
					z4++;
					os100[z4] = os_buffer[j];
					for (i=0; i<z4; i++) if (os100[i] == os_buffer[j]) z4--;
					break;
				case SPEED_53G:
					speed[os_buffer[j]]=SPEED_50G;
				case SPEED_50G:
					os_50 = BOOL_TRUE;
					z5++;
					os50[z5] = os_buffer[j];
					for (i=0; i<z5; i++) if (os50[i] == os_buffer[j]) z5--;
					break;
				case SPEED_27G:
					speed[os_buffer[j]]=SPEED_25G;
				case SPEED_25G:
					os_25 = BOOL_TRUE;
					z6++;
					os25[z6] = os_buffer[j];
					for (i=0; i<z6; i++) if (os25[i] == os_buffer[j]) z6--;
					break;				
				default:
					LOG_ERROR(BSL_LS_SOC_TDM,
                              (BSL_META("speed %0d port %0d was skipped in oversub preallocation\n"),
                               speed[os_buffer[j]],os_buffer[j]));
			}
		}
		else if (os_buffer[j] != _TH_NUM_EXT_PORTS) {
            LOG_ERROR(BSL_LS_SOC_TDM,
                      (BSL_META("failed to presort oversub port %0d with state mapping %0d\n"),
                       os_buffer[j], port_state_map[os_buffer[j]-1]));
		}
	}
	os_enable = (os_10||os_20||os_25||os_40||os_50||os_100);
	lr_enable = (lr_10||lr_20||lr_25||lr_40||lr_50||lr_100);
	num_lr=(y1+y2+y3+y4+y5+y6);
	num_os=(z1+z2+z3+z4+z5+z6);
	num_100g=y4;
	num_40g=y3;
	/* 4x106HG+3x40G is a specific exception to all other scheduling rules */
	if (num_40g==3 && num_100g==4 && freq==850) {
		accessories=9;
		lr_idx_limit=201;
		HG4X106G_3X40G=BOOL_TRUE;
	}	
	
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("(10G - %0d) (20G - %0d) (25G - %0d) (40G - %0d) (50G - %0d) (100G - %0d) LR Variety - %0d\n"),
                 y1, y2, y6, y3, y5, y4,
                 (lr_10+lr_20+lr_25+lr_40+lr_50+lr_100)));
	TDM_BIG_BAR

	/* Postallocation */
	/*
		100G
	*/
	if (y4 > 0)
	{
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: 100G scheduling pass\n")));
		TDM_SML_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: Stack contains: [ ")));
		for (v=y4; v>0; v--) {
			if (lr100[v]!=_TH_NUM_EXT_PORTS) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d "),
                             lr100[v]));
			}
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META(" ]\n")));
		TDM_SML_BAR
		while (y4 > 0)
		{
			/* Must account for 4x106HG+3x40G special case */
			if (HG4X106G_3X40G==BOOL_TRUE) {
				load_status = TDM_scheduler_vector_load(vector_map, freq, 107, yy, lr_idx_limit);
			}
			else {
				if (check_ethernet(lr100[y4],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
					load_status = TDM_scheduler_vector_load(vector_map, freq, 100, yy, lr_idx_limit);
				}
				else {
					load_status = TDM_scheduler_vector_load(vector_map, freq, 106, yy, lr_idx_limit);
				}
			}
			if (load_status==FAIL)
			{
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Failed to load 100G/106G vector for port %0d\n"), lr100[y4]));
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_100G || vector_map[yy][v]==TOKEN_106G) {
						vector_map[yy][v]=lr100[y4];
					}
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("loaded 100G port %d vector from stack pointer %d into map at index %0d\n"),
                             lr100[y4], y4, yy));
			}
			y4--; yy++;
			if (y4>0) {
				TDM_SML_BAR
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: map pointer rolled to: %0d, stack pointer is at: %0d\n"),
                             yy, y4));
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: DONE loading 100G vectors, map pointer floating at: %0d\n"),
                             yy));
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	
	/*
		50G
	*/
	if (y5 > 0)
	{
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: 50G scheduling pass\n")));
		/* scatter sister ports before postallocation to reduce entropy of proxcheck later on */
		for (v=y5; v>0; v--) {
			if (which_tsc(lr50[v+1],tsc)==which_tsc(lr50[v],tsc) ||
				which_tsc(lr50[v-1],tsc)==which_tsc(lr50[v],tsc)) {
				prox_swap=lr50[v];
				for (w=v; w>0; w--) {
					lr50[w]=lr50[w-1];
				}
				lr50[1]=prox_swap;
			}
		}
		TDM_SML_BAR
            LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Stack contains: [ ")));
		for (v=y5; v>0; v--) {
			if (lr50[v]!=_TH_NUM_EXT_PORTS) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d "), lr50[v]));
			}
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META(" ]\n")));
		TDM_SML_BAR
		
		for (v=y5; v>0; v--) {
			tri_chk = which_tsc(lr50[v],tsc);
			for (w=0; w<4; w++) {
				if (speed[tsc[tri_chk][w]]!=SPEED_50G) {
					tri_en_50=BOOL_TRUE;
					break;
				}
			}
			if (tri_en_50==BOOL_TRUE) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: 50G triport detected\n")));
				break;
			}
		}
		while (y5 > 0)
		{
			if (check_ethernet(lr50[y5],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
				load_status = TDM_scheduler_vector_load(vector_map, freq, 50, yy, lr_idx_limit);
			}
			else {
				load_status = TDM_scheduler_vector_load(vector_map, freq, 53, yy, lr_idx_limit);
			}
			if (load_status==FAIL)
			{
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Failed to load 50G vector for port %0d\n"), lr50[y5]));
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_50G) {
						vector_map[yy][v]=lr50[y5];
					}
				}			
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("loaded 50G port %d vector from stack pointer %d into map at index %0d\n"),
                             lr50[y5], y5, yy));
			}			
			y5--; yy++;
			if (y5>0) {
				TDM_SML_BAR
                    LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: map pointer rolled to: %0d, stack pointer is at: %0d\n"),
                                 yy, y5));
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
                    LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: DONE scheduling 50G, map pointer floating at: %0d\n"),
                                 yy));
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	/*
		40G
	*/
	if (y3 > 0)
	{
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: 40G scheduling pass\n")));
		/* scatter sister ports before postallocation to reduce entropy of proxcheck later on */
		for (v=y3; v>0; v--) {
			if (which_tsc(lr40[v+1],tsc)==which_tsc(lr40[v],tsc) ||
				which_tsc(lr40[v-1],tsc)==which_tsc(lr40[v],tsc)) {
				prox_swap=lr40[v];
				for (w=v; w>0; w--) {
					lr40[w]=lr40[w-1];
				}
				lr40[1]=prox_swap;
			}
		}
		TDM_SML_BAR
            LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Stack contains: [ ")));
		for (v=y3; v>0; v--) {
			if (lr40[v]!=_TH_NUM_EXT_PORTS) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d "), lr40[v]));
			}
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META(" ]\n")));
		TDM_SML_BAR
		for (v=y3; v>0; v--) {
			tri_chk = which_tsc(lr40[v],tsc);
			for (w=0; w<4; w++) {
				if (speed[tsc[tri_chk][w]]!=SPEED_40G && port_state_map[tsc[tri_chk][w]-1]!=0) {
					tri_en_40=BOOL_TRUE;
					break;
				}
			}
			if (tri_en_40==BOOL_TRUE) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: 40G triport detected\n")));
				break;
			}
		}
		while (y3 > 0)
		{
			/* Must account for 4x106HG+3x40G special case */
			if (HG4X106G_3X40G==BOOL_TRUE) {
				if (y3==3) {
					load_status = TDM_scheduler_vector_load(vector_map, freq, 43, yy, lr_idx_limit);
				}
				else if (y3==2) {
					load_status = TDM_scheduler_vector_load(vector_map, freq, 44, yy, lr_idx_limit);
				}
				else if (y3==1) {
					load_status = TDM_scheduler_vector_load(vector_map, freq, 45, yy, lr_idx_limit);
				}				
			}
			else {
				if (check_ethernet(lr40[y3],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
					load_status = TDM_scheduler_vector_load(vector_map, freq, 40, yy, lr_idx_limit);
				}
				else {
					load_status = TDM_scheduler_vector_load(vector_map, freq, 42, yy, lr_idx_limit);
				}
			}
			if (load_status==FAIL)
			{
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Failed to load 40G vector for port %0d\n"), lr40[y3]));
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_40G) {
						vector_map[yy][v]=lr40[y3];
					}
				}			
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("loaded 40G port %d vector from stack pointer %d into map at index %0d\n"),
                             lr40[y3], y3, yy));
			}			
			y3--; yy++;
			if (y3>0) {
				TDM_SML_BAR
                    LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: map pointer rolled to: %0d, stack pointer is at: %0d\n"),
                                 yy, y3));
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
                    LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: DONE scheduling 40G, map pointer floating at: %0d\n"),
                                 yy));
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	/*
		25G
	*/
	if (y6 > 0)
	{
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: 25G scheduling pass\n")));
		TDM_SML_BAR
            LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Stack contains: [ ")));
		for (v=y6; v>0; v--) {
			if (lr25[v]!=_TH_NUM_EXT_PORTS) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d "),
                             lr25[v]));
			}
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META(" ]\n")));
		TDM_SML_BAR
		while (y6 > 0)
		{
			if (check_ethernet(lr25[y6],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
				load_status = TDM_scheduler_vector_load(vector_map, freq, 25, yy, lr_idx_limit);
			}
			else {
				load_status = TDM_scheduler_vector_load(vector_map, freq, 27, yy, lr_idx_limit);
			}
			if (load_status==FAIL)
			{
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Failed to load 25G vector for port %0d\n"), lr25[y6]));
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_25G) {
						vector_map[yy][v]=lr25[y6];
					}
				}			
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("loaded 25G port %d vector from stack pointer %d into map at index %0d\n"),
                             lr25[y6], y6, yy));
			}			
			y6--; yy++;
			if (y6>0) {
				TDM_SML_BAR
                    LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: map pointer rolled to: %0d, stack pointer is at: %0d\n"),
                                 yy, y6));
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: DONE scheduling 25G, map pointer floating at: %0d\n"),
                             yy));
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	/*
		20G
	*/
	if (y2 > 0)
	{
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: 20G scheduling pass\n")));
		TDM_SML_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: Stack contains: [ ")));
		for (v=y2; v>0; v--) {
			if (lr20[v]!=_TH_NUM_EXT_PORTS) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d "),
                             lr20[v]));
			}
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META(" ]\n")));
		TDM_SML_BAR
		for (v=y2; v>0; v--) {
			tri_chk = which_tsc(lr20[v],tsc);
			for (w=0; w<4; w++) {
				if (speed[tsc[tri_chk][w]]!=SPEED_20G) {
					tri_en_20=BOOL_TRUE;
					break;
				}
			}
			if (tri_en_20==BOOL_TRUE) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: 20G triport detected\n")));
				break;
			}
		}
		while (y2 > 0)
		{
			if (check_ethernet(lr20[y2],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
				load_status = TDM_scheduler_vector_load(vector_map, freq, 20, yy, lr_idx_limit);
			}
			else {
				load_status = TDM_scheduler_vector_load(vector_map, freq, 21, yy, lr_idx_limit);
			}
			if (load_status==FAIL)
			{
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Failed to load 20G vector for port %0d\n"), lr20[y2]));
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_20G) {
						vector_map[yy][v]=lr20[y2];
					}
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("loaded 20G port %d vector from stack pointer %d into map at index %0d\n"),
                             lr20[y2], y2, yy));
			}			
			y2--; yy++;
			if (y2>0) {
				TDM_SML_BAR
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: map pointer rolled to: %0d, stack pointer is at: %0d\n"),
                             yy, y2));
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: DONE scheduling 20G, map pointer floating at: %0d\n"),
                             yy));
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	/*
		10G
	*/
	if (y1 > 0)
	{
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: 10G scheduling pass\n")));
		TDM_SML_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: Stack contains: [ ")));
		for (v=y1; v>0; v--) {
			if (lr10[v]!=_TH_NUM_EXT_PORTS) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d "),
                             lr10[v]));
			}
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META(" ]\n")));
		TDM_SML_BAR
		while (y1 > 0)
		{
			if (check_ethernet(lr10[y1],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
				load_status = TDM_scheduler_vector_load(vector_map, freq, 10, yy, lr_idx_limit);
			}
			else {
				load_status = TDM_scheduler_vector_load(vector_map, freq, 11, yy, lr_idx_limit);
			}
			if (load_status==FAIL)
			{
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Failed to load 10G vector for port %0d\n"), lr10[y1]));
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_10G) {
						vector_map[yy][v]=lr10[y1];
					}
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("loaded 10G port %d vector from stack pointer %d into map at index %0d\n"),
                             lr10[y1], y1, yy));
			}			
			y1--; yy++;
			if (y1>0) {
				TDM_SML_BAR
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: map pointer rolled to: %0d, stack pointer is at: %0d\n"),
                             yy, y1));
				TDM_SML_BAR			
			}
			else {
				TDM_SML_BAR
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: DONE scheduling 10G, map pointer floating at: %0d\n"),
                             yy));
				TDM_SML_BAR			
			}
		}
		TDM_BIG_BAR
	}
	
	/* Parameterize oversub vectors and insert them into map */
	if (os_enable && lr_enable) {
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Oversub scheduling pass\n")));
		TDM_SML_BAR
		lr_vec_cnt=0;
		os_param_cnt=lr_idx_limit;
		for (i=0; i<VEC_MAP_WID; i++) {
			if (vector_map[i][0]!=_TH_NUM_EXT_PORTS) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("deducting vector PORT:%d SPEED:%d from pipe bandwidth\n"),
                             vector_map[i][0],
                             ((speed[vector_map[i][0]])/1000)));
				os_param_cnt-=TDM_scheduler_slots(((speed[vector_map[i][0]])/1000));
				lr_vec_cnt++;
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("oversub parameter:%d, LR vector count:%d\n"),
                             os_param_cnt,lr_vec_cnt));
				TDM_SML_BAR
			}
		}
		for (i=(VEC_MAP_WID-1); i>=0; i--) {
			if (vector_map[i][0]!=_TH_NUM_EXT_PORTS) {
				k=1;
				for (v=1; v<lr_idx_limit; v++) {
					if (vector_map[i][v]!=_TH_NUM_EXT_PORTS) {
						k++;
					}
				}
				switch(k) {
					case 4: if (check_ethernet(vector_map[i][0], speed, tsc, traffic)) {os_param_spd = 10;} else {os_param_spd = 11;} break;
					case 8: if (check_ethernet(vector_map[i][0], speed, tsc, traffic)) {os_param_spd = 20;} else {os_param_spd = 21;} break;
					case 10: if (check_ethernet(vector_map[i][0], speed, tsc, traffic)) {os_param_spd = 25;} else {os_param_spd = 27;} break;
					case 16: if (check_ethernet(vector_map[i][0], speed, tsc, traffic)) {os_param_spd = 40;} else {os_param_spd = 42;} break;
					case 20: if (check_ethernet(vector_map[i][0], speed, tsc, traffic)) {os_param_spd = 50;} else {os_param_spd = 53;} break;
					case 39: os_param_spd = 107; break;
					case 40: if (check_ethernet(vector_map[i][0], speed, tsc, traffic)) {os_param_spd = 100;} else {os_param_spd = 106;} break;
				}
				break;
			}
		}
		if (((os_param_cnt*BW_QUANTA)/10)>0) {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("virtualizing from %d param %dG oversub bandwidth\n"),
                         os_param_cnt,((os_param_cnt*BW_QUANTA)/10)));
		}
		else {
			LOG_ERROR(BSL_LS_SOC_TDM,
                      (BSL_META("Could not virtualize %dG oversub bandwidth\n"), ((os_param_cnt*BW_QUANTA)/10)));
			return 0;
		}
		TDM_SML_BAR
		vec_slot_cnt=0;
		for (j=0; j<lr_idx_limit; j++) {
			if (vector_map[0][j]==_TH_NUM_EXT_PORTS) {
				vec_slot_cnt++;
			}
			if (j!=0 && vector_map[0][j]!=_TH_NUM_EXT_PORTS && vector_map[0][(j-vec_slot_cnt-1)]!=_TH_NUM_EXT_PORTS) {
				break;
			}
		}
		ovs_vec_cnt = ( (vec_slot_cnt-lr_vec_cnt)>(VEC_MAP_WID-2-num_lr) )?((2)):((vec_slot_cnt-lr_vec_cnt));
			
			/* Clamp number of parameterized oversub vectors added to reduce complexity */
			ovs_vec_cnt=0;
			/* -------------------------------------- */
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("partitioning %d %dG oversub vectors\n"),
                     ovs_vec_cnt, os_param_spd));
		TDM_SML_BAR
		/* --------------- */
		if (os_param_spd!=40) {
			for (j=0; j<ovs_vec_cnt; j++) {
				load_status = TDM_scheduler_vector_load(vector_map, freq, os_param_spd, yy, lr_idx_limit);
				if (load_status==FAIL) {
					LOG_ERROR(BSL_LS_SOC_TDM,
                              (BSL_META("Failed to load oversub virtual vector at"
                                        "speed %0d at index %d\n"), 
                                        os_param_spd, yy));
					return 0;
				}
				else {
					for (i=0; i<lr_idx_limit; i++) {
						if (vector_map[yy][i]!=_TH_NUM_EXT_PORTS) {
							vector_map[yy][i]=OVSB_TOKEN;
						}
					}
					yy++;
				}
			}
		}
		else {
			for (j=0; j<(ovs_vec_cnt-1); j++) {
				load_status = TDM_scheduler_vector_load(vector_map, freq, os_param_spd, yy, lr_idx_limit);
				if (load_status==FAIL) {
					LOG_ERROR(BSL_LS_SOC_TDM,
                              (BSL_META("Failed to load oversub virtual" 
                                        "vector at speed %0d at index %d\n"),
                                        os_param_spd, yy));
					return 0;
				}
				else {
					for (i=0; i<lr_idx_limit; i++) {
						if (vector_map[yy][i]!=_TH_NUM_EXT_PORTS) {
							vector_map[yy][i]=OVSB_TOKEN;
						}
					}
					yy++;
				}
			}		
		}
	}
	else {lr_vec_cnt=0; for (i=0; i<VEC_MAP_WID; i++) {if (vector_map[i][0]!=_TH_NUM_EXT_PORTS) {lr_vec_cnt++;}}}
	for (i=0; i<lr_vec_cnt; i++) {
		lr_slot_cnt+=TDM_scheduler_slots(speed[vector_map[i][0]]/1000);
	}
	if ( ((lr_10+lr_20+lr_40+lr_50+lr_100+lr_25)==1) ) {
		for (i=0; i<lr_vec_cnt; i++) {
			if (check_type_2(vector_map[i][0],tsc)!=type_mod) {
				if (i>0) {type_mod_diff=BOOL_TRUE;}
				if (check_type_2(vector_map[i][0],tsc)>type_mod) {type_mod=check_type_2(vector_map[i][0],tsc);}
			}
			/* type_mod=( check_type(vector_map[i][0],tsc)>type_mod )?(check_type(vector_map[i][0],tsc)):(type_mod); */
		}
	}
	if (lr_enable) {
		/* Only 1 type of speed in the TDM, bypass heuristics and just divide evenly */
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: taking line rate shortcut, type_mod is %0d\n"),
                     type_mod));
		for (i=1; i<60; i++) { TOKEN_CHECK(vector_map[0][i]) {break;} else {min_slots++;} }
		if ( ((lr_10+lr_20+lr_40+lr_50+lr_100+lr_25)==1) && (((2*num_lr)<=min_slots)||(type_mod==1)) && (type_mod_diff==BOOL_FALSE) ) {
			if ( ((2*num_lr)<=min_slots) ) {
				for (i=1; i<lr_vec_cnt; i++) {
					TDM_scheduler_vector_rotate(vector_map[i], lr_idx_limit, ((lr_idx_limit/lr_slot_cnt)*i)); j=0;
					while ( which_tsc(vector_map[i][((lr_idx_limit/lr_slot_cnt)*i)],tsc)==which_tsc(vector_map[(i-1)][((lr_idx_limit/lr_slot_cnt)*(i-1))],tsc) ) {
						if ((i+(++j))<lr_vec_cnt) {TDM_scheduler_map_transpose(vector_map, lr_idx_limit, i, (i+j));}
						else {break;}
					}
				}
				j=0;
				while ( (min_slots-TDM_scheduler_map_find_y(vector_map,lr_idx_limit,(lr_vec_cnt-1),0))>(lr_idx_limit/lr_slot_cnt) ) {
					for (i=(++j); i<lr_vec_cnt; i++) {
						TDM_scheduler_vector_rotate(vector_map[i], lr_idx_limit, 1);
					}
				}
				TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
			}
			else {
				for (i=1; i<lr_vec_cnt; i++) {
					TDM_scheduler_vector_rotate(vector_map[i], lr_idx_limit, i);
				}
				TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
			}
		}
		else {
			/* Reduce vector map to singular time vector, presort min spacing for second pass, clamp to O(2n) */
			timeout=lr_vec_cnt; last_rotation_idx=0;
			do {
				dim_reduce=PASS;
				for (i=1; i<lr_vec_cnt; i++) {
					for (k=0; k<lr_idx_limit; k++) {
						TDM_scheduler_vector_rotate(vector_map[i], lr_idx_limit, 1);
						TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
						#ifdef _SET_TDM_DEV
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("\n\t>>> PASS 0 >>> Column %0d >>> Subpass %0d >>> Interrupt @ %0d"),
                                     i,k,timeout));
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("\n")));
						#endif
						if (os_enable) {
							/* last_rotation_idx is a clamp that ensures full utilization of vector space */
							if ( (TDM_scheduler_fit_singular_col(vector_map,i,lr_idx_limit)==PASS) && (TDM_scheduler_fit_prox(vector_map,lr_idx_limit,i,i,tsc)==PASS) && (k>last_rotation_idx||i==1) ) {last_rotation_idx=k; break;}
						}
						else {
							if ( (TDM_scheduler_fit_singular_col(vector_map,i,lr_idx_limit)==PASS) && (TDM_scheduler_fit_prox(vector_map,lr_idx_limit,i,i,tsc)==PASS) ) {break;}
						}
					}
				}
				for (i=1; i<lr_vec_cnt; i++) {
					if (TDM_scheduler_fit_prox(vector_map,lr_idx_limit,lr_vec_cnt,i,tsc)==FAIL) {
						dim_reduce=FAIL;
						break;
					}
				}
			} while (dim_reduce==FAIL && ((--timeout)>0));
			if (!os_enable) {
				for (i=(lr_vec_cnt-1); i>0; i--) {
					if ( (TDM_scheduler_fit_singular_col(vector_map,i,lr_idx_limit)==FAIL) || (TDM_scheduler_fit_prox(vector_map,lr_idx_limit,lr_vec_cnt,i,tsc)==FAIL) ) {
						for (j=1; j<=i; j++) {
							dr_weights[j]=TDM_scheduler_map_shortest_distance(vector_map,lr_idx_limit,i,(i-j));
						}
						mpass_weight_min = TDM_scheduler_weight_min(dr_weights,i);
						mpass_weight_vnum = TDM_scheduler_weight_vnum(dr_weights,i,2);				
						mpass_optimal_node = TDM_scheduler_map_find_y(vector_map,lr_idx_limit,i,0);
						for (j=0; j<lr_idx_limit; j++) {
							for (k=0; k<lr_idx_limit; k++) {
								TDM_scheduler_vector_rotate(vector_map[i], lr_idx_limit, 1);
								if (TDM_scheduler_fit_prox(vector_map,lr_idx_limit,lr_vec_cnt,i,tsc)==PASS) {break;}
							}
							TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
							TDM_scheduler_weight_update(vector_map,lr_idx_limit,dr_weights,dr_weights_last,dr_weight_cmp,i,j,lr_slot_cnt);
							if ( (TDM_scheduler_weight_min(dr_weights,i) > mpass_weight_min) ||
								 (TDM_scheduler_weight_vnum(dr_weights,i,2) < mpass_weight_vnum) ) {
								mpass_optimal_node = TDM_scheduler_map_find_y(vector_map,lr_idx_limit,i,0);
								mpass_weight_min = ( (TDM_scheduler_weight_min(dr_weights,i) > mpass_weight_min) ) ? (TDM_scheduler_weight_min(dr_weights,i)):(mpass_weight_min);
								mpass_weight_vnum = ( (TDM_scheduler_weight_vnum(dr_weights,i,2) < mpass_weight_vnum) ) ? (TDM_scheduler_weight_vnum(dr_weights,i,2)):(mpass_weight_vnum);						
							}
							#ifdef _SET_TDM_DEV
                            LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("\n\t>>> PASS 1 >>> Column %0d >>> Subpass %0d >>> Node %0d >>> Min %0d >>> VNUM %0d"),i,j,mpass_optimal_node,mpass_weight_min,mpass_weight_vnum));
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("\n")));
							#endif
						}
						inner_timeout=0;
						while (TDM_scheduler_map_find_y(vector_map,lr_idx_limit,i,0)!=mpass_optimal_node) {
							TDM_scheduler_vector_rotate(vector_map[i], lr_idx_limit, 1);
							TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
							++inner_timeout;
							#ifdef _SET_TDM_DEV
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("\n\t>>> PASS 2 >>> Column %0d >>> Subpass %0d"),
                                         i,inner_timeout));
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("\n")));
							#endif
						}
						for (j=0; j<lr_idx_limit; j++) {
							TOKEN_CHECK(vector_map[i][j]) {
								if (TDM_scheduler_fit_singular_cnt(vector_map,j)>1) {
									node_transmuted=TDM_scheduler_nodal_transmute_lr(vector_map,freq,speed,tsc,traffic,lr_idx_limit,j,i,lr_vec_cnt);
									if (node_transmuted==BLANK) {
										#ifdef _SET_TDM_DEV
										LOG_WARN(BSL_LS_SOC_TDM,
                                                 (BSL_META("\tTransmute operation at node [%0d,%0d] failed\n"),
                                                  i,j));
										#endif
									}
									TDM_scheduler_weight_update(vector_map,lr_idx_limit,dr_weights,dr_weights_last,dr_weight_cmp,i,j,lr_slot_cnt);
									#ifdef _SET_TDM_DEV
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("\n\t>>> PASS 3 >>> Column %0d >>> Transmute %0d >>> Shift %0d"),
                                                 i,j,node_transmuted));
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("\n")));
									#endif
								}
							}
						}
					}
				}
				for (i=(lr_vec_cnt-1); i>0; i--) {
					if ( (TDM_scheduler_fit_singular_col(vector_map,i,lr_idx_limit)==FAIL) || (TDM_scheduler_fit_prox(vector_map,lr_idx_limit,lr_vec_cnt,i,tsc)==FAIL) ) {
						for (j=0; j<lr_idx_limit; j++) {
							if (TDM_scheduler_fit_singular_cnt(vector_map,j)==0) {
								for (k=0; k<lr_idx_limit; k++) {
									TDM_scheduler_vector_rotate(vector_map[i],lr_idx_limit,1);
									if (TDM_scheduler_map_find_y(vector_map,lr_idx_limit,i,0)==j) {
										break;
									}
								}
								break;
							}
						}
						for (j=0; j<lr_idx_limit; j++) {
							TOKEN_CHECK(vector_map[i][j]) {
								if (TDM_scheduler_fit_singular_cnt(vector_map,j)>1) {
									node_transmuted=TDM_scheduler_nodal_transmute_lr(vector_map,freq,speed,tsc,traffic,lr_idx_limit,j,i,lr_vec_cnt);
									if (node_transmuted==BLANK) {
										#ifdef _SET_TDM_DEV
										LOG_WARN(BSL_LS_SOC_TDM,
                                                 (BSL_META("\tTransmute operation at node [%0d,%0d] failed\n"),
                                                  i,j));
										#endif
									}
									TDM_scheduler_weight_update(vector_map,lr_idx_limit,dr_weights,dr_weights_last,dr_weight_cmp,i,j,lr_slot_cnt);
									#ifdef _SET_TDM_DEV
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("\n\t>>> PASS 3.5 >>> Column %0d >>> Transmute %0d >>> Shift %0d"),
                                                 i,j,node_transmuted));
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("\n")));
									#endif
								}
							}
						}
					}
				}
			}
			else {
				/* Analyze each vector and enhance singular vector */
				for (i=1; i<lr_vec_cnt; i++) {
					for (j=1; j<=i; j++) {
						dr_weights[j]=TDM_scheduler_map_shortest_distance(vector_map,lr_idx_limit,i,(i-j));
					}
					mpass_weight_min = TDM_scheduler_weight_min(dr_weights,i);		
					mpass_optimal_node = TDM_scheduler_map_find_y(vector_map,lr_idx_limit,i,0);
					mpass_backup_node = mpass_optimal_node;
					for (k=0; k<lr_idx_limit; k++) {
						TDM_scheduler_vector_rotate(vector_map[i], lr_idx_limit, 1);
						if (TDM_scheduler_fit_prox(vector_map,lr_idx_limit,lr_vec_cnt,i,tsc)==PASS) {mpass_backup_node = TDM_scheduler_map_find_y(vector_map,lr_idx_limit,i,0); break;}
					}
					TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
					TDM_scheduler_weight_update(vector_map,lr_idx_limit,dr_weights,dr_weights_last,dr_weight_cmp,i,j,lr_slot_cnt);
					if ( (TDM_scheduler_weight_min(dr_weights,i) > mpass_weight_min) ) {
						mpass_optimal_node = TDM_scheduler_map_find_y(vector_map,lr_idx_limit,i,0);
						mpass_weight_min = ( (TDM_scheduler_weight_min(dr_weights,i) > mpass_weight_min) ) ? (TDM_scheduler_weight_min(dr_weights,i)):(mpass_weight_min);					
					}
					#ifdef _SET_TDM_DEV
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n\t>>> PASS 4 >>> Column %0d >>> Subpass %0d >>> Node %0d >>> Min %0d"),
                                 i,j,mpass_optimal_node,mpass_weight_min));
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					#endif
					inner_timeout=0;
					while (TDM_scheduler_map_find_y(vector_map,lr_idx_limit,i,0)!=mpass_optimal_node) {
						TDM_scheduler_vector_rotate(vector_map[i], lr_idx_limit, 1);
						TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
						++inner_timeout;
						#ifdef _SET_TDM_DEV
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("\n\t>>> PASS 5 >>> Column %0d >>> Subpass %0d"),
                                     i,inner_timeout));
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("\n")));
						#endif
					}
					if (TDM_scheduler_fit_prox(vector_map,lr_idx_limit,lr_vec_cnt,i,tsc)==FAIL) {
						while (TDM_scheduler_map_find_y(vector_map,lr_idx_limit,i,0)!=mpass_backup_node) {
							TDM_scheduler_vector_rotate(vector_map[i], lr_idx_limit, 1);
							TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
							++inner_timeout;
							#ifdef _SET_TDM_DEV
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("\n\t>>> PASS 5 >>> Column %0d >>> Subpass %0d"),
                                         i,inner_timeout));
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("\n")));
							#endif
						}			
					}
					for (j=0; j<lr_idx_limit; j++) {
						TOKEN_CHECK(vector_map[i][j]) {
							if (TDM_scheduler_fit_singular_cnt(vector_map,j)>1) {
								node_transmuted=TDM_scheduler_nodal_transmute_lr(vector_map,freq,speed,tsc,traffic,lr_idx_limit,j,i,lr_vec_cnt);
								if (node_transmuted==BLANK) {
									#ifdef _SET_TDM_DEV
									LOG_WARN(BSL_LS_SOC_TDM,
                                             (BSL_META("\tTransmute operation at node [%0d,%0d] failed\n"),
                                              i,j));
									#endif
								}
								TDM_scheduler_weight_update(vector_map,lr_idx_limit,dr_weights,dr_weights_last,dr_weight_cmp,i,j,lr_slot_cnt);
								#ifdef _SET_TDM_DEV
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("\n\t>>> PASS 6 >>> Column %0d >>> Transmute %0d >>> Shift %0d")
                                             ,i,j,node_transmuted));
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("\n")));
								#endif
							}
						}
					}
				}
			}
		}
		if ( os_enable && (lr_vec_cnt>1) ) {
			/* Slice line rate regions */
			timeout=(lr_idx_limit*4); x=0;
			if ( (lr_idx_limit-lr_slot_cnt)>=((200*10)/BW_QUANTA) ) {
				/* If total core bandwidth allocated to OS >= 200G, required range of 1 */
				lr_burst_max_threshold=1;
			}
			else if ( (lr_idx_limit-lr_slot_cnt)<((200*10)/BW_QUANTA) && (lr_idx_limit-lr_slot_cnt)>=((100*10)/BW_QUANTA) ) {
				/* If total core bandwidth allocated to OS < 200G and >= 100G, required range of 2 */
				lr_burst_max_threshold=2;
			}
			else {
				/* For all lower bandwidths, use same acceptable range as front panel ports */
				lr_burst_max_threshold=((lr_idx_limit/lr_slot_cnt)+1);
			}
			do {
				dim_reduce_4=PASS;
				lr_burst_max=0;
				lr_burst_max_idx=0;
				for (k=0; k<lr_idx_limit; k++) {
					if (TDM_scheduler_fit_singular_cnt(vector_map,k)>0) {
						if (k!=lr_burst_max_last_idx) {
							x=1;
							for (v=1; v<60; v++) {
								if ( (k+v)<lr_idx_limit ) {
									if (TDM_scheduler_fit_singular_cnt(vector_map,(k+v))>0) {++x;}
									else {break;}
								}
							}
							if (x > lr_burst_max) {
								lr_burst_max = x;
								lr_burst_max_idx = k;
							}
						}
						else {
							while ( (TDM_scheduler_fit_singular_cnt(vector_map,k)>0) && (k<lr_idx_limit) ) {
								k++;
							}
						}
					}
				}
				if ( (lr_burst_max>lr_burst_max_threshold) && (!TDM_scheduler_exists(lr_burst_max_idx,slice_blacklist)) ) {
					lr_burst_max_last_idx=lr_burst_max_idx;
					slice_row_up=(lr_burst_max_idx);
					slice_col_up=TDM_scheduler_map_find_x(vector_map,lr_idx_limit,slice_row_up,0);
					slice_row_dn=(lr_burst_max_idx+lr_burst_max-1);
					slice_col_dn=TDM_scheduler_map_find_x(vector_map,lr_idx_limit,slice_row_dn,0);
					if (slice_row_dn<(lr_idx_limit-1)) {slice_result_up=TDM_scheduler_nodal_slice_lr(vector_map,freq,speed,tsc,traffic,lr_idx_limit,slice_row_dn,slice_col_dn,lr_vec_cnt,1,lr_burst_max);}
					if (slice_row_up>0) {slice_result_dn=TDM_scheduler_nodal_slice_lr(vector_map,freq,speed,tsc,traffic,lr_idx_limit,slice_row_up,slice_col_up,lr_vec_cnt,-1,lr_burst_max);}
					if (slice_result_up==FAIL && slice_result_dn==FAIL) {
						slice_blacklist[slice_blacklist_idx]=lr_burst_max_idx;
						slice_blacklist_idx++;
						#ifdef _SET_TDM_DEV
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("\n\tBanned port slice %d, slice blacklist index at %d\n"),
                                     lr_burst_max_idx,slice_blacklist_idx));
						#endif						
					}
					else {
						TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
					}
					dim_reduce_4=FAIL;
				}
			} while ( dim_reduce_4==FAIL && ((--timeout)>0) );
			/* Slice oversub regions using all remaining transmutation space */
			timeout=(lr_idx_limit*4); x=0; os_burst_max_threshold=(num_os>((lr_idx_limit/lr_slot_cnt)+1))?(((lr_idx_limit/lr_slot_cnt)+1)):(num_os);
			do {
				dim_reduce_4=PASS;
				os_burst_max=0;
				os_burst_max_idx=0;
				for (k=1; k<lr_idx_limit; k++) {
					if (TDM_scheduler_fit_singular_cnt(vector_map,k)==0) {
						if (k!=os_burst_max_last_idx) {
							x=1;
							for (v=1; v<60; v++) {
								if ( (k+v)<lr_idx_limit ) {
									if (TDM_scheduler_fit_singular_cnt(vector_map,(k+v))==0) {++x;}
									else {break;}
								}
							}
							if (x > os_burst_max) {
								os_burst_max = x;
								os_burst_max_idx = k;
							}
						}
						else {
							while (TDM_scheduler_fit_singular_cnt(vector_map,k)==0 && (k<lr_idx_limit) ) {
								k++;
							}
						}
					}
				}
				if ( (os_burst_max > os_burst_max_threshold) && (!TDM_scheduler_exists(os_burst_max_idx,slice_blacklist)) ) {
					os_burst_max_last_idx=os_burst_max_idx;
					slice_row_up=(os_burst_max_idx-1);
					slice_col_up=TDM_scheduler_map_find_x(vector_map,lr_idx_limit,slice_row_up,0);
					slice_row_dn=(os_burst_max_idx+os_burst_max);
					slice_col_dn=TDM_scheduler_map_find_x(vector_map,lr_idx_limit,slice_row_dn,0);
					/* Figure out slicing direction that creates smallest clusters */
					if (TDM_scheduler_fit_singular_cnt(vector_map,(slice_row_up-1))==0) {
						slice_row_up_cnt=1;
						for (v=1; v<60; v++) {
							if ( ((slice_row_up-1)-v)>0 ) {
								if (TDM_scheduler_fit_singular_cnt(vector_map,((slice_row_up-1)-v))==0) {++slice_row_up_cnt;}
								else {break;}
							}
						}
					}
					if (TDM_scheduler_fit_singular_cnt(vector_map,(slice_row_dn+1))==0) {
						slice_row_dn_cnt=1;
						for (v=1; v<60; v++) {
							if ( ((slice_row_dn+1)+v)>0 ) {
								if (TDM_scheduler_fit_singular_cnt(vector_map,((slice_row_dn+1)+v))==0) {++slice_row_dn_cnt;}
								else {break;}
							}
						}
					}
					slice_result_dn=PASS;
					if ( (slice_row_dn_cnt<=slice_row_up_cnt) || (slice_col_up==0) ) {
						if (slice_row_dn<lr_idx_limit) {
							slice_result_dn=TDM_scheduler_nodal_slice(vector_map,freq,speed,tsc,traffic,lr_idx_limit,slice_row_dn,slice_col_dn,lr_vec_cnt,-1);
						}
						else {
							slice_result_dn=FAIL;
						}
					}
					if ( (slice_row_dn_cnt>slice_row_up_cnt) || (slice_result_dn==FAIL) ) {
						slice_result_up = TDM_scheduler_nodal_slice(vector_map,freq,speed,tsc,traffic,lr_idx_limit,slice_row_up,slice_col_up,lr_vec_cnt,1);
					}
					if (slice_result_dn==FAIL && slice_result_up==FAIL) {
						slice_blacklist[slice_blacklist_idx]=os_burst_max_idx;
						slice_blacklist_idx++;
						#ifdef _SET_TDM_DEV
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("\n\tBanned oversub slice %d, slice blacklist index at %d\n"),
                                     os_burst_max_idx,slice_blacklist_idx));
						#endif
					}
					else {
						TDM_scheduler_vector_dump(file, vector_map, lr_idx_limit);
					}
					dim_reduce_4=FAIL;
				}
			} while ( dim_reduce_4==FAIL && ((--timeout)>0) );
		}
	}

	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n\n")));
	TDM_SML_BAR
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nSolution matrix pipe %0d:\n\n           "),
                 ((vector_map[0][0])/33)));
	for (v=0; v<VEC_MAP_WID; v++) {
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("%d"),v));
		if ((v+1)<VEC_MAP_WID) {
			if ((v+1)/10==0) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("___")));
			}
			else {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("__")));
			}
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n")));
	for (v=0; v<lr_idx_limit; v++) {
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("\n %d___\t"), v));
		for (j=0; j<VEC_MAP_WID; j++) {
			if (vector_map[j][v]!=_TH_NUM_EXT_PORTS) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %03d"), vector_map[j][v]));
			}
			else {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" ---")));
			}
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n\n\n")));
	TDM_SML_BAR
	
	/* Dump singular vector into 1-D array as TDM calender */
	for (j=0; j<lr_idx_limit; j++) {
		for (i=0; i<VEC_MAP_WID; i++) {
			if (vector_map[i][j]!=_TH_NUM_EXT_PORTS) {
				if (tdm_tbl[j]==_TH_NUM_EXT_PORTS) {
					tdm_tbl[j]=vector_map[i][j];
				}
				else {
					for (g=1; g<JITTER_THRESH_HI; g++) {
						if ( ((j-g)>0) && tdm_tbl[j-g]==_TH_NUM_EXT_PORTS ) {
							tdm_tbl[j-g]=vector_map[i][j];
						}
					}
				}
			}
		}
	}
	/* Free vector map */
	/* for (j=0; j<map_width; j++) {
		TDM_FREE(vector_map[j]);
	}
	TDM_FREE(vector_map); */
	/* Try to compress overflow back into legal size of TDM */
	for (j=lr_idx_limit; j<256; j++) {
		TOKEN_CHECK(tdm_tbl[j]) {
			for (i=1; i<60; i++) {
				if (tdm_tbl[lr_idx_limit-i]==_TH_NUM_EXT_PORTS) {
					tdm_tbl[lr_idx_limit-i]=tdm_tbl[j];
					tdm_tbl[j]=_TH_NUM_EXT_PORTS;
					break;
				}
			}
		}
	}	
 	/* port scan for last entry */
	cap=0;
	for (j=254; j>0; j--)
	{
		TOKEN_CHECK(tdm_tbl[j]) {
			cap=(j+1); 
			break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("TDM: _____VERBOSE: preprocessed TDM table terminates at index %0d\n"),
                 cap));
	/* find loopback TSC */
	if ( cap>0 && (speed[tdm_tbl[cap-1]]>=SPEED_10G) )
	{
		cap_tsc=_TH_NUM_EXT_PORTS;
		for (j=0; j<NUM_TSC; j++) {
			for (i=0; i<4; i++) {
				if (tsc[j][i]==tdm_tbl[cap-1]) {
					cap_tsc=j;
					break;
				}
			}
			if (cap_tsc!=_TH_NUM_EXT_PORTS) {
				break;
			}
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: TDM lookback warp core is %0d\n"),
                     cap_tsc));
		cap_lkbk=1;
		for (j=(i-2); j>=0; j--) {
			if (tdm_tbl[j]!=tsc[cap_tsc][0]&&tdm_tbl[j]!=tsc[cap_tsc][1]&&tdm_tbl[j]!=tsc[cap_tsc][2]&&tdm_tbl[j]!=tsc[cap_tsc][3]) {
				cap_lkbk++;
			}
			else {
				break;
			}
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: end of TDM table, lookback spacing is %0d\n"),
                     cap_lkbk));
		cap_lpbk=1;
		for (j=0; j<i; j++)
		{
			if (tdm_tbl[j]!=tsc[cap_tsc][0]&&tdm_tbl[j]!=tsc[cap_tsc][1]&&tdm_tbl[j]!=tsc[cap_tsc][2]&&tdm_tbl[j]!=tsc[cap_tsc][3]) {
				cap_lpbk++;
			}
			else {
				break;
			}
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: end of TDM table, loopback spacing is %0d\n"),
                     cap_lpbk));
		if (cap_lpbk <= cap_lkbk) {
			cap+=(cap_lkbk-cap_lpbk);
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("TDM: _____VERBOSE: postprocessed TDM table terminates at index %0d\n"),
                 cap));
	
	if (cap>lr_idx_limit)
	{
		LOG_ERROR(BSL_LS_SOC_TDM,
                  (BSL_META("Could not find solution TDM within max length at %0d MHz\n"), freq));
		return 0;
	}
	else {
		cap = lr_idx_limit;
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("TDM: _____VERBOSE: TDM extended to slot %0d\n"),
                 cap));
	
	if (os_enable && !lr_enable) {
		for (j=0; j<cap; j++) {
			if (tdm_tbl[j]==_TH_NUM_EXT_PORTS) {
				tdm_tbl[j] = OVSB_TOKEN;
			}
		}
	}
	else {
		for (j=0; j<cap; j++) {
			if (tdm_tbl[j]==_TH_NUM_EXT_PORTS) {
				if (os_enable) {
					tdm_tbl[j] = OVSB_TOKEN;
				}
				else {
					if (j%2==0) {
						tdm_tbl[j] = IDL1_TOKEN;
					}
					else {
						tdm_tbl[j] = IDL2_TOKEN;
					}		
				}
			}
		}
	}
	for (j=1; j<=accessories; j++) {
		g=TDM_scheduler_PQ((((10*cap)/accessories)*j))+(j-1);
		for (l=255; l>g; l--) {
			tdm_tbl[l]=tdm_tbl[l-1];
		}
		tdm_tbl[g]=ACC_TOKEN;
	}
	/* Post process smoothing */
	if (lr_enable && os_enable) {
		TDM_SML_BAR
		
		if ( ((((lr_idx_limit-lr_slot_cnt)*BW_QUANTA)/10)<120) && ((lr_10+lr_20+lr_40+lr_50+lr_100+lr_25)==1) && (freq==850) && (lr_100==1) ) {
			TDM_scheduler_filter_refactor(tdm_tbl,lr_idx_limit,accessories,lr_vec_cnt);
		}
		else {
			/* Local clump slice (line rate) */
			for (i=1; i<(lr_idx_limit-4); i++) {
				if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i-1]!=OVSB_TOKEN && tdm_tbl[i+1]==OVSB_TOKEN && tdm_tbl[i+2]==OVSB_TOKEN) {
					/* Down direction */
					/* TDM_scheduler_filter_local_slice(i,tdm_tbl,tsc,DN); */
					TDM_scheduler_filter_local_slice_dn(i,tdm_tbl,tsc);
				}
			}
			for (i=4; i<(lr_idx_limit-1); i++) {
				if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i+1]!=OVSB_TOKEN && tdm_tbl[i-1]==OVSB_TOKEN && tdm_tbl[i-2]==OVSB_TOKEN) {
					/* Do it again in up direction */
					/* TDM_scheduler_filter_local_slice(i,tdm_tbl,tsc,UP); */
					TDM_scheduler_filter_local_slice_up(i,tdm_tbl,tsc);
				}
			}
			/* Slice translation */
			for (i=1; i<(lr_idx_limit-4); i++) {
				if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i-1]!=OVSB_TOKEN && tdm_tbl[i+1]==OVSB_TOKEN && tdm_tbl[i+2]!=OVSB_TOKEN) {
					i=TDM_scheduler_filter_slice_dn(i,tdm_tbl,lr_idx_limit,tsc);
				}
			}
			for (i=4; i<(lr_idx_limit-1); i++) {
				if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i+1]!=OVSB_TOKEN && tdm_tbl[i-1]==OVSB_TOKEN && tdm_tbl[i-2]!=OVSB_TOKEN) {
					i=TDM_scheduler_filter_slice_up(i,tdm_tbl,lr_idx_limit,tsc);
				}
			}
			/* Local clump slice (oversub) */
			if ( TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,lr_idx_limit)<4 ) {
				TDM_scheduler_filter_local_slice_oversub(tdm_tbl,lr_idx_limit,tsc);
			}
			/* Smoothing filter ancillary slots */
			/* First possible ancillary slot is 13, lowest freq */
			for (i=13; i<(lr_idx_limit+accessories); i++) {
				if (tdm_tbl[i]==ACC_TOKEN && TDM_scheduler_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories))>=3) {
					TDM_scheduler_filter_ancillary_smooth(i,tdm_tbl,lr_idx_limit,accessories);
				}
			}
			/* Oversub and linerate dithering filter */
			timeout=DITHER_PASS;
			while ( TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>1 && TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>=TDM_scheduler_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && ((--timeout)>0) ) {
				TDM_scheduler_filter_dither(tdm_tbl,lr_idx_limit,accessories,tsc,DITHER_THRESHOLD);
			}
			timeout=DITHER_PASS;
			while ( TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>1 && TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>=TDM_scheduler_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && ((--timeout)>0) ) {
				TDM_scheduler_filter_dither(tdm_tbl,lr_idx_limit,accessories,tsc,5);
			}
			timeout=DITHER_PASS;
			while ( TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>1 && TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>=TDM_scheduler_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && ((--timeout)>0) ) {
				TDM_scheduler_filter_dither(tdm_tbl,lr_idx_limit,accessories,tsc,3);
			}
			/* Oversub fine dithering filter */
			/*
				1. If the min clump size of line rate is between 1 and 2 (high speed ports)
				2. Check prox tsc min spacing
				3. Split 1:3-1:3-1:3 slice by enlarging a size 1 slice somewhere else and then slicing this one
			*/
			if (TDM_scheduler_slice_size(1, tdm_tbl, (lr_idx_limit+accessories))==2) {
				/* 4 slices (normal) */
				for (i=3; i<(lr_idx_limit+accessories); i++) {
					if (tdm_tbl[i-3]!=OVSB_TOKEN && tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i+3]!=OVSB_TOKEN && tdm_tbl[i+6]!=OVSB_TOKEN && TDM_scheduler_slice_size_local((i-3), tdm_tbl, lr_idx_limit)==2 && TDM_scheduler_slice_size_local(i, tdm_tbl, lr_idx_limit)==2 && TDM_scheduler_slice_size_local((i+3), tdm_tbl, lr_idx_limit)==2 && TDM_scheduler_slice_size_local((i+6), tdm_tbl, lr_idx_limit)==2) {
						if (TDM_scheduler_filter_fine_dither(i,tdm_tbl,lr_idx_limit,accessories,tsc)==FAIL) {
							break;
						}
					}
				}
				/* Do it again at 3 slices (normal) */
				for (i=3; i<(lr_idx_limit+accessories); i++) {
					if (tdm_tbl[i-3]!=OVSB_TOKEN && tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i+3]!=OVSB_TOKEN && TDM_scheduler_slice_size_local((i-3), tdm_tbl, lr_idx_limit)==2 && TDM_scheduler_slice_size_local(i, tdm_tbl, lr_idx_limit)==2 && TDM_scheduler_slice_size_local((i+3), tdm_tbl, lr_idx_limit)==2) {
						if (TDM_scheduler_filter_fine_dither(i,tdm_tbl,lr_idx_limit,accessories,tsc)==FAIL) {
							break;
						}
					}
				}			
			}
		}
	}

	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("OS10-%0d, OS20-%0d, OS25-%0d, OS40-%0d, OS50-%0d, OS100-%0d, OS Types - %0d\n"),
                 z1, z2, z6, z3, z5, z4,
                 (os_10 + os_20 + os_25 + os_40 + os_50 + os_100)));	
	if ((os_10 + os_20 + os_25 + os_40 + os_50 + os_100) > OS_GROUP_NUM) {
		LOG_ERROR(BSL_LS_SOC_TDM,
                  (BSL_META("Oversub speed type limit exceeded\n")));
		return 0;
	}
	if (((os_10 + os_20 + os_25 + os_40 + os_50 + os_100)==OS_GROUP_NUM && (z6>_TH_OS_GROUP_LEN || z1>_TH_OS_GROUP_LEN || z2>_TH_OS_GROUP_LEN)) || ((os_10 + os_20 + os_25 + os_40 + os_50 + os_100)>=(OS_GROUP_NUM-1) && (z6>32 || z1>32 || z2>32)) || ((os_10 + os_20 + os_25 + os_40 + os_50 + os_100)>=(OS_GROUP_NUM-2) && (z6>48 || z1>48)) || ((os_10 + os_20 + os_25 + os_40 + os_50 + os_100)>=(OS_GROUP_NUM-3) && (z6>64 || z1>64))) {
		LOG_ERROR(BSL_LS_SOC_TDM,
                  (BSL_META("Oversub bucket overflow\n")));
		return 0;
	}
	
	/* dequeue from all non-empty ovs lists into buckets */
	if (z6 > 0) {
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Scheduling 25G oversub speed groups\n")));
		while (z6 > 0) {
			TDM_BIG_BAR
			while (z6>=_TH_OS_GROUP_LEN) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: currently in abundant sorting\n")));
				for (v=0; v<4; v++) {
					/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
					for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
					i = 0; tsc_swap[0] = os25[z6]; os25[z6] = _TH_NUM_EXT_PORTS;
					for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
					for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z6+1); a++) if (os25[a]==tsc[c][b]) {
						d=a;
						i++;
						tsc_swap[i] = tsc[c][b];
						for (f=d; f<z6; f++) os25[f] = os25[f+1];
					}
					z6-=(1+i);
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
					for (m=0; m<4; m++) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META(" %0d "), tsc[c][m]));
					}
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
					for (m=0; m<4; m++) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META(" %0d "), tsc_swap[m]));
					}
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					if (speed[tsc[c][0]]!=SPEED_25G || speed[tsc[c][2]]!=SPEED_25G || speed[tsc[c][1]]!=SPEED_25G || speed[tsc[c][3]]!=SPEED_25G) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: _____VERBOSE: the current TSC is a triport\n")));
						if (z11 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket1[z11+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 1: pointer at %0d triport content is %0d\n"),
                                             z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z22 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket2[z22+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 2: pointer at %0d triport content is %0d\n"),
                                             z22,
                                             bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z33 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket3[z33+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 3: pointer at %0d triport content is %0d\n"),
                                             z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z44 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket4[z44+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 4: pointer at %0d triport content is %0d\n"),
                                             z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z55 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket5[z55+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 5: pointer at %0d triport content is %0d\n"),
                                             z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}						
						else if (z66 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket6[z66+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 6: pointer at %0d triport content is %0d\n"),
                                             z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z77 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket7[z77+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 7: pointer at %0d triport content is %0d\n"),
                                             z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z88 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket8[z88+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 8: pointer at %0d triport content is %0d\n"),
                                             z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
					}
					else {
						if (z11 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */
								bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                             z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z22 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                             z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z33 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                             z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z44 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                             z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z55 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                             z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}						
						else if (z66 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                             z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}						
						else if (z77 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                             z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}						
						else if (z88 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                             z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
					}
					if (z6<_TH_OS_GROUP_LEN && z6>0) {
						break;
					}
				}
			}
			while (z6<_TH_OS_GROUP_LEN && z6>0) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: currently in scarce sorting\n")));
				/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
				for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
				i = 0; tsc_swap[0] = os25[z6]; os25[z6] = _TH_NUM_EXT_PORTS;
				for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
				for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z6+1); a++) if (os25[a]==tsc[c][b])
				{
					d=a;
					i++;
					tsc_swap[i] = tsc[c][b];
					for (f=d; f<z6; f++) os25[f] = os25[f+1];
				}
				z6-=(1+i);
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc[c][m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc_swap[m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				if (speed[tsc[c][0]]!=SPEED_25G || speed[tsc[c][2]]!=SPEED_25G || speed[tsc[c][1]]!=SPEED_25G || speed[tsc[c][3]]!=SPEED_25G) {
					if (z11 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d triport content is %0d\n"),
                                         z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z22 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d triport content is %0d\n"),
                                         z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z33 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d triport content is %0d\n"),
                                         z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z44 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d triport content is %0d\n"),
                                         z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z55 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d triport content is %0d\n"),
                                         z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z66 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d triport content is %0d\n"),
                                         z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z77 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d triport content is %0d\n"),
                                         z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z88 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d triport content is %0d\n"),
                                         z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
				}
				else {
					if (z11 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                         z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z22 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                         z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z33 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                         z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z44 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                         z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z55 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                         z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z66 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                         z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z77 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                         z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z88 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                         z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
				}
			}
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: _____VERBOSE: z6 pointer position is %0d\n"),
                         z6));
		}
		TDM_BIG_BAR
	}
	z11 = (z11>0) ? _TH_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TH_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TH_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TH_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TH_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TH_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TH_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TH_OS_GROUP_LEN:0;

	if (z1 > 0) {
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Scheduling 10G oversub speed groups\n")));
		while (z1 > 0) {
			TDM_BIG_BAR
			while (z1>=_TH_OS_GROUP_LEN) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: currently in abundant sorting\n")));
				for (v=0; v<4; v++) {
					/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
					for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
					i = 0; tsc_swap[0] = os10[z1]; os10[z1] = _TH_NUM_EXT_PORTS;
					for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
					for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z1+1); a++) if (os10[a]==tsc[c][b]) {
						d=a;
						i++;
						tsc_swap[i] = tsc[c][b];
						for (f=d; f<z1; f++) os10[f] = os10[f+1];
					}
					z1-=(1+i);
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
					for (m=0; m<4; m++) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META(" %0d "), tsc[c][m]));
					}
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
					for (m=0; m<4; m++) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META(" %0d "), tsc_swap[m]));
					}
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					if (speed[tsc[c][0]]>=SPEED_20G || speed[tsc[c][2]]>=SPEED_20G || speed[tsc[c][1]]>=SPEED_20G || speed[tsc[c][3]]>=SPEED_20G) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: _____VERBOSE: the current TSC is a triport\n")));
						if (z11 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket1[z11+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 1: pointer at %0d triport content is %0d\n"),
                                             z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z22 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket2[z22+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 2: pointer at %0d triport content is %0d\n"),
                                             z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z33 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket3[z33+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 3: pointer at %0d triport content is %0d\n"),
                                             z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z44 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket4[z44+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 4: pointer at %0d triport content is %0d\n"),
                                             z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z55 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket5[z55+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 5: pointer at %0d triport content is %0d\n"),
                                             z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}					
						else if (z66 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket6[z66+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 6: pointer at %0d triport content is %0d\n"),
                                             z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}					
						else if (z77 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket7[z77+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 7: pointer at %0d triport content is %0d\n"),
                                             z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}					
						else if (z88 < (_TH_OS_GROUP_LEN/2) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket8[z88+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 8: pointer at %0d triport content is %0d\n"),
                                             z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
					}
					else {
						if (z11 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                             z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z22 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                             z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )]));
							}	
							timeout=TIMEOUT;
							do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z33 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                             z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z44 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                             z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if (z55 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                             z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}					
						else if (z66 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                             z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}					
						else if (z77 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                             z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}					
						else if (z88 < (_TH_OS_GROUP_LEN/4) ) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
								bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
								LOG_VERBOSE(BSL_LS_SOC_TDM,
                                            (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                             z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )]));
							}
							timeout=TIMEOUT;
							do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
					}
				}
			}
			while (z1<_TH_OS_GROUP_LEN && z1>0) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: currently in scarce sorting\n")));
				/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
				for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
				i = 0; tsc_swap[0] = os10[z1]; os10[z1] = _TH_NUM_EXT_PORTS;
				for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
				for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z1+1); a++) if (os10[a]==tsc[c][b]) {
					d=a;
					i++;
					tsc_swap[i] = tsc[c][b];
					for (f=d; f<z1; f++) os10[f] = os10[f+1];
				}
				z1-=(1+i);
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc[c][m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc_swap[m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				if (speed[tsc[c][0]]>=SPEED_20G || speed[tsc[c][2]]>=SPEED_20G || speed[tsc[c][1]]>=SPEED_20G || speed[tsc[c][3]]>=SPEED_20G) {
					if (z11 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d triport content is %0d\n"),
                                         z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z22 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d triport content is %0d\n"),
                                         z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z33 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d triport content is %0d\n"),
                                         z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z44 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d triport content is %0d\n"),
                                         z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z55 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d triport content is %0d\n"),
                                         z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z66 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d triport content is %0d\n"),
                                         z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z77 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d triport content is %0d\n"),
                                         z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z88 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d triport content is %0d\n"),
                                         z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
				}
				else {
					if (z11 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                         z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z22 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                         z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )]));
						}	
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z33 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                         z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z44 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                         z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z55 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                         z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z66 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                         z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z77 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                         z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z88 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                         z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
				}
			}
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: _____VERBOSE: z1 pointer position is %0d\n"),
                         z1));
		}
		TDM_BIG_BAR
	}
	z11 = (z11>0) ? _TH_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TH_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TH_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TH_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TH_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TH_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TH_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TH_OS_GROUP_LEN:0;
	
	if (z2 > 0) {
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Scheduling 20G oversub speed groups\n")));
		while (z2 > 0) {
			TDM_BIG_BAR
			while (z2>=_TH_OS_GROUP_LEN) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: currently in abundant sorting\n")));
				/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
				for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
				i = 0; tsc_swap[0] = os20[z2]; os20[z2] = _TH_NUM_EXT_PORTS;
				for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
				for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z2+1); a++) if (os20[a]==tsc[c][b]) {
					d=a;
					i++;
					tsc_swap[i] = tsc[c][b];
					for (f=d; f<z2; f++) os20[f] = os20[f+1];
				}
				z2-=(1+i);
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc[c][m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc_swap[m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				if (speed[tsc[c][0]]<SPEED_20G || speed[tsc[c][2]]<SPEED_20G || speed[tsc[c][1]]<SPEED_20G || speed[tsc[c][3]]<SPEED_20G ||
					speed[tsc[c][0]]>SPEED_21G_DUAL || speed[tsc[c][2]]>SPEED_21G_DUAL || speed[tsc[c][1]]>SPEED_21G_DUAL || speed[tsc[c][3]]>SPEED_21G_DUAL) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: the current TSC is a triport\n")));
					if (z11 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d triport content is %0d\n"),
                                         z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z22 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d triport content is %0d\n"),
                                         z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z33 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d triport content is %0d\n"),
                                         z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z44 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d triport content is %0d\n"),
                                         z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z55 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d triport content is %0d\n"),
                                         z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z66 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d triport content is %0d\n"),
                                         z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z77 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d triport content is %0d\n"),
                                         z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z88 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d triport content is %0d\n"),
                                         z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
				}
				else {
					if (z11 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                         (z11+(m*(_TH_OS_GROUP_LEN/4))), bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z22 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                         (z22+(m*(_TH_OS_GROUP_LEN/4))), bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )]));
						}	
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z33 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                         (z33+(m*(_TH_OS_GROUP_LEN/4))), bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z44 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                         (z44+(m*(_TH_OS_GROUP_LEN/4))), bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z55 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                         (z55+(m*(_TH_OS_GROUP_LEN/4))), bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z66 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                         (z66+(m*(_TH_OS_GROUP_LEN/4))), bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z77 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                         (z77+(m*(_TH_OS_GROUP_LEN/4))), bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z88 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                         (z88+(m*(_TH_OS_GROUP_LEN/4))), bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
				}
			}
			while (z2<_TH_OS_GROUP_LEN && z2>0) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: currently in scarce sorting\n")));
				/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
				for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
				i = 0; tsc_swap[0] = os20[z2]; os20[z2] = _TH_NUM_EXT_PORTS;
				for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
				for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z2+1); a++) if (os20[a]==tsc[c][b]) {
					d=a;
					i++;
					tsc_swap[i] = tsc[c][b];
					for (f=d; f<z2; f++) os20[f] = os20[f+1];
				}
				z2-=(1+i);
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc[c][m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc_swap[m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));				
				if (speed[tsc[c][0]]<SPEED_20G || speed[tsc[c][2]]<SPEED_20G || speed[tsc[c][1]]<SPEED_20G || speed[tsc[c][3]]<SPEED_20G ||
					speed[tsc[c][0]]>SPEED_21G_DUAL || speed[tsc[c][2]]>SPEED_21G_DUAL || speed[tsc[c][1]]>SPEED_21G_DUAL || speed[tsc[c][3]]>SPEED_21G_DUAL) {
					if (z11 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                         (z11+(m*(_TH_OS_GROUP_LEN/2))), bucket1[z11+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z22 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                         (z22+(m*(_TH_OS_GROUP_LEN/2))), bucket2[z22+(m*(_TH_OS_GROUP_LEN/2) )]));
						}	
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z33 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                         (z33+(m*(_TH_OS_GROUP_LEN/2))), bucket3[z33+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z44 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                         (z44+(m*(_TH_OS_GROUP_LEN/2))), bucket4[z44+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z55 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                         (z55+(m*(_TH_OS_GROUP_LEN/2))), bucket5[z55+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z66 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                         (z66+(m*(_TH_OS_GROUP_LEN/2))), bucket6[z66+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z77 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                                  (z77+(m*(_TH_OS_GROUP_LEN/2))), bucket7[z77+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z88 < (_TH_OS_GROUP_LEN/2) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/2) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                         (z88+(m*(_TH_OS_GROUP_LEN/2))), bucket8[z88+(m*(_TH_OS_GROUP_LEN/2) )]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
				}
				else {
					if (z11 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                         (z11+(m*(_TH_OS_GROUP_LEN/4))), bucket1[z11+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z22 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                         (z22+(m*(_TH_OS_GROUP_LEN/4))), bucket2[z22+(m*(_TH_OS_GROUP_LEN/4) )]));
						}	
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z33 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                         (z33+(m*(_TH_OS_GROUP_LEN/4))), bucket3[z33+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z44 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                         (z44+(m*(_TH_OS_GROUP_LEN/4))), bucket4[z44+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if (z55 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                         (z55+(m*(_TH_OS_GROUP_LEN/4))), bucket5[z55+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z66 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                         (z66+(m*(_TH_OS_GROUP_LEN/4))), bucket6[z66+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z77 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                         (z77+(m*(_TH_OS_GROUP_LEN/4))), bucket7[z77+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if (z88 < (_TH_OS_GROUP_LEN/4) ) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                         (z88+(m*(_TH_OS_GROUP_LEN/4))), bucket8[z88+(m*(_TH_OS_GROUP_LEN/4) )]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
				}
			}
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: _____VERBOSE: z2 pointer position is %0d\n"),
                         z2));
		}
		TDM_BIG_BAR
	}
	z11 = (z11>0) ? _TH_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TH_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TH_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TH_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TH_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TH_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TH_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TH_OS_GROUP_LEN:0;
	
	if (z3 > 0) {
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Scheduling 40G oversub speed groups\n")));
		while (z3 > 0) {
			TDM_BIG_BAR
			/* all triport 20G ports are orphans */
			for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == os40[z3]) c=a;
			if (speed[tsc[c][0]]<SPEED_40G || speed[tsc[c][2]]<SPEED_40G || speed[tsc[c][1]]<SPEED_40G || speed[tsc[c][3]]<SPEED_40G ||
				speed[tsc[c][0]]>SPEED_42G || speed[tsc[c][2]]>SPEED_42G || speed[tsc[c][1]]>SPEED_42G || speed[tsc[c][3]]>SPEED_42G) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc[c][m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				tsc_swap[0] = os40[z3]; 
				os40[z3] = _TH_NUM_EXT_PORTS;
				z3--;
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc_swap[m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: the current TSC is a triport\n")));				
				if (z11 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket1[p]==_TH_NUM_EXT_PORTS) {
							z11=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket1[z11] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 1: pointer at %0d triport content is %0d\n"),
                                     z11, bucket1[z11]));
					}
					timeout=TIMEOUT;
					do {z11++; if (z11>=_TH_OS_GROUP_LEN) {break;}} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}
				else if (z22 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket2[p]==_TH_NUM_EXT_PORTS) {
							z22=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket2[z22] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 2: pointer at %0d triport content is %0d\n"),
                                     z22, bucket2[z22]));
					}
					timeout=TIMEOUT;
					do {z22++; if (z22>=_TH_OS_GROUP_LEN) {break;}} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}					
				else if (z33 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket3[p]==_TH_NUM_EXT_PORTS) {
							z33=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket3[z33] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 3: pointer at %0d triport content is %0d\n"),
                                     z33, bucket3[z33]));
					}
					timeout=TIMEOUT;
					do {z33++; if (z33>=_TH_OS_GROUP_LEN) {break;}} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}							
				else if (z44 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket4[p]==_TH_NUM_EXT_PORTS) {
							z44=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket4[z44] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 4: pointer at %0d triport content is %0d\n"),
                                     z44, bucket4[z44]));
					}
					timeout=TIMEOUT;
					do {z44++; if (z44>=_TH_OS_GROUP_LEN) {break;}} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}
				else if (z55 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket5[p]==_TH_NUM_EXT_PORTS) {
							z55=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket5[z55] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 5: pointer at %0d triport content is %0d\n"),
                                     z55, bucket5[z55]));
					}
					timeout=TIMEOUT;
					do {z55++; if (z55>=_TH_OS_GROUP_LEN) {break;}} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}				
				else if (z66 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket6[p]==_TH_NUM_EXT_PORTS) {
							z66=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket6[z66] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 6: pointer at %0d triport content is %0d\n"),
                                     z66, bucket6[z66]));
					}
					timeout=TIMEOUT;
					do {z66++; if (z66>=_TH_OS_GROUP_LEN) {break;}} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}				
				else if (z77 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket7[p]==_TH_NUM_EXT_PORTS) {
							z77=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket7[z77] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 7: pointer at %0d triport content is %0d\n"),
                                     z77, bucket7[z77]));
					}
					timeout=TIMEOUT;
					do {z77++; if (z77>=_TH_OS_GROUP_LEN) {break;}} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}				
				else if (z88 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket8[p]==_TH_NUM_EXT_PORTS) {
							z88=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket8[z88] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 8: pointer at %0d triport content is %0d\n"),
                                     z88, bucket8[z88]));
					}
					timeout=TIMEOUT;
					do {z88++; if (z88>=_TH_OS_GROUP_LEN) {break;}} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}
				TDM_SML_BAR
			}
			else {
				while (z3>=(_TH_OS_GROUP_LEN/2))
				{
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: currently in abundant sorting\n")));
					for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == os40[z3]) c=a;
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
					for (m=0; m<4; m++) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META(" %0d "), tsc[c][m]));
					}
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
					for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
					i = 0; tsc_swap[0] = os40[z3]; os40[z3] = _TH_NUM_EXT_PORTS;
					for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
					for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z3+1); a++) if (os40[a]==tsc[c][b])
					{
						d=a;
						i++;
						tsc_swap[i] = tsc[c][b];
						for (f=d; f<z3; f++) os40[f] = os40[f+1];
					}
					z3-=(1+i);
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
					for (m=0; m<4; m++) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META(" %0d "), tsc_swap[m]));
					}
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					if ((2*z11) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                         z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z22) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                         z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z33) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                         z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z44) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                         z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/2))]));
						}								
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z55) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                         z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/2))]));
						}								
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if ((2*z66) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                         z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/2))]));
						}								
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}							
					else if ((2*z77) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                         z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/2))]));
						}								
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}							
					else if ((2*z88) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                         z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/2))]));
						}								
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					TDM_SML_BAR
				}
				while (z3<(_TH_OS_GROUP_LEN/2) && z3>0) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: currently in scarce sorting\n")));
					for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == os40[z3]) c=a;
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
					for (m=0; m<4; m++) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META(" %0d "), tsc[c][m]));
					}
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
					for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
					i = 0; tsc_swap[0] = os40[z3]; os40[z3] = _TH_NUM_EXT_PORTS;
					for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
					for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z3+1); a++) if (os40[a]==tsc[c][b]) {
						d=a;
						i++;
						tsc_swap[i] = tsc[c][b];
						for (f=d; f<z3; f++) os40[f] = os40[f+1];
					}
					z3-=(1+i);
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
					for (m=0; m<4; m++) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META(" %0d "), tsc_swap[m]));
					}
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					if ((2*z11) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                         z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z22) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                         z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z33) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                         z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z44) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                         z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z55) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                         z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if ((2*z66) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                         z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if ((2*z77) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                         z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if ((2*z88) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                         z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					TDM_SML_BAR
				}
			}
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: _____VERBOSE: z3 pointer position is %0d\n"),
                         z3));
		}
		TDM_BIG_BAR
	}
	z11 = (z11>0) ? _TH_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TH_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TH_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TH_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TH_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TH_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TH_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TH_OS_GROUP_LEN:0;
	
	if (z5 > 0) {
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Scheduling 50G oversub speed groups\n")));
		while (z5 > 0) {
			TDM_BIG_BAR
			for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == os50[z5]) c=a;
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: _____VERBOSE: TSC lookup: ")));
			for (m=0; m<4; m++) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d "), tsc[c][m]));
			}
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("\n")));
			/* all triport 50G ports are orphans */
			if (speed[tsc[c][0]]!=SPEED_50G || speed[tsc[c][2]]!=SPEED_50G || speed[tsc[c][1]]!=SPEED_50G || speed[tsc[c][3]]!=SPEED_50G) {
				tsc_swap[0] = os50[z5]; 
				os50[z5] = _TH_NUM_EXT_PORTS;
				z5--;
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
				for (m=0; m<4; m++) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META(" %0d "), tsc_swap[m]));
				}
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n")));
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: the current TSC is a triport\n")));
				if (z11 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket1[p]==_TH_NUM_EXT_PORTS) {
							z11=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket1[z11] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 1: pointer at %0d triport content is %0d\n"),
                                     z11, bucket1[z11]));
					}
					timeout=TIMEOUT;
					do {z11++; if (z11>=_TH_OS_GROUP_LEN) {break;}} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}
				else if (z22 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket2[p]==_TH_NUM_EXT_PORTS) {
							z22=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket2[z22] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 2: pointer at %0d triport content is %0d\n"),
                                     z22, bucket2[z22]));
					}
					timeout=TIMEOUT;
					do {z22++; if (z22>=_TH_OS_GROUP_LEN) {break;}} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}					
				else if (z33 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket3[p]==_TH_NUM_EXT_PORTS) {
							z33=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket3[z33] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 3: pointer at %0d triport content is %0d\n"),
                                     z33, bucket3[z33]));
					}
					timeout=TIMEOUT;
					do {z33++; if (z33>=_TH_OS_GROUP_LEN) {break;}} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}							
				else if (z44 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket4[p]==_TH_NUM_EXT_PORTS) {
							z44=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket4[z44] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 4: pointer at %0d triport content is %0d\n"),
                                     z44, bucket4[z44]));
					}
					timeout=TIMEOUT;
					do {z44++; if (z44>=_TH_OS_GROUP_LEN) {break;}} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}
				else if (z55 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket5[p]==_TH_NUM_EXT_PORTS) {
							z55=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket5[z55] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 5: pointer at %0d triport content is %0d\n"),
                                     z55, bucket5[z55]));
					}
					timeout=TIMEOUT;
					do {z55++; if (z55>=_TH_OS_GROUP_LEN) {break;}} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}		
				else if (z66 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket6[p]==_TH_NUM_EXT_PORTS) {
							z66=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket6[z66] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 6: pointer at %0d triport content is %0d\n"),
                                     z66, bucket6[z66]));
					}
					timeout=TIMEOUT;
					do {z66++; if (z66>=_TH_OS_GROUP_LEN) {break;}} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}				
				else if (z77 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket7[p]==_TH_NUM_EXT_PORTS) {
							z77=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket7[z77] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 7: pointer at %0d triport content is %0d\n"),
                                     z77, bucket7[z77]));
					}
					timeout=TIMEOUT;
					do {z77++; if (z77>=_TH_OS_GROUP_LEN) {break;}} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}				
				else if (z88 < _TH_OS_GROUP_LEN) {
					q=0;
					for (p=0; p<_TH_OS_GROUP_LEN; p++) {
						if (bucket8[p]==_TH_NUM_EXT_PORTS) {
							z88=p;
							break;
						}
						else {
							q++;
						}
					}
					if (q<_TH_OS_GROUP_LEN) {
						bucket8[z88] = tsc_swap[0];
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: Speed Bin 8: pointer at %0d triport content is %0d\n"),
                                     z88, bucket8[z88]));
					}
					timeout=TIMEOUT;
					do {z88++; if (z88>=_TH_OS_GROUP_LEN) {break;}} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
				}
			}
			else {
				while (z5>=(_TH_OS_GROUP_LEN/2)) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: currently in abundant sorting\n")));
					for (v=0; v<3; v++) {
						/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
						for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
						i = 0; tsc_swap[0] = os50[z5]; os50[z5] = _TH_NUM_EXT_PORTS;
						for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
						for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z5+1); a++) if (os50[a]==tsc[c][b])
						{
							d=a;
							i++;
							tsc_swap[i] = tsc[c][b];
							for (f=d; f<z5; f++) os50[f] = os50[f+1];
						}
						z5-=(1+i);
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
						for (m=0; m<4; m++) {
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META(" %0d "), tsc_swap[m]));
						}
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META("\n")));
						if ((2*z11) <= _TH_OS_GROUP_LEN-i) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */
								if ( (z11+(m*(_TH_OS_GROUP_LEN/2))) < _TH_OS_GROUP_LEN ) {
									bucket1[z11+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                                 z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/2))]));
								}
							}
							timeout=TIMEOUT;
							do {z11++; if (z11>=_TH_OS_GROUP_LEN) {break;}} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if ((2*z22) <= _TH_OS_GROUP_LEN-i) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */
								if ( (z22+(m*(_TH_OS_GROUP_LEN/2))) < _TH_OS_GROUP_LEN ) {
									bucket2[z22+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                                 z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/2))]));
								}
							}
							timeout=TIMEOUT;
							do {z22++; if (z22>=_TH_OS_GROUP_LEN) {break;}} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if ((2*z33) <= _TH_OS_GROUP_LEN-i) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */
								if ( (z33+(m*(_TH_OS_GROUP_LEN/2))) < _TH_OS_GROUP_LEN ) {
									bucket3[z33+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                                 z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/2))]));
								}
							}
							timeout=TIMEOUT;
							do {z33++; if (z33>=_TH_OS_GROUP_LEN) {break;}} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if ((2*z44) <= _TH_OS_GROUP_LEN-i) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */
								if ( (z44+(m*(_TH_OS_GROUP_LEN/2))) < _TH_OS_GROUP_LEN ) {
									bucket4[z44+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                                 z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/2))]));
								}
							}
							timeout=TIMEOUT;
							do {z44++; if (z44>=_TH_OS_GROUP_LEN) {break;}} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
						else if ((2*z55) <= _TH_OS_GROUP_LEN-i) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */
								if ( (z55+(m*(_TH_OS_GROUP_LEN/2))) < _TH_OS_GROUP_LEN ) {
									bucket5[z55+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                                 z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/2))]));
								}
							}
							timeout=TIMEOUT;
							do {z55++; if (z55>=_TH_OS_GROUP_LEN) {break;}} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}						
						else if ((2*z66) <= _TH_OS_GROUP_LEN-i) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */
								if ( (z66+(m*(_TH_OS_GROUP_LEN/2))) < _TH_OS_GROUP_LEN ) {
									bucket6[z66+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                                 z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/2))]));
								}
							}
							timeout=TIMEOUT;
							do {z66++; if (z66>=_TH_OS_GROUP_LEN) {break;}} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}						
						else if ((2*z77) <= _TH_OS_GROUP_LEN-i) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */
								if ( (z77+(m*(_TH_OS_GROUP_LEN/2))) < _TH_OS_GROUP_LEN ) {
									bucket7[z77+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                                 z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/2))]));
								}
							}
							timeout=TIMEOUT;
							do {z77++; if (z77>=_TH_OS_GROUP_LEN) {break;}} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}						
						else if ((2*z88) <= _TH_OS_GROUP_LEN-i) {
							for (m = 0; m <= i; m++) {
								/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */
								if ( (z88+(m*(_TH_OS_GROUP_LEN/2))) < _TH_OS_GROUP_LEN ) {
									bucket8[z88+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
									LOG_VERBOSE(BSL_LS_SOC_TDM,
                                                (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                                 z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/2))]));
								}
							}
							timeout=TIMEOUT;
							do {z88++; if (z88>=_TH_OS_GROUP_LEN) {break;}} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
						}
					}
				}
				while (z5<(_TH_OS_GROUP_LEN/2) && z5>0) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: currently in scarce sorting\n")));
					/* 1G, 10G, and 20G need additional mutable stack to ensure no TSC splitting */
					for (i = 0; i < 4; i++) tsc_swap[i] = _TH_NUM_EXT_PORTS;
					i = 0; tsc_swap[0] = os50[z5]; os50[z5] = _TH_NUM_EXT_PORTS;
					for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (tsc[a][b] == tsc_swap[0]) c=a;
					for (b=0; b<4; b++) if (tsc[c][b]!=tsc_swap[0] && tsc[c][b]!=_TH_NUM_EXT_PORTS) for (a=1; a<(z5+1); a++) if (os50[a]==tsc[c][b])
					{
						d=a;
						i++;
						tsc_swap[i] = tsc[c][b];
						for (f=d; f<z5; f++) os50[f] = os50[f+1];
					}
					z5-=(1+i);
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: Buffer fetch: ")));
					for (m=0; m<4; m++) {
						LOG_VERBOSE(BSL_LS_SOC_TDM,
                                    (BSL_META(" %0d "), tsc_swap[m]));
					}
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\n")));
					if ((2*z11) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket1[z11+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                                         z11, bucket1[z11+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z11++;} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z22) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket2[z22+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                                         z22, bucket2[z22+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z22++;} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z33) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket3[z33+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                                         z33, bucket3[z33+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z33++;} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z44) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket4[z44+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                                         z44, bucket4[z44+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z44++;} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
					else if ((2*z55) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket5[z55+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                                         z55, bucket5[z55+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z55++;} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if ((2*z66) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket6[z66+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                                         z66, bucket6[z66+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z66++;} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if ((2*z77) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket7[z77+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                                         z77, bucket7[z77+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z77++;} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}					
					else if ((2*z88) <= _TH_OS_GROUP_LEN-i) {
						for (m = 0; m <= i; m++) {
							/* if (tsc_swap[m] != _TH_NUM_EXT_PORTS) */ 
							bucket8[z88+(m*(_TH_OS_GROUP_LEN/2))] = tsc_swap[m];
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                                         z88, bucket8[z88+(m*(_TH_OS_GROUP_LEN/2))]));
						}
						timeout=TIMEOUT;
						do {z88++;} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
					}
				}
			}
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: _____VERBOSE: z5 pointer position is %0d\n"),
                         z5));
		}
		TDM_BIG_BAR
	}
	z11 = (z11>0) ? _TH_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TH_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TH_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TH_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TH_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TH_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TH_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TH_OS_GROUP_LEN:0;
	
	while (z4 > 0) {
		TDM_BIG_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Scheduling 100G oversub speed groups\n")));
		if (z11 < _TH_OS_GROUP_LEN) {
			bucket1[z11] = os100[z4];
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Speed Bin 1: pointer at %0d content is %0d\n"),
                         z11, bucket1[z11]));
			timeout=TIMEOUT;
			do {z11++; if (z11>=_TH_OS_GROUP_LEN) {break;}} while ((bucket1[z11]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
			z4--;
		}
		else if (z22 < _TH_OS_GROUP_LEN) {
			bucket2[z22] = os100[z4];
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Speed Bin 2: pointer at %0d content is %0d\n"),
                         z22, bucket2[z22]));
			timeout=TIMEOUT;
			do {z22++; if (z22>=_TH_OS_GROUP_LEN) {break;}} while ((bucket2[z22]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
			z4--;
		}
		else if (z33 < _TH_OS_GROUP_LEN) {
			bucket3[z33] = os100[z4];
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Speed Bin 3: pointer at %0d content is %0d\n"),
                         z33, bucket3[z33]));
			timeout=TIMEOUT;
			do {z33++; if (z33>=_TH_OS_GROUP_LEN) {break;}} while ((bucket3[z33]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
			z4--;
		}
		else if (z44 < _TH_OS_GROUP_LEN) {
			bucket4[z44] = os100[z4];
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Speed Bin 4: pointer at %0d content is %0d\n"),
                         z44, bucket4[z44]));
			timeout=TIMEOUT;
			do {z44++; if (z44>=_TH_OS_GROUP_LEN) {break;}} while ((bucket4[z44]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
			z4--;
		}
		else if (z55 < _TH_OS_GROUP_LEN) {
			bucket5[z55] = os100[z4];
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Speed Bin 5: pointer at %0d content is %0d\n"),
                         z55, bucket5[z55]));
			timeout=TIMEOUT;
			do {z55++; if (z55>=_TH_OS_GROUP_LEN) {break;}} while ((bucket5[z55]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
			z4--;
		}		
		else if (z66 < _TH_OS_GROUP_LEN) {
			bucket6[z66] = os100[z4];
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Speed Bin 6: pointer at %0d content is %0d\n"),
                         z66, bucket6[z66]));
			timeout=TIMEOUT;
			do {z66++; if (z66>=_TH_OS_GROUP_LEN) {break;}} while ((bucket6[z66]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
			z4--;
		}		
		else if (z77 < _TH_OS_GROUP_LEN) {
			bucket7[z77] = os100[z4];
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Speed Bin 7: pointer at %0d content is %0d\n"),
                         z77, bucket7[z77]));
			timeout=TIMEOUT;
			do {z77++; if (z77>=_TH_OS_GROUP_LEN) {break;}} while ((bucket7[z77]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
			z4--;
		}		
		else {
			bucket8[z88] = os100[z4];
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: Speed Bin 8: pointer at %0d content is %0d\n"),
                         z88, bucket8[z88]));
			timeout=TIMEOUT;
			do {z88++; if (z88>=_TH_OS_GROUP_LEN) {break;}} while ((bucket8[z88]!=_TH_NUM_EXT_PORTS) && ((--timeout)>0));
			z4--;
		}
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: z4 pointer position is %0d\n"),
                     z4));
		TDM_BIG_BAR
	}
	
	return 1;
	
}


/**
@name: TDM_scheduler_map_distance_x
@param: unsigned char **, int, int, int

Returns diagonal distance between two nearest neighbor nodes in the x dimension
**/
int TDM_scheduler_map_distance_x(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2) 
{
	int i;
	unsigned int a=0, b=0, dist;
	
	for (i=0; i<limit; i++) {
		if (map[idx1][i]!=_TH_NUM_EXT_PORTS) {
			a = i;
			break;
		}
	}
	for (i=0; i<limit; i++) {
		if (map[idx2][i]!=_TH_NUM_EXT_PORTS) {
			b = i;
			break;
		}
	}
	
	#ifdef _CMATH
		dist = sqrt(((tdm_abs(idx2-idx1))*(tdm_abs(idx2-idx1)))+((tdm_abs(b-a))*(tdm_abs(b-a))));
	#else
		dist = tdm_sqrt(((tdm_abs(idx2-idx1))*(tdm_abs(idx2-idx1)))+((tdm_abs(b-a))*(tdm_abs(b-a))));
	#endif
	
	return dist;
	
}


/**
@name: TDM_scheduler_map_shortest_distance_calc
@param: unsigned char **, int, int, int

Returns shortest vertical distance between nodes
**/
int TDM_scheduler_map_shortest_distance_calc(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2, unsigned int start)
{
	int i;
	unsigned int a=start, dist, up=limit, down=limit;
	
	for (i=0; i<limit; i++) {
		if ((a+i)>=limit) {break;}
		if (map[idx2][i+a]!=_TH_NUM_EXT_PORTS) {
			down = (i+a);
			break;
		}
	}
	for (i=0; i<=a; i++) {
		if (map[idx2][a-i]!=_TH_NUM_EXT_PORTS) {
			up = (a-i);
			break;
		}
	}
	
	dist = (tdm_abs(up-a)>tdm_abs(down-a))?(tdm_abs(down-a)):(tdm_abs(up-a));
	
	return dist;
}


/**
@name: TDM_scheduler_map_shortest_distance
@param: unsigned char **, int, int, int

Calls shortest distance calc and returns only shortest among all nodes
**/
int TDM_scheduler_map_shortest_distance(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2) 
{
	int i, last_dist, dist=_TH_NUM_EXT_PORTS;
	
	for (i=0; i<limit; i++) {
		if (map[idx1][i]!=_TH_NUM_EXT_PORTS) {
			last_dist = TDM_scheduler_map_shortest_distance_calc(map,limit,idx1,idx2,i);
			if (last_dist < dist) {
				dist = last_dist;
			}
		}
	}
	
	return dist;
}


/**
@name: TDM_scheduler_map_distance_y
@param: unsigned char **, int, int, int

Returns diagonal distance between two nearest neighbor nodes in the y dimension
**/
int TDM_scheduler_map_distance_y(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2) 
{
	int i;
	unsigned int a=0, b=0, dist;
	
	for (i=0; i<VEC_MAP_WID; i++) {
		if (map[i][idx1]!=_TH_NUM_EXT_PORTS) {
			a = i;
			break;
		}
	}
	for (i=0; i<VEC_MAP_WID; i++) {
		if (map[i][idx2]!=_TH_NUM_EXT_PORTS) {
			b = i;
			break;
		}
	}
	
	#ifdef _CMATH
		dist = sqrt(((tdm_abs(idx2-idx1))*(tdm_abs(idx2-idx1)))+((tdm_abs(b-a))*(tdm_abs(b-a))));
	#else
		dist = tdm_sqrt(((tdm_abs(idx2-idx1))*(tdm_abs(idx2-idx1)))+((tdm_abs(b-a))*(tdm_abs(b-a))));
	#endif
	
	return dist;
	
}


/**
@name: TDM_scheduler_map_transpose
@param: unsigned char **, int, int, int

Transpose in-place the position of two vectors of the same speed. Returns if they are different speeds.
**/
int TDM_scheduler_map_transpose(unsigned char **map, int limit, unsigned int xpos_1, unsigned int xpos_2)
{
	int i, store1=_TH_NUM_EXT_PORTS, store2=_TH_NUM_EXT_PORTS, count1=0, count2=0;
	
	for (i=0; i<limit; i++) {
		if (map[xpos_1][i]!=_TH_NUM_EXT_PORTS) {
			store1=map[xpos_1][i];
			count1++;
		}
	}
	for (i=0; i<limit; i++) {
		if (map[xpos_2][i]!=_TH_NUM_EXT_PORTS) {
			store2=map[xpos_2][i];
			count2++;
		}
	}
	if (count1!=count2) {
		return 0;
	}
	else {
		for (i=0; i<limit; i++) {
			if (map[xpos_1][i]==store1) {
				map[xpos_1][i]=store2;
			}
		}
		for (i=0; i<limit; i++) {
			if (map[xpos_2][i]==store2) {
				map[xpos_2][i]=store1;
			}
		}
	}

	return 1;
	
}


/**
@name: TDM_scheduler_vector_rotate_step
@param: int[], int, int

Performs rotation transform on input vector array of nodes in integer steps, can wrap around feature space
**/
void TDM_scheduler_vector_rotate_step(unsigned char vector[], int size, int step)
{
	int i,j, last;
	
	if (step > 0) {
		for (j=0; j<step; j++) {
			last=vector[size-1];
			for (i=(size-1); i>0; i--) {
				vector[i]=vector[i-1];
			}
			vector[0]=last;
		}
	}
	else if (step < 0) {
		for (j=(tdm_abs(step)); j>0; j--) {
			last=vector[0];
			for (i=0; i<size; i++) {
				vector[i]=vector[i+1];
			}
			vector[size-1]=last;
		}
	}
}


/**
@name: TDM_scheduler_vector_rotate
@param: int[], int, int

Wraps rotation transform contingent upon the skew of the vector
**/
void TDM_scheduler_vector_rotate(unsigned char vector[], int size, int step)
{
	int i;
	
	/* 
	 * If congruent vectors are allowed to wrap in rotation their fractional average spaced nodes will
	 * skew. That will cause hang in aligning the vectors.
	 */ 
	if (step > 0) {
		for (i=0; i<step; i++) {
			if (vector[size-1]!=_TH_NUM_EXT_PORTS && i<step) {
				while (vector[0]==_TH_NUM_EXT_PORTS) {
					TDM_scheduler_vector_rotate_step(vector, size, -1);
				}
			}
			else {
				TDM_scheduler_vector_rotate_step(vector, size, 1);
			}
		}
	}
	else {
		TDM_scheduler_vector_rotate_step(vector, size, step);
	}
}


/**
@name: TDM_scheduler_vector_translate
@param: int[], int, int

Performs transposition transform on input vector array of nodes in integer steps, always clamped to feature space
**/
void TDM_scheduler_vector_translate(unsigned char vector[], int size, int step)
{
	int i,j;
	
	for (j=(tdm_abs(step)); j>0; j--) {
		if (vector[0]!=_TH_NUM_EXT_PORTS) {
			break;
		}
		for (i=0; i<size; i++) {
			vector[i]=vector[i+1];
		}
	}
	
}


/**
@name: TDM_scheduler_vector_clear
@param: int[], int, int

Remove vector at index in TDM vector map
**/
void TDM_scheduler_vector_clear(unsigned char **map, int yy, int len)
{
	int i;
	
	for (i=0; i<len; i++) {
		map[yy][i]=_TH_NUM_EXT_PORTS;
	}
	
}


/**
@name: TDM_scheduler_nodal_transmute_lr
@param:

Transmutes individual nodes. Use for line rate.
**/
int TDM_scheduler_nodal_transmute_lr(unsigned char **map, int freq, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt)
{
	int slot_cnt=0, jitter, v, shift, up=0, down=0;
	
	for (v=0; v<lr_idx_limit; v++) {TOKEN_CHECK(map[col][v]) {slot_cnt++;}}
	switch(slot_cnt) {
		case 4:
			jitter = JITTER_THRESH_HI; break;
		case 8: case 10:
			jitter = JITTER_THRESH_MH; break;
		case 16: case 20:
			jitter = JITTER_THRESH_ML; break;
		default:
			jitter = JITTER_THRESH_LO; break;
	}
	
	do {++up;} while ( ((TDM_scheduler_fit_singular_cnt(map,(row-up))>0) || (TDM_scheduler_fit_prox_node(map,(row),lr_vec_cnt,col,tsc,(row-up))==FAIL)) && ( (map[col][row-up]!=map[col][row]) || ((row-up)>0) ) );
	do {++down;} while ( ((TDM_scheduler_fit_singular_cnt(map,(row+down))>0) || (TDM_scheduler_fit_prox_node(map,(row),lr_vec_cnt,col,tsc,(row+down))==FAIL)) && ( (map[col][row+down]!=map[col][row]) || ((row+down)<lr_idx_limit) ) );
	shift = (up>down) ? (row+down):(row-up);
	if ( (TDM_scheduler_fit_singular_cnt(map,shift)==0) && (TDM_scheduler_fit_prox_node(map,shift,lr_vec_cnt,col,tsc,shift)==PASS) ) {
		if (tdm_abs(row-shift) > jitter) {
			#ifdef _SET_TDM_DEV
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("\n")));
			LOG_WARN(BSL_LS_SOC_TDM,
                     (BSL_META("Node transmute @ [%0d,%0d] shift %0d will exceed jitter threshold by %0d\n"),
                      col,row,shift,tdm_abs(row-shift)));
			#endif
		}
		map[col][shift]=map[col][row];
		map[col][row]=_TH_NUM_EXT_PORTS;
		#ifdef _SET_TDM_DEV
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("\n\tNode %0d transmute for line rate @ [%0d,%0d] shift %0d\n\n"),
                     map[col][shift],col,row,shift));
		#endif
		if ( (map[col][shift+1]!=_TH_NUM_EXT_PORTS) ||
		     (map[col][shift+2]!=_TH_NUM_EXT_PORTS) ||
		     (map[col][shift+3]!=_TH_NUM_EXT_PORTS) ||
		     (map[col][shift-1]!=_TH_NUM_EXT_PORTS) ||
		     (map[col][shift-2]!=_TH_NUM_EXT_PORTS) ||
		     (map[col][shift-3]!=_TH_NUM_EXT_PORTS) ) {
				map[col][row]=map[col][shift];
				map[col][shift]=_TH_NUM_EXT_PORTS;
				#ifdef _SET_TDM_DEV
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\tReverted illegal transmute at [%0d,%0d]=%0d\n"),
                             col,shift,map[col][row]));
				#endif
				return BLANK;
			}
		else {
			return shift;
		}
	}
	else {
		return BLANK;
	}
}


/**
@name: TDM_scheduler_nodal_transmute_slice
@param:

Transmutes individual nodes with preference for slicing empty regions. Use for oversub.
**/
int TDM_scheduler_nodal_slice(unsigned char **map, int freq, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt, int direction)
{
	int i, node_transmuted=BOOL_FALSE, result=FAIL, prox=PASS;
	
	if (direction==1) {
		if (map[col][row]!=_TH_NUM_EXT_PORTS) {
			if (map[col][row+1]==_TH_NUM_EXT_PORTS && map[col][row+2]==_TH_NUM_EXT_PORTS) {
				map[col][row+1]=map[col][row];
				map[col][row]=_TH_NUM_EXT_PORTS;
				node_transmuted=BOOL_TRUE;
				result=PASS;
			}
			if (node_transmuted == BOOL_TRUE) {
				#ifdef _SET_TDM_DEV
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n\tNode %0d transmute for slice @ [%0d,%0d] shift %0d\n"),
                             map[col][row+1],col,row,(row+1)));
				#endif
				for (i=1; i<4; i++) {
					if (map[col][row+1]==map[col][row+1+i]) {
						prox=FAIL;
						break;
					}
				}
				if ( (TDM_scheduler_fit_prox(map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) || (prox==FAIL) ) {
					map[col][row]=map[col][row+1];
					map[col][row+1]=_TH_NUM_EXT_PORTS;
					#ifdef _SET_TDM_DEV
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\tReverted illegal transmute at [%0d,%0d]=%0d\n"),
                                 col,row,map[col][row]));
					#endif
					result=FAIL;
				}
			}
		}
	}
	else if (direction==-1) {
		if (map[col][row]!=_TH_NUM_EXT_PORTS) {
			if (map[col][row-1]==_TH_NUM_EXT_PORTS && map[col][row-2]==_TH_NUM_EXT_PORTS) {
				map[col][row-1]=map[col][row];
				map[col][row]=_TH_NUM_EXT_PORTS;
				node_transmuted=BOOL_TRUE;
				result=PASS;
			}
			if (node_transmuted == BOOL_TRUE) {
				#ifdef _SET_TDM_DEV
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n\tNode %0d transmute for slice @ [%0d,%0d] shift %0d\n"),
                             map[col][row-1],col,row,(row-1)));
				#endif
				for (i=1; i<4; i++) {
					if (map[col][row-1]==map[col][row-1-i]) {
						prox=FAIL;
						break;
					}
				}
				if ( (TDM_scheduler_fit_prox(map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) || (prox==FAIL) ) {
					map[col][row]=map[col][row-1];
					map[col][row-1]=_TH_NUM_EXT_PORTS;
					#ifdef _SET_TDM_DEV
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\tReverted illegal transmute at [%0d,%0d]=%0d\n"),
                                 col,row,map[col][row]));
					#endif
					result=FAIL;
				}
			}
		}
	}
	
	return result;
	
}


/**
@name: TDM_scheduler_nodal_transmute_slice_lr
@param:

Transmutes individual nodes with preference for slicing empty regions. Use for line rate.
**/
int TDM_scheduler_nodal_slice_lr(unsigned char **map, int freq, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt, int direction, int curr_cnt)
{
	int i, node_transmuted=BOOL_FALSE, result=FAIL, prox=PASS, cnt=1;
	
	if (direction==1) {
		if (map[col][row]!=_TH_NUM_EXT_PORTS && map[col][row+1]==_TH_NUM_EXT_PORTS) {
			for (i=1; i<60; i++) {
				if ((row+1+i)<lr_idx_limit) {
					if (TDM_scheduler_fit_singular_cnt(map,(row+1+i))>0) {++cnt;}
					else {break;}
				}
				else {break;}
			}
			if (map[col][row+2]==_TH_NUM_EXT_PORTS || cnt<curr_cnt) {
				map[col][row+1]=map[col][row];
				map[col][row]=_TH_NUM_EXT_PORTS;
				node_transmuted=BOOL_TRUE;
				result=PASS;
			}
			if (node_transmuted == BOOL_TRUE) {
				#ifdef _SET_TDM_DEV
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n\tNode %0d transmute for slice @ [%0d,%0d] shift %0d\n"),
                             map[col][row+1],col,row,(row+1)));
				#endif
				for (i=1; i<4; i++) {
					if (map[col][row+1]==map[col][row+1+i]) {
						prox=FAIL;
						break;
					}
				}
				if ( (TDM_scheduler_fit_prox(map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) || (prox==FAIL) ) {
					map[col][row]=map[col][row+1];
					map[col][row+1]=_TH_NUM_EXT_PORTS;
					#ifdef _SET_TDM_DEV
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\tReverted illegal transmute at [%0d,%0d]=%0d\n"),
                                 col,row,map[col][row]));
					#endif
					result=FAIL;
				}
			}
		}
	}
	else if (direction==-1) {
		if (map[col][row]!=_TH_NUM_EXT_PORTS && map[col][row-1]==_TH_NUM_EXT_PORTS) {
			for (i=1; i<60; i++) {
				if ((row-1-i)>0) {
					if (TDM_scheduler_fit_singular_cnt(map,(row-1-i))>0) {++cnt;}
					else {break;}
				}
				else {break;}
			}
			if (map[col][row-2]==_TH_NUM_EXT_PORTS || cnt<curr_cnt) {
				map[col][row-1]=map[col][row];
				map[col][row]=_TH_NUM_EXT_PORTS;
				node_transmuted=BOOL_TRUE;
				result=PASS;
			}
			if (node_transmuted == BOOL_TRUE) {
				#ifdef _SET_TDM_DEV
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("\n\tNode %0d transmute for slice @ [%0d,%0d] shift %0d\n"),
                             map[col][row-1],col,row,(row-1)));
				#endif
				for (i=1; i<4; i++) {
					if (map[col][row-1]==map[col][row-1-i]) {
						prox=FAIL;
						break;
					}
				}
				if ( (TDM_scheduler_fit_prox(map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) || (prox==FAIL) ) {
					map[col][row]=map[col][row-1];
					map[col][row-1]=_TH_NUM_EXT_PORTS;
					#ifdef _SET_TDM_DEV
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("\tReverted illegal transmute at [%0d,%0d]=%0d\n"),
                                 col,row,map[col][row]));
					#endif
					result=FAIL;
				}
			}
		}
	}
	
	return result;
	
}


/**
@name: TDM_scheduler_nodal_transmute
@param:

Transmutes individual nodes and qualifies with protovector subject to jitter contraints
**/
int TDM_scheduler_nodal_transmute(unsigned char **vector_map, int freq, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt)
{
	int node_transmuted, node=BLANK;
	
	if (vector_map[col][row]!=_TH_NUM_EXT_PORTS) {
		node_transmuted = BOOL_FALSE;
		/* if (TDM_scheduler_fit_singular_cnt(vector_map,(row-1))>0 && TDM_scheduler_fit_singular_cnt(vector_map,(row+1))==0 && TDM_scheduler_fit_singular_cnt(vector_map,(row+2))==0) { */
		if (TDM_scheduler_fit_singular_cnt(vector_map,(row+1))==0 && TDM_scheduler_fit_singular_cnt(vector_map,(row+2))==0) {
			vector_map[col][row+1]=vector_map[col][row];
			vector_map[col][row]=_TH_NUM_EXT_PORTS;
			node_transmuted = BOOL_TRUE;
			#ifdef _SET_TDM_DEV
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("transmuted [%0d,%0d]=%0d down\n"),
                         col,row,vector_map[col][row+1]));
			#endif
		}
		/* else if (row>1 && TDM_scheduler_fit_singular_cnt(vector_map,(row+1))>0 && TDM_scheduler_fit_singular_cnt(vector_map,(row-1))==0 && TDM_scheduler_fit_singular_cnt(vector_map,(row-2))==0) { */
		else if (row>1 && TDM_scheduler_fit_singular_cnt(vector_map,(row-1))==0 && TDM_scheduler_fit_singular_cnt(vector_map,(row-2))==0) {
			vector_map[col][row-1]=vector_map[col][row];
			vector_map[col][row]=_TH_NUM_EXT_PORTS;
			node_transmuted = BOOL_TRUE;
			#ifdef _SET_TDM_DEV
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("transmuted [%0d,%0d]=%0d up\n"),
                         col,row,vector_map[col][row-1]));
			#endif
		}
		if (node_transmuted == BOOL_TRUE) {
			node = row;
			if (TDM_scheduler_vector_diff(vector_map,freq,lr_idx_limit,col,speed,tsc,traffic)!=PASS || TDM_scheduler_fit_prox(vector_map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) {
				if (vector_map[col][row-1]!=_TH_NUM_EXT_PORTS) {
					vector_map[col][row]=vector_map[col][row-1];
					vector_map[col][row-1]=_TH_NUM_EXT_PORTS;
				}
				else if (vector_map[col][row+1]!=_TH_NUM_EXT_PORTS) {
					vector_map[col][row]=vector_map[col][row+1];
					vector_map[col][row+1]=_TH_NUM_EXT_PORTS;
				}
				#ifdef _SET_TDM_DEV
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("reverted illegal transmute at [%0d,%0d]=%0d\n"),
                             col,row,vector_map[col][row]));
				#endif
			}
		}
	}
	
	return node;
	
}


/**
@name: TDM_scheduler_fit_distal_1
@param: unsigned char **, int, int

Given x index, determines fit based on if nodes of the current vector are locationed at a distal maximum from nodes of other vectors
*** Returns the x_pos2 coordinate of recommended transpose ***
*** This only matters if oversub is enabled, to increase performance of oversub arbiter ***
**/
int TDM_scheduler_fit_distal_pos1(unsigned char **map, int lim, int node_x, int lr_slot_cnt) {
	int i, j, v, n;
	/* unsigned char ovs_no_clump_idx; */
	
	/* Transform vectors to avoid large clusters of OVS tokens once vectors are compressed */
	v=0; n=_TH_NUM_EXT_PORTS;
	for (j=0; j<lim; j++) {
		if ((v>((lim/lr_slot_cnt)+1))) {
			/* Find nearest vectored oversub node as reference, we cannot use a null slot */
			for (i=0; i<v; i++) {
				if (TDM_scheduler_map_search_x(map,(j-i))==OVSB_TOKEN) {
					n=(j-i);
					break;
				}
			}
			if ((TDM_scheduler_map_search_x(map,n)!=_TH_NUM_EXT_PORTS)) {
				break;
			}
		}
		if ((TDM_scheduler_map_search_x(map,j)==OVSB_TOKEN||TDM_scheduler_map_search_x(map,j)==_TH_NUM_EXT_PORTS) &&
			(TDM_scheduler_map_search_x(map,(j+1))==OVSB_TOKEN||TDM_scheduler_map_search_x(map,(j+1))==_TH_NUM_EXT_PORTS)) {
			v++;
		}
		else {
			v=0;
		}
	}
	
	return n;
}


/**
@name: TDM_scheduler_fit_distal_2
@param: unsigned char **, int, int

Given x index, determines fit based on if nodes of the current vector are locationed at a distal maximum from nodes of other vectors
*** Returns the x_pos1 coordinate of recommended transpose ***
*** This only matters if oversub is enabled, to increase performance of oversub arbiter ***
**/
int TDM_scheduler_fit_distal_pos2(unsigned char **map, int lim, int node_x, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int os_param_spd) {
	int j, n=0, prox1d=0, prox2d=0, prox3d=0, prox1u=0, prox2u=0, prox3u=0;
	unsigned char ovs_no_clump_idx=1;
	
	/*if (1) {*/
		if (TDM_scheduler_map_find_x(map,lim,(n+1),0)!=_TH_NUM_EXT_PORTS) {
			if (TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n+1),0),0)!=_TH_NUM_EXT_PORTS) {
				prox1d=which_tsc(map[TDM_scheduler_map_find_x(map,lim,(n+1),0)][TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n+1),0),0)],tsc);
			}
			else {
				prox1d=_TH_NUM_EXT_PORTS;
			}
		}
		else {
			prox1d=_TH_NUM_EXT_PORTS;
		}
		if (TDM_scheduler_map_find_x(map,lim,(n+2),0)!=_TH_NUM_EXT_PORTS) {
			if (TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n+2),0),0)!=_TH_NUM_EXT_PORTS) {
				prox2d=which_tsc(map[TDM_scheduler_map_find_x(map,lim,(n+2),0)][TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n+2),0),0)],tsc);
			}
			else {
				prox2d=_TH_NUM_EXT_PORTS;
			}
		}
		else {
			prox2d=_TH_NUM_EXT_PORTS;
		}
		if (TDM_scheduler_map_find_x(map,lim,(n+3),0)!=_TH_NUM_EXT_PORTS) {
			if (TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n+3),0),0)!=_TH_NUM_EXT_PORTS) {
				prox3d=which_tsc(map[TDM_scheduler_map_find_x(map,lim,(n+3),0)][TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n+3),0),0)],tsc);
			}
			else {
				prox3d=_TH_NUM_EXT_PORTS;
			}
		}
		else {
			prox3d=_TH_NUM_EXT_PORTS;
		}
		if (TDM_scheduler_map_find_x(map,lim,(n-1),0)!=_TH_NUM_EXT_PORTS) {
			if (TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n-1),0),0)!=_TH_NUM_EXT_PORTS) {
				prox1u=which_tsc(map[TDM_scheduler_map_find_x(map,lim,(n-1),0)][TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n-1),0),0)],tsc);
			}
			else {
				prox1u=_TH_NUM_EXT_PORTS;
			}
		}
		else {
			prox1u=_TH_NUM_EXT_PORTS;
		}
		if (TDM_scheduler_map_find_x(map,lim,(n-2),0)!=_TH_NUM_EXT_PORTS) {
			if (TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n-2),0),0)!=_TH_NUM_EXT_PORTS) {
				prox2u=which_tsc(map[TDM_scheduler_map_find_x(map,lim,(n-2),0)][TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n-2),0),0)],tsc);
			}
			else {
				prox2u=_TH_NUM_EXT_PORTS;
			}
		}
		else {
			prox2u=_TH_NUM_EXT_PORTS;
		}
		if (TDM_scheduler_map_find_x(map,lim,(n-3),0)!=_TH_NUM_EXT_PORTS) {
			if (TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n-3),0),0)!=_TH_NUM_EXT_PORTS) {
				prox3u=which_tsc(map[TDM_scheduler_map_find_x(map,lim,(n-3),0)][TDM_scheduler_map_find_y(map,lim,TDM_scheduler_map_find_x(map,lim,(n-3),0),0)],tsc);
			}
			else {
				prox3u=_TH_NUM_EXT_PORTS;
			}
		}
		else {
			prox3u=_TH_NUM_EXT_PORTS;
		}
		ovs_no_clump_idx=_TH_NUM_EXT_PORTS;
		for (j=1; j<VEC_MAP_WID; j++) {
			if (TDM_scheduler_map_find_y(map,lim,j,0)!=_TH_NUM_EXT_PORTS) {
				/* Only one type of 25G vector per core bandwidth */
				if (((speed[map[j][TDM_scheduler_map_find_y(map,lim,j,0)]])/1000)==25) {
					if ((((speed[map[j][TDM_scheduler_map_find_y(map,lim,j,0)]])/1000)==os_param_spd) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox1d) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox2d) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox3d) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox1u) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox2u) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox3u)) {
							ovs_no_clump_idx=j;
							break;
					}
				}
				/* Byproduct of vector labeling scheme */
				else {
					if ((((speed[map[j][TDM_scheduler_map_find_y(map,lim,j,0)]])/10000)==(os_param_spd/10)) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox1d) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox2d) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox3d) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox1u) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox2u) &&
						(which_tsc(map[j][TDM_scheduler_map_find_y(map,lim,j,0)],tsc)!=prox3u)) {
							ovs_no_clump_idx=j;
							break;
					}
				}
			}
		}
	/*}*/
	
	return ovs_no_clump_idx;
}

/**
@name: TDM_scheduler_filter_dither
@param:

**/
void TDM_scheduler_filter_dither(int tdm_tbl[256], int lr_idx_limit, int accessories, int tsc[NUM_TSC][4], int threshold)
{
	int g, i, j, k=1, l=_TH_NUM_EXT_PORTS;
	unsigned char dither_shift_done=BOOL_FALSE, dither_done=BOOL_FALSE, dither_slice_counter=0;

	dither_shift_done=BOOL_FALSE;
	for (i=0; i<(lr_idx_limit+accessories); i++) {
		if (tdm_tbl[i]==OVSB_TOKEN && TDM_scheduler_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories))==TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))) {
			g=i; while( (tdm_tbl[g]==OVSB_TOKEN) && (g<(lr_idx_limit+accessories)) ) {g++;}
			if ( TDM_scheduler_slice_prox_up(g,tdm_tbl,tsc) &&
				 TDM_scheduler_slice_prox_dn((TDM_scheduler_slice_idx(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))-1),tdm_tbl,(lr_idx_limit+accessories),tsc) ) {
				l=tdm_tbl[i];
				for (j=i; j<255; j++) {
					tdm_tbl[j]=tdm_tbl[j+1];
				}
				k=i; dither_shift_done=BOOL_TRUE;
				break;
			}
		}
	}
	dither_done=BOOL_FALSE;
	if (dither_shift_done) {
		for (i=1; i<(lr_idx_limit+accessories); i++) {
			dither_slice_counter=0;
			while (tdm_tbl[i]!=OVSB_TOKEN && TDM_scheduler_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories))==TDM_scheduler_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && i<(lr_idx_limit+accessories) ) {
				if (++dither_slice_counter>=threshold && tdm_tbl[i-1]==OVSB_TOKEN) {
					for (j=254; j>i; j--) {
						tdm_tbl[j]=tdm_tbl[j-1];
					}
					tdm_tbl[i+1]=OVSB_TOKEN;
					dither_done=BOOL_TRUE;
					break;
				}
				/* do {i++;} while (tdm_tbl[i]==OVSB_TOKEN && i<(lr_idx_limit+accessories)); */
				i++; if (tdm_tbl[i]==OVSB_TOKEN) {i++;}
			}
			if (dither_done) {
				break;
			}
		}
		if (!dither_done) {
			for (j=255; j>k; j--) {
				tdm_tbl[j]=tdm_tbl[j-1];
			}
			tdm_tbl[k]=l;
		}
		else {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: _____VERBOSE: Filter applied: Dither (quantization correction)\n")));
		}
	}
}


/**
@name: TDM_scheduler_filter_fine_dither_normal
@param:

SINGLE PASS PER CALL
**/
int TDM_scheduler_filter_fine_dither(int port, int tdm_tbl[256], int lr_idx_limit, int accessories, int tsc[NUM_TSC][4])
{
	int i, j, k;
	unsigned char fine_dither_done;

	i=port; fine_dither_done=BOOL_FALSE;
	for (j=2; j<(lr_idx_limit+accessories-4); j++) {
		if ( tdm_tbl[j]!=OVSB_TOKEN && tdm_tbl[j-2]!=OVSB_TOKEN && tdm_tbl[j+2]!=OVSB_TOKEN && tdm_tbl[j+4]!=OVSB_TOKEN && 
			 TDM_scheduler_slice_size_local((j-2), tdm_tbl, (lr_idx_limit+accessories))==1 && 
			 TDM_scheduler_slice_size_local(j, tdm_tbl, (lr_idx_limit+accessories))==1 && 
			 TDM_scheduler_slice_size_local((j+2), tdm_tbl, (lr_idx_limit+accessories))==1 && 
			 TDM_scheduler_slice_size_local((j+4), tdm_tbl, (lr_idx_limit+accessories))==1 &&
			 TDM_scheduler_slice_prox_local(j, tdm_tbl, (lr_idx_limit+accessories), tsc)>4 )
		{
			/* Destructively shift slice */
			for (k=(j+1); k<(lr_idx_limit+accessories); k++) {
				tdm_tbl[k]=tdm_tbl[k+1];
			}
			fine_dither_done=BOOL_TRUE;
			break;
		}
	}
	if (fine_dither_done) {
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Filter applied: Fine dithering (normal), index %0d\n"),
                     port));
		/* Split new slice at original index */
		for (j=255; j>i; j--) {
			tdm_tbl[j]=tdm_tbl[j-1];
		}
		tdm_tbl[i]=OVSB_TOKEN;
		
		return PASS;
	}
	else {
		return FAIL;
	}
}


/**
@name: TDM_scheduler_filter_ancillary_smooth
@param:

SINGLE PASS PER CALL
**/
void TDM_scheduler_filter_ancillary_smooth(int port, int tdm_tbl[256], int lr_idx_limit, int accessories)
{
	int i, j, k;
	unsigned char ancillary_slice_counter;
	
	i=port; ancillary_slice_counter=0;
	for (j=1; (i-j)>0; j++) {
		TOKEN_CHECK(tdm_tbl[i-j]) {
			if (TDM_scheduler_slice_size_local((i-j),tdm_tbl,(lr_idx_limit+accessories))==1) {
				/* Prevent cumulative sop to mop cycle delay */
				if (++ancillary_slice_counter==2) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: Filter applied: Smooth ancillary slots, index %0d\n"),
                                 port));
					/* Destructively shift slice with ancillary token */
					for (k=i; k<(lr_idx_limit+accessories); k++) {
						tdm_tbl[k]=tdm_tbl[k+1];
					}
					/* Shift new slice to insert ancillary token */
					for (k=(lr_idx_limit+accessories); k>(i-j); k--) {
						tdm_tbl[k]=tdm_tbl[k-1];
					}
					tdm_tbl[i-j]=ACC_TOKEN;
				}
			}
		}
	}
}


/**
@name: TDM_scheduler_filter_refactor
@param:

USE ONLY IF ALL SINGLE MODE PORTS
**/
void TDM_scheduler_filter_refactor(int tdm_tbl[256], int lr_idx_limit, int accessories, int lr_vec_cnt)
{
	int g, i, j, l;
	unsigned char os_re_pool=0, os_re_tdm_len=0, os_re_spacing=0, os_re_spacing_last=0;

	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("TDM: _____VERBOSE: Filter applied: Refactor table list\n")));
	for (i=0; i<255; i++) {
		if (tdm_tbl[i]==OVSB_TOKEN) {
			++os_re_pool;
			/* Destructively shift slice */
			for (j=i; j<255; j++) {
				tdm_tbl[j]=tdm_tbl[j+1];
			}
			i--;
		}
	}
	for (i=255; i>=0; i--) {
		if (tdm_tbl[i]!=_TH_NUM_EXT_PORTS) {
			os_re_tdm_len=i;
			break;
		}
	}
	os_re_spacing=(((os_re_tdm_len+os_re_pool)/os_re_pool)>=lr_vec_cnt)?((os_re_tdm_len+os_re_pool)/os_re_pool):(lr_vec_cnt);		
	for (j=1; j<=os_re_pool; j++) {
		g=(os_re_spacing*j)+(j-1);
		if (j>((os_re_tdm_len+os_re_pool)%os_re_pool)) {
			g=(os_re_spacing_last+4);
		}
		for (l=255; l>g; l--) {
			tdm_tbl[l]=tdm_tbl[l-1];
		}
		tdm_tbl[g]=OVSB_TOKEN;
		os_re_spacing_last=g;
	}
	
}


/**
@name: TDM_scheduler_filter_local_slice
@param:

SINGLE PASS PER CALL
**/
void TDM_scheduler_filter_local_slice_dn(int port, int tdm_tbl[256], int tsc[NUM_TSC][4])
{
	int i, j, pp_prox;

	i=port; pp_prox=PASS;
	if (tdm_tbl[i]<129 && tdm_tbl[i]>0) {
		for (j=1; j<4; j++) {
			if (which_tsc(tdm_tbl[i+1+j],tsc)==which_tsc(tdm_tbl[i],tsc)) {
				pp_prox=FAIL;
				break;
			}
		}
	}
	if (pp_prox) {
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Filter applied: Local Slice, Down, index %0d\n"),
                     port));
		tdm_tbl[i+1]=tdm_tbl[i];
		tdm_tbl[i]=OVSB_TOKEN;
	}

}


/**
@name: TDM_scheduler_filter_local_slice
@param:

SINGLE PASS PER CALL
**/
void TDM_scheduler_filter_local_slice_up(int port, int tdm_tbl[256], int tsc[NUM_TSC][4])
{
	int i, j, pp_prox;

	i=port; pp_prox=PASS;
	if (tdm_tbl[i]<129 && tdm_tbl[i]>0) {
		for (j=1; j<4; j++) {
			if (which_tsc(tdm_tbl[i-1-j],tsc)==which_tsc(tdm_tbl[i],tsc)) {
				pp_prox=FAIL;
				break;
			}
		}
	}
	if (pp_prox) {
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Filter applied: Local Slice, Up, index %0d\n"),
                     port));
		tdm_tbl[i-1]=tdm_tbl[i];
		tdm_tbl[i]=OVSB_TOKEN;
	}

}


/**
@name: TDM_scheduler_filter_local_slice_oversub
@param:

**/
void TDM_scheduler_filter_local_slice_oversub(int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4])
{
	int g, i, last_g=0;
	
	for (i=0; i<lr_idx_limit; i++) {
		if ( (TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,lr_idx_limit)>(TDM_scheduler_slice_size(1,tdm_tbl,lr_idx_limit)+1)) ) {
			g=TDM_scheduler_slice_idx(OVSB_TOKEN,tdm_tbl,lr_idx_limit);
			if (TDM_scheduler_slice_prox_dn((g-1),tdm_tbl,lr_idx_limit,tsc)) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: Filter applied: Local slice oversub index %0d\n"),
                             g));
				tdm_tbl[g]=tdm_tbl[g-1];
				tdm_tbl[g-1]=OVSB_TOKEN;
			}
		}
	}
	for (i=0; i<lr_idx_limit; i++) {
		if ( (TDM_scheduler_slice_size(OVSB_TOKEN,tdm_tbl,lr_idx_limit)>(TDM_scheduler_slice_size(1,tdm_tbl,lr_idx_limit)+1)) ) {
			g=TDM_scheduler_slice_idx(OVSB_TOKEN,tdm_tbl,lr_idx_limit);
			if (tdm_tbl[g-1]!=OVSB_TOKEN) {
				continue;
			}
			else if (TDM_scheduler_slice_prox_up(g,tdm_tbl,tsc)) {
				if (g!=last_g) {
					LOG_VERBOSE(BSL_LS_SOC_TDM,
                                (BSL_META("TDM: _____VERBOSE: Filter applied: Local slice oversub index %0d\n"),
                                 g));
					tdm_tbl[g-1]=tdm_tbl[g];
					tdm_tbl[g]=OVSB_TOKEN;
					last_g=g;
				}
			}
		}
	}

}


/**
@name: TDM_scheduler_filter_slice_dn
@param:

**/
int TDM_scheduler_filter_slice_dn(int port, int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4])
{
	int i, j, k, slice_translation_done;
	
	i=port; slice_translation_done=BOOL_FALSE;
	for (j=3; (i+j)<(lr_idx_limit-1); j++) {
		if (tdm_tbl[i+j]==OVSB_TOKEN && 
			tdm_tbl[i+1+j]==OVSB_TOKEN &&
			(tdm_tbl[i+j-1]==OVSB_TOKEN || (which_tsc(tdm_tbl[i+j-1],tsc)!=which_tsc(tdm_tbl[i+j+3],tsc))) &&
			(tdm_tbl[i+j-2]==OVSB_TOKEN || (which_tsc(tdm_tbl[i+j-2],tsc)!=which_tsc(tdm_tbl[i+j+2],tsc)))) {
			for (k=(i+j); k>i; k--) {
				tdm_tbl[k]=tdm_tbl[k-1];
				slice_translation_done=BOOL_TRUE;
			}
			tdm_tbl[i]=OVSB_TOKEN;
		}
		if (slice_translation_done) {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: _____VERBOSE: Filter applied: Slice translation, Down, index %0d\n"),
                         port));
			i++;
			break;
		}
	}
	
	return i;

}


/**
@name: TDM_scheduler_filter_slice_up
@param:

**/
int TDM_scheduler_filter_slice_up(int port, int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4])
{
	int i, j, k, slice_translation_done;
	
	i=port; slice_translation_done=BOOL_FALSE;
	for (j=3; j<(lr_idx_limit-1); j++) {
		if ((i-j-3)>=0) {
			if (tdm_tbl[i-j]==OVSB_TOKEN && 
				tdm_tbl[i-1-j]==OVSB_TOKEN &&
				(tdm_tbl[i-j+1]==OVSB_TOKEN || (which_tsc(tdm_tbl[i-j+1],tsc)!=which_tsc(tdm_tbl[i-j-3],tsc))) &&
				(tdm_tbl[i-j+2]==OVSB_TOKEN || (which_tsc(tdm_tbl[i-j+2],tsc)!=which_tsc(tdm_tbl[i-j-2],tsc)))) {
				for (k=(i-j); k<i; k++) {
					tdm_tbl[k]=tdm_tbl[k+1];
					slice_translation_done=BOOL_TRUE;
				}
				tdm_tbl[i]=OVSB_TOKEN;
			}
			if (slice_translation_done) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: _____VERBOSE: Filter applied: Slice translation, Up, index %0d\n"),
                             port));
				i--;
				break;
			}
		}
	}
	
	return i;

}


/**
@name: TDM_scheduler_vector_dump
@param: int[][]

Dumps current content of TDM vector map
**/
void TDM_scheduler_vector_dump(FILE *file, unsigned char **map, int idx)
{
#ifdef _SET_TDM_DEV
	int j, v;
	
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n")));
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("           ")));
	for (v=0; v<VEC_MAP_WID; v++) {
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("%d"), v));
		if ((v+1)<VEC_MAP_WID) {
			if ((v+1)/10==0) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("___")));
			}
			else {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("__")));
			}
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n")));
	for (v=0; v<idx; v++) {
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("\n %d___\t"), v));
		for (j=0; j<VEC_MAP_WID; j++) {
			if (map[j][v]!=_TH_NUM_EXT_PORTS) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %03d"), map[j][v]));
			}
			else {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" ---")));
			}
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n")));
#endif
}


/**
@name: print_tsc
@param: int[][]

Prints summary of all warp cores and their associated subports
 **/
void print_tsc(FILE *file, int wc_array[NUM_TSC][4])
{
	int i,j;
	
  	for(i=0; i<NUM_TSC; i++) {
      	for (j=0; j<4; j++) {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("Warpcore #%0d, subport #%0d, contains physical port #%0d\n"),
                         i, j, wc_array[i][j]));
      	}
	}
	
}


/**
@name: TH_print_tdm_tbl
@param: int[32], const char*

Prints debug form summary of quadrant's table
**/
void TH_print_tdm_tbl(FILE *file, int pgw_tdm_tbl[32], const char* name)
{
	int j;
	
	for (j = 0; j < 32; j++)
	{
		switch (pgw_tdm_tbl[j])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: PIPE: %s, TDM Calendar, element #%0d, contains an invalid or disabled port\n"),
                             name, j));
				break;
			case 131:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: PIPE: %s, TDM Calendar, element #%0d, contains an oversubscription token\n"),
                             name, j));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("TDM: PIPE: %s, TDM Calendar, element #%0d, contains physical port #%0d\n"),
                             name, j, pgw_tdm_tbl[j]));
				break;
		}
	}
}


/**
@name: TH_print_tbl_summary
@param: int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int

Prints short form final summary of all tables generated
**/
void TH_print_tbl_summary(FILE *file, int x0[32], int x1[32], int y0[32], int y1[32], int ox0[32], int ox1[32], int oy0[32], int oy1[32], int sx0[32], int sx1[32], int sy0[32], int sy1[32], int bw)
{
	int t;
	
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("TDM: _____VERBOSE: TDM: core bandwidth is %0d\n"),
                 bw));
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("TDM: X0, TDM Calendar\n")));
	for (t = 0; t < 32; t++)
	{
		switch (x0[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			case 131:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" O")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), x0[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: X0, OVS Calendar\n")));
	for (t = 0; t < 32; t++)
	{
		switch (ox0[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), ox0[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: X0, OVS Spacing Info\n")));
	for (t = 0; t < 32; t++)
	{
		switch (sx0[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), sx0[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: X1, TDM Calendar\n")));
	for (t = 0; t < 32; t++)
	{
		switch (x1[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			case 131:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" O")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), x1[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: X1, OVS Calendar\n")));
	for (t = 0; t < 32; t++)
	{
		switch (ox1[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), ox1[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: X1, OVS Spacing Info\n")));
	for (t = 0; t < 32; t++)
	{
		switch (sx1[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), sx1[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: Y0, TDM Calendar\n")));
	for (t = 0; t < 32; t++)
	{
		switch (y0[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			case 131:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" O")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), y0[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: Y0, OVS Calendar\n")));
	for (t = 0; t < 32; t++)
	{
		switch (oy0[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), oy0[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: Y0, OVS Spacing Info\n")));
	for (t = 0; t < 32; t++)
	{
		switch (sy0[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), sy0[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: Y1, TDM Calendar\n")));
	for (t = 0; t < 32; t++)
	{
		switch (y1[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			case 131:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" O")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), y1[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: Y1, OVS Calendar\n")));
	for (t = 0; t < 32; t++)
	{
		switch (oy1[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), oy1[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\nTDM: Y1, OVS Spacing Info\n")));
	for (t = 0; t < 32; t++)
	{
		switch (sy1[t])
		{
			case 130:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" x")));
				break;
			default:
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META(" %0d"), sy1[t]));
				break;
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n")));
}


/**
@name: which_tsc
@param: int

Returns the TSC to which the input port belongs given pointer to transcribed TSC matrix
**/
int which_tsc(unsigned char port, int tsc[NUM_TSC][4]) {

	int i, j, which=_TH_NUM_EXT_PORTS;
		
	for (i=0; i<NUM_TSC; i++) {
		for (j=0; j<4; j++) {
			if (tsc[i][j]==port) {
				which=i;
			}
		}
		if (which!=_TH_NUM_EXT_PORTS) {
			break;
		}
	}
	
	return which;
	
}


/**
@name: check_mode
@param: unsigned char, int[][]

Returns how many lanes are active in the TSC of the input port
**/
int check_mode(unsigned char port, int tsc[NUM_TSC][4])
{
	int i,j=0;

	for (i=0; i<4; i++) {
		if (tsc[which_tsc(port,tsc)][i]!=_TH_NUM_EXT_PORTS) {
			j++;
		}
	}
	
	return j;

}


/**
@name: check_type_2
@param: unsigned char, int[][]

Complex returns port type (unique lanes) in the TSC of the input port
**/
int check_type_2(unsigned char port, int tsc[NUM_TSC][4])
{
	int i, j, tmp=_TH_NUM_EXT_PORTS, cnt=1, tsc_arr[4], tsc_inst=which_tsc(port,tsc);
	
	tsc_arr[0]=tsc[tsc_inst][0];
	tsc_arr[1]=tsc[tsc_inst][1];
	tsc_arr[2]=tsc[tsc_inst][2];
	tsc_arr[3]=tsc[tsc_inst][3];
	/* Bubble sort array into ascending order */
	for (i=0; i<4; i++) {
		for (j=0; j<4-i; j++) {
			if ((j+1)<4) {
				if (tsc_arr[j] > tsc_arr[j+1]) {
					tmp=tsc_arr[j];
					tsc_arr[j]=tsc_arr[j+1];
					tsc_arr[j+1]=tmp;
				}
			}
		}
	}
	/* Count transitions */
	for (i=1; i<4; i++) {
		if (tsc_arr[i]!=_TH_NUM_EXT_PORTS && tsc_arr[i]!=tsc_arr[i-1]) {
			cnt++;
		}
	}
	
	return cnt;

}


/**
@name: check_ethernet
@param: int, enum[], int[][], int[]

Returns BOOL_TRUE or BOOL_FALSE depending on if pipe of the given port has traffic entirely Ethernet.
**/
int check_ethernet(int port, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1]) {

	int i, type=BOOL_TRUE;
	
	if (port<33) {
		for (i=1; i<33; i++) {
			if (speed[i]!=SPEED_0 && traffic[which_tsc(i,tsc)]!=PORT_ETHERNET) {
				type=BOOL_FALSE;
				break;
			}
		}
	}
	else if (port<65) {
		for (i=33; i<65; i++) {
			if (speed[i]!=SPEED_0 && traffic[which_tsc(i,tsc)]!=PORT_ETHERNET) {
				type=BOOL_FALSE;
				break;
			}
		}
	}
	else if (port<97) {
		for (i=65; i<97; i++) {
			if (speed[i]!=SPEED_0 && traffic[which_tsc(i,tsc)]!=PORT_ETHERNET) {
				type=BOOL_FALSE;
				break;
			}
		}	
	}
	else {
		for (i=97; i<129; i++) {
			if (speed[i]!=SPEED_0 && traffic[which_tsc(i,tsc)]!=PORT_ETHERNET) {
				type=BOOL_FALSE;
				break;
			}
		}	
	}

	return type;
	
}


/**
@name: check_spd
@param: unsigned char **, int, int

Returns parameterized speed of a vector on the vector map by counting the number of nodes
**/
int check_spd(unsigned char **map, int lim, int x_pos, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1])
{
	int k=0, v, param_spd=_TH_NUM_EXT_PORTS, y_pos=0;
	
	for (v=0; v<lim; v++) {
		if (map[x_pos][v]!=_TH_NUM_EXT_PORTS) {
			y_pos = (y_pos==0) ? (v):(y_pos);
			k++;
		}
	}
	switch(k) {
		case 4: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 10;} else {param_spd = 11;} break;
		case 8: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 20;} else {param_spd = 21;} break;
		case 10: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 25;} else {param_spd = 27;} break;
		case 16: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 40;} else {param_spd = 42;} break;
		case 20: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 50;} else {param_spd = 53;} break;
		case 39: param_spd = 107; break;
		case 40: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 100;} else {param_spd = 106;} break;
	}
	
	return param_spd;

}


/**
@name: TDM_scheduler_slice_size_local
@param:

Given index, returns SIZE of LARGEST slice contiguous to given index
**/
int TDM_scheduler_slice_size_local(unsigned char idx, int tdm[256], int lim)
{
	int i, slice_size=(-1);
	
	if (tdm[idx]!=OVSB_TOKEN && tdm[idx]!=_TH_NUM_EXT_PORTS) {
		for (i=idx; i>=0; i--) {
			if (tdm[i]!=OVSB_TOKEN && tdm[i]!=_TH_NUM_EXT_PORTS) {
				slice_size++;
			}
			else {
				break;
			}
		}
		for (i=idx; i<lim; i++) {
			if (tdm[i]!=OVSB_TOKEN && tdm[i]!=_TH_NUM_EXT_PORTS) {
				slice_size++;
			}
			else {
				break;
			}
		}
	}
	else if (tdm[idx]==OVSB_TOKEN) {
		for (i=idx; i>=0; i--) {
			if (tdm[i]==OVSB_TOKEN) {
				slice_size++;
			}
			else {
				break;
			}
		}
		for (i=idx; i<lim; i++) {
			if (tdm[i]==OVSB_TOKEN) {
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
@name: TDM_scheduler_slice_prox_local
@param:

Given index, checks min spacing of two nearest non-token ports
**/
int TDM_scheduler_slice_prox_local(unsigned char idx, int tdm[256], int lim, int tsc[NUM_TSC][4])
{
	int i, prox_len=0, wc=_TH_NUM_EXT_PORTS;
	
	/* Nearest non-token port */
	TOKEN_CHECK(tdm[idx]) {
		wc=which_tsc(tdm[idx],tsc);
	}
	else {
		for (i=1; (idx-i)>=0; i++) {
			TOKEN_CHECK(tdm[i]) {
				wc=which_tsc(tdm[idx-i],tsc);
				break;
			}
		}
	}
	for (i=1; (idx+i)<lim; i++) {
		if (which_tsc(tdm[idx+i],tsc)!=wc) {
			prox_len++;
		}
		else {
			break;
		}
	}

	return prox_len;

}


/**
@name: TDM_scheduler_slice_size
@param:

Given port number, returns SIZE of LARGEST slice
**/
int TDM_scheduler_slice_size(unsigned char port, int tdm[256], int lim)
{
	int i, j, k=0, slice_size=0;
	
	if (port<129 && port>0) {
		for (i=0; i<lim; i++) {
			TOKEN_CHECK(tdm[i]) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					TOKEN_CHECK(tdm[j]) {k++;}
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
@name: TDM_scheduler_slice_idx
@param:

Given port number, returns INDEX of LARGEST slice
**/
int TDM_scheduler_slice_idx(unsigned char port, int tdm[256], int lim)
{
	int i, j, k=0, slice_size=0, slice_idx=0;
	
	if (port<129 && port>0) {
		for (i=0; i<lim; i++) {
			TOKEN_CHECK(tdm[i]) {
				k=1;
				for (j=(i+1); j<lim; j++) {
					TOKEN_CHECK(tdm[j]) {k++;}
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
@name: TDM_scheduler_slice_prox_dn
@param:

Given port number, checks min spacing in a slice in down direction
**/
int TDM_scheduler_slice_prox_dn(int slot, int tdm[256], int lim, int tsc[NUM_TSC][4])
{
	int i, cnt=0, wc, idx=(slot+1), slice_prox=PASS;
	
	wc=(tdm[slot]==ACC_TOKEN)?(tdm[slot]):(which_tsc(tdm[slot],tsc));
	if (slot<=(lim-5)) {
		if ( wc==which_tsc(tdm[slot+1],tsc) ||
		     wc==which_tsc(tdm[slot+2],tsc) ||
		     wc==which_tsc(tdm[slot+3],tsc) ||
			 wc==which_tsc(tdm[slot+4],tsc) ) {
			slice_prox=FAIL;
		}
	}
	else {
		while (idx<lim) {
			if (wc==which_tsc(tdm[idx],tsc)) {
				slice_prox=FAIL;
				break;
			}
			idx++; cnt++;
		}
		for (i=(lim-slot-cnt-1); i>=0; i--) {
			if (wc==which_tsc(tdm[i],tsc)) {
				slice_prox=FAIL;
				break;
			}
		}
	}
	
	return slice_prox;

}


/**
@name: TDM_scheduler_slice_prox_up
@param:

Given port number, checks min spacing in a slice in up direction
**/
int TDM_scheduler_slice_prox_up(int slot, int tdm[256], int tsc[NUM_TSC][4])
{
	int wc, slice_prox=PASS;
	
	wc=(tdm[slot]==ACC_TOKEN)?(tdm[slot]):(which_tsc(tdm[slot],tsc));
	if (slot>=4) {
		if ( wc==which_tsc(tdm[slot-1],tsc) ||
		     wc==which_tsc(tdm[slot-2],tsc) ||
		     wc==which_tsc(tdm[slot-3],tsc) ||
			 wc==which_tsc(tdm[slot-4],tsc) ) {
			slice_prox=FAIL;
		}
	}
	
	return slice_prox;

}


/**
@name: TDM_scheduler_table_scan
@param: int, int

Scans table for next valid slot
**/
int TDM_scheduler_table_scan(FILE *file, unsigned short yy, unsigned short spacing, int limit, int speed, unsigned char stack[64], unsigned short stack_pointer, int tsc[NUM_TSC][4], int tdm_tbl[256]) 
{
	int j, prox_chk, vec_cnt;
	unsigned short timeout;
	
	timeout=TIMEOUT;
	do {
		prox_chk=PASS;
		TDM_SML_BAR
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: Scanning table pointer %0d\n"),
                     yy));
		if (yy>=3 && yy<252) {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("[%0d] | (%0d)(%0d)(%0d)[[%0d]](%0d)(%0d)(%0d)\n"),
                         which_tsc(stack[stack_pointer],tsc), which_tsc(tdm_tbl[yy-3],tsc),which_tsc(tdm_tbl[yy-2],tsc),which_tsc(tdm_tbl[yy-1],tsc),which_tsc(tdm_tbl[yy],tsc),which_tsc(tdm_tbl[yy+1],tsc),which_tsc(tdm_tbl[yy+2],tsc),which_tsc(tdm_tbl[yy+3],tsc)));		
		}
		if ((which_tsc(stack[stack_pointer],tsc)==which_tsc(tdm_tbl[yy],tsc)) ||
			 (which_tsc(stack[stack_pointer],tsc)==which_tsc(tdm_tbl[yy-1],tsc)) ||
			 (which_tsc(stack[stack_pointer],tsc)==which_tsc(tdm_tbl[yy-2],tsc)) ||
			 (which_tsc(stack[stack_pointer],tsc)==which_tsc(tdm_tbl[yy-3],tsc)) ||
			 (which_tsc(stack[stack_pointer],tsc)==which_tsc(tdm_tbl[yy+1],tsc)) ||
			 (which_tsc(stack[stack_pointer],tsc)==which_tsc(tdm_tbl[yy+2],tsc)) ||
			 (which_tsc(stack[stack_pointer],tsc)==which_tsc(tdm_tbl[yy+3],tsc))) {
			prox_chk=FAIL;
		}
		vec_cnt=0;
		for (j=yy; j<yy+(TDM_scheduler_slots(speed)*spacing); j+=spacing) {
			if (tdm_tbl[j]==_TH_NUM_EXT_PORTS) {
				vec_cnt++;
			}
		}
		if (vec_cnt!=TDM_scheduler_slots(speed) || prox_chk==FAIL) {
			yy++;
		}
		if ((yy+(spacing*(TDM_scheduler_slots(speed)-1))) >= limit) {
			TDM_SML_BAR
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("could not schedule %0dG vector for port %0d, breaking table pointer scan\n"),
                         speed, stack[stack_pointer]));
			break;
		}
	} while (yy<256 && (vec_cnt!=TDM_scheduler_slots(speed) || prox_chk==FAIL) && ((--timeout)>0));
	
	return yy;
}


/**
@name: TDM_scheduler_map_search_x
@param: unsigned char **, int, int

Scans vector map x axis for singular node
**/
int TDM_scheduler_map_search_x(unsigned char **map, int idx) 
{
	int i, id;
	
	id = _TH_NUM_EXT_PORTS;
	for (i=0; i<VEC_MAP_WID; i++) {
		if (map[i][idx]!=_TH_NUM_EXT_PORTS) {
			id = map[i][idx];
			break;
		}
	}
	
	return id;
	
}


/**
@name: TDM_scheduler_map_search_y
@param: unsigned char **, int, int

Scans vector map y axis for singular node
**/
int TDM_scheduler_map_search_y(unsigned char **map, int limit, int idx) 
{
	int i, id;
	
	id = _TH_NUM_EXT_PORTS;
	if (idx<VEC_MAP_WID) {
		for (i=0; i<limit; i++) {
			if (map[idx][i]!=_TH_NUM_EXT_PORTS) {
				id = map[idx][i];
				break;
			}
		}
	}
	
	return id;
	
}


/**
@name: TDM_scheduler_map_scan_x
@param: unsigned char **, int, int

Scans vector map x axis for X INDEX OF NEXT NON-FEATURE NODE
**/
int TDM_scheduler_map_scan_x(unsigned char **map, int limit, int idx, int principle)
{
	int i, id;
	
	id = _TH_NUM_EXT_PORTS;
	if (principle!=_TH_NUM_EXT_PORTS && idx<limit) {
		for (i=0; i<VEC_MAP_WID; i++) {
			if (map[i][idx]==OVSB_TOKEN && map[i][idx]!=principle) {
				id = i;
				break;
			}
		}
	}
	
	return id;
	
}


/**
@name: TDM_scheduler_map_find_x
@param: unsigned char **, int, int

Scans vector map x axis for X INDEX OF ANY NON-PRINCIPLE NODE
**/
int TDM_scheduler_map_find_x(unsigned char **map, int limit, int idx, int principle)
{
	int i, id;
	
	id = _TH_NUM_EXT_PORTS;
	if (principle!=_TH_NUM_EXT_PORTS && idx<limit) {
		for (i=0; i<VEC_MAP_WID; i++) {
			if (map[i][idx]!=_TH_NUM_EXT_PORTS && map[i][idx]!=principle) {
				id = i;
				break;
			}
		}
	}
	
	return id;
	
}


/**
@name: TDM_scheduler_map_find_y
@param: unsigned char **, int, int

Scans vector map Y axis for Y INDEX OF ANY NON-PRINCIPLE NODE
**/
int TDM_scheduler_map_find_y(unsigned char **map, int limit, int idx, int principle)
{
	int i, id=_TH_NUM_EXT_PORTS;
	
	if (principle!=_TH_NUM_EXT_PORTS && idx<VEC_MAP_WID) {
		for (i=0; i<limit; i++) {
			if (map[idx][i]!=_TH_NUM_EXT_PORTS && map[idx][i]!=principle) {
				id = i;
				break;
			}
		}
	}
	
	return id;
	
}


/**
@name: TDM_scheduler_fit_prox
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port spacing rule
**/
int TDM_scheduler_fit_prox(unsigned char **map, int lim, int wid, int node_x, int tsc[NUM_TSC][4]) {
	int i, v, check=PASS;
	
	for (i=3; i<lim; i++) {
		TOKEN_CHECK(map[node_x][i]) {
			/* Check proximate TSC violation */
			for (v=0; v<wid; v++) {
				if (v!=node_x) {
					if ((which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i+1],tsc)) ||
						(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i+2],tsc)) ||
						(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i+3],tsc)) ||
						(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i],tsc)) ||
						(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i-1],tsc)) ||
						(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i-2],tsc)) ||
						(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i-3],tsc))) 
						{
							check=FAIL;
						}
				}
			}			
		}
	}

	return check;
}


/**
@name: TDM_scheduler_fit_prox_partial
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port spacing rule
**/
int TDM_scheduler_fit_prox_partial(unsigned char **map, int lim, int wid, int node_x, int tsc[NUM_TSC][4]) {
	int i, v, check=PASS;
	
	for (i=0; i<lim; i++) {
		TOKEN_CHECK(map[node_x][i]) {
			/* Check proximate TSC violation */
			for (v=0; v<wid; v++) {
				if ((which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i+1],tsc)) ||
					(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i+2],tsc)) ||
					(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i+3],tsc))) 
					{
						check=FAIL;
					}
			}			
		}
	}

	return check;
}


/**
@name: TDM_scheduler_fit_prox_partial2
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port spacing rule
**/
int TDM_scheduler_fit_prox_partial2(unsigned char **map, int lim, int wid, int node_x, int tsc[NUM_TSC][4]) {
	int i, v, check=PASS;
	
	for (i=3; i<lim; i++) {
		TOKEN_CHECK(map[node_x][i]) {
			/* Check proximate TSC violation */
			for (v=0; v<wid; v++) {
				if ((which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i-1],tsc)) ||
					(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i-2],tsc)) ||
					(which_tsc(map[node_x][i],tsc)==which_tsc(map[v][i-3],tsc))) 
					{
						check=FAIL;
					}
			}			
		}
	}

	return check;
}


/**
@name: TDM_scheduler_fit_prox_node
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port spacing rule
**/
int TDM_scheduler_fit_prox_node(unsigned char **map, int node_y, int wid, int node_x, int tsc[NUM_TSC][4], int new_node_y) {
	int v, check=PASS;
	int look_tsc=which_tsc(map[node_x][node_y],tsc);
	
	TOKEN_CHECK(map[node_x][node_y]) {
		/* Check proximate TSC violation */
		for (v=0; v<wid; v++) {
			if (v!=node_x) {
				if (((which_tsc(map[v][new_node_y+1],tsc))==look_tsc) ||
					((which_tsc(map[v][new_node_y+2],tsc))==look_tsc) ||
					((which_tsc(map[v][new_node_y+3],tsc))==look_tsc) ||
					((which_tsc(map[v][new_node_y],tsc))==look_tsc) ||
					((which_tsc(map[v][new_node_y-1],tsc))==look_tsc) ||
					((which_tsc(map[v][new_node_y-2],tsc))==look_tsc) ||
					((which_tsc(map[v][new_node_y-3],tsc))==look_tsc))
					{
						check=FAIL;
					}
			}
		}			
	}

	return check;
}


/**
@name: TDM_scheduler_fit_prox_node_partial
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port spacing rule
**/
int TDM_scheduler_fit_prox_node_partial(unsigned char **map, int node_y, int wid, int node_x, int tsc[NUM_TSC][4]) {
	int v, check=PASS;
	
	TOKEN_CHECK(map[node_x][node_y]) {
		/* Check proximate TSC violation */
		for (v=0; v<wid; v++) {
			if ((which_tsc(map[node_x][node_y],tsc)==which_tsc(map[v][node_y+1],tsc)) ||
				(which_tsc(map[node_x][node_y],tsc)==which_tsc(map[v][node_y+2],tsc)) ||
				(which_tsc(map[node_x][node_y],tsc)==which_tsc(map[v][node_y+3],tsc))) 
				{
					check=FAIL;
				}
		}			
	}

	return check;
}


/**
@name: TDM_scheduler_fit_singular_cnt
@param:

Given y index, count number of nodes
**/
int TDM_scheduler_fit_singular_cnt(unsigned char **map, int node_y) {
	int v, cnt=0;
	
	for (v=0; v<VEC_MAP_WID; v++) {
		TOKEN_CHECK(map[v][node_y]) {
			++cnt;
		}
	}
	
	return cnt;

}


/**
@name: TDM_scheduler_fit_singular_col
@param:

Given x index, determines fit based on if current column is reducible
**/
int TDM_scheduler_fit_singular_col(unsigned char **map, int node_x, int lim) {
	int v, result=PASS;
	
	for (v=0; v<lim; v++) {
		TOKEN_CHECK(map[node_x][v]) {
			if ( TDM_scheduler_fit_singular_cnt(map,v)>1 ) {
				result=FAIL;
				break;
			}
		}
	}
	
	return result;

}


/**
@name: TDM_scheduler_fit_singular_row
@param: unsigned char **, int

Given y index, determines fit based on if current row is reducible
**/
int TDM_scheduler_fit_singular(unsigned char **map, int node_y) {
	int v, node_id=_TH_NUM_EXT_PORTS, node_idx=_TH_NUM_EXT_PORTS, node_idx_last=_TH_NUM_EXT_PORTS;/*, check=PASS;*/
	
	/* Check that the node is a singular node or can be reduced to a singular node */
	for (v=0; v<VEC_MAP_WID; v++) {
		if (map[v][node_y]!=_TH_NUM_EXT_PORTS && node_id==_TH_NUM_EXT_PORTS) {
			node_id = map[v][node_y];
			node_idx_last = v;
		}
		if (node_id!=_TH_NUM_EXT_PORTS && map[v][node_y]!=_TH_NUM_EXT_PORTS) {
			node_idx = (v > node_idx_last)?(v):(_TH_NUM_EXT_PORTS);
			if (node_idx!=_TH_NUM_EXT_PORTS) {break;}
		}
	}
	if (node_id==_TH_NUM_EXT_PORTS) {
		node_idx=BLANK;
	}
	
	return node_idx;
}


/**
@name: TDM_scheduler_weight_update
@param:

Recalculates nodal weight array
**/
void TDM_scheduler_weight_update(unsigned char **map, int lim, int weights[VEC_MAP_WID], int weights_last[VEC_MAP_WID], int weights_cmp[VEC_MAP_WID], int span, int mpass, int lr_slot_cnt)
{
	int i;
	
	for (i=0; i<VEC_MAP_WID; i++) {
		weights_last[i] = weights[i];
	}
	for (i=1; i<=span; i++) {
		weights[i]=TDM_scheduler_map_shortest_distance(map,lim,span,(span-i));
	}
	#ifdef _SET_TDM_DEV
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n")));
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("Analyzing TDM matrix...\n")));
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\tvector %0d | subpass %0d | threshold %0d\n"),
                 span,mpass,((lim/lr_slot_cnt)+VECTOR_ISOLATION)));
	#endif
	for (i=1; i<=span; i++) {
		weights_cmp[i]=tdm_abs(weights[i]-weights_last[i]);
		#ifdef _SET_TDM_DEV
        LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("\tDR_weight_last[%0d]=%03d | DR_weight[%0d]=%03d | DR_weight_cmp[%0d]=%03d\n"),
                     i,weights_last[i],i,weights[i],i,weights_cmp[i]));
		#endif
	}
	#ifdef _SET_TDM_DEV
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n")));
	#endif
}


/**
@name: TDM_scheduler_weight_min
@param:

Returns minimum value from among nodal weight array
**/
int TDM_scheduler_weight_min(int weights[VEC_MAP_WID], int span)
{
	int i, min=weights[1];
	
	for (i=2; i<=span; i++) {
		if (weights[i]<min) {
			min = weights[i];
		}
	}
	
	return min;
}


/**
@name: TDM_scheduler_weight_vnum
@param:

Returns number of subthreshold violations from nodal weight array
**/
int TDM_scheduler_weight_vnum(int weights[VEC_MAP_WID], int span, int threshold)
{
	int i, cnt=0;
	
	for (i=1; i<=span; i++) {
		if (weights[i]<threshold) {
			++cnt;
		}
	}
	
	return cnt;
}


/**
@name: TDM_scheduler_vector_diff
@param: unsigned char **, int, int

Given x index, diffs the transmuted vector at this position with the prototype from library
*** Returns PASS if within jitter limits, FAIL if illegal ***
**/
int TDM_scheduler_vector_diff(unsigned char **map, int bw, int lim, int x, enum port_speed speed[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1]) {

	/*
	int v, i, k, y, tmp_length, tmp_width, result=PASS, spd=0, jitter;
	unsigned char **temp_map;
	*/
	int result=PASS;

	/* Generate fake TDM vector map */
	/* tmp_length=VEC_MAP_LEN; */
	/* tmp_width=2; */
	
	/*
	temp_map=(unsigned char **) TDM_ALLOC(tmp_width*sizeof(unsigned char *), "temp_map_l1");
	for (v=0; v<tmp_width; v++) {
		temp_map[v]=(unsigned char *) TDM_ALLOC(tmp_length*sizeof(unsigned char), "temp_map_l2");
	}
	for (v=0; v<tmp_length; v++) {
		temp_map[0][v]=_TH_NUM_EXT_PORTS;
	}
	k=0; y=(VEC_MAP_LEN+1);
	for (v=0; v<lim; v++) {
		if (map[x][v]!=_TH_NUM_EXT_PORTS) {
			if (y>VEC_MAP_LEN) {y=v;}
			k++;
		}
	}
	switch(k) {
		case 4: if (check_ethernet(map[x][y], speed, tsc, traffic)) {spd = 10;} else {spd = 11;} break;
		case 8: if (check_ethernet(map[x][y], speed, tsc, traffic)) {spd = 20;} else {spd = 21;} break;
		case 10: if (check_ethernet(map[x][y], speed, tsc, traffic)) {spd = 25;} else {spd = 27;} break;
		case 16: if (check_ethernet(map[x][y], speed, tsc, traffic)) {spd = 40;} else {spd = 42;} break;
		case 20: if (check_ethernet(map[x][y], speed, tsc, traffic)) {spd = 50;} else {spd = 53;} break;
		case 39: spd = 107; break;
		case 40: if (check_ethernet(map[x][y], speed, tsc, traffic)) {spd = 100;} else {spd = 106;} break;
	}
	*/
	
	/* Load prototype vector at index 0 in temp map */
	/*
	result = tdm_vector_load(temp_map, bw, spd, 0, lim);
	if (result==0) {
		return 0;
	}
	*/
	/* Copy vector to be diffed at index 1 in temp map, deskew it */
	/*
	for (v=0; v<lim; v++) {
		temp_map[1][v]=map[x][v];
	}
	while(temp_map[1][0]==_TH_NUM_EXT_PORTS) {
		tdm_vector_rotate(temp_map[1], lim, -1);
	}
	for (v=0; v<lim; v++) {
		if (temp_map[0][v]!=_TH_NUM_EXT_PORTS) {temp_map[0][v]=temp_map[1][0];}
	}
	*/

	/* Diff all nodes */
	/*
	for (v=0; v<lim; v++) {
		if (temp_map[0][v]!=temp_map[1][v]) {
			switch(spd) {
				case 10:case 11:
					jitter=JITTER_THRESH_HI;
					break;
				case 20:case 21:case 25:case 27:
					jitter=JITTER_THRESH_MH;
					break;
				case 40:case 42:case 50:case 53:
					jitter=JITTER_THRESH_ML;
					break;
				default:
					jitter=JITTER_THRESH_LO;
					break;
			}
			for (i=1; i<=jitter; i++) {
				result=FAIL;
				if (temp_map[0][v]==temp_map[1][v-i] || temp_map[0][v]==temp_map[1][v+i]) {
					result=PASS;
					break;
				}
			}
		}
		if (result==FAIL) {
			break;
		}
	}
	
	for (v=0; v<tmp_width; v++) {
		TDM_FREE(temp_map[v]);
	}
	TDM_FREE(temp_map);
	*/
	
	return result;
	
}


/**
@name: TSC_port_transcription
@param: int[][], enum, int[]

For Tomahawk BCM56960
Physical port transcriber for tsc subports into physical port numbers using round robin distribution
**/
void tsc_port_transcription(int wc_array[NUM_TSC][4], enum port_speed speed[_TH_NUM_EXT_PORTS], int port_state_array[_TH_NUM_EXT_PORTS])
{
	int i, j, last_port=_TH_NUM_EXT_PORTS;
	/*char tmp_str[16];*/
	/*sprintf (cmd_str, "soc setmem -index %x %s {", index->d, addr);*/

	for (i=0; i<32; i++) {
		for (j=0; j<4; j++) {
			wc_array[i][j] = _TH_NUM_EXT_PORTS;
		}
	}
	for (i = 1; i < _TH_NUM_EXT_PORTS-7; i+=4) {
		if (speed[i] > SPEED_0) {
			for (j=0; j<4; j++) {
				switch (port_state_array[i-1+j]) {
					case 1: case 2:
						wc_array[(i-1)/4][j] = (i+j);
						last_port=(i+j);
						break;
					case 3:
						wc_array[(i-1)/4][j] = last_port;
						break;
					default:
						wc_array[(i-1)/4][j] = _TH_NUM_EXT_PORTS;
						break;
				}
			}
			/*tri mode x_xx*/
			if (speed[i]>speed[i+2] && speed[i+2]==speed[i+3] && speed[i+2]!=SPEED_0) {
				wc_array[(i-1)/4][1] = wc_array[(i-1)/4][2];
				wc_array[(i-1)/4][2] = wc_array[(i-1)/4][0];
			}
			/*tri mode xxx_*/
			else if (speed[i]==speed[i+1] && speed[i]<speed[i+2] && speed[i]!=SPEED_0)  {
				wc_array[(i-1)/4][2] = wc_array[(i-1)/4][1];
				wc_array[(i-1)/4][1] = wc_array[(i-1)/4][3];
			}
			/*dual mode*/
			else if (speed[i]!=speed[i+1] && speed[i]==speed[i+2] && speed[i]!=SPEED_0) {
				wc_array[(i-1)/4][1] = wc_array[(i-1)/4][3];
				wc_array[(i-1)/4][2] = wc_array[(i-1)/4][0];
			}
		}
	}

}


/**
@name: parse_mmu_tdm_tbl
@param: int, int, int[], int[], int[], int[], int[], int

Converts accessory tokens and prints debug form summary of pipe table
 **/
void parse_tdm_tbl(FILE *file, int mgmt_bw, int tdm_tbl[256], int tdm_ovs_a[_TH_OS_GROUP_LEN], int tdm_ovs_b[_TH_OS_GROUP_LEN], int tdm_ovs_c[_TH_OS_GROUP_LEN], int tdm_ovs_d[_TH_OS_GROUP_LEN], int tdm_ovs_e[_TH_OS_GROUP_LEN], int tdm_ovs_f[_TH_OS_GROUP_LEN], int tdm_ovs_g[_TH_OS_GROUP_LEN], int tdm_ovs_h[_TH_OS_GROUP_LEN], int block, int pipe)
{
	int i, j, m=0;
	const char *name;
   
	name = (block==0) ? "IDB" : "MMU";
   
	for (j=0; j<256; j++) {
		if (tdm_tbl[j]!=ACC_TOKEN) {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: PIPE: %d, %s TDM TABLE, element #%0d, contains physical port #%0d\n"),
                         pipe, name, j, tdm_tbl[j]));
		}
		else {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("TDM: PIPE: %d, %s TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as "),
                         pipe, name, j));
			m++;
			/* IDB */
			if (block==0) {
				switch (pipe) {
					case 0:
						switch (m) {
							case 1: case 3: case 7: case 9:
								CMIC(j)
							case 2: case 6:
								LPB0(j)
							case 4: case 10:
								OPRT(j)
							case 5:
								NULL_SLOT(j)
							case 8:		
								SBUS(j)
						}
						break;
					case 1:
						switch (m) {
							case 1: case 3: case 7: case 9:
								MGM1(j)
							case 2: case 6:
								LPB1(j)
							case 4: case 10:
								OPRT(j)
							case 5:
								NULL_SLOT(j)
							case 8:		
								SBUS(j)		
						}			
						break;
					case 2:
						switch (m) {
							case 1: case 3: case 7: case 9:
								MGM2(j)
							case 2: case 6:
								LPB2(j)
							case 4: case 10:
								OPRT(j)
							case 5:
								NULL_SLOT(j)
							case 8:		
								SBUS(j)
						}
						break;
					case 3:
						switch (m) {
							case 1: case 3: case 7: case 9:
								DOT3(j)
							case 2: case 6:
								LPB3(j)
							case 4: case 10:
								OPRT(j)		
							case 5:
								NULL_SLOT(j)								
							case 8:
								SBUS(j)
						}			
						break;
				}
			}
			else {
				switch (pipe) {
					case 0:
						switch (m) {
							case 1: case 3: case 7: case 9:
								CMIC(j)
							case 2: case 6:
								LPB0(j)
							case 4: case 5: case 8:
								NULL_SLOT(j)
							case 10:
								OPRT(j)
						}
						break;
					case 1:
						switch (m) {
							case 1: case 3: case 7: case 9:
								MGM1(j)
							case 2: case 6:
								LPB1(j)
							case 4: case 5: case 8:
								NULL_SLOT(j)
							case 10:
								OPRT(j)		
						}			
						break;
					case 2:
						switch (m) {
							case 1: case 3: case 7: case 9:
								MGM2(j)
							case 2: case 6:
								LPB2(j)
							case 4: case 5: case 8:
								NULL_SLOT(j)
							case 10:
								OPRT(j)
						}
						break;
					case 3:
						switch (m) {
							case 1: case 3: case 7: case 9:
								DOT3(j)
							case 2: case 6:
								LPB3(j)								
							case 4: case 5: case 8:
								NULL_SLOT(j)
							case 10:
								OPRT(j)
						}			
						break;
				}
			}
		}
	}
	for (j=0; j<_TH_OS_GROUP_LEN; j++)
	{
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: PIPE: %d, %s OVS BUCKET 0, element #%0d, contains physical port #%0d\n"),
                     pipe, name, j, tdm_ovs_a[j]));
	}
	for (j=0; j<_TH_OS_GROUP_LEN; j++)
	{
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: PIPE: %d, %s OVS BUCKET 1, element #%0d, contains physical port #%0d\n"),
                     pipe, name, j, tdm_ovs_b[j]));
	}
	for (j=0; j<_TH_OS_GROUP_LEN; j++)
	{
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: PIPE: %d, %s OVS BUCKET 2, element #%0d, contains physical port #%0d\n"),
                     pipe, name, j, tdm_ovs_c[j]));
	}
	for (j=0; j<_TH_OS_GROUP_LEN; j++)
	{
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: PIPE: %d, %s OVS BUCKET 3, element #%0d, contains physical port #%0d\n"),
                     pipe, name, j, tdm_ovs_d[j]));
	}
	for (j=0; j<_TH_OS_GROUP_LEN; j++)
	{
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: PIPE: %d, %s OVS BUCKET 4, element #%0d, contains physical port #%0d\n"),
                     pipe, name, j, tdm_ovs_e[j]));
	}
	for (j=0; j<_TH_OS_GROUP_LEN; j++)
	{
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: PIPE: %d, %s OVS BUCKET 5, element #%0d, contains physical port #%0d\n"),
                     pipe, name, j, tdm_ovs_f[j]));
	}
	for (j=0; j<_TH_OS_GROUP_LEN; j++)
	{
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: PIPE: %d, %s OVS BUCKET 6, element #%0d, contains physical port #%0d\n"),
                     pipe, name, j, tdm_ovs_g[j]));
	}
	for (j=0; j<_TH_OS_GROUP_LEN; j++)
	{
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: PIPE: %d, %s OVS BUCKET 7, element #%0d, contains physical port #%0d\n"),
                     pipe, name, j, tdm_ovs_h[j]));
	}
}


/**
@name: TDM_scheduler_parse_pipe
@param:

Summarizes pipe config
**/
void TDM_scheduler_parse_pipe(FILE *file, enum port_speed speed[_TH_NUM_EXT_PORTS], int portmap[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int pipe_start, int pipe_end)
{
	int iter, iter2;

	/* Summarize the port config in this quadrant */
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("TDM: --- Pipe Config ---: ")));
	for (iter=pipe_start; iter<=pipe_end; iter++) {
		if ((((iter-1)%16)==0)) {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("\nTDM: ")));
		}		
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("{%03d}\t"),
                     (iter)));
		if (iter%16==0) {
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("\nTDM: ")));
			for (iter2=(iter-16); iter2<iter; iter2++) {
				LOG_VERBOSE(BSL_LS_SOC_TDM,
                            (BSL_META("%d\t"),
                             speed[iter2+1]));
			}
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("\nTDM: ")));
			for (iter2=(iter-16); iter2<iter; iter2++) {
				if ((iter2-1)%4==0) {
					switch (portmap[iter2-1]) {
						case 1:
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("LINE\t---\t---\t---\t")));
							break;
						case 2:
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("OVSB\t---\t---\t---\t")));
							break;
						default:
							break;
					}
				}
			}
			LOG_VERBOSE(BSL_LS_SOC_TDM,
                        (BSL_META("\nTDM: ")));
			for (iter2=(iter-16); iter2<iter; iter2++) {
				if ((iter2-1)%4==0) {
					switch (traffic[which_tsc((iter2+1), tsc)]) {
						case 999:
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("HIGIG2\t---\t---\t---\t")));
							break;
						case 998:
							LOG_VERBOSE(BSL_LS_SOC_TDM,
                                        (BSL_META("ETHRNT\t---\t---\t---\t")));
							break;
						default:
							break;
					}
				}
			}
		}
	}
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("\n")));
	TDM_BIG_BAR

}


/**
@name: TDM_scheduler_wrap
@param: int[][], enum, int[], int[], int, int, int[], int, int

Wrapper for TDM populating function
**/
int TDM_scheduler_wrap(FILE *file, int bw, enum port_speed speed[_TH_NUM_EXT_PORTS], int portmap[_TH_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int pipe_start, int pipe_end, 
					   int idb_tdm_tbl[256], 
					   int idb_tdm_ovs_a[_TH_OS_GROUP_LEN], int idb_tdm_ovs_b[_TH_OS_GROUP_LEN], int idb_tdm_ovs_c[_TH_OS_GROUP_LEN], int idb_tdm_ovs_d[_TH_OS_GROUP_LEN], 
					   int idb_tdm_ovs_e[_TH_OS_GROUP_LEN], int idb_tdm_ovs_f[_TH_OS_GROUP_LEN], int idb_tdm_ovs_g[_TH_OS_GROUP_LEN], int idb_tdm_ovs_h[_TH_OS_GROUP_LEN], 
					   int mmu_tdm_tbl[256], 
					   int mmu_tdm_ovs_a[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_b[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_c[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_d[_TH_OS_GROUP_LEN],
					   int mmu_tdm_ovs_e[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_f[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_g[_TH_OS_GROUP_LEN], int mmu_tdm_ovs_h[_TH_OS_GROUP_LEN])
{
	int lr_idx_limit=LEN_850MHZ_EN, iter, idx1, idx2, scheduler_state, lr_buffer[64], os_buffer[64], pipe=0;
	unsigned char acc=TH_ANCILLARY_PORTS, higig_mgmt=BOOL_FALSE;
	
	switch (pipe_start/32) {
		case 0: pipe=0; break;
		case 1: pipe=1; break;
		case 2: pipe=2; break;
		case 3: pipe=3; break;
	}
	
	TDM_scheduler_parse_pipe(file, speed, portmap, tsc, traffic, pipe_start, pipe_end);
	
	/* Queue all enabled ports */
	idx1=0; idx2=0;
	for (iter=0; iter<64; iter++) {
		lr_buffer[iter]=_TH_NUM_EXT_PORTS;
		os_buffer[iter]=_TH_NUM_EXT_PORTS;
	}
	for (iter=(pipe_start-1); iter<(pipe_end); iter++) {
		if (portmap[iter]==1) {
			lr_buffer[idx1]=(iter+1);
			if (idx1<32) {
				idx1++;
			}
			else {
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Line rate queue overflow, ports may have been skipped.\n")));
			}
		}
		else if (portmap[iter]==2) {
			os_buffer[idx2]=(iter+1);
			if (idx2<32) {
				idx2++;
			}
			else {
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Oversub queue overflow, ports may have been skipped.\n")));
			}
		}
	}
	
	/* Max length of the TDM vector map */
	/* --- Line rate, all HG2 is not necessary */
    /* Supported formats:
	 * 1. All OS, no DVFS
	 * 2. All OS, DVFS (use 560MHz length)
	 * 3. Line rate, mixed HG2/Ethernet
	 * 4. Line rate, all Ethernet 
	 */
	if ( (traffic[NUM_TSC]==PORT_HIGIG2&&(pipe==1 || pipe==2)) && ((lr_buffer[0]!=_TH_NUM_EXT_PORTS&&bw>=MIN_HG_FREQ)||(lr_buffer[0]==_TH_NUM_EXT_PORTS)) ) {
		higig_mgmt=BOOL_TRUE;
	}
	if ( (check_ethernet(lr_buffer[0], speed, tsc, traffic)) && (higig_mgmt==BOOL_FALSE) ) {
		switch (bw) {
			/* TH */
			case 850: lr_idx_limit=(LEN_850MHZ_EN-acc); break; /* 850MHz (214+10)*2.65G */
			case 766:
			case 765:
				lr_idx_limit=(LEN_765MHZ_EN-acc); break; /* 765MHz (192+10)*2.65G */
			case 672: lr_idx_limit=(LEN_672MHZ_EN-acc); break; /* 672MHz (167+10)*2.65G */
			case 645: lr_idx_limit=(LEN_645MHZ_EN-acc); break; /* 645MHz (160+10)*2.65G */
			case 545: lr_idx_limit=(LEN_545MHZ_EN-acc); break; /* 545MHz (133+10)*2.65G */	
			default:
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Invalid frequency\n")));
				return 0;
				break;
		}
	}
	else {
		switch (bw) {
			/* TH */
			case 850: lr_idx_limit=(LEN_850MHZ_HG-acc); break; /* 850MHz (205+10)*2.65G */
			case 766:
			case 765: 
				lr_idx_limit=(LEN_765MHZ_HG-acc); break; /* 765MHz (184+10)*2.65G */
			case 672: lr_idx_limit=(LEN_672MHZ_HG-acc); break; /* 672MHz (160+10)*2.65G */
			case 645: lr_idx_limit=(LEN_645MHZ_HG-acc); break; /* 645MHz (153+10)*2.65G */
			case 545: lr_idx_limit=(LEN_545MHZ_HG-acc); break; /* 545MHz (128+10)*2.65G */
			default:
				LOG_ERROR(BSL_LS_SOC_TDM,
                          (BSL_META("Invalid frequency\n")));
				return 0;
				break;
		}
	}	
	
#ifdef _SET_TDM_LEGACY
	scheduler_state = TDM_scheduler_1d(file, bw, speed, portmap, tsc, lr_buffer, os_buffer, idb_tdm_tbl, idb_tdm_ovs_a, idb_tdm_ovs_b, idb_tdm_ovs_c, idb_tdm_ovs_d);
	/* scheduler_state = TDM_scheduler_1d(file, bw, speed, portmap, tsc, lr_buffer, os_buffer, mmu_tdm_tbl, mmu_tdm_ovs_a, mmu_tdm_ovs_b, mmu_tdm_ovs_c, mmu_tdm_ovs_d); */
#else
	scheduler_state = TDM_scheduler_2d(file, bw, speed, portmap, tsc, traffic, lr_buffer, os_buffer, lr_idx_limit, idb_tdm_tbl, idb_tdm_ovs_a, idb_tdm_ovs_b, idb_tdm_ovs_c, idb_tdm_ovs_d, idb_tdm_ovs_e, idb_tdm_ovs_f, idb_tdm_ovs_g, idb_tdm_ovs_h, pipe, acc, higig_mgmt);
	/* scheduler_state = TDM_scheduler_2d(file, bw, speed, portmap, tsc, traffic, lr_buffer, os_buffer, mmu_tdm_tbl, mmu_tdm_ovs_a, mmu_tdm_ovs_b, mmu_tdm_ovs_c, mmu_tdm_ovs_d, mmu_tdm_ovs_e, mmu_tdm_ovs_f, mmu_tdm_ovs_g, mmu_tdm_ovs_h); */
#endif
	for (iter=0; iter<256; iter++) {
		mmu_tdm_tbl[iter]=idb_tdm_tbl[iter];
	}
	for (iter=0; iter<_TH_OS_GROUP_LEN; iter++) {
		mmu_tdm_ovs_a[iter]=idb_tdm_ovs_a[iter];
		mmu_tdm_ovs_b[iter]=idb_tdm_ovs_b[iter];
		mmu_tdm_ovs_c[iter]=idb_tdm_ovs_c[iter];
		mmu_tdm_ovs_d[iter]=idb_tdm_ovs_d[iter];
		mmu_tdm_ovs_e[iter]=idb_tdm_ovs_e[iter];
		mmu_tdm_ovs_f[iter]=idb_tdm_ovs_f[iter];
		mmu_tdm_ovs_g[iter]=idb_tdm_ovs_g[iter];
		mmu_tdm_ovs_h[iter]=idb_tdm_ovs_h[iter];	
	}	
	
	TDM_BIG_BAR
	
	return scheduler_state;
	
}


/**
@name: TH_set_tdm_tbl
@param:

SV interfaced entry point for the 19 pre-configured TDM configurations
**/
int TH_set_tdm_tbl(th_tdm_globals_t *tdm_globals, th_tdm_pipes_t *tdm_pipe_tables)
{
	int tsc_array[NUM_TSC][4];
	int i, mgmtbw=0;
	int checkpoint[4];
	int ver[2];
	FILE *file;
	
	enum port_speed *speed;
	int freq;
	int *port_state_map;
	int *pm_encap_type;

	int *idb_tdm_tbl_0;
	int *idb_tdm_ovs_0_a;
	int *idb_tdm_ovs_0_b;
	int *idb_tdm_ovs_0_c;
	int *idb_tdm_ovs_0_d;
	int *idb_tdm_ovs_0_e;
	int *idb_tdm_ovs_0_f;
	int *idb_tdm_ovs_0_g;
	int *idb_tdm_ovs_0_h;
	int *idb_tdm_tbl_1;
	int *idb_tdm_ovs_1_a;
	int *idb_tdm_ovs_1_b;
	int *idb_tdm_ovs_1_c;
	int *idb_tdm_ovs_1_d;
	int *idb_tdm_ovs_1_e;
	int *idb_tdm_ovs_1_f;
	int *idb_tdm_ovs_1_g;
	int *idb_tdm_ovs_1_h;
	int *idb_tdm_tbl_2;
	int *idb_tdm_ovs_2_a;
	int *idb_tdm_ovs_2_b;
	int *idb_tdm_ovs_2_c;
	int *idb_tdm_ovs_2_d;
	int *idb_tdm_ovs_2_e;
	int *idb_tdm_ovs_2_f;
	int *idb_tdm_ovs_2_g;
	int *idb_tdm_ovs_2_h;
	int *idb_tdm_tbl_3;
	int *idb_tdm_ovs_3_a;
	int *idb_tdm_ovs_3_b;
	int *idb_tdm_ovs_3_c;
	int *idb_tdm_ovs_3_d;
	int *idb_tdm_ovs_3_e;
	int *idb_tdm_ovs_3_f;
	int *idb_tdm_ovs_3_g;
	int *idb_tdm_ovs_3_h;
	int *mmu_tdm_tbl_0;
	int *mmu_tdm_ovs_0_a;
	int *mmu_tdm_ovs_0_b;
	int *mmu_tdm_ovs_0_c;
	int *mmu_tdm_ovs_0_d;
	int *mmu_tdm_ovs_0_e;
	int *mmu_tdm_ovs_0_f;
	int *mmu_tdm_ovs_0_g;
	int *mmu_tdm_ovs_0_h;
	int *mmu_tdm_tbl_1;
	int *mmu_tdm_ovs_1_a;
	int *mmu_tdm_ovs_1_b;
	int *mmu_tdm_ovs_1_c;
	int *mmu_tdm_ovs_1_d;
	int *mmu_tdm_ovs_1_e;
	int *mmu_tdm_ovs_1_f;
	int *mmu_tdm_ovs_1_g;
	int *mmu_tdm_ovs_1_h;
	int *mmu_tdm_tbl_2;
	int *mmu_tdm_ovs_2_a;
	int *mmu_tdm_ovs_2_b;
	int *mmu_tdm_ovs_2_c;
	int *mmu_tdm_ovs_2_d;
	int *mmu_tdm_ovs_2_e;
	int *mmu_tdm_ovs_2_f;
	int *mmu_tdm_ovs_2_g;
	int *mmu_tdm_ovs_2_h;
	int *mmu_tdm_tbl_3;
	int *mmu_tdm_ovs_3_a;
	int *mmu_tdm_ovs_3_b;
	int *mmu_tdm_ovs_3_c;
	int *mmu_tdm_ovs_3_d;
	int *mmu_tdm_ovs_3_e;
	int *mmu_tdm_ovs_3_f;
	int *mmu_tdm_ovs_3_g;
	int *mmu_tdm_ovs_3_h;

	speed = tdm_globals->speed;
	freq = tdm_globals->clk_freq;
	port_state_map = tdm_globals->port_rates_array;
	pm_encap_type = tdm_globals->pm_encap_type;

	idb_tdm_tbl_0 = tdm_pipe_tables->idb_tdm_tbl_0;
	idb_tdm_ovs_0_a = tdm_pipe_tables->idb_tdm_ovs_0_a;
	idb_tdm_ovs_0_b = tdm_pipe_tables->idb_tdm_ovs_0_b;
	idb_tdm_ovs_0_c = tdm_pipe_tables->idb_tdm_ovs_0_c;
	idb_tdm_ovs_0_d = tdm_pipe_tables->idb_tdm_ovs_0_d;
	idb_tdm_ovs_0_e = tdm_pipe_tables->idb_tdm_ovs_0_e;
	idb_tdm_ovs_0_f = tdm_pipe_tables->idb_tdm_ovs_0_f;
	idb_tdm_ovs_0_g = tdm_pipe_tables->idb_tdm_ovs_0_g;
	idb_tdm_ovs_0_h = tdm_pipe_tables->idb_tdm_ovs_0_h;
	idb_tdm_tbl_1 = tdm_pipe_tables->idb_tdm_tbl_1;
	idb_tdm_ovs_1_a = tdm_pipe_tables->idb_tdm_ovs_1_a;
	idb_tdm_ovs_1_b = tdm_pipe_tables->idb_tdm_ovs_1_b;
	idb_tdm_ovs_1_c = tdm_pipe_tables->idb_tdm_ovs_1_c;
	idb_tdm_ovs_1_d = tdm_pipe_tables->idb_tdm_ovs_1_d;
	idb_tdm_ovs_1_e = tdm_pipe_tables->idb_tdm_ovs_1_e;
	idb_tdm_ovs_1_f = tdm_pipe_tables->idb_tdm_ovs_1_f;
	idb_tdm_ovs_1_g = tdm_pipe_tables->idb_tdm_ovs_1_g;
	idb_tdm_ovs_1_h = tdm_pipe_tables->idb_tdm_ovs_1_h;
	idb_tdm_tbl_2 = tdm_pipe_tables->idb_tdm_tbl_2;
	idb_tdm_ovs_2_a = tdm_pipe_tables->idb_tdm_ovs_2_a;
	idb_tdm_ovs_2_b = tdm_pipe_tables->idb_tdm_ovs_2_b;
	idb_tdm_ovs_2_c = tdm_pipe_tables->idb_tdm_ovs_2_c;
	idb_tdm_ovs_2_d = tdm_pipe_tables->idb_tdm_ovs_2_d;
	idb_tdm_ovs_2_e = tdm_pipe_tables->idb_tdm_ovs_2_e;
	idb_tdm_ovs_2_f = tdm_pipe_tables->idb_tdm_ovs_2_f;
	idb_tdm_ovs_2_g = tdm_pipe_tables->idb_tdm_ovs_2_g;
	idb_tdm_ovs_2_h = tdm_pipe_tables->idb_tdm_ovs_2_h;
	idb_tdm_tbl_3 = tdm_pipe_tables->idb_tdm_tbl_3;
	idb_tdm_ovs_3_a = tdm_pipe_tables->idb_tdm_ovs_3_a;
	idb_tdm_ovs_3_b = tdm_pipe_tables->idb_tdm_ovs_3_b;
	idb_tdm_ovs_3_c = tdm_pipe_tables->idb_tdm_ovs_3_c;
	idb_tdm_ovs_3_d = tdm_pipe_tables->idb_tdm_ovs_3_d;
	idb_tdm_ovs_3_e = tdm_pipe_tables->idb_tdm_ovs_3_e;
	idb_tdm_ovs_3_f = tdm_pipe_tables->idb_tdm_ovs_3_f;
	idb_tdm_ovs_3_g = tdm_pipe_tables->idb_tdm_ovs_3_g;
	idb_tdm_ovs_3_h = tdm_pipe_tables->idb_tdm_ovs_3_h;
	mmu_tdm_tbl_0 = tdm_pipe_tables->mmu_tdm_tbl_0;
	mmu_tdm_ovs_0_a = tdm_pipe_tables->mmu_tdm_ovs_0_a;
	mmu_tdm_ovs_0_b = tdm_pipe_tables->mmu_tdm_ovs_0_b;
	mmu_tdm_ovs_0_c = tdm_pipe_tables->mmu_tdm_ovs_0_c;
	mmu_tdm_ovs_0_d = tdm_pipe_tables->mmu_tdm_ovs_0_d;
	mmu_tdm_ovs_0_e = tdm_pipe_tables->mmu_tdm_ovs_0_e;
	mmu_tdm_ovs_0_f = tdm_pipe_tables->mmu_tdm_ovs_0_f;
	mmu_tdm_ovs_0_g = tdm_pipe_tables->mmu_tdm_ovs_0_g;
	mmu_tdm_ovs_0_h = tdm_pipe_tables->mmu_tdm_ovs_0_h;
	mmu_tdm_tbl_1 = tdm_pipe_tables->mmu_tdm_tbl_1;
	mmu_tdm_ovs_1_a = tdm_pipe_tables->mmu_tdm_ovs_1_a;
	mmu_tdm_ovs_1_b = tdm_pipe_tables->mmu_tdm_ovs_1_b;
	mmu_tdm_ovs_1_c = tdm_pipe_tables->mmu_tdm_ovs_1_c;
	mmu_tdm_ovs_1_d = tdm_pipe_tables->mmu_tdm_ovs_1_d;
	mmu_tdm_ovs_1_e = tdm_pipe_tables->mmu_tdm_ovs_1_e;
	mmu_tdm_ovs_1_f = tdm_pipe_tables->mmu_tdm_ovs_1_f;
	mmu_tdm_ovs_1_g = tdm_pipe_tables->mmu_tdm_ovs_1_g;
	mmu_tdm_ovs_1_h = tdm_pipe_tables->mmu_tdm_ovs_1_h;
	mmu_tdm_tbl_2 = tdm_pipe_tables->mmu_tdm_tbl_2;
	mmu_tdm_ovs_2_a = tdm_pipe_tables->mmu_tdm_ovs_2_a;
	mmu_tdm_ovs_2_b = tdm_pipe_tables->mmu_tdm_ovs_2_b;
	mmu_tdm_ovs_2_c = tdm_pipe_tables->mmu_tdm_ovs_2_c;
	mmu_tdm_ovs_2_d = tdm_pipe_tables->mmu_tdm_ovs_2_d;
	mmu_tdm_ovs_2_e = tdm_pipe_tables->mmu_tdm_ovs_2_e;
	mmu_tdm_ovs_2_f = tdm_pipe_tables->mmu_tdm_ovs_2_f;
	mmu_tdm_ovs_2_g = tdm_pipe_tables->mmu_tdm_ovs_2_g;
	mmu_tdm_ovs_2_h = tdm_pipe_tables->mmu_tdm_ovs_2_h;
	mmu_tdm_tbl_3 = tdm_pipe_tables->mmu_tdm_tbl_3;
	mmu_tdm_ovs_3_a = tdm_pipe_tables->mmu_tdm_ovs_3_a;
	mmu_tdm_ovs_3_b = tdm_pipe_tables->mmu_tdm_ovs_3_b;
	mmu_tdm_ovs_3_c = tdm_pipe_tables->mmu_tdm_ovs_3_c;
	mmu_tdm_ovs_3_d = tdm_pipe_tables->mmu_tdm_ovs_3_d;
	mmu_tdm_ovs_3_e = tdm_pipe_tables->mmu_tdm_ovs_3_e;
	mmu_tdm_ovs_3_f = tdm_pipe_tables->mmu_tdm_ovs_3_f;
	mmu_tdm_ovs_3_g = tdm_pipe_tables->mmu_tdm_ovs_3_g;
	mmu_tdm_ovs_3_h = tdm_pipe_tables->mmu_tdm_ovs_3_h;

	#ifdef _SET_TDM_DUMP
	remove(LOG);
	file = fopen(LOG,"a+");
	#else
	file = (FILE *) TDM_ALLOC((sizeof(int)*56),"FILE");
	#endif
	
	tdm_ver(ver);
	
	for (i=0; i<256; i++) {
		idb_tdm_tbl_0[i]=_TH_NUM_EXT_PORTS; idb_tdm_tbl_1[i]=_TH_NUM_EXT_PORTS; idb_tdm_tbl_2[i]=_TH_NUM_EXT_PORTS; idb_tdm_tbl_3[i]=_TH_NUM_EXT_PORTS;
		mmu_tdm_tbl_0[i]=_TH_NUM_EXT_PORTS; mmu_tdm_tbl_1[i]=_TH_NUM_EXT_PORTS; mmu_tdm_tbl_2[i]=_TH_NUM_EXT_PORTS; mmu_tdm_tbl_3[i]=_TH_NUM_EXT_PORTS;
	}
	for (i=0; i<_TH_OS_GROUP_LEN; i++) {
		mmu_tdm_ovs_0_a[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_0_b[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_0_c[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_0_d[i] = _TH_NUM_EXT_PORTS;
		mmu_tdm_ovs_0_e[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_0_f[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_0_g[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_0_h[i] = _TH_NUM_EXT_PORTS;
		mmu_tdm_ovs_1_a[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_1_b[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_1_c[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_1_d[i] = _TH_NUM_EXT_PORTS;
		mmu_tdm_ovs_1_e[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_1_f[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_1_g[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_1_h[i] = _TH_NUM_EXT_PORTS;
		mmu_tdm_ovs_2_a[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_2_b[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_2_c[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_2_d[i] = _TH_NUM_EXT_PORTS;
		mmu_tdm_ovs_2_e[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_2_f[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_2_g[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_2_h[i] = _TH_NUM_EXT_PORTS;
		mmu_tdm_ovs_3_a[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_3_b[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_3_c[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_3_d[i] = _TH_NUM_EXT_PORTS;
		mmu_tdm_ovs_3_e[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_3_f[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_3_g[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_3_h[i] = _TH_NUM_EXT_PORTS;
		idb_tdm_ovs_0_a[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_0_b[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_0_c[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_0_d[i] = _TH_NUM_EXT_PORTS;
		idb_tdm_ovs_0_e[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_0_f[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_0_g[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_0_h[i] = _TH_NUM_EXT_PORTS;
		idb_tdm_ovs_1_a[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_1_b[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_1_c[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_1_d[i] = _TH_NUM_EXT_PORTS;
		idb_tdm_ovs_1_e[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_1_f[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_1_g[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_1_h[i] = _TH_NUM_EXT_PORTS;
		idb_tdm_ovs_2_a[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_2_b[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_2_c[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_2_d[i] = _TH_NUM_EXT_PORTS;
		idb_tdm_ovs_2_e[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_2_f[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_2_g[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_2_h[i] = _TH_NUM_EXT_PORTS;
		idb_tdm_ovs_3_a[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_3_b[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_3_c[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_3_d[i] = _TH_NUM_EXT_PORTS;
		idb_tdm_ovs_3_e[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_3_f[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_3_g[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_3_h[i] = _TH_NUM_EXT_PORTS;
	}
	for (i=0; i<4; i++) {
		checkpoint[i]=1;
	}
	for (i=0; i<(_TH_NUM_EXT_PORTS-6); i++) {
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("the speed for port %0d is %0d state %0d\n"),
                     i, speed[i], port_state_map[i-1]));
	}
	
	tsc_port_transcription(tsc_array, speed, port_state_map);
	TDM_SML_BAR
	print_tsc(file, tsc_array);

	TDM_BIG_BAR
	LOG_VERBOSE(BSL_LS_SOC_TDM,
                (BSL_META("TDM: _____VERBOSE: the chip frequency is %0d\n"),
                 freq));
	TDM_BIG_BAR
	
	checkpoint[0] = TDM_scheduler_wrap(file, freq, speed, port_state_map, tsc_array, pm_encap_type, 1, 32, idb_tdm_tbl_0, idb_tdm_ovs_0_a, idb_tdm_ovs_0_b, idb_tdm_ovs_0_c, idb_tdm_ovs_0_d, idb_tdm_ovs_0_e, idb_tdm_ovs_0_f, idb_tdm_ovs_0_g, idb_tdm_ovs_0_h, mmu_tdm_tbl_0, mmu_tdm_ovs_0_a, mmu_tdm_ovs_0_b, mmu_tdm_ovs_0_c, mmu_tdm_ovs_0_d, mmu_tdm_ovs_0_e, mmu_tdm_ovs_0_f, mmu_tdm_ovs_0_g, mmu_tdm_ovs_0_h);
	parse_tdm_tbl(file, mgmtbw, idb_tdm_tbl_0, idb_tdm_ovs_0_a, idb_tdm_ovs_0_b, idb_tdm_ovs_0_c, idb_tdm_ovs_0_d, idb_tdm_ovs_0_e, idb_tdm_ovs_0_f, idb_tdm_ovs_0_g, idb_tdm_ovs_0_h, 0, 0);
	parse_tdm_tbl(file, mgmtbw, mmu_tdm_tbl_0, mmu_tdm_ovs_0_a, mmu_tdm_ovs_0_b, mmu_tdm_ovs_0_c, mmu_tdm_ovs_0_d, mmu_tdm_ovs_0_e, mmu_tdm_ovs_0_f, mmu_tdm_ovs_0_g, mmu_tdm_ovs_0_h, 1, 0);	
	TDM_BIG_BAR
	checkpoint[1] = TDM_scheduler_wrap(file, freq, speed, port_state_map, tsc_array, pm_encap_type, 33, 64, idb_tdm_tbl_1, idb_tdm_ovs_1_a, idb_tdm_ovs_1_b, idb_tdm_ovs_1_c, idb_tdm_ovs_1_d, idb_tdm_ovs_1_e, idb_tdm_ovs_1_f, idb_tdm_ovs_1_g, idb_tdm_ovs_1_h, mmu_tdm_tbl_1, mmu_tdm_ovs_1_a, mmu_tdm_ovs_1_b, mmu_tdm_ovs_1_c, mmu_tdm_ovs_1_d, mmu_tdm_ovs_1_e, mmu_tdm_ovs_1_f, mmu_tdm_ovs_1_g, mmu_tdm_ovs_1_h);
	parse_tdm_tbl(file, mgmtbw, idb_tdm_tbl_1, idb_tdm_ovs_1_a, idb_tdm_ovs_1_b, idb_tdm_ovs_1_c, idb_tdm_ovs_1_d, idb_tdm_ovs_1_e, idb_tdm_ovs_1_f, idb_tdm_ovs_1_g, idb_tdm_ovs_1_h, 0, 1);
	parse_tdm_tbl(file, mgmtbw, mmu_tdm_tbl_1, mmu_tdm_ovs_1_a, mmu_tdm_ovs_1_b, mmu_tdm_ovs_1_c, mmu_tdm_ovs_1_d, mmu_tdm_ovs_1_e, mmu_tdm_ovs_1_f, mmu_tdm_ovs_1_g, mmu_tdm_ovs_1_h, 1, 1);	
	TDM_BIG_BAR
	checkpoint[2] = TDM_scheduler_wrap(file, freq, speed, port_state_map, tsc_array, pm_encap_type, 65, 96, idb_tdm_tbl_2, idb_tdm_ovs_2_a, idb_tdm_ovs_2_b, idb_tdm_ovs_2_c, idb_tdm_ovs_2_d, idb_tdm_ovs_2_e, idb_tdm_ovs_2_f, idb_tdm_ovs_2_g, idb_tdm_ovs_2_h, mmu_tdm_tbl_2, mmu_tdm_ovs_2_a, mmu_tdm_ovs_2_b, mmu_tdm_ovs_2_c, mmu_tdm_ovs_2_d, mmu_tdm_ovs_2_e, mmu_tdm_ovs_2_f, mmu_tdm_ovs_2_g, mmu_tdm_ovs_2_h);
	parse_tdm_tbl(file, mgmtbw, idb_tdm_tbl_2, idb_tdm_ovs_2_a, idb_tdm_ovs_2_b, idb_tdm_ovs_2_c, idb_tdm_ovs_2_d, idb_tdm_ovs_2_e, idb_tdm_ovs_2_f, idb_tdm_ovs_2_g, idb_tdm_ovs_2_h, 0, 2);
	parse_tdm_tbl(file, mgmtbw, mmu_tdm_tbl_2, mmu_tdm_ovs_2_a, mmu_tdm_ovs_2_b, mmu_tdm_ovs_2_c, mmu_tdm_ovs_2_d, mmu_tdm_ovs_2_e, mmu_tdm_ovs_2_f, mmu_tdm_ovs_2_g, mmu_tdm_ovs_2_h, 1, 2);	
	TDM_BIG_BAR
	checkpoint[3] = TDM_scheduler_wrap(file, freq, speed, port_state_map, tsc_array, pm_encap_type, 97, 128, idb_tdm_tbl_3, idb_tdm_ovs_3_a, idb_tdm_ovs_3_b, idb_tdm_ovs_3_c, idb_tdm_ovs_3_d, idb_tdm_ovs_3_e, idb_tdm_ovs_3_f, idb_tdm_ovs_3_g, idb_tdm_ovs_3_h, mmu_tdm_tbl_3, mmu_tdm_ovs_3_a, mmu_tdm_ovs_3_b, mmu_tdm_ovs_3_c, mmu_tdm_ovs_3_d, mmu_tdm_ovs_3_e, mmu_tdm_ovs_3_f, mmu_tdm_ovs_3_g, mmu_tdm_ovs_3_h);	
	parse_tdm_tbl(file, mgmtbw, idb_tdm_tbl_3, idb_tdm_ovs_3_a, idb_tdm_ovs_3_b, idb_tdm_ovs_3_c, idb_tdm_ovs_3_d, idb_tdm_ovs_3_e, idb_tdm_ovs_3_f, idb_tdm_ovs_3_g, idb_tdm_ovs_3_h, 0, 3);
	parse_tdm_tbl(file, mgmtbw, mmu_tdm_tbl_3, mmu_tdm_ovs_3_a, mmu_tdm_ovs_3_b, mmu_tdm_ovs_3_c, mmu_tdm_ovs_3_d, mmu_tdm_ovs_3_e, mmu_tdm_ovs_3_f, mmu_tdm_ovs_3_g, mmu_tdm_ovs_3_h, 1, 3);	
	TDM_BIG_BAR

	if (checkpoint[0]==0||checkpoint[1]==0||checkpoint[2]==0||checkpoint[3]==0) {
		if (checkpoint[0]==0) {
            LOG_ERROR(BSL_LS_SOC_TDM,
                      (BSL_META("Pipe 0 failed to schedule\n")));
        }
		if (checkpoint[1]==0) {
            LOG_ERROR(BSL_LS_SOC_TDM,
                      (BSL_META("Pipe 1 failed to schedule\n")));
        }
		if (checkpoint[2]==0) {
            LOG_ERROR(BSL_LS_SOC_TDM,
                      (BSL_META("Pipe 2 failed to schedule\n")));
        }
		if (checkpoint[3]==0) {
            LOG_ERROR(BSL_LS_SOC_TDM,
                      (BSL_META("Pipe 3 failed to schedule\n")));
        }
		#ifdef _SET_TDM_DUMP
		fclose(file);
		#endif
		return 0;
	}
	else {
		#ifdef _SET_TDM_DUMP
		fclose(file);
		#endif
		return 1;
	}
	
}


/**
@name: set_tdm_tbl_tdm56
@param:

SV interfaced entry point for hard coded TDM 56.
 **/
int set_tdm_tbl_tdm56(enum port_speed speed[_TH_NUM_EXT_PORTS], int tdm_bw,
	int idb_tdm_tbl_0[256], int idb_tdm_ovs_0_a[12], int idb_tdm_ovs_0_b[12], int idb_tdm_ovs_0_c[12], int idb_tdm_ovs_0_d[12],
	int idb_tdm_tbl_1[256], int idb_tdm_ovs_1_a[12], int idb_tdm_ovs_1_b[12], int idb_tdm_ovs_1_c[12], int idb_tdm_ovs_1_d[12],
	int idb_tdm_tbl_2[256], int idb_tdm_ovs_2_a[12], int idb_tdm_ovs_2_b[12], int idb_tdm_ovs_2_c[12], int idb_tdm_ovs_2_d[12],
	int idb_tdm_tbl_3[256], int idb_tdm_ovs_3_a[12], int idb_tdm_ovs_3_b[12], int idb_tdm_ovs_3_c[12], int idb_tdm_ovs_3_d[12],
	int mmu_tdm_tbl_0[256], int mmu_tdm_ovs_0_a[12], int mmu_tdm_ovs_0_b[12], int mmu_tdm_ovs_0_c[12], int mmu_tdm_ovs_0_d[12],
	int mmu_tdm_tbl_1[256], int mmu_tdm_ovs_1_a[12], int mmu_tdm_ovs_1_b[12], int mmu_tdm_ovs_1_c[12], int mmu_tdm_ovs_1_d[12],
	int mmu_tdm_tbl_2[256], int mmu_tdm_ovs_2_a[12], int mmu_tdm_ovs_2_b[12], int mmu_tdm_ovs_2_c[12], int mmu_tdm_ovs_2_d[12],
	int mmu_tdm_tbl_3[256], int mmu_tdm_ovs_3_a[12], int mmu_tdm_ovs_3_b[12], int mmu_tdm_ovs_3_c[12], int mmu_tdm_ovs_3_d[12],
	int port_state_map[128])
{
	/* int wc_array[NUM_TSC][4], mgmtbw=0, i, j, p, q=0, k=0, checkpoint[6]; */
	int i=0, j, k=0;
	#ifdef _SET_TDM_DUMP
	FILE *file;
	remove(LOG);
	file = fopen(LOG,"a");
	#endif	

	/* Prevent compiler errors */
	/* int pgw_tdm_tbl_x0[32], ovs_tdm_tbl_x0[32], ovs_spacing_x0[32], pgw_tdm_tbl_x1[32], ovs_tdm_tbl_x1[32], ovs_spacing_x1[32], pgw_tdm_tbl_y0[32], ovs_tdm_tbl_y0[32], ovs_spacing_y0[32], pgw_tdm_tbl_y1[32], ovs_tdm_tbl_y1[32], ovs_spacing_y1[32], iarb_tdm_tbl_x[512], iarb_tdm_tbl_y[512]; */
	
		/* Hard coded 24x100G configuration
			W Pipe (0) Ports - 1, 5, 9, 13 + 25 + 29 + 0 + 132
			X Pipe (1) Ports - 33, 37, 41, 45 + 53 + 57 + 129 + 133
			Y Pipe (2) Ports - 65, 69, 73, 77 + 81 + 85 + 131 + 134
			Z Pipe (3) Ports - 97, 101, 105, 109 + 113 + 125 + 130 + 135 */
			
		LOG_VERBOSE(BSL_LS_SOC_TDM,
                    (BSL_META("TDM: _____VERBOSE: TDM 56 Hard Coded 24x100G Configuration\n\tPipe 0 Ports - 1, 5, 9, 13 + 0 + 132\n\tPipe 1 Ports - 33, 37, 41, 45 + 129 + 133\n\tPipe 2 Ports - 65, 69, 73, 77 + 131 + 134\n\tPipe 3 Ports - 97, 101, 105, 109 + 130 + 135\n")));

		for (i=0; i<12; i++) {
			mmu_tdm_ovs_0_a[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_0_b[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_0_c[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_0_d[i] = _TH_NUM_EXT_PORTS;
			mmu_tdm_ovs_1_a[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_1_b[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_1_c[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_1_d[i] = _TH_NUM_EXT_PORTS;
			mmu_tdm_ovs_2_a[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_2_b[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_2_c[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_2_d[i] = _TH_NUM_EXT_PORTS;
			mmu_tdm_ovs_3_a[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_3_b[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_3_c[i] = _TH_NUM_EXT_PORTS; mmu_tdm_ovs_3_d[i] = _TH_NUM_EXT_PORTS;
			idb_tdm_ovs_0_a[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_0_b[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_0_c[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_0_d[i] = _TH_NUM_EXT_PORTS;
			idb_tdm_ovs_1_a[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_1_b[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_1_c[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_1_d[i] = _TH_NUM_EXT_PORTS; 
			idb_tdm_ovs_2_a[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_2_b[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_2_c[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_2_d[i] = _TH_NUM_EXT_PORTS;
			idb_tdm_ovs_3_a[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_3_b[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_3_c[i] = _TH_NUM_EXT_PORTS; idb_tdm_ovs_3_d[i] = _TH_NUM_EXT_PORTS;
		}

		j=40; for (i=0; i<256; i+=6) {if (j>0) mmu_tdm_tbl_0[i]=1; j--;}
		j=40; for (i=1; i<256; i+=6) {if (j>0) mmu_tdm_tbl_0[i]=5; j--;}
		j=40; for (i=2; i<256; i+=6) {if (j>0) mmu_tdm_tbl_0[i]=9; j--;}
		j=40; for (i=3; i<256; i+=6) {if (j>0) mmu_tdm_tbl_0[i]=13; j--;}
		j=40; for (i=4; i<256; i+=6) {if (j>0) mmu_tdm_tbl_0[i]=25; j--;}
		j=40; for (i=5; i<256; i+=6) {if (j>0) mmu_tdm_tbl_0[i]=29; j--;}
				
		j=40; for (i=0; i<256; i+=6) {if (j>0) mmu_tdm_tbl_1[i]=33; j--;}
		j=40; for (i=1; i<256; i+=6) {if (j>0) mmu_tdm_tbl_1[i]=37; j--;}
		j=40; for (i=2; i<256; i+=6) {if (j>0) mmu_tdm_tbl_1[i]=41; j--;}
		j=40; for (i=3; i<256; i+=6) {if (j>0) mmu_tdm_tbl_1[i]=45; j--;}
		j=40; for (i=4; i<256; i+=6) {if (j>0) mmu_tdm_tbl_1[i]=53; j--;}
		j=40; for (i=5; i<256; i+=6) {if (j>0) mmu_tdm_tbl_1[i]=57; j--;}
						
		j=40; for (i=0; i<256; i+=6) {if (j>0) mmu_tdm_tbl_2[i]=65; j--;}
		j=40; for (i=1; i<256; i+=6) {if (j>0) mmu_tdm_tbl_2[i]=69; j--;}
		j=40; for (i=2; i<256; i+=6) {if (j>0) mmu_tdm_tbl_2[i]=73; j--;}
		j=40; for (i=3; i<256; i+=6) {if (j>0) mmu_tdm_tbl_2[i]=77; j--;}
		j=40; for (i=4; i<256; i+=6) {if (j>0) mmu_tdm_tbl_2[i]=81; j--;}
		j=40; for (i=5; i<256; i+=6) {if (j>0) mmu_tdm_tbl_2[i]=85; j--;}
						
		j=40; for (i=0; i<256; i+=6) {if (j>0) mmu_tdm_tbl_3[i]=97; j--;}
		j=40; for (i=1; i<256; i+=6) {if (j>0) mmu_tdm_tbl_3[i]=101; j--;}
		j=40; for (i=2; i<256; i+=6) {if (j>0) mmu_tdm_tbl_3[i]=105; j--;}
		j=40; for (i=3; i<256; i+=6) {if (j>0) mmu_tdm_tbl_3[i]=109; j--;}
		j=40; for (i=4; i<256; i+=6) {if (j>0) mmu_tdm_tbl_3[i]=113; j--;}
		j=40; for (i=5; i<256; i+=6) {if (j>0) mmu_tdm_tbl_3[i]=125; j--;}
		
		/* Insert tokens */
		k = 0;
		for (i=0; i<256; i++) {
			if ((i+1)%25==0) {
				for (j=255; j>i; j--) {mmu_tdm_tbl_0[j]=mmu_tdm_tbl_0[j-1];}
				for (j=255; j>i; j--) {mmu_tdm_tbl_1[j]=mmu_tdm_tbl_1[j-1];}
				for (j=255; j>i; j--) {mmu_tdm_tbl_2[j]=mmu_tdm_tbl_2[j-1];}
				for (j=255; j>i; j--) {mmu_tdm_tbl_3[j]=mmu_tdm_tbl_3[j-1];}
				switch (k) {
				   case 0:
				   case 2:
				   case 5:
				   case 7:
					   mmu_tdm_tbl_0[i]=0;
					   mmu_tdm_tbl_1[i]=129;
					   mmu_tdm_tbl_2[i]=131;
					   mmu_tdm_tbl_3[i]=IDL1_TOKEN;
					   break;
				   case 1:
				   case 3:
				   case 6:
				   case 8:
					   mmu_tdm_tbl_0[i]=IDL1_TOKEN;
					   mmu_tdm_tbl_1[i]=IDL1_TOKEN;
					   mmu_tdm_tbl_2[i]=IDL1_TOKEN;
					   mmu_tdm_tbl_3[i]=IDL1_TOKEN;
					   break;
				   case 4:
				   case 9:
					   mmu_tdm_tbl_0[i]=132;
					   mmu_tdm_tbl_1[i]=133;
					   mmu_tdm_tbl_2[i]=134;
					   mmu_tdm_tbl_3[i]=135;
						break;
				   default:
						break;
				}
				k++;
			}
		}
						
		j=40; for (i=0; i<256; i+=6) {if (j>0) idb_tdm_tbl_0[i]=1; j--;}
		j=40; for (i=1; i<256; i+=6) {if (j>0) idb_tdm_tbl_0[i]=5; j--;}
		j=40; for (i=2; i<256; i+=6) {if (j>0) idb_tdm_tbl_0[i]=9; j--;}
		j=40; for (i=3; i<256; i+=6) {if (j>0) idb_tdm_tbl_0[i]=13; j--;}
		j=40; for (i=4; i<256; i+=6) {if (j>0) idb_tdm_tbl_0[i]=25; j--;}
		j=40; for (i=5; i<256; i+=6) {if (j>0) idb_tdm_tbl_0[i]=29; j--;}
						
		j=40; for (i=0; i<256; i+=6) {if (j>0) idb_tdm_tbl_1[i]=33; j--;}
		j=40; for (i=1; i<256; i+=6) {if (j>0) idb_tdm_tbl_1[i]=37; j--;}
		j=40; for (i=2; i<256; i+=6) {if (j>0) idb_tdm_tbl_1[i]=41; j--;}
		j=40; for (i=3; i<256; i+=6) {if (j>0) idb_tdm_tbl_1[i]=45; j--;}
		j=40; for (i=4; i<256; i+=6) {if (j>0) idb_tdm_tbl_1[i]=53; j--;}
		j=40; for (i=5; i<256; i+=6) {if (j>0) idb_tdm_tbl_1[i]=57; j--;}
						
		j=40; for (i=0; i<256; i+=6) {if (j>0) idb_tdm_tbl_2[i]=65; j--;}
		j=40; for (i=1; i<256; i+=6) {if (j>0) idb_tdm_tbl_2[i]=69; j--;}
		j=40; for (i=2; i<256; i+=6) {if (j>0) idb_tdm_tbl_2[i]=73; j--;}
		j=40; for (i=3; i<256; i+=6) {if (j>0) idb_tdm_tbl_2[i]=77; j--;}
		j=40; for (i=4; i<256; i+=6) {if (j>0) idb_tdm_tbl_2[i]=81; j--;}
		j=40; for (i=5; i<256; i+=6) {if (j>0) idb_tdm_tbl_2[i]=85; j--;}
		
		j=40; for (i=0; i<256; i+=6) {if (j>0) idb_tdm_tbl_3[i]=97; j--;}
		j=40; for (i=1; i<256; i+=6) {if (j>0) idb_tdm_tbl_3[i]=101; j--;}
		j=40; for (i=2; i<256; i+=6) {if (j>0) idb_tdm_tbl_3[i]=105; j--;}
		j=40; for (i=3; i<256; i+=6) {if (j>0) idb_tdm_tbl_3[i]=109; j--;}
		j=40; for (i=4; i<256; i+=6) {if (j>0) idb_tdm_tbl_3[i]=113; j--;}
		j=40; for (i=5; i<256; i+=6) {if (j>0) idb_tdm_tbl_3[i]=125; j--;}
						
                k = 0;
		for (i=0; i<256; i++) {
			if ((i+1)%25==0) {
			    	for (j=255; j>i; j--) {idb_tdm_tbl_0[j]=idb_tdm_tbl_0[j-1];}
			    	for (j=255; j>i; j--) {idb_tdm_tbl_1[j]=idb_tdm_tbl_1[j-1];}
			    	for (j=255; j>i; j--) {idb_tdm_tbl_2[j]=idb_tdm_tbl_2[j-1];}
			    	for (j=255; j>i; j--) {idb_tdm_tbl_3[j]=idb_tdm_tbl_3[j-1];}
			        switch (k) {
				   case 0:
				   case 2:
				   case 5:
				   case 7:
					   idb_tdm_tbl_0[i]=0;
					   idb_tdm_tbl_1[i]=129;
					   idb_tdm_tbl_2[i]=131;
					   idb_tdm_tbl_3[i]=IDL1_TOKEN;
					   break;
				   case 1:
				   case 3:
				   case 6:
				   case 8:
					   idb_tdm_tbl_0[i]=IDL1_TOKEN;
					   idb_tdm_tbl_1[i]=IDL1_TOKEN;
					   idb_tdm_tbl_2[i]=IDL1_TOKEN;
					   idb_tdm_tbl_3[i]=IDL1_TOKEN;
					   break;
				   case 4:
				   case 9:
					   idb_tdm_tbl_0[i]=132;
					   idb_tdm_tbl_1[i]=133;
					   idb_tdm_tbl_2[i]=134;
					   idb_tdm_tbl_3[i]=135;
                                           break;
				   default:
                                           break;
			        }
                            	k++;
                       }
		}
		
		for (i=250; i<256; i++) {
			idb_tdm_tbl_0[i]=_TH_NUM_EXT_PORTS;
			idb_tdm_tbl_1[i]=_TH_NUM_EXT_PORTS;
			idb_tdm_tbl_2[i]=_TH_NUM_EXT_PORTS;
			idb_tdm_tbl_3[i]=_TH_NUM_EXT_PORTS;
			mmu_tdm_tbl_0[i]=_TH_NUM_EXT_PORTS;
			mmu_tdm_tbl_1[i]=_TH_NUM_EXT_PORTS;
			mmu_tdm_tbl_2[i]=_TH_NUM_EXT_PORTS;
			mmu_tdm_tbl_3[i]=_TH_NUM_EXT_PORTS;			
		}

	
	return 1;
	
}
