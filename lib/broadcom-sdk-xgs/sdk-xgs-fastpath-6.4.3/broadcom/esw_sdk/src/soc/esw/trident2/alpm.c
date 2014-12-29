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
 * Purpose: Primitives for LPM management in ALPM - Mode.
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
soc_alpm_bucket_t soc_alpm_bucket[SOC_MAX_NUM_DEVICES];void
soc_alpm_lpm_state_dump(int l1);static int l2(int l1);static int l3(int l1);
static int l4(int l1,void*l5,int*index);static int l6(int l1,void*l7);static
int l8(int l1,void*l7,int l9,int l10,int l11,defip_aux_scratch_entry_t*l12);
static int l13(int l1,void*l7,void*l14,int*l15,int*l16,int*l9);static int l17
(int l1,void*l7,void*l14,int*l15);static int l18(int l1);static int l19(int
l20,void*lpm_entry,void*l21,void*l22,soc_mem_t l23,uint32 l24,uint32*l25);
static int l26(int l20,void*l21,soc_mem_t l23,int l9,int l27,int l28,int index
,void*lpm_entry);static int l29(int l20,uint32*key,int len,int l27,int l9,
defip_entry_t*lpm_entry,int l30,int l31);static int l32(int l1,int l27,int l33
);static int l34(int l1,void*entry,int*l16);typedef struct l35{int l36;int l37
;int l38;int next;int l39;int l40;}l41,*l42;static l42 l43[
SOC_MAX_NUM_DEVICES];typedef struct l44{soc_field_info_t*l45;soc_field_info_t
*l46;soc_field_info_t*l47;soc_field_info_t*l48;soc_field_info_t*l49;
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
soc_field_info_t*l89;soc_field_info_t*l90;soc_field_info_t*l91;
soc_field_info_t*l92;}l93,*l94;static l94 l95[SOC_MAX_NUM_DEVICES];typedef
struct l96{int l20;int l97;int l98;uint16*l99;uint16*l100;}l101;typedef uint32
l102[5];typedef int(*l103)(l102 l104,l102 l105);static l101*l106[
SOC_MAX_NUM_DEVICES];static void l107(int l1,void*l14,int index,l102 l108);
static uint16 l109(uint8*l110,int l111);static int l112(int l20,int l97,int
l98,l101**l113);static int l114(l101*l115);static int l116(l101*l117,l103 l118
,l102 entry,int l119,uint16*l120);static int l121(l101*l117,l103 l118,l102
entry,int l119,uint16 l122,uint16 l123);static int l124(l101*l117,l103 l118,
l102 entry,int l119,uint16 l125);alpm_vrf_handle_t*alpm_vrf_handle[
SOC_MAX_NUM_DEVICES];alpm_pivot_t**tcam_pivot[SOC_MAX_NUM_DEVICES];int
soc_alpm_mode_get(int l1){if(soc_trident2_alpm_mode_get(l1) == 1){return 1;}
else{return 0;}}static int l126(int l1,const void*entry,int*l119){int l127;
uint32 l128;int l9;l9 = soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f);if(l9)
{l128 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l127 = 
_ipmask2pfx(l128,l119))<0){return(l127);}l128 = soc_mem_field32_get(l1,
L3_DEFIPm,entry,IP_ADDR_MASK1f);if(*l119){if(l128!= 0xffffffff){return(
SOC_E_PARAM);}*l119+= 32;}else{if((l127 = _ipmask2pfx(l128,l119))<0){return(
l127);}}}else{l128 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);
if((l127 = _ipmask2pfx(l128,l119))<0){return(l127);}}return SOC_E_NONE;}int
_soc_alpm_rpf_entry(int l1,int l129){int l130;l130 = (l129>>2)&0x3fff;l130+= 
SOC_ALPM_BUCKET_COUNT(l1);return(l129&~(0x3fff<<2))|(l130<<2);}int
soc_alpm_physical_idx(int l1,soc_mem_t l23,int index,int l131){int l132 = 
index&1;if(l131){return soc_trident2_l3_defip_index_map(l1,l23,index);}index
>>= 1;index = soc_trident2_l3_defip_index_map(l1,l23,index);index<<= 1;index
|= l132;return index;}int soc_alpm_logical_idx(int l1,soc_mem_t l23,int index
,int l131){int l132 = index&1;if(l131){return
soc_trident2_l3_defip_index_remap(l1,l23,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_remap(l1,l23,index);index<<= 1;index|= l132;
return index;}static int l133(int l1,void*entry,uint32*prefix,uint32*l16,int*
l25){int l134,l135,l9;int l119 = 0;int l127 = SOC_E_NONE;uint32 l136,l132;
prefix[0] = prefix[1] = prefix[2] = prefix[3] = prefix[4] = 0;l9 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f);l134 = soc_mem_field32_get(l1,
L3_DEFIPm,entry,IP_ADDR0f);l135 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK0f);prefix[1] = l134;l134 = soc_mem_field32_get(l1,L3_DEFIPm,
entry,IP_ADDR1f);l135 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK1f
);prefix[0] = l134;if(l9){prefix[4] = prefix[1];prefix[3] = prefix[0];prefix[
1] = prefix[0] = 0;l135 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK0f);if((l127 = _ipmask2pfx(l135,&l119))<0){return(l127);}l135 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK1f);if(l119){if(l135!= 
0xffffffff){return(SOC_E_PARAM);}l119+= 32;}else{if((l127 = _ipmask2pfx(l135,
&l119))<0){return(l127);}}l136 = 64-l119;if(l136<32){prefix[4]>>= l136;l132 = 
(((32-l136) == 32)?0:(prefix[3])<<(32-l136));prefix[3]>>= l136;prefix[4]|= 
l132;}else{prefix[4] = (((l136-32) == 32)?0:(prefix[3])>>(l136-32));prefix[3]
= 0;}}else{l135 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((
l127 = _ipmask2pfx(l135,&l119))<0){return(l127);}prefix[1] = (((32-l119) == 
32)?0:(prefix[1])>>(32-l119));prefix[0] = 0;}*l16 = l119;*l25 = (prefix[0] == 
0)&&(prefix[1] == 0)&&(l119 == 0);return SOC_E_NONE;}int _soc_alpm_find_in_bkt
(int l1,soc_mem_t l23,int bucket_index,int l137,uint32*l14,void*l138,int*l120
,int l33){int l127;l127 = soc_mem_alpm_lookup(l1,l23,bucket_index,
MEM_BLOCK_ANY,l137,l14,l138,l120);if(SOC_SUCCESS(l127)){return l127;}if(
SOC_ALPM_V6_SCALE_CHECK(l1,l33)){return soc_mem_alpm_lookup(l1,l23,
bucket_index+1,MEM_BLOCK_ANY,l137,l14,l138,l120);}return l127;}static int l139
(int l1,uint32*prefix,uint32 l140,int l33,int l27,int*l141,int*l142,int*
bucket_index){int l127 = SOC_E_NONE;trie_t*l143;trie_node_t*l144 = NULL;
alpm_pivot_t*l145;if(l33){l143 = VRF_PIVOT_TRIE_IPV6(l1,l27);}else{l143 = 
VRF_PIVOT_TRIE_IPV4(l1,l27);}l127 = trie_find_lpm(l143,prefix,l140,&l144);if(
SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l127;}l145 = (alpm_pivot_t*)l144;*l141 = 1;*
l142 = PIVOT_TCAM_INDEX(l145);*bucket_index = PIVOT_BUCKET_INDEX(l145);return
SOC_E_NONE;}static int l146(int l1,void*l7,soc_mem_t l23,void*l138,int*l142,
int*bucket_index,int*l15,int l147){uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int
l148,l27,l33;int l120;uint32 l10,l137;int l127 = SOC_E_NONE;int l141 = 0;l33 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f);if(l33){if(!(l33 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l7,&l148,&l27));if(l148 == 0){if(
soc_alpm_mode_get(l1)){return SOC_E_PARAM;}}if(l27 == SOC_VRF_MAX(l1)+1){l10 = 
0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l137);}else{l10 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l137);}if(l148!= SOC_L3_VRF_OVERRIDE){if(
l147){uint32 prefix[5],l140;int l25 = 0;l127 = l133(l1,l7,prefix,&l140,&l25);
if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l127;}l127 = l139(l1,
prefix,l140,l33,l27,&l141,l142,bucket_index);SOC_IF_ERROR_RETURN(l127);}else{
defip_aux_scratch_entry_t l12;sal_memset(&l12,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(l1,l7,l33,l10,0,&l12));
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l12,TRUE,&l141,l142,
bucket_index));}if(l141){l19(l1,l7,l14,0,l23,0,0);l127 = 
_soc_alpm_find_in_bkt(l1,l23,*bucket_index,l137,l14,l138,&l120,l33);if(
SOC_SUCCESS(l127)){*l15 = l120;}}else{l127 = SOC_E_NOT_FOUND;}}return l127;}
static int l149(int l1,void*l7,void*l138,void*l150,soc_mem_t l23,int l120){
defip_aux_scratch_entry_t l12;int l148,l33,l27;int bucket_index;uint32 l10,
l137;int l127 = SOC_E_NONE;int l141 = 0,l132 = 0;int l142;l33 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f);if(l33){if(!(l33 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l7,&l148,&l27));if(l27 == 
SOC_VRF_MAX(l1)+1){l10 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l137);}else{
l10 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l137);}if(!soc_alpm_mode_get(l1)){
l10 = 2;}if(l148!= SOC_L3_VRF_OVERRIDE){sal_memset(&l12,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(l1,l7,l33,l10,0,&l12));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l23,MEM_BLOCK_ANY,l120,l138));if(
SOC_URPF_STATUS_GET(l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,l23,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l120),l150));if(l127!= SOC_E_NONE){
return SOC_E_FAIL;}}l132 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l12,
IP_LENGTHf);soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,REPLACE_LENf,
l132);SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&
l141,&l142,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l132 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf);l132+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l132);
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l141,&
l142,&bucket_index));}}return l127;}int alpm_mem_prefix_array_cb(trie_node_t*
node,void*l151){alpm_mem_prefix_array_t*l152 = (alpm_mem_prefix_array_t*)l151
;if(node->type == PAYLOAD){l152->prefix[l152->count] = (payload_t*)node;l152
->count++;}return SOC_E_NONE;}int alpm_delete_node_cb(trie_node_t*node,void*
l151){if(node!= NULL){sal_free(node);}return SOC_E_NONE;}static int l153(int
l1,int l154,int l33,int l155){int l127,l132,index;defip_aux_table_entry_t
entry;index = l154>>(l33?0:1);l127 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l127);if(l33){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l155);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l155);l132 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l154&1)
{soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l155);l132 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l155);l132 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l127 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l127);if(SOC_URPF_STATUS_GET(l1)){l132++;if(l33){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l155);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l155);}else{if
(l154&1){soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l155)
;}else{soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l155);}
}soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l132);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l132);index+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l127 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}
return l127;}static int l156(int l1,int l157,void*entry,void*l158,int l159){
uint32 l132,l135,l33,l10,l160 = 0;soc_mem_t l23 = L3_DEFIPm;soc_mem_t l161 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l162;int l127 = SOC_E_NONE,l119,l163,l27,
l164;SOC_IF_ERROR_RETURN(soc_mem_read(l1,l161,MEM_BLOCK_ANY,l157,l158));l132 = 
soc_mem_field32_get(l1,l23,entry,VRF_ID_0f);soc_mem_field32_set(l1,l161,l158,
VRF0f,l132);l132 = soc_mem_field32_get(l1,l23,entry,VRF_ID_1f);
soc_mem_field32_set(l1,l161,l158,VRF1f,l132);l132 = soc_mem_field32_get(l1,
l23,entry,MODE0f);soc_mem_field32_set(l1,l161,l158,MODE0f,l132);l132 = 
soc_mem_field32_get(l1,l23,entry,MODE1f);soc_mem_field32_set(l1,l161,l158,
MODE1f,l132);l33 = l132;l132 = soc_mem_field32_get(l1,l23,entry,VALID0f);
soc_mem_field32_set(l1,l161,l158,VALID0f,l132);l132 = soc_mem_field32_get(l1,
l23,entry,VALID1f);soc_mem_field32_set(l1,l161,l158,VALID1f,l132);l132 = 
soc_mem_field32_get(l1,l23,entry,IP_ADDR_MASK0f);if((l127 = _ipmask2pfx(l132,
&l119))<0){return l127;}l135 = soc_mem_field32_get(l1,l23,entry,
IP_ADDR_MASK1f);if((l127 = _ipmask2pfx(l135,&l163))<0){return l127;}if(l33){
soc_mem_field32_set(l1,l161,l158,IP_LENGTH0f,l119+l163);soc_mem_field32_set(
l1,l161,l158,IP_LENGTH1f,l119+l163);}else{soc_mem_field32_set(l1,l161,l158,
IP_LENGTH0f,l119);soc_mem_field32_set(l1,l161,l158,IP_LENGTH1f,l163);}l132 = 
soc_mem_field32_get(l1,l23,entry,IP_ADDR0f);soc_mem_field32_set(l1,l161,l158,
IP_ADDR0f,l132);l132 = soc_mem_field32_get(l1,l23,entry,IP_ADDR1f);
soc_mem_field32_set(l1,l161,l158,IP_ADDR1f,l132);l132 = soc_mem_field32_get(
l1,l23,entry,ENTRY_TYPE0f);soc_mem_field32_set(l1,l161,l158,ENTRY_TYPE0f,l132
);l132 = soc_mem_field32_get(l1,l23,entry,ENTRY_TYPE1f);soc_mem_field32_set(
l1,l161,l158,ENTRY_TYPE1f,l132);if(!l33){sal_memcpy(&l162,entry,sizeof(l162))
;l127 = soc_alpm_lpm_vrf_get(l1,(void*)&l162,&l27,&l119);SOC_IF_ERROR_RETURN(
l127);SOC_IF_ERROR_RETURN(soc_alpm_lpm_ip4entry1_to_0(l1,&l162,&l162,
PRESERVE_HIT));l127 = soc_alpm_lpm_vrf_get(l1,(void*)&l162,&l164,&l119);
SOC_IF_ERROR_RETURN(l127);}else{l127 = soc_alpm_lpm_vrf_get(l1,entry,&l27,&
l119);}if(SOC_URPF_STATUS_GET(l1)){if(l159>= (soc_mem_index_count(l1,
L3_DEFIPm)>>1)){l160 = 1;}}switch(l27){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l161,l158,VALID0f,0);l10 = 0;break;case
SOC_L3_VRF_GLOBAL:l10 = l160?1:0;break;default:l10 = l160?3:2;break;}
soc_mem_field32_set(l1,l161,l158,DB_TYPE0f,l10);if(!l33){switch(l164){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l1,l161,l158,VALID1f,0);l10 = 0;break
;case SOC_L3_VRF_GLOBAL:l10 = l160?1:0;break;default:l10 = l160?3:2;break;}
soc_mem_field32_set(l1,l161,l158,DB_TYPE1f,l10);}else{if(l27 == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l1,l161,l158,VALID1f,0);}
soc_mem_field32_set(l1,l161,l158,DB_TYPE1f,l10);}if(l160){l132 = 
soc_mem_field32_get(l1,l23,entry,ALG_BKT_PTR0f);if(l132){l132+= 
SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l23,entry,ALG_BKT_PTR0f,l132
);}if(!l33){l132 = soc_mem_field32_get(l1,l23,entry,ALG_BKT_PTR1f);if(l132){
l132+= SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l23,entry,
ALG_BKT_PTR1f,l132);}}}return SOC_E_NONE;}static int l165(int l1,int l166,int
index,int l167,void*entry){defip_aux_table_entry_t l158;l167 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l167,1);SOC_IF_ERROR_RETURN(l156(l1,l167,
entry,(void*)&l158,index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l1,
MEM_BLOCK_ANY,index,entry));index = soc_alpm_physical_idx(l1,L3_DEFIPm,index,
1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,index,&l158
));return SOC_E_NONE;}int _soc_alpm_insert_in_bkt(int l1,soc_mem_t l23,int
bucket_index,int l137,void*l138,uint32*l14,int*l120,int l33){int l127;l127 = 
soc_mem_alpm_insert(l1,l23,bucket_index,MEM_BLOCK_ANY,l137,l138,l14,l120);if(
l127 == SOC_E_FULL){if(SOC_ALPM_V6_SCALE_CHECK(l1,l33)){return
soc_mem_alpm_insert(l1,l23,bucket_index+1,MEM_BLOCK_ANY,l137,l138,l14,l120);}
}return l127;}int _soc_alpm_mem_index(int l1,soc_mem_t l23,int bucket_index,
int l168,uint32 l137,int*l169){int l170,l171 = 0;int l172[4] = {0};int l173 = 
0;int l174 = 0;int l175;int l33 = 0;int l176 = 6;switch(l23){case
L3_DEFIP_ALPM_IPV6_64m:l176 = 4;l33 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:
l176 = 2;l33 = 1;break;default:break;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l33)){if(
l168>= ALPM_RAW_BKT_COUNT*l176){bucket_index++;l168-= ALPM_RAW_BKT_COUNT*l176
;}}l175 = (4)-_shr_popcount(l137&((1<<(4))-1));if(bucket_index>= (1<<16)||
l168>= l175*l176){return SOC_E_FULL;}l174 = l168%l176;for(l170 = 0;l170<(4);
l170++){if((1<<l170)&l137){continue;}l172[l171++] = l170;}l173 = l172[l168/
l176];*l169 = (l174<<16)|(bucket_index<<2)|(l173);return SOC_E_NONE;}int l177
(int l20,int l33){if(SOC_ALPM_V6_SCALE_CHECK(l20,l33)){return
ALPM_RAW_BKT_COUNT_DW;}else{return ALPM_RAW_BKT_COUNT;}}int l178(soc_mem_t l23
,int index){uint32 l179 = 0x7;if(l23 == L3_DEFIP_ALPM_IPV6_128m){l179 = 0x3;}
return((uint32)index>>16)&l179;}int l180(int l20,int l33,int index){return
index%l177(l20,l33);}void _soc_alpm_raw_mem_read(int l20,soc_mem_t l23,void*
l181,int index,void*entry){int l9 = 1;int l182;int l183;
defip_alpm_raw_entry_t*l184;soc_mem_info_t l185;soc_field_info_t l186;int l187
= soc_mem_entry_bits(l20,l23)-1;if(l23 == L3_DEFIP_ALPM_IPV4m){l9 = 0;}l182 = 
l180(l20,l9,index);l183 = l178(l23,index);l184 = &((defip_alpm_raw_entry_t*)
l181)[l182];l185.flags = 0;l185.bytes = sizeof(defip_alpm_raw_entry_t);l186.
flags = SOCF_LE;l186.bp = l187*l183;l186.len = l187;(void)
soc_meminfo_fieldinfo_field_get((void*)l184,&l185,&l186,entry);}void
_soc_alpm_raw_mem_write(int l20,soc_mem_t l23,void*l181,int index,void*entry)
{int l9 = 1;int l182;int l183;defip_alpm_raw_entry_t*l184;soc_mem_info_t l185
;soc_field_info_t l186;int l187 = soc_mem_entry_bits(l20,l23)-1;if(l23 == 
L3_DEFIP_ALPM_IPV4m){l9 = 0;}(void)soc_mem_cache_invalidate(l20,l23,
MEM_BLOCK_ANY,index);l182 = l180(l20,l9,index);l183 = l178(l23,index);l184 = 
&((defip_alpm_raw_entry_t*)l181)[l182];l185.flags = 0;l185.bytes = sizeof(
defip_alpm_raw_entry_t);l186.bp = l187*l183;l186.len = l187;l186.flags = 
SOCF_LE;(void)soc_meminfo_fieldinfo_field_set((void*)l184,&l185,&l186,entry);
}int l188(int l1,soc_mem_t l23,int index){return SOC_ALPM_BKT_ENTRY_TO_IDX(
index%(1<<16));}int _soc_alpm_raw_bucket_read(int l1,soc_mem_t l23,int
bucket_index,void*l184,void*l189){int l170,l33 = 1;int l182,l190;
defip_alpm_raw_entry_t*l191 = l184;defip_alpm_raw_entry_t*l192 = l189;if(l23
== L3_DEFIP_ALPM_IPV4m){l33 = 0;}l182 = SOC_ALPM_BKT_IDX_TO_ENTRY(
bucket_index);for(l170 = 0;l170<l177(l1,l33);l170++){SOC_IF_ERROR_RETURN(
soc_mem_read(l1,L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l182+l170,&l191[l170]));if(
SOC_URPF_STATUS_GET(l1)){l190 = _soc_alpm_rpf_entry(l1,l182+l170);
SOC_IF_ERROR_RETURN(soc_mem_read(l1,L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l190,&
l192[l170]));}}return SOC_E_NONE;}int _soc_alpm_raw_bucket_write(int l1,
soc_mem_t l23,int bucket_index,uint32 l137,void*l184,void*l189,int l193){int
l170 = 0,l194,l33 = 1;int l182,l190,l195;defip_alpm_raw_entry_t*l191 = l184;
defip_alpm_raw_entry_t*l192 = l189;int l196 = 6;switch(l23){case
L3_DEFIP_ALPM_IPV4m:l196 = 6;l33 = 0;break;case L3_DEFIP_ALPM_IPV6_64m:l196 = 
4;l33 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:l196 = 2;l33 = 1;break;default:
break;}l182 = SOC_ALPM_BKT_IDX_TO_ENTRY(bucket_index);if(l193 == -1){l195 = 
l177(l1,l33);}else{l195 = (l193/l196)+1;}for(l194 = 0;l194<l177(l1,l33);l194
++){if((1<<(l194%(4)))&l137){continue;}SOC_IF_ERROR_RETURN(soc_mem_write(l1,
L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l182+l194,&l191[l194]));
_soc_trident2_alpm_bkt_view_set(l1,l182+l194,l23);if(SOC_URPF_STATUS_GET(l1))
{l190 = _soc_alpm_rpf_entry(l1,l182+l194);_soc_trident2_alpm_bkt_view_set(l1,
l190,l23);SOC_IF_ERROR_RETURN(soc_mem_write(l1,L3_DEFIP_ALPM_RAWm,
MEM_BLOCK_ANY,l190,&l192[l194]));}if(++l170 == l195){break;}}return SOC_E_NONE
;}void _soc_alpm_raw_parity_set(int l1,soc_mem_t l23,void*l138){int l170,l197
,l198 = 0;uint32*entry = l138;l197 = soc_mem_entry_words(l1,l23);for(l170 = 0
;l170<l197;l170++){l198+= _shr_popcount(entry[l170]);}if(l198&0x1){
soc_mem_field32_set(l1,l23,l138,EVEN_PARITYf,1);}}static int l199(int l1,void
*l7,soc_mem_t l23,void*l138,void*l150,int*l15,int bucket_index,int l142){
alpm_pivot_t*l145,*l200,*l201;defip_aux_scratch_entry_t l12;uint32 l14[
SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l202,l140;uint32 l203[5];int l33,
l27,l148;int l120;int l127 = SOC_E_NONE,l204;uint32 l10,l137,l155 = 0;int l141
=0;int l154;int l205 = 0;trie_t*trie,*l206;trie_node_t*l207,*l208 = NULL,*
l144 = NULL;payload_t*l209,*l210,*l211;defip_entry_t lpm_entry;
alpm_bucket_handle_t*l212;int l170,l213 = -1,l25 = 0;alpm_mem_prefix_array_t
l152;defip_alpm_ipv4_entry_t l214,l215;defip_alpm_ipv6_64_entry_t l216,l217;
void*l218,*l219;int*l123 = NULL;trie_t*l143 = NULL;int l220;
defip_alpm_raw_entry_t*l221 = NULL;defip_alpm_raw_entry_t*l184;
defip_alpm_raw_entry_t*l189;defip_alpm_raw_entry_t*l222;
defip_alpm_raw_entry_t*l223;l33 = soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f)
;if(l33){if(!(l33 = soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l7,&l148,&l27));if
(l27 == SOC_VRF_MAX(l1)+1){l10 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l137);
}else{l10 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l137);}l23 = (l33)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l218 = ((l33)?((uint32*)&(l216)):(
(uint32*)&(l214)));l219 = ((l33)?((uint32*)&(l217)):((uint32*)&(l215)));l127 = 
l133(l1,l7,prefix,&l140,&l25);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_insert: prefix create failed\n")));return l127;}
sal_memset(&l12,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(
l1,l7,l33,l10,0,&l12));if(bucket_index == 0){l127 = l139(l1,prefix,l140,l33,
l27,&l141,&l142,&bucket_index);SOC_IF_ERROR_RETURN(l127);if(l141 == 0){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: "
"Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l127 = 
_soc_alpm_insert_in_bkt(l1,l23,bucket_index,l137,l138,l14,&l120,l33);if(l127
== SOC_E_NONE){*l15 = l120;if(SOC_URPF_STATUS_GET(l1)){l204 = soc_mem_write(
l1,l23,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l120),l150);if(SOC_FAILURE(l204))
{return l204;}}}if(l127 == SOC_E_FULL){l205 = 1;}l145 = ALPM_TCAM_PIVOT(l1,
l142);trie = PIVOT_BUCKET_TRIE(l145);l201 = l145;l209 = sal_alloc(sizeof(
payload_t),"Payload for Key");if(l209 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Unable to allocate memory for "
"trie node \n")));return SOC_E_MEMORY;}l210 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l210 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l209);return SOC_E_MEMORY;}sal_memset(l209,0,
sizeof(*l209));sal_memset(l210,0,sizeof(*l210));l209->key[0] = prefix[0];l209
->key[1] = prefix[1];l209->key[2] = prefix[2];l209->key[3] = prefix[3];l209->
key[4] = prefix[4];l209->len = l140;l209->index = l120;sal_memcpy(l210,l209,
sizeof(*l209));l210->bkt_ptr = l209;l127 = trie_insert(trie,prefix,NULL,l140,
(trie_node_t*)l209);if(SOC_FAILURE(l127)){goto l224;}if(l33){l206 = 
VRF_PREFIX_TRIE_IPV6(l1,l27);}else{l206 = VRF_PREFIX_TRIE_IPV4(l1,l27);}if(!
l25){l127 = trie_insert(l206,prefix,NULL,l140,(trie_node_t*)l210);}else{l144 = 
NULL;l127 = trie_find_lpm(l206,0,0,&l144);l211 = (payload_t*)l144;if(
SOC_SUCCESS(l127)){l211->bkt_ptr = l209;}}l202 = l140;if(SOC_FAILURE(l127)){
goto l225;}if(l205){l127 = alpm_bucket_assign(l1,&bucket_index,l33);if(
SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate""new bucket for split\n")));
bucket_index = -1;goto l226;}l127 = trie_split(trie,l33?_MAX_KEY_LEN_144_:
_MAX_KEY_LEN_48_,FALSE,l203,&l140,&l207,NULL,FALSE);if(SOC_FAILURE(l127)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not split bucket""for prefix 0x%08x 0x%08x\n"),
prefix[0],prefix[1]));goto l226;}l144 = NULL;l127 = trie_find_lpm(l206,l203,
l140,&l144);l211 = (payload_t*)l144;if(SOC_FAILURE(l127)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l203[0],l203[1],l203[
2],l203[3],l203[4],l140));goto l226;}if(l211->bkt_ptr){if(l211->bkt_ptr == 
l209){sal_memcpy(l218,l138,l33?sizeof(defip_alpm_ipv6_64_entry_t):sizeof(
defip_alpm_ipv4_entry_t));}else{l127 = soc_mem_read(l1,l23,MEM_BLOCK_ANY,((
payload_t*)l211->bkt_ptr)->index,l218);}if(SOC_FAILURE(l127)){goto l226;}l127
= l26(l1,l218,l23,l33,l148,bucket_index,0,&lpm_entry);if(SOC_FAILURE(l127)){
goto l226;}l155 = ((payload_t*)(l211->bkt_ptr))->len;}else{l127 = 
soc_mem_read(l1,L3_DEFIPm,MEM_BLOCK_ANY,soc_alpm_logical_idx(l1,L3_DEFIPm,
l142>>1,1),&lpm_entry);if((!l33)&&(l142&1)){l127 = 
soc_alpm_lpm_ip4entry1_to_0(l1,&lpm_entry,&lpm_entry,0);}}l212 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l212 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l127 = SOC_E_MEMORY;goto l226;}sal_memset(l212,0,sizeof(*l212));l145 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l145 == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l127 = SOC_E_MEMORY;goto l226;}sal_memset(l145,0,sizeof(*l145));
PIVOT_BUCKET_HANDLE(l145) = l212;if(l33){l127 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l145));}else{l127 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l145));}PIVOT_BUCKET_TRIE(l145)->trie = l207;
PIVOT_BUCKET_INDEX(l145) = bucket_index;PIVOT_BUCKET_VRF(l145) = l27;
PIVOT_BUCKET_IPV6(l145) = l33;PIVOT_BUCKET_DEF(l145) = FALSE;l145->key[0] = 
l203[0];l145->key[1] = l203[1];l145->len = l140;l145->key[2] = l203[2];l145->
key[3] = l203[3];l145->key[4] = l203[4];if(l33){l143 = VRF_PIVOT_TRIE_IPV6(l1
,l27);}else{l143 = VRF_PIVOT_TRIE_IPV4(l1,l27);}do{if(!(l33)){l203[0] = (((32
-l140) == 32)?0:(l203[1])<<(32-l140));l203[1] = 0;}else{int l227 = 64-l140;
int l228;if(l227<32){l228 = l203[3]<<l227;l228|= (((32-l227) == 32)?0:(l203[4
])>>(32-l227));l203[0] = l203[4]<<l227;l203[1] = l228;l203[2] = l203[3] = 
l203[4] = 0;}else{l203[1] = (((l227-32) == 32)?0:(l203[4])<<(l227-32));l203[0
] = l203[2] = l203[3] = l203[4] = 0;}}}while(0);l29(l1,l203,l140,l27,l33,&
lpm_entry,0,0);soc_L3_DEFIPm_field32_set(l1,&lpm_entry,ALG_BKT_PTR0f,
bucket_index);sal_memset(&l152,0,sizeof(l152));l127 = trie_traverse(
PIVOT_BUCKET_TRIE(l145),alpm_mem_prefix_array_cb,&l152,_TRIE_INORDER_TRAVERSE
);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l226;}l123 = sal_alloc(sizeof(*l123)*l152.count,
"Temp storage for location of prefixes in new bucket");if(l123 == NULL){l127 = 
SOC_E_MEMORY;goto l226;}sal_memset(l123,-1,sizeof(*l123)*l152.count);l220 = 
sizeof(defip_alpm_raw_entry_t)*ALPM_RAW_BKT_COUNT_DW;l221 = sal_alloc(4*l220,
"Raw memory buffer");if(l221 == NULL){l127 = SOC_E_MEMORY;goto l226;}
sal_memset(l221,0,4*l220);l184 = (defip_alpm_raw_entry_t*)l221;l189 = (
defip_alpm_raw_entry_t*)((uint8*)l184+l220);l222 = (defip_alpm_raw_entry_t*)(
(uint8*)l189+l220);l223 = (defip_alpm_raw_entry_t*)((uint8*)l222+l220);l127 = 
_soc_alpm_raw_bucket_read(l1,l23,PIVOT_BUCKET_INDEX(l201),(void*)l184,(void*)
l189);if(SOC_FAILURE(l127)){goto l226;}for(l170 = 0;l170<l152.count;l170++){
payload_t*l119 = l152.prefix[l170];if(l119->index>0){_soc_alpm_raw_mem_read(
l1,l23,l184,l119->index,l218);_soc_alpm_raw_mem_write(l1,l23,l184,l119->index
,soc_mem_entry_null(l1,l23));if(SOC_URPF_STATUS_GET(l1)){
_soc_alpm_raw_mem_read(l1,l23,l189,_soc_alpm_rpf_entry(l1,l119->index),l219);
_soc_alpm_raw_mem_write(l1,l23,l189,_soc_alpm_rpf_entry(l1,l119->index),
soc_mem_entry_null(l1,l23));}l127 = _soc_alpm_mem_index(l1,l23,bucket_index,
l170,l137,&l120);if(SOC_SUCCESS(l127)){_soc_alpm_raw_mem_write(l1,l23,l222,
l120,l218);if(SOC_URPF_STATUS_GET(l1)){_soc_alpm_raw_mem_write(l1,l23,l223,
_soc_alpm_rpf_entry(l1,l120),l219);}}}else{l127 = _soc_alpm_mem_index(l1,l23,
bucket_index,l170,l137,&l120);if(SOC_SUCCESS(l127)){l213 = l170;*l15 = l120;
_soc_alpm_raw_parity_set(l1,l23,l138);_soc_alpm_raw_mem_write(l1,l23,l222,
l120,l138);if(SOC_URPF_STATUS_GET(l1)){_soc_alpm_raw_parity_set(l1,l23,l150);
_soc_alpm_raw_mem_write(l1,l23,l223,_soc_alpm_rpf_entry(l1,l120),l150);}}}
l123[l170] = l120;}l127 = _soc_alpm_raw_bucket_write(l1,l23,bucket_index,l137
,(void*)l222,(void*)l223,l152.count);if(SOC_FAILURE(l127)){goto l229;}l127 = 
l4(l1,&lpm_entry,&l154);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Unable to add new""pivot to tcam\n")));if(
l127 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l27,l33);}goto l229;}l154 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l154,l33);l127 = l153(l1,l154,l33,l155);if
(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),l154));goto l230
;}l127 = trie_insert(l143,l145->key,NULL,l145->len,(trie_node_t*)l145);if(
SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));goto l230;}ALPM_TCAM_PIVOT(l1,l154<<(
l33?1:0)) = l145;PIVOT_TCAM_INDEX(l145) = l154<<(l33?1:0);VRF_PIVOT_REF_INC(
l1,l27,l33);for(l170 = 0;l170<l152.count;l170++){l152.prefix[l170]->index = 
l123[l170];}sal_free(l123);l127 = _soc_alpm_raw_bucket_write(l1,l23,
PIVOT_BUCKET_INDEX(l201),l137,(void*)l184,(void*)l189,-1);if(SOC_FAILURE(l127
)){goto l230;}if(l213 == -1){l127 = _soc_alpm_insert_in_bkt(l1,l23,
PIVOT_BUCKET_INDEX(l201),l137,l138,l14,&l120,l33);if(SOC_FAILURE(l127)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n")));
goto l230;}if(SOC_URPF_STATUS_GET(l1)){l127 = soc_mem_write(l1,l23,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l120),l150);}*l15 = l120;l209->index = 
l120;}sal_free(l221);PIVOT_BUCKET_ENT_CNT_UPDATE(l145);VRF_BUCKET_SPLIT_INC(
l1,l27,l33);}VRF_TRIE_ROUTES_INC(l1,l27,l33);if(l25){sal_free(l210);}
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l141,&
l142,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE
,&l12,FALSE,&l141,&l142,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l140 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf);l140+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l140);
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l141,&
l142,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE
,&l12,FALSE,&l141,&l142,&bucket_index));}PIVOT_BUCKET_ENT_CNT_UPDATE(l201);
return l127;l230:l204 = l6(l1,&lpm_entry);if(SOC_FAILURE(l204)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failure to free new prefix"
"at %d\n"),soc_alpm_logical_idx(l1,L3_DEFIPm,l154,l33)));}if(ALPM_TCAM_PIVOT(
l1,l154<<(l33?1:0))!= NULL){l204 = trie_delete(l143,l145->key,l145->len,&l208
);if(SOC_FAILURE(l204)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l1,l154<<(l33?1:0)) = NULL;VRF_PIVOT_REF_DEC(l1,l27,l33);l229
:l200 = l201;for(l170 = 0;l170<l152.count;l170++){payload_t*prefix = l152.
prefix[l170];if(l123[l170]!= -1){if(l33){sal_memset(l218,0,sizeof(
defip_alpm_ipv6_64_entry_t));}else{sal_memset(l218,0,sizeof(
defip_alpm_ipv4_entry_t));}l204 = soc_mem_write(l1,l23,MEM_BLOCK_ANY,l123[
l170],l218);_soc_trident2_alpm_bkt_view_set(l1,l123[l170],INVALIDm);if(
SOC_FAILURE(l204)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l1)){l204 = soc_mem_write(l1,l23,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l123[l170]),l218);_soc_trident2_alpm_bkt_view_set(l1,
_soc_alpm_rpf_entry(l1,l123[l170]),INVALIDm);if(SOC_FAILURE(l204)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l208 = NULL;l204 = trie_delete(
PIVOT_BUCKET_TRIE(l145),prefix->key,prefix->len,&l208);l209 = (payload_t*)
l208;if(SOC_FAILURE(l204)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(prefix
->index>0){l204 = trie_insert(PIVOT_BUCKET_TRIE(l200),prefix->key,NULL,prefix
->len,(trie_node_t*)l209);if(SOC_FAILURE(l204)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l209!= NULL){sal_free(l209);}}}if(l213
== -1){l208 = NULL;l204 = trie_delete(PIVOT_BUCKET_TRIE(l200),prefix,l202,&
l208);l209 = (payload_t*)l208;if(SOC_FAILURE(l204)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l209!= 
NULL){sal_free(l209);}}l204 = alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l145)
,l33);if(SOC_FAILURE(l204)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: new bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(l145
)));}trie_destroy(PIVOT_BUCKET_TRIE(l145));sal_free(l212);sal_free(l145);
sal_free(l123);sal_free(l221);l208 = NULL;l204 = trie_delete(l206,prefix,l202
,&l208);l210 = (payload_t*)l208;if(SOC_FAILURE(l204)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l210){sal_free(
l210);}return l127;l226:if(l123!= NULL){sal_free(l123);}if(l221!= NULL){
sal_free(l221);}l208 = NULL;(void)trie_delete(l206,prefix,l202,&l208);l210 = 
(payload_t*)l208;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l33);}l225:l208 = NULL;(void)trie_delete(trie,prefix,l202,&l208)
;l209 = (payload_t*)l208;l224:if(l209!= NULL){sal_free(l209);}if(l210!= NULL)
{sal_free(l210);}return l127;}static int l29(int l20,uint32*key,int len,int
l27,int l9,defip_entry_t*lpm_entry,int l30,int l31){uint32 l179;if(l31){
sal_memset(lpm_entry,0,sizeof(defip_entry_t));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l75),(l27&SOC_VRF_MAX(l20)));if(l27 == (SOC_VRF_MAX(l20)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l77),(0));}else{soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO
(l20,L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l77),(SOC_VRF_MAX(l20)));}if(l9){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l59),(key[0]));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l20,L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l60),(key[1]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l63),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,
L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l64),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l76),(l27&SOC_VRF_MAX(l20)));if(l27 == (SOC_VRF_MAX(l20)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l78),(0));}else{soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO
(l20,L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l78),(SOC_VRF_MAX(l20)));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l74),(1));if(len>= 32){l179 = 0xffffffff;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l62),(l179));l179 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32))
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry)
,(l95[(l20)]->l61),(l179));}else{l179 = ~(0xffffffff>>len);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l62),(l179));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l20,L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l61),(0));}}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l59),(key[0]));assert(len<= 32);l179 = (len == 32)?0xffffffff:~(
0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,
L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l61),(l179));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l95[(l20)]->l73),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,
L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l65),((1<<soc_mem_field_length(l20,
L3_DEFIPm,MODE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l20,L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l66),((1<<soc_mem_field_length(l20,
L3_DEFIPm,MODE_MASK1f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l20,L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l91),((1<<soc_mem_field_length(l20,
L3_DEFIPm,ENTRY_TYPE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l92),((1<<
soc_mem_field_length(l20,L3_DEFIPm,ENTRY_TYPE_MASK1f))-1));return(SOC_E_NONE)
;}int _soc_alpm_delete_in_bkt(int l1,soc_mem_t l23,int l231,int l137,void*
l232,uint32*l14,int*l120,int l33){int l127;l127 = soc_mem_alpm_delete(l1,l23,
l231,MEM_BLOCK_ALL,l137,l232,l14,l120);if(SOC_SUCCESS(l127)){return l127;}if(
SOC_ALPM_V6_SCALE_CHECK(l1,l33)){return soc_mem_alpm_delete(l1,l23,l231+1,
MEM_BLOCK_ALL,l137,l232,l14,l120);}return l127;}static int l233(int l1,void*
l7,int bucket_index,int l142,int l120){alpm_pivot_t*l145;
defip_alpm_ipv4_entry_t l214,l215,l234;defip_alpm_ipv6_64_entry_t l216,l217,
l235;defip_aux_scratch_entry_t l12;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];
soc_mem_t l23;void*l218,*l232,*l219 = NULL;int l148;int l9;int l127 = 
SOC_E_NONE,l204;uint32 l236[5],prefix[5];int l33,l27;uint32 l140;int l231;
uint32 l10,l137;int l141,l25 = 0;trie_t*trie,*l206;uint32 l237;defip_entry_t
lpm_entry,*l238;payload_t*l209 = NULL,*l239 = NULL,*l211 = NULL;trie_node_t*
l208 = NULL,*l144 = NULL;trie_t*l143 = NULL;l9 = l33 = soc_mem_field32_get(l1
,L3_DEFIPm,l7,MODE0f);if(l33){if(!(l33 = soc_mem_field32_get(l1,L3_DEFIPm,l7,
MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,
l7,&l148,&l27));if(l148!= SOC_L3_VRF_OVERRIDE){if(l27 == SOC_VRF_MAX(l1)+1){
l10 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l137);}else{l10 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l137);}l127 = l133(l1,l7,prefix,&l140,&l25);
if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: prefix create failed\n")));return l127;}if(!
soc_alpm_mode_get(l1)){if(l148!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1
,l27,l33)>1){if(l25){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot delete default ""route if other routes are present in "
"this mode"),l27));return SOC_E_PARAM;}}}l10 = 2;}l23 = (l33)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l218 = ((l33)?((uint32*)&(l216)):(
(uint32*)&(l214)));l232 = ((l33)?((uint32*)&(l235)):((uint32*)&(l234)));
SOC_ALPM_LPM_LOCK(l1);if(bucket_index == 0){l127 = l146(l1,l7,l23,l218,&l142,
&bucket_index,&l120,TRUE);}else{l127 = l19(l1,l7,l218,0,l23,0,0);}sal_memcpy(
l232,l218,l33?sizeof(l216):sizeof(l214));if(SOC_FAILURE(l127)){
SOC_ALPM_LPM_UNLOCK(l1);LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to find prefix for delete\n")));return l127;}l231 = 
bucket_index;l145 = ALPM_TCAM_PIVOT(l1,l142);trie = PIVOT_BUCKET_TRIE(l145);
l127 = trie_delete(trie,prefix,l140,&l208);l209 = (payload_t*)l208;if(l127!= 
SOC_E_NONE){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l127;}if(l33){l206 = VRF_PREFIX_TRIE_IPV6(l1,
l27);}else{l206 = VRF_PREFIX_TRIE_IPV4(l1,l27);}if(l33){l143 = 
VRF_PIVOT_TRIE_IPV6(l1,l27);}else{l143 = VRF_PIVOT_TRIE_IPV4(l1,l27);}if(!l25
){l127 = trie_delete(l206,prefix,l140,&l208);l239 = (payload_t*)l208;if(
SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l240;}l144 = NULL;l127 = trie_find_lpm(l206,prefix,l140,&
l144);l211 = (payload_t*)l144;if(SOC_SUCCESS(l127)){payload_t*l241 = (
payload_t*)(l211->bkt_ptr);if(l241!= NULL){l237 = l241->len;}else{l237 = 0;}}
else{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l242;}
sal_memcpy(l236,prefix,sizeof(prefix));do{if(!(l33)){l236[0] = (((32-l140) == 
32)?0:(l236[1])<<(32-l140));l236[1] = 0;}else{int l227 = 64-l140;int l228;if(
l227<32){l228 = l236[3]<<l227;l228|= (((32-l227) == 32)?0:(l236[4])>>(32-l227
));l236[0] = l236[4]<<l227;l236[1] = l228;l236[2] = l236[3] = l236[4] = 0;}
else{l236[1] = (((l227-32) == 32)?0:(l236[4])<<(l227-32));l236[0] = l236[2] = 
l236[3] = l236[4] = 0;}}}while(0);l127 = l29(l1,l236,l237,l27,l9,&lpm_entry,0
,1);l204 = l146(l1,&lpm_entry,l23,l218,&l142,&bucket_index,&l120,TRUE);(void)
l26(l1,l218,l23,l9,l148,bucket_index,0,&lpm_entry);(void)l29(l1,l236,l140,l27
,l9,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(l127)){l219 = 
((l33)?((uint32*)&(l217)):((uint32*)&(l215)));l204 = soc_mem_read(l1,l23,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l120),l219);}}if((l237 == 0)&&
SOC_FAILURE(l204)){l238 = l33?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l27);sal_memcpy(&lpm_entry,l238,sizeof(
lpm_entry));l127 = l29(l1,l236,l140,l27,l9,&lpm_entry,0,1);}if(SOC_FAILURE(
l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l242;}l238 = 
&lpm_entry;}else{l144 = NULL;l127 = trie_find_lpm(l206,prefix,l140,&l144);
l211 = (payload_t*)l144;if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),l27));goto l240;}l211->bkt_ptr = NULL;l237 = 
0;l238 = l33?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27):VRF_TRIE_DEFAULT_ROUTE_IPV4(
l1,l27);}l127 = l8(l1,l238,l33,l10,l237,&l12);if(SOC_FAILURE(l127)){goto l242
;}l127 = _soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l141,&l142,&
bucket_index);if(SOC_FAILURE(l127)){goto l242;}if(SOC_URPF_STATUS_GET(l1)){
uint32 l132;if(l219!= NULL){l132 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf);l132++;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l132);l132 = soc_mem_field32_get(l1,l23,
l219,SRC_DISCARDf);soc_mem_field32_set(l1,l23,&l12,SRC_DISCARDf,l132);l132 = 
soc_mem_field32_get(l1,l23,l219,DEFAULTROUTEf);soc_mem_field32_set(l1,l23,&
l12,DEFAULTROUTEf,l132);l127 = _soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE
,&l141,&l142,&bucket_index);}if(SOC_FAILURE(l127)){goto l242;}}sal_free(l209)
;if(!l25){sal_free(l239);}PIVOT_BUCKET_ENT_CNT_UPDATE(l145);if((l145->len!= 0
)&&(trie->trie == NULL)){uint32 l243[5];sal_memcpy(l243,l145->key,sizeof(l243
));do{if(!(l9)){l243[0] = (((32-l145->len) == 32)?0:(l243[1])<<(32-l145->len)
);l243[1] = 0;}else{int l227 = 64-l145->len;int l228;if(l227<32){l228 = l243[
3]<<l227;l228|= (((32-l227) == 32)?0:(l243[4])>>(32-l227));l243[0] = l243[4]
<<l227;l243[1] = l228;l243[2] = l243[3] = l243[4] = 0;}else{l243[1] = (((l227
-32) == 32)?0:(l243[4])<<(l227-32));l243[0] = l243[2] = l243[3] = l243[4] = 0
;}}}while(0);l29(l1,l243,l145->len,l27,l9,&lpm_entry,0,1);l127 = l6(l1,&
lpm_entry);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to ""delete pivot 0x%08x 0x%08x \n"),l145->key[0],
l145->key[1]));}}l127 = _soc_alpm_delete_in_bkt(l1,l23,l231,l137,l232,l14,&
l120,l33);if(!SOC_SUCCESS(l127)){SOC_ALPM_LPM_UNLOCK(l1);l127 = SOC_E_FAIL;
return l127;}if(SOC_URPF_STATUS_GET(l1)){l127 = soc_mem_alpm_delete(l1,l23,
SOC_ALPM_RPF_BKT_IDX(l1,l231),MEM_BLOCK_ALL,l137,l232,l14,&l141);if(!
SOC_SUCCESS(l127)){SOC_ALPM_LPM_UNLOCK(l1);l127 = SOC_E_FAIL;return l127;}}if
((l145->len!= 0)&&(trie->trie == NULL)){l127 = alpm_bucket_release(l1,
PIVOT_BUCKET_INDEX(l145),l33);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(l145)));}l127 = trie_delete(l143,l145->key,l145->len,&l208
);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l145));sal_free(PIVOT_BUCKET_HANDLE(l145));sal_free(l145);
_soc_trident2_alpm_bkt_view_set(l1,l231<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l33)){_soc_trident2_alpm_bkt_view_set(l1,(l231+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l27,l33);if(VRF_TRIE_ROUTES_CNT(l1,
l27,l33) == 0){l127 = l32(l1,l27,l33);}SOC_ALPM_LPM_UNLOCK(l1);return l127;
l242:l204 = trie_insert(l206,prefix,NULL,l140,(trie_node_t*)l239);if(
SOC_FAILURE(l204)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into pfx trie\n")
,prefix[0],prefix[1]));}l240:l204 = trie_insert(trie,prefix,NULL,l140,(
trie_node_t*)l209);if(SOC_FAILURE(l204)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into bkt trie\n"),prefix[0],prefix[1]));}
SOC_ALPM_LPM_UNLOCK(l1);return l127;}int soc_alpm_init(int l1){int l170;int
l127 = SOC_E_NONE;l127 = l2(l1);SOC_IF_ERROR_RETURN(l127);l127 = l18(l1);
alpm_vrf_handle[l1] = sal_alloc((MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t),
"ALPM VRF Handles");if(alpm_vrf_handle[l1] == NULL){return SOC_E_MEMORY;}
tcam_pivot[l1] = sal_alloc(MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*),
"ALPM pivots");if(tcam_pivot[l1] == NULL){return SOC_E_MEMORY;}sal_memset(
alpm_vrf_handle[l1],0,(MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t));sal_memset(
tcam_pivot[l1],0,MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*));for(l170 = 0;l170<
MAX_PIVOT_COUNT;l170++){ALPM_TCAM_PIVOT(l1,l170) = NULL;}if(SOC_CONTROL(l1)->
alpm_bulk_retry == NULL){SOC_CONTROL(l1)->alpm_bulk_retry = sal_sem_create(
"ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_lookup_retry == 
NULL){SOC_CONTROL(l1)->alpm_lookup_retry = sal_sem_create("ALPM lookup retry"
,sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_insert_retry == NULL){
SOC_CONTROL(l1)->alpm_insert_retry = sal_sem_create("ALPM insert retry",
sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_delete_retry == NULL){SOC_CONTROL
(l1)->alpm_delete_retry = sal_sem_create("ALPM delete retry",sal_sem_BINARY,0
);}l127 = soc_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l127);return l127;}
static int l244(int l1){int l170,l127;alpm_pivot_t*l132;for(l170 = 0;l170<
MAX_PIVOT_COUNT;l170++){l132 = ALPM_TCAM_PIVOT(l1,l170);if(l132){if(
PIVOT_BUCKET_HANDLE(l132)){if(PIVOT_BUCKET_TRIE(l132)){l127 = trie_traverse(
PIVOT_BUCKET_TRIE(l132),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l127)){trie_destroy(PIVOT_BUCKET_TRIE(l132));}else{LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Unable to clear trie state for unit %d\n"),l1
));return l127;}}sal_free(PIVOT_BUCKET_HANDLE(l132));}sal_free(
ALPM_TCAM_PIVOT(l1,l170));ALPM_TCAM_PIVOT(l1,l170) = NULL;}}for(l170 = 0;l170
<= SOC_VRF_MAX(l1)+1;l170++){l127 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l1,
l170),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l127)){
trie_destroy(VRF_PREFIX_TRIE_IPV4(l1,l170));}else{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l1,l170));
return l127;}l127 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l1,l170),
alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l127)){
trie_destroy(VRF_PREFIX_TRIE_IPV6(l1,l170));}else{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v6 pfx trie for vrf %d\n"),l1,l170));
return l127;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l170)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l170));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l170
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l170));}sal_memset(&
alpm_vrf_handle[l1][l170],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l1][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,1,1);
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,2,1);if(SOC_ALPM_BUCKET_BMAP(l1)!= NULL){
sal_free(SOC_ALPM_BUCKET_BMAP(l1));}sal_memset(&soc_alpm_bucket[l1],0,sizeof(
soc_alpm_bucket_t));if(alpm_vrf_handle[l1])sal_free(alpm_vrf_handle[l1]);if(
tcam_pivot[l1])sal_free(tcam_pivot[l1]);alpm_vrf_handle[l1] = NULL;tcam_pivot
[l1] = NULL;return SOC_E_NONE;}int soc_alpm_deinit(int l1){l3(l1);
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_deinit(l1));SOC_IF_ERROR_RETURN(
soc_alpm_128_state_clear(l1));SOC_IF_ERROR_RETURN(l244(l1));if(SOC_CONTROL(l1
)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l1)->alpm_bulk_retry);
SOC_CONTROL(l1)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_lookup_retry
);SOC_CONTROL(l1)->alpm_lookup_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_insert_retry
);SOC_CONTROL(l1)->alpm_insert_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_delete_retry
);SOC_CONTROL(l1)->alpm_delete_retry = NULL;}return SOC_E_NONE;}static int
l245(int l1,int l27,int l33){defip_entry_t*lpm_entry,l246;int l247;int index;
int l127 = SOC_E_NONE;uint32 key[2] = {0,0};uint32 l140;alpm_bucket_handle_t*
l212;alpm_pivot_t*l145;payload_t*l239;trie_t*l248;trie_t*l249 = NULL;if(l33 == 
0){trie_init(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(l1,l27));l249 = 
VRF_PIVOT_TRIE_IPV4(l1,l27);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l1,l27));l249 = VRF_PIVOT_TRIE_IPV6(l1,l27);}if(l33 == 0)
{trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l1,l27));l248 = 
VRF_PREFIX_TRIE_IPV4(l1,l27);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6(l1,l27));l248 = VRF_PREFIX_TRIE_IPV6(l1,l27);}lpm_entry = 
sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n")));
return SOC_E_MEMORY;}l29(l1,key,0,l27,l33,lpm_entry,0,1);if(l33 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l27) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27) = lpm_entry;}if(l27 == SOC_VRF_MAX(l1)+1)
{soc_L3_DEFIPm_field32_set(l1,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l1,lpm_entry,DEFAULT_MISS0f,1);}l127 = 
alpm_bucket_assign(l1,&l247,l33);soc_L3_DEFIPm_field32_set(l1,lpm_entry,
ALG_BKT_PTR0f,l247);sal_memcpy(&l246,lpm_entry,sizeof(l246));l127 = l4(l1,&
l246,&index);l212 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l212 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
return SOC_E_NONE;}sal_memset(l212,0,sizeof(*l212));l145 = sal_alloc(sizeof(
alpm_pivot_t),"Payload for Pivot");if(l145 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"soc_alpm_vrf_add: Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l212);return SOC_E_MEMORY;}l239 = sal_alloc(
sizeof(payload_t),"Payload for pfx trie key");if(l239 == NULL){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n")));
sal_free(l212);sal_free(l145);return SOC_E_MEMORY;}sal_memset(l145,0,sizeof(*
l145));sal_memset(l239,0,sizeof(*l239));l140 = 0;PIVOT_BUCKET_HANDLE(l145) = 
l212;if(l33){trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l145));}else{
trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l145));}PIVOT_BUCKET_INDEX(l145
) = l247;PIVOT_BUCKET_VRF(l145) = l27;PIVOT_BUCKET_IPV6(l145) = l33;
PIVOT_BUCKET_DEF(l145) = TRUE;l145->key[0] = l239->key[0] = key[0];l145->key[
1] = l239->key[1] = key[1];l145->len = l239->len = l140;l127 = trie_insert(
l248,key,NULL,l140,&(l239->node));if(SOC_FAILURE(l127)){sal_free(l239);
sal_free(l145);sal_free(l212);return l127;}l127 = trie_insert(l249,key,NULL,
l140,(trie_node_t*)l145);if(SOC_FAILURE(l127)){trie_node_t*l208 = NULL;(void)
trie_delete(l248,key,l140,&l208);sal_free(l239);sal_free(l145);sal_free(l212)
;return l127;}index = soc_alpm_physical_idx(l1,L3_DEFIPm,index,l33);if(l33 == 
0){ALPM_TCAM_PIVOT(l1,index) = l145;PIVOT_TCAM_INDEX(l145) = index;}else{
ALPM_TCAM_PIVOT(l1,index<<1) = l145;PIVOT_TCAM_INDEX(l145) = index<<1;}
VRF_PIVOT_REF_INC(l1,l27,l33);VRF_TRIE_INIT_DONE(l1,l27,l33,1);return l127;}
static int l32(int l1,int l27,int l33){defip_entry_t*lpm_entry;int l247;int
l129;int l127 = SOC_E_NONE;uint32 key[2] = {0,0},l250[SOC_MAX_MEM_FIELD_WORDS
];payload_t*l209;alpm_pivot_t*l251;trie_node_t*l208;trie_t*l248;trie_t*l249 = 
NULL;if(l33 == 0){lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l27);}else{
lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27);}l247 = 
soc_L3_DEFIPm_field32_get(l1,lpm_entry,ALG_BKT_PTR0f);l127 = 
alpm_bucket_release(l1,l247,l33);_soc_trident2_alpm_bkt_view_set(l1,l247<<2,
INVALIDm);l127 = l17(l1,lpm_entry,(void*)l250,&l129);if(SOC_FAILURE(l127)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n"),
l27,l33));l129 = -1;}l129 = soc_alpm_physical_idx(l1,L3_DEFIPm,l129,l33);if(
l33 == 0){l251 = ALPM_TCAM_PIVOT(l1,l129);}else{l251 = ALPM_TCAM_PIVOT(l1,
l129<<1);}l127 = l6(l1,lpm_entry);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),l27,l33));}sal_free(lpm_entry);if(
l33 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l27) = NULL;l248 = 
VRF_PREFIX_TRIE_IPV4(l1,l27);VRF_PREFIX_TRIE_IPV4(l1,l27) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27) = NULL;l248 = VRF_PREFIX_TRIE_IPV6(l1,l27
);VRF_PREFIX_TRIE_IPV6(l1,l27) = NULL;}VRF_TRIE_INIT_DONE(l1,l27,l33,0);l127 = 
trie_delete(l248,key,0,&l208);l209 = (payload_t*)l208;if(SOC_FAILURE(l127)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal default for vrf "" %d/%d\n"),l27,l33));}sal_free(
l209);(void)trie_destroy(l248);if(l33 == 0){l249 = VRF_PIVOT_TRIE_IPV4(l1,l27
);VRF_PIVOT_TRIE_IPV4(l1,l27) = NULL;}else{l249 = VRF_PIVOT_TRIE_IPV6(l1,l27)
;VRF_PIVOT_TRIE_IPV6(l1,l27) = NULL;}l208 = NULL;l127 = trie_delete(l249,key,
0,&l208);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l27,l33));}(void)
trie_destroy(l249);(void)trie_destroy(PIVOT_BUCKET_TRIE(l251));sal_free(
PIVOT_BUCKET_HANDLE(l251));sal_free(l251);return l127;}int soc_alpm_insert(
int l1,void*l5,uint32 l24,int l252,int l253){defip_alpm_ipv4_entry_t l214,
l215;defip_alpm_ipv6_64_entry_t l216,l217;soc_mem_t l23;void*l218,*l232;int
l148,l27;int index;int l9;int l127 = SOC_E_NONE;uint32 l25;l9 = 
soc_mem_field32_get(l1,L3_DEFIPm,l5,MODE0f);l23 = (l9)?L3_DEFIP_ALPM_IPV6_64m
:L3_DEFIP_ALPM_IPV4m;l218 = ((l9)?((uint32*)&(l216)):((uint32*)&(l214)));l232
= ((l9)?((uint32*)&(l217)):((uint32*)&(l215)));SOC_IF_ERROR_RETURN(l19(l1,l5,
l218,l232,l23,l24,&l25));SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l5,&l148
,&l27));if(l148 == SOC_L3_VRF_OVERRIDE){l127 = l4(l1,l5,&index);if(
SOC_SUCCESS(l127)){VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l9);VRF_TRIE_ROUTES_INC(l1
,MAX_VRF_ID,l9);}else if(l127 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID
,l9);}return(l127);}else if(l27 == 0){if(soc_alpm_mode_get(l1)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"VRF=0 cannot be added in Parallel mode\n")));
return SOC_E_PARAM;}}if(l148!= SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l1)){
if(VRF_TRIE_ROUTES_CNT(l1,l27,l9) == 0){if(!l25){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n"),l148));return SOC_E_PARAM;}}}}if(!
VRF_TRIE_INIT_COMPLETED(l1,l27,l9)){LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_insert:VRF %d is not ""initialized\n"),l27));l127 = l245(l1,l27,
l9);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n"),l27,l9));return l127;}
LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),l27,l9));}if(l253&
SOC_ALPM_LOOKUP_HIT){l127 = l149(l1,l5,l218,l232,l23,l252);}else{if(l252 == -
1){l252 = 0;}l127 = l199(l1,l5,l23,l218,l232,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l252),l253);}if(l127!= SOC_E_NONE){LOG_BSL_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l1,soc_errmsg(l127)
));}return(l127);}int soc_alpm_lookup(int l1,void*l7,void*l14,int*l15,int*
l254){defip_alpm_ipv4_entry_t l214;defip_alpm_ipv6_64_entry_t l216;soc_mem_t
l23;int bucket_index;int l142;void*l218;int l148,l27;int l9,l119;int l127 = 
SOC_E_NONE;l9 = soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l7,&l148,&l27));l127 = l13(l1,l7,
l14,l15,&l119,&l9);if(SOC_SUCCESS(l127)){if(!l9&&(*l15&0x1)){l127 = 
soc_alpm_lpm_ip4entry1_to_0(l1,l14,l14,PRESERVE_HIT);}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l14,&l148,&l27));if(l148 == SOC_L3_VRF_OVERRIDE){
return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l27,l9)){LOG_BSL_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_lookup:VRF %d is not initialized\n")
,l27));*l254 = 0;return SOC_E_NOT_FOUND;}l23 = (l9)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l218 = ((l9)?((uint32*)&(l216)):((uint32*)&(l214)));
SOC_ALPM_LPM_LOCK(l1);l127 = l146(l1,l7,l23,l218,&l142,&bucket_index,l15,TRUE
);SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l127)){*l254 = l142;*l15 = 
bucket_index<<2;return l127;}l127 = l26(l1,l218,l23,l9,l148,bucket_index,*l15
,l14);*l254 = SOC_ALPM_LOOKUP_HIT|l142;return(l127);}static int l255(int l1,
void*l7,void*l14,int l27,int*l142,int*bucket_index,int*l120,int l256){int l127
= SOC_E_NONE;int l170,l257,l33,l141 = 0;uint32 l10,l137;
defip_aux_scratch_entry_t l12;int l258,l259;int index;soc_mem_t l23,l260;int
l261,l262;int l263;uint32 l264[SOC_MAX_MEM_FIELD_WORDS] = {0};int l265 = -1;
int l266 = 0;soc_field_t l267[2] = {IP_ADDR0f,IP_ADDR1f,};l260 = L3_DEFIPm;
l33 = soc_mem_field32_get(l1,l260,l7,MODE0f);l258 = soc_mem_field32_get(l1,
l260,l7,GLOBAL_ROUTE0f);l259 = soc_mem_field32_get(l1,l260,l7,VRF_ID_0f);
LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d:\n"),l27 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l33,l258,l259));if(l27 == SOC_L3_VRF_GLOBAL){l10 = l256?1:0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l137);soc_mem_field32_set(l1,l260,l7,
GLOBAL_ROUTE0f,1);soc_mem_field32_set(l1,l260,l7,VRF_ID_0f,0);}else{l10 = 
l256?3:2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l137);}sal_memset(&l12,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(l1,l7,l33,l10,0,&l12));if(
l27 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l1,l260,l7,GLOBAL_ROUTE0f,l258)
;soc_mem_field32_set(l1,l260,l7,VRF_ID_0f,l259);}SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l12,TRUE,&l141,l142,bucket_index));if(l141
== 0){LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Could not find bucket\n")))
;return SOC_E_NOT_FOUND;}LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in memory %s, index %d, ""bucket_index %d\n"),SOC_MEM_NAME(l1,l260),
soc_alpm_logical_idx(l1,l260,(*l142)>>1,1),*bucket_index));l23 = (l33)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l127 = l126(l1,l7,&l262);if(
SOC_FAILURE(l127)){return l127;}l263 = 24;if(l33){if(SOC_ALPM_V6_SCALE_CHECK(
l1,l33)){l263 = 32;}else{l263 = 16;}}LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Start searching mem %s bucket %d(count %d) ""for Length %d\n"),
SOC_MEM_NAME(l1,l23),*bucket_index,l263,l262));for(l170 = 0;l170<l263;l170++)
{uint32 l218[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l179[2] = {0};uint32 l268[
2] = {0};uint32 l269[2] = {0};int l270;l127 = _soc_alpm_mem_index(l1,l23,*
bucket_index,l170,l137,&index);if(l127 == SOC_E_FULL){continue;}
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l23,MEM_BLOCK_ANY,index,(void*)l218));
l270 = soc_mem_field32_get(l1,l23,l218,VALIDf);l261 = soc_mem_field32_get(l1,
l23,l218,LENGTHf);LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket %5d Index %6d: Valid %d, Length %d\n"),*bucket_index,index,l270,l261)
);if(!l270||(l261>l262)){continue;}SHR_BITSET_RANGE(l179,(l33?64:32)-l261,
l261);(void)soc_mem_field_get(l1,l23,(uint32*)l218,KEYf,(uint32*)l268);l269[1
] = soc_mem_field32_get(l1,l260,l7,l267[1]);l269[0] = soc_mem_field32_get(l1,
l260,l7,l267[0]);LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tmask %08x %08x\n\t key %08x %08x\n""\thost %08x %08x\n"),l179[1],l179[0],
l268[1],l268[0],l269[1],l269[0]));for(l257 = l33?1:0;l257>= 0;l257--){if((
l269[l257]&l179[l257])!= (l268[l257]&l179[l257])){break;}}if(l257>= 0){
continue;}LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket %d, ""index %d\n"),SOC_MEM_NAME(l1,l23),*
bucket_index,index));if(l265 == -1||l265<l261){l265 = l261;l266 = index;
sal_memcpy(l264,l218,sizeof(l218));}}if(l265!= -1){l127 = l26(l1,l264,l23,l33
,l27,*bucket_index,l266,l14);if(SOC_SUCCESS(l127)){*l120 = l266;if(bsl_check(
bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"Hit mem %s bucket %d, index %d\n"),SOC_MEM_NAME(l1,l23),*
bucket_index,l266));}}return l127;}*l120 = soc_alpm_logical_idx(l1,l260,(*
l142)>>1,1);LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Miss in mem %s bucket %d, use associate data ""in mem %s LOG index %d\n"),
SOC_MEM_NAME(l1,l23),*bucket_index,SOC_MEM_NAME(l1,l260),*l120));
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l260,MEM_BLOCK_ANY,*l120,(void*)l14));if(
(!l33)&&((*l142)&1)){l127 = soc_alpm_lpm_ip4entry1_to_0(l1,l14,l14,
PRESERVE_HIT);}return SOC_E_NONE;}int soc_alpm_find_best_match(int l1,void*l7
,void*l14,int*l15,int l256){int l127 = SOC_E_NONE;int l170,l271,l272;
defip_entry_t l273;uint32 l274[2];uint32 l268[2];uint32 l275[2];uint32 l269[2
];uint32 l276,l277;int l148,l27 = 0;int l278[2] = {0};int l142,bucket_index;
soc_mem_t l260 = L3_DEFIPm;int l191,l33,l279,l280 = 0;soc_field_t l281[] = {
GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l282[] = {GLOBAL_ROUTE0f,
GLOBAL_ROUTE1f};l33 = soc_mem_field32_get(l1,l260,l7,MODE0f);if(!
SOC_URPF_STATUS_GET(l1)&&l256){return SOC_E_PARAM;}l271 = soc_mem_index_min(
l1,l260);l272 = soc_mem_index_count(l1,l260);if(SOC_URPF_STATUS_GET(l1)){l272
>>= 1;}if(soc_alpm_mode_get(l1)){l272>>= 1;l271+= l272;}if(l256){l271+= 
soc_mem_index_count(l1,l260)/2;}LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Launch LPM searching from index %d count %d\n"),l271,l272));for(l170 = l271;
l170<l271+l272;l170++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l260,MEM_BLOCK_ANY
,l170,(void*)&l273));l278[0] = soc_mem_field32_get(l1,l260,&l273,VALID0f);
l278[1] = soc_mem_field32_get(l1,l260,&l273,VALID1f);if(l278[0] == 0&&l278[1]
== 0){continue;}l279 = soc_mem_field32_get(l1,l260,&l273,MODE0f);if(l279!= 
l33){continue;}for(l191 = 0;l191<(l33?1:2);l191++){if(l278[l191] == 0){
continue;}l276 = soc_mem_field32_get(l1,l260,&l273,l281[l191]);l277 = 
soc_mem_field32_get(l1,l260,&l273,l282[l191]);if(!l276||!l277){continue;}
LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Match a Global High route: ent %d\n"),l191));l274[0] = soc_mem_field32_get(
l1,l260,&l273,IP_ADDR_MASK0f);l274[1] = soc_mem_field32_get(l1,l260,&l273,
IP_ADDR_MASK1f);l268[0] = soc_mem_field32_get(l1,l260,&l273,IP_ADDR0f);l268[1
] = soc_mem_field32_get(l1,l260,&l273,IP_ADDR1f);l275[0] = 
soc_mem_field32_get(l1,l260,l7,IP_ADDR_MASK0f);l275[1] = soc_mem_field32_get(
l1,l260,l7,IP_ADDR_MASK1f);l269[0] = soc_mem_field32_get(l1,l260,l7,IP_ADDR0f
);l269[1] = soc_mem_field32_get(l1,l260,l7,IP_ADDR1f);LOG_BSL_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\thmsk %08x %08x\n\thkey %08x %08x\n"
"\tsmsk %08x %08x\n\tskey %08x %08x\n"),l274[1],l274[0],l268[1],l268[0],l275[
1],l275[0],l269[1],l269[0]));if(l33&&(((l274[1]&l275[1])!= l274[1])||((l274[0
]&l275[0])!= l274[0]))){continue;}if(!l33&&((l274[l191]&l275[0])!= l274[l191]
)){continue;}if(l33&&((l269[0]&l274[0]) == (l268[0]&l274[0]))&&((l269[1]&l274
[1]) == (l268[1]&l274[1]))){l280 = TRUE;break;}if(!l33&&((l269[0]&l274[l191])
== (l268[l191]&l274[l191]))){l280 = TRUE;break;}}if(!l280){continue;}
LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit Global High route in index = %d(%d)\n"),l170,l191));sal_memcpy(l14,&l273
,sizeof(l273));if(!l33&&l191 == 1){l127 = soc_alpm_lpm_ip4entry1_to_0(l1,l14,
l14,PRESERVE_HIT);}*l15 = l170;return l127;}LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for VRF and Global Low routes\n")));SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l7,&l148,&l27));l127 = l255(l1,l7,l14,l27,&l142,&
bucket_index,l15,l256);if(l127 == SOC_E_NOT_FOUND){l27 = SOC_L3_VRF_GLOBAL;
LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Not found in VRF region, trying Global ""region\n")));l127 = l255(l1,l7,l14,
l27,&l142,&bucket_index,l15,l256);}if(SOC_SUCCESS(l127)){LOG_BSL_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit in %s region in TCAM index %d, "
"buckekt_index %d\n"),l27 == SOC_L3_VRF_GLOBAL?"Global Low":"VRF",l142,
bucket_index));}else{LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Search miss for given address\n")));}return(l127);}int soc_alpm_delete(int l1
,void*l7,int l252,int l253){int l148,l27;int l9;int l127 = SOC_E_NONE;l9 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f);SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l7,&l148,&l27));if(l148 == SOC_L3_VRF_OVERRIDE){l127 = 
l6(l1,l7);if(SOC_SUCCESS(l127)){VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l9);
VRF_TRIE_ROUTES_DEC(l1,MAX_VRF_ID,l9);}return(l127);}else{if(!
VRF_TRIE_INIT_COMPLETED(l1,l27,l9)){LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_delete:VRF %d/%d is not initialized\n"),l27,l9));return
SOC_E_NONE;}if(l252 == -1){l252 = 0;}l127 = l233(l1,l7,
SOC_ALPM_BKT_ENTRY_TO_IDX(l252),l253&~SOC_ALPM_LOOKUP_HIT,l252);}return(l127)
;}static int l18(int l1){int l283;l283 = soc_mem_index_count(l1,L3_DEFIPm)+
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)*2;if(SOC_URPF_STATUS_GET(l1)){l283
>>= 1;}SOC_ALPM_BUCKET_COUNT(l1) = l283*2;SOC_ALPM_BUCKET_BMAP_SIZE(l1) = 
SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(l1));SOC_ALPM_BUCKET_BMAP(l1) = 
sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l1),"alpm_shared_bucket_bitmap");if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){LOG_BSL_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_shared_mem_init: Memory allocation for "
"bucket bitmap management failed\n")));return SOC_E_MEMORY;}sal_memset(
SOC_ALPM_BUCKET_BMAP(l1),0,SOC_ALPM_BUCKET_BMAP_SIZE(l1));alpm_bucket_assign(
l1,&l283,1);return SOC_E_NONE;}int alpm_bucket_assign(int l1,int*l247,int l33
){int l170,l284 = 1,l285 = 0;if(l33){if(!soc_alpm_mode_get(l1)&&!
SOC_URPF_STATUS_GET(l1)){l284 = 2;}}for(l170 = 0;l170<SOC_ALPM_BUCKET_COUNT(
l1);l170+= l284){SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l170,l284,l285);
if(0 == l285){break;}}if(l170 == SOC_ALPM_BUCKET_COUNT(l1)){return SOC_E_FULL
;}SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l170,l284);*l247 = l170;
SOC_ALPM_BUCKET_NEXT_FREE(l1) = l170;return SOC_E_NONE;}int
alpm_bucket_release(int l1,int l247,int l33){int l284 = 1,l285 = 0;if((l247<1
)||(l247>SOC_ALPM_BUCKET_MAX_INDEX(l1))){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Unit %d\n, freeing invalid bucket index %d\n"),l1,l247));
return SOC_E_PARAM;}if(l33){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(
l1)){l284 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l247,l284,l285);if
(!l285){return SOC_E_PARAM;}SHR_BITCLR_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l247,
l284);return SOC_E_NONE;}int alpm_bucket_is_assigned(int l1,int l286,int l9,
int*l285){int l284 = 1;if((l286<1)||(l286>SOC_ALPM_BUCKET_MAX_INDEX(l1))){
return SOC_E_PARAM;}if(l9){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1
)){l284 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l286,l284,*l285);
return SOC_E_NONE;}static void l107(int l1,void*l14,int index,l102 l108){if(
index&(0x8000)){l108[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l14),(l95[(l1)]->l59));l108[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l61));l108[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l60));l108[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l62));if((!(SOC_IS_HURRICANE(l1)))&&(((l95[(l1)]->l75)!= NULL))){int
l287;(void)soc_alpm_lpm_vrf_get(l1,l14,(int*)&l108[4],&l287);}else{l108[4] = 
0;};}else{if(index&0x1){l108[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(l1)]->l60));l108[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l62));l108[2] = 0;l108[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l95[(l1)]->l76)!= NULL))){int l287;defip_entry_t l288;(void)
soc_alpm_lpm_ip4entry1_to_0(l1,l14,&l288,0);(void)soc_alpm_lpm_vrf_get(l1,&
l288,(int*)&l108[4],&l287);}else{l108[4] = 0;};}else{l108[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l59));l108[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l61));l108[2] = 0;l108[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l95[(l1)]->l75)!= NULL))){int l287;(void)
soc_alpm_lpm_vrf_get(l1,l14,(int*)&l108[4],&l287);}else{l108[4] = 0;};}}}
static int l289(l102 l104,l102 l105){int l129;for(l129 = 0;l129<5;l129++){{if
((l104[l129])<(l105[l129])){return-1;}if((l104[l129])>(l105[l129])){return 1;
}};}return(0);}static void l290(int l1,void*l5,uint32 l291,uint32 l122,int
l119){l102 l292;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l95[(l1)]->l63))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l95[(l1)]->l74))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l95[(l1
)]->l73))){l292[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l95[(l1)]->l59));l292[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l95[(l1
)]->l61));l292[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l95[(l1)]->l60));l292[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l95[(l1
)]->l62));if((!(SOC_IS_HURRICANE(l1)))&&(((l95[(l1)]->l75)!= NULL))){int l287
;(void)soc_alpm_lpm_vrf_get(l1,l5,(int*)&l292[4],&l287);}else{l292[4] = 0;};
l121((l106[(l1)]),l289,l292,l119,l122,((uint16)l291<<1)|(0x8000));}}else{if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l95[(l1
)]->l73))){l292[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l95[(l1)]->l59));l292[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l95[(l1
)]->l61));l292[2] = 0;l292[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&(((
l95[(l1)]->l75)!= NULL))){int l287;(void)soc_alpm_lpm_vrf_get(l1,l5,(int*)&
l292[4],&l287);}else{l292[4] = 0;};l121((l106[(l1)]),l289,l292,l119,l122,((
uint16)l291<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l95[(l1)]->l74))){l292[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l95[(l1
)]->l60));l292[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l95[(l1)]->l62));l292[2] = 0;l292[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l95[(l1)]->l76)!= NULL))){int l287;defip_entry_t
l288;(void)soc_alpm_lpm_ip4entry1_to_0(l1,l5,&l288,0);(void)
soc_alpm_lpm_vrf_get(l1,&l288,(int*)&l292[4],&l287);}else{l292[4] = 0;};l121(
(l106[(l1)]),l289,l292,l119,l122,(((uint16)l291<<1)+1));}}}static void l293(
int l1,void*l7,uint32 l291){l102 l292;int l119 = -1;int l127;uint16 index;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1
)]->l63))){l292[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l7),(l95[(l1)]->l59));l292[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1
)]->l61));l292[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l7),(l95[(l1)]->l60));l292[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1
)]->l62));if((!(SOC_IS_HURRICANE(l1)))&&(((l95[(l1)]->l75)!= NULL))){int l287
;(void)soc_alpm_lpm_vrf_get(l1,l7,(int*)&l292[4],&l287);}else{l292[4] = 0;};
index = (l291<<1)|(0x8000);}else{l292[0] = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1)]->l59));l292[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1
)]->l61));l292[2] = 0;l292[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&(((
l95[(l1)]->l75)!= NULL))){int l287;(void)soc_alpm_lpm_vrf_get(l1,l7,(int*)&
l292[4],&l287);}else{l292[4] = 0;};index = l291;}l127 = l124((l106[(l1)]),
l289,l292,l119,index);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\ndel  index: H %d error %d\n"),index,l127));}}static int l294
(int l1,void*l7,int l119,int*l120){l102 l292;int l295;int l127;uint16 index = 
(0xFFFF);l295 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm
)),(l7),(l95[(l1)]->l63));if(l295){l292[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1
)]->l59));l292[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l7),(l95[(l1)]->l61));l292[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1
)]->l60));l292[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l7),(l95[(l1)]->l62));if((!(SOC_IS_HURRICANE(l1)))&&(((l95[(l1)]
->l75)!= NULL))){int l287;(void)soc_alpm_lpm_vrf_get(l1,l7,(int*)&l292[4],&
l287);}else{l292[4] = 0;};}else{l292[0] = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1)]->l59));l292[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1
)]->l61));l292[2] = 0;l292[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&(((
l95[(l1)]->l75)!= NULL))){int l287;(void)soc_alpm_lpm_vrf_get(l1,l7,(int*)&
l292[4],&l287);}else{l292[4] = 0;};}l127 = l116((l106[(l1)]),l289,l292,l119,&
index);if(SOC_FAILURE(l127)){*l120 = 0xFFFFFFFF;return(l127);}*l120 = index;
return(SOC_E_NONE);}static uint16 l109(uint8*l110,int l111){return(
_shr_crc16b(0,l110,l111));}static int l112(int l20,int l97,int l98,l101**l113
){l101*l117;int index;if(l98>l97){return SOC_E_MEMORY;}l117 = sal_alloc(
sizeof(l101),"lpm_hash");if(l117 == NULL){return SOC_E_MEMORY;}sal_memset(
l117,0,sizeof(*l117));l117->l20 = l20;l117->l97 = l97;l117->l98 = l98;l117->
l99 = sal_alloc(l117->l98*sizeof(*(l117->l99)),"hash_table");if(l117->l99 == 
NULL){sal_free(l117);return SOC_E_MEMORY;}l117->l100 = sal_alloc(l117->l97*
sizeof(*(l117->l100)),"link_table");if(l117->l100 == NULL){sal_free(l117->l99
);sal_free(l117);return SOC_E_MEMORY;}for(index = 0;index<l117->l98;index++){
l117->l99[index] = (0xFFFF);}for(index = 0;index<l117->l97;index++){l117->
l100[index] = (0xFFFF);}*l113 = l117;return SOC_E_NONE;}static int l114(l101*
l115){if(l115!= NULL){sal_free(l115->l99);sal_free(l115->l100);sal_free(l115)
;}return SOC_E_NONE;}static int l116(l101*l117,l103 l118,l102 entry,int l119,
uint16*l120){int l1 = l117->l20;uint16 l296;uint16 index;l296 = l109((uint8*)
entry,(32*5))%l117->l98;index = l117->l99[l296];;;while(index!= (0xFFFF)){
uint32 l14[SOC_MAX_MEM_FIELD_WORDS];l102 l108;int l297;l297 = (index&(0x7FFF)
)>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l297,l14));l107(l1,
l14,index,l108);if((*l118)(entry,l108) == 0){*l120 = (index&(0x7FFF))>>((
index&(0x8000))?1:0);;return(SOC_E_NONE);}index = l117->l100[index&(0x7FFF)];
;};return(SOC_E_NOT_FOUND);}static int l121(l101*l117,l103 l118,l102 entry,
int l119,uint16 l122,uint16 l123){int l1 = l117->l20;uint16 l296;uint16 index
;uint16 l298;l296 = l109((uint8*)entry,(32*5))%l117->l98;index = l117->l99[
l296];;;;l298 = (0xFFFF);if(l122!= (0xFFFF)){while(index!= (0xFFFF)){uint32
l14[SOC_MAX_MEM_FIELD_WORDS];l102 l108;int l297;l297 = (index&(0x7FFF))>>1;
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l297,l14));l107(l1,l14,
index,l108);if((*l118)(entry,l108) == 0){if(l123!= index){;if(l298 == (0xFFFF
)){l117->l99[l296] = l123;l117->l100[l123&(0x7FFF)] = l117->l100[index&(
0x7FFF)];l117->l100[index&(0x7FFF)] = (0xFFFF);}else{l117->l100[l298&(0x7FFF)
] = l123;l117->l100[l123&(0x7FFF)] = l117->l100[index&(0x7FFF)];l117->l100[
index&(0x7FFF)] = (0xFFFF);}};return(SOC_E_NONE);}l298 = index;index = l117->
l100[index&(0x7FFF)];;}}l117->l100[l123&(0x7FFF)] = l117->l99[l296];l117->l99
[l296] = l123;return(SOC_E_NONE);}static int l124(l101*l117,l103 l118,l102
entry,int l119,uint16 l125){uint16 l296;uint16 index;uint16 l298;l296 = l109(
(uint8*)entry,(32*5))%l117->l98;index = l117->l99[l296];;;l298 = (0xFFFF);
while(index!= (0xFFFF)){if(l125 == index){;if(l298 == (0xFFFF)){l117->l99[
l296] = l117->l100[l125&(0x7FFF)];l117->l100[l125&(0x7FFF)] = (0xFFFF);}else{
l117->l100[l298&(0x7FFF)] = l117->l100[l125&(0x7FFF)];l117->l100[l125&(0x7FFF
)] = (0xFFFF);}return(SOC_E_NONE);}l298 = index;index = l117->l100[index&(
0x7FFF)];;}return(SOC_E_NOT_FOUND);}int _ipmask2pfx(uint32 l299,int*l300){*
l300 = 0;while(l299&(1<<31)){*l300+= 1;l299<<= 1;}return((l299)?SOC_E_PARAM:
SOC_E_NONE);}int soc_alpm_lpm_ip4entry0_to_0(int l1,void*l301,void*l302,int
l303){uint32 l128;l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l301),(l95[(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l73),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l63));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l63),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l59));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l59),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l61),(l128));if(((l95[(l1)]->
l49)!= NULL)){l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l301),(l95[(l1)]->l49));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l49),(l128));}if(((l95[(l1)]->
l51)!= NULL)){l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l301),(l95[(l1)]->l51));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l51),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l53));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l53),(l128));}else{l128 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l67));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l67),(l128));}l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l301),(l95[(l1)]->l69));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l69),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l71));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l71),(l128));if(((l95[(l1)]->l75)!= NULL)){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l75),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l77));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l77),(l128));}if(((l95[(l1)]->l47)!= NULL)){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l47));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l47),(l128));}if(l303){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l57));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l57),(l128));}l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l79),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l81));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l81),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l83));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l83),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l85));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l85),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l87),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l89),(l128));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_1(int l1,void*l301,void*l302,int l303){uint32 l128;
l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301)
,(l95[(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l302),(l95[(l1)]->l74),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l64));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l64),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l60));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l60),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l62));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l62),(l128));if(((l95[(l1)]->
l50)!= NULL)){l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l301),(l95[(l1)]->l50));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l50),(l128));}if(((l95[(l1)]->
l52)!= NULL)){l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l301),(l95[(l1)]->l52));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l52),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l54));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l54),(l128));}else{l128 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l68));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l68),(l128));}l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l301),(l95[(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l70),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l72));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l72),(l128));if(((l95[(l1)]->l76)!= NULL)){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l76),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l78));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l78),(l128));}if(((l95[(l1)]->l48)!= NULL)){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l48),(l128));}if(l303){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l58));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l58),(l128));}l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l80),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l82));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l82),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l84));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l84),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l86));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l86),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l88),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l90),(l128));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_1(int l1,void*l301,void*l302,int l303){uint32 l128;
l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301)
,(l95[(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l302),(l95[(l1)]->l74),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l63));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l64),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l59));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l60),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l62),(l128));if(!
SOC_IS_HURRICANE(l1)){l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l49));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l50),(l128));}if(((l95[(l1)]->
l51)!= NULL)){l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l301),(l95[(l1)]->l51));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l52),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l53));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l54),(l128));}else{l128 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l67));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l68),(l128));}l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l301),(l95[(l1)]->l69));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l70),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l71));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l72),(l128));if(((l95[(l1)]->l75)!= NULL)){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l76),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l77));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l78),(l128));}if(((l95[(l1)]->l47)!= NULL)){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l47));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l48),(l128));}if(l303){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l57));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l58),(l128));}l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l80),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l81));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l82),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l83));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l84),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l85));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l86),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l88),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l90),(l128));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_0(int l1,void*l301,void*l302,int l303){uint32 l128;
l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301)
,(l95[(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l302),(l95[(l1)]->l73),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l64));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l63),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l60));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l59),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l62));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l61),(l128));if(!
SOC_IS_HURRICANE(l1)){l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l50));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l49),(l128));}if(((l95[(l1)]->
l52)!= NULL)){l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l301),(l95[(l1)]->l52));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l51),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l54));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l53),(l128));}else{l128 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l68));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l67),(l128));}l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l301),(l95[(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l69),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l72));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l71),(l128));if(((l95[(l1)]->l76)!= NULL)){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l75),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l78));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l77),(l128));}if(((l95[(l1)]->l48)!= NULL)){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l47),(l128));}if(l303){l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l58));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l57),(l128));}l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l79),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l82));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l81),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l84));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l83),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(l1)]->l86));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(
l1)]->l85),(l128));l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l301),(l95[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l302),(l95[(l1)]->l87),(l128));l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l301),(l95[(
l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l302),(l95[(l1)]->l89),(l128));return(SOC_E_NONE);}static int l304(int l1,
void*l14){return(SOC_E_NONE);}void soc_alpm_lpm_state_dump(int l1){int l170;
int l305;l305 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l1)){return;}for(l170 = l305;l170>= 0;l170--){if((l170!= (
(3*(64+32+2+1))-1))&&((l43[(l1)][(l170)].l36) == -1)){continue;}LOG_BSL_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l170,(l43
[(l1)][(l170)].l38),(l43[(l1)][(l170)].next),(l43[(l1)][(l170)].l36),(l43[(l1
)][(l170)].l37),(l43[(l1)][(l170)].l39),(l43[(l1)][(l170)].l40)));}
COMPILER_REFERENCE(l304);}static int l306(int l1,int index,uint32*l14){int
l307;int l9;uint32 l308;uint32 l309;int l310;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l307 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l1)){l307 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1)+0x0400;}else{l307 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}l9 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(l1)]->l63));if(((l95[(l1)]->l47)!= 
NULL)){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),
(l95[(l1)]->l47),(0));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l48),(0));}l308 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l61));l309 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l62));if(!l9){if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l73))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l14),(l95[(l1)]->l71),((!l308)?1:0));}if(soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(l1)]->l74))){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l72),((!l309)?1:0));}}else{l310 = ((!l308)&&(!l309))?1:0;l308 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l73));l309 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l74));if(l308&&l309){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l71),(l310));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l72),(l310));}}return l165(l1,MEM_BLOCK_ANY,
index+l307,index,l14);}static int l311(int l1,int l312,int l313){uint32 l14[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,
l312,l14));l290(l1,l14,l313,0x4000,0);SOC_IF_ERROR_RETURN(l165(l1,
MEM_BLOCK_ANY,l313,l312,l14));SOC_IF_ERROR_RETURN(l306(l1,l313,l14));do{int
l314,l315;l314 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l312),1);l315 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l313),1);ALPM_TCAM_PIVOT(l1,l315<<1) = 
ALPM_TCAM_PIVOT(l1,l314<<1);ALPM_TCAM_PIVOT(l1,(l315<<1)+1) = ALPM_TCAM_PIVOT
(l1,(l314<<1)+1);if(ALPM_TCAM_PIVOT((l1),l315<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l1),l315<<1)) = l315<<1;}if(ALPM_TCAM_PIVOT((l1),(l315<<1)+1
)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(l315<<1)+1)) = (l315<<1)+1;}
ALPM_TCAM_PIVOT(l1,l314<<1) = NULL;ALPM_TCAM_PIVOT(l1,(l314<<1)+1) = NULL;}
while(0);return(SOC_E_NONE);}static int l316(int l1,int l119,int l9){uint32
l14[SOC_MAX_MEM_FIELD_WORDS];int l312;int l313;uint32 l317,l318;l313 = (l43[(
l1)][(l119)].l37)+1;if(!l9){l312 = (l43[(l1)][(l119)].l37);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l312,l14));l317 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l73));l318 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l74));if((l317 == 0)||(l318 == 0)){l290(l1,l14,
l313,0x4000,0);SOC_IF_ERROR_RETURN(l165(l1,MEM_BLOCK_ANY,l313,l312,l14));
SOC_IF_ERROR_RETURN(l306(l1,l313,l14));do{int l319 = soc_alpm_physical_idx((
l1),L3_DEFIPm,(l312),1)<<1;int l228 = soc_alpm_physical_idx((l1),L3_DEFIPm,(
l313),1)*2+(l319&1);if((l318)){l319++;}ALPM_TCAM_PIVOT((l1),l228) = 
ALPM_TCAM_PIVOT((l1),l319);if(ALPM_TCAM_PIVOT((l1),l228)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l1),l228)) = l228;}ALPM_TCAM_PIVOT((l1),l319) = NULL;}while(
0);l313--;}}l312 = (l43[(l1)][(l119)].l36);if(l312!= l313){
SOC_IF_ERROR_RETURN(l311(l1,l312,l313));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l9)
;}(l43[(l1)][(l119)].l36)+= 1;(l43[(l1)][(l119)].l37)+= 1;return(SOC_E_NONE);
}static int l320(int l1,int l119,int l9){uint32 l14[SOC_MAX_MEM_FIELD_WORDS];
int l312;int l313;int l321;uint32 l317,l318;l313 = (l43[(l1)][(l119)].l36)-1;
if((l43[(l1)][(l119)].l39) == 0){(l43[(l1)][(l119)].l36) = l313;(l43[(l1)][(
l119)].l37) = l313-1;return(SOC_E_NONE);}if((!l9)&&((l43[(l1)][(l119)].l37)!= 
(l43[(l1)][(l119)].l36))){l312 = (l43[(l1)][(l119)].l37);SOC_IF_ERROR_RETURN(
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l312,l14));l317 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l73));l318 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l74));if((l317 == 0)||(l318 == 0)){l321 = l312-
1;SOC_IF_ERROR_RETURN(l311(l1,l321,l313));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,
l9);l290(l1,l14,l321,0x4000,0);SOC_IF_ERROR_RETURN(l165(l1,MEM_BLOCK_ANY,l321
,l312,l14));SOC_IF_ERROR_RETURN(l306(l1,l321,l14));do{int l319 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l312),1)<<1;int l228 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l321),1)*2+(l319&1);if((l318)){l319++;}
ALPM_TCAM_PIVOT((l1),l228) = ALPM_TCAM_PIVOT((l1),l319);if(ALPM_TCAM_PIVOT((
l1),l228)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l228)) = l228;}
ALPM_TCAM_PIVOT((l1),l319) = NULL;}while(0);}else{l290(l1,l14,l313,0x4000,0);
SOC_IF_ERROR_RETURN(l165(l1,MEM_BLOCK_ANY,l313,l312,l14));SOC_IF_ERROR_RETURN
(l306(l1,l313,l14));do{int l314,l315;l314 = soc_alpm_physical_idx((l1),
L3_DEFIPm,(l312),1);l315 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l313),1);
ALPM_TCAM_PIVOT(l1,l315<<1) = ALPM_TCAM_PIVOT(l1,l314<<1);ALPM_TCAM_PIVOT(l1,
(l315<<1)+1) = ALPM_TCAM_PIVOT(l1,(l314<<1)+1);if(ALPM_TCAM_PIVOT((l1),l315<<
1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l315<<1)) = l315<<1;}if(
ALPM_TCAM_PIVOT((l1),(l315<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(
l315<<1)+1)) = (l315<<1)+1;}ALPM_TCAM_PIVOT(l1,l314<<1) = NULL;
ALPM_TCAM_PIVOT(l1,(l314<<1)+1) = NULL;}while(0);}}else{l312 = (l43[(l1)][(
l119)].l37);SOC_IF_ERROR_RETURN(l311(l1,l312,l313));VRF_PIVOT_SHIFT_INC(l1,
MAX_VRF_ID,l9);}(l43[(l1)][(l119)].l36)-= 1;(l43[(l1)][(l119)].l37)-= 1;
return(SOC_E_NONE);}static int l322(int l1,int l119,int l9,void*l14,int*l323)
{int l324;int l325;int l326;int l327;int l312;uint32 l317,l318;int l127;if((
l43[(l1)][(l119)].l39) == 0){l327 = ((3*(64+32+2+1))-1);if(soc_alpm_mode_get(
l1) == SOC_ALPM_MODE_PARALLEL){if(l119<= (((3*(64+32+2+1))/3)-1)){l327 = (((3
*(64+32+2+1))/3)-1);}}while((l43[(l1)][(l327)].next)>l119){l327 = (l43[(l1)][
(l327)].next);}l325 = (l43[(l1)][(l327)].next);if(l325!= -1){(l43[(l1)][(l325
)].l38) = l119;}(l43[(l1)][(l119)].next) = (l43[(l1)][(l327)].next);(l43[(l1)
][(l119)].l38) = l327;(l43[(l1)][(l327)].next) = l119;(l43[(l1)][(l119)].l40)
= ((l43[(l1)][(l327)].l40)+1)/2;(l43[(l1)][(l327)].l40)-= (l43[(l1)][(l119)].
l40);(l43[(l1)][(l119)].l36) = (l43[(l1)][(l327)].l37)+(l43[(l1)][(l327)].l40
)+1;(l43[(l1)][(l119)].l37) = (l43[(l1)][(l119)].l36)-1;(l43[(l1)][(l119)].
l39) = 0;}else if(!l9){l312 = (l43[(l1)][(l119)].l36);if((l127 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l312,l14))<0){return l127;}l317 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l73));l318 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l74));if((l317 == 0)||(l318 == 0)){*l323 = (
l312<<1)+((l318 == 0)?1:0);return(SOC_E_NONE);}l312 = (l43[(l1)][(l119)].l37)
;if((l127 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l312,l14))<0){return l127;}l317 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l95[(
l1)]->l73));l318 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l95[(l1)]->l74));if((l317 == 0)||(l318 == 0)){*l323 = (
l312<<1)+((l318 == 0)?1:0);return(SOC_E_NONE);}}l326 = l119;while((l43[(l1)][
(l326)].l40) == 0){l326 = (l43[(l1)][(l326)].next);if(l326 == -1){l326 = l119
;break;}}while((l43[(l1)][(l326)].l40) == 0){l326 = (l43[(l1)][(l326)].l38);
if(l326 == -1){if((l43[(l1)][(l119)].l39) == 0){l324 = (l43[(l1)][(l119)].l38
);l325 = (l43[(l1)][(l119)].next);if(-1!= l324){(l43[(l1)][(l324)].next) = 
l325;}if(-1!= l325){(l43[(l1)][(l325)].l38) = l324;}}return(SOC_E_FULL);}}
while(l326>l119){l325 = (l43[(l1)][(l326)].next);SOC_IF_ERROR_RETURN(l320(l1,
l325,l9));(l43[(l1)][(l326)].l40)-= 1;(l43[(l1)][(l325)].l40)+= 1;l326 = l325
;}while(l326<l119){SOC_IF_ERROR_RETURN(l316(l1,l326,l9));(l43[(l1)][(l326)].
l40)-= 1;l324 = (l43[(l1)][(l326)].l38);(l43[(l1)][(l324)].l40)+= 1;l326 = 
l324;}(l43[(l1)][(l119)].l39)+= 1;(l43[(l1)][(l119)].l40)-= 1;(l43[(l1)][(
l119)].l37)+= 1;*l323 = (l43[(l1)][(l119)].l37)<<((l9)?0:1);sal_memcpy(l14,
soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);return(
SOC_E_NONE);}static int l328(int l1,int l119,int l9,void*l14,int l329){int
l324;int l325;int l312;int l313;uint32 l330[SOC_MAX_MEM_FIELD_WORDS];uint32
l331[SOC_MAX_MEM_FIELD_WORDS];uint32 l332[SOC_MAX_MEM_FIELD_WORDS];void*l333;
int l127;int l334,l155;l312 = (l43[(l1)][(l119)].l37);l313 = l329;if(!l9){
l313>>= 1;if((l127 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l312,l330))<0){return
l127;}if((l127 = READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
soc_alpm_physical_idx(l1,L3_DEFIPm,l312,1),l331))<0){return l127;}if((l127 = 
READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,soc_alpm_physical_idx(l1,L3_DEFIPm,
l313,1),l332))<0){return l127;}l333 = (l313 == l312)?l330:l14;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l330),(l95[(
l1)]->l74))){l155 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l331,
BPM_LENGTH1f);if(l329&1){l127 = soc_alpm_lpm_ip4entry1_to_1(l1,l330,l333,
PRESERVE_HIT);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l332,BPM_LENGTH1f,
l155);}else{l127 = soc_alpm_lpm_ip4entry1_to_0(l1,l330,l333,PRESERVE_HIT);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l332,BPM_LENGTH0f,l155);}l334 = (
l312<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l330),(l95[(l1)]->l74),(0));}else{l155 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l331,BPM_LENGTH0f);if(l329&1){l127 = 
soc_alpm_lpm_ip4entry0_to_1(l1,l330,l333,PRESERVE_HIT);soc_mem_field32_set(l1
,L3_DEFIP_AUX_TABLEm,l332,BPM_LENGTH1f,l155);}else{l127 = 
soc_alpm_lpm_ip4entry0_to_0(l1,l330,l333,PRESERVE_HIT);soc_mem_field32_set(l1
,L3_DEFIP_AUX_TABLEm,l332,BPM_LENGTH0f,l155);}l334 = l312<<1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l330),(l95[(
l1)]->l73),(0));(l43[(l1)][(l119)].l39)-= 1;(l43[(l1)][(l119)].l40)+= 1;(l43[
(l1)][(l119)].l37)-= 1;}l334 = soc_alpm_physical_idx(l1,L3_DEFIPm,l334,0);
l329 = soc_alpm_physical_idx(l1,L3_DEFIPm,l329,0);ALPM_TCAM_PIVOT(l1,l329) = 
ALPM_TCAM_PIVOT(l1,l334);if(ALPM_TCAM_PIVOT(l1,l329)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l1,l329)) = l329;}ALPM_TCAM_PIVOT(l1,l334) = NULL;if((l127 = 
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ALL,soc_alpm_physical_idx(l1,L3_DEFIPm
,l313,1),l332))<0){return l127;}if(l313!= l312){l290(l1,l333,l313,0x4000,0);
if((l127 = l165(l1,MEM_BLOCK_ANY,l313,l313,l333))<0){return l127;}if((l127 = 
l306(l1,l313,l333))<0){return l127;}}l290(l1,l330,l312,0x4000,0);if((l127 = 
l165(l1,MEM_BLOCK_ANY,l312,l312,l330))<0){return l127;}if((l127 = l306(l1,
l312,l330))<0){return l127;}}else{(l43[(l1)][(l119)].l39)-= 1;(l43[(l1)][(
l119)].l40)+= 1;(l43[(l1)][(l119)].l37)-= 1;if(l313!= l312){if((l127 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l312,l330))<0){return l127;}l290(l1,l330,l313
,0x4000,0);if((l127 = l165(l1,MEM_BLOCK_ANY,l313,l312,l330))<0){return l127;}
if((l127 = l306(l1,l313,l330))<0){return l127;}}l329 = soc_alpm_physical_idx(
l1,L3_DEFIPm,l313,1);l334 = soc_alpm_physical_idx(l1,L3_DEFIPm,l312,1);
ALPM_TCAM_PIVOT(l1,l329<<1) = ALPM_TCAM_PIVOT(l1,l334<<1);ALPM_TCAM_PIVOT(l1,
l334<<1) = NULL;if(ALPM_TCAM_PIVOT(l1,l329<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l1,l329<<1)) = l329<<1;}sal_memcpy(l330,soc_mem_entry_null(l1
,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);l290(l1,l330,l312,0x4000,0);
if((l127 = l165(l1,MEM_BLOCK_ANY,l312,l312,l330))<0){return l127;}if((l127 = 
l306(l1,l312,l330))<0){return l127;}}if((l43[(l1)][(l119)].l39) == 0){l324 = 
(l43[(l1)][(l119)].l38);assert(l324!= -1);l325 = (l43[(l1)][(l119)].next);(
l43[(l1)][(l324)].next) = l325;(l43[(l1)][(l324)].l40)+= (l43[(l1)][(l119)].
l40);(l43[(l1)][(l119)].l40) = 0;if(l325!= -1){(l43[(l1)][(l325)].l38) = l324
;}(l43[(l1)][(l119)].next) = -1;(l43[(l1)][(l119)].l38) = -1;(l43[(l1)][(l119
)].l36) = -1;(l43[(l1)][(l119)].l37) = -1;}return(l127);}int
soc_alpm_lpm_vrf_get(int l20,void*lpm_entry,int*l27,int*l335){int l148;if(((
l95[(l20)]->l77)!= NULL)){l148 = soc_L3_DEFIPm_field32_get(l20,lpm_entry,
VRF_ID_0f);*l335 = l148;if(soc_L3_DEFIPm_field32_get(l20,lpm_entry,
VRF_ID_MASK0f)){*l27 = l148;}else if(!soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),(l95[(l20)]->l79))){*l27 = 
SOC_L3_VRF_GLOBAL;*l335 = SOC_VRF_MAX(l20)+1;}else{*l27 = SOC_L3_VRF_OVERRIDE
;}}else{*l27 = SOC_L3_VRF_DEFAULT;}return(SOC_E_NONE);}static int l34(int l1,
void*entry,int*l16){int l119;int l127;int l9;uint32 l128;int l148;int l336;l9
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(
l95[(l1)]->l63));if(l9){l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l95[(l1)]->l61));if((l127 = _ipmask2pfx(
l128,&l119))<0){return(l127);}l128 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l95[(l1)]->l62));if(l119){if(l128!= 
0xffffffff){return(SOC_E_PARAM);}l119+= 32;}else{if((l127 = _ipmask2pfx(l128,
&l119))<0){return(l127);}}l119+= 33;}else{l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l95[
(l1)]->l61));if((l127 = _ipmask2pfx(l128,&l119))<0){return(l127);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,entry,&l148,&l127));l336 = 
soc_alpm_mode_get(l1);switch(l148){case SOC_L3_VRF_GLOBAL:if(l336 == 
SOC_ALPM_MODE_PARALLEL){*l16 = l119+((3*(64+32+2+1))/3);}else{*l16 = l119;}
break;case SOC_L3_VRF_OVERRIDE:*l16 = l119+2*((3*(64+32+2+1))/3);break;
default:if(l336 == SOC_ALPM_MODE_PARALLEL){*l16 = l119;}else{*l16 = l119+((3*
(64+32+2+1))/3);}break;}return(SOC_E_NONE);}static int l13(int l1,void*l7,
void*l14,int*l15,int*l16,int*l9){int l127;int l33;int l120;int l119 = 0;l33 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l95[(l1
)]->l63));if(l33){if(!(l33 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIPm)),(l7),(l95[(l1)]->l64)))){return(SOC_E_PARAM);}}*l9 = l33;l34(
l1,l7,&l119);*l16 = l119;if(l294(l1,l7,l119,&l120) == SOC_E_NONE){*l15 = l120
;if((l127 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,(*l9)?*l15:(*l15>>1),l14))<0){
return l127;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}static int l2
(int l1){int l305;int l170;int l283;int l337;uint32 l338 = 0;uint32 l336;if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}if((l336 = 
soc_property_get(l1,spn_L3_ALPM_ENABLE,0))){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l1,&l338));soc_reg_field_set(l1,
L3_DEFIP_RPF_CONTROLr,&l338,LPM_MODEf,1);if(l336 == SOC_ALPM_MODE_PARALLEL){
soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l338,LOOKUP_MODEf,1);}else{
soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l338,LOOKUP_MODEf,0);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l1,l338));l338 = 0;if(
SOC_URPF_STATUS_GET(l1)){soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l338,
URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l338,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l338,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l338,
URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l1,l338))
;l338 = 0;if(l336 == SOC_ALPM_MODE_PARALLEL){if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l338,TCAM2_SELf,1);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l338,TCAM3_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM4_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM6_SELf,3);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM7_SELf,3);}else{soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM4_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM5_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM6_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l338,TCAM4_SELf,2);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l338,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM6_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l338,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l1,l338));if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l339 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l1,&l339));soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&
l339,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l339,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l339,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l339,
V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l1,l339))
;}}l305 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l1);l337 = sizeof(l41)*(l305);if(
(l43[(l1)]!= NULL)){if(soc_alpm_deinit(l1)<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}}l95[l1] = sal_alloc(sizeof(l93),"lpm_field_state");if(NULL == 
l95[l1]){SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}(l95[l1])->l45 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,CLASS_ID0f);(l95[l1])->l46 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,CLASS_ID1f);(l95[l1])->l47 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD0f);(l95[l1])->l48 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD1f);(l95[l1])->l49 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP0f);(l95[l1])->l50 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP1f);(l95[l1])->l51 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT0f);(l95[l1])->l52 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT1f);(l95[l1])->l53 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR0f);(l95[l1])->l54 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR1f);(l95[l1])->l55 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE0f);(l95[l1])->l56 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE1f);(l95[l1])->l57 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,HIT0f);(l95[l1])->l58 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,HIT1f);(l95[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR0f);(l95[l1])->l60 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR1f);(l95[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK0f);(l95[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK1f);(l95[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE0f);(l95[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE1f);(l95[l1])->l65 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK0f);(l95[l1])->l66 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK1f);(l95[l1])->l67 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX0f);(l95[l1])->l68 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX1f);(l95[l1])->l69 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI0f);(l95[l1])->l70 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI1f);(l95[l1])->l71 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE0f);(l95[l1])->l72 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE1f);(l95[l1])->l73 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID0f);(l95[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID1f);(l95[l1])->l75 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_0f);(l95[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_1f);(l95[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK0f);(l95[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK1f);(l95[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH0f);(l95[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH1f);(l95[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX0f);(l95[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX1f);(l95[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR0f);(l95[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR1f);(l95[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS0f);(l95[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS1f);(l95[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l95[l1])->
l88 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l95[l1
])->l89 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l95[l1]
)->l90 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(l95[l1])
->l91 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,ENTRY_TYPE_MASK0f);(l95[l1])->l92 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ENTRY_TYPE_MASK1f);(l43[(l1)]) = sal_alloc
(l337,"LPM prefix info");if(NULL == (l43[(l1)])){sal_free(l95[l1]);l95[l1] = 
NULL;SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}sal_memset((l43[(l1)]),0,
l337);for(l170 = 0;l170<l305;l170++){(l43[(l1)][(l170)].l36) = -1;(l43[(l1)][
(l170)].l37) = -1;(l43[(l1)][(l170)].l38) = -1;(l43[(l1)][(l170)].next) = -1;
(l43[(l1)][(l170)].l39) = 0;(l43[(l1)][(l170)].l40) = 0;}l283 = 
soc_mem_index_count(l1,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l1)){l283>>= 1;}if(
l336 == SOC_ALPM_MODE_PARALLEL){(l43[(l1)][(((3*(64+32+2+1))-1))].l37) = (
l283>>1)-1;(l43[(l1)][(((((3*(64+32+2+1))/3)-1)))].l40) = l283>>1;(l43[(l1)][
((((3*(64+32+2+1))-1)))].l40) = (l283-(l43[(l1)][(((((3*(64+32+2+1))/3)-1)))]
.l40));}else{(l43[(l1)][((((3*(64+32+2+1))-1)))].l40) = l283;}if((l106[(l1)])
!= NULL){if(l114((l106[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}(l106[(l1)]) = NULL;}if(l112(l1,l283*2,l283,&(l106[(l1)]))<0)
{SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l1);return(
SOC_E_NONE);}static int l3(int l1){if(!soc_feature(l1,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);if((l106[(l1)])!= NULL){l114((
l106[(l1)]));(l106[(l1)]) = NULL;}if((l43[(l1)]!= NULL)){sal_free(l95[l1]);
l95[l1] = NULL;sal_free((l43[(l1)]));(l43[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(
l1);return(SOC_E_NONE);}static int l4(int l1,void*l5,int*l340){int l119;int
index;int l9;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int l127 = SOC_E_NONE;int
l341 = 0;sal_memcpy(l14,soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(
l1,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l1);l127 = l13(l1,l5,l14,&index,&l119,&l9)
;if(l127 == SOC_E_NOT_FOUND){l127 = l322(l1,l119,l9,l14,&index);if(l127<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l127);}}else{l341 = 1;}*l340 = index;if(l127 == 
SOC_E_NONE){if(!l9){if(index&1){l127 = soc_alpm_lpm_ip4entry0_to_1(l1,l5,l14,
PRESERVE_HIT);}else{l127 = soc_alpm_lpm_ip4entry0_to_0(l1,l5,l14,PRESERVE_HIT
);}if(l127<0){SOC_ALPM_LPM_UNLOCK(l1);return(l127);}l5 = (void*)l14;index>>= 
1;}soc_alpm_lpm_state_dump(l1);LOG_BSL_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\nsoc_alpm_lpm_insert: %d %d\n"),index,l119));if(!l341){l290(l1,l5,index,
0x4000,0);}l127 = l165(l1,MEM_BLOCK_ANY,index,index,l5);if(l127>= 0){l127 = 
l306(l1,index,l5);}}SOC_ALPM_LPM_UNLOCK(l1);return(l127);}static int l6(int l1
,void*l7){int l119;int index;int l9;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int
l127 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l127 = l13(l1,l7,l14,&index,&l119,&l9
);if(l127 == SOC_E_NONE){LOG_BSL_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\nsoc_alpm_lpm_delete: %d %d\n"),index,l119));l293(l1,l7,index);l127 = l328(
l1,l119,l9,l14,index);}soc_alpm_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);
return(l127);}static int l17(int l1,void*l7,void*l14,int*l15){int l119;int
l127;int l9;SOC_ALPM_LPM_LOCK(l1);l127 = l13(l1,l7,l14,l15,&l119,&l9);
SOC_ALPM_LPM_UNLOCK(l1);return(l127);}static int l8(int l20,void*l7,int l9,
int l10,int l11,defip_aux_scratch_entry_t*l12){uint32 l128;uint32 l342[4] = {
0,0,0,0};int l119 = 0;int l127 = SOC_E_NONE;l128 = soc_mem_field32_get(l20,
L3_DEFIPm,l7,VALID0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,
VALIDf,l128);l128 = soc_mem_field32_get(l20,L3_DEFIPm,l7,MODE0f);
soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,MODEf,l128);l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,ENTRY_TYPE0f);soc_mem_field32_set(l20,
L3_DEFIP_AUX_SCRATCHm,l12,ENTRY_TYPEf,0);l128 = soc_mem_field32_get(l20,
L3_DEFIPm,l7,GLOBAL_ROUTE0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,
l12,GLOBAL_ROUTEf,l128);l128 = soc_mem_field32_get(l20,L3_DEFIPm,l7,ECMP0f);
soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,ECMPf,l128);l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,ECMP_PTR0f);soc_mem_field32_set(l20,
L3_DEFIP_AUX_SCRATCHm,l12,ECMP_PTRf,l128);l128 = soc_mem_field32_get(l20,
L3_DEFIPm,l7,NEXT_HOP_INDEX0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,
l12,NEXT_HOP_INDEXf,l128);l128 = soc_mem_field32_get(l20,L3_DEFIPm,l7,PRI0f);
soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,PRIf,l128);l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,RPE0f);soc_mem_field32_set(l20,
L3_DEFIP_AUX_SCRATCHm,l12,RPEf,l128);l128 =soc_mem_field32_get(l20,L3_DEFIPm,
l7,VRF_ID_0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,VRFf,l128);
soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,DB_TYPEf,l10);l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,DST_DISCARD0f);soc_mem_field32_set(l20,
L3_DEFIP_AUX_SCRATCHm,l12,DST_DISCARDf,l128);l128 = soc_mem_field32_get(l20,
L3_DEFIPm,l7,CLASS_ID0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,
CLASS_IDf,l128);if(l9){l342[2] = soc_mem_field32_get(l20,L3_DEFIPm,l7,
IP_ADDR0f);l342[3] = soc_mem_field32_get(l20,L3_DEFIPm,l7,IP_ADDR1f);}else{
l342[0] = soc_mem_field32_get(l20,L3_DEFIPm,l7,IP_ADDR0f);}soc_mem_field_set(
l20,L3_DEFIP_AUX_SCRATCHm,(uint32*)l12,IP_ADDRf,(uint32*)l342);if(l9){l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,IP_ADDR_MASK0f);if((l127 = _ipmask2pfx(
l128,&l119))<0){return(l127);}l128 = soc_mem_field32_get(l20,L3_DEFIPm,l7,
IP_ADDR_MASK1f);if(l119){if(l128!= 0xffffffff){return(SOC_E_PARAM);}l119+= 32
;}else{if((l127 = _ipmask2pfx(l128,&l119))<0){return(l127);}}}else{l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,IP_ADDR_MASK0f);if((l127 = _ipmask2pfx(
l128,&l119))<0){return(l127);}}soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,
l12,IP_LENGTHf,l119);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,
REPLACE_LENf,l11);return(SOC_E_NONE);}int _soc_alpm_aux_op(int l1,
_soc_aux_op_t l343,defip_aux_scratch_entry_t*l12,int l344,int*l141,int*l142,
int*bucket_index){uint32 l338 = 0;uint32 l345 = 0;int l346;soc_timeout_t l347
;int l127 = SOC_E_NONE;int l348 = 0;if(l344){SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_SCRATCHm(l1,MEM_BLOCK_ANY,0,l12));}l349:l338 = 0;switch(
l343){case INSERT_PROPAGATE:l346 = 0;break;case DELETE_PROPAGATE:l346 = 1;
break;case PREFIX_LOOKUP:l346 = 2;break;case HITBIT_REPLACE:l346 = 3;break;
default:return SOC_E_PARAM;}soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l338,
OPCODEf,l346);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l338,STARTf,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l338));soc_timeout_init(&l347
,50000,5);l346 = 0;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l1,&l338));
l346 = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l338,DONEf);if(l346 == 1){l127
= SOC_E_NONE;break;}if(soc_timeout_check(&l347)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l1,&l338));l346 = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRLr,l338,DONEf);if(l346 == 1){l127 = SOC_E_NONE;}else{LOG_WARN
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d : DEFIP AUX Operation timeout\n"),l1
));l127 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l127)){if(
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l338,ERRORf)){soc_reg_field_set(l1,
L3_DEFIP_AUX_CTRLr,&l338,STARTf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&
l338,ERRORf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l338,DONEf,0);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l338));LOG_BSL_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: DEFIP AUX Operation encountered "
"parity error !!\n"),l1));l348++;if(SOC_CONTROL(l1)->alpm_bulk_retry){
sal_sem_take(SOC_CONTROL(l1)->alpm_bulk_retry,1000000);}if(l348<5){LOG_BSL_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: Retry DEFIP AUX Operation..\n"),l1))
;goto l349;}else{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l1));return SOC_E_INTERNAL;}}if(l343 == PREFIX_LOOKUP){if(l141&&l142){*l141 = 
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l338,HITf);*l142 = soc_reg_field_get(
l1,L3_DEFIP_AUX_CTRLr,l338,BKT_INDEXf);SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRL_1r(l1,&l345));*bucket_index = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRL_1r,l345,BKT_PTRf);}}}return l127;}static int l19(int l20,
void*lpm_entry,void*l21,void*l22,soc_mem_t l23,uint32 l24,uint32*l350){uint32
l128;uint32 l342[4] = {0,0};int l119 = 0;int l127 = SOC_E_NONE;int l9;uint32
l25 = 0;l9 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,MODE0f);sal_memset(
l21,0,soc_mem_entry_words(l20,l23)*4);l128 = soc_mem_field32_get(l20,
L3_DEFIPm,lpm_entry,HIT0f);soc_mem_field32_set(l20,l23,l21,HITf,l128);l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,VALID0f);soc_mem_field32_set(l20,
l23,l21,VALIDf,l128);l128 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,
ECMP0f);soc_mem_field32_set(l20,l23,l21,ECMPf,l128);l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set(
l20,l23,l21,ECMP_PTRf,l128);l128 = soc_mem_field32_get(l20,L3_DEFIPm,
lpm_entry,NEXT_HOP_INDEX0f);soc_mem_field32_set(l20,l23,l21,NEXT_HOP_INDEXf,
l128);l128 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,PRI0f);
soc_mem_field32_set(l20,l23,l21,PRIf,l128);l128 = soc_mem_field32_get(l20,
L3_DEFIPm,lpm_entry,RPE0f);soc_mem_field32_set(l20,l23,l21,RPEf,l128);l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(l20,l23,l21,DST_DISCARDf,l128);l128 = soc_mem_field32_get
(l20,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);soc_mem_field32_set(l20,l23,l21,
SRC_DISCARDf,l128);l128 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,
CLASS_ID0f);soc_mem_field32_set(l20,l23,l21,CLASS_IDf,l128);l342[0] = 
soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,IP_ADDR0f);if(l9){l342[1] = 
soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,IP_ADDR1f);}soc_mem_field_set(l20
,l23,(uint32*)l21,KEYf,(uint32*)l342);if(l9){l128 = soc_mem_field32_get(l20,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l127 = _ipmask2pfx(l128,&l119))<0){
return(l127);}l128 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f);if(l119){if(l128!= 0xffffffff){return(SOC_E_PARAM);}l119+= 32
;}else{if((l127 = _ipmask2pfx(l128,&l119))<0){return(l127);}}}else{l128 = 
soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l127 = 
_ipmask2pfx(l128,&l119))<0){return(l127);}}if((l119 == 0)&&(l342[0] == 0)&&(
l342[1] == 0)){l25 = 1;}if(l350!= NULL){*l350 = l25;}soc_mem_field32_set(l20,
l23,l21,LENGTHf,l119);if(l22 == NULL){return(SOC_E_NONE);}if(
SOC_URPF_STATUS_GET(l20)){sal_memset(l22,0,soc_mem_entry_words(l20,l23)*4);
sal_memcpy(l22,l21,soc_mem_entry_words(l20,l23)*4);soc_mem_field32_set(l20,
l23,l22,DST_DISCARDf,0);soc_mem_field32_set(l20,l23,l22,RPEf,0);
soc_mem_field32_set(l20,l23,l22,SRC_DISCARDf,l24&SOC_ALPM_RPF_SRC_DISCARD);
soc_mem_field32_set(l20,l23,l22,DEFAULTROUTEf,l25);}return(SOC_E_NONE);}
static int l26(int l20,void*l21,soc_mem_t l23,int l9,int l27,int l28,int index
,void*lpm_entry){uint32 l128;uint32 l342[4] = {0,0};uint32 l119 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_words(l20,L3_DEFIPm)*4);l128 = 
soc_mem_field32_get(l20,l23,l21,HITf);soc_mem_field32_set(l20,L3_DEFIPm,
lpm_entry,HIT0f,l128);if(l9){soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
HIT1f,l128);}l128 = soc_mem_field32_get(l20,l23,l21,VALIDf);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VALID0f,l128);if(l9){
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VALID1f,l128);}l128 = 
soc_mem_field32_get(l20,l23,l21,ECMPf);soc_mem_field32_set(l20,L3_DEFIPm,
lpm_entry,ECMP0f,l128);l128 = soc_mem_field32_get(l20,l23,l21,ECMP_PTRf);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,ECMP_PTR0f,l128);l128 = 
soc_mem_field32_get(l20,l23,l21,NEXT_HOP_INDEXf);soc_mem_field32_set(l20,
L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f,l128);l128 = soc_mem_field32_get(l20,l23
,l21,PRIf);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,PRI0f,l128);l128 = 
soc_mem_field32_get(l20,l23,l21,RPEf);soc_mem_field32_set(l20,L3_DEFIPm,
lpm_entry,RPE0f,l128);l128 = soc_mem_field32_get(l20,l23,l21,DST_DISCARDf);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,DST_DISCARD0f,l128);l128 = 
soc_mem_field32_get(l20,l23,l21,SRC_DISCARDf);soc_mem_field32_set(l20,
L3_DEFIPm,lpm_entry,SRC_DISCARD0f,l128);l128 = soc_mem_field32_get(l20,l23,
l21,CLASS_IDf);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,CLASS_ID0f,l128);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,ALG_BKT_PTR0f,l28);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,ALG_HIT_IDX0f,index);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,MODE_MASK0f,3);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l9){
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(l20,
l23,l21,KEYf,l342);if(l9){soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
IP_ADDR1f,l342[1]);}soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,IP_ADDR0f,
l342[0]);l128 = soc_mem_field32_get(l20,l23,l21,LENGTHf);if(l9){if(l128>= 32)
{l119 = 0xffffffff;soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f
,l119);l119 = ~(((l128-32) == 32)?0:(0xffffffff)>>(l128-32));
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l119);}else{l119 = 
~(0xffffffff>>l128);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l119);}}else{assert(l128<= 32);l119 = ~(((l128) == 32)?0:(
0xffffffff)>>(l128));soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,l119);}if(l27 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l20,
L3_DEFIPm,lpm_entry,GLOBAL_HIGH0f,1);soc_mem_field32_set(l20,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else if(l27 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry
,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_0f,0);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_0f,l27);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(l20));}
return(SOC_E_NONE);}int soc_alpm_warmboot_pivot_add(int l20,int l9,void*
lpm_entry,int l351,int l352){int l127 = SOC_E_NONE;uint32 key[2] = {0,0};
alpm_pivot_t*l203 = NULL;alpm_bucket_handle_t*l212 = NULL;int l148 = 0,l27 = 
0;uint32 l353;trie_t*l249 = NULL;uint32 prefix[5] = {0};int l25 = 0;l127 = 
l133(l20,lpm_entry,prefix,&l353,&l25);SOC_IF_ERROR_RETURN(l127);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l20,lpm_entry,&l148,&l27));l351 = 
soc_alpm_physical_idx(l20,L3_DEFIPm,l351,l9);l212 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l212 == NULL){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l20,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l212,0,sizeof(*l212));
l203 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l203 == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l20,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l212);return SOC_E_MEMORY;}sal_memset(l203,0
,sizeof(*l203));PIVOT_BUCKET_HANDLE(l203) = l212;if(l9){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l203));key[0] = 
soc_L3_DEFIPm_field32_get(l20,lpm_entry,IP_ADDR0f);key[1] = 
soc_L3_DEFIPm_field32_get(l20,lpm_entry,IP_ADDR1f);}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l203));key[0] = soc_L3_DEFIPm_field32_get
(l20,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l203) = l352;PIVOT_TCAM_INDEX(
l203) = l351;if(l148!= SOC_L3_VRF_OVERRIDE){if(l9 == 0){l249 = 
VRF_PIVOT_TRIE_IPV4(l20,l27);if(l249 == NULL){trie_init(_MAX_KEY_LEN_48_,&
VRF_PIVOT_TRIE_IPV4(l20,l27));l249 = VRF_PIVOT_TRIE_IPV4(l20,l27);}}else{l249
= VRF_PIVOT_TRIE_IPV6(l20,l27);if(l249 == NULL){trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l20,l27));l249 = VRF_PIVOT_TRIE_IPV6(l20,l27);}}
sal_memcpy(l203->key,prefix,sizeof(prefix));l203->len = l353;l127 = 
trie_insert(l249,l203->key,NULL,l203->len,(trie_node_t*)l203);if(SOC_FAILURE(
l127)){sal_free(l212);sal_free(l203);return l127;}}ALPM_TCAM_PIVOT(l20,l351) = 
l203;PIVOT_BUCKET_VRF(l203) = l27;PIVOT_BUCKET_IPV6(l203) = l9;
PIVOT_BUCKET_ENT_CNT_UPDATE(l203);if(key[0] == 0&&key[1] == 0){
PIVOT_BUCKET_DEF(l203) = TRUE;}VRF_PIVOT_REF_INC(l20,l27,l9);return l127;}
static int l354(int l20,int l9,void*lpm_entry,void*l21,soc_mem_t l23,int l351
,int l352,int l355){int l356;int l27;int l127 = SOC_E_NONE;int l25 = 0;uint32
prefix[5] = {0,0,0,0,0};uint32 l140;void*l357 = NULL;trie_t*l358 = NULL;
trie_t*l206 = NULL;trie_node_t*l208 = NULL;payload_t*l359 = NULL;payload_t*
l210 = NULL;alpm_pivot_t*l145 = NULL;if((NULL == lpm_entry)||(NULL == l21)){
return SOC_E_PARAM;}if(l9){if(!(l9 = soc_mem_field32_get(l20,L3_DEFIPm,
lpm_entry,MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l20,lpm_entry,&l356,&l27));l23 = (l9)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l357 = sal_alloc(sizeof(
defip_entry_t),"Temp lpm_entr");if(NULL == l357){return SOC_E_MEMORY;}
SOC_IF_ERROR_RETURN(l26(l20,l21,l23,l9,l356,l352,l351,l357));l127 = l133(l20,
l357,prefix,&l140,&l25);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l20,"prefix create failed\n")));return l127;}sal_free(l357);l145 = 
ALPM_TCAM_PIVOT(l20,l351);l358 = PIVOT_BUCKET_TRIE(l145);l359 = sal_alloc(
sizeof(payload_t),"Payload for Key");if(NULL == l359){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l20,"Unable to allocate memory for trie node.\n")
));return SOC_E_MEMORY;}l210 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(NULL == l210){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l20,"Unable to allocate memory for pfx trie node\n")));sal_free(
l359);return SOC_E_MEMORY;}sal_memset(l359,0,sizeof(*l359));sal_memset(l210,0
,sizeof(*l210));l359->key[0] = prefix[0];l359->key[1] = prefix[1];l359->key[2
] = prefix[2];l359->key[3] = prefix[3];l359->key[4] = prefix[4];l359->len = 
l140;l359->index = l355;sal_memcpy(l210,l359,sizeof(*l359));l127 = 
trie_insert(l358,prefix,NULL,l140,(trie_node_t*)l359);if(SOC_FAILURE(l127)){
goto l360;}if(l9){l206 = VRF_PREFIX_TRIE_IPV6(l20,l27);}else{l206 = 
VRF_PREFIX_TRIE_IPV4(l20,l27);}if(!l25){l127 = trie_insert(l206,prefix,NULL,
l140,(trie_node_t*)l210);if(SOC_FAILURE(l127)){goto l225;}}return l127;l225:(
void)trie_delete(l358,prefix,l140,&l208);l359 = (payload_t*)l208;l360:
sal_free(l359);sal_free(l210);return l127;}static int l361(int l20,int l33,
int l27,int l129,int bkt_ptr){int l127 = SOC_E_NONE;uint32 l140;uint32 key[2]
= {0,0};trie_t*l362 = NULL;payload_t*l239 = NULL;defip_entry_t*lpm_entry = 
NULL;lpm_entry = sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(
lpm_entry == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l20,
"unable to allocate memory for LPM entry\n")));return SOC_E_MEMORY;}l29(l20,
key,0,l27,l33,lpm_entry,0,1);if(l27 == SOC_VRF_MAX(l20)+1){
soc_L3_DEFIPm_field32_set(l20,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l20,lpm_entry,DEFAULT_MISS0f,1);}
soc_L3_DEFIPm_field32_set(l20,lpm_entry,ALG_BKT_PTR0f,bkt_ptr);if(l33 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l20,l27) = lpm_entry;trie_init(_MAX_KEY_LEN_48_,&
VRF_PREFIX_TRIE_IPV4(l20,l27));l362 = VRF_PREFIX_TRIE_IPV4(l20,l27);}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l20,l27) = lpm_entry;trie_init(_MAX_KEY_LEN_144_,
&VRF_PREFIX_TRIE_IPV6(l20,l27));l362 = VRF_PREFIX_TRIE_IPV6(l20,l27);}l239 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(l239 == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l20,
"Unable to allocate memory for pfx trie node \n")));return SOC_E_MEMORY;}
sal_memset(l239,0,sizeof(*l239));l140 = 0;l239->key[0] = key[0];l239->key[1] = 
key[1];l239->len = l140;l127 = trie_insert(l362,key,NULL,l140,&(l239->node));
if(SOC_FAILURE(l127)){sal_free(l239);return l127;}VRF_TRIE_INIT_DONE(l20,l27,
l33,1);return l127;}int soc_alpm_warmboot_prefix_insert(int l20,int l9,void*
lpm_entry,void*l21,int l351,int l352,int l355){int l356;int l27;int l127 = 
SOC_E_NONE;soc_mem_t l23;l351 = soc_alpm_physical_idx(l20,L3_DEFIPm,l351,l9);
l23 = (l9)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l20,lpm_entry,&l356,&l27));if(l356 == 
SOC_L3_VRF_OVERRIDE){return(l127);}if(!VRF_TRIE_INIT_COMPLETED(l20,l27,l9)){
LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l20,"VRF %d is not initialized\n"),
l27));l127 = l361(l20,l9,l27,l351,l352);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l20,"VRF %d/%d trie init \n""failed\n"),l27,l9));
return l127;}LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l20,
"VRF %d/%d trie init completed\n"),l27,l9));}l127 = l354(l20,l9,lpm_entry,l21
,l23,l351,l352,l355);if(l127!= SOC_E_NONE){LOG_BSL_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l20,"unit %d : Route Insertion Failed :%s\n"),l20,soc_errmsg(l127)
));return(l127);}VRF_TRIE_ROUTES_INC(l20,l27,l9);return(l127);}int
soc_alpm_warmboot_bucket_bitmap_set(int l1,int l33,int l252){int l284 = 1;if(
l33){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l284 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l252,l284);return SOC_E_NONE;}int
soc_alpm_warmboot_lpm_reinit_done(int l20){int l129;int l363 = ((3*(64+32+2+1
))-1);int l283 = soc_mem_index_count(l20,L3_DEFIPm);if(SOC_URPF_STATUS_GET(
l20)){l283>>= 1;}if(!soc_alpm_mode_get(l20)){(l43[(l20)][(((3*(64+32+2+1))-1)
)].l38) = -1;for(l129 = ((3*(64+32+2+1))-1);l129>-1;l129--){if(-1 == (l43[(
l20)][(l129)].l36)){continue;}(l43[(l20)][(l129)].l38) = l363;(l43[(l20)][(
l363)].next) = l129;(l43[(l20)][(l363)].l40) = (l43[(l20)][(l129)].l36)-(l43[
(l20)][(l363)].l37)-1;l363 = l129;}(l43[(l20)][(l363)].next) = -1;(l43[(l20)]
[(l363)].l40) = l283-(l43[(l20)][(l363)].l37)-1;}else{(l43[(l20)][(((3*(64+32
+2+1))-1))].l38) = -1;for(l129 = ((3*(64+32+2+1))-1);l129>(((3*(64+32+2+1))-1
)/3);l129--){if(-1 == (l43[(l20)][(l129)].l36)){continue;}(l43[(l20)][(l129)]
.l38) = l363;(l43[(l20)][(l363)].next) = l129;(l43[(l20)][(l363)].l40) = (l43
[(l20)][(l129)].l36)-(l43[(l20)][(l363)].l37)-1;l363 = l129;}(l43[(l20)][(
l363)].next) = -1;(l43[(l20)][(l363)].l40) = l283-(l43[(l20)][(l363)].l37)-1;
l363 = (((3*(64+32+2+1))-1)/3);(l43[(l20)][((((3*(64+32+2+1))-1)/3))].l38) = 
-1;for(l129 = ((((3*(64+32+2+1))-1)/3)-1);l129>-1;l129--){if(-1 == (l43[(l20)
][(l129)].l36)){continue;}(l43[(l20)][(l129)].l38) = l363;(l43[(l20)][(l363)]
.next) = l129;(l43[(l20)][(l363)].l40) = (l43[(l20)][(l129)].l36)-(l43[(l20)]
[(l363)].l37)-1;l363 = l129;}(l43[(l20)][(l363)].next) = -1;(l43[(l20)][(l363
)].l40) = (l283>>1)-(l43[(l20)][(l363)].l37)-1;}return(SOC_E_NONE);}int
soc_alpm_warmboot_lpm_reinit(int l20,int l9,int l129,void*lpm_entry){int l16;
defip_entry_t*l364;if(soc_L3_DEFIPm_field32_get(l20,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(l20,lpm_entry,VALID1f)){l290(l20,lpm_entry,l129,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(l20,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l34(l20,lpm_entry,&l16));if((l43[(l20)][(l16)].l39) == 0)
{(l43[(l20)][(l16)].l36) = l129;(l43[(l20)][(l16)].l37) = l129;}else{(l43[(
l20)][(l16)].l37) = l129;}(l43[(l20)][(l16)].l39)++;if(l9){return(SOC_E_NONE)
;}}else{if(soc_L3_DEFIPm_field32_get(l20,lpm_entry,VALID1f)){l364 = sal_alloc
(sizeof(defip_entry_t),"lpm_entry_hi");soc_alpm_lpm_ip4entry1_to_0(l20,
lpm_entry,l364,TRUE);SOC_IF_ERROR_RETURN(l34(l20,l364,&l16));if((l43[(l20)][(
l16)].l39) == 0){(l43[(l20)][(l16)].l36) = l129;(l43[(l20)][(l16)].l37) = 
l129;}else{(l43[(l20)][(l16)].l37) = l129;}sal_free(l364);(l43[(l20)][(l16)].
l39)++;}}return(SOC_E_NONE);}typedef struct l365{int v4;int v6_64;int v6_128;
int l366;int l367;int l368;int l285;}l369;typedef enum l370{l371 = 0,l372,
l373,l374,l375,l376}l377;static void l378(int l1,alpm_vrf_counter_t*l379){
l379->v4 = soc_mem_index_count(l1,L3_DEFIPm)*2;l379->v6_128 = 
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){l379->v6_64 = l379->v6_128;}else{l379->v6_64 = 
l379->v4>>1;}if(SOC_URPF_STATUS_GET(l1)){l379->v4>>= 1;l379->v6_128>>= 1;l379
->v6_64>>= 1;}}static void l380(int l1,int l148,alpm_vrf_handle_t*l381,l377
l382){alpm_vrf_counter_t*l383;int l170,l384,l385,l386;int l341 = 0;
alpm_vrf_counter_t l379;switch(l382){case l371:LOG_CLI((BSL_META_U(l1,
"\nAdd Counter:\n")));break;case l372:LOG_CLI((BSL_META_U(l1,
"\nDelete Counter:\n")));break;case l373:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - 1:\n")));break;case l374:l378(l1,&l379);LOG_CLI((
BSL_META_U(l1,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l379.v4,
l379.v6_64,l379.v6_128));break;case l375:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l376:LOG_CLI((
BSL_META_U(l1,"\nInternal Debug Counter - LPM Full:\n")));break;default:break
;}LOG_CLI((BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n"))
);LOG_CLI((BSL_META_U(l1,"-----------------------------------------------\n")
));l384 = l385 = l386 = 0;for(l170 = 0;l170<MAX_VRF_ID+1;l170++){int l387,
l388,l389;if(l381[l170].init_done == 0&&l170!= MAX_VRF_ID){continue;}if(l148
!= -1&&l148!= l170){continue;}l341 = 1;switch(l382){case l371:l383 = &l381[
l170].add;break;case l372:l383 = &l381[l170].del;break;case l373:l383 = &l381
[l170].bkt_split;break;case l375:l383 = &l381[l170].lpm_shift;break;case l376
:l383 = &l381[l170].lpm_full;break;case l374:l383 = &l381[l170].pivot_used;
break;default:l383 = &l381[l170].pivot_used;break;}l387 = l383->v4;l388 = 
l383->v6_64;l389 = l383->v6_128;l384+= l387;l385+= l388;l386+= l389;do{
LOG_CLI((BSL_META_U(l1,"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l170 == 
MAX_VRF_ID?-1:l170),(l387),(l388),(l389),((l387+l388+l389)),(l170) == 
MAX_VRF_ID?"GHi":(l170) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l341 == 
0){LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));do{
LOG_CLI((BSL_META_U(l1,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l384),(
l385),(l386),((l384+l385+l386))));}while(0);}return;}int soc_alpm_debug_show(
int l1,int l148,uint32 flags){int l170,l390,l341 = 0;l369*l391;l369 l392;l369
l393;if(l148>(SOC_VRF_MAX(l1)+1)){return SOC_E_PARAM;}l390 = MAX_VRF_ID*
sizeof(l369);l391 = sal_alloc(l390,"_alpm_dbg_cnt");if(l391 == NULL){return
SOC_E_MEMORY;}sal_memset(l391,0,l390);l392.v4 = ALPM_IPV4_BKT_COUNT;l392.
v6_64 = ALPM_IPV6_64_BKT_COUNT;l392.v6_128 = ALPM_IPV6_128_BKT_COUNT;if(!
soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l392.v6_64<<= 1;l392.v6_128
<<= 1;}LOG_CLI((BSL_META_U(l1,"\nBucket Occupancy:\n")));if(flags&(
SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l1,
"\n  BKT/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));}for(
l170 = 0;l170<MAX_PIVOT_COUNT;l170++){alpm_pivot_t*l394 = ALPM_TCAM_PIVOT(l1,
l170);if(l394!= NULL){l369*l395;int l27 = PIVOT_BUCKET_VRF(l394);if(l27<0||
l27>(SOC_VRF_MAX(l1)+1)){continue;}if(l148!= -1&&l148!= l27){continue;}if(
flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l341 = 1;do{LOG_CLI((BSL_META_U(l1,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l170,l27,PIVOT_BUCKET_MIN(l394),
PIVOT_BUCKET_MAX(l394),PIVOT_BUCKET_COUNT(l394),PIVOT_BUCKET_DEF(l394)?"Def":
(l27) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}l395 = &l391[l27];if(
PIVOT_BUCKET_IPV6(l394) == L3_DEFIP_MODE_128){l395->v6_128+= 
PIVOT_BUCKET_COUNT(l394);l395->l368+= l392.v6_128;}else if(PIVOT_BUCKET_IPV6(
l394) == L3_DEFIP_MODE_64){l395->v6_64+= PIVOT_BUCKET_COUNT(l394);l395->l367
+= l392.v6_64;}else{l395->v4+= PIVOT_BUCKET_COUNT(l394);l395->l366+= l392.v4;
}l395->l285 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l341 == 0){
LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}}sal_memset(&l393
,0,sizeof(l393));l341 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){LOG_CLI((
BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));for(l170
= 0;l170<MAX_VRF_ID;l170++){l369*l395;if(l391[l170].l285!= TRUE){continue;}if
(l148!= -1&&l148!= l170){continue;}l341 = 1;l395 = &l391[l170];do{(&l393)->v4
+= (l395)->v4;(&l393)->l366+= (l395)->l366;(&l393)->v6_64+= (l395)->v6_64;(&
l393)->l367+= (l395)->l367;(&l393)->v6_128+= (l395)->v6_128;(&l393)->l368+= (
l395)->l368;}while(0);do{LOG_CLI((BSL_META_U(l1,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l170),(l395
->l366)?(l395->v4)*100/(l395->l366):0,(l395->l366)?(l395->v4)*1000/(l395->
l366)%10:0,(l395->l367)?(l395->v6_64)*100/(l395->l367):0,(l395->l367)?(l395->
v6_64)*1000/(l395->l367)%10:0,(l395->l368)?(l395->v6_128)*100/(l395->l368):0,
(l395->l368)?(l395->v6_128)*1000/(l395->l368)%10:0,((l395->l366+l395->l367+
l395->l368))?((l395->v4+l395->v6_64+l395->v6_128))*100/((l395->l366+l395->
l367+l395->l368)):0,((l395->l366+l395->l367+l395->l368))?((l395->v4+l395->
v6_64+l395->v6_128))*1000/((l395->l366+l395->l367+l395->l368))%10:0,(l170) == 
SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l341 == 0){LOG_CLI((BSL_META_U(l1,
"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l1,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l1,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l393)->l366)?((&l393)->v4)*100/((&l393)->l366):0,((&l393)->l366)?((&l393)->v4
)*1000/((&l393)->l366)%10:0,((&l393)->l367)?((&l393)->v6_64)*100/((&l393)->
l367):0,((&l393)->l367)?((&l393)->v6_64)*1000/((&l393)->l367)%10:0,((&l393)->
l368)?((&l393)->v6_128)*100/((&l393)->l368):0,((&l393)->l368)?((&l393)->
v6_128)*1000/((&l393)->l368)%10:0,(((&l393)->l366+(&l393)->l367+(&l393)->l368
))?(((&l393)->v4+(&l393)->v6_64+(&l393)->v6_128))*100/(((&l393)->l366+(&l393)
->l367+(&l393)->l368)):0,(((&l393)->l366+(&l393)->l367+(&l393)->l368))?(((&
l393)->v4+(&l393)->v6_64+(&l393)->v6_128))*1000/(((&l393)->l366+(&l393)->l367
+(&l393)->l368))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l380(l1,l148,alpm_vrf_handle[l1],l374);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_CNT
){l380(l1,l148,alpm_vrf_handle[l1],l371);l380(l1,l148,alpm_vrf_handle[l1],
l372);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l380(l1,l148,alpm_vrf_handle
[l1],l373);l380(l1,l148,alpm_vrf_handle[l1],l376);l380(l1,l148,
alpm_vrf_handle[l1],l375);}sal_free(l391);return SOC_E_NONE;}int
soc_alpm_bucket_sanity_check(int l1,soc_mem_t l260,int index){int l127 = 
SOC_E_NONE;int l170,l257,l266,l9,l142 = -1;int l148 = 0,l27;uint32 l137 = 0;
uint32 l14[SOC_MAX_MEM_FIELD_WORDS];uint32 l396[SOC_MAX_MEM_FIELD_WORDS];int
l397,l398,l399;defip_entry_t lpm_entry;int l271,l400;soc_mem_t l23;int l191,
l263,l401,l402;int l403 = 0;soc_field_t l404[2] = {VALID0f,VALID1f};
soc_field_t l405[2] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l406[2] = {
ALG_BKT_PTR0f,ALG_BKT_PTR1f};l271 = soc_mem_index_min(l1,l260);l400 = 
soc_mem_index_max(l1,l260);if((index>= 0)&&(index<l271||index>l400)){return
SOC_E_PARAM;}else if(index>= 0){l271 = index;l400 = index;}SOC_ALPM_LPM_LOCK(
l1);for(l170 = l271;l170<= l400;l170++){SOC_ALPM_LPM_UNLOCK(l1);
SOC_ALPM_LPM_LOCK(l1);l127 = soc_mem_read(l1,l260,MEM_BLOCK_ANY,l170,(void*)
l14);if(SOC_FAILURE(l127)){continue;}l9 = soc_mem_field32_get(l1,l260,(void*)
l14,MODE0f);if(l9){l263 = 1;l23 = L3_DEFIP_ALPM_IPV6_64m;l401 = 16;}else{l263
= 2;l23 = L3_DEFIP_ALPM_IPV4m;l401 = 24;}for(l191 = 0;l191<l263;l191++){if(
soc_mem_field32_get(l1,l260,(void*)l14,l404[l191]) == 0||soc_mem_field32_get(
l1,l260,(void*)l14,l405[l191]) == 1){continue;}l402 = soc_mem_field32_get(l1,
l260,(void*)l14,l406[l191]);if(l191 == 1){soc_alpm_lpm_ip4entry1_to_0(l1,l14,
l396,PRESERVE_HIT);}else{soc_alpm_lpm_ip4entry0_to_0(l1,l14,l396,PRESERVE_HIT
);}l127 = soc_alpm_lpm_vrf_get(l1,l396,&l148,&l27);if(SOC_FAILURE(l127)){
continue;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l9)){l401<<= 1;}l142 = -1;for(l257 = 
0;l257<l401;l257++){l127 = _soc_alpm_mem_index(l1,l23,l402,l257,l137,&l266);
if(SOC_FAILURE(l127)){continue;}l127 = soc_mem_read(l1,l23,MEM_BLOCK_ANY,l266
,(void*)l14);if(SOC_FAILURE(l127)){break;}if(!soc_mem_field32_get(l1,l23,(
void*)l14,VALIDf)){continue;}l127 = l26(l1,(void*)l14,l23,l9,l148,l402,0,&
lpm_entry);if(SOC_FAILURE(l127)){continue;}l127 = l146(l1,(void*)&lpm_entry,
l23,(void*)l14,&l397,&l398,&l399,FALSE);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLaunched AUX operation for "
"index %d bucket %d sanity check failed\n"),l170,l402));l403++;continue;}if(
l398!= l402){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tEntry at index %d does not belong ""to bucket %d(from bucket %d)\n"),l399,
l402,l398));l403++;}if(l142 == -1){l142 = l397;continue;}if(l142!= l397){int
l407,l408;l407 = soc_alpm_logical_idx(l1,l260,l142>>1,1);l408 = 
soc_alpm_logical_idx(l1,l260,l397>>1,1);LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"\tAliased bucket %d(returned "
"bucket %d) found from TCAM1 %d and TCAM2 %d\n"),l402,l398,l407,l408));l403++
;}}}}SOC_ALPM_LPM_UNLOCK(l1);if(l403 == 0){LOG_BSL_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check passed\n"),
SOC_MEM_NAME(l1,l260),index));return SOC_E_NONE;}LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l1,l260),index,l403));return
SOC_E_FAIL;}int soc_alpm_pivot_sanity_check(int l1,soc_mem_t l260,int index){
int l170,l191,l285 = 0;int l266,l271,l400;int l127 = SOC_E_NONE;int*l409 = 
NULL;int l397,l398,l399;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int l130,l9 = 0,
l263 = 2;int l410,l411[2];soc_mem_t l412,l413;int l403 = 0;soc_field_t l404[2
] = {VALID0f,VALID1f};soc_field_t l405[2] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};
soc_field_t l414[2] = {NEXT_HOP_INDEX0f,NEXT_HOP_INDEX1f};soc_field_t l406[2]
= {ALG_BKT_PTR0f,ALG_BKT_PTR1f};l271 = soc_mem_index_min(l1,l260);l400 = 
soc_mem_index_max(l1,l260);if((index>= 0)&&(index<l271||index>l400)){return
SOC_E_PARAM;}else if(index>= 0){l271 = index;l400 = index;}l409 = sal_alloc(
sizeof(int)*MAX_PIVOT_COUNT,"Bucket index array");if(l409 == NULL){l127 = 
SOC_E_MEMORY;return l127;}sal_memset(l409,0xff,sizeof(int)*MAX_PIVOT_COUNT);
SOC_ALPM_LPM_LOCK(l1);for(l170 = l271;l170<= l400;l170++){SOC_ALPM_LPM_UNLOCK
(l1);SOC_ALPM_LPM_LOCK(l1);l127 = soc_mem_read(l1,l260,MEM_BLOCK_ANY,l170,(
void*)l14);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l1,l260),
l170,l127));l403++;continue;}l9 = soc_mem_field32_get(l1,l260,(void*)l14,
MODE0f);if(l9){l263 = 1;l413 = L3_DEFIP_ALPM_IPV6_64m;}else{l263 = 2;l413 = 
L3_DEFIP_ALPM_IPV4m;}for(l191 = 0;l191<l263;l191++){if(soc_mem_field32_get(l1
,l260,(void*)l14,l404[l191]) == 0||soc_mem_field32_get(l1,l260,(void*)l14,
l405[l191]) == 1){continue;}l130 = soc_mem_field32_get(l1,l260,(void*)l14,
l406[l191]);if(l130!= 0){if(l409[l130] == -1){l409[l130] = l170;}else{
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tDuplicated bucket pointer "
"%d detected, in memory %s index1 %d ""and index2 %d\n"),l130,SOC_MEM_NAME(l1
,l260),l409[l130],l170));l403++;continue;}}l127 = alpm_bucket_is_assigned(l1,
l130,l9,&l285);if(l127 == SOC_E_PARAM){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"\tInvalid bucket pointer %d ""detected, in memory %s index %d\n"),l130,
SOC_MEM_NAME(l1,l260),l170));l403++;continue;}if(l127>= 0&&l285 == 0){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),l130,SOC_MEM_NAME(l1,l260),l170));l403++
;continue;}l412 = _soc_trident2_alpm_bkt_view_get(l1,l130<<2);if(l413!= l412)
{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual"" %s\n"),l130,SOC_MEM_NAME(l1,l413),
SOC_MEM_NAME(l1,l412)));l403++;continue;}l410 = soc_mem_field32_get(l1,l260,(
void*)l14,l414[l191]);if(l191 == 1){l127 = soc_alpm_lpm_ip4entry1_to_0(l1,l14
,l14,PRESERVE_HIT);if(SOC_FAILURE(l127)){continue;}}l397 = -1;l127 = l146(l1,
l14,l413,(void*)l14,&l397,&l398,&l399,FALSE);if(l397 == -1){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLaunched AUX operation for PIVOT "
"index %d sanity check failed\n"),l170));l403++;continue;}l266 = 
soc_alpm_logical_idx(l1,l260,l397>>1,1);l127 = soc_mem_read(l1,l260,
MEM_BLOCK_ANY,l266,(void*)l14);if(SOC_FAILURE(l127)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tRead memory %s index %d (nexthop) "
"return %d\n"),SOC_MEM_NAME(l1,l260),l266,l127));l403++;continue;}l411[0] = 
soc_mem_field32_get(l1,l260,(void*)l14,l414[0]);l411[1] = soc_mem_field32_get
(l1,l260,(void*)l14,l414[1]);if(l410!= l411[l397&1]){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d entry1 %d, index2 %d ""entry2 %d\n"),l170,l191,
l266,l397&1));l403++;continue;}}}SOC_ALPM_LPM_UNLOCK(l1);sal_free(l409);if(
l403 == 0){LOG_BSL_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMemory %s index %d Pivot sanity check passed\n"),SOC_MEM_NAME(l1,l260),
index));return SOC_E_NONE;}LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMemory %s index %d Pivot sanity check failed, ""encountered %d error(s)\n"
),SOC_MEM_NAME(l1,l260),index,l403));return SOC_E_FAIL;}
#endif
#endif /* ALPM_ENABLE */
