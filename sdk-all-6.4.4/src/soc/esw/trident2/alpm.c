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
soc_field_info_t*l92;soc_field_info_t*l93;soc_field_info_t*l94;
soc_field_info_t*l95;soc_field_info_t*l96;soc_field_info_t*l97;
soc_field_info_t*l98;}l99,*l100;static l100 l101[SOC_MAX_NUM_DEVICES];typedef
struct l102{int l20;int l103;int l104;uint16*l105;uint16*l106;}l107;typedef
uint32 l108[5];typedef int(*l109)(l108 l110,l108 l111);static l107*l112[
SOC_MAX_NUM_DEVICES];static void l113(int l1,void*l14,int index,l108 l114);
static uint16 l115(uint8*l116,int l117);static int l118(int l20,int l103,int
l104,l107**l119);static int l120(l107*l121);static int l122(l107*l123,l109
l124,l108 entry,int l125,uint16*l126);static int l127(l107*l123,l109 l124,
l108 entry,int l125,uint16 l128,uint16 l129);static int l130(l107*l123,l109
l124,l108 entry,int l125,uint16 l131);alpm_vrf_handle_t*alpm_vrf_handle[
SOC_MAX_NUM_DEVICES];alpm_pivot_t**tcam_pivot[SOC_MAX_NUM_DEVICES];int
soc_alpm_mode_get(int l1){if(soc_trident2_alpm_mode_get(l1) == 1){return 1;}
else{return 0;}}static int l132(int l1,const void*entry,int*l125){int l133;
uint32 l134;int l9;l9 = soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f);if(l9)
{l134 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l133 = 
_ipmask2pfx(l134,l125))<0){return(l133);}l134 = soc_mem_field32_get(l1,
L3_DEFIPm,entry,IP_ADDR_MASK1f);if(*l125){if(l134!= 0xffffffff){return(
SOC_E_PARAM);}*l125+= 32;}else{if((l133 = _ipmask2pfx(l134,l125))<0){return(
l133);}}}else{l134 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);
if((l133 = _ipmask2pfx(l134,l125))<0){return(l133);}}return SOC_E_NONE;}int
_soc_alpm_rpf_entry(int l1,int l135){int l136;l136 = (l135>>2)&0x3fff;l136+= 
SOC_ALPM_BUCKET_COUNT(l1);return(l135&~(0x3fff<<2))|(l136<<2);}int
soc_alpm_physical_idx(int l1,soc_mem_t l23,int index,int l137){int l138 = 
index&1;if(l137){return soc_trident2_l3_defip_index_map(l1,l23,index);}index
>>= 1;index = soc_trident2_l3_defip_index_map(l1,l23,index);index<<= 1;index
|= l138;return index;}int soc_alpm_logical_idx(int l1,soc_mem_t l23,int index
,int l137){int l138 = index&1;if(l137){return
soc_trident2_l3_defip_index_remap(l1,l23,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_remap(l1,l23,index);index<<= 1;index|= l138;
return index;}static int l139(int l1,void*entry,uint32*prefix,uint32*l16,int*
l25){int l140,l141,l9;int l125 = 0;int l133 = SOC_E_NONE;uint32 l142,l138;
prefix[0] = prefix[1] = prefix[2] = prefix[3] = prefix[4] = 0;l9 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f);l140 = soc_mem_field32_get(l1,
L3_DEFIPm,entry,IP_ADDR0f);l141 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK0f);prefix[1] = l140;l140 = soc_mem_field32_get(l1,L3_DEFIPm,
entry,IP_ADDR1f);l141 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK1f
);prefix[0] = l140;if(l9){prefix[4] = prefix[1];prefix[3] = prefix[0];prefix[
1] = prefix[0] = 0;l141 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(l141,&l125))<0){return(l133);}l141 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK1f);if(l125){if(l141!= 
0xffffffff){return(SOC_E_PARAM);}l125+= 32;}else{if((l133 = _ipmask2pfx(l141,
&l125))<0){return(l133);}}l142 = 64-l125;if(l142<32){prefix[4]>>= l142;l138 = 
(((32-l142) == 32)?0:(prefix[3])<<(32-l142));prefix[3]>>= l142;prefix[4]|= 
l138;}else{prefix[4] = (((l142-32) == 32)?0:(prefix[3])>>(l142-32));prefix[3]
= 0;}}else{l141 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((
l133 = _ipmask2pfx(l141,&l125))<0){return(l133);}prefix[1] = (((32-l125) == 
32)?0:(prefix[1])>>(32-l125));prefix[0] = 0;}*l16 = l125;*l25 = (prefix[0] == 
0)&&(prefix[1] == 0)&&(l125 == 0);return SOC_E_NONE;}int _soc_alpm_find_in_bkt
(int l1,soc_mem_t l23,int bucket_index,int l143,uint32*l14,void*l144,int*l126
,int l33){int l133;l133 = soc_mem_alpm_lookup(l1,l23,bucket_index,
MEM_BLOCK_ANY,l143,l14,l144,l126);if(SOC_SUCCESS(l133)){return l133;}if(
SOC_ALPM_V6_SCALE_CHECK(l1,l33)){return soc_mem_alpm_lookup(l1,l23,
bucket_index+1,MEM_BLOCK_ANY,l143,l14,l144,l126);}return l133;}static int l145
(int l1,uint32*prefix,uint32 l146,int l33,int l27,int*l147,int*l148,int*
bucket_index){int l133 = SOC_E_NONE;trie_t*l149;trie_node_t*l150 = NULL;
alpm_pivot_t*l151;if(l33){l149 = VRF_PIVOT_TRIE_IPV6(l1,l27);}else{l149 = 
VRF_PIVOT_TRIE_IPV4(l1,l27);}l133 = trie_find_lpm(l149,prefix,l146,&l150);if(
SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l133;}l151 = (alpm_pivot_t*)l150;*l147 = 1;*
l148 = PIVOT_TCAM_INDEX(l151);*bucket_index = PIVOT_BUCKET_INDEX(l151);return
SOC_E_NONE;}static int l152(int l1,void*l7,soc_mem_t l23,void*l144,int*l148,
int*bucket_index,int*l15,int l153){uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int
l154,l27,l33;int l126;uint32 l10,l143;int l133 = SOC_E_NONE;int l147 = 0;l33 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f);if(l33){if(!(l33 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l7,&l154,&l27));if(l154 == 0){if(
soc_alpm_mode_get(l1)){return SOC_E_PARAM;}}if(l27 == SOC_VRF_MAX(l1)+1){l10 = 
0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l143);}else{l10 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l143);}if(l154!= SOC_L3_VRF_OVERRIDE){if(
l153){uint32 prefix[5],l146;int l25 = 0;l133 = l139(l1,l7,prefix,&l146,&l25);
if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l133;}l133 = l145(l1,
prefix,l146,l33,l27,&l147,l148,bucket_index);SOC_IF_ERROR_RETURN(l133);}else{
defip_aux_scratch_entry_t l12;sal_memset(&l12,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(l1,l7,l33,l10,0,&l12));
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l12,TRUE,&l147,l148,
bucket_index));}if(l147){l19(l1,l7,l14,0,l23,0,0);l133 = 
_soc_alpm_find_in_bkt(l1,l23,*bucket_index,l143,l14,l144,&l126,l33);if(
SOC_SUCCESS(l133)){*l15 = l126;}}else{l133 = SOC_E_NOT_FOUND;}}return l133;}
static int l155(int l1,void*l7,void*l144,void*l156,soc_mem_t l23,int l126){
defip_aux_scratch_entry_t l12;int l154,l33,l27;int bucket_index;uint32 l10,
l143;int l133 = SOC_E_NONE;int l147 = 0,l138 = 0;int l148;l33 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f);if(l33){if(!(l33 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l7,&l154,&l27));if(l27 == 
SOC_VRF_MAX(l1)+1){l10 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l143);}else{
l10 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l143);}if(!soc_alpm_mode_get(l1)){
l10 = 2;}if(l154!= SOC_L3_VRF_OVERRIDE){sal_memset(&l12,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(l1,l7,l33,l10,0,&l12));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l23,MEM_BLOCK_ANY,l126,l144));if(
SOC_URPF_STATUS_GET(l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,l23,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l126),l156));}l138 = soc_mem_field32_get
(l1,L3_DEFIP_AUX_SCRATCHm,&l12,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,REPLACE_LENf,l138);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l147,&l148,&bucket_index));if
(SOC_URPF_STATUS_GET(l1)){l138 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm
,&l12,DB_TYPEf);l138+= 1;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,
DB_TYPEf,l138);SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,
TRUE,&l147,&l148,&bucket_index));}}return l133;}int alpm_mem_prefix_array_cb(
trie_node_t*node,void*l157){alpm_mem_prefix_array_t*l158 = (
alpm_mem_prefix_array_t*)l157;if(node->type == PAYLOAD){l158->prefix[l158->
count] = (payload_t*)node;l158->count++;}return SOC_E_NONE;}int
alpm_delete_node_cb(trie_node_t*node,void*l157){if(node!= NULL){sal_free(node
);}return SOC_E_NONE;}static int l159(int l1,int l160,int l33,int l161){int
l133,l138,index;defip_aux_table_entry_t entry;index = l160>>(l33?0:1);l133 = 
soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l133);if(l33){soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,
&entry,BPM_LENGTH0f,l161);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH1f,l161);l138 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE0f);}else{if(l160&1){soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry
,BPM_LENGTH1f,l161);l138 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE1f);}else{soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH0f,l161);l138 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE0f);}}l133 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,
&entry);SOC_IF_ERROR_RETURN(l133);if(SOC_URPF_STATUS_GET(l1)){l138++;if(l33){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l161);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l161);}else{if
(l160&1){soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l161)
;}else{soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l161);}
}soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l138);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l138);index+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l133 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}
return l133;}static int l162(int l1,int l163,void*entry,void*l164,int l165){
uint32 l138,l141,l33,l10,l166 = 0;soc_mem_t l23 = L3_DEFIPm;soc_mem_t l167 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l168;int l133 = SOC_E_NONE,l125,l169,l27,
l170;SOC_IF_ERROR_RETURN(soc_mem_read(l1,l167,MEM_BLOCK_ANY,l163,l164));l138 = 
soc_mem_field32_get(l1,l23,entry,VRF_ID_0f);soc_mem_field32_set(l1,l167,l164,
VRF0f,l138);l138 = soc_mem_field32_get(l1,l23,entry,VRF_ID_1f);
soc_mem_field32_set(l1,l167,l164,VRF1f,l138);l138 = soc_mem_field32_get(l1,
l23,entry,MODE0f);soc_mem_field32_set(l1,l167,l164,MODE0f,l138);l138 = 
soc_mem_field32_get(l1,l23,entry,MODE1f);soc_mem_field32_set(l1,l167,l164,
MODE1f,l138);l33 = l138;l138 = soc_mem_field32_get(l1,l23,entry,VALID0f);
soc_mem_field32_set(l1,l167,l164,VALID0f,l138);l138 = soc_mem_field32_get(l1,
l23,entry,VALID1f);soc_mem_field32_set(l1,l167,l164,VALID1f,l138);l138 = 
soc_mem_field32_get(l1,l23,entry,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(l138,
&l125))<0){return l133;}l141 = soc_mem_field32_get(l1,l23,entry,
IP_ADDR_MASK1f);if((l133 = _ipmask2pfx(l141,&l169))<0){return l133;}if(l33){
soc_mem_field32_set(l1,l167,l164,IP_LENGTH0f,l125+l169);soc_mem_field32_set(
l1,l167,l164,IP_LENGTH1f,l125+l169);}else{soc_mem_field32_set(l1,l167,l164,
IP_LENGTH0f,l125);soc_mem_field32_set(l1,l167,l164,IP_LENGTH1f,l169);}l138 = 
soc_mem_field32_get(l1,l23,entry,IP_ADDR0f);soc_mem_field32_set(l1,l167,l164,
IP_ADDR0f,l138);l138 = soc_mem_field32_get(l1,l23,entry,IP_ADDR1f);
soc_mem_field32_set(l1,l167,l164,IP_ADDR1f,l138);l138 = soc_mem_field32_get(
l1,l23,entry,ENTRY_TYPE0f);soc_mem_field32_set(l1,l167,l164,ENTRY_TYPE0f,l138
);l138 = soc_mem_field32_get(l1,l23,entry,ENTRY_TYPE1f);soc_mem_field32_set(
l1,l167,l164,ENTRY_TYPE1f,l138);if(!l33){sal_memcpy(&l168,entry,sizeof(l168))
;l133 = soc_alpm_lpm_vrf_get(l1,(void*)&l168,&l27,&l125);SOC_IF_ERROR_RETURN(
l133);SOC_IF_ERROR_RETURN(soc_alpm_lpm_ip4entry1_to_0(l1,&l168,&l168,
PRESERVE_HIT));l133 = soc_alpm_lpm_vrf_get(l1,(void*)&l168,&l170,&l125);
SOC_IF_ERROR_RETURN(l133);}else{l133 = soc_alpm_lpm_vrf_get(l1,entry,&l27,&
l125);}if(SOC_URPF_STATUS_GET(l1)){if(l165>= (soc_mem_index_count(l1,
L3_DEFIPm)>>1)){l166 = 1;}}switch(l27){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l167,l164,VALID0f,0);l10 = 0;break;case
SOC_L3_VRF_GLOBAL:l10 = l166?1:0;break;default:l10 = l166?3:2;break;}if(
SOC_MEM_FIELD_VALID(l1,l23,MULTICAST_ROUTE0f)){l138 = soc_mem_field32_get(l1,
l23,entry,MULTICAST_ROUTE0f);if(l138){l10 = l166?5:4;}}soc_mem_field32_set(l1
,l167,l164,DB_TYPE0f,l10);if(!l33){switch(l170){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l167,l164,VALID1f,0);l10 = 0;break;case
SOC_L3_VRF_GLOBAL:l10 = l166?1:0;break;default:l10 = l166?3:2;break;}if(
SOC_MEM_FIELD_VALID(l1,l23,MULTICAST_ROUTE1f)){l138 = soc_mem_field32_get(l1,
l23,entry,MULTICAST_ROUTE1f);if(l138){l10 = l166?5:4;}}soc_mem_field32_set(l1
,l167,l164,DB_TYPE1f,l10);}else{if(l27 == SOC_L3_VRF_OVERRIDE){
soc_mem_field32_set(l1,l167,l164,VALID1f,0);}if(SOC_MEM_FIELD_VALID(l1,l23,
MULTICAST_ROUTE1f)){l138 = soc_mem_field32_get(l1,l23,entry,MULTICAST_ROUTE1f
);if(l138){l10 = l166?5:4;}}soc_mem_field32_set(l1,l167,l164,DB_TYPE1f,l10);}
if(l166){l138 = soc_mem_field32_get(l1,l23,entry,ALG_BKT_PTR0f);if(l138){l138
+= SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l23,entry,ALG_BKT_PTR0f,
l138);}if(!l33){l138 = soc_mem_field32_get(l1,l23,entry,ALG_BKT_PTR1f);if(
l138){l138+= SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l23,entry,
ALG_BKT_PTR1f,l138);}}}return SOC_E_NONE;}static int l171(int l1,int l172,int
index,int l173,void*entry){defip_aux_table_entry_t l164;l173 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l173,1);SOC_IF_ERROR_RETURN(l162(l1,l173,
entry,(void*)&l164,index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l1,
MEM_BLOCK_ANY,index,entry));index = soc_alpm_physical_idx(l1,L3_DEFIPm,index,
1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,index,&l164
));return SOC_E_NONE;}int _soc_alpm_insert_in_bkt(int l1,soc_mem_t l23,int
bucket_index,int l143,void*l144,uint32*l14,int*l126,int l33){int l133;l133 = 
soc_mem_alpm_insert(l1,l23,bucket_index,MEM_BLOCK_ANY,l143,l144,l14,l126);if(
l133 == SOC_E_FULL){if(SOC_ALPM_V6_SCALE_CHECK(l1,l33)){return
soc_mem_alpm_insert(l1,l23,bucket_index+1,MEM_BLOCK_ANY,l143,l144,l14,l126);}
}return l133;}int _soc_alpm_mem_index(int l1,soc_mem_t l23,int bucket_index,
int l174,uint32 l143,int*l175){int l176,l177 = 0;int l178[4] = {0};int l179 = 
0;int l180 = 0;int l181;int l33 = 0;int l182 = 6;switch(l23){case
L3_DEFIP_ALPM_IPV6_64m:l182 = 4;l33 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:
l182 = 2;l33 = 1;break;default:break;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l33)){if(
l174>= ALPM_RAW_BKT_COUNT*l182){bucket_index++;l174-= ALPM_RAW_BKT_COUNT*l182
;}}l181 = (4)-_shr_popcount(l143&((1<<(4))-1));if(bucket_index>= (1<<16)||
l174>= l181*l182){return SOC_E_FULL;}l180 = l174%l182;for(l176 = 0;l176<(4);
l176++){if((1<<l176)&l143){continue;}l178[l177++] = l176;}l179 = l178[l174/
l182];*l175 = (l180<<16)|(bucket_index<<2)|(l179);return SOC_E_NONE;}void
_soc_alpm_raw_mem_read(int l20,soc_mem_t l23,void*l183,int l184,void*entry){
soc_mem_info_t l185;soc_field_info_t l186;int l187 = soc_mem_entry_bits(l20,
l23)-1;l185.flags = 0;l185.bytes = sizeof(defip_alpm_raw_entry_t);l186.flags = 
SOCF_LE;l186.bp = l187*l184;l186.len = l187;(void)
soc_meminfo_fieldinfo_field_get(l183,&l185,&l186,entry);}void
_soc_alpm_raw_mem_write(int l20,soc_mem_t l23,void*l183,int l184,void*entry){
soc_mem_info_t l185;soc_field_info_t l186;int l187 = soc_mem_entry_bits(l20,
l23)-1;l185.flags = 0;l185.bytes = sizeof(defip_alpm_raw_entry_t);l186.bp = 
l187*l184;l186.len = l187;l186.flags = SOCF_LE;(void)
soc_meminfo_fieldinfo_field_set(l183,&l185,&l186,entry);}int l188(int l1,
soc_mem_t l23,int index){return SOC_ALPM_BKT_ENTRY_TO_IDX(index%(1<<16));}int
_soc_alpm_raw_bucket_read(int l1,soc_mem_t l23,int bucket_index,void*l183,
void*l189){int l176,l33 = 1;int l190,l191;defip_alpm_raw_entry_t*l192 = l183;
defip_alpm_raw_entry_t*l193 = l189;if(l23 == L3_DEFIP_ALPM_IPV4m){l33 = 0;}
l190 = SOC_ALPM_BKT_IDX_TO_ENTRY(bucket_index);for(l176 = 0;l176<
SOC_ALPM_RAW_BUCKET_SIZE(l1,l33);l176++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,
L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l190+l176,&l192[l176]));if(
SOC_URPF_STATUS_GET(l1)){l191 = _soc_alpm_rpf_entry(l1,l190+l176);
SOC_IF_ERROR_RETURN(soc_mem_read(l1,L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l191,&
l193[l176]));}}return SOC_E_NONE;}int _soc_alpm_raw_bucket_write(int l1,
soc_mem_t l23,int bucket_index,uint32 l143,void*l183,void*l189,int l194){int
l176 = 0,l195,l33 = 1;int l190,l191,l196;defip_alpm_raw_entry_t*l192 = l183;
defip_alpm_raw_entry_t*l193 = l189;int l197 = 6;switch(l23){case
L3_DEFIP_ALPM_IPV4m:l197 = 6;l33 = 0;break;case L3_DEFIP_ALPM_IPV6_64m:l197 = 
4;l33 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:l197 = 2;l33 = 1;break;default:
break;}l190 = SOC_ALPM_BKT_IDX_TO_ENTRY(bucket_index);if(l194 == -1){l196 = 
SOC_ALPM_RAW_BUCKET_SIZE(l1,l33);}else{l196 = (l194/l197)+1;}for(l195 = 0;
l195<SOC_ALPM_RAW_BUCKET_SIZE(l1,l33);l195++){if((1<<(l195%(4)))&l143){
continue;}SOC_IF_ERROR_RETURN(soc_mem_write(l1,L3_DEFIP_ALPM_RAWm,
MEM_BLOCK_ANY,l190+l195,&l192[l195]));_soc_trident2_alpm_bkt_view_set(l1,l190
+l195,l23);if(SOC_URPF_STATUS_GET(l1)){l191 = _soc_alpm_rpf_entry(l1,l190+
l195);_soc_trident2_alpm_bkt_view_set(l1,l191,l23);SOC_IF_ERROR_RETURN(
soc_mem_write(l1,L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l191,&l193[l195]));}if(++
l176 == l196){break;}}return SOC_E_NONE;}void _soc_alpm_raw_parity_set(int l1
,soc_mem_t l23,void*l144){int l176,l198,l199 = 0;uint32*entry = l144;l198 = 
soc_mem_entry_words(l1,l23);for(l176 = 0;l176<l198;l176++){l199+= 
_shr_popcount(entry[l176]);}if(l199&0x1){if(SOC_MEM_FIELD_VALID(l1,l23,
EVEN_PARITYf)){soc_mem_field32_set(l1,l23,l144,EVEN_PARITYf,1);}}}static int
l200(int l1,void*l7,soc_mem_t l23,void*l144,void*l156,int*l15,int bucket_index
,int l148){alpm_pivot_t*l151,*l201,*l202;defip_aux_scratch_entry_t l12;uint32
l14[SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l203,l146;uint32 l204[5];int l33
,l27,l154;int l126;int l133 = SOC_E_NONE,l205;uint32 l10,l143,l161 = 0;int
l147 =0;int l160;int l206 = 0;trie_t*trie,*l207;trie_node_t*l208,*l209 = NULL
,*l150 = NULL;payload_t*l210,*l211,*l212;defip_entry_t lpm_entry;
alpm_bucket_handle_t*l213;int l176,l214 = -1,l25 = 0;alpm_mem_prefix_array_t
l158;defip_alpm_ipv4_entry_t l215,l216;defip_alpm_ipv6_64_entry_t l217,l218;
void*l219,*l220;int*l129 = NULL;trie_t*l149 = NULL;int l221;int l222;void*
l223;void*l224 = NULL;void*l225;void*l226;void*l227;int l228 = 0;int l184 = 0
;l33 = soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f);if(l33){if(!(l33 = 
soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l7,&l154,&l27));if(l27 == 
SOC_VRF_MAX(l1)+1){l10 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l143);}else{
l10 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l143);}l23 = (l33)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l219 = ((l33)?((uint32*)&(l217)):(
(uint32*)&(l215)));l220 = ((l33)?((uint32*)&(l218)):((uint32*)&(l216)));l133 = 
l139(l1,l7,prefix,&l146,&l25);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_insert: prefix create failed\n")));return l133;}
sal_memset(&l12,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(
l1,l7,l33,l10,0,&l12));if(bucket_index == 0){l133 = l145(l1,prefix,l146,l33,
l27,&l147,&l148,&bucket_index);SOC_IF_ERROR_RETURN(l133);if(l147 == 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: "
"Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l133 = 
_soc_alpm_insert_in_bkt(l1,l23,bucket_index,l143,l144,l14,&l126,l33);if(l133
== SOC_E_NONE){*l15 = l126;if(SOC_URPF_STATUS_GET(l1)){l205 = soc_mem_write(
l1,l23,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l126),l156);if(SOC_FAILURE(l205))
{return l205;}}}if(l133 == SOC_E_FULL){l206 = 1;}l151 = ALPM_TCAM_PIVOT(l1,
l148);trie = PIVOT_BUCKET_TRIE(l151);l202 = l151;l210 = sal_alloc(sizeof(
payload_t),"Payload for Key");if(l210 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Unable to allocate memory for "
"trie node \n")));return SOC_E_MEMORY;}l211 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l211 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l210);return SOC_E_MEMORY;}sal_memset(l210,0,
sizeof(*l210));sal_memset(l211,0,sizeof(*l211));l210->key[0] = prefix[0];l210
->key[1] = prefix[1];l210->key[2] = prefix[2];l210->key[3] = prefix[3];l210->
key[4] = prefix[4];l210->len = l146;l210->index = l126;sal_memcpy(l211,l210,
sizeof(*l210));l211->bkt_ptr = l210;l133 = trie_insert(trie,prefix,NULL,l146,
(trie_node_t*)l210);if(SOC_FAILURE(l133)){goto l229;}if(l33){l207 = 
VRF_PREFIX_TRIE_IPV6(l1,l27);}else{l207 = VRF_PREFIX_TRIE_IPV4(l1,l27);}if(!
l25){l133 = trie_insert(l207,prefix,NULL,l146,(trie_node_t*)l211);}else{l150 = 
NULL;l133 = trie_find_lpm(l207,0,0,&l150);l212 = (payload_t*)l150;if(
SOC_SUCCESS(l133)){l212->bkt_ptr = l210;}}l203 = l146;if(SOC_FAILURE(l133)){
goto l230;}if(l206){l133 = alpm_bucket_assign(l1,&bucket_index,l33);if(
SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate""new bucket for split\n")));
bucket_index = -1;goto l231;}l133 = trie_split(trie,l33?_MAX_KEY_LEN_144_:
_MAX_KEY_LEN_48_,FALSE,l204,&l146,&l208,NULL,FALSE);if(SOC_FAILURE(l133)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not split bucket""for prefix 0x%08x 0x%08x\n"),
prefix[0],prefix[1]));goto l231;}l150 = NULL;l133 = trie_find_lpm(l207,l204,
l146,&l150);l212 = (payload_t*)l150;if(SOC_FAILURE(l133)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l204[0],l204[1],l204[
2],l204[3],l204[4],l146));goto l231;}if(l212->bkt_ptr){if(l212->bkt_ptr == 
l210){sal_memcpy(l219,l144,l33?sizeof(defip_alpm_ipv6_64_entry_t):sizeof(
defip_alpm_ipv4_entry_t));}else{l133 = soc_mem_read(l1,l23,MEM_BLOCK_ANY,((
payload_t*)l212->bkt_ptr)->index,l219);}if(SOC_FAILURE(l133)){goto l231;}l133
= l26(l1,l219,l23,l33,l154,bucket_index,0,&lpm_entry);if(SOC_FAILURE(l133)){
goto l231;}l161 = ((payload_t*)(l212->bkt_ptr))->len;}else{l133 = 
soc_mem_read(l1,L3_DEFIPm,MEM_BLOCK_ANY,soc_alpm_logical_idx(l1,L3_DEFIPm,
l148>>1,1),&lpm_entry);if((!l33)&&(l148&1)){l133 = 
soc_alpm_lpm_ip4entry1_to_0(l1,&lpm_entry,&lpm_entry,0);}}l213 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l213 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l133 = SOC_E_MEMORY;goto l231;}sal_memset(l213,0,sizeof(*l213));l151 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l151 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l133 = SOC_E_MEMORY;goto l231;}sal_memset(l151,0,sizeof(*l151));
PIVOT_BUCKET_HANDLE(l151) = l213;if(l33){l133 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l151));}else{l133 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l151));}PIVOT_BUCKET_TRIE(l151)->trie = l208;
PIVOT_BUCKET_INDEX(l151) = bucket_index;PIVOT_BUCKET_VRF(l151) = l27;
PIVOT_BUCKET_IPV6(l151) = l33;PIVOT_BUCKET_DEF(l151) = FALSE;l151->key[0] = 
l204[0];l151->key[1] = l204[1];l151->len = l146;l151->key[2] = l204[2];l151->
key[3] = l204[3];l151->key[4] = l204[4];if(l33){l149 = VRF_PIVOT_TRIE_IPV6(l1
,l27);}else{l149 = VRF_PIVOT_TRIE_IPV4(l1,l27);}do{if(!(l33)){l204[0] = (((32
-l146) == 32)?0:(l204[1])<<(32-l146));l204[1] = 0;}else{int l232 = 64-l146;
int l233;if(l232<32){l233 = l204[3]<<l232;l233|= (((32-l232) == 32)?0:(l204[4
])>>(32-l232));l204[0] = l204[4]<<l232;l204[1] = l233;l204[2] = l204[3] = 
l204[4] = 0;}else{l204[1] = (((l232-32) == 32)?0:(l204[4])<<(l232-32));l204[0
] = l204[2] = l204[3] = l204[4] = 0;}}}while(0);l29(l1,l204,l146,l27,l33,&
lpm_entry,0,0);soc_L3_DEFIPm_field32_set(l1,&lpm_entry,ALG_BKT_PTR0f,
bucket_index);sal_memset(&l158,0,sizeof(l158));l133 = trie_traverse(
PIVOT_BUCKET_TRIE(l151),alpm_mem_prefix_array_cb,&l158,_TRIE_INORDER_TRAVERSE
);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l231;}l129 = sal_alloc(sizeof(*l129)*l158.count,
"Temp storage for location of prefixes in new bucket");if(l129 == NULL){l133 = 
SOC_E_MEMORY;goto l231;}sal_memset(l129,-1,sizeof(*l129)*l158.count);l221 = 
sizeof(defip_alpm_raw_entry_t);l222 = l221*ALPM_RAW_BKT_COUNT_DW;l224 = 
sal_alloc(4*l222,"Raw memory buffer");if(l224 == NULL){l133 = SOC_E_MEMORY;
goto l231;}sal_memset(l224,0,4*l222);l225 = (uint8*)l224+l222;l226 = (uint8*)
l224+l222*2;l227 = (uint8*)l224+l222*3;l133 = _soc_alpm_raw_bucket_read(l1,
l23,PIVOT_BUCKET_INDEX(l202),l224,l225);if(SOC_FAILURE(l133)){goto l231;}for(
l176 = 0;l176<l158.count;l176++){payload_t*l125 = l158.prefix[l176];if(l125->
index>0){SOC_ALPM_RAW_INDEX_DECODE(l1,l33,l125->index,l228,l184);l223 = (
uint8*)l224+l228*l221;_soc_alpm_raw_mem_read(l1,l23,l223,l184,l219);
_soc_alpm_raw_mem_write(l1,l23,l223,l184,soc_mem_entry_null(l1,l23));if(
SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE(l1,l33,_soc_alpm_rpf_entry
(l1,l125->index),l228,l184);l223 = (uint8*)l225+l228*l221;
_soc_alpm_raw_mem_read(l1,l23,l223,l184,l220);_soc_alpm_raw_mem_write(l1,l23,
l223,l184,soc_mem_entry_null(l1,l23));}l133 = _soc_alpm_mem_index(l1,l23,
bucket_index,l176,l143,&l126);if(SOC_SUCCESS(l133)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l33,l126,l228,l184);l223 = (uint8*)l226+l228*l221;_soc_alpm_raw_mem_write
(l1,l23,l223,l184,l219);if(SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l33,_soc_alpm_rpf_entry(l1,l126),l228,l184);l223 = (uint8*)l227+l228*l221
;_soc_alpm_raw_mem_write(l1,l23,l223,l184,l220);}}}else{l133 = 
_soc_alpm_mem_index(l1,l23,bucket_index,l176,l143,&l126);if(SOC_SUCCESS(l133)
){l214 = l176;*l15 = l126;_soc_alpm_raw_parity_set(l1,l23,l144);
SOC_ALPM_RAW_INDEX_DECODE(l1,l33,l126,l228,l184);l223 = (uint8*)l226+l228*
l221;_soc_alpm_raw_mem_write(l1,l23,l223,l184,l144);if(SOC_URPF_STATUS_GET(l1
)){_soc_alpm_raw_parity_set(l1,l23,l156);SOC_ALPM_RAW_INDEX_DECODE(l1,l33,
_soc_alpm_rpf_entry(l1,l126),l228,l184);l223 = (uint8*)l227+l228*l221;
_soc_alpm_raw_mem_write(l1,l23,l223,l184,l156);}}}l129[l176] = l126;}l133 = 
_soc_alpm_raw_bucket_write(l1,l23,bucket_index,l143,(void*)l226,(void*)l227,
l158.count);if(SOC_FAILURE(l133)){goto l234;}l133 = l4(l1,&lpm_entry,&l160);
if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to add new""pivot to tcam\n")));if(l133 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l27,l33);}goto l234;}l160 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l160,l33);l133 = l159(l1,l160,l33,l161);if
(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),l160));goto l235
;}l133 = trie_insert(l149,l151->key,NULL,l151->len,(trie_node_t*)l151);if(
SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));goto l235;}ALPM_TCAM_PIVOT(l1,l160<<(
l33?1:0)) = l151;PIVOT_TCAM_INDEX(l151) = l160<<(l33?1:0);VRF_PIVOT_REF_INC(
l1,l27,l33);for(l176 = 0;l176<l158.count;l176++){payload_t*l125 = l158.prefix
[l176];if(l125->index>0){l133 = soc_mem_cache_invalidate(l1,l23,MEM_BLOCK_ANY
,l125->index);if(SOC_SUCCESS(l133)){if(SOC_URPF_STATUS_GET(l1)){
SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(l1,l23,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l125->index)));}}if(SOC_FAILURE(l133)){goto l235;}}
l125->index = l129[l176];}sal_free(l129);l133 = _soc_alpm_raw_bucket_write(l1
,l23,PIVOT_BUCKET_INDEX(l202),l143,(void*)l224,(void*)l225,-1);if(SOC_FAILURE
(l133)){goto l235;}if(l214 == -1){l133 = _soc_alpm_insert_in_bkt(l1,l23,
PIVOT_BUCKET_INDEX(l202),l143,l144,l14,&l126,l33);if(SOC_FAILURE(l133)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n")));
goto l235;}if(SOC_URPF_STATUS_GET(l1)){l133 = soc_mem_write(l1,l23,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l126),l156);}*l15 = l126;l210->index = 
l126;}sal_free(l224);PIVOT_BUCKET_ENT_CNT_UPDATE(l151);VRF_BUCKET_SPLIT_INC(
l1,l27,l33);}VRF_TRIE_ROUTES_INC(l1,l27,l33);if(l25){sal_free(l211);}
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l147,&
l148,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE
,&l12,FALSE,&l147,&l148,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l146 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf);l146+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l146);
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l147,&
l148,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE
,&l12,FALSE,&l147,&l148,&bucket_index));}PIVOT_BUCKET_ENT_CNT_UPDATE(l202);
return l133;l235:l205 = l6(l1,&lpm_entry);if(SOC_FAILURE(l205)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failure to free new prefix"
"at %d\n"),soc_alpm_logical_idx(l1,L3_DEFIPm,l160,l33)));}if(ALPM_TCAM_PIVOT(
l1,l160<<(l33?1:0))!= NULL){l205 = trie_delete(l149,l151->key,l151->len,&l209
);if(SOC_FAILURE(l205)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l1,l160<<(l33?1:0)) = NULL;VRF_PIVOT_REF_DEC(l1,l27,l33);l234
:l201 = l202;for(l176 = 0;l176<l158.count;l176++){payload_t*prefix = l158.
prefix[l176];if(l129[l176]!= -1){if(l33){sal_memset(l219,0,sizeof(
defip_alpm_ipv6_64_entry_t));}else{sal_memset(l219,0,sizeof(
defip_alpm_ipv4_entry_t));}l205 = soc_mem_write(l1,l23,MEM_BLOCK_ANY,l129[
l176],l219);_soc_trident2_alpm_bkt_view_set(l1,l129[l176],INVALIDm);if(
SOC_FAILURE(l205)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l1)){l205 = soc_mem_write(l1,l23,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l129[l176]),l219);_soc_trident2_alpm_bkt_view_set(l1,
_soc_alpm_rpf_entry(l1,l129[l176]),INVALIDm);if(SOC_FAILURE(l205)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l209 = NULL;l205 = trie_delete(
PIVOT_BUCKET_TRIE(l151),prefix->key,prefix->len,&l209);l210 = (payload_t*)
l209;if(SOC_FAILURE(l205)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(prefix
->index>0){l205 = trie_insert(PIVOT_BUCKET_TRIE(l201),prefix->key,NULL,prefix
->len,(trie_node_t*)l210);if(SOC_FAILURE(l205)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l210!= NULL){sal_free(l210);}}}if(l214
== -1){l209 = NULL;l205 = trie_delete(PIVOT_BUCKET_TRIE(l201),prefix,l203,&
l209);l210 = (payload_t*)l209;if(SOC_FAILURE(l205)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l210!= 
NULL){sal_free(l210);}}l205 = alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l151)
,l33);if(SOC_FAILURE(l205)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: new bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(l151
)));}trie_destroy(PIVOT_BUCKET_TRIE(l151));sal_free(l213);sal_free(l151);
sal_free(l129);sal_free(l224);l209 = NULL;l205 = trie_delete(l207,prefix,l203
,&l209);l211 = (payload_t*)l209;if(SOC_FAILURE(l205)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l211){sal_free(
l211);}return l133;l231:if(l129!= NULL){sal_free(l129);}if(l224!= NULL){
sal_free(l224);}l209 = NULL;(void)trie_delete(l207,prefix,l203,&l209);l211 = 
(payload_t*)l209;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l33);}l230:l209 = NULL;(void)trie_delete(trie,prefix,l203,&l209)
;l210 = (payload_t*)l209;l229:if(l210!= NULL){sal_free(l210);}if(l211!= NULL)
{sal_free(l211);}return l133;}static int l29(int l20,uint32*key,int len,int
l27,int l9,defip_entry_t*lpm_entry,int l30,int l31){uint32 l236;if(l31){
sal_memset(lpm_entry,0,sizeof(defip_entry_t));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l101[(l20)]->l81),(l27&SOC_VRF_MAX(l20)));if(l27 == (SOC_VRF_MAX(l20)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l101[(l20)]->l83),(0));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l83),(SOC_VRF_MAX(l20)
));}if(l9){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(
lpm_entry),(l101[(l20)]->l65),(key[0]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l66),(key[1]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l101[(l20)]->l69),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,
L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l70),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l101[(l20)]->l82),(l27&SOC_VRF_MAX(l20)));if(l27 == (SOC_VRF_MAX(l20)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l101[(l20)]->l84),(0));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l84),(SOC_VRF_MAX(l20)
));}soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(
lpm_entry),(l101[(l20)]->l80),(1));if(len>= 32){l236 = 0xffffffff;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l101[(l20)]->l68),(l236));l236 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry
),(l101[(l20)]->l67),(l236));}else{l236 = ~(0xffffffff>>len);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l101[(l20)]->l68),(l236));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l20,L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l67),(0));}}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l101[(l20)]->l65),(key[0]));assert(len<= 32);l236 = (len == 32)?0xffffffff:~
(0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,
L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l67),(l236));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),
(l101[(l20)]->l79),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l20,
L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l71),((1<<soc_mem_field_length(l20,
L3_DEFIPm,MODE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l20,L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l72),((1<<soc_mem_field_length(l20,
L3_DEFIPm,MODE_MASK1f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l20,L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l97),((1<<soc_mem_field_length(l20,
L3_DEFIPm,ENTRY_TYPE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l98),((1<<
soc_mem_field_length(l20,L3_DEFIPm,ENTRY_TYPE_MASK1f))-1));return(SOC_E_NONE)
;}int _soc_alpm_delete_in_bkt(int l1,soc_mem_t l23,int l237,int l143,void*
l238,uint32*l14,int*l126,int l33){int l133;l133 = soc_mem_alpm_delete(l1,l23,
l237,MEM_BLOCK_ALL,l143,l238,l14,l126);if(SOC_SUCCESS(l133)){return l133;}if(
SOC_ALPM_V6_SCALE_CHECK(l1,l33)){return soc_mem_alpm_delete(l1,l23,l237+1,
MEM_BLOCK_ALL,l143,l238,l14,l126);}return l133;}static int l239(int l1,void*
l7,int bucket_index,int l148,int l126,int l240){alpm_pivot_t*l151;
defip_alpm_ipv4_entry_t l215,l216,l241;defip_alpm_ipv6_64_entry_t l217,l218,
l242;defip_aux_scratch_entry_t l12;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];
soc_mem_t l23;void*l219,*l238,*l220 = NULL;int l154;int l9;int l133 = 
SOC_E_NONE,l205;uint32 l243[5],prefix[5];int l33,l27;uint32 l146;int l237;
uint32 l10,l143;int l147,l25 = 0;trie_t*trie,*l207;uint32 l244;defip_entry_t
lpm_entry,*l245;payload_t*l210 = NULL,*l246 = NULL,*l212 = NULL;trie_node_t*
l209 = NULL,*l150 = NULL;trie_t*l149 = NULL;l9 = l33 = soc_mem_field32_get(l1
,L3_DEFIPm,l7,MODE0f);if(l33){if(!(l33 = soc_mem_field32_get(l1,L3_DEFIPm,l7,
MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,
l7,&l154,&l27));if(l154!= SOC_L3_VRF_OVERRIDE){if(l27 == SOC_VRF_MAX(l1)+1){
l10 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l143);}else{l10 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l143);}l133 = l139(l1,l7,prefix,&l146,&l25);
if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: prefix create failed\n")));return l133;}if(!
soc_alpm_mode_get(l1)){if(l154!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1
,l27,l33)>1){if(l25){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot delete default ""route if other routes are present in "
"this mode"),l27));return SOC_E_PARAM;}}}l10 = 2;}l23 = (l33)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l219 = ((l33)?((uint32*)&(l217)):(
(uint32*)&(l215)));l238 = ((l33)?((uint32*)&(l242)):((uint32*)&(l241)));
SOC_ALPM_LPM_LOCK(l1);if(bucket_index == 0){l133 = l152(l1,l7,l23,l219,&l148,
&bucket_index,&l126,TRUE);}else{l133 = l19(l1,l7,l219,0,l23,0,0);}sal_memcpy(
l238,l219,l33?sizeof(l217):sizeof(l215));if(SOC_FAILURE(l133)){
SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to find prefix for delete\n")));return l133;}l237 = 
bucket_index;l151 = ALPM_TCAM_PIVOT(l1,l148);trie = PIVOT_BUCKET_TRIE(l151);
l133 = trie_delete(trie,prefix,l146,&l209);l210 = (payload_t*)l209;if(l133!= 
SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l133;}if(l33){l207 = VRF_PREFIX_TRIE_IPV6(l1,
l27);}else{l207 = VRF_PREFIX_TRIE_IPV4(l1,l27);}if(l33){l149 = 
VRF_PIVOT_TRIE_IPV6(l1,l27);}else{l149 = VRF_PIVOT_TRIE_IPV4(l1,l27);}if(!l25
){l133 = trie_delete(l207,prefix,l146,&l209);l246 = (payload_t*)l209;if(
SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l247;}if(l240){l150 = NULL;l133 = trie_find_lpm(l207,
prefix,l146,&l150);l212 = (payload_t*)l150;if(SOC_SUCCESS(l133)){payload_t*
l248 = (payload_t*)(l212->bkt_ptr);if(l248!= NULL){l244 = l248->len;}else{
l244 = 0;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l249;}
sal_memcpy(l243,prefix,sizeof(prefix));do{if(!(l33)){l243[0] = (((32-l146) == 
32)?0:(l243[1])<<(32-l146));l243[1] = 0;}else{int l232 = 64-l146;int l233;if(
l232<32){l233 = l243[3]<<l232;l233|= (((32-l232) == 32)?0:(l243[4])>>(32-l232
));l243[0] = l243[4]<<l232;l243[1] = l233;l243[2] = l243[3] = l243[4] = 0;}
else{l243[1] = (((l232-32) == 32)?0:(l243[4])<<(l232-32));l243[0] = l243[2] = 
l243[3] = l243[4] = 0;}}}while(0);l133 = l29(l1,l243,l244,l27,l9,&lpm_entry,0
,1);l205 = l152(l1,&lpm_entry,l23,l219,&l148,&bucket_index,&l126,TRUE);(void)
l26(l1,l219,l23,l9,l154,bucket_index,0,&lpm_entry);(void)l29(l1,l243,l146,l27
,l9,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(l133)){l220 = 
((l33)?((uint32*)&(l218)):((uint32*)&(l216)));l205 = soc_mem_read(l1,l23,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l126),l220);}}if((l244 == 0)&&
SOC_FAILURE(l205)){l245 = l33?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l27);sal_memcpy(&lpm_entry,l245,sizeof(
lpm_entry));l133 = l29(l1,l243,l146,l27,l9,&lpm_entry,0,1);}if(SOC_FAILURE(
l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l249;}l245 = 
&lpm_entry;}}else{l150 = NULL;l133 = trie_find_lpm(l207,prefix,l146,&l150);
l212 = (payload_t*)l150;if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),l27));goto l247;}l212->bkt_ptr = NULL;l244 = 
0;l245 = l33?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27):VRF_TRIE_DEFAULT_ROUTE_IPV4(
l1,l27);}if(l240){l133 = l8(l1,l245,l33,l10,l244,&l12);if(SOC_FAILURE(l133)){
goto l249;}l133 = _soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l147,&l148,
&bucket_index);if(SOC_FAILURE(l133)){goto l249;}if(SOC_URPF_STATUS_GET(l1)){
uint32 l138;if(l220!= NULL){l138 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf);l138++;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l138);l138 = soc_mem_field32_get(l1,l23,
l220,SRC_DISCARDf);soc_mem_field32_set(l1,l23,&l12,SRC_DISCARDf,l138);l138 = 
soc_mem_field32_get(l1,l23,l220,DEFAULTROUTEf);soc_mem_field32_set(l1,l23,&
l12,DEFAULTROUTEf,l138);l133 = _soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE
,&l147,&l148,&bucket_index);}if(SOC_FAILURE(l133)){goto l249;}}}sal_free(l210
);if(!l25){sal_free(l246);}PIVOT_BUCKET_ENT_CNT_UPDATE(l151);if((l151->len!= 
0)&&(trie->trie == NULL)){uint32 l250[5];sal_memcpy(l250,l151->key,sizeof(
l250));do{if(!(l9)){l250[0] = (((32-l151->len) == 32)?0:(l250[1])<<(32-l151->
len));l250[1] = 0;}else{int l232 = 64-l151->len;int l233;if(l232<32){l233 = 
l250[3]<<l232;l233|= (((32-l232) == 32)?0:(l250[4])>>(32-l232));l250[0] = 
l250[4]<<l232;l250[1] = l233;l250[2] = l250[3] = l250[4] = 0;}else{l250[1] = 
(((l232-32) == 32)?0:(l250[4])<<(l232-32));l250[0] = l250[2] = l250[3] = l250
[4] = 0;}}}while(0);l29(l1,l250,l151->len,l27,l9,&lpm_entry,0,1);l133 = l6(l1
,&lpm_entry);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to ""delete pivot 0x%08x 0x%08x \n"),l151->key[0],
l151->key[1]));}}l133 = _soc_alpm_delete_in_bkt(l1,l23,l237,l143,l238,l14,&
l126,l33);if(!SOC_SUCCESS(l133)){SOC_ALPM_LPM_UNLOCK(l1);l133 = SOC_E_FAIL;
return l133;}if(SOC_URPF_STATUS_GET(l1)){l133 = soc_mem_alpm_delete(l1,l23,
SOC_ALPM_RPF_BKT_IDX(l1,l237),MEM_BLOCK_ALL,l143,l238,l14,&l147);if(!
SOC_SUCCESS(l133)){SOC_ALPM_LPM_UNLOCK(l1);l133 = SOC_E_FAIL;return l133;}}if
((l151->len!= 0)&&(trie->trie == NULL)){l133 = alpm_bucket_release(l1,
PIVOT_BUCKET_INDEX(l151),l33);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(l151)));}l133 = trie_delete(l149,l151->key,l151->len,&l209
);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l151));sal_free(PIVOT_BUCKET_HANDLE(l151));sal_free(l151);
_soc_trident2_alpm_bkt_view_set(l1,l237<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l33)){_soc_trident2_alpm_bkt_view_set(l1,(l237+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l27,l33);if(VRF_TRIE_ROUTES_CNT(l1,
l27,l33) == 0){l133 = l32(l1,l27,l33);}SOC_ALPM_LPM_UNLOCK(l1);return l133;
l249:l205 = trie_insert(l207,prefix,NULL,l146,(trie_node_t*)l246);if(
SOC_FAILURE(l205)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into pfx trie\n")
,prefix[0],prefix[1]));}l247:l205 = trie_insert(trie,prefix,NULL,l146,(
trie_node_t*)l210);if(SOC_FAILURE(l205)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into bkt trie\n"),prefix[0],prefix[1]));}
SOC_ALPM_LPM_UNLOCK(l1);return l133;}int soc_alpm_ipmc_war(int l20,int l251){
int l176,l9 = 0;int index = -1;defip_entry_t lpm_entry;soc_mem_t l23 = 
L3_DEFIPm;int count = 1;int l252 = 63;if(!soc_property_get(l20,
spn_L3_ALPM_ENABLE,0)){return SOC_E_NONE;}if(!SOC_IS_TRIDENT2PLUS(l20)){
return SOC_E_NONE;}if(SOC_WARM_BOOT(l20)){return SOC_E_NONE;}if(
soc_mem_index_count(l20,L3_DEFIP_PAIR_128m)<= 0){count = 2;}l252 = 
soc_mem_field_length(l20,ING_ACTIVE_L3_IIF_PROFILEm,RPA_ID_PROFILEf);for(l176
= 0;l176<count;l176++){sal_memset(&lpm_entry,0,soc_mem_entry_words(l20,l23)*4
);l9 = l176;soc_mem_field32_set(l20,l23,&lpm_entry,VALID0f,1);
soc_mem_field32_set(l20,l23,&lpm_entry,MODE_MASK0f,3);soc_mem_field32_set(l20
,l23,&lpm_entry,ENTRY_TYPE_MASK0f,1);soc_mem_field32_set(l20,l23,&lpm_entry,
MULTICAST_ROUTE0f,1);soc_mem_field32_set(l20,l23,&lpm_entry,GLOBAL_ROUTE0f,1)
;soc_mem_field32_set(l20,l23,&lpm_entry,RPA_ID0f,l252-1);soc_mem_field32_set(
l20,l23,&lpm_entry,EXPECTED_L3_IIF0f,16383);if(l9){soc_mem_field32_set(l20,
l23,&lpm_entry,VALID1f,1);soc_mem_field32_set(l20,l23,&lpm_entry,MODE0f,1);
soc_mem_field32_set(l20,l23,&lpm_entry,MODE1f,1);soc_mem_field32_set(l20,l23,
&lpm_entry,MODE_MASK1f,3);soc_mem_field32_set(l20,l23,&lpm_entry,
ENTRY_TYPE_MASK1f,1);soc_mem_field32_set(l20,l23,&lpm_entry,MULTICAST_ROUTE1f
,1);soc_mem_field32_set(l20,l23,&lpm_entry,GLOBAL_ROUTE1f,1);
soc_mem_field32_set(l20,l23,&lpm_entry,RPA_ID1f,l252-1);soc_mem_field32_set(
l20,l23,&lpm_entry,EXPECTED_L3_IIF1f,16383);soc_mem_field32_set(l20,l23,&
lpm_entry,IP_ADDR0f,0);soc_mem_field32_set(l20,l23,&lpm_entry,IP_ADDR_MASK0f,
0);soc_mem_field32_set(l20,l23,&lpm_entry,IP_ADDR1f,0xff000000);
soc_mem_field32_set(l20,l23,&lpm_entry,IP_ADDR_MASK1f,0xff000000);}else{
soc_mem_field32_set(l20,l23,&lpm_entry,IP_ADDR0f,0xe0000000);
soc_mem_field32_set(l20,l23,&lpm_entry,IP_ADDR_MASK0f,0xe0000000);}if(l251){
SOC_IF_ERROR_RETURN(l4(l20,&lpm_entry,&index));}else{SOC_IF_ERROR_RETURN(l6(
l20,&lpm_entry));}}return SOC_E_NONE;}int soc_alpm_init(int l1){int l176;int
l133 = SOC_E_NONE;l133 = l2(l1);SOC_IF_ERROR_RETURN(l133);l133 = l18(l1);
alpm_vrf_handle[l1] = sal_alloc((MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t),
"ALPM VRF Handles");if(alpm_vrf_handle[l1] == NULL){return SOC_E_MEMORY;}
tcam_pivot[l1] = sal_alloc(MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*),
"ALPM pivots");if(tcam_pivot[l1] == NULL){return SOC_E_MEMORY;}sal_memset(
alpm_vrf_handle[l1],0,(MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t));sal_memset(
tcam_pivot[l1],0,MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*));for(l176 = 0;l176<
MAX_PIVOT_COUNT;l176++){ALPM_TCAM_PIVOT(l1,l176) = NULL;}if(SOC_CONTROL(l1)->
alpm_bulk_retry == NULL){SOC_CONTROL(l1)->alpm_bulk_retry = sal_sem_create(
"ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_lookup_retry == 
NULL){SOC_CONTROL(l1)->alpm_lookup_retry = sal_sem_create("ALPM lookup retry"
,sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_insert_retry == NULL){
SOC_CONTROL(l1)->alpm_insert_retry = sal_sem_create("ALPM insert retry",
sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_delete_retry == NULL){SOC_CONTROL
(l1)->alpm_delete_retry = sal_sem_create("ALPM delete retry",sal_sem_BINARY,0
);}l133 = soc_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l133);return l133;}
static int l253(int l1){int l176,l133;alpm_pivot_t*l138;for(l176 = 0;l176<
MAX_PIVOT_COUNT;l176++){l138 = ALPM_TCAM_PIVOT(l1,l176);if(l138){if(
PIVOT_BUCKET_HANDLE(l138)){if(PIVOT_BUCKET_TRIE(l138)){l133 = trie_traverse(
PIVOT_BUCKET_TRIE(l138),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l133)){trie_destroy(PIVOT_BUCKET_TRIE(l138));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Unable to clear trie state for unit %d\n"),l1
));return l133;}}sal_free(PIVOT_BUCKET_HANDLE(l138));}sal_free(
ALPM_TCAM_PIVOT(l1,l176));ALPM_TCAM_PIVOT(l1,l176) = NULL;}}for(l176 = 0;l176
<= SOC_VRF_MAX(l1)+1;l176++){l133 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l1,
l176),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l133)){
trie_destroy(VRF_PREFIX_TRIE_IPV4(l1,l176));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l1,l176));
return l133;}l133 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l1,l176),
alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l133)){
trie_destroy(VRF_PREFIX_TRIE_IPV6(l1,l176));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v6 pfx trie for vrf %d\n"),l1,l176));
return l133;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l176)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l176));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l176
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l176));}sal_memset(&
alpm_vrf_handle[l1][l176],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l1][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,1,1);
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,2,1);if(SOC_ALPM_BUCKET_BMAP(l1)!= NULL){
sal_free(SOC_ALPM_BUCKET_BMAP(l1));}sal_memset(&soc_alpm_bucket[l1],0,sizeof(
soc_alpm_bucket_t));if(alpm_vrf_handle[l1])sal_free(alpm_vrf_handle[l1]);if(
tcam_pivot[l1])sal_free(tcam_pivot[l1]);alpm_vrf_handle[l1] = NULL;tcam_pivot
[l1] = NULL;return SOC_E_NONE;}int soc_alpm_deinit(int l1){l3(l1);
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_deinit(l1));SOC_IF_ERROR_RETURN(
soc_alpm_128_state_clear(l1));SOC_IF_ERROR_RETURN(l253(l1));if(SOC_CONTROL(l1
)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l1)->alpm_bulk_retry);
SOC_CONTROL(l1)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_lookup_retry
);SOC_CONTROL(l1)->alpm_lookup_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_insert_retry
);SOC_CONTROL(l1)->alpm_insert_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_delete_retry
);SOC_CONTROL(l1)->alpm_delete_retry = NULL;}return SOC_E_NONE;}static int
l254(int l1,int l27,int l33){defip_entry_t*lpm_entry,l255;int l256;int index;
int l133 = SOC_E_NONE;uint32 key[2] = {0,0};uint32 l146;alpm_bucket_handle_t*
l213;alpm_pivot_t*l151;payload_t*l246;trie_t*l257;trie_t*l258 = NULL;if(l33 == 
0){trie_init(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(l1,l27));l258 = 
VRF_PIVOT_TRIE_IPV4(l1,l27);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l1,l27));l258 = VRF_PIVOT_TRIE_IPV6(l1,l27);}if(l33 == 0)
{trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l1,l27));l257 = 
VRF_PREFIX_TRIE_IPV4(l1,l27);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6(l1,l27));l257 = VRF_PREFIX_TRIE_IPV6(l1,l27);}lpm_entry = 
sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n")));
return SOC_E_MEMORY;}l29(l1,key,0,l27,l33,lpm_entry,0,1);if(l33 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l27) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27) = lpm_entry;}if(l27 == SOC_VRF_MAX(l1)+1)
{soc_L3_DEFIPm_field32_set(l1,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l1,lpm_entry,DEFAULT_MISS0f,1);}l133 = 
alpm_bucket_assign(l1,&l256,l33);if(SOC_FAILURE(l133)){return l133;}
soc_L3_DEFIPm_field32_set(l1,lpm_entry,ALG_BKT_PTR0f,l256);sal_memcpy(&l255,
lpm_entry,sizeof(l255));l133 = l4(l1,&l255,&index);if(SOC_FAILURE(l133)){
return l133;}l213 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l213 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
return SOC_E_NONE;}sal_memset(l213,0,sizeof(*l213));l151 = sal_alloc(sizeof(
alpm_pivot_t),"Payload for Pivot");if(l151 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"soc_alpm_vrf_add: Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l213);return SOC_E_MEMORY;}l246 = sal_alloc(
sizeof(payload_t),"Payload for pfx trie key");if(l246 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n")));
sal_free(l213);sal_free(l151);return SOC_E_MEMORY;}sal_memset(l151,0,sizeof(*
l151));sal_memset(l246,0,sizeof(*l246));l146 = 0;PIVOT_BUCKET_HANDLE(l151) = 
l213;if(l33){trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l151));}else{
trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l151));}PIVOT_BUCKET_INDEX(l151
) = l256;PIVOT_BUCKET_VRF(l151) = l27;PIVOT_BUCKET_IPV6(l151) = l33;
PIVOT_BUCKET_DEF(l151) = TRUE;l151->key[0] = l246->key[0] = key[0];l151->key[
1] = l246->key[1] = key[1];l151->len = l246->len = l146;l133 = trie_insert(
l257,key,NULL,l146,&(l246->node));if(SOC_FAILURE(l133)){sal_free(l246);
sal_free(l151);sal_free(l213);return l133;}l133 = trie_insert(l258,key,NULL,
l146,(trie_node_t*)l151);if(SOC_FAILURE(l133)){trie_node_t*l209 = NULL;(void)
trie_delete(l257,key,l146,&l209);sal_free(l246);sal_free(l151);sal_free(l213)
;return l133;}index = soc_alpm_physical_idx(l1,L3_DEFIPm,index,l33);if(l33 == 
0){ALPM_TCAM_PIVOT(l1,index) = l151;PIVOT_TCAM_INDEX(l151) = index;}else{
ALPM_TCAM_PIVOT(l1,index<<1) = l151;PIVOT_TCAM_INDEX(l151) = index<<1;}
VRF_PIVOT_REF_INC(l1,l27,l33);VRF_TRIE_INIT_DONE(l1,l27,l33,1);return l133;}
static int l32(int l1,int l27,int l33){defip_entry_t*lpm_entry;int l256;int
l135;int l133 = SOC_E_NONE;uint32 key[2] = {0,0},l259[SOC_MAX_MEM_FIELD_WORDS
];payload_t*l210;alpm_pivot_t*l260;trie_node_t*l209;trie_t*l257;trie_t*l258 = 
NULL;if(l33 == 0){lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l27);}else{
lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27);}l256 = 
soc_L3_DEFIPm_field32_get(l1,lpm_entry,ALG_BKT_PTR0f);l133 = 
alpm_bucket_release(l1,l256,l33);_soc_trident2_alpm_bkt_view_set(l1,l256<<2,
INVALIDm);l133 = l17(l1,lpm_entry,(void*)l259,&l135);if(SOC_FAILURE(l133)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n"),
l27,l33));l135 = -1;}l135 = soc_alpm_physical_idx(l1,L3_DEFIPm,l135,l33);if(
l33 == 0){l260 = ALPM_TCAM_PIVOT(l1,l135);}else{l260 = ALPM_TCAM_PIVOT(l1,
l135<<1);}l133 = l6(l1,lpm_entry);if(SOC_FAILURE(l133)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),l27,l33));}sal_free(lpm_entry);if(
l33 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l27) = NULL;l257 = 
VRF_PREFIX_TRIE_IPV4(l1,l27);VRF_PREFIX_TRIE_IPV4(l1,l27) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l27) = NULL;l257 = VRF_PREFIX_TRIE_IPV6(l1,l27
);VRF_PREFIX_TRIE_IPV6(l1,l27) = NULL;}VRF_TRIE_INIT_DONE(l1,l27,l33,0);l133 = 
trie_delete(l257,key,0,&l209);l210 = (payload_t*)l209;if(SOC_FAILURE(l133)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal default for vrf "" %d/%d\n"),l27,l33));}sal_free(
l210);(void)trie_destroy(l257);if(l33 == 0){l258 = VRF_PIVOT_TRIE_IPV4(l1,l27
);VRF_PIVOT_TRIE_IPV4(l1,l27) = NULL;}else{l258 = VRF_PIVOT_TRIE_IPV6(l1,l27)
;VRF_PIVOT_TRIE_IPV6(l1,l27) = NULL;}l209 = NULL;l133 = trie_delete(l258,key,
0,&l209);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l27,l33));}(void)
trie_destroy(l258);(void)trie_destroy(PIVOT_BUCKET_TRIE(l260));sal_free(
PIVOT_BUCKET_HANDLE(l260));sal_free(l260);return l133;}int soc_alpm_insert(
int l1,void*l5,uint32 l24,int l261,int l262){defip_alpm_ipv4_entry_t l215,
l216;defip_alpm_ipv6_64_entry_t l217,l218;soc_mem_t l23;void*l219,*l238;int
l154,l27;int index;int l9;int l133 = SOC_E_NONE;uint32 l25;l9 = 
soc_mem_field32_get(l1,L3_DEFIPm,l5,MODE0f);l23 = (l9)?L3_DEFIP_ALPM_IPV6_64m
:L3_DEFIP_ALPM_IPV4m;l219 = ((l9)?((uint32*)&(l217)):((uint32*)&(l215)));l238
= ((l9)?((uint32*)&(l218)):((uint32*)&(l216)));SOC_IF_ERROR_RETURN(l19(l1,l5,
l219,l238,l23,l24,&l25));SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l5,&l154
,&l27));if(l154 == SOC_L3_VRF_OVERRIDE){l133 = l4(l1,l5,&index);if(
SOC_SUCCESS(l133)){VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l9);VRF_TRIE_ROUTES_INC(l1
,MAX_VRF_ID,l9);}else if(l133 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID
,l9);}return(l133);}else if(l27 == 0){if(soc_alpm_mode_get(l1)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"VRF=0 cannot be added in Parallel mode\n")));
return SOC_E_PARAM;}}if(l154!= SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l1)){
if(VRF_TRIE_ROUTES_CNT(l1,l27,l9) == 0){if(!l25){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n"),l154));return SOC_E_PARAM;}}}}if(!
VRF_TRIE_INIT_COMPLETED(l1,l27,l9)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_insert:VRF %d is not ""initialized\n"),l27));l133 = l254(l1,l27,
l9);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n"),l27,l9));return l133;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),l27,l9));}if(l262&
SOC_ALPM_LOOKUP_HIT){l133 = l155(l1,l5,l219,l238,l23,l261);}else{if(l261 == -
1){l261 = 0;}l133 = l200(l1,l5,l23,l219,l238,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l261),l262);}if(l133!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l1,soc_errmsg(l133)
));}return(l133);}int soc_alpm_lookup(int l1,void*l7,void*l14,int*l15,int*
l263){defip_alpm_ipv4_entry_t l215;defip_alpm_ipv6_64_entry_t l217;soc_mem_t
l23;int bucket_index;int l148;void*l219;int l154,l27;int l9,l125;int l133 = 
SOC_E_NONE;l9 = soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l7,&l154,&l27));l133 = l13(l1,l7,
l14,l15,&l125,&l9);if(SOC_SUCCESS(l133)){if(!l9&&(*l15&0x1)){l133 = 
soc_alpm_lpm_ip4entry1_to_0(l1,l14,l14,PRESERVE_HIT);}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l14,&l154,&l27));if(l154 == SOC_L3_VRF_OVERRIDE){
return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l27,l9)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_lookup:VRF %d is not initialized\n")
,l27));*l263 = 0;return SOC_E_NOT_FOUND;}l23 = (l9)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l219 = ((l9)?((uint32*)&(l217)):((uint32*)&(l215)));
SOC_ALPM_LPM_LOCK(l1);l133 = l152(l1,l7,l23,l219,&l148,&bucket_index,l15,TRUE
);SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l133)){*l263 = l148;*l15 = 
bucket_index<<2;return l133;}l133 = l26(l1,l219,l23,l9,l154,bucket_index,*l15
,l14);*l263 = SOC_ALPM_LOOKUP_HIT|l148;return(l133);}static int l264(int l1,
void*l7,void*l14,int l27,int*l148,int*bucket_index,int*l126,int l265){int l133
= SOC_E_NONE;int l176,l266,l33,l147 = 0;uint32 l10,l143;
defip_aux_scratch_entry_t l12;int l267,l268;int index;soc_mem_t l23,l269;int
l270,l271;int l272;uint32 l273[SOC_MAX_MEM_FIELD_WORDS] = {0};int l274 = -1;
int l275 = 0;soc_field_t l276[2] = {IP_ADDR0f,IP_ADDR1f,};l269 = L3_DEFIPm;
l33 = soc_mem_field32_get(l1,l269,l7,MODE0f);l267 = soc_mem_field32_get(l1,
l269,l7,GLOBAL_ROUTE0f);l268 = soc_mem_field32_get(l1,l269,l7,VRF_ID_0f);
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d:\n"),l27 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l33,l267,l268));if(l27 == SOC_L3_VRF_GLOBAL){l10 = l265?1:0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l143);soc_mem_field32_set(l1,l269,l7,
GLOBAL_ROUTE0f,1);soc_mem_field32_set(l1,l269,l7,VRF_ID_0f,0);}else{l10 = 
l265?3:2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l143);}sal_memset(&l12,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l8(l1,l7,l33,l10,0,&l12));if(
l27 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l1,l269,l7,GLOBAL_ROUTE0f,l267)
;soc_mem_field32_set(l1,l269,l7,VRF_ID_0f,l268);}SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l12,TRUE,&l147,l148,bucket_index));if(l147
== 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Could not find bucket\n")))
;return SOC_E_NOT_FOUND;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in memory %s, index %d, ""bucket_index %d\n"),SOC_MEM_NAME(l1,l269),
soc_alpm_logical_idx(l1,l269,(*l148)>>1,1),*bucket_index));l23 = (l33)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l133 = l132(l1,l7,&l271);if(
SOC_FAILURE(l133)){return l133;}l272 = 24;if(l33){if(SOC_ALPM_V6_SCALE_CHECK(
l1,l33)){l272 = 32;}else{l272 = 16;}}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Start searching mem %s bucket %d(count %d) ""for Length %d\n"),
SOC_MEM_NAME(l1,l23),*bucket_index,l272,l271));for(l176 = 0;l176<l272;l176++)
{uint32 l219[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l236[2] = {0};uint32 l277[
2] = {0};uint32 l278[2] = {0};int l279;l133 = _soc_alpm_mem_index(l1,l23,*
bucket_index,l176,l143,&index);if(l133 == SOC_E_FULL){continue;}
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l23,MEM_BLOCK_ANY,index,(void*)l219));
l279 = soc_mem_field32_get(l1,l23,l219,VALIDf);l270 = soc_mem_field32_get(l1,
l23,l219,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket %5d Index %6d: Valid %d, Length %d\n"),*bucket_index,index,l279,l270)
);if(!l279||(l270>l271)){continue;}SHR_BITSET_RANGE(l236,(l33?64:32)-l270,
l270);(void)soc_mem_field_get(l1,l23,(uint32*)l219,KEYf,(uint32*)l277);l278[1
] = soc_mem_field32_get(l1,l269,l7,l276[1]);l278[0] = soc_mem_field32_get(l1,
l269,l7,l276[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tmask %08x %08x\n\t key %08x %08x\n""\thost %08x %08x\n"),l236[1],l236[0],
l277[1],l277[0],l278[1],l278[0]));for(l266 = l33?1:0;l266>= 0;l266--){if((
l278[l266]&l236[l266])!= (l277[l266]&l236[l266])){break;}}if(l266>= 0){
continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket %d, ""index %d\n"),SOC_MEM_NAME(l1,l23),*
bucket_index,index));if(l274 == -1||l274<l270){l274 = l270;l275 = index;
sal_memcpy(l273,l219,sizeof(l219));}}if(l274!= -1){l133 = l26(l1,l273,l23,l33
,l27,*bucket_index,l275,l14);if(SOC_SUCCESS(l133)){*l126 = l275;if(bsl_check(
bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){LOG_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"Hit mem %s bucket %d, index %d\n"),SOC_MEM_NAME(l1,l23),*
bucket_index,l275));}}return l133;}*l126 = soc_alpm_logical_idx(l1,l269,(*
l148)>>1,1);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Miss in mem %s bucket %d, use associate data ""in mem %s LOG index %d\n"),
SOC_MEM_NAME(l1,l23),*bucket_index,SOC_MEM_NAME(l1,l269),*l126));
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l269,MEM_BLOCK_ANY,*l126,(void*)l14));if(
(!l33)&&((*l148)&1)){l133 = soc_alpm_lpm_ip4entry1_to_0(l1,l14,l14,
PRESERVE_HIT);}return SOC_E_NONE;}int soc_alpm_find_best_match(int l1,void*l7
,void*l14,int*l15,int l265){int l133 = SOC_E_NONE;int l176,l280,l281;
defip_entry_t l282;uint32 l283[2];uint32 l277[2];uint32 l284[2];uint32 l278[2
];uint32 l285,l286;int l154,l27 = 0;int l287[2] = {0};int l148,bucket_index;
soc_mem_t l269 = L3_DEFIPm;int l192,l33,l288,l289 = 0;soc_field_t l290[] = {
GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l291[] = {GLOBAL_ROUTE0f,
GLOBAL_ROUTE1f};l33 = soc_mem_field32_get(l1,l269,l7,MODE0f);if(!
SOC_URPF_STATUS_GET(l1)&&l265){return SOC_E_PARAM;}l280 = soc_mem_index_min(
l1,l269);l281 = soc_mem_index_count(l1,l269);if(SOC_URPF_STATUS_GET(l1)){l281
>>= 1;}if(soc_alpm_mode_get(l1)){l281>>= 1;l280+= l281;}if(l265){l280+= 
soc_mem_index_count(l1,l269)/2;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Launch LPM searching from index %d count %d\n"),l280,l281));for(l176 = l280;
l176<l280+l281;l176++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l269,MEM_BLOCK_ANY
,l176,(void*)&l282));l287[0] = soc_mem_field32_get(l1,l269,&l282,VALID0f);
l287[1] = soc_mem_field32_get(l1,l269,&l282,VALID1f);if(l287[0] == 0&&l287[1]
== 0){continue;}l288 = soc_mem_field32_get(l1,l269,&l282,MODE0f);if(l288!= 
l33){continue;}for(l192 = 0;l192<(l33?1:2);l192++){if(l287[l192] == 0){
continue;}l285 = soc_mem_field32_get(l1,l269,&l282,l290[l192]);l286 = 
soc_mem_field32_get(l1,l269,&l282,l291[l192]);if(!l285||!l286){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Match a Global High route: ent %d\n"),l192));l283[0] = soc_mem_field32_get(
l1,l269,&l282,IP_ADDR_MASK0f);l283[1] = soc_mem_field32_get(l1,l269,&l282,
IP_ADDR_MASK1f);l277[0] = soc_mem_field32_get(l1,l269,&l282,IP_ADDR0f);l277[1
] = soc_mem_field32_get(l1,l269,&l282,IP_ADDR1f);l284[0] = 
soc_mem_field32_get(l1,l269,l7,IP_ADDR_MASK0f);l284[1] = soc_mem_field32_get(
l1,l269,l7,IP_ADDR_MASK1f);l278[0] = soc_mem_field32_get(l1,l269,l7,IP_ADDR0f
);l278[1] = soc_mem_field32_get(l1,l269,l7,IP_ADDR1f);LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\thmsk %08x %08x\n\thkey %08x %08x\n"
"\tsmsk %08x %08x\n\tskey %08x %08x\n"),l283[1],l283[0],l277[1],l277[0],l284[
1],l284[0],l278[1],l278[0]));if(l33&&(((l283[1]&l284[1])!= l283[1])||((l283[0
]&l284[0])!= l283[0]))){continue;}if(!l33&&((l283[l192]&l284[0])!= l283[l192]
)){continue;}if(l33&&((l278[0]&l283[0]) == (l277[0]&l283[0]))&&((l278[1]&l283
[1]) == (l277[1]&l283[1]))){l289 = TRUE;break;}if(!l33&&((l278[0]&l283[l192])
== (l277[l192]&l283[l192]))){l289 = TRUE;break;}}if(!l289){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit Global High route in index = %d(%d)\n"),l176,l192));sal_memcpy(l14,&l282
,sizeof(l282));if(!l33&&l192 == 1){l133 = soc_alpm_lpm_ip4entry1_to_0(l1,l14,
l14,PRESERVE_HIT);}*l15 = l176;return l133;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for VRF and Global Low routes\n")));SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l7,&l154,&l27));l133 = l264(l1,l7,l14,l27,&l148,&
bucket_index,l15,l265);if(l133 == SOC_E_NOT_FOUND){l27 = SOC_L3_VRF_GLOBAL;
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Not found in VRF region, trying Global ""region\n")));l133 = l264(l1,l7,l14,
l27,&l148,&bucket_index,l15,l265);}if(SOC_SUCCESS(l133)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit in %s region in TCAM index %d, "
"buckekt_index %d\n"),l27 == SOC_L3_VRF_GLOBAL?"Global Low":"VRF",l148,
bucket_index));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Search miss for given address\n")));}return(l133);}int soc_alpm_delete(int l1
,void*l7,int l261,int l262){int l154,l27;int l9;int l133 = SOC_E_NONE;int l240
= 0;l9 = soc_mem_field32_get(l1,L3_DEFIPm,l7,MODE0f);SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l7,&l154,&l27));if(l154 == SOC_L3_VRF_OVERRIDE){l133 = 
l6(l1,l7);if(SOC_SUCCESS(l133)){VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l9);
VRF_TRIE_ROUTES_DEC(l1,MAX_VRF_ID,l9);}return(l133);}else{if(!
VRF_TRIE_INIT_COMPLETED(l1,l27,l9)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_delete:VRF %d/%d is not initialized\n"),l27,l9));return
SOC_E_NONE;}if(l261 == -1){l261 = 0;}l240 = !(l262&SOC_ALPM_DELETE_ALL);l133 = 
l239(l1,l7,SOC_ALPM_BKT_ENTRY_TO_IDX(l261),l262&~SOC_ALPM_LOOKUP_HIT,l261,
l240);}return(l133);}static int l18(int l1){int l292;l292 = 
soc_mem_index_count(l1,L3_DEFIPm)+soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)*
2;if(SOC_URPF_STATUS_GET(l1)){l292>>= 1;}SOC_ALPM_BUCKET_COUNT(l1) = l292*2;
SOC_ALPM_BUCKET_BMAP_SIZE(l1) = SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(l1));
SOC_ALPM_BUCKET_BMAP(l1) = sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l1),
"alpm_shared_bucket_bitmap");if(SOC_ALPM_BUCKET_BMAP(l1) == NULL){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_shared_mem_init: Memory allocation for "
"bucket bitmap management failed\n")));return SOC_E_MEMORY;}sal_memset(
SOC_ALPM_BUCKET_BMAP(l1),0,SOC_ALPM_BUCKET_BMAP_SIZE(l1));alpm_bucket_assign(
l1,&l292,1);return SOC_E_NONE;}int alpm_bucket_assign(int l1,int*l256,int l33
){int l176,l293 = 1,l294 = 0;if(l33){if(!soc_alpm_mode_get(l1)&&!
SOC_URPF_STATUS_GET(l1)){l293 = 2;}}for(l176 = 0;l176<SOC_ALPM_BUCKET_COUNT(
l1);l176+= l293){SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l176,l293,l294);
if(0 == l294){break;}}if(l176 == SOC_ALPM_BUCKET_COUNT(l1)){return SOC_E_FULL
;}SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l176,l293);*l256 = l176;
SOC_ALPM_BUCKET_NEXT_FREE(l1) = l176;return SOC_E_NONE;}int
alpm_bucket_release(int l1,int l256,int l33){int l293 = 1,l294 = 0;if((l256<1
)||(l256>SOC_ALPM_BUCKET_MAX_INDEX(l1))){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Unit %d\n, freeing invalid bucket index %d\n"),l1,l256));
return SOC_E_PARAM;}if(l33){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(
l1)){l293 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l256,l293,l294);if
(!l294){return SOC_E_PARAM;}SHR_BITCLR_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l256,
l293);return SOC_E_NONE;}int alpm_bucket_is_assigned(int l1,int l295,int l9,
int*l294){int l293 = 1;if((l295<1)||(l295>SOC_ALPM_BUCKET_MAX_INDEX(l1))){
return SOC_E_PARAM;}if(l9){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1
)){l293 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l295,l293,*l294);
return SOC_E_NONE;}static void l113(int l1,void*l14,int index,l108 l114){if(
index&(0x8000)){l114[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l14),(l101[(l1)]->l65));l114[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l67));l114[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l101[(l1)]->l66));l114[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l101[(l1)]->l81)!= NULL))){int
l296;(void)soc_alpm_lpm_vrf_get(l1,l14,(int*)&l114[4],&l296);}else{l114[4] = 
0;};}else{if(index&0x1){l114[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(l1)]->l66));l114[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l68));l114[2] = 0;l114[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l101[(l1)]->l82)!= NULL))){int l296;defip_entry_t l297;(void)
soc_alpm_lpm_ip4entry1_to_0(l1,l14,&l297,0);(void)soc_alpm_lpm_vrf_get(l1,&
l297,(int*)&l114[4],&l296);}else{l114[4] = 0;};}else{l114[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l65));l114[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l101[(l1)]->l67));l114[2] = 0;l114[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l101[(l1)]->l81)!= NULL))){int l296;(void)
soc_alpm_lpm_vrf_get(l1,l14,(int*)&l114[4],&l296);}else{l114[4] = 0;};}}}
static int l298(l108 l110,l108 l111){int l135;for(l135 = 0;l135<5;l135++){{if
((l110[l135])<(l111[l135])){return-1;}if((l110[l135])>(l111[l135])){return 1;
}};}return(0);}static void l299(int l1,void*l5,uint32 l300,uint32 l128,int
l125){l108 l301;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l101[(l1)]->l69))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l101[(l1)]->l80))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l101[(
l1)]->l79))){l301[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l101[(l1)]->l65));l301[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l101[(
l1)]->l67));l301[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l101[(l1)]->l66));l301[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l101[(
l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l101[(l1)]->l81)!= NULL))){int
l296;(void)soc_alpm_lpm_vrf_get(l1,l5,(int*)&l301[4],&l296);}else{l301[4] = 0
;};l127((l112[(l1)]),l298,l301,l125,l128,((uint16)l300<<1)|(0x8000));}}else{
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l101
[(l1)]->l79))){l301[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l101[(l1)]->l65));l301[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l101[(
l1)]->l67));l301[2] = 0;l301[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l101[(l1)]->l81)!= NULL))){int l296;(void)soc_alpm_lpm_vrf_get(l1,l5,(int*)&
l301[4],&l296);}else{l301[4] = 0;};l127((l112[(l1)]),l298,l301,l125,l128,((
uint16)l300<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l101[(l1)]->l80))){l301[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l101[(
l1)]->l66));l301[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l101[(l1)]->l68));l301[2] = 0;l301[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l101[(l1)]->l82)!= NULL))){int l296;defip_entry_t
l297;(void)soc_alpm_lpm_ip4entry1_to_0(l1,l5,&l297,0);(void)
soc_alpm_lpm_vrf_get(l1,&l297,(int*)&l301[4],&l296);}else{l301[4] = 0;};l127(
(l112[(l1)]),l298,l301,l125,l128,(((uint16)l300<<1)+1));}}}static void l302(
int l1,void*l7,uint32 l300){l108 l301;int l125 = -1;int l133;uint16 index;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(
l1)]->l69))){l301[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l7),(l101[(l1)]->l65));l301[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(
l1)]->l67));l301[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l7),(l101[(l1)]->l66));l301[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(
l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l101[(l1)]->l81)!= NULL))){int
l296;(void)soc_alpm_lpm_vrf_get(l1,l7,(int*)&l301[4],&l296);}else{l301[4] = 0
;};index = (l300<<1)|(0x8000);}else{l301[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(
l1)]->l65));l301[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l7),(l101[(l1)]->l67));l301[2] = 0;l301[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l101[(l1)]->l81)!= NULL))){int l296;(void)
soc_alpm_lpm_vrf_get(l1,l7,(int*)&l301[4],&l296);}else{l301[4] = 0;};index = 
l300;}l133 = l130((l112[(l1)]),l298,l301,l125,index);if(SOC_FAILURE(l133)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\ndel  index: H %d error %d\n"),
index,l133));}}static int l303(int l1,void*l7,int l125,int*l126){l108 l301;
int l304;int l133;uint16 index = (0xFFFF);l304 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(
l1)]->l69));if(l304){l301[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(l1)]->l65));l301[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(
l1)]->l67));l301[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l7),(l101[(l1)]->l66));l301[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(
l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l101[(l1)]->l81)!= NULL))){int
l296;(void)soc_alpm_lpm_vrf_get(l1,l7,(int*)&l301[4],&l296);}else{l301[4] = 0
;};}else{l301[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l7),(l101[(l1)]->l65));l301[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(
l1)]->l67));l301[2] = 0;l301[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l101[(l1)]->l81)!= NULL))){int l296;(void)soc_alpm_lpm_vrf_get(l1,l7,(int*)&
l301[4],&l296);}else{l301[4] = 0;};}l133 = l122((l112[(l1)]),l298,l301,l125,&
index);if(SOC_FAILURE(l133)){*l126 = 0xFFFFFFFF;return(l133);}*l126 = index;
return(SOC_E_NONE);}static uint16 l115(uint8*l116,int l117){return(
_shr_crc16b(0,l116,l117));}static int l118(int l20,int l103,int l104,l107**
l119){l107*l123;int index;if(l104>l103){return SOC_E_MEMORY;}l123 = sal_alloc
(sizeof(l107),"lpm_hash");if(l123 == NULL){return SOC_E_MEMORY;}sal_memset(
l123,0,sizeof(*l123));l123->l20 = l20;l123->l103 = l103;l123->l104 = l104;
l123->l105 = sal_alloc(l123->l104*sizeof(*(l123->l105)),"hash_table");if(l123
->l105 == NULL){sal_free(l123);return SOC_E_MEMORY;}l123->l106 = sal_alloc(
l123->l103*sizeof(*(l123->l106)),"link_table");if(l123->l106 == NULL){
sal_free(l123->l105);sal_free(l123);return SOC_E_MEMORY;}for(index = 0;index<
l123->l104;index++){l123->l105[index] = (0xFFFF);}for(index = 0;index<l123->
l103;index++){l123->l106[index] = (0xFFFF);}*l119 = l123;return SOC_E_NONE;}
static int l120(l107*l121){if(l121!= NULL){sal_free(l121->l105);sal_free(l121
->l106);sal_free(l121);}return SOC_E_NONE;}static int l122(l107*l123,l109 l124
,l108 entry,int l125,uint16*l126){int l1 = l123->l20;uint16 l305;uint16 index
;l305 = l115((uint8*)entry,(32*5))%l123->l104;index = l123->l105[l305];;;
while(index!= (0xFFFF)){uint32 l14[SOC_MAX_MEM_FIELD_WORDS];l108 l114;int l306
;l306 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,l306,l14));l113(l1,l14,index,l114);if((*l124)(entry,l114) == 0)
{*l126 = (index&(0x7FFF))>>((index&(0x8000))?1:0);;return(SOC_E_NONE);}index = 
l123->l106[index&(0x7FFF)];;};return(SOC_E_NOT_FOUND);}static int l127(l107*
l123,l109 l124,l108 entry,int l125,uint16 l128,uint16 l129){int l1 = l123->
l20;uint16 l305;uint16 index;uint16 l307;l305 = l115((uint8*)entry,(32*5))%
l123->l104;index = l123->l105[l305];;;;l307 = (0xFFFF);if(l128!= (0xFFFF)){
while(index!= (0xFFFF)){uint32 l14[SOC_MAX_MEM_FIELD_WORDS];l108 l114;int l306
;l306 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,l306,l14));l113(l1,l14,index,l114);if((*l124)(entry,l114) == 0)
{if(l129!= index){;if(l307 == (0xFFFF)){l123->l105[l305] = l129;l123->l106[
l129&(0x7FFF)] = l123->l106[index&(0x7FFF)];l123->l106[index&(0x7FFF)] = (
0xFFFF);}else{l123->l106[l307&(0x7FFF)] = l129;l123->l106[l129&(0x7FFF)] = 
l123->l106[index&(0x7FFF)];l123->l106[index&(0x7FFF)] = (0xFFFF);}};return(
SOC_E_NONE);}l307 = index;index = l123->l106[index&(0x7FFF)];;}}l123->l106[
l129&(0x7FFF)] = l123->l105[l305];l123->l105[l305] = l129;return(SOC_E_NONE);
}static int l130(l107*l123,l109 l124,l108 entry,int l125,uint16 l131){uint16
l305;uint16 index;uint16 l307;l305 = l115((uint8*)entry,(32*5))%l123->l104;
index = l123->l105[l305];;;l307 = (0xFFFF);while(index!= (0xFFFF)){if(l131 == 
index){;if(l307 == (0xFFFF)){l123->l105[l305] = l123->l106[l131&(0x7FFF)];
l123->l106[l131&(0x7FFF)] = (0xFFFF);}else{l123->l106[l307&(0x7FFF)] = l123->
l106[l131&(0x7FFF)];l123->l106[l131&(0x7FFF)] = (0xFFFF);}return(SOC_E_NONE);
}l307 = index;index = l123->l106[index&(0x7FFF)];;}return(SOC_E_NOT_FOUND);}
int _ipmask2pfx(uint32 l308,int*l309){*l309 = 0;while(l308&(1<<31)){*l309+= 1
;l308<<= 1;}return((l308)?SOC_E_PARAM:SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_0(int l1,void*l310,void*l311,int l312){uint32 l134;
l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310)
,(l101[(l1)]->l79));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l311),(l101[(l1)]->l79),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l69));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l69),(l134));if(SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,
MULTICAST_ROUTE0f)){l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l57));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l57),(l134));}if(
SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,RPA_ID0f)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l59),(l134));}if(SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,
EXPECTED_L3_IIF0f)){l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l61),(l134));}l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l65));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l65),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l67));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l67),(l134));if(((l101[(l1)]->l49)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l49),(l134));}if(((l101[(l1)]->l51)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l51),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l53));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l53),(l134));}else{l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l73));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l73),(l134));}l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l75));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l75),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l77),(l134));if(((l101[(l1)]->l81)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l81));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l81),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l83));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l83),(l134));}if(((l101[(l1)]->l47)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l47));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l47),(l134));}if(l312){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l63));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l63),(l134));}l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l85));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l85),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l87),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l89),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l91),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l93));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l93),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l95),(l134));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_1(int l1,void*l310,void*l311,int l312){uint32 l134;
l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310)
,(l101[(l1)]->l80));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l311),(l101[(l1)]->l80),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l70),(l134));if(SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,
MULTICAST_ROUTE1f)){l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l58));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l58),(l134));}if(
SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,RPA_ID1f)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l60),(l134));}if(SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,
EXPECTED_L3_IIF1f)){l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l62));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l62),(l134));}l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l66),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l68));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l68),(l134));if(((l101[(l1)]->l50)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l50),(l134));}if(((l101[(l1)]->l52)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l52));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l52),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l54));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l54),(l134));}else{l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l74));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l74),(l134));}l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l76),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l78),(l134));if(((l101[(l1)]->l82)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l82));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l82),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l84));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l84),(l134));}if(((l101[(l1)]->l48)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l48),(l134));}if(l312){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l64));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l64),(l134));}l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l86));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l86),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l88),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l90),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l92),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l94));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l94),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l96),(l134));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_1(int l1,void*l310,void*l311,int l312){uint32 l134;
l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310)
,(l101[(l1)]->l79));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l311),(l101[(l1)]->l80),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l69));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l70),(l134));if(SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,
MULTICAST_ROUTE0f)){l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l57));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l58),(l134));}if(
SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,RPA_ID0f)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l60),(l134));}if(SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,
EXPECTED_L3_IIF0f)){l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l62),(l134));}l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l65));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l66),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l67));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l68),(l134));if(!SOC_IS_HURRICANE(l1)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l50),(l134));}if(((l101[(l1)]->l51)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l52),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l53));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l54),(l134));}else{l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l73));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l74),(l134));}l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l75));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l76),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l78),(l134));if(((l101[(l1)]->l81)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l81));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l82),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l83));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l84),(l134));}if(((l101[(l1)]->l47)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l47));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l48),(l134));}if(l312){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l63));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l64),(l134));}l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l85));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l86),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l88),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l90),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l92),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l93));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l94),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l96),(l134));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_0(int l1,void*l310,void*l311,int l312){uint32 l134;
l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310)
,(l101[(l1)]->l80));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l311),(l101[(l1)]->l79),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l69),(l134));if(SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,
MULTICAST_ROUTE1f)){l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l58));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l57),(l134));}if(
SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,RPA_ID1f)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l59),(l134));}if(SOC_MEM_FIELD_VALID(l1,L3_DEFIPm,
EXPECTED_L3_IIF1f)){l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l62));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l61),(l134));}l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l65),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l68));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l67),(l134));if(!SOC_IS_HURRICANE(l1)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l49),(l134));}if(((l101[(l1)]->l52)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l52));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l51),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l54));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l53),(l134));}else{l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l74));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l73),(l134));}l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l75),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l77),(l134));if(((l101[(l1)]->l82)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l82));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l81),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l84));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l83),(l134));}if(((l101[(l1)]->l48)!= NULL)){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l47),(l134));}if(l312){l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l64));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l63),(l134));}l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l86));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l85),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l87),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l89),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[
(l1)]->l91),(l134));l134 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l310),(l101[(l1)]->l94));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l311),(l101[(l1)]->l93),(l134));l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l310),(l101[
(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l311),(l101[(l1)]->l95),(l134));return(SOC_E_NONE);}static int l313(int l1,
void*l14){return(SOC_E_NONE);}void soc_alpm_lpm_state_dump(int l1){int l176;
int l314;l314 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l1)){return;}for(l176 = l314;l176>= 0;l176--){if((l176!= (
(3*(64+32+2+1))-1))&&((l43[(l1)][(l176)].l36) == -1)){continue;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l176,(l43
[(l1)][(l176)].l38),(l43[(l1)][(l176)].next),(l43[(l1)][(l176)].l36),(l43[(l1
)][(l176)].l37),(l43[(l1)][(l176)].l39),(l43[(l1)][(l176)].l40)));}
COMPILER_REFERENCE(l313);}static int l315(int l1,int index,uint32*l14){int
l316;int l9;uint32 l317;uint32 l318;int l319;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l316 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l1)){l316 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1)+0x0400;}else{l316 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}l9 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(l1)]->l69));if(((l101[(l1)]->l47)
!= NULL)){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l14),(l101[(l1)]->l47),(0));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l14),(l101[(l1)]->l48),(0));}l317 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l67));l318 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l101[(l1)]->l68));if(!l9){if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l79))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l14),(l101[(l1)]->l77),((!l317)?1:0));}if(soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(l1)]->l80))){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l78),((!l318)?1:0));}}else{l319 = ((!l317)&&(!l318))?1:0;l317 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l79));l318 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l101[(l1)]->l80));if(l317&&l318){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l77),(l319));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l101[(l1)]->l78),(l319));}}return l171(l1,MEM_BLOCK_ANY,
index+l316,index,l14);}static int l320(int l1,int l321,int l322){uint32 l14[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,
l321,l14));l299(l1,l14,l322,0x4000,0);SOC_IF_ERROR_RETURN(l171(l1,
MEM_BLOCK_ANY,l322,l321,l14));SOC_IF_ERROR_RETURN(l315(l1,l322,l14));do{int
l323,l324;l323 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l321),1);l324 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l322),1);ALPM_TCAM_PIVOT(l1,l324<<1) = 
ALPM_TCAM_PIVOT(l1,l323<<1);ALPM_TCAM_PIVOT(l1,(l324<<1)+1) = ALPM_TCAM_PIVOT
(l1,(l323<<1)+1);if(ALPM_TCAM_PIVOT((l1),l324<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l1),l324<<1)) = l324<<1;}if(ALPM_TCAM_PIVOT((l1),(l324<<1)+1
)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(l324<<1)+1)) = (l324<<1)+1;}
ALPM_TCAM_PIVOT(l1,l323<<1) = NULL;ALPM_TCAM_PIVOT(l1,(l323<<1)+1) = NULL;}
while(0);return(SOC_E_NONE);}static int l325(int l1,int l125,int l9){uint32
l14[SOC_MAX_MEM_FIELD_WORDS];int l321;int l322;uint32 l326,l327;l322 = (l43[(
l1)][(l125)].l37)+1;if(!l9){l321 = (l43[(l1)][(l125)].l37);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l321,l14));l326 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l79));l327 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l101[(l1)]->l80));if((l326 == 0)||(l327 == 0)){l299(l1,l14
,l322,0x4000,0);SOC_IF_ERROR_RETURN(l171(l1,MEM_BLOCK_ANY,l322,l321,l14));
SOC_IF_ERROR_RETURN(l315(l1,l322,l14));do{int l328 = soc_alpm_physical_idx((
l1),L3_DEFIPm,(l321),1)<<1;int l233 = soc_alpm_physical_idx((l1),L3_DEFIPm,(
l322),1)*2+(l328&1);if((l327)){l328++;}ALPM_TCAM_PIVOT((l1),l233) = 
ALPM_TCAM_PIVOT((l1),l328);if(ALPM_TCAM_PIVOT((l1),l233)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l1),l233)) = l233;}ALPM_TCAM_PIVOT((l1),l328) = NULL;}while(
0);l322--;}}l321 = (l43[(l1)][(l125)].l36);if(l321!= l322){
SOC_IF_ERROR_RETURN(l320(l1,l321,l322));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l9)
;}(l43[(l1)][(l125)].l36)+= 1;(l43[(l1)][(l125)].l37)+= 1;return(SOC_E_NONE);
}static int l329(int l1,int l125,int l9){uint32 l14[SOC_MAX_MEM_FIELD_WORDS];
int l321;int l322;int l330;uint32 l326,l327;l322 = (l43[(l1)][(l125)].l36)-1;
if((l43[(l1)][(l125)].l39) == 0){(l43[(l1)][(l125)].l36) = l322;(l43[(l1)][(
l125)].l37) = l322-1;return(SOC_E_NONE);}if((!l9)&&((l43[(l1)][(l125)].l37)!= 
(l43[(l1)][(l125)].l36))){l321 = (l43[(l1)][(l125)].l37);SOC_IF_ERROR_RETURN(
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l321,l14));l326 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l79));l327 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l101[(l1)]->l80));if((l326 == 0)||(l327 == 0)){l330 = l321
-1;SOC_IF_ERROR_RETURN(l320(l1,l330,l322));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,
l9);l299(l1,l14,l330,0x4000,0);SOC_IF_ERROR_RETURN(l171(l1,MEM_BLOCK_ANY,l330
,l321,l14));SOC_IF_ERROR_RETURN(l315(l1,l330,l14));do{int l328 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l321),1)<<1;int l233 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l330),1)*2+(l328&1);if((l327)){l328++;}
ALPM_TCAM_PIVOT((l1),l233) = ALPM_TCAM_PIVOT((l1),l328);if(ALPM_TCAM_PIVOT((
l1),l233)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l233)) = l233;}
ALPM_TCAM_PIVOT((l1),l328) = NULL;}while(0);}else{l299(l1,l14,l322,0x4000,0);
SOC_IF_ERROR_RETURN(l171(l1,MEM_BLOCK_ANY,l322,l321,l14));SOC_IF_ERROR_RETURN
(l315(l1,l322,l14));do{int l323,l324;l323 = soc_alpm_physical_idx((l1),
L3_DEFIPm,(l321),1);l324 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l322),1);
ALPM_TCAM_PIVOT(l1,l324<<1) = ALPM_TCAM_PIVOT(l1,l323<<1);ALPM_TCAM_PIVOT(l1,
(l324<<1)+1) = ALPM_TCAM_PIVOT(l1,(l323<<1)+1);if(ALPM_TCAM_PIVOT((l1),l324<<
1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l324<<1)) = l324<<1;}if(
ALPM_TCAM_PIVOT((l1),(l324<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(
l324<<1)+1)) = (l324<<1)+1;}ALPM_TCAM_PIVOT(l1,l323<<1) = NULL;
ALPM_TCAM_PIVOT(l1,(l323<<1)+1) = NULL;}while(0);}}else{l321 = (l43[(l1)][(
l125)].l37);SOC_IF_ERROR_RETURN(l320(l1,l321,l322));VRF_PIVOT_SHIFT_INC(l1,
MAX_VRF_ID,l9);}(l43[(l1)][(l125)].l36)-= 1;(l43[(l1)][(l125)].l37)-= 1;
return(SOC_E_NONE);}static int l331(int l1,int l125,int l9,void*l14,int*l332)
{int l333;int l334;int l335;int l336;int l321;uint32 l326,l327;int l133;if((
l43[(l1)][(l125)].l39) == 0){l336 = ((3*(64+32+2+1))-1);if(soc_alpm_mode_get(
l1) == SOC_ALPM_MODE_PARALLEL){if(l125<= (((3*(64+32+2+1))/3)-1)){l336 = (((3
*(64+32+2+1))/3)-1);}}while((l43[(l1)][(l336)].next)>l125){l336 = (l43[(l1)][
(l336)].next);}l334 = (l43[(l1)][(l336)].next);if(l334!= -1){(l43[(l1)][(l334
)].l38) = l125;}(l43[(l1)][(l125)].next) = (l43[(l1)][(l336)].next);(l43[(l1)
][(l125)].l38) = l336;(l43[(l1)][(l336)].next) = l125;(l43[(l1)][(l125)].l40)
= ((l43[(l1)][(l336)].l40)+1)/2;(l43[(l1)][(l336)].l40)-= (l43[(l1)][(l125)].
l40);(l43[(l1)][(l125)].l36) = (l43[(l1)][(l336)].l37)+(l43[(l1)][(l336)].l40
)+1;(l43[(l1)][(l125)].l37) = (l43[(l1)][(l125)].l36)-1;(l43[(l1)][(l125)].
l39) = 0;}else if(!l9){l321 = (l43[(l1)][(l125)].l36);if((l133 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l321,l14))<0){return l133;}l326 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l79));l327 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l101[(l1)]->l80));if((l326 == 0)||(l327 == 0)){*l332 = (
l321<<1)+((l327 == 0)?1:0);return(SOC_E_NONE);}l321 = (l43[(l1)][(l125)].l37)
;if((l133 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l321,l14))<0){return l133;}l326 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l14),(l101[(
l1)]->l79));l327 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l14),(l101[(l1)]->l80));if((l326 == 0)||(l327 == 0)){*l332 = (
l321<<1)+((l327 == 0)?1:0);return(SOC_E_NONE);}}l335 = l125;while((l43[(l1)][
(l335)].l40) == 0){l335 = (l43[(l1)][(l335)].next);if(l335 == -1){l335 = l125
;break;}}while((l43[(l1)][(l335)].l40) == 0){l335 = (l43[(l1)][(l335)].l38);
if(l335 == -1){if((l43[(l1)][(l125)].l39) == 0){l333 = (l43[(l1)][(l125)].l38
);l334 = (l43[(l1)][(l125)].next);if(-1!= l333){(l43[(l1)][(l333)].next) = 
l334;}if(-1!= l334){(l43[(l1)][(l334)].l38) = l333;}}return(SOC_E_FULL);}}
while(l335>l125){l334 = (l43[(l1)][(l335)].next);SOC_IF_ERROR_RETURN(l329(l1,
l334,l9));(l43[(l1)][(l335)].l40)-= 1;(l43[(l1)][(l334)].l40)+= 1;l335 = l334
;}while(l335<l125){SOC_IF_ERROR_RETURN(l325(l1,l335,l9));(l43[(l1)][(l335)].
l40)-= 1;l333 = (l43[(l1)][(l335)].l38);(l43[(l1)][(l333)].l40)+= 1;l335 = 
l333;}(l43[(l1)][(l125)].l39)+= 1;(l43[(l1)][(l125)].l40)-= 1;(l43[(l1)][(
l125)].l37)+= 1;*l332 = (l43[(l1)][(l125)].l37)<<((l9)?0:1);sal_memcpy(l14,
soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);return(
SOC_E_NONE);}static int l337(int l1,int l125,int l9,void*l14,int l338){int
l333;int l334;int l321;int l322;uint32 l339[SOC_MAX_MEM_FIELD_WORDS];uint32
l340[SOC_MAX_MEM_FIELD_WORDS];uint32 l341[SOC_MAX_MEM_FIELD_WORDS];void*l342;
int l133;int l343,l161;l321 = (l43[(l1)][(l125)].l37);l322 = l338;if(!l9){
l322>>= 1;if((l133 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l321,l339))<0){return
l133;}if((l133 = READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
soc_alpm_physical_idx(l1,L3_DEFIPm,l321,1),l340))<0){return l133;}if((l133 = 
READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,soc_alpm_physical_idx(l1,L3_DEFIPm,
l322,1),l341))<0){return l133;}l342 = (l322 == l321)?l339:l14;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l339),(l101[
(l1)]->l80))){l161 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l340,
BPM_LENGTH1f);if(l338&1){l133 = soc_alpm_lpm_ip4entry1_to_1(l1,l339,l342,
PRESERVE_HIT);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l341,BPM_LENGTH1f,
l161);}else{l133 = soc_alpm_lpm_ip4entry1_to_0(l1,l339,l342,PRESERVE_HIT);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l341,BPM_LENGTH0f,l161);}l343 = (
l321<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l339),(l101[(l1)]->l80),(0));}else{l161 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l340,BPM_LENGTH0f);if(l338&1){l133 = 
soc_alpm_lpm_ip4entry0_to_1(l1,l339,l342,PRESERVE_HIT);soc_mem_field32_set(l1
,L3_DEFIP_AUX_TABLEm,l341,BPM_LENGTH1f,l161);}else{l133 = 
soc_alpm_lpm_ip4entry0_to_0(l1,l339,l342,PRESERVE_HIT);soc_mem_field32_set(l1
,L3_DEFIP_AUX_TABLEm,l341,BPM_LENGTH0f,l161);}l343 = l321<<1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l339),(l101[
(l1)]->l79),(0));(l43[(l1)][(l125)].l39)-= 1;(l43[(l1)][(l125)].l40)+= 1;(l43
[(l1)][(l125)].l37)-= 1;}l343 = soc_alpm_physical_idx(l1,L3_DEFIPm,l343,0);
l338 = soc_alpm_physical_idx(l1,L3_DEFIPm,l338,0);ALPM_TCAM_PIVOT(l1,l338) = 
ALPM_TCAM_PIVOT(l1,l343);if(ALPM_TCAM_PIVOT(l1,l338)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l1,l338)) = l338;}ALPM_TCAM_PIVOT(l1,l343) = NULL;if((l133 = 
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ALL,soc_alpm_physical_idx(l1,L3_DEFIPm
,l322,1),l341))<0){return l133;}if(l322!= l321){l299(l1,l342,l322,0x4000,0);
if((l133 = l171(l1,MEM_BLOCK_ANY,l322,l322,l342))<0){return l133;}if((l133 = 
l315(l1,l322,l342))<0){return l133;}}l299(l1,l339,l321,0x4000,0);if((l133 = 
l171(l1,MEM_BLOCK_ANY,l321,l321,l339))<0){return l133;}if((l133 = l315(l1,
l321,l339))<0){return l133;}}else{(l43[(l1)][(l125)].l39)-= 1;(l43[(l1)][(
l125)].l40)+= 1;(l43[(l1)][(l125)].l37)-= 1;if(l322!= l321){if((l133 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l321,l339))<0){return l133;}l299(l1,l339,l322
,0x4000,0);if((l133 = l171(l1,MEM_BLOCK_ANY,l322,l321,l339))<0){return l133;}
if((l133 = l315(l1,l322,l339))<0){return l133;}}l338 = soc_alpm_physical_idx(
l1,L3_DEFIPm,l322,1);l343 = soc_alpm_physical_idx(l1,L3_DEFIPm,l321,1);
ALPM_TCAM_PIVOT(l1,l338<<1) = ALPM_TCAM_PIVOT(l1,l343<<1);ALPM_TCAM_PIVOT(l1,
l343<<1) = NULL;if(ALPM_TCAM_PIVOT(l1,l338<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l1,l338<<1)) = l338<<1;}sal_memcpy(l339,soc_mem_entry_null(l1
,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);l299(l1,l339,l321,0x4000,0);
if((l133 = l171(l1,MEM_BLOCK_ANY,l321,l321,l339))<0){return l133;}if((l133 = 
l315(l1,l321,l339))<0){return l133;}}if((l43[(l1)][(l125)].l39) == 0){l333 = 
(l43[(l1)][(l125)].l38);assert(l333!= -1);l334 = (l43[(l1)][(l125)].next);(
l43[(l1)][(l333)].next) = l334;(l43[(l1)][(l333)].l40)+= (l43[(l1)][(l125)].
l40);(l43[(l1)][(l125)].l40) = 0;if(l334!= -1){(l43[(l1)][(l334)].l38) = l333
;}(l43[(l1)][(l125)].next) = -1;(l43[(l1)][(l125)].l38) = -1;(l43[(l1)][(l125
)].l36) = -1;(l43[(l1)][(l125)].l37) = -1;}return(l133);}int
soc_alpm_lpm_vrf_get(int l20,void*lpm_entry,int*l27,int*l344){int l154;if(((
l101[(l20)]->l83)!= NULL)){l154 = soc_L3_DEFIPm_field32_get(l20,lpm_entry,
VRF_ID_0f);*l344 = l154;if(soc_L3_DEFIPm_field32_get(l20,lpm_entry,
VRF_ID_MASK0f)){*l27 = l154;}else if(!soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l20,L3_DEFIPm)),(lpm_entry),(l101[(l20)]->l85))){*l27 = 
SOC_L3_VRF_GLOBAL;*l344 = SOC_VRF_MAX(l20)+1;}else{*l27 = SOC_L3_VRF_OVERRIDE
;}}else{*l27 = SOC_L3_VRF_DEFAULT;}return(SOC_E_NONE);}static int l34(int l1,
void*entry,int*l16){int l125;int l133;int l9;uint32 l134;int l154;int l345;l9
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(
l101[(l1)]->l69));if(l9){l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l101[(l1)]->l67));if((l133 = _ipmask2pfx
(l134,&l125))<0){return(l133);}l134 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l101[(l1)]->l68));if(l125){if(l134!= 
0xffffffff){return(SOC_E_PARAM);}l125+= 32;}else{if((l133 = _ipmask2pfx(l134,
&l125))<0){return(l133);}}l125+= 33;}else{l134 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l101
[(l1)]->l67));if((l133 = _ipmask2pfx(l134,&l125))<0){return(l133);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,entry,&l154,&l133));l345 = 
soc_alpm_mode_get(l1);switch(l154){case SOC_L3_VRF_GLOBAL:if(l345 == 
SOC_ALPM_MODE_PARALLEL){*l16 = l125+((3*(64+32+2+1))/3);}else{*l16 = l125;}
break;case SOC_L3_VRF_OVERRIDE:*l16 = l125+2*((3*(64+32+2+1))/3);break;
default:if(l345 == SOC_ALPM_MODE_PARALLEL){*l16 = l125;}else{*l16 = l125+((3*
(64+32+2+1))/3);}break;}return(SOC_E_NONE);}static int l13(int l1,void*l7,
void*l14,int*l15,int*l16,int*l9){int l133;int l33;int l126;int l125 = 0;l33 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(
l1)]->l69));if(l33){if(!(l33 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l7),(l101[(l1)]->l70)))){return(SOC_E_PARAM);}}*
l9 = l33;l34(l1,l7,&l125);*l16 = l125;if(l303(l1,l7,l125,&l126) == SOC_E_NONE
){*l15 = l126;if((l133 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,(*l9)?*l15:(*l15>>1)
,l14))<0){return l133;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}
static int l2(int l1){int l314;int l176;int l292;int l346;uint32 l347 = 0;
uint32 l345;if(!soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}
if((l345 = soc_property_get(l1,spn_L3_ALPM_ENABLE,0))){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l1,&l347));soc_reg_field_set(l1,
L3_DEFIP_RPF_CONTROLr,&l347,LPM_MODEf,1);if(l345 == SOC_ALPM_MODE_PARALLEL){
soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l347,LOOKUP_MODEf,1);}else{
soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l347,LOOKUP_MODEf,0);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l1,l347));l347 = 0;if(
SOC_URPF_STATUS_GET(l1)){soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l347,
URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l347,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l347,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l347,
URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l1,l347))
;l347 = 0;if(l345 == SOC_ALPM_MODE_PARALLEL){if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l347,TCAM2_SELf,1);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l347,TCAM3_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM4_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM6_SELf,3);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM7_SELf,3);}else{soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM4_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM5_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM6_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l347,TCAM4_SELf,2);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l347,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM6_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l347,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l1,l347));if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l348 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l1,&l348));soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&
l348,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l348,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l348,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l348,
V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l1,l348))
;}}l314 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l1);l346 = sizeof(l41)*(l314);if(
(l43[(l1)]!= NULL)){if(soc_alpm_deinit(l1)<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}}l101[l1] = sal_alloc(sizeof(l99),"lpm_field_state");if(NULL
== l101[l1]){SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}(l101[l1])->l45 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,CLASS_ID0f);(l101[l1])->l46 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,CLASS_ID1f);(l101[l1])->l47 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD0f);(l101[l1])->l48 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD1f);(l101[l1])->l49 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP0f);(l101[l1])->l50 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP1f);(l101[l1])->l51 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT0f);(l101[l1])->l52 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT1f);(l101[l1])->l53 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR0f);(l101[l1])->l54 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR1f);(l101[l1])->l55 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE0f);(l101[l1])->l56 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE1f);(l101[l1])->l57 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MULTICAST_ROUTE0f);(l101[l1])->l58 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MULTICAST_ROUTE1f);(l101[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPA_ID0f);(l101[l1])->l60 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPA_ID1f);(l101[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,EXPECTED_L3_IIF0f);(l101[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,EXPECTED_L3_IIF1f);(l101[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,HIT0f);(l101[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,HIT1f);(l101[l1])->l65 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR0f);(l101[l1])->l66 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR1f);(l101[l1])->l67 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK0f);(l101[l1])->l68 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK1f);(l101[l1])->l69 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE0f);(l101[l1])->l70 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE1f);(l101[l1])->l71 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK0f);(l101[l1])->l72 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK1f);(l101[l1])->l73 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX0f);(l101[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX1f);(l101[l1])->l75 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI0f);(l101[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI1f);(l101[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE0f);(l101[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE1f);(l101[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID0f);(l101[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID1f);(l101[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_0f);(l101[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_1f);(l101[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK0f);(l101[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK1f);(l101[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH0f);(l101[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH1f);(l101[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX0f);(l101[l1])->l88 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX1f);(l101[l1])->l89 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR0f);(l101[l1])->l90 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR1f);(l101[l1])->l91 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS0f);(l101[l1])->l92 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS1f);(l101[l1])->l93 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l101[l1])->
l94 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l101[
l1])->l95 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l101[
l1])->l96 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(l101[
l1])->l97 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,ENTRY_TYPE_MASK0f);(l101[l1])
->l98 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,ENTRY_TYPE_MASK1f);(l43[(l1)]) = 
sal_alloc(l346,"LPM prefix info");if(NULL == (l43[(l1)])){sal_free(l101[l1]);
l101[l1] = NULL;SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}sal_memset((l43
[(l1)]),0,l346);for(l176 = 0;l176<l314;l176++){(l43[(l1)][(l176)].l36) = -1;(
l43[(l1)][(l176)].l37) = -1;(l43[(l1)][(l176)].l38) = -1;(l43[(l1)][(l176)].
next) = -1;(l43[(l1)][(l176)].l39) = 0;(l43[(l1)][(l176)].l40) = 0;}l292 = 
soc_mem_index_count(l1,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l1)){l292>>= 1;}if(
l345 == SOC_ALPM_MODE_PARALLEL){(l43[(l1)][(((3*(64+32+2+1))-1))].l37) = (
l292>>1)-1;(l43[(l1)][(((((3*(64+32+2+1))/3)-1)))].l40) = l292>>1;(l43[(l1)][
((((3*(64+32+2+1))-1)))].l40) = (l292-(l43[(l1)][(((((3*(64+32+2+1))/3)-1)))]
.l40));}else{(l43[(l1)][((((3*(64+32+2+1))-1)))].l40) = l292;}if((l112[(l1)])
!= NULL){if(l120((l112[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}(l112[(l1)]) = NULL;}if(l118(l1,l292*2,l292,&(l112[(l1)]))<0)
{SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;}(void)soc_alpm_ipmc_war(l1,TRUE
);SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}static int l3(int l1){if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}
SOC_ALPM_LPM_LOCK(l1);(void)soc_alpm_ipmc_war(l1,FALSE);if((l112[(l1)])!= 
NULL){l120((l112[(l1)]));(l112[(l1)]) = NULL;}if((l43[(l1)]!= NULL)){sal_free
(l101[l1]);l101[l1] = NULL;sal_free((l43[(l1)]));(l43[(l1)]) = NULL;}
SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}static int l4(int l1,void*l5,int*
l349){int l125;int index;int l9;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int l133 = 
SOC_E_NONE;int l350 = 0;sal_memcpy(l14,soc_mem_entry_null(l1,L3_DEFIPm),
soc_mem_entry_words(l1,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l1);l133 = l13(l1,l5,
l14,&index,&l125,&l9);if(l133 == SOC_E_NOT_FOUND){l133 = l331(l1,l125,l9,l14,
&index);if(l133<0){SOC_ALPM_LPM_UNLOCK(l1);return(l133);}}else{l350 = 1;}*
l349 = index;if(l133 == SOC_E_NONE){if(!l9){if(index&1){l133 = 
soc_alpm_lpm_ip4entry0_to_1(l1,l5,l14,PRESERVE_HIT);}else{l133 = 
soc_alpm_lpm_ip4entry0_to_0(l1,l5,l14,PRESERVE_HIT);}if(l133<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l133);}l5 = (void*)l14;index>>= 1;}
soc_alpm_lpm_state_dump(l1);LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\nsoc_alpm_lpm_insert: %d %d\n"),index,l125));if(!l350){l299(l1,l5,index,
0x4000,0);}l133 = l171(l1,MEM_BLOCK_ANY,index,index,l5);if(l133>= 0){l133 = 
l315(l1,index,l5);}}SOC_ALPM_LPM_UNLOCK(l1);return(l133);}static int l6(int l1
,void*l7){int l125;int index;int l9;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int
l133 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l133 = l13(l1,l7,l14,&index,&l125,&l9
);if(l133 == SOC_E_NONE){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\nsoc_alpm_lpm_delete: %d %d\n"),index,l125));l302(l1,l7,index);l133 = l337(
l1,l125,l9,l14,index);}soc_alpm_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);
return(l133);}static int l17(int l1,void*l7,void*l14,int*l15){int l125;int
l133;int l9;SOC_ALPM_LPM_LOCK(l1);l133 = l13(l1,l7,l14,l15,&l125,&l9);
SOC_ALPM_LPM_UNLOCK(l1);return(l133);}static int l8(int l20,void*l7,int l9,
int l10,int l11,defip_aux_scratch_entry_t*l12){uint32 l134;uint32 l351[4] = {
0,0,0,0};int l125 = 0;int l133 = SOC_E_NONE;l134 = soc_mem_field32_get(l20,
L3_DEFIPm,l7,VALID0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,
VALIDf,l134);l134 = soc_mem_field32_get(l20,L3_DEFIPm,l7,MODE0f);
soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,MODEf,l134);l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,ENTRY_TYPE0f);soc_mem_field32_set(l20,
L3_DEFIP_AUX_SCRATCHm,l12,ENTRY_TYPEf,0);l134 = soc_mem_field32_get(l20,
L3_DEFIPm,l7,GLOBAL_ROUTE0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,
l12,GLOBAL_ROUTEf,l134);if(SOC_MEM_FIELD_VALID(l20,L3_DEFIPm,RPA_ID0f)){l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,RPA_ID0f);soc_mem_field32_set(l20,
L3_DEFIP_AUX_SCRATCHm,l12,RPA_IDf,l134);}if(SOC_MEM_FIELD_VALID(l20,L3_DEFIPm
,EXPECTED_L3_IIF0f)){l134 = soc_mem_field32_get(l20,L3_DEFIPm,l7,
EXPECTED_L3_IIF0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,
EXPECTED_L3_IIFf,l134);}l134 = soc_mem_field32_get(l20,L3_DEFIPm,l7,ECMP0f);
soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,ECMPf,l134);l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,ECMP_PTR0f);soc_mem_field32_set(l20,
L3_DEFIP_AUX_SCRATCHm,l12,ECMP_PTRf,l134);l134 = soc_mem_field32_get(l20,
L3_DEFIPm,l7,NEXT_HOP_INDEX0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,
l12,NEXT_HOP_INDEXf,l134);l134 = soc_mem_field32_get(l20,L3_DEFIPm,l7,PRI0f);
soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,PRIf,l134);l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,RPE0f);soc_mem_field32_set(l20,
L3_DEFIP_AUX_SCRATCHm,l12,RPEf,l134);l134 =soc_mem_field32_get(l20,L3_DEFIPm,
l7,VRF_ID_0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,VRFf,l134);
soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,DB_TYPEf,l10);l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,DST_DISCARD0f);soc_mem_field32_set(l20,
L3_DEFIP_AUX_SCRATCHm,l12,DST_DISCARDf,l134);l134 = soc_mem_field32_get(l20,
L3_DEFIPm,l7,CLASS_ID0f);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,
CLASS_IDf,l134);if(l9){l351[2] = soc_mem_field32_get(l20,L3_DEFIPm,l7,
IP_ADDR0f);l351[3] = soc_mem_field32_get(l20,L3_DEFIPm,l7,IP_ADDR1f);}else{
l351[0] = soc_mem_field32_get(l20,L3_DEFIPm,l7,IP_ADDR0f);}soc_mem_field_set(
l20,L3_DEFIP_AUX_SCRATCHm,(uint32*)l12,IP_ADDRf,(uint32*)l351);if(l9){l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(
l134,&l125))<0){return(l133);}l134 = soc_mem_field32_get(l20,L3_DEFIPm,l7,
IP_ADDR_MASK1f);if(l125){if(l134!= 0xffffffff){return(SOC_E_PARAM);}l125+= 32
;}else{if((l133 = _ipmask2pfx(l134,&l125))<0){return(l133);}}}else{l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,l7,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(
l134,&l125))<0){return(l133);}}soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,
l12,IP_LENGTHf,l125);soc_mem_field32_set(l20,L3_DEFIP_AUX_SCRATCHm,l12,
REPLACE_LENf,l11);return(SOC_E_NONE);}int _soc_alpm_aux_op(int l1,
_soc_aux_op_t l352,defip_aux_scratch_entry_t*l12,int l353,int*l147,int*l148,
int*bucket_index){uint32 l347 = 0;uint32 l354 = 0;int l355;soc_timeout_t l356
;int l133 = SOC_E_NONE;int l357 = 0;if(l353){SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_SCRATCHm(l1,MEM_BLOCK_ANY,0,l12));}l358:l347 = 0;switch(
l352){case INSERT_PROPAGATE:l355 = 0;break;case DELETE_PROPAGATE:l355 = 1;
break;case PREFIX_LOOKUP:l355 = 2;break;case HITBIT_REPLACE:l355 = 3;break;
default:return SOC_E_PARAM;}soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l347,
OPCODEf,l355);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l347,STARTf,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l347));soc_timeout_init(&l356
,50000,5);l355 = 0;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l1,&l347));
l355 = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l347,DONEf);if(l355 == 1){l133
= SOC_E_NONE;break;}if(soc_timeout_check(&l356)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l1,&l347));l355 = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRLr,l347,DONEf);if(l355 == 1){l133 = SOC_E_NONE;}else{LOG_WARN
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d : DEFIP AUX Operation timeout\n"),l1
));l133 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l133)){if(
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l347,ERRORf)){soc_reg_field_set(l1,
L3_DEFIP_AUX_CTRLr,&l347,STARTf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&
l347,ERRORf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l347,DONEf,0);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l347));LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: DEFIP AUX Operation encountered "
"parity error !!\n"),l1));l357++;if(SOC_CONTROL(l1)->alpm_bulk_retry){
sal_sem_take(SOC_CONTROL(l1)->alpm_bulk_retry,1000000);}if(l357<5){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: Retry DEFIP AUX Operation..\n"),l1))
;goto l358;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l1));return SOC_E_INTERNAL;}}if(l352 == PREFIX_LOOKUP){if(l147&&l148){*l147 = 
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l347,HITf);*l148 = soc_reg_field_get(
l1,L3_DEFIP_AUX_CTRLr,l347,BKT_INDEXf);SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRL_1r(l1,&l354));*bucket_index = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRL_1r,l354,BKT_PTRf);}}}return l133;}static int l19(int l20,
void*lpm_entry,void*l21,void*l22,soc_mem_t l23,uint32 l24,uint32*l359){uint32
l134;uint32 l351[4] = {0,0};int l125 = 0;int l133 = SOC_E_NONE;int l9;uint32
l25 = 0;l9 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,MODE0f);sal_memset(
l21,0,soc_mem_entry_words(l20,l23)*4);l134 = soc_mem_field32_get(l20,
L3_DEFIPm,lpm_entry,VALID0f);soc_mem_field32_set(l20,l23,l21,VALIDf,l134);
l134 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,ECMP0f);
soc_mem_field32_set(l20,l23,l21,ECMPf,l134);l134 = soc_mem_field32_get(l20,
L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set(l20,l23,l21,ECMP_PTRf,
l134);l134 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f);
soc_mem_field32_set(l20,l23,l21,NEXT_HOP_INDEXf,l134);l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,PRI0f);soc_mem_field32_set(l20,
l23,l21,PRIf,l134);l134 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,RPE0f);
soc_mem_field32_set(l20,l23,l21,RPEf,l134);l134 = soc_mem_field32_get(l20,
L3_DEFIPm,lpm_entry,DST_DISCARD0f);soc_mem_field32_set(l20,l23,l21,
DST_DISCARDf,l134);l134 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,
SRC_DISCARD0f);soc_mem_field32_set(l20,l23,l21,SRC_DISCARDf,l134);l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,CLASS_ID0f);soc_mem_field32_set(
l20,l23,l21,CLASS_IDf,l134);l351[0] = soc_mem_field32_get(l20,L3_DEFIPm,
lpm_entry,IP_ADDR0f);if(l9){l351[1] = soc_mem_field32_get(l20,L3_DEFIPm,
lpm_entry,IP_ADDR1f);}soc_mem_field_set(l20,l23,(uint32*)l21,KEYf,(uint32*)
l351);if(l9){l134 = soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(l134,&l125))<0){return(l133);}l134 = 
soc_mem_field32_get(l20,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f);if(l125){if(l134
!= 0xffffffff){return(SOC_E_PARAM);}l125+= 32;}else{if((l133 = _ipmask2pfx(
l134,&l125))<0){return(l133);}}}else{l134 = soc_mem_field32_get(l20,L3_DEFIPm
,lpm_entry,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(l134,&l125))<0){return(l133
);}}if((l125 == 0)&&(l351[0] == 0)&&(l351[1] == 0)){l25 = 1;}if(l359!= NULL){
*l359 = l25;}soc_mem_field32_set(l20,l23,l21,LENGTHf,l125);if(l22 == NULL){
return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(l20)){sal_memset(l22,0,
soc_mem_entry_words(l20,l23)*4);sal_memcpy(l22,l21,soc_mem_entry_words(l20,
l23)*4);soc_mem_field32_set(l20,l23,l22,DST_DISCARDf,0);soc_mem_field32_set(
l20,l23,l22,RPEf,0);soc_mem_field32_set(l20,l23,l22,SRC_DISCARDf,l24&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(l20,l23,l22,DEFAULTROUTEf,l25);
}return(SOC_E_NONE);}static int l26(int l20,void*l21,soc_mem_t l23,int l9,int
l27,int l28,int index,void*lpm_entry){uint32 l134;uint32 l351[4] = {0,0};
uint32 l125 = 0;sal_memset(lpm_entry,0,soc_mem_entry_words(l20,L3_DEFIPm)*4);
l134 = soc_mem_field32_get(l20,l23,l21,VALIDf);soc_mem_field32_set(l20,
L3_DEFIPm,lpm_entry,VALID0f,l134);if(l9){soc_mem_field32_set(l20,L3_DEFIPm,
lpm_entry,VALID1f,l134);}l134 = soc_mem_field32_get(l20,l23,l21,ECMPf);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,ECMP0f,l134);l134 = 
soc_mem_field32_get(l20,l23,l21,ECMP_PTRf);soc_mem_field32_set(l20,L3_DEFIPm,
lpm_entry,ECMP_PTR0f,l134);l134 = soc_mem_field32_get(l20,l23,l21,
NEXT_HOP_INDEXf);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f
,l134);l134 = soc_mem_field32_get(l20,l23,l21,PRIf);soc_mem_field32_set(l20,
L3_DEFIPm,lpm_entry,PRI0f,l134);l134 = soc_mem_field32_get(l20,l23,l21,RPEf);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,RPE0f,l134);l134 = 
soc_mem_field32_get(l20,l23,l21,DST_DISCARDf);soc_mem_field32_set(l20,
L3_DEFIPm,lpm_entry,DST_DISCARD0f,l134);l134 = soc_mem_field32_get(l20,l23,
l21,SRC_DISCARDf);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,SRC_DISCARD0f,
l134);l134 = soc_mem_field32_get(l20,l23,l21,CLASS_IDf);soc_mem_field32_set(
l20,L3_DEFIPm,lpm_entry,CLASS_ID0f,l134);soc_mem_field32_set(l20,L3_DEFIPm,
lpm_entry,ALG_BKT_PTR0f,l28);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
ALG_HIT_IDX0f,index);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,MODE_MASK0f,
3);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l9){
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(l20,
l23,l21,KEYf,l351);if(l9){soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
IP_ADDR1f,l351[1]);}soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,IP_ADDR0f,
l351[0]);l134 = soc_mem_field32_get(l20,l23,l21,LENGTHf);if(l9){if(l134>= 32)
{l125 = 0xffffffff;soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f
,l125);l125 = ~(((l134-32) == 32)?0:(0xffffffff)>>(l134-32));
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l125);}else{l125 = 
~(0xffffffff>>l134);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l125);}}else{assert(l134<= 32);l125 = ~(((l134) == 32)?0:(
0xffffffff)>>(l134));soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,l125);}if(l27 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l20,
L3_DEFIPm,lpm_entry,GLOBAL_HIGH0f,1);soc_mem_field32_set(l20,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else if(l27 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry
,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_0f,0);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_0f,l27);
soc_mem_field32_set(l20,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(l20));}
return(SOC_E_NONE);}int soc_alpm_warmboot_pivot_add(int l20,int l9,void*
lpm_entry,int l360,int l361){int l133 = SOC_E_NONE;uint32 key[2] = {0,0};
alpm_pivot_t*l204 = NULL;alpm_bucket_handle_t*l213 = NULL;int l154 = 0,l27 = 
0;uint32 l362;trie_t*l258 = NULL;uint32 prefix[5] = {0};int l25 = 0;l133 = 
l139(l20,lpm_entry,prefix,&l362,&l25);SOC_IF_ERROR_RETURN(l133);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l20,lpm_entry,&l154,&l27));l360 = 
soc_alpm_physical_idx(l20,L3_DEFIPm,l360,l9);l213 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l213 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l20,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l213,0,sizeof(*l213));
l204 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l204 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l20,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l213);return SOC_E_MEMORY;}sal_memset(l204,0
,sizeof(*l204));PIVOT_BUCKET_HANDLE(l204) = l213;if(l9){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l204));key[0] = 
soc_L3_DEFIPm_field32_get(l20,lpm_entry,IP_ADDR0f);key[1] = 
soc_L3_DEFIPm_field32_get(l20,lpm_entry,IP_ADDR1f);}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l204));key[0] = soc_L3_DEFIPm_field32_get
(l20,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l204) = l361;PIVOT_TCAM_INDEX(
l204) = l360;if(l154!= SOC_L3_VRF_OVERRIDE){if(l9 == 0){l258 = 
VRF_PIVOT_TRIE_IPV4(l20,l27);if(l258 == NULL){trie_init(_MAX_KEY_LEN_48_,&
VRF_PIVOT_TRIE_IPV4(l20,l27));l258 = VRF_PIVOT_TRIE_IPV4(l20,l27);}}else{l258
= VRF_PIVOT_TRIE_IPV6(l20,l27);if(l258 == NULL){trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l20,l27));l258 = VRF_PIVOT_TRIE_IPV6(l20,l27);}}
sal_memcpy(l204->key,prefix,sizeof(prefix));l204->len = l362;l133 = 
trie_insert(l258,l204->key,NULL,l204->len,(trie_node_t*)l204);if(SOC_FAILURE(
l133)){sal_free(l213);sal_free(l204);return l133;}}ALPM_TCAM_PIVOT(l20,l360) = 
l204;PIVOT_BUCKET_VRF(l204) = l27;PIVOT_BUCKET_IPV6(l204) = l9;
PIVOT_BUCKET_ENT_CNT_UPDATE(l204);if(key[0] == 0&&key[1] == 0){
PIVOT_BUCKET_DEF(l204) = TRUE;}VRF_PIVOT_REF_INC(l20,l27,l9);return l133;}
static int l363(int l20,int l9,void*lpm_entry,void*l21,soc_mem_t l23,int l360
,int l361,int l364){int l365;int l27;int l133 = SOC_E_NONE;int l25 = 0;uint32
prefix[5] = {0,0,0,0,0};uint32 l146;void*l366 = NULL;trie_t*l367 = NULL;
trie_t*l207 = NULL;trie_node_t*l209 = NULL;payload_t*l368 = NULL;payload_t*
l211 = NULL;alpm_pivot_t*l151 = NULL;if((NULL == lpm_entry)||(NULL == l21)){
return SOC_E_PARAM;}if(l9){if(!(l9 = soc_mem_field32_get(l20,L3_DEFIPm,
lpm_entry,MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l20,lpm_entry,&l365,&l27));l23 = (l9)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l366 = sal_alloc(sizeof(
defip_entry_t),"Temp lpm_entr");if(NULL == l366){return SOC_E_MEMORY;}
SOC_IF_ERROR_RETURN(l26(l20,l21,l23,l9,l365,l361,l360,l366));l133 = l139(l20,
l366,prefix,&l146,&l25);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l20,"prefix create failed\n")));return l133;}sal_free(l366);l151 = 
ALPM_TCAM_PIVOT(l20,l360);l367 = PIVOT_BUCKET_TRIE(l151);l368 = sal_alloc(
sizeof(payload_t),"Payload for Key");if(NULL == l368){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l20,"Unable to allocate memory for trie node.\n")
));return SOC_E_MEMORY;}l211 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(NULL == l211){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l20,"Unable to allocate memory for pfx trie node\n")));sal_free(
l368);return SOC_E_MEMORY;}sal_memset(l368,0,sizeof(*l368));sal_memset(l211,0
,sizeof(*l211));l368->key[0] = prefix[0];l368->key[1] = prefix[1];l368->key[2
] = prefix[2];l368->key[3] = prefix[3];l368->key[4] = prefix[4];l368->len = 
l146;l368->index = l364;sal_memcpy(l211,l368,sizeof(*l368));l133 = 
trie_insert(l367,prefix,NULL,l146,(trie_node_t*)l368);if(SOC_FAILURE(l133)){
goto l369;}if(l9){l207 = VRF_PREFIX_TRIE_IPV6(l20,l27);}else{l207 = 
VRF_PREFIX_TRIE_IPV4(l20,l27);}if(!l25){l133 = trie_insert(l207,prefix,NULL,
l146,(trie_node_t*)l211);if(SOC_FAILURE(l133)){goto l230;}}return l133;l230:(
void)trie_delete(l367,prefix,l146,&l209);l368 = (payload_t*)l209;l369:
sal_free(l368);sal_free(l211);return l133;}static int l370(int l20,int l33,
int l27,int l135,int bkt_ptr){int l133 = SOC_E_NONE;uint32 l146;uint32 key[2]
= {0,0};trie_t*l371 = NULL;payload_t*l246 = NULL;defip_entry_t*lpm_entry = 
NULL;lpm_entry = sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(
lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l20,
"unable to allocate memory for LPM entry\n")));return SOC_E_MEMORY;}l29(l20,
key,0,l27,l33,lpm_entry,0,1);if(l27 == SOC_VRF_MAX(l20)+1){
soc_L3_DEFIPm_field32_set(l20,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l20,lpm_entry,DEFAULT_MISS0f,1);}
soc_L3_DEFIPm_field32_set(l20,lpm_entry,ALG_BKT_PTR0f,bkt_ptr);if(l33 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l20,l27) = lpm_entry;trie_init(_MAX_KEY_LEN_48_,&
VRF_PREFIX_TRIE_IPV4(l20,l27));l371 = VRF_PREFIX_TRIE_IPV4(l20,l27);}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l20,l27) = lpm_entry;trie_init(_MAX_KEY_LEN_144_,
&VRF_PREFIX_TRIE_IPV6(l20,l27));l371 = VRF_PREFIX_TRIE_IPV6(l20,l27);}l246 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(l246 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l20,
"Unable to allocate memory for pfx trie node \n")));return SOC_E_MEMORY;}
sal_memset(l246,0,sizeof(*l246));l146 = 0;l246->key[0] = key[0];l246->key[1] = 
key[1];l246->len = l146;l133 = trie_insert(l371,key,NULL,l146,&(l246->node));
if(SOC_FAILURE(l133)){sal_free(l246);return l133;}VRF_TRIE_INIT_DONE(l20,l27,
l33,1);return l133;}int soc_alpm_warmboot_prefix_insert(int l20,int l9,void*
lpm_entry,void*l21,int l360,int l361,int l364){int l365;int l27;int l133 = 
SOC_E_NONE;soc_mem_t l23;l360 = soc_alpm_physical_idx(l20,L3_DEFIPm,l360,l9);
l23 = (l9)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l20,lpm_entry,&l365,&l27));if(l365 == 
SOC_L3_VRF_OVERRIDE){return(l133);}if(!VRF_TRIE_INIT_COMPLETED(l20,l27,l9)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l20,"VRF %d is not initialized\n"),
l27));l133 = l370(l20,l9,l27,l360,l361);if(SOC_FAILURE(l133)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l20,"VRF %d/%d trie init \n""failed\n"),l27,l9));
return l133;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l20,
"VRF %d/%d trie init completed\n"),l27,l9));}l133 = l363(l20,l9,lpm_entry,l21
,l23,l360,l361,l364);if(l133!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l20,"unit %d : Route Insertion Failed :%s\n"),l20,soc_errmsg(l133)
));return(l133);}VRF_TRIE_ROUTES_INC(l20,l27,l9);return(l133);}int
soc_alpm_warmboot_bucket_bitmap_set(int l1,int l33,int l261){int l293 = 1;if(
l33){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l293 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l261,l293);return SOC_E_NONE;}int
soc_alpm_warmboot_lpm_reinit_done(int l20){int l135;int l372 = ((3*(64+32+2+1
))-1);int l292 = soc_mem_index_count(l20,L3_DEFIPm);if(SOC_URPF_STATUS_GET(
l20)){l292>>= 1;}if(!soc_alpm_mode_get(l20)){(l43[(l20)][(((3*(64+32+2+1))-1)
)].l38) = -1;for(l135 = ((3*(64+32+2+1))-1);l135>-1;l135--){if(-1 == (l43[(
l20)][(l135)].l36)){continue;}(l43[(l20)][(l135)].l38) = l372;(l43[(l20)][(
l372)].next) = l135;(l43[(l20)][(l372)].l40) = (l43[(l20)][(l135)].l36)-(l43[
(l20)][(l372)].l37)-1;l372 = l135;}(l43[(l20)][(l372)].next) = -1;(l43[(l20)]
[(l372)].l40) = l292-(l43[(l20)][(l372)].l37)-1;}else{(l43[(l20)][(((3*(64+32
+2+1))-1))].l38) = -1;for(l135 = ((3*(64+32+2+1))-1);l135>(((3*(64+32+2+1))-1
)/3);l135--){if(-1 == (l43[(l20)][(l135)].l36)){continue;}(l43[(l20)][(l135)]
.l38) = l372;(l43[(l20)][(l372)].next) = l135;(l43[(l20)][(l372)].l40) = (l43
[(l20)][(l135)].l36)-(l43[(l20)][(l372)].l37)-1;l372 = l135;}(l43[(l20)][(
l372)].next) = -1;(l43[(l20)][(l372)].l40) = l292-(l43[(l20)][(l372)].l37)-1;
l372 = (((3*(64+32+2+1))-1)/3);(l43[(l20)][((((3*(64+32+2+1))-1)/3))].l38) = 
-1;for(l135 = ((((3*(64+32+2+1))-1)/3)-1);l135>-1;l135--){if(-1 == (l43[(l20)
][(l135)].l36)){continue;}(l43[(l20)][(l135)].l38) = l372;(l43[(l20)][(l372)]
.next) = l135;(l43[(l20)][(l372)].l40) = (l43[(l20)][(l135)].l36)-(l43[(l20)]
[(l372)].l37)-1;l372 = l135;}(l43[(l20)][(l372)].next) = -1;(l43[(l20)][(l372
)].l40) = (l292>>1)-(l43[(l20)][(l372)].l37)-1;}return(SOC_E_NONE);}int
soc_alpm_warmboot_lpm_reinit(int l20,int l9,int l135,void*lpm_entry){int l16;
defip_entry_t*l373;if(soc_L3_DEFIPm_field32_get(l20,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(l20,lpm_entry,VALID1f)){l299(l20,lpm_entry,l135,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(l20,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l34(l20,lpm_entry,&l16));if((l43[(l20)][(l16)].l39) == 0)
{(l43[(l20)][(l16)].l36) = l135;(l43[(l20)][(l16)].l37) = l135;}else{(l43[(
l20)][(l16)].l37) = l135;}(l43[(l20)][(l16)].l39)++;if(l9){return(SOC_E_NONE)
;}}else{if(soc_L3_DEFIPm_field32_get(l20,lpm_entry,VALID1f)){l373 = sal_alloc
(sizeof(defip_entry_t),"lpm_entry_hi");soc_alpm_lpm_ip4entry1_to_0(l20,
lpm_entry,l373,TRUE);SOC_IF_ERROR_RETURN(l34(l20,l373,&l16));if((l43[(l20)][(
l16)].l39) == 0){(l43[(l20)][(l16)].l36) = l135;(l43[(l20)][(l16)].l37) = 
l135;}else{(l43[(l20)][(l16)].l37) = l135;}sal_free(l373);(l43[(l20)][(l16)].
l39)++;}}return(SOC_E_NONE);}typedef struct l374{int v4;int v6_64;int v6_128;
int l375;int l376;int l377;int l294;}l378;typedef enum l379{l380 = 0,l381,
l382,l383,l384,l385}l386;static void l387(int l1,alpm_vrf_counter_t*l388){
l388->v4 = soc_mem_index_count(l1,L3_DEFIPm)*2;l388->v6_128 = 
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){l388->v6_64 = l388->v6_128;}else{l388->v6_64 = 
l388->v4>>1;}if(SOC_URPF_STATUS_GET(l1)){l388->v4>>= 1;l388->v6_128>>= 1;l388
->v6_64>>= 1;}}static void l389(int l1,int l154,alpm_vrf_handle_t*l390,l386
l391){alpm_vrf_counter_t*l392;int l176,l393,l394,l395;int l350 = 0;
alpm_vrf_counter_t l388;switch(l391){case l380:LOG_CLI((BSL_META_U(l1,
"\nAdd Counter:\n")));break;case l381:LOG_CLI((BSL_META_U(l1,
"\nDelete Counter:\n")));break;case l382:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - 1:\n")));break;case l383:l387(l1,&l388);LOG_CLI((
BSL_META_U(l1,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l388.v4,
l388.v6_64,l388.v6_128));break;case l384:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l385:LOG_CLI((
BSL_META_U(l1,"\nInternal Debug Counter - LPM Full:\n")));break;default:break
;}LOG_CLI((BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n"))
);LOG_CLI((BSL_META_U(l1,"-----------------------------------------------\n")
));l393 = l394 = l395 = 0;for(l176 = 0;l176<MAX_VRF_ID+1;l176++){int l396,
l397,l398;if(l390[l176].init_done == 0&&l176!= MAX_VRF_ID){continue;}if(l154
!= -1&&l154!= l176){continue;}l350 = 1;switch(l391){case l380:l392 = &l390[
l176].add;break;case l381:l392 = &l390[l176].del;break;case l382:l392 = &l390
[l176].bkt_split;break;case l384:l392 = &l390[l176].lpm_shift;break;case l385
:l392 = &l390[l176].lpm_full;break;case l383:l392 = &l390[l176].pivot_used;
break;default:l392 = &l390[l176].pivot_used;break;}l396 = l392->v4;l397 = 
l392->v6_64;l398 = l392->v6_128;l393+= l396;l394+= l397;l395+= l398;do{
LOG_CLI((BSL_META_U(l1,"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l176 == 
MAX_VRF_ID?-1:l176),(l396),(l397),(l398),((l396+l397+l398)),(l176) == 
MAX_VRF_ID?"GHi":(l176) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l350 == 
0){LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));do{
LOG_CLI((BSL_META_U(l1,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l393),(
l394),(l395),((l393+l394+l395))));}while(0);}return;}int soc_alpm_debug_show(
int l1,int l154,uint32 flags){int l176,l399,l350 = 0;l378*l400;l378 l401;l378
l402;if(l154>(SOC_VRF_MAX(l1)+1)){return SOC_E_PARAM;}l399 = MAX_VRF_ID*
sizeof(l378);l400 = sal_alloc(l399,"_alpm_dbg_cnt");if(l400 == NULL){return
SOC_E_MEMORY;}sal_memset(l400,0,l399);l401.v4 = ALPM_IPV4_BKT_COUNT;l401.
v6_64 = ALPM_IPV6_64_BKT_COUNT;l401.v6_128 = ALPM_IPV6_128_BKT_COUNT;if(!
soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l401.v6_64<<= 1;l401.v6_128
<<= 1;}LOG_CLI((BSL_META_U(l1,"\nBucket Occupancy:\n")));if(flags&(
SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l1,
"\n  BKT/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));}for(
l176 = 0;l176<MAX_PIVOT_COUNT;l176++){alpm_pivot_t*l403 = ALPM_TCAM_PIVOT(l1,
l176);if(l403!= NULL){l378*l404;int l27 = PIVOT_BUCKET_VRF(l403);if(l27<0||
l27>(SOC_VRF_MAX(l1)+1)){continue;}if(l154!= -1&&l154!= l27){continue;}if(
flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l350 = 1;do{LOG_CLI((BSL_META_U(l1,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l176,l27,PIVOT_BUCKET_MIN(l403),
PIVOT_BUCKET_MAX(l403),PIVOT_BUCKET_COUNT(l403),PIVOT_BUCKET_DEF(l403)?"Def":
(l27) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}l404 = &l400[l27];if(
PIVOT_BUCKET_IPV6(l403) == L3_DEFIP_MODE_128){l404->v6_128+= 
PIVOT_BUCKET_COUNT(l403);l404->l377+= l401.v6_128;}else if(PIVOT_BUCKET_IPV6(
l403) == L3_DEFIP_MODE_64){l404->v6_64+= PIVOT_BUCKET_COUNT(l403);l404->l376
+= l401.v6_64;}else{l404->v4+= PIVOT_BUCKET_COUNT(l403);l404->l375+= l401.v4;
}l404->l294 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l350 == 0){
LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}}sal_memset(&l402
,0,sizeof(l402));l350 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){LOG_CLI((
BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));for(l176
= 0;l176<MAX_VRF_ID;l176++){l378*l404;if(l400[l176].l294!= TRUE){continue;}if
(l154!= -1&&l154!= l176){continue;}l350 = 1;l404 = &l400[l176];do{(&l402)->v4
+= (l404)->v4;(&l402)->l375+= (l404)->l375;(&l402)->v6_64+= (l404)->v6_64;(&
l402)->l376+= (l404)->l376;(&l402)->v6_128+= (l404)->v6_128;(&l402)->l377+= (
l404)->l377;}while(0);do{LOG_CLI((BSL_META_U(l1,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l176),(l404
->l375)?(l404->v4)*100/(l404->l375):0,(l404->l375)?(l404->v4)*1000/(l404->
l375)%10:0,(l404->l376)?(l404->v6_64)*100/(l404->l376):0,(l404->l376)?(l404->
v6_64)*1000/(l404->l376)%10:0,(l404->l377)?(l404->v6_128)*100/(l404->l377):0,
(l404->l377)?(l404->v6_128)*1000/(l404->l377)%10:0,((l404->l375+l404->l376+
l404->l377))?((l404->v4+l404->v6_64+l404->v6_128))*100/((l404->l375+l404->
l376+l404->l377)):0,((l404->l375+l404->l376+l404->l377))?((l404->v4+l404->
v6_64+l404->v6_128))*1000/((l404->l375+l404->l376+l404->l377))%10:0,(l176) == 
SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l350 == 0){LOG_CLI((BSL_META_U(l1,
"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l1,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l1,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l402)->l375)?((&l402)->v4)*100/((&l402)->l375):0,((&l402)->l375)?((&l402)->v4
)*1000/((&l402)->l375)%10:0,((&l402)->l376)?((&l402)->v6_64)*100/((&l402)->
l376):0,((&l402)->l376)?((&l402)->v6_64)*1000/((&l402)->l376)%10:0,((&l402)->
l377)?((&l402)->v6_128)*100/((&l402)->l377):0,((&l402)->l377)?((&l402)->
v6_128)*1000/((&l402)->l377)%10:0,(((&l402)->l375+(&l402)->l376+(&l402)->l377
))?(((&l402)->v4+(&l402)->v6_64+(&l402)->v6_128))*100/(((&l402)->l375+(&l402)
->l376+(&l402)->l377)):0,(((&l402)->l375+(&l402)->l376+(&l402)->l377))?(((&
l402)->v4+(&l402)->v6_64+(&l402)->v6_128))*1000/(((&l402)->l375+(&l402)->l376
+(&l402)->l377))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l389(l1,l154,alpm_vrf_handle[l1],l383);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_CNT
){l389(l1,l154,alpm_vrf_handle[l1],l380);l389(l1,l154,alpm_vrf_handle[l1],
l381);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l389(l1,l154,alpm_vrf_handle
[l1],l382);l389(l1,l154,alpm_vrf_handle[l1],l385);l389(l1,l154,
alpm_vrf_handle[l1],l384);}sal_free(l400);return SOC_E_NONE;}int
soc_alpm_bucket_sanity_check(int l1,soc_mem_t l269,int index){int l133 = 
SOC_E_NONE;int l176,l266,l275,l9,l148 = -1;int l154 = 0,l27;uint32 l143 = 0;
uint32 l14[SOC_MAX_MEM_FIELD_WORDS];uint32 l405[SOC_MAX_MEM_FIELD_WORDS];int
l406,l407,l408;defip_entry_t lpm_entry;int l280,l409;soc_mem_t l23;int l192,
l272,l410,l411;int l412 = 0;soc_field_t l413[2] = {VALID0f,VALID1f};
soc_field_t l414[2] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l415[2] = {
ALG_BKT_PTR0f,ALG_BKT_PTR1f};l280 = soc_mem_index_min(l1,l269);l409 = 
soc_mem_index_max(l1,l269);if((index>= 0)&&(index<l280||index>l409)){return
SOC_E_PARAM;}else if(index>= 0){l280 = index;l409 = index;}SOC_ALPM_LPM_LOCK(
l1);for(l176 = l280;l176<= l409;l176++){SOC_ALPM_LPM_UNLOCK(l1);
SOC_ALPM_LPM_LOCK(l1);l133 = soc_mem_read(l1,l269,MEM_BLOCK_ANY,l176,(void*)
l14);if(SOC_FAILURE(l133)){continue;}l9 = soc_mem_field32_get(l1,l269,(void*)
l14,MODE0f);if(l9){l272 = 1;l23 = L3_DEFIP_ALPM_IPV6_64m;l410 = 16;}else{l272
= 2;l23 = L3_DEFIP_ALPM_IPV4m;l410 = 24;}for(l192 = 0;l192<l272;l192++){if(
soc_mem_field32_get(l1,l269,(void*)l14,l413[l192]) == 0||soc_mem_field32_get(
l1,l269,(void*)l14,l414[l192]) == 1){continue;}l411 = soc_mem_field32_get(l1,
l269,(void*)l14,l415[l192]);if(l192 == 1){soc_alpm_lpm_ip4entry1_to_0(l1,l14,
l405,PRESERVE_HIT);}else{soc_alpm_lpm_ip4entry0_to_0(l1,l14,l405,PRESERVE_HIT
);}l133 = soc_alpm_lpm_vrf_get(l1,l405,&l154,&l27);if(SOC_FAILURE(l133)){
continue;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l9)){l410<<= 1;}l148 = -1;for(l266 = 
0;l266<l410;l266++){l133 = _soc_alpm_mem_index(l1,l23,l411,l266,l143,&l275);
if(SOC_FAILURE(l133)){continue;}l133 = soc_mem_read(l1,l23,MEM_BLOCK_ANY,l275
,(void*)l14);if(SOC_FAILURE(l133)){break;}if(!soc_mem_field32_get(l1,l23,(
void*)l14,VALIDf)){continue;}l133 = l26(l1,(void*)l14,l23,l9,l154,l411,0,&
lpm_entry);if(SOC_FAILURE(l133)){continue;}l133 = l152(l1,(void*)&lpm_entry,
l23,(void*)l14,&l406,&l407,&l408,FALSE);if(SOC_FAILURE(l133)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLaunched AUX operation for "
"index %d bucket %d sanity check failed\n"),l176,l411));l412++;continue;}if(
l407!= l411){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tEntry at index %d does not belong ""to bucket %d(from bucket %d)\n"),l408,
l411,l407));l412++;}if(l148 == -1){l148 = l406;continue;}if(l148!= l406){int
l416,l417;l416 = soc_alpm_logical_idx(l1,l269,l148>>1,1);l417 = 
soc_alpm_logical_idx(l1,l269,l406>>1,1);LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"\tAliased bucket %d(returned "
"bucket %d) found from TCAM1 %d and TCAM2 %d\n"),l411,l407,l416,l417));l412++
;}}}}SOC_ALPM_LPM_UNLOCK(l1);if(l412 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check passed\n"),
SOC_MEM_NAME(l1,l269),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l1,l269),index,l412));return
SOC_E_FAIL;}int soc_alpm_pivot_sanity_check(int l1,soc_mem_t l269,int index){
int l176,l192,l294 = 0;int l275,l280,l409;int l133 = SOC_E_NONE;int*l418 = 
NULL;int l406,l407,l408;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int l136,l9 = 0,
l272 = 2;int l419,l420[2];soc_mem_t l421,l422;int l412 = 0;soc_field_t l413[3
] = {VALID0f,VALID1f,INVALIDf};soc_field_t l414[3] = {GLOBAL_HIGH0f,
GLOBAL_HIGH1f,INVALIDf};soc_field_t l423[3] = {NEXT_HOP_INDEX0f,
NEXT_HOP_INDEX1f,INVALIDf};soc_field_t l415[3] = {ALG_BKT_PTR0f,ALG_BKT_PTR1f
,INVALIDf};l280 = soc_mem_index_min(l1,l269);l409 = soc_mem_index_max(l1,l269
);if((index>= 0)&&(index<l280||index>l409)){return SOC_E_PARAM;}else if(index
>= 0){l280 = index;l409 = index;}l418 = sal_alloc(sizeof(int)*MAX_PIVOT_COUNT
,"Bucket index array");if(l418 == NULL){l133 = SOC_E_MEMORY;return l133;}
sal_memset(l418,0xff,sizeof(int)*MAX_PIVOT_COUNT);SOC_ALPM_LPM_LOCK(l1);for(
l176 = l280;l176<= l409;l176++){SOC_ALPM_LPM_UNLOCK(l1);SOC_ALPM_LPM_LOCK(l1)
;l133 = soc_mem_read(l1,l269,MEM_BLOCK_ANY,l176,(void*)l14);if(SOC_FAILURE(
l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l1,l269),
l176,l133));l412++;continue;}l9 = soc_mem_field32_get(l1,l269,(void*)l14,
MODE0f);if(l9){l272 = 1;l422 = L3_DEFIP_ALPM_IPV6_64m;}else{l272 = 2;l422 = 
L3_DEFIP_ALPM_IPV4m;}for(l192 = 0;l192<l272;l192++){if(soc_mem_field32_get(l1
,l269,(void*)l14,l413[l192]) == 0||soc_mem_field32_get(l1,l269,(void*)l14,
l414[l192]) == 1){continue;}l136 = soc_mem_field32_get(l1,l269,(void*)l14,
l415[l192]);if(l136!= 0){if(l418[l136] == -1){l418[l136] = l176;}else{
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tDuplicated bucket pointer "
"%d detected, in memory %s index1 %d ""and index2 %d\n"),l136,SOC_MEM_NAME(l1
,l269),l418[l136],l176));l412++;continue;}}l133 = alpm_bucket_is_assigned(l1,
l136,l9,&l294);if(l133 == SOC_E_PARAM){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"\tInvalid bucket pointer %d ""detected, in memory %s index %d\n"),l136,
SOC_MEM_NAME(l1,l269),l176));l412++;continue;}if(l133>= 0&&l294 == 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),l136,SOC_MEM_NAME(l1,l269),l176));l412++
;continue;}l421 = _soc_trident2_alpm_bkt_view_get(l1,l136<<2);if(l422!= l421)
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual"" %s\n"),l136,SOC_MEM_NAME(l1,l422),
SOC_MEM_NAME(l1,l421)));l412++;continue;}l419 = soc_mem_field32_get(l1,l269,(
void*)l14,l423[l192]);if(l192 == 1){l133 = soc_alpm_lpm_ip4entry1_to_0(l1,l14
,l14,PRESERVE_HIT);if(SOC_FAILURE(l133)){continue;}}l406 = -1;l133 = l152(l1,
l14,l422,(void*)l14,&l406,&l407,&l408,FALSE);if(l406 == -1){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLaunched AUX operation for PIVOT "
"index %d sanity check failed\n"),l176));l412++;continue;}l275 = 
soc_alpm_logical_idx(l1,l269,l406>>1,1);l133 = soc_mem_read(l1,l269,
MEM_BLOCK_ANY,l275,(void*)l14);if(SOC_FAILURE(l133)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tRead memory %s index %d (nexthop) "
"return %d\n"),SOC_MEM_NAME(l1,l269),l275,l133));l412++;continue;}l420[0] = 
soc_mem_field32_get(l1,l269,(void*)l14,l423[0]);l420[1] = soc_mem_field32_get
(l1,l269,(void*)l14,l423[1]);if(l419!= l420[l406&1]){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d entry1 %d, index2 %d ""entry2 %d\n"),l176,l192,
l275,l406&1));l412++;continue;}}}SOC_ALPM_LPM_UNLOCK(l1);sal_free(l418);if(
l412 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMemory %s index %d Pivot sanity check passed\n"),SOC_MEM_NAME(l1,l269),
index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMemory %s index %d Pivot sanity check failed, ""encountered %d error(s)\n"
),SOC_MEM_NAME(l1,l269),index,l412));return SOC_E_FAIL;}
#endif
#endif /* ALPM_ENABLE */
