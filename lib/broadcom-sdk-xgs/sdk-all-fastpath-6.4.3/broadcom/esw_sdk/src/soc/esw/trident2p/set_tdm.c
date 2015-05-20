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

#include <soc/tdm_trident2plus.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <sal/core/alloc.h>
typedef int FILE;
#define TDM_VERBOSE0(a) LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META("%s"),a))
#define TDM_VERBOSE(_frmt,a) LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(_frmt),a))
#ifndef TDM_PREPROCESSOR_DIRECTIVES
#define TDM_PREPROCESSOR_DIRECTIVES






/*  No variadic macros */

	#define TDM_ERROR8(a,b,c,d,e,f,g,h,i) LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h,i)) 
	#define TDM_ERROR7(a,b,c,d,e,f,g,h)   LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h))
	#define TDM_ERROR6(a,b,c,d,e,f,g) 	  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
	#define TDM_ERROR5(a,b,c,d,e,f) 	  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
	#define TDM_ERROR4(a,b,c,d,e) 		  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
	#define TDM_ERROR3(a,b,c,d) 		  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
	#define TDM_ERROR2(a,b,c) 			  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
	#define TDM_ERROR1(a,b) 			  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a),b))
	#define TDM_ERROR0(a) 				  LOG_ERROR(BSL_LS_SOC_TDM, (BSL_META(a)))
	#define TDM_WARN6(a,b,c,d,e,f,g) LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
	#define TDM_WARN5(a,b,c,d,e,f)   LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
	#define TDM_WARN4(a,b,c,d,e)     LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
	#define TDM_WARN3(a,b,c,d)       LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
	#define TDM_WARN2(a,b,c)         LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
	#define TDM_WARN1(a,b)           LOG_WARN(BSL_LS_SOC_TDM, (BSL_META(a),b))
	#define TDM_WARN0(a)             LOG_WARN(BSL_LS_SOC_TDM, (BSL_META("%s"),a))
	#define TDM_VERBOSE9(a,b,c,d,e,f,g,h,i,j) LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h,i,j))
	#define TDM_VERBOSE8(a,b,c,d,e,f,g,h,i)   LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h,i))
	#define TDM_VERBOSE7(a,b,c,d,e,f,g,h)     LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h))
	#define TDM_VERBOSE6(a,b,c,d,e,f,g)       LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
	#define TDM_VERBOSE5(a,b,c,d,e,f)         LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
	#define TDM_VERBOSE4(a,b,c,d,e)           LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
	#define TDM_VERBOSE3(a,b,c,d)             LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
	#define TDM_VERBOSE2(a,b,c)               LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
	#define TDM_VERBOSE1(a,b)                 LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b))
	#define TDM_BIG_BAR TDM_VERBOSE0(("TDM: #################################################################################################################################\n"));
	#define TDM_SML_BAR TDM_VERBOSE0(("TDM: ---------------------------------------------------------------------------------------------------------------------------------\n"));
	#define TDM_PRINT8(a,b,c,d,e,f,g,h,i) LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h,i))
	#define TDM_PRINT7(a,b,c,d,e,f,g,h)   LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g,h))
	#define TDM_PRINT6(a,b,c,d,e,f,g)     LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f,g))
	#define TDM_PRINT5(a,b,c,d,e,f)       LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e,f))
	#define TDM_PRINT4(a,b,c,d,e)         LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d,e))
	#define TDM_PRINT3(a,b,c,d)           LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c,d))
	#define TDM_PRINT2(a,b,c)             LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b,c))
	#define TDM_PRINT1(a,b)               LOG_VERBOSE(BSL_LS_SOC_TDM, (BSL_META(a),b))
	#define TDM_PRINT0(a)                 TDM_VERBOSE0((a))
	#define TDM_DEBUG TDM_VERBOSE0(("--- DEBUG ---\n"));

#ifdef _STD_ALLOC

	#define TDM_ALLOC(a,b) malloc(a)
	#define TDM_FREE(a) free(a)
	#define TDM_COPY(dest,src,len) memcpy(dest,src,len)
#else
	#define TDM_ALLOC(_sz,_id) sal_alloc(_sz,_id)
	#define TDM_FREE(_id) sal_free(_id)
	#define TDM_COPY(_dst,_src,_len) sal_memcpy(_dst,_src_len)
#endif



/* Minimum freq in MHz at which all HG speeds are supported */
#define MIN_HG_FREQ 545

/* Max clk frquency in MHz */
#define MAX_FREQ 850

/* Granularity of TDM in 100MBps */
#define BW_QUANTA 25

/* Max size between speed vectors that can be the same, i.e. 40G vs 42G */
#define SPEED_DELIMITER 2000

/* LLS min spacing requirement */
/* Set this to total length in order to disable enforcement */
#define LLS_MIN_SPACING 11
#define VBS_MIN_SPACING 4

/* Amount to relax the check of oversub token smoothness */
#define OS_RANGE_MARGIN 0
/* uArch range for oversub smoothness as ratio with line rate slots */
#define OS_RANGE_BW_HI 1 /* OS>=200G */
#define OS_RANGE_BW_MD 2 /* OS<200G and OS>=100G */
/* #define OS_RANGE_BW_LO (USE SAME VALUE AS LR) */

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
#define LEN_608MHZ_HG 160
#define LEN_608MHZ_EN 160
#define LEN_517MHZ_HG 136
#define LEN_517MHZ_EN 136
#define LEN_415MHZ_HG 106
#define LEN_415MHZ_EN 106

/* Length of TDM vector map - max length of TDM memory */
#define VEC_MAP_LEN 255
/* Width of TDM vector map - max number of vectors to reduce */
#define VEC_MAP_WID 49
/* Scan sees blank row or column */
#define BLANK 12345

/* Port tokens */
#define TOKEN_120G _TD2P_NUM_EXT_PORTS+119
#define TOKEN_106G _TD2P_NUM_EXT_PORTS+106
#define TOKEN_100G _TD2P_NUM_EXT_PORTS+100
#define TOKEN_40G _TD2P_NUM_EXT_PORTS+40
#define TOKEN_50G _TD2P_NUM_EXT_PORTS+50
#define TOKEN_25G _TD2P_NUM_EXT_PORTS+25
#define TOKEN_20G _TD2P_NUM_EXT_PORTS+20
#define TOKEN_12G _TD2P_NUM_EXT_PORTS+12
#define TOKEN_10G _TD2P_NUM_EXT_PORTS+11
#define TOKEN_1G _TD2P_NUM_EXT_PORTS+120

/* Max supported number of oversub speed groups */
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

/* Vector scheduling repititions */
#define MAIN_SCHEDULER_PASSES 5

/* Filter sensitivity */
#define DEFAULT_STEP_SIZE 1
#define DITHER_SUBPASS_STEP_SIZE 2
#define DITHER_THRESHOLD 7
#define DITHER_PASS 10

/* TDMV driver constants */
#define MAX_SPEED_TYPES 8 /* Max number of speeds that can be randomized */

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
/* Default number of ancillary ports in calendar */
#define TD2_ANCILLARY_PORTS 10

/* TSC transcription management port token */
#define MGM_TOKEN (_TD2P_NUM_EXT_PORTS+3)
/* Port scanning microsub token */
#define U1G_TOKEN (_TD2P_NUM_EXT_PORTS+4)
			
/* Excludes the management ports which are also physical port numbers */
#define MGMT_CHECK(a)		\
			if (a!=13 && 	\
			a!=14 && 		\
			a!=15 && 		\
			a!=16) 			\
	
/* MMU table accessory token */
#define ACC_TOKEN (_TD2P_NUM_EXT_PORTS+5)

/* Excludes all tokenized port numbers */
#define TOKEN_CHECK(a)  			\
			if (a!=_TD2P_NUM_EXT_PORTS && \
			a!=OVSB_TOKEN && 		\
			a!=IDL1_TOKEN && 		\
			a!=IDL2_TOKEN && 		\
			a!=ACC_TOKEN &&			\
			a!=MGM_TOKEN && 		\
			a!=U1G_TOKEN && 		\
			a!=0 &&					\
			a!=129) 				\

#define CMIC(a) {				\
			mmu_tdm_tbl[a]=0; 	\
			TDM_VERBOSE0("CMIC\n"); \
			break;			\
		}
#define LPBK(a) {					\
			mmu_tdm_tbl[a]=129; 		\
			TDM_VERBOSE0("LOOPBACK\n"); 	\
			break;				\
		}
#define ANCL(a) {					\
			mmu_tdm_tbl[a]=IDL1_TOKEN; 	\
			TDM_VERBOSE0("OPPORTUNISTIC\n");	\
			break;				\
		}
#define REFR(a) {					\
			mmu_tdm_tbl[a]=IDL2_TOKEN; 	\
			TDM_VERBOSE0("REFRESH\n");	\
			break;				\
		}
#define MM13(a) {						\
			mmu_tdm_tbl[a]=13; 			\
			TDM_VERBOSE0("management phyport 13\n");\
			break;					\
		}
#define MM14(a) {						\
			mmu_tdm_tbl[a]=14; 			\
			TDM_VERBOSE0("management phyport 14\n");\
			break;					\
		}
#define MM15(a) {						\
			mmu_tdm_tbl[a]=15; 			\
			TDM_VERBOSE0("management phyport 15\n");\
			break;					\
		}
#define MM16(a) {						\
			mmu_tdm_tbl[a]=16; 			\
			TDM_VERBOSE0("management phyport 16\n");\
			break;					\
		}

#define IARB_MAIN_TDM__TDM_SLOT_PGW_0         0
#define IARB_MAIN_TDM__TDM_SLOT_PGW_1         1
#define IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT     2
#define IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK   3
#define IARB_MAIN_TDM__TDM_SLOT_QGP_PORT      4
#define IARB_MAIN_TDM__TDM_SLOT_RESERVED      6
#define IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT  7

static int TDM_scheduler_2d(FILE *file, int freq, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int port_state_map[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_buffer[64], int os_buffer[64], int lr_idx_limit, int tdm_tbl[256], int bucket1[_TD2P_OS_GROUP_LEN], int bucket2[_TD2P_OS_GROUP_LEN], int bucket3[_TD2P_OS_GROUP_LEN], int bucket4[_TD2P_OS_GROUP_LEN], int bucket5[_TD2P_OS_GROUP_LEN], int bucket6[_TD2P_OS_GROUP_LEN], int bucket7[_TD2P_OS_GROUP_LEN], int bucket8[_TD2P_OS_GROUP_LEN], int pipe, unsigned char accessories, unsigned char higig_mgmt);
static int tdm_PQ(int f);
static int check_type_2(unsigned char port, int tsc[NUM_TSC][4]);
static int check_fit_smooth(int tdm_tbl[256], int port, int lr_idx_limit, int clump_thresh);
static int check_lls_flat_up(int idx, int tdm_tbl[256], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static int tdm_slice_size_local(unsigned char idx, int tdm[256], int lim);
static int tdm_slice_prox_local(unsigned char idx, int tdm[256], int lim, int tsc[NUM_TSC][4]);
static int tdm_slice_size(unsigned char port, int tdm[256], int lim);
static int tdm_slice_idx(unsigned char port, int tdm[256], int lim);
static int tdm_slice_prox_dn(int slot, int tdm[256], int lim, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static int tdm_slice_prox_up(int slot, int tdm[256], int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
/* static int tdm_arr_exists(int element, int len, int arr[len]); */ /* C90 not supported */
/* static int tdm_arr_append(int element, int len, int arr[len]); */ /* C90 not supported */
static int tdm_arr_exists(int element, int len, int arr[107]);
static int tdm_arr_append(int element, int len, int arr[107]);
static int check_spd(unsigned char **map, int lim, int x_pos, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int round);
static int tdm_fit_prox(unsigned char **map, int lim, int wid, int node_x, int tsc[NUM_TSC][4]);
static int tdm_fit_prox_node(unsigned char **map, int node_y, int wid, int node_x, int tsc[NUM_TSC][4], int new_node_y);
static int tdm_fit_singular_cnt(unsigned char **map, int node_y);
static int tdm_fit_singular_col(unsigned char **map, int node_x, int lim);
static int tdm_map_find_x(unsigned char **map, int limit, int idx, int principle);
static int tdm_map_find_y(unsigned char **map, int limit, int idx, int principle);
static int tdm_map_search_x(unsigned char **map, int idx);
static int tdm_count_nonsingular(unsigned char **map, unsigned char x_idx, int lim);
static int check_fastest_triport(int port, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static unsigned int check_fastest_lane(int port, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static int find_fastest_lane(int sample, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static int tdm_pick_vec(unsigned char **vector_map, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS], int lr_vec_cnt, int tsc_dq, int *triport_priority, int prev_vec);
static void tdm_mem_transpose(unsigned char **map, int src, int dest);
static int tdm_map_retrace_count(unsigned char **map, int x_idx, int y_idx);
static int tdm_map_cadence_count(unsigned char *vector, int idx);
static void tdm_sticky_transpose(unsigned char **map, int lr_idx_limit, int src, int dst);
static int tdm_map_shortest_distance(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2);
static int tdm_map_shortest_distance_calc(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2, unsigned int start);
static int tdm_slots(int port_speed);
static int tdm_vector_diff(unsigned char **map, int bw, int lim, int x, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1]);
static void tdm_vector_dump(FILE *file, unsigned char **map, int idx);
static int tdm_vector_load(unsigned char **map, int bw, int port_speed, int yy, int len);
static void tdm_vector_rotate(unsigned char vector[], int size, int step);
static void tdm_vector_rotate_step(unsigned char vector[], int size, int step);
static void tsc_port_transcription(int wc_array[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS], int port_state_array[_TD2P_NUM_EXT_PORTS]);
static int check_ethernet(int port, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1]);
static void print_tsc(FILE *file, int wc_array[NUM_TSC][4]);
static int tdm_abs(int num);
static int tdm_fac(int num_signed);
static int tdm_pow(int num, int pow);
static int tdm_sqrt(int input_signed);
static int tdm_check_nonsingular(unsigned char **map, unsigned char y_idx);
static int check_same_port_dist_up(int idx, int tdm_tbl[256], int lim);
static int check_same_port_dist_dn(int idx, int tdm_tbl[256], int lim);
static int which_tsc(unsigned char port, int tsc[NUM_TSC][4]);
static int tdm_filter_vector_shift(int tdm_tbl[256], int port, int dir);
static void tdm_filter_dither(int tdm_tbl[256], int lr_idx_limit, int accessories, int tsc[NUM_TSC][4], int threshold, enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static int tdm_filter_fine_dither(int port, int tdm_tbl[256], int lr_idx_limit, int accessories, int tsc[NUM_TSC][4]);
static void tdm_filter_ancillary_smooth(int port, int tdm_tbl[256], int lr_idx_limit, int accessories);
static void tdm_filter_local_slice_up(int port, int tdm_tbl[256], int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static void tdm_filter_local_slice_dn(int port, int tdm_tbl[256], int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static int tdm_filter_slice_dn(int port, int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static int tdm_filter_slice_up(int port, int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS]);
static int tdm_slice_size_2d(unsigned char **map, unsigned char y_idx, int lim);
static int tdm_check_blank(unsigned char **map, unsigned char y_idx);
static void TDM_scheduler_filter_refactor(int tdm_tbl[256], int lr_idx_limit, int lr_vec_cnt);
static void tdm_vector_clear(unsigned char **map, int yy, int len);
/*
* $Id: $
* $Copyright: (c) 2011 Broadcom Corp.
* All Rights Reserved.$
*
* File:        set_tdm.c
* Purpose:     Derive TDM schedules
*/

#ifndef TDM_VECTOR_LIBRARY
#define TDM_VECTOR_LIBRARY
/**
@name: TDM_scheduler_vector_load
@param:

Generate vector at index in TDM vector map
**/
int tdm_vector_load(unsigned char **map, int bw, int port_speed, int yy, int len)
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
/* ################################################################################################################## */
		case 760: /* TD2/+ */
			switch (port_speed)
			{
				case 1:
					map[yy][0] = TOKEN_1G;
					break;
				case 10:
				case 11:
					map[yy][0] = TOKEN_10G;
					map[yy][48] = TOKEN_10G;
					map[yy][96] = TOKEN_10G;
					map[yy][144] = TOKEN_10G;
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
				case 40:
				case 41:
				case 42:
				case 43:
				case 44:
				case 45:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*3)) {
						map[yy][i] = TOKEN_40G;
					}					
					break;
				case 100:
				case 106:
				case 107:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S*6)) {
						map[yy][i] = TOKEN_100G;
						map[yy][i+5] = TOKEN_100G;
						map[yy][i+10] = TOKEN_100G;
						map[yy][i+15] = TOKEN_100G;
						map[yy][i+19] = TOKEN_100G;
					}
					break;
				case 108:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S)) {
						map[yy][i] = TOKEN_100G;
					}
					/*
					map[yy][20] = _TD2P_NUM_EXT_PORTS;
					map[yy][44] = _TD2P_NUM_EXT_PORTS;
					map[yy][68] = _TD2P_NUM_EXT_PORTS;
					map[yy][92] = _TD2P_NUM_EXT_PORTS;
					map[yy][116] = _TD2P_NUM_EXT_PORTS;
					map[yy][140] = _TD2P_NUM_EXT_PORTS;
					map[yy][164] = _TD2P_NUM_EXT_PORTS;
					map[yy][188] = _TD2P_NUM_EXT_PORTS;
					*/
					break;
				case 120:
					for (i=0; i<len; i+=(VECTOR_QUANTA_S)) {
						map[yy][i] = TOKEN_120G;
					}
					break;
				default:
					return 0;
					break;
			}
			break;			
		case 609: /* TD2/+ */
		case 608:
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
						map[yy][108] = TOKEN_20G;
						map[yy][18] = TOKEN_20G;
						map[yy][54] = TOKEN_20G;
						map[yy][90] = TOKEN_20G;
						map[yy][126] = TOKEN_20G;
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
					default:
						return 0;
						break;
			}
			break;
		case 518: /* TD2/+ */
		case 517: case 537:
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
		case 415: /* TD2/+ */
		case 416:
			switch (port_speed)
			{
					case 1:
						map[yy][0] = TOKEN_1G;
						break;
					case 10:
					case 11:
						map[yy][0] = TOKEN_10G;
						map[yy][24] = TOKEN_10G;
						map[yy][48] = TOKEN_10G;
						map[yy][72] = TOKEN_10G;
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
						map[yy][12] = TOKEN_20G;
						map[yy][24] = TOKEN_20G;
						map[yy][36] = TOKEN_20G;
						map[yy][48] = TOKEN_20G;
						map[yy][60] = TOKEN_20G;
						map[yy][72] = TOKEN_20G;
						map[yy][84] = TOKEN_20G;						
						break;					
					case 40:
					case 41:
					case 42:
					case 43:
					case 44:
					case 45:
						map[yy][0] = TOKEN_40G;
						map[yy][6] = TOKEN_40G;
						map[yy][12] = TOKEN_40G;
						map[yy][18] = TOKEN_40G;
						map[yy][24] = TOKEN_40G;
						map[yy][30] = TOKEN_40G;
						map[yy][36] = TOKEN_40G;
						map[yy][42] = TOKEN_40G;
						map[yy][48] = TOKEN_40G;
						map[yy][54] = TOKEN_40G;
						map[yy][60] = TOKEN_40G;
						map[yy][66] = TOKEN_40G;
						map[yy][72] = TOKEN_40G;
						map[yy][78] = TOKEN_40G;
						map[yy][84] = TOKEN_40G;
						map[yy][90] = TOKEN_40G;						
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
/*
* $Id: $
* $Copyright: (c) 2011 Broadcom Corp.
* All Rights Reserved.$
*
* File:        set_tdm.c
* Purpose:     Derive TDM schedules
*/

/* -I  $(VCS_HOME)/include  */

struct node {
	unsigned char port;
	struct node *next;
}*tdm_ll;


/**
@name: tdm_ver
@param: int[]

Populates manually annotated version number of C code
**/
void tdm_ver(int ver[2]) {
	/* P4 */
	ver[0] = 27;
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
@name: tdm_ll_append
@param:

Appends to TDM linked list
**/
void tdm_ll_append(unsigned char port_append)
{
	struct node *mbox, *tail;
	
	mbox = (struct node *) TDM_ALLOC(sizeof(struct node),"node");
	tail = (struct node *) tdm_ll;
	
	mbox->port=port_append;
	mbox->next=NULL;
	
	if (tdm_ll) {
		while (tail->next!=NULL) {
			tail=tail->next;
		}
	}
	
	if (tail) {
		tail->next=mbox;
		tail=mbox;
		tail->next=NULL;
	}
	else {
		tdm_ll=mbox;
	}
}


/**
@name: tdm_ll_insert
@param:

Inserts to TDM linked list
**/
void tdm_ll_insert(unsigned char port_insert, int idx)
{
	int i;
	struct node *mbox, *right, *left;
	
	right = tdm_ll;
	left = NULL;
	if (right==NULL) {
		tdm_ll_append(port_insert);
	}
	else {
		for (i=1; i<=idx; i++) {
			left=right;
			right=right->next;
		}
		mbox = (struct node *) TDM_ALLOC(sizeof(struct node),"node");
		mbox->port=port_insert;
		left->next=mbox;
		left=mbox;
		left->next=right;
	}
}


/**
@name: tdm_ll_delete
@param:

Delete element from TDM linked list
**/
int tdm_ll_delete(int idx)
{
	int i;
	struct node *mbox, *last;
	
	mbox = tdm_ll;
	last = NULL;
	for (i=1; i<=idx; i++) {
		last=mbox;
		mbox=mbox->next;
	}
	if (mbox==tdm_ll) {
		tdm_ll=mbox->next;
		TDM_FREE(mbox);
		return PASS;
	}
	else {
		last->next=mbox->next;
		TDM_FREE(mbox);
		
		return PASS;;
	}
	
	return FAIL;
	
}


/**
@name: tdm_ll_print
@param:

Prints elements from TDM linked list
**/
void tdm_ll_print(struct node *list)
{
	int i=0;

	list=tdm_ll;
	
	if (list!=NULL) {
		TDM_PRINT0("\n");
		TDM_PRINT0("\tTDM linked list content: \n\t\t");
		while(list!=NULL) {
			TDM_PRINT1("[%03d]",list->port);
			list=list->next;
			if ((++i)%10==0) {
				TDM_PRINT0("->\n\t\t");
			}
			else {
				TDM_PRINT0("->");
			}
		}
		TDM_PRINT0("[*]\n\n\n");
	}
	else {
		TDM_ERROR0("\nUnable to parse TDM linked list for print\n");
	}
}


/**
@name: tdm_ll_deref
@param:

Dereferences entire TDM linked list into unlinked array
**/
void tdm_ll_deref(struct node *list, int tdm[256], int lim)
{
	int i=0;

	list=tdm_ll;
	
	if (list!=NULL) {
		while ( (list!=NULL) && (i<lim) ) {
			tdm[i++]=list->port;
			list=list->next;
		}
	}
	else {
		TDM_ERROR0("Unable to parse TDM linked list for deref\n");
	}
}


/**
@name: tdm_ll_get
@param:

Returns content of node at index
**/
int tdm_ll_get(struct node *list, int idx)
{
	int i;
	
	list=tdm_ll;
	
	if (list!=NULL) {
		for (i=0; ((i<idx) && (list!=NULL)); i++) {
			list=list->next;
		}
	}
	
	if (list!=NULL) {
		return list->port;
	}
	else {
		return _TD2P_NUM_EXT_PORTS;
	}
	
}


/**
@name: tdm_ll_dist
@param:

Returns distance within TDM linked list between one node and its nearest identical node
**/
int tdm_ll_dist(struct node *list, int idx)
{
	int i, dist=0;
	unsigned char store_port;
	
	list=tdm_ll;
	
	if (list!=NULL) {
		for (i=0; ((i<idx) && (list!=NULL)); i++) {
			list=list->next;
		}
		store_port=(list!=NULL)?(list->port):(255);
	}
	
	while (list!=NULL) {
		list=list->next;
		dist++;
		if ((list->port)==store_port) {
			break;
		}
	}
	
	return dist;
	
}


/**
@name: tdm_ll_tsc_dist
@param:

Returns distance within TDM linked list between one node and its nearest sister node
**/
int tdm_ll_tsc_dist(struct node *list, int tsc[NUM_TSC][4], int idx)
{
	int i, dist=0;
	unsigned char store_port;
	
	list=tdm_ll;
	
	if (list!=NULL) {
		for (i=0; ((i<idx) && (list!=NULL)); i++) {
			list=list->next;
		}
		store_port=(list!=NULL)?(list->port):(255);
	}
	
	while (list!=NULL) {
		list=list->next;
		dist++;
		if (which_tsc((list->port),tsc)==which_tsc(store_port,tsc)) {
			break;
		}
	}
	
	return dist;
	
}


/**
@name: tdm_ll_strip
@param:

Strips all of one tokenized value from TDM linked list
**/
void tdm_ll_strip(struct node *list, int cadence_start_idx, int *pool, int *s_idx)
{
	int i;
	unsigned char start_port=_TD2P_NUM_EXT_PORTS;
	
	list=tdm_ll;

	if ( list!=NULL ) {
		for (i=0; ((i<cadence_start_idx) && (list!=NULL)); i++) {
			list=list->next;
		}
		start_port=(list!=NULL)?(list->port):(255);
	}
	
	while (list!=NULL) {
		if ( ((*s_idx)>cadence_start_idx) && ((list->port)==start_port) ) {
			break;
		}
		else if ((list->port)==_TD2P_NUM_EXT_PORTS) {
			tdm_ll_delete((*s_idx));
			(*s_idx)--;
			(*pool)++;
		}
		list=list->next;
		(*s_idx)++;
	}

}


/**
@name: tdm_ll_retrace
@param:

Retraces all null slots in TDM linked list inside specified cadence
Enforces both VBS and LLS min constraints
**/
int tdm_ll_retrace(struct node *list, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int cadence_start_idx)
{
	int i, store_idx=cadence_start_idx, oversub_pool=0, starting_dist, ending_dist, relinked, num_ins=0, cadence_step;
	
	list=tdm_ll;
	starting_dist=tdm_ll_dist(list,cadence_start_idx);
	
	tdm_ll_strip(list,cadence_start_idx,&oversub_pool,&store_idx);	
	
	if (oversub_pool>0) {
		if (tdm_ll_dist(list,cadence_start_idx)>oversub_pool) {
			cadence_step=tdm_ll_dist(list,cadence_start_idx)/oversub_pool;
			for (i=(store_idx-1+num_ins); i>cadence_start_idx; i-=cadence_step) {
				if (tdm_ll_get(list,i)!=_TD2P_NUM_EXT_PORTS) {
					tdm_ll_insert(_TD2P_NUM_EXT_PORTS,(i+1));
					oversub_pool--;
				}
			}
		}
		else {
			for (i=(store_idx-1+num_ins); i>cadence_start_idx; i--) {
				if (tdm_ll_get(list,i)!=_TD2P_NUM_EXT_PORTS) {
					if ( (tdm_ll_dist(list,i)<LLS_MIN_SPACING) ||
						 (tdm_ll_tsc_dist(list,tsc,i)<VBS_MIN_SPACING) ) {
						break;
					}
				}
			}
			while (oversub_pool>0) {
				relinked=BOOL_FALSE;
				for (i=(store_idx-1+num_ins); i>cadence_start_idx; i--) {
					if (tdm_ll_get(list,i)!=_TD2P_NUM_EXT_PORTS) {
						if ( (tdm_ll_dist(list,i)<LLS_MIN_SPACING) ||
							 (tdm_ll_tsc_dist(list,tsc,i)<VBS_MIN_SPACING) ) {
							tdm_ll_insert(_TD2P_NUM_EXT_PORTS,(i+1));
							relinked=BOOL_TRUE; oversub_pool--; num_ins++;
							break;
						}
					}
				}
				if (!relinked) {
					for (i=(cadence_start_idx+1); i<tdm_ll_dist(list,cadence_start_idx); i++) {
						if (tdm_ll_get(list,i)!=_TD2P_NUM_EXT_PORTS && tdm_ll_get(list,(i-1))!=_TD2P_NUM_EXT_PORTS) {
							tdm_ll_insert(_TD2P_NUM_EXT_PORTS,i);
							relinked=BOOL_TRUE; oversub_pool--;
							break;
						}
					}
					if (!relinked) {
						tdm_ll_insert(_TD2P_NUM_EXT_PORTS,(cadence_start_idx+1));
						relinked=BOOL_TRUE; oversub_pool--;
						continue;
					}
				}
			}		
		}
	}

	ending_dist=tdm_ll_dist(list,cadence_start_idx);
	
	return tdm_abs(ending_dist-starting_dist);
	
}


/**
@name: fill_ovs
@param:

Fills and sorts oversub speed buckets. Supports up to 8 oversub speed groups.
 **/
int fill_ovs(short *z, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], unsigned char ovs_buf[64], int bucket1[_TD2P_OS_GROUP_LEN], unsigned char *z11, int bucket2[_TD2P_OS_GROUP_LEN], unsigned char *z22, int bucket3[_TD2P_OS_GROUP_LEN], unsigned char *z33, int bucket4[_TD2P_OS_GROUP_LEN], unsigned char *z44, int bucket5[_TD2P_OS_GROUP_LEN], unsigned char *z55, int bucket6[_TD2P_OS_GROUP_LEN], unsigned char *z66, int bucket7[_TD2P_OS_GROUP_LEN], unsigned char *z77, int bucket8[_TD2P_OS_GROUP_LEN], unsigned char *z88)
{
	/* int recount=0, n, l; */
	int i, j, k, tsc_id, tsc_fetch_cnt=0, slice_factor;
	unsigned char tsc_stack[4];
	
	TDM_SML_BAR
	TDM_VERBOSE2("Pointer location: %0d | Get port: %0d\n",(*z),ovs_buf[*z]);
	for (i=0; i<4; i++) {
		tsc_stack[i]=_TD2P_NUM_EXT_PORTS;
	}
	tsc_stack[0]=ovs_buf[*z]; ovs_buf[*z]=_TD2P_NUM_EXT_PORTS; tsc_id=which_tsc(tsc_stack[0],tsc);

	for (i=0; i<4; i++) {
		if (tsc[tsc_id][i]!=tsc_stack[0] && tsc[tsc_id][i]!=_TD2P_NUM_EXT_PORTS && speed[tsc[tsc_id][i]]==speed[tsc_stack[0]]) {
			for (j=1; j<=*z; j++) {
				if (ovs_buf[j]==tsc[tsc_id][i]) {
					tsc_stack[++tsc_fetch_cnt]=tsc[tsc_id][i];
					for (k=j; k<*z; k++) {
						ovs_buf[k]=ovs_buf[k+1];
					}
				}
			}
		}
	}
	/*
	while (tsc_fetch_cnt<4 && ++n<4) {
		for (l=1; l<=*z; l++) {
			if (((check_type_2(ovs_buf[l],tsc))+tsc_fetch_cnt)<4) {
				for (i=0; i<4; i++) {
					if (tsc[which_tsc(ovs_buf[l],tsc)][i]!=_TD2P_NUM_EXT_PORTS && speed[tsc[which_tsc(ovs_buf[l],tsc)][i]]==speed[ovs_buf[l]]) {
						for (j=1; j<=*z; j++) {
							if (ovs_buf[j]==tsc[which_tsc(ovs_buf[l],tsc)][i]) {
								if ((++tsc_fetch_cnt)<4) {
									tsc_stack[tsc_fetch_cnt]=tsc[which_tsc(ovs_buf[l],tsc)][i];
									for (k=j; k<*z; k++) {
										ovs_buf[k]=ovs_buf[k+1];
									}
								}
							}
						}
					}
				}
				break;
			}
		}
	}
	*/
	(*z)-=(1+tsc_fetch_cnt);	
	TDM_VERBOSE1("TDM: _____VERBOSE: TSC fetch {%0d}: ",(tsc_fetch_cnt+1));
	for (i=0; i<4; i++) {
		TDM_VERBOSE1("[%0d] ",tsc_stack[i]);
	}
	TDM_VERBOSE0("\n");
	
	switch (tsc_fetch_cnt) {
		case 1:
			slice_factor=(_TD2P_OS_GROUP_LEN/2);
			break;
		case 2:
		case 3:
			slice_factor=(_TD2P_OS_GROUP_LEN/4);
			break;
		default:
			slice_factor=_TD2P_OS_GROUP_LEN;
			break;
	}
	TDM_VERBOSE2("Slice size: %0d | Group speed: %0dG\n",slice_factor,(speed[tsc_stack[0]]/1000));
	while (bucket1[*z11]!=_TD2P_NUM_EXT_PORTS && (*z11)<(_TD2P_OS_GROUP_LEN-1)) (*z11)++;
	while (bucket2[*z22]!=_TD2P_NUM_EXT_PORTS && (*z22)<(_TD2P_OS_GROUP_LEN-1)) (*z22)++;
	while (bucket3[*z33]!=_TD2P_NUM_EXT_PORTS && (*z33)<(_TD2P_OS_GROUP_LEN-1)) (*z33)++;
	while (bucket4[*z44]!=_TD2P_NUM_EXT_PORTS && (*z44)<(_TD2P_OS_GROUP_LEN-1)) (*z44)++;
	while (bucket5[*z55]!=_TD2P_NUM_EXT_PORTS && (*z55)<(_TD2P_OS_GROUP_LEN-1)) (*z55)++;
	while (bucket6[*z66]!=_TD2P_NUM_EXT_PORTS && (*z66)<(_TD2P_OS_GROUP_LEN-1)) (*z66)++;
	while (bucket7[*z77]!=_TD2P_NUM_EXT_PORTS && (*z77)<(_TD2P_OS_GROUP_LEN-1)) (*z77)++;
	while (bucket8[*z88]!=_TD2P_NUM_EXT_PORTS && (*z88)<(_TD2P_OS_GROUP_LEN-1)) (*z88)++;
	
	if ( ((*z11)+(tsc_fetch_cnt*slice_factor)) < _TD2P_OS_GROUP_LEN ) {
		for (i=0; i<4; i++) {
			if (tsc_stack[i]!=_TD2P_NUM_EXT_PORTS) {
				bucket1[(*z11)+(i*slice_factor)] = tsc_stack[i];
				TDM_VERBOSE2("Group: 0 | Index: %0d | Port: %0d\n",((*z11)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(*z11)++;
	}
	else if ( ((*z22)+(tsc_fetch_cnt*slice_factor)) < _TD2P_OS_GROUP_LEN ) {
		for (i=0; i<4; i++) {
			if (tsc_stack[i]!=_TD2P_NUM_EXT_PORTS) {
				bucket2[(*z22)+(i*slice_factor)] = tsc_stack[i];
				TDM_VERBOSE2("Group: 1 | Index: %0d | Port: %0d\n",((*z22)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(*z22)++;
	}
	else if ( ((*z33)+(tsc_fetch_cnt*slice_factor)) < _TD2P_OS_GROUP_LEN ) {
		for (i=0; i<4; i++) {
			if (tsc_stack[i]!=_TD2P_NUM_EXT_PORTS) {
				bucket3[(*z33)+(i*slice_factor)] = tsc_stack[i];
				TDM_VERBOSE2("Group: 2 | Index: %0d | Port: %0d\n",((*z33)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(*z33)++;
	}
	else if ( ((*z44)+(tsc_fetch_cnt*slice_factor)) < _TD2P_OS_GROUP_LEN ) {
		for (i=0; i<4; i++) {
			if (tsc_stack[i]!=_TD2P_NUM_EXT_PORTS) {
				bucket4[(*z44)+(i*slice_factor)] = tsc_stack[i];
				TDM_VERBOSE2("Group: 3 | Index: %0d | Port: %0d\n",((*z44)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(*z44)++;
	}
	else if ( ((*z55)+(tsc_fetch_cnt*slice_factor)) < _TD2P_OS_GROUP_LEN ) {
		for (i=0; i<4; i++) {
			if (tsc_stack[i]!=_TD2P_NUM_EXT_PORTS) {
				bucket5[(*z55)+(i*slice_factor)] = tsc_stack[i];
				TDM_VERBOSE2("Group: 4 | Index: %0d | Port: %0d\n",((*z55)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(*z55)++;
	}
	else if ( ((*z66)+(tsc_fetch_cnt*slice_factor)) < _TD2P_OS_GROUP_LEN ) {
		for (i=0; i<4; i++) {
			if (tsc_stack[i]!=_TD2P_NUM_EXT_PORTS) {
				bucket6[(*z66)+(i*slice_factor)] = tsc_stack[i];
				TDM_VERBOSE2("Group: 5 | Index: %0d | Port: %0d\n",((*z66)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(*z66)++;
	}
	else if ( ((*z77)+(tsc_fetch_cnt*slice_factor)) < _TD2P_OS_GROUP_LEN ) {
		for (i=0; i<4; i++) {
			if (tsc_stack[i]!=_TD2P_NUM_EXT_PORTS) {
				bucket7[(*z77)+(i*slice_factor)] = tsc_stack[i];
				TDM_VERBOSE2("Group: 6 | Index: %0d | Port: %0d\n",((*z77)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(*z77)++;
	}
	else if ( ((*z88)+(tsc_fetch_cnt*slice_factor)) < _TD2P_OS_GROUP_LEN ) {
		for (i=0; i<4; i++) {
			if (tsc_stack[i]!=_TD2P_NUM_EXT_PORTS) {
				bucket8[(*z88)+(i*slice_factor)] = tsc_stack[i];
				TDM_VERBOSE2("Group: 7 | Index: %0d | Port: %0d\n",((*z88)+(i*slice_factor)),tsc_stack[i]);
			}
		}
		(*z88)++;
	}
	else {
		TDM_ERROR1("No speed group available, pointer at %0d\n",(*z));
		return FAIL;
	}
	
	TDM_VERBOSE1("TDM: _____VERBOSE: Pointer location: %0d\n",(*z));
	return PASS;
	
}


/**
@name: fill_ovs_simple
@param:

Fills oversub speed buckets without sort. Supports up to 8 oversub speed groups.
 **/
int fill_ovs_simple(short *z, unsigned char ovs_buf[64], int bucket1[_TD2P_OS_GROUP_LEN], unsigned char *z11, int bucket2[_TD2P_OS_GROUP_LEN], unsigned char *z22, int bucket3[_TD2P_OS_GROUP_LEN], unsigned char *z33, int bucket4[_TD2P_OS_GROUP_LEN], unsigned char *z44, int bucket5[_TD2P_OS_GROUP_LEN], unsigned char *z55, int bucket6[_TD2P_OS_GROUP_LEN], unsigned char *z66, int bucket7[_TD2P_OS_GROUP_LEN], unsigned char *z77, int bucket8[_TD2P_OS_GROUP_LEN], unsigned char *z88)
{
	if (*z11 < _TD2P_OS_GROUP_LEN) {
		bucket1[*z11] = ovs_buf[*z];
		(*z11)++;
		(*z)--;
	}
	else if (*z22 < _TD2P_OS_GROUP_LEN) {
		bucket2[*z22] = ovs_buf[*z];
		(*z22)++;
		(*z)--;
	}
	else if (*z33 < _TD2P_OS_GROUP_LEN) {
		bucket3[*z33] = ovs_buf[*z];
		(*z33)++;
		(*z)--;
	}
	else if (*z44 < _TD2P_OS_GROUP_LEN) {
		bucket4[*z44] = ovs_buf[*z];
		(*z44)++;
		(*z)--;
	}
	else if (*z55 < _TD2P_OS_GROUP_LEN) {
		bucket5[*z55] = ovs_buf[*z];
		(*z55)++;
		(*z)--;
	}
	else if (*z66 < _TD2P_OS_GROUP_LEN) {
		bucket6[*z66] = ovs_buf[*z];
		(*z66)++;
		(*z)--;
	}
	else if (*z77 < _TD2P_OS_GROUP_LEN) {
		bucket7[*z77] = ovs_buf[*z];
		(*z77)++;
		(*z)--;
	}
	else if (*z88 < _TD2P_OS_GROUP_LEN) {
		bucket8[*z88] = ovs_buf[*z];
		(*z88)++;
		(*z)--;
	}
	else {
		return FAIL;
	}
	
	return PASS;
	
}


/**
@name: block1
@param: int[256]

For debugging. Prints TDM table and blocks on keypress before resuming execution
**/
void block1(int tdm_tbl[256])
{
#ifdef _SET_TDM_DEV
	int i, j=1;
	
	for (i=255; i>0; i--) {
		if (tdm_tbl[i]!=_TD2P_NUM_EXT_PORTS) {
			j=(i+1);
			break;
		}
	}
	TDM_PRINT0("\n\t");
	TDM_PRINT1("[0] = %03d \n",tdm_tbl[0]);
	for (i=1; i<j; i++) {
		if (tdm_tbl[i]!=OVSB_TOKEN) {
			TDM_PRINT2("\t[%0d] = %03d \n",i,tdm_tbl[i]);
		}
		else {
			TDM_PRINT1("\t[%0d] = --- \n",i);
		}
	}
	while(getchar() != '\n');
	TDM_PRINT0("\n");
#endif
}


/**
@name: block2
@param:

For debugging. Prints TDM vector map and blocks on keypress before resuming execution
**/
void block2(FILE *file, unsigned char **vector_map, int lr_idx_limit)
{
#ifdef _SET_TDM_DEV
	tdm_vector_dump(file, vector_map, lr_idx_limit);
	while(getchar() != '\n');
	while(getchar() != '\n');
#endif
}


/**
@name: tdm_arr_exists
@param:

Returns boolean on whether an element exists in an array
**/
/* int tdm_arr_exists(int element, int len, int arr[len]) { */
/* C90 not supported */
int tdm_arr_exists(int element, int len, int arr[107]) {
	int i, result=FAIL;
	
	for (i=0; i<len; i++) {
		if (arr[i]==element) {
			result=PASS;
			break;
		}
	}
	
	return result;
}


/**
@name: tdm_arr_append
@param:

Appends to array without needing to track an index
Array must be initialized to _TD2P_NUM_EXT_PORTS
**/
/* int tdm_arr_append(int element, int len, int arr[len]) { */
/* C90 not supported */
int tdm_arr_append(int element, int len, int arr[107]) {
	int i, done=BOOL_FALSE, idx=0;
	
	for (i=0; i<len; i++) {
		if (arr[i]==_TD2P_NUM_EXT_PORTS) {
			idx=i;
			done=BOOL_TRUE;
			break;
		}
	}
	if (done) {
		arr[idx]=element;
	}
	
	return done;
}


/**
@name: tdm_PQ
@param: int

Customizable partial quotient ceiling function
**/
/* int tdm_PQ(float f) { return (int)((f < 0.0f) ? f - 0.5f : f + 0.5f); }*/
int tdm_PQ(int f) { 
	return ( (int)( ( f+5 )/10) );
}


/**
@name: tdm_slots
@param: int, int

Calculates number of slots required for a specific port
**/
int tdm_slots(int port_speed) {
	return ( (int)( (port_speed*10)/BW_QUANTA ) ); 
}


/**
@name: tdm_prealloc
@param:

Unless special exceptions are needed for certain port speeds, this is the standard procedure to presort ports by speed
**/
void tdm_prealloc(unsigned char stack[64], int buffer[64], short *x, char *bool, int j)
{
	int i;

	*bool = BOOL_TRUE;
	(*x)++;
	stack[*x] = buffer[j];
	for (i=0; i<(*x); i++) {
		if (stack[i] == buffer[j]) {
			(*x)--;
		}
	}
}


/**
@name: tdm_postalloc
@param:

Unless special exceptions are needed for certain port speeds, this is the standard procedure to load time vectors into TDM vector map
**/
int tdm_postalloc(unsigned char **vector_map, int freq, unsigned char spd, short *yy, short *y, int lr_idx_limit, unsigned char lr_stack[64], int token, const char* speed)
{
	int v, load_status;
	
	TDM_BIG_BAR
	TDM_VERBOSE1("TDM: _____VERBOSE: %sG scheduling pass\n",speed);
	TDM_SML_BAR
	TDM_VERBOSE0("TDM: _____VERBOSE: Stack contains: [ ");
	for (v=*y; v>0; v--) {
		if (lr_stack[v]!=_TD2P_NUM_EXT_PORTS) {
			TDM_VERBOSE1(" %0d ",lr_stack[v]);
		}
	}
	TDM_VERBOSE0(" ]\n");
	TDM_SML_BAR
	while (*y > 0) {
		load_status = tdm_vector_load(vector_map, freq, spd, *yy, lr_idx_limit);
		if (load_status==FAIL) {
			TDM_ERROR2("Failed to load %sG vector for port %0d\n",speed,lr_stack[*y]);
			return FAIL;
		}
		else {
			for (v=0; v<VEC_MAP_LEN; v++) {
				if (vector_map[*yy][v]==token) {
					vector_map[*yy][v]=lr_stack[*y];
				}
			}
			TDM_VERBOSE4("Loaded %sG port %d vector from stack pointer %d into map at index %0d\n",speed, lr_stack[*y], *y, *yy);
		}
		(*y)--; (*yy)++;
		if ((*y)>0) {
			TDM_SML_BAR
			TDM_VERBOSE2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", *yy, *y);
			TDM_SML_BAR
		}
		else {
			TDM_SML_BAR
			TDM_VERBOSE2("Done loading %sG vectors, map pointer floating at: %0d\n",speed, *yy);
			TDM_SML_BAR
		}
	}
	TDM_BIG_BAR
	
	return PASS;
	
}


/**
@name: TDM_scheduler_2d
@param:

TDM scheduler function
**/
int TDM_scheduler_2d(FILE *file, int freq, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int port_state_map[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_buffer[64], int os_buffer[64], int lr_idx_limit, int tdm_tbl[256], int bucket1[_TD2P_OS_GROUP_LEN], int bucket2[_TD2P_OS_GROUP_LEN], int bucket3[_TD2P_OS_GROUP_LEN], int bucket4[_TD2P_OS_GROUP_LEN], int bucket5[_TD2P_OS_GROUP_LEN], int bucket6[_TD2P_OS_GROUP_LEN], int bucket7[_TD2P_OS_GROUP_LEN], int bucket8[_TD2P_OS_GROUP_LEN], int pipe, unsigned char accessories, unsigned char higig_mgmt)
{
	/* Values */
	int num_40g, num_100g, timeout=TIMEOUT;
	/* Dynamic data struc */
	struct node *linked_list;
	/* Single use variables or loop iterators */
	int g, i=0, j, k, l, v, w;
	/* Vector TDM variables */
	unsigned short map_width, map_length;
	unsigned char **vector_map;
	unsigned char dim_reduce, refactor_done=BOOL_FALSE;
	char HG4X106G_3X40G=BOOL_FALSE, HG1X106G_xX40G_OVERSUB=BOOL_FALSE;
	int load_status=FAIL, vec_slot_cnt=0, lr_vec_cnt=0;
	/* int num_lr=0, num_os=0; This value is set but not used [-Werror=unused-but-set-variable] */
	int lr_slot_cnt=0,
		tsc_pipeline[107], xx=0, x0=0, triport_op=BOOL_FALSE, triport_priority=BOOL_TRUE, vec_id, ns_min_cnt, rotation_step=DEFAULT_STEP_SIZE, main_pass_done, main_pass_timeout,
		cadence_cnt=0, cadence_step_size, cadence_ref_idx, redis_100g_cnt, redis_100g_port;
	/* Linked list TDM variables */
	int cadence_step, cadence_step_limit, cadence_blocks, retrace_num=0, retrace_result;
	/* TDM filter variables */
	int vec_trans_port_list[107], last_smooth;
	/* int vec_filter_result; This value is set but not used [-Werror=unused-but-set-variable] */
	int vec_filter_abort, vec_filter_port, vec_filter_wc,
		filter_slice_size=0, filter_slice_start, filter_slice_end=0, filter_slice_wc_0, filter_slice_wc_1, filter_slice_found, filter_slice_dist,
		local_clump_wc;
	/* Table post processing variables */
	int cap=0, cap_tsc=_TD2P_NUM_EXT_PORTS, cap_lpbk=0, cap_lkbk=0;
	/* Flags to denote which linerate speeds are active */
	char lr_1=BOOL_FALSE, lr_100=BOOL_FALSE, lr_10=BOOL_FALSE, lr_20=BOOL_FALSE, lr_25=BOOL_FALSE, lr_40=BOOL_FALSE, lr_50=BOOL_FALSE, lr_120=BOOL_FALSE, lr_enable=BOOL_FALSE;
	/* Line rate port lists */
	unsigned char lr1[64], lr10[64], lr20[64], lr25[64], lr40[64], lr50[64], lr100[64], lr120[64];
	/* Flags to denote which oversub speeds are active */
	char os_1=BOOL_FALSE, os_10=BOOL_FALSE, os_20=BOOL_FALSE, os_25=BOOL_FALSE, os_40=BOOL_FALSE, os_50=BOOL_FALSE, os_100=BOOL_FALSE, os_120=BOOL_FALSE, os_enable=BOOL_FALSE;
	/* Oversub port lists */
	unsigned char os1[64], os10[64], os20[64], os25[64], os40[64], os50[64], os100[64], os120[64];
	/* Create local stack pointers for lr - 7 local pointers, 1 schedule pointer */
	short y8=0, y7=0, y1=0, y2=0, y3=0, y4=0, y5=0, y6=0, yy=0;
	/* Create local stack pointers for os - 5 local pointers, 4 bucket pointers */
	short z8=0, z7=0, z6=0, z1=0, z2=0, z3=0, z4=0, z5=0;
	unsigned char z11=0, z22=0, z33=0, z44=0, z55=0, z66=0, z77=0, z88=0;
	/* Sister port proxcheck variables */
	unsigned char prox_swap=_TD2P_NUM_EXT_PORTS;
	/* Triport */
	unsigned char tri_chk, tri_en_50=BOOL_FALSE, tri_en_40=BOOL_FALSE, tri_en_20=BOOL_FALSE;

	TDM_VERBOSE1("TDM: _____VERBOSE: index limit set to %d\n",lr_idx_limit);
	for (j=0; j<64; j++) {
		lr100[j] = _TD2P_NUM_EXT_PORTS; lr50[j] = _TD2P_NUM_EXT_PORTS; lr40[j] = _TD2P_NUM_EXT_PORTS;
		lr25[j] = _TD2P_NUM_EXT_PORTS; lr20[j] = _TD2P_NUM_EXT_PORTS; lr10[j] = _TD2P_NUM_EXT_PORTS;
		os10[j] = _TD2P_NUM_EXT_PORTS; os20[j] = _TD2P_NUM_EXT_PORTS; os25[j] = _TD2P_NUM_EXT_PORTS;
		os40[j] = _TD2P_NUM_EXT_PORTS; os50[j] = _TD2P_NUM_EXT_PORTS; os100[j] = _TD2P_NUM_EXT_PORTS;
		lr1[j]=_TD2P_NUM_EXT_PORTS; lr10[j]=_TD2P_NUM_EXT_PORTS; lr20[j]=_TD2P_NUM_EXT_PORTS; lr25[j]=_TD2P_NUM_EXT_PORTS; lr40[j]=_TD2P_NUM_EXT_PORTS; lr50[j]=_TD2P_NUM_EXT_PORTS; lr100[j]=_TD2P_NUM_EXT_PORTS; lr120[j]=_TD2P_NUM_EXT_PORTS;
		os1[j]=_TD2P_NUM_EXT_PORTS; os10[j]=_TD2P_NUM_EXT_PORTS; os20[j]=_TD2P_NUM_EXT_PORTS; os25[j]=_TD2P_NUM_EXT_PORTS; os40[j]=_TD2P_NUM_EXT_PORTS; os50[j]=_TD2P_NUM_EXT_PORTS; os100[j]=_TD2P_NUM_EXT_PORTS; os120[j]=_TD2P_NUM_EXT_PORTS;
	}
	for (j=0; j<107; j++) {
		tsc_pipeline[j]=_TD2P_NUM_EXT_PORTS;
		vec_trans_port_list[j]=_TD2P_NUM_EXT_PORTS;
	}

	linked_list = NULL;
	
	/* Generate the TDM vector map */
	map_length=VEC_MAP_LEN;
	map_width=VEC_MAP_WID;
	vector_map=(unsigned char **) TDM_ALLOC(map_width*sizeof(unsigned char *), "vector_map_l1");
	for (j=0; j<map_width; j++) {
		vector_map[j]=(unsigned char *) TDM_ALLOC(map_length*sizeof(unsigned char), "vector_map_l2");
	}
	for (v=0; v<map_length; v++) {
		for (j=0; j<map_width; j++) {
			vector_map[j][v]=_TD2P_NUM_EXT_PORTS;
		}
	}
	
	/* Preallocation */
	for (j=0; j<64; j++) {
		if ((lr_buffer[j] != _TD2P_NUM_EXT_PORTS) && (port_state_map[lr_buffer[j]-1] == 1)) {
			switch (speed[lr_buffer[j]]) {
				case SPEED_1G:
				case SPEED_2p5G:
					tdm_prealloc(lr1, lr_buffer, &y8, &lr_1, j);
					break;
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
					tdm_prealloc(lr10, lr_buffer, &y1, &lr_10, j);
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					tdm_prealloc(lr20, lr_buffer, &y2, &lr_20, j);
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					tdm_prealloc(lr40, lr_buffer, &y3, &lr_40, j);
					break;
				case SPEED_100G:
					tdm_prealloc(lr100, lr_buffer, &y4, &lr_100, j);
					break;
				case SPEED_50G:
					tdm_prealloc(lr50, lr_buffer, &y5, &lr_50, j);
					break;
				case SPEED_25G:
					tdm_prealloc(lr25, lr_buffer, &y6, &lr_25, j);
					break;
				case SPEED_120G:
					tdm_prealloc(lr120, lr_buffer, &y7, &lr_120, j);
					break;					
				default:
					TDM_ERROR2("Speed %0d port %0d was skipped in linerate preallocation\n",speed[lr_buffer[j]],lr_buffer[j]);
			}
		}
		else if (lr_buffer[j] != _TD2P_NUM_EXT_PORTS && lr_buffer[j] != OVSB_TOKEN) {
			TDM_ERROR2("Failed to presort linerate port %0d with state mapping %0d\n", lr_buffer[j], port_state_map[lr_buffer[j]-1]);
		}
		if ((os_buffer[j] != _TD2P_NUM_EXT_PORTS) && (port_state_map[os_buffer[j]-1] == 2)) {
			switch (speed[os_buffer[j]]) {
				case SPEED_1G:
				case SPEED_2p5G:
					tdm_prealloc(os1, os_buffer, &z8, &os_1, j);
					break;
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
					tdm_prealloc(os10, os_buffer, &z1, &os_10, j);
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					tdm_prealloc(os20, os_buffer, &z2, &os_20, j);
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					tdm_prealloc(os40, os_buffer, &z3, &os_40, j);
					break;
				case SPEED_100G:
					tdm_prealloc(os100, os_buffer, &z4, &os_100, j);
					break;
				case SPEED_50G:
					tdm_prealloc(os50, os_buffer, &z5, &os_50, j);
					break;
				case SPEED_25G:
					tdm_prealloc(os25, os_buffer, &z6, &os_25, j);
					break;
				case SPEED_120G:
					tdm_prealloc(os120, os_buffer, &z7, &os_120, j);
					break;					
				default:
					TDM_ERROR2("Speed %0d port %0d was skipped in oversub preallocation\n",speed[os_buffer[j]],os_buffer[j]);
			}
		}
		else if (os_buffer[j] != _TD2P_NUM_EXT_PORTS) {
			TDM_ERROR2("Failed to presort oversub port %0d with state mapping %0d\n", os_buffer[j], port_state_map[os_buffer[j]-1]);
		}
	}
	os_enable = (os_1||os_10||os_20||os_25||os_40||os_50||os_100||os_120);
	lr_enable = (lr_1||lr_10||lr_20||lr_25||lr_40||lr_50||lr_100||lr_120);
	/* This value is set but not used [-Werror=unused-but-set-variable]
	num_lr=(y8+y1+y2+y3+y4+y5+y6+y7);
	num_os=(z8+z1+z2+z3+z4+z5+z6+z7);
	*/
	num_100g=y4;
	num_40g=y3;
	/* 4x106HG+3x40G is a specific exception to all other scheduling rules */
	if (num_40g==3 && num_100g==4 && freq==850) {
		accessories=9;
		lr_idx_limit=201;
		HG4X106G_3X40G=BOOL_TRUE;
	}
	/* To guarantee oversub smoothness in nonscaling 100G+40G case, algorithm must sort with a 120G port vector (bw will be reclaimed) */
	if (num_100g==1 && lr_40 && os_enable) {
		HG1X106G_xX40G_OVERSUB=BOOL_TRUE;
	}
	
	TDM_VERBOSE9("(1G - %0d) (10G - %0d) (20G - %0d) (25G - %0d) (40G - %0d) (50G - %0d) (100G - %0d) (120G - %0d) (Number of Line Rate Types - %0d)\n", y8, y1, y2, y6, y3, y5, y4, y7, (lr_1+lr_10+lr_20+lr_25+lr_40+lr_50+lr_100+lr_120));
	TDM_BIG_BAR

	/* Postallocation */
	/*
		120G
	*/
	if (y7 > 0) {
		if (tdm_postalloc(vector_map, freq, 120, &yy, &y7, lr_idx_limit, lr120, TOKEN_120G, "120")!=PASS) {
			TDM_ERROR0("Critical error in 120G time vector preallocation\n");
			return 0;
		}
	}
	/*
		100G
	*/
	if (y4 > 0) {
		TDM_BIG_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: 100G scheduling pass\n");
		TDM_SML_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: Stack contains: [ ");
		for (v=y4; v>0; v--) {
			if (lr100[v]!=_TD2P_NUM_EXT_PORTS) {
				TDM_VERBOSE1(" %0d ",lr100[v]);
			}
		}
		TDM_VERBOSE0(" ]\n");
		TDM_SML_BAR
		while (y4 > 0) {
			/* Must account for 4x106HG+3x40G special case */
			if (HG4X106G_3X40G==BOOL_TRUE) {
				load_status = tdm_vector_load(vector_map, freq, 107, yy, lr_idx_limit);
			}
			else if (HG1X106G_xX40G_OVERSUB==BOOL_TRUE) {
				load_status = tdm_vector_load(vector_map, freq, 108, yy, lr_idx_limit);
			}
			else {
				if (check_ethernet(lr100[y4],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
					load_status = tdm_vector_load(vector_map, freq, 100, yy, lr_idx_limit);
				}
				else {
					load_status = tdm_vector_load(vector_map, freq, 106, yy, lr_idx_limit);
				}
			}
			if (load_status==FAIL) {
				TDM_ERROR1("Failed to load 100G/106G vector for port %0d\n",lr100[y4]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_100G || vector_map[yy][v]==TOKEN_106G) {
						vector_map[yy][v]=lr100[y4];
					}
				}
				TDM_VERBOSE3("Loaded 100G port %d vector from stack pointer %d into map at index %0d\n", lr100[y4], y4, yy);
			}
			y4--; yy++;
			if (y4>0) {
				TDM_SML_BAR
				TDM_VERBOSE2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", yy, y4);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_VERBOSE1("TDM: _____VERBOSE: Done loading 100G vectors, map pointer floating at: %0d\n", yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	/*
		50G
	*/
	if (y5 > 0) {
		TDM_BIG_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: 50G scheduling pass\n");
		/* Scatter sister ports before postallocation to reduce entropy of proxcheck later on */
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
		TDM_VERBOSE0("TDM: _____VERBOSE: Stack contains: [ ");
		for (v=y5; v>0; v--) {
			if (lr50[v]!=_TD2P_NUM_EXT_PORTS) {
				TDM_VERBOSE1(" %0d ",lr50[v]);
			}
		}
		TDM_VERBOSE0(" ]\n");
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
				TDM_VERBOSE0("TDM: _____VERBOSE: 50G triport detected\n");
				break;
			}
		}
		while (y5 > 0) {
			if (check_ethernet(lr50[y5],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
				load_status = tdm_vector_load(vector_map, freq, 50, yy, lr_idx_limit);
			}
			else {
				load_status = tdm_vector_load(vector_map, freq, 53, yy, lr_idx_limit);
			}
			if (load_status==FAIL)
			{
				TDM_ERROR1("Failed to load 50G vector for port %0d\n",lr50[y5]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_50G) {
						vector_map[yy][v]=lr50[y5];
					}
				}			
				TDM_VERBOSE3("Loaded 50G port %d vector from stack pointer %d into map at index %0d\n", lr50[y5], y5, yy);
			}			
			y5--; yy++;
			if (y5>0) {
				TDM_SML_BAR
				TDM_VERBOSE2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", yy, y5);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_VERBOSE1("TDM: _____VERBOSE: Done scheduling 50G, map pointer floating at: %0d\n", yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	/*
		40G
	*/
	if (y3 > 0) {
		TDM_BIG_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: 40G scheduling pass\n");
		/* Scatter sister ports before postallocation to reduce entropy of proxcheck later on */
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
		TDM_VERBOSE0("TDM: _____VERBOSE: Stack contains: [ ");
		for (v=y3; v>0; v--) {
			if (lr40[v]!=_TD2P_NUM_EXT_PORTS) {
				TDM_VERBOSE1(" %0d ",lr40[v]);
			}
		}
		TDM_VERBOSE0(" ]\n");
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
				TDM_VERBOSE0("TDM: _____VERBOSE: 40G triport detected\n");
				break;
			}
		}
		while (y3 > 0) {
			/* Must account for 4x106HG+3x40G special case */
			if (HG4X106G_3X40G==BOOL_TRUE) {
				if (y3==3) {
					load_status = tdm_vector_load(vector_map, freq, 43, yy, lr_idx_limit);
				}
				else if (y3==2) {
					load_status = tdm_vector_load(vector_map, freq, 44, yy, lr_idx_limit);
				}
				else if (y3==1) {
					load_status = tdm_vector_load(vector_map, freq, 45, yy, lr_idx_limit);
				}				
			}
			else {
				if (check_ethernet(lr40[y3],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
					load_status = tdm_vector_load(vector_map, freq, 40, yy, lr_idx_limit);
				}
				else {
					load_status = tdm_vector_load(vector_map, freq, 42, yy, lr_idx_limit);
				}
			}
			if (load_status==FAIL) {
				TDM_ERROR1("Failed to load 40G vector for port %0d\n",lr40[y3]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_40G) {
						vector_map[yy][v]=lr40[y3];
					}
				}			
				TDM_VERBOSE3("Loaded 40G port %d vector from stack pointer %d into map at index %0d\n", lr40[y3], y3, yy);
			}			
			y3--; yy++;
			if (y3>0) {
				TDM_SML_BAR
				TDM_VERBOSE2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", yy, y3);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_VERBOSE1("TDM: _____VERBOSE: Done scheduling 40G, map pointer floating at: %0d\n", yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}	
	/*
		25G
	*/
	if (y6 > 0) {
		TDM_BIG_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: 25G scheduling pass\n");
		TDM_SML_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: Stack contains: [ ");
		for (v=y6; v>0; v--) {
			if (lr25[v]!=_TD2P_NUM_EXT_PORTS) {
				TDM_VERBOSE1(" %0d ",lr25[v]);
			}
		}
		TDM_VERBOSE0(" ]\n");
		TDM_SML_BAR
		while (y6 > 0)
		{
			if (check_ethernet(lr25[y6],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
				load_status = tdm_vector_load(vector_map, freq, 25, yy, lr_idx_limit);
			}
			else {
				load_status = tdm_vector_load(vector_map, freq, 27, yy, lr_idx_limit);
			}
			if (load_status==FAIL)
			{
				TDM_ERROR1("Failed to load 25G vector for port %0d\n",lr25[y6]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_25G) {
						vector_map[yy][v]=lr25[y6];
					}
				}			
				TDM_VERBOSE3("Loaded 25G port %d vector from stack pointer %d into map at index %0d\n", lr25[y6], y6, yy);
			}			
			y6--; yy++;
			if (y6>0) {
				TDM_SML_BAR
				TDM_VERBOSE2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", yy, y6);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_VERBOSE1("TDM: _____VERBOSE: Done scheduling 25G, map pointer floating at: %0d\n", yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	/*
		20G
	*/
	if (y2 > 0) {
		TDM_BIG_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: 20G scheduling pass\n");
		TDM_SML_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: Stack contains: [ ");
		for (v=y2; v>0; v--) {
			if (lr20[v]!=_TD2P_NUM_EXT_PORTS) {
				TDM_VERBOSE1(" %0d ",lr20[v]);
			}
		}
		TDM_VERBOSE0(" ]\n");
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
				TDM_VERBOSE0("TDM: _____VERBOSE: 20G triport detected\n");
				break;
			}
		}
		while (y2 > 0) {
			if (check_ethernet(lr20[y2],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
				load_status = tdm_vector_load(vector_map, freq, 20, yy, lr_idx_limit);
			}
			else {
				load_status = tdm_vector_load(vector_map, freq, 21, yy, lr_idx_limit);
			}
			if (load_status==FAIL)
			{
				TDM_ERROR1("Failed to load 20G vector for port %0d\n",lr20[y2]);
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_20G) {
						vector_map[yy][v]=lr20[y2];
					}
				}
				TDM_VERBOSE3("Loaded 20G port %d vector from stack pointer %d into map at index %0d\n", lr20[y2], y2, yy);
			}			
			y2--; yy++;
			if (y2>0) {
				TDM_SML_BAR
				TDM_VERBOSE2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", yy, y2);
				TDM_SML_BAR
			}
			else {
				TDM_SML_BAR
				TDM_VERBOSE1("TDM: _____VERBOSE: Done scheduling 20G, map pointer floating at: %0d\n", yy);
				TDM_SML_BAR
			}
		}
		TDM_BIG_BAR
	}
	/*
		10G
	*/
	if (y1 > 0) {
		TDM_BIG_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: 10G scheduling pass\n");
		TDM_SML_BAR
		TDM_VERBOSE0("TDM: _____VERBOSE: Stack contains: [ ");
		for (v=y1; v>0; v--) {
			if (lr10[v]!=_TD2P_NUM_EXT_PORTS) {
				TDM_VERBOSE1(" %0d ",lr10[v]);
			}
		}
		TDM_VERBOSE0(" ]\n");
		TDM_SML_BAR
		while (y1 > 0)
		{
			if (check_ethernet(lr10[y1],speed,tsc,traffic) && higig_mgmt==BOOL_FALSE) {
				load_status = tdm_vector_load(vector_map, freq, 10, yy, lr_idx_limit);
			}
			else {
				load_status = tdm_vector_load(vector_map, freq, 11, yy, lr_idx_limit);
			}
			if (load_status==FAIL)
			{
				TDM_ERROR1("Failed to load 10G vector for port %0d\n",lr10[y1]);
				TDM_ERROR0("Critical error in 10G time vector preallocation\n");
				return 0;
			}
			else {
				for (v=0; v<VEC_MAP_LEN; v++) {
					if (vector_map[yy][v]==TOKEN_10G) {
						vector_map[yy][v]=lr10[y1];
					}
				}
				TDM_VERBOSE3("Loaded 10G port %d vector from stack pointer %d into map at index %0d\n", lr10[y1], y1, yy);
			}			
			y1--; yy++;
			if (y1>0) {
				TDM_SML_BAR
				TDM_VERBOSE2("Map pointer rolled to: %0d, stack pointer is at: %0d\n", yy, y1);
				TDM_SML_BAR			
			}
			else {
				TDM_SML_BAR
				TDM_VERBOSE1("TDM: _____VERBOSE: Done scheduling 10G, map pointer floating at: %0d\n", yy);
				TDM_SML_BAR			
			}
		}
		TDM_BIG_BAR
	}
	/*
		1G
	*/
	if (y8 > 0) {
		if (tdm_postalloc(vector_map, freq, 1, &yy, &y8, lr_idx_limit, lr1, TOKEN_1G, "1")!=PASS) {
			TDM_ERROR0("Critical error in 1G time vector preallocation\n");
			return 0;
		}
	}
	
	/* Store some table vector parameters */
	
	if (os_enable && lr_enable) {
		for (i=0; i<VEC_MAP_WID; i++) {
			if (vector_map[i][0]!=_TD2P_NUM_EXT_PORTS) {
				lr_vec_cnt++;
			}
		}
		for (j=0; j<lr_idx_limit; j++) {
			if (vector_map[0][j]==_TD2P_NUM_EXT_PORTS) {
				vec_slot_cnt++;
			}
			if (j!=0 && vector_map[0][j]!=_TD2P_NUM_EXT_PORTS && vector_map[0][(j-vec_slot_cnt-1)]!=_TD2P_NUM_EXT_PORTS) {
				break;
			}
		}
	}
	else {
		lr_vec_cnt=0;
		for (i=0; i<VEC_MAP_WID; i++) {
			if (vector_map[i][0]!=_TD2P_NUM_EXT_PORTS) {
				lr_vec_cnt++;
			}
		}
	}
	for (i=0; i<lr_vec_cnt; i++) {
		if (check_type_2(vector_map[i][0],tsc)==3) {
			triport_op=BOOL_TRUE;
			break;
		}
	}
	for (i=0; i<lr_vec_cnt; i++) {
		lr_slot_cnt+=tdm_slots(speed[vector_map[i][0]]/1000);
	}
	/* Construct TSC priority queue */
	for (i=0; i<lr_vec_cnt; i++) {
		if ( !(tdm_arr_exists(which_tsc(vector_map[i][0],tsc),107,tsc_pipeline)) ) {
			tsc_pipeline[xx++]=which_tsc(vector_map[i][0],tsc);
		}
	}

	if (lr_enable) {
		if (!triport_op) {
			tdm_vector_dump(file, vector_map, lr_idx_limit);
			for (i=0; i<lr_vec_cnt; i++) {
				if ( (check_type_2(vector_map[i][0],tsc)>1) && (check_spd(vector_map,lr_idx_limit,i,speed,tsc,traffic,BOOL_FALSE)>=10) ) {
					/* Implement round robin with TSC priority queue */
					do {
						vec_id=tdm_pick_vec(vector_map,tsc,speed,lr_vec_cnt,tsc_pipeline[x0],&triport_priority, i);
						if ( (check_fastest_lane(vector_map[vec_id][0],tsc,speed)<=(speed[vector_map[i][0]]-SPEED_DELIMITER)) ||
							 (check_fastest_lane(vector_map[vec_id][0],tsc,speed)>=(speed[vector_map[i][0]]+SPEED_DELIMITER)) ) {
							if ((x0++)>=xx) {x0=0;}
						}
					} while ( /*(vec_id<=i) && */
							  ( (check_fastest_lane(vector_map[vec_id][0],tsc,speed)<=(speed[vector_map[i][0]]-SPEED_DELIMITER)) ||
								(check_fastest_lane(vector_map[vec_id][0],tsc,speed)>=(speed[vector_map[i][0]]+SPEED_DELIMITER)) ) );
					for (j=i; j<lr_vec_cnt; j++) {
						if (vector_map[j][0]==find_fastest_lane(vector_map[vec_id][0],tsc,speed)) {
							vec_id=j;
							break;
						}
					}
					/* Sort vectors according to pipeline ordering */
					if (vec_id!=i) {
						tdm_mem_transpose(vector_map,i,(map_width-1));
						tdm_mem_transpose(vector_map,vec_id,i);
						tdm_mem_transpose(vector_map,(map_width-1),vec_id);
					}
				}
				if ((x0++)>=xx) {x0=0;}
			}
		}
		tdm_vector_dump(file, vector_map, lr_idx_limit);
		if (os_enable) {
			/* Divide groups of vectors between cadences for oversub smoothing */			
			for (i=1; i<lr_vec_cnt; i++) {
				if ( (check_spd(vector_map,lr_idx_limit,i,speed,tsc,traffic,BOOL_FALSE)!=check_spd(vector_map,lr_idx_limit,(i-1),speed,tsc,traffic,BOOL_FALSE)) && (check_spd(vector_map,lr_idx_limit,(i-1),speed,tsc,traffic,BOOL_TRUE)%check_spd(vector_map,lr_idx_limit,i,speed,tsc,traffic,BOOL_TRUE)==0) ) {
					cadence_blocks=1;
					cadence_ref_idx=i-1;
					while(cadence_ref_idx>0 && (check_spd(vector_map,lr_idx_limit,cadence_ref_idx,speed,tsc,traffic,BOOL_FALSE)==check_spd(vector_map,lr_idx_limit,(cadence_ref_idx-1),speed,tsc,traffic,BOOL_FALSE)) ) {
						cadence_ref_idx--;
					}
					for (j=0; j<lr_idx_limit; j++) {
						if (vector_map[cadence_ref_idx][j]!=_TD2P_NUM_EXT_PORTS && vector_map[i][j]==_TD2P_NUM_EXT_PORTS) {
							cadence_blocks++;
						}
						if (j>0 && vector_map[cadence_ref_idx][j]!=_TD2P_NUM_EXT_PORTS && vector_map[i][j]!=_TD2P_NUM_EXT_PORTS) {
							break;
						}
					}
					for (j=i; j<lr_vec_cnt; j++) {
						if ( check_spd(vector_map,lr_idx_limit,j,speed,tsc,traffic,BOOL_FALSE)==check_spd(vector_map,lr_idx_limit,i,speed,tsc,traffic,BOOL_FALSE) ) {
							cadence_cnt++;
						}
					}
					cadence_step_size=(cadence_cnt/cadence_blocks);
					for (j=(i+cadence_step_size); j<(i+cadence_cnt); j+=cadence_step_size) {
						for (k=0; k<cadence_step_size; k++) {
							cadence_step=0; cadence_step_limit=(lr_idx_limit/4);
							do {
								tdm_vector_rotate(vector_map[j+k],lr_idx_limit,1);
								if (vector_map[cadence_ref_idx][tdm_map_find_y(vector_map,lr_idx_limit,(j+k),0)]!=_TD2P_NUM_EXT_PORTS && vector_map[j+k][tdm_map_find_y(vector_map,lr_idx_limit,(j+k),0)]!=_TD2P_NUM_EXT_PORTS) {
									cadence_step++;
								}
								if ( cadence_step==((j-i)/cadence_step_size) ) {
									break;
								}
							} while ( (--cadence_step_limit)>0 );
						}
					}
					tdm_vector_dump(file, vector_map, lr_idx_limit);
				}
			}
		}		
		/* Main Pass */
		/* Rotate each vector to singular (or most singular possible) position */
		/* VBS min spacing is enforced at this stage */
		/* LLS min spacing should already be guaranteed by the loaded vector */
		main_pass_timeout=0;
		do {
			for (i=1; i<lr_vec_cnt; i++) {
				dim_reduce=FAIL;
				for (k=0; k<lr_idx_limit; k++) {
					tdm_vector_rotate(vector_map[i], lr_idx_limit, rotation_step);
					tdm_vector_dump(file, vector_map, lr_idx_limit);
					if ( (tdm_fit_singular_col(vector_map,i,lr_idx_limit)==PASS) && 
						 (tdm_fit_prox(vector_map,lr_idx_limit,i,i,tsc)==PASS) ) {
						dim_reduce=PASS;
						break;
					}
				}
				if (!dim_reduce) {
					ns_min_cnt=tdm_count_nonsingular(vector_map,i,lr_idx_limit);
					for (k=0; k<lr_idx_limit; k++) {
						tdm_vector_rotate(vector_map[i], lr_idx_limit, rotation_step);
						if ( (tdm_count_nonsingular(vector_map,i,lr_idx_limit)<ns_min_cnt) &&
							 (tdm_fit_prox(vector_map,lr_idx_limit,i,i,tsc)==PASS) ) {
							ns_min_cnt=tdm_count_nonsingular(vector_map,i,lr_idx_limit);
						}
					}
					while ( ns_min_cnt!=tdm_count_nonsingular(vector_map,i,lr_idx_limit) ) {
						tdm_vector_rotate(vector_map[i], lr_idx_limit, rotation_step);
						tdm_vector_dump(file, vector_map, lr_idx_limit);
					}
				}
			}
			for (i=1; i<lr_vec_cnt; i++) {
				if (tdm_fit_prox(vector_map,lr_idx_limit,lr_vec_cnt,i,tsc)!=PASS) {
					for (j=0; j<lr_vec_cnt; j++) {
						if (check_spd(vector_map,lr_idx_limit,j,speed,tsc,traffic,BOOL_FALSE)==check_spd(vector_map,lr_idx_limit,i,speed,tsc,traffic,BOOL_FALSE)) {
							tdm_sticky_transpose(vector_map,lr_idx_limit,i,j);
							if ( (tdm_fit_prox(vector_map,lr_idx_limit,lr_vec_cnt,i,tsc)==PASS) &&
								 (tdm_fit_prox(vector_map,lr_idx_limit,lr_vec_cnt,j,tsc)==PASS) ) {
								tdm_vector_dump(file, vector_map, lr_idx_limit);
								break;
							}
							else {
								tdm_sticky_transpose(vector_map,lr_idx_limit,i,j);
							}
						}
					}
				}
			}
			main_pass_done=BOOL_TRUE;
			if (!lr_1) {
				for (i=0; i<lr_vec_cnt; i++) {
					if (tdm_check_nonsingular(vector_map,i)==FAIL) {
						main_pass_done=BOOL_FALSE;
					}
				}
			}
		} while (!main_pass_done && (++main_pass_timeout<MAIN_SCHEDULER_PASSES));
	}
	if (HG1X106G_xX40G_OVERSUB==BOOL_TRUE) {
		redis_100g_cnt=0;
		redis_100g_port=vector_map[0][0];
		tdm_vector_clear(vector_map,0,VEC_MAP_LEN);
		for (i=0; i<lr_idx_limit; i+=24) {
			vector_map[0][i] = redis_100g_port;
			vector_map[0][i+4] = redis_100g_port;
			vector_map[0][i+8] = redis_100g_port;
			vector_map[0][i+12] = redis_100g_port;
			vector_map[0][i+18] = redis_100g_port;
		}
		for (j=0; j<lr_idx_limit; j++) {
			if (!tdm_check_nonsingular(vector_map,j)) {
				tdm_vector_clear(vector_map,0,VEC_MAP_LEN);
				for (i=0; ( (i<(lr_idx_limit-4)) && ((++redis_100g_cnt)<=40) ); i+=5) {
					if (tdm_check_blank(vector_map,i)) {
						vector_map[0][i]=redis_100g_port;
					}
					else if ( (tdm_check_blank(vector_map,(i-1))) && (tdm_slice_size_2d(vector_map,(i-1),lr_idx_limit)<=tdm_slice_size_2d(vector_map,(i+1),lr_idx_limit)) ) {
						vector_map[0][i-1]=redis_100g_port;
					}
					else if (tdm_check_blank(vector_map,(i+1))) {
						vector_map[0][i+1]=redis_100g_port;
					}
				}
				break;
			}
		}
	}

	TDM_PRINT0("\n\n");
	TDM_SML_BAR
	TDM_PRINT1("\nSolution vmap pipe %0d:\n\n           ",((vector_map[0][0])/64));
	for (v=0; v<VEC_MAP_WID; v++) {
		TDM_PRINT1("%d",v);
		if ((v+1)<VEC_MAP_WID) {
			if ((v+1)/10==0) {
				TDM_PRINT0("___");
			}
			else {
				TDM_PRINT0("__");
			}
		}
	}
	TDM_PRINT0("\n");
	for (v=0; v<lr_idx_limit; v++) {
		TDM_PRINT1("\n %d___\t", v);
		for (j=0; j<VEC_MAP_WID; j++) {
			if (vector_map[j][v]!=_TD2P_NUM_EXT_PORTS) {
				TDM_PRINT1(" %03d", vector_map[j][v]);
			}
			else {
				TDM_PRINT0(" ---");
			}
		}
	}
	TDM_PRINT0("\n\n\n");
	TDM_SML_BAR
	
	TDM_PRINT0("\n\tConstructing TDM linked list...\n");
	TDM_PRINT5("\t\t| Span=%0d | Length=%0d | Pivot=%0d | Slice Size=%0d | Retrace=%0d |\n",lr_vec_cnt,lr_idx_limit,vector_map[0][0],tdm_map_cadence_count(vector_map[0],0),tdm_map_retrace_count(vector_map,0,0));
	/* Traverse vector map to construct linked list */
	tdm_ll=NULL;
	for (i=0; i<lr_idx_limit; i++) {
		for (j=0; j<map_width; j++) {
			if (tdm_fit_singular_cnt(vector_map,i)>0) {
				TOKEN_CHECK(vector_map[j][i]) {
					tdm_ll_append(vector_map[j][i]);
					if (vector_map[j][i]==vector_map[0][0]) {
						retrace_num++;
					}
				}
			}
			else {
				tdm_ll_append(_TD2P_NUM_EXT_PORTS);
				break;
			}
		}
	}
	/* Append up to one additional cadence of ports to buffer linked list transcription */
	for (i=0; i<60; i++) {
		for (j=0; j<map_width; j++) {
			if (tdm_fit_singular_cnt(vector_map,i)>0) {
				TOKEN_CHECK(vector_map[j][i]) {
					tdm_ll_append(vector_map[j][i]);
				}
			}
			else {
				tdm_ll_append(_TD2P_NUM_EXT_PORTS);
				break;
			}
		}
	}
	tdm_ll_print(linked_list);

	/* Do not retrace if fastest speed is over 42G */
	if (os_enable && lr_enable && lr_50==BOOL_FALSE && lr_100==BOOL_FALSE && lr_120==BOOL_FALSE) {
		TDM_PRINT0("\tRetracing linked list...\n");
		/* Apply retrace algorithm for oversub spread */
		j=0;
		for (i=0; i<lr_idx_limit; i++) {
			if (vector_map[0][i]!=_TD2P_NUM_EXT_PORTS) {
				TDM_PRINT2("\t\t| Pass = %0d of %0d |\n",(++j),retrace_num);
				retrace_result=tdm_ll_retrace(linked_list,speed,tsc,i);
				if (retrace_result!=0) {
					TDM_ERROR1("Linked list retrace returned length mismatch = %0d\n",retrace_result);
				}
#ifdef _SET_TDM_DEV
				if (j<retrace_num) {
					tdm_ll_print(linked_list);
				}
#endif
			}
		}
		tdm_ll_print(linked_list);
	}
		
	/* Convert dynamic list to static calendar */
	tdm_ll_deref(linked_list,tdm_tbl,lr_idx_limit);
	for (j=lr_idx_limit; j<256; j++) {
		TOKEN_CHECK(tdm_tbl[j]) {
			for (i=1; i<60; i++) {
				if (tdm_tbl[lr_idx_limit-i]==_TD2P_NUM_EXT_PORTS) {
					tdm_tbl[lr_idx_limit-i]=tdm_tbl[j];
					tdm_tbl[j]=_TD2P_NUM_EXT_PORTS;
					break;
				}
			}
		}
	}
	
	/* Free vector map object */
	/* for (j=0; j<map_width; j++) {
		TDM_FREE(vector_map[j]);
	}
	TDM_FREE(vector_map); */
	
	/* Cap the table */
	cap=0;
	for (j=254; j>0; j--) {
		TOKEN_CHECK(tdm_tbl[j]) {
			cap=(j+1); 
			break;
		}
	}
	TDM_VERBOSE1("TDM: _____VERBOSE: preprocessed TDM table terminates at index %0d\n", cap);
	
	/* Find loopback TSC */
	if ( cap>0 && (speed[tdm_tbl[cap-1]]>=SPEED_10G) ) {
		cap_tsc=_TD2P_NUM_EXT_PORTS;
		for (j=0; j<NUM_TSC; j++) {
			for (i=0; i<4; i++) {
				if (tsc[j][i]==tdm_tbl[cap-1]) {
					cap_tsc=j;
					break;
				}
			}
			if (cap_tsc!=_TD2P_NUM_EXT_PORTS) {
				break;
			}
		}
		TDM_VERBOSE1("TDM: _____VERBOSE: TDM lookback warp core is %0d\n", cap_tsc);
		cap_lkbk=1;
		for (j=(i-2); j>=0; j--) {
			if (tdm_tbl[j]!=tsc[cap_tsc][0]&&tdm_tbl[j]!=tsc[cap_tsc][1]&&tdm_tbl[j]!=tsc[cap_tsc][2]&&tdm_tbl[j]!=tsc[cap_tsc][3]) {
				cap_lkbk++;
			}
			else {
				break;
			}
		}
		TDM_VERBOSE1("TDM: _____VERBOSE: end of TDM table, lookback spacing is %0d\n", cap_lkbk);
		cap_lpbk=1;
		for (j=0; j<i; j++) {
			if (tdm_tbl[j]!=tsc[cap_tsc][0]&&tdm_tbl[j]!=tsc[cap_tsc][1]&&tdm_tbl[j]!=tsc[cap_tsc][2]&&tdm_tbl[j]!=tsc[cap_tsc][3]) {
				cap_lpbk++;
			}
			else {
				break;
			}
		}
		TDM_VERBOSE1("TDM: _____VERBOSE: end of TDM table, loopback spacing is %0d\n", cap_lpbk);
		if (cap_lpbk <= cap_lkbk) {
			cap+=(cap_lkbk-cap_lpbk);
		}
	}
	TDM_VERBOSE1("TDM: _____VERBOSE: postprocessed TDM table terminates at index %0d\n", cap);
	
	if (cap>lr_idx_limit) {
		TDM_ERROR1("Could not find solution TDM within max length at %0d MHz\n",freq);
		return 0;
	}
	else {
		cap = lr_idx_limit;
	}
	TDM_VERBOSE1("TDM: _____VERBOSE: TDM extended to slot %0d\n",cap);
	
	if (os_enable && !lr_enable) {
		for (j=0; j<cap; j++) {
			if (tdm_tbl[j]==_TD2P_NUM_EXT_PORTS) {
				tdm_tbl[j] = OVSB_TOKEN;
			}
		}
	}
	else {
		for (j=0; j<cap; j++) {
			if (tdm_tbl[j]==_TD2P_NUM_EXT_PORTS) {
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

	if ( lr_enable && os_enable && ((((lr_idx_limit-lr_slot_cnt)*BW_QUANTA)/10)<100) && ((lr_10+lr_20+lr_40+lr_50+lr_100+lr_25)==1) ) {
		TDM_scheduler_filter_refactor(tdm_tbl,lr_idx_limit,lr_vec_cnt);
		refactor_done=BOOL_TRUE;
	}	
	
	for (j=1; j<=accessories; j++) {
		g=tdm_PQ((((10*cap)/accessories)*j))+(j-1);
		for (l=255; l>g; l--) {
			tdm_tbl[l]=tdm_tbl[l-1];
		}
		tdm_tbl[g]=ACC_TOKEN;
	}
	
	/* Smoothing filters */
	if (lr_enable && os_enable && !refactor_done) {
		/* Flat vector translation */
		/* Down */
		for (i=1; i<lr_idx_limit; i++) {
			TOKEN_CHECK(tdm_tbl[i]) {
				if (tdm_arr_exists(tdm_tbl[i],107,vec_trans_port_list)) {
					continue;
				}
				else {
					vec_filter_port=tdm_tbl[i];
					vec_filter_wc=which_tsc(vec_filter_port,tsc);
					if (tdm_arr_append(vec_filter_port,107,vec_trans_port_list)==BOOL_TRUE) {
						last_smooth = check_fit_smooth(tdm_tbl,vec_filter_port,lr_idx_limit,2);
						if (tdm_filter_vector_shift(tdm_tbl,vec_filter_port,DN)) {
							vec_filter_abort=BOOL_FALSE;
							for (j=0; j<lr_idx_limit; j++) {
								if (tdm_tbl[j]==vec_filter_port) {
									if ( (which_tsc(tdm_tbl[j+1],tsc)==vec_filter_wc) ||
										 (which_tsc(tdm_tbl[j+2],tsc)==vec_filter_wc) ||
										 (which_tsc(tdm_tbl[j+3],tsc)==vec_filter_wc) ) {
										vec_filter_abort=BOOL_TRUE;
										break;
									}
								}
							}
							if (check_fit_smooth(tdm_tbl,vec_filter_port,lr_idx_limit,2)>=last_smooth) {
								vec_filter_abort=BOOL_TRUE;
							}
							if (vec_filter_abort) {
								/* This value is set but not used [-Werror=unused-but-set-variable] 
								vec_filter_result=tdm_filter_vector_shift(tdm_tbl,vec_filter_port,UP);
								*/
							}
							else {
								TDM_VERBOSE2("Filter applied: Vector translation down index %0d, port %0d\n",i,vec_filter_port);
								i++;
							}
						}
					}
				}					
			}
		}
		for (i=0; i<107; i++) {vec_trans_port_list[i]=_TD2P_NUM_EXT_PORTS;}
		/* Up */
		for (i=4; i<lr_idx_limit; i++) {
			TOKEN_CHECK(tdm_tbl[i]) {
				if (tdm_arr_exists(tdm_tbl[i],107,vec_trans_port_list)) {
					continue;
				}
				else {
					vec_filter_port=tdm_tbl[i];
					vec_filter_wc=which_tsc(vec_filter_port,tsc);
					if (tdm_arr_append(vec_filter_port,107,vec_trans_port_list)==BOOL_TRUE) {
						last_smooth = check_fit_smooth(tdm_tbl,vec_filter_port,lr_idx_limit,2);
						if (tdm_filter_vector_shift(tdm_tbl,vec_filter_port,UP)) {
							vec_filter_abort=BOOL_FALSE;
							for (j=4; j<lr_idx_limit; j++) {
								if (tdm_tbl[j]==vec_filter_port) {
									if ( (which_tsc(tdm_tbl[j-1],tsc)==vec_filter_wc) ||
										 (which_tsc(tdm_tbl[j-2],tsc)==vec_filter_wc) ||
										 (which_tsc(tdm_tbl[j-3],tsc)==vec_filter_wc) ) {
										vec_filter_abort=BOOL_TRUE;
										break;
									}
								}
							}
							if (check_fit_smooth(tdm_tbl,vec_filter_port,lr_idx_limit,2)>=last_smooth) {
								vec_filter_abort=BOOL_TRUE;
							}
							if (vec_filter_abort) {
								/* This value is set but not used [-Werror=unused-but-set-variable]
								vec_filter_result=tdm_filter_vector_shift(tdm_tbl,vec_filter_port,DN);
								*/
							}
							else {
								TDM_VERBOSE2("Filter applied: Vector translation up index %0d, port %0d\n",i,vec_filter_port);
							}
						}
					}
				}					
			}
		}
		/* Local clump slice 1 (line rate) */
		for (i=1; i<(lr_idx_limit-4); i++) {
			if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i-1]!=OVSB_TOKEN && tdm_tbl[i+1]==OVSB_TOKEN && tdm_tbl[i+2]==OVSB_TOKEN) {
				/* Down direction */
				/* tdm_filter_local_slice(i,tdm_tbl,tsc,DN); */
				tdm_filter_local_slice_dn(i,tdm_tbl,tsc,speed);
			}
		}
		for (i=4; i<(lr_idx_limit-1); i++) {
			if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i+1]!=OVSB_TOKEN && tdm_tbl[i-1]==OVSB_TOKEN && tdm_tbl[i-2]==OVSB_TOKEN) {
				/* Up direction */
				/* tdm_filter_local_slice(i,tdm_tbl,tsc,UP); */
				tdm_filter_local_slice_up(i,tdm_tbl,tsc,speed);
			}
		}
		/* Smoothing filter ancillary slots */
		/* First possible ancillary slot is 13 at lowest freq */
		for (i=13; i<(lr_idx_limit+accessories); i++) {
			if (tdm_tbl[i]==ACC_TOKEN && tdm_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories))>=3) {
				tdm_filter_ancillary_smooth(i,tdm_tbl,lr_idx_limit,accessories);
			}
		}
		/* Port slice translation */
		/* Up direction */
		i=0;
		while (i<lr_idx_limit) {
			/* Find next slice */
			while (i<lr_idx_limit) {i++; if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i-1]==OVSB_TOKEN) {break;}}
			filter_slice_size=tdm_slice_size_local(i,tdm_tbl,lr_idx_limit);
			if (filter_slice_size>3) {
				filter_slice_start=i;
				filter_slice_found=FAIL;
				for (j=4; j<lr_idx_limit; j++) {
					if (tdm_tbl[i+j]==OVSB_TOKEN && tdm_tbl[i+j+1]==OVSB_TOKEN) {
						filter_slice_end=(i+j-1);
						filter_slice_found=PASS;
						filter_slice_wc_0=which_tsc(tdm_tbl[filter_slice_end],tsc);
						filter_slice_wc_1=which_tsc(tdm_tbl[filter_slice_end-1],tsc);
						/* Check if VBS violation (ignore within slice itself) */
						if ( (which_tsc(tdm_tbl[filter_slice_end+3],tsc)==filter_slice_wc_0) || 
							 (which_tsc(tdm_tbl[filter_slice_end+4],tsc)==filter_slice_wc_0) ||
							 (which_tsc(tdm_tbl[filter_slice_end+3],tsc)==filter_slice_wc_1) ) {
							filter_slice_found=FAIL;
						}
						/* Check if LLS violation (ignore within slice itself) */
						for (k=filter_slice_end; k>=i; k--) {
							TOKEN_CHECK(tdm_tbl[k]) {
								if (tdm_tbl[k]<=SPEED_42G_HG2) {
									filter_slice_dist=check_same_port_dist_dn(k,tdm_tbl,(lr_idx_limit+accessories));
									if ( (filter_slice_dist<=LLS_MIN_SPACING) && ((k+filter_slice_dist)>filter_slice_end) ) {
										filter_slice_found=FAIL;
										break;
									}
								}
							}
						}
						/* Check if still bounded */
						if ((i+j)>=(lr_idx_limit+accessories)) {
							filter_slice_found=FAIL;
							break;
						}
						if (filter_slice_found) {
							break;
						}
					}
				}
				if (filter_slice_found) {
					TDM_VERBOSE3("Filter applied: Port slice translation - Size: %0d | Begin: %0d | End: %0d\n",filter_slice_size,filter_slice_start,filter_slice_end);
					for (j=(filter_slice_end+1); j>=(filter_slice_start+(filter_slice_size/2)); j--) {
						tdm_tbl[j]=tdm_tbl[j-1];
					}
					tdm_tbl[filter_slice_start+(filter_slice_size/2)]=OVSB_TOKEN;
				}
			}
		}
		
		/* Local clump slice 2 (line rate) */
		for (i=1; i<(lr_idx_limit+accessories); i++) {
			if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i-1]!=OVSB_TOKEN && tdm_tbl[i+1]==OVSB_TOKEN && tdm_tbl[i+2]==OVSB_TOKEN) {
				/* Down direction */
				if ((tdm_tbl[i]>SPEED_42G_HG2) || (check_same_port_dist_dn(i,tdm_tbl,(lr_idx_limit+accessories))>LLS_MIN_SPACING)) {
					local_clump_wc=which_tsc(tdm_tbl[i],tsc);
					if ((which_tsc(tdm_tbl[i+3],tsc)!=local_clump_wc) && (which_tsc(tdm_tbl[i+4],tsc)!=local_clump_wc)) {
						tdm_tbl[i+1]=tdm_tbl[i];
						tdm_tbl[i]=OVSB_TOKEN;
						TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: (2) Local slice oversub index %0d\n",i);
					}
				}
			}
		}
		for (i=(lr_idx_limit+accessories-2); i>=4; i--) {
			if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i+1]!=OVSB_TOKEN && tdm_tbl[i-1]==OVSB_TOKEN && tdm_tbl[i-2]==OVSB_TOKEN) {
				/* Up direction */
				if ((tdm_tbl[i]>SPEED_42G_HG2) || (check_same_port_dist_up(i,tdm_tbl,(lr_idx_limit+accessories))>LLS_MIN_SPACING)) {
					local_clump_wc=which_tsc(tdm_tbl[i],tsc);
					if ((which_tsc(tdm_tbl[i-3],tsc)!=local_clump_wc) && (which_tsc(tdm_tbl[i-4],tsc)!=local_clump_wc)) {
						tdm_tbl[i-1]=tdm_tbl[i];
						tdm_tbl[i]=OVSB_TOKEN;
						TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: (2) Local slice oversub index %0d\n",i);
					}
				}
			}
		}

		/* Token slice translation */
		
		
		/* Local slice translation */
#ifdef BCM_TRIDENT2PLUS_SUPPORT
		if ( (freq<760) || (freq>=760 && num_40g==0) ) {
#endif
			for (i=1; i<(lr_idx_limit-4); i++) {
				if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i-1]!=OVSB_TOKEN && tdm_tbl[i+1]==OVSB_TOKEN && tdm_tbl[i+2]!=OVSB_TOKEN) {
					i=tdm_filter_slice_dn(i,tdm_tbl,lr_idx_limit,tsc,speed);
				}
			}
			for (i=4; i<(lr_idx_limit-1); i++) {
				if (tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i+1]!=OVSB_TOKEN && tdm_tbl[i-1]==OVSB_TOKEN && tdm_tbl[i-2]!=OVSB_TOKEN) {
					i=tdm_filter_slice_up(i,tdm_tbl,lr_idx_limit,tsc,speed);
				}
			}
#ifdef BCM_TRIDENT2PLUS_SUPPORT
		}
#endif
		/* Local clump slice (oversub) */
		/*
			if ( tdm_slice_size(OVSB_TOKEN,tdm_tbl,lr_idx_limit)<4 ) {
				tdm_filter_local_slice_oversub(tdm_tbl,lr_idx_limit,tsc,speed);
			}
		*/
		/* Oversub and linerate dithering filter */
		timeout=DITHER_PASS;
		while ( tdm_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>1 && tdm_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>=tdm_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && ((--timeout)>0) ) {
			tdm_filter_dither(tdm_tbl,lr_idx_limit,accessories,tsc,DITHER_THRESHOLD,speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>1 && tdm_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>=tdm_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && ((--timeout)>0) ) {
			tdm_filter_dither(tdm_tbl,lr_idx_limit,accessories,tsc,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE),speed);
		}
		timeout=DITHER_PASS;
		while ( tdm_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>1 && tdm_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))>=tdm_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && ((--timeout)>0) ) {
			tdm_filter_dither(tdm_tbl,lr_idx_limit,accessories,tsc,(DITHER_THRESHOLD-DITHER_SUBPASS_STEP_SIZE-DITHER_SUBPASS_STEP_SIZE),speed);
		}
		/* Oversub fine dithering filter */
		if (tdm_slice_size(1, tdm_tbl, (lr_idx_limit+accessories))==2) {
			/* 4 slices (normal) */
			for (i=3; i<(lr_idx_limit+accessories); i++) {
				if (tdm_tbl[i-3]!=OVSB_TOKEN && tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i+3]!=OVSB_TOKEN && tdm_tbl[i+6]!=OVSB_TOKEN && tdm_slice_size_local((i-3), tdm_tbl, lr_idx_limit)==2 && tdm_slice_size_local(i, tdm_tbl, lr_idx_limit)==2 && tdm_slice_size_local((i+3), tdm_tbl, lr_idx_limit)==2 && tdm_slice_size_local((i+6), tdm_tbl, lr_idx_limit)==2) {
					if (tdm_filter_fine_dither(i,tdm_tbl,lr_idx_limit,accessories,tsc)==FAIL) {
						break;
					}
				}
			}
			/* Do it again at 3 slices (normal) */
			for (i=3; i<(lr_idx_limit+accessories); i++) {
				if (tdm_tbl[i-3]!=OVSB_TOKEN && tdm_tbl[i]!=OVSB_TOKEN && tdm_tbl[i+3]!=OVSB_TOKEN && tdm_slice_size_local((i-3), tdm_tbl, lr_idx_limit)==2 && tdm_slice_size_local(i, tdm_tbl, lr_idx_limit)==2 && tdm_slice_size_local((i+3), tdm_tbl, lr_idx_limit)==2) {
					if (tdm_filter_fine_dither(i,tdm_tbl,lr_idx_limit,accessories,tsc)==FAIL) {
						break;
					}
				}
			}
		}
	}
	
	TDM_BIG_BAR
	TDM_VERBOSE9("(1G - %0d) (10G - %0d) (20G - %0d) (25G - %0d) (40G - %0d) (50G - %0d) (100G - %0d) (120G - %0d) (Number of Oversub Types - %0d)\n", z8, z1, z2, z6, z3, z5, z4, z7, (os_1 + os_10 + os_20 + os_25 + os_40 + os_50 + os_100 + os_120));
	if ((os_1 + os_10 + os_20 + os_25 + os_40 + os_50 + os_100 + os_120) > OS_GROUP_NUM) {
		TDM_ERROR0("Oversub speed type limit exceeded\n");
		return 0;
	}
	if (((os_1 + os_10 + os_20 + os_25 + os_40 + os_50 + os_100)==OS_GROUP_NUM && (z6>_TD2P_OS_GROUP_LEN || z1>_TD2P_OS_GROUP_LEN || z2>_TD2P_OS_GROUP_LEN)) || ((os_1 + os_10 + os_20 + os_25 + os_40 + os_50 + os_100)>=(OS_GROUP_NUM-1) && (z6>32 || z1>32 || z2>32)) || ((os_1 + os_10 + os_20 + os_25 + os_40 + os_50 + os_100)>=(OS_GROUP_NUM-2) && (z6>48 || z1>48)) || ((os_1 + os_10 + os_20 + os_25 + os_40 + os_50 + os_100)>=(OS_GROUP_NUM-3) && (z6>64 || z1>64))) {
		TDM_ERROR0("Oversub bucket overflow\n");
		return 0;
	}
	/* TD2/TD2+ doesn't use 25G and 50G */
	z5=0; z6=0;
	/* Sort and populate oversub speed group */
	while (z8 > 0) {
		if (fill_ovs(&z8, speed, tsc, os1, bucket1, &z11, bucket2, &z22, bucket3, &z33, bucket4, &z44, bucket5, &z55, bucket6, &z66, bucket7, &z77, bucket8, &z88)==FAIL) {
			TDM_ERROR0("Could not sort 1G oversub speed groups\n");
			break;
		}
	}
	z11 = (z11>0) ? _TD2P_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TD2P_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TD2P_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TD2P_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TD2P_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TD2P_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TD2P_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TD2P_OS_GROUP_LEN:0;	
	while (z1 > 0) {
		if (fill_ovs(&z1, speed, tsc, os10, bucket1, &z11, bucket2, &z22, bucket3, &z33, bucket4, &z44, bucket5, &z55, bucket6, &z66, bucket7, &z77, bucket8, &z88)==FAIL) {
			TDM_ERROR0("Could not sort 10G oversub speed groups\n");
			break;
		}
	}
	z11 = (z11>0) ? _TD2P_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TD2P_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TD2P_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TD2P_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TD2P_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TD2P_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TD2P_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TD2P_OS_GROUP_LEN:0;
	while (z2 > 0) {
		if (fill_ovs(&z2, speed, tsc, os20, bucket1, &z11, bucket2, &z22, bucket3, &z33, bucket4, &z44, bucket5, &z55, bucket6, &z66, bucket7, &z77, bucket8, &z88)==FAIL) {
			TDM_ERROR0("Could not sort 20G oversub speed groups\n");
			break;
		}
	}
	z11 = (z11>0) ? _TD2P_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TD2P_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TD2P_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TD2P_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TD2P_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TD2P_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TD2P_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TD2P_OS_GROUP_LEN:0;
	while (z3 > 0) {
		if (fill_ovs_simple(&z3, os40, bucket1, &z11, bucket2, &z22, bucket3, &z33, bucket4, &z44, bucket5, &z55, bucket6, &z66, bucket7, &z77, bucket8, &z88)==FAIL) {
			TDM_ERROR0("Could not sort 40G oversub speed groups\n");
			break;
		}
	}
	z11 = (z11>0) ? _TD2P_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TD2P_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TD2P_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TD2P_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TD2P_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TD2P_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TD2P_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TD2P_OS_GROUP_LEN:0;
	while (z4 > 0) {
		if (fill_ovs_simple(&z4, os100, bucket1, &z11, bucket2, &z22, bucket3, &z33, bucket4, &z44, bucket5, &z55, bucket6, &z66, bucket7, &z77, bucket8, &z88)==FAIL) {
			TDM_ERROR0("Could not sort 100G oversub speed groups\n");
			break;
		}
	}
	z11 = (z11>0) ? _TD2P_OS_GROUP_LEN:0;
	z22 = (z22>0) ? _TD2P_OS_GROUP_LEN:0;
	z33 = (z33>0) ? _TD2P_OS_GROUP_LEN:0;
	z44 = (z44>0) ? _TD2P_OS_GROUP_LEN:0;
	z55 = (z55>0) ? _TD2P_OS_GROUP_LEN:0;
	z66 = (z66>0) ? _TD2P_OS_GROUP_LEN:0;
	z77 = (z77>0) ? _TD2P_OS_GROUP_LEN:0;
	z88 = (z88>0) ? _TD2P_OS_GROUP_LEN:0;
	while (z7 > 0) {
		if (fill_ovs_simple(&z7, os120, bucket1, &z11, bucket2, &z22, bucket3, &z33, bucket4, &z44, bucket5, &z55, bucket6, &z66, bucket7, &z77, bucket8, &z88)==FAIL) {
			TDM_ERROR0("Could not sort 120G oversub speed groups\n");
			break;
		}
	}
	
	return 1;
	
}


/**
@name: tdm_mem_transpose
@param:

Transpose vector in vector map using memcpy standard function
**/
void tdm_mem_transpose(unsigned char **map, int src, int dst)
{
	int i;
	
	/* memcpy( &(map[dst]), &(map[src]), sizeof(map[0]) ); // Requires perfectly contiguous memory */
	for (i=0; i<VEC_MAP_LEN; i++) {
		memcpy( &(map[dst][i]), &(map[src][i]), sizeof(unsigned char) );
	}
	/* Clear source vector */
	for (i=0; i<VEC_MAP_LEN; i++) {
		map[src][i]=_TD2P_NUM_EXT_PORTS;
	}
	
}


void tdm_sticky_transpose(unsigned char **map, int lr_idx_limit, int src, int dst)
{
	int transpose_tmp, k;
	
	transpose_tmp=map[dst][tdm_map_find_y(map,lr_idx_limit,dst,0)];
	
	for (k=0; k<lr_idx_limit; k++) {
		map[dst][k]=(map[dst][k]==_TD2P_NUM_EXT_PORTS)?(_TD2P_NUM_EXT_PORTS):(map[src][tdm_map_find_y(map,lr_idx_limit,src,0)]);
	}
	for (k=0; k<lr_idx_limit; k++) {
		map[src][k]=(map[src][k]==_TD2P_NUM_EXT_PORTS)?(_TD2P_NUM_EXT_PORTS):(transpose_tmp);
	}
	
}


/**
@name: tdm_map_distance_x
@param: unsigned char **, int, int, int

Returns diagonal distance between two nearest neighbor nodes in the x dimension
**/
int tdm_map_distance_x(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2) 
{
	int i;
	unsigned int a=0, b=0, dist;
	
	for (i=0; i<limit; i++) {
		if (map[idx1][i]!=_TD2P_NUM_EXT_PORTS) {
			a = i;
			break;
		}
	}
	for (i=0; i<limit; i++) {
		if (map[idx2][i]!=_TD2P_NUM_EXT_PORTS) {
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
@name: tdm_map_shortest_distance_calc
@param: unsigned char **, int, int, int

Returns shortest vertical distance between nodes
**/
int tdm_map_shortest_distance_calc(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2, unsigned int start)
{
	int i;
	unsigned int a=start, dist, up=limit, down=limit;
	
	for (i=0; i<limit; i++) {
		if ((a+i)>=limit) {break;}
		if (map[idx2][i+a]!=_TD2P_NUM_EXT_PORTS) {
			down = (i+a);
			break;
		}
	}
	for (i=0; i<=a; i++) {
		if (map[idx2][a-i]!=_TD2P_NUM_EXT_PORTS) {
			up = (a-i);
			break;
		}
	}
	
	dist = (tdm_abs(up-a)>tdm_abs(down-a))?(tdm_abs(down-a)):(tdm_abs(up-a));
	
	return dist;
}


/**
@name: tdm_map_shortest_distance
@param: unsigned char **, int, int, int

Calls shortest distance calc and returns only shortest among all nodes
**/
int tdm_map_shortest_distance(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2) 
{
	int i, last_dist, dist=_TD2P_NUM_EXT_PORTS;
	
	for (i=0; i<limit; i++) {
		if (map[idx1][i]!=_TD2P_NUM_EXT_PORTS) {
			last_dist = tdm_map_shortest_distance_calc(map,limit,idx1,idx2,i);
			if (last_dist < dist) {
				dist = last_dist;
			}
		}
	}
	
	return dist;
}


/**
@name: tdm_map_distance_y
@param: unsigned char **, int, int, int

Returns diagonal distance between two nearest neighbor nodes in the y dimension
**/
int tdm_map_distance_y(unsigned char **map, int limit, unsigned int idx1, unsigned int idx2) 
{
	int i;
	unsigned int a=0, b=0, dist;
	
	for (i=0; i<VEC_MAP_WID; i++) {
		if (map[i][idx1]!=_TD2P_NUM_EXT_PORTS) {
			a = i;
			break;
		}
	}
	for (i=0; i<VEC_MAP_WID; i++) {
		if (map[i][idx2]!=_TD2P_NUM_EXT_PORTS) {
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
@name: tdm_map_transpose
@param: unsigned char **, int, int, int

Transpose in-place the position of two vectors of the same speed. Returns if they are different speeds.
**/
int tdm_map_transpose(unsigned char **map, int limit, unsigned int xpos_1, unsigned int xpos_2)
{
	int i, store1=_TD2P_NUM_EXT_PORTS, store2=_TD2P_NUM_EXT_PORTS, count1=0, count2=0;
	
	for (i=0; i<limit; i++) {
		if (map[xpos_1][i]!=_TD2P_NUM_EXT_PORTS) {
			store1=map[xpos_1][i];
			count1++;
		}
	}
	for (i=0; i<limit; i++) {
		if (map[xpos_2][i]!=_TD2P_NUM_EXT_PORTS) {
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
@name: tdm_vector_rotate_step
@param: int[], int, int

Performs rotation transform on input vector array of nodes in integer steps, can wrap around feature space
**/
void tdm_vector_rotate_step(unsigned char vector[], int size, int step)
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
@name: tdm_vector_rotate
@param: int[], int, int

Wraps rotation transform contingent upon the skew of the vector
**/
void tdm_vector_rotate(unsigned char vector[], int size, int step)
{
	int i;
	
	/* 
	 * If congruent vectors are allowed to wrap in rotation their fractional average spaced nodes will
	 * skew. That will cause hang in aligning the vectors.
	 */ 
	if (step > 0) {
		for (i=0; i<step; i++) {
			if (vector[size-1]!=_TD2P_NUM_EXT_PORTS && i<step) {
				while (vector[0]==_TD2P_NUM_EXT_PORTS) {
					tdm_vector_rotate_step(vector, size, -1);
				}
			}
			else {
				tdm_vector_rotate_step(vector, size, 1);
			}
		}
	}
	else {
		tdm_vector_rotate_step(vector, size, step);
	}
}


/**
@name: tdm_vector_translate
@param: int[], int, int

Performs transposition transform on input vector array of nodes in integer steps, always clamped to feature space
**/
void tdm_vector_translate(unsigned char vector[], int size, int step)
{
	int i,j;
	
	for (j=(tdm_abs(step)); j>0; j--) {
		if (vector[0]!=_TD2P_NUM_EXT_PORTS) {
			break;
		}
		for (i=0; i<size; i++) {
			vector[i]=vector[i+1];
		}
	}
	
}


/**
@name: tdm_vector_clear
@param: int[], int, int

Remove vector at index in TDM vector map
**/
void tdm_vector_clear(unsigned char **map, int yy, int len)
{
	int i;
	
	for (i=0; i<len; i++) {
		map[yy][i]=_TD2P_NUM_EXT_PORTS;
	}
	
}


/**
@name: tdm_nodal_transmute_lr
@param:

Transmutes individual nodes. Use for line rate.
**/
int tdm_nodal_transmute_lr(unsigned char **map, int freq, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt)
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
	
	do {++up;} while ( ((tdm_fit_singular_cnt(map,(row-up))>0) || (tdm_fit_prox_node(map,(row),lr_vec_cnt,col,tsc,(row-up))==FAIL)) && ( (map[col][row-up]!=map[col][row]) || ((row-up)>0) ) );
	do {++down;} while ( ((tdm_fit_singular_cnt(map,(row+down))>0) || (tdm_fit_prox_node(map,(row),lr_vec_cnt,col,tsc,(row+down))==FAIL)) && ( (map[col][row+down]!=map[col][row]) || ((row+down)<lr_idx_limit) ) );
	shift = (up>down) ? (row+down):(row-up);
	if ( (tdm_fit_singular_cnt(map,shift)==0) && (tdm_fit_prox_node(map,shift,lr_vec_cnt,col,tsc,shift)==PASS) ) {
		if (tdm_abs(row-shift) > jitter) {
			#ifdef _SET_TDM_DEV
			TDM_PRINT0("\n");
			TDM_WARN4("Node transmute @ [%0d,%0d] shift %0d will exceed jitter threshold by %0d\n",col,row,shift,tdm_abs(row-shift));
			#endif
		}
		map[col][shift]=map[col][row];
		map[col][row]=_TD2P_NUM_EXT_PORTS;
		#ifdef _SET_TDM_DEV
		TDM_PRINT4("\n\tNode %0d transmute for line rate @ [%0d,%0d] shift %0d\n\n",map[col][shift],col,row,shift);
		#endif
		if ( (map[col][shift+1]!=_TD2P_NUM_EXT_PORTS) ||
		     (map[col][shift+2]!=_TD2P_NUM_EXT_PORTS) ||
		     (map[col][shift+3]!=_TD2P_NUM_EXT_PORTS) ||
		     (map[col][shift-1]!=_TD2P_NUM_EXT_PORTS) ||
		     (map[col][shift-2]!=_TD2P_NUM_EXT_PORTS) ||
		     (map[col][shift-3]!=_TD2P_NUM_EXT_PORTS) ) {
				map[col][row]=map[col][shift];
				map[col][shift]=_TD2P_NUM_EXT_PORTS;
				#ifdef _SET_TDM_DEV
				TDM_PRINT3("\tReverted illegal transmute at [%0d,%0d]=%0d\n",col,shift,map[col][row]);
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
@name: tdm_nodal_transmute_slice
@param:

Transmutes individual nodes with preference for slicing empty regions. Use for oversub.
**/
int tdm_nodal_slice(unsigned char **map, int freq, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt, int direction)
{
	int i, node_transmuted=BOOL_FALSE, result=FAIL, prox=PASS;
	
	if (direction==1) {
		if (map[col][row]!=_TD2P_NUM_EXT_PORTS) {
			if (map[col][row+1]==_TD2P_NUM_EXT_PORTS && map[col][row+2]==_TD2P_NUM_EXT_PORTS) {
				map[col][row+1]=map[col][row];
				map[col][row]=_TD2P_NUM_EXT_PORTS;
				node_transmuted=BOOL_TRUE;
				result=PASS;
			}
			if (node_transmuted == BOOL_TRUE) {
				#ifdef _SET_TDM_DEV
				TDM_PRINT4("\n\tNode %0d transmute for slice @ [%0d,%0d] shift %0d\n",map[col][row+1],col,row,(row+1));
				#endif
				for (i=1; i<4; i++) {
					if (map[col][row+1]==map[col][row+1+i]) {
						prox=FAIL;
						break;
					}
				}
				if ( (tdm_fit_prox(map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) || (prox==FAIL) ) {
					map[col][row]=map[col][row+1];
					map[col][row+1]=_TD2P_NUM_EXT_PORTS;
					#ifdef _SET_TDM_DEV
					TDM_PRINT3("\tReverted illegal transmute at [%0d,%0d]=%0d\n",col,row,map[col][row]);
					#endif
					result=FAIL;
				}
			}
		}
	}
	else if (direction==-1) {
		if (map[col][row]!=_TD2P_NUM_EXT_PORTS) {
			if (map[col][row-1]==_TD2P_NUM_EXT_PORTS && map[col][row-2]==_TD2P_NUM_EXT_PORTS) {
				map[col][row-1]=map[col][row];
				map[col][row]=_TD2P_NUM_EXT_PORTS;
				node_transmuted=BOOL_TRUE;
				result=PASS;
			}
			if (node_transmuted == BOOL_TRUE) {
				#ifdef _SET_TDM_DEV
				TDM_PRINT4("\n\tNode %0d transmute for slice @ [%0d,%0d] shift %0d\n",map[col][row-1],col,row,(row-1));
				#endif
				for (i=1; i<4; i++) {
					if (map[col][row-1]==map[col][row-1-i]) {
						prox=FAIL;
						break;
					}
				}
				if ( (tdm_fit_prox(map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) || (prox==FAIL) ) {
					map[col][row]=map[col][row-1];
					map[col][row-1]=_TD2P_NUM_EXT_PORTS;
					#ifdef _SET_TDM_DEV
					TDM_PRINT3("\tReverted illegal transmute at [%0d,%0d]=%0d\n",col,row,map[col][row]);
					#endif
					result=FAIL;
				}
			}
		}
	}
	
	return result;
	
}


/**
@name: tdm_nodal_transmute_slice_lr
@param:

Transmutes individual nodes with preference for slicing empty regions. Use for line rate.
**/
int tdm_nodal_slice_lr(unsigned char **map, int freq, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt, int direction, int curr_cnt)
{
	int i, node_transmuted=BOOL_FALSE, result=FAIL, prox=PASS, cnt=1;
	
	if (direction==1) {
		if (map[col][row]!=_TD2P_NUM_EXT_PORTS && map[col][row+1]==_TD2P_NUM_EXT_PORTS) {
			for (i=1; i<60; i++) {
				if ((row+1+i)<lr_idx_limit) {
					if (tdm_fit_singular_cnt(map,(row+1+i))>0) {++cnt;}
					else {break;}
				}
				else {break;}
			}
			if (map[col][row+2]==_TD2P_NUM_EXT_PORTS || cnt<curr_cnt) {
				map[col][row+1]=map[col][row];
				map[col][row]=_TD2P_NUM_EXT_PORTS;
				node_transmuted=BOOL_TRUE;
				result=PASS;
			}
			if (node_transmuted == BOOL_TRUE) {
				#ifdef _SET_TDM_DEV
				TDM_PRINT4("\n\tNode %0d transmute for slice @ [%0d,%0d] shift %0d\n",map[col][row+1],col,row,(row+1));
				#endif
				for (i=1; i<4; i++) {
					if (map[col][row+1]==map[col][row+1+i]) {
						prox=FAIL;
						break;
					}
				}
				if ( (tdm_fit_prox(map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) || (prox==FAIL) ) {
					map[col][row]=map[col][row+1];
					map[col][row+1]=_TD2P_NUM_EXT_PORTS;
					#ifdef _SET_TDM_DEV
					TDM_PRINT3("\tReverted illegal transmute at [%0d,%0d]=%0d\n",col,row,map[col][row]);
					#endif
					result=FAIL;
				}
			}
		}
	}
	else if (direction==-1) {
		if (map[col][row]!=_TD2P_NUM_EXT_PORTS && map[col][row-1]==_TD2P_NUM_EXT_PORTS) {
			for (i=1; i<60; i++) {
				if ((row-1-i)>0) {
					if (tdm_fit_singular_cnt(map,(row-1-i))>0) {++cnt;}
					else {break;}
				}
				else {break;}
			}
			if (map[col][row-2]==_TD2P_NUM_EXT_PORTS || cnt<curr_cnt) {
				map[col][row-1]=map[col][row];
				map[col][row]=_TD2P_NUM_EXT_PORTS;
				node_transmuted=BOOL_TRUE;
				result=PASS;
			}
			if (node_transmuted == BOOL_TRUE) {
				#ifdef _SET_TDM_DEV
				TDM_PRINT4("\n\tNode %0d transmute for slice @ [%0d,%0d] shift %0d\n",map[col][row-1],col,row,(row-1));
				#endif
				for (i=1; i<4; i++) {
					if (map[col][row-1]==map[col][row-1-i]) {
						prox=FAIL;
						break;
					}
				}
				if ( (tdm_fit_prox(map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) || (prox==FAIL) ) {
					map[col][row]=map[col][row-1];
					map[col][row-1]=_TD2P_NUM_EXT_PORTS;
					#ifdef _SET_TDM_DEV
					TDM_PRINT3("\tReverted illegal transmute at [%0d,%0d]=%0d\n",col,row,map[col][row]);
					#endif
					result=FAIL;
				}
			}
		}
	}
	
	return result;
	
}


/**
@name: tdm_nodal_transmute
@param:

Transmutes individual nodes and qualifies with protovector subject to jitter contraints
**/
int tdm_nodal_transmute(unsigned char **vector_map, int freq, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int lr_idx_limit, int row, int col, int lr_vec_cnt)
{
	int node_transmuted, node=BLANK;
	
	if (vector_map[col][row]!=_TD2P_NUM_EXT_PORTS) {
		node_transmuted = BOOL_FALSE;
		/* if (tdm_fit_singular_cnt(vector_map,(row-1))>0 && tdm_fit_singular_cnt(vector_map,(row+1))==0 && tdm_fit_singular_cnt(vector_map,(row+2))==0) { */
		if (tdm_fit_singular_cnt(vector_map,(row+1))==0 && tdm_fit_singular_cnt(vector_map,(row+2))==0) {
			vector_map[col][row+1]=vector_map[col][row];
			vector_map[col][row]=_TD2P_NUM_EXT_PORTS;
			node_transmuted = BOOL_TRUE;
			#ifdef _SET_TDM_DEV
			TDM_VERBOSE3("transmuted [%0d,%0d]=%0d down\n",col,row,vector_map[col][row+1]);
			#endif
		}
		/* else if (row>1 && tdm_fit_singular_cnt(vector_map,(row+1))>0 && tdm_fit_singular_cnt(vector_map,(row-1))==0 && tdm_fit_singular_cnt(vector_map,(row-2))==0) { */
		else if (row>1 && tdm_fit_singular_cnt(vector_map,(row-1))==0 && tdm_fit_singular_cnt(vector_map,(row-2))==0) {
			vector_map[col][row-1]=vector_map[col][row];
			vector_map[col][row]=_TD2P_NUM_EXT_PORTS;
			node_transmuted = BOOL_TRUE;
			#ifdef _SET_TDM_DEV
			TDM_VERBOSE3("transmuted [%0d,%0d]=%0d up\n",col,row,vector_map[col][row-1]);
			#endif
		}
		if (node_transmuted == BOOL_TRUE) {
			node = row;
			if (tdm_vector_diff(vector_map,freq,lr_idx_limit,col,speed,tsc,traffic)!=PASS || tdm_fit_prox(vector_map,lr_idx_limit,VEC_MAP_WID,col,tsc)!=PASS) {
				if (vector_map[col][row-1]!=_TD2P_NUM_EXT_PORTS) {
					vector_map[col][row]=vector_map[col][row-1];
					vector_map[col][row-1]=_TD2P_NUM_EXT_PORTS;
				}
				else if (vector_map[col][row+1]!=_TD2P_NUM_EXT_PORTS) {
					vector_map[col][row]=vector_map[col][row+1];
					vector_map[col][row+1]=_TD2P_NUM_EXT_PORTS;
				}
				#ifdef _SET_TDM_DEV
				TDM_VERBOSE3("reverted illegal transmute at [%0d,%0d]=%0d\n",col,row,vector_map[col][row]);
				#endif
			}
		}
	}
	
	return node;
	
}


/**
@name: tdm_fit_distal_1
@param: unsigned char **, int, int

Given x index, determines fit based on if nodes of the current vector are locationed at a distal maximum from nodes of other vectors
*** Returns the x_pos2 coordinate of recommended transpose ***
*** This only matters if oversub is enabled, to increase performance of oversub arbiter ***
**/
int tdm_fit_distal_pos1(unsigned char **map, int lim, int node_x, int lr_slot_cnt) {
	int i, j, v, n;
	/* This value is set but not used [-Werror=unused-but-set-variable]
	int vec_prox_chk;
	*/
	/* unsigned char ovs_no_clump_idx; */
	
	/* Transform vectors to avoid large clusters of OVS tokens once vectors are compressed */
	/* This value is set but not used [-Werror=unused-but-set-variable]
	vec_prox_chk=PASS;
	*/
	v=0; n=_TD2P_NUM_EXT_PORTS;
	for (j=0; j<lim; j++) {
		if ((v>((lim/lr_slot_cnt)+1))) {
			/* Find nearest vectored oversub node as reference, we cannot use a null slot */
			for (i=0; i<v; i++) {
				if (tdm_map_search_x(map,(j-i))==OVSB_TOKEN) {
					n=(j-i);
					break;
				}
			}
			if ((tdm_map_search_x(map,n)!=_TD2P_NUM_EXT_PORTS)) {
				/* This value is set but not used [-Werror=unused-but-set-variable]
				vec_prox_chk=FAIL;
				*/
				break;
			}
		}
		if ((tdm_map_search_x(map,j)==OVSB_TOKEN||tdm_map_search_x(map,j)==_TD2P_NUM_EXT_PORTS) &&
			(tdm_map_search_x(map,(j+1))==OVSB_TOKEN||tdm_map_search_x(map,(j+1))==_TD2P_NUM_EXT_PORTS)) {
			v++;
		}
		else {
			v=0;
		}
	}
	
	return n;
}


/**
@name: tdm_fit_distal_2
@param: unsigned char **, int, int

Given x index, determines fit based on if nodes of the current vector are locationed at a distal maximum from nodes of other vectors
*** Returns the x_pos1 coordinate of recommended transpose ***
*** This only matters if oversub is enabled, to increase performance of oversub arbiter ***
**/
int tdm_fit_distal_pos2(unsigned char **map, int lim, int node_x, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int os_param_spd) {
	int j, n=0, prox1d=0, prox2d=0, prox3d=0, prox1u=0, prox2u=0, prox3u=0;
	unsigned char ovs_no_clump_idx=1;
	
	if (tdm_map_find_x(map,lim,(n+1),0)!=_TD2P_NUM_EXT_PORTS) {
		if (tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n+1),0),0)!=_TD2P_NUM_EXT_PORTS) {
			prox1d=which_tsc(map[tdm_map_find_x(map,lim,(n+1),0)][tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n+1),0),0)],tsc);
		}
		else {
			prox1d=_TD2P_NUM_EXT_PORTS;
		}
	}
	else {
		prox1d=_TD2P_NUM_EXT_PORTS;
	}
	if (tdm_map_find_x(map,lim,(n+2),0)!=_TD2P_NUM_EXT_PORTS) {
		if (tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n+2),0),0)!=_TD2P_NUM_EXT_PORTS) {
			prox2d=which_tsc(map[tdm_map_find_x(map,lim,(n+2),0)][tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n+2),0),0)],tsc);
		}
		else {
			prox2d=_TD2P_NUM_EXT_PORTS;
		}
	}
	else {
		prox2d=_TD2P_NUM_EXT_PORTS;
	}
	if (tdm_map_find_x(map,lim,(n+3),0)!=_TD2P_NUM_EXT_PORTS) {
		if (tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n+3),0),0)!=_TD2P_NUM_EXT_PORTS) {
			prox3d=which_tsc(map[tdm_map_find_x(map,lim,(n+3),0)][tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n+3),0),0)],tsc);
		}
		else {
			prox3d=_TD2P_NUM_EXT_PORTS;
		}
	}
	else {
		prox3d=_TD2P_NUM_EXT_PORTS;
	}
	if (tdm_map_find_x(map,lim,(n-1),0)!=_TD2P_NUM_EXT_PORTS) {
		if (tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n-1),0),0)!=_TD2P_NUM_EXT_PORTS) {
			prox1u=which_tsc(map[tdm_map_find_x(map,lim,(n-1),0)][tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n-1),0),0)],tsc);
		}
		else {
			prox1u=_TD2P_NUM_EXT_PORTS;
		}
	}
	else {
		prox1u=_TD2P_NUM_EXT_PORTS;
	}
	if (tdm_map_find_x(map,lim,(n-2),0)!=_TD2P_NUM_EXT_PORTS) {
		if (tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n-2),0),0)!=_TD2P_NUM_EXT_PORTS) {
			prox2u=which_tsc(map[tdm_map_find_x(map,lim,(n-2),0)][tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n-2),0),0)],tsc);
		}
		else {
			prox2u=_TD2P_NUM_EXT_PORTS;
		}
	}
	else {
		prox2u=_TD2P_NUM_EXT_PORTS;
	}
	if (tdm_map_find_x(map,lim,(n-3),0)!=_TD2P_NUM_EXT_PORTS) {
		if (tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n-3),0),0)!=_TD2P_NUM_EXT_PORTS) {
			prox3u=which_tsc(map[tdm_map_find_x(map,lim,(n-3),0)][tdm_map_find_y(map,lim,tdm_map_find_x(map,lim,(n-3),0),0)],tsc);
		}
		else {
			prox3u=_TD2P_NUM_EXT_PORTS;
		}
	}
	else {
		prox3u=_TD2P_NUM_EXT_PORTS;
	}
	ovs_no_clump_idx=_TD2P_NUM_EXT_PORTS;
	for (j=1; j<VEC_MAP_WID; j++) {
		if (tdm_map_find_y(map,lim,j,0)!=_TD2P_NUM_EXT_PORTS) {
			/* Only one type of 25G vector per core bandwidth */
			if (((speed[map[j][tdm_map_find_y(map,lim,j,0)]])/1000)==25) {
				if ((((speed[map[j][tdm_map_find_y(map,lim,j,0)]])/1000)==os_param_spd) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox1d) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox2d) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox3d) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox1u) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox2u) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox3u)) {
						ovs_no_clump_idx=j;
						break;
				}
			}
			/* Byproduct of vector labeling scheme */
			else {
				if ((((speed[map[j][tdm_map_find_y(map,lim,j,0)]])/10000)==(os_param_spd/10)) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox1d) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox2d) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox3d) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox1u) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox2u) &&
					(which_tsc(map[j][tdm_map_find_y(map,lim,j,0)],tsc)!=prox3u)) {
						ovs_no_clump_idx=j;
						break;
				}
			}
		}
	}
	
	return ovs_no_clump_idx;
}


/**
@name: tdm_vector_dump
@param: int[][]

Dumps current content of TDM vector map
**/
void tdm_vector_dump(FILE *file, unsigned char **map, int idx)
{
#ifdef _SET_TDM_DEV
	int j, v;
	
	TDM_VERBOSE0("\n");
	TDM_VERBOSE0("           ");
	for (v=0; v<VEC_MAP_WID; v++) {
		TDM_VERBOSE1("%d",v);
		if ((v+1)<VEC_MAP_WID) {
			if ((v+1)/10==0) {
				TDM_VERBOSE0("___");
			}
			else {
				TDM_VERBOSE0("__");
			}
		}
	}
	TDM_VERBOSE0("\n");
	for (v=0; v<idx; v++) {
		TDM_VERBOSE1("\n %d___\t", v);
		for (j=0; j<VEC_MAP_WID; j++) {
			if (map[j][v]!=_TD2P_NUM_EXT_PORTS) {
				TDM_VERBOSE1(" %03d", map[j][v]);
			}
			else {
				TDM_VERBOSE0(" ---");
			}
		}
	}
	TDM_VERBOSE0("\n");
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
			TDM_VERBOSE3("Warpcore #%0d, subport #%0d, contains physical port #%0d\n", i, j, wc_array[i][j]);
      	}
	}
	
}


/**
@name: TD2P_print_tdm_tbl
@param: int[32], const char*

Prints debug form summary of quadrant's table
**/
void TD2P_print_tdm_tbl(FILE *file, int pgw_tdm_tbl[32], const char* name)
{
	int j;
	
	for (j = 0; j < 32; j++)
	{
		switch (pgw_tdm_tbl[j])
		{
			case 130:
				TDM_PRINT2("TDM: PIPE: %s, TDM Calendar, element #%0d, contains an invalid or disabled port\n", name, j);
				break;
			case 131:
				TDM_PRINT2("TDM: PIPE: %s, TDM Calendar, element #%0d, contains an oversubscription token\n", name, j);
				break;
			default:
				TDM_PRINT3("TDM: PIPE: %s, TDM Calendar, element #%0d, contains physical port #%0d\n", name, j, pgw_tdm_tbl[j]);
				break;
		}
	}
}


/**
@name: TD2P_print_tbl_summary
@param: int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int[32], int

Prints short form final summary of all tables generated
**/
void TD2P_print_tbl_summary(FILE *file, int x0[32], int x1[32], int y0[32], int y1[32], int ox0[32], int ox1[32], int oy0[32], int oy1[32], int sx0[32], int sx1[32], int sy0[32], int sy1[32], int bw)
{
	int t;
	
	TDM_VERBOSE1("TDM: _____VERBOSE: TDM: core bandwidth is %0d\n", bw);
	TDM_VERBOSE0("TDM: X0, TDM Calendar\n");
	for (t = 0; t < 32; t++)
	{
		switch (x0[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			case 131:
				TDM_VERBOSE0(" O");
				break;
			default:
				TDM_VERBOSE1(" %0d", x0[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: X0, OVS Calendar\n");
	for (t = 0; t < 32; t++)
	{
		switch (ox0[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			default:
				TDM_VERBOSE1(" %0d", ox0[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: X0, OVS Spacing Info\n");
	for (t = 0; t < 32; t++)
	{
		switch (sx0[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			default:
				TDM_VERBOSE1(" %0d", sx0[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: X1, TDM Calendar\n");
	for (t = 0; t < 32; t++)
	{
		switch (x1[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			case 131:
				TDM_VERBOSE0(" O");
				break;
			default:
				TDM_VERBOSE1(" %0d", x1[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: X1, OVS Calendar\n");
	for (t = 0; t < 32; t++)
	{
		switch (ox1[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			default:
				TDM_VERBOSE1(" %0d", ox1[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: X1, OVS Spacing Info\n");
	for (t = 0; t < 32; t++)
	{
		switch (sx1[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			default:
				TDM_VERBOSE1(" %0d", sx1[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: Y0, TDM Calendar\n");
	for (t = 0; t < 32; t++)
	{
		switch (y0[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			case 131:
				TDM_VERBOSE0(" O");
				break;
			default:
				TDM_VERBOSE1(" %0d", y0[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: Y0, OVS Calendar\n");
	for (t = 0; t < 32; t++)
	{
		switch (oy0[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			default:
				TDM_VERBOSE1(" %0d", oy0[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: Y0, OVS Spacing Info\n");
	for (t = 0; t < 32; t++)
	{
		switch (sy0[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			default:
				TDM_VERBOSE1(" %0d", sy0[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: Y1, TDM Calendar\n");
	for (t = 0; t < 32; t++)
	{
		switch (y1[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			case 131:
				TDM_VERBOSE0(" O");
				break;
			default:
				TDM_VERBOSE1(" %0d", y1[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: Y1, OVS Calendar\n");
	for (t = 0; t < 32; t++)
	{
		switch (oy1[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			default:
				TDM_VERBOSE1(" %0d", oy1[t]);
				break;
		}
	}
	TDM_VERBOSE0("\nTDM: Y1, OVS Spacing Info\n");
	for (t = 0; t < 32; t++)
	{
		switch (sy1[t])
		{
			case 130:
				TDM_VERBOSE0(" x");
				break;
			default:
				TDM_VERBOSE1(" %0d", sy1[t]);
				break;
		}
	}
	TDM_VERBOSE0("\n");
}


/**
@name: tdm_filter_vector_shift
@param:

Translates an entire vector within flat unlinked table for purpose of smoothing oversub
Can only be used when all destination nodes are sparse
In this filter, only need to check VBS min spacing, as LLS is guaranteed by the vector itself
**/
int tdm_filter_vector_shift(int tdm_tbl[256], int port, int dir)
{
	int i, done=PASS, abort=BOOL_FALSE;
	
	TOKEN_CHECK(port) {
		if (dir==DN) {
			for (i=0; i<256; i++) {
				if (tdm_tbl[i]==port && tdm_tbl[i+1]!=OVSB_TOKEN) {
					abort=BOOL_TRUE;
					break;
				}
			}
			if (!abort) {
				for (i=0; i<255; i++) {
					if (tdm_tbl[i]==port) {
						tdm_tbl[i+1]=port;
						tdm_tbl[i]=OVSB_TOKEN;
						i++;
					}
				}
			}
			else {
				done=FAIL;
			}
		}
		else if (dir==UP) {
			for (i=1; i<256; i++) {
				if (tdm_tbl[i]==port && tdm_tbl[i-1]!=OVSB_TOKEN) {
					abort=BOOL_TRUE;
					break;
				}
			}
			if (!abort) {
				for (i=1; i<256; i++) {
					if (tdm_tbl[i]==port) {
						tdm_tbl[i-1]=port;
						tdm_tbl[i]=OVSB_TOKEN;
					}
				}
			}
			else {
				done=FAIL;
			}
		}
	}
	else {
		done=FAIL;
	}
	
	return done;

}


/**
@name: TDM_scheduler_filter_refactor
@param:

USE ONLY IF ALL SINGLE MODE PORTS
**/
void TDM_scheduler_filter_refactor(int tdm_tbl[256], int lr_idx_limit, int lr_vec_cnt)
{
	int i, j, os_re_pool=0, os_re_spacing, os_re_cnt=0;
	
	TDM_VERBOSE0("TDM: _____VERBOSE: Filter applied: Refactor table list\n");
	for (i=0; i<lr_idx_limit; i++) {
		if (tdm_tbl[i]==OVSB_TOKEN) {
			++os_re_pool;
			/* Destructively shift slice */
			for (j=i; j<lr_idx_limit; j++) {
				tdm_tbl[j]=tdm_tbl[j+1];
			}
			i--;
		}
	}
	os_re_spacing=(os_re_pool>0)?(lr_idx_limit/os_re_pool):(1);
	
	for (i=lr_vec_cnt; ( (i<lr_idx_limit) && ((++os_re_cnt)<=os_re_pool) ); i+=os_re_spacing) {
		for (j=lr_idx_limit; j>i; j--) {
			tdm_tbl[j]=tdm_tbl[j-1];
		}
		tdm_tbl[i]=OVSB_TOKEN;
	}
	
}


/**
@name: tdm_filter_dither
@param:

Vector sorting during scheduling results in quantization error that inflates max slice
distribution of oversub/linerate. This repartitions from an oversub slice to a linerate 
slice to reduce max slice size overall.
**/
void tdm_filter_dither(int tdm_tbl[256], int lr_idx_limit, int accessories, int tsc[NUM_TSC][4], int threshold, enum port_speed speed[_TD2P_NUM_EXT_PORTS])
{
	int g, i, j, k=1, l=_TD2P_NUM_EXT_PORTS;
	unsigned char dither_shift_done=BOOL_FALSE, dither_done=BOOL_FALSE, dither_slice_counter=0;

	dither_shift_done=BOOL_FALSE;
	for (i=0; i<(lr_idx_limit+accessories); i++) {
		if (tdm_tbl[i]==OVSB_TOKEN && tdm_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories))==tdm_slice_size(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))) {
			g=i; while( (tdm_tbl[g]==OVSB_TOKEN) && (g<(lr_idx_limit+accessories)) ) {g++;}
			if ( tdm_slice_prox_up(g,tdm_tbl,tsc,speed) &&
				 tdm_slice_prox_dn((tdm_slice_idx(OVSB_TOKEN,tdm_tbl,(lr_idx_limit+accessories))-1),tdm_tbl,(lr_idx_limit+accessories),tsc,speed) ) {
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
		for (i=1; i<(lr_idx_limit+accessories-1); i++) {
			dither_slice_counter=0;
			while (tdm_tbl[i]!=OVSB_TOKEN && tdm_slice_size_local(i,tdm_tbl,(lr_idx_limit+accessories))==tdm_slice_size(1,tdm_tbl,(lr_idx_limit+accessories)) && i<(lr_idx_limit+accessories) ) {
				if (++dither_slice_counter>=threshold && tdm_tbl[i-1]==OVSB_TOKEN) {
					for (j=255; j>i; j--) {
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
			TDM_VERBOSE0("TDM: _____VERBOSE: Filter applied: Dither (quantization correction)\n");
		}
	}
}


/**
@name: tdm_filter_fine_dither_normal
@param:

SINGLE PASS PER CALL
**/
int tdm_filter_fine_dither(int port, int tdm_tbl[256], int lr_idx_limit, int accessories, int tsc[NUM_TSC][4])
{
	int i, j, k;
	unsigned char fine_dither_done;
#ifdef BCM_TRIDENT2PLUS_SUPPORT
	int min_prox=LLS_MIN_SPACING;
#else
	int min_prox=VBS_MIN_SPACING;
#endif

	i=port; fine_dither_done=BOOL_FALSE;
	for (j=2; j<(lr_idx_limit+accessories-4); j++) {
		if ( tdm_tbl[j]!=OVSB_TOKEN && tdm_tbl[j-2]!=OVSB_TOKEN && tdm_tbl[j+2]!=OVSB_TOKEN && tdm_tbl[j+4]!=OVSB_TOKEN && 
			 tdm_slice_size_local((j-2), tdm_tbl, (lr_idx_limit+accessories))==1 && 
			 tdm_slice_size_local(j, tdm_tbl, (lr_idx_limit+accessories))==1 && 
			 tdm_slice_size_local((j+2), tdm_tbl, (lr_idx_limit+accessories))==1 && 
			 tdm_slice_size_local((j+4), tdm_tbl, (lr_idx_limit+accessories))==1 &&
			 tdm_slice_prox_local(j, tdm_tbl, (lr_idx_limit+accessories), tsc)>min_prox )
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
		TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: Fine dithering (normal), index %0d\n",port);
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
@name: tdm_filter_ancillary_smooth
@param:

Ancillary slots are exempt from typical min/max spacing rules. If an
ancillary slice is increasing jitter in the TDM, this filter will smooth the 
contribution from ancillary slots to reduce likelihood of cumulative sop to mop cycle
delay.

SINGLE PASS PER CALL
**/
void tdm_filter_ancillary_smooth(int port, int tdm_tbl[256], int lr_idx_limit, int accessories)
{
	int i, j, k;
	unsigned char ancillary_slice_counter;
	
	i=port; ancillary_slice_counter=0;
	for (j=1; (i-j)>0; j++) {
		TOKEN_CHECK(tdm_tbl[i-j]) {
			if (tdm_slice_size_local((i-j),tdm_tbl,(lr_idx_limit+accessories))==1) {
				/* Prevent cumulative sop to mop cycle delay */
				if (++ancillary_slice_counter==2) {
					TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: Smooth ancillary slots, index %0d\n",port);
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
@name: tdm_filter_redist
@param:

Vector sorting during scheduling of fast speeds results in quantization error. If the
speeds are homogenous and single mode it is easier and more effective to simply redistribute
oversub slots by running average than to apply any other filters.

USE ONLY IF ALL SINGLE MODE PORTS
**/
void tdm_filter_refactor(int tdm_tbl[256], int lr_idx_limit, int accessories, int lr_vec_cnt)
{
	int g, i, j, l;
	unsigned char os_re_pool=0, os_re_tdm_len=0, os_re_spacing=0, os_re_spacing_last=0;

	TDM_VERBOSE0("TDM: _____VERBOSE: Filter applied: Refactor table list\n");
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
		if (tdm_tbl[i]!=_TD2P_NUM_EXT_PORTS) {
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
@name: tdm_filter_local_slice
@param:

SINGLE PASS PER CALL
**/
void tdm_filter_local_slice_dn(int port, int tdm_tbl[256], int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
{
	int i=port, j, pp_prox=PASS;

	if (tdm_tbl[i]<125 && tdm_tbl[i]>0) {
		for (j=1; j<VBS_MIN_SPACING; j++) {
			if (which_tsc(tdm_tbl[i+1+j],tsc)==which_tsc(tdm_tbl[i],tsc)) {
				pp_prox=FAIL;
				break;
			}
		}
	}
#ifdef BCM_TRIDENT2PLUS_SUPPORT
	if (speed[tdm_tbl[i]]<=SPEED_42G_HG2) {
		if (i<(VEC_MAP_LEN-LLS_MIN_SPACING)) {
			for (j=1; j<LLS_MIN_SPACING; j++) {
				if (tdm_tbl[i+j]==tdm_tbl[i]) {
					pp_prox=FAIL;
					break;
				}
			}
		}
	}
#endif
	if (pp_prox) {
		TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: (1) Local Slice, Down, index %0d\n",port);
		tdm_tbl[i+1]=tdm_tbl[i];
		tdm_tbl[i]=OVSB_TOKEN;
	}

}


/**
@name: tdm_filter_local_slice
@param:

SINGLE PASS PER CALL
**/
void tdm_filter_local_slice_up(int port, int tdm_tbl[256], int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
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
#ifdef BCM_TRIDENT2PLUS_SUPPORT
	if (speed[tdm_tbl[i]]<=SPEED_42G_HG2) {
		if (i>=LLS_MIN_SPACING) {
			for (j=1; j<LLS_MIN_SPACING; j++) {
				if (tdm_tbl[i-j]==tdm_tbl[i]) {
					pp_prox=FAIL;
					break;
				}
			}
		}
	}
#endif
	if (pp_prox) {
		TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: (1) Local Slice, Up, index %0d\n",port);
		tdm_tbl[i-1]=tdm_tbl[i];
		tdm_tbl[i]=OVSB_TOKEN;
	}

}


/**
@name: tdm_filter_local_slice_oversub
@param:

**/
void tdm_filter_local_slice_oversub(int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
{
	int g, i;
	
	for (i=0; i<lr_idx_limit; i++) {
		if ( (tdm_slice_size(OVSB_TOKEN,tdm_tbl,lr_idx_limit)>(tdm_slice_size(1,tdm_tbl,lr_idx_limit)+1)) ) {
			g=tdm_slice_idx(OVSB_TOKEN,tdm_tbl,lr_idx_limit);
			if (tdm_slice_prox_dn((g-1),tdm_tbl,lr_idx_limit,tsc,speed)) {
				TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: Local slice oversub index %0d\n",g);
				tdm_tbl[g]=tdm_tbl[g-1];
				tdm_tbl[g-1]=OVSB_TOKEN;
			}
		}
	}
	for (i=0; i<lr_idx_limit; i++) {
		if ( (tdm_slice_size(OVSB_TOKEN,tdm_tbl,lr_idx_limit)>(tdm_slice_size(1,tdm_tbl,lr_idx_limit)+1)) ) {
			g=tdm_slice_idx(OVSB_TOKEN,tdm_tbl,lr_idx_limit);
			do {g++;} while(tdm_tbl[g]>128 || tdm_tbl[g]==0);
			if (tdm_slice_prox_up(g,tdm_tbl,tsc,speed)) {
				TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: Local slice oversub index %0d\n",g);
				tdm_tbl[g-1]=tdm_tbl[g];
				tdm_tbl[g]=OVSB_TOKEN;
			}
		}
	}

}


/**
@name: tdm_filter_slice_dn
@param:

**/
int tdm_filter_slice_dn(int port, int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
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
			TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: Slice translation, Down, index %0d\n",port);
			i++;
			break;
		}
	}
	
	return i;

}


/**
@name: tdm_filter_slice_up
@param:

**/
int tdm_filter_slice_up(int port, int tdm_tbl[256], int lr_idx_limit, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
{
	int i, j, k, slice_translation_done;
	
	i=port; slice_translation_done=BOOL_FALSE;
	for (j=3; j<(lr_idx_limit-1); j++) {
		if ((i-j-3)>=0) {
			if (tdm_tbl[i-j]==OVSB_TOKEN && 
				tdm_tbl[i-1-j]==OVSB_TOKEN &&
				(tdm_tbl[i-j+1]==OVSB_TOKEN || (which_tsc(tdm_tbl[i-j+1],tsc)!=which_tsc(tdm_tbl[i-j-3],tsc))) &&
				(tdm_tbl[i-j+2]==OVSB_TOKEN || (which_tsc(tdm_tbl[i-j+2],tsc)!=which_tsc(tdm_tbl[i-j-2],tsc))) &&
				(check_lls_flat_up(i,tdm_tbl,speed))) {
				for (k=(i-j); k<i; k++) {
					tdm_tbl[k]=tdm_tbl[k+1];
					slice_translation_done=BOOL_TRUE;
				}
				tdm_tbl[i]=OVSB_TOKEN;
			}
			if (slice_translation_done) {
				TDM_VERBOSE1("TDM: _____VERBOSE: Filter applied: Slice translation, Up, index %0d\n",port);
				i--;
				break;
			}
		}
	}
	
	return i;

}


/**
@name: which_tsc
@param: int

Returns the TSC to which the input port belongs given pointer to transcribed TSC matrix
**/
int which_tsc(unsigned char port, int tsc[NUM_TSC][4]) {

	int i, j, which=_TD2P_NUM_EXT_PORTS;
		
	for (i=0; i<NUM_TSC; i++) {
		for (j=0; j<4; j++) {
			if (tsc[i][j]==port) {
				which=i;
			}
		}
		if (which!=_TD2P_NUM_EXT_PORTS) {
			break;
		}
	}
	
	return which;
	
}

#if 0
/* Temporarily disable this definition. same function name is defined in src/soc/esw/tomahawk/set_tdm.c */
/**
@name: check_mode
@param: unsigned char, int[][]

Returns how many lanes are active in the TSC of the input port
**/
int check_mode(unsigned char port, int tsc[NUM_TSC][4])
{
	int i,j=0;

	for (i=0; i<4; i++) {
		if (tsc[which_tsc(port,tsc)][i]!=_TD2P_NUM_EXT_PORTS) {
			j++;
		}
	}
	
	return j;

}
#endif

/**
@name: check_same_port_dist_dn
@param:

Returns distance to next index with same port number, in down direction
Capable of wraparound without mirroring
Down direction
**/
int check_same_port_dist_dn(int idx, int tdm_tbl[256], int lim)
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
@name: check_same_port_dist_up
@param:

Returns distance to next index with same port number, in down direction
Capable of wraparound without mirroring
Up direction
**/
int check_same_port_dist_up(int idx, int tdm_tbl[256], int lim)
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
@name: check_lls_flat
@param:

Checks LLS scheduler min spacing in tdm array
**/
int check_lls_flat(int idx, int tdm_tbl[256])
{
	int lls_prox=PASS;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
	{
		int i=idx, j;
		
		if (i>=LLS_MIN_SPACING) {
			for (j=1; j<LLS_MIN_SPACING; j++) {
				if (tdm_tbl[i-j]==tdm_tbl[i]) {
					lls_prox=FAIL;
					break;
				}
			}
		}
		if (i<(VEC_MAP_LEN-LLS_MIN_SPACING-1)) {
			for (j=1; j<LLS_MIN_SPACING; j++) {
				if (tdm_tbl[i+j]==tdm_tbl[i]) {
					lls_prox=FAIL;
					break;
				}
			}
		}
	}
#endif

	return lls_prox;

}


/**
@name: check_lls_flat_dn
@param:

Checks LLS scheduler min spacing in tdm array, down direction only, returns dist
**/
int check_lls_flat_dn(int idx, int tdm_tbl[256], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
{
	int lls_prox=VEC_MAP_LEN;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
	{
		int i=idx, j;
		
		lls_prox=1;
		if (i<(VEC_MAP_LEN-LLS_MIN_SPACING-1) && tdm_tbl[idx]<=SPEED_42G_HG2) {
			for (j=1; j<LLS_MIN_SPACING; j++) {
				if (tdm_tbl[i+j]==tdm_tbl[i]) {
					break;
				}
				lls_prox++;
			}
		}
	}
#endif

	return lls_prox;

}


/**
@name: check_lls_flat_up
@param:

Checks LLS scheduler min spacing in tdm array, up direction only, returns dist
**/
int check_lls_flat_up(int idx, int tdm_tbl[256], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
{
	int lls_prox=VEC_MAP_LEN;

#ifdef BCM_TRIDENT2PLUS_SUPPORT
	{
		int i=idx, j;
		
		lls_prox=1;
		if (i>=LLS_MIN_SPACING && tdm_tbl[idx]<=SPEED_42G_HG2) {
			for (j=1; j<LLS_MIN_SPACING; j++) {
				if (tdm_tbl[i-j]==tdm_tbl[i]) {
					break;
				}
				lls_prox++;
			}
		}
	}
#endif

	return lls_prox;

}


/**
@name: check_lls_map
@param:

Checks LLS scheduler min spacing in tdm vector map
**/
int check_lls_map(unsigned char **map)
{
	return PASS;
}


/**
@name: check_lls_ll
@param:

Checks LLS scheduler min spacing in tdm linked list
**/
/* int check_lls_ll(struct node *list) */
int check_lls_ll(void)
{
	return PASS;
}


/**
@name: check_type_2
@param: unsigned char, int[][]

Complex returns port type (unique lanes) in the TSC of the input port
**/
int check_type_2(unsigned char port, int tsc[NUM_TSC][4])
{
	int i, j, tmp=_TD2P_NUM_EXT_PORTS, cnt=1, tsc_arr[4], tsc_inst=which_tsc(port,tsc);
	
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
		if (tsc_arr[i]!=_TD2P_NUM_EXT_PORTS && tsc_arr[i]!=tsc_arr[i-1]) {
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
int check_ethernet(int port, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1]) {

	int i, type=BOOL_TRUE;
	
	if (port<33) {
		for (i=1; i<33; i++) {
			if (speed[i]!=SPEED_0 && traffic[which_tsc(i,tsc)]==PORT_HIGIG2) {
				type=BOOL_FALSE;
				break;
			}
		}
	}
	else if (port<65) {
		for (i=33; i<65; i++) {
			if (speed[i]!=SPEED_0 && traffic[which_tsc(i,tsc)]==PORT_HIGIG2) {
				type=BOOL_FALSE;
				break;
			}
		}
	}
	else if (port<97) {
		for (i=65; i<97; i++) {
			if (speed[i]!=SPEED_0 && traffic[which_tsc(i,tsc)]==PORT_HIGIG2) {
				type=BOOL_FALSE;
				break;
			}
		}	
	}
	else {
		for (i=97; i<129; i++) {
			if (speed[i]!=SPEED_0 && traffic[which_tsc(i,tsc)]==PORT_HIGIG2) {
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
Use on transmuted vectors and special vectors (i.e. higig2) with odd number of nodes
**/
int check_spd(unsigned char **map, int lim, int x_pos, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int round)
{
	int k=0, v, param_spd=_TD2P_NUM_EXT_PORTS, y_pos=0;
	
	for (v=0; v<lim; v++) {
		if (map[x_pos][v]!=_TD2P_NUM_EXT_PORTS) {
			y_pos = (y_pos==0) ? (v):(y_pos);
			k++;
		}
	}
	switch(k) {
		case 1: param_spd = 1; break;
		case 4: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 10;} else {param_spd = 11;} break;
		case 8: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 20;} else {param_spd = 21;} break;
		case 10: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 25;} else {param_spd = 27;} break;
		case 16: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 40;} else {param_spd = 42;} break;
		case 20: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 50;} else {param_spd = 53;} break;
		case 39: param_spd = 107; break;
		case 40: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 100;} else {param_spd = 106;} break;
		case 48: if (check_ethernet(map[x_pos][y_pos], speed, tsc, traffic)) {param_spd = 120;} else {param_spd = 124;} break;
	}

	while (round && param_spd%5!=0) {
		param_spd--;
	}
	
	return param_spd;

}


/**
@name: check_fastest_triport
@param:

Returns whether the current port is the fastest port from within a triport
**/
int check_fastest_triport(int port, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
{
	int i, tsc_id=which_tsc(port,tsc);
	
	for (i=0; i<4; i++) {
		if (tsc[tsc_id][i]!=port) {
			if (speed[port]<speed[tsc[tsc_id][i]]) {
				return BOOL_FALSE;
			}
		}
	}
	
	return BOOL_TRUE;
	
}


/**
@name: tdm_slice_size_local
@param:

Given index, returns SIZE of LARGEST slice contiguous to given index
**/
int tdm_slice_size_local(unsigned char idx, int tdm[256], int lim)
{
	int i, slice_size=(-1);
	
	if (tdm[idx]!=OVSB_TOKEN && tdm[idx]!=_TD2P_NUM_EXT_PORTS) {
		for (i=idx; i>=0; i--) {
			if (tdm[i]!=OVSB_TOKEN && tdm[i]!=_TD2P_NUM_EXT_PORTS) {
				slice_size++;
			}
			else {
				break;
			}
		}
		for (i=idx; i<lim; i++) {
			if (tdm[i]!=OVSB_TOKEN && tdm[i]!=_TD2P_NUM_EXT_PORTS) {
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
@name: check_fit_smooth
@param:

Inside of flat unlinked table, returns NUMBER of nodes inside a port vector that clump with other nodes of the same type
**/
int check_fit_smooth(int tdm_tbl[256], int port, int lr_idx_limit, int clump_thresh)
{
	int i, cnt=0;

	for (i=0; i<lr_idx_limit; i++) {
		if ( (tdm_tbl[i]==port) && (tdm_slice_size_local(i,tdm_tbl,lr_idx_limit)>=clump_thresh) ) {
			cnt++;
		}
	}

	return cnt;

}


/**
@name: check_fastest_lane
@param:

Returns the speed of the fastest lane from within a tsc
**/
unsigned int check_fastest_lane(int port, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
{
	int i, tsc_id=which_tsc(port,tsc);
	unsigned int spd=SPEED_0;
	
	for (i=0; i<4; i++) {
		if (tsc[tsc_id][i]!=_TD2P_NUM_EXT_PORTS) {	
			if (speed[tsc[tsc_id][i]]>spd) {
				spd=speed[tsc[tsc_id][i]];
			}
		}
	}
	
	return spd;
	
}


/**
@name: find_fastest_lane
@param:

Returns the fastest port number from within a tsc
**/
int find_fastest_lane(int sample, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
{
	int i, port=_TD2P_NUM_EXT_PORTS, tsc_id=which_tsc(sample,tsc);
	unsigned int spd=SPEED_0;
	
	for (i=0; i<4; i++) {
		if (tsc[tsc_id][i]!=_TD2P_NUM_EXT_PORTS) {	
			if (speed[tsc[tsc_id][i]]>spd) {
				port=tsc[tsc_id][i];
			}
		}
	}
	
	return port;
	
}


/**
@name: tdm_find_nonsingular
@param:

Returns index of first unreduced row
**/
int tdm_find_nonsingular(unsigned char **map, unsigned char x_idx, int lim)
{
	int i, j, ns_idx=_TD2P_NUM_EXT_PORTS;
	
	for (i=0; i<lim; i++) {
		TOKEN_CHECK(map[x_idx][i]) {
			for (j=0; j<VEC_MAP_WID; j++) {
				if ( (j!=x_idx) && (map[j][i]!=_TD2P_NUM_EXT_PORTS) ) {
					ns_idx=j;
					break;
				}
			}
		}
	}
	
	return ns_idx;
	
}


/**
@name: tdm_check_nonsingular
@param:

Checks if current row is singular
**/
int tdm_check_nonsingular(unsigned char **map, unsigned char y_idx)
{
	int i, found_port=_TD2P_NUM_EXT_PORTS, check=PASS;
	
	for (i=0; i<VEC_MAP_WID; i++) {
		TOKEN_CHECK(map[i][y_idx]) {
			if (found_port==_TD2P_NUM_EXT_PORTS) {
				found_port=map[i][y_idx];
			}
			else {
				check=FAIL;
				break;
			}
		}
	}
	
	return check;
	
}


/**
@name: tdm_slice_size_2d
@param:

Returns size of proximate 2-D slice in vector map
Returns 0 if the passed row was blank
**/
int tdm_slice_size_2d(unsigned char **map, unsigned char y_idx, int lim)
{
	int i, slice_size=1;
	
	if (tdm_check_blank(map,y_idx)) {
		return 0;
	}
	else {
		for (i=1; (y_idx-i)>=0; i++) {
			if (tdm_check_blank(map,i)) {
				break;
			}
			slice_size++;
		}
		for (i=1; (y_idx+i)<lim; i++) {
			if (tdm_check_blank(map,i)) {
				break;
			}
			slice_size++;
		}
	}
	
	return slice_size;
	
}


/**
@name: tdm_check_blank
@param:

Checks if current row is blank
**/
int tdm_check_blank(unsigned char **map, unsigned char y_idx)
{
	int i, check=PASS;
	
	for (i=0; i<VEC_MAP_WID; i++) {
		TOKEN_CHECK(map[i][y_idx]) {
			check=FAIL;
			break;
		}
	}
	
	return check;
	
}


/**
@name: tdm_count_nonsingular
@param:

Counts number of unreduced rows at current rotation
**/
int tdm_count_nonsingular(unsigned char **map, unsigned char x_idx, int lim)
{
	int i, j, ns_cnt=0;
	
	for (i=0; i<lim; i++) {
		TOKEN_CHECK(map[x_idx][i]) {
			for (j=0; j<VEC_MAP_WID; j++) {
				if ( (j!=x_idx) && (map[j][i]!=_TD2P_NUM_EXT_PORTS) ) {
					ns_cnt++;
				}
			}
		}
	}
	
	return ns_cnt;
	
}


/**
@name: tdm_pick_vec
@param:

Select vector index on x axis to rotate based on priority of TSC pipeline
**/
int tdm_pick_vec(unsigned char **vector_map, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS], int lr_vec_cnt, int tsc_dq, int *triport_priority, int prev_vec)
{
	int i, vec_sel=1;
	
	for (i=prev_vec; i<lr_vec_cnt; i++) {
		if ( (which_tsc(vector_map[i][0],tsc)==tsc_dq) ) {
			if ((*triport_priority)) {
				if (check_fastest_triport(vector_map[i][0],tsc,speed)) {
					vec_sel=i;
					(*triport_priority)=BOOL_FALSE;
					break;
				}
				else {
					continue;
				}
			}
			else {
				vec_sel=i;
				break;
			}
		}
	}
	
	return vec_sel;
	
}


/**
@name: tdm_slice_prox_local
@param:

Given index, checks min spacing of two nearest non-token ports
**/
int tdm_slice_prox_local(unsigned char idx, int tdm[256], int lim, int tsc[NUM_TSC][4])
{
	int i, prox_len=0, wc=_TD2P_NUM_EXT_PORTS;
	
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
@name: tdm_slice_size
@param:

Given port number, returns SIZE of LARGEST slice
**/
int tdm_slice_size(unsigned char port, int tdm[256], int lim)
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
@name: tdm_slice_idx
@param:

Given port number, returns INDEX of LARGEST slice
**/
int tdm_slice_idx(unsigned char port, int tdm[256], int lim)
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
@name: tdm_slice_prox_dn
@param:

Given port number, checks min spacing in a slice in down direction
**/
int tdm_slice_prox_dn(int slot, int tdm[256], int lim, int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
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
#ifdef BCM_TRIDENT2PLUS_SUPPORT
	{
		int i=slot, j;
		
		if (speed[tdm[i]]<=SPEED_42G_HG2) {
			if (i<(VEC_MAP_LEN-LLS_MIN_SPACING)) {
				for (j=1; j<LLS_MIN_SPACING; j++) {
					if (tdm[i+j]==tdm[i]) {
						slice_prox=FAIL;
						break;
					}
				}
			}
		}
	}
#endif

	return slice_prox;

}


/**
@name: tdm_slice_prox_up
@param:

Given port number, checks min spacing in a slice in up direction
**/
int tdm_slice_prox_up(int slot, int tdm[256], int tsc[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS])
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
#ifdef BCM_TRIDENT2PLUS_SUPPORT
	{
		int i=slot, j;
		
		if (speed[tdm[i]]<=SPEED_42G_HG2) {
			if (i>=LLS_MIN_SPACING) {
				for (j=1; j<LLS_MIN_SPACING; j++) {
					if (tdm[i-j]==tdm[i]) {
						slice_prox=FAIL;
						break;
					}
				}
			}
		}
	}
#endif

	return slice_prox;

}


/**
@name: tdm_table_scan
@param: int, int

Scans table for next valid slot
**/
int tdm_table_scan(FILE *file, unsigned short yy, unsigned short spacing, int limit, int speed, unsigned char stack[64], unsigned short stack_pointer, int tsc[NUM_TSC][4], int tdm_tbl[256]) 
{
	int j, prox_chk, vec_cnt;
	unsigned short timeout;

    if ( yy >= 253 ) {
		TDM_ERROR1("Parameter value is too big %d \n", yy);
    }
	
	timeout=TIMEOUT;
	do {
		prox_chk=PASS;
		TDM_SML_BAR
		TDM_VERBOSE1("TDM: _____VERBOSE: Scanning table pointer %0d\n",yy);
		TDM_VERBOSE8("[%0d] | (%0d)(%0d)(%0d)[[%0d]](%0d)(%0d)(%0d)\n",which_tsc(stack[stack_pointer],tsc), which_tsc(tdm_tbl[yy-3],tsc),which_tsc(tdm_tbl[yy-2],tsc),which_tsc(tdm_tbl[yy-1],tsc),which_tsc(tdm_tbl[yy],tsc),which_tsc(tdm_tbl[yy+1],tsc),which_tsc(tdm_tbl[yy+2],tsc),which_tsc(tdm_tbl[yy+3],tsc));		
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
		for (j=yy; j<yy+(tdm_slots(speed)*spacing); j+=spacing) {
			if (tdm_tbl[j]==_TD2P_NUM_EXT_PORTS) {
				vec_cnt++;
			}
		}
		if (vec_cnt!=tdm_slots(speed) || prox_chk==FAIL) {
			yy++;
		}
		if ((yy+(spacing*(tdm_slots(speed)-1))) >= limit) {
			TDM_SML_BAR
			TDM_VERBOSE2("could not schedule %0dG vector for port %0d, breaking table pointer scan\n", speed, stack[stack_pointer]);
			break;
		}
	} while (yy<253 && (vec_cnt!=tdm_slots(speed) || prox_chk==FAIL) && ((--timeout)>0));
	
	return yy;
}


/**
@name: tdm_map_search_x
@param: unsigned char **, int, int

Scans vector map x axis for singular node
**/
int tdm_map_search_x(unsigned char **map, int idx) 
{
	int i, id;
	
	id = _TD2P_NUM_EXT_PORTS;
	for (i=0; i<VEC_MAP_WID; i++) {
		if (map[i][idx]!=_TD2P_NUM_EXT_PORTS) {
			id = map[i][idx];
			break;
		}
	}
	
	return id;
	
}


/**
@name: tdm_map_search_y
@param: unsigned char **, int, int

Scans vector map y axis for singular node
**/
int tdm_map_search_y(unsigned char **map, int limit, int idx) 
{
	int i, id;
	
	id = _TD2P_NUM_EXT_PORTS;
	if (idx<VEC_MAP_WID) {
		for (i=0; i<limit; i++) {
			if (map[idx][i]!=_TD2P_NUM_EXT_PORTS) {
				id = map[idx][i];
				break;
			}
		}
	}
	
	return id;
	
}


/**
@name: tdm_map_scan_x
@param: unsigned char **, int, int

Scans vector map x axis for X INDEX OF NEXT NON-FEATURE NODE
**/
int tdm_map_scan_x(unsigned char **map, int limit, int idx, int principle)
{
	int i, id;
	
	id = _TD2P_NUM_EXT_PORTS;
	if (principle!=_TD2P_NUM_EXT_PORTS && idx<limit) {
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
@name: tdm_map_find_x
@param: unsigned char **, int, int

Scans vector map x axis for X INDEX OF ANY NON-PRINCIPLE NODE
**/
int tdm_map_find_x(unsigned char **map, int limit, int idx, int principle)
{
	int i, id;
	
	id = _TD2P_NUM_EXT_PORTS;
	if (principle!=_TD2P_NUM_EXT_PORTS && idx<limit) {
		for (i=0; i<VEC_MAP_WID; i++) {
			if (map[i][idx]!=_TD2P_NUM_EXT_PORTS && map[i][idx]!=principle) {
				id = i;
				break;
			}
		}
	}
	
	return id;
	
}


/**
@name: tdm_map_find_y
@param: unsigned char **, int, int

Scans vector map Y axis for Y INDEX OF ANY NON-PRINCIPLE NODE
**/
int tdm_map_find_y(unsigned char **map, int limit, int idx, int principle)
{
	int i, id=_TD2P_NUM_EXT_PORTS;
	
	if (principle!=_TD2P_NUM_EXT_PORTS && idx<VEC_MAP_WID) {
		for (i=0; i<limit; i++) {
			if (map[idx][i]!=_TD2P_NUM_EXT_PORTS && map[idx][i]!=principle) {
				id = i;
				break;
			}
		}
	}
	
	return id;
	
}


/**
@name: tdm_fit_prox
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port (VBS) spacing rule
**/
int tdm_fit_prox(unsigned char **map, int lim, int wid, int node_x, int tsc[NUM_TSC][4]) {
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
@name: tdm_fit_prox_partial
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port spacing rule
**/
int tdm_fit_prox_partial(unsigned char **map, int lim, int wid, int node_x, int tsc[NUM_TSC][4]) {
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
@name: tdm_fit_prox_partial2
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port spacing rule
**/
int tdm_fit_prox_partial2(unsigned char **map, int lim, int wid, int node_x, int tsc[NUM_TSC][4]) {
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
@name: tdm_fit_prox_node
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port spacing rule
**/
int tdm_fit_prox_node(unsigned char **map, int node_y, int wid, int node_x, int tsc[NUM_TSC][4], int new_node_y) {
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
@name: tdm_fit_prox_node_partial
@param: unsigned char **, int, int

Given x index, determines fit for current vector based on sister port spacing rule
**/
int tdm_fit_prox_node_partial(unsigned char **map, int node_y, int wid, int node_x, int tsc[NUM_TSC][4]) {
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
@name: tdm_fit_singular_cnt
@param:

Given y index, count number of nodes
**/
int tdm_fit_singular_cnt(unsigned char **map, int node_y) {
	int v, cnt=0;
	
	for (v=0; v<VEC_MAP_WID; v++) {
		TOKEN_CHECK(map[v][node_y]) {
			++cnt;
		}
	}
	
	return cnt;

}


/**
@name: tdm_map_retrace_count
@param:

Returns number of retraceable slots within cadence at index
**/
int tdm_map_retrace_count(unsigned char **map, int x_idx, int y_idx)
{
	int i=y_idx, cnt=0;
	unsigned char port=map[x_idx][y_idx];
	
	while (map[x_idx][++i]!=port && i<VEC_MAP_LEN) {
		if ( tdm_fit_singular_cnt(map,i)==0 ) {
			cnt++;
		}
	}
	
	return cnt;
	
}


/**
@name: tdm_map_cadence_count
@param:

Returns size of port sequence at index
**/
int tdm_map_cadence_count(unsigned char *vector, int idx)
{
	int i=idx, cnt=0;
	unsigned char port=vector[idx];
	
	while (vector[++i]!=port && i<VEC_MAP_LEN) {
		cnt++;
	}
	
	return cnt;
	
}


/**
@name: tdm_fit_singular_col
@param:

Given x index, determines fit based on if current column is reducible
**/
int tdm_fit_singular_col(unsigned char **map, int node_x, int lim) {
	int v, result=PASS;
	
	for (v=0; v<lim; v++) {
		TOKEN_CHECK(map[node_x][v]) {
			if ( tdm_fit_singular_cnt(map,v)>1 ) {
				result=FAIL;
				break;
			}
		}
	}
	
	return result;

}


/**
@name: tdm_fit_singular_row
@param: unsigned char **, int

Given y index, determines fit based on if current row is reducible
**/
int tdm_fit_singular(unsigned char **map, int node_y) {
	int v, node_id=_TD2P_NUM_EXT_PORTS, node_idx=_TD2P_NUM_EXT_PORTS, node_idx_last=_TD2P_NUM_EXT_PORTS;/*, check=PASS;*/
	
	/* Check that the node is a singular node or can be reduced to a singular node */
	for (v=0; v<VEC_MAP_WID; v++) {
		if (map[v][node_y]!=_TD2P_NUM_EXT_PORTS && node_id==_TD2P_NUM_EXT_PORTS) {
			node_id = map[v][node_y];
			node_idx_last = v;
		}
		if (node_id!=_TD2P_NUM_EXT_PORTS && map[v][node_y]!=_TD2P_NUM_EXT_PORTS) {
			node_idx = (v > node_idx_last)?(v):(_TD2P_NUM_EXT_PORTS);
			if (node_idx!=_TD2P_NUM_EXT_PORTS) {break;}
		}
	}
	if (node_id==_TD2P_NUM_EXT_PORTS) {
		node_idx=BLANK;
	}
	
	return node_idx;
}


/**
@name: tdm_weight_update
@param:

Recalculates nodal weight array
**/
void tdm_weight_update(unsigned char **map, int lim, int weights[VEC_MAP_WID], int weights_last[VEC_MAP_WID], int weights_cmp[VEC_MAP_WID], int span, int mpass, int lr_slot_cnt)
{
	int i;
	
	for (i=0; i<VEC_MAP_WID; i++) {
		weights_last[i] = weights[i];
	}
	for (i=1; i<=span; i++) {
		weights[i]=tdm_map_shortest_distance(map,lim,span,(span-i));
	}
	#ifdef _SET_TDM_DEV
	TDM_PRINT0("\n");
	TDM_PRINT0("Analyzing TDM matrix...\n");
	TDM_PRINT3("\tvector %0d | subpass %0d | threshold %0d\n",span,mpass,((lim/lr_slot_cnt)+VECTOR_ISOLATION));
	#endif
	for (i=1; i<=span; i++) {
		weights_cmp[i]=tdm_abs(weights[i]-weights_last[i]);
		#ifdef _SET_TDM_DEV
		TDM_PRINT6("\tDR_weight_last[%0d]=%03d | DR_weight[%0d]=%03d | DR_weight_cmp[%0d]=%03d\n",i,weights_last[i],i,weights[i],i,weights_cmp[i]);
		#endif
	}
	#ifdef _SET_TDM_DEV
	TDM_PRINT0("\n");
	#endif
}


/**
@name: tdm_weight_min
@param:

Returns minimum value from among nodal weight array
**/
int tdm_weight_min(int weights[VEC_MAP_WID], int span)
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
@name: tdm_weight_vnum
@param:

Returns number of subthreshold violations from nodal weight array
**/
int tdm_weight_vnum(int weights[VEC_MAP_WID], int span, int threshold)
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
@name: tdm_vector_diff
@param: unsigned char **, int, int

Given x index, diffs the transmuted vector at this position with the prototype from library
*** Returns PASS if within jitter limits, FAIL if illegal ***
**/
int tdm_vector_diff(unsigned char **map, int bw, int lim, int x, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1]) {

	/*
	int v, i, k, y, tmp_length, tmp_width, result=PASS, spd=0, jitter;
	unsigned char **temp_map;
	*/
	/* tmp_length, tmp_width is set but not used [-Werror=unused-but-set-variable]
	int tmp_length, tmp_width;
	*/
	int result=PASS;

	/* Generate fake TDM vector map */
	/* tmp_length, tmp_width is set but not used [-Werror=unused-but-set-variable]
	tmp_length=VEC_MAP_LEN;
	tmp_width=2; */ /* Accomodate 1 prototype and 1 to be diffed */
	
	/*
	temp_map=(unsigned char **) TDM_ALLOC(tmp_width*sizeof(unsigned char *), "temp_map_l1");
	for (v=0; v<tmp_width; v++) {
		temp_map[v]=(unsigned char *) TDM_ALLOC(tmp_length*sizeof(unsigned char), "temp_map_l2");
	}
	for (v=0; v<tmp_length; v++) {
		temp_map[0][v]=_TD2P_NUM_EXT_PORTS;
	}
	k=0; y=(VEC_MAP_LEN+1);
	for (v=0; v<lim; v++) {
		if (map[x][v]!=_TD2P_NUM_EXT_PORTS) {
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
	while(temp_map[1][0]==_TD2P_NUM_EXT_PORTS) {
		tdm_vector_rotate(temp_map[1], lim, -1);
	}
	for (v=0; v<lim; v++) {
		if (temp_map[0][v]!=_TD2P_NUM_EXT_PORTS) {temp_map[0][v]=temp_map[1][0];}
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
PUT ALL CHIP SPECIFIC CODE HERE
**/

/**
@name: TSC_port_transcription
@param: int[][], enum, int[]

For Trident2+ BCM56850
Physical port transcriber for tsc subports into physical port numbers

	40G 	- xxxx
      	20G 	- xx
	      	    xx
      	10G	- x
		   x
		    x
		     x
     	1X0G 	- xxxx_xxxx_xxxx
**/
void tsc_port_transcription(int wc_array[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS], int port_state_array[_TD2P_NUM_EXT_PORTS]) {
	int i,j,last_port=_TD2P_NUM_EXT_PORTS,tsc_bool;
	/*char tmp_str[16];*/
	/*sprintf (cmd_str, "soc setmem -index %x %s {", index->d, addr);*/
	
	for (i=0; i<32; i++) {
		for (j=0; j<4; j++) {
			if (wc_array[i][j]!=MGM_TOKEN) {
				wc_array[i][j]=_TD2P_NUM_EXT_PORTS;
			}
		}
	}
	for (i=1; i<(_TD2P_NUM_EXT_PORTS-1); i+=4) {
		tsc_bool=BOOL_FALSE;
		for (j=0; j<4; j++) {
			if (port_state_array[i-1+j]==1||port_state_array[i-1+j]==2) {
				tsc_bool=BOOL_TRUE;
			}
		}
		/* set management port tokens if enabled */
		if (((i-1)/4==3) && wc_array[(i-1)/4][0]==MGM_TOKEN) {
			if (speed[i] == speed[i+1]) {
				/* wc_array[(i-1)/4][0] = MGM_TOKEN;*/
				wc_array[(i-1)/4][1] = MGM_TOKEN;
				wc_array[(i-1)/4][2] = MGM_TOKEN;
				wc_array[(i-1)/4][3] = MGM_TOKEN;
			}
			else {
				/* wc_array[(i-1)/4][0] = MGM_TOKEN;*/
				wc_array[(i-1)/4][1] = _TD2P_NUM_EXT_PORTS;
				wc_array[(i-1)/4][2] = _TD2P_NUM_EXT_PORTS;
				wc_array[(i-1)/4][3] = _TD2P_NUM_EXT_PORTS;
			}
		}
		else if (tsc_bool) {
			if (speed[i] <= SPEED_42G_HG2 || port_state_array[i-1]==0) {
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
							wc_array[(i-1)/4][j] = _TD2P_NUM_EXT_PORTS;
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
			else {
				if (i<(_TD2P_NUM_EXT_PORTS-8)) {
					/*true 100G port*/
					if (speed[i] == SPEED_100G && port_state_array[i+10] != 3) {
						wc_array[(i-1)/4][0] = i;
						wc_array[(i-1)/4][1] = i;
						wc_array[(i-1)/4][2] = i;
						wc_array[(i-1)/4][3] = i;
						wc_array[(i+3)/4][0] = i;
						wc_array[(i+3)/4][1] = i;
						wc_array[(i+3)/4][2] = i;
						wc_array[(i+3)/4][3] = i;
						wc_array[(i+7)/4][0] = i;
						wc_array[(i+7)/4][1] = i;
						wc_array[(i+7)/4][2] = _TD2P_NUM_EXT_PORTS;
						wc_array[(i+7)/4][3] = _TD2P_NUM_EXT_PORTS;
					}
					/*120G and stretched 100G port*/
					else if (speed[i] == SPEED_100G || speed[i] == SPEED_120G) {
						wc_array[(i-1)/4][0] = i;	
						wc_array[(i-1)/4][1] = i;
						wc_array[(i-1)/4][2] = i;
						wc_array[(i-1)/4][3] = i;
						wc_array[(i+3)/4][0] = i;
						wc_array[(i+3)/4][1] = i;
						wc_array[(i+3)/4][2] = i;
						wc_array[(i+3)/4][3] = i;
						wc_array[(i+7)/4][0] = i;
						wc_array[(i+7)/4][1] = i;
						wc_array[(i+7)/4][2] = i;
						wc_array[(i+7)/4][3] = i;
					}
					i+=8;
				}
			}
		}
	}
}



/**
@name: TDM_pgw_scheduler
@param: int[][], enum, int*, int*, int*, int, int[], int[], int, int, int, int[], int, int, int[], int*, int

Populate bilayer PGW calender
**/
int TDM_pgw_scheduler(int wc_array[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS], int *cur_idx, int *pgw_tdm_idx, int *ovs_tdm_idx, int subp, int pgw_tdm_tbl[64], int ovs_tdm_tbl[32], int upperlimit, int iter1, int bw, int port_state_map[_TD2P_NUM_EXT_PORTS], int iter3, int stop1, int swap_array[32], int *z, int subport, int op_flags[1])
{
	/* General use */
	int p;
	/* Port counts */
	int num_lr=0, num_lr_1=0, num_lr_10=0, num_lr_12=0, num_lr_20=0, num_lr_40=0, num_lr_100=0, num_lr_120=0, num_ovs=0, num_off=0;
	/* Scheduler structural variables */
	int l1_tdm_len, l1_ovs_cnt = 0, max_tdm_len, pgw_tdm_idx_sub, gestalt_iter, gestalt_id_iter, gestalt_id = 0, gestalt_tag;
	/* Scheduler operation variables */
	int pad_40g_in_480g = BOOL_FALSE, xcount, pgw_tdm_idx_lookback = 0, sm_iter = 0, ovs_pad_iter = 0;

	TDM_VERBOSE0("TDM: ----------------------------------------------------------------\n");
	/* Set layer 1 TDM max subspace length */
	switch(bw) {
		case 415:
		case 416:
			pgw_tdm_idx_sub = 3;
			break;
		case 517: case 537:
		case 518:  
			pgw_tdm_idx_sub = 4;
			break;
		case 608:
		case 609:
			if ((iter3 == 32 || iter3 == 64) && (stop1 == 64 || stop1 == 96)) {pgw_tdm_idx_sub = 5;}
			else {pgw_tdm_idx_sub = 4;}
			break;
		case 760:
			pgw_tdm_idx_sub = 6;
			break;
		default:
			pgw_tdm_idx_sub = 8;
			break;
	}
	max_tdm_len=(4*pgw_tdm_idx_sub);
	for (p = iter3; p < stop1; p++) {
		if (port_state_map[p] == 1) {
			num_lr++;
			switch (speed[p+1]) {
				case SPEED_10G:
				case SPEED_10G_DUAL:
				case SPEED_10G_XAUI:
					if (iter3<17 && wc_array[3][0]==MGM_TOKEN) {
						num_lr--;
					}
					else {
						num_lr_10++;
					}
					break;
				case SPEED_12G:
					num_lr_12++;
					break;
				case SPEED_20G:
				case SPEED_21G:
				case SPEED_21G_DUAL:
					num_lr_20++;
					break;
				case SPEED_40G:
				case SPEED_42G:
				case SPEED_42G_HG2:
					num_lr_40++;
					break;
				case SPEED_100G:
					num_lr_100++;
					break;
				case SPEED_120G:
					num_lr_120++;
					break;
				default:
					break;
			}
		}
		else if (port_state_map[p] == 2) {
			num_ovs++;
		}
		else if (port_state_map[p] == 0 || ( (p%4==0) && (port_state_map[p]==3) ) ) {
			num_off++;
		}
	}
	/* Number of slots required */
	/* 10G resolution */
	if (num_lr_12>0) {
		max_tdm_len*=2;
		l1_tdm_len = (num_lr_1+(num_lr_10*2)+(num_lr_12*3)+(num_lr_20*4)+(num_lr_40*8)+(num_lr_100*20)+(num_lr_120*24));
	}
	else {
		l1_tdm_len = (num_lr_1+num_lr_10+(num_lr_20*2)+(num_lr_40*4)+(num_lr_100*10)+(num_lr_120*12));
	}
	/* Check bandwidth overloading */
	if ( (l1_tdm_len>max_tdm_len) && (l1_tdm_len>32) ) {
		TDM_ERROR0("bandwidth overloaded\n");
		return 0;
	}
	/* Number of OVS tokens required */
	if (num_ovs > 0) {
		TDM_VERBOSE0("TDM: _____VERBOSE: oversub detected\n");
		if (num_lr_100!=1 && l1_tdm_len!=0) {
			if ( (max_tdm_len-l1_tdm_len)%4==0 ) {
				l1_ovs_cnt=((max_tdm_len-l1_tdm_len)/4);
			}
			else if ( (max_tdm_len-l1_tdm_len)%2==0 ) {
				if (subport == 1 || subport == 3) {
					l1_ovs_cnt=(max_tdm_len-l1_tdm_len+((max_tdm_len-l1_tdm_len)%4))/4;
				}
				else {
					l1_ovs_cnt = ((max_tdm_len-l1_tdm_len)/4);
				}
			}
			else {
				l1_ovs_cnt = ((max_tdm_len - l1_tdm_len - (4 - l1_tdm_len % 4))/4);
			}
		}
		else if (num_lr_100==1) {
			l1_ovs_cnt = 1;
		}
		else if (l1_tdm_len == 0) {
			l1_ovs_cnt = max_tdm_len;
		}
		if ((l1_ovs_cnt==0) && (op_flags[0]==0)) {
			TDM_ERROR0("TDM: _____ERROR: unable to partition oversubscription tokens\n");
		}
		else {
			TDM_VERBOSE1("TDM: _____VERBOSE: %0d oversub tokens assigned\n", l1_ovs_cnt);
		}
	}
	/* Check 480G padding */
	if (bw==480 && num_lr<3) {
		pad_40g_in_480g = BOOL_TRUE;
	}	
	TDM_VERBOSE1("TDM: _____VERBOSE: base oversubscription token pool is %0d\n", l1_ovs_cnt);
	TDM_VERBOSE0("TDM: ----------------------------------------------------------------\n");
	TDM_VERBOSE1("TDM: _____VERBOSE: 1G line rate ports in this quadrant: %0d\n", num_lr_1);
	TDM_VERBOSE1("TDM: _____VERBOSE: 10G line rate ports in this quadrant: %0d\n", num_lr_10);
	TDM_VERBOSE1("TDM: _____VERBOSE: 12G line rate ports in this quadrant: %0d\n", num_lr_12);
	TDM_VERBOSE1("TDM: _____VERBOSE: 20G line rate ports in this quadrant: %0d\n", num_lr_20);
	TDM_VERBOSE1("TDM: _____VERBOSE: 40G line rate ports in this quadrant: %0d\n", num_lr_40);
	TDM_VERBOSE1("TDM: _____VERBOSE: 100G gestalt line rate ports in this quadrant: %0d\n", num_lr_100);
	TDM_VERBOSE1("TDM: _____VERBOSE: 120G gestalt line rate ports in this quadrant: %0d\n", num_lr_120);
	TDM_VERBOSE1("TDM: _____VERBOSE: oversubscribed ports in this quadrant: %0d\n", num_ovs);
	TDM_VERBOSE1("TDM: _____VERBOSE: disabled subports in this quadrant: %0d\n", num_off);
	TDM_VERBOSE2("COVERAGE FROM PHYSICAL PORTS %0d to %0d\n", iter3, stop1);
	/* Obligate oversubscription */
	if ((l1_tdm_len>max_tdm_len) && (num_ovs == 0)) {
		TDM_WARN0("TDM: _____WARNING: applying obligate oversubscription correction\n");
		for (xcount=0; xcount<max_tdm_len; xcount++) {
			while ((*cur_idx < upperlimit) && ((wc_array[*cur_idx][subp] == _TD2P_NUM_EXT_PORTS))) {	
				(*cur_idx)++;
			}
			if (*cur_idx >= upperlimit) {
				return 1;
			}
			if ((*pgw_tdm_idx) < max_tdm_len) {
				pgw_tdm_tbl[*pgw_tdm_idx] = OVSB_TOKEN;
				TDM_VERBOSE2("Pipe : %0d pgw tdm tbl element #0%0d has an oversub token placed\n", upperlimit, *pgw_tdm_idx);
				(*pgw_tdm_idx)++;
			}
			ovs_tdm_tbl[*ovs_tdm_idx] = wc_array[*cur_idx][subp];
			TDM_VERBOSE3("Pipe : %0d ovs tdm tbl element #0%0d, content added: %0d\n", upperlimit, *ovs_tdm_idx,  ovs_tdm_tbl[*ovs_tdm_idx]);
			(*ovs_tdm_idx)++;
			(*cur_idx)++;
			while ((*cur_idx < upperlimit) && ((wc_array[*cur_idx][subp] == _TD2P_NUM_EXT_PORTS))) {	
				(*cur_idx)++;
			}
			if (*cur_idx >= upperlimit) {
				return 1;
			}
		}
		return 1;
	}
	/* 100G/120G dominating case */
	else if ((num_lr_100 + num_lr_120 == 1) && (num_lr_10)+((num_lr_20)*2)+((num_lr_40)*4) < ((num_lr_100)*10)+((num_lr_120)*12)) {
		/* Preemptively find the 100G port */
		for (gestalt_id_iter = iter3; gestalt_id_iter < stop1; gestalt_id_iter++) {
			if (speed[gestalt_id_iter+1]==SPEED_100G || speed[gestalt_id_iter+1]==SPEED_120G) {
				gestalt_id = (gestalt_id_iter + 1);
			}
		}
		if (num_lr_100 == 1 && num_lr_120 == 0) {
			for (gestalt_iter = 0; gestalt_iter < 5; gestalt_iter++) {
				gestalt_tag = tdm_PQ((((max_tdm_len/2)*10)/5) * gestalt_iter);
				/* gestalt_tag = tdm_PQ((((float) (max_tdm_len/2))/((float) 5.0)) * gestalt_iter); */
				pgw_tdm_tbl[gestalt_tag] = gestalt_id;
				pgw_tdm_tbl[gestalt_tag+12] = gestalt_id;
			}
		}
		else if (num_lr_100 == 0 && num_lr_120 == 1) {
			for (gestalt_iter = 0; gestalt_iter < 12; gestalt_iter++) {
				gestalt_tag = tdm_PQ((((max_tdm_len-1)*10)/11) * gestalt_iter);
				/* gestalt_tag = tdm_PQ((((float) max_tdm_len-1)/((float) 11.0)) * gestalt_iter); */
				pgw_tdm_tbl[gestalt_tag] = gestalt_id;
			}
		}
		while (sm_iter < _TD2P_NUM_EXT_PORTS) {
			/* Chopping block */
			while ((*cur_idx < upperlimit) && ((wc_array[*cur_idx][subp] == _TD2P_NUM_EXT_PORTS) || (wc_array[*cur_idx][subp] == MGM_TOKEN))) {
				(*cur_idx)++;
			}
			if (*cur_idx >= upperlimit) {
				int pp_iter;
				for (pp_iter = 0; pp_iter < max_tdm_len; pp_iter++) if (pgw_tdm_tbl[pp_iter] == _TD2P_NUM_EXT_PORTS) pgw_tdm_tbl[pp_iter] = OVSB_TOKEN;
				return 1;
			}
			TDM_VERBOSE0("TDM: ----------------------------------------------------------------\n");
			TDM_VERBOSE2("the present port is %0d, speed %0d\n", wc_array[*cur_idx][subp], speed[wc_array[*cur_idx][subp]]);
			/* Port state machine */
			switch(port_state_map[(wc_array[*cur_idx][subp])-1]) {
				case 0:
					TDM_VERBOSE0("TDM: _____VERBOSE: the port is DISABLED\n");
					TDM_VERBOSE2("TDM: Pipe : %0d the pgw tdm tbl element #0%0d would be an invalid port\n", upperlimit, *pgw_tdm_idx);
					if (pgw_tdm_tbl[*pgw_tdm_idx] != gestalt_id) {
						pgw_tdm_tbl[*pgw_tdm_idx] = OVSB_TOKEN;
						(*pgw_tdm_idx)++;
						(*cur_idx)++;
					}
					else if (num_ovs > 0) {
						(*z)++;
						swap_array[*z] = OVSB_TOKEN;
						TDM_VERBOSE2("TDM: Pipe : %0d pushed an OVS token into LIFO swap buffer at index %0d\n", upperlimit, *z);
						(*cur_idx)++;
					}
					break;
				case 1:
					TDM_VERBOSE0("TDM: _____VERBOSE: the port is LINE RATE\n");
					while (pgw_tdm_tbl[(*pgw_tdm_idx)] == gestalt_id) {
						(*pgw_tdm_idx)++;
					}
					while (swap_array[*z] == gestalt_id) {
						swap_array[*z] = 0;
						(*z)--;
					}
					if (num_ovs == 0) {
						TDM_VERBOSE1("TDM: the cur_idx is %0d\n", *cur_idx);
						while ((*z) > 0 && 
						        pgw_tdm_tbl[(*pgw_tdm_idx)-1] != swap_array[*z])
						{
							TDM_VERBOSE4("TDM: Pipe : %0d priority dequeuing from LIFO swap buffer, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *z, *pgw_tdm_idx, swap_array[*z]);
							pgw_tdm_tbl[*pgw_tdm_idx] = swap_array[*z];
							(*pgw_tdm_idx)++;
							(*z)--;
							TDM_VERBOSE1("TDM: _____DEBUG: LIFO index pointer is %0d\n", *z);
						}
						if ((wc_array[*cur_idx][subp] != pgw_tdm_tbl[(*pgw_tdm_idx)-1] || (*pgw_tdm_idx)==0))
						{
							pgw_tdm_tbl[*pgw_tdm_idx] = wc_array[*cur_idx][subp];
							TDM_VERBOSE3("TDM: Pipe : %0d the pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *pgw_tdm_idx, pgw_tdm_tbl[*pgw_tdm_idx]);
							(*pgw_tdm_idx)++;
							(*cur_idx)++;
						}
						else if (wc_array[*cur_idx][subp] != gestalt_id) {
							(*z)++;
							swap_array[*z] = wc_array[*cur_idx][subp];
							TDM_VERBOSE3("TDM: Pipe : %0d pushed port %0d into LIFO swap buffer at index %0d\n", upperlimit, wc_array[*cur_idx][subp], *z);
							(*cur_idx)++;
						}
						else (*cur_idx)++;
					}
					else if (num_ovs > 0) {
						TDM_VERBOSE1("TDM: the cur_idx is %0d\n", *cur_idx);
						if ((*z) > 0 && 
						    pgw_tdm_tbl[(*pgw_tdm_idx)-2] != swap_array[*z] &&
						    pgw_tdm_tbl[(*pgw_tdm_idx)-1] == OVSB_TOKEN)
						{
							TDM_VERBOSE4("TDM: Pipe : %0d priority dequeuing from LIFO swap buffer, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *z, *pgw_tdm_idx, swap_array[*z]);
							pgw_tdm_tbl[*pgw_tdm_idx] = swap_array[*z];
							(*pgw_tdm_idx)++;
							(*z)--;
							TDM_VERBOSE1("TDM: _____DEBUG: LIFO index pointer is %0d\n", *z);
						}
						while ((*z) > 0 && 
							pgw_tdm_tbl[(*pgw_tdm_idx)-1] != swap_array[*z] && 
							pgw_tdm_tbl[(*pgw_tdm_idx)-1] != OVSB_TOKEN)
						{
							TDM_VERBOSE4("TDM: Pipe : %0d priority dequeuing from LIFO swap buffer, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *z, *pgw_tdm_idx, swap_array[*z]);
							pgw_tdm_tbl[*pgw_tdm_idx] = swap_array[*z];
							(*pgw_tdm_idx)++;
							(*z)--;
							TDM_VERBOSE1("TDM: _____DEBUG: LIFO index pointer is %0d\n", *z);
						}
						if ((wc_array[*cur_idx][subp] != pgw_tdm_tbl[(*pgw_tdm_idx)-1] || (*pgw_tdm_idx)==0) &&
						    wc_array[*cur_idx][subp] != gestalt_id)
						{
							pgw_tdm_tbl[*pgw_tdm_idx] = wc_array[*cur_idx][subp];
							TDM_VERBOSE3("TDM: Pipe : %0d the pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *pgw_tdm_idx, pgw_tdm_tbl[*pgw_tdm_idx]);
							(*pgw_tdm_idx)++;
							(*cur_idx)++;
						}
						else if (wc_array[*cur_idx][subp] != gestalt_id) {
							(*z)++;
							swap_array[*z] = wc_array[*cur_idx][subp];
							TDM_VERBOSE3("TDM: Pipe : %0d pushed port %0d into LIFO swap buffer at index %0d\n", upperlimit, wc_array[*cur_idx][subp], *z);
							(*cur_idx)++;
						}
						else (*cur_idx)++;
					}
					break;
				case 2:
					TDM_VERBOSE0("TDM: _____VERBOSE: the port is OVERSUBSCRIBED\n");
					TDM_VERBOSE1("TDM: the ovs_tdm_idx is %0d\n", *ovs_tdm_idx);
					TDM_VERBOSE1("TDM: the cur_idx is %0d\n", *cur_idx);
					ovs_tdm_tbl[*ovs_tdm_idx] = wc_array[*cur_idx][subp];
					TDM_VERBOSE3("TDM: Pipe : %0d the ovs tdm tbl element #0%0d, content is %0d\n", upperlimit, *ovs_tdm_idx,  ovs_tdm_tbl[*ovs_tdm_idx]);
					(*ovs_tdm_idx)++;
					(*cur_idx)++;
					break;
			}
			/* Chopping block */
			while ((*cur_idx < upperlimit) && ((wc_array[*cur_idx][subp] == _TD2P_NUM_EXT_PORTS) || (wc_array[*cur_idx][subp] == MGM_TOKEN))) {
				(*cur_idx)++;
			}
			if (*cur_idx >= upperlimit)  {
				int pp_iter;
				for (pp_iter = 0; pp_iter < max_tdm_len; pp_iter++) if (pgw_tdm_tbl[pp_iter] == _TD2P_NUM_EXT_PORTS) pgw_tdm_tbl[pp_iter] = OVSB_TOKEN;
				return 1;
			}
			sm_iter++;
		}
		return 1;
	}
	else {
		while (sm_iter < _TD2P_NUM_EXT_PORTS) {
			while ((*cur_idx < upperlimit) && ((wc_array[*cur_idx][subp] == _TD2P_NUM_EXT_PORTS) || (wc_array[*cur_idx][subp] == MGM_TOKEN))) {
				(*cur_idx)++;
			}
			if (*cur_idx >= upperlimit) {
				TDM_VERBOSE2("entering post-processing 1, executing lookback %0d against subset limit %0d\n", pgw_tdm_idx_lookback, pgw_tdm_idx_sub);
				if (l1_ovs_cnt > 0) {
					ovs_pad_iter = 0;
					while (ovs_pad_iter < l1_ovs_cnt && pgw_tdm_idx_lookback < pgw_tdm_idx_sub) {
						pgw_tdm_tbl[*pgw_tdm_idx] = OVSB_TOKEN;
						TDM_VERBOSE2("Pipe : %0d the pgw tdm tbl element #0%0d, insert OVS token\n", upperlimit, *pgw_tdm_idx);
						(*pgw_tdm_idx)++;
						pgw_tdm_idx_lookback++;
						ovs_pad_iter++;
					}
				}
				if (pad_40g_in_480g==BOOL_TRUE) {
					/* At 480G, TDM must be padded to max length or underrun */
					while ( (bw<=480) && (pgw_tdm_idx_sub > pgw_tdm_idx_lookback) )
					{
						pgw_tdm_tbl[*pgw_tdm_idx] = OVSB_TOKEN;
						(*pgw_tdm_idx)++;
						pgw_tdm_idx_lookback++;
						if (pgw_tdm_idx_lookback >= pgw_tdm_idx_sub) break;
					}
				}
				return 1;
			}
			TDM_VERBOSE0("TDM: ----------------------------------------------------------------\n");
			TDM_VERBOSE2("the present port is %0d, speed %0d\n", wc_array[*cur_idx][subp],speed[wc_array[*cur_idx][subp]]);
			switch(port_state_map[(wc_array[*cur_idx][subp])-1]) {
				case 0:
					TDM_VERBOSE0("TDM: _____VERBOSE: the port is DISABLED\n");
					TDM_VERBOSE2("Pipe : %0d the pgw tdm tbl element #0%0d would be an invalid port\n", upperlimit, *pgw_tdm_idx);
					if (num_lr > 0 && num_ovs > 0) {
						pgw_tdm_tbl[*pgw_tdm_idx] = OVSB_TOKEN;
						(*pgw_tdm_idx)++;
						pgw_tdm_idx_lookback++;
					}
					(*cur_idx)++;
					break;
				/* this only iterates by active port (not lane) so it should never see this */	
				/* case 3:
					TDM_VERBOSE0("TDM: _____VERBOSE: the port is part of a DUAL/QUAD/100G\n"); */
				case 1:
					TDM_VERBOSE0("TDM: _____VERBOSE: the port is LINE RATE\n");
					if (num_ovs == 0) {
						TDM_VERBOSE1("TDM: _____VERBOSE: the cur_idx is %0d\n", *cur_idx);
						while ((*z) > 0 && pgw_tdm_tbl[(*pgw_tdm_idx)-1] != swap_array[*z]) {
							TDM_VERBOSE4("TDM: Pipe : %0d priority dequeuing from LIFO swap buffer, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *z, *pgw_tdm_idx, swap_array[*z]);
							pgw_tdm_tbl[*pgw_tdm_idx] = swap_array[*z];
							(*pgw_tdm_idx)++;
							(*z)--;
							TDM_VERBOSE1("TDM: _____DEBUG: LIFO index pointer is %0d\n", *z);
						}
						if ((wc_array[*cur_idx][subp] != pgw_tdm_tbl[(*pgw_tdm_idx)-1] || (*pgw_tdm_idx)==0)) {
							pgw_tdm_tbl[*pgw_tdm_idx] = wc_array[*cur_idx][subp];
							TDM_VERBOSE3("TDM: Pipe : %0d the pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *pgw_tdm_idx, pgw_tdm_tbl[*pgw_tdm_idx]);
							(*pgw_tdm_idx)++;
							(*cur_idx)++;
						}
						else {
							(*z)++;
							swap_array[*z] = wc_array[*cur_idx][subp];
							TDM_VERBOSE3("TDM: Pipe : %0d pushed port %0d into LIFO swap buffer at index %0d\n", upperlimit, wc_array[*cur_idx][subp], *z);
							(*cur_idx)++;
						}
						/* At 480G the table must be padded or underrun */
						if (bw<=480 && pad_40g_in_480g==BOOL_TRUE) {
							pgw_tdm_idx_lookback++;
						}
						/* All faster bandwidths ok */
						else {
							pgw_tdm_idx_lookback = pgw_tdm_idx_sub;
						}
					}
					else if (num_ovs > 0) {
						TDM_VERBOSE1("TDM: _____VERBOSE: the cur_idx is %0d\n", *cur_idx);
						if ((*z) > 0 && 
						    pgw_tdm_tbl[(*pgw_tdm_idx)-2] != swap_array[*z] &&
						    pgw_tdm_tbl[(*pgw_tdm_idx)-1] == OVSB_TOKEN &&
						    (pgw_tdm_idx_lookback < (pgw_tdm_idx_sub-l1_ovs_cnt)))
						{
							TDM_VERBOSE4("TDM: Pipe : %0d priority dequeuing from LIFO swap buffer, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *z, *pgw_tdm_idx, swap_array[*z]);
							pgw_tdm_tbl[*pgw_tdm_idx] = swap_array[*z];
							(*pgw_tdm_idx)++;
							(*z)--;
							pgw_tdm_idx_lookback++;
							TDM_VERBOSE1("TDM: _____DEBUG: LIFO index pointer is %0d\n", *z);
						}
						while ((*z) > 0 && 
							pgw_tdm_tbl[(*pgw_tdm_idx)-1] != swap_array[*z] && 
							pgw_tdm_tbl[(*pgw_tdm_idx)-1] != OVSB_TOKEN &&
							(pgw_tdm_idx_lookback < (pgw_tdm_idx_sub-l1_ovs_cnt)))
						{
							TDM_VERBOSE4("TDM: Pipe : %0d priority dequeuing from LIFO swap buffer, pointer index %0d, to pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *z, *pgw_tdm_idx, swap_array[*z]);
							pgw_tdm_tbl[*pgw_tdm_idx] = swap_array[*z];
							(*pgw_tdm_idx)++;
							(*z)--;
							pgw_tdm_idx_lookback++;
							TDM_VERBOSE1("TDM: _____DEBUG: LIFO index pointer is %0d\n", *z);
						}
						if ((wc_array[*cur_idx][subp] != pgw_tdm_tbl[(*pgw_tdm_idx)-1] || (*pgw_tdm_idx)==0) && 
						    (pgw_tdm_idx_lookback < (pgw_tdm_idx_sub-l1_ovs_cnt)))
						{
							pgw_tdm_tbl[*pgw_tdm_idx] = wc_array[*cur_idx][subp];
							TDM_VERBOSE3("TDM: Pipe : %0d the pgw tdm tbl element #0%0d, content is %0d\n", upperlimit, *pgw_tdm_idx, pgw_tdm_tbl[*pgw_tdm_idx]);
							(*pgw_tdm_idx)++;
							(*cur_idx)++;
							pgw_tdm_idx_lookback++;
						}
						else {
							(*z)++;
							swap_array[*z] = wc_array[*cur_idx][subp];
							TDM_VERBOSE3("TDM: Pipe : %0d pushed port %0d into LIFO swap buffer at index %0d\n", upperlimit, wc_array[*cur_idx][subp], *z);
							(*cur_idx)++;
						}
					}
					break;
				case 2:
					TDM_VERBOSE0("TDM: _____VERBOSE: the port is OVERSUBSCRIBED\n");
					TDM_VERBOSE1("TDM: the ovs_tdm_idx is %0d\n", *ovs_tdm_idx);
					TDM_VERBOSE1("TDM: the cur_idx is %0d\n", *cur_idx);
					ovs_tdm_tbl[*ovs_tdm_idx] = wc_array[*cur_idx][subp];
					TDM_VERBOSE3("TDM: Pipe : %0d the ovs tdm tbl element #0%0d, content is %0d\n", upperlimit, *ovs_tdm_idx,  ovs_tdm_tbl[*ovs_tdm_idx]);
					(*ovs_tdm_idx)++;
					if (subp != 0 && port_state_map[(wc_array[*cur_idx][subp-1])-1] == 1) {
						pgw_tdm_tbl[*pgw_tdm_idx] = OVSB_TOKEN;
						(*pgw_tdm_idx)++;
						pgw_tdm_idx_lookback++;
					}
					(*cur_idx)++;
					break;
			}
			while ((*cur_idx < upperlimit) && ((wc_array[*cur_idx][subp] == _TD2P_NUM_EXT_PORTS) || (wc_array[*cur_idx][subp] == MGM_TOKEN))) {
				(*cur_idx)++;
			}
			if (*cur_idx >= upperlimit) {
				/* Table post processing */
				TDM_VERBOSE2("entering post-processing 2, executing lookback %0d against subset limit %0d\n", pgw_tdm_idx_lookback, pgw_tdm_idx_sub);
				if (l1_ovs_cnt > 0) {
					int ovs_pad_iter = 0;
					while (ovs_pad_iter < l1_ovs_cnt && pgw_tdm_idx_lookback < pgw_tdm_idx_sub) {
						pgw_tdm_tbl[*pgw_tdm_idx] = OVSB_TOKEN;
						TDM_VERBOSE2("TDM: Pipe : %0d the pgw tdm tbl element #0%0d, insert OVS token\n", upperlimit, *pgw_tdm_idx);
						(*pgw_tdm_idx)++;
						pgw_tdm_idx_lookback++;
						ovs_pad_iter++;
					}
				}
				if (pad_40g_in_480g==BOOL_TRUE) {
					/* At 480G, TDM must be padded to max length or underrun */
					while ( (bw<=480) && (pgw_tdm_idx_sub > pgw_tdm_idx_lookback) ) {
						pgw_tdm_tbl[*pgw_tdm_idx] = OVSB_TOKEN;
						(*pgw_tdm_idx)++;
						pgw_tdm_idx_lookback++;
						if (pgw_tdm_idx_lookback >= pgw_tdm_idx_sub) break;
					}
				}
				return 1;
			}
			sm_iter++;
		}
	}
	TDM_VERBOSE1("TDM: the pgw_tdm_idx is %0d\n", *pgw_tdm_idx);
	TDM_VERBOSE1("TDM: the cur_idx is %0d\n", *cur_idx);
	
	return 1;
	
}


/**
@name: tdm_parse_quad
@param:

Summarizes quadrant config
**/
void tdm_parse_quad(FILE *file, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int portmap[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int traffic[NUM_TSC+1], int pipe_start, int pipe_end)
{
	int iter, iter2;
	
	/* Summarize the port config in this quadrant */
	TDM_BIG_BAR
	TDM_PRINT0("TDM: --- Pipe Config ---: ");
	for (iter=pipe_start; iter<=pipe_end; iter++) {
		if ((((iter-1)%16)==0)) {
			TDM_PRINT0("\nTDM: ");
		}		
		TDM_PRINT1("{%03d}\t",(iter));
		if (iter%16==0) {
			TDM_PRINT0("\nTDM: ");
			for (iter2=(iter-16); iter2<iter; iter2++) {
				TDM_PRINT1("%d\t", speed[iter2+1]);
			}
			TDM_PRINT0("\nTDM: ");
			for (iter2=(iter-16); iter2<iter; iter2++) {
				if ((iter2-1)%4==0) {
					switch (portmap[iter2-1]) {
						case 1:
							TDM_PRINT0("LINE\t---\t---\t---\t");
							break;
						case 2:
							TDM_PRINT0("OVSB\t---\t---\t---\t");
							break;
						default:
							break;
					}
				}
			}
			TDM_PRINT0("\nTDM: ");
			for (iter2=(iter-16); iter2<iter; iter2++) {
				if ((iter2-1)%4==0) {
					switch (traffic[which_tsc((iter2+1), tsc)]) {
						case 999:
							TDM_PRINT0("HIGIG2\t---\t---\t---\t");
							break;
						case 998:
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
@name: write_mmu_tdm_tbl
@param:

MMU table collector function
 **/
int write_mmu_tdm_tbl(FILE *file, int pgw0[64], int pgw1[64], int ovs0[32], int ovs1[32], int mmu[256], int bucket1[_TD2P_OS_GROUP_LEN], int bucket2[_TD2P_OS_GROUP_LEN], int bucket3[_TD2P_OS_GROUP_LEN], int bucket4[_TD2P_OS_GROUP_LEN], int x_pipe, int port_state_map[_TD2P_NUM_EXT_PORTS], int tsc[NUM_TSC][4], int bw, enum port_speed speed[_TD2P_NUM_EXT_PORTS], int (*op_flags)[1], int traffic[NUM_TSC+1])
{
	/* Iterators and temporary */
	int i, j;
	/* Buffers */
	int lr_buffer[64], os_buffer[64], bucket5[_TD2P_OS_GROUP_LEN], bucket6[_TD2P_OS_GROUP_LEN], bucket7[_TD2P_OS_GROUP_LEN], bucket8[_TD2P_OS_GROUP_LEN];
	/* Constants */
	int clk, scheduler_state, pipe, lr_idx_limit=LEN_760MHZ_EN;
	unsigned char acc, higig_mgmt=BOOL_FALSE, lr_buffer_idx=0, os_buffer_idx=0;

	switch (bw) {
		case 608:
		case 609: 
			clk=609; acc=TD2_ANCILLARY_PORTS; break;
		case 517: case 537:
		case 518: 
			clk=518; acc=(TD2_ANCILLARY_PORTS-2); break;
		case 416:
		case 415:
			clk=415; acc=TD2_ANCILLARY_PORTS; break;
		default:
			clk=760; acc=(TD2_ANCILLARY_PORTS-2); break;
	}
	for (j=0; j<_TD2P_OS_GROUP_LEN; j++) {
		bucket5[j]=_TD2P_NUM_EXT_PORTS;
		bucket6[j]=_TD2P_NUM_EXT_PORTS;
		bucket7[j]=_TD2P_NUM_EXT_PORTS;
		bucket8[j]=_TD2P_NUM_EXT_PORTS;
	}
	for (j=0; j<64; j++) {
		lr_buffer[j]=_TD2P_NUM_EXT_PORTS;
		os_buffer[j]=_TD2P_NUM_EXT_PORTS;
	}
	
	for (j=0; j<64; j++) {
		TOKEN_CHECK(pgw0[j]) {
			lr_buffer[lr_buffer_idx++]=pgw0[j];
		}
	}
	for (j=0; j<64; j++) {
		TOKEN_CHECK(pgw1[j]) {
			lr_buffer[lr_buffer_idx++]=pgw1[j];
		}
	}
	for (j=0; j<32; j++) {
		TOKEN_CHECK(ovs0[j]) {
			os_buffer[os_buffer_idx++]=ovs0[j];
		}
	}
	for (j=0; j<32; j++) {
		TOKEN_CHECK(ovs1[j]) {
			os_buffer[os_buffer_idx++]=ovs1[j];
		}
	}
	
	if (x_pipe) {pipe=0; tdm_parse_quad(file, speed, port_state_map, tsc, traffic, 1, 64);}
	else {pipe=1; tdm_parse_quad(file, speed, port_state_map, tsc, traffic, 65, 128);}
	
	/* Modify port_state_map if obligate oversubscription has been applied */
	for (j=0; j<128; j++) {
		if (port_state_map[j]==1) {
			for (i=0; i<64; i++) {
				if (os_buffer[i]==(j+1)) {
					port_state_map[j]=2;
				}
			}
		}
	}

	/* Max length of the TDM vector map */
	if ( (traffic[NUM_TSC]==PORT_HIGIG2&&(pipe==1 || pipe==2)) && ((lr_buffer[0]!=_TD2P_NUM_EXT_PORTS&&bw>=MIN_HG_FREQ)||(lr_buffer[0]==_TD2P_NUM_EXT_PORTS)) ) {
		higig_mgmt=BOOL_TRUE;
	}
	if ( (check_ethernet(lr_buffer[0], speed, tsc, traffic)) && (higig_mgmt==BOOL_FALSE) ) {
		switch (bw) {
			case 760: lr_idx_limit=(LEN_760MHZ_EN-acc); break; /* 760MHz (192+8)*2.65G */			
			case 609: case 608: lr_idx_limit=(LEN_608MHZ_EN-acc); break; /* 608.333MHz (160+10)*2.65G */			
			case 518: case 517: case 537: lr_idx_limit=(LEN_517MHZ_EN-acc); break; /* 517.857MHz (128+8)*2.65G */			
			case 415: case 416: lr_idx_limit=(LEN_415MHZ_EN-acc); break; /* 415.625MHz (106+10)*2.65G */			
			default:
				TDM_ERROR0("Invalid frequency\n");
				return 0;
				break;
		}
	}
	else {
		switch (bw) {
			case 760: lr_idx_limit=(LEN_760MHZ_HG-acc); break; /* 760MHz (192+8)*2.65G */
			case 609: case 608: lr_idx_limit=(LEN_608MHZ_HG-acc); break; /* 608.333MHz (160+10)*2.65G */
			case 518: case 517: case 537: lr_idx_limit=(LEN_517MHZ_HG-acc); break; /* 517.857MHz (128+8)*2.65G */
			case 415: case 416: lr_idx_limit=(LEN_415MHZ_HG-acc); break; /* 415.625MHz (106+10)*2.65G */
			default:
				TDM_ERROR0("Invalid frequency\n");
				return 0;
				break;
		}
	}
	
	scheduler_state = TDM_scheduler_2d(file, clk, speed, port_state_map, tsc, traffic, lr_buffer, os_buffer, lr_idx_limit, mmu, bucket1, bucket2, bucket3, bucket4, bucket5, bucket6, bucket7, bucket8, pipe, acc, higig_mgmt);
	TDM_BIG_BAR	
	
	return scheduler_state;
	
}


/**
@name: TD2p_tdm_ovs_spacer
@param: int[][], int[], int[]

Expands OVS table into Flexport configuration
**/
void TD2p_tdm_ovs_spacer(int wc[NUM_TSC][4], int ovs_tdm_tbl[32], int ovs_spacing[32])
{
	int i, j, v=0, w=0, k=0, a, b, c=0;
	int pivot[32]; for (i=0; i<32; i++) pivot[i] = -1;
	/* spacing state machine */
	for (j=0; j<32; j++) if (ovs_tdm_tbl[j] == _TD2P_NUM_EXT_PORTS) k++;
	switch (k)
	{
		case 0:
			break;
		case 1:
			for (j=31; j>16; j--) ovs_tdm_tbl[j] = ovs_tdm_tbl[j-1];
			ovs_tdm_tbl[16] = _TD2P_NUM_EXT_PORTS;
			break;
		case 2:
			for (j=30; j>16; j--) ovs_tdm_tbl[j] = ovs_tdm_tbl[j-1];
			ovs_tdm_tbl[16] = _TD2P_NUM_EXT_PORTS;
			ovs_tdm_tbl[31] = _TD2P_NUM_EXT_PORTS;
			break;
		case 3:
			for (j=31; j>8; j--) ovs_tdm_tbl[j] = ovs_tdm_tbl[j-1];
			for (j=31; j>16; j--) ovs_tdm_tbl[j] = ovs_tdm_tbl[j-1];
			for (j=31; j>24; j--) ovs_tdm_tbl[j] = ovs_tdm_tbl[j-1];
			ovs_tdm_tbl[8] = _TD2P_NUM_EXT_PORTS;
			ovs_tdm_tbl[16] = _TD2P_NUM_EXT_PORTS;
			ovs_tdm_tbl[24] = _TD2P_NUM_EXT_PORTS;
			break;
		default:
			k/=4;
			/* TSC searching algorithm, expected singular result */
			for (i=0; i<NUM_TSC; i++) for (j=0; j<4; j++) if (wc[i][j] == ovs_tdm_tbl[0]) w = i;
			/*. find pivot points */
			for (j=1; j<32; j++) if (ovs_tdm_tbl[j]==wc[w][0] || ovs_tdm_tbl[j]==wc[w][1] || ovs_tdm_tbl[j]==wc[w][2] || ovs_tdm_tbl[j]==wc[w][3])
			{
				pivot[v] = j;
				v++;
			}
			for (j=0; j<3; j++)
			{
				if (pivot[j]!=-1)
				{
					for (i=31; i>(pivot[j]+k-1); i--) ovs_tdm_tbl[i] = ovs_tdm_tbl[i-k];
					for (i=pivot[j]; i<(pivot[j]+k); i++) ovs_tdm_tbl[i] = _TD2P_NUM_EXT_PORTS;
					for (i=0; i<8; i++) if (pivot[i] != -1) pivot[i]+=k;
				}
				else break;
			}

			break;
	}
	/* populate spacing information array (to program PGW registers) */
	for (j=0; j<32; j++)
	{
		w=33; v=33;
		if (ovs_tdm_tbl[j]!=_TD2P_NUM_EXT_PORTS) for (a=0; a<NUM_TSC; a++) for (b=0; b<4; b++) if (wc[a][b] == ovs_tdm_tbl[j]) c = a;
		if (j<31) {
			for (i=j+1; i<32; i++) {
				if (ovs_tdm_tbl[j]==ovs_tdm_tbl[i] ||
					ovs_tdm_tbl[i]==wc[c][0] ||
					ovs_tdm_tbl[i]==wc[c][1] ||
					ovs_tdm_tbl[i]==wc[c][2] ||
					ovs_tdm_tbl[i]==wc[c][3]) {
					w=i-j; 
					break;
				}
			}
		}
		if (j>0) {
			for (k=j-1; k>=0; k--) {
				if (ovs_tdm_tbl[j]==ovs_tdm_tbl[k] ||
					ovs_tdm_tbl[k]==wc[c][0] ||
					ovs_tdm_tbl[k]==wc[c][1] ||
					ovs_tdm_tbl[k]==wc[c][2] ||
					ovs_tdm_tbl[k]==wc[c][3]) {
					v=j-k; 
					break;
				}
			}
		}
		ovs_spacing[j] = ((w <= v) ? w : v);
	}
	for (k=0; k<32; k++) 
	{
		if (ovs_spacing[k] == 33) ovs_spacing[k] = 32;
		if (ovs_tdm_tbl[k] == _TD2P_NUM_EXT_PORTS) ovs_spacing[k] = 32;
	}
}


/**
@name: parse_mmu_tdm_tbl
@param: int, int, int[], int[], int[], int[], int[], int

Converts accessory tokens and prints debug form summary of pipe mmu table
 **/
void parse_mmu_tdm_tbl(int bw, int mgmt_bw, int mmu_tdm_tbl[256], int mmu_tdm_ovs_1[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_2[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_3[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_4[_TD2P_OS_GROUP_LEN], int pipe)
{
   int j, k=256, m=0;
   const char *name;
   int oversub=BOOL_FALSE;
   
   name = (pipe==0) ? "X-PIPE" : "Y-PIPE";
   
   for (j=0; j<16; j++) {if (mmu_tdm_ovs_1[j]!=_TD2P_NUM_EXT_PORTS || mmu_tdm_ovs_2[j]!=_TD2P_NUM_EXT_PORTS || mmu_tdm_ovs_3[j]!=_TD2P_NUM_EXT_PORTS || mmu_tdm_ovs_4[j]!=_TD2P_NUM_EXT_PORTS) {oversub=BOOL_TRUE;}}
     for (j=0; j<k; j++) {
	 if (mmu_tdm_tbl[j]!=ACC_TOKEN) {TDM_VERBOSE3("PIPE: %s, MMU TDM TABLE, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_tbl[j]);}
	 else {
		TDM_VERBOSE2("PIPE: %s, MMU TDM TABLE, element #%0d, CONTAINS ACCESSORY TOKEN - assigned as ", name, j);
		/* hierarchically determine how to assign the accessory token */
		m++;
		if (bw==760) {
			if (oversub==BOOL_FALSE) {
				if (pipe==0) {
					if (mgmt_bw==0) {
						switch (m) {
							case 1: case 3: case 5: case 7:
								CMIC(j)
							case 2: case 4: case 6: case 8:
								ANCL(j)
						}
					}
					else if (mgmt_bw==4) {
						switch (m) {
							case 1:
								MM13(j)
							case 3:
								MM14(j)
							case 5:
								MM15(j)
							case 7:
								MM16(j)
							case 2: case 6:
								ANCL(j)
							case 4: case 8:
								CMIC(j)
						}
					}
					else if (mgmt_bw==1) {
						switch (m)
						{
							case 2: case 4: case 6: case 8:
								MM13(j)
							case 1: case 5:
								ANCL(j)
							case 3: case 7:
								REFR(j)
						}
					}
				}
				else if (pipe==1) {
					switch (m) {
						case 1: case 3: case 5: case 7:
							LPBK(j)
						case 2: case 4: case 6: case 8:
							ANCL(j)
					}
				}
			}
			else if (oversub==BOOL_TRUE) {
				if (pipe==0) {
					if (mgmt_bw==0) {
						switch (m) {
							case 1: case 3: case 6:
								CMIC(j)
							case 2: case 4: case 5: case 7: case 8:
								ANCL(j)
						}
					}
					else if (mgmt_bw==4) {
						switch (m) {
							case 1: 
								MM13(j)
							case 3: 
								MM14(j)
							case 5: 
								MM15(j)
							case 7: 
								MM16(j)
							case 2: case 4: case 6:
								ANCL(j)
							case 8: 
								CMIC(j)
						}
					}
					else if (mgmt_bw==1) {
						TDM_VERBOSE0("UNSUPPORTED\n");
					}
				}
				else if (pipe==1) {
					switch (m)
					{
						case 1: case 3: case 6:
							LPBK(j)
						case 2: case 4: case 5: case 7: case 8:
							ANCL(j)
					}
				}
			}
		}
		else if (bw==608 || bw==609) {
			if (pipe==0) {
				if (mgmt_bw==0) {
					switch (m) {
						case 1: case 3: case 5: case 7: case 9:
							CMIC(j)
						case 2: case 4: case 6: case 8: case 10:
							ANCL(j)
					}
				}
				else if (mgmt_bw==4) {
					switch (m) {
						case 1: 
							MM13(j)
						case 3: 
							MM14(j) 
						case 6: 
							MM15(j)
						case 8: 
							MM16(j)
						case 2: case 5: case 9:
							ANCL(j)
						case 4: case 7: case 10:
							CMIC(j)
					}
				}
				else if (mgmt_bw==1) {
					switch (m) {
						case 2: case 4: case 7: case 9:
							MM13(j)
						case 1: case 3: case 5: case 6: case 8:
							ANCL(j)
						case 10: 
							CMIC(j)
					}
				}
			}
			else if (pipe==1) {
				switch (m) {
					case 1: case 3: case 5: case 7: case 9:
						LPBK(j)
					case 2: case 4: case 6: case 8: case 10:
						ANCL(j)
				}
			}
		}
		else if (bw==517 || bw==518) {
			if (oversub==BOOL_FALSE) {
				if (pipe==0) {
					if (mgmt_bw==0) {
						switch (m) {
							case 1: case 3: case 5: case 7:
								CMIC(j)
							case 2: case 4: case 6: case 8:
								ANCL(j)
						}
					}
					else if (mgmt_bw==4) {
						switch (m) {
							case 1: 
								MM13(j)
							case 3: 
								MM14(j)
							case 5: 
								MM15(j)
							case 7: 
								MM16(j)
							case 2: case 6:
								ANCL(j)
							case 4: case 8:
								CMIC(j)
						}
					}
					else if (mgmt_bw==1) {
						switch (m) {
							case 2: case 4: case 6: case 8:
								MM13(j)
							case 1: case 5:
								ANCL(j)
							case 3: case 7:
								REFR(j)
						}
					}
				}
				else if (pipe==1) {
					switch (m) {
						case 1: case 3: case 5: case 7:
							LPBK(j)
						case 2: case 4: case 6: case 8:
							ANCL(j)
					}
				}
			}
			else if (oversub==BOOL_TRUE) {
				if (pipe==0) {
					if (mgmt_bw==0) {
						switch (m) {
							case 1: case 3: case 6:
								CMIC(j)
							case 2: case 4: case 5: case 7: case 8:
								ANCL(j)
						}
					}
					else if (mgmt_bw==4) {
						switch (m) {
							case 1: 
								MM13(j)
							case 3: 
								MM14(j)
							case 5: 
								MM15(j)
							case 7: 
								MM16(j)
							case 2: case 4: case 6:
								ANCL(j)
							case 8: 
								CMIC(j)
						}
					}
					else if (mgmt_bw==1) {
						TDM_VERBOSE0("UNSUPPORTED\n");
					}
				}
				else if (pipe==1) {
					switch (m) {
						case 1: case 3: case 6:
							LPBK(j)
						case 2: case 4: case 5: case 7: case 8:
							ANCL(j)
					}
				}
			}
		}
		else if (bw==415 || bw==416) {
			if (pipe==0) {
				if (mgmt_bw==0) {
					switch (m) {
						case 1: case 3: case 5: case 7: case 9:
							CMIC(j)
						case 2: case 4: case 6: case 8: case 10:
							ANCL(j)
					}
				}
				else if (mgmt_bw==4) {
					switch (m) {
						case 1: 
							MM13(j)
						case 3: 
							MM14(j)
						case 6: 
							MM15(j)
						case 8: 
							MM16(j)
						case 2: case 5: case 9:
							ANCL(j)
						case 4: case 7: case 10:
							CMIC(j)
					}
				}
				else if (mgmt_bw==1) {
					switch (m) {
						case 2: case 4: case 7: case 9:
							MM13(j)
						case 1: case 3: case 5: case 6: case 8:
							ANCL(j)
						case 10: 
							CMIC(j)
					}
				}
			}
			else if (pipe==1) {
				switch (m) {
					case 1: case 3: case 5: case 7: case 9:
						LPBK(j)
					case 2: case 4: case 6: case 8: case 10:
						ANCL(j)
				}
			}
		}
		else {
			TDM_VERBOSE0("INVALID PARAMETERS\n");
		}
	 }
    }
	for (j=0; j<_TD2P_OS_GROUP_LEN; j++) {
		TDM_VERBOSE3("PIPE: %s, MMU OVS BUCKET 0, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_1[j]);
	}
	for (j=0; j<_TD2P_OS_GROUP_LEN; j++) {
		TDM_VERBOSE3("PIPE: %s, MMU OVS BUCKET 1, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_2[j]);
	}
	for (j=0; j<_TD2P_OS_GROUP_LEN; j++) {
		TDM_VERBOSE3("PIPE: %s, MMU OVS BUCKET 2, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_3[j]);
	}
	for (j=0; j<_TD2P_OS_GROUP_LEN; j++) {
		TDM_VERBOSE3("PIPE: %s, MMU OVS BUCKET 3, element #%0d, contains physical port #%0d\n", name, j, mmu_tdm_ovs_4[j]);
	}
}


/**
@name: TD2p_tdm_scheduler_wrap
@param: int[][], enum, int[], int[], int, int, int[], int, int

Wrapper for TDM populating function
**/
int TD2p_tdm_scheduler_wrap(int wc_array[NUM_TSC][4], enum port_speed speed[_TD2P_NUM_EXT_PORTS], int pgw_tdm_tbl[64], int ovs_tdm_tbl[32], int pgw_num, int bw, int port_state_map[_TD2P_NUM_EXT_PORTS], int iter2, int stop, int (*op_flags)[1])
{
	int first_wc = 0;		/* the principle TSC */
	int cur_idx;			/* the current TSC */
	int pgw_tdm_idx = 0;	/* pgw tdm index */
	int ovs_tdm_idx = 0;	/* ovs tdm index */
	int swap_array[32], i, j, z=0, scheduler_state, op_flags_str[1], timeout, z_cnt, z_iter;
	
	for (i = 0; i < 32; i++) swap_array[i] = 0;
	switch(pgw_num) 
	{
		case 0: first_wc = 0; break;
		case 1: first_wc = 8; break;
		case 2: first_wc = 16; break;
		case 3: first_wc = 24; break;
	}
	/* memcpy(&op_flags, op_flags_str, 1* sizeof(int) );*/
	/* for (j=0; j<1; j++) */op_flags_str[0]=(*op_flags)[0];
	for (j=0; j<4; j++) {
		cur_idx = first_wc;
		scheduler_state = TDM_pgw_scheduler(wc_array, speed, &cur_idx, &pgw_tdm_idx, &ovs_tdm_idx, j, pgw_tdm_tbl, ovs_tdm_tbl, first_wc+8, 100, bw, port_state_map, iter2, stop, swap_array, &z, j, op_flags_str);
	}
	/* for (j=0; j<1; j++) */(*op_flags)[0]=op_flags_str[0];
	timeout=64;
	while ( (z>0) && ((--timeout)>0) ) {
		for (i=0; i<pgw_tdm_idx; i++) {
			if (pgw_tdm_tbl[i]==swap_array[z]) {
				z++;
				swap_array[z]=pgw_tdm_tbl[i];
				for (j=i; j<31; j++) {
					pgw_tdm_tbl[j]=pgw_tdm_tbl[j+1];
				}
				pgw_tdm_idx--;
			}
		}
		z_cnt=1;
		for (i=(z-1); i>=0; i--) {
			if (swap_array[i]==swap_array[z]) {z_cnt++;}
		}
		z_iter=z_cnt;
		for (i=pgw_tdm_idx; i>0; i-=(pgw_tdm_idx/z_cnt)) {
			if (pgw_tdm_tbl[i]!=swap_array[z]) {
				for (j=31; j>i; j--) {
					pgw_tdm_tbl[j]=pgw_tdm_tbl[j-1];
				}
				pgw_tdm_tbl[i]=swap_array[z];
				if ((--z_iter)==0) {
					break;
				}
			}
		}
		z-=z_cnt;
	}
	if (z>0) {
		TDM_WARN2("TDM: _____WARNING: swap buffer not empty, index %0d, table at %0d, dumping all remaining entries\n", z, pgw_tdm_idx);
	}	
	if ((*op_flags)[0]==1) {
		for (i=0; i<(bw/40); i++) {
			if (pgw_tdm_tbl[i]==_TD2P_NUM_EXT_PORTS) {
				pgw_tdm_tbl[i]=OVSB_TOKEN;
			}
		}
	}
	TDM_VERBOSE0("TDM: ----------------------------------------------------------------\n");
	
	return scheduler_state;
}


/**
@name: TD2P_set_tdm_tbl
@param:

SV interfaced entry point for the 19 pre-configured TDM configurations
**/
int TD2P_set_tdm_tbl(td2p_tdm_globals_t *tdm_globals, td2p_tdm_pipes_t *tdm_pipe_tables)
{
	int wc_array[NUM_TSC][4];
	int mgmtbw=0;
	int i;
	int checkpoint[6];
	int op_flags_x[1];
	int op_flags_y[1];
	FILE *file;
	
	enum port_speed *speed;
	int tdm_bw;
	int *port_state_map;
	int *pm_encap_type;
	
	int *pgw_tdm_tbl_x0;
	int *ovs_tdm_tbl_x0;
	int *ovs_spacing_x0;
	int *pgw_tdm_tbl_x1;
	int *ovs_tdm_tbl_x1;
	int *ovs_spacing_x1;
	int *pgw_tdm_tbl_y0;
	int *ovs_tdm_tbl_y0;
	int *ovs_spacing_y0;
	int *pgw_tdm_tbl_y1;
	int *ovs_tdm_tbl_y1;
	int *ovs_spacing_y1;
	int *mmu_tdm_tbl_x;
	int *mmu_tdm_ovs_x_0;
	int *mmu_tdm_ovs_x_1;
	int *mmu_tdm_ovs_x_2;
	int *mmu_tdm_ovs_x_3;
	int *mmu_tdm_ovs_x_4;
	int *mmu_tdm_ovs_x_5;
	int *mmu_tdm_ovs_x_6;
	int *mmu_tdm_ovs_x_7;
	int *mmu_tdm_tbl_y;
	int *mmu_tdm_ovs_y_0;
	int *mmu_tdm_ovs_y_1;
	int *mmu_tdm_ovs_y_2;
	int *mmu_tdm_ovs_y_3;
	int *mmu_tdm_ovs_y_4;
	int *mmu_tdm_ovs_y_5;
	int *mmu_tdm_ovs_y_6;
	int *mmu_tdm_ovs_y_7;
	/* iarb_tdm_tbl_x, iarb_tdm_tbl_y is set but not used [-Werror=unused-but-set-variable]
	int *iarb_tdm_tbl_x;
	int *iarb_tdm_tbl_y;
	*/
	
	speed = tdm_globals->speed;
	tdm_bw = tdm_globals->clk_freq;
	port_state_map = tdm_globals->port_rates_array;
	pm_encap_type = tdm_globals->pm_encap_type;

	pgw_tdm_tbl_x0 = tdm_pipe_tables->pgw_tdm_tbl_x0;
	ovs_tdm_tbl_x0 = tdm_pipe_tables->ovs_tdm_tbl_x0;
	ovs_spacing_x0 = tdm_pipe_tables->ovs_spacing_x0;
	pgw_tdm_tbl_x1 = tdm_pipe_tables->pgw_tdm_tbl_x1;
	ovs_tdm_tbl_x1 = tdm_pipe_tables->ovs_tdm_tbl_x1;
	ovs_spacing_x1 = tdm_pipe_tables->ovs_spacing_x1;
	pgw_tdm_tbl_y0 = tdm_pipe_tables->pgw_tdm_tbl_y0;
	ovs_tdm_tbl_y0 = tdm_pipe_tables->ovs_tdm_tbl_y0;
	ovs_spacing_y0 = tdm_pipe_tables->ovs_spacing_y0;
	pgw_tdm_tbl_y1 = tdm_pipe_tables->pgw_tdm_tbl_y1;
	ovs_tdm_tbl_y1 = tdm_pipe_tables->ovs_tdm_tbl_y1;
	ovs_spacing_y1 = tdm_pipe_tables->ovs_spacing_y1;
	mmu_tdm_tbl_x = tdm_pipe_tables->mmu_tdm_tbl_x;
	mmu_tdm_ovs_x_0 = tdm_pipe_tables->mmu_tdm_ovs_x_0;
	mmu_tdm_ovs_x_1 = tdm_pipe_tables->mmu_tdm_ovs_x_1;
	mmu_tdm_ovs_x_2 = tdm_pipe_tables->mmu_tdm_ovs_x_2;
	mmu_tdm_ovs_x_3 = tdm_pipe_tables->mmu_tdm_ovs_x_3;
	mmu_tdm_ovs_x_4 = tdm_pipe_tables->mmu_tdm_ovs_x_4;
	mmu_tdm_ovs_x_5 = tdm_pipe_tables->mmu_tdm_ovs_x_5;
	mmu_tdm_ovs_x_6 = tdm_pipe_tables->mmu_tdm_ovs_x_6;
	mmu_tdm_ovs_x_7 = tdm_pipe_tables->mmu_tdm_ovs_x_7;
	mmu_tdm_tbl_y = tdm_pipe_tables->mmu_tdm_tbl_y;
	mmu_tdm_ovs_y_0 = tdm_pipe_tables->mmu_tdm_ovs_y_0;
	mmu_tdm_ovs_y_1 = tdm_pipe_tables->mmu_tdm_ovs_y_1;
	mmu_tdm_ovs_y_2 = tdm_pipe_tables->mmu_tdm_ovs_y_2;
	mmu_tdm_ovs_y_3 = tdm_pipe_tables->mmu_tdm_ovs_y_3;
	mmu_tdm_ovs_y_4 = tdm_pipe_tables->mmu_tdm_ovs_y_4;
	mmu_tdm_ovs_y_5 = tdm_pipe_tables->mmu_tdm_ovs_y_5;
	mmu_tdm_ovs_y_6 = tdm_pipe_tables->mmu_tdm_ovs_y_6;
	mmu_tdm_ovs_y_7 = tdm_pipe_tables->mmu_tdm_ovs_y_7;
	/* iarb_tdm_tbl_x, iarb_tdm_tbl_y is set but not used [-Werror=unused-but-set-variable]
	iarb_tdm_tbl_x = tdm_pipe_tables->iarb_tdm_tbl_x;
	iarb_tdm_tbl_y = tdm_pipe_tables->iarb_tdm_tbl_y;
	*/
	
	#ifdef _SET_TDM_DUMP
	remove(LOG);
	file = fopen(LOG,"a+");
	#else
	file = (FILE *) TDM_ALLOC((sizeof(int)*56),"FILE");
	#endif

	if (pgw_tdm_tbl_x0[0]==1234) {
		wc_array[3][0]=MGM_TOKEN;
	}
	for (i=0; i<256; i++) {
		mmu_tdm_tbl_x[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_tbl_y[i] = _TD2P_NUM_EXT_PORTS;
	}
	for (i=0; i<_TD2P_OS_GROUP_LEN; i++) {
		mmu_tdm_ovs_x_0[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_1[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_2[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_3[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_ovs_x_4[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_5[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_6[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_7[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_ovs_y_0[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_1[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_2[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_3[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_ovs_y_4[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_5[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_6[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_7[i] = _TD2P_NUM_EXT_PORTS;		
	}
	for (i=0; i<32; i++) {
		ovs_tdm_tbl_x0[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_x0[i] = _TD2P_NUM_EXT_PORTS;
		ovs_tdm_tbl_x1[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_x1[i] = _TD2P_NUM_EXT_PORTS;
		ovs_tdm_tbl_y0[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_y0[i] = _TD2P_NUM_EXT_PORTS;
		ovs_tdm_tbl_y1[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_y1[i] = _TD2P_NUM_EXT_PORTS;
	}
	for (i=0; i<64; i++) {
		pgw_tdm_tbl_x0[i] = _TD2P_NUM_EXT_PORTS;
		pgw_tdm_tbl_x1[i] = _TD2P_NUM_EXT_PORTS;
		pgw_tdm_tbl_y0[i] = _TD2P_NUM_EXT_PORTS;
		pgw_tdm_tbl_y1[i] = _TD2P_NUM_EXT_PORTS;
	}
	for (i=0; i<1; i++) {
		op_flags_x[i]=0;
		op_flags_y[i]=0;
	}
	
	TDM_VERBOSE1("TDM: _____VERBOSE: the speed for the CPU port is %0d\n",speed[0]);
	for (i=1; i<(_TD2P_NUM_EXT_PORTS-1); i++) {
		TDM_VERBOSE2("the speed for port %0d is %0d",i,speed[i]);
		TDM_PRINT1("\t| [%0d]\n",port_state_map[i-1]);
	}
	TDM_VERBOSE1("TDM: _____VERBOSE: the speed for the LOOPBACK port is %0d\n",speed[129]);
	
	tsc_port_transcription(wc_array, speed, port_state_map);
	print_tsc(file, wc_array);

	for (i=0; i<4; i++) {if (wc_array[3][i]==MGM_TOKEN) {mgmtbw++;}}

	TDM_VERBOSE1("TDM: _____VERBOSE: the chip frequency is %0d\n", tdm_bw);

	checkpoint[0] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_x0, ovs_tdm_tbl_x0, 0, tdm_bw, port_state_map, 0, 32, &op_flags_x);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_x0, "Pipe_x0_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_x0, "Pipe_x0_OVS");
	checkpoint[1] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_x1, ovs_tdm_tbl_x1, 1, tdm_bw, port_state_map, 32, 64, &op_flags_x);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_x1, "Pipe_x1_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_x1, "Pipe_x1_OVS");
	checkpoint[2] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_y0, ovs_tdm_tbl_y0, 2, tdm_bw, port_state_map, 64, 96, &op_flags_y);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_y0, "Pipe_y0_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_y0, "Pipe_y0_OVS");
	checkpoint[3] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_y1, ovs_tdm_tbl_y1, 3, tdm_bw, port_state_map, 96, 128, &op_flags_y);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_y1, "Pipe_y1_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_y1, "Pipe_y1_OVS");

	if (ovs_tdm_tbl_x0[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_x0, ovs_spacing_x0);}
	if (ovs_tdm_tbl_x1[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_x1, ovs_spacing_x1);}
	if (ovs_tdm_tbl_y0[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_y0, ovs_spacing_y0);}
	if (ovs_tdm_tbl_y1[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_y1, ovs_spacing_y1);}

	TD2P_print_tbl_summary(file, pgw_tdm_tbl_x0, pgw_tdm_tbl_x1, pgw_tdm_tbl_y0, pgw_tdm_tbl_y1,
			ovs_tdm_tbl_x0, ovs_tdm_tbl_x1, ovs_tdm_tbl_y0, ovs_tdm_tbl_y1,
			ovs_spacing_x0, ovs_spacing_x1, ovs_spacing_y0, ovs_spacing_y1,
			tdm_bw);	

	checkpoint[4] = write_mmu_tdm_tbl(file, pgw_tdm_tbl_x0, pgw_tdm_tbl_x1, ovs_tdm_tbl_x0, ovs_tdm_tbl_x1, mmu_tdm_tbl_x, mmu_tdm_ovs_x_0, mmu_tdm_ovs_x_1, mmu_tdm_ovs_x_2, mmu_tdm_ovs_x_3, BOOL_TRUE, port_state_map, wc_array, tdm_bw, speed, &op_flags_x, pm_encap_type);
	parse_mmu_tdm_tbl(tdm_bw, mgmtbw, mmu_tdm_tbl_x, mmu_tdm_ovs_x_0, mmu_tdm_ovs_x_1, mmu_tdm_ovs_x_2, mmu_tdm_ovs_x_3, 0);
	checkpoint[5] = write_mmu_tdm_tbl(file, pgw_tdm_tbl_y0, pgw_tdm_tbl_y1, ovs_tdm_tbl_y0, ovs_tdm_tbl_y1, mmu_tdm_tbl_y, mmu_tdm_ovs_y_0, mmu_tdm_ovs_y_1, mmu_tdm_ovs_y_2, mmu_tdm_ovs_y_3, BOOL_FALSE, port_state_map, wc_array, tdm_bw, speed, &op_flags_y, pm_encap_type);
	parse_mmu_tdm_tbl(tdm_bw, mgmtbw, mmu_tdm_tbl_y, mmu_tdm_ovs_y_0, mmu_tdm_ovs_y_1, mmu_tdm_ovs_y_2, mmu_tdm_ovs_y_3, 1);

	if (checkpoint[0]==0||checkpoint[1]==0||checkpoint[2]==0||checkpoint[3]==0||checkpoint[4]==0||checkpoint[5]==0) {
		if (checkpoint[0]==0) TDM_ERROR0("TDM: _____ERROR: quadrant x0 failed to schedule\n");
		if (checkpoint[1]==0) TDM_ERROR0("TDM: _____ERROR: quadrant x1 failed to schedule\n");
		if (checkpoint[2]==0) TDM_ERROR0("TDM: _____ERROR: quadrant y0 failed to schedule\n");
		if (checkpoint[3]==0) TDM_ERROR0("TDM: _____ERROR: quadrant y1 failed to schedule\n");
		if (checkpoint[4]==0) TDM_ERROR0("TDM: _____ERROR: mmu x pipe table failed to schedule\n");
		if (checkpoint[5]==0) TDM_ERROR0("TDM: _____ERROR: mmu y pipe table failed to schedule\n");
		return 0;
	}
	else return 1;
	
}


/**
@name: set_tdm_tbl_tdm19
@param:

SV interfaced entry point for hard coded TDM 19
**/
int set_tdm_tbl_tdm19(td2p_tdm_globals_t *tdm_globals, td2p_tdm_pipes_t *tdm_pipe_tables)
{
	int wc_array[NUM_TSC][4];
	int mgmtbw=0;
	int i, j, l, g, accessories=8, cap=192;
	int checkpoint[6];
	int op_flags_x[1];
	int op_flags_y[1];
	FILE *file;
	
	enum port_speed *speed;
	int tdm_bw;
	int *port_state_map;
	int *pm_encap_type;
	
	int *pgw_tdm_tbl_x0;
	int *ovs_tdm_tbl_x0;
	int *ovs_spacing_x0;
	int *pgw_tdm_tbl_x1;
	int *ovs_tdm_tbl_x1;
	int *ovs_spacing_x1;
	int *pgw_tdm_tbl_y0;
	int *ovs_tdm_tbl_y0;
	int *ovs_spacing_y0;
	int *pgw_tdm_tbl_y1;
	int *ovs_tdm_tbl_y1;
	int *ovs_spacing_y1;
	int *mmu_tdm_tbl_x;
	int *mmu_tdm_ovs_x_0;
	int *mmu_tdm_ovs_x_1;
	int *mmu_tdm_ovs_x_2;
	int *mmu_tdm_ovs_x_3;
	int *mmu_tdm_ovs_x_4;
	int *mmu_tdm_ovs_x_5;
	int *mmu_tdm_ovs_x_6;
	int *mmu_tdm_ovs_x_7;
	int *mmu_tdm_tbl_y;
	int *mmu_tdm_ovs_y_0;
	int *mmu_tdm_ovs_y_1;
	int *mmu_tdm_ovs_y_2;
	int *mmu_tdm_ovs_y_3;
	int *mmu_tdm_ovs_y_4;
	int *mmu_tdm_ovs_y_5;
	int *mmu_tdm_ovs_y_6;
	int *mmu_tdm_ovs_y_7;
	/* iarb_tdm_tbl_x, iarb_tdm_tbl_y is set but not used [-Werror=unused-but-set-variable]
	int *iarb_tdm_tbl_x;
	int *iarb_tdm_tbl_y;
	*/
	
	speed = tdm_globals->speed;
	tdm_bw = tdm_globals->clk_freq;
	port_state_map = tdm_globals->port_rates_array;
	pm_encap_type = tdm_globals->pm_encap_type;

	pgw_tdm_tbl_x0 = tdm_pipe_tables->pgw_tdm_tbl_x0;
	ovs_tdm_tbl_x0 = tdm_pipe_tables->ovs_tdm_tbl_x0;
	ovs_spacing_x0 = tdm_pipe_tables->ovs_spacing_x0;
	pgw_tdm_tbl_x1 = tdm_pipe_tables->pgw_tdm_tbl_x1;
	ovs_tdm_tbl_x1 = tdm_pipe_tables->ovs_tdm_tbl_x1;
	ovs_spacing_x1 = tdm_pipe_tables->ovs_spacing_x1;
	pgw_tdm_tbl_y0 = tdm_pipe_tables->pgw_tdm_tbl_y0;
	ovs_tdm_tbl_y0 = tdm_pipe_tables->ovs_tdm_tbl_y0;
	ovs_spacing_y0 = tdm_pipe_tables->ovs_spacing_y0;
	pgw_tdm_tbl_y1 = tdm_pipe_tables->pgw_tdm_tbl_y1;
	ovs_tdm_tbl_y1 = tdm_pipe_tables->ovs_tdm_tbl_y1;
	ovs_spacing_y1 = tdm_pipe_tables->ovs_spacing_y1;
	mmu_tdm_tbl_x = tdm_pipe_tables->mmu_tdm_tbl_x;
	mmu_tdm_ovs_x_0 = tdm_pipe_tables->mmu_tdm_ovs_x_0;
	mmu_tdm_ovs_x_1 = tdm_pipe_tables->mmu_tdm_ovs_x_1;
	mmu_tdm_ovs_x_2 = tdm_pipe_tables->mmu_tdm_ovs_x_2;
	mmu_tdm_ovs_x_3 = tdm_pipe_tables->mmu_tdm_ovs_x_3;
	mmu_tdm_ovs_x_4 = tdm_pipe_tables->mmu_tdm_ovs_x_4;
	mmu_tdm_ovs_x_5 = tdm_pipe_tables->mmu_tdm_ovs_x_5;
	mmu_tdm_ovs_x_6 = tdm_pipe_tables->mmu_tdm_ovs_x_6;
	mmu_tdm_ovs_x_7 = tdm_pipe_tables->mmu_tdm_ovs_x_7;
	mmu_tdm_tbl_y = tdm_pipe_tables->mmu_tdm_tbl_y;
	mmu_tdm_ovs_y_0 = tdm_pipe_tables->mmu_tdm_ovs_y_0;
	mmu_tdm_ovs_y_1 = tdm_pipe_tables->mmu_tdm_ovs_y_1;
	mmu_tdm_ovs_y_2 = tdm_pipe_tables->mmu_tdm_ovs_y_2;
	mmu_tdm_ovs_y_3 = tdm_pipe_tables->mmu_tdm_ovs_y_3;
	mmu_tdm_ovs_y_4 = tdm_pipe_tables->mmu_tdm_ovs_y_4;
	mmu_tdm_ovs_y_5 = tdm_pipe_tables->mmu_tdm_ovs_y_5;
	mmu_tdm_ovs_y_6 = tdm_pipe_tables->mmu_tdm_ovs_y_6;
	mmu_tdm_ovs_y_7 = tdm_pipe_tables->mmu_tdm_ovs_y_7;
	/* iarb_tdm_tbl_x, iarb_tdm_tbl_y is set but not used [-Werror=unused-but-set-variable]
	iarb_tdm_tbl_x = tdm_pipe_tables->iarb_tdm_tbl_x;
	iarb_tdm_tbl_y = tdm_pipe_tables->iarb_tdm_tbl_y;
	*/
	
	#ifdef _SET_TDM_DUMP
	remove(LOG);
	file = fopen(LOG,"a+");
	#else
	file = (FILE *) TDM_ALLOC((sizeof(int)*56),"FILE");
	#endif

	if (pgw_tdm_tbl_x0[0]==1234) {
		wc_array[3][0]=MGM_TOKEN;
	}
	for (i=0; i<256; i++) {
		mmu_tdm_tbl_x[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_tbl_y[i] = _TD2P_NUM_EXT_PORTS;
	}
	for (i=0; i<_TD2P_OS_GROUP_LEN; i++) {
		mmu_tdm_ovs_x_0[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_1[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_2[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_3[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_ovs_x_4[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_5[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_6[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_7[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_ovs_y_0[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_1[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_2[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_3[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_ovs_y_4[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_5[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_6[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_7[i] = _TD2P_NUM_EXT_PORTS;		
	}
	for (i=0; i<32; i++) {
		ovs_tdm_tbl_x0[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_x0[i] = _TD2P_NUM_EXT_PORTS;
		ovs_tdm_tbl_x1[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_x1[i] = _TD2P_NUM_EXT_PORTS;
		ovs_tdm_tbl_y0[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_y0[i] = _TD2P_NUM_EXT_PORTS;
		ovs_tdm_tbl_y1[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_y1[i] = _TD2P_NUM_EXT_PORTS;
	}
	for (i=0; i<64; i++) {
		pgw_tdm_tbl_x0[i] = _TD2P_NUM_EXT_PORTS;
		pgw_tdm_tbl_x1[i] = _TD2P_NUM_EXT_PORTS;
		pgw_tdm_tbl_y0[i] = _TD2P_NUM_EXT_PORTS;
		pgw_tdm_tbl_y1[i] = _TD2P_NUM_EXT_PORTS;
	}
	for (i=0; i<1; i++) {
		op_flags_x[i]=0;
		op_flags_y[i]=0;
	}
	
	TDM_VERBOSE1("TDM: _____VERBOSE: the speed for the CPU port is %0d\n",speed[0]);
	for (i=1; i<(_TD2P_NUM_EXT_PORTS-1); i++) {
		TDM_VERBOSE2("the speed for port %0d is %0d",i,speed[i]);
		TDM_PRINT1("\t| [%0d]\n",port_state_map[i-1]);
	}
	TDM_VERBOSE1("TDM: _____VERBOSE: the speed for the LOOPBACK port is %0d\n",speed[129]);
	
	tsc_port_transcription(wc_array, speed, port_state_map);
	print_tsc(file, wc_array);

	for (i=0; i<4; i++) {if (wc_array[3][i]==MGM_TOKEN) {mgmtbw++;}}

	TDM_VERBOSE1("TDM: _____VERBOSE: the chip frequency is %0d\n", tdm_bw);

	checkpoint[0] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_x0, ovs_tdm_tbl_x0, 0, tdm_bw, port_state_map, 0, 32, &op_flags_x);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_x0, "Pipe_x0_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_x0, "Pipe_x0_OVS");
	checkpoint[1] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_x1, ovs_tdm_tbl_x1, 1, tdm_bw, port_state_map, 32, 64, &op_flags_x);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_x1, "Pipe_x1_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_x1, "Pipe_x1_OVS");
	checkpoint[2] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_y0, ovs_tdm_tbl_y0, 2, tdm_bw, port_state_map, 64, 96, &op_flags_y);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_y0, "Pipe_y0_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_y0, "Pipe_y0_OVS");
	checkpoint[3] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_y1, ovs_tdm_tbl_y1, 3, tdm_bw, port_state_map, 96, 128, &op_flags_y);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_y1, "Pipe_y1_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_y1, "Pipe_y1_OVS");

	if (ovs_tdm_tbl_x0[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_x0, ovs_spacing_x0);}
	if (ovs_tdm_tbl_x1[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_x1, ovs_spacing_x1);}
	if (ovs_tdm_tbl_y0[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_y0, ovs_spacing_y0);}
	if (ovs_tdm_tbl_y1[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_y1, ovs_spacing_y1);}

	TD2P_print_tbl_summary(file, pgw_tdm_tbl_x0, pgw_tdm_tbl_x1, pgw_tdm_tbl_y0, pgw_tdm_tbl_y1,
			ovs_tdm_tbl_x0, ovs_tdm_tbl_x1, ovs_tdm_tbl_y0, ovs_tdm_tbl_y1,
			ovs_spacing_x0, ovs_spacing_x1, ovs_spacing_y0, ovs_spacing_y1,
			tdm_bw);
			
	checkpoint[4] = write_mmu_tdm_tbl(file, pgw_tdm_tbl_x0, pgw_tdm_tbl_x1, ovs_tdm_tbl_x0, ovs_tdm_tbl_x1, mmu_tdm_tbl_x, mmu_tdm_ovs_x_0, mmu_tdm_ovs_x_1, mmu_tdm_ovs_x_2, mmu_tdm_ovs_x_3, BOOL_TRUE, port_state_map, wc_array, tdm_bw, speed, &op_flags_x, pm_encap_type);
	parse_mmu_tdm_tbl(tdm_bw, mgmtbw, mmu_tdm_tbl_x, mmu_tdm_ovs_x_0, mmu_tdm_ovs_x_1, mmu_tdm_ovs_x_2, mmu_tdm_ovs_x_3, 0);
	/*
	checkpoint[5] = write_mmu_tdm_tbl(file, pgw_tdm_tbl_y0, pgw_tdm_tbl_y1, ovs_tdm_tbl_y0, ovs_tdm_tbl_y1, mmu_tdm_tbl_y, mmu_tdm_ovs_y_0, mmu_tdm_ovs_y_1, mmu_tdm_ovs_y_2, mmu_tdm_ovs_y_3, BOOL_FALSE, port_state_map, wc_array, tdm_bw, speed, &op_flags_y, pm_encap_type);
	*/
	/* Hard code Y pipe egress TDM here */
	checkpoint[5]=1;
	mmu_tdm_ovs_y_0[0]=125;
	mmu_tdm_ovs_y_0[1]=121;
	mmu_tdm_ovs_y_0[2]=117;
	mmu_tdm_ovs_y_0[3]=113;
	mmu_tdm_ovs_y_0[4]=93;
	mmu_tdm_ovs_y_0[5]=89;
	mmu_tdm_ovs_y_0[6]=85;
	mmu_tdm_ovs_y_0[7]=81;
	for (i=0; i<192; i+=12) {
		mmu_tdm_tbl_y[i]=65;
		mmu_tdm_tbl_y[i+1]=69;
		mmu_tdm_tbl_y[i+3]=73;
		mmu_tdm_tbl_y[i+4]=77;
		mmu_tdm_tbl_y[i+6]=97;
		mmu_tdm_tbl_y[i+7]=101;
		mmu_tdm_tbl_y[i+9]=105;
		mmu_tdm_tbl_y[i+10]=109;
	}
	for (i=2; i<192; i+=3) {
		mmu_tdm_tbl_y[i]=OVSB_TOKEN;
	}
	for (j=1; j<=accessories; j++) {
		g=tdm_PQ((((10*cap)/accessories)*j))+(j-1);
		for (l=255; l>g; l--) {
			mmu_tdm_tbl_y[l]=mmu_tdm_tbl_y[l-1];
		}
		mmu_tdm_tbl_y[g]=ACC_TOKEN;
	}
	parse_mmu_tdm_tbl(tdm_bw, mgmtbw, mmu_tdm_tbl_y, mmu_tdm_ovs_y_0, mmu_tdm_ovs_y_1, mmu_tdm_ovs_y_2, mmu_tdm_ovs_y_3, 1);

	if (checkpoint[0]==0||checkpoint[1]==0||checkpoint[2]==0||checkpoint[3]==0||checkpoint[4]==0||checkpoint[5]==0) {
		if (checkpoint[0]==0) TDM_ERROR0("TDM: _____ERROR: quadrant x0 failed to schedule\n");
		if (checkpoint[1]==0) TDM_ERROR0("TDM: _____ERROR: quadrant x1 failed to schedule\n");
		if (checkpoint[2]==0) TDM_ERROR0("TDM: _____ERROR: quadrant y0 failed to schedule\n");
		if (checkpoint[3]==0) TDM_ERROR0("TDM: _____ERROR: quadrant y1 failed to schedule\n");
		if (checkpoint[4]==0) TDM_ERROR0("TDM: _____ERROR: mmu x pipe table failed to schedule\n");
		if (checkpoint[5]==0) TDM_ERROR0("TDM: _____ERROR: mmu y pipe table failed to schedule\n");
		return 0;
	}
	else return 1;
	
}


/**
@name: set_tdm_tbl_tdm13
@param:

SV interfaced entry point for hard coded TDM 13
**/
int set_tdm_tbl_tdm13(td2p_tdm_globals_t *tdm_globals, td2p_tdm_pipes_t *tdm_pipe_tables)
{
	int wc_array[NUM_TSC][4];
	int mgmtbw=0;
	int i, j, l, g, accessories=8, cap=192;
	int checkpoint[6];
	int op_flags_x[1];
	int op_flags_y[1];
	FILE *file;
	
	enum port_speed *speed;
	int tdm_bw;
	int *port_state_map;
	int *pm_encap_type;
	
	int *pgw_tdm_tbl_x0;
	int *ovs_tdm_tbl_x0;
	int *ovs_spacing_x0;
	int *pgw_tdm_tbl_x1;
	int *ovs_tdm_tbl_x1;
	int *ovs_spacing_x1;
	int *pgw_tdm_tbl_y0;
	int *ovs_tdm_tbl_y0;
	int *ovs_spacing_y0;
	int *pgw_tdm_tbl_y1;
	int *ovs_tdm_tbl_y1;
	int *ovs_spacing_y1;
	int *mmu_tdm_tbl_x;
	int *mmu_tdm_ovs_x_0;
	int *mmu_tdm_ovs_x_1;
	int *mmu_tdm_ovs_x_2;
	int *mmu_tdm_ovs_x_3;
	int *mmu_tdm_ovs_x_4;
	int *mmu_tdm_ovs_x_5;
	int *mmu_tdm_ovs_x_6;
	int *mmu_tdm_ovs_x_7;
	int *mmu_tdm_tbl_y;
	int *mmu_tdm_ovs_y_0;
	int *mmu_tdm_ovs_y_1;
	int *mmu_tdm_ovs_y_2;
	int *mmu_tdm_ovs_y_3;
	int *mmu_tdm_ovs_y_4;
	int *mmu_tdm_ovs_y_5;
	int *mmu_tdm_ovs_y_6;
	int *mmu_tdm_ovs_y_7;
	/* iarb_tdm_tbl_x, iarb_tdm_tbl_y is set but not used [-Werror=unused-but-set-variable]
	int *iarb_tdm_tbl_x;
	int *iarb_tdm_tbl_y;
	*/
	
	speed = tdm_globals->speed;
	tdm_bw = tdm_globals->clk_freq;
	port_state_map = tdm_globals->port_rates_array;
	pm_encap_type = tdm_globals->pm_encap_type;

	pgw_tdm_tbl_x0 = tdm_pipe_tables->pgw_tdm_tbl_x0;
	ovs_tdm_tbl_x0 = tdm_pipe_tables->ovs_tdm_tbl_x0;
	ovs_spacing_x0 = tdm_pipe_tables->ovs_spacing_x0;
	pgw_tdm_tbl_x1 = tdm_pipe_tables->pgw_tdm_tbl_x1;
	ovs_tdm_tbl_x1 = tdm_pipe_tables->ovs_tdm_tbl_x1;
	ovs_spacing_x1 = tdm_pipe_tables->ovs_spacing_x1;
	pgw_tdm_tbl_y0 = tdm_pipe_tables->pgw_tdm_tbl_y0;
	ovs_tdm_tbl_y0 = tdm_pipe_tables->ovs_tdm_tbl_y0;
	ovs_spacing_y0 = tdm_pipe_tables->ovs_spacing_y0;
	pgw_tdm_tbl_y1 = tdm_pipe_tables->pgw_tdm_tbl_y1;
	ovs_tdm_tbl_y1 = tdm_pipe_tables->ovs_tdm_tbl_y1;
	ovs_spacing_y1 = tdm_pipe_tables->ovs_spacing_y1;
	mmu_tdm_tbl_x = tdm_pipe_tables->mmu_tdm_tbl_x;
	mmu_tdm_ovs_x_0 = tdm_pipe_tables->mmu_tdm_ovs_x_0;
	mmu_tdm_ovs_x_1 = tdm_pipe_tables->mmu_tdm_ovs_x_1;
	mmu_tdm_ovs_x_2 = tdm_pipe_tables->mmu_tdm_ovs_x_2;
	mmu_tdm_ovs_x_3 = tdm_pipe_tables->mmu_tdm_ovs_x_3;
	mmu_tdm_ovs_x_4 = tdm_pipe_tables->mmu_tdm_ovs_x_4;
	mmu_tdm_ovs_x_5 = tdm_pipe_tables->mmu_tdm_ovs_x_5;
	mmu_tdm_ovs_x_6 = tdm_pipe_tables->mmu_tdm_ovs_x_6;
	mmu_tdm_ovs_x_7 = tdm_pipe_tables->mmu_tdm_ovs_x_7;
	mmu_tdm_tbl_y = tdm_pipe_tables->mmu_tdm_tbl_y;
	mmu_tdm_ovs_y_0 = tdm_pipe_tables->mmu_tdm_ovs_y_0;
	mmu_tdm_ovs_y_1 = tdm_pipe_tables->mmu_tdm_ovs_y_1;
	mmu_tdm_ovs_y_2 = tdm_pipe_tables->mmu_tdm_ovs_y_2;
	mmu_tdm_ovs_y_3 = tdm_pipe_tables->mmu_tdm_ovs_y_3;
	mmu_tdm_ovs_y_4 = tdm_pipe_tables->mmu_tdm_ovs_y_4;
	mmu_tdm_ovs_y_5 = tdm_pipe_tables->mmu_tdm_ovs_y_5;
	mmu_tdm_ovs_y_6 = tdm_pipe_tables->mmu_tdm_ovs_y_6;
	mmu_tdm_ovs_y_7 = tdm_pipe_tables->mmu_tdm_ovs_y_7;
	/* iarb_tdm_tbl_x, iarb_tdm_tbl_y is set but not used [-Werror=unused-but-set-variable]
	iarb_tdm_tbl_x = tdm_pipe_tables->iarb_tdm_tbl_x;
	iarb_tdm_tbl_y = tdm_pipe_tables->iarb_tdm_tbl_y;
	*/
	
	#ifdef _SET_TDM_DUMP
	remove(LOG);
	file = fopen(LOG,"a+");
	#else
	file = (FILE *) TDM_ALLOC((sizeof(int)*56),"FILE");
	#endif

	if (pgw_tdm_tbl_x0[0]==1234) {
		wc_array[3][0]=MGM_TOKEN;
	}
	for (i=0; i<256; i++) {
		mmu_tdm_tbl_x[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_tbl_y[i] = _TD2P_NUM_EXT_PORTS;
	}
	for (i=0; i<_TD2P_OS_GROUP_LEN; i++) {
		mmu_tdm_ovs_x_0[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_1[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_2[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_3[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_ovs_x_4[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_5[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_6[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_x_7[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_ovs_y_0[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_1[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_2[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_3[i] = _TD2P_NUM_EXT_PORTS;
		mmu_tdm_ovs_y_4[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_5[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_6[i] = _TD2P_NUM_EXT_PORTS; mmu_tdm_ovs_y_7[i] = _TD2P_NUM_EXT_PORTS;		
	}
	for (i=0; i<32; i++) {
		ovs_tdm_tbl_x0[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_x0[i] = _TD2P_NUM_EXT_PORTS;
		ovs_tdm_tbl_x1[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_x1[i] = _TD2P_NUM_EXT_PORTS;
		ovs_tdm_tbl_y0[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_y0[i] = _TD2P_NUM_EXT_PORTS;
		ovs_tdm_tbl_y1[i] = _TD2P_NUM_EXT_PORTS; ovs_spacing_y1[i] = _TD2P_NUM_EXT_PORTS;
	}
	for (i=0; i<64; i++) {
		pgw_tdm_tbl_x0[i] = _TD2P_NUM_EXT_PORTS;
		pgw_tdm_tbl_x1[i] = _TD2P_NUM_EXT_PORTS;
		pgw_tdm_tbl_y0[i] = _TD2P_NUM_EXT_PORTS;
		pgw_tdm_tbl_y1[i] = _TD2P_NUM_EXT_PORTS;
	}
	for (i=0; i<1; i++) {
		op_flags_x[i]=0;
		op_flags_y[i]=0;
	}
	
	TDM_VERBOSE1("TDM: _____VERBOSE: the speed for the CPU port is %0d\n",speed[0]);
	for (i=1; i<(_TD2P_NUM_EXT_PORTS-1); i++) {
		TDM_VERBOSE2("the speed for port %0d is %0d",i,speed[i]);
		TDM_PRINT1("\t| [%0d]\n",port_state_map[i-1]);
	}
	TDM_VERBOSE1("TDM: _____VERBOSE: the speed for the LOOPBACK port is %0d\n",speed[129]);
	
	tsc_port_transcription(wc_array, speed, port_state_map);
	print_tsc(file, wc_array);

	for (i=0; i<4; i++) {if (wc_array[3][i]==MGM_TOKEN) {mgmtbw++;}}

	TDM_VERBOSE1("TDM: _____VERBOSE: the chip frequency is %0d\n", tdm_bw);

	checkpoint[0] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_x0, ovs_tdm_tbl_x0, 0, tdm_bw, port_state_map, 0, 32, &op_flags_x);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_x0, "Pipe_x0_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_x0, "Pipe_x0_OVS");
	checkpoint[1] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_x1, ovs_tdm_tbl_x1, 1, tdm_bw, port_state_map, 32, 64, &op_flags_x);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_x1, "Pipe_x1_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_x1, "Pipe_x1_OVS");
	checkpoint[2] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_y0, ovs_tdm_tbl_y0, 2, tdm_bw, port_state_map, 64, 96, &op_flags_y);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_y0, "Pipe_y0_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_y0, "Pipe_y0_OVS");
	checkpoint[3] = TD2p_tdm_scheduler_wrap(wc_array, speed, pgw_tdm_tbl_y1, ovs_tdm_tbl_y1, 3, tdm_bw, port_state_map, 96, 128, &op_flags_y);
	TD2P_print_tdm_tbl(file, pgw_tdm_tbl_y1, "Pipe_y1_PGW");
	TD2P_print_tdm_tbl(file, ovs_tdm_tbl_y1, "Pipe_y1_OVS");

	if (ovs_tdm_tbl_x0[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_x0, ovs_spacing_x0);}
	if (ovs_tdm_tbl_x1[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_x1, ovs_spacing_x1);}
	if (ovs_tdm_tbl_y0[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_y0, ovs_spacing_y0);}
	if (ovs_tdm_tbl_y1[0] != _TD2P_NUM_EXT_PORTS) {TD2p_tdm_ovs_spacer(wc_array, ovs_tdm_tbl_y1, ovs_spacing_y1);}

	TD2P_print_tbl_summary(file, pgw_tdm_tbl_x0, pgw_tdm_tbl_x1, pgw_tdm_tbl_y0, pgw_tdm_tbl_y1,
			ovs_tdm_tbl_x0, ovs_tdm_tbl_x1, ovs_tdm_tbl_y0, ovs_tdm_tbl_y1,
			ovs_spacing_x0, ovs_spacing_x1, ovs_spacing_y0, ovs_spacing_y1,
			tdm_bw);
			
	checkpoint[4] = write_mmu_tdm_tbl(file, pgw_tdm_tbl_x0, pgw_tdm_tbl_x1, ovs_tdm_tbl_x0, ovs_tdm_tbl_x1, mmu_tdm_tbl_x, mmu_tdm_ovs_x_0, mmu_tdm_ovs_x_1, mmu_tdm_ovs_x_2, mmu_tdm_ovs_x_3, BOOL_TRUE, port_state_map, wc_array, tdm_bw, speed, &op_flags_x, pm_encap_type);
	parse_mmu_tdm_tbl(tdm_bw, mgmtbw, mmu_tdm_tbl_x, mmu_tdm_ovs_x_0, mmu_tdm_ovs_x_1, mmu_tdm_ovs_x_2, mmu_tdm_ovs_x_3, 0);
	/*
	checkpoint[5] = write_mmu_tdm_tbl(file, pgw_tdm_tbl_y0, pgw_tdm_tbl_y1, ovs_tdm_tbl_y0, ovs_tdm_tbl_y1, mmu_tdm_tbl_y, mmu_tdm_ovs_y_0, mmu_tdm_ovs_y_1, mmu_tdm_ovs_y_2, mmu_tdm_ovs_y_3, BOOL_FALSE, port_state_map, wc_array, tdm_bw, speed, &op_flags_y, pm_encap_type);
	*/
	/* Hard code Y pipe egress TDM here */
	checkpoint[5]=1;
	mmu_tdm_ovs_y_0[0]=125;
	mmu_tdm_ovs_y_0[1]=121;
	mmu_tdm_ovs_y_0[2]=117;
	mmu_tdm_ovs_y_0[3]=109;
	mmu_tdm_ovs_y_0[4]=81;
	mmu_tdm_ovs_y_0[5]=73;
	mmu_tdm_ovs_y_0[6]=69;
	mmu_tdm_ovs_y_0[7]=65;
	for (i=0; i<192; i+=12) {
		mmu_tdm_tbl_y[i]=85;
		mmu_tdm_tbl_y[i+1]=89;
		mmu_tdm_tbl_y[i+3]=93;
		mmu_tdm_tbl_y[i+4]=97;
		mmu_tdm_tbl_y[i+6]=101;
		mmu_tdm_tbl_y[i+7]=105;
	}
	for (i=9; i<192; i+=48) {mmu_tdm_tbl_y[i]=77;}
	for (i=10; i<192; i+=48) {mmu_tdm_tbl_y[i]=113;}
	for (i=21; i<192; i+=48) {mmu_tdm_tbl_y[i]=78;}
	for (i=22; i<192; i+=48) {mmu_tdm_tbl_y[i]=114;}
	for (i=33; i<192; i+=48) {mmu_tdm_tbl_y[i]=79;}
	for (i=34; i<192; i+=48) {mmu_tdm_tbl_y[i]=115;}
	for (i=45; i<192; i+=48) {mmu_tdm_tbl_y[i]=80;}
	for (i=46; i<192; i+=48) {mmu_tdm_tbl_y[i]=116;}
	for (i=2; i<192; i+=3) {
		mmu_tdm_tbl_y[i]=OVSB_TOKEN;
	}
	for (j=1; j<=accessories; j++) {
		g=tdm_PQ((((10*cap)/accessories)*j))+(j-1);
		for (l=255; l>g; l--) {
			mmu_tdm_tbl_y[l]=mmu_tdm_tbl_y[l-1];
		}
		mmu_tdm_tbl_y[g]=ACC_TOKEN;
	}
	parse_mmu_tdm_tbl(tdm_bw, mgmtbw, mmu_tdm_tbl_y, mmu_tdm_ovs_y_0, mmu_tdm_ovs_y_1, mmu_tdm_ovs_y_2, mmu_tdm_ovs_y_3, 1);	

	if (checkpoint[0]==0||checkpoint[1]==0||checkpoint[2]==0||checkpoint[3]==0||checkpoint[4]==0||checkpoint[5]==0) {
		if (checkpoint[0]==0) TDM_ERROR0("TDM: _____ERROR: quadrant x0 failed to schedule\n");
		if (checkpoint[1]==0) TDM_ERROR0("TDM: _____ERROR: quadrant x1 failed to schedule\n");
		if (checkpoint[2]==0) TDM_ERROR0("TDM: _____ERROR: quadrant y0 failed to schedule\n");
		if (checkpoint[3]==0) TDM_ERROR0("TDM: _____ERROR: quadrant y1 failed to schedule\n");
		if (checkpoint[4]==0) TDM_ERROR0("TDM: _____ERROR: mmu x pipe table failed to schedule\n");
		if (checkpoint[5]==0) TDM_ERROR0("TDM: _____ERROR: mmu y pipe table failed to schedule\n");
		return 0;
	}
	else return 1;
	
}


/**
@name: init_iarb_tdm_ovs_table
@param: int, int, int, int, int*, int*, int[512], int[512]

IARB TDM Oversubscription Schedule:
  Input  1  => Core bandwidth - 960/720/640/480.
  Input  2  => 1 if there are 4x1G management ports, 0 otherwise.
  Input  3  => 1 if there are 4x2.5G management ports, 0 otherwise.
  Input  4  => 1 if there are 1x10G management port, 0 otherwise.
  Output 5  => The X-pipe TDM oversubscription table wrap pointer value.
  Output 6  => The Y-pipe TDM oversubscription table wrap pointer value.
  Output 7  => The X-pipe TDM oversubscription schedule.
  Output 8  => The Y-pipe TDM oversubscription schedule.
**/
void init_iarb_tdm_ovs_table (
          int core_bw,
          int mgm4x1,
          int mgm4x2p5,
          int mgm1x10,
          int *iarb_tdm_wrap_ptr_ovs_x,
          int *iarb_tdm_wrap_ptr_ovs_y,
          int iarb_tdm_tbl_ovs_x[512],
          int iarb_tdm_tbl_ovs_y[512]
          ) {
  int i;

  switch (core_bw) {
    case 960 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_ovs_x = 199;
      *iarb_tdm_wrap_ptr_ovs_y = 199;
      for (i = 0; i < 24; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[24] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[24] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 25; i < 49; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[49] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[49] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 50; i < 74; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[74] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[74] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 75; i < 99; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[99] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[99] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 100; i < 124; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[124] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[124] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 125; i < 149; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[149] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[149] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 150; i < 174; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[174] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[174] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 175; i < 199; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[199] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[199] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_ovs_x[24] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[49] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[99] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        /*
          Nate's comment in spreadsheet: "Not suppoted in Iarb. It's still possible
          to have a 10G management port scheduled with regular ports in PGW".
        */
      }
      break;
    case 720 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_ovs_x = 159;
      *iarb_tdm_wrap_ptr_ovs_y = 159;
      for (i = 0; i < 15; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[15] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[15] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[16] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 17; i < 31; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[31] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[31] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 32; i < 47; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[47] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[47] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[48] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[48] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 49; i < 63; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 64; i < 79; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[79] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[79] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[80] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[80] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 81; i < 95; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[95] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[95] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 96; i < 111; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[111] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[111] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[112] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[112] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 113; i < 127; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[127] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[127] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 128; i < 143; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[143] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[143] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[144] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[144] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 145; i < 159; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[159] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[159] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[31] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[48] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[31] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[48] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[80] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[95] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[112] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[127] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
    case 640 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_ovs_x = 135;
      *iarb_tdm_wrap_ptr_ovs_y = 135;
      for (i = 0; i < 16; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[16] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 17; i < 33; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[33] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[33] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 34; i < 50; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[50] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[50] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 51; i < 67; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[67] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[67] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 68; i < 84; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[84] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[84] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 85; i < 101; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[101] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[101] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 102; i < 118; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[118] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[118] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 119; i < 135; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[135] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[135] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_ovs_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[50] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[33] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[67] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        /*
          Nate's comment in spreadsheet: "Not suppoted in Iarb. It's still possible
          to have a 10G management port scheduled with regular ports in PGW".
        */
      }
      break;
    case 480 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_ovs_x = 105;
      *iarb_tdm_wrap_ptr_ovs_y = 105;
      for (i = 0; i < 10; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[10] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[10] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 11; i < 20; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[20] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[20] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_x[21] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[21] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 22; i < 30; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[30] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[30] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_ovs_x[31] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_ovs_y[31] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 32; i < 41; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[41] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[41] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 42; i < 52; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[52] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[52] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 53; i < 63; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 64; i < 73; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[73] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_ovs_y[73] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_ovs_x[74] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[74] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 75; i < 83; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[83] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[83] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_ovs_x[84] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_ovs_y[84] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 85; i < 94; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_ovs_x[94] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_ovs_y[94] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 95; i < 105; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_ovs_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_ovs_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_ovs_x[105] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_ovs_y[105] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_ovs_x[10] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[21] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[30] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[41] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_ovs_x[10] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[21] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[30] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[41] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[52] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_ovs_x[83] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
  }
}


/**
@name: init_iarb_tdm_lr_table
@param: int, int, int, int, int*, int*, int[512], int[512]

IARB TDM Linerate Schedule:
  Input  1  => Core bandwidth - 960/720/640/480.
  Input  2  => 1 if there are 4x1G management ports, 0 otherwise.
  Input  3  => 1 if there are 4x2.5G management ports, 0 otherwise.
  Input  4  => 1 if there are 1x10G management port, 0 otherwise.
  Output 5  => The X-pipe TDM linerate table wrap pointer value.
  Output 6  => The Y-pipe TDM linerate table wrap pointer value.
  Output 7  => The X-pipe TDM linerate schedule.
  Output 8  => The Y-pipe TDM linerate schedule.
**/
void init_iarb_tdm_lr_table (
          int core_bw,
          int mgm4x1,
          int mgm4x2p5,
          int mgm1x10,
          int *iarb_tdm_wrap_ptr_lr_x,
          int *iarb_tdm_wrap_ptr_lr_y,
          int iarb_tdm_tbl_lr_x[512],
          int iarb_tdm_tbl_lr_y[512]
          ) {
  int i;

  switch (core_bw) {
    case 960 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_lr_x = 199;
      *iarb_tdm_wrap_ptr_lr_y = 199;
      for (i = 0; i < 24; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[24] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[24] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 25; i < 49; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[49] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[49] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 50; i < 74; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[74] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 75; i < 99; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[99] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[99] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 100; i < 124; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[124] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[124] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 125; i < 149; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[149] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[149] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 150; i < 174; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[174] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[174] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 175; i < 199; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[199] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[199] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_lr_x[24] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[124] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[174] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[199] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_lr_x[24] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[124] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[174] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[49] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[99] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[149] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[199] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
    case 720 : 
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_lr_x = 159;
      *iarb_tdm_wrap_ptr_lr_y = 159;
      for (i = 0; i < 15; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[15] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[15] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 16; i < 31; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[31] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[31] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 32; i < 47; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[47] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[47] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 48; i < 63; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 64; i < 79; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[79] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[79] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 80; i < 95; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[95] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[95] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 96; i < 111; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[111] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[111] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 112; i < 127; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[127] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[127] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 128; i < 143; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[143] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[143] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 144; i < 159; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[159] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[159] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_lr_x[15] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[31] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[47] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_lr_x[15] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[31] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[47] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[79] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[95] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[111] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[127] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
    case 640 : 
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_lr_x = 135;
      *iarb_tdm_wrap_ptr_lr_y = 135;
      for (i = 0; i < 16; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[16] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[16] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 17; i < 33; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[33] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[33] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 34; i < 50; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[50] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[50] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 51; i < 67; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[67] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[67] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 68; i < 84; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[84] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[84] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 85; i < 101; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[101] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[101] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 102; i < 118; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[118] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[118] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 119; i < 135; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[135] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[135] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_lr_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[50] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[33] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[67] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_lr_x[16] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[50] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[33] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[67] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[84] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[101] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[118] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[135] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
    case 480 :
      /* First, assume no management ports. */
      *iarb_tdm_wrap_ptr_lr_x = 105;
      *iarb_tdm_wrap_ptr_lr_y = 105;
      for (i = 0; i < 10; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[10] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[10] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 11; i < 20; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[20] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_lr_y[20] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_x[21] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[21] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 22; i < 30; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[30] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[30] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_lr_x[31] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_lr_y[31] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 32; i < 41; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[41] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[41] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 42; i < 52; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[52] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[52] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 53; i < 63; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[63] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 64; i < 73; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[73] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      iarb_tdm_tbl_lr_y[73] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[74] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
      for (i = 75; i < 83; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[83] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[83] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_lr_x[84] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
      iarb_tdm_tbl_lr_y[84] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      for (i = 85; i < 94; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        }
      }
      iarb_tdm_tbl_lr_x[94] = IARB_MAIN_TDM__TDM_SLOT_CMIC_PORT;
      iarb_tdm_tbl_lr_y[94] = IARB_MAIN_TDM__TDM_SLOT_EP_LOOPBACK;
      for (i = 95; i < 105; i++) {
        if (i%2 == 0) {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_1;
        } else {
          iarb_tdm_tbl_lr_x[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
          iarb_tdm_tbl_lr_y[i] = IARB_MAIN_TDM__TDM_SLOT_PGW_0;
        }
      }
      iarb_tdm_tbl_lr_x[105] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;
      iarb_tdm_tbl_lr_y[105] = IARB_MAIN_TDM__TDM_SLOT_AUX_OPS_SLOT;

      /* Then check for management ports. */
      if (mgm4x1) {
        iarb_tdm_tbl_lr_x[10] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[21] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[30] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[41] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      } else if (mgm4x2p5 || mgm1x10) {
        iarb_tdm_tbl_lr_x[10] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[21] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[30] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[41] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[52] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[63] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[74] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
        iarb_tdm_tbl_lr_x[83] = IARB_MAIN_TDM__TDM_SLOT_QGP_PORT;
      }
      break;
  }
}


/**
@name: TD2P_set_iarb_tdm_table
@param: int, int, int, int, int, int, int*, int*, int[512], int[512]

IARB TDM Schedule Generator:
  Return    => 1 if error, 0 otherwise.
  Input  1  => Core bandwidth - 960/720/640/480.
  Input  2  => 1 if X-pipe is oversub, 0 otherwise.
  Input  3  => 1 if Y-pipe is oversub, 0 otherwise.
  Input  4  => 1 if there are 4x1G management ports, 0 otherwise.
  Input  5  => 1 if there are 4x2.5G management ports, 0 otherwise.
  Input  6  => 1 if there are 1x10G management port, 0 otherwise.
  Output 7  => The X-pipe TDM table wrap pointer value.
  Output 8  => The Y-pipe TDM table wrap pointer value.
  Output 9  => The X-pipe TDM schedule.
  Output 10 => The Y-pipe TDM schedule.
**/
int TD2P_set_iarb_tdm_table (
          int core_bw,
          int is_x_ovs,
          int is_y_ovs,
          int mgm4x1,
          int mgm4x2p5,
          int mgm1x10,
          int *iarb_tdm_wrap_ptr_x,
          int *iarb_tdm_wrap_ptr_y,
          int iarb_tdm_tbl_x[512],
          int iarb_tdm_tbl_y[512]
          ) {
		  
	/* #if ( defined(_SET_TDM_DV) || defined(_SET_TDM_DEV) ) */
		int i;
		int is_succ;
		int iarb_tdm_wrap_ptr_ovs_x, iarb_tdm_wrap_ptr_ovs_y;
		/*int iarb_tdm_tbl_ovs_x[512], iarb_tdm_tbl_ovs_y[512];*/
		int iarb_tdm_wrap_ptr_lr_x, iarb_tdm_wrap_ptr_lr_y;
		/*int iarb_tdm_tbl_lr_x[512], iarb_tdm_tbl_lr_y[512];*/
		int *iarb_tdm_tbl_ovs_x, *iarb_tdm_tbl_ovs_y;
		int *iarb_tdm_tbl_lr_x, *iarb_tdm_tbl_lr_y;
		iarb_tdm_tbl_ovs_x = (int *) TDM_ALLOC(sizeof(int) * 512, "iarb_tdm_tbl_ovs_x");
		iarb_tdm_tbl_ovs_y = (int *) TDM_ALLOC(sizeof(int) * 512, "iarb_tdm_tbl_ovs_y");
		iarb_tdm_tbl_lr_x = (int *) TDM_ALLOC(sizeof(int) * 512, "iarb_tdm_tbl_lr_x");
		iarb_tdm_tbl_lr_y = (int *) TDM_ALLOC(sizeof(int) * 512, "iarb_tdm_tbl_lr_y");
	/* #else
		int i;
		int is_succ;
		int iarb_tdm_wrap_ptr_ovs_x, iarb_tdm_wrap_ptr_ovs_y;
		int iarb_tdm_wrap_ptr_lr_x, iarb_tdm_wrap_ptr_lr_y;
		TDM_ALLOC(iarb_tdm_tbl_ovs_x, int, 512, "iarb_tdm_tbl_ovs_x");
		TDM_ALLOC(iarb_tdm_tbl_ovs_y, int, 512, "iarb_tdm_tbl_ovs_y");
		TDM_ALLOC(iarb_tdm_tbl_lr_x, int, 512, "iarb_tdm_tbl_lr_x");
		TDM_ALLOC(iarb_tdm_tbl_lr_y, int, 512, "iarb_tdm_tbl_lr_y");
	#endif */

  /*
    Initial IARB TDM table containers - to be copied into final container based
    on the TDM selected.
  */
  if (!(!mgm4x1 && !mgm4x2p5 && !mgm1x10) && !(mgm4x1 ^ mgm4x2p5 ^ mgm1x10)) {
      TDM_ERROR0("IARB TDM: _____ERROR: Multiple management port settings specified!\n");
  }

	init_iarb_tdm_ovs_table(core_bw, mgm4x1, mgm4x2p5, mgm1x10, 
                          &iarb_tdm_wrap_ptr_ovs_x, &iarb_tdm_wrap_ptr_ovs_y,
                          iarb_tdm_tbl_ovs_x, iarb_tdm_tbl_ovs_y);
	init_iarb_tdm_lr_table(core_bw, mgm4x1, mgm4x2p5, mgm1x10,
                         &iarb_tdm_wrap_ptr_lr_x, &iarb_tdm_wrap_ptr_lr_y,
                         iarb_tdm_tbl_lr_x, iarb_tdm_tbl_lr_y);

  if ((is_x_ovs == 0) && (is_y_ovs == 0)) {
    /* The following TDMs have linerate X-pipe and linerate Y-pipe. */
    *iarb_tdm_wrap_ptr_x = iarb_tdm_wrap_ptr_lr_x;
    *iarb_tdm_wrap_ptr_y = iarb_tdm_wrap_ptr_lr_y;
    memcpy(iarb_tdm_tbl_x, iarb_tdm_tbl_lr_x, sizeof(int) * 512);
    memcpy(iarb_tdm_tbl_y, iarb_tdm_tbl_lr_y, sizeof(int) * 512);
  }
  if ((is_x_ovs == 0) && (is_y_ovs == 1)) {
    /* The following TDMs have linerate X-pipe and oversubscribed Y-pipe. */
    *iarb_tdm_wrap_ptr_x = iarb_tdm_wrap_ptr_lr_x;
    *iarb_tdm_wrap_ptr_y = iarb_tdm_wrap_ptr_ovs_y;
    memcpy(iarb_tdm_tbl_x, iarb_tdm_tbl_lr_x, sizeof(int) * 512);
    memcpy(iarb_tdm_tbl_y, iarb_tdm_tbl_ovs_y, sizeof(int) * 512);
  }
  if ((is_x_ovs == 1) && (is_y_ovs == 0)) {
    /* The following TDMs have oversubscribed X-pipe and linerate Y-pipe. */
    *iarb_tdm_wrap_ptr_x = iarb_tdm_wrap_ptr_ovs_x;
    *iarb_tdm_wrap_ptr_y = iarb_tdm_wrap_ptr_lr_y;
    memcpy(iarb_tdm_tbl_x, iarb_tdm_tbl_ovs_x, sizeof(int) * 512);
    memcpy(iarb_tdm_tbl_y, iarb_tdm_tbl_lr_y, sizeof(int) * 512);
  }
  if ((is_x_ovs == 1) && (is_y_ovs == 1)) {
    /* The following TDMs have oversubscribed X-pipe and oversubscribed Y-pipe. */
    *iarb_tdm_wrap_ptr_x = iarb_tdm_wrap_ptr_ovs_x;
    *iarb_tdm_wrap_ptr_y = iarb_tdm_wrap_ptr_ovs_y;
    memcpy(iarb_tdm_tbl_x, iarb_tdm_tbl_ovs_x, sizeof(int) * 512);
    memcpy(iarb_tdm_tbl_y, iarb_tdm_tbl_ovs_y, sizeof(int) * 512);
  }

  TDM_PRINT1("IARB TDM: _____DEBUG: iarb_tdm_wrap_ptr_x = %d\n",*iarb_tdm_wrap_ptr_x);
  for (i = 0; i <= *iarb_tdm_wrap_ptr_x; i++) {
    TDM_PRINT2("IARB TDM: _____DEBUG: iarb_tdm_tbl_x[%d] = %d\n",i,iarb_tdm_tbl_x[i]);
  }
  TDM_PRINT1("IARB TDM: _____DEBUG: iarb_tdm_wrap_ptr_y = %d\n",*iarb_tdm_wrap_ptr_y);
  for (i = 0; i <= *iarb_tdm_wrap_ptr_y; i++) {
    TDM_PRINT2("IARB TDM: _____DEBUG: iarb_tdm_tbl_y[%d] = %d\n",i,iarb_tdm_tbl_y[i]);
  }

  /* Always succeeds by definition. */
  is_succ = 1;
  TDM_FREE(iarb_tdm_tbl_ovs_x);
  TDM_FREE(iarb_tdm_tbl_ovs_y);
  TDM_FREE(iarb_tdm_tbl_lr_x);
  TDM_FREE(iarb_tdm_tbl_lr_y);
  return is_succ;
}


void chk_tdm_td2p( enum port_speed speed[_TD2P_NUM_EXT_PORTS], int core_bw,
		 int pgw_tdm_tbl_x0[64], int ovs_tdm_tbl_x0[32], int ovs_spacing_x0[32],
   		 int pgw_tdm_tbl_x1[64], int ovs_tdm_tbl_x1[32], int ovs_spacing_x1[32],
   		 int pgw_tdm_tbl_y0[64], int ovs_tdm_tbl_y0[32], int ovs_spacing_y0[32],
   		 int pgw_tdm_tbl_y1[64], int ovs_tdm_tbl_y1[32], int ovs_spacing_y1[32],
		 int mmu_tdm_tbl_x[256], int mmu_tdm_ovs_x_0[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_x_1[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_x_2[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_x_3[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_x_4[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_x_5[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_x_6[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_x_7[_TD2P_OS_GROUP_LEN],
   		 int mmu_tdm_tbl_y[256], int mmu_tdm_ovs_y_0[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_y_1[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_y_2[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_y_3[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_y_4[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_y_5[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_y_6[_TD2P_OS_GROUP_LEN], int mmu_tdm_ovs_y_7[_TD2P_OS_GROUP_LEN],
      	 int port_state_map[_TD2P_NUM_EXT_PORTS], int wc_checker[NUM_TSC][4],
		 int fail[14] )
{
	int *pgw_x0_mtbl, *pgw_x1_mtbl, *pgw_y0_mtbl, *pgw_y1_mtbl, *pgw_x0_tracker, *pgw_x1_tracker, *pgw_y0_tracker, *pgw_y1_tracker;
	int s1, s1xcnt=0, s1xavg=0, s1ycnt=0, s1yavg=0, s2, s2xcnt=0, s2xavg=0, s2ycnt=0, s2yavg=0, s3, s3xcnt=0, s3xavg=0, s3ycnt=0, s3yavg=0, /*s5=4,*/ s5xcnt=0, s5xavg=0, s5ycnt=0, s5yavg=0, /*s4=5,*/ s4xcnt=0, s4xavg=0, s4ycnt=0, s4yavg=0, t2x0=0, t2x1=0, t2y0=0, t2y1=0, t3x0=0, t3x1=0, t3y0=0, t3y1=0, t4t5=2;
	long int clk;
	int accessories;
	/* int b1x_spd, b2x_spd, b3x_spd, b4x_spd, b1y_spd, b2y_spd, b3y_spd, b4y_spd; */
	int pgw_x0_len=0, pgw_x1_len=0, pgw_y0_len=0, pgw_y1_len=0;
	int msub_x0=0, msub_x1=0, msub_y0=0, msub_y1=0;
	int i, j, k;
	int wc_principle=-1;	
	
	pgw_x0_mtbl = (int *) TDM_ALLOC(sizeof(int) * 64, "pgw_x0_mtbl");
	pgw_x1_mtbl = (int *) TDM_ALLOC(sizeof(int) * 64, "pgw_x1_mtbl");
	pgw_y0_mtbl = (int *) TDM_ALLOC(sizeof(int) * 64, "pgw_y0_mtbl");
	pgw_y1_mtbl = (int *) TDM_ALLOC(sizeof(int) * 64, "pgw_y1_mtbl");
	pgw_x0_tracker = (int *) TDM_ALLOC(sizeof(int) * 130, "pgw_x0_tracker");
	pgw_x1_tracker = (int *) TDM_ALLOC(sizeof(int) * 130, "pgw_x1_tracker");
	pgw_y0_tracker = (int *) TDM_ALLOC(sizeof(int) * 130, "pgw_y0_tracker");
	pgw_y1_tracker = (int *) TDM_ALLOC(sizeof(int) * 130, "pgw_y1_tracker");
	
	switch (core_bw) {
		case 960: clk=76000; accessories=8; break;
		case 720: clk=59878; accessories=10; break;
		/* case 720: clk=60833; accessories=10; break;  */
		case 640: clk=51786; accessories=8; break;
		case 480: clk=39000; accessories=10; break;
		/* case 480: clk=41563; accessories=10; break; */
		default: clk=76000; accessories=8; break;
	}
	
	TDM_VERBOSE3("(clk - %0ld) (core bw - %0d) (mmu acc - %0d)\n", clk, core_bw, accessories);
	s3=(((12*((clk/76))))/1000); /*40G*/
	TDM_VERBOSE1("TDM: _____VERBOSE: calculated 40G MMU spacing as %0d\n", s3);
	s2=(((24*((clk/76))))/1000); /*20G*/
	TDM_VERBOSE1("TDM: _____VERBOSE: calculated 20G MMU spacing as %0d\n", s2);
	s1=(((495*((clk/76))))/10000); /*10G*/
	TDM_VERBOSE1("TDM: _____VERBOSE: calculated 10G MMU spacing as %0d\n", s1);	
	
	for (i=0; i<32; i++) {
		if (pgw_tdm_tbl_x0[i]!=_TD2P_NUM_EXT_PORTS) pgw_x0_len++;
		if (pgw_tdm_tbl_x1[i]!=_TD2P_NUM_EXT_PORTS) pgw_x1_len++;
		if (pgw_tdm_tbl_y0[i]!=_TD2P_NUM_EXT_PORTS) pgw_y0_len++;
		if (pgw_tdm_tbl_y1[i]!=_TD2P_NUM_EXT_PORTS) pgw_y1_len++;
	}
	TDM_VERBOSE1("TDM: _____VERBOSE: length of pgw table x0 is %0d\n", pgw_x0_len);
	TDM_VERBOSE1("TDM: _____VERBOSE: length of pgw table x1 is %0d\n", pgw_x1_len);
	TDM_VERBOSE1("TDM: _____VERBOSE: length of pgw table y0 is %0d\n", pgw_y0_len);
	TDM_VERBOSE1("TDM: _____VERBOSE: length of pgw table y1 is %0d\n", pgw_y1_len);	
	
	t3x0 = (pgw_x0_len==(core_bw/40)) ? (((6*((clk/76))))/1000) : (pgw_x0_len/4);
	t3x1 = (pgw_x1_len==(core_bw/40)) ? (((6*((clk/76))))/1000) : (pgw_x1_len/4);
	t3y0 = (pgw_y0_len==(core_bw/40)) ? (((6*((clk/76))))/1000) : (pgw_y0_len/4);
	t3y1 = (pgw_y1_len==(core_bw/40)) ? (((6*((clk/76))))/1000) : (pgw_y1_len/4);
	TDM_VERBOSE4("calculated 40G PGW spacing x0-%0d x1-%0d y0-%0d y1-%0d\n", t3x0, t3x1, t3y0, t3y1);
	t2x0 = (pgw_x0_len==(core_bw/40)) ? (((12*((clk/76))))/1000): (pgw_x0_len/2);
	t2x1 = (pgw_x1_len==(core_bw/40)) ? (((12*((clk/76))))/1000): (pgw_x1_len/2);
	t2y0 = (pgw_y0_len==(core_bw/40)) ? (((12*((clk/76))))/1000): (pgw_y0_len/2);
	t2y1 = (pgw_y1_len==(core_bw/40)) ? (((12*((clk/76))))/1000): (pgw_y1_len/2);
	TDM_VERBOSE4("calculated 20G PGW spacing x0-%0d x1-%0d y0-%0d y1-%0d\n", t2x0, t2x1, t2y0, t2y1);
	
	for (i=0; i<pgw_x0_len; i++) {pgw_x0_mtbl[i]=pgw_tdm_tbl_x0[i]; pgw_x0_mtbl[i+pgw_x0_len]=pgw_tdm_tbl_x0[i];}
	for (i=0; i<pgw_x1_len; i++) {pgw_x1_mtbl[i]=pgw_tdm_tbl_x1[i]; pgw_x1_mtbl[i+pgw_x1_len]=pgw_tdm_tbl_x1[i];}
	for (i=0; i<pgw_y0_len; i++) {pgw_y0_mtbl[i]=pgw_tdm_tbl_y0[i]; pgw_y0_mtbl[i+pgw_y0_len]=pgw_tdm_tbl_y0[i];}
	for (i=0; i<pgw_y1_len; i++) {pgw_y1_mtbl[i]=pgw_tdm_tbl_y1[i]; pgw_y1_mtbl[i+pgw_y1_len]=pgw_tdm_tbl_y1[i];}
	
	for (i=0; i<32; i++) {if (port_state_map[i] == 1) {if (port_state_map[i+1] != 3) {if (speed[i+1]<SPEED_10G) msub_x0++;}}}
	for (i=32; i<64; i++) {if (port_state_map[i] == 1) {if (port_state_map[i+1] != 3) {if (speed[i+1]<SPEED_10G) msub_x1++;}}}
	for (i=64; i<96; i++) {if (port_state_map[i] == 1) {if (port_state_map[i+1] != 3) {if (speed[i+1]<SPEED_10G) msub_y0++;}}}
	for (i=96; i<128; i++) {if (port_state_map[i] == 1) {if (port_state_map[i+1] != 3) {if (speed[i+1]<SPEED_10G) msub_y1++;}}}
	if (msub_x0!=0) for (i=0; i<32; i++) {if (port_state_map[i] == 1) {if (port_state_map[i+1] != 3) {if (speed[i+1]>=SPEED_10G) msub_x0+=10;}}}
	if (msub_x1!=0) for (i=32; i<64; i++) {if (port_state_map[i] == 1) {if (port_state_map[i+1] != 3) {if (speed[i+1]>=SPEED_10G) msub_x1+=10;}}}
	if (msub_y0!=0) for (i=64; i<96; i++) {if (port_state_map[i] == 1) {if (port_state_map[i+1] != 3) {if (speed[i+1]>=SPEED_10G) msub_y0+=10;}}}
	if (msub_y1!=0) for (i=96; i<128; i++) {if (port_state_map[i] == 1) {if (port_state_map[i+1] != 3) {if (speed[i+1]>=SPEED_10G) msub_y1+=10;}}}

	if (fail[7]==2) {
		/* Check length (overflow and symmetry) */
		if (msub_x0==0 && msub_x1==0 && msub_y0==0 && msub_y1==0) {
			switch (core_bw) {
				case 960:
					if (pgw_x0_len>24 || pgw_x1_len>24 || pgw_y0_len>24 || pgw_y1_len>24) {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					if (pgw_x0_len!=pgw_x1_len || pgw_y0_len!=pgw_y1_len) {/*fail[7]=1; */TDM_WARN0("TDM: _____WARNING: PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				case 720:
					if (pgw_x0_len>16 || pgw_y1_len>16) {fail[7]=1; TDM_ERROR0("PGW table short quadrant overscheduled\n");}
					if (pgw_x1_len>20 || pgw_y0_len>20) {fail[7]=1; TDM_ERROR0("PGW table long quadrant overscheduled\n");}
					break;
				case 640:
					if (pgw_x0_len>16 || pgw_x1_len>16 || pgw_y0_len>16 || pgw_y1_len>16) {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					if (pgw_x0_len!=pgw_x1_len || pgw_y0_len!=pgw_y1_len) {/*fail[7]=1; */TDM_WARN0("TDM: _____WARNING: PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				case 480:
					if (pgw_x0_len>12 || pgw_x1_len>12 || pgw_y0_len>12 || pgw_y1_len>12) {fail[7]=1; TDM_ERROR0("PGW table overscheduled\n");}
					if (pgw_x0_len!=pgw_x1_len || pgw_y0_len!=pgw_y1_len) {/*fail[7]=1; */TDM_WARN0("TDM: _____WARNING: PGW table asymmetry on symmetrically subscribed core bw\n");}
					break;
				default: break;
			}
		}
	}

	/* Init bitmaps */
	for (i=0; i<130; i++) {
		pgw_x0_tracker[i]=0; pgw_x1_tracker[i]=0; pgw_y0_tracker[i]=0; pgw_y1_tracker[i]=0;
	}
	
	if (fail[8]==2 && fail[9]==2) {
		/* Check pgw tables */
		/* Length and loopback passes are implicitly combined */
		for (i=0; i<pgw_x0_len; i++) {
			TOKEN_CHECK(pgw_x0_mtbl[i]) {
				pgw_x0_tracker[pgw_x0_mtbl[i]]=1;
				for (j=0; j<NUM_TSC; j++) {
					for (k=0; k<4; k++) {
						if (wc_checker[j][k] == pgw_x0_mtbl[i]) {
							wc_principle = j; break;
						}
					}
					if (wc_principle != -1) {
						break;
					}
				}
				if (wc_principle == -1) {fail[8] = 1; TDM_ERROR1("TSC transcription corruption pgw quadrant x0 port %0d\n", pgw_x0_mtbl[i]);}
				if (pgw_x0_len>8) if (pgw_x0_mtbl[i+1]==wc_checker[wc_principle][0] || pgw_x0_mtbl[i+1]==wc_checker[wc_principle][1] || pgw_x0_mtbl[i+1]==wc_checker[wc_principle][2] || pgw_x0_mtbl[i+1]==wc_checker[wc_principle][3]) {fail[9]=1; TDM_ERROR2("TSC proximity violation at pgw x0 between index %0d and index %0d\n", i, (i+1));}
				for (j=1; j<=pgw_x0_len; j++) {if (pgw_x0_mtbl[i+j]!=pgw_x0_mtbl[i]) pgw_x0_tracker[pgw_x0_mtbl[i]]++; else break;}
				wc_principle=-1;
			}
		}
		for (i=0; i<pgw_x1_len; i++) {
			TOKEN_CHECK(pgw_x1_mtbl[i]) {
				pgw_x1_tracker[pgw_x1_mtbl[i]]=1;
				for (j=0; j<NUM_TSC; j++) {
					for (k=0; k<4; k++) {
						if (wc_checker[j][k] == pgw_x1_mtbl[i]) {
							wc_principle = j; break;
						}
					}
					if (wc_principle != -1) {
						break;
					}
				}
				if (wc_principle == -1) {fail[8] = 1; TDM_ERROR1("TSC transcription corruption pgw quadrant x1 port %0d\n", pgw_x1_mtbl[i]);}
				if (pgw_x1_len>8) if (pgw_x1_mtbl[i+1]==wc_checker[wc_principle][0] || pgw_x1_mtbl[i+1]==wc_checker[wc_principle][1] || pgw_x1_mtbl[i+1]==wc_checker[wc_principle][2] || pgw_x1_mtbl[i+1]==wc_checker[wc_principle][3]) {fail[9]=1; TDM_ERROR2("TSC proximity violation at pgw x1 between index %0d and index %0d\n", i, (i+1));}
				for (j=1; j<=pgw_x1_len; j++) {if (pgw_x1_mtbl[i+j]!=pgw_x1_mtbl[i]) pgw_x1_tracker[pgw_x1_mtbl[i]]++; else break;}
				wc_principle=-1;
			}
		}
		for (i=0; i<pgw_y0_len; i++) {
			TOKEN_CHECK(pgw_y0_mtbl[i]) {
				pgw_y0_tracker[pgw_y0_mtbl[i]]=1;
				for (j=0; j<NUM_TSC; j++) {
					for (k=0; k<4; k++) {
						if (wc_checker[j][k] == pgw_y0_mtbl[i]) {
							wc_principle = j; break;
						}
					}
					if (wc_principle != -1) {
						break;
					}
				}
				if (wc_principle == -1) {fail[8] = 1; TDM_ERROR1("TSC transcription corruption pgw quadrant y0 port %0d\n", pgw_y0_mtbl[i]);}
				if (pgw_y0_len>8) if (pgw_y0_mtbl[i+1]==wc_checker[wc_principle][0] || pgw_y0_mtbl[i+1]==wc_checker[wc_principle][1] || pgw_y0_mtbl[i+1]==wc_checker[wc_principle][2] || pgw_y0_mtbl[i+1]==wc_checker[wc_principle][3]) {fail[9]=1; TDM_ERROR2("TSC proximity violation at pgw y0 between index %0d and index %0d\n", i, (i+1));}
				for (j=1; j<=pgw_y0_len; j++) {if (pgw_y0_mtbl[i+j]!=pgw_y0_mtbl[i]) pgw_y0_tracker[pgw_y0_mtbl[i]]++; else break;}
				wc_principle=-1;
			}
		}
		for (i=0; i<pgw_y1_len; i++) {
			TOKEN_CHECK(pgw_y1_mtbl[i]) {
				pgw_y1_tracker[pgw_y1_mtbl[i]]=1;
				for (j=0; j<NUM_TSC; j++) {
					for (k=0; k<4; k++) {
						if (wc_checker[j][k] == pgw_y1_mtbl[i]) {
							wc_principle = j; break;
						}
					}
					if (wc_principle != -1) {
						break;
					}
				}
				if (wc_principle == -1) {fail[8] = 1; TDM_ERROR1("TSC transcription corruption pgw quadrant y1 port %0d\n", pgw_y1_mtbl[i]);}
				if (pgw_y1_len>8) if (pgw_y1_mtbl[i+1]==wc_checker[wc_principle][0] || pgw_y1_mtbl[i+1]==wc_checker[wc_principle][1] || pgw_y1_mtbl[i+1]==wc_checker[wc_principle][2] || pgw_y1_mtbl[i+1]==wc_checker[wc_principle][3]) {fail[9]=1; TDM_ERROR2("TSC proximity violation at pgw y1 between index %0d and index %0d\n", i, (i+1));}
				for (j=1; j<=pgw_y1_len; j++) {if (pgw_y1_mtbl[i+j]!=pgw_y1_mtbl[i]) pgw_y1_tracker[pgw_y1_mtbl[i]]++; else break;}
				wc_principle=-1;
			}
		}
	}

	if (fail[10]==2) {
		
		for (i=1; i<=32; i++) {
			if (pgw_x0_tracker[i]!=0) {
				switch (speed[i]) {
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						if (pgw_x0_tracker[i] < t2x0-4 || pgw_x0_tracker[i] > t2x0+4) {fail[10]=1; TDM_ERROR2("20G port spacing error pgw x0 port %0d (%0d)\n", i,pgw_x0_tracker[i]);}
						break;
					case SPEED_42G:
					case SPEED_40G:
						if (pgw_x0_tracker[i] < t3x0-4 || pgw_x0_tracker[i] > t3x0+4) {fail[10]=1; TDM_ERROR2("40G port spacing error pgw x0 port %0d (%0d)\n", i,pgw_x0_tracker[i]);}
						break;
					case SPEED_120G:
					case SPEED_100G:
						if (pgw_x0_tracker[i] < t4t5 || pgw_x0_tracker[i] > t4t5+1) {fail[10]=1; TDM_ERROR2("100G/120G port spacing error pgw x0 port %0d (%0d)\n", i,pgw_x0_tracker[i]);}
						break;
					default:
						/* 10G/1G spacing is implicitly checked by both the TSC spacing and the spacing of all other speeds */ 
						break;
				}
			}
		}
		for (i=33; i<=64; i++) {
			if (pgw_x1_tracker[i]!=0) {
				switch (speed[i]) {
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						if (pgw_x1_tracker[i] < t2x1-4 || pgw_x1_tracker[i] > t2x1+4) {fail[10]=1; TDM_ERROR2("20G port spacing error pgw x1 port %0d (%0d)\n", i,pgw_x1_tracker[i]);}
						break;
					case SPEED_42G:
					case SPEED_40G:
						if (pgw_x1_tracker[i] < t3x1-4 || pgw_x1_tracker[i] > t3x1+4) {fail[10]=1; TDM_ERROR2("40G port spacing error pgw x1 port %0d (%0d)\n", i,pgw_x1_tracker[i]);}
						break;
					case SPEED_120G:
					case SPEED_100G:
						if (pgw_x1_tracker[i] < t4t5 || pgw_x1_tracker[i] > t4t5+1) {fail[10]=1; TDM_ERROR2("100G/120G port spacing error pgw x1 port %0d (%0d)\n", i,pgw_x1_tracker[i]);}
						break;
					default:
						/* 10G/1G spacing is implicitly checked by both the TSC spacing and the spacing of all other speeds */ 
						break;
				}
			}
		}
		for (i=65; i<=96; i++) {
			if (pgw_y0_tracker[i]!=0) {
				switch (speed[i]) {
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						if (pgw_y0_tracker[i] < t2y0-4 || pgw_y0_tracker[i] > t2y0+4) {fail[10]=1; TDM_ERROR2("20G port spacing error pgw y0 port %0d (%0d)\n", i,pgw_y0_tracker[i]);}
						break;
					case SPEED_42G:
					case SPEED_40G:
						if (pgw_y0_tracker[i] < t3y0-4 || pgw_y0_tracker[i] > t3y0+4) {fail[10]=1; TDM_ERROR2("40G port spacing error pgw y0 port %0d (%0d)\n", i,pgw_y0_tracker[i]);}
						break;
					case SPEED_120G:
					case SPEED_100G:
						if (pgw_y0_tracker[i] < t4t5 || pgw_y0_tracker[i] > t4t5+1) {fail[10]=1; TDM_ERROR2("100G/120G port spacing error pgw y0 port %0d (%0d)\n", i,pgw_y0_tracker[i]);}
						break;
					default:
						/* 10G/1G spacing is implicitly checked by both the TSC spacing and the spacing of all other speeds */ 
						break;
				}
			}
		}
		for (i=97; i<=128; i++) {
			if (pgw_y1_tracker[i]!=0) {
				switch (speed[i]) {
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						if (pgw_y1_tracker[i] < t2y1-4 || pgw_y1_tracker[i] > t2y1+4) {fail[10]=1; TDM_ERROR2("20G port spacing error pgw y1 port %0d (%0d)\n", i,pgw_y1_tracker[i]);}
						break;
					case SPEED_42G:
					case SPEED_40G:
						if (pgw_y1_tracker[i] < t3y1-4 || pgw_y1_tracker[i] > t3y1+4) {fail[10]=1; TDM_ERROR2("40G port spacing error pgw y1 port %0d (%0d)\n", i,pgw_y1_tracker[i]);}
						break;
					case SPEED_120G:
					case SPEED_100G:
						if (pgw_y1_tracker[i] < t4t5 || pgw_y1_tracker[i] > t4t5+1) {fail[10]=1; TDM_ERROR2("100G/120G port spacing error pgw y1 port %0d (%0d)\n", i,pgw_y1_tracker[i]);}
						break;
					default:
						/* 10G/1G spacing is implicitly checked by both the TSC spacing and the spacing of all other speeds */ 
						break;
				}
			}
		}
	}
	
	if (fail[11]==2) {
		s1xavg = (s1xcnt==0) ? 0 : ((s1xavg*100)/s1xcnt);
		s2xavg = (s2xcnt==0) ? 0 : ((s2xavg*100)/s2xcnt);
		s3xavg = (s3xcnt==0) ? 0 : ((s3xavg*100)/s3xcnt);
		s4xavg = (s4xcnt==0) ? 0 : ((s4xavg*100)/s4xcnt);
		s5xavg = (s5xcnt==0) ? 0 : ((s5xavg*100)/s5xcnt);
		s1yavg = (s1ycnt==0) ? 0 : ((s1yavg*100)/s1ycnt);
		s2yavg = (s2ycnt==0) ? 0 : ((s2yavg*100)/s2ycnt);
		s3yavg = (s3ycnt==0) ? 0 : ((s3yavg*100)/s3ycnt);
		s4yavg = (s4ycnt==0) ? 0 : ((s4yavg*100)/s4ycnt);
		s5yavg = (s5ycnt==0) ? 0 : ((s5yavg*100)/s5ycnt);
		if (s1xavg>0) if ( (s1xavg > (s1*101) || s1xavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("10G line rate not within 1 percent tolerance in x pipe, (s1xavg = %0d)\n", s1xavg);}
		if (s1yavg>0) if ( (s1yavg > (s1*101) || s1yavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("10G line rate not within 1 percent tolerance in y pipe, (s1yavg = %0d)\n", s1yavg);}
		if (s2xavg>0) if ( (s2xavg > (s1*101) || s2xavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("20G line rate not within 1 percent tolerance in x pipe, (s2xavg = %0d)\n", s2xavg);}
		if (s2yavg>0) if ( (s2yavg > (s1*101) || s2yavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("20G line rate not within 1 percent tolerance in y pipe, (s2yavg = %0d)\n", s2yavg);}
		if (s3xavg>0) if ( (s3xavg > (s1*101) || s3xavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("40G line rate not within 1 percent tolerance in x pipe, (s3xavg = %0d)\n", s3xavg);}
		if (s3yavg>0) if ( (s3yavg > (s1*101) || s3yavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("40G line rate not within 1 percent tolerance in y pipe, (s3yavg = %0d)\n", s3yavg);}
		if (s4xavg>0) if ( (s4xavg > (s1*101) || s4xavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("100G line rate not within 1 percent tolerance in x pipe, (s4xavg = %0d)\n", s4xavg);}
		if (s4yavg>0) if ( (s4yavg > (s1*101) || s4yavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("100G line rate not within 1 percent tolerance in y pipe, (s4yavg = %0d)\n", s4yavg);}
		if (s5xavg>0) if ( (s5xavg > (s1*101) || s5xavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("120G line rate not within 1 percent tolerance in x pipe, (s5xavg = %0d)\n", s5xavg);}
		if (s5yavg>0) if ( (s5yavg > (s1*101) || s5yavg < (s1*99)) ) {fail[11]=1; TDM_WARN1("120G line rate not within 1 percent tolerance in y pipe, (s5yavg = %0d)\n", s5yavg);}
	}
	
	if (fail[12]==2) {
		for (i=1; i<129; i++) {
			if (speed[i]==SPEED_100G || speed[i]==SPEED_120G) {
				if (i!=1 && i!=21 && i!=33 && i!=53 && i!=65 && i!=85 && i!=97 && i!=117) {fail[12]=1; TDM_WARN1("port number %0d is a 100G port that is improperly subscribed\n",i);
				}
			}
		}
	}
	
	if (fail[13]==2) {
		for (i=0; i<12; i++) {
			k=0;
			if (port_state_map[i]==1) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_120G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=12) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_100G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=12 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;	
				}
			}
			if (port_state_map[i]==2) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;
				}
			}
		}
		for (i=16; i<32; i++) {
			k=0;
			if (port_state_map[i]==1) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_120G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=12) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_100G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=12 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;	
				}
			}
			if (port_state_map[i]==2) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x0[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;
				}
			}
		}		
		for (i=32; i<64; i++) {
			k=0;
			if (port_state_map[i]==1) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_120G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=12) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_100G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=12 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;	
				}
			}
			if (port_state_map[i]==2) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_x1[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the X1 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;
				}
			}
		}
		for (i=64; i<96; i++) {
			k=0;
			if (port_state_map[i]==1) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_120G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=12) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_100G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=12 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;	
				}
			}
			if (port_state_map[i]==2) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_y0[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y0 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;
				}
			}
		}
		for (i=96; i<128; i++) {
			k=0;
			if (port_state_map[i]==1) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_120G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=12) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_100G:
						for (j=0; j<32; j++) if (pgw_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=12 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;	
				}
			}
			if (port_state_map[i]==2) {
				switch (speed[i+1]) {
					case SPEED_1G:
					case SPEED_2p5G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=1) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_10G:
					case SPEED_10G_XAUI:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=1 && k!=4 && k!=10) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_21G:
					case SPEED_20G:
					case SPEED_21G_DUAL:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=2) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					case SPEED_42G_HG2:
					case SPEED_42G:
					case SPEED_40G:
						for (j=0; j<32; j++) if (ovs_tdm_tbl_y1[j]==(i+1)) k++;
						if (k!=4) {fail[13]=1; TDM_ERROR2("port %0d subscription error in the Y1 PGW OVS table (k=%0d)\n",(i+1),k);}
						break;
					default:
						break;
				}
			}
		}
	}
		
	TDM_FREE(pgw_x0_mtbl);
	TDM_FREE(pgw_x1_mtbl);
	TDM_FREE(pgw_y0_mtbl);
	TDM_FREE(pgw_y1_mtbl);
	TDM_FREE(pgw_x0_tracker);
	TDM_FREE(pgw_x1_tracker);
	TDM_FREE(pgw_y0_tracker);
	TDM_FREE(pgw_y1_tracker);
	
}
