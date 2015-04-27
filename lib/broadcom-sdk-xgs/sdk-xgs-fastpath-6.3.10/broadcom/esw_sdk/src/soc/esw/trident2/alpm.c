/*
 * $Id$
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
soc_alpm_bucket_t soc_alpm_bucket[SOC_MAX_NUM_DEVICES];void l1(int l2);static
int l3(int l2);static int l4(int l2);static int l5(int l2,void*l6,int*index);
static int l7(int l2,void*l8);static int l9(int l2,void*l8,int l10,int l11,
int l12,defip_aux_scratch_entry_t*l13);static int l14(int l2,void*l8,void*l15
,int*l16,int*l17,int*l10);static int l18(int l2,void*l8,void*l15,int*l16);
static int l19(int l2);static int l20(int l21,void*lpm_entry,void*l22,void*
l23,soc_mem_t l24,uint32 l25,uint32*l26);static int l27(int l21,void*l22,
soc_mem_t l24,int l10,int l28,int l29,int index,void*lpm_entry);static int l30
(int l21,uint32*key,int len,int l28,int l10,defip_entry_t*lpm_entry,int l31,
int l32);static int l33(int l2,int l28,int l34);extern int _ipmask2pfx(uint32
l35,int*l36);static int l37(int l2,void*entry,int*l17);typedef struct l38{int
l39;int l40;int l41;int next;int l42;int l43;}l44,*l45;static l45 l46[
SOC_MAX_NUM_DEVICES];typedef struct l47{soc_field_info_t*l48;soc_field_info_t
*l49;soc_field_info_t*l50;soc_field_info_t*l51;soc_field_info_t*l52;
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
soc_field_info_t*l95;}l96,*l97;static l97 l98[SOC_MAX_NUM_DEVICES];typedef
struct l99{int l21;int l100;int l101;uint16*l102;uint16*l103;}l104;typedef
uint32 l105[5];typedef int(*l106)(l105 l107,l105 l108);static l104*l109[
SOC_MAX_NUM_DEVICES];static void l110(int l2,void*l15,int index,l105 l111);
static uint16 l112(uint8*l113,int l114);static int l115(int l21,int l100,int
l101,l104**l116);static int l117(l104*l118);static int l119(l104*l120,l106
l121,l105 entry,int l122,uint16*l123);static int l124(l104*l120,l106 l121,
l105 entry,int l122,uint16 l125,uint16 l126);static int l127(l104*l120,l106
l121,l105 entry,int l122,uint16 l128);alpm_vrf_handle_t alpm_vrf_handle[
SOC_MAX_NUM_DEVICES][MAX_VRF_ID+1];alpm_pivot_t*tcam_pivot[
SOC_MAX_NUM_DEVICES][MAX_PIVOT_COUNT];int soc_alpm_mode_get(int l2){if(
soc_trident2_alpm_mode_get(l2) == 1){return 1;}else{return 0;}}static int l129
(int l2,const void*entry,int*l122){int l130;uint32 l131;int l10;l10 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,MODE0f);if(l10){l131 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l130 = _ipmask2pfx
(l131,l122))<0){return(l130);}l131 = soc_mem_field32_get(l2,L3_DEFIPm,entry,
IP_ADDR_MASK1f);if(*l122){if(l131!= 0xffffffff){return(SOC_E_PARAM);}*l122+= 
32;}else{if((l130 = _ipmask2pfx(l131,l122))<0){return(l130);}}}else{l131 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l130 = _ipmask2pfx
(l131,l122))<0){return(l130);}}return SOC_E_NONE;}int _soc_alpm_rpf_entry(int
l2,int l132){int l133;l133 = (l132>>2)&0x3fff;l133+= SOC_ALPM_BUCKET_COUNT(l2
);return(l132&~(0x3fff<<2))|(l133<<2);}int soc_alpm_physical_idx(int l2,
soc_mem_t l24,int index,int l134){int l135 = index&1;if(l134){return
soc_trident2_l3_defip_index_map(l2,l24,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_map(l2,l24,index);index<<= 1;index|= l135;return
index;}int soc_alpm_logical_idx(int l2,soc_mem_t l24,int index,int l134){int
l135 = index&1;if(l134){return soc_trident2_l3_defip_index_remap(l2,l24,index
);}index>>= 1;index = soc_trident2_l3_defip_index_remap(l2,l24,index);index
<<= 1;index|= l135;return index;}static int l136(int l2,void*entry,uint32*
prefix,uint32*l17,int*l26){int l137,l138,l10;int l122 = 0;int l130 = 
SOC_E_NONE;uint32 l139,l135;prefix[0] = prefix[1] = prefix[2] = prefix[3] = 
prefix[4] = 0;l10 = soc_mem_field32_get(l2,L3_DEFIPm,entry,MODE0f);l137 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR0f);l138 = soc_mem_field32_get(
l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);prefix[1] = l137;l137 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR1f);l138 = soc_mem_field32_get(
l2,L3_DEFIPm,entry,IP_ADDR_MASK1f);prefix[0] = l137;if(l10){prefix[4] = 
prefix[1];prefix[3] = prefix[0];prefix[1] = prefix[0] = 0;l138 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l130 = _ipmask2pfx
(l138,&l122))<0){return(l130);}l138 = soc_mem_field32_get(l2,L3_DEFIPm,entry,
IP_ADDR_MASK1f);if(l122){if(l138!= 0xffffffff){return(SOC_E_PARAM);}l122+= 32
;}else{if((l130 = _ipmask2pfx(l138,&l122))<0){return(l130);}}l139 = 64-l122;
if(l139<32){prefix[4]>>= l139;l135 = (((32-l139) == 32)?0:(prefix[3])<<(32-
l139));prefix[3]>>= l139;prefix[4]|= l135;}else{prefix[4] = (((l139-32) == 32
)?0:(prefix[3])>>(l139-32));prefix[3] = 0;}}else{l138 = soc_mem_field32_get(
l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l130 = _ipmask2pfx(l138,&l122))<0){
return(l130);}prefix[1] = (((32-l122) == 32)?0:(prefix[1])>>(32-l122));prefix
[0] = 0;}*l17 = l122;*l26 = (prefix[0] == 0)&&(prefix[1] == 0)&&(l122 == 0);
return SOC_E_NONE;}int _soc_alpm_find_in_bkt(int l2,soc_mem_t l24,int
bucket_index,int l140,uint32*l15,void*l141,int*l123,int l34){int l130;l130 = 
soc_mem_alpm_lookup(l2,l24,bucket_index,MEM_BLOCK_ANY,l140,l15,l141,l123);if(
SOC_SUCCESS(l130)){return l130;}if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_lookup(l2,l24,bucket_index+1,MEM_BLOCK_ANY,l140,l15,l141,l123);}
return l130;}static int l142(int l2,uint32*prefix,uint32 l143,int l34,int l28
,int*l144,int*l145,int*bucket_index){int l130 = SOC_E_NONE;trie_t*l146;
trie_node_t*l147 = NULL;alpm_pivot_t*l148;if(l34){l146 = VRF_PIVOT_TRIE_IPV6(
l2,l28);}else{l146 = VRF_PIVOT_TRIE_IPV4(l2,l28);}l130 = trie_find_lpm(l146,
prefix,l143,&l147);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"Pivot find failed\n");return l130;}l148 = (alpm_pivot_t*)l147;*l144 = 1;*
l145 = PIVOT_TCAM_INDEX(l148);*bucket_index = PIVOT_BUCKET_INDEX(l148);return
SOC_E_NONE;}static int l149(int l2,void*l8,soc_mem_t l24,void*l141,int*l145,
int*bucket_index,int*l16,int l150){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int
l151,l28,l34;int l123;uint32 l11,l140;int l130 = SOC_E_NONE;int l144 = 0;l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l151,&l28));if(l151 == 0){if(
soc_alpm_mode_get(l2)){return SOC_E_PARAM;}}if(l28 == SOC_VRF_MAX(l2)+1){l11 = 
0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l140);}else{l11 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l140);}if(l151!= SOC_L3_VRF_OVERRIDE){if(
l150){uint32 prefix[5],l143;int l26 = 0;l130 = l136(l2,l8,prefix,&l143,&l26);
if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: prefix create failed\n");return l130;}l130 = l142(l2,
prefix,l143,l34,l28,&l144,l145,bucket_index);SOC_IF_ERROR_RETURN(l130);}else{
defip_aux_scratch_entry_t l13;sal_memset(&l13,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l2,l8,l34,l11,0,&l13));
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,PREFIX_LOOKUP,&l13,TRUE,&l144,l145,
bucket_index));}if(l144){l20(l2,l8,l15,0,l24,0,0);l130 = 
_soc_alpm_find_in_bkt(l2,l24,*bucket_index,l140,l15,l141,&l123,l34);if(
SOC_SUCCESS(l130)){*l16 = l123;}}else{l130 = SOC_E_NOT_FOUND;}}return l130;}
static int l152(int l2,void*l8,void*l141,void*l153,soc_mem_t l24,int l123){
defip_aux_scratch_entry_t l13;int l151,l34,l28;int bucket_index;uint32 l11,
l140;int l130 = SOC_E_NONE;int l144 = 0,l135 = 0;int l145;l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l151,&l28));if(l28 == 
SOC_VRF_MAX(l2)+1){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l140);}else{
l11 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l140);}if(!soc_alpm_mode_get(l2)){
l11 = 2;}if(l151!= SOC_L3_VRF_OVERRIDE){sal_memset(&l13,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l2,l8,l34,l11,0,&l13));
SOC_IF_ERROR_RETURN(soc_mem_write(l2,l24,MEM_BLOCK_ANY,l123,l141));if(
SOC_URPF_STATUS_GET(l2)){SOC_IF_ERROR_RETURN(soc_mem_write(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l123),l153));if(l130!= SOC_E_NONE){
return SOC_E_FAIL;}}l135 = soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,
IP_LENGTHf);soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,REPLACE_LENf,
l135);SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,TRUE,&
l144,&l145,&bucket_index));if(SOC_URPF_STATUS_GET(l2)){l135 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l135+= 1;
soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l135);
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,TRUE,&l144,&
l145,&bucket_index));}}return l130;}int alpm_mem_prefix_array_cb(trie_node_t*
node,void*l154){alpm_mem_prefix_array_t*l155 = (alpm_mem_prefix_array_t*)l154
;if(node->type == PAYLOAD){l155->prefix[l155->count] = (payload_t*)node;l155
->count++;}return SOC_E_NONE;}int alpm_delete_node_cb(trie_node_t*node,void*
l154){if(node!= NULL){sal_free(node);}return SOC_E_NONE;}static int l156(int
l2,int l157,int l34,int l158){int l130,l135,index;defip_aux_table_entry_t
entry;index = l157>>(l34?0:1);l130 = soc_mem_read(l2,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l130);if(l34){
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l158);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l158);l135 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l157&1)
{soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l158);l135 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l158);l135 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l130 = 
soc_mem_write(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l130);if(SOC_URPF_STATUS_GET(l2)){l135++;if(l34){
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l158);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l158);}else{if
(l157&1){soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l158)
;}else{soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l158);}
}soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l135);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l135);index+= (2*
soc_mem_index_count(l2,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l2,L3_DEFIPm))
/2;l130 = soc_mem_write(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}
return l130;}static int l159(int l2,int l160,void*entry,void*l161,int l162){
uint32 l135,l138,l34,l11,l163 = 0;soc_mem_t l24 = L3_DEFIPm;soc_mem_t l164 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l165;int l130 = SOC_E_NONE,l122,l166,l28,
l167;SOC_IF_ERROR_RETURN(soc_mem_read(l2,l164,MEM_BLOCK_ANY,l160,l161));l135 = 
soc_mem_field32_get(l2,l24,entry,VRF_ID_0f);soc_mem_field32_set(l2,l164,l161,
VRF0f,l135);l135 = soc_mem_field32_get(l2,l24,entry,VRF_ID_1f);
soc_mem_field32_set(l2,l164,l161,VRF1f,l135);l135 = soc_mem_field32_get(l2,
l24,entry,MODE0f);soc_mem_field32_set(l2,l164,l161,MODE0f,l135);l135 = 
soc_mem_field32_get(l2,l24,entry,MODE1f);soc_mem_field32_set(l2,l164,l161,
MODE1f,l135);l34 = l135;l135 = soc_mem_field32_get(l2,l24,entry,VALID0f);
soc_mem_field32_set(l2,l164,l161,VALID0f,l135);l135 = soc_mem_field32_get(l2,
l24,entry,VALID1f);soc_mem_field32_set(l2,l164,l161,VALID1f,l135);l135 = 
soc_mem_field32_get(l2,l24,entry,IP_ADDR_MASK0f);if((l130 = _ipmask2pfx(l135,
&l122))<0){return l130;}l138 = soc_mem_field32_get(l2,l24,entry,
IP_ADDR_MASK1f);if((l130 = _ipmask2pfx(l138,&l166))<0){return l130;}if(l34){
soc_mem_field32_set(l2,l164,l161,IP_LENGTH0f,l122+l166);soc_mem_field32_set(
l2,l164,l161,IP_LENGTH1f,l122+l166);}else{soc_mem_field32_set(l2,l164,l161,
IP_LENGTH0f,l122);soc_mem_field32_set(l2,l164,l161,IP_LENGTH1f,l166);}l135 = 
soc_mem_field32_get(l2,l24,entry,IP_ADDR0f);soc_mem_field32_set(l2,l164,l161,
IP_ADDR0f,l135);l135 = soc_mem_field32_get(l2,l24,entry,IP_ADDR1f);
soc_mem_field32_set(l2,l164,l161,IP_ADDR1f,l135);l135 = soc_mem_field32_get(
l2,l24,entry,ENTRY_TYPE0f);soc_mem_field32_set(l2,l164,l161,ENTRY_TYPE0f,l135
);l135 = soc_mem_field32_get(l2,l24,entry,ENTRY_TYPE1f);soc_mem_field32_set(
l2,l164,l161,ENTRY_TYPE1f,l135);if(!l34){sal_memcpy(&l165,entry,sizeof(l165))
;l130 = soc_alpm_lpm_vrf_get(l2,(void*)&l165,&l28,&l122);SOC_IF_ERROR_RETURN(
l130);SOC_IF_ERROR_RETURN(soc_alpm_lpm_ip4entry1_to_0(l2,&l165,&l165,
PRESERVE_HIT));l130 = soc_alpm_lpm_vrf_get(l2,(void*)&l165,&l167,&l122);
SOC_IF_ERROR_RETURN(l130);}else{l130 = soc_alpm_lpm_vrf_get(l2,entry,&l28,&
l122);}if(SOC_URPF_STATUS_GET(l2)){if(l162>= (soc_mem_index_count(l2,
L3_DEFIPm)>>1)){l163 = 1;}}switch(l28){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l2,l164,l161,VALID0f,0);l11 = 0;break;case
SOC_L3_VRF_GLOBAL:l11 = l163?1:0;break;default:l11 = l163?3:2;break;}
soc_mem_field32_set(l2,l164,l161,DB_TYPE0f,l11);if(!l34){switch(l167){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l2,l164,l161,VALID1f,0);l11 = 0;break
;case SOC_L3_VRF_GLOBAL:l11 = l163?1:0;break;default:l11 = l163?3:2;break;}
soc_mem_field32_set(l2,l164,l161,DB_TYPE1f,l11);}else{if(l28 == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l2,l164,l161,VALID1f,0);}
soc_mem_field32_set(l2,l164,l161,DB_TYPE1f,l11);}if(l163){l135 = 
soc_mem_field32_get(l2,l24,entry,ALG_BKT_PTR0f);if(l135){l135+= 
SOC_ALPM_BUCKET_COUNT(l2);soc_mem_field32_set(l2,l24,entry,ALG_BKT_PTR0f,l135
);}if(!l34){l135 = soc_mem_field32_get(l2,l24,entry,ALG_BKT_PTR1f);if(l135){
l135+= SOC_ALPM_BUCKET_COUNT(l2);soc_mem_field32_set(l2,l24,entry,
ALG_BKT_PTR1f,l135);}}}return SOC_E_NONE;}static int l168(int l2,int l169,int
index,int l170,void*entry){defip_aux_table_entry_t l161;l170 = 
soc_alpm_physical_idx(l2,L3_DEFIPm,l170,1);SOC_IF_ERROR_RETURN(l159(l2,l170,
entry,(void*)&l161,index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l2,
MEM_BLOCK_ANY,index,entry));index = soc_alpm_physical_idx(l2,L3_DEFIPm,index,
1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,index,&l161
));return SOC_E_NONE;}int _soc_alpm_insert_in_bkt(int l2,soc_mem_t l24,int
bucket_index,int l140,void*l141,uint32*l15,int*l123,int l34){int l130;l130 = 
soc_mem_alpm_insert(l2,l24,bucket_index,MEM_BLOCK_ANY,l140,l141,l15,l123);if(
l130 == SOC_E_FULL){if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_insert(l2,l24,bucket_index+1,MEM_BLOCK_ANY,l140,l141,l15,l123);}
}return l130;}int _soc_alpm_mem_index(int l2,soc_mem_t l24,int bucket_index,
int l171,uint32 l140,int*l172){int l173,l174 = 0;int l175[4] = {0};int l176 = 
0;int l177 = 0;int l178;int l34 = 0;int l179 = 6;switch(l24){case
L3_DEFIP_ALPM_IPV6_64m:l179 = 4;l34 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:
l179 = 2;l34 = 1;break;default:break;}if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){if(
l171>= ALPM_RAW_BKT_COUNT*l179){bucket_index++;l171-= ALPM_RAW_BKT_COUNT*l179
;}}l178 = (4)-_shr_popcount(l140&((1<<(4))-1));if(bucket_index>= (1<<16)||
l171>= l178*l179){return SOC_E_FULL;}l177 = l171%l179;for(l173 = 0;l173<(4);
l173++){if((1<<l173)&l140){continue;}l175[l174++] = l173;}l176 = l175[l171/
l179];*l172 = (l177<<16)|(bucket_index<<2)|(l176);return SOC_E_NONE;}int l180
(int l21,int l34){if(SOC_ALPM_V6_SCALE_CHECK(l21,l34)){return
ALPM_RAW_BKT_COUNT_DW;}else{return ALPM_RAW_BKT_COUNT;}}int l181(soc_mem_t l24
,int index){uint32 l182 = 0x7;if(l24 == L3_DEFIP_ALPM_IPV6_128m){l182 = 0x3;}
return((uint32)index>>16)&l182;}int l183(int l21,int l34,int index){return
index%l180(l21,l34);}void _soc_alpm_raw_mem_read(int l21,soc_mem_t l24,void*
l184,int index,void*entry){int l10 = 1;int l185;int l186;
defip_alpm_raw_entry_t*l187;soc_mem_info_t l188;soc_field_info_t l189;int l190
= soc_mem_entry_bits(l21,l24)-1;if(l24 == L3_DEFIP_ALPM_IPV4m){l10 = 0;}l185 = 
l183(l21,l10,index);l186 = l181(l24,index);l187 = &((defip_alpm_raw_entry_t*)
l184)[l185];l188.flags = 0;l188.bytes = sizeof(defip_alpm_raw_entry_t);l189.
flags = SOCF_LE;l189.bp = l190*l186;l189.len = l190;(void)
soc_meminfo_fieldinfo_field_get((void*)l187,&l188,&l189,entry);}void
_soc_alpm_raw_mem_write(int l21,soc_mem_t l24,void*l184,int index,void*entry)
{int l10 = 1;int l185;int l186;defip_alpm_raw_entry_t*l187;soc_mem_info_t l188
;soc_field_info_t l189;int l190 = soc_mem_entry_bits(l21,l24)-1;if(l24 == 
L3_DEFIP_ALPM_IPV4m){l10 = 0;}(void)soc_mem_cache_invalidate(l21,l24,
MEM_BLOCK_ANY,index);l185 = l183(l21,l10,index);l186 = l181(l24,index);l187 = 
&((defip_alpm_raw_entry_t*)l184)[l185];l188.flags = 0;l188.bytes = sizeof(
defip_alpm_raw_entry_t);l189.bp = l190*l186;l189.len = l190;l189.flags = 
SOCF_LE;(void)soc_meminfo_fieldinfo_field_set((void*)l187,&l188,&l189,entry);
}int l191(int l2,soc_mem_t l24,int index){return SOC_ALPM_BKT_ENTRY_TO_IDX(
index%(1<<16));}int _soc_alpm_raw_bucket_read(int l2,soc_mem_t l24,int
bucket_index,void*l187,void*l192){int l173,l34 = 1;int l185,l193;
defip_alpm_raw_entry_t*l194 = l187;defip_alpm_raw_entry_t*l195 = l192;if(l24
== L3_DEFIP_ALPM_IPV4m){l34 = 0;}l185 = SOC_ALPM_BKT_IDX_TO_ENTRY(
bucket_index);for(l173 = 0;l173<l180(l2,l34);l173++){SOC_IF_ERROR_RETURN(
soc_mem_read(l2,L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l185+l173,&l194[l173]));if(
SOC_URPF_STATUS_GET(l2)){l193 = _soc_alpm_rpf_entry(l2,l185+l173);
SOC_IF_ERROR_RETURN(soc_mem_read(l2,L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l193,&
l195[l173]));}}return SOC_E_NONE;}int _soc_alpm_raw_bucket_write(int l2,
soc_mem_t l24,int bucket_index,uint32 l140,void*l187,void*l192,int l196){int
l173 = 0,l197,l34 = 1;int l185,l193,l198;defip_alpm_raw_entry_t*l194 = l187;
defip_alpm_raw_entry_t*l195 = l192;int l199 = 6;switch(l24){case
L3_DEFIP_ALPM_IPV4m:l199 = 6;l34 = 0;break;case L3_DEFIP_ALPM_IPV6_64m:l199 = 
4;l34 = 1;break;case L3_DEFIP_ALPM_IPV6_128m:l199 = 2;l34 = 1;break;default:
break;}l185 = SOC_ALPM_BKT_IDX_TO_ENTRY(bucket_index);if(l196 == -1){l198 = 
l180(l2,l34);}else{l198 = (l196/l199)+1;}for(l197 = 0;l197<l180(l2,l34);l197
++){if((1<<(l197%(4)))&l140){continue;}SOC_IF_ERROR_RETURN(soc_mem_write(l2,
L3_DEFIP_ALPM_RAWm,MEM_BLOCK_ANY,l185+l197,&l194[l197]));
_soc_trident2_alpm_bkt_view_set(l2,l185+l197,l24);if(SOC_URPF_STATUS_GET(l2))
{l193 = _soc_alpm_rpf_entry(l2,l185+l197);_soc_trident2_alpm_bkt_view_set(l2,
l193,l24);SOC_IF_ERROR_RETURN(soc_mem_write(l2,L3_DEFIP_ALPM_RAWm,
MEM_BLOCK_ANY,l193,&l195[l197]));}if(++l173 == l198){break;}}return SOC_E_NONE
;}void _soc_alpm_raw_parity_set(int l2,soc_mem_t l24,void*l141){int l173,l200
,l201 = 0;uint32*entry = l141;l200 = soc_mem_entry_words(l2,l24);for(l173 = 0
;l173<l200;l173++){l201+= _shr_popcount(entry[l173]);}if(l201&0x1){
soc_mem_field32_set(l2,l24,l141,EVEN_PARITYf,1);}}static int l202(int l2,void
*l8,soc_mem_t l24,void*l141,void*l153,int*l16,int bucket_index,int l145){
alpm_pivot_t*l148,*l203,*l204;defip_aux_scratch_entry_t l13;uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l205,l143;uint32 l206[5];int l34,
l28,l151;int l123;int l130 = SOC_E_NONE,l207;uint32 l11,l140,l158 = 0;int l144
=0;int l157;int l208 = 0;trie_t*trie,*l209;trie_node_t*l210,*l211 = NULL,*
l147 = NULL;payload_t*l212,*l213,*l214;defip_entry_t lpm_entry;
alpm_bucket_handle_t*l215;int l173,l216 = -1,l26 = 0;alpm_mem_prefix_array_t
l155;defip_alpm_ipv4_entry_t l217,l218;defip_alpm_ipv6_64_entry_t l219,l220;
void*l221,*l222;int*l126 = NULL;trie_t*l146 = NULL;int l223;
defip_alpm_raw_entry_t*l224 = NULL;defip_alpm_raw_entry_t*l187;
defip_alpm_raw_entry_t*l192;defip_alpm_raw_entry_t*l225;
defip_alpm_raw_entry_t*l226;l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f)
;if(l34){if(!(l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l151,&l28));if
(l28 == SOC_VRF_MAX(l2)+1){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l140);
}else{l11 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l140);}l24 = (l34)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l221 = ((l34)?((uint32*)&(l219)):(
(uint32*)&(l217)));l222 = ((l34)?((uint32*)&(l220)):((uint32*)&(l218)));l130 = 
l136(l2,l8,prefix,&l143,&l26);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: prefix create failed\n");return l130;}sal_memset(&l13,0,
sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l2,l8,l34,l11,0,&
l13));if(bucket_index == 0){l130 = l142(l2,prefix,l143,l34,l28,&l144,&l145,&
bucket_index);SOC_IF_ERROR_RETURN(l130);if(l144 == 0){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not find bucket to ""insert prefix\n");return
SOC_E_NOT_FOUND;}}l130 = _soc_alpm_insert_in_bkt(l2,l24,bucket_index,l140,
l141,l15,&l123,l34);if(l130 == SOC_E_NONE){*l16 = l123;if(SOC_URPF_STATUS_GET
(l2)){l207 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l123),
l153);if(SOC_FAILURE(l207)){return l207;}}}if(l130 == SOC_E_FULL){l208 = 1;}
l148 = ALPM_TCAM_PIVOT(l2,l145);trie = PIVOT_BUCKET_TRIE(l148);l204 = l148;
l212 = sal_alloc(sizeof(payload_t),"Payload for Key");if(l212 == NULL){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Unable to allocate memory for "
"trie node \n");return SOC_E_MEMORY;}l213 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l213 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory for ""pfx trie node \n");
sal_free(l212);return SOC_E_MEMORY;}sal_memset(l212,0,sizeof(*l212));
sal_memset(l213,0,sizeof(*l213));l212->key[0] = prefix[0];l212->key[1] = 
prefix[1];l212->key[2] = prefix[2];l212->key[3] = prefix[3];l212->key[4] = 
prefix[4];l212->len = l143;l212->index = l123;sal_memcpy(l213,l212,sizeof(*
l212));l213->bkt_ptr = l212;l130 = trie_insert(trie,prefix,NULL,l143,(
trie_node_t*)l212);if(SOC_FAILURE(l130)){goto l227;}if(l34){l209 = 
VRF_PREFIX_TRIE_IPV6(l2,l28);}else{l209 = VRF_PREFIX_TRIE_IPV4(l2,l28);}if(!
l26){l130 = trie_insert(l209,prefix,NULL,l143,(trie_node_t*)l213);}else{l147 = 
NULL;l130 = trie_find_lpm(l209,0,0,&l147);l214 = (payload_t*)l147;if(
SOC_SUCCESS(l130)){l214->bkt_ptr = l212;}}l205 = l143;if(SOC_FAILURE(l130)){
goto l228;}if(l208){l130 = alpm_bucket_assign(l2,&bucket_index,l34);if(
SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate ""new bucket for split\n");bucket_index
= -1;goto l229;}l130 = trie_split(trie,l34?_MAX_KEY_LEN_144_:_MAX_KEY_LEN_48_
,FALSE,l206,&l143,&l210,NULL,FALSE);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR
,"_soc_alpm_insert: Could not split bucket ""for prefix 0x%08x 0x%08x\n",
prefix[0],prefix[1]);goto l229;}l147 = NULL;l130 = trie_find_lpm(l209,l206,
l143,&l147);l214 = (payload_t*)l147;if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR
,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n",l2,l206[0],l206[1],l206[2
],l206[3],l206[4],l143);goto l229;}if(l214->bkt_ptr){if(l214->bkt_ptr == l212
){sal_memcpy(l221,l141,l34?sizeof(defip_alpm_ipv6_64_entry_t):sizeof(
defip_alpm_ipv4_entry_t));}else{l130 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,((
payload_t*)l214->bkt_ptr)->index,l221);}if(SOC_FAILURE(l130)){goto l229;}l130
= l27(l2,l221,l24,l34,l151,bucket_index,0,&lpm_entry);if(SOC_FAILURE(l130)){
goto l229;}l158 = ((payload_t*)(l214->bkt_ptr))->len;}else{l130 = 
soc_mem_read(l2,L3_DEFIPm,MEM_BLOCK_ANY,soc_alpm_logical_idx(l2,L3_DEFIPm,
l145>>1,1),&lpm_entry);if((!l34)&&(l145&1)){l130 = 
soc_alpm_lpm_ip4entry1_to_0(l2,&lpm_entry,&lpm_entry,0);}}l215 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l215 == NULL){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Unable to allocate memory "
"for PIVOT trie node \n");l130 = SOC_E_MEMORY;goto l229;}sal_memset(l215,0,
sizeof(*l215));l148 = sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot")
;if(l148 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n");l130 = 
SOC_E_MEMORY;goto l229;}sal_memset(l148,0,sizeof(*l148));PIVOT_BUCKET_HANDLE(
l148) = l215;if(l34){l130 = trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(
l148));}else{l130 = trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l148));}
PIVOT_BUCKET_TRIE(l148)->trie = l210;PIVOT_BUCKET_INDEX(l148) = bucket_index;
PIVOT_BUCKET_VRF(l148) = l28;PIVOT_BUCKET_IPV6(l148) = l34;PIVOT_BUCKET_DEF(
l148) = FALSE;l148->key[0] = l206[0];l148->key[1] = l206[1];l148->len = l143;
l148->key[2] = l206[2];l148->key[3] = l206[3];l148->key[4] = l206[4];if(l34){
l146 = VRF_PIVOT_TRIE_IPV6(l2,l28);}else{l146 = VRF_PIVOT_TRIE_IPV4(l2,l28);}
do{if(!(l34)){l206[0] = (((32-l143) == 32)?0:(l206[1])<<(32-l143));l206[1] = 
0;}else{int l230 = 64-l143;int l231;if(l230<32){l231 = l206[3]<<l230;l231|= (
((32-l230) == 32)?0:(l206[4])>>(32-l230));l206[0] = l206[4]<<l230;l206[1] = 
l231;l206[2] = l206[3] = l206[4] = 0;}else{l206[1] = (((l230-32) == 32)?0:(
l206[4])<<(l230-32));l206[0] = l206[2] = l206[3] = l206[4] = 0;}}}while(0);
l30(l2,l206,l143,l28,l34,&lpm_entry,0,0);soc_L3_DEFIPm_field32_set(l2,&
lpm_entry,ALG_BKT_PTR0f,bucket_index);sal_memset(&l155,0,sizeof(l155));l130 = 
trie_traverse(PIVOT_BUCKET_TRIE(l148),alpm_mem_prefix_array_cb,&l155,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Bucket split failed "
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n",prefix[1],prefix[2],prefix[3],
prefix[4]);goto l229;}l126 = sal_alloc(sizeof(*l126)*l155.count,
"Temp storage for location of prefixes in new bucket");if(l126 == NULL){l130 = 
SOC_E_MEMORY;goto l229;}sal_memset(l126,-1,sizeof(*l126)*l155.count);l223 = 
sizeof(defip_alpm_raw_entry_t)*ALPM_RAW_BKT_COUNT_DW;l224 = sal_alloc(4*l223,
"Raw memory buffer");if(l224 == NULL){l130 = SOC_E_MEMORY;goto l229;}
sal_memset(l224,0,4*l223);l187 = (defip_alpm_raw_entry_t*)l224;l192 = (
defip_alpm_raw_entry_t*)((uint8*)l187+l223);l225 = (defip_alpm_raw_entry_t*)(
(uint8*)l192+l223);l226 = (defip_alpm_raw_entry_t*)((uint8*)l225+l223);l130 = 
_soc_alpm_raw_bucket_read(l2,l24,PIVOT_BUCKET_INDEX(l204),(void*)l187,(void*)
l192);if(SOC_FAILURE(l130)){goto l229;}for(l173 = 0;l173<l155.count;l173++){
payload_t*l122 = l155.prefix[l173];if(l122->index>0){_soc_alpm_raw_mem_read(
l2,l24,l187,l122->index,l221);_soc_alpm_raw_mem_write(l2,l24,l187,l122->index
,soc_mem_entry_null(l2,l24));if(SOC_URPF_STATUS_GET(l2)){
_soc_alpm_raw_mem_read(l2,l24,l192,_soc_alpm_rpf_entry(l2,l122->index),l222);
_soc_alpm_raw_mem_write(l2,l24,l192,_soc_alpm_rpf_entry(l2,l122->index),
soc_mem_entry_null(l2,l24));}l130 = _soc_alpm_mem_index(l2,l24,bucket_index,
l173,l140,&l123);if(SOC_SUCCESS(l130)){_soc_alpm_raw_mem_write(l2,l24,l225,
l123,l221);if(SOC_URPF_STATUS_GET(l2)){_soc_alpm_raw_mem_write(l2,l24,l226,
_soc_alpm_rpf_entry(l2,l123),l222);}}}else{l130 = _soc_alpm_mem_index(l2,l24,
bucket_index,l173,l140,&l123);if(SOC_SUCCESS(l130)){l216 = l173;*l16 = l123;
_soc_alpm_raw_parity_set(l2,l24,l141);_soc_alpm_raw_mem_write(l2,l24,l225,
l123,l141);if(SOC_URPF_STATUS_GET(l2)){_soc_alpm_raw_parity_set(l2,l24,l153);
_soc_alpm_raw_mem_write(l2,l24,l226,_soc_alpm_rpf_entry(l2,l123),l153);}}}
l126[l173] = l123;}l130 = _soc_alpm_raw_bucket_write(l2,l24,bucket_index,l140
,(void*)l225,(void*)l226,l155.count);if(SOC_FAILURE(l130)){goto l232;}l130 = 
l5(l2,&lpm_entry,&l157);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to add new ""pivot to tcam\n");if(l130 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l2,l28,l34);}goto l232;}l157 = 
soc_alpm_physical_idx(l2,L3_DEFIPm,l157,l34);l130 = l156(l2,l157,l34,l158);if
(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n",l157);goto l233;}
l130 = trie_insert(l146,l148->key,NULL,l148->len,(trie_node_t*)l148);if(
SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,"failed to insert into pivot trie\n");
goto l233;}ALPM_TCAM_PIVOT(l2,l157<<(l34?1:0)) = l148;PIVOT_TCAM_INDEX(l148) = 
l157<<(l34?1:0);VRF_PIVOT_REF_INC(l2,l28,l34);for(l173 = 0;l173<l155.count;
l173++){l155.prefix[l173]->index = l126[l173];}sal_free(l126);l130 = 
_soc_alpm_raw_bucket_write(l2,l24,PIVOT_BUCKET_INDEX(l204),l140,(void*)l187,(
void*)l192,-1);if(SOC_FAILURE(l130)){goto l233;}if(l216 == -1){l130 = 
_soc_alpm_insert_in_bkt(l2,l24,PIVOT_BUCKET_INDEX(l204),l140,l141,l15,&l123,
l34);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n");
goto l233;}if(SOC_URPF_STATUS_GET(l2)){l130 = soc_mem_write(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l123),l153);}*l16 = l123;l212->index = 
l123;}sal_free(l224);PIVOT_BUCKET_ENT_CNT_UPDATE(l148);VRF_BUCKET_SPLIT_INC(
l2,l28,l34);}VRF_TRIE_ROUTES_INC(l2,l28,l34);if(l26){sal_free(l213);}
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,TRUE,&l144,&
l145,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,INSERT_PROPAGATE
,&l13,FALSE,&l144,&l145,&bucket_index));if(SOC_URPF_STATUS_GET(l2)){l143 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l143+= 1;
soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l143);
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,TRUE,&l144,&
l145,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,INSERT_PROPAGATE
,&l13,FALSE,&l144,&l145,&bucket_index));}PIVOT_BUCKET_ENT_CNT_UPDATE(l204);
return l130;l233:l207 = l7(l2,&lpm_entry);if(SOC_FAILURE(l207)){soc_cm_debug(
DK_ERR,"_soc_alpm_insert: Failure to free new prefix ""at %d\n",
soc_alpm_logical_idx(l2,L3_DEFIPm,l157,l34));}if(ALPM_TCAM_PIVOT(l2,l157<<(
l34?1:0))!= NULL){l207 = trie_delete(l146,l148->key,l148->len,&l211);if(
SOC_FAILURE(l207)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n");}}
ALPM_TCAM_PIVOT(l2,l157<<(l34?1:0)) = NULL;VRF_PIVOT_REF_DEC(l2,l28,l34);l232
:l203 = l204;for(l173 = 0;l173<l155.count;l173++){payload_t*prefix = l155.
prefix[l173];if(l126[l173]!= -1){if(l34){sal_memset(l221,0,sizeof(
defip_alpm_ipv6_64_entry_t));}else{sal_memset(l221,0,sizeof(
defip_alpm_ipv4_entry_t));}l207 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,l126[
l173],l221);_soc_trident2_alpm_bkt_view_set(l2,l126[l173],INVALIDm);if(
SOC_FAILURE(l207)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: mem write failure "
"in bkt move rollback\n");}if(SOC_URPF_STATUS_GET(l2)){l207 = soc_mem_write(
l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l126[l173]),l221);
_soc_trident2_alpm_bkt_view_set(l2,_soc_alpm_rpf_entry(l2,l126[l173]),
INVALIDm);if(SOC_FAILURE(l207)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: urpf mem write ""failure in bkt move rollback\n");}}}l211 = 
NULL;l207 = trie_delete(PIVOT_BUCKET_TRIE(l148),prefix->key,prefix->len,&l211
);l212 = (payload_t*)l211;if(SOC_FAILURE(l207)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n");}if(prefix
->index>0){l207 = trie_insert(PIVOT_BUCKET_TRIE(l203),prefix->key,NULL,prefix
->len,(trie_node_t*)l212);if(SOC_FAILURE(l207)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: trie reinsert failure ""in bkt move rollback\n");}}else{if
(l212!= NULL){sal_free(l212);}}}if(l216 == -1){l211 = NULL;l207 = trie_delete
(PIVOT_BUCKET_TRIE(l203),prefix,l205,&l211);l212 = (payload_t*)l211;if(
SOC_FAILURE(l207)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n",prefix[0],prefix[1]);}if(l212!= 
NULL){sal_free(l212);}}l207 = alpm_bucket_release(l2,PIVOT_BUCKET_INDEX(l148)
,l34);if(SOC_FAILURE(l207)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: new bucket release ""failure: %d\n",PIVOT_BUCKET_INDEX(
l148));}trie_destroy(PIVOT_BUCKET_TRIE(l148));sal_free(l215);sal_free(l148);
sal_free(l126);sal_free(l224);l211 = NULL;l207 = trie_delete(l209,prefix,l205
,&l211);l213 = (payload_t*)l211;if(SOC_FAILURE(l207)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: failed to delete new prefix "
"0x%08x 0x%08x from pfx trie\n",prefix[0],prefix[1]);}if(l213){sal_free(l213)
;}return l130;l229:if(l126!= NULL){sal_free(l126);}if(l224!= NULL){sal_free(
l224);}l211 = NULL;(void)trie_delete(l209,prefix,l205,&l211);l213 = (
payload_t*)l211;if(bucket_index!= -1){(void)alpm_bucket_release(l2,
bucket_index,l34);}l228:l211 = NULL;(void)trie_delete(trie,prefix,l205,&l211)
;l212 = (payload_t*)l211;l227:if(l212!= NULL){sal_free(l212);}if(l213!= NULL)
{sal_free(l213);}return l130;}static int l30(int l21,uint32*key,int len,int
l28,int l10,defip_entry_t*lpm_entry,int l31,int l32){uint32 l182;if(l32){
sal_memset(lpm_entry,0,sizeof(defip_entry_t));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l78),(l28&SOC_VRF_MAX(l21)));if(l28 == (SOC_VRF_MAX(l21)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l80),(0));}else{soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO
(l21,L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l80),(SOC_VRF_MAX(l21)));}if(l10){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l62),(key[0]));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l21,L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l63),(key[1]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l66),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,
L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l67),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l79),(l28&SOC_VRF_MAX(l21)));if(l28 == (SOC_VRF_MAX(l21)+1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l81),(0));}else{soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO
(l21,L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l81),(SOC_VRF_MAX(l21)));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l77),(1));if(len>= 32){l182 = 0xffffffff;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l65),(l182));l182 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32))
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry)
,(l98[(l21)]->l64),(l182));}else{l182 = ~(0xffffffff>>len);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l65),(l182));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l21,L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l64),(0));}}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l62),(key[0]));assert(len<= 32);l182 = (len == 32)?0xffffffff:~(
0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,
L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l64),(l182));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),
(l98[(l21)]->l76),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l21,
L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l68),((1<<soc_mem_field_length(l21,
L3_DEFIPm,MODE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l21,L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l69),((1<<soc_mem_field_length(l21,
L3_DEFIPm,MODE_MASK1f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l21,L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l94),((1<<soc_mem_field_length(l21,
L3_DEFIPm,ENTRY_TYPE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l95),((1<<
soc_mem_field_length(l21,L3_DEFIPm,ENTRY_TYPE_MASK1f))-1));return(SOC_E_NONE)
;}int _soc_alpm_delete_in_bkt(int l2,soc_mem_t l24,int l234,int l140,void*
l235,uint32*l15,int*l123,int l34){int l130;l130 = soc_mem_alpm_delete(l2,l24,
l234,MEM_BLOCK_ALL,l140,l235,l15,l123);if(SOC_SUCCESS(l130)){return l130;}if(
SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return soc_mem_alpm_delete(l2,l24,l234+1,
MEM_BLOCK_ALL,l140,l235,l15,l123);}return l130;}static int l236(int l2,void*
l8,int bucket_index,int l145,int l123){alpm_pivot_t*l148;
defip_alpm_ipv4_entry_t l217,l218,l237;defip_alpm_ipv6_64_entry_t l219,l220,
l238;defip_aux_scratch_entry_t l13;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];
soc_mem_t l24;void*l221,*l235,*l222 = NULL;int l151;int l10;int l130 = 
SOC_E_NONE,l207;uint32 l239[5],prefix[5];int l34,l28;uint32 l143;int l234;
uint32 l11,l140;int l144,l26 = 0;trie_t*trie,*l209;uint32 l240;defip_entry_t
lpm_entry,*l241;payload_t*l212 = NULL,*l242 = NULL,*l214 = NULL;trie_node_t*
l211 = NULL,*l147 = NULL;trie_t*l146 = NULL;l10 = l34 = soc_mem_field32_get(
l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = soc_mem_field32_get(l2,L3_DEFIPm,
l8,MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(
l2,l8,&l151,&l28));if(l151!= SOC_L3_VRF_OVERRIDE){if(l28 == SOC_VRF_MAX(l2)+1
){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l140);}else{l11 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l140);}l130 = l136(l2,l8,prefix,&l143,&l26);
if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: prefix create failed\n");return l130;}if(!
soc_alpm_mode_get(l2)){if(l151!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l2
,l28,l34)>1){if(l26){soc_cm_debug(DK_ERR,"VRF %d: Cannot delete default "
"route if other routes are present in ""this mode",l28);return SOC_E_PARAM;}}
}l11 = 2;}l24 = (l34)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l221 = ((l34
)?((uint32*)&(l219)):((uint32*)&(l217)));l235 = ((l34)?((uint32*)&(l238)):((
uint32*)&(l237)));SOC_ALPM_LPM_LOCK(l2);if(bucket_index == 0){l130 = l149(l2,
l8,l24,l221,&l145,&bucket_index,&l123,TRUE);}else{l130 = l20(l2,l8,l221,0,l24
,0,0);}sal_memcpy(l235,l221,l34?sizeof(l219):sizeof(l217));if(SOC_FAILURE(
l130)){SOC_ALPM_LPM_UNLOCK(l2);soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to find ""prefix for delete\n");return l130;}l234 = 
bucket_index;l148 = ALPM_TCAM_PIVOT(l2,l145);trie = PIVOT_BUCKET_TRIE(l148);
l130 = trie_delete(trie,prefix,l143,&l211);l212 = (payload_t*)l211;if(l130!= 
SOC_E_NONE){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Error prefix not present "
"in trie \n");SOC_ALPM_LPM_UNLOCK(l2);return l130;}if(l34){l209 = 
VRF_PREFIX_TRIE_IPV6(l2,l28);}else{l209 = VRF_PREFIX_TRIE_IPV4(l2,l28);}if(
l34){l146 = VRF_PIVOT_TRIE_IPV6(l2,l28);}else{l146 = VRF_PIVOT_TRIE_IPV4(l2,
l28);}if(!l26){l130 = trie_delete(l209,prefix,l143,&l211);l242 = (payload_t*)
l211;if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n",prefix[0
],prefix[1]);goto l243;}l147 = NULL;l130 = trie_find_lpm(l209,prefix,l143,&
l147);l214 = (payload_t*)l147;if(SOC_SUCCESS(l130)){payload_t*l244 = (
payload_t*)(l214->bkt_ptr);if(l244!= NULL){l240 = l244->len;}else{l240 = 0;}}
else{soc_cm_debug(DK_ERR,"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n",prefix[0],prefix[1]);goto l245;}sal_memcpy(
l239,prefix,sizeof(prefix));do{if(!(l34)){l239[0] = (((32-l143) == 32)?0:(
l239[1])<<(32-l143));l239[1] = 0;}else{int l230 = 64-l143;int l231;if(l230<32
){l231 = l239[3]<<l230;l231|= (((32-l230) == 32)?0:(l239[4])>>(32-l230));l239
[0] = l239[4]<<l230;l239[1] = l231;l239[2] = l239[3] = l239[4] = 0;}else{l239
[1] = (((l230-32) == 32)?0:(l239[4])<<(l230-32));l239[0] = l239[2] = l239[3] = 
l239[4] = 0;}}}while(0);l130 = l30(l2,l239,l240,l28,l10,&lpm_entry,0,1);l207 = 
l149(l2,&lpm_entry,l24,l221,&l145,&bucket_index,&l123,TRUE);(void)l27(l2,l221
,l24,l10,l151,bucket_index,0,&lpm_entry);(void)l30(l2,l239,l143,l28,l10,&
lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l2)){if(SOC_SUCCESS(l130)){l222 = ((l34
)?((uint32*)&(l220)):((uint32*)&(l218)));l207 = soc_mem_read(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l123),l222);}}if((l240 == 0)&&
SOC_FAILURE(l207)){l241 = l34?VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);sal_memcpy(&lpm_entry,l241,sizeof(
lpm_entry));l130 = l30(l2,l239,l143,l28,l10,&lpm_entry,0,1);}if(SOC_FAILURE(
l130)){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n",prefix[0],prefix[1]);goto l245;}l241 = 
&lpm_entry;}else{l147 = NULL;l130 = trie_find_lpm(l209,prefix,l143,&l147);
l214 = (payload_t*)l147;if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Could not find default ""route in the trie for vrf %d\n",
l28);goto l243;}l214->bkt_ptr = NULL;l240 = 0;l241 = l34?
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28):VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);}l130
= l9(l2,l241,l34,l11,l240,&l13);if(SOC_FAILURE(l130)){goto l245;}l130 = 
_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,TRUE,&l144,&l145,&bucket_index);if(
SOC_FAILURE(l130)){goto l245;}if(SOC_URPF_STATUS_GET(l2)){uint32 l135;if(l222
!= NULL){l135 = soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);
l135++;soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l135);l135 = 
soc_mem_field32_get(l2,l24,l222,SRC_DISCARDf);soc_mem_field32_set(l2,l24,&l13
,SRC_DISCARDf,l135);l135 = soc_mem_field32_get(l2,l24,l222,DEFAULTROUTEf);
soc_mem_field32_set(l2,l24,&l13,DEFAULTROUTEf,l135);l130 = _soc_alpm_aux_op(
l2,DELETE_PROPAGATE,&l13,TRUE,&l144,&l145,&bucket_index);}if(SOC_FAILURE(l130
)){goto l245;}}sal_free(l212);if(!l26){sal_free(l242);}
PIVOT_BUCKET_ENT_CNT_UPDATE(l148);if((l148->len!= 0)&&(trie->trie == NULL)){
uint32 l246[5];sal_memcpy(l246,l148->key,sizeof(l246));do{if(!(l10)){l246[0] = 
(((32-l148->len) == 32)?0:(l246[1])<<(32-l148->len));l246[1] = 0;}else{int
l230 = 64-l148->len;int l231;if(l230<32){l231 = l246[3]<<l230;l231|= (((32-
l230) == 32)?0:(l246[4])>>(32-l230));l246[0] = l246[4]<<l230;l246[1] = l231;
l246[2] = l246[3] = l246[4] = 0;}else{l246[1] = (((l230-32) == 32)?0:(l246[4]
)<<(l230-32));l246[0] = l246[2] = l246[3] = l246[4] = 0;}}}while(0);l30(l2,
l246,l148->len,l28,l10,&lpm_entry,0,1);l130 = l7(l2,&lpm_entry);if(
SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Unable to "
"delete pivot 0x%08x 0x%08x \n",l148->key[0],l148->key[1]);}}l130 = 
_soc_alpm_delete_in_bkt(l2,l24,l234,l140,l235,l15,&l123,l34);if(!SOC_SUCCESS(
l130)){SOC_ALPM_LPM_UNLOCK(l2);l130 = SOC_E_FAIL;return l130;}if(
SOC_URPF_STATUS_GET(l2)){l130 = soc_mem_alpm_delete(l2,l24,
SOC_ALPM_RPF_BKT_IDX(l2,l234),MEM_BLOCK_ALL,l140,l235,l15,&l144);if(!
SOC_SUCCESS(l130)){SOC_ALPM_LPM_UNLOCK(l2);l130 = SOC_E_FAIL;return l130;}}if
((l148->len!= 0)&&(trie->trie == NULL)){l130 = alpm_bucket_release(l2,
PIVOT_BUCKET_INDEX(l148),l34);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to release""empty bucket: %d\n",PIVOT_BUCKET_INDEX(
l148));}l130 = trie_delete(l146,l148->key,l148->len,&l211);if(SOC_FAILURE(
l130)){soc_cm_debug(DK_ERR,"could not delete pivot from pivot trie\n");}
trie_destroy(PIVOT_BUCKET_TRIE(l148));sal_free(PIVOT_BUCKET_HANDLE(l148));
sal_free(l148);_soc_trident2_alpm_bkt_view_set(l2,l234<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l2,l34)){_soc_trident2_alpm_bkt_view_set(l2,(l234+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l2,l28,l34);if(VRF_TRIE_ROUTES_CNT(l2,
l28,l34) == 0){l130 = l33(l2,l28,l34);}SOC_ALPM_LPM_UNLOCK(l2);return l130;
l245:l207 = trie_insert(l209,prefix,NULL,l143,(trie_node_t*)l242);if(
SOC_FAILURE(l207)){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into pfx trie\n",prefix[0],prefix[1]);}l243:l207 = 
trie_insert(trie,prefix,NULL,l143,(trie_node_t*)l212);if(SOC_FAILURE(l207)){
soc_cm_debug(DK_ERR,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into bkt trie\n",prefix[0],prefix[1]);}
SOC_ALPM_LPM_UNLOCK(l2);return l130;}int soc_alpm_init(int l2){int l173;int
l130 = SOC_E_NONE;l130 = l3(l2);SOC_IF_ERROR_RETURN(l130);l130 = l19(l2);for(
l173 = 0;l173<MAX_PIVOT_COUNT;l173++){ALPM_TCAM_PIVOT(l2,l173) = NULL;}if(
SOC_CONTROL(l2)->alpm_bulk_retry == NULL){SOC_CONTROL(l2)->alpm_bulk_retry = 
sal_sem_create("ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l2)->
alpm_lookup_retry == NULL){SOC_CONTROL(l2)->alpm_lookup_retry = 
sal_sem_create("ALPM lookup retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l2)->
alpm_insert_retry == NULL){SOC_CONTROL(l2)->alpm_insert_retry = 
sal_sem_create("ALPM insert retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l2)->
alpm_delete_retry == NULL){SOC_CONTROL(l2)->alpm_delete_retry = 
sal_sem_create("ALPM delete retry",sal_sem_BINARY,0);}l130 = 
soc_alpm_128_lpm_init(l2);SOC_IF_ERROR_RETURN(l130);return l130;}static int
l247(int l2){int l173,l130;alpm_pivot_t*l135;for(l173 = 0;l173<
MAX_PIVOT_COUNT;l173++){l135 = ALPM_TCAM_PIVOT(l2,l173);if(l135){if(
PIVOT_BUCKET_HANDLE(l135)){if(PIVOT_BUCKET_TRIE(l135)){l130 = trie_traverse(
PIVOT_BUCKET_TRIE(l135),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l130)){trie_destroy(PIVOT_BUCKET_TRIE(l135));}else{soc_cm_debug(
DK_ERR,"Unable to clear trie state for ""unit %d\n",l2);return l130;}}
sal_free(PIVOT_BUCKET_HANDLE(l135));}sal_free(ALPM_TCAM_PIVOT(l2,l173));
ALPM_TCAM_PIVOT(l2,l173) = NULL;}}for(l173 = 0;l173<= SOC_VRF_MAX(l2)+1;l173
++){l130 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l2,l173),alpm_delete_node_cb,
NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l130)){trie_destroy(
VRF_PREFIX_TRIE_IPV4(l2,l173));}else{soc_cm_debug(DK_ERR,
"unit: %d Unable to clear v4 pfx trie for ""vrf %d\n",l2,l173);return l130;}
l130 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l2,l173),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l130)){trie_destroy(
VRF_PREFIX_TRIE_IPV6(l2,l173));}else{soc_cm_debug(DK_ERR,
"unit: %d Unable to clear v6 pfx trie for ""vrf %d\n",l2,l173);return l130;}
if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l173)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l173));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l173
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l173));}sal_memset(&
alpm_vrf_handle[l2][l173],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l2][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l2,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l2,MAX_VRF_ID,1,1);
VRF_TRIE_INIT_DONE(l2,MAX_VRF_ID,2,1);if(SOC_ALPM_BUCKET_BMAP(l2)!= NULL){
sal_free(SOC_ALPM_BUCKET_BMAP(l2));}sal_memset(&soc_alpm_bucket[l2],0,sizeof(
soc_alpm_bucket_t));return SOC_E_NONE;}int soc_alpm_deinit(int l2){l4(l2);
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_deinit(l2));SOC_IF_ERROR_RETURN(
soc_alpm_128_state_clear(l2));SOC_IF_ERROR_RETURN(l247(l2));if(SOC_CONTROL(l2
)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l2)->alpm_bulk_retry);
SOC_CONTROL(l2)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l2)->
alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l2)->alpm_lookup_retry
);SOC_CONTROL(l2)->alpm_lookup_retry = NULL;}if(SOC_CONTROL(l2)->
alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l2)->alpm_insert_retry
);SOC_CONTROL(l2)->alpm_insert_retry = NULL;}if(SOC_CONTROL(l2)->
alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l2)->alpm_delete_retry
);SOC_CONTROL(l2)->alpm_delete_retry = NULL;}return SOC_E_NONE;}static int
l248(int l2,int l28,int l34){defip_entry_t*lpm_entry,l249;int l250;int index;
int l130 = SOC_E_NONE;uint32 key[2] = {0,0};uint32 l143;alpm_bucket_handle_t*
l215;alpm_pivot_t*l148;payload_t*l242;trie_t*l251;trie_t*l252 = NULL;if(l34 == 
0){trie_init(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(l2,l28));l252 = 
VRF_PIVOT_TRIE_IPV4(l2,l28);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l2,l28));l252 = VRF_PIVOT_TRIE_IPV6(l2,l28);}if(l34 == 0)
{trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l2,l28));l251 = 
VRF_PREFIX_TRIE_IPV4(l2,l28);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6(l2,l28));l251 = VRF_PREFIX_TRIE_IPV6(l2,l28);}lpm_entry = 
sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){
soc_cm_debug(DK_ERR,"soc_alpm_vrf_add: unable to allocate memory for "
"IPv4 LPM entry\n");return SOC_E_MEMORY;}l30(l2,key,0,l28,l34,lpm_entry,0,1);
if(l34 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28) = lpm_entry;}if(l28 == SOC_VRF_MAX(l2)+1)
{soc_L3_DEFIPm_field32_set(l2,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l2,lpm_entry,DEFAULT_MISS0f,1);}l130 = 
alpm_bucket_assign(l2,&l250,l34);soc_L3_DEFIPm_field32_set(l2,lpm_entry,
ALG_BKT_PTR0f,l250);sal_memcpy(&l249,lpm_entry,sizeof(l249));l130 = l5(l2,&
l249,&index);l215 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l215 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n");
return SOC_E_NONE;}sal_memset(l215,0,sizeof(*l215));l148 = sal_alloc(sizeof(
alpm_pivot_t),"Payload for Pivot");if(l148 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n");
sal_free(l215);return SOC_E_MEMORY;}l242 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l242 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n");
sal_free(l215);sal_free(l148);return SOC_E_MEMORY;}sal_memset(l148,0,sizeof(*
l148));sal_memset(l242,0,sizeof(*l242));l143 = 0;PIVOT_BUCKET_HANDLE(l148) = 
l215;if(l34){trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l148));}else{
trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l148));}PIVOT_BUCKET_INDEX(l148
) = l250;PIVOT_BUCKET_VRF(l148) = l28;PIVOT_BUCKET_IPV6(l148) = l34;
PIVOT_BUCKET_DEF(l148) = TRUE;l148->key[0] = l242->key[0] = key[0];l148->key[
1] = l242->key[1] = key[1];l148->len = l242->len = l143;l130 = trie_insert(
l251,key,NULL,l143,&(l242->node));if(SOC_FAILURE(l130)){sal_free(l242);
sal_free(l148);sal_free(l215);return l130;}l130 = trie_insert(l252,key,NULL,
l143,(trie_node_t*)l148);if(SOC_FAILURE(l130)){trie_node_t*l211 = NULL;(void)
trie_delete(l251,key,l143,&l211);sal_free(l242);sal_free(l148);sal_free(l215)
;return l130;}index = soc_alpm_physical_idx(l2,L3_DEFIPm,index,l34);if(l34 == 
0){ALPM_TCAM_PIVOT(l2,index) = l148;PIVOT_TCAM_INDEX(l148) = index;}else{
ALPM_TCAM_PIVOT(l2,index<<1) = l148;PIVOT_TCAM_INDEX(l148) = index<<1;}
VRF_PIVOT_REF_INC(l2,l28,l34);VRF_TRIE_INIT_DONE(l2,l28,l34,1);return l130;}
static int l33(int l2,int l28,int l34){defip_entry_t*lpm_entry;int l250;int
l132;int l130 = SOC_E_NONE;uint32 key[2] = {0,0},l253[SOC_MAX_MEM_FIELD_WORDS
];payload_t*l212;alpm_pivot_t*l254;trie_node_t*l211;trie_t*l251;trie_t*l252 = 
NULL;if(l34 == 0){lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);}else{
lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28);}l250 = 
soc_L3_DEFIPm_field32_get(l2,lpm_entry,ALG_BKT_PTR0f);l130 = 
alpm_bucket_release(l2,l250,l34);_soc_trident2_alpm_bkt_view_set(l2,l250<<2,
INVALIDm);l130 = l18(l2,lpm_entry,(void*)l253,&l132);if(SOC_FAILURE(l130)){
soc_cm_debug(DK_ERR,"soc_alpm_vrf_delete: unable to get internal"
" pivot idx for vrf %d/%d\n",l28,l34);l132 = -1;}l132 = soc_alpm_physical_idx
(l2,L3_DEFIPm,l132,l34);if(l34 == 0){l254 = ALPM_TCAM_PIVOT(l2,l132);}else{
l254 = ALPM_TCAM_PIVOT(l2,l132<<1);}l130 = l7(l2,lpm_entry);if(SOC_FAILURE(
l130)){soc_cm_debug(DK_ERR,"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n",l28,l34);}sal_free(lpm_entry);if(l34
== 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28) = NULL;l251 = VRF_PREFIX_TRIE_IPV4(
l2,l28);VRF_PREFIX_TRIE_IPV4(l2,l28) = NULL;}else{VRF_TRIE_DEFAULT_ROUTE_IPV6
(l2,l28) = NULL;l251 = VRF_PREFIX_TRIE_IPV6(l2,l28);VRF_PREFIX_TRIE_IPV6(l2,
l28) = NULL;}VRF_TRIE_INIT_DONE(l2,l28,l34,0);l130 = trie_delete(l251,key,0,&
l211);l212 = (payload_t*)l211;if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"Unable to delete internal default for vrf "" %d/%d\n",l28,l34);}sal_free(
l212);(void)trie_destroy(l251);if(l34 == 0){l252 = VRF_PIVOT_TRIE_IPV4(l2,l28
);VRF_PIVOT_TRIE_IPV4(l2,l28) = NULL;}else{l252 = VRF_PIVOT_TRIE_IPV6(l2,l28)
;VRF_PIVOT_TRIE_IPV6(l2,l28) = NULL;}l211 = NULL;l130 = trie_delete(l252,key,
0,&l211);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"Unable to delete internal pivot node for vrf"" %d/%d\n",l28,l34);}(void)
trie_destroy(l252);(void)trie_destroy(PIVOT_BUCKET_TRIE(l254));sal_free(
PIVOT_BUCKET_HANDLE(l254));sal_free(l254);return l130;}int soc_alpm_insert(
int l2,void*l6,uint32 l25,int l255,int l256){defip_alpm_ipv4_entry_t l217,
l218;defip_alpm_ipv6_64_entry_t l219,l220;soc_mem_t l24;void*l221,*l235;int
l151,l28;int index;int l10;int l130 = SOC_E_NONE;uint32 l26;l10 = 
soc_mem_field32_get(l2,L3_DEFIPm,l6,MODE0f);l24 = (l10)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l221 = ((l10)?((uint32*)&(l219)):(
(uint32*)&(l217)));l235 = ((l10)?((uint32*)&(l220)):((uint32*)&(l218)));
SOC_IF_ERROR_RETURN(l20(l2,l6,l221,l235,l24,l25,&l26));SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l2,l6,&l151,&l28));if(l151 == SOC_L3_VRF_OVERRIDE){l130 = 
l5(l2,l6,&index);if(SOC_SUCCESS(l130)){VRF_PIVOT_REF_INC(l2,MAX_VRF_ID,l10);
VRF_TRIE_ROUTES_INC(l2,MAX_VRF_ID,l10);}else if(l130 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l2,MAX_VRF_ID,l10);}return(l130);}else if(l28 == 0){if(
soc_alpm_mode_get(l2)){soc_cm_debug(DK_ERR,
"Unit %d, VRF=0 cannot be added in Parallel ""mode\n",l2);return SOC_E_PARAM;
}}if(l151!= SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l2)){if(
VRF_TRIE_ROUTES_CNT(l2,l28,l10) == 0){if(!l26){soc_cm_debug(DK_ERR,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n",
l151);return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){
soc_cm_debug(DK_VERBOSE,"soc_alpm_insert:VRF %d is not ""initialized\n",l28);
l130 = l248(l2,l28,l10);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n",l28,l10);return l130;}
soc_cm_debug(DK_VERBOSE,"soc_alpm_insert:VRF %d/%d trie init ""completed\n",
l28,l10);}if(l256&SOC_ALPM_LOOKUP_HIT){l130 = l152(l2,l6,l221,l235,l24,l255);
}else{if(l255 == -1){l255 = 0;}l130 = l202(l2,l6,l24,l221,l235,&index,
SOC_ALPM_BKT_ENTRY_TO_IDX(l255),l256);}if(l130!= SOC_E_NONE){soc_cm_debug(
DK_WARN,"unit %d :soc_alpm_insert: ""Route Insertion Failed :%s\n",l2,
soc_errmsg(l130));}return(l130);}int soc_alpm_lookup(int l2,void*l8,void*l15,
int*l16,int*l257){defip_alpm_ipv4_entry_t l217;defip_alpm_ipv6_64_entry_t l219
;soc_mem_t l24;int bucket_index;int l145;void*l221;int l151,l28;int l10,l122;
int l130 = SOC_E_NONE;l10 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l151,&l28));l130 = l14(l2,l8,
l15,l16,&l122,&l10);if(SOC_SUCCESS(l130)){if(!l10&&(*l16&0x1)){l130 = 
soc_alpm_lpm_ip4entry1_to_0(l2,l15,l15,PRESERVE_HIT);}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l2,l15,&l151,&l28));if(l151 == SOC_L3_VRF_OVERRIDE){
return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){soc_cm_debug(
DK_VERBOSE,"soc_alpm_lookup:VRF %d is not ""initialized\n",l28);*l257 = 0;
return SOC_E_NOT_FOUND;}l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l221 = ((l10)?((uint32*)&(l219)):((uint32*)&(l217)));
SOC_ALPM_LPM_LOCK(l2);l130 = l149(l2,l8,l24,l221,&l145,&bucket_index,l16,TRUE
);SOC_ALPM_LPM_UNLOCK(l2);if(SOC_FAILURE(l130)){*l257 = l145;*l16 = 
bucket_index<<2;return l130;}l130 = l27(l2,l221,l24,l10,l151,bucket_index,*
l16,l15);*l257 = SOC_ALPM_LOOKUP_HIT|l145;return(l130);}static int l258(int l2
,void*l8,void*l15,int l28,int*l145,int*bucket_index,int*l123,int l259){int
l130 = SOC_E_NONE;int l173,l260,l34,l144 = 0;uint32 l11,l140;
defip_aux_scratch_entry_t l13;int l261,l262;int index;soc_mem_t l24,l263;int
l264,l265;int l266;uint32 l267[SOC_MAX_MEM_FIELD_WORDS] = {0};int l268 = -1;
int l269 = 0;soc_field_t l270[2] = {IP_ADDR0f,IP_ADDR1f,};l263 = L3_DEFIPm;
l34 = soc_mem_field32_get(l2,l263,l8,MODE0f);l261 = soc_mem_field32_get(l2,
l263,l8,GLOBAL_ROUTE0f);l262 = soc_mem_field32_get(l2,l263,l8,VRF_ID_0f);
soc_cm_debug(DK_VERBOSE,"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d:\n",l28 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l34,l261,l262);if(l28 == SOC_L3_VRF_GLOBAL){l11 = l259?1:0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l140);soc_mem_field32_set(l2,l263,l8,
GLOBAL_ROUTE0f,1);soc_mem_field32_set(l2,l263,l8,VRF_ID_0f,0);}else{l11 = 
l259?3:2;SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l140);}sal_memset(&l13,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l2,l8,l34,l11,0,&l13));if(
l28 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l2,l263,l8,GLOBAL_ROUTE0f,l261)
;soc_mem_field32_set(l2,l263,l8,VRF_ID_0f,l262);}SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l2,PREFIX_LOOKUP,&l13,TRUE,&l144,l145,bucket_index));if(l144
== 0){soc_cm_debug(DK_VERBOSE,"Could not find bucket\n");return
SOC_E_NOT_FOUND;}soc_cm_debug(DK_VERBOSE,"Hit in memory %s, index %d, "
"bucket_index %d\n",SOC_MEM_NAME(l2,l263),soc_alpm_logical_idx(l2,l263,(*l145
)>>1,1),*bucket_index);l24 = (l34)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m
;l130 = l129(l2,l8,&l265);if(SOC_FAILURE(l130)){return l130;}l266 = 24;if(l34
){if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){l266 = 32;}else{l266 = 16;}}
soc_cm_debug(DK_VERBOSE,"Start searching mem %s bucket %d(count %d) "
"for Length %d\n",SOC_MEM_NAME(l2,l24),*bucket_index,l266,l265);for(l173 = 0;
l173<l266;l173++){uint32 l221[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l182[2] = 
{0};uint32 l271[2] = {0};uint32 l272[2] = {0};int l273;l130 = 
_soc_alpm_mem_index(l2,l24,*bucket_index,l173,l140,&index);if(l130 == 
SOC_E_FULL){continue;}SOC_IF_ERROR_RETURN(soc_mem_read(l2,l24,MEM_BLOCK_ANY,
index,(void*)l221));l273 = soc_mem_field32_get(l2,l24,l221,VALIDf);l264 = 
soc_mem_field32_get(l2,l24,l221,LENGTHf);soc_cm_debug(DK_VERBOSE,
"Bucket %5d Index %6d: Valid %d, Length %d\n",*bucket_index,index,l273,l264);
if(!l273||(l264>l265)){continue;}SHR_BITSET_RANGE(l182,(l34?64:32)-l264,l264)
;(void)soc_mem_field_get(l2,l24,(uint32*)l221,KEYf,(uint32*)l271);l272[1] = 
soc_mem_field32_get(l2,l263,l8,l270[1]);l272[0] = soc_mem_field32_get(l2,l263
,l8,l270[0]);soc_cm_debug(DK_VERBOSE,"\tmask %08x %08x\n\t key %08x %08x\n"
"\thost %08x %08x\n",l182[1],l182[0],l271[1],l271[0],l272[1],l272[0]);for(
l260 = l34?1:0;l260>= 0;l260--){if((l272[l260]&l182[l260])!= (l271[l260]&l182
[l260])){break;}}if(l260>= 0){continue;}soc_cm_debug(DK_VERBOSE,
"Found a match in mem %s bucket %d, ""index %d\n",SOC_MEM_NAME(l2,l24),*
bucket_index,index);if(l268 == -1||l268<l264){l268 = l264;l269 = index;
sal_memcpy(l267,l221,sizeof(l221));}}if(l268!= -1){l130 = l27(l2,l267,l24,l34
,l28,*bucket_index,l269,l15);if(SOC_SUCCESS(l130)){*l123 = l269;soc_cm_debug(
DK_VERBOSE,"Hit mem %s bucket %d, index %d\n",SOC_MEM_NAME(l2,l24),*
bucket_index,l269);}return l130;}*l123 = soc_alpm_logical_idx(l2,l263,(*l145)
>>1,1);soc_cm_debug(DK_VERBOSE,
"Miss in mem %s bucket %d, use associate data ""in mem %s LOG index %d\n",
SOC_MEM_NAME(l2,l24),*bucket_index,SOC_MEM_NAME(l2,l263),*l123);
SOC_IF_ERROR_RETURN(soc_mem_read(l2,l263,MEM_BLOCK_ANY,*l123,(void*)l15));if(
(!l34)&&((*l145)&1)){l130 = soc_alpm_lpm_ip4entry1_to_0(l2,l15,l15,
PRESERVE_HIT);}return SOC_E_NONE;}int soc_alpm_find_best_match(int l2,void*l8
,void*l15,int*l16,int l259){int l130 = SOC_E_NONE;int l173,l274,l275;
defip_entry_t l276;uint32 l277[2];uint32 l271[2];uint32 l278[2];uint32 l272[2
];uint32 l279,l280;int l151,l28 = 0;int l281[2] = {0};int l145,bucket_index;
soc_mem_t l263 = L3_DEFIPm;int l194,l34,l282,l283 = 0;soc_field_t l284[] = {
GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l285[] = {GLOBAL_ROUTE0f,
GLOBAL_ROUTE1f};l34 = soc_mem_field32_get(l2,l263,l8,MODE0f);if(!
SOC_URPF_STATUS_GET(l2)&&l259){return SOC_E_PARAM;}l274 = soc_mem_index_min(
l2,l263);l275 = soc_mem_index_count(l2,l263);if(SOC_URPF_STATUS_GET(l2)){l275
>>= 1;}if(soc_alpm_mode_get(l2)){l275>>= 1;l274+= l275;}if(l259){l274+= 
soc_mem_index_count(l2,l263)/2;}soc_cm_debug(DK_VERBOSE,
"Launch LPM searching from index %d count %d\n",l274,l275);for(l173 = l274;
l173<l274+l275;l173++){SOC_IF_ERROR_RETURN(soc_mem_read(l2,l263,MEM_BLOCK_ANY
,l173,(void*)&l276));l281[0] = soc_mem_field32_get(l2,l263,&l276,VALID0f);
l281[1] = soc_mem_field32_get(l2,l263,&l276,VALID1f);if(l281[0] == 0&&l281[1]
== 0){continue;}l282 = soc_mem_field32_get(l2,l263,&l276,MODE0f);if(l282!= 
l34){continue;}for(l194 = 0;l194<(l34?1:2);l194++){if(l281[l194] == 0){
continue;}l279 = soc_mem_field32_get(l2,l263,&l276,l284[l194]);l280 = 
soc_mem_field32_get(l2,l263,&l276,l285[l194]);if(!l279||!l280){continue;}
soc_cm_debug(DK_VERBOSE,"Match a Global High route: ent %d\n",l194);l277[0] = 
soc_mem_field32_get(l2,l263,&l276,IP_ADDR_MASK0f);l277[1] = 
soc_mem_field32_get(l2,l263,&l276,IP_ADDR_MASK1f);l271[0] = 
soc_mem_field32_get(l2,l263,&l276,IP_ADDR0f);l271[1] = soc_mem_field32_get(l2
,l263,&l276,IP_ADDR1f);l278[0] = soc_mem_field32_get(l2,l263,l8,
IP_ADDR_MASK0f);l278[1] = soc_mem_field32_get(l2,l263,l8,IP_ADDR_MASK1f);l272
[0] = soc_mem_field32_get(l2,l263,l8,IP_ADDR0f);l272[1] = soc_mem_field32_get
(l2,l263,l8,IP_ADDR1f);soc_cm_debug(DK_VERBOSE,
"\thmsk %08x %08x\n\thkey %08x %08x\n""\tsmsk %08x %08x\n\tskey %08x %08x\n",
l277[1],l277[0],l271[1],l271[0],l278[1],l278[0],l272[1],l272[0]);if(l34&&(((
l277[1]&l278[1])!= l277[1])||((l277[0]&l278[0])!= l277[0]))){continue;}if(!
l34&&((l277[l194]&l278[0])!= l277[l194])){continue;}if(l34&&((l272[0]&l277[0]
) == (l271[0]&l277[0]))&&((l272[1]&l277[1]) == (l271[1]&l277[1]))){l283 = 
TRUE;break;}if(!l34&&((l272[0]&l277[l194]) == (l271[l194]&l277[l194]))){l283 = 
TRUE;break;}}if(!l283){continue;}soc_cm_debug(DK_VERBOSE,
"Hit Global High route in index = %d(%d)\n",l173,l194);sal_memcpy(l15,&l276,
sizeof(l276));if(!l34&&l194 == 1){l130 = soc_alpm_lpm_ip4entry1_to_0(l2,l15,
l15,PRESERVE_HIT);}*l16 = l173;return l130;}soc_cm_debug(DK_VERBOSE,
"Global high lookup miss, use AUX engine to "
"search for VRF and Global Low routes\n");SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l2,l8,&l151,&l28));l130 = l258(l2,l8,l15,l28,&l145,&
bucket_index,l16,l259);if(l130 == SOC_E_NOT_FOUND){l28 = SOC_L3_VRF_GLOBAL;
soc_cm_debug(DK_VERBOSE,"Not found in VRF region, trying Global ""region\n");
l130 = l258(l2,l8,l15,l28,&l145,&bucket_index,l16,l259);}if(SOC_SUCCESS(l130)
){soc_cm_debug(DK_VERBOSE,"Hit in %s region in TCAM index %d, "
"buckekt_index %d\n",l28 == SOC_L3_VRF_GLOBAL?"Global Low":"VRF",l145,
bucket_index);}else{soc_cm_debug(DK_VERBOSE,"Search miss for given address\n"
);}return(l130);}int soc_alpm_delete(int l2,void*l8,int l255,int l256){int
l151,l28;int l10;int l130 = SOC_E_NONE;l10 = soc_mem_field32_get(l2,L3_DEFIPm
,l8,MODE0f);SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l151,&l28));if(
l151 == SOC_L3_VRF_OVERRIDE){l130 = l7(l2,l8);if(SOC_SUCCESS(l130)){
VRF_PIVOT_REF_DEC(l2,MAX_VRF_ID,l10);VRF_TRIE_ROUTES_DEC(l2,MAX_VRF_ID,l10);}
return(l130);}else{if(!VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){soc_cm_debug(
DK_VERBOSE,"soc_alpm_delete:VRF %d/%d is not ""initialized\n",l28,l10);return
SOC_E_NONE;}if(l255 == -1){l255 = 0;}l130 = l236(l2,l8,
SOC_ALPM_BKT_ENTRY_TO_IDX(l255),l256&~SOC_ALPM_LOOKUP_HIT,l255);}return(l130)
;}static int l19(int l2){int l286;l286 = soc_mem_index_count(l2,L3_DEFIPm)+
soc_mem_index_count(l2,L3_DEFIP_PAIR_128m)*2;if(SOC_URPF_STATUS_GET(l2)){l286
>>= 1;}SOC_ALPM_BUCKET_COUNT(l2) = l286*2;SOC_ALPM_BUCKET_BMAP_SIZE(l2) = 
SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(l2));SOC_ALPM_BUCKET_BMAP(l2) = 
sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l2),"alpm_shared_bucket_bitmap");if(
SOC_ALPM_BUCKET_BMAP(l2) == NULL){soc_cm_debug(DK_WARN,
"soc_alpm_shared_mem_init: Memory allocation for bucket bitmap management failed\n"
);return SOC_E_MEMORY;}sal_memset(SOC_ALPM_BUCKET_BMAP(l2),0,
SOC_ALPM_BUCKET_BMAP_SIZE(l2));alpm_bucket_assign(l2,&l286,1);return
SOC_E_NONE;}int alpm_bucket_assign(int l2,int*l250,int l34){int l173,l287 = 1
,l288 = 0;if(l34){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l287 = 
2;}}for(l173 = 0;l173<SOC_ALPM_BUCKET_COUNT(l2);l173+= l287){
SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l173,l287,l288);if(0 == l288){
break;}}if(l173 == SOC_ALPM_BUCKET_COUNT(l2)){return SOC_E_FULL;}
SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l173,l287);*l250 = l173;
SOC_ALPM_BUCKET_NEXT_FREE(l2) = l173;return SOC_E_NONE;}int
alpm_bucket_release(int l2,int l250,int l34){int l287 = 1,l288 = 0;if((l250<1
)||(l250>SOC_ALPM_BUCKET_MAX_INDEX(l2))){soc_cm_debug(DK_ERR,
"Unit %d\n, freeing invalid bucket index %d\n",l2,l250);return SOC_E_PARAM;}
if(l34){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l287 = 2;}}
SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l250,l287,l288);if(!l288){return
SOC_E_PARAM;}SHR_BITCLR_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l250,l287);return
SOC_E_NONE;}int alpm_bucket_is_assigned(int l2,int l289,int l10,int*l288){int
l287 = 1;if((l289<1)||(l289>SOC_ALPM_BUCKET_MAX_INDEX(l2))){return SOC_E_PARAM
;}if(l10){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l287 = 2;}}
SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l289,l287,*l288);return SOC_E_NONE
;}static void l110(int l2,void*l15,int index,l105 l111){if(index&(0x8000)){
l111[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l15),(l98[(l2)]->l62));l111[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(l2)]->l64));l111[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l63));l111[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l98[(l2)]->l65));if((!(SOC_IS_HURRICANE(l2)))&&(((l98[(l2)
]->l78)!= NULL))){int l290;(void)soc_alpm_lpm_vrf_get(l2,l15,(int*)&l111[4],&
l290);}else{l111[4] = 0;};}else{if(index&0x1){l111[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l63));l111[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l98[(l2)]->l65));l111[2] = 0;l111[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l2)))&&(((l98[(l2)]->l79)!= NULL))){int l290;defip_entry_t
l291;(void)soc_alpm_lpm_ip4entry1_to_0(l2,l15,&l291,0);(void)
soc_alpm_lpm_vrf_get(l2,&l291,(int*)&l111[4],&l290);}else{l111[4] = 0;};}else
{l111[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l15),(l98[(l2)]->l62));l111[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(l2)]->l64));l111[2] = 0;l111[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((l98[(l2)]->l78)!= NULL))){int l290
;(void)soc_alpm_lpm_vrf_get(l2,l15,(int*)&l111[4],&l290);}else{l111[4] = 0;};
}}}static int l292(l105 l107,l105 l108){int l132;for(l132 = 0;l132<5;l132++){
{if((l107[l132])<(l108[l132])){return-1;}if((l107[l132])>(l108[l132])){return
1;}};}return(0);}static void l293(int l2,void*l6,uint32 l294,uint32 l125,int
l122){l105 l295;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l98[(l2)]->l66))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l98[(l2)]->l77))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l98[(l2
)]->l76))){l295[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l98[(l2)]->l62));l295[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l98[(l2
)]->l64));l295[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l98[(l2)]->l63));l295[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l98[(l2
)]->l65));if((!(SOC_IS_HURRICANE(l2)))&&(((l98[(l2)]->l78)!= NULL))){int l290
;(void)soc_alpm_lpm_vrf_get(l2,l6,(int*)&l295[4],&l290);}else{l295[4] = 0;};
l124((l109[(l2)]),l292,l295,l122,l125,((uint16)l294<<1)|(0x8000));}}else{if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l98[(l2
)]->l76))){l295[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l98[(l2)]->l62));l295[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l98[(l2
)]->l64));l295[2] = 0;l295[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((
l98[(l2)]->l78)!= NULL))){int l290;(void)soc_alpm_lpm_vrf_get(l2,l6,(int*)&
l295[4],&l290);}else{l295[4] = 0;};l124((l109[(l2)]),l292,l295,l122,l125,((
uint16)l294<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l98[(l2)]->l77))){l295[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l98[(l2
)]->l63));l295[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l98[(l2)]->l65));l295[2] = 0;l295[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l2)))&&(((l98[(l2)]->l79)!= NULL))){int l290;defip_entry_t
l291;(void)soc_alpm_lpm_ip4entry1_to_0(l2,l6,&l291,0);(void)
soc_alpm_lpm_vrf_get(l2,&l291,(int*)&l295[4],&l290);}else{l295[4] = 0;};l124(
(l109[(l2)]),l292,l295,l122,l125,(((uint16)l294<<1)+1));}}}static void l296(
int l2,void*l8,uint32 l294){l105 l295;int l122 = -1;int l130;uint16 index;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2
)]->l66))){l295[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l98[(l2)]->l62));l295[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2
)]->l64));l295[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l98[(l2)]->l63));l295[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2
)]->l65));if((!(SOC_IS_HURRICANE(l2)))&&(((l98[(l2)]->l78)!= NULL))){int l290
;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&l295[4],&l290);}else{l295[4] = 0;};
index = (l294<<1)|(0x8000);}else{l295[0] = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2)]->l62));l295[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2
)]->l64));l295[2] = 0;l295[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((
l98[(l2)]->l78)!= NULL))){int l290;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&
l295[4],&l290);}else{l295[4] = 0;};index = l294;}l130 = l127((l109[(l2)]),
l292,l295,l122,index);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"\ndel  index: H %d error %d\n",index,l130);}}static int l297(int l2,void*l8,
int l122,int*l123){l105 l295;int l298;int l130;uint16 index = (0xFFFF);l298 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2
)]->l66));if(l298){l295[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l8),(l98[(l2)]->l62));l295[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2
)]->l64));l295[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l98[(l2)]->l63));l295[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2
)]->l65));if((!(SOC_IS_HURRICANE(l2)))&&(((l98[(l2)]->l78)!= NULL))){int l290
;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&l295[4],&l290);}else{l295[4] = 0;};}
else{l295[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)
),(l8),(l98[(l2)]->l62));l295[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2)]->l64));l295[2] = 0;l295[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((l98[(l2)]->l78)!= NULL))){int l290
;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&l295[4],&l290);}else{l295[4] = 0;};}
l130 = l119((l109[(l2)]),l292,l295,l122,&index);if(SOC_FAILURE(l130)){*l123 = 
0xFFFFFFFF;return(l130);}*l123 = index;return(SOC_E_NONE);}static uint16 l112
(uint8*l113,int l114){return(_shr_crc16b(0,l113,l114));}static int l115(int
l21,int l100,int l101,l104**l116){l104*l120;int index;if(l101>l100){return
SOC_E_MEMORY;}l120 = sal_alloc(sizeof(l104),"lpm_hash");if(l120 == NULL){
return SOC_E_MEMORY;}sal_memset(l120,0,sizeof(*l120));l120->l21 = l21;l120->
l100 = l100;l120->l101 = l101;l120->l102 = sal_alloc(l120->l101*sizeof(*(l120
->l102)),"hash_table");if(l120->l102 == NULL){sal_free(l120);return
SOC_E_MEMORY;}l120->l103 = sal_alloc(l120->l100*sizeof(*(l120->l103)),
"link_table");if(l120->l103 == NULL){sal_free(l120->l102);sal_free(l120);
return SOC_E_MEMORY;}for(index = 0;index<l120->l101;index++){l120->l102[index
] = (0xFFFF);}for(index = 0;index<l120->l100;index++){l120->l103[index] = (
0xFFFF);}*l116 = l120;return SOC_E_NONE;}static int l117(l104*l118){if(l118!= 
NULL){sal_free(l118->l102);sal_free(l118->l103);sal_free(l118);}return
SOC_E_NONE;}static int l119(l104*l120,l106 l121,l105 entry,int l122,uint16*
l123){int l2 = l120->l21;uint16 l299;uint16 index;l299 = l112((uint8*)entry,(
32*5))%l120->l101;index = l120->l102[l299];;;while(index!= (0xFFFF)){uint32
l15[SOC_MAX_MEM_FIELD_WORDS];l105 l111;int l300;l300 = (index&(0x3FFF))>>1;
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l300,l15));l110(l2,l15,
index,l111);if((*l121)(entry,l111) == 0){*l123 = (index&(0x3FFF))>>((index&(
0x8000))?1:0);;return(SOC_E_NONE);}index = l120->l103[index&(0x3FFF)];;};
return(SOC_E_NOT_FOUND);}static int l124(l104*l120,l106 l121,l105 entry,int
l122,uint16 l125,uint16 l126){int l2 = l120->l21;uint16 l299;uint16 index;
uint16 l301;l299 = l112((uint8*)entry,(32*5))%l120->l101;index = l120->l102[
l299];;;;l301 = (0xFFFF);if(l125!= (0xFFFF)){while(index!= (0xFFFF)){uint32
l15[SOC_MAX_MEM_FIELD_WORDS];l105 l111;int l300;l300 = (index&(0x3FFF))>>1;
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l300,l15));l110(l2,l15,
index,l111);if((*l121)(entry,l111) == 0){if(l126!= index){;if(l301 == (0xFFFF
)){l120->l102[l299] = l126;l120->l103[l126&(0x3FFF)] = l120->l103[index&(
0x3FFF)];l120->l103[index&(0x3FFF)] = (0xFFFF);}else{l120->l103[l301&(0x3FFF)
] = l126;l120->l103[l126&(0x3FFF)] = l120->l103[index&(0x3FFF)];l120->l103[
index&(0x3FFF)] = (0xFFFF);}};return(SOC_E_NONE);}l301 = index;index = l120->
l103[index&(0x3FFF)];;}}l120->l103[l126&(0x3FFF)] = l120->l102[l299];l120->
l102[l299] = l126;return(SOC_E_NONE);}static int l127(l104*l120,l106 l121,
l105 entry,int l122,uint16 l128){uint16 l299;uint16 index;uint16 l301;l299 = 
l112((uint8*)entry,(32*5))%l120->l101;index = l120->l102[l299];;;l301 = (
0xFFFF);while(index!= (0xFFFF)){if(l128 == index){;if(l301 == (0xFFFF)){l120
->l102[l299] = l120->l103[l128&(0x3FFF)];l120->l103[l128&(0x3FFF)] = (0xFFFF)
;}else{l120->l103[l301&(0x3FFF)] = l120->l103[l128&(0x3FFF)];l120->l103[l128&
(0x3FFF)] = (0xFFFF);}return(SOC_E_NONE);}l301 = index;index = l120->l103[
index&(0x3FFF)];;}return(SOC_E_NOT_FOUND);}int _ipmask2pfx(uint32 l35,int*l36
){*l36 = 0;while(l35&(1<<31)){*l36+= 1;l35<<= 1;}return((l35)?SOC_E_PARAM:
SOC_E_NONE);}int soc_alpm_lpm_ip4entry0_to_0(int l2,void*l302,void*l303,int
l304){uint32 l131;l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l302),(l98[(l2)]->l76));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l76),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l66),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l62));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l62),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l64),(l131));if(((l98[(l2)]->
l52)!= NULL)){l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l302),(l98[(l2)]->l52));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l52),(l131));}if(((l98[(l2)]->
l54)!= NULL)){l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l302),(l98[(l2)]->l54));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l54),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l56),(l131));}else{l131 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l70));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l70),(l131));}l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l302),(l98[(l2)]->l72));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l72),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l74),(l131));if(((l98[(l2)]->l78)!= NULL)){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l78),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l80),(l131));}if(((l98[(l2)]->l50)!= NULL)){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l50),(l131));}if(l304){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l60),(l131));}l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l82),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l84),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l86),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l88));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l88),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l90));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l90),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l92));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l92),(l131));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_1(int l2,void*l302,void*l303,int l304){uint32 l131;
l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302)
,(l98[(l2)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l303),(l98[(l2)]->l77),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l67));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l67),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l63));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l63),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l65));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l65),(l131));if(((l98[(l2)]->
l53)!= NULL)){l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l302),(l98[(l2)]->l53));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l53),(l131));}if(((l98[(l2)]->
l55)!= NULL)){l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l302),(l98[(l2)]->l55));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l55),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l57));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l57),(l131));}else{l131 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l71));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l71),(l131));}l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l302),(l98[(l2)]->l73));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l73),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l75),(l131));if(((l98[(l2)]->l79)!= NULL)){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l79));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l79),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l81),(l131));}if(((l98[(l2)]->l51)!= NULL)){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l51),(l131));}if(l304){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l61));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l61),(l131));}l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l83));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l83),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l85));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l85),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l87),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l89));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l89),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l91));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l91),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l93));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l93),(l131));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_1(int l2,void*l302,void*l303,int l304){uint32 l131;
l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302)
,(l98[(l2)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l303),(l98[(l2)]->l77),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l67),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l62));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l63),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l65),(l131));if(!
SOC_IS_HURRICANE(l2)){l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l52));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l53),(l131));}if(((l98[(l2)]->
l54)!= NULL)){l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l302),(l98[(l2)]->l54));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l55),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l57),(l131));}else{l131 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l70));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l71),(l131));}l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l302),(l98[(l2)]->l72));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l73),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l75),(l131));if(((l98[(l2)]->l78)!= NULL)){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l79),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l81),(l131));}if(((l98[(l2)]->l50)!= NULL)){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l51),(l131));}if(l304){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l61),(l131));}l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l83),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l85),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l87),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l88));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l89),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l90));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l91),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l92));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l93),(l131));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_0(int l2,void*l302,void*l303,int l304){uint32 l131;
l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302)
,(l98[(l2)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l303),(l98[(l2)]->l76),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l67));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l66),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l63));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l62),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l65));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l64),(l131));if(!
SOC_IS_HURRICANE(l2)){l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l53));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l52),(l131));}if(((l98[(l2)]->
l55)!= NULL)){l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l302),(l98[(l2)]->l55));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l54),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l57));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l56),(l131));}else{l131 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l71));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l70),(l131));}l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l302),(l98[(l2)]->l73));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l72),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l74),(l131));if(((l98[(l2)]->l79)!= NULL)){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l79));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l78),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l80),(l131));}if(((l98[(l2)]->l51)!= NULL)){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l50),(l131));}if(l304){l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l61));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l60),(l131));}l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l83));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l82),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l85));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l84),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l86),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(l2)]->l89));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(
l2)]->l88),(l131));l131 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l302),(l98[(l2)]->l91));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l303),(l98[(l2)]->l90),(l131));l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l302),(l98[(
l2)]->l93));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l303),(l98[(l2)]->l92),(l131));return(SOC_E_NONE);}static int l305(int l2,
void*l15){return(SOC_E_NONE);}void l1(int l2){int l173;int l306;l306 = ((3*(
64+32+2+1))-1);if(!soc_cm_debug_check(DK_L3|DK_SOCMEM|DK_VERBOSE)){return;}
for(l173 = l306;l173>= 0;l173--){if((l173!= ((3*(64+32+2+1))-1))&&((l46[(l2)]
[(l173)].l39) == -1)){continue;}soc_cm_debug(DK_L3|DK_SOCMEM|DK_VERBOSE,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n",l173,(l46[
(l2)][(l173)].l41),(l46[(l2)][(l173)].next),(l46[(l2)][(l173)].l39),(l46[(l2)
][(l173)].l40),(l46[(l2)][(l173)].l42),(l46[(l2)][(l173)].l43));}
COMPILER_REFERENCE(l305);}static int l307(int l2,int index,uint32*l15){int
l308;int l10;uint32 l309;uint32 l310;int l311;if(!SOC_URPF_STATUS_GET(l2)){
return(SOC_E_NONE);}if(soc_feature(l2,soc_feature_l3_defip_hole)){l308 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l2)){l308 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1)+0x0400;}else{l308 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1);}l10 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l66));if(((l98[(l2)]->l50)!= NULL)){soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(l2)]->l50),(0));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l51),(0));}l309 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l98[(l2)]->l64));l310 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(l2)]->l65));if(!l10){if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l76))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l15),(l98[(l2)]->l74),((!l309)?1:0));}if(soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(l2)]->l77))){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l75),((!l310)?1:0));}}else{l311 = ((!l309)&&(!l310))?1:0;l309 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l76));l310 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l98[(l2)]->l77));if(l309&&l310){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l74),(l311));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l98[(l2)]->l75),(l311));}}return l168(l2,MEM_BLOCK_ANY,
index+l308,index,l15);}static int l312(int l2,int l313,int l314){uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,
l313,l15));l293(l2,l15,l314,0x4000,0);SOC_IF_ERROR_RETURN(l168(l2,
MEM_BLOCK_ANY,l314,l313,l15));SOC_IF_ERROR_RETURN(l307(l2,l314,l15));do{int
l315,l316;l315 = soc_alpm_physical_idx((l2),L3_DEFIPm,(l313),1);l316 = 
soc_alpm_physical_idx((l2),L3_DEFIPm,(l314),1);ALPM_TCAM_PIVOT(l2,l316<<1) = 
ALPM_TCAM_PIVOT(l2,l315<<1);ALPM_TCAM_PIVOT(l2,(l316<<1)+1) = ALPM_TCAM_PIVOT
(l2,(l315<<1)+1);if(ALPM_TCAM_PIVOT((l2),l316<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l2),l316<<1)) = l316<<1;}if(ALPM_TCAM_PIVOT((l2),(l316<<1)+1
)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l2),(l316<<1)+1)) = (l316<<1)+1;}
ALPM_TCAM_PIVOT(l2,l315<<1) = NULL;ALPM_TCAM_PIVOT(l2,(l315<<1)+1) = NULL;}
while(0);return(SOC_E_NONE);}static int l317(int l2,int l122,int l10){uint32
l15[SOC_MAX_MEM_FIELD_WORDS];int l313;int l314;uint32 l318,l319;l314 = (l46[(
l2)][(l122)].l40)+1;if(!l10){l313 = (l46[(l2)][(l122)].l40);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l313,l15));l318 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l76));l319 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l98[(l2)]->l77));if((l318 == 0)||(l319 == 0)){l293(l2,l15,
l314,0x4000,0);SOC_IF_ERROR_RETURN(l168(l2,MEM_BLOCK_ANY,l314,l313,l15));
SOC_IF_ERROR_RETURN(l307(l2,l314,l15));do{int l320 = soc_alpm_physical_idx((
l2),L3_DEFIPm,(l313),1)<<1;int l231 = soc_alpm_physical_idx((l2),L3_DEFIPm,(
l314),1)*2+(l320&1);if((l319)){l320++;}ALPM_TCAM_PIVOT((l2),l231) = 
ALPM_TCAM_PIVOT((l2),l320);if(ALPM_TCAM_PIVOT((l2),l231)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l2),l231)) = l231;}ALPM_TCAM_PIVOT((l2),l320) = NULL;}while(
0);l314--;}}l313 = (l46[(l2)][(l122)].l39);if(l313!= l314){
SOC_IF_ERROR_RETURN(l312(l2,l313,l314));VRF_PIVOT_SHIFT_INC(l2,MAX_VRF_ID,l10
);}(l46[(l2)][(l122)].l39)+= 1;(l46[(l2)][(l122)].l40)+= 1;return(SOC_E_NONE)
;}static int l321(int l2,int l122,int l10){uint32 l15[SOC_MAX_MEM_FIELD_WORDS
];int l313;int l314;int l322;uint32 l318,l319;l314 = (l46[(l2)][(l122)].l39)-
1;if((l46[(l2)][(l122)].l42) == 0){(l46[(l2)][(l122)].l39) = l314;(l46[(l2)][
(l122)].l40) = l314-1;return(SOC_E_NONE);}if((!l10)&&((l46[(l2)][(l122)].l40)
!= (l46[(l2)][(l122)].l39))){l313 = (l46[(l2)][(l122)].l40);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l313,l15));l318 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l76));l319 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l98[(l2)]->l77));if((l318 == 0)||(l319 == 0)){l322 = l313-
1;SOC_IF_ERROR_RETURN(l312(l2,l322,l314));VRF_PIVOT_SHIFT_INC(l2,MAX_VRF_ID,
l10);l293(l2,l15,l322,0x4000,0);SOC_IF_ERROR_RETURN(l168(l2,MEM_BLOCK_ANY,
l322,l313,l15));SOC_IF_ERROR_RETURN(l307(l2,l322,l15));do{int l320 = 
soc_alpm_physical_idx((l2),L3_DEFIPm,(l313),1)<<1;int l231 = 
soc_alpm_physical_idx((l2),L3_DEFIPm,(l322),1)*2+(l320&1);if((l319)){l320++;}
ALPM_TCAM_PIVOT((l2),l231) = ALPM_TCAM_PIVOT((l2),l320);if(ALPM_TCAM_PIVOT((
l2),l231)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l2),l231)) = l231;}
ALPM_TCAM_PIVOT((l2),l320) = NULL;}while(0);}else{l293(l2,l15,l314,0x4000,0);
SOC_IF_ERROR_RETURN(l168(l2,MEM_BLOCK_ANY,l314,l313,l15));SOC_IF_ERROR_RETURN
(l307(l2,l314,l15));do{int l315,l316;l315 = soc_alpm_physical_idx((l2),
L3_DEFIPm,(l313),1);l316 = soc_alpm_physical_idx((l2),L3_DEFIPm,(l314),1);
ALPM_TCAM_PIVOT(l2,l316<<1) = ALPM_TCAM_PIVOT(l2,l315<<1);ALPM_TCAM_PIVOT(l2,
(l316<<1)+1) = ALPM_TCAM_PIVOT(l2,(l315<<1)+1);if(ALPM_TCAM_PIVOT((l2),l316<<
1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l2),l316<<1)) = l316<<1;}if(
ALPM_TCAM_PIVOT((l2),(l316<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l2),(
l316<<1)+1)) = (l316<<1)+1;}ALPM_TCAM_PIVOT(l2,l315<<1) = NULL;
ALPM_TCAM_PIVOT(l2,(l315<<1)+1) = NULL;}while(0);}}else{l313 = (l46[(l2)][(
l122)].l40);SOC_IF_ERROR_RETURN(l312(l2,l313,l314));VRF_PIVOT_SHIFT_INC(l2,
MAX_VRF_ID,l10);}(l46[(l2)][(l122)].l39)-= 1;(l46[(l2)][(l122)].l40)-= 1;
return(SOC_E_NONE);}static int l323(int l2,int l122,int l10,void*l15,int*l324
){int l325;int l326;int l327;int l328;int l313;uint32 l318,l319;int l130;if((
l46[(l2)][(l122)].l42) == 0){l328 = ((3*(64+32+2+1))-1);if(soc_alpm_mode_get(
l2) == SOC_ALPM_MODE_PARALLEL){if(l122<= (((3*(64+32+2+1))/3)-1)){l328 = (((3
*(64+32+2+1))/3)-1);}}while((l46[(l2)][(l328)].next)>l122){l328 = (l46[(l2)][
(l328)].next);}l326 = (l46[(l2)][(l328)].next);if(l326!= -1){(l46[(l2)][(l326
)].l41) = l122;}(l46[(l2)][(l122)].next) = (l46[(l2)][(l328)].next);(l46[(l2)
][(l122)].l41) = l328;(l46[(l2)][(l328)].next) = l122;(l46[(l2)][(l122)].l43)
= ((l46[(l2)][(l328)].l43)+1)/2;(l46[(l2)][(l328)].l43)-= (l46[(l2)][(l122)].
l43);(l46[(l2)][(l122)].l39) = (l46[(l2)][(l328)].l40)+(l46[(l2)][(l328)].l43
)+1;(l46[(l2)][(l122)].l40) = (l46[(l2)][(l122)].l39)-1;(l46[(l2)][(l122)].
l42) = 0;}else if(!l10){l313 = (l46[(l2)][(l122)].l39);if((l130 = 
READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l313,l15))<0){return l130;}l318 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l76));l319 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l98[(l2)]->l77));if((l318 == 0)||(l319 == 0)){*l324 = (
l313<<1)+((l319 == 0)?1:0);return(SOC_E_NONE);}l313 = (l46[(l2)][(l122)].l40)
;if((l130 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l313,l15))<0){return l130;}l318 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l98[(
l2)]->l76));l319 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l98[(l2)]->l77));if((l318 == 0)||(l319 == 0)){*l324 = (
l313<<1)+((l319 == 0)?1:0);return(SOC_E_NONE);}}l327 = l122;while((l46[(l2)][
(l327)].l43) == 0){l327 = (l46[(l2)][(l327)].next);if(l327 == -1){l327 = l122
;break;}}while((l46[(l2)][(l327)].l43) == 0){l327 = (l46[(l2)][(l327)].l41);
if(l327 == -1){if((l46[(l2)][(l122)].l42) == 0){l325 = (l46[(l2)][(l122)].l41
);l326 = (l46[(l2)][(l122)].next);if(-1!= l325){(l46[(l2)][(l325)].next) = 
l326;}if(-1!= l326){(l46[(l2)][(l326)].l41) = l325;}}return(SOC_E_FULL);}}
while(l327>l122){l326 = (l46[(l2)][(l327)].next);SOC_IF_ERROR_RETURN(l321(l2,
l326,l10));(l46[(l2)][(l327)].l43)-= 1;(l46[(l2)][(l326)].l43)+= 1;l327 = 
l326;}while(l327<l122){SOC_IF_ERROR_RETURN(l317(l2,l327,l10));(l46[(l2)][(
l327)].l43)-= 1;l325 = (l46[(l2)][(l327)].l41);(l46[(l2)][(l325)].l43)+= 1;
l327 = l325;}(l46[(l2)][(l122)].l42)+= 1;(l46[(l2)][(l122)].l43)-= 1;(l46[(l2
)][(l122)].l40)+= 1;*l324 = (l46[(l2)][(l122)].l40)<<((l10)?0:1);sal_memcpy(
l15,soc_mem_entry_null(l2,L3_DEFIPm),soc_mem_entry_words(l2,L3_DEFIPm)*4);
return(SOC_E_NONE);}static int l329(int l2,int l122,int l10,void*l15,int l330
){int l325;int l326;int l313;int l314;uint32 l331[SOC_MAX_MEM_FIELD_WORDS];
uint32 l332[SOC_MAX_MEM_FIELD_WORDS];uint32 l333[SOC_MAX_MEM_FIELD_WORDS];
void*l334;int l130;int l335,l158;l313 = (l46[(l2)][(l122)].l40);l314 = l330;
if(!l10){l314>>= 1;if((l130 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l313,l331))<0){
return l130;}if((l130 = READ_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,
soc_alpm_physical_idx(l2,L3_DEFIPm,l313,1),l332))<0){return l130;}if((l130 = 
READ_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,soc_alpm_physical_idx(l2,L3_DEFIPm,
l314,1),l333))<0){return l130;}l334 = (l314 == l313)?l331:l15;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l331),(l98[(
l2)]->l77))){l158 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,l332,
BPM_LENGTH1f);if(l330&1){l130 = soc_alpm_lpm_ip4entry1_to_1(l2,l331,l334,
PRESERVE_HIT);soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,l333,BPM_LENGTH1f,
l158);}else{l130 = soc_alpm_lpm_ip4entry1_to_0(l2,l331,l334,PRESERVE_HIT);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,l333,BPM_LENGTH0f,l158);}l335 = (
l313<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l331),(l98[(l2)]->l77),(0));}else{l158 = soc_mem_field32_get(l2,
L3_DEFIP_AUX_TABLEm,l332,BPM_LENGTH0f);if(l330&1){l130 = 
soc_alpm_lpm_ip4entry0_to_1(l2,l331,l334,PRESERVE_HIT);soc_mem_field32_set(l2
,L3_DEFIP_AUX_TABLEm,l333,BPM_LENGTH1f,l158);}else{l130 = 
soc_alpm_lpm_ip4entry0_to_0(l2,l331,l334,PRESERVE_HIT);soc_mem_field32_set(l2
,L3_DEFIP_AUX_TABLEm,l333,BPM_LENGTH0f,l158);}l335 = l313<<1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l331),(l98[(
l2)]->l76),(0));(l46[(l2)][(l122)].l42)-= 1;(l46[(l2)][(l122)].l43)+= 1;(l46[
(l2)][(l122)].l40)-= 1;}l335 = soc_alpm_physical_idx(l2,L3_DEFIPm,l335,0);
l330 = soc_alpm_physical_idx(l2,L3_DEFIPm,l330,0);ALPM_TCAM_PIVOT(l2,l330) = 
ALPM_TCAM_PIVOT(l2,l335);if(ALPM_TCAM_PIVOT(l2,l330)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l2,l330)) = l330;}ALPM_TCAM_PIVOT(l2,l335) = NULL;if((l130 = 
WRITE_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ALL,soc_alpm_physical_idx(l2,L3_DEFIPm
,l314,1),l333))<0){return l130;}if(l314!= l313){l293(l2,l334,l314,0x4000,0);
if((l130 = l168(l2,MEM_BLOCK_ANY,l314,l314,l334))<0){return l130;}if((l130 = 
l307(l2,l314,l334))<0){return l130;}}l293(l2,l331,l313,0x4000,0);if((l130 = 
l168(l2,MEM_BLOCK_ANY,l313,l313,l331))<0){return l130;}if((l130 = l307(l2,
l313,l331))<0){return l130;}}else{(l46[(l2)][(l122)].l42)-= 1;(l46[(l2)][(
l122)].l43)+= 1;(l46[(l2)][(l122)].l40)-= 1;if(l314!= l313){if((l130 = 
READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l313,l331))<0){return l130;}l293(l2,l331,l314
,0x4000,0);if((l130 = l168(l2,MEM_BLOCK_ANY,l314,l313,l331))<0){return l130;}
if((l130 = l307(l2,l314,l331))<0){return l130;}}l330 = soc_alpm_physical_idx(
l2,L3_DEFIPm,l314,1);l335 = soc_alpm_physical_idx(l2,L3_DEFIPm,l313,1);
ALPM_TCAM_PIVOT(l2,l330<<1) = ALPM_TCAM_PIVOT(l2,l335<<1);ALPM_TCAM_PIVOT(l2,
l335<<1) = NULL;if(ALPM_TCAM_PIVOT(l2,l330<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l2,l330<<1)) = l330<<1;}sal_memcpy(l331,soc_mem_entry_null(l2
,L3_DEFIPm),soc_mem_entry_words(l2,L3_DEFIPm)*4);l293(l2,l331,l313,0x4000,0);
if((l130 = l168(l2,MEM_BLOCK_ANY,l313,l313,l331))<0){return l130;}if((l130 = 
l307(l2,l313,l331))<0){return l130;}}if((l46[(l2)][(l122)].l42) == 0){l325 = 
(l46[(l2)][(l122)].l41);assert(l325!= -1);l326 = (l46[(l2)][(l122)].next);(
l46[(l2)][(l325)].next) = l326;(l46[(l2)][(l325)].l43)+= (l46[(l2)][(l122)].
l43);(l46[(l2)][(l122)].l43) = 0;if(l326!= -1){(l46[(l2)][(l326)].l41) = l325
;}(l46[(l2)][(l122)].next) = -1;(l46[(l2)][(l122)].l41) = -1;(l46[(l2)][(l122
)].l39) = -1;(l46[(l2)][(l122)].l40) = -1;}return(l130);}int
soc_alpm_lpm_vrf_get(int l21,void*lpm_entry,int*l28,int*l336){int l151;if(((
l98[(l21)]->l80)!= NULL)){l151 = soc_L3_DEFIPm_field32_get(l21,lpm_entry,
VRF_ID_0f);*l336 = l151;if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,
VRF_ID_MASK0f)){*l28 = l151;}else if(!soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry),(l98[(l21)]->l82))){*l28 = 
SOC_L3_VRF_GLOBAL;*l336 = SOC_VRF_MAX(l21)+1;}else{*l28 = SOC_L3_VRF_OVERRIDE
;}}else{*l28 = SOC_L3_VRF_DEFAULT;}return(SOC_E_NONE);}static int l37(int l2,
void*entry,int*l17){int l122;int l130;int l10;uint32 l131;int l151;int l337;
l10 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry)
,(l98[(l2)]->l66));if(l10){l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l98[(l2)]->l64));if((l130 = _ipmask2pfx(
l131,&l122))<0){return(l130);}l131 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l98[(l2)]->l65));if(l122){if(l131!= 
0xffffffff){return(SOC_E_PARAM);}l122+= 32;}else{if((l130 = _ipmask2pfx(l131,
&l122))<0){return(l130);}}l122+= 33;}else{l131 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l98[
(l2)]->l64));if((l130 = _ipmask2pfx(l131,&l122))<0){return(l130);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,entry,&l151,&l130));l337 = 
soc_alpm_mode_get(l2);switch(l151){case SOC_L3_VRF_GLOBAL:if(l337 == 
SOC_ALPM_MODE_PARALLEL){*l17 = l122+((3*(64+32+2+1))/3);}else{*l17 = l122;}
break;case SOC_L3_VRF_OVERRIDE:*l17 = l122+2*((3*(64+32+2+1))/3);break;
default:if(l337 == SOC_ALPM_MODE_PARALLEL){*l17 = l122;}else{*l17 = l122+((3*
(64+32+2+1))/3);}break;}return(SOC_E_NONE);}static int l14(int l2,void*l8,
void*l15,int*l16,int*l17,int*l10){int l130;int l34;int l123;int l122 = 0;l34 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l98[(l2
)]->l66));if(l34){if(!(l34 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l8),(l98[(l2)]->l67)))){return(SOC_E_PARAM);}}*l10 = l34;l37
(l2,l8,&l122);*l17 = l122;if(l297(l2,l8,l122,&l123) == SOC_E_NONE){*l16 = 
l123;if((l130 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,(*l10)?*l16:(*l16>>1),l15))<0
){return l130;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}static int
l3(int l2){int l306;int l173;int l286;int l338;uint32 l339 = 0;uint32 l337;if
(!soc_feature(l2,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}if((l337 = 
soc_property_get(l2,spn_L3_ALPM_ENABLE,0))){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l2,&l339));soc_reg_field_set(l2,
L3_DEFIP_RPF_CONTROLr,&l339,LPM_MODEf,1);if(l337 == SOC_ALPM_MODE_PARALLEL){
soc_reg_field_set(l2,L3_DEFIP_RPF_CONTROLr,&l339,LOOKUP_MODEf,1);}else{
soc_reg_field_set(l2,L3_DEFIP_RPF_CONTROLr,&l339,LOOKUP_MODEf,0);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l2,l339));l339 = 0;if(
SOC_URPF_STATUS_GET(l2)){soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l339,
URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l339,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l339,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l339,
URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l2,l339))
;l339 = 0;if(l337 == SOC_ALPM_MODE_PARALLEL){if(SOC_URPF_STATUS_GET(l2)){
soc_reg_field_set(l2,L3_DEFIP_ALPM_CFGr,&l339,TCAM2_SELf,1);soc_reg_field_set
(l2,L3_DEFIP_ALPM_CFGr,&l339,TCAM3_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM4_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM5_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM6_SELf,3);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM7_SELf,3);}else{soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM4_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM5_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM6_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l2)){
soc_reg_field_set(l2,L3_DEFIP_ALPM_CFGr,&l339,TCAM4_SELf,2);soc_reg_field_set
(l2,L3_DEFIP_ALPM_CFGr,&l339,TCAM5_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM6_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l339,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l2,l339));if(soc_property_get(l2,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l340 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l2,&l340));soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&
l340,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l340,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l340,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l340,
V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l2,l340))
;}}l306 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l2);l338 = sizeof(l44)*(l306);if(
(l46[(l2)]!= NULL)){if(soc_alpm_deinit(l2)<0){SOC_ALPM_LPM_UNLOCK(l2);return
SOC_E_INTERNAL;}}l98[l2] = sal_alloc(sizeof(l96),"lpm_field_state");if(NULL == 
l98[l2]){SOC_ALPM_LPM_UNLOCK(l2);return(SOC_E_MEMORY);}(l98[l2])->l48 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,CLASS_ID0f);(l98[l2])->l49 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,CLASS_ID1f);(l98[l2])->l50 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DST_DISCARD0f);(l98[l2])->l51 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DST_DISCARD1f);(l98[l2])->l52 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP0f);(l98[l2])->l53 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP1f);(l98[l2])->l54 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_COUNT0f);(l98[l2])->l55 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_COUNT1f);(l98[l2])->l56 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_PTR0f);(l98[l2])->l57 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_PTR1f);(l98[l2])->l58 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_ROUTE0f);(l98[l2])->l59 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_ROUTE1f);(l98[l2])->l60 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,HIT0f);(l98[l2])->l61 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,HIT1f);(l98[l2])->l62 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR0f);(l98[l2])->l63 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR1f);(l98[l2])->l64 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR_MASK0f);(l98[l2])->l65 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR_MASK1f);(l98[l2])->l66 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE0f);(l98[l2])->l67 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE1f);(l98[l2])->l68 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE_MASK0f);(l98[l2])->l69 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE_MASK1f);(l98[l2])->l70 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,NEXT_HOP_INDEX0f);(l98[l2])->l71 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,NEXT_HOP_INDEX1f);(l98[l2])->l72 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,PRI0f);(l98[l2])->l73 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,PRI1f);(l98[l2])->l74 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,RPE0f);(l98[l2])->l75 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,RPE1f);(l98[l2])->l76 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VALID0f);(l98[l2])->l77 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VALID1f);(l98[l2])->l78 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_0f);(l98[l2])->l79 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_1f);(l98[l2])->l80 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_MASK0f);(l98[l2])->l81 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_MASK1f);(l98[l2])->l82 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_HIGH0f);(l98[l2])->l83 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_HIGH1f);(l98[l2])->l84 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_HIT_IDX0f);(l98[l2])->l85 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_HIT_IDX1f);(l98[l2])->l86 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_BKT_PTR0f);(l98[l2])->l87 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_BKT_PTR1f);(l98[l2])->l88 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DEFAULT_MISS0f);(l98[l2])->l89 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DEFAULT_MISS1f);(l98[l2])->l90 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l98[l2])->
l91 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l98[l2
])->l92 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l98[l2]
)->l93 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(l98[l2])
->l94 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,ENTRY_TYPE_MASK0f);(l98[l2])->l95 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ENTRY_TYPE_MASK1f);(l46[(l2)]) = sal_alloc
(l338,"LPM prefix info");if(NULL == (l46[(l2)])){sal_free(l98[l2]);l98[l2] = 
NULL;SOC_ALPM_LPM_UNLOCK(l2);return(SOC_E_MEMORY);}sal_memset((l46[(l2)]),0,
l338);for(l173 = 0;l173<l306;l173++){(l46[(l2)][(l173)].l39) = -1;(l46[(l2)][
(l173)].l40) = -1;(l46[(l2)][(l173)].l41) = -1;(l46[(l2)][(l173)].next) = -1;
(l46[(l2)][(l173)].l42) = 0;(l46[(l2)][(l173)].l43) = 0;}l286 = 
soc_mem_index_count(l2,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l2)){l286>>= 1;}if(
l337 == SOC_ALPM_MODE_PARALLEL){(l46[(l2)][(((3*(64+32+2+1))-1))].l40) = (
l286>>1)-1;(l46[(l2)][(((((3*(64+32+2+1))/3)-1)))].l43) = l286>>1;(l46[(l2)][
((((3*(64+32+2+1))-1)))].l43) = (l286-(l46[(l2)][(((((3*(64+32+2+1))/3)-1)))]
.l43));}else{(l46[(l2)][((((3*(64+32+2+1))-1)))].l43) = l286;}if((l109[(l2)])
!= NULL){if(l117((l109[(l2)]))<0){SOC_ALPM_LPM_UNLOCK(l2);return
SOC_E_INTERNAL;}(l109[(l2)]) = NULL;}if(l115(l2,l286*2,l286,&(l109[(l2)]))<0)
{SOC_ALPM_LPM_UNLOCK(l2);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l2);return(
SOC_E_NONE);}static int l4(int l2){if(!soc_feature(l2,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l2);if((l109[(l2)])!= NULL){l117((
l109[(l2)]));(l109[(l2)]) = NULL;}if((l46[(l2)]!= NULL)){sal_free(l98[l2]);
l98[l2] = NULL;sal_free((l46[(l2)]));(l46[(l2)]) = NULL;}SOC_ALPM_LPM_UNLOCK(
l2);return(SOC_E_NONE);}static int l5(int l2,void*l6,int*l341){int l122;int
index;int l10;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l130 = SOC_E_NONE;int
l342 = 0;sal_memcpy(l15,soc_mem_entry_null(l2,L3_DEFIPm),soc_mem_entry_words(
l2,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l2);l130 = l14(l2,l6,l15,&index,&l122,&l10
);if(l130 == SOC_E_NOT_FOUND){l130 = l323(l2,l122,l10,l15,&index);if(l130<0){
SOC_ALPM_LPM_UNLOCK(l2);return(l130);}}else{l342 = 1;}*l341 = index;if(l130 == 
SOC_E_NONE){if(!l10){if(index&1){l130 = soc_alpm_lpm_ip4entry0_to_1(l2,l6,l15
,PRESERVE_HIT);}else{l130 = soc_alpm_lpm_ip4entry0_to_0(l2,l6,l15,
PRESERVE_HIT);}if(l130<0){SOC_ALPM_LPM_UNLOCK(l2);return(l130);}l6 = (void*)
l15;index>>= 1;}l1(l2);soc_cm_debug(DK_L3|DK_SOCMEM,
"\nsoc_alpm_lpm_insert: %d %d\n",index,l122);if(!l342){l293(l2,l6,index,
0x4000,0);}l130 = l168(l2,MEM_BLOCK_ANY,index,index,l6);if(l130>= 0){l130 = 
l307(l2,index,l6);}}SOC_ALPM_LPM_UNLOCK(l2);return(l130);}static int l7(int l2
,void*l8){int l122;int index;int l10;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int
l130 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l2);l130 = l14(l2,l8,l15,&index,&l122,&
l10);if(l130 == SOC_E_NONE){soc_cm_debug(DK_L3|DK_SOCMEM,
"\nsoc_alpm_lpm_delete: %d %d\n",index,l122);l296(l2,l8,index);l130 = l329(l2
,l122,l10,l15,index);}l1(l2);SOC_ALPM_LPM_UNLOCK(l2);return(l130);}static int
l18(int l2,void*l8,void*l15,int*l16){int l122;int l130;int l10;
SOC_ALPM_LPM_LOCK(l2);l130 = l14(l2,l8,l15,l16,&l122,&l10);
SOC_ALPM_LPM_UNLOCK(l2);return(l130);}static int l9(int l21,void*l8,int l10,
int l11,int l12,defip_aux_scratch_entry_t*l13){uint32 l131;uint32 l343[4] = {
0,0,0,0};int l122 = 0;int l130 = SOC_E_NONE;l131 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,VALID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
VALIDf,l131);l131 = soc_mem_field32_get(l21,L3_DEFIPm,l8,MODE0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,MODEf,l131);l131 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ENTRY_TYPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ENTRY_TYPEf,0);l131 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,GLOBAL_ROUTE0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,GLOBAL_ROUTEf,l131);l131 = soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,ECMPf,l131);l131 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP_PTR0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ECMP_PTRf,l131);l131 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,NEXT_HOP_INDEX0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,NEXT_HOP_INDEXf,l131);l131 = soc_mem_field32_get(l21,L3_DEFIPm,l8,PRI0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,PRIf,l131);l131 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,RPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,RPEf,l131);l131 =soc_mem_field32_get(l21,L3_DEFIPm,
l8,VRF_ID_0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,VRFf,l131);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,DB_TYPEf,l11);l131 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,DST_DISCARD0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,DST_DISCARDf,l131);l131 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,CLASS_ID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
CLASS_IDf,l131);if(l10){l343[2] = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR0f);l343[3] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR1f);}else{
l343[0] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR0f);}soc_mem_field_set(
l21,L3_DEFIP_AUX_SCRATCHm,(uint32*)l13,IP_ADDRf,(uint32*)l343);if(l10){l131 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l130 = _ipmask2pfx(
l131,&l122))<0){return(l130);}l131 = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR_MASK1f);if(l122){if(l131!= 0xffffffff){return(SOC_E_PARAM);}l122+= 32
;}else{if((l130 = _ipmask2pfx(l131,&l122))<0){return(l130);}}}else{l131 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l130 = _ipmask2pfx(
l131,&l122))<0){return(l130);}}soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,IP_LENGTHf,l122);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
REPLACE_LENf,l12);return(SOC_E_NONE);}int _soc_alpm_aux_op(int l2,
_soc_aux_op_t l344,defip_aux_scratch_entry_t*l13,int l345,int*l144,int*l145,
int*bucket_index){uint32 l339 = 0;uint32 l346 = 0;int l347;soc_timeout_t l348
;int l130 = SOC_E_NONE;int l349 = 0;if(l345){SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_SCRATCHm(l2,MEM_BLOCK_ANY,0,l13));}l350:l339 = 0;switch(
l344){case INSERT_PROPAGATE:l347 = 0;break;case DELETE_PROPAGATE:l347 = 1;
break;case PREFIX_LOOKUP:l347 = 2;break;case HITBIT_REPLACE:l347 = 3;break;
default:return SOC_E_PARAM;}soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l339,
OPCODEf,l347);soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l339,STARTf,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l2,l339));soc_timeout_init(&l348
,50000,5);l347 = 0;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l2,&l339));
l347 = soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l339,DONEf);if(l347 == 1){l130
= SOC_E_NONE;break;}if(soc_timeout_check(&l348)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l2,&l339));l347 = soc_reg_field_get(l2,
L3_DEFIP_AUX_CTRLr,l339,DONEf);if(l347 == 1){l130 = SOC_E_NONE;}else{
soc_cm_debug(DK_ERR,"unit %d : DEFIP AUX Operation timeout\n",l2);l130 = 
SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l130)){if(soc_reg_field_get(l2
,L3_DEFIP_AUX_CTRLr,l339,ERRORf)){soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&
l339,STARTf,0);soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l339,ERRORf,0);
soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l339,DONEf,0);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l2,l339));soc_cm_debug(DK_WARN,
"unit %d: DEFIP AUX Operation encountered ""parity error !!\n",l2);l349++;if(
SOC_CONTROL(l2)->alpm_bulk_retry){sal_sem_take(SOC_CONTROL(l2)->
alpm_bulk_retry,1000000);}if(l349<5){soc_cm_debug(DK_WARN,
"unit %d: Retry DEFIP AUX Operation..\n",l2);goto l350;}else{soc_cm_debug(
DK_ERR,"unit %d: Aborting DEFIP AUX Operation "
"due to un-correctable error !!\n",l2);return SOC_E_INTERNAL;}}if(l344 == 
PREFIX_LOOKUP){if(l144&&l145){*l144 = soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr
,l339,HITf);*l145 = soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l339,BKT_INDEXf);
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRL_1r(l2,&l346));*bucket_index = 
soc_reg_field_get(l2,L3_DEFIP_AUX_CTRL_1r,l346,BKT_PTRf);}}}return l130;}
static int l20(int l21,void*lpm_entry,void*l22,void*l23,soc_mem_t l24,uint32
l25,uint32*l351){uint32 l131;uint32 l343[4] = {0,0};int l122 = 0;int l130 = 
SOC_E_NONE;int l10;uint32 l26 = 0;l10 = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,MODE0f);sal_memset(l22,0,soc_mem_entry_words(l21,l24)*4);l131 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,HIT0f);soc_mem_field32_set(l21,
l24,l22,HITf,l131);l131 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,VALID0f
);soc_mem_field32_set(l21,l24,l22,VALIDf,l131);l131 = soc_mem_field32_get(l21
,L3_DEFIPm,lpm_entry,ECMP0f);soc_mem_field32_set(l21,l24,l22,ECMPf,l131);l131
= soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set
(l21,l24,l22,ECMP_PTRf,l131);l131 = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,NEXT_HOP_INDEX0f);soc_mem_field32_set(l21,l24,l22,NEXT_HOP_INDEXf,
l131);l131 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,PRI0f);
soc_mem_field32_set(l21,l24,l22,PRIf,l131);l131 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,RPE0f);soc_mem_field32_set(l21,l24,l22,RPEf,l131);l131 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(l21,l24,l22,DST_DISCARDf,l131);l131 = soc_mem_field32_get
(l21,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);soc_mem_field32_set(l21,l24,l22,
SRC_DISCARDf,l131);l131 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
CLASS_ID0f);soc_mem_field32_set(l21,l24,l22,CLASS_IDf,l131);l343[0] = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR0f);if(l10){l343[1] = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR1f);}soc_mem_field_set(l21
,l24,(uint32*)l22,KEYf,(uint32*)l343);if(l10){l131 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l130 = _ipmask2pfx(l131,&l122))<0){
return(l130);}l131 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f);if(l122){if(l131!= 0xffffffff){return(SOC_E_PARAM);}l122+= 32
;}else{if((l130 = _ipmask2pfx(l131,&l122))<0){return(l130);}}}else{l131 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l130 = 
_ipmask2pfx(l131,&l122))<0){return(l130);}}if((l122 == 0)&&(l343[0] == 0)&&(
l343[1] == 0)){l26 = 1;}if(l351!= NULL){*l351 = l26;}soc_mem_field32_set(l21,
l24,l22,LENGTHf,l122);if(l23 == NULL){return(SOC_E_NONE);}if(
SOC_URPF_STATUS_GET(l21)){sal_memset(l23,0,soc_mem_entry_words(l21,l24)*4);
sal_memcpy(l23,l22,soc_mem_entry_words(l21,l24)*4);soc_mem_field32_set(l21,
l24,l23,DST_DISCARDf,0);soc_mem_field32_set(l21,l24,l23,RPEf,0);
soc_mem_field32_set(l21,l24,l23,SRC_DISCARDf,l25&SOC_ALPM_RPF_SRC_DISCARD);
soc_mem_field32_set(l21,l24,l23,DEFAULTROUTEf,l26);}return(SOC_E_NONE);}
static int l27(int l21,void*l22,soc_mem_t l24,int l10,int l28,int l29,int
index,void*lpm_entry){uint32 l131;uint32 l343[4] = {0,0};uint32 l122 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_words(l21,L3_DEFIPm)*4);l131 = 
soc_mem_field32_get(l21,l24,l22,HITf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,HIT0f,l131);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
HIT1f,l131);}l131 = soc_mem_field32_get(l21,l24,l22,VALIDf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VALID0f,l131);if(l10){
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VALID1f,l131);}l131 = 
soc_mem_field32_get(l21,l24,l22,ECMPf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,ECMP0f,l131);l131 = soc_mem_field32_get(l21,l24,l22,ECMP_PTRf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ECMP_PTR0f,l131);l131 = 
soc_mem_field32_get(l21,l24,l22,NEXT_HOP_INDEXf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f,l131);l131 = soc_mem_field32_get(l21,l24
,l22,PRIf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,PRI0f,l131);l131 = 
soc_mem_field32_get(l21,l24,l22,RPEf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,RPE0f,l131);l131 = soc_mem_field32_get(l21,l24,l22,DST_DISCARDf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,DST_DISCARD0f,l131);l131 = 
soc_mem_field32_get(l21,l24,l22,SRC_DISCARDf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,SRC_DISCARD0f,l131);l131 = soc_mem_field32_get(l21,l24,
l22,CLASS_IDf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,CLASS_ID0f,l131);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ALG_BKT_PTR0f,l29);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ALG_HIT_IDX0f,index);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE_MASK0f,3);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l10){
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(l21,
l24,l22,KEYf,l343);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR1f,l343[1]);}soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR0f,
l343[0]);l131 = soc_mem_field32_get(l21,l24,l22,LENGTHf);if(l10){if(l131>= 32
){l122 = 0xffffffff;soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l122);l122 = ~(((l131-32) == 32)?0:(0xffffffff)>>(l131-32));
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l122);}else{l122 = 
~(0xffffffff>>l131);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l122);}}else{assert(l131<= 32);l122 = ~(((l131) == 32)?0:(
0xffffffff)>>(l131));soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,l122);}if(l28 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,GLOBAL_HIGH0f,1);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else if(l28 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry
,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,0);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,l28);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(l21));}
return(SOC_E_NONE);}int soc_alpm_warmboot_pivot_add(int l21,int l10,void*
lpm_entry,int l352,int l353){int l130 = SOC_E_NONE;uint32 key[2] = {0,0};
alpm_pivot_t*l206 = NULL;alpm_bucket_handle_t*l215 = NULL;int l151 = 0,l28 = 
0;uint32 l354;trie_t*l252 = NULL;uint32 prefix[5] = {0};int l26 = 0;l130 = 
l136(l21,lpm_entry,prefix,&l354,&l26);SOC_IF_ERROR_RETURN(l130);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l21,lpm_entry,&l151,&l28));l352 = 
soc_alpm_physical_idx(l21,L3_DEFIPm,l352,l10);l215 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l215 == NULL){soc_cm_debug(
DK_ERR,"Unable to allocate memory for ""PIVOT trie node \n");return SOC_E_NONE
;}sal_memset(l215,0,sizeof(*l215));l206 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l206 == NULL){soc_cm_debug(DK_ERR,
"Unable to allocate memory for ""PIVOT trie node \n");sal_free(l215);return
SOC_E_MEMORY;}sal_memset(l206,0,sizeof(*l206));PIVOT_BUCKET_HANDLE(l206) = 
l215;if(l10){trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l206));key[0] = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR0f);key[1] = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR1f);}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l206));key[0] = soc_L3_DEFIPm_field32_get
(l21,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l206) = l353;PIVOT_TCAM_INDEX(
l206) = l352;if(l151!= SOC_L3_VRF_OVERRIDE){if(l10 == 0){l252 = 
VRF_PIVOT_TRIE_IPV4(l21,l28);if(l252 == NULL){trie_init(_MAX_KEY_LEN_48_,&
VRF_PIVOT_TRIE_IPV4(l21,l28));l252 = VRF_PIVOT_TRIE_IPV4(l21,l28);}}else{l252
= VRF_PIVOT_TRIE_IPV6(l21,l28);if(l252 == NULL){trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l21,l28));l252 = VRF_PIVOT_TRIE_IPV6(l21,l28);}}
sal_memcpy(l206->key,prefix,sizeof(prefix));l206->len = l354;l130 = 
trie_insert(l252,l206->key,NULL,l206->len,(trie_node_t*)l206);if(SOC_FAILURE(
l130)){sal_free(l215);sal_free(l206);return l130;}}ALPM_TCAM_PIVOT(l21,l352) = 
l206;PIVOT_BUCKET_VRF(l206) = l28;PIVOT_BUCKET_IPV6(l206) = l10;
PIVOT_BUCKET_ENT_CNT_UPDATE(l206);if(key[0] == 0&&key[1] == 0){
PIVOT_BUCKET_DEF(l206) = TRUE;}VRF_PIVOT_REF_INC(l21,l28,l10);return l130;}
static int l355(int l21,int l10,void*lpm_entry,void*l22,soc_mem_t l24,int l352
,int l353,int l356){int l357;int l28;int l130 = SOC_E_NONE;int l26 = 0;uint32
prefix[5] = {0,0,0,0,0};uint32 l143;void*l358 = NULL;trie_t*l359 = NULL;
trie_t*l209 = NULL;trie_node_t*l211 = NULL;payload_t*l360 = NULL;payload_t*
l213 = NULL;alpm_pivot_t*l148 = NULL;if((NULL == lpm_entry)||(NULL == l22)){
return SOC_E_PARAM;}if(l10){if(!(l10 = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l21,lpm_entry,&l357,&l28));l24 = (l10)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l358 = sal_alloc(sizeof(
defip_entry_t),"Temp lpm_entr");if(NULL == l358){return SOC_E_MEMORY;}
SOC_IF_ERROR_RETURN(l27(l21,l22,l24,l10,l357,l353,l352,l358));l130 = l136(l21
,l358,prefix,&l143,&l26);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"prefix create failed\n");return l130;}sal_free(l358);l148 = ALPM_TCAM_PIVOT(
l21,l352);l359 = PIVOT_BUCKET_TRIE(l148);l360 = sal_alloc(sizeof(payload_t),
"Payload for Key");if(NULL == l360){soc_cm_debug(DK_ERR,
"Unable to allocate memory for ""trie node.\n");return SOC_E_MEMORY;}l213 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l213){
soc_cm_debug(DK_ERR,"Unable to allocate memory for ""pfx trie node\n");
sal_free(l360);return SOC_E_MEMORY;}sal_memset(l360,0,sizeof(*l360));
sal_memset(l213,0,sizeof(*l213));l360->key[0] = prefix[0];l360->key[1] = 
prefix[1];l360->key[2] = prefix[2];l360->key[3] = prefix[3];l360->key[4] = 
prefix[4];l360->len = l143;l360->index = l356;sal_memcpy(l213,l360,sizeof(*
l360));l130 = trie_insert(l359,prefix,NULL,l143,(trie_node_t*)l360);if(
SOC_FAILURE(l130)){goto l361;}if(l10){l209 = VRF_PREFIX_TRIE_IPV6(l21,l28);}
else{l209 = VRF_PREFIX_TRIE_IPV4(l21,l28);}if(!l26){l130 = trie_insert(l209,
prefix,NULL,l143,(trie_node_t*)l213);if(SOC_FAILURE(l130)){goto l228;}}return
l130;l228:(void)trie_delete(l359,prefix,l143,&l211);l360 = (payload_t*)l211;
l361:sal_free(l360);sal_free(l213);return l130;}static int l362(int l21,int
l34,int l28,int l132,int bkt_ptr){int l130 = SOC_E_NONE;uint32 l143;uint32 key
[2] = {0,0};trie_t*l363 = NULL;payload_t*l242 = NULL;defip_entry_t*lpm_entry = 
NULL;lpm_entry = sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(
lpm_entry == NULL){soc_cm_debug(DK_ERR,"unable to allocate memory for "
"LPM entry\n");return SOC_E_MEMORY;}l30(l21,key,0,l28,l34,lpm_entry,0,1);if(
l28 == SOC_VRF_MAX(l21)+1){soc_L3_DEFIPm_field32_set(l21,lpm_entry,
GLOBAL_ROUTE0f,1);}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,
DEFAULT_MISS0f,1);}soc_L3_DEFIPm_field32_set(l21,lpm_entry,ALG_BKT_PTR0f,
bkt_ptr);if(l34 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l21,l28) = lpm_entry;
trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l21,l28));l363 = 
VRF_PREFIX_TRIE_IPV4(l21,l28);}else{VRF_TRIE_DEFAULT_ROUTE_IPV6(l21,l28) = 
lpm_entry;trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l21,l28));l363 = 
VRF_PREFIX_TRIE_IPV6(l21,l28);}l242 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l242 == NULL){soc_cm_debug(DK_ERR,
"Unable to allocate memory for ""pfx trie node \n");return SOC_E_MEMORY;}
sal_memset(l242,0,sizeof(*l242));l143 = 0;l242->key[0] = key[0];l242->key[1] = 
key[1];l242->len = l143;l130 = trie_insert(l363,key,NULL,l143,&(l242->node));
if(SOC_FAILURE(l130)){sal_free(l242);return l130;}VRF_TRIE_INIT_DONE(l21,l28,
l34,1);return l130;}int soc_alpm_warmboot_prefix_insert(int l21,int l10,void*
lpm_entry,void*l22,int l352,int l353,int l356){int l357;int l28;int l130 = 
SOC_E_NONE;soc_mem_t l24;l352 = soc_alpm_physical_idx(l21,L3_DEFIPm,l352,l10)
;l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l21,lpm_entry,&l357,&l28));if(l357 == 
SOC_L3_VRF_OVERRIDE){return(l130);}if(!VRF_TRIE_INIT_COMPLETED(l21,l28,l10)){
soc_cm_debug(DK_VERBOSE,"VRF %d is not ""initialized\n",l28);l130 = l362(l21,
l10,l28,l352,l353);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"VRF %d/%d trie init \n""failed\n",l28,l10);return l130;}soc_cm_debug(
DK_VERBOSE,"VRF %d/%d trie init ""completed\n",l28,l10);}l130 = l355(l21,l10,
lpm_entry,l22,l24,l352,l353,l356);if(l130!= SOC_E_NONE){soc_cm_debug(DK_WARN,
"unit %d : ""Route Insertion Failed :%s\n",l21,soc_errmsg(l130));return(l130)
;}VRF_TRIE_ROUTES_INC(l21,l28,l10);return(l130);}int
soc_alpm_warmboot_bucket_bitmap_set(int l2,int l34,int l255){int l287 = 1;if(
l34){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l287 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l2) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l2),l255,l287);return SOC_E_NONE;}int
soc_alpm_warmboot_lpm_reinit_done(int l21){int l132;int l364 = ((3*(64+32+2+1
))-1);int l286 = soc_mem_index_count(l21,L3_DEFIPm);if(SOC_URPF_STATUS_GET(
l21)){l286>>= 1;}if(!soc_alpm_mode_get(l21)){(l46[(l21)][(((3*(64+32+2+1))-1)
)].l41) = -1;for(l132 = ((3*(64+32+2+1))-1);l132>-1;l132--){if(-1 == (l46[(
l21)][(l132)].l39)){continue;}(l46[(l21)][(l132)].l41) = l364;(l46[(l21)][(
l364)].next) = l132;(l46[(l21)][(l364)].l43) = (l46[(l21)][(l132)].l39)-(l46[
(l21)][(l364)].l40)-1;l364 = l132;}(l46[(l21)][(l364)].next) = -1;(l46[(l21)]
[(l364)].l43) = l286-(l46[(l21)][(l364)].l40)-1;}else{(l46[(l21)][(((3*(64+32
+2+1))-1))].l41) = -1;for(l132 = ((3*(64+32+2+1))-1);l132>(((3*(64+32+2+1))-1
)/3);l132--){if(-1 == (l46[(l21)][(l132)].l39)){continue;}(l46[(l21)][(l132)]
.l41) = l364;(l46[(l21)][(l364)].next) = l132;(l46[(l21)][(l364)].l43) = (l46
[(l21)][(l132)].l39)-(l46[(l21)][(l364)].l40)-1;l364 = l132;}(l46[(l21)][(
l364)].next) = -1;(l46[(l21)][(l364)].l43) = l286-(l46[(l21)][(l364)].l40)-1;
l364 = (((3*(64+32+2+1))-1)/3);(l46[(l21)][((((3*(64+32+2+1))-1)/3))].l41) = 
-1;for(l132 = ((((3*(64+32+2+1))-1)/3)-1);l132>-1;l132--){if(-1 == (l46[(l21)
][(l132)].l39)){continue;}(l46[(l21)][(l132)].l41) = l364;(l46[(l21)][(l364)]
.next) = l132;(l46[(l21)][(l364)].l43) = (l46[(l21)][(l132)].l39)-(l46[(l21)]
[(l364)].l40)-1;l364 = l132;}(l46[(l21)][(l364)].next) = -1;(l46[(l21)][(l364
)].l43) = (l286>>1)-(l46[(l21)][(l364)].l40)-1;}return(SOC_E_NONE);}int
soc_alpm_warmboot_lpm_reinit(int l21,int l10,int l132,void*lpm_entry){int l17
;defip_entry_t*l365;if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l293(l21,lpm_entry,l132,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l37(l21,lpm_entry,&l17));if((l46[(l21)][(l17)].l42) == 0)
{(l46[(l21)][(l17)].l39) = l132;(l46[(l21)][(l17)].l40) = l132;}else{(l46[(
l21)][(l17)].l40) = l132;}(l46[(l21)][(l17)].l42)++;if(l10){return(SOC_E_NONE
);}}else{if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l365 = 
sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");soc_alpm_lpm_ip4entry1_to_0(
l21,lpm_entry,l365,TRUE);SOC_IF_ERROR_RETURN(l37(l21,l365,&l17));if((l46[(l21
)][(l17)].l42) == 0){(l46[(l21)][(l17)].l39) = l132;(l46[(l21)][(l17)].l40) = 
l132;}else{(l46[(l21)][(l17)].l40) = l132;}sal_free(l365);(l46[(l21)][(l17)].
l42)++;}}return(SOC_E_NONE);}typedef struct l366{int v4;int v6_64;int v6_128;
int l367;int l368;int l369;int l288;}l370;typedef enum l371{l372 = 0,l373,
l374,l375,l376,l377}l378;static void l379(int l2,alpm_vrf_counter_t*l380){
l380->v4 = soc_mem_index_count(l2,L3_DEFIPm)*2;l380->v6_128 = 
soc_mem_index_count(l2,L3_DEFIP_PAIR_128m);if(soc_property_get(l2,
spn_IPV6_LPM_128B_ENABLE,1)){l380->v6_64 = l380->v6_128;}else{l380->v6_64 = 
l380->v4>>1;}if(SOC_URPF_STATUS_GET(l2)){l380->v4>>= 1;l380->v6_128>>= 1;l380
->v6_64>>= 1;}}static void l381(int l2,int l151,alpm_vrf_handle_t*l382,l378
l383){alpm_vrf_counter_t*l384;int l173,l385,l386,l387;int l342 = 0;
alpm_vrf_counter_t l380;switch(l383){case l372:soc_cm_print(
"\nAdd Counter:\n");break;case l373:soc_cm_print("\nDelete Counter:\n");break
;case l374:soc_cm_print("\nInternal Debug Counter - 1:\n");break;case l375:
l379(l2,&l380);soc_cm_print(
"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n",l380.v4,l380.v6_64,l380
.v6_128);break;case l376:soc_cm_print(
"\nInternal Debug Counter - LPM Shift:\n");break;case l377:soc_cm_print(
"\nInternal Debug Counter - LPM Full:\n");break;default:break;}soc_cm_print(
"\n      VRF  v4      v6-64   v6-128  |   Total\n");soc_cm_print(
"-----------------------------------------------\n");l385 = l386 = l387 = 0;
for(l173 = 0;l173<MAX_VRF_ID+1;l173++){int l388,l389,l390;if(l382[l173].
init_done == 0&&l173!= MAX_VRF_ID){continue;}if(l151!= -1&&l151!= l173){
continue;}l342 = 1;switch(l383){case l372:l384 = &l382[l173].add;break;case
l373:l384 = &l382[l173].del;break;case l374:l384 = &l382[l173].bkt_split;
break;case l376:l384 = &l382[l173].lpm_shift;break;case l377:l384 = &l382[
l173].lpm_full;break;case l375:l384 = &l382[l173].pivot_used;break;default:
l384 = &l382[l173].pivot_used;break;}l388 = l384->v4;l389 = l384->v6_64;l390 = 
l384->v6_128;l385+= l388;l386+= l389;l387+= l390;do{soc_cm_print(
"%9d  %-7d %-7d %-7d |   %-7d %s\n",(l173 == MAX_VRF_ID?-1:l173),(l388),(l389
),(l390),((l388+l389+l390)),(l173) == MAX_VRF_ID?"GHi":(l173) == SOC_VRF_MAX(
l2)+1?"GLo":"");}while(0);}if(l342 == 0){soc_cm_print("%9s\n",
"Specific VRF not found");}else{soc_cm_print(
"-----------------------------------------------\n");do{soc_cm_print(
"%9s  %-7d %-7d %-7d |   %-7d \n","Total",(l385),(l386),(l387),((l385+l386+
l387)));}while(0);}return;}int soc_alpm_debug_show(int l2,int l151,uint32
flags){int l173,l391,l342 = 0;l370*l392;l370 l393;l370 l394;if(l151>(
SOC_VRF_MAX(l2)+1)){return SOC_E_PARAM;}l391 = MAX_VRF_ID*sizeof(l370);l392 = 
sal_alloc(l391,"_alpm_dbg_cnt");if(l392 == NULL){return SOC_E_MEMORY;}
sal_memset(l392,0,l391);l393.v4 = ALPM_IPV4_BKT_COUNT;l393.v6_64 = 
ALPM_IPV6_64_BKT_COUNT;l393.v6_128 = ALPM_IPV6_128_BKT_COUNT;if(!
soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l393.v6_64<<= 1;l393.v6_128
<<= 1;}soc_cm_print("\nBucket Occupancy:\n");if(flags&(
SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{soc_cm_print(
"\n  BKT/VRF  Min     Max     Cur     |   Comment\n");}while(0);soc_cm_print(
"-----------------------------------------------\n");}for(l173 = 0;l173<
MAX_PIVOT_COUNT;l173++){alpm_pivot_t*l395 = ALPM_TCAM_PIVOT(l2,l173);if(l395
!= NULL){l370*l396;int l28 = PIVOT_BUCKET_VRF(l395);if(l28<0||l28>(
SOC_VRF_MAX(l2)+1)){continue;}if(l151!= -1&&l151!= l28){continue;}if(flags&
SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l342 = 1;do{soc_cm_print(
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n",l173,l28,PIVOT_BUCKET_MIN(l395),
PIVOT_BUCKET_MAX(l395),PIVOT_BUCKET_COUNT(l395),PIVOT_BUCKET_DEF(l395)?"Def":
(l28) == SOC_VRF_MAX(l2)+1?"GLo":"");}while(0);}l396 = &l392[l28];if(
PIVOT_BUCKET_IPV6(l395) == L3_DEFIP_MODE_128){l396->v6_128+= 
PIVOT_BUCKET_COUNT(l395);l396->l369+= l393.v6_128;}else if(PIVOT_BUCKET_IPV6(
l395) == L3_DEFIP_MODE_64){l396->v6_64+= PIVOT_BUCKET_COUNT(l395);l396->l368
+= l393.v6_64;}else{l396->v4+= PIVOT_BUCKET_COUNT(l395);l396->l367+= l393.v4;
}l396->l288 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l342 == 0){
soc_cm_print("%9s\n","Specific VRF not found");}}sal_memset(&l394,0,sizeof(
l394));l342 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){soc_cm_print(
"\n      VRF  v4      v6-64   v6-128  |   Total\n");soc_cm_print(
"-----------------------------------------------\n");for(l173 = 0;l173<
MAX_VRF_ID;l173++){l370*l396;if(l392[l173].l288!= TRUE){continue;}if(l151!= -
1&&l151!= l173){continue;}l342 = 1;l396 = &l392[l173];do{(&l394)->v4+= (l396)
->v4;(&l394)->l367+= (l396)->l367;(&l394)->v6_64+= (l396)->v6_64;(&l394)->
l368+= (l396)->l368;(&l394)->v6_128+= (l396)->v6_128;(&l394)->l369+= (l396)->
l369;}while(0);do{soc_cm_print(
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n",(l173),(l396->
l367)?(l396->v4)*100/(l396->l367):0,(l396->l367)?(l396->v4)*1000/(l396->l367)
%10:0,(l396->l368)?(l396->v6_64)*100/(l396->l368):0,(l396->l368)?(l396->v6_64
)*1000/(l396->l368)%10:0,(l396->l369)?(l396->v6_128)*100/(l396->l369):0,(l396
->l369)?(l396->v6_128)*1000/(l396->l369)%10:0,((l396->l367+l396->l368+l396->
l369))?((l396->v4+l396->v6_64+l396->v6_128))*100/((l396->l367+l396->l368+l396
->l369)):0,((l396->l367+l396->l368+l396->l369))?((l396->v4+l396->v6_64+l396->
v6_128))*1000/((l396->l367+l396->l368+l396->l369))%10:0,(l173) == SOC_VRF_MAX
(l2)+1?"GLo":"");}while(0);}if(l342 == 0){soc_cm_print("%9s\n",
"Specific VRF not found");}else{soc_cm_print(
"-----------------------------------------------\n");do{soc_cm_print(
"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n","Total",((&l394)
->l367)?((&l394)->v4)*100/((&l394)->l367):0,((&l394)->l367)?((&l394)->v4)*
1000/((&l394)->l367)%10:0,((&l394)->l368)?((&l394)->v6_64)*100/((&l394)->l368
):0,((&l394)->l368)?((&l394)->v6_64)*1000/((&l394)->l368)%10:0,((&l394)->l369
)?((&l394)->v6_128)*100/((&l394)->l369):0,((&l394)->l369)?((&l394)->v6_128)*
1000/((&l394)->l369)%10:0,(((&l394)->l367+(&l394)->l368+(&l394)->l369))?(((&
l394)->v4+(&l394)->v6_64+(&l394)->v6_128))*100/(((&l394)->l367+(&l394)->l368+
(&l394)->l369)):0,(((&l394)->l367+(&l394)->l368+(&l394)->l369))?(((&l394)->v4
+(&l394)->v6_64+(&l394)->v6_128))*1000/(((&l394)->l367+(&l394)->l368+(&l394)
->l369))%10:0);}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){l381(l2,
l151,alpm_vrf_handle[l2],l375);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_CNT){l381(
l2,l151,alpm_vrf_handle[l2],l372);l381(l2,l151,alpm_vrf_handle[l2],l373);}if(
flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l381(l2,l151,alpm_vrf_handle[l2],l374)
;l381(l2,l151,alpm_vrf_handle[l2],l377);l381(l2,l151,alpm_vrf_handle[l2],l376
);}sal_free(l392);return SOC_E_NONE;}int soc_alpm_bucket_sanity_check(int l2,
soc_mem_t l263,int index){int l130 = SOC_E_NONE;int l173,l260,l269,l10,l145 = 
-1;int l151 = 0,l28;uint32 l140 = 0;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];
uint32 l397[SOC_MAX_MEM_FIELD_WORDS];int l398,l399,l400;defip_entry_t
lpm_entry;int l274,l401;soc_mem_t l24;int l194,l266,l402,l403;int l404 = 0;
soc_field_t l405[2] = {VALID0f,VALID1f};soc_field_t l406[2] = {GLOBAL_HIGH0f,
GLOBAL_HIGH1f};soc_field_t l407[2] = {ALG_BKT_PTR0f,ALG_BKT_PTR1f};l274 = 
soc_mem_index_min(l2,l263);l401 = soc_mem_index_max(l2,l263);if((index>= 0)&&
(index<l274||index>l401)){return SOC_E_PARAM;}else if(index>= 0){l274 = index
;l401 = index;}SOC_ALPM_LPM_LOCK(l2);for(l173 = l274;l173<= l401;l173++){
SOC_ALPM_LPM_UNLOCK(l2);SOC_ALPM_LPM_LOCK(l2);l130 = soc_mem_read(l2,l263,
MEM_BLOCK_ANY,l173,(void*)l15);if(SOC_FAILURE(l130)){continue;}l10 = 
soc_mem_field32_get(l2,l263,(void*)l15,MODE0f);if(l10){l266 = 1;l24 = 
L3_DEFIP_ALPM_IPV6_64m;l402 = 16;}else{l266 = 2;l24 = L3_DEFIP_ALPM_IPV4m;
l402 = 24;}for(l194 = 0;l194<l266;l194++){if(soc_mem_field32_get(l2,l263,(
void*)l15,l405[l194]) == 0||soc_mem_field32_get(l2,l263,(void*)l15,l406[l194]
) == 1){continue;}l403 = soc_mem_field32_get(l2,l263,(void*)l15,l407[l194]);
if(l194 == 1){soc_alpm_lpm_ip4entry1_to_0(l2,l15,l397,PRESERVE_HIT);}else{
soc_alpm_lpm_ip4entry0_to_0(l2,l15,l397,PRESERVE_HIT);}l130 = 
soc_alpm_lpm_vrf_get(l2,l397,&l151,&l28);if(SOC_FAILURE(l130)){continue;}if(
SOC_ALPM_V6_SCALE_CHECK(l2,l10)){l402<<= 1;}l145 = -1;for(l260 = 0;l260<l402;
l260++){l130 = _soc_alpm_mem_index(l2,l24,l403,l260,l140,&l269);if(
SOC_FAILURE(l130)){continue;}l130 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,l269,(
void*)l15);if(SOC_FAILURE(l130)){break;}if(!soc_mem_field32_get(l2,l24,(void*
)l15,VALIDf)){continue;}l130 = l27(l2,(void*)l15,l24,l10,l151,l403,0,&
lpm_entry);if(SOC_FAILURE(l130)){continue;}l130 = l149(l2,(void*)&lpm_entry,
l24,(void*)l15,&l398,&l399,&l400,FALSE);if(SOC_FAILURE(l130)){soc_cm_debug(
DK_ERR,"\tLaunched AUX operation for "
"index %d bucket %d sanity check failed\n",l173,l403);l404++;continue;}if(
l399!= l403){soc_cm_debug(DK_ERR,"\tEntry at index %d does not belong "
"to bucket %d(from bucket %d)\n",l400,l403,l399);l404++;}if(l145 == -1){l145 = 
l398;continue;}if(l145!= l398){int l408,l409;l408 = soc_alpm_logical_idx(l2,
l263,l145>>1,1);l409 = soc_alpm_logical_idx(l2,l263,l398>>1,1);soc_cm_debug(
DK_ERR,"\tAliased bucket %d(returned "
"bucket %d) found from TCAM1 %d and TCAM2 %d\n",l403,l399,l408,l409);l404++;}
}}}SOC_ALPM_LPM_UNLOCK(l2);if(l404 == 0){soc_cm_print(
"\tMemory %s index %d Bucket sanity check passed\n",SOC_MEM_NAME(l2,l263),
index);return SOC_E_NONE;}soc_cm_debug(DK_ERR,
"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n",SOC_MEM_NAME(l2,l263),index,l404);return
SOC_E_FAIL;}int soc_alpm_pivot_sanity_check(int l2,soc_mem_t l263,int index){
int l173,l194,l288 = 0;int l269,l274,l401;int l130 = SOC_E_NONE;int*l410 = 
NULL;int l398,l399,l400;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l133,l10 = 0,
l266 = 2;int l411,l412[2];soc_mem_t l413,l414;int l404 = 0;soc_field_t l405[2
] = {VALID0f,VALID1f};soc_field_t l406[2] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};
soc_field_t l415[2] = {NEXT_HOP_INDEX0f,NEXT_HOP_INDEX1f};soc_field_t l407[2]
= {ALG_BKT_PTR0f,ALG_BKT_PTR1f};l274 = soc_mem_index_min(l2,l263);l401 = 
soc_mem_index_max(l2,l263);if((index>= 0)&&(index<l274||index>l401)){return
SOC_E_PARAM;}else if(index>= 0){l274 = index;l401 = index;}l410 = sal_alloc(
sizeof(int)*MAX_PIVOT_COUNT,"Bucket index array");if(l410 == NULL){l130 = 
SOC_E_MEMORY;return l130;}sal_memset(l410,0xff,sizeof(int)*MAX_PIVOT_COUNT);
SOC_ALPM_LPM_LOCK(l2);for(l173 = l274;l173<= l401;l173++){SOC_ALPM_LPM_UNLOCK
(l2);SOC_ALPM_LPM_LOCK(l2);l130 = soc_mem_read(l2,l263,MEM_BLOCK_ANY,l173,(
void*)l15);if(SOC_FAILURE(l130)){soc_cm_debug(DK_ERR,
"\tRead memory %s index %d (original) ""return %d\n",SOC_MEM_NAME(l2,l263),
l173,l130);l404++;continue;}l10 = soc_mem_field32_get(l2,l263,(void*)l15,
MODE0f);if(l10){l266 = 1;l414 = L3_DEFIP_ALPM_IPV6_64m;}else{l266 = 2;l414 = 
L3_DEFIP_ALPM_IPV4m;}for(l194 = 0;l194<l266;l194++){if(soc_mem_field32_get(l2
,l263,(void*)l15,l405[l194]) == 0||soc_mem_field32_get(l2,l263,(void*)l15,
l406[l194]) == 1){continue;}l133 = soc_mem_field32_get(l2,l263,(void*)l15,
l407[l194]);if(l133!= 0){if(l410[l133] == -1){l410[l133] = l173;}else{
soc_cm_debug(DK_ERR,"\tDuplicated bucket pointer "
"%d detected, in memory %s index1 %d ""and index2 %d\n",l133,SOC_MEM_NAME(l2,
l263),l410[l133],l173);l404++;continue;}}l130 = alpm_bucket_is_assigned(l2,
l133,l10,&l288);if(l130 == SOC_E_PARAM){soc_cm_debug(DK_ERR,
"\tInvalid bucket pointer %d ""detected, in memory %s index %d\n",l133,
SOC_MEM_NAME(l2,l263),l173);l404++;continue;}if(l130>= 0&&l288 == 0){
soc_cm_debug(DK_ERR,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n",l133,SOC_MEM_NAME(l2,l263),l173);l404++;
continue;}l413 = _soc_trident2_alpm_bkt_view_get(l2,l133<<2);if(l414!= l413){
soc_cm_debug(DK_ERR,"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual"" %s\n",l133,SOC_MEM_NAME(l2,l414),
SOC_MEM_NAME(l2,l413));l404++;continue;}l411 = soc_mem_field32_get(l2,l263,(
void*)l15,l415[l194]);if(l194 == 1){l130 = soc_alpm_lpm_ip4entry1_to_0(l2,l15
,l15,PRESERVE_HIT);if(SOC_FAILURE(l130)){continue;}}l398 = -1;l130 = l149(l2,
l15,l414,(void*)l15,&l398,&l399,&l400,FALSE);if(l398 == -1){soc_cm_debug(
DK_ERR,"\tLaunched AUX operation for PIVOT ""index %d sanity check failed\n",
l173);l404++;continue;}l269 = soc_alpm_logical_idx(l2,l263,l398>>1,1);l130 = 
soc_mem_read(l2,l263,MEM_BLOCK_ANY,l269,(void*)l15);if(SOC_FAILURE(l130)){
soc_cm_debug(DK_ERR,"\tRead memory %s index %d (nexthop) ""return %d\n",
SOC_MEM_NAME(l2,l263),l269,l130);l404++;continue;}l412[0] = 
soc_mem_field32_get(l2,l263,(void*)l15,l415[0]);l412[1] = soc_mem_field32_get
(l2,l263,(void*)l15,l415[1]);if(l411!= l412[l398&1]){soc_cm_debug(DK_ERR,
"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d entry1 %d, index2 %d ""entry2 %d\n",l173,l194,l269
,l398&1);l404++;continue;}}}SOC_ALPM_LPM_UNLOCK(l2);sal_free(l410);if(l404 == 
0){soc_cm_print("\tMemory %s index %d Pivot sanity check passed\n",
SOC_MEM_NAME(l2,l263),index);return SOC_E_NONE;}soc_cm_debug(DK_ERR,
"\tMemory %s index %d Pivot sanity check failed, ""encountered %d error(s)\n"
,SOC_MEM_NAME(l2,l263),index,l404);return SOC_E_FAIL;}
#endif /* ALPM_ENABLE */
