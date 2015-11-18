/*
 * $Id: alpm_128.c,v 1.13 Broadcom SDK $
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
 * File:    alpm_128.c
 * Purpose: Tomahawk Primitives for LPM management in ALPM - Mode for IPv6-128.
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#ifdef ALPM_ENABLE

#include <shared/util.h>
#include <shared/l3.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#include <soc/esw/trie.h>

#if 1
void soc_th_alpm_128_lpm_state_dump(int l1);extern int l2(int l3);static int
l4(int l1,void*l5,int*index);static int l6(int l1,void*l7);static int l8(int
l1,void*l7,int l9,int l10,int l11,defip_aux_scratch_entry_t*l12);static int
l13(int l1,void*l7,void*l14,int*l15,int*l16,int*l9);static int l17(int l1,
void*l7,void*l14,int*l15);static int l18(int l3,void*lpm_entry,void*l19,void*
l20,soc_mem_t l21,uint32 l22,uint32*l23);static int l24(int l3,void*l19,
soc_mem_t l21,int l9,int l25,int l26,int index,void*lpm_entry);static int l27
(int l3,uint32*key,int len,int l25,int l9,defip_pair_128_entry_t*lpm_entry,
int l28,int l29);static int l30(int l1,int l25,int l31);static int l32(l33*
l34,trie_t*l35,uint32*l36,uint32 l37,trie_node_t*l38,defip_pair_128_entry_t*
lpm_entry);static int l39(l33*l34,int*l40,int*l15);int l41(int l1,trie_t*l35,
trie_t*l42,payload_t*l43,int*l44,int bucket_index,int l31);int
_soc_th_alpm_rollback_bkt_move(int l1,void*l7,soc_mem_t l21,alpm_pivot_t*l45,
alpm_pivot_t*l46,alpm_mem_prefix_array_t*l47,int*l44,int l48);void l49(int l1
,defip_pair_128_entry_t*l50,void*l7,int l51,alpm_pivot_t*l52);typedef struct
l53{int l54;int l55;int l56;int next;int l57;int l58;}l59,*l60;static l60 l61
[SOC_MAX_NUM_DEVICES];typedef struct l62{soc_field_info_t*l63;
soc_field_info_t*l64;soc_field_info_t*l65;soc_field_info_t*l66;
soc_field_info_t*l67;soc_field_info_t*l68;soc_field_info_t*l69;
soc_field_info_t*l70;soc_field_info_t*l71;soc_field_info_t*l72;
soc_field_info_t*l73;soc_field_info_t*l74;soc_field_info_t*l75;
soc_field_info_t*l76;soc_field_info_t*l77;soc_field_info_t*l78;
soc_field_info_t*l79;soc_field_info_t*l80;soc_field_info_t*l81;
soc_field_info_t*l82;soc_field_info_t*l83;soc_field_info_t*l84;
soc_field_info_t*l85;soc_field_info_t*l86;soc_field_info_t*l87;
soc_field_info_t*l88;soc_field_info_t*l89;soc_field_info_t*l90;
soc_field_info_t*l91;soc_field_info_t*l92;soc_field_info_t*l93;
soc_field_info_t*l94;soc_field_info_t*l95;soc_field_info_t*l96;
soc_field_info_t*l97;soc_field_info_t*l98;soc_field_info_t*l99;
soc_field_info_t*l100;soc_field_info_t*l101;soc_field_info_t*l102;
soc_field_info_t*l103;soc_field_info_t*l104;soc_field_info_t*l105;
soc_field_info_t*l106;}l107,*l108;static l108 l109[SOC_MAX_NUM_DEVICES];
typedef struct l110{int l3;int l111;int l112;uint16*l113;uint16*l114;}l115;
typedef uint32 l116[9];typedef int(*l117)(l116 l118,l116 l119);static l115*
l120[SOC_MAX_NUM_DEVICES];static void l121(int l1,void*l14,int index,l116 l122
);static uint16 l123(uint8*l124,int l125);static int l126(int l3,int l111,int
l112,l115**l127);static int l128(l115*l129);static int l130(l115*l131,l117
l132,l116 entry,int l133,uint16*l134);static int l135(l115*l131,l117 l132,
l116 entry,int l133,uint16 l136,uint16 l44);static int l137(l115*l131,l117
l132,l116 entry,int l133,uint16 l138);static int l139(int l1,const void*entry
,int*l133){int l140,l141;int l142[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf
,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};uint32 l143;l143 = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l142[0]);if((l141 = 
_ipmask2pfx(l143,l133))<0){return(l141);}for(l140 = 1;l140<4;l140++){l143 = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l142[l140]);if(*l133){if(l143
!= 0xffffffff){return(SOC_E_PARAM);}*l133+= 32;}else{if((l141 = _ipmask2pfx(
l143,l133))<0){return(l141);}}}return SOC_E_NONE;}static void l144(uint32*
l145,int l37,int l31){uint32 l146,l147,l54,prefix[5];int l140;sal_memcpy(
prefix,l145,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(l145,0,
sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));l146 = 128-l37;l54 = (l146+31)/
32;if((l146%32) == 0){l54++;}l146 = l146%32;for(l140 = l54;l140<= 4;l140++){
prefix[l140]<<= l146;l147 = prefix[l140+1]&~(0xffffffff>>l146);l147 = (((32-
l146) == 32)?0:(l147)>>(32-l146));if(l140<4){prefix[l140]|= l147;}}for(l140 = 
l54;l140<= 4;l140++){l145[3-(l140-l54)] = prefix[l140];}}static void l148(int
l3,void*lpm_entry,int l16){int l140;soc_field_t l149[4] = {IP_ADDR_MASK0_LWRf
,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};for(l140 = 0;l140<
4;l140++){soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,l149[l140],0);}
for(l140 = 0;l140<4;l140++){if(l16<= 32)break;soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,l149[3-l140],0xffffffff);l16-= 32;}
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,l149[3-l140],~(((l16) == 
32)?0:(0xffffffff)>>(l16)));}static int l150(int l1,void*entry,uint32*l145,
uint32*l16,int*l23){int l140;int l133 = 0,l54;int l141 = SOC_E_NONE;uint32
l146,l147;uint32 prefix[5];sal_memset(l145,0,sizeof(uint32)*BITS2WORDS(
_MAX_KEY_LEN_144_));sal_memset(prefix,0,sizeof(uint32)*BITS2WORDS(
_MAX_KEY_LEN_144_));prefix[0] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR0_LWRf);prefix[1] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR1_LWRf);prefix[2] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR0_UPRf);prefix[3] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR1_UPRf);if(l23!= NULL){*l23 = (prefix[0] == 0)&&(prefix[1] == 0)
&&(prefix[2] == 0)&&(prefix[3] == 0)&&(l133 == 0);}l141 = l139(l1,entry,&l133
);if(SOC_FAILURE(l141)){return l141;}l146 = 128-l133;l54 = l146/32;l146 = 
l146%32;for(l140 = l54;l140<4;l140++){prefix[l140]>>= l146;l147 = prefix[l140
+1]&((1<<l146)-1);l147 = (((32-l146) == 32)?0:(l147)<<(32-l146));prefix[l140]
|= l147;}for(l140 = l54;l140<4;l140++){l145[4-(l140-l54)] = prefix[l140];}*
l16 = l133;return SOC_E_NONE;}int l151(int l1,uint32*prefix,uint32 l37,int l9
,int l25,int*l152,int*l51,int*bucket_index){int l141 = SOC_E_NONE;trie_t*l153
;trie_node_t*l154 = NULL;alpm_pivot_t*l52;l153 = VRF_PIVOT_TRIE_IPV6_128(l1,
l25);l141 = trie_find_lpm(l153,prefix,l37,&l154);if(SOC_FAILURE(l141)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Pivot find failed\n")));return l141
;}l52 = (alpm_pivot_t*)l154;*l152 = 1;*l51 = PIVOT_TCAM_INDEX(l52);*
bucket_index = PIVOT_BUCKET_INDEX(l52);return SOC_E_NONE;}static int l155(int
l1,void*l7,soc_mem_t l21,void*l156,int*l51,int*bucket_index,int*l15){uint32
l14[SOC_MAX_MEM_FIELD_WORDS];int l157,l25,l31;int l134;uint32 l10,l158;int
l141 = SOC_E_NONE;int l152 = 0;l31 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(l1,l7,&l157,&l25));if(l157 == 0){if(
soc_th_alpm_mode_get(l1)){return SOC_E_PARAM;}}if(l25 == SOC_VRF_MAX(l1)+1){
l10 = 0;if(l2(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l158)
;}else{SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l158);}}else{l10 = 2;if(l2(l1)<= 2
){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,l158);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l158);}}if(l157!= SOC_L3_VRF_OVERRIDE){
uint32 prefix[5],l37;int l23 = 0;l141 = l150(l1,l7,prefix,&l37,&l23);if(
SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l141;}l141 = l151(l1,
prefix,l37,l31,l25,&l152,l51,bucket_index);SOC_IF_ERROR_RETURN(l141);if(l152)
{l18(l1,l7,l14,0,l21,0,0);l141 = _soc_th_alpm_find_in_bkt(l1,l21,*
bucket_index,l158,l14,l156,&l134,l31);if(SOC_SUCCESS(l141)){*l15 = l134;}}
else{l141 = SOC_E_NOT_FOUND;}}return l141;}static int l159(int l1,void*l7,
void*l156,void*l160,soc_mem_t l21,int l134){defip_aux_scratch_entry_t l12;int
l157,l31,l25;int bucket_index;uint32 l10,l158;int l141 = SOC_E_NONE;int l152 = 
0,l147 = 0;int l51;l31 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(l1,l7,&l157,&l25));if(l25 == SOC_VRF_MAX(l1)+1){
l10 = 0;if(l2(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l158)
;}else{SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l158);}}else{l10 = 2;if(l2(l1)<= 2
){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,l158);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l158);}}if(!soc_th_alpm_mode_get(l1)){l10 = 
2;}if(l157!= SOC_L3_VRF_OVERRIDE){sal_memset(&l12,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(l1,l7,l31,l10,0,&l12));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l21,MEM_BLOCK_ANY,l134,l156));if(l141!= 
SOC_E_NONE){return SOC_E_FAIL;}if(SOC_URPF_STATUS_GET(l1)){
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l21,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry
(l1,l134),l160));if(l141!= SOC_E_NONE){return SOC_E_FAIL;}}l147 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l12,IP_LENGTHf);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,REPLACE_LENf,l147);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l152,&
l51,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l147 = soc_mem_field32_get(l1
,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf);l147+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l147);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l152,&l51,&bucket_index));
}}return l141;}static int l161(int l1,int l40,int l162){int l141,l147,l163,
l164;defip_aux_table_entry_t l165,l166;l163 = SOC_ALPM_128_ADDR_LWR(l40);l164
= SOC_ALPM_128_ADDR_UPR(l40);l141 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l163,&l165);SOC_IF_ERROR_RETURN(l141);l141 = soc_mem_read(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l164,&l166);SOC_IF_ERROR_RETURN(l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l165,BPM_LENGTH0f,l162);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l165,BPM_LENGTH1f,l162);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,BPM_LENGTH0f,l162);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,BPM_LENGTH1f,l162);l147 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&l165,DB_TYPE0f);l141 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l163,&l165);
SOC_IF_ERROR_RETURN(l141);l141 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l164,&l166);SOC_IF_ERROR_RETURN(l141);if(SOC_URPF_STATUS_GET(l1
)){l147++;soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l165,BPM_LENGTH0f,l162)
;soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l165,BPM_LENGTH1f,l162);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,BPM_LENGTH0f,l162);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,BPM_LENGTH1f,l162);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l165,DB_TYPE0f,l147);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l165,DB_TYPE1f,l147);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,DB_TYPE0f,l147);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,DB_TYPE1f,l147);l163+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l164+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(
l1,L3_DEFIPm))/2;l141 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
l163,&l165);SOC_IF_ERROR_RETURN(l141);l141 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l164,&l166);}return l141;}static int l167(
int l1,int l168,void*entry,defip_aux_table_entry_t*l169,int l170){uint32 l147
,l10,l171 = 0;soc_mem_t l21 = L3_DEFIP_PAIR_128m;soc_mem_t l172 = 
L3_DEFIP_AUX_TABLEm;int l141 = SOC_E_NONE,l133,l25;void*l173,*l174;l173 = (
void*)l169;l174 = (void*)(l169+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l172,
MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l168),l169));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l172,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l168),l169+1));l147 = 
soc_mem_field32_get(l1,l21,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l172,
l173,VRF0f,l147);l147 = soc_mem_field32_get(l1,l21,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l172,l173,VRF1f,l147);l147 = soc_mem_field32_get(l1,
l21,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l172,l174,VRF0f,l147);l147 = 
soc_mem_field32_get(l1,l21,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l172,
l174,VRF1f,l147);l147 = soc_mem_field32_get(l1,l21,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l172,l173,MODE0f,l147);l147 = soc_mem_field32_get(l1,
l21,entry,MODE1_LWRf);soc_mem_field32_set(l1,l172,l173,MODE1f,l147);l147 = 
soc_mem_field32_get(l1,l21,entry,MODE0_UPRf);soc_mem_field32_set(l1,l172,l174
,MODE0f,l147);l147 = soc_mem_field32_get(l1,l21,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l172,l174,MODE1f,l147);l147 = soc_mem_field32_get(l1,
l21,entry,VALID0_LWRf);soc_mem_field32_set(l1,l172,l173,VALID0f,l147);l147 = 
soc_mem_field32_get(l1,l21,entry,VALID1_LWRf);soc_mem_field32_set(l1,l172,
l173,VALID1f,l147);l147 = soc_mem_field32_get(l1,l21,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l172,l174,VALID0f,l147);l147 = soc_mem_field32_get(l1,
l21,entry,VALID1_UPRf);soc_mem_field32_set(l1,l172,l174,VALID1f,l147);l141 = 
l139(l1,entry,&l133);SOC_IF_ERROR_RETURN(l141);soc_mem_field32_set(l1,l172,
l173,IP_LENGTH0f,l133);soc_mem_field32_set(l1,l172,l173,IP_LENGTH1f,l133);
soc_mem_field32_set(l1,l172,l174,IP_LENGTH0f,l133);soc_mem_field32_set(l1,
l172,l174,IP_LENGTH1f,l133);l147 = soc_mem_field32_get(l1,l21,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l172,l173,IP_ADDR0f,l147);l147 = 
soc_mem_field32_get(l1,l21,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l172,
l173,IP_ADDR1f,l147);l147 = soc_mem_field32_get(l1,l21,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l172,l174,IP_ADDR0f,l147);l147 = soc_mem_field32_get(
l1,l21,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l172,l174,IP_ADDR1f,l147);
l147 = soc_mem_field32_get(l1,l21,entry,ENTRY_TYPE0_LWRf);soc_mem_field32_set
(l1,l172,l173,ENTRY_TYPE0f,l147);l147 = soc_mem_field32_get(l1,l21,entry,
ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,l172,l173,ENTRY_TYPE1f,l147);l147 = 
soc_mem_field32_get(l1,l21,entry,ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,
l172,l174,ENTRY_TYPE0f,l147);l147 = soc_mem_field32_get(l1,l21,entry,
ENTRY_TYPE1_UPRf);soc_mem_field32_set(l1,l172,l174,ENTRY_TYPE1f,l147);l141 = 
soc_th_alpm_128_lpm_vrf_get(l1,entry,&l25,&l133);SOC_IF_ERROR_RETURN(l141);if
(SOC_URPF_STATUS_GET(l1)){if(l170>= (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1)){l171 = 1;}}switch(l25){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l172,l173,VALID0f,0);soc_mem_field32_set(l1,l172,l173,
VALID1f,0);soc_mem_field32_set(l1,l172,l174,VALID0f,0);soc_mem_field32_set(l1
,l172,l174,VALID1f,0);l10 = 0;break;case SOC_L3_VRF_GLOBAL:l10 = l171?1:0;
break;default:l10 = l171?3:2;break;}soc_mem_field32_set(l1,l172,l173,
DB_TYPE0f,l10);soc_mem_field32_set(l1,l172,l173,DB_TYPE1f,l10);
soc_mem_field32_set(l1,l172,l174,DB_TYPE0f,l10);soc_mem_field32_set(l1,l172,
l174,DB_TYPE1f,l10);if(l171){l147 = soc_mem_field32_get(l1,l21,entry,
ALG_BKT_PTRf);if(l147){l147+= SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(
l1,l21,entry,ALG_BKT_PTRf,l147);}}return SOC_E_NONE;}static int l175(int l1,
int l176,int index,int l177,void*entry){defip_aux_table_entry_t l169[2];l177 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l177,1);SOC_IF_ERROR_RETURN(
l167(l1,l177,entry,&l169[0],index));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,entry));index = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(index),l169)
);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_UPR(index),l169+1));return SOC_E_NONE;}static int l32(l33*
l34,trie_t*l35,uint32*l36,uint32 l37,trie_node_t*l38,defip_pair_128_entry_t*
lpm_entry){trie_node_t*l154 = NULL;int l31,l25,l157;uint32 l162 = 0;
defip_alpm_ipv6_64_entry_t l178,l179;payload_t*l180 = NULL;int l181;void*l182
,*l183;trie_t*l153 = NULL;int l141 = SOC_E_NONE;soc_mem_t l21;
alpm_bucket_handle_t*l184;l181 = (l34->l52)->l51;l31 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l34->l1,l34->l7,&l157,&l25));l21 = 
L3_DEFIP_ALPM_IPV6_128m;l182 = ((uint32*)&(l178));l183 = ((uint32*)&(l179));
l154 = NULL;l141 = trie_find_lpm(l35,l36,l37,&l154);l180 = (payload_t*)l154;
if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l34->l1,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l34->l1,l36[0],l36[1],
l36[2],l36[3],l36[4],l37));return l141;}if(l180->bkt_ptr){if(l180->bkt_ptr == 
l34->l43){sal_memcpy(l182,l34->l156,sizeof(defip_alpm_ipv6_128_entry_t));}
else{l141 = soc_mem_read(l34->l1,l21,MEM_BLOCK_ANY,((payload_t*)l180->bkt_ptr
)->index,l182);}if(SOC_FAILURE(l141)){return l141;}l141 = l24(l34->l1,l182,
l21,l31,l157,l34->bucket_index,0,&lpm_entry);if(SOC_FAILURE(l141)){return l141
;}l162 = ((payload_t*)(l180->bkt_ptr))->len;}else{l141 = soc_mem_read(l34->l1
,L3_DEFIPm,MEM_BLOCK_ANY,soc_th_alpm_logical_idx(l34->l1,L3_DEFIPm,l181>>1,1)
,&lpm_entry);if((!l31)&&(l181&1)){l141 = soc_th_alpm_lpm_ip4entry1_to_0(l34->
l1,&lpm_entry,&lpm_entry,0);}}l184 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l184 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l34->l1,"_soc_alpm_insert: Unable to allocate memory "
"for PIVOT trie node \n")));l141 = SOC_E_MEMORY;return l141;}sal_memset(l184,
0,sizeof(*l184));l34->l52 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for new Pivot");if(l34->l52 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l34->l1,"_soc_alpm_insert: Unable to allocate memory "
"for PIVOT trie node \n")));l141 = SOC_E_MEMORY;return l141;}sal_memset(l34->
l52,0,sizeof(l34->l52));PIVOT_BUCKET_HANDLE(l34->l52) = l184;l141 = trie_init
(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l34->l52));PIVOT_BUCKET_TRIE(l34->l52)
->trie = l38;PIVOT_BUCKET_INDEX(l34->l52) = l34->bucket_index;
PIVOT_BUCKET_VRF(l34->l52) = l25;PIVOT_BUCKET_IPV6(l34->l52) = l31;
PIVOT_BUCKET_DEF(l34->l52) = FALSE;(l34->l52)->key[0] = l36[0];(l34->l52)->
key[1] = l36[1];(l34->l52)->key[2] = l36[2];(l34->l52)->key[3] = l36[3];(l34
->l52)->key[4] = l36[4];(l34->l52)->len = l37;l153 = VRF_PIVOT_TRIE_IPV6_128(
l34->l1,l25);l144((l36),(l37),(l31));l27(l34->l1,l36,l37,l25,l31,lpm_entry,0,
0);soc_L3_DEFIP_PAIR_128m_field32_set(l34->l1,&lpm_entry,ALG_BKT_PTR0f,l34->
bucket_index);return l141;}static int l39(l33*l34,int*l40,int*l15){
trie_node_t*l38;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int l31,l25,l157;uint32
l37,l162 = 0;uint32 l158 = 0;uint32 l36[5];int l134,l181;
defip_pair_128_entry_t l178,l179;trie_t*l35,*trie;void*l182,*l183;
alpm_pivot_t*l185 = l34->l52;defip_pair_128_entry_t lpm_entry;soc_mem_t l21;
trie_t*l153 = NULL;alpm_mem_prefix_array_t l186;int*l44 = NULL;int l141 = 
SOC_E_NONE,l140,l187,l48 = -1;int l188;defip_alpm_raw_entry_t*l189 = NULL;
defip_alpm_raw_entry_t*l190;defip_alpm_raw_entry_t*l191;
defip_alpm_raw_entry_t*l192;defip_alpm_raw_entry_t*l193;l31 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l34->l1,l34->l7
,&l157,&l25));l21 = L3_DEFIP_ALPM_IPV6_128m;l182 = ((uint32*)&(l178));l183 = 
((uint32*)&(l179));l181 = l34->l52->l51;if(l25 == SOC_VRF_MAX(l34->l1)+1){if(
l2(l34->l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l34->l1,l158);
}else{SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l34->l1,l158);}}else{if(l2(l34->l1)<= 
2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l34->l1,l158);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l34->l1,l158);}}l35 = VRF_PREFIX_TRIE_IPV6_128(
l34->l1,l25);trie = PIVOT_BUCKET_TRIE(l34->l52);l141 = alpm_bucket_assign(l34
->l1,&l34->bucket_index,l31);if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l34->l1,"_soc_alpm_insert: Unable to allocate"
"new bucket for split\n")));l34->bucket_index = -1;l41(l34->l1,l35,trie,l34->
l43,l44,l34->bucket_index,l31);return l141;}l141 = trie_split(trie,
_MAX_KEY_LEN_144_,FALSE,l36,&l37,&l38,NULL,FALSE);if(SOC_FAILURE(l141)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l34->l1,
"_soc_alpm_insert: Could not split bucket")));l41(l34->l1,l35,trie,l34->l43,
l44,l34->bucket_index,l31);return l141;}l141 = l32(l34,l35,l36,l37,l38,&
lpm_entry);if(l141!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l34->
l1,"could not initialize pivot")));l41(l34->l1,l35,trie,l34->l43,l44,l34->
bucket_index,l31);return l141;}sal_memset(&l186,0,sizeof(l186));l141 = 
trie_traverse(PIVOT_BUCKET_TRIE(l34->l52),alpm_mem_prefix_array_cb,&l186,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l34->l1,"_soc_alpm_insert: Bucket split failed")));l41(l34->l1,l35
,trie,l34->l43,l44,l34->bucket_index,l31);return l141;}l44 = sal_alloc(sizeof
(*l44)*l186.count,"Temp storage for location of prefixes in new 128b bucket")
;if(l44 == NULL){l141 = SOC_E_MEMORY;l41(l34->l1,l35,trie,l34->l43,l44,l34->
bucket_index,l31);return l141;}sal_memset(l44,-1,sizeof(*l44)*l186.count);
l188 = sizeof(defip_alpm_raw_entry_t)*ALPM_RAW_BKT_COUNT_DW;l189 = sal_alloc(
4*l188,"Raw memory buffer");if(l189 == NULL){l141 = SOC_E_MEMORY;l41(l34->l1,
l35,trie,l34->l43,l44,l34->bucket_index,l31);if(l189!= NULL){sal_free(l189);}
return l141;}sal_memset(l189,0,4*l188);l190 = (defip_alpm_raw_entry_t*)l189;
l191 = (defip_alpm_raw_entry_t*)((uint8*)l190+l188);l192 = (
defip_alpm_raw_entry_t*)((uint8*)l191+l188);l193 = (defip_alpm_raw_entry_t*)(
(uint8*)l192+l188);l141 = _soc_alpm_raw_bucket_read(l34->l1,l21,
PIVOT_BUCKET_INDEX(l185),(void*)l190,(void*)l191);if(SOC_FAILURE(l141)){l41(
l34->l1,l35,trie,l34->l43,l44,l34->bucket_index,l31);if(l189!= NULL){sal_free
(l189);}return l141;}for(l140 = 0;l140<l186.count;l140++){payload_t*l133 = 
l186.prefix[l140];if(l133->index>0){_soc_alpm_raw_mem_read(l34->l1,l21,l190,
l133->index,l182);_soc_alpm_raw_mem_write(l34->l1,l21,l190,l133->index,
soc_mem_entry_null(l34->l1,l21));if(SOC_URPF_STATUS_GET(l34->l1)){
_soc_alpm_raw_mem_read(l34->l1,l21,l191,_soc_th_alpm_rpf_entry(l34->l1,l133->
index),l183);_soc_alpm_raw_mem_write(l34->l1,l21,l191,_soc_th_alpm_rpf_entry(
l34->l1,l133->index),soc_mem_entry_null(l34->l1,l21));}l141 = l194(l34->l1,
l21,l34->bucket_index,l140,l158,&l134);if(SOC_SUCCESS(l141)){
_soc_alpm_raw_mem_write(l34->l1,l21,l192,l134,l182);if(SOC_URPF_STATUS_GET(
l34->l1)){_soc_alpm_raw_mem_write(l34->l1,l21,l193,_soc_th_alpm_rpf_entry(l34
->l1,l134),l183);}}}else{l141 = l194(l34->l1,l21,l34->bucket_index,l140,l158,
&l134);if(SOC_SUCCESS(l141)){l48 = l140;*l15 = l134;_soc_alpm_raw_parity_set(
l34->l1,l21,l34->l156);_soc_alpm_raw_mem_write(l34->l1,l21,l192,l134,l34->
l156);if(SOC_URPF_STATUS_GET(l34->l1)){_soc_alpm_raw_parity_set(l34->l1,l21,
l34->l160);_soc_alpm_raw_mem_write(l34->l1,l21,l193,_soc_th_alpm_rpf_entry(
l34->l1,l134),l34->l160);}}}l44[l140] = l134;}l141 = 
_soc_alpm_raw_bucket_write(l34->l1,l21,l34->bucket_index,l158,(void*)l192,(
void*)l193,l186.count);if(SOC_FAILURE(l141)){l187 = 
_soc_th_alpm_rollback_bkt_move(l34->l1,l34->l7,l21,l185,l34->l52,&l186,l44,
l48);sal_free(l189);return l187;}l141 = l4(l34->l1,&lpm_entry,l40);if(
SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l34->l1,
"_soc_alpm_insert: Unable to add new""pivot to tcam\n")));if(l141 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l34->l1,l25,l31);}l187 = 
_soc_th_alpm_rollback_bkt_move(l34->l1,l34->l7,l21,l185,l34->l52,&l186,l44,
l48);return l141;}*l40 = soc_th_alpm_physical_idx(l34->l1,L3_DEFIPm,*l40,l31)
;l141 = l161(l34->l1,*l40,l162);if(SOC_FAILURE(l141)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l34->l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l40));l49(l34->
l1,&lpm_entry,l34->l7,*l40,l34->l52);l187 = _soc_th_alpm_rollback_bkt_move(
l34->l1,l34->l7,l21,l185,l34->l52,&l186,l44,l48);return l141;}l141 = 
trie_insert(l153,(l34->l52)->key,NULL,(l34->l52)->len,(trie_node_t*)l34->l52)
;if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l34->l1,
"failed to insert into pivot trie\n")));l187 = _soc_th_alpm_rollback_bkt_move
(l34->l1,l34->l7,l21,l185,l34->l52,&l186,l44,l48);return l141;}
ALPM_TCAM_PIVOT(l34->l1,SOC_ALPM_128_ADDR_LWR(*l40)<<1) = l34->l52;
PIVOT_TCAM_INDEX(l34->l52) = SOC_ALPM_128_ADDR_LWR(*l40)<<1;VRF_PIVOT_REF_INC
(l34->l1,l25,l31);for(l140 = 0;l140<l186.count;l140++){l186.prefix[l140]->
index = l44[l140];}sal_free(l44);l141 = _soc_alpm_raw_bucket_write(l34->l1,
l21,PIVOT_BUCKET_INDEX(l185),l158,(void*)l190,(void*)l191,l186.count);if(
SOC_FAILURE(l141)){l187 = _soc_th_alpm_rollback_bkt_move(l34->l1,l34->l7,l21,
l185,l34->l52,&l186,l44,l48);sal_free(l189);}sal_free(l189);if(l48 == -1){
l141 = _soc_th_alpm_insert_in_bkt(l34->l1,l21,PIVOT_BUCKET_INDEX(l185),l158,
l34->l156,l14,&l134,l31);if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l34->l1,"_soc_alpm_insert: Could not insert new "
"prefix into trie after split\n")));l41(l34->l1,l35,trie,l34->l43,l44,l34->
bucket_index,l31);if(l189!= NULL){sal_free(l189);}return l141;}if(
SOC_URPF_STATUS_GET(l34->l1)){l141 = soc_mem_write(l34->l1,l21,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l34->l1,l134),l34->l160);}*l15 = l134;l34->l43->index = 
l134;}PIVOT_BUCKET_ENT_CNT_UPDATE(l34->l52);VRF_BUCKET_SPLIT_INC(l34->l1,l25,
l31);return l141;}static int l195(int l1,void*l7,soc_mem_t l21,void*l156,void
*l160,int*l15,int bucket_index,int l51){alpm_pivot_t*l52,*l185;
defip_aux_scratch_entry_t l12;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l196,l37;int l31,l25,l157;int l134;int l141 = SOC_E_NONE,l187;
uint32 l10,l158;int l152 =0;int l40;int l197 = 0;trie_t*trie,*l35;trie_node_t
*l154 = NULL;payload_t*l198,*l199,*l180;int l23 = 0;
defip_alpm_ipv6_128_entry_t l178,l179;void*l182,*l183;int*l44 = NULL;l33 l34;
l31 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l7
,&l157,&l25));if(l25 == SOC_VRF_MAX(l1)+1){l10 = 0;if(l2(l1)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l158);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l158);}}else{l10 = 2;if(l2(l1)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,l158);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l158);}}l21 = L3_DEFIP_ALPM_IPV6_128m;l182 = 
((uint32*)&(l178));l183 = ((uint32*)&(l179));l141 = l150(l1,l7,prefix,&l37,&
l23);if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: prefix create failed\n")));return l141;}sal_memset(&
l12,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(l1,l7,l31,l10
,0,&l12));if(bucket_index == 0){l141 = l151(l1,prefix,l37,l31,l25,&l152,&l51,
&bucket_index);SOC_IF_ERROR_RETURN(l141);if(l152 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: "
" Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l141 = 
_soc_th_alpm_insert_in_bkt(l1,l21,bucket_index,l158,l156,l14,&l134,l31);if(
l141 == SOC_E_NONE){*l15 = l134;if(SOC_URPF_STATUS_GET(l1)){l187 = 
soc_mem_write(l1,l21,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l134),l160);if(
SOC_FAILURE(l187)){return l187;}}}if(l141 == SOC_E_FULL){l197 = 1;}l52 = 
ALPM_TCAM_PIVOT(l1,l51);trie = PIVOT_BUCKET_TRIE(l52);l185 = l52;l198 = 
sal_alloc(sizeof(payload_t),"Payload for 128b Key");if(l198 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Unable to allocate memory for ""trie node \n")));
return SOC_E_MEMORY;}l199 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l199 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l198);return SOC_E_MEMORY;}sal_memset(l198,0,
sizeof(*l198));sal_memset(l199,0,sizeof(*l199));l198->key[0] = prefix[0];l198
->key[1] = prefix[1];l198->key[2] = prefix[2];l198->key[3] = prefix[3];l198->
key[4] = prefix[4];l198->len = l37;l198->index = l134;sal_memcpy(l199,l198,
sizeof(*l198));l199->bkt_ptr = l198;l141 = trie_insert(trie,prefix,NULL,l37,(
trie_node_t*)l198);if(SOC_FAILURE(l141)){if(l198!= NULL){sal_free(l198);}if(
l199!= NULL){sal_free(l199);}return l141;}l35 = VRF_PREFIX_TRIE_IPV6_128(l1,
l25);if(!l23){l141 = trie_insert(l35,prefix,NULL,l37,(trie_node_t*)l199);}
else{l154 = NULL;l141 = trie_find_lpm(l35,0,0,&l154);l180 = (payload_t*)l154;
if(SOC_SUCCESS(l141)){l180->bkt_ptr = l198;}}l196 = l37;if(SOC_FAILURE(l141))
{l41(l1,l35,trie,l199,l44,bucket_index,l31);return l141;}if(l197){l34.l1 = l1
;l34.l7 = l7;l34.l43 = l198;l34.l52 = l52;l34.l156 = l156;l34.l160 = l160;l34
.bucket_index = bucket_index;l141 = l39(&l34,&l40,l15);if(l141!= SOC_E_NONE){
return l141;}l52 = ALPM_TCAM_PIVOT(l1,l40);}VRF_TRIE_ROUTES_INC(l1,l25,l31);
if(l23){sal_free(l199);}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
DELETE_PROPAGATE,&l12,TRUE,&l152,&l51,&bucket_index));SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l12,FALSE,&l152,&l51,&bucket_index))
;if(SOC_URPF_STATUS_GET(l1)){l37 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf);l37+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l37);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l152,&l51,&bucket_index));
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l12,FALSE,&l152,
&l51,&bucket_index));}PIVOT_BUCKET_ENT_CNT_UPDATE(l185);return l141;}static
int l27(int l3,uint32*key,int len,int l25,int l9,defip_pair_128_entry_t*
lpm_entry,int l28,int l29){uint32 l147;if(l29){sal_memset(lpm_entry,0,sizeof(
defip_pair_128_entry_t));}soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
VRF_ID_0_LWRf,l25&SOC_VRF_MAX(l3));soc_L3_DEFIP_PAIR_128m_field32_set(l3,
lpm_entry,VRF_ID_1_LWRf,l25&SOC_VRF_MAX(l3));
soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,VRF_ID_0_UPRf,l25&SOC_VRF_MAX
(l3));soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,VRF_ID_1_UPRf,l25&
SOC_VRF_MAX(l3));if(l25 == (SOC_VRF_MAX(l3)+1)){l147 = 0;}else{l147 = 
SOC_VRF_MAX(l3);}soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
VRF_ID_MASK0_LWRf,l147);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
VRF_ID_MASK1_LWRf,l147);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
VRF_ID_MASK0_UPRf,l147);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
VRF_ID_MASK1_UPRf,l147);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
IP_ADDR0_LWRf,key[0]);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
IP_ADDR1_LWRf,key[1]);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
IP_ADDR0_UPRf,key[2]);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
IP_ADDR1_UPRf,key[3]);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,
MODE0_LWRf,3);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,MODE1_LWRf,3);
soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,MODE0_UPRf,3);
soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,MODE1_UPRf,3);l148(l3,(void*)
lpm_entry,len);soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,VALID0_LWRf,1)
;soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,VALID1_LWRf,1);
soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,VALID0_UPRf,1);
soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,VALID1_UPRf,1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,(1<<
soc_mem_field_length(l3,L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf))-1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,(1<<
soc_mem_field_length(l3,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf))-1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,(1<<
soc_mem_field_length(l3,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf))-1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,(1<<
soc_mem_field_length(l3,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf))-1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,(1
<<soc_mem_field_length(l3,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf))-1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,(1
<<soc_mem_field_length(l3,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_LWRf))-1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,(1
<<soc_mem_field_length(l3,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf))-1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,(1
<<soc_mem_field_length(l3,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_UPRf))-1);
return(SOC_E_NONE);}static int l200(int l1,void*l7,int bucket_index,int l51,
int l134){alpm_pivot_t*l52;defip_alpm_ipv6_128_entry_t l178,l201,l179;
defip_aux_scratch_entry_t l12;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t
l21;void*l182,*l202,*l183 = NULL;int l157;int l9;int l141 = SOC_E_NONE,l187 = 
SOC_E_NONE;uint32 l203[5],prefix[5];int l31,l25;uint32 l37;int l204;uint32 l10
,l158;int l152,l23 = 0;trie_t*trie,*l35;uint32 l205;defip_pair_128_entry_t
lpm_entry,*l206;payload_t*l198 = NULL,*l207 = NULL,*l180 = NULL;trie_node_t*
l208 = NULL,*l154 = NULL;trie_t*l153 = NULL;l9 = l31 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l7,&l157,&l25));if(l157!= 
SOC_L3_VRF_OVERRIDE){if(l25 == SOC_VRF_MAX(l1)+1){l10 = 0;if(l2(l1)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l158);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l158);}}else{l10 = 2;if(l2(l1)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,l158);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l158);}}l141 = l150(l1,l7,prefix,&l37,&l23);
if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: prefix create failed\n")));return l141;}if(!
soc_th_alpm_mode_get(l1)){if(l157!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT
(l1,l25,l31)>1){if(l23){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot v6-128 delete ""default route if other routes are present "
"in this mode"),l25));return SOC_E_PARAM;}}}l10 = 2;}l21 = 
L3_DEFIP_ALPM_IPV6_128m;l182 = ((uint32*)&(l178));SOC_ALPM_LPM_LOCK(l1);if(
bucket_index == 0){l141 = l155(l1,l7,l21,l182,&l51,&bucket_index,&l134);}else
{l141 = l18(l1,l7,l182,0,l21,0,0);}sal_memcpy(&l201,l182,sizeof(l201));l202 = 
&l201;if(SOC_FAILURE(l141)){SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_delete: Unable to find ""prefix for delete\n")
));return l141;}l204 = bucket_index;l52 = ALPM_TCAM_PIVOT(l1,l51);trie = 
PIVOT_BUCKET_TRIE(l52);l141 = trie_delete(trie,prefix,l37,&l208);l198 = (
payload_t*)l208;if(l141!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l141;}l35 = VRF_PREFIX_TRIE_IPV6_128(l1,l25);
l153 = VRF_PIVOT_TRIE_IPV6_128(l1,l25);if(!l23){l141 = trie_delete(l35,prefix
,l37,&l208);l207 = (payload_t*)l208;if(SOC_FAILURE(l141)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l209;}l154 = NULL;l141 = trie_find_lpm(l35,prefix,l37,&l154)
;l180 = (payload_t*)l154;if(SOC_SUCCESS(l141)){payload_t*l210 = (payload_t*)(
l180->bkt_ptr);if(l210!= NULL){l205 = l210->len;}else{l205 = 0;}}else{
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l211;}sal_memcpy(l203,prefix,sizeof(
prefix));l144((l203),(l37),(l31));l141 = l27(l1,l203,l205,l25,l9,&lpm_entry,0
,1);(void)l24(l1,l182,l21,l9,l157,bucket_index,0,&lpm_entry);(void)l27(l1,
l203,l37,l25,l9,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(
l141)){l183 = ((uint32*)&(l179));l187 = soc_mem_read(l1,l21,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l134),l183);}}if((l205 == 0)&&SOC_FAILURE(l187)){
l206 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25);sal_memcpy(&lpm_entry,l206,
sizeof(lpm_entry));l141 = l27(l1,prefix,l205,l25,l9,&lpm_entry,0,1);}if(
SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l211;}l206 = &lpm_entry;}else{l154 = 
NULL;l141 = trie_find_lpm(l35,prefix,l37,&l154);l180 = (payload_t*)l154;if(
SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),l25));goto l209;}l180->bkt_ptr = 0;
l205 = 0;l206 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25);}l141 = l8(l1,l206,
l31,l10,l205,&l12);if(SOC_FAILURE(l141)){goto l211;}l141 = 
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l152,&l51,&bucket_index);
if(SOC_FAILURE(l141)){goto l211;}if(SOC_URPF_STATUS_GET(l1)){uint32 l147;if(
l183!= NULL){l147 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l12,
DB_TYPEf);l147++;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,
l147);l147 = soc_mem_field32_get(l1,l21,l183,SRC_DISCARDf);
soc_mem_field32_set(l1,l21,&l12,SRC_DISCARDf,l147);l147 = soc_mem_field32_get
(l1,l21,l183,DEFAULTROUTEf);soc_mem_field32_set(l1,l21,&l12,DEFAULTROUTEf,
l147);l141 = _soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l152,&l51,&
bucket_index);}if(SOC_FAILURE(l141)){goto l211;}}sal_free(l198);if(!l23){
sal_free(l207);}PIVOT_BUCKET_ENT_CNT_UPDATE(l52);if((l52->len!= 0)&&(trie->
trie == NULL)){uint32 l212[5];sal_memcpy(l212,l52->key,sizeof(l212));l144((
l212),(l52->len),(l9));l27(l1,l212,l52->len,l25,l9,&lpm_entry,0,1);l141 = l6(
l1,&lpm_entry);if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1
,"_soc_th_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),l52->key[1],l52->key[2],l52->
key[3],l52->key[4]));}}l141 = _soc_th_alpm_delete_in_bkt(l1,l21,l204,l158,
l202,l14,&l134,l31);if(!SOC_SUCCESS(l141)){SOC_ALPM_LPM_UNLOCK(l1);l141 = 
SOC_E_FAIL;return l141;}if(SOC_URPF_STATUS_GET(l1)){l141 = 
soc_mem_alpm_delete(l1,l21,SOC_ALPM_RPF_BKT_IDX(l1,l204),MEM_BLOCK_ALL,l158,
l202,l14,&l152);if(!SOC_SUCCESS(l141)){SOC_ALPM_LPM_UNLOCK(l1);l141 = 
SOC_E_FAIL;return l141;}}if((l52->len!= 0)&&(trie->trie == NULL)){l141 = 
alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l52),l31);if(SOC_FAILURE(l141)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(l52)));}l141 = trie_delete(l153,l52->key,l52->len,&l208);
if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l52));sal_free(PIVOT_BUCKET_HANDLE(l52));sal_free(l52);
_soc_trident2_alpm_bkt_view_set(l1,l204<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l31)){_soc_trident2_alpm_bkt_view_set(l1,(l204+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l25,l31);if(VRF_TRIE_ROUTES_CNT(l1,
l25,l31) == 0){l141 = l30(l1,l25,l31);}SOC_ALPM_LPM_UNLOCK(l1);return l141;
l211:l187 = trie_insert(l35,prefix,NULL,l37,(trie_node_t*)l207);if(
SOC_FAILURE(l187)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l209:l187 = trie_insert(trie,prefix,NULL,l37,(
trie_node_t*)l198);if(SOC_FAILURE(l187)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}SOC_ALPM_LPM_UNLOCK(l1);return l141;}int
soc_th_alpm_128_init(int l1){int l141 = SOC_E_NONE;l141 = 
soc_th_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l141);return l141;}int
soc_th_alpm_128_state_clear(int l1){int l140,l141;for(l140 = 0;l140<= 
SOC_VRF_MAX(l1)+1;l140++){l141 = trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,
l140),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l141)){
trie_destroy(VRF_PREFIX_TRIE_IPV6_128(l1,l140));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n"),l1,l140));return
l141;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l140)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l140));}}return SOC_E_NONE;}int
soc_th_alpm_128_deinit(int l1){soc_th_alpm_128_lpm_deinit(l1);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_state_clear(l1));return SOC_E_NONE;}
static int l213(int l1,int l25,int l31){defip_pair_128_entry_t*lpm_entry,l214
;int l215;int index;int l141 = SOC_E_NONE;uint32 key[5] = {0,0,0,0,0};uint32
l37;alpm_bucket_handle_t*l184;alpm_pivot_t*l52;payload_t*l207;trie_t*l216;
trie_t*l217 = NULL;trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(l1,
l25));l217 = VRF_PIVOT_TRIE_IPV6_128(l1,l25);trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6_128(l1,l25));l216 = VRF_PREFIX_TRIE_IPV6_128(l1,l25);
lpm_entry = sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(
lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: unable to allocate memory "
"for IPv6-128 LPM entry\n")));return SOC_E_MEMORY;}l27(l1,key,0,l25,l31,
lpm_entry,0,1);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25) = lpm_entry;if(l25 == 
SOC_VRF_MAX(l1)+1){soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,
DEFAULT_MISSf,1);}l141 = alpm_bucket_assign(l1,&l215,l31);
soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,ALG_BKT_PTRf,l215);sal_memcpy
(&l214,lpm_entry,sizeof(l214));l141 = l4(l1,&l214,&index);l184 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l184 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: Unable to allocate memory ""for bucket handle \n"))
);return SOC_E_NONE;}sal_memset(l184,0,sizeof(*l184));l52 = sal_alloc(sizeof(
alpm_pivot_t),"Payload for Pivot");if(l52 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l1,"soc_th_alpm_128_vrf_add: Unable to allocate memory "
"for PIVOT trie node \n")));sal_free(l184);return SOC_E_MEMORY;}l207 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(l207 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: Unable to allocate memory ""for pfx trie node \n"))
);sal_free(l184);sal_free(l52);return SOC_E_MEMORY;}sal_memset(l52,0,sizeof(*
l52));sal_memset(l207,0,sizeof(*l207));l37 = 0;PIVOT_BUCKET_HANDLE(l52) = 
l184;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l52));PIVOT_BUCKET_INDEX(
l52) = l215;PIVOT_BUCKET_VRF(l52) = l25;PIVOT_BUCKET_IPV6(l52) = l31;
PIVOT_BUCKET_DEF(l52) = TRUE;l52->key[0] = l207->key[0] = key[0];l52->key[1] = 
l207->key[1] = key[1];l52->key[2] = l207->key[2] = key[2];l52->key[3] = l207
->key[3] = key[3];l52->key[4] = l207->key[4] = key[4];l52->len = l207->len = 
l37;l141 = trie_insert(l216,key,NULL,l37,&(l207->node));if(SOC_FAILURE(l141))
{sal_free(l207);sal_free(l52);sal_free(l184);return l141;}l141 = trie_insert(
l217,key,NULL,l37,(trie_node_t*)l52);if(SOC_FAILURE(l141)){trie_node_t*l208 = 
NULL;(void)trie_delete(l216,key,l37,&l208);sal_free(l207);sal_free(l52);
sal_free(l184);return l141;}index = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,index,l31);ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(index)
<<1) = l52;PIVOT_TCAM_INDEX(l52) = SOC_ALPM_128_ADDR_LWR(index)<<1;
VRF_PIVOT_REF_INC(l1,l25,l31);VRF_TRIE_INIT_DONE(l1,l25,l31,1);return l141;}
static int l30(int l1,int l25,int l31){defip_pair_128_entry_t*lpm_entry;int
l215;int l218;int l141 = SOC_E_NONE;uint32 key[2] = {0,0},l145[
SOC_MAX_MEM_FIELD_WORDS];payload_t*l198;alpm_pivot_t*l219;trie_node_t*l208;
trie_t*l216;trie_t*l217 = NULL;lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1
,l25);l215 = soc_L3_DEFIP_PAIR_128m_field32_get(l1,lpm_entry,ALG_BKT_PTRf);
l141 = alpm_bucket_release(l1,l215,l31);_soc_trident2_alpm_bkt_view_set(l1,
l215<<2,INVALIDm);if(SOC_ALPM_V6_SCALE_CHECK(l1,l31)){
_soc_trident2_alpm_bkt_view_set(l1,(l215+1)<<2,INVALIDm);}l141 = l17(l1,
lpm_entry,(void*)l145,&l218);if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l1,"soc_th_alpm_vrf_delete: unable to get internal"
" pivot idx for vrf %d/%d\n"),l25,l31));l218 = -1;}l218 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l218,l31);l219 = 
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l218)<<1);l141 = l6(l1,lpm_entry);if
(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),l25,l31));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25) = NULL;l216 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l25);VRF_PREFIX_TRIE_IPV6_128(l1,l25) = NULL;
VRF_TRIE_INIT_DONE(l1,l25,l31,0);l141 = trie_delete(l216,key,0,&l208);l198 = 
(payload_t*)l208;if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),l25,l31));}
sal_free(l198);(void)trie_destroy(l216);l217 = VRF_PIVOT_TRIE_IPV6_128(l1,l25
);VRF_PIVOT_TRIE_IPV6_128(l1,l25) = NULL;l208 = NULL;l141 = trie_delete(l217,
key,0,&l208);if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l25,l31));}(void)
trie_destroy(l217);sal_free(PIVOT_BUCKET_HANDLE(l219));(void)trie_destroy(
PIVOT_BUCKET_TRIE(l219));sal_free(l219);return l141;}int
soc_th_alpm_128_insert(int l1,void*l5,uint32 l22,int l220,int l221){
defip_alpm_ipv6_128_entry_t l178,l179;soc_mem_t l21;void*l182,*l202;int l157,
l25;int index;int l9;int l141 = SOC_E_NONE;uint32 l23;l9 = L3_DEFIP_MODE_128;
l21 = L3_DEFIP_ALPM_IPV6_128m;l182 = ((uint32*)&(l178));l202 = ((uint32*)&(
l179));SOC_IF_ERROR_RETURN(l18(l1,l5,l182,l202,l21,l22,&l23));
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l5,&l157,&l25));if(((l157
== SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l157 == SOC_L3_VRF_GLOBAL)))){l141 = l4(l1,l5,&
index);if(SOC_SUCCESS(l141)){VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l9);
VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l9);}else if(l141 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID,l9);}return(l141);}else if(l25 == 0){if(
soc_th_alpm_mode_get(l1)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unit %d, VRF=0 cannot be added in Parallel mode\n"),l1));return SOC_E_PARAM;
}}if(l157!= SOC_L3_VRF_GLOBAL){if(!soc_th_alpm_mode_get(l1)){if(
VRF_TRIE_ROUTES_CNT(l1,l25,l9) == 0){if(!l23){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n"),l157));return SOC_E_PARAM;}}}}if(!
VRF_TRIE_INIT_COMPLETED(l1,l25,l9)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d is not ""initialized\n"),l25));l141 = l213(l1,
l25,l9);if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),l25,l9));return
l141;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init ""completed\n"),l25,l9));}if(l221
&SOC_ALPM_LOOKUP_HIT){l141 = l159(l1,l5,l182,l202,l21,l220);}else{if(l220 == 
-1){l220 = 0;}l141 = l195(l1,l5,l21,l182,l202,&index,
SOC_ALPM_BKT_ENTRY_TO_IDX(l220),l221);}if(l141!= SOC_E_NONE){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d :soc_th_alpm_128_insert: "
"Route Insertion Failed :%s\n"),l1,soc_errmsg(l141)));}return(l141);}int
soc_th_alpm_128_lookup(int l1,void*l7,void*l14,int*l15,int*l222){
defip_alpm_ipv6_128_entry_t l178;soc_mem_t l21;int bucket_index;int l51;void*
l182;int l157,l25;int l9 = 2,l133;int l141 = SOC_E_NONE;SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(l1,l7,&l157,&l25));l141 = l13(l1,l7,l14,l15,&l133
,&l9);if(SOC_SUCCESS(l141)){SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(
l1,l14,&l157,&l25));if(l157 == SOC_L3_VRF_OVERRIDE){return SOC_E_NONE;}}if(!
VRF_TRIE_INIT_COMPLETED(l1,l25,l9)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_lookup:VRF %d is not ""initialized\n"),l25));*l15 = 0;*l222 = 0;
return SOC_E_NOT_FOUND;}l21 = L3_DEFIP_ALPM_IPV6_128m;l182 = ((uint32*)&(l178
));SOC_ALPM_LPM_LOCK(l1);l141 = l155(l1,l7,l21,l182,&l51,&bucket_index,l15);
SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l141)){*l222 = l51;*l15 = bucket_index
<<2;return l141;}l141 = l24(l1,l182,l21,l9,l157,bucket_index,*l15,l14);*l222 = 
SOC_ALPM_LOOKUP_HIT|l51;return(l141);}int soc_th_alpm_128_delete(int l1,void*
l7,int l220,int l221){int l157,l25;int l9;int l141 = SOC_E_NONE;l9 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l7,&l157
,&l25));if(l157 == SOC_L3_VRF_OVERRIDE){l141 = l6(l1,l7);if(SOC_SUCCESS(l141)
){VRF_TRIE_ROUTES_DEC(l1,MAX_VRF_ID,l9);VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l9);}
return(l141);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l25,l9)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_th_alpm_128_delete:VRF %d/%d is not "
"initialized\n"),l25,l9));return SOC_E_NONE;}if(l220 == -1){l220 = 0;}l141 = 
l200(l1,l7,SOC_ALPM_BKT_ENTRY_TO_IDX(l220),l221&~SOC_ALPM_LOOKUP_HIT,l220);}
return(l141);}static void l121(int l1,void*l14,int index,l116 l122){l122[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l72));l122[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l109[(l1)]->l70));l122[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l76));l122[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l109[(l1)]->l74));l122[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l73));l122[5] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l109[(l1)]->l71));l122[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l77));l122[7] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l109[(l1)]->l75));if((!(
SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l95)!= NULL))){int l223;(void)
soc_th_alpm_128_lpm_vrf_get(l1,l14,(int*)&l122[8],&l223);}else{l122[8] = 0;};
}static int l224(l116 l118,l116 l119){int l218;for(l218 = 0;l218<9;l218++){{
if((l118[l218])<(l119[l218])){return-1;}if((l118[l218])>(l119[l218])){return 1
;}};}return(0);}static void l225(int l1,void*l5,uint32 l226,uint32 l136,int
l133){l116 l227;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l5),(l109[(l1)]->l92))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l109[(l1)]->l91))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l5),(l109[(l1)]->l90))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l109[(l1)]->l89))){l227[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5),(l109[(l1)]->l72));l227[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l109[(l1)]->l70));l227[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5),(l109[(l1)]->l76));l227[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l109[(l1)]->l74));l227[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5),(l109[(l1)]->l73));l227[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l109[(l1)]->l71));l227[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5),(l109[(l1)]->l77));l227[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l109[(l1)]->l75));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l95)!= NULL)
)){int l223;(void)soc_th_alpm_128_lpm_vrf_get(l1,l5,(int*)&l227[8],&l223);}
else{l227[8] = 0;};l135((l120[(l1)]),l224,l227,l133,l136,l226);}}static void
l228(int l1,void*l7,uint32 l226){l116 l227;int l133 = -1;int l141;uint16 index
;l227[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l7),(l109[(l1)]->l72));l227[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7)
,(l109[(l1)]->l70));l227[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7),(l109[(l1)]->l76));l227[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7)
,(l109[(l1)]->l74));l227[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7),(l109[(l1)]->l73));l227[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7)
,(l109[(l1)]->l71));l227[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7),(l109[(l1)]->l77));l227[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7)
,(l109[(l1)]->l75));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l95)!= NULL)
)){int l223;(void)soc_th_alpm_128_lpm_vrf_get(l1,l7,(int*)&l227[8],&l223);}
else{l227[8] = 0;};index = l226;l141 = l137((l120[(l1)]),l224,l227,l133,index
);if(SOC_FAILURE(l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\ndel  index: H %d error %d\n"),index,l141));}}static int l229(int l1,void*
l7,int l133,int*l134){l116 l227;int l141;uint16 index = (0xFFFF);l227[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7)
,(l109[(l1)]->l72));l227[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7),(l109[(l1)]->l70));l227[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7)
,(l109[(l1)]->l76));l227[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7),(l109[(l1)]->l74));l227[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7)
,(l109[(l1)]->l73));l227[5] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7),(l109[(l1)]->l71));l227[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7)
,(l109[(l1)]->l77));l227[7] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l7),(l109[(l1)]->l75));if((!(
SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l95)!= NULL))){int l223;(void)
soc_th_alpm_128_lpm_vrf_get(l1,l7,(int*)&l227[8],&l223);}else{l227[8] = 0;};
l141 = l130((l120[(l1)]),l224,l227,l133,&index);if(SOC_FAILURE(l141)){*l134 = 
0xFFFFFFFF;return(l141);}*l134 = index;return(SOC_E_NONE);}static uint16 l123
(uint8*l124,int l125){return(_shr_crc16b(0,l124,l125));}static int l126(int l3
,int l111,int l112,l115**l127){l115*l131;int index;if(l112>l111){return
SOC_E_MEMORY;}l131 = sal_alloc(sizeof(l115),"lpm_hash");if(l131 == NULL){
return SOC_E_MEMORY;}sal_memset(l131,0,sizeof(*l131));l131->l3 = l3;l131->
l111 = l111;l131->l112 = l112;l131->l113 = sal_alloc(l131->l112*sizeof(*(l131
->l113)),"hash_table");if(l131->l113 == NULL){sal_free(l131);return
SOC_E_MEMORY;}l131->l114 = sal_alloc(l131->l111*sizeof(*(l131->l114)),
"link_table");if(l131->l114 == NULL){sal_free(l131->l113);sal_free(l131);
return SOC_E_MEMORY;}for(index = 0;index<l131->l112;index++){l131->l113[index
] = (0xFFFF);}for(index = 0;index<l131->l111;index++){l131->l114[index] = (
0xFFFF);}*l127 = l131;return SOC_E_NONE;}static int l128(l115*l129){if(l129!= 
NULL){sal_free(l129->l113);sal_free(l129->l114);sal_free(l129);}return
SOC_E_NONE;}static int l130(l115*l131,l117 l132,l116 entry,int l133,uint16*
l134){int l1 = l131->l3;uint16 l230;uint16 index;l230 = l123((uint8*)entry,(
32*9))%l131->l112;index = l131->l113[l230];;;while(index!= (0xFFFF)){uint32
l14[SOC_MAX_MEM_FIELD_WORDS];l116 l122;int l231;l231 = index;
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l231,l14));l121(
l1,l14,index,l122);if((*l132)(entry,l122) == 0){*l134 = index;;return(
SOC_E_NONE);}index = l131->l114[index&(0x3FFF)];;};return(SOC_E_NOT_FOUND);}
static int l135(l115*l131,l117 l132,l116 entry,int l133,uint16 l136,uint16 l44
){int l1 = l131->l3;uint16 l230;uint16 index;uint16 l232;l230 = l123((uint8*)
entry,(32*9))%l131->l112;index = l131->l113[l230];;;;l232 = (0xFFFF);if(l136
!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l14[SOC_MAX_MEM_FIELD_WORDS];l116
l122;int l231;l231 = index;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l231,l14));l121(l1,l14,index,l122);if((*l132)(entry,l122) == 0)
{if(l44!= index){;if(l232 == (0xFFFF)){l131->l113[l230] = l44;l131->l114[l44&
(0x3FFF)] = l131->l114[index&(0x3FFF)];l131->l114[index&(0x3FFF)] = (0xFFFF);
}else{l131->l114[l232&(0x3FFF)] = l44;l131->l114[l44&(0x3FFF)] = l131->l114[
index&(0x3FFF)];l131->l114[index&(0x3FFF)] = (0xFFFF);}};return(SOC_E_NONE);}
l232 = index;index = l131->l114[index&(0x3FFF)];;}}l131->l114[l44&(0x3FFF)] = 
l131->l113[l230];l131->l113[l230] = l44;return(SOC_E_NONE);}static int l137(
l115*l131,l117 l132,l116 entry,int l133,uint16 l138){uint16 l230;uint16 index
;uint16 l232;l230 = l123((uint8*)entry,(32*9))%l131->l112;index = l131->l113[
l230];;;l232 = (0xFFFF);while(index!= (0xFFFF)){if(l138 == index){;if(l232 == 
(0xFFFF)){l131->l113[l230] = l131->l114[l138&(0x3FFF)];l131->l114[l138&(
0x3FFF)] = (0xFFFF);}else{l131->l114[l232&(0x3FFF)] = l131->l114[l138&(0x3FFF
)];l131->l114[l138&(0x3FFF)] = (0xFFFF);}return(SOC_E_NONE);}l232 = index;
index = l131->l114[index&(0x3FFF)];;}return(SOC_E_NOT_FOUND);}static int l233
(int l1,void*l14){return(SOC_E_NONE);}void soc_th_alpm_128_lpm_state_dump(int
l1){int l140;int l234;l234 = ((3*(128+2+1))-1);if(!bsl_check(bslLayerSoc,
bslSourceAlpm,bslSeverityVerbose,l1)){return;}for(l140 = l234;l140>= 0;l140--
){if((l140!= ((3*(128+2+1))-1))&&((l61[(l1)][(l140)].l54) == -1)){continue;}
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"PFX = %d P = %d N = %d START = %d "
"END = %d VENT = %d FENT = %d\n"),l140,(l61[(l1)][(l140)].l56),(l61[(l1)][(
l140)].next),(l61[(l1)][(l140)].l54),(l61[(l1)][(l140)].l55),(l61[(l1)][(l140
)].l57),(l61[(l1)][(l140)].l58)));}COMPILER_REFERENCE(l233);}static int l235(
int l1,int index,uint32*l14){int l236;uint32 l237,l238,l239;uint32 l240;int
l241;if(!SOC_URPF_STATUS_GET(l1)){return(SOC_E_NONE);}if(soc_feature(l1,
soc_feature_l3_defip_hole)){l236 = (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m
)>>1);}else if(SOC_IS_APOLLO(l1)){l236 = (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l236 = (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1);}if(((l109[(l1)]->l64)!= NULL)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l64),(0));}l237 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l109[(l1)]->l76));l240 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l77));l238 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l14),(l109[(l1)]->l74));l239 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l75));l241 = ((!l237)&&(!l240)&&(!l238)&&(!l239))?1:0;l237 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l91));l240 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l14),(l109[(l1)]->l89));l238 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l90));l239 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l14),(l109[(l1)]->l90));if(l237&&l240&&l238&&l239){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l109[(l1)]->l88),(l241));}return l175(l1,MEM_BLOCK_ANY,index+l236,index,
l14);}static int l242(int l1,int l243,int l244){uint32 l14[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l243,l14));l225(l1,l14,l244,0x4000,0);SOC_IF_ERROR_RETURN(l175(
l1,MEM_BLOCK_ANY,l244,l243,l14));SOC_IF_ERROR_RETURN(l235(l1,l244,l14));do{
int l245,l246;l245 = soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l243),
1);l246 = soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l244),1);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l246))<<1) = ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l245))<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR
((l246))<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l246
))<<1)) = SOC_ALPM_128_ADDR_LWR((l246))<<1;}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l245))<<1) = NULL;}while(0);return(SOC_E_NONE);}static
int l247(int l1,int l133,int l9){int l243;int l244;l244 = (l61[(l1)][(l133)].
l55)+1;l243 = (l61[(l1)][(l133)].l54);if(l243!= l244){SOC_IF_ERROR_RETURN(
l242(l1,l243,l244));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l9);}(l61[(l1)][(l133)]
.l54)+= 1;(l61[(l1)][(l133)].l55)+= 1;return(SOC_E_NONE);}static int l248(int
l1,int l133,int l9){int l243;int l244;l244 = (l61[(l1)][(l133)].l54)-1;if((
l61[(l1)][(l133)].l57) == 0){(l61[(l1)][(l133)].l54) = l244;(l61[(l1)][(l133)
].l55) = l244-1;return(SOC_E_NONE);}l243 = (l61[(l1)][(l133)].l55);
SOC_IF_ERROR_RETURN(l242(l1,l243,l244));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l9)
;(l61[(l1)][(l133)].l54)-= 1;(l61[(l1)][(l133)].l55)-= 1;return(SOC_E_NONE);}
static int l249(int l1,int l133,int l9,void*l14,int*l250){int l251;int l252;
int l253;int l254;if((l61[(l1)][(l133)].l57) == 0){l254 = ((3*(128+2+1))-1);
if((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL)||(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)){if(l133<= (((3*(128+2+1
))/3)-1)){l254 = (((3*(128+2+1))/3)-1);}}while((l61[(l1)][(l254)].next)>l133)
{l254 = (l61[(l1)][(l254)].next);}l252 = (l61[(l1)][(l254)].next);if(l252!= -
1){(l61[(l1)][(l252)].l56) = l133;}(l61[(l1)][(l133)].next) = (l61[(l1)][(
l254)].next);(l61[(l1)][(l133)].l56) = l254;(l61[(l1)][(l254)].next) = l133;(
l61[(l1)][(l133)].l58) = ((l61[(l1)][(l254)].l58)+1)/2;(l61[(l1)][(l254)].l58
)-= (l61[(l1)][(l133)].l58);(l61[(l1)][(l133)].l54) = (l61[(l1)][(l254)].l55)
+(l61[(l1)][(l254)].l58)+1;(l61[(l1)][(l133)].l55) = (l61[(l1)][(l133)].l54)-
1;(l61[(l1)][(l133)].l57) = 0;}l253 = l133;while((l61[(l1)][(l253)].l58) == 0
){l253 = (l61[(l1)][(l253)].next);if(l253 == -1){l253 = l133;break;}}while((
l61[(l1)][(l253)].l58) == 0){l253 = (l61[(l1)][(l253)].l56);if(l253 == -1){if
((l61[(l1)][(l133)].l57) == 0){l251 = (l61[(l1)][(l133)].l56);l252 = (l61[(l1
)][(l133)].next);if(-1!= l251){(l61[(l1)][(l251)].next) = l252;}if(-1!= l252)
{(l61[(l1)][(l252)].l56) = l251;}}return(SOC_E_FULL);}}while(l253>l133){l252 = 
(l61[(l1)][(l253)].next);SOC_IF_ERROR_RETURN(l248(l1,l252,l9));(l61[(l1)][(
l253)].l58)-= 1;(l61[(l1)][(l252)].l58)+= 1;l253 = l252;}while(l253<l133){
SOC_IF_ERROR_RETURN(l247(l1,l253,l9));(l61[(l1)][(l253)].l58)-= 1;l251 = (l61
[(l1)][(l253)].l56);(l61[(l1)][(l251)].l58)+= 1;l253 = l251;}(l61[(l1)][(l133
)].l57)+= 1;(l61[(l1)][(l133)].l58)-= 1;(l61[(l1)][(l133)].l55)+= 1;*l250 = (
l61[(l1)][(l133)].l55);sal_memcpy(l14,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);return(
SOC_E_NONE);}static int l255(int l1,int l133,int l9,void*l14,int l256){int
l251;int l252;int l243;int l244;uint32 l257[SOC_MAX_MEM_FIELD_WORDS];int l141
;int l147;l243 = (l61[(l1)][(l133)].l55);l244 = l256;(l61[(l1)][(l133)].l57)
-= 1;(l61[(l1)][(l133)].l58)+= 1;(l61[(l1)][(l133)].l55)-= 1;if(l244!= l243){
if((l141 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l243,l257))<0){return l141
;}l225(l1,l257,l244,0x4000,0);if((l141 = l175(l1,MEM_BLOCK_ANY,l244,l243,l257
))<0){return l141;}if((l141 = l235(l1,l244,l257))<0){return l141;}}l147 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l244,1);l256 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l243,1);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l147)<<1) = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
l256)<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l147)<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l147)<<1)) = 
SOC_ALPM_128_ADDR_LWR(l147)<<1;}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l256
)<<1) = NULL;sal_memcpy(l257,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),
soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);l225(l1,l257,l243,0x4000,0);if(
(l141 = l175(l1,MEM_BLOCK_ANY,l243,l243,l257))<0){return l141;}if((l141 = 
l235(l1,l243,l257))<0){return l141;}if((l61[(l1)][(l133)].l57) == 0){l251 = (
l61[(l1)][(l133)].l56);assert(l251!= -1);l252 = (l61[(l1)][(l133)].next);(l61
[(l1)][(l251)].next) = l252;(l61[(l1)][(l251)].l58)+= (l61[(l1)][(l133)].l58)
;(l61[(l1)][(l133)].l58) = 0;if(l252!= -1){(l61[(l1)][(l252)].l56) = l251;}(
l61[(l1)][(l133)].next) = -1;(l61[(l1)][(l133)].l56) = -1;(l61[(l1)][(l133)].
l54) = -1;(l61[(l1)][(l133)].l55) = -1;}return(l141);}int
soc_th_alpm_128_lpm_vrf_get(int l3,void*lpm_entry,int*l25,int*l258){int l157;
if(((l109[(l3)]->l99)!= NULL)){l157 = soc_L3_DEFIP_PAIR_128m_field32_get(l3,
lpm_entry,VRF_ID_0_LWRf);*l258 = l157;if(soc_L3_DEFIP_PAIR_128m_field32_get(
l3,lpm_entry,VRF_ID_MASK0_LWRf)){*l25 = l157;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l3,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(l3)]->l101))){*l25 = SOC_L3_VRF_GLOBAL;*l258 = SOC_VRF_MAX(
l3)+1;}else{*l25 = SOC_L3_VRF_OVERRIDE;}}else{*l25 = SOC_L3_VRF_DEFAULT;}
return(SOC_E_NONE);}static int l259(int l1,void*entry,int*l16){int l133=0;int
l141;int l157;int l260;l141 = l139(l1,entry,&l133);if(l141<0){return l141;}
l133+= 0;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,entry,&l157,&l141
));l260 = soc_th_alpm_mode_get(l1);switch(l157){case SOC_L3_VRF_GLOBAL:if((
l260 == SOC_ALPM_MODE_PARALLEL)||(l260 == SOC_ALPM_MODE_TCAM_ALPM)){*l16 = 
l133+((3*(128+2+1))/3);}else{*l16 = l133;}break;case SOC_L3_VRF_OVERRIDE:*l16
= l133+2*((3*(128+2+1))/3);break;default:if((l260 == SOC_ALPM_MODE_PARALLEL)
||(l260 == SOC_ALPM_MODE_TCAM_ALPM)){*l16 = l133;}else{*l16 = l133+((3*(128+2
+1))/3);}break;}return(SOC_E_NONE);}static int l13(int l1,void*l7,void*l14,
int*l15,int*l16,int*l9){int l141;int l134;int l133 = 0;*l9 = 
L3_DEFIP_MODE_128;l259(l1,l7,&l133);*l16 = l133;if(l229(l1,l7,l133,&l134) == 
SOC_E_NONE){*l15 = l134;if((l141 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(
*l9)?*l15:(*l15>>1),l14))<0){return l141;}return(SOC_E_NONE);}else{return(
SOC_E_NOT_FOUND);}}int soc_th_alpm_128_lpm_init(int l1){int l234;int l140;int
l261;int l262;if(!soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL)
;}l234 = (3*(128+2+1));l262 = sizeof(l59)*(l234);if((l61[(l1)]!= NULL)){
SOC_IF_ERROR_RETURN(soc_th_alpm_128_deinit(l1));}l109[l1] = sal_alloc(sizeof(
l107),"lpm_128_field_state");if(NULL == l109[l1]){return(SOC_E_MEMORY);}(l109
[l1])->l63 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l109[l1]
)->l64 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l109[l1])
->l65 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l109[l1])->l66 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l109[l1])->l67 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l109[l1])->l68 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l109[l1])->l69 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l109[l1])->l72 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l109[l1])->l70 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l109[l1])->l73 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l109[l1])->l71 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l109[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l109[l1])->
l74 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l109[
l1])->l77 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l109[l1])->l75 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l109[l1])->l80 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l109[l1])->l78 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l109[l1])->l81 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l109[l1])->l79 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l109[l1])->l84 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l109[l1])->l82 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l109[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l109[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l109[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l109[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l109[l1])->l88 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l109[l1])->l91 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l109[l1])->l89 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l109[l1])->l92 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l109[l1])->l90 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l109[l1])->l95 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l109[l1])->l93 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l109[l1])->l96 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l109[l1])->l94 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l109[l1])->l99 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l109[l1])->
l97 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l109[l1
])->l100 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(
l109[l1])->l98 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
VRF_ID_MASK1_UPRf);(l109[l1])->l101 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l109[l1])->l102 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l109[l1])->l103 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l109[l1])->l104 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l109[l1])->l105 = soc_mem_fieldinfo_get(l1
,L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf);(l109[l1])->l106 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_POOL_NUMBERf);(l61[(l1)]
) = sal_alloc(l262,"LPM 128 prefix info");if(NULL == (l61[(l1)])){sal_free(
l109[l1]);l109[l1] = NULL;return(SOC_E_MEMORY);}SOC_ALPM_LPM_LOCK(l1);
sal_memset((l61[(l1)]),0,l262);for(l140 = 0;l140<l234;l140++){(l61[(l1)][(
l140)].l54) = -1;(l61[(l1)][(l140)].l55) = -1;(l61[(l1)][(l140)].l56) = -1;(
l61[(l1)][(l140)].next) = -1;(l61[(l1)][(l140)].l57) = 0;(l61[(l1)][(l140)].
l58) = 0;}l261 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l1)){l261>>= 1;}if((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM
)){(l61[(l1)][(((3*(128+2+1))-1))].l55) = (l261>>1)-1;(l61[(l1)][(((((3*(128+
2+1))/3)-1)))].l58) = l261>>1;(l61[(l1)][((((3*(128+2+1))-1)))].l58) = (l261-
(l61[(l1)][(((((3*(128+2+1))/3)-1)))].l58));}else{(l61[(l1)][((((3*(128+2+1))
-1)))].l58) = l261;}if((l120[(l1)])!= NULL){if(l128((l120[(l1)]))<0){
SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(l120[(l1)]) = NULL;}if(l126(
l1,l261*2,l261,&(l120[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;
}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}int soc_th_alpm_128_lpm_deinit(
int l1){if(!soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}
SOC_ALPM_LPM_LOCK(l1);if((l120[(l1)])!= NULL){l128((l120[(l1)]));(l120[(l1)])
= NULL;}if((l61[(l1)]!= NULL)){sal_free(l109[l1]);l109[l1] = NULL;sal_free((
l61[(l1)]));(l61[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}
static int l4(int l1,void*l5,int*l263){int l133;int index;int l9;uint32 l14[
SOC_MAX_MEM_FIELD_WORDS];int l141 = SOC_E_NONE;int l264 = 0;sal_memcpy(l14,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);SOC_ALPM_LPM_LOCK(l1);l141 = l13(l1,l5,l14,&index,&
l133,&l9);if(l141 == SOC_E_NOT_FOUND){l141 = l249(l1,l133,l9,l14,&index);if(
l141<0){SOC_ALPM_LPM_UNLOCK(l1);return(l141);}}else{l264 = 1;}*l263 = index;
if(l141 == SOC_E_NONE){soc_th_alpm_128_lpm_state_dump(l1);LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_th_alpm_128_lpm_insert: %d %d\n"),index
,l133));if(!l264){l225(l1,l5,index,0x4000,0);}l141 = l175(l1,MEM_BLOCK_ANY,
index,index,l5);if(l141>= 0){l141 = l235(l1,index,l5);}}SOC_ALPM_LPM_UNLOCK(
l1);return(l141);}static int l6(int l1,void*l7){int l133;int index;int l9;
uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int l141 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(
l1);l141 = l13(l1,l7,l14,&index,&l133,&l9);if(l141 == SOC_E_NONE){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_th_alpm_lpm_delete: %d %d\n"),index,
l133));l228(l1,l7,index);l141 = l255(l1,l133,l9,l14,index);}
soc_th_alpm_128_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l141);}
static int l17(int l1,void*l7,void*l14,int*l15){int l133;int l141;int l9;
SOC_ALPM_LPM_LOCK(l1);l141 = l13(l1,l7,l14,l15,&l133,&l9);SOC_ALPM_LPM_UNLOCK
(l1);return(l141);}static int l8(int l3,void*l7,int l9,int l10,int l11,
defip_aux_scratch_entry_t*l12){uint32 l143;uint32 l265[4] = {0,0,0,0};int l133
= 0;int l141 = SOC_E_NONE;l143 = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7
,VALID0_LWRf);soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm,l12,VALIDf,l143);
l143 = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,MODE0_LWRf);
soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm,l12,MODEf,l143);l143 = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,ENTRY_TYPE0_LWRf);
soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm,l12,ENTRY_TYPEf,0);l143 = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,GLOBAL_ROUTEf);
soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm,l12,GLOBAL_ROUTEf,l143);l143 = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,ECMPf);soc_mem_field32_set(l3,
L3_DEFIP_AUX_SCRATCHm,l12,ECMPf,l143);l143 = soc_mem_field32_get(l3,
L3_DEFIP_PAIR_128m,l7,ECMP_PTRf);soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm
,l12,ECMP_PTRf,l143);l143 = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,
NEXT_HOP_INDEXf);soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm,l12,
NEXT_HOP_INDEXf,l143);l143 = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,
PRIf);soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm,l12,PRIf,l143);l143 = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,RPEf);soc_mem_field32_set(l3,
L3_DEFIP_AUX_SCRATCHm,l12,RPEf,l143);l143 =soc_mem_field32_get(l3,
L3_DEFIP_PAIR_128m,l7,VRF_ID_0_LWRf);soc_mem_field32_set(l3,
L3_DEFIP_AUX_SCRATCHm,l12,VRFf,l143);soc_mem_field32_set(l3,
L3_DEFIP_AUX_SCRATCHm,l12,DB_TYPEf,l10);l143 = soc_mem_field32_get(l3,
L3_DEFIP_PAIR_128m,l7,DST_DISCARDf);soc_mem_field32_set(l3,
L3_DEFIP_AUX_SCRATCHm,l12,DST_DISCARDf,l143);l143 = soc_mem_field32_get(l3,
L3_DEFIP_PAIR_128m,l7,CLASS_IDf);soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm
,l12,CLASS_IDf,l143);l265[0] = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,
IP_ADDR0_LWRf);l265[1] = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,
IP_ADDR1_LWRf);l265[2] = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,
IP_ADDR0_UPRf);l265[3] = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,l7,
IP_ADDR1_UPRf);soc_mem_field_set(l3,L3_DEFIP_AUX_SCRATCHm,(uint32*)l12,
IP_ADDRf,(uint32*)l265);l141 = l139(l3,l7,&l133);if(SOC_FAILURE(l141)){return
l141;}soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm,l12,IP_LENGTHf,l133);
soc_mem_field32_set(l3,L3_DEFIP_AUX_SCRATCHm,l12,REPLACE_LENf,l11);return(
SOC_E_NONE);}static int l18(int l3,void*lpm_entry,void*l19,void*l20,soc_mem_t
l21,uint32 l22,uint32*l266){uint32 l143;uint32 l265[4];int l133 = 0;int l141 = 
SOC_E_NONE;uint32 l23 = 0;sal_memset(l19,0,soc_mem_entry_words(l3,l21)*4);
l143 = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry,HITf);
soc_mem_field32_set(l3,l21,l19,HITf,l143);l143 = soc_mem_field32_get(l3,
L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf);soc_mem_field32_set(l3,l21,l19,
VALIDf,l143);l143 = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry,ECMPf
);soc_mem_field32_set(l3,l21,l19,ECMPf,l143);l143 = soc_mem_field32_get(l3,
L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf);soc_mem_field32_set(l3,l21,l19,
ECMP_PTRf,l143);l143 = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry,
NEXT_HOP_INDEXf);soc_mem_field32_set(l3,l21,l19,NEXT_HOP_INDEXf,l143);l143 = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry,PRIf);soc_mem_field32_set
(l3,l21,l19,PRIf,l143);l143 = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,
lpm_entry,RPEf);soc_mem_field32_set(l3,l21,l19,RPEf,l143);l143 = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf);
soc_mem_field32_set(l3,l21,l19,DST_DISCARDf,l143);l143 = soc_mem_field32_get(
l3,L3_DEFIP_PAIR_128m,lpm_entry,SRC_DISCARDf);soc_mem_field32_set(l3,l21,l19,
SRC_DISCARDf,l143);l143 = soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry
,CLASS_IDf);soc_mem_field32_set(l3,l21,l19,CLASS_IDf,l143);l265[0] = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf);l265[1] = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf);l265[2] = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf);l265[3] = 
soc_mem_field32_get(l3,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf);
soc_mem_field_set(l3,l21,(uint32*)l19,KEYf,(uint32*)l265);l141 = l139(l3,
lpm_entry,&l133);if(SOC_FAILURE(l141)){return l141;}if((l133 == 0)&&(l265[0]
== 0)&&(l265[1] == 0)&&(l265[2] == 0)&&(l265[3] == 0)){l23 = 1;}if(l266!= 
NULL){*l266 = l23;}soc_mem_field32_set(l3,l21,l19,LENGTHf,l133);if(l20 == 
NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(l3)){sal_memset(l20,0,
soc_mem_entry_words(l3,l21)*4);sal_memcpy(l20,l19,soc_mem_entry_words(l3,l21)
*4);soc_mem_field32_set(l3,l21,l20,DST_DISCARDf,0);soc_mem_field32_set(l3,l21
,l20,RPEf,0);soc_mem_field32_set(l3,l21,l20,SRC_DISCARDf,l22&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(l3,l21,l20,DEFAULTROUTEf,l23);}
return(SOC_E_NONE);}static int l24(int l3,void*l19,soc_mem_t l21,int l9,int
l25,int l26,int index,void*lpm_entry){uint32 l143;uint32 l265[4];uint32 l157,
l267;sal_memset(lpm_entry,0,soc_mem_entry_words(l3,L3_DEFIP_PAIR_128m)*4);
l143 = soc_mem_field32_get(l3,l21,l19,HITf);soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,HITf,l143);l143 = soc_mem_field32_get(l3,l21,l19
,VALIDf);soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf,l143
);soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_LWRf,l143);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_UPRf,l143);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_UPRf,l143);l143 = 
soc_mem_field32_get(l3,l21,l19,ECMPf);soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,ECMPf,l143);l143 = soc_mem_field32_get(l3,l21,
l19,ECMP_PTRf);soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf,
l143);l143 = soc_mem_field32_get(l3,l21,l19,NEXT_HOP_INDEXf);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf,l143);
l143 = soc_mem_field32_get(l3,l21,l19,PRIf);soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,PRIf,l143);l143 = soc_mem_field32_get(l3,l21,l19
,RPEf);soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,RPEf,l143);l143 = 
soc_mem_field32_get(l3,l21,l19,DST_DISCARDf);soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf,l143);l143 = soc_mem_field32_get(l3
,l21,l19,SRC_DISCARDf);soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,
SRC_DISCARDf,l143);l143 = soc_mem_field32_get(l3,l21,l19,CLASS_IDf);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf,l143);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ALG_BKT_PTRf,l26);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ALG_HIT_IDXf,index);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,3);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,3);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,3);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,3);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,1);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_LWRf,3);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_LWRf,3);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_UPRf,3);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_UPRf,3);
soc_mem_field_get(l3,l21,l19,KEYf,l265);soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf,l265[0]);soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf,l265[1]);soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf,l265[2]);soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf,l265[3]);l265[0] = l265[1] = l265[
2] = l265[3] = 0;l143 = soc_mem_field32_get(l3,l21,l19,LENGTHf);l148(l3,
lpm_entry,l143);if(l25 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_HIGHf,1);soc_mem_field32_set(l3,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_ROUTEf,1);l157 = 0;l267 = 0;}else if(l25
== SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,
GLOBAL_ROUTEf,1);l157 = 0;l267 = 0;}else{l157 = l25;l267 = SOC_VRF_MAX(l3);}
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_LWRf,l157);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_LWRf,l157);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_UPRf,l157);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_UPRf,l157);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_LWRf,l267);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_LWRf,l267);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_UPRf,l267);
soc_mem_field32_set(l3,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_UPRf,l267);
return(SOC_E_NONE);}int soc_th_alpm_128_warmboot_pivot_add(int l3,int l9,void
*lpm_entry,int l268,int l269){int l141 = SOC_E_NONE;uint32 key[4] = {0,0,0,0}
;alpm_pivot_t*l36 = NULL;alpm_bucket_handle_t*l184 = NULL;int l157 = 0,l25 = 
0;uint32 l270;trie_t*l217 = NULL;uint32 prefix[5] = {0};int l23 = 0;l141 = 
l150(l3,lpm_entry,prefix,&l270,&l23);SOC_IF_ERROR_RETURN(l141);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l3,lpm_entry,&l157,&l25));
l268 = soc_th_alpm_physical_idx(l3,L3_DEFIP_PAIR_128m,l268,l9);l184 = 
sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l184 == NULL)
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l3,
"Unable to allocate memory for PIVOT trie node \n")));return SOC_E_NONE;}
sal_memset(l184,0,sizeof(*l184));l36 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l36 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l3
,"Unable to allocate memory for PIVOT trie node \n")));sal_free(l184);return
SOC_E_MEMORY;}sal_memset(l36,0,sizeof(*l36));PIVOT_BUCKET_HANDLE(l36) = l184;
trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l36));key[0] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l3,lpm_entry,IP_ADDR0_LWRf);key[1] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l3,lpm_entry,IP_ADDR1_LWRf);key[2] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l3,lpm_entry,IP_ADDR0_UPRf);key[3] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l3,lpm_entry,IP_ADDR1_UPRf);
PIVOT_BUCKET_INDEX(l36) = l269;PIVOT_TCAM_INDEX(l36) = SOC_ALPM_128_ADDR_LWR(
l268)<<1;if(l157!= SOC_L3_VRF_OVERRIDE){l217 = VRF_PIVOT_TRIE_IPV6_128(l3,l25
);if(l217 == NULL){trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(l3,
l25));l217 = VRF_PIVOT_TRIE_IPV6_128(l3,l25);}sal_memcpy(l36->key,prefix,
sizeof(prefix));l36->len = l270;l141 = trie_insert(l217,l36->key,NULL,l36->
len,(trie_node_t*)l36);if(SOC_FAILURE(l141)){sal_free(l184);sal_free(l36);
return l141;}}ALPM_TCAM_PIVOT(l3,SOC_ALPM_128_ADDR_LWR(l268)<<1) = l36;
PIVOT_BUCKET_VRF(l36) = l25;PIVOT_BUCKET_IPV6(l36) = l9;
PIVOT_BUCKET_ENT_CNT_UPDATE(l36);if(key[0] == 0&&key[1] == 0&&key[2] == 0&&
key[3] == 0){PIVOT_BUCKET_DEF(l36) = TRUE;}VRF_PIVOT_REF_INC(l3,l25,l9);
return l141;}static int l271(int l3,int l9,void*lpm_entry,void*l19,soc_mem_t
l21,int l268,int l269,int l272){int l273;int l25;int l141 = SOC_E_NONE;int l23
= 0;uint32 prefix[5] = {0,0,0,0,0};uint32 l37;void*l274 = NULL;trie_t*l42 = 
NULL;trie_t*l35 = NULL;trie_node_t*l208 = NULL;payload_t*l275 = NULL;
payload_t*l199 = NULL;alpm_pivot_t*l52 = NULL;if((NULL == lpm_entry)||(NULL == 
l19)){return SOC_E_PARAM;}SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l3,
lpm_entry,&l273,&l25));l21 = L3_DEFIP_ALPM_IPV6_128m;l274 = sal_alloc(sizeof(
defip_pair_128_entry_t),"Temp Defip Pair lpm_entry");if(NULL == l274){return
SOC_E_MEMORY;}SOC_IF_ERROR_RETURN(l24(l3,l19,l21,l9,l273,l269,l268,l274));
l141 = l150(l3,l274,prefix,&l37,&l23);if(SOC_FAILURE(l141)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l3,"prefix create failed\n")));return l141;}
sal_free(l274);l268 = soc_th_alpm_physical_idx(l3,L3_DEFIP_PAIR_128m,l268,l9)
;l52 = ALPM_TCAM_PIVOT(l3,SOC_ALPM_128_ADDR_LWR(l268)<<1);l42 = 
PIVOT_BUCKET_TRIE(l52);l275 = sal_alloc(sizeof(payload_t),"Payload for Key");
if(NULL == l275){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l3,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l199 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l199){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l3,
"Unable to allocate memory for pfx trie node\n")));sal_free(l275);return
SOC_E_MEMORY;}sal_memset(l275,0,sizeof(*l275));sal_memset(l199,0,sizeof(*l199
));l275->key[0] = prefix[0];l275->key[1] = prefix[1];l275->key[2] = prefix[2]
;l275->key[3] = prefix[3];l275->key[4] = prefix[4];l275->len = l37;l275->
index = l272;sal_memcpy(l199,l275,sizeof(*l275));l141 = trie_insert(l42,
prefix,NULL,l37,(trie_node_t*)l275);if(SOC_FAILURE(l141)){goto l276;}if(l9){
l35 = VRF_PREFIX_TRIE_IPV6_128(l3,l25);}if(!l23){l141 = trie_insert(l35,
prefix,NULL,l37,(trie_node_t*)l199);if(SOC_FAILURE(l141)){goto l277;}}return
l141;l277:(void)trie_delete(l42,prefix,l37,&l208);l275 = (payload_t*)l208;
l276:sal_free(l275);sal_free(l199);return l141;}static int l278(int l3,int l31
,int l25,int l218,int bkt_ptr){int l141 = SOC_E_NONE;uint32 l37;uint32 key[5]
= {0,0,0,0,0};trie_t*l279 = NULL;payload_t*l207 = NULL;defip_pair_128_entry_t
*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(defip_pair_128_entry_t),
"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l3,"unable to allocate memory for LPM entry\n")));return
SOC_E_MEMORY;}l27(l3,key,0,l25,l31,lpm_entry,0,1);if(l25 == SOC_VRF_MAX(l3)+1
){soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,GLOBAL_ROUTEf,1);}else{
soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,DEFAULT_MISSf,1);}
soc_L3_DEFIP_PAIR_128m_field32_set(l3,lpm_entry,ALG_BKT_PTRf,bkt_ptr);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l3,l25) = lpm_entry;trie_init(
_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(l3,l25));l279 = 
VRF_PREFIX_TRIE_IPV6_128(l3,l25);l207 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l207 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l3,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l207,0,sizeof(*l207));l37 = 0;l207->key[0] = key[0];
l207->key[1] = key[1];l207->len = l37;l141 = trie_insert(l279,key,NULL,l37,&(
l207->node));if(SOC_FAILURE(l141)){sal_free(l207);return l141;}
VRF_TRIE_INIT_DONE(l3,l25,l31,1);return l141;}int
soc_th_alpm_128_warmboot_prefix_insert(int l3,int l9,void*lpm_entry,void*l19,
int l268,int l269,int l272){int l273;int l25;int l141 = SOC_E_NONE;soc_mem_t
l21;l21 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(l3,lpm_entry,&l273,&l25));if(l273 == 
SOC_L3_VRF_OVERRIDE){return(l141);}if(!VRF_TRIE_INIT_COMPLETED(l3,l25,l9)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l3,"soc_alpm_128_insert:VRF %d is not "
"initialized\n"),l25));l141 = l278(l3,l9,l25,l268,l269);if(SOC_FAILURE(l141))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l3,"VRF %d/%d trie init \n""failed\n")
,l25,l9));return l141;}}l141 = l271(l3,l9,lpm_entry,l19,l21,l268,l269,l272);
if(l141!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l3,"unit %d : "
"Route Insertion Failed :%s\n"),l3,soc_errmsg(l141)));return(l141);}
VRF_TRIE_ROUTES_INC(l3,l25,l9);return(l141);}int
soc_th_alpm_128_warmboot_bucket_bitmap_set(int l1,int l31,int l220){int l280 = 
1;if(l31){if(!soc_th_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l280 = 2;}}
if(SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l220,l280);return SOC_E_NONE;}int
soc_th_alpm_128_warmboot_lpm_reinit_done(int l3){int l218;int l281 = ((3*(128
+2+1))-1);int l261 = soc_mem_index_count(l3,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l3)){l261>>= 1;}if(!soc_th_alpm_mode_get(l3)){(l61[(l3)][
(((3*(128+2+1))-1))].l56) = -1;for(l218 = ((3*(128+2+1))-1);l218>-1;l218--){
if(-1 == (l61[(l3)][(l218)].l54)){continue;}(l61[(l3)][(l218)].l56) = l281;(
l61[(l3)][(l281)].next) = l218;(l61[(l3)][(l281)].l58) = (l61[(l3)][(l218)].
l54)-(l61[(l3)][(l281)].l55)-1;l281 = l218;}(l61[(l3)][(l281)].next) = -1;(
l61[(l3)][(l281)].l58) = l261-(l61[(l3)][(l281)].l55)-1;}else{(l61[(l3)][(((3
*(128+2+1))-1))].l56) = -1;for(l218 = ((3*(128+2+1))-1);l218>(((3*(128+2+1))-
1)/3);l218--){if(-1 == (l61[(l3)][(l218)].l54)){continue;}(l61[(l3)][(l218)].
l56) = l281;(l61[(l3)][(l281)].next) = l218;(l61[(l3)][(l281)].l58) = (l61[(
l3)][(l218)].l54)-(l61[(l3)][(l281)].l55)-1;l281 = l218;}(l61[(l3)][(l281)].
next) = -1;(l61[(l3)][(l281)].l58) = l261-(l61[(l3)][(l281)].l55)-1;l281 = ((
(3*(128+2+1))-1)/3);(l61[(l3)][((((3*(128+2+1))-1)/3))].l56) = -1;for(l218 = 
((((3*(128+2+1))-1)/3)-1);l218>-1;l218--){if(-1 == (l61[(l3)][(l218)].l54)){
continue;}(l61[(l3)][(l218)].l56) = l281;(l61[(l3)][(l281)].next) = l218;(l61
[(l3)][(l281)].l58) = (l61[(l3)][(l218)].l54)-(l61[(l3)][(l281)].l55)-1;l281 = 
l218;}(l61[(l3)][(l281)].next) = -1;(l61[(l3)][(l281)].l58) = (l261>>1)-(l61[
(l3)][(l281)].l55)-1;}return(SOC_E_NONE);}int
soc_th_alpm_128_warmboot_lpm_reinit(int l3,int l9,int l218,void*lpm_entry){
int l16;l225(l3,lpm_entry,l218,0x4000,0);SOC_IF_ERROR_RETURN(l259(l3,
lpm_entry,&l16));if((l61[(l3)][(l16)].l57) == 0){(l61[(l3)][(l16)].l54) = 
l218;(l61[(l3)][(l16)].l55) = l218;}else{(l61[(l3)][(l16)].l55) = l218;}(l61[
(l3)][(l16)].l57)++;return(SOC_E_NONE);}
#endif

#endif /* ALPM_ENABLE */
