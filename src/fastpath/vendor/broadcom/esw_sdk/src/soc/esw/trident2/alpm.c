/*
 * $Id: alpm_c_ref 1.1.6.6 Broadcom SDK $
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
int l32);static int l33(int l2,int l28,int l34);typedef struct l35{int l36;
int l37;int l38;int next;int l39;int l40;}l41,*l42;static l42 l43[
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
soc_field_info_t*l89;soc_field_info_t*l90;}l91,*l92;static l92 l93[
SOC_MAX_NUM_DEVICES];typedef struct l94{int l21;int l95;int l96;uint16*l97;
uint16*l98;}l99;typedef uint32 l100[5];typedef int(*l101)(l100 l102,l100 l103
);static l99*l104[SOC_MAX_NUM_DEVICES];static void l105(int l2,void*l15,int
index,l100 l106);static uint16 l107(uint8*l108,int l109);static int l110(int
l21,int l95,int l96,l99**l111);static int l112(l99*l113);static int l114(l99*
l115,l101 l116,l100 entry,int l117,uint16*l118);static int l119(l99*l115,l101
l116,l100 entry,int l117,uint16 l120,uint16 l121);static int l122(l99*l115,
l101 l116,l100 entry,int l117,uint16 l123);alpm_vrf_handle_t alpm_vrf_handle[
SOC_MAX_NUM_DEVICES][MAX_VRF_ID];alpm_pivot_t*tcam_pivot[SOC_MAX_NUM_DEVICES]
[MAX_PIVOT_COUNT];int soc_alpm_mode_get(int l2){uint32 l124;
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l2,&l124));return
soc_reg_field_get(l2,L3_DEFIP_RPF_CONTROLr,l124,LOOKUP_MODEf);}int
_soc_alpm_rpf_entry(int l2,int l125){int l126;l126 = (l125>>2)&0x3fff;l126+= 
SOC_ALPM_BUCKET_COUNT(l2);return(l125&~(0x3fff<<2))|(l126<<2);}int
soc_alpm_physical_idx(int l2,soc_mem_t l24,int index,int l127){int l128 = 
index&1;if(l127){return soc_trident2_l3_defip_index_map(l2,l24,index);}index
>>= 1;index = soc_trident2_l3_defip_index_map(l2,l24,index);index<<= 1;index
|= l128;return index;}int soc_alpm_logical_idx(int l2,soc_mem_t l24,int index
,int l127){int l128 = index&1;if(l127){return
soc_trident2_l3_defip_index_remap(l2,l24,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_remap(l2,l24,index);index<<= 1;index|= l128;
return index;}static int l129(int l2,void*entry,uint32*prefix,uint32*l17,int*
l26){int l130,l131,l10;int l117 = 0;int l132 = SOC_E_NONE;uint32 l133,l128;
prefix[0] = prefix[1] = prefix[2] = prefix[3] = prefix[4] = 0;l10 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,MODE0f);l130 = soc_mem_field32_get(l2,
L3_DEFIPm,entry,IP_ADDR0f);l131 = soc_mem_field32_get(l2,L3_DEFIPm,entry,
IP_ADDR_MASK0f);prefix[1] = l130;l130 = soc_mem_field32_get(l2,L3_DEFIPm,
entry,IP_ADDR1f);l131 = soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR_MASK1f
);prefix[0] = l130;if(l10){prefix[4] = prefix[1];prefix[3] = prefix[0];prefix
[1] = prefix[0] = 0;l131 = soc_mem_field32_get(l2,L3_DEFIPm,entry,
IP_ADDR_MASK0f);if((l132 = _ipmask2pfx(l131,&l117))<0){return(l132);}l131 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR_MASK1f);if(l117){if(l131!= 
0xffffffff){return(SOC_E_PARAM);}l117+= 32;}else{if((l132 = _ipmask2pfx(l131,
&l117))<0){return(l132);}}l133 = 64-l117;if(l133<32){prefix[4]>>= l133;l128 = 
(((32-l133) == 32)?0:(prefix[3])<<(32-l133));prefix[3]>>= l133;prefix[4]|= 
l128;}else{prefix[4] = (((l133-32) == 32)?0:(prefix[3])>>(l133-32));prefix[3]
= 0;}}else{l131 = soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((
l132 = _ipmask2pfx(l131,&l117))<0){return(l132);}prefix[1] = (((32-l117) == 
32)?0:(prefix[1])>>(32-l117));prefix[0] = 0;}*l17 = l117;*l26 = (prefix[0] == 
0)&&(prefix[1] == 0)&&(l117 == 0);return SOC_E_NONE;}int _soc_alpm_find_in_bkt
(int l2,soc_mem_t l24,int bucket_index,int l134,uint32*l15,void*l135,int*l118
,int l34){int l132;l132 = soc_mem_alpm_lookup(l2,l24,bucket_index,
MEM_BLOCK_ANY,l134,l15,l135,l118);if(SOC_SUCCESS(l132)){return l132;}if(
SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return soc_mem_alpm_lookup(l2,l24,
bucket_index+1,MEM_BLOCK_ANY,l134,l15,l135,l118);}return l132;}static int l136
(int l2,void*l8,soc_mem_t l24,void*l135,int*l137,int*bucket_index,int*l16){
defip_aux_scratch_entry_t l13;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l138,
l28,l34;int l118;uint32 l11,l134;int l132 = SOC_E_NONE;int l139 = 0;l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l138,&l28));if(l138 == 0){if(
soc_alpm_mode_get(l2)){return SOC_E_PARAM;}}if(l28 == SOC_VRF_MAX(l2)+1){l11 = 
0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l134);}else{l11 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l134);}if(l138!= SOC_L3_VRF_OVERRIDE){
sal_memset(&l13,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(
l2,l8,l34,l11,0,&l13));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,PREFIX_LOOKUP,
&l13,&l139,l137,bucket_index));if(l139){l20(l2,l8,l15,0,l24,0,0);l132 = 
_soc_alpm_find_in_bkt(l2,l24,*bucket_index,l134,l15,l135,&l118,l34);if(
SOC_SUCCESS(l132)){*l16 = l118;}}else{l132 = SOC_E_NOT_FOUND;}}return l132;}
static int l140(int l2,void*l8,void*l135,void*l141,soc_mem_t l24,int*l16){
defip_aux_scratch_entry_t l13;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l138,
l34,l28;int l118,bucket_index;uint32 l11,l134;int l132 = SOC_E_NONE;int l139 = 
0,l128 = 0;int l137;l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34)
{if(!(l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM)
;}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l138,&l28));if(l28 == 
SOC_VRF_MAX(l2)+1){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l134);}else{
l11 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l134);}if(!soc_alpm_mode_get(l2)){
l11 = 2;}if(l138!= SOC_L3_VRF_OVERRIDE){sal_memset(&l13,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l2,l8,l34,l11,0,&l13));
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,PREFIX_LOOKUP,&l13,&l139,&l137,&
bucket_index));if(l139){l132 = _soc_alpm_find_in_bkt(l2,l24,bucket_index,l134
,l135,l15,&l118,l34);if(SOC_SUCCESS(l132)){*l16 = l118;SOC_IF_ERROR_RETURN(
soc_mem_write(l2,l24,MEM_BLOCK_ANY,l118,l135));if(l132!= SOC_E_NONE){return
SOC_E_FAIL;}if(SOC_URPF_STATUS_GET(l2)){SOC_IF_ERROR_RETURN(soc_mem_write(l2,
l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l118),l141));if(l132!= SOC_E_NONE){
return SOC_E_FAIL;}}l128 = soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,
IP_LENGTHf);soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,REPLACE_LENf,
l128);SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l139,&
l137,&bucket_index));if(SOC_URPF_STATUS_GET(l2)){l128 = soc_mem_field32_get(
l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l128+= 1;soc_mem_field32_set(l2,
L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l128);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l139,&l137,&bucket_index));}}}else
{return(SOC_E_NOT_FOUND);}}return l132;}int alpm_mem_prefix_array_cb(
trie_node_t*node,void*l142){alpm_mem_prefix_array_t*l143 = (
alpm_mem_prefix_array_t*)l142;if(node->type == PAYLOAD){l143->prefix[l143->
count] = (payload_t*)node;l143->count++;}return SOC_E_NONE;}int
alpm_delete_node_cb(trie_node_t*node,void*l142){if(node!= NULL){sal_free(node
);}return SOC_E_NONE;}static int l144(int l2,int l145,int l34,int l146){int
l132,l128,index;defip_aux_table_entry_t entry;index = l145>>(l34?0:1);l132 = 
soc_mem_read(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l132);if(l34){soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,
&entry,BPM_LENGTH0f,l146);soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH1f,l146);l128 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE0f);}else{if(l145&1){soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry
,BPM_LENGTH1f,l146);l128 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE1f);}else{soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH0f,l146);l128 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE0f);}}l132 = soc_mem_write(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,
&entry);SOC_IF_ERROR_RETURN(l132);if(SOC_URPF_STATUS_GET(l2)){l128++;if(l34){
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l146);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l146);}else{if
(l145&1){soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l146)
;}else{soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l146);}
}soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l128);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l128);index+= (2*
soc_mem_index_count(l2,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l2,L3_DEFIPm))
/2;l132 = soc_mem_write(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}
return l132;}static int l147(int l2,int l148,void*entry,void*l149,int l150){
uint32 l128,l131,l34,l11,l151 = 0;soc_mem_t l24 = L3_DEFIPm;soc_mem_t l152 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l153;int l132 = SOC_E_NONE,l117,l154,l28,
l155;SOC_IF_ERROR_RETURN(soc_mem_read(l2,l152,MEM_BLOCK_ANY,l148,l149));l128 = 
soc_mem_field32_get(l2,l24,entry,VRF_ID_0f);soc_mem_field32_set(l2,l152,l149,
VRF0f,l128);l128 = soc_mem_field32_get(l2,l24,entry,VRF_ID_1f);
soc_mem_field32_set(l2,l152,l149,VRF1f,l128);l128 = soc_mem_field32_get(l2,
l24,entry,MODE0f);soc_mem_field32_set(l2,l152,l149,MODE0f,l128);l128 = 
soc_mem_field32_get(l2,l24,entry,MODE1f);soc_mem_field32_set(l2,l152,l149,
MODE1f,l128);l34 = l128;l128 = soc_mem_field32_get(l2,l24,entry,VALID0f);
soc_mem_field32_set(l2,l152,l149,VALID0f,l128);l128 = soc_mem_field32_get(l2,
l24,entry,VALID1f);soc_mem_field32_set(l2,l152,l149,VALID1f,l128);l128 = 
soc_mem_field32_get(l2,l24,entry,IP_ADDR_MASK0f);if((l132 = _ipmask2pfx(l128,
&l117))<0){return l132;}l131 = soc_mem_field32_get(l2,l24,entry,
IP_ADDR_MASK1f);if((l132 = _ipmask2pfx(l131,&l154))<0){return l132;}if(l34){
soc_mem_field32_set(l2,l152,l149,IP_LENGTH0f,l117+l154);soc_mem_field32_set(
l2,l152,l149,IP_LENGTH1f,l117+l154);}else{soc_mem_field32_set(l2,l152,l149,
IP_LENGTH0f,l117);soc_mem_field32_set(l2,l152,l149,IP_LENGTH1f,l154);}l128 = 
soc_mem_field32_get(l2,l24,entry,IP_ADDR0f);soc_mem_field32_set(l2,l152,l149,
IP_ADDR0f,l128);l128 = soc_mem_field32_get(l2,l24,entry,IP_ADDR1f);
soc_mem_field32_set(l2,l152,l149,IP_ADDR1f,l128);l128 = soc_mem_field32_get(
l2,l24,entry,ENTRY_TYPE0f);soc_mem_field32_set(l2,l152,l149,ENTRY_TYPE0f,l128
);l128 = soc_mem_field32_get(l2,l24,entry,ENTRY_TYPE1f);soc_mem_field32_set(
l2,l152,l149,ENTRY_TYPE1f,l128);if(!l34){sal_memcpy(&l153,entry,sizeof(l153))
;l132 = soc_alpm_lpm_vrf_get(l2,(void*)&l153,&l28,&l117);SOC_IF_ERROR_RETURN(
l132);SOC_IF_ERROR_RETURN(soc_alpm_lpm_ip4entry1_to_0(l2,&l153,&l153,
PRESERVE_HIT));l132 = soc_alpm_lpm_vrf_get(l2,(void*)&l153,&l155,&l117);
SOC_IF_ERROR_RETURN(l132);}else{l132 = soc_alpm_lpm_vrf_get(l2,entry,&l28,&
l117);}if(SOC_URPF_STATUS_GET(l2)){if(l150>= (soc_mem_index_count(l2,
L3_DEFIPm)>>1)){l151 = 1;}}switch(l28){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l2,l152,l149,VALID0f,0);l11 = 0;break;case
SOC_L3_VRF_GLOBAL:l11 = l151?1:0;break;default:l11 = l151?3:2;break;}
soc_mem_field32_set(l2,l152,l149,DB_TYPE0f,l11);if(!l34){switch(l155){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l2,l152,l149,VALID1f,0);l11 = 0;break
;case SOC_L3_VRF_GLOBAL:l11 = l151?1:0;break;default:l11 = l151?3:2;break;}
soc_mem_field32_set(l2,l152,l149,DB_TYPE1f,l11);}else{if(l155 == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l2,l152,l149,VALID1f,0);}
soc_mem_field32_set(l2,l152,l149,DB_TYPE1f,l11);}if(l151){l128 = 
soc_mem_field32_get(l2,l24,entry,ALG_BKT_PTR0f);if(l128){l128+= 
SOC_ALPM_BUCKET_COUNT(l2);soc_mem_field32_set(l2,l24,entry,ALG_BKT_PTR0f,l128
);}if(!l34){l128 = soc_mem_field32_get(l2,l24,entry,ALG_BKT_PTR1f);if(l128){
l128+= SOC_ALPM_BUCKET_COUNT(l2);soc_mem_field32_set(l2,l24,entry,
ALG_BKT_PTR1f,l128);}}}return SOC_E_NONE;}static int l156(int l2,int l157,int
index,int l158,void*entry){defip_aux_table_entry_t l149;l158 = 
soc_alpm_physical_idx(l2,L3_DEFIPm,l158,1);SOC_IF_ERROR_RETURN(l147(l2,l158,
entry,(void*)&l149,index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l2,
MEM_BLOCK_ANY,index,entry));index = soc_alpm_physical_idx(l2,L3_DEFIPm,index,
1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,index,&l149
));return SOC_E_NONE;}int _soc_alpm_insert_in_bkt(int l2,soc_mem_t l24,int
bucket_index,int l134,void*l135,uint32*l15,int*l118,int l34){int l132;l132 = 
soc_mem_alpm_insert(l2,l24,bucket_index,MEM_BLOCK_ANY,l134,l135,l15,l118);if(
l132 == SOC_E_FULL){if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_insert(l2,l24,bucket_index+1,MEM_BLOCK_ANY,l134,l135,l15,l118);}
}return l132;}static int l159(int l2,void*l8,soc_mem_t l24,void*l135,void*
l141,int*l16){alpm_pivot_t*l160,*l161,*l162;defip_aux_scratch_entry_t l13;
uint32 l15[SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l163,l164;uint32 l165[5]
;int l34,l28,l138;int l118,bucket_index;int l132 = SOC_E_NONE,l166;uint32 l11
,l134,l146 = 0;int l139 =0;int l137,l145;int l167 = 0;trie_t*trie,*l168;
trie_node_t*l169,*l170 = NULL,*l171 = NULL;payload_t*l172,*l173,*l174;
defip_entry_t lpm_entry;alpm_bucket_handle_t*l175;int l176,l177 = -1,l26 = 0;
alpm_mem_prefix_array_t l143;defip_alpm_ipv4_entry_t l178,l179;
defip_alpm_ipv6_64_entry_t l180,l181;void*l182,*l183;int*l121;l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l138,&l28));if(l28 == 
SOC_VRF_MAX(l2)+1){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l134);}else{
l11 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l134);}l24 = (l34)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l182 = ((l34)?((uint32*)&(l180)):(
(uint32*)&(l178)));l183 = ((l34)?((uint32*)&(l181)):((uint32*)&(l179)));
sal_memset(&l13,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(
l2,l8,l34,l11,0,&l13));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,PREFIX_LOOKUP,
&l13,&l139,&l137,&bucket_index));if(l139 == 0){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not find bucket to"" insert prefix\n");return
SOC_E_NOT_FOUND;}l132 = _soc_alpm_insert_in_bkt(l2,l24,bucket_index,l134,l135
,l15,&l118,l34);if(l132 == SOC_E_NONE){*l16 = l118;if(SOC_URPF_STATUS_GET(l2)
){l166 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l118),l141
);if(SOC_FAILURE(l166)){return l166;}}}if(l132 == SOC_E_FULL){l167 = 1;}l132 = 
l129(l2,l8,prefix,&l164,&l26);if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: prefix create failed\n");return l132;}l160 = 
ALPM_TCAM_PIVOT(l2,l137);trie = PIVOT_BUCKET_TRIE(l160);l162 = l160;l172 = 
sal_alloc(sizeof(payload_t),"Payload for Key");if(l172 == NULL){soc_cm_debug(
DK_ERR,"_soc_alpm_insert: Unable to allocate memory for ""trie node \n");
return SOC_E_MEMORY;}l173 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l173 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory for ""pfx trie node \n");
sal_free(l172);return SOC_E_MEMORY;}sal_memset(l172,0,sizeof(*l172));
sal_memset(l173,0,sizeof(*l173));l172->key[0] = prefix[0];l172->key[1] = 
prefix[1];l172->key[2] = prefix[2];l172->key[3] = prefix[3];l172->key[4] = 
prefix[4];l172->len = l164;l172->index = l118;sal_memcpy(l173,l172,sizeof(*
l172));l173->index = (int)l172;l132 = trie_insert(trie,prefix,NULL,l164,(
trie_node_t*)l172);if(SOC_FAILURE(l132)){goto l184;}if(l34){l168 = 
VRF_PREFIX_TRIE_IPV6(l2,l28);}else{l168 = VRF_PREFIX_TRIE_IPV4(l2,l28);}if(!
l26){l132 = trie_insert(l168,prefix,NULL,l164,(trie_node_t*)l173);}else{l171 = 
NULL;l132 = trie_find_lpm(l168,0,0,&l171);l174 = (payload_t*)l171;if(
SOC_SUCCESS(l132)){l174->index = (int)l172;}}l163 = l164;if(SOC_FAILURE(l132)
){goto l185;}if(l167){l132 = alpm_bucket_assign(l2,&bucket_index,l34);if(
SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Unable to allocate"
"new bucket for split\n");bucket_index = -1;goto l186;}l132 = trie_split(trie
,l34?_MAX_KEY_LEN_144_:_MAX_KEY_LEN_48_,FALSE,l165,&l164,&l169,NULL,FALSE);if
(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not split bucket""for prefix 0x%08x 0x%08x\n",prefix
[0],prefix[1]);goto l186;}l171 = NULL;l132 = trie_find_lpm(l168,l165,l164,&
l171);l174 = (payload_t*)l171;if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n",l2,l165[0],l165[1],l165[2
],l165[3],l165[4],l164);goto l186;}if(l174->index){if(l174->index == (int)
l172){sal_memcpy(l182,l135,l34?sizeof(defip_alpm_ipv6_64_entry_t):sizeof(
defip_alpm_ipv4_entry_t));}else{l132 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,((
payload_t*)l174->index)->index,l182);}if(SOC_FAILURE(l132)){goto l186;}l132 = 
l27(l2,l182,l24,l34,l138,bucket_index,0,&lpm_entry);if(SOC_FAILURE(l132)){
goto l186;}l146 = ((payload_t*)(l174->index))->len;}else{l132 = soc_mem_read(
l2,L3_DEFIPm,MEM_BLOCK_ANY,soc_alpm_logical_idx(l2,L3_DEFIPm,l137>>1,1),&
lpm_entry);if((!l34)&&(l137&1)){l132 = soc_alpm_lpm_ip4entry1_to_0(l2,&
lpm_entry,&lpm_entry,0);}}do{if(!(l34)){l165[0] = (((32-l164) == 32)?0:(l165[
1])<<(32-l164));l165[1] = 0;}else{int l187 = 64-l164;int l188;if(l187<32){
l188 = l165[3]<<l187;l188|= (((32-l187) == 32)?0:(l165[4])>>(32-l187));l165[0
] = l165[4]<<l187;l165[1] = l188;l165[2] = l165[3] = l165[4] = 0;}else{l165[1
] = (((l187-32) == 32)?0:(l165[4])<<(l187-32));l165[0] = l165[2] = l165[3] = 
l165[4] = 0;}}}while(0);l30(l2,l165,l164,l28,l34,&lpm_entry,0,0);
soc_L3_DEFIPm_field32_set(l2,&lpm_entry,ALG_BKT_PTR0f,bucket_index);l175 = 
sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l175 == NULL)
{soc_cm_debug(DK_ERR,"_soc_alpm_insert: Unable to allocate memory "
"for PIVOT trie node \n");l132 = SOC_E_MEMORY;goto l186;}sal_memset(l175,0,
sizeof(*l175));l160 = sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot")
;if(l160 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n");l132 = 
SOC_E_MEMORY;goto l186;}sal_memset(l160,0,sizeof(*l160));PIVOT_BUCKET_HANDLE(
l160) = l175;if(l34){l132 = trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(
l160));}else{l132 = trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l160));}
PIVOT_BUCKET_TRIE(l160)->trie = l169;PIVOT_BUCKET_INDEX(l160) = bucket_index;
l160->key[0] = l165[0];l160->key[1] = l165[1];l160->len = l164;sal_memset(&
l143,0,sizeof(l143));l132 = trie_traverse(PIVOT_BUCKET_TRIE(l160),
alpm_mem_prefix_array_cb,&l143,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l132)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n",prefix[1],prefix[2],prefix[3],
prefix[4]);goto l186;}l121 = sal_alloc(sizeof(*l121)*l143.count,
"Temp storage for location of prefixes in new bucket");if(l121 == NULL){l132 = 
SOC_E_MEMORY;goto l186;}sal_memset(l121,-1,sizeof(*l121)*l143.count);for(l176
= 0;l176<l143.count;l176++){payload_t*prefix = l143.prefix[l176];if(prefix->
index>0){l132 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,prefix->index,l182);if(
SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to"
"read prefix 0x%08x 0x%08x 0x%08x 0x%08x for move\n",l143.prefix[l176]->key[1
],l143.prefix[l176]->key[2],l143.prefix[l176]->key[3],l143.prefix[l176]->key[
4]);goto l189;}if(SOC_URPF_STATUS_GET(l2)){l132 = soc_mem_read(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,prefix->index),l183);if(SOC_FAILURE(l132
)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to"
"read rpf prefix 0x%08x 0x%08x 0x%08x 0x%08x for move\n",l143.prefix[l176]->
key[1],l143.prefix[l176]->key[2],l143.prefix[l176]->key[3],l143.prefix[l176]
->key[4]);goto l189;}}l132 = _soc_alpm_insert_in_bkt(l2,l24,bucket_index,l134
,l182,l15,&l118,l34);if(SOC_SUCCESS(l132)){if(SOC_URPF_STATUS_GET(l2)){l132 = 
soc_mem_write(l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l118),l183);}}}else
{l132 = _soc_alpm_insert_in_bkt(l2,l24,bucket_index,l134,l135,l15,&l118,l34);
if(SOC_SUCCESS(l132)){l177 = l176;*l16 = l118;if(SOC_URPF_STATUS_GET(l2)){
l132 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l118),l141);
}}}l121[l176] = l118;if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Failed to"
"write prefix 0x%08x 0x%08x 0x%08x 0x%08x for move\n",l143.prefix[l176]->key[
1],l143.prefix[l176]->key[2],l143.prefix[l176]->key[3],l143.prefix[l176]->key
[4]);goto l189;}}l132 = l5(l2,&lpm_entry,&l145);if(SOC_FAILURE(l132)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Unable to add new""pivot to tcam\n");
goto l189;}l145 = soc_alpm_physical_idx(l2,L3_DEFIPm,l145,l34);l132 = l144(l2
,l145,l34,l146);if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n",l145);goto l190;}
ALPM_TCAM_PIVOT(l2,l145<<(l34?1:0)) = l160;PIVOT_TCAM_INDEX(l160) = l145<<(
l34?1:0);for(l176 = 0;l176<l143.count;l176++){payload_t*prefix = l143.prefix[
l176];if(prefix->index>0){l132 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,prefix->
index,l182);soc_mem_field32_set(l2,l24,l182,VALIDf,0);if(SOC_FAILURE(l132)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to read"
"bkt entry for invalidate for pfx 0x%08x 0x%08x 0x%08x ""0x%08x\n",prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]);goto l191;}if(
SOC_URPF_STATUS_GET(l2)){l132 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l2,prefix->index),l183);soc_mem_field32_set(l2,l24,l183,
VALIDf,0);}l132 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,prefix->index,l182);if(
SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to clear"
" prefixes from old bkt for pfx 0x%08x 0x%08x 0x%08x\n""0x%08x\n",prefix->key
[1],prefix->key[2],prefix->key[3],prefix->key[4]);goto l191;}if(
SOC_URPF_STATUS_GET(l2)){l132 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l2,prefix->index),l183);}}}for(l176 = 0;l176<l143.count;
l176++){payload_t*prefix = l143.prefix[l176];l132 = soc_mem_write(l2,l24,
MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null(l2,l24));if(SOC_FAILURE(l132))
{soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to remove"
" prefixes from old bkt for pfx 0x%08x 0x%08x\n",prefix->key[0],prefix->key[1
]);}if(SOC_URPF_STATUS_GET(l2)){l132 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l2,prefix->index),soc_mem_entry_null(l2,l24));if(
SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to remove"
" prefixes from old urpf bkt for pfx 0x%08x 0x%08x\n",prefix->key[0],prefix->
key[1]);}}prefix->index = l121[l176];}sal_free(l121);if(l177 == -1){l132 = 
_soc_alpm_insert_in_bkt(l2,l24,PIVOT_BUCKET_HANDLE(l162)->bucket_index,l134,
l135,l15,&l118,l34);if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n");
goto l186;}if(SOC_URPF_STATUS_GET(l2)){l132 = soc_mem_write(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l118),l141);}*l16 = l118;l172->index = 
l118;}}VRF_TRIE_ROUTES_INC(l2,l28,l34);if(l26){SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l139,&l137,&bucket_index));
sal_free(l173);}SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,INSERT_PROPAGATE,&l13
,&l139,&l137,&bucket_index));if(SOC_URPF_STATUS_GET(l2)){l164 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l164+= 1;
soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l164);
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l139,&l137,&
bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,INSERT_PROPAGATE,&l13,
&l139,&l137,&bucket_index));}return l132;l191:for(l176 = 0;l176<l143.count;
l176++){payload_t*prefix = l143.prefix[l176];if(prefix->index>0){l166 = 
soc_mem_read(l2,l24,MEM_BLOCK_ANY,prefix->index,l182);if(SOC_FAILURE(l166)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failure to read prefix"
"0x%08x 0x%08x for move back\n",prefix->key[0],prefix->key[1]);break;}if(
soc_mem_field32_get(l2,l24,l182,VALIDf)){break;}soc_mem_field32_set(l2,l24,
l182,VALIDf,1);l132 = soc_mem_write(l2,l24,MEM_BLOCK_ALL,prefix->index,l182);
if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Failure to write prefix""0x%08x 0x%08x for move back\n",
prefix->key[0],prefix->key[1]);break;}if(SOC_URPF_STATUS_GET(l2)){l166 = 
soc_mem_read(l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,prefix->index),l182)
;soc_mem_field32_set(l2,l24,l182,VALIDf,1);l166 = soc_mem_write(l2,l24,
MEM_BLOCK_ALL,_soc_alpm_rpf_entry(l2,prefix->index),l182);}}}l190:l166 = l7(
l2,&lpm_entry);if(SOC_FAILURE(l166)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Failure to free new prefix""at %d\n",soc_alpm_logical_idx(
l2,L3_DEFIPm,l145,l34));}ALPM_TCAM_PIVOT(l2,l145<<(l34?1:0)) = NULL;l189:l161
= l162;for(l176 = 0;l176<l143.count;l176++){payload_t*prefix = l143.prefix[
l176];if(l121[l176]!= -1){if(l34){sal_memset(l182,0,sizeof(
defip_alpm_ipv6_64_entry_t));}else{sal_memset(l182,0,sizeof(
defip_alpm_ipv4_entry_t));}l166 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,l121[
l176],l182);_soc_trident2_alpm_bkt_view_set(l2,l121[l176],INVALIDm);if(
SOC_FAILURE(l166)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: mem write failure"
"in bkt move rollback\n");}if(SOC_URPF_STATUS_GET(l2)){l166 = soc_mem_write(
l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l121[l176]),l182);
_soc_trident2_alpm_bkt_view_set(l2,_soc_alpm_rpf_entry(l2,l121[l176]),
INVALIDm);if(SOC_FAILURE(l166)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: urpf mem write ""failure in bkt move rollback\n");}}}l170 = 
NULL;l166 = trie_delete(PIVOT_BUCKET_TRIE(l160),prefix->key,prefix->len,&l170
);l172 = (payload_t*)l170;if(SOC_FAILURE(l166)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n");}if(prefix->
index>0){l166 = trie_insert(PIVOT_BUCKET_TRIE(l161),prefix->key,NULL,prefix->
len,(trie_node_t*)l172);if(SOC_FAILURE(l166)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: trie reinsert failure""in bkt move rollback\n");}}}if(l177
== -1){l166 = trie_delete(PIVOT_BUCKET_TRIE(l161),prefix,l163,&l170);if(
SOC_FAILURE(l166)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n",prefix[0],prefix[1]);}}l166 = 
alpm_bucket_release(l2,PIVOT_BUCKET_INDEX(l160),l34);if(SOC_FAILURE(l166)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: new bucket release ""failure: %d\n",
PIVOT_BUCKET_INDEX(l160));}trie_destroy(PIVOT_BUCKET_TRIE(l160));sal_free(
l175);sal_free(l160);sal_free(l121);l173 = NULL;l166 = trie_delete(l168,
prefix,l163,&l170);l173 = (payload_t*)l170;if(SOC_FAILURE(l166)){soc_cm_debug
(DK_ERR,"_soc_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n",prefix[0],prefix[1]);}if(l173){sal_free(l173)
;}return l132;l186:l170 = NULL;(void)trie_delete(l168,prefix,l163,&l170);l173
= (payload_t*)l170;if(bucket_index!= -1){(void)alpm_bucket_release(l2,
bucket_index,l34);}l185:l170 = NULL;(void)trie_delete(trie,prefix,l163,&l170)
;l172 = (payload_t*)l170;l184:if(l172!= NULL){sal_free(l172);}if(l173!= NULL)
{sal_free(l173);}return l132;}static int l30(int l21,uint32*key,int len,int
l28,int l10,defip_entry_t*lpm_entry,int l31,int l32){uint32 l192;if(l32){
sal_memset(lpm_entry,0,sizeof(defip_entry_t));}soc_L3_DEFIPm_field32_set(l21,
lpm_entry,VRF_ID_0f,l28&SOC_VRF_MAX(l21));if(l28 == (SOC_VRF_MAX(l21)+1)){
soc_L3_DEFIPm_field32_set(l21,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_L3_DEFIPm_field32_set(l21,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(l21));}if(
l10){soc_L3_DEFIPm_field32_set(l21,lpm_entry,IP_ADDR0f,key[0]);
soc_L3_DEFIPm_field32_set(l21,lpm_entry,IP_ADDR1f,key[1]);
soc_L3_DEFIPm_field32_set(l21,lpm_entry,MODE0f,1);soc_L3_DEFIPm_field32_set(
l21,lpm_entry,MODE1f,1);soc_L3_DEFIPm_field32_set(l21,lpm_entry,VRF_ID_1f,l28
&SOC_VRF_MAX(l21));if(l28 == (SOC_VRF_MAX(l21)+1)){soc_L3_DEFIPm_field32_set(
l21,lpm_entry,VRF_ID_MASK1f,0);}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,
VRF_ID_MASK1f,SOC_VRF_MAX(l21));}soc_L3_DEFIPm_field32_set(l21,lpm_entry,
VALID1f,1);if(len>= 32){l192 = 0xffffffff;soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,IP_ADDR_MASK1f,l192);l192 = ~(((len-32) == 32)?0:(0xffffffff)>>(len
-32));soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l192);}else{
l192 = ~(0xffffffff>>len);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l192);}}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,IP_ADDR0f
,key[0]);assert(len<= 32);l192 = (len == 32)?0xffffffff:~(0xffffffff>>len);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l192);}
soc_L3_DEFIPm_field32_set(l21,lpm_entry,VALID0f,1);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,MODE_MASK0f,(1<<soc_mem_field_length(l21,L3_DEFIPm,
MODE_MASK0f))-1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE_MASK1f,(1<<
soc_mem_field_length(l21,L3_DEFIPm,MODE_MASK1f))-1);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,(1<<soc_mem_field_length(l21,L3_DEFIPm,
ENTRY_TYPE_MASK0f))-1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
ENTRY_TYPE_MASK1f,(1<<soc_mem_field_length(l21,L3_DEFIPm,ENTRY_TYPE_MASK1f))-
1);return(SOC_E_NONE);}int _soc_alpm_delete_in_bkt(int l2,soc_mem_t l24,int
l193,int l134,void*l194,uint32*l15,int*l118,int l34){int l132;l132 = 
soc_mem_alpm_delete(l2,l24,l193,MEM_BLOCK_ALL,l134,l194,l15,l118);if(
SOC_SUCCESS(l132)){return l132;}if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_delete(l2,l24,l193+1,MEM_BLOCK_ALL,l134,l194,l15,l118);}return
l132;}static int l195(int l2,void*l8){alpm_pivot_t*l160;
defip_alpm_ipv4_entry_t l178,l179;defip_alpm_ipv6_64_entry_t l180,l181;
defip_alpm_ipv4_entry_t l196,l197;defip_aux_scratch_entry_t l13;uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l24;void*l182,*l194,*l183 = NULL;int l138;
int l10;int l132 = SOC_E_NONE,l166;uint32 l198[5],prefix[5];int l34,l28;
uint32 l164;int l118,bucket_index;int l193;uint32 l11,l134;int l137,l139,l26 = 
0;trie_t*trie,*l168;uint32 l199;defip_entry_t lpm_entry,*l200;payload_t*l172 = 
NULL,*l201 = NULL,*l174 = NULL;trie_node_t*l170 = NULL,*l171 = NULL;l10 = l34
= soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l138,&l28));if(l138!= 
SOC_L3_VRF_OVERRIDE){if(l28 == SOC_VRF_MAX(l2)+1){l11 = 0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l134);}else{l11 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l134);}l132 = l129(l2,l8,prefix,&l164,&l26);
if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: prefix create failed\n");return l132;}if(!
soc_alpm_mode_get(l2)){if(l138!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l2
,l28,l34)>1){if(l26){soc_cm_debug(DK_ERR,"VRF %d: Cannot delete default "
"route if other routes are present in ""this mode",l28);return SOC_E_PARAM;}}
}l11 = 2;}l24 = (l34)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l182 = ((l34
)?((uint32*)&(l180)):((uint32*)&(l178)));l194 = ((l34)?((uint32*)&(l197)):((
uint32*)&(l196)));SOC_ALPM_LPM_LOCK(l2);l132 = l136(l2,l8,l24,l182,&l137,&
bucket_index,&l118);sal_memcpy(l194,l182,l34?sizeof(l180):sizeof(l178));if(
SOC_FAILURE(l132)){SOC_ALPM_LPM_UNLOCK(l2);soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to find ""prefix for delete\n");return l132;}l193 = 
bucket_index;l160 = ALPM_TCAM_PIVOT(l2,l137);trie = PIVOT_BUCKET_TRIE(l160);
l132 = trie_delete(trie,prefix,l164,&l170);l172 = (payload_t*)l170;if(l132!= 
SOC_E_NONE){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Error prefix not present "
"in trie \n");SOC_ALPM_LPM_UNLOCK(l2);return l132;}if(l34){l168 = 
VRF_PREFIX_TRIE_IPV6(l2,l28);}else{l168 = VRF_PREFIX_TRIE_IPV4(l2,l28);}if(!
l26){l132 = trie_delete(l168,prefix,l164,&l170);l201 = (payload_t*)l170;if(
SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n",prefix[0
],prefix[1]);goto l202;}l171 = NULL;l132 = trie_find_lpm(l168,prefix,l164,&
l171);l174 = (payload_t*)l171;if(SOC_SUCCESS(l132)){payload_t*l203 = (
payload_t*)(l174->index);if(l203!= NULL){l199 = l203->len;}else{l199 = 0;}}
else{soc_cm_debug(DK_ERR,"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n",prefix[0],prefix[1]);goto l204;}sal_memcpy(
l198,prefix,sizeof(prefix));do{if(!(l34)){l198[0] = (((32-l164) == 32)?0:(
l198[1])<<(32-l164));l198[1] = 0;}else{int l187 = 64-l164;int l188;if(l187<32
){l188 = l198[3]<<l187;l188|= (((32-l187) == 32)?0:(l198[4])>>(32-l187));l198
[0] = l198[4]<<l187;l198[1] = l188;l198[2] = l198[3] = l198[4] = 0;}else{l198
[1] = (((l187-32) == 32)?0:(l198[4])<<(l187-32));l198[0] = l198[2] = l198[3] = 
l198[4] = 0;}}}while(0);l132 = l30(l2,prefix,l199,l28,l10,&lpm_entry,0,1);
l166 = l136(l2,&lpm_entry,l24,l182,&l137,&bucket_index,&l118);(void)l27(l2,
l182,l24,l10,l138,bucket_index,0,&lpm_entry);(void)l30(l2,l198,l164,l28,l10,&
lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l2)){if(SOC_SUCCESS(l132)){l183 = ((l34
)?((uint32*)&(l181)):((uint32*)&(l179)));l166 = soc_mem_read(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l118),l183);}}if((l199 == 0)&&
SOC_FAILURE(l166)){l200 = l34?VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);sal_memcpy(&lpm_entry,l200,sizeof(
lpm_entry));l132 = l30(l2,l198,l164,l28,l10,&lpm_entry,0,1);}if(SOC_FAILURE(
l132)){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n",prefix[0],prefix[1]);goto l204;}l200 = 
&lpm_entry;}else{l171 = NULL;l132 = trie_find_lpm(l168,prefix,l164,&l171);
l174 = (payload_t*)l171;if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Could not find default ""route in the trie for vrf %d\n",
l28);goto l202;}l174->index = 0;l199 = 0;l200 = l34?
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28):VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);}l132
= l9(l2,l200,l34,l11,l199,&l13);if(SOC_FAILURE(l132)){goto l204;}l132 = 
_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l139,&l137,&bucket_index);if(
SOC_FAILURE(l132)){goto l204;}if(SOC_URPF_STATUS_GET(l2)){uint32 l128;if(l183
!= NULL){l128 = soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);
l128++;soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l128);l128 = 
soc_mem_field32_get(l2,l24,l183,SRC_DISCARDf);soc_mem_field32_set(l2,l24,&l13
,SRC_DISCARDf,l128);l128 = soc_mem_field32_get(l2,l24,l183,DEFAULTROUTEf);
soc_mem_field32_set(l2,l24,&l13,DEFAULTROUTEf,l128);l132 = _soc_alpm_aux_op(
l2,DELETE_PROPAGATE,&l13,&l139,&l137,&bucket_index);}if(SOC_FAILURE(l132)){
goto l204;}}sal_free(l172);if(!l26){sal_free(l201);}if((l160->len!= 0)&&(trie
->trie == NULL)){l30(l2,l160->key,l160->len,l28,l10,&lpm_entry,0,1);l132 = l7
(l2,&lpm_entry);if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to ""delete pivot 0x%08x 0x%08x \n",l160->key[0],
l160->key[1]);}}l132 = _soc_alpm_delete_in_bkt(l2,l24,l193,l134,l194,l15,&
l118,l34);if(!SOC_SUCCESS(l132)){SOC_ALPM_LPM_UNLOCK(l2);l132 = SOC_E_FAIL;
return l132;}if(SOC_URPF_STATUS_GET(l2)){l132 = soc_mem_alpm_delete(l2,l24,
SOC_ALPM_RPF_BKT_IDX(l2,l193),MEM_BLOCK_ALL,l134,l194,l15,&l139);if(!
SOC_SUCCESS(l132)){SOC_ALPM_LPM_UNLOCK(l2);l132 = SOC_E_FAIL;return l132;}}if
((l160->len!= 0)&&(trie->trie == NULL)){l132 = alpm_bucket_release(l2,
PIVOT_BUCKET_INDEX(l160),l34);if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to release""empty bucket: %d\n",PIVOT_BUCKET_INDEX(
l160));}trie_destroy(PIVOT_BUCKET_TRIE(l160));sal_free(PIVOT_BUCKET_HANDLE(
l160));sal_free(l160);_soc_trident2_alpm_bkt_view_set(l2,l193<<2,INVALIDm);if
(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){_soc_trident2_alpm_bkt_view_set(l2,(l193+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l2,l28,l34);if(VRF_TRIE_ROUTES_CNT(l2,
l28,l34) == 0){l132 = l33(l2,l28,l34);}SOC_ALPM_LPM_UNLOCK(l2);return l132;
l204:l166 = trie_insert(l168,prefix,NULL,l164,(trie_node_t*)l201);if(
SOC_FAILURE(l166)){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into pfx trie\n",prefix[0],prefix[1]);}l202:l166 = 
trie_insert(trie,prefix,NULL,l164,(trie_node_t*)l172);if(SOC_FAILURE(l166)){
soc_cm_debug(DK_ERR,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into bkt trie\n",prefix[0],prefix[1]);}
SOC_ALPM_LPM_UNLOCK(l2);return l132;}int soc_alpm_init(int l2){int l176;int
l132 = SOC_E_NONE;l132 = l3(l2);SOC_IF_ERROR_RETURN(l132);l132 = l19(l2);for(
l176 = 0;l176<MAX_PIVOT_COUNT;l176++){ALPM_TCAM_PIVOT(l2,l176) = NULL;}if(
SOC_CONTROL(l2)->alpm_bulk_retry == NULL){SOC_CONTROL(l2)->alpm_bulk_retry = 
sal_sem_create("ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l2)->
alpm_lookup_retry == NULL){SOC_CONTROL(l2)->alpm_lookup_retry = 
sal_sem_create("ALPM lookup retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l2)->
alpm_insert_retry == NULL){SOC_CONTROL(l2)->alpm_insert_retry = 
sal_sem_create("ALPM insert retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l2)->
alpm_delete_retry == NULL){SOC_CONTROL(l2)->alpm_delete_retry = 
sal_sem_create("ALPM delete retry",sal_sem_BINARY,0);}l132 = 
soc_alpm_128_lpm_init(l2);SOC_IF_ERROR_RETURN(l132);return l132;}static int
l205(int l2){int l176,l132;alpm_pivot_t*l128;for(l176 = 0;l176<
MAX_PIVOT_COUNT;l176++){l128 = ALPM_TCAM_PIVOT(l2,l176);if(l128){if(
PIVOT_BUCKET_HANDLE(l128)){if(PIVOT_BUCKET_TRIE(l128)){l132 = trie_traverse(
PIVOT_BUCKET_TRIE(l128),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l132)){trie_destroy(PIVOT_BUCKET_TRIE(l128));}else{soc_cm_debug(
DK_ERR,"Unable to clear trie state for ""unit %d\n",l2);return l132;}}
sal_free(PIVOT_BUCKET_HANDLE(l128));}sal_free(ALPM_TCAM_PIVOT(l2,l176));
ALPM_TCAM_PIVOT(l2,l176) = NULL;}}for(l176 = 0;l176<= SOC_VRF_MAX(l2)+1;l176
++){l132 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l2,l176),alpm_delete_node_cb,
NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l132)){trie_destroy(
VRF_PREFIX_TRIE_IPV4(l2,l176));}else{soc_cm_debug(DK_ERR,
"unit: %d Unable to clear v4 pfx trie for ""vrf %d\n",l2,l176);return l132;}
l132 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l2,l176),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l132)){trie_destroy(
VRF_PREFIX_TRIE_IPV6(l2,l176));}else{soc_cm_debug(DK_ERR,
"unit: %d Unable to clear v6 pfx trie for ""vrf %d\n",l2,l176);return l132;}
if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l176)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l176));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l176
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l176));}sal_memset(&
alpm_vrf_handle[l2][l176],0,sizeof(alpm_vrf_handle_t));}if(
SOC_ALPM_BUCKET_BMAP(l2)!= NULL){sal_free(SOC_ALPM_BUCKET_BMAP(l2));}
sal_memset(&soc_alpm_bucket[l2],0,sizeof(soc_alpm_bucket_t));return SOC_E_NONE
;}int soc_alpm_deinit(int l2){l4(l2);SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_deinit(l2));SOC_IF_ERROR_RETURN(soc_alpm_128_state_clear(l2)
);SOC_IF_ERROR_RETURN(l205(l2));if(SOC_CONTROL(l2)->alpm_bulk_retry){
sal_sem_destroy(SOC_CONTROL(l2)->alpm_bulk_retry);SOC_CONTROL(l2)->
alpm_bulk_retry = NULL;}if(SOC_CONTROL(l2)->alpm_lookup_retry == NULL){
sal_sem_destroy(SOC_CONTROL(l2)->alpm_lookup_retry);SOC_CONTROL(l2)->
alpm_lookup_retry = NULL;}if(SOC_CONTROL(l2)->alpm_insert_retry == NULL){
sal_sem_destroy(SOC_CONTROL(l2)->alpm_insert_retry);SOC_CONTROL(l2)->
alpm_insert_retry = NULL;}if(SOC_CONTROL(l2)->alpm_delete_retry == NULL){
sal_sem_destroy(SOC_CONTROL(l2)->alpm_delete_retry);SOC_CONTROL(l2)->
alpm_delete_retry = NULL;}return SOC_E_NONE;}static int l206(int l2,int l28,
int l34){defip_entry_t*lpm_entry,l207;int l208;int index;int l132 = 
SOC_E_NONE;uint32 key[2] = {0,0};uint32 l164;alpm_bucket_handle_t*l175;
alpm_pivot_t*l160;payload_t*l201;trie_t*l209;if(l34 == 0){trie_init(
_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l2,l28));l209 = VRF_PREFIX_TRIE_IPV4(
l2,l28);}else{trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l2,l28));l209
= VRF_PREFIX_TRIE_IPV6(l2,l28);}lpm_entry = sal_alloc(sizeof(defip_entry_t),
"Default LPM entry");if(lpm_entry == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n");return
SOC_E_MEMORY;}l30(l2,key,0,l28,l34,lpm_entry,0,1);if(l34 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28) = lpm_entry;}if(l28 == SOC_VRF_MAX(l2)+1)
{soc_L3_DEFIPm_field32_set(l2,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l2,lpm_entry,DEFAULT_MISS0f,1);}l132 = 
alpm_bucket_assign(l2,&l208,l34);soc_L3_DEFIPm_field32_set(l2,lpm_entry,
ALG_BKT_PTR0f,l208);sal_memcpy(&l207,lpm_entry,sizeof(l207));l132 = l5(l2,&
l207,&index);l175 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l175 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n");
return SOC_E_NONE;}sal_memset(l175,0,sizeof(*l175));l160 = sal_alloc(sizeof(
alpm_pivot_t),"Payload for Pivot");if(l160 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n");
sal_free(l175);return SOC_E_MEMORY;}l201 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l201 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n");
sal_free(l175);sal_free(l160);return SOC_E_MEMORY;}sal_memset(l160,0,sizeof(*
l160));sal_memset(l201,0,sizeof(*l201));l164 = 0;PIVOT_BUCKET_HANDLE(l160) = 
l175;if(l34){trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l160));}else{
trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l160));}PIVOT_BUCKET_INDEX(l160
) = l208;PIVOT_TCAM_INDEX(l160) = index;l160->key[0] = l201->key[0] = key[0];
l160->key[1] = l201->key[1] = key[1];l160->len = l201->len = l164;l132 = 
trie_insert(l209,key,NULL,l164,&(l201->node));if(SOC_FAILURE(l132)){sal_free(
l201);sal_free(l160);sal_free(l175);return l132;}index = 
soc_alpm_physical_idx(l2,L3_DEFIPm,index,l34);if(l34 == 0){ALPM_TCAM_PIVOT(l2
,index) = l160;}else{ALPM_TCAM_PIVOT(l2,index<<1) = l160;}VRF_TRIE_INIT_DONE(
l2,l28,l34,1);return l132;}static int l33(int l2,int l28,int l34){
defip_entry_t*lpm_entry;int l208;int l125;int l132 = SOC_E_NONE;uint32 key[2]
= {0,0},l210[SOC_MAX_MEM_FIELD_WORDS];payload_t*l172;alpm_pivot_t*l211;
trie_node_t*l170;trie_t*l209;if(l34 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);}else{lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28);}l208 = soc_L3_DEFIPm_field32_get(l2,
lpm_entry,ALG_BKT_PTR0f);l132 = alpm_bucket_release(l2,l208,l34);
_soc_trident2_alpm_bkt_view_set(l2,l208<<2,INVALIDm);l132 = l18(l2,lpm_entry,
(void*)l210,&l125);if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n",l28
,l34);l125 = -1;}l125 = soc_alpm_physical_idx(l2,L3_DEFIPm,l125,l34);if(l34 == 
0){l211 = ALPM_TCAM_PIVOT(l2,l125);}else{l211 = ALPM_TCAM_PIVOT(l2,l125<<1);}
l132 = l7(l2,lpm_entry);if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n",l28,l34);}sal_free(lpm_entry);if(l34
== 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28) = NULL;l209 = VRF_PREFIX_TRIE_IPV4(
l2,l28);VRF_PREFIX_TRIE_IPV4(l2,l28) = NULL;}else{VRF_TRIE_DEFAULT_ROUTE_IPV6
(l2,l28) = NULL;l209 = VRF_PREFIX_TRIE_IPV6(l2,l28);VRF_PREFIX_TRIE_IPV6(l2,
l28) = NULL;}VRF_TRIE_INIT_DONE(l2,l28,l34,0);l132 = trie_delete(l209,key,0,&
l170);l172 = (payload_t*)l170;if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"Unable to delete internal default for vrf "" %d/%d\n",l28,l34);}sal_free(
l172);(void)trie_destroy(l209);sal_free(PIVOT_BUCKET_HANDLE(l211));sal_free(
l211);return l132;}int soc_alpm_insert(int l2,void*l6,uint32 l25){
defip_alpm_ipv4_entry_t l178,l179;defip_alpm_ipv6_64_entry_t l180,l181;
soc_mem_t l24;void*l182,*l194;int l138,l28;int index;int l10;int l132 = 
SOC_E_NONE;uint32 l26;l10 = soc_mem_field32_get(l2,L3_DEFIPm,l6,MODE0f);l24 = 
(l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l182 = ((l10)?((uint32*)&(
l180)):((uint32*)&(l178)));l194 = ((l10)?((uint32*)&(l181)):((uint32*)&(l179)
));SOC_IF_ERROR_RETURN(l20(l2,l6,l182,l194,l24,l25,&l26));SOC_IF_ERROR_RETURN
(soc_alpm_lpm_vrf_get(l2,l6,&l138,&l28));if(l138 == SOC_L3_VRF_OVERRIDE){l132
= l5(l2,l6,&index);return(l132);}if(l138!= SOC_L3_VRF_GLOBAL){if(!
soc_alpm_mode_get(l2)){if(VRF_TRIE_ROUTES_CNT(l2,l28,l10) == 0){if(!l26){
soc_cm_debug(DK_ERR,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n",l138);return SOC_E_PARAM;}}}}if(!
VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){soc_cm_debug(DK_VERBOSE,
"soc_alpm_insert:VRF %d is not ""initialized\n",l28);l132 = l206(l2,l28,l10);
if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n",l28,l10);return l132;}
soc_cm_debug(DK_VERBOSE,"soc_alpm_insert:VRF %d/%d trie init ""completed\n",
l28,l10);}l132 = l140(l2,l6,l182,l194,l24,&index);if(l132 == SOC_E_NOT_FOUND)
{l132 = l159(l2,l6,l24,l182,l194,&index);}if(l132!= SOC_E_NONE){soc_cm_debug(
DK_WARN,"unit %d :soc_alpm_insert: ""Route Insertion Failed :%s\n",l2,
soc_errmsg(l132));}return(l132);}int soc_alpm_lookup(int l2,void*l8,void*l15,
int*l16){defip_alpm_ipv4_entry_t l178;defip_alpm_ipv6_64_entry_t l180;
soc_mem_t l24;int bucket_index;int l137;void*l182;int l138,l28;int l10,l117;
int l132 = SOC_E_NONE;l10 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l138,&l28));l132 = l14(l2,l8,
l15,l16,&l117,&l10);if(SOC_SUCCESS(l132)){if(!l10&&(*l16&0x1)){l132 = 
soc_alpm_lpm_ip4entry1_to_0(l2,l15,l15,PRESERVE_HIT);}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l2,l15,&l138,&l28));if(l138 == SOC_L3_VRF_OVERRIDE){
return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){soc_cm_debug(
DK_VERBOSE,"soc_alpm_lookup:VRF %d is not ""initialized\n",l28);return
SOC_E_NOT_FOUND;}l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l182 = 
((l10)?((uint32*)&(l180)):((uint32*)&(l178)));SOC_ALPM_LPM_LOCK(l2);l132 = 
l136(l2,l8,l24,l182,&l137,&bucket_index,l16);SOC_ALPM_LPM_UNLOCK(l2);if(
SOC_FAILURE(l132)){return l132;}l132 = l27(l2,l182,l24,l10,l138,bucket_index,
*l16,l15);return(l132);}int soc_alpm_delete(int l2,void*l8){int l138,l28;int
l10;int l132 = SOC_E_NONE;l10 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l138,&l28));if(l138 == 
SOC_L3_VRF_OVERRIDE){l132 = l7(l2,l8);return(l132);}else{if(!
VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){soc_cm_debug(DK_VERBOSE,
"soc_alpm_delete:VRF %d/%d is not ""initialized\n",l28,l10);return SOC_E_NONE
;}l132 = l195(l2,l8);}return(l132);}static int l19(int l2){int l212;l212 = 
soc_mem_index_count(l2,L3_DEFIPm)+soc_mem_index_count(l2,L3_DEFIP_PAIR_128m)*
2;if(SOC_URPF_STATUS_GET(l2)){l212>>= 1;}SOC_ALPM_BUCKET_COUNT(l2) = l212*2;
SOC_ALPM_BUCKET_BMAP_SIZE(l2) = SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(l2));
SOC_ALPM_BUCKET_BMAP(l2) = sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l2),
"alpm_shared_bucket_bitmap");if(SOC_ALPM_BUCKET_BMAP(l2) == NULL){
soc_cm_debug(DK_WARN,
"soc_alpm_shared_mem_init: Memory allocation for bucket bitmap management failed\n"
);return SOC_E_MEMORY;}sal_memset(SOC_ALPM_BUCKET_BMAP(l2),0,
SOC_ALPM_BUCKET_BMAP_SIZE(l2));alpm_bucket_assign(l2,&l212,1);return
SOC_E_NONE;}int alpm_bucket_assign(int l2,int*l208,int l34){int l176,l213 = 1
,l214 = 0;if(l34){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l213 = 
2;}}for(l176 = 0;l176<SOC_ALPM_BUCKET_COUNT(l2);l176+= l213){
SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l176,l213,l214);if(0 == l214){
break;}}if(l176 == SOC_ALPM_BUCKET_COUNT(l2)){return SOC_E_FULL;}
SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l176,l213);*l208 = l176;
SOC_ALPM_BUCKET_NEXT_FREE(l2) = l176;return SOC_E_NONE;}int
alpm_bucket_release(int l2,int l208,int l34){int l213 = 1,l214 = 0;if((l208<1
)||(l208>SOC_ALPM_BUCKET_MAX_INDEX(l2))){soc_cm_debug(DK_ERR,
"Unit %d\n, freeing invalid bucket index %d\n",l2,l208);return SOC_E_PARAM;}
if(l34){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l213 = 2;}}
SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l208,l213,l214);if(!l214){return
SOC_E_PARAM;}SHR_BITCLR_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l208,l213);return
SOC_E_NONE;}static void l105(int l2,void*l15,int index,l100 l106){if(index&(
0x8000)){l106[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l93[(l2)]->l59));l106[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l61));l106[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l93[(l2)]->l60));l106[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l62));if((!(SOC_IS_HURRICANE(l2)))&&(((l93[(l2)]->l75)!= NULL))){int
l215;(void)soc_alpm_lpm_vrf_get(l2,l15,(int*)&l106[4],&l215);}else{l106[4] = 
0;};}else{if(index&0x1){l106[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(l2)]->l60));l106[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l62));l106[2] = 0;l106[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&((
(l93[(l2)]->l76)!= NULL))){int l215;defip_entry_t l216;(void)
soc_alpm_lpm_ip4entry1_to_0(l2,l15,&l216,0);(void)soc_alpm_lpm_vrf_get(l2,&
l216,(int*)&l106[4],&l215);}else{l106[4] = 0;};}else{l106[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l59));l106[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l93[(l2)]->l61));l106[2] = 0;l106[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l2)))&&(((l93[(l2)]->l75)!= NULL))){int l215;(void)
soc_alpm_lpm_vrf_get(l2,l15,(int*)&l106[4],&l215);}else{l106[4] = 0;};}}}
static int l217(l100 l102,l100 l103){int l125;for(l125 = 0;l125<5;l125++){{if
((l102[l125])<(l103[l125])){return-1;}if((l102[l125])>(l103[l125])){return 1;
}};}return(0);}static void l218(int l2,void*l6,uint32 l219,uint32 l120,int
l117){l100 l220;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l93[(l2)]->l63))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l93[(l2)]->l74))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l93[(l2
)]->l73))){l220[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l93[(l2)]->l59));l220[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l93[(l2
)]->l61));l220[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l93[(l2)]->l60));l220[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l93[(l2
)]->l62));if((!(SOC_IS_HURRICANE(l2)))&&(((l93[(l2)]->l75)!= NULL))){int l215
;(void)soc_alpm_lpm_vrf_get(l2,l6,(int*)&l220[4],&l215);}else{l220[4] = 0;};
l119((l104[(l2)]),l217,l220,l117,l120,((uint16)l219<<1)|(0x8000));}}else{if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l93[(l2
)]->l73))){l220[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l93[(l2)]->l59));l220[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l93[(l2
)]->l61));l220[2] = 0;l220[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((
l93[(l2)]->l75)!= NULL))){int l215;(void)soc_alpm_lpm_vrf_get(l2,l6,(int*)&
l220[4],&l215);}else{l220[4] = 0;};l119((l104[(l2)]),l217,l220,l117,l120,((
uint16)l219<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l93[(l2)]->l74))){l220[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l93[(l2
)]->l60));l220[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l93[(l2)]->l62));l220[2] = 0;l220[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l2)))&&(((l93[(l2)]->l76)!= NULL))){int l215;defip_entry_t
l216;(void)soc_alpm_lpm_ip4entry1_to_0(l2,l6,&l216,0);(void)
soc_alpm_lpm_vrf_get(l2,&l216,(int*)&l220[4],&l215);}else{l220[4] = 0;};l119(
(l104[(l2)]),l217,l220,l117,l120,(((uint16)l219<<1)+1));}}}static void l221(int l2,
void*l8,uint32 l219){l100 l220;int l117 = -1;int l132;uint16 index;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2
)]->l63))){l220[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l93[(l2)]->l59));l220[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2
)]->l61));l220[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l93[(l2)]->l60));l220[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2
)]->l62));if((!(SOC_IS_HURRICANE(l2)))&&(((l93[(l2)]->l75)!= NULL))){int l215
;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&l220[4],&l215);}else{l220[4] = 0;};
index = (l219<<1)|(0x8000);}else{l220[0] = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2)]->l59));l220[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2
)]->l61));l220[2] = 0;l220[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((
l93[(l2)]->l75)!= NULL))){int l215;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&
l220[4],&l215);}else{l220[4] = 0;};index = l219;}l132 = l122((l104[(l2)]),
l217,l220,l117,index);if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"\ndel  index: H %d error %d\n",index,l132);}}int l222(int l2,void*l8,int l117
,int*l118){l100 l220;int l223;int l132;uint16 index = (0xFFFF);l223 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2
)]->l63));if(l223){l220[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l8),(l93[(l2)]->l59));l220[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2
)]->l61));l220[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l93[(l2)]->l60));l220[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2
)]->l62));if((!(SOC_IS_HURRICANE(l2)))&&(((l93[(l2)]->l75)!= NULL))){int l215
;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&l220[4],&l215);}else{l220[4] = 0;};}
else{l220[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)
),(l8),(l93[(l2)]->l59));l220[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2)]->l61));l220[2] = 0;l220[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((l93[(l2)]->l75)!= NULL))){int l215
;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&l220[4],&l215);}else{l220[4] = 0;};}
l132 = l114((l104[(l2)]),l217,l220,l117,&index);if(SOC_FAILURE(l132)){*l118 = 
0xFFFFFFFF;return(l132);}*l118 = index;return(SOC_E_NONE);}static uint16 l107
(uint8*l108,int l109){return(_shr_crc16b(0,l108,l109));}static int l110(int
l21,int l95,int l96,l99**l111){l99*l115;int index;if(l96>l95){return
SOC_E_MEMORY;}l115 = sal_alloc(sizeof(l99),"lpm_hash");if(l115 == NULL){
return SOC_E_MEMORY;}sal_memset(l115,0,sizeof(*l115));l115->l21 = l21;l115->
l95 = l95;l115->l96 = l96;l115->l97 = sal_alloc(l115->l96*sizeof(*(l115->l97)
),"hash_table");if(l115->l97 == NULL){sal_free(l115);return SOC_E_MEMORY;}
l115->l98 = sal_alloc(l115->l95*sizeof(*(l115->l98)),"link_table");if(l115->
l98 == NULL){sal_free(l115->l97);sal_free(l115);return SOC_E_MEMORY;}for(
index = 0;index<l115->l96;index++){l115->l97[index] = (0xFFFF);}for(index = 0
;index<l115->l95;index++){l115->l98[index] = (0xFFFF);}*l111 = l115;return
SOC_E_NONE;}static int l112(l99*l113){if(l113!= NULL){sal_free(l113->l97);
sal_free(l113->l98);sal_free(l113);}return SOC_E_NONE;}static int l114(l99*
l115,l101 l116,l100 entry,int l117,uint16*l118){int l2 = l115->l21;uint16 l224
;uint16 index;l224 = l107((uint8*)entry,(32*5))%l115->l96;index = l115->l97[
l224];;;while(index!= (0xFFFF)){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];l100 l106
;int l225;l225 = (index&(0x3FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,
MEM_BLOCK_ANY,l225,l15));l105(l2,l15,index,l106);if((*l116)(entry,l106) == 0)
{*l118 = (index&(0x3FFF))>>((index&(0x8000))?1:0);;return(SOC_E_NONE);}index = 
l115->l98[index&(0x3FFF)];;};return(SOC_E_NOT_FOUND);}static int l119(l99*
l115,l101 l116,l100 entry,int l117,uint16 l120,uint16 l121){int l2 = l115->
l21;uint16 l224;uint16 index;uint16 l226;l224 = l107((uint8*)entry,(32*5))%
l115->l96;index = l115->l97[l224];;;;l226 = (0xFFFF);if(l120!= (0xFFFF)){
while(index!= (0xFFFF)){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];l100 l106;int l225
;l225 = (index&(0x3FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,
MEM_BLOCK_ANY,l225,l15));l105(l2,l15,index,l106);if((*l116)(entry,l106) == 0)
{if(l121!= index){;if(l226 == (0xFFFF)){l115->l97[l224] = l121;l115->l98[l121
&(0x3FFF)] = l115->l98[index&(0x3FFF)];l115->l98[index&(0x3FFF)] = (0xFFFF);}
else{l115->l98[l226&(0x3FFF)] = l121;l115->l98[l121&(0x3FFF)] = l115->l98[
index&(0x3FFF)];l115->l98[index&(0x3FFF)] = (0xFFFF);}};return(SOC_E_NONE);}
l226 = index;index = l115->l98[index&(0x3FFF)];;}}l115->l98[l121&(0x3FFF)] = 
l115->l97[l224];l115->l97[l224] = l121;return(SOC_E_NONE);}static int l122(
l99*l115,l101 l116,l100 entry,int l117,uint16 l123){uint16 l224;uint16 index;
uint16 l226;l224 = l107((uint8*)entry,(32*5))%l115->l96;index = l115->l97[
l224];;;l226 = (0xFFFF);while(index!= (0xFFFF)){if(l123 == index){;if(l226 == 
(0xFFFF)){l115->l97[l224] = l115->l98[l123&(0x3FFF)];l115->l98[l123&(0x3FFF)]
= (0xFFFF);}else{l115->l98[l226&(0x3FFF)] = l115->l98[l123&(0x3FFF)];l115->
l98[l123&(0x3FFF)] = (0xFFFF);}return(SOC_E_NONE);}l226 = index;index = l115
->l98[index&(0x3FFF)];;}return(SOC_E_NOT_FOUND);}int _ipmask2pfx(uint32 l227,
int*l228){*l228 = 0;while(l227&(1<<31)){*l228+= 1;l227<<= 1;}return((l227)?
SOC_E_PARAM:SOC_E_NONE);}int soc_alpm_lpm_ip4entry0_to_0(int l2,void*l229,
void*l230,int l231){uint32 l232;l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l73));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l73),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l63));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l63),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l59),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l61));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l61),(l232));if(((l93[(l2)]->l49)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l49),(l232));}if(((l93[(l2)]->l51)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l51),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l53));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l53),(l232));}else{l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l67));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l67),(l232));}l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l229),(l93[(l2)]->l69));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l69),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l71));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l71),(l232));if(((l93[(l2)]->l75)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l75),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l77));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l77),(l232));}if(((l93[(l2)]->l47)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l47));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l47),(l232));}if(l231){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l57));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l57),(l232));}l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l79),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l81));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l81),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l83));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l83),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l85));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l85),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l87));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l87),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l89),(l232));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_1(int l2,void*l229,void*l230,int l231){uint32 l232;
l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229)
,(l93[(l2)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l230),(l93[(l2)]->l74),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l64));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l64),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l60));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l60),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l62));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l62),(l232));if(((l93[(l2)]->
l50)!= NULL)){l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l229),(l93[(l2)]->l50));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l50),(l232));}if(((l93[(l2)]->
l52)!= NULL)){l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l229),(l93[(l2)]->l52));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l52),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l54));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l54),(l232));}else{l232 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l68));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l68),(l232));}l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l229),(l93[(l2)]->l70));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l70),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l72));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l72),(l232));if(((l93[(l2)]->l76)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l76),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l78));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l78),(l232));}if(((l93[(l2)]->l48)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l48),(l232));}if(l231){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l58));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l58),(l232));}l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l80),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l82));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l82),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l84));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l84),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l86));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l86),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l88),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l90),(l232));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_1(int l2,void*l229,void*l230,int l231){uint32 l232;
l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229)
,(l93[(l2)]->l73));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l230),(l93[(l2)]->l74),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l63));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l64),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l59));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l60),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l61));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l62),(l232));if(!
SOC_IS_HURRICANE(l2)){l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l49));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l50),(l232));}if(((l93[(l2)]->
l51)!= NULL)){l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l229),(l93[(l2)]->l51));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l52),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l53));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l54),(l232));}else{l232 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l67));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l68),(l232));}l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l229),(l93[(l2)]->l69));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l70),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l71));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l72),(l232));if(((l93[(l2)]->l75)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l76),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l77));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l78),(l232));}if(((l93[(l2)]->l47)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l47));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l48),(l232));}if(l231){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l57));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l58),(l232));}l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l80),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l81));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l82),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l83));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l84),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l85));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l86),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l87));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l88),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l90),(l232));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_0(int l2,void*l229,void*l230,int l231){uint32 l232;
l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229)
,(l93[(l2)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l230),(l93[(l2)]->l73),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l64));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l63),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l60));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l59),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l62));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l61),(l232));if(!
SOC_IS_HURRICANE(l2)){l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l50));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l49),(l232));}if(((l93[(l2)]->
l52)!= NULL)){l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l229),(l93[(l2)]->l52));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l51),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l54));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l53),(l232));}else{l232 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l68));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l67),(l232));}l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l229),(l93[(l2)]->l70));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l69),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l72));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l71),(l232));if(((l93[(l2)]->l76)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l75),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l78));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l77),(l232));}if(((l93[(l2)]->l48)!= NULL)){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l48));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l47),(l232));}if(l231){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l58));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l57),(l232));}l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l79),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l82));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l81),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l84));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l83),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(l2)]->l86));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(
l2)]->l85),(l232));l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l229),(l93[(l2)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l230),(l93[(l2)]->l87),(l232));l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l229),(l93[(
l2)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l230),(l93[(l2)]->l89),(l232));return(SOC_E_NONE);}static int l233(int l2,
void*l15){return(SOC_E_NONE);}void l1(int l2){int l176;int l234;l234 = ((3*(
64+32+2+1))-1);if(!soc_cm_debug_check(DK_L3|DK_SOCMEM|DK_VERBOSE)){return;}
for(l176 = l234;l176>= 0;l176--){if((l176!= ((3*(64+32+2+1))-1))&&((l43[(l2)]
[(l176)].l36) == -1)){continue;}soc_cm_debug(DK_L3|DK_SOCMEM|DK_VERBOSE,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n",l176,(l43[
(l2)][(l176)].l38),(l43[(l2)][(l176)].next),(l43[(l2)][(l176)].l36),(l43[(l2)
][(l176)].l37),(l43[(l2)][(l176)].l39),(l43[(l2)][(l176)].l40));}
COMPILER_REFERENCE(l233);}static int l235(int l2,int index,uint32*l15){int
l236;int l10;uint32 l237;uint32 l238;int l239;if(!SOC_URPF_STATUS_GET(l2)){
return(SOC_E_NONE);}if(soc_feature(l2,soc_feature_l3_defip_hole)){l236 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l2)){l236 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1)+0x0400;}else{l236 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1);}l10 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l63));if(((l93[(l2)]->l47)!= NULL)){soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(l2)]->l47),(0));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l48),(0));}l237 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l93[(l2)]->l61));l238 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(l2)]->l62));if(!l10){if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l73))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l15),(l93[(l2)]->l71),((!l237)?1:0));}if(soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(l2)]->l74))){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l72),((!l238)?1:0));}}else{l239 = ((!l237)&&(!l238))?1:0;l237 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l73));l238 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l93[(l2)]->l74));if(l237&&l238){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l71),(l239));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l93[(l2)]->l72),(l239));}}return l156(l2,MEM_BLOCK_ANY,
index+l236,index,l15);}static int l240(int l2,int l241,int l242){uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,
l241,l15));l218(l2,l15,l242,0x4000,0);SOC_IF_ERROR_RETURN(l156(l2,
MEM_BLOCK_ANY,l242,l241,l15));SOC_IF_ERROR_RETURN(l235(l2,l242,l15));do{int
l243,l244;l243 = soc_alpm_physical_idx((l2),L3_DEFIPm,(l241),1);l244 = 
soc_alpm_physical_idx((l2),L3_DEFIPm,(l242),1);ALPM_TCAM_PIVOT(l2,l244<<1) = 
ALPM_TCAM_PIVOT(l2,l243<<1);ALPM_TCAM_PIVOT(l2,(l244<<1)+1) = ALPM_TCAM_PIVOT
(l2,(l243<<1)+1);ALPM_TCAM_PIVOT(l2,l243<<1) = NULL;ALPM_TCAM_PIVOT(l2,(l243
<<1)+1) = NULL;}while(0);return(SOC_E_NONE);}static int l245(int l2,int l117,
int l10){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l241;int l242;uint32 l246,
l247;l242 = (l43[(l2)][(l117)].l37)+1;if(!l10){l241 = (l43[(l2)][(l117)].l37)
;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l241,l15));l246 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l73));l247 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l93[(l2)]->l74));if((l246 == 0)||(l247 == 0)){l218(l2,l15,
l242,0x4000,0);SOC_IF_ERROR_RETURN(l156(l2,MEM_BLOCK_ANY,l242,l241,l15));
SOC_IF_ERROR_RETURN(l235(l2,l242,l15));do{int l248 = soc_alpm_physical_idx((
l2),L3_DEFIPm,(l241),1)<<1;if((l247)){l248++;}ALPM_TCAM_PIVOT((l2),
soc_alpm_physical_idx((l2),L3_DEFIPm,(l242),1)*2+(l248&1)) = ALPM_TCAM_PIVOT(
(l2),l248);ALPM_TCAM_PIVOT((l2),l248) = NULL;}while(0);l242--;}}l241 = (l43[(
l2)][(l117)].l36);if(l241!= l242){SOC_IF_ERROR_RETURN(l240(l2,l241,l242));}(
l43[(l2)][(l117)].l36)+= 1;(l43[(l2)][(l117)].l37)+= 1;return(SOC_E_NONE);}
static int l249(int l2,int l117,int l10){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];
int l241;int l242;int l250;uint32 l246,l247;l242 = (l43[(l2)][(l117)].l36)-1;
if((l43[(l2)][(l117)].l39) == 0){(l43[(l2)][(l117)].l36) = l242;(l43[(l2)][(
l117)].l37) = l242-1;return(SOC_E_NONE);}if((!l10)&&((l43[(l2)][(l117)].l37)
!= (l43[(l2)][(l117)].l36))){l241 = (l43[(l2)][(l117)].l37);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l241,l15));l246 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(
l2)]->l73));l247 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l93[(l2)]->l74));if((l246 == 0)||(l247 == 0)){l250 = l241-
1;SOC_IF_ERROR_RETURN(l240(l2,l250,l242));l218(l2,l15,l250,0x4000,0);
SOC_IF_ERROR_RETURN(l156(l2,MEM_BLOCK_ANY,l250,l241,l15));SOC_IF_ERROR_RETURN
(l235(l2,l250,l15));do{int l248 = soc_alpm_physical_idx((l2),L3_DEFIPm,(l241)
,1)<<1;if((l247)){l248++;}ALPM_TCAM_PIVOT((l2),soc_alpm_physical_idx((l2),
L3_DEFIPm,(l250),1)*2+(l248&1)) = ALPM_TCAM_PIVOT((l2),l248);ALPM_TCAM_PIVOT(
(l2),l248) = NULL;}while(0);}else{l218(l2,l15,l242,0x4000,0);
SOC_IF_ERROR_RETURN(l156(l2,MEM_BLOCK_ANY,l242,l241,l15));SOC_IF_ERROR_RETURN
(l235(l2,l242,l15));do{int l243,l244;l243 = soc_alpm_physical_idx((l2),
L3_DEFIPm,(l241),1);l244 = soc_alpm_physical_idx((l2),L3_DEFIPm,(l242),1);
ALPM_TCAM_PIVOT(l2,l244<<1) = ALPM_TCAM_PIVOT(l2,l243<<1);ALPM_TCAM_PIVOT(l2,
(l244<<1)+1) = ALPM_TCAM_PIVOT(l2,(l243<<1)+1);ALPM_TCAM_PIVOT(l2,l243<<1) = 
NULL;ALPM_TCAM_PIVOT(l2,(l243<<1)+1) = NULL;}while(0);}}else{l241 = (l43[(l2)
][(l117)].l37);SOC_IF_ERROR_RETURN(l240(l2,l241,l242));}(l43[(l2)][(l117)].
l36)-= 1;(l43[(l2)][(l117)].l37)-= 1;return(SOC_E_NONE);}static int l251(int
l2,int l117,int l10,void*l15,int*l252){int l253;int l254;int l255;int l256;
int l241;uint32 l246,l247;int l132;if((l43[(l2)][(l117)].l39) == 0){l256 = ((
3*(64+32+2+1))-1);if(soc_alpm_mode_get(l2) == SOC_ALPM_MODE_PARALLEL){if(l117
<= (((3*(64+32+2+1))/3)-1)){l256 = (((3*(64+32+2+1))/3)-1);}}while((l43[(l2)]
[(l256)].next)>l117){l256 = (l43[(l2)][(l256)].next);}l254 = (l43[(l2)][(l256
)].next);if(l254!= -1){(l43[(l2)][(l254)].l38) = l117;}(l43[(l2)][(l117)].
next) = (l43[(l2)][(l256)].next);(l43[(l2)][(l117)].l38) = l256;(l43[(l2)][(
l256)].next) = l117;(l43[(l2)][(l117)].l40) = ((l43[(l2)][(l256)].l40)+1)/2;(
l43[(l2)][(l256)].l40)-= (l43[(l2)][(l117)].l40);(l43[(l2)][(l117)].l36) = (
l43[(l2)][(l256)].l37)+(l43[(l2)][(l256)].l40)+1;(l43[(l2)][(l117)].l37) = (
l43[(l2)][(l117)].l36)-1;(l43[(l2)][(l117)].l39) = 0;}else if(!l10){l241 = (
l43[(l2)][(l117)].l36);if((l132 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l241,l15))<
0){return l132;}l246 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l93[(l2)]->l73));l247 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(l2)]->l74));if((l246 == 0)||(l247 == 
0)){*l252 = (l241<<1)+((l247 == 0)?1:0);return(SOC_E_NONE);}l241 = (l43[(l2)]
[(l117)].l37);if((l132 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l241,l15))<0){return
l132;}l246 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l15),(l93[(l2)]->l73));l247 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l93[(l2)]->l74));if((l246 == 0)||(l247 == 
0)){*l252 = (l241<<1)+((l247 == 0)?1:0);return(SOC_E_NONE);}}l255 = l117;
while((l43[(l2)][(l255)].l40) == 0){l255 = (l43[(l2)][(l255)].next);if(l255 == 
-1){l255 = l117;break;}}while((l43[(l2)][(l255)].l40) == 0){l255 = (l43[(l2)]
[(l255)].l38);if(l255 == -1){if((l43[(l2)][(l117)].l39) == 0){l253 = (l43[(l2
)][(l117)].l38);l254 = (l43[(l2)][(l117)].next);if(-1!= l253){(l43[(l2)][(
l253)].next) = l254;}if(-1!= l254){(l43[(l2)][(l254)].l38) = l253;}}return(
SOC_E_FULL);}}while(l255>l117){l254 = (l43[(l2)][(l255)].next);
SOC_IF_ERROR_RETURN(l249(l2,l254,l10));(l43[(l2)][(l255)].l40)-= 1;(l43[(l2)]
[(l254)].l40)+= 1;l255 = l254;}while(l255<l117){SOC_IF_ERROR_RETURN(l245(l2,
l255,l10));(l43[(l2)][(l255)].l40)-= 1;l253 = (l43[(l2)][(l255)].l38);(l43[(
l2)][(l253)].l40)+= 1;l255 = l253;}(l43[(l2)][(l117)].l39)+= 1;(l43[(l2)][(
l117)].l40)-= 1;(l43[(l2)][(l117)].l37)+= 1;*l252 = (l43[(l2)][(l117)].l37)<<
((l10)?0:1);sal_memcpy(l15,soc_mem_entry_null(l2,L3_DEFIPm),
soc_mem_entry_words(l2,L3_DEFIPm)*4);return(SOC_E_NONE);}static int l257(int
l2,int l117,int l10,void*l15,int l258){int l253;int l254;int l241;int l242;
uint32 l259[SOC_MAX_MEM_FIELD_WORDS];uint32 l260[SOC_MAX_MEM_FIELD_WORDS];
uint32 l261[SOC_MAX_MEM_FIELD_WORDS];void*l262;int l132;int l263,l146;l241 = 
(l43[(l2)][(l117)].l37);l242 = l258;if(!l10){l242>>= 1;if((l132 = 
READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l241,l259))<0){return l132;}if((l132 = 
READ_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,soc_alpm_physical_idx(l2,L3_DEFIPm,
l241,1),l260))<0){return l132;}if((l132 = READ_L3_DEFIP_AUX_TABLEm(l2,
MEM_BLOCK_ANY,soc_alpm_physical_idx(l2,L3_DEFIPm,l242,1),l261))<0){return l132
;}l262 = (l242 == l241)?l259:l15;if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l259),(l93[(l2)]->l74))){l146 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,l260,BPM_LENGTH1f);if(l258&1){l132
= soc_alpm_lpm_ip4entry1_to_1(l2,l259,l262,PRESERVE_HIT);soc_mem_field32_set(
l2,L3_DEFIP_AUX_TABLEm,l261,BPM_LENGTH1f,l146);}else{l132 = 
soc_alpm_lpm_ip4entry1_to_0(l2,l259,l262,PRESERVE_HIT);soc_mem_field32_set(l2
,L3_DEFIP_AUX_TABLEm,l261,BPM_LENGTH0f,l146);}l263 = (l241<<1)+1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l259),(l93[(
l2)]->l74),(0));}else{l146 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,l260,
BPM_LENGTH0f);if(l258&1){l132 = soc_alpm_lpm_ip4entry0_to_1(l2,l259,l262,
PRESERVE_HIT);soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,l261,BPM_LENGTH1f,
l146);}else{l132 = soc_alpm_lpm_ip4entry0_to_0(l2,l259,l262,PRESERVE_HIT);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,l261,BPM_LENGTH0f,l146);}l263 = 
l241<<1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l259
),(l93[(l2)]->l73),(0));(l43[(l2)][(l117)].l39)-= 1;(l43[(l2)][(l117)].l40)+= 
1;(l43[(l2)][(l117)].l37)-= 1;}l263 = soc_alpm_physical_idx(l2,L3_DEFIPm,l263
,0);l258 = soc_alpm_physical_idx(l2,L3_DEFIPm,l258,0);ALPM_TCAM_PIVOT(l2,l258
) = ALPM_TCAM_PIVOT(l2,l263);ALPM_TCAM_PIVOT(l2,l263) = NULL;if((l132 = 
WRITE_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ALL,soc_alpm_physical_idx(l2,L3_DEFIPm
,l242,1),l261))<0){return l132;}if(l242!= l241){l218(l2,l262,l242,0x4000,0);
if((l132 = l156(l2,MEM_BLOCK_ANY,l242,l242,l262))<0){return l132;}if((l132 = 
l235(l2,l242,l262))<0){return l132;}}l218(l2,l259,l241,0x4000,0);if((l132 = 
l156(l2,MEM_BLOCK_ANY,l241,l241,l259))<0){return l132;}if((l132 = l235(l2,
l241,l259))<0){return l132;}}else{(l43[(l2)][(l117)].l39)-= 1;(l43[(l2)][(
l117)].l40)+= 1;(l43[(l2)][(l117)].l37)-= 1;if(l242!= l241){if((l132 = 
READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l241,l259))<0){return l132;}l218(l2,l259,l242
,0x4000,0);if((l132 = l156(l2,MEM_BLOCK_ANY,l242,l241,l259))<0){return l132;}
if((l132 = l235(l2,l242,l259))<0){return l132;}}l258 = soc_alpm_physical_idx(
l2,L3_DEFIPm,l242,1);l263 = soc_alpm_physical_idx(l2,L3_DEFIPm,l241,1);
ALPM_TCAM_PIVOT(l2,l258<<1) = ALPM_TCAM_PIVOT(l2,l263<<1);ALPM_TCAM_PIVOT(l2,
l263<<1) = NULL;sal_memcpy(l259,soc_mem_entry_null(l2,L3_DEFIPm),
soc_mem_entry_words(l2,L3_DEFIPm)*4);l218(l2,l259,l241,0x4000,0);if((l132 = 
l156(l2,MEM_BLOCK_ANY,l241,l241,l259))<0){return l132;}if((l132 = l235(l2,
l241,l259))<0){return l132;}}if((l43[(l2)][(l117)].l39) == 0){l253 = (l43[(l2
)][(l117)].l38);assert(l253!= -1);l254 = (l43[(l2)][(l117)].next);(l43[(l2)][
(l253)].next) = l254;(l43[(l2)][(l253)].l40)+= (l43[(l2)][(l117)].l40);(l43[(
l2)][(l117)].l40) = 0;if(l254!= -1){(l43[(l2)][(l254)].l38) = l253;}(l43[(l2)
][(l117)].next) = -1;(l43[(l2)][(l117)].l38) = -1;(l43[(l2)][(l117)].l36) = -
1;(l43[(l2)][(l117)].l37) = -1;}return(l132);}int soc_alpm_lpm_vrf_get(int l21
,void*lpm_entry,int*l28,int*l264){int l138;if(((l93[(l21)]->l77)!= NULL)){
l138 = soc_L3_DEFIPm_field32_get(l21,lpm_entry,VRF_ID_0f);*l264 = l138;if(
soc_L3_DEFIPm_field32_get(l21,lpm_entry,VRF_ID_MASK0f)){*l28 = l138;}else if(
!soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry)
,(l93[(l21)]->l79))){*l28 = SOC_L3_VRF_GLOBAL;*l264 = SOC_VRF_MAX(l21)+1;}
else{*l28 = SOC_L3_VRF_OVERRIDE;}}else{*l28 = SOC_L3_VRF_DEFAULT;}return(
SOC_E_NONE);}static int l265(int l2,void*entry,int*l17){int l117;int l132;int
l10;uint32 l232;int l138;int l266;l10 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l93[(l2)]->l63));if(l10){l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l93[
(l2)]->l61));if((l132 = _ipmask2pfx(l232,&l117))<0){return(l132);}l232 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l93[
(l2)]->l62));if(l117){if(l232!= 0xffffffff){return(SOC_E_PARAM);}l117+= 32;}
else{if((l132 = _ipmask2pfx(l232,&l117))<0){return(l132);}}l117+= 33;}else{
l232 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry
),(l93[(l2)]->l61));if((l132 = _ipmask2pfx(l232,&l117))<0){return(l132);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,entry,&l138,&l132));l266 = 
soc_alpm_mode_get(l2);switch(l138){case SOC_L3_VRF_GLOBAL:if(l266 == 
SOC_ALPM_MODE_PARALLEL){*l17 = l117+((3*(64+32+2+1))/3);}else{*l17 = l117;}
break;case SOC_L3_VRF_OVERRIDE:*l17 = l117+2*((3*(64+32+2+1))/3);break;
default:if(l266 == SOC_ALPM_MODE_PARALLEL){*l17 = l117;}else{*l17 = l117+((3*
(64+32+2+1))/3);}break;}return(SOC_E_NONE);}static int l14(int l2,void*l8,
void*l15,int*l16,int*l17,int*l10){int l132;int l34;int l118;int l117 = 0;l34 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l93[(l2
)]->l63));if(l34){if(!(l34 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l8),(l93[(l2)]->l64)))){return(SOC_E_PARAM);}}*l10 = l34;
l265(l2,l8,&l117);*l17 = l117;if(l222(l2,l8,l117,&l118) == SOC_E_NONE){*l16 = 
l118;if((l132 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,(*l10)?*l16:(*l16>>1),l15))<0
){return l132;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}static int
l3(int l2){int l234;int l176;int l212;int l267;uint32 l124,l266;if(!
soc_feature(l2,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}if((l266 = 
soc_property_get(l2,spn_L3_ALPM_ENABLE,0))){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l2,&l124));soc_reg_field_set(l2,
L3_DEFIP_RPF_CONTROLr,&l124,LPM_MODEf,1);if(l266 == SOC_ALPM_MODE_PARALLEL){
soc_reg_field_set(l2,L3_DEFIP_RPF_CONTROLr,&l124,LOOKUP_MODEf,1);}else{
soc_reg_field_set(l2,L3_DEFIP_RPF_CONTROLr,&l124,LOOKUP_MODEf,0);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l2,l124));if(
SOC_URPF_STATUS_GET(l2)){l124 = 0;soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&
l124,URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l124,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l124,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l124,
URPF_LOOKUP_CAM7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l2,l124));
}l124 = 0;if(l266 == SOC_ALPM_MODE_PARALLEL){if(SOC_URPF_STATUS_GET(l2)){
soc_reg_field_set(l2,L3_DEFIP_ALPM_CFGr,&l124,TCAM2_SELf,1);soc_reg_field_set
(l2,L3_DEFIP_ALPM_CFGr,&l124,TCAM3_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM4_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM5_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM6_SELf,3);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM7_SELf,3);}else{soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM4_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM5_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM6_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l2)){
soc_reg_field_set(l2,L3_DEFIP_ALPM_CFGr,&l124,TCAM4_SELf,2);soc_reg_field_set
(l2,L3_DEFIP_ALPM_CFGr,&l124,TCAM5_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM6_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l124,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l2,l124));if(soc_property_get(l2,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l268 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l2,&l268));if(l266!= SOC_ALPM_MODE_PARALLEL){if(!
SOC_URPF_STATUS_GET(l2)){soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l268,
V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l268,
V6_KEY_SEL_CAM2_3f,0x1);}else{soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l268,
V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l268,
V6_KEY_SEL_CAM4_5f,0x1);}}else{soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l268,
V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l268,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l268,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l268,
V6_KEY_SEL_CAM6_7f,0x1);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l2,l268)
);}}l234 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l2);l267 = sizeof(l41)*(l234);if
((l43[(l2)]!= NULL)){if(soc_alpm_deinit(l2)<0){SOC_ALPM_LPM_UNLOCK(l2);return
SOC_E_INTERNAL;}}l93[l2] = sal_alloc(sizeof(l91),"lpm_field_state");if(NULL == 
l93[l2]){SOC_ALPM_LPM_UNLOCK(l2);return(SOC_E_MEMORY);}(l93[l2])->l45 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,CLASS_ID0f);(l93[l2])->l46 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,CLASS_ID1f);(l93[l2])->l47 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DST_DISCARD0f);(l93[l2])->l48 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DST_DISCARD1f);(l93[l2])->l49 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP0f);(l93[l2])->l50 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP1f);(l93[l2])->l51 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_COUNT0f);(l93[l2])->l52 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_COUNT1f);(l93[l2])->l53 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_PTR0f);(l93[l2])->l54 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_PTR1f);(l93[l2])->l55 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_ROUTE0f);(l93[l2])->l56 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_ROUTE1f);(l93[l2])->l57 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,HIT0f);(l93[l2])->l58 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,HIT1f);(l93[l2])->l59 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR0f);(l93[l2])->l60 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR1f);(l93[l2])->l61 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR_MASK0f);(l93[l2])->l62 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR_MASK1f);(l93[l2])->l63 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE0f);(l93[l2])->l64 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE1f);(l93[l2])->l65 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE_MASK0f);(l93[l2])->l66 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE_MASK1f);(l93[l2])->l67 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,NEXT_HOP_INDEX0f);(l93[l2])->l68 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,NEXT_HOP_INDEX1f);(l93[l2])->l69 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,PRI0f);(l93[l2])->l70 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,PRI1f);(l93[l2])->l71 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,RPE0f);(l93[l2])->l72 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,RPE1f);(l93[l2])->l73 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VALID0f);(l93[l2])->l74 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VALID1f);(l93[l2])->l75 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_0f);(l93[l2])->l76 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_1f);(l93[l2])->l77 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_MASK0f);(l93[l2])->l78 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_MASK1f);(l93[l2])->l79 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_HIGH0f);(l93[l2])->l80 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_HIGH1f);(l93[l2])->l81 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_HIT_IDX0f);(l93[l2])->l82 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_HIT_IDX1f);(l93[l2])->l83 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_BKT_PTR0f);(l93[l2])->l84 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_BKT_PTR1f);(l93[l2])->l85 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DEFAULT_MISS0f);(l93[l2])->l86 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DEFAULT_MISS1f);(l93[l2])->l87 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l93[l2])->
l88 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l93[l2
])->l89 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l93[l2]
)->l90 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(l43[(l2)
]) = sal_alloc(l267,"LPM prefix info");if(NULL == (l43[(l2)])){sal_free(l93[
l2]);l93[l2] = NULL;SOC_ALPM_LPM_UNLOCK(l2);return(SOC_E_MEMORY);}sal_memset(
(l43[(l2)]),0,l267);for(l176 = 0;l176<l234;l176++){(l43[(l2)][(l176)].l36) = 
-1;(l43[(l2)][(l176)].l37) = -1;(l43[(l2)][(l176)].l38) = -1;(l43[(l2)][(l176
)].next) = -1;(l43[(l2)][(l176)].l39) = 0;(l43[(l2)][(l176)].l40) = 0;}l212 = 
soc_mem_index_count(l2,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l2)){l212>>= 1;}if(
l266 == SOC_ALPM_MODE_PARALLEL){(l43[(l2)][(((3*(64+32+2+1))-1))].l37) = (
l212>>1)-1;(l43[(l2)][(((((3*(64+32+2+1))/3)-1)))].l40) = l212>>1;(l43[(l2)][
((((3*(64+32+2+1))-1)))].l40) = (l212-(l43[(l2)][(((((3*(64+32+2+1))/3)-1)))]
.l40));}else{(l43[(l2)][((((3*(64+32+2+1))-1)))].l40) = l212;}if((l104[(l2)])
!= NULL){if(l112((l104[(l2)]))<0){SOC_ALPM_LPM_UNLOCK(l2);return
SOC_E_INTERNAL;}(l104[(l2)]) = NULL;}if(l110(l2,l212*2,l212,&(l104[(l2)]))<0)
{SOC_ALPM_LPM_UNLOCK(l2);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l2);return(
SOC_E_NONE);}static int l4(int l2){if(!soc_feature(l2,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l2);if((l104[(l2)])!= NULL){l112((
l104[(l2)]));(l104[(l2)]) = NULL;}if((l43[(l2)]!= NULL)){sal_free(l93[l2]);
l93[l2] = NULL;sal_free((l43[(l2)]));(l43[(l2)]) = NULL;}SOC_ALPM_LPM_UNLOCK(
l2);return(SOC_E_NONE);}static int l5(int l2,void*l6,int*l269){int l117;int
index;int l10;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l132 = SOC_E_NONE;int
l270 = 0;sal_memcpy(l15,soc_mem_entry_null(l2,L3_DEFIPm),soc_mem_entry_words(
l2,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l2);l132 = l14(l2,l6,l15,&index,&l117,&l10
);if(l132 == SOC_E_NOT_FOUND){l132 = l251(l2,l117,l10,l15,&index);if(l132<0){
SOC_ALPM_LPM_UNLOCK(l2);return(l132);}}else{l270 = 1;}*l269 = index;if(l132 == 
SOC_E_NONE){if(!l10){if(index&1){l132 = soc_alpm_lpm_ip4entry0_to_1(l2,l6,l15
,PRESERVE_HIT);}else{l132 = soc_alpm_lpm_ip4entry0_to_0(l2,l6,l15,
PRESERVE_HIT);}if(l132<0){SOC_ALPM_LPM_UNLOCK(l2);return(l132);}l6 = (void*)
l15;index>>= 1;}l1(l2);soc_cm_debug(DK_L3|DK_SOCMEM,
"\nsoc_alpm_lpm_insert: %d %d\n",index,l117);if(!l270){l218(l2,l6,index,
0x4000,0);}l132 = l156(l2,MEM_BLOCK_ANY,index,index,l6);if(l132>= 0){l132 = 
l235(l2,index,l6);}}SOC_ALPM_LPM_UNLOCK(l2);return(l132);}static int l7(int l2
,void*l8){int l117;int index;int l10;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int
l132 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l2);l132 = l14(l2,l8,l15,&index,&l117,&
l10);if(l132 == SOC_E_NONE){soc_cm_debug(DK_L3|DK_SOCMEM,
"\nsoc_alpm_lpm_delete: %d %d\n",index,l117);l221(l2,l8,index);l132 = l257(l2
,l117,l10,l15,index);}l1(l2);SOC_ALPM_LPM_UNLOCK(l2);return(l132);}static int
l18(int l2,void*l8,void*l15,int*l16){int l117;int l132;int l10;
SOC_ALPM_LPM_LOCK(l2);l132 = l14(l2,l8,l15,l16,&l117,&l10);
SOC_ALPM_LPM_UNLOCK(l2);return(l132);}static int l9(int l21,void*l8,int l10,
int l11,int l12,defip_aux_scratch_entry_t*l13){uint32 l232;uint32 l271[4] = {
0,0,0,0};int l117 = 0;int l132 = SOC_E_NONE;l232 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,VALID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
VALIDf,l232);l232 = soc_mem_field32_get(l21,L3_DEFIPm,l8,MODE0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,MODEf,l232);l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ENTRY_TYPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ENTRY_TYPEf,0);l232 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,GLOBAL_ROUTE0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,GLOBAL_ROUTEf,l232);l232 = soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,ECMPf,l232);l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP_PTR0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ECMP_PTRf,l232);l232 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,NEXT_HOP_INDEX0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,NEXT_HOP_INDEXf,l232);l232 = soc_mem_field32_get(l21,L3_DEFIPm,l8,PRI0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,PRIf,l232);l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,RPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,RPEf,l232);l232 =soc_mem_field32_get(l21,L3_DEFIPm,
l8,VRF_ID_0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,VRFf,l232);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,DB_TYPEf,l11);l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,DST_DISCARD0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,DST_DISCARDf,l232);l232 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,CLASS_ID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
CLASS_IDf,l232);if(l10){l271[2] = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR0f);l271[3] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR1f);}else{
l271[0] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR0f);}soc_mem_field_set(
l21,L3_DEFIP_AUX_SCRATCHm,(uint32*)l13,IP_ADDRf,(uint32*)l271);if(l10){l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l132 = _ipmask2pfx(
l232,&l117))<0){return(l132);}l232 = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR_MASK1f);if(l117){if(l232!= 0xffffffff){return(SOC_E_PARAM);}l117+= 32
;}else{if((l132 = _ipmask2pfx(l232,&l117))<0){return(l132);}}}else{l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l132 = _ipmask2pfx(
l232,&l117))<0){return(l132);}}soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,IP_LENGTHf,l117);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
REPLACE_LENf,l12);return(SOC_E_NONE);}int _soc_alpm_aux_op(int l2,
_soc_aux_op_t l272,defip_aux_scratch_entry_t*l13,int*l139,int*l137,int*
bucket_index){uint32 l124,l273;int l274;soc_timeout_t l275;int l132 = 
SOC_E_NONE;int l276 = 0;SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_SCRATCHm(l2,
MEM_BLOCK_ANY,0,l13));SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l2,&l124));
if(soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l124,STARTf)){return SOC_E_BUSY;}
l277:l124 = 0;soc_timeout_init(&l275,50000,0);switch(l272){case
INSERT_PROPAGATE:l274 = 0;break;case DELETE_PROPAGATE:l274 = 1;break;case
PREFIX_LOOKUP:l274 = 2;break;case HITBIT_REPLACE:l274 = 3;break;default:
return SOC_E_PARAM;}soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l124,OPCODEf,
l274);soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l124,STARTf,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l2,l124));l274 = 0;do{
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l2,&l124));l274 = 
soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l124,DONEf);if(soc_timeout_check(&
l275)){if(l274 == 1){l132 = SOC_E_NONE;}else{soc_cm_debug(DK_WARN,
"unit %d : DEFIP AUX Operation timeout\n",l2);l132 = SOC_E_TIMEOUT;}break;}}
while(l274!= 1);if(SOC_SUCCESS(l132)){if(soc_reg_field_get(l2,
L3_DEFIP_AUX_CTRLr,l124,ERRORf)){soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&
l124,STARTf,0);soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l124,ERRORf,0);
soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l124,DONEf,0);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l2,l124));soc_cm_debug(DK_WARN,
"unit %d: DEFIP AUX Operation encountered ""parity error !!\n",l2);l276++;if(
SOC_CONTROL(l2)->alpm_bulk_retry){sal_sem_take(SOC_CONTROL(l2)->
alpm_bulk_retry,1000000);}if(l276<5){soc_cm_debug(DK_WARN,
"unit %d: Retry DEFIP AUX Operation..\n",l2);goto l277;}else{soc_cm_debug(
DK_ERR,"unit %d: Aborting DEFIP AUX Operation "
"due to un-correctable error !!\n",l2);return SOC_E_INTERNAL;}}if(l272 == 
PREFIX_LOOKUP){if(l139&&l137){*l139 = soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr
,l124,HITf);*l137 = soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l124,BKT_INDEXf);
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRL_1r(l2,&l273));*bucket_index = 
soc_reg_field_get(l2,L3_DEFIP_AUX_CTRL_1r,l273,BKT_PTRf);}}}soc_reg_field_set
(l2,L3_DEFIP_AUX_CTRLr,&l124,STARTf,0);soc_reg_field_set(l2,
L3_DEFIP_AUX_CTRLr,&l124,DONEf,0);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l2,l124));return l132;}static int l20(int l21,void*
lpm_entry,void*l22,void*l23,soc_mem_t l24,uint32 l25,uint32*l278){uint32 l232
;uint32 l271[4] = {0,0};int l117 = 0;int l132 = SOC_E_NONE;int l10;uint32 l26
= 0;l10 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,MODE0f);sal_memset(l22,
0,soc_mem_entry_bytes(l21,l24));l232 = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,HIT0f);soc_mem_field32_set(l21,l24,l22,HITf,l232);l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,VALID0f);soc_mem_field32_set(l21,
l24,l22,VALIDf,l232);l232 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
ECMP0f);soc_mem_field32_set(l21,l24,l22,ECMPf,l232);l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set(
l21,l24,l22,ECMP_PTRf,l232);l232 = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,NEXT_HOP_INDEX0f);soc_mem_field32_set(l21,l24,l22,NEXT_HOP_INDEXf,
l232);l232 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,PRI0f);
soc_mem_field32_set(l21,l24,l22,PRIf,l232);l232 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,RPE0f);soc_mem_field32_set(l21,l24,l22,RPEf,l232);l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(l21,l24,l22,DST_DISCARDf,l232);l232 = soc_mem_field32_get
(l21,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);soc_mem_field32_set(l21,l24,l22,
SRC_DISCARDf,l232);l232 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
CLASS_ID0f);soc_mem_field32_set(l21,l24,l22,CLASS_IDf,l232);l271[0] = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR0f);if(l10){l271[1] = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR1f);}soc_mem_field_set(l21
,l24,(uint32*)l22,KEYf,(uint32*)l271);if(l10){l232 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l132 = _ipmask2pfx(l232,&l117))<0){
return(l132);}l232 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f);if(l117){if(l232!= 0xffffffff){return(SOC_E_PARAM);}l117+= 32
;}else{if((l132 = _ipmask2pfx(l232,&l117))<0){return(l132);}}}else{l232 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l132 = 
_ipmask2pfx(l232,&l117))<0){return(l132);}}if((l117 == 0)&&(l271[0] == 0)&&(
l271[1] == 0)){l26 = 1;}if(l278!= NULL){*l278 = l26;}soc_mem_field32_set(l21,
l24,l22,LENGTHf,l117);if(l23 == NULL){return(SOC_E_NONE);}if(
SOC_URPF_STATUS_GET(l21)){sal_memset(l23,0,soc_mem_entry_bytes(l21,l24));
sal_memcpy(l23,l22,soc_mem_entry_bytes(l21,l24));soc_mem_field32_set(l21,l24,
l23,DST_DISCARDf,0);soc_mem_field32_set(l21,l24,l23,RPEf,0);
soc_mem_field32_set(l21,l24,l23,SRC_DISCARDf,l25&SOC_ALPM_RPF_SRC_DISCARD);
soc_mem_field32_set(l21,l24,l23,DEFAULTROUTEf,l26);}return(SOC_E_NONE);}
static int l27(int l21,void*l22,soc_mem_t l24,int l10,int l28,int l29,int
index,void*lpm_entry){uint32 l232;uint32 l271[4] = {0,0};uint32 l117 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_bytes(l21,L3_DEFIPm));l232 = 
soc_mem_field32_get(l21,l24,l22,HITf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,HIT0f,l232);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
HIT1f,l232);}l232 = soc_mem_field32_get(l21,l24,l22,VALIDf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VALID0f,l232);if(l10){
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VALID1f,l232);}l232 = 
soc_mem_field32_get(l21,l24,l22,ECMPf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,ECMP0f,l232);l232 = soc_mem_field32_get(l21,l24,l22,ECMP_PTRf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ECMP_PTR0f,l232);l232 = 
soc_mem_field32_get(l21,l24,l22,NEXT_HOP_INDEXf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f,l232);l232 = soc_mem_field32_get(l21,l24
,l22,PRIf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,PRI0f,l232);l232 = 
soc_mem_field32_get(l21,l24,l22,RPEf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,RPE0f,l232);l232 = soc_mem_field32_get(l21,l24,l22,DST_DISCARDf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,DST_DISCARD0f,l232);l232 = 
soc_mem_field32_get(l21,l24,l22,SRC_DISCARDf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,SRC_DISCARD0f,l232);l232 = soc_mem_field32_get(l21,l24,
l22,CLASS_IDf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,CLASS_ID0f,l232);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ALG_BKT_PTR0f,l29);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ALG_HIT_IDX0f,index);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE_MASK0f,3);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l10){
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(l21,
l24,l22,KEYf,l271);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR1f,l271[1]);}soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR0f,
l271[0]);l232 = soc_mem_field32_get(l21,l24,l22,LENGTHf);if(l10){if(l232>= 32
){l117 = 0xffffffff;soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l117);l117 = ~(((l232-32) == 32)?0:(0xffffffff)>>(l232-32));
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l117);}else{l117 = 
~(0xffffffff>>l232);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l117);}}else{assert(l232<= 32);l117 = ~(((l232) == 32)?0:(
0xffffffff)>>(l232));soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,l117);}if(l28 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,GLOBAL_HIGH0f,1);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else if(l28 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry
,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,0);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,l28);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(l21));}
return(SOC_E_NONE);}int soc_alpm_warmboot_pivot_add(int l21,int l10,void*
lpm_entry,int l279,int l280){int l132 = SOC_E_NONE;int l164;uint32 key[2] = {
0,0};alpm_pivot_t*l165 = NULL;alpm_bucket_handle_t*l175 = NULL;uint32 l192=0;
l279 = soc_alpm_physical_idx(l21,L3_DEFIPm,l279,l10);if(l10){l192 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l132 = 
_ipmask2pfx(l192,&l164))<0){return(l132);}l192 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f);if(l164){if(l192!= 0xffffffff){return(
SOC_E_PARAM);}l164+= 32;}else{if((l132 = _ipmask2pfx(l192,&l164))<0){return(
l132);}}}else{l192= soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR_MASK0f);
if((l132 = _ipmask2pfx(l192,&l164))<0){return(l132);}}l175 = sal_alloc(sizeof
(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l175 == NULL){soc_cm_debug(
DK_ERR,"Unable to allocate memory for ""PIVOT trie node \n");return SOC_E_NONE
;}sal_memset(l175,0,sizeof(*l175));l165 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l165 == NULL){soc_cm_debug(DK_ERR,
"Unable to allocate memory for ""PIVOT trie node \n");sal_free(l175);return
SOC_E_MEMORY;}sal_memset(l165,0,sizeof(*l165));PIVOT_BUCKET_HANDLE(l165) = 
l175;if(l10){trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l165));key[0] = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR0f);key[1] = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR1f);}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l165));key[0] = soc_L3_DEFIPm_field32_get
(l21,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l165) = l280;PIVOT_TCAM_INDEX(
l165) = l279;l165->key[0] = key[0];l165->key[1] = key[1];l165->len = l164;
ALPM_TCAM_PIVOT(l21,l279) = l165;return l132;}static int l281(int l21,int l10
,void*lpm_entry,void*l22,soc_mem_t l24,int l279,int l280,int l282){int l283;
int l28;int l132 = SOC_E_NONE;int l26 = 0;uint32 prefix[5] = {0,0,0,0,0};
uint32 l164;void*l284 = NULL;trie_t*l285 = NULL;trie_t*l168 = NULL;
trie_node_t*l170 = NULL;payload_t*l286 = NULL;payload_t*l173 = NULL;
alpm_pivot_t*l160 = NULL;if((NULL == lpm_entry)||(NULL == l22)){return
SOC_E_PARAM;}if(l10){if(!(l10 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l21,
lpm_entry,&l283,&l28));l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m
;l284 = sal_alloc(sizeof(defip_entry_t),"Temp lpm_entr");if(NULL == l284){
return SOC_E_MEMORY;}SOC_IF_ERROR_RETURN(l27(l21,l22,l24,l10,l283,l280,l279,
l284));l132 = l129(l21,l284,prefix,&l164,&l26);if(SOC_FAILURE(l132)){
soc_cm_debug(DK_ERR,"prefix create failed\n");return l132;}sal_free(l284);
l160 = ALPM_TCAM_PIVOT(l21,l279);l285 = PIVOT_BUCKET_TRIE(l160);l286 = 
sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == l286){soc_cm_debug(
DK_ERR,"Unable to allocate memory for ""trie node.\n");return SOC_E_MEMORY;}
l173 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == 
l173){soc_cm_debug(DK_ERR,"Unable to allocate memory for ""pfx trie node\n");
sal_free(l286);return SOC_E_MEMORY;}sal_memset(l286,0,sizeof(*l286));
sal_memset(l173,0,sizeof(*l173));l286->key[0] = prefix[0];l286->key[1] = 
prefix[1];l286->key[2] = prefix[2];l286->key[3] = prefix[3];l286->key[4] = 
prefix[4];l286->len = l164;l286->index = l282;sal_memcpy(l173,l286,sizeof(*
l286));l132 = trie_insert(l285,prefix,NULL,l164,(trie_node_t*)l286);if(
SOC_FAILURE(l132)){goto l287;}if(l10){l168 = VRF_PREFIX_TRIE_IPV6(l21,l28);}
else{l168 = VRF_PREFIX_TRIE_IPV4(l21,l28);}if(!l26){l132 = trie_insert(l168,
prefix,NULL,l164,(trie_node_t*)l173);if(SOC_FAILURE(l132)){goto l185;}}return
l132;l185:(void)trie_delete(l285,prefix,l164,&l170);l286 = (payload_t*)l170;
l287:sal_free(l286);sal_free(l173);return l132;}static int l288(int l21,int
l34,int l28,int l125,int l289){int l132 = SOC_E_NONE;uint32 l164;uint32 key[2
] = {0,0};trie_t*l290 = NULL;payload_t*l201 = NULL;defip_entry_t*lpm_entry = 
NULL;lpm_entry = sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(
lpm_entry == NULL){soc_cm_debug(DK_ERR,"unable to allocate memory for "
"LPM entry\n");return SOC_E_MEMORY;}l30(l21,key,0,l28,l34,lpm_entry,0,1);if(
l28 == SOC_VRF_MAX(l21)+1){soc_L3_DEFIPm_field32_set(l21,lpm_entry,
GLOBAL_ROUTE0f,1);}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,
DEFAULT_MISS0f,1);}soc_L3_DEFIPm_field32_set(l21,lpm_entry,ALG_BKT_PTR0f,l289
);if(l34 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l21,l28) = lpm_entry;trie_init(
_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l21,l28));l290 = VRF_PREFIX_TRIE_IPV4(
l21,l28);}else{VRF_TRIE_DEFAULT_ROUTE_IPV6(l21,l28) = lpm_entry;trie_init(
_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l21,l28));l290 = VRF_PREFIX_TRIE_IPV6
(l21,l28);}l201 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(
l201 == NULL){soc_cm_debug(DK_ERR,"Unable to allocate memory for "
"pfx trie node \n");return SOC_E_MEMORY;}sal_memset(l201,0,sizeof(*l201));
l164 = 0;l201->key[0] = key[0];l201->key[1] = key[1];l201->len = l164;l132 = 
trie_insert(l290,key,NULL,l164,&(l201->node));if(SOC_FAILURE(l132)){sal_free(
l201);return l132;}VRF_TRIE_INIT_DONE(l21,l28,l34,1);return l132;}int
soc_alpm_warmboot_prefix_insert(int l21,int l10,void*lpm_entry,void*l22,int
l279,int l280,int l282){int l283;int l28;int l132 = SOC_E_NONE;soc_mem_t l24;
l279 = soc_alpm_physical_idx(l21,L3_DEFIPm,l279,l10);l24 = (l10)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l21,lpm_entry,&l283,&l28));if(l283 == 
SOC_L3_VRF_OVERRIDE){return(l132);}if(!VRF_TRIE_INIT_COMPLETED(l21,l28,l10)){
soc_cm_debug(DK_VERBOSE,"VRF %d is not ""initialized\n",l28);l132 = l288(l21,
l10,l28,l279,l280);if(SOC_FAILURE(l132)){soc_cm_debug(DK_ERR,
"VRF %d/%d trie init \n""failed\n",l28,l10);return l132;}soc_cm_debug(
DK_VERBOSE,"VRF %d/%d trie init ""completed\n",l28,l10);}l132 = l281(l21,l10,
lpm_entry,l22,l24,l279,l280,l282);if(l132!= SOC_E_NONE){soc_cm_debug(DK_WARN,
"unit %d : ""Route Insertion Failed :%s\n",l21,soc_errmsg(l132));return(l132)
;}VRF_TRIE_ROUTES_INC(l21,l28,l10);return(l132);}int
soc_alpm_warmboot_bucket_bitmap_set(int l2,int l34,int l291){int l213 = 1;if(
l34){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l213 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l2) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l2),l291,l213);return SOC_E_NONE;}int
soc_alpm_warmboot_lpm_reinit_done(int l21){int l125;int l292 = ((3*(64+32+2+1
))-1);int l212 = soc_mem_index_count(l21,L3_DEFIPm);if(SOC_URPF_STATUS_GET(
l21)){l212>>= 1;}if(!soc_alpm_mode_get(l21)){(l43[(l21)][(((3*(64+32+2+1))-1)
)].l38) = -1;for(l125 = ((3*(64+32+2+1))-1);l125>-1;l125--){if(-1 == (l43[(
l21)][(l125)].l36)){continue;}(l43[(l21)][(l125)].l38) = l292;(l43[(l21)][(
l292)].next) = l125;(l43[(l21)][(l292)].l40) = (l43[(l21)][(l125)].l36)-(l43[
(l21)][(l292)].l37)-1;l292 = l125;}(l43[(l21)][(l292)].next) = -1;(l43[(l21)]
[(l292)].l40) = l212-(l43[(l21)][(l292)].l37)-1;}else{(l43[(l21)][(((3*(64+32
+2+1))-1))].l38) = -1;for(l125 = ((3*(64+32+2+1))-1);l125>(((3*(64+32+2+1))-1
)/3);l125--){if(-1 == (l43[(l21)][(l125)].l36)){continue;}(l43[(l21)][(l125)]
.l38) = l292;(l43[(l21)][(l292)].next) = l125;(l43[(l21)][(l292)].l40) = (l43
[(l21)][(l125)].l36)-(l43[(l21)][(l292)].l37)-1;l292 = l125;}(l43[(l21)][(
l292)].next) = -1;(l43[(l21)][(l292)].l40) = l212-(l43[(l21)][(l292)].l37)-1;
l292 = (((3*(64+32+2+1))-1)/3);(l43[(l21)][((((3*(64+32+2+1))-1)/3))].l38) = 
-1;for(l125 = ((((3*(64+32+2+1))-1)/3)-1);l125>-1;l125--){if(-1 == (l43[(l21)
][(l125)].l36)){continue;}(l43[(l21)][(l125)].l38) = l292;(l43[(l21)][(l292)]
.next) = l125;(l43[(l21)][(l292)].l40) = (l43[(l21)][(l125)].l36)-(l43[(l21)]
[(l292)].l37)-1;l292 = l125;}(l43[(l21)][(l292)].next) = -1;(l43[(l21)][(l292
)].l40) = (l212>>1)-(l43[(l21)][(l292)].l37)-1;}return(SOC_E_NONE);}int
soc_alpm_warmboot_lpm_reinit(int l21,int l10,int l125,void*lpm_entry){int l17
;defip_entry_t*l293;if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l218(l21,lpm_entry,l125,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l265(l21,lpm_entry,&l17));if((l43[(l21)][(l17)].l39) == 0
){(l43[(l21)][(l17)].l36) = l125;(l43[(l21)][(l17)].l37) = l125;}else{(l43[(
l21)][(l17)].l37) = l125;}(l43[(l21)][(l17)].l39)++;if(l10){return(SOC_E_NONE
);}}else{if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l293 = 
sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");soc_alpm_lpm_ip4entry1_to_0(
l21,lpm_entry,l293,TRUE);SOC_IF_ERROR_RETURN(l265(l21,l293,&l17));if((l43[(
l21)][(l17)].l39) == 0){(l43[(l21)][(l17)].l36) = l125;(l43[(l21)][(l17)].l37
) = l125;}else{(l43[(l21)][(l17)].l37) = l125;}sal_free(l293);(l43[(l21)][(
l17)].l39)++;}}return(SOC_E_NONE);}
#endif /* ALPM_ENABLE */
