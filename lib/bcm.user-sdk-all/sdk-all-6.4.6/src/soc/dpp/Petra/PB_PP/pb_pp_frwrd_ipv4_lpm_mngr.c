/*
 * $Id: pb_pp_frwrd_ipv4_lpm_mngr.c,v 1.9 Broadcom SDK $
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
 * DPP IPv4 LPM Manager
 */

#include<soc/dpp/SAND/Utils/sand_header.h>
#include<soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv4_lpm_mngr.h>
#include<soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include<soc/dpp/SAND/Utils/sand_array_memory_allocator.h>
#include<soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include<soc/dpp/SAND/Utils/sand_os_interface.h>
#include<soc/dpp/SAND/Utils/sand_bitstream.h>
#include<soc/dpp/SAND/Management/sand_error_code.h>
#include<soc/dpp/SAND/Management/sand_low_level.h>
#define l164 typedef
#define l45 int32
#define l312 SOC_SAND_LONG
#define ld uint32
#define l7 SOC_SAND_ULONG
#define l316 enum
#define l278 SOC_SAND_ARR_MEM_ALLOCATOR_PTR
#define lz static
#define l264 struct
#define lt uint8
#define l11 prefix
#define l56 void
#define l8 SOC_SAND_OUT
#define lv SOC_PB_PP_IPV4_LPM_MNGR_INST
#define lf SOC_SAND_IN
#define l318 soc_sand_os_memcpy
#define li arr
#define l135 sizeof
#define lw SOC_PB_PP_IPV4_LPM_MNGR_INFO
#define l10 SOC_PB_PP_IPV4_LPM_MNGR_INIT_INFO
#define ls init_info
#define lg if
#define lp pxx_model
#define lj return
#define ll else
#define l130 SOC_SAND_U64
#define l19 val
#define l14 SOC_SAND_INOUT
#define l37 SOC_PB_PP_IPV4_LPM_PXX_MODEL_P6N
#define l29 SOC_PB_PP_IPV4_LPM_PXX_MODEL_P4X
#define l34 SOC_PB_PP_IPV4_LPM_PXX_MODEL_P4X_COMPLETE
#define l117 nof_rows_per_mem
#define l88 bank_to_mem
#define l50 SOC_PB_PP_IPV4_LPM_PXX_MODEL_P6X
#define l294 soc_pb_pp_ipv4_lpm_entry_encode_node_id
#define l272 SOC_SAND_GET_BITS_RANGE
#define l287 SOC_SAND_SET_BITS_RANGE
#define l60 payload
#define l310 soc_pb_pp_ipv4_lpm_entry_decode
#define l244 SOC_SAND_ARR_MEM_ALLOCATOR_ENTRY
#define l15 data
#define l0 for
#define l144 SOC_SAND_ARR_MEM_ALLOCATOR_INFO
#define lh res
#define l20 SOC_SAND_INIT_ERROR_DEFINITIONS
#define l128 mem_alloc_get_fun
#define l31 prime_handle
#define l46 sec_handle
#define lq SOC_SAND_CHECK_FUNC_RESULT
#define lk exit
#define l219 soc_sand_arr_mem_allocator_read
#define l237 SOC_SAND_OK
#define l193 soc_sand_os_memset
#define l290 soc_sand_arr_mem_allocator_write
#define l28 SOC_SAND_EXIT_AND_SEND_ERROR
#define l16 switch
#define lm case
#define l33 default
#define l26 FALSE
#define l90 soc_pb_pp_ipv4_lpm_mngr_row_to_base_addr
#define l358 soc_pb_pp_ipv4_lpm_mngr_update_base_addr
#define l18 break
#define l1 data_info
#define ln bit_depth_per_bank
#define l80 SOC_PB_PP_IPV4_LPM_MNGR_T
#define l338 SOC_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS
#define l341 soc_sand_SAND_ARR_MEM_ALLOCATOR_REQ_BLOCKS_clear
#define l265 nof_reqs
#define l91 nof_mems
#define l220 block_size
#define l281 nof_bits_per_bank
#define l325 soc_sand_arr_mem_allocator_is_availabe_blocks
#define l125 goto
#define l256 soc_pb_pp_sw_db_ipv4_cache_mode_for_ip_type_get
#define l275 SOC_PB_PP_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM
#define l331 soc_pb_pp_sw_db_free_list_add
#define l335 soc_sand_arr_mem_allocator_free
#define l227 nof_banks
#define l363 soc_sand_arr_mem_allocator_malloc
#define l334 soc_sand_os_printf
#define l111 SOC_SAND_PAT_TREE_NODE_KEY
#define l124 SOC_SAND_PAT_TREE_NODE_INFO
#define l83 SOC_SAND_PAT_TREE_INFO
#define l2 pat_tree
#define l337 SOC_SAND_HASH_TABLE_DESTROY
#define l30 nof_vrf_bits
#define l81 pat_tree_get_fun
#define l173 soc_sand_pat_tree_lpm_get
#define l66 TRUE
#define l307 node_place
#define l194 SOC_PB_DO_NOTHING_AND_EXIT
#define l362 soc_pb_pp_sw_db_free_list_commit
#define l304 soc_pb_pp_ipv4_lpm_mngr_lookup0
#define l340 soc_pb_pp_ipv4_lpm_mngr_create
#define l230 nof_lpms
#define l147 lpms
#define l322 node_data_is_identical_fun
#define l339 soc_pb_pp_ipv4_lpm_mngr_pat_tree_node_data_is_identical_fun
#define l356 node_is_skip_in_lpm_identical_data_query_fun
#define l346 soc_pb_pp_ipv4_lpm_mngr_pat_node_is_skip_in_lpm_identical_data_query_fun
#define l306 soc_sand_pat_tree_create
#define l364 soc_sand_arr_mem_allocator_create
#define l195 mem_allocators
#define l359 soc_sand_os_malloc
#define l143 flags
#define l137 SOC_PB_PP_LPV4_LPM_SUPPORT_DEFRAG
#define l329 soc_sand_group_mem_ll_create
#define l126 rev_ptrs
#define l317 soc_pb_pp_ipv4_lpm_mngr_vrf_init
#define l170 SOC_SAND_PP_SYSTEM_FEC_ID
#define l12 key
#define l282 SOC_SAND_PAT_TREE_NODE_DATA
#define l196 soc_sand_pat_tree_clear
#define l201 SOC_PPC_FRWRD_IP_ROUTE_STATUS_COMMITED
#define l179 SOC_PPC_FRWRD_IP_ROUTE_LOCATION_LPM
#define l189 soc_sand_pat_tree_node_add
#define l351 soc_pb_pp_ipv4_lpm_mngr_vrf_clear
#define l186 SOC_PPC_FRWRD_IP_ROUTE_STATUS
#define l108 SOC_SAND_PP_IPV4_SUBNET
#define l311 soc_sand_SAND_PP_IPV4_SUBNET_clear
#define l98 ip_address
#define l95 prefix_len
#define l321 nof_entries_for_hw_lpm_set_fun
#define l292 soc_pb_pp_ipv4_lpm_mngr_prefix_add
#define l326 soc_pb_pp_ipv4_lpm_mngr_all_vrfs_clear
#define l296 soc_sand_arr_mem_allocator_clear
#define l293 soc_sand_group_mem_ll_clear
#define l347 soc_pb_pp_ipv4_lpm_mngr_destroy
#define l367 soc_sand_pat_tree_destroy
#define l330 soc_sand_arr_mem_allocator_destroy
#define l360 soc_sand_group_mem_ll_destroy
#define l239 soc_sand_os_free_any_size
#define l262 soc_pb_pp_ipv4_lpm_mngr_pat_foreach_fn_hw_sync
#define l361 SOC_SAND_PAT_TREE_KEY
#define l280 is_prefix
#define l204 SOC_PPC_FRWRD_IP_ROUTE_LOCATION
#define l353 SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_REMOVE
#define l251 SOC_PPC_FRWRD_IP_ROUTE_STATUS_PEND_ADD
#define l350 soc_pb_pp_ipv4_lpm_mngr_hw_sync
#define l158 NULL
#define l365 SOC_SAND_MALLOC
#define l258 max_nof_entries_for_hw_lpm
#define l308 soc_sand_pat_tree_foreach
#define l250 soc_sand_pat_tree_node_remove
#define l323 SOC_SAND_FREE
#define l352 soc_pb_pp_ipv4_lpm_mngr_mark_non_lpm_route_pending_type_set
#define l205 sys_fec_id
#define l236 SOC_SAND_SUCCESS_FAILURE
#define l182 SOC_SAND_SUCCESS
#define l176 soc_pb_pp_ipv4_lpm_mngr_cache_set
#define l214 soc_pb_pp_ipv4_lpm_mngr_sync
#define l255 soc_sand_bitstream_get_any_field
#define l279 continue
#define l333 SOC_SAND_FAILURE_OUT_OF_RESOURCES
#define l328 soc_sand_arr_mem_allocator_commit
#define l343 soc_sand_group_mem_ll_commit
#define l342 soc_sand_pat_tree_cache_commit
#define l320 soc_sand_arr_mem_allocator_cache_set
#define l348 soc_sand_group_mem_ll_cache_set
#define l366 SOC_PB_PP_FRWRD_IP_ALL_VRFS_ID
#define l300 soc_sand_pat_tree_cache_set
#define l332 soc_pb_pp_ipv4_lpm_mngr_prefix_remove
#define l276 soc_pb_pp_ipv4_lpm_mngr_prefix_lookup
#define l357 soc_pb_pp_ipv4_lpm_mngr_prefix_is_exist
#define l309 soc_pb_pp_ipv4_lpm_mngr_sys_fec_get
#define l336 soc_pb_pp_ipv4_lpm_mngr_get_stat
#define l215 SOC_PB_PP_IPV4_LPM_MNGR_STATUS
#define l234 SOC_SAND_CHECK_NULL_INPUT
#define l355 soc_sand_pat_tree_get_size
#define l261 total_nodes
#define l297 prefix_nodes
#define l283 free_nodes
#define l315 soc_pb_pp_ipv4_lpm_mngr_clear
#define l327 soc_pb_pp_ipv4_lpm_mngr_get_block
#define l354 SOC_SAND_PAT_TREE_ITER_TYPE
#define l344 SOC_PETRA_ALLOC
#define l345 while
#define l314 SOC_SAND_PAT_TREE_ITER_IS_END
#define l349 soc_sand_pat_tree_get_block
#define l305 SOC_PETRA_FREE
#define l313 soc_pb_pp_PB_PP_IPV4_LPM_MNGR_INFO_clear
#define l100 info
#define l303 SOC_SAND_MAGIC_NUM_SET
#define l301 SOC_SAND_VOID_EXIT_AND_SEND_ERROR
#define l319 soc_pb_pp_PB_PP_IPV4_LPM_MNGR_STATUS_clear
l164 l45 l312;l164 ld l7;l164 l316{l229,l103,l92,l105,l36,l51,l67,l24
,l115}l121;l164 l278 l65;lz l45 l197[]={0,35,0,-1,35};lz ld l142[]={0
,36,0,32,36};l164 l264{l264{ld l13;lt l11;lt l324;} *l94;ld l155;}
l192;lz l56 l40(l8 lv*l39,lf lv*l41){l318(l39->li,l41->li,l135( *l39));
}lz l45 l107(lf lw*la,l8 lv*l39,lf lv*l41){lf l10*lb;lb=&(la->ls);lg(
l142[lb->lp]<=32){lj((l39->li[0]^l41->li[0])&(0xffffffff>>(32-l142[lb
->lp])));}ll{lj(l39->li[0]!=l41->li[0]||((l39->li[1]^l41->li[1])&(
0xffffffff>>(64-l142[lb->lp]))));}}lz lt l268(lf lw*la,lf l130*l19){
ld lu;lg(l142[la->ls.lp]<=32){lu=l19->li[0]&(0xffffffff>>(32-l142[la
->ls.lp]));}ll{lu=l19->li[0];lu^=l19->li[1]&(0xffffffff>>(64-l142[la
->ls.lp]));}lu=lu^lu>>16;lu=lu^lu>>8;lu=lu^lu>>4;lu=lu^lu>>2;lu=lu^lu
>>1;lj(((lt)lu)&1);}lz l56 l185(lf lw*la,l14 l130*l19){ld lu;lf l10*
lb;lb=&(la->ls);lg(lb->lp==l37){lj;}lu=l268(la,l19);lg(l197[lb->lp]<
32){l19->li[0]^=lu<<l197[lb->lp];}ll{l19->li[1]^=lu<<(l197[lb->lp]-32
);}}lz lt l295(lf l130*l19){ld lu;lu=l19->li[0]&0x3ffff;lu=lu^lu>>16;
lu=lu^lu>>8;lu=lu^lu>>4;lu=lu^lu>>2;lu=lu^lu>>1;lj(((lt)lu)&1);}lz l56
l248(l14 l130*l19){ld lu;lu=l295(l19);l19->li[0]^=lu<<17;}lz lt l298(
lf l130*l19){ld lu;lu=l19->li[0]&0xfffc0000;lu^=l19->li[1]&0x0000000f
;lu=lu^lu>>16;lu=lu^lu>>8;lu=lu^lu>>4;lu=lu^lu>>2;lu=lu^lu>>1;lj(((lt
)lu)&1);}lz l56 l247(l14 l130*l19){ld lu;lu=l298(l19);l19->li[1]^=lu
<<3;}lz l45 l84(ld l89){l45 l58=0;lg(l89==0)lj(0x10000000);lg(l89>=(1
<<16)){l58+=16;l89>>=16;}lg(l89>=(1<<8)){l58+=8;l89>>=8;}lg(l89>=(1<<
4)){l58+=4;l89>>=4;}lg(l89>=(1<<2)){l58+=2;l89>>=2;}lg(l89>=(1<<1)){
l58+=1;l89>>=1;}lj(l58);}lz l56 l252(lf lw*la,l8 lv*le,lf ld lc,lf ld
l5,lf ld l42,lf ld lx){l45 l44;lf l10*lb;lb=&(la->ls);lg(lb->lp==l29
||lb->lp==l34){lg(l5==4){l44=l84(lb->l117[lb->l88[lc]]-1)-16;lg(l44>0
/* Petra b code. Almost not in use. Ignore coverity defects */
/* coverity[large_shift] */
){le->li[0]=lx>>l44;}ll lg(l44<0){le->li[0]=lx<<-l44;}}ll{le->li[0]=
lx&0x1fffff;}lg(l5==4){le->li[0]|=l42<<17;le->li[1]=l42>>15;}ll{le->
li[0]|=l42<<21;}lg(l5==4){le->li[1]|=0x4;}ll lg(l5==3){le->li[0]|=1<<
31;le->li[1]=0x6;}ll lg(l5==2){le->li[1]=0x7;}l185(la,le);}ll lg(lb->
lp==l37){le->li[0]=(lx>>1)&0x1fff;le->li[0]|=(l42&0xff)<<13;le->li[0]
|=0x400000;}ll lg(lb->lp==l50){le->li[0]=(lx>>1)&0x3fff;le->li[0]|=(
l42&0xf)<<14;le->li[0]|=0x180000;}ll{}}lz l56 l249(lf lw*la,l8 lv*le,
lf ld lc,lf ld l5,lf ld l23,lf ld lx){l45 l44;lf l10*lb;lb=&(la->ls);
lg(lb->lp==l29||lb->lp==l34){l44=l84(lb->l117[lb->l88[lc]]-1)-17;lg(
/* Petra b code. Almost not in use. Ignore coverity defects */
/* coverity[large_shift] */
l44>0){le->li[0]=lx>>l44;}ll lg(l44<0){le->li[0]=lx<<-l44;}le->li[0]
|=l23<<18;le->li[1]=l23>>14;l185(la,le);}ll lg(lb->lp==l37){le->li[0]
=lx&0x3fff;le->li[0]|=l23<<14;}ll{}}lz l56 l271(lf lw*la,l8 lv*le,lf
ld lc,lf ld l54,lf ld l4,lf ld lx){lf l10*lb;lb=&(la->ls);lg(lb->lp==
l29||lb->lp==l34){le->li[0]=lx&0x1fffff;le->li[0]|=(l4<<(30-l54))&
0x3fffffff;le->li[0]|=1<<(29-l54);le->li[1]=0x6;l185(la,le);}ll lg(lb
->lp==l37){le->li[0]=(lx>>1)&0x1fff;le->li[0]|=(l4<<(20-l54))&0xfffff
;le->li[0]|=1<<(19-l54);le->li[0]|=0x600000;}ll lg(lb->lp==l50){lg(
l54==5){le->li[0]=(lx>>1)&0x3fff;le->li[0]|=(l4<<(19-l54))&0x7ffff;le
->li[0]|=0x100000;}ll{le->li[0]=(lx>>1)&0x3fff;le->li[0]|=(l4<<(18-
l54))&0x3ffff;le->li[0]|=(l54-1)<<18;}}ll{}}lz ld l294(lf ld l269,lf
ld l168){ld l187=l269;ld l246,l226;l246=l272(l168,5,0);l226=l272(l168
,14,6);l187|=l287(l246,19,14);l187|=l287(l226,31,23);lj l187;}lz l56
l148(lf lw*la,l8 lv*le,lf ld lc,lf ld l60,lf ld l168){lf l10*lb;lb=&(
la->ls);lg(lb->lp==l29||lb->lp==l34){le->li[0]=l60&0xfffff;le->li[0]
|=1<<31;le->li[1]=0x7;l185(la,le);}ll lg(lb->lp==l37){{le->li[0]=l60&
0x3fff;le->li[0]|=0x700000;}le->li[0]=l294(le->li[0],l168);}ll lg(lb
->lp==l50){le->li[0]=l60&0x3fff;le->li[0]|=0x1c0000;}ll{}}lz l121 l35
(lf lw*la,lf lv*le,lf ld lc){lg(la->ls.lp==l29||la->ls.lp==l34){lg(lc
==3)lj(l115);ll lg((le->li[1]&0x4)==0)lj(l103);ll lg((le->li[1]&0x6)==
0x4)lj(l105);ll lg((le->li[1]&0x7)==0x6&&(le->li[0]&0x80000000)==0)lj
(l36);ll lg((le->li[1]&0x7)==0x6&&(le->li[0]&0x80000000)==0x80000000)lj
(l51);ll lg((le->li[1]&0x7)==0x7&&(le->li[0]&0x80000000)==0)lj(l67);
ll lg((le->li[1]&0x7)==0x7&&(le->li[0]&0x80000000)==0x80000000)lj(l24
);}ll lg(la->ls.lp==l37){lg(lc>=5){lj(l24);}ll lg((le->li[0]&0x400000
)==0)lj(l92);ll lg((le->li[0]&0x600000)==0x400000)lj(l51);ll lg((le->
li[0]&0x700000)==0x600000)lj(l36);ll lg((le->li[0]&0x700000)==
0x700000)lj(l24);}ll lg(la->ls.lp==l50){lg((le->li[0]&0x100000)==0)lj
(l36);ll lg((le->li[0]&0x180000)==0x100000)lj(l36);ll lg((le->li[0]&
0x1c0000)==0x180000)lj(l67);ll lg((le->li[0]&0x1c0000)==0x1c0000)lj(
l24);}lj(l229);}lz lt l153(lf lw*la,lf lv*le,lf ld lc){l121 l102;l102
=l35(la,le,lc);lj((lt)((l102==l24)||(l102==l115)));}l56 l310(lf lw*la
,lf ld lc,lf l244*l15,lf ld l263,l8 ld*l180){ld l127;l121 l102;lv le;
l0(l127=0;l127<l263;++l127){le.li[l127]=l15[l127];l180[l127]=l15[l127
];}l102=l35(la,&le,lc);lg(l102==l24){lg(lc>=5){l180[0]= *l15&0x3fff;}
ll{l180[0]= *l15&0x3fff;l180[0]|=0x700000;}}}lz ld l109(lf lw*la,lf ld
lc,lf ld l13,l8 lv*le,lf ld l82){l45 l85;ld l48;lv*l119;lv l72;l144*
l70;l244 l61[2];ld lh;lf l10*lb;l20(0);lb=&(la->ls);l85=l82;l119=le;
l48=l13;lh=lb->l128(lb->l31,lb->l46,lb->l88[lc],&l70);lq(lh,10,lk);lg
((lb->lp==l29||lb->lp==l34)&&lc>=3){l48/=2;l0(;l85>0;l85-=2){lh=l219(
l70,l48,l61);lq(lh,20,lk);l72.li[0]=l61[0];l72.li[1]=l61[1];++l48;
l148(la,&(l119[0]),lc,l72.li[0]&0x1ffff,(0));l148(la,&(l119[1]),lc,(
l72.li[0]>>18)|((l72.li[1]&0x7)<<14),(0));l119+=2;}}ll{l0(;l85>0;l85
--){lh=l219(l70,l48,l61);lq(lh,20,lk);l119->li[0]=l61[0];l119->li[1]=
l61[1];++l48;++l119;}}lk:lj l237;}lz ld l120(lf lw*la,lf ld lc,lf ld
l13,lf lv*le,lf ld l82){lv l72;l45 l85;ld l48;l144*l70;l244 l61[2],
l231[2];ld lh;lf l10*lb;l20(0);l193(&l72,0x0,l135(lv));lb=&(la->ls);
l85=(l45)l82;l48=l13;lh=lb->l128(lb->l31,lb->l46,lb->l88[lc],&l70);lq
(lh,10,lk);lg((lb->lp!=l29&&lb->lp!=l34)||lc<3){l0(;l85>0;l85--){lg(
lc>=5&&lb->lp==l37){l61[0]=le->li[0]&0x3ffff;l61[1]=le->li[1];}ll{l61
[0]=le->li[0];l61[1]=le->li[1];}lh=l219(l70,l48,l231);lq(lh,20,lk);lg
(l231[0]!=l61[0]){lh=l290(l70,l48,l61);lq(lh,20,lk);}++l48;++le;}}ll{
l48/=2;l0(;l85>0;l85-=2){l72.li[0]=0x20000|(le[0].li[0]&0x1ffff);lg(
l85>1){l72.li[0]|=le[1].li[0]<<18;l72.li[1]=(le[1].li[0]>>14)&0x7;}
l248(&l72);l247(&l72);l61[0]=l72.li[0];l61[1]=l72.li[1];lh=l290(l70,
l48,l61);lq(lh,20,lk);++l48;le+=2;}}lk:l28("\x65\x72\x72\x6f\x72\x20"
"\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34"
"\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x77\x72\x69\x74\x65\x5f\x72"
"\x6f\x77\x73\x28\x29",0,0);}lz lt l284(lf lw*la,l8 lv*l39,lf lv*l41,
lf ld lc){lf l10*lb;lb=&(la->ls);lg(lb->lp==l29||lb->lp==l34){l16(l35
(la,l41,lc)){lm l103:lj(lt)(((l41->li[1]&0x7)==(l39->li[1]&0x7))&&((
l41->li[0]&~0x3ffff)==(l39->li[0]&~0x3ffff)));lm l105:lj(lt)(((l41->
li[1]&0x7)==(l39->li[1]&0x7))&&((l41->li[0]&~0x1ffff)==(l39->li[0]&~
0x1ffff)));lm l36:lj(lt)(((l41->li[1]&0x7)==(l39->li[1]&0x7))&&((l41
->li[0]&~0x1ffff)==(l39->li[0]&~0x1ffff)));lm l51:lj(lt)(((l41->li[1]
&0x7)==(l39->li[1]&0x7))&&((l41->li[0]&~0x1ffff)==(l39->li[0]&~
0x1ffff)));lm l67:lj(lt)(((l41->li[1]&0x7)==(l39->li[1]&0x7))&&((l41
->li[0]&~0x1ffff)==(l39->li[0]&~0x1ffff)));lm l24:lj(lt)(l35(la,l39,
lc)==l24);lm l115:lj(lt)(l35(la,l39,lc)==l115);l33:lj(l26);}}ll lg(lb
->lp==l37){l16(l35(la,l41,lc)){lm l92:lj((lt)((l41->li[0]&~0x3fff)==(
l39->li[0]&~0x3fff)));lm l51:lj((lt)((l41->li[0]&~0x1fff)==(l39->li[0
]&~0x1fff)));lm l36:lj((lt)((l41->li[0]&~0x1fff)==(l39->li[0]&~0x1fff
)));lm l24:lj((lt)(l35(la,l39,lc)==l24));l33:lj(l26);}}ll lg(lb->lp==
l50){l16(l35(la,l41,lc)){lm l67:lm l36:lm l24:lj((lt)((l41->li[0]&
0x1fc000)==(l39->li[0]&0x1fc000)));l33:lj(l26);}}ll{lj(l26);}lj(l26);
}l45 l90(lf lw*la,lf lv*le,lf ld lc){l45 l44;lf l10*lb;lb=&(la->ls);
lg(lb->lp==l29||lb->lp==l34){l16(l35(la,le,lc)){lm l103:l44=l84(lb->
l117[lb->l88[lc]]-1)-17;lj(l44>0?(le->li[0]&0x3ffff)<<l44:(le->li[0]&
/* Petra b code. Almost not in use. Ignore coverity defects */
/* coverity[negative_shift] */
0x3ffff)>>l44);lm l105:l44=l84(lb->l117[lb->l88[lc]]-1)-16;lj(l44>0?(
le->li[0]&0x1ffff)<<l44:(le->li[0]&0x1ffff)>>l44);lm l36:lj(le->li[0]
&0x1fffff);lm l51:lj(le->li[0]&0x1fffff);lm l67:lj(le->li[0]&0x1fffff
);lm l24:lj(-1);lm l115:lj(-1);l33:lj(-1);}}ll lg(lb->lp==l37){l16(
l35(la,le,lc)){lm l92:lj(le->li[0]&0x3fff);lm l51:lm l36:lj((le->li[0
]&0x1fff) *2);lm l24:lj(-1);l33:lj(-1);}}ll lg(lb->lp==l50){l16(l35(
la,le,lc)){lm l67:lm l36:lj((le->li[0]&0x3fff) *2);lm l24:lj(-1);l33:
lj(-1);}}ll{lj(-1);}lj(-1);}l45 l358(lf lw*la,l14 lv*le,lf ld lc,lf ld
l235){lg(la->ls.lp==l37){l16(l35(la,le,lc)){lm l51:lm l36:le->li[0]=(
(le->li[0]&~0x1fff)|l235/2);lj le->li[0];lm l92:lj le->li[0]=((le->li
[0]&~0x3fff)|l235);lm l24:lj(-1);l33:lj(-1);}}ll{lj(-1);}}lz ld l233(
lf lw*la,lf ld lc,lf ld l23,lf ld l5){ld l3;ld l58;ld l212=l23;l212
<<=1;l212|=1;l0(l58=0,l3=0;l3<(ld)(1<<l5);l3++){l16((l212>>l3)&0x3){
lm 0x0:lm 0x2:l18;lm 0x1:l58++;l18;lm 0x3:l58+=1<<(la->l1.ln[lc+2]-la
->l1.ln[lc+1]-l5);l18;}}lj(l58);}lz ld l162(lf lw*la,lf ld lc,lf ld
l42,lf ld l5){ld l3;ld l58;l0(l58=0,l3=0;l3<(ld)(1<<l5);l3++){lg((l42
>>l3)&1){l58+=1<<(la->l1.ln[lc+2]-la->l1.ln[lc+1]-l5);}ll{l58++;}}lj(
l58);}lz ld l259(lf lw*la,lf lv*le,lf ld lc){lf l10*lb;lb=&(la->ls);
lg(lb->lp==l29||lb->lp==l34){l16(l35(la,le,lc)){lm l103:lj(l233(la,lc
,(le->li[0]>>18)|(le->li[1]<<14),4));lm l105:lj(l162(la,lc,(le->li[0]
>>17)|(le->li[1]<<15),4));lm l36:lj(2);lm l51:lj(l162(la,lc,le->li[0]
>>21,3));lm l67:lj(l162(la,lc,le->li[0]>>21,2));lm l24:lj(0);lm l115:
lj(0);l33:lj(0);}}ll lg(lb->lp==l37){l16(l35(la,le,lc)){lm l92:lj(
l233(la,lc,le->li[0]>>14,3));lm l51:lj(l162(la,lc,le->li[0]>>13,3));
lm l36:lj(2);lm l24:lj(0);l33:lj(0);}}ll lg(lb->lp==l50){l16(l35(la,
le,lc)){lm l67:lj(l162(la,lc,le->li[0]>>14,2));lm l36:lj(2);lm l24:lj
(0);l33:lj(0);}}ll{lj(0);}lj(0);}lz ld l151(lf lw*la,lf lv*le,lf ld lc
,lf ld l5,l8 lv*lr){ld l3,l77;ld lx;ld l9;ld l42=0;ld lh;ld l25;ld l78
;lf l10*lb;lf l80*lo;l20(0);lb=&(la->ls);lo=&(la->l1);l78=1<<l5;lx=
l90(la,le,lc);l9=1<<(lo->ln[lc+2]-lo->ln[lc+1]);lg(lb->lp==l29||lb->
lp==l34){lg(l5==4)l42=le->li[0]>>17;ll l42=le->li[0]>>21;}ll lg(lb->
lp==l37){l42=le->li[0]>>13;}ll lg(lb->lp==l50){l42=le->li[0]>>14;}ll{
}l42&=(1<<l78)-1;l25=0;l0(l77=0;l77<l78;l77++){lg((l42>>l77)&1){lh=
l109(la,lc+1,lx,&lr[l25],l9/l78);lq(lh,10,lk);lx+=l9/l78;}ll{lh=l109(
la,lc+1,lx,&lr[l25],1);lq(lh,20,lk);l0(l3=1;l3<l9/l78;l3++){l40(&lr[
l25+l3],&lr[l25]);}lx+=1;}l25+=l9/l78;}lk:l28("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f\x74\x6f"
"\x5f\x66\x75\x6c\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x65\x6c\x69\x64\x65"
"\x5f\x6c\x67\x6e\x28\x29",0,0);}lz ld l241(lf lw*la,lf lv*le,lf ld lc
,lf ld l5,l8 lv*lr){ld l3,l77;ld lx;ld l9;ld l23=0;ld lh;ld l25;ld l76
;lf l10*lb;lf l80*lo;l20(0);lb=&(la->ls);lo=&(la->l1);l76=1<<l5;lx=
l90(la,le,lc);l9=1<<(lo->ln[lc+2]-lo->ln[lc+1]);lg(lb->lp==l29||lb->
lp==l34)l23=(le->li[0]>>18)|(le->li[1]<<14);ll lg(lb->lp==l37){l23=le
->li[0]>>14;}ll{}l23&=(1<<l76)-1;l25=0;l23<<=1;l23|=1;l0(l77=0;l77<
l76;l77++,l23>>=1){l16(l23&0x3){lm 0x0:lm 0x2:l0(l3=0;l3<l9/l76;l3++){
l40(&lr[l25+l3],&lr[l25-1]);}l18;lm 0x1:lh=l109(la,lc+1,lx,&lr[l25],1
);lq(lh,10,lk);l0(l3=1;l3<l9/l76;l3++){l40(&lr[l25+l3],&lr[l25]);}lx
+=1;l18;lm 0x3:lh=l109(la,lc+1,lx,&lr[l25],l9/l76);lq(lh,20,lk);lx+=
l9/l76;l18;}l25+=l9/l76;}lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70"
"\x6d\x5f\x6d\x6e\x67\x72\x5f\x72\x6f\x77\x5f\x74\x6f\x5f\x66\x75\x6c"
"\x6c\x5f\x65\x6c\x69\x64\x65\x5f\x63\x6f\x64\x65\x5f\x6c\x67\x6e\x28"
"\x29",0,0);}lz ld l198(lf lw*la,lf lv*le,lf ld lc,l8 lv*lr){ld lx;ld
l9;ld lh;ld l25;ld l4=0;ld l54=0;ld l131;ld l200;ld l165;lf l10*lb;lf
l80*lo;l20(0);lb=&(la->ls);lo=&(la->l1);lx=l90(la,le,lc);l9=1<<(lo->
ln[lc+2]-lo->ln[lc+1]);lg(lb->lp==l29||lb->lp==l34){l4=(le->li[0]>>21
)&0x1fff;l54=8;}ll lg(lb->lp==l37){l4=(le->li[0]>>13)&0x7f;l54=6;}ll
lg(lb->lp==l50){lg((le->li[0]&0x100000)==0){l4=(le->li[0]>>14)&0xf;l4
>>=3-((le->li[0]>>18)&0x3);l4<<=3-((le->li[0]>>18)&0x3);l4<<=1;l4|=1
<<(3-((le->li[0]>>18)&0x3));l4<<=1;}ll{l4=(le->li[0]>>14)&0x1f;l4<<=1
;l4|=1;}l54=5;}ll{}lg((lo->ln[lc+2]-lo->ln[lc+1])<l54){l4>>=l54-(lo->
ln[lc+2]-lo->ln[lc+1]);}ll lg((lo->ln[lc+2]-lo->ln[lc+1])>l54){l4<<=(
lo->ln[lc+2]-lo->ln[lc+1])-l54;}l131=((l4-1)^l4);l131>>=1;l4>>=1;l4&=
~l131;l200=l4;lg(l4==0){l165=l4+l131+1;}ll{l165=0;}l0(l25=0;l25<l9;
l25++){lg((l25&~l131)==(l4&~l131)){lg(l25==l200){lh=l109(la,lc+1,lx+1
,&lr[l25],1);lq(lh,10,lk);}ll{l40(&lr[l25],&lr[l200]);}}ll{lg(l25==
l165){lh=l109(la,lc+1,lx,&lr[l25],1);lq(lh,20,lk);}ll{l40(&lr[l25],&
lr[l165]);}}}lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63"
"\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d"
"\x6e\x67\x72\x5f\x72\x6f\x77\x5f\x74\x6f\x5f\x66\x75\x6c\x6c\x5f\x65"
"\x6c\x69\x64\x65\x5f\x63\x6f\x6d\x70\x61\x72\x65\x28\x29",0,0);}lz
l56 l206(lf lw*la,lf lv*le,lf ld lc,l14 lv*lr){ld l9;ld l77;lf l80*lo
;lo=&(la->l1);l9=1<<(lo->ln[lc+2]-lo->ln[lc+1]);l0(l77=0;l77<l9;l77++
){l40(&lr[l77],le);}}lz l56 l277(lf lw*la,lf lv*le,lf ld lc,l14 lv*lr
){lf l10*lb;lb=&(la->ls);lg(lb->lp==l29||lb->lp==l34){l16(l35(la,le,
lc)){lm l103:l241(la,le,lc,4,lr);lj;lm l105:l151(la,le,lc,4,lr);lj;lm
l36:l198(la,le,lc,lr);lj;lm l51:l151(la,le,lc,3,lr);lj;lm l67:l151(la
,le,lc,2,lr);lj;lm l24:lm l115:l206(la,le,lc,lr);lj;l33:lj;}}ll lg(lb
->lp==l37){l16(l35(la,le,lc)){lm l92:l241(la,le,lc,3,lr);lj;lm l51:
l151(la,le,lc,3,lr);lj;lm l36:l198(la,le,lc,lr);lj;lm l24:l206(la,le,
lc,lr);lj;l33:lj;}}ll lg(lb->lp==l50){l16(l35(la,le,lc)){lm l67:l151(
la,le,lc,2,lr);lj;lm l36:l198(la,le,lc,lr);lj;lm l24:l206(la,le,lc,lr
);lj;l33:lj;}}ll{}lj;}lz l45 l138(lf lw*la,l8 lv*le,lf ld lc,lf ld l5
,l14 lv*lr,lf l65 lx){ld l9;ld l38;ld l74;lv l163;lt l172=1;ld l42;lf
l80*lo;lo=&(la->l1);l9=1<<(lo->ln[lc+2]-lo->ln[lc+1]);l42=0;l0(l38=0,
l74=0;l38<l9;){lg(l38%(1<<(lo->ln[lc+2]-lo->ln[lc+1]-l5))==0){l172=1;
l40(&l163,&lr[l38]);}lg(lx!=-1)l40(&lr[l74],&lr[l38]);lg(l107(la,&
l163,&lr[l38])!=0)l172=0;l38++;l74++;lg((l38)%(1<<(lo->ln[lc+2]-lo->
ln[lc+1]-l5))==0){l42|=(~l172&1)<<(((l38)>>(lo->ln[lc+2]-lo->ln[lc+1]
-l5))-1);lg(l172)l74-=(1<<(lo->ln[lc+2]-lo->ln[lc+1]-l5))-1;}}l252(la
,le,lc,l5,l42,lx);lg(lx!=-1)l120(la,lc+1,lx,lr,l74);lj(l74);}lz l45
l166(lf lw*la,l8 lv*le,lf ld lc,lf ld l5,l14 lv*lr,lf l65 lx){ld l9;
ld l38;ld l74;ld l23;ld l93=0;ld l225=0;ld l174=0;lf l80*lo;lo=&(la->
l1);l9=1<<(lo->ln[lc+2]-lo->ln[lc+1]);l23=1;l0(l38=0;l38<l9;l38++){lg
(l38%(1<<(lo->ln[lc+2]-lo->ln[lc+1]-l5))==0){l93=l38>>(lo->ln[lc+2]-
lo->ln[lc+1]-l5);l174=l93<<(lo->ln[lc+2]-lo->ln[lc+1]-l5);l225=l174-1
;}ll{lg(l107(la,&lr[l174],&lr[l38])!=0){l23|=0x2<<l93;}}lg(l174==0||
l107(la,&lr[l225],&lr[l38])!=0)l23|=0x1<<l93;}l0(l74=0,l93=0,l38=0;
l93<(ld)(1<<l5);l93++){lg(l23&(0x1<<l93)){lg(l23&(0x2<<l93)){l0(;l38<
(l93+1)<<(lo->ln[lc+2]-lo->ln[lc+1]-l5);l38++,l74++){lg(lx!=-1)l40(&
lr[l74],&lr[l38]);}}ll{lg(lx!=-1)l40(&lr[l74],&lr[l38]);l74++;l38+=1
<<(lo->ln[lc+2]-lo->ln[lc+1]-l5);}}ll{l38+=1<<(lo->ln[lc+2]-lo->ln[lc
+1]-l5);}}l23>>=1;l249(la,le,lc,l5,l23,lx);lg(lx!=-1)l120(la,lc+1,lx,
lr,l74);lj(l74);}lz l45 l224(lf lw*la,l8 lv*le,lf ld lc,l14 lv*lr,lf
l65 lx){ld l9;ld l69;lv l159[2];ld l62[2];ld l134[2];ld l75;lf l80*lo
;lf l10*lb;lb=&(la->ls);lo=&(la->l1);l9=1<<(lo->ln[lc+2]-lo->ln[lc+1]
);l40(&l159[0],&lr[0]);l62[0]=1;l134[0]=0;l62[1]=0;l0(l69=1;l69<l9&&
l107(la,&lr[l69],&l159[0])==0;l69++){l62[0]++;}lg(l69>=l9){lj(-1);}
l40(&l159[1],&lr[l69]);l134[1]=l69;l62[1]++;l0(l69++;l69<l9&&l107(la,
&lr[l69],&l159[1])==0;l69++){l62[1]++;}l0(;l69<l9&&l107(la,&lr[l69],&
l159[0])==0;l69++){l62[0]++;}lg(l69<l9){lj(-1);}lg(l62[0]<l62[1])l75=
0;ll l75=1;lg(((l62[l75]-1)&l62[l75])!=0||((l62[l75]-1)&l134[l75])!=0
){lj(-1);}l16(lb->lp){lm l29:lm l34:lg(l84(l9/l62[l75])>8)lj(-1);l18;
lm l37:lg(l84(l9/l62[l75])>6)lj(-1);l18;lm l50:lg(l84(l9/l62[l75])>5)lj
/* Petra b code. Almost not in use. Ignore coverity defects */
/* coverity[large_shift] */
(-1);l18;l33:;}l271(la,le,lc,l84(l9/l62[l75]),l134[l75]>>l84(l62[l75]
),lx);lg(lx!=-1){l120(la,lc+1,lx,&lr[l134[1-l75]],1);l120(la,lc+1,lx+
1,&lr[l134[l75]],1);}lj(2);}lz l45 l213(lf lw*la,l8 lv*le,lf ld lc,
l14 lv*lr){ld l9;ld l146;lf l80*lo;lo=&(la->l1);l9=1<<(lo->ln[lc+2]-
lo->ln[lc+1]);l0(l146=1;l146<l9;l146++){lg(l107(la,&lr[0],&lr[l146])!=
0)l18;}l40(le,&lr[0]);lg(l146==l9){lj(0);}ll lj(-1);}lz ld l291(lf lw
 *la,l8 lv*le,lf ld lc,l14 lv*lr,l8 l121*l71){l45 l49;l45 l22;lv l132
;lf l10*lb;lb=&(la->ls);l49=(1<<(8))+1;l22=l213(la,le,lc,lr);lg(l22!=
-1&&l22<l49){l49=l22; *l71=l24;lj(l49);}l22=l224(la,le,lc,lr,((l65)(-
1)));lg(l22!=-1&&l22<l49){l49=l22; *l71=l36;lj(l49);}lg(lb->lp==l37||
lb->lp==l29||lb->lp==l34){l22=l138(la,le,lc,3,lr,((l65)(-1)));lg(l22
!=-1&&l22<l49){l49=l22; *l71=l51;l40(&l132,le);}}lg(lb->lp==l37){l22=
l166(la,le,lc,3,lr,((l65)(-1)));lg(l22!=-1&&l22<l49){l49=l22; *l71=
l92;l40(&l132,le);}}lg(lb->lp==l34){l22=l166(la,le,lc,4,lr,((l65)(-1)));
lg(l22!=-1&&l22<l49){l49=l22; *l71=l103;l40(&l132,le);}l22=l138(la,le
,lc,4,lr,((l65)(-1)));lg(l22!=-1&&l22<l49){l49=l22; *l71=l105;l40(&
l132,le);}}lg(lb->lp==l34||lb->lp==l50){l22=l138(la,le,lc,2,lr,((l65)(
-1)));lg(l22!=-1&&l22<l49){l49=l22; *l71=l67;l40(&l132,le);}}l40(le,&
l132);lj(l49);}lz l56 l228(lf lw*la,l8 lv*le,lf ld lc,l14 lv*lr,lf l65
lx,l8 l121 l71){l16(l71){lm l103:l166(la,le,lc,4,lr,lx);l18;lm l92:
l166(la,le,lc,3,lr,lx);l18;lm l105:l138(la,le,lc,4,lr,lx);l18;lm l51:
l138(la,le,lc,3,lr,lx);l18;lm l67:l138(la,le,lc,2,lr,lx);l18;lm l36:
l224(la,le,lc,lr,lx);l18;lm l24:l213(la,le,lc,lr);l18;l33:l18;}}lz ld
l245(l14 lw*la,lf ld l254,l8 lt*l232){l144*l114;l338 l118;ld l47;lf
l10*lb;ld lh=l237;l20(0);lb=&(la->ls);l341(&l118);lg(l254==0){l118.
l265=2;}ll{l118.l265=1;}l0(l47=1;l47<lb->l91;++l47){lh=lb->l128(lb->
l31,lb->l46,l47,&l114);lq(lh,10,lk);l118.l220[0]=1<<la->ls.l281[l47];
l118.l220[1]=l118.l220[0];lh=l325(l114,&l118,l232);lq(lh,30,lk);lg(! *
l232){l125 lk;}}lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f"
"\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f"
"\x6d\x6e\x67\x72\x5f\x6d\x65\x6d\x6f\x72\x79\x5f\x73\x75\x66\x66\x69"
"\x63\x69\x65\x6e\x74\x28\x29",0,0);}lz ld l299(lf lw*la,lf ld lc,lf
l65 l13){l65 l48;l144*l70;lf l10*lb;lt l157;ld lh;l20(0);lb=&(la->ls);
lh=lb->l128(lb->l31,lb->l46,lb->l88[lc],&l70);lq(lh,10,lk);lg(lb->lp
==l29&&lc>=3){l48=l13/2;}ll{l48=l13;}lh=l256(lb->l31,l275,&l157);lq(
lh,5,lk);lg(!l157){lh=l331(lb->l31,lb->l88[lc],l48);lq(lh,30,lk);}ll{
lh=l335(l70,l48);lq(lh,40,lk);}lk:lj l237;}lz l56 l202(lf lw*la,lf ld
lc,lf lv*le){ld lx;lx=l90(la,le,lc);l299(la,lc+1,lx);}lz l56 l190(lf
lw*la,lf ld lc,lf lv*le){ld l3,lx,l216;lv l163;lf l10*lb;lb=&(la->ls);
lx=l90(la,le,lc);lg(lx==-1)lj;l216=l259(la,le,lc);lg(lc<lb->l227-2)l0
/* Petra b code. Almost not in use. Ignore coverity defects */
/* coverity[overrun-buffer-val] */
(l3=0;l3<l216;l3++){l109(la,lc+1,lx+l3,&l163,1);l190(la,lc+1,&l163);}
l202(la,lc,le);}lz ld l243(lf lw*la,lf ld lc,lf l121 l102,lf ld l238,
l8 l65*l169){l278 l184;l144*l70;ld l209;ld l97=1;ld lh;lf l10*lb;l20(
0);lb=&(la->ls);lg(lb->lp==l29||lb->lp==l34){l16(l102){lm l51:lm l67:
lm l36:l97=1;l18;lm l103:lg(l84(lb->l117[lb->l88[lc]]-1)-17>0){l97=1
<<(l84(lb->l117[lb->l88[lc]]-1)-17);}l18;lm l105:lg(l84(lb->l117[lb->
/* Petra b code. Almost not in use. Ignore coverity defects */
/* coverity[negative_shift] */
l88[lc]]-1)-16>0){l97=1<<(l84(lb->l117[lb->l88[lc]]-1)-16);}l18;l33:
l97=1;l18;}}ll lg(lb->lp==l37){l16(l102){lm l51:l97=2;l18;lm l92:l97=
1;l18;lm l36:l97=2;l18;l33:l97=1;l18;}}ll lg(lb->lp==l50){l97=2;}ll{}
lg(lb->lp==l29&&lc>=3)l209=(l238+1)/2;ll l209=l238;lh=lb->l128(lb->
l31,lb->l46,lb->l88[lc],&l70);lq(lh,10,lk);l363(l70,l209,l97,&l184);
lg(l184==-1){l334("\x45\x52\x52\x4f\x52\x3a\x20\x4c\x50\x4d\x20\x6d"
"\x61\x6c\x6c\x6f\x63\x20\x72\x61\x6e\x20\x6f\x75\x74\x20\x6f\x66\x20"
"\x6d\x65\x6d\x6f\x72\x79\n"); *l169=(0xFFFFFFFF);}lg(lb->lp==l29&&lc
>=3)l184*=2; *l169=l184;lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20"
"\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70"
"\x6d\x5f\x6d\x6e\x67\x72\x5f\x6d\x61\x6c\x6c\x6f\x63\x28\x29",0,0);}
lz ld l218(lf lw*la,lf ld ly,lf ld l13,lf lt l11,lf ld lc,lf lv*l171,
l8 lv*l208,lf lt l188,l8 lt*l6){lt l210;lv l68,l55,lr[(1<<(8))];l121
l71=0;ld l73,l82,l101,l161,l96;l111 l21;l124 l32;lt l64;lt l43;ld lh;
ld l207;l65 l150;l83*l2;lt l152;lf l10*lb;lf l80*lo;l20(l337);l96=l13
;l43=l11;lb=&(la->ls);lo=&(la->l1);l21.l19=l96;l21.l11=(lt)(lo->ln[lc
]-lb->l30);lh=lb->l81(lb->l31,lb->l46,ly,&l2);lq(lh,10,lk);lh=l173(l2
,&l21,l66,&l32,&l64);lq(lh,20,lk);lg(l64){l161=(((l32.l15.l60)>>0)&
0xFFFFFFF);{l148(la,l208,lc-1,l161,l32.l307);lg(l6){ *l6=l66;}l194;}}
l277(la,l171,lc-1,lr);lg(l43<32){l96&=~(((ld)-1)>>l43);}lg(32+lb->l30
-lo->ln[lc+1]<32){l73=l96>>(32+lb->l30-lo->ln[lc+1]);}ll{l73=0;}l73&=
((ld)(-1))>>(32-(lo->ln[lc+1]-lo->ln[lc]));l82=1;lg(l43+lb->l30<lo->
ln[lc+1]){l82<<=lo->ln[lc+1]-l43-lb->l30;}lg(l43==0){l101=0;}ll{l101=
l96&(((ld)(-1))<<(32-l43));}lg(l43<lo->ln[lc+1]-lb->l30){l43=(lt)(lo
->ln[lc+1]-lb->l30);}l210=0;l0(;l82;l82--,l73++,l101+=(1<<(32+lb->l30
-lo->ln[lc+1]))){l40(&l68,&lr[l73]);lh=l218(la,ly,l101,l43,lc+1,&l68,
&l55,l188,&l152);lq(lh,30,lk);lg(!l152){lg(l6){ *l6=l26;}l194;}lg(
l107(la,&l55,&l68)!=0){l210=1;lg(!l153(la,&l68,lc)){lg(l153(la,&l55,
lc)){l190(la,lc,&l68);}ll lg(l90(la,&l68,lc)!=l90(la,&l55,lc)){l202(
la,lc,&l68);}}l40(&lr[l73],&l55);}}lg(!l210){l40(l208,l171);}ll{l207=
l291(la,&l55,lc-1,lr,&l71);lg(!l284(la,&l55,l171,lc-1)){lg(l207!=0){
lh=l243(la,lc,l71,l207,&l150);lq(lh,10,lk);lg(l150==(0xFFFFFFFF)){lg(
l6){ *l6=l26;}l194;}l228(la,&l55,lc-1,lr,l150,l71);}}ll{l150=l90(la,
l171,lc-1);l228(la,&l55,lc-1,lr,l150,l71);}l40(l208,&l55);}lg(l6){ *
l6=l66;}lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f"
"\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e"
"\x67\x72\x5f\x61\x64\x64\x5f\x70\x72\x65\x66\x69\x78\x5f\x68\x65\x6c"
"\x70\x65\x72\x28\x29",0,0);}lz ld l183(lf lw*la,lf ld ly,lf ld l13,
lf lt l11,lf lt l188,l8 lt*l6){ld lc;lv l55,l68;ld l73,l101,l82,l96;
lt l157;lt l43;ld lh;lt l152;lf l10*lb;lf l80*lo;l20(0);lb=&(la->ls);
lo=&(la->l1);l43=l11;l96=l13;lc=0;lg(l43<32){l96&=~(((ld)-1)>>l43);}
lg(32+lb->l30-lo->ln[lc+1]<32){l73=l96>>(32+lb->l30-lo->ln[lc+1]);l73
&=((ld)(-1))>>(32+lb->l30-(lo->ln[lc+1]-lo->ln[lc]));}ll{l73=0;}l73|=
ly<<(lo->ln[1]-lb->l30);l82=1;lg(l43+lb->l30<lo->ln[lc+1]){l82<<=lo->
ln[lc+1]-l43-lb->l30;}lg(l43==0){l101=0;}ll{l101=l96&(((ld)(-1))<<(32
-l43));}lg(l43<lo->ln[lc+1]-lb->l30){l43=(lt)(lo->ln[lc+1]-lb->l30);}
l0(;l82;l82--,l73++,l101+=(1<<(32+lb->l30-lo->ln[lc+1]))){lh=l109(la,
lc,l73,&l68,1);lq(lh,10,lk);lh=l218(la,ly,l101,l43,lc+1,&l68,&l55,
l188,&l152);lq(lh,20,lk);lg(!l152){lg(l6){ *l6=l26;}l194;}lg(l107(la,
&l55,&l68)!=0){lg(!l153(la,&l68,lc)){lg(l153(la,&l55,lc)){l190(la,lc,
&l68);}ll lg(l90(la,&l68,lc)!=l90(la,&l55,lc)){l202(la,lc,&l68);}}lh=
l120(la,lc,l73,&l55,1);lq(lh,20,lk);}}l256(lb->l31,l275,&l157);lg(!
l157){l362(lb->l31);}lg(l6){ *l6=l66;}lk:l28("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x61\x64\x64\x5f\x70\x72"
"\x65\x66\x69\x78\x5f\x68\x65\x6c\x70\x65\x72\x30\x28\x29",0,0);}lz l7
l274(lf lw*la,lf lv*le,lf ld lc){lg(la->ls.lp==l29||la->ls.lp==l34){
l16(l35(la,le,lc)){lm l24:lj(le->li[0]&0xfffff);lm l115:l33:lj(l7)(-1
);}}ll lg(la->ls.lp==l37){l16(l35(la,le,lc)){lm l24:lj(le->li[0]&
0x3fff);l33:lj(l7)(-1);}}ll lg(la->ls.lp==l50){l16(l35(la,le,lc)){lm
l24:lj(le->li[0]&0x3fff);l33:lj(l7)(-1);}}ll{lj(l7)(-1);}lj(l7)(-1);}
lz l7 l266(lf lw*la,lf lv*le,lf ld lc,lf ld l5){lg(la->ls.lp==l29||la
->ls.lp==l34){l16(l5){lm 4:lj(((le->li[0]>>18)|(le->li[1]<<14))&
0xffff);}}ll lg(la->ls.lp==l37){l16(l5){lm 3:lj((le->li[0]>>14)&0xff);
}}lj(l7)(-1);}lz l7 l222(lf lw*la,lf lv*le,lf ld lc,lf l7 l53,lf ld l5
){l7 l23;l7 l87;ld l3;ld l76;ld l104;l7 l57=l53;l23=l266(la,le,lc,l5);
l104=1<<(la->l1.ln[lc+2]-la->l1.ln[lc+1]);l76=1<<l5;l23=(l23<<1)|1;
l87=0;l0(l3=0;l3<l76;l3++){lg(l57<l104/l76)l16((l23>>l3)&0x3){lm 0x0:
lm 0x2:lj(l87-1);lm 0x1:lj(l87);lm 0x3:lj(l87+l57);}l57-=l104/l76;l16
((l23>>l3)&0x3){lm 0x0:lm 0x2:l18;lm 0x1:l87+=1;l18;lm 0x3:l87+=l104/
l76;l18;}}lj(l7)(-1);}lz l7 l285(lf lw*la,lf lv*le,lf ld lc,lf ld l5){
lg(la->ls.lp==l29||la->ls.lp==l34){l16(l5){lm 4:lj(((le->li[0]>>17)|(
le->li[1]<<15))&0xffff);lm 3:lj((le->li[0]>>21)&0xff);lm 2:lj((le->li
[0]>>21)&0xf);l33:lj(l7)(-1);}}ll lg(la->ls.lp==l37){l16(l5){lm 3:lj(
(le->li[0]>>13)&0xff);l33:lj(l7)(-1);}}ll lg(la->ls.lp==l50){l16(l5){
lm 2:lj((le->li[0]>>14)&0xf);l33:lj(l7)(-1);}}ll{lj(l7)(-1);}lj(l7)(-
1);}lz l7 l156(lf lw*la,lf lv*le,lf ld lc,lf l7 l53,lf ld l5){l7 l42;
l7 l87;ld l3;ld l78;ld l104;ld l57=l53;l42=l285(la,le,lc,l5);l104=1<<
(la->l1.ln[lc+2]-la->l1.ln[lc+1]);l78=1<<l5;l87=0;l0(l3=0;l3<l78;l3++
){lg(l57<l104/l78)l16((l42>>l3)&0x1){lm 0x0:lj(l87);lm 0x1:lj(l87+l57
);}l57-=l104/l78;l16((l42>>l3)&0x1){lm 0x0:l87+=1;l18;lm 0x1:l87+=
l104/l78;l18;}}lj(l7)(-1);}lz l7 l302(lf lw*la,lf lv*le,lf ld lc){lg(
la->ls.lp==l29||la->ls.lp==l34){lj((le->li[0]>>21)&0x1ff);}ll lg(la->
ls.lp==l37){lj((le->li[0]>>13)&0x7f);}ll lg(la->ls.lp==l50){ld l4;lg(
(le->li[0]&0x100000)==0){l4=(le->li[0]>>14)&0xf;l4>>=3-((le->li[0]>>
18)&0x3);l4<<=3-((le->li[0]>>18)&0x3);l4<<=1;l4|=1<<(3-((le->li[0]>>
18)&0x3));l4<<=1;}ll{l4=(le->li[0]>>14)&0x1f;l4<<=1;l4|=1;}lj(l4);}lj
(l7)(-1);}lz l7 l203(lf lw*la,lf lv*le,lf ld lc,lf l7 l53){l7 l4;l7
l57=l53;l4=l302(la,le,lc);lg(la->ls.lp==l29||la->ls.lp==l34){lg(la->
l1.ln[lc+2]-la->l1.ln[lc+1]<=9)l57<<=9-(la->l1.ln[lc+2]-la->l1.ln[lc+
1]);ll l57>>=(la->l1.ln[lc+2]-la->l1.ln[lc+1])-9;}ll lg(la->ls.lp==
l37){lg(la->l1.ln[lc+2]-la->l1.ln[lc+1]<=7)l57<<=7-(la->l1.ln[lc+2]-
la->l1.ln[lc+1]);ll l57>>=(la->l1.ln[lc+2]-la->l1.ln[lc+1])-7;}ll lg(
la->ls.lp==l50){lg(la->l1.ln[lc+2]-la->l1.ln[lc+1]<=6)l57<<=6-(la->l1
.ln[lc+2]-la->l1.ln[lc+1]);ll l57>>=(la->l1.ln[lc+2]-la->l1.ln[lc+1])-
6;}ll{lj(l7)(-1);}l57&=~(l4^(l4-1));l4&=~(l4^(l4-1));lg(l57==l4){lj(1
);}ll{lj(0);}}lz l7 l273(lf lw*la,lf lv*le,lf ld lc,lf l7 l53){lg(la
->ls.lp==l29||la->ls.lp==l34){l16(l35(la,le,lc)){lm l103:lj(l222(la,
le,lc,l53,4));lm l105:lj(l156(la,le,lc,l53,4));lm l36:lj(l203(la,le,
lc,l53));lm l51:lj(l156(la,le,lc,l53,3));lm l67:lj(l156(la,le,lc,l53,
2));l33:lj(l7)(-1);}}ll lg(la->ls.lp==l37){l16(l35(la,le,lc)){lm l92:
lj(l222(la,le,lc,l53,3));lm l51:lj(l156(la,le,lc,l53,3));lm l36:lj(
l203(la,le,lc,l53));l33:lj(l7)(-1);}}ll lg(la->ls.lp==l50){l16(l35(la
,le,lc)){lm l67:lj(l156(la,le,lc,l53,2));lm l36:lj(l203(la,le,lc,l53));
l33:lj(l7)(-1);}}lj(l7)(-1);}lz ld l223(lf lw*la,lf ld l199,lf ld l13
/* Petra b code. Almost not in use. Ignore coverity defects */
/* coverity[overrun-buffer-val] */
,lf ld lc,lf l7 l136){lv le;l7 l113=l136;l109(la,lc,l113,&le,1);lg(
l153(la,&le,lc)){lj(l274(la,&le,lc));}lg(32+la->ls.l30-la->l1.ln[lc+2
]<32){l113=l13>>(32+la->ls.l30-la->l1.ln[lc+2]);l113&=(1<<(la->l1.ln[
lc+2]-la->l1.ln[lc+1]))-1;}ll{l113=0;}l113=l273(la,&le,lc,l113);l113
+=l90(la,&le,lc);lj(l223(la,l199,l13,lc+1,l113));}ld l304(lf lw*la,lf
ld l199,lf ld ly,lf ld l13){l7 l136;l45 lc;lc=-1;lg(32+la->ls.l30-la
->l1.ln[lc+2]<32){l136=l13>>(32+la->ls.l30-la->l1.ln[lc+2]);}ll{l136=
0;}l136|=ly<<(la->l1.ln[lc+2]-la->ls.l30);lj(l223(la,l199,l13,lc+1,
l136));}ld l340(l14 lw*la){l10*lb;l80*lo;l65 l169;l45 l17;ld l59,l139
;ld lh;l20(0);lb=&(la->ls);lo=&(la->l1);l0(l59=0;l59<lb->l230;++l59){
lb->l147[l59].l322=l339;lb->l147[l59].l356=l346;lh=l306(&(lb->l147[
l59]));lq(lh,10,lk);}l0(l17=0;(ld)l17<lb->l91;++l17){lh=l364(&(lb->
l195[l17]));lq(lh,20,lk);}lo->ln=(ld* )l359(l135(ld) * (lb->l227+1),""
);lg(lb->l143&l137){l0(l17=1;(ld)l17<lb->l91;++l17){lh=l329((lb->l126
[l17]));lq(lh,20,lk);}}lo->ln[0]=0;l0(l139=0;l139<lb->l227;l139++){lo
->ln[l139+1]=lo->ln[l139]+lb->l281[l139];}l0(l17=0;l17<(1<<(lo->ln[1]
-lo->ln[0]));){ld l167;lg((1<<(lo->ln[1]-lo->ln[0]))-l17<256)l167=(1
<<(lo->ln[1]-lo->ln[0]))-l17;ll l167=256;lh=l243(la,0,l229,l167,&l169
);lq(lh,30,lk);l17+=l167;}lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c"
"\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x63\x72\x65\x61\x74\x65\x28\x29",0,
0);}ld l317(l14 lw*la,lf ld ly,lf l170 l122){l83*l2;lv l154;l111 l12;
l282 l15;ld l27;ld l99;lt l6;lf l10*lb;lf l80*lo;ld lh;l20(0);lb=&(la
->ls);lo=&(la->l1);lh=lb->l81(lb->l31,lb->l46,ly,&l2);lq(lh,10,lk);lh
=l196(l2);lq(lh,15,lk);l12.l11=0;l12.l19=0;l15.l60=((((l201)&0x3)<<30
)|(((l179)&0x3)<<(30-2))|(((l122)&0xFFFFFFF)<<0));lh=l189(l2,&l12,&
l15,&l6);lq(lh,20,lk);l148(la,&l154,0,l122,0);l99=ly;l99<<=lo->ln[1]-
lb->l30;l0(l27=l99;l27<l99+(ld)(1<<(lo->ln[1]-lb->l30));++l27){lh=
l120(la,0,l27,&l154,1);lq(lh,100,lk);}lk:l28("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x76\x72\x66\x5f\x69\x6e"
"\x69\x74\x28\x29",0,0);}ld l351(l14 lw*la,lf ld ly,lf l170 l122,lf
l186 l79){l83*l2;lt l6;lf l10*lb;l108 l178;ld lh;l20(0);lb=&(la->ls);
lh=lb->l81(lb->l31,lb->l46,ly,&l2);lq(lh,10,lk);l311(&l178);l178.l98=
0;l178.l95=0;lh=l196(l2);lq(lh,15,lk);lh=lb->l321(lb->l31,0);lq(lh,15
,lk);l292(la,ly,&l178,l122,l79,l179,&l6);lk:l28("\x65\x72\x72\x6f\x72"
"\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76"
"\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x76\x72\x66\x5f\x63\x6c"
"\x65\x61\x72\x28\x29",0,0);}ld l326(l14 lw*la,lf ld l112,lf l170 l122
){l83*l2;lv l154;l111 l12;l282 l15;ld l27;ld l99,ly;lt l6;lf l10*lb;
lf l80*lo;ld lh;l20(0);lb=&(la->ls);lo=&(la->l1);l0(l27=0;l27<la->ls.
l91;++l27){lh=l296(&(lb->l195[l27]));lq(lh,40,lk);lg(l27>=1&&la->ls.
l143&l137){lh=l293((lb->l126[l27]));lq(lh,30,lk);}}l0(ly=0;ly<l112;++
ly){lh=lb->l81(lb->l31,lb->l46,ly,&l2);lq(lh,10,lk);lh=l196(l2);lq(lh
,15,lk);l12.l11=0;l12.l19=0;l15.l60=((((l201)&0x3)<<30)|(((l179)&0x3)<<
(30-2))|(((l122)&0xFFFFFFF)<<0));lh=l189(l2,&l12,&l15,&l6);lq(lh,20,
lk);l148(la,&l154,0,l122,0);l99=ly;l99<<=lo->ln[1]-lb->l30;l0(l27=l99
;l27<l99+(ld)(1<<(lo->ln[1]-lb->l30));++l27){lh=l120(la,0,l27,&l154,1
);lq(lh,100,lk);}}lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73"
"\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x61\x6c\x6c\x5f\x76\x72\x66\x73\x5f\x63\x6c"
"\x65\x61\x72\x28\x29",0,0);}ld l347(l14 lw*la){l10*lb;l80*lo;ld l17,
l59;ld lh;l20(0);lb=&(la->ls);lo=&(la->l1);l0(l59=0;l59<lb->l230;++
l59){lh=l367(&(lb->l147[l59]));lq(lh,10,lk);}l0(l17=0;l17<lb->l91;++
l17){lh=l330(&(lb->l195[l17]));lq(lh,20,lk);}lg(lb->l143&l137){l0(l17
=1;l17<lb->l91;++l17){lh=l360(lb->l126[l17]);lq(lh,260,lk);}}lg(lb->
l143&l137){l0(l17=1;l17<lb->l91;++l17){l239(lb->l126[l17]);}l239(lb->
l126);}l239(lo->ln);lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73"
"\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x64\x65\x73\x74\x72\x6f\x79\x28\x29",0,0);}
lz ld l262(l14 l83*l2,lf l361 l12,lf lt l11,l14 ld*l15,lf lt l280,l14
l56*l267){l192*l52=(l192* )l267;l186 l79;l204 l86;ld l161;l20(0);lg(
l280){l161=((( *l15)>>0)&0xFFFFFFF);l79=((( *l15)>>30)&0x3);l86=((( *
/* Petra b code. Almost not in use. Ignore coverity defects */
/* coverity[dead_error_begin] */
l15)>>(30-2))&0x3);lg(l86==l179){lg(l79==l353){l52->l94[l52->l155].
l11=l11;l52->l94[l52->l155].l13=l12;l52->l155++;}ll lg(l79==l251){ *
l15=((((l201)&0x3)<<30)|(((l86)&0x3)<<(30-2))|(((l161)&0xFFFFFFF)<<0));
}}}l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62"
"\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72"
"\x5f\x70\x61\x74\x5f\x66\x6f\x72\x65\x61\x63\x68\x5f\x66\x6e\x5f\x68"
"\x77\x5f\x73\x79\x6e\x63\x28\x29",0,0);}ld l350(l14 lw*la,lf ld ly){
ld l13;lt l11;ld lh;ld l3;l192 l52;l111 l21;l83*l2;l10*lb;l20(0);l52.
l94=l158;lb=&(la->ls);lh=lb->l81(lb->l31,lb->l46,ly,&l2);lq(lh,20,lk);
l365(l52.l94,l135( *l52.l94) *lb->l258,"");l193(l52.l94,0,l135( *l52.
l94) *lb->l258);l52.l155=0;lh=l308(l2,l262,&l52);lq(lh,25,lk);l0(l3=0
;l3<l52.l155;l3++){l21.l11=l52.l94[l3].l11;l21.l19=l52.l94[l3].l13;
l250(l2,&l21);}l13=0;l11=0;lh=l183(la,ly,l13,l11,l66,l158);lq(lh,30,
lk);lk:l323(l52.l94);l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73"
"\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d"
"\x5f\x6d\x6e\x67\x72\x5f\x68\x77\x5f\x73\x79\x6e\x63\x28\x29",0,0);}
ld l352(l14 lw*la,lf ld ly,lf l108*l12,l186 l79){l83*l2;l10*lb;l111
l21;l124 l32;ld l13,l15;lt l11;ld lh;lt l64,l6;l20(0);lb=&(la->ls);
l13=l12->l98;l11=l12->l95;l21.l11=l11;l21.l19=l13;lh=lb->l81(lb->l31,
lb->l46,ly,&l2);lq(lh,10,lk);lh=l173(l2,&l21,l26,&l32,&l64);lq(lh,20,
lk);lg(!l64){l125 lk;}l15=((((l79)&0x3)<<30)|((((((l32.l15.l60)>>(30-
2))&0x3))&0x3)<<(30-2))|((((((l32.l15.l60)>>0)&0xFFFFFFF))&0xFFFFFFF)<<
0));l32.l15.l60=l15;lh=l189(l2,&l21,&l32.l15,&l6);lq(lh,10,lk);lk:l28
("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f"
"\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f"
"\x6d\x61\x72\x6b\x5f\x6e\x6f\x6e\x5f\x6c\x70\x6d\x5f\x72\x6f\x75\x74"
"\x65\x5f\x70\x65\x6e\x64\x69\x6e\x67\x5f\x74\x79\x70\x65\x5f\x73\x65"
"\x74\x28\x29",0,0);}ld l292(l14 lw*la,lf ld ly,lf l108*l12,lf l170
l205,lf lt l133,lf l204 l86,l8 lt*l6){l83*l2;l10*lb;l111 l21;l124 l32
;ld l13,l15;lt l11;ld lh;lt l106=l26;l236 l145=l182;l20(0);lb=&(la->
ls);l13=l12->l98;l11=l12->l95;lg(!l133){lh=l245(la,0,l6);lq(lh,10,lk);
lg(! *l6){l106=l66;}}lg(l106){lh=l176(la,ly,1,l66);lq(lh,10,lk);}l21.
l11=l11;l21.l19=l13;lh=lb->l81(lb->l31,lb->l46,ly,&l2);lq(lh,10,lk);
lq(lh,20,lk);l15=(((((l133==l66)?l251:l201)&0x3)<<30)|(((l86)&0x3)<<(
30-2))|(((l205)&0xFFFFFFF)<<0));l32.l15.l60=l15;lh=l189(l2,&l21,&l32.
l15,l6);lq(lh,10,lk);lg(! *l6){l125 lk;}lg((!l133)&&((l86==l179))){lh
=l183(la,ly,l13,l11,l26,l6);lq(lh,20,lk);}lg(l106&& *l6){lh=l214(la,
ly,l158,1,l26,&l145);lq(lh,10,lk);lg(l145!=l182){ *l6=l26;}ll{ *l6=
l66;}}lg(l106){lh=l176(la,ly,1,l26);lq(lh,10,lk);}lk:l28("\x65\x72"
"\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f"
"\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65"
"\x66\x69\x78\x5f\x61\x64\x64\x28\x29",0,0);}ld l214(l14 lw*la,lf ld
ly,lf ld*l221,lf ld l112,lf lt l289,l8 l236*l6){ld l63,l140;lt l149=
l26;ld l13,l129;lt l11;ld l47;l144*l114;ld lh;lf l10*lb;l20(0);lb=&(
la->ls);l13=0;l11=0;lg(l289){l0(l63=0;l63<l112;++l63){lg(l112==1){
l140=ly;}ll{l129=0;lh=l255(l221,l63,1,&l129);lg(!l129){l279;}l140=l63
;}lh=l183(la,l140,l13,l11,l66,&l149);lq(lh,30,lk);lg(!l149){l18;}}lg(
!l149){ *l6=l333;l125 lk;}}ll{l149=l66;}l0(l47=0;l47<lb->l91;++l47){
lh=lb->l128(lb->l31,lb->l46,lb->l91-l47-1,&l114);lq(lh,30,lk);lg(l149
){lh=l328(l114,0);lq(lh,30,lk);lg(l47>=1&&la->ls.l143&l137){lh=l343((
lb->l126[l47]),0);lq(lh,40,lk);} *l6=l182;}}l0(l63=0;l63<l112;++l63){
lg(l112==1){l140=ly;}ll{l129=0;lh=l255(l221,l63,1,&l129);lg(!l129){
l279;}l140=l63;}lh=l342(&(la->ls.l147[l140]),0);lq(lh,30,lk);}lk:l28(""
"\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70"
"\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70"
"\x72\x65\x66\x69\x78\x5f\x61\x64\x64\x28\x29",0,0);}ld l176(l14 lw*
la,lf ld ly,lf ld l112,lf lt l177){ld l47,l63;l144*l114;l83*l2;ld lh;
lf l10*lb;l20(0);lb=&(la->ls);l0(l47=0;l47<lb->l91;++l47){lh=lb->l128
(lb->l31,lb->l46,l47,&l114);lq(lh,10,lk);lh=l320(l114,l177);lq(lh,10,
lk);lg(la->ls.l143&l137&&l47>=1){lh=l348((lb->l126[l47]),l177);lq(lh,
20,lk);}}lg(ly==l366){l0(l63=0;l63<l112;++l63){lh=lb->l81(lb->l31,lb
->l46,l63,&l2);lq(lh,20,lk);lh=l300(l2,l177);lq(lh,30,lk);}}ll{lh=lb
->l81(lb->l31,lb->l46,ly,&l2);lq(lh,40,lk);lh=l300(l2,l177);lq(lh,50,
lk);}lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70"
"\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67"
"\x72\x5f\x63\x61\x63\x68\x65\x5f\x73\x65\x74\x28\x29",0,0);}ld l332(
l14 lw*la,lf ld ly,lf l108*l12,lf lt l133,l8 lt*l6){l83*l2;l10*lb;
l111 l21;l124 l32;ld l13;lt l11;ld lh;lt l64,l106=l26;l236 l145=l182;
l20(0);l13=l12->l98;l11=l12->l95;lg(!l133){lh=l245(la,1,l6);lq(lh,10,
lk);lg(! *l6){lg(! *l6){l106=l66;}}}lg(l106){lh=l176(la,ly,1,l66);lq(
lh,10,lk);}lb=&(la->ls);lh=lb->l81(lb->l31,lb->l46,ly,&l2);lq(lh,10,
lk);l21.l11=l11;l21.l19=l13;lh=l173(l2,&l21,l26,&l32,&l64);lq(lh,20,
lk);lg(!l64){l125 lk;}{lh=l250(l2,&l21);lq(lh,10,lk);lg(!l133){lh=
l183(la,ly,l13,l11,l26,l6);lq(lh,20,lk);}lg(l106&& *l6){lh=l214(la,ly
,l158,1,l26,&l145);lq(lh,10,lk);lg(l145!=l182){ *l6=l26;}ll{ *l6=l66;
}}lg(l106){lh=l176(la,ly,1,l26);lq(lh,10,lk);}}lk:l28("\x65\x72\x72"
"\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f\x69"
"\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72\x65\x66"
"\x69\x78\x5f\x72\x65\x6d\x6f\x76\x65\x28\x29",0,0);}ld l276(l14 lw*
la,lf ld ly,lf l108*l12,l8 l108*l141,l8 lt*l110,l8 lt*l64){l83*l2;l10
 *lb;l111 l21;l124 l32;ld lh;l20(0);lb=&(la->ls);lh=lb->l81(lb->l31,
lb->l46,ly,&l2);lq(lh,10,lk);l21.l19=l12->l98;l21.l11=l12->l95;lh=
l173(l2,&l21,l26,&l32,l64);lq(lh,20,lk);lg(!( *l64)){ *l110=l26;l125
lk;}l141->l98=l32.l12.l19;l141->l95=l32.l12.l11;lg((l141->l98==l12->
l98)&&(l141->l95==l12->l95)){ *l110=l66;}ll{ *l110=l26;}lk:l28("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70"
"\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x70\x72"
"\x65\x66\x69\x78\x5f\x6c\x6f\x6f\x6b\x75\x70\x28\x29",0,0);}ld l357(
l14 lw*la,lf ld ly,lf l108*l12,l8 lt*l288){l108 l141;lt l270;lt l110;
ld lh;l20(0);lh=l276(la,ly,l12,&l141,&l110,&l270);lq(lh,10,lk); *l288
=l110;lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f"
"\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e"
"\x67\x72\x5f\x70\x72\x65\x66\x69\x78\x5f\x69\x73\x5f\x65\x78\x69\x73"
"\x74\x28\x29",0,0);}ld l309(l14 lw*la,lf ld ly,lf l108*l12,lf lt l110
,l8 l170*l205,l8 l186*l79,l8 l204*l86,l8 lt*l64){l83*l2;l10*lb;l111
l21;l124 l32;lt l242;ld lh;l20(0);lb=&(la->ls);lh=lb->l81(lb->l31,lb
->l46,ly,&l2);lq(lh,10,lk);l21.l19=l12->l98;l21.l11=l12->l95;lh=l173(
l2,&l21,l26,&l32,&l242);lq(lh,20,lk);lg(!l242){ *l64=l26;l125 lk;}lg(
l110&&((l32.l12.l19!=l12->l98)||(l32.l12.l11!=l12->l95))){ *l64=l26;
l125 lk;} *l64=l66; *l205=(((l32.l15.l60)>>0)&0xFFFFFFF);lg(l79){ *
l79=(((l32.l15.l60)>>30)&0x3);}lg(l86){ *l86=(((l32.l15.l60)>>(30-2))&
0x3);}lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f"
"\x70\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e"
"\x67\x72\x5f\x73\x79\x73\x5f\x66\x65\x63\x5f\x67\x65\x74\x28\x29",0,
0);}ld l336(l14 lw*la,lf ld ly,l8 l215*l181){l83*l2;l10*lb;ld lh;l20(
0);l234(l181);lb=&(la->ls);lh=lb->l81(lb->l31,lb->l46,ly,&l2);lq(lh,
10,lk);lh=l355(l2,&(l181->l261),&(l181->l297),&(l181->l283));lq(lh,20
,lk);lk:l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70"
"\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67"
"\x72\x5f\x67\x65\x74\x5f\x73\x74\x61\x74\x28\x29",0,0);}ld l315(l14
lw*la){l10*lb;ld l17,l59;ld lh;l20(0);lb=&(la->ls);l0(l59=0;l59<lb->
l230;++l59){lh=l196(&(lb->l147[l59]));lq(lh,10,lk);}l0(l17=0;l17<lb->
l91;++l17){lh=l296(&(lb->l195[l17]));lq(lh,20,lk);lg(l17>=1&&la->ls.
l143&l137){lh=l293((lb->l126[l17]));lq(lh,30,lk);}}lk:l28("\x65\x72"
"\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x62\x5f\x70\x70\x5f"
"\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67\x72\x5f\x63\x6c\x65"
"\x61\x72\x28\x29",0,0);}ld l327(l14 lw*la,lf ld ly,lf l354 l286,l14
l130*l240,lf ld l253,lf ld l257,l8 l108*l217,l8 ld*l260,l8 l186*l79,
l8 l204*l86,l8 ld*l191){l124*l116=l158;ld l175,l160,l211,l123,l27;l10
 *lb;l83*l2;ld lh;l20(0);lb=&(la->ls);lh=lb->l81(lb->l31,lb->l46,ly,&
l2);lq(lh,10,lk);l160=l257;l27=0;l211=0;l344(l116,l124,100);l345(!
l314((l240))&&l160){lg(l160>100){l175=100;}ll{l175=l160;}l160-=l175;
lh=l349(l2,l286,l240,l253,l175,l116,l191);lq(lh,20,lk);l211+= *l191;
l0(l123=0;l27<l211;++l27,++l123){l217[l27].l98=l116[l123].l12.l19;
l217[l27].l95=l116[l123].l12.l11;l260[l27]=(((l116[l123].l15.l60)>>0)&
0xFFFFFFF);lg(l79){l79[l27]=(((l116[l123].l15.l60)>>30)&0x3);}lg(l86){
l86[l27]=(((l116[l123].l15.l60)>>(30-2))&0x3);}}} *l191=l27;lk:l305(
l116);l28("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70"
"\x62\x5f\x70\x70\x5f\x69\x70\x76\x34\x5f\x6c\x70\x6d\x5f\x6d\x6e\x67"
"\x72\x5f\x67\x65\x74\x5f\x62\x6c\x6f\x63\x6b\x28\x29",0,0);}l56 l313
(l14 lw*l100){l20(0);l234(l100);l193(l100,0x0,l135(lw));l100->l1.ln=
l158;l303;lk:l301(0,0,0);}l56 l319(l14 l215*l100){l20(0);l234(l100);
l193(l100,0x0,l135(l215));l100->l283=0;l100->l297=0;l100->l261=0;l303
;lk:l301(0,0,0);}
#undef l164
#undef l45
#undef l312
#undef ld
#undef l7
#undef l316
#undef l278
#undef lz
#undef l264
#undef lt
#undef l11
#undef l56
#undef l8
#undef lv
#undef lf
#undef l318
#undef li
#undef l135
#undef lw
#undef l10
#undef ls
#undef lg
#undef lp
#undef lj
#undef ll
#undef l130
#undef l19
#undef l14
#undef l37
#undef l29
#undef l34
#undef l117
#undef l88
#undef l50
#undef l294
#undef l272
#undef l287
#undef l60
#undef l310
#undef l244
#undef l15
#undef l0
#undef l144
#undef lh
#undef l20
#undef l128
#undef l31
#undef l46
#undef lq
#undef lk
#undef l219
#undef l237
#undef l193
#undef l290
#undef l28
#undef l16
#undef lm
#undef l33
#undef l26
#undef l90
#undef l358
#undef l18
#undef l1
#undef ln
#undef l80
#undef l338
#undef l341
#undef l265
#undef l91
#undef l220
#undef l281
#undef l325
#undef l125
#undef l256
#undef l275
#undef l331
#undef l335
#undef l227
#undef l363
#undef l334
#undef l111
#undef l124
#undef l83
#undef l2
#undef l337
#undef l30
#undef l81
#undef l173
#undef l66
#undef l307
#undef l194
#undef l362
#undef l304
#undef l340
#undef l230
#undef l147
#undef l322
#undef l339
#undef l356
#undef l346
#undef l306
#undef l364
#undef l195
#undef l359
#undef l143
#undef l137
#undef l329
#undef l126
#undef l317
#undef l170
#undef l12
#undef l282
#undef l196
#undef l201
#undef l179
#undef l189
#undef l351
#undef l186
#undef l108
#undef l311
#undef l98
#undef l95
#undef l321
#undef l292
#undef l326
#undef l296
#undef l293
#undef l347
#undef l367
#undef l330
#undef l360
#undef l239
#undef l262
#undef l361
#undef l280
#undef l204
#undef l353
#undef l251
#undef l350
#undef l158
#undef l365
#undef l258
#undef l308
#undef l250
#undef l323
#undef l352
#undef l205
#undef l236
#undef l182
#undef l176
#undef l214
#undef l255
#undef l279
#undef l333
#undef l328
#undef l343
#undef l342
#undef l320
#undef l348
#undef l366
#undef l300
#undef l332
#undef l276
#undef l357
#undef l309
#undef l336
#undef l215
#undef l234
#undef l355
#undef l261
#undef l297
#undef l283
#undef l315
#undef l327
#undef l354
#undef l344
#undef l345
#undef l314
#undef l349
#undef l305
#undef l313
#undef l100
#undef l303
#undef l301
#undef l319
#include<soc/dpp/SAND/Utils/sand_footer.h>
