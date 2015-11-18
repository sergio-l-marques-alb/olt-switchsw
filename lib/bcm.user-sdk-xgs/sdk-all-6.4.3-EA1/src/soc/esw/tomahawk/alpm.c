/*
 * $Id: alpm.c,v 1.49 Broadcom SDK $
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
 * File:    alpm.c
 * Purpose: Tomahawk Primitives for LPM management in ALPM - Mode.
 * Requires:
 */

/* Implementation notes:
 */
#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <shared/bsl.h>

#ifdef ALPM_ENABLE
#include <shared/util.h>
#include <shared/l3.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#include <soc/esw/trie.h>

#if 1
soc_alpm_bucket_t soc_th_alpm_bucket[SOC_MAX_NUM_DEVICES];void l1(int l2);
static int l3(int l2);static int l4(int l2);static int l5(int l2,void*l6,int*
index);static int l7(int l2,void*l8);static int l9(int l2,void*l8,int l10,int
l11,int l12,defip_aux_scratch_entry_t*l13);static int l14(int l2,void*l8,void
*l15,int*l16,int*l17,int*l10);static int l18(int l2,void*l8,void*l15,int*l16)
;static int l19(int l2);static int l20(int l21,void*lpm_entry,void*l22,void*
l23,soc_mem_t l24,uint32 l25,uint32*l26);static int l27(int l21,void*l22,
soc_mem_t l24,int l10,int l28,int l29,int index,void*lpm_entry);static int l30
(int l21,uint32*key,int len,int l28,int l10,defip_entry_t*lpm_entry,int l31,
int l32);static int l33(int l2,int l28,int l34);static int l35(l36*l37,trie_t
*l38,uint32*l39,uint32 l40,trie_node_t*l41,defip_entry_t*lpm_entry,uint32*l42
);int soc_th_alpm_lpm_vrf_get(int l21,void*lpm_entry,int*l28,int*l43);static
int l44(l36*l37,int*l45,int*l16);int _soc_th_alpm_rollback_bkt_move(int l2,
void*l8,soc_mem_t l24,alpm_pivot_t*l46,alpm_pivot_t*l47,
alpm_mem_prefix_array_t*l48,int*l49,int l50);void l51(int l2,defip_entry_t*
l52,void*l8,int l53,alpm_pivot_t*l54);typedef struct l55{int l56;int l57;int
l58;int next;int l59;int l60;}l61,*l62;static l62 l63[SOC_MAX_NUM_DEVICES];
typedef struct l64{soc_field_info_t*l65;soc_field_info_t*l66;soc_field_info_t
*l67;soc_field_info_t*l68;soc_field_info_t*l69;soc_field_info_t*l70;
soc_field_info_t*l71;soc_field_info_t*l72;soc_field_info_t*l73;
soc_field_info_t*l74;soc_field_info_t*l75;soc_field_info_t*l76;
soc_field_info_t*l77;soc_field_info_t*l78;soc_field_info_t*l79;
soc_field_info_t*l80;soc_field_info_t*l81;soc_field_info_t*l82;
soc_field_info_t*l83;soc_field_info_t*l84;soc_field_info_t*l85;
soc_field_info_t*l86;soc_field_info_t*l87;soc_field_info_t*l88;
soc_field_info_t*l89;soc_field_info_t*l90;soc_field_info_t*l91;
soc_field_info_t*l92;soc_field_info_t*l93;soc_field_info_t*l94;
soc_field_info_t*l95;soc_field_info_t*l96;soc_field_info_t*l97;
soc_field_info_t*l98;soc_field_info_t*l99;soc_field_info_t*l100;
soc_field_info_t*l101;soc_field_info_t*l102;soc_field_info_t*l103;
soc_field_info_t*l104;soc_field_info_t*l105;soc_field_info_t*l106;
soc_field_info_t*l107;soc_field_info_t*l108;soc_field_info_t*l109;
soc_field_info_t*l110;}l111,*l112;static l112 l113[SOC_MAX_NUM_DEVICES];
typedef struct l114{int l21;int l115;int l116;uint16*l117;uint16*l118;}l119;
typedef uint32 l120[5];typedef int(*l121)(l120 l122,l120 l123);static l119*
l124[SOC_MAX_NUM_DEVICES];static void l125(int l2,void*l15,int index,l120 l126
);static uint16 l127(uint8*l128,int l129);static int l130(int l21,int l115,
int l116,l119**l131);static int l132(l119*l133);static int l134(l119*l135,
l121 l136,l120 entry,int l137,uint16*l138);static int l139(l119*l135,l121 l136
,l120 entry,int l137,uint16 l140,uint16 l49);static int l141(l119*l135,l121
l136,l120 entry,int l137,uint16 l142);extern alpm_vrf_handle_t*
alpm_vrf_handle[SOC_MAX_NUM_DEVICES];extern alpm_pivot_t**tcam_pivot[
SOC_MAX_NUM_DEVICES];extern int l143(int l21);extern int l144(int l21);int
soc_th_alpm_mode_get(int l2){uint32 l145;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l2,&l145));return soc_reg_field_get(l2,
L3_DEFIP_RPF_CONTROLr,l145,LOOKUP_MODEf);}int _soc_th_alpm_rpf_entry(int l2,
int l146){int l147;l147 = (l146>>2)&0x3fff;l147+= SOC_ALPM_BUCKET_COUNT(l2);
return(l146&~(0x3fff<<2))|(l147<<2);}int soc_th_alpm_physical_idx(int l2,
soc_mem_t l24,int index,int l148){int l149 = index&1;if(l148){return
soc_trident2_l3_defip_index_map(l2,l24,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_map(l2,l24,index);index<<= 1;index|= l149;return
index;}int soc_th_alpm_logical_idx(int l2,soc_mem_t l24,int index,int l148){
int l149 = index&1;if(l148){return soc_trident2_l3_defip_index_remap(l2,l24,
index);}index>>= 1;index = soc_trident2_l3_defip_index_remap(l2,l24,index);
index<<= 1;index|= l149;return index;}static int l150(int l2,void*entry,
uint32*prefix,uint32*l17,int*l26){int l151,l152,l10;int l137 = 0;int l153 = 
SOC_E_NONE;uint32 l154,l149;prefix[0] = prefix[1] = prefix[2] = prefix[3] = 
prefix[4] = 0;l10 = soc_mem_field32_get(l2,L3_DEFIPm,entry,MODE0f);l151 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR0f);l152 = soc_mem_field32_get(
l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);prefix[1] = l151;l151 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR1f);l152 = soc_mem_field32_get(
l2,L3_DEFIPm,entry,IP_ADDR_MASK1f);prefix[0] = l151;if(l10){prefix[4] = 
prefix[1];prefix[3] = prefix[0];prefix[1] = prefix[0] = 0;l152 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l153 = _ipmask2pfx
(l152,&l137))<0){return(l153);}l152 = soc_mem_field32_get(l2,L3_DEFIPm,entry,
IP_ADDR_MASK1f);if(l137){if(l152!= 0xffffffff){return(SOC_E_PARAM);}l137+= 32
;}else{if((l153 = _ipmask2pfx(l152,&l137))<0){return(l153);}}l154 = 64-l137;
if(l154<32){prefix[4]>>= l154;l149 = (((32-l154) == 32)?0:(prefix[3])<<(32-
l154));prefix[3]>>= l154;prefix[4]|= l149;}else{prefix[4] = (((l154-32) == 32
)?0:(prefix[3])>>(l154-32));prefix[3] = 0;}}else{l152 = soc_mem_field32_get(
l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l153 = _ipmask2pfx(l152,&l137))<0){
return(l153);}prefix[1] = (((32-l137) == 32)?0:(prefix[1])>>(32-l137));prefix
[0] = 0;}*l17 = l137;*l26 = (prefix[0] == 0)&&(prefix[1] == 0)&&(l137 == 0);
return SOC_E_NONE;}int _soc_th_alpm_find_in_bkt(int l2,soc_mem_t l24,int
bucket_index,int l155,uint32*l15,void*l156,int*l138,int l34){int l153;l153 = 
soc_mem_alpm_lookup(l2,l24,bucket_index,MEM_BLOCK_ANY,l155,l15,l156,l138);if(
SOC_SUCCESS(l153)){return l153;}if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_lookup(l2,l24,bucket_index+1,MEM_BLOCK_ANY,l155,l15,l156,l138);}
return l153;}static int l157(int l2,uint32*prefix,uint32 l40,int l34,int l28,
int*l158,int*l53,int*bucket_index){int l153 = SOC_E_NONE;trie_t*l159;
trie_node_t*l160 = NULL;alpm_pivot_t*l54;if(l34){l159 = VRF_PIVOT_TRIE_IPV6(
l2,l28);}else{l159 = VRF_PIVOT_TRIE_IPV4(l2,l28);}l153 = trie_find_lpm(l159,
prefix,l40,&l160);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l2,"Pivot find failed\n")));return l153;}l54 = (alpm_pivot_t*)l160;*l158 = 1
;*l53 = PIVOT_TCAM_INDEX(l54);*bucket_index = PIVOT_BUCKET_INDEX(l54);return
SOC_E_NONE;}static int l161(int l2,void*l8,soc_mem_t l24,void*l156,int*l53,
int*bucket_index,int*l16){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l162,l28,
l34;int l138;uint32 l11,l155;int l153 = SOC_E_NONE;int l158 = 0;l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l2,l8,&l162,&l28));if(l162 == 0){
if(soc_th_alpm_mode_get(l2)){return SOC_E_PARAM;}}if(l28 == SOC_VRF_MAX(l2)+1
){l11 = 0;if(l144(l2)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l2,
l155);}else{SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l155);}}else{l11 = 2;if(l144(
l2)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l2,l155);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l155);}}if(!(((l162 == SOC_L3_VRF_OVERRIDE)
||((soc_th_alpm_mode_get(l2) == SOC_ALPM_MODE_TCAM_ALPM)&&(l162 == 
SOC_L3_VRF_GLOBAL))))){uint32 prefix[5],l40;int l26 = 0;l153 = l150(l2,l8,
prefix,&l40,&l26);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l2,"_soc_alpm_insert: prefix create failed\n")));return l153;}l153 = l157(l2
,prefix,l40,l34,l28,&l158,l53,bucket_index);SOC_IF_ERROR_RETURN(l153);if(l158
){l20(l2,l8,l15,0,l24,0,0);l153 = _soc_th_alpm_find_in_bkt(l2,l24,*
bucket_index,l155,l15,l156,&l138,l34);if(SOC_SUCCESS(l153)){*l16 = l138;}}
else{l153 = SOC_E_NOT_FOUND;}}return l153;}static int l163(int l2,void*l8,
void*l156,void*l164,soc_mem_t l24,int l138){defip_aux_scratch_entry_t l13;int
l162,l34,l28;int bucket_index;uint32 l11,l155;int l153 = SOC_E_NONE;int l158 = 
0,l149 = 0;int l53;l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){
if(!(l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);
}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l2,l8,&l162,&l28));if(l28 == 
SOC_VRF_MAX(l2)+1){l11 = 0;if(l144(l2)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l2,l155);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l155);}}else{l11 = 2;if(l144(l2)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l2,l155);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l155);}}if(!soc_th_alpm_mode_get(l2)){l11 = 
2;}if(l162!= SOC_L3_VRF_OVERRIDE){sal_memset(&l13,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l2,l8,l34,l11,0,&l13));
SOC_IF_ERROR_RETURN(soc_mem_write(l2,l24,MEM_BLOCK_ANY,l138,l156));if(
SOC_URPF_STATUS_GET(l2)){SOC_IF_ERROR_RETURN(soc_mem_write(l2,l24,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l2,l138),l164));if(l153!= SOC_E_NONE){
return SOC_E_FAIL;}}l149 = soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,
IP_LENGTHf);soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,REPLACE_LENf,
l149);SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,TRUE,&
l158,&l53,&bucket_index));if(SOC_URPF_STATUS_GET(l2)){l149 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l149+= 1;
soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l149);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,TRUE,&l158,&
l53,&bucket_index));}}return l153;}static int l165(int l2,int l45,int l34,int
l42){int l153,l149,index;defip_aux_table_entry_t entry;index = l45>>(l34?0:1)
;l153 = soc_mem_read(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l153);if(l34){soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,
&entry,BPM_LENGTH0f,l42);soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH1f,l42);l149 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE0f);}else{if(l45&1){soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH1f,l42);l149 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE1f);}else{soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH0f,l42);l149 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE0f);}}l153 = soc_mem_write(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,
&entry);SOC_IF_ERROR_RETURN(l153);if(SOC_URPF_STATUS_GET(l2)){l149++;if(l34){
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l42);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l42);}else{if(
l45&1){soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l42);}
else{soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l42);}}
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l149);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l149);index+= (2*
soc_mem_index_count(l2,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l2,L3_DEFIPm))
/2;l153 = soc_mem_write(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}
return l153;}static int l166(int l2,int l167,void*entry,void*l168,int l169){
uint32 l149,l152,l34,l11,l170 = 0;soc_mem_t l24 = L3_DEFIPm;soc_mem_t l171 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l172;int l153 = SOC_E_NONE,l137,l173,l28,
l174;SOC_IF_ERROR_RETURN(soc_mem_read(l2,l171,MEM_BLOCK_ANY,l167,l168));l149 = 
soc_mem_field32_get(l2,l24,entry,VRF_ID_0f);soc_mem_field32_set(l2,l171,l168,
VRF0f,l149);l149 = soc_mem_field32_get(l2,l24,entry,VRF_ID_1f);
soc_mem_field32_set(l2,l171,l168,VRF1f,l149);l149 = soc_mem_field32_get(l2,
l24,entry,MODE0f);soc_mem_field32_set(l2,l171,l168,MODE0f,l149);l149 = 
soc_mem_field32_get(l2,l24,entry,MODE1f);soc_mem_field32_set(l2,l171,l168,
MODE1f,l149);l34 = l149;l149 = soc_mem_field32_get(l2,l24,entry,VALID0f);
soc_mem_field32_set(l2,l171,l168,VALID0f,l149);l149 = soc_mem_field32_get(l2,
l24,entry,VALID1f);soc_mem_field32_set(l2,l171,l168,VALID1f,l149);l149 = 
soc_mem_field32_get(l2,l24,entry,IP_ADDR_MASK0f);if((l153 = _ipmask2pfx(l149,
&l137))<0){return l153;}l152 = soc_mem_field32_get(l2,l24,entry,
IP_ADDR_MASK1f);if((l153 = _ipmask2pfx(l152,&l173))<0){return l153;}if(l34){
soc_mem_field32_set(l2,l171,l168,IP_LENGTH0f,l137+l173);soc_mem_field32_set(
l2,l171,l168,IP_LENGTH1f,l137+l173);}else{soc_mem_field32_set(l2,l171,l168,
IP_LENGTH0f,l137);soc_mem_field32_set(l2,l171,l168,IP_LENGTH1f,l173);}l149 = 
soc_mem_field32_get(l2,l24,entry,IP_ADDR0f);soc_mem_field32_set(l2,l171,l168,
IP_ADDR0f,l149);l149 = soc_mem_field32_get(l2,l24,entry,IP_ADDR1f);
soc_mem_field32_set(l2,l171,l168,IP_ADDR1f,l149);l149 = soc_mem_field32_get(
l2,l24,entry,ENTRY_TYPE0f);soc_mem_field32_set(l2,l171,l168,ENTRY_TYPE0f,l149
);l149 = soc_mem_field32_get(l2,l24,entry,ENTRY_TYPE1f);soc_mem_field32_set(
l2,l171,l168,ENTRY_TYPE1f,l149);if(!l34){sal_memcpy(&l172,entry,sizeof(l172))
;l153 = soc_th_alpm_lpm_vrf_get(l2,(void*)&l172,&l28,&l137);
SOC_IF_ERROR_RETURN(l153);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_ip4entry1_to_0(
l2,&l172,&l172,PRESERVE_HIT));l153 = soc_th_alpm_lpm_vrf_get(l2,(void*)&l172,
&l174,&l137);SOC_IF_ERROR_RETURN(l153);}else{l153 = soc_th_alpm_lpm_vrf_get(
l2,entry,&l28,&l137);}if(SOC_URPF_STATUS_GET(l2)){if(l169>= (
soc_mem_index_count(l2,L3_DEFIPm)>>1)){l170 = 1;}}switch(l28){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l2,l171,l168,VALID0f,0);l11 = 0;break
;case SOC_L3_VRF_GLOBAL:l11 = l170?1:0;break;default:l11 = l170?3:2;break;}
soc_mem_field32_set(l2,l171,l168,DB_TYPE0f,l11);if(!l34){switch(l174){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l2,l171,l168,VALID1f,0);l11 = 0;break
;case SOC_L3_VRF_GLOBAL:l11 = l170?1:0;break;default:l11 = l170?3:2;break;}
soc_mem_field32_set(l2,l171,l168,DB_TYPE1f,l11);}else{if(l28 == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l2,l171,l168,VALID1f,0);}
soc_mem_field32_set(l2,l171,l168,DB_TYPE1f,l11);}if(l170){l149 = 
soc_mem_field32_get(l2,l24,entry,ALG_BKT_PTR0f);if(l149){l149+= 
SOC_ALPM_BUCKET_COUNT(l2);soc_mem_field32_set(l2,l24,entry,ALG_BKT_PTR0f,l149
);}if(!l34){l149 = soc_mem_field32_get(l2,l24,entry,ALG_BKT_PTR1f);if(l149){
l149+= SOC_ALPM_BUCKET_COUNT(l2);soc_mem_field32_set(l2,l24,entry,
ALG_BKT_PTR1f,l149);}}}return SOC_E_NONE;}static int l175(int l2,int l176,int
index,int l177,void*entry){defip_aux_table_entry_t l168;l177 = 
soc_th_alpm_physical_idx(l2,L3_DEFIPm,l177,1);SOC_IF_ERROR_RETURN(l166(l2,
l177,entry,(void*)&l168,index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l2,
MEM_BLOCK_ANY,index,entry));index = soc_th_alpm_physical_idx(l2,L3_DEFIPm,
index,1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,index
,&l168));return SOC_E_NONE;}int _soc_th_alpm_insert_in_bkt(int l2,soc_mem_t
l24,int bucket_index,int l155,void*l156,uint32*l15,int*l138,int l34){int l153
;l153 = soc_mem_alpm_insert(l2,l24,bucket_index,MEM_BLOCK_ANY,l155,l156,l15,
l138);if(l153 == SOC_E_FULL){if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_insert(l2,l24,bucket_index+1,MEM_BLOCK_ANY,l155,l156,l15,l138);}
}return l153;}int l178(int l2,soc_mem_t l24,int bucket_index,int l179,uint32
l155,int*l180){int l181,l182 = 0;int l183[4] = {0};int l184 = 0;int l185 = 0;
int l186;int l34 = 0;int l187 = 6;int l188;int l189 = 0;switch(l24){case
L3_DEFIP_ALPM_IPV6_64m:l187 = 4;l34 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:
l187 = 2;l34 = 1;break;default:break;}if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){if(
l179>= ALPM_RAW_BKT_COUNT*l187){bucket_index++;l179-= ALPM_RAW_BKT_COUNT*l187
;}}l188 = 4;if(l144(l2)<= 2){l188 = 2;}l189 = ((1<<l188)-1);l186 = l188-
_shr_popcount(l155&l189);if(bucket_index>= (1<<16)||l179>= l186*l187){return
SOC_E_FULL;}l185 = l179%l187;for(l181 = 0;l181<l188;l181++){if((1<<l181)&l155
){continue;}l183[l182++] = l181;}l184 = l183[l179/l187];*l180 = (l185<<16)|(
bucket_index<<2)|(l184);return SOC_E_NONE;}static int l190(int l2,soc_mem_t
l24,int l34,alpm_mem_prefix_array_t*l191,int*l49){int l181,l153 = SOC_E_NONE,
l192;defip_alpm_ipv4_entry_t l193,l194;defip_alpm_ipv6_64_entry_t l195,l196;
void*l197 = NULL,*l198 = NULL;int l199,l200;int*l201 = NULL;int l202 = FALSE;
if(l34){l199 = sizeof(l195);l200 = sizeof(l196);}else{l199 = sizeof(l193);
l200 = sizeof(l194);}l197 = sal_alloc(l199*l191->count,"rb_bufp");if(l197 == 
NULL){l153 = SOC_E_MEMORY;goto l203;}l198 = sal_alloc(l200*l191->count,
"rb_sip_bufp");if(l198 == NULL){l153 = SOC_E_MEMORY;goto l203;}l201 = 
sal_alloc(sizeof(*l201)*l191->count,"roll_back_index");if(l201 == NULL){l153 = 
SOC_E_MEMORY;goto l203;}sal_memset(l201,-1,sizeof(*l201)*l191->count);for(
l181 = 0;l181<l191->count;l181++){payload_t*prefix = l191->prefix[l181];if(
prefix->index>= 0){l153 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,prefix->index,(
uint8*)l197+l181*l199);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l2,"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x 0x%08x"" 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l181--;l202 = TRUE;
break;}if(SOC_URPF_STATUS_GET(l2)){l153 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l2,prefix->index),(uint8*)l198+l181*l200);if(
SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x"" 0x%08x 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l181--;l202 = TRUE;
break;}}l153 = soc_mem_write(l2,l24,MEM_BLOCK_ALL,prefix->index,
soc_mem_entry_null(l2,l24));if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l2,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old bkt for pfx 0x%08x 0x%08x\n"),prefix->key[0],prefix->key[
1]));l201[l181] = prefix->index;l202 = TRUE;break;}if(SOC_URPF_STATUS_GET(l2)
){l153 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l2,prefix
->index),soc_mem_entry_null(l2,l24));if(SOC_FAILURE(l153)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old urpf bkt for pfx 0x%08x"" 0x%08x\n"),prefix->key[0],
prefix->key[1]));l201[l181] = prefix->index;l202 = TRUE;break;}}}l201[l181] = 
prefix->index;prefix->index = l49[l181];}if(l202){for(;l181>= 0;l181--){
payload_t*prefix = l191->prefix[l181];prefix->index = l201[l181];if(l201[l181
]<0){continue;}l192 = soc_mem_write(l2,l24,MEM_BLOCK_ALL,l201[l181],(uint8*)
l197+l181*l199);if(SOC_FAILURE(l192)){break;}if(!SOC_URPF_STATUS_GET(l2)){
continue;}l192 = soc_mem_write(l2,l24,MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(l2
,l201[l181]),(uint8*)l198+l181*l200);if(SOC_FAILURE(l192)){break;}}}l203:if(
l201){sal_free(l201);l201 = NULL;}if(l198){sal_free(l198);l198 = NULL;}if(
l197){sal_free(l197);l197 = NULL;}return l153;}void l51(int l2,defip_entry_t*
l52,void*l8,int l53,alpm_pivot_t*l54){int l153;trie_t*l159 = NULL;int l34,l28
,l162;trie_node_t*l204 = NULL;l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,
MODE0f);soc_th_alpm_lpm_vrf_get(l2,l8,&l162,&l28);l153 = l7(l2,l52);if(
SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_insert: Failure to free new prefix""at %d\n"),l205(l2,L3_DEFIPm,
l53,l34)));}if(l34){l159 = VRF_PIVOT_TRIE_IPV6(l2,l28);}else{l159 = 
VRF_PIVOT_TRIE_IPV4(l2,l28);}if(ALPM_TCAM_PIVOT(l2,l53<<(l34?1:0))!= NULL){
l153 = trie_delete(l159,l54->key,l54->len,&l204);if(SOC_FAILURE(l153)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l2,l53<<(l34?1:0)) = NULL;VRF_PIVOT_REF_DEC(l2,l28,l34);}int
_soc_th_alpm_rollback_bkt_move(int l2,void*l8,soc_mem_t l24,alpm_pivot_t*l46,
alpm_pivot_t*l47,alpm_mem_prefix_array_t*l48,int*l49,int l50){trie_node_t*
l204 = NULL;uint32 prefix[5],l40;trie_t*trie,*l38;int l34,l28,l162,l181,l26 = 
0;;defip_alpm_ipv4_entry_t l193;defip_alpm_ipv6_64_entry_t l195;void*l206;
payload_t*l207;int l153 = SOC_E_NONE;alpm_bucket_handle_t*l208;l208 = 
PIVOT_BUCKET_HANDLE(l47);trie = PIVOT_BUCKET_TRIE(l47);l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l24 == L3_DEFIP_ALPM_IPV6_128m
){l34 =L3_DEFIP_MODE_128;}l153 = l150(l2,l8,prefix,&l40,&l26);if(SOC_FAILURE(
l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_insert: prefix create failed\n")));return l153;}if(l24 == 
L3_DEFIP_ALPM_IPV6_128m){l206 = ((uint32*)&(l195));}else{l206 = ((l34)?((
uint32*)&(l195)):((uint32*)&(l193)));}soc_th_alpm_lpm_vrf_get(l2,l8,&l162,&
l28);if(l24 == L3_DEFIP_ALPM_IPV6_128m){l38 = VRF_PREFIX_TRIE_IPV6_128(l2,l28
);}else if(l34){l38 = VRF_PREFIX_TRIE_IPV6(l2,l28);}else{l38 = 
VRF_PREFIX_TRIE_IPV4(l2,l28);}for(l181 = 0;l181<l48->count;l181++){payload_t*
l137 = l48->prefix[l181];if(l49[l181]!= -1){if(l24 == L3_DEFIP_ALPM_IPV6_128m
){sal_memset(l206,0,sizeof(defip_alpm_ipv6_128_entry_t));}else if(l34){
sal_memset(l206,0,sizeof(defip_alpm_ipv6_64_entry_t));}else{sal_memset(l206,0
,sizeof(defip_alpm_ipv4_entry_t));}l153 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,
l49[l181],l206);_soc_trident2_alpm_bkt_view_set(l2,l49[l181],INVALIDm);if(
SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l2)){l153 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l2,l49[l181]),l206);_soc_trident2_alpm_bkt_view_set(l2
,_soc_th_alpm_rpf_entry(l2,l49[l181]),INVALIDm);if(SOC_FAILURE(l153)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l204 = NULL;l153 = trie_delete(
PIVOT_BUCKET_TRIE(l47),l137->key,l137->len,&l204);l207 = (payload_t*)l204;if(
SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(l137->
index>0){l153 = trie_insert(PIVOT_BUCKET_TRIE(l46),l137->key,NULL,l137->len,(
trie_node_t*)l207);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l2,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l207!= NULL){sal_free(l207);}}}if(l50 == 
-1){l204 = NULL;l153 = trie_delete(PIVOT_BUCKET_TRIE(l46),prefix,l40,&l204);
l207 = (payload_t*)l204;if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l2,"_soc_th_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l207!= 
NULL){sal_free(l207);}}l153 = alpm_bucket_release(l2,PIVOT_BUCKET_INDEX(l47),
l34);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_th_alpm_insert: new bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(
l47)));}trie_destroy(PIVOT_BUCKET_TRIE(l47));sal_free(l208);sal_free(l47);
sal_free(l49);l204 = NULL;l153 = trie_delete(l38,prefix,l40,&l204);l207 = (
payload_t*)l204;if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l2,"_soc_th_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l207){sal_free(
l207);}return l153;}int l209(int l2,trie_t*l38,trie_t*l210,payload_t*l211,int
*l49,int bucket_index,int l34){trie_node_t*l204 = NULL;payload_t*l212 = NULL;
if(l49!= NULL){sal_free(l49);}l204 = NULL;(void)trie_delete(l38,l211->key,
l211->len,&l204);l211 = (payload_t*)l204;if(bucket_index!= -1){(void)
alpm_bucket_release(l2,bucket_index,l34);}(void)trie_delete(l210,l211->key,
l211->len,&l204);l212 = (payload_t*)l204;if(l212!= NULL){sal_free(l212);}if(
l211!= NULL){sal_free(l211);}return SOC_E_NONE;}static int l35(l36*l37,trie_t
*l38,uint32*l39,uint32 l40,trie_node_t*l41,defip_entry_t*lpm_entry,uint32*l42
){trie_node_t*l160 = NULL;int l34,l28,l162;defip_alpm_ipv4_entry_t l193,l194;
defip_alpm_ipv6_64_entry_t l195,l196;payload_t*l212 = NULL;int l213;void*l206
,*l214;alpm_pivot_t*l54;alpm_bucket_handle_t*l208;int l153 = SOC_E_NONE;int l2
;soc_mem_t l24;l2 = l37->l2;l54 = l37->l54;l213 = l54->l53;l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l37->l8,MODE0f);SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l2,l37->l8,&l162,&l28));l24 = (l34)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l206 = ((l34)?((uint32*)&(l195)):(
(uint32*)&(l193)));l214 = ((l34)?((uint32*)&(l196)):((uint32*)&(l194)));l160 = 
NULL;l153 = trie_find_lpm(l38,l39,l40,&l160);l212 = (payload_t*)l160;if(
SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l2,l39[0],l39[1],l39[2],
l39[3],l39[4],l40));return l153;}if(l212->bkt_ptr){if(l212->bkt_ptr == l37->
l211){sal_memcpy(l206,l37->l156,l34?sizeof(defip_alpm_ipv6_64_entry_t):sizeof
(defip_alpm_ipv4_entry_t));}else{l153 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,((
payload_t*)l212->bkt_ptr)->index,l206);}if(SOC_FAILURE(l153)){return l153;}
l153 = l27(l2,l206,l24,l34,l162,l37->bucket_index,0,lpm_entry);if(SOC_FAILURE
(l153)){return l153;}*l42 = ((payload_t*)(l212->bkt_ptr))->len;}else{l153 = 
soc_mem_read(l2,L3_DEFIPm,MEM_BLOCK_ANY,soc_th_alpm_logical_idx(l2,L3_DEFIPm,
l213>>1,1),lpm_entry);if((!l34)&&(l213&1)){l153 = 
soc_th_alpm_lpm_ip4entry1_to_0(l2,lpm_entry,lpm_entry,0);}}l208 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l208 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l153 = SOC_E_MEMORY;return l153;}sal_memset(l208,0,sizeof(*l208));l54 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");l37->l54 = l54;if(l54
== NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l153 = SOC_E_MEMORY;return l153;}sal_memset(l54,0,sizeof(*l54));
PIVOT_BUCKET_HANDLE(l54) = l208;if(l34){l153 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l54));}else{l153 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l54));}PIVOT_BUCKET_TRIE(l54)->trie = l41;
PIVOT_BUCKET_INDEX(l54) = l37->bucket_index;PIVOT_BUCKET_VRF(l54) = l28;
PIVOT_BUCKET_IPV6(l54) = l34;PIVOT_BUCKET_DEF(l54) = FALSE;(l54)->key[0] = 
l39[0];(l54)->key[1] = l39[1];(l54)->len = l40;(l54)->key[2] = l39[2];(l54)->
key[3] = l39[3];(l54)->key[4] = l39[4];do{if(!(l34)){l39[0] = (((32-l40) == 
32)?0:(l39[1])<<(32-l40));l39[1] = 0;}else{int l215 = 64-l40;int l216;if(l215
<32){l216 = l39[3]<<l215;l216|= (((32-l215) == 32)?0:(l39[4])>>(32-l215));l39
[0] = l39[4]<<l215;l39[1] = l216;l39[2] = l39[3] = l39[4] = 0;}else{l39[1] = 
(((l215-32) == 32)?0:(l39[4])<<(l215-32));l39[0] = l39[2] = l39[3] = l39[4] = 
0;}}}while(0);l30(l2,l39,l40,l28,l34,lpm_entry,0,0);soc_L3_DEFIPm_field32_set
(l2,lpm_entry,ALG_BKT_PTR0f,l37->bucket_index);l37->l54 = l54;return l153;}
static int l44(l36*l37,int*l45,int*l16){trie_node_t*l41;uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];int l34,l28,l162;uint32 l40,l42 = 0;uint32 l155 = 0;
uint32 l39[5];int l138,l213;defip_alpm_ipv4_entry_t l193,l194;
defip_alpm_ipv6_64_entry_t l195,l196;trie_t*l38,*trie;void*l206,*l214;
alpm_pivot_t*l217 = l37->l54;defip_entry_t lpm_entry;soc_mem_t l24;trie_t*
l159 = NULL;alpm_mem_prefix_array_t l191;int*l49 = NULL;int l153 = SOC_E_NONE
,l181,l192,l50 = -1;l34 = soc_mem_field32_get(l37->l2,L3_DEFIPm,l37->l8,
MODE0f);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l37->l2,l37->l8,&l162,&
l28));l24 = (l34)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l206 = ((l34)?((
uint32*)&(l195)):((uint32*)&(l193)));l214 = ((l34)?((uint32*)&(l196)):((
uint32*)&(l194)));l213 = (l37->l54)->l53;if(l28 == SOC_VRF_MAX(l37->l2)+1){if
(l144(l37->l2)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l37->l2,
l155);}else{SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l37->l2,l155);}}else{if(l144(l37
->l2)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l37->l2,l155);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l37->l2,l155);}}if(l34){l38 = 
VRF_PREFIX_TRIE_IPV6(l37->l2,l28);}else{l38 = VRF_PREFIX_TRIE_IPV4(l37->l2,
l28);}trie = PIVOT_BUCKET_TRIE(l37->l54);l153 = alpm_bucket_assign(l37->l2,&
l37->bucket_index,l34);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l37->l2,"_soc_th_alpm_insert: Unable to allocate"
"new bucket for split\n")));l37->bucket_index = -1;l209(l37->l2,l38,trie,l37
->l211,l49,l37->bucket_index,l34);return l153;}l153 = trie_split(trie,l34?
_MAX_KEY_LEN_144_:_MAX_KEY_LEN_48_,FALSE,l39,&l40,&l41,NULL,FALSE);if(
SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l37->l2,
"_soc_th_alpm_insert: Could not split bucket")));l209(l37->l2,l38,trie,l37->
l211,l49,l37->bucket_index,l34);return l153;}l153 = l35(l37,l38,l39,l40,l41,&
lpm_entry,&l42);if(l153!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l37->l2,"could not initialize pivot")));l209(l37->l2,l38,trie,l37->l211,l49,
l37->bucket_index,l34);return l153;}sal_memset(&l191,0,sizeof(l191));l153 = 
trie_traverse(PIVOT_BUCKET_TRIE(l37->l54),alpm_mem_prefix_array_cb,&l191,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l37->l2,"_soc_th_alpm_insert: Bucket split failed")));l209(l37->l2
,l38,trie,l37->l211,l49,l37->bucket_index,l34);return l153;}l49 = sal_alloc(
sizeof(*l49)*l191.count,"Temp storage for location of prefixes in new bucket"
);if(l49 == NULL){l153 = SOC_E_MEMORY;l209(l37->l2,l38,trie,l37->l211,l49,l37
->bucket_index,l34);return l153;}sal_memset(l49,-1,sizeof(*l49)*l191.count);
for(l181 = 0;l181<l191.count;l181++){payload_t*l137 = l191.prefix[l181];if(
l137->index>0){l153 = soc_mem_read(l37->l2,l24,MEM_BLOCK_ANY,l137->index,l206
);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l37->l2,
"_soc_alpm_insert: Failed to""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l191.prefix[l181]->key[1],l191.
prefix[l181]->key[2],l191.prefix[l181]->key[3],l191.prefix[l181]->key[4]));
l192 = _soc_th_alpm_rollback_bkt_move(l37->l2,l37->l8,l24,l217,l37->l54,&l191
,l49,l50);return l153;}if(SOC_URPF_STATUS_GET(l37->l2)){l153 = soc_mem_read(
l37->l2,l24,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l37->l2,l137->index),l214);
if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l37->l2,
"_soc_th_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l191.prefix[l181]->key[1],l191.
prefix[l181]->key[2],l191.prefix[l181]->key[3],l191.prefix[l181]->key[4]));
l192 = _soc_th_alpm_rollback_bkt_move(l37->l2,l37->l8,l24,l217,l37->l54,&l191
,l49,l50);return l153;}}l153 = _soc_th_alpm_insert_in_bkt(l37->l2,l24,l37->
bucket_index,l155,l206,l15,&l138,l34);if(SOC_SUCCESS(l153)){if(
SOC_URPF_STATUS_GET(l37->l2)){l153 = soc_mem_write(l37->l2,l24,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l37->l2,l138),l214);}}}else{l153 = 
_soc_th_alpm_insert_in_bkt(l37->l2,l24,l37->bucket_index,l155,l37->l156,l15,&
l138,l34);if(SOC_SUCCESS(l153)){l50 = l181;*l16 = l138;if(SOC_URPF_STATUS_GET
(l37->l2)){l153 = soc_mem_write(l37->l2,l24,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l37->l2,l138),l37->l164);}}}l49[l181] = l138;if(
SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l37->l2,
"_soc_th_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l191.prefix[l181]->key[1],l191.
prefix[l181]->key[2],l191.prefix[l181]->key[3],l191.prefix[l181]->key[4]));
l192 = _soc_th_alpm_rollback_bkt_move(l37->l2,l37->l8,l24,l217,l37->l54,&l191
,l49,l50);return l153;}}l153 = l5(l37->l2,&lpm_entry,l45);if(SOC_FAILURE(l153
)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l37->l2,
"_soc_th_alpm_insert: Unable to add new""pivot to tcam\n")));if(l153 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l37->l2,l28,l34);}l192 = 
_soc_th_alpm_rollback_bkt_move(l37->l2,l37->l8,l24,l217,l37->l54,&l191,l49,
l50);return l153;}*l45 = soc_th_alpm_physical_idx(l37->l2,L3_DEFIPm,*l45,l34)
;l153 = l165(l37->l2,*l45,l34,l42);if(SOC_FAILURE(l153)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l37->l2,
"_soc_th_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l45));l51(
l37->l2,&lpm_entry,l37->l8,*l45,l37->l54);l192 = 
_soc_th_alpm_rollback_bkt_move(l37->l2,l37->l8,l24,l217,l37->l54,&l191,l49,
l50);return l153;}if(l34){l159 = VRF_PIVOT_TRIE_IPV6(l37->l2,l28);}else{l159 = 
VRF_PIVOT_TRIE_IPV4(l37->l2,l28);}l153 = trie_insert(l159,l37->l54->key,NULL,
l37->l54->len,(trie_node_t*)l37->l54);if(SOC_FAILURE(l153)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l37->l2,"failed to insert into pivot trie\n")));
l192 = _soc_th_alpm_rollback_bkt_move(l37->l2,l37->l8,l24,l217,l37->l54,&l191
,l49,l50);return l153;}ALPM_TCAM_PIVOT(l37->l2,*l45<<(l34?1:0)) = l37->l54;
PIVOT_TCAM_INDEX(l37->l54) = *l45<<(l34?1:0);VRF_PIVOT_REF_INC(l37->l2,l28,
l34);l153 = l190(l37->l2,l24,l34,&l191,l49);if(SOC_FAILURE(l153)){l51(l37->l2
,&lpm_entry,l37->l8,*l45,l37->l54);l192 = _soc_th_alpm_rollback_bkt_move(l37
->l2,l37->l8,l24,l217,l37->l54,&l191,l49,l50);sal_free(l49);l49 = NULL;return
l153;}sal_free(l49);if(l50 == -1){l153 = _soc_th_alpm_insert_in_bkt(l37->l2,
l24,PIVOT_BUCKET_INDEX(l217),l155,l37->l156,l15,&l138,l34);if(SOC_FAILURE(
l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l37->l2,
"_soc_th_alpm_insert: Could not insert new ""prefix into trie after split\n")
));l209(l37->l2,l38,trie,l37->l211,l49,l37->bucket_index,l34);return l153;}if
(SOC_URPF_STATUS_GET(l37->l2)){l153 = soc_mem_write(l37->l2,l24,MEM_BLOCK_ANY
,_soc_th_alpm_rpf_entry(l37->l2,l138),l37->l164);}*l16 = l138;l37->l211->
index = l138;}PIVOT_BUCKET_ENT_CNT_UPDATE(l37->l54);VRF_BUCKET_SPLIT_INC(l37
->l2,l28,l34);return l153;}static int l218(int l2,void*l8,soc_mem_t l24,void*
l156,void*l164,int*l16,int bucket_index,int l53){alpm_pivot_t*l54,*l217;
defip_aux_scratch_entry_t l13;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l219,l40;int l34,l28,l162;int l138;int l153 = SOC_E_NONE,l192;
uint32 l11,l155;int l158 =0;int l45;int l220 = 0;trie_t*trie,*l38;trie_node_t
*l160 = NULL,*l204 = NULL;payload_t*l207,*l221,*l212;int l26 = 0;
defip_alpm_ipv4_entry_t l193,l194;defip_alpm_ipv6_64_entry_t l195,l196;void*
l206,*l214;l36 l37;l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){
if(!(l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);
}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l2,l8,&l162,&l28));if(l28 == 
SOC_VRF_MAX(l2)+1){l11 = 0;if(l144(l2)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l2,l155);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l155);}}else{l11 = 2;if(l144(l2)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l2,l155);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l155);}}l24 = (l34)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l206 = ((l34)?((uint32*)&(l195)):((uint32*)&(l193)));l214
= ((l34)?((uint32*)&(l196)):((uint32*)&(l194)));l153 = l150(l2,l8,prefix,&l40
,&l26);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_th_alpm_insert: prefix create failed\n")));return l153;}sal_memset(&l13
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l2,l8,l34,l11,0,
&l13));if(bucket_index == 0){l153 = l157(l2,prefix,l40,l34,l28,&l158,&l53,&
bucket_index);SOC_IF_ERROR_RETURN(l153);if(l158 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,"_soc_th_alpm_insert: "
"Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l153 = 
_soc_th_alpm_insert_in_bkt(l2,l24,bucket_index,l155,l156,l15,&l138,l34);if(
l153 == SOC_E_NONE){*l16 = l138;if(SOC_URPF_STATUS_GET(l2)){l192 = 
soc_mem_write(l2,l24,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l2,l138),l164);if(
SOC_FAILURE(l192)){return l192;}}}if(l153 == SOC_E_FULL){l220 = 1;}l54 = 
ALPM_TCAM_PIVOT(l2,l53);trie = PIVOT_BUCKET_TRIE(l54);l217 = l54;l207 = 
sal_alloc(sizeof(payload_t),"Payload for Key");if(l207 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 payload entry\n")));
return SOC_E_MEMORY;}l221 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l221 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l2,"soc_alpm_vrf_add: unable to allocate memory for "
"pfx trie node\n")));sal_free(l207);return SOC_E_MEMORY;}sal_memset(l207,0,
sizeof(*l207));sal_memset(l221,0,sizeof(*l221));l207->key[0] = prefix[0];l207
->key[1] = prefix[1];l207->key[2] = prefix[2];l207->key[3] = prefix[3];l207->
key[4] = prefix[4];l207->len = l40;l207->index = l138;sal_memcpy(l221,l207,
sizeof(*l207));l221->bkt_ptr = l207;l153 = trie_insert(trie,prefix,NULL,l40,(
trie_node_t*)l207);if(SOC_FAILURE(l153)){if(l207!= NULL){sal_free(l207);}if(
l221!= NULL){sal_free(l221);}return l153;}if(l34){l38 = VRF_PREFIX_TRIE_IPV6(
l2,l28);}else{l38 = VRF_PREFIX_TRIE_IPV4(l2,l28);}if(!l26){l153 = trie_insert
(l38,prefix,NULL,l40,(trie_node_t*)l221);}else{l160 = NULL;l153 = 
trie_find_lpm(l38,0,0,&l160);l212 = (payload_t*)l160;if(SOC_SUCCESS(l153)){
l212->bkt_ptr = l207;}}l219 = l40;if(SOC_FAILURE(l153)){l204 = NULL;(void)
trie_delete(trie,prefix,l219,&l204);l212 = (payload_t*)l204;sal_free(l212);
sal_free(l221);return l153;}if(l220){l37.l2 = l2;l37.l8 = l8;l37.l211 = l207;
l37.l54 = l54;l37.l156 = l156;l37.l164 = l164;l37.bucket_index = bucket_index
;l153 = l44(&l37,&l45,l16);if(l153!= SOC_E_NONE){return l153;}l54 = l37.l54;
bucket_index = l37.bucket_index;}VRF_TRIE_ROUTES_INC(l2,l28,l34);if(l26){
sal_free(l221);}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l2,DELETE_PROPAGATE,&
l13,TRUE,&l158,&l53,&bucket_index));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(
l2,INSERT_PROPAGATE,&l13,FALSE,&l158,&l53,&bucket_index));if(
SOC_URPF_STATUS_GET(l2)){l40 = soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&
l13,DB_TYPEf);l40+= 1;soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,
DB_TYPEf,l40);SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l2,DELETE_PROPAGATE,&
l13,TRUE,&l158,&l53,&bucket_index));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(
l2,INSERT_PROPAGATE,&l13,FALSE,&l158,&l53,&bucket_index));}
PIVOT_BUCKET_ENT_CNT_UPDATE(l217);return l153;}static int l30(int l21,uint32*
key,int len,int l28,int l10,defip_entry_t*lpm_entry,int l31,int l32){uint32
l222;if(l32){sal_memset(lpm_entry,0,sizeof(defip_entry_t));}
soc_L3_DEFIPm_field32_set(l21,lpm_entry,VRF_ID_0f,l28&SOC_VRF_MAX(l21));if(
l28 == (SOC_VRF_MAX(l21)+1)){soc_L3_DEFIPm_field32_set(l21,lpm_entry,
VRF_ID_MASK0f,0);}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,VRF_ID_MASK0f,
SOC_VRF_MAX(l21));}if(l10){soc_L3_DEFIPm_field32_set(l21,lpm_entry,IP_ADDR0f,
key[0]);soc_L3_DEFIPm_field32_set(l21,lpm_entry,IP_ADDR1f,key[1]);
soc_L3_DEFIPm_field32_set(l21,lpm_entry,MODE0f,1);soc_L3_DEFIPm_field32_set(
l21,lpm_entry,MODE1f,1);soc_L3_DEFIPm_field32_set(l21,lpm_entry,VRF_ID_1f,l28
&SOC_VRF_MAX(l21));if(l28 == (SOC_VRF_MAX(l21)+1)){soc_L3_DEFIPm_field32_set(
l21,lpm_entry,VRF_ID_MASK1f,0);}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,
VRF_ID_MASK1f,SOC_VRF_MAX(l21));}soc_L3_DEFIPm_field32_set(l21,lpm_entry,
VALID1f,1);if(len>= 32){l222 = 0xffffffff;soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,IP_ADDR_MASK1f,l222);l222 = ~(((len-32) == 32)?0:(0xffffffff)>>(len
-32));soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l222);}else{
l222 = ~(0xffffffff>>len);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l222);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,0);}}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,IP_ADDR0f,
key[0]);assert(len<= 32);l222 = (len == 32)?0xffffffff:~(0xffffffff>>len);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l222);}
soc_L3_DEFIPm_field32_set(l21,lpm_entry,VALID0f,1);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,MODE_MASK0f,(1<<soc_mem_field_length(l21,L3_DEFIPm,
MODE_MASK0f))-1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE_MASK1f,(1<<
soc_mem_field_length(l21,L3_DEFIPm,MODE_MASK1f))-1);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,(1<<soc_mem_field_length(l21,L3_DEFIPm,
ENTRY_TYPE_MASK0f))-1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
ENTRY_TYPE_MASK1f,(1<<soc_mem_field_length(l21,L3_DEFIPm,ENTRY_TYPE_MASK1f))-
1);return(SOC_E_NONE);}int _soc_th_alpm_delete_in_bkt(int l2,soc_mem_t l24,
int l223,int l155,void*l224,uint32*l15,int*l138,int l34){int l153;l153 = 
soc_mem_alpm_delete(l2,l24,l223,MEM_BLOCK_ALL,l155,l224,l15,l138);if(
SOC_SUCCESS(l153)){return l153;}if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_delete(l2,l24,l223+1,MEM_BLOCK_ALL,l155,l224,l15,l138);}return
l153;}static int l225(int l2,void*l8,int bucket_index,int l53,int l138){
alpm_pivot_t*l54;defip_alpm_ipv4_entry_t l193,l194;defip_alpm_ipv6_64_entry_t
l195,l196;defip_alpm_ipv4_entry_t l226,l227;defip_aux_scratch_entry_t l13;
uint32 l15[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l24;void*l206,*l224,*l214 = 
NULL;int l162;int l10;int l153 = SOC_E_NONE,l192;uint32 l228[5],prefix[5];int
l34,l28;uint32 l40;int l223;uint32 l11,l155;int l158,l26 = 0;trie_t*trie,*l38
;uint32 l229;defip_entry_t lpm_entry,*l230;payload_t*l207 = NULL,*l231 = NULL
,*l212 = NULL;trie_node_t*l204 = NULL,*l160 = NULL;trie_t*l159 = NULL;l10 = 
l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l2,l8,&l162,&l28));if(l162!= 
SOC_L3_VRF_OVERRIDE){if(l28 == SOC_VRF_MAX(l2)+1){l11 = 0;if(l144(l2)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l2,l155);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l155);}}else{l11 = 2;if(l144(l2)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l2,l155);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l155);}}l153 = l150(l2,l8,prefix,&l40,&l26);
if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_delete: prefix create failed\n")));return l153;}if(!
soc_th_alpm_mode_get(l2)){if(l162!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT
(l2,l28,l34)>1){if(l26){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"VRF %d: Cannot delete default ""route if other routes are present in "
"this mode"),l28));return SOC_E_PARAM;}}}l11 = 2;}l24 = (l34)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l206 = ((l34)?((uint32*)&(l195)):(
(uint32*)&(l193)));l224 = ((l34)?((uint32*)&(l227)):((uint32*)&(l226)));
SOC_ALPM_LPM_LOCK(l2);if(bucket_index == 0){l153 = l161(l2,l8,l24,l206,&l53,&
bucket_index,&l138);}else{l153 = l20(l2,l8,l206,0,l24,0,0);}sal_memcpy(l224,
l206,l34?sizeof(l195):sizeof(l193));if(SOC_FAILURE(l153)){SOC_ALPM_LPM_UNLOCK
(l2);LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_delete: Unable to find prefix for delete\n")));return l153;}l223 = 
bucket_index;l54 = ALPM_TCAM_PIVOT(l2,l53);trie = PIVOT_BUCKET_TRIE(l54);l153
= trie_delete(trie,prefix,l40,&l204);l207 = (payload_t*)l204;if(l153!= 
SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l2);return l153;}if(l34){l38 = VRF_PREFIX_TRIE_IPV6(l2,
l28);}else{l38 = VRF_PREFIX_TRIE_IPV4(l2,l28);}if(l34){l159 = 
VRF_PIVOT_TRIE_IPV6(l2,l28);}else{l159 = VRF_PIVOT_TRIE_IPV4(l2,l28);}if(!l26
){l153 = trie_delete(l38,prefix,l40,&l204);l231 = (payload_t*)l204;if(
SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l232;}l160 = NULL;l153 = trie_find_lpm(l38,prefix,l40,&
l160);l212 = (payload_t*)l160;if(SOC_SUCCESS(l153)){payload_t*l233 = (
payload_t*)(l212->bkt_ptr);if(l233!= NULL){l229 = l233->len;}else{l229 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l234;}
sal_memcpy(l228,prefix,sizeof(prefix));do{if(!(l34)){l228[0] = (((32-l40) == 
32)?0:(l228[1])<<(32-l40));l228[1] = 0;}else{int l215 = 64-l40;int l216;if(
l215<32){l216 = l228[3]<<l215;l216|= (((32-l215) == 32)?0:(l228[4])>>(32-l215
));l228[0] = l228[4]<<l215;l228[1] = l216;l228[2] = l228[3] = l228[4] = 0;}
else{l228[1] = (((l215-32) == 32)?0:(l228[4])<<(l215-32));l228[0] = l228[2] = 
l228[3] = l228[4] = 0;}}}while(0);l153 = l30(l2,prefix,l229,l28,l10,&
lpm_entry,0,1);l192 = l161(l2,&lpm_entry,l24,l206,&l53,&bucket_index,&l138);(
void)l27(l2,l206,l24,l10,l162,bucket_index,0,&lpm_entry);(void)l30(l2,l228,
l40,l28,l10,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l2)){if(SOC_SUCCESS(l153))
{l214 = ((l34)?((uint32*)&(l196)):((uint32*)&(l194)));l192 = soc_mem_read(l2,
l24,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l2,l138),l214);}}if((l229 == 0)&&
SOC_FAILURE(l192)){l230 = l34?VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);sal_memcpy(&lpm_entry,l230,sizeof(
lpm_entry));l153 = l30(l2,l228,l40,l28,l10,&lpm_entry,0,1);}if(SOC_FAILURE(
l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l234;}l230 = 
&lpm_entry;}else{l160 = NULL;l153 = trie_find_lpm(l38,prefix,l40,&l160);l212 = 
(payload_t*)l160;if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l2,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),l28));goto l232;}l212->bkt_ptr = NULL;l229 = 
0;l230 = l34?VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28):VRF_TRIE_DEFAULT_ROUTE_IPV4(
l2,l28);}l153 = l9(l2,l230,l34,l11,l229,&l13);if(SOC_FAILURE(l153)){goto l234
;}l153 = _soc_th_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,TRUE,&l158,&l53,&
bucket_index);if(SOC_FAILURE(l153)){goto l234;}if(SOC_URPF_STATUS_GET(l2)){
uint32 l149;if(l214!= NULL){l149 = soc_mem_field32_get(l2,
L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l149++;soc_mem_field32_set(l2,
L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l149);l149 = soc_mem_field32_get(l2,l24,
l214,SRC_DISCARDf);soc_mem_field32_set(l2,l24,&l13,SRC_DISCARDf,l149);l149 = 
soc_mem_field32_get(l2,l24,l214,DEFAULTROUTEf);soc_mem_field32_set(l2,l24,&
l13,DEFAULTROUTEf,l149);l153 = _soc_th_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,
TRUE,&l158,&l53,&bucket_index);}if(SOC_FAILURE(l153)){goto l234;}}sal_free(
l207);if(!l26){sal_free(l231);}PIVOT_BUCKET_ENT_CNT_UPDATE(l54);if((l54->len
!= 0)&&(trie->trie == NULL)){uint32 l235[5];sal_memcpy(l235,l54->key,sizeof(
l235));do{if(!(l10)){l235[0] = (((32-l54->len) == 32)?0:(l235[1])<<(32-l54->
len));l235[1] = 0;}else{int l215 = 64-l54->len;int l216;if(l215<32){l216 = 
l235[3]<<l215;l216|= (((32-l215) == 32)?0:(l235[4])>>(32-l215));l235[0] = 
l235[4]<<l215;l235[1] = l216;l235[2] = l235[3] = l235[4] = 0;}else{l235[1] = 
(((l215-32) == 32)?0:(l235[4])<<(l215-32));l235[0] = l235[2] = l235[3] = l235
[4] = 0;}}}while(0);l30(l2,l235,l54->len,l28,l10,&lpm_entry,0,1);l153 = l7(l2
,&lpm_entry);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_delete: Unable to ""delete pivot 0x%08x 0x%08x \n"),l54->key[0],
l54->key[1]));}}l153 = _soc_th_alpm_delete_in_bkt(l2,l24,l223,l155,l224,l15,&
l138,l34);if(!SOC_SUCCESS(l153)){SOC_ALPM_LPM_UNLOCK(l2);l153 = SOC_E_FAIL;
return l153;}if(SOC_URPF_STATUS_GET(l2)){l153 = soc_mem_alpm_delete(l2,l24,
SOC_ALPM_RPF_BKT_IDX(l2,l223),MEM_BLOCK_ALL,l155,l224,l15,&l158);if(!
SOC_SUCCESS(l153)){SOC_ALPM_LPM_UNLOCK(l2);l153 = SOC_E_FAIL;return l153;}}if
((l54->len!= 0)&&(trie->trie == NULL)){l153 = alpm_bucket_release(l2,
PIVOT_BUCKET_INDEX(l54),l34);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l2,"_soc_alpm_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(l54)));}l153 = trie_delete(l159,l54->key,l54->len,&l204);
if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l54));sal_free(PIVOT_BUCKET_HANDLE(l54));sal_free(l54);
_soc_trident2_alpm_bkt_view_set(l2,l223<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l2,l34)){_soc_trident2_alpm_bkt_view_set(l2,(l223+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l2,l28,l34);if(VRF_TRIE_ROUTES_CNT(l2,
l28,l34) == 0){l153 = l33(l2,l28,l34);}SOC_ALPM_LPM_UNLOCK(l2);return l153;
l234:LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"recovering soc_th_alpm_vrf_delete failed\n ")));l192 = trie_insert(l38,
prefix,NULL,l40,(trie_node_t*)l231);if(SOC_FAILURE(l192)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into pfx trie\n"),prefix[0],prefix[1]));}l232:l192 = 
trie_insert(trie,prefix,NULL,l40,(trie_node_t*)l207);if(SOC_FAILURE(l192)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into bkt trie\n")
,prefix[0],prefix[1]));}SOC_ALPM_LPM_UNLOCK(l2);return l153;}int
soc_th_alpm_init(int l2){int l181;int l153 = SOC_E_NONE;l153 = l3(l2);
SOC_IF_ERROR_RETURN(l153);l153 = l19(l2);alpm_vrf_handle[l2] = sal_alloc((
MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t),"ALPM VRF Handles");if(
alpm_vrf_handle[l2] == NULL){return SOC_E_MEMORY;}tcam_pivot[l2] = sal_alloc(
MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*),"ALPM pivots");if(tcam_pivot[l2] == 
NULL){return SOC_E_MEMORY;}sal_memset(alpm_vrf_handle[l2],0,(MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t));sal_memset(tcam_pivot[l2],0,MAX_PIVOT_COUNT*sizeof
(alpm_pivot_t*));for(l181 = 0;l181<MAX_PIVOT_COUNT;l181++){ALPM_TCAM_PIVOT(l2
,l181) = NULL;}if(SOC_CONTROL(l2)->alpm_bulk_retry == NULL){SOC_CONTROL(l2)->
alpm_bulk_retry = sal_sem_create("ALPM bulk retry",sal_sem_BINARY,0);}if(
SOC_CONTROL(l2)->alpm_lookup_retry == NULL){SOC_CONTROL(l2)->
alpm_lookup_retry = sal_sem_create("ALPM lookup retry",sal_sem_BINARY,0);}if(
SOC_CONTROL(l2)->alpm_insert_retry == NULL){SOC_CONTROL(l2)->
alpm_insert_retry = sal_sem_create("ALPM insert retry",sal_sem_BINARY,0);}if(
SOC_CONTROL(l2)->alpm_delete_retry == NULL){SOC_CONTROL(l2)->
alpm_delete_retry = sal_sem_create("ALPM delete retry",sal_sem_BINARY,0);}
l153 = soc_th_alpm_128_lpm_init(l2);SOC_IF_ERROR_RETURN(l153);return l153;}
static int l236(int l2){int l181,l153;alpm_pivot_t*l149;for(l181 = 0;l181<
MAX_PIVOT_COUNT;l181++){l149 = ALPM_TCAM_PIVOT(l2,l181);if(l149){if(
PIVOT_BUCKET_HANDLE(l149)){if(PIVOT_BUCKET_TRIE(l149)){l153 = trie_traverse(
PIVOT_BUCKET_TRIE(l149),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l153)){trie_destroy(PIVOT_BUCKET_TRIE(l149));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,"Unable to clear trie state for unit %d\n"),l2
));return l153;}}sal_free(PIVOT_BUCKET_HANDLE(l149));}sal_free(
ALPM_TCAM_PIVOT(l2,l181));ALPM_TCAM_PIVOT(l2,l181) = NULL;}}for(l181 = 0;l181
<= SOC_VRF_MAX(l2)+1;l181++){l153 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l2,
l181),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l153)){
trie_destroy(VRF_PREFIX_TRIE_IPV4(l2,l181));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l2,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l2,l181));
return l153;}l153 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l2,l181),
alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l153)){
trie_destroy(VRF_PREFIX_TRIE_IPV6(l2,l181));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l2,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l2,l181));
return l153;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l181)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l181));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l181
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l181));}sal_memset(&
alpm_vrf_handle[l2][l181],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l2][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l2,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l2,MAX_VRF_ID,1,1);
VRF_TRIE_INIT_DONE(l2,MAX_VRF_ID,2,1);if(SOC_ALPM_BUCKET_BMAP(l2)!= NULL){
sal_free(SOC_ALPM_BUCKET_BMAP(l2));}sal_memset(&soc_th_alpm_bucket[l2],0,
sizeof(soc_alpm_bucket_t));if(alpm_vrf_handle[l2])sal_free(alpm_vrf_handle[l2
]);if(tcam_pivot[l2])sal_free(tcam_pivot[l2]);alpm_vrf_handle[l2] = NULL;
tcam_pivot[l2] = NULL;return SOC_E_NONE;}int soc_th_alpm_deinit(int l2){l4(l2
);SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_deinit(l2));SOC_IF_ERROR_RETURN(
soc_th_alpm_128_state_clear(l2));SOC_IF_ERROR_RETURN(l236(l2));if(SOC_CONTROL
(l2)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l2)->alpm_bulk_retry);
SOC_CONTROL(l2)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l2)->
alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l2)->alpm_lookup_retry
);SOC_CONTROL(l2)->alpm_lookup_retry = NULL;}if(SOC_CONTROL(l2)->
alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l2)->alpm_insert_retry
);SOC_CONTROL(l2)->alpm_insert_retry = NULL;}if(SOC_CONTROL(l2)->
alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l2)->alpm_delete_retry
);SOC_CONTROL(l2)->alpm_delete_retry = NULL;}return SOC_E_NONE;}static int
l237(int l2,int l28,int l34){defip_entry_t*lpm_entry,l238;int l239;int index;
int l153 = SOC_E_NONE;uint32 key[2] = {0,0};uint32 l40;alpm_bucket_handle_t*
l208;alpm_pivot_t*l54;payload_t*l231;trie_t*l240;trie_t*l241 = NULL;if(l34 == 
0){trie_init(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(l2,l28));l241 = 
VRF_PIVOT_TRIE_IPV4(l2,l28);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l2,l28));l241 = VRF_PIVOT_TRIE_IPV6(l2,l28);}if(l34 == 0)
{trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l2,l28));l240 = 
VRF_PREFIX_TRIE_IPV4(l2,l28);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6(l2,l28));l240 = VRF_PREFIX_TRIE_IPV6(l2,l28);}lpm_entry = 
sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n")));
return SOC_E_MEMORY;}l30(l2,key,0,l28,l34,lpm_entry,0,1);if(l34 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28) = lpm_entry;}if(l28 == SOC_VRF_MAX(l2)+1)
{soc_L3_DEFIPm_field32_set(l2,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l2,lpm_entry,DEFAULT_MISS0f,1);}l153 = 
alpm_bucket_assign(l2,&l239,l34);soc_L3_DEFIPm_field32_set(l2,lpm_entry,
ALG_BKT_PTR0f,l239);sal_memcpy(&l238,lpm_entry,sizeof(l238));l153 = l5(l2,&
l238,&index);l208 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l208 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l2,"soc_th_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")
));return SOC_E_NONE;}sal_memset(l208,0,sizeof(*l208));l54 = sal_alloc(sizeof
(alpm_pivot_t),"Payload for Pivot");if(l54 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l2,"soc_alpm_vrf_add: Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l208);return SOC_E_MEMORY;}l231 = sal_alloc(
sizeof(payload_t),"Payload for pfx trie key");if(l231 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n")));
sal_free(l208);sal_free(l54);return SOC_E_MEMORY;}sal_memset(l54,0,sizeof(*
l54));sal_memset(l231,0,sizeof(*l231));l40 = 0;PIVOT_BUCKET_HANDLE(l54) = 
l208;if(l34){trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l54));}else{
trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l54));}PIVOT_BUCKET_INDEX(l54) = 
l239;PIVOT_BUCKET_VRF(l54) = l28;PIVOT_BUCKET_IPV6(l54) = l34;
PIVOT_BUCKET_DEF(l54) = TRUE;l54->key[0] = l231->key[0] = key[0];l54->key[1] = 
l231->key[1] = key[1];l54->len = l231->len = l40;l153 = trie_insert(l240,key,
NULL,l40,&(l231->node));if(SOC_FAILURE(l153)){sal_free(l231);sal_free(l54);
sal_free(l208);return l153;}l153 = trie_insert(l241,key,NULL,l40,(trie_node_t
*)l54);if(SOC_FAILURE(l153)){trie_node_t*l204 = NULL;(void)trie_delete(l240,
key,l40,&l204);sal_free(l231);sal_free(l54);sal_free(l208);return l153;}index
= soc_th_alpm_physical_idx(l2,L3_DEFIPm,index,l34);if(l34 == 0){
ALPM_TCAM_PIVOT(l2,index) = l54;PIVOT_TCAM_INDEX(l54) = index;}else{
ALPM_TCAM_PIVOT(l2,index<<1) = l54;PIVOT_TCAM_INDEX(l54) = index<<1;}
VRF_PIVOT_REF_INC(l2,l28,l34);VRF_TRIE_INIT_DONE(l2,l28,l34,1);return l153;}
static int l33(int l2,int l28,int l34){defip_entry_t*lpm_entry;int l239;int
l146;int l153 = SOC_E_NONE;uint32 key[2] = {0,0},l242[SOC_MAX_MEM_FIELD_WORDS
];payload_t*l207;alpm_pivot_t*l243;trie_node_t*l204;trie_t*l240;trie_t*l241 = 
NULL;if(l34 == 0){lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);}else{
lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28);}l239 = 
soc_L3_DEFIPm_field32_get(l2,lpm_entry,ALG_BKT_PTR0f);l153 = 
alpm_bucket_release(l2,l239,l34);_soc_trident2_alpm_bkt_view_set(l2,l239<<2,
INVALIDm);l153 = l18(l2,lpm_entry,(void*)l242,&l146);if(SOC_FAILURE(l153)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n"),
l28,l34));l146 = -1;}l146 = soc_th_alpm_physical_idx(l2,L3_DEFIPm,l146,l34);
if(l34 == 0){l243 = ALPM_TCAM_PIVOT(l2,l146);}else{l243 = ALPM_TCAM_PIVOT(l2,
l146<<1);}l153 = l7(l2,lpm_entry);if(SOC_FAILURE(l153)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),l28,l34));}sal_free(lpm_entry);if(
l34 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28) = NULL;l240 = 
VRF_PREFIX_TRIE_IPV4(l2,l28);VRF_PREFIX_TRIE_IPV4(l2,l28) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28) = NULL;l240 = VRF_PREFIX_TRIE_IPV6(l2,l28
);VRF_PREFIX_TRIE_IPV6(l2,l28) = NULL;}VRF_TRIE_INIT_DONE(l2,l28,l34,0);l153 = 
trie_delete(l240,key,0,&l204);l207 = (payload_t*)l204;if(SOC_FAILURE(l153)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"Unable to delete internal default for vrf "" %d/%d\n"),l28,l34));}sal_free(
l207);(void)trie_destroy(l240);if(l34 == 0){l241 = VRF_PIVOT_TRIE_IPV4(l2,l28
);VRF_PIVOT_TRIE_IPV4(l2,l28) = NULL;}else{l241 = VRF_PIVOT_TRIE_IPV6(l2,l28)
;VRF_PIVOT_TRIE_IPV6(l2,l28) = NULL;}l204 = NULL;l153 = trie_delete(l241,key,
0,&l204);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l28,l34));}(void)
trie_destroy(l241);sal_free(PIVOT_BUCKET_HANDLE(l243));(void)trie_destroy(
PIVOT_BUCKET_TRIE(l243));sal_free(l243);return l153;}int soc_th_alpm_insert(
int l2,void*l6,uint32 l25,int l244,int l245){defip_alpm_ipv4_entry_t l193,
l194;defip_alpm_ipv6_64_entry_t l195,l196;soc_mem_t l24;void*l206,*l224;int
l162,l28;int index;int l10;int l153 = SOC_E_NONE;uint32 l26;l10 = 
soc_mem_field32_get(l2,L3_DEFIPm,l6,MODE0f);l24 = (l10)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l206 = ((l10)?((uint32*)&(l195)):(
(uint32*)&(l193)));l224 = ((l10)?((uint32*)&(l196)):((uint32*)&(l194)));
SOC_IF_ERROR_RETURN(l20(l2,l6,l206,l224,l24,l25,&l26));SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l2,l6,&l162,&l28));if(((l162 == SOC_L3_VRF_OVERRIDE)
||((soc_th_alpm_mode_get(l2) == SOC_ALPM_MODE_TCAM_ALPM)&&(l162 == 
SOC_L3_VRF_GLOBAL)))){l153 = l5(l2,l6,&index);if(SOC_SUCCESS(l153)){
VRF_PIVOT_REF_INC(l2,MAX_VRF_ID,l10);VRF_TRIE_ROUTES_INC(l2,MAX_VRF_ID,l10);}
else if(l153 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l2,MAX_VRF_ID,l10);}return(
l153);}else if(l28 == 0){if(soc_th_alpm_mode_get(l2)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"Unit %d, VRF=0 cannot be added in Parallel mode\n"),l2));return SOC_E_PARAM;
}}if(l162!= SOC_L3_VRF_GLOBAL){if(!soc_th_alpm_mode_get(l2)){if(
VRF_TRIE_ROUTES_CNT(l2,l28,l10) == 0){if(!l26){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l2,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n"),l162));return SOC_E_PARAM;}}}}if(!
VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l2,"soc_alpm_insert:VRF %d is not ""initialized\n"),l28));l153 = l237(l2,l28,
l10);if(SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n"),l28,l10));return l153;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),l28,l10));}if(l245&
SOC_ALPM_LOOKUP_HIT){l153 = l163(l2,l6,l206,l224,l24,l244);}else{if(l244 == -
1){l244 = 0;}l153 = l218(l2,l6,l24,l206,l224,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l244),l245);}if(l153!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l2,soc_errmsg(l153)
));}return(l153);}int soc_th_alpm_lookup(int l2,void*l8,void*l15,int*l16,int*
l246){defip_alpm_ipv4_entry_t l193;defip_alpm_ipv6_64_entry_t l195;soc_mem_t
l24;int bucket_index;int l53;void*l206;int l162,l28;int l10,l137;int l153 = 
SOC_E_NONE;l10 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l2,l8,&l162,&l28));l153 = l14(l2,
l8,l15,l16,&l137,&l10);if(SOC_SUCCESS(l153)){if(!l10&&(*l16&0x1)){l153 = 
soc_th_alpm_lpm_ip4entry1_to_0(l2,l15,l15,PRESERVE_HIT);}SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l2,l15,&l162,&l28));if(l162 == SOC_L3_VRF_OVERRIDE){
return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,"soc_alpm_lookup:VRF %d is not initialized\n")
,l28));*l246 = 0;return SOC_E_NOT_FOUND;}l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l206 = ((l10)?((uint32*)&(l195)):((uint32*)&(l193)));
SOC_ALPM_LPM_LOCK(l2);l153 = l161(l2,l8,l24,l206,&l53,&bucket_index,l16);
SOC_ALPM_LPM_UNLOCK(l2);if(SOC_FAILURE(l153)){*l246 = l53;*l16 = bucket_index
<<2;return l153;}l153 = l27(l2,l206,l24,l10,l162,bucket_index,*l16,l15);*l246
= SOC_ALPM_LOOKUP_HIT|l53;return(l153);}int soc_th_alpm_delete(int l2,void*l8
,int l244,int l245){int l162,l28;int l10;int l153 = SOC_E_NONE;l10 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l2,l8,&l162,&l28));if(((l162 == SOC_L3_VRF_OVERRIDE)
||((soc_th_alpm_mode_get(l2) == SOC_ALPM_MODE_TCAM_ALPM)&&(l162 == 
SOC_L3_VRF_GLOBAL)))){l153 = l7(l2,l8);if(SOC_SUCCESS(l153)){
VRF_PIVOT_REF_DEC(l2,MAX_VRF_ID,l10);VRF_TRIE_ROUTES_DEC(l2,MAX_VRF_ID,l10);}
return(l153);}else{if(!VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"soc_alpm_delete:VRF %d/%d is not initialized\n"),l28,l10));return SOC_E_NONE
;}if(l244 == -1){l244 = 0;}l153 = l225(l2,l8,SOC_ALPM_BKT_ENTRY_TO_IDX(l244),
l245&~SOC_ALPM_LOOKUP_HIT,l244);}return(l153);}static int l19(int l2){int l247
;l247 = soc_mem_index_count(l2,L3_DEFIPm)+soc_mem_index_count(l2,
L3_DEFIP_PAIR_128m)*2;if(SOC_URPF_STATUS_GET(l2)){l247>>= 1;}
SOC_ALPM_BUCKET_COUNT(l2) = l247*2;SOC_ALPM_BUCKET_BMAP_SIZE(l2) = 
SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(l2));SOC_ALPM_BUCKET_BMAP(l2) = 
sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l2),"alpm_shared_bucket_bitmap");if(
SOC_ALPM_BUCKET_BMAP(l2) == NULL){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"soc_alpm_shared_mem_init: Memory allocation for "
"bucket bitmap management failed\n")));return SOC_E_MEMORY;}sal_memset(
SOC_ALPM_BUCKET_BMAP(l2),0,SOC_ALPM_BUCKET_BMAP_SIZE(l2));alpm_bucket_assign(
l2,&l247,1);return SOC_E_NONE;}static void l125(int l2,void*l15,int index,
l120 l126){if(index&(0x8000)){l126[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(l2)]->l79));l126[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l81));l126[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l113[(l2)]->l80));l126[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l82));if((!(SOC_IS_HURRICANE(l2)))&&(((l113[(l2)]->l95)!= NULL))){int
l248;(void)soc_th_alpm_lpm_vrf_get(l2,l15,(int*)&l126[4],&l248);}else{l126[4]
= 0;};}else{if(index&0x1){l126[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(l2)]->l80));l126[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l82));l126[2] = 0;l126[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&((
(l113[(l2)]->l96)!= NULL))){int l248;defip_entry_t l249;(void)
soc_th_alpm_lpm_ip4entry1_to_0(l2,l15,&l249,0);(void)soc_th_alpm_lpm_vrf_get(
l2,&l249,(int*)&l126[4],&l248);}else{l126[4] = 0;};}else{l126[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l79));l126[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l113[(l2)]->l81));l126[2] = 0;l126[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l2)))&&(((l113[(l2)]->l95)!= NULL))){int l248;(void)
soc_th_alpm_lpm_vrf_get(l2,l15,(int*)&l126[4],&l248);}else{l126[4] = 0;};}}}
static int l250(l120 l122,l120 l123){int l146;for(l146 = 0;l146<5;l146++){{if
((l122[l146])<(l123[l146])){return-1;}if((l122[l146])>(l123[l146])){return 1;
}};}return(0);}static void l251(int l2,void*l6,uint32 l252,uint32 l140,int
l137){l120 l253;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l113[(l2)]->l83))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l113[(l2)]->l94))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l113[(
l2)]->l93))){l253[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l113[(l2)]->l79));l253[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l113[(
l2)]->l81));l253[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l113[(l2)]->l80));l253[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l113[(
l2)]->l82));if((!(SOC_IS_HURRICANE(l2)))&&(((l113[(l2)]->l95)!= NULL))){int
l248;(void)soc_th_alpm_lpm_vrf_get(l2,l6,(int*)&l253[4],&l248);}else{l253[4] = 
0;};l139((l124[(l2)]),l250,l253,l137,l140,((uint16)l252<<1)|(0x8000));}}else{
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l113
[(l2)]->l93))){l253[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l113[(l2)]->l79));l253[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l113[(
l2)]->l81));l253[2] = 0;l253[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&((
(l113[(l2)]->l95)!= NULL))){int l248;(void)soc_th_alpm_lpm_vrf_get(l2,l6,(int
*)&l253[4],&l248);}else{l253[4] = 0;};l139((l124[(l2)]),l250,l253,l137,l140,(
(uint16)l252<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l113[(l2)]->l94))){l253[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l113[(
l2)]->l80));l253[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l113[(l2)]->l82));l253[2] = 0;l253[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l2)))&&(((l113[(l2)]->l96)!= NULL))){int l248;defip_entry_t
l249;(void)soc_th_alpm_lpm_ip4entry1_to_0(l2,l6,&l249,0);(void)
soc_th_alpm_lpm_vrf_get(l2,&l249,(int*)&l253[4],&l248);}else{l253[4] = 0;};
l139((l124[(l2)]),l250,l253,l137,l140,(((uint16)l252<<1)+1));}}}static void
l254(int l2,void*l8,uint32 l252){l120 l253;int l137 = -1;int l153;uint16 index
;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(
l113[(l2)]->l83))){l253[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l8),(l113[(l2)]->l79));l253[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(
l2)]->l81));l253[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l113[(l2)]->l80));l253[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(
l2)]->l82));if((!(SOC_IS_HURRICANE(l2)))&&(((l113[(l2)]->l95)!= NULL))){int
l248;(void)soc_th_alpm_lpm_vrf_get(l2,l8,(int*)&l253[4],&l248);}else{l253[4] = 
0;};index = (l252<<1)|(0x8000);}else{l253[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(
l2)]->l79));l253[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l113[(l2)]->l81));l253[2] = 0;l253[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l2)))&&(((l113[(l2)]->l95)!= NULL))){int l248;(void)
soc_th_alpm_lpm_vrf_get(l2,l8,(int*)&l253[4],&l248);}else{l253[4] = 0;};index
= l252;}l153 = l141((l124[(l2)]),l250,l253,l137,index);if(SOC_FAILURE(l153)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,"\ndel  index: H %d error %d\n"),
index,l153));}}static int l255(int l2,void*l8,int l137,int*l138){l120 l253;
int l256;int l153;uint16 index = (0xFFFF);l256 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(
l2)]->l83));if(l256){l253[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(l2)]->l79));l253[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(
l2)]->l81));l253[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l113[(l2)]->l80));l253[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(
l2)]->l82));if((!(SOC_IS_HURRICANE(l2)))&&(((l113[(l2)]->l95)!= NULL))){int
l248;(void)soc_th_alpm_lpm_vrf_get(l2,l8,(int*)&l253[4],&l248);}else{l253[4] = 
0;};}else{l253[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l113[(l2)]->l79));l253[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(
l2)]->l81));l253[2] = 0;l253[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&((
(l113[(l2)]->l95)!= NULL))){int l248;(void)soc_th_alpm_lpm_vrf_get(l2,l8,(int
*)&l253[4],&l248);}else{l253[4] = 0;};}l153 = l134((l124[(l2)]),l250,l253,
l137,&index);if(SOC_FAILURE(l153)){*l138 = 0xFFFFFFFF;return(l153);}*l138 = 
index;return(SOC_E_NONE);}static uint16 l127(uint8*l128,int l129){return(
_shr_crc16b(0,l128,l129));}static int l130(int l21,int l115,int l116,l119**
l131){l119*l135;int index;if(l116>l115){return SOC_E_MEMORY;}l135 = sal_alloc
(sizeof(l119),"lpm_hash");if(l135 == NULL){return SOC_E_MEMORY;}sal_memset(
l135,0,sizeof(*l135));l135->l21 = l21;l135->l115 = l115;l135->l116 = l116;
l135->l117 = sal_alloc(l135->l116*sizeof(*(l135->l117)),"hash_table");if(l135
->l117 == NULL){sal_free(l135);return SOC_E_MEMORY;}l135->l118 = sal_alloc(
l135->l115*sizeof(*(l135->l118)),"link_table");if(l135->l118 == NULL){
sal_free(l135->l117);sal_free(l135);return SOC_E_MEMORY;}for(index = 0;index<
l135->l116;index++){l135->l117[index] = (0xFFFF);}for(index = 0;index<l135->
l115;index++){l135->l118[index] = (0xFFFF);}*l131 = l135;return SOC_E_NONE;}
static int l132(l119*l133){if(l133!= NULL){sal_free(l133->l117);sal_free(l133
->l118);sal_free(l133);}return SOC_E_NONE;}static int l134(l119*l135,l121 l136
,l120 entry,int l137,uint16*l138){int l2 = l135->l21;uint16 l257;uint16 index
;l257 = l127((uint8*)entry,(32*5))%l135->l116;index = l135->l117[l257];;;
while(index!= (0xFFFF)){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];l120 l126;int l258
;l258 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,
MEM_BLOCK_ANY,l258,l15));l125(l2,l15,index,l126);if((*l136)(entry,l126) == 0)
{*l138 = (index&(0x7FFF))>>((index&(0x8000))?1:0);;return(SOC_E_NONE);}index = 
l135->l118[index&(0x7FFF)];;};return(SOC_E_NOT_FOUND);}static int l139(l119*
l135,l121 l136,l120 entry,int l137,uint16 l140,uint16 l49){int l2 = l135->l21
;uint16 l257;uint16 index;uint16 l259;l257 = l127((uint8*)entry,(32*5))%l135
->l116;index = l135->l117[l257];;;;l259 = (0xFFFF);if(l140!= (0xFFFF)){while(
index!= (0xFFFF)){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];l120 l126;int l258;l258
= (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,
l258,l15));l125(l2,l15,index,l126);if((*l136)(entry,l126) == 0){if(l49!= 
index){;if(l259 == (0xFFFF)){l135->l117[l257] = l49;l135->l118[l49&(0x7FFF)] = 
l135->l118[index&(0x7FFF)];l135->l118[index&(0x7FFF)] = (0xFFFF);}else{l135->
l118[l259&(0x7FFF)] = l49;l135->l118[l49&(0x7FFF)] = l135->l118[index&(0x7FFF
)];l135->l118[index&(0x7FFF)] = (0xFFFF);}};return(SOC_E_NONE);}l259 = index;
index = l135->l118[index&(0x7FFF)];;}}l135->l118[l49&(0x7FFF)] = l135->l117[
l257];l135->l117[l257] = l49;return(SOC_E_NONE);}static int l141(l119*l135,
l121 l136,l120 entry,int l137,uint16 l142){uint16 l257;uint16 index;uint16
l259;l257 = l127((uint8*)entry,(32*5))%l135->l116;index = l135->l117[l257];;;
l259 = (0xFFFF);while(index!= (0xFFFF)){if(l142 == index){;if(l259 == (0xFFFF
)){l135->l117[l257] = l135->l118[l142&(0x7FFF)];l135->l118[l142&(0x7FFF)] = (
0xFFFF);}else{l135->l118[l259&(0x7FFF)] = l135->l118[l142&(0x7FFF)];l135->
l118[l142&(0x7FFF)] = (0xFFFF);}return(SOC_E_NONE);}l259 = index;index = l135
->l118[index&(0x7FFF)];;}return(SOC_E_NOT_FOUND);}int
soc_th_alpm_lpm_ip4entry0_to_0(int l2,void*l260,void*l261,int l262){uint32
l263;l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l260),(l113[(l2)]->l93));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l261),(l113[(l2)]->l93),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l83));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l83),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l79),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l81));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l81),(l263));if(((l113[(l2)]
->l69)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l69));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l69),(l263));}if(((l113[(l2)]
->l71)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l71));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l71),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l73));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l73),(l263));}else{l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l87),(l263));}l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l89));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l89),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l91));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l91),(l263));if(((l113[(l2)]
->l95)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l95));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l95),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l97));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l97),(l263));}if(((l113[(l2)]->l67)!= NULL)){l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l67));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l67),(l263));}if(l262){l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l77),(l263));}l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l99));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l99),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l101));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l101),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l103));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm))
,(l261),(l113[(l2)]->l103),(l263));l263 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l105));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l105),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l107));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l107),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l109));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm))
,(l261),(l113[(l2)]->l109),(l263));return(SOC_E_NONE);}int
soc_th_alpm_lpm_ip4entry1_to_1(int l2,void*l260,void*l261,int l262){uint32
l263;l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l260),(l113[(l2)]->l94));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l261),(l113[(l2)]->l94),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l84));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l84),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l80),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l82));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l82),(l263));if(((l113[(l2)]
->l70)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l70));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l70),(l263));}if(((l113[(l2)]
->l72)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l72));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l72),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l74),(l263));}else{l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l88));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l88),(l263));}l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l90));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l90),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l92));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l92),(l263));if(((l113[(l2)]
->l96)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l96));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l96),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l98));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l98),(l263));}if(((l113[(l2)]->l68)!= NULL)){l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l68));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l68),(l263));}if(l262){l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l78),(l263));}l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l100));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l100),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l102));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l102),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l104));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm))
,(l261),(l113[(l2)]->l104),(l263));l263 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l106));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l106),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l108));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l108),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l110));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm))
,(l261),(l113[(l2)]->l110),(l263));return(SOC_E_NONE);}int
soc_th_alpm_lpm_ip4entry0_to_1(int l2,void*l260,void*l261,int l262){uint32
l263;l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l260),(l113[(l2)]->l93));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l261),(l113[(l2)]->l94),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l83));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l84),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l80),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l81));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l82),(l263));if(!
SOC_IS_HURRICANE(l2)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l69));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l70),(l263));}if(((l113[(l2)
]->l71)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l71));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l72),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l73));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l74),(l263));}else{l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l88),(l263));}l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l89));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l90),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l91));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l92),(l263));if(((l113[(l2)]
->l95)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l95));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l96),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l97));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l98),(l263));}if(((l113[(l2)]->l67)!= NULL)){l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l67));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l68),(l263));}if(l262){l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l78),(l263));}l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l99));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l100),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l101));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l102),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l103));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm))
,(l261),(l113[(l2)]->l104),(l263));l263 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l105));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l106),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l107));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l108),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l109));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm))
,(l261),(l113[(l2)]->l110),(l263));return(SOC_E_NONE);}int
soc_th_alpm_lpm_ip4entry1_to_0(int l2,void*l260,void*l261,int l262){uint32
l263;l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l260),(l113[(l2)]->l94));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l261),(l113[(l2)]->l93),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l84));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l83),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l79),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l82));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l81),(l263));if(!
SOC_IS_HURRICANE(l2)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l70));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l69),(l263));}if(((l113[(l2)
]->l72)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l72));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l71),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l73),(l263));}else{l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l88));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l87),(l263));}l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l90));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l89),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l92));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l91),(l263));if(((l113[(l2)]
->l96)!= NULL)){l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l260),(l113[(l2)]->l96));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l95),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l98));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l97),(l263));}if(((l113[(l2)]->l68)!= NULL)){l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l68));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l67),(l263));}if(l262){l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l261),(l113[(l2)]->l77),(l263));}l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l100));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l99),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l102));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l101),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l104));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm))
,(l261),(l113[(l2)]->l103),(l263));l263 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[(l2)]->l106));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[
(l2)]->l105),(l263));l263 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l260),(l113[(l2)]->l108));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l261),(l113[(l2)]->l107),(l263));l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l260),(l113[
(l2)]->l110));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm))
,(l261),(l113[(l2)]->l109),(l263));return(SOC_E_NONE);}static int l264(int l2
,void*l15){return(SOC_E_NONE);}void l1(int l2){int l181;int l265;l265 = ((3*(
64+32+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l2)
){return;}for(l181 = l265;l181>= 0;l181--){if((l181!= ((3*(64+32+2+1))-1))&&(
(l63[(l2)][(l181)].l56) == -1)){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l2,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l181,(l63
[(l2)][(l181)].l58),(l63[(l2)][(l181)].next),(l63[(l2)][(l181)].l56),(l63[(l2
)][(l181)].l57),(l63[(l2)][(l181)].l59),(l63[(l2)][(l181)].l60)));}
COMPILER_REFERENCE(l264);}static int l266(int l2,int index,uint32*l15){int
l267;int l10;uint32 l268;uint32 l269;int l270;if(!SOC_URPF_STATUS_GET(l2)){
return(SOC_E_NONE);}if(soc_feature(l2,soc_feature_l3_defip_hole)){l267 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l2)){l267 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1)+0x0400;}else{l267 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1);}l10 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l83));if(((l113[(l2)]->l67)!= NULL)){soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(l2)]->l67),(0));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l68),(0));}l268 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l113[(l2)]->l81));l269 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(l2)]->l82));if(!l10){if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l93))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l15),(l113[(l2)]->l91),((!l268)?1:0));}if(soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(l2)]->l94))){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l92),((!l269)?1:0));}}else{l270 = ((!l268)&&(!l269))?1:0;l268 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l93));l269 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l113[(l2)]->l94));if(l268&&l269){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l91),(l270));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l113[(l2)]->l92),(l270));}}return l175(l2,MEM_BLOCK_ANY,
index+l267,index,l15);}static int l271(int l2,int l272,int l273){uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,
l272,l15));l251(l2,l15,l273,0x4000,0);SOC_IF_ERROR_RETURN(l175(l2,
MEM_BLOCK_ANY,l273,l272,l15));SOC_IF_ERROR_RETURN(l266(l2,l273,l15));do{int
l274,l275;l274 = soc_th_alpm_physical_idx((l2),L3_DEFIPm,(l272),1);l275 = 
soc_th_alpm_physical_idx((l2),L3_DEFIPm,(l273),1);ALPM_TCAM_PIVOT(l2,l275<<1)
= ALPM_TCAM_PIVOT(l2,l274<<1);ALPM_TCAM_PIVOT(l2,(l275<<1)+1) = 
ALPM_TCAM_PIVOT(l2,(l274<<1)+1);if(ALPM_TCAM_PIVOT((l2),l275<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l2),l275<<1)) = l275<<1;}if(ALPM_TCAM_PIVOT
((l2),(l275<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l2),(l275<<1)+1)) = (
l275<<1)+1;}ALPM_TCAM_PIVOT(l2,l274<<1) = NULL;ALPM_TCAM_PIVOT(l2,(l274<<1)+1
) = NULL;}while(0);return(SOC_E_NONE);}static int l276(int l2,int l137,int l10
){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l272;int l273;uint32 l277,l278;l273
= (l63[(l2)][(l137)].l57)+1;if(!l10){l272 = (l63[(l2)][(l137)].l57);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l272,l15));l277 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l93));l278 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l113[(l2)]->l94));if((l277 == 0)||(l278 == 0)){l251(l2,l15
,l273,0x4000,0);SOC_IF_ERROR_RETURN(l175(l2,MEM_BLOCK_ANY,l273,l272,l15));
SOC_IF_ERROR_RETURN(l266(l2,l273,l15));do{int l279 = soc_th_alpm_physical_idx
((l2),L3_DEFIPm,(l272),1)<<1;int l216 = soc_th_alpm_physical_idx((l2),
L3_DEFIPm,(l273),1)*2+(l279&1);if((l278)){l279++;}ALPM_TCAM_PIVOT((l2),l216) = 
ALPM_TCAM_PIVOT((l2),l279);if(ALPM_TCAM_PIVOT((l2),l216)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l2),l216)) = l216;}ALPM_TCAM_PIVOT((l2),l279) = NULL;}while(
0);l273--;}}l272 = (l63[(l2)][(l137)].l56);if(l272!= l273){
SOC_IF_ERROR_RETURN(l271(l2,l272,l273));VRF_PIVOT_SHIFT_INC(l2,MAX_VRF_ID,l10
);}(l63[(l2)][(l137)].l56)+= 1;(l63[(l2)][(l137)].l57)+= 1;return(SOC_E_NONE)
;}static int l280(int l2,int l137,int l10){uint32 l15[SOC_MAX_MEM_FIELD_WORDS
];int l272;int l273;int l281;uint32 l277,l278;l273 = (l63[(l2)][(l137)].l56)-
1;if((l63[(l2)][(l137)].l59) == 0){(l63[(l2)][(l137)].l56) = l273;(l63[(l2)][
(l137)].l57) = l273-1;return(SOC_E_NONE);}if((!l10)&&((l63[(l2)][(l137)].l57)
!= (l63[(l2)][(l137)].l56))){l272 = (l63[(l2)][(l137)].l57);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l272,l15));l277 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l93));l278 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l113[(l2)]->l94));if((l277 == 0)||(l278 == 0)){l281 = l272
-1;SOC_IF_ERROR_RETURN(l271(l2,l281,l273));VRF_PIVOT_SHIFT_INC(l2,MAX_VRF_ID,
l10);l251(l2,l15,l281,0x4000,0);SOC_IF_ERROR_RETURN(l175(l2,MEM_BLOCK_ANY,
l281,l272,l15));SOC_IF_ERROR_RETURN(l266(l2,l281,l15));do{int l279 = 
soc_th_alpm_physical_idx((l2),L3_DEFIPm,(l272),1)<<1;int l216 = 
soc_th_alpm_physical_idx((l2),L3_DEFIPm,(l281),1)*2+(l279&1);if((l278)){l279
++;}ALPM_TCAM_PIVOT((l2),l216) = ALPM_TCAM_PIVOT((l2),l279);if(
ALPM_TCAM_PIVOT((l2),l216)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l2),l216)) = 
l216;}ALPM_TCAM_PIVOT((l2),l279) = NULL;}while(0);}else{l251(l2,l15,l273,
0x4000,0);SOC_IF_ERROR_RETURN(l175(l2,MEM_BLOCK_ANY,l273,l272,l15));
SOC_IF_ERROR_RETURN(l266(l2,l273,l15));do{int l274,l275;l274 = 
soc_th_alpm_physical_idx((l2),L3_DEFIPm,(l272),1);l275 = 
soc_th_alpm_physical_idx((l2),L3_DEFIPm,(l273),1);ALPM_TCAM_PIVOT(l2,l275<<1)
= ALPM_TCAM_PIVOT(l2,l274<<1);ALPM_TCAM_PIVOT(l2,(l275<<1)+1) = 
ALPM_TCAM_PIVOT(l2,(l274<<1)+1);if(ALPM_TCAM_PIVOT((l2),l275<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l2),l275<<1)) = l275<<1;}if(ALPM_TCAM_PIVOT
((l2),(l275<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l2),(l275<<1)+1)) = (
l275<<1)+1;}ALPM_TCAM_PIVOT(l2,l274<<1) = NULL;ALPM_TCAM_PIVOT(l2,(l274<<1)+1
) = NULL;}while(0);}}else{l272 = (l63[(l2)][(l137)].l57);SOC_IF_ERROR_RETURN(
l271(l2,l272,l273));VRF_PIVOT_SHIFT_INC(l2,MAX_VRF_ID,l10);}(l63[(l2)][(l137)
].l56)-= 1;(l63[(l2)][(l137)].l57)-= 1;return(SOC_E_NONE);}static int l282(
int l2,int l137,int l10,void*l15,int*l283){int l284;int l285;int l286;int l287
;int l272;uint32 l277,l278;int l153;if((l63[(l2)][(l137)].l59) == 0){l287 = (
(3*(64+32+2+1))-1);if((soc_th_alpm_mode_get(l2) == SOC_ALPM_MODE_PARALLEL)||(
soc_th_alpm_mode_get(l2) == SOC_ALPM_MODE_TCAM_ALPM)){if(l137<= (((3*(64+32+2
+1))/3)-1)){l287 = (((3*(64+32+2+1))/3)-1);}}while((l63[(l2)][(l287)].next)>
l137){l287 = (l63[(l2)][(l287)].next);}l285 = (l63[(l2)][(l287)].next);if(
l285!= -1){(l63[(l2)][(l285)].l58) = l137;}(l63[(l2)][(l137)].next) = (l63[(
l2)][(l287)].next);(l63[(l2)][(l137)].l58) = l287;(l63[(l2)][(l287)].next) = 
l137;(l63[(l2)][(l137)].l60) = ((l63[(l2)][(l287)].l60)+1)/2;(l63[(l2)][(l287
)].l60)-= (l63[(l2)][(l137)].l60);(l63[(l2)][(l137)].l56) = (l63[(l2)][(l287)
].l57)+(l63[(l2)][(l287)].l60)+1;(l63[(l2)][(l137)].l57) = (l63[(l2)][(l137)]
.l56)-1;(l63[(l2)][(l137)].l59) = 0;}else if(!l10){l272 = (l63[(l2)][(l137)].
l56);if((l153 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l272,l15))<0){return l153;}
l277 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),
(l113[(l2)]->l93));l278 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l15),(l113[(l2)]->l94));if((l277 == 0)||(l278 == 0)){*l283 = (
l272<<1)+((l278 == 0)?1:0);return(SOC_E_NONE);}l272 = (l63[(l2)][(l137)].l57)
;if((l153 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l272,l15))<0){return l153;}l277 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l113[(
l2)]->l93));l278 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l113[(l2)]->l94));if((l277 == 0)||(l278 == 0)){*l283 = (
l272<<1)+((l278 == 0)?1:0);return(SOC_E_NONE);}}l286 = l137;while((l63[(l2)][
(l286)].l60) == 0){l286 = (l63[(l2)][(l286)].next);if(l286 == -1){l286 = l137
;break;}}while((l63[(l2)][(l286)].l60) == 0){l286 = (l63[(l2)][(l286)].l58);
if(l286 == -1){if((l63[(l2)][(l137)].l59) == 0){l284 = (l63[(l2)][(l137)].l58
);l285 = (l63[(l2)][(l137)].next);if(-1!= l284){(l63[(l2)][(l284)].next) = 
l285;}if(-1!= l285){(l63[(l2)][(l285)].l58) = l284;}}return(SOC_E_FULL);}}
while(l286>l137){l285 = (l63[(l2)][(l286)].next);SOC_IF_ERROR_RETURN(l280(l2,
l285,l10));(l63[(l2)][(l286)].l60)-= 1;(l63[(l2)][(l285)].l60)+= 1;l286 = 
l285;}while(l286<l137){SOC_IF_ERROR_RETURN(l276(l2,l286,l10));(l63[(l2)][(
l286)].l60)-= 1;l284 = (l63[(l2)][(l286)].l58);(l63[(l2)][(l284)].l60)+= 1;
l286 = l284;}(l63[(l2)][(l137)].l59)+= 1;(l63[(l2)][(l137)].l60)-= 1;(l63[(l2
)][(l137)].l57)+= 1;*l283 = (l63[(l2)][(l137)].l57)<<((l10)?0:1);sal_memcpy(
l15,soc_mem_entry_null(l2,L3_DEFIPm),soc_mem_entry_words(l2,L3_DEFIPm)*4);
return(SOC_E_NONE);}static int l288(int l2,int l137,int l10,void*l15,int l289
){int l284;int l285;int l272;int l273;uint32 l290[SOC_MAX_MEM_FIELD_WORDS];
uint32 l291[SOC_MAX_MEM_FIELD_WORDS];uint32 l292[SOC_MAX_MEM_FIELD_WORDS];
void*l293;int l153;int l294,l42;l272 = (l63[(l2)][(l137)].l57);l273 = l289;if
(!l10){l273>>= 1;if((l153 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l272,l290))<0){
return l153;}if((l153 = READ_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,
soc_th_alpm_physical_idx(l2,L3_DEFIPm,l272,1),l291))<0){return l153;}if((l153
= READ_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,soc_th_alpm_physical_idx(l2,
L3_DEFIPm,l273,1),l292))<0){return l153;}l293 = (l273 == l272)?l290:l15;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l290),(l113[
(l2)]->l94))){l42 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,l291,
BPM_LENGTH1f);if(l289&1){l153 = soc_th_alpm_lpm_ip4entry1_to_1(l2,l290,l293,
PRESERVE_HIT);soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,l292,BPM_LENGTH1f,
l42);}else{l153 = soc_th_alpm_lpm_ip4entry1_to_0(l2,l290,l293,PRESERVE_HIT);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,l292,BPM_LENGTH0f,l42);}l294 = (
l272<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l290),(l113[(l2)]->l94),(0));}else{l42 = soc_mem_field32_get(l2,
L3_DEFIP_AUX_TABLEm,l291,BPM_LENGTH0f);if(l289&1){l153 = 
soc_th_alpm_lpm_ip4entry0_to_1(l2,l290,l293,PRESERVE_HIT);soc_mem_field32_set
(l2,L3_DEFIP_AUX_TABLEm,l292,BPM_LENGTH1f,l42);}else{l153 = 
soc_th_alpm_lpm_ip4entry0_to_0(l2,l290,l293,PRESERVE_HIT);soc_mem_field32_set
(l2,L3_DEFIP_AUX_TABLEm,l292,BPM_LENGTH0f,l42);}l294 = l272<<1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l290),(l113[
(l2)]->l93),(0));(l63[(l2)][(l137)].l59)-= 1;(l63[(l2)][(l137)].l60)+= 1;(l63
[(l2)][(l137)].l57)-= 1;}l294 = soc_th_alpm_physical_idx(l2,L3_DEFIPm,l294,0)
;l289 = soc_th_alpm_physical_idx(l2,L3_DEFIPm,l289,0);ALPM_TCAM_PIVOT(l2,l289
) = ALPM_TCAM_PIVOT(l2,l294);if(ALPM_TCAM_PIVOT(l2,l289)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l2,l289)) = l289;}ALPM_TCAM_PIVOT(l2,l294) = NULL;if((l153 = 
WRITE_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ALL,soc_th_alpm_physical_idx(l2,
L3_DEFIPm,l273,1),l292))<0){return l153;}if(l273!= l272){l251(l2,l293,l273,
0x4000,0);if((l153 = l175(l2,MEM_BLOCK_ANY,l273,l273,l293))<0){return l153;}
if((l153 = l266(l2,l273,l293))<0){return l153;}}l251(l2,l290,l272,0x4000,0);
if((l153 = l175(l2,MEM_BLOCK_ANY,l272,l272,l290))<0){return l153;}if((l153 = 
l266(l2,l272,l290))<0){return l153;}}else{(l63[(l2)][(l137)].l59)-= 1;(l63[(
l2)][(l137)].l60)+= 1;(l63[(l2)][(l137)].l57)-= 1;if(l273!= l272){if((l153 = 
READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l272,l290))<0){return l153;}l251(l2,l290,l273
,0x4000,0);if((l153 = l175(l2,MEM_BLOCK_ANY,l273,l272,l290))<0){return l153;}
if((l153 = l266(l2,l273,l290))<0){return l153;}}l289 = 
soc_th_alpm_physical_idx(l2,L3_DEFIPm,l273,1);l294 = soc_th_alpm_physical_idx
(l2,L3_DEFIPm,l272,1);ALPM_TCAM_PIVOT(l2,l289<<1) = ALPM_TCAM_PIVOT(l2,l294<<
1);ALPM_TCAM_PIVOT(l2,l294<<1) = NULL;if(ALPM_TCAM_PIVOT(l2,l289<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l2,l289<<1)) = l289<<1;}sal_memcpy(l290,
soc_mem_entry_null(l2,L3_DEFIPm),soc_mem_entry_words(l2,L3_DEFIPm)*4);l251(l2
,l290,l272,0x4000,0);if((l153 = l175(l2,MEM_BLOCK_ANY,l272,l272,l290))<0){
return l153;}if((l153 = l266(l2,l272,l290))<0){return l153;}}if((l63[(l2)][(
l137)].l59) == 0){l284 = (l63[(l2)][(l137)].l58);assert(l284!= -1);l285 = (
l63[(l2)][(l137)].next);(l63[(l2)][(l284)].next) = l285;(l63[(l2)][(l284)].
l60)+= (l63[(l2)][(l137)].l60);(l63[(l2)][(l137)].l60) = 0;if(l285!= -1){(l63
[(l2)][(l285)].l58) = l284;}(l63[(l2)][(l137)].next) = -1;(l63[(l2)][(l137)].
l58) = -1;(l63[(l2)][(l137)].l56) = -1;(l63[(l2)][(l137)].l57) = -1;}return(
l153);}int soc_th_alpm_lpm_vrf_get(int l21,void*lpm_entry,int*l28,int*l43){
int l162;if(((l113[(l21)]->l97)!= NULL)){l162 = soc_L3_DEFIPm_field32_get(l21
,lpm_entry,VRF_ID_0f);*l43 = l162;if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,
VRF_ID_MASK0f)){*l28 = l162;}else if(!soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),(l113[(l21)]->l99))){*l28 = 
SOC_L3_VRF_GLOBAL;*l43 = SOC_VRF_MAX(l21)+1;}else{*l28 = SOC_L3_VRF_OVERRIDE;
}}else{*l28 = SOC_L3_VRF_DEFAULT;}return(SOC_E_NONE);}static int l295(int l2,
void*entry,int*l17){int l137;int l153;int l10;uint32 l263;int l162;int l296;
l10 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry)
,(l113[(l2)]->l83));if(l10){l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l113[(l2)]->l81));if((l153 = _ipmask2pfx
(l263,&l137))<0){return(l153);}l263 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l113[(l2)]->l82));if(l137){if(l263!= 
0xffffffff){return(SOC_E_PARAM);}l137+= 32;}else{if((l153 = _ipmask2pfx(l263,
&l137))<0){return(l153);}}l137+= 33;}else{l263 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l113
[(l2)]->l81));if((l153 = _ipmask2pfx(l263,&l137))<0){return(l153);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l2,entry,&l162,&l153));l296 = 
soc_th_alpm_mode_get(l2);switch(l162){case SOC_L3_VRF_GLOBAL:if((l296 == 
SOC_ALPM_MODE_PARALLEL)||(l296 == SOC_ALPM_MODE_TCAM_ALPM)){*l17 = l137+((3*(
64+32+2+1))/3);}else{*l17 = l137;}break;case SOC_L3_VRF_OVERRIDE:*l17 = l137+
2*((3*(64+32+2+1))/3);break;default:if((l296 == SOC_ALPM_MODE_PARALLEL)||(
l296 == SOC_ALPM_MODE_TCAM_ALPM)){*l17 = l137;}else{*l17 = l137+((3*(64+32+2+
1))/3);}break;}return(SOC_E_NONE);}static int l14(int l2,void*l8,void*l15,int
*l16,int*l17,int*l10){int l153;int l34;int l138;int l137 = 0;l34 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(
l2)]->l83));if(l34){if(!(l34 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l113[(l2)]->l84)))){return(SOC_E_PARAM);}}*
l10 = l34;l295(l2,l8,&l137);*l17 = l137;if(l255(l2,l8,l137,&l138) == 
SOC_E_NONE){*l16 = l138;if((l153 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,(*l10)?*
l16:(*l16>>1),l15))<0){return l153;}return(SOC_E_NONE);}else{return(
SOC_E_NOT_FOUND);}}static int l3(int l2){int l265;int l181;int l247;int l297;
uint32 l145,l296;if(!soc_feature(l2,soc_feature_lpm_tcam)){return(
SOC_E_UNAVAIL);}if((l296 = soc_property_get(l2,spn_L3_ALPM_ENABLE,0))){
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l2,&l145));soc_reg_field_set(
l2,L3_DEFIP_RPF_CONTROLr,&l145,LPM_MODEf,1);if(l296 == SOC_ALPM_MODE_PARALLEL
){soc_reg_field_set(l2,L3_DEFIP_RPF_CONTROLr,&l145,LOOKUP_MODEf,1);}else if(
l296 == SOC_ALPM_MODE_TCAM_ALPM){soc_reg_field_set(l2,L3_DEFIP_RPF_CONTROLr,&
l145,LOOKUP_MODEf,2);}else{soc_reg_field_set(l2,L3_DEFIP_RPF_CONTROLr,&l145,
LOOKUP_MODEf,0);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l2,l145));
l145 = 0;if(SOC_URPF_STATUS_GET(l2)){soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&
l145,URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l145,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l145,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l145,
URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l2,l145))
;l145 = 0;if(l296 == SOC_ALPM_MODE_PARALLEL){if(SOC_URPF_STATUS_GET(l2)){
soc_reg_field_set(l2,L3_DEFIP_ALPM_CFGr,&l145,TCAM2_SELf,1);soc_reg_field_set
(l2,L3_DEFIP_ALPM_CFGr,&l145,TCAM3_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM4_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM5_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM6_SELf,3);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM7_SELf,3);}else{soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM4_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM5_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM6_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l2)){
soc_reg_field_set(l2,L3_DEFIP_ALPM_CFGr,&l145,TCAM4_SELf,2);soc_reg_field_set
(l2,L3_DEFIP_ALPM_CFGr,&l145,TCAM5_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM6_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l145,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l2,l145));if(soc_property_get(l2,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l298 = 0;if(l296!= SOC_ALPM_MODE_PARALLEL
){uint32 l299;l299 = soc_property_get(l2,spn_NUM_IPV6_LPM_128B_ENTRIES,2048);
if(l299!= 2048){if(SOC_URPF_STATUS_GET(l2)){LOG_CLI((BSL_META_U(l2,
"URPF supported in combined mode only""with 2048 v6-128 entries\n")));return
SOC_E_PARAM;}if((l299!= 1024)&&(l299!= 3072)){LOG_CLI((BSL_META_U(l2,
"Only supported values for v6-128 in"
"nonURPF combined mode are 1024 and 3072\n")));return SOC_E_PARAM;}}}
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_KEY_SELr(l2,&l298));soc_reg_field_set(l2,
L3_DEFIP_KEY_SELr,&l298,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l2,
L3_DEFIP_KEY_SELr,&l298,V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l2,
L3_DEFIP_KEY_SELr,&l298,V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l2,
L3_DEFIP_KEY_SELr,&l298,V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_KEY_SELr(l2,l298));}}l265 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(
l2);l297 = sizeof(l61)*(l265);if((l63[(l2)]!= NULL)){if(soc_th_alpm_deinit(l2
)<0){SOC_ALPM_LPM_UNLOCK(l2);return SOC_E_INTERNAL;}}l113[l2] = sal_alloc(
sizeof(l111),"lpm_field_state");if(NULL == l113[l2]){SOC_ALPM_LPM_UNLOCK(l2);
return(SOC_E_MEMORY);}(l113[l2])->l65 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,
CLASS_ID0f);(l113[l2])->l66 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,CLASS_ID1f);
(l113[l2])->l67 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,DST_DISCARD0f);(l113[l2]
)->l68 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,DST_DISCARD1f);(l113[l2])->l69 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP0f);(l113[l2])->l70 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP1f);(l113[l2])->l71 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_COUNT0f);(l113[l2])->l72 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_COUNT1f);(l113[l2])->l73 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_PTR0f);(l113[l2])->l74 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_PTR1f);(l113[l2])->l75 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_ROUTE0f);(l113[l2])->l76 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_ROUTE1f);(l113[l2])->l77 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,HIT0f);(l113[l2])->l78 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,HIT1f);(l113[l2])->l79 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR0f);(l113[l2])->l80 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR1f);(l113[l2])->l81 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR_MASK0f);(l113[l2])->l82 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR_MASK1f);(l113[l2])->l83 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE0f);(l113[l2])->l84 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE1f);(l113[l2])->l85 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE_MASK0f);(l113[l2])->l86 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE_MASK1f);(l113[l2])->l87 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,NEXT_HOP_INDEX0f);(l113[l2])->l88 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,NEXT_HOP_INDEX1f);(l113[l2])->l89 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,PRI0f);(l113[l2])->l90 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,PRI1f);(l113[l2])->l91 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,RPE0f);(l113[l2])->l92 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,RPE1f);(l113[l2])->l93 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VALID0f);(l113[l2])->l94 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VALID1f);(l113[l2])->l95 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_0f);(l113[l2])->l96 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_1f);(l113[l2])->l97 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_MASK0f);(l113[l2])->l98 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_MASK1f);(l113[l2])->l99 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_HIGH0f);(l113[l2])->l100 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_HIGH1f);(l113[l2])->l101 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_HIT_IDX0f);(l113[l2])->l102 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_HIT_IDX1f);(l113[l2])->l103 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_BKT_PTR0f);(l113[l2])->l104 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_BKT_PTR1f);(l113[l2])->l105 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DEFAULT_MISS0f);(l113[l2])->l106 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DEFAULT_MISS1f);(l113[l2])->l107 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l113[l2])->
l108 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l113[
l2])->l109 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l113
[l2])->l110 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(l63
[(l2)]) = sal_alloc(l297,"LPM prefix info");if(NULL == (l63[(l2)])){sal_free(
l113[l2]);l113[l2] = NULL;SOC_ALPM_LPM_UNLOCK(l2);return(SOC_E_MEMORY);}
sal_memset((l63[(l2)]),0,l297);for(l181 = 0;l181<l265;l181++){(l63[(l2)][(
l181)].l56) = -1;(l63[(l2)][(l181)].l57) = -1;(l63[(l2)][(l181)].l58) = -1;(
l63[(l2)][(l181)].next) = -1;(l63[(l2)][(l181)].l59) = 0;(l63[(l2)][(l181)].
l60) = 0;}l247 = soc_mem_index_count(l2,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l2)
){l247>>= 1;}if((l296 == SOC_ALPM_MODE_PARALLEL)||(l296 == 
SOC_ALPM_MODE_TCAM_ALPM)){(l63[(l2)][(((3*(64+32+2+1))-1))].l57) = (l247>>1)-
1;(l63[(l2)][(((((3*(64+32+2+1))/3)-1)))].l60) = l247>>1;(l63[(l2)][((((3*(64
+32+2+1))-1)))].l60) = (l247-(l63[(l2)][(((((3*(64+32+2+1))/3)-1)))].l60));}
else{(l63[(l2)][((((3*(64+32+2+1))-1)))].l60) = l247;}if((l124[(l2)])!= NULL)
{if(l132((l124[(l2)]))<0){SOC_ALPM_LPM_UNLOCK(l2);return SOC_E_INTERNAL;}(
l124[(l2)]) = NULL;}if(l130(l2,l247*2,l247,&(l124[(l2)]))<0){
SOC_ALPM_LPM_UNLOCK(l2);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l2);return(
SOC_E_NONE);}static int l4(int l2){if(!soc_feature(l2,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l2);if((l124[(l2)])!= NULL){l132((
l124[(l2)]));(l124[(l2)]) = NULL;}if((l63[(l2)]!= NULL)){sal_free(l113[l2]);
l113[l2] = NULL;sal_free((l63[(l2)]));(l63[(l2)]) = NULL;}SOC_ALPM_LPM_UNLOCK
(l2);return(SOC_E_NONE);}static int l5(int l2,void*l6,int*l300){int l137;int
index;int l10;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l153 = SOC_E_NONE;int
l301 = 0;sal_memcpy(l15,soc_mem_entry_null(l2,L3_DEFIPm),soc_mem_entry_words(
l2,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l2);l153 = l14(l2,l6,l15,&index,&l137,&l10
);if(l153 == SOC_E_NOT_FOUND){l153 = l282(l2,l137,l10,l15,&index);if(l153<0){
SOC_ALPM_LPM_UNLOCK(l2);return(l153);}}else{l301 = 1;}*l300 = index;if(l153 == 
SOC_E_NONE){if(!l10){if(index&1){l153 = soc_th_alpm_lpm_ip4entry0_to_1(l2,l6,
l15,PRESERVE_HIT);}else{l153 = soc_th_alpm_lpm_ip4entry0_to_0(l2,l6,l15,
PRESERVE_HIT);}if(l153<0){SOC_ALPM_LPM_UNLOCK(l2);return(l153);}l6 = (void*)
l15;index>>= 1;}l1(l2);LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"\nsoc_alpm_lpm_insert: %d %d\n"),index,l137));if(!l301){l251(l2,l6,index,
0x4000,0);}l153 = l175(l2,MEM_BLOCK_ANY,index,index,l6);if(l153>= 0){l153 = 
l266(l2,index,l6);}}SOC_ALPM_LPM_UNLOCK(l2);return(l153);}static int l7(int l2
,void*l8){int l137;int index;int l10;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int
l153 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l2);l153 = l14(l2,l8,l15,&index,&l137,&
l10);if(l153 == SOC_E_NONE){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"\nsoc_alpm_lpm_delete: %d %d\n"),index,l137));l254(l2,l8,index);l153 = l288(
l2,l137,l10,l15,index);}l1(l2);SOC_ALPM_LPM_UNLOCK(l2);return(l153);}static
int l18(int l2,void*l8,void*l15,int*l16){int l137;int l153;int l10;
SOC_ALPM_LPM_LOCK(l2);l153 = l14(l2,l8,l15,l16,&l137,&l10);
SOC_ALPM_LPM_UNLOCK(l2);return(l153);}static int l9(int l21,void*l8,int l10,
int l11,int l12,defip_aux_scratch_entry_t*l13){uint32 l263;uint32 l302[4] = {
0,0,0,0};int l137 = 0;int l153 = SOC_E_NONE;l263 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,VALID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
VALIDf,l263);l263 = soc_mem_field32_get(l21,L3_DEFIPm,l8,MODE0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,MODEf,l263);l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ENTRY_TYPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ENTRY_TYPEf,0);l263 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,GLOBAL_ROUTE0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,GLOBAL_ROUTEf,l263);l263 = soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,ECMPf,l263);l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP_PTR0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ECMP_PTRf,l263);l263 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,NEXT_HOP_INDEX0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,NEXT_HOP_INDEXf,l263);l263 = soc_mem_field32_get(l21,L3_DEFIPm,l8,PRI0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,PRIf,l263);l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,RPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,RPEf,l263);l263 =soc_mem_field32_get(l21,L3_DEFIPm,
l8,VRF_ID_0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,VRFf,l263);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,DB_TYPEf,l11);l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,DST_DISCARD0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,DST_DISCARDf,l263);l263 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,CLASS_ID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
CLASS_IDf,l263);if(l10){l302[2] = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR0f);l302[3] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR1f);}else{
l302[0] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR0f);}soc_mem_field_set(
l21,L3_DEFIP_AUX_SCRATCHm,(uint32*)l13,IP_ADDRf,(uint32*)l302);if(l10){l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l153 = _ipmask2pfx(
l263,&l137))<0){return(l153);}l263 = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR_MASK1f);if(l137){if(l263!= 0xffffffff){return(SOC_E_PARAM);}l137+= 32
;}else{if((l153 = _ipmask2pfx(l263,&l137))<0){return(l153);}}}else{l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l153 = _ipmask2pfx(
l263,&l137))<0){return(l153);}}soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,IP_LENGTHf,l137);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
REPLACE_LENf,l12);return(SOC_E_NONE);}int _soc_th_alpm_aux_op(int l2,
_soc_aux_op_t l303,defip_aux_scratch_entry_t*l13,int l304,int*l158,int*l53,
int*bucket_index){uint32 l145,l305;int l306;soc_timeout_t l307;int l153 = 
SOC_E_NONE;int l308 = 0;if(l304){SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_SCRATCHm(l2,MEM_BLOCK_ANY,0,l13));}l309:l145 = 0;switch(
l303){case INSERT_PROPAGATE:l306 = 0;break;case DELETE_PROPAGATE:l306 = 1;
break;case PREFIX_LOOKUP:l306 = 2;break;case HITBIT_REPLACE:l306 = 3;break;
default:return SOC_E_PARAM;}soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l145,
OPCODEf,l306);soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l145,STARTf,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l2,l145));soc_timeout_init(&l307
,50000,5);l306 = 0;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l2,&l145));
l306 = soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l145,DONEf);if(l306 == 1){l153
= SOC_E_NONE;break;}if(soc_timeout_check(&l307)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l2,&l145));l306 = soc_reg_field_get(l2,
L3_DEFIP_AUX_CTRLr,l145,DONEf);if(l306 == 1){l153 = SOC_E_NONE;}else{LOG_WARN
(BSL_LS_SOC_ALPM,(BSL_META_U(l2,"unit %d : DEFIP AUX Operation timeout\n"),l2
));l153 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l153)){if(
soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l145,ERRORf)){soc_reg_field_set(l2,
L3_DEFIP_AUX_CTRLr,&l145,STARTf,0);soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&
l145,ERRORf,0);soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l145,DONEf,0);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l2,l145));LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,"unit %d: DEFIP AUX Operation encountered "
"parity error !!\n"),l2));l308++;if(SOC_CONTROL(l2)->alpm_bulk_retry){
sal_sem_take(SOC_CONTROL(l2)->alpm_bulk_retry,1000000);}if(l308<5){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l2,"unit %d: Retry DEFIP AUX Operation..\n"),l2))
;goto l309;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l2,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l2));return SOC_E_INTERNAL;}}if(l303 == PREFIX_LOOKUP){if(l158&&l53){*l158 = 
soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l145,HITf);*l53 = soc_reg_field_get(
l2,L3_DEFIP_AUX_CTRLr,l145,BKT_INDEXf);SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRL_1r(l2,&l305));*bucket_index = soc_reg_field_get(l2,
L3_DEFIP_AUX_CTRL_1r,l305,BKT_PTRf);}}}return l153;}static int l20(int l21,
void*lpm_entry,void*l22,void*l23,soc_mem_t l24,uint32 l25,uint32*l310){uint32
l263;uint32 l302[4] = {0,0};int l137 = 0;int l153 = SOC_E_NONE;int l10;uint32
l26 = 0;l10 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,MODE0f);sal_memset(
l22,0,soc_mem_entry_words(l21,l24)*4);l263 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,HIT0f);soc_mem_field32_set(l21,l24,l22,HITf,l263);l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,VALID0f);soc_mem_field32_set(l21,
l24,l22,VALIDf,l263);l263 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
ECMP0f);soc_mem_field32_set(l21,l24,l22,ECMPf,l263);l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set(
l21,l24,l22,ECMP_PTRf,l263);l263 = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,NEXT_HOP_INDEX0f);soc_mem_field32_set(l21,l24,l22,NEXT_HOP_INDEXf,
l263);l263 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,PRI0f);
soc_mem_field32_set(l21,l24,l22,PRIf,l263);l263 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,RPE0f);soc_mem_field32_set(l21,l24,l22,RPEf,l263);l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(l21,l24,l22,DST_DISCARDf,l263);l263 = soc_mem_field32_get
(l21,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);soc_mem_field32_set(l21,l24,l22,
SRC_DISCARDf,l263);l263 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
CLASS_ID0f);soc_mem_field32_set(l21,l24,l22,CLASS_IDf,l263);l302[0] = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR0f);if(l10){l302[1] = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR1f);}soc_mem_field_set(l21
,l24,(uint32*)l22,KEYf,(uint32*)l302);if(l10){l263 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l153 = _ipmask2pfx(l263,&l137))<0){
return(l153);}l263 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f);if(l137){if(l263!= 0xffffffff){return(SOC_E_PARAM);}l137+= 32
;}else{if((l153 = _ipmask2pfx(l263,&l137))<0){return(l153);}}}else{l263 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l153 = 
_ipmask2pfx(l263,&l137))<0){return(l153);}}if((l137 == 0)&&(l302[0] == 0)&&(
l302[1] == 0)){l26 = 1;}if(l310!= NULL){*l310 = l26;}soc_mem_field32_set(l21,
l24,l22,LENGTHf,l137);if(l23 == NULL){return(SOC_E_NONE);}if(
SOC_URPF_STATUS_GET(l21)){sal_memset(l23,0,soc_mem_entry_words(l21,l24)*4);
sal_memcpy(l23,l22,soc_mem_entry_words(l21,l24)*4);soc_mem_field32_set(l21,
l24,l23,DST_DISCARDf,0);soc_mem_field32_set(l21,l24,l23,RPEf,0);
soc_mem_field32_set(l21,l24,l23,SRC_DISCARDf,l25&SOC_ALPM_RPF_SRC_DISCARD);
soc_mem_field32_set(l21,l24,l23,DEFAULTROUTEf,l26);}return(SOC_E_NONE);}
static int l27(int l21,void*l22,soc_mem_t l24,int l10,int l28,int l29,int
index,void*lpm_entry){uint32 l263;uint32 l302[4] = {0,0};uint32 l137 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_words(l21,L3_DEFIPm)*4);l263 = 
soc_mem_field32_get(l21,l24,l22,HITf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,HIT0f,l263);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
HIT1f,l263);}l263 = soc_mem_field32_get(l21,l24,l22,VALIDf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VALID0f,l263);if(l10){
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VALID1f,l263);}l263 = 
soc_mem_field32_get(l21,l24,l22,ECMPf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,ECMP0f,l263);l263 = soc_mem_field32_get(l21,l24,l22,ECMP_PTRf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ECMP_PTR0f,l263);l263 = 
soc_mem_field32_get(l21,l24,l22,NEXT_HOP_INDEXf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f,l263);l263 = soc_mem_field32_get(l21,l24
,l22,PRIf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,PRI0f,l263);l263 = 
soc_mem_field32_get(l21,l24,l22,RPEf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,RPE0f,l263);l263 = soc_mem_field32_get(l21,l24,l22,DST_DISCARDf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,DST_DISCARD0f,l263);l263 = 
soc_mem_field32_get(l21,l24,l22,SRC_DISCARDf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,SRC_DISCARD0f,l263);l263 = soc_mem_field32_get(l21,l24,
l22,CLASS_IDf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,CLASS_ID0f,l263);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ALG_BKT_PTR0f,l29);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ALG_HIT_IDX0f,index);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE_MASK0f,3);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l10){
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(l21,
l24,l22,KEYf,l302);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR1f,l302[1]);}soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR0f,
l302[0]);l263 = soc_mem_field32_get(l21,l24,l22,LENGTHf);if(l10){if(l263>= 32
){l137 = 0xffffffff;soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l137);l137 = ~(((l263-32) == 32)?0:(0xffffffff)>>(l263-32));
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l137);}else{l137 = 
~(0xffffffff>>l263);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l137);}}else{assert(l263<= 32);l137 = ~(((l263) == 32)?0:(
0xffffffff)>>(l263));soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,l137);}if(l28 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,GLOBAL_HIGH0f,1);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else if(l28 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry
,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,0);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,l28);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(l21));}
return(SOC_E_NONE);}int soc_th_alpm_warmboot_pivot_add(int l21,int l10,void*
lpm_entry,int l311,int l312){int l153 = SOC_E_NONE;uint32 key[2] = {0,0};
alpm_pivot_t*l39 = NULL;alpm_bucket_handle_t*l208 = NULL;int l162 = 0,l28 = 0
;uint32 l313;trie_t*l241 = NULL;uint32 prefix[5] = {0};int l26 = 0;l153 = 
l150(l21,lpm_entry,prefix,&l313,&l26);SOC_IF_ERROR_RETURN(l153);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l21,lpm_entry,&l162,&l28));l311 = 
soc_th_alpm_physical_idx(l21,L3_DEFIPm,l311,l10);l208 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l208 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l21,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l208,0,sizeof(*l208));
l39 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l39 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l21,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l208);return SOC_E_MEMORY;}sal_memset(l39,0,
sizeof(*l39));PIVOT_BUCKET_HANDLE(l39) = l208;if(l10){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l39));key[0] = soc_L3_DEFIPm_field32_get
(l21,lpm_entry,IP_ADDR0f);key[1] = soc_L3_DEFIPm_field32_get(l21,lpm_entry,
IP_ADDR1f);}else{trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l39));key[0] = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l39) = 
l312;PIVOT_TCAM_INDEX(l39) = l311;if(l162!= SOC_L3_VRF_OVERRIDE){if(l10 == 0)
{l241 = VRF_PIVOT_TRIE_IPV4(l21,l28);if(l241 == NULL){trie_init(
_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(l21,l28));l241 = VRF_PIVOT_TRIE_IPV4(
l21,l28);}}else{l241 = VRF_PIVOT_TRIE_IPV6(l21,l28);if(l241 == NULL){
trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(l21,l28));l241 = 
VRF_PIVOT_TRIE_IPV6(l21,l28);}}sal_memcpy(l39->key,prefix,sizeof(prefix));l39
->len = l313;l153 = trie_insert(l241,l39->key,NULL,l39->len,(trie_node_t*)l39
);if(SOC_FAILURE(l153)){sal_free(l208);sal_free(l39);return l153;}}
ALPM_TCAM_PIVOT(l21,l311) = l39;PIVOT_BUCKET_VRF(l39) = l28;PIVOT_BUCKET_IPV6
(l39) = l10;PIVOT_BUCKET_ENT_CNT_UPDATE(l39);if(key[0] == 0&&key[1] == 0){
PIVOT_BUCKET_DEF(l39) = TRUE;}VRF_PIVOT_REF_INC(l21,l28,l10);return l153;}
static int l314(int l21,int l10,void*lpm_entry,void*l22,soc_mem_t l24,int l311
,int l312,int l315){int l316;int l28;int l153 = SOC_E_NONE;int l26 = 0;uint32
prefix[5] = {0,0,0,0,0};uint32 l40;void*l317 = NULL;trie_t*l210 = NULL;trie_t
*l38 = NULL;trie_node_t*l204 = NULL;payload_t*l318 = NULL;payload_t*l221 = 
NULL;alpm_pivot_t*l54 = NULL;if((NULL == lpm_entry)||(NULL == l22)){return
SOC_E_PARAM;}if(l10){if(!(l10 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(
l21,lpm_entry,&l316,&l28));l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l317 = sal_alloc(sizeof(defip_entry_t),"Temp lpm_entr");
if(NULL == l317){return SOC_E_MEMORY;}SOC_IF_ERROR_RETURN(l27(l21,l22,l24,l10
,l316,l312,l311,l317));l153 = l150(l21,l317,prefix,&l40,&l26);if(SOC_FAILURE(
l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l21,"prefix create failed\n")));
return l153;}sal_free(l317);l54 = ALPM_TCAM_PIVOT(l21,l311);l210 = 
PIVOT_BUCKET_TRIE(l54);l318 = sal_alloc(sizeof(payload_t),"Payload for Key");
if(NULL == l318){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l21,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l221 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l221){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l21,
"Unable to allocate memory for pfx trie node\n")));sal_free(l318);return
SOC_E_MEMORY;}sal_memset(l318,0,sizeof(*l318));sal_memset(l221,0,sizeof(*l221
));l318->key[0] = prefix[0];l318->key[1] = prefix[1];l318->key[2] = prefix[2]
;l318->key[3] = prefix[3];l318->key[4] = prefix[4];l318->len = l40;l318->
index = l315;sal_memcpy(l221,l318,sizeof(*l318));l153 = trie_insert(l210,
prefix,NULL,l40,(trie_node_t*)l318);if(SOC_FAILURE(l153)){goto l319;}if(l10){
l38 = VRF_PREFIX_TRIE_IPV6(l21,l28);}else{l38 = VRF_PREFIX_TRIE_IPV4(l21,l28)
;}if(!l26){l153 = trie_insert(l38,prefix,NULL,l40,(trie_node_t*)l221);if(
SOC_FAILURE(l153)){goto l320;}}return l153;l320:(void)trie_delete(l210,prefix
,l40,&l204);l318 = (payload_t*)l204;l319:sal_free(l318);sal_free(l221);return
l153;}static int l321(int l21,int l34,int l28,int l146,int bkt_ptr){int l153 = 
SOC_E_NONE;uint32 l40;uint32 key[2] = {0,0};trie_t*l322 = NULL;payload_t*l231
= NULL;defip_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l21,"unable to allocate memory for LPM entry\n"))
);return SOC_E_MEMORY;}l30(l21,key,0,l28,l34,lpm_entry,0,1);if(l28 == 
SOC_VRF_MAX(l21)+1){soc_L3_DEFIPm_field32_set(l21,lpm_entry,GLOBAL_ROUTE0f,1)
;}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,DEFAULT_MISS0f,1);}
soc_L3_DEFIPm_field32_set(l21,lpm_entry,ALG_BKT_PTR0f,bkt_ptr);if(l34 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l21,l28) = lpm_entry;trie_init(_MAX_KEY_LEN_48_,&
VRF_PREFIX_TRIE_IPV4(l21,l28));l322 = VRF_PREFIX_TRIE_IPV4(l21,l28);}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l21,l28) = lpm_entry;trie_init(_MAX_KEY_LEN_144_,
&VRF_PREFIX_TRIE_IPV6(l21,l28));l322 = VRF_PREFIX_TRIE_IPV6(l21,l28);}l231 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(l231 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l21,
"Unable to allocate memory for pfx trie node \n")));return SOC_E_MEMORY;}
sal_memset(l231,0,sizeof(*l231));l40 = 0;l231->key[0] = key[0];l231->key[1] = 
key[1];l231->len = l40;l153 = trie_insert(l322,key,NULL,l40,&(l231->node));if
(SOC_FAILURE(l153)){sal_free(l231);return l153;}VRF_TRIE_INIT_DONE(l21,l28,
l34,1);return l153;}int soc_th_alpm_warmboot_prefix_insert(int l21,int l10,
void*lpm_entry,void*l22,int l311,int l312,int l315){int l316;int l28;int l153
= SOC_E_NONE;soc_mem_t l24;l311 = soc_th_alpm_physical_idx(l21,L3_DEFIPm,l311
,l10);l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l21,lpm_entry,&l316,&l28));if(
l316 == SOC_L3_VRF_OVERRIDE){return(l153);}if(!VRF_TRIE_INIT_COMPLETED(l21,
l28,l10)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l21,
"VRF %d is not initialized\n"),l28));l153 = l321(l21,l10,l28,l311,l312);if(
SOC_FAILURE(l153)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l21,
"VRF %d/%d trie init \n""failed\n"),l28,l10));return l153;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l21,"VRF %d/%d trie init completed\n"),l28,l10));
}l153 = l314(l21,l10,lpm_entry,l22,l24,l311,l312,l315);if(l153!= SOC_E_NONE){
LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l21,
"unit %d : Route Insertion Failed :%s\n"),l21,soc_errmsg(l153)));return(l153)
;}VRF_TRIE_ROUTES_INC(l21,l28,l10);return(l153);}int
soc_th_alpm_warmboot_bucket_bitmap_set(int l2,int l34,int l244){int l323 = 1;
if(l34){if(!soc_th_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l323 = 2;}}if
(SOC_ALPM_BUCKET_BMAP(l2) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l2),l244,l323);return SOC_E_NONE;}int
soc_th_alpm_warmboot_lpm_reinit_done(int l21){int l146;int l324 = ((3*(64+32+
2+1))-1);int l247 = soc_mem_index_count(l21,L3_DEFIPm);if(SOC_URPF_STATUS_GET
(l21)){l247>>= 1;}if(!soc_th_alpm_mode_get(l21)){(l63[(l21)][(((3*(64+32+2+1)
)-1))].l58) = -1;for(l146 = ((3*(64+32+2+1))-1);l146>-1;l146--){if(-1 == (l63
[(l21)][(l146)].l56)){continue;}(l63[(l21)][(l146)].l58) = l324;(l63[(l21)][(
l324)].next) = l146;(l63[(l21)][(l324)].l60) = (l63[(l21)][(l146)].l56)-(l63[
(l21)][(l324)].l57)-1;l324 = l146;}(l63[(l21)][(l324)].next) = -1;(l63[(l21)]
[(l324)].l60) = l247-(l63[(l21)][(l324)].l57)-1;}else{(l63[(l21)][(((3*(64+32
+2+1))-1))].l58) = -1;for(l146 = ((3*(64+32+2+1))-1);l146>(((3*(64+32+2+1))-1
)/3);l146--){if(-1 == (l63[(l21)][(l146)].l56)){continue;}(l63[(l21)][(l146)]
.l58) = l324;(l63[(l21)][(l324)].next) = l146;(l63[(l21)][(l324)].l60) = (l63
[(l21)][(l146)].l56)-(l63[(l21)][(l324)].l57)-1;l324 = l146;}(l63[(l21)][(
l324)].next) = -1;(l63[(l21)][(l324)].l60) = l247-(l63[(l21)][(l324)].l57)-1;
l324 = (((3*(64+32+2+1))-1)/3);(l63[(l21)][((((3*(64+32+2+1))-1)/3))].l58) = 
-1;for(l146 = ((((3*(64+32+2+1))-1)/3)-1);l146>-1;l146--){if(-1 == (l63[(l21)
][(l146)].l56)){continue;}(l63[(l21)][(l146)].l58) = l324;(l63[(l21)][(l324)]
.next) = l146;(l63[(l21)][(l324)].l60) = (l63[(l21)][(l146)].l56)-(l63[(l21)]
[(l324)].l57)-1;l324 = l146;}(l63[(l21)][(l324)].next) = -1;(l63[(l21)][(l324
)].l60) = (l247>>1)-(l63[(l21)][(l324)].l57)-1;}return(SOC_E_NONE);}int
soc_th_alpm_warmboot_lpm_reinit(int l21,int l10,int l146,void*lpm_entry){int
l17;defip_entry_t*l325;if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l251(l21,lpm_entry,l146,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l295(l21,lpm_entry,&l17));if((l63[(l21)][(l17)].l59) == 0
){(l63[(l21)][(l17)].l56) = l146;(l63[(l21)][(l17)].l57) = l146;}else{(l63[(
l21)][(l17)].l57) = l146;}(l63[(l21)][(l17)].l59)++;if(l10){return(SOC_E_NONE
);}}else{if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l325 = 
sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");
soc_th_alpm_lpm_ip4entry1_to_0(l21,lpm_entry,l325,TRUE);SOC_IF_ERROR_RETURN(
l295(l21,l325,&l17));if((l63[(l21)][(l17)].l59) == 0){(l63[(l21)][(l17)].l56)
= l146;(l63[(l21)][(l17)].l57) = l146;}else{(l63[(l21)][(l17)].l57) = l146;}
sal_free(l325);(l63[(l21)][(l17)].l59)++;}}return(SOC_E_NONE);}typedef struct
l326{int v4;int v6_64;int v6_128;int l327;int l328;int l329;int l330;}l331;
typedef enum l332{l333 = 0,l334,l335,l336,l337,l338}l339;static void l340(int
l2,alpm_vrf_counter_t*l341){l341->v4 = soc_mem_index_count(l2,L3_DEFIPm)*2;
l341->v6_128 = soc_mem_index_count(l2,L3_DEFIP_PAIR_128m);if(soc_property_get
(l2,spn_IPV6_LPM_128B_ENABLE,1)){l341->v6_64 = l341->v6_128;}else{l341->v6_64
= l341->v4>>1;}if(SOC_URPF_STATUS_GET(l2)){l341->v4>>= 1;l341->v6_128>>= 1;
l341->v6_64>>= 1;}}static void l342(int l2,int l162,alpm_vrf_handle_t*l343,
l339 l344){alpm_vrf_counter_t*l345;int l181,l346,l347,l348;int l301 = 0;
alpm_vrf_counter_t l341;switch(l344){case l333:LOG_CLI((BSL_META_U(l2,
"\nAdd Counter:\n")));break;case l334:LOG_CLI((BSL_META_U(l2,
"\nDelete Counter:\n")));break;case l335:LOG_CLI((BSL_META_U(l2,
"\nInternal Debug Counter - 1:\n")));break;case l336:l340(l2,&l341);LOG_CLI((
BSL_META_U(l2,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l341.v4,
l341.v6_64,l341.v6_128));break;case l337:LOG_CLI((BSL_META_U(l2,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l338:LOG_CLI((
BSL_META_U(l2,"\nInternal Debug Counter - LPM Full:\n")));break;default:break
;}LOG_CLI((BSL_META_U(l2,"\n      VRF  v4      v6-64   v6-128  |   Total\n"))
);LOG_CLI((BSL_META_U(l2,"-----------------------------------------------\n")
));l346 = l347 = l348 = 0;for(l181 = 0;l181<MAX_VRF_ID+1;l181++){int l349,
l350,l351;if(l343[l181].init_done == 0&&l181!= MAX_VRF_ID){continue;}if(l162
!= -1&&l162!= l181){continue;}l301 = 1;switch(l344){case l333:l345 = &l343[
l181].add;break;case l334:l345 = &l343[l181].del;break;case l335:l345 = &l343
[l181].bkt_split;break;case l337:l345 = &l343[l181].lpm_shift;break;case l338
:l345 = &l343[l181].lpm_full;break;case l336:l345 = &l343[l181].pivot_used;
break;default:l345 = &l343[l181].pivot_used;break;}l349 = l345->v4;l350 = 
l345->v6_64;l351 = l345->v6_128;l346+= l349;l347+= l350;l348+= l351;do{
LOG_CLI((BSL_META_U(l2,"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l181 == 
MAX_VRF_ID?-1:l181),(l349),(l350),(l351),((l349+l350+l351)),(l181) == 
MAX_VRF_ID?"GHi":(l181) == SOC_VRF_MAX(l2)+1?"GLo":""));}while(0);}if(l301 == 
0){LOG_CLI((BSL_META_U(l2,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((
BSL_META_U(l2,"-----------------------------------------------\n")));do{
LOG_CLI((BSL_META_U(l2,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l346),(
l347),(l348),((l346+l347+l348))));}while(0);}return;}int l352(int l2,int l162
,uint32 flags){int l181,l353,l301 = 0;l331*l354;l331 l355;l331 l356;if(l162>(
SOC_VRF_MAX(l2)+1)){return SOC_E_PARAM;}l353 = MAX_VRF_ID*sizeof(l331);l354 = 
sal_alloc(l353,"_alpm_dbg_cnt");if(l354 == NULL){return SOC_E_MEMORY;}
sal_memset(l354,0,l353);l355.v4 = ALPM_IPV4_BKT_COUNT;l355.v6_64 = 
ALPM_IPV6_64_BKT_COUNT;l355.v6_128 = ALPM_IPV6_128_BKT_COUNT;if(!
soc_th_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l355.v6_64<<= 1;l355.
v6_128<<= 1;}LOG_CLI((BSL_META_U(l2,"\nBucket Occupancy:\n")));if(flags&(
SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l2,
"\n  BKT/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l2,"-----------------------------------------------\n")));}for(
l181 = 0;l181<MAX_PIVOT_COUNT;l181++){alpm_pivot_t*l357 = ALPM_TCAM_PIVOT(l2,
l181);if(l357!= NULL){l331*l358;int l28 = PIVOT_BUCKET_VRF(l357);if(l28<0||
l28>(SOC_VRF_MAX(l2)+1)){continue;}if(l162!= -1&&l162!= l28){continue;}if(
flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l301 = 1;do{LOG_CLI((BSL_META_U(l2,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l181,l28,PIVOT_BUCKET_MIN(l357),
PIVOT_BUCKET_MAX(l357),PIVOT_BUCKET_COUNT(l357),PIVOT_BUCKET_DEF(l357)?"Def":
(l28) == SOC_VRF_MAX(l2)+1?"GLo":""));}while(0);}l358 = &l354[l28];if(
PIVOT_BUCKET_IPV6(l357) == L3_DEFIP_MODE_128){l358->v6_128+= 
PIVOT_BUCKET_COUNT(l357);l358->l329+= l355.v6_128;}else if(PIVOT_BUCKET_IPV6(
l357) == L3_DEFIP_MODE_64){l358->v6_64+= PIVOT_BUCKET_COUNT(l357);l358->l328
+= l355.v6_64;}else{l358->v4+= PIVOT_BUCKET_COUNT(l357);l358->l327+= l355.v4;
}l358->l330 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l301 == 0){
LOG_CLI((BSL_META_U(l2,"%9s\n"),"Specific VRF not found"));}}sal_memset(&l356
,0,sizeof(l356));l301 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){LOG_CLI((
BSL_META_U(l2,"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((
BSL_META_U(l2,"-----------------------------------------------\n")));for(l181
= 0;l181<MAX_VRF_ID;l181++){l331*l358;if(l354[l181].l330!= TRUE){continue;}if
(l162!= -1&&l162!= l181){continue;}l301 = 1;l358 = &l354[l181];do{(&l356)->v4
+= (l358)->v4;(&l356)->l327+= (l358)->l327;(&l356)->v6_64+= (l358)->v6_64;(&
l356)->l328+= (l358)->l328;(&l356)->v6_128+= (l358)->v6_128;(&l356)->l329+= (
l358)->l329;}while(0);do{LOG_CLI((BSL_META_U(l2,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l181),(l358
->l327)?(l358->v4)*100/(l358->l327):0,(l358->l327)?(l358->v4)*1000/(l358->
l327)%10:0,(l358->l328)?(l358->v6_64)*100/(l358->l328):0,(l358->l328)?(l358->
v6_64)*1000/(l358->l328)%10:0,(l358->l329)?(l358->v6_128)*100/(l358->l329):0,
(l358->l329)?(l358->v6_128)*1000/(l358->l329)%10:0,((l358->l327+l358->l328+
l358->l329))?((l358->v4+l358->v6_64+l358->v6_128))*100/((l358->l327+l358->
l328+l358->l329)):0,((l358->l327+l358->l328+l358->l329))?((l358->v4+l358->
v6_64+l358->v6_128))*1000/((l358->l327+l358->l328+l358->l329))%10:0,(l181) == 
SOC_VRF_MAX(l2)+1?"GLo":""));}while(0);}if(l301 == 0){LOG_CLI((BSL_META_U(l2,
"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l2,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l2,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l356)->l327)?((&l356)->v4)*100/((&l356)->l327):0,((&l356)->l327)?((&l356)->v4
)*1000/((&l356)->l327)%10:0,((&l356)->l328)?((&l356)->v6_64)*100/((&l356)->
l328):0,((&l356)->l328)?((&l356)->v6_64)*1000/((&l356)->l328)%10:0,((&l356)->
l329)?((&l356)->v6_128)*100/((&l356)->l329):0,((&l356)->l329)?((&l356)->
v6_128)*1000/((&l356)->l329)%10:0,(((&l356)->l327+(&l356)->l328+(&l356)->l329
))?(((&l356)->v4+(&l356)->v6_64+(&l356)->v6_128))*100/(((&l356)->l327+(&l356)
->l328+(&l356)->l329)):0,(((&l356)->l327+(&l356)->l328+(&l356)->l329))?(((&
l356)->v4+(&l356)->v6_64+(&l356)->v6_128))*1000/(((&l356)->l327+(&l356)->l328
+(&l356)->l329))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l342(l2,l162,alpm_vrf_handle[l2],l336);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_CNT
){l342(l2,l162,alpm_vrf_handle[l2],l333);l342(l2,l162,alpm_vrf_handle[l2],
l334);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l342(l2,l162,alpm_vrf_handle
[l2],l335);l342(l2,l162,alpm_vrf_handle[l2],l338);l342(l2,l162,
alpm_vrf_handle[l2],l337);}sal_free(l354);return SOC_E_NONE;}
#endif

#endif /* ALPM_ENABLE */
