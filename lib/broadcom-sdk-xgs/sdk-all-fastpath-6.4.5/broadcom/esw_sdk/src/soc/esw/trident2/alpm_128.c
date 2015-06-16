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
index,int l4);static int l5(int l1,void*l6);static int l7(int l1,void*l6,int
l8,int l9,int l10,defip_aux_scratch_entry_t*l11);static int l12(int l1,void*
l6,void*l13,int*l14,int*l15,int*l8);static int l16(int l1,void*l6,void*l13,
int*l14);static int l17(int l18,void*lpm_entry,void*l19,void*l20,soc_mem_t l21
,uint32 l22,uint32*l23);static int l24(int l18,void*l19,soc_mem_t l21,int l8,
int l25,int l26,int index,void*lpm_entry);static int l27(int l18,uint32*key,
int len,int l25,int l8,defip_pair_128_entry_t*lpm_entry,int l28,int l29);
static int l30(int l1,int l25,int l31);typedef struct l32{int l33;int l34;int
l35;int next;int l36;int l37;}l38,*l39;static l39 l40[SOC_MAX_NUM_DEVICES];
typedef struct l41{soc_field_info_t*l42;soc_field_info_t*l43;soc_field_info_t
*l44;soc_field_info_t*l45;soc_field_info_t*l46;soc_field_info_t*l47;
soc_field_info_t*l48;soc_field_info_t*l49;soc_field_info_t*l50;
soc_field_info_t*l51;soc_field_info_t*l52;soc_field_info_t*l53;
soc_field_info_t*l54;soc_field_info_t*l55;soc_field_info_t*l56;
soc_field_info_t*l57;soc_field_info_t*l58;soc_field_info_t*l59;
soc_field_info_t*l60;soc_field_info_t*l61;soc_field_info_t*l62;
soc_field_info_t*l63;soc_field_info_t*l64;soc_field_info_t*l65;
soc_field_info_t*l66;soc_field_info_t*l67;soc_field_info_t*l68;
soc_field_info_t*l69;soc_field_info_t*l70;soc_field_info_t*l71;
soc_field_info_t*l72;soc_field_info_t*l73;soc_field_info_t*l74;
soc_field_info_t*l75;soc_field_info_t*l76;soc_field_info_t*l77;
soc_field_info_t*l78;soc_field_info_t*l79;soc_field_info_t*l80;
soc_field_info_t*l81;soc_field_info_t*l82;soc_field_info_t*l83;
soc_field_info_t*l84;soc_field_info_t*l85;soc_field_info_t*l86;
soc_field_info_t*l87;soc_field_info_t*l88;soc_field_info_t*l89;
soc_field_info_t*l90;soc_field_info_t*l91;soc_field_info_t*l92;
soc_field_info_t*l93;soc_field_info_t*l94;soc_field_info_t*l95;}l96,*l97;
static l97 l98[SOC_MAX_NUM_DEVICES];typedef struct l99{int l18;int l100;int
l101;uint16*l102;uint16*l103;}l104;typedef uint32 l105[9];typedef int(*l106)(
l105 l107,l105 l108);static l104*l109[SOC_MAX_NUM_DEVICES];static void l110(
int l1,void*l13,int index,l105 l111);static uint16 l112(uint8*l113,int l114);
static int l115(int l18,int l100,int l101,l104**l116);static int l117(l104*
l118);static int l119(l104*l120,l106 l121,l105 entry,int l122,uint16*l123);
static int l124(l104*l120,l106 l121,l105 entry,int l122,uint16 l125,uint16
l126);static int l127(l104*l120,l106 l121,l105 entry,int l122,uint16 l128);
static int l129(int l1,const void*entry,int*l122){int l130,l131;int l132[4] = 
{IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf}
;uint32 l133;l133 = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l132[0]);
if((l131 = _ipmask2pfx(l133,l122))<0){return(l131);}for(l130 = 1;l130<4;l130
++){l133 = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l132[l130]);if(*
l122){if(l133!= 0xffffffff){return(SOC_E_PARAM);}*l122+= 32;}else{if((l131 = 
_ipmask2pfx(l133,l122))<0){return(l131);}}}return SOC_E_NONE;}static void l134
(uint32*l135,int l136,int l31){uint32 l137,l138,l33,prefix[5];int l130;
sal_memcpy(prefix,l135,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));
sal_memset(l135,0,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));l137 = 128-
l136;l33 = (l137+31)/32;if((l137%32) == 0){l33++;}l137 = l137%32;for(l130 = 
l33;l130<= 4;l130++){prefix[l130]<<= l137;if(l130<4){l138 = prefix[l130+1]&~(
0xffffffff>>l137);l138 = (((32-l137) == 32)?0:(l138)>>(32-l137));prefix[l130]
|= l138;}}for(l130 = l33;l130<= 4;l130++){l135[3-(l130-l33)] = prefix[l130];}
}static void l139(int l18,void*lpm_entry,int l15){int l130;soc_field_t l140[4
] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,
IP_ADDR_MASK1_UPRf};for(l130 = 0;l130<4;l130++){soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,l140[l130],0);}for(l130 = 0;l130<4;l130++){if(
l15<= 32)break;soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,l140[3-
l130],0xffffffff);l15-= 32;}soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,
lpm_entry,l140[3-l130],~(((l15) == 32)?0:(0xffffffff)>>(l15)));}static int
l141(int l1,void*entry,uint32*l135,uint32*l15,int*l23){int l130;int l122 = 0,
l33;int l131 = SOC_E_NONE;uint32 l137,l138;uint32 prefix[5];sal_memset(l135,0
,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(prefix,0,sizeof(
uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));prefix[0] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR0_LWRf);prefix[1] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR1_LWRf);prefix[2] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR0_UPRf);prefix[3] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR1_UPRf);l131 = l129(l1,entry,&l122);if(
SOC_FAILURE(l131)){return l131;}l137 = 128-l122;l33 = l137/32;l137 = l137%32;
for(l130 = l33;l130<4;l130++){prefix[l130]>>= l137;l138 = prefix[l130+1]&((1
<<l137)-1);l138 = (((32-l137) == 32)?0:(l138)<<(32-l137));prefix[l130]|= l138
;}for(l130 = l33;l130<4;l130++){l135[4-(l130-l33)] = prefix[l130];}*l15 = 
l122;if(l23!= NULL){*l23 = (prefix[0] == 0)&&(prefix[1] == 0)&&(prefix[2] == 
0)&&(prefix[3] == 0)&&(l122 == 0);}return SOC_E_NONE;}int l142(int l1,uint32*
prefix,uint32 l136,int l8,int l25,int*l143,int*l144,int*bucket_index){int l131
= SOC_E_NONE;trie_t*l145;trie_node_t*l146 = NULL;alpm_pivot_t*l147;l145 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l25);l131 = trie_find_lpm(l145,prefix,l136,&l146);
if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l131;}l147 = (alpm_pivot_t*)l146;*l143 = 1;*
l144 = PIVOT_TCAM_INDEX(l147);*bucket_index = PIVOT_BUCKET_INDEX(l147);return
SOC_E_NONE;}static int l148(int l1,void*l6,soc_mem_t l21,void*l149,int*l144,
int*bucket_index,int*l14,int l150){uint32 l13[SOC_MAX_MEM_FIELD_WORDS];int
l151,l25,l31;int l123;uint32 l9,l152;int l131 = SOC_E_NONE;int l143 = 0;l31 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&
l25));if(l151 == 0){if(soc_alpm_mode_get(l1)){return SOC_E_PARAM;}}if(l25 == 
SOC_VRF_MAX(l1)+1){l9 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l152);}else{l9 = 
2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l152);}if(l151!= SOC_L3_VRF_OVERRIDE){if(
l150){uint32 prefix[5],l136;int l23 = 0;l131 = l141(l1,l6,prefix,&l136,&l23);
if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l131;}l131 = l142(l1,
prefix,l136,l31,l25,&l143,l144,bucket_index);SOC_IF_ERROR_RETURN(l131);}else{
defip_aux_scratch_entry_t l11;sal_memset(&l11,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l7(l1,l6,l31,l9,0,&l11));
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l11,TRUE,&l143,l144,
bucket_index));}if(l143){l17(l1,l6,l13,0,l21,0,0);l131 = 
_soc_alpm_find_in_bkt(l1,l21,*bucket_index,l152,l13,l149,&l123,l31);if(
SOC_SUCCESS(l131)){*l14 = l123;}}else{l131 = SOC_E_NOT_FOUND;}}return l131;}
static int l153(int l1,void*l6,void*l149,void*l154,soc_mem_t l21,int l123){
defip_aux_scratch_entry_t l11;int l151,l31,l25;int bucket_index;uint32 l9,
l152;int l143 = 0,l138 = 0;int l144;l31 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&l25));if(l25 == 
SOC_VRF_MAX(l1)+1){l9 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l152);}else{l9 = 
2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l152);}if(!soc_alpm_mode_get(l1)){l9 = 2;}
if(l151!= SOC_L3_VRF_OVERRIDE){sal_memset(&l11,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l7(l1,l6,l31,l9,0,&l11));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l21,MEM_BLOCK_ANY,l123,l149));if(
SOC_URPF_STATUS_GET(l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,l21,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l123),l154));}l138 = soc_mem_field32_get
(l1,L3_DEFIP_AUX_SCRATCHm,&l11,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l11,REPLACE_LENf,l138);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l11,TRUE,&l143,&l144,&bucket_index));if
(SOC_URPF_STATUS_GET(l1)){if(l138 == 0){soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l11,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l11,RPEf,0);}l138 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l11,DB_TYPEf);l138+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l11,DB_TYPEf,l138);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l11,TRUE,&l143,&l144,&bucket_index));}}
return SOC_E_NONE;}static int l155(int l1,int l156,int l157){int l131,l138,
l158,l159;defip_aux_table_entry_t l160,l161;l158 = SOC_ALPM_128_ADDR_LWR(l156
);l159 = SOC_ALPM_128_ADDR_UPR(l156);l131 = soc_mem_read(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l158,&l160);SOC_IF_ERROR_RETURN(l131);l131 = 
soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l159,&l161);
SOC_IF_ERROR_RETURN(l131);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l160,
BPM_LENGTH0f,l157);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l160,
BPM_LENGTH1f,l157);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,
BPM_LENGTH0f,l157);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,
BPM_LENGTH1f,l157);l138 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&l160,
DB_TYPE0f);l131 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l158,&
l160);SOC_IF_ERROR_RETURN(l131);l131 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l159,&l161);SOC_IF_ERROR_RETURN(l131);if(SOC_URPF_STATUS_GET(l1
)){l138++;soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l160,BPM_LENGTH0f,l157)
;soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l160,BPM_LENGTH1f,l157);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,BPM_LENGTH0f,l157);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,BPM_LENGTH1f,l157);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l160,DB_TYPE0f,l138);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l160,DB_TYPE1f,l138);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,DB_TYPE0f,l138);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,DB_TYPE1f,l138);l158+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l159+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(
l1,L3_DEFIPm))/2;l131 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
l158,&l160);SOC_IF_ERROR_RETURN(l131);l131 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l159,&l161);}return l131;}static int l162(
int l1,int l163,void*entry,defip_aux_table_entry_t*l164,int l165){uint32 l138
,l9,l166 = 0;soc_mem_t l21 = L3_DEFIP_PAIR_128m;soc_mem_t l167 = 
L3_DEFIP_AUX_TABLEm;int l131 = SOC_E_NONE,l122,l25;void*l168,*l169;l168 = (
void*)l164;l169 = (void*)(l164+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l167,
MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l163),l164));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l167,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l163),l164+1));l138 = 
soc_mem_field32_get(l1,l21,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l167,
l168,VRF0f,l138);l138 = soc_mem_field32_get(l1,l21,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l167,l168,VRF1f,l138);l138 = soc_mem_field32_get(l1,
l21,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l167,l169,VRF0f,l138);l138 = 
soc_mem_field32_get(l1,l21,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l167,
l169,VRF1f,l138);l138 = soc_mem_field32_get(l1,l21,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l167,l168,MODE0f,l138);l138 = soc_mem_field32_get(l1,
l21,entry,MODE1_LWRf);soc_mem_field32_set(l1,l167,l168,MODE1f,l138);l138 = 
soc_mem_field32_get(l1,l21,entry,MODE0_UPRf);soc_mem_field32_set(l1,l167,l169
,MODE0f,l138);l138 = soc_mem_field32_get(l1,l21,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l167,l169,MODE1f,l138);l138 = soc_mem_field32_get(l1,
l21,entry,VALID0_LWRf);soc_mem_field32_set(l1,l167,l168,VALID0f,l138);l138 = 
soc_mem_field32_get(l1,l21,entry,VALID1_LWRf);soc_mem_field32_set(l1,l167,
l168,VALID1f,l138);l138 = soc_mem_field32_get(l1,l21,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l167,l169,VALID0f,l138);l138 = soc_mem_field32_get(l1,
l21,entry,VALID1_UPRf);soc_mem_field32_set(l1,l167,l169,VALID1f,l138);l131 = 
l129(l1,entry,&l122);SOC_IF_ERROR_RETURN(l131);soc_mem_field32_set(l1,l167,
l168,IP_LENGTH0f,l122);soc_mem_field32_set(l1,l167,l168,IP_LENGTH1f,l122);
soc_mem_field32_set(l1,l167,l169,IP_LENGTH0f,l122);soc_mem_field32_set(l1,
l167,l169,IP_LENGTH1f,l122);l138 = soc_mem_field32_get(l1,l21,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l167,l168,IP_ADDR0f,l138);l138 = 
soc_mem_field32_get(l1,l21,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l167,
l168,IP_ADDR1f,l138);l138 = soc_mem_field32_get(l1,l21,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l167,l169,IP_ADDR0f,l138);l138 = soc_mem_field32_get(
l1,l21,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l167,l169,IP_ADDR1f,l138);
l138 = soc_mem_field32_get(l1,l21,entry,ENTRY_TYPE0_LWRf);soc_mem_field32_set
(l1,l167,l168,ENTRY_TYPE0f,l138);l138 = soc_mem_field32_get(l1,l21,entry,
ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,l167,l168,ENTRY_TYPE1f,l138);l138 = 
soc_mem_field32_get(l1,l21,entry,ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,
l167,l169,ENTRY_TYPE0f,l138);l138 = soc_mem_field32_get(l1,l21,entry,
ENTRY_TYPE1_UPRf);soc_mem_field32_set(l1,l167,l169,ENTRY_TYPE1f,l138);l131 = 
soc_alpm_128_lpm_vrf_get(l1,entry,&l25,&l122);SOC_IF_ERROR_RETURN(l131);if(
SOC_URPF_STATUS_GET(l1)){if(l165>= (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m
)>>1)){l166 = 1;}}switch(l25){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l1
,l167,l168,VALID0f,0);soc_mem_field32_set(l1,l167,l168,VALID1f,0);
soc_mem_field32_set(l1,l167,l169,VALID0f,0);soc_mem_field32_set(l1,l167,l169,
VALID1f,0);l9 = 0;break;case SOC_L3_VRF_GLOBAL:l9 = l166?1:0;break;default:l9
= l166?3:2;break;}if(SOC_MEM_FIELD_VALID(l1,l21,MULTICAST_ROUTEf)){l138 = 
soc_mem_field32_get(l1,l21,entry,MULTICAST_ROUTEf);if(l138){l9 = l166?5:4;}}
soc_mem_field32_set(l1,l167,l168,DB_TYPE0f,l9);soc_mem_field32_set(l1,l167,
l168,DB_TYPE1f,l9);soc_mem_field32_set(l1,l167,l169,DB_TYPE0f,l9);
soc_mem_field32_set(l1,l167,l169,DB_TYPE1f,l9);if(l166){l138 = 
soc_mem_field32_get(l1,l21,entry,ALG_BKT_PTRf);if(l138){l138+= 
SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l21,entry,ALG_BKT_PTRf,l138)
;}}return SOC_E_NONE;}static int l170(int l1,int l171,int index,int l172,void
*entry){defip_aux_table_entry_t l164[2];l172 = soc_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l172,1);SOC_IF_ERROR_RETURN(l162(l1,l172,entry,&l164[0],
index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,
entry));index = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(index),l164));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(index),l164+
1));return SOC_E_NONE;}static int l173(int l1,void*l6,soc_mem_t l21,void*l149
,void*l154,int*l14,int bucket_index,int l144){alpm_pivot_t*l147,*l174,*l175;
defip_aux_scratch_entry_t l11;uint32 l13[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l176,l136;uint32 l177[5];int l31,l25,l151;int l123;int l131 = 
SOC_E_NONE,l178;uint32 l9,l152,l157 = 0;int l143 =0;int l156;int l179 = 0;
trie_t*trie,*l180;trie_node_t*l181,*l182 = NULL,*l146 = NULL;payload_t*l183,*
l184,*l185;defip_pair_128_entry_t lpm_entry;alpm_bucket_handle_t*l186;int l130
,l187 = -1,l23 = 0,l188 = 0;alpm_mem_prefix_array_t l189;
defip_alpm_ipv6_128_entry_t l190,l191;void*l192,*l193;int*l126 = NULL;trie_t*
l145 = NULL;int l194;int l195;void*l196 = NULL;void*l197 = NULL;void*l198;
void*l199;void*l200;int l201 = 0;int l202 = 0;l31 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&l25));if(l25 == 
SOC_VRF_MAX(l1)+1){l9 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l152);}else{l9 = 
2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l152);}l21 = L3_DEFIP_ALPM_IPV6_128m;l192 = 
((uint32*)&(l190));l193 = ((uint32*)&(l191));l131 = l141(l1,l6,prefix,&l136,&
l23);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: prefix create failed\n")));return l131;}sal_memset(&
l11,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l7(l1,l6,l31,l9,
0,&l11));if(bucket_index == 0){l131 = l142(l1,prefix,l136,l31,l25,&l143,&l144
,&bucket_index);SOC_IF_ERROR_RETURN(l131);if(l143 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: "
" Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l131 = 
_soc_alpm_insert_in_bkt(l1,l21,bucket_index,l152,l149,l13,&l123,l31);if(l131
== SOC_E_NONE){*l14 = l123;if(SOC_URPF_STATUS_GET(l1)){l178 = soc_mem_write(
l1,l21,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l123),l154);if(SOC_FAILURE(l178))
{return l178;}}}if(l131 == SOC_E_FULL){l179 = 1;}l147 = ALPM_TCAM_PIVOT(l1,
l144);trie = PIVOT_BUCKET_TRIE(l147);l175 = l147;l183 = sal_alloc(sizeof(
payload_t),"Payload for 128b Key");if(l183 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"trie node \n")));return SOC_E_MEMORY;}l184 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l184 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l183);return SOC_E_MEMORY;}sal_memset(l183,0,
sizeof(*l183));sal_memset(l184,0,sizeof(*l184));l183->key[0] = prefix[0];l183
->key[1] = prefix[1];l183->key[2] = prefix[2];l183->key[3] = prefix[3];l183->
key[4] = prefix[4];l183->len = l136;l183->index = l123;sal_memcpy(l184,l183,
sizeof(*l183));l184->bkt_ptr = l183;l131 = trie_insert(trie,prefix,NULL,l136,
(trie_node_t*)l183);if(SOC_FAILURE(l131)){goto l203;}l180 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l25);if(!l23){l131 = trie_insert(l180,prefix,NULL
,l136,(trie_node_t*)l184);}else{l146 = NULL;l131 = trie_find_lpm(l180,0,0,&
l146);l185 = (payload_t*)l146;if(SOC_SUCCESS(l131)){l185->bkt_ptr = l183;}}
l176 = l136;if(SOC_FAILURE(l131)){goto l204;}if(l179){l131 = 
alpm_bucket_assign(l1,&bucket_index,l31);if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate"
"new bucket for split\n")));bucket_index = -1;goto l205;}l131 = trie_split(
trie,_MAX_KEY_LEN_144_,FALSE,l177,&l136,&l181,NULL,FALSE);if(SOC_FAILURE(l131
)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Could not split bucket"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l205;}l146 = NULL;l131 = trie_find_lpm(l180,l177,l136,&l146)
;l185 = (payload_t*)l146;if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l177[0],l177[1],l177[
2],l177[3],l177[4],l136));goto l205;}if(l185->bkt_ptr){if(l185->bkt_ptr == 
l183){sal_memcpy(l192,l149,sizeof(defip_alpm_ipv6_128_entry_t));}else{l131 = 
soc_mem_read(l1,l21,MEM_BLOCK_ANY,((payload_t*)l185->bkt_ptr)->index,l192);if
(SOC_FAILURE(l131)){goto l205;}if(SOC_URPF_STATUS_GET(l1)){l131 = 
soc_mem_read(l1,l21,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,((payload_t*)l185->
bkt_ptr)->index),l193);l188 = soc_mem_field32_get(l1,l21,l193,DEFAULTROUTEf);
}}if(SOC_FAILURE(l131)){goto l205;}l131 = l24(l1,l192,l21,l31,l151,
bucket_index,0,&lpm_entry);if(SOC_FAILURE(l131)){goto l205;}l157 = ((
payload_t*)(l185->bkt_ptr))->len;}else{l131 = soc_mem_read(l1,
L3_DEFIP_PAIR_128m,MEM_BLOCK_ANY,soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,
SOC_ALPM_128_DEFIP_TO_PAIR(l144>>1),1),&lpm_entry);}l186 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM 128 Bucket Handle");if(l186 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate "
" memory for PIVOT trie node \n")));l131 = SOC_E_MEMORY;goto l205;}sal_memset
(l186,0,sizeof(*l186));l147 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for new 128b Pivot");if(l147 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate "
"memory for PIVOT trie node \n")));l131 = SOC_E_MEMORY;goto l205;}sal_memset(
l147,0,sizeof(*l147));PIVOT_BUCKET_HANDLE(l147) = l186;l131 = trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l147));PIVOT_BUCKET_TRIE(l147)->trie = 
l181;PIVOT_BUCKET_INDEX(l147) = bucket_index;PIVOT_BUCKET_VRF(l147) = l25;
PIVOT_BUCKET_IPV6(l147) = l31;PIVOT_BUCKET_DEF(l147) = FALSE;l147->key[0] = 
l177[0];l147->key[1] = l177[1];l147->key[2] = l177[2];l147->key[3] = l177[3];
l147->key[4] = l177[4];l147->len = l136;l145 = VRF_PIVOT_TRIE_IPV6_128(l1,l25
);l134((l177),(l136),(l31));l27(l1,l177,l136,l25,l31,&lpm_entry,0,0);
soc_L3_DEFIP_PAIR_128m_field32_set(l1,&lpm_entry,ALG_BKT_PTRf,bucket_index);
sal_memset(&l189,0,sizeof(l189));l131 = trie_traverse(PIVOT_BUCKET_TRIE(l147)
,alpm_mem_prefix_array_cb,&l189,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l131))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_128_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l205;}l126 = sal_alloc(sizeof(*l126)*l189.count,
"Temp storage for location of prefixes in new 128b bucket");if(l126 == NULL){
l131 = SOC_E_MEMORY;goto l205;}sal_memset(l126,-1,sizeof(*l126)*l189.count);
l194 = sizeof(defip_alpm_raw_entry_t);l195 = l194*ALPM_RAW_BKT_COUNT_DW;l197 = 
sal_alloc(4*l195,"Raw memory buffer");if(l197 == NULL){l131 = SOC_E_MEMORY;
goto l205;}sal_memset(l197,0,4*l195);l198 = (uint8*)l197+l195;l199 = (uint8*)
l197+l195*2;l200 = (uint8*)l197+l195*3;l131 = _soc_alpm_raw_bucket_read(l1,
l21,PIVOT_BUCKET_INDEX(l175),l197,l198);if(SOC_FAILURE(l131)){goto l205;}for(
l130 = 0;l130<l189.count;l130++){payload_t*l122 = l189.prefix[l130];if(l122->
index>0){SOC_ALPM_RAW_INDEX_DECODE(l1,l31,l122->index,l201,l202);l196 = (
uint8*)l197+l201*l194;_soc_alpm_raw_mem_read(l1,l21,l196,l202,l192);
_soc_alpm_raw_mem_write(l1,l21,l196,l202,soc_mem_entry_null(l1,l21));if(
SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE(l1,l31,_soc_alpm_rpf_entry
(l1,l122->index),l201,l202);l196 = (uint8*)l198+l201*l194;
_soc_alpm_raw_mem_read(l1,l21,l196,l202,l193);_soc_alpm_raw_mem_write(l1,l21,
l196,l202,soc_mem_entry_null(l1,l21));}l131 = _soc_alpm_mem_index(l1,l21,
bucket_index,l130,l152,&l123);if(SOC_SUCCESS(l131)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l31,l123,l201,l202);l196 = (uint8*)l199+l201*l194;_soc_alpm_raw_mem_write
(l1,l21,l196,l202,l192);if(SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l31,_soc_alpm_rpf_entry(l1,l123),l201,l202);l196 = (uint8*)l200+l201*l194
;_soc_alpm_raw_mem_write(l1,l21,l196,l202,l193);}}}else{l131 = 
_soc_alpm_mem_index(l1,l21,bucket_index,l130,l152,&l123);if(SOC_SUCCESS(l131)
){l187 = l130;*l14 = l123;_soc_alpm_raw_parity_set(l1,l21,l149);
SOC_ALPM_RAW_INDEX_DECODE(l1,l31,l123,l201,l202);l196 = (uint8*)l199+l201*
l194;_soc_alpm_raw_mem_write(l1,l21,l196,l202,l149);if(SOC_URPF_STATUS_GET(l1
)){_soc_alpm_raw_parity_set(l1,l21,l154);SOC_ALPM_RAW_INDEX_DECODE(l1,l31,
_soc_alpm_rpf_entry(l1,l123),l201,l202);l196 = (uint8*)l200+l201*l194;
_soc_alpm_raw_mem_write(l1,l21,l196,l202,l154);}}}l126[l130] = l123;}l131 = 
_soc_alpm_raw_bucket_write(l1,l21,bucket_index,l152,(void*)l199,(void*)l200,
l189.count);if(SOC_FAILURE(l131)){goto l206;}l131 = l2(l1,&lpm_entry,&l156,
l188);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Unable to add new ""pivot to tcam\n")));if(l131 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l25,l31);}goto l206;}l156 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l156,l31);l131 = l155(l1,l156,
l157);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),l156));goto l207
;}l131 = trie_insert(l145,l147->key,NULL,l147->len,(trie_node_t*)l147);if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));goto l207;}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l156)<<1) = l147;PIVOT_TCAM_INDEX(l147) = 
SOC_ALPM_128_ADDR_LWR(l156)<<1;VRF_PIVOT_REF_INC(l1,l25,l31);for(l130 = 0;
l130<l189.count;l130++){l189.prefix[l130]->index = l126[l130];}sal_free(l126)
;l131 = _soc_alpm_raw_bucket_write(l1,l21,PIVOT_BUCKET_INDEX(l175),l152,(void
*)l197,(void*)l198,-1);if(SOC_FAILURE(l131)){goto l207;}sal_free(l197);if(
l187 == -1){l131 = _soc_alpm_insert_in_bkt(l1,l21,PIVOT_BUCKET_HANDLE(l175)->
bucket_index,l152,l149,l13,&l123,l31);if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: Could not insert new "
"prefix into trie after split\n")));goto l205;}if(SOC_URPF_STATUS_GET(l1)){
l131 = soc_mem_write(l1,l21,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l123),l154);
}*l14 = l123;l183->index = l123;}PIVOT_BUCKET_ENT_CNT_UPDATE(l147);
VRF_BUCKET_SPLIT_INC(l1,l25,l31);}VRF_TRIE_ROUTES_INC(l1,l25,l31);if(l23){
sal_free(l184);}SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l11
,TRUE,&l143,&l144,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,
INSERT_PROPAGATE,&l11,FALSE,&l143,&l144,&bucket_index));if(
SOC_URPF_STATUS_GET(l1)){l136 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,
&l11,DB_TYPEf);l136+= 1;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l11,
DB_TYPEf,l136);if(l23){soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l11,RPEf
,1);}else{soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l11,RPEf,0);}
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l11,TRUE,&l143,&
l144,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE
,&l11,FALSE,&l143,&l144,&bucket_index));}PIVOT_BUCKET_ENT_CNT_UPDATE(l175);
return l131;l207:l178 = l5(l1,&lpm_entry);if(SOC_FAILURE(l178)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Failure to free new prefix""at %d\n"),
soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,l156,l31)));}if(ALPM_TCAM_PIVOT(l1
,SOC_ALPM_128_ADDR_LWR(l156)<<1)!= NULL){l178 = trie_delete(l145,l147->key,
l147->len,&l182);if(SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l156)<<1) = NULL;VRF_PIVOT_REF_DEC(
l1,l25,l31);l206:l174 = l175;for(l130 = 0;l130<l189.count;l130++){payload_t*
prefix = l189.prefix[l130];if(l126[l130]!= -1){sal_memset(l192,0,sizeof(
defip_alpm_ipv6_128_entry_t));l178 = soc_mem_write(l1,l21,MEM_BLOCK_ANY,l126[
l130],l192);_soc_trident2_alpm_bkt_view_set(l1,l126[l130],INVALIDm);if(
SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l1)){l178 = soc_mem_write(l1,l21,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l126[l130]),l192);_soc_trident2_alpm_bkt_view_set(l1,
_soc_alpm_rpf_entry(l1,l126[l130]),INVALIDm);if(SOC_FAILURE(l178)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l182 = NULL;l178 = trie_delete(
PIVOT_BUCKET_TRIE(l147),prefix->key,prefix->len,&l182);l183 = (payload_t*)
l182;if(SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: trie delete failure""in bkt move rollback\n")));}if(
prefix->index>0){l178 = trie_insert(PIVOT_BUCKET_TRIE(l174),prefix->key,NULL,
prefix->len,(trie_node_t*)l183);if(SOC_FAILURE(l178)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l183!= NULL){sal_free(l183);}}}if(l187
== -1){l182 = NULL;l178 = trie_delete(PIVOT_BUCKET_TRIE(l174),prefix,l176,&
l182);l183 = (payload_t*)l182;if(SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l183!= 
NULL){sal_free(l183);}}l178 = alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l147)
,l31);if(SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: new bucket release ""failure: %d\n"),
PIVOT_BUCKET_INDEX(l147)));}trie_destroy(PIVOT_BUCKET_TRIE(l147));sal_free(
l186);sal_free(l147);sal_free(l126);sal_free(l197);l182 = NULL;l178 = 
trie_delete(l180,prefix,l176,&l182);l184 = (payload_t*)l182;if(SOC_FAILURE(
l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l184){sal_free(
l184);}return l131;l205:if(l126!= NULL){sal_free(l126);}if(l197!= NULL){
sal_free(l197);}l182 = NULL;(void)trie_delete(l180,prefix,l176,&l182);l184 = 
(payload_t*)l182;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l31);}l204:l182 = NULL;(void)trie_delete(trie,prefix,l176,&l182)
;l183 = (payload_t*)l182;l203:if(l183!= NULL){sal_free(l183);}if(l184!= NULL)
{sal_free(l184);}return l131;}static int l27(int l18,uint32*key,int len,int
l25,int l8,defip_pair_128_entry_t*lpm_entry,int l28,int l29){uint32 l138;if(
l29){sal_memset(lpm_entry,0,sizeof(defip_pair_128_entry_t));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l80),(l25&SOC_VRF_MAX(l18)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l81),(l25&SOC_VRF_MAX(l18)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l78),(l25&SOC_VRF_MAX(l18)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l79),(l25&SOC_VRF_MAX(l18)));if(l25 == (SOC_VRF_MAX(
l18)+1)){l138 = 0;}else{l138 = SOC_VRF_MAX(l18);}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l84),(l138));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l85),(l138));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l82),(l138));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l83),(l138));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l57),(key[0]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l58),(key[1]))
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l55),(key[2]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l56),(key[3]))
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l65),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l66),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l63),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l64),(3));l139
(l18,(void*)lpm_entry,len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l76),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l77),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l74),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l75),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l69),((1<<
soc_mem_field_length(l18,L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l70),((1<<soc_mem_field_length(l18,L3_DEFIP_PAIR_128m
,MODE_MASK1_LWRf))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l67),((1<<soc_mem_field_length(
l18,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l68),((1<<soc_mem_field_length(l18,L3_DEFIP_PAIR_128m
,MODE_MASK1_UPRf))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l92),((1<<soc_mem_field_length(
l18,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l93),((1<<soc_mem_field_length(l18,L3_DEFIP_PAIR_128m
,ENTRY_TYPE_MASK1_LWRf))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO
(l18,L3_DEFIP_PAIR_128m)),(lpm_entry),(l98[(l18)]->l94),((1<<
soc_mem_field_length(l18,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l95),((1<<soc_mem_field_length(l18,L3_DEFIP_PAIR_128m
,ENTRY_TYPE_MASK1_UPRf))-1));return(SOC_E_NONE);}static int l208(int l1,void*
l6,int bucket_index,int l144,int l123,int l209){alpm_pivot_t*l147;
defip_alpm_ipv6_128_entry_t l190,l210,l191;defip_aux_scratch_entry_t l11;
uint32 l13[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l21;void*l192,*l211,*l193 = 
NULL;int l151;int l8;int l131 = SOC_E_NONE,l178 = SOC_E_NONE;uint32 l212[5],
prefix[5];int l31,l25;uint32 l136;int l213;uint32 l9,l152;int l143,l23 = 0;
trie_t*trie,*l180;uint32 l214;defip_pair_128_entry_t lpm_entry,*l215;
payload_t*l183 = NULL,*l216 = NULL,*l185 = NULL;trie_node_t*l182 = NULL,*l146
= NULL;trie_t*l145 = NULL;l8 = l31 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&l25));if(l151!= SOC_L3_VRF_OVERRIDE){if
(l25 == SOC_VRF_MAX(l1)+1){l9 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l152);}
else{l9 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l152);}l131 = l141(l1,l6,prefix,
&l136,&l23);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: prefix create failed\n")));return l131;}if(!
soc_alpm_mode_get(l1)){if(l151!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1
,l25,l31)>1){if(l23){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot v6-128 delete ""default route if other routes are present "
"in this mode"),l25));return SOC_E_PARAM;}}}l9 = 2;}l21 = 
L3_DEFIP_ALPM_IPV6_128m;l192 = ((uint32*)&(l190));SOC_ALPM_LPM_LOCK(l1);if(
bucket_index == 0){l131 = l148(l1,l6,l21,l192,&l144,&bucket_index,&l123,TRUE)
;}else{l131 = l17(l1,l6,l192,0,l21,0,0);}sal_memcpy(&l210,l192,sizeof(l210));
l211 = &l210;if(SOC_FAILURE(l131)){SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_delete: Unable to find "
"prefix for delete\n")));return l131;}l213 = bucket_index;l147 = 
ALPM_TCAM_PIVOT(l1,l144);trie = PIVOT_BUCKET_TRIE(l147);l131 = trie_delete(
trie,prefix,l136,&l182);l183 = (payload_t*)l182;if(l131!= SOC_E_NONE){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l131;}l180 = VRF_PREFIX_TRIE_IPV6_128(l1,l25);
l145 = VRF_PIVOT_TRIE_IPV6_128(l1,l25);if(!l23){l131 = trie_delete(l180,
prefix,l136,&l182);l216 = (payload_t*)l182;if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l217;}if(l209){l146 = NULL;l131 = trie_find_lpm(l180,prefix,
l136,&l146);l185 = (payload_t*)l146;if(SOC_SUCCESS(l131)){payload_t*l218 = (
payload_t*)(l185->bkt_ptr);if(l218!= NULL){l214 = l218->len;}else{l214 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l219;}sal_memcpy(l212,prefix,sizeof(
prefix));l134((l212),(l136),(l31));l131 = l27(l1,l212,l214,l25,l8,&lpm_entry,
0,1);l178 = l148(l1,&lpm_entry,l21,l192,&l144,&bucket_index,&l123,TRUE);(void
)l24(l1,l192,l21,l8,l151,bucket_index,0,&lpm_entry);(void)l27(l1,l212,l136,
l25,l8,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(l131)){l193
= ((uint32*)&(l191));l178 = soc_mem_read(l1,l21,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l123),l193);}}if((l214 == 0)&&SOC_FAILURE(l178)){l215 = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25);sal_memcpy(&lpm_entry,l215,sizeof(
lpm_entry));l131 = l27(l1,l212,l136,l25,l8,&lpm_entry,0,1);}if(SOC_FAILURE(
l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l219;}l215 = &lpm_entry;}}else{l146 = 
NULL;l131 = trie_find_lpm(l180,prefix,l136,&l146);l185 = (payload_t*)l146;if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),l25));goto l217;}l185->bkt_ptr = 0;
l214 = 0;l215 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25);}if(l209){l131 = l7(
l1,l215,l31,l9,l214,&l11);if(SOC_FAILURE(l131)){goto l219;}l131 = 
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l11,TRUE,&l143,&l144,&bucket_index);if(
SOC_FAILURE(l131)){goto l219;}if(SOC_URPF_STATUS_GET(l1)){uint32 l138;if(l193
!= NULL){l138 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l11,DB_TYPEf);
l138++;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l11,DB_TYPEf,l138);l138 = 
soc_mem_field32_get(l1,l21,l193,SRC_DISCARDf);soc_mem_field32_set(l1,l21,&l11
,SRC_DISCARDf,l138);l138 = soc_mem_field32_get(l1,l21,l193,DEFAULTROUTEf);
soc_mem_field32_set(l1,l21,&l11,DEFAULTROUTEf,l138);l131 = _soc_alpm_aux_op(
l1,DELETE_PROPAGATE,&l11,TRUE,&l143,&l144,&bucket_index);}if(SOC_FAILURE(l131
)){goto l219;}}}sal_free(l183);if(!l23){sal_free(l216);}
PIVOT_BUCKET_ENT_CNT_UPDATE(l147);if((l147->len!= 0)&&(trie->trie == NULL)){
uint32 l220[5];sal_memcpy(l220,l147->key,sizeof(l220));l134((l220),(l147->len
),(l8));l27(l1,l220,l147->len,l25,l8,&lpm_entry,0,1);l131 = l5(l1,&lpm_entry)
;if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),l147->key[1],l147->key[2],l147
->key[3],l147->key[4]));}}l131 = _soc_alpm_delete_in_bkt(l1,l21,l213,l152,
l211,l13,&l123,l31);if(!SOC_SUCCESS(l131)){SOC_ALPM_LPM_UNLOCK(l1);l131 = 
SOC_E_FAIL;return l131;}if(SOC_URPF_STATUS_GET(l1)){l131 = 
soc_mem_alpm_delete(l1,l21,SOC_ALPM_RPF_BKT_IDX(l1,l213),MEM_BLOCK_ALL,l152,
l211,l13,&l143);if(!SOC_SUCCESS(l131)){SOC_ALPM_LPM_UNLOCK(l1);l131 = 
SOC_E_FAIL;return l131;}}if((l147->len!= 0)&&(trie->trie == NULL)){l131 = 
alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l147),l31);if(SOC_FAILURE(l131)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(l147)));}l131 = trie_delete(l145,l147->key,l147->len,&l182
);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l147));sal_free(PIVOT_BUCKET_HANDLE(l147));sal_free(l147);
_soc_trident2_alpm_bkt_view_set(l1,l213<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l31)){_soc_trident2_alpm_bkt_view_set(l1,(l213+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l25,l31);if(VRF_TRIE_ROUTES_CNT(l1,
l25,l31) == 0){l131 = l30(l1,l25,l31);}SOC_ALPM_LPM_UNLOCK(l1);return l131;
l219:l178 = trie_insert(l180,prefix,NULL,l136,(trie_node_t*)l216);if(
SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l217:l178 = trie_insert(trie,prefix,NULL,l136,(
trie_node_t*)l183);if(SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}SOC_ALPM_LPM_UNLOCK(l1);return l131;}int
soc_alpm_128_init(int l1){int l131 = SOC_E_NONE;l131 = soc_alpm_128_lpm_init(
l1);SOC_IF_ERROR_RETURN(l131);return l131;}int soc_alpm_128_state_clear(int l1
){int l130,l131;for(l130 = 0;l130<= SOC_VRF_MAX(l1)+1;l130++){l131 = 
trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,l130),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l131)){trie_destroy(
VRF_PREFIX_TRIE_IPV6_128(l1,l130));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n"),l1,
l130));return l131;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l130)!= NULL){
sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l130));}}return SOC_E_NONE;}int
soc_alpm_128_deinit(int l1){soc_alpm_128_lpm_deinit(l1);SOC_IF_ERROR_RETURN(
soc_alpm_128_state_clear(l1));return SOC_E_NONE;}static int l221(int l1,int
l25,int l31){defip_pair_128_entry_t*lpm_entry,l222;int l223;int index;int l131
= SOC_E_NONE;uint32 key[5] = {0,0,0,0,0};uint32 l136;alpm_bucket_handle_t*
l186;alpm_pivot_t*l147;payload_t*l216;trie_t*l224;trie_t*l225 = NULL;
trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(l1,l25));l225 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l25);trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6_128(l1,l25));l224 = VRF_PREFIX_TRIE_IPV6_128(l1,l25);
lpm_entry = sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(
lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: unable to allocate memory ""for IPv6-128 LPM entry\n")
));return SOC_E_MEMORY;}l27(l1,key,0,l25,l31,lpm_entry,0,1);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25) = lpm_entry;if(l25 == SOC_VRF_MAX(l1)
+1){soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,GLOBAL_ROUTEf,1);}else{
soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,DEFAULT_MISSf,1);}l131 = 
alpm_bucket_assign(l1,&l223,l31);if(SOC_FAILURE(l131)){return l131;}
soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,ALG_BKT_PTRf,l223);sal_memcpy
(&l222,lpm_entry,sizeof(l222));l131 = l2(l1,&l222,&index,0);if(SOC_FAILURE(
l131)){return l131;}l186 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l186 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_128_vrf_add: Unable to allocate memory ""for bucket handle \n"))
);return SOC_E_NONE;}sal_memset(l186,0,sizeof(*l186));l147 = sal_alloc(sizeof
(alpm_pivot_t),"Payload for Pivot");if(l147 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: Unable to allocate memory ""for PIVOT trie node \n")))
;sal_free(l186);return SOC_E_MEMORY;}l216 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l216 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_128_vrf_add: Unable to allocate memory "
"for pfx trie node \n")));sal_free(l186);sal_free(l147);return SOC_E_MEMORY;}
sal_memset(l147,0,sizeof(*l147));sal_memset(l216,0,sizeof(*l216));l136 = 0;
PIVOT_BUCKET_HANDLE(l147) = l186;trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l147));PIVOT_BUCKET_INDEX(l147) = l223;PIVOT_BUCKET_VRF(
l147) = l25;PIVOT_BUCKET_IPV6(l147) = l31;PIVOT_BUCKET_DEF(l147) = TRUE;l147
->key[0] = l216->key[0] = key[0];l147->key[1] = l216->key[1] = key[1];l147->
key[2] = l216->key[2] = key[2];l147->key[3] = l216->key[3] = key[3];l147->key
[4] = l216->key[4] = key[4];l147->len = l216->len = l136;l131 = trie_insert(
l224,key,NULL,l136,&(l216->node));if(SOC_FAILURE(l131)){sal_free(l216);
sal_free(l147);sal_free(l186);return l131;}l131 = trie_insert(l225,key,NULL,
l136,(trie_node_t*)l147);if(SOC_FAILURE(l131)){trie_node_t*l182 = NULL;(void)
trie_delete(l224,key,l136,&l182);sal_free(l216);sal_free(l147);sal_free(l186)
;return l131;}index = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,l31);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(index)<<1) = l147;PIVOT_TCAM_INDEX(
l147) = SOC_ALPM_128_ADDR_LWR(index)<<1;VRF_PIVOT_REF_INC(l1,l25,l31);
VRF_TRIE_INIT_DONE(l1,l25,l31,1);return l131;}static int l30(int l1,int l25,
int l31){defip_pair_128_entry_t*lpm_entry;int l223;int l226;int l131 = 
SOC_E_NONE;uint32 key[2] = {0,0},l135[SOC_MAX_MEM_FIELD_WORDS];payload_t*l183
;alpm_pivot_t*l227;trie_node_t*l182;trie_t*l224;trie_t*l225 = NULL;lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25);l223 = 
soc_L3_DEFIP_PAIR_128m_field32_get(l1,lpm_entry,ALG_BKT_PTRf);l131 = 
alpm_bucket_release(l1,l223,l31);_soc_trident2_alpm_bkt_view_set(l1,l223<<2,
INVALIDm);if(SOC_ALPM_V6_SCALE_CHECK(l1,l31)){_soc_trident2_alpm_bkt_view_set
(l1,(l223+1)<<2,INVALIDm);}l131 = l16(l1,lpm_entry,(void*)l135,&l226);if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n"),
l25,l31));l226 = -1;}l226 = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l226,
l31);l227 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l226)<<1);l131 = l5(l1,
lpm_entry);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),l25,l31));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25) = NULL;l224 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l25);VRF_PREFIX_TRIE_IPV6_128(l1,l25) = NULL;
VRF_TRIE_INIT_DONE(l1,l25,l31,0);l131 = trie_delete(l224,key,0,&l182);l183 = 
(payload_t*)l182;if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),l25,l31));}
sal_free(l183);(void)trie_destroy(l224);l225 = VRF_PIVOT_TRIE_IPV6_128(l1,l25
);VRF_PIVOT_TRIE_IPV6_128(l1,l25) = NULL;l182 = NULL;l131 = trie_delete(l225,
key,0,&l182);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l25,l31));}(void)
trie_destroy(l225);(void)trie_destroy(PIVOT_BUCKET_TRIE(l227));sal_free(
PIVOT_BUCKET_HANDLE(l227));sal_free(l227);return l131;}int soc_alpm_128_insert
(int l1,void*l3,uint32 l22,int l228,int l229){defip_alpm_ipv6_128_entry_t l190
,l191;soc_mem_t l21;void*l192,*l211;int l151,l25;int index;int l8;int l131 = 
SOC_E_NONE;uint32 l23;l8 = L3_DEFIP_MODE_128;l21 = L3_DEFIP_ALPM_IPV6_128m;
l192 = ((uint32*)&(l190));l211 = ((uint32*)&(l191));SOC_IF_ERROR_RETURN(l17(
l1,l3,l192,l211,l21,l22,&l23));SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(
l1,l3,&l151,&l25));if(l151 == SOC_L3_VRF_OVERRIDE){l131 = l2(l1,l3,&index,0);
if(SOC_SUCCESS(l131)){VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l8);VRF_PIVOT_REF_INC
(l1,MAX_VRF_ID,l8);}else if(l131 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,
MAX_VRF_ID,l8);}return(l131);}else if(l25 == 0){if(soc_alpm_mode_get(l1)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}}if(l151!= 
SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l1)){if(VRF_TRIE_ROUTES_CNT(l1,l25,
l8) == 0){if(!l23){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
l151));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l25,l8)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d is not ""initialized\n"),l25));l131 = l221(l1,l25
,l8);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),l25,l8));return l131;
}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d/%d trie init ""completed\n"),l25,l8));}if(l229&
SOC_ALPM_LOOKUP_HIT){l131 = l153(l1,l3,l192,l211,l21,l228);}else{if(l228 == -
1){l228 = 0;}l131 = l173(l1,l3,l21,l192,l211,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l228),l229);}if(l131!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_128_insert: ""Route Insertion Failed :%s\n"),l1,soc_errmsg
(l131)));}return(l131);}int soc_alpm_128_lookup(int l1,void*l6,void*l13,int*
l14,int*l230){defip_alpm_ipv6_128_entry_t l190;soc_mem_t l21;int bucket_index
;int l144;void*l192;int l151,l25;int l8 = 2,l122;int l131 = SOC_E_NONE;
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&l25));l131 = l12(l1
,l6,l13,l14,&l122,&l8);if(SOC_SUCCESS(l131)){SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l13,&l151,&l25));if(l151 == SOC_L3_VRF_OVERRIDE){
return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l25,l8)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_lookup:VRF %d is not "
"initialized\n"),l25));*l14 = 0;*l230 = 0;return SOC_E_NOT_FOUND;}l21 = 
L3_DEFIP_ALPM_IPV6_128m;l192 = ((uint32*)&(l190));SOC_ALPM_LPM_LOCK(l1);l131 = 
l148(l1,l6,l21,l192,&l144,&bucket_index,l14,TRUE);SOC_ALPM_LPM_UNLOCK(l1);if(
SOC_FAILURE(l131)){*l230 = l144;*l14 = bucket_index<<2;return l131;}l131 = 
l24(l1,l192,l21,l8,l151,bucket_index,*l14,l13);*l230 = SOC_ALPM_LOOKUP_HIT|
l144;return(l131);}static int l231(int l1,void*l6,void*l13,int l25,int*l144,
int*bucket_index,int*l123,int l232){int l131 = SOC_E_NONE;int l130,l233,l31,
l143 = 0;uint32 l9,l152;defip_aux_scratch_entry_t l11;int l234,l235;int index
;soc_mem_t l21,l236;int l237,l238,l239;soc_field_t l240[4] = {IP_ADDR0_LWRf,
IP_ADDR1_LWRf,IP_ADDR0_UPRf,IP_ADDR1_UPRf};uint32 l241[
SOC_MAX_MEM_FIELD_WORDS] = {0};int l242 = -1;int l243 = 0;l31 = 
L3_DEFIP_MODE_128;l236 = L3_DEFIP_PAIR_128m;l234 = soc_mem_field32_get(l1,
l236,l6,GLOBAL_ROUTEf);l235 = soc_mem_field32_get(l1,l236,l6,VRF_ID_0_LWRf);
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d\n"),l25 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l31,l234,l235));if(l25 == SOC_L3_VRF_GLOBAL){l9 = l232?1:0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l152);soc_mem_field32_set(l1,l236,l6,
GLOBAL_ROUTEf,1);soc_mem_field32_set(l1,l236,l6,VRF_ID_0_LWRf,0);}else{l9 = 
l232?3:2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l152);}sal_memset(&l11,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l7(l1,l6,l31,l9,0,&l11));if(
l25 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l1,l236,l6,GLOBAL_ROUTEf,l234);
soc_mem_field32_set(l1,l236,l6,VRF_ID_0_LWRf,l235);}SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l11,TRUE,&l143,l144,bucket_index));if(l143
== 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Could not find bucket\n")))
;return SOC_E_NOT_FOUND;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in memory %s, index %d, ""bucket_index %d\n"),SOC_MEM_NAME(l1,l236),
soc_alpm_logical_idx(l1,l236,SOC_ALPM_128_DEFIP_TO_PAIR((*l144)>>1),1),*
bucket_index));l21 = L3_DEFIP_ALPM_IPV6_128m;l131 = l129(l1,l6,&l238);if(
SOC_FAILURE(l131)){return l131;}l239 = SOC_ALPM_V6_SCALE_CHECK(l1,l31)?16:8;
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Start searching mem %s bucket %d(count %d) ""for Length %d\n"),SOC_MEM_NAME(
l1,l21),*bucket_index,l239,l238));for(l130 = 0;l130<l239;l130++){uint32 l192[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l244[4] = {0};uint32 l245[4] = {0};
uint32 l246[4] = {0};int l247;l131 = _soc_alpm_mem_index(l1,l21,*bucket_index
,l130,l152,&index);if(l131 == SOC_E_FULL){continue;}SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l21,MEM_BLOCK_ANY,index,(void*)&l192));l247 = 
soc_mem_field32_get(l1,l21,&l192,VALIDf);l237 = soc_mem_field32_get(l1,l21,&
l192,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket %5d Index %6d: Valid %d, Length %d\n"),*bucket_index,index,l247,l237)
);if(!l247||(l237>l238)){continue;}SHR_BITSET_RANGE(l244,128-l237,l237);(void
)soc_mem_field_get(l1,l21,(uint32*)&l192,KEYf,(uint32*)l245);l246[3] = 
soc_mem_field32_get(l1,l236,l6,l240[3]);l246[2] = soc_mem_field32_get(l1,l236
,l6,l240[2]);l246[1] = soc_mem_field32_get(l1,l236,l6,l240[1]);l246[0] = 
soc_mem_field32_get(l1,l236,l6,l240[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tmask %08x %08x %08x %08x \n""\t key %08x %08x %08x %08x \n"
"\thost %08x %08x %08x %08x \n"),l244[3],l244[2],l244[1],l244[0],l245[3],l245
[2],l245[1],l245[0],l246[3],l246[2],l246[1],l246[0]));for(l233 = 3;l233>= 0;
l233--){if((l246[l233]&l244[l233])!= (l245[l233]&l244[l233])){break;}}if(l233
>= 0){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket %d, ""index %d\n"),SOC_MEM_NAME(l1,l21),*
bucket_index,index));if(l242 == -1||l242<l237){l242 = l237;l243 = index;
sal_memcpy(l241,l192,sizeof(l192));}}if(l242!= -1){l131 = l24(l1,&l241,l21,
l31,l25,*bucket_index,l243,l13);if(SOC_SUCCESS(l131)){*l123 = l243;if(
bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit mem %s bucket %d, index %d\n"),
SOC_MEM_NAME(l1,l21),*bucket_index,l243));}}return l131;}*l123 = 
soc_alpm_logical_idx(l1,l236,SOC_ALPM_128_DEFIP_TO_PAIR((*l144)>>1),1);
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l236,MEM_BLOCK_ANY,*l123,(void*)l13));
return SOC_E_NONE;}int soc_alpm_128_find_best_match(int l1,void*l6,void*l13,
int*l14,int l232){int l131 = SOC_E_NONE;int l130,l233;int l248,l249;
defip_pair_128_entry_t l250;uint32 l251,l245,l246;int l237,l238;int l252,l253
;int l151,l25 = 0;int l144,bucket_index;soc_mem_t l236;soc_field_t l254[4] = 
{IP_ADDR_MASK1_UPRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_LWRf}
;soc_field_t l255[4] = {IP_ADDR1_UPRf,IP_ADDR0_UPRf,IP_ADDR1_LWRf,
IP_ADDR0_LWRf};l236 = L3_DEFIP_PAIR_128m;if(!SOC_URPF_STATUS_GET(l1)&&l232){
return SOC_E_PARAM;}l248 = soc_mem_index_min(l1,l236);l249 = 
soc_mem_index_count(l1,l236);if(SOC_URPF_STATUS_GET(l1)){l249>>= 1;}if(
soc_alpm_mode_get(l1)){l249>>= 1;l248+= l249;}if(l232){l248+= 
soc_mem_index_count(l1,l236)/2;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Start LPM searchng from %d, count %d\n"),l248,l249));for(l130 = l248;l130<
l248+l249;l130++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l236,MEM_BLOCK_ANY,l130
,(void*)&l250));if(!soc_mem_field32_get(l1,l236,&l250,VALID0_LWRf)){continue;
}l252 = soc_mem_field32_get(l1,l236,&l250,GLOBAL_HIGHf);l253 = 
soc_mem_field32_get(l1,l236,&l250,GLOBAL_ROUTEf);if(!l253||!l252){continue;}
l131 = l129(l1,l6,&l238);l131 = l129(l1,&l250,&l237);if(SOC_FAILURE(l131)||(
l237>l238)){continue;}for(l233 = 0;l233<4;l233++){l251 = soc_mem_field32_get(
l1,l236,&l250,l254[l233]);l245 = soc_mem_field32_get(l1,l236,&l250,l255[l233]
);l246 = soc_mem_field32_get(l1,l236,l6,l255[l233]);if((l246&l251)!= (l245&
l251)){break;}}if(l233<4){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Hit Global High route in index = %d\n"),l233));sal_memcpy(l13,&l250,
sizeof(l250));*l14 = l130;return SOC_E_NONE;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for Global Low and VRF routes\n")));SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&l25));l131 = l231(l1,l6,l13,l25,&l144,&
bucket_index,l14,l232);if(l131 == SOC_E_NOT_FOUND){l25 = SOC_L3_VRF_GLOBAL;
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Not found in VRF region, try Global ""region\n")));l131 = l231(l1,l6,l13,l25
,&l144,&bucket_index,l14,l232);}if(SOC_SUCCESS(l131)){l144 = 
soc_alpm_logical_idx(l1,l236,SOC_ALPM_128_DEFIP_TO_PAIR(l144>>1),1);
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in %s region in TCAM index %d, ""buckekt_index %d\n"),l25 == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",l144,bucket_index));}else{LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Search miss for given address\n")));}return(
l131);}int soc_alpm_128_delete(int l1,void*l6,int l228,int l229){int l151,l25
;int l8;int l131 = SOC_E_NONE;int l209 = 0;l8 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&l25));if(l151 == 
SOC_L3_VRF_OVERRIDE){l131 = l5(l1,l6);if(SOC_SUCCESS(l131)){
VRF_TRIE_ROUTES_DEC(l1,MAX_VRF_ID,l8);VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l8);}
return(l131);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l25,l8)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_128_delete:VRF %d/%d is not "
"initialized\n"),l25,l8));return SOC_E_NONE;}if(l228 == -1){l228 = 0;}l209 = 
!(l229&SOC_ALPM_DELETE_ALL);l131 = l208(l1,l6,SOC_ALPM_BKT_ENTRY_TO_IDX(l228)
,l229&~SOC_ALPM_LOOKUP_HIT,l228,l209);}return(l131);}static void l110(int l1,
void*l13,int index,l105 l111){l111[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l57));l111[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l55));l111[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l61));l111[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l59));l111[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l58));l111[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l56));l111[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l62));l111[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l60));if((!(SOC_IS_HURRICANE(l1)))&&(((l98[(l1)]->l80)!= NULL))
){int l256;(void)soc_alpm_128_lpm_vrf_get(l1,l13,(int*)&l111[8],&l256);}else{
l111[8] = 0;};}static int l257(l105 l107,l105 l108){int l226;for(l226 = 0;
l226<9;l226++){{if((l107[l226])<(l108[l226])){return-1;}if((l107[l226])>(l108
[l226])){return 1;}};}return(0);}static void l258(int l1,void*l3,uint32 l259,
uint32 l125,int l122){l105 l260;if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l77))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l76))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l75))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l74))){l260[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l57));l260[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l55));l260[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l61));l260[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l59));l260[4] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l58));l260[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l56));l260[6] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l62));l260[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l60));if((!(SOC_IS_HURRICANE(l1)))&&(((l98[(l1)]->l80)!= NULL)))
{int l256;(void)soc_alpm_128_lpm_vrf_get(l1,l3,(int*)&l260[8],&l256);}else{
l260[8] = 0;};l124((l109[(l1)]),l257,l260,l122,l125,l259);}}static void l261(
int l1,void*l6,uint32 l259){l105 l260;int l122 = -1;int l131;uint16 index;
l260[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l57));l260[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l55));l260[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l61));l260[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l59));l260[4] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l58));l260[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l56));l260[6] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l62));l260[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l60));if((!(SOC_IS_HURRICANE(l1)))&&(((l98[(l1)]->l80)!= NULL)))
{int l256;(void)soc_alpm_128_lpm_vrf_get(l1,l6,(int*)&l260[8],&l256);}else{
l260[8] = 0;};index = l259;l131 = l127((l109[(l1)]),l257,l260,l122,index);if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\ndel  index: H %d error %d\n"),index,l131));}}static int l262(int l1,void*
l6,int l122,int*l123){l105 l260;int l131;uint16 index = (0xFFFF);l260[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l57));l260[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l55));l260[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l61));l260[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l59));l260[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l58));l260[5] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l56));l260[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l62));l260[7] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l60));if((!(SOC_IS_HURRICANE(l1)))
&&(((l98[(l1)]->l80)!= NULL))){int l256;(void)soc_alpm_128_lpm_vrf_get(l1,l6,
(int*)&l260[8],&l256);}else{l260[8] = 0;};l131 = l119((l109[(l1)]),l257,l260,
l122,&index);if(SOC_FAILURE(l131)){*l123 = 0xFFFFFFFF;return(l131);}*l123 = 
index;return(SOC_E_NONE);}static uint16 l112(uint8*l113,int l114){return(
_shr_crc16b(0,l113,l114));}static int l115(int l18,int l100,int l101,l104**
l116){l104*l120;int index;if(l101>l100){return SOC_E_MEMORY;}l120 = sal_alloc
(sizeof(l104),"lpm_hash");if(l120 == NULL){return SOC_E_MEMORY;}sal_memset(
l120,0,sizeof(*l120));l120->l18 = l18;l120->l100 = l100;l120->l101 = l101;
l120->l102 = sal_alloc(l120->l101*sizeof(*(l120->l102)),"hash_table");if(l120
->l102 == NULL){sal_free(l120);return SOC_E_MEMORY;}l120->l103 = sal_alloc(
l120->l100*sizeof(*(l120->l103)),"link_table");if(l120->l103 == NULL){
sal_free(l120->l102);sal_free(l120);return SOC_E_MEMORY;}for(index = 0;index<
l120->l101;index++){l120->l102[index] = (0xFFFF);}for(index = 0;index<l120->
l100;index++){l120->l103[index] = (0xFFFF);}*l116 = l120;return SOC_E_NONE;}
static int l117(l104*l118){if(l118!= NULL){sal_free(l118->l102);sal_free(l118
->l103);sal_free(l118);}return SOC_E_NONE;}static int l119(l104*l120,l106 l121
,l105 entry,int l122,uint16*l123){int l1 = l120->l18;uint16 l263;uint16 index
;l263 = l112((uint8*)entry,(32*9))%l120->l101;index = l120->l102[l263];;;
while(index!= (0xFFFF)){uint32 l13[SOC_MAX_MEM_FIELD_WORDS];l105 l111;int l264
;l264 = index;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,
l264,l13));l110(l1,l13,index,l111);if((*l121)(entry,l111) == 0){*l123 = index
;;return(SOC_E_NONE);}index = l120->l103[index&(0x3FFF)];;};return(
SOC_E_NOT_FOUND);}static int l124(l104*l120,l106 l121,l105 entry,int l122,
uint16 l125,uint16 l126){int l1 = l120->l18;uint16 l263;uint16 index;uint16
l265;l263 = l112((uint8*)entry,(32*9))%l120->l101;index = l120->l102[l263];;;
;l265 = (0xFFFF);if(l125!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l13[
SOC_MAX_MEM_FIELD_WORDS];l105 l111;int l264;l264 = index;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l264,l13));l110(l1,l13,index,l111);
if((*l121)(entry,l111) == 0){if(l126!= index){;if(l265 == (0xFFFF)){l120->
l102[l263] = l126;l120->l103[l126&(0x3FFF)] = l120->l103[index&(0x3FFF)];l120
->l103[index&(0x3FFF)] = (0xFFFF);}else{l120->l103[l265&(0x3FFF)] = l126;l120
->l103[l126&(0x3FFF)] = l120->l103[index&(0x3FFF)];l120->l103[index&(0x3FFF)]
= (0xFFFF);}};return(SOC_E_NONE);}l265 = index;index = l120->l103[index&(
0x3FFF)];;}}l120->l103[l126&(0x3FFF)] = l120->l102[l263];l120->l102[l263] = 
l126;return(SOC_E_NONE);}static int l127(l104*l120,l106 l121,l105 entry,int
l122,uint16 l128){uint16 l263;uint16 index;uint16 l265;l263 = l112((uint8*)
entry,(32*9))%l120->l101;index = l120->l102[l263];;;l265 = (0xFFFF);while(
index!= (0xFFFF)){if(l128 == index){;if(l265 == (0xFFFF)){l120->l102[l263] = 
l120->l103[l128&(0x3FFF)];l120->l103[l128&(0x3FFF)] = (0xFFFF);}else{l120->
l103[l265&(0x3FFF)] = l120->l103[l128&(0x3FFF)];l120->l103[l128&(0x3FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l265 = index;index = l120->l103[index&(0x3FFF)];
;}return(SOC_E_NOT_FOUND);}static int l266(int l1,void*l13){return(SOC_E_NONE
);}void soc_alpm_128_lpm_state_dump(int l1){int l130;int l267;l267 = ((3*(128
+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){
return;}for(l130 = l267;l130>= 0;l130--){if((l130!= ((3*(128+2+1))-1))&&((l40
[(l1)][(l130)].l33) == -1)){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"PFX = %d P = %d N = %d START = %d ""END = %d VENT = %d FENT = %d\n"),
l130,(l40[(l1)][(l130)].l35),(l40[(l1)][(l130)].next),(l40[(l1)][(l130)].l33)
,(l40[(l1)][(l130)].l34),(l40[(l1)][(l130)].l36),(l40[(l1)][(l130)].l37)));}
COMPILER_REFERENCE(l266);}static int l268(int l1,int l269,int index,uint32*
l13,int l4){int l270;uint32 l271,l272,l273;uint32 l274;int l275;uint32 l276[
SOC_MAX_MEM_FIELD_WORDS] = {0};if(!SOC_URPF_STATUS_GET(l1)){return(SOC_E_NONE
);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l270 = (soc_mem_index_count(
l1,L3_DEFIP_PAIR_128m)>>1);}else if(SOC_IS_APOLLO(l1)){l270 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l270 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}if(l4>= 2){
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l269+l270,l276))
;l4 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)
),(l276),(l98[(l1)]->l73));}if(((l98[(l1)]->l43)!= NULL)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l43),(0));}l271 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l61));l274 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l62));l272 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l59));l273 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l60));l275 = ((!l271)&&(!l274)&&(!l272)&&(!l273))?1:0;l271 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l76));l274 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l74));l272 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l75));l273 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l75));if(l271&&l274&&l272&&l273){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l73),(l4|l275));}return l170(l1,MEM_BLOCK_ANY,index+l270,index,
l13);}static int l277(int l1,int l278,int l279){uint32 l13[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l278,l13));l258(l1,l13,l279,0x4000,0);SOC_IF_ERROR_RETURN(l170(
l1,MEM_BLOCK_ANY,l279,l278,l13));SOC_IF_ERROR_RETURN(l268(l1,l278,l279,l13,2)
);do{int l280,l281;l280 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l278
),1);l281 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l279),1);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l281))<<1) = ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l280))<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR
((l281))<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l281
))<<1)) = SOC_ALPM_128_ADDR_LWR((l281))<<1;}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l280))<<1) = NULL;}while(0);return(SOC_E_NONE);}static
int l282(int l1,int l122,int l8){int l278;int l279;l279 = (l40[(l1)][(l122)].
l34)+1;l278 = (l40[(l1)][(l122)].l33);if(l278!= l279){SOC_IF_ERROR_RETURN(
l277(l1,l278,l279));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l8);}(l40[(l1)][(l122)]
.l33)+= 1;(l40[(l1)][(l122)].l34)+= 1;return(SOC_E_NONE);}static int l283(int
l1,int l122,int l8){int l278;int l279;l279 = (l40[(l1)][(l122)].l33)-1;if((
l40[(l1)][(l122)].l36) == 0){(l40[(l1)][(l122)].l33) = l279;(l40[(l1)][(l122)
].l34) = l279-1;return(SOC_E_NONE);}l278 = (l40[(l1)][(l122)].l34);
SOC_IF_ERROR_RETURN(l277(l1,l278,l279));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l8)
;(l40[(l1)][(l122)].l33)-= 1;(l40[(l1)][(l122)].l34)-= 1;return(SOC_E_NONE);}
static int l284(int l1,int l122,int l8,void*l13,int*l285){int l286;int l287;
int l288;int l289;if((l40[(l1)][(l122)].l36) == 0){l289 = ((3*(128+2+1))-1);
if(soc_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL){if(l122<= (((3*(128+2+1))
/3)-1)){l289 = (((3*(128+2+1))/3)-1);}}while((l40[(l1)][(l289)].next)>l122){
l289 = (l40[(l1)][(l289)].next);}l287 = (l40[(l1)][(l289)].next);if(l287!= -1
){(l40[(l1)][(l287)].l35) = l122;}(l40[(l1)][(l122)].next) = (l40[(l1)][(l289
)].next);(l40[(l1)][(l122)].l35) = l289;(l40[(l1)][(l289)].next) = l122;(l40[
(l1)][(l122)].l37) = ((l40[(l1)][(l289)].l37)+1)/2;(l40[(l1)][(l289)].l37)-= 
(l40[(l1)][(l122)].l37);(l40[(l1)][(l122)].l33) = (l40[(l1)][(l289)].l34)+(
l40[(l1)][(l289)].l37)+1;(l40[(l1)][(l122)].l34) = (l40[(l1)][(l122)].l33)-1;
(l40[(l1)][(l122)].l36) = 0;}l288 = l122;while((l40[(l1)][(l288)].l37) == 0){
l288 = (l40[(l1)][(l288)].next);if(l288 == -1){l288 = l122;break;}}while((l40
[(l1)][(l288)].l37) == 0){l288 = (l40[(l1)][(l288)].l35);if(l288 == -1){if((
l40[(l1)][(l122)].l36) == 0){l286 = (l40[(l1)][(l122)].l35);l287 = (l40[(l1)]
[(l122)].next);if(-1!= l286){(l40[(l1)][(l286)].next) = l287;}if(-1!= l287){(
l40[(l1)][(l287)].l35) = l286;}}return(SOC_E_FULL);}}while(l288>l122){l287 = 
(l40[(l1)][(l288)].next);SOC_IF_ERROR_RETURN(l283(l1,l287,l8));(l40[(l1)][(
l288)].l37)-= 1;(l40[(l1)][(l287)].l37)+= 1;l288 = l287;}while(l288<l122){
SOC_IF_ERROR_RETURN(l282(l1,l288,l8));(l40[(l1)][(l288)].l37)-= 1;l286 = (l40
[(l1)][(l288)].l35);(l40[(l1)][(l286)].l37)+= 1;l288 = l286;}(l40[(l1)][(l122
)].l36)+= 1;(l40[(l1)][(l122)].l37)-= 1;(l40[(l1)][(l122)].l34)+= 1;*l285 = (
l40[(l1)][(l122)].l34);sal_memcpy(l13,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);return(
SOC_E_NONE);}static int l290(int l1,int l122,int l8,void*l13,int l291){int
l286;int l287;int l278;int l279;uint32 l292[SOC_MAX_MEM_FIELD_WORDS];int l131
;int l138;l278 = (l40[(l1)][(l122)].l34);l279 = l291;(l40[(l1)][(l122)].l36)
-= 1;(l40[(l1)][(l122)].l37)+= 1;(l40[(l1)][(l122)].l34)-= 1;if(l279!= l278){
if((l131 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l278,l292))<0){return l131
;}l258(l1,l292,l279,0x4000,0);if((l131 = l170(l1,MEM_BLOCK_ANY,l279,l278,l292
))<0){return l131;}if((l131 = l268(l1,l278,l279,l292,2))<0){return l131;}}
l138 = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l279,1);l291 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l278,1);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l138)<<1) = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
l291)<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l138)<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l138)<<1)) = 
SOC_ALPM_128_ADDR_LWR(l138)<<1;}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l291
)<<1) = NULL;sal_memcpy(l292,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),
soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);l279 = l278;l258(l1,l292,l279,
0x4000,0);if((l131 = l170(l1,MEM_BLOCK_ANY,l279,l278,l292))<0){return l131;}
if((l131 = l268(l1,l278,l278,l292,0))<0){return l131;}if((l40[(l1)][(l122)].
l36) == 0){l286 = (l40[(l1)][(l122)].l35);assert(l286!= -1);l287 = (l40[(l1)]
[(l122)].next);(l40[(l1)][(l286)].next) = l287;(l40[(l1)][(l286)].l37)+= (l40
[(l1)][(l122)].l37);(l40[(l1)][(l122)].l37) = 0;if(l287!= -1){(l40[(l1)][(
l287)].l35) = l286;}(l40[(l1)][(l122)].next) = -1;(l40[(l1)][(l122)].l35) = -
1;(l40[(l1)][(l122)].l33) = -1;(l40[(l1)][(l122)].l34) = -1;}return(l131);}
int soc_alpm_128_lpm_vrf_get(int l18,void*lpm_entry,int*l25,int*l293){int l151
;if(((l98[(l18)]->l84)!= NULL)){l151 = soc_L3_DEFIP_PAIR_128m_field32_get(l18
,lpm_entry,VRF_ID_0_LWRf);*l293 = l151;if(soc_L3_DEFIP_PAIR_128m_field32_get(
l18,lpm_entry,VRF_ID_MASK0_LWRf)){*l25 = l151;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l86))){*l25 = SOC_L3_VRF_GLOBAL;*l293 = SOC_VRF_MAX(
l18)+1;}else{*l25 = SOC_L3_VRF_OVERRIDE;}}else{*l25 = SOC_L3_VRF_DEFAULT;}
return(SOC_E_NONE);}static int l294(int l1,void*entry,int*l15){int l122=0;int
l131;int l151;int l295;l131 = l129(l1,entry,&l122);if(l131<0){return l131;}
l122+= 0;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,entry,&l151,&l131));
l295 = soc_alpm_mode_get(l1);switch(l151){case SOC_L3_VRF_GLOBAL:if(l295 == 
SOC_ALPM_MODE_PARALLEL){*l15 = l122+((3*(128+2+1))/3);}else{*l15 = l122;}
break;case SOC_L3_VRF_OVERRIDE:*l15 = l122+2*((3*(128+2+1))/3);break;default:
if(l295 == SOC_ALPM_MODE_PARALLEL){*l15 = l122;}else{*l15 = l122+((3*(128+2+1
))/3);}break;}return(SOC_E_NONE);}static int l12(int l1,void*l6,void*l13,int*
l14,int*l15,int*l8){int l131;int l123;int l122 = 0;*l8 = L3_DEFIP_MODE_128;
l294(l1,l6,&l122);*l15 = l122;if(l262(l1,l6,l122,&l123) == SOC_E_NONE){*l14 = 
l123;if((l131 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(*l8)?*l14:(*l14>>1)
,l13))<0){return l131;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}int
soc_alpm_128_ipmc_war(int l18,int l296){int index = -1;defip_pair_128_entry_t
lpm_entry;soc_mem_t l21 = L3_DEFIP_PAIR_128m;int l297 = 63;if(!
soc_property_get(l18,spn_L3_ALPM_ENABLE,0)){return SOC_E_NONE;}if(
soc_mem_index_count(l18,L3_DEFIP_PAIR_128m)<= 0){return SOC_E_NONE;}if(!
SOC_IS_TRIDENT2PLUS(l18)){return SOC_E_NONE;}if(SOC_WARM_BOOT(l18)){return
SOC_E_NONE;}l297 = soc_mem_field_length(l18,ING_ACTIVE_L3_IIF_PROFILEm,
RPA_ID_PROFILEf);sal_memset(&lpm_entry,0,soc_mem_entry_words(l18,l21)*4);
soc_mem_field32_set(l18,l21,&lpm_entry,VALID0_LWRf,1);soc_mem_field32_set(l18
,l21,&lpm_entry,VALID1_LWRf,1);soc_mem_field32_set(l18,l21,&lpm_entry,
VALID0_UPRf,1);soc_mem_field32_set(l18,l21,&lpm_entry,VALID1_UPRf,1);
soc_mem_field32_set(l18,l21,&lpm_entry,MODE0_LWRf,3);soc_mem_field32_set(l18,
l21,&lpm_entry,MODE1_LWRf,3);soc_mem_field32_set(l18,l21,&lpm_entry,
MODE0_UPRf,3);soc_mem_field32_set(l18,l21,&lpm_entry,MODE1_UPRf,3);
soc_mem_field32_set(l18,l21,&lpm_entry,MODE_MASK0_LWRf,3);soc_mem_field32_set
(l18,l21,&lpm_entry,MODE_MASK1_LWRf,3);soc_mem_field32_set(l18,l21,&lpm_entry
,MODE_MASK0_UPRf,3);soc_mem_field32_set(l18,l21,&lpm_entry,MODE_MASK1_UPRf,3)
;soc_mem_field32_set(l18,l21,&lpm_entry,ENTRY_TYPE_MASK0_LWRf,1);
soc_mem_field32_set(l18,l21,&lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);
soc_mem_field32_set(l18,l21,&lpm_entry,ENTRY_TYPE_MASK0_UPRf,1);
soc_mem_field32_set(l18,l21,&lpm_entry,ENTRY_TYPE_MASK1_UPRf,1);
soc_mem_field32_set(l18,l21,&lpm_entry,MULTICAST_ROUTEf,1);
soc_mem_field32_set(l18,l21,&lpm_entry,GLOBAL_ROUTEf,1);soc_mem_field32_set(
l18,l21,&lpm_entry,RPA_IDf,l297-1);soc_mem_field32_set(l18,l21,&lpm_entry,
EXPECTED_L3_IIFf,16383);soc_mem_field32_set(l18,l21,&lpm_entry,IP_ADDR0_LWRf,
0);soc_mem_field32_set(l18,l21,&lpm_entry,IP_ADDR1_LWRf,0);
soc_mem_field32_set(l18,l21,&lpm_entry,IP_ADDR0_UPRf,0);soc_mem_field32_set(
l18,l21,&lpm_entry,IP_ADDR1_UPRf,0xff000000);soc_mem_field32_set(l18,l21,&
lpm_entry,IP_ADDR_MASK0_LWRf,0);soc_mem_field32_set(l18,l21,&lpm_entry,
IP_ADDR_MASK1_UPRf,0);soc_mem_field32_set(l18,l21,&lpm_entry,
IP_ADDR_MASK0_LWRf,0);soc_mem_field32_set(l18,l21,&lpm_entry,
IP_ADDR_MASK1_UPRf,0xff000000);if(l296){SOC_IF_ERROR_RETURN(l2(l18,&lpm_entry
,&index,0));}else{SOC_IF_ERROR_RETURN(l5(l18,&lpm_entry));}return SOC_E_NONE;
}int soc_alpm_128_lpm_init(int l1){int l267;int l130;int l298;int l299;if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}l267 = (3*(128+2
+1));l299 = sizeof(l38)*(l267);if((l40[(l1)]!= NULL)){SOC_IF_ERROR_RETURN(
soc_alpm_128_deinit(l1));}l98[l1] = sal_alloc(sizeof(l96),
"lpm_128_field_state");if(NULL == l98[l1]){return(SOC_E_MEMORY);}(l98[l1])->
l42 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l98[l1])->l43 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l98[l1])->l44 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l98[l1])->l45 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l98[l1])->l46 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l98[l1])->l47 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l98[l1])->l48 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MULTICAST_ROUTEf);(l98[l1])->l49 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPA_IDf);(l98[l1])->l50 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,EXPECTED_L3_IIFf);(l98[l1])->l51 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf);(l98[l1])->l52 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf);(l98[l1])->l53 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,L3MC_INDEXf);(l98[l1])->l54 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l98[l1])->l57 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l98[l1])->l55 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l98[l1])->l58 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l98[l1])->l56 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l98[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l98[l1])->
l59 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l98[l1
])->l62 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l98[l1])->l60 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l98[l1])->l65 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l98[l1])->l63 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l98[l1])->l66 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l98[l1])->l64 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l98[l1])->l69 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l98[l1])->l67 = soc_mem_fieldinfo_get(l1
,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l98[l1])->l70 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l98[l1])->l68 = soc_mem_fieldinfo_get
(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l98[l1])->l71 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l98[l1])->l72 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l98[l1])->l73 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l98[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l98[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l98[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l98[l1])->l75 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l98[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l98[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l98[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l98[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l98[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l98[l1])->l82
= soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l98[l1])->
l85 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(l98[l1]
)->l83 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_UPRf);(l98[
l1])->l86 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l98[l1
])->l87 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l98[l1])
->l88 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l98[l1])->
l89 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l98[l1])->
l90 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf)
;(l98[l1])->l91 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
FLEX_CTR_POOL_NUMBERf);(l98[l1])->l92 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf);(l98[l1])->l93 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_LWRf);(l98[l1])
->l94 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf);(
l98[l1])->l95 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
ENTRY_TYPE_MASK1_UPRf);(l40[(l1)]) = sal_alloc(l299,"LPM 128 prefix info");if
(NULL == (l40[(l1)])){sal_free(l98[l1]);l98[l1] = NULL;return(SOC_E_MEMORY);}
SOC_ALPM_LPM_LOCK(l1);sal_memset((l40[(l1)]),0,l299);for(l130 = 0;l130<l267;
l130++){(l40[(l1)][(l130)].l33) = -1;(l40[(l1)][(l130)].l34) = -1;(l40[(l1)][
(l130)].l35) = -1;(l40[(l1)][(l130)].next) = -1;(l40[(l1)][(l130)].l36) = 0;(
l40[(l1)][(l130)].l37) = 0;}l298 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)
;if(SOC_URPF_STATUS_GET(l1)){l298>>= 1;}if(soc_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL){(l40[(l1)][(((3*(128+2+1))-1))].l34) = (l298>>1)-1;(
l40[(l1)][(((((3*(128+2+1))/3)-1)))].l37) = l298>>1;(l40[(l1)][((((3*(128+2+1
))-1)))].l37) = (l298-(l40[(l1)][(((((3*(128+2+1))/3)-1)))].l37));}else{(l40[
(l1)][((((3*(128+2+1))-1)))].l37) = l298;}if((l109[(l1)])!= NULL){if(l117((
l109[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(l109[(l1)]) = 
NULL;}if(l115(l1,l298*2,l298,&(l109[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_MEMORY;}(void)soc_alpm_128_ipmc_war(l1,TRUE);SOC_ALPM_LPM_UNLOCK(l1);
return(SOC_E_NONE);}int soc_alpm_128_lpm_deinit(int l1){if(!soc_feature(l1,
soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);(void)
soc_alpm_128_ipmc_war(l1,FALSE);if((l109[(l1)])!= NULL){l117((l109[(l1)]));(
l109[(l1)]) = NULL;}if((l40[(l1)]!= NULL)){sal_free(l98[l1]);l98[l1] = NULL;
sal_free((l40[(l1)]));(l40[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l1);return(
SOC_E_NONE);}static int l2(int l1,void*l3,int*l300,int l4){int l122;int index
;int l8;uint32 l13[SOC_MAX_MEM_FIELD_WORDS];int l131 = SOC_E_NONE;int l301 = 
0;if(l4>= 2){return SOC_E_INTERNAL;}sal_memcpy(l13,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);
SOC_ALPM_LPM_LOCK(l1);l131 = l12(l1,l3,l13,&index,&l122,&l8);if(l131 == 
SOC_E_NOT_FOUND){l131 = l284(l1,l122,l8,l13,&index);if(l131<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l131);}}else{l301 = 1;}*l300 = index;if(l131 == 
SOC_E_NONE){soc_alpm_128_lpm_state_dump(l1);LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\nsoc_alpm_128_lpm_insert: %d %d\n"),index,l122));if(!l301){
l258(l1,l3,index,0x4000,0);}l131 = l170(l1,MEM_BLOCK_ANY,index,index,l3);if(
l131>= 0){l131 = l268(l1,index,index,l3,l4);}}SOC_ALPM_LPM_UNLOCK(l1);return(
l131);}static int l5(int l1,void*l6){int l122;int index;int l8;uint32 l13[
SOC_MAX_MEM_FIELD_WORDS];int l131 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l131 = 
l12(l1,l6,l13,&index,&l122,&l8);if(l131 == SOC_E_NONE){LOG_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_alpm_lpm_delete: %d %d\n"),index,l122))
;l261(l1,l6,index);l131 = l290(l1,l122,l8,l13,index);}
soc_alpm_128_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l131);}static
int l16(int l1,void*l6,void*l13,int*l14){int l122;int l131;int l8;
SOC_ALPM_LPM_LOCK(l1);l131 = l12(l1,l6,l13,l14,&l122,&l8);SOC_ALPM_LPM_UNLOCK
(l1);return(l131);}static int l7(int l18,void*l6,int l8,int l9,int l10,
defip_aux_scratch_entry_t*l11){uint32 l133;uint32 l302[4] = {0,0,0,0};int l122
= 0;int l131 = SOC_E_NONE;l133 = soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,
l6,VALID0_LWRf);soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,VALIDf,l133
);l133 = soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,l6,MODE0_LWRf);
soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,MODEf,l133);l133 = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,l6,ENTRY_TYPE0_LWRf);
soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,ENTRY_TYPEf,0);l133 = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,l6,GLOBAL_ROUTEf);
soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,GLOBAL_ROUTEf,l133);if(
soc_feature(l18,soc_feature_ipmc_defip)){l133 = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,EXPECTED_L3_IIFf);soc_mem_field32_set(l18,
L3_DEFIP_AUX_SCRATCHm,l11,EXPECTED_L3_IIFf,l133);l133 = soc_mem_field32_get(
l18,L3_DEFIP_PAIR_128m,l6,RPA_IDf);soc_mem_field32_set(l18,
L3_DEFIP_AUX_SCRATCHm,l11,RPA_IDf,l133);l133 = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf);
soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,
IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf,l133);l133 = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf);
soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,
IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf,l133);l133 = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,L3MC_INDEXf);soc_mem_field32_set(l18,
L3_DEFIP_AUX_SCRATCHm,l11,L3MC_INDEXf,l133);}l133 = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,ECMPf);soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,
l11,ECMPf,l133);l133 = soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,l6,
ECMP_PTRf);soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,ECMP_PTRf,l133);
l133 = soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,l6,NEXT_HOP_INDEXf);
soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,NEXT_HOP_INDEXf,l133);l133 = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,l6,PRIf);soc_mem_field32_set(l18,
L3_DEFIP_AUX_SCRATCHm,l11,PRIf,l133);l133 = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,RPEf);soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11
,RPEf,l133);l133 =soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,l6,VRF_ID_0_LWRf
);soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,VRFf,l133);
soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,DB_TYPEf,l9);l133 = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,l6,DST_DISCARDf);
soc_mem_field32_set(l18,L3_DEFIP_AUX_SCRATCHm,l11,DST_DISCARDf,l133);l133 = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,l6,CLASS_IDf);soc_mem_field32_set(
l18,L3_DEFIP_AUX_SCRATCHm,l11,CLASS_IDf,l133);l302[0] = soc_mem_field32_get(
l18,L3_DEFIP_PAIR_128m,l6,IP_ADDR0_LWRf);l302[1] = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,IP_ADDR1_LWRf);l302[2] = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,IP_ADDR0_UPRf);l302[3] = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,IP_ADDR1_UPRf);soc_mem_field_set(l18,
L3_DEFIP_AUX_SCRATCHm,(uint32*)l11,IP_ADDRf,(uint32*)l302);l131 = l129(l18,l6
,&l122);if(SOC_FAILURE(l131)){return l131;}soc_mem_field32_set(l18,
L3_DEFIP_AUX_SCRATCHm,l11,IP_LENGTHf,l122);soc_mem_field32_set(l18,
L3_DEFIP_AUX_SCRATCHm,l11,REPLACE_LENf,l10);return(SOC_E_NONE);}static int l17
(int l18,void*lpm_entry,void*l19,void*l20,soc_mem_t l21,uint32 l22,uint32*
l303){uint32 l133;uint32 l302[4];int l122 = 0;int l131 = SOC_E_NONE;uint32 l23
= 0;sal_memset(l19,0,soc_mem_entry_words(l18,l21)*4);l133 = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf);
soc_mem_field32_set(l18,l21,l19,VALIDf,l133);l133 = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,lpm_entry,ECMPf);soc_mem_field32_set(l18,l21,l19,ECMPf,
l133);l133 = soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf);
soc_mem_field32_set(l18,l21,l19,ECMP_PTRf,l133);l133 = soc_mem_field32_get(
l18,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf);soc_mem_field32_set(l18,l21
,l19,NEXT_HOP_INDEXf,l133);l133 = soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,
lpm_entry,PRIf);soc_mem_field32_set(l18,l21,l19,PRIf,l133);l133 = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,RPEf);
soc_mem_field32_set(l18,l21,l19,RPEf,l133);l133 = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf);soc_mem_field32_set(l18,l21,l19,
DST_DISCARDf,l133);l133 = soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,
lpm_entry,SRC_DISCARDf);soc_mem_field32_set(l18,l21,l19,SRC_DISCARDf,l133);
l133 = soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf);
soc_mem_field32_set(l18,l21,l19,CLASS_IDf,l133);l302[0] = soc_mem_field32_get
(l18,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf);l302[1] = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf);l302[2] = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf);l302[3] = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf);
soc_mem_field_set(l18,l21,(uint32*)l19,KEYf,(uint32*)l302);l131 = l129(l18,
lpm_entry,&l122);if(SOC_FAILURE(l131)){return l131;}if((l122 == 0)&&(l302[0]
== 0)&&(l302[1] == 0)&&(l302[2] == 0)&&(l302[3] == 0)){l23 = 1;}if(l303!= 
NULL){*l303 = l23;}soc_mem_field32_set(l18,l21,l19,LENGTHf,l122);if(l20 == 
NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(l18)){sal_memset(l20,0,
soc_mem_entry_words(l18,l21)*4);sal_memcpy(l20,l19,soc_mem_entry_words(l18,
l21)*4);soc_mem_field32_set(l18,l21,l20,DST_DISCARDf,0);soc_mem_field32_set(
l18,l21,l20,RPEf,0);soc_mem_field32_set(l18,l21,l20,SRC_DISCARDf,l22&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(l18,l21,l20,DEFAULTROUTEf,l23);
}return(SOC_E_NONE);}static int l24(int l18,void*l19,soc_mem_t l21,int l8,int
l25,int l26,int index,void*lpm_entry){uint32 l133;uint32 l302[4];uint32 l151,
l304;sal_memset(lpm_entry,0,soc_mem_entry_words(l18,L3_DEFIP_PAIR_128m)*4);
l133 = soc_mem_field32_get(l18,l21,l19,VALIDf);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf,l133);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,VALID1_LWRf,l133);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,VALID0_UPRf,l133);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,VALID1_UPRf,l133);l133 = soc_mem_field32_get(l18
,l21,l19,ECMPf);soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,ECMPf,
l133);l133 = soc_mem_field32_get(l18,l21,l19,ECMP_PTRf);soc_mem_field32_set(
l18,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf,l133);l133 = soc_mem_field32_get(
l18,l21,l19,NEXT_HOP_INDEXf);soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,
lpm_entry,NEXT_HOP_INDEXf,l133);l133 = soc_mem_field32_get(l18,l21,l19,PRIf);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,PRIf,l133);l133 = 
soc_mem_field32_get(l18,l21,l19,RPEf);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,RPEf,l133);l133 = soc_mem_field32_get(l18,l21,
l19,DST_DISCARDf);soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,
DST_DISCARDf,l133);l133 = soc_mem_field32_get(l18,l21,l19,SRC_DISCARDf);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,SRC_DISCARDf,l133);l133 = 
soc_mem_field32_get(l18,l21,l19,CLASS_IDf);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf,l133);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,ALG_BKT_PTRf,l26);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,ALG_HIT_IDXf,index);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,3);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,3);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,3);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,3);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,1);soc_mem_field32_set(l18
,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);soc_mem_field32_set(
l18,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,1);soc_mem_field32_set
(l18,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,1);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_LWRf,3);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_LWRf,3);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_UPRf,3);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_UPRf,3);
soc_mem_field_get(l18,l21,l19,KEYf,l302);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf,l302[0]);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf,l302[1]);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf,l302[2]);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf,l302[3]);l302[0] = l302[1] = l302[
2] = l302[3] = 0;l133 = soc_mem_field32_get(l18,l21,l19,LENGTHf);l139(l18,
lpm_entry,l133);if(l25 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_HIGHf,1);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_ROUTEf,1);l151 = 0;l304 = 0;}else if(l25
== SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,
GLOBAL_ROUTEf,1);l151 = 0;l304 = 0;}else{l151 = l25;l304 = SOC_VRF_MAX(l18);}
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_LWRf,l151);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_LWRf,l151);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_UPRf,l151);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_UPRf,l151);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_LWRf,l304);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_LWRf,l304);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_UPRf,l304);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_UPRf,l304);
return(SOC_E_NONE);}int soc_alpm_128_warmboot_pivot_add(int l18,int l8,void*
lpm_entry,int l305,int l306){int l131 = SOC_E_NONE;uint32 key[4] = {0,0,0,0};
alpm_pivot_t*l177 = NULL;alpm_bucket_handle_t*l186 = NULL;int l151 = 0,l25 = 
0;uint32 l307;trie_t*l225 = NULL;uint32 prefix[5] = {0};int l23 = 0;l131 = 
l141(l18,lpm_entry,prefix,&l307,&l23);SOC_IF_ERROR_RETURN(l131);
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l18,lpm_entry,&l151,&l25));l305 = 
soc_alpm_physical_idx(l18,L3_DEFIP_PAIR_128m,l305,l8);l186 = sal_alloc(sizeof
(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l186 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l18,
"Unable to allocate memory for PIVOT trie node \n")));return SOC_E_NONE;}
sal_memset(l186,0,sizeof(*l186));l177 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l177 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l18,"Unable to allocate memory for PIVOT trie node \n")));sal_free(l186);
return SOC_E_MEMORY;}sal_memset(l177,0,sizeof(*l177));PIVOT_BUCKET_HANDLE(
l177) = l186;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l177));key[0] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l18,lpm_entry,IP_ADDR0_LWRf);key[1] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l18,lpm_entry,IP_ADDR1_LWRf);key[2] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l18,lpm_entry,IP_ADDR0_UPRf);key[3] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l18,lpm_entry,IP_ADDR1_UPRf);
PIVOT_BUCKET_INDEX(l177) = l306;PIVOT_TCAM_INDEX(l177) = 
SOC_ALPM_128_ADDR_LWR(l305)<<1;if(l151!= SOC_L3_VRF_OVERRIDE){l225 = 
VRF_PIVOT_TRIE_IPV6_128(l18,l25);if(l225 == NULL){trie_init(_MAX_KEY_LEN_144_
,&VRF_PIVOT_TRIE_IPV6_128(l18,l25));l225 = VRF_PIVOT_TRIE_IPV6_128(l18,l25);}
sal_memcpy(l177->key,prefix,sizeof(prefix));l177->len = l307;l131 = 
trie_insert(l225,l177->key,NULL,l177->len,(trie_node_t*)l177);if(SOC_FAILURE(
l131)){sal_free(l186);sal_free(l177);return l131;}}ALPM_TCAM_PIVOT(l18,
SOC_ALPM_128_ADDR_LWR(l305)<<1) = l177;PIVOT_BUCKET_VRF(l177) = l25;
PIVOT_BUCKET_IPV6(l177) = l8;PIVOT_BUCKET_ENT_CNT_UPDATE(l177);if(key[0] == 0
&&key[1] == 0&&key[2] == 0&&key[3] == 0){PIVOT_BUCKET_DEF(l177) = TRUE;}
VRF_PIVOT_REF_INC(l18,l25,l8);return l131;}static int l308(int l18,int l8,
void*lpm_entry,void*l19,soc_mem_t l21,int l305,int l306,int l309){int l310;
int l25;int l131 = SOC_E_NONE;int l23 = 0;uint32 prefix[5] = {0,0,0,0,0};
uint32 l136;void*l311 = NULL;trie_t*l312 = NULL;trie_t*l180 = NULL;
trie_node_t*l182 = NULL;payload_t*l313 = NULL;payload_t*l184 = NULL;
alpm_pivot_t*l147 = NULL;if((NULL == lpm_entry)||(NULL == l19)){return
SOC_E_PARAM;}SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l18,lpm_entry,&l310
,&l25));l21 = L3_DEFIP_ALPM_IPV6_128m;l311 = sal_alloc(sizeof(
defip_pair_128_entry_t),"Temp Defip Pair lpm_entry");if(NULL == l311){return
SOC_E_MEMORY;}SOC_IF_ERROR_RETURN(l24(l18,l19,l21,l8,l310,l306,l305,l311));
l131 = l141(l18,l311,prefix,&l136,&l23);if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l18,"prefix create failed\n")));return l131;}
sal_free(l311);l305 = soc_alpm_physical_idx(l18,L3_DEFIP_PAIR_128m,l305,l8);
l147 = ALPM_TCAM_PIVOT(l18,SOC_ALPM_128_ADDR_LWR(l305)<<1);l312 = 
PIVOT_BUCKET_TRIE(l147);l313 = sal_alloc(sizeof(payload_t),"Payload for Key")
;if(NULL == l313){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l18,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l184 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l184){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l18,
"Unable to allocate memory for pfx trie node\n")));sal_free(l313);return
SOC_E_MEMORY;}sal_memset(l313,0,sizeof(*l313));sal_memset(l184,0,sizeof(*l184
));l313->key[0] = prefix[0];l313->key[1] = prefix[1];l313->key[2] = prefix[2]
;l313->key[3] = prefix[3];l313->key[4] = prefix[4];l313->len = l136;l313->
index = l309;sal_memcpy(l184,l313,sizeof(*l313));l131 = trie_insert(l312,
prefix,NULL,l136,(trie_node_t*)l313);if(SOC_FAILURE(l131)){goto l314;}if(l8){
l180 = VRF_PREFIX_TRIE_IPV6_128(l18,l25);}if(!l23){l131 = trie_insert(l180,
prefix,NULL,l136,(trie_node_t*)l184);if(SOC_FAILURE(l131)){goto l204;}}return
l131;l204:(void)trie_delete(l312,prefix,l136,&l182);l313 = (payload_t*)l182;
l314:sal_free(l313);sal_free(l184);return l131;}static int l315(int l18,int
l31,int l25,int l226,int bkt_ptr){int l131 = SOC_E_NONE;uint32 l136;uint32 key
[5] = {0,0,0,0,0};trie_t*l316 = NULL;payload_t*l216 = NULL;
defip_pair_128_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_pair_128_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l18,"unable to allocate memory for LPM entry\n"))
);return SOC_E_MEMORY;}l27(l18,key,0,l25,l31,lpm_entry,0,1);if(l25 == 
SOC_VRF_MAX(l18)+1){soc_L3_DEFIP_PAIR_128m_field32_set(l18,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(l18,lpm_entry,
DEFAULT_MISSf,1);}soc_L3_DEFIP_PAIR_128m_field32_set(l18,lpm_entry,
ALG_BKT_PTRf,bkt_ptr);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l18,l25) = lpm_entry;
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(l18,l25));l316 = 
VRF_PREFIX_TRIE_IPV6_128(l18,l25);l216 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l216 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l18,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l216,0,sizeof(*l216));l136 = 0;l216->key[0] = key[0]
;l216->key[1] = key[1];l216->len = l136;l131 = trie_insert(l316,key,NULL,l136
,&(l216->node));if(SOC_FAILURE(l131)){sal_free(l216);return l131;}
VRF_TRIE_INIT_DONE(l18,l25,l31,1);return l131;}int
soc_alpm_128_warmboot_prefix_insert(int l18,int l8,void*lpm_entry,void*l19,
int l305,int l306,int l309){int l310;int l25;int l131 = SOC_E_NONE;soc_mem_t
l21;l21 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l18,lpm_entry,&l310,&l25));if(l310 == 
SOC_L3_VRF_OVERRIDE){return(l131);}if(!VRF_TRIE_INIT_COMPLETED(l18,l25,l8)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l18,"VRF %d is not initialized\n"),
l25));l131 = l315(l18,l8,l25,l305,l306);if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l18,"VRF %d/%d trie init \n""failed\n"),l25,l8));
return l131;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l18,
"VRF %d/%d trie init ""completed\n"),l25,l8));}l131 = l308(l18,l8,lpm_entry,
l19,l21,l305,l306,l309);if(l131!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l18,"unit %d : Route Insertion Failed :%s\n"),l18,soc_errmsg(l131)
));return(l131);}VRF_TRIE_ROUTES_INC(l18,l25,l8);return(l131);}int
soc_alpm_128_warmboot_bucket_bitmap_set(int l1,int l31,int l228){int l317 = 1
;if(l31){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l317 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l228,l317);return SOC_E_NONE;}int
soc_alpm_128_warmboot_lpm_reinit_done(int l18){int l226;int l318 = ((3*(128+2
+1))-1);int l298 = soc_mem_index_count(l18,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l18)){l298>>= 1;}if(!soc_alpm_mode_get(l18)){(l40[(l18)][
(((3*(128+2+1))-1))].l35) = -1;for(l226 = ((3*(128+2+1))-1);l226>-1;l226--){
if(-1 == (l40[(l18)][(l226)].l33)){continue;}(l40[(l18)][(l226)].l35) = l318;
(l40[(l18)][(l318)].next) = l226;(l40[(l18)][(l318)].l37) = (l40[(l18)][(l226
)].l33)-(l40[(l18)][(l318)].l34)-1;l318 = l226;}(l40[(l18)][(l318)].next) = -
1;(l40[(l18)][(l318)].l37) = l298-(l40[(l18)][(l318)].l34)-1;}else{(l40[(l18)
][(((3*(128+2+1))-1))].l35) = -1;for(l226 = ((3*(128+2+1))-1);l226>(((3*(128+
2+1))-1)/3);l226--){if(-1 == (l40[(l18)][(l226)].l33)){continue;}(l40[(l18)][
(l226)].l35) = l318;(l40[(l18)][(l318)].next) = l226;(l40[(l18)][(l318)].l37)
= (l40[(l18)][(l226)].l33)-(l40[(l18)][(l318)].l34)-1;l318 = l226;}(l40[(l18)
][(l318)].next) = -1;(l40[(l18)][(l318)].l37) = l298-(l40[(l18)][(l318)].l34)
-1;l318 = (((3*(128+2+1))-1)/3);(l40[(l18)][((((3*(128+2+1))-1)/3))].l35) = -
1;for(l226 = ((((3*(128+2+1))-1)/3)-1);l226>-1;l226--){if(-1 == (l40[(l18)][(
l226)].l33)){continue;}(l40[(l18)][(l226)].l35) = l318;(l40[(l18)][(l318)].
next) = l226;(l40[(l18)][(l318)].l37) = (l40[(l18)][(l226)].l33)-(l40[(l18)][
(l318)].l34)-1;l318 = l226;}(l40[(l18)][(l318)].next) = -1;(l40[(l18)][(l318)
].l37) = (l298>>1)-(l40[(l18)][(l318)].l34)-1;}return(SOC_E_NONE);}int
soc_alpm_128_warmboot_lpm_reinit(int l18,int l8,int l226,void*lpm_entry){int
l15;l258(l18,lpm_entry,l226,0x4000,0);SOC_IF_ERROR_RETURN(l294(l18,lpm_entry,
&l15));if((l40[(l18)][(l15)].l36) == 0){(l40[(l18)][(l15)].l33) = l226;(l40[(
l18)][(l15)].l34) = l226;}else{(l40[(l18)][(l15)].l34) = l226;}(l40[(l18)][(
l15)].l36)++;return(SOC_E_NONE);}int soc_alpm_128_bucket_sanity_check(int l1,
soc_mem_t l236,int index){int l131 = SOC_E_NONE;int l130,l233,l243,l8,l144 = 
-1;int l151 = 0,l25;uint32 l152 = 0;uint32 l13[SOC_MAX_MEM_FIELD_WORDS];int
l319,l320,l321;defip_pair_128_entry_t lpm_entry;int l248,l322;soc_mem_t l21 = 
L3_DEFIP_ALPM_IPV6_128m;int l323 = 8,l324;int l325 = 0;soc_field_t l326 = 
VALID0_LWRf;soc_field_t l327 = GLOBAL_HIGHf;soc_field_t l328 = ALG_BKT_PTRf;
l8 = L3_DEFIP_MODE_128;l248 = soc_mem_index_min(l1,l236);l322 = 
soc_mem_index_max(l1,l236);if((index>= 0)&&(index<l248||index>l322)){return
SOC_E_PARAM;}else if(index>= 0){l248 = index;l322 = index;}SOC_ALPM_LPM_LOCK(
l1);for(l130 = l248;l130<= l322;l130++){l131 = soc_mem_read(l1,l236,
MEM_BLOCK_ANY,l130,(void*)l13);if(SOC_FAILURE(l131)){continue;}if(
soc_mem_field32_get(l1,l236,(void*)l13,l326) == 0||soc_mem_field32_get(l1,
l236,(void*)l13,l327) == 1){continue;}l324 = soc_mem_field32_get(l1,l236,(
void*)l13,l328);l131 = soc_alpm_128_lpm_vrf_get(l1,l13,&l151,&l25);if(
SOC_FAILURE(l131)){continue;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l8)){l323<<= 1;}
l144 = -1;for(l233 = 0;l233<l323;l233++){l131 = _soc_alpm_mem_index(l1,l21,
l324,l233,l152,&l243);if(SOC_FAILURE(l131)){continue;}l131 = soc_mem_read(l1,
l21,MEM_BLOCK_ANY,l243,(void*)l13);if(SOC_FAILURE(l131)){break;}if(!
soc_mem_field32_get(l1,l21,(void*)l13,VALIDf)){continue;}l131 = l24(l1,(void*
)l13,l21,l8,l151,l324,0,&lpm_entry);if(SOC_FAILURE(l131)){continue;}l131 = 
l148(l1,(void*)&lpm_entry,l21,(void*)l13,&l319,&l320,&l321,FALSE);if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tLaunched AUX operation for ""index %d bucket %d sanity check failed\n"),
l130,l324));l325++;continue;}if(l320!= l324){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tEntry at index %d does not belong "
"to bucket %d(from bucket %d)\n"),l321,l324,l320));l325++;}if(l144 == -1){
l144 = l319;continue;}if(l144!= l319){int l158,l159;l158 = 
soc_alpm_logical_idx(l1,l236,SOC_ALPM_128_DEFIP_TO_PAIR(l144>>1),1);l159 = 
soc_alpm_logical_idx(l1,l236,SOC_ALPM_128_DEFIP_TO_PAIR(l319>>1),1);LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tAliased bucket %d(returned "
"bucket %d) found from TCAM1 %d and TCAM2 %d\n"),l324,l320,l158,l159));l325++
;}}}SOC_ALPM_LPM_UNLOCK(l1);if(l325 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check passed\n"),
SOC_MEM_NAME(l1,l236),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l1,l236),index,l325));return l325;}
int soc_alpm_128_pivot_sanity_check(int l1,soc_mem_t l236,int index){int l130
,l329;int l243,l248,l322;int l131 = SOC_E_NONE;int*l330 = NULL;int l319,l320,
l321;uint32 l13[SOC_MAX_MEM_FIELD_WORDS];int l331,l332,l333;soc_mem_t l334;
soc_mem_t l335 = L3_DEFIP_ALPM_IPV6_128m;int l8 = L3_DEFIP_MODE_128;int l325 = 
0;soc_field_t l326 = VALID0_LWRf;soc_field_t l327 = GLOBAL_HIGHf;soc_field_t
l336 = NEXT_HOP_INDEXf;soc_field_t l328 = ALG_BKT_PTRf;l248 = 
soc_mem_index_min(l1,l236);l322 = soc_mem_index_max(l1,l236);if((index>= 0)&&
(index<l248||index>l322)){return SOC_E_PARAM;}else if(index>= 0){l248 = index
;l322 = index;}l330 = sal_alloc(sizeof(int)*MAX_PIVOT_COUNT,
"Bucket index array 128");if(l330 == NULL){l131 = SOC_E_MEMORY;return l131;}
sal_memset(l330,0xff,sizeof(int)*MAX_PIVOT_COUNT);SOC_ALPM_LPM_LOCK(l1);for(
l130 = l248;l130<= l322;l130++){l131 = soc_mem_read(l1,l236,MEM_BLOCK_ANY,
l130,(void*)l13);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l1,l236
),l130,l131));l325 = l131;continue;}if(soc_mem_field32_get(l1,l236,(void*)l13
,l326) == 0||soc_mem_field32_get(l1,l236,(void*)l13,l327) == 1){continue;}
l331 = soc_mem_field32_get(l1,l236,(void*)l13,l328);if(l331!= 0){if(l330[l331
] == -1){l330[l331] = l130;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tDuplicated bucket pointer ""%d detected, in memory %s index1 %d and "
"index2 %d\n"),l331,SOC_MEM_NAME(l1,l236),l330[l331],l130));l325 = l131;
continue;}}l131 = alpm_bucket_is_assigned(l1,l331,l8,&l329);if(l131 == 
SOC_E_PARAM){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tInvalid bucket pointer %d ""detected, in memory %s index %d\n"),l331,
SOC_MEM_NAME(l1,l236),l130));l325 = l131;continue;}if(l131>= 0&&l329 == 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),l331,SOC_MEM_NAME(l1,l236),l130));l325 = 
l131;continue;}l334 = _soc_trident2_alpm_bkt_view_get(l1,l331<<2);if(l334!= 
L3_DEFIP_ALPM_IPV6_128m){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual %s\n"),l331,SOC_MEM_NAME(l1,
L3_DEFIP_ALPM_IPV6_128m),SOC_MEM_NAME(l1,l334)));l325 = l131;continue;}l319 = 
-1;l332 = soc_mem_field32_get(l1,l236,(void*)l13,l336);l131 = l148(l1,l13,
l335,(void*)l13,&l319,&l320,&l321,FALSE);if(l319 == -1){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLaunched AUX operation for PIVOT "
"index %d sanity check failed\n"),l130));l325 = l131;continue;}l243 = 
soc_alpm_logical_idx(l1,l236,SOC_ALPM_128_DEFIP_TO_PAIR(l319>>1),1);l131 = 
soc_mem_read(l1,l236,MEM_BLOCK_ANY,l243,(void*)l13);if(SOC_FAILURE(l131)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tRead memory %s index %d (nexthop) ""return %d\n"),SOC_MEM_NAME(l1,l236),
l243,l131));l325 = l131;continue;}l333 = soc_mem_field32_get(l1,l236,(void*)
l13,l336);if(l332!= l333){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d index2 %d \n"),l130,l243));l325 = l131;continue;}}
SOC_ALPM_LPM_UNLOCK(l1);sal_free(l330);if(l325 == 0){LOG_INFO(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check passed\n"),
SOC_MEM_NAME(l1,l236),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check "
"failed, encountered %d error(s)\n"),SOC_MEM_NAME(l1,l236),index,l325));
return SOC_E_FAIL;}
#endif
#endif /* ALPM_ENABLE */
