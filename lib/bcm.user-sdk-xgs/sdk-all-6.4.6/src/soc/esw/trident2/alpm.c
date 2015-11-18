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
soc_alpm_bucket_t soc_alpm_bucket[SOC_MAX_NUM_DEVICES];void
soc_alpm_lpm_state_dump(int l1);static int l2(int l1);static int l3(int l1);
static int l4(int l1,void*l5,int*index,int l6);static int l7(int l1,void*l8);
static int l9(int l1,void*l8,int l10,int l11,int l12,
defip_aux_scratch_entry_t*l13);static int l14(int l1,void*l8,void*l15,int*l16
,int*l17,int*l10);static int l18(int l1,void*l8,void*l15,int*l16);static int
l19(int l1);static int l20(int l21,void*lpm_entry,void*l22,void*l23,soc_mem_t
l24,uint32 l25,uint32*l26);static int l27(int l21,void*l22,soc_mem_t l24,int
l10,int l28,int l29,int index,void*lpm_entry);static int l30(int l21,uint32*
key,int len,int l28,int l10,defip_entry_t*lpm_entry,int l31,int l32);static
int l33(int l1,int l28,int l34);static int l35(int l1,void*entry,int*l17);
typedef struct l36{int l37;int l38;int l39;int next;int l40;int l41;}l42,*l43
;static l43 l44[SOC_MAX_NUM_DEVICES];typedef struct l45{soc_field_info_t*l46;
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
soc_field_info_t*l89;soc_field_info_t*l90;soc_field_info_t*l91;
soc_field_info_t*l92;soc_field_info_t*l93;soc_field_info_t*l94;
soc_field_info_t*l95;soc_field_info_t*l96;soc_field_info_t*l97;
soc_field_info_t*l98;soc_field_info_t*l99;soc_field_info_t*l100;
soc_field_info_t*l101;soc_field_info_t*l102;soc_field_info_t*l103;
soc_field_info_t*l104;soc_field_info_t*l105;}l106,*l107;static l107 l108[
SOC_MAX_NUM_DEVICES];typedef struct l109{int l21;int l110;int l111;uint16*
l112;uint16*l113;}l114;typedef uint32 l115[5];typedef int(*l116)(l115 l117,
l115 l118);static l114*l119[SOC_MAX_NUM_DEVICES];static void l120(int l1,void
*l15,int index,l115 l121);static uint16 l122(uint8*l123,int l124);static int
l125(int l21,int l110,int l111,l114**l126);static int l127(l114*l128);static
int l129(l114*l130,l116 l131,l115 entry,int l132,uint16*l133);static int l134
(l114*l130,l116 l131,l115 entry,int l132,uint16 l135,uint16 l136);static int
l137(l114*l130,l116 l131,l115 entry,int l132,uint16 l138);alpm_vrf_handle_t*
alpm_vrf_handle[SOC_MAX_NUM_DEVICES];alpm_pivot_t**tcam_pivot[
SOC_MAX_NUM_DEVICES];int soc_alpm_mode_get(int l1){if(
soc_trident2_alpm_mode_get(l1) == 1){return 1;}else{return 0;}}static int l139
(int l1,const void*entry,int*l132){int l140;uint32 l141;int l10;l10 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f);if(l10){l141 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l140 = _ipmask2pfx
(l141,l132))<0){return(l140);}l141 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK1f);if(*l132){if(l141!= 0xffffffff){return(SOC_E_PARAM);}*l132+= 
32;}else{if((l140 = _ipmask2pfx(l141,l132))<0){return(l140);}}}else{l141 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l140 = _ipmask2pfx
(l141,l132))<0){return(l140);}}return SOC_E_NONE;}int _soc_alpm_rpf_entry(int
l1,int l142){int l143;l143 = (l142>>2)&0x3fff;l143+= SOC_ALPM_BUCKET_COUNT(l1
);return(l142&~(0x3fff<<2))|(l143<<2);}int soc_alpm_physical_idx(int l1,
soc_mem_t l24,int index,int l144){int l145 = index&1;if(l144){return
soc_trident2_l3_defip_index_map(l1,l24,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_map(l1,l24,index);index<<= 1;index|= l145;return
index;}int soc_alpm_logical_idx(int l1,soc_mem_t l24,int index,int l144){int
l145 = index&1;if(l144){return soc_trident2_l3_defip_index_remap(l1,l24,index
);}index>>= 1;index = soc_trident2_l3_defip_index_remap(l1,l24,index);index
<<= 1;index|= l145;return index;}static int l146(int l1,void*entry,uint32*
prefix,uint32*l17,int*l26){int l147,l148,l10;int l132 = 0;int l140 = 
SOC_E_NONE;uint32 l149,l145;prefix[0] = prefix[1] = prefix[2] = prefix[3] = 
prefix[4] = 0;l10 = soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f);l147 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR0f);l148 = soc_mem_field32_get(
l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);prefix[1] = l147;l147 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR1f);l148 = soc_mem_field32_get(
l1,L3_DEFIPm,entry,IP_ADDR_MASK1f);prefix[0] = l147;if(l10){prefix[4] = 
prefix[1];prefix[3] = prefix[0];prefix[1] = prefix[0] = 0;l148 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l140 = _ipmask2pfx
(l148,&l132))<0){return(l140);}l148 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK1f);if(l132){if(l148!= 0xffffffff){return(SOC_E_PARAM);}l132+= 32
;}else{if((l140 = _ipmask2pfx(l148,&l132))<0){return(l140);}}l149 = 64-l132;
if(l149<32){prefix[4]>>= l149;l145 = (((32-l149) == 32)?0:(prefix[3])<<(32-
l149));prefix[3]>>= l149;prefix[4]|= l145;}else{prefix[4] = (((l149-32) == 32
)?0:(prefix[3])>>(l149-32));prefix[3] = 0;}}else{l148 = soc_mem_field32_get(
l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l140 = _ipmask2pfx(l148,&l132))<0){
return(l140);}prefix[1] = (((32-l132) == 32)?0:(prefix[1])>>(32-l132));prefix
[0] = 0;}*l17 = l132;*l26 = (prefix[0] == 0)&&(prefix[1] == 0)&&(l132 == 0);
return SOC_E_NONE;}int _soc_alpm_find_in_bkt(int l1,soc_mem_t l24,int
bucket_index,int l150,uint32*l15,void*l151,int*l133,int l34){int l140;l140 = 
soc_mem_alpm_lookup(l1,l24,bucket_index,MEM_BLOCK_ANY,l150,l15,l151,l133);if(
SOC_SUCCESS(l140)){return l140;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l34)){return
soc_mem_alpm_lookup(l1,l24,bucket_index+1,MEM_BLOCK_ANY,l150,l15,l151,l133);}
return l140;}static int l152(int l1,uint32*prefix,uint32 l153,int l34,int l28
,int*l154,int*l155,int*bucket_index){int l140 = SOC_E_NONE;trie_t*l156;
trie_node_t*l157 = NULL;alpm_pivot_t*l158;if(l34){l156 = VRF_PIVOT_TRIE_IPV6(
l1,l28);}else{l156 = VRF_PIVOT_TRIE_IPV4(l1,l28);}l140 = trie_find_lpm(l156,
prefix,l153,&l157);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Pivot find failed\n")));return l140;}l158 = (alpm_pivot_t*)
l157;*l154 = 1;*l155 = PIVOT_TCAM_INDEX(l158);*bucket_index = 
PIVOT_BUCKET_INDEX(l158);return SOC_E_NONE;}static int l159(int l1,void*l8,
soc_mem_t l24,void*l151,int*l155,int*bucket_index,int*l16,int l160){uint32 l15
[SOC_MAX_MEM_FIELD_WORDS];int l161,l28,l34;int l133;uint32 l11,l150;int l140 = 
SOC_E_NONE;int l154 = 0;l34 = soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE0f);if(
l34){if(!(l34 = soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l8,&l161,&l28));if
(l161 == 0){if(soc_alpm_mode_get(l1)){return SOC_E_PARAM;}}if(l28 == 
SOC_VRF_MAX(l1)+1){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l150);}else{
l11 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l150);}if(l161!= SOC_L3_VRF_OVERRIDE
){if(l160){uint32 prefix[5],l153;int l26 = 0;l140 = l146(l1,l8,prefix,&l153,&
l26);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l140;}l140 = l152(l1,
prefix,l153,l34,l28,&l154,l155,bucket_index);SOC_IF_ERROR_RETURN(l140);}else{
defip_aux_scratch_entry_t l13;sal_memset(&l13,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l1,l8,l34,l11,0,&l13));
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l13,TRUE,&l154,l155,
bucket_index));}if(l154){l20(l1,l8,l15,0,l24,0,0);l140 = 
_soc_alpm_find_in_bkt(l1,l24,*bucket_index,l150,l15,l151,&l133,l34);if(
SOC_SUCCESS(l140)){*l16 = l133;}}else{l140 = SOC_E_NOT_FOUND;}}return l140;}
static int l162(int l1,void*l8,void*l151,void*l163,soc_mem_t l24,int l133){
defip_aux_scratch_entry_t l13;int l161,l34,l28;int bucket_index;uint32 l11,
l150;int l140 = SOC_E_NONE;int l154 = 0,l145 = 0;int l155;l34 = 
soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = 
soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l8,&l161,&l28));if(l28 == 
SOC_VRF_MAX(l1)+1){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l150);}else{
l11 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l150);}if(!soc_alpm_mode_get(l1)){
l11 = 2;}if(l161!= SOC_L3_VRF_OVERRIDE){sal_memset(&l13,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l1,l8,l34,l11,0,&l13));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l24,MEM_BLOCK_ANY,l133,l151));if(
SOC_URPF_STATUS_GET(l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l133),l163));}l145 = soc_mem_field32_get
(l1,L3_DEFIP_AUX_SCRATCHm,&l13,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l13,REPLACE_LENf,l145);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l13,TRUE,&l154,&l155,&bucket_index));if
(SOC_URPF_STATUS_GET(l1)){if(l145 == 0){soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l13,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l13,RPEf,0);}l145 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l145+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l145);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l13,TRUE,&l154,&l155,&bucket_index));}}
return l140;}int alpm_mem_prefix_array_cb(trie_node_t*node,void*l164){
alpm_mem_prefix_array_t*l165 = (alpm_mem_prefix_array_t*)l164;if(node->type == 
PAYLOAD){l165->prefix[l165->count] = (payload_t*)node;l165->count++;}return
SOC_E_NONE;}int alpm_delete_node_cb(trie_node_t*node,void*l164){if(node!= 
NULL){sal_free(node);}return SOC_E_NONE;}static int l166(int l1,int l167,int
l34,int l168){int l140,l145,index;defip_aux_table_entry_t entry;index = l167
>>(l34?0:1);l140 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&
entry);SOC_IF_ERROR_RETURN(l140);if(l34){soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l168);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l168);l145 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l167&1){soc_mem_field32_set(l1
,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l168);l145 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l168);l145 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l140 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l140);if(
SOC_URPF_STATUS_GET(l1)){l145++;if(l34){soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l168);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l168);}else{if(l167&1){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l168);}else{
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l168);}}
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l145);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l145);index+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l140 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}
return l140;}static int l169(int l1,int l170,void*entry,void*l171,int l172){
uint32 l145,l148,l34,l11,l173 = 0;soc_mem_t l24 = L3_DEFIPm;soc_mem_t l174 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l175;int l140 = SOC_E_NONE,l132,l176,l28,
l177;SOC_IF_ERROR_RETURN(soc_mem_read(l1,l174,MEM_BLOCK_ANY,l170,l171));l145 = 
soc_mem_field32_get(l1,l24,entry,VRF_ID_0f);soc_mem_field32_set(l1,l174,l171,
VRF0f,l145);l145 = soc_mem_field32_get(l1,l24,entry,VRF_ID_1f);
soc_mem_field32_set(l1,l174,l171,VRF1f,l145);l145 = soc_mem_field32_get(l1,
l24,entry,MODE0f);soc_mem_field32_set(l1,l174,l171,MODE0f,l145);l145 = 
soc_mem_field32_get(l1,l24,entry,MODE1f);soc_mem_field32_set(l1,l174,l171,
MODE1f,l145);l34 = l145;l145 = soc_mem_field32_get(l1,l24,entry,VALID0f);
soc_mem_field32_set(l1,l174,l171,VALID0f,l145);l145 = soc_mem_field32_get(l1,
l24,entry,VALID1f);soc_mem_field32_set(l1,l174,l171,VALID1f,l145);l145 = 
soc_mem_field32_get(l1,l24,entry,IP_ADDR_MASK0f);if((l140 = _ipmask2pfx(l145,
&l132))<0){return l140;}l148 = soc_mem_field32_get(l1,l24,entry,
IP_ADDR_MASK1f);if((l140 = _ipmask2pfx(l148,&l176))<0){return l140;}if(l34){
soc_mem_field32_set(l1,l174,l171,IP_LENGTH0f,l132+l176);soc_mem_field32_set(
l1,l174,l171,IP_LENGTH1f,l132+l176);}else{soc_mem_field32_set(l1,l174,l171,
IP_LENGTH0f,l132);soc_mem_field32_set(l1,l174,l171,IP_LENGTH1f,l176);}l145 = 
soc_mem_field32_get(l1,l24,entry,IP_ADDR0f);soc_mem_field32_set(l1,l174,l171,
IP_ADDR0f,l145);l145 = soc_mem_field32_get(l1,l24,entry,IP_ADDR1f);
soc_mem_field32_set(l1,l174,l171,IP_ADDR1f,l145);l145 = soc_mem_field32_get(
l1,l24,entry,ENTRY_TYPE0f);soc_mem_field32_set(l1,l174,l171,ENTRY_TYPE0f,l145
);l145 = soc_mem_field32_get(l1,l24,entry,ENTRY_TYPE1f);soc_mem_field32_set(
l1,l174,l171,ENTRY_TYPE1f,l145);if(!l34){sal_memcpy(&l175,entry,sizeof(l175))
;l140 = soc_alpm_lpm_vrf_get(l1,(void*)&l175,&l28,&l132);SOC_IF_ERROR_RETURN(
l140);SOC_IF_ERROR_RETURN(soc_alpm_lpm_ip4entry1_to_0(l1,&l175,&l175,
PRESERVE_HIT));l140 = soc_alpm_lpm_vrf_get(l1,(void*)&l175,&l177,&l132);
SOC_IF_ERROR_RETURN(l140);}else{l140 = soc_alpm_lpm_vrf_get(l1,entry,&l28,&
l132);}if(SOC_URPF_STATUS_GET(l1)){if(l172>= (soc_mem_index_count(l1,
L3_DEFIPm)>>1)){l173 = 1;}}switch(l28){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l174,l171,VALID0f,0);l11 = 0;break;case
SOC_L3_VRF_GLOBAL:l11 = l173?1:0;break;default:l11 = l173?3:2;break;}if(
SOC_MEM_FIELD_VALID(l1,l24,MULTICAST_ROUTE0f)){l145 = soc_mem_field32_get(l1,
l24,entry,MULTICAST_ROUTE0f);if(l145){l11 = l173?5:4;}}soc_mem_field32_set(l1
,l174,l171,DB_TYPE0f,l11);if(!l34){switch(l177){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l174,l171,VALID1f,0);l11 = 0;break;case
SOC_L3_VRF_GLOBAL:l11 = l173?1:0;break;default:l11 = l173?3:2;break;}if(
SOC_MEM_FIELD_VALID(l1,l24,MULTICAST_ROUTE1f)){l145 = soc_mem_field32_get(l1,
l24,entry,MULTICAST_ROUTE1f);if(l145){l11 = l173?5:4;}}soc_mem_field32_set(l1
,l174,l171,DB_TYPE1f,l11);}else{if(l28 == SOC_L3_VRF_OVERRIDE){
soc_mem_field32_set(l1,l174,l171,VALID1f,0);}if(SOC_MEM_FIELD_VALID(l1,l24,
MULTICAST_ROUTE1f)){l145 = soc_mem_field32_get(l1,l24,entry,MULTICAST_ROUTE1f
);if(l145){l11 = l173?5:4;}}soc_mem_field32_set(l1,l174,l171,DB_TYPE1f,l11);}
if(l173){l145 = soc_mem_field32_get(l1,l24,entry,ALG_BKT_PTR0f);if(l145){l145
+= SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l24,entry,ALG_BKT_PTR0f,
l145);}if(!l34){l145 = soc_mem_field32_get(l1,l24,entry,ALG_BKT_PTR1f);if(
l145){l145+= SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l24,entry,
ALG_BKT_PTR1f,l145);}}}return SOC_E_NONE;}static int l178(int l1,int l179,int
index,int l180,void*entry){defip_aux_table_entry_t l171;l180 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l180,1);SOC_IF_ERROR_RETURN(l169(l1,l180,
entry,(void*)&l171,index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l1,
MEM_BLOCK_ANY,index,entry));index = soc_alpm_physical_idx(l1,L3_DEFIPm,index,
1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,index,&l171
));return SOC_E_NONE;}int _soc_alpm_insert_in_bkt(int l1,soc_mem_t l24,int
bucket_index,int l150,void*l151,uint32*l15,int*l133,int l34){int l140;l140 = 
soc_mem_alpm_insert(l1,l24,bucket_index,MEM_BLOCK_ANY,l150,l151,l15,l133);if(
l140 == SOC_E_FULL){if(SOC_ALPM_V6_SCALE_CHECK(l1,l34)){return
soc_mem_alpm_insert(l1,l24,bucket_index+1,MEM_BLOCK_ANY,l150,l151,l15,l133);}
}return l140;}int _soc_alpm_mem_index(int l1,soc_mem_t l24,int bucket_index,
int l181,uint32 l150,int*l182){int l183,l184 = 0;int l185[4] = {0};int l186 = 
0;int l187 = 0;int l188;int l34 = 0;int l189 = 6;switch(l24){case
L3_DEFIP_ALPM_IPV6_64m:l189 = 4;l34 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:
l189 = 2;l34 = 1;break;default:break;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l34)){if(
l181>= ALPM_RAW_BKT_COUNT*l189){bucket_index++;l181-= ALPM_RAW_BKT_COUNT*l189
;}}l188 = (4)-_shr_popcount(l150&((1<<(4))-1));if(bucket_index>= (1<<16)||
l181>= l188*l189){return SOC_E_FULL;}l187 = l181%l189;for(l183 = 0;l183<(4);
l183++){if((1<<l183)&l150){continue;}l185[l184++] = l183;}l186 = l185[l181/
l189];*l182 = (l187<<16)|(bucket_index<<2)|(l186);return SOC_E_NONE;}void
_soc_alpm_raw_mem_read(int l21,soc_mem_t l24,void*l190,int l191,void*entry){
soc_mem_info_t l192;soc_field_info_t l193;int l194 = soc_mem_entry_bits(l21,
l24)-1;l192.flags = 0;l192.bytes = sizeof(defip_alpm_raw_entry_t);l193.flags = 
SOCF_LE;l193.bp = l194*l191;l193.len = l194;(void)
soc_meminfo_fieldinfo_field_get(l190,&l192,&l193,entry);}void
_soc_alpm_raw_mem_write(int l21,soc_mem_t l24,void*l190,int l191,void*entry){
soc_mem_info_t l192;soc_field_info_t l193;int l194 = soc_mem_entry_bits(l21,
l24)-1;l192.flags = 0;l192.bytes = sizeof(defip_alpm_raw_entry_t);l193.bp = 
l194*l191;l193.len = l194;l193.flags = SOCF_LE;(void)
soc_meminfo_fieldinfo_field_set(l190,&l192,&l193,entry);}int l195(int l1,
soc_mem_t l24,int index){return SOC_ALPM_BKT_ENTRY_TO_IDX(index%(1<<16));}int
_soc_alpm_raw_bucket_read(int l1,soc_mem_t l24,int bucket_index,void*l190,
void*l196){int l183,l34 = 1;int l197,l198;defip_alpm_raw_entry_t*l199 = l190;
defip_alpm_raw_entry_t*l200 = l196;if(l24 == L3_DEFIP_ALPM_IPV4m){l34 = 0;}
l197 = SOC_ALPM_BKT_IDX_TO_ENTRY(bucket_index);for(l183 = 0;l183<
SOC_ALPM_RAW_BUCKET_SIZE(l1,l34);l183++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,
L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l197+l183,&l199[l183]));if(
SOC_URPF_STATUS_GET(l1)){l198 = _soc_alpm_rpf_entry(l1,l197+l183);
SOC_IF_ERROR_RETURN(soc_mem_read(l1,L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l198,&
l200[l183]));}}return SOC_E_NONE;}int _soc_alpm_raw_bucket_write(int l1,
soc_mem_t l24,int bucket_index,uint32 l150,void*l190,void*l196,int l201){int
l183 = 0,l202,l34 = 1;int l197,l198,l203;defip_alpm_raw_entry_t*l199 = l190;
defip_alpm_raw_entry_t*l200 = l196;int l204 = 6;switch(l24){case
L3_DEFIP_ALPM_IPV4m:l204 = 6;l34 = 0;break;case L3_DEFIP_ALPM_IPV6_64m:l204 = 
4;l34 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:l204 = 2;l34 = 1;break;default:
break;}l197 = SOC_ALPM_BKT_IDX_TO_ENTRY(bucket_index);if(l201 == -1){l203 = 
SOC_ALPM_RAW_BUCKET_SIZE(l1,l34);}else{l203 = (l201/l204)+1;}for(l202 = 0;
l202<SOC_ALPM_RAW_BUCKET_SIZE(l1,l34);l202++){if((1<<(l202%(4)))&l150){
continue;}SOC_IF_ERROR_RETURN(soc_mem_write(l1,L3_DEFIP_ALPM_RAWm,
MEM_BLOCK_ANY,l197+l202,&l199[l202]));_soc_trident2_alpm_bkt_view_set(l1,l197
+l202,l24);if(SOC_URPF_STATUS_GET(l1)){l198 = _soc_alpm_rpf_entry(l1,l197+
l202);_soc_trident2_alpm_bkt_view_set(l1,l198,l24);SOC_IF_ERROR_RETURN(
soc_mem_write(l1,L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l198,&l200[l202]));}if(++
l183 == l203){break;}}return SOC_E_NONE;}void _soc_alpm_raw_parity_set(int l1
,soc_mem_t l24,void*l151){int l183,l205,l206 = 0;uint32*entry = l151;l205 = 
soc_mem_entry_words(l1,l24);for(l183 = 0;l183<l205;l183++){l206+= 
_shr_popcount(entry[l183]);}if(l206&0x1){if(SOC_MEM_FIELD_VALID(l1,l24,
EVEN_PARITYf)){soc_mem_field32_set(l1,l24,l151,EVEN_PARITYf,1);}}}static int
l207(int l1,void*l8,soc_mem_t l24,void*l151,void*l163,int*l16,int bucket_index
,int l155){alpm_pivot_t*l158,*l208,*l209;defip_aux_scratch_entry_t l13;uint32
l15[SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l210,l153;uint32 l211[5];int l34
,l28,l161;int l133;int l140 = SOC_E_NONE,l212;uint32 l11,l150,l168 = 0;int
l154 =0;int l167;int l213 = 0;trie_t*trie,*l214;trie_node_t*l215,*l216 = NULL
,*l157 = NULL;payload_t*l217,*l218,*l219;defip_entry_t lpm_entry;
alpm_bucket_handle_t*l220;int l183,l221 = -1,l26 = 0,l222 = 0;
alpm_mem_prefix_array_t l165;defip_alpm_ipv4_entry_t l223,l224;
defip_alpm_ipv6_64_entry_t l225,l226;void*l227,*l228;int*l136 = NULL;int l229
= 0;trie_t*l156 = NULL;int l230;int l231;void*l232;void*l233 = NULL;void*l234
;void*l235;void*l236;int l237 = 0;int l191 = 0;l34 = soc_mem_field32_get(l1,
L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = soc_mem_field32_get(l1,L3_DEFIPm,l8,
MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,
l8,&l161,&l28));if(l28 == SOC_VRF_MAX(l1)+1){l11 = 0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l150);}else{l11 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l150);}l24 = (l34)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l227 = ((l34)?((uint32*)&(l225)):((uint32*)&(l223)));l228
= ((l34)?((uint32*)&(l226)):((uint32*)&(l224)));l140 = l146(l1,l8,prefix,&
l153,&l26);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l140;}sal_memset(&l13,0,
sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l1,l8,l34,l11,0,&
l13));if(bucket_index == 0){l140 = l152(l1,prefix,l153,l34,l28,&l154,&l155,&
bucket_index);SOC_IF_ERROR_RETURN(l140);if(l154 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: "
"Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l140 = 
_soc_alpm_insert_in_bkt(l1,l24,bucket_index,l150,l151,l15,&l133,l34);if(l140
== SOC_E_NONE){*l16 = l133;if(SOC_URPF_STATUS_GET(l1)){l212 = soc_mem_write(
l1,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l133),l163);if(SOC_FAILURE(l212))
{return l212;}}}if(l140 == SOC_E_FULL){l213 = 1;}l158 = ALPM_TCAM_PIVOT(l1,
l155);if(l158 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"ins: pivot index %d bkt %d is not valid \n"),l155,bucket_index));return
SOC_E_PARAM;}trie = PIVOT_BUCKET_TRIE(l158);l209 = l158;l217 = sal_alloc(
sizeof(payload_t),"Payload for Key");if(l217 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory for ""trie node \n")));return
SOC_E_MEMORY;}l218 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");
if(l218 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory for ""pfx trie node \n")));
sal_free(l217);return SOC_E_MEMORY;}sal_memset(l217,0,sizeof(*l217));
sal_memset(l218,0,sizeof(*l218));l217->key[0] = prefix[0];l217->key[1] = 
prefix[1];l217->key[2] = prefix[2];l217->key[3] = prefix[3];l217->key[4] = 
prefix[4];l217->len = l153;l217->index = l133;sal_memcpy(l218,l217,sizeof(*
l217));l218->bkt_ptr = l217;l140 = trie_insert(trie,prefix,NULL,l153,(
trie_node_t*)l217);if(SOC_FAILURE(l140)){goto l238;}if(l34){l214 = 
VRF_PREFIX_TRIE_IPV6(l1,l28);}else{l214 = VRF_PREFIX_TRIE_IPV4(l1,l28);}if(!
l26){l140 = trie_insert(l214,prefix,NULL,l153,(trie_node_t*)l218);}else{l157 = 
NULL;l140 = trie_find_lpm(l214,0,0,&l157);l219 = (payload_t*)l157;if(
SOC_SUCCESS(l140)){l219->bkt_ptr = l217;}}l210 = l153;if(SOC_FAILURE(l140)){
goto l239;}if(l213){l140 = alpm_bucket_assign(l1,&bucket_index,l34);if(
SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate""new bucket for split\n")));
bucket_index = -1;goto l240;}l140 = trie_split(trie,l34?_MAX_KEY_LEN_144_:
_MAX_KEY_LEN_48_,FALSE,l211,&l153,&l215,NULL,FALSE,1024);if(SOC_FAILURE(l140)
){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not split bucket""for prefix 0x%08x 0x%08x\n"),
prefix[0],prefix[1]));goto l240;}l157 = NULL;l140 = trie_find_lpm(l214,l211,
l153,&l157);l219 = (payload_t*)l157;if(SOC_FAILURE(l140)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l211[0],l211[1],l211[
2],l211[3],l211[4],l153));goto l240;}if(l219->bkt_ptr){if(l219->bkt_ptr == 
l217){sal_memcpy(l227,l151,l34?sizeof(defip_alpm_ipv6_64_entry_t):sizeof(
defip_alpm_ipv4_entry_t));}else{l140 = soc_mem_read(l1,l24,MEM_BLOCK_ANY,((
payload_t*)l219->bkt_ptr)->index,l227);if(SOC_FAILURE(l140)){goto l240;}if(
SOC_URPF_STATUS_GET(l1)){l140 = soc_mem_read(l1,l24,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,((payload_t*)l219->bkt_ptr)->index),l228);l222 = 
soc_mem_field32_get(l1,l24,l228,DEFAULTROUTEf);}}if(SOC_FAILURE(l140)){goto
l240;}l140 = l27(l1,l227,l24,l34,l161,bucket_index,0,&lpm_entry);if(
SOC_FAILURE(l140)){goto l240;}l168 = ((payload_t*)(l219->bkt_ptr))->len;}else
{l140 = soc_mem_read(l1,L3_DEFIPm,MEM_BLOCK_ANY,soc_alpm_logical_idx(l1,
L3_DEFIPm,l155>>1,1),&lpm_entry);if((!l34)&&(l155&1)){l140 = 
soc_alpm_lpm_ip4entry1_to_0(l1,&lpm_entry,&lpm_entry,0);}}l220 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l220 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l140 = SOC_E_MEMORY;goto l240;}sal_memset(l220,0,sizeof(*l220));l158 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l158 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l140 = SOC_E_MEMORY;goto l240;}sal_memset(l158,0,sizeof(*l158));
PIVOT_BUCKET_HANDLE(l158) = l220;if(l34){l140 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l158));}else{l140 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l158));}PIVOT_BUCKET_TRIE(l158)->trie = l215;
PIVOT_BUCKET_INDEX(l158) = bucket_index;PIVOT_BUCKET_VRF(l158) = l28;
PIVOT_BUCKET_IPV6(l158) = l34;PIVOT_BUCKET_DEF(l158) = FALSE;l158->key[0] = 
l211[0];l158->key[1] = l211[1];l158->len = l153;l158->key[2] = l211[2];l158->
key[3] = l211[3];l158->key[4] = l211[4];if(l34){l156 = VRF_PIVOT_TRIE_IPV6(l1
,l28);}else{l156 = VRF_PIVOT_TRIE_IPV4(l1,l28);}do{if(!(l34)){l211[0] = (((32
-l153) == 32)?0:(l211[1])<<(32-l153));l211[1] = 0;}else{int l241 = 64-l153;
int l242;if(l241<32){l242 = l211[3]<<l241;l242|= (((32-l241) == 32)?0:(l211[4
])>>(32-l241));l211[0] = l211[4]<<l241;l211[1] = l242;l211[2] = l211[3] = 
l211[4] = 0;}else{l211[1] = (((l241-32) == 32)?0:(l211[4])<<(l241-32));l211[0
] = l211[2] = l211[3] = l211[4] = 0;}}}while(0);l30(l1,l211,l153,l28,l34,&
lpm_entry,0,0);soc_L3_DEFIPm_field32_set(l1,&lpm_entry,ALG_BKT_PTR0f,
bucket_index);sal_memset(&l165,0,sizeof(l165));l140 = trie_traverse(
PIVOT_BUCKET_TRIE(l158),alpm_mem_prefix_array_cb,&l165,_TRIE_INORDER_TRAVERSE
);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l240;}l136 = sal_alloc(sizeof(*l136)*l165.count,
"Temp storage for location of prefixes in new bucket");if(l136 == NULL){l140 = 
SOC_E_MEMORY;goto l240;}sal_memset(l136,-1,sizeof(*l136)*l165.count);l230 = 
sizeof(defip_alpm_raw_entry_t);l231 = l230*ALPM_RAW_BKT_COUNT_DW;l233 = 
sal_alloc(4*l231,"Raw memory buffer");if(l233 == NULL){l140 = SOC_E_MEMORY;
goto l240;}sal_memset(l233,0,4*l231);l234 = (uint8*)l233+l231;l235 = (uint8*)
l233+l231*2;l236 = (uint8*)l233+l231*3;l140 = _soc_alpm_raw_bucket_read(l1,
l24,PIVOT_BUCKET_INDEX(l209),l233,l234);if(SOC_FAILURE(l140)){goto l240;}for(
l183 = 0;l183<l165.count;l183++){payload_t*l132 = l165.prefix[l183];if(l132->
index>0){SOC_ALPM_RAW_INDEX_DECODE(l1,l34,l132->index,l237,l191);l232 = (
uint8*)l233+l237*l230;_soc_alpm_raw_mem_read(l1,l24,l232,l191,l227);
_soc_alpm_raw_mem_write(l1,l24,l232,l191,soc_mem_entry_null(l1,l24));if(
SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE(l1,l34,_soc_alpm_rpf_entry
(l1,l132->index),l237,l191);l232 = (uint8*)l234+l237*l230;
_soc_alpm_raw_mem_read(l1,l24,l232,l191,l228);_soc_alpm_raw_mem_write(l1,l24,
l232,l191,soc_mem_entry_null(l1,l24));}l140 = _soc_alpm_mem_index(l1,l24,
bucket_index,l183,l150,&l133);if(SOC_SUCCESS(l140)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l34,l133,l237,l191);l232 = (uint8*)l235+l237*l230;_soc_alpm_raw_mem_write
(l1,l24,l232,l191,l227);if(SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l34,_soc_alpm_rpf_entry(l1,l133),l237,l191);l232 = (uint8*)l236+l237*l230
;_soc_alpm_raw_mem_write(l1,l24,l232,l191,l228);}}}else{l140 = 
_soc_alpm_mem_index(l1,l24,bucket_index,l183,l150,&l133);if(SOC_SUCCESS(l140)
){l221 = l183;*l16 = l133;_soc_alpm_raw_parity_set(l1,l24,l151);
SOC_ALPM_RAW_INDEX_DECODE(l1,l34,l133,l237,l191);l232 = (uint8*)l235+l237*
l230;_soc_alpm_raw_mem_write(l1,l24,l232,l191,l151);if(SOC_URPF_STATUS_GET(l1
)){_soc_alpm_raw_parity_set(l1,l24,l163);SOC_ALPM_RAW_INDEX_DECODE(l1,l34,
_soc_alpm_rpf_entry(l1,l133),l237,l191);l232 = (uint8*)l236+l237*l230;
_soc_alpm_raw_mem_write(l1,l24,l232,l191,l163);}}}l136[l183] = l133;}l140 = 
_soc_alpm_raw_bucket_write(l1,l24,bucket_index,l150,(void*)l235,(void*)l236,
l165.count);if(SOC_FAILURE(l140)){goto l243;}l140 = l4(l1,&lpm_entry,&l167,
l222);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to add new""pivot to tcam\n")));if(l140 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l28,l34);}goto l243;}l167 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l167,l34);l140 = l166(l1,l167,l34,l168);if
(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),l167));goto l244
;}l140 = trie_insert(l156,l158->key,NULL,l158->len,(trie_node_t*)l158);if(
SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));goto l244;}l229 = 1;ALPM_TCAM_PIVOT(
l1,l167<<(l34?1:0)) = l158;PIVOT_TCAM_INDEX(l158) = l167<<(l34?1:0);
VRF_PIVOT_REF_INC(l1,l28,l34);for(l183 = 0;l183<l165.count;l183++){payload_t*
l132 = l165.prefix[l183];if(l132->index>0){l140 = soc_mem_cache_invalidate(l1
,l24,MEM_BLOCK_ANY,l132->index);if(SOC_SUCCESS(l140)){if(SOC_URPF_STATUS_GET(
l1)){SOC_IF_ERROR_RETURN(soc_mem_cache_invalidate(l1,l24,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l132->index)));}}if(SOC_FAILURE(l140)){goto l244;}}
l132->index = l136[l183];}sal_free(l136);l136 = NULL;l140 = 
_soc_alpm_raw_bucket_write(l1,l24,PIVOT_BUCKET_INDEX(l209),l150,(void*)l233,(
void*)l234,-1);if(SOC_FAILURE(l140)){goto l244;}if(l221 == -1){l140 = 
_soc_alpm_insert_in_bkt(l1,l24,PIVOT_BUCKET_INDEX(l209),l150,l151,l15,&l133,
l34);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n")));
goto l244;}if(SOC_URPF_STATUS_GET(l1)){l140 = soc_mem_write(l1,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l133),l163);}*l16 = l133;l217->index = 
l133;}sal_free(l233);PIVOT_BUCKET_ENT_CNT_UPDATE(l158);VRF_BUCKET_SPLIT_INC(
l1,l28,l34);}VRF_TRIE_ROUTES_INC(l1,l28,l34);if(l26){sal_free(l218);}
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l13,TRUE,&l154,&
l155,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE
,&l13,FALSE,&l154,&l155,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l153 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l153+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l153);if(l26){
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l13,RPEf,1);}else{
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l13,RPEf,0);}
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l13,TRUE,&l154,&
l155,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE
,&l13,FALSE,&l154,&l155,&bucket_index));}PIVOT_BUCKET_ENT_CNT_UPDATE(l209);
return l140;l244:l212 = l7(l1,&lpm_entry);if(SOC_FAILURE(l212)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failure to free new prefix"
"at %d\n"),soc_alpm_logical_idx(l1,L3_DEFIPm,l167,l34)));}if(l229){l212 = 
trie_delete(l156,l158->key,l158->len,&l216);if(SOC_FAILURE(l212)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: trie delete failure "
"in bkt move rollback\n")));}}VRF_PIVOT_REF_DEC(l1,l28,l34);l243:l208 = l209;
for(l183 = 0;l183<l165.count;l183++){payload_t*prefix = l165.prefix[l183];if(
l136[l183]!= -1){if(l34){sal_memset(l227,0,sizeof(defip_alpm_ipv6_64_entry_t)
);}else{sal_memset(l227,0,sizeof(defip_alpm_ipv4_entry_t));}l212 = 
soc_mem_write(l1,l24,MEM_BLOCK_ANY,l136[l183],l227);
_soc_trident2_alpm_bkt_view_set(l1,l136[l183],INVALIDm);if(SOC_FAILURE(l212))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l1)){l212 = soc_mem_write(l1,l24,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l136[l183]),l227);_soc_trident2_alpm_bkt_view_set(l1,
_soc_alpm_rpf_entry(l1,l136[l183]),INVALIDm);if(SOC_FAILURE(l212)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l216 = NULL;l212 = trie_delete(
PIVOT_BUCKET_TRIE(l158),prefix->key,prefix->len,&l216);l217 = (payload_t*)
l216;if(SOC_FAILURE(l212)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(prefix
->index>0){l212 = trie_insert(PIVOT_BUCKET_TRIE(l208),prefix->key,NULL,prefix
->len,(trie_node_t*)l217);if(SOC_FAILURE(l212)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l217!= NULL){sal_free(l217);}}}if(l221
== -1){l216 = NULL;l212 = trie_delete(PIVOT_BUCKET_TRIE(l208),prefix,l210,&
l216);l217 = (payload_t*)l216;if(SOC_FAILURE(l212)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l217!= 
NULL){sal_free(l217);}}l212 = alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l158)
,l34);if(SOC_FAILURE(l212)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: new bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(l158
)));}trie_destroy(PIVOT_BUCKET_TRIE(l158));sal_free(l220);sal_free(l158);if(
l136!= NULL){sal_free(l136);}sal_free(l233);l216 = NULL;l212 = trie_delete(
l214,prefix,l210,&l216);l218 = (payload_t*)l216;if(SOC_FAILURE(l212)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l218){sal_free(
l218);}return l140;l240:if(l136!= NULL){sal_free(l136);}if(l233!= NULL){
sal_free(l233);}l216 = NULL;(void)trie_delete(l214,prefix,l210,&l216);l218 = 
(payload_t*)l216;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l34);}l239:l216 = NULL;(void)trie_delete(trie,prefix,l210,&l216)
;l217 = (payload_t*)l216;l238:if(l217!= NULL){sal_free(l217);}if(l218!= NULL)
{sal_free(l218);}return l140;}static int l30(int l21,uint32*key,int len,int
l28,int l10,defip_entry_t*lpm_entry,int l31,int l32){uint32 l245;if(l32){
sal_memset(lpm_entry,0,sizeof(defip_entry_t));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l108[(l21)]->l88),(l28&SOC_VRF_MAX(l21)));if(l28 == (SOC_VRF_MAX(l21)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l108[(l21)]->l90),(0));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l90),(SOC_VRF_MAX(l21)
));}if(l10){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),
(lpm_entry),(l108[(l21)]->l72),(key[0]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l73),(key[1]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l108[(l21)]->l76),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,
L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l77),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l108[(l21)]->l89),(l28&SOC_VRF_MAX(l21)));if(l28 == (SOC_VRF_MAX(l21)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l108[(l21)]->l91),(0));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l91),(SOC_VRF_MAX(l21)
));}soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(
lpm_entry),(l108[(l21)]->l87),(1));if(len>= 32){l245 = 0xffffffff;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l108[(l21)]->l75),(l245));l245 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry
),(l108[(l21)]->l74),(l245));}else{l245 = ~(0xffffffff>>len);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l108[(l21)]->l75),(l245));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l21,L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l74),(0));}}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l108[(l21)]->l72),(key[0]));assert(len<= 32);l245 = (len == 32)?0xffffffff:~
(0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,
L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l74),(l245));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l108[(l21)]->l86),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,
L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l78),((1<<soc_mem_field_length(l21,
L3_DEFIPm,MODE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l21,L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l79),((1<<soc_mem_field_length(l21,
L3_DEFIPm,MODE_MASK1f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l21,L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l104),((1<<soc_mem_field_length(l21
,L3_DEFIPm,ENTRY_TYPE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),(l108[(l21)]->l105),((1<<
soc_mem_field_length(l21,L3_DEFIPm,ENTRY_TYPE_MASK1f))-1));return(SOC_E_NONE)
;}int _soc_alpm_delete_in_bkt(int l1,soc_mem_t l24,int l246,int l150,void*
l247,uint32*l15,int*l133,int l34){int l140;l140 = soc_mem_alpm_delete(l1,l24,
l246,MEM_BLOCK_ALL,l150,l247,l15,l133);if(SOC_SUCCESS(l140)){return l140;}if(
SOC_ALPM_V6_SCALE_CHECK(l1,l34)){return soc_mem_alpm_delete(l1,l24,l246+1,
MEM_BLOCK_ALL,l150,l247,l15,l133);}return l140;}static int l248(int l1,void*
l8,int bucket_index,int l155,int l133,int l249){alpm_pivot_t*l158;
defip_alpm_ipv4_entry_t l223,l224,l250;defip_alpm_ipv6_64_entry_t l225,l226,
l251;defip_aux_scratch_entry_t l13;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];
soc_mem_t l24;void*l227,*l247,*l228 = NULL;int l161;int l10;int l140 = 
SOC_E_NONE,l212;uint32 l252[5],prefix[5];int l34,l28;uint32 l153;int l246;
uint32 l11,l150;int l154,l26 = 0;trie_t*trie,*l214;uint32 l253;defip_entry_t
lpm_entry,*l254;payload_t*l217 = NULL,*l255 = NULL,*l219 = NULL;trie_node_t*
l216 = NULL,*l157 = NULL;trie_t*l156 = NULL;l10 = l34 = soc_mem_field32_get(
l1,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = soc_mem_field32_get(l1,L3_DEFIPm,
l8,MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(
l1,l8,&l161,&l28));if(l161!= SOC_L3_VRF_OVERRIDE){if(l28 == SOC_VRF_MAX(l1)+1
){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l150);}else{l11 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l150);}l140 = l146(l1,l8,prefix,&l153,&l26);
if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: prefix create failed\n")));return l140;}if(!
soc_alpm_mode_get(l1)){if(l161!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1
,l28,l34)>1){if(l26){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot delete default ""route if other routes are present in "
"this mode"),l28));return SOC_E_PARAM;}}}l11 = 2;}l24 = (l34)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l227 = ((l34)?((uint32*)&(l225)):(
(uint32*)&(l223)));l247 = ((l34)?((uint32*)&(l251)):((uint32*)&(l250)));
SOC_ALPM_LPM_LOCK(l1);if(bucket_index == 0){l140 = l159(l1,l8,l24,l227,&l155,
&bucket_index,&l133,TRUE);}else{l140 = l20(l1,l8,l227,0,l24,0,0);}sal_memcpy(
l247,l227,l34?sizeof(l225):sizeof(l223));if(SOC_FAILURE(l140)){
SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to find prefix for delete\n")));return l140;}l246 = 
bucket_index;l158 = ALPM_TCAM_PIVOT(l1,l155);if(l158 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"del: pivot index %d bkt %d is not valid \n"),
l155,bucket_index));SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_PARAM;}trie = 
PIVOT_BUCKET_TRIE(l158);l140 = trie_delete(trie,prefix,l153,&l216);l217 = (
payload_t*)l216;if(l140!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l140;}if(l34){l214 = VRF_PREFIX_TRIE_IPV6(l1,
l28);}else{l214 = VRF_PREFIX_TRIE_IPV4(l1,l28);}if(l34){l156 = 
VRF_PIVOT_TRIE_IPV6(l1,l28);}else{l156 = VRF_PIVOT_TRIE_IPV4(l1,l28);}if(!l26
){l140 = trie_delete(l214,prefix,l153,&l216);l255 = (payload_t*)l216;if(
SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l256;}if(l249){l157 = NULL;l140 = trie_find_lpm(l214,
prefix,l153,&l157);l219 = (payload_t*)l157;if(SOC_SUCCESS(l140)){payload_t*
l257 = (payload_t*)(l219->bkt_ptr);if(l257!= NULL){l253 = l257->len;}else{
l253 = 0;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l258;}
sal_memcpy(l252,prefix,sizeof(prefix));do{if(!(l34)){l252[0] = (((32-l153) == 
32)?0:(l252[1])<<(32-l153));l252[1] = 0;}else{int l241 = 64-l153;int l242;if(
l241<32){l242 = l252[3]<<l241;l242|= (((32-l241) == 32)?0:(l252[4])>>(32-l241
));l252[0] = l252[4]<<l241;l252[1] = l242;l252[2] = l252[3] = l252[4] = 0;}
else{l252[1] = (((l241-32) == 32)?0:(l252[4])<<(l241-32));l252[0] = l252[2] = 
l252[3] = l252[4] = 0;}}}while(0);l140 = l30(l1,l252,l253,l28,l10,&lpm_entry,
0,1);l212 = l159(l1,&lpm_entry,l24,l227,&l155,&bucket_index,&l133,TRUE);(void
)l27(l1,l227,l24,l10,l161,bucket_index,0,&lpm_entry);(void)l30(l1,l252,l153,
l28,l10,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(l140)){
l228 = ((l34)?((uint32*)&(l226)):((uint32*)&(l224)));l212 = soc_mem_read(l1,
l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l133),l228);}}if((l253 == 0)&&
SOC_FAILURE(l212)){l254 = l34?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l28):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l28);sal_memcpy(&lpm_entry,l254,sizeof(
lpm_entry));l140 = l30(l1,l252,l153,l28,l10,&lpm_entry,0,1);}if(SOC_FAILURE(
l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l258;}l254 = 
&lpm_entry;}}else{l157 = NULL;l140 = trie_find_lpm(l214,prefix,l153,&l157);
l219 = (payload_t*)l157;if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),l28));goto l256;}l219->bkt_ptr = NULL;l253 = 
0;l254 = l34?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l28):VRF_TRIE_DEFAULT_ROUTE_IPV4(
l1,l28);}if(l249){l140 = l9(l1,l254,l34,l11,l253,&l13);if(SOC_FAILURE(l140)){
goto l258;}l140 = _soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l13,TRUE,&l154,&l155,
&bucket_index);if(SOC_FAILURE(l140)){goto l258;}if(SOC_URPF_STATUS_GET(l1)){
uint32 l145;if(l228!= NULL){l145 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l145++;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l145);l145 = soc_mem_field32_get(l1,l24,
l228,SRC_DISCARDf);soc_mem_field32_set(l1,l24,&l13,SRC_DISCARDf,l145);l145 = 
soc_mem_field32_get(l1,l24,l228,DEFAULTROUTEf);soc_mem_field32_set(l1,l24,&
l13,DEFAULTROUTEf,l145);l140 = _soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l13,TRUE
,&l154,&l155,&bucket_index);}if(SOC_FAILURE(l140)){goto l258;}}}sal_free(l217
);if(!l26){sal_free(l255);}PIVOT_BUCKET_ENT_CNT_UPDATE(l158);if((l158->len!= 
0)&&(trie->trie == NULL)){uint32 l259[5];sal_memcpy(l259,l158->key,sizeof(
l259));do{if(!(l10)){l259[0] = (((32-l158->len) == 32)?0:(l259[1])<<(32-l158
->len));l259[1] = 0;}else{int l241 = 64-l158->len;int l242;if(l241<32){l242 = 
l259[3]<<l241;l242|= (((32-l241) == 32)?0:(l259[4])>>(32-l241));l259[0] = 
l259[4]<<l241;l259[1] = l242;l259[2] = l259[3] = l259[4] = 0;}else{l259[1] = 
(((l241-32) == 32)?0:(l259[4])<<(l241-32));l259[0] = l259[2] = l259[3] = l259
[4] = 0;}}}while(0);l30(l1,l259,l158->len,l28,l10,&lpm_entry,0,1);l140 = l7(
l1,&lpm_entry);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1
,"_soc_alpm_delete: Unable to ""delete pivot 0x%08x 0x%08x \n"),l158->key[0],
l158->key[1]));}}l140 = _soc_alpm_delete_in_bkt(l1,l24,l246,l150,l247,l15,&
l133,l34);if(!SOC_SUCCESS(l140)){SOC_ALPM_LPM_UNLOCK(l1);l140 = SOC_E_FAIL;
return l140;}if(SOC_URPF_STATUS_GET(l1)){l140 = soc_mem_alpm_delete(l1,l24,
SOC_ALPM_RPF_BKT_IDX(l1,l246),MEM_BLOCK_ALL,l150,l247,l15,&l154);if(!
SOC_SUCCESS(l140)){SOC_ALPM_LPM_UNLOCK(l1);l140 = SOC_E_FAIL;return l140;}}if
((l158->len!= 0)&&(trie->trie == NULL)){l140 = alpm_bucket_release(l1,
PIVOT_BUCKET_INDEX(l158),l34);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(l158)));}l140 = trie_delete(l156,l158->key,l158->len,&l216
);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l158));sal_free(PIVOT_BUCKET_HANDLE(l158));sal_free(l158);
_soc_trident2_alpm_bkt_view_set(l1,l246<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l34)){_soc_trident2_alpm_bkt_view_set(l1,(l246+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l28,l34);if(VRF_TRIE_ROUTES_CNT(l1,
l28,l34) == 0){l140 = l33(l1,l28,l34);}SOC_ALPM_LPM_UNLOCK(l1);return l140;
l258:l212 = trie_insert(l214,prefix,NULL,l153,(trie_node_t*)l255);if(
SOC_FAILURE(l212)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into pfx trie\n")
,prefix[0],prefix[1]));}l256:l212 = trie_insert(trie,prefix,NULL,l153,(
trie_node_t*)l217);if(SOC_FAILURE(l212)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into bkt trie\n"),prefix[0],prefix[1]));}
SOC_ALPM_LPM_UNLOCK(l1);return l140;}int soc_alpm_ipmc_war(int l21,int l260){
int l183,l10 = 0;int index = -1;defip_entry_t lpm_entry;soc_mem_t l24 = 
L3_DEFIPm;int count = 1;int l261 = 63;if(!soc_property_get(l21,
spn_L3_ALPM_ENABLE,0)){return SOC_E_NONE;}if(!SOC_IS_TRIDENT2PLUS(l21)){
return SOC_E_NONE;}if(SOC_WARM_BOOT(l21)){return SOC_E_NONE;}if(
soc_mem_index_count(l21,L3_DEFIP_PAIR_128m)<= 0){count = 2;}l261 = 
soc_mem_field_length(l21,ING_ACTIVE_L3_IIF_PROFILEm,RPA_ID_PROFILEf);for(l183
= 0;l183<count;l183++){sal_memset(&lpm_entry,0,soc_mem_entry_words(l21,l24)*4
);l10 = l183;soc_mem_field32_set(l21,l24,&lpm_entry,VALID0f,1);
soc_mem_field32_set(l21,l24,&lpm_entry,MODE_MASK0f,3);soc_mem_field32_set(l21
,l24,&lpm_entry,ENTRY_TYPE_MASK0f,1);soc_mem_field32_set(l21,l24,&lpm_entry,
MULTICAST_ROUTE0f,1);soc_mem_field32_set(l21,l24,&lpm_entry,GLOBAL_ROUTE0f,1)
;soc_mem_field32_set(l21,l24,&lpm_entry,RPA_ID0f,l261-1);soc_mem_field32_set(
l21,l24,&lpm_entry,EXPECTED_L3_IIF0f,16383);if(l10){soc_mem_field32_set(l21,
l24,&lpm_entry,VALID1f,1);soc_mem_field32_set(l21,l24,&lpm_entry,MODE0f,1);
soc_mem_field32_set(l21,l24,&lpm_entry,MODE1f,1);soc_mem_field32_set(l21,l24,
&lpm_entry,MODE_MASK1f,3);soc_mem_field32_set(l21,l24,&lpm_entry,
ENTRY_TYPE_MASK1f,1);soc_mem_field32_set(l21,l24,&lpm_entry,MULTICAST_ROUTE1f
,1);soc_mem_field32_set(l21,l24,&lpm_entry,GLOBAL_ROUTE1f,1);
soc_mem_field32_set(l21,l24,&lpm_entry,RPA_ID1f,l261-1);soc_mem_field32_set(
l21,l24,&lpm_entry,EXPECTED_L3_IIF1f,16383);soc_mem_field32_set(l21,l24,&
lpm_entry,IP_ADDR0f,0);soc_mem_field32_set(l21,l24,&lpm_entry,IP_ADDR_MASK0f,
0);soc_mem_field32_set(l21,l24,&lpm_entry,IP_ADDR1f,0xff000000);
soc_mem_field32_set(l21,l24,&lpm_entry,IP_ADDR_MASK1f,0xff000000);}else{
soc_mem_field32_set(l21,l24,&lpm_entry,IP_ADDR0f,0xe0000000);
soc_mem_field32_set(l21,l24,&lpm_entry,IP_ADDR_MASK0f,0xe0000000);}if(l260){
SOC_IF_ERROR_RETURN(l4(l21,&lpm_entry,&index,0));}else{SOC_IF_ERROR_RETURN(l7
(l21,&lpm_entry));}}return SOC_E_NONE;}int soc_alpm_init(int l1){int l183;int
l140 = SOC_E_NONE;l140 = l2(l1);SOC_IF_ERROR_RETURN(l140);l140 = l19(l1);
alpm_vrf_handle[l1] = sal_alloc((MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t),
"ALPM VRF Handles");if(alpm_vrf_handle[l1] == NULL){return SOC_E_MEMORY;}
tcam_pivot[l1] = sal_alloc(MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*),
"ALPM pivots");if(tcam_pivot[l1] == NULL){return SOC_E_MEMORY;}sal_memset(
alpm_vrf_handle[l1],0,(MAX_VRF_ID+1)*sizeof(alpm_vrf_handle_t));sal_memset(
tcam_pivot[l1],0,MAX_PIVOT_COUNT*sizeof(alpm_pivot_t*));for(l183 = 0;l183<
MAX_PIVOT_COUNT;l183++){ALPM_TCAM_PIVOT(l1,l183) = NULL;}if(SOC_CONTROL(l1)->
alpm_bulk_retry == NULL){SOC_CONTROL(l1)->alpm_bulk_retry = sal_sem_create(
"ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_lookup_retry == 
NULL){SOC_CONTROL(l1)->alpm_lookup_retry = sal_sem_create("ALPM lookup retry"
,sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_insert_retry == NULL){
SOC_CONTROL(l1)->alpm_insert_retry = sal_sem_create("ALPM insert retry",
sal_sem_BINARY,0);}if(SOC_CONTROL(l1)->alpm_delete_retry == NULL){SOC_CONTROL
(l1)->alpm_delete_retry = sal_sem_create("ALPM delete retry",sal_sem_BINARY,0
);}l140 = soc_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l140);return l140;}
static int l262(int l1){int l183,l140;alpm_pivot_t*l145;for(l183 = 0;l183<
MAX_PIVOT_COUNT;l183++){l145 = ALPM_TCAM_PIVOT(l1,l183);if(l145){if(
PIVOT_BUCKET_HANDLE(l145)){if(PIVOT_BUCKET_TRIE(l145)){l140 = trie_traverse(
PIVOT_BUCKET_TRIE(l145),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l140)){trie_destroy(PIVOT_BUCKET_TRIE(l145));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Unable to clear trie state for unit %d\n"),l1
));return l140;}}sal_free(PIVOT_BUCKET_HANDLE(l145));}sal_free(
ALPM_TCAM_PIVOT(l1,l183));ALPM_TCAM_PIVOT(l1,l183) = NULL;}}for(l183 = 0;l183
<= SOC_VRF_MAX(l1)+1;l183++){l140 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l1,
l183),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l140)){
trie_destroy(VRF_PREFIX_TRIE_IPV4(l1,l183));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l1,l183));
return l140;}l140 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l1,l183),
alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l140)){
trie_destroy(VRF_PREFIX_TRIE_IPV6(l1,l183));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v6 pfx trie for vrf %d\n"),l1,l183));
return l140;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l183)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l183));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l183
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l183));}sal_memset(&
alpm_vrf_handle[l1][l183],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l1][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,1,1);
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,2,1);if(SOC_ALPM_BUCKET_BMAP(l1)!= NULL){
sal_free(SOC_ALPM_BUCKET_BMAP(l1));}sal_memset(&soc_alpm_bucket[l1],0,sizeof(
soc_alpm_bucket_t));if(alpm_vrf_handle[l1])sal_free(alpm_vrf_handle[l1]);if(
tcam_pivot[l1])sal_free(tcam_pivot[l1]);alpm_vrf_handle[l1] = NULL;tcam_pivot
[l1] = NULL;return SOC_E_NONE;}int soc_alpm_deinit(int l1){l3(l1);
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_deinit(l1));SOC_IF_ERROR_RETURN(
soc_alpm_128_state_clear(l1));SOC_IF_ERROR_RETURN(l262(l1));if(SOC_CONTROL(l1
)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l1)->alpm_bulk_retry);
SOC_CONTROL(l1)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_lookup_retry
);SOC_CONTROL(l1)->alpm_lookup_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_insert_retry
);SOC_CONTROL(l1)->alpm_insert_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_delete_retry
);SOC_CONTROL(l1)->alpm_delete_retry = NULL;}return SOC_E_NONE;}static int
l263(int l1,int l28,int l34){defip_entry_t*lpm_entry = NULL,l264;int l265;int
index;int l140 = SOC_E_NONE;uint32 key[2] = {0,0};uint32 l153;
alpm_bucket_handle_t*l220 = NULL;alpm_pivot_t*l158 = NULL;payload_t*l255 = 
NULL;trie_t*l266 = NULL,*l267 = NULL;int l268 = 0;trie_t*l269 = NULL;int l270
= 0;l140 = alpm_bucket_assign(l1,&l265,l34);if(SOC_FAILURE(l140)){return l140
;}l220 = sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l220
== NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
l140 = SOC_E_MEMORY;goto l271;}sal_memset(l220,0,sizeof(*l220));l158 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l158 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
l140 = SOC_E_MEMORY;goto l271;}sal_memset(l158,0,sizeof(*l158));
PIVOT_BUCKET_HANDLE(l158) = l220;l255 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l255 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_vrf_add: Unable to allocate memory for "
"pfx trie node \n")));l140 = SOC_E_MEMORY;goto l271;}sal_memset(l255,0,sizeof
(*l255));if(l34 == 0){l140 = trie_init(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(
l1,l28));l269 = VRF_PIVOT_TRIE_IPV4(l1,l28);}else{l140 = trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(l1,l28));l269 = VRF_PIVOT_TRIE_IPV6(l1
,l28);}if(SOC_FAILURE(l140)){goto l271;}if(l34 == 0){l140 = trie_init(
_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l1,l28));l266 = VRF_PREFIX_TRIE_IPV4(
l1,l28);}else{l140 = trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l1,l28
));l266 = VRF_PREFIX_TRIE_IPV6(l1,l28);}if(SOC_FAILURE(l140)){goto l271;}if(
l34){l140 = trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l158));}else{l140 = 
trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l158));}l267 = 
PIVOT_BUCKET_TRIE(l158);if(SOC_FAILURE(l140)){goto l271;}PIVOT_BUCKET_INDEX(
l158) = l265;PIVOT_BUCKET_VRF(l158) = l28;PIVOT_BUCKET_IPV6(l158) = l34;
PIVOT_BUCKET_DEF(l158) = TRUE;l153 = 0;l158->key[0] = l255->key[0] = key[0];
l158->key[1] = l255->key[1] = key[1];l158->len = l255->len = l153;l140 = 
trie_insert(l266,key,NULL,l153,&(l255->node));if(SOC_FAILURE(l140)){goto l271
;}l268 = 1;l140 = trie_insert(l269,key,NULL,l153,(trie_node_t*)l158);if(
SOC_FAILURE(l140)){goto l271;}l270 = 1;(void)l30(l1,key,0,l28,l34,&l264,0,1);
if(l28 == SOC_VRF_MAX(l1)+1){soc_L3_DEFIPm_field32_set(l1,&l264,
GLOBAL_ROUTE0f,1);}else{soc_L3_DEFIPm_field32_set(l1,&l264,DEFAULT_MISS0f,1);
}soc_L3_DEFIPm_field32_set(l1,&l264,ALG_BKT_PTR0f,l265);lpm_entry = sal_alloc
(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n")));l140 = 
SOC_E_MEMORY;goto l271;}sal_memcpy(lpm_entry,&l264,sizeof(l264));l140 = l4(l1
,&l264,&index,0);if(SOC_FAILURE(l140)){goto l271;}if(l34 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l28) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l28) = lpm_entry;}index = 
soc_alpm_physical_idx(l1,L3_DEFIPm,index,l34);if(l34 == 0){ALPM_TCAM_PIVOT(l1
,index) = l158;PIVOT_TCAM_INDEX(l158) = index;}else{ALPM_TCAM_PIVOT(l1,index
<<1) = l158;PIVOT_TCAM_INDEX(l158) = index<<1;}VRF_PIVOT_REF_INC(l1,l28,l34);
VRF_TRIE_INIT_DONE(l1,l28,l34,1);return SOC_E_NONE;l271:if(lpm_entry){
sal_free(lpm_entry);}if(l270){trie_node_t*l216 = NULL;(void)trie_delete(l269,
key,l153,&l216);}if(l268){trie_node_t*l216 = NULL;(void)trie_delete(l266,key,
l153,&l216);}if(l267){(void)trie_destroy(l267);PIVOT_BUCKET_TRIE(l158) = NULL
;}if(l266){(void)trie_destroy(l266);if(l34 == 0){VRF_PREFIX_TRIE_IPV4(l1,l28)
= NULL;}else{VRF_PREFIX_TRIE_IPV6(l1,l28) = NULL;}}if(l269){(void)
trie_destroy(l269);if(l34 == 0){VRF_PIVOT_TRIE_IPV4(l1,l28) = NULL;}else{
VRF_PIVOT_TRIE_IPV6(l1,l28) = NULL;}}if(l255){sal_free(l255);}if(l158){
sal_free(l158);}if(l220){sal_free(l220);}(void)alpm_bucket_release(l1,l265,
l34);return l140;}static int l33(int l1,int l28,int l34){defip_entry_t*
lpm_entry;int l265;int l142;int l140 = SOC_E_NONE;uint32 key[2] = {0,0},l272[
SOC_MAX_MEM_FIELD_WORDS];payload_t*l217;alpm_pivot_t*l273;trie_node_t*l216;
trie_t*l274;trie_t*l269 = NULL;if(l34 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l28);}else{lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l28);}l265 = soc_L3_DEFIPm_field32_get(l1,
lpm_entry,ALG_BKT_PTR0f);l140 = alpm_bucket_release(l1,l265,l34);
_soc_trident2_alpm_bkt_view_set(l1,l265<<2,INVALIDm);l140 = l18(l1,lpm_entry,
(void*)l272,&l142);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_vrf_delete: unable to get internal"
" pivot idx for vrf %d/%d\n"),l28,l34));l142 = -1;}l142 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l142,l34);if(l34 == 0){l273 = 
ALPM_TCAM_PIVOT(l1,l142);}else{l273 = ALPM_TCAM_PIVOT(l1,l142<<1);}l140 = l7(
l1,lpm_entry);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),l28,l34));}sal_free(lpm_entry);if(
l34 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l28) = NULL;l274 = 
VRF_PREFIX_TRIE_IPV4(l1,l28);VRF_PREFIX_TRIE_IPV4(l1,l28) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l28) = NULL;l274 = VRF_PREFIX_TRIE_IPV6(l1,l28
);VRF_PREFIX_TRIE_IPV6(l1,l28) = NULL;}VRF_TRIE_INIT_DONE(l1,l28,l34,0);l140 = 
trie_delete(l274,key,0,&l216);l217 = (payload_t*)l216;if(SOC_FAILURE(l140)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal default for vrf "" %d/%d\n"),l28,l34));}sal_free(
l217);(void)trie_destroy(l274);if(l34 == 0){l269 = VRF_PIVOT_TRIE_IPV4(l1,l28
);VRF_PIVOT_TRIE_IPV4(l1,l28) = NULL;}else{l269 = VRF_PIVOT_TRIE_IPV6(l1,l28)
;VRF_PIVOT_TRIE_IPV6(l1,l28) = NULL;}l216 = NULL;l140 = trie_delete(l269,key,
0,&l216);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l28,l34));}(void)
trie_destroy(l269);(void)trie_destroy(PIVOT_BUCKET_TRIE(l273));sal_free(
PIVOT_BUCKET_HANDLE(l273));sal_free(l273);return l140;}int soc_alpm_insert(
int l1,void*l5,uint32 l25,int l275,int l276){defip_alpm_ipv4_entry_t l223,
l224;defip_alpm_ipv6_64_entry_t l225,l226;soc_mem_t l24;void*l227,*l247;int
l161,l28;int index;int l10;int l140 = SOC_E_NONE;uint32 l26;int l277 = 0;l10 = 
soc_mem_field32_get(l1,L3_DEFIPm,l5,MODE0f);l24 = (l10)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l227 = ((l10)?((uint32*)&(l225)):(
(uint32*)&(l223)));l247 = ((l10)?((uint32*)&(l226)):((uint32*)&(l224)));
SOC_IF_ERROR_RETURN(l20(l1,l5,l227,l247,l24,l25,&l26));SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l5,&l161,&l28));if(soc_feature(l1,
soc_feature_ipmc_defip)){l277 = soc_mem_field32_get(l1,L3_DEFIPm,l5,
MULTICAST_ROUTE0f);}if((l161 == SOC_L3_VRF_OVERRIDE)||(l277)){l140 = l4(l1,l5
,&index,0);if(SOC_SUCCESS(l140)){VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l10);
VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l10);}else if(l140 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID,l10);}return(l140);}else if(l28 == 0){if(
soc_alpm_mode_get(l1)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}}if(l161!= 
SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l1)){if(VRF_TRIE_ROUTES_CNT(l1,l28,
l10) == 0){if(!l26){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
l161));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l28,l10)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_insert:VRF %d is not "
"initialized\n"),l28));l140 = l263(l1,l28,l10);if(SOC_FAILURE(l140)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n"),l28,l10));return l140;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),l28,l10));}if(l276&
SOC_ALPM_LOOKUP_HIT){l140 = l162(l1,l5,l227,l247,l24,l275);}else{if(l275 == -
1){l275 = 0;}l140 = l207(l1,l5,l24,l227,l247,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l275),l276);}if(l140!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l1,soc_errmsg(l140)
));}return(l140);}int soc_alpm_lookup(int l1,void*l8,void*l15,int*l16,int*
l278){defip_alpm_ipv4_entry_t l223;defip_alpm_ipv6_64_entry_t l225;soc_mem_t
l24;int bucket_index;int l155;void*l227;int l161,l28;int l10,l132;int l140 = 
SOC_E_NONE;int l277 = 0;l10 = soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE0f);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,l8,&l161,&l28));l140 = l14(l1,l8,
l15,l16,&l132,&l10);if(SOC_SUCCESS(l140)){if(!l10&&(*l16&0x1)){l140 = 
soc_alpm_lpm_ip4entry1_to_0(l1,l15,l15,PRESERVE_HIT);}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l15,&l161,&l28));if(soc_feature(l1,
soc_feature_ipmc_defip)){l277 = soc_mem_field32_get(l1,L3_DEFIPm,l15,
MULTICAST_ROUTE0f);}if((l161 == SOC_L3_VRF_OVERRIDE)||(l277)){return
SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l28,l10)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_lookup:VRF %d is not initialized\n")
,l28));*l278 = 0;return SOC_E_NOT_FOUND;}l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l227 = ((l10)?((uint32*)&(l225)):((uint32*)&(l223)));
SOC_ALPM_LPM_LOCK(l1);l140 = l159(l1,l8,l24,l227,&l155,&bucket_index,l16,TRUE
);SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l140)){*l278 = l155;*l16 = 
bucket_index<<2;return l140;}l140 = l27(l1,l227,l24,l10,l161,bucket_index,*
l16,l15);*l278 = SOC_ALPM_LOOKUP_HIT|l155;return(l140);}static int l279(int l1
,void*l8,void*l15,int l28,int*l155,int*bucket_index,int*l133,int l280){int
l140 = SOC_E_NONE;int l183,l281,l34,l154 = 0;uint32 l11,l150;
defip_aux_scratch_entry_t l13;int l282,l283;int index;soc_mem_t l24,l284;int
l285,l286;int l287;uint32 l288[SOC_MAX_MEM_FIELD_WORDS] = {0};int l289 = -1;
int l290 = 0;soc_field_t l291[2] = {IP_ADDR0f,IP_ADDR1f,};l284 = L3_DEFIPm;
l34 = soc_mem_field32_get(l1,l284,l8,MODE0f);l282 = soc_mem_field32_get(l1,
l284,l8,GLOBAL_ROUTE0f);l283 = soc_mem_field32_get(l1,l284,l8,VRF_ID_0f);
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d:\n"),l28 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l34,l282,l283));if(l28 == SOC_L3_VRF_GLOBAL){l11 = l280?1:0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l150);soc_mem_field32_set(l1,l284,l8,
GLOBAL_ROUTE0f,1);soc_mem_field32_set(l1,l284,l8,VRF_ID_0f,0);}else{l11 = 
l280?3:2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l150);}sal_memset(&l13,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l1,l8,l34,l11,0,&l13));if(
l28 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l1,l284,l8,GLOBAL_ROUTE0f,l282)
;soc_mem_field32_set(l1,l284,l8,VRF_ID_0f,l283);}SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l13,TRUE,&l154,l155,bucket_index));if(l154
== 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Could not find bucket\n")))
;return SOC_E_NOT_FOUND;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in memory %s, index %d, ""bucket_index %d\n"),SOC_MEM_NAME(l1,l284),
soc_alpm_logical_idx(l1,l284,(*l155)>>1,1),*bucket_index));l24 = (l34)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l140 = l139(l1,l8,&l286);if(
SOC_FAILURE(l140)){return l140;}l287 = 24;if(l34){if(SOC_ALPM_V6_SCALE_CHECK(
l1,l34)){l287 = 32;}else{l287 = 16;}}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Start searching mem %s bucket %d(count %d) ""for Length %d\n"),
SOC_MEM_NAME(l1,l24),*bucket_index,l287,l286));for(l183 = 0;l183<l287;l183++)
{uint32 l227[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l245[2] = {0};uint32 l292[
2] = {0};uint32 l293[2] = {0};int l294;l140 = _soc_alpm_mem_index(l1,l24,*
bucket_index,l183,l150,&index);if(l140 == SOC_E_FULL){continue;}
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l24,MEM_BLOCK_ANY,index,(void*)l227));
l294 = soc_mem_field32_get(l1,l24,l227,VALIDf);l285 = soc_mem_field32_get(l1,
l24,l227,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket %5d Index %6d: Valid %d, Length %d\n"),*bucket_index,index,l294,l285)
);if(!l294||(l285>l286)){continue;}SHR_BITSET_RANGE(l245,(l34?64:32)-l285,
l285);(void)soc_mem_field_get(l1,l24,(uint32*)l227,KEYf,(uint32*)l292);l293[1
] = soc_mem_field32_get(l1,l284,l8,l291[1]);l293[0] = soc_mem_field32_get(l1,
l284,l8,l291[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tmask %08x %08x\n\t key %08x %08x\n""\thost %08x %08x\n"),l245[1],l245[0],
l292[1],l292[0],l293[1],l293[0]));for(l281 = l34?1:0;l281>= 0;l281--){if((
l293[l281]&l245[l281])!= (l292[l281]&l245[l281])){break;}}if(l281>= 0){
continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket %d, ""index %d\n"),SOC_MEM_NAME(l1,l24),*
bucket_index,index));if(l289 == -1||l289<l285){l289 = l285;l290 = index;
sal_memcpy(l288,l227,sizeof(l227));}}if(l289!= -1){l140 = l27(l1,l288,l24,l34
,l28,*bucket_index,l290,l15);if(SOC_SUCCESS(l140)){*l133 = l290;if(bsl_check(
bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){LOG_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"Hit mem %s bucket %d, index %d\n"),SOC_MEM_NAME(l1,l24),*
bucket_index,l290));}}return l140;}*l133 = soc_alpm_logical_idx(l1,l284,(*
l155)>>1,1);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Miss in mem %s bucket %d, use associate data ""in mem %s LOG index %d\n"),
SOC_MEM_NAME(l1,l24),*bucket_index,SOC_MEM_NAME(l1,l284),*l133));
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l284,MEM_BLOCK_ANY,*l133,(void*)l15));if(
(!l34)&&((*l155)&1)){l140 = soc_alpm_lpm_ip4entry1_to_0(l1,l15,l15,
PRESERVE_HIT);}return SOC_E_NONE;}int soc_alpm_find_best_match(int l1,void*l8
,void*l15,int*l16,int l280){int l140 = SOC_E_NONE;int l183,l295,l296;
defip_entry_t l297;uint32 l298[2];uint32 l292[2];uint32 l299[2];uint32 l293[2
];uint32 l300,l301;int l161,l28 = 0;int l302[2] = {0};int l155,bucket_index;
soc_mem_t l284 = L3_DEFIPm;int l199,l34,l303,l304 = 0;soc_field_t l305[] = {
GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l306[] = {GLOBAL_ROUTE0f,
GLOBAL_ROUTE1f};l34 = soc_mem_field32_get(l1,l284,l8,MODE0f);if(!
SOC_URPF_STATUS_GET(l1)&&l280){return SOC_E_PARAM;}l295 = soc_mem_index_min(
l1,l284);l296 = soc_mem_index_count(l1,l284);if(SOC_URPF_STATUS_GET(l1)){l296
>>= 1;}if(soc_alpm_mode_get(l1)){l296>>= 1;l295+= l296;}if(l280){l295+= 
soc_mem_index_count(l1,l284)/2;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Launch LPM searching from index %d count %d\n"),l295,l296));for(l183 = l295;
l183<l295+l296;l183++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l284,MEM_BLOCK_ANY
,l183,(void*)&l297));l302[0] = soc_mem_field32_get(l1,l284,&l297,VALID0f);
l302[1] = soc_mem_field32_get(l1,l284,&l297,VALID1f);if(l302[0] == 0&&l302[1]
== 0){continue;}l303 = soc_mem_field32_get(l1,l284,&l297,MODE0f);if(l303!= 
l34){continue;}for(l199 = 0;l199<(l34?1:2);l199++){if(l302[l199] == 0){
continue;}l300 = soc_mem_field32_get(l1,l284,&l297,l305[l199]);l301 = 
soc_mem_field32_get(l1,l284,&l297,l306[l199]);if(!l300||!l301){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Match a Global High route: ent %d\n"),l199));l298[0] = soc_mem_field32_get(
l1,l284,&l297,IP_ADDR_MASK0f);l298[1] = soc_mem_field32_get(l1,l284,&l297,
IP_ADDR_MASK1f);l292[0] = soc_mem_field32_get(l1,l284,&l297,IP_ADDR0f);l292[1
] = soc_mem_field32_get(l1,l284,&l297,IP_ADDR1f);l299[0] = 
soc_mem_field32_get(l1,l284,l8,IP_ADDR_MASK0f);l299[1] = soc_mem_field32_get(
l1,l284,l8,IP_ADDR_MASK1f);l293[0] = soc_mem_field32_get(l1,l284,l8,IP_ADDR0f
);l293[1] = soc_mem_field32_get(l1,l284,l8,IP_ADDR1f);LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\thmsk %08x %08x\n\thkey %08x %08x\n"
"\tsmsk %08x %08x\n\tskey %08x %08x\n"),l298[1],l298[0],l292[1],l292[0],l299[
1],l299[0],l293[1],l293[0]));if(l34&&(((l298[1]&l299[1])!= l298[1])||((l298[0
]&l299[0])!= l298[0]))){continue;}if(!l34&&((l298[l199]&l299[0])!= l298[l199]
)){continue;}if(l34&&((l293[0]&l298[0]) == (l292[0]&l298[0]))&&((l293[1]&l298
[1]) == (l292[1]&l298[1]))){l304 = TRUE;break;}if(!l34&&((l293[0]&l298[l199])
== (l292[l199]&l298[l199]))){l304 = TRUE;break;}}if(!l304){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit Global High route in index = %d(%d)\n"),l183,l199));sal_memcpy(l15,&l297
,sizeof(l297));if(!l34&&l199 == 1){l140 = soc_alpm_lpm_ip4entry1_to_0(l1,l15,
l15,PRESERVE_HIT);}*l16 = l183;return l140;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for VRF and Global Low routes\n")));SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l1,l8,&l161,&l28));l140 = l279(l1,l8,l15,l28,&l155,&
bucket_index,l16,l280);if(l140 == SOC_E_NOT_FOUND){l28 = SOC_L3_VRF_GLOBAL;
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Not found in VRF region, trying Global ""region\n")));l140 = l279(l1,l8,l15,
l28,&l155,&bucket_index,l16,l280);}if(SOC_SUCCESS(l140)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit in %s region in TCAM index %d, "
"buckekt_index %d\n"),l28 == SOC_L3_VRF_GLOBAL?"Global Low":"VRF",l155,
bucket_index));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Search miss for given address\n")));}return(l140);}int soc_alpm_delete(int l1
,void*l8,int l275,int l276){int l161,l28;int l10;int l140 = SOC_E_NONE;int
l249 = 0;int l277 = 0;l10 = soc_mem_field32_get(l1,L3_DEFIPm,l8,MODE0f);if(
soc_feature(l1,soc_feature_ipmc_defip)){l277 = soc_mem_field32_get(l1,
L3_DEFIPm,l8,MULTICAST_ROUTE0f);}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,
l8,&l161,&l28));if((l161 == SOC_L3_VRF_OVERRIDE)||(l277)){l140 = l7(l1,l8);if
(SOC_SUCCESS(l140)){VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l10);VRF_TRIE_ROUTES_DEC(
l1,MAX_VRF_ID,l10);}return(l140);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l28,l10
)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_delete:VRF %d/%d is not initialized\n"),l28,l10));return SOC_E_NONE
;}if(l275 == -1){l275 = 0;}l249 = !(l276&SOC_ALPM_DELETE_ALL);l140 = l248(l1,
l8,SOC_ALPM_BKT_ENTRY_TO_IDX(l275),l276&~SOC_ALPM_LOOKUP_HIT,l275,l249);}
return(l140);}static int l19(int l1){int l307;l307 = soc_mem_index_count(l1,
L3_DEFIPm)+soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)*2;if(
SOC_URPF_STATUS_GET(l1)){l307>>= 1;}SOC_ALPM_BUCKET_COUNT(l1) = l307*2;
SOC_ALPM_BUCKET_BMAP_SIZE(l1) = SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(l1));
SOC_ALPM_BUCKET_BMAP(l1) = sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l1),
"alpm_shared_bucket_bitmap");if(SOC_ALPM_BUCKET_BMAP(l1) == NULL){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_shared_mem_init: Memory allocation for "
"bucket bitmap management failed\n")));return SOC_E_MEMORY;}sal_memset(
SOC_ALPM_BUCKET_BMAP(l1),0,SOC_ALPM_BUCKET_BMAP_SIZE(l1));alpm_bucket_assign(
l1,&l307,1);return SOC_E_NONE;}int alpm_bucket_assign(int l1,int*l265,int l34
){int l183,l308 = 1,l309 = 0;if(l34){if(!soc_alpm_mode_get(l1)&&!
SOC_URPF_STATUS_GET(l1)){l308 = 2;}}for(l183 = 0;l183<SOC_ALPM_BUCKET_COUNT(
l1);l183+= l308){SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l183,l308,l309);
if(0 == l309){break;}}if(l183 == SOC_ALPM_BUCKET_COUNT(l1)){return SOC_E_FULL
;}SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l183,l308);*l265 = l183;
SOC_ALPM_BUCKET_NEXT_FREE(l1) = l183;return SOC_E_NONE;}int
alpm_bucket_release(int l1,int l265,int l34){int l308 = 1,l309 = 0;if((l265<1
)||(l265>SOC_ALPM_BUCKET_MAX_INDEX(l1))){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Unit %d\n, freeing invalid bucket index %d\n"),l1,l265));
return SOC_E_PARAM;}if(l34){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(
l1)){l308 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l265,l308,l309);if
(!l309){return SOC_E_PARAM;}SHR_BITCLR_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l265,
l308);return SOC_E_NONE;}int alpm_bucket_is_assigned(int l1,int l310,int l10,
int*l309){int l308 = 1;if((l310<1)||(l310>SOC_ALPM_BUCKET_MAX_INDEX(l1))){
return SOC_E_PARAM;}if(l10){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(
l1)){l308 = 2;}}SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l310,l308,*l309);
return SOC_E_NONE;}static void l120(int l1,void*l15,int index,l115 l121){if(
index&(0x8000)){l121[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l15),(l108[(l1)]->l72));l121[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l74));l121[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l15),(l108[(l1)]->l73));l121[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l75));if((!(SOC_IS_HURRICANE(l1)))&&(((l108[(l1)]->l88)!= NULL))){int
l311;(void)soc_alpm_lpm_vrf_get(l1,l15,(int*)&l121[4],&l311);}else{l121[4] = 
0;};}else{if(index&0x1){l121[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(l1)]->l73));l121[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l75));l121[2] = 0;l121[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l108[(l1)]->l89)!= NULL))){int l311;defip_entry_t l312;(void)
soc_alpm_lpm_ip4entry1_to_0(l1,l15,&l312,0);(void)soc_alpm_lpm_vrf_get(l1,&
l312,(int*)&l121[4],&l311);}else{l121[4] = 0;};}else{l121[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l72));l121[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l15),(l108[(l1)]->l74));l121[2] = 0;l121[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l108[(l1)]->l88)!= NULL))){int l311;(void)
soc_alpm_lpm_vrf_get(l1,l15,(int*)&l121[4],&l311);}else{l121[4] = 0;};}}}
static int l313(l115 l117,l115 l118){int l142;for(l142 = 0;l142<5;l142++){{if
((l117[l142])<(l118[l142])){return-1;}if((l117[l142])>(l118[l142])){return 1;
}};}return(0);}static void l314(int l1,void*l5,uint32 l315,uint32 l135,int
l132){l115 l316;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l108[(l1)]->l76))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l108[(l1)]->l87))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l108[(
l1)]->l86))){l316[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l108[(l1)]->l72));l316[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l108[(
l1)]->l74));l316[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l108[(l1)]->l73));l316[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l108[(
l1)]->l75));if((!(SOC_IS_HURRICANE(l1)))&&(((l108[(l1)]->l88)!= NULL))){int
l311;(void)soc_alpm_lpm_vrf_get(l1,l5,(int*)&l316[4],&l311);}else{l316[4] = 0
;};l134((l119[(l1)]),l313,l316,l132,l135,((uint16)l315<<1)|(0x8000));}}else{
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l108
[(l1)]->l86))){l316[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l108[(l1)]->l72));l316[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l108[(
l1)]->l74));l316[2] = 0;l316[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l108[(l1)]->l88)!= NULL))){int l311;(void)soc_alpm_lpm_vrf_get(l1,l5,(int*)&
l316[4],&l311);}else{l316[4] = 0;};l134((l119[(l1)]),l313,l316,l132,l135,((
uint16)l315<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l108[(l1)]->l87))){l316[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l108[(
l1)]->l73));l316[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l108[(l1)]->l75));l316[2] = 0;l316[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l108[(l1)]->l89)!= NULL))){int l311;defip_entry_t
l312;(void)soc_alpm_lpm_ip4entry1_to_0(l1,l5,&l312,0);(void)
soc_alpm_lpm_vrf_get(l1,&l312,(int*)&l316[4],&l311);}else{l316[4] = 0;};l134(
(l119[(l1)]),l313,l316,l132,l135,(((uint16)l315<<1)+1));}}}static void l317(
int l1,void*l8,uint32 l315){l115 l316;int l132 = -1;int l140;uint16 index;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(
l1)]->l76))){l316[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l108[(l1)]->l72));l316[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(
l1)]->l74));l316[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l108[(l1)]->l73));l316[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(
l1)]->l75));if((!(SOC_IS_HURRICANE(l1)))&&(((l108[(l1)]->l88)!= NULL))){int
l311;(void)soc_alpm_lpm_vrf_get(l1,l8,(int*)&l316[4],&l311);}else{l316[4] = 0
;};index = (l315<<1)|(0x8000);}else{l316[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(
l1)]->l72));l316[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l108[(l1)]->l74));l316[2] = 0;l316[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l108[(l1)]->l88)!= NULL))){int l311;(void)
soc_alpm_lpm_vrf_get(l1,l8,(int*)&l316[4],&l311);}else{l316[4] = 0;};index = 
l315;}l140 = l137((l119[(l1)]),l313,l316,l132,index);if(SOC_FAILURE(l140)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\ndel  index: H %d error %d\n"),
index,l140));}}static int l318(int l1,void*l8,int l132,int*l133){l115 l316;
int l319;int l140;uint16 index = (0xFFFF);l319 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(
l1)]->l76));if(l319){l316[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(l1)]->l72));l316[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(
l1)]->l74));l316[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l108[(l1)]->l73));l316[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(
l1)]->l75));if((!(SOC_IS_HURRICANE(l1)))&&(((l108[(l1)]->l88)!= NULL))){int
l311;(void)soc_alpm_lpm_vrf_get(l1,l8,(int*)&l316[4],&l311);}else{l316[4] = 0
;};}else{l316[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l8),(l108[(l1)]->l72));l316[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(
l1)]->l74));l316[2] = 0;l316[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l108[(l1)]->l88)!= NULL))){int l311;(void)soc_alpm_lpm_vrf_get(l1,l8,(int*)&
l316[4],&l311);}else{l316[4] = 0;};}l140 = l129((l119[(l1)]),l313,l316,l132,&
index);if(SOC_FAILURE(l140)){*l133 = 0xFFFFFFFF;return(l140);}*l133 = index;
return(SOC_E_NONE);}static uint16 l122(uint8*l123,int l124){return(
_shr_crc16b(0,l123,l124));}static int l125(int l21,int l110,int l111,l114**
l126){l114*l130;int index;if(l111>l110){return SOC_E_MEMORY;}l130 = sal_alloc
(sizeof(l114),"lpm_hash");if(l130 == NULL){return SOC_E_MEMORY;}sal_memset(
l130,0,sizeof(*l130));l130->l21 = l21;l130->l110 = l110;l130->l111 = l111;
l130->l112 = sal_alloc(l130->l111*sizeof(*(l130->l112)),"hash_table");if(l130
->l112 == NULL){sal_free(l130);return SOC_E_MEMORY;}l130->l113 = sal_alloc(
l130->l110*sizeof(*(l130->l113)),"link_table");if(l130->l113 == NULL){
sal_free(l130->l112);sal_free(l130);return SOC_E_MEMORY;}for(index = 0;index<
l130->l111;index++){l130->l112[index] = (0xFFFF);}for(index = 0;index<l130->
l110;index++){l130->l113[index] = (0xFFFF);}*l126 = l130;return SOC_E_NONE;}
static int l127(l114*l128){if(l128!= NULL){sal_free(l128->l112);sal_free(l128
->l113);sal_free(l128);}return SOC_E_NONE;}static int l129(l114*l130,l116 l131
,l115 entry,int l132,uint16*l133){int l1 = l130->l21;uint16 l320;uint16 index
;l320 = l122((uint8*)entry,(32*5))%l130->l111;index = l130->l112[l320];;;
while(index!= (0xFFFF)){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];l115 l121;int l321
;l321 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,l321,l15));l120(l1,l15,index,l121);if((*l131)(entry,l121) == 0)
{*l133 = (index&(0x7FFF))>>((index&(0x8000))?1:0);;return(SOC_E_NONE);}index = 
l130->l113[index&(0x7FFF)];;};return(SOC_E_NOT_FOUND);}static int l134(l114*
l130,l116 l131,l115 entry,int l132,uint16 l135,uint16 l136){int l1 = l130->
l21;uint16 l320;uint16 index;uint16 l322;l320 = l122((uint8*)entry,(32*5))%
l130->l111;index = l130->l112[l320];;;;l322 = (0xFFFF);if(l135!= (0xFFFF)){
while(index!= (0xFFFF)){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];l115 l121;int l321
;l321 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,l321,l15));l120(l1,l15,index,l121);if((*l131)(entry,l121) == 0)
{if(l136!= index){;if(l322 == (0xFFFF)){l130->l112[l320] = l136;l130->l113[
l136&(0x7FFF)] = l130->l113[index&(0x7FFF)];l130->l113[index&(0x7FFF)] = (
0xFFFF);}else{l130->l113[l322&(0x7FFF)] = l136;l130->l113[l136&(0x7FFF)] = 
l130->l113[index&(0x7FFF)];l130->l113[index&(0x7FFF)] = (0xFFFF);}};return(
SOC_E_NONE);}l322 = index;index = l130->l113[index&(0x7FFF)];;}}l130->l113[
l136&(0x7FFF)] = l130->l112[l320];l130->l112[l320] = l136;return(SOC_E_NONE);
}static int l137(l114*l130,l116 l131,l115 entry,int l132,uint16 l138){uint16
l320;uint16 index;uint16 l322;l320 = l122((uint8*)entry,(32*5))%l130->l111;
index = l130->l112[l320];;;l322 = (0xFFFF);while(index!= (0xFFFF)){if(l138 == 
index){;if(l322 == (0xFFFF)){l130->l112[l320] = l130->l113[l138&(0x7FFF)];
l130->l113[l138&(0x7FFF)] = (0xFFFF);}else{l130->l113[l322&(0x7FFF)] = l130->
l113[l138&(0x7FFF)];l130->l113[l138&(0x7FFF)] = (0xFFFF);}return(SOC_E_NONE);
}l322 = index;index = l130->l113[index&(0x7FFF)];;}return(SOC_E_NOT_FOUND);}
int _ipmask2pfx(uint32 l323,int*l324){*l324 = 0;while(l323&(1<<31)){*l324+= 1
;l323<<= 1;}return((l323)?SOC_E_PARAM:SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_0(int l1,void*l325,void*l326,int l327){uint32 l141;
l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325)
,(l108[(l1)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l326),(l108[(l1)]->l86),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l76),(l141));if(soc_feature(l1,soc_feature_ipmc_defip)){
l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325)
,(l108[(l1)]->l58));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l326),(l108[(l1)]->l58),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l60),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l62));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l62),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l64),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l66),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l68));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l68),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l72));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l72),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l74),(l141));if(((l108[(l1)]->l50)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l50),(l141));}if(((l108[(l1)]->l52)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l52));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l52),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l54));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l54),(l141));}else{l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l80),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l82));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l82),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l84));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l84),(l141));if(((l108[(l1)]->l88)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l88),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l90));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l90),(l141));}if(((l108[(l1)]->l48)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l48),(l141));}if(l327){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l70),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l92),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l94));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l94),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l96),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l98));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l98),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l100));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l100),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l102));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l326),(l108[(l1)]->l102),(l141));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_1(int l1,void*l325,void*l326,int l327){uint32 l141;
l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325)
,(l108[(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l326),(l108[(l1)]->l87),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l77),(l141));if(soc_feature(l1,soc_feature_ipmc_defip)){
l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325)
,(l108[(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l326),(l108[(l1)]->l59),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l61),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l63));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l63),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l65));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l65),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l67));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l67),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l69));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l69),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l73),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l75),(l141));if(((l108[(l1)]->l51)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l51),(l141));}if(((l108[(l1)]->l53)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l53));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l53),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l55));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l55),(l141));}else{l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l81),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l83));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l83),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l85),(l141));if(((l108[(l1)]->l89)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l89),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l91),(l141));}if(((l108[(l1)]->l49)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l49),(l141));}if(l327){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l71));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l71),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l93));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l93),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l95),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l97));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l97),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l99));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l99),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l101));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l101),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l103));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l326),(l108[(l1)]->l103),(l141));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_1(int l1,void*l325,void*l326,int l327){uint32 l141;
l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325)
,(l108[(l1)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l326),(l108[(l1)]->l87),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l77),(l141));if(soc_feature(l1,soc_feature_ipmc_defip)){
l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325)
,(l108[(l1)]->l58));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l326),(l108[(l1)]->l59),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l61),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l62));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l63),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l65),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l67),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l68));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l69),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l72));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l73),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l75),(l141));if(!SOC_IS_HURRICANE(l1)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l51),(l141));}if(((l108[(l1)]->l52)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l52));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l53),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l54));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l55),(l141));}else{l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l81),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l82));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l83),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l84));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l85),(l141));if(((l108[(l1)]->l88)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l89),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l90));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l91),(l141));}if(((l108[(l1)]->l48)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l49),(l141));}if(l327){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l71),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l93),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l94));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l95),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l97),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l98));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l99),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l100));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l101),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l102));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l326),(l108[(l1)]->l103),(l141));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_0(int l1,void*l325,void*l326,int l327){uint32 l141;
l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325)
,(l108[(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l326),(l108[(l1)]->l86),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l76),(l141));if(soc_feature(l1,soc_feature_ipmc_defip)){
l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325)
,(l108[(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l326),(l108[(l1)]->l58),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l60),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l63));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l62),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l65));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l64),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l67));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l66),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l69));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l68),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l72),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l74),(l141));if(!SOC_IS_HURRICANE(l1)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l50),(l141));}if(((l108[(l1)]->l53)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l53));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l52),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l55));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l54),(l141));}else{l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l80),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l83));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l82),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l84),(l141));if(((l108[(l1)]->l89)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l88),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l90),(l141));}if(((l108[(l1)]->l49)!= NULL)){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l48),(l141));}if(l327){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l71));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l70),(l141));}l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l93));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l92),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l94),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l97));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l326),(l108[(l1)]->l96),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[(l1)]->l99));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[
(l1)]->l98),(l141));l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l325),(l108[(l1)]->l101));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l326),(l108[(l1)]->l100),(l141));l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l325),(l108[
(l1)]->l103));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l326),(l108[(l1)]->l102),(l141));return(SOC_E_NONE);}static int l328(int l1
,void*l15){return(SOC_E_NONE);}void soc_alpm_lpm_state_dump(int l1){int l183;
int l329;l329 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l1)){return;}for(l183 = l329;l183>= 0;l183--){if((l183!= (
(3*(64+32+2+1))-1))&&((l44[(l1)][(l183)].l37) == -1)){continue;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l183,(l44
[(l1)][(l183)].l39),(l44[(l1)][(l183)].next),(l44[(l1)][(l183)].l37),(l44[(l1
)][(l183)].l38),(l44[(l1)][(l183)].l40),(l44[(l1)][(l183)].l41)));}
COMPILER_REFERENCE(l328);}static int l330(int l1,int l331,int index,uint32*
l15,int l6){int l332;int l10;uint32 l333;uint32 l334;int l335;uint32 l336[
SOC_MAX_MEM_FIELD_WORDS] = {0};int l337 = 0,l338 = 0;if(!SOC_URPF_STATUS_GET(
l1)){return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l332 = 
(soc_mem_index_count(l1,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l1)){l332 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1)+0x0400;}else{l332 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}l10 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l76));if(l6>= 2){SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,
l331+l332,l336));l337 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l336),(l108[(l1)]->l84));l338 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l336),(l108[
(l1)]->l85));}else{l337 = l6;l338 = l6;}if(((l108[(l1)]->l48)!= NULL)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l48),(0));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm
)),(l15),(l108[(l1)]->l49),(0));}l333 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(l1)]->l74));l334 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l75));if(!l10){if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l15),(l108[(l1)]->l86))){l335 = (!l333)?1:0;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l84),(l337|l335));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l15),(l108[(l1)]->l87))){l335 = (!l334)?1:0;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l85),(l338|l335));}}else{l335 = ((!l333)&&(!l334))?1:0;l333 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l86));l334 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l15),(l108[(l1)]->l87));if(l333&&l334){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l84),(l337|l335));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l15),(l108[(l1)]->l85),(l338|l335));}}return l178(l1,
MEM_BLOCK_ANY,index+l332,index,l15);}static int l339(int l1,int l340,int l341
){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,l340,l15));l314(l1,l15,l341,0x4000,0);SOC_IF_ERROR_RETURN(l178(
l1,MEM_BLOCK_ANY,l341,l340,l15));SOC_IF_ERROR_RETURN(l330(l1,l340,l341,l15,2)
);do{int l342,l343;l342 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l340),1);l343
= soc_alpm_physical_idx((l1),L3_DEFIPm,(l341),1);ALPM_TCAM_PIVOT(l1,l343<<1) = 
ALPM_TCAM_PIVOT(l1,l342<<1);ALPM_TCAM_PIVOT(l1,(l343<<1)+1) = ALPM_TCAM_PIVOT
(l1,(l342<<1)+1);if(ALPM_TCAM_PIVOT((l1),l343<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l1),l343<<1)) = l343<<1;}if(ALPM_TCAM_PIVOT((l1),(l343<<1)+1
)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(l343<<1)+1)) = (l343<<1)+1;}
ALPM_TCAM_PIVOT(l1,l342<<1) = NULL;ALPM_TCAM_PIVOT(l1,(l342<<1)+1) = NULL;}
while(0);return(SOC_E_NONE);}static int l344(int l1,int l132,int l10){uint32
l15[SOC_MAX_MEM_FIELD_WORDS];int l340;int l341;uint32 l345,l346;l341 = (l44[(
l1)][(l132)].l38)+1;if(!l10){l340 = (l44[(l1)][(l132)].l38);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l340,l15));l345 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l86));l346 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l15),(l108[(l1)]->l87));if((l345 == 0)||(l346 == 0)){l314(l1,l15
,l341,0x4000,0);SOC_IF_ERROR_RETURN(l178(l1,MEM_BLOCK_ANY,l341,l340,l15));
SOC_IF_ERROR_RETURN(l330(l1,l340,l341,l15,2));do{int l347 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l340),1)<<1;int l242 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l341),1)*2+(l347&1);if((l346)){l347++;}
ALPM_TCAM_PIVOT((l1),l242) = ALPM_TCAM_PIVOT((l1),l347);if(ALPM_TCAM_PIVOT((
l1),l242)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l242)) = l242;}
ALPM_TCAM_PIVOT((l1),l347) = NULL;}while(0);l341--;}}l340 = (l44[(l1)][(l132)
].l37);if(l340!= l341){SOC_IF_ERROR_RETURN(l339(l1,l340,l341));
VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l10);}(l44[(l1)][(l132)].l37)+= 1;(l44[(l1)
][(l132)].l38)+= 1;return(SOC_E_NONE);}static int l348(int l1,int l132,int l10
){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l340;int l341;int l349;uint32 l345,
l346;l341 = (l44[(l1)][(l132)].l37)-1;if((l44[(l1)][(l132)].l40) == 0){(l44[(
l1)][(l132)].l37) = l341;(l44[(l1)][(l132)].l38) = l341-1;return(SOC_E_NONE);
}if((!l10)&&((l44[(l1)][(l132)].l38)!= (l44[(l1)][(l132)].l37))){l340 = (l44[
(l1)][(l132)].l38);SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l340,
l15));l345 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l15),(l108[(l1)]->l86));l346 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(l1)]->l87));if((l345 == 0)||(l346 == 
0)){l349 = l340-1;SOC_IF_ERROR_RETURN(l339(l1,l349,l341));VRF_PIVOT_SHIFT_INC
(l1,MAX_VRF_ID,l10);l314(l1,l15,l349,0x4000,0);SOC_IF_ERROR_RETURN(l178(l1,
MEM_BLOCK_ANY,l349,l340,l15));SOC_IF_ERROR_RETURN(l330(l1,l340,l349,l15,2));
do{int l347 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l340),1)<<1;int l242 = 
soc_alpm_physical_idx((l1),L3_DEFIPm,(l349),1)*2+(l347&1);if((l346)){l347++;}
ALPM_TCAM_PIVOT((l1),l242) = ALPM_TCAM_PIVOT((l1),l347);if(ALPM_TCAM_PIVOT((
l1),l242)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l242)) = l242;}
ALPM_TCAM_PIVOT((l1),l347) = NULL;}while(0);}else{l314(l1,l15,l341,0x4000,0);
SOC_IF_ERROR_RETURN(l178(l1,MEM_BLOCK_ANY,l341,l340,l15));SOC_IF_ERROR_RETURN
(l330(l1,l340,l341,l15,2));do{int l342,l343;l342 = soc_alpm_physical_idx((l1)
,L3_DEFIPm,(l340),1);l343 = soc_alpm_physical_idx((l1),L3_DEFIPm,(l341),1);
ALPM_TCAM_PIVOT(l1,l343<<1) = ALPM_TCAM_PIVOT(l1,l342<<1);ALPM_TCAM_PIVOT(l1,
(l343<<1)+1) = ALPM_TCAM_PIVOT(l1,(l342<<1)+1);if(ALPM_TCAM_PIVOT((l1),l343<<
1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l343<<1)) = l343<<1;}if(
ALPM_TCAM_PIVOT((l1),(l343<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(
l343<<1)+1)) = (l343<<1)+1;}ALPM_TCAM_PIVOT(l1,l342<<1) = NULL;
ALPM_TCAM_PIVOT(l1,(l342<<1)+1) = NULL;}while(0);}}else{l340 = (l44[(l1)][(
l132)].l38);SOC_IF_ERROR_RETURN(l339(l1,l340,l341));VRF_PIVOT_SHIFT_INC(l1,
MAX_VRF_ID,l10);}(l44[(l1)][(l132)].l37)-= 1;(l44[(l1)][(l132)].l38)-= 1;
return(SOC_E_NONE);}static int l350(int l1,int l132,int l10,void*l15,int*l351
){int l352;int l353;int l354;int l355;int l340;uint32 l345,l346;int l140;if((
l44[(l1)][(l132)].l40) == 0){l355 = ((3*(64+32+2+1))-1);if(soc_alpm_mode_get(
l1) == SOC_ALPM_MODE_PARALLEL){if(l132<= (((3*(64+32+2+1))/3)-1)){l355 = (((3
*(64+32+2+1))/3)-1);}}while((l44[(l1)][(l355)].next)>l132){l355 = (l44[(l1)][
(l355)].next);}l353 = (l44[(l1)][(l355)].next);if(l353!= -1){(l44[(l1)][(l353
)].l39) = l132;}(l44[(l1)][(l132)].next) = (l44[(l1)][(l355)].next);(l44[(l1)
][(l132)].l39) = l355;(l44[(l1)][(l355)].next) = l132;(l44[(l1)][(l132)].l41)
= ((l44[(l1)][(l355)].l41)+1)/2;(l44[(l1)][(l355)].l41)-= (l44[(l1)][(l132)].
l41);(l44[(l1)][(l132)].l37) = (l44[(l1)][(l355)].l38)+(l44[(l1)][(l355)].l41
)+1;(l44[(l1)][(l132)].l38) = (l44[(l1)][(l132)].l37)-1;(l44[(l1)][(l132)].
l40) = 0;}else if(!l10){l340 = (l44[(l1)][(l132)].l37);if((l140 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l340,l15))<0){return l140;}l345 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l86));l346 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l15),(l108[(l1)]->l87));if((l345 == 0)||(l346 == 0)){*l351 = (
l340<<1)+((l346 == 0)?1:0);return(SOC_E_NONE);}l340 = (l44[(l1)][(l132)].l38)
;if((l140 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l340,l15))<0){return l140;}l345 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l15),(l108[(
l1)]->l86));l346 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l15),(l108[(l1)]->l87));if((l345 == 0)||(l346 == 0)){*l351 = (
l340<<1)+((l346 == 0)?1:0);return(SOC_E_NONE);}}l354 = l132;while((l44[(l1)][
(l354)].l41) == 0){l354 = (l44[(l1)][(l354)].next);if(l354 == -1){l354 = l132
;break;}}while((l44[(l1)][(l354)].l41) == 0){l354 = (l44[(l1)][(l354)].l39);
if(l354 == -1){if((l44[(l1)][(l132)].l40) == 0){l352 = (l44[(l1)][(l132)].l39
);l353 = (l44[(l1)][(l132)].next);if(-1!= l352){(l44[(l1)][(l352)].next) = 
l353;}if(-1!= l353){(l44[(l1)][(l353)].l39) = l352;}}return(SOC_E_FULL);}}
while(l354>l132){l353 = (l44[(l1)][(l354)].next);SOC_IF_ERROR_RETURN(l348(l1,
l353,l10));(l44[(l1)][(l354)].l41)-= 1;(l44[(l1)][(l353)].l41)+= 1;l354 = 
l353;}while(l354<l132){SOC_IF_ERROR_RETURN(l344(l1,l354,l10));(l44[(l1)][(
l354)].l41)-= 1;l352 = (l44[(l1)][(l354)].l39);(l44[(l1)][(l352)].l41)+= 1;
l354 = l352;}(l44[(l1)][(l132)].l40)+= 1;(l44[(l1)][(l132)].l41)-= 1;(l44[(l1
)][(l132)].l38)+= 1;*l351 = (l44[(l1)][(l132)].l38)<<((l10)?0:1);sal_memcpy(
l15,soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);
return(SOC_E_NONE);}static int l356(int l1,int l132,int l10,void*l15,int l357
){int l352;int l353;int l340;int l341;uint32 l358[SOC_MAX_MEM_FIELD_WORDS];
uint32 l359[SOC_MAX_MEM_FIELD_WORDS];uint32 l360[SOC_MAX_MEM_FIELD_WORDS];
void*l361;int l140;int l362,l168;l340 = (l44[(l1)][(l132)].l38);l341 = l357;
if(!l10){l341>>= 1;if((l140 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l340,l358))<0){
return l140;}if((l140 = READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
soc_alpm_physical_idx(l1,L3_DEFIPm,l340,1),l359))<0){return l140;}if((l140 = 
READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,soc_alpm_physical_idx(l1,L3_DEFIPm,
l341,1),l360))<0){return l140;}l361 = (l341 == l340)?l358:l15;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l358),(l108[
(l1)]->l87))){l168 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l359,
BPM_LENGTH1f);if(l357&1){l140 = soc_alpm_lpm_ip4entry1_to_1(l1,l358,l361,
PRESERVE_HIT);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l360,BPM_LENGTH1f,
l168);}else{l140 = soc_alpm_lpm_ip4entry1_to_0(l1,l358,l361,PRESERVE_HIT);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l360,BPM_LENGTH0f,l168);}l362 = (
l340<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l358),(l108[(l1)]->l87),(0));}else{l168 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l359,BPM_LENGTH0f);if(l357&1){l140 = 
soc_alpm_lpm_ip4entry0_to_1(l1,l358,l361,PRESERVE_HIT);soc_mem_field32_set(l1
,L3_DEFIP_AUX_TABLEm,l360,BPM_LENGTH1f,l168);}else{l140 = 
soc_alpm_lpm_ip4entry0_to_0(l1,l358,l361,PRESERVE_HIT);soc_mem_field32_set(l1
,L3_DEFIP_AUX_TABLEm,l360,BPM_LENGTH0f,l168);}l362 = l340<<1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l358),(l108[
(l1)]->l86),(0));(l44[(l1)][(l132)].l40)-= 1;(l44[(l1)][(l132)].l41)+= 1;(l44
[(l1)][(l132)].l38)-= 1;}l362 = soc_alpm_physical_idx(l1,L3_DEFIPm,l362,0);
l357 = soc_alpm_physical_idx(l1,L3_DEFIPm,l357,0);ALPM_TCAM_PIVOT(l1,l357) = 
ALPM_TCAM_PIVOT(l1,l362);if(ALPM_TCAM_PIVOT(l1,l357)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l1,l357)) = l357;}ALPM_TCAM_PIVOT(l1,l362) = NULL;if((l140 = 
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ALL,soc_alpm_physical_idx(l1,L3_DEFIPm
,l341,1),l360))<0){return l140;}if(l341!= l340){l314(l1,l361,l341,0x4000,0);
if((l140 = l178(l1,MEM_BLOCK_ANY,l341,l341,l361))<0){return l140;}if((l140 = 
l330(l1,l340,l341,l361,2))<0){return l140;}}l314(l1,l358,l340,0x4000,0);if((
l140 = l178(l1,MEM_BLOCK_ANY,l340,l340,l358))<0){return l140;}if((l140 = l330
(l1,l340,l340,l358,0))<0){return l140;}}else{(l44[(l1)][(l132)].l40)-= 1;(l44
[(l1)][(l132)].l41)+= 1;(l44[(l1)][(l132)].l38)-= 1;if(l341!= l340){if((l140 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l340,l358))<0){return l140;}l314(l1,l358,l341
,0x4000,0);if((l140 = l178(l1,MEM_BLOCK_ANY,l341,l340,l358))<0){return l140;}
if((l140 = l330(l1,l340,l341,l358,2))<0){return l140;}}l357 = 
soc_alpm_physical_idx(l1,L3_DEFIPm,l341,1);l362 = soc_alpm_physical_idx(l1,
L3_DEFIPm,l340,1);ALPM_TCAM_PIVOT(l1,l357<<1) = ALPM_TCAM_PIVOT(l1,l362<<1);
ALPM_TCAM_PIVOT(l1,l362<<1) = NULL;if(ALPM_TCAM_PIVOT(l1,l357<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,l357<<1)) = l357<<1;}sal_memcpy(l358,
soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);l314(l1
,l358,l340,0x4000,0);if((l140 = l178(l1,MEM_BLOCK_ANY,l340,l340,l358))<0){
return l140;}if((l140 = l330(l1,l340,l340,l358,0))<0){return l140;}}if((l44[(
l1)][(l132)].l40) == 0){l352 = (l44[(l1)][(l132)].l39);assert(l352!= -1);l353
= (l44[(l1)][(l132)].next);(l44[(l1)][(l352)].next) = l353;(l44[(l1)][(l352)]
.l41)+= (l44[(l1)][(l132)].l41);(l44[(l1)][(l132)].l41) = 0;if(l353!= -1){(
l44[(l1)][(l353)].l39) = l352;}(l44[(l1)][(l132)].next) = -1;(l44[(l1)][(l132
)].l39) = -1;(l44[(l1)][(l132)].l37) = -1;(l44[(l1)][(l132)].l38) = -1;}
return(l140);}int soc_alpm_lpm_vrf_get(int l21,void*lpm_entry,int*l28,int*
l363){int l161;if(((l108[(l21)]->l90)!= NULL)){l161 = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,VRF_ID_0f);*l363 = l161;if(
soc_L3_DEFIPm_field32_get(l21,lpm_entry,VRF_ID_MASK0f)){*l28 = l161;}else if(
!soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry)
,(l108[(l21)]->l92))){*l28 = SOC_L3_VRF_GLOBAL;*l363 = SOC_VRF_MAX(l21)+1;}
else{*l28 = SOC_L3_VRF_OVERRIDE;}}else{*l28 = SOC_L3_VRF_DEFAULT;}return(
SOC_E_NONE);}static int l35(int l1,void*entry,int*l17){int l132;int l140;int
l10;uint32 l141;int l161;int l364;l10 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l108[(l1)]->l76));if(l10){l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l108
[(l1)]->l74));if((l140 = _ipmask2pfx(l141,&l132))<0){return(l140);}l141 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l108
[(l1)]->l75));if(l132){if(l141!= 0xffffffff){return(SOC_E_PARAM);}l132+= 32;}
else{if((l140 = _ipmask2pfx(l141,&l132))<0){return(l140);}}l132+= 33;}else{
l141 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry
),(l108[(l1)]->l74));if((l140 = _ipmask2pfx(l141,&l132))<0){return(l140);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l1,entry,&l161,&l140));l364 = 
soc_alpm_mode_get(l1);switch(l161){case SOC_L3_VRF_GLOBAL:if(l364 == 
SOC_ALPM_MODE_PARALLEL){*l17 = l132+((3*(64+32+2+1))/3);}else{*l17 = l132;}
break;case SOC_L3_VRF_OVERRIDE:*l17 = l132+2*((3*(64+32+2+1))/3);break;
default:if(l364 == SOC_ALPM_MODE_PARALLEL){*l17 = l132;}else{*l17 = l132+((3*
(64+32+2+1))/3);}break;}return(SOC_E_NONE);}static int l14(int l1,void*l8,
void*l15,int*l16,int*l17,int*l10){int l140;int l34;int l133;int l132 = 0;l34 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(
l1)]->l76));if(l34){if(!(l34 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l8),(l108[(l1)]->l77)))){return(SOC_E_PARAM);}}*
l10 = l34;l35(l1,l8,&l132);*l17 = l132;if(l318(l1,l8,l132,&l133) == 
SOC_E_NONE){*l16 = l133;if((l140 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,(*l10)?*
l16:(*l16>>1),l15))<0){return l140;}return(SOC_E_NONE);}else{return(
SOC_E_NOT_FOUND);}}static int l2(int l1){int l329;int l183;int l307;int l365;
uint32 l366 = 0;uint32 l364;if(!soc_feature(l1,soc_feature_lpm_tcam)){return(
SOC_E_UNAVAIL);}if((l364 = soc_property_get(l1,spn_L3_ALPM_ENABLE,0))){
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l1,&l366));soc_reg_field_set(
l1,L3_DEFIP_RPF_CONTROLr,&l366,LPM_MODEf,1);if(l364 == SOC_ALPM_MODE_PARALLEL
){soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l366,LOOKUP_MODEf,1);}else{
soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l366,LOOKUP_MODEf,0);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l1,l366));l366 = 0;if(
SOC_URPF_STATUS_GET(l1)){soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l366,
URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l366,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l366,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l366,
URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l1,l366))
;l366 = 0;if(l364 == SOC_ALPM_MODE_PARALLEL){if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l366,TCAM2_SELf,1);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l366,TCAM3_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM4_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM6_SELf,3);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM7_SELf,3);}else{soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM4_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM5_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM6_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l366,TCAM4_SELf,2);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l366,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM6_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l366,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l1,l366));if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l367 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l1,&l367));soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&
l367,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l367,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l367,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l367,
V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l1,l367))
;}}l329 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l1);l365 = sizeof(l42)*(l329);if(
(l44[(l1)]!= NULL)){if(soc_alpm_deinit(l1)<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}}l108[l1] = sal_alloc(sizeof(l106),"lpm_field_state");if(NULL
== l108[l1]){SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}(l108[l1])->l46 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,CLASS_ID0f);(l108[l1])->l47 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,CLASS_ID1f);(l108[l1])->l48 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD0f);(l108[l1])->l49 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DST_DISCARD1f);(l108[l1])->l50 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP0f);(l108[l1])->l51 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP1f);(l108[l1])->l52 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT0f);(l108[l1])->l53 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_COUNT1f);(l108[l1])->l54 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR0f);(l108[l1])->l55 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ECMP_PTR1f);(l108[l1])->l56 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE0f);(l108[l1])->l57 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_ROUTE1f);(l108[l1])->l58 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MULTICAST_ROUTE0f);(l108[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MULTICAST_ROUTE1f);(l108[l1])->l60 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPA_ID0f);(l108[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPA_ID1f);(l108[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,EXPECTED_L3_IIF0f);(l108[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,EXPECTED_L3_IIF1f);(l108[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);(
l108[l1])->l65 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,
IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU1f);(l108[l1])->l66 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);(
l108[l1])->l67 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,
IPMC_EXPECTED_L3_IIF_MISMATCH_DROP1f);(l108[l1])->l68 = soc_mem_fieldinfo_get
(l1,L3_DEFIPm,L3MC_INDEX0f);(l108[l1])->l69 = soc_mem_fieldinfo_get(l1,
L3_DEFIPm,L3MC_INDEX1f);(l108[l1])->l70 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,
HIT0f);(l108[l1])->l71 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,HIT1f);(l108[l1])
->l72 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR0f);(l108[l1])->l73 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR1f);(l108[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK0f);(l108[l1])->l75 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,IP_ADDR_MASK1f);(l108[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE0f);(l108[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE1f);(l108[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK0f);(l108[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,MODE_MASK1f);(l108[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX0f);(l108[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,NEXT_HOP_INDEX1f);(l108[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI0f);(l108[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,PRI1f);(l108[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE0f);(l108[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,RPE1f);(l108[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID0f);(l108[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VALID1f);(l108[l1])->l88 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_0f);(l108[l1])->l89 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_1f);(l108[l1])->l90 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK0f);(l108[l1])->l91 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,VRF_ID_MASK1f);(l108[l1])->l92 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH0f);(l108[l1])->l93 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,GLOBAL_HIGH1f);(l108[l1])->l94 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX0f);(l108[l1])->l95 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_HIT_IDX1f);(l108[l1])->l96 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR0f);(l108[l1])->l97 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,ALG_BKT_PTR1f);(l108[l1])->l98 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS0f);(l108[l1])->l99 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,DEFAULT_MISS1f);(l108[l1])->l100 = 
soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l108[l1])->
l101 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l108[
l1])->l102 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l108
[l1])->l103 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(
l108[l1])->l104 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,ENTRY_TYPE_MASK0f);(l108
[l1])->l105 = soc_mem_fieldinfo_get(l1,L3_DEFIPm,ENTRY_TYPE_MASK1f);(l44[(l1)
]) = sal_alloc(l365,"LPM prefix info");if(NULL == (l44[(l1)])){sal_free(l108[
l1]);l108[l1] = NULL;SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}sal_memset
((l44[(l1)]),0,l365);for(l183 = 0;l183<l329;l183++){(l44[(l1)][(l183)].l37) = 
-1;(l44[(l1)][(l183)].l38) = -1;(l44[(l1)][(l183)].l39) = -1;(l44[(l1)][(l183
)].next) = -1;(l44[(l1)][(l183)].l40) = 0;(l44[(l1)][(l183)].l41) = 0;}l307 = 
soc_mem_index_count(l1,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l1)){l307>>= 1;}if(
l364 == SOC_ALPM_MODE_PARALLEL){(l44[(l1)][(((3*(64+32+2+1))-1))].l38) = (
l307>>1)-1;(l44[(l1)][(((((3*(64+32+2+1))/3)-1)))].l41) = l307>>1;(l44[(l1)][
((((3*(64+32+2+1))-1)))].l41) = (l307-(l44[(l1)][(((((3*(64+32+2+1))/3)-1)))]
.l41));}else{(l44[(l1)][((((3*(64+32+2+1))-1)))].l41) = l307;}if((l119[(l1)])
!= NULL){if(l127((l119[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}(l119[(l1)]) = NULL;}if(l125(l1,l307*2,l307,&(l119[(l1)]))<0)
{SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;}(void)soc_alpm_ipmc_war(l1,TRUE
);SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}static int l3(int l1){if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}
SOC_ALPM_LPM_LOCK(l1);(void)soc_alpm_ipmc_war(l1,FALSE);if((l119[(l1)])!= 
NULL){l127((l119[(l1)]));(l119[(l1)]) = NULL;}if((l44[(l1)]!= NULL)){sal_free
(l108[l1]);l108[l1] = NULL;sal_free((l44[(l1)]));(l44[(l1)]) = NULL;}
SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}static int l4(int l1,void*l5,int*
l368,int l6){int l132;int index;int l10;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];
int l140 = SOC_E_NONE;int l369 = 0;if(l6>= 2){return SOC_E_INTERNAL;}
sal_memcpy(l15,soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,
L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l1);l140 = l14(l1,l5,l15,&index,&l132,&l10);
if(l140 == SOC_E_NOT_FOUND){l140 = l350(l1,l132,l10,l15,&index);if(l140<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l140);}}else{l369 = 1;}*l368 = index;if(l140 == 
SOC_E_NONE){if(!l10){if(index&1){l140 = soc_alpm_lpm_ip4entry0_to_1(l1,l5,l15
,PRESERVE_HIT);}else{l140 = soc_alpm_lpm_ip4entry0_to_0(l1,l5,l15,
PRESERVE_HIT);}if(l140<0){SOC_ALPM_LPM_UNLOCK(l1);return(l140);}l5 = (void*)
l15;index>>= 1;}soc_alpm_lpm_state_dump(l1);LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\nsoc_alpm_lpm_insert: %d %d\n"),index,l132));if(!l369){l314(
l1,l5,index,0x4000,0);}l140 = l178(l1,MEM_BLOCK_ANY,index,index,l5);if(l140>= 
0){l140 = l330(l1,index,index,l5,l6);}}SOC_ALPM_LPM_UNLOCK(l1);return(l140);}
static int l7(int l1,void*l8){int l132;int index;int l10;uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];int l140 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l140 = 
l14(l1,l8,l15,&index,&l132,&l10);if(l140 == SOC_E_NONE){LOG_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_alpm_lpm_delete: %d %d\n"),index,l132))
;l317(l1,l8,index);l140 = l356(l1,l132,l10,l15,index);}
soc_alpm_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l140);}static int
l18(int l1,void*l8,void*l15,int*l16){int l132;int l140;int l10;
SOC_ALPM_LPM_LOCK(l1);l140 = l14(l1,l8,l15,l16,&l132,&l10);
SOC_ALPM_LPM_UNLOCK(l1);return(l140);}static int l9(int l21,void*l8,int l10,
int l11,int l12,defip_aux_scratch_entry_t*l13){uint32 l141;uint32 l370[4] = {
0,0,0,0};int l132 = 0;int l140 = SOC_E_NONE;l141 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,VALID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
VALIDf,l141);l141 = soc_mem_field32_get(l21,L3_DEFIPm,l8,MODE0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,MODEf,l141);l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ENTRY_TYPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ENTRY_TYPEf,0);l141 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,GLOBAL_ROUTE0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,GLOBAL_ROUTEf,l141);if(soc_feature(l21,soc_feature_ipmc_defip)){l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,RPA_ID0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,RPA_IDf,l141);l141 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,EXPECTED_L3_IIF0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm
,l13,EXPECTED_L3_IIFf,l141);l141 = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPU0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf,l141);l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IPMC_EXPECTED_L3_IIF_MISMATCH_DROP0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf,l141);l141 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,L3MC_INDEX0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
L3MC_INDEXf,l141);}l141 = soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,ECMPf,l141);l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP_PTR0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ECMP_PTRf,l141);l141 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,NEXT_HOP_INDEX0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,NEXT_HOP_INDEXf,l141);l141 = soc_mem_field32_get(l21,L3_DEFIPm,l8,PRI0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,PRIf,l141);l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,RPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,RPEf,l141);l141 =soc_mem_field32_get(l21,L3_DEFIPm,
l8,VRF_ID_0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,VRFf,l141);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,DB_TYPEf,l11);l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,DST_DISCARD0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,DST_DISCARDf,l141);l141 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,CLASS_ID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
CLASS_IDf,l141);if(l10){l370[2] = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR0f);l370[3] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR1f);}else{
l370[0] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR0f);}soc_mem_field_set(
l21,L3_DEFIP_AUX_SCRATCHm,(uint32*)l13,IP_ADDRf,(uint32*)l370);if(l10){l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l140 = _ipmask2pfx(
l141,&l132))<0){return(l140);}l141 = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR_MASK1f);if(l132){if(l141!= 0xffffffff){return(SOC_E_PARAM);}l132+= 32
;}else{if((l140 = _ipmask2pfx(l141,&l132))<0){return(l140);}}}else{l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l140 = _ipmask2pfx(
l141,&l132))<0){return(l140);}}soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,IP_LENGTHf,l132);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
REPLACE_LENf,l12);return(SOC_E_NONE);}int _soc_alpm_aux_op(int l1,
_soc_aux_op_t l371,defip_aux_scratch_entry_t*l13,int l372,int*l154,int*l155,
int*bucket_index){uint32 l366 = 0;uint32 l373 = 0;int l374;soc_timeout_t l375
;int l140 = SOC_E_NONE;int l376 = 0;if(l372){SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_SCRATCHm(l1,MEM_BLOCK_ANY,0,l13));}l377:l366 = 0;switch(
l371){case INSERT_PROPAGATE:l374 = 0;break;case DELETE_PROPAGATE:l374 = 1;
break;case PREFIX_LOOKUP:l374 = 2;break;case HITBIT_REPLACE:l374 = 3;break;
default:return SOC_E_PARAM;}soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l366,
OPCODEf,l374);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l366,STARTf,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l366));soc_timeout_init(&l375
,50000,5);l374 = 0;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l1,&l366));
l374 = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l366,DONEf);if(l374 == 1){l140
= SOC_E_NONE;break;}if(soc_timeout_check(&l375)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l1,&l366));l374 = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRLr,l366,DONEf);if(l374 == 1){l140 = SOC_E_NONE;}else{LOG_WARN
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d : DEFIP AUX Operation timeout\n"),l1
));l140 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l140)){if(
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l366,ERRORf)){soc_reg_field_set(l1,
L3_DEFIP_AUX_CTRLr,&l366,STARTf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&
l366,ERRORf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l366,DONEf,0);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l366));LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: DEFIP AUX Operation encountered "
"parity error !!\n"),l1));l376++;if(SOC_CONTROL(l1)->alpm_bulk_retry){
sal_sem_take(SOC_CONTROL(l1)->alpm_bulk_retry,1000000);}if(l376<5){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: Retry DEFIP AUX Operation..\n"),l1))
;goto l377;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l1));return SOC_E_INTERNAL;}}if(l371 == PREFIX_LOOKUP){if(l154&&l155){*l154 = 
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l366,HITf);*l155 = soc_reg_field_get(
l1,L3_DEFIP_AUX_CTRLr,l366,BKT_INDEXf);SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRL_1r(l1,&l373));*bucket_index = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRL_1r,l373,BKT_PTRf);}}}return l140;}static int l20(int l21,
void*lpm_entry,void*l22,void*l23,soc_mem_t l24,uint32 l25,uint32*l378){uint32
l141;uint32 l370[4] = {0,0};int l132 = 0;int l140 = SOC_E_NONE;int l10;uint32
l26 = 0;l10 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,MODE0f);sal_memset(
l22,0,soc_mem_entry_words(l21,l24)*4);l141 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,VALID0f);soc_mem_field32_set(l21,l24,l22,VALIDf,l141);
l141 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,ECMP0f);
soc_mem_field32_set(l21,l24,l22,ECMPf,l141);l141 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set(l21,l24,l22,ECMP_PTRf,
l141);l141 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f);
soc_mem_field32_set(l21,l24,l22,NEXT_HOP_INDEXf,l141);l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,PRI0f);soc_mem_field32_set(l21,
l24,l22,PRIf,l141);l141 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,RPE0f);
soc_mem_field32_set(l21,l24,l22,RPEf,l141);l141 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,DST_DISCARD0f);soc_mem_field32_set(l21,l24,l22,
DST_DISCARDf,l141);l141 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
SRC_DISCARD0f);soc_mem_field32_set(l21,l24,l22,SRC_DISCARDf,l141);l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,CLASS_ID0f);soc_mem_field32_set(
l21,l24,l22,CLASS_IDf,l141);l370[0] = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,IP_ADDR0f);if(l10){l370[1] = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,IP_ADDR1f);}soc_mem_field_set(l21,l24,(uint32*)l22,KEYf,(uint32*)
l370);if(l10){l141 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f);if((l140 = _ipmask2pfx(l141,&l132))<0){return(l140);}l141 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f);if(l132){if(l141
!= 0xffffffff){return(SOC_E_PARAM);}l132+= 32;}else{if((l140 = _ipmask2pfx(
l141,&l132))<0){return(l140);}}}else{l141 = soc_mem_field32_get(l21,L3_DEFIPm
,lpm_entry,IP_ADDR_MASK0f);if((l140 = _ipmask2pfx(l141,&l132))<0){return(l140
);}}if((l132 == 0)&&(l370[0] == 0)&&(l370[1] == 0)){l26 = 1;}if(l378!= NULL){
*l378 = l26;}soc_mem_field32_set(l21,l24,l22,LENGTHf,l132);if(l23 == NULL){
return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(l21)){sal_memset(l23,0,
soc_mem_entry_words(l21,l24)*4);sal_memcpy(l23,l22,soc_mem_entry_words(l21,
l24)*4);soc_mem_field32_set(l21,l24,l23,DST_DISCARDf,0);soc_mem_field32_set(
l21,l24,l23,RPEf,0);soc_mem_field32_set(l21,l24,l23,SRC_DISCARDf,l25&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(l21,l24,l23,DEFAULTROUTEf,l26);
}return(SOC_E_NONE);}static int l27(int l21,void*l22,soc_mem_t l24,int l10,
int l28,int l29,int index,void*lpm_entry){uint32 l141;uint32 l370[4] = {0,0};
uint32 l132 = 0;sal_memset(lpm_entry,0,soc_mem_entry_words(l21,L3_DEFIPm)*4);
l141 = soc_mem_field32_get(l21,l24,l22,VALIDf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,VALID0f,l141);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,VALID1f,l141);}l141 = soc_mem_field32_get(l21,l24,l22,ECMPf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ECMP0f,l141);l141 = 
soc_mem_field32_get(l21,l24,l22,ECMP_PTRf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,ECMP_PTR0f,l141);l141 = soc_mem_field32_get(l21,l24,l22,
NEXT_HOP_INDEXf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f
,l141);l141 = soc_mem_field32_get(l21,l24,l22,PRIf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,PRI0f,l141);l141 = soc_mem_field32_get(l21,l24,l22,RPEf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,RPE0f,l141);l141 = 
soc_mem_field32_get(l21,l24,l22,DST_DISCARDf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,DST_DISCARD0f,l141);l141 = soc_mem_field32_get(l21,l24,
l22,SRC_DISCARDf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,SRC_DISCARD0f,
l141);l141 = soc_mem_field32_get(l21,l24,l22,CLASS_IDf);soc_mem_field32_set(
l21,L3_DEFIPm,lpm_entry,CLASS_ID0f,l141);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,ALG_BKT_PTR0f,l29);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
ALG_HIT_IDX0f,index);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE_MASK0f,
3);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l10){
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(l21,
l24,l22,KEYf,l370);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR1f,l370[1]);}soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR0f,
l370[0]);l141 = soc_mem_field32_get(l21,l24,l22,LENGTHf);if(l10){if(l141>= 32
){l132 = 0xffffffff;soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l132);l132 = ~(((l141-32) == 32)?0:(0xffffffff)>>(l141-32));
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l132);}else{l132 = 
~(0xffffffff>>l141);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l132);}}else{assert(l141<= 32);l132 = ~(((l141) == 32)?0:(
0xffffffff)>>(l141));soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,l132);}if(l28 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,GLOBAL_HIGH0f,1);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else if(l28 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry
,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,0);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,l28);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(l21));}
return(SOC_E_NONE);}int soc_alpm_warmboot_pivot_add(int l21,int l10,void*
lpm_entry,int l379,int l380){int l140 = SOC_E_NONE;uint32 key[2] = {0,0};
alpm_pivot_t*l211 = NULL;alpm_bucket_handle_t*l220 = NULL;int l161 = 0,l28 = 
0;uint32 l381;trie_t*l269 = NULL;uint32 prefix[5] = {0};int l26 = 0;l140 = 
l146(l21,lpm_entry,prefix,&l381,&l26);SOC_IF_ERROR_RETURN(l140);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l21,lpm_entry,&l161,&l28));l379 = 
soc_alpm_physical_idx(l21,L3_DEFIPm,l379,l10);l220 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l220 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l21,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l220,0,sizeof(*l220));
l211 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l211 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l21,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l220);return SOC_E_MEMORY;}sal_memset(l211,0
,sizeof(*l211));PIVOT_BUCKET_HANDLE(l211) = l220;if(l10){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l211));key[0] = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR0f);key[1] = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR1f);}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l211));key[0] = soc_L3_DEFIPm_field32_get
(l21,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l211) = l380;PIVOT_TCAM_INDEX(
l211) = l379;if(l161!= SOC_L3_VRF_OVERRIDE){if(l10 == 0){l269 = 
VRF_PIVOT_TRIE_IPV4(l21,l28);if(l269 == NULL){trie_init(_MAX_KEY_LEN_48_,&
VRF_PIVOT_TRIE_IPV4(l21,l28));l269 = VRF_PIVOT_TRIE_IPV4(l21,l28);}}else{l269
= VRF_PIVOT_TRIE_IPV6(l21,l28);if(l269 == NULL){trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l21,l28));l269 = VRF_PIVOT_TRIE_IPV6(l21,l28);}}
sal_memcpy(l211->key,prefix,sizeof(prefix));l211->len = l381;l140 = 
trie_insert(l269,l211->key,NULL,l211->len,(trie_node_t*)l211);if(SOC_FAILURE(
l140)){sal_free(l220);sal_free(l211);return l140;}}ALPM_TCAM_PIVOT(l21,l379) = 
l211;PIVOT_BUCKET_VRF(l211) = l28;PIVOT_BUCKET_IPV6(l211) = l10;
PIVOT_BUCKET_ENT_CNT_UPDATE(l211);if(key[0] == 0&&key[1] == 0){
PIVOT_BUCKET_DEF(l211) = TRUE;}VRF_PIVOT_REF_INC(l21,l28,l10);return l140;}
static int l382(int l21,int l10,void*lpm_entry,void*l22,soc_mem_t l24,int l379
,int l380,int l383){int l384;int l28;int l140 = SOC_E_NONE;int l26 = 0;uint32
prefix[5] = {0,0,0,0,0};uint32 l153;void*l385 = NULL;trie_t*l386 = NULL;
trie_t*l214 = NULL;trie_node_t*l216 = NULL;payload_t*l387 = NULL;payload_t*
l218 = NULL;alpm_pivot_t*l158 = NULL;if((NULL == lpm_entry)||(NULL == l22)){
return SOC_E_PARAM;}if(l10){if(!(l10 = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l21,lpm_entry,&l384,&l28));l24 = (l10)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l385 = sal_alloc(sizeof(
defip_entry_t),"Temp lpm_entr");if(NULL == l385){return SOC_E_MEMORY;}
SOC_IF_ERROR_RETURN(l27(l21,l22,l24,l10,l384,l380,l379,l385));l140 = l146(l21
,l385,prefix,&l153,&l26);if(SOC_FAILURE(l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l21,"prefix create failed\n")));return l140;}sal_free(l385);l158 = 
ALPM_TCAM_PIVOT(l21,l379);l386 = PIVOT_BUCKET_TRIE(l158);l387 = sal_alloc(
sizeof(payload_t),"Payload for Key");if(NULL == l387){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l21,"Unable to allocate memory for trie node.\n")
));return SOC_E_MEMORY;}l218 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(NULL == l218){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l21,"Unable to allocate memory for pfx trie node\n")));sal_free(
l387);return SOC_E_MEMORY;}sal_memset(l387,0,sizeof(*l387));sal_memset(l218,0
,sizeof(*l218));l387->key[0] = prefix[0];l387->key[1] = prefix[1];l387->key[2
] = prefix[2];l387->key[3] = prefix[3];l387->key[4] = prefix[4];l387->len = 
l153;l387->index = l383;sal_memcpy(l218,l387,sizeof(*l387));l140 = 
trie_insert(l386,prefix,NULL,l153,(trie_node_t*)l387);if(SOC_FAILURE(l140)){
goto l388;}if(l10){l214 = VRF_PREFIX_TRIE_IPV6(l21,l28);}else{l214 = 
VRF_PREFIX_TRIE_IPV4(l21,l28);}if(!l26){l140 = trie_insert(l214,prefix,NULL,
l153,(trie_node_t*)l218);if(SOC_FAILURE(l140)){goto l239;}}return l140;l239:(
void)trie_delete(l386,prefix,l153,&l216);l387 = (payload_t*)l216;l388:
sal_free(l387);sal_free(l218);return l140;}static int l389(int l21,int l34,
int l28,int l142,int bkt_ptr){int l140 = SOC_E_NONE;uint32 l153;uint32 key[2]
= {0,0};trie_t*l390 = NULL;payload_t*l255 = NULL;defip_entry_t*lpm_entry = 
NULL;lpm_entry = sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(
lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l21,
"unable to allocate memory for LPM entry\n")));return SOC_E_MEMORY;}l30(l21,
key,0,l28,l34,lpm_entry,0,1);if(l28 == SOC_VRF_MAX(l21)+1){
soc_L3_DEFIPm_field32_set(l21,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l21,lpm_entry,DEFAULT_MISS0f,1);}
soc_L3_DEFIPm_field32_set(l21,lpm_entry,ALG_BKT_PTR0f,bkt_ptr);if(l34 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l21,l28) = lpm_entry;trie_init(_MAX_KEY_LEN_48_,&
VRF_PREFIX_TRIE_IPV4(l21,l28));l390 = VRF_PREFIX_TRIE_IPV4(l21,l28);}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l21,l28) = lpm_entry;trie_init(_MAX_KEY_LEN_144_,
&VRF_PREFIX_TRIE_IPV6(l21,l28));l390 = VRF_PREFIX_TRIE_IPV6(l21,l28);}l255 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(l255 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l21,
"Unable to allocate memory for pfx trie node \n")));return SOC_E_MEMORY;}
sal_memset(l255,0,sizeof(*l255));l153 = 0;l255->key[0] = key[0];l255->key[1] = 
key[1];l255->len = l153;l140 = trie_insert(l390,key,NULL,l153,&(l255->node));
if(SOC_FAILURE(l140)){sal_free(l255);return l140;}VRF_TRIE_INIT_DONE(l21,l28,
l34,1);return l140;}int soc_alpm_warmboot_prefix_insert(int l21,int l10,void*
lpm_entry,void*l22,int l379,int l380,int l383){int l384;int l28;int l140 = 
SOC_E_NONE;soc_mem_t l24;l379 = soc_alpm_physical_idx(l21,L3_DEFIPm,l379,l10)
;l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l21,lpm_entry,&l384,&l28));if(l384 == 
SOC_L3_VRF_OVERRIDE){return(l140);}if(!VRF_TRIE_INIT_COMPLETED(l21,l28,l10)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l21,"VRF %d is not initialized\n"),
l28));l140 = l389(l21,l10,l28,l379,l380);if(SOC_FAILURE(l140)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l21,"VRF %d/%d trie init \n""failed\n"),l28,l10))
;return l140;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l21,
"VRF %d/%d trie init completed\n"),l28,l10));}l140 = l382(l21,l10,lpm_entry,
l22,l24,l379,l380,l383);if(l140!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l21,"unit %d : Route Insertion Failed :%s\n"),l21,soc_errmsg(l140)
));return(l140);}VRF_TRIE_ROUTES_INC(l21,l28,l10);return(l140);}int
soc_alpm_warmboot_bucket_bitmap_set(int l1,int l34,int l275){int l308 = 1;if(
l34){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l308 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l275,l308);return SOC_E_NONE;}int
soc_alpm_warmboot_lpm_reinit_done(int l21){int l142;int l391 = ((3*(64+32+2+1
))-1);int l307 = soc_mem_index_count(l21,L3_DEFIPm);if(SOC_URPF_STATUS_GET(
l21)){l307>>= 1;}if(!soc_alpm_mode_get(l21)){(l44[(l21)][(((3*(64+32+2+1))-1)
)].l39) = -1;for(l142 = ((3*(64+32+2+1))-1);l142>-1;l142--){if(-1 == (l44[(
l21)][(l142)].l37)){continue;}(l44[(l21)][(l142)].l39) = l391;(l44[(l21)][(
l391)].next) = l142;(l44[(l21)][(l391)].l41) = (l44[(l21)][(l142)].l37)-(l44[
(l21)][(l391)].l38)-1;l391 = l142;}(l44[(l21)][(l391)].next) = -1;(l44[(l21)]
[(l391)].l41) = l307-(l44[(l21)][(l391)].l38)-1;}else{(l44[(l21)][(((3*(64+32
+2+1))-1))].l39) = -1;for(l142 = ((3*(64+32+2+1))-1);l142>(((3*(64+32+2+1))-1
)/3);l142--){if(-1 == (l44[(l21)][(l142)].l37)){continue;}(l44[(l21)][(l142)]
.l39) = l391;(l44[(l21)][(l391)].next) = l142;(l44[(l21)][(l391)].l41) = (l44
[(l21)][(l142)].l37)-(l44[(l21)][(l391)].l38)-1;l391 = l142;}(l44[(l21)][(
l391)].next) = -1;(l44[(l21)][(l391)].l41) = l307-(l44[(l21)][(l391)].l38)-1;
l391 = (((3*(64+32+2+1))-1)/3);(l44[(l21)][((((3*(64+32+2+1))-1)/3))].l39) = 
-1;for(l142 = ((((3*(64+32+2+1))-1)/3)-1);l142>-1;l142--){if(-1 == (l44[(l21)
][(l142)].l37)){continue;}(l44[(l21)][(l142)].l39) = l391;(l44[(l21)][(l391)]
.next) = l142;(l44[(l21)][(l391)].l41) = (l44[(l21)][(l142)].l37)-(l44[(l21)]
[(l391)].l38)-1;l391 = l142;}(l44[(l21)][(l391)].next) = -1;(l44[(l21)][(l391
)].l41) = (l307>>1)-(l44[(l21)][(l391)].l38)-1;}return(SOC_E_NONE);}int
soc_alpm_warmboot_lpm_reinit(int l21,int l10,int l142,void*lpm_entry){int l17
;defip_entry_t*l392;if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l314(l21,lpm_entry,l142,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l35(l21,lpm_entry,&l17));if((l44[(l21)][(l17)].l40) == 0)
{(l44[(l21)][(l17)].l37) = l142;(l44[(l21)][(l17)].l38) = l142;}else{(l44[(
l21)][(l17)].l38) = l142;}(l44[(l21)][(l17)].l40)++;if(l10){return(SOC_E_NONE
);}}else{if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l392 = 
sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");soc_alpm_lpm_ip4entry1_to_0(
l21,lpm_entry,l392,TRUE);SOC_IF_ERROR_RETURN(l35(l21,l392,&l17));if((l44[(l21
)][(l17)].l40) == 0){(l44[(l21)][(l17)].l37) = l142;(l44[(l21)][(l17)].l38) = 
l142;}else{(l44[(l21)][(l17)].l38) = l142;}sal_free(l392);(l44[(l21)][(l17)].
l40)++;}}return(SOC_E_NONE);}typedef struct l393{int v4;int v6_64;int v6_128;
int l394;int l395;int l396;int l309;}l397;typedef enum l398{l399 = 0,l400,
l401,l402,l403,l404}l405;static void l406(int l1,alpm_vrf_counter_t*l407){
l407->v4 = soc_mem_index_count(l1,L3_DEFIPm)*2;l407->v6_128 = 
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){l407->v6_64 = l407->v6_128;}else{l407->v6_64 = 
l407->v4>>1;}if(SOC_URPF_STATUS_GET(l1)){l407->v4>>= 1;l407->v6_128>>= 1;l407
->v6_64>>= 1;}}static void l408(int l1,int l161,alpm_vrf_handle_t*l409,l405
l410){alpm_vrf_counter_t*l411;int l183,l412,l413,l414;int l369 = 0;
alpm_vrf_counter_t l407;switch(l410){case l399:LOG_CLI((BSL_META_U(l1,
"\nAdd Counter:\n")));break;case l400:LOG_CLI((BSL_META_U(l1,
"\nDelete Counter:\n")));break;case l401:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - 1:\n")));break;case l402:l406(l1,&l407);LOG_CLI((
BSL_META_U(l1,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l407.v4,
l407.v6_64,l407.v6_128));break;case l403:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l404:LOG_CLI((
BSL_META_U(l1,"\nInternal Debug Counter - LPM Full:\n")));break;default:break
;}LOG_CLI((BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n"))
);LOG_CLI((BSL_META_U(l1,"-----------------------------------------------\n")
));l412 = l413 = l414 = 0;for(l183 = 0;l183<MAX_VRF_ID+1;l183++){int l415,
l416,l417;if(l409[l183].init_done == 0&&l183!= MAX_VRF_ID){continue;}if(l161
!= -1&&l161!= l183){continue;}l369 = 1;switch(l410){case l399:l411 = &l409[
l183].add;break;case l400:l411 = &l409[l183].del;break;case l401:l411 = &l409
[l183].bkt_split;break;case l403:l411 = &l409[l183].lpm_shift;break;case l404
:l411 = &l409[l183].lpm_full;break;case l402:l411 = &l409[l183].pivot_used;
break;default:l411 = &l409[l183].pivot_used;break;}l415 = l411->v4;l416 = 
l411->v6_64;l417 = l411->v6_128;l412+= l415;l413+= l416;l414+= l417;do{
LOG_CLI((BSL_META_U(l1,"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l183 == 
MAX_VRF_ID?-1:l183),(l415),(l416),(l417),((l415+l416+l417)),(l183) == 
MAX_VRF_ID?"GHi":(l183) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l369 == 
0){LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));do{
LOG_CLI((BSL_META_U(l1,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l412),(
l413),(l414),((l412+l413+l414))));}while(0);}return;}int soc_alpm_debug_show(
int l1,int l161,uint32 flags){int l183,l418,l369 = 0;l397*l419;l397 l420;l397
l421;if(l161>(SOC_VRF_MAX(l1)+1)){return SOC_E_PARAM;}l418 = MAX_VRF_ID*
sizeof(l397);l419 = sal_alloc(l418,"_alpm_dbg_cnt");if(l419 == NULL){return
SOC_E_MEMORY;}sal_memset(l419,0,l418);l420.v4 = ALPM_IPV4_BKT_COUNT;l420.
v6_64 = ALPM_IPV6_64_BKT_COUNT;l420.v6_128 = ALPM_IPV6_128_BKT_COUNT;if(!
soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l420.v6_64<<= 1;l420.v6_128
<<= 1;}LOG_CLI((BSL_META_U(l1,"\nBucket Occupancy:\n")));if(flags&(
SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l1,
"\n  BKT/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));}for(
l183 = 0;l183<MAX_PIVOT_COUNT;l183++){alpm_pivot_t*l422 = ALPM_TCAM_PIVOT(l1,
l183);if(l422!= NULL){l397*l423;int l28 = PIVOT_BUCKET_VRF(l422);if(l28<0||
l28>(SOC_VRF_MAX(l1)+1)){continue;}if(l161!= -1&&l161!= l28){continue;}if(
flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l369 = 1;do{LOG_CLI((BSL_META_U(l1,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l183,l28,PIVOT_BUCKET_MIN(l422),
PIVOT_BUCKET_MAX(l422),PIVOT_BUCKET_COUNT(l422),PIVOT_BUCKET_DEF(l422)?"Def":
(l28) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}l423 = &l419[l28];if(
PIVOT_BUCKET_IPV6(l422) == L3_DEFIP_MODE_128){l423->v6_128+= 
PIVOT_BUCKET_COUNT(l422);l423->l396+= l420.v6_128;}else if(PIVOT_BUCKET_IPV6(
l422) == L3_DEFIP_MODE_64){l423->v6_64+= PIVOT_BUCKET_COUNT(l422);l423->l395
+= l420.v6_64;}else{l423->v4+= PIVOT_BUCKET_COUNT(l422);l423->l394+= l420.v4;
}l423->l309 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l369 == 0){
LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}}sal_memset(&l421
,0,sizeof(l421));l369 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){LOG_CLI((
BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));for(l183
= 0;l183<MAX_VRF_ID;l183++){l397*l423;if(l419[l183].l309!= TRUE){continue;}if
(l161!= -1&&l161!= l183){continue;}l369 = 1;l423 = &l419[l183];do{(&l421)->v4
+= (l423)->v4;(&l421)->l394+= (l423)->l394;(&l421)->v6_64+= (l423)->v6_64;(&
l421)->l395+= (l423)->l395;(&l421)->v6_128+= (l423)->v6_128;(&l421)->l396+= (
l423)->l396;}while(0);do{LOG_CLI((BSL_META_U(l1,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l183),(l423
->l394)?(l423->v4)*100/(l423->l394):0,(l423->l394)?(l423->v4)*1000/(l423->
l394)%10:0,(l423->l395)?(l423->v6_64)*100/(l423->l395):0,(l423->l395)?(l423->
v6_64)*1000/(l423->l395)%10:0,(l423->l396)?(l423->v6_128)*100/(l423->l396):0,
(l423->l396)?(l423->v6_128)*1000/(l423->l396)%10:0,((l423->l394+l423->l395+
l423->l396))?((l423->v4+l423->v6_64+l423->v6_128))*100/((l423->l394+l423->
l395+l423->l396)):0,((l423->l394+l423->l395+l423->l396))?((l423->v4+l423->
v6_64+l423->v6_128))*1000/((l423->l394+l423->l395+l423->l396))%10:0,(l183) == 
SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l369 == 0){LOG_CLI((BSL_META_U(l1,
"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l1,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l1,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l421)->l394)?((&l421)->v4)*100/((&l421)->l394):0,((&l421)->l394)?((&l421)->v4
)*1000/((&l421)->l394)%10:0,((&l421)->l395)?((&l421)->v6_64)*100/((&l421)->
l395):0,((&l421)->l395)?((&l421)->v6_64)*1000/((&l421)->l395)%10:0,((&l421)->
l396)?((&l421)->v6_128)*100/((&l421)->l396):0,((&l421)->l396)?((&l421)->
v6_128)*1000/((&l421)->l396)%10:0,(((&l421)->l394+(&l421)->l395+(&l421)->l396
))?(((&l421)->v4+(&l421)->v6_64+(&l421)->v6_128))*100/(((&l421)->l394+(&l421)
->l395+(&l421)->l396)):0,(((&l421)->l394+(&l421)->l395+(&l421)->l396))?(((&
l421)->v4+(&l421)->v6_64+(&l421)->v6_128))*1000/(((&l421)->l394+(&l421)->l395
+(&l421)->l396))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l408(l1,l161,alpm_vrf_handle[l1],l402);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_CNT
){l408(l1,l161,alpm_vrf_handle[l1],l399);l408(l1,l161,alpm_vrf_handle[l1],
l400);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l408(l1,l161,alpm_vrf_handle
[l1],l401);l408(l1,l161,alpm_vrf_handle[l1],l404);l408(l1,l161,
alpm_vrf_handle[l1],l403);}sal_free(l419);return SOC_E_NONE;}int
soc_alpm_bucket_sanity_check(int l1,soc_mem_t l284,int index){int l140 = 
SOC_E_NONE;int l183,l281,l290,l10,l155 = -1;int l161 = 0,l28;uint32 l150 = 0;
uint32 l15[SOC_MAX_MEM_FIELD_WORDS];uint32 l424[SOC_MAX_MEM_FIELD_WORDS];int
l425,l426,l427;defip_entry_t lpm_entry;int l295,l428;soc_mem_t l24;int l199,
l287,l429,l430;int l431 = 0;soc_field_t l432[2] = {VALID0f,VALID1f};
soc_field_t l433[2] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l434[2] = {
ALG_BKT_PTR0f,ALG_BKT_PTR1f};l295 = soc_mem_index_min(l1,l284);l428 = 
soc_mem_index_max(l1,l284);if((index>= 0)&&(index<l295||index>l428)){return
SOC_E_PARAM;}else if(index>= 0){l295 = index;l428 = index;}SOC_ALPM_LPM_LOCK(
l1);for(l183 = l295;l183<= l428;l183++){SOC_ALPM_LPM_UNLOCK(l1);
SOC_ALPM_LPM_LOCK(l1);l140 = soc_mem_read(l1,l284,MEM_BLOCK_ANY,l183,(void*)
l15);if(SOC_FAILURE(l140)){continue;}l10 = soc_mem_field32_get(l1,l284,(void*
)l15,MODE0f);if(l10){l287 = 1;l24 = L3_DEFIP_ALPM_IPV6_64m;l429 = 16;}else{
l287 = 2;l24 = L3_DEFIP_ALPM_IPV4m;l429 = 24;}for(l199 = 0;l199<l287;l199++){
if(soc_mem_field32_get(l1,l284,(void*)l15,l432[l199]) == 0||
soc_mem_field32_get(l1,l284,(void*)l15,l433[l199]) == 1){continue;}l430 = 
soc_mem_field32_get(l1,l284,(void*)l15,l434[l199]);if(l199 == 1){
soc_alpm_lpm_ip4entry1_to_0(l1,l15,l424,PRESERVE_HIT);}else{
soc_alpm_lpm_ip4entry0_to_0(l1,l15,l424,PRESERVE_HIT);}l140 = 
soc_alpm_lpm_vrf_get(l1,l424,&l161,&l28);if(SOC_FAILURE(l140)){continue;}if(
SOC_ALPM_V6_SCALE_CHECK(l1,l10)){l429<<= 1;}l155 = -1;for(l281 = 0;l281<l429;
l281++){l140 = _soc_alpm_mem_index(l1,l24,l430,l281,l150,&l290);if(
SOC_FAILURE(l140)){continue;}l140 = soc_mem_read(l1,l24,MEM_BLOCK_ANY,l290,(
void*)l15);if(SOC_FAILURE(l140)){break;}if(!soc_mem_field32_get(l1,l24,(void*
)l15,VALIDf)){continue;}l140 = l27(l1,(void*)l15,l24,l10,l161,l430,0,&
lpm_entry);if(SOC_FAILURE(l140)){continue;}l140 = l159(l1,(void*)&lpm_entry,
l24,(void*)l15,&l425,&l426,&l427,FALSE);if(SOC_FAILURE(l140)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLaunched AUX operation for "
"index %d bucket %d sanity check failed\n"),l183,l430));l431++;continue;}if(
l426!= l430){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tEntry at index %d does not belong ""to bucket %d(from bucket %d)\n"),l427,
l430,l426));l431++;}if(l155 == -1){l155 = l425;continue;}if(l155!= l425){int
l435,l436;l435 = soc_alpm_logical_idx(l1,l284,l155>>1,1);l436 = 
soc_alpm_logical_idx(l1,l284,l425>>1,1);LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"\tAliased bucket %d(returned "
"bucket %d) found from TCAM1 %d and TCAM2 %d\n"),l430,l426,l435,l436));l431++
;}}}}SOC_ALPM_LPM_UNLOCK(l1);if(l431 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check passed\n"),
SOC_MEM_NAME(l1,l284),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l1,l284),index,l431));return
SOC_E_FAIL;}int soc_alpm_pivot_sanity_check(int l1,soc_mem_t l284,int index){
int l183,l199,l309 = 0;int l290,l295,l428;int l140 = SOC_E_NONE;int*l437 = 
NULL;int l425,l426,l427;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l143,l10 = 0,
l287 = 2;int l438,l439[2];soc_mem_t l440,l441;int l431 = 0;soc_field_t l432[3
] = {VALID0f,VALID1f,INVALIDf};soc_field_t l433[3] = {GLOBAL_HIGH0f,
GLOBAL_HIGH1f,INVALIDf};soc_field_t l442[3] = {NEXT_HOP_INDEX0f,
NEXT_HOP_INDEX1f,INVALIDf};soc_field_t l434[3] = {ALG_BKT_PTR0f,ALG_BKT_PTR1f
,INVALIDf};l295 = soc_mem_index_min(l1,l284);l428 = soc_mem_index_max(l1,l284
);if((index>= 0)&&(index<l295||index>l428)){return SOC_E_PARAM;}else if(index
>= 0){l295 = index;l428 = index;}l437 = sal_alloc(sizeof(int)*MAX_PIVOT_COUNT
,"Bucket index array");if(l437 == NULL){l140 = SOC_E_MEMORY;return l140;}
sal_memset(l437,0xff,sizeof(int)*MAX_PIVOT_COUNT);SOC_ALPM_LPM_LOCK(l1);for(
l183 = l295;l183<= l428;l183++){SOC_ALPM_LPM_UNLOCK(l1);SOC_ALPM_LPM_LOCK(l1)
;l140 = soc_mem_read(l1,l284,MEM_BLOCK_ANY,l183,(void*)l15);if(SOC_FAILURE(
l140)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l1,l284),
l183,l140));l431++;continue;}l10 = soc_mem_field32_get(l1,l284,(void*)l15,
MODE0f);if(l10){l287 = 1;l441 = L3_DEFIP_ALPM_IPV6_64m;}else{l287 = 2;l441 = 
L3_DEFIP_ALPM_IPV4m;}for(l199 = 0;l199<l287;l199++){if(soc_mem_field32_get(l1
,l284,(void*)l15,l432[l199]) == 0||soc_mem_field32_get(l1,l284,(void*)l15,
l433[l199]) == 1){continue;}l143 = soc_mem_field32_get(l1,l284,(void*)l15,
l434[l199]);if(l143!= 0){if(l437[l143] == -1){l437[l143] = l183;}else{
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tDuplicated bucket pointer "
"%d detected, in memory %s index1 %d ""and index2 %d\n"),l143,SOC_MEM_NAME(l1
,l284),l437[l143],l183));l431++;continue;}}l140 = alpm_bucket_is_assigned(l1,
l143,l10,&l309);if(l140 == SOC_E_PARAM){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"\tInvalid bucket pointer %d ""detected, in memory %s index %d\n"),l143,
SOC_MEM_NAME(l1,l284),l183));l431++;continue;}if(l140>= 0&&l309 == 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),l143,SOC_MEM_NAME(l1,l284),l183));l431++
;continue;}l440 = _soc_trident2_alpm_bkt_view_get(l1,l143<<2);if(l441!= l440)
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual"" %s\n"),l143,SOC_MEM_NAME(l1,l441),
SOC_MEM_NAME(l1,l440)));l431++;continue;}l438 = soc_mem_field32_get(l1,l284,(
void*)l15,l442[l199]);if(l199 == 1){l140 = soc_alpm_lpm_ip4entry1_to_0(l1,l15
,l15,PRESERVE_HIT);if(SOC_FAILURE(l140)){continue;}}l425 = -1;l140 = l159(l1,
l15,l441,(void*)l15,&l425,&l426,&l427,FALSE);if(l425 == -1){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLaunched AUX operation for PIVOT "
"index %d sanity check failed\n"),l183));l431++;continue;}l290 = 
soc_alpm_logical_idx(l1,l284,l425>>1,1);l140 = soc_mem_read(l1,l284,
MEM_BLOCK_ANY,l290,(void*)l15);if(SOC_FAILURE(l140)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tRead memory %s index %d (nexthop) "
"return %d\n"),SOC_MEM_NAME(l1,l284),l290,l140));l431++;continue;}l439[0] = 
soc_mem_field32_get(l1,l284,(void*)l15,l442[0]);l439[1] = soc_mem_field32_get
(l1,l284,(void*)l15,l442[1]);if(l438!= l439[l425&1]){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d entry1 %d, index2 %d ""entry2 %d\n"),l183,l199,
l290,l425&1));l431++;continue;}}}SOC_ALPM_LPM_UNLOCK(l1);sal_free(l437);if(
l431 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMemory %s index %d Pivot sanity check passed\n"),SOC_MEM_NAME(l1,l284),
index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMemory %s index %d Pivot sanity check failed, ""encountered %d error(s)\n"
),SOC_MEM_NAME(l1,l284),index,l431));return SOC_E_FAIL;}
#endif /* ALPM_ENABLE */
