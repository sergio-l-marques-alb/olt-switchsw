/*
 * $Id: alpm.c,v 1.49 Broadcom SDK $
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
soc_alpm_bucket_t soc_alpm_bucket[SOC_MAX_NUM_DEVICES];int
l3_alpm_sw_prefix_lookup[SOC_MAX_NUM_DEVICES];void soc_alpm_lpm_state_dump(
int l1);static int l2(int l1);static int l3(int l1);static int l4(int l1,void
*l5,int*index,int l6);static int l7(int l1,void*l8);static int l9(int l1,void
*l8,int l10,int l11,int l12,int l13,defip_aux_scratch_entry_t*l14);static int
l15(int l1,void*l8,void*l16,int*l17,int*l18,int*l10);static int l19(int l1,
void*l8,void*l16,int*l17);static int l20(int l1);static int l21(int l22,void*
lpm_entry,void*l23,void*l24,soc_mem_t l25,uint32 l26,uint32*l27);static int
l28(int l22,void*l23,soc_mem_t l25,int l10,int l29,int l30,int index,void*
lpm_entry);static int l31(int l22,uint32*key,int len,int l29,int l10,
defip_entry_t*lpm_entry,int l32,int l33);static int l34(int l1,int l29,int l35
);static int l36(int l1,void*entry,int*l18);typedef struct l37{int l38;int l39
;int l40;int next;int l41;int l42;}l43,*l44;static l44 l45[
SOC_MAX_NUM_DEVICES];typedef struct l46{soc_field_info_t*l47;soc_field_info_t
*l48;soc_field_info_t*l49;soc_field_info_t*l50;soc_field_info_t*l51;
soc_field_info_t*l52;soc_field_info_t*l53;soc_field_info_t*l54;
soc_field_info_t*l55;soc_field_info_t*l56;soc_field_info_t*l57;
soc_field_info_t*l58;soc_field_info_t*l59;soc_field_info_t*l60;
soc_field_info_t*l61;soc_field_info_t*l62;soc_field_info_t*l63;
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
typedef struct l110{int l22;int l111;int l112;uint16*l113;uint16*l114;}l115;
typedef uint32 l116[5];typedef int(*l117)(l116 l118,l116 l119);static l115*
l120[SOC_MAX_NUM_DEVICES];static void l121(int l1,void*l16,int index,l116 l122
);static uint16 l123(uint8*l124,int l125);static int l126(int l22,int l111,
int l112,l115**l127);static int l128(l115*l129);static int l130(l115*l131,
l117 l132,l116 entry,int l133,uint16*l134);static int l135(l115*l131,l117 l132
,l116 entry,int l133,uint16 l136,uint16 l137);static int l138(l115*l131,l117
l132,l116 entry,int l133,uint16 l139);static int l140(l115*l131,l117 l132,
l116 entry,int l133,uint16 index);alpm_vrf_handle_t*alpm_vrf_handle[
SOC_MAX_NUM_DEVICES];alpm_pivot_t**tcam_pivot[SOC_MAX_NUM_DEVICES];int
soc_alpm_mode_get(int l1){if(soc_trident2_alpm_mode_get(l1) == 1){return 1;}
else{return 0;}}static int l141(int l1,const void*entry,int*l133){int l142;
uint32 l143;int l10;l10 = soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f);if(
l10){l143 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l142 = 
_ipmask2pfx(l143,l133))<0){return(l142);}l143 = soc_mem_field32_get(l1,
L3_DEFIPm,entry,IP_ADDR_MASK1f);if(*l133){if(l143!= 0xffffffff){return(
SOC_E_PARAM);}*l133+= 32;}else{if((l142 = _ipmask2pfx(l143,l133))<0){return(
l142);}}}else{l143 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);
if((l142 = _ipmask2pfx(l143,l133))<0){return(l142);}}return SOC_E_NONE;}void
soc_alpm_db_ent_type_encoding(int l1,int l29,uint32*l11,uint32*l12){int l144,
l145,l146;uint32 l147[4] = {2,4,6,4};uint32 l148[4] = {0,0,0,1};if(l29 == (
SOC_VRF_MAX(l1)+1)){l144 = 0;l145 = 0;}else if(SOC_IS_TRIDENT2PLUS(l1)||
SOC_IS_TOMAHAWK(l1)){l146 = (l29>>10)&0x3;l144 = l147[l146];l145 = l148[l146]
;}else{l144 = 2;l145 = 0;}if(l11!= NULL){*l11 = l144;}if(l12!= NULL){*l12 = 
l145;}return;}int _soc_alpm_rpf_entry(int l1,int l149){int l150;l150 = (l149
>>2)&0x3fff;l150+= SOC_ALPM_BUCKET_COUNT(l1);return(l149&~(0x3fff<<2))|(l150
<<2);}int soc_alpm_physical_idx(int l1,soc_mem_t l25,int index,int l151){int
l152 = index&1;if(l151){return soc_trident2_l3_defip_index_map(l1,l25,index);
}index>>= 1;index = soc_trident2_l3_defip_index_map(l1,l25,index);index<<= 1;
index|= l152;return index;}int soc_alpm_logical_idx(int l1,soc_mem_t l25,int
index,int l151){int l152 = index&1;if(l151){return
soc_trident2_l3_defip_index_remap(l1,l25,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_remap(l1,l25,index);index<<= 1;index|= l152;
return index;}void l153(int l1,void*lpm_entry,int len,int l10){uint32 l154 = 
0;if(l10){if(len>= 32){l154 = 0xffffffff;soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l76),(l154));l154 = ~(((
len-32) == 32)?0:(0xffffffff)>>(len-32));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l75),(l154));}else{l154 = 
~(0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l76),(l154));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(
l109[(l1)]->l75),(0));}}else{assert(len<= 32);l154 = (len == 32)?0xffffffff:~
(0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l75),(l154));}}static int l155(int l1,
void*entry,uint32*prefix,uint32*l18,int*l27){int l156,l157,l10;int l133 = 0;
int l142 = SOC_E_NONE;uint32 l158,l152;prefix[0] = prefix[1] = prefix[2] = 
prefix[3] = prefix[4] = 0;l10 = soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f
);l156 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR0f);l157 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);prefix[1] = l156;l156 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR1f);l157 = soc_mem_field32_get(
l1,L3_DEFIPm,entry,IP_ADDR_MASK1f);prefix[0] = l156;if(l10){prefix[4] = 
prefix[1];prefix[3] = prefix[0];prefix[1] = prefix[0] = 0;l157 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l142 = _ipmask2pfx
(l157,&l133))<0){return(l142);}l157 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK1f);if(l133){if(l157!= 0xffffffff){return(SOC_E_PARAM);}l133+= 32
;}else{if((l142 = _ipmask2pfx(l157,&l133))<0){return(l142);}}l158 = 64-l133;
if(l158<32){prefix[4]>>= l158;l152 = (((32-l158) == 32)?0:(prefix[3])<<(32-
l158));prefix[3]>>= l158;prefix[4]|= l152;}else{prefix[4] = (((l158-32) == 32
)?0:(prefix[3])>>(l158-32));prefix[3] = 0;}}else{l157 = soc_mem_field32_get(
l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l142 = _ipmask2pfx(l157,&l133))<0){
return(l142);}prefix[1] = (((32-l133) == 32)?0:(prefix[1])>>(32-l133));prefix
[0] = 0;}*l18 = l133;*l27 = (prefix[0] == 0)&&(prefix[1] == 0)&&(l133 == 0);
return SOC_E_NONE;}int _soc_alpm_find_in_bkt(int l1,soc_mem_t l25,int
bucket_index,int l159,uint32*l16,void*l160,int*l134,int l35){int l142;l142 = 
soc_mem_alpm_lookup(l1,l25,bucket_index,MEM_BLOCK_ANY,l159,l16,l160,l134);if(
SOC_SUCCESS(l142)){return l142;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l35)){return
soc_mem_alpm_lookup(l1,l25,bucket_index+1,MEM_BLOCK_ANY,l159,l16,l160,l134);}
return l142;}static int l161(int l1,uint32*prefix,uint32 l162,int l35,int l29
,int*l163,int*tcam_index,int*bucket_index){int l142 = SOC_E_NONE;trie_t*l164;
trie_node_t*l165 = NULL;alpm_pivot_t*l166;if(l35){l164 = VRF_PIVOT_TRIE_IPV6(
l1,l29);}else{l164 = VRF_PIVOT_TRIE_IPV4(l1,l29);}l142 = trie_find_lpm(l164,
prefix,l162,&l165);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Pivot find failed\n")));return l142;}l166 = (alpm_pivot_t*)
l165;*l163 = 1;*tcam_index = PIVOT_TCAM_INDEX(l166);*bucket_index = 
PIVOT_BUCKET_INDEX(l166);return SOC_E_NONE;}static int l167(int l1,void*l8,
soc_mem_t l25,void*l160,int*tcam_index,int*bucket_index,int*l17,int l168){
uint32 l16[SOC_MAX_MEM_FIELD_WORDS];int l169,l29,l35;int l134;uint32 l11,l12,
l159;int l142 = SOC_E_NONE;int l163 = 0;l35 = soc_mem_field32_get(l1,
L3_DEFIPm,l8,MODE0f);if(l35){if(!(l35 = soc_mem_field32_get(l1,L3_DEFIPm,l8,
MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,
l8,&l169,&l29));if(l169 == 0){if(soc_alpm_mode_get(l1)){return SOC_E_PARAM;}}
soc_alpm_db_ent_type_encoding(l1,l29,&l11,&l12);if(l29 == SOC_VRF_MAX(l1)+1){
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l159);}else{SOC_ALPM_GET_VRF_BANK_DISABLE
(l1,l159);}if(l169!= SOC_L3_VRF_OVERRIDE){if(l168){uint32 prefix[5],l162;int
l27 = 0;l142 = l155(l1,l8,prefix,&l162,&l27);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: prefix create failed\n")));
return l142;}l142 = l161(l1,prefix,l162,l35,l29,&l163,tcam_index,bucket_index
);SOC_IF_ERROR_RETURN(l142);}else{defip_aux_scratch_entry_t l14;sal_memset(&
l14,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l1,l8,l35,l11
,l12,0,&l14));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l14,TRUE
,&l163,tcam_index,bucket_index));}if(l163){l21(l1,l8,l16,0,l25,0,0);l142 = 
_soc_alpm_find_in_bkt(l1,l25,*bucket_index,l159,l16,l160,&l134,l35);if(
SOC_SUCCESS(l142)){*l17 = l134;}}else{l142 = SOC_E_NOT_FOUND;}}return l142;}
static int l170(int l1,void*l8,void*l160,void*l171,soc_mem_t l25,int l134){
defip_aux_scratch_entry_t l14;int l169,l35,l29;int bucket_index;uint32 l11,
l12,l159;int l142 = SOC_E_NONE;int l163 = 0,l152 = 0;int tcam_index;l35 = 
soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE0f);if(l35){if(!(l35 = 
soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l8,&l169,&l29));
soc_alpm_db_ent_type_encoding(l1,l29,&l11,&l12);if(l29 == SOC_VRF_MAX(l1)+1){
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l159);}else{SOC_ALPM_GET_VRF_BANK_DISABLE
(l1,l159);}if(l169!= SOC_L3_VRF_OVERRIDE){sal_memset(&l14,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l1,l8,l35,l11,l12,0,&l14))
;SOC_IF_ERROR_RETURN(soc_mem_write(l1,l25,MEM_BLOCK_ANY,l134,l160));if(
SOC_URPF_STATUS_GET(l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,l25,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l134),l171));}l152 = soc_mem_field32_get
(l1,L3_DEFIP_AUX_SCRATCHm,&l14,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l14,REPLACE_LENf,l152);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l14,TRUE,&l163,&tcam_index,&
bucket_index));if(SOC_URPF_STATUS_GET(l1)){if(l152 == 0){soc_mem_field32_set(
l1,L3_DEFIP_AUX_SCRATCHm,&l14,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l14,RPEf,0);}soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l14,DB_TYPEf,l11+1);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l14,TRUE,&l163,&tcam_index,&
bucket_index));}}return l142;}int alpm_mem_prefix_array_cb(trie_node_t*node,
void*l172){alpm_mem_prefix_array_t*l173 = (alpm_mem_prefix_array_t*)l172;if(
node->type == PAYLOAD){l173->prefix[l173->count] = (payload_t*)node;l173->
count++;}return SOC_E_NONE;}int alpm_delete_node_cb(trie_node_t*node,void*
l172){if(node!= NULL){sal_free(node);}return SOC_E_NONE;}static int l174(int
l1,int l175,int l35,int l176){int l142,l152,index;defip_aux_table_entry_t
entry;index = l175>>(l35?0:1);l142 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l142);if(l35){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l176);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l176);l152 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l175&1)
{soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l176);l152 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l176);l152 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l142 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l142);if(SOC_URPF_STATUS_GET(l1)){l152++;if(l35){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l176);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l176);}else{if
(l175&1){soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l176)
;}else{soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l176);}
}soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l152);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l152);index+= (
soc_mem_index_count(l1,L3_DEFIP_AUX_TABLEm))/2;l142 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}return l142;}static int l177
(int l1,int l178,void*entry,void*l179,int l180){uint32 l152,l157,l35,l11,l12,
l181 = 0;soc_mem_t l25 = L3_DEFIPm;soc_mem_t l182 = L3_DEFIP_AUX_TABLEm;
defip_entry_t l183;int l142 = SOC_E_NONE,l133,l184,l29,l185;
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l182,MEM_BLOCK_ANY,l178,l179));l152 = 
soc_mem_field32_get(l1,l25,entry,VRF_ID_0f);soc_mem_field32_set(l1,l182,l179,
VRF0f,l152);l152 = soc_mem_field32_get(l1,l25,entry,VRF_ID_1f);
soc_mem_field32_set(l1,l182,l179,VRF1f,l152);l152 = soc_mem_field32_get(l1,
l25,entry,MODE0f);soc_mem_field32_set(l1,l182,l179,MODE0f,l152);l152 = 
soc_mem_field32_get(l1,l25,entry,MODE1f);soc_mem_field32_set(l1,l182,l179,
MODE1f,l152);l35 = l152;l152 = soc_mem_field32_get(l1,l25,entry,VALID0f);
soc_mem_field32_set(l1,l182,l179,VALID0f,l152);l152 = soc_mem_field32_get(l1,
l25,entry,VALID1f);soc_mem_field32_set(l1,l182,l179,VALID1f,l152);l152 = 
soc_mem_field32_get(l1,l25,entry,IP_ADDR_MASK0f);if((l142 = _ipmask2pfx(l152,
&l133))<0){return l142;}l157 = soc_mem_field32_get(l1,l25,entry,
IP_ADDR_MASK1f);if((l142 = _ipmask2pfx(l157,&l184))<0){return l142;}if(l35){
soc_mem_field32_set(l1,l182,l179,IP_LENGTH0f,l133+l184);soc_mem_field32_set(
l1,l182,l179,IP_LENGTH1f,l133+l184);}else{soc_mem_field32_set(l1,l182,l179,
IP_LENGTH0f,l133);soc_mem_field32_set(l1,l182,l179,IP_LENGTH1f,l184);}l152 = 
soc_mem_field32_get(l1,l25,entry,IP_ADDR0f);soc_mem_field32_set(l1,l182,l179,
IP_ADDR0f,l152);l152 = soc_mem_field32_get(l1,l25,entry,IP_ADDR1f);
soc_mem_field32_set(l1,l182,l179,IP_ADDR1f,l152);if(!l35){sal_memcpy(&l183,
entry,sizeof(l183));l142 = soc_alpm_lpm_vrf_get(l1,(void*)&l183,&l29,&l133);
SOC_IF_ERROR_RETURN(l142);SOC_IF_ERROR_RETURN(soc_alpm_lpm_ip4entry1_to_0(l1,
&l183,&l183,PRESERVE_HIT));l142 = soc_alpm_lpm_vrf_get(l1,(void*)&l183,&l185,
&l184);SOC_IF_ERROR_RETURN(l142);}else{l142 = soc_alpm_lpm_vrf_get(l1,entry,&
l29,&l133);}if(SOC_URPF_STATUS_GET(l1)){if(l180>= (soc_mem_index_count(l1,
L3_DEFIPm)>>1)){l181 = 1;}}soc_alpm_db_ent_type_encoding(l1,l133,&l11,&l12);
if(l29 == SOC_L3_VRF_OVERRIDE||(SOC_MEM_FIELD_VALID(l1,l25,MULTICAST_ROUTE0f)
&&soc_mem_field32_get(l1,l25,entry,MULTICAST_ROUTE0f))){soc_mem_field32_set(
l1,l182,l179,VALID0f,0);l11 = 0;}else{if(l181){l11+= 1;}}soc_mem_field32_set(
l1,l182,l179,DB_TYPE0f,l11);l152 = soc_mem_field32_get(l1,l25,entry,
ENTRY_TYPE0f);soc_mem_field32_set(l1,l182,l179,ENTRY_TYPE0f,l152|l12);if(!l35
){soc_alpm_db_ent_type_encoding(l1,l184,&l11,&l12);if(l185 == 
SOC_L3_VRF_OVERRIDE||(SOC_MEM_FIELD_VALID(l1,l25,MULTICAST_ROUTE1f)&&
soc_mem_field32_get(l1,l25,entry,MULTICAST_ROUTE1f))){soc_mem_field32_set(l1,
l182,l179,VALID1f,0);l11 = 0;}else{if(l181){l11+= 1;}}soc_mem_field32_set(l1,
l182,l179,DB_TYPE1f,l11);l152 = soc_mem_field32_get(l1,l25,entry,ENTRY_TYPE1f
);soc_mem_field32_set(l1,l182,l179,ENTRY_TYPE1f,l152|l12);}else{if(l29 == 
SOC_L3_VRF_OVERRIDE||(SOC_MEM_FIELD_VALID(l1,l25,MULTICAST_ROUTE1f)&&
soc_mem_field32_get(l1,l25,entry,MULTICAST_ROUTE1f))){soc_mem_field32_set(l1,
l182,l179,VALID1f,0);}soc_mem_field32_set(l1,l182,l179,DB_TYPE1f,l11);l152 = 
soc_mem_field32_get(l1,l25,entry,ENTRY_TYPE1f);soc_mem_field32_set(l1,l182,
l179,ENTRY_TYPE1f,l152|l12);}if(l181){l152 = soc_mem_field32_get(l1,l25,entry
,ALG_BKT_PTR0f);if(l152){l152+= SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set
(l1,l25,entry,ALG_BKT_PTR0f,l152);}if(!l35){l152 = soc_mem_field32_get(l1,l25
,entry,ALG_BKT_PTR1f);if(l152){l152+= SOC_ALPM_BUCKET_COUNT(l1);
soc_mem_field32_set(l1,l25,entry,ALG_BKT_PTR1f,l152);}}}return SOC_E_NONE;}
static int l186(int l1,int l187,int index,int l188,void*entry){
defip_aux_table_entry_t l179;l188 = soc_alpm_physical_idx(l1,L3_DEFIPm,l188,1
);SOC_IF_ERROR_RETURN(l177(l1,l188,entry,(void*)&l179,index));
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l1,MEM_BLOCK_ANY,index,entry));index = 
soc_alpm_physical_idx(l1,L3_DEFIPm,index,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,index,&l179));return SOC_E_NONE;}
int _soc_alpm_insert_in_bkt(int l1,soc_mem_t l25,int bucket_index,int l159,
void*l160,uint32*l16,int*l134,int l35){int l142;l142 = soc_mem_alpm_insert(l1
,l25,bucket_index,MEM_BLOCK_ANY,l159,l160,l16,l134);if(l142 == SOC_E_FULL){if
(SOC_ALPM_V6_SCALE_CHECK(l1,l35)){return soc_mem_alpm_insert(l1,l25,
bucket_index+1,MEM_BLOCK_ANY,l159,l160,l16,l134);}}return l142;}int
_soc_alpm_mem_index(int l1,soc_mem_t l25,int bucket_index,int l189,uint32 l159
,int*l190){int l191,l192 = 0;int l193[4] = {0};int l194 = 0;int l195 = 0;int
l196;int l35 = 0;int l197 = 6;switch(l25){case L3_DEFIP_ALPM_IPV6_64m:l197 = 
4;l35 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:l197 = 2;l35 = 1;break;default:
break;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l35)){if(l189>= ALPM_RAW_BKT_COUNT*l197)
{bucket_index++;l189-= ALPM_RAW_BKT_COUNT*l197;}}l196 = (4)-_shr_popcount(
l159&((1<<(4))-1));if(bucket_index>= (1<<16)||l189>= l196*l197){return
SOC_E_FULL;}l195 = l189%l197;for(l191 = 0;l191<(4);l191++){if((1<<l191)&l159)
{continue;}l193[l192++] = l191;}l194 = l193[l189/l197];*l190 = (l195<<16)|(
bucket_index<<2)|(l194);return SOC_E_NONE;}void _soc_alpm_raw_mem_read(int l22
,soc_mem_t l25,void*l198,int l199,void*entry){soc_mem_info_t l200;
soc_field_info_t l201;int l202 = soc_mem_entry_bits(l22,l25)-1;l200.flags = 0
;l200.bytes = sizeof(defip_alpm_raw_entry_t);l201.flags = SOCF_LE;l201.bp = 
l202*l199;l201.len = l202;(void)soc_meminfo_fieldinfo_field_get(l198,&l200,&
l201,entry);}void _soc_alpm_raw_mem_write(int l22,soc_mem_t l25,void*l198,int
l199,void*entry){soc_mem_info_t l200;soc_field_info_t l201;int l202 = 
soc_mem_entry_bits(l22,l25)-1;l200.flags = 0;l200.bytes = sizeof(
defip_alpm_raw_entry_t);l201.bp = l202*l199;l201.len = l202;l201.flags = 
SOCF_LE;(void)soc_meminfo_fieldinfo_field_set(l198,&l200,&l201,entry);}int
_soc_alpm_raw_bucket_get(int l1,soc_mem_t l25,int index){return
SOC_ALPM_BKT_ENTRY_TO_IDX(index%(1<<16));}int _soc_alpm_raw_bucket_read(int l1
,soc_mem_t l25,int bucket_index,void*l198,void*l203){int l191,l35 = 1;int l204
,l205;defip_alpm_raw_entry_t*l145 = l198;defip_alpm_raw_entry_t*l206 = l203;
if(l25 == L3_DEFIP_ALPM_IPV4m){l35 = 0;}l204 = SOC_ALPM_BKT_IDX_TO_ENTRY(
bucket_index);for(l191 = 0;l191<SOC_ALPM_RAW_BUCKET_SIZE(l1,l35);l191++){
SOC_IF_ERROR_RETURN(soc_mem_read(l1,L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l204+
l191,&l145[l191]));if(SOC_URPF_STATUS_GET(l1)){l205 = _soc_alpm_rpf_entry(l1,
l204+l191);SOC_IF_ERROR_RETURN(soc_mem_read(l1,L3_DEFIP_ALPM_RAWm,
MEM_BLOCK_ANY,l205,&l206[l191]));}}return SOC_E_NONE;}int
_soc_alpm_raw_bucket_write(int l1,soc_mem_t l25,int bucket_index,uint32 l159,
void*l198,void*l203,int l207){int l191 = 0,l208,l35 = 1;int l204,l205,l209;
defip_alpm_raw_entry_t*l145 = l198;defip_alpm_raw_entry_t*l206 = l203;int l210
= 6;switch(l25){case L3_DEFIP_ALPM_IPV4m:l210 = 6;l35 = 0;break;case
L3_DEFIP_ALPM_IPV6_64m:l210 = 4;l35 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:
l210 = 2;l35 = 1;break;default:break;}l204 = SOC_ALPM_BKT_IDX_TO_ENTRY(
bucket_index);if(l207 == -1){l209 = SOC_ALPM_RAW_BUCKET_SIZE(l1,l35);}else{
l209 = (l207/l210)+1;}for(l208 = 0;l208<SOC_ALPM_RAW_BUCKET_SIZE(l1,l35);l208
++){if((1<<(l208%(4)))&l159){continue;}SOC_IF_ERROR_RETURN(soc_mem_write(l1,
L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l204+l208,&l145[l208]));
_soc_trident2_alpm_bkt_view_set(l1,l204+l208,l25);if(SOC_URPF_STATUS_GET(l1))
{l205 = _soc_alpm_rpf_entry(l1,l204+l208);_soc_trident2_alpm_bkt_view_set(l1,
l205,l25);SOC_IF_ERROR_RETURN(soc_mem_write(l1,L3_DEFIP_ALPM_RAWm,
MEM_BLOCK_ANY,l205,&l206[l208]));}if(++l191 == l209){break;}}return SOC_E_NONE
;}void _soc_alpm_raw_parity_set(int l1,soc_mem_t l25,void*l160){int l191,l211
,l212 = 0;uint32*entry = l160;l211 = soc_mem_entry_words(l1,l25);for(l191 = 0
;l191<l211;l191++){l212+= _shr_popcount(entry[l191]);}if(l212&0x1){if(
SOC_MEM_FIELD_VALID(l1,l25,EVEN_PARITYf)){soc_mem_field32_set(l1,l25,l160,
EVEN_PARITYf,1);}}}static int l213(int l1,void*l8,soc_mem_t l25,void*l160,
void*l171,int*l17,int bucket_index,int tcam_index){alpm_pivot_t*l166,*l214,*
l215;defip_aux_scratch_entry_t l14;uint32 l16[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l216,l162;uint32 l217[5];int l35,l29,l169;int l134;int l142 = 
SOC_E_NONE,l218;uint32 l11,l12,l159,l176 = 0;int l163 =0;int l175;int l219 = 
0;trie_t*trie,*l220;trie_node_t*l221,*l222 = NULL,*l165 = NULL;payload_t*l223
,*l224,*l225;defip_entry_t lpm_entry;alpm_bucket_handle_t*l226;int l191,l227 = 
-1,l27 = 0,l228 = 0;alpm_mem_prefix_array_t l173;defip_alpm_ipv4_entry_t l229
,l230;defip_alpm_ipv6_64_entry_t l231,l232;void*l233,*l234;int*l137 = NULL;
int l235 = 0;trie_t*l164 = NULL;int l236;int l237;void*l238;void*l239 = NULL;
void*l240;void*l241;void*l242;int l243 = 0;int l199 = 0;l35 = 
soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE0f);if(l35){if(!(l35 = 
soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l8,&l169,&l29));
soc_alpm_db_ent_type_encoding(l1,l29,&l11,&l12);if(l29 == SOC_VRF_MAX(l1)+1){
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l159);}else{SOC_ALPM_GET_VRF_BANK_DISABLE
(l1,l159);}l25 = (l35)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l233 = ((
l35)?((uint32*)&(l231)):((uint32*)&(l229)));l234 = ((l35)?((uint32*)&(l232)):
((uint32*)&(l230)));l142 = l155(l1,l8,prefix,&l162,&l27);if(SOC_FAILURE(l142)
){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l142;}sal_memset(&l14,0,
sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l1,l8,l35,l11,l12,0
,&l14));if(bucket_index == 0){if(l3_alpm_sw_prefix_lookup[l1]){l142 = l161(l1
,prefix,l162,l35,l29,&l163,&tcam_index,&bucket_index);SOC_IF_ERROR_RETURN(
l142);}else{SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l14,TRUE,&
l163,&tcam_index,&bucket_index));}if(l163 == 0){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: ""Could not find bucket to insert prefix\n")
));return SOC_E_NOT_FOUND;}}l142 = _soc_alpm_insert_in_bkt(l1,l25,
bucket_index,l159,l160,l16,&l134,l35);if(l142 == SOC_E_NONE){*l17 = l134;if(
SOC_URPF_STATUS_GET(l1)){l218 = soc_mem_write(l1,l25,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l134),l171);if(SOC_FAILURE(l218)){return l218;}}}if(
l142 == SOC_E_FULL){l219 = 1;}l166 = ALPM_TCAM_PIVOT(l1,tcam_index);if(l166 == 
NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"ins: pivot index %d bkt %d is not valid \n"),tcam_index,bucket_index));
return SOC_E_PARAM;}trie = PIVOT_BUCKET_TRIE(l166);l215 = l166;l223 = 
sal_alloc(sizeof(payload_t),"Payload for Key");if(l223 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory for ""trie node \n")));return
SOC_E_MEMORY;}l224 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");
if(l224 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory for ""pfx trie node \n")));
sal_free(l223);return SOC_E_MEMORY;}sal_memset(l223,0,sizeof(*l223));
sal_memset(l224,0,sizeof(*l224));l223->key[0] = prefix[0];l223->key[1] = 
prefix[1];l223->key[2] = prefix[2];l223->key[3] = prefix[3];l223->key[4] = 
prefix[4];l223->len = l162;l223->index = l134;sal_memcpy(l224,l223,sizeof(*
l223));l224->bkt_ptr = l223;l142 = trie_insert(trie,prefix,NULL,l162,(
trie_node_t*)l223);if(SOC_FAILURE(l142)){goto l244;}if(l35){l220 = 
VRF_PREFIX_TRIE_IPV6(l1,l29);}else{l220 = VRF_PREFIX_TRIE_IPV4(l1,l29);}if(!
l27){l142 = trie_insert(l220,prefix,NULL,l162,(trie_node_t*)l224);}else{l165 = 
NULL;l142 = trie_find_lpm(l220,0,0,&l165);l225 = (payload_t*)l165;if(
SOC_SUCCESS(l142)){l225->bkt_ptr = l223;}}l216 = l162;if(SOC_FAILURE(l142)){
goto l245;}if(l219){l142 = alpm_bucket_assign(l1,&bucket_index,l35);if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate""new bucket for split\n")));
bucket_index = -1;goto l246;}l142 = trie_split(trie,l35?_MAX_KEY_LEN_144_:
_MAX_KEY_LEN_48_,FALSE,l217,&l162,&l221,NULL,FALSE,1024);if(SOC_FAILURE(l142)
){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not split bucket""for prefix 0x%08x 0x%08x\n"),
prefix[0],prefix[1]));goto l246;}l165 = NULL;l142 = trie_find_lpm(l220,l217,
l162,&l165);l225 = (payload_t*)l165;if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l217[0],l217[1],l217[
2],l217[3],l217[4],l162));goto l246;}if(l225->bkt_ptr){if(l225->bkt_ptr == 
l223){sal_memcpy(l233,l160,l35?sizeof(defip_alpm_ipv6_64_entry_t):sizeof(
defip_alpm_ipv4_entry_t));}else{l142 = soc_mem_read(l1,l25,MEM_BLOCK_ANY,((
payload_t*)l225->bkt_ptr)->index,l233);if(SOC_FAILURE(l142)){goto l246;}if(
SOC_URPF_STATUS_GET(l1)){l142 = soc_mem_read(l1,l25,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,((payload_t*)l225->bkt_ptr)->index),l234);l228 = 
soc_mem_field32_get(l1,l25,l234,DEFAULTROUTEf);}}if(SOC_FAILURE(l142)){goto
l246;}l142 = l28(l1,l233,l25,l35,l169,bucket_index,0,&lpm_entry);if(
SOC_FAILURE(l142)){goto l246;}l176 = ((payload_t*)(l225->bkt_ptr))->len;}else
{l142 = soc_mem_read(l1,L3_DEFIPm,MEM_BLOCK_ANY,soc_alpm_logical_idx(l1,
L3_DEFIPm,tcam_index>>1,1),&lpm_entry);if((!l35)&&(tcam_index&1)){l142 = 
soc_alpm_lpm_ip4entry1_to_0(l1,&lpm_entry,&lpm_entry,0);}}l226 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l226 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l142 = SOC_E_MEMORY;goto l246;}sal_memset(l226,0,sizeof(*l226));l166 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l166 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l142 = SOC_E_MEMORY;goto l246;}sal_memset(l166,0,sizeof(*l166));
PIVOT_BUCKET_HANDLE(l166) = l226;if(l35){l142 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l166));}else{l142 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l166));}PIVOT_BUCKET_TRIE(l166)->trie = l221;
PIVOT_BUCKET_INDEX(l166) = bucket_index;PIVOT_BUCKET_VRF(l166) = l29;
PIVOT_BUCKET_IPV6(l166) = l35;PIVOT_BUCKET_DEF(l166) = FALSE;l166->key[0] = 
l217[0];l166->key[1] = l217[1];l166->len = l162;l166->key[2] = l217[2];l166->
key[3] = l217[3];l166->key[4] = l217[4];if(l35){l164 = VRF_PIVOT_TRIE_IPV6(l1
,l29);}else{l164 = VRF_PIVOT_TRIE_IPV4(l1,l29);}do{if(!(l35)){l217[0] = (((32
-l162) == 32)?0:(l217[1])<<(32-l162));l217[1] = 0;}else{int l247 = 64-l162;
int l248;if(l247<32){l248 = l217[3]<<l247;l248|= (((32-l247) == 32)?0:(l217[4
])>>(32-l247));l217[0] = l217[4]<<l247;l217[1] = l248;l217[2] = l217[3] = 
l217[4] = 0;}else{l217[1] = (((l247-32) == 32)?0:(l217[4])<<(l247-32));l217[0
] = l217[2] = l217[3] = l217[4] = 0;}}}while(0);(void)l31(l1,l217,l162,l29,
l35,&lpm_entry,0,0);soc_L3_DEFIPm_field32_set(l1,&lpm_entry,ALG_BKT_PTR0f,
bucket_index);sal_memset(&l173,0,sizeof(l173));l142 = trie_traverse(
PIVOT_BUCKET_TRIE(l166),alpm_mem_prefix_array_cb,&l173,_TRIE_INORDER_TRAVERSE
);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l246;}l137 = sal_alloc(sizeof(*l137)*l173.count,
"Temp storage for location of prefixes in new bucket");if(l137 == NULL){l142 = 
SOC_E_MEMORY;goto l246;}sal_memset(l137,-1,sizeof(*l137)*l173.count);l236 = 
sizeof(defip_alpm_raw_entry_t);l237 = l236*ALPM_RAW_BKT_COUNT_DW;l239 = 
sal_alloc(4*l237,"Raw memory buffer");if(l239 == NULL){l142 = SOC_E_MEMORY;
goto l246;}sal_memset(l239,0,4*l237);l240 = (uint8*)l239+l237;l241 = (uint8*)
l239+l237*2;l242 = (uint8*)l239+l237*3;l142 = _soc_alpm_raw_bucket_read(l1,
l25,PIVOT_BUCKET_INDEX(l215),l239,l240);if(SOC_FAILURE(l142)){goto l246;}for(
l191 = 0;l191<l173.count;l191++){payload_t*l133 = l173.prefix[l191];if(l133->
index>0){SOC_ALPM_RAW_INDEX_DECODE(l1,l35,l133->index,l243,l199);l238 = (
uint8*)l239+l243*l236;_soc_alpm_raw_mem_read(l1,l25,l238,l199,l233);
_soc_alpm_raw_mem_write(l1,l25,l238,l199,soc_mem_entry_null(l1,l25));if(
SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE(l1,l35,_soc_alpm_rpf_entry
(l1,l133->index),l243,l199);l238 = (uint8*)l240+l243*l236;
_soc_alpm_raw_mem_read(l1,l25,l238,l199,l234);_soc_alpm_raw_mem_write(l1,l25,
l238,l199,soc_mem_entry_null(l1,l25));}l142 = _soc_alpm_mem_index(l1,l25,
bucket_index,l191,l159,&l134);if(SOC_SUCCESS(l142)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l35,l134,l243,l199);l238 = (uint8*)l241+l243*l236;_soc_alpm_raw_mem_write
(l1,l25,l238,l199,l233);if(SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l35,_soc_alpm_rpf_entry(l1,l134),l243,l199);l238 = (uint8*)l242+l243*l236
;_soc_alpm_raw_mem_write(l1,l25,l238,l199,l234);}}}else{l142 = 
_soc_alpm_mem_index(l1,l25,bucket_index,l191,l159,&l134);if(SOC_SUCCESS(l142)
){l227 = l191;*l17 = l134;_soc_alpm_raw_parity_set(l1,l25,l160);
SOC_ALPM_RAW_INDEX_DECODE(l1,l35,l134,l243,l199);l238 = (uint8*)l241+l243*
l236;_soc_alpm_raw_mem_write(l1,l25,l238,l199,l160);if(SOC_URPF_STATUS_GET(l1
)){_soc_alpm_raw_parity_set(l1,l25,l171);SOC_ALPM_RAW_INDEX_DECODE(l1,l35,
_soc_alpm_rpf_entry(l1,l134),l243,l199);l238 = (uint8*)l242+l243*l236;
_soc_alpm_raw_mem_write(l1,l25,l238,l199,l171);}}}l137[l191] = l134;}l142 = 
_soc_alpm_raw_bucket_write(l1,l25,bucket_index,l159,(void*)l241,(void*)l242,
l173.count);if(SOC_FAILURE(l142)){goto l249;}l142 = l4(l1,&lpm_entry,&l175,
l228);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to add new""pivot to tcam\n")));if(l142 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l29,l35);}goto l249;}l175 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l175,l35);l142 = l174(l1,l175,l35,l176);if
(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),l175));goto l250
;}l142 = trie_insert(l164,l166->key,NULL,l166->len,(trie_node_t*)l166);if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));goto l250;}l235 = 1;ALPM_TCAM_PIVOT(
l1,l175<<(l35?1:0)) = l166;PIVOT_TCAM_INDEX(l166) = l175<<(l35?1:0);
VRF_PIVOT_REF_INC(l1,l29,l35);for(l191 = 0;l191<l173.count;l191++){payload_t*
l133 = l173.prefix[l191];if(l133->index>0){l142 = soc_mem_cache_invalidate(l1
,l25,MEM_BLOCK_ANY,l133->index);if(SOC_SUCCESS(l142)){if(SOC_URPF_STATUS_GET(
l1)){SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(l1,l25,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l133->index)));}}if(SOC_FAILURE(l142)){goto l250;}}
l133->index = l137[l191];}sal_free(l137);l137 = NULL;l142 = 
_soc_alpm_raw_bucket_write(l1,l25,PIVOT_BUCKET_INDEX(l215),l159,(void*)l239,(
void*)l240,-1);if(SOC_FAILURE(l142)){goto l250;}if(l227 == -1){l142 = 
_soc_alpm_insert_in_bkt(l1,l25,PIVOT_BUCKET_INDEX(l215),l159,l160,l16,&l134,
l35);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n")));
goto l250;}if(SOC_URPF_STATUS_GET(l1)){l142 = soc_mem_write(l1,l25,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l134),l171);}*l17 = l134;l223->index = 
l134;}sal_free(l239);PIVOT_BUCKET_ENT_CNT_UPDATE(l166);VRF_BUCKET_SPLIT_INC(
l1,l29,l35);}VRF_TRIE_ROUTES_INC(l1,l29,l35);if(l27){sal_free(l224);}if(
SOC_IS_TRIDENT2(l1)){SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE
,&l14,TRUE,&l163,&tcam_index,&bucket_index));SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,INSERT_PROPAGATE,&l14,FALSE,&l163,&tcam_index,&
bucket_index));}else{SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE
,&l14,TRUE,&l163,&tcam_index,&bucket_index));}if(SOC_URPF_STATUS_GET(l1)){
l162 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l14,DB_TYPEf);l162+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l14,DB_TYPEf,l162);if(l27){
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l14,RPEf,1);}else{
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l14,RPEf,0);}if(SOC_IS_TRIDENT2
(l1)){SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l14,TRUE,&
l163,&tcam_index,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,
INSERT_PROPAGATE,&l14,FALSE,&l163,&tcam_index,&bucket_index));}else{
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE,&l14,TRUE,&l163,&
tcam_index,&bucket_index));}}PIVOT_BUCKET_ENT_CNT_UPDATE(l215);return l142;
l250:l218 = l7(l1,&lpm_entry);if(SOC_FAILURE(l218)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_insert: Failure to free new prefix""at %d\n"),
soc_alpm_logical_idx(l1,L3_DEFIPm,l175,l35)));}if(l235){l218 = trie_delete(
l164,l166->key,l166->len,&l222);if(SOC_FAILURE(l218)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: trie delete failure "
"in bkt move rollback\n")));}}VRF_PIVOT_REF_DEC(l1,l29,l35);l249:l214 = l215;
for(l191 = 0;l191<l173.count;l191++){payload_t*prefix = l173.prefix[l191];if(
l137[l191]!= -1){if(l35){sal_memset(l233,0,sizeof(defip_alpm_ipv6_64_entry_t)
);}else{sal_memset(l233,0,sizeof(defip_alpm_ipv4_entry_t));}l218 = 
soc_mem_write(l1,l25,MEM_BLOCK_ANY,l137[l191],l233);
_soc_trident2_alpm_bkt_view_set(l1,l137[l191],INVALIDm);if(SOC_FAILURE(l218))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l1)){l218 = soc_mem_write(l1,l25,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l137[l191]),l233);_soc_trident2_alpm_bkt_view_set(l1,
_soc_alpm_rpf_entry(l1,l137[l191]),INVALIDm);if(SOC_FAILURE(l218)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l222 = NULL;l218 = trie_delete(
PIVOT_BUCKET_TRIE(l166),prefix->key,prefix->len,&l222);l223 = (payload_t*)
l222;if(SOC_FAILURE(l218)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(prefix
->index>0){l218 = trie_insert(PIVOT_BUCKET_TRIE(l214),prefix->key,NULL,prefix
->len,(trie_node_t*)l223);if(SOC_FAILURE(l218)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l223!= NULL){sal_free(l223);}}}if(l227
== -1){l222 = NULL;l218 = trie_delete(PIVOT_BUCKET_TRIE(l214),prefix,l216,&
l222);l223 = (payload_t*)l222;if(SOC_FAILURE(l218)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l223!= 
NULL){sal_free(l223);}}l218 = alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l166)
,l35);if(SOC_FAILURE(l218)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: new bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(l166
)));}trie_destroy(PIVOT_BUCKET_TRIE(l166));sal_free(l226);sal_free(l166);if(
l137!= NULL){sal_free(l137);}sal_free(l239);l222 = NULL;l218 = trie_delete(
l220,prefix,l216,&l222);l224 = (payload_t*)l222;if(SOC_FAILURE(l218)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l224){sal_free(
l224);}return l142;l246:if(l137!= NULL){sal_free(l137);}if(l239!= NULL){
sal_free(l239);}l222 = NULL;(void)trie_delete(l220,prefix,l216,&l222);l224 = 
(payload_t*)l222;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l35);}l245:l222 = NULL;(void)trie_delete(trie,prefix,l216,&l222)
;l223 = (payload_t*)l222;l244:if(l223!= NULL){sal_free(l223);}if(l224!= NULL)
{sal_free(l224);}return l142;}static int l31(int l22,uint32*key,int len,int
l29,int l10,defip_entry_t*lpm_entry,int l32,int l33){uint32 l154;if(l33){
sal_memset(lpm_entry,0,sizeof(defip_entry_t));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),
(l109[(l22)]->l89),(l29&SOC_VRF_MAX(l22)));if(l29 == (SOC_VRF_MAX(l22)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),
(l109[(l22)]->l91),(0));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l91),(SOC_VRF_MAX(l22)
));}if(l10){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),
(lpm_entry),(l109[(l22)]->l73),(key[0]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l74),(key[1]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),
(l109[(l22)]->l77),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l78),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),
(l109[(l22)]->l90),(l29&SOC_VRF_MAX(l22)));if(l29 == (SOC_VRF_MAX(l22)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),
(l109[(l22)]->l92),(0));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l92),(SOC_VRF_MAX(l22)
));}soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
lpm_entry),(l109[(l22)]->l88),(1));if(len>= 32){l154 = 0xffffffff;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),
(l109[(l22)]->l76),(l154));l154 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry
),(l109[(l22)]->l75),(l154));}else{l154 = ~(0xffffffff>>len);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),
(l109[(l22)]->l76),(l154));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l75),(0));}}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),
(l109[(l22)]->l73),(key[0]));assert(len<= 32);l154 = (len == 32)?0xffffffff:~
(0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l75),(l154));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),
(l109[(l22)]->l87),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l79),((1<<soc_mem_field_length(l22,
L3_DEFIPm,MODE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l80),((1<<soc_mem_field_length(l22,
L3_DEFIPm,MODE_MASK1f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l105),((1<<soc_mem_field_length(l22
,L3_DEFIPm,ENTRY_TYPE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry),(l109[(l22)]->l106),((1<<
soc_mem_field_length(l22,L3_DEFIPm,ENTRY_TYPE_MASK1f))-1));return(SOC_E_NONE)
;}int _soc_alpm_delete_in_bkt(int l1,soc_mem_t l25,int l251,int l159,void*
l252,uint32*l16,int*l134,int l35){int l142;l142 = soc_mem_alpm_delete(l1,l25,
l251,MEM_BLOCK_ALL,l159,l252,l16,l134);if(SOC_SUCCESS(l142)){return l142;}if(
SOC_ALPM_V6_SCALE_CHECK(l1,l35)){return soc_mem_alpm_delete(l1,l25,l251+1,
MEM_BLOCK_ALL,l159,l252,l16,l134);}return l142;}static int l253(int l1,void*
l8,int bucket_index,int tcam_index,int l134,int l254){alpm_pivot_t*l166;
defip_alpm_ipv4_entry_t l229,l230,l255;defip_alpm_ipv6_64_entry_t l231,l232,
l256;defip_aux_scratch_entry_t l14;uint32 l16[SOC_MAX_MEM_FIELD_WORDS];
soc_mem_t l25;void*l233,*l252,*l234 = NULL;int l169;int l10;int l142 = 
SOC_E_NONE,l218;uint32 l257[5],prefix[5];int l35,l29;uint32 l162;int l251;
uint32 l11,l12,l159;int l163,l27 = 0;trie_t*trie,*l220;uint32 l258;
defip_entry_t lpm_entry,*l259;payload_t*l223 = NULL,*l260 = NULL,*l225 = NULL
;trie_node_t*l222 = NULL,*l165 = NULL;uint32 l261 = 0,l262 = 0;trie_t*l164 = 
NULL;l10 = l35 = soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE0f);if(l35){if(!(l35
= soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l8,&l169,&l29));
soc_alpm_db_ent_type_encoding(l1,l29,&l11,&l12);if(l169!= SOC_L3_VRF_OVERRIDE
){if(l29 == SOC_VRF_MAX(l1)+1){SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l159);}
else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l159);}l142 = l155(l1,l8,prefix,&l162,&
l27);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: prefix create failed\n")));return l142;}if(!
soc_alpm_mode_get(l1)){if(l169!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1
,l29,l35)>1){if(l27){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot delete default ""route if other routes are present in "
"this mode"),l29));return SOC_E_PARAM;}}}}l25 = (l35)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l233 = ((l35)?((uint32*)&(l231)):((uint32*)&(l229)));l252
= ((l35)?((uint32*)&(l256)):((uint32*)&(l255)));SOC_ALPM_LPM_LOCK(l1);if(
bucket_index == 0){l142 = l167(l1,l8,l25,l233,&tcam_index,&bucket_index,&l134
,l3_alpm_sw_prefix_lookup[l1]);}else{l142 = l21(l1,l8,l233,0,l25,0,0);}
sal_memcpy(l252,l233,l35?sizeof(l231):sizeof(l229));if(SOC_FAILURE(l142)){
SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to find prefix for delete\n")));return l142;}l251 = 
bucket_index;l166 = ALPM_TCAM_PIVOT(l1,tcam_index);if(l166 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"del: pivot index %d bkt %d is not valid \n")
,tcam_index,bucket_index));SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_PARAM;}trie = 
PIVOT_BUCKET_TRIE(l166);l142 = trie_delete(trie,prefix,l162,&l222);l223 = (
payload_t*)l222;if(l142!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l142;}if(l35){l220 = VRF_PREFIX_TRIE_IPV6(l1,
l29);}else{l220 = VRF_PREFIX_TRIE_IPV4(l1,l29);}if(l35){l164 = 
VRF_PIVOT_TRIE_IPV6(l1,l29);}else{l164 = VRF_PIVOT_TRIE_IPV4(l1,l29);}if(!l27
){l142 = trie_delete(l220,prefix,l162,&l222);l260 = (payload_t*)l222;if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l263;}if(l254){l165 = NULL;l142 = trie_find_lpm(l220,
prefix,l162,&l165);l225 = (payload_t*)l165;if(SOC_SUCCESS(l142)){payload_t*
l264 = (payload_t*)(l225->bkt_ptr);if(l264!= NULL){l258 = l264->len;}else{
l258 = 0;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l265;}
sal_memcpy(l257,prefix,sizeof(prefix));do{if(!(l35)){l257[0] = (((32-l162) == 
32)?0:(l257[1])<<(32-l162));l257[1] = 0;}else{int l247 = 64-l162;int l248;if(
l247<32){l248 = l257[3]<<l247;l248|= (((32-l247) == 32)?0:(l257[4])>>(32-l247
));l257[0] = l257[4]<<l247;l257[1] = l248;l257[2] = l257[3] = l257[4] = 0;}
else{l257[1] = (((l247-32) == 32)?0:(l257[4])<<(l247-32));l257[0] = l257[2] = 
l257[3] = l257[4] = 0;}}}while(0);(void)l31(l1,l257,l258,l29,l10,&lpm_entry,0
,1);l142 = l167(l1,&lpm_entry,l25,l233,&tcam_index,&bucket_index,&l134,
l3_alpm_sw_prefix_lookup[l1]);if(SOC_SUCCESS(l142)){(void)l28(l1,l233,l25,l10
,l169,bucket_index,0,&lpm_entry);(void)l31(l1,l257,l162,l29,l10,&lpm_entry,0,
0);if(SOC_URPF_STATUS_GET(l1)){l234 = ((l35)?((uint32*)&(l232)):((uint32*)&(
l230)));l142 = soc_mem_read(l1,l25,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l134)
,l234);l261 = soc_mem_field32_get(l1,l25,l234,SRC_DISCARDf);l262 = 
soc_mem_field32_get(l1,l25,l234,DEFAULTROUTEf);}}else if(l258 == 0){l259 = 
l35?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l29):VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l29);
sal_memcpy(&lpm_entry,l259,sizeof(lpm_entry));(void)l31(l1,l257,l162,l29,l10,
&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){l261 = soc_mem_field32_get(l1,
L3_DEFIPm,&lpm_entry,SRC_DISCARD0f);l262 = soc_mem_field32_get(l1,L3_DEFIPm,&
lpm_entry,RPE0f);}l142 = SOC_E_NONE;}if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x, prop_pfx:"" 0x%08x 0x%08x, bpm_len %d \n"
),prefix[0],prefix[1],l257[0],l257[1],l258));goto l265;}l259 = &lpm_entry;}}
else{l165 = NULL;l142 = trie_find_lpm(l220,prefix,l162,&l165);l225 = (
payload_t*)l165;if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),l29));goto l263;}l225->bkt_ptr = NULL;l258 = 
0;l259 = l35?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l29):VRF_TRIE_DEFAULT_ROUTE_IPV4(
l1,l29);}if(l254){l142 = l9(l1,l259,l35,l11,l12,l258,&l14);if(SOC_FAILURE(
l142)){goto l265;}l142 = _soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l14,TRUE,&l163
,&tcam_index,&bucket_index);if(SOC_FAILURE(l142)){goto l265;}if(
SOC_URPF_STATUS_GET(l1)){soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l14,
DB_TYPEf,l11+1);soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l14,
SRC_DISCARDf,l261);soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l14,
DEFAULTROUTEf,l262);l142 = _soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l14,TRUE,&
l163,&tcam_index,&bucket_index);if(SOC_FAILURE(l142)){goto l265;}}}sal_free(
l223);if(!l27){sal_free(l260);}PIVOT_BUCKET_ENT_CNT_UPDATE(l166);if((l166->
len!= 0)&&(trie->trie == NULL)){uint32 l266[5];sal_memcpy(l266,l166->key,
sizeof(l266));do{if(!(l10)){l266[0] = (((32-l166->len) == 32)?0:(l266[1])<<(
32-l166->len));l266[1] = 0;}else{int l247 = 64-l166->len;int l248;if(l247<32)
{l248 = l266[3]<<l247;l248|= (((32-l247) == 32)?0:(l266[4])>>(32-l247));l266[
0] = l266[4]<<l247;l266[1] = l248;l266[2] = l266[3] = l266[4] = 0;}else{l266[
1] = (((l247-32) == 32)?0:(l266[4])<<(l247-32));l266[0] = l266[2] = l266[3] = 
l266[4] = 0;}}}while(0);(void)l31(l1,l266,l166->len,l29,l10,&lpm_entry,0,1);
l142 = l7(l1,&lpm_entry);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Unable to ""delete pivot 0x%08x 0x%08x \n"),
l166->key[0],l166->key[1]));}}l142 = _soc_alpm_delete_in_bkt(l1,l25,l251,l159
,l252,l16,&l134,l35);if(!SOC_SUCCESS(l142)){SOC_ALPM_LPM_UNLOCK(l1);l142 = 
SOC_E_FAIL;return l142;}if(SOC_URPF_STATUS_GET(l1)){l142 = 
soc_mem_alpm_delete(l1,l25,SOC_ALPM_RPF_BKT_IDX(l1,l251),MEM_BLOCK_ALL,l159,
l252,l16,&l163);if(!SOC_SUCCESS(l142)){SOC_ALPM_LPM_UNLOCK(l1);l142 = 
SOC_E_FAIL;return l142;}}if((l166->len!= 0)&&(trie->trie == NULL)){l142 = 
alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l166),l35);if(SOC_FAILURE(l142)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to release""empty bucket: %d\n"),PIVOT_BUCKET_INDEX
(l166)));}l142 = trie_delete(l164,l166->key,l166->len,&l222);if(SOC_FAILURE(
l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l166));sal_free(PIVOT_BUCKET_HANDLE(l166));sal_free(l166);
_soc_trident2_alpm_bkt_view_set(l1,l251<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l35)){_soc_trident2_alpm_bkt_view_set(l1,(l251+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l29,l35);if(VRF_TRIE_ROUTES_CNT(l1,
l29,l35) == 0){l142 = l34(l1,l29,l35);}SOC_ALPM_LPM_UNLOCK(l1);return l142;
l265:l218 = trie_insert(l220,prefix,NULL,l162,(trie_node_t*)l260);if(
SOC_FAILURE(l218)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into pfx trie\n")
,prefix[0],prefix[1]));}l263:l218 = trie_insert(trie,prefix,NULL,l162,(
trie_node_t*)l223);if(SOC_FAILURE(l218)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into bkt trie\n"),prefix[0],prefix[1]));}
SOC_ALPM_LPM_UNLOCK(l1);return l142;}int soc_alpm_ipmc_war(int l22,int l267){
int l191,l10 = 0;int index = -1;defip_entry_t lpm_entry;soc_mem_t l25 = 
L3_DEFIPm;int count = 1;int l268 = 63;if(!(soc_feature(l22,soc_feature_alpm)
&&soc_feature(l22,soc_feature_td2p_a0_sw_war)&&soc_property_get(l22,
spn_L3_ALPM_ENABLE,0))){return SOC_E_NONE;}if(!SOC_IS_TRIDENT2PLUS(l22)){
return SOC_E_NONE;}if(SOC_WARM_BOOT(l22)){return SOC_E_NONE;}if(
soc_mem_index_count(l22,L3_DEFIPm)<= 0){return SOC_E_NONE;}if(
soc_mem_index_count(l22,L3_DEFIP_PAIR_128m)<= 0){count = 2;}l268 = 
soc_mem_field_length(l22,ING_ACTIVE_L3_IIF_PROFILEm,RPA_ID_PROFILEf);for(l191
= 0;l191<count;l191++){sal_memset(&lpm_entry,0,soc_mem_entry_words(l22,l25)*4
);l10 = l191;soc_mem_field32_set(l22,l25,&lpm_entry,VALID0f,1);
soc_mem_field32_set(l22,l25,&lpm_entry,MODE_MASK0f,3);soc_mem_field32_set(l22
,l25,&lpm_entry,ENTRY_TYPE_MASK0f,1);soc_mem_field32_set(l22,l25,&lpm_entry,
MULTICAST_ROUTE0f,1);soc_mem_field32_set(l22,l25,&lpm_entry,GLOBAL_ROUTE0f,1)
;soc_mem_field32_set(l22,l25,&lpm_entry,RPA_ID0f,l268-1);soc_mem_field32_set(
l22,l25,&lpm_entry,EXPECTED_L3_IIF0f,16383);if(l10){soc_mem_field32_set(l22,
l25,&lpm_entry,VALID1f,1);soc_mem_field32_set(l22,l25,&lpm_entry,MODE0f,1);
soc_mem_field32_set(l22,l25,&lpm_entry,MODE1f,1);soc_mem_field32_set(l22,l25,
&lpm_entry,MODE_MASK1f,3);soc_mem_field32_set(l22,l25,&lpm_entry,
ENTRY_TYPE_MASK1f,1);soc_mem_field32_set(l22,l25,&lpm_entry,MULTICAST_ROUTE1f
,1);soc_mem_field32_set(l22,l25,&lpm_entry,GLOBAL_ROUTE1f,1);
soc_mem_field32_set(l22,l25,&lpm_entry,RPA_ID1f,l268-1);soc_mem_field32_set(
l22,l25,&lpm_entry,EXPECTED_L3_IIF1f,16383);soc_mem_field32_set(l22,l25,&
lpm_entry,IP_ADDR0f,0);soc_mem_field32_set(l22,l25,&lpm_entry,IP_ADDR_MASK0f,
0);soc_mem_field32_set(l22,l25,&lpm_entry,IP_ADDR1f,0xff000000);
soc_mem_field32_set(l22,l25,&lpm_entry,IP_ADDR_MASK1f,0xff000000);}else{
soc_mem_field32_set(l22,l25,&lpm_entry,IP_ADDR0f,0xe0000000);
soc_mem_field32_set(l22,l25,&lpm_entry,IP_ADDR_MASK0f,0xe0000000);}if(l267){
SOC_IF_ERROR_RETURN(l4(l22,&lpm_entry,&index,0));}else{SOC_IF_ERROR_RETURN(l7
(l22,&lpm_entry));}}return SOC_E_NONE;}int soc_alpm_init(int l1){int l191;int
l142 = SOC_E_NONE;l142 = l2(l1);SOC_IF_ERROR_RETURN(l142);l142 = l20(l1);
alpm_vrf_handle[l1] = sal_alloc((MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t),
"ALPM VRF Handles");if(alpm_vrf_handle[l1] == NULL){return SOC_E_MEMORY;}
tcam_pivot[l1] = sal_alloc(MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*),
"ALPM pivots");if(tcam_pivot[l1] == NULL){return SOC_E_MEMORY;}sal_memset(
alpm_vrf_handle[l1],0,(MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t));sal_memset(
tcam_pivot[l1],0,MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*));for(l191 = 0;l191<
MAX_PIVOT_COUNT;l191++){ALPM_TCAM_PIVOT(l1,l191) = NULL;}if(SOC_CONTROL(l1)->
alpm_bulk_retry == NULL){SOC_CONTROL(l1)->alpm_bulk_retry = sal_sem_create(
"ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_lookup_retry == 
NULL){SOC_CONTROL(l1)->alpm_lookup_retry = sal_sem_create("ALPM lookup retry"
,sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_insert_retry == NULL){
SOC_CONTROL(l1)->alpm_insert_retry = sal_sem_create("ALPM insert retry",
sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_delete_retry == NULL){SOC_CONTROL
(l1)->alpm_delete_retry = sal_sem_create("ALPM delete retry",sal_sem_BINARY,0
);}l142 = soc_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l142);return l142;}
static int l269(int l1){int l191,l142;alpm_pivot_t*l152;for(l191 = 0;l191<
MAX_PIVOT_COUNT;l191++){l152 = ALPM_TCAM_PIVOT(l1,l191);if(l152){if(
PIVOT_BUCKET_HANDLE(l152)){if(PIVOT_BUCKET_TRIE(l152)){l142 = trie_traverse(
PIVOT_BUCKET_TRIE(l152),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l142)){trie_destroy(PIVOT_BUCKET_TRIE(l152));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Unable to clear trie state for unit %d\n"),l1
));return l142;}}sal_free(PIVOT_BUCKET_HANDLE(l152));}sal_free(
ALPM_TCAM_PIVOT(l1,l191));ALPM_TCAM_PIVOT(l1,l191) = NULL;}}for(l191 = 0;l191
<= SOC_VRF_MAX(l1)+1;l191++){l142 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l1,
l191),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l142)){
trie_destroy(VRF_PREFIX_TRIE_IPV4(l1,l191));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l1,l191));
return l142;}l142 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l1,l191),
alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l142)){
trie_destroy(VRF_PREFIX_TRIE_IPV6(l1,l191));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v6 pfx trie for vrf %d\n"),l1,l191));
return l142;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l191)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l191));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l191
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l191));}sal_memset(&
alpm_vrf_handle[l1][l191],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l1][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,1,1);
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,2,1);if(SOC_ALPM_BUCKET_BMAP(l1)!= NULL){
sal_free(SOC_ALPM_BUCKET_BMAP(l1));}sal_memset(&soc_alpm_bucket[l1],0,sizeof(
soc_alpm_bucket_t));if(alpm_vrf_handle[l1])sal_free(alpm_vrf_handle[l1]);if(
tcam_pivot[l1])sal_free(tcam_pivot[l1]);alpm_vrf_handle[l1] = NULL;tcam_pivot
[l1] = NULL;return SOC_E_NONE;}int soc_alpm_deinit(int l1){l3(l1);
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_deinit(l1));SOC_IF_ERROR_RETURN(
soc_alpm_128_state_clear(l1));SOC_IF_ERROR_RETURN(l269(l1));if(SOC_CONTROL(l1
)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l1)->alpm_bulk_retry);
SOC_CONTROL(l1)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_lookup_retry
);SOC_CONTROL(l1)->alpm_lookup_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_insert_retry
);SOC_CONTROL(l1)->alpm_insert_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_delete_retry
);SOC_CONTROL(l1)->alpm_delete_retry = NULL;}return SOC_E_NONE;}static int
l270(int l1,int l29,int l35){defip_entry_t*lpm_entry = NULL,l271;int l272;int
index;int l142 = SOC_E_NONE;uint32 key[2] = {0,0};uint32 l162;
alpm_bucket_handle_t*l226 = NULL;alpm_pivot_t*l166 = NULL;payload_t*l260 = 
NULL;trie_t*l273 = NULL,*l274 = NULL;int l275 = 0;trie_t*l276 = NULL;int l277
= 0;l142 = alpm_bucket_assign(l1,&l272,l35);if(SOC_FAILURE(l142)){return l142
;}l226 = sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l226
== NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
l142 = SOC_E_MEMORY;goto l278;}sal_memset(l226,0,sizeof(*l226));l166 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l166 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
l142 = SOC_E_MEMORY;goto l278;}sal_memset(l166,0,sizeof(*l166));
PIVOT_BUCKET_HANDLE(l166) = l226;l260 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l260 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_vrf_add: Unable to allocate memory for "
"pfx trie node \n")));l142 = SOC_E_MEMORY;goto l278;}sal_memset(l260,0,sizeof
(*l260));if(l35 == 0){l142 = trie_init(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(
l1,l29));l276 = VRF_PIVOT_TRIE_IPV4(l1,l29);}else{l142 = trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(l1,l29));l276 = VRF_PIVOT_TRIE_IPV6(l1
,l29);}if(SOC_FAILURE(l142)){goto l278;}if(l35 == 0){l142 = trie_init(
_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l1,l29));l273 = VRF_PREFIX_TRIE_IPV4(
l1,l29);}else{l142 = trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l1,l29
));l273 = VRF_PREFIX_TRIE_IPV6(l1,l29);}if(SOC_FAILURE(l142)){goto l278;}if(
l35){l142 = trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l166));}else{l142 = 
trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l166));}l274 = 
PIVOT_BUCKET_TRIE(l166);if(SOC_FAILURE(l142)){goto l278;}PIVOT_BUCKET_INDEX(
l166) = l272;PIVOT_BUCKET_VRF(l166) = l29;PIVOT_BUCKET_IPV6(l166) = l35;
PIVOT_BUCKET_DEF(l166) = TRUE;l162 = 0;l166->key[0] = l260->key[0] = key[0];
l166->key[1] = l260->key[1] = key[1];l166->len = l260->len = l162;l142 = 
trie_insert(l273,key,NULL,l162,&(l260->node));if(SOC_FAILURE(l142)){goto l278
;}l275 = 1;l142 = trie_insert(l276,key,NULL,l162,(trie_node_t*)l166);if(
SOC_FAILURE(l142)){goto l278;}l277 = 1;(void)l31(l1,key,0,l29,l35,&l271,0,1);
if(l29 == SOC_VRF_MAX(l1)+1){soc_L3_DEFIPm_field32_set(l1,&l271,
GLOBAL_ROUTE0f,1);}else{soc_L3_DEFIPm_field32_set(l1,&l271,DEFAULT_MISS0f,1);
}soc_L3_DEFIPm_field32_set(l1,&l271,ALG_BKT_PTR0f,l272);lpm_entry = sal_alloc
(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n")));l142 = 
SOC_E_MEMORY;goto l278;}sal_memcpy(lpm_entry,&l271,sizeof(l271));l142 = l4(l1
,&l271,&index,0);if(SOC_FAILURE(l142)){goto l278;}if(l35 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l29) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l29) = lpm_entry;}index = 
soc_alpm_physical_idx(l1,L3_DEFIPm,index,l35);if(l35 == 0){ALPM_TCAM_PIVOT(l1
,index) = l166;PIVOT_TCAM_INDEX(l166) = index;}else{ALPM_TCAM_PIVOT(l1,index
<<1) = l166;PIVOT_TCAM_INDEX(l166) = index<<1;}VRF_PIVOT_REF_INC(l1,l29,l35);
VRF_TRIE_INIT_DONE(l1,l29,l35,1);return SOC_E_NONE;l278:if(lpm_entry){
sal_free(lpm_entry);}if(l277){trie_node_t*l222 = NULL;(void)trie_delete(l276,
key,l162,&l222);}if(l275){trie_node_t*l222 = NULL;(void)trie_delete(l273,key,
l162,&l222);}if(l274){(void)trie_destroy(l274);PIVOT_BUCKET_TRIE(l166) = NULL
;}if(l273){(void)trie_destroy(l273);if(l35 == 0){VRF_PREFIX_TRIE_IPV4(l1,l29)
= NULL;}else{VRF_PREFIX_TRIE_IPV6(l1,l29) = NULL;}}if(l276){(void)
trie_destroy(l276);if(l35 == 0){VRF_PIVOT_TRIE_IPV4(l1,l29) = NULL;}else{
VRF_PIVOT_TRIE_IPV6(l1,l29) = NULL;}}if(l260){sal_free(l260);}if(l166){
sal_free(l166);}if(l226){sal_free(l226);}(void)alpm_bucket_release(l1,l272,
l35);return l142;}static int l34(int l1,int l29,int l35){defip_entry_t*
lpm_entry;int l272;int l149;int l142 = SOC_E_NONE;uint32 key[2] = {0,0},l279[
SOC_MAX_MEM_FIELD_WORDS];payload_t*l223;alpm_pivot_t*l280;trie_node_t*l222;
trie_t*l281;trie_t*l276 = NULL;if(l35 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l29);}else{lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l29);}l272 = soc_L3_DEFIPm_field32_get(l1,
lpm_entry,ALG_BKT_PTR0f);l142 = alpm_bucket_release(l1,l272,l35);
_soc_trident2_alpm_bkt_view_set(l1,l272<<2,INVALIDm);l142 = l19(l1,lpm_entry,
(void*)l279,&l149);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_vrf_delete: unable to get internal"
" pivot idx for vrf %d/%d\n"),l29,l35));l149 = -1;}l149 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l149,l35);if(l35 == 0){l280 = 
ALPM_TCAM_PIVOT(l1,l149);}else{l280 = ALPM_TCAM_PIVOT(l1,l149<<1);}l142 = l7(
l1,lpm_entry);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),l29,l35));}sal_free(lpm_entry);if(
l35 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l29) = NULL;l281 = 
VRF_PREFIX_TRIE_IPV4(l1,l29);VRF_PREFIX_TRIE_IPV4(l1,l29) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l29) = NULL;l281 = VRF_PREFIX_TRIE_IPV6(l1,l29
);VRF_PREFIX_TRIE_IPV6(l1,l29) = NULL;}VRF_TRIE_INIT_DONE(l1,l29,l35,0);l142 = 
trie_delete(l281,key,0,&l222);l223 = (payload_t*)l222;if(SOC_FAILURE(l142)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal default for vrf "" %d/%d\n"),l29,l35));}sal_free(
l223);(void)trie_destroy(l281);if(l35 == 0){l276 = VRF_PIVOT_TRIE_IPV4(l1,l29
);VRF_PIVOT_TRIE_IPV4(l1,l29) = NULL;}else{l276 = VRF_PIVOT_TRIE_IPV6(l1,l29)
;VRF_PIVOT_TRIE_IPV6(l1,l29) = NULL;}l222 = NULL;l142 = trie_delete(l276,key,
0,&l222);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l29,l35));}(void)
trie_destroy(l276);(void)trie_destroy(PIVOT_BUCKET_TRIE(l280));sal_free(
PIVOT_BUCKET_HANDLE(l280));sal_free(l280);return l142;}int soc_alpm_insert(
int l1,void*l5,uint32 l26,int l282,int l283){defip_alpm_ipv4_entry_t l229,
l230;defip_alpm_ipv6_64_entry_t l231,l232;soc_mem_t l25;void*l233,*l252;int
l169,l29;int index;int l10;int l142 = SOC_E_NONE;uint32 l27;int l284 = 0;l10 = 
soc_mem_field32_get(l1,L3_DEFIPm,l5,MODE0f);l25 = (l10)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l233 = ((l10)?((uint32*)&(l231)):(
(uint32*)&(l229)));l252 = ((l10)?((uint32*)&(l232)):((uint32*)&(l230)));
SOC_IF_ERROR_RETURN(l21(l1,l5,l233,l252,l25,l26,&l27));SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l5,&l169,&l29));if(soc_feature(l1,
soc_feature_ipmc_defip)){l284 = soc_mem_field32_get(l1,L3_DEFIPm,l5,
MULTICAST_ROUTE0f);}if((l169!= SOC_L3_VRF_OVERRIDE)&&(l29 == 0)&&
soc_alpm_mode_get(l1)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}if((l169 == 
SOC_L3_VRF_OVERRIDE)||(l284)){l142 = l4(l1,l5,&index,0);if(SOC_SUCCESS(l142))
{VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l10);VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l10);
}else if(l142 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID,l10);}return(
l142);}if(l169!= SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l1)){if(
VRF_TRIE_ROUTES_CNT(l1,l29,l10) == 0){if(!l27){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n"),l169));return SOC_E_PARAM;}}}}if(!
VRF_TRIE_INIT_COMPLETED(l1,l29,l10)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_insert:VRF %d is not ""initialized\n"),l29));l142 = l270(l1,l29,
l10);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n"),l29,l10));return l142;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),l29,l10));}if(l283&
SOC_ALPM_LOOKUP_HIT){l142 = l170(l1,l5,l233,l252,l25,l282);}else{if(l282 == -
1){l282 = 0;}l142 = l213(l1,l5,l25,l233,l252,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l282),l283);}if(l142!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l1,soc_errmsg(l142)
));}return(l142);}int soc_alpm_lookup(int l1,void*l8,void*l16,int*l17,int*
l285){defip_alpm_ipv4_entry_t l229;defip_alpm_ipv6_64_entry_t l231;soc_mem_t
l25;int bucket_index;int tcam_index = -1;void*l233;int l169,l29;int l10,l133;
int l142 = SOC_E_NONE;int l284 = 0;l10 = soc_mem_field32_get(l1,L3_DEFIPm,l8,
MODE0f);SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l8,&l169,&l29));*l17 = 0;
l142 = l15(l1,l8,l16,l17,&l133,&l10);if(SOC_SUCCESS(l142)){if(!l10&&(*l17&0x1
)){l142 = soc_alpm_lpm_ip4entry1_to_0(l1,l16,l16,PRESERVE_HIT);}if(
soc_feature(l1,soc_feature_ipmc_defip)){l284 = soc_mem_field32_get(l1,
L3_DEFIPm,l16,MULTICAST_ROUTE0f);}}if((l169 == SOC_L3_VRF_OVERRIDE)||(l284)){
*l285 = 0;return l142;}if(!VRF_TRIE_INIT_COMPLETED(l1,l29,l10)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_lookup:VRF %d is not initialized\n")
,l29));*l285 = 0;return SOC_E_NOT_FOUND;}l25 = (l10)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l233 = ((l10)?((uint32*)&(l231)):((uint32*)&(l229)));
SOC_ALPM_LPM_LOCK(l1);l142 = l167(l1,l8,l25,l233,&tcam_index,&bucket_index,
l17,l3_alpm_sw_prefix_lookup[l1]);SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l142
)){*l285 = tcam_index;*l17 = bucket_index<<2;return l142;}l142 = l28(l1,l233,
l25,l10,l169,bucket_index,*l17,l16);*l285 = SOC_ALPM_LOOKUP_HIT|tcam_index;
return(l142);}static int l286(int l1,void*l8,void*l16,int l29,int*tcam_index,
int*bucket_index,int*l134,int l287){int l142 = SOC_E_NONE;int l191,l288,l35,
l163 = 0;uint32 l11,l12,l159;defip_aux_scratch_entry_t l14;int l289,l290;int
index;soc_mem_t l25,l291;int l292,l293;int l294;uint32 l295[
SOC_MAX_MEM_FIELD_WORDS] = {0};int l296 = -1;int l297 = 0;soc_field_t l298[2]
= {IP_ADDR0f,IP_ADDR1f,};l291 = L3_DEFIPm;l35 = soc_mem_field32_get(l1,l291,
l8,MODE0f);l289 = soc_mem_field32_get(l1,l291,l8,GLOBAL_ROUTE0f);l290 = 
soc_mem_field32_get(l1,l291,l8,VRF_ID_0f);LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d:\n"),l29 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l35,l289,l290));soc_alpm_db_ent_type_encoding(l1,l29 == 
SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l1)+1):l290,&l11,&l12);if(l287){l11++;}if(l29
== SOC_L3_VRF_GLOBAL){SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l159);
soc_mem_field32_set(l1,l291,l8,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l1,l291,
l8,VRF_ID_0f,0);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l159);}sal_memset(&l14
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l1,l8,l35,l11,
l12,0,&l14));if(l29 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l1,l291,l8,
GLOBAL_ROUTE0f,l289);soc_mem_field32_set(l1,l291,l8,VRF_ID_0f,l290);}
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l14,TRUE,&l163,
tcam_index,bucket_index));if(l163 == 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Could not find bucket\n")));return SOC_E_NOT_FOUND;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit in memory %s, index %d, "
"bucket_index %d\n"),SOC_MEM_NAME(l1,l291),soc_alpm_logical_idx(l1,l291,(*
tcam_index)>>1,1),*bucket_index));l25 = (l35)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l142 = l141(l1,l8,&l293);if(SOC_FAILURE(l142)){return l142
;}l294 = 24;if(l35){if(SOC_ALPM_V6_SCALE_CHECK(l1,l35)){l294 = 32;}else{l294 = 
16;}}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Start searching mem %s bucket %d(count %d) ""for Length %d\n"),SOC_MEM_NAME(
l1,l25),*bucket_index,l294,l293));for(l191 = 0;l191<l294;l191++){uint32 l233[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l154[2] = {0};uint32 l299[2] = {0};
uint32 l300[2] = {0};int l301;l142 = _soc_alpm_mem_index(l1,l25,*bucket_index
,l191,l159,&index);if(l142 == SOC_E_FULL){continue;}SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l25,MEM_BLOCK_ANY,index,(void*)l233));l301 = 
soc_mem_field32_get(l1,l25,l233,VALIDf);l292 = soc_mem_field32_get(l1,l25,
l233,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket %5d Index %6d: Valid %d, Length %d\n"),*bucket_index,index,l301,l292)
);if(!l301||(l292>l293)){continue;}SHR_BITSET_RANGE(l154,(l35?64:32)-l292,
l292);(void)soc_mem_field_get(l1,l25,(uint32*)l233,KEYf,(uint32*)l299);l300[1
] = soc_mem_field32_get(l1,l291,l8,l298[1]);l300[0] = soc_mem_field32_get(l1,
l291,l8,l298[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tmask %08x %08x\n\t key %08x %08x\n""\thost %08x %08x\n"),l154[1],l154[0],
l299[1],l299[0],l300[1],l300[0]));for(l288 = l35?1:0;l288>= 0;l288--){if((
l300[l288]&l154[l288])!= (l299[l288]&l154[l288])){break;}}if(l288>= 0){
continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket %d, ""index %d\n"),SOC_MEM_NAME(l1,l25),*
bucket_index,index));if(l296 == -1||l296<l292){l296 = l292;l297 = index;
sal_memcpy(l295,l233,sizeof(l233));}}if(l296!= -1){l142 = l28(l1,l295,l25,l35
,l29,*bucket_index,l297,l16);if(SOC_SUCCESS(l142)){*l134 = l297;if(bsl_check(
bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){LOG_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"Hit mem %s bucket %d, index %d\n"),SOC_MEM_NAME(l1,l25),*
bucket_index,l297));}}return l142;}*l134 = soc_alpm_logical_idx(l1,l291,(*
tcam_index)>>1,1);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Miss in mem %s bucket %d, use associate data ""in mem %s LOG index %d\n"),
SOC_MEM_NAME(l1,l25),*bucket_index,SOC_MEM_NAME(l1,l291),*l134));
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l291,MEM_BLOCK_ANY,*l134,(void*)l16));if(
(!l35)&&((*tcam_index)&1)){l142 = soc_alpm_lpm_ip4entry1_to_0(l1,l16,l16,
PRESERVE_HIT);}return SOC_E_NONE;}int soc_alpm_find_best_match(int l1,void*l8
,void*l16,int*l17,int l287){int l142 = SOC_E_NONE;int l191,l302,l303;
defip_entry_t l304;uint32 l305[2];uint32 l299[2];uint32 l306[2];uint32 l300[2
];uint32 l307,l308;int l169,l29 = 0;int l309[2] = {0};int tcam_index,
bucket_index;soc_mem_t l291 = L3_DEFIPm;int l145,l35,l310,l311 = 0;
soc_field_t l312[] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l313[] = {
GLOBAL_ROUTE0f,GLOBAL_ROUTE1f};l35 = soc_mem_field32_get(l1,l291,l8,MODE0f);
if(!SOC_URPF_STATUS_GET(l1)&&l287){return SOC_E_PARAM;}l302 = 
soc_mem_index_min(l1,l291);l303 = soc_mem_index_count(l1,l291);if(
SOC_URPF_STATUS_GET(l1)){l303>>= 1;}if(soc_alpm_mode_get(l1)){l303>>= 1;l302
+= l303;}if(l287){l302+= soc_mem_index_count(l1,l291)/2;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Launch LPM searching from index %d count %d\n"),l302,l303));for(l191 = l302;
l191<l302+l303;l191++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l291,MEM_BLOCK_ANY
,l191,(void*)&l304));l309[0] = soc_mem_field32_get(l1,l291,&l304,VALID0f);
l309[1] = soc_mem_field32_get(l1,l291,&l304,VALID1f);if(l309[0] == 0&&l309[1]
== 0){continue;}l310 = soc_mem_field32_get(l1,l291,&l304,MODE0f);if(l310!= 
l35){continue;}for(l145 = 0;l145<(l35?1:2);l145++){if(l309[l145] == 0){
continue;}l307 = soc_mem_field32_get(l1,l291,&l304,l312[l145]);l308 = 
soc_mem_field32_get(l1,l291,&l304,l313[l145]);if(!l307||!l308){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Match a Global High route: ent %d\n"),l145));l305[0] = soc_mem_field32_get(
l1,l291,&l304,IP_ADDR_MASK0f);l305[1] = soc_mem_field32_get(l1,l291,&l304,
IP_ADDR_MASK1f);l299[0] = soc_mem_field32_get(l1,l291,&l304,IP_ADDR0f);l299[1
] = soc_mem_field32_get(l1,l291,&l304,IP_ADDR1f);l306[0] = 
soc_mem_field32_get(l1,l291,l8,IP_ADDR_MASK0f);l306[1] = soc_mem_field32_get(
l1,l291,l8,IP_ADDR_MASK1f);l300[0] = soc_mem_field32_get(l1,l291,l8,IP_ADDR0f
);l300[1] = soc_mem_field32_get(l1,l291,l8,IP_ADDR1f);LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\thmsk %08x %08x\n\thkey %08x %08x\n"
"\tsmsk %08x %08x\n\tskey %08x %08x\n"),l305[1],l305[0],l299[1],l299[0],l306[
1],l306[0],l300[1],l300[0]));if(l35&&(((l305[1]&l306[1])!= l305[1])||((l305[0
]&l306[0])!= l305[0]))){continue;}if(!l35&&((l305[l145]&l306[0])!= l305[l145]
)){continue;}if(l35&&((l300[0]&l305[0]) == (l299[0]&l305[0]))&&((l300[1]&l305
[1]) == (l299[1]&l305[1]))){l311 = TRUE;break;}if(!l35&&((l300[0]&l305[l145])
== (l299[l145]&l305[l145]))){l311 = TRUE;break;}}if(!l311){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit Global High route in index = %d(%d)\n"),l191,l145));sal_memcpy(l16,&l304
,sizeof(l304));if(!l35&&l145 == 1){l142 = soc_alpm_lpm_ip4entry1_to_0(l1,l16,
l16,PRESERVE_HIT);}*l17 = l191;return l142;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for VRF and Global Low routes\n")));SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l8,&l169,&l29));l142 = l286(l1,l8,l16,l29,&tcam_index
,&bucket_index,l17,l287);if(l142 == SOC_E_NOT_FOUND){l29 = SOC_L3_VRF_GLOBAL;
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Not found in VRF region, trying Global ""region\n")));l142 = l286(l1,l8,l16,
l29,&tcam_index,&bucket_index,l17,l287);}if(SOC_SUCCESS(l142)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit in %s region in TCAM index %d, "
"buckekt_index %d\n"),l29 == SOC_L3_VRF_GLOBAL?"Global Low":"VRF",tcam_index,
bucket_index));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Search miss for given address\n")));}return(l142);}int soc_alpm_delete(int l1
,void*l8,int l282,int l283){int l169,l29;int l10;int l142 = SOC_E_NONE;int
l254 = 0;int l284 = 0;l10 = soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE0f);if(
soc_feature(l1,soc_feature_ipmc_defip)){l284 = soc_mem_field32_get(l1,
L3_DEFIPm,l8,MULTICAST_ROUTE0f);}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,
l8,&l169,&l29));if((l169 == SOC_L3_VRF_OVERRIDE)||(l284)){l142 = l7(l1,l8);if
(SOC_SUCCESS(l142)){VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l10);VRF_TRIE_ROUTES_DEC(
l1,MAX_VRF_ID,l10);}return(l142);}else{int tcam_index;if(!
VRF_TRIE_INIT_COMPLETED(l1,l29,l10)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_delete:VRF %d/%d is not initialized\n"),l29,l10));return
SOC_E_NONE;}if(l282 == -1){l282 = 0;}l254 = !(l283&SOC_ALPM_DELETE_ALL);
tcam_index = l283&~(SOC_ALPM_LOOKUP_HIT|SOC_ALPM_DELETE_ALL);l142 = l253(l1,
l8,SOC_ALPM_BKT_ENTRY_TO_IDX(l282),tcam_index,l282,l254);}return(l142);}
static int l20(int l1){int l314;SOC_ALPM_BUCKET_COUNT(l1) = 
SOC_TD2_ALPM_MAX_BKTS;if(SOC_URPF_STATUS_GET(l1)){SOC_ALPM_BUCKET_COUNT(l1)
>>= 1;}SOC_ALPM_BUCKET_BMAP_SIZE(l1) = SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT
(l1));SOC_ALPM_BUCKET_BMAP(l1) = sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l1),
"alpm_shared_bucket_bitmap");if(SOC_ALPM_BUCKET_BMAP(l1) == NULL){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_shared_mem_init: Memory allocation for "
"bucket bitmap management failed\n")));return SOC_E_MEMORY;}sal_memset(
SOC_ALPM_BUCKET_BMAP(l1),0,SOC_ALPM_BUCKET_BMAP_SIZE(l1));alpm_bucket_assign(
l1,&l314,1);return SOC_E_NONE;}int alpm_bucket_assign(int l1,int*l272,int l35
){int l191,l315 = 1,l316 = 0;if(l35){if(!soc_alpm_mode_get(l1)&&!
SOC_URPF_STATUS_GET(l1)){l315 = 2;}}for(l191 = 0;l191<SOC_ALPM_BUCKET_COUNT(
l1);l191+= l315){SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l191,l315,l316);
if(0 == l316){break;}}if(l191 == SOC_ALPM_BUCKET_COUNT(l1)){return SOC_E_FULL
;}SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l191,l315);*l272 = l191;
SOC_ALPM_BUCKET_NEXT_FREE(l1) = l191;return SOC_E_NONE;}int
alpm_bucket_release(int l1,int l272,int l35){int l315 = 1,l316 = 0;if((l272<1
)||(l272>SOC_ALPM_BUCKET_MAX_INDEX(l1))){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Unit %d\n, freeing invalid bucket index %d\n"),l1,l272));
return SOC_E_PARAM;}if(l35){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(
l1)){l315 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l272,l315,l316);if
(!l316){return SOC_E_PARAM;}SHR_BITCLR_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l272,
l315);return SOC_E_NONE;}int alpm_bucket_is_assigned(int l1,int l317,int l10,
int*l316){int l315 = 1;if((l317<1)||(l317>SOC_ALPM_BUCKET_MAX_INDEX(l1))){
return SOC_E_PARAM;}if(l10){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(
l1)){l315 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l317,l315,*l316);
return SOC_E_NONE;}static void l121(int l1,void*l16,int index,l116 l122){if(
index&(0x8000)){l122[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l16),(l109[(l1)]->l73));l122[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l75));l122[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l16),(l109[(l1)]->l74));l122[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l76));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l89)!= NULL))){int
l318;(void)soc_alpm_lpm_vrf_get(l1,l16,(int*)&l122[4],&l318);}else{l122[4] = 
0;};}else{if(index&0x1){l122[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(l1)]->l74));l122[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l76));l122[2] = 0;l122[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l109[(l1)]->l90)!= NULL))){int l318;defip_entry_t l319;(void)
soc_alpm_lpm_ip4entry1_to_0(l1,l16,&l319,0);(void)soc_alpm_lpm_vrf_get(l1,&
l319,(int*)&l122[4],&l318);}else{l122[4] = 0;};}else{l122[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l73));l122[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l16),(l109[(l1)]->l75));l122[2] = 0;l122[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l89)!= NULL))){int l318;(void)
soc_alpm_lpm_vrf_get(l1,l16,(int*)&l122[4],&l318);}else{l122[4] = 0;};}}}
static int l320(l116 l118,l116 l119){int l149;for(l149 = 0;l149<5;l149++){{if
((l118[l149])<(l119[l149])){return-1;}if((l118[l149])>(l119[l149])){return 1;
}};}return(0);}static void l321(int l1,void*l5,uint32 l322,uint32 l136,int
l133){l116 l323;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l109[(l1)]->l77))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l109[(l1)]->l88))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l109[(
l1)]->l87))){l323[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l109[(l1)]->l73));l323[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l109[(
l1)]->l75));l323[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l109[(l1)]->l74));l323[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l109[(
l1)]->l76));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l89)!= NULL))){int
l318;(void)soc_alpm_lpm_vrf_get(l1,l5,(int*)&l323[4],&l318);}else{l323[4] = 0
;};l135((l120[(l1)]),l320,l323,l133,l136,((uint16)l322<<1)|(0x8000));}}else{
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l109
[(l1)]->l87))){l323[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l109[(l1)]->l73));l323[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l109[(
l1)]->l75));l323[2] = 0;l323[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l109[(l1)]->l89)!= NULL))){int l318;(void)soc_alpm_lpm_vrf_get(l1,l5,(int*)&
l323[4],&l318);}else{l323[4] = 0;};l135((l120[(l1)]),l320,l323,l133,l136,((
uint16)l322<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l109[(l1)]->l88))){l323[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l109[(
l1)]->l74));l323[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l109[(l1)]->l76));l323[2] = 0;l323[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l90)!= NULL))){int l318;defip_entry_t
l319;(void)soc_alpm_lpm_ip4entry1_to_0(l1,l5,&l319,0);(void)
soc_alpm_lpm_vrf_get(l1,&l319,(int*)&l323[4],&l318);}else{l323[4] = 0;};l135(
(l120[(l1)]),l320,l323,l133,l136,(((uint16)l322<<1)+1));}}}static void l324(
int l1,void*l8,uint32 l322){l116 l323;int l133 = -1;int l142;uint16 index;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l77))){l323[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l109[(l1)]->l73));l323[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l75));l323[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l109[(l1)]->l74));l323[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l76));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l89)!= NULL))){int
l318;(void)soc_alpm_lpm_vrf_get(l1,l8,(int*)&l323[4],&l318);}else{l323[4] = 0
;};index = (l322<<1)|(0x8000);}else{l323[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l73));l323[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l109[(l1)]->l75));l323[2] = 0;l323[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l89)!= NULL))){int l318;(void)
soc_alpm_lpm_vrf_get(l1,l8,(int*)&l323[4],&l318);}else{l323[4] = 0;};index = 
l322;}l142 = l138((l120[(l1)]),l320,l323,l133,index);if(SOC_FAILURE(l142)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\ndel  index: H %d error %d\n"),
index,l142));}}static int l325(int l1,void*l8,int l133,int*l134){l116 l323;
int l326;int l142;uint16 index = (0xFFFF);l326 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l77));if(l326){l323[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(l1)]->l73));l323[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l75));l323[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l109[(l1)]->l74));l323[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l76));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l89)!= NULL))){int
l318;(void)soc_alpm_lpm_vrf_get(l1,l8,(int*)&l323[4],&l318);}else{l323[4] = 0
;};}else{l323[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l109[(l1)]->l73));l323[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l75));l323[2] = 0;l323[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l109[(l1)]->l89)!= NULL))){int l318;(void)soc_alpm_lpm_vrf_get(l1,l8,(int*)&
l323[4],&l318);}else{l323[4] = 0;};}l142 = l130((l120[(l1)]),l320,l323,l133,&
index);if(SOC_FAILURE(l142)){*l134 = 0xFFFFFFFF;return(l142);}*l134 = index;
return(SOC_E_NONE);}static int l327(int l1,void*l8,uint32 l322){l116 l323;int
l133 = -1;int l142;uint16 index;if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(l1)]->l77))){l323[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l73));l323[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l109[(l1)]->l75));l323[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l74));l323[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l109[(l1)]->l76));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1
)]->l89)!= NULL))){int l318;(void)soc_alpm_lpm_vrf_get(l1,l8,(int*)&l323[4],&
l318);}else{l323[4] = 0;};index = (l322<<1)|(0x8000);}else{l323[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l73));l323[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l109[(l1)]->l75));l323[2] = 0;l323[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l89)!= NULL))){int l318;(void)
soc_alpm_lpm_vrf_get(l1,l8,(int*)&l323[4],&l318);}else{l323[4] = 0;};index = 
l322;}l142 = l140((l120[(l1)]),l320,l323,l133,index);if(SOC_FAILURE(l142)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nveri index: H %d error %d\n"),
index,l142));}return l142;}static uint16 l123(uint8*l124,int l125){return(
_shr_crc16b(0,l124,l125));}static int l126(int l22,int l111,int l112,l115**
l127){l115*l131;int index;if(l112>l111){return SOC_E_MEMORY;}l131 = sal_alloc
(sizeof(l115),"lpm_hash");if(l131 == NULL){return SOC_E_MEMORY;}sal_memset(
l131,0,sizeof(*l131));l131->l22 = l22;l131->l111 = l111;l131->l112 = l112;
l131->l113 = sal_alloc(l131->l112*sizeof(*(l131->l113)),"hash_table");if(l131
->l113 == NULL){sal_free(l131);return SOC_E_MEMORY;}l131->l114 = sal_alloc(
l131->l111*sizeof(*(l131->l114)),"link_table");if(l131->l114 == NULL){
sal_free(l131->l113);sal_free(l131);return SOC_E_MEMORY;}for(index = 0;index<
l131->l112;index++){l131->l113[index] = (0xFFFF);}for(index = 0;index<l131->
l111;index++){l131->l114[index] = (0xFFFF);}*l127 = l131;return SOC_E_NONE;}
static int l128(l115*l129){if(l129!= NULL){sal_free(l129->l113);sal_free(l129
->l114);sal_free(l129);}return SOC_E_NONE;}static int l130(l115*l131,l117 l132
,l116 entry,int l133,uint16*l134){int l1 = l131->l22;uint16 l328;uint16 index
;l328 = l123((uint8*)entry,(32*5))%l131->l112;index = l131->l113[l328];;;
while(index!= (0xFFFF)){uint32 l16[SOC_MAX_MEM_FIELD_WORDS];l116 l122;int l329
;l329 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,l329,l16));l121(l1,l16,index,l122);if((*l132)(entry,l122) == 0)
{*l134 = (index&(0x7FFF))>>((index&(0x8000))?1:0);;return(SOC_E_NONE);}index = 
l131->l114[index&(0x7FFF)];;};return(SOC_E_NOT_FOUND);}static int l135(l115*
l131,l117 l132,l116 entry,int l133,uint16 l136,uint16 l137){int l1 = l131->
l22;uint16 l328;uint16 index;uint16 l330;l328 = l123((uint8*)entry,(32*5))%
l131->l112;index = l131->l113[l328];;;;l330 = (0xFFFF);if(l136!= (0xFFFF)){
while(index!= (0xFFFF)){uint32 l16[SOC_MAX_MEM_FIELD_WORDS];l116 l122;int l329
;l329 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,l329,l16));l121(l1,l16,index,l122);if((*l132)(entry,l122) == 0)
{if(l137!= index){;if(l330 == (0xFFFF)){l131->l113[l328] = l137;l131->l114[
l137&(0x7FFF)] = l131->l114[index&(0x7FFF)];l131->l114[index&(0x7FFF)] = (
0xFFFF);}else{l131->l114[l330&(0x7FFF)] = l137;l131->l114[l137&(0x7FFF)] = 
l131->l114[index&(0x7FFF)];l131->l114[index&(0x7FFF)] = (0xFFFF);}};return(
SOC_E_NONE);}l330 = index;index = l131->l114[index&(0x7FFF)];;}}l131->l114[
l137&(0x7FFF)] = l131->l113[l328];l131->l113[l328] = l137;return(SOC_E_NONE);
}static int l138(l115*l131,l117 l132,l116 entry,int l133,uint16 l139){uint16
l328;uint16 index;uint16 l330;l328 = l123((uint8*)entry,(32*5))%l131->l112;
index = l131->l113[l328];;;l330 = (0xFFFF);while(index!= (0xFFFF)){if(l139 == 
index){;if(l330 == (0xFFFF)){l131->l113[l328] = l131->l114[l139&(0x7FFF)];
l131->l114[l139&(0x7FFF)] = (0xFFFF);}else{l131->l114[l330&(0x7FFF)] = l131->
l114[l139&(0x7FFF)];l131->l114[l139&(0x7FFF)] = (0xFFFF);}return(SOC_E_NONE);
}l330 = index;index = l131->l114[index&(0x7FFF)];;}return(SOC_E_NOT_FOUND);}
static int l140(l115*l131,l117 l132,l116 entry,int l133,uint16 l331){uint16
l328;uint16 index;l328 = l123((uint8*)entry,(32*5))%l131->l112;index = l131->
l113[l328];;;while(index!= (0xFFFF)){if(l331 == index){;return(SOC_E_NONE);}
index = l131->l114[index&(0x7FFF)];;}return(SOC_E_NOT_FOUND);}int _ipmask2pfx
(uint32 l332,int*l333){*l333 = 0;while(l332&(1<<31)){*l333+= 1;l332<<= 1;}
return((l332)?SOC_E_PARAM:SOC_E_NONE);}int soc_alpm_lpm_ip4entry0_to_0(int l1
,void*l334,void*l335,int l336){uint32 l143;l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l87),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l77));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l77),(l143));if(((l109[(l1)]->l79)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l79));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l79),(l143));}if(((l109[(l1)]->l57)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l57));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l57),(l143));}if(soc_feature(l1,soc_feature_ipmc_defip)){
l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334)
,(l109[(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l335),(l109[(l1)]->l59),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l61),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l63));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l63),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l65));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l65),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l67));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l67),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l69));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l69),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l73),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l75),(l143));if(((l109[(l1)]->l51)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l51),(l143));}if(((l109[(l1)]->l53)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l53));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l53),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l55));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l55),(l143));}else{l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l81),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l83));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l83),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l85),(l143));if(((l109[(l1)]->l89)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l89),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l91),(l143));}if(((l109[(l1)]->l49)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l49),(l143));}if(((l109[(l1)]->l47)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l47));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l47),(l143));}if(l336){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l71));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l71),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l93));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l93),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l95),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l97));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l97),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l99));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l99),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l101));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l101),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l103));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l335),(l109[(l1)]->l103),(l143));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_1(int l1,void*l334,void*l335,int l336){uint32 l143;
l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334)
,(l109[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l335),(l109[(l1)]->l88),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l78),(l143));if(((l109[(l1)]->l80)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l80));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l80),(l143));}if(((l109[(l1)]->l58)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l58));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l58),(l143));}if(soc_feature(l1,soc_feature_ipmc_defip)){
l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334)
,(l109[(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l335),(l109[(l1)]->l60),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l62));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l62),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l64));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l64),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l66),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l68));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l68),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l70));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l70),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l74),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l76),(l143));if(((l109[(l1)]->l52)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l52));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l52),(l143));}if(((l109[(l1)]->l54)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l54));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l54),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l56));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l56),(l143));}else{l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l82),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l84),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l86),(l143));if(((l109[(l1)]->l90)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l90),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l92),(l143));}if(((l109[(l1)]->l50)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l50),(l143));}if(((l109[(l1)]->l48)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l48),(l143));}if(l336){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l72));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l72),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l94));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l94),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l96),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l98));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l98),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l100));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l100),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l102));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l102),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l104));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l335),(l109[(l1)]->l104),(l143));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_1(int l1,void*l334,void*l335,int l336){uint32 l143;
l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334)
,(l109[(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l335),(l109[(l1)]->l88),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l78),(l143));if(((l109[(l1)]->l79)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l79));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l80),(l143));}if(((l109[(l1)]->l57)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l57));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l58),(l143));}if(soc_feature(l1,soc_feature_ipmc_defip)){
l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334)
,(l109[(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l335),(l109[(l1)]->l60),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l62),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l63));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l64),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l65));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l66),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l67));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l68),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l69));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l70),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l74),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l76),(l143));if(!SOC_IS_HURRICANE(l1)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l52),(l143));}if(((l109[(l1)]->l53)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l53));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l54),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l55));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l56),(l143));}else{l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l82),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l83));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l84),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l86),(l143));if(((l109[(l1)]->l89)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l90),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l92),(l143));}if(((l109[(l1)]->l49)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l50),(l143));}if(((l109[(l1)]->l47)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l47));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l48),(l143));}if(l336){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l71));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l72),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l93));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l94),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l96),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l97));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l98),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l99));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l100),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l101));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l102),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l103));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l335),(l109[(l1)]->l104),(l143));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_0(int l1,void*l334,void*l335,int l336){uint32 l143;
l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334)
,(l109[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l335),(l109[(l1)]->l87),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l77),(l143));if(((l109[(l1)]->l79)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l80));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l79),(l143));}if(((l109[(l1)]->l58)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l58));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l57),(l143));}if(soc_feature(l1,soc_feature_ipmc_defip)){
l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334)
,(l109[(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l335),(l109[(l1)]->l59),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l62));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l61),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l64));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l63),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l65),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l68));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l67),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l70));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l69),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l73),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l75),(l143));if(!SOC_IS_HURRICANE(l1)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l52));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l51),(l143));}if(((l109[(l1)]->l54)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l54));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l53),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l56));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l55),(l143));}else{l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l81),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l83),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l85),(l143));if(((l109[(l1)]->l90)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l89),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l91),(l143));}if(((l109[(l1)]->l50)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l49),(l143));}if(((l109[(l1)]->l48)!= NULL)){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l47),(l143));}if(l336){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l72));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l71),(l143));}l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l94));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l93),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l95),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l98));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l335),(l109[(l1)]->l97),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[(l1)]->l100));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[
(l1)]->l99),(l143));l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l334),(l109[(l1)]->l102));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l335),(l109[(l1)]->l101),(l143));l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l334),(l109[
(l1)]->l104));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l335),(l109[(l1)]->l103),(l143));return(SOC_E_NONE);}static int l337(int l1
,void*l16){return(SOC_E_NONE);}void soc_alpm_lpm_state_dump(int l1){int l191;
int l338;l338 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l1)){return;}for(l191 = l338;l191>= 0;l191--){if((l191!= (
(3*(64+32+2+1))-1))&&((l45[(l1)][(l191)].l38) == -1)){continue;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l191,(l45
[(l1)][(l191)].l40),(l45[(l1)][(l191)].next),(l45[(l1)][(l191)].l38),(l45[(l1
)][(l191)].l39),(l45[(l1)][(l191)].l41),(l45[(l1)][(l191)].l42)));}
COMPILER_REFERENCE(l337);}static int l339(int l1,int l340,int index,uint32*
l16,int l6){int l341;int l10;uint32 l342;uint32 l343;int l344;uint32 l345[
SOC_MAX_MEM_FIELD_WORDS] = {0};int l346 = 0,l347 = 0;if(!SOC_URPF_STATUS_GET(
l1)){return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l341 = 
(soc_mem_index_count(l1,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l1)){l341 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1)+0x0400;}else{l341 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}l10 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l77));if(l6>= 2){SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,
l340+l341,l345));l346 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l345),(l109[(l1)]->l85));l347 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l345),(l109[
(l1)]->l86));}else{l346 = l6;l347 = l6;}if(((l109[(l1)]->l49)!= NULL)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l49),(0));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm
)),(l16),(l109[(l1)]->l50),(0));}l342 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(l1)]->l75));l343 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l76));if(!l10){if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l16),(l109[(l1)]->l87))){l344 = (!l342)?1:0;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l85),(l346|l344));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l16),(l109[(l1)]->l88))){l344 = (!l343)?1:0;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l86),(l347|l344));}}else{l344 = ((!l342)&&(!l343))?1:0;l342 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l87));l343 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l16),(l109[(l1)]->l88));if(l342&&l343){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l85),(l346|l344));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l16),(l109[(l1)]->l86),(l347|l344));}}return l186(l1,
MEM_BLOCK_ANY,index+l341,index,l16);}static int l348(int l1,int l349,int l350
){uint32 l16[SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,l349,l16));l321(l1,l16,l350,0x4000,0);SOC_IF_ERROR_RETURN(l186(
l1,MEM_BLOCK_ANY,l350,l349,l16));SOC_IF_ERROR_RETURN(l339(l1,l349,l350,l16,2)
);do{int l351,l352;l351 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l349),1);l352
= soc_alpm_physical_idx((l1),L3_DEFIPm,(l350),1);ALPM_TCAM_PIVOT(l1,l352<<1) = 
ALPM_TCAM_PIVOT(l1,l351<<1);ALPM_TCAM_PIVOT(l1,(l352<<1)+1) = ALPM_TCAM_PIVOT
(l1,(l351<<1)+1);if(ALPM_TCAM_PIVOT((l1),l352<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l1),l352<<1)) = l352<<1;}if(ALPM_TCAM_PIVOT((l1),(l352<<1)+1
)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(l352<<1)+1)) = (l352<<1)+1;}
ALPM_TCAM_PIVOT(l1,l351<<1) = NULL;ALPM_TCAM_PIVOT(l1,(l351<<1)+1) = NULL;}
while(0);return(SOC_E_NONE);}static int l353(int l1,int l133,int l10){uint32
l16[SOC_MAX_MEM_FIELD_WORDS];int l349;int l350;uint32 l354,l355;l350 = (l45[(
l1)][(l133)].l39)+1;if(!l10){l349 = (l45[(l1)][(l133)].l39);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l349,l16));l354 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l87));l355 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l16),(l109[(l1)]->l88));if((l354 == 0)||(l355 == 0)){l321(l1,l16
,l350,0x4000,0);SOC_IF_ERROR_RETURN(l186(l1,MEM_BLOCK_ANY,l350,l349,l16));
SOC_IF_ERROR_RETURN(l339(l1,l349,l350,l16,2));do{int l356 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l349),1)<<1;int l248 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l350),1)*2+(l356&1);if((l355)){l356++;}
ALPM_TCAM_PIVOT((l1),l248) = ALPM_TCAM_PIVOT((l1),l356);if(ALPM_TCAM_PIVOT((
l1),l248)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l248)) = l248;}
ALPM_TCAM_PIVOT((l1),l356) = NULL;}while(0);l350--;}}l349 = (l45[(l1)][(l133)
].l38);if(l349!= l350){SOC_IF_ERROR_RETURN(l348(l1,l349,l350));
VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l10);}(l45[(l1)][(l133)].l38)+= 1;(l45[(l1)
][(l133)].l39)+= 1;return(SOC_E_NONE);}static int l357(int l1,int l133,int l10
){uint32 l16[SOC_MAX_MEM_FIELD_WORDS];int l349;int l350;int l358;uint32 l354,
l355;l350 = (l45[(l1)][(l133)].l38)-1;if((l45[(l1)][(l133)].l41) == 0){(l45[(
l1)][(l133)].l38) = l350;(l45[(l1)][(l133)].l39) = l350-1;return(SOC_E_NONE);
}if((!l10)&&((l45[(l1)][(l133)].l39)!= (l45[(l1)][(l133)].l38))){l349 = (l45[
(l1)][(l133)].l39);SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l349,
l16));l354 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l16),(l109[(l1)]->l87));l355 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(l1)]->l88));if((l354 == 0)||(l355 == 
0)){l358 = l349-1;SOC_IF_ERROR_RETURN(l348(l1,l358,l350));VRF_PIVOT_SHIFT_INC
(l1,MAX_VRF_ID,l10);l321(l1,l16,l358,0x4000,0);SOC_IF_ERROR_RETURN(l186(l1,
MEM_BLOCK_ANY,l358,l349,l16));SOC_IF_ERROR_RETURN(l339(l1,l349,l358,l16,2));
do{int l356 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l349),1)<<1;int l248 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l358),1)*2+(l356&1);if((l355)){l356++;}
ALPM_TCAM_PIVOT((l1),l248) = ALPM_TCAM_PIVOT((l1),l356);if(ALPM_TCAM_PIVOT((
l1),l248)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l248)) = l248;}
ALPM_TCAM_PIVOT((l1),l356) = NULL;}while(0);}else{l321(l1,l16,l350,0x4000,0);
SOC_IF_ERROR_RETURN(l186(l1,MEM_BLOCK_ANY,l350,l349,l16));SOC_IF_ERROR_RETURN
(l339(l1,l349,l350,l16,2));do{int l351,l352;l351 = soc_alpm_physical_idx((l1)
,L3_DEFIPm,(l349),1);l352 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l350),1);
ALPM_TCAM_PIVOT(l1,l352<<1) = ALPM_TCAM_PIVOT(l1,l351<<1);ALPM_TCAM_PIVOT(l1,
(l352<<1)+1) = ALPM_TCAM_PIVOT(l1,(l351<<1)+1);if(ALPM_TCAM_PIVOT((l1),l352<<
1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l352<<1)) = l352<<1;}if(
ALPM_TCAM_PIVOT((l1),(l352<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(
l352<<1)+1)) = (l352<<1)+1;}ALPM_TCAM_PIVOT(l1,l351<<1) = NULL;
ALPM_TCAM_PIVOT(l1,(l351<<1)+1) = NULL;}while(0);}}else{l349 = (l45[(l1)][(
l133)].l39);SOC_IF_ERROR_RETURN(l348(l1,l349,l350));VRF_PIVOT_SHIFT_INC(l1,
MAX_VRF_ID,l10);}(l45[(l1)][(l133)].l38)-= 1;(l45[(l1)][(l133)].l39)-= 1;
return(SOC_E_NONE);}static int l359(int l1,int l133,int l10,void*l16,int*l360
){int l361;int l362;int l363;int l364;int l349;uint32 l354,l355;int l142;if((
l45[(l1)][(l133)].l41) == 0){l364 = ((3*(64+32+2+1))-1);if(soc_alpm_mode_get(
l1) == SOC_ALPM_MODE_PARALLEL){if(l133<= (((3*(64+32+2+1))/3)-1)){l364 = (((3
*(64+32+2+1))/3)-1);}}while((l45[(l1)][(l364)].next)>l133){l364 = (l45[(l1)][
(l364)].next);}l362 = (l45[(l1)][(l364)].next);if(l362!= -1){(l45[(l1)][(l362
)].l40) = l133;}(l45[(l1)][(l133)].next) = (l45[(l1)][(l364)].next);(l45[(l1)
][(l133)].l40) = l364;(l45[(l1)][(l364)].next) = l133;(l45[(l1)][(l133)].l42)
= ((l45[(l1)][(l364)].l42)+1)/2;(l45[(l1)][(l364)].l42)-= (l45[(l1)][(l133)].
l42);(l45[(l1)][(l133)].l38) = (l45[(l1)][(l364)].l39)+(l45[(l1)][(l364)].l42
)+1;(l45[(l1)][(l133)].l39) = (l45[(l1)][(l133)].l38)-1;(l45[(l1)][(l133)].
l41) = 0;}else if(!l10){l349 = (l45[(l1)][(l133)].l38);if((l142 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l349,l16))<0){return l142;}l354 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l87));l355 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l16),(l109[(l1)]->l88));if((l354 == 0)||(l355 == 0)){*l360 = (
l349<<1)+((l355 == 0)?1:0);return(SOC_E_NONE);}l349 = (l45[(l1)][(l133)].l39)
;if((l142 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l349,l16))<0){return l142;}l354 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l16),(l109[(
l1)]->l87));l355 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l16),(l109[(l1)]->l88));if((l354 == 0)||(l355 == 0)){*l360 = (
l349<<1)+((l355 == 0)?1:0);return(SOC_E_NONE);}}l363 = l133;while((l45[(l1)][
(l363)].l42) == 0){l363 = (l45[(l1)][(l363)].next);if(l363 == -1){l363 = l133
;break;}}while((l45[(l1)][(l363)].l42) == 0){l363 = (l45[(l1)][(l363)].l40);
if(l363 == -1){if((l45[(l1)][(l133)].l41) == 0){l361 = (l45[(l1)][(l133)].l40
);l362 = (l45[(l1)][(l133)].next);if(-1!= l361){(l45[(l1)][(l361)].next) = 
l362;}if(-1!= l362){(l45[(l1)][(l362)].l40) = l361;}}return(SOC_E_FULL);}}
while(l363>l133){l362 = (l45[(l1)][(l363)].next);SOC_IF_ERROR_RETURN(l357(l1,
l362,l10));(l45[(l1)][(l363)].l42)-= 1;(l45[(l1)][(l362)].l42)+= 1;l363 = 
l362;}while(l363<l133){SOC_IF_ERROR_RETURN(l353(l1,l363,l10));(l45[(l1)][(
l363)].l42)-= 1;l361 = (l45[(l1)][(l363)].l40);(l45[(l1)][(l361)].l42)+= 1;
l363 = l361;}(l45[(l1)][(l133)].l41)+= 1;(l45[(l1)][(l133)].l42)-= 1;(l45[(l1
)][(l133)].l39)+= 1;*l360 = (l45[(l1)][(l133)].l39)<<((l10)?0:1);sal_memcpy(
l16,soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);
return(SOC_E_NONE);}static int l365(int l1,int l133,int l10,void*l16,int l366
){int l361;int l362;int l349;int l350;uint32 l367[SOC_MAX_MEM_FIELD_WORDS];
uint32 l368[SOC_MAX_MEM_FIELD_WORDS];uint32 l369[SOC_MAX_MEM_FIELD_WORDS];
void*l370;int l142;int l371,l176;l349 = (l45[(l1)][(l133)].l39);l350 = l366;
if(!l10){l350>>= 1;if((l142 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l349,l367))<0){
return l142;}if((l142 = READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
soc_alpm_physical_idx(l1,L3_DEFIPm,l349,1),l368))<0){return l142;}if((l142 = 
READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,soc_alpm_physical_idx(l1,L3_DEFIPm,
l350,1),l369))<0){return l142;}l370 = (l350 == l349)?l367:l16;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l367),(l109[
(l1)]->l88))){l176 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l368,
BPM_LENGTH1f);if(l366&1){l142 = soc_alpm_lpm_ip4entry1_to_1(l1,l367,l370,
PRESERVE_HIT);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l369,BPM_LENGTH1f,
l176);}else{l142 = soc_alpm_lpm_ip4entry1_to_0(l1,l367,l370,PRESERVE_HIT);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l369,BPM_LENGTH0f,l176);}l371 = (
l349<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l367),(l109[(l1)]->l88),(0));}else{l176 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l368,BPM_LENGTH0f);if(l366&1){l142 = 
soc_alpm_lpm_ip4entry0_to_1(l1,l367,l370,PRESERVE_HIT);soc_mem_field32_set(l1
,L3_DEFIP_AUX_TABLEm,l369,BPM_LENGTH1f,l176);}else{l142 = 
soc_alpm_lpm_ip4entry0_to_0(l1,l367,l370,PRESERVE_HIT);soc_mem_field32_set(l1
,L3_DEFIP_AUX_TABLEm,l369,BPM_LENGTH0f,l176);}l371 = l349<<1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l367),(l109[
(l1)]->l87),(0));(l45[(l1)][(l133)].l41)-= 1;(l45[(l1)][(l133)].l42)+= 1;(l45
[(l1)][(l133)].l39)-= 1;}l371 = soc_alpm_physical_idx(l1,L3_DEFIPm,l371,0);
l366 = soc_alpm_physical_idx(l1,L3_DEFIPm,l366,0);ALPM_TCAM_PIVOT(l1,l366) = 
ALPM_TCAM_PIVOT(l1,l371);if(ALPM_TCAM_PIVOT(l1,l366)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l1,l366)) = l366;}ALPM_TCAM_PIVOT(l1,l371) = NULL;if((l142 = 
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ALL,soc_alpm_physical_idx(l1,L3_DEFIPm
,l350,1),l369))<0){return l142;}if(l350!= l349){l321(l1,l370,l350,0x4000,0);
if((l142 = l186(l1,MEM_BLOCK_ANY,l350,l350,l370))<0){return l142;}if((l142 = 
l339(l1,l349,l350,l370,2))<0){return l142;}}l321(l1,l367,l349,0x4000,0);if((
l142 = l186(l1,MEM_BLOCK_ANY,l349,l349,l367))<0){return l142;}if((l142 = l339
(l1,l349,l349,l367,0))<0){return l142;}}else{(l45[(l1)][(l133)].l41)-= 1;(l45
[(l1)][(l133)].l42)+= 1;(l45[(l1)][(l133)].l39)-= 1;if(l350!= l349){if((l142 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l349,l367))<0){return l142;}l321(l1,l367,l350
,0x4000,0);if((l142 = l186(l1,MEM_BLOCK_ANY,l350,l349,l367))<0){return l142;}
if((l142 = l339(l1,l349,l350,l367,2))<0){return l142;}}l366 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l350,1);l371 = soc_alpm_physical_idx(l1,
L3_DEFIPm,l349,1);ALPM_TCAM_PIVOT(l1,l366<<1) = ALPM_TCAM_PIVOT(l1,l371<<1);
ALPM_TCAM_PIVOT(l1,l371<<1) = NULL;if(ALPM_TCAM_PIVOT(l1,l366<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,l366<<1)) = l366<<1;}sal_memcpy(l367,
soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);l321(l1
,l367,l349,0x4000,0);if((l142 = l186(l1,MEM_BLOCK_ANY,l349,l349,l367))<0){
return l142;}if((l142 = l339(l1,l349,l349,l367,0))<0){return l142;}}if((l45[(
l1)][(l133)].l41) == 0){l361 = (l45[(l1)][(l133)].l40);assert(l361!= -1);l362
= (l45[(l1)][(l133)].next);(l45[(l1)][(l361)].next) = l362;(l45[(l1)][(l361)]
.l42)+= (l45[(l1)][(l133)].l42);(l45[(l1)][(l133)].l42) = 0;if(l362!= -1){(
l45[(l1)][(l362)].l40) = l361;}(l45[(l1)][(l133)].next) = -1;(l45[(l1)][(l133
)].l40) = -1;(l45[(l1)][(l133)].l38) = -1;(l45[(l1)][(l133)].l39) = -1;}
return(l142);}int soc_alpm_lpm_vrf_get(int l22,void*lpm_entry,int*l29,int*
l372){int l169;if(((l109[(l22)]->l91)!= NULL)){l169 = 
soc_L3_DEFIPm_field32_get(l22,lpm_entry,VRF_ID_0f);*l372 = l169;if(
soc_L3_DEFIPm_field32_get(l22,lpm_entry,VRF_ID_MASK0f)){*l29 = l169;}else if(
!soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(lpm_entry)
,(l109[(l22)]->l93))){*l29 = SOC_L3_VRF_GLOBAL;*l372 = SOC_VRF_MAX(l22)+1;}
else{*l29 = SOC_L3_VRF_OVERRIDE;}}else{*l29 = SOC_L3_VRF_DEFAULT;}return(
SOC_E_NONE);}static int l36(int l1,void*entry,int*l18){int l133;int l142;int
l10;uint32 l143;int l169;int l373;l10 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l109[(l1)]->l77));if(l10){l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l109
[(l1)]->l75));if((l142 = _ipmask2pfx(l143,&l133))<0){return(l142);}l143 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l109
[(l1)]->l76));if(l133){if(l143!= 0xffffffff){return(SOC_E_PARAM);}l133+= 32;}
else{if((l142 = _ipmask2pfx(l143,&l133))<0){return(l142);}}l133+= 33;}else{
l143 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry
),(l109[(l1)]->l75));if((l142 = _ipmask2pfx(l143,&l133))<0){return(l142);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,entry,&l169,&l142));l373 = 
soc_alpm_mode_get(l1);switch(l169){case SOC_L3_VRF_GLOBAL:if(l373 == 
SOC_ALPM_MODE_PARALLEL){*l18 = l133+((3*(64+32+2+1))/3);}else{*l18 = l133;}
break;case SOC_L3_VRF_OVERRIDE:*l18 = l133+2*((3*(64+32+2+1))/3);break;
default:if(l373 == SOC_ALPM_MODE_PARALLEL){*l18 = l133;}else{*l18 = l133+((3*
(64+32+2+1))/3);}break;}return(SOC_E_NONE);}static int l15(int l1,void*l8,
void*l16,int*l17,int*l18,int*l10){int l142;int l35;int l134;int l133 = 0;l35 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(
l1)]->l77));if(l35){if(!(l35 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l109[(l1)]->l78)))){return(SOC_E_PARAM);}}*
l10 = l35;l36(l1,l8,&l133);*l18 = l133;if(l325(l1,l8,l133,&l134) == 
SOC_E_NONE){*l17 = l134;if((l142 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,(*l10)?*
l17:(*l17>>1),l16))<0){return l142;}return(SOC_E_NONE);}else{return(
SOC_E_NOT_FOUND);}}static int l2(int l1){int l338;int l191;int l314;int l374;
uint32 l375 = 0;uint32 l373;if(!soc_feature(l1,soc_feature_lpm_tcam)){return(
SOC_E_UNAVAIL);}if((l373 = soc_property_get(l1,spn_L3_ALPM_ENABLE,0))){
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l1,&l375));soc_reg_field_set(
l1,L3_DEFIP_RPF_CONTROLr,&l375,LPM_MODEf,1);if(l373 == SOC_ALPM_MODE_PARALLEL
){soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l375,LOOKUP_MODEf,1);}else{
soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l375,LOOKUP_MODEf,0);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l1,l375));
l3_alpm_sw_prefix_lookup[l1] = soc_property_get(l1,"l3_alpm_sw_prefix_lookup"
,1);l375 = 0;if(SOC_URPF_STATUS_GET(l1)){soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l375,URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l375,URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l375,URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l375,URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_KEY_SELr(l1,l375));l375 = 0;if(l373 == SOC_ALPM_MODE_PARALLEL)
{if(SOC_URPF_STATUS_GET(l1)){soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l375,
TCAM2_SELf,1);soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l375,TCAM3_SELf,1);
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l375,TCAM4_SELf,2);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l375,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l375,TCAM6_SELf,3);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l375,TCAM7_SELf,3);}else{soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l375,TCAM4_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l375,TCAM5_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l375,TCAM6_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l375,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l375,TCAM4_SELf,2);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l375,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l375,TCAM6_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l375,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l1,l375));if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l376 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l1,&l376));soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&
l376,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l376,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l376,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l376,
V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l1,l376))
;}}l338 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l1);l374 = sizeof(l43)*(l338);if(
(l45[(l1)]!= NULL)){if(soc_alpm_deinit(l1)<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}}l109[l1] = sal_alloc(sizeof(l107),"lpm_field_state");if(NULL
== l109[l1]){SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}(l109[l1])->l47 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,CLASS_ID0f);(l109[l1])->l48 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,CLASS_ID1f);(l109[l1])->l49 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD0f);(l109[l1])->l50 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD1f);(l109[l1])->l51 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP0f);(l109[l1])->l52 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP1f);(l109[l1])->l53 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT0f);(l109[l1])->l54 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT1f);(l109[l1])->l55 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR0f);(l109[l1])->l56 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR1f);(l109[l1])->l57 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE0f);(l109[l1])->l58 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE1f);(l109[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MULTICAST_ROUTE0f);(l109[l1])->l60 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MULTICAST_ROUTE1f);(l109[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPA_ID0f);(l109[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPA_ID1f);(l109[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,EXPECTED_L3_IIF0f);(l109[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,EXPECTED_L3_IIF1f);(l109[l1])->l65 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);(
l109[l1])->l66 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,
IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);(l109[l1])->l67 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);(
l109[l1])->l68 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,
IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);(l109[l1])->l69 = soc_mem_fieldinfo_get
(l1,L3_DEFIPm,L3MC_INDEX0f);(l109[l1])->l70 = soc_mem_fieldinfo_get(l1,
L3_DEFIPm,L3MC_INDEX1f);(l109[l1])->l71 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,
HIT0f);(l109[l1])->l72 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,HIT1f);(l109[l1])
->l73 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR0f);(l109[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR1f);(l109[l1])->l75 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK0f);(l109[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK1f);(l109[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE0f);(l109[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE1f);(l109[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK0f);(l109[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK1f);(l109[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX0f);(l109[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX1f);(l109[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI0f);(l109[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI1f);(l109[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE0f);(l109[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE1f);(l109[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID0f);(l109[l1])->l88 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID1f);(l109[l1])->l89 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_0f);(l109[l1])->l90 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_1f);(l109[l1])->l91 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK0f);(l109[l1])->l92 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK1f);(l109[l1])->l93 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH0f);(l109[l1])->l94 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH1f);(l109[l1])->l95 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX0f);(l109[l1])->l96 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX1f);(l109[l1])->l97 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR0f);(l109[l1])->l98 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR1f);(l109[l1])->l99 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS0f);(l109[l1])->l100 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS1f);(l109[l1])->l101 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l109[l1])->
l102 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l109[
l1])->l103 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l109
[l1])->l104 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(
l109[l1])->l105 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,ENTRY_TYPE_MASK0f);(l109
[l1])->l106 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,ENTRY_TYPE_MASK1f);(l45[(l1)
]) = sal_alloc(l374,"LPM prefix info");if(NULL == (l45[(l1)])){sal_free(l109[
l1]);l109[l1] = NULL;SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}sal_memset
((l45[(l1)]),0,l374);for(l191 = 0;l191<l338;l191++){(l45[(l1)][(l191)].l38) = 
-1;(l45[(l1)][(l191)].l39) = -1;(l45[(l1)][(l191)].l40) = -1;(l45[(l1)][(l191
)].next) = -1;(l45[(l1)][(l191)].l41) = 0;(l45[(l1)][(l191)].l42) = 0;}l314 = 
soc_mem_index_count(l1,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l1)){l314>>= 1;}if(
l373 == SOC_ALPM_MODE_PARALLEL){(l45[(l1)][(((3*(64+32+2+1))-1))].l39) = (
l314>>1)-1;(l45[(l1)][(((((3*(64+32+2+1))/3)-1)))].l42) = l314>>1;(l45[(l1)][
((((3*(64+32+2+1))-1)))].l42) = (l314-(l45[(l1)][(((((3*(64+32+2+1))/3)-1)))]
.l42));}else{(l45[(l1)][((((3*(64+32+2+1))-1)))].l42) = l314;}if((l120[(l1)])
!= NULL){if(l128((l120[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}(l120[(l1)]) = NULL;}if(l126(l1,l314*2,l314,&(l120[(l1)]))<0)
{SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;}(void)soc_alpm_ipmc_war(l1,TRUE
);SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}static int l3(int l1){if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}
SOC_ALPM_LPM_LOCK(l1);(void)soc_alpm_ipmc_war(l1,FALSE);if((l120[(l1)])!= 
NULL){l128((l120[(l1)]));(l120[(l1)]) = NULL;}if((l45[(l1)]!= NULL)){sal_free
(l109[l1]);l109[l1] = NULL;sal_free((l45[(l1)]));(l45[(l1)]) = NULL;}
SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}static int l4(int l1,void*l5,int*
l377,int l6){int l133;int index;int l10;uint32 l16[SOC_MAX_MEM_FIELD_WORDS];
int l142 = SOC_E_NONE;int l378 = 0;if(l6>= 2){return SOC_E_INTERNAL;}
sal_memcpy(l16,soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,
L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l1);l142 = l15(l1,l5,l16,&index,&l133,&l10);
if(l142 == SOC_E_NOT_FOUND){l142 = l359(l1,l133,l10,l16,&index);if(l142<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l142);}}else{l378 = 1;}*l377 = index;if(l142 == 
SOC_E_NONE){if(!l10){if(index&1){l142 = soc_alpm_lpm_ip4entry0_to_1(l1,l5,l16
,PRESERVE_HIT);}else{l142 = soc_alpm_lpm_ip4entry0_to_0(l1,l5,l16,
PRESERVE_HIT);}if(l142<0){SOC_ALPM_LPM_UNLOCK(l1);return(l142);}l5 = (void*)
l16;index>>= 1;}soc_alpm_lpm_state_dump(l1);LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\nsoc_alpm_lpm_insert: %d %d\n"),index,l133));if(!l378){l321(
l1,l5,index,0x4000,0);}l142 = l186(l1,MEM_BLOCK_ANY,index,index,l5);if(l142>= 
0){l142 = l339(l1,index,index,l5,l6);}}SOC_ALPM_LPM_UNLOCK(l1);return(l142);}
static int l7(int l1,void*l8){int l133;int index;int l10;uint32 l16[
SOC_MAX_MEM_FIELD_WORDS];int l142 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l142 = 
l15(l1,l8,l16,&index,&l133,&l10);if(l142 == SOC_E_NONE){LOG_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_alpm_lpm_delete: %d %d\n"),index,l133))
;l324(l1,l8,index);l142 = l365(l1,l133,l10,l16,index);}
soc_alpm_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l142);}static int
l19(int l1,void*l8,void*l16,int*l17){int l133;int l142;int l10;
SOC_ALPM_LPM_LOCK(l1);l142 = l15(l1,l8,l16,l17,&l133,&l10);
SOC_ALPM_LPM_UNLOCK(l1);return(l142);}static int l9(int l22,void*l8,int l10,
int l11,int l12,int l13,defip_aux_scratch_entry_t*l14){uint32 l143;uint32 l379
[4] = {0,0,0,0};int l133 = 0;int l142 = SOC_E_NONE;l143 = soc_mem_field32_get
(l22,L3_DEFIPm,l8,VALID0f);soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,
VALIDf,l143);l143 = soc_mem_field32_get(l22,L3_DEFIPm,l8,MODE0f);
soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,MODEf,l143);l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,l8,ENTRY_TYPE0f);soc_mem_field32_set(l22,
L3_DEFIP_AUX_SCRATCHm,l14,ENTRY_TYPEf,l12);l143 = soc_mem_field32_get(l22,
L3_DEFIPm,l8,GLOBAL_ROUTE0f);soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,
l14,GLOBAL_ROUTEf,l143);if(soc_feature(l22,soc_feature_ipmc_defip)){l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,l8,RPA_ID0f);soc_mem_field32_set(l22,
L3_DEFIP_AUX_SCRATCHm,l14,RPA_IDf,l143);l143 = soc_mem_field32_get(l22,
L3_DEFIPm,l8,EXPECTED_L3_IIF0f);soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm
,l14,EXPECTED_L3_IIFf,l143);l143 = soc_mem_field32_get(l22,L3_DEFIPm,l8,
IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);soc_mem_field32_set(l22,
L3_DEFIP_AUX_SCRATCHm,l14,IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf,l143);l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,l8,IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,
IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf,l143);l143 = soc_mem_field32_get(l22,
L3_DEFIPm,l8,L3MC_INDEX0f);soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,
L3MC_INDEXf,l143);}l143 = soc_mem_field32_get(l22,L3_DEFIPm,l8,ECMP0f);
soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,ECMPf,l143);l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,l8,ECMP_PTR0f);soc_mem_field32_set(l22,
L3_DEFIP_AUX_SCRATCHm,l14,ECMP_PTRf,l143);l143 = soc_mem_field32_get(l22,
L3_DEFIPm,l8,NEXT_HOP_INDEX0f);soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,
l14,NEXT_HOP_INDEXf,l143);l143 = soc_mem_field32_get(l22,L3_DEFIPm,l8,PRI0f);
soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,PRIf,l143);l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,l8,RPE0f);soc_mem_field32_set(l22,
L3_DEFIP_AUX_SCRATCHm,l14,RPEf,l143);l143 =soc_mem_field32_get(l22,L3_DEFIPm,
l8,VRF_ID_0f);soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,VRFf,l143);
soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,DB_TYPEf,l11);l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,l8,DST_DISCARD0f);soc_mem_field32_set(l22,
L3_DEFIP_AUX_SCRATCHm,l14,DST_DISCARDf,l143);l143 = soc_mem_field32_get(l22,
L3_DEFIPm,l8,CLASS_ID0f);soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,
CLASS_IDf,l143);if(l10){l379[2] = soc_mem_field32_get(l22,L3_DEFIPm,l8,
IP_ADDR0f);l379[3] = soc_mem_field32_get(l22,L3_DEFIPm,l8,IP_ADDR1f);}else{
l379[0] = soc_mem_field32_get(l22,L3_DEFIPm,l8,IP_ADDR0f);}soc_mem_field_set(
l22,L3_DEFIP_AUX_SCRATCHm,(uint32*)l14,IP_ADDRf,(uint32*)l379);if(l10){l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l142 = _ipmask2pfx(
l143,&l133))<0){return(l142);}l143 = soc_mem_field32_get(l22,L3_DEFIPm,l8,
IP_ADDR_MASK1f);if(l133){if(l143!= 0xffffffff){return(SOC_E_PARAM);}l133+= 32
;}else{if((l142 = _ipmask2pfx(l143,&l133))<0){return(l142);}}}else{l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l142 = _ipmask2pfx(
l143,&l133))<0){return(l142);}}soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,
l14,IP_LENGTHf,l133);soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,l14,
REPLACE_LENf,l13);return(SOC_E_NONE);}int l380(int l1,int l381,int*index){
char*l382 = NULL;soc_mem_t l25 = L3_DEFIP_AUX_TABLEm;int l383,l384;uint32 l385
;int l386 = soc_mem_index_count(l1,l25);int l387 = sizeof(uint32)*
soc_mem_entry_words(l1,l25);l383 = soc_mem_index_min(l1,l25);l384 = 
soc_mem_index_max(l1,l25);l382 = soc_cm_salloc(l1,l386*l387,"auxtbl_dmabuf");
if(l382 == NULL){return SOC_E_MEMORY;}switch(l381){case 0:l385 = 
_SOC_MEM_ADDR_ACC_TYPE_PIPE_X;break;case 1:l385 = 
_SOC_MEM_ADDR_ACC_TYPE_PIPE_Y;break;default:l385 = 
_SOC_MEM_ADDR_ACC_TYPE_PIPE_X;break;}l385|= _SOC_SER_FLAG_DMA_ERR_RETURN;l385
|= _SOC_SER_FLAG_DISCARD_READ;l385|= _SOC_SER_FLAG_MULTI_PIPE;while(l383<= 
l384){int l388 = l383+(l384-l383)/2;if(soc_mem_ser_read_range(l1,l25,
MEM_BLOCK_ALL,l383,l388,l385,l382)<0){if(l384 == l383){*index = l383;break;}
l384 = l388;}else{l383 = l388+1;}}soc_cm_sfree(l1,l382);if(l383>l384){
LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"No corrupted index in AUX Table Pipe %d\n"),l381));return SOC_E_NOT_FOUND;}
else{LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found corrupted Index %d in AUX Table Pipe %d\n"),*index,l381));return
SOC_E_NONE;}}int _soc_alpm_aux_op(int l1,_soc_aux_op_t l389,
defip_aux_scratch_entry_t*l14,int l390,int*l163,int*tcam_index,int*
bucket_index){soc_timeout_t l391;int l381,l142 = SOC_E_NONE;int l392 = 0;
uint32 l375[2];int l393[2];int l394[2];uint32 l395;int l396;uint8 l397;if(
l390){SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_SCRATCHm(l1,MEM_BLOCK_ANY,0,l14)
);}l398:switch(l389){case INSERT_PROPAGATE:l393[0] = 0;break;case
DELETE_PROPAGATE:l393[0] = 1;break;case PREFIX_LOOKUP:l393[0] = 2;break;case
HITBIT_REPLACE:l393[0] = 3;break;default:return SOC_E_PARAM;}l375[0] = 0;
soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l375[0],OPCODEf,l393[0]);
soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l375[0],STARTf,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l375[0]));soc_timeout_init(&
l391,50000,5);sal_memset(l393,0,sizeof(l393));sal_memset(l394,0,sizeof(l394))
;l395 = soc_reg_addr_get(l1,L3_DEFIP_AUX_CTRLr,REG_PORT_ANY,0,
SOC_REG_ADDR_OPTION_NONE,&l396,&l397);do{for(l381 = 0;l381<2;l381++){
SOC_IF_ERROR_RETURN(_soc_reg32_get(l1,l396,l381+1,l395,&l375[l381]));l393[
l381] = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l375[l381],DONEf);l394[l381] = 
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l375[l381],ERRORf);if(l393[l381]!= 1)
{break;}}if(l381>= 2){l142 = SOC_E_NONE;break;}if(soc_timeout_check(&l391)){
sal_memset(l393,0,sizeof(l393));sal_memset(l394,0,sizeof(l394));for(l381 = 0;
l381<2;l381++){SOC_IF_ERROR_RETURN(_soc_reg32_get(l1,l396,l381+1,l395,&l375[
l381]));l393[l381] = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l375[l381],DONEf
);l394[l381] = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l375[l381],ERRORf);if(
l393[l381]!= 1){break;}}if(l381>= 2){l142 = SOC_E_NONE;}else{LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d : DEFIP AUX Operation "
"timeout, Pipe %d\n"),l1,l381));l142 = SOC_E_TIMEOUT;}break;}}while(1);if(
SOC_SUCCESS(l142)){for(l381 = 0;l381<2;l381++){if(l394[l381]){
soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l375[l381],STARTf,0);
soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l375[l381],ERRORf,0);
soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l375[l381],DONEf,0);
SOC_IF_ERROR_RETURN(_soc_reg32_set(l1,l396,l381+1,l395,l375[l381]));LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"DEFIP AUX Operation encountered "
"parity error in Pipe %d!!\n"),l381));l392++;if(SOC_CONTROL(l1)->
alpm_bulk_retry){sal_sem_take(SOC_CONTROL(l1)->alpm_bulk_retry,1000000);}if(
l392<100){int l399;if(l380(l1,l381,&l399)>= 0){l142 = 
soc_mem_alpm_aux_table_correction(l1,l381,l399);if(l142<0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_aux_error_index "
"pipe %d index %d failed\n"),l381,l399));}}LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Retry DEFIP AUX Operation in ""Pipe %d.\n"),l381));goto l398;}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l1));return SOC_E_INTERNAL;}}}if(l389 == PREFIX_LOOKUP){if(l163&&tcam_index){
*l163 = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l375[0],HITf);*tcam_index = 
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l375[0],BKT_INDEXf);
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRL_1r(l1,&l375[1]));*bucket_index = 
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRL_1r,l375[1],BKT_PTRf);}}}return l142;}
static int l21(int l22,void*lpm_entry,void*l23,void*l24,soc_mem_t l25,uint32
l26,uint32*l400){uint32 l143;uint32 l379[4] = {0,0};int l133 = 0;int l142 = 
SOC_E_NONE;int l10;uint32 l27 = 0;l10 = soc_mem_field32_get(l22,L3_DEFIPm,
lpm_entry,MODE0f);sal_memset(l23,0,soc_mem_entry_words(l22,l25)*4);l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,VALID0f);soc_mem_field32_set(l22,
l25,l23,VALIDf,l143);l143 = soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,
ECMP0f);soc_mem_field32_set(l22,l25,l23,ECMPf,l143);l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set(
l22,l25,l23,ECMP_PTRf,l143);l143 = soc_mem_field32_get(l22,L3_DEFIPm,
lpm_entry,NEXT_HOP_INDEX0f);soc_mem_field32_set(l22,l25,l23,NEXT_HOP_INDEXf,
l143);l143 = soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,PRI0f);
soc_mem_field32_set(l22,l25,l23,PRIf,l143);l143 = soc_mem_field32_get(l22,
L3_DEFIPm,lpm_entry,RPE0f);soc_mem_field32_set(l22,l25,l23,RPEf,l143);l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(l22,l25,l23,DST_DISCARDf,l143);l143 = soc_mem_field32_get
(l22,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);soc_mem_field32_set(l22,l25,l23,
SRC_DISCARDf,l143);l143 = soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,
CLASS_ID0f);soc_mem_field32_set(l22,l25,l23,CLASS_IDf,l143);l379[0] = 
soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,IP_ADDR0f);if(l10){l379[1] = 
soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,IP_ADDR1f);}soc_mem_field_set(l22
,l25,(uint32*)l23,KEYf,(uint32*)l379);if(l10){l143 = soc_mem_field32_get(l22,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l142 = _ipmask2pfx(l143,&l133))<0){
return(l142);}l143 = soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f);if(l133){if(l143!= 0xffffffff){return(SOC_E_PARAM);}l133+= 32
;}else{if((l142 = _ipmask2pfx(l143,&l133))<0){return(l142);}}}else{l143 = 
soc_mem_field32_get(l22,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l142 = 
_ipmask2pfx(l143,&l133))<0){return(l142);}}if((l133 == 0)&&(l379[0] == 0)&&(
l379[1] == 0)){l27 = 1;}if(l400!= NULL){*l400 = l27;}soc_mem_field32_set(l22,
l25,l23,LENGTHf,l133);if(l24 == NULL){return(SOC_E_NONE);}if(
SOC_URPF_STATUS_GET(l22)){sal_memset(l24,0,soc_mem_entry_words(l22,l25)*4);
sal_memcpy(l24,l23,soc_mem_entry_words(l22,l25)*4);soc_mem_field32_set(l22,
l25,l24,DST_DISCARDf,0);soc_mem_field32_set(l22,l25,l24,RPEf,0);
soc_mem_field32_set(l22,l25,l24,SRC_DISCARDf,l26&SOC_ALPM_RPF_SRC_DISCARD);
soc_mem_field32_set(l22,l25,l24,DEFAULTROUTEf,l27);}return(SOC_E_NONE);}
static int l28(int l22,void*l23,soc_mem_t l25,int l10,int l29,int l30,int
index,void*lpm_entry){uint32 l143;uint32 l379[4] = {0,0};uint32 l133 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_words(l22,L3_DEFIPm)*4);l143 = 
soc_mem_field32_get(l22,l25,l23,VALIDf);soc_mem_field32_set(l22,L3_DEFIPm,
lpm_entry,VALID0f,l143);if(l10){soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,
VALID1f,l143);}l143 = soc_mem_field32_get(l22,l25,l23,ECMPf);
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,ECMP0f,l143);l143 = 
soc_mem_field32_get(l22,l25,l23,ECMP_PTRf);soc_mem_field32_set(l22,L3_DEFIPm,
lpm_entry,ECMP_PTR0f,l143);l143 = soc_mem_field32_get(l22,l25,l23,
NEXT_HOP_INDEXf);soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f
,l143);l143 = soc_mem_field32_get(l22,l25,l23,PRIf);soc_mem_field32_set(l22,
L3_DEFIPm,lpm_entry,PRI0f,l143);l143 = soc_mem_field32_get(l22,l25,l23,RPEf);
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,RPE0f,l143);l143 = 
soc_mem_field32_get(l22,l25,l23,DST_DISCARDf);soc_mem_field32_set(l22,
L3_DEFIPm,lpm_entry,DST_DISCARD0f,l143);l143 = soc_mem_field32_get(l22,l25,
l23,SRC_DISCARDf);soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,SRC_DISCARD0f,
l143);l143 = soc_mem_field32_get(l22,l25,l23,CLASS_IDf);soc_mem_field32_set(
l22,L3_DEFIPm,lpm_entry,CLASS_ID0f,l143);soc_mem_field32_set(l22,L3_DEFIPm,
lpm_entry,ALG_BKT_PTR0f,l30);soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,
ALG_HIT_IDX0f,index);soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,MODE_MASK0f,
3);soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l10){
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,MODE0f,1);soc_mem_field32_set(l22
,L3_DEFIPm,lpm_entry,MODE1f,1);}soc_mem_field_get(l22,l25,l23,KEYf,l379);if(
l10){soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,IP_ADDR1f,l379[1]);}
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,IP_ADDR0f,l379[0]);l143 = 
soc_mem_field32_get(l22,l25,l23,LENGTHf);if(l10){if(l143>= 32){l133 = 
0xffffffff;soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f,l133);
l133 = ~(((l143-32) == 32)?0:(0xffffffff)>>(l143-32));soc_mem_field32_set(l22
,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l133);}else{l133 = ~(0xffffffff>>l143);
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f,l133);}}else{
assert(l143<= 32);l133 = ~(((l143) == 32)?0:(0xffffffff)>>(l143));
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l133);}if(l29 == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,
GLOBAL_HIGH0f,1);soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,GLOBAL_ROUTE0f,1
);soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,VRF_ID_0f,0);
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}else if(l29 == 
SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,GLOBAL_ROUTE0f
,1);soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,VRF_ID_0f,0);
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,VRF_ID_0f,l29);
soc_mem_field32_set(l22,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(l22));}
return(SOC_E_NONE);}int soc_alpm_warmboot_pivot_add(int l22,int l10,void*
lpm_entry,int l401,int l402){int l142 = SOC_E_NONE;uint32 key[2] = {0,0};
alpm_pivot_t*l217 = NULL;alpm_bucket_handle_t*l226 = NULL;int l169 = 0,l29 = 
0;uint32 l403;trie_t*l276 = NULL;uint32 prefix[5] = {0};int l27 = 0;l142 = 
l155(l22,lpm_entry,prefix,&l403,&l27);SOC_IF_ERROR_RETURN(l142);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l22,lpm_entry,&l169,&l29));l401 = 
soc_alpm_physical_idx(l22,L3_DEFIPm,l401,l10);l226 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l226 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l22,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l226,0,sizeof(*l226));
l217 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l217 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l22,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l226);return SOC_E_MEMORY;}sal_memset(l217,0
,sizeof(*l217));PIVOT_BUCKET_HANDLE(l217) = l226;if(l10){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l217));key[0] = 
soc_L3_DEFIPm_field32_get(l22,lpm_entry,IP_ADDR0f);key[1] = 
soc_L3_DEFIPm_field32_get(l22,lpm_entry,IP_ADDR1f);}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l217));key[0] = soc_L3_DEFIPm_field32_get
(l22,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l217) = l402;PIVOT_TCAM_INDEX(
l217) = l401;if(l169!= SOC_L3_VRF_OVERRIDE){if(l10 == 0){l276 = 
VRF_PIVOT_TRIE_IPV4(l22,l29);if(l276 == NULL){trie_init(_MAX_KEY_LEN_48_,&
VRF_PIVOT_TRIE_IPV4(l22,l29));l276 = VRF_PIVOT_TRIE_IPV4(l22,l29);}}else{l276
= VRF_PIVOT_TRIE_IPV6(l22,l29);if(l276 == NULL){trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l22,l29));l276 = VRF_PIVOT_TRIE_IPV6(l22,l29);}}
sal_memcpy(l217->key,prefix,sizeof(prefix));l217->len = l403;l142 = 
trie_insert(l276,l217->key,NULL,l217->len,(trie_node_t*)l217);if(SOC_FAILURE(
l142)){sal_free(l226);sal_free(l217);return l142;}}ALPM_TCAM_PIVOT(l22,l401) = 
l217;PIVOT_BUCKET_VRF(l217) = l29;PIVOT_BUCKET_IPV6(l217) = l10;
PIVOT_BUCKET_ENT_CNT_UPDATE(l217);if(key[0] == 0&&key[1] == 0){
PIVOT_BUCKET_DEF(l217) = TRUE;}VRF_PIVOT_REF_INC(l22,l29,l10);return l142;}
static int l404(int l22,int l10,void*lpm_entry,void*l23,soc_mem_t l25,int l401
,int l402,int l405){int l406;int l29;int l142 = SOC_E_NONE;int l27 = 0;uint32
prefix[5] = {0,0,0,0,0};uint32 l162;void*l407 = NULL;trie_t*l408 = NULL;
trie_t*l220 = NULL;trie_node_t*l222 = NULL;payload_t*l409 = NULL;payload_t*
l224 = NULL;alpm_pivot_t*l166 = NULL;if((NULL == lpm_entry)||(NULL == l23)){
return SOC_E_PARAM;}if(l10){if(!(l10 = soc_mem_field32_get(l22,L3_DEFIPm,
lpm_entry,MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l22,lpm_entry,&l406,&l29));l25 = (l10)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l407 = sal_alloc(sizeof(
defip_entry_t),"Temp lpm_entr");if(NULL == l407){return SOC_E_MEMORY;}
SOC_IF_ERROR_RETURN(l28(l22,l23,l25,l10,l406,l402,l401,l407));l142 = l155(l22
,l407,prefix,&l162,&l27);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l22,"prefix create failed\n")));return l142;}sal_free(l407);l166 = 
ALPM_TCAM_PIVOT(l22,l401);l408 = PIVOT_BUCKET_TRIE(l166);l409 = sal_alloc(
sizeof(payload_t),"Payload for Key");if(NULL == l409){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l22,"Unable to allocate memory for trie node.\n")
));return SOC_E_MEMORY;}l224 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(NULL == l224){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l22,"Unable to allocate memory for pfx trie node\n")));sal_free(
l409);return SOC_E_MEMORY;}sal_memset(l409,0,sizeof(*l409));sal_memset(l224,0
,sizeof(*l224));l409->key[0] = prefix[0];l409->key[1] = prefix[1];l409->key[2
] = prefix[2];l409->key[3] = prefix[3];l409->key[4] = prefix[4];l409->len = 
l162;l409->index = l405;sal_memcpy(l224,l409,sizeof(*l409));l142 = 
trie_insert(l408,prefix,NULL,l162,(trie_node_t*)l409);if(SOC_FAILURE(l142)){
goto l410;}if(l10){l220 = VRF_PREFIX_TRIE_IPV6(l22,l29);}else{l220 = 
VRF_PREFIX_TRIE_IPV4(l22,l29);}if(!l27){l142 = trie_insert(l220,prefix,NULL,
l162,(trie_node_t*)l224);if(SOC_FAILURE(l142)){goto l245;}}return l142;l245:(
void)trie_delete(l408,prefix,l162,&l222);l409 = (payload_t*)l222;l410:
sal_free(l409);sal_free(l224);return l142;}static int l411(int l22,int l35,
int l29,int l149,int bkt_ptr){int l142 = SOC_E_NONE;uint32 l162;uint32 key[2]
= {0,0};trie_t*l412 = NULL;payload_t*l260 = NULL;defip_entry_t*lpm_entry = 
NULL;lpm_entry = sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(
lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l22,
"unable to allocate memory for LPM entry\n")));return SOC_E_MEMORY;}(void)l31
(l22,key,0,l29,l35,lpm_entry,0,1);if(l29 == SOC_VRF_MAX(l22)+1){
soc_L3_DEFIPm_field32_set(l22,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l22,lpm_entry,DEFAULT_MISS0f,1);}
soc_L3_DEFIPm_field32_set(l22,lpm_entry,ALG_BKT_PTR0f,bkt_ptr);if(l35 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l22,l29) = lpm_entry;trie_init(_MAX_KEY_LEN_48_,&
VRF_PREFIX_TRIE_IPV4(l22,l29));l412 = VRF_PREFIX_TRIE_IPV4(l22,l29);}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l22,l29) = lpm_entry;trie_init(_MAX_KEY_LEN_144_,
&VRF_PREFIX_TRIE_IPV6(l22,l29));l412 = VRF_PREFIX_TRIE_IPV6(l22,l29);}l260 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(l260 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l22,
"Unable to allocate memory for pfx trie node \n")));return SOC_E_MEMORY;}
sal_memset(l260,0,sizeof(*l260));l162 = 0;l260->key[0] = key[0];l260->key[1] = 
key[1];l260->len = l162;l142 = trie_insert(l412,key,NULL,l162,&(l260->node));
if(SOC_FAILURE(l142)){sal_free(l260);return l142;}VRF_TRIE_INIT_DONE(l22,l29,
l35,1);return l142;}int soc_alpm_warmboot_prefix_insert(int l22,int l10,void*
lpm_entry,void*l23,int l401,int l402,int l405){int l406;int l29;int l142 = 
SOC_E_NONE;soc_mem_t l25;l401 = soc_alpm_physical_idx(l22,L3_DEFIPm,l401,l10)
;l25 = (l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l22,lpm_entry,&l406,&l29));if(l406 == 
SOC_L3_VRF_OVERRIDE){return(l142);}if(!VRF_TRIE_INIT_COMPLETED(l22,l29,l10)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l22,"VRF %d is not initialized\n"),
l29));l142 = l411(l22,l10,l29,l401,l402);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l22,"VRF %d/%d trie init \n""failed\n"),l29,l10))
;return l142;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l22,
"VRF %d/%d trie init completed\n"),l29,l10));}l142 = l404(l22,l10,lpm_entry,
l23,l25,l401,l402,l405);if(l142!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l22,"unit %d : Route Insertion Failed :%s\n"),l22,soc_errmsg(l142)
));return(l142);}VRF_TRIE_ROUTES_INC(l22,l29,l10);return(l142);}int
soc_alpm_warmboot_bucket_bitmap_set(int l1,int l35,int l282){int l315 = 1;if(
l35){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l315 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l282,l315);return SOC_E_NONE;}int
soc_alpm_warmboot_lpm_reinit_done(int l22){int l149;int l413 = ((3*(64+32+2+1
))-1);int l314 = soc_mem_index_count(l22,L3_DEFIPm);if(SOC_URPF_STATUS_GET(
l22)){l314>>= 1;}if(!soc_alpm_mode_get(l22)){(l45[(l22)][(((3*(64+32+2+1))-1)
)].l40) = -1;for(l149 = ((3*(64+32+2+1))-1);l149>-1;l149--){if(-1 == (l45[(
l22)][(l149)].l38)){continue;}(l45[(l22)][(l149)].l40) = l413;(l45[(l22)][(
l413)].next) = l149;(l45[(l22)][(l413)].l42) = (l45[(l22)][(l149)].l38)-(l45[
(l22)][(l413)].l39)-1;l413 = l149;}(l45[(l22)][(l413)].next) = -1;(l45[(l22)]
[(l413)].l42) = l314-(l45[(l22)][(l413)].l39)-1;}else{(l45[(l22)][(((3*(64+32
+2+1))-1))].l40) = -1;for(l149 = ((3*(64+32+2+1))-1);l149>(((3*(64+32+2+1))-1
)/3);l149--){if(-1 == (l45[(l22)][(l149)].l38)){continue;}(l45[(l22)][(l149)]
.l40) = l413;(l45[(l22)][(l413)].next) = l149;(l45[(l22)][(l413)].l42) = (l45
[(l22)][(l149)].l38)-(l45[(l22)][(l413)].l39)-1;l413 = l149;}(l45[(l22)][(
l413)].next) = -1;(l45[(l22)][(l413)].l42) = l314-(l45[(l22)][(l413)].l39)-1;
l413 = (((3*(64+32+2+1))-1)/3);(l45[(l22)][((((3*(64+32+2+1))-1)/3))].l40) = 
-1;for(l149 = ((((3*(64+32+2+1))-1)/3)-1);l149>-1;l149--){if(-1 == (l45[(l22)
][(l149)].l38)){continue;}(l45[(l22)][(l149)].l40) = l413;(l45[(l22)][(l413)]
.next) = l149;(l45[(l22)][(l413)].l42) = (l45[(l22)][(l149)].l38)-(l45[(l22)]
[(l413)].l39)-1;l413 = l149;}(l45[(l22)][(l413)].next) = -1;(l45[(l22)][(l413
)].l42) = (l314>>1)-(l45[(l22)][(l413)].l39)-1;}return(SOC_E_NONE);}int
soc_alpm_warmboot_lpm_reinit(int l22,int l10,int l149,void*lpm_entry){int l18
;defip_entry_t*l414;if(soc_L3_DEFIPm_field32_get(l22,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(l22,lpm_entry,VALID1f)){l321(l22,lpm_entry,l149,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(l22,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l36(l22,lpm_entry,&l18));if((l45[(l22)][(l18)].l41) == 0)
{(l45[(l22)][(l18)].l38) = l149;(l45[(l22)][(l18)].l39) = l149;}else{(l45[(
l22)][(l18)].l39) = l149;}(l45[(l22)][(l18)].l41)++;if(l10){return(SOC_E_NONE
);}}else{if(soc_L3_DEFIPm_field32_get(l22,lpm_entry,VALID1f)){l414 = 
sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");soc_alpm_lpm_ip4entry1_to_0(
l22,lpm_entry,l414,TRUE);SOC_IF_ERROR_RETURN(l36(l22,l414,&l18));if((l45[(l22
)][(l18)].l41) == 0){(l45[(l22)][(l18)].l38) = l149;(l45[(l22)][(l18)].l39) = 
l149;}else{(l45[(l22)][(l18)].l39) = l149;}sal_free(l414);(l45[(l22)][(l18)].
l41)++;}}return(SOC_E_NONE);}typedef struct l415{int v4;int v6_64;int v6_128;
int l416;int l417;int l418;int l316;}l419;typedef enum l420{l421 = 0,l422,
l423,l424,l425,l426}l427;static void l428(int l1,alpm_vrf_counter_t*l429){
l429->v4 = soc_mem_index_count(l1,L3_DEFIPm)*2;l429->v6_128 = 
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){l429->v6_64 = l429->v6_128;}else{l429->v6_64 = 
l429->v4>>1;}if(SOC_URPF_STATUS_GET(l1)){l429->v4>>= 1;l429->v6_128>>= 1;l429
->v6_64>>= 1;}}static void l430(int l1,int l169,alpm_vrf_handle_t*l431,l427
l432){alpm_vrf_counter_t*l433;int l191,l434,l435,l436;int l378 = 0;
alpm_vrf_counter_t l429;switch(l432){case l421:LOG_CLI((BSL_META_U(l1,
"\nAdd Counter:\n")));break;case l422:LOG_CLI((BSL_META_U(l1,
"\nDelete Counter:\n")));break;case l423:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - 1:\n")));break;case l424:l428(l1,&l429);LOG_CLI((
BSL_META_U(l1,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l429.v4,
l429.v6_64,l429.v6_128));break;case l425:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l426:LOG_CLI((
BSL_META_U(l1,"\nInternal Debug Counter - LPM Full:\n")));break;default:break
;}LOG_CLI((BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n"))
);LOG_CLI((BSL_META_U(l1,"-----------------------------------------------\n")
));l434 = l435 = l436 = 0;for(l191 = 0;l191<MAX_VRF_ID+1;l191++){int l437,
l438,l439;if(l431[l191].init_done == 0&&l191!= MAX_VRF_ID){continue;}if(l169
!= -1&&l169!= l191){continue;}l378 = 1;switch(l432){case l421:l433 = &l431[
l191].add;break;case l422:l433 = &l431[l191].del;break;case l423:l433 = &l431
[l191].bkt_split;break;case l425:l433 = &l431[l191].lpm_shift;break;case l426
:l433 = &l431[l191].lpm_full;break;case l424:l433 = &l431[l191].pivot_used;
break;default:l433 = &l431[l191].pivot_used;break;}l437 = l433->v4;l438 = 
l433->v6_64;l439 = l433->v6_128;l434+= l437;l435+= l438;l436+= l439;do{
LOG_CLI((BSL_META_U(l1,"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l191 == 
MAX_VRF_ID?-1:l191),(l437),(l438),(l439),((l437+l438+l439)),(l191) == 
MAX_VRF_ID?"GHi":(l191) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l378 == 
0){LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));do{
LOG_CLI((BSL_META_U(l1,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l434),(
l435),(l436),((l434+l435+l436))));}while(0);}return;}int soc_alpm_debug_show(
int l1,int l169,uint32 flags){int l191,l440,l378 = 0;l419*l441;l419 l442;l419
l443;if(l169>(SOC_VRF_MAX(l1)+1)){return SOC_E_PARAM;}l440 = MAX_VRF_ID*
sizeof(l419);l441 = sal_alloc(l440,"_alpm_dbg_cnt");if(l441 == NULL){return
SOC_E_MEMORY;}sal_memset(l441,0,l440);l442.v4 = ALPM_IPV4_BKT_COUNT;l442.
v6_64 = ALPM_IPV6_64_BKT_COUNT;l442.v6_128 = ALPM_IPV6_128_BKT_COUNT;if(!
soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l442.v6_64<<= 1;l442.v6_128
<<= 1;}LOG_CLI((BSL_META_U(l1,"\nBucket Occupancy:\n")));if(flags&(
SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l1,
"\n  BKT/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));}for(
l191 = 0;l191<MAX_PIVOT_COUNT;l191++){alpm_pivot_t*l444 = ALPM_TCAM_PIVOT(l1,
l191);if(l444!= NULL){l419*l445;int l29 = PIVOT_BUCKET_VRF(l444);if(l29<0||
l29>(SOC_VRF_MAX(l1)+1)){continue;}if(l169!= -1&&l169!= l29){continue;}if(
flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l378 = 1;do{LOG_CLI((BSL_META_U(l1,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l191,l29,PIVOT_BUCKET_MIN(l444),
PIVOT_BUCKET_MAX(l444),PIVOT_BUCKET_COUNT(l444),PIVOT_BUCKET_DEF(l444)?"Def":
(l29) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}l445 = &l441[l29];if(
PIVOT_BUCKET_IPV6(l444) == L3_DEFIP_MODE_128){l445->v6_128+= 
PIVOT_BUCKET_COUNT(l444);l445->l418+= l442.v6_128;}else if(PIVOT_BUCKET_IPV6(
l444) == L3_DEFIP_MODE_64){l445->v6_64+= PIVOT_BUCKET_COUNT(l444);l445->l417
+= l442.v6_64;}else{l445->v4+= PIVOT_BUCKET_COUNT(l444);l445->l416+= l442.v4;
}l445->l316 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l378 == 0){
LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}}sal_memset(&l443
,0,sizeof(l443));l378 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){LOG_CLI((
BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));for(l191
= 0;l191<MAX_VRF_ID;l191++){l419*l445;if(l441[l191].l316!= TRUE){continue;}if
(l169!= -1&&l169!= l191){continue;}l378 = 1;l445 = &l441[l191];do{(&l443)->v4
+= (l445)->v4;(&l443)->l416+= (l445)->l416;(&l443)->v6_64+= (l445)->v6_64;(&
l443)->l417+= (l445)->l417;(&l443)->v6_128+= (l445)->v6_128;(&l443)->l418+= (
l445)->l418;}while(0);do{LOG_CLI((BSL_META_U(l1,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l191),(l445
->l416)?(l445->v4)*100/(l445->l416):0,(l445->l416)?(l445->v4)*1000/(l445->
l416)%10:0,(l445->l417)?(l445->v6_64)*100/(l445->l417):0,(l445->l417)?(l445->
v6_64)*1000/(l445->l417)%10:0,(l445->l418)?(l445->v6_128)*100/(l445->l418):0,
(l445->l418)?(l445->v6_128)*1000/(l445->l418)%10:0,((l445->l416+l445->l417+
l445->l418))?((l445->v4+l445->v6_64+l445->v6_128))*100/((l445->l416+l445->
l417+l445->l418)):0,((l445->l416+l445->l417+l445->l418))?((l445->v4+l445->
v6_64+l445->v6_128))*1000/((l445->l416+l445->l417+l445->l418))%10:0,(l191) == 
SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l378 == 0){LOG_CLI((BSL_META_U(l1,
"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l1,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l1,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l443)->l416)?((&l443)->v4)*100/((&l443)->l416):0,((&l443)->l416)?((&l443)->v4
)*1000/((&l443)->l416)%10:0,((&l443)->l417)?((&l443)->v6_64)*100/((&l443)->
l417):0,((&l443)->l417)?((&l443)->v6_64)*1000/((&l443)->l417)%10:0,((&l443)->
l418)?((&l443)->v6_128)*100/((&l443)->l418):0,((&l443)->l418)?((&l443)->
v6_128)*1000/((&l443)->l418)%10:0,(((&l443)->l416+(&l443)->l417+(&l443)->l418
))?(((&l443)->v4+(&l443)->v6_64+(&l443)->v6_128))*100/(((&l443)->l416+(&l443)
->l417+(&l443)->l418)):0,(((&l443)->l416+(&l443)->l417+(&l443)->l418))?(((&
l443)->v4+(&l443)->v6_64+(&l443)->v6_128))*1000/(((&l443)->l416+(&l443)->l417
+(&l443)->l418))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l430(l1,l169,alpm_vrf_handle[l1],l424);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_CNT
){l430(l1,l169,alpm_vrf_handle[l1],l421);l430(l1,l169,alpm_vrf_handle[l1],
l422);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l430(l1,l169,alpm_vrf_handle
[l1],l423);l430(l1,l169,alpm_vrf_handle[l1],l426);l430(l1,l169,
alpm_vrf_handle[l1],l425);}sal_free(l441);return SOC_E_NONE;}STATIC int
_soc_alpm_sanity_check(int l1,int l383,int l384,int l446,int*l447){int l142 = 
SOC_E_NONE;int l448,l449,l450,l405,l413 = 0;int l191,l288,l10,l451 = 0,l452 = 
0;int l169,l29;int l453,l454,l455;int l145,l294,bkt_ptr;int l456 = 0,l457 = 0
,l458 = 0,l459 = 0;int l316;int l176,l460;int l461,l27;int l462;uint32 l159 = 
0;uint32 l16[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l179[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 prefix[5],l162;trie_t*l220 = NULL;
trie_node_t*l165 = NULL;payload_t*l225 = NULL;uint32 l463[10] = {0};uint32
l464[10] = {0};char*l465 = NULL;defip_entry_t*lpm_entry,l466,l467;soc_mem_t
l468;soc_mem_t l469;l462 = sizeof(defip_entry_t)*(l384-l383+1);l465 = 
soc_cm_salloc(l1,l462,"lpm_tbl");if(l465 == NULL){return(SOC_E_MEMORY);}
sal_memset(l465,0,l462);SOC_ALPM_LPM_LOCK(l1);if(soc_mem_read_range(l1,
L3_DEFIPm,MEM_BLOCK_ANY,l383,l384,l465)<0){soc_cm_sfree(l1,l465);
SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_INTERNAL);}for(l448 = l383;l448<= l384;
l448++){lpm_entry = soc_mem_table_idx_to_pointer(l1,L3_DEFIPm,defip_entry_t*,
l465,l448-l383);l10 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l77));if(l10){l469 = 
L3_DEFIP_ALPM_IPV6_64m;if(SOC_ALPM_V6_SCALE_CHECK(l1,l10)){l294 = 32;}else{
l294 = 16;}}else{l469 = L3_DEFIP_ALPM_IPV4m;l294 = 24;}for(l191 = 0;l191<= (
l10?0:1);l191++){if(l456){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP index:[%d,%d] check failed. ipv6 %d error count %d\n"),l413,l452,
l451,l456));l458 = 1;}else if(l459){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP index:[%d,%d] check passed. ipv6 %d success count %d\n"),l413,
l452,l451,l459));}l456 = l457 = l459 = 0;l451 = l10;l413 = l448;l452 = l191;
if(l191 == 1){soc_alpm_lpm_ip4entry1_to_0(l1,lpm_entry,lpm_entry,PRESERVE_HIT
);}bkt_ptr = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(lpm_entry),(l109[(l1)]->l97));if(!soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l87))){continue;}if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(
l109[(l1)]->l93))){if(bkt_ptr!= 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tWrong bkt_ptr %d\n"),bkt_ptr));l456++;}continue;}if(bkt_ptr == 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tWrong bkt_ptr2 %d\n"),bkt_ptr));
l456++;}l288 = 0;l463[l288++] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l51));l463[l288++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(
l109[(l1)]->l55));l463[l288++] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l81));l463[l288++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(
l109[(l1)]->l83));l463[l288++] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l85));l463[l288++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(
l109[(l1)]->l49));l463[l288++] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l47));if(l447){if(l447[
bkt_ptr] == -1){l447[bkt_ptr] = l448;if(SOC_ALPM_V6_SCALE_CHECK(l1,l10)){l447
[bkt_ptr+1] = l448;}l459++;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tConflict bucket pointer %d: ""was %d now %d\n"),bkt_ptr,l447[bkt_ptr],l448
));l456++;}}l449 = soc_alpm_physical_idx(l1,L3_DEFIPm,l448,1);l142 = 
soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l449,l179);l176 = -1;if(
SOC_SUCCESS(l142)){if(l191 == 0){l176 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l179,BPM_LENGTH0f);l460 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l179,IP_LENGTH0f);}else{l176 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l179,BPM_LENGTH1f);l460 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l179,IP_LENGTH1f);}if(l176>l460){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tAUX Table corrupted\n")));l456++;}else{l459
++;}if(l176 == 0&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(lpm_entry),(l109[(l1)]->l99))){continue;}sal_memcpy(&l467,
lpm_entry,sizeof(defip_entry_t));l153(l1,&l467,l176,l10);l453 = l454 = l455 = 
-1;l142 = l167(l1,&l467,l469,l16,&l453,&l454,&l455,FALSE);if(SOC_SUCCESS(l142
)){l288 = 0;l464[l288++] = soc_mem_field32_get(l1,l469,l16,ECMPf);l464[l288++
] = soc_mem_field32_get(l1,l469,l16,ECMP_PTRf);l464[l288++] = 
soc_mem_field32_get(l1,l469,l16,NEXT_HOP_INDEXf);l464[l288++] = 
soc_mem_field32_get(l1,l469,l16,PRIf);l464[l288++] = soc_mem_field32_get(l1,
l469,l16,RPEf);l464[l288++] = soc_mem_field32_get(l1,l469,l16,DST_DISCARDf);
l464[l288++] = soc_mem_field32_get(l1,l469,l16,CLASS_IDf);if(sal_memcmp(l463,
l464,sizeof(l463))!= 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tData mismatch: ""lpm_idx %d find_lpm_idx %d "
"find_alpm_idx %d find_bkt_ptr %d\n"),l448,l453,l455,l454));l456++;}else{l459
++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\t_soc_alpm_find rv=%d: "
"lpm_idx %d find_lpm_idx %d ""find_alpm_idx %d find_bkt_ptr %d\n"),l142,l448,
l453,l455,l454));l456++;}}(void)soc_alpm_lpm_vrf_get(l1,lpm_entry,&l169,&l29)
;for(l145 = 0;l145<l294;l145++){(void)_soc_alpm_mem_index(l1,l469,bkt_ptr,
l145,l159,&l405);l142 = soc_mem_read(l1,l469,MEM_BLOCK_ANY,l405,l16);if(
SOC_FAILURE(l142)){continue;}if(!soc_mem_field32_get(l1,l469,l16,VALIDf)){
continue;}(void)l28(l1,l16,l469,l10,l169,bkt_ptr,0,&l466);l450 = l453 = l454 = 
l455 = -1;l142 = l167(l1,&l466,l469,l16,&l453,&l454,&l455,FALSE);if(
SOC_SUCCESS(l142)){l450 = soc_alpm_logical_idx(l1,L3_DEFIPm,l453>>1,1);}if(
SOC_FAILURE(l142)||l454!= bkt_ptr||l450!= l448||l453>>1!= l449||l455!= l405){
l456++;l457++;LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\t_soc_alpm_find2 rv=%d: ""fl [%d,%d] fa %d fb %d ""l [%d,%d] a %d b %d\n"),
l142,l453>>1,l450,l455,l454,l449,l448,l405,bkt_ptr));}}if(l457 == 0){l459++;}
if(!l446){continue;}l142 = l327(l1,lpm_entry,l10?l448:((l448<<1)+l191));if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tLPM Hash check failed: rv %d ""lpm_idx %d\n"),l142,l448));l456++;}else{
l459++;}l142 = alpm_bucket_is_assigned(l1,bkt_ptr,l10,&l316);if(SOC_FAILURE(
l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tInvalid bucket pointer %d ""detected, in memory %s index %d\n"),bkt_ptr,
SOC_MEM_NAME(l1,L3_DEFIPm),l448));l456++;}else if(l316 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),bkt_ptr,SOC_MEM_NAME(l1,L3_DEFIPm),l448)
);l456++;}else{l459++;}l468 = _soc_trident2_alpm_bkt_view_get(l1,bkt_ptr<<2);
if(l469!= l468){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMismatched alpm view ""in bucket %d, expected %s, was %s\n"),bkt_ptr,
SOC_MEM_NAME(l1,l469),SOC_MEM_NAME(l1,l468)));l456++;}else{l459++;}l142 = 
l155(l1,lpm_entry,prefix,&l162,&l27);if(SOC_SUCCESS(l142)){if(l10){l220 = 
VRF_PREFIX_TRIE_IPV6(l1,l29);}else{l220 = VRF_PREFIX_TRIE_IPV4(l1,l29);}l165 = 
NULL;if(l220){l142 = trie_find_lpm(l220,prefix,l162,&l165);}if(SOC_SUCCESS(
l142)&&l165){l225 = (payload_t*)l165;l461 = ((payload_t*)(l225->bkt_ptr))->
len;if(l461!= l176){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tBPM len mismatch: lpm_idx %d"" alpm_idx %d bpm_len %d trie_bpm_len %d\n"),
l448,l405,l176,l461));l456++;}else{l459++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\ttrie_find_lpm failed: ""lpm_idx %d alpm_idx %d lpmp %p\n"),
l448,l405,l165));l456++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tPrefix creating failed: ""lpm_idx %d alpm_idx %d\n"),l448,l405));l456++;}}
}if(l456){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP index:[%d,%d] check failed. ipv6 %d error count %d\n"),l413,l452,
l451,l456));l458 = 1;}else if(l459){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP index:[%d,%d] check passed. ipv6 %d success count %d\n"),l413,
l452,l451,l459));}SOC_ALPM_LPM_UNLOCK(l1);soc_cm_sfree(l1,l465);return(l458?
SOC_E_FAIL:SOC_E_NONE);}extern int _soc_alpm_128_sanity_check(int l1,int l383
,int l384,int l446,int*l447);int soc_alpm_sanity_check(int l1,soc_mem_t l25,
int index,int l446){int l142 = SOC_E_NONE;int l218 = SOC_E_NONE;int l302 = -1
,l470 = -1;int l471 = -1,l472 = -1;int*l447 = NULL;if((l25 == L3_DEFIPm||
index == -2)&&soc_mem_index_max(l1,L3_DEFIPm)!= -1){l302 = soc_mem_index_min(
l1,L3_DEFIPm);l470 = soc_mem_index_max(l1,L3_DEFIPm);if(SOC_URPF_STATUS_GET(
l1)){l470>>= 1;}if(index<-2||index>l470){return SOC_E_PARAM;}if(index>= l302
&&index<= l470){l302 = index;l470 = index;}}if((l25 == L3_DEFIP_PAIR_128m||
index == -2)&&soc_mem_index_max(l1,L3_DEFIP_PAIR_128m)!= -1){l471 = 
soc_mem_index_min(l1,L3_DEFIP_PAIR_128m);l472 = soc_mem_index_max(l1,
L3_DEFIP_PAIR_128m);if(SOC_URPF_STATUS_GET(l1)){l472>>= 1;}if(index<-2||index
>l472){return SOC_E_PARAM;}if(index>= l471&&index<= l472){l471 = index;l472 = 
index;}}if(l470 == -1&&l472 == -1){return SOC_E_PARAM;}l447 = sal_alloc(
sizeof(int)*MAX_PIVOT_COUNT,"Bucket ptr array");if(l447 == NULL){return
SOC_E_MEMORY;}sal_memset(l447,0xff,sizeof(int)*MAX_PIVOT_COUNT);if(l470!= -1)
{LOG_CLI(("Processing ALPM sanity check L3_DEFIP "
"from index:%d to index:%d\n",l302,l470));l142 = _soc_alpm_sanity_check(l1,
l302,l470,l446,l447);LOG_CLI(("ALPM sanity check L3_DEFIP %s. \n\n",
SOC_SUCCESS(l142)?"passed":"failed"));}if(l472!= -1){LOG_CLI((
"Processing ALPM sanity check L3_DEFIP_PAIR_128 "
"from index:%d to index:%d\n",l471,l472));l218 = _soc_alpm_128_sanity_check(
l1,l471,l472,l446,l447);LOG_CLI(("ALPM sanity check L3_DEFIP_PAIR_128 %s. \n"
,SOC_SUCCESS(l218)?"passed":"failed"));}sal_free(l447);return SOC_SUCCESS(
l142)?l218:l142;}int soc_alpm_route_capacity_get(int l22,int l473,int*l474,
int*l475){int l142 = SOC_E_NONE;int l476,l477,l478;if(l474 == NULL&&l475 == 
NULL){return SOC_E_PARAM;}if(l473 == L3_DEFIP_MODE_V4){l478 = 
SOC_TD2_ALPM_MAX_BKTS;if(SOC_URPF_STATUS_GET(l22)){if(soc_alpm_mode_get(l22)
== SOC_ALPM_MODE_COMBINED){l476 = l478/2*ALPM_IPV4_BKT_COUNT;l477 = l478/2*(
ALPM_IPV4_BKT_COUNT/3);}else{l476 = l478/2*(ALPM_IPV4_BKT_COUNT/2);l477 = 
l478/2*((ALPM_IPV4_BKT_COUNT/2)/3);}}else{l476 = l478*ALPM_IPV4_BKT_COUNT;
l477 = l478*(ALPM_IPV4_BKT_COUNT/3);}}else if(l473 == L3_DEFIP_MODE_64){l478 = 
SOC_TD2_ALPM_MAX_BKTS/2;if(SOC_URPF_STATUS_GET(l22)){if(soc_alpm_mode_get(l22
) == SOC_ALPM_MODE_COMBINED){l476 = l478/2*ALPM_IPV6_64_BKT_COUNT;l477 = l478
/2*(ALPM_IPV6_64_BKT_COUNT/3);}else{l476 = l478/2*(ALPM_IPV6_64_BKT_COUNT/2);
l477 = l478/2*(ALPM_IPV6_64_BKT_COUNT/2/3);}}else{if(soc_alpm_mode_get(l22) == 
SOC_ALPM_MODE_COMBINED){l476 = l478*ALPM_IPV6_64_BKT_COUNT*2;l477 = l478*(
ALPM_IPV6_64_BKT_COUNT*2/3);}else{l476 = l478*ALPM_IPV6_64_BKT_COUNT;l477 = 
l478*(ALPM_IPV6_64_BKT_COUNT/3);}}}else if(l473 == L3_DEFIP_MODE_128){l478 = 
SOC_TD2_ALPM_MAX_BKTS/4;if(SOC_URPF_STATUS_GET(l22)){if(soc_alpm_mode_get(l22
) == SOC_ALPM_MODE_COMBINED){l476 = l478/2*ALPM_IPV6_128_BKT_COUNT;l477 = 
l478/2*(ALPM_IPV6_128_BKT_COUNT/3);}else{l476 = l478/2*(
ALPM_IPV6_128_BKT_COUNT/2);l477 = l478/2*(ALPM_IPV6_128_BKT_COUNT/2/3);}}else
{if(soc_alpm_mode_get(l22) == SOC_ALPM_MODE_COMBINED){l476 = l478*
ALPM_IPV6_128_BKT_COUNT*2;l477 = l478*(ALPM_IPV6_128_BKT_COUNT*2/3);}else{
l476 = l478*ALPM_IPV6_128_BKT_COUNT;l477 = l478*(ALPM_IPV6_128_BKT_COUNT/3);}
}}else{return SOC_E_UNAVAIL;}if(l474!= NULL){*l474 = l476;}if(l475!= NULL){*
l475 = l477;}return l142;}
#endif /* ALPM_ENABLE */
