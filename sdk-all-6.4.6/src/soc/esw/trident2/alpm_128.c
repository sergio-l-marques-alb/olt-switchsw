/*
 * $Id: alpm_128.c,v 1.13 Broadcom SDK $
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
defip_alpm_ipv6_128_entry_t l190,l191;void*l192,*l193;int*l126 = NULL;int l194
= 0;trie_t*l145 = NULL;int l195;int l196;void*l197 = NULL;void*l198 = NULL;
void*l199;void*l200;void*l201;int l202 = 0;int l203 = 0;l31 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&
l25));if(l25 == SOC_VRF_MAX(l1)+1){l9 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1
,l152);}else{l9 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l152);}l21 = 
L3_DEFIP_ALPM_IPV6_128m;l192 = ((uint32*)&(l190));l193 = ((uint32*)&(l191));
l131 = l141(l1,l6,prefix,&l136,&l23);if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: prefix create failed\n"
)));return l131;}sal_memset(&l11,0,sizeof(defip_aux_scratch_entry_t));
SOC_IF_ERROR_RETURN(l7(l1,l6,l31,l9,0,&l11));if(bucket_index == 0){l131 = 
l142(l1,prefix,l136,l31,l25,&l143,&l144,&bucket_index);SOC_IF_ERROR_RETURN(
l131);if(l143 == 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: "" Could not find bucket to insert prefix\n")));return
SOC_E_NOT_FOUND;}}l131 = _soc_alpm_insert_in_bkt(l1,l21,bucket_index,l152,
l149,l13,&l123,l31);if(l131 == SOC_E_NONE){*l14 = l123;if(SOC_URPF_STATUS_GET
(l1)){l178 = soc_mem_write(l1,l21,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l123),
l154);if(SOC_FAILURE(l178)){return l178;}}}if(l131 == SOC_E_FULL){l179 = 1;}
l147 = ALPM_TCAM_PIVOT(l1,l144);if(l147 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"ins128: pivot index %d bkt %d is not valid \n"),l144,
bucket_index));return SOC_E_PARAM;}trie = PIVOT_BUCKET_TRIE(l147);l175 = l147
;l183 = sal_alloc(sizeof(payload_t),"Payload for 128b Key");if(l183 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Unable to allocate memory for ""trie node \n")));
return SOC_E_MEMORY;}l184 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l184 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l183);return SOC_E_MEMORY;}sal_memset(l183,0,
sizeof(*l183));sal_memset(l184,0,sizeof(*l184));l183->key[0] = prefix[0];l183
->key[1] = prefix[1];l183->key[2] = prefix[2];l183->key[3] = prefix[3];l183->
key[4] = prefix[4];l183->len = l136;l183->index = l123;sal_memcpy(l184,l183,
sizeof(*l183));l184->bkt_ptr = l183;l131 = trie_insert(trie,prefix,NULL,l136,
(trie_node_t*)l183);if(SOC_FAILURE(l131)){goto l204;}l180 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l25);if(!l23){l131 = trie_insert(l180,prefix,NULL
,l136,(trie_node_t*)l184);}else{l146 = NULL;l131 = trie_find_lpm(l180,0,0,&
l146);l185 = (payload_t*)l146;if(SOC_SUCCESS(l131)){l185->bkt_ptr = l183;}}
l176 = l136;if(SOC_FAILURE(l131)){goto l205;}if(l179){l131 = 
alpm_bucket_assign(l1,&bucket_index,l31);if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate"
"new bucket for split\n")));bucket_index = -1;goto l206;}l131 = trie_split(
trie,_MAX_KEY_LEN_144_,FALSE,l177,&l136,&l181,NULL,FALSE,1024);if(SOC_FAILURE
(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Could not split bucket"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l206;}l146 = NULL;l131 = trie_find_lpm(l180,l177,l136,&l146)
;l185 = (payload_t*)l146;if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l177[0],l177[1],l177[
2],l177[3],l177[4],l136));goto l206;}if(l185->bkt_ptr){if(l185->bkt_ptr == 
l183){sal_memcpy(l192,l149,sizeof(defip_alpm_ipv6_128_entry_t));}else{l131 = 
soc_mem_read(l1,l21,MEM_BLOCK_ANY,((payload_t*)l185->bkt_ptr)->index,l192);if
(SOC_FAILURE(l131)){goto l206;}if(SOC_URPF_STATUS_GET(l1)){l131 = 
soc_mem_read(l1,l21,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,((payload_t*)l185->
bkt_ptr)->index),l193);l188 = soc_mem_field32_get(l1,l21,l193,DEFAULTROUTEf);
}}if(SOC_FAILURE(l131)){goto l206;}l131 = l24(l1,l192,l21,l31,l151,
bucket_index,0,&lpm_entry);if(SOC_FAILURE(l131)){goto l206;}l157 = ((
payload_t*)(l185->bkt_ptr))->len;}else{l131 = soc_mem_read(l1,
L3_DEFIP_PAIR_128m,MEM_BLOCK_ANY,soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,
SOC_ALPM_128_DEFIP_TO_PAIR(l144>>1),1),&lpm_entry);}l186 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM 128 Bucket Handle");if(l186 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate "
" memory for PIVOT trie node \n")));l131 = SOC_E_MEMORY;goto l206;}sal_memset
(l186,0,sizeof(*l186));l147 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for new 128b Pivot");if(l147 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate "
"memory for PIVOT trie node \n")));l131 = SOC_E_MEMORY;goto l206;}sal_memset(
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
prefix[4]));goto l206;}l126 = sal_alloc(sizeof(*l126)*l189.count,
"Temp storage for location of prefixes in new 128b bucket");if(l126 == NULL){
l131 = SOC_E_MEMORY;goto l206;}sal_memset(l126,-1,sizeof(*l126)*l189.count);
l195 = sizeof(defip_alpm_raw_entry_t);l196 = l195*ALPM_RAW_BKT_COUNT_DW;l198 = 
sal_alloc(4*l196,"Raw memory buffer");if(l198 == NULL){l131 = SOC_E_MEMORY;
goto l206;}sal_memset(l198,0,4*l196);l199 = (uint8*)l198+l196;l200 = (uint8*)
l198+l196*2;l201 = (uint8*)l198+l196*3;l131 = _soc_alpm_raw_bucket_read(l1,
l21,PIVOT_BUCKET_INDEX(l175),l198,l199);if(SOC_FAILURE(l131)){goto l206;}for(
l130 = 0;l130<l189.count;l130++){payload_t*l122 = l189.prefix[l130];if(l122->
index>0){SOC_ALPM_RAW_INDEX_DECODE(l1,l31,l122->index,l202,l203);l197 = (
uint8*)l198+l202*l195;_soc_alpm_raw_mem_read(l1,l21,l197,l203,l192);
_soc_alpm_raw_mem_write(l1,l21,l197,l203,soc_mem_entry_null(l1,l21));if(
SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE(l1,l31,_soc_alpm_rpf_entry
(l1,l122->index),l202,l203);l197 = (uint8*)l199+l202*l195;
_soc_alpm_raw_mem_read(l1,l21,l197,l203,l193);_soc_alpm_raw_mem_write(l1,l21,
l197,l203,soc_mem_entry_null(l1,l21));}l131 = _soc_alpm_mem_index(l1,l21,
bucket_index,l130,l152,&l123);if(SOC_SUCCESS(l131)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l31,l123,l202,l203);l197 = (uint8*)l200+l202*l195;_soc_alpm_raw_mem_write
(l1,l21,l197,l203,l192);if(SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l31,_soc_alpm_rpf_entry(l1,l123),l202,l203);l197 = (uint8*)l201+l202*l195
;_soc_alpm_raw_mem_write(l1,l21,l197,l203,l193);}}}else{l131 = 
_soc_alpm_mem_index(l1,l21,bucket_index,l130,l152,&l123);if(SOC_SUCCESS(l131)
){l187 = l130;*l14 = l123;_soc_alpm_raw_parity_set(l1,l21,l149);
SOC_ALPM_RAW_INDEX_DECODE(l1,l31,l123,l202,l203);l197 = (uint8*)l200+l202*
l195;_soc_alpm_raw_mem_write(l1,l21,l197,l203,l149);if(SOC_URPF_STATUS_GET(l1
)){_soc_alpm_raw_parity_set(l1,l21,l154);SOC_ALPM_RAW_INDEX_DECODE(l1,l31,
_soc_alpm_rpf_entry(l1,l123),l202,l203);l197 = (uint8*)l201+l202*l195;
_soc_alpm_raw_mem_write(l1,l21,l197,l203,l154);}}}l126[l130] = l123;}l131 = 
_soc_alpm_raw_bucket_write(l1,l21,bucket_index,l152,(void*)l200,(void*)l201,
l189.count);if(SOC_FAILURE(l131)){goto l207;}l131 = l2(l1,&lpm_entry,&l156,
l188);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Unable to add new ""pivot to tcam\n")));if(l131 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l25,l31);}goto l207;}l156 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l156,l31);l131 = l155(l1,l156,
l157);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),l156));goto l208
;}l131 = trie_insert(l145,l147->key,NULL,l147->len,(trie_node_t*)l147);if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));goto l208;}l194 = 1;ALPM_TCAM_PIVOT(
l1,SOC_ALPM_128_ADDR_LWR(l156)<<1) = l147;PIVOT_TCAM_INDEX(l147) = 
SOC_ALPM_128_ADDR_LWR(l156)<<1;VRF_PIVOT_REF_INC(l1,l25,l31);for(l130 = 0;
l130<l189.count;l130++){l189.prefix[l130]->index = l126[l130];}sal_free(l126)
;l126 = NULL;l131 = _soc_alpm_raw_bucket_write(l1,l21,PIVOT_BUCKET_INDEX(l175
),l152,(void*)l198,(void*)l199,-1);if(SOC_FAILURE(l131)){goto l208;}sal_free(
l198);if(l187 == -1){l131 = _soc_alpm_insert_in_bkt(l1,l21,
PIVOT_BUCKET_HANDLE(l175)->bucket_index,l152,l149,l13,&l123,l31);if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Could not insert new ""prefix into trie after split\n"
)));goto l206;}if(SOC_URPF_STATUS_GET(l1)){l131 = soc_mem_write(l1,l21,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l123),l154);}*l14 = l123;l183->index = 
l123;}PIVOT_BUCKET_ENT_CNT_UPDATE(l147);VRF_BUCKET_SPLIT_INC(l1,l25,l31);}
VRF_TRIE_ROUTES_INC(l1,l25,l31);if(l23){sal_free(l184);}SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l11,TRUE,&l143,&l144,&bucket_index));
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE,&l11,FALSE,&l143,&
l144,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l136 = soc_mem_field32_get(
l1,L3_DEFIP_AUX_SCRATCHm,&l11,DB_TYPEf);l136+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l11,DB_TYPEf,l136);if(l23){soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l11,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l11,RPEf,0);}SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,
DELETE_PROPAGATE,&l11,TRUE,&l143,&l144,&bucket_index));SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,INSERT_PROPAGATE,&l11,FALSE,&l143,&l144,&bucket_index));}
PIVOT_BUCKET_ENT_CNT_UPDATE(l175);return l131;l208:l178 = l5(l1,&lpm_entry);
if(SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Failure to free new prefix""at %d\n"),
soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,l156,l31)));}if(l194){l178 = 
trie_delete(l145,l147->key,l147->len,&l182);if(SOC_FAILURE(l178)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: trie delete failure"
"in bkt move rollback\n")));}}VRF_PIVOT_REF_DEC(l1,l25,l31);l207:l174 = l175;
for(l130 = 0;l130<l189.count;l130++){payload_t*prefix = l189.prefix[l130];if(
l126[l130]!= -1){sal_memset(l192,0,sizeof(defip_alpm_ipv6_128_entry_t));l178 = 
soc_mem_write(l1,l21,MEM_BLOCK_ANY,l126[l130],l192);
_soc_trident2_alpm_bkt_view_set(l1,l126[l130],INVALIDm);if(SOC_FAILURE(l178))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
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
l186);sal_free(l147);if(l126!= NULL){sal_free(l126);}sal_free(l198);l182 = 
NULL;l178 = trie_delete(l180,prefix,l176,&l182);l184 = (payload_t*)l182;if(
SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l184){sal_free(
l184);}return l131;l206:if(l126!= NULL){sal_free(l126);}if(l198!= NULL){
sal_free(l198);}l182 = NULL;(void)trie_delete(l180,prefix,l176,&l182);l184 = 
(payload_t*)l182;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l31);}l205:l182 = NULL;(void)trie_delete(trie,prefix,l176,&l182)
;l183 = (payload_t*)l182;l204:if(l183!= NULL){sal_free(l183);}if(l184!= NULL)
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
,ENTRY_TYPE_MASK1_UPRf))-1));return(SOC_E_NONE);}static int l209(int l1,void*
l6,int bucket_index,int l144,int l123,int l210){alpm_pivot_t*l147;
defip_alpm_ipv6_128_entry_t l190,l211,l191;defip_aux_scratch_entry_t l11;
uint32 l13[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l21;void*l192,*l212,*l193 = 
NULL;int l151;int l8;int l131 = SOC_E_NONE,l178 = SOC_E_NONE;uint32 l213[5],
prefix[5];int l31,l25;uint32 l136;int l214;uint32 l9,l152;int l143,l23 = 0;
trie_t*trie,*l180;uint32 l215;defip_pair_128_entry_t lpm_entry,*l216;
payload_t*l183 = NULL,*l217 = NULL,*l185 = NULL;trie_node_t*l182 = NULL,*l146
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
;}else{l131 = l17(l1,l6,l192,0,l21,0,0);}sal_memcpy(&l211,l192,sizeof(l211));
l212 = &l211;if(SOC_FAILURE(l131)){SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_delete: Unable to find "
"prefix for delete\n")));return l131;}l214 = bucket_index;l147 = 
ALPM_TCAM_PIVOT(l1,l144);if(l147 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"del128: pivot index %d bkt %d is not valid \n"),l144,
bucket_index));SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_PARAM;}trie = 
PIVOT_BUCKET_TRIE(l147);l131 = trie_delete(trie,prefix,l136,&l182);l183 = (
payload_t*)l182;if(l131!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l131;}l180 = VRF_PREFIX_TRIE_IPV6_128(l1,l25);
l145 = VRF_PIVOT_TRIE_IPV6_128(l1,l25);if(!l23){l131 = trie_delete(l180,
prefix,l136,&l182);l217 = (payload_t*)l182;if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l218;}if(l210){l146 = NULL;l131 = trie_find_lpm(l180,prefix,
l136,&l146);l185 = (payload_t*)l146;if(SOC_SUCCESS(l131)){payload_t*l219 = (
payload_t*)(l185->bkt_ptr);if(l219!= NULL){l215 = l219->len;}else{l215 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l220;}sal_memcpy(l213,prefix,sizeof(
prefix));l134((l213),(l136),(l31));l131 = l27(l1,l213,l215,l25,l8,&lpm_entry,
0,1);l178 = l148(l1,&lpm_entry,l21,l192,&l144,&bucket_index,&l123,TRUE);(void
)l24(l1,l192,l21,l8,l151,bucket_index,0,&lpm_entry);(void)l27(l1,l213,l136,
l25,l8,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(l131)){l193
= ((uint32*)&(l191));l178 = soc_mem_read(l1,l21,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l123),l193);}}if((l215 == 0)&&SOC_FAILURE(l178)){l216 = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25);sal_memcpy(&lpm_entry,l216,sizeof(
lpm_entry));l131 = l27(l1,l213,l136,l25,l8,&lpm_entry,0,1);}if(SOC_FAILURE(
l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l220;}l216 = &lpm_entry;}}else{l146 = 
NULL;l131 = trie_find_lpm(l180,prefix,l136,&l146);l185 = (payload_t*)l146;if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),l25));goto l218;}l185->bkt_ptr = 0;
l215 = 0;l216 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25);}if(l210){l131 = l7(
l1,l216,l31,l9,l215,&l11);if(SOC_FAILURE(l131)){goto l220;}l131 = 
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l11,TRUE,&l143,&l144,&bucket_index);if(
SOC_FAILURE(l131)){goto l220;}if(SOC_URPF_STATUS_GET(l1)){uint32 l138;if(l193
!= NULL){l138 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l11,DB_TYPEf);
l138++;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l11,DB_TYPEf,l138);l138 = 
soc_mem_field32_get(l1,l21,l193,SRC_DISCARDf);soc_mem_field32_set(l1,l21,&l11
,SRC_DISCARDf,l138);l138 = soc_mem_field32_get(l1,l21,l193,DEFAULTROUTEf);
soc_mem_field32_set(l1,l21,&l11,DEFAULTROUTEf,l138);l131 = _soc_alpm_aux_op(
l1,DELETE_PROPAGATE,&l11,TRUE,&l143,&l144,&bucket_index);}if(SOC_FAILURE(l131
)){goto l220;}}}sal_free(l183);if(!l23){sal_free(l217);}
PIVOT_BUCKET_ENT_CNT_UPDATE(l147);if((l147->len!= 0)&&(trie->trie == NULL)){
uint32 l221[5];sal_memcpy(l221,l147->key,sizeof(l221));l134((l221),(l147->len
),(l8));l27(l1,l221,l147->len,l25,l8,&lpm_entry,0,1);l131 = l5(l1,&lpm_entry)
;if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),l147->key[1],l147->key[2],l147
->key[3],l147->key[4]));}}l131 = _soc_alpm_delete_in_bkt(l1,l21,l214,l152,
l212,l13,&l123,l31);if(!SOC_SUCCESS(l131)){SOC_ALPM_LPM_UNLOCK(l1);l131 = 
SOC_E_FAIL;return l131;}if(SOC_URPF_STATUS_GET(l1)){l131 = 
soc_mem_alpm_delete(l1,l21,SOC_ALPM_RPF_BKT_IDX(l1,l214),MEM_BLOCK_ALL,l152,
l212,l13,&l143);if(!SOC_SUCCESS(l131)){SOC_ALPM_LPM_UNLOCK(l1);l131 = 
SOC_E_FAIL;return l131;}}if((l147->len!= 0)&&(trie->trie == NULL)){l131 = 
alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l147),l31);if(SOC_FAILURE(l131)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(l147)));}l131 = trie_delete(l145,l147->key,l147->len,&l182
);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l147));sal_free(PIVOT_BUCKET_HANDLE(l147));sal_free(l147);
_soc_trident2_alpm_bkt_view_set(l1,l214<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l31)){_soc_trident2_alpm_bkt_view_set(l1,(l214+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l25,l31);if(VRF_TRIE_ROUTES_CNT(l1,
l25,l31) == 0){l131 = l30(l1,l25,l31);}SOC_ALPM_LPM_UNLOCK(l1);return l131;
l220:l178 = trie_insert(l180,prefix,NULL,l136,(trie_node_t*)l217);if(
SOC_FAILURE(l178)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l218:l178 = trie_insert(trie,prefix,NULL,l136,(
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
soc_alpm_128_state_clear(l1));return SOC_E_NONE;}static int l222(int l1,int
l25,int l31){defip_pair_128_entry_t*lpm_entry = NULL,l223;int l224;int index;
int l131 = SOC_E_NONE;uint32 key[5] = {0,0,0,0,0};uint32 l136;
alpm_bucket_handle_t*l186 = NULL;alpm_pivot_t*l147 = NULL;payload_t*l217 = 
NULL;trie_t*l225 = NULL,*l226 = NULL;int l227 = 0;trie_t*l228 = NULL;int l229
= 0;l131 = alpm_bucket_assign(l1,&l224,l31);if(SOC_FAILURE(l131)){return l131
;}l186 = sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l186
== NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: Unable to allocate memory ""for bucket handle \n")));
l131 = SOC_E_MEMORY;goto l230;}sal_memset(l186,0,sizeof(*l186));l147 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l147 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: Unable to allocate memory ""for PIVOT trie node \n")))
;l131 = SOC_E_MEMORY;goto l230;}sal_memset(l147,0,sizeof(*l147));
PIVOT_BUCKET_HANDLE(l147) = l186;l217 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l217 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_128_vrf_add: Unable to allocate memory "
"for pfx trie node \n")));l131 = SOC_E_MEMORY;goto l230;}sal_memset(l217,0,
sizeof(*l217));l131 = trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(l1
,l25));l228 = VRF_PIVOT_TRIE_IPV6_128(l1,l25);if(SOC_FAILURE(l131)){goto l230
;}l131 = trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(l1,l25));l225 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l25);if(SOC_FAILURE(l131)){goto l230;}l131 = 
trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l147));l226 = 
PIVOT_BUCKET_TRIE(l147);if(SOC_FAILURE(l131)){goto l230;}PIVOT_BUCKET_INDEX(
l147) = l224;PIVOT_BUCKET_VRF(l147) = l25;PIVOT_BUCKET_IPV6(l147) = l31;
PIVOT_BUCKET_DEF(l147) = TRUE;l136 = 0;l147->key[0] = l217->key[0] = key[0];
l147->key[1] = l217->key[1] = key[1];l147->key[2] = l217->key[2] = key[2];
l147->key[3] = l217->key[3] = key[3];l147->key[4] = l217->key[4] = key[4];
l147->len = l217->len = l136;l131 = trie_insert(l225,key,NULL,l136,&(l217->
node));if(SOC_FAILURE(l131)){goto l230;}l227 = 1;l131 = trie_insert(l228,key,
NULL,l136,(trie_node_t*)l147);if(SOC_FAILURE(l131)){goto l230;}l229 = 1;(void
)l27(l1,key,0,l25,l31,&l223,0,1);if(l25 == SOC_VRF_MAX(l1)+1){
soc_L3_DEFIP_PAIR_128m_field32_set(l1,&l223,GLOBAL_ROUTEf,1);}else{
soc_L3_DEFIP_PAIR_128m_field32_set(l1,&l223,DEFAULT_MISSf,1);}
soc_L3_DEFIP_PAIR_128m_field32_set(l1,&l223,ALG_BKT_PTRf,l224);lpm_entry = 
sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(lpm_entry == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: unable to allocate memory ""for IPv6-128 LPM entry\n")
));l131 = SOC_E_MEMORY;goto l230;}sal_memcpy(lpm_entry,&l223,sizeof(l223));
l131 = l2(l1,&l223,&index,0);if(SOC_FAILURE(l131)){goto l230;}
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25) = lpm_entry;index = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,l31);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(index)<<1) = l147;PIVOT_TCAM_INDEX(l147) = 
SOC_ALPM_128_ADDR_LWR(index)<<1;VRF_PIVOT_REF_INC(l1,l25,l31);
VRF_TRIE_INIT_DONE(l1,l25,l31,1);return SOC_E_NONE;l230:if(lpm_entry){
sal_free(lpm_entry);}if(l229){trie_node_t*l182 = NULL;(void)trie_delete(l228,
key,l136,&l182);}if(l227){trie_node_t*l182 = NULL;(void)trie_delete(l225,key,
l136,&l182);}if(l226){(void)trie_destroy(l226);PIVOT_BUCKET_TRIE(l147) = NULL
;}if(l225){(void)trie_destroy(l225);VRF_PREFIX_TRIE_IPV6_128(l1,l25) = NULL;}
if(l228){(void)trie_destroy(l228);VRF_PIVOT_TRIE_IPV6_128(l1,l25) = NULL;}if(
l217){sal_free(l217);}if(l147){sal_free(l147);}if(l186){sal_free(l186);}(void
)alpm_bucket_release(l1,l224,l31);return l131;}static int l30(int l1,int l25,
int l31){defip_pair_128_entry_t*lpm_entry;int l224;int l231;int l131 = 
SOC_E_NONE;uint32 key[2] = {0,0},l135[SOC_MAX_MEM_FIELD_WORDS];payload_t*l183
;alpm_pivot_t*l232;trie_node_t*l182;trie_t*l233;trie_t*l228 = NULL;lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25);l224 = 
soc_L3_DEFIP_PAIR_128m_field32_get(l1,lpm_entry,ALG_BKT_PTRf);l131 = 
alpm_bucket_release(l1,l224,l31);_soc_trident2_alpm_bkt_view_set(l1,l224<<2,
INVALIDm);if(SOC_ALPM_V6_SCALE_CHECK(l1,l31)){_soc_trident2_alpm_bkt_view_set
(l1,(l224+1)<<2,INVALIDm);}l131 = l16(l1,lpm_entry,(void*)l135,&l231);if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n"),
l25,l31));l231 = -1;}l231 = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l231,
l31);l232 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l231)<<1);l131 = l5(l1,
lpm_entry);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),l25,l31));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l25) = NULL;l233 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l25);VRF_PREFIX_TRIE_IPV6_128(l1,l25) = NULL;
VRF_TRIE_INIT_DONE(l1,l25,l31,0);l131 = trie_delete(l233,key,0,&l182);l183 = 
(payload_t*)l182;if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),l25,l31));}
sal_free(l183);(void)trie_destroy(l233);l228 = VRF_PIVOT_TRIE_IPV6_128(l1,l25
);VRF_PIVOT_TRIE_IPV6_128(l1,l25) = NULL;l182 = NULL;l131 = trie_delete(l228,
key,0,&l182);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l25,l31));}(void)
trie_destroy(l228);(void)trie_destroy(PIVOT_BUCKET_TRIE(l232));sal_free(
PIVOT_BUCKET_HANDLE(l232));sal_free(l232);return l131;}int soc_alpm_128_insert
(int l1,void*l3,uint32 l22,int l234,int l235){defip_alpm_ipv6_128_entry_t l190
,l191;soc_mem_t l21;void*l192,*l212;int l151,l25;int index;int l8;int l131 = 
SOC_E_NONE;uint32 l23;int l236 = 0;l8 = L3_DEFIP_MODE_128;l21 = 
L3_DEFIP_ALPM_IPV6_128m;l192 = ((uint32*)&(l190));l212 = ((uint32*)&(l191));
SOC_IF_ERROR_RETURN(l17(l1,l3,l192,l212,l21,l22,&l23));SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l3,&l151,&l25));if(soc_feature(l1,
soc_feature_ipmc_defip)){l236 = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,l3,
MULTICAST_ROUTEf);}if((l151 == SOC_L3_VRF_OVERRIDE)||(l236)){l131 = l2(l1,l3,
&index,0);if(SOC_SUCCESS(l131)){VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l8);
VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l8);}else if(l131 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID,l8);}return(l131);}else if(l25 == 0){if(
soc_alpm_mode_get(l1)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}}if(l151!= 
SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l1)){if(VRF_TRIE_ROUTES_CNT(l1,l25,
l8) == 0){if(!l23){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
l151));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l25,l8)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d is not ""initialized\n"),l25));l131 = l222(l1,l25
,l8);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),l25,l8));return l131;
}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d/%d trie init ""completed\n"),l25,l8));}if(l235&
SOC_ALPM_LOOKUP_HIT){l131 = l153(l1,l3,l192,l212,l21,l234);}else{if(l234 == -
1){l234 = 0;}l131 = l173(l1,l3,l21,l192,l212,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l234),l235);}if(l131!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_128_insert: ""Route Insertion Failed :%s\n"),l1,soc_errmsg
(l131)));}return(l131);}int soc_alpm_128_lookup(int l1,void*l6,void*l13,int*
l14,int*l237){defip_alpm_ipv6_128_entry_t l190;soc_mem_t l21;int bucket_index
;int l144;void*l192;int l151,l25;int l8 = 2,l122;int l131 = SOC_E_NONE;int
l236 = 0;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&l25));l131
= l12(l1,l6,l13,l14,&l122,&l8);if(SOC_SUCCESS(l131)){SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l13,&l151,&l25));if(soc_feature(l1,
soc_feature_ipmc_defip)){l236 = soc_L3_DEFIP_PAIR_128m_field32_get(l1,l13,
MULTICAST_ROUTEf);}if((l151 == SOC_L3_VRF_OVERRIDE)||(l236)){return SOC_E_NONE
;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l25,l8)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_lookup:VRF %d is not ""initialized\n"),l25));*l14 = 0
;*l237 = 0;return SOC_E_NOT_FOUND;}l21 = L3_DEFIP_ALPM_IPV6_128m;l192 = ((
uint32*)&(l190));SOC_ALPM_LPM_LOCK(l1);l131 = l148(l1,l6,l21,l192,&l144,&
bucket_index,l14,TRUE);SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l131)){*l237 = 
l144;*l14 = bucket_index<<2;return l131;}l131 = l24(l1,l192,l21,l8,l151,
bucket_index,*l14,l13);*l237 = SOC_ALPM_LOOKUP_HIT|l144;return(l131);}static
int l238(int l1,void*l6,void*l13,int l25,int*l144,int*bucket_index,int*l123,
int l239){int l131 = SOC_E_NONE;int l130,l240,l31,l143 = 0;uint32 l9,l152;
defip_aux_scratch_entry_t l11;int l241,l242;int index;soc_mem_t l21,l243;int
l244,l245,l246;soc_field_t l247[4] = {IP_ADDR0_LWRf,IP_ADDR1_LWRf,
IP_ADDR0_UPRf,IP_ADDR1_UPRf};uint32 l248[SOC_MAX_MEM_FIELD_WORDS] = {0};int
l249 = -1;int l250 = 0;l31 = L3_DEFIP_MODE_128;l243 = L3_DEFIP_PAIR_128m;l241
= soc_mem_field32_get(l1,l243,l6,GLOBAL_ROUTEf);l242 = soc_mem_field32_get(l1
,l243,l6,VRF_ID_0_LWRf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d\n"),l25 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l31,l241,l242));if(l25 == SOC_L3_VRF_GLOBAL){l9 = l239?1:0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l152);soc_mem_field32_set(l1,l243,l6,
GLOBAL_ROUTEf,1);soc_mem_field32_set(l1,l243,l6,VRF_ID_0_LWRf,0);}else{l9 = 
l239?3:2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l152);}sal_memset(&l11,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l7(l1,l6,l31,l9,0,&l11));if(
l25 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l1,l243,l6,GLOBAL_ROUTEf,l241);
soc_mem_field32_set(l1,l243,l6,VRF_ID_0_LWRf,l242);}SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l11,TRUE,&l143,l144,bucket_index));if(l143
== 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Could not find bucket\n")))
;return SOC_E_NOT_FOUND;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in memory %s, index %d, ""bucket_index %d\n"),SOC_MEM_NAME(l1,l243),
soc_alpm_logical_idx(l1,l243,SOC_ALPM_128_DEFIP_TO_PAIR((*l144)>>1),1),*
bucket_index));l21 = L3_DEFIP_ALPM_IPV6_128m;l131 = l129(l1,l6,&l245);if(
SOC_FAILURE(l131)){return l131;}l246 = SOC_ALPM_V6_SCALE_CHECK(l1,l31)?16:8;
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Start searching mem %s bucket %d(count %d) ""for Length %d\n"),SOC_MEM_NAME(
l1,l21),*bucket_index,l246,l245));for(l130 = 0;l130<l246;l130++){uint32 l192[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l251[4] = {0};uint32 l252[4] = {0};
uint32 l253[4] = {0};int l254;l131 = _soc_alpm_mem_index(l1,l21,*bucket_index
,l130,l152,&index);if(l131 == SOC_E_FULL){continue;}SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l21,MEM_BLOCK_ANY,index,(void*)&l192));l254 = 
soc_mem_field32_get(l1,l21,&l192,VALIDf);l244 = soc_mem_field32_get(l1,l21,&
l192,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket %5d Index %6d: Valid %d, Length %d\n"),*bucket_index,index,l254,l244)
);if(!l254||(l244>l245)){continue;}SHR_BITSET_RANGE(l251,128-l244,l244);(void
)soc_mem_field_get(l1,l21,(uint32*)&l192,KEYf,(uint32*)l252);l253[3] = 
soc_mem_field32_get(l1,l243,l6,l247[3]);l253[2] = soc_mem_field32_get(l1,l243
,l6,l247[2]);l253[1] = soc_mem_field32_get(l1,l243,l6,l247[1]);l253[0] = 
soc_mem_field32_get(l1,l243,l6,l247[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tmask %08x %08x %08x %08x \n""\t key %08x %08x %08x %08x \n"
"\thost %08x %08x %08x %08x \n"),l251[3],l251[2],l251[1],l251[0],l252[3],l252
[2],l252[1],l252[0],l253[3],l253[2],l253[1],l253[0]));for(l240 = 3;l240>= 0;
l240--){if((l253[l240]&l251[l240])!= (l252[l240]&l251[l240])){break;}}if(l240
>= 0){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket %d, ""index %d\n"),SOC_MEM_NAME(l1,l21),*
bucket_index,index));if(l249 == -1||l249<l244){l249 = l244;l250 = index;
sal_memcpy(l248,l192,sizeof(l192));}}if(l249!= -1){l131 = l24(l1,&l248,l21,
l31,l25,*bucket_index,l250,l13);if(SOC_SUCCESS(l131)){*l123 = l250;if(
bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit mem %s bucket %d, index %d\n"),
SOC_MEM_NAME(l1,l21),*bucket_index,l250));}}return l131;}*l123 = 
soc_alpm_logical_idx(l1,l243,SOC_ALPM_128_DEFIP_TO_PAIR((*l144)>>1),1);
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l243,MEM_BLOCK_ANY,*l123,(void*)l13));
return SOC_E_NONE;}int soc_alpm_128_find_best_match(int l1,void*l6,void*l13,
int*l14,int l239){int l131 = SOC_E_NONE;int l130,l240;int l255,l256;
defip_pair_128_entry_t l257;uint32 l258,l252,l253;int l244,l245;int l259,l260
;int l151,l25 = 0;int l144,bucket_index;soc_mem_t l243;soc_field_t l261[4] = 
{IP_ADDR_MASK1_UPRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_LWRf}
;soc_field_t l262[4] = {IP_ADDR1_UPRf,IP_ADDR0_UPRf,IP_ADDR1_LWRf,
IP_ADDR0_LWRf};l243 = L3_DEFIP_PAIR_128m;if(!SOC_URPF_STATUS_GET(l1)&&l239){
return SOC_E_PARAM;}l255 = soc_mem_index_min(l1,l243);l256 = 
soc_mem_index_count(l1,l243);if(SOC_URPF_STATUS_GET(l1)){l256>>= 1;}if(
soc_alpm_mode_get(l1)){l256>>= 1;l255+= l256;}if(l239){l255+= 
soc_mem_index_count(l1,l243)/2;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Start LPM searchng from %d, count %d\n"),l255,l256));for(l130 = l255;l130<
l255+l256;l130++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l243,MEM_BLOCK_ANY,l130
,(void*)&l257));if(!soc_mem_field32_get(l1,l243,&l257,VALID0_LWRf)){continue;
}l259 = soc_mem_field32_get(l1,l243,&l257,GLOBAL_HIGHf);l260 = 
soc_mem_field32_get(l1,l243,&l257,GLOBAL_ROUTEf);if(!l260||!l259){continue;}
l131 = l129(l1,l6,&l245);l131 = l129(l1,&l257,&l244);if(SOC_FAILURE(l131)||(
l244>l245)){continue;}for(l240 = 0;l240<4;l240++){l258 = soc_mem_field32_get(
l1,l243,&l257,l261[l240]);l252 = soc_mem_field32_get(l1,l243,&l257,l262[l240]
);l253 = soc_mem_field32_get(l1,l243,l6,l262[l240]);if((l253&l258)!= (l252&
l258)){break;}}if(l240<4){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Hit Global High route in index = %d\n"),l240));sal_memcpy(l13,&l257,
sizeof(l257));*l14 = l130;return SOC_E_NONE;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for Global Low and VRF routes\n")));SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&l25));l131 = l238(l1,l6,l13,l25,&l144,&
bucket_index,l14,l239);if(l131 == SOC_E_NOT_FOUND){l25 = SOC_L3_VRF_GLOBAL;
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Not found in VRF region, try Global ""region\n")));l131 = l238(l1,l6,l13,l25
,&l144,&bucket_index,l14,l239);}if(SOC_SUCCESS(l131)){l144 = 
soc_alpm_logical_idx(l1,l243,SOC_ALPM_128_DEFIP_TO_PAIR(l144>>1),1);
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in %s region in TCAM index %d, ""buckekt_index %d\n"),l25 == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",l144,bucket_index));}else{LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Search miss for given address\n")));}return(
l131);}int soc_alpm_128_delete(int l1,void*l6,int l234,int l235){int l151,l25
;int l8;int l131 = SOC_E_NONE;int l210 = 0;int l236 = 0;l8 = 
L3_DEFIP_MODE_128;if(soc_feature(l1,soc_feature_ipmc_defip)){l236 = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,l6,MULTICAST_ROUTEf);}
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l151,&l25));if((l151 == 
SOC_L3_VRF_OVERRIDE)||(l236)){l131 = l5(l1,l6);if(SOC_SUCCESS(l131)){
VRF_TRIE_ROUTES_DEC(l1,MAX_VRF_ID,l8);VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l8);}
return(l131);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l25,l8)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_128_delete:VRF %d/%d is not "
"initialized\n"),l25,l8));return SOC_E_NONE;}if(l234 == -1){l234 = 0;}l210 = 
!(l235&SOC_ALPM_DELETE_ALL);l131 = l209(l1,l6,SOC_ALPM_BKT_ENTRY_TO_IDX(l234)
,l235&~SOC_ALPM_LOOKUP_HIT,l234,l210);}return(l131);}static void l110(int l1,
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
){int l263;(void)soc_alpm_128_lpm_vrf_get(l1,l13,(int*)&l111[8],&l263);}else{
l111[8] = 0;};}static int l264(l105 l107,l105 l108){int l231;for(l231 = 0;
l231<9;l231++){{if((l107[l231])<(l108[l231])){return-1;}if((l107[l231])>(l108
[l231])){return 1;}};}return(0);}static void l265(int l1,void*l3,uint32 l266,
uint32 l125,int l122){l105 l267;if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l77))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l76))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l75))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l74))){l267[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l57));l267[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l55));l267[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l61));l267[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l59));l267[4] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l58));l267[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l56));l267[6] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l98[(l1)]->l62));l267[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l98[(l1)]->l60));if((!(SOC_IS_HURRICANE(l1)))&&(((l98[(l1)]->l80)!= NULL)))
{int l263;(void)soc_alpm_128_lpm_vrf_get(l1,l3,(int*)&l267[8],&l263);}else{
l267[8] = 0;};l124((l109[(l1)]),l264,l267,l122,l125,l266);}}static void l268(
int l1,void*l6,uint32 l266){l105 l267;int l122 = -1;int l131;uint16 index;
l267[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l57));l267[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l55));l267[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l61));l267[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l59));l267[4] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l58));l267[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l56));l267[6] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l62));l267[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l60));if((!(SOC_IS_HURRICANE(l1)))&&(((l98[(l1)]->l80)!= NULL)))
{int l263;(void)soc_alpm_128_lpm_vrf_get(l1,l6,(int*)&l267[8],&l263);}else{
l267[8] = 0;};index = l266;l131 = l127((l109[(l1)]),l264,l267,l122,index);if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\ndel  index: H %d error %d\n"),index,l131));}}static int l269(int l1,void*
l6,int l122,int*l123){l105 l267;int l131;uint16 index = (0xFFFF);l267[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l57));l267[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l55));l267[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l61));l267[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l59));l267[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l58));l267[5] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l56));l267[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l98[(l1)]->l62));l267[7] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l98[(l1)]->l60));if((!(SOC_IS_HURRICANE(l1)))
&&(((l98[(l1)]->l80)!= NULL))){int l263;(void)soc_alpm_128_lpm_vrf_get(l1,l6,
(int*)&l267[8],&l263);}else{l267[8] = 0;};l131 = l119((l109[(l1)]),l264,l267,
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
,l105 entry,int l122,uint16*l123){int l1 = l120->l18;uint16 l270;uint16 index
;l270 = l112((uint8*)entry,(32*9))%l120->l101;index = l120->l102[l270];;;
while(index!= (0xFFFF)){uint32 l13[SOC_MAX_MEM_FIELD_WORDS];l105 l111;int l271
;l271 = index;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,
l271,l13));l110(l1,l13,index,l111);if((*l121)(entry,l111) == 0){*l123 = index
;;return(SOC_E_NONE);}index = l120->l103[index&(0x3FFF)];;};return(
SOC_E_NOT_FOUND);}static int l124(l104*l120,l106 l121,l105 entry,int l122,
uint16 l125,uint16 l126){int l1 = l120->l18;uint16 l270;uint16 index;uint16
l272;l270 = l112((uint8*)entry,(32*9))%l120->l101;index = l120->l102[l270];;;
;l272 = (0xFFFF);if(l125!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l13[
SOC_MAX_MEM_FIELD_WORDS];l105 l111;int l271;l271 = index;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l271,l13));l110(l1,l13,index,l111);
if((*l121)(entry,l111) == 0){if(l126!= index){;if(l272 == (0xFFFF)){l120->
l102[l270] = l126;l120->l103[l126&(0x3FFF)] = l120->l103[index&(0x3FFF)];l120
->l103[index&(0x3FFF)] = (0xFFFF);}else{l120->l103[l272&(0x3FFF)] = l126;l120
->l103[l126&(0x3FFF)] = l120->l103[index&(0x3FFF)];l120->l103[index&(0x3FFF)]
= (0xFFFF);}};return(SOC_E_NONE);}l272 = index;index = l120->l103[index&(
0x3FFF)];;}}l120->l103[l126&(0x3FFF)] = l120->l102[l270];l120->l102[l270] = 
l126;return(SOC_E_NONE);}static int l127(l104*l120,l106 l121,l105 entry,int
l122,uint16 l128){uint16 l270;uint16 index;uint16 l272;l270 = l112((uint8*)
entry,(32*9))%l120->l101;index = l120->l102[l270];;;l272 = (0xFFFF);while(
index!= (0xFFFF)){if(l128 == index){;if(l272 == (0xFFFF)){l120->l102[l270] = 
l120->l103[l128&(0x3FFF)];l120->l103[l128&(0x3FFF)] = (0xFFFF);}else{l120->
l103[l272&(0x3FFF)] = l120->l103[l128&(0x3FFF)];l120->l103[l128&(0x3FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l272 = index;index = l120->l103[index&(0x3FFF)];
;}return(SOC_E_NOT_FOUND);}static int l273(int l1,void*l13){return(SOC_E_NONE
);}void soc_alpm_128_lpm_state_dump(int l1){int l130;int l274;l274 = ((3*(128
+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){
return;}for(l130 = l274;l130>= 0;l130--){if((l130!= ((3*(128+2+1))-1))&&((l40
[(l1)][(l130)].l33) == -1)){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"PFX = %d P = %d N = %d START = %d ""END = %d VENT = %d FENT = %d\n"),
l130,(l40[(l1)][(l130)].l35),(l40[(l1)][(l130)].next),(l40[(l1)][(l130)].l33)
,(l40[(l1)][(l130)].l34),(l40[(l1)][(l130)].l36),(l40[(l1)][(l130)].l37)));}
COMPILER_REFERENCE(l273);}static int l275(int l1,int l276,int index,uint32*
l13,int l4){int l277;uint32 l278,l279,l280;uint32 l281;int l282;uint32 l283[
SOC_MAX_MEM_FIELD_WORDS] = {0};if(!SOC_URPF_STATUS_GET(l1)){return(SOC_E_NONE
);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l277 = (soc_mem_index_count(
l1,L3_DEFIP_PAIR_128m)>>1);}else if(SOC_IS_APOLLO(l1)){l277 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l277 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}if(l4>= 2){
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l276+l277,l283))
;l4 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)
),(l283),(l98[(l1)]->l73));}if(((l98[(l1)]->l43)!= NULL)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l43),(0));}l278 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l61));l281 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l62));l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l59));l280 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l60));l282 = ((!l278)&&(!l281)&&(!l279)&&(!l280))?1:0;l278 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l76));l281 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l74));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l75));l280 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l13),(l98[(l1)]->l75));if(l278&&l281&&l279&&l280){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l13
),(l98[(l1)]->l73),(l4|l282));}return l170(l1,MEM_BLOCK_ANY,index+l277,index,
l13);}static int l284(int l1,int l285,int l286){uint32 l13[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l285,l13));l265(l1,l13,l286,0x4000,0);SOC_IF_ERROR_RETURN(l170(
l1,MEM_BLOCK_ANY,l286,l285,l13));SOC_IF_ERROR_RETURN(l275(l1,l285,l286,l13,2)
);do{int l287,l288;l287 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l285
),1);l288 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l286),1);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l288))<<1) = ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l287))<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR
((l288))<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l288
))<<1)) = SOC_ALPM_128_ADDR_LWR((l288))<<1;}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l287))<<1) = NULL;}while(0);return(SOC_E_NONE);}static
int l289(int l1,int l122,int l8){int l285;int l286;l286 = (l40[(l1)][(l122)].
l34)+1;l285 = (l40[(l1)][(l122)].l33);if(l285!= l286){SOC_IF_ERROR_RETURN(
l284(l1,l285,l286));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l8);}(l40[(l1)][(l122)]
.l33)+= 1;(l40[(l1)][(l122)].l34)+= 1;return(SOC_E_NONE);}static int l290(int
l1,int l122,int l8){int l285;int l286;l286 = (l40[(l1)][(l122)].l33)-1;if((
l40[(l1)][(l122)].l36) == 0){(l40[(l1)][(l122)].l33) = l286;(l40[(l1)][(l122)
].l34) = l286-1;return(SOC_E_NONE);}l285 = (l40[(l1)][(l122)].l34);
SOC_IF_ERROR_RETURN(l284(l1,l285,l286));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l8)
;(l40[(l1)][(l122)].l33)-= 1;(l40[(l1)][(l122)].l34)-= 1;return(SOC_E_NONE);}
static int l291(int l1,int l122,int l8,void*l13,int*l292){int l293;int l294;
int l295;int l296;if((l40[(l1)][(l122)].l36) == 0){l296 = ((3*(128+2+1))-1);
if(soc_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL){if(l122<= (((3*(128+2+1))
/3)-1)){l296 = (((3*(128+2+1))/3)-1);}}while((l40[(l1)][(l296)].next)>l122){
l296 = (l40[(l1)][(l296)].next);}l294 = (l40[(l1)][(l296)].next);if(l294!= -1
){(l40[(l1)][(l294)].l35) = l122;}(l40[(l1)][(l122)].next) = (l40[(l1)][(l296
)].next);(l40[(l1)][(l122)].l35) = l296;(l40[(l1)][(l296)].next) = l122;(l40[
(l1)][(l122)].l37) = ((l40[(l1)][(l296)].l37)+1)/2;(l40[(l1)][(l296)].l37)-= 
(l40[(l1)][(l122)].l37);(l40[(l1)][(l122)].l33) = (l40[(l1)][(l296)].l34)+(
l40[(l1)][(l296)].l37)+1;(l40[(l1)][(l122)].l34) = (l40[(l1)][(l122)].l33)-1;
(l40[(l1)][(l122)].l36) = 0;}l295 = l122;while((l40[(l1)][(l295)].l37) == 0){
l295 = (l40[(l1)][(l295)].next);if(l295 == -1){l295 = l122;break;}}while((l40
[(l1)][(l295)].l37) == 0){l295 = (l40[(l1)][(l295)].l35);if(l295 == -1){if((
l40[(l1)][(l122)].l36) == 0){l293 = (l40[(l1)][(l122)].l35);l294 = (l40[(l1)]
[(l122)].next);if(-1!= l293){(l40[(l1)][(l293)].next) = l294;}if(-1!= l294){(
l40[(l1)][(l294)].l35) = l293;}}return(SOC_E_FULL);}}while(l295>l122){l294 = 
(l40[(l1)][(l295)].next);SOC_IF_ERROR_RETURN(l290(l1,l294,l8));(l40[(l1)][(
l295)].l37)-= 1;(l40[(l1)][(l294)].l37)+= 1;l295 = l294;}while(l295<l122){
SOC_IF_ERROR_RETURN(l289(l1,l295,l8));(l40[(l1)][(l295)].l37)-= 1;l293 = (l40
[(l1)][(l295)].l35);(l40[(l1)][(l293)].l37)+= 1;l295 = l293;}(l40[(l1)][(l122
)].l36)+= 1;(l40[(l1)][(l122)].l37)-= 1;(l40[(l1)][(l122)].l34)+= 1;*l292 = (
l40[(l1)][(l122)].l34);sal_memcpy(l13,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);return(
SOC_E_NONE);}static int l297(int l1,int l122,int l8,void*l13,int l298){int
l293;int l294;int l285;int l286;uint32 l299[SOC_MAX_MEM_FIELD_WORDS];int l131
;int l138;l285 = (l40[(l1)][(l122)].l34);l286 = l298;(l40[(l1)][(l122)].l36)
-= 1;(l40[(l1)][(l122)].l37)+= 1;(l40[(l1)][(l122)].l34)-= 1;if(l286!= l285){
if((l131 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l285,l299))<0){return l131
;}l265(l1,l299,l286,0x4000,0);if((l131 = l170(l1,MEM_BLOCK_ANY,l286,l285,l299
))<0){return l131;}if((l131 = l275(l1,l285,l286,l299,2))<0){return l131;}}
l138 = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l286,1);l298 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l285,1);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l138)<<1) = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
l298)<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l138)<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l138)<<1)) = 
SOC_ALPM_128_ADDR_LWR(l138)<<1;}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l298
)<<1) = NULL;sal_memcpy(l299,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),
soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);l286 = l285;l265(l1,l299,l286,
0x4000,0);if((l131 = l170(l1,MEM_BLOCK_ANY,l286,l285,l299))<0){return l131;}
if((l131 = l275(l1,l285,l285,l299,0))<0){return l131;}if((l40[(l1)][(l122)].
l36) == 0){l293 = (l40[(l1)][(l122)].l35);assert(l293!= -1);l294 = (l40[(l1)]
[(l122)].next);(l40[(l1)][(l293)].next) = l294;(l40[(l1)][(l293)].l37)+= (l40
[(l1)][(l122)].l37);(l40[(l1)][(l122)].l37) = 0;if(l294!= -1){(l40[(l1)][(
l294)].l35) = l293;}(l40[(l1)][(l122)].next) = -1;(l40[(l1)][(l122)].l35) = -
1;(l40[(l1)][(l122)].l33) = -1;(l40[(l1)][(l122)].l34) = -1;}return(l131);}
int soc_alpm_128_lpm_vrf_get(int l18,void*lpm_entry,int*l25,int*l300){int l151
;if(((l98[(l18)]->l84)!= NULL)){l151 = soc_L3_DEFIP_PAIR_128m_field32_get(l18
,lpm_entry,VRF_ID_0_LWRf);*l300 = l151;if(soc_L3_DEFIP_PAIR_128m_field32_get(
l18,lpm_entry,VRF_ID_MASK0_LWRf)){*l25 = l151;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l18,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l98[(l18)]->l86))){*l25 = SOC_L3_VRF_GLOBAL;*l300 = SOC_VRF_MAX(
l18)+1;}else{*l25 = SOC_L3_VRF_OVERRIDE;}}else{*l25 = SOC_L3_VRF_DEFAULT;}
return(SOC_E_NONE);}static int l301(int l1,void*entry,int*l15){int l122=0;int
l131;int l151;int l302;l131 = l129(l1,entry,&l122);if(l131<0){return l131;}
l122+= 0;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,entry,&l151,&l131));
l302 = soc_alpm_mode_get(l1);switch(l151){case SOC_L3_VRF_GLOBAL:if(l302 == 
SOC_ALPM_MODE_PARALLEL){*l15 = l122+((3*(128+2+1))/3);}else{*l15 = l122;}
break;case SOC_L3_VRF_OVERRIDE:*l15 = l122+2*((3*(128+2+1))/3);break;default:
if(l302 == SOC_ALPM_MODE_PARALLEL){*l15 = l122;}else{*l15 = l122+((3*(128+2+1
))/3);}break;}return(SOC_E_NONE);}static int l12(int l1,void*l6,void*l13,int*
l14,int*l15,int*l8){int l131;int l123;int l122 = 0;*l8 = L3_DEFIP_MODE_128;
l301(l1,l6,&l122);*l15 = l122;if(l269(l1,l6,l122,&l123) == SOC_E_NONE){*l14 = 
l123;if((l131 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(*l8)?*l14:(*l14>>1)
,l13))<0){return l131;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}int
soc_alpm_128_ipmc_war(int l18,int l303){int index = -1;defip_pair_128_entry_t
lpm_entry;soc_mem_t l21 = L3_DEFIP_PAIR_128m;int l304 = 63;if(!
soc_property_get(l18,spn_L3_ALPM_ENABLE,0)){return SOC_E_NONE;}if(
soc_mem_index_count(l18,L3_DEFIP_PAIR_128m)<= 0){return SOC_E_NONE;}if(!
SOC_IS_TRIDENT2PLUS(l18)){return SOC_E_NONE;}if(SOC_WARM_BOOT(l18)){return
SOC_E_NONE;}l304 = soc_mem_field_length(l18,ING_ACTIVE_L3_IIF_PROFILEm,
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
l18,l21,&lpm_entry,RPA_IDf,l304-1);soc_mem_field32_set(l18,l21,&lpm_entry,
EXPECTED_L3_IIFf,16383);soc_mem_field32_set(l18,l21,&lpm_entry,IP_ADDR0_LWRf,
0);soc_mem_field32_set(l18,l21,&lpm_entry,IP_ADDR1_LWRf,0);
soc_mem_field32_set(l18,l21,&lpm_entry,IP_ADDR0_UPRf,0);soc_mem_field32_set(
l18,l21,&lpm_entry,IP_ADDR1_UPRf,0xff000000);soc_mem_field32_set(l18,l21,&
lpm_entry,IP_ADDR_MASK0_LWRf,0);soc_mem_field32_set(l18,l21,&lpm_entry,
IP_ADDR_MASK1_UPRf,0);soc_mem_field32_set(l18,l21,&lpm_entry,
IP_ADDR_MASK0_LWRf,0);soc_mem_field32_set(l18,l21,&lpm_entry,
IP_ADDR_MASK1_UPRf,0xff000000);if(l303){SOC_IF_ERROR_RETURN(l2(l18,&lpm_entry
,&index,0));}else{SOC_IF_ERROR_RETURN(l5(l18,&lpm_entry));}return SOC_E_NONE;
}int soc_alpm_128_lpm_init(int l1){int l274;int l130;int l305;int l306;if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}l274 = (3*(128+2
+1));l306 = sizeof(l38)*(l274);if((l40[(l1)]!= NULL)){SOC_IF_ERROR_RETURN(
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
ENTRY_TYPE_MASK1_UPRf);(l40[(l1)]) = sal_alloc(l306,"LPM 128 prefix info");if
(NULL == (l40[(l1)])){sal_free(l98[l1]);l98[l1] = NULL;return(SOC_E_MEMORY);}
SOC_ALPM_LPM_LOCK(l1);sal_memset((l40[(l1)]),0,l306);for(l130 = 0;l130<l274;
l130++){(l40[(l1)][(l130)].l33) = -1;(l40[(l1)][(l130)].l34) = -1;(l40[(l1)][
(l130)].l35) = -1;(l40[(l1)][(l130)].next) = -1;(l40[(l1)][(l130)].l36) = 0;(
l40[(l1)][(l130)].l37) = 0;}l305 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)
;if(SOC_URPF_STATUS_GET(l1)){l305>>= 1;}if(soc_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL){(l40[(l1)][(((3*(128+2+1))-1))].l34) = (l305>>1)-1;(
l40[(l1)][(((((3*(128+2+1))/3)-1)))].l37) = l305>>1;(l40[(l1)][((((3*(128+2+1
))-1)))].l37) = (l305-(l40[(l1)][(((((3*(128+2+1))/3)-1)))].l37));}else{(l40[
(l1)][((((3*(128+2+1))-1)))].l37) = l305;}if((l109[(l1)])!= NULL){if(l117((
l109[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(l109[(l1)]) = 
NULL;}if(l115(l1,l305*2,l305,&(l109[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_MEMORY;}(void)soc_alpm_128_ipmc_war(l1,TRUE);SOC_ALPM_LPM_UNLOCK(l1);
return(SOC_E_NONE);}int soc_alpm_128_lpm_deinit(int l1){if(!soc_feature(l1,
soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);(void)
soc_alpm_128_ipmc_war(l1,FALSE);if((l109[(l1)])!= NULL){l117((l109[(l1)]));(
l109[(l1)]) = NULL;}if((l40[(l1)]!= NULL)){sal_free(l98[l1]);l98[l1] = NULL;
sal_free((l40[(l1)]));(l40[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l1);return(
SOC_E_NONE);}static int l2(int l1,void*l3,int*l307,int l4){int l122;int index
;int l8;uint32 l13[SOC_MAX_MEM_FIELD_WORDS];int l131 = SOC_E_NONE;int l308 = 
0;if(l4>= 2){return SOC_E_INTERNAL;}sal_memcpy(l13,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);
SOC_ALPM_LPM_LOCK(l1);l131 = l12(l1,l3,l13,&index,&l122,&l8);if(l131 == 
SOC_E_NOT_FOUND){l131 = l291(l1,l122,l8,l13,&index);if(l131<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l131);}}else{l308 = 1;}*l307 = index;if(l131 == 
SOC_E_NONE){soc_alpm_128_lpm_state_dump(l1);LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\nsoc_alpm_128_lpm_insert: %d %d\n"),index,l122));if(!l308){
l265(l1,l3,index,0x4000,0);}l131 = l170(l1,MEM_BLOCK_ANY,index,index,l3);if(
l131>= 0){l131 = l275(l1,index,index,l3,l4);}}SOC_ALPM_LPM_UNLOCK(l1);return(
l131);}static int l5(int l1,void*l6){int l122;int index;int l8;uint32 l13[
SOC_MAX_MEM_FIELD_WORDS];int l131 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l131 = 
l12(l1,l6,l13,&index,&l122,&l8);if(l131 == SOC_E_NONE){LOG_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_alpm_lpm_delete: %d %d\n"),index,l122))
;l268(l1,l6,index);l131 = l297(l1,l122,l8,l13,index);}
soc_alpm_128_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l131);}static
int l16(int l1,void*l6,void*l13,int*l14){int l122;int l131;int l8;
SOC_ALPM_LPM_LOCK(l1);l131 = l12(l1,l6,l13,l14,&l122,&l8);SOC_ALPM_LPM_UNLOCK
(l1);return(l131);}static int l7(int l18,void*l6,int l8,int l9,int l10,
defip_aux_scratch_entry_t*l11){uint32 l133;uint32 l309[4] = {0,0,0,0};int l122
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
l18,L3_DEFIP_AUX_SCRATCHm,l11,CLASS_IDf,l133);l309[0] = soc_mem_field32_get(
l18,L3_DEFIP_PAIR_128m,l6,IP_ADDR0_LWRf);l309[1] = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,IP_ADDR1_LWRf);l309[2] = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,IP_ADDR0_UPRf);l309[3] = soc_mem_field32_get(l18,
L3_DEFIP_PAIR_128m,l6,IP_ADDR1_UPRf);soc_mem_field_set(l18,
L3_DEFIP_AUX_SCRATCHm,(uint32*)l11,IP_ADDRf,(uint32*)l309);l131 = l129(l18,l6
,&l122);if(SOC_FAILURE(l131)){return l131;}soc_mem_field32_set(l18,
L3_DEFIP_AUX_SCRATCHm,l11,IP_LENGTHf,l122);soc_mem_field32_set(l18,
L3_DEFIP_AUX_SCRATCHm,l11,REPLACE_LENf,l10);return(SOC_E_NONE);}static int l17
(int l18,void*lpm_entry,void*l19,void*l20,soc_mem_t l21,uint32 l22,uint32*
l310){uint32 l133;uint32 l309[4];int l122 = 0;int l131 = SOC_E_NONE;uint32 l23
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
soc_mem_field32_set(l18,l21,l19,CLASS_IDf,l133);l309[0] = soc_mem_field32_get
(l18,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf);l309[1] = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf);l309[2] = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf);l309[3] = 
soc_mem_field32_get(l18,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf);
soc_mem_field_set(l18,l21,(uint32*)l19,KEYf,(uint32*)l309);l131 = l129(l18,
lpm_entry,&l122);if(SOC_FAILURE(l131)){return l131;}if((l122 == 0)&&(l309[0]
== 0)&&(l309[1] == 0)&&(l309[2] == 0)&&(l309[3] == 0)){l23 = 1;}if(l310!= 
NULL){*l310 = l23;}soc_mem_field32_set(l18,l21,l19,LENGTHf,l122);if(l20 == 
NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(l18)){sal_memset(l20,0,
soc_mem_entry_words(l18,l21)*4);sal_memcpy(l20,l19,soc_mem_entry_words(l18,
l21)*4);soc_mem_field32_set(l18,l21,l20,DST_DISCARDf,0);soc_mem_field32_set(
l18,l21,l20,RPEf,0);soc_mem_field32_set(l18,l21,l20,SRC_DISCARDf,l22&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(l18,l21,l20,DEFAULTROUTEf,l23);
}return(SOC_E_NONE);}static int l24(int l18,void*l19,soc_mem_t l21,int l8,int
l25,int l26,int index,void*lpm_entry){uint32 l133;uint32 l309[4];uint32 l151,
l311;sal_memset(lpm_entry,0,soc_mem_entry_words(l18,L3_DEFIP_PAIR_128m)*4);
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
soc_mem_field_get(l18,l21,l19,KEYf,l309);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf,l309[0]);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf,l309[1]);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf,l309[2]);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf,l309[3]);l309[0] = l309[1] = l309[
2] = l309[3] = 0;l133 = soc_mem_field32_get(l18,l21,l19,LENGTHf);l139(l18,
lpm_entry,l133);if(l25 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_HIGHf,1);soc_mem_field32_set(l18,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_ROUTEf,1);l151 = 0;l311 = 0;}else if(l25
== SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,
GLOBAL_ROUTEf,1);l151 = 0;l311 = 0;}else{l151 = l25;l311 = SOC_VRF_MAX(l18);}
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_LWRf,l151);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_LWRf,l151);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_UPRf,l151);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_UPRf,l151);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_LWRf,l311);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_LWRf,l311);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_UPRf,l311);
soc_mem_field32_set(l18,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_UPRf,l311);
return(SOC_E_NONE);}int soc_alpm_128_warmboot_pivot_add(int l18,int l8,void*
lpm_entry,int l312,int l313){int l131 = SOC_E_NONE;uint32 key[4] = {0,0,0,0};
alpm_pivot_t*l177 = NULL;alpm_bucket_handle_t*l186 = NULL;int l151 = 0,l25 = 
0;uint32 l314;trie_t*l228 = NULL;uint32 prefix[5] = {0};int l23 = 0;l131 = 
l141(l18,lpm_entry,prefix,&l314,&l23);SOC_IF_ERROR_RETURN(l131);
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l18,lpm_entry,&l151,&l25));l312 = 
soc_alpm_physical_idx(l18,L3_DEFIP_PAIR_128m,l312,l8);l186 = sal_alloc(sizeof
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
PIVOT_BUCKET_INDEX(l177) = l313;PIVOT_TCAM_INDEX(l177) = 
SOC_ALPM_128_ADDR_LWR(l312)<<1;if(l151!= SOC_L3_VRF_OVERRIDE){l228 = 
VRF_PIVOT_TRIE_IPV6_128(l18,l25);if(l228 == NULL){trie_init(_MAX_KEY_LEN_144_
,&VRF_PIVOT_TRIE_IPV6_128(l18,l25));l228 = VRF_PIVOT_TRIE_IPV6_128(l18,l25);}
sal_memcpy(l177->key,prefix,sizeof(prefix));l177->len = l314;l131 = 
trie_insert(l228,l177->key,NULL,l177->len,(trie_node_t*)l177);if(SOC_FAILURE(
l131)){sal_free(l186);sal_free(l177);return l131;}}ALPM_TCAM_PIVOT(l18,
SOC_ALPM_128_ADDR_LWR(l312)<<1) = l177;PIVOT_BUCKET_VRF(l177) = l25;
PIVOT_BUCKET_IPV6(l177) = l8;PIVOT_BUCKET_ENT_CNT_UPDATE(l177);if(key[0] == 0
&&key[1] == 0&&key[2] == 0&&key[3] == 0){PIVOT_BUCKET_DEF(l177) = TRUE;}
VRF_PIVOT_REF_INC(l18,l25,l8);return l131;}static int l315(int l18,int l8,
void*lpm_entry,void*l19,soc_mem_t l21,int l312,int l313,int l316){int l317;
int l25;int l131 = SOC_E_NONE;int l23 = 0;uint32 prefix[5] = {0,0,0,0,0};
uint32 l136;void*l318 = NULL;trie_t*l319 = NULL;trie_t*l180 = NULL;
trie_node_t*l182 = NULL;payload_t*l320 = NULL;payload_t*l184 = NULL;
alpm_pivot_t*l147 = NULL;if((NULL == lpm_entry)||(NULL == l19)){return
SOC_E_PARAM;}SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l18,lpm_entry,&l317
,&l25));l21 = L3_DEFIP_ALPM_IPV6_128m;l318 = sal_alloc(sizeof(
defip_pair_128_entry_t),"Temp Defip Pair lpm_entry");if(NULL == l318){return
SOC_E_MEMORY;}SOC_IF_ERROR_RETURN(l24(l18,l19,l21,l8,l317,l313,l312,l318));
l131 = l141(l18,l318,prefix,&l136,&l23);if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l18,"prefix create failed\n")));return l131;}
sal_free(l318);l312 = soc_alpm_physical_idx(l18,L3_DEFIP_PAIR_128m,l312,l8);
l147 = ALPM_TCAM_PIVOT(l18,SOC_ALPM_128_ADDR_LWR(l312)<<1);l319 = 
PIVOT_BUCKET_TRIE(l147);l320 = sal_alloc(sizeof(payload_t),"Payload for Key")
;if(NULL == l320){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l18,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l184 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l184){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l18,
"Unable to allocate memory for pfx trie node\n")));sal_free(l320);return
SOC_E_MEMORY;}sal_memset(l320,0,sizeof(*l320));sal_memset(l184,0,sizeof(*l184
));l320->key[0] = prefix[0];l320->key[1] = prefix[1];l320->key[2] = prefix[2]
;l320->key[3] = prefix[3];l320->key[4] = prefix[4];l320->len = l136;l320->
index = l316;sal_memcpy(l184,l320,sizeof(*l320));l131 = trie_insert(l319,
prefix,NULL,l136,(trie_node_t*)l320);if(SOC_FAILURE(l131)){goto l321;}if(l8){
l180 = VRF_PREFIX_TRIE_IPV6_128(l18,l25);}if(!l23){l131 = trie_insert(l180,
prefix,NULL,l136,(trie_node_t*)l184);if(SOC_FAILURE(l131)){goto l205;}}return
l131;l205:(void)trie_delete(l319,prefix,l136,&l182);l320 = (payload_t*)l182;
l321:sal_free(l320);sal_free(l184);return l131;}static int l322(int l18,int
l31,int l25,int l231,int bkt_ptr){int l131 = SOC_E_NONE;uint32 l136;uint32 key
[5] = {0,0,0,0,0};trie_t*l323 = NULL;payload_t*l217 = NULL;
defip_pair_128_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_pair_128_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l18,"unable to allocate memory for LPM entry\n"))
);return SOC_E_MEMORY;}l27(l18,key,0,l25,l31,lpm_entry,0,1);if(l25 == 
SOC_VRF_MAX(l18)+1){soc_L3_DEFIP_PAIR_128m_field32_set(l18,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(l18,lpm_entry,
DEFAULT_MISSf,1);}soc_L3_DEFIP_PAIR_128m_field32_set(l18,lpm_entry,
ALG_BKT_PTRf,bkt_ptr);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l18,l25) = lpm_entry;
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(l18,l25));l323 = 
VRF_PREFIX_TRIE_IPV6_128(l18,l25);l217 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l217 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l18,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l217,0,sizeof(*l217));l136 = 0;l217->key[0] = key[0]
;l217->key[1] = key[1];l217->len = l136;l131 = trie_insert(l323,key,NULL,l136
,&(l217->node));if(SOC_FAILURE(l131)){sal_free(l217);return l131;}
VRF_TRIE_INIT_DONE(l18,l25,l31,1);return l131;}int
soc_alpm_128_warmboot_prefix_insert(int l18,int l8,void*lpm_entry,void*l19,
int l312,int l313,int l316){int l317;int l25;int l131 = SOC_E_NONE;soc_mem_t
l21;l21 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l18,lpm_entry,&l317,&l25));if(l317 == 
SOC_L3_VRF_OVERRIDE){return(l131);}if(!VRF_TRIE_INIT_COMPLETED(l18,l25,l8)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l18,"VRF %d is not initialized\n"),
l25));l131 = l322(l18,l8,l25,l312,l313);if(SOC_FAILURE(l131)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l18,"VRF %d/%d trie init \n""failed\n"),l25,l8));
return l131;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l18,
"VRF %d/%d trie init ""completed\n"),l25,l8));}l131 = l315(l18,l8,lpm_entry,
l19,l21,l312,l313,l316);if(l131!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l18,"unit %d : Route Insertion Failed :%s\n"),l18,soc_errmsg(l131)
));return(l131);}VRF_TRIE_ROUTES_INC(l18,l25,l8);return(l131);}int
soc_alpm_128_warmboot_bucket_bitmap_set(int l1,int l31,int l234){int l324 = 1
;if(l31){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l324 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l234,l324);return SOC_E_NONE;}int
soc_alpm_128_warmboot_lpm_reinit_done(int l18){int l231;int l325 = ((3*(128+2
+1))-1);int l305 = soc_mem_index_count(l18,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l18)){l305>>= 1;}if(!soc_alpm_mode_get(l18)){(l40[(l18)][
(((3*(128+2+1))-1))].l35) = -1;for(l231 = ((3*(128+2+1))-1);l231>-1;l231--){
if(-1 == (l40[(l18)][(l231)].l33)){continue;}(l40[(l18)][(l231)].l35) = l325;
(l40[(l18)][(l325)].next) = l231;(l40[(l18)][(l325)].l37) = (l40[(l18)][(l231
)].l33)-(l40[(l18)][(l325)].l34)-1;l325 = l231;}(l40[(l18)][(l325)].next) = -
1;(l40[(l18)][(l325)].l37) = l305-(l40[(l18)][(l325)].l34)-1;}else{(l40[(l18)
][(((3*(128+2+1))-1))].l35) = -1;for(l231 = ((3*(128+2+1))-1);l231>(((3*(128+
2+1))-1)/3);l231--){if(-1 == (l40[(l18)][(l231)].l33)){continue;}(l40[(l18)][
(l231)].l35) = l325;(l40[(l18)][(l325)].next) = l231;(l40[(l18)][(l325)].l37)
= (l40[(l18)][(l231)].l33)-(l40[(l18)][(l325)].l34)-1;l325 = l231;}(l40[(l18)
][(l325)].next) = -1;(l40[(l18)][(l325)].l37) = l305-(l40[(l18)][(l325)].l34)
-1;l325 = (((3*(128+2+1))-1)/3);(l40[(l18)][((((3*(128+2+1))-1)/3))].l35) = -
1;for(l231 = ((((3*(128+2+1))-1)/3)-1);l231>-1;l231--){if(-1 == (l40[(l18)][(
l231)].l33)){continue;}(l40[(l18)][(l231)].l35) = l325;(l40[(l18)][(l325)].
next) = l231;(l40[(l18)][(l325)].l37) = (l40[(l18)][(l231)].l33)-(l40[(l18)][
(l325)].l34)-1;l325 = l231;}(l40[(l18)][(l325)].next) = -1;(l40[(l18)][(l325)
].l37) = (l305>>1)-(l40[(l18)][(l325)].l34)-1;}return(SOC_E_NONE);}int
soc_alpm_128_warmboot_lpm_reinit(int l18,int l8,int l231,void*lpm_entry){int
l15;l265(l18,lpm_entry,l231,0x4000,0);SOC_IF_ERROR_RETURN(l301(l18,lpm_entry,
&l15));if((l40[(l18)][(l15)].l36) == 0){(l40[(l18)][(l15)].l33) = l231;(l40[(
l18)][(l15)].l34) = l231;}else{(l40[(l18)][(l15)].l34) = l231;}(l40[(l18)][(
l15)].l36)++;return(SOC_E_NONE);}int soc_alpm_128_bucket_sanity_check(int l1,
soc_mem_t l243,int index){int l131 = SOC_E_NONE;int l130,l240,l250,l8,l144 = 
-1;int l151 = 0,l25;uint32 l152 = 0;uint32 l13[SOC_MAX_MEM_FIELD_WORDS];int
l326,l327,l328;defip_pair_128_entry_t lpm_entry;int l255,l329;soc_mem_t l21 = 
L3_DEFIP_ALPM_IPV6_128m;int l330 = 8,l331;int l332 = 0;soc_field_t l333 = 
VALID0_LWRf;soc_field_t l334 = GLOBAL_HIGHf;soc_field_t l335 = ALG_BKT_PTRf;
l8 = L3_DEFIP_MODE_128;l255 = soc_mem_index_min(l1,l243);l329 = 
soc_mem_index_max(l1,l243);if((index>= 0)&&(index<l255||index>l329)){return
SOC_E_PARAM;}else if(index>= 0){l255 = index;l329 = index;}SOC_ALPM_LPM_LOCK(
l1);for(l130 = l255;l130<= l329;l130++){l131 = soc_mem_read(l1,l243,
MEM_BLOCK_ANY,l130,(void*)l13);if(SOC_FAILURE(l131)){continue;}if(
soc_mem_field32_get(l1,l243,(void*)l13,l333) == 0||soc_mem_field32_get(l1,
l243,(void*)l13,l334) == 1){continue;}l331 = soc_mem_field32_get(l1,l243,(
void*)l13,l335);l131 = soc_alpm_128_lpm_vrf_get(l1,l13,&l151,&l25);if(
SOC_FAILURE(l131)){continue;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l8)){l330<<= 1;}
l144 = -1;for(l240 = 0;l240<l330;l240++){l131 = _soc_alpm_mem_index(l1,l21,
l331,l240,l152,&l250);if(SOC_FAILURE(l131)){continue;}l131 = soc_mem_read(l1,
l21,MEM_BLOCK_ANY,l250,(void*)l13);if(SOC_FAILURE(l131)){break;}if(!
soc_mem_field32_get(l1,l21,(void*)l13,VALIDf)){continue;}l131 = l24(l1,(void*
)l13,l21,l8,l151,l331,0,&lpm_entry);if(SOC_FAILURE(l131)){continue;}l131 = 
l148(l1,(void*)&lpm_entry,l21,(void*)l13,&l326,&l327,&l328,FALSE);if(
SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tLaunched AUX operation for ""index %d bucket %d sanity check failed\n"),
l130,l331));l332++;continue;}if(l327!= l331){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tEntry at index %d does not belong "
"to bucket %d(from bucket %d)\n"),l328,l331,l327));l332++;}if(l144 == -1){
l144 = l326;continue;}if(l144!= l326){int l158,l159;l158 = 
soc_alpm_logical_idx(l1,l243,SOC_ALPM_128_DEFIP_TO_PAIR(l144>>1),1);l159 = 
soc_alpm_logical_idx(l1,l243,SOC_ALPM_128_DEFIP_TO_PAIR(l326>>1),1);LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tAliased bucket %d(returned "
"bucket %d) found from TCAM1 %d and TCAM2 %d\n"),l331,l327,l158,l159));l332++
;}}}SOC_ALPM_LPM_UNLOCK(l1);if(l332 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check passed\n"),
SOC_MEM_NAME(l1,l243),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l1,l243),index,l332));return l332;}
int soc_alpm_128_pivot_sanity_check(int l1,soc_mem_t l243,int index){int l130
,l336;int l250,l255,l329;int l131 = SOC_E_NONE;int*l337 = NULL;int l326,l327,
l328;uint32 l13[SOC_MAX_MEM_FIELD_WORDS];int l338,l339,l340;soc_mem_t l341;
soc_mem_t l342 = L3_DEFIP_ALPM_IPV6_128m;int l8 = L3_DEFIP_MODE_128;int l332 = 
0;soc_field_t l333 = VALID0_LWRf;soc_field_t l334 = GLOBAL_HIGHf;soc_field_t
l343 = NEXT_HOP_INDEXf;soc_field_t l335 = ALG_BKT_PTRf;l255 = 
soc_mem_index_min(l1,l243);l329 = soc_mem_index_max(l1,l243);if((index>= 0)&&
(index<l255||index>l329)){return SOC_E_PARAM;}else if(index>= 0){l255 = index
;l329 = index;}l337 = sal_alloc(sizeof(int)*MAX_PIVOT_COUNT,
"Bucket index array 128");if(l337 == NULL){l131 = SOC_E_MEMORY;return l131;}
sal_memset(l337,0xff,sizeof(int)*MAX_PIVOT_COUNT);SOC_ALPM_LPM_LOCK(l1);for(
l130 = l255;l130<= l329;l130++){l131 = soc_mem_read(l1,l243,MEM_BLOCK_ANY,
l130,(void*)l13);if(SOC_FAILURE(l131)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l1,l243
),l130,l131));l332 = l131;continue;}if(soc_mem_field32_get(l1,l243,(void*)l13
,l333) == 0||soc_mem_field32_get(l1,l243,(void*)l13,l334) == 1){continue;}
l338 = soc_mem_field32_get(l1,l243,(void*)l13,l335);if(l338!= 0){if(l337[l338
] == -1){l337[l338] = l130;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tDuplicated bucket pointer ""%d detected, in memory %s index1 %d and "
"index2 %d\n"),l338,SOC_MEM_NAME(l1,l243),l337[l338],l130));l332 = l131;
continue;}}l131 = alpm_bucket_is_assigned(l1,l338,l8,&l336);if(l131 == 
SOC_E_PARAM){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tInvalid bucket pointer %d ""detected, in memory %s index %d\n"),l338,
SOC_MEM_NAME(l1,l243),l130));l332 = l131;continue;}if(l131>= 0&&l336 == 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),l338,SOC_MEM_NAME(l1,l243),l130));l332 = 
l131;continue;}l341 = _soc_trident2_alpm_bkt_view_get(l1,l338<<2);if(l341!= 
L3_DEFIP_ALPM_IPV6_128m){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual %s\n"),l338,SOC_MEM_NAME(l1,
L3_DEFIP_ALPM_IPV6_128m),SOC_MEM_NAME(l1,l341)));l332 = l131;continue;}l326 = 
-1;l339 = soc_mem_field32_get(l1,l243,(void*)l13,l343);l131 = l148(l1,l13,
l342,(void*)l13,&l326,&l327,&l328,FALSE);if(l326 == -1){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLaunched AUX operation for PIVOT "
"index %d sanity check failed\n"),l130));l332 = l131;continue;}l250 = 
soc_alpm_logical_idx(l1,l243,SOC_ALPM_128_DEFIP_TO_PAIR(l326>>1),1);l131 = 
soc_mem_read(l1,l243,MEM_BLOCK_ANY,l250,(void*)l13);if(SOC_FAILURE(l131)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tRead memory %s index %d (nexthop) ""return %d\n"),SOC_MEM_NAME(l1,l243),
l250,l131));l332 = l131;continue;}l340 = soc_mem_field32_get(l1,l243,(void*)
l13,l343);if(l339!= l340){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d index2 %d \n"),l130,l250));l332 = l131;continue;}}
SOC_ALPM_LPM_UNLOCK(l1);sal_free(l337);if(l332 == 0){LOG_INFO(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check passed\n"),
SOC_MEM_NAME(l1,l243),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check "
"failed, encountered %d error(s)\n"),SOC_MEM_NAME(l1,l243),index,l332));
return SOC_E_FAIL;}
#endif /* ALPM_ENABLE */
