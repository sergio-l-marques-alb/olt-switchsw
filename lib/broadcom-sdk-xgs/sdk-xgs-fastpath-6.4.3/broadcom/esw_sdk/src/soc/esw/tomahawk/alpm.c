/*
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
 * $Id: alpm.c$
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
#include <soc/tomahawk.h>
#include <shared/bsl.h>

#ifdef ALPM_ENABLE
#include <shared/util.h>
#include <shared/l3.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#include <soc/esw/trie.h>

#if 1
soc_alpm_bucket_t soc_th_alpm_bucket[SOC_MAX_NUM_DEVICES];void
soc_th_alpm_lpm_state_dump(int l1);static int l2(int l1);static int l3(int l1
);static int l4(int l1,void*l5,int*index);static int soc_th_alpm_lpm_delete(
int l1,void*key_data);static int l6(int l1,void*key_data,int l7,int l8,int l9
,defip_aux_scratch_entry_t*l10);static int l11(int l1,void*key_data,void*l12,
int*l13,int*l14,int*l7);static int l15(int l1,void*key_data,void*l12,int*l13)
;static int l16(int l1);static int l17(int unit,void*lpm_entry,void*l18,void*
l19,soc_mem_t l20,uint32 l21,uint32*l22);static int l23(int unit,void*l18,
soc_mem_t l20,int l7,int l24,int l25,int index,void*lpm_entry);static int l26
(int unit,uint32*key,int len,int l24,int l7,defip_entry_t*lpm_entry,int l27,
int l28);static int l29(int l1,int l24,int l30);static int l31(int l1,
alpm_pfx_info_t*l32,trie_t*l33,uint32*l34,uint32 l35,trie_node_t*l36,
defip_entry_t*lpm_entry,uint32*l37);int soc_th_alpm_lpm_vrf_get(int unit,void
*lpm_entry,int*l24,int*l38);static int l39(int l1,alpm_pfx_info_t*l32,int*l40
,int*l13);int _soc_th_alpm_rollback_bkt_move(int l1,void*key_data,soc_mem_t
l20,alpm_pivot_t*l41,alpm_pivot_t*l42,alpm_mem_prefix_array_t*l43,int*l44,int
l45);int soc_th_alpm_lpm_delete(int l1,void*key_data);void
_soc_th_alpm_rollback_pivot_add(int l1,defip_entry_t*l46,void*key_data,int
tcam_index,alpm_pivot_t*pivot_pyld);typedef struct l47{int l48;int l49;int l50
;int next;int l51;int l52;}l53,*l54;static l54 l55[SOC_MAX_NUM_DEVICES];
typedef struct l56{soc_field_info_t*l57;soc_field_info_t*l58;soc_field_info_t
*l59;soc_field_info_t*l60;soc_field_info_t*l61;soc_field_info_t*l62;
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
soc_field_info_t*l93;soc_field_info_t*l94;soc_field_info_t*l95;
soc_field_info_t*l96;soc_field_info_t*l97;soc_field_info_t*l98;
soc_field_info_t*l99;soc_field_info_t*l100;soc_field_info_t*l101;
soc_field_info_t*l102;}l103,*l104;static l104 l105[SOC_MAX_NUM_DEVICES];
typedef struct l106{int unit;int l107;int l108;uint16*l109;uint16*l110;}l111;
typedef uint32 l112[5];typedef int(*l113)(l112 l114,l112 l115);static l111*
l116[SOC_MAX_NUM_DEVICES];static void l117(int l1,void*l12,int index,l112 l118
);static uint16 l119(uint8*l120,int l121);static int l122(int unit,int l107,
int l108,l111**l123);static int l124(l111*l125);static int l126(l111*l127,
l113 l128,l112 entry,int l129,uint16*l130);static int l131(l111*l127,l113 l128
,l112 entry,int l129,uint16 l132,uint16 l44);static int l133(l111*l127,l113
l128,l112 entry,int l129,uint16 l134);extern alpm_vrf_handle_t*
alpm_vrf_handle[SOC_MAX_NUM_DEVICES];extern alpm_pivot_t**tcam_pivot[
SOC_MAX_NUM_DEVICES];extern int l135(int unit);extern int
soc_th_get_alpm_banks(int unit);extern int _alpm_128_prefix_create(int l1,
void*entry,uint32*l136,uint32*l14,int*l22);int soc_th_alpm_mode_get(int l1){
uint32 l137;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l1,&l137));return
soc_reg_field_get(l1,L3_DEFIP_RPF_CONTROLr,l137,LOOKUP_MODEf);}void
soc_th_alpm_bank_db_type_get(int l1,int l24,uint32*l138,uint32*l8){if(l24 == 
SOC_VRF_MAX(l1)+1){if(l8){*l8 = 0;}if(soc_th_get_alpm_banks(l1)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,*l138);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,*l138);}}else{if(l8){*l8 = 2;}if(
soc_th_get_alpm_banks(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,
*l138);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,*l138);}}}int
_soc_th_alpm_rpf_entry(int l1,int l139){int l140;int l141 = 
soc_th_get_alpm_banks(l1)/2;l140 = (l139>>l141)&SOC_TH_ALPM_BKT_MASK;l140+= 
SOC_ALPM_BUCKET_COUNT(l1);return(l139&~(SOC_TH_ALPM_BKT_MASK<<l141))|(l140<<
l141);}int soc_th_alpm_physical_idx(int l1,soc_mem_t l20,int index,int l142){
int l143 = index&1;if(l142){return soc_trident2_l3_defip_index_map(l1,l20,
index);}index>>= 1;index = soc_trident2_l3_defip_index_map(l1,l20,index);
index<<= 1;index|= l143;return index;}int soc_th_alpm_logical_idx(int l1,
soc_mem_t l20,int index,int l142){int l143 = index&1;if(l142){return
soc_trident2_l3_defip_index_remap(l1,l20,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_remap(l1,l20,index);index<<= 1;index|= l143;
return index;}static int l144(int l1,void*entry,uint32*prefix,uint32*l14,int*
l22){int l145,l146,l7;int l129 = 0;int l147 = SOC_E_NONE;uint32 l148,l143;
prefix[0] = prefix[1] = prefix[2] = prefix[3] = prefix[4] = 0;l7 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f);l145 = soc_mem_field32_get(l1,
L3_DEFIPm,entry,IP_ADDR0f);l146 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK0f);prefix[1] = l145;l145 = soc_mem_field32_get(l1,L3_DEFIPm,
entry,IP_ADDR1f);l146 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK1f
);prefix[0] = l145;if(l7){prefix[4] = prefix[1];prefix[3] = prefix[0];prefix[
1] = prefix[0] = 0;l146 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK0f);if((l147 = _ipmask2pfx(l146,&l129))<0){return(l147);}l146 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK1f);if(l129){if(l146!= 
0xffffffff){return(SOC_E_PARAM);}l129+= 32;}else{if((l147 = _ipmask2pfx(l146,
&l129))<0){return(l147);}}l148 = 64-l129;if(l148<32){prefix[4]>>= l148;l143 = 
(((32-l148) == 32)?0:(prefix[3])<<(32-l148));prefix[3]>>= l148;prefix[4]|= 
l143;}else{prefix[4] = (((l148-32) == 32)?0:(prefix[3])>>(l148-32));prefix[3]
= 0;}}else{l146 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((
l147 = _ipmask2pfx(l146,&l129))<0){return(l147);}prefix[1] = (((32-l129) == 
32)?0:(prefix[1])>>(32-l129));prefix[0] = 0;}*l14 = l129;*l22 = (prefix[0] == 
0)&&(prefix[1] == 0)&&(l129 == 0);return SOC_E_NONE;}int
_soc_th_alpm_find_in_bkt(int l1,soc_mem_t l20,int bucket_index,int l138,
uint32*l12,void*alpm_data,int*l130,int l30){int l147;l147 = 
soc_mem_alpm_lookup(l1,l20,bucket_index,MEM_BLOCK_ANY,l138,l12,alpm_data,l130
);if(SOC_SUCCESS(l147)){return l147;}if(SOC_TH_ALPM_V6_SCALE_CHECK(l1,l30)){
return soc_mem_alpm_lookup(l1,l20,bucket_index+1,MEM_BLOCK_ANY,l138,l12,
alpm_data,l130);}return l147;}static int l149(int l1,uint32*prefix,uint32 l35
,int l30,int l24,int*l150,int*tcam_index,int*bucket_index){int l147 = 
SOC_E_NONE;trie_t*l151;trie_node_t*l152 = NULL;alpm_pivot_t*pivot_pyld;if(l30
){l151 = VRF_PIVOT_TRIE_IPV6(l1,l24);}else{l151 = VRF_PIVOT_TRIE_IPV4(l1,l24)
;}l147 = trie_find_lpm(l151,prefix,l35,&l152);if(SOC_FAILURE(l147)){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Pivot find failed\n")));return l147;}
pivot_pyld = (alpm_pivot_t*)l152;*l150 = 1;*tcam_index = PIVOT_TCAM_INDEX(
pivot_pyld);*bucket_index = PIVOT_BUCKET_INDEX(pivot_pyld);return SOC_E_NONE;
}static int l153(int l1,void*key_data,soc_mem_t l20,void*alpm_data,int*
tcam_index,int*bucket_index,int*l13){uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int
l154,l24,l30;int l130;uint32 l8,l138;int l147 = SOC_E_NONE;int l150 = 0;l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);if(l30){if(!(l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l154,&l24));if(l154
== 0){if(soc_th_alpm_mode_get(l1)){return SOC_E_PARAM;}}
soc_th_alpm_bank_db_type_get(l1,l24,&l138,&l8);if(!(((l154 == 
SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)
&&(l154 == SOC_L3_VRF_GLOBAL))))){uint32 prefix[5],l35;int l22 = 0;l147 = 
l144(l1,key_data,prefix,&l35,&l22);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: prefix create failed\n")));
return l147;}l147 = l149(l1,prefix,l35,l30,l24,&l150,tcam_index,bucket_index)
;SOC_IF_ERROR_RETURN(l147);if(l150){l17(l1,key_data,l12,0,l20,0,0);l147 = 
_soc_th_alpm_find_in_bkt(l1,l20,*bucket_index,l138,l12,alpm_data,&l130,l30);
if(SOC_SUCCESS(l147)){*l13 = l130;}}else{l147 = SOC_E_NOT_FOUND;}}return l147
;}static int l155(int l1,void*key_data,void*alpm_data,void*alpm_sip_data,
soc_mem_t l20,int l130){defip_aux_scratch_entry_t l10;int l154,l30,l24;int
bucket_index;uint32 l8,l138;int l147 = SOC_E_NONE;int l150 = 0,l143 = 0;int
tcam_index;l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);if(l30){if
(!(l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l154,
&l24));soc_th_alpm_bank_db_type_get(l1,l24,&l138,&l8);if(!
soc_th_alpm_mode_get(l1)){l8 = 2;}if(l154!= SOC_L3_VRF_OVERRIDE){sal_memset(&
l10,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(l1,key_data,
l30,l8,0,&l10));SOC_IF_ERROR_RETURN(soc_mem_write(l1,l20,MEM_BLOCK_ANY,l130,
alpm_data));if(SOC_URPF_STATUS_GET(l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,
l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l130),alpm_sip_data));if(l147!= 
SOC_E_NONE){return SOC_E_FAIL;}}l143 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,REPLACE_LENf,l143);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l150,&tcam_index,&
bucket_index));if(SOC_URPF_STATUS_GET(l1)){l143 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf);l143+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf,l143);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l150,&tcam_index,&
bucket_index));}}return l147;}int _soc_th_alpm_mem_prefix_array_cb(
trie_node_t*node,void*l156){alpm_mem_prefix_array_t*l157 = (
alpm_mem_prefix_array_t*)l156;if(node->type == PAYLOAD){l157->prefix[l157->
count] = (payload_t*)node;l157->count++;}return SOC_E_NONE;}static int l158(
int l1,int l40,int l30,int l37){int l147,l143,index;defip_aux_table_entry_t
entry;index = l40>>(l30?0:1);l147 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l147);if(l30){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l37);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l37);l143 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l40&1){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l37);l143 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l37);l143 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l147 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l147);if(SOC_URPF_STATUS_GET(l1)){l143++;if(l30){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l37);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l37);}else{if(
l40&1){soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l37);}
else{soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l37);}}
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l143);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l143);index+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l147 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}
return l147;}static int l159(int l1,int l160,void*entry,void*l161,int l162){
uint32 l143,l146,l30,l8,l163 = 0;soc_mem_t l20 = L3_DEFIPm;soc_mem_t l164 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l165;int l147 = SOC_E_NONE,l129,l166,l24,
l167;SOC_IF_ERROR_RETURN(soc_mem_read(l1,l164,MEM_BLOCK_ANY,l160,l161));l143 = 
soc_mem_field32_get(l1,l20,entry,VRF_ID_0f);soc_mem_field32_set(l1,l164,l161,
VRF0f,l143);l143 = soc_mem_field32_get(l1,l20,entry,VRF_ID_1f);
soc_mem_field32_set(l1,l164,l161,VRF1f,l143);l143 = soc_mem_field32_get(l1,
l20,entry,MODE0f);soc_mem_field32_set(l1,l164,l161,MODE0f,l143);l143 = 
soc_mem_field32_get(l1,l20,entry,MODE1f);soc_mem_field32_set(l1,l164,l161,
MODE1f,l143);l30 = l143;l143 = soc_mem_field32_get(l1,l20,entry,VALID0f);
soc_mem_field32_set(l1,l164,l161,VALID0f,l143);l143 = soc_mem_field32_get(l1,
l20,entry,VALID1f);soc_mem_field32_set(l1,l164,l161,VALID1f,l143);l143 = 
soc_mem_field32_get(l1,l20,entry,IP_ADDR_MASK0f);if((l147 = _ipmask2pfx(l143,
&l129))<0){return l147;}l146 = soc_mem_field32_get(l1,l20,entry,
IP_ADDR_MASK1f);if((l147 = _ipmask2pfx(l146,&l166))<0){return l147;}if(l30){
soc_mem_field32_set(l1,l164,l161,IP_LENGTH0f,l129+l166);soc_mem_field32_set(
l1,l164,l161,IP_LENGTH1f,l129+l166);}else{soc_mem_field32_set(l1,l164,l161,
IP_LENGTH0f,l129);soc_mem_field32_set(l1,l164,l161,IP_LENGTH1f,l166);}l143 = 
soc_mem_field32_get(l1,l20,entry,IP_ADDR0f);soc_mem_field32_set(l1,l164,l161,
IP_ADDR0f,l143);l143 = soc_mem_field32_get(l1,l20,entry,IP_ADDR1f);
soc_mem_field32_set(l1,l164,l161,IP_ADDR1f,l143);l143 = soc_mem_field32_get(
l1,l20,entry,ENTRY_TYPE0f);soc_mem_field32_set(l1,l164,l161,ENTRY_TYPE0f,l143
);l143 = soc_mem_field32_get(l1,l20,entry,ENTRY_TYPE1f);soc_mem_field32_set(
l1,l164,l161,ENTRY_TYPE1f,l143);if(!l30){sal_memcpy(&l165,entry,sizeof(l165))
;l147 = soc_th_alpm_lpm_vrf_get(l1,(void*)&l165,&l24,&l129);
SOC_IF_ERROR_RETURN(l147);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_ip4entry1_to_0(
l1,&l165,&l165,PRESERVE_HIT));l147 = soc_th_alpm_lpm_vrf_get(l1,(void*)&l165,
&l167,&l129);SOC_IF_ERROR_RETURN(l147);}else{l147 = soc_th_alpm_lpm_vrf_get(
l1,entry,&l24,&l129);}if(SOC_URPF_STATUS_GET(l1)){if(l162>= (
soc_mem_index_count(l1,L3_DEFIPm)>>1)){l163 = 1;}}switch(l24){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l1,l164,l161,VALID0f,0);l8 = 0;break;
case SOC_L3_VRF_GLOBAL:l8 = l163?1:0;break;default:l8 = l163?3:2;break;}
soc_mem_field32_set(l1,l164,l161,DB_TYPE0f,l8);if(!l30){switch(l167){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l1,l164,l161,VALID1f,0);l8 = 0;break;
case SOC_L3_VRF_GLOBAL:l8 = l163?1:0;break;default:l8 = l163?3:2;break;}
soc_mem_field32_set(l1,l164,l161,DB_TYPE1f,l8);}else{if(l24 == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l1,l164,l161,VALID1f,0);}
soc_mem_field32_set(l1,l164,l161,DB_TYPE1f,l8);}if(l163){l143 = 
soc_mem_field32_get(l1,l20,entry,ALG_BKT_PTR0f);if(l143){l143+= 
SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l20,entry,ALG_BKT_PTR0f,l143
);}if(!l30){l143 = soc_mem_field32_get(l1,l20,entry,ALG_BKT_PTR1f);if(l143){
l143+= SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l20,entry,
ALG_BKT_PTR1f,l143);}}}return SOC_E_NONE;}static int l168(int l1,int l169,int
index,int l170,void*entry){defip_aux_table_entry_t l161;l170 = 
soc_th_alpm_physical_idx(l1,L3_DEFIPm,l170,1);SOC_IF_ERROR_RETURN(l159(l1,
l170,entry,(void*)&l161,index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l1,
MEM_BLOCK_ANY,index,entry));index = soc_th_alpm_physical_idx(l1,L3_DEFIPm,
index,1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,index
,&l161));return SOC_E_NONE;}int _soc_th_alpm_insert_in_bkt(int l1,soc_mem_t
l20,int bucket_index,int l138,void*alpm_data,uint32*l12,int*l130,int l30){int
l147;l147 = soc_mem_alpm_insert(l1,l20,bucket_index,MEM_BLOCK_ANY,l138,
alpm_data,l12,l130);if(l147 == SOC_E_FULL){if(SOC_TH_ALPM_V6_SCALE_CHECK(l1,
l30)){return soc_mem_alpm_insert(l1,l20,bucket_index+1,MEM_BLOCK_ANY,l138,
alpm_data,l12,l130);}}return l147;}int l171(int l1,soc_mem_t l20,int
bucket_index,int l172,uint32 l138,int*l173){int l174,l175 = 0;int l176[4] = {
0};int l177 = 0;int l178 = 0;int l179;int l180 = 6;int l181;int l182;int l183
;int l184 = 0;switch(l20){case L3_DEFIP_ALPM_IPV6_64m:l180 = 4;break;case
L3_DEFIP_ALPM_IPV6_128m:l180 = 2;break;default:break;}if(
SOC_TH_ALPM_V6_SCALE_CHECK(l1,(l20!= L3_DEFIP_ALPM_IPV4m))){if(l172>= 
ALPM_RAW_BKT_COUNT*l180){bucket_index++;l172-= ALPM_RAW_BKT_COUNT*l180;}}l181
= 4;l182 = 15;l183 = 2;if(soc_th_get_alpm_banks(l1)<= 2){l181 = 2;l182 = 14;
l183 = 1;}l184 = ((1<<l181)-1);l179 = l181-_shr_popcount(l138&l184);if(
bucket_index>= (1<<l182)||l172>= l179*l180){return SOC_E_PARAM;}l178 = l172%
l180;for(l174 = 0;l174<l181;l174++){if((1<<l174)&l138){continue;}l176[l175++]
= l174;}l177 = l176[l172/l180];*l173 = (l178<<l182)|(bucket_index<<l183)|(
l177);return SOC_E_NONE;}static int l185(int l1,soc_mem_t l20,int l30,
alpm_mem_prefix_array_t*l157,int*l44){int l174,l147 = SOC_E_NONE,l186;
defip_alpm_ipv4_entry_t l187,l188;defip_alpm_ipv6_64_entry_t l189,l190;void*
l191 = NULL,*l192 = NULL;int l193,l194;int*l195 = NULL;int l196 = FALSE;if(
l30){l193 = sizeof(l189);l194 = sizeof(l190);}else{l193 = sizeof(l187);l194 = 
sizeof(l188);}l191 = sal_alloc(l193*l157->count,"rb_bufp");if(l191 == NULL){
l147 = SOC_E_MEMORY;goto l197;}l192 = sal_alloc(l194*l157->count,
"rb_sip_bufp");if(l192 == NULL){l147 = SOC_E_MEMORY;goto l197;}l195 = 
sal_alloc(sizeof(*l195)*l157->count,"roll_back_index");if(l195 == NULL){l147 = 
SOC_E_MEMORY;goto l197;}sal_memset(l195,-1,sizeof(*l195)*l157->count);for(
l174 = 0;l174<l157->count;l174++){payload_t*prefix = l157->prefix[l174];if(
prefix->index>= 0){l147 = soc_mem_read(l1,l20,MEM_BLOCK_ANY,prefix->index,(
uint8*)l191+l174*l193);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x 0x%08x"" 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l174--;l196 = TRUE;
break;}if(SOC_URPF_STATUS_GET(l1)){l147 = soc_mem_read(l1,l20,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,prefix->index),(uint8*)l192+l174*l194);if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x"" 0x%08x 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l174--;l196 = TRUE;
break;}}l147 = soc_mem_write(l1,l20,MEM_BLOCK_ALL,prefix->index,
soc_mem_entry_null(l1,l20));if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old bkt for pfx 0x%08x 0x%08x\n"),prefix->key[0],prefix->key[
1]));l195[l174] = prefix->index;l196 = TRUE;break;}if(SOC_URPF_STATUS_GET(l1)
){l147 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,prefix
->index),soc_mem_entry_null(l1,l20));if(SOC_FAILURE(l147)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old urpf bkt for pfx 0x%08x"" 0x%08x\n"),prefix->key[0],
prefix->key[1]));l195[l174] = prefix->index;l196 = TRUE;break;}}}l195[l174] = 
prefix->index;prefix->index = l44[l174];}if(l196){for(;l174>= 0;l174--){
payload_t*prefix = l157->prefix[l174];prefix->index = l195[l174];if(l195[l174
]<0){continue;}l186 = soc_mem_write(l1,l20,MEM_BLOCK_ALL,l195[l174],(uint8*)
l191+l174*l193);if(SOC_FAILURE(l186)){break;}if(!SOC_URPF_STATUS_GET(l1)){
continue;}l186 = soc_mem_write(l1,l20,MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(l1
,l195[l174]),(uint8*)l192+l174*l194);if(SOC_FAILURE(l186)){break;}}}l197:if(
l195){sal_free(l195);l195 = NULL;}if(l192){sal_free(l192);l192 = NULL;}if(
l191){sal_free(l191);l191 = NULL;}return l147;}void
_soc_th_alpm_rollback_pivot_add(int l1,defip_entry_t*l46,void*key_data,int
tcam_index,alpm_pivot_t*pivot_pyld){int l147;trie_t*l151 = NULL;int l30,l24,
l154;trie_node_t*l198 = NULL;l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,
MODE0f);soc_th_alpm_lpm_vrf_get(l1,key_data,&l154,&l24);l147 = 
soc_th_alpm_lpm_delete(l1,l46);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failure to free new prefix"
"at %d\n"),soc_th_alpm_logical_idx(l1,L3_DEFIPm,tcam_index,l30)));}if(l30){
l151 = VRF_PIVOT_TRIE_IPV6(l1,l24);}else{l151 = VRF_PIVOT_TRIE_IPV4(l1,l24);}
if(ALPM_TCAM_PIVOT(l1,tcam_index<<(l30?1:0))!= NULL){l147 = trie_delete(l151,
pivot_pyld->key,pivot_pyld->len,&l198);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: trie delete failure "
"in bkt move rollback\n")));}}ALPM_TCAM_PIVOT(l1,tcam_index<<(l30?1:0)) = 
NULL;VRF_PIVOT_REF_DEC(l1,l24,l30);}int _soc_th_alpm_rollback_bkt_move(int l1
,void*key_data,soc_mem_t l20,alpm_pivot_t*l41,alpm_pivot_t*l42,
alpm_mem_prefix_array_t*l43,int*l44,int l45){trie_node_t*l198 = NULL;uint32
prefix[5],l35;trie_t*l33;int l30,l24,l154,l174,l22 = 0;;
defip_alpm_ipv4_entry_t l187;defip_alpm_ipv6_64_entry_t l189;
defip_alpm_ipv6_128_entry_t l199;void*l200;payload_t*l201;int l147 = 
SOC_E_NONE;alpm_bucket_handle_t*l202;l202 = PIVOT_BUCKET_HANDLE(l42);l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);if(l20 == 
L3_DEFIP_ALPM_IPV6_128m){l30 = L3_DEFIP_MODE_128;}if(l20 == 
L3_DEFIP_ALPM_IPV6_128m){l147 = _alpm_128_prefix_create(l1,key_data,prefix,&
l35,&l22);}else{l147 = l144(l1,key_data,prefix,&l35,&l22);}if(SOC_FAILURE(
l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"prefix create failed\n")));
return l147;}if(l20 == L3_DEFIP_ALPM_IPV6_128m){l200 = ((uint32*)&(l199));
soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l154,&l24);}else{l200 = ((l30)?((
uint32*)&(l189)):((uint32*)&(l187)));soc_th_alpm_lpm_vrf_get(l1,key_data,&
l154,&l24);}if(l20 == L3_DEFIP_ALPM_IPV6_128m){l33 = VRF_PREFIX_TRIE_IPV6_128
(l1,l24);}else if(l30){l33 = VRF_PREFIX_TRIE_IPV6(l1,l24);}else{l33 = 
VRF_PREFIX_TRIE_IPV4(l1,l24);}for(l174 = 0;l174<l43->count;l174++){payload_t*
l129 = l43->prefix[l174];if(l44[l174]!= -1){if(l20 == L3_DEFIP_ALPM_IPV6_128m
){sal_memset(l200,0,sizeof(defip_alpm_ipv6_128_entry_t));}else if(l30){
sal_memset(l200,0,sizeof(defip_alpm_ipv6_64_entry_t));}else{sal_memset(l200,0
,sizeof(defip_alpm_ipv4_entry_t));}l147 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,
l44[l174],l200);_soc_tomahawk_alpm_bkt_view_set(l1,l44[l174],INVALIDm);if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l1)){l147 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l44[l174]),l200);_soc_tomahawk_alpm_bkt_view_set(l1
,_soc_th_alpm_rpf_entry(l1,l44[l174]),INVALIDm);if(SOC_FAILURE(l147)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l198 = NULL;l147 = trie_delete(
PIVOT_BUCKET_TRIE(l42),l129->key,l129->len,&l198);l201 = (payload_t*)l198;if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(l129->
index>0){l147 = trie_insert(PIVOT_BUCKET_TRIE(l41),l129->key,NULL,l129->len,(
trie_node_t*)l201);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l201!= NULL){sal_free(l201);}}}if(l45 == 
-1){l198 = NULL;l147 = trie_delete(PIVOT_BUCKET_TRIE(l41),prefix,l35,&l198);
l201 = (payload_t*)l198;if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l201!= 
NULL){sal_free(l201);}}l147 = soc_th_alpm_bucket_release(l1,
PIVOT_BUCKET_INDEX(l42),l30);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l1,"_soc_th_alpm_insert: new bucket release failure: %d\n"),
PIVOT_BUCKET_INDEX(l42)));}trie_destroy(PIVOT_BUCKET_TRIE(l42));sal_free(l202
);sal_free(l42);sal_free(l44);l198 = NULL;l147 = trie_delete(l33,prefix,l35,&
l198);l201 = (payload_t*)l198;if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_th_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l201){sal_free(
l201);}return l147;}int _soc_th_alpm_free_pfx_trie(int l1,trie_t*l33,trie_t*
l203,payload_t*new_pfx_pyld,int*l44,int bucket_index,int l30){trie_node_t*
l198 = NULL;payload_t*l204 = NULL;if(l44!= NULL){sal_free(l44);}l198 = NULL;(
void)trie_delete(l33,new_pfx_pyld->key,new_pfx_pyld->len,&l198);new_pfx_pyld = 
(payload_t*)l198;if(bucket_index!= -1){(void)soc_th_alpm_bucket_release(l1,
bucket_index,l30);}(void)trie_delete(l203,new_pfx_pyld->key,new_pfx_pyld->len
,&l198);l204 = (payload_t*)l198;if(l204!= NULL){sal_free(l204);}if(
new_pfx_pyld!= NULL){sal_free(new_pfx_pyld);}return SOC_E_NONE;}static int l31
(int l1,alpm_pfx_info_t*l32,trie_t*l33,uint32*l34,uint32 l35,trie_node_t*l36,
defip_entry_t*lpm_entry,uint32*l37){trie_node_t*l152 = NULL;int l30,l24,l154;
defip_alpm_ipv4_entry_t l187;defip_alpm_ipv6_64_entry_t l189;payload_t*l204 = 
NULL;int l205;void*l200;alpm_pivot_t*l206;alpm_bucket_handle_t*l202;int l147 = 
SOC_E_NONE;soc_mem_t l20;l206 = l32->pivot_pyld;l205 = l206->tcam_index;l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,l32->key_data,MODE0f);SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l1,l32->key_data,&l154,&l24));l20 = (l30)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l200 = ((l30)?((uint32*)&(l189)):(
(uint32*)&(l187)));l152 = NULL;l147 = trie_find_lpm(l33,l34,l35,&l152);l204 = 
(payload_t*)l152;if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l34[0],l34[1],l34[2],
l34[3],l34[4],l35));return l147;}if(l204->bkt_ptr){if(l204->bkt_ptr == l32->
new_pfx_pyld){sal_memcpy(l200,l32->alpm_data,l30?sizeof(
defip_alpm_ipv6_64_entry_t):sizeof(defip_alpm_ipv4_entry_t));}else{l147 = 
soc_mem_read(l1,l20,MEM_BLOCK_ANY,((payload_t*)l204->bkt_ptr)->index,l200);}
if(SOC_FAILURE(l147)){return l147;}l147 = l23(l1,l200,l20,l30,l154,l32->
bucket_index,0,lpm_entry);if(SOC_FAILURE(l147)){return l147;}*l37 = ((
payload_t*)(l204->bkt_ptr))->len;}else{l147 = soc_mem_read(l1,L3_DEFIPm,
MEM_BLOCK_ANY,soc_th_alpm_logical_idx(l1,L3_DEFIPm,l205>>1,1),lpm_entry);if((
!l30)&&(l205&1)){l147 = soc_th_alpm_lpm_ip4entry1_to_0(l1,lpm_entry,lpm_entry
,0);}}l202 = sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(
l202 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l147 = SOC_E_MEMORY;return l147;}sal_memset(l202,0,sizeof(*l202));l206 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l206 == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l147 = SOC_E_MEMORY;return l147;}sal_memset(l206,0,sizeof(*l206));
PIVOT_BUCKET_HANDLE(l206) = l202;if(l30){l147 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l206));}else{l147 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l206));}PIVOT_BUCKET_TRIE(l206)->trie = l36;
PIVOT_BUCKET_INDEX(l206) = l32->bucket_index;PIVOT_BUCKET_VRF(l206) = l24;
PIVOT_BUCKET_IPV6(l206) = l30;PIVOT_BUCKET_DEF(l206) = FALSE;(l206)->key[0] = 
l34[0];(l206)->key[1] = l34[1];(l206)->key[2] = l34[2];(l206)->key[3] = l34[3
];(l206)->key[4] = l34[4];(l206)->len = l35;do{if(!(l30)){l34[0] = (((32-l35)
== 32)?0:(l34[1])<<(32-l35));l34[1] = 0;}else{int l207 = 64-l35;int l208;if(
l207<32){l208 = l34[3]<<l207;l208|= (((32-l207) == 32)?0:(l34[4])>>(32-l207))
;l34[0] = l34[4]<<l207;l34[1] = l208;l34[2] = l34[3] = l34[4] = 0;}else{l34[1
] = (((l207-32) == 32)?0:(l34[4])<<(l207-32));l34[0] = l34[2] = l34[3] = l34[
4] = 0;}}}while(0);l26(l1,l34,l35,l24,l30,lpm_entry,0,0);
soc_L3_DEFIPm_field32_set(l1,lpm_entry,ALG_BKT_PTR0f,l32->bucket_index);l32->
pivot_pyld = l206;return l147;}static int l39(int l1,alpm_pfx_info_t*l32,int*
l40,int*l13){trie_node_t*l36;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l30,l24,
l154;uint32 l35,l37 = 0;uint32 l138 = 0;uint32 l34[5];int l130;
defip_alpm_ipv4_entry_t l187,l188;defip_alpm_ipv6_64_entry_t l189,l190;trie_t
*l33,*trie;void*l200,*l209;alpm_pivot_t*l210 = l32->pivot_pyld;defip_entry_t
lpm_entry;soc_mem_t l20;trie_t*l151 = NULL;alpm_mem_prefix_array_t l157;int*
l44 = NULL;int l147 = SOC_E_NONE,l174,l45 = -1;l30 = soc_mem_field32_get(l1,
L3_DEFIPm,l32->key_data,MODE0f);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(
l1,l32->key_data,&l154,&l24));l20 = (l30)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l200 = ((l30)?((uint32*)&(l189)):((uint32*)&(l187)));l209
= ((l30)?((uint32*)&(l190)):((uint32*)&(l188)));soc_th_alpm_bank_db_type_get(
l1,l24,&l138,NULL);if(l30){l33 = VRF_PREFIX_TRIE_IPV6(l1,l24);}else{l33 = 
VRF_PREFIX_TRIE_IPV4(l1,l24);}trie = PIVOT_BUCKET_TRIE(l32->pivot_pyld);l147 = 
soc_th_alpm_bucket_assign(l1,&l32->bucket_index,l30);if(SOC_FAILURE(l147)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_insert: Unable to allocate""new bucket for split\n")));l32->
bucket_index = -1;_soc_th_alpm_free_pfx_trie(l1,l33,trie,l32->new_pfx_pyld,
l44,l32->bucket_index,l30);return l147;}l147 = trie_split(trie,l30?
_MAX_KEY_LEN_144_:_MAX_KEY_LEN_48_,FALSE,l34,&l35,&l36,NULL,FALSE);if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_insert: Could not split bucket")));_soc_th_alpm_free_pfx_trie(
l1,l33,trie,l32->new_pfx_pyld,l44,l32->bucket_index,l30);return l147;}l147 = 
l31(l1,l32,l33,l34,l35,l36,&lpm_entry,&l37);if(l147!= SOC_E_NONE){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"could not initialize pivot")));
_soc_th_alpm_free_pfx_trie(l1,l33,trie,l32->new_pfx_pyld,l44,l32->
bucket_index,l30);return l147;}sal_memset(&l157,0,sizeof(l157));l147 = 
trie_traverse(PIVOT_BUCKET_TRIE(l32->pivot_pyld),
_soc_th_alpm_mem_prefix_array_cb,&l157,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE
(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_insert: Bucket split failed")));_soc_th_alpm_free_pfx_trie(l1,
l33,trie,l32->new_pfx_pyld,l44,l32->bucket_index,l30);return l147;}l44 = 
sal_alloc(sizeof(*l44)*l157.count,
"Temp storage for location of prefixes in new bucket");if(l44 == NULL){l147 = 
SOC_E_MEMORY;_soc_th_alpm_free_pfx_trie(l1,l33,trie,l32->new_pfx_pyld,l44,l32
->bucket_index,l30);return l147;}sal_memset(l44,-1,sizeof(*l44)*l157.count);
for(l174 = 0;l174<l157.count;l174++){payload_t*l129 = l157.prefix[l174];if(
l129->index>0){l147 = soc_mem_read(l1,l20,MEM_BLOCK_ANY,l129->index,l200);if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Failed to""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l157.prefix[l174]->key[1],l157.
prefix[l174]->key[2],l157.prefix[l174]->key[3],l157.prefix[l174]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,l32->key_data,l20,l210,l32->pivot_pyld
,&l157,l44,l45);return l147;}if(SOC_URPF_STATUS_GET(l1)){l147 = soc_mem_read(
l1,l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l129->index),l209);if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l157.prefix[l174]->key[1],l157.
prefix[l174]->key[2],l157.prefix[l174]->key[3],l157.prefix[l174]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,l32->key_data,l20,l210,l32->pivot_pyld
,&l157,l44,l45);return l147;}}l147 = _soc_th_alpm_insert_in_bkt(l1,l20,l32->
bucket_index,l138,l200,l12,&l130,l30);if(SOC_SUCCESS(l147)){if(
SOC_URPF_STATUS_GET(l1)){l147 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l130),l209);}}}else{l147 = 
_soc_th_alpm_insert_in_bkt(l1,l20,l32->bucket_index,l138,l32->alpm_data,l12,&
l130,l30);if(SOC_SUCCESS(l147)){l45 = l174;*l13 = l130;if(SOC_URPF_STATUS_GET
(l1)){l147 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,
l130),l32->alpm_sip_data);}}}l44[l174] = l130;if(SOC_FAILURE(l147)){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_insert: Failed to "
"insert prefix ""0x%08x 0x%08x 0x%08x 0x%08x to bucket %d\n"),l157.prefix[
l174]->key[1],l157.prefix[l174]->key[2],l157.prefix[l174]->key[3],l157.prefix
[l174]->key[4],l32->bucket_index));(void)_soc_th_alpm_rollback_bkt_move(l1,
l32->key_data,l20,l210,l32->pivot_pyld,&l157,l44,l45);return l147;}}l147 = l4
(l1,&lpm_entry,l40);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_insert: Unable to add new""pivot to tcam\n")));if
(l147 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l24,l30);}(void)
_soc_th_alpm_rollback_bkt_move(l1,l32->key_data,l20,l210,l32->pivot_pyld,&
l157,l44,l45);return l147;}*l40 = soc_th_alpm_physical_idx(l1,L3_DEFIPm,*l40,
l30);l147 = l158(l1,*l40,l30,l37);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_insert: Unable to init bpm_len "
"for index %d\n"),*l40));_soc_th_alpm_rollback_pivot_add(l1,&lpm_entry,l32->
key_data,*l40,l32->pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l1,l32->
key_data,l20,l210,l32->pivot_pyld,&l157,l44,l45);return l147;}if(l30){l151 = 
VRF_PIVOT_TRIE_IPV6(l1,l24);}else{l151 = VRF_PIVOT_TRIE_IPV4(l1,l24);}l147 = 
trie_insert(l151,l32->pivot_pyld->key,NULL,l32->pivot_pyld->len,(trie_node_t*
)l32->pivot_pyld);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"failed to insert into pivot trie\n")));(void)
_soc_th_alpm_rollback_bkt_move(l1,l32->key_data,l20,l210,l32->pivot_pyld,&
l157,l44,l45);return l147;}ALPM_TCAM_PIVOT(l1,*l40<<(l30?1:0)) = l32->
pivot_pyld;PIVOT_TCAM_INDEX(l32->pivot_pyld) = *l40<<(l30?1:0);
VRF_PIVOT_REF_INC(l1,l24,l30);l147 = l185(l1,l20,l30,&l157,l44);if(
SOC_FAILURE(l147)){_soc_th_alpm_rollback_pivot_add(l1,&lpm_entry,l32->
key_data,*l40,l32->pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l1,l32->
key_data,l20,l210,l32->pivot_pyld,&l157,l44,l45);sal_free(l44);l44 = NULL;
return l147;}sal_free(l44);if(l45 == -1){l147 = _soc_th_alpm_insert_in_bkt(l1
,l20,PIVOT_BUCKET_INDEX(l210),l138,l32->alpm_data,l12,&l130,l30);if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_insert: Could not insert new ""prefix into trie after split\n")
));_soc_th_alpm_free_pfx_trie(l1,l33,trie,l32->new_pfx_pyld,l44,l32->
bucket_index,l30);return l147;}if(SOC_URPF_STATUS_GET(l1)){l147 = 
soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l130),l32->
alpm_sip_data);}*l13 = l130;l32->new_pfx_pyld->index = l130;}
PIVOT_BUCKET_ENT_CNT_UPDATE(l32->pivot_pyld);VRF_BUCKET_SPLIT_INC(l1,l24,l30)
;return l147;}static int l211(int l1,void*key_data,soc_mem_t l20,void*l212,
void*l213,int*l13,int bucket_index,int tcam_index){alpm_pivot_t*l206,*l210;
defip_aux_scratch_entry_t l10;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l214,l35;int l30,l24,l154;int l130;int l147 = SOC_E_NONE,l186;
uint32 l8,l138;int l150 =0;int l40;int l215 = 0;trie_t*trie,*l33;trie_node_t*
l152 = NULL,*l198 = NULL;payload_t*l201,*l216,*l204;int l22 = 0;
alpm_pfx_info_t l32;l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);
if(l30){if(!(l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE1f))){return
(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l154
,&l24));soc_th_alpm_bank_db_type_get(l1,l24,&l138,&l8);l20 = (l30)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l147 = l144(l1,key_data,prefix,&
l35,&l22);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_insert: prefix create failed\n")));return l147;}sal_memset(&l10
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(l1,key_data,l30,
l8,0,&l10));if(bucket_index == 0){l147 = l149(l1,prefix,l35,l30,l24,&l150,&
tcam_index,&bucket_index);SOC_IF_ERROR_RETURN(l147);if(l150 == 0){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_insert: "
"Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l147 = 
_soc_th_alpm_insert_in_bkt(l1,l20,bucket_index,l138,l212,l12,&l130,l30);if(
l147 == SOC_E_NONE){*l13 = l130;if(SOC_URPF_STATUS_GET(l1)){l186 = 
soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l130),l213);if(
SOC_FAILURE(l186)){return l186;}}}if(l147 == SOC_E_FULL){l215 = 1;}l206 = 
ALPM_TCAM_PIVOT(l1,tcam_index);trie = PIVOT_BUCKET_TRIE(l206);l210 = l206;
l201 = sal_alloc(sizeof(payload_t),"Payload for Key");if(l201 == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 payload entry\n")));
return SOC_E_MEMORY;}l216 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l216 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_vrf_add: unable to allocate memory for "
"pfx trie node\n")));sal_free(l201);return SOC_E_MEMORY;}sal_memset(l201,0,
sizeof(*l201));sal_memset(l216,0,sizeof(*l216));sal_memcpy(l201->key,prefix,
sizeof(prefix));l201->len = l35;l201->index = l130;sal_memcpy(l216,l201,
sizeof(*l201));l216->bkt_ptr = l201;l147 = trie_insert(trie,prefix,NULL,l35,(
trie_node_t*)l201);if(SOC_FAILURE(l147)){if(l201!= NULL){sal_free(l201);}if(
l216!= NULL){sal_free(l216);}return l147;}if(l30){l33 = VRF_PREFIX_TRIE_IPV6(
l1,l24);}else{l33 = VRF_PREFIX_TRIE_IPV4(l1,l24);}if(!l22){l147 = trie_insert
(l33,prefix,NULL,l35,(trie_node_t*)l216);}else{l152 = NULL;l147 = 
trie_find_lpm(l33,0,0,&l152);l204 = (payload_t*)l152;if(SOC_SUCCESS(l147)){
l204->bkt_ptr = l201;}}l214 = l35;if(SOC_FAILURE(l147)){l198 = NULL;(void)
trie_delete(trie,prefix,l214,&l198);l204 = (payload_t*)l198;sal_free(l204);
sal_free(l216);return l147;}if(l215){l32.key_data = key_data;l32.new_pfx_pyld
= l201;l32.pivot_pyld = l206;l32.alpm_data = l212;l32.alpm_sip_data = l213;
l32.bucket_index = bucket_index;l147 = l39(l1,&l32,&l40,l13);if(l147!= 
SOC_E_NONE){return l147;}bucket_index = l32.bucket_index;}VRF_TRIE_ROUTES_INC
(l1,l24,l30);if(l22){sal_free(l216);}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(
l1,DELETE_PROPAGATE,&l10,TRUE,&l150,&tcam_index,&bucket_index));
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l10,FALSE,&l150,
&tcam_index,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l35 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf);l35+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf,l35);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l150,&
tcam_index,&bucket_index));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
INSERT_PROPAGATE,&l10,FALSE,&l150,&tcam_index,&bucket_index));}
PIVOT_BUCKET_ENT_CNT_UPDATE(l210);return l147;}static int l26(int unit,uint32
*key,int len,int l24,int l7,defip_entry_t*lpm_entry,int l27,int l28){uint32
l217;if(l28){sal_memset(lpm_entry,0,sizeof(defip_entry_t));}
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_0f,l24&SOC_VRF_MAX(unit));if(
l24 == (SOC_VRF_MAX(unit)+1)){soc_L3_DEFIPm_field32_set(unit,lpm_entry,
VRF_ID_MASK0f,0);}else{soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_MASK0f
,SOC_VRF_MAX(unit));}if(l7){soc_L3_DEFIPm_field32_set(unit,lpm_entry,
IP_ADDR0f,key[0]);soc_L3_DEFIPm_field32_set(unit,lpm_entry,IP_ADDR1f,key[1]);
soc_L3_DEFIPm_field32_set(unit,lpm_entry,MODE0f,1);soc_L3_DEFIPm_field32_set(
unit,lpm_entry,MODE1f,1);soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_1f,
l24&SOC_VRF_MAX(unit));if(l24 == (SOC_VRF_MAX(unit)+1)){
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_MASK1f,0);}else{
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_MASK1f,SOC_VRF_MAX(unit));}
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VALID1f,1);if(len>= 32){l217 = 
0xffffffff;soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f,l217);
l217 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32));soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l217);}else{l217 = ~(0xffffffff>>len);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f,l217);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,0);}}else{
soc_L3_DEFIPm_field32_set(unit,lpm_entry,IP_ADDR0f,key[0]);assert(len<= 32);
l217 = (len == 32)?0xffffffff:~(0xffffffff>>len);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l217);}soc_L3_DEFIPm_field32_set(unit,
lpm_entry,VALID0f,1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE_MASK0f
,(1<<soc_mem_field_length(unit,L3_DEFIPm,MODE_MASK0f))-1);soc_mem_field32_set
(unit,L3_DEFIPm,lpm_entry,MODE_MASK1f,(1<<soc_mem_field_length(unit,L3_DEFIPm
,MODE_MASK1f))-1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
ENTRY_TYPE_MASK0f,(1<<soc_mem_field_length(unit,L3_DEFIPm,ENTRY_TYPE_MASK0f))
-1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK1f,(1<<
soc_mem_field_length(unit,L3_DEFIPm,ENTRY_TYPE_MASK1f))-1);return(SOC_E_NONE)
;}int _soc_th_alpm_delete_in_bkt(int l1,soc_mem_t l20,int l218,int l138,void*
l219,uint32*l12,int*l130,int l30){int l147;l147 = soc_mem_alpm_delete(l1,l20,
l218,MEM_BLOCK_ALL,l138,l219,l12,l130);if(SOC_SUCCESS(l147)){return l147;}if(
SOC_TH_ALPM_V6_SCALE_CHECK(l1,l30)){return soc_mem_alpm_delete(l1,l20,l218+1,
MEM_BLOCK_ALL,l138,l219,l12,l130);}return l147;}static int l220(int l1,void*
key_data,int bucket_index,int tcam_index,int l130){alpm_pivot_t*pivot_pyld;
defip_alpm_ipv4_entry_t l187,l188;defip_alpm_ipv6_64_entry_t l189,l190;
defip_alpm_ipv4_entry_t l221,l222;defip_aux_scratch_entry_t l10;uint32 l12[
SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l20;void*l200,*l219,*l209 = NULL;int l154;
int l7;int l147 = SOC_E_NONE,l186;uint32 l223[5],prefix[5];int l30,l24;uint32
l35;int l218;uint32 l8,l138;int l150,l22 = 0;trie_t*trie,*l33;uint32 l224;
defip_entry_t lpm_entry,*l225;payload_t*l201 = NULL,*l226 = NULL,*l204 = NULL
;trie_node_t*l198 = NULL,*l152 = NULL;trie_t*l151 = NULL;l7 = l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);if(l30){if(!(l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l154,&l24));if(l154
!= SOC_L3_VRF_OVERRIDE){soc_th_alpm_bank_db_type_get(l1,l24,&l138,&l8);l147 = 
l144(l1,key_data,prefix,&l35,&l22);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_delete: prefix create failed\n")));
return l147;}if(!soc_th_alpm_mode_get(l1)){if(l154!= SOC_L3_VRF_GLOBAL){if(
VRF_TRIE_ROUTES_CNT(l1,l24,l30)>1){if(l22){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"VRF %d: Cannot delete default "
"route if other routes are present in ""this mode"),l24));return SOC_E_PARAM;
}}}l8 = 2;}l20 = (l30)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l200 = ((
l30)?((uint32*)&(l189)):((uint32*)&(l187)));l219 = ((l30)?((uint32*)&(l222)):
((uint32*)&(l221)));SOC_ALPM_LPM_LOCK(l1);if(bucket_index == 0){l147 = l153(
l1,key_data,l20,l200,&tcam_index,&bucket_index,&l130);}else{l147 = l17(l1,
key_data,l200,0,l20,0,0);}sal_memcpy(l219,l200,l30?sizeof(l189):sizeof(l187))
;if(SOC_FAILURE(l147)){SOC_ALPM_LPM_UNLOCK(l1);LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Unable to find prefix for delete\n")));
return l147;}l218 = bucket_index;pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);
trie = PIVOT_BUCKET_TRIE(pivot_pyld);l147 = trie_delete(trie,prefix,l35,&l198
);l201 = (payload_t*)l198;if(l147!= SOC_E_NONE){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l147;}if(l30){l33 = VRF_PREFIX_TRIE_IPV6(l1,
l24);}else{l33 = VRF_PREFIX_TRIE_IPV4(l1,l24);}if(l30){l151 = 
VRF_PIVOT_TRIE_IPV6(l1,l24);}else{l151 = VRF_PIVOT_TRIE_IPV4(l1,l24);}if(!l22
){l147 = trie_delete(l33,prefix,l35,&l198);l226 = (payload_t*)l198;if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l227;}l152 = NULL;l147 = trie_find_lpm(l33,prefix,l35,&
l152);l204 = (payload_t*)l152;if(SOC_SUCCESS(l147)){payload_t*l228 = (
payload_t*)(l204->bkt_ptr);if(l228!= NULL){l224 = l228->len;}else{l224 = 0;}}
else{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l229;}
sal_memcpy(l223,prefix,sizeof(prefix));do{if(!(l30)){l223[0] = (((32-l35) == 
32)?0:(l223[1])<<(32-l35));l223[1] = 0;}else{int l207 = 64-l35;int l208;if(
l207<32){l208 = l223[3]<<l207;l208|= (((32-l207) == 32)?0:(l223[4])>>(32-l207
));l223[0] = l223[4]<<l207;l223[1] = l208;l223[2] = l223[3] = l223[4] = 0;}
else{l223[1] = (((l207-32) == 32)?0:(l223[4])<<(l207-32));l223[0] = l223[2] = 
l223[3] = l223[4] = 0;}}}while(0);l147 = l26(l1,prefix,l224,l24,l7,&lpm_entry
,0,1);l186 = l153(l1,&lpm_entry,l20,l200,&tcam_index,&bucket_index,&l130);(
void)l23(l1,l200,l20,l7,l154,bucket_index,0,&lpm_entry);(void)l26(l1,l223,l35
,l24,l7,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(l147)){
l209 = ((l30)?((uint32*)&(l190)):((uint32*)&(l188)));l186 = soc_mem_read(l1,
l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l130),l209);}}if((l224 == 0)&&
SOC_FAILURE(l186)){l225 = l30?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l24);sal_memcpy(&lpm_entry,l225,sizeof(
lpm_entry));l147 = l26(l1,l223,l35,l24,l7,&lpm_entry,0,1);}if(SOC_FAILURE(
l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l229;}l225 = 
&lpm_entry;}else{l152 = NULL;l147 = trie_find_lpm(l33,prefix,l35,&l152);l204 = 
(payload_t*)l152;if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),l24));goto l227;}l204->bkt_ptr = NULL;l224 = 
0;l225 = l30?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24):VRF_TRIE_DEFAULT_ROUTE_IPV4(
l1,l24);}l147 = l6(l1,l225,l30,l8,l224,&l10);if(SOC_FAILURE(l147)){goto l229;
}l147 = _soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l150,&tcam_index,&
bucket_index);if(SOC_FAILURE(l147)){goto l229;}if(SOC_URPF_STATUS_GET(l1)){
uint32 l143;if(l209!= NULL){l143 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf);l143++;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf,l143);l143 = soc_mem_field32_get(l1,l20,
l209,SRC_DISCARDf);soc_mem_field32_set(l1,l20,&l10,SRC_DISCARDf,l143);l143 = 
soc_mem_field32_get(l1,l20,l209,DEFAULTROUTEf);soc_mem_field32_set(l1,l20,&
l10,DEFAULTROUTEf,l143);l147 = _soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,
TRUE,&l150,&tcam_index,&bucket_index);}if(SOC_FAILURE(l147)){goto l229;}}
sal_free(l201);if(!l22){sal_free(l226);}PIVOT_BUCKET_ENT_CNT_UPDATE(
pivot_pyld);if((pivot_pyld->len!= 0)&&(trie->trie == NULL)){uint32 l230[5];
sal_memcpy(l230,pivot_pyld->key,sizeof(l230));do{if(!(l7)){l230[0] = (((32-
pivot_pyld->len) == 32)?0:(l230[1])<<(32-pivot_pyld->len));l230[1] = 0;}else{
int l207 = 64-pivot_pyld->len;int l208;if(l207<32){l208 = l230[3]<<l207;l208
|= (((32-l207) == 32)?0:(l230[4])>>(32-l207));l230[0] = l230[4]<<l207;l230[1]
= l208;l230[2] = l230[3] = l230[4] = 0;}else{l230[1] = (((l207-32) == 32)?0:(
l230[4])<<(l207-32));l230[0] = l230[2] = l230[3] = l230[4] = 0;}}}while(0);
l26(l1,l230,pivot_pyld->len,l24,l7,&lpm_entry,0,1);l147 = 
soc_th_alpm_lpm_delete(l1,&lpm_entry);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_delete: Unable to "
"delete pivot 0x%08x 0x%08x \n"),pivot_pyld->key[0],pivot_pyld->key[1]));}}
l147 = _soc_th_alpm_delete_in_bkt(l1,l20,l218,l138,l219,l12,&l130,l30);if(!
SOC_SUCCESS(l147)){SOC_ALPM_LPM_UNLOCK(l1);l147 = SOC_E_FAIL;return l147;}if(
SOC_URPF_STATUS_GET(l1)){l147 = soc_mem_alpm_delete(l1,l20,
SOC_ALPM_RPF_BKT_IDX(l1,l218),MEM_BLOCK_ALL,l138,l219,l12,&l150);if(!
SOC_SUCCESS(l147)){SOC_ALPM_LPM_UNLOCK(l1);l147 = SOC_E_FAIL;return l147;}}if
((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l147 = 
soc_th_alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(pivot_pyld),l30);if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to release""empty bucket: %d\n"),PIVOT_BUCKET_INDEX
(pivot_pyld)));}l147 = trie_delete(l151,pivot_pyld->key,pivot_pyld->len,&l198
);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(pivot_pyld));sal_free(pivot_pyld);
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,l218),
INVALIDm);if(SOC_TH_ALPM_V6_SCALE_CHECK(l1,l30)){
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,l218+1),
INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l24,l30);if(VRF_TRIE_ROUTES_CNT(l1,l24,
l30) == 0){l147 = l29(l1,l24,l30);}SOC_ALPM_LPM_UNLOCK(l1);return l147;l229:
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"recovering soc_th_alpm_vrf_delete failed\n ")));l186 = trie_insert(l33,
prefix,NULL,l35,(trie_node_t*)l226);if(SOC_FAILURE(l186)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into pfx trie\n"),prefix[0],prefix[1]));}l227:l186 = 
trie_insert(trie,prefix,NULL,l35,(trie_node_t*)l201);if(SOC_FAILURE(l186)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into bkt trie\n")
,prefix[0],prefix[1]));}SOC_ALPM_LPM_UNLOCK(l1);return l147;}int
soc_th_alpm_init(int l1){int l174;int l147 = SOC_E_NONE;l147 = l2(l1);
SOC_IF_ERROR_RETURN(l147);l147 = l16(l1);alpm_vrf_handle[l1] = sal_alloc((
MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t),"ALPM VRF Handles");if(
alpm_vrf_handle[l1] == NULL){return SOC_E_MEMORY;}tcam_pivot[l1] = sal_alloc(
MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*),"ALPM pivots");if(tcam_pivot[l1] == 
NULL){return SOC_E_MEMORY;}sal_memset(alpm_vrf_handle[l1],0,(MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t));sal_memset(tcam_pivot[l1],0,MAX_PIVOT_COUNT*sizeof
(alpm_pivot_t*));for(l174 = 0;l174<MAX_PIVOT_COUNT;l174++){ALPM_TCAM_PIVOT(l1
,l174) = NULL;}if(SOC_CONTROL(l1)->alpm_bulk_retry == NULL){SOC_CONTROL(l1)->
alpm_bulk_retry = sal_sem_create("ALPM bulk retry",sal_sem_BINARY,0);}if(
SOC_CONTROL(l1)->alpm_lookup_retry == NULL){SOC_CONTROL(l1)->
alpm_lookup_retry = sal_sem_create("ALPM lookup retry",sal_sem_BINARY,0);}if(
SOC_CONTROL(l1)->alpm_insert_retry == NULL){SOC_CONTROL(l1)->
alpm_insert_retry = sal_sem_create("ALPM insert retry",sal_sem_BINARY,0);}if(
SOC_CONTROL(l1)->alpm_delete_retry == NULL){SOC_CONTROL(l1)->
alpm_delete_retry = sal_sem_create("ALPM delete retry",sal_sem_BINARY,0);}
l147 = soc_th_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l147);return l147;}
static int l231(int l1){int l174,l147;alpm_pivot_t*l143;for(l174 = 0;l174<
MAX_PIVOT_COUNT;l174++){l143 = ALPM_TCAM_PIVOT(l1,l174);if(l143){if(
PIVOT_BUCKET_HANDLE(l143)){if(PIVOT_BUCKET_TRIE(l143)){l147 = trie_traverse(
PIVOT_BUCKET_TRIE(l143),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l147)){trie_destroy(PIVOT_BUCKET_TRIE(l143));}else{LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Unable to clear trie state for unit %d\n"),l1
));return l147;}}sal_free(PIVOT_BUCKET_HANDLE(l143));}sal_free(
ALPM_TCAM_PIVOT(l1,l174));ALPM_TCAM_PIVOT(l1,l174) = NULL;}}for(l174 = 0;l174
<= SOC_VRF_MAX(l1)+1;l174++){l147 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l1,
l174),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l147)){
trie_destroy(VRF_PREFIX_TRIE_IPV4(l1,l174));}else{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l1,l174));
return l147;}l147 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l1,l174),
alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l147)){
trie_destroy(VRF_PREFIX_TRIE_IPV6(l1,l174));}else{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l1,l174));
return l147;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l174)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l174));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l174
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l174));}sal_memset(&
alpm_vrf_handle[l1][l174],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l1][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,1,1);
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,2,1);if(SOC_ALPM_BUCKET_BMAP(l1)!= NULL){
sal_free(SOC_ALPM_BUCKET_BMAP(l1));}sal_memset(&soc_th_alpm_bucket[l1],0,
sizeof(soc_alpm_bucket_t));if(alpm_vrf_handle[l1])sal_free(alpm_vrf_handle[l1
]);if(tcam_pivot[l1])sal_free(tcam_pivot[l1]);alpm_vrf_handle[l1] = NULL;
tcam_pivot[l1] = NULL;return SOC_E_NONE;}int soc_th_alpm_deinit(int l1){l3(l1
);SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_deinit(l1));SOC_IF_ERROR_RETURN(
soc_th_alpm_128_state_clear(l1));SOC_IF_ERROR_RETURN(l231(l1));if(SOC_CONTROL
(l1)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l1)->alpm_bulk_retry);
SOC_CONTROL(l1)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_lookup_retry
);SOC_CONTROL(l1)->alpm_lookup_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_insert_retry
);SOC_CONTROL(l1)->alpm_insert_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_delete_retry
);SOC_CONTROL(l1)->alpm_delete_retry = NULL;}return SOC_E_NONE;}static int
l232(int l1,int l24,int l30){defip_entry_t*lpm_entry,l233;int l234;int index;
int l147 = SOC_E_NONE;uint32 key[2] = {0,0};uint32 l35;alpm_bucket_handle_t*
l202;alpm_pivot_t*pivot_pyld;payload_t*l226;trie_t*l235;trie_t*l236 = NULL;if
(l30 == 0){trie_init(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(l1,l24));l236 = 
VRF_PIVOT_TRIE_IPV4(l1,l24);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l1,l24));l236 = VRF_PIVOT_TRIE_IPV6(l1,l24);}if(l30 == 0)
{trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l1,l24));l235 = 
VRF_PREFIX_TRIE_IPV4(l1,l24);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6(l1,l24));l235 = VRF_PREFIX_TRIE_IPV6(l1,l24);}lpm_entry = 
sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n")));
return SOC_E_MEMORY;}l26(l1,key,0,l24,l30,lpm_entry,0,1);if(l30 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l24) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24) = lpm_entry;}if(l24 == SOC_VRF_MAX(l1)+1)
{soc_L3_DEFIPm_field32_set(l1,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l1,lpm_entry,DEFAULT_MISS0f,1);}l147 = 
soc_th_alpm_bucket_assign(l1,&l234,l30);soc_L3_DEFIPm_field32_set(l1,
lpm_entry,ALG_BKT_PTR0f,l234);sal_memcpy(&l233,lpm_entry,sizeof(l233));l147 = 
l4(l1,&l233,&index);l202 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l202 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_th_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")
));return SOC_E_NONE;}sal_memset(l202,0,sizeof(*l202));pivot_pyld = sal_alloc
(sizeof(alpm_pivot_t),"Payload for Pivot");if(pivot_pyld == NULL){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
sal_free(l202);return SOC_E_MEMORY;}l226 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l226 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_vrf_add: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l202);sal_free(pivot_pyld);return SOC_E_MEMORY
;}sal_memset(pivot_pyld,0,sizeof(*pivot_pyld));sal_memset(l226,0,sizeof(*l226
));l35 = 0;PIVOT_BUCKET_HANDLE(pivot_pyld) = l202;if(l30){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(pivot_pyld));}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(pivot_pyld));}PIVOT_BUCKET_INDEX(
pivot_pyld) = l234;PIVOT_BUCKET_VRF(pivot_pyld) = l24;PIVOT_BUCKET_IPV6(
pivot_pyld) = l30;PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;pivot_pyld->key[0] = 
l226->key[0] = key[0];pivot_pyld->key[1] = l226->key[1] = key[1];pivot_pyld->
len = l226->len = l35;l147 = trie_insert(l235,key,NULL,l35,&(l226->node));if(
SOC_FAILURE(l147)){sal_free(l226);sal_free(pivot_pyld);sal_free(l202);return
l147;}l147 = trie_insert(l236,key,NULL,l35,(trie_node_t*)pivot_pyld);if(
SOC_FAILURE(l147)){trie_node_t*l198 = NULL;(void)trie_delete(l235,key,l35,&
l198);sal_free(l226);sal_free(pivot_pyld);sal_free(l202);return l147;}index = 
soc_th_alpm_physical_idx(l1,L3_DEFIPm,index,l30);if(l30 == 0){ALPM_TCAM_PIVOT
(l1,index) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = index;}else{
ALPM_TCAM_PIVOT(l1,index<<1) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = 
index<<1;}VRF_PIVOT_REF_INC(l1,l24,l30);VRF_TRIE_INIT_DONE(l1,l24,l30,1);
return l147;}static int l29(int l1,int l24,int l30){defip_entry_t*lpm_entry;
int l234;int l139;int l147 = SOC_E_NONE;uint32 key[2] = {0,0},l136[
SOC_MAX_MEM_FIELD_WORDS];payload_t*l201;alpm_pivot_t*l237;trie_node_t*l198;
trie_t*l235;trie_t*l236 = NULL;if(l30 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l24);}else{lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24);}l234 = soc_L3_DEFIPm_field32_get(l1,
lpm_entry,ALG_BKT_PTR0f);l147 = soc_th_alpm_bucket_release(l1,l234,l30);
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,l234),
INVALIDm);if(SOC_TH_ALPM_V6_SCALE_CHECK(l1,l30)){
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,l234+1),
INVALIDm);}l147 = l15(l1,lpm_entry,(void*)l136,&l139);if(SOC_FAILURE(l147)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n"),
l24,l30));l139 = -1;}l139 = soc_th_alpm_physical_idx(l1,L3_DEFIPm,l139,l30);
if(l30 == 0){l237 = ALPM_TCAM_PIVOT(l1,l139);}else{l237 = ALPM_TCAM_PIVOT(l1,
l139<<1);}l147 = soc_th_alpm_lpm_delete(l1,lpm_entry);if(SOC_FAILURE(l147)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),l24,l30));}sal_free(lpm_entry);if(
l30 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l24) = NULL;l235 = 
VRF_PREFIX_TRIE_IPV4(l1,l24);VRF_PREFIX_TRIE_IPV4(l1,l24) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24) = NULL;l235 = VRF_PREFIX_TRIE_IPV6(l1,l24
);VRF_PREFIX_TRIE_IPV6(l1,l24) = NULL;}VRF_TRIE_INIT_DONE(l1,l24,l30,0);l147 = 
trie_delete(l235,key,0,&l198);l201 = (payload_t*)l198;if(SOC_FAILURE(l147)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal default for vrf "" %d/%d\n"),l24,l30));}sal_free(
l201);(void)trie_destroy(l235);if(l30 == 0){l236 = VRF_PIVOT_TRIE_IPV4(l1,l24
);VRF_PIVOT_TRIE_IPV4(l1,l24) = NULL;}else{l236 = VRF_PIVOT_TRIE_IPV6(l1,l24)
;VRF_PIVOT_TRIE_IPV6(l1,l24) = NULL;}l198 = NULL;l147 = trie_delete(l236,key,
0,&l198);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l24,l30));}(void)
trie_destroy(l236);(void)trie_destroy(PIVOT_BUCKET_TRIE(l237));sal_free(
PIVOT_BUCKET_HANDLE(l237));sal_free(l237);return l147;}int soc_th_alpm_insert
(int l1,void*l5,uint32 l21,int l238,int l239){defip_alpm_ipv4_entry_t l187,
l188;defip_alpm_ipv6_64_entry_t l189,l190;soc_mem_t l20;void*l200,*l219;int
l154,l24;int index;int l7;int l147 = SOC_E_NONE;uint32 l22;l7 = 
soc_mem_field32_get(l1,L3_DEFIPm,l5,MODE0f);l20 = (l7)?L3_DEFIP_ALPM_IPV6_64m
:L3_DEFIP_ALPM_IPV4m;l200 = ((l7)?((uint32*)&(l189)):((uint32*)&(l187)));l219
= ((l7)?((uint32*)&(l190)):((uint32*)&(l188)));SOC_IF_ERROR_RETURN(l17(l1,l5,
l200,l219,l20,l21,&l22));SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,l5,&
l154,&l24));if(((l154 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l154 == SOC_L3_VRF_GLOBAL)))){l147 = l4(l1,l5,&
index);if(SOC_SUCCESS(l147)){VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l7);
VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l7);}else if(l147 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID,l7);}return(l147);}else if(l24 == 0){if(
soc_th_alpm_mode_get(l1)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}}if(l154!= 
SOC_L3_VRF_GLOBAL){if(!soc_th_alpm_mode_get(l1)){if(VRF_TRIE_ROUTES_CNT(l1,
l24,l7) == 0){if(!l22){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
l154));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l24,l7)){
LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_insert:VRF %d is not "
"initialized\n"),l24));l147 = l232(l1,l24,l7);if(SOC_FAILURE(l147)){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_insert:VRF %d/%d trie init \n"
"failed\n"),l24,l7));return l147;}LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),l24,l7));}if(l239&
SOC_ALPM_LOOKUP_HIT){l147 = l155(l1,l5,l200,l219,l20,l238);}else{if(l238 == -
1){l238 = 0;}l147 = l211(l1,l5,l20,l200,l219,&index,
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l238),l239);}if(l147!= SOC_E_NONE){LOG_BSL_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l1,soc_errmsg(l147)
));}return(l147);}int soc_th_alpm_lookup(int l1,void*key_data,void*l12,int*
l13,int*l240){defip_alpm_ipv4_entry_t l187;defip_alpm_ipv6_64_entry_t l189;
soc_mem_t l20;int bucket_index = 0;int tcam_index;void*l200;int l154,l24;int
l7,l129;int l147 = SOC_E_NONE;l7 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,
MODE0f);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l154,&l24));
l147 = l11(l1,key_data,l12,l13,&l129,&l7);if(SOC_SUCCESS(l147)){if(!l7&&(*l13
&0x1)){l147 = soc_th_alpm_lpm_ip4entry1_to_0(l1,l12,l12,PRESERVE_HIT);}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,l12,&l154,&l24));if(l154 == 
SOC_L3_VRF_OVERRIDE){return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l24,
l7)){LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_lookup:VRF %d is not initialized\n"),l24));*l240 = 0;return
SOC_E_NOT_FOUND;}l20 = (l7)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l200 = 
((l7)?((uint32*)&(l189)):((uint32*)&(l187)));SOC_ALPM_LPM_LOCK(l1);l147 = 
l153(l1,key_data,l20,l200,&tcam_index,&bucket_index,l13);SOC_ALPM_LPM_UNLOCK(
l1);if(SOC_FAILURE(l147)){*l240 = tcam_index;*l13 = 
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,bucket_index);return l147;}l147 = l23(l1,l200
,l20,l7,l154,bucket_index,*l13,l12);*l240 = SOC_ALPM_LOOKUP_HIT|tcam_index;
return(l147);}int soc_th_alpm_delete(int l1,void*key_data,int l238,int l239){
int l154,l24;int l7;int l147 = SOC_E_NONE;l7 = soc_mem_field32_get(l1,
L3_DEFIPm,key_data,MODE0f);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,
key_data,&l154,&l24));if(((l154 == SOC_L3_VRF_OVERRIDE)||((
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)&&(l154 == 
SOC_L3_VRF_GLOBAL)))){l147 = soc_th_alpm_lpm_delete(l1,key_data);if(
SOC_SUCCESS(l147)){VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l7);VRF_TRIE_ROUTES_DEC(l1
,MAX_VRF_ID,l7);}return(l147);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l24,l7)){
LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_delete:VRF %d/%d is not initialized\n"),l24,l7));return SOC_E_NONE;
}if(l238 == -1){l238 = 0;}l147 = l220(l1,key_data,
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l238),l239&~SOC_ALPM_LOOKUP_HIT,l238);}return
(l147);}static int l16(int l1){int l241;l241 = soc_mem_index_count(l1,
L3_DEFIPm)+soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)*2;SOC_ALPM_BUCKET_COUNT
(l1) = SOC_TH_ALPM_MAX_BKTS;if(SOC_URPF_STATUS_GET(l1)){l241>>= 1;
SOC_ALPM_BUCKET_COUNT(l1)>>= 1;}SOC_ALPM_BUCKET_BMAP_SIZE(l1) = 
SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(l1));SOC_ALPM_BUCKET_BMAP(l1) = 
sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l1),"alpm_shared_bucket_bitmap");if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){LOG_BSL_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_shared_mem_init: Memory allocation for "
"bucket bitmap management failed\n")));return SOC_E_MEMORY;}sal_memset(
SOC_ALPM_BUCKET_BMAP(l1),0,SOC_ALPM_BUCKET_BMAP_SIZE(l1));
soc_th_alpm_bucket_assign(l1,&l241,1);return SOC_E_NONE;}int
soc_th_alpm_bucket_assign(int l1,int*l234,int l30){int l174,l242 = 1,l243 = 0
;if(l30||SOC_IS_TOMAHAWK(l1)){if(!soc_th_alpm_mode_get(l1)&&!
SOC_URPF_STATUS_GET(l1)){l242 = 2;}}for(l174 = 0;l174<SOC_ALPM_BUCKET_COUNT(
l1);l174+= l242){SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l174,l242,l243);
if(0 == l243){break;}}if(l174 == SOC_ALPM_BUCKET_COUNT(l1)){return SOC_E_FULL
;}SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l174,l242);*l234 = l174;
SOC_ALPM_BUCKET_NEXT_FREE(l1) = l174;return SOC_E_NONE;}int
soc_th_alpm_bucket_release(int l1,int l234,int l30){int l242 = 1,l243 = 0;if(
(l234<1)||(l234>SOC_ALPM_BUCKET_MAX_INDEX(l1))){return SOC_E_PARAM;}if(l30||
SOC_IS_TOMAHAWK(l1)){if(!soc_th_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){
l242 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l234,l242,l243);if(!
l243){return SOC_E_PARAM;}SHR_BITCLR_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l234,l242
);return SOC_E_NONE;}int l244(int l1,int l245,int l7,int*l243){int l242 = 1;
if((l245<1)||(l245>SOC_ALPM_BUCKET_MAX_INDEX(l1))){return SOC_E_PARAM;}if(l7
||SOC_IS_TOMAHAWK(l1)){if(!soc_th_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)
){l242 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l245,l242,*l243);
return SOC_E_NONE;}static void l117(int l1,void*l12,int index,l112 l118){if(
index&(0x8000)){l118[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l12),(l105[(l1)]->l71));l118[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l73));l118[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l72));l118[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l74));if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int
l246;(void)soc_th_alpm_lpm_vrf_get(l1,l12,(int*)&l118[4],&l246);}else{l118[4]
= 0;};}else{if(index&0x1){l118[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(l1)]->l72));l118[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l74));l118[2] = 0;l118[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l105[(l1)]->l88)!= NULL))){int l246;defip_entry_t l247;(void)
soc_th_alpm_lpm_ip4entry1_to_0(l1,l12,&l247,0);(void)soc_th_alpm_lpm_vrf_get(
l1,&l247,(int*)&l118[4],&l246);}else{l118[4] = 0;};}else{l118[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l71));l118[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l73));l118[2] = 0;l118[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int l246;(void)
soc_th_alpm_lpm_vrf_get(l1,l12,(int*)&l118[4],&l246);}else{l118[4] = 0;};}}}
static int l248(l112 l114,l112 l115){int l139;for(l139 = 0;l139<5;l139++){{if
((l114[l139])<(l115[l139])){return-1;}if((l114[l139])>(l115[l139])){return 1;
}};}return(0);}static void l249(int l1,void*l5,uint32 l250,uint32 l132,int
l129){l112 l251;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l75))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(l1)]->l86))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l85))){l251[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l71));l251[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l73));l251[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l72));l251[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l74));if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int
l246;(void)soc_th_alpm_lpm_vrf_get(l1,l5,(int*)&l251[4],&l246);}else{l251[4] = 
0;};l131((l116[(l1)]),l248,l251,l129,l132,((uint16)l250<<1)|(0x8000));}}else{
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105
[(l1)]->l85))){l251[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l71));l251[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l73));l251[2] = 0;l251[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l105[(l1)]->l87)!= NULL))){int l246;(void)soc_th_alpm_lpm_vrf_get(l1,l5,(int
*)&l251[4],&l246);}else{l251[4] = 0;};l131((l116[(l1)]),l248,l251,l129,l132,(
(uint16)l250<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l86))){l251[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l72));l251[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l74));l251[2] = 0;l251[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l88)!= NULL))){int l246;defip_entry_t
l247;(void)soc_th_alpm_lpm_ip4entry1_to_0(l1,l5,&l247,0);(void)
soc_th_alpm_lpm_vrf_get(l1,&l247,(int*)&l251[4],&l246);}else{l251[4] = 0;};
l131((l116[(l1)]),l248,l251,l129,l132,(((uint16)l250<<1)+1));}}}static void
l252(int l1,void*key_data,uint32 l250){l112 l251;int l129 = -1;int l147;
uint16 index;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm
)),(key_data),(l105[(l1)]->l75))){l251[0] = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l71));l251[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l73));l251[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l72));l251[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l74));if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL)))
{int l246;(void)soc_th_alpm_lpm_vrf_get(l1,key_data,(int*)&l251[4],&l246);}
else{l251[4] = 0;};index = (l250<<1)|(0x8000);}else{l251[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l71));l251[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l73));l251[2] = 0;l251[3] = 0x80000001
;if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int l246;(void)
soc_th_alpm_lpm_vrf_get(l1,key_data,(int*)&l251[4],&l246);}else{l251[4] = 0;}
;index = l250;}l147 = l133((l116[(l1)]),l248,l251,l129,index);if(SOC_FAILURE(
l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\ndel  index: H %d error %d\n"),index,l147));}}static int l253(int l1,void*
key_data,int l129,int*l130){l112 l251;int l254;int l147;uint16 index = (
0xFFFF);l254 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)
),(key_data),(l105[(l1)]->l75));if(l254){l251[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l71));l251[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l73));l251[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l72));l251[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l74));if((!(SOC_IS_HURRICANE(l1)))&&((
(l105[(l1)]->l87)!= NULL))){int l246;(void)soc_th_alpm_lpm_vrf_get(l1,
key_data,(int*)&l251[4],&l246);}else{l251[4] = 0;};}else{l251[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l71));l251[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l73));l251[2] = 0;l251[3] = 0x80000001
;if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int l246;(void)
soc_th_alpm_lpm_vrf_get(l1,key_data,(int*)&l251[4],&l246);}else{l251[4] = 0;}
;}l147 = l126((l116[(l1)]),l248,l251,l129,&index);if(SOC_FAILURE(l147)){*l130
= 0xFFFFFFFF;return(l147);}*l130 = index;return(SOC_E_NONE);}static uint16
l119(uint8*l120,int l121){return(_shr_crc16b(0,l120,l121));}static int l122(
int unit,int l107,int l108,l111**l123){l111*l127;int index;if(l108>l107){
return SOC_E_MEMORY;}l127 = sal_alloc(sizeof(l111),"lpm_hash");if(l127 == 
NULL){return SOC_E_MEMORY;}sal_memset(l127,0,sizeof(*l127));l127->unit = unit
;l127->l107 = l107;l127->l108 = l108;l127->l109 = sal_alloc(l127->l108*sizeof
(*(l127->l109)),"hash_table");if(l127->l109 == NULL){sal_free(l127);return
SOC_E_MEMORY;}l127->l110 = sal_alloc(l127->l107*sizeof(*(l127->l110)),
"link_table");if(l127->l110 == NULL){sal_free(l127->l109);sal_free(l127);
return SOC_E_MEMORY;}for(index = 0;index<l127->l108;index++){l127->l109[index
] = (0xFFFF);}for(index = 0;index<l127->l107;index++){l127->l110[index] = (
0xFFFF);}*l123 = l127;return SOC_E_NONE;}static int l124(l111*l125){if(l125!= 
NULL){sal_free(l125->l109);sal_free(l125->l110);sal_free(l125);}return
SOC_E_NONE;}static int l126(l111*l127,l113 l128,l112 entry,int l129,uint16*
l130){int l1 = l127->unit;uint16 l255;uint16 index;l255 = l119((uint8*)entry,
(32*5))%l127->l108;index = l127->l109[l255];;;while(index!= (0xFFFF)){uint32
l12[SOC_MAX_MEM_FIELD_WORDS];l112 l118;int l256;l256 = (index&(0x7FFF))>>1;
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l256,l12));l117(l1,l12,
index,l118);if((*l128)(entry,l118) == 0){*l130 = (index&(0x7FFF))>>((index&(
0x8000))?1:0);;return(SOC_E_NONE);}index = l127->l110[index&(0x7FFF)];;};
return(SOC_E_NOT_FOUND);}static int l131(l111*l127,l113 l128,l112 entry,int
l129,uint16 l132,uint16 l44){int l1 = l127->unit;uint16 l255;uint16 index;
uint16 l257;l255 = l119((uint8*)entry,(32*5))%l127->l108;index = l127->l109[
l255];;;;l257 = (0xFFFF);if(l132!= (0xFFFF)){while(index!= (0xFFFF)){uint32
l12[SOC_MAX_MEM_FIELD_WORDS];l112 l118;int l256;l256 = (index&(0x7FFF))>>1;
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l256,l12));l117(l1,l12,
index,l118);if((*l128)(entry,l118) == 0){if(l44!= index){;if(l257 == (0xFFFF)
){l127->l109[l255] = l44;l127->l110[l44&(0x7FFF)] = l127->l110[index&(0x7FFF)
];l127->l110[index&(0x7FFF)] = (0xFFFF);}else{l127->l110[l257&(0x7FFF)] = l44
;l127->l110[l44&(0x7FFF)] = l127->l110[index&(0x7FFF)];l127->l110[index&(
0x7FFF)] = (0xFFFF);}};return(SOC_E_NONE);}l257 = index;index = l127->l110[
index&(0x7FFF)];;}}l127->l110[l44&(0x7FFF)] = l127->l109[l255];l127->l109[
l255] = l44;return(SOC_E_NONE);}static int l133(l111*l127,l113 l128,l112 entry
,int l129,uint16 l134){uint16 l255;uint16 index;uint16 l257;l255 = l119((
uint8*)entry,(32*5))%l127->l108;index = l127->l109[l255];;;l257 = (0xFFFF);
while(index!= (0xFFFF)){if(l134 == index){;if(l257 == (0xFFFF)){l127->l109[
l255] = l127->l110[l134&(0x7FFF)];l127->l110[l134&(0x7FFF)] = (0xFFFF);}else{
l127->l110[l257&(0x7FFF)] = l127->l110[l134&(0x7FFF)];l127->l110[l134&(0x7FFF
)] = (0xFFFF);}return(SOC_E_NONE);}l257 = index;index = l127->l110[index&(
0x7FFF)];;}return(SOC_E_NOT_FOUND);}int soc_th_alpm_lpm_ip4entry0_to_0(int l1
,void*l258,void*l259,int l260){uint32 l261;l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l85),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l75));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l75),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l71));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l71),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l73),(l261));if(((l105[(l1)]->l61)!= NULL)){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l61),(l261));}if(((l105[(l1)]->l63)!= NULL)){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l63));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l63),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l65));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l65),(l261));}else{l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l79),(l261));}l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l81));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l81),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l83));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l83),(l261));if(((l105[(l1)]->l87)!= NULL)){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l87),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l89));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l89),(l261));}if(((l105[(l1)]->l59)!= NULL)){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l59),(l261));}if(l260){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l69));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l69),(l261));}l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l91),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l93));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l93),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l95),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l97));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l97),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l99));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l99),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l101));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l259),(l105[(l1)]->l101),(l261));return(SOC_E_NONE);}int
soc_th_alpm_lpm_ip4entry1_to_1(int l1,void*l258,void*l259,int l260){uint32
l261;l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l258),(l105[(l1)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l259),(l105[(l1)]->l86),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l76),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l72));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l72),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l74),(l261));if(((l105[(l1)]
->l62)!= NULL)){l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l258),(l105[(l1)]->l62));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l62),(l261));}if(((l105[(l1)]
->l64)!= NULL)){l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l258),(l105[(l1)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l64),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l66),(l261));}else{l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l80));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l80),(l261));}l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l82),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l84),(l261));if(((l105[(l1)]
->l88)!= NULL)){l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l258),(l105[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l88),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l90),(l261));}if(((l105[(l1)]->l60)!= NULL)){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l60),(l261));}if(l260){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l70),(l261));}l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l92),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l94));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l94),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l96),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l98));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l98),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l100));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l100),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l102));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l259),(l105[(l1)]->l102),(l261));return(SOC_E_NONE);}int
soc_th_alpm_lpm_ip4entry0_to_1(int l1,void*l258,void*l259,int l260){uint32
l261;l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l258),(l105[(l1)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l259),(l105[(l1)]->l86),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l76),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l71));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l72),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l74),(l261));if(!
SOC_IS_HURRICANE(l1)){l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l61));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l62),(l261));}if(((l105[(l1)
]->l63)!= NULL)){l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l258),(l105[(l1)]->l63));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l64),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l65));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l66),(l261));}else{l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l79));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l80),(l261));}l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l82),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l83));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l84),(l261));if(((l105[(l1)]
->l87)!= NULL)){l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l258),(l105[(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l88),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l90),(l261));}if(((l105[(l1)]->l59)!= NULL)){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l60),(l261));}if(l260){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l69));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l70),(l261));}l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l92),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l93));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l94),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l96),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l97));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l98),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l99));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l100),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l101));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l259),(l105[(l1)]->l102),(l261));return(SOC_E_NONE);}int
soc_th_alpm_lpm_ip4entry1_to_0(int l1,void*l258,void*l259,int l260){uint32
l261;l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l258),(l105[(l1)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l259),(l105[(l1)]->l85),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l75),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l72));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l71),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l73),(l261));if(!
SOC_IS_HURRICANE(l1)){l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l62));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l61),(l261));}if(((l105[(l1)
]->l64)!= NULL)){l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l258),(l105[(l1)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l63),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l65),(l261));}else{l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l80));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l79),(l261));}l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l81),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l83),(l261));if(((l105[(l1)]
->l88)!= NULL)){l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l258),(l105[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l87),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l89),(l261));}if(((l105[(l1)]->l60)!= NULL)){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l59),(l261));}if(l260){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l69),(l261));}l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l91),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l94));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l93),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l259),(l105[(l1)]->l95),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[(l1)]->l98));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[
(l1)]->l97),(l261));l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l258),(l105[(l1)]->l100));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l259),(l105[(l1)]->l99),(l261));l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l258),(l105[
(l1)]->l102));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l259),(l105[(l1)]->l101),(l261));return(SOC_E_NONE);}static int l262(int l1
,void*l12){return(SOC_E_NONE);}void soc_th_alpm_lpm_state_dump(int l1){int
l174;int l263;l263 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,
bslSourceAlpm,bslSeverityVerbose,l1)){return;}for(l174 = l263;l174>= 0;l174--
){if((l174!= ((3*(64+32+2+1))-1))&&((l55[(l1)][(l174)].l48) == -1)){continue;
}LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l174,(l55
[(l1)][(l174)].l50),(l55[(l1)][(l174)].next),(l55[(l1)][(l174)].l48),(l55[(l1
)][(l174)].l49),(l55[(l1)][(l174)].l51),(l55[(l1)][(l174)].l52)));}
COMPILER_REFERENCE(l262);}static int l264(int l1,int index,uint32*l12){int
l265;int l7;uint32 l266;uint32 l267;int l268;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l265 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l1)){l265 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1)+0x0400;}else{l265 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}l7 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(l1)]->l75));if(((l105[(l1)]->l59)
!= NULL)){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l12),(l105[(l1)]->l59),(0));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l12),(l105[(l1)]->l60),(0));}l266 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l73));l267 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l74));if(!l7){if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l12),(l105[(l1)]->l83),((!l266)?1:0));}if(soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(l1)]->l86))){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l84),((!l267)?1:0));}}else{l268 = ((!l266)&&(!l267))?1:0;l266 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85));l267 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l86));if(l266&&l267){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l83),(l268));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l84),(l268));}}return l168(l1,MEM_BLOCK_ANY,
index+l265,index,l12);}static int l269(int l1,int l270,int l271){uint32 l12[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,
l270,l12));l249(l1,l12,l271,0x4000,0);SOC_IF_ERROR_RETURN(l168(l1,
MEM_BLOCK_ANY,l271,l270,l12));SOC_IF_ERROR_RETURN(l264(l1,l271,l12));do{int
l272,l273;l272 = soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l270),1);l273 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l271),1);ALPM_TCAM_PIVOT(l1,l273<<1)
= ALPM_TCAM_PIVOT(l1,l272<<1);ALPM_TCAM_PIVOT(l1,(l273<<1)+1) = 
ALPM_TCAM_PIVOT(l1,(l272<<1)+1);if(ALPM_TCAM_PIVOT((l1),l273<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l273<<1)) = l273<<1;}if(ALPM_TCAM_PIVOT
((l1),(l273<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(l273<<1)+1)) = (
l273<<1)+1;}ALPM_TCAM_PIVOT(l1,l272<<1) = NULL;ALPM_TCAM_PIVOT(l1,(l272<<1)+1
) = NULL;}while(0);return(SOC_E_NONE);}static int l274(int l1,int l129,int l7
){uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l270;int l271;uint32 l275,l276;l271
= (l55[(l1)][(l129)].l49)+1;if(!l7){l270 = (l55[(l1)][(l129)].l49);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l270,l12));l275 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85));l276 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l86));if((l275 == 0)||(l276 == 0)){l249(l1,l12
,l271,0x4000,0);SOC_IF_ERROR_RETURN(l168(l1,MEM_BLOCK_ANY,l271,l270,l12));
SOC_IF_ERROR_RETURN(l264(l1,l271,l12));do{int l277 = soc_th_alpm_physical_idx
((l1),L3_DEFIPm,(l270),1)<<1;int l208 = soc_th_alpm_physical_idx((l1),
L3_DEFIPm,(l271),1)*2+(l277&1);if((l276)){l277++;}ALPM_TCAM_PIVOT((l1),l208) = 
ALPM_TCAM_PIVOT((l1),l277);if(ALPM_TCAM_PIVOT((l1),l208)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l1),l208)) = l208;}ALPM_TCAM_PIVOT((l1),l277) = NULL;}while(
0);l271--;}}l270 = (l55[(l1)][(l129)].l48);if(l270!= l271){
SOC_IF_ERROR_RETURN(l269(l1,l270,l271));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l7)
;}(l55[(l1)][(l129)].l48)+= 1;(l55[(l1)][(l129)].l49)+= 1;return(SOC_E_NONE);
}static int l278(int l1,int l129,int l7){uint32 l12[SOC_MAX_MEM_FIELD_WORDS];
int l270;int l271;int l279;uint32 l275,l276;l271 = (l55[(l1)][(l129)].l48)-1;
if((l55[(l1)][(l129)].l51) == 0){(l55[(l1)][(l129)].l48) = l271;(l55[(l1)][(
l129)].l49) = l271-1;return(SOC_E_NONE);}if((!l7)&&((l55[(l1)][(l129)].l49)!= 
(l55[(l1)][(l129)].l48))){l270 = (l55[(l1)][(l129)].l49);SOC_IF_ERROR_RETURN(
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l270,l12));l275 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85));l276 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l86));if((l275 == 0)||(l276 == 0)){l279 = l270
-1;SOC_IF_ERROR_RETURN(l269(l1,l279,l271));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,
l7);l249(l1,l12,l279,0x4000,0);SOC_IF_ERROR_RETURN(l168(l1,MEM_BLOCK_ANY,l279
,l270,l12));SOC_IF_ERROR_RETURN(l264(l1,l279,l12));do{int l277 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l270),1)<<1;int l208 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l279),1)*2+(l277&1);if((l276)){l277
++;}ALPM_TCAM_PIVOT((l1),l208) = ALPM_TCAM_PIVOT((l1),l277);if(
ALPM_TCAM_PIVOT((l1),l208)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l208)) = 
l208;}ALPM_TCAM_PIVOT((l1),l277) = NULL;}while(0);}else{l249(l1,l12,l271,
0x4000,0);SOC_IF_ERROR_RETURN(l168(l1,MEM_BLOCK_ANY,l271,l270,l12));
SOC_IF_ERROR_RETURN(l264(l1,l271,l12));do{int l272,l273;l272 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l270),1);l273 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l271),1);ALPM_TCAM_PIVOT(l1,l273<<1)
= ALPM_TCAM_PIVOT(l1,l272<<1);ALPM_TCAM_PIVOT(l1,(l273<<1)+1) = 
ALPM_TCAM_PIVOT(l1,(l272<<1)+1);if(ALPM_TCAM_PIVOT((l1),l273<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l273<<1)) = l273<<1;}if(ALPM_TCAM_PIVOT
((l1),(l273<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(l273<<1)+1)) = (
l273<<1)+1;}ALPM_TCAM_PIVOT(l1,l272<<1) = NULL;ALPM_TCAM_PIVOT(l1,(l272<<1)+1
) = NULL;}while(0);}}else{l270 = (l55[(l1)][(l129)].l49);SOC_IF_ERROR_RETURN(
l269(l1,l270,l271));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l7);}(l55[(l1)][(l129)]
.l48)-= 1;(l55[(l1)][(l129)].l49)-= 1;return(SOC_E_NONE);}static int l280(int
l1,int l129,int l7,void*l12,int*l281){int l282;int l283;int l284;int l285;int
l270;uint32 l275,l276;int l147;if((l55[(l1)][(l129)].l51) == 0){l285 = ((3*(
64+32+2+1))-1);if((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL)||(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)){if(l129<= (((3*(64+32+2
+1))/3)-1)){l285 = (((3*(64+32+2+1))/3)-1);}}while((l55[(l1)][(l285)].next)>
l129){l285 = (l55[(l1)][(l285)].next);}l283 = (l55[(l1)][(l285)].next);if(
l283!= -1){(l55[(l1)][(l283)].l50) = l129;}(l55[(l1)][(l129)].next) = (l55[(
l1)][(l285)].next);(l55[(l1)][(l129)].l50) = l285;(l55[(l1)][(l285)].next) = 
l129;(l55[(l1)][(l129)].l52) = ((l55[(l1)][(l285)].l52)+1)/2;(l55[(l1)][(l285
)].l52)-= (l55[(l1)][(l129)].l52);(l55[(l1)][(l129)].l48) = (l55[(l1)][(l285)
].l49)+(l55[(l1)][(l285)].l52)+1;(l55[(l1)][(l129)].l49) = (l55[(l1)][(l129)]
.l48)-1;(l55[(l1)][(l129)].l51) = 0;}else if(!l7){l270 = (l55[(l1)][(l129)].
l48);if((l147 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l270,l12))<0){return l147;}
l275 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),
(l105[(l1)]->l85));l276 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l12),(l105[(l1)]->l86));if((l275 == 0)||(l276 == 0)){*l281 = (
l270<<1)+((l276 == 0)?1:0);return(SOC_E_NONE);}l270 = (l55[(l1)][(l129)].l49)
;if((l147 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l270,l12))<0){return l147;}l275 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85));l276 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l86));if((l275 == 0)||(l276 == 0)){*l281 = (
l270<<1)+((l276 == 0)?1:0);return(SOC_E_NONE);}}l284 = l129;while((l55[(l1)][
(l284)].l52) == 0){l284 = (l55[(l1)][(l284)].next);if(l284 == -1){l284 = l129
;break;}}while((l55[(l1)][(l284)].l52) == 0){l284 = (l55[(l1)][(l284)].l50);
if(l284 == -1){if((l55[(l1)][(l129)].l51) == 0){l282 = (l55[(l1)][(l129)].l50
);l283 = (l55[(l1)][(l129)].next);if(-1!= l282){(l55[(l1)][(l282)].next) = 
l283;}if(-1!= l283){(l55[(l1)][(l283)].l50) = l282;}}return(SOC_E_FULL);}}
while(l284>l129){l283 = (l55[(l1)][(l284)].next);SOC_IF_ERROR_RETURN(l278(l1,
l283,l7));(l55[(l1)][(l284)].l52)-= 1;(l55[(l1)][(l283)].l52)+= 1;l284 = l283
;}while(l284<l129){SOC_IF_ERROR_RETURN(l274(l1,l284,l7));(l55[(l1)][(l284)].
l52)-= 1;l282 = (l55[(l1)][(l284)].l50);(l55[(l1)][(l282)].l52)+= 1;l284 = 
l282;}(l55[(l1)][(l129)].l51)+= 1;(l55[(l1)][(l129)].l52)-= 1;(l55[(l1)][(
l129)].l49)+= 1;*l281 = (l55[(l1)][(l129)].l49)<<((l7)?0:1);sal_memcpy(l12,
soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);return(
SOC_E_NONE);}static int l286(int l1,int l129,int l7,void*l12,int l287){int
l282;int l283;int l270;int l271;uint32 l288[SOC_MAX_MEM_FIELD_WORDS];uint32
l289[SOC_MAX_MEM_FIELD_WORDS];uint32 l290[SOC_MAX_MEM_FIELD_WORDS];void*l291;
int l147;int l292,l37;l270 = (l55[(l1)][(l129)].l49);l271 = l287;if(!l7){l271
>>= 1;if((l147 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l270,l288))<0){return l147;}
if((l147 = READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,soc_th_alpm_physical_idx
(l1,L3_DEFIPm,l270,1),l289))<0){return l147;}if((l147 = 
READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,soc_th_alpm_physical_idx(l1,
L3_DEFIPm,l271,1),l290))<0){return l147;}l291 = (l271 == l270)?l288:l12;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l288),(l105[
(l1)]->l86))){l37 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l289,
BPM_LENGTH1f);if(l287&1){l147 = soc_th_alpm_lpm_ip4entry1_to_1(l1,l288,l291,
PRESERVE_HIT);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l290,BPM_LENGTH1f,
l37);}else{l147 = soc_th_alpm_lpm_ip4entry1_to_0(l1,l288,l291,PRESERVE_HIT);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l290,BPM_LENGTH0f,l37);}l292 = (
l270<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l288),(l105[(l1)]->l86),(0));}else{l37 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l289,BPM_LENGTH0f);if(l287&1){l147 = 
soc_th_alpm_lpm_ip4entry0_to_1(l1,l288,l291,PRESERVE_HIT);soc_mem_field32_set
(l1,L3_DEFIP_AUX_TABLEm,l290,BPM_LENGTH1f,l37);}else{l147 = 
soc_th_alpm_lpm_ip4entry0_to_0(l1,l288,l291,PRESERVE_HIT);soc_mem_field32_set
(l1,L3_DEFIP_AUX_TABLEm,l290,BPM_LENGTH0f,l37);}l292 = l270<<1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l288),(l105[
(l1)]->l85),(0));(l55[(l1)][(l129)].l51)-= 1;(l55[(l1)][(l129)].l52)+= 1;(l55
[(l1)][(l129)].l49)-= 1;}l292 = soc_th_alpm_physical_idx(l1,L3_DEFIPm,l292,0)
;l287 = soc_th_alpm_physical_idx(l1,L3_DEFIPm,l287,0);ALPM_TCAM_PIVOT(l1,l287
) = ALPM_TCAM_PIVOT(l1,l292);if(ALPM_TCAM_PIVOT(l1,l287)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l1,l287)) = l287;}ALPM_TCAM_PIVOT(l1,l292) = NULL;if((l147 = 
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ALL,soc_th_alpm_physical_idx(l1,
L3_DEFIPm,l271,1),l290))<0){return l147;}if(l271!= l270){l249(l1,l291,l271,
0x4000,0);if((l147 = l168(l1,MEM_BLOCK_ANY,l271,l271,l291))<0){return l147;}
if((l147 = l264(l1,l271,l291))<0){return l147;}}l249(l1,l288,l270,0x4000,0);
if((l147 = l168(l1,MEM_BLOCK_ANY,l270,l270,l288))<0){return l147;}if((l147 = 
l264(l1,l270,l288))<0){return l147;}}else{(l55[(l1)][(l129)].l51)-= 1;(l55[(
l1)][(l129)].l52)+= 1;(l55[(l1)][(l129)].l49)-= 1;if(l271!= l270){if((l147 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l270,l288))<0){return l147;}l249(l1,l288,l271
,0x4000,0);if((l147 = l168(l1,MEM_BLOCK_ANY,l271,l270,l288))<0){return l147;}
if((l147 = l264(l1,l271,l288))<0){return l147;}}l287 = 
soc_th_alpm_physical_idx(l1,L3_DEFIPm,l271,1);l292 = soc_th_alpm_physical_idx
(l1,L3_DEFIPm,l270,1);ALPM_TCAM_PIVOT(l1,l287<<1) = ALPM_TCAM_PIVOT(l1,l292<<
1);ALPM_TCAM_PIVOT(l1,l292<<1) = NULL;if(ALPM_TCAM_PIVOT(l1,l287<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,l287<<1)) = l287<<1;}sal_memcpy(l288,
soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);l249(l1
,l288,l270,0x4000,0);if((l147 = l168(l1,MEM_BLOCK_ANY,l270,l270,l288))<0){
return l147;}if((l147 = l264(l1,l270,l288))<0){return l147;}}if((l55[(l1)][(
l129)].l51) == 0){l282 = (l55[(l1)][(l129)].l50);assert(l282!= -1);l283 = (
l55[(l1)][(l129)].next);(l55[(l1)][(l282)].next) = l283;(l55[(l1)][(l282)].
l52)+= (l55[(l1)][(l129)].l52);(l55[(l1)][(l129)].l52) = 0;if(l283!= -1){(l55
[(l1)][(l283)].l50) = l282;}(l55[(l1)][(l129)].next) = -1;(l55[(l1)][(l129)].
l50) = -1;(l55[(l1)][(l129)].l48) = -1;(l55[(l1)][(l129)].l49) = -1;}return(
l147);}int soc_th_alpm_lpm_vrf_get(int unit,void*lpm_entry,int*l24,int*l38){
int l154;if(((l105[(unit)]->l89)!= NULL)){l154 = soc_L3_DEFIPm_field32_get(
unit,lpm_entry,VRF_ID_0f);*l38 = l154;if(soc_L3_DEFIPm_field32_get(unit,
lpm_entry,VRF_ID_MASK0f)){*l24 = l154;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l105[(unit)]->l91))){*l24 = SOC_L3_VRF_GLOBAL;*l38 = SOC_VRF_MAX(unit)+1;}
else{*l24 = SOC_L3_VRF_OVERRIDE;}}else{*l24 = SOC_L3_VRF_DEFAULT;}return(
SOC_E_NONE);}static int l293(int l1,void*entry,int*l14){int l129;int l147;int
l7;uint32 l261;int l154;int l294;l7 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l105[(l1)]->l75));if(l7){l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l105
[(l1)]->l73));if((l147 = _ipmask2pfx(l261,&l129))<0){return(l147);}l261 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l105
[(l1)]->l74));if(l129){if(l261!= 0xffffffff){return(SOC_E_PARAM);}l129+= 32;}
else{if((l147 = _ipmask2pfx(l261,&l129))<0){return(l147);}}l129+= 33;}else{
l261 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry
),(l105[(l1)]->l73));if((l147 = _ipmask2pfx(l261,&l129))<0){return(l147);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,entry,&l154,&l147));l294 = 
soc_th_alpm_mode_get(l1);switch(l154){case SOC_L3_VRF_GLOBAL:if((l294 == 
SOC_ALPM_MODE_PARALLEL)||(l294 == SOC_ALPM_MODE_TCAM_ALPM)){*l14 = l129+((3*(
64+32+2+1))/3);}else{*l14 = l129;}break;case SOC_L3_VRF_OVERRIDE:*l14 = l129+
2*((3*(64+32+2+1))/3);break;default:if((l294 == SOC_ALPM_MODE_PARALLEL)||(
l294 == SOC_ALPM_MODE_TCAM_ALPM)){*l14 = l129;}else{*l14 = l129+((3*(64+32+2+
1))/3);}break;}return(SOC_E_NONE);}static int l11(int l1,void*key_data,void*
l12,int*l13,int*l14,int*l7){int l147;int l30;int l130;int l129 = 0;l30 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l75));if(l30){if(!(l30 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l76)))){return(
SOC_E_PARAM);}}*l7 = l30;l293(l1,key_data,&l129);*l14 = l129;if(l253(l1,
key_data,l129,&l130) == SOC_E_NONE){*l13 = l130;if((l147 = READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,(*l7)?*l13:(*l13>>1),l12))<0){return l147;}return(SOC_E_NONE);}
else{return(SOC_E_NOT_FOUND);}}static int l2(int l1){int l263;int l174;int
l241;int l295;uint32 l137,l294;if(!soc_feature(l1,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}if((l294 = soc_property_get(l1,spn_L3_ALPM_ENABLE,0)))
{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l1,&l137));soc_reg_field_set(
l1,L3_DEFIP_RPF_CONTROLr,&l137,LPM_MODEf,1);if(l294 == 1){soc_reg_field_set(
l1,L3_DEFIP_RPF_CONTROLr,&l137,LOOKUP_MODEf,1);}else if(l294 == 2){
soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l137,LOOKUP_MODEf,0);
soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l137,
ENABLE_DOUBLE_WIDE_BUCKETS_FOR_IPV4f,1);}else if(l294 == 3){soc_reg_field_set
(l1,L3_DEFIP_RPF_CONTROLr,&l137,LOOKUP_MODEf,2);}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_RPF_CONTROLr(l1,l137));l137 = 0;if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l137,URPF_LOOKUP_CAM4f,0x1);
soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l137,URPF_LOOKUP_CAM5f,0x1);
soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l137,URPF_LOOKUP_CAM6f,0x1);
soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l137,URPF_LOOKUP_CAM7f,0x1);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l1,l137));l137 = 0;if(l294 == 1){
if(SOC_URPF_STATUS_GET(l1)){soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l137,
TCAM2_SELf,1);soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l137,TCAM3_SELf,1);
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l137,TCAM4_SELf,2);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l137,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l137,TCAM6_SELf,3);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l137,TCAM7_SELf,3);}else{soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l137,TCAM4_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l137,TCAM5_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l137,TCAM6_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l137,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l137,TCAM4_SELf,2);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l137,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l137,TCAM6_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l137,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l1,l137));if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l296 = 0;if(l294!= 1){uint32 l297;l297 = 
soc_property_get(l1,spn_NUM_IPV6_LPM_128B_ENTRIES,2048);if(l297!= 2048){if(
SOC_URPF_STATUS_GET(l1)){LOG_CLI((BSL_META_U(l1,
"URPF supported in combined mode only""with 2048 v6-128 entries\n")));return
SOC_E_PARAM;}if((l297!= 1024)&&(l297!= 3072)){LOG_CLI((BSL_META_U(l1,
"Only supported values for v6-128 in"
"nonURPF combined mode are 1024 and 3072\n")));return SOC_E_PARAM;}}}
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_KEY_SELr(l1,&l296));soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l296,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l296,V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l296,V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l296,V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_KEY_SELr(l1,l296));}}l263 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(
l1);l295 = sizeof(l53)*(l263);if((l55[(l1)]!= NULL)){if(soc_th_alpm_deinit(l1
)<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}}l105[l1] = sal_alloc(
sizeof(l103),"lpm_field_state");if(NULL == l105[l1]){SOC_ALPM_LPM_UNLOCK(l1);
return(SOC_E_MEMORY);}(l105[l1])->l57 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,
CLASS_ID0f);(l105[l1])->l58 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,CLASS_ID1f);
(l105[l1])->l59 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD0f);(l105[l1]
)->l60 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD1f);(l105[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP0f);(l105[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP1f);(l105[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT0f);(l105[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT1f);(l105[l1])->l65 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR0f);(l105[l1])->l66 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR1f);(l105[l1])->l67 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE0f);(l105[l1])->l68 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE1f);(l105[l1])->l69 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,HIT0f);(l105[l1])->l70 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,HIT1f);(l105[l1])->l71 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR0f);(l105[l1])->l72 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR1f);(l105[l1])->l73 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK0f);(l105[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK1f);(l105[l1])->l75 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE0f);(l105[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE1f);(l105[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK0f);(l105[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK1f);(l105[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX0f);(l105[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX1f);(l105[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI0f);(l105[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI1f);(l105[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE0f);(l105[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE1f);(l105[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID0f);(l105[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID1f);(l105[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_0f);(l105[l1])->l88 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_1f);(l105[l1])->l89 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK0f);(l105[l1])->l90 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK1f);(l105[l1])->l91 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH0f);(l105[l1])->l92 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH1f);(l105[l1])->l93 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX0f);(l105[l1])->l94 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX1f);(l105[l1])->l95 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR0f);(l105[l1])->l96 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR1f);(l105[l1])->l97 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS0f);(l105[l1])->l98 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS1f);(l105[l1])->l99 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l105[l1])->
l100 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l105[
l1])->l101 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l105
[l1])->l102 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(l55
[(l1)]) = sal_alloc(l295,"LPM prefix info");if(NULL == (l55[(l1)])){sal_free(
l105[l1]);l105[l1] = NULL;SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}
sal_memset((l55[(l1)]),0,l295);for(l174 = 0;l174<l263;l174++){(l55[(l1)][(
l174)].l48) = -1;(l55[(l1)][(l174)].l49) = -1;(l55[(l1)][(l174)].l50) = -1;(
l55[(l1)][(l174)].next) = -1;(l55[(l1)][(l174)].l51) = 0;(l55[(l1)][(l174)].
l52) = 0;}l241 = soc_mem_index_count(l1,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l1)
){l241>>= 1;}if((l294 == 1)||(l294 == 3)){(l55[(l1)][(((3*(64+32+2+1))-1))].
l49) = (l241>>1)-1;(l55[(l1)][(((((3*(64+32+2+1))/3)-1)))].l52) = l241>>1;(
l55[(l1)][((((3*(64+32+2+1))-1)))].l52) = (l241-(l55[(l1)][(((((3*(64+32+2+1)
)/3)-1)))].l52));}else{(l55[(l1)][((((3*(64+32+2+1))-1)))].l52) = l241;}if((
l116[(l1)])!= NULL){if(l124((l116[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}(l116[(l1)]) = NULL;}if(l122(l1,l241*2,l241,&(l116[(l1)]))<0)
{SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l1);return(
SOC_E_NONE);}static int l3(int l1){if(!soc_feature(l1,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);if((l116[(l1)])!= NULL){l124((
l116[(l1)]));(l116[(l1)]) = NULL;}if((l55[(l1)]!= NULL)){sal_free(l105[l1]);
l105[l1] = NULL;sal_free((l55[(l1)]));(l55[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK
(l1);return(SOC_E_NONE);}static int l4(int l1,void*l5,int*l298){int l129;int
index;int l7;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l147 = SOC_E_NONE;int
l299 = 0;sal_memcpy(l12,soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(
l1,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l1);l147 = l11(l1,l5,l12,&index,&l129,&l7)
;if(l147 == SOC_E_NOT_FOUND){l147 = l280(l1,l129,l7,l12,&index);if(l147<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l147);}}else{l299 = 1;}*l298 = index;if(l147 == 
SOC_E_NONE){if(!l7){if(index&1){l147 = soc_th_alpm_lpm_ip4entry0_to_1(l1,l5,
l12,PRESERVE_HIT);}else{l147 = soc_th_alpm_lpm_ip4entry0_to_0(l1,l5,l12,
PRESERVE_HIT);}if(l147<0){SOC_ALPM_LPM_UNLOCK(l1);return(l147);}l5 = (void*)
l12;index>>= 1;}soc_th_alpm_lpm_state_dump(l1);LOG_BSL_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_lpm_insert: %d %d\n"),index,l129));if(!l299){l249(l1,
l5,index,0x4000,0);}l147 = l168(l1,MEM_BLOCK_ANY,index,index,l5);if(l147>= 0)
{l147 = l264(l1,index,l5);}}SOC_ALPM_LPM_UNLOCK(l1);return(l147);}static int
soc_th_alpm_lpm_delete(int l1,void*key_data){int l129;int index;int l7;uint32
l12[SOC_MAX_MEM_FIELD_WORDS];int l147 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l147
= l11(l1,key_data,l12,&index,&l129,&l7);if(l147 == SOC_E_NONE){LOG_BSL_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_lpm_delete: %d %d\n"),index,l129));
l252(l1,key_data,index);l147 = l286(l1,l129,l7,l12,index);}
soc_th_alpm_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l147);}static
int l15(int l1,void*key_data,void*l12,int*l13){int l129;int l147;int l7;
SOC_ALPM_LPM_LOCK(l1);l147 = l11(l1,key_data,l12,l13,&l129,&l7);
SOC_ALPM_LPM_UNLOCK(l1);return(l147);}static int l6(int unit,void*key_data,
int l7,int l8,int l9,defip_aux_scratch_entry_t*l10){uint32 l261;uint32 l300[4
] = {0,0,0,0};int l129 = 0;int l147 = SOC_E_NONE;l261 = soc_mem_field32_get(
unit,L3_DEFIPm,key_data,VALID0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l10,VALIDf,l261);l261 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,MODE0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10
,MODEf,l261);l261 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,ENTRY_TYPE0f)
;soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,ENTRY_TYPEf,0);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,GLOBAL_ROUTE0f);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,GLOBAL_ROUTEf,l261);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,ECMP0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l10,ECMPf,l261);l261 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,ECMP_PTR0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l10,ECMP_PTRf,l261);l261 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
NEXT_HOP_INDEX0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,
NEXT_HOP_INDEXf,l261);l261 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
PRI0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,PRIf,l261);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,RPE0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l10,RPEf,l261);l261 =soc_mem_field32_get(unit,L3_DEFIPm
,key_data,VRF_ID_0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,VRFf,
l261);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,DB_TYPEf,l8);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,DST_DISCARD0f);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,DST_DISCARDf,l261);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,CLASS_ID0f);soc_mem_field32_set(
unit,L3_DEFIP_AUX_SCRATCHm,l10,CLASS_IDf,l261);if(l7){l300[2] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR0f);l300[3] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR1f);}else{l300[0] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR0f);}soc_mem_field_set(
unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)l10,IP_ADDRf,(uint32*)l300);if(l7){l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR_MASK0f);if((l147 = 
_ipmask2pfx(l261,&l129))<0){return(l147);}l261 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,IP_ADDR_MASK1f);if(l129){if(l261!= 0xffffffff){return(
SOC_E_PARAM);}l129+= 32;}else{if((l147 = _ipmask2pfx(l261,&l129))<0){return(
l147);}}}else{l261 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
IP_ADDR_MASK0f);if((l147 = _ipmask2pfx(l261,&l129))<0){return(l147);}}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,IP_LENGTHf,l129);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,REPLACE_LENf,l9);return(
SOC_E_NONE);}int _soc_th_alpm_aux_op(int l1,_soc_aux_op_t l301,
defip_aux_scratch_entry_t*l10,int l302,int*l150,int*tcam_index,int*
bucket_index){uint32 l137,l303;int l304;soc_timeout_t l305;int l147 = 
SOC_E_NONE;int l306 = 0;if(l302){SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_SCRATCHm(l1,MEM_BLOCK_ANY,0,l10));}l307:l137 = 0;switch(
l301){case INSERT_PROPAGATE:l304 = 0;break;case DELETE_PROPAGATE:l304 = 1;
break;case PREFIX_LOOKUP:l304 = 2;break;case HITBIT_REPLACE:l304 = 3;break;
default:return SOC_E_PARAM;}soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l137,
OPCODEf,l304);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l137,STARTf,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l137));soc_timeout_init(&l305
,50000,5);l304 = 0;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l1,&l137));
l304 = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l137,DONEf);if(l304 == 1){l147
= SOC_E_NONE;break;}if(soc_timeout_check(&l305)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l1,&l137));l304 = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRLr,l137,DONEf);if(l304 == 1){l147 = SOC_E_NONE;}else{LOG_WARN
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d : DEFIP AUX Operation timeout\n"),l1
));l147 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l147)){if(
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l137,ERRORf)){soc_reg_field_set(l1,
L3_DEFIP_AUX_CTRLr,&l137,STARTf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&
l137,ERRORf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l137,DONEf,0);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l137));LOG_BSL_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: DEFIP AUX Operation encountered "
"parity error !!\n"),l1));l306++;if(SOC_CONTROL(l1)->alpm_bulk_retry){
sal_sem_take(SOC_CONTROL(l1)->alpm_bulk_retry,1000000);}if(l306<5){LOG_BSL_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: Retry DEFIP AUX Operation..\n"),l1))
;goto l307;}else{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l1));return SOC_E_INTERNAL;}}if(l301 == PREFIX_LOOKUP){if(l150&&tcam_index){*
l150 = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l137,HITf);*tcam_index = 
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l137,BKT_INDEXf);SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRL_1r(l1,&l303));*bucket_index = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRL_1r,l303,BKT_PTRf);}}}return l147;}static int l17(int unit,
void*lpm_entry,void*l18,void*l19,soc_mem_t l20,uint32 l21,uint32*l308){uint32
l261;uint32 l300[4] = {0,0};int l129 = 0;int l147 = SOC_E_NONE;int l7;uint32
l22 = 0;l7 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,MODE0f);sal_memset(
l18,0,soc_mem_entry_words(unit,l20)*4);l261 = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,HIT0f);soc_mem_field32_set(unit,l20,l18,HITf,l261);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,VALID0f);soc_mem_field32_set(
unit,l20,l18,VALIDf,l261);l261 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry
,ECMP0f);soc_mem_field32_set(unit,l20,l18,ECMPf,l261);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set(
unit,l20,l18,ECMP_PTRf,l261);l261 = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,NEXT_HOP_INDEX0f);soc_mem_field32_set(unit,l20,l18,NEXT_HOP_INDEXf,
l261);l261 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,PRI0f);
soc_mem_field32_set(unit,l20,l18,PRIf,l261);l261 = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,RPE0f);soc_mem_field32_set(unit,l20,l18,RPEf,l261);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(unit,l20,l18,DST_DISCARDf,l261);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);
soc_mem_field32_set(unit,l20,l18,SRC_DISCARDf,l261);l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,CLASS_ID0f);soc_mem_field32_set(
unit,l20,l18,CLASS_IDf,l261);l300[0] = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,IP_ADDR0f);if(l7){l300[1] = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,IP_ADDR1f);}soc_mem_field_set(unit,l20,(uint32*)l18,KEYf,(uint32*)
l300);if(l7){l261 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f);if((l147 = _ipmask2pfx(l261,&l129))<0){return(l147);}l261 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f);if(l129){if(l261
!= 0xffffffff){return(SOC_E_PARAM);}l129+= 32;}else{if((l147 = _ipmask2pfx(
l261,&l129))<0){return(l147);}}}else{l261 = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l147 = _ipmask2pfx(l261,&l129))<0){
return(l147);}}if((l129 == 0)&&(l300[0] == 0)&&(l300[1] == 0)){l22 = 1;}if(
l308!= NULL){*l308 = l22;}soc_mem_field32_set(unit,l20,l18,LENGTHf,l129);if(
l19 == NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){sal_memset(l19
,0,soc_mem_entry_words(unit,l20)*4);sal_memcpy(l19,l18,soc_mem_entry_words(
unit,l20)*4);soc_mem_field32_set(unit,l20,l19,DST_DISCARDf,0);
soc_mem_field32_set(unit,l20,l19,RPEf,0);soc_mem_field32_set(unit,l20,l19,
SRC_DISCARDf,l21&SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(unit,l20,l19,
DEFAULTROUTEf,l22);}return(SOC_E_NONE);}static int l23(int unit,void*l18,
soc_mem_t l20,int l7,int l24,int l25,int index,void*lpm_entry){uint32 l261;
uint32 l300[4] = {0,0};uint32 l129 = 0;sal_memset(lpm_entry,0,
soc_mem_entry_words(unit,L3_DEFIPm)*4);l261 = soc_mem_field32_get(unit,l20,
l18,HITf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,HIT0f,l261);if(l7){
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,HIT1f,l261);}l261 = 
soc_mem_field32_get(unit,l20,l18,VALIDf);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,VALID0f,l261);if(l7){soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
VALID1f,l261);}l261 = soc_mem_field32_get(unit,l20,l18,ECMPf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ECMP0f,l261);l261 = 
soc_mem_field32_get(unit,l20,l18,ECMP_PTRf);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,ECMP_PTR0f,l261);l261 = soc_mem_field32_get(unit,l20,l18,
NEXT_HOP_INDEXf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
NEXT_HOP_INDEX0f,l261);l261 = soc_mem_field32_get(unit,l20,l18,PRIf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,PRI0f,l261);l261 = 
soc_mem_field32_get(unit,l20,l18,RPEf);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,RPE0f,l261);l261 = soc_mem_field32_get(unit,l20,l18,DST_DISCARDf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,DST_DISCARD0f,l261);l261 = 
soc_mem_field32_get(unit,l20,l18,SRC_DISCARDf);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,SRC_DISCARD0f,l261);l261 = soc_mem_field32_get(unit,l20,
l18,CLASS_IDf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,CLASS_ID0f,l261);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ALG_BKT_PTR0f,l25);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ALG_HIT_IDX0f,index);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE_MASK0f,3);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l7){
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(
unit,l20,l18,KEYf,l300);if(l7){soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR1f,l300[1]);}soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR0f,
l300[0]);l261 = soc_mem_field32_get(unit,l20,l18,LENGTHf);if(l7){if(l261>= 32
){l129 = 0xffffffff;soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l129);l129 = ~(((l261-32) == 32)?0:(0xffffffff)>>(l261-32));
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l129);}else{l129 = 
~(0xffffffff>>l261);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l129);}}else{assert(l261<= 32);l129 = ~(((l261) == 32)?0:(
0xffffffff)>>(l261));soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,l129);}if(l24 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(unit
,L3_DEFIPm,lpm_entry,GLOBAL_HIGH0f,1);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else if(l24 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else{soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VRF_ID_0f,l24);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(unit))
;}return(SOC_E_NONE);}int soc_th_alpm_warmboot_pivot_add(int unit,int l7,void
*lpm_entry,int l309,int l310){int l147 = SOC_E_NONE;uint32 key[2] = {0,0};
alpm_pivot_t*l34 = NULL;alpm_bucket_handle_t*l202 = NULL;int l154 = 0,l24 = 0
;uint32 l311;trie_t*l236 = NULL;uint32 prefix[5] = {0};int l22 = 0;l147 = 
l144(unit,lpm_entry,prefix,&l311,&l22);SOC_IF_ERROR_RETURN(l147);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l154,&l24));l309 = 
soc_th_alpm_physical_idx(unit,L3_DEFIPm,l309,l7);l202 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l202 == NULL){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l202,0,sizeof(*l202));
l34 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l34 == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l202);return SOC_E_MEMORY;}sal_memset(l34,0,
sizeof(*l34));PIVOT_BUCKET_HANDLE(l34) = l202;if(l7){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l34));key[0] = soc_L3_DEFIPm_field32_get
(unit,lpm_entry,IP_ADDR0f);key[1] = soc_L3_DEFIPm_field32_get(unit,lpm_entry,
IP_ADDR1f);}else{trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l34));key[0] = 
soc_L3_DEFIPm_field32_get(unit,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l34) = 
l310;PIVOT_TCAM_INDEX(l34) = l309;if(l154!= SOC_L3_VRF_OVERRIDE){if(l7 == 0){
l236 = VRF_PIVOT_TRIE_IPV4(unit,l24);if(l236 == NULL){trie_init(
_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(unit,l24));l236 = VRF_PIVOT_TRIE_IPV4(
unit,l24);}}else{l236 = VRF_PIVOT_TRIE_IPV6(unit,l24);if(l236 == NULL){
trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(unit,l24));l236 = 
VRF_PIVOT_TRIE_IPV6(unit,l24);}}sal_memcpy(l34->key,prefix,sizeof(prefix));
l34->len = l311;l147 = trie_insert(l236,l34->key,NULL,l34->len,(trie_node_t*)
l34);if(SOC_FAILURE(l147)){sal_free(l202);sal_free(l34);return l147;}}
ALPM_TCAM_PIVOT(unit,l309) = l34;PIVOT_BUCKET_VRF(l34) = l24;
PIVOT_BUCKET_IPV6(l34) = l7;PIVOT_BUCKET_ENT_CNT_UPDATE(l34);if(key[0] == 0&&
key[1] == 0){PIVOT_BUCKET_DEF(l34) = TRUE;}VRF_PIVOT_REF_INC(unit,l24,l7);
return l147;}static int l312(int unit,int l7,void*lpm_entry,void*l18,
soc_mem_t l20,int l309,int l310,int l313){int l314;int l24;int l147 = 
SOC_E_NONE;int l22 = 0;uint32 prefix[5] = {0,0,0,0,0};uint32 l35;void*l315 = 
NULL;trie_t*l203 = NULL;trie_t*l33 = NULL;trie_node_t*l198 = NULL;payload_t*
l316 = NULL;payload_t*l216 = NULL;alpm_pivot_t*pivot_pyld = NULL;if((NULL == 
lpm_entry)||(NULL == l18)){return SOC_E_PARAM;}if(l7){if(!(l7 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l314,&l24));l20 = 
(l7)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l315 = sal_alloc(sizeof(
defip_entry_t),"Temp lpm_entr");if(NULL == l315){return SOC_E_MEMORY;}
SOC_IF_ERROR_RETURN(l23(unit,l18,l20,l7,l314,l310,l309,l315));l147 = l144(
unit,l315,prefix,&l35,&l22);if(SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"prefix create failed\n")));return l147;}sal_free(l315);
pivot_pyld = ALPM_TCAM_PIVOT(unit,l309);l203 = PIVOT_BUCKET_TRIE(pivot_pyld);
l316 = sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == l316){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l216 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l216){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node\n")));sal_free(l316);return
SOC_E_MEMORY;}sal_memset(l316,0,sizeof(*l316));sal_memset(l216,0,sizeof(*l216
));l316->key[0] = prefix[0];l316->key[1] = prefix[1];l316->key[2] = prefix[2]
;l316->key[3] = prefix[3];l316->key[4] = prefix[4];l316->len = l35;l316->
index = l313;sal_memcpy(l216,l316,sizeof(*l316));l147 = trie_insert(l203,
prefix,NULL,l35,(trie_node_t*)l316);if(SOC_FAILURE(l147)){goto l317;}if(l7){
l33 = VRF_PREFIX_TRIE_IPV6(unit,l24);}else{l33 = VRF_PREFIX_TRIE_IPV4(unit,
l24);}if(!l22){l147 = trie_insert(l33,prefix,NULL,l35,(trie_node_t*)l216);if(
SOC_FAILURE(l147)){goto l318;}}return l147;l318:(void)trie_delete(l203,prefix
,l35,&l198);l316 = (payload_t*)l198;l317:sal_free(l316);sal_free(l216);return
l147;}static int l319(int unit,int l30,int l24,int l139,int bkt_ptr){int l147
= SOC_E_NONE;uint32 l35;uint32 key[2] = {0,0};trie_t*l320 = NULL;payload_t*
l226 = NULL;defip_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unable to allocate memory for LPM entry\n")
));return SOC_E_MEMORY;}l26(unit,key,0,l24,l30,lpm_entry,0,1);if(l24 == 
SOC_VRF_MAX(unit)+1){soc_L3_DEFIPm_field32_set(unit,lpm_entry,GLOBAL_ROUTE0f,
1);}else{soc_L3_DEFIPm_field32_set(unit,lpm_entry,DEFAULT_MISS0f,1);}
soc_L3_DEFIPm_field32_set(unit,lpm_entry,ALG_BKT_PTR0f,bkt_ptr);if(l30 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(unit,l24) = lpm_entry;trie_init(_MAX_KEY_LEN_48_,
&VRF_PREFIX_TRIE_IPV4(unit,l24));l320 = VRF_PREFIX_TRIE_IPV4(unit,l24);}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(unit,l24) = lpm_entry;trie_init(_MAX_KEY_LEN_144_
,&VRF_PREFIX_TRIE_IPV6(unit,l24));l320 = VRF_PREFIX_TRIE_IPV6(unit,l24);}l226
= sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(l226 == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node \n")));return SOC_E_MEMORY;}
sal_memset(l226,0,sizeof(*l226));l35 = 0;l226->key[0] = key[0];l226->key[1] = 
key[1];l226->len = l35;l147 = trie_insert(l320,key,NULL,l35,&(l226->node));if
(SOC_FAILURE(l147)){sal_free(l226);return l147;}VRF_TRIE_INIT_DONE(unit,l24,
l30,1);return l147;}int soc_th_alpm_warmboot_prefix_insert(int unit,int l7,
void*lpm_entry,void*l18,int l309,int l310,int l313){int l314;int l24;int l147
= SOC_E_NONE;soc_mem_t l20;l309 = soc_th_alpm_physical_idx(unit,L3_DEFIPm,
l309,l7);l20 = (l7)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l314,&l24));if(
l314 == SOC_L3_VRF_OVERRIDE){return(l147);}if(!VRF_TRIE_INIT_COMPLETED(unit,
l24,l7)){LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d is not initialized\n"),l24));l147 = l319(unit,l7,l24,l309,l310);if(
SOC_FAILURE(l147)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d/%d trie init \n""failed\n"),l24,l7));return l147;}LOG_BSL_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d/%d trie init completed\n"),l24,l7));
}l147 = l312(unit,l7,lpm_entry,l18,l20,l309,l310,l313);if(l147!= SOC_E_NONE){
LOG_BSL_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"unit %d : Route Insertion Failed :%s\n"),unit,soc_errmsg(l147)));return(l147
);}VRF_TRIE_ROUTES_INC(unit,l24,l7);return(l147);}int
soc_th_alpm_warmboot_bucket_bitmap_set(int l1,int l30,int l238){int l242 = 1;
if(l30){if(!soc_th_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l242 = 2;}}if
(SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l238,l242);return SOC_E_NONE;}int
soc_th_alpm_warmboot_lpm_reinit_done(int unit){int l139;int l321 = ((3*(64+32
+2+1))-1);int l241 = soc_mem_index_count(unit,L3_DEFIPm);if(
SOC_URPF_STATUS_GET(unit)){l241>>= 1;}if(!soc_th_alpm_mode_get(unit)){(l55[(
unit)][(((3*(64+32+2+1))-1))].l50) = -1;for(l139 = ((3*(64+32+2+1))-1);l139>-
1;l139--){if(-1 == (l55[(unit)][(l139)].l48)){continue;}(l55[(unit)][(l139)].
l50) = l321;(l55[(unit)][(l321)].next) = l139;(l55[(unit)][(l321)].l52) = (
l55[(unit)][(l139)].l48)-(l55[(unit)][(l321)].l49)-1;l321 = l139;}(l55[(unit)
][(l321)].next) = -1;(l55[(unit)][(l321)].l52) = l241-(l55[(unit)][(l321)].
l49)-1;}else{(l55[(unit)][(((3*(64+32+2+1))-1))].l50) = -1;for(l139 = ((3*(64
+32+2+1))-1);l139>(((3*(64+32+2+1))-1)/3);l139--){if(-1 == (l55[(unit)][(l139
)].l48)){continue;}(l55[(unit)][(l139)].l50) = l321;(l55[(unit)][(l321)].next
) = l139;(l55[(unit)][(l321)].l52) = (l55[(unit)][(l139)].l48)-(l55[(unit)][(
l321)].l49)-1;l321 = l139;}(l55[(unit)][(l321)].next) = -1;(l55[(unit)][(l321
)].l52) = l241-(l55[(unit)][(l321)].l49)-1;l321 = (((3*(64+32+2+1))-1)/3);(
l55[(unit)][((((3*(64+32+2+1))-1)/3))].l50) = -1;for(l139 = ((((3*(64+32+2+1)
)-1)/3)-1);l139>-1;l139--){if(-1 == (l55[(unit)][(l139)].l48)){continue;}(l55
[(unit)][(l139)].l50) = l321;(l55[(unit)][(l321)].next) = l139;(l55[(unit)][(
l321)].l52) = (l55[(unit)][(l139)].l48)-(l55[(unit)][(l321)].l49)-1;l321 = 
l139;}(l55[(unit)][(l321)].next) = -1;(l55[(unit)][(l321)].l52) = (l241>>1)-(
l55[(unit)][(l321)].l49)-1;}return(SOC_E_NONE);}int
soc_th_alpm_warmboot_lpm_reinit(int unit,int l7,int l139,void*lpm_entry){int
l14;defip_entry_t*l322;if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID1f)){l249(unit,lpm_entry,l139,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l293(unit,lpm_entry,&l14));if((l55[(unit)][(l14)].l51) == 
0){(l55[(unit)][(l14)].l48) = l139;(l55[(unit)][(l14)].l49) = l139;}else{(l55
[(unit)][(l14)].l49) = l139;}(l55[(unit)][(l14)].l51)++;if(l7){return(
SOC_E_NONE);}}else{if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID1f)){l322
= sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");
soc_th_alpm_lpm_ip4entry1_to_0(unit,lpm_entry,l322,TRUE);SOC_IF_ERROR_RETURN(
l293(unit,l322,&l14));if((l55[(unit)][(l14)].l51) == 0){(l55[(unit)][(l14)].
l48) = l139;(l55[(unit)][(l14)].l49) = l139;}else{(l55[(unit)][(l14)].l49) = 
l139;}sal_free(l322);(l55[(unit)][(l14)].l51)++;}}return(SOC_E_NONE);}typedef
struct l323{int v4;int v6_64;int v6_128;int l324;int l325;int l326;int l243;}
l327;typedef enum l328{l329 = 0,l330,l331,l332,l333,l334}l335;static void l336
(int l1,alpm_vrf_counter_t*l337){l337->v4 = soc_mem_index_count(l1,L3_DEFIPm)
*2;l337->v6_128 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(
soc_property_get(l1,spn_IPV6_LPM_128B_ENABLE,1)){l337->v6_64 = l337->v6_128;}
else{l337->v6_64 = l337->v4>>1;}if(SOC_URPF_STATUS_GET(l1)){l337->v4>>= 1;
l337->v6_128>>= 1;l337->v6_64>>= 1;}}static void l338(int l1,int l154,
alpm_vrf_handle_t*l339,l335 l340){alpm_vrf_counter_t*l341;int l174,l342,l343,
l344;int l299 = 0;alpm_vrf_counter_t l337;switch(l340){case l329:LOG_CLI((
BSL_META_U(l1,"\nAdd Counter:\n")));break;case l330:LOG_CLI((BSL_META_U(l1,
"\nDelete Counter:\n")));break;case l331:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - 1:\n")));break;case l332:l336(l1,&l337);LOG_CLI((
BSL_META_U(l1,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l337.v4,
l337.v6_64,l337.v6_128));break;case l333:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l334:LOG_CLI((
BSL_META_U(l1,"\nInternal Debug Counter - LPM Full:\n")));break;default:break
;}LOG_CLI((BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n"))
);LOG_CLI((BSL_META_U(l1,"-----------------------------------------------\n")
));l342 = l343 = l344 = 0;for(l174 = 0;l174<MAX_VRF_ID+1;l174++){int l345,
l346,l347;if(l339[l174].init_done == 0&&l174!= MAX_VRF_ID){continue;}if(l154
!= -1&&l154!= l174){continue;}l299 = 1;switch(l340){case l329:l341 = &l339[
l174].add;break;case l330:l341 = &l339[l174].del;break;case l331:l341 = &l339
[l174].bkt_split;break;case l333:l341 = &l339[l174].lpm_shift;break;case l334
:l341 = &l339[l174].lpm_full;break;case l332:l341 = &l339[l174].pivot_used;
break;default:l341 = &l339[l174].pivot_used;break;}l345 = l341->v4;l346 = 
l341->v6_64;l347 = l341->v6_128;l342+= l345;l343+= l346;l344+= l347;do{
LOG_CLI((BSL_META_U(l1,"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l174 == 
MAX_VRF_ID?-1:l174),(l345),(l346),(l347),((l345+l346+l347)),(l174) == 
MAX_VRF_ID?"GHi":(l174) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l299 == 
0){LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));do{
LOG_CLI((BSL_META_U(l1,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l342),(
l343),(l344),((l342+l343+l344))));}while(0);}return;}int l348(int l1,int l154
,uint32 flags){int l174,l349,l299 = 0;l327*l350;l327 l351;l327 l352;if(l154>(
SOC_VRF_MAX(l1)+1)){return SOC_E_PARAM;}l349 = MAX_VRF_ID*sizeof(l327);l350 = 
sal_alloc(l349,"_alpm_dbg_cnt");if(l350 == NULL){return SOC_E_MEMORY;}
sal_memset(l350,0,l349);l351.v4 = ALPM_IPV4_BKT_COUNT;l351.v6_64 = 
ALPM_IPV6_64_BKT_COUNT;l351.v6_128 = ALPM_IPV6_128_BKT_COUNT;if(!
soc_th_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l351.v6_64<<= 1;l351.
v6_128<<= 1;}LOG_CLI((BSL_META_U(l1,"\nBucket Occupancy:\n")));if(flags&(
SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l1,
"\n  BKT/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));}for(
l174 = 0;l174<MAX_PIVOT_COUNT;l174++){alpm_pivot_t*l353 = ALPM_TCAM_PIVOT(l1,
l174);if(l353!= NULL){l327*l354;int l24 = PIVOT_BUCKET_VRF(l353);if(l24<0||
l24>(SOC_VRF_MAX(l1)+1)){continue;}if(l154!= -1&&l154!= l24){continue;}if(
flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l299 = 1;do{LOG_CLI((BSL_META_U(l1,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l174,l24,PIVOT_BUCKET_MIN(l353),
PIVOT_BUCKET_MAX(l353),PIVOT_BUCKET_COUNT(l353),PIVOT_BUCKET_DEF(l353)?"Def":
(l24) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}l354 = &l350[l24];if(
PIVOT_BUCKET_IPV6(l353) == L3_DEFIP_MODE_128){l354->v6_128+= 
PIVOT_BUCKET_COUNT(l353);l354->l326+= l351.v6_128;}else if(PIVOT_BUCKET_IPV6(
l353) == L3_DEFIP_MODE_64){l354->v6_64+= PIVOT_BUCKET_COUNT(l353);l354->l325
+= l351.v6_64;}else{l354->v4+= PIVOT_BUCKET_COUNT(l353);l354->l324+= l351.v4;
}l354->l243 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l299 == 0){
LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}}sal_memset(&l352
,0,sizeof(l352));l299 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){LOG_CLI((
BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));for(l174
= 0;l174<MAX_VRF_ID;l174++){l327*l354;if(l350[l174].l243!= TRUE){continue;}if
(l154!= -1&&l154!= l174){continue;}l299 = 1;l354 = &l350[l174];do{(&l352)->v4
+= (l354)->v4;(&l352)->l324+= (l354)->l324;(&l352)->v6_64+= (l354)->v6_64;(&
l352)->l325+= (l354)->l325;(&l352)->v6_128+= (l354)->v6_128;(&l352)->l326+= (
l354)->l326;}while(0);do{LOG_CLI((BSL_META_U(l1,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l174),(l354
->l324)?(l354->v4)*100/(l354->l324):0,(l354->l324)?(l354->v4)*1000/(l354->
l324)%10:0,(l354->l325)?(l354->v6_64)*100/(l354->l325):0,(l354->l325)?(l354->
v6_64)*1000/(l354->l325)%10:0,(l354->l326)?(l354->v6_128)*100/(l354->l326):0,
(l354->l326)?(l354->v6_128)*1000/(l354->l326)%10:0,((l354->l324+l354->l325+
l354->l326))?((l354->v4+l354->v6_64+l354->v6_128))*100/((l354->l324+l354->
l325+l354->l326)):0,((l354->l324+l354->l325+l354->l326))?((l354->v4+l354->
v6_64+l354->v6_128))*1000/((l354->l324+l354->l325+l354->l326))%10:0,(l174) == 
SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l299 == 0){LOG_CLI((BSL_META_U(l1,
"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l1,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l1,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l352)->l324)?((&l352)->v4)*100/((&l352)->l324):0,((&l352)->l324)?((&l352)->v4
)*1000/((&l352)->l324)%10:0,((&l352)->l325)?((&l352)->v6_64)*100/((&l352)->
l325):0,((&l352)->l325)?((&l352)->v6_64)*1000/((&l352)->l325)%10:0,((&l352)->
l326)?((&l352)->v6_128)*100/((&l352)->l326):0,((&l352)->l326)?((&l352)->
v6_128)*1000/((&l352)->l326)%10:0,(((&l352)->l324+(&l352)->l325+(&l352)->l326
))?(((&l352)->v4+(&l352)->v6_64+(&l352)->v6_128))*100/(((&l352)->l324+(&l352)
->l325+(&l352)->l326)):0,(((&l352)->l324+(&l352)->l325+(&l352)->l326))?(((&
l352)->v4+(&l352)->v6_64+(&l352)->v6_128))*1000/(((&l352)->l324+(&l352)->l325
+(&l352)->l326))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l338(l1,l154,alpm_vrf_handle[l1],l332);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_CNT
){l338(l1,l154,alpm_vrf_handle[l1],l329);l338(l1,l154,alpm_vrf_handle[l1],
l330);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l338(l1,l154,alpm_vrf_handle
[l1],l331);l338(l1,l154,alpm_vrf_handle[l1],l334);l338(l1,l154,
alpm_vrf_handle[l1],l333);}sal_free(l350);return SOC_E_NONE;}
#endif

#endif /* ALPM_ENABLE */
