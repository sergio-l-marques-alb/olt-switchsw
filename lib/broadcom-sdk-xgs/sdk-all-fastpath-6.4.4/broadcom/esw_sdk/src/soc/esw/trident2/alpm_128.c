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
 * Purpose: Primitives for LPM management in ALPM - Mode for IPv6-128.
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
void soc_alpm_128_lpm_state_dump(int l1);static int l2(int l1,void*l3,int*
index);static int l4(int l1,void*l5);static int l6(int l1,void*l5,int l7,int
l8,int l9,defip_aux_scratch_entry_t*l10);static int l11(int l1,void*l5,void*
l12,int*l13,int*l14,int*l7);static int l15(int l1,void*l5,void*l12,int*l13);
static int l16(int l17,void*lpm_entry,void*l18,void*l19,soc_mem_t l20,uint32
l21,uint32*l22);static int l23(int l17,void*l18,soc_mem_t l20,int l7,int l24,
int l25,int index,void*lpm_entry);static int l26(int l17,uint32*key,int len,
int l24,int l7,defip_pair_128_entry_t*lpm_entry,int l27,int l28);static int
l29(int l1,int l24,int l30);typedef struct l31{int l32;int l33;int l34;int
next;int l35;int l36;}l37,*l38;static l38 l39[SOC_MAX_NUM_DEVICES];typedef
struct l40{soc_field_info_t*l41;soc_field_info_t*l42;soc_field_info_t*l43;
soc_field_info_t*l44;soc_field_info_t*l45;soc_field_info_t*l46;
soc_field_info_t*l47;soc_field_info_t*l48;soc_field_info_t*l49;
soc_field_info_t*l50;soc_field_info_t*l51;soc_field_info_t*l52;
soc_field_info_t*l53;soc_field_info_t*l54;soc_field_info_t*l55;
soc_field_info_t*l56;soc_field_info_t*l57;soc_field_info_t*l58;
soc_field_info_t*l59;soc_field_info_t*l60;soc_field_info_t*l61;
soc_field_info_t*l62;soc_field_info_t*l63;soc_field_info_t*l64;
soc_field_info_t*l65;soc_field_info_t*l66;soc_field_info_t*l67;
soc_field_info_t*l68;soc_field_info_t*l69;soc_field_info_t*l70;
soc_field_info_t*l71;soc_field_info_t*l72;soc_field_info_t*l73;
soc_field_info_t*l74;soc_field_info_t*l75;soc_field_info_t*l76;
soc_field_info_t*l77;soc_field_info_t*l78;soc_field_info_t*l79;
soc_field_info_t*l80;soc_field_info_t*l81;soc_field_info_t*l82;
soc_field_info_t*l83;soc_field_info_t*l84;soc_field_info_t*l85;
soc_field_info_t*l86;soc_field_info_t*l87;soc_field_info_t*l88;
soc_field_info_t*l89;soc_field_info_t*l90;soc_field_info_t*l91;}l92,*l93;
static l93 l94[SOC_MAX_NUM_DEVICES];typedef struct l95{int l17;int l96;int l97
;uint16*l98;uint16*l99;}l100;typedef uint32 l101[9];typedef int(*l102)(l101
l103,l101 l104);static l100*l105[SOC_MAX_NUM_DEVICES];static void l106(int l1
,void*l12,int index,l101 l107);static uint16 l108(uint8*l109,int l110);static
int l111(int l17,int l96,int l97,l100**l112);static int l113(l100*l114);
static int l115(l100*l116,l102 l117,l101 entry,int l118,uint16*l119);static
int l120(l100*l116,l102 l117,l101 entry,int l118,uint16 l121,uint16 l122);
static int l123(l100*l116,l102 l117,l101 entry,int l118,uint16 l124);static
int l125(int l1,const void*entry,int*l118){int l126,l127;int l128[4] = {
IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};
uint32 l129;l129 = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l128[0]);
if((l127 = _ipmask2pfx(l129,l118))<0){return(l127);}for(l126 = 1;l126<4;l126
++){l129 = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l128[l126]);if(*
l118){if(l129!= 0xffffffff){return(SOC_E_PARAM);}*l118+= 32;}else{if((l127 = 
_ipmask2pfx(l129,l118))<0){return(l127);}}}return SOC_E_NONE;}static void l130
(uint32*l131,int l132,int l30){uint32 l133,l134,l32,prefix[5];int l126;
sal_memcpy(prefix,l131,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));
sal_memset(l131,0,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));l133 = 128-
l132;l32 = (l133+31)/32;if((l133%32) == 0){l32++;}l133 = l133%32;for(l126 = 
l32;l126<= 4;l126++){prefix[l126]<<= l133;if(l126<4){l134 = prefix[l126+1]&~(
0xffffffff>>l133);l134 = (((32-l133) == 32)?0:(l134)>>(32-l133));prefix[l126]
|= l134;}}for(l126 = l32;l126<= 4;l126++){l131[3-(l126-l32)] = prefix[l126];}
}static void l135(int l17,void*lpm_entry,int l14){int l126;soc_field_t l136[4
] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,
IP_ADDR_MASK1_UPRf};for(l126 = 0;l126<4;l126++){soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,l136[l126],0);}for(l126 = 0;l126<4;l126++){if(
l14<= 32)break;soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,l136[3-
l126],0xffffffff);l14-= 32;}soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,
lpm_entry,l136[3-l126],~(((l14) == 32)?0:(0xffffffff)>>(l14)));}static int
l137(int l1,void*entry,uint32*l131,uint32*l14,int*l22){int l126;int l118 = 0,
l32;int l127 = SOC_E_NONE;uint32 l133,l134;uint32 prefix[5];sal_memset(l131,0
,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(prefix,0,sizeof(
uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));prefix[0] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR0_LWRf);prefix[1] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR1_LWRf);prefix[2] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR0_UPRf);prefix[3] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR1_UPRf);l127 = l125(l1,entry,&l118);if(
SOC_FAILURE(l127)){return l127;}l133 = 128-l118;l32 = l133/32;l133 = l133%32;
for(l126 = l32;l126<4;l126++){prefix[l126]>>= l133;l134 = prefix[l126+1]&((1
<<l133)-1);l134 = (((32-l133) == 32)?0:(l134)<<(32-l133));prefix[l126]|= l134
;}for(l126 = l32;l126<4;l126++){l131[4-(l126-l32)] = prefix[l126];}*l14 = 
l118;if(l22!= NULL){*l22 = (prefix[0] == 0)&&(prefix[1] == 0)&&(prefix[2] == 
0)&&(prefix[3] == 0)&&(l118 == 0);}return SOC_E_NONE;}int l138(int l1,uint32*
prefix,uint32 l132,int l7,int l24,int*l139,int*l140,int*bucket_index){int l127
= SOC_E_NONE;trie_t*l141;trie_node_t*l142 = NULL;alpm_pivot_t*l143;l141 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l24);l127 = trie_find_lpm(l141,prefix,l132,&l142);
if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l127;}l143 = (alpm_pivot_t*)l142;*l139 = 1;*
l140 = PIVOT_TCAM_INDEX(l143);*bucket_index = PIVOT_BUCKET_INDEX(l143);return
SOC_E_NONE;}static int l144(int l1,void*l5,soc_mem_t l20,void*l145,int*l140,
int*bucket_index,int*l13,int l146){uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int
l147,l24,l30;int l119;uint32 l8,l148;int l127 = SOC_E_NONE;int l139 = 0;l30 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l5,&l147,&
l24));if(l147 == 0){if(soc_alpm_mode_get(l1)){return SOC_E_PARAM;}}if(l24 == 
SOC_VRF_MAX(l1)+1){l8 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l148);}else{l8 = 
2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l148);}if(l147!= SOC_L3_VRF_OVERRIDE){if(
l146){uint32 prefix[5],l132;int l22 = 0;l127 = l137(l1,l5,prefix,&l132,&l22);
if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l127;}l127 = l138(l1,
prefix,l132,l30,l24,&l139,l140,bucket_index);SOC_IF_ERROR_RETURN(l127);}else{
defip_aux_scratch_entry_t l10;sal_memset(&l10,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(l1,l5,l30,l8,0,&l10));
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l10,TRUE,&l139,l140,
bucket_index));}if(l139){l16(l1,l5,l12,0,l20,0,0);l127 = 
_soc_alpm_find_in_bkt(l1,l20,*bucket_index,l148,l12,l145,&l119,l30);if(
SOC_SUCCESS(l127)){*l13 = l119;}}else{l127 = SOC_E_NOT_FOUND;}}return l127;}
static int l149(int l1,void*l5,void*l145,void*l150,soc_mem_t l20,int l119){
defip_aux_scratch_entry_t l10;int l147,l30,l24;int bucket_index;uint32 l8,
l148;int l139 = 0,l134 = 0;int l140;l30 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l5,&l147,&l24));if(l24 == 
SOC_VRF_MAX(l1)+1){l8 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l148);}else{l8 = 
2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l148);}if(!soc_alpm_mode_get(l1)){l8 = 2;}
if(l147!= SOC_L3_VRF_OVERRIDE){sal_memset(&l10,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(l1,l5,l30,l8,0,&l10));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l20,MEM_BLOCK_ANY,l119,l145));if(
SOC_URPF_STATUS_GET(l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,l20,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l119),l150));}l134 = soc_mem_field32_get
(l1,L3_DEFIP_AUX_SCRATCHm,&l10,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,REPLACE_LENf,l134);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l139,&l140,&bucket_index));if
(SOC_URPF_STATUS_GET(l1)){l134 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm
,&l10,DB_TYPEf);l134+= 1;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l10,
DB_TYPEf,l134);SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,
TRUE,&l139,&l140,&bucket_index));}}return SOC_E_NONE;}static int l151(int l1,
int l152,int l153){int l127,l134,l154,l155;defip_aux_table_entry_t l156,l157;
l154 = SOC_ALPM_128_ADDR_LWR(l152);l155 = SOC_ALPM_128_ADDR_UPR(l152);l127 = 
soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l154,&l156);
SOC_IF_ERROR_RETURN(l127);l127 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l155,&l157);SOC_IF_ERROR_RETURN(l127);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH0f,l153);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH1f,l153);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH0f,l153);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH1f,l153);l134 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,&l156,DB_TYPE0f);l127 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l154,&l156);SOC_IF_ERROR_RETURN(l127);l127 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l155,&l157);
SOC_IF_ERROR_RETURN(l127);if(SOC_URPF_STATUS_GET(l1)){l134++;
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH0f,l153);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH1f,l153);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH0f,l153);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH1f,l153);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,DB_TYPE0f,l134);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,DB_TYPE1f,l134);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,DB_TYPE0f,l134);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,DB_TYPE1f,l134);l154+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l155+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(
l1,L3_DEFIPm))/2;l127 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
l154,&l156);SOC_IF_ERROR_RETURN(l127);l127 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l155,&l157);}return l127;}static int l158(
int l1,int l159,void*entry,defip_aux_table_entry_t*l160,int l161){uint32 l134
,l8,l162 = 0;soc_mem_t l20 = L3_DEFIP_PAIR_128m;soc_mem_t l163 = 
L3_DEFIP_AUX_TABLEm;int l127 = SOC_E_NONE,l118,l24;void*l164,*l165;l164 = (
void*)l160;l165 = (void*)(l160+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l163,
MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l159),l160));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l163,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l159),l160+1));l134 = 
soc_mem_field32_get(l1,l20,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l163,
l164,VRF0f,l134);l134 = soc_mem_field32_get(l1,l20,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l163,l164,VRF1f,l134);l134 = soc_mem_field32_get(l1,
l20,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l163,l165,VRF0f,l134);l134 = 
soc_mem_field32_get(l1,l20,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l163,
l165,VRF1f,l134);l134 = soc_mem_field32_get(l1,l20,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l163,l164,MODE0f,l134);l134 = soc_mem_field32_get(l1,
l20,entry,MODE1_LWRf);soc_mem_field32_set(l1,l163,l164,MODE1f,l134);l134 = 
soc_mem_field32_get(l1,l20,entry,MODE0_UPRf);soc_mem_field32_set(l1,l163,l165
,MODE0f,l134);l134 = soc_mem_field32_get(l1,l20,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l163,l165,MODE1f,l134);l134 = soc_mem_field32_get(l1,
l20,entry,VALID0_LWRf);soc_mem_field32_set(l1,l163,l164,VALID0f,l134);l134 = 
soc_mem_field32_get(l1,l20,entry,VALID1_LWRf);soc_mem_field32_set(l1,l163,
l164,VALID1f,l134);l134 = soc_mem_field32_get(l1,l20,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l163,l165,VALID0f,l134);l134 = soc_mem_field32_get(l1,
l20,entry,VALID1_UPRf);soc_mem_field32_set(l1,l163,l165,VALID1f,l134);l127 = 
l125(l1,entry,&l118);SOC_IF_ERROR_RETURN(l127);soc_mem_field32_set(l1,l163,
l164,IP_LENGTH0f,l118);soc_mem_field32_set(l1,l163,l164,IP_LENGTH1f,l118);
soc_mem_field32_set(l1,l163,l165,IP_LENGTH0f,l118);soc_mem_field32_set(l1,
l163,l165,IP_LENGTH1f,l118);l134 = soc_mem_field32_get(l1,l20,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l163,l164,IP_ADDR0f,l134);l134 = 
soc_mem_field32_get(l1,l20,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l163,
l164,IP_ADDR1f,l134);l134 = soc_mem_field32_get(l1,l20,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l163,l165,IP_ADDR0f,l134);l134 = soc_mem_field32_get(
l1,l20,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l163,l165,IP_ADDR1f,l134);
l134 = soc_mem_field32_get(l1,l20,entry,ENTRY_TYPE0_LWRf);soc_mem_field32_set
(l1,l163,l164,ENTRY_TYPE0f,l134);l134 = soc_mem_field32_get(l1,l20,entry,
ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,l163,l164,ENTRY_TYPE1f,l134);l134 = 
soc_mem_field32_get(l1,l20,entry,ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,
l163,l165,ENTRY_TYPE0f,l134);l134 = soc_mem_field32_get(l1,l20,entry,
ENTRY_TYPE1_UPRf);soc_mem_field32_set(l1,l163,l165,ENTRY_TYPE1f,l134);l127 = 
soc_alpm_128_lpm_vrf_get(l1,entry,&l24,&l118);SOC_IF_ERROR_RETURN(l127);if(
SOC_URPF_STATUS_GET(l1)){if(l161>= (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m
)>>1)){l162 = 1;}}switch(l24){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l1
,l163,l164,VALID0f,0);soc_mem_field32_set(l1,l163,l164,VALID1f,0);
soc_mem_field32_set(l1,l163,l165,VALID0f,0);soc_mem_field32_set(l1,l163,l165,
VALID1f,0);l8 = 0;break;case SOC_L3_VRF_GLOBAL:l8 = l162?1:0;break;default:l8
= l162?3:2;break;}if(SOC_MEM_FIELD_VALID(l1,l20,MULTICAST_ROUTEf)){l134 = 
soc_mem_field32_get(l1,l20,entry,MULTICAST_ROUTEf);if(l134){l8 = l162?5:4;}}
soc_mem_field32_set(l1,l163,l164,DB_TYPE0f,l8);soc_mem_field32_set(l1,l163,
l164,DB_TYPE1f,l8);soc_mem_field32_set(l1,l163,l165,DB_TYPE0f,l8);
soc_mem_field32_set(l1,l163,l165,DB_TYPE1f,l8);if(l162){l134 = 
soc_mem_field32_get(l1,l20,entry,ALG_BKT_PTRf);if(l134){l134+= 
SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l20,entry,ALG_BKT_PTRf,l134)
;}}return SOC_E_NONE;}static int l166(int l1,int l167,int index,int l168,void
*entry){defip_aux_table_entry_t l160[2];l168 = soc_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l168,1);SOC_IF_ERROR_RETURN(l158(l1,l168,entry,&l160[0],
index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,
entry));index = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(index),l160));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(index),l160+
1));return SOC_E_NONE;}static int l169(int l1,void*l5,soc_mem_t l20,void*l145
,void*l150,int*l13,int bucket_index,int l140){alpm_pivot_t*l143,*l170,*l171;
defip_aux_scratch_entry_t l10;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l172,l132;uint32 l173[5];int l30,l24,l147;int l119;int l127 = 
SOC_E_NONE,l174;uint32 l8,l148,l153 = 0;int l139 =0;int l152;int l175 = 0;
trie_t*trie,*l176;trie_node_t*l177,*l178 = NULL,*l142 = NULL;payload_t*l179,*
l180,*l181;defip_pair_128_entry_t lpm_entry;alpm_bucket_handle_t*l182;int l126
,l183 = -1,l22 = 0;alpm_mem_prefix_array_t l184;defip_alpm_ipv6_128_entry_t
l185,l186;void*l187,*l188;int*l122 = NULL;trie_t*l141 = NULL;int l189;int l190
;void*l191 = NULL;void*l192 = NULL;void*l193;void*l194;void*l195;int l196 = 0
;int l197 = 0;l30 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l5,&l147,&l24));if(l24 == SOC_VRF_MAX(l1)+1){l8 = 
0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l148);}else{l8 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l148);}l20 = L3_DEFIP_ALPM_IPV6_128m;l187 = 
((uint32*)&(l185));l188 = ((uint32*)&(l186));l127 = l137(l1,l5,prefix,&l132,&
l22);if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: prefix create failed\n")));return l127;}sal_memset(&
l10,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(l1,l5,l30,l8,
0,&l10));if(bucket_index == 0){l127 = l138(l1,prefix,l132,l30,l24,&l139,&l140
,&bucket_index);SOC_IF_ERROR_RETURN(l127);if(l139 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: "
" Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l127 = 
_soc_alpm_insert_in_bkt(l1,l20,bucket_index,l148,l145,l12,&l119,l30);if(l127
== SOC_E_NONE){*l13 = l119;if(SOC_URPF_STATUS_GET(l1)){l174 = soc_mem_write(
l1,l20,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l119),l150);if(SOC_FAILURE(l174))
{return l174;}}}if(l127 == SOC_E_FULL){l175 = 1;}l143 = ALPM_TCAM_PIVOT(l1,
l140);trie = PIVOT_BUCKET_TRIE(l143);l171 = l143;l179 = sal_alloc(sizeof(
payload_t),"Payload for 128b Key");if(l179 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"trie node \n")));return SOC_E_MEMORY;}l180 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l180 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l179);return SOC_E_MEMORY;}sal_memset(l179,0,
sizeof(*l179));sal_memset(l180,0,sizeof(*l180));l179->key[0] = prefix[0];l179
->key[1] = prefix[1];l179->key[2] = prefix[2];l179->key[3] = prefix[3];l179->
key[4] = prefix[4];l179->len = l132;l179->index = l119;sal_memcpy(l180,l179,
sizeof(*l179));l180->bkt_ptr = l179;l127 = trie_insert(trie,prefix,NULL,l132,
(trie_node_t*)l179);if(SOC_FAILURE(l127)){goto l198;}l176 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l24);if(!l22){l127 = trie_insert(l176,prefix,NULL
,l132,(trie_node_t*)l180);}else{l142 = NULL;l127 = trie_find_lpm(l176,0,0,&
l142);l181 = (payload_t*)l142;if(SOC_SUCCESS(l127)){l181->bkt_ptr = l179;}}
l172 = l132;if(SOC_FAILURE(l127)){goto l199;}if(l175){l127 = 
alpm_bucket_assign(l1,&bucket_index,l30);if(SOC_FAILURE(l127)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate"
"new bucket for split\n")));bucket_index = -1;goto l200;}l127 = trie_split(
trie,_MAX_KEY_LEN_144_,FALSE,l173,&l132,&l177,NULL,FALSE);if(SOC_FAILURE(l127
)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Could not split bucket"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l200;}l142 = NULL;l127 = trie_find_lpm(l176,l173,l132,&l142)
;l181 = (payload_t*)l142;if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l173[0],l173[1],l173[
2],l173[3],l173[4],l132));goto l200;}if(l181->bkt_ptr){if(l181->bkt_ptr == 
l179){sal_memcpy(l187,l145,sizeof(defip_alpm_ipv6_128_entry_t));}else{l127 = 
soc_mem_read(l1,l20,MEM_BLOCK_ANY,((payload_t*)l181->bkt_ptr)->index,l187);}
if(SOC_FAILURE(l127)){goto l200;}l127 = l23(l1,l187,l20,l30,l147,bucket_index
,0,&lpm_entry);if(SOC_FAILURE(l127)){goto l200;}l153 = ((payload_t*)(l181->
bkt_ptr))->len;}else{l127 = soc_mem_read(l1,L3_DEFIP_PAIR_128m,MEM_BLOCK_ANY,
soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,SOC_ALPM_128_DEFIP_TO_PAIR(l140>>1
),1),&lpm_entry);}l182 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM 128 Bucket Handle");if(l182 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate "
" memory for PIVOT trie node \n")));l127 = SOC_E_MEMORY;goto l200;}sal_memset
(l182,0,sizeof(*l182));l143 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for new 128b Pivot");if(l143 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate "
"memory for PIVOT trie node \n")));l127 = SOC_E_MEMORY;goto l200;}sal_memset(
l143,0,sizeof(*l143));PIVOT_BUCKET_HANDLE(l143) = l182;l127 = trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l143));PIVOT_BUCKET_TRIE(l143)->trie = 
l177;PIVOT_BUCKET_INDEX(l143) = bucket_index;PIVOT_BUCKET_VRF(l143) = l24;
PIVOT_BUCKET_IPV6(l143) = l30;PIVOT_BUCKET_DEF(l143) = FALSE;l143->key[0] = 
l173[0];l143->key[1] = l173[1];l143->key[2] = l173[2];l143->key[3] = l173[3];
l143->key[4] = l173[4];l143->len = l132;l141 = VRF_PIVOT_TRIE_IPV6_128(l1,l24
);l130((l173),(l132),(l30));l26(l1,l173,l132,l24,l30,&lpm_entry,0,0);
soc_L3_DEFIP_PAIR_128m_field32_set(l1,&lpm_entry,ALG_BKT_PTRf,bucket_index);
sal_memset(&l184,0,sizeof(l184));l127 = trie_traverse(PIVOT_BUCKET_TRIE(l143)
,alpm_mem_prefix_array_cb,&l184,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l127))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_128_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l200;}l122 = sal_alloc(sizeof(*l122)*l184.count,
"Temp storage for location of prefixes in new 128b bucket");if(l122 == NULL){
l127 = SOC_E_MEMORY;goto l200;}sal_memset(l122,-1,sizeof(*l122)*l184.count);
l189 = sizeof(defip_alpm_raw_entry_t);l190 = l189*ALPM_RAW_BKT_COUNT_DW;l192 = 
sal_alloc(4*l190,"Raw memory buffer");if(l192 == NULL){l127 = SOC_E_MEMORY;
goto l200;}sal_memset(l192,0,4*l190);l193 = (uint8*)l192+l190;l194 = (uint8*)
l192+l190*2;l195 = (uint8*)l192+l190*3;l127 = _soc_alpm_raw_bucket_read(l1,
l20,PIVOT_BUCKET_INDEX(l171),l192,l193);if(SOC_FAILURE(l127)){goto l200;}for(
l126 = 0;l126<l184.count;l126++){payload_t*l118 = l184.prefix[l126];if(l118->
index>0){SOC_ALPM_RAW_INDEX_DECODE(l1,l30,l118->index,l196,l197);l191 = (
uint8*)l192+l196*l189;_soc_alpm_raw_mem_read(l1,l20,l191,l197,l187);
_soc_alpm_raw_mem_write(l1,l20,l191,l197,soc_mem_entry_null(l1,l20));if(
SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE(l1,l30,_soc_alpm_rpf_entry
(l1,l118->index),l196,l197);l191 = (uint8*)l193+l196*l189;
_soc_alpm_raw_mem_read(l1,l20,l191,l197,l188);_soc_alpm_raw_mem_write(l1,l20,
l191,l197,soc_mem_entry_null(l1,l20));}l127 = _soc_alpm_mem_index(l1,l20,
bucket_index,l126,l148,&l119);if(SOC_SUCCESS(l127)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l30,l119,l196,l197);l191 = (uint8*)l194+l196*l189;_soc_alpm_raw_mem_write
(l1,l20,l191,l197,l187);if(SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l30,_soc_alpm_rpf_entry(l1,l119),l196,l197);l191 = (uint8*)l195+l196*l189
;_soc_alpm_raw_mem_write(l1,l20,l191,l197,l188);}}}else{l127 = 
_soc_alpm_mem_index(l1,l20,bucket_index,l126,l148,&l119);if(SOC_SUCCESS(l127)
){l183 = l126;*l13 = l119;_soc_alpm_raw_parity_set(l1,l20,l145);
SOC_ALPM_RAW_INDEX_DECODE(l1,l30,l119,l196,l197);l191 = (uint8*)l194+l196*
l189;_soc_alpm_raw_mem_write(l1,l20,l191,l197,l145);if(SOC_URPF_STATUS_GET(l1
)){_soc_alpm_raw_parity_set(l1,l20,l150);SOC_ALPM_RAW_INDEX_DECODE(l1,l30,
_soc_alpm_rpf_entry(l1,l119),l196,l197);l191 = (uint8*)l195+l196*l189;
_soc_alpm_raw_mem_write(l1,l20,l191,l197,l150);}}}l122[l126] = l119;}l127 = 
_soc_alpm_raw_bucket_write(l1,l20,bucket_index,l148,(void*)l194,(void*)l195,
l184.count);if(SOC_FAILURE(l127)){goto l201;}l127 = l2(l1,&lpm_entry,&l152);
if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Unable to add new ""pivot to tcam\n")));if(l127 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l24,l30);}goto l201;}l152 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l152,l30);l127 = l151(l1,l152,
l153);if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),l152));goto l202
;}l127 = trie_insert(l141,l143->key,NULL,l143->len,(trie_node_t*)l143);if(
SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));goto l202;}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l152)<<1) = l143;PIVOT_TCAM_INDEX(l143) = 
SOC_ALPM_128_ADDR_LWR(l152)<<1;VRF_PIVOT_REF_INC(l1,l24,l30);for(l126 = 0;
l126<l184.count;l126++){l184.prefix[l126]->index = l122[l126];}sal_free(l122)
;l127 = _soc_alpm_raw_bucket_write(l1,l20,PIVOT_BUCKET_INDEX(l171),l148,(void
*)l192,(void*)l193,-1);if(SOC_FAILURE(l127)){goto l202;}sal_free(l192);if(
l183 == -1){l127 = _soc_alpm_insert_in_bkt(l1,l20,PIVOT_BUCKET_HANDLE(l171)->
bucket_index,l148,l145,l12,&l119,l30);if(SOC_FAILURE(l127)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: Could not insert new "
"prefix into trie after split\n")));goto l200;}if(SOC_URPF_STATUS_GET(l1)){
l127 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l119),l150);
}*l13 = l119;l179->index = l119;}PIVOT_BUCKET_ENT_CNT_UPDATE(l143);
VRF_BUCKET_SPLIT_INC(l1,l24,l30);}VRF_TRIE_ROUTES_INC(l1,l24,l30);if(l22){
sal_free(l180);}SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l10
,TRUE,&l139,&l140,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,
INSERT_PROPAGATE,&l10,FALSE,&l139,&l140,&bucket_index));if(
SOC_URPF_STATUS_GET(l1)){l132 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,
&l10,DB_TYPEf);l132+= 1;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l10,
DB_TYPEf,l132);SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,
TRUE,&l139,&l140,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,
INSERT_PROPAGATE,&l10,FALSE,&l139,&l140,&bucket_index));}
PIVOT_BUCKET_ENT_CNT_UPDATE(l171);return l127;l202:l174 = l4(l1,&lpm_entry);
if(SOC_FAILURE(l174)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Failure to free new prefix""at %d\n"),
soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,l152,l30)));}if(ALPM_TCAM_PIVOT(l1
,SOC_ALPM_128_ADDR_LWR(l152)<<1)!= NULL){l174 = trie_delete(l141,l143->key,
l143->len,&l178);if(SOC_FAILURE(l174)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l152)<<1) = NULL;VRF_PIVOT_REF_DEC(
l1,l24,l30);l201:l170 = l171;for(l126 = 0;l126<l184.count;l126++){payload_t*
prefix = l184.prefix[l126];if(l122[l126]!= -1){sal_memset(l187,0,sizeof(
defip_alpm_ipv6_128_entry_t));l174 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,l122[
l126],l187);_soc_trident2_alpm_bkt_view_set(l1,l122[l126],INVALIDm);if(
SOC_FAILURE(l174)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l1)){l174 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l122[l126]),l187);_soc_trident2_alpm_bkt_view_set(l1,
_soc_alpm_rpf_entry(l1,l122[l126]),INVALIDm);if(SOC_FAILURE(l174)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l178 = NULL;l174 = trie_delete(
PIVOT_BUCKET_TRIE(l143),prefix->key,prefix->len,&l178);l179 = (payload_t*)
l178;if(SOC_FAILURE(l174)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: trie delete failure""in bkt move rollback\n")));}if(
prefix->index>0){l174 = trie_insert(PIVOT_BUCKET_TRIE(l170),prefix->key,NULL,
prefix->len,(trie_node_t*)l179);if(SOC_FAILURE(l174)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l179!= NULL){sal_free(l179);}}}if(l183
== -1){l178 = NULL;l174 = trie_delete(PIVOT_BUCKET_TRIE(l170),prefix,l172,&
l178);l179 = (payload_t*)l178;if(SOC_FAILURE(l174)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l179!= 
NULL){sal_free(l179);}}l174 = alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l143)
,l30);if(SOC_FAILURE(l174)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: new bucket release ""failure: %d\n"),
PIVOT_BUCKET_INDEX(l143)));}trie_destroy(PIVOT_BUCKET_TRIE(l143));sal_free(
l182);sal_free(l143);sal_free(l122);sal_free(l192);l178 = NULL;l174 = 
trie_delete(l176,prefix,l172,&l178);l180 = (payload_t*)l178;if(SOC_FAILURE(
l174)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l180){sal_free(
l180);}return l127;l200:if(l122!= NULL){sal_free(l122);}if(l192!= NULL){
sal_free(l192);}l178 = NULL;(void)trie_delete(l176,prefix,l172,&l178);l180 = 
(payload_t*)l178;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l30);}l199:l178 = NULL;(void)trie_delete(trie,prefix,l172,&l178)
;l179 = (payload_t*)l178;l198:if(l179!= NULL){sal_free(l179);}if(l180!= NULL)
{sal_free(l180);}return l127;}static int l26(int l17,uint32*key,int len,int
l24,int l7,defip_pair_128_entry_t*lpm_entry,int l27,int l28){uint32 l134;if(
l28){sal_memset(lpm_entry,0,sizeof(defip_pair_128_entry_t));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l76),(l24&SOC_VRF_MAX(l17)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l77),(l24&SOC_VRF_MAX(l17)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l74),(l24&SOC_VRF_MAX(l17)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l75),(l24&SOC_VRF_MAX(l17)));if(l24 == (SOC_VRF_MAX(
l17)+1)){l134 = 0;}else{l134 = SOC_VRF_MAX(l17);}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l80),(l134));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l81),(l134));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l78),(l134));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l79),(l134));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l53),(key[0]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l54),(key[1]))
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l51),(key[2]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l52),(key[3]))
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l61),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l62),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l59),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l60),(3));l135
(l17,(void*)lpm_entry,len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l72),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l73),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l70),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l71),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l65),((1<<
soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l66),((1<<soc_mem_field_length(l17,L3_DEFIP_PAIR_128m
,MODE_MASK1_LWRf))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l63),((1<<soc_mem_field_length(
l17,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l64),((1<<soc_mem_field_length(l17,L3_DEFIP_PAIR_128m
,MODE_MASK1_UPRf))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l88),((1<<soc_mem_field_length(
l17,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l89),((1<<soc_mem_field_length(l17,L3_DEFIP_PAIR_128m
,ENTRY_TYPE_MASK1_LWRf))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO
(l17,L3_DEFIP_PAIR_128m)),(lpm_entry),(l94[(l17)]->l90),((1<<
soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l91),((1<<soc_mem_field_length(l17,L3_DEFIP_PAIR_128m
,ENTRY_TYPE_MASK1_UPRf))-1));return(SOC_E_NONE);}static int l203(int l1,void*
l5,int bucket_index,int l140,int l119,int l204){alpm_pivot_t*l143;
defip_alpm_ipv6_128_entry_t l185,l205,l186;defip_aux_scratch_entry_t l10;
uint32 l12[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l20;void*l187,*l206,*l188 = 
NULL;int l147;int l7;int l127 = SOC_E_NONE,l174 = SOC_E_NONE;uint32 l207[5],
prefix[5];int l30,l24;uint32 l132;int l208;uint32 l8,l148;int l139,l22 = 0;
trie_t*trie,*l176;uint32 l209;defip_pair_128_entry_t lpm_entry,*l210;
payload_t*l179 = NULL,*l211 = NULL,*l181 = NULL;trie_node_t*l178 = NULL,*l142
= NULL;trie_t*l141 = NULL;l7 = l30 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l5,&l147,&l24));if(l147!= SOC_L3_VRF_OVERRIDE){if
(l24 == SOC_VRF_MAX(l1)+1){l8 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l148);}
else{l8 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l148);}l127 = l137(l1,l5,prefix,
&l132,&l22);if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: prefix create failed\n")));return l127;}if(!
soc_alpm_mode_get(l1)){if(l147!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1
,l24,l30)>1){if(l22){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot v6-128 delete ""default route if other routes are present "
"in this mode"),l24));return SOC_E_PARAM;}}}l8 = 2;}l20 = 
L3_DEFIP_ALPM_IPV6_128m;l187 = ((uint32*)&(l185));SOC_ALPM_LPM_LOCK(l1);if(
bucket_index == 0){l127 = l144(l1,l5,l20,l187,&l140,&bucket_index,&l119,TRUE)
;}else{l127 = l16(l1,l5,l187,0,l20,0,0);}sal_memcpy(&l205,l187,sizeof(l205));
l206 = &l205;if(SOC_FAILURE(l127)){SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_delete: Unable to find "
"prefix for delete\n")));return l127;}l208 = bucket_index;l143 = 
ALPM_TCAM_PIVOT(l1,l140);trie = PIVOT_BUCKET_TRIE(l143);l127 = trie_delete(
trie,prefix,l132,&l178);l179 = (payload_t*)l178;if(l127!= SOC_E_NONE){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l127;}l176 = VRF_PREFIX_TRIE_IPV6_128(l1,l24);
l141 = VRF_PIVOT_TRIE_IPV6_128(l1,l24);if(!l22){l127 = trie_delete(l176,
prefix,l132,&l178);l211 = (payload_t*)l178;if(SOC_FAILURE(l127)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l212;}if(l204){l142 = NULL;l127 = trie_find_lpm(l176,prefix,
l132,&l142);l181 = (payload_t*)l142;if(SOC_SUCCESS(l127)){payload_t*l213 = (
payload_t*)(l181->bkt_ptr);if(l213!= NULL){l209 = l213->len;}else{l209 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l214;}sal_memcpy(l207,prefix,sizeof(
prefix));l130((l207),(l132),(l30));l127 = l26(l1,l207,l209,l24,l7,&lpm_entry,
0,1);l174 = l144(l1,&lpm_entry,l20,l187,&l140,&bucket_index,&l119,TRUE);(void
)l23(l1,l187,l20,l7,l147,bucket_index,0,&lpm_entry);(void)l26(l1,l207,l132,
l24,l7,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(l127)){l188
= ((uint32*)&(l186));l174 = soc_mem_read(l1,l20,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l119),l188);}}if((l209 == 0)&&SOC_FAILURE(l174)){l210 = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24);sal_memcpy(&lpm_entry,l210,sizeof(
lpm_entry));l127 = l26(l1,l207,l132,l24,l7,&lpm_entry,0,1);}if(SOC_FAILURE(
l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l214;}l210 = &lpm_entry;}}else{l142 = 
NULL;l127 = trie_find_lpm(l176,prefix,l132,&l142);l181 = (payload_t*)l142;if(
SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),l24));goto l212;}l181->bkt_ptr = 0;
l209 = 0;l210 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24);}if(l204){l127 = l6(
l1,l210,l30,l8,l209,&l10);if(SOC_FAILURE(l127)){goto l214;}l127 = 
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l139,&l140,&bucket_index);if(
SOC_FAILURE(l127)){goto l214;}if(SOC_URPF_STATUS_GET(l1)){uint32 l134;if(l188
!= NULL){l134 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf);
l134++;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf,l134);l134 = 
soc_mem_field32_get(l1,l20,l188,SRC_DISCARDf);soc_mem_field32_set(l1,l20,&l10
,SRC_DISCARDf,l134);l134 = soc_mem_field32_get(l1,l20,l188,DEFAULTROUTEf);
soc_mem_field32_set(l1,l20,&l10,DEFAULTROUTEf,l134);l127 = _soc_alpm_aux_op(
l1,DELETE_PROPAGATE,&l10,TRUE,&l139,&l140,&bucket_index);}if(SOC_FAILURE(l127
)){goto l214;}}}sal_free(l179);if(!l22){sal_free(l211);}
PIVOT_BUCKET_ENT_CNT_UPDATE(l143);if((l143->len!= 0)&&(trie->trie == NULL)){
uint32 l215[5];sal_memcpy(l215,l143->key,sizeof(l215));l130((l215),(l143->len
),(l7));l26(l1,l215,l143->len,l24,l7,&lpm_entry,0,1);l127 = l4(l1,&lpm_entry)
;if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),l143->key[1],l143->key[2],l143
->key[3],l143->key[4]));}}l127 = _soc_alpm_delete_in_bkt(l1,l20,l208,l148,
l206,l12,&l119,l30);if(!SOC_SUCCESS(l127)){SOC_ALPM_LPM_UNLOCK(l1);l127 = 
SOC_E_FAIL;return l127;}if(SOC_URPF_STATUS_GET(l1)){l127 = 
soc_mem_alpm_delete(l1,l20,SOC_ALPM_RPF_BKT_IDX(l1,l208),MEM_BLOCK_ALL,l148,
l206,l12,&l139);if(!SOC_SUCCESS(l127)){SOC_ALPM_LPM_UNLOCK(l1);l127 = 
SOC_E_FAIL;return l127;}}if((l143->len!= 0)&&(trie->trie == NULL)){l127 = 
alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l143),l30);if(SOC_FAILURE(l127)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(l143)));}l127 = trie_delete(l141,l143->key,l143->len,&l178
);if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l143));sal_free(PIVOT_BUCKET_HANDLE(l143));sal_free(l143);
_soc_trident2_alpm_bkt_view_set(l1,l208<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l30)){_soc_trident2_alpm_bkt_view_set(l1,(l208+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l24,l30);if(VRF_TRIE_ROUTES_CNT(l1,
l24,l30) == 0){l127 = l29(l1,l24,l30);}SOC_ALPM_LPM_UNLOCK(l1);return l127;
l214:l174 = trie_insert(l176,prefix,NULL,l132,(trie_node_t*)l211);if(
SOC_FAILURE(l174)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l212:l174 = trie_insert(trie,prefix,NULL,l132,(
trie_node_t*)l179);if(SOC_FAILURE(l174)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}SOC_ALPM_LPM_UNLOCK(l1);return l127;}int
soc_alpm_128_init(int l1){int l127 = SOC_E_NONE;l127 = soc_alpm_128_lpm_init(
l1);SOC_IF_ERROR_RETURN(l127);return l127;}int soc_alpm_128_state_clear(int l1
){int l126,l127;for(l126 = 0;l126<= SOC_VRF_MAX(l1)+1;l126++){l127 = 
trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,l126),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l127)){trie_destroy(
VRF_PREFIX_TRIE_IPV6_128(l1,l126));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n"),l1,
l126));return l127;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l126)!= NULL){
sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l126));}}return SOC_E_NONE;}int
soc_alpm_128_deinit(int l1){soc_alpm_128_lpm_deinit(l1);SOC_IF_ERROR_RETURN(
soc_alpm_128_state_clear(l1));return SOC_E_NONE;}static int l216(int l1,int
l24,int l30){defip_pair_128_entry_t*lpm_entry,l217;int l218;int index;int l127
= SOC_E_NONE;uint32 key[5] = {0,0,0,0,0};uint32 l132;alpm_bucket_handle_t*
l182;alpm_pivot_t*l143;payload_t*l211;trie_t*l219;trie_t*l220 = NULL;
trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(l1,l24));l220 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l24);trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6_128(l1,l24));l219 = VRF_PREFIX_TRIE_IPV6_128(l1,l24);
lpm_entry = sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(
lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: unable to allocate memory ""for IPv6-128 LPM entry\n")
));return SOC_E_MEMORY;}l26(l1,key,0,l24,l30,lpm_entry,0,1);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24) = lpm_entry;if(l24 == SOC_VRF_MAX(l1)
+1){soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,GLOBAL_ROUTEf,1);}else{
soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,DEFAULT_MISSf,1);}l127 = 
alpm_bucket_assign(l1,&l218,l30);if(SOC_FAILURE(l127)){return l127;}
soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,ALG_BKT_PTRf,l218);sal_memcpy
(&l217,lpm_entry,sizeof(l217));l127 = l2(l1,&l217,&index);if(SOC_FAILURE(l127
)){return l127;}l182 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l182 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_128_vrf_add: Unable to allocate memory ""for bucket handle \n"))
);return SOC_E_NONE;}sal_memset(l182,0,sizeof(*l182));l143 = sal_alloc(sizeof
(alpm_pivot_t),"Payload for Pivot");if(l143 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: Unable to allocate memory ""for PIVOT trie node \n")))
;sal_free(l182);return SOC_E_MEMORY;}l211 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l211 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_128_vrf_add: Unable to allocate memory "
"for pfx trie node \n")));sal_free(l182);sal_free(l143);return SOC_E_MEMORY;}
sal_memset(l143,0,sizeof(*l143));sal_memset(l211,0,sizeof(*l211));l132 = 0;
PIVOT_BUCKET_HANDLE(l143) = l182;trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l143));PIVOT_BUCKET_INDEX(l143) = l218;PIVOT_BUCKET_VRF(
l143) = l24;PIVOT_BUCKET_IPV6(l143) = l30;PIVOT_BUCKET_DEF(l143) = TRUE;l143
->key[0] = l211->key[0] = key[0];l143->key[1] = l211->key[1] = key[1];l143->
key[2] = l211->key[2] = key[2];l143->key[3] = l211->key[3] = key[3];l143->key
[4] = l211->key[4] = key[4];l143->len = l211->len = l132;l127 = trie_insert(
l219,key,NULL,l132,&(l211->node));if(SOC_FAILURE(l127)){sal_free(l211);
sal_free(l143);sal_free(l182);return l127;}l127 = trie_insert(l220,key,NULL,
l132,(trie_node_t*)l143);if(SOC_FAILURE(l127)){trie_node_t*l178 = NULL;(void)
trie_delete(l219,key,l132,&l178);sal_free(l211);sal_free(l143);sal_free(l182)
;return l127;}index = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,l30);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(index)<<1) = l143;PIVOT_TCAM_INDEX(
l143) = SOC_ALPM_128_ADDR_LWR(index)<<1;VRF_PIVOT_REF_INC(l1,l24,l30);
VRF_TRIE_INIT_DONE(l1,l24,l30,1);return l127;}static int l29(int l1,int l24,
int l30){defip_pair_128_entry_t*lpm_entry;int l218;int l221;int l127 = 
SOC_E_NONE;uint32 key[2] = {0,0},l131[SOC_MAX_MEM_FIELD_WORDS];payload_t*l179
;alpm_pivot_t*l222;trie_node_t*l178;trie_t*l219;trie_t*l220 = NULL;lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24);l218 = 
soc_L3_DEFIP_PAIR_128m_field32_get(l1,lpm_entry,ALG_BKT_PTRf);l127 = 
alpm_bucket_release(l1,l218,l30);_soc_trident2_alpm_bkt_view_set(l1,l218<<2,
INVALIDm);if(SOC_ALPM_V6_SCALE_CHECK(l1,l30)){_soc_trident2_alpm_bkt_view_set
(l1,(l218+1)<<2,INVALIDm);}l127 = l15(l1,lpm_entry,(void*)l131,&l221);if(
SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n"),
l24,l30));l221 = -1;}l221 = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l221,
l30);l222 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l221)<<1);l127 = l4(l1,
lpm_entry);if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),l24,l30));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24) = NULL;l219 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l24);VRF_PREFIX_TRIE_IPV6_128(l1,l24) = NULL;
VRF_TRIE_INIT_DONE(l1,l24,l30,0);l127 = trie_delete(l219,key,0,&l178);l179 = 
(payload_t*)l178;if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),l24,l30));}
sal_free(l179);(void)trie_destroy(l219);l220 = VRF_PIVOT_TRIE_IPV6_128(l1,l24
);VRF_PIVOT_TRIE_IPV6_128(l1,l24) = NULL;l178 = NULL;l127 = trie_delete(l220,
key,0,&l178);if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l24,l30));}(void)
trie_destroy(l220);(void)trie_destroy(PIVOT_BUCKET_TRIE(l222));sal_free(
PIVOT_BUCKET_HANDLE(l222));sal_free(l222);return l127;}int soc_alpm_128_insert
(int l1,void*l3,uint32 l21,int l223,int l224){defip_alpm_ipv6_128_entry_t l185
,l186;soc_mem_t l20;void*l187,*l206;int l147,l24;int index;int l7;int l127 = 
SOC_E_NONE;uint32 l22;l7 = L3_DEFIP_MODE_128;l20 = L3_DEFIP_ALPM_IPV6_128m;
l187 = ((uint32*)&(l185));l206 = ((uint32*)&(l186));SOC_IF_ERROR_RETURN(l16(
l1,l3,l187,l206,l20,l21,&l22));SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(
l1,l3,&l147,&l24));if(l147 == SOC_L3_VRF_OVERRIDE){l127 = l2(l1,l3,&index);if
(SOC_SUCCESS(l127)){VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l7);VRF_PIVOT_REF_INC(
l1,MAX_VRF_ID,l7);}else if(l127 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,
MAX_VRF_ID,l7);}return(l127);}else if(l24 == 0){if(soc_alpm_mode_get(l1)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}}if(l147!= 
SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l1)){if(VRF_TRIE_ROUTES_CNT(l1,l24,
l7) == 0){if(!l22){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
l147));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l24,l7)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d is not ""initialized\n"),l24));l127 = l216(l1,l24
,l7);if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),l24,l7));return l127;
}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d/%d trie init ""completed\n"),l24,l7));}if(l224&
SOC_ALPM_LOOKUP_HIT){l127 = l149(l1,l3,l187,l206,l20,l223);}else{if(l223 == -
1){l223 = 0;}l127 = l169(l1,l3,l20,l187,l206,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l223),l224);}if(l127!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_128_insert: ""Route Insertion Failed :%s\n"),l1,soc_errmsg
(l127)));}return(l127);}int soc_alpm_128_lookup(int l1,void*l5,void*l12,int*
l13,int*l225){defip_alpm_ipv6_128_entry_t l185;soc_mem_t l20;int bucket_index
;int l140;void*l187;int l147,l24;int l7 = 2,l118;int l127 = SOC_E_NONE;
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l5,&l147,&l24));l127 = l11(l1
,l5,l12,l13,&l118,&l7);if(SOC_SUCCESS(l127)){SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l12,&l147,&l24));if(l147 == SOC_L3_VRF_OVERRIDE){
return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l24,l7)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_lookup:VRF %d is not "
"initialized\n"),l24));*l13 = 0;*l225 = 0;return SOC_E_NOT_FOUND;}l20 = 
L3_DEFIP_ALPM_IPV6_128m;l187 = ((uint32*)&(l185));SOC_ALPM_LPM_LOCK(l1);l127 = 
l144(l1,l5,l20,l187,&l140,&bucket_index,l13,TRUE);SOC_ALPM_LPM_UNLOCK(l1);if(
SOC_FAILURE(l127)){*l225 = l140;*l13 = bucket_index<<2;return l127;}l127 = 
l23(l1,l187,l20,l7,l147,bucket_index,*l13,l12);*l225 = SOC_ALPM_LOOKUP_HIT|
l140;return(l127);}int l226(int l1,void*l5,void*l12,int l24,int*l140,int*
bucket_index,int*l119,int l227){int l127 = SOC_E_NONE;int l126,l228,l30,l139 = 
0;uint32 l8,l148;defip_aux_scratch_entry_t l10;int l229,l230;int index;
soc_mem_t l20,l231;int l232,l233,l234;soc_field_t l235[4] = {IP_ADDR0_LWRf,
IP_ADDR1_LWRf,IP_ADDR0_UPRf,IP_ADDR1_UPRf};uint32 l236[
SOC_MAX_MEM_FIELD_WORDS] = {0};int l237 = -1;int l238 = 0;l30 = 
L3_DEFIP_MODE_128;l231 = L3_DEFIP_PAIR_128m;l229 = soc_mem_field32_get(l1,
l231,l5,GLOBAL_ROUTEf);l230 = soc_mem_field32_get(l1,l231,l5,VRF_ID_0_LWRf);
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d\n"),l24 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l30,l229,l230));if(l24 == SOC_L3_VRF_GLOBAL){l8 = l227?1:0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l148);soc_mem_field32_set(l1,l231,l5,
GLOBAL_ROUTEf,1);soc_mem_field32_set(l1,l231,l5,VRF_ID_0_LWRf,0);}else{l8 = 
l227?3:2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l148);}sal_memset(&l10,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(l1,l5,l30,l8,0,&l10));if(
l24 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l1,l231,l5,GLOBAL_ROUTEf,l229);
soc_mem_field32_set(l1,l231,l5,VRF_ID_0_LWRf,l230);}SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l10,TRUE,&l139,l140,bucket_index));if(l139
== 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Could not find bucket\n")))
;return SOC_E_NOT_FOUND;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in memory %s, index %d, ""bucket_index %d\n"),SOC_MEM_NAME(l1,l231),
soc_alpm_logical_idx(l1,l231,SOC_ALPM_128_DEFIP_TO_PAIR((*l140)>>1),1),*
bucket_index));l20 = L3_DEFIP_ALPM_IPV6_128m;l127 = l125(l1,l5,&l233);if(
SOC_FAILURE(l127)){return l127;}l234 = SOC_ALPM_V6_SCALE_CHECK(l1,l30)?16:8;
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Start searching mem %s bucket %d(count %d) ""for Length %d\n"),SOC_MEM_NAME(
l1,l20),*bucket_index,l234,l233));for(l126 = 0;l126<l234;l126++){uint32 l187[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l239[4] = {0};uint32 l240[4] = {0};
uint32 l241[4] = {0};int l242;l127 = _soc_alpm_mem_index(l1,l20,*bucket_index
,l126,l148,&index);if(l127 == SOC_E_FULL){continue;}SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l20,MEM_BLOCK_ANY,index,(void*)&l187));l242 = 
soc_mem_field32_get(l1,l20,&l187,VALIDf);l232 = soc_mem_field32_get(l1,l20,&
l187,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket %5d Index %6d: Valid %d, Length %d\n"),*bucket_index,index,l242,l232)
);if(!l242||(l232>l233)){continue;}SHR_BITSET_RANGE(l239,128-l232,l232);(void
)soc_mem_field_get(l1,l20,(uint32*)&l187,KEYf,(uint32*)l240);l241[3] = 
soc_mem_field32_get(l1,l231,l5,l235[3]);l241[2] = soc_mem_field32_get(l1,l231
,l5,l235[2]);l241[1] = soc_mem_field32_get(l1,l231,l5,l235[1]);l241[0] = 
soc_mem_field32_get(l1,l231,l5,l235[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tmask %08x %08x %08x %08x \n""\t key %08x %08x %08x %08x \n"
"\thost %08x %08x %08x %08x \n"),l239[3],l239[2],l239[1],l239[0],l240[3],l240
[2],l240[1],l240[0],l241[3],l241[2],l241[1],l241[0]));for(l228 = 3;l228>= 0;
l228--){if((l241[l228]&l239[l228])!= (l240[l228]&l239[l228])){break;}}if(l228
>= 0){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket %d, ""index %d\n"),SOC_MEM_NAME(l1,l20),*
bucket_index,index));if(l237 == -1||l237<l232){l237 = l232;l238 = index;
sal_memcpy(l236,l187,sizeof(l187));}}if(l237!= -1){l127 = l23(l1,&l236,l20,
l30,l24,*bucket_index,l238,l12);if(SOC_SUCCESS(l127)){*l119 = l238;if(
bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit mem %s bucket %d, index %d\n"),
SOC_MEM_NAME(l1,l20),*bucket_index,l238));}}return l127;}*l119 = 
soc_alpm_logical_idx(l1,l231,SOC_ALPM_128_DEFIP_TO_PAIR((*l140)>>1),1);
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l231,MEM_BLOCK_ANY,*l119,(void*)l12));
return SOC_E_NONE;}int soc_alpm_128_find_best_match(int l1,void*l5,void*l12,
int*l13,int l227){int l127 = SOC_E_NONE;int l126,l228;int l243,l244;
defip_pair_128_entry_t l245;uint32 l246,l240,l241;int l232,l233;int l247,l248
;int l147,l24 = 0;int l140,bucket_index;soc_mem_t l231;soc_field_t l249[4] = 
{IP_ADDR_MASK1_UPRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_LWRf}
;soc_field_t l250[4] = {IP_ADDR1_UPRf,IP_ADDR0_UPRf,IP_ADDR1_LWRf,
IP_ADDR0_LWRf};l231 = L3_DEFIP_PAIR_128m;if(!SOC_URPF_STATUS_GET(l1)&&l227){
return SOC_E_PARAM;}l243 = soc_mem_index_min(l1,l231);l244 = 
soc_mem_index_count(l1,l231);if(SOC_URPF_STATUS_GET(l1)){l244>>= 1;}if(
soc_alpm_mode_get(l1)){l244>>= 1;l243+= l244;}if(l227){l243+= 
soc_mem_index_count(l1,l231)/2;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Start LPM searchng from %d, count %d\n"),l243,l244));for(l126 = l243;l126<
l243+l244;l126++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l231,MEM_BLOCK_ANY,l126
,(void*)&l245));if(!soc_mem_field32_get(l1,l231,&l245,VALID0_LWRf)){continue;
}l247 = soc_mem_field32_get(l1,l231,&l245,GLOBAL_HIGHf);l248 = 
soc_mem_field32_get(l1,l231,&l245,GLOBAL_ROUTEf);if(!l248||!l247){continue;}
l127 = l125(l1,l5,&l233);l127 = l125(l1,&l245,&l232);if(SOC_FAILURE(l127)||(
l232>l233)){continue;}for(l228 = 0;l228<4;l228++){l246 = soc_mem_field32_get(
l1,l231,&l245,l249[l228]);l240 = soc_mem_field32_get(l1,l231,&l245,l250[l228]
);l241 = soc_mem_field32_get(l1,l231,l5,l250[l228]);if((l241&l246)!= (l240&
l246)){break;}}if(l228<4){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Hit Global High route in index = %d\n"),l228));sal_memcpy(l12,&l245,
sizeof(l245));*l13 = l126;return SOC_E_NONE;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for Global Low and VRF routes\n")));SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l5,&l147,&l24));l127 = l226(l1,l5,l12,l24,&l140,&
bucket_index,l13,l227);if(l127 == SOC_E_NOT_FOUND){l24 = SOC_L3_VRF_GLOBAL;
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Not found in VRF region, try Global ""region\n")));l127 = l226(l1,l5,l12,l24
,&l140,&bucket_index,l13,l227);}if(SOC_SUCCESS(l127)){l140 = 
soc_alpm_logical_idx(l1,l231,SOC_ALPM_128_DEFIP_TO_PAIR(l140>>1),1);
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in %s region in TCAM index %d, ""buckekt_index %d\n"),l24 == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",l140,bucket_index));}else{LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Search miss for given address\n")));}return(
l127);}int soc_alpm_128_delete(int l1,void*l5,int l223,int l224){int l147,l24
;int l7;int l127 = SOC_E_NONE;int l204 = 0;l7 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l5,&l147,&l24));if(l147 == 
SOC_L3_VRF_OVERRIDE){l127 = l4(l1,l5);if(SOC_SUCCESS(l127)){
VRF_TRIE_ROUTES_DEC(l1,MAX_VRF_ID,l7);VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l7);}
return(l127);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l24,l7)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_128_delete:VRF %d/%d is not "
"initialized\n"),l24,l7));return SOC_E_NONE;}if(l223 == -1){l223 = 0;}l204 = 
!(l224&SOC_ALPM_DELETE_ALL);l127 = l203(l1,l5,SOC_ALPM_BKT_ENTRY_TO_IDX(l223)
,l224&~SOC_ALPM_LOOKUP_HIT,l223,l204);}return(l127);}static void l106(int l1,
void*l12,int index,l101 l107){l107[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l94[(l1)]->l53));l107[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l94[(l1)]->l51));l107[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l94[(l1)]->l57));l107[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l94[(l1)]->l55));l107[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l94[(l1)]->l54));l107[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l94[(l1)]->l52));l107[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l94[(l1)]->l58));l107[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l94[(l1)]->l56));if((!(SOC_IS_HURRICANE(l1)))&&(((l94[(l1)]->l76)!= NULL))
){int l251;(void)soc_alpm_128_lpm_vrf_get(l1,l12,(int*)&l107[8],&l251);}else{
l107[8] = 0;};}static int l252(l101 l103,l101 l104){int l221;for(l221 = 0;
l221<9;l221++){{if((l103[l221])<(l104[l221])){return-1;}if((l103[l221])>(l104
[l221])){return 1;}};}return(0);}static void l253(int l1,void*l3,uint32 l254,
uint32 l121,int l118){l101 l255;if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l94[(l1)]->l73))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l94[(l1)]->l72))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l94[(l1)]->l71))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l94[(l1)]->l70))){l255[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l94[(l1)]->l53));l255[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l94[(l1)]->l51));l255[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l94[(l1)]->l57));l255[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l94[(l1)]->l55));l255[4] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l94[(l1)]->l54));l255[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l94[(l1)]->l52));l255[6] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l94[(l1)]->l58));l255[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l94[(l1)]->l56));if((!(SOC_IS_HURRICANE(l1)))&&(((l94[(l1)]->l76)!= NULL)))
{int l251;(void)soc_alpm_128_lpm_vrf_get(l1,l3,(int*)&l255[8],&l251);}else{
l255[8] = 0;};l120((l105[(l1)]),l252,l255,l118,l121,l254);}}static void l256(
int l1,void*l5,uint32 l254){l101 l255;int l118 = -1;int l127;uint16 index;
l255[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l5),(l94[(l1)]->l53));l255[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l94[(l1)]->l51));l255[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l94[(l1)]->l57));l255[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l94[(l1)]->l55));l255[4] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l94[(l1)]->l54));l255[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l94[(l1)]->l52));l255[6] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l94[(l1)]->l58));l255[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l94[(l1)]->l56));if((!(SOC_IS_HURRICANE(l1)))&&(((l94[(l1)]->l76)!= NULL)))
{int l251;(void)soc_alpm_128_lpm_vrf_get(l1,l5,(int*)&l255[8],&l251);}else{
l255[8] = 0;};index = l254;l127 = l123((l105[(l1)]),l252,l255,l118,index);if(
SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\ndel  index: H %d error %d\n"),index,l127));}}static int l257(int l1,void*
l5,int l118,int*l119){l101 l255;int l127;uint16 index = (0xFFFF);l255[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l94[(l1)]->l53));l255[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l94[(l1)]->l51));l255[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l94[(l1)]->l57));l255[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l94[(l1)]->l55));l255[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l94[(l1)]->l54));l255[5] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l94[(l1)]->l52));l255[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l94[(l1)]->l58));l255[7] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l94[(l1)]->l56));if((!(SOC_IS_HURRICANE(l1)))
&&(((l94[(l1)]->l76)!= NULL))){int l251;(void)soc_alpm_128_lpm_vrf_get(l1,l5,
(int*)&l255[8],&l251);}else{l255[8] = 0;};l127 = l115((l105[(l1)]),l252,l255,
l118,&index);if(SOC_FAILURE(l127)){*l119 = 0xFFFFFFFF;return(l127);}*l119 = 
index;return(SOC_E_NONE);}static uint16 l108(uint8*l109,int l110){return(
_shr_crc16b(0,l109,l110));}static int l111(int l17,int l96,int l97,l100**l112
){l100*l116;int index;if(l97>l96){return SOC_E_MEMORY;}l116 = sal_alloc(
sizeof(l100),"lpm_hash");if(l116 == NULL){return SOC_E_MEMORY;}sal_memset(
l116,0,sizeof(*l116));l116->l17 = l17;l116->l96 = l96;l116->l97 = l97;l116->
l98 = sal_alloc(l116->l97*sizeof(*(l116->l98)),"hash_table");if(l116->l98 == 
NULL){sal_free(l116);return SOC_E_MEMORY;}l116->l99 = sal_alloc(l116->l96*
sizeof(*(l116->l99)),"link_table");if(l116->l99 == NULL){sal_free(l116->l98);
sal_free(l116);return SOC_E_MEMORY;}for(index = 0;index<l116->l97;index++){
l116->l98[index] = (0xFFFF);}for(index = 0;index<l116->l96;index++){l116->l99
[index] = (0xFFFF);}*l112 = l116;return SOC_E_NONE;}static int l113(l100*l114
){if(l114!= NULL){sal_free(l114->l98);sal_free(l114->l99);sal_free(l114);}
return SOC_E_NONE;}static int l115(l100*l116,l102 l117,l101 entry,int l118,
uint16*l119){int l1 = l116->l17;uint16 l258;uint16 index;l258 = l108((uint8*)
entry,(32*9))%l116->l97;index = l116->l98[l258];;;while(index!= (0xFFFF)){
uint32 l12[SOC_MAX_MEM_FIELD_WORDS];l101 l107;int l259;l259 = index;
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l259,l12));l106(
l1,l12,index,l107);if((*l117)(entry,l107) == 0){*l119 = index;;return(
SOC_E_NONE);}index = l116->l99[index&(0x3FFF)];;};return(SOC_E_NOT_FOUND);}
static int l120(l100*l116,l102 l117,l101 entry,int l118,uint16 l121,uint16
l122){int l1 = l116->l17;uint16 l258;uint16 index;uint16 l260;l258 = l108((
uint8*)entry,(32*9))%l116->l97;index = l116->l98[l258];;;;l260 = (0xFFFF);if(
l121!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l12[SOC_MAX_MEM_FIELD_WORDS];
l101 l107;int l259;l259 = index;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(
l1,MEM_BLOCK_ANY,l259,l12));l106(l1,l12,index,l107);if((*l117)(entry,l107) == 
0){if(l122!= index){;if(l260 == (0xFFFF)){l116->l98[l258] = l122;l116->l99[
l122&(0x3FFF)] = l116->l99[index&(0x3FFF)];l116->l99[index&(0x3FFF)] = (
0xFFFF);}else{l116->l99[l260&(0x3FFF)] = l122;l116->l99[l122&(0x3FFF)] = l116
->l99[index&(0x3FFF)];l116->l99[index&(0x3FFF)] = (0xFFFF);}};return(
SOC_E_NONE);}l260 = index;index = l116->l99[index&(0x3FFF)];;}}l116->l99[l122
&(0x3FFF)] = l116->l98[l258];l116->l98[l258] = l122;return(SOC_E_NONE);}
static int l123(l100*l116,l102 l117,l101 entry,int l118,uint16 l124){uint16
l258;uint16 index;uint16 l260;l258 = l108((uint8*)entry,(32*9))%l116->l97;
index = l116->l98[l258];;;l260 = (0xFFFF);while(index!= (0xFFFF)){if(l124 == 
index){;if(l260 == (0xFFFF)){l116->l98[l258] = l116->l99[l124&(0x3FFF)];l116
->l99[l124&(0x3FFF)] = (0xFFFF);}else{l116->l99[l260&(0x3FFF)] = l116->l99[
l124&(0x3FFF)];l116->l99[l124&(0x3FFF)] = (0xFFFF);}return(SOC_E_NONE);}l260 = 
index;index = l116->l99[index&(0x3FFF)];;}return(SOC_E_NOT_FOUND);}static int
l261(int l1,void*l12){return(SOC_E_NONE);}void soc_alpm_128_lpm_state_dump(
int l1){int l126;int l262;l262 = ((3*(128+2+1))-1);if(!bsl_check(bslLayerSoc,
bslSourceAlpm,bslSeverityVerbose,l1)){return;}for(l126 = l262;l126>= 0;l126--
){if((l126!= ((3*(128+2+1))-1))&&((l39[(l1)][(l126)].l32) == -1)){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"PFX = %d P = %d N = %d START = %d ""END = %d VENT = %d FENT = %d\n"),l126,(
l39[(l1)][(l126)].l34),(l39[(l1)][(l126)].next),(l39[(l1)][(l126)].l32),(l39[
(l1)][(l126)].l33),(l39[(l1)][(l126)].l35),(l39[(l1)][(l126)].l36)));}
COMPILER_REFERENCE(l261);}static int l263(int l1,int index,uint32*l12){int
l264;uint32 l265,l266,l267;uint32 l268;int l269;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l264 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}else if(SOC_IS_APOLLO(l1)){
l264 = (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l264 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}if(((l94[(l1)]->l42)!= NULL))
{soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
l12),(l94[(l1)]->l42),(0));}l265 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l94[(l1)]->l57));l268 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l94[(l1)]->l58));l266 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l12),(l94[(l1)]->l55));l267 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l94[(l1)]->l56));l269 = ((!l265)&&(!l268)&&(!l266)&&(!l267))?1:0;l265 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l94[(l1)]->l72));l268 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l12),(l94[(l1)]->l70));l266 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l94[(l1)]->l71));l267 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l12),(l94[(l1)]->l71));if(l265&&l268&&l266&&l267){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l94[(l1)]->l69),(l269));}return l166(l1,MEM_BLOCK_ANY,index+l264,index,l12
);}static int l270(int l1,int l271,int l272){uint32 l12[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l271,l12));l253(l1,l12,l272,0x4000,0);SOC_IF_ERROR_RETURN(l166(
l1,MEM_BLOCK_ANY,l272,l271,l12));SOC_IF_ERROR_RETURN(l263(l1,l272,l12));do{
int l273,l274;l273 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l271),1);
l274 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l272),1);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l274))<<1) = ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l273))<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR
((l274))<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l274
))<<1)) = SOC_ALPM_128_ADDR_LWR((l274))<<1;}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l273))<<1) = NULL;}while(0);return(SOC_E_NONE);}static
int l275(int l1,int l118,int l7){int l271;int l272;l272 = (l39[(l1)][(l118)].
l33)+1;l271 = (l39[(l1)][(l118)].l32);if(l271!= l272){SOC_IF_ERROR_RETURN(
l270(l1,l271,l272));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l7);}(l39[(l1)][(l118)]
.l32)+= 1;(l39[(l1)][(l118)].l33)+= 1;return(SOC_E_NONE);}static int l276(int
l1,int l118,int l7){int l271;int l272;l272 = (l39[(l1)][(l118)].l32)-1;if((
l39[(l1)][(l118)].l35) == 0){(l39[(l1)][(l118)].l32) = l272;(l39[(l1)][(l118)
].l33) = l272-1;return(SOC_E_NONE);}l271 = (l39[(l1)][(l118)].l33);
SOC_IF_ERROR_RETURN(l270(l1,l271,l272));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l7)
;(l39[(l1)][(l118)].l32)-= 1;(l39[(l1)][(l118)].l33)-= 1;return(SOC_E_NONE);}
static int l277(int l1,int l118,int l7,void*l12,int*l278){int l279;int l280;
int l281;int l282;if((l39[(l1)][(l118)].l35) == 0){l282 = ((3*(128+2+1))-1);
if(soc_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL){if(l118<= (((3*(128+2+1))
/3)-1)){l282 = (((3*(128+2+1))/3)-1);}}while((l39[(l1)][(l282)].next)>l118){
l282 = (l39[(l1)][(l282)].next);}l280 = (l39[(l1)][(l282)].next);if(l280!= -1
){(l39[(l1)][(l280)].l34) = l118;}(l39[(l1)][(l118)].next) = (l39[(l1)][(l282
)].next);(l39[(l1)][(l118)].l34) = l282;(l39[(l1)][(l282)].next) = l118;(l39[
(l1)][(l118)].l36) = ((l39[(l1)][(l282)].l36)+1)/2;(l39[(l1)][(l282)].l36)-= 
(l39[(l1)][(l118)].l36);(l39[(l1)][(l118)].l32) = (l39[(l1)][(l282)].l33)+(
l39[(l1)][(l282)].l36)+1;(l39[(l1)][(l118)].l33) = (l39[(l1)][(l118)].l32)-1;
(l39[(l1)][(l118)].l35) = 0;}l281 = l118;while((l39[(l1)][(l281)].l36) == 0){
l281 = (l39[(l1)][(l281)].next);if(l281 == -1){l281 = l118;break;}}while((l39
[(l1)][(l281)].l36) == 0){l281 = (l39[(l1)][(l281)].l34);if(l281 == -1){if((
l39[(l1)][(l118)].l35) == 0){l279 = (l39[(l1)][(l118)].l34);l280 = (l39[(l1)]
[(l118)].next);if(-1!= l279){(l39[(l1)][(l279)].next) = l280;}if(-1!= l280){(
l39[(l1)][(l280)].l34) = l279;}}return(SOC_E_FULL);}}while(l281>l118){l280 = 
(l39[(l1)][(l281)].next);SOC_IF_ERROR_RETURN(l276(l1,l280,l7));(l39[(l1)][(
l281)].l36)-= 1;(l39[(l1)][(l280)].l36)+= 1;l281 = l280;}while(l281<l118){
SOC_IF_ERROR_RETURN(l275(l1,l281,l7));(l39[(l1)][(l281)].l36)-= 1;l279 = (l39
[(l1)][(l281)].l34);(l39[(l1)][(l279)].l36)+= 1;l281 = l279;}(l39[(l1)][(l118
)].l35)+= 1;(l39[(l1)][(l118)].l36)-= 1;(l39[(l1)][(l118)].l33)+= 1;*l278 = (
l39[(l1)][(l118)].l33);sal_memcpy(l12,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);return(
SOC_E_NONE);}static int l283(int l1,int l118,int l7,void*l12,int l284){int
l279;int l280;int l271;int l272;uint32 l285[SOC_MAX_MEM_FIELD_WORDS];int l127
;int l134;l271 = (l39[(l1)][(l118)].l33);l272 = l284;(l39[(l1)][(l118)].l35)
-= 1;(l39[(l1)][(l118)].l36)+= 1;(l39[(l1)][(l118)].l33)-= 1;if(l272!= l271){
if((l127 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l271,l285))<0){return l127
;}l253(l1,l285,l272,0x4000,0);if((l127 = l166(l1,MEM_BLOCK_ANY,l272,l271,l285
))<0){return l127;}if((l127 = l263(l1,l272,l285))<0){return l127;}}l134 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l272,1);l284 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l271,1);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l134)<<1) = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
l284)<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l134)<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l134)<<1)) = 
SOC_ALPM_128_ADDR_LWR(l134)<<1;}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l284
)<<1) = NULL;sal_memcpy(l285,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),
soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);l272 = l271;l253(l1,l285,l272,
0x4000,0);if((l127 = l166(l1,MEM_BLOCK_ANY,l272,l271,l285))<0){return l127;}
if((l127 = l263(l1,l271,l285))<0){return l127;}if((l39[(l1)][(l118)].l35) == 
0){l279 = (l39[(l1)][(l118)].l34);assert(l279!= -1);l280 = (l39[(l1)][(l118)]
.next);(l39[(l1)][(l279)].next) = l280;(l39[(l1)][(l279)].l36)+= (l39[(l1)][(
l118)].l36);(l39[(l1)][(l118)].l36) = 0;if(l280!= -1){(l39[(l1)][(l280)].l34)
= l279;}(l39[(l1)][(l118)].next) = -1;(l39[(l1)][(l118)].l34) = -1;(l39[(l1)]
[(l118)].l32) = -1;(l39[(l1)][(l118)].l33) = -1;}return(l127);}int
soc_alpm_128_lpm_vrf_get(int l17,void*lpm_entry,int*l24,int*l286){int l147;if
(((l94[(l17)]->l80)!= NULL)){l147 = soc_L3_DEFIP_PAIR_128m_field32_get(l17,
lpm_entry,VRF_ID_0_LWRf);*l286 = l147;if(soc_L3_DEFIP_PAIR_128m_field32_get(
l17,lpm_entry,VRF_ID_MASK0_LWRf)){*l24 = l147;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l94[(l17)]->l82))){*l24 = SOC_L3_VRF_GLOBAL;*l286 = SOC_VRF_MAX(
l17)+1;}else{*l24 = SOC_L3_VRF_OVERRIDE;}}else{*l24 = SOC_L3_VRF_DEFAULT;}
return(SOC_E_NONE);}static int l287(int l1,void*entry,int*l14){int l118=0;int
l127;int l147;int l288;l127 = l125(l1,entry,&l118);if(l127<0){return l127;}
l118+= 0;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,entry,&l147,&l127));
l288 = soc_alpm_mode_get(l1);switch(l147){case SOC_L3_VRF_GLOBAL:if(l288 == 
SOC_ALPM_MODE_PARALLEL){*l14 = l118+((3*(128+2+1))/3);}else{*l14 = l118;}
break;case SOC_L3_VRF_OVERRIDE:*l14 = l118+2*((3*(128+2+1))/3);break;default:
if(l288 == SOC_ALPM_MODE_PARALLEL){*l14 = l118;}else{*l14 = l118+((3*(128+2+1
))/3);}break;}return(SOC_E_NONE);}static int l11(int l1,void*l5,void*l12,int*
l13,int*l14,int*l7){int l127;int l119;int l118 = 0;*l7 = L3_DEFIP_MODE_128;
l287(l1,l5,&l118);*l14 = l118;if(l257(l1,l5,l118,&l119) == SOC_E_NONE){*l13 = 
l119;if((l127 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(*l7)?*l13:(*l13>>1)
,l12))<0){return l127;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}int
soc_alpm_128_ipmc_war(int l17,int l289){int index = -1;defip_pair_128_entry_t
lpm_entry;soc_mem_t l20 = L3_DEFIP_PAIR_128m;int l290 = 63;if(!
soc_property_get(l17,spn_L3_ALPM_ENABLE,0)){return SOC_E_NONE;}if(
soc_mem_index_count(l17,L3_DEFIP_PAIR_128m)<= 0){return SOC_E_NONE;}if(!
SOC_IS_TRIDENT2PLUS(l17)){return SOC_E_NONE;}if(SOC_WARM_BOOT(l17)){return
SOC_E_NONE;}l290 = soc_mem_field_length(l17,ING_ACTIVE_L3_IIF_PROFILEm,
RPA_ID_PROFILEf);sal_memset(&lpm_entry,0,soc_mem_entry_words(l17,l20)*4);
soc_mem_field32_set(l17,l20,&lpm_entry,VALID0_LWRf,1);soc_mem_field32_set(l17
,l20,&lpm_entry,VALID1_LWRf,1);soc_mem_field32_set(l17,l20,&lpm_entry,
VALID0_UPRf,1);soc_mem_field32_set(l17,l20,&lpm_entry,VALID1_UPRf,1);
soc_mem_field32_set(l17,l20,&lpm_entry,MODE0_LWRf,3);soc_mem_field32_set(l17,
l20,&lpm_entry,MODE1_LWRf,3);soc_mem_field32_set(l17,l20,&lpm_entry,
MODE0_UPRf,3);soc_mem_field32_set(l17,l20,&lpm_entry,MODE1_UPRf,3);
soc_mem_field32_set(l17,l20,&lpm_entry,MODE_MASK0_LWRf,3);soc_mem_field32_set
(l17,l20,&lpm_entry,MODE_MASK1_LWRf,3);soc_mem_field32_set(l17,l20,&lpm_entry
,MODE_MASK0_UPRf,3);soc_mem_field32_set(l17,l20,&lpm_entry,MODE_MASK1_UPRf,3)
;soc_mem_field32_set(l17,l20,&lpm_entry,ENTRY_TYPE_MASK0_LWRf,1);
soc_mem_field32_set(l17,l20,&lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);
soc_mem_field32_set(l17,l20,&lpm_entry,ENTRY_TYPE_MASK0_UPRf,1);
soc_mem_field32_set(l17,l20,&lpm_entry,ENTRY_TYPE_MASK1_UPRf,1);
soc_mem_field32_set(l17,l20,&lpm_entry,MULTICAST_ROUTEf,1);
soc_mem_field32_set(l17,l20,&lpm_entry,GLOBAL_ROUTEf,1);soc_mem_field32_set(
l17,l20,&lpm_entry,RPA_IDf,l290-1);soc_mem_field32_set(l17,l20,&lpm_entry,
EXPECTED_L3_IIFf,16383);soc_mem_field32_set(l17,l20,&lpm_entry,IP_ADDR0_LWRf,
0);soc_mem_field32_set(l17,l20,&lpm_entry,IP_ADDR1_LWRf,0);
soc_mem_field32_set(l17,l20,&lpm_entry,IP_ADDR0_UPRf,0);soc_mem_field32_set(
l17,l20,&lpm_entry,IP_ADDR1_UPRf,0xff000000);soc_mem_field32_set(l17,l20,&
lpm_entry,IP_ADDR_MASK0_LWRf,0);soc_mem_field32_set(l17,l20,&lpm_entry,
IP_ADDR_MASK1_UPRf,0);soc_mem_field32_set(l17,l20,&lpm_entry,
IP_ADDR_MASK0_LWRf,0);soc_mem_field32_set(l17,l20,&lpm_entry,
IP_ADDR_MASK1_UPRf,0xff000000);if(l289){SOC_IF_ERROR_RETURN(l2(l17,&lpm_entry
,&index));}else{SOC_IF_ERROR_RETURN(l4(l17,&lpm_entry));}return SOC_E_NONE;}
int soc_alpm_128_lpm_init(int l1){int l262;int l126;int l291;int l292;if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}l262 = (3*(128+2
+1));l292 = sizeof(l37)*(l262);if((l39[(l1)]!= NULL)){SOC_IF_ERROR_RETURN(
soc_alpm_128_deinit(l1));}l94[l1] = sal_alloc(sizeof(l92),
"lpm_128_field_state");if(NULL == l94[l1]){return(SOC_E_MEMORY);}(l94[l1])->
l41 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l94[l1])->l42 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l94[l1])->l43 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l94[l1])->l44 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l94[l1])->l45 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l94[l1])->l46 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l94[l1])->l47 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MULTICAST_ROUTEf);(l94[l1])->l48 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPA_IDf);(l94[l1])->l49 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,EXPECTED_L3_IIFf);(l94[l1])->l50 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l94[l1])->l53 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l94[l1])->l51 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l94[l1])->l54 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l94[l1])->l52 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l94[l1])->l57 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l94[l1])->
l55 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l94[l1
])->l58 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l94[l1])->l56 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l94[l1])->l61 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l94[l1])->l59 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l94[l1])->l62 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l94[l1])->l60 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l94[l1])->l65 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l94[l1])->l63 = soc_mem_fieldinfo_get(l1
,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l94[l1])->l66 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l94[l1])->l64 = soc_mem_fieldinfo_get
(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l94[l1])->l67 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l94[l1])->l68 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l94[l1])->l69 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l94[l1])->l72 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l94[l1])->l70 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l94[l1])->l73 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l94[l1])->l71 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l94[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l94[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l94[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l94[l1])->l75 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l94[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l94[l1])->l78
= soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l94[l1])->
l81 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(l94[l1]
)->l79 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_UPRf);(l94[
l1])->l82 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l94[l1
])->l83 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l94[l1])
->l84 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l94[l1])->
l85 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l94[l1])->
l86 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf)
;(l94[l1])->l87 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
FLEX_CTR_POOL_NUMBERf);(l94[l1])->l88 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf);(l94[l1])->l89 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_LWRf);(l94[l1])
->l90 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf);(
l94[l1])->l91 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
ENTRY_TYPE_MASK1_UPRf);(l39[(l1)]) = sal_alloc(l292,"LPM 128 prefix info");if
(NULL == (l39[(l1)])){sal_free(l94[l1]);l94[l1] = NULL;return(SOC_E_MEMORY);}
SOC_ALPM_LPM_LOCK(l1);sal_memset((l39[(l1)]),0,l292);for(l126 = 0;l126<l262;
l126++){(l39[(l1)][(l126)].l32) = -1;(l39[(l1)][(l126)].l33) = -1;(l39[(l1)][
(l126)].l34) = -1;(l39[(l1)][(l126)].next) = -1;(l39[(l1)][(l126)].l35) = 0;(
l39[(l1)][(l126)].l36) = 0;}l291 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)
;if(SOC_URPF_STATUS_GET(l1)){l291>>= 1;}if(soc_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL){(l39[(l1)][(((3*(128+2+1))-1))].l33) = (l291>>1)-1;(
l39[(l1)][(((((3*(128+2+1))/3)-1)))].l36) = l291>>1;(l39[(l1)][((((3*(128+2+1
))-1)))].l36) = (l291-(l39[(l1)][(((((3*(128+2+1))/3)-1)))].l36));}else{(l39[
(l1)][((((3*(128+2+1))-1)))].l36) = l291;}if((l105[(l1)])!= NULL){if(l113((
l105[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(l105[(l1)]) = 
NULL;}if(l111(l1,l291*2,l291,&(l105[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_MEMORY;}(void)soc_alpm_128_ipmc_war(l1,TRUE);SOC_ALPM_LPM_UNLOCK(l1);
return(SOC_E_NONE);}int soc_alpm_128_lpm_deinit(int l1){if(!soc_feature(l1,
soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);(void)
soc_alpm_128_ipmc_war(l1,FALSE);if((l105[(l1)])!= NULL){l113((l105[(l1)]));(
l105[(l1)]) = NULL;}if((l39[(l1)]!= NULL)){sal_free(l94[l1]);l94[l1] = NULL;
sal_free((l39[(l1)]));(l39[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l1);return(
SOC_E_NONE);}static int l2(int l1,void*l3,int*l293){int l118;int index;int l7
;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l127 = SOC_E_NONE;int l294 = 0;
sal_memcpy(l12,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(
l1,L3_DEFIP_PAIR_128m)*4);SOC_ALPM_LPM_LOCK(l1);l127 = l11(l1,l3,l12,&index,&
l118,&l7);if(l127 == SOC_E_NOT_FOUND){l127 = l277(l1,l118,l7,l12,&index);if(
l127<0){SOC_ALPM_LPM_UNLOCK(l1);return(l127);}}else{l294 = 1;}*l293 = index;
if(l127 == SOC_E_NONE){soc_alpm_128_lpm_state_dump(l1);LOG_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_alpm_128_lpm_insert: %d %d\n"),index,
l118));if(!l294){l253(l1,l3,index,0x4000,0);}l127 = l166(l1,MEM_BLOCK_ANY,
index,index,l3);if(l127>= 0){l127 = l263(l1,index,l3);}}SOC_ALPM_LPM_UNLOCK(
l1);return(l127);}static int l4(int l1,void*l5){int l118;int index;int l7;
uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l127 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(
l1);l127 = l11(l1,l5,l12,&index,&l118,&l7);if(l127 == SOC_E_NONE){LOG_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_alpm_lpm_delete: %d %d\n"),index,l118))
;l256(l1,l5,index);l127 = l283(l1,l118,l7,l12,index);}
soc_alpm_128_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l127);}static
int l15(int l1,void*l5,void*l12,int*l13){int l118;int l127;int l7;
SOC_ALPM_LPM_LOCK(l1);l127 = l11(l1,l5,l12,l13,&l118,&l7);SOC_ALPM_LPM_UNLOCK
(l1);return(l127);}static int l6(int l17,void*l5,int l7,int l8,int l9,
defip_aux_scratch_entry_t*l10){uint32 l129;uint32 l295[4] = {0,0,0,0};int l118
= 0;int l127 = SOC_E_NONE;l129 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,
l5,VALID0_LWRf);soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,VALIDf,l129
);l129 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,MODE0_LWRf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,MODEf,l129);l129 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,ENTRY_TYPE0_LWRf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,ENTRY_TYPEf,0);l129 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,GLOBAL_ROUTEf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,GLOBAL_ROUTEf,l129);if(
SOC_MEM_FIELD_VALID(l17,L3_DEFIP_PAIR_128m,EXPECTED_L3_IIFf)){l129 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,EXPECTED_L3_IIFf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,EXPECTED_L3_IIFf,l129);}if(
SOC_MEM_FIELD_VALID(l17,L3_DEFIP_PAIR_128m,RPA_IDf)){l129 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,RPA_IDf);soc_mem_field32_set(
l17,L3_DEFIP_AUX_SCRATCHm,l10,RPA_IDf,l129);}l129 = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,ECMPf);soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,
l10,ECMPf,l129);l129 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,
ECMP_PTRf);soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,ECMP_PTRf,l129);
l129 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,NEXT_HOP_INDEXf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,NEXT_HOP_INDEXf,l129);l129 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,PRIf);soc_mem_field32_set(l17,
L3_DEFIP_AUX_SCRATCHm,l10,PRIf,l129);l129 = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,RPEf);soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10
,RPEf,l129);l129 =soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,VRF_ID_0_LWRf
);soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,VRFf,l129);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,DB_TYPEf,l8);l129 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,DST_DISCARDf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,DST_DISCARDf,l129);l129 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,CLASS_IDf);soc_mem_field32_set(
l17,L3_DEFIP_AUX_SCRATCHm,l10,CLASS_IDf,l129);l295[0] = soc_mem_field32_get(
l17,L3_DEFIP_PAIR_128m,l5,IP_ADDR0_LWRf);l295[1] = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,IP_ADDR1_LWRf);l295[2] = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,IP_ADDR0_UPRf);l295[3] = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,IP_ADDR1_UPRf);soc_mem_field_set(l17,
L3_DEFIP_AUX_SCRATCHm,(uint32*)l10,IP_ADDRf,(uint32*)l295);l127 = l125(l17,l5
,&l118);if(SOC_FAILURE(l127)){return l127;}soc_mem_field32_set(l17,
L3_DEFIP_AUX_SCRATCHm,l10,IP_LENGTHf,l118);soc_mem_field32_set(l17,
L3_DEFIP_AUX_SCRATCHm,l10,REPLACE_LENf,l9);return(SOC_E_NONE);}static int l16
(int l17,void*lpm_entry,void*l18,void*l19,soc_mem_t l20,uint32 l21,uint32*
l296){uint32 l129;uint32 l295[4];int l118 = 0;int l127 = SOC_E_NONE;uint32 l22
= 0;sal_memset(l18,0,soc_mem_entry_words(l17,l20)*4);l129 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf);
soc_mem_field32_set(l17,l20,l18,VALIDf,l129);l129 = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,lpm_entry,ECMPf);soc_mem_field32_set(l17,l20,l18,ECMPf,
l129);l129 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf);
soc_mem_field32_set(l17,l20,l18,ECMP_PTRf,l129);l129 = soc_mem_field32_get(
l17,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf);soc_mem_field32_set(l17,l20
,l18,NEXT_HOP_INDEXf,l129);l129 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,
lpm_entry,PRIf);soc_mem_field32_set(l17,l20,l18,PRIf,l129);l129 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,RPEf);
soc_mem_field32_set(l17,l20,l18,RPEf,l129);l129 = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf);soc_mem_field32_set(l17,l20,l18,
DST_DISCARDf,l129);l129 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,
lpm_entry,SRC_DISCARDf);soc_mem_field32_set(l17,l20,l18,SRC_DISCARDf,l129);
l129 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf);
soc_mem_field32_set(l17,l20,l18,CLASS_IDf,l129);l295[0] = soc_mem_field32_get
(l17,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf);l295[1] = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf);l295[2] = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf);l295[3] = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf);
soc_mem_field_set(l17,l20,(uint32*)l18,KEYf,(uint32*)l295);l127 = l125(l17,
lpm_entry,&l118);if(SOC_FAILURE(l127)){return l127;}if((l118 == 0)&&(l295[0]
== 0)&&(l295[1] == 0)&&(l295[2] == 0)&&(l295[3] == 0)){l22 = 1;}if(l296!= 
NULL){*l296 = l22;}soc_mem_field32_set(l17,l20,l18,LENGTHf,l118);if(l19 == 
NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(l17)){sal_memset(l19,0,
soc_mem_entry_words(l17,l20)*4);sal_memcpy(l19,l18,soc_mem_entry_words(l17,
l20)*4);soc_mem_field32_set(l17,l20,l19,DST_DISCARDf,0);soc_mem_field32_set(
l17,l20,l19,RPEf,0);soc_mem_field32_set(l17,l20,l19,SRC_DISCARDf,l21&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(l17,l20,l19,DEFAULTROUTEf,l22);
}return(SOC_E_NONE);}static int l23(int l17,void*l18,soc_mem_t l20,int l7,int
l24,int l25,int index,void*lpm_entry){uint32 l129;uint32 l295[4];uint32 l147,
l297;sal_memset(lpm_entry,0,soc_mem_entry_words(l17,L3_DEFIP_PAIR_128m)*4);
l129 = soc_mem_field32_get(l17,l20,l18,VALIDf);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf,l129);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,VALID1_LWRf,l129);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,VALID0_UPRf,l129);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,VALID1_UPRf,l129);l129 = soc_mem_field32_get(l17
,l20,l18,ECMPf);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ECMPf,
l129);l129 = soc_mem_field32_get(l17,l20,l18,ECMP_PTRf);soc_mem_field32_set(
l17,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf,l129);l129 = soc_mem_field32_get(
l17,l20,l18,NEXT_HOP_INDEXf);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,
lpm_entry,NEXT_HOP_INDEXf,l129);l129 = soc_mem_field32_get(l17,l20,l18,PRIf);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,PRIf,l129);l129 = 
soc_mem_field32_get(l17,l20,l18,RPEf);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,RPEf,l129);l129 = soc_mem_field32_get(l17,l20,
l18,DST_DISCARDf);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,
DST_DISCARDf,l129);l129 = soc_mem_field32_get(l17,l20,l18,SRC_DISCARDf);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,SRC_DISCARDf,l129);l129 = 
soc_mem_field32_get(l17,l20,l18,CLASS_IDf);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf,l129);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,ALG_BKT_PTRf,l25);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,ALG_HIT_IDXf,index);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,3);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,3);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,3);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,3);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,1);soc_mem_field32_set(l17
,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);soc_mem_field32_set(
l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,1);soc_mem_field32_set
(l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,1);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_LWRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_LWRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_UPRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_UPRf,3);
soc_mem_field_get(l17,l20,l18,KEYf,l295);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf,l295[0]);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf,l295[1]);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf,l295[2]);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf,l295[3]);l295[0] = l295[1] = l295[
2] = l295[3] = 0;l129 = soc_mem_field32_get(l17,l20,l18,LENGTHf);l135(l17,
lpm_entry,l129);if(l24 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_HIGHf,1);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_ROUTEf,1);l147 = 0;l297 = 0;}else if(l24
== SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,
GLOBAL_ROUTEf,1);l147 = 0;l297 = 0;}else{l147 = l24;l297 = SOC_VRF_MAX(l17);}
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_LWRf,l147);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_LWRf,l147);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_UPRf,l147);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_UPRf,l147);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_LWRf,l297);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_LWRf,l297);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_UPRf,l297);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_UPRf,l297);
return(SOC_E_NONE);}int soc_alpm_128_warmboot_pivot_add(int l17,int l7,void*
lpm_entry,int l298,int l299){int l127 = SOC_E_NONE;uint32 key[4] = {0,0,0,0};
alpm_pivot_t*l173 = NULL;alpm_bucket_handle_t*l182 = NULL;int l147 = 0,l24 = 
0;uint32 l300;trie_t*l220 = NULL;uint32 prefix[5] = {0};int l22 = 0;l127 = 
l137(l17,lpm_entry,prefix,&l300,&l22);SOC_IF_ERROR_RETURN(l127);
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l17,lpm_entry,&l147,&l24));l298 = 
soc_alpm_physical_idx(l17,L3_DEFIP_PAIR_128m,l298,l7);l182 = sal_alloc(sizeof
(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l182 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l17,
"Unable to allocate memory for PIVOT trie node \n")));return SOC_E_NONE;}
sal_memset(l182,0,sizeof(*l182));l173 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l173 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l17,"Unable to allocate memory for PIVOT trie node \n")));sal_free(l182);
return SOC_E_MEMORY;}sal_memset(l173,0,sizeof(*l173));PIVOT_BUCKET_HANDLE(
l173) = l182;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l173));key[0] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l17,lpm_entry,IP_ADDR0_LWRf);key[1] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l17,lpm_entry,IP_ADDR1_LWRf);key[2] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l17,lpm_entry,IP_ADDR0_UPRf);key[3] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l17,lpm_entry,IP_ADDR1_UPRf);
PIVOT_BUCKET_INDEX(l173) = l299;PIVOT_TCAM_INDEX(l173) = 
SOC_ALPM_128_ADDR_LWR(l298)<<1;if(l147!= SOC_L3_VRF_OVERRIDE){l220 = 
VRF_PIVOT_TRIE_IPV6_128(l17,l24);if(l220 == NULL){trie_init(_MAX_KEY_LEN_144_
,&VRF_PIVOT_TRIE_IPV6_128(l17,l24));l220 = VRF_PIVOT_TRIE_IPV6_128(l17,l24);}
sal_memcpy(l173->key,prefix,sizeof(prefix));l173->len = l300;l127 = 
trie_insert(l220,l173->key,NULL,l173->len,(trie_node_t*)l173);if(SOC_FAILURE(
l127)){sal_free(l182);sal_free(l173);return l127;}}ALPM_TCAM_PIVOT(l17,
SOC_ALPM_128_ADDR_LWR(l298)<<1) = l173;PIVOT_BUCKET_VRF(l173) = l24;
PIVOT_BUCKET_IPV6(l173) = l7;PIVOT_BUCKET_ENT_CNT_UPDATE(l173);if(key[0] == 0
&&key[1] == 0&&key[2] == 0&&key[3] == 0){PIVOT_BUCKET_DEF(l173) = TRUE;}
VRF_PIVOT_REF_INC(l17,l24,l7);return l127;}static int l301(int l17,int l7,
void*lpm_entry,void*l18,soc_mem_t l20,int l298,int l299,int l302){int l303;
int l24;int l127 = SOC_E_NONE;int l22 = 0;uint32 prefix[5] = {0,0,0,0,0};
uint32 l132;void*l304 = NULL;trie_t*l305 = NULL;trie_t*l176 = NULL;
trie_node_t*l178 = NULL;payload_t*l306 = NULL;payload_t*l180 = NULL;
alpm_pivot_t*l143 = NULL;if((NULL == lpm_entry)||(NULL == l18)){return
SOC_E_PARAM;}SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l17,lpm_entry,&l303
,&l24));l20 = L3_DEFIP_ALPM_IPV6_128m;l304 = sal_alloc(sizeof(
defip_pair_128_entry_t),"Temp Defip Pair lpm_entry");if(NULL == l304){return
SOC_E_MEMORY;}SOC_IF_ERROR_RETURN(l23(l17,l18,l20,l7,l303,l299,l298,l304));
l127 = l137(l17,l304,prefix,&l132,&l22);if(SOC_FAILURE(l127)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l17,"prefix create failed\n")));return l127;}
sal_free(l304);l298 = soc_alpm_physical_idx(l17,L3_DEFIP_PAIR_128m,l298,l7);
l143 = ALPM_TCAM_PIVOT(l17,SOC_ALPM_128_ADDR_LWR(l298)<<1);l305 = 
PIVOT_BUCKET_TRIE(l143);l306 = sal_alloc(sizeof(payload_t),"Payload for Key")
;if(NULL == l306){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l17,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l180 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l180){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l17,
"Unable to allocate memory for pfx trie node\n")));sal_free(l306);return
SOC_E_MEMORY;}sal_memset(l306,0,sizeof(*l306));sal_memset(l180,0,sizeof(*l180
));l306->key[0] = prefix[0];l306->key[1] = prefix[1];l306->key[2] = prefix[2]
;l306->key[3] = prefix[3];l306->key[4] = prefix[4];l306->len = l132;l306->
index = l302;sal_memcpy(l180,l306,sizeof(*l306));l127 = trie_insert(l305,
prefix,NULL,l132,(trie_node_t*)l306);if(SOC_FAILURE(l127)){goto l307;}if(l7){
l176 = VRF_PREFIX_TRIE_IPV6_128(l17,l24);}if(!l22){l127 = trie_insert(l176,
prefix,NULL,l132,(trie_node_t*)l180);if(SOC_FAILURE(l127)){goto l199;}}return
l127;l199:(void)trie_delete(l305,prefix,l132,&l178);l306 = (payload_t*)l178;
l307:sal_free(l306);sal_free(l180);return l127;}static int l308(int l17,int
l30,int l24,int l221,int bkt_ptr){int l127 = SOC_E_NONE;uint32 l132;uint32 key
[5] = {0,0,0,0,0};trie_t*l309 = NULL;payload_t*l211 = NULL;
defip_pair_128_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_pair_128_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l17,"unable to allocate memory for LPM entry\n"))
);return SOC_E_MEMORY;}l26(l17,key,0,l24,l30,lpm_entry,0,1);if(l24 == 
SOC_VRF_MAX(l17)+1){soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
DEFAULT_MISSf,1);}soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
ALG_BKT_PTRf,bkt_ptr);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l17,l24) = lpm_entry;
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(l17,l24));l309 = 
VRF_PREFIX_TRIE_IPV6_128(l17,l24);l211 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l211 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l17,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l211,0,sizeof(*l211));l132 = 0;l211->key[0] = key[0]
;l211->key[1] = key[1];l211->len = l132;l127 = trie_insert(l309,key,NULL,l132
,&(l211->node));if(SOC_FAILURE(l127)){sal_free(l211);return l127;}
VRF_TRIE_INIT_DONE(l17,l24,l30,1);return l127;}int
soc_alpm_128_warmboot_prefix_insert(int l17,int l7,void*lpm_entry,void*l18,
int l298,int l299,int l302){int l303;int l24;int l127 = SOC_E_NONE;soc_mem_t
l20;l20 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l17,lpm_entry,&l303,&l24));if(l303 == 
SOC_L3_VRF_OVERRIDE){return(l127);}if(!VRF_TRIE_INIT_COMPLETED(l17,l24,l7)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l17,"VRF %d is not initialized\n"),
l24));l127 = l308(l17,l7,l24,l298,l299);if(SOC_FAILURE(l127)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l17,"VRF %d/%d trie init \n""failed\n"),l24,l7));
return l127;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l17,
"VRF %d/%d trie init ""completed\n"),l24,l7));}l127 = l301(l17,l7,lpm_entry,
l18,l20,l298,l299,l302);if(l127!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l17,"unit %d : Route Insertion Failed :%s\n"),l17,soc_errmsg(l127)
));return(l127);}VRF_TRIE_ROUTES_INC(l17,l24,l7);return(l127);}int
soc_alpm_128_warmboot_bucket_bitmap_set(int l1,int l30,int l223){int l310 = 1
;if(l30){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l310 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l223,l310);return SOC_E_NONE;}int
soc_alpm_128_warmboot_lpm_reinit_done(int l17){int l221;int l311 = ((3*(128+2
+1))-1);int l291 = soc_mem_index_count(l17,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l17)){l291>>= 1;}if(!soc_alpm_mode_get(l17)){(l39[(l17)][
(((3*(128+2+1))-1))].l34) = -1;for(l221 = ((3*(128+2+1))-1);l221>-1;l221--){
if(-1 == (l39[(l17)][(l221)].l32)){continue;}(l39[(l17)][(l221)].l34) = l311;
(l39[(l17)][(l311)].next) = l221;(l39[(l17)][(l311)].l36) = (l39[(l17)][(l221
)].l32)-(l39[(l17)][(l311)].l33)-1;l311 = l221;}(l39[(l17)][(l311)].next) = -
1;(l39[(l17)][(l311)].l36) = l291-(l39[(l17)][(l311)].l33)-1;}else{(l39[(l17)
][(((3*(128+2+1))-1))].l34) = -1;for(l221 = ((3*(128+2+1))-1);l221>(((3*(128+
2+1))-1)/3);l221--){if(-1 == (l39[(l17)][(l221)].l32)){continue;}(l39[(l17)][
(l221)].l34) = l311;(l39[(l17)][(l311)].next) = l221;(l39[(l17)][(l311)].l36)
= (l39[(l17)][(l221)].l32)-(l39[(l17)][(l311)].l33)-1;l311 = l221;}(l39[(l17)
][(l311)].next) = -1;(l39[(l17)][(l311)].l36) = l291-(l39[(l17)][(l311)].l33)
-1;l311 = (((3*(128+2+1))-1)/3);(l39[(l17)][((((3*(128+2+1))-1)/3))].l34) = -
1;for(l221 = ((((3*(128+2+1))-1)/3)-1);l221>-1;l221--){if(-1 == (l39[(l17)][(
l221)].l32)){continue;}(l39[(l17)][(l221)].l34) = l311;(l39[(l17)][(l311)].
next) = l221;(l39[(l17)][(l311)].l36) = (l39[(l17)][(l221)].l32)-(l39[(l17)][
(l311)].l33)-1;l311 = l221;}(l39[(l17)][(l311)].next) = -1;(l39[(l17)][(l311)
].l36) = (l291>>1)-(l39[(l17)][(l311)].l33)-1;}return(SOC_E_NONE);}int
soc_alpm_128_warmboot_lpm_reinit(int l17,int l7,int l221,void*lpm_entry){int
l14;l253(l17,lpm_entry,l221,0x4000,0);SOC_IF_ERROR_RETURN(l287(l17,lpm_entry,
&l14));if((l39[(l17)][(l14)].l35) == 0){(l39[(l17)][(l14)].l32) = l221;(l39[(
l17)][(l14)].l33) = l221;}else{(l39[(l17)][(l14)].l33) = l221;}(l39[(l17)][(
l14)].l35)++;return(SOC_E_NONE);}int soc_alpm_128_bucket_sanity_check(int l1,
soc_mem_t l231,int index){int l127 = SOC_E_NONE;int l126,l228,l238,l7,l140 = 
-1;int l147 = 0,l24;uint32 l148 = 0;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int
l312,l313,l314;defip_pair_128_entry_t lpm_entry;int l243,l315;soc_mem_t l20 = 
L3_DEFIP_ALPM_IPV6_128m;int l316 = 8,l317;int l318 = 0;soc_field_t l319 = 
VALID0_LWRf;soc_field_t l320 = GLOBAL_HIGHf;soc_field_t l321 = ALG_BKT_PTRf;
l7 = L3_DEFIP_MODE_128;l243 = soc_mem_index_min(l1,l231);l315 = 
soc_mem_index_max(l1,l231);if((index>= 0)&&(index<l243||index>l315)){return
SOC_E_PARAM;}else if(index>= 0){l243 = index;l315 = index;}SOC_ALPM_LPM_LOCK(
l1);for(l126 = l243;l126<= l315;l126++){l127 = soc_mem_read(l1,l231,
MEM_BLOCK_ANY,l126,(void*)l12);if(SOC_FAILURE(l127)){continue;}if(
soc_mem_field32_get(l1,l231,(void*)l12,l319) == 0||soc_mem_field32_get(l1,
l231,(void*)l12,l320) == 1){continue;}l317 = soc_mem_field32_get(l1,l231,(
void*)l12,l321);l127 = soc_alpm_128_lpm_vrf_get(l1,l12,&l147,&l24);if(
SOC_FAILURE(l127)){continue;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l7)){l316<<= 1;}
l140 = -1;for(l228 = 0;l228<l316;l228++){l127 = _soc_alpm_mem_index(l1,l20,
l317,l228,l148,&l238);if(SOC_FAILURE(l127)){continue;}l127 = soc_mem_read(l1,
l20,MEM_BLOCK_ANY,l238,(void*)l12);if(SOC_FAILURE(l127)){break;}if(!
soc_mem_field32_get(l1,l20,(void*)l12,VALIDf)){continue;}l127 = l23(l1,(void*
)l12,l20,l7,l147,l317,0,&lpm_entry);if(SOC_FAILURE(l127)){continue;}l127 = 
l144(l1,(void*)&lpm_entry,l20,(void*)l12,&l312,&l313,&l314,FALSE);if(
SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tLaunched AUX operation for ""index %d bucket %d sanity check failed\n"),
l126,l317));l318++;continue;}if(l313!= l317){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tEntry at index %d does not belong "
"to bucket %d(from bucket %d)\n"),l314,l317,l313));l318++;}if(l140 == -1){
l140 = l312;continue;}if(l140!= l312){int l154,l155;l154 = 
soc_alpm_logical_idx(l1,l231,SOC_ALPM_128_DEFIP_TO_PAIR(l140>>1),1);l155 = 
soc_alpm_logical_idx(l1,l231,SOC_ALPM_128_DEFIP_TO_PAIR(l312>>1),1);LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tAliased bucket %d(returned "
"bucket %d) found from TCAM1 %d and TCAM2 %d\n"),l317,l313,l154,l155));l318++
;}}}SOC_ALPM_LPM_UNLOCK(l1);if(l318 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check passed\n"),
SOC_MEM_NAME(l1,l231),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l1,l231),index,l318));return l318;}
int soc_alpm_128_pivot_sanity_check(int l1,soc_mem_t l231,int index){int l126
,l322;int l238,l243,l315;int l127 = SOC_E_NONE;int*l323 = NULL;int l312,l313,
l314;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l324,l325,l326;soc_mem_t l327;
soc_mem_t l328 = L3_DEFIP_ALPM_IPV6_128m;int l7 = L3_DEFIP_MODE_128;int l318 = 
0;soc_field_t l319 = VALID0_LWRf;soc_field_t l320 = GLOBAL_HIGHf;soc_field_t
l329 = NEXT_HOP_INDEXf;soc_field_t l321 = ALG_BKT_PTRf;l243 = 
soc_mem_index_min(l1,l231);l315 = soc_mem_index_max(l1,l231);if((index>= 0)&&
(index<l243||index>l315)){return SOC_E_PARAM;}else if(index>= 0){l243 = index
;l315 = index;}l323 = sal_alloc(sizeof(int)*MAX_PIVOT_COUNT,
"Bucket index array 128");if(l323 == NULL){l127 = SOC_E_MEMORY;return l127;}
sal_memset(l323,0xff,sizeof(int)*MAX_PIVOT_COUNT);SOC_ALPM_LPM_LOCK(l1);for(
l126 = l243;l126<= l315;l126++){l127 = soc_mem_read(l1,l231,MEM_BLOCK_ANY,
l126,(void*)l12);if(SOC_FAILURE(l127)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l1,l231
),l126,l127));l318 = l127;continue;}if(soc_mem_field32_get(l1,l231,(void*)l12
,l319) == 0||soc_mem_field32_get(l1,l231,(void*)l12,l320) == 1){continue;}
l324 = soc_mem_field32_get(l1,l231,(void*)l12,l321);if(l324!= 0){if(l323[l324
] == -1){l323[l324] = l126;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tDuplicated bucket pointer ""%d detected, in memory %s index1 %d and "
"index2 %d\n"),l324,SOC_MEM_NAME(l1,l231),l323[l324],l126));l318 = l127;
continue;}}l127 = alpm_bucket_is_assigned(l1,l324,l7,&l322);if(l127 == 
SOC_E_PARAM){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tInvalid bucket pointer %d ""detected, in memory %s index %d\n"),l324,
SOC_MEM_NAME(l1,l231),l126));l318 = l127;continue;}if(l127>= 0&&l322 == 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),l324,SOC_MEM_NAME(l1,l231),l126));l318 = 
l127;continue;}l327 = _soc_trident2_alpm_bkt_view_get(l1,l324<<2);if(l327!= 
L3_DEFIP_ALPM_IPV6_128m){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual %s\n"),l324,SOC_MEM_NAME(l1,
L3_DEFIP_ALPM_IPV6_128m),SOC_MEM_NAME(l1,l327)));l318 = l127;continue;}l312 = 
-1;l325 = soc_mem_field32_get(l1,l231,(void*)l12,l329);l127 = l144(l1,l12,
l328,(void*)l12,&l312,&l313,&l314,FALSE);if(l312 == -1){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLaunched AUX operation for PIVOT "
"index %d sanity check failed\n"),l126));l318 = l127;continue;}l238 = 
soc_alpm_logical_idx(l1,l231,SOC_ALPM_128_DEFIP_TO_PAIR(l312>>1),1);l127 = 
soc_mem_read(l1,l231,MEM_BLOCK_ANY,l238,(void*)l12);if(SOC_FAILURE(l127)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tRead memory %s index %d (nexthop) ""return %d\n"),SOC_MEM_NAME(l1,l231),
l238,l127));l318 = l127;continue;}l326 = soc_mem_field32_get(l1,l231,(void*)
l12,l329);if(l325!= l326){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d index2 %d \n"),l126,l238));l318 = l127;continue;}}
SOC_ALPM_LPM_UNLOCK(l1);sal_free(l323);if(l318 == 0){LOG_INFO(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check passed\n"),
SOC_MEM_NAME(l1,l231),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check "
"failed, encountered %d error(s)\n"),SOC_MEM_NAME(l1,l231),index,l318));
return SOC_E_FAIL;}
#endif
#endif /* ALPM_ENABLE */
