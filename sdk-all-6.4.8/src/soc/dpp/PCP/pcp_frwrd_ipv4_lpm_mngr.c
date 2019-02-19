/*
 * $Id: pcp_frwrd_ipv4_lpm_mngr.c,v 1.11 Broadcom SDK $
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
*/
#include<soc/dpp/SAND/Utils/sand_header.h>
#include<soc/dpp/PCP/pcp_frwrd_ipv4_lpm_mngr.h>
#include<soc/dpp/PCP/pcp_sw_db.h>
#include<soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include<soc/dpp/SAND/Utils/sand_os_interface.h>
#include<soc/dpp/SAND/Utils/sand_bitstream.h>
#include<soc/dpp/SAND/Management/sand_error_code.h>
#include<soc/dpp/SAND/Management/sand_low_level.h>
#include<assert.h>
#define l204 typedef
#define l60 int32
#define l356 SOC_SAND_LONG
#define ld uint32
#define l9 SOC_SAND_ULONG
#define l353 enum
#define l292 PCP_ARR_MEM_ALLOCATOR_PTR
#define l4 static
#define l295 struct
#define l0 uint8
#define l23 prefix
#define l66 void
#define l12 SOC_SAND_OUT
#define ly PCP_IPV4_LPM_MNGR_INST
#define lg SOC_SAND_IN
#define l341 soc_sand_os_memcpy
#define lh arr
#define l146 sizeof
#define lz PCP_IPV4_LPM_MNGR_INFO
#define li return
#define l147 SOC_SAND_U64
#define l32 val
#define le if
#define lr init_info
#define ln pxx_model
#define lk else
#define l20 SOC_SAND_INOUT
#define l16 PCP_IPV4_LPM_MNGR_INIT_INFO
#define l47 PCP_IPV4_LPM_PXX_MODEL_P4X
#define l53 PCP_IPV4_LPM_PXX_MODEL_P4X_COMPLETE
#define l143 nof_rows_per_mem
#define l77 bank_to_mem
#define lw assert
#define l56 PCP_IPV4_LPM_PXX_MODEL_P6N
#define l71 PCP_IPV4_LPM_PXX_MODEL_P6X
#define l59 PCP_IPV4_LPM_PXX_MODEL_P6M
#define l36 PCP_IPV4_LPM_PXX_MODEL_LP4
#define l35 payload
#define l397 pcp_ipv4_lpm_entry_decode
#define l246 PCP_ARR_MEM_ALLOCATOR_ENTRY
#define l26 data
#define l7 for
#define l95 nof_mems
#define l164 nof_banks
#define l154 PCP_ARR_MEM_ALLOCATOR_INFO
#define lj res
#define l33 SOC_SAND_INIT_ERROR_DEFINITIONS
#define l159 mem_alloc_get_fun
#define l50 prime_handle
#define l65 sec_handle
#define lv SOC_SAND_CHECK_FUNC_RESULT
#define lp exit
#define l260 pcp_arr_mem_allocator_read
#define l270 SOC_SAND_OK
#define l217 soc_sand_os_memset
#define l286 pcp_arr_mem_allocator_write
#define l45 SOC_SAND_EXIT_AND_SEND_ERROR
#define l6 switch
#define lm case
#define l22 default
#define l39 FALSE
#define l100 pcp_ipv4_lpm_mngr_row_to_base_addr
#define l358 pcp_ipv4_lpm_mngr_update_base_addr
#define l8 break
#define lt data_info
#define ll bit_depth_per_bank
#define l86 PCP_IPV4_LPM_MNGR_T
#define l190 PCP_DO_NOTHING_AND_EXIT
#define l404 PCP_ARR_MEM_ALLOCATOR_REQ_BLOCKS
#define l374 PCP_ARR_MEM_ALLOCATOR_REQ_BLOCKS_clear
#define l304 nof_reqs
#define l343 mem_to_bank
#define l255 block_size
#define l310 nof_bits_per_bank
#define l355 pcp_arr_mem_allocator_is_availabe_blocks
#define l133 goto
#define l336 pcp_sw_db_ipv4_cache_mode_for_ip_type_get
#define l315 PCP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM
#define l347 pcp_sw_db_free_list_add
#define l403 pcp_arr_mem_allocator_free
#define l349 pcp_arr_mem_allocator_malloc
#define l360 soc_sand_os_printf
#define l120 SOC_SAND_PAT_TREE_NODE_KEY
#define l134 SOC_SAND_PAT_TREE_NODE_INFO
#define l94 SOC_SAND_PAT_TREE_INFO
#define l11 pat_tree
#define l359 SOC_SAND_HASH_TABLE_DESTROY
#define l48 nof_vrf_bits
#define l96 pat_tree_get_fun
#define l186 soc_sand_pat_tree_lpm_get
#define l88 TRUE
#define l342 node_place
#define l362 pcp_sw_db_free_list_commit
#define l396 pcp_ipv4_lpm_mngr_lookup0
#define l339 pcp_ipv4_lpm_mngr_create
#define l256 nof_lpms
#define l161 lpms
#define l346 node_data_is_identical_fun
#define l401 pcp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun
#define l389 node_is_skip_in_lpm_identical_data_query_fun
#define l366 pcp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun
#define l340 soc_sand_pat_tree_create
#define l377 pcp_arr_mem_allocator_create
#define l224 mem_allocators
#define l392 soc_sand_os_malloc
#define l155 flags
#define l152 PCP_LPV4_LPM_SUPPORT_DEFRAG
#define l364 soc_sand_group_mem_ll_create
#define l130 rev_ptrs
#define l381 pcp_ipv4_lpm_mngr_vrf_init
#define l199 SOC_SAND_PP_SYSTEM_FEC_ID
#define l25 key
#define l329 SOC_SAND_PAT_TREE_NODE_DATA
#define l213 soc_sand_pat_tree_clear
#define l219 SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED
#define l195 SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM
#define l211 soc_sand_pat_tree_node_add
#define l361 pcp_ipv4_lpm_mngr_vrf_clear
#define l205 SOC_PPC_FRWRD_IP_ROUTE_STATUS
#define l115 SOC_SAND_PP_IPV4_SUBNET
#define l350 soc_sand_SAND_PP_IPV4_SUBNET_clear
#define l107 ip_address
#define l110 prefix_len
#define l344 nof_entries_for_hw_lpm_set_fun
#define l285 pcp_ipv4_lpm_mngr_prefix_add
#define l379 pcp_ipv4_lpm_mngr_all_vrfs_clear
#define l307 pcp_arr_mem_allocator_clear
#define l335 soc_sand_group_mem_ll_clear
#define l369 pcp_ipv4_lpm_mngr_destroy
#define l365 soc_sand_pat_tree_destroy
#define l370 pcp_arr_mem_allocator_destroy
#define l390 soc_sand_group_mem_ll_destroy
#define l265 soc_sand_os_free_any_size
#define l311 pcp_ipv4_lpm_mngr_pat_foreach_fn_hw_sync
#define l394 SOC_SAND_PAT_TREE_KEY
#define l308 is_prefix
#define l226 SOC_PPC_FRWRD_IP_ROUTE_LOCATION
#define l391 SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE
#define l289 SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_ADD
#define l376 pcp_ipv4_lpm_mngr_hw_sync
#define l178 NULL
#define l388 SOC_SAND_MALLOC
#define l301 max_nof_entries_for_hw_lpm
#define l400 soc_sand_pat_tree_foreach
#define l333 soc_sand_pat_tree_node_remove
#define l363 SOC_SAND_FREE
#define l375 pcp_ipv4_lpm_mngr_mark_non_lpm_route_pending_type_set
#define l231 sys_fec_id
#define l261 SOC_SAND_SUCCESS_FAILURE
#define l196 SOC_SAND_SUCCESS
#define l200 pcp_ipv4_lpm_mngr_cache_set
#define l274 pcp_ipv4_lpm_mngr_sync
#define l280 soc_sand_bitstream_get_any_field
#define l328 continue
#define l357 SOC_SAND_FAILURE_OUT_OF_RESOURCES
#define l368 pcp_arr_mem_allocator_commit
#define l393 soc_sand_group_mem_ll_commit
#define l372 soc_sand_pat_tree_cache_commit
#define l371 pcp_arr_mem_allocator_cache_set
#define l378 soc_sand_group_mem_ll_cache_set
#define l384 PCP_FRWRD_IP_ALL_VRFS_ID
#define l327 soc_sand_pat_tree_cache_set
#define l352 pcp_ipv4_lpm_mngr_prefix_remove
#define l323 pcp_ipv4_lpm_mngr_prefix_lookup
#define l380 pcp_ipv4_lpm_mngr_prefix_is_exist
#define l398 pcp_ipv4_lpm_mngr_sys_fec_get
#define l367 pcp_ipv4_lpm_mngr_get_stat
#define l269 PCP_IPV4_LPM_MNGR_STATUS
#define l252 SOC_SAND_CHECK_NULL_INPUT
#define l385 soc_sand_pat_tree_get_size
#define l287 total_nodes
#define l326 prefix_nodes
#define l314 free_nodes
#define l402 pcp_ipv4_lpm_mngr_clear
#define l383 pcp_ipv4_lpm_mngr_get_block
#define l387 SOC_SAND_PAT_TREE_ITER_TYPE
#define l345 PCP_ALLOC
#define l373 while
#define l351 SOC_SAND_PAT_TREE_ITER_IS_END
#define l382 soc_sand_pat_tree_get_block
#define l386 PCP_FREE
#define l399 pcp_PCP_IPV4_LPM_MNGR_INFO_clear
#define l114 info
#define l337 SOC_SAND_MAGIC_NUM_SET
#define l331 SOC_SAND_VOID_EXIT_AND_SEND_ERROR
#define l348 pcp_PCP_IPV4_LPM_MNGR_STATUS_clear
l204 l60 l356;l204 ld l9;l204 l353{l275,l82,l111,l46,l19,l74,l81,l13,
l126,l101}l140;l204 l292 l41;l4 l60 l202[]={0,35,21,-1,35,-1,35};l4 ld
l235[]={0,36,22,22,36,32,36};l204 l295{l295{ld l17;l0 l23;l0 l354;} *
l109;ld l173;}l209;l4 l66 l43(l12 ly*l30,lg ly*l34){l341(l30->lh,l34
->lh,l146( *l30));}l4 l60 l106(lg lz*la,l12 ly*l30,lg ly*l34){li(l30
->lh[0]!=l34->lh[0]||l30->lh[1]!=l34->lh[1]);}l4 l0 l324(lg lz*la,lg
l147*l32){ld l3;le(l235[la->lr.ln]<=32){l3=l32->lh[0]&(0xffffffff>>(
32-l235[la->lr.ln]));}lk{l3=l32->lh[0];l3^=l32->lh[1]&(0xffffffff>>(
64-l235[la->lr.ln]));}l3=l3^l3>>16;l3=l3^l3>>8;l3=l3^l3>>4;l3=l3^l3>>
2;l3=l3^l3>>1;li(((l0)l3)&1);}l4 l66 l203(lg lz*la,l20 l147*l32){ld l3
;lg l16*lf;lf=&(la->lr);le(l202[lf->ln]<0){li;}l3=l324(la,l32);le(
l202[lf->ln]<32){l32->lh[0]^=l3<<l202[lf->ln];}lk{l32->lh[1]^=l3<<(
l202[lf->ln]-32);}}l4 l0 l317(lg l147*l32){ld l3;l3=l32->lh[0]&
0x3ffff;l3=l3^l3>>16;l3=l3^l3>>8;l3=l3^l3>>4;l3=l3^l3>>2;l3=l3^l3>>1;
li(((l0)l3)&1);}l4 l66 l306(l20 l147*l32){ld l3;l3=l317(l32);l32->lh[
0]^=l3<<17;}l4 l0 l318(lg l147*l32){ld l3;l3=l32->lh[0]&0xfffc0000;l3
^=l32->lh[1]&0x0000000f;l3=l3^l3>>16;l3=l3^l3>>8;l3=l3^l3>>4;l3=l3^l3
>>2;l3=l3^l3>>1;li(((l0)l3)&1);}l4 l66 l305(l20 l147*l32){ld l3;l3=
l318(l32);l32->lh[1]^=l3<<3;}l4 l60 l85(ld l103){l60 l62=0;le(l103==0
)li(0x10000000);le(l103>=(1<<16)){l62+=16;l103>>=16;}le(l103>=(1<<8)){
l62+=8;l103>>=8;}le(l103>=(1<<4)){l62+=4;l103>>=4;}le(l103>=(1<<2)){
l62+=2;l103>>=2;}le(l103>=(1<<1)){l62+=1;l103>>=1;}li(l62);}l4 l66
l294(lg lz*la,l12 ly*lc,lg ld lb,lg ld lq,lg ld l40,lg ld lu){l60 l54
;lg l16*lf;lf=&(la->lr);le(lf->ln==l47||lf->ln==l53){le(lq==4){l54=
l85(lf->l143[lf->l77[lb]]-1)-16;le(l54>0){lw(lu==((l41)(-1))||(lu>>
/* Old code, almost not in use - ignore coverity defects */
/* coverity[large_shift] */
l54)<<l54==lu);lc->lh[0]=lu>>l54;}lk le(l54<0){lc->lh[0]=lu<<-l54;}}
lk{lc->lh[0]=lu&0x1fffff;}le(lq==4){lc->lh[0]|=l40<<17;lc->lh[1]=l40
>>15;}lk{lc->lh[0]|=l40<<21;}le(lq==4){lc->lh[1]|=0x4;}lk le(lq==3){
lc->lh[0]|=1<<31;lc->lh[1]=0x6;}lk le(lq==2){lc->lh[1]=0x7;}l203(la,
lc);}lk le(lf->ln==l56){lw(lq==3);lw(lu==((l41)(-1))||lu%2==0);lc->lh
[0]=(lu>>1)&0x1fff;lc->lh[0]|=(l40&0xff)<<13;lc->lh[0]|=0x400000;lc->
lh[1]=0;}lk le(lf->ln==l71){lw(lq==2);lw(lu==((l41)(-1))||lu%2==0);lc
->lh[0]=(lu>>1)&0x3fff;lc->lh[0]|=(l40&0xf)<<14;lc->lh[0]|=0x180000;
lc->lh[1]=0;}lk le(lf->ln==l59){lw(lq==4);lw(lu==((l41)(-1))||lu%4==0
);lc->lh[0]=(lu>>2)&0x1fff;lc->lh[0]|=(l40&0xffff)<<13;lc->lh[0]|=
0x40000000;}lk le(lf->ln==l36){lw(lq==4);lw(lu==((l41)(-1))||lu%2==0);
lc->lh[0]=(lu>>1)&0x3ffff;lc->lh[0]|=(l40&0xffff)<<18;lc->lh[1]=0;lc
->lh[1]|=(l40&0xffff)>>14;}lk{lw(0);}}l4 l66 l303(lg lz*la,l12 ly*lc,
lg ld lb,lg ld lq,lg ld l5,lg ld lu){l60 l54;lg l16*lf;lf=&(la->lr);
le(lf->ln==l47||lf->ln==l53){lw(lq==4);l54=l85(lf->l143[lf->l77[lb]]-
/* Old code, almost not in use - ignore coverity defects */
/* coverity[large_shift] */
1)-17;le(l54>0){lw(lu==((l41)(-1))||(lu>>l54)<<l54==lu);lc->lh[0]=lu
>>l54;}lk le(l54<0){lc->lh[0]=lu<<-l54;}lc->lh[0]|=l5<<18;lc->lh[1]=
l5>>14;l203(la,lc);}lk le(lf->ln==l56){lw(lq==3);lc->lh[0]=lu&0x3fff;
lc->lh[0]|=l5<<14;lc->lh[1]=0;}lk le(lf->ln==l59){lw(lq==4);lw(lu==((
l41)(-1))||lu%2==0);lc->lh[0]=(lu>>1)&0x3fff;lc->lh[0]|=l5<<14;lc->lh
[1]=0;}lk{lw(0);}}l4 l66 l299(lg lz*la,l12 ly*lc,lg ld lb,lg ld lq,lg
ld l5,lg ld lu){lg l16*lf;lf=&(la->lr);le(lf->ln==l36){lw(lq==4);lw(
lu==((l41)(-1))||lu%2==0);lc->lh[0]=(lu>>1)&0x3ffff;lc->lh[0]|=(l5&
0x7fff)<<18;lc->lh[1]=0;lc->lh[1]|=(l5&0x7fff)>>14;lc->lh[1]|=1<<2;}
lk{lw(0);}}l4 l66 l334(lg lz*la,l12 ly*lc,lg ld lb,lg ld l27,lg ld l2
,lg ld lu){lg l16*lf;lf=&(la->lr);le(lf->ln==l47||lf->ln==l53){lc->lh
[0]=lu&0x1fffff;lc->lh[0]|=(l2<<(30-l27))&0x3fffffff;lc->lh[0]|=1<<(
29-l27);lc->lh[1]=0x6;l203(la,lc);}lk le(lf->ln==l56){lw(lu==((l41)(-
1))||lu%2==0);lw(l27<=6);lw(l27>0);lw((l2&((1<<l27)-1))==l2);lc->lh[0
]=(lu>>1)&0x1fff;lc->lh[0]|=(l2<<(20-l27))&0xfffff;lc->lh[0]|=1<<(19-
l27);lc->lh[0]|=0x600000;lc->lh[1]=0;}lk le(lf->ln==l71){lw(lu==((l41
)(-1))||lu%2==0);lw(l27<=5);lw(l27>0);lw((l2&((1<<l27)-1))==l2);le(
l27==5){lc->lh[0]=(lu>>1)&0x3fff;lc->lh[0]|=(l2<<(19-l27))&0x7ffff;lc
->lh[0]|=0x100000;lc->lh[1]=0;}lk{lc->lh[0]=(lu>>1)&0x3fff;lc->lh[0]
|=(l2<<(18-l27))&0x3ffff;lc->lh[0]|=(l27-1)<<18;lc->lh[1]=0;}}lk le(
lf->ln==l59){lw(lu==((l41)(-1))||lu%2==0);lw(l27<=7);lw(l27>0);lw((l2
&((1<<l27)-1))==l2);lc->lh[0]=(lu>>1)&0x3fff;lc->lh[0]|=(l2<<(21-l27));
lc->lh[0]|=((1<<(l27-1))-1)<<(28-l27);lc->lh[0]|=0x60000000;lc->lh[1]
=0;}lk le(lf->ln==l36){ld l160;lw(l27<=7);lw(l27>0);l160=l2<<1;l160|=
1;l160=l160<<(7-l27);lc->lh[0]=(lu)&0x1fffff;lc->lh[0]|=((l160&0xff)<<
21);lc->lh[0]|=(((l27-1)&0x7)<<29);lc->lh[1]=0x6;}lk{lw(0);}}l4 l66
l316(l20 ly*lc,lg ld l229){lw((lc->lh[1]&0x00000fff)==lc->lh[1]);lc->
lh[1]&=0x00000fff;lc->lh[1]|=l229<<12;}l66 l395(l20 ly*lc){lc->lh[1]
&=0x00000fff;}l4 l66 l174(lg lz*la,l12 ly*lc,lg ld lb,lg ld l35,lg ld
l229){lg l16*lf;lf=&(la->lr);le(lf->ln==l47||lf->ln==l53){lw((l35&
0xfffff)==l35);lc->lh[0]=l35&0xfffff;lc->lh[0]|=1<<31;lc->lh[1]=0x7;
l203(la,lc);}lk le(lf->ln==l56){lw((l35&0x3fff)==l35);le(lb>=5){lc->
lh[0]=l35&0x3fff;}lk{lc->lh[0]=l35&0x3fff;lc->lh[0]|=0x700000;}lc->lh
[1]=0x0;}lk le(lf->ln==l59){lw((l35&0x7fff)==l35);lc->lh[0]=l35&
0x7fff;lc->lh[0]|=0x70000000;lc->lh[1]=0x0;}lk le(lf->ln==l71){lw((
l35&0x3fff)==l35);lc->lh[0]=l35&0x3fff;lc->lh[0]|=0x1c0000;lc->lh[1]=
0x0;}lk le(lf->ln==l36){lw((l35&0xffffffff)==l35);lc->lh[0]=(l35&
0xffffffff);lc->lh[1]=7;}lk{lw(0);}l316(lc,l229);}l4 l140 l15(lg lz*
la,lg ly*lc,lg ld lb){le(la->lr.ln==l47||la->lr.ln==l53){le(lb==3)li(
l126);lk le((lc->lh[1]&0x4)==0)li(l82);lk le((lc->lh[1]&0x6)==0x4)li(
l46);lk le((lc->lh[1]&0x7)==0x6&&(lc->lh[0]&0x80000000)==0)li(l19);lk
le((lc->lh[1]&0x7)==0x6&&(lc->lh[0]&0x80000000)==0x80000000)li(l74);
lk le((lc->lh[1]&0x7)==0x7&&(lc->lh[0]&0x80000000)==0)li(l81);lk le((
lc->lh[1]&0x7)==0x7&&(lc->lh[0]&0x80000000)==0x80000000)li(l13);}lk le
(la->lr.ln==l56){le(lb>=5){li(l13);}lk le((lc->lh[0]&0x400000)==0)li(
l111);lk le((lc->lh[0]&0x600000)==0x400000)li(l74);lk le((lc->lh[0]&
0x700000)==0x600000)li(l19);lk le((lc->lh[0]&0x700000)==0x700000)li(
l13);}lk le(la->lr.ln==l71){le((lc->lh[0]&0x100000)==0)li(l19);lk le(
(lc->lh[0]&0x180000)==0x100000)li(l19);lk le((lc->lh[0]&0x1c0000)==
0x180000)li(l81);lk le((lc->lh[0]&0x1c0000)==0x1c0000)li(l13);}lk le(
la->lr.ln==l59){le(lb>=5)li(l13);lk le((lc->lh[0]&0x40000000)==0)li(
l82);lk le((lc->lh[0]&0x60000000)==0x40000000)li(l46);lk le((lc->lh[0
]&0x70000000)==0x60000000)li(l19);lk le((lc->lh[0]&0x70000000)==
0x70000000)li(l13);}lk le(la->lr.ln==l36){le((lc->lh[1]&0x4)==0x0)li(
l46);lk le((lc->lh[1]&0x6)==0x4)li(l101);lk le((lc->lh[1]&0x7)==0x6)li
(l19);lk le((lc->lh[1]&0x7)==0x7)li(l13);}lw(0);li(l275);}l4 l0 l170(
lg lz*la,lg ly*lc,lg ld lb){l140 l102;l102=l15(la,lc,lb);li((l0)((
l102==l13)||(l102==l126)));}l66 l397(lg lz*la,lg ld lb,lg l246*l26,lg
ld l281,l12 ld*l148){ld l141;l140 l102;ly lc;l7(l141=0;l141<l281;++
l141){lc.lh[l141]=l26[l141];l148[l141]=l26[l141];}l102=l15(la,&lc,lb);
le(l102==l13){le(la->lr.ln==l71){le(lb>=5){l148[0]= *l26&0x3fff;}lk{
l148[0]= *l26&0x3fff;l148[0]|=0x700000;}}lk le(la->lr.ln==l36){l148[0
]=l26[0];l148[1]=7;}}}l4 ld l210(lg lz*la,lg ld lb,l12 ld*l17){lg l16
 *lf;ld l58=0;ld l117;ld l42;l42= *l17;lf=&(la->lr);le(lf->ln==l36&&
lb>0){l117=(lf->l95-1)/(lf->l164-1);le(l117==1){l58=lf->l77[lb];}lk le
(l117==2){l58=lf->l77[lb]+((l42>>19)&0x1);}lk le(l117==4){l58=lf->l77
[lb]+((l42>>19)&0x3);}lk{lw(0);} *l17=( *l17&0x7ffff);}lk{l58=lf->l77
[lb];}li l58;}l4 ld l108(lg lz*la,lg ld lb,lg ld l17,l12 ly*lc,lg ld
l92){l60 l99;ld l42;ly*l142;ly l91;l154*l90;l246 l69[2];ld lj;lg l16*
lf;ld l58;l33(0);lf=&(la->lr);l99=l92;l142=lc;l42=l17;l58=l210(la,lb,
&l42);lj=lf->l159(lf->l50,lf->l65,l58,&l90);lv(lj,10,lp);le((lf->ln==
l47||lf->ln==l53)&&lb>=3){l42/=2;l7(;l99>0;l99-=2){lj=l260(l90,l42,
l69);lv(lj,20,lp);l91.lh[0]=l69[0];l91.lh[1]=l69[1];++l42;l174(la,&(
l142[0]),lb,l91.lh[0]&0x1ffff,(0));l174(la,&(l142[1]),lb,(l91.lh[0]>>
18)|((l91.lh[1]&0x7)<<14),(0));l142+=2;}}lk{l7(;l99>0;l99--){lj=l260(
l90,l42,l69);lv(lj,20,lp);l142->lh[0]=l69[0];l142->lh[1]=l69[1];++l42
;++l142;}}lp:li l270;}l4 ld l128(lg lz*la,lg ld lb,lg ld l17,lg ly*lc
,lg ld l92){ly l91;l60 l99;ld l42;l154*l90;l246 l69[2],l212[2];ld lj;
lg l16*lf;ld l58;l33(0);l217(&l91,0x0,l146(ly));lf=&(la->lr);l99=(l60
)l92;l42=l17;l58=l210(la,lb,&l42);lj=lf->l159(lf->l50,lf->l65,l58,&
l90);lv(lj,10,lp);le((lf->ln!=l47&&lf->ln!=l53)||lb<3){l7(;l99>0;l99
--){le(lb>=5&&lf->ln==l56){l69[0]=lc->lh[0]&0x3fff;l69[1]=0;}lk le(lb
>=5&&lf->ln==l59){l69[0]=lc->lh[0]&0x7fff;l69[1]=0;}lk{l69[0]=lc->lh[
0];l69[1]=lc->lh[1];}lj=l260(l90,l42,l212);lv(lj,20,lp);le(l212[0]!=
l69[0]||l212[1]!=l69[1]){lj=l286(l90,l42,l69);lv(lj,20,lp);}++l42;++
lc;}}lk{l42/=2;l7(;l99>0;l99-=2){l91.lh[0]=0x20000|(lc[0].lh[0]&
0x1ffff);le(l99>1){l91.lh[0]|=lc[1].lh[0]<<18;l91.lh[1]=(lc[1].lh[0]
>>14)&0x7;}l306(&l91);l305(&l91);l69[0]=l91.lh[0];l69[1]=l91.lh[1];lj
=l286(l90,l42,l69);lv(lj,20,lp);++l42;lc+=2;}}lp:l45("\x65\x72\x72"
"\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70"
"\x6d\x5f\x6d\x6e\x67\x72\x5f\x77\x72\x69\x74\x65\x5f\x72\x6f\x77\x73"
"\x28\x29",0,0);}l4 l0 l293(lg lz*la,l12 ly*l30,lg ly*l34,lg ld lb){
lg l16*lf;lf=&(la->lr);le(lf->ln==l47||lf->ln==l53){l6(l15(la,l34,lb)){
lm l82:li(l0)(((l34->lh[1]&0x7)==(l30->lh[1]&0x7))&&((l34->lh[0]&~
0x3ffff)==(l30->lh[0]&~0x3ffff)));lm l46:li(l0)(((l34->lh[1]&0x7)==(
l30->lh[1]&0x7))&&((l34->lh[0]&~0x1ffff)==(l30->lh[0]&~0x1ffff)));lm
l19:li(l0)(((l34->lh[1]&0x7)==(l30->lh[1]&0x7))&&((l34->lh[0]&~
0x1ffff)==(l30->lh[0]&~0x1ffff)));lm l74:li(l0)(((l34->lh[1]&0x7)==(
l30->lh[1]&0x7))&&((l34->lh[0]&~0x1ffff)==(l30->lh[0]&~0x1ffff)));lm
l81:li(l0)(((l34->lh[1]&0x7)==(l30->lh[1]&0x7))&&((l34->lh[0]&~
0x1ffff)==(l30->lh[0]&~0x1ffff)));lm l13:li(l0)(l15(la,l30,lb)==l13);
lm l126:li(l0)(l15(la,l30,lb)==l126);l22:lw(0);li(l39);}}lk le(lf->ln
==l56){l6(l15(la,l34,lb)){lm l111:li((l0)((l34->lh[0]&~0x3fff)==(l30
->lh[0]&~0x3fff)));lm l74:li((l0)((l34->lh[0]&~0x1fff)==(l30->lh[0]&~
0x1fff)));lm l19:li((l0)((l34->lh[0]&~0x1fff)==(l30->lh[0]&~0x1fff)));
lm l13:li((l0)(l15(la,l30,lb)==l13));l22:lw(0);li(l39);}}lk le(lf->ln
==l71){l6(l15(la,l34,lb)){lm l81:lm l19:lm l13:li((l0)((l34->lh[0]&
0x1fc000)==(l30->lh[0]&0x1fc000)));l22:lw(0);li(l39);}}lk le(lf->ln==
l59){l6(l15(la,l34,lb)){lm l82:lm l19:li((l0)((l34->lh[0]&~0x3fff)==(
l30->lh[0]&~0x3fff)));lm l46:li((l0)((l34->lh[0]&~0x1fff)==(l30->lh[0
]&~0x1fff)));lm l13:li((l0)(l15(la,l30,lb)==l13));l22:lw(0);li(l39);}
}lk le(lf->ln==l36){l6(l15(la,l34,lb)){lm l101:lm l46:li(l0)(((l34->
lh[0]&0xfffa0000)==(l30->lh[0]&0xfffa0000))&&((l34->lh[1]&0x7)==(l30
->lh[1]&0x7)));lm l19:li(l0)(((l34->lh[0]&0xffe00000)==(l30->lh[0]&
0xffe00000))&&((l34->lh[1]&0x7)==(l30->lh[1]&0x7)));lm l13:li((l0)(
l15(la,l30,lb)==l13));l22:lw(0);li(l39);}}lk{lw(0);li(l39);}}l60 l100
(lg lz*la,lg ly*lc,lg ld lb,lg ld l21){l60 l54;lg l16*lf;lf=&(la->lr);
le(lf->ln==l47||lf->ln==l53){l6(l15(la,lc,lb)){lm l82:l54=l85(lf->
l143[lf->l77[lb]]-1)-17;li(l54>0?(lc->lh[0]&0x3ffff)<<l54:(lc->lh[0]&
/* Old code, almost not in use - ignore coverity defects */
/* coverity[negative_shift] */
0x3ffff)>>l54);lm l46:l54=l85(lf->l143[lf->l77[lb]]-1)-16;li(l54>0?(
lc->lh[0]&0x1ffff)<<l54:(lc->lh[0]&0x1ffff)>>l54);lm l19:li(lc->lh[0]
&0x1fffff);lm l74:li(lc->lh[0]&0x1fffff);lm l81:li(lc->lh[0]&0x1fffff
);lm l13:li(-1);lm l126:li(-1);l22:lw(0);li(-1);}}lk le(lf->ln==l56){
l6(l15(la,lc,lb)){lm l111:li(lc->lh[0]&0x3fff);lm l74:lm l19:li((lc->
lh[0]&0x1fff) *2);lm l13:li(-1);l22:lw(0);li(-1);}}lk le(lf->ln==l59){
l6(l15(la,lc,lb)){lm l82:lm l19:li((lc->lh[0]&0x3fff) *2);lm l46:li((
lc->lh[0]&0x1fff) *4);lm l13:li(-1);l22:lw(0);li(-1);}}lk le(lf->ln==
l71){l6(l15(la,lc,lb)){lm l81:lm l19:li((lc->lh[0]&0x3fff) *2);lm l13
:li(-1);l22:lw(0);li(-1);}}lk le(lf->ln==l36){ld l187=0;ld l241=(lf->
l95-1)/(lf->l164-1);le(l241==1){l187=l21&0x0;}lk le(l241==2){l187=l21
&0x1;}lk le(l241==4){l187=l21&0x3;}lk{lw(0);}l6(l15(la,lc,lb)){lm l101
:lm l46:li((l187<<19)|((lc->lh[0]&0x3ffff) *2));lm l19:li(lc->lh[0]&
0x1fffff);lm l13:li(-1);l22:lw(0);li(l39);}}lk{lw(0);li(-1);}}l60 l358
(lg lz*la,l20 ly*lc,lg ld lb,lg ld l189){le(la->lr.ln==l56){l6(l15(la
,lc,lb)){lm l74:lm l19:lc->lh[0]=((lc->lh[0]&~0x1fff)|l189/2);li lc->
lh[0];lm l111:li lc->lh[0]=((lc->lh[0]&~0x3fff)|l189);lm l13:li(-1);
l22:li(-1);}}lk le(la->lr.ln==l36){l6(l15(la,lc,lb)){lm l46:lm l101:
lc->lh[0]=((lc->lh[0]&~0x3ffff)|l189/2);li lc->lh[0];lm l19:lc->lh[0]
=((lc->lh[0]&~0x1fffff)|l189);li lc->lh[0];lm l13:li(-1);l22:li(-1);}
}lk{li(-1);}}l4 ld l228(lg lz*la,lg ld lb,lg ld l5,lg ld lq){ld lx;ld
l62;ld l137=l5;l137<<=1;l137|=1;l7(l62=0,lx=0;lx<(ld)(1<<lq);lx++){l6
((l137>>lx)&0x3){lm 0x0:lm 0x2:l8;lm 0x1:l62++;l8;lm 0x3:lw(la->lt.ll
[lb+2]>=la->lt.ll[lb+1]+lq);l62+=1<<(la->lt.ll[lb+2]-la->lt.ll[lb+1]-
lq);l8;}}li(l62);}l4 ld l338(lg lz*la,lg ld lb,lg ld l5,lg ld lq){ld
lx;ld l62;ld l137=l5;ld l240;le((la->lt.ll[lb+2]-la->lt.ll[lb+1])<=lq
)l240=1;lk l240=1<<(la->lt.ll[lb+2]-la->lt.ll[lb+1]-lq);l137<<=1;l137
|=1;l7(l62=0,lx=0;lx<((ld)(1<<lq)-1);lx++){l6((l137>>lx)&0x1){lm 0x0:
l8;lm 0x1:l62+=l240;l8;}}li(l62);}l4 ld l131(lg lz*la,lg ld lb,lg ld
l40,lg ld lq){ld lx;ld l62;l7(l62=0,lx=0;lx<(ld)(1<<lq);){le((l40>>lx
)&1){le(la->lt.ll[lb+2]-la->lt.ll[lb+1]<lq)l62++;lk l62+=1<<(la->lt.
ll[lb+2]-la->lt.ll[lb+1]-lq);}lk{l62++;}le(lq>la->lt.ll[lb+2]-la->lt.
ll[lb+1])lx+=1<<(lq-(la->lt.ll[lb+2]-la->lt.ll[lb+1]));lk lx++;}li(
l62);}l4 ld l321(lg lz*la,lg ly*lc,lg ld lb){lg l16*lf;lf=&(la->lr);
le(lf->ln==l47||lf->ln==l53){l6(l15(la,lc,lb)){lm l82:li(l228(la,lb,(
lc->lh[0]>>18)|(lc->lh[1]<<14),4));lm l46:li(l131(la,lb,(lc->lh[0]>>
17)|(lc->lh[1]<<15),4));lm l19:li(2);lm l74:li(l131(la,lb,lc->lh[0]>>
21,3));lm l81:li(l131(la,lb,lc->lh[0]>>21,2));lm l13:li(0);lm l126:li
(0);l22:li(0);}}lk le(lf->ln==l56){l6(l15(la,lc,lb)){lm l111:li(l228(
la,lb,lc->lh[0]>>14,3));lm l74:li(l131(la,lb,lc->lh[0]>>13,3));lm l19
:li(2);lm l13:li(0);l22:lw(0);li(0);}}lk le(lf->ln==l59){l6(l15(la,lc
,lb)){lm l82:li(l228(la,lb,lc->lh[0]>>14,4));lm l46:li(l131(la,lb,lc
->lh[0]>>13,4));lm l19:li(2);lm l13:li(0);l22:lw(0);li(0);}}lk le(lf
->ln==l71){l6(l15(la,lc,lb)){lm l81:li(l131(la,lb,lc->lh[0]>>14,2));
lm l19:li(2);lm l13:li(0);l22:lw(0);li(0);}}lk le(lf->ln==l36){ld l248
=((lc->lh[1]&0x3)<<14)+(lc->lh[0]>>18);l6(l15(la,lc,lb)){lm l46:li(
l131(la,lb,l248,4));lm l19:li(2);lm l101:li(l338(la,lb,l248,4));lm l13
:li(0);l22:lw(0);li(0);}}lk{lw(0);li(0);}}l4 ld l132(lg lz*la,lg ly*
lc,lg ld lb,lg ld lq,l12 ly*ls,lg ld l21){ld lx,l57;ld lu;ld l1;ld l40
=0;ld lj;ld l29;ld l149;lg l16*lf;lg l86*lo;l33(0);lf=&(la->lr);lo=&(
la->lt);le((la->lr.ln==l59||la->lr.ln==l36)&&lo->ll[lb+2]-lo->ll[lb+1
]<5){l190;}l149=1<<lq;lu=l100(la,lc,lb,l21);le(lo->ll[lb+2]-lo->ll[lb
+1]<lq){l1=1;}lk{l1=1<<(lo->ll[lb+2]-lo->ll[lb+1]-lq);}le(lf->ln==l47
||lf->ln==l53){le(lq==4)l40=lc->lh[0]>>17;lk l40=lc->lh[0]>>21;}lk le
(lf->ln==l56){l40=lc->lh[0]>>13;}lk le(lf->ln==l59){l40=lc->lh[0]>>13
;}lk le(lf->ln==l71){l40=lc->lh[0]>>14;}lk le(lf->ln==l36){l40=(lc->
lh[0]>>18)+((lc->lh[1]&0x3)<<14);}lk{lw(0);}l40&=(1<<l149)-1;l29=0;l7
(l57=0;l57<l149;){le((l40>>l57)&1){lj=l108(la,lb+1,lu,&ls[l29],l1);lv
(lj,10,lp);lu+=l1;}lk{lj=l108(la,lb+1,lu,&ls[l29],1);lv(lj,20,lp);l7(
lx=1;lx<l1;lx++)l43(&ls[l29+lx],&ls[l29]);lu+=1;}l29+=l1;le(lq>lo->ll
[lb+2]-lo->ll[lb+1])l57+=1<<(lq-(lo->ll[lb+2]-lo->ll[lb+1]));lk l57++
;}lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69"
"\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f"
"\x74\x6f\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x65\x6c\x69"
"\x64\x65\x5f\x6c\x67\x6e\x28\x29",0,0);}l4 ld l216(lg lz*la,lg ly*lc
,lg ld lb,lg ld lq,l12 ly*ls,lg ld l21){ld lx,l57;ld lu;ld l1;ld l5=0
;ld lj;ld l29;ld l63;lg l16*lf;lg l86*lo;l33(0);lf=&(la->lr);lo=&(la
->lt);lu=l100(la,lc,lb,l21);l63=1<<lq;le(lo->ll[lb+2]-lo->ll[lb+1]<lq
){l1=1;}lk{l1=1<<(lo->ll[lb+2]-lo->ll[lb+1]-lq);}le(lf->ln==l47||lf->
ln==l53)l5=(lc->lh[0]>>18)|(lc->lh[1]<<14);lk le(lf->ln==l56){l5=lc->
lh[0]>>14;}lk le(lf->ln==l59){l5=lc->lh[0]>>14;}lk{lw(0);}l5&=(1<<l63
)-1;l29=0;l5<<=1;l5|=1;l7(l57=0;l57<l63;){l6((l5>>l57)&0x3){lm 0x0:lm
0x2:l7(lx=0;lx<l1;lx++){l43(&ls[l29+lx],&ls[l29-1]);}l8;lm 0x1:lj=
l108(la,lb+1,lu,&ls[l29],1);lv(lj,10,lp);l7(lx=1;lx<l1;lx++){l43(&ls[
l29+lx],&ls[l29]);}lu+=1;l8;lm 0x3:lj=l108(la,lb+1,lu,&ls[l29],l1);lv
(lj,20,lp);lu+=l1;l8;}l29+=l1;le(lq>lo->ll[lb+2]-lo->ll[lb+1])l57+=1
<<(lq-(lo->ll[lb+2]-lo->ll[lb+1]));lk l57++;}lp:l45("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f\x74\x6f\x5f\x66\x75\x6c\x6c"
"\x5f\x65\x6c\x69\x64\x65\x5f\x63\x6f\x64\x65\x5f\x6c\x67\x6e\x28\x29"
,0,0);}l4 ld l313(lg lz*la,lg ly*lc,lg ld lb,lg ld lq,l12 ly*ls,lg ld
l21){ld lx,l57;ld lu;ld l1;ld l5=0;ld lj;ld l29;ld l63;lg l16*lf;lg
l86*lo;l33(0);lf=&(la->lr);lo=&(la->lt);lu=l100(la,lc,lb,l21);l63=1<<
lq;le(lo->ll[lb+2]-lo->ll[lb+1]<lq){l1=1;}lk{l1=1<<(lo->ll[lb+2]-lo->
ll[lb+1]-lq);}le(lf->ln==l36){l5=(lc->lh[0]>>18)+((lc->lh[1]&0x1)<<14
);}lk{lw(0);}l5&=(1<<l63)-1;l29=0;l5<<=1;l5|=1;l7(l57=0;l57<l63;){l6(
(l5>>l57)&0x1){lm 0x0:l7(lx=0;lx<l1;lx++){l43(&ls[l29+lx],&ls[l29-1]);
}l8;lm 0x1:lj=l108(la,lb+1,lu,&ls[l29],l1);lv(lj,10,lp);lu+=l1;l8;}
l29+=l1;le(lq>lo->ll[lb+2]-lo->ll[lb+1])l57+=1<<(lq-(lo->ll[lb+2]-lo
->ll[lb+1]));lk l57++;}lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72"
"\x5f\x72\x6f\x77\x5f\x74\x6f\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64"
"\x65\x5f\x63\x6f\x64\x65\x5f\x31\x5f\x6c\x67\x6e\x28\x29",0,0);}l4 l9
l232(lg lz*la,lg ly*lc,lg ld lb){le(la->lr.ln==l47||la->lr.ln==l53){
li((lc->lh[0]>>21)&0x1ff);}lk le(la->lr.ln==l56){li((lc->lh[0]>>13)&
0x7f);}lk le(la->lr.ln==l59){ld l2;l2=(lc->lh[0]>>14)&0x7f;lw((((~lc
->lh[0]>>21)&0x3f)&(((~lc->lh[0]>>21)&0x3f)+1))==0);l2&=0x40|((lc->lh
[0]>>21)&0x3f);l2<<=1;l2|=(((~lc->lh[0]>>21)&0x3f)+1);li(l2);}lk le(
la->lr.ln==l71){ld l2;le((lc->lh[0]&0x100000)==0){l2=(lc->lh[0]>>14)&
0xf;l2>>=3-((lc->lh[0]>>18)&0x3);l2<<=3-((lc->lh[0]>>18)&0x3);l2<<=1;
l2|=1<<(3-((lc->lh[0]>>18)&0x3));l2<<=1;}lk{l2=(lc->lh[0]>>14)&0x1f;
l2<<=1;l2|=1;}li(l2);}lk le(la->lr.ln==l36){ld l2;ld l198;ld l218;l2=
(lc->lh[0]>>21)&0xff;l218=(lc->lh[0]>>29)&0x7;l198=l2|(1<<(6-l218));
l198=l198&(0xff<<(6-l218));lw(l198==l2);li l2;}lw(0);li(l9)(-1);}l4 ld
l166(lg lz*la,lg ly*lc,lg ld lb,l12 ly*ls,lg ld l21){ld lu;ld l31;ld
lj;ld l29;ld l2=0;ld l27=0;ld l104;ld l221;ld l206;lg l16*lf;lg l86*
lo;l33(0);lf=&(la->lr);lo=&(la->lt);lu=l100(la,lc,lb,l21);l31=1<<(lo
->ll[lb+2]-lo->ll[lb+1]);l2=l232(la,lc,lb);le(lf->ln==l47||lf->ln==
l53){l27=8;}lk le(lf->ln==l56){l27=6;}lk le(lf->ln==l59){l27=7;}lk le
(lf->ln==l71){l27=5;}lk le(lf->ln==l36){l27=7;}lk{lw(0);}le((lo->ll[
lb+2]-lo->ll[lb+1])<l27){l2>>=l27-(lo->ll[lb+2]-lo->ll[lb+1]);}lk le(
(lo->ll[lb+2]-lo->ll[lb+1])>l27){l2<<=(lo->ll[lb+2]-lo->ll[lb+1])-l27
;}l104=((l2-1)^l2);l104>>=1;l2>>=1;l2&=~l104;l221=l2;le(l2==0){l206=
l2+l104+1;}lk{l206=0;}l7(l29=0;l29<l31;l29++){le((l29&~l104)==(l2&~
l104)){le(l29==l221){lj=l108(la,lb+1,lu+1,&ls[l29],1);lv(lj,10,lp);}
lk{l43(&ls[l29],&ls[l221]);}}lk{le(l29==l206){lj=l108(la,lb+1,lu,&ls[
l29],1);lv(lj,20,lp);}lk{l43(&ls[l29],&ls[l206]);}}}lp:l45("\x65\x72"
"\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c"
"\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f\x74\x6f\x5f\x66\x75"
"\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x63\x6f\x6d\x70\x61\x72\x65\x28"
"\x29",0,0);}l4 l66 l181(lg lz*la,lg ly*lc,lg ld lb,l20 ly*ls,lg ld
l21){ld l31;ld l57;lg l86*lo;lo=&(la->lt);l31=1<<(lo->ll[lb+2]-lo->ll
[lb+1]);l7(l57=0;l57<l31;l57++){l43(&ls[l57],lc);}}l4 l66 l309(lg lz*
la,lg ly*lc,lg ld lb,l20 ly*ls,lg ld l21){lg l16*lf;lf=&(la->lr);le(
lf->ln==l47||lf->ln==l53){l6(l15(la,lc,lb)){lm l82:l216(la,lc,lb,4,ls
,l21);li;lm l46:l132(la,lc,lb,4,ls,l21);li;lm l19:l166(la,lc,lb,ls,
l21);li;lm l74:l132(la,lc,lb,3,ls,l21);li;lm l81:l132(la,lc,lb,2,ls,
l21);li;lm l13:lm l126:l181(la,lc,lb,ls,l21);li;l22:lw(0);li;}}lk le(
lf->ln==l56){l6(l15(la,lc,lb)){lm l111:l216(la,lc,lb,3,ls,l21);li;lm
l74:l132(la,lc,lb,3,ls,l21);li;lm l19:l166(la,lc,lb,ls,l21);li;lm l13
:l181(la,lc,lb,ls,l21);li;l22:lw(0);li;}}lk le(lf->ln==l59){l6(l15(la
,lc,lb)){lm l82:l216(la,lc,lb,4,ls,l21);li;lm l46:l132(la,lc,lb,4,ls,
l21);li;lm l19:l166(la,lc,lb,ls,l21);li;lm l13:l181(la,lc,lb,ls,l21);
li;l22:lw(0);li;}}lk le(lf->ln==l71){l6(l15(la,lc,lb)){lm l81:l132(la
,lc,lb,2,ls,l21);li;lm l19:l166(la,lc,lb,ls,l21);li;lm l13:l181(la,lc
,lb,ls,l21);li;l22:lw(0);li;}}lk le(lf->ln==l36){l6(l15(la,lc,lb)){lm
l46:l132(la,lc,lb,4,ls,l21);li;lm l19:l166(la,lc,lb,ls,l21);li;lm l101
:l313(la,lc,lb,4,ls,l21);li;lm l13:l181(la,lc,lb,ls,l21);li;l22:lw(0);
li;}}lk{lw(0);}lw(0);li;}l4 l60 l138(lg lz*la,l12 ly*lc,lg ld lb,lg ld
lq,l20 ly*ls,lg l41 lu){ld l31;ld l1;ld l18;ld l64;ly l180;l0 l165=1;
ld l40;lg l86*lo;lo=&(la->lt);l31=1<<(lo->ll[lb+2]-lo->ll[lb+1]);le(
lo->ll[lb+2]-lo->ll[lb+1]<lq){li(-1);}lk{l1=1<<(lo->ll[lb+2]-lo->ll[
lb+1]-lq);}l40=0;l7(l18=0,l64=0;l18<l31;){le(l18%l1==0){l165=1;l43(&
l180,&ls[l18]);}lk le(l106(la,&l180,&ls[l18])!=0)l165=0;le(lu!=-1)l43
(&ls[l64],&ls[l18]);l18++;l64++;le(l18%l1==0){le(lo->ll[lb+2]-lo->ll[
lb+1]>=lq){l40|=(~l165&1)<<(((l18-l1)>>(lo->ll[lb+2]-lo->ll[lb+1]-lq)));
}lk{l40|=(~l165&1)<<(((l18-l1)<<(lq-(lo->ll[lb+2]-lo->ll[lb+1]))));}
le(l165)l64-=l1-1;}}l294(la,lc,lb,lq,l40,lu);le(lu!=-1)l128(la,lb+1,
lu,ls,l64);li(l64);}l4 l60 l185(lg lz*la,l12 ly*lc,lg ld lb,lg ld lq,
l20 ly*ls,lg l41 lu){ld l31;ld l1;ld l18;ld l64;ld l5;ld l67=0;ld l163
=0;ld l124=0;lg l86*lo;lo=&(la->lt);l31=1<<(lo->ll[lb+2]-lo->ll[lb+1]
);le(lo->ll[lb+2]-lo->ll[lb+1]<lq){l1=1;}lk{l1=1<<(lo->ll[lb+2]-lo->
ll[lb+1]-lq);}l5=1;l7(l18=0;l18<l31;l18++){le(l18%l1==0){le(lo->ll[lb
+2]-lo->ll[lb+1]>lq)l67=l18>>(lo->ll[lb+2]-lo->ll[lb+1]-lq);lk l67=
l18<<(lq-(lo->ll[lb+2]-lo->ll[lb+1]));l124=l18;l163=l124-1;}lk{le(
l106(la,&ls[l124],&ls[l18])!=0){l5|=0x2<<l67;}}le(l124==0||l106(la,&
ls[l163],&ls[l18])!=0)l5|=0x1<<l67;}l7(l64=0,l67=0,l18=0;l67<(ld)(1<<
lq);){le(l5&(0x1<<l67)){le(l5&(0x2<<l67)){ld lx;l7(lx=0;lx<l1;lx++){
le(lu!=-1)l43(&ls[l64],&ls[l18]);l64++;l18++;}}lk{le(lu!=-1)l43(&ls[
l64],&ls[l18]);l64++;l18+=l1;}}lk{l18+=l1;}le(lq>lo->ll[lb+2]-lo->ll[
lb+1])l67+=1<<(lq-(lo->ll[lb+2]-lo->ll[lb+1]));lk l67++;}l5>>=1;l303(
la,lc,lb,lq,l5,lu);le(lu!=-1)l128(la,lb+1,lu,ls,l64);li(l64);}l4 l60
l272(lg lz*la,l12 ly*lc,lg ld lb,lg ld lq,l20 ly*ls,lg l41 lu){ld l31
;ld l1;ld l18;ld l64;ld l5;ld l67=0;ld l163=0;ld l124=0;ld l98;lg l86
 *lo;lo=&(la->lt);l98=lo->ll[lb+2]-lo->ll[lb+1];l31=1<<(l98);le(l98<
lq){l1=1;}lk{l1=1<<(l98-lq);}l5=1;l7(l18=0;l18<l31;l18++){le(l18%l1==
0){le(l98>lq)l67=l18>>(l98-lq);lk l67=l18<<(lq-l98);l124=l18;l163=
l124-1;}le(l124==0||l106(la,&ls[l163],&ls[l18])!=0)l5|=0x1<<l67;}l7(
l64=0,l67=0,l18=0;l67<(ld)(1<<lq);){le(l5&(0x1<<l67)){ld lx;l7(lx=0;
lx<l1;lx++){le(lu!=-1)l43(&ls[l64],&ls[l18]);l64++;l18++;}}lk{l18+=l1
;}le(lq>l98)l67+=1<<(lq-l98);lk l67++;}l5>>=1;l299(la,lc,lb,lq,l5,lu);
le(lu!=-1)l128(la,lb+1,lu,ls,l64);li(l64);}l4 l60 l271(lg lz*la,l12 ly
 *lc,lg ld lb,l20 ly*ls,lg l41 lu){ld l31;ld l87;ly l177[2];ld l70[2]
;ld l150[2];ld l78;ld l98;lg l86*lo;lg l16*lf;lf=&(la->lr);lo=&(la->
lt);l98=lo->ll[lb+2]-lo->ll[lb+1];l31=1<<l98;l43(&l177[0],&ls[0]);l70
[0]=1;l150[0]=0;l70[1]=0;l7(l87=1;l87<l31&&l106(la,&ls[l87],&l177[0])==
0;l87++){l70[0]++;}le(l87>=l31){li(-1);}l43(&l177[1],&ls[l87]);l150[1
]=l87;l70[1]++;l7(l87++;l87<l31&&l106(la,&ls[l87],&l177[1])==0;l87++){
l70[1]++;}l7(;l87<l31&&l106(la,&ls[l87],&l177[0])==0;l87++){l70[0]++;
}le(l87<l31){li(-1);}le(l70[0]<l70[1])l78=0;lk l78=1;le(((l70[l78]-1)&
l70[l78])!=0||((l70[l78]-1)&l150[l78])!=0){li(-1);}l6(lf->ln){lm l47:
lm l53:le(l85(l31/l70[l78])>8)li(-1);l8;lm l56:le(l85(l31/l70[l78])>6
)li(-1);l8;lm l71:le(l85(l31/l70[l78])>5)li(-1);l8;lm l59:le(l85(l31/
l70[l78])>7)li(-1);l8;lm l36:le(l85(l31/l70[l78])>7)li(-1);l8;l22:lw(
/* Old code, almost not in use - ignore coverity defects */
/* coverity[large_shift] */
0);l8;}l334(la,lc,lb,l85(l31/l70[l78]),l150[l78]>>l85(l70[l78]),lu);
le(lu!=-1){l128(la,lb+1,lu,&ls[l150[1-l78]],1);l128(la,lb+1,lu+1,&ls[
l150[l78]],1);}li(2);}l4 l60 l264(lg lz*la,l12 ly*lc,lg ld lb,l20 ly*
ls){ld l31;ld l183;lg l86*lo;lo=&(la->lt);l31=1<<(lo->ll[lb+2]-lo->ll
[lb+1]);l7(l183=1;l183<l31;l183++){le(l106(la,&ls[0],&ls[l183])!=0)l8
;}l43(lc,&ls[0]);le(l183==l31){li(0);}lk li(-1);}l4 ld l297(lg lz*la,
l12 ly*lc,lg ld lb,l20 ly*ls,l12 l140*l79){l60 l55;l60 l24;ly l123;lg
l16*lf;lf=&(la->lr);l55=(1<<(8))+1;l24=l264(la,lc,lb,ls);le(l24!=-1&&
l24<l55){l55=l24; *l79=l13;li(l55);}l24=l271(la,lc,lb,ls,((l41)(-1)));
le(l24!=-1&&l24<l55){l55=l24; *l79=l19;li(l55);}le(lf->ln==l56||lf->
ln==l47||lf->ln==l53){l24=l138(la,lc,lb,3,ls,((l41)(-1)));le(l24!=-1
&&l24<l55){l55=l24; *l79=l74;l43(&l123,lc);}}le(lf->ln==l56){l24=l185
(la,lc,lb,3,ls,((l41)(-1)));le(l24!=-1&&l24<l55){l55=l24; *l79=l111;
l43(&l123,lc);}}le(lf->ln==l53||lf->ln==l59){l24=l185(la,lc,lb,4,ls,(
(l41)(-1)));le(l24!=-1&&l24<l55){l55=l24; *l79=l82;l43(&l123,lc);}l24
=l138(la,lc,lb,4,ls,((l41)(-1)));le(l24!=-1&&l24<l55){l55=l24; *l79=
l46;l43(&l123,lc);}}le(lf->ln==l53||lf->ln==l71){l24=l138(la,lc,lb,2,
ls,((l41)(-1)));le(l24!=-1&&l24<l55){l55=l24; *l79=l81;l43(&l123,lc);
}}le(lf->ln==l36){l24=l272(la,lc,lb,4,ls,((l41)(-1)));le(l24!=-1&&l24
<l55){l55=l24; *l79=l101;l43(&l123,lc);}l24=l138(la,lc,lb,4,ls,((l41)(
-1)));le(l24!=-1&&l24<l55){l55=l24; *l79=l46;l43(&l123,lc);}}l43(lc,&
l123);lw(l55<(1<<(8))+1);li(l55);}l4 l66 l249(lg lz*la,l12 ly*lc,lg ld
lb,l20 ly*ls,lg l41 lu,l12 l140 l79){l6(l79){lm l82:l185(la,lc,lb,4,
ls,lu);l8;lm l101:l272(la,lc,lb,4,ls,lu);l8;lm l111:l185(la,lc,lb,3,
ls,lu);l8;lm l46:l138(la,lc,lb,4,ls,lu);l8;lm l74:l138(la,lc,lb,3,ls,
lu);l8;lm l81:l138(la,lc,lb,2,ls,lu);l8;lm l19:l271(la,lc,lb,ls,lu);
l8;lm l13:l264(la,lc,lb,ls);l8;l22:lw(0);l8;}}l4 ld l262(l20 lz*la,lg
ld l279,l12 l0*l258){l154*l125;l404 l129;ld l68;lg l16*lf;ld lj=l270;
ld l243;l33(0);lf=&(la->lr);l374(&l129);le(l279==0){l129.l304=2;}lk{
l129.l304=1;}l7(l68=1;l68<lf->l95;++l68){l243=la->lr.l343[l68];lj=lf
->l159(lf->l50,lf->l65,l68,&l125);lv(lj,10,lp);l129.l255[0]=1<<la->lr
.l310[l243];l129.l255[1]=l129.l255[0];lj=l355(l125,&l129,l258);lv(lj,
30,lp);le(! *l258){l133 lp;}}lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67"
"\x72\x5f\x6d\x65\x6d\x6f\x72\x79\x5f\x73\x75\x66\x66\x69\x63\x69\x65"
"\x6e\x74\x28\x29",0,0);}l4 ld l296(lg lz*la,lg ld lb,lg l41 l17){ld
l42;l154*l90;lg l16*lf;l0 l179;ld lj;ld l58;l33(0);l42=l17;lf=&(la->
lr);l58=l210(la,lb,&l42);lj=lf->l159(lf->l50,lf->l65,l58,&l90);lv(lj,
10,lp);le(lf->ln==l47&&lb>=3){l42=l42/2;}lj=l336(lf->l50,l315,&l179);
lv(lj,5,lp);le(!l179){lj=l347(lf->l50,l58,l42);lv(lj,30,lp);}lk{lj=
l403(l90,l42);lv(lj,40,lp);}lp:li l270;}l4 l9 l290(lg lz*la,lg ly*lc,
lg ld lb,lg ld lq){le(la->lr.ln==l47||la->lr.ln==l53){l6(lq){lm 4:li(
((lc->lh[0]>>18)|(lc->lh[1]<<14))&0xffff);}}lk le(la->lr.ln==l56){l6(
lq){lm 3:li((lc->lh[0]>>14)&0xff);}}lk le(la->lr.ln==l59){l6(lq){lm 4
:li((lc->lh[0]>>14)&0xffff);}}lw(0);li(l9)(-1);}l4 l9 l253(lg lz*la,
lg ly*lc,lg ld lb,lg ld lq){le(la->lr.ln==l36){l6(lq){lm 4:li(((lc->
lh[1]<<14)|(lc->lh[0]>>18))&0x7fff);}}lw(0);li(l9)(-1);}l4 l9 l259(lg
lz*la,lg ly*lc,lg ld lb,lg ld lq){le(la->lr.ln==l47||la->lr.ln==l53){
l6(lq){lm 4:li(((lc->lh[0]>>17)|(lc->lh[1]<<15))&0xffff);lm 3:li((lc
->lh[0]>>21)&0xff);lm 2:li((lc->lh[0]>>21)&0xf);}}lk le(la->lr.ln==
l56){l6(lq){lm 3:li((lc->lh[0]>>13)&0xff);}}lk le(la->lr.ln==l59){l6(
lq){lm 4:li((lc->lh[0]>>13)&0xffff);}}lk le(la->lr.ln==l71){l6(lq){lm
2:li((lc->lh[0]>>14)&0xf);}}lk le(la->lr.ln==l36){l6(lq){lm 4:li(((lc
->lh[1]<<14)|(lc->lh[0]>>18))&0xffff);}}lw(0);li(l9)(-1);}l4 l9 l288(
lg lz*la,lg ly*lc,lg ld lb,lg l9 l278,lg ld lq){l9 l5;ld lx;ld l63;ld
l245;ld l251;ld l214;ld l37;ld l1;l5=l253(la,lc,lb,lq);l63=1<<lq;le(
la->lt.ll[lb+2]-la->lt.ll[lb+1]<lq){l1=1;}lk{l1=1<<(la->lt.ll[lb+2]-
la->lt.ll[lb+1]-lq);}l5=(l5<<1)|1;l245=l278/l1;l251=l278%l1;l7(lx=0,
l214=0;l214<=l245&&lx<l63;lx++){l6((l5>>lx)&0x1){lm 0x0:l8;lm 0x1:
l214++;l8;}}le(lx==l63)lw(0);lk{le(la->lt.ll[lb+2]-la->lt.ll[lb+1]<lq
){l37=(lx-1)>>(lq-(la->lt.ll[lb+2]-la->lt.ll[lb+1]));}lk{l37=(lx-1)<<
(la->lt.ll[lb+2]-la->lt.ll[lb+1]-lq);l37+=l251;}li l37;}li(l9)(-1);}
l4 l9 l322(lg lz*la,lg ly*lc,lg ld lb,lg l9 l244,lg ld lq){l9 l5;ld lx
;ld l63;ld l236=0;ld l167;ld l242=0;ld l1=0;l5=l259(la,lc,lb,lq);l63=
1<<lq;le(la->lt.ll[lb+2]-la->lt.ll[lb+1]<lq){lw(0);}lk{l1=1<<(la->lt.
ll[lb+2]-la->lt.ll[lb+1]-lq);}l167=0;l242=0;l7(lx=0;l167<=l244&&lx<
l63;lx++){l242=l167;l6((l5>>lx)&0x1){lm 0x0:l167++;l8;lm 0x1:l167+=l1
;l8;}le(la->lt.ll[lb+2]-la->lt.ll[lb+1]<lq){lw(0);}lk{l236=lx*l1;}}
l236+=(l244-l242);li l236;}l4 l9 l283(lg lz*la,lg ly*lc,lg ld lb,lg l9
l332){ld l2;ld l104;ld l227;l227=la->lt.ll[lb+2]-la->lt.ll[lb+1];l2=
l232(la,lc,lb);l104=l2^(l2-1);l2=l2&(~(l2^(l2-1)));l2=l2>>1;l104=l104
>>1;le(l332==0){le(l2==0){li(l104+1)>>(7-l227);}lk{li 0;}}lk{li l2>>(
7-l227);}}l4 l9 l330(lg lz*la,lg ly*lc,lg ld lb,lg l9 l220){le(la->lr
.ln==l36){l6(l15(la,lc,lb)){lm l101:li l288(la,lc,lb,l220,4);lm l46:
li l322(la,lc,lb,l220,4);lm l19:li l283(la,lc,lb,l220);l22:li(l9)-1;}
}lk{li 0;}}l4 l66 l225(lg lz*la,lg ld lb,lg ly*lc,lg ld l21){ld lu;lu
=l100(la,lc,lb,l21);l296(la,lb+1,lu);}l4 l66 l238(lg lz*la,lg ld lb,
lg ly*lc,lg ld l116){ld lx,lu,l273;ly l180;lg l16*lf;ld l254;lf=&(la
->lr);lu=l100(la,lc,lb,l116);le(lu==-1)li;l273=l321(la,lc,lb);le(lb<
/* Old code, almost not in use - ignore coverity defects */
/* coverity[overrun-buffer-val] */
lf->l164-2)l7(lx=0;lx<l273;lx++){l254=l330(la,lc,lb,lx);l108(la,lb+1,
lu+lx,&l180,1);l238(la,lb+1,&l180,l254);}l225(la,lb,lc,l116);}l4 ld
l257(lg lz*la,lg ld lb,lg l140 l102,lg ld l250,lg ld l116,l12 l41*
l188){l292 l153;l154*l90;ld l207;ld l75=1;ld lj;ld l58=0;ld l117;lg
l16*lf;l33(0);lf=&(la->lr);le(lf->ln==l47||lf->ln==l53){l6(l102){lm
l74:lm l81:lm l19:l75=1;l8;lm l82:le(l85(lf->l143[lf->l77[lb]]-1)-17>
0){l75=1<<(l85(lf->l143[lf->l77[lb]]-1)-17);}l8;lm l46:le(l85(lf->
/* Old code, almost not in use - ignore coverity defects */
/* coverity[negative_shift] */
l143[lf->l77[lb]]-1)-16>0){l75=1<<(l85(lf->l143[lf->l77[lb]]-1)-16);}
l8;l22:l75=1;l8;}}lk le(lf->ln==l56){l6(l102){lm l74:l75=2;l8;lm l111
:l75=1;l8;lm l19:l75=2;l8;l22:l75=1;l8;}}lk le(lf->ln==l59){l6(l102){
lm l46:l75=4;l8;l22:l75=2;l8;}}lk le(lf->ln==l71){l75=2;}lk le(lf->ln
==l36){l6(l102){lm l19:l75=1;l8;lm l101:l75=2;l8;lm l46:l75=2;l8;l22:
l75=1;l8;}}lk{}le(lf->ln==l47&&lb>=3)l207=(l250+1)/2;lk l207=l250;le(
lf->ln==l36){l117=(lf->l95-1)/(lf->l164-1);le(l117==1){l58=lf->l77[lb
];}lk le(l117==2){l58=lf->l77[lb]+(l116&0x1);}lk le(l117==4){l58=lf->
l77[lb]+(l116&0x3);}lk{lw(0);}}lk{l58=lf->l77[lb];}lj=lf->l159(lf->
l50,lf->l65,l58,&l90);lv(lj,10,lp);l349(l90,l207,l75,&l153);le(l153==
-1){l360("\x45\x52\x52\x4f\x52\x3a\x20\x4c\x50\x4d\x20\x6d\x61\x6c"
"\x6c\x6f\x63\x20\x72\x61\x6e\x20\x6f\x75\x74\x20\x6f\x66\x20\x6d\x65"
"\x6d\x6f\x72\x79\n"); *l188=(0xFFFFFFFF);}le(lf->ln==l47&&lb>=3)l153
 *=2;lk le(lf->ln==l36)l153=l153+((l58-lf->l77[lb])<<19); *l188=l153;
lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70"
"\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x6d\x61\x6c\x6c\x6f"
"\x63\x28\x29",0,0);}l4 ld l267(lg lz*la,lg ld l10,lg ld l17,lg l0 l23
,lg ld lb,lg ly*l191,l12 ly*l233,lg ld l116,lg l0 l239,l12 l0*l14){l0
l234;ly l83,l80,ls[(1<<(8))];l140 l79=0;ld l52,l92,l112,l176,l105;
l120 l38;l134 l51;l0 l84;l0 l61;ld lj;ld l215;l41 l162=(0xFFFFFFFF);
l94*l11;l0 l169;lg l16*lf;lg l86*lo;l33(l359);l105=l17;l61=l23;lf=&(
la->lr);lo=&(la->lt);l38.l32=l105;l38.l23=(l0)(lo->ll[lb]-lf->l48);lj
=lf->l96(lf->l50,lf->l65,l10,&l11);lv(lj,10,lp);lj=l186(l11,&l38,l88,
&l51,&l84);lv(lj,20,lp);le(l84){l176=(((l51.l26.l35)>>0)&0xFFFFFFF);{
l174(la,l233,lb-1,l176,l51.l342);le(l14){ *l14=l88;}l190;}}le(l61<32){
l105&=~(((ld)-1)>>l61);}le(32+lf->l48-lo->ll[lb+1]<32){l52=l105>>(32+
lf->l48-lo->ll[lb+1]);}lk{l52=0;}l52&=((ld)(-1))>>(32-(lo->ll[lb+1]-
lo->ll[lb]));l92=1;l309(la,l191,lb-1,ls,l116);le(l61+lf->l48<lo->ll[
lb+1]){l92<<=lo->ll[lb+1]-l61-lf->l48;}le(l61==0){l112=0;}lk{l112=
l105&(((ld)(-1))<<(32-l61));}le(l61<lo->ll[lb+1]-lf->l48){l61=(l0)(lo
->ll[lb+1]-lf->l48);}l234=0;l7(;l92;l92--,l52++,l112+=(1<<(32+lf->l48
-lo->ll[lb+1]))){l43(&l83,&ls[l52]);lj=l267(la,l10,l112,l61,lb+1,&l83
,&l80,l52,l239,&l169);lv(lj,30,lp);le(!l169){le(l14){ *l14=l39;}l190;
}le(l106(la,&l80,&l83)!=0){l234=1;le(!l170(la,&l83,lb)){le(l170(la,&
l80,lb)){l238(la,lb,&l83,l52);}lk le(l100(la,&l83,lb,l52)!=l100(la,&
l80,lb,l52)){l225(la,lb,&l83,l52);}}l43(&ls[l52],&l80);}}le(!l234){
l43(l233,l191);}lk{l215=l297(la,&l80,lb-1,ls,&l79);le(!l293(la,&l80,
l191,lb-1)){le(l215!=0){lj=l257(la,lb,l79,l215,l116,&l162);lv(lj,10,
lp);le(l162==(0xFFFFFFFF)){le(l14){ *l14=l39;}l190;}l249(la,&l80,lb-1
,ls,l162,l79);}}lk{l162=l100(la,l191,lb-1,l116);l249(la,&l80,lb-1,ls,
l162,l79);}l43(l233,&l80);}le(l14){ *l14=l88;}lp:l45("\x65\x72\x72"
"\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70"
"\x6d\x5f\x6d\x6e\x67\x72\x5f\x61\x64\x64\x5f\x70\x72\x65\x66\x69\x78"
"\x5f\x68\x65\x6c\x70\x65\x72\x28\x29",0,0);}l4 ld l201(lg lz*la,lg ld
l10,lg ld l17,lg l0 l23,lg l0 l239,l12 l0*l14){ld lb;ly l80,l83;ld l52
,l112,l92,l105;l0 l179;l0 l61;ld lj;l0 l169;lg l16*lf;lg l86*lo;l33(0
);lf=&(la->lr);lo=&(la->lt);l61=l23;l105=l17;lb=0;le(l61<32){l105&=~(
((ld)-1)>>l61);}le(32+lf->l48-lo->ll[lb+1]<32){l52=l105>>(32+lf->l48-
lo->ll[lb+1]);l52&=((ld)(-1))>>(32+lf->l48-(lo->ll[lb+1]-lo->ll[lb]));
}lk{l52=0;}l52|=l10<<(lo->ll[1]-lf->l48);l92=1;le(l61+lf->l48<lo->ll[
lb+1]){l92<<=lo->ll[lb+1]-l61-lf->l48;}le(l61==0){l112=0;}lk{l112=
l105&(((ld)(-1))<<(32-l61));}le(l61<lo->ll[lb+1]-lf->l48){l61=(l0)(lo
->ll[lb+1]-lf->l48);}l7(;l92;l92--,l52++,l112+=(1<<(32+lf->l48-lo->ll
[lb+1]))){lj=l108(la,lb,l52,&l83,1);lv(lj,10,lp);lj=l267(la,l10,l112,
l61,lb+1,&l83,&l80,l52,l239,&l169);lv(lj,20,lp);le(!l169){le(l14){ *
l14=l39;}l190;}le(l106(la,&l80,&l83)!=0){le(!l170(la,&l83,lb)){le(
l170(la,&l80,lb)){l238(la,lb,&l83,l52);}lk le(l100(la,&l83,lb,l52)!=
l100(la,&l80,lb,l52)){l225(la,lb,&l83,l52);}}lj=l128(la,lb,l52,&l80,1
);lv(lj,20,lp);}}l336(lf->l50,l315,&l179);le(!l179){l362(lf->l50);}le
(l14){ *l14=l88;}lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63"
"\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x61"
"\x64\x64\x5f\x70\x72\x65\x66\x69\x78\x5f\x68\x65\x6c\x70\x65\x72\x30"
"\x28\x29",0,0);}l4 l9 l302(lg lz*la,lg ly*lc,lg ld lb){le(la->lr.ln
==l47||la->lr.ln==l53){l6(l15(la,lc,lb)){lm l13:li(lc->lh[0]&0xfffff);
lm l126:l22:li(l9)(-1);}}lk le(la->lr.ln==l56){l6(l15(la,lc,lb)){lm
l13:li(lc->lh[0]&0x3fff);l22:li(l9)(-1);}}lk le(la->lr.ln==l59){l6(
l15(la,lc,lb)){lm l13:li(lc->lh[0]&0x7fff);l22:li(l9)(-1);}}lk le(la
->lr.ln==l71){l6(l15(la,lc,lb)){lm l13:li(lc->lh[0]&0x3fff);l22:li(l9
)(-1);}}lk le(la->lr.ln==l36){l6(l15(la,lc,lb)){lm l13:li(lc->lh[0]&
0xffffffff);l22:li(l9)(-1);}}lk{lw(0);li(l9)(-1);}}l4 l9 l222(lg lz*
la,lg ly*lc,lg ld lb,lg l9 l37,lg ld lq){l9 l5;l9 l73;ld lx;ld l63;ld
l1;l9 l49=l37;l5=l290(la,lc,lb,lq);l63=1<<lq;le(la->lt.ll[lb+2]-la->
lt.ll[lb+1]<lq){l1=1;l49<<=lq-(la->lt.ll[lb+2]-la->lt.ll[lb+1]);}lk{
l1=1<<(la->lt.ll[lb+2]-la->lt.ll[lb+1]-lq);}l5=(l5<<1)|1;l73=0;l7(lx=
0;lx<l63;lx++){le(l49<l1)l6((l5>>lx)&0x3){lm 0x0:lm 0x2:li(l73-1);lm
0x1:li(l73);lm 0x3:li(l73+l49);}l49-=l1;l6((l5>>lx)&0x3){lm 0x0:lm 0x2
:l8;lm 0x1:l73+=1;l8;lm 0x3:l73+=l1;l8;}}lw(0);li(l9)(-1);}l4 l9 l291
(lg lz*la,lg ly*lc,lg ld lb,lg l9 l37,lg ld lq){l9 l5;l9 l73;ld lx;ld
l63;ld l1;l9 l49=l37;l5=l253(la,lc,lb,lq);l63=1<<lq;le(la->lt.ll[lb+2
]-la->lt.ll[lb+1]<lq){l1=1;l49<<=lq-(la->lt.ll[lb+2]-la->lt.ll[lb+1]);
}lk{l1=1<<(la->lt.ll[lb+2]-la->lt.ll[lb+1]-lq);}l5=(l5<<1)|1;l73=0;l7
(lx=0;lx<l63;lx++){le(l49<l1)l6((l5>>lx)&0x1){lm 0x0:li(l73-1);lm 0x1
:li(l73+l49);}l49-=l1;l6((l5>>lx)&0x1){lm 0x0:l8;lm 0x1:l73+=l1;l8;}}
lw(0);li(l9)(-1);}l4 l9 l136(lg lz*la,lg ly*lc,lg ld lb,lg l9 l37,lg
ld lq){l9 l40;ld l1;l9 l73;ld lx;ld l149;ld l49=l37;l40=l259(la,lc,lb
,lq);l149=1<<lq;le(la->lt.ll[lb+2]-la->lt.ll[lb+1]<lq){l1=1;}lk{l1=1
<<(la->lt.ll[lb+2]-la->lt.ll[lb+1]-lq);}l73=0;l7(lx=0;lx<l149;){le(
l49<l1)l6((l40>>lx)&0x1){lm 0x0:li(l73);lm 0x1:li(l73+l49);}l49-=l1;
l6((l40>>lx)&0x1){lm 0x0:l73+=1;l8;lm 0x1:l73+=l1;l8;}le(lq>la->lt.ll
[lb+2]-la->lt.ll[lb+1])lx+=1<<(lq-(la->lt.ll[lb+2]-la->lt.ll[lb+1]));
lk lx++;}lw(0);li(l9)(-1);}l4 l9 l168(lg lz*la,lg ly*lc,lg ld lb,lg l9
l37){l9 l2;l9 l49=l37;l2=l232(la,lc,lb);le(la->lr.ln==l47||la->lr.ln
==l53){le(la->lt.ll[lb+2]-la->lt.ll[lb+1]<=9)l49<<=9-(la->lt.ll[lb+2]
-la->lt.ll[lb+1]);lk l49>>=(la->lt.ll[lb+2]-la->lt.ll[lb+1])-9;}lk le
(la->lr.ln==l56){le(la->lt.ll[lb+2]-la->lt.ll[lb+1]<=7)l49<<=7-(la->
lt.ll[lb+2]-la->lt.ll[lb+1]);lk l49>>=(la->lt.ll[lb+2]-la->lt.ll[lb+1
])-7;}lk le(la->lr.ln==l59){le(la->lt.ll[lb+2]-la->lt.ll[lb+1]<=8)l49
<<=8-(la->lt.ll[lb+2]-la->lt.ll[lb+1]);lk l49>>=(la->lt.ll[lb+2]-la->
lt.ll[lb+1])-8;}lk le(la->lr.ln==l71){le(la->lt.ll[lb+2]-la->lt.ll[lb
+1]<=6)l49<<=6-(la->lt.ll[lb+2]-la->lt.ll[lb+1]);lk l49>>=(la->lt.ll[
lb+2]-la->lt.ll[lb+1])-6;}lk le(la->lr.ln==l36){l49<<=8-(la->lt.ll[lb
+2]-la->lt.ll[lb+1]);}lk{lw(0);li(l9)(-1);}l49&=~(l2^(l2-1));l2&=~(l2
^(l2-1));le(l49==l2){li(1);}lk{li(0);}}l4 l9 l300(lg lz*la,lg ly*lc,
lg ld lb,lg l9 l37){le(la->lr.ln==l47||la->lr.ln==l53){l6(l15(la,lc,
lb)){lm l82:li(l222(la,lc,lb,l37,4));lm l46:li(l136(la,lc,lb,l37,4));
lm l19:li(l168(la,lc,lb,l37));lm l74:li(l136(la,lc,lb,l37,3));lm l81:
li(l136(la,lc,lb,l37,2));l22:l8;}}lk le(la->lr.ln==l56){l6(l15(la,lc,
lb)){lm l111:li(l222(la,lc,lb,l37,3));lm l74:li(l136(la,lc,lb,l37,3));
lm l19:li(l168(la,lc,lb,l37));l22:l8;}}lk le(la->lr.ln==l59){l6(l15(
la,lc,lb)){lm l82:li(l222(la,lc,lb,l37,4));lm l46:li(l136(la,lc,lb,
l37,4));lm l19:li(l168(la,lc,lb,l37));l22:l8;}}lk le(la->lr.ln==l71){
l6(l15(la,lc,lb)){lm l81:li(l136(la,lc,lb,l37,2));lm l19:li(l168(la,
lc,lb,l37));l22:l8;}}lk le(la->lr.ln==l36){l6(l15(la,lc,lb)){lm l101:
li(l291(la,lc,lb,l37,4));lm l46:li(l136(la,lc,lb,l37,4));lm l19:li(
l168(la,lc,lb,l37));l22:l8;}}lw(0);li(l9)(-1);}l4 ld l268(lg lz*la,lg
ld l223,lg ld l17,lg ld lb,lg l9 l144,lg ld l21){ly lc;l9 l113=l144;
/* Old code, almost not in use - ignore coverity defects */
/* coverity[overrun-buffer-val] */
ld l277;l108(la,lb,l113,&lc,1);le(l170(la,&lc,lb)){li(l302(la,&lc,lb));
}le(32+la->lr.l48-la->lt.ll[lb+2]<32){l113=l17>>(32+la->lr.l48-la->lt
.ll[lb+2]);l113&=(1<<(la->lt.ll[lb+2]-la->lt.ll[lb+1]))-1;}lk{l113=0;
}l277=l113;l113=l300(la,&lc,lb,l113);l113+=l100(la,&lc,lb,l21);li(
l268(la,l223,l17,lb+1,l113,l277));}ld l396(lg lz*la,lg ld l223,lg ld
l10,lg ld l17){l9 l144;l60 lb;lb=-1;le(32+la->lr.l48-la->lt.ll[lb+2]<
32){l144=l17>>(32+la->lr.l48-la->lt.ll[lb+2]);}lk{l144=0;}l144|=l10<<
(la->lt.ll[lb+2]-la->lr.l48);li(l268(la,l223,l17,lb+1,l144,l144));}ld
l339(l20 lz*la){l16*lf;l86*lo;l41 l188;l60 l28;ld l76,l157;ld lj;l33(
0);lf=&(la->lr);lo=&(la->lt);l7(l76=0;l76<lf->l256;++l76){lf->l161[
l76].l346=l401;lf->l161[l76].l389=l366;lj=l340(&(lf->l161[l76]));lv(
lj,10,lp);}l7(l28=0;(ld)l28<lf->l95;++l28){le(la->lr.ln!=l36||l28!=
100){lj=l377(&(lf->l224[l28]));lv(lj,20,lp);}}lo->ll=(ld* )l392(l146(
ld) * (lf->l164+1),"\x6d\x61\x74\x63\x68\x31\x31");le(lf->l155&l152){
l7(l28=1;(ld)l28<lf->l95;++l28){lj=l364((lf->l130[l28]));lv(lj,20,lp);
/* Old code, almost not in use - ignore coverity defects */
/* coverity[dereference] */
}}lo->ll[0]=0;l7(l157=0;l157<lf->l164;l157++){lo->ll[l157+1]=lo->ll[
l157]+lf->l310[l157];}le(la->lr.ln!=l36){l7(l28=0;l28<(1<<(lo->ll[1]-
lo->ll[0]));){ld l184;le((1<<(lo->ll[1]-lo->ll[0]))-l28<256)l184=(1<<
(lo->ll[1]-lo->ll[0]))-l28;lk l184=256;lj=l257(la,0,l275,l184,0,&l188
);lv(lj,30,lp);l28+=l184;}}lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67"
"\x72\x5f\x63\x72\x65\x61\x74\x65\x28\x29",0,0);}ld l381(l20 lz*la,lg
ld l10,lg l199 l139){l94*l11;ly l171;l120 l25;l329 l26;ld l44;ld l118
;l0 l14;lg l16*lf;lg l86*lo;ld lj;l33(0);lf=&(la->lr);lo=&(la->lt);lj
=lf->l96(lf->l50,lf->l65,l10,&l11);lv(lj,10,lp);lj=l213(l11);lv(lj,15
,lp);l25.l23=0;l25.l32=0;l26.l35=((((l219)&0x3)<<30)|(((l195)&0x3)<<(
30-2))|(((l139)&0xFFFFFFF)<<0));lj=l211(l11,&l25,&l26,&l14);lv(lj,20,
lp);l174(la,&l171,0,l139,0);l118=l10;l118<<=lo->ll[1]-lf->l48;l7(l44=
l118;l44<l118+(ld)(1<<(lo->ll[1]-lf->l48));++l44){lj=l128(la,0,l44,&
l171,1);lv(lj,100,lp);}lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72"
"\x5f\x76\x72\x66\x5f\x69\x6e\x69\x74\x28\x29",0,0);}ld l361(l20 lz*
la,lg ld l10,lg l199 l139,lg l205 l93){l94*l11;l0 l14;lg l16*lf;l115
l194;ld lj;l33(0);lf=&(la->lr);lj=lf->l96(lf->l50,lf->l65,l10,&l11);
lv(lj,10,lp);l350(&l194);l194.l107=0;l194.l110=0;lj=l213(l11);lv(lj,
15,lp);lj=lf->l344(lf->l50,0);lv(lj,15,lp);l285(la,l10,&l194,l139,l93
,l195,&l14);lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70"
"\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x76\x72"
"\x66\x5f\x63\x6c\x65\x61\x72\x28\x29",0,0);}ld l379(l20 lz*la,lg ld
l122,lg l199 l139){l94*l11;ly l171;l120 l25;l329 l26;ld l44;ld l118,
l10;l0 l14;lg l16*lf;lg l86*lo;ld lj;l33(0);lf=&(la->lr);lo=&(la->lt);
l7(l44=0;l44<la->lr.l95;++l44){lj=l307(&(lf->l224[l44]));lv(lj,40,lp);
le(l44>=1&&la->lr.l155&l152){lj=l335((lf->l130[l44]));lv(lj,30,lp);}}
l7(l10=0;l10<l122;++l10){lj=lf->l96(lf->l50,lf->l65,l10,&l11);lv(lj,
10,lp);lj=l213(l11);lv(lj,15,lp);l25.l23=0;l25.l32=0;l26.l35=((((l219
)&0x3)<<30)|(((l195)&0x3)<<(30-2))|(((l139)&0xFFFFFFF)<<0));lj=l211(
l11,&l25,&l26,&l14);lv(lj,20,lp);l174(la,&l171,0,l139,0);l118=l10;
l118<<=lo->ll[1]-lf->l48;l7(l44=l118;l44<l118+(ld)(1<<(lo->ll[1]-lf->
l48));++l44){lj=l128(la,0,l44,&l171,1);lv(lj,100,lp);}}lp:l45("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f"
"\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x61\x6c\x6c\x5f\x76\x72\x66\x73"
"\x5f\x63\x6c\x65\x61\x72\x28\x29",0,0);}ld l369(l20 lz*la){l16*lf;
l86*lo;ld l28,l76;ld lj;l33(0);lf=&(la->lr);lo=&(la->lt);l7(l76=0;l76
<lf->l256;++l76){lj=l365(&(lf->l161[l76]));lv(lj,10,lp);}l7(l28=0;l28
<lf->l95;++l28){lj=l370(&(lf->l224[l28]));lv(lj,20,lp);}le(lf->l155&
l152){l7(l28=1;l28<lf->l95;++l28){lj=l390(lf->l130[l28]);lv(lj,260,lp
);}}le(lf->l155&l152){l7(l28=1;l28<lf->l95;++l28){l265(lf->l130[l28]);
}l265(lf->l130);}l265(lo->ll);lp:l45("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e"
"\x67\x72\x5f\x64\x65\x73\x74\x72\x6f\x79\x28\x29",0,0);}l4 ld l311(
l20 l94*l11,lg l394 l25,lg l0 l23,l20 ld*l26,lg l0 l308,l20 l66*l298){
l209*l72=(l209* )l298;l205 l93;l226 l97;ld l176;l33(0);le(l308){l176=
((( *l26)>>0)&0xFFFFFFF);l93=((( *l26)>>30)&0x3);l97=((( *l26)>>(30-2
/* Old code, almost not in use - ignore coverity defects */
/* coverity[dead_error_begin] */
))&0x3);le(l97==l195){le(l93==l391){l72->l109[l72->l173].l23=l23;l72
->l109[l72->l173].l17=l25;l72->l173++;}lk le(l93==l289){ *l26=((((
l219)&0x3)<<30)|(((l97)&0x3)<<(30-2))|(((l176)&0xFFFFFFF)<<0));}}}l45
("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x61\x74\x5f\x66\x6f"
"\x72\x65\x61\x63\x68\x5f\x66\x6e\x5f\x68\x77\x5f\x73\x79\x6e\x63\x28"
"\x29",0,0);}ld l376(l20 lz*la,lg ld l10){ld l17;l0 l23;ld lj;ld lx;
l209 l72;l120 l38;l94*l11;l16*lf;l33(0);l72.l109=l178;lf=&(la->lr);lj
=lf->l96(lf->l50,lf->l65,l10,&l11);lv(lj,20,lp);l388(l72.l109,l146( *
l72.l109) *lf->l301,"\x6d\x61\x74\x63\x68\x31\x32");l217(l72.l109,0,
l146( *l72.l109) *lf->l301);l72.l173=0;lj=l400(l11,l311,&l72);lv(lj,
25,lp);l7(lx=0;lx<l72.l173;lx++){l38.l23=l72.l109[lx].l23;l38.l32=l72
.l109[lx].l17;l333(l11,&l38);}l17=0;l23=0;lj=l201(la,l10,l17,l23,l88,
l178);lv(lj,30,lp);lp:l363(l72.l109);l45("\x65\x72\x72\x6f\x72\x20"
"\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d"
"\x6e\x67\x72\x5f\x68\x77\x5f\x73\x79\x6e\x63\x28\x29",0,0);}ld l375(
l20 lz*la,lg ld l10,lg l115*l25,l205 l93){l94*l11;l16*lf;l120 l38;
l134 l51;ld l17,l26;l0 l23;ld lj;l0 l84,l14;l33(0);lf=&(la->lr);l17=
l25->l107;l23=l25->l110;l38.l23=l23;l38.l32=l17;lj=lf->l96(lf->l50,lf
->l65,l10,&l11);lv(lj,10,lp);lj=l186(l11,&l38,l39,&l51,&l84);lv(lj,20
,lp);le(!l84){l133 lp;}l26=((((l93)&0x3)<<30)|((((((l51.l26.l35)>>(30
-2))&0x3))&0x3)<<(30-2))|((((((l51.l26.l35)>>0)&0xFFFFFFF))&0xFFFFFFF
)<<0));l51.l26.l35=l26;lj=l211(l11,&l38,&l51.l26,&l14);lv(lj,10,lp);
lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70"
"\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x6d\x61\x72\x6b\x5f"
"\x6e\x6f\x6e\x5f\x6c\x70\x6d\x5f\x72\x6f\x75\x74\x65\x5f\x70\x65\x6e"
"\x64\x69\x6e\x67\x5f\x74\x79\x70\x65\x5f\x73\x65\x74\x28\x29",0,0);}
ld l285(l20 lz*la,lg ld l10,lg l115*l25,lg l199 l231,lg l0 l151,lg
l226 l97,l12 l0*l14){l94*l11;l16*lf;l120 l38;l134 l51;ld l17,l26;l0
l23;ld lj;l0 l119=l39;l261 l182=l196;l33(0);lf=&(la->lr);l17=l25->
l107;l23=l25->l110;le(!l151){lj=l262(la,0,l14);lv(lj,10,lp);le(! *l14
){l119=l88;}}le(l119){lj=l200(la,l10,1,l88);lv(lj,10,lp);}l38.l23=l23
;l38.l32=l17;lj=lf->l96(lf->l50,lf->l65,l10,&l11);lv(lj,10,lp);lv(lj,
20,lp);l26=(((((l151==l88)?l289:l219)&0x3)<<30)|(((l97)&0x3)<<(30-2))|
(((l231)&0xFFFFFFF)<<0));l51.l26.l35=l26;lj=l211(l11,&l38,&l51.l26,
l14);lv(lj,10,lp);le(! *l14){l133 lp;}le((!l151)&&((l97==l195))){lj=
l201(la,l10,l17,l23,l39,l14);lv(lj,20,lp);}le(l119&& *l14){lj=l274(la
,l10,l178,1,l39,&l182);lv(lj,10,lp);le(l182!=l196){ *l14=l39;}lk{ *
l14=l88;}}le(l119){lj=l200(la,l10,1,l39);lv(lj,10,lp);}lp:l45("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f"
"\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66\x69\x78\x5f\x61"
"\x64\x64\x28\x29",0,0);}ld l274(l20 lz*la,lg ld l10,lg ld*l247,lg ld
l122,lg l0 l320,l12 l261*l14){ld l89,l158;l0 l172=l39;ld l17,l145;l0
l23;ld l68;l154*l125;ld lj;lg l16*lf;l33(0);lf=&(la->lr);l17=0;l23=0;
le(l320){l7(l89=0;l89<l122;++l89){le(l122==1){l158=l10;}lk{l145=0;lj=
l280(l247,l89,1,&l145);le(!l145){l328;}l158=l89;}lj=l201(la,l158,l17,
l23,l88,&l172);lv(lj,30,lp);le(!l172){l8;}}le(!l172){ *l14=l357;l133
lp;}}lk{l172=l88;}l7(l68=0;l68<lf->l95;++l68){lj=lf->l159(lf->l50,lf
->l65,lf->l95-l68-1,&l125);lv(lj,30,lp);le(l172){lj=l368(l125,0);lv(
lj,30,lp);le(l68>=1&&la->lr.l155&l152){lj=l393((lf->l130[l68]),0);lv(
lj,40,lp);} *l14=l196;}}l7(l89=0;l89<l122;++l89){le(l122==1){l158=l10
;}lk{l145=0;lj=l280(l247,l89,1,&l145);le(!l145){l328;}l158=l89;}lj=
l372(&(la->lr.l161[l158]),0);lv(lj,30,lp);}lp:l45("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66\x69\x78\x5f\x61\x64\x64\x28"
"\x29",0,0);}ld l200(l20 lz*la,lg ld l10,lg ld l122,lg l0 l193){ld l68
,l89;l154*l125;l94*l11;ld lj;lg l16*lf;l33(0);lf=&(la->lr);l7(l68=0;
l68<lf->l95;++l68){lj=lf->l159(lf->l50,lf->l65,l68,&l125);lv(lj,10,lp
);lj=l371(l125,l193);lv(lj,10,lp);le(la->lr.l155&l152&&l68>=1){lj=
l378((lf->l130[l68]),l193);lv(lj,20,lp);}}le(l10==l384){l7(l89=0;l89<
l122;++l89){lj=lf->l96(lf->l50,lf->l65,l89,&l11);lv(lj,20,lp);lj=l327
(l11,l193);lv(lj,30,lp);}}lk{lj=lf->l96(lf->l50,lf->l65,l10,&l11);lv(
lj,40,lp);lj=l327(l11,l193);lv(lj,50,lp);}lp:l45("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x63\x61\x63\x68\x65\x5f\x73\x65\x74\x28\x29"
,0,0);}ld l352(l20 lz*la,lg ld l10,lg l115*l25,lg l0 l151,l12 l0*l14){
l94*l11;l16*lf;l120 l38;l134 l51;ld l17;l0 l23;ld lj;l0 l84,l119=l39;
l261 l182=l196;l33(0);l17=l25->l107;l23=l25->l110;le(!l151){lj=l262(
la,1,l14);lv(lj,10,lp);le(! *l14){le(! *l14){l119=l88;}}}le(l119){lj=
l200(la,l10,1,l88);lv(lj,10,lp);}lf=&(la->lr);lj=lf->l96(lf->l50,lf->
l65,l10,&l11);lv(lj,10,lp);l38.l23=l23;l38.l32=l17;lj=l186(l11,&l38,
l39,&l51,&l84);lv(lj,20,lp);le(!l84){l133 lp;}{lj=l333(l11,&l38);lv(
lj,10,lp);le(!l151){lj=l201(la,l10,l17,l23,l39,l14);lv(lj,20,lp);}le(
l119&& *l14){lj=l274(la,l10,l178,1,l39,&l182);lv(lj,10,lp);le(l182!=
l196){ *l14=l39;}lk{ *l14=l88;}}le(l119){lj=l200(la,l10,1,l39);lv(lj,
10,lp);}}lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f"
"\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65"
"\x66\x69\x78\x5f\x72\x65\x6d\x6f\x76\x65\x28\x29",0,0);}ld l323(l20
lz*la,lg ld l10,lg l115*l25,l12 l115*l156,l12 l0*l121,l12 l0*l84){l94
 *l11;l16*lf;l120 l38;l134 l51;ld lj;l33(0);lf=&(la->lr);lj=lf->l96(
lf->l50,lf->l65,l10,&l11);lv(lj,10,lp);l38.l32=l25->l107;l38.l23=l25
->l110;lj=l186(l11,&l38,l39,&l51,l84);lv(lj,20,lp);le(!( *l84)){ *
l121=l39;l133 lp;}l156->l107=l51.l25.l32;l156->l110=l51.l25.l23;le((
l156->l107==l25->l107)&&(l156->l110==l25->l110)){ *l121=l88;}lk{ *
l121=l39;}lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70"
"\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72"
"\x65\x66\x69\x78\x5f\x6c\x6f\x6f\x6b\x75\x70\x28\x29",0,0);}ld l380(
l20 lz*la,lg ld l10,lg l115*l25,l12 l0*l319){l115 l156;l0 l312;l0 l121
;ld lj;l33(0);lj=l323(la,l10,l25,&l156,&l121,&l312);lv(lj,10,lp); *
l319=l121;lp:l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70"
"\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72"
"\x65\x66\x69\x78\x5f\x69\x73\x5f\x65\x78\x69\x73\x74\x28\x29",0,0);}
ld l398(l20 lz*la,lg ld l10,lg l115*l25,lg l0 l121,l12 l199*l231,l12
l205*l93,l12 l226*l97,l12 l0*l84){l94*l11;l16*lf;l120 l38;l134 l51;l0
l266;ld lj;l33(0);lf=&(la->lr);lj=lf->l96(lf->l50,lf->l65,l10,&l11);
lv(lj,10,lp);l38.l32=l25->l107;l38.l23=l25->l110;lj=l186(l11,&l38,l39
,&l51,&l266);lv(lj,20,lp);le(!l266){ *l84=l39;l133 lp;}le(l121&&((l51
.l25.l32!=l25->l107)||(l51.l25.l23!=l25->l110))){ *l84=l39;l133 lp;} *
l84=l88; *l231=(((l51.l26.l35)>>0)&0xFFFFFFF);le(l93){ *l93=(((l51.
l26.l35)>>30)&0x3);}le(l97){ *l97=(((l51.l26.l35)>>(30-2))&0x3);}lp:
l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x73\x79\x73\x5f\x66\x65"
"\x63\x5f\x67\x65\x74\x28\x29",0,0);}ld l367(l20 lz*la,lg ld l10,l12
l269*l197){l94*l11;l16*lf;ld lj;l33(0);l252(l197);lf=&(la->lr);lj=lf
->l96(lf->l50,lf->l65,l10,&l11);lv(lj,10,lp);lj=l385(l11,&(l197->l287
),&(l197->l326),&(l197->l314));lv(lj,20,lp);lp:l45("\x65\x72\x72\x6f"
"\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x67\x65\x74\x5f\x73\x74\x61\x74\x28\x29",0,
0);}ld l402(l20 lz*la){l16*lf;ld l28,l76;ld lj;l33(0);lf=&(la->lr);l7
(l76=0;l76<lf->l256;++l76){lj=l213(&(lf->l161[l76]));lv(lj,10,lp);}l7
(l28=0;l28<lf->l95;++l28){lj=l307(&(lf->l224[l28]));lv(lj,20,lp);le(
l28>=1&&la->lr.l155&l152){lj=l335((lf->l130[l28]));lv(lj,30,lp);}}lp:
l45("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x70\x63\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x63\x6c\x65\x61\x72\x28"
"\x29",0,0);}ld l383(l20 lz*la,lg ld l10,lg l387 l325,l20 l147*l263,
lg ld l282,lg ld l284,l12 l115*l276,l12 ld*l230,l12 l205*l93,l12 l226
 *l97,l12 ld*l208){l134*l127=l178;ld l192,l175,l237,l135,l44;l16*lf;
l94*l11;ld lj;l33(0);lf=&(la->lr);lj=lf->l96(lf->l50,lf->l65,l10,&l11
);lv(lj,10,lp);l175=l284;l44=0;l237=0;l345(l127,l134,100,"\x70\x63"
"\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x67"
"\x65\x74\x5f\x62\x6c\x6f\x63\x6b\x2e\x6e\x6f\x64\x65\x73");l373(!
l351((l263))&&l175){le(l175>100){l192=100;}lk{l192=l175;}l175-=l192;
lj=l382(l11,l325,l263,l282,l192,l127,l208);lv(lj,20,lp);l237+= *l208;
l7(l135=0;l44<l237;++l44,++l135){l276[l44].l107=l127[l135].l25.l32;
l276[l44].l110=l127[l135].l25.l23;l230[l44]=(((l127[l135].l26.l35)>>0
)&0xFFFFFFF);l230[l44]=0x7FFF&l230[l44];le(l93){l93[l44]=(((l127[l135
].l26.l35)>>30)&0x3);}le(l97){l97[l44]=(((l127[l135].l26.l35)>>(30-2))&
0x3);}}} *l208=l44;lp:l386(l127);l45("\x65\x72\x72\x6f\x72\x20\x69"
"\x6e\x20\x70\x63\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e"
"\x67\x72\x5f\x67\x65\x74\x5f\x62\x6c\x6f\x63\x6b\x28\x29",0,0);}l66
l399(l20 lz*l114){l33(0);l252(l114);l217(l114,0x0,l146(lz));l114->lt.
ll=l178;l337;lp:l331(0,0,0);}l66 l348(l20 l269*l114){l33(0);l252(l114
);l217(l114,0x0,l146(l269));l114->l314=0;l114->l326=0;l114->l287=0;
l337;lp:l331(0,0,0);}
#undef l204
#undef l60
#undef l356
#undef ld
#undef l9
#undef l353
#undef l292
#undef l4
#undef l295
#undef l0
#undef l23
#undef l66
#undef l12
#undef ly
#undef lg
#undef l341
#undef lh
#undef l146
#undef lz
#undef li
#undef l147
#undef l32
#undef le
#undef lr
#undef ln
#undef lk
#undef l20
#undef l16
#undef l47
#undef l53
#undef l143
#undef l77
#undef lw
#undef l56
#undef l71
#undef l59
#undef l36
#undef l35
#undef l397
#undef l246
#undef l26
#undef l7
#undef l95
#undef l164
#undef l154
#undef lj
#undef l33
#undef l159
#undef l50
#undef l65
#undef lv
#undef lp
#undef l260
#undef l270
#undef l217
#undef l286
#undef l45
#undef l6
#undef lm
#undef l22
#undef l39
#undef l100
#undef l358
#undef l8
#undef lt
#undef ll
#undef l86
#undef l190
#undef l404
#undef l374
#undef l304
#undef l343
#undef l255
#undef l310
#undef l355
#undef l133
#undef l336
#undef l315
#undef l347
#undef l403
#undef l349
#undef l360
#undef l120
#undef l134
#undef l94
#undef l11
#undef l359
#undef l48
#undef l96
#undef l186
#undef l88
#undef l342
#undef l362
#undef l396
#undef l339
#undef l256
#undef l161
#undef l346
#undef l401
#undef l389
#undef l366
#undef l340
#undef l377
#undef l224
#undef l392
#undef l155
#undef l152
#undef l364
#undef l130
#undef l381
#undef l199
#undef l25
#undef l329
#undef l213
#undef l219
#undef l195
#undef l211
#undef l361
#undef l205
#undef l115
#undef l350
#undef l107
#undef l110
#undef l344
#undef l285
#undef l379
#undef l307
#undef l335
#undef l369
#undef l365
#undef l370
#undef l390
#undef l265
#undef l311
#undef l394
#undef l308
#undef l226
#undef l391
#undef l289
#undef l376
#undef l178
#undef l388
#undef l301
#undef l400
#undef l333
#undef l363
#undef l375
#undef l231
#undef l261
#undef l196
#undef l200
#undef l274
#undef l280
#undef l328
#undef l357
#undef l368
#undef l393
#undef l372
#undef l371
#undef l378
#undef l384
#undef l327
#undef l352
#undef l323
#undef l380
#undef l398
#undef l367
#undef l269
#undef l252
#undef l385
#undef l287
#undef l326
#undef l314
#undef l402
#undef l383
#undef l387
#undef l345
#undef l373
#undef l351
#undef l382
#undef l386
#undef l399
#undef l114
#undef l337
#undef l331
#undef l348
#include<soc/dpp/SAND/Utils/sand_footer.h>
