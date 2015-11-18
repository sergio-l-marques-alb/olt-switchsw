/*
 * $Id: arad_pp_frwrd_ipv4_lpm_mngr.c,v 1.34 Broadcom SDK $
 *
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
 *
 */

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)

#include<soc/dpp/SAND/Utils/sand_header.h>
#include<soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_ipv4_lpm_mngr.h>
#include<soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include<soc/dpp/SAND/Utils/sand_array_memory_allocator.h>
#include<soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include<soc/dpp/SAND/Utils/sand_os_interface.h>
#include<soc/dpp/SAND/Management/sand_error_code.h>
#include<soc/dpp/SAND/Management/sand_low_level.h>
#include<soc/dpp/dpp_wb_engine.h>
#include<soc/dpp/drv.h>
#include<assert.h>
#define l195 typedef
#define l67 int32
#define l369 SOC_SAND_LONG
#define le uint32
#define l8 SOC_SAND_ULONG
#define l4 static
#define lx uint8
#define l324 int
#define l355 enum
#define l285 ARAD_PP_ARR_MEM_ALLOCATOR_PTR
#define l281 struct
#define l18 prefix
#define l82 void
#define l11 SOC_SAND_OUT
#define lv ARAD_PP_IPV4_LPM_MNGR_INST
#define lg SOC_SAND_IN
#define l202 soc_sand_os_memcpy
#define li arr
#define l126 sizeof
#define lw ARAD_PP_IPV4_LPM_MNGR_INFO
#define lj return
#define l240 SOC_SAND_U64
#define l53 val
#define lf if
#define lr init_info
#define lo pxx_model
#define lk else
#define l19 SOC_SAND_INOUT
#define l14 ARAD_PP_IPV4_LPM_MNGR_INIT_INFO
#define l43 ARAD_PP_IPV4_LPM_PXX_MODEL_P6M
#define l48 ARAD_PP_IPV4_LPM_PXX_MODEL_P4X
#define l51 ARAD_PP_IPV4_LPM_PXX_MODEL_P4X_COMPLETE
#define l141 nof_rows_per_mem
#define l59 bank_to_mem
#define l50 ARAD_PP_IPV4_LPM_PXX_MODEL_P6N
#define l66 ARAD_PP_IPV4_LPM_PXX_MODEL_P6X
#define l39 ARAD_PP_IPV4_LPM_PXX_MODEL_LP4
#define l73 payload
#define l373 arad_pp_ipv4_lpm_entry_decode
#define l329 ARAD_PP_ARR_MEM_ALLOCATOR_ENTRY
#define l27 data
#define l5 for
#define l90 nof_mems
#define l171 nof_banks
#define l153 ARAD_PP_ARR_MEM_ALLOCATOR_INFO
#define lh res
#define l23 SOC_SAND_INIT_ERROR_DEFINITIONS
#define l125 mem_alloc_get_fun
#define l26 prime_handle
#define l40 sec_handle
#define lq SOC_SAND_CHECK_FUNC_RESULT
#define ln exit
#define l377 arad_pp_arr_mem_allocator_read_chunk
#define l29 SOC_SAND_EXIT_AND_SEND_ERROR
#define l248 soc_sand_os_memset
#define l93 goto
#define l226 continue
#define l406 arad_pp_arr_mem_allocator_read
#define l297 arad_pp_arr_mem_allocator_write_chunk
#define lz switch
#define ll case
#define l17 default
#define l62 FALSE
#define l91 arad_pp_ipv4_lpm_mngr_row_to_base_addr
#define l304 arad_pp_ipv4_lpm_mngr_update_base_addr
#define l7 break
#define lu data_info
#define lm bit_depth_per_bank
#define l83 ARAD_PP_IPV4_LPM_MNGR_T
#define l24 ARAD_DO_NOTHING_AND_EXIT
#define l388 ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS
#define l408 SOC_SAND_OK
#define l383 ARAD_PP_ARR_MEM_ALLOCATOR_REQ_BLOCKS_clear
#define l298 nof_reqs
#define l344 mem_to_bank
#define l238 block_size
#define l300 nof_bits_per_bank
#define l365 arad_pp_arr_mem_allocator_is_availabe_blocks
#define l280 arad_pp_sw_db_ipv4_cache_mode_for_ip_type_get
#define l318 ARAD_PP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM
#define l332 arad_pp_arr_mem_allocator_block_size
#define l370 arad_pp_sw_db_free_list_add
#define l382 arad_pp_arr_mem_allocator_free
#define l204 SOC_SAND_CHECK_NULL_INPUT
#define l361 arad_pp_arr_mem_allocator_malloc
#define l363 soc_sand_os_printf
#define l116 SOC_SAND_PAT_TREE_NODE_KEY
#define l142 SOC_SAND_PAT_TREE_NODE_INFO
#define l84 SOC_SAND_PAT_TREE_INFO
#define l2 pat_tree
#define l358 SOC_SAND_PAT_TREE_FIND_IDENTICAL
#define l360 SOC_SAND_HASH_TABLE_DESTROY
#define l34 nof_vrf_bits
#define l72 pat_tree_get_fun
#define l162 soc_sand_pat_tree_lpm_get
#define l340 node_place
#define l134 TRUE
#define l398 arad_pp_sw_db_free_list_commit
#define l348 arad_pp_ipv4_lpm_mngr_lookup0
#define l362 arad_pp_ipv4_lpm_mngr_create
#define l241 nof_lpms
#define l63 lpms
#define l342 node_data_is_identical_fun
#define l404 arad_pp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun
#define l390 node_is_skip_in_lpm_identical_data_query_fun
#define l339 arad_pp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun
#define l138 pat_tree_data
#define l368 root
#define l333 SOC_SAND_PAT_TREE_NULL
#define l112 flags
#define l279 ARAD_PP_LPV4_LPM_SHARED_MEM
#define l244 tree_memory
#define l287 tree_memory_cache
#define l283 tree_size
#define l308 support_cache
#define l325 memory_use
#define l336 soc_sand_pat_tree_create
#define l167 arad_pp_arr_mem_allocator_create
#define l119 mem_allocators
#define l371 ARAD_PP_IPV4_LPM_MEMORY_1
#define l293 ARAD_PP_IPV4_LPM_MEMORY_2
#define l294 ARAD_PP_IPV4_LPM_MEMORY_3
#define l292 ARAD_PP_IPV4_LPM_MEMORY_4
#define l291 ARAD_PP_IPV4_LPM_MEMORY_5
#define l290 ARAD_PP_IPV4_LPM_MEMORY_6
#define l397 soc_sand_os_malloc
#define l217 NULL
#define l346 SOC_SAND_SET_ERROR_CODE
#define l396 SOC_SAND_MALLOC_FAIL
#define l124 ARAD_PP_LPV4_LPM_SUPPORT_DEFRAG
#define l186 soc_sand_group_mem_ll_create
#define l92 rev_ptrs
#define l338 arad_pp_ipv4_lpm_mngr_vrf_init
#define l174 SOC_SAND_PP_SYSTEM_FEC_ID
#define l20 key
#define l314 SOC_SAND_PAT_TREE_NODE_DATA
#define l233 SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED
#define l205 soc_sand_pat_tree_node_add
#define l400 SAL_BOOT_QUICKTURN
#define l394 arad_pp_ipv4_lpm_mngr_vrf_clear
#define l197 SOC_PPC_FRWRD_IP_ROUTE_STATUS
#define l102 SOC_SAND_PP_IPV4_SUBNET
#define l275 soc_sand_SAND_PP_IPV4_SUBNET_clear
#define l94 ip_address
#define l96 prefix_len
#define l261 soc_sand_pat_tree_clear
#define l271 nof_entries_for_hw_lpm_set_fun
#define l251 arad_pp_ipv4_lpm_mngr_prefix_add
#define l190 SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM
#define l337 arad_pp_ipv4_lpm_mngr_vrf_routes_clear
#define l372 soc_sand_pat_tree_clear_nodes
#define l405 arad_pp_ipv4_lpm_mngr_all_vrfs_clear
#define l295 arad_pp_arr_mem_allocator_clear
#define l264 soc_sand_group_mem_ll_clear
#define l391 soc_sand_pat_tree_root_reset
#define l347 arad_pp_ipv4_lpm_mngr_destroy
#define l410 soc_sand_pat_tree_destroy
#define l403 arad_pp_arr_mem_allocator_destroy
#define l393 soc_sand_group_mem_ll_destroy
#define l249 soc_sand_os_free_any_size
#define l359 arad_pp_ipv4_lpm_mngr_hw_sync
#define l374 arad_pp_ipv4_lpm_mngr_mark_non_lpm_route_pending_type_set
#define l220 sys_fec_id
#define l247 SOC_PPC_FRWRD_IP_ROUTE_LOCATION
#define l350 SOC_SAND_PAT_TREE_FIND_EXACT
#define l411 SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_ADD
#define l326 soc_sand_pat_tree_node_remove
#define l407 arad_pp_ipv4_lpm_mngr_sync
#define l380 SOC_SAND_SUCCESS_FAILURE
#define l330 soc_sand_bitstream_get_any_field
#define l376 soc_sand_pat_tree_head_key_of_changes
#define l364 arad_pp_arr_mem_allocator_rollback
#define l354 soc_sand_group_mem_ll_rollback
#define l353 SOC_SAND_FAILURE_OUT_OF_RESOURCES
#define l352 arad_pp_arr_mem_allocator_commit
#define l401 soc_sand_group_mem_ll_commit
#define l386 SOC_SAND_SUCCESS
#define l375 soc_sand_pat_tree_cache_commit
#define l351 arad_pp_ipv4_lpm_mngr_cache_set
#define l366 arad_pp_arr_mem_allocator_cache_set
#define l389 soc_sand_group_mem_ll_cache_set
#define l402 ARAD_PP_FRWRD_IP_ALL_VRFS_ID
#define l319 soc_sand_pat_tree_cache_set
#define l341 arad_pp_ipv4_lpm_mngr_prefix_remove
#define l309 arad_pp_ipv4_lpm_mngr_prefix_lookup
#define l356 arad_pp_ipv4_lpm_mngr_prefix_is_exist
#define l334 arad_pp_ipv4_lpm_mngr_sys_fec_get
#define l392 arad_pp_ipv4_lpm_mngr_get_stat
#define l243 ARAD_PP_IPV4_LPM_MNGR_STATUS
#define l387 soc_sand_pat_tree_get_size
#define l276 total_nodes
#define l320 prefix_nodes
#define l305 free_nodes
#define l399 arad_pp_ipv4_lpm_mngr_clear
#define l335 arad_pp_ipv4_lpm_mngr_get_block
#define l395 SOC_SAND_PAT_TREE_ITER_TYPE
#define l381 ARAD_ALLOC
#define l378 while
#define l409 SOC_SAND_PAT_TREE_ITER_IS_END
#define l385 soc_sand_pat_tree_get_block
#define l357 ARAD_FREE
#define l345 arad_pp_ARAD_PP_IPV4_LPM_MNGR_INFO_clear
#define l106 info
#define l267 SOC_SAND_MAGIC_NUM_SET
#define l323 SOC_SAND_VOID_EXIT_AND_SEND_ERROR
#define l343 arad_pp_ARAD_PP_IPV4_LPM_MNGR_STATUS_clear
l195 l67 l369;l195 le l8;l4 lx l302[]={0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,
3};l4 l324 l161[]={0,0,0,0,0,0,0,0,0,0,0};l4 le l133=0;l195 l355{l163
,l75,l118,l36,l13,l77,l87,l10,l131,l115}l140;l195 l285 l97;l4 l67 l194
[]={0,35,21,-1,35,-1,35};l4 le l223[]={0,36,22,22,36,32,36};l195 l281
{l281{le l12;lx l18;lx l349;} *l331;le l379;}l384;l4 l82 l45(l11 lv*
l22,lg lv*l25){l202(l22->li,l25->li,l126( *l22));}l4 l324 l113(lg lw*
la,lg lv*l22,lg lv*l25){lj(l22->li[0]!=l25->li[0]||l22->li[1]!=l25->
li[1]);}l4 lx l301(lg lw*la,lg l240*l53){le l44;lf(l223[la->lr.lo]<=
32){l44=l53->li[0]&(0xffffffff>>(32-l223[la->lr.lo]));}lk{l44=l53->li
[0];l44^=l53->li[1]&(0xffffffff>>(64-l223[la->lr.lo]));}l44=l44^l44>>
16;l44=l44^l44>>8;l44=l44^l44>>4;l44=l44^l44>>2;l44=l44^l44>>1;lj(((
lx)l44)&1);}l4 l82 l201(lg lw*la,l19 l240*l53){le l44;lg l14*lc;lc=&(
la->lr);lf(l194[lc->lo]<0){lj;}l44=l301(la,l53);lf(l194[lc->lo]<32){
l53->li[0]^=l44<<l194[lc->lo];}lk{l53->li[1]^=l44<<(l194[lc->lo]-32);
}}l4 l67 l79(le l120){l67 l55=0;lf(l120==0)lj(0x10000000);lf(l120&
0xFFFF0000){l55+=16;l120>>=16;}lf(l120&0xFF00){l55+=8;l120>>=8;}lf(
l120&0xf0){l55+=4;l120>>=4;}l55+=l302[l120];lj(l55);}l4 l82 l307(lg lw
 *la,l11 lv*ld,lg le lb,lg le ls,lg le l30,lg le l0){l67 l54;lg l14*
lc;lc=&(la->lr);lf(lc->lo==l43){;;ld->li[0]=(l0>>2)&0x1fff;ld->li[0]
|=(l30&0xffff)<<13;ld->li[0]|=0x40000000;}lk lf(lc->lo==l48||lc->lo==
l51){lf(ls==4){l54=l79(lc->l141[lc->l59[lb]]-1)-16;lf(l54>0){;ld->li[
0]=l0>>l54;}lk lf(l54<0){ld->li[0]=l0<<-l54;}}lk{ld->li[0]=l0&
0x1fffff;}lf(ls==4){ld->li[0]|=l30<<17;ld->li[1]=l30>>15;}lk{ld->li[0
]|=l30<<21;}lf(ls==4){ld->li[1]|=0x4;}lk lf(ls==3){ld->li[0]|=1<<31;
ld->li[1]=0x6;}lk lf(ls==2){ld->li[1]=0x7;}l201(la,ld);}lk lf(lc->lo
==l50){;;ld->li[0]=(l0>>1)&0x1fff;ld->li[0]|=(l30&0xff)<<13;ld->li[0]
|=0x400000;ld->li[1]=0;}lk lf(lc->lo==l66){;;ld->li[0]=(l0>>1)&0x3fff
;ld->li[0]|=(l30&0xf)<<14;ld->li[0]|=0x180000;ld->li[1]=0;}lk lf(lc->
lo==l39){;;ld->li[0]=(l0>>1)&0x3ffff;ld->li[0]|=(l30&0xffff)<<18;ld->
li[1]=0;ld->li[1]|=(l30&0xffff)>>14;}lk{;}}l4 l82 l289(lg lw*la,l11 lv
 *ld,lg le lb,lg le ls,lg le l9,lg le l0){l67 l54;lg l14*lc;lc=&(la->
lr);lf(lc->lo==l43){;;ld->li[0]=(l0>>1)&0x3fff;ld->li[0]|=l9<<14;ld->
li[1]=0;}lk lf(lc->lo==l48||lc->lo==l51){;l54=l79(lc->l141[lc->l59[lb
]]-1)-17;lf(l54>0){;ld->li[0]=l0>>l54;}lk lf(l54<0){ld->li[0]=l0<<-
l54;}ld->li[0]|=l9<<18;ld->li[1]=l9>>14;l201(la,ld);}lk lf(lc->lo==
l50){;ld->li[0]=l0&0x3fff;ld->li[0]|=l9<<14;ld->li[1]=0;}lk{;}}l4 l82
l311(lg lw*la,l11 lv*ld,lg le lb,lg le l46,lg le l6,lg le l0){lg l14*
lc;lc=&(la->lr);lf(lc->lo==l43){;;;;ld->li[0]=(l0>>1)&0x3fff;ld->li[0
]|=(l6<<(21-l46));ld->li[0]|=((1<<(l46-1))-1)<<(28-l46);ld->li[0]|=
0x60000000;ld->li[1]=0;}lk lf(lc->lo==l48||lc->lo==l51){ld->li[0]=l0&
0x1fffff;ld->li[0]|=(l6<<(30-l46))&0x3fffffff;ld->li[0]|=1<<(29-l46);
ld->li[1]=0x6;l201(la,ld);}lk lf(lc->lo==l50){;;;;ld->li[0]=(l0>>1)&
0x1fff;ld->li[0]|=(l6<<(20-l46))&0xfffff;ld->li[0]|=1<<(19-l46);ld->
li[0]|=0x600000;ld->li[1]=0;}lk lf(lc->lo==l66){;;;;lf(l46==5){ld->li
[0]=(l0>>1)&0x3fff;ld->li[0]|=(l6<<(19-l46))&0x7ffff;ld->li[0]|=
0x100000;ld->li[1]=0;}lk{ld->li[0]=(l0>>1)&0x3fff;ld->li[0]|=(l6<<(18
-l46))&0x3ffff;ld->li[0]|=(l46-1)<<18;ld->li[1]=0;}}lk lf(lc->lo==l39
){le l178;;;l178=l6<<1;l178|=1;l178=l178<<(7-l46);ld->li[0]=(l0)&
0x1fffff;ld->li[0]|=((l178&0xff)<<21);ld->li[0]|=(((l46-1)&0x7)<<29);
ld->li[1]=0x6;}lk{;}}l4 l82 l321(l19 lv*ld,lg le l211){;ld->li[1]&=
0x00000fff;ld->li[1]|=l211<<12;}l82 l367(l19 lv*ld){ld->li[1]&=
0x00000fff;}l4 l82 l219(lg lw*la,l11 lv*ld,lg le lb,lg le l73,lg le
l211){lg l14*lc;lc=&(la->lr);lf(lc->lo==l43){;ld->li[0]=l73&0x7fff;ld
->li[0]|=0x70000000;ld->li[1]=0x0;}lk lf(lc->lo==l48||lc->lo==l51){;
ld->li[0]=l73&0xfffff;ld->li[0]|=1<<31;ld->li[1]=0x7;l201(la,ld);}lk
lf(lc->lo==l50){;lf(lb>=5){ld->li[0]=l73&0x3fff;}lk{ld->li[0]=l73&
0x3fff;ld->li[0]|=0x700000;}ld->li[1]=0x0;}lk lf(lc->lo==l66){;ld->li
[0]=l73&0x3fff;ld->li[0]|=0x1c0000;ld->li[1]=0x0;}lk lf(lc->lo==l39){
;ld->li[0]=(l73&0xffffffff);ld->li[1]=7;}lk{;}l321(ld,l211);}l4 l140
l16(lg lw*la,lg lv*ld,lg le lb){lf(la->lr.lo==l43){lf(lb>=5)lj(l10);
lk lf((ld->li[0]&0x40000000)==0)lj(l75);lk lf((ld->li[0]&0x60000000)==
0x40000000)lj(l36);lk lf((ld->li[0]&0x70000000)==0x60000000)lj(l13);
lk lf((ld->li[0]&0x70000000)==0x70000000)lj(l10);}lk lf(la->lr.lo==
l48||la->lr.lo==l51){lf(lb==3)lj(l131);lk lf((ld->li[1]&0x4)==0)lj(
l75);lk lf((ld->li[1]&0x6)==0x4)lj(l36);lk lf((ld->li[1]&0x7)==0x6&&(
ld->li[0]&0x80000000)==0)lj(l13);lk lf((ld->li[1]&0x7)==0x6&&(ld->li[
0]&0x80000000)==0x80000000)lj(l77);lk lf((ld->li[1]&0x7)==0x7&&(ld->
li[0]&0x80000000)==0)lj(l87);lk lf((ld->li[1]&0x7)==0x7&&(ld->li[0]&
0x80000000)==0x80000000)lj(l10);}lk lf(la->lr.lo==l50){lf(lb>=5){lj(
l10);}lk lf((ld->li[0]&0x400000)==0)lj(l118);lk lf((ld->li[0]&
0x600000)==0x400000)lj(l77);lk lf((ld->li[0]&0x700000)==0x600000)lj(
l13);lk lf((ld->li[0]&0x700000)==0x700000)lj(l10);}lk lf(la->lr.lo==
l66){lf((ld->li[0]&0x100000)==0)lj(l13);lk lf((ld->li[0]&0x180000)==
0x100000)lj(l13);lk lf((ld->li[0]&0x1c0000)==0x180000)lj(l87);lk lf((
ld->li[0]&0x1c0000)==0x1c0000)lj(l10);}lk lf(la->lr.lo==l39){lf((ld->
li[1]&0x4)==0x0)lj(l36);lk lf((ld->li[1]&0x6)==0x4)lj(l115);lk lf((ld
->li[1]&0x7)==0x6)lj(l13);lk lf((ld->li[1]&0x7)==0x7)lj(l10);};lj(
l163);}l4 lx l130(lg lw*la,lg lv*ld,lg le lb){lf(lb>=5)lj 1;lf((ld->
li[0]&0x70000000)==0x70000000)lj 1;lj 0;}l82 l373(lg lw*la,lg le lb,
lg l329*l27,lg le l327,l11 le*l111){le l143;lv ld;l5(l143=0;l143<l327
;++l143){ld.li[l143]=l27[l143];l111[l143]=l27[l143];}lf(l130(la,&ld,
lb)){lf(la->lr.lo==l43){lf(lb>=5){l111[0]= *l27&0x7fff;}lk{l111[0]= *
l27&0x7fff;l111[0]|=0x70000000;}}lk lf(la->lr.lo==l66){lf(lb>=5){l111
[0]= *l27&0x3fff;}lk{l111[0]= *l27&0x3fff;l111[0]|=0x700000;}}lk lf(
la->lr.lo==l39){l111[0]=l27[0];l111[1]=7;}}}l4 le l265(lg lw*la,lg le
lb,l11 le*l12){lg l14*lc;le l61=0;le l114;le l52;l52= *l12;lc=&(la->
lr);lf(lc->lo==l39&&lb>0){l114=(lc->l90-1)/(lc->l171-1);lf(l114==1){
l61=lc->l59[lb];}lk lf(l114==2){l61=lc->l59[lb]+((l52>>19)&0x1);}lk lf
(l114==4){l61=lc->l59[lb]+((l52>>19)&0x3);}lk{;} *l12=( *l12&0x7ffff);
}lk{l61=lc->l59[lb];}lj l61;}l4 le l98(lg lw*la,lg le lb,lg le l12,
l11 lv*ld,lg le l71){l67 l64;le l52;lv*l208;l153*l85;le lh;lg l14*lc;
le l89[2* (1<<(8))];l23(0);lc=&(la->lr);l64=l71;l208=ld;l52=l12;lh=lc
->l125(lc->l26,lc->l40,lc->l59[lb],&l85);lq(lh,10,ln);lh=l377(l85,l52
,l71,l89);lq(lh,20,ln);l5(l64=0;l64<l71;++l64){l208[l64].li[0]=l89[2*
l64];l208[l64].li[1]=l89[2*l64+1];}ln:l29("\x65\x72\x72\x6f\x72\x20"
"\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c"
"\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x65\x61\x64\x5f\x72\x6f\x77\x73"
"\x28\x29",0,0);}l4 le l184(lg lw*la,lg le lb,lg le l12,lg lv*ld,lg le
l71){lv l299;le l64;le l52;le l254=0;l153*l85;l329 l215[2],l218[2],
l109=0,l103[2]={0};le lh;lg l14*lc;le l89[2* (1<<(8))];l23(0);l248(&
l299,0x0,l126(lv));lc=&(la->lr);lf(l133){l93 ln;}l52=l12;lh=lc->l125(
lc->l26,lc->l40,lc->l59[lb],&l85);lq(lh,10,ln);l5(l64=0;l64<l71;l64++
){lf(lb>=5&&lc->lo==l43){l89[2*l64]=l103[0]=ld->li[0]&0x7fff;l89[2*
l64+1]=l103[1]=ld->li[1];}lk lf(lb>=5&&lc->lo==l50){l103[0]=ld->li[0]
&0x3fff;l89[2*l64+1]=l103[1];}lk{l89[2*l64]=l103[0]=ld->li[0];l89[2*
l64+1]=l103[1]=ld->li[1];}lf(l109>1){++l52;++ld;l226;}lh=l406(l85,l52
,l215);lq(lh,20,ln);lf(l215[0]!=l103[0]||l215[1]!=l103[1]){lf(l109==0
){l218[0]=l103[0];l218[1]=l103[1];l254=l52;}++l109;}++l52;++ld;}lf(
l109==1){lh=l297(l85,l254,1,l218);lq(lh,20,ln);}lk lf(l109>1){lh=l297
(l85,l12,l71,l89);lq(lh,20,ln);}ln:l29("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70"
"\x6d\x5f\x6d\x6e\x67\x72\x5f\x77\x72\x69\x74\x65\x5f\x72\x6f\x77\x73"
"\x28\x29",0,0);}lx l282(lg lw*la,l11 lv*l22,lg lv*l25,lg le lb){lg
l14*lc;lc=&(la->lr);lf(lc->lo==l43){lz(l16(la,l25,lb)){ll l75:ll l13:
lj((lx)((l25->li[0]&~0x3fff)==(l22->li[0]&~0x3fff)));ll l36:lj((lx)((
l25->li[0]&~0x1fff)==(l22->li[0]&~0x1fff)));ll l10:lj((lx)(l130(la,
l22,lb)));l17:;lj(l62);}}lk lf(lc->lo==l48||lc->lo==l51){lz(l16(la,
l25,lb)){ll l75:lj(lx)(((l25->li[1]&0x7)==(l22->li[1]&0x7))&&((l25->
li[0]&~0x3ffff)==(l22->li[0]&~0x3ffff)));ll l36:lj(lx)(((l25->li[1]&
0x7)==(l22->li[1]&0x7))&&((l25->li[0]&~0x1ffff)==(l22->li[0]&~0x1ffff
)));ll l13:lj(lx)(((l25->li[1]&0x7)==(l22->li[1]&0x7))&&((l25->li[0]&
~0x1ffff)==(l22->li[0]&~0x1ffff)));ll l77:lj(lx)(((l25->li[1]&0x7)==(
l22->li[1]&0x7))&&((l25->li[0]&~0x1ffff)==(l22->li[0]&~0x1ffff)));ll
l87:lj(lx)(((l25->li[1]&0x7)==(l22->li[1]&0x7))&&((l25->li[0]&~
0x1ffff)==(l22->li[0]&~0x1ffff)));ll l10:lj(lx)(l16(la,l22,lb)==l10);
ll l131:lj(lx)(l16(la,l22,lb)==l131);l17:;lj(l62);}}lk lf(lc->lo==l50
){lz(l16(la,l25,lb)){ll l118:lj((lx)((l25->li[0]&~0x3fff)==(l22->li[0
]&~0x3fff)));ll l77:lj((lx)((l25->li[0]&~0x1fff)==(l22->li[0]&~0x1fff
)));ll l13:lj((lx)((l25->li[0]&~0x1fff)==(l22->li[0]&~0x1fff)));ll l10
:lj((lx)(l16(la,l22,lb)==l10));l17:;lj(l62);}}lk lf(lc->lo==l66){lz(
l16(la,l25,lb)){ll l87:ll l13:ll l10:lj((lx)((l25->li[0]&0x1fc000)==(
l22->li[0]&0x1fc000)));l17:;lj(l62);}}lk lf(lc->lo==l39){lz(l16(la,
l25,lb)){ll l115:ll l36:lj(lx)(((l25->li[0]&0xfffa0000)==(l22->li[0]&
0xfffa0000))&&((l25->li[1]&0x7)==(l22->li[1]&0x7)));ll l13:lj(lx)(((
l25->li[0]&0xffe00000)==(l22->li[0]&0xffe00000))&&((l25->li[1]&0x7)==
(l22->li[1]&0x7)));ll l10:lj((lx)(l16(la,l22,lb)==l10));l17:;lj(l62);
}}lk{;lj(l62);}}l67 l91(lg lw*la,lg lv*ld,lg le lb){l67 l54;lg l14*lc
;le l229=0;lc=&(la->lr);lf(lc->lo==l43){lz(l16(la,ld,lb)){ll l75:ll
l13:lj((ld->li[0]&0x3fff) *2);ll l36:lj((ld->li[0]&0x1fff) *4);ll l10
:lj(-1);l17:;lj(-1);}}lk lf(lc->lo==l48||lc->lo==l51){lz(l16(la,ld,lb
)){ll l75:l54=l79(lc->l141[lc->l59[lb]]-1)-17;lj(l54>0?(ld->li[0]&
0x3ffff)<<l54:(ld->li[0]&0x3ffff)>>l54);ll l36:l54=l79(lc->l141[lc->
l59[lb]]-1)-16;lj(l54>0?(ld->li[0]&0x1ffff)<<l54:(ld->li[0]&0x1ffff)>>
l54);ll l13:lj(ld->li[0]&0x1fffff);ll l77:lj(ld->li[0]&0x1fffff);ll
l87:lj(ld->li[0]&0x1fffff);ll l10:lj(-1);ll l131:lj(-1);l17:;lj(-1);}
}lk lf(lc->lo==l50){lz(l16(la,ld,lb)){ll l118:lj(ld->li[0]&0x3fff);ll
l77:ll l13:lj((ld->li[0]&0x1fff) *2);ll l10:lj(-1);l17:;lj(-1);}}lk lf
(lc->lo==l66){lz(l16(la,ld,lb)){ll l87:ll l13:lj((ld->li[0]&0x3fff) *
2);ll l10:lj(-1);l17:;lj(-1);}}lk lf(lc->lo==l39){le l180=0;le l224=(
lc->l90-1)/(lc->l171-1);lf(l224==1){l180=l229&0x0;}lk lf(l224==2){
l180=l229&0x1;}lk lf(l224==4){l180=l229&0x3;}lk{;}lz(l16(la,ld,lb)){
ll l115:ll l36:lj((l180<<19)|((ld->li[0]&0x3ffff) *2));ll l13:lj(ld->
li[0]&0x1fffff);ll l10:lj(-1);l17:;lj(l62);}}lk{;lj(-1);}}l67 l304(lg
lw*la,l19 lv*ld,lg le lb,lg le l159){lf(la->lr.lo==l43){lz(l16(la,ld,
lb)){ll l75:ll l13:ld->li[0]=((ld->li[0]&~0x3fff)|l159/2);lj ld->li[0
];ll l36:ld->li[0]=((ld->li[0]&~0x1fff)|l159/4);lj ld->li[0];ll l10:
lj(-1);l17:lj(-1);}}lk lf(la->lr.lo==l50){lz(l16(la,ld,lb)){ll l77:ll
l13:ld->li[0]=((ld->li[0]&~0x1fff)|l159/2);lj ld->li[0];ll l118:lj ld
->li[0]=((ld->li[0]&~0x3fff)|l159);ll l10:lj(-1);l17:lj(-1);}}lk lf(
la->lr.lo==l39){lz(l16(la,ld,lb)){ll l36:ll l115:ld->li[0]=((ld->li[0
]&~0x3ffff)|l159/2);lj ld->li[0];ll l13:ld->li[0]=((ld->li[0]&~
0x1fffff)|l159);lj ld->li[0];ll l10:lj(-1);l17:lj(-1);}}lk{lj(-1);}}
l4 le l231(lg lw*la,lg le lb,lg le l9,lg le ls){le ly;le l55;le l146=
l9;l146<<=1;l146|=1;l5(l55=0,ly=0;ly<(le)(1<<ls);ly++){lz((l146>>ly)&
0x3){ll 0x0:ll 0x2:l7;ll 0x1:l55++;l7;ll 0x3:;l55+=1<<(la->lu.lm[lb+2
]-la->lu.lm[lb+1]-ls);l7;}}lj(l55);}l4 le l296(lg lw*la,lg le lb,lg le
l9,lg le ls){le ly;le l55;le l146=l9;le l230;lf((la->lu.lm[lb+2]-la->
lu.lm[lb+1])<=ls)l230=1;lk l230=1<<(la->lu.lm[lb+2]-la->lu.lm[lb+1]-
ls);l146<<=1;l146|=1;l5(l55=0,ly=0;ly<((le)(1<<ls)-1);ly++){lz((l146
>>ly)&0x1){ll 0x0:l7;ll 0x1:l55+=l230;l7;}}lj(l55);}l4 le l136(lg lw*
la,lg le lb,lg le l30,lg le ls){le ly;le l55;l5(l55=0,ly=0;ly<(le)(1
<<ls);){lf((l30>>ly)&1){lf(la->lu.lm[lb+2]-la->lu.lm[lb+1]<ls)l55++;
lk l55+=1<<(la->lu.lm[lb+2]-la->lu.lm[lb+1]-ls);}lk{l55++;}lf(ls>la->
lu.lm[lb+2]-la->lu.lm[lb+1])ly+=1<<(ls-(la->lu.lm[lb+2]-la->lu.lm[lb+
1]));lk ly++;}lj(l55);}le l266(lg lw*la,lg lv*ld,lg le lb){lg l14*lc;
lc=&(la->lr);lf(lc->lo==l43){lz(l16(la,ld,lb)){ll l75:lj(l231(la,lb,
ld->li[0]>>14,4));ll l36:lj(l136(la,lb,ld->li[0]>>13,4));ll l13:lj(2);
ll l10:lj(0);l17:;lj(0);}}lk lf(lc->lo==l48||lc->lo==l51){lz(l16(la,
ld,lb)){ll l75:lj(l231(la,lb,(ld->li[0]>>18)|(ld->li[1]<<14),4));ll
l36:lj(l136(la,lb,(ld->li[0]>>17)|(ld->li[1]<<15),4));ll l13:lj(2);ll
l77:lj(l136(la,lb,ld->li[0]>>21,3));ll l87:lj(l136(la,lb,ld->li[0]>>
21,2));ll l10:lj(0);ll l131:lj(0);l17:lj(0);}}lk lf(lc->lo==l50){lz(
l16(la,ld,lb)){ll l118:lj(l231(la,lb,ld->li[0]>>14,3));ll l77:lj(l136
(la,lb,ld->li[0]>>13,3));ll l13:lj(2);ll l10:lj(0);l17:;lj(0);}}lk lf
(lc->lo==l66){lz(l16(la,ld,lb)){ll l87:lj(l136(la,lb,ld->li[0]>>14,2));
ll l13:lj(2);ll l10:lj(0);l17:;lj(0);}}lk lf(lc->lo==l39){le l236=((
ld->li[1]&0x3)<<14)+(ld->li[0]>>18);lz(l16(la,ld,lb)){ll l36:lj(l136(
la,lb,l236,4));ll l13:lj(2);ll l115:lj(l296(la,lb,l236,4));ll l10:lj(
0);l17:;lj(0);}}lk{;lj(0);}}l4 le l132(lg lw*la,lg lv*ld,lg le lb,lg
le ls,l11 lv*lt){le ly,l47;le l0;le l1;le l30=0;le lh;le l21;le l154;
lg l14*lc;lg l83*lp;l23(0);lc=&(la->lr);lp=&(la->lu);lf((la->lr.lo==
l43||la->lr.lo==l39)&&lp->lm[lb+2]-lp->lm[lb+1]<5){;l24;}l154=1<<ls;
l0=l91(la,ld,lb);lf(lp->lm[lb+2]-lp->lm[lb+1]<ls){l1=1;}lk{l1=1<<(lp
->lm[lb+2]-lp->lm[lb+1]-ls);}lf(lc->lo==l48||lc->lo==l51){lf(ls==4)l30
=ld->li[0]>>17;lk l30=ld->li[0]>>21;}lk lf(lc->lo==l50){l30=ld->li[0]
>>13;}lk lf(lc->lo==l43){l30=ld->li[0]>>13;}lk lf(lc->lo==l66){l30=ld
->li[0]>>14;}lk lf(lc->lo==l39){l30=(ld->li[0]>>18)+((ld->li[1]&0x3)<<
14);}lk{;}l30&=(1<<l154)-1;l21=0;l5(l47=0;l47<l154;){lf((l30>>l47)&1){
lh=l98(la,lb+1,l0,&lt[l21],l1);lq(lh,10,ln);l0+=l1;}lk{lh=l98(la,lb+1
,l0,&lt[l21],1);lq(lh,20,ln);l5(ly=1;ly<l1;ly++)l45(&lt[l21+ly],&lt[
l21]);l0+=1;}l21+=l1;lf(ls>lp->lm[lb+2]-lp->lm[lb+1])l47+=1<<(ls-(lp
->lm[lb+2]-lp->lm[lb+1]));lk l47++;}ln:l29("\x65\x72\x72\x6f\x72\x20"
"\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c"
"\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f\x74\x6f\x5f\x66\x75"
"\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x65\x6c\x69\x64\x65\x5f\x6c\x67"
"\x6e\x28\x29",0,0);}l4 le l222(lg lw*la,lg lv*ld,lg le lb,lg le ls,
l11 lv*lt){le ly,l47;le l0;le l1;le l9=0;le lh;le l21;le l86;lg l14*
lc;lg l83*lp;l23(0);lc=&(la->lr);lp=&(la->lu);l0=l91(la,ld,lb);l86=1
<<ls;lf(lp->lm[lb+2]-lp->lm[lb+1]<ls){l1=1;}lk{l1=1<<(lp->lm[lb+2]-lp
->lm[lb+1]-ls);}lf(lc->lo==l43){l9=ld->li[0]>>14;}lk lf(lc->lo==l48||
lc->lo==l51)l9=(ld->li[0]>>18)|(ld->li[1]<<14);lk lf(lc->lo==l50){l9=
ld->li[0]>>14;}lk{;}l9&=(1<<l86)-1;l21=0;l9<<=1;l9|=1;l5(l47=0;l47<
l86;){lz((l9>>l47)&0x3){ll 0x0:ll 0x2:l5(ly=0;ly<l1;ly++){l45(&lt[l21
+ly],&lt[l21-1]);}l7;ll 0x1:lh=l98(la,lb+1,l0,&lt[l21],1);lq(lh,10,ln
);l5(ly=1;ly<l1;ly++){l45(&lt[l21+ly],&lt[l21]);}l0+=1;l7;ll 0x3:lh=
l98(la,lb+1,l0,&lt[l21],l1);lq(lh,20,ln);l0+=l1;l7;}l21+=l1;lf(ls>lp
->lm[lb+2]-lp->lm[lb+1])l47+=1<<(ls-(lp->lm[lb+2]-lp->lm[lb+1]));lk
l47++;}ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64"
"\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72"
"\x5f\x72\x6f\x77\x5f\x74\x6f\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64"
"\x65\x5f\x63\x6f\x64\x65\x5f\x6c\x67\x6e\x28\x29",0,0);}l4 le l317(
lg lw*la,lg lv*ld,lg le lb,lg le ls,l11 lv*lt){le ly,l47;le l0;le l1;
le l9=0;le lh;le l21;le l86;lg l14*lc;lg l83*lp;l23(0);lc=&(la->lr);
lp=&(la->lu);l0=l91(la,ld,lb);l86=1<<ls;lf(lp->lm[lb+2]-lp->lm[lb+1]<
ls){l1=1;}lk{l1=1<<(lp->lm[lb+2]-lp->lm[lb+1]-ls);}lf(lc->lo==l39){l9
=(ld->li[0]>>18)+((ld->li[1]&0x1)<<14);}lk{;}l9&=(1<<l86)-1;l21=0;l9
<<=1;l9|=1;l5(l47=0;l47<l86;){lz((l9>>l47)&0x1){ll 0x0:l5(ly=0;ly<l1;
ly++){l45(&lt[l21+ly],&lt[l21-1]);}l7;ll 0x1:lh=l98(la,lb+1,l0,&lt[
l21],l1);lq(lh,10,ln);l0+=l1;l7;}l21+=l1;lf(ls>lp->lm[lb+2]-lp->lm[lb
+1])l47+=1<<(ls-(lp->lm[lb+2]-lp->lm[lb+1]));lk l47++;}ln:l29("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69"
"\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f"
"\x74\x6f\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x63\x6f\x64"
"\x65\x5f\x31\x5f\x6c\x67\x6e\x28\x29",0,0);}l4 l8 l246(lg lw*la,lg lv
 *ld,lg le lb){lf(la->lr.lo==l43){le l6;l6=(ld->li[0]>>14)&0x7f;;l6&=
0x40|((ld->li[0]>>21)&0x3f);l6<<=1;l6|=(((~ld->li[0]>>21)&0x3f)+1);lj
(l6);}lk lf(la->lr.lo==l48||la->lr.lo==l51){lj((ld->li[0]>>21)&0x1ff);
}lk lf(la->lr.lo==l50){lj((ld->li[0]>>13)&0x7f);}lk lf(la->lr.lo==l66
){le l6;lf((ld->li[0]&0x100000)==0){l6=(ld->li[0]>>14)&0xf;l6>>=3-((
ld->li[0]>>18)&0x3);l6<<=3-((ld->li[0]>>18)&0x3);l6<<=1;l6|=1<<(3-((
ld->li[0]>>18)&0x3));l6<<=1;}lk{l6=(ld->li[0]>>14)&0x1f;l6<<=1;l6|=1;
}lj(l6);}lk lf(la->lr.lo==l39){le l6;le l207;le l213;l6=(ld->li[0]>>
21)&0xff;l213=(ld->li[0]>>29)&0x7;l207=l6|(1<<(6-l213));l207=l207&(
0xff<<(6-l213));;lj l6;};lj(l8)(-1);}l4 le l177(lg lw*la,lg lv*ld,lg
le lb,l11 lv*lt){le l0;le l32;le lh;le l21;le l6=0;le l46=0;le l150;
le l209;le l199;lg l14*lc;lg l83*lp;l23(0);lc=&(la->lr);lp=&(la->lu);
l0=l91(la,ld,lb);l32=1<<(lp->lm[lb+2]-lp->lm[lb+1]);l6=l246(la,ld,lb);
lf(lc->lo==l43){l46=7;}lk lf(lc->lo==l48||lc->lo==l51){l46=8;}lk lf(
lc->lo==l50){l46=6;}lk lf(lc->lo==l66){l46=5;}lk lf(lc->lo==l39){l46=
7;}lk{;}lf((lp->lm[lb+2]-lp->lm[lb+1])<l46){l6>>=l46-(lp->lm[lb+2]-lp
->lm[lb+1]);}lk lf((lp->lm[lb+2]-lp->lm[lb+1])>l46){l6<<=(lp->lm[lb+2
]-lp->lm[lb+1])-l46;}l150=((l6-1)^l6);l150>>=1;l6>>=1;l6&=~l150;l209=
l6;lf(l6==0){l199=l6+l150+1;}lk{l199=0;}l5(l21=0;l21<l32;l21++){lf((
l21&~l150)==(l6&~l150)){lf(l21==l209){lh=l98(la,lb+1,l0+1,&lt[l21],1);
lq(lh,10,ln);}lk{l45(&lt[l21],&lt[l209]);}}lk{lf(l21==l199){lh=l98(la
,lb+1,l0,&lt[l21],1);lq(lh,20,ln);}lk{l45(&lt[l21],&lt[l199]);}}}ln:
l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70"
"\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f"
"\x77\x5f\x74\x6f\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x63"
"\x6f\x6d\x70\x61\x72\x65\x28\x29",0,0);}l4 l82 l165(lg lw*la,lg lv*
ld,lg le lb,l19 lv*lt){le l32;le l47;lg l83*lp;lp=&(la->lu);l32=1<<(
lp->lm[lb+2]-lp->lm[lb+1]);l5(l47=0;l47<l32;l47++){l45(&lt[l47],ld);}
}l4 le l262(lg lw*la,lg lv*ld,lg le lb,l19 lv*lt,l19 l140*l104){le lh
;lg l14*lc;l23(0);lc=&(la->lr);lf(lc->lo==l43){lz(l16(la,ld,lb)){ll
l75:lh=l222(la,ld,lb,4,lt);lq(lh,10,ln); *l104=l75;l24;ll l36:lh=l132
(la,ld,lb,4,lt);lq(lh,20,ln); *l104=l36;l24;ll l13:lh=l177(la,ld,lb,
lt);lq(lh,30,ln); *l104=l13;l24;ll l10:l165(la,ld,lb,lt); *l104=l10;
l24;l17:;l24;}}lk lf(lc->lo==l48||lc->lo==l51){lz(l16(la,ld,lb)){ll
l75:lh=l222(la,ld,lb,4,lt);lq(lh,40,ln);l24;ll l36:lh=l132(la,ld,lb,4
,lt);lq(lh,50,ln);l24;ll l13:lh=l177(la,ld,lb,lt);lq(lh,60,ln);l24;ll
l77:lh=l132(la,ld,lb,3,lt);lq(lh,70,ln);l24;ll l87:lh=l132(la,ld,lb,2
,lt);lq(lh,80,ln);l24;ll l10:ll l131:l165(la,ld,lb,lt);l24;l17:;l24;}
}lk lf(lc->lo==l50){lz(l16(la,ld,lb)){ll l118:lh=l222(la,ld,lb,3,lt);
lq(lh,90,ln);l24;ll l77:lh=l132(la,ld,lb,3,lt);lq(lh,100,ln);l24;ll
l13:lh=l177(la,ld,lb,lt);lq(lh,110,ln);l24;ll l10:l165(la,ld,lb,lt);
l24;l17:;l24;}}lk lf(lc->lo==l66){lz(l16(la,ld,lb)){ll l87:lh=l132(la
,ld,lb,2,lt);lq(lh,120,ln);l24;ll l13:lh=l177(la,ld,lb,lt);lq(lh,130,
ln);l24;ll l10:l165(la,ld,lb,lt);l24;l17:;l24;}}lk lf(lc->lo==l39){lz
(l16(la,ld,lb)){ll l36:lh=l132(la,ld,lb,4,lt);lq(lh,140,ln);l24;ll l13
:lh=l177(la,ld,lb,lt);lq(lh,150,ln);l24;ll l115:lh=l317(la,ld,lb,4,lt
);lq(lh,160,ln);l24;ll l10:l165(la,ld,lb,lt);l24;l17:;l24;}}lk{;};ln:
l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70"
"\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f"
"\x77\x5f\x74\x6f\x5f\x65\x6c\x69\x64\x65\x28\x29",0,0);}l67 l288(lg
lw*la,l11 lv*ld,lg le lb,lg le ls,l19 lv*lt,l19 lv*l129,lg le l182,lg
l97 l0){le l32;le l1;le l33;le l60;lv l175;lx l139=1;le l30;lg l83*lp
;lp=&(la->lu);l32=1<<(lp->lm[lb+2]-lp->lm[lb+1]);lf(lp->lm[lb+2]-lp->
lm[lb+1]<ls){lj(-1);}lk{l1=1<<(lp->lm[lb+2]-lp->lm[lb+1]-ls);}l30=0;
l5(l33=0,l60=0;l33<l32;){lf(l33%l1==0){l139=1;l45(&l175,&lt[l33]);}lk
lf(!l139||l113(la,&l175,&lt[l33])!=0)l139=0;l45(&l129[l60],&lt[l33]);
l33++;l60++;lf(!l139&&l60>l182){lj 10000;}lf(l33%l1==0){lf(lp->lm[lb+
2]-lp->lm[lb+1]>=ls){l30|=(~l139&1)<<(((l33-l1)>>(lp->lm[lb+2]-lp->lm
[lb+1]-ls)));}lk{l30|=(~l139&1)<<(((l33-l1)<<(ls-(lp->lm[lb+2]-lp->lm
[lb+1]))));}lf(l139){l60-=l1-1;}lf(l60>l182){lj 10000;}}}l307(la,ld,
lb,ls,l30,l0);lj(l60);}l67 l303(lg lw*la,l11 lv*ld,lg le lb,lg le ls,
l19 lv*lt,l19 lv*l129,lg le l182,lg l97 l0){le l32;le l1;le l33;le l60
;le l9;le l100=0;le l235=0;le l192=0;lg l83*lp;lp=&(la->lu);l32=1<<(
lp->lm[lb+2]-lp->lm[lb+1]);lf(lp->lm[lb+2]-lp->lm[lb+1]<ls){l1=1;}lk{
l1=1<<(lp->lm[lb+2]-lp->lm[lb+1]-ls);}l9=1;l5(l33=0;l33<l32;l33++){lf
(l33%l1==0){lf(lp->lm[lb+2]-lp->lm[lb+1]>ls)l100=l33>>(lp->lm[lb+2]-
lp->lm[lb+1]-ls);lk l100=l33<<(ls-(lp->lm[lb+2]-lp->lm[lb+1]));l192=
l33;l235=l192-1;}lk{lf(l113(la,&lt[l192],&lt[l33])!=0){l9|=0x2<<l100;
}}lf(l192==0||l113(la,&lt[l235],&lt[l33])!=0)l9|=0x1<<l100;}l5(l60=0,
l100=0,l33=0;l100<(le)(1<<ls);){lf(l9&(0x1<<l100)){lf(l9&(0x2<<l100)){
le ly;l5(ly=0;ly<l1;ly++){l45(&l129[l60],&lt[l33]);l60++;l33++;}}lk{
l45(&l129[l60],&lt[l33]);l60++;lf(l60>l182){lj l60;}l33+=l1;}}lk{l33
+=l1;}lf(ls>lp->lm[lb+2]-lp->lm[lb+1])l100+=1<<(ls-(lp->lm[lb+2]-lp->
lm[lb+1]));lk l100++;}l9>>=1;l289(la,ld,lb,ls,l9,l0);lj(l60);}l4 l67
l310(lg lw*la,l11 lv*ld,lg le lb,l19 lv*lt,l19 lv*l129,lg l97 l0){le
l32;le l76;lv l173[2];le l58[2];le l151[2];le l69;lg l83*lp;lg l14*lc
;le l242;lc=&(la->lr);lp=&(la->lu);l242=lp->lm[lb+2]-lp->lm[lb+1];l32
=1<<l242;l45(&l173[0],&lt[0]);l58[0]=1;l151[0]=0;l58[1]=0;l5(l76=1;
l76<l32&&l113(la,&lt[l76],&l173[0])==0;l76++){l58[0]++;}lf(l76>=l32){
lj(-1);}l45(&l173[1],&lt[l76]);l151[1]=l76;l58[1]++;l5(l76++;l76<l32
&&l113(la,&lt[l76],&l173[1])==0;l76++){l58[1]++;}l5(;l76<l32&&l113(la
,&lt[l76],&l173[0])==0;l76++){l58[0]++;}lf(l76<l32){lj(-1);}lf(l58[0]
<l58[1])l69=0;lk l69=1;lf(((l58[l69]-1)&l58[l69])!=0||((l58[l69]-1)&
l151[l69])!=0){lj(-1);}lz(lc->lo){ll l43:lf(l79(l32/l58[l69])>7)lj(-1
);l7;ll l48:ll l51:lf(l79(l32/l58[l69])>8)lj(-1);l7;ll l50:lf(l79(l32
/l58[l69])>6)lj(-1);l7;ll l66:lf(l79(l32/l58[l69])>5)lj(-1);l7;ll l39
:lf(l79(l32/l58[l69])>7)lj(-1);l7;l17:;l7;}l311(la,ld,lb,l79(l32/l58[
l69]),l151[l69]>>l79(l58[l69]),l0);l45(&l129[0],&lt[l151[1-l69]]);l45
(&l129[1],&lt[l151[l69]]);lj(2);}l4 l67 l316(lg lw*la,l11 lv*ld,lg le
lb,l19 lv*lt){le l32;le l200;lg l83*lp;lp=&(la->lu);l32=1<<(lp->lm[lb
+2]-lp->lm[lb+1]);l5(l200=l32-1;l200>0;--l200){lf(l113(la,&lt[0],&lt[
l200])!=0){lj-1;}}l45(ld,&lt[0]);lj(0);}l4 le l328(lg lw*la,l11 lv*ld
,lg le lb,l19 lv*lt,l11 l140*l127,lg l140 l123){l67 l65;l67 l88=-1;
l67 l183=-1;lv l210;lg l14*lc;lv l191[(1<<(8))];lv l234[(1<<(8))];lc=
&(la->lr);l65=(1<<(8))+1;lf(l123==l163){l88=l316(la,ld,lb,lt);lf(l88
!=-1&&l88<l65){l65=l88; *l127=l10;lj(l65);}}lf(l123==l10||l123==l13||
l123==l163){l88=l310(la,ld,lb,lt,l191,((l97)(-1)));lf(l88!=-1&&l88<
l65){l65=l88; *l127=l13;l202(lt,l191,l126(lv) *l65);lj(l65);}}lf(lc->
lo==l51||lc->lo==l43){l88=l303(la,ld,lb,4,lt,l191,10000,((l97)(-1)));
lf(l88!=-1&&l88<l65){l65=l88; *l127=l75;l45(&l210,ld);}l183=l288(la,
ld,lb,4,lt,l234,l88,((l97)(-1)));lf(l183!=-1&&l183<l65){l65=l183; *
l127=l36;l45(&l210,ld);l202(lt,l234,l126(lv) *l65);}lk{lf(l65>=0&&l65
<(1<<(8))){l202(lt,l191,l126(lv) *l65);}}}l45(ld,&l210);;lj(l65);}l4
l82 l250(lg lw*la,l11 lv*ld,lg le lb,l19 lv*lt,lg l97 l0,lg l97 l60){
l304(la,ld,lb,l0);l184(la,lb+1,l0,lt,l60);}l4 le l181(lg lw*la,lg le
l259,l11 lx*l216){l153*l101;l388 l147;le l31;lg l14*lc;le lh=l408;le
l232;l23(0);lc=&(la->lr);l383(&l147);lf(l259==0){l147.l298=2;}lk{l147
.l298=1;}l5(l31=1;l31<lc->l90;++l31){l232=la->lr.l344[l31];lh=lc->
l125(lc->l26,lc->l40,l31,&l101);lq(lh,10,ln);l147.l238[0]=((1<<la->lr
.l300[l232])+2);l147.l238[1]=l147.l238[0];lh=l365(l101,&l147,l216);lq
(lh,30,ln);lf(! *l216){lf(l259==2&&l161[l31]<=0){l161[l31]=0; *l216=1
;l226;}l161[l31]=0;l93 ln;}}ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x6d\x65\x6d\x6f\x72\x79\x5f\x73\x75\x66\x66"
"\x69\x63\x69\x65\x6e\x74\x28\x29",0,0);}l4 le l277(lg lw*la,lg le lb
,lg l97 l12){le l52,l253;l153*l85;lg l14*lc;lx l170;le lh;le l61;l23(
0);l52=l12;lc=&(la->lr);l61=l265(la,lb,&l52);lh=lc->l125(lc->l26,lc->
l40,l61,&l85);lq(lh,10,ln);lf(lc->lo==l48&&lb>=3){l52=l52/2;}lh=l280(
lc->l26,l318,&l170);lq(lh,5,ln);lf(l133){l332(l85,l52,&l253);l161[l61
]-=l253;l93 ln;}lf(!l170){lh=l370(lc->l26,l61,l52);lq(lh,30,ln);}lk{
lh=l382(l85,l52);lq(lh,40,ln);}ln:l29("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70"
"\x6d\x5f\x6d\x6e\x67\x72\x5f\x66\x72\x65\x65\x28\x29",0,0);}l4 l8
l272(lg lw*la,lg lv*ld,lg le lb,lg le ls){lf(la->lr.lo==l43){lz(ls){
ll 4:lj((ld->li[0]>>14)&0xffff);}}lk lf(la->lr.lo==l48||la->lr.lo==
l51){lz(ls){ll 4:lj(((ld->li[0]>>18)|(ld->li[1]<<14))&0xffff);}}lk lf
(la->lr.lo==l50){lz(ls){ll 3:lj((ld->li[0]>>14)&0xff);}};lj(l8)(-1);}
l4 l8 l269(lg lw*la,lg lv*ld,lg le lb,lg le ls){lf(la->lr.lo==l39){lz
(ls){ll 4:lj(((ld->li[1]<<14)|(ld->li[0]>>18))&0x7fff);}};lj(l8)(-1);
}l4 l8 l278(lg lw*la,lg lv*ld,lg le lb,lg le ls){lf(la->lr.lo==l43){
lz(ls){ll 4:lj((ld->li[0]>>13)&0xffff);}}lk lf(la->lr.lo==l48||la->lr
.lo==l51){lz(ls){ll 4:lj(((ld->li[0]>>17)|(ld->li[1]<<15))&0xffff);ll
3:lj((ld->li[0]>>21)&0xff);ll 2:lj((ld->li[0]>>21)&0xf);}}lk lf(la->
lr.lo==l50){lz(ls){ll 3:lj((ld->li[0]>>13)&0xff);}}lk lf(la->lr.lo==
l66){lz(ls){ll 2:lj((ld->li[0]>>14)&0xf);}}lk lf(la->lr.lo==l39){lz(
ls){ll 4:lj(((ld->li[1]<<14)|(ld->li[0]>>18))&0xffff);}};lj(l8)(-1);}
l4 l82 l227(lg lw*la,lg le lb,lg lv*ld){le l0;l0=l91(la,ld,lb);l277(
la,lb+1,l0);}l4 l82 l228(lg lw*la,lg le lb,lg lv*ld){le ly,l0,l256;lv
l175;lg l14*lc;lc=&(la->lr);l0=l91(la,ld,lb);lf(l0==-1)lj;l256=l266(
la,ld,lb);lf(lb<lc->l171-2)l5(ly=0;ly<l256;ly++){l98(la,lb+1,l0+ly,&
l175,1);l228(la,lb+1,&l175);}l227(la,lb,ld);}l4 le l284(lg lw*la,lg le
lb,lg l140 l104,lg le l89,l11 l97*l206){l285 l157;l153*l85;le l179;le
l74=1;le lh;le l61=0;le l114;lg l14*lc;le l255=0;l23(0);l204(l206);lc
=&(la->lr);lf(lc->lo==l43){lz(l104){ll l36:l74=4;l7;l17:l74=2;l7;}}lk
lf(lc->lo==l48||lc->lo==l51){lz(l104){ll l77:ll l87:ll l13:l74=1;l7;
ll l75:lf(l79(lc->l141[lc->l59[lb]]-1)-17>0){l74=1<<(l79(lc->l141[lc
->l59[lb]]-1)-17);}l7;ll l36:lf(l79(lc->l141[lc->l59[lb]]-1)-16>0){
l74=1<<(l79(lc->l141[lc->l59[lb]]-1)-16);}l7;l17:l74=1;l7;}}lk lf(lc
->lo==l50){lz(l104){ll l77:l74=2;l7;ll l118:l74=1;l7;ll l13:l74=2;l7;
l17:l74=1;l7;}}lk lf(lc->lo==l66){l74=2;}lk lf(lc->lo==l39){lz(l104){
ll l13:l74=1;l7;ll l115:l74=2;l7;ll l36:l74=2;l7;l17:l74=1;l7;}}lk{}
lf(lc->lo==l48&&lb>=3)l179=(l89+1)/2;lk l179=l89;lf(lc->lo==l39){l114
=(lc->l90-1)/(lc->l171-1);lf(l114==1){l61=lc->l59[lb];}lk lf(l114==2){
l61=lc->l59[lb]+(l255&0x1);}lk lf(l114==4){l61=lc->l59[lb]+(l255&0x3);
}lk{;}}lk{l61=lc->l59[lb];}lf(l133){l161[l61]+=l179;lj 1;}lh=lc->l125
(lc->l26,lc->l40,l61,&l85);lq(lh,10,ln);l361(l85,l179,l74,&l157);lf(
l157==-1){l363("\x45\x52\x52\x4f\x52\x3a\x20\x4c\x50\x4d\x20\x6d\x61"
"\x6c\x6c\x6f\x63\x20\x72\x61\x6e\x20\x6f\x75\x74\x20\x6f\x66\x20\x6d"
"\x65\x6d\x6f\x72\x79\n"); *l206=(0xFFFFFFFF);}lf(lc->lo==l48&&lb>=3)l157
 *=2;lk lf(lc->lo==l39)l157=l157+((l61-lc->l59[lb])<<19); *l206=l157;
ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70"
"\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x6d"
"\x61\x6c\x6c\x6f\x63\x28\x29",0,0);}l4 le l239(lg lw*la,lg le l3,lg
le l12,lg lx l18,lg le lb,lg lv*l185,l11 lv*l221,lg lx l198,l11 lx*
l15){lx l109;lv l78,l70,lt[(1<<(8))];l140 l127=0;le l81,l71,l105,l237
,l99;l116 l28;l142 l41;lx l57;lx l49;le lh;le l176;l97 l164=(
0xFFFFFFFF);l84*l2;lx l169,l315=l358;lg l14*lc;lg l83*lp;l140 l123=
l163;l23(l360);l99=l12;l49=l18;lc=&(la->lr);lp=&(la->lu);l28.l53=l99;
l28.l18=(lx)(lp->lm[lb]-lc->l34);lh=lc->l72(lc->l26,lc->l40,l3,&l2);
lq(lh,10,ln);lh=l162(l2,&l28,l315,&l41,&l57);lq(lh,20,ln);lf(l57){
l237=(((l41.l27.l73)>>0)&0xFFFFFFF);{l219(la,l221,lb-1,l237,l41.l340);
lf(l15){ *l15=l134;}l24;}}lh=l262(la,l185,lb-1,lt,&l123);lq(lh,30,ln);
lf(l198&((0x1)|(0x4))){l123=l163;}lf(l49<32){l99&=~(((le)-1)>>l49);}
lf(32+lc->l34-lp->lm[lb+1]<32){l81=l99>>(32+lc->l34-lp->lm[lb+1]);}lk
{l81=0;}l81&=((le)(-1))>>(32-(lp->lm[lb+1]-lp->lm[lb]));l71=1;lf(l49+
lc->l34<lp->lm[lb+1]){l71<<=lp->lm[lb+1]-l49-lc->l34;}lf(l49==0){l105
=0;}lk{l105=l99&(((le)(-1))<<(32-l49));}lf(l49<lp->lm[lb+1]-lc->l34){
l49=(lx)(lp->lm[lb+1]-lc->l34);}l109=0;l5(;l71;l71--,l81++,l105+=(1<<
(32+lc->l34-lp->lm[lb+1]))){l45(&l78,&lt[l81]);lh=l239(la,l3,l105,l49
,lb+1,&l78,&l70,l198,&l169);lq(lh,30,ln);lf(!l169){lf(l15){ *l15=l62;
}l24;}lf(l113(la,&l70,&l78)!=0){l109=1;lf(!l130(la,&l78,lb)){lf(l130(
la,&l70,lb)){l228(la,lb,&l78);}lk lf(l91(la,&l78,lb)!=l91(la,&l70,lb)){
l227(la,lb,&l78);}}l45(&lt[l81],&l70);}}lf(!l109){l45(l221,l185);}lk{
l176=l328(la,&l70,lb-1,lt,&l127,l123);lf(!l282(la,&l70,l185,lb-1)){lf
(l176!=0){lh=l284(la,lb,l127,l176,&l164);lq(lh,10,ln);lf(l164==(
0xFFFFFFFF)){lf(l15){ *l15=l62;}l24;}lf(!l133){l250(la,&l70,lb-1,lt,
l164,l176);}}}lk{l164=l91(la,l185,lb-1);l250(la,&l70,lb-1,lt,l164,
l176);}l45(l221,&l70);}lf(l15){ *l15=l134;}ln:l29("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x61\x64\x64\x5f\x70\x72\x65"
"\x66\x69\x78\x5f\x68\x65\x6c\x70\x65\x72\x28\x29",0,0);}l4 le l187(
lg lw*la,lg le l3,lg le l12,lg lx l18,lg lx l198,l11 lx*l15){le lb;lv
l70,l78;le l81,l105,l71,l99;lx l170;lx l49;le lh;lx l169;lg l14*lc;lg
l83*lp;l23(0);lc=&(la->lr);lp=&(la->lu);l49=l18;l99=l12;lb=0;lf(l49<
32){l99&=~(((le)-1)>>l49);}lf(32+lc->l34-lp->lm[lb+1]<32){l81=l99>>(
32+lc->l34-lp->lm[lb+1]);l81&=((le)(-1))>>(32+lc->l34-(lp->lm[lb+1]-
lp->lm[lb]));}lk{l81=0;}l81|=l3<<(lp->lm[1]-lc->l34);l71=1;lf(l49+lc
->l34<lp->lm[lb+1]){l71<<=lp->lm[lb+1]-l49-lc->l34;}lf(l49==0){l105=0
;}lk{l105=l99&(((le)(-1))<<(32-l49));}lf(l49<lp->lm[lb+1]-lc->l34){
l49=(lx)(lp->lm[lb+1]-lc->l34);}l5(;l71;l71--,l81++,l105+=(1<<(32+lc
->l34-lp->lm[lb+1]))){lh=l98(la,lb,l81,&l78,1);lq(lh,10,ln);lh=l239(
la,l3,l105,l49,lb+1,&l78,&l70,l198,&l169);lq(lh,20,ln);lf(!l169){lf(
l15){ *l15=l62;}l24;}lf(l113(la,&l70,&l78)!=0){lf(!l130(la,&l78,lb)){
lf(l130(la,&l70,lb)){l228(la,lb,&l78);}lk lf(l91(la,&l78,lb)!=l91(la,
&l70,lb)){l227(la,lb,&l78);}}lh=l184(la,lb,l81,&l70,1);lq(lh,20,ln);}
}l280(lc->l26,l318,&l170);lf(!l170){l398(lc->l26);}lf(l15){ *l15=l134
;}ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f"
"\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f"
"\x61\x64\x64\x5f\x70\x72\x65\x66\x69\x78\x5f\x68\x65\x6c\x70\x65\x72"
"\x30\x28\x29",0,0);}l8 l263(lg lw*la,lg lv*ld,lg le lb){lf(la->lr.lo
==l43){lz(l16(la,ld,lb)){ll l10:lj(ld->li[0]&0x7fff);l17:lj(l8)(-1);}
}lk lf(la->lr.lo==l48||la->lr.lo==l51){lz(l16(la,ld,lb)){ll l10:lj(ld
->li[0]&0xfffff);ll l131:l17:lj(l8)(-1);}}lk lf(la->lr.lo==l50){lz(
l16(la,ld,lb)){ll l10:lj(ld->li[0]&0x3fff);l17:lj(l8)(-1);}}lk lf(la
->lr.lo==l66){lz(l16(la,ld,lb)){ll l10:lj(ld->li[0]&0x3fff);l17:lj(l8
)(-1);}}lk lf(la->lr.lo==l39){lz(l16(la,ld,lb)){ll l10:lj(ld->li[0]&
0xffffffff);l17:lj(l8)(-1);}}lk{;lj(l8)(-1);}}l4 l8 l212(lg lw*la,lg
lv*ld,lg le lb,lg l8 l38,lg le ls){l8 l9;l8 l68;le ly;le l86;le l1;l8
l37=l38;l9=l272(la,ld,lb,ls);l86=1<<ls;lf(la->lu.lm[lb+2]-la->lu.lm[
lb+1]<ls){l1=1;l37<<=ls-(la->lu.lm[lb+2]-la->lu.lm[lb+1]);}lk{l1=1<<(
la->lu.lm[lb+2]-la->lu.lm[lb+1]-ls);}l9=(l9<<1)|1;l68=0;l5(ly=0;ly<
l86;ly++){lf(l37<l1)lz((l9>>ly)&0x3){ll 0x0:ll 0x2:lj(l68-1);ll 0x1:
lj(l68);ll 0x3:lj(l68+l37);}l37-=l1;lz((l9>>ly)&0x3){ll 0x0:ll 0x2:l7
;ll 0x1:l68+=1;l7;ll 0x3:l68+=l1;l7;}};lj(l8)(-1);}l4 l8 l268(lg lw*
la,lg lv*ld,lg le lb,lg l8 l38,lg le ls){l8 l9;l8 l68;le ly;le l86;le
l1;l8 l37=l38;l9=l269(la,ld,lb,ls);l86=1<<ls;lf(la->lu.lm[lb+2]-la->
lu.lm[lb+1]<ls){l1=1;l37<<=ls-(la->lu.lm[lb+2]-la->lu.lm[lb+1]);}lk{
l1=1<<(la->lu.lm[lb+2]-la->lu.lm[lb+1]-ls);}l9=(l9<<1)|1;l68=0;l5(ly=
0;ly<l86;ly++){lf(l37<l1)lz((l9>>ly)&0x1){ll 0x0:lj(l68-1);ll 0x1:lj(
l68+l37);}l37-=l1;lz((l9>>ly)&0x1){ll 0x0:l7;ll 0x1:l68+=l1;l7;}};lj(
l8)(-1);}l4 l8 l137(lg lw*la,lg lv*ld,lg le lb,lg l8 l38,lg le ls){l8
l30;le l1;l8 l68;le ly;le l154;le l37=l38;l30=l278(la,ld,lb,ls);l154=
1<<ls;lf(la->lu.lm[lb+2]-la->lu.lm[lb+1]<ls){l1=1;}lk{l1=1<<(la->lu.
lm[lb+2]-la->lu.lm[lb+1]-ls);}l68=0;l5(ly=0;ly<l154;){lf(l37<l1)lz((
l30>>ly)&0x1){ll 0x0:lj(l68);ll 0x1:lj(l68+l37);}l37-=l1;lz((l30>>ly)&
0x1){ll 0x0:l68+=1;l7;ll 0x1:l68+=l1;l7;}lf(ls>la->lu.lm[lb+2]-la->lu
.lm[lb+1])ly+=1<<(ls-(la->lu.lm[lb+2]-la->lu.lm[lb+1]));lk ly++;};lj(
l8)(-1);}l4 l8 l160(lg lw*la,lg lv*ld,lg le lb,lg l8 l38){l8 l6;l8 l37
=l38;l6=l246(la,ld,lb);lf(la->lr.lo==l43){lf(la->lu.lm[lb+2]-la->lu.
lm[lb+1]<=8)l37<<=8-(la->lu.lm[lb+2]-la->lu.lm[lb+1]);lk l37>>=(la->
lu.lm[lb+2]-la->lu.lm[lb+1])-8;}lk lf(la->lr.lo==l48||la->lr.lo==l51){
lf(la->lu.lm[lb+2]-la->lu.lm[lb+1]<=9)l37<<=9-(la->lu.lm[lb+2]-la->lu
.lm[lb+1]);lk l37>>=(la->lu.lm[lb+2]-la->lu.lm[lb+1])-9;}lk lf(la->lr
.lo==l50){lf(la->lu.lm[lb+2]-la->lu.lm[lb+1]<=7)l37<<=7-(la->lu.lm[lb
+2]-la->lu.lm[lb+1]);lk l37>>=(la->lu.lm[lb+2]-la->lu.lm[lb+1])-7;}lk
lf(la->lr.lo==l66){lf(la->lu.lm[lb+2]-la->lu.lm[lb+1]<=6)l37<<=6-(la
->lu.lm[lb+2]-la->lu.lm[lb+1]);lk l37>>=(la->lu.lm[lb+2]-la->lu.lm[lb
+1])-6;}lk lf(la->lr.lo==l39){l37<<=8-(la->lu.lm[lb+2]-la->lu.lm[lb+1
]);}lk{;lj(l8)(-1);}l37&=~(l6^(l6-1));l6&=~(l6^(l6-1));lf(l37==l6){lj
(1);}lk{lj(0);}}l4 l8 l306(lg lw*la,lg lv*ld,lg le lb,lg l8 l38){lf(
la->lr.lo==l43){lz(l16(la,ld,lb)){ll l75:lj(l212(la,ld,lb,l38,4));ll
l36:lj(l137(la,ld,lb,l38,4));ll l13:lj(l160(la,ld,lb,l38));l17:l7;}}
lk lf(la->lr.lo==l48||la->lr.lo==l51){lz(l16(la,ld,lb)){ll l75:lj(
l212(la,ld,lb,l38,4));ll l36:lj(l137(la,ld,lb,l38,4));ll l13:lj(l160(
la,ld,lb,l38));ll l77:lj(l137(la,ld,lb,l38,3));ll l87:lj(l137(la,ld,
lb,l38,2));l17:l7;}}lk lf(la->lr.lo==l50){lz(l16(la,ld,lb)){ll l118:
lj(l212(la,ld,lb,l38,3));ll l77:lj(l137(la,ld,lb,l38,3));ll l13:lj(
l160(la,ld,lb,l38));l17:l7;}}lk lf(la->lr.lo==l66){lz(l16(la,ld,lb)){
ll l87:lj(l137(la,ld,lb,l38,2));ll l13:lj(l160(la,ld,lb,l38));l17:l7;
}}lk lf(la->lr.lo==l39){lz(l16(la,ld,lb)){ll l115:lj(l268(la,ld,lb,
l38,4));ll l36:lj(l137(la,ld,lb,l38,4));ll l13:lj(l160(la,ld,lb,l38));
l17:l7;}};lj(l8)(-1);}l4 le l258(lg lw*la,lg le l166,lg le l12,lg le
lb,lg l8 l156){lv ld;l8 l121=l156;l98(la,lb,l121,&ld,1);lf(l130(la,&
ld,lb)){lj(l263(la,&ld,lb));}lf(32+la->lr.l34-la->lu.lm[lb+2]<32){
l121=l12>>(32+la->lr.l34-la->lu.lm[lb+2]);l121&=(1<<(la->lu.lm[lb+2]-
la->lu.lm[lb+1]))-1;}lk{l121=0;}l121=l306(la,&ld,lb,l121);l121+=l91(
la,&ld,lb);lj(l258(la,l166,l12,lb+1,l121));}le l348(lg lw*la,lg le
l166,lg le l3,lg le l12){l8 l156;l67 lb;lb=-1;lf(32+la->lr.l34-la->lu
.lm[lb+2]<32){l156=l12>>(32+la->lr.l34-la->lu.lm[lb+2]);}lk{l156=0;}
l156|=l3<<(la->lu.lm[lb+2]-la->lr.l34);lj(l258(la,l166,l12,lb+1,l156));
}le l362(l19 lw*la,lg le l166){l14*lc;l83*lp;le l42,l148;le lh;l23(0);
lc=&(la->lr);lp=&(la->lu);l5(l42=0;l42<lc->l241;++l42){lc->l63[l42].
l342=l404;lc->l63[l42].l390=l339;lc->l63[l42].l138.l368=l333;lf(l42>0
&&lc->l112&l279){lc->l63[l42].l138.l244=lc->l63[0].l138.l244;lc->l63[
l42].l138.l287=lc->l63[0].l138.l287;lc->l63[l42].l283=lc->l63[0].l283
;lc->l63[l42].l308=lc->l63[0].l308;lc->l63[l42].l138.l325=lc->l63[0].
l138.l325;}lk{lc->l63[l42].l138.l244=0;}lh=l336(&(lc->l63[l42]));lq(
lh,10,ln);}lh=l167(&(lc->l119[l371]));lq(lh,20,ln);lh=l167(&(lc->l119
[l293]));lq(lh,30,ln);lh=l167(&(lc->l119[l294]));lq(lh,40,ln);lh=l167
(&(lc->l119[l292]));lq(lh,50,ln);lh=l167(&(lc->l119[l291]));lq(lh,60,
ln);lh=l167(&(lc->l119[l290]));lq(lh,70,ln);lp->lm=(le* )l397(l126(le
) * (lc->l171+1),"\x64\x61\x74\x61\x5f\x69\x6e\x66\x6f\x5f\x70\x74"
"\x72\x2d\x3e\x62\x69\x74\x5f\x64\x65\x70\x74\x68\x5f\x70\x65\x72\x5f"
"\x62\x61\x6e\x6b");lf(lp->lm==l217){l346(l396,75,ln);}lf(lc->l112&
l124){lh=l186(lc->l92[l293]);lq(lh,80,ln);lh=l186(lc->l92[l294]);lq(
lh,90,ln);lh=l186(lc->l92[l292]);lq(lh,100,ln);lh=l186(lc->l92[l291]);
lq(lh,110,ln);lh=l186(lc->l92[l290]);lq(lh,120,ln);}lp->lm[0]=0;l5(
l148=0;l148<lc->l171;l148++){lp->lm[l148+1]=lp->lm[l148]+lc->l300[
l148];}ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64"
"\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72"
"\x5f\x63\x72\x65\x61\x74\x65\x28\x29",0,0);}le l338(lg le l166,l19 lw
 *la,lg le l3,lg l174 l108){l84*l2;lv l168;l116 l20;l314 l27;le l35;
le l110;lx l15;lg l14*lc;lg l83*lp;le lh;l23(0);lc=&(la->lr);lp=&(la
->lu);lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,10,ln);l20.l18=0;l20.
l53=0;l27.l73=((((l233)&0x3)<<28)|(((l108)&0xFFFFFFF)<<0));lh=l205(l2
,&l20,&l27,&l15);lq(lh,20,ln);lf(!l400){l219(la,&l168,0,l108,0);l110=
l3;l110<<=lp->lm[1]-lc->l34;l5(l35=l110;l35<l110+(le)(1<<(lp->lm[1]-
lc->l34));++l35){lh=l184(la,0,l35,&l168,1);lq(lh,100,ln);}}ln:l29(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f"
"\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x76\x72\x66"
"\x5f\x69\x6e\x69\x74\x28\x29",0,0);}le l394(l19 lw*la,lg le l3,lg
l174 l108,lg l197 l95){l84*l2;lx l15;lg l14*lc;l102 l107;le lh;l23(0);
lc=&(la->lr);lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,10,ln);l275(&
l107);l107.l94=0;l107.l96=0;lh=l261(l2);lq(lh,15,ln);lh=lc->l271(lc->
l26,0);lq(lh,15,ln);l251(la,l3,&l107,l108,l95,l190,&l15);ln:l29("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69"
"\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x76\x72\x66\x5f"
"\x63\x6c\x65\x61\x72\x28\x29",0,0);}le l337(l19 lw*la,lg le l3,lg
l174 l108,lg l197 l95){l84*l2;lx l15;lg l14*lc;l102 l107;le lh;l23(0);
lc=&(la->lr);lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,10,ln);l275(&
l107);l107.l94=0;l107.l96=0;lh=l372(l2);lq(lh,15,ln);lh=lc->l271(lc->
l26,0);lq(lh,20,ln);l251(la,l3,&l107,l108,l95,l190,&l15);ln:l29("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69"
"\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x76\x72\x66\x5f"
"\x72\x6f\x75\x74\x65\x73\x5f\x63\x6c\x65\x61\x72\x28\x29",0,0);}le
l405(l19 lw*la,lg le l117,lg l174 l108){l84*l2;lv l168;l116 l20;l314
l27;le l35;le l110,l3;lx l15;lg l14*lc;lg l83*lp;le lh;l23(0);lc=&(la
->lr);lp=&(la->lu);l5(l35=0;l35<la->lr.l90;++l35){lh=l295(&(lc->l119[
l35]));lq(lh,40,ln);lf(l35>=1&&la->lr.l112&l124){lh=l264((lc->l92[l35
]));lq(lh,30,ln);}}l5(l3=0;l3<l117;++l3){lh=lc->l72(lc->l26,lc->l40,
l3,&l2);lq(lh,10,ln);lf(l3==0){lh=l261(l2);lq(lh,15,ln);}lk{lh=l391(
l2);lq(lh,16,ln);}l20.l18=0;l20.l53=0;l27.l73=((((l233)&0x3)<<28)|(((
l108)&0xFFFFFFF)<<0));lh=l205(l2,&l20,&l27,&l15);lq(lh,20,ln);l219(la
,&l168,0,l108,0);l110=l3;l110<<=lp->lm[1]-lc->l34;l5(l35=l110;l35<
l110+(le)(1<<(lp->lm[1]-lc->l34));++l35){lh=l184(la,0,l35,&l168,1);lq
(lh,100,ln);}}ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72"
"\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e"
"\x67\x72\x5f\x61\x6c\x6c\x5f\x76\x72\x66\x73\x5f\x63\x6c\x65\x61\x72"
"\x28\x29",0,0);}le l347(l19 lw*la){l14*lc;l83*lp;le l56,l42;le lh;
l23(0);lc=&(la->lr);lp=&(la->lu);l5(l42=0;l42<lc->l241;++l42){lh=l410
(&(lc->l63[l42]));lq(lh,10,ln);lf(lc->l112&l279){l7;}}l5(l56=0;l56<lc
->l90;++l56){lh=l403(&(lc->l119[l56]));lq(lh,20,ln);}lf(lc->l112&l124
){l5(l56=1;l56<lc->l90;++l56){lh=l393(lc->l92[l56]);lq(lh,260,ln);}}
lf(lc->l112&l124){l5(l56=1;l56<lc->l90;++l56){l249(lc->l92[l56]);}
l249(lc->l92);}l249(lp->lm);ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x64\x65\x73\x74\x72\x6f\x79\x28\x29",0,0);}
le l359(l19 lw*la,lg le l3){le l12;lx l18;le lh;l84*l2;l14*lc;l23(0);
lc=&(la->lr);lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,20,ln);l12=0;
l18=0;lh=l187(la,l3,l12,l18,(0x1),l217);lq(lh,30,ln);ln:l29("\x65\x72"
"\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70"
"\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x68\x77\x5f\x73\x79"
"\x6e\x63\x28\x29",0,0);}le l374(l19 lw*la,lg le l3,lg l102*l20,l197
l95){l84*l2;l14*lc;l116 l28;l142 l41;le l12,l27;lx l18;le lh;lx l57,
l15;l23(0);lc=&(la->lr);l12=l20->l94;l18=l20->l96;l28.l18=l18;l28.l53
=l12;lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,10,ln);lh=l162(l2,&l28,
l62,&l41,&l57);lq(lh,20,ln);lf(!l57){l93 ln;}l27=((((l95)&0x3)<<28)|(
(((((l41.l27.l73)>>0)&0xFFFFFFF))&0xFFFFFFF)<<0));l41.l27.l73=l27;lh=
l205(l2,&l28,&l41.l27,&l15);lq(lh,10,ln);ln:l29("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f"
"\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x6d\x61\x72\x6b\x5f\x6e\x6f\x6e"
"\x5f\x6c\x70\x6d\x5f\x72\x6f\x75\x74\x65\x5f\x70\x65\x6e\x64\x69\x6e"
"\x67\x5f\x74\x79\x70\x65\x5f\x73\x65\x74\x28\x29",0,0);}le l251(l19
lw*la,lg le l3,lg l102*l20,lg l174 l220,lg lx l152,lg l247 l128,l11 lx
 *l15){l84*l2;l14*lc;l116 l28;l142 l41;le l12,l27;lx l18,l57;le lh;
l23(0);lc=&(la->lr);l12=l20->l94;l18=l20->l96;lf(!l152){lh=l181(la,0,
l15);lq(lh,10,ln);lf(! *l15){lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh
,10,ln);l28.l18=l18;l28.l53=l12;lh=l162(l2,&l28,l350,&l41,&l57);lq(lh
,20,ln);lf(!l57){l93 ln;}lk{ *l15=l134;}}}l196:l28.l18=l18;l28.l53=
l12;lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,10,ln);l27=(((((l152==
l134)?l411:l233)&0x3)<<28)|(((l220)&0xFFFFFFF)<<0));l41.l27.l73=l27;
lh=l205(l2,&l28,&l41.l27,l15);lq(lh,10,ln);lf(! *l15){l133=0;l93 ln;}
lf((!l152)&&((l128==l190))){lh=l187(la,l3,l12,l18,(0x2),l15);lq(lh,20
,ln);}lf(l133==1){lh=l181(la,2,l15);lq(lh,101,ln);l133=0;lf( *l15){
l93 l196;}lk{lh=l326(l2,&l28);lq(lh,102,ln);}}ln:l29("\x65\x72\x72"
"\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66\x69\x78"
"\x5f\x61\x64\x64\x28\x29",0,0);}le l407(l19 lw*la,lg le l3,lg le*
l260,lg le l117,lg lx l312,l11 l380*l15){le l80,l145;lx l135=l62;le
l12,l149;lx l196,l18;le l31;l153*l101;le lh;l84*l2;lg l14*lc;l116 l214
;l23(0);lc=&(la->lr);l12=0;l18=0;lf(l312){l5(l80=0;l80<l117;++l80){lf
(l117==1){l145=l3;}lk{l149=0;lh=l330(l260,l80,1,&l149);lf(!l149){l226
;}l145=l80;}lh=lc->l72(lc->l26,lc->l40,l145,&l2);lq(lh,20,ln);l376(l2
,&l214);l18=l214.l18;l12=l214.l53;lh=l187(la,l145,l12,l18,(0x1),&l135
);lq(lh,30,ln);lf(!l135){l7;}}lf(l135){l181(la,1,&l196);l135=l196;}lf
(!l135){l5(l31=0;l31<lc->l90;++l31){lh=lc->l125(lc->l26,lc->l40,lc->
l90-l31-1,&l101);lq(lh,30,ln);lh=l364(l101,0);lq(lh,30,ln);lf(l31>=1
&&la->lr.l112&l124){lh=l354((lc->l92[l31]),0);lq(lh,40,ln);}} *l15=
l353;l93 ln;}}lk{l135=l134;}l5(l31=0;l31<lc->l90;++l31){lh=lc->l125(
lc->l26,lc->l40,lc->l90-l31-1,&l101);lq(lh,30,ln);lf(l135){lh=l352(
l101,0);lq(lh,30,ln);lf(l31>=1&&la->lr.l112&l124){lh=l401((lc->l92[
l31]),0);lq(lh,40,ln);} *l15=l386;}}l5(l80=0;l80<l117;++l80){lf(l117
==1){l145=l3;}lk{l149=0;lh=l330(l260,l80,1,&l149);lf(!l149){l226;}
l145=l80;}lh=l375(&(la->lr.l63[l145]),0);lq(lh,30,ln);}ln:l29("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69"
"\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66"
"\x69\x78\x5f\x61\x64\x64\x28\x29",0,0);}le l351(l19 lw*la,lg le l3,
lg le l117,lg lx l189){le l31,l80;l153*l101;l84*l2;le lh;lg l14*lc;
l23(0);lc=&(la->lr);l5(l31=0;l31<lc->l90;++l31){lh=lc->l125(lc->l26,
lc->l40,l31,&l101);lq(lh,10,ln);lh=l366(l101,l189);lq(lh,10,ln);lf(la
->lr.l112&l124&&l31>=1){lh=l389((lc->l92[l31]),l189);lq(lh,20,ln);}}
lf(l3==l402){l5(l80=0;l80<l117;++l80){lh=lc->l72(lc->l26,lc->l40,l80,
&l2);lq(lh,20,ln);lh=l319(l2,l189);lq(lh,30,ln);}}lk{lh=lc->l72(lc->
l26,lc->l40,l3,&l2);lq(lh,40,ln);lh=l319(l2,l189);lq(lh,50,ln);}ln:
l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70"
"\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x63\x61"
"\x63\x68\x65\x5f\x73\x65\x74\x28\x29",0,0);}le l341(l19 lw*la,lg le
l3,lg l102*l20,lg lx l152,l11 lx*l15){l84*l2;l14*lc;l116 l28;l142 l41
;le l12;lx l18;le lh;lx l57;l23(0);l12=l20->l94;l18=l20->l96;lf(!l152
){lh=l181(la,1,l15);lq(lh,10,ln);lf(! *l15){lf(! *l15){l93 ln;}}}lc=&
(la->lr);lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,10,ln);l28.l18=l18;
l28.l53=l12;lh=l162(l2,&l28,l62,&l41,&l57);lq(lh,20,ln);lf(!l57){l93
ln;}{lh=l326(l2,&l28);lq(lh,10,ln);lf(!l152){lh=l187(la,l3,l12,l18,(
0x4),l15);lq(lh,20,ln);}}ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f"
"\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66\x69\x78\x5f\x72\x65\x6d\x6f\x76"
"\x65\x28\x29",0,0);}le l309(l19 lw*la,lg le l3,lg l102*l20,l11 l102*
l158,l11 lx*l122,l11 lx*l57){l84*l2;l14*lc;l116 l28;l142 l41;le lh;
l23(0);lc=&(la->lr);lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,10,ln);
l28.l53=l20->l94;l28.l18=l20->l96;lh=l162(l2,&l28,l62,&l41,l57);lq(lh
,20,ln);lf(!( *l57)){ *l122=l62;l93 ln;}l158->l94=l41.l20.l53;l158->
l96=l41.l20.l18;lf((l158->l94==l20->l94)&&(l158->l96==l20->l96)){ *
l122=l134;}lk{ *l122=l62;}ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66\x69\x78\x5f\x6c\x6f\x6f\x6b"
"\x75\x70\x28\x29",0,0);}le l356(l19 lw*la,lg le l3,lg l102*l20,l11 lx
 *l313){l102 l158;lx l286;lx l122;le lh;l23(0);lh=l309(la,l3,l20,&
l158,&l122,&l286);lq(lh,10,ln); *l313=l122;ln:l29("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66\x69\x78\x5f"
"\x69\x73\x5f\x65\x78\x69\x73\x74\x28\x29",0,0);}le l334(l19 lw*la,lg
le l3,lg l102*l20,lg lx l122,l11 l174*l220,l11 l197*l95,l11 l247*l128
,l11 lx*l57){l84*l2;l14*lc;l116 l28;l142 l41;lx l252;le lh;l23(0);lc=
&(la->lr);lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,10,ln);l28.l53=l20
->l94;l28.l18=l20->l96;lh=l162(l2,&l28,l62,&l41,&l252);lq(lh,20,ln);
lf(!l252){ *l57=l62;l93 ln;}lf(l122&&((l41.l20.l53!=l20->l94)||(l41.
l20.l18!=l20->l96))){ *l57=l62;l93 ln;} *l57=l134; *l220=(((l41.l27.
l73)>>0)&0xFFFFFFF);lf(l95){ *l95=(((l41.l27.l73)>>28)&0x3);}lf(l128){
 *l128=(l190);}ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72"
"\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e"
"\x67\x72\x5f\x73\x79\x73\x5f\x66\x65\x63\x5f\x67\x65\x74\x28\x29",0,
0);}le l392(l19 lw*la,lg le l3,l11 l243*l193){l84*l2;l14*lc;le lh;l23
(0);l204(l193);lc=&(la->lr);lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,
10,ln);lh=l387(l2,&(l193->l276),&(l193->l320),&(l193->l305));lq(lh,20
,ln);ln:l29("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f"
"\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f"
"\x67\x65\x74\x5f\x73\x74\x61\x74\x28\x29",0,0);}le l399(l19 lw*la){
l14*lc;le l56,l42;le lh;l23(0);lc=&(la->lr);l5(l42=0;l42<lc->l241;++
l42){lh=l261(&(lc->l63[l42]));lq(lh,10,ln);}l5(l56=0;l56<lc->l90;++
l56){lh=l295(&(lc->l119[l56]));lq(lh,20,ln);lf(l56>=1&&la->lr.l112&
l124){lh=l264((lc->l92[l56]));lq(lh,30,ln);}}ln:l29("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x63\x6c\x65\x61\x72\x28\x29"
,0,0);}le l335(l19 lw*la,lg le l3,lg l395 l273,l19 l240*l245,lg le
l274,lg le l270,l11 l102*l257,l11 le*l322,l11 l197*l95,l11 l247*l128,
l11 le*l203){l142*l144=l217;le l188,l172,l225,l155,l35;l14*lc;l84*l2;
le lh;l23(0);lc=&(la->lr);lh=lc->l72(lc->l26,lc->l40,l3,&l2);lq(lh,10
,ln);l172=l270;l35=0;l225=0;l381(l144,l142,1000,"\x61\x72\x61\x64\x5f"
"\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f"
"\x67\x65\x74\x5f\x62\x6c\x6f\x63\x6b\x2e\x6e\x6f\x64\x65\x73");l378(
!l409((l245))&&l172){lf(l172>1000){l188=1000;}lk{l188=l172;}l172-=
l188;lh=l385(l2,l273,l245,l274,l188,l144,l203);lq(lh,20,ln);l225+= *
l203;l5(l155=0;l35<l225;++l35,++l155){l257[l35].l94=l144[l155].l20.
l53;l257[l35].l96=l144[l155].l20.l18;l322[l35]=(((l144[l155].l27.l73)>>
0)&0xFFFFFFF);lf(l95){l95[l35]=(((l144[l155].l27.l73)>>28)&0x3);}lf(
l128){l128[l35]=(l190);}}} *l203=l35;ln:l357(l144);l29("\x65\x72\x72"
"\x6f\x72\x20\x69\x6e\x20\x61\x72\x61\x64\x5f\x70\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x67\x65\x74\x5f\x62\x6c"
"\x6f\x63\x6b\x28\x29",0,0);}l82 l345(l19 lw*l106){l23(0);l204(l106);
l248(l106,0x0,l126(lw));l106->lu.lm=l217;l267;ln:l323(0,0,0);}l82 l343
(l19 l243*l106){l23(0);l204(l106);l248(l106,0x0,l126(l243));l106->
l305=0;l106->l320=0;l106->l276=0;l267;ln:l323(0,0,0);}
#undef l195
#undef l67
#undef l369
#undef le
#undef l8
#undef l4
#undef lx
#undef l324
#undef l355
#undef l285
#undef l281
#undef l18
#undef l82
#undef l11
#undef lv
#undef lg
#undef l202
#undef li
#undef l126
#undef lw
#undef lj
#undef l240
#undef l53
#undef lf
#undef lr
#undef lo
#undef lk
#undef l19
#undef l14
#undef l43
#undef l48
#undef l51
#undef l141
#undef l59
#undef l50
#undef l66
#undef l39
#undef l73
#undef l373
#undef l329
#undef l27
#undef l5
#undef l90
#undef l171
#undef l153
#undef lh
#undef l23
#undef l125
#undef l26
#undef l40
#undef lq
#undef ln
#undef l377
#undef l29
#undef l248
#undef l93
#undef l226
#undef l406
#undef l297
#undef lz
#undef ll
#undef l17
#undef l62
#undef l91
#undef l304
#undef l7
#undef lu
#undef lm
#undef l83
#undef l24
#undef l388
#undef l408
#undef l383
#undef l298
#undef l344
#undef l238
#undef l300
#undef l365
#undef l280
#undef l318
#undef l332
#undef l370
#undef l382
#undef l204
#undef l361
#undef l363
#undef l116
#undef l142
#undef l84
#undef l2
#undef l358
#undef l360
#undef l34
#undef l72
#undef l162
#undef l340
#undef l134
#undef l398
#undef l348
#undef l362
#undef l241
#undef l63
#undef l342
#undef l404
#undef l390
#undef l339
#undef l138
#undef l368
#undef l333
#undef l112
#undef l279
#undef l244
#undef l287
#undef l283
#undef l308
#undef l325
#undef l336
#undef l167
#undef l119
#undef l371
#undef l293
#undef l294
#undef l292
#undef l291
#undef l290
#undef l397
#undef l217
#undef l346
#undef l396
#undef l124
#undef l186
#undef l92
#undef l338
#undef l174
#undef l20
#undef l314
#undef l233
#undef l205
#undef l400
#undef l394
#undef l197
#undef l102
#undef l275
#undef l94
#undef l96
#undef l261
#undef l271
#undef l251
#undef l190
#undef l337
#undef l372
#undef l405
#undef l295
#undef l264
#undef l391
#undef l347
#undef l410
#undef l403
#undef l393
#undef l249
#undef l359
#undef l374
#undef l220
#undef l247
#undef l350
#undef l411
#undef l326
#undef l407
#undef l380
#undef l330
#undef l376
#undef l364
#undef l354
#undef l353
#undef l352
#undef l401
#undef l386
#undef l375
#undef l351
#undef l366
#undef l389
#undef l402
#undef l319
#undef l341
#undef l309
#undef l356
#undef l334
#undef l392
#undef l243
#undef l387
#undef l276
#undef l320
#undef l305
#undef l399
#undef l335
#undef l395
#undef l381
#undef l378
#undef l409
#undef l385
#undef l357
#undef l345
#undef l106
#undef l267
#undef l323
#undef l343
#include<soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */

