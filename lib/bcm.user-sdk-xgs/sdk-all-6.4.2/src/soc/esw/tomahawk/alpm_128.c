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
void soc_th_alpm_128_lpm_state_dump(int l1);extern int soc_th_get_alpm_banks(
int unit);extern int soc_th_alpm_mode_get(int l1);static int l2(int l1,void*
l3,int*index);static int l4(int l1,void*key_data);static int l5(int l1,void*
key_data,int l6,int l7,int l8,defip_aux_scratch_entry_t*l9);static int l10(
int l1,void*key_data,void*l11,int*l12,int*l13,int*l6);static int l14(int l1,
void*key_data,void*l11,int*l12);static int l15(int unit,void*lpm_entry,void*
l16,void*l17,soc_mem_t l18,uint32 l19,uint32*l20);static int l21(int unit,
void*l16,soc_mem_t l18,int l6,int l22,int l23,int index,void*lpm_entry);
static int l24(int unit,uint32*key,int len,int l22,int l6,
defip_pair_128_entry_t*lpm_entry,int l25,int l26);static int l27(int l1,int
l22,int l28);static int l29(alpm_pfx_info_t*l30,trie_t*l31,uint32*l32,uint32
l33,trie_node_t*l34,defip_pair_128_entry_t*lpm_entry);static int l35(
alpm_pfx_info_t*l30,int*l36,int*l12);extern int l37(int l1,trie_t*l31,trie_t*
l38,payload_t*new_pfx_pyld,int*l39,int bucket_index,int l28);extern int
_soc_th_alpm_rollback_bkt_move(int l1,void*key_data,soc_mem_t l18,
alpm_pivot_t*l40,alpm_pivot_t*l41,alpm_mem_prefix_array_t*l42,int*l39,int l43
);extern int soc_th_alpm_lpm_delete(int l1,void*key_data);void l44(int l1,
defip_pair_128_entry_t*l45,void*key_data,int tcam_index,alpm_pivot_t*
pivot_pyld);typedef struct l46{int l47;int l48;int l49;int next;int l50;int
l51;}l52,*l53;static l53 l54[SOC_MAX_NUM_DEVICES];typedef struct l55{
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
soc_field_info_t*l89;soc_field_info_t*l90;soc_field_info_t*l91;
soc_field_info_t*l92;soc_field_info_t*l93;soc_field_info_t*l94;
soc_field_info_t*l95;soc_field_info_t*l96;soc_field_info_t*l97;
soc_field_info_t*l98;soc_field_info_t*l99;}l100,*l101;static l101 l102[
SOC_MAX_NUM_DEVICES];typedef struct l103{int unit;int l104;int l105;uint16*
l106;uint16*l107;}l108;typedef uint32 l109[9];typedef int(*l110)(l109 l111,
l109 l112);static l108*l113[SOC_MAX_NUM_DEVICES];static void l114(int l1,void
*l11,int index,l109 l115);static uint16 l116(uint8*l117,int l118);static int
l119(int unit,int l104,int l105,l108**l120);static int l121(l108*l122);static
int l123(l108*l124,l110 l125,l109 entry,int l126,uint16*l127);static int l128
(l108*l124,l110 l125,l109 entry,int l126,uint16 l129,uint16 l39);static int
l130(l108*l124,l110 l125,l109 entry,int l126,uint16 l131);static int l132(int
l1,const void*entry,int*l126){int l133,l134;int l135[4] = {IP_ADDR_MASK0_LWRf
,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};uint32 l136;l136 = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l135[0]);if((l134 = 
_ipmask2pfx(l136,l126))<0){return(l134);}for(l133 = 1;l133<4;l133++){l136 = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l135[l133]);if(*l126){if(l136
!= 0xffffffff){return(SOC_E_PARAM);}*l126+= 32;}else{if((l134 = _ipmask2pfx(
l136,l126))<0){return(l134);}}}return SOC_E_NONE;}static void l137(uint32*
l138,int l33,int l28){uint32 l139,l140,l47,prefix[5];int l133;sal_memcpy(
prefix,l138,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(l138,0,
sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));l139 = 128-l33;l47 = (l139+31)/
32;if((l139%32) == 0){l47++;}l139 = l139%32;for(l133 = l47;l133<= 4;l133++){
prefix[l133]<<= l139;l140 = prefix[l133+1]&~(0xffffffff>>l139);l140 = (((32-
l139) == 32)?0:(l140)>>(32-l139));if(l133<4){prefix[l133]|= l140;}}for(l133 = 
l47;l133<= 4;l133++){l138[3-(l133-l47)] = prefix[l133];}}static void l141(int
unit,void*lpm_entry,int l13){int l133;soc_field_t l142[4] = {
IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};
for(l133 = 0;l133<4;l133++){soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,
lpm_entry,l142[l133],0);}for(l133 = 0;l133<4;l133++){if(l13<= 32)break;
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l142[3-l133],0xffffffff
);l13-= 32;}soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l142[3-l133
],~(((l13) == 32)?0:(0xffffffff)>>(l13)));}static int l143(int l1,void*entry,
uint32*l138,uint32*l13,int*l20){int l133;int l126 = 0,l47;int l134 = 
SOC_E_NONE;uint32 l139,l140;uint32 prefix[5];sal_memset(l138,0,sizeof(uint32)
*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(prefix,0,sizeof(uint32)*BITS2WORDS
(_MAX_KEY_LEN_144_));prefix[0] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR0_LWRf);prefix[1] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR1_LWRf);prefix[2] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR0_UPRf);prefix[3] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR1_UPRf);if(l20!= NULL){*l20 = (prefix[0] == 0)&&(prefix[1] == 0)
&&(prefix[2] == 0)&&(prefix[3] == 0)&&(l126 == 0);}l134 = l132(l1,entry,&l126
);if(SOC_FAILURE(l134)){return l134;}l139 = 128-l126;l47 = l139/32;l139 = 
l139%32;for(l133 = l47;l133<4;l133++){prefix[l133]>>= l139;l140 = prefix[l133
+1]&((1<<l139)-1);l140 = (((32-l139) == 32)?0:(l140)<<(32-l139));prefix[l133]
|= l140;}for(l133 = l47;l133<4;l133++){l138[4-(l133-l47)] = prefix[l133];}*
l13 = l126;return SOC_E_NONE;}int l144(int l1,uint32*prefix,uint32 l33,int l6
,int l22,int*l145,int*tcam_index,int*bucket_index){int l134 = SOC_E_NONE;
trie_t*l146;trie_node_t*l147 = NULL;alpm_pivot_t*pivot_pyld;l146 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l22);l134 = trie_find_lpm(l146,prefix,l33,&l147);
if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l134;}pivot_pyld = (alpm_pivot_t*)l147;*l145 = 
1;*tcam_index = PIVOT_TCAM_INDEX(pivot_pyld);*bucket_index = 
PIVOT_BUCKET_INDEX(pivot_pyld);return SOC_E_NONE;}static int l148(int l1,void
*key_data,soc_mem_t l18,void*alpm_data,int*tcam_index,int*bucket_index,int*
l12){uint32 l11[SOC_MAX_MEM_FIELD_WORDS];int l149,l22,l28;int l127;uint32 l150
;int l134 = SOC_E_NONE;int l145 = 0;l28 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l149,&l22));if(
l149 == 0){if(soc_th_alpm_mode_get(l1)){return SOC_E_PARAM;}}if(l22 == 
SOC_VRF_MAX(l1)+1){if(soc_th_get_alpm_banks(l1)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l150);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l150);}}else{if(soc_th_get_alpm_banks(l1)
<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,l150);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l150);}}if(l149!= SOC_L3_VRF_OVERRIDE){
uint32 prefix[5],l33;int l20 = 0;l134 = l143(l1,key_data,prefix,&l33,&l20);if
(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l134;}l134 = l144(l1,
prefix,l33,l28,l22,&l145,tcam_index,bucket_index);SOC_IF_ERROR_RETURN(l134);
if(l145){l15(l1,key_data,l11,0,l18,0,0);l134 = _soc_th_alpm_find_in_bkt(l1,
l18,*bucket_index,l150,l11,alpm_data,&l127,l28);if(SOC_SUCCESS(l134)){*l12 = 
l127;}}else{l134 = SOC_E_NOT_FOUND;}}return l134;}static int l151(int l1,void
*key_data,void*alpm_data,void*alpm_sip_data,soc_mem_t l18,int l127){
defip_aux_scratch_entry_t l9;int l149,l28,l22;int bucket_index;uint32 l7,l150
;int l134 = SOC_E_NONE;int l145 = 0,l140 = 0;int tcam_index;l28 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data
,&l149,&l22));if(l22 == SOC_VRF_MAX(l1)+1){l7 = 0;if(soc_th_get_alpm_banks(l1
)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l150);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l150);}}else{l7 = 2;if(
soc_th_get_alpm_banks(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,
l150);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l150);}}if(!soc_th_alpm_mode_get
(l1)){l7 = 2;}if(l149!= SOC_L3_VRF_OVERRIDE){sal_memset(&l9,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l5(l1,key_data,l28,l7,0,&l9))
;SOC_IF_ERROR_RETURN(soc_mem_write(l1,l18,MEM_BLOCK_ANY,l127,alpm_data));if(
l134!= SOC_E_NONE){return SOC_E_FAIL;}if(SOC_URPF_STATUS_GET(l1)){
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry
(l1,l127),alpm_sip_data));if(l134!= SOC_E_NONE){return SOC_E_FAIL;}}l140 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l9,IP_LENGTHf);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,REPLACE_LENf,l140);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l9,TRUE,&l145,&
tcam_index,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l140 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l9,DB_TYPEf);l140+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,DB_TYPEf,l140);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l9,TRUE,&l145,&
tcam_index,&bucket_index));}}return l134;}static int l152(int l1,int l36,int
l153){int l134,l140,l154,l155;defip_aux_table_entry_t l156,l157;l154 = 
SOC_ALPM_128_ADDR_LWR(l36);l155 = SOC_ALPM_128_ADDR_UPR(l36);l134 = 
soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l154,&l156);
SOC_IF_ERROR_RETURN(l134);l134 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l155,&l157);SOC_IF_ERROR_RETURN(l134);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH0f,l153);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH1f,l153);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH0f,l153);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH1f,l153);l140 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,&l156,DB_TYPE0f);l134 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l154,&l156);SOC_IF_ERROR_RETURN(l134);l134 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l155,&l157);
SOC_IF_ERROR_RETURN(l134);if(SOC_URPF_STATUS_GET(l1)){l140++;
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH0f,l153);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH1f,l153);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH0f,l153);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH1f,l153);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,DB_TYPE0f,l140);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,DB_TYPE1f,l140);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,DB_TYPE0f,l140);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,DB_TYPE1f,l140);l154+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l155+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(
l1,L3_DEFIPm))/2;l134 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
l154,&l156);SOC_IF_ERROR_RETURN(l134);l134 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l155,&l157);}return l134;}static int l158(
int l1,int l159,void*entry,defip_aux_table_entry_t*l160,int l161){uint32 l140
,l7,l162 = 0;soc_mem_t l18 = L3_DEFIP_PAIR_128m;soc_mem_t l163 = 
L3_DEFIP_AUX_TABLEm;int l134 = SOC_E_NONE,l126,l22;void*l164,*l165;l164 = (
void*)l160;l165 = (void*)(l160+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l163,
MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l159),l160));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l163,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l159),l160+1));l140 = 
soc_mem_field32_get(l1,l18,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l163,
l164,VRF0f,l140);l140 = soc_mem_field32_get(l1,l18,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l163,l164,VRF1f,l140);l140 = soc_mem_field32_get(l1,
l18,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l163,l165,VRF0f,l140);l140 = 
soc_mem_field32_get(l1,l18,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l163,
l165,VRF1f,l140);l140 = soc_mem_field32_get(l1,l18,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l163,l164,MODE0f,l140);l140 = soc_mem_field32_get(l1,
l18,entry,MODE1_LWRf);soc_mem_field32_set(l1,l163,l164,MODE1f,l140);l140 = 
soc_mem_field32_get(l1,l18,entry,MODE0_UPRf);soc_mem_field32_set(l1,l163,l165
,MODE0f,l140);l140 = soc_mem_field32_get(l1,l18,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l163,l165,MODE1f,l140);l140 = soc_mem_field32_get(l1,
l18,entry,VALID0_LWRf);soc_mem_field32_set(l1,l163,l164,VALID0f,l140);l140 = 
soc_mem_field32_get(l1,l18,entry,VALID1_LWRf);soc_mem_field32_set(l1,l163,
l164,VALID1f,l140);l140 = soc_mem_field32_get(l1,l18,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l163,l165,VALID0f,l140);l140 = soc_mem_field32_get(l1,
l18,entry,VALID1_UPRf);soc_mem_field32_set(l1,l163,l165,VALID1f,l140);l134 = 
l132(l1,entry,&l126);SOC_IF_ERROR_RETURN(l134);soc_mem_field32_set(l1,l163,
l164,IP_LENGTH0f,l126);soc_mem_field32_set(l1,l163,l164,IP_LENGTH1f,l126);
soc_mem_field32_set(l1,l163,l165,IP_LENGTH0f,l126);soc_mem_field32_set(l1,
l163,l165,IP_LENGTH1f,l126);l140 = soc_mem_field32_get(l1,l18,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l163,l164,IP_ADDR0f,l140);l140 = 
soc_mem_field32_get(l1,l18,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l163,
l164,IP_ADDR1f,l140);l140 = soc_mem_field32_get(l1,l18,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l163,l165,IP_ADDR0f,l140);l140 = soc_mem_field32_get(
l1,l18,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l163,l165,IP_ADDR1f,l140);
l140 = soc_mem_field32_get(l1,l18,entry,ENTRY_TYPE0_LWRf);soc_mem_field32_set
(l1,l163,l164,ENTRY_TYPE0f,l140);l140 = soc_mem_field32_get(l1,l18,entry,
ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,l163,l164,ENTRY_TYPE1f,l140);l140 = 
soc_mem_field32_get(l1,l18,entry,ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,
l163,l165,ENTRY_TYPE0f,l140);l140 = soc_mem_field32_get(l1,l18,entry,
ENTRY_TYPE1_UPRf);soc_mem_field32_set(l1,l163,l165,ENTRY_TYPE1f,l140);l134 = 
soc_th_alpm_128_lpm_vrf_get(l1,entry,&l22,&l126);SOC_IF_ERROR_RETURN(l134);if
(SOC_URPF_STATUS_GET(l1)){if(l161>= (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1)){l162 = 1;}}switch(l22){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l163,l164,VALID0f,0);soc_mem_field32_set(l1,l163,l164,
VALID1f,0);soc_mem_field32_set(l1,l163,l165,VALID0f,0);soc_mem_field32_set(l1
,l163,l165,VALID1f,0);l7 = 0;break;case SOC_L3_VRF_GLOBAL:l7 = l162?1:0;break
;default:l7 = l162?3:2;break;}soc_mem_field32_set(l1,l163,l164,DB_TYPE0f,l7);
soc_mem_field32_set(l1,l163,l164,DB_TYPE1f,l7);soc_mem_field32_set(l1,l163,
l165,DB_TYPE0f,l7);soc_mem_field32_set(l1,l163,l165,DB_TYPE1f,l7);if(l162){
l140 = soc_mem_field32_get(l1,l18,entry,ALG_BKT_PTRf);if(l140){l140+= 
SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l18,entry,ALG_BKT_PTRf,l140)
;}}return SOC_E_NONE;}static int l166(int l1,int l167,int index,int l168,void
*entry){defip_aux_table_entry_t l160[2];l168 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l168,1);SOC_IF_ERROR_RETURN(l158(l1,l168,entry,&l160[0],
index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,
entry));index = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(index),l160));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(index),l160+
1));return SOC_E_NONE;}void l44(int l1,defip_pair_128_entry_t*l45,void*
key_data,int tcam_index,alpm_pivot_t*pivot_pyld){int l134;trie_t*l146 = NULL;
int l28,l22,l149;trie_node_t*l169 = NULL;l28 = L3_DEFIP_MODE_128;
soc_th_alpm_lpm_vrf_get(l1,key_data,&l149,&l22);l134 = soc_th_alpm_lpm_delete
(l1,l45);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Failure to free new prefix""at %d\n"),
soc_th_alpm_logical_idx(l1,L3_DEFIPm,tcam_index,l28)));}l146 = 
VRF_PIVOT_TRIE_IPV6(l1,l22);if(ALPM_TCAM_PIVOT(l1,tcam_index<<(l28?1:0))!= 
NULL){l134 = trie_delete(l146,pivot_pyld->key,pivot_pyld->len,&l169);if(
SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l1,tcam_index<<(l28?1:0)) = NULL;VRF_PIVOT_REF_DEC(l1,l22,l28
);}static int l170(int l1,soc_mem_t l18,alpm_mem_prefix_array_t*l171,int*l39)
{int l133,l134 = SOC_E_NONE,l172;defip_alpm_ipv6_128_entry_t l173,l174;void*
l175 = NULL,*l176 = NULL;int l177,l178;int*l179 = NULL;int l180 = FALSE;l177 = 
sizeof(l173);l178 = sizeof(l174);l175 = sal_alloc(l177*l171->count,"rb_bufp")
;if(l175 == NULL){l134 = SOC_E_MEMORY;goto l181;}l176 = sal_alloc(l178*l171->
count,"rb_sip_bufp");if(l176 == NULL){l134 = SOC_E_MEMORY;goto l181;}l179 = 
sal_alloc(sizeof(*l179)*l171->count,"roll_back_index");if(l179 == NULL){l134 = 
SOC_E_MEMORY;goto l181;}sal_memset(l179,-1,sizeof(*l179)*l171->count);for(
l133 = 0;l133<l171->count;l133++){payload_t*prefix = l171->prefix[l133];if(
prefix->index>= 0){l134 = soc_mem_read(l1,l18,MEM_BLOCK_ANY,prefix->index,(
uint8*)l175+l133*l177);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x 0x%08x"" 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l133--;l180 = TRUE;
break;}if(SOC_URPF_STATUS_GET(l1)){l134 = soc_mem_read(l1,l18,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,prefix->index),(uint8*)l176+l133*l178);if(
SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x"" 0x%08x 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l133--;l180 = TRUE;
break;}}l134 = soc_mem_write(l1,l18,MEM_BLOCK_ALL,prefix->index,
soc_mem_entry_null(l1,l18));if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old bkt for pfx 0x%08x 0x%08x\n"),prefix->key[0],prefix->key[
1]));l179[l133] = prefix->index;l180 = TRUE;break;}if(SOC_URPF_STATUS_GET(l1)
){l134 = soc_mem_write(l1,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,prefix
->index),soc_mem_entry_null(l1,l18));if(SOC_FAILURE(l134)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old urpf bkt for pfx 0x%08x"" 0x%08x\n"),prefix->key[0],
prefix->key[1]));l179[l133] = prefix->index;l180 = TRUE;break;}}}l179[l133] = 
prefix->index;prefix->index = l39[l133];}if(l180){for(;l133>= 0;l133--){
payload_t*prefix = l171->prefix[l133];prefix->index = l179[l133];if(l179[l133
]<0){continue;}l172 = soc_mem_write(l1,l18,MEM_BLOCK_ALL,l179[l133],(uint8*)
l175+l133*l177);if(SOC_FAILURE(l172)){break;}if(!SOC_URPF_STATUS_GET(l1)){
continue;}l172 = soc_mem_write(l1,l18,MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(l1
,l179[l133]),(uint8*)l176+l133*l178);if(SOC_FAILURE(l172)){break;}}}l181:if(
l179){sal_free(l179);l179 = NULL;}if(l176){sal_free(l176);l176 = NULL;}if(
l175){sal_free(l175);l175 = NULL;}return l134;}static int l29(alpm_pfx_info_t
*l30,trie_t*l31,uint32*l32,uint32 l33,trie_node_t*l34,defip_pair_128_entry_t*
lpm_entry){trie_node_t*l147 = NULL;int l28,l22,l149;
defip_alpm_ipv6_64_entry_t l173;payload_t*l182 = NULL;int l183;void*l184;int
l134 = SOC_E_NONE;soc_mem_t l18;alpm_bucket_handle_t*l185;l183 = (l30->
pivot_pyld)->tcam_index;l28 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l30->unit,l30->key_data,&l149,&l22));l18 = 
L3_DEFIP_ALPM_IPV6_128m;l184 = ((uint32*)&(l173));l147 = NULL;l134 = 
trie_find_lpm(l31,l32,l33,&l147);l182 = (payload_t*)l147;if(SOC_FAILURE(l134)
){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l30->unit,l32[0],l32[1],
l32[2],l32[3],l32[4],l33));return l134;}if(l182->bkt_ptr){if(l182->bkt_ptr == 
l30->new_pfx_pyld){sal_memcpy(l184,l30->alpm_data,sizeof(
defip_alpm_ipv6_128_entry_t));}else{l134 = soc_mem_read(l30->unit,l18,
MEM_BLOCK_ANY,((payload_t*)l182->bkt_ptr)->index,l184);}if(SOC_FAILURE(l134))
{return l134;}l134 = l21(l30->unit,l184,l18,l28,l149,l30->bucket_index,0,&
lpm_entry);if(SOC_FAILURE(l134)){return l134;}}else{l134 = soc_mem_read(l30->
unit,L3_DEFIPm,MEM_BLOCK_ANY,soc_th_alpm_logical_idx(l30->unit,L3_DEFIPm,l183
>>1,1),&lpm_entry);if((!l28)&&(l183&1)){l134 = soc_th_alpm_lpm_ip4entry1_to_0
(l30->unit,&lpm_entry,&lpm_entry,0);}}l185 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l185 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l134 = SOC_E_MEMORY;return l134;}sal_memset(l185,0,sizeof(*l185));l30->
pivot_pyld = sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l30->
pivot_pyld == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l134 = SOC_E_MEMORY;return l134;}sal_memset(l30->pivot_pyld,0,sizeof(l30->
pivot_pyld));PIVOT_BUCKET_HANDLE(l30->pivot_pyld) = l185;l134 = trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l30->pivot_pyld));PIVOT_BUCKET_TRIE(l30
->pivot_pyld)->trie = l34;PIVOT_BUCKET_INDEX(l30->pivot_pyld) = l30->
bucket_index;PIVOT_BUCKET_VRF(l30->pivot_pyld) = l22;PIVOT_BUCKET_IPV6(l30->
pivot_pyld) = l28;PIVOT_BUCKET_DEF(l30->pivot_pyld) = FALSE;(l30->pivot_pyld)
->key[0] = l32[0];(l30->pivot_pyld)->key[1] = l32[1];(l30->pivot_pyld)->key[2
] = l32[2];(l30->pivot_pyld)->key[3] = l32[3];(l30->pivot_pyld)->key[4] = l32
[4];(l30->pivot_pyld)->len = l33;l137((l32),(l33),(l28));l24(l30->unit,l32,
l33,l22,l28,lpm_entry,0,0);soc_L3_DEFIP_PAIR_128m_field32_set(l30->unit,
lpm_entry,ALG_BKT_PTR0f,l30->bucket_index);return l134;}static int l35(
alpm_pfx_info_t*l30,int*l36,int*l12){trie_node_t*l34;uint32 l11[
SOC_MAX_MEM_FIELD_WORDS];int l28,l22,l149;uint32 l33,l153 = 0;uint32 l150 = 0
;uint32 l32[5];int l127;defip_pair_128_entry_t l173,l174;trie_t*l31,*trie;
void*l184,*l186;alpm_pivot_t*l187 = l30->pivot_pyld;defip_pair_128_entry_t
lpm_entry;soc_mem_t l18;trie_t*l146 = NULL;alpm_mem_prefix_array_t l171;int*
l39 = NULL;int l134 = SOC_E_NONE,l133,l43 = -1;l28 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l30->unit,l30->key_data,&l149,&
l22));l18 = L3_DEFIP_ALPM_IPV6_128m;l184 = ((uint32*)&(l173));l186 = ((uint32
*)&(l174));if(l22 == SOC_VRF_MAX(l30->unit)+1){if(soc_th_get_alpm_banks(l30->
unit)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l30->unit,l150);}
else{SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l30->unit,l150);}}else{if(
soc_th_get_alpm_banks(l30->unit)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l30->unit,l150);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l30->unit,l150);}}l31 = 
VRF_PREFIX_TRIE_IPV6_128(l30->unit,l22);trie = PIVOT_BUCKET_TRIE(l30->
pivot_pyld);l134 = alpm_bucket_assign(l30->unit,&l30->bucket_index,l28);if(
SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,
"_soc_alpm_insert: Unable to allocate""new bucket for split\n")));l30->
bucket_index = -1;l37(l30->unit,l31,trie,l30->new_pfx_pyld,l39,l30->
bucket_index,l28);return l134;}l134 = trie_split(trie,_MAX_KEY_LEN_144_,FALSE
,l32,&l33,&l34,NULL,FALSE);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l30->unit,"_soc_alpm_insert: Could not split bucket")));l37(l30->
unit,l31,trie,l30->new_pfx_pyld,l39,l30->bucket_index,l28);return l134;}l134 = 
l29(l30,l31,l32,l33,l34,&lpm_entry);if(l134!= SOC_E_NONE){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,"could not initialize pivot")));l37(l30
->unit,l31,trie,l30->new_pfx_pyld,l39,l30->bucket_index,l28);return l134;}
sal_memset(&l171,0,sizeof(l171));l134 = trie_traverse(PIVOT_BUCKET_TRIE(l30->
pivot_pyld),alpm_mem_prefix_array_cb,&l171,_TRIE_INORDER_TRAVERSE);if(
SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,
"_soc_alpm_insert: Bucket split failed")));l37(l30->unit,l31,trie,l30->
new_pfx_pyld,l39,l30->bucket_index,l28);return l134;}l39 = sal_alloc(sizeof(*
l39)*l171.count,"Temp storage for location of prefixes in new 128b bucket");
if(l39 == NULL){l134 = SOC_E_MEMORY;l37(l30->unit,l31,trie,l30->new_pfx_pyld,
l39,l30->bucket_index,l28);return l134;}sal_memset(l39,-1,sizeof(*l39)*l171.
count);for(l133 = 0;l133<l171.count;l133++){payload_t*l126 = l171.prefix[l133
];if(l126->index>0){l134 = soc_mem_read(l30->unit,l18,MEM_BLOCK_ANY,l126->
index,l184);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l30->
unit,"_soc_alpm_insert: Failed to""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l171.prefix[l133]->key[1],l171.
prefix[l133]->key[2],l171.prefix[l133]->key[3],l171.prefix[l133]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l30->unit,l30->key_data,l18,l187,l30->
pivot_pyld,&l171,l39,l43);return l134;}if(SOC_URPF_STATUS_GET(l30->unit)){
l134 = soc_mem_read(l30->unit,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l30->
unit,l126->index),l186);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l30->unit,"_soc_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l171.prefix[l133]->key[1],l171.
prefix[l133]->key[2],l171.prefix[l133]->key[3],l171.prefix[l133]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l30->unit,l30->key_data,l18,l187,l30->
pivot_pyld,&l171,l39,l43);return l134;}}l134 = _soc_th_alpm_insert_in_bkt(l30
->unit,l18,l30->bucket_index,l150,l184,l11,&l127,l28);if(SOC_SUCCESS(l134)){
if(SOC_URPF_STATUS_GET(l30->unit)){l134 = soc_mem_write(l30->unit,l18,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l30->unit,l127),l186);}}}else{l134 = 
_soc_th_alpm_insert_in_bkt(l30->unit,l18,l30->bucket_index,l150,l30->
alpm_data,l11,&l127,l28);if(SOC_SUCCESS(l134)){l43 = l133;*l12 = l127;if(
SOC_URPF_STATUS_GET(l30->unit)){l134 = soc_mem_write(l30->unit,l18,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l30->unit,l127),l30->alpm_sip_data);}}}
l39[l133] = l127;if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l30->unit,"_soc_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l171.prefix[l133]->key[1],l171.
prefix[l133]->key[2],l171.prefix[l133]->key[3],l171.prefix[l133]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l30->unit,l30->key_data,l18,l187,l30->
pivot_pyld,&l171,l39,l43);return l134;}}l134 = l2(l30->unit,&lpm_entry,l36);
if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,
"_soc_alpm_insert: Unable to add new""pivot to tcam\n")));if(l134 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l30->unit,l22,l28);}(void)
_soc_th_alpm_rollback_bkt_move(l30->unit,l30->key_data,l18,l187,l30->
pivot_pyld,&l171,l39,l43);return l134;}*l36 = soc_th_alpm_physical_idx(l30->
unit,L3_DEFIPm,*l36,l28);l134 = l152(l30->unit,*l36,l153);if(SOC_FAILURE(l134
)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l36));l44(l30->
unit,&lpm_entry,l30->key_data,*l36,l30->pivot_pyld);(void)
_soc_th_alpm_rollback_bkt_move(l30->unit,l30->key_data,l18,l187,l30->
pivot_pyld,&l171,l39,l43);return l134;}l134 = trie_insert(l146,(l30->
pivot_pyld)->key,NULL,(l30->pivot_pyld)->len,(trie_node_t*)l30->pivot_pyld);
if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,
"failed to insert into pivot trie\n")));(void)_soc_th_alpm_rollback_bkt_move(
l30->unit,l30->key_data,l18,l187,l30->pivot_pyld,&l171,l39,l43);return l134;}
ALPM_TCAM_PIVOT(l30->unit,SOC_ALPM_128_ADDR_LWR(*l36)<<1) = l30->pivot_pyld;
PIVOT_TCAM_INDEX(l30->pivot_pyld) = SOC_ALPM_128_ADDR_LWR(*l36)<<1;
VRF_PIVOT_REF_INC(l30->unit,l22,l28);l134 = l170(l30->unit,l18,&l171,l39);if(
SOC_FAILURE(l134)){l44(l30->unit,&lpm_entry,l30->key_data,*l36,l30->
pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l30->unit,l30->key_data,l18,
l187,l30->pivot_pyld,&l171,l39,l43);sal_free(l39);l39 = NULL;return l134;}
sal_free(l39);if(l43 == -1){l134 = _soc_th_alpm_insert_in_bkt(l30->unit,l18,
PIVOT_BUCKET_INDEX(l187),l150,l30->alpm_data,l11,&l127,l28);if(SOC_FAILURE(
l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l30->unit,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n")));
l37(l30->unit,l31,trie,l30->new_pfx_pyld,l39,l30->bucket_index,l28);return
l134;}if(SOC_URPF_STATUS_GET(l30->unit)){l134 = soc_mem_write(l30->unit,l18,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l30->unit,l127),l30->alpm_sip_data);}*
l12 = l127;l30->new_pfx_pyld->index = l127;}PIVOT_BUCKET_ENT_CNT_UPDATE(l30->
pivot_pyld);VRF_BUCKET_SPLIT_INC(l30->unit,l22,l28);return l134;}static int
l188(int l1,void*key_data,soc_mem_t l18,void*alpm_data,void*alpm_sip_data,int
*l12,int bucket_index,int tcam_index){alpm_pivot_t*pivot_pyld,*l187;
defip_aux_scratch_entry_t l9;uint32 l11[SOC_MAX_MEM_FIELD_WORDS];uint32 prefix
[5],l33;int l28,l22,l149;int l127;int l134 = SOC_E_NONE,l172;uint32 l7,l150;
int l145 =0;int l36;int l189 = 0;trie_t*trie,*l31;trie_node_t*l147 = NULL;
payload_t*l190,*l191,*l182;int l20 = 0;int*l39 = NULL;alpm_pfx_info_t l30;l28
= L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,
key_data,&l149,&l22));if(l22 == SOC_VRF_MAX(l1)+1){l7 = 0;if(
soc_th_get_alpm_banks(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(
l1,l150);}else{SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l150);}}else{l7 = 2;if(
soc_th_get_alpm_banks(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,
l150);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l150);}}l18 = 
L3_DEFIP_ALPM_IPV6_128m;l134 = l143(l1,key_data,prefix,&l33,&l20);if(
SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: prefix create failed\n")));return l134;}sal_memset(&l9
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l5(l1,key_data,l28,
l7,0,&l9));if(bucket_index == 0){l134 = l144(l1,prefix,l33,l28,l22,&l145,&
tcam_index,&bucket_index);SOC_IF_ERROR_RETURN(l134);if(l145 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: "
" Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l134 = 
_soc_th_alpm_insert_in_bkt(l1,l18,bucket_index,l150,alpm_data,l11,&l127,l28);
if(l134 == SOC_E_NONE){*l12 = l127;if(SOC_URPF_STATUS_GET(l1)){l172 = 
soc_mem_write(l1,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l127),
alpm_sip_data);if(SOC_FAILURE(l172)){return l172;}}}if(l134 == SOC_E_FULL){
l189 = 1;}pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l187 = pivot_pyld;l190 = sal_alloc(sizeof(
payload_t),"Payload for 128b Key");if(l190 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"trie node \n")));return SOC_E_MEMORY;}l191 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l191 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l190);return SOC_E_MEMORY;}sal_memset(l190,0,
sizeof(*l190));sal_memset(l191,0,sizeof(*l191));l190->key[0] = prefix[0];l190
->key[1] = prefix[1];l190->key[2] = prefix[2];l190->key[3] = prefix[3];l190->
key[4] = prefix[4];l190->len = l33;l190->index = l127;sal_memcpy(l191,l190,
sizeof(*l190));l191->bkt_ptr = l190;l134 = trie_insert(trie,prefix,NULL,l33,(
trie_node_t*)l190);if(SOC_FAILURE(l134)){if(l190!= NULL){sal_free(l190);}if(
l191!= NULL){sal_free(l191);}return l134;}l31 = VRF_PREFIX_TRIE_IPV6_128(l1,
l22);if(!l20){l134 = trie_insert(l31,prefix,NULL,l33,(trie_node_t*)l191);}
else{l147 = NULL;l134 = trie_find_lpm(l31,0,0,&l147);l182 = (payload_t*)l147;
if(SOC_SUCCESS(l134)){l182->bkt_ptr = l190;}}if(SOC_FAILURE(l134)){l37(l1,l31
,trie,l191,l39,bucket_index,l28);return l134;}if(l189){l30.unit = l1;l30.
key_data = key_data;l30.new_pfx_pyld = l190;l30.pivot_pyld = pivot_pyld;l30.
alpm_data = alpm_data;l30.alpm_sip_data = alpm_sip_data;l30.bucket_index = 
bucket_index;l134 = l35(&l30,&l36,l12);if(l134!= SOC_E_NONE){return l134;}
pivot_pyld = ALPM_TCAM_PIVOT(l1,l36);}VRF_TRIE_ROUTES_INC(l1,l22,l28);if(l20)
{sal_free(l191);}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,
&l9,TRUE,&l145,&tcam_index,&bucket_index));SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l9,FALSE,&l145,&tcam_index,&
bucket_index));if(SOC_URPF_STATUS_GET(l1)){l33 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l9,DB_TYPEf);l33+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l9,DB_TYPEf,l33);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l9,TRUE,&l145,&tcam_index,&
bucket_index));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&
l9,FALSE,&l145,&tcam_index,&bucket_index));}PIVOT_BUCKET_ENT_CNT_UPDATE(l187)
;return l134;}static int l24(int unit,uint32*key,int len,int l22,int l6,
defip_pair_128_entry_t*lpm_entry,int l25,int l26){uint32 l140;if(l26){
sal_memset(lpm_entry,0,sizeof(defip_pair_128_entry_t));}
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VRF_ID_0_LWRf,l22&
SOC_VRF_MAX(unit));soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_1_LWRf,l22&SOC_VRF_MAX(unit));soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,VRF_ID_0_UPRf,l22&SOC_VRF_MAX(unit));
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VRF_ID_1_UPRf,l22&
SOC_VRF_MAX(unit));if(l22 == (SOC_VRF_MAX(unit)+1)){l140 = 0;}else{l140 = 
SOC_VRF_MAX(unit);}soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_MASK0_LWRf,l140);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_MASK1_LWRf,l140);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_MASK0_UPRf,l140);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_MASK1_UPRf,l140);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
IP_ADDR0_LWRf,key[0]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
IP_ADDR1_LWRf,key[1]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
IP_ADDR0_UPRf,key[2]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
IP_ADDR1_UPRf,key[3]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
MODE0_LWRf,3);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,MODE1_LWRf,3)
;soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,MODE0_UPRf,3);
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,MODE1_UPRf,3);l141(unit,(
void*)lpm_entry,len);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VALID0_LWRf,1);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VALID1_LWRf,
1);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VALID0_UPRf,1);
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VALID1_UPRf,1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,(1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,(1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,(1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,(1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,(
1<<soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,(
1<<soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_LWRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,(
1<<soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,(
1<<soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_UPRf))-1);
return(SOC_E_NONE);}static int l192(int l1,void*key_data,int bucket_index,int
tcam_index,int l127){alpm_pivot_t*pivot_pyld;defip_alpm_ipv6_128_entry_t l173
,l193,l174;defip_aux_scratch_entry_t l9;uint32 l11[SOC_MAX_MEM_FIELD_WORDS];
soc_mem_t l18;void*l184,*l194,*l186 = NULL;int l149;int l6;int l134 = 
SOC_E_NONE,l172 = SOC_E_NONE;uint32 l195[5],prefix[5];int l28,l22;uint32 l33;
int l196;uint32 l7,l150;int l145,l20 = 0;trie_t*trie,*l31;uint32 l197;
defip_pair_128_entry_t lpm_entry,*l198;payload_t*l190 = NULL,*l199 = NULL,*
l182 = NULL;trie_node_t*l169 = NULL,*l147 = NULL;trie_t*l146 = NULL;l6 = l28 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data
,&l149,&l22));if(l149!= SOC_L3_VRF_OVERRIDE){if(l22 == SOC_VRF_MAX(l1)+1){l7 = 
0;if(soc_th_get_alpm_banks(l1)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l150);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l150);}}else{l7 = 2;if(
soc_th_get_alpm_banks(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,
l150);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l150);}}l134 = l143(l1,key_data,
prefix,&l33,&l20);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"_soc_alpm_128_delete: prefix create failed\n")));return l134;}if(!
soc_th_alpm_mode_get(l1)){if(l149!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT
(l1,l22,l28)>1){if(l20){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot v6-128 delete ""default route if other routes are present "
"in this mode"),l22));return SOC_E_PARAM;}}}l7 = 2;}l18 = 
L3_DEFIP_ALPM_IPV6_128m;l184 = ((uint32*)&(l173));SOC_ALPM_LPM_LOCK(l1);if(
bucket_index == 0){l134 = l148(l1,key_data,l18,l184,&tcam_index,&bucket_index
,&l127);}else{l134 = l15(l1,key_data,l184,0,l18,0,0);}sal_memcpy(&l193,l184,
sizeof(l193));l194 = &l193;if(SOC_FAILURE(l134)){SOC_ALPM_LPM_UNLOCK(l1);
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to find ""prefix for delete\n")));return l134;}
l196 = bucket_index;pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l134 = trie_delete(trie,prefix,l33,&l169);l190 = 
(payload_t*)l169;if(l134!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l134;}l31 = VRF_PREFIX_TRIE_IPV6_128(l1,l22);
l146 = VRF_PIVOT_TRIE_IPV6_128(l1,l22);if(!l20){l134 = trie_delete(l31,prefix
,l33,&l169);l199 = (payload_t*)l169;if(SOC_FAILURE(l134)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l200;}l147 = NULL;l134 = trie_find_lpm(l31,prefix,l33,&l147)
;l182 = (payload_t*)l147;if(SOC_SUCCESS(l134)){payload_t*l201 = (payload_t*)(
l182->bkt_ptr);if(l201!= NULL){l197 = l201->len;}else{l197 = 0;}}else{
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l202;}sal_memcpy(l195,prefix,sizeof(
prefix));l137((l195),(l33),(l28));l134 = l24(l1,l195,l197,l22,l6,&lpm_entry,0
,1);(void)l21(l1,l184,l18,l6,l149,bucket_index,0,&lpm_entry);(void)l24(l1,
l195,l33,l22,l6,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(
l134)){l186 = ((uint32*)&(l174));l172 = soc_mem_read(l1,l18,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l127),l186);}}if((l197 == 0)&&SOC_FAILURE(l172)){
l198 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22);sal_memcpy(&lpm_entry,l198,
sizeof(lpm_entry));l134 = l24(l1,prefix,l197,l22,l6,&lpm_entry,0,1);}if(
SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l202;}l198 = &lpm_entry;}else{l147 = 
NULL;l134 = trie_find_lpm(l31,prefix,l33,&l147);l182 = (payload_t*)l147;if(
SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),l22));goto l200;}l182->bkt_ptr = 0;
l197 = 0;l198 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22);}l134 = l5(l1,l198,
l28,l7,l197,&l9);if(SOC_FAILURE(l134)){goto l202;}l134 = _soc_th_alpm_aux_op(
l1,DELETE_PROPAGATE,&l9,TRUE,&l145,&tcam_index,&bucket_index);if(SOC_FAILURE(
l134)){goto l202;}if(SOC_URPF_STATUS_GET(l1)){uint32 l140;if(l186!= NULL){
l140 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l9,DB_TYPEf);l140++;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,DB_TYPEf,l140);l140 = 
soc_mem_field32_get(l1,l18,l186,SRC_DISCARDf);soc_mem_field32_set(l1,l18,&l9,
SRC_DISCARDf,l140);l140 = soc_mem_field32_get(l1,l18,l186,DEFAULTROUTEf);
soc_mem_field32_set(l1,l18,&l9,DEFAULTROUTEf,l140);l134 = _soc_th_alpm_aux_op
(l1,DELETE_PROPAGATE,&l9,TRUE,&l145,&tcam_index,&bucket_index);}if(
SOC_FAILURE(l134)){goto l202;}}sal_free(l190);if(!l20){sal_free(l199);}
PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);if((pivot_pyld->len!= 0)&&(trie->trie
== NULL)){uint32 l203[5];sal_memcpy(l203,pivot_pyld->key,sizeof(l203));l137((
l203),(pivot_pyld->len),(l6));l24(l1,l203,pivot_pyld->len,l22,l6,&lpm_entry,0
,1);l134 = l4(l1,&lpm_entry);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),pivot_pyld->key[1],pivot_pyld
->key[2],pivot_pyld->key[3],pivot_pyld->key[4]));}}l134 = 
_soc_th_alpm_delete_in_bkt(l1,l18,l196,l150,l194,l11,&l127,l28);if(!
SOC_SUCCESS(l134)){SOC_ALPM_LPM_UNLOCK(l1);l134 = SOC_E_FAIL;return l134;}if(
SOC_URPF_STATUS_GET(l1)){l134 = soc_mem_alpm_delete(l1,l18,
SOC_ALPM_RPF_BKT_IDX(l1,l196),MEM_BLOCK_ALL,l150,l194,l11,&l145);if(!
SOC_SUCCESS(l134)){SOC_ALPM_LPM_UNLOCK(l1);l134 = SOC_E_FAIL;return l134;}}if
((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l134 = alpm_bucket_release(l1,
PIVOT_BUCKET_INDEX(pivot_pyld),l28);if(SOC_FAILURE(l134)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to release"
"empty bucket: %d\n"),PIVOT_BUCKET_INDEX(pivot_pyld)));}l134 = trie_delete(
l146,pivot_pyld->key,pivot_pyld->len,&l169);if(SOC_FAILURE(l134)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"could not delete pivot from pivot trie\n")));
}trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(
pivot_pyld));sal_free(pivot_pyld);_soc_trident2_alpm_bkt_view_set(l1,l196<<2,
INVALIDm);if(SOC_ALPM_V6_SCALE_CHECK(l1,l28)){_soc_trident2_alpm_bkt_view_set
(l1,(l196+1)<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l22,l28);if(
VRF_TRIE_ROUTES_CNT(l1,l22,l28) == 0){l134 = l27(l1,l22,l28);}
SOC_ALPM_LPM_UNLOCK(l1);return l134;l202:l172 = trie_insert(l31,prefix,NULL,
l33,(trie_node_t*)l199);if(SOC_FAILURE(l172)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l200:l172 = trie_insert(trie,prefix,NULL,l33,(
trie_node_t*)l190);if(SOC_FAILURE(l172)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}SOC_ALPM_LPM_UNLOCK(l1);return l134;}int
soc_th_alpm_128_init(int l1){int l134 = SOC_E_NONE;l134 = 
soc_th_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l134);return l134;}int
soc_th_alpm_128_state_clear(int l1){int l133,l134;for(l133 = 0;l133<= 
SOC_VRF_MAX(l1)+1;l133++){l134 = trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,
l133),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l134)){
trie_destroy(VRF_PREFIX_TRIE_IPV6_128(l1,l133));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n"),l1,l133));return
l134;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l133)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l133));}}return SOC_E_NONE;}int
soc_th_alpm_128_deinit(int l1){soc_th_alpm_128_lpm_deinit(l1);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_state_clear(l1));return SOC_E_NONE;}
static int l204(int l1,int l22,int l28){defip_pair_128_entry_t*lpm_entry,l205
;int l206;int index;int l134 = SOC_E_NONE;uint32 key[5] = {0,0,0,0,0};uint32
l33;alpm_bucket_handle_t*l185;alpm_pivot_t*pivot_pyld;payload_t*l199;trie_t*
l207;trie_t*l208 = NULL;trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(
l1,l22));l208 = VRF_PIVOT_TRIE_IPV6_128(l1,l22);trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6_128(l1,l22));l207 = VRF_PREFIX_TRIE_IPV6_128(l1,l22);
lpm_entry = sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(
lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: unable to allocate memory "
"for IPv6-128 LPM entry\n")));return SOC_E_MEMORY;}l24(l1,key,0,l22,l28,
lpm_entry,0,1);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22) = lpm_entry;if(l22 == 
SOC_VRF_MAX(l1)+1){soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,
DEFAULT_MISSf,1);}l134 = alpm_bucket_assign(l1,&l206,l28);
soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,ALG_BKT_PTRf,l206);sal_memcpy
(&l205,lpm_entry,sizeof(l205));l134 = l2(l1,&l205,&index);l185 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l185 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: Unable to allocate memory ""for bucket handle \n"))
);return SOC_E_NONE;}sal_memset(l185,0,sizeof(*l185));pivot_pyld = sal_alloc(
sizeof(alpm_pivot_t),"Payload for Pivot");if(pivot_pyld == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: Unable to allocate memory ""for PIVOT trie node \n"
)));sal_free(l185);return SOC_E_MEMORY;}l199 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l199 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_th_alpm_128_vrf_add: Unable to allocate memory "
"for pfx trie node \n")));sal_free(l185);sal_free(pivot_pyld);return
SOC_E_MEMORY;}sal_memset(pivot_pyld,0,sizeof(*pivot_pyld));sal_memset(l199,0,
sizeof(*l199));l33 = 0;PIVOT_BUCKET_HANDLE(pivot_pyld) = l185;trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(pivot_pyld));PIVOT_BUCKET_INDEX(
pivot_pyld) = l206;PIVOT_BUCKET_VRF(pivot_pyld) = l22;PIVOT_BUCKET_IPV6(
pivot_pyld) = l28;PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;pivot_pyld->key[0] = 
l199->key[0] = key[0];pivot_pyld->key[1] = l199->key[1] = key[1];pivot_pyld->
key[2] = l199->key[2] = key[2];pivot_pyld->key[3] = l199->key[3] = key[3];
pivot_pyld->key[4] = l199->key[4] = key[4];pivot_pyld->len = l199->len = l33;
l134 = trie_insert(l207,key,NULL,l33,&(l199->node));if(SOC_FAILURE(l134)){
sal_free(l199);sal_free(pivot_pyld);sal_free(l185);return l134;}l134 = 
trie_insert(l208,key,NULL,l33,(trie_node_t*)pivot_pyld);if(SOC_FAILURE(l134))
{trie_node_t*l169 = NULL;(void)trie_delete(l207,key,l33,&l169);sal_free(l199)
;sal_free(pivot_pyld);sal_free(l185);return l134;}index = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,l28);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(index)<<1) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = 
SOC_ALPM_128_ADDR_LWR(index)<<1;VRF_PIVOT_REF_INC(l1,l22,l28);
VRF_TRIE_INIT_DONE(l1,l22,l28,1);return l134;}static int l27(int l1,int l22,
int l28){defip_pair_128_entry_t*lpm_entry;int l206;int l209;int l134 = 
SOC_E_NONE;uint32 key[2] = {0,0},l138[SOC_MAX_MEM_FIELD_WORDS];payload_t*l190
;alpm_pivot_t*l210;trie_node_t*l169;trie_t*l207;trie_t*l208 = NULL;lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22);l206 = 
soc_L3_DEFIP_PAIR_128m_field32_get(l1,lpm_entry,ALG_BKT_PTRf);l134 = 
alpm_bucket_release(l1,l206,l28);_soc_trident2_alpm_bkt_view_set(l1,l206<<2,
INVALIDm);if(SOC_ALPM_V6_SCALE_CHECK(l1,l28)){_soc_trident2_alpm_bkt_view_set
(l1,(l206+1)<<2,INVALIDm);}l134 = l14(l1,lpm_entry,(void*)l138,&l209);if(
SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n")
,l22,l28));l209 = -1;}l209 = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,
l209,l28);l210 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l209)<<1);l134 = l4
(l1,lpm_entry);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1
,"soc_th_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),l22,l28));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22) = NULL;l207 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l22);VRF_PREFIX_TRIE_IPV6_128(l1,l22) = NULL;
VRF_TRIE_INIT_DONE(l1,l22,l28,0);l134 = trie_delete(l207,key,0,&l169);l190 = 
(payload_t*)l169;if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),l22,l28));}
sal_free(l190);(void)trie_destroy(l207);l208 = VRF_PIVOT_TRIE_IPV6_128(l1,l22
);VRF_PIVOT_TRIE_IPV6_128(l1,l22) = NULL;l169 = NULL;l134 = trie_delete(l208,
key,0,&l169);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l22,l28));}(void)
trie_destroy(l208);sal_free(PIVOT_BUCKET_HANDLE(l210));(void)trie_destroy(
PIVOT_BUCKET_TRIE(l210));sal_free(l210);return l134;}int
soc_th_alpm_128_insert(int l1,void*l3,uint32 l19,int l211,int l212){
defip_alpm_ipv6_128_entry_t l173,l174;soc_mem_t l18;void*l184,*l194;int l149,
l22;int index;int l6;int l134 = SOC_E_NONE;uint32 l20;l6 = L3_DEFIP_MODE_128;
l18 = L3_DEFIP_ALPM_IPV6_128m;l184 = ((uint32*)&(l173));l194 = ((uint32*)&(
l174));SOC_IF_ERROR_RETURN(l15(l1,l3,l184,l194,l18,l19,&l20));
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l3,&l149,&l22));if(((l149
== SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l149 == SOC_L3_VRF_GLOBAL)))){l134 = l2(l1,l3,&
index);if(SOC_SUCCESS(l134)){VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l6);
VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l6);}else if(l134 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID,l6);}return(l134);}else if(l22 == 0){if(
soc_th_alpm_mode_get(l1)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}}if(l149!= 
SOC_L3_VRF_GLOBAL){if(!soc_th_alpm_mode_get(l1)){if(VRF_TRIE_ROUTES_CNT(l1,
l22,l6) == 0){if(!l20){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
l149));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l22,l6)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d is not ""initialized\n"),l22));l134 = l204(l1,
l22,l6);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),l22,l6));return
l134;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init ""completed\n"),l22,l6));}if(l212
&SOC_ALPM_LOOKUP_HIT){l134 = l151(l1,l3,l184,l194,l18,l211);}else{if(l211 == 
-1){l211 = 0;}l134 = l188(l1,l3,l18,l184,l194,&index,
SOC_ALPM_BKT_ENTRY_TO_IDX(l211),l212);}if(l134!= SOC_E_NONE){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d :soc_th_alpm_128_insert: "
"Route Insertion Failed :%s\n"),l1,soc_errmsg(l134)));}return(l134);}int
soc_th_alpm_128_lookup(int l1,void*key_data,void*l11,int*l12,int*l213){
defip_alpm_ipv6_128_entry_t l173;soc_mem_t l18;int bucket_index;int tcam_index
;void*l184;int l149,l22;int l6 = 2,l126;int l134 = SOC_E_NONE;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l149,&l22));l134
= l10(l1,key_data,l11,l12,&l126,&l6);if(SOC_SUCCESS(l134)){
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l11,&l149,&l22));if(l149 == 
SOC_L3_VRF_OVERRIDE){return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l22,
l6)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_lookup:VRF %d is not ""initialized\n"),l22));*l12 = 0;*l213 = 0;
return SOC_E_NOT_FOUND;}l18 = L3_DEFIP_ALPM_IPV6_128m;l184 = ((uint32*)&(l173
));SOC_ALPM_LPM_LOCK(l1);l134 = l148(l1,key_data,l18,l184,&tcam_index,&
bucket_index,l12);SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l134)){*l213 = 
tcam_index;*l12 = bucket_index<<2;return l134;}l134 = l21(l1,l184,l18,l6,l149
,bucket_index,*l12,l11);*l213 = SOC_ALPM_LOOKUP_HIT|tcam_index;return(l134);}
int soc_th_alpm_128_delete(int l1,void*key_data,int l211,int l212){int l149,
l22;int l6;int l134 = SOC_E_NONE;l6 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l149,&l22));if(l149 == 
SOC_L3_VRF_OVERRIDE){l134 = l4(l1,key_data);if(SOC_SUCCESS(l134)){
VRF_TRIE_ROUTES_DEC(l1,MAX_VRF_ID,l6);VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l6);}
return(l134);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l22,l6)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_th_alpm_128_delete:VRF %d/%d is not "
"initialized\n"),l22,l6));return SOC_E_NONE;}if(l211 == -1){l211 = 0;}l134 = 
l192(l1,key_data,SOC_ALPM_BKT_ENTRY_TO_IDX(l211),l212&~SOC_ALPM_LOOKUP_HIT,
l211);}return(l134);}static void l114(int l1,void*l11,int index,l109 l115){
l115[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l11),(l102[(l1)]->l65));l115[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l102[(l1)]->l63));l115[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11),(l102[(l1)]->l69));l115[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l102[(l1)]->l67));l115[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11),(l102[(l1)]->l66));l115[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l102[(l1)]->l64));l115[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11),(l102[(l1)]->l70));l115[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l102[(l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l102[(l1)]->l88)!= NULL
))){int l214;(void)soc_th_alpm_128_lpm_vrf_get(l1,l11,(int*)&l115[8],&l214);}
else{l115[8] = 0;};}static int l215(l109 l111,l109 l112){int l209;for(l209 = 
0;l209<9;l209++){{if((l111[l209])<(l112[l209])){return-1;}if((l111[l209])>(
l112[l209])){return 1;}};}return(0);}static void l216(int l1,void*l3,uint32
l217,uint32 l129,int l126){l109 l218;if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l102[(l1)]->l85))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l102[(l1)]->l84))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l102[(l1)]->l83))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l102[(l1)]->l82))){l218[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l102[(l1)]->l65));l218[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l102[(l1)]->l63));l218[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l102[(l1)]->l69));l218[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l102[(l1)]->l67));l218[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l102[(l1)]->l66));l218[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l102[(l1)]->l64));l218[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l102[(l1)]->l70));l218[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l102[(l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l102[(l1)]->l88)!= NULL)
)){int l214;(void)soc_th_alpm_128_lpm_vrf_get(l1,l3,(int*)&l218[8],&l214);}
else{l218[8] = 0;};l128((l113[(l1)]),l215,l218,l126,l129,l217);}}static void
l219(int l1,void*key_data,uint32 l217){l109 l218;int l126 = -1;int l134;
uint16 index;l218[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l102[(l1)]->l65));l218[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l102[(l1)]->l63));l218[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l102[(l1)]->l69));l218[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l102[(l1)]->l67));l218[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l102[(l1)]->l66));l218[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l102[(l1)]->l64));l218[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l102[(l1)]->l70));l218[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l102[(l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l102[(l1)]->l88
)!= NULL))){int l214;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l218[8],&l214);}else{l218[8] = 0;};index = l217;l134 = l130((l113[(l1)]),l215
,l218,l126,index);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"\ndel  index: H %d error %d\n"),index,l134));}}static int l220(int l1,
void*key_data,int l126,int*l127){l109 l218;int l134;uint16 index = (0xFFFF);
l218[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l102[(l1)]->l65));l218[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l102[(l1)]->l63));l218[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l102[(l1)]->l69));l218[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l102[(l1)]->l67));l218[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l102[(l1)]->l66));l218[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l102[(l1)]->l64));l218[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l102[(l1)]->l70));l218[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l102[(l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l102[(l1)]->l88
)!= NULL))){int l214;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l218[8],&l214);}else{l218[8] = 0;};l134 = l123((l113[(l1)]),l215,l218,l126,&
index);if(SOC_FAILURE(l134)){*l127 = 0xFFFFFFFF;return(l134);}*l127 = index;
return(SOC_E_NONE);}static uint16 l116(uint8*l117,int l118){return(
_shr_crc16b(0,l117,l118));}static int l119(int unit,int l104,int l105,l108**
l120){l108*l124;int index;if(l105>l104){return SOC_E_MEMORY;}l124 = sal_alloc
(sizeof(l108),"lpm_hash");if(l124 == NULL){return SOC_E_MEMORY;}sal_memset(
l124,0,sizeof(*l124));l124->unit = unit;l124->l104 = l104;l124->l105 = l105;
l124->l106 = sal_alloc(l124->l105*sizeof(*(l124->l106)),"hash_table");if(l124
->l106 == NULL){sal_free(l124);return SOC_E_MEMORY;}l124->l107 = sal_alloc(
l124->l104*sizeof(*(l124->l107)),"link_table");if(l124->l107 == NULL){
sal_free(l124->l106);sal_free(l124);return SOC_E_MEMORY;}for(index = 0;index<
l124->l105;index++){l124->l106[index] = (0xFFFF);}for(index = 0;index<l124->
l104;index++){l124->l107[index] = (0xFFFF);}*l120 = l124;return SOC_E_NONE;}
static int l121(l108*l122){if(l122!= NULL){sal_free(l122->l106);sal_free(l122
->l107);sal_free(l122);}return SOC_E_NONE;}static int l123(l108*l124,l110 l125
,l109 entry,int l126,uint16*l127){int l1 = l124->unit;uint16 l221;uint16 index
;l221 = l116((uint8*)entry,(32*9))%l124->l105;index = l124->l106[l221];;;
while(index!= (0xFFFF)){uint32 l11[SOC_MAX_MEM_FIELD_WORDS];l109 l115;int l222
;l222 = index;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,
l222,l11));l114(l1,l11,index,l115);if((*l125)(entry,l115) == 0){*l127 = index
;;return(SOC_E_NONE);}index = l124->l107[index&(0x3FFF)];;};return(
SOC_E_NOT_FOUND);}static int l128(l108*l124,l110 l125,l109 entry,int l126,
uint16 l129,uint16 l39){int l1 = l124->unit;uint16 l221;uint16 index;uint16
l223;l221 = l116((uint8*)entry,(32*9))%l124->l105;index = l124->l106[l221];;;
;l223 = (0xFFFF);if(l129!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l11[
SOC_MAX_MEM_FIELD_WORDS];l109 l115;int l222;l222 = index;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l222,l11));l114(l1,l11,index,l115);
if((*l125)(entry,l115) == 0){if(l39!= index){;if(l223 == (0xFFFF)){l124->l106
[l221] = l39;l124->l107[l39&(0x3FFF)] = l124->l107[index&(0x3FFF)];l124->l107
[index&(0x3FFF)] = (0xFFFF);}else{l124->l107[l223&(0x3FFF)] = l39;l124->l107[
l39&(0x3FFF)] = l124->l107[index&(0x3FFF)];l124->l107[index&(0x3FFF)] = (
0xFFFF);}};return(SOC_E_NONE);}l223 = index;index = l124->l107[index&(0x3FFF)
];;}}l124->l107[l39&(0x3FFF)] = l124->l106[l221];l124->l106[l221] = l39;
return(SOC_E_NONE);}static int l130(l108*l124,l110 l125,l109 entry,int l126,
uint16 l131){uint16 l221;uint16 index;uint16 l223;l221 = l116((uint8*)entry,(
32*9))%l124->l105;index = l124->l106[l221];;;l223 = (0xFFFF);while(index!= (
0xFFFF)){if(l131 == index){;if(l223 == (0xFFFF)){l124->l106[l221] = l124->
l107[l131&(0x3FFF)];l124->l107[l131&(0x3FFF)] = (0xFFFF);}else{l124->l107[
l223&(0x3FFF)] = l124->l107[l131&(0x3FFF)];l124->l107[l131&(0x3FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l223 = index;index = l124->l107[index&(0x3FFF)];
;}return(SOC_E_NOT_FOUND);}static int l224(int l1,void*l11){return(SOC_E_NONE
);}void soc_th_alpm_128_lpm_state_dump(int l1){int l133;int l225;l225 = ((3*(
128+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){
return;}for(l133 = l225;l133>= 0;l133--){if((l133!= ((3*(128+2+1))-1))&&((l54
[(l1)][(l133)].l47) == -1)){continue;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"PFX = %d P = %d N = %d START = %d ""END = %d VENT = %d FENT = %d\n"),l133
,(l54[(l1)][(l133)].l49),(l54[(l1)][(l133)].next),(l54[(l1)][(l133)].l47),(
l54[(l1)][(l133)].l48),(l54[(l1)][(l133)].l50),(l54[(l1)][(l133)].l51)));}
COMPILER_REFERENCE(l224);}static int l226(int l1,int index,uint32*l11){int
l227;uint32 l228,l229,l230;uint32 l231;int l232;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l227 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}else if(SOC_IS_APOLLO(l1)){
l227 = (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l227 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}if(((l102[(l1)]->l57)!= NULL)
){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
l11),(l102[(l1)]->l57),(0));}l228 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11),(l102[(l1)]->l69));l231 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l102[(l1)]->l70));l229 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l11),(l102[(l1)]->l67));l230 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l102[(l1)]->l68));l232 = ((!l228)&&(!l231)&&(!l229)&&(!l230))?1:0;l228 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l102[(l1)]->l84));l231 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l11),(l102[(l1)]->l82));l229 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l102[(l1)]->l83));l230 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l11),(l102[(l1)]->l83));if(l228&&l231&&l229&&l230){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l102[(l1)]->l81),(l232));}return l166(l1,MEM_BLOCK_ANY,index+l227,index,
l11);}static int l233(int l1,int l234,int l235){uint32 l11[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l234,l11));l216(l1,l11,l235,0x4000,0);SOC_IF_ERROR_RETURN(l166(
l1,MEM_BLOCK_ANY,l235,l234,l11));SOC_IF_ERROR_RETURN(l226(l1,l235,l11));do{
int l236,l237;l236 = soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l234),
1);l237 = soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l235),1);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l237))<<1) = ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l236))<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR
((l237))<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l237
))<<1)) = SOC_ALPM_128_ADDR_LWR((l237))<<1;}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l236))<<1) = NULL;}while(0);return(SOC_E_NONE);}static
int l238(int l1,int l126,int l6){int l234;int l235;l235 = (l54[(l1)][(l126)].
l48)+1;l234 = (l54[(l1)][(l126)].l47);if(l234!= l235){SOC_IF_ERROR_RETURN(
l233(l1,l234,l235));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l6);}(l54[(l1)][(l126)]
.l47)+= 1;(l54[(l1)][(l126)].l48)+= 1;return(SOC_E_NONE);}static int l239(int
l1,int l126,int l6){int l234;int l235;l235 = (l54[(l1)][(l126)].l47)-1;if((
l54[(l1)][(l126)].l50) == 0){(l54[(l1)][(l126)].l47) = l235;(l54[(l1)][(l126)
].l48) = l235-1;return(SOC_E_NONE);}l234 = (l54[(l1)][(l126)].l48);
SOC_IF_ERROR_RETURN(l233(l1,l234,l235));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l6)
;(l54[(l1)][(l126)].l47)-= 1;(l54[(l1)][(l126)].l48)-= 1;return(SOC_E_NONE);}
static int l240(int l1,int l126,int l6,void*l11,int*l241){int l242;int l243;
int l244;int l245;if((l54[(l1)][(l126)].l50) == 0){l245 = ((3*(128+2+1))-1);
if((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL)||(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)){if(l126<= (((3*(128+2+1
))/3)-1)){l245 = (((3*(128+2+1))/3)-1);}}while((l54[(l1)][(l245)].next)>l126)
{l245 = (l54[(l1)][(l245)].next);}l243 = (l54[(l1)][(l245)].next);if(l243!= -
1){(l54[(l1)][(l243)].l49) = l126;}(l54[(l1)][(l126)].next) = (l54[(l1)][(
l245)].next);(l54[(l1)][(l126)].l49) = l245;(l54[(l1)][(l245)].next) = l126;(
l54[(l1)][(l126)].l51) = ((l54[(l1)][(l245)].l51)+1)/2;(l54[(l1)][(l245)].l51
)-= (l54[(l1)][(l126)].l51);(l54[(l1)][(l126)].l47) = (l54[(l1)][(l245)].l48)
+(l54[(l1)][(l245)].l51)+1;(l54[(l1)][(l126)].l48) = (l54[(l1)][(l126)].l47)-
1;(l54[(l1)][(l126)].l50) = 0;}l244 = l126;while((l54[(l1)][(l244)].l51) == 0
){l244 = (l54[(l1)][(l244)].next);if(l244 == -1){l244 = l126;break;}}while((
l54[(l1)][(l244)].l51) == 0){l244 = (l54[(l1)][(l244)].l49);if(l244 == -1){if
((l54[(l1)][(l126)].l50) == 0){l242 = (l54[(l1)][(l126)].l49);l243 = (l54[(l1
)][(l126)].next);if(-1!= l242){(l54[(l1)][(l242)].next) = l243;}if(-1!= l243)
{(l54[(l1)][(l243)].l49) = l242;}}return(SOC_E_FULL);}}while(l244>l126){l243 = 
(l54[(l1)][(l244)].next);SOC_IF_ERROR_RETURN(l239(l1,l243,l6));(l54[(l1)][(
l244)].l51)-= 1;(l54[(l1)][(l243)].l51)+= 1;l244 = l243;}while(l244<l126){
SOC_IF_ERROR_RETURN(l238(l1,l244,l6));(l54[(l1)][(l244)].l51)-= 1;l242 = (l54
[(l1)][(l244)].l49);(l54[(l1)][(l242)].l51)+= 1;l244 = l242;}(l54[(l1)][(l126
)].l50)+= 1;(l54[(l1)][(l126)].l51)-= 1;(l54[(l1)][(l126)].l48)+= 1;*l241 = (
l54[(l1)][(l126)].l48);sal_memcpy(l11,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);return(
SOC_E_NONE);}static int l246(int l1,int l126,int l6,void*l11,int l247){int
l242;int l243;int l234;int l235;uint32 l248[SOC_MAX_MEM_FIELD_WORDS];int l134
;int l140;l234 = (l54[(l1)][(l126)].l48);l235 = l247;(l54[(l1)][(l126)].l50)
-= 1;(l54[(l1)][(l126)].l51)+= 1;(l54[(l1)][(l126)].l48)-= 1;if(l235!= l234){
if((l134 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l234,l248))<0){return l134
;}l216(l1,l248,l235,0x4000,0);if((l134 = l166(l1,MEM_BLOCK_ANY,l235,l234,l248
))<0){return l134;}if((l134 = l226(l1,l235,l248))<0){return l134;}}l140 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l235,1);l247 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l234,1);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l140)<<1) = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
l247)<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l140)<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l140)<<1)) = 
SOC_ALPM_128_ADDR_LWR(l140)<<1;}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l247
)<<1) = NULL;sal_memcpy(l248,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),
soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);l216(l1,l248,l234,0x4000,0);if(
(l134 = l166(l1,MEM_BLOCK_ANY,l234,l234,l248))<0){return l134;}if((l134 = 
l226(l1,l234,l248))<0){return l134;}if((l54[(l1)][(l126)].l50) == 0){l242 = (
l54[(l1)][(l126)].l49);assert(l242!= -1);l243 = (l54[(l1)][(l126)].next);(l54
[(l1)][(l242)].next) = l243;(l54[(l1)][(l242)].l51)+= (l54[(l1)][(l126)].l51)
;(l54[(l1)][(l126)].l51) = 0;if(l243!= -1){(l54[(l1)][(l243)].l49) = l242;}(
l54[(l1)][(l126)].next) = -1;(l54[(l1)][(l126)].l49) = -1;(l54[(l1)][(l126)].
l47) = -1;(l54[(l1)][(l126)].l48) = -1;}return(l134);}int
soc_th_alpm_128_lpm_vrf_get(int unit,void*lpm_entry,int*l22,int*l249){int l149
;if(((l102[(unit)]->l92)!= NULL)){l149 = soc_L3_DEFIP_PAIR_128m_field32_get(
unit,lpm_entry,VRF_ID_0_LWRf);*l249 = l149;if(
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,VRF_ID_MASK0_LWRf)){*l22 = 
l149;}else if(!soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l102[(unit)]->l94))){*l22 = 
SOC_L3_VRF_GLOBAL;*l249 = SOC_VRF_MAX(unit)+1;}else{*l22 = 
SOC_L3_VRF_OVERRIDE;}}else{*l22 = SOC_L3_VRF_DEFAULT;}return(SOC_E_NONE);}
static int l250(int l1,void*entry,int*l13){int l126=0;int l134;int l149;int
l251;l134 = l132(l1,entry,&l126);if(l134<0){return l134;}l126+= 0;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,entry,&l149,&l134));l251 = 
soc_th_alpm_mode_get(l1);switch(l149){case SOC_L3_VRF_GLOBAL:if((l251 == 
SOC_ALPM_MODE_PARALLEL)||(l251 == SOC_ALPM_MODE_TCAM_ALPM)){*l13 = l126+((3*(
128+2+1))/3);}else{*l13 = l126;}break;case SOC_L3_VRF_OVERRIDE:*l13 = l126+2*
((3*(128+2+1))/3);break;default:if((l251 == SOC_ALPM_MODE_PARALLEL)||(l251 == 
SOC_ALPM_MODE_TCAM_ALPM)){*l13 = l126;}else{*l13 = l126+((3*(128+2+1))/3);}
break;}return(SOC_E_NONE);}static int l10(int l1,void*key_data,void*l11,int*
l12,int*l13,int*l6){int l134;int l127;int l126 = 0;*l6 = L3_DEFIP_MODE_128;
l250(l1,key_data,&l126);*l13 = l126;if(l220(l1,key_data,l126,&l127) == 
SOC_E_NONE){*l12 = l127;if((l134 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(
*l6)?*l12:(*l12>>1),l11))<0){return l134;}return(SOC_E_NONE);}else{return(
SOC_E_NOT_FOUND);}}int soc_th_alpm_128_lpm_init(int l1){int l225;int l133;int
l252;int l253;if(!soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL)
;}l225 = (3*(128+2+1));l253 = sizeof(l52)*(l225);if((l54[(l1)]!= NULL)){
SOC_IF_ERROR_RETURN(soc_th_alpm_128_deinit(l1));}l102[l1] = sal_alloc(sizeof(
l100),"lpm_128_field_state");if(NULL == l102[l1]){return(SOC_E_MEMORY);}(l102
[l1])->l56 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l102[l1]
)->l57 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l102[l1])
->l58 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l102[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l102[l1])->l60 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l102[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l102[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l102[l1])->l65 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l102[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l102[l1])->l66 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l102[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l102[l1])->l69 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l102[l1])->
l67 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l102[
l1])->l70 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l102[l1])->l68 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l102[l1])->l73 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l102[l1])->l71 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l102[l1])->l74 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l102[l1])->l72 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l102[l1])->l77 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l102[l1])->l75 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l102[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l102[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l102[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l102[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l102[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l102[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l102[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l102[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l102[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l102[l1])->l88 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l102[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l102[l1])->l89 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l102[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l102[l1])->l92 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l102[l1])->
l90 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l102[l1
])->l93 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(
l102[l1])->l91 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
VRF_ID_MASK1_UPRf);(l102[l1])->l94 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l102[l1])->l95 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l102[l1])->l96 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l102[l1])->l97 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l102[l1])->l98 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf);(l102[l1])->l99 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_POOL_NUMBERf);(l54[(l1)]
) = sal_alloc(l253,"LPM 128 prefix info");if(NULL == (l54[(l1)])){sal_free(
l102[l1]);l102[l1] = NULL;return(SOC_E_MEMORY);}SOC_ALPM_LPM_LOCK(l1);
sal_memset((l54[(l1)]),0,l253);for(l133 = 0;l133<l225;l133++){(l54[(l1)][(
l133)].l47) = -1;(l54[(l1)][(l133)].l48) = -1;(l54[(l1)][(l133)].l49) = -1;(
l54[(l1)][(l133)].next) = -1;(l54[(l1)][(l133)].l50) = 0;(l54[(l1)][(l133)].
l51) = 0;}l252 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l1)){l252>>= 1;}if((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM
)){(l54[(l1)][(((3*(128+2+1))-1))].l48) = (l252>>1)-1;(l54[(l1)][(((((3*(128+
2+1))/3)-1)))].l51) = l252>>1;(l54[(l1)][((((3*(128+2+1))-1)))].l51) = (l252-
(l54[(l1)][(((((3*(128+2+1))/3)-1)))].l51));}else{(l54[(l1)][((((3*(128+2+1))
-1)))].l51) = l252;}if((l113[(l1)])!= NULL){if(l121((l113[(l1)]))<0){
SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(l113[(l1)]) = NULL;}if(l119(
l1,l252*2,l252,&(l113[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;
}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}int soc_th_alpm_128_lpm_deinit(
int l1){if(!soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}
SOC_ALPM_LPM_LOCK(l1);if((l113[(l1)])!= NULL){l121((l113[(l1)]));(l113[(l1)])
= NULL;}if((l54[(l1)]!= NULL)){sal_free(l102[l1]);l102[l1] = NULL;sal_free((
l54[(l1)]));(l54[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}
static int l2(int l1,void*l3,int*l254){int l126;int index;int l6;uint32 l11[
SOC_MAX_MEM_FIELD_WORDS];int l134 = SOC_E_NONE;int l255 = 0;sal_memcpy(l11,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);SOC_ALPM_LPM_LOCK(l1);l134 = l10(l1,l3,l11,&index,&
l126,&l6);if(l134 == SOC_E_NOT_FOUND){l134 = l240(l1,l126,l6,l11,&index);if(
l134<0){SOC_ALPM_LPM_UNLOCK(l1);return(l134);}}else{l255 = 1;}*l254 = index;
if(l134 == SOC_E_NONE){soc_th_alpm_128_lpm_state_dump(l1);LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_th_alpm_128_lpm_insert: %d %d\n"),index
,l126));if(!l255){l216(l1,l3,index,0x4000,0);}l134 = l166(l1,MEM_BLOCK_ANY,
index,index,l3);if(l134>= 0){l134 = l226(l1,index,l3);}}SOC_ALPM_LPM_UNLOCK(
l1);return(l134);}static int l4(int l1,void*key_data){int l126;int index;int
l6;uint32 l11[SOC_MAX_MEM_FIELD_WORDS];int l134 = SOC_E_NONE;
SOC_ALPM_LPM_LOCK(l1);l134 = l10(l1,key_data,l11,&index,&l126,&l6);if(l134 == 
SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\nsoc_th_alpm_lpm_delete: %d %d\n"),index,l126));l219(l1,key_data,index);
l134 = l246(l1,l126,l6,l11,index);}soc_th_alpm_128_lpm_state_dump(l1);
SOC_ALPM_LPM_UNLOCK(l1);return(l134);}static int l14(int l1,void*key_data,
void*l11,int*l12){int l126;int l134;int l6;SOC_ALPM_LPM_LOCK(l1);l134 = l10(
l1,key_data,l11,l12,&l126,&l6);SOC_ALPM_LPM_UNLOCK(l1);return(l134);}static
int l5(int unit,void*key_data,int l6,int l7,int l8,defip_aux_scratch_entry_t*
l9){uint32 l136;uint32 l256[4] = {0,0,0,0};int l126 = 0;int l134 = SOC_E_NONE
;l136 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,VALID0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,VALIDf,l136);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,MODE0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,MODEf,l136);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,ENTRY_TYPE0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,ENTRY_TYPEf,0);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,GLOBAL_ROUTEf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,GLOBAL_ROUTEf,l136);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,ECMPf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,ECMPf,l136);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,ECMP_PTRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,ECMP_PTRf,l136);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,NEXT_HOP_INDEXf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,NEXT_HOP_INDEXf,l136);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,PRIf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,PRIf,l136);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,RPEf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,RPEf,l136);l136 =
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,VRF_ID_0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,VRFf,l136);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,DB_TYPEf,l7);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,DST_DISCARDf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,DST_DISCARDf,l136);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,CLASS_IDf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,CLASS_IDf,l136);l256[0] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR0_LWRf);l256[1] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR1_LWRf);l256[2] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR0_UPRf);l256[3] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR1_UPRf);
soc_mem_field_set(unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)l9,IP_ADDRf,(uint32*)
l256);l134 = l132(unit,key_data,&l126);if(SOC_FAILURE(l134)){return l134;}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,IP_LENGTHf,l126);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,REPLACE_LENf,l8);return(
SOC_E_NONE);}static int l15(int unit,void*lpm_entry,void*l16,void*l17,
soc_mem_t l18,uint32 l19,uint32*l257){uint32 l136;uint32 l256[4];int l126 = 0
;int l134 = SOC_E_NONE;uint32 l20 = 0;sal_memset(l16,0,soc_mem_entry_words(
unit,l18)*4);l136 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,
HITf);soc_mem_field32_set(unit,l18,l16,HITf,l136);l136 = soc_mem_field32_get(
unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf);soc_mem_field32_set(unit,l18,
l16,VALIDf,l136);l136 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry
,ECMPf);soc_mem_field32_set(unit,l18,l16,ECMPf,l136);l136 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf);
soc_mem_field32_set(unit,l18,l16,ECMP_PTRf,l136);l136 = soc_mem_field32_get(
unit,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf);soc_mem_field32_set(unit,
l18,l16,NEXT_HOP_INDEXf,l136);l136 = soc_mem_field32_get(unit,
L3_DEFIP_PAIR_128m,lpm_entry,PRIf);soc_mem_field32_set(unit,l18,l16,PRIf,l136
);l136 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,RPEf);
soc_mem_field32_set(unit,l18,l16,RPEf,l136);l136 = soc_mem_field32_get(unit,
L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf);soc_mem_field32_set(unit,l18,l16,
DST_DISCARDf,l136);l136 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,
lpm_entry,SRC_DISCARDf);soc_mem_field32_set(unit,l18,l16,SRC_DISCARDf,l136);
l136 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf);
soc_mem_field32_set(unit,l18,l16,CLASS_IDf,l136);l256[0] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf);l256[1] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf);l256[2] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf);l256[3] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf);
soc_mem_field_set(unit,l18,(uint32*)l16,KEYf,(uint32*)l256);l134 = l132(unit,
lpm_entry,&l126);if(SOC_FAILURE(l134)){return l134;}if((l126 == 0)&&(l256[0]
== 0)&&(l256[1] == 0)&&(l256[2] == 0)&&(l256[3] == 0)){l20 = 1;}if(l257!= 
NULL){*l257 = l20;}soc_mem_field32_set(unit,l18,l16,LENGTHf,l126);if(l17 == 
NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){sal_memset(l17,0,
soc_mem_entry_words(unit,l18)*4);sal_memcpy(l17,l16,soc_mem_entry_words(unit,
l18)*4);soc_mem_field32_set(unit,l18,l17,DST_DISCARDf,0);soc_mem_field32_set(
unit,l18,l17,RPEf,0);soc_mem_field32_set(unit,l18,l17,SRC_DISCARDf,l19&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(unit,l18,l17,DEFAULTROUTEf,l20)
;}return(SOC_E_NONE);}static int l21(int unit,void*l16,soc_mem_t l18,int l6,
int l22,int l23,int index,void*lpm_entry){uint32 l136;uint32 l256[4];uint32
l149,l258;sal_memset(lpm_entry,0,soc_mem_entry_words(unit,L3_DEFIP_PAIR_128m)
*4);l136 = soc_mem_field32_get(unit,l18,l16,HITf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,HITf,l136);l136 = soc_mem_field32_get(unit,l18,
l16,VALIDf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf
,l136);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_LWRf,l136
);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_UPRf,l136);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_UPRf,l136);l136 = 
soc_mem_field32_get(unit,l18,l16,ECMPf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,ECMPf,l136);l136 = soc_mem_field32_get(unit,l18,
l16,ECMP_PTRf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ECMP_PTRf,l136);l136 = soc_mem_field32_get(unit,l18,l16,NEXT_HOP_INDEXf);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf,l136);
l136 = soc_mem_field32_get(unit,l18,l16,PRIf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,PRIf,l136);l136 = soc_mem_field32_get(unit,l18,
l16,RPEf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,RPEf,l136);
l136 = soc_mem_field32_get(unit,l18,l16,DST_DISCARDf);soc_mem_field32_set(
unit,L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf,l136);l136 = 
soc_mem_field32_get(unit,l18,l16,SRC_DISCARDf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,SRC_DISCARDf,l136);l136 = soc_mem_field32_get(
unit,l18,l16,CLASS_IDf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry
,CLASS_IDf,l136);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ALG_BKT_PTRf,l23);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ALG_HIT_IDXf,index);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
MODE_MASK0_LWRf,3);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
MODE_MASK1_LWRf,3);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
MODE_MASK0_UPRf,3);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
MODE_MASK1_UPRf,3);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ENTRY_TYPE_MASK0_LWRf,1);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,
lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,1);soc_mem_field32_set(
unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_LWRf,3);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_LWRf,3);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_UPRf,3);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_UPRf,3);
soc_mem_field_get(unit,l18,l16,KEYf,l256);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf,l256[0]);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf,l256[1]);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf,l256[2]);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf,l256[3]);l256[0] = l256[1] = l256[
2] = l256[3] = 0;l136 = soc_mem_field32_get(unit,l18,l16,LENGTHf);l141(unit,
lpm_entry,l136);if(l22 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_HIGHf,1);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_ROUTEf,1);l149 = 0;l258 = 0;}else if(l22
== SOC_L3_VRF_GLOBAL){soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
GLOBAL_ROUTEf,1);l149 = 0;l258 = 0;}else{l149 = l22;l258 = SOC_VRF_MAX(unit);
}soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_LWRf,l149);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_LWRf,l149);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_UPRf,l149);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_UPRf,l149);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_LWRf,l258)
;soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_LWRf,l258
);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_UPRf,
l258);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_UPRf
,l258);return(SOC_E_NONE);}int soc_th_alpm_128_warmboot_pivot_add(int unit,
int l6,void*lpm_entry,int l259,int l260){int l134 = SOC_E_NONE;uint32 key[4] = 
{0,0,0,0};alpm_pivot_t*l32 = NULL;alpm_bucket_handle_t*l185 = NULL;int l149 = 
0,l22 = 0;uint32 l261;trie_t*l208 = NULL;uint32 prefix[5] = {0};int l20 = 0;
l134 = l143(unit,lpm_entry,prefix,&l261,&l20);SOC_IF_ERROR_RETURN(l134);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&l149,&l22));
l259 = soc_th_alpm_physical_idx(unit,L3_DEFIP_PAIR_128m,l259,l6);l185 = 
sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l185 == NULL)
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for PIVOT trie node \n")));return SOC_E_NONE;}
sal_memset(l185,0,sizeof(*l185));l32 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l32 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for PIVOT trie node \n")));sal_free(l185);
return SOC_E_MEMORY;}sal_memset(l32,0,sizeof(*l32));PIVOT_BUCKET_HANDLE(l32) = 
l185;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l32));key[0] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR0_LWRf);key[1] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR1_LWRf);key[2] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR0_UPRf);key[3] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR1_UPRf);
PIVOT_BUCKET_INDEX(l32) = l260;PIVOT_TCAM_INDEX(l32) = SOC_ALPM_128_ADDR_LWR(
l259)<<1;if(l149!= SOC_L3_VRF_OVERRIDE){l208 = VRF_PIVOT_TRIE_IPV6_128(unit,
l22);if(l208 == NULL){trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(
unit,l22));l208 = VRF_PIVOT_TRIE_IPV6_128(unit,l22);}sal_memcpy(l32->key,
prefix,sizeof(prefix));l32->len = l261;l134 = trie_insert(l208,l32->key,NULL,
l32->len,(trie_node_t*)l32);if(SOC_FAILURE(l134)){sal_free(l185);sal_free(l32
);return l134;}}ALPM_TCAM_PIVOT(unit,SOC_ALPM_128_ADDR_LWR(l259)<<1) = l32;
PIVOT_BUCKET_VRF(l32) = l22;PIVOT_BUCKET_IPV6(l32) = l6;
PIVOT_BUCKET_ENT_CNT_UPDATE(l32);if(key[0] == 0&&key[1] == 0&&key[2] == 0&&
key[3] == 0){PIVOT_BUCKET_DEF(l32) = TRUE;}VRF_PIVOT_REF_INC(unit,l22,l6);
return l134;}static int l262(int unit,int l6,void*lpm_entry,void*l16,
soc_mem_t l18,int l259,int l260,int l263){int l264;int l22;int l134 = 
SOC_E_NONE;int l20 = 0;uint32 prefix[5] = {0,0,0,0,0};uint32 l33;void*l265 = 
NULL;trie_t*l38 = NULL;trie_t*l31 = NULL;trie_node_t*l169 = NULL;payload_t*
l266 = NULL;payload_t*l191 = NULL;alpm_pivot_t*pivot_pyld = NULL;if((NULL == 
lpm_entry)||(NULL == l16)){return SOC_E_PARAM;}SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&l264,&l22));l18 = 
L3_DEFIP_ALPM_IPV6_128m;l265 = sal_alloc(sizeof(defip_pair_128_entry_t),
"Temp Defip Pair lpm_entry");if(NULL == l265){return SOC_E_MEMORY;}
SOC_IF_ERROR_RETURN(l21(unit,l16,l18,l6,l264,l260,l259,l265));l134 = l143(
unit,l265,prefix,&l33,&l20);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"prefix create failed\n")));return l134;}sal_free(l265);l259 = 
soc_th_alpm_physical_idx(unit,L3_DEFIP_PAIR_128m,l259,l6);pivot_pyld = 
ALPM_TCAM_PIVOT(unit,SOC_ALPM_128_ADDR_LWR(l259)<<1);l38 = PIVOT_BUCKET_TRIE(
pivot_pyld);l266 = sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == 
l266){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l191 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l191){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node\n")));sal_free(l266);return
SOC_E_MEMORY;}sal_memset(l266,0,sizeof(*l266));sal_memset(l191,0,sizeof(*l191
));l266->key[0] = prefix[0];l266->key[1] = prefix[1];l266->key[2] = prefix[2]
;l266->key[3] = prefix[3];l266->key[4] = prefix[4];l266->len = l33;l266->
index = l263;sal_memcpy(l191,l266,sizeof(*l266));l134 = trie_insert(l38,
prefix,NULL,l33,(trie_node_t*)l266);if(SOC_FAILURE(l134)){goto l267;}if(l6){
l31 = VRF_PREFIX_TRIE_IPV6_128(unit,l22);}if(!l20){l134 = trie_insert(l31,
prefix,NULL,l33,(trie_node_t*)l191);if(SOC_FAILURE(l134)){goto l268;}}return
l134;l268:(void)trie_delete(l38,prefix,l33,&l169);l266 = (payload_t*)l169;
l267:sal_free(l266);sal_free(l191);return l134;}static int l269(int unit,int
l28,int l22,int l209,int bkt_ptr){int l134 = SOC_E_NONE;uint32 l33;uint32 key
[5] = {0,0,0,0,0};trie_t*l270 = NULL;payload_t*l199 = NULL;
defip_pair_128_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_pair_128_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unable to allocate memory for LPM entry\n")
));return SOC_E_MEMORY;}l24(unit,key,0,l22,l28,lpm_entry,0,1);if(l22 == 
SOC_VRF_MAX(unit)+1){soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
DEFAULT_MISSf,1);}soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
ALG_BKT_PTRf,bkt_ptr);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(unit,l22) = lpm_entry;
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(unit,l22));l270 = 
VRF_PREFIX_TRIE_IPV6_128(unit,l22);l199 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l199 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l199,0,sizeof(*l199));l33 = 0;l199->key[0] = key[0];
l199->key[1] = key[1];l199->len = l33;l134 = trie_insert(l270,key,NULL,l33,&(
l199->node));if(SOC_FAILURE(l134)){sal_free(l199);return l134;}
VRF_TRIE_INIT_DONE(unit,l22,l28,1);return l134;}int
soc_th_alpm_128_warmboot_prefix_insert(int unit,int l6,void*lpm_entry,void*
l16,int l259,int l260,int l263){int l264;int l22;int l134 = SOC_E_NONE;
soc_mem_t l18;l18 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&l264,&l22));if(l264 == 
SOC_L3_VRF_OVERRIDE){return(l134);}if(!VRF_TRIE_INIT_COMPLETED(unit,l22,l6)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"soc_alpm_128_insert:VRF %d is not ""initialized\n"),l22));l134 = l269(unit,
l6,l22,l259,l260);if(SOC_FAILURE(l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(unit,"VRF %d/%d trie init \n""failed\n"),l22,l6));return l134;}}l134 = l262(
unit,l6,lpm_entry,l16,l18,l259,l260,l263);if(l134!= SOC_E_NONE){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unit %d : ""Route Insertion Failed :%s\n"),
unit,soc_errmsg(l134)));return(l134);}VRF_TRIE_ROUTES_INC(unit,l22,l6);return
(l134);}int soc_th_alpm_128_warmboot_bucket_bitmap_set(int l1,int l28,int l211
){int l271 = 1;if(l28){if(!soc_th_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)
){l271 = 2;}}if(SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}
SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l211,l271);return SOC_E_NONE;}int
soc_th_alpm_128_warmboot_lpm_reinit_done(int unit){int l209;int l272 = ((3*(
128+2+1))-1);int l252 = soc_mem_index_count(unit,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(unit)){l252>>= 1;}if(!soc_th_alpm_mode_get(unit)){(l54[(
unit)][(((3*(128+2+1))-1))].l49) = -1;for(l209 = ((3*(128+2+1))-1);l209>-1;
l209--){if(-1 == (l54[(unit)][(l209)].l47)){continue;}(l54[(unit)][(l209)].
l49) = l272;(l54[(unit)][(l272)].next) = l209;(l54[(unit)][(l272)].l51) = (
l54[(unit)][(l209)].l47)-(l54[(unit)][(l272)].l48)-1;l272 = l209;}(l54[(unit)
][(l272)].next) = -1;(l54[(unit)][(l272)].l51) = l252-(l54[(unit)][(l272)].
l48)-1;}else{(l54[(unit)][(((3*(128+2+1))-1))].l49) = -1;for(l209 = ((3*(128+
2+1))-1);l209>(((3*(128+2+1))-1)/3);l209--){if(-1 == (l54[(unit)][(l209)].l47
)){continue;}(l54[(unit)][(l209)].l49) = l272;(l54[(unit)][(l272)].next) = 
l209;(l54[(unit)][(l272)].l51) = (l54[(unit)][(l209)].l47)-(l54[(unit)][(l272
)].l48)-1;l272 = l209;}(l54[(unit)][(l272)].next) = -1;(l54[(unit)][(l272)].
l51) = l252-(l54[(unit)][(l272)].l48)-1;l272 = (((3*(128+2+1))-1)/3);(l54[(
unit)][((((3*(128+2+1))-1)/3))].l49) = -1;for(l209 = ((((3*(128+2+1))-1)/3)-1
);l209>-1;l209--){if(-1 == (l54[(unit)][(l209)].l47)){continue;}(l54[(unit)][
(l209)].l49) = l272;(l54[(unit)][(l272)].next) = l209;(l54[(unit)][(l272)].
l51) = (l54[(unit)][(l209)].l47)-(l54[(unit)][(l272)].l48)-1;l272 = l209;}(
l54[(unit)][(l272)].next) = -1;(l54[(unit)][(l272)].l51) = (l252>>1)-(l54[(
unit)][(l272)].l48)-1;}return(SOC_E_NONE);}int
soc_th_alpm_128_warmboot_lpm_reinit(int unit,int l6,int l209,void*lpm_entry){
int l13;l216(unit,lpm_entry,l209,0x4000,0);SOC_IF_ERROR_RETURN(l250(unit,
lpm_entry,&l13));if((l54[(unit)][(l13)].l50) == 0){(l54[(unit)][(l13)].l47) = 
l209;(l54[(unit)][(l13)].l48) = l209;}else{(l54[(unit)][(l13)].l48) = l209;}(
l54[(unit)][(l13)].l50)++;return(SOC_E_NONE);}
#endif

#endif /* ALPM_ENABLE */
