/*
 * $Id: 02d0dfc21bf397bab8b98d93b8539f98b5707d8a $
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
l35,int*l36);typedef struct l37{int l38;int l39;int l40;int next;int l41;int
l42;}l43,*l44;static l44 l45[SOC_MAX_NUM_DEVICES];typedef struct l46{
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
soc_field_info_t*l92;}l93,*l94;static l94 l95[SOC_MAX_NUM_DEVICES];typedef
struct l96{int l21;int l97;int l98;uint16*l99;uint16*l100;}l101;typedef uint32
l102[5];typedef int(*l103)(l102 l104,l102 l105);static l101*l106[
SOC_MAX_NUM_DEVICES];static void l107(int l2,void*l15,int index,l102 l108);
static uint16 l109(uint8*l110,int l111);static int l112(int l21,int l97,int
l98,l101**l113);static int l114(l101*l115);static int l116(l101*l117,l103 l118
,l102 entry,int l119,uint16*l120);static int l121(l101*l117,l103 l118,l102
entry,int l119,uint16 l122,uint16 l123);static int l124(l101*l117,l103 l118,
l102 entry,int l119,uint16 l125);alpm_vrf_handle_t alpm_vrf_handle[
SOC_MAX_NUM_DEVICES][MAX_VRF_ID];alpm_pivot_t*tcam_pivot[SOC_MAX_NUM_DEVICES]
[MAX_PIVOT_COUNT];int soc_alpm_mode_get(int l2){if(soc_trident2_alpm_mode_get
(l2) == 1){return 1;}else{return 0;}}int _soc_alpm_rpf_entry(int l2,int l126)
{int l127;l127 = (l126>>2)&0x3fff;l127+= SOC_ALPM_BUCKET_COUNT(l2);return(
l126&~(0x3fff<<2))|(l127<<2);}int soc_alpm_physical_idx(int l2,soc_mem_t l24,
int index,int l128){int l129 = index&1;if(l128){return
soc_trident2_l3_defip_index_map(l2,l24,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_map(l2,l24,index);index<<= 1;index|= l129;return
index;}int soc_alpm_logical_idx(int l2,soc_mem_t l24,int index,int l128){int
l129 = index&1;if(l128){return soc_trident2_l3_defip_index_remap(l2,l24,index
);}index>>= 1;index = soc_trident2_l3_defip_index_remap(l2,l24,index);index
<<= 1;index|= l129;return index;}static int l130(int l2,void*entry,uint32*
prefix,uint32*l17,int*l26){int l131,l132,l10;int l119 = 0;int l133 = 
SOC_E_NONE;uint32 l134,l129;prefix[0] = prefix[1] = prefix[2] = prefix[3] = 
prefix[4] = 0;l10 = soc_mem_field32_get(l2,L3_DEFIPm,entry,MODE0f);l131 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR0f);l132 = soc_mem_field32_get(
l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);prefix[1] = l131;l131 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR1f);l132 = soc_mem_field32_get(
l2,L3_DEFIPm,entry,IP_ADDR_MASK1f);prefix[0] = l131;if(l10){prefix[4] = 
prefix[1];prefix[3] = prefix[0];prefix[1] = prefix[0] = 0;l132 = 
soc_mem_field32_get(l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx
(l132,&l119))<0){return(l133);}l132 = soc_mem_field32_get(l2,L3_DEFIPm,entry,
IP_ADDR_MASK1f);if(l119){if(l132!= 0xffffffff){return(SOC_E_PARAM);}l119+= 32
;}else{if((l133 = _ipmask2pfx(l132,&l119))<0){return(l133);}}l134 = 64-l119;
if(l134<32){prefix[4]>>= l134;l129 = (((32-l134) == 32)?0:(prefix[3])<<(32-
l134));prefix[3]>>= l134;prefix[4]|= l129;}else{prefix[4] = (((l134-32) == 32
)?0:(prefix[3])>>(l134-32));prefix[3] = 0;}}else{l132 = soc_mem_field32_get(
l2,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(l132,&l119))<0){
return(l133);}prefix[1] = (((32-l119) == 32)?0:(prefix[1])>>(32-l119));prefix
[0] = 0;}*l17 = l119;*l26 = (prefix[0] == 0)&&(prefix[1] == 0)&&(l119 == 0);
return SOC_E_NONE;}int _soc_alpm_find_in_bkt(int l2,soc_mem_t l24,int
bucket_index,int l135,uint32*l15,void*l136,int*l120,int l34){int l133;l133 = 
soc_mem_alpm_lookup(l2,l24,bucket_index,MEM_BLOCK_ANY,l135,l15,l136,l120);if(
SOC_SUCCESS(l133)){return l133;}if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_lookup(l2,l24,bucket_index+1,MEM_BLOCK_ANY,l135,l15,l136,l120);}
return l133;}static int l137(int l2,void*l8,soc_mem_t l24,void*l136,int*l138,
int*bucket_index,int*l16){defip_aux_scratch_entry_t l13;uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];int l139,l28,l34;int l120;uint32 l11,l135;int l133 = 
SOC_E_NONE;int l140 = 0;l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(
l34){if(!(l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l139,&l28));if
(l139 == 0){if(soc_alpm_mode_get(l2)){return SOC_E_PARAM;}}if(l28 == 
SOC_VRF_MAX(l2)+1){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l135);}else{
l11 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l135);}if(l139!= SOC_L3_VRF_OVERRIDE
){sal_memset(&l13,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9
(l2,l8,l34,l11,0,&l13));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,PREFIX_LOOKUP
,&l13,&l140,l138,bucket_index));if(l140){l20(l2,l8,l15,0,l24,0,0);l133 = 
_soc_alpm_find_in_bkt(l2,l24,*bucket_index,l135,l15,l136,&l120,l34);if(
SOC_SUCCESS(l133)){*l16 = l120;}}else{l133 = SOC_E_NOT_FOUND;}}return l133;}
static int l141(int l2,void*l8,void*l136,void*l142,soc_mem_t l24,int l120){
defip_aux_scratch_entry_t l13;int l139,l34,l28;int bucket_index;uint32 l11,
l135;int l133 = SOC_E_NONE;int l140 = 0,l129 = 0;int l138;l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(l34 = 
soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l139,&l28));if(l28 == 
SOC_VRF_MAX(l2)+1){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l135);}else{
l11 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l135);}if(!soc_alpm_mode_get(l2)){
l11 = 2;}if(l139!= SOC_L3_VRF_OVERRIDE){sal_memset(&l13,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(l2,l8,l34,l11,0,&l13));
SOC_IF_ERROR_RETURN(soc_mem_write(l2,l24,MEM_BLOCK_ANY,l120,l136));if(
SOC_URPF_STATUS_GET(l2)){SOC_IF_ERROR_RETURN(soc_mem_write(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l120),l142));if(l133!= SOC_E_NONE){
return SOC_E_FAIL;}}l129 = soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,
IP_LENGTHf);soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,REPLACE_LENf,
l129);SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l140,&
l138,&bucket_index));if(SOC_URPF_STATUS_GET(l2)){l129 = soc_mem_field32_get(
l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l129+= 1;soc_mem_field32_set(l2,
L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l129);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l140,&l138,&bucket_index));}}
return l133;}int alpm_mem_prefix_array_cb(trie_node_t*node,void*l143){
alpm_mem_prefix_array_t*l144 = (alpm_mem_prefix_array_t*)l143;if(node->type == 
PAYLOAD){l144->prefix[l144->count] = (payload_t*)node;l144->count++;}return
SOC_E_NONE;}int alpm_delete_node_cb(trie_node_t*node,void*l143){if(node!= 
NULL){sal_free(node);}return SOC_E_NONE;}static int l145(int l2,int l146,int
l34,int l147){int l133,l129,index;defip_aux_table_entry_t entry;index = l146
>>(l34?0:1);l133 = soc_mem_read(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&
entry);SOC_IF_ERROR_RETURN(l133);if(l34){soc_mem_field32_set(l2,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l147);soc_mem_field32_set(l2,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l147);l129 = soc_mem_field32_get(l2,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l146&1){soc_mem_field32_set(l2
,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l147);l129 = soc_mem_field32_get(l2,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{soc_mem_field32_set(l2,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l147);l129 = soc_mem_field32_get(l2,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l133 = soc_mem_write(l2,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l133);if(
SOC_URPF_STATUS_GET(l2)){l129++;if(l34){soc_mem_field32_set(l2,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l147);soc_mem_field32_set(l2,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l147);}else{if(l146&1){
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l147);}else{
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l147);}}
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l129);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l129);index+= (2*
soc_mem_index_count(l2,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l2,L3_DEFIPm))
/2;l133 = soc_mem_write(l2,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}
return l133;}static int l148(int l2,int l149,void*entry,void*l150,int l151){
uint32 l129,l132,l34,l11,l152 = 0;soc_mem_t l24 = L3_DEFIPm;soc_mem_t l153 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l154;int l133 = SOC_E_NONE,l119,l155,l28,
l156;SOC_IF_ERROR_RETURN(soc_mem_read(l2,l153,MEM_BLOCK_ANY,l149,l150));l129 = 
soc_mem_field32_get(l2,l24,entry,VRF_ID_0f);soc_mem_field32_set(l2,l153,l150,
VRF0f,l129);l129 = soc_mem_field32_get(l2,l24,entry,VRF_ID_1f);
soc_mem_field32_set(l2,l153,l150,VRF1f,l129);l129 = soc_mem_field32_get(l2,
l24,entry,MODE0f);soc_mem_field32_set(l2,l153,l150,MODE0f,l129);l129 = 
soc_mem_field32_get(l2,l24,entry,MODE1f);soc_mem_field32_set(l2,l153,l150,
MODE1f,l129);l34 = l129;l129 = soc_mem_field32_get(l2,l24,entry,VALID0f);
soc_mem_field32_set(l2,l153,l150,VALID0f,l129);l129 = soc_mem_field32_get(l2,
l24,entry,VALID1f);soc_mem_field32_set(l2,l153,l150,VALID1f,l129);l129 = 
soc_mem_field32_get(l2,l24,entry,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(l129,
&l119))<0){return l133;}l132 = soc_mem_field32_get(l2,l24,entry,
IP_ADDR_MASK1f);if((l133 = _ipmask2pfx(l132,&l155))<0){return l133;}if(l34){
soc_mem_field32_set(l2,l153,l150,IP_LENGTH0f,l119+l155);soc_mem_field32_set(
l2,l153,l150,IP_LENGTH1f,l119+l155);}else{soc_mem_field32_set(l2,l153,l150,
IP_LENGTH0f,l119);soc_mem_field32_set(l2,l153,l150,IP_LENGTH1f,l155);}l129 = 
soc_mem_field32_get(l2,l24,entry,IP_ADDR0f);soc_mem_field32_set(l2,l153,l150,
IP_ADDR0f,l129);l129 = soc_mem_field32_get(l2,l24,entry,IP_ADDR1f);
soc_mem_field32_set(l2,l153,l150,IP_ADDR1f,l129);l129 = soc_mem_field32_get(
l2,l24,entry,ENTRY_TYPE0f);soc_mem_field32_set(l2,l153,l150,ENTRY_TYPE0f,l129
);l129 = soc_mem_field32_get(l2,l24,entry,ENTRY_TYPE1f);soc_mem_field32_set(
l2,l153,l150,ENTRY_TYPE1f,l129);if(!l34){sal_memcpy(&l154,entry,sizeof(l154))
;l133 = soc_alpm_lpm_vrf_get(l2,(void*)&l154,&l28,&l119);SOC_IF_ERROR_RETURN(
l133);SOC_IF_ERROR_RETURN(soc_alpm_lpm_ip4entry1_to_0(l2,&l154,&l154,
PRESERVE_HIT));l133 = soc_alpm_lpm_vrf_get(l2,(void*)&l154,&l156,&l119);
SOC_IF_ERROR_RETURN(l133);}else{l133 = soc_alpm_lpm_vrf_get(l2,entry,&l28,&
l119);}if(SOC_URPF_STATUS_GET(l2)){if(l151>= (soc_mem_index_count(l2,
L3_DEFIPm)>>1)){l152 = 1;}}switch(l28){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l2,l153,l150,VALID0f,0);l11 = 0;break;case
SOC_L3_VRF_GLOBAL:l11 = l152?1:0;break;default:l11 = l152?3:2;break;}
soc_mem_field32_set(l2,l153,l150,DB_TYPE0f,l11);if(!l34){switch(l156){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l2,l153,l150,VALID1f,0);l11 = 0;break
;case SOC_L3_VRF_GLOBAL:l11 = l152?1:0;break;default:l11 = l152?3:2;break;}
soc_mem_field32_set(l2,l153,l150,DB_TYPE1f,l11);}else{if(l28 == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l2,l153,l150,VALID1f,0);}
soc_mem_field32_set(l2,l153,l150,DB_TYPE1f,l11);}if(l152){l129 = 
soc_mem_field32_get(l2,l24,entry,ALG_BKT_PTR0f);if(l129){l129+= 
SOC_ALPM_BUCKET_COUNT(l2);soc_mem_field32_set(l2,l24,entry,ALG_BKT_PTR0f,l129
);}if(!l34){l129 = soc_mem_field32_get(l2,l24,entry,ALG_BKT_PTR1f);if(l129){
l129+= SOC_ALPM_BUCKET_COUNT(l2);soc_mem_field32_set(l2,l24,entry,
ALG_BKT_PTR1f,l129);}}}return SOC_E_NONE;}static int l157(int l2,int l158,int
index,int l159,void*entry){defip_aux_table_entry_t l150;l159 = 
soc_alpm_physical_idx(l2,L3_DEFIPm,l159,1);SOC_IF_ERROR_RETURN(l148(l2,l159,
entry,(void*)&l150,index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l2,
MEM_BLOCK_ANY,index,entry));index = soc_alpm_physical_idx(l2,L3_DEFIPm,index,
1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,index,&l150
));return SOC_E_NONE;}int _soc_alpm_insert_in_bkt(int l2,soc_mem_t l24,int
bucket_index,int l135,void*l136,uint32*l15,int*l120,int l34){int l133;l133 = 
soc_mem_alpm_insert(l2,l24,bucket_index,MEM_BLOCK_ANY,l135,l136,l15,l120);if(
l133 == SOC_E_FULL){if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_insert(l2,l24,bucket_index+1,MEM_BLOCK_ANY,l135,l136,l15,l120);}
}return l133;}static int l160(int l2,soc_mem_t l24,int l34,
alpm_mem_prefix_array_t*l144,int*l123){int l161,l133 = SOC_E_NONE,l162;
defip_alpm_ipv4_entry_t l163,l164;defip_alpm_ipv6_64_entry_t l165,l166;void*
l167 = NULL,*l168 = NULL;int l169,l170;int*l171 = NULL;int l172 = FALSE;if(
l34){l169 = sizeof(l165);l170 = sizeof(l166);}else{l169 = sizeof(l163);l170 = 
sizeof(l164);}l167 = sal_alloc(l169*l144->count,"rb_bufp");if(l167 == NULL){
l133 = SOC_E_MEMORY;goto l173;}l168 = sal_alloc(l170*l144->count,
"rb_sip_bufp");if(l168 == NULL){l133 = SOC_E_MEMORY;goto l173;}l171 = 
sal_alloc(sizeof(*l171)*l144->count,"roll_back_index");if(l171 == NULL){l133 = 
SOC_E_MEMORY;goto l173;}sal_memset(l171,-1,sizeof(*l171)*l144->count);for(
l161 = 0;l161<l144->count;l161++){payload_t*prefix = l144->prefix[l161];if(
prefix->index>= 0){l133 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,prefix->index,(
uint8*)l167+l161*l169);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x 0x%08x"" 0x%08x 0x%08x\n",prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]);l161--;l172 = TRUE;break
;}if(SOC_URPF_STATUS_GET(l2)){l133 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l2,prefix->index),(uint8*)l168+l161*l170);if(SOC_FAILURE(
l133)){soc_cm_debug(DK_ERR,"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x"" 0x%08x 0x%08x 0x%08x\n",prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]);l161--;l172 = TRUE;break
;}}l133 = soc_mem_write(l2,l24,MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null
(l2,l24));if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_split_inval: Failed to remove"
" prefixes from old bkt for pfx 0x%08x 0x%08x\n",prefix->key[0],prefix->key[1
]);l171[l161] = prefix->index;l172 = TRUE;break;}if(SOC_URPF_STATUS_GET(l2)){
l133 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,prefix->
index),soc_mem_entry_null(l2,l24));if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_split_inval: Failed to remove"
" prefixes from old urpf bkt for pfx 0x%08x"" 0x%08x\n",prefix->key[0],prefix
->key[1]);l171[l161] = prefix->index;l172 = TRUE;break;}}}l171[l161] = prefix
->index;prefix->index = l123[l161];}if(l172){for(;l161>= 0;l161--){payload_t*
prefix = l144->prefix[l161];prefix->index = l171[l161];if(l171[l161]<0){
continue;}l162 = soc_mem_write(l2,l24,MEM_BLOCK_ALL,l171[l161],(uint8*)l167+
l161*l169);if(SOC_FAILURE(l162)){break;}if(!SOC_URPF_STATUS_GET(l2)){continue
;}l162 = soc_mem_write(l2,l24,MEM_BLOCK_ALL,_soc_alpm_rpf_entry(l2,l171[l161]
),(uint8*)l168+l161*l170);if(SOC_FAILURE(l162)){break;}}}l173:if(l171){
sal_free(l171);l171 = NULL;}if(l168){sal_free(l168);l168 = NULL;}if(l167){
sal_free(l167);l167 = NULL;}return l133;}static int l174(int l2,void*l8,
soc_mem_t l24,void*l136,void*l142,int*l16,int bucket_index,int l138){
alpm_pivot_t*l175,*l176,*l177;defip_aux_scratch_entry_t l13;uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l178,l179;uint32 l180[5];int l34,
l28,l139;int l120;int l133 = SOC_E_NONE,l162;uint32 l11,l135,l147 = 0;int l140
=0;int l146;int l181 = 0;trie_t*trie,*l182;trie_node_t*l183,*l184 = NULL,*
l185 = NULL;payload_t*l186,*l187,*l188;defip_entry_t lpm_entry;
alpm_bucket_handle_t*l189;int l161,l190 = -1,l26 = 0;alpm_mem_prefix_array_t
l144;defip_alpm_ipv4_entry_t l163,l164;defip_alpm_ipv6_64_entry_t l165,l166;
void*l191,*l192;int*l123;l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if
(l34){if(!(l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l139,&l28));if
(l28 == SOC_VRF_MAX(l2)+1){l11 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l135);
}else{l11 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l135);}l24 = (l34)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l191 = ((l34)?((uint32*)&(l165)):(
(uint32*)&(l163)));l192 = ((l34)?((uint32*)&(l166)):((uint32*)&(l164)));
sal_memset(&l13,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l9(
l2,l8,l34,l11,0,&l13));if(bucket_index == 0){SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l2,PREFIX_LOOKUP,&l13,&l140,&l138,&bucket_index));if(l140 == 
0){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Could not find bucket to "
"insert prefix\n");return SOC_E_NOT_FOUND;}}l133 = _soc_alpm_insert_in_bkt(l2
,l24,bucket_index,l135,l136,l15,&l120,l34);if(l133 == SOC_E_NONE){*l16 = l120
;if(SOC_URPF_STATUS_GET(l2)){l162 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l2,l120),l142);if(SOC_FAILURE(l162)){return l162;}}}if(
l133 == SOC_E_FULL){l181 = 1;}l133 = l130(l2,l8,prefix,&l179,&l26);if(
SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: prefix create failed\n");return l133;}l175 = 
ALPM_TCAM_PIVOT(l2,l138);trie = PIVOT_BUCKET_TRIE(l175);l177 = l175;l186 = 
sal_alloc(sizeof(payload_t),"Payload for Key");if(l186 == NULL){soc_cm_debug(
DK_ERR,"_soc_alpm_insert: Unable to allocate memory for ""trie node \n");
return SOC_E_MEMORY;}l187 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l187 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory for ""pfx trie node \n");
sal_free(l186);return SOC_E_MEMORY;}sal_memset(l186,0,sizeof(*l186));
sal_memset(l187,0,sizeof(*l187));l186->key[0] = prefix[0];l186->key[1] = 
prefix[1];l186->key[2] = prefix[2];l186->key[3] = prefix[3];l186->key[4] = 
prefix[4];l186->len = l179;l186->index = l120;sal_memcpy(l187,l186,sizeof(*
l186));l187->bkt_ptr = l186;l133 = trie_insert(trie,prefix,NULL,l179,(
trie_node_t*)l186);if(SOC_FAILURE(l133)){goto l193;}if(l34){l182 = 
VRF_PREFIX_TRIE_IPV6(l2,l28);}else{l182 = VRF_PREFIX_TRIE_IPV4(l2,l28);}if(!
l26){l133 = trie_insert(l182,prefix,NULL,l179,(trie_node_t*)l187);}else{l185 = 
NULL;l133 = trie_find_lpm(l182,0,0,&l185);l188 = (payload_t*)l185;if(
SOC_SUCCESS(l133)){l188->bkt_ptr = l186;}}l178 = l179;if(SOC_FAILURE(l133)){
goto l194;}if(l181){l133 = alpm_bucket_assign(l2,&bucket_index,l34);if(
SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Unable to allocate"
"new bucket for split\n");bucket_index = -1;goto l195;}l133 = trie_split(trie
,l34?_MAX_KEY_LEN_144_:_MAX_KEY_LEN_48_,FALSE,l180,&l179,&l183,NULL,FALSE);if
(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not split bucket""for prefix 0x%08x 0x%08x\n",prefix
[0],prefix[1]);goto l195;}l185 = NULL;l133 = trie_find_lpm(l182,l180,l179,&
l185);l188 = (payload_t*)l185;if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n",l2,l180[0],l180[1],l180[2
],l180[3],l180[4],l179);goto l195;}if(l188->bkt_ptr){if(l188->bkt_ptr == l186
){sal_memcpy(l191,l136,l34?sizeof(defip_alpm_ipv6_64_entry_t):sizeof(
defip_alpm_ipv4_entry_t));}else{l133 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,((
payload_t*)l188->bkt_ptr)->index,l191);}if(SOC_FAILURE(l133)){goto l195;}l133
= l27(l2,l191,l24,l34,l139,bucket_index,0,&lpm_entry);if(SOC_FAILURE(l133)){
goto l195;}l147 = ((payload_t*)(l188->bkt_ptr))->len;}else{l133 = 
soc_mem_read(l2,L3_DEFIPm,MEM_BLOCK_ANY,soc_alpm_logical_idx(l2,L3_DEFIPm,
l138>>1,1),&lpm_entry);if((!l34)&&(l138&1)){l133 = 
soc_alpm_lpm_ip4entry1_to_0(l2,&lpm_entry,&lpm_entry,0);}}do{if(!(l34)){l180[
0] = (((32-l179) == 32)?0:(l180[1])<<(32-l179));l180[1] = 0;}else{int l196 = 
64-l179;int l197;if(l196<32){l197 = l180[3]<<l196;l197|= (((32-l196) == 32)?0
:(l180[4])>>(32-l196));l180[0] = l180[4]<<l196;l180[1] = l197;l180[2] = l180[
3] = l180[4] = 0;}else{l180[1] = (((l196-32) == 32)?0:(l180[4])<<(l196-32));
l180[0] = l180[2] = l180[3] = l180[4] = 0;}}}while(0);l30(l2,l180,l179,l28,
l34,&lpm_entry,0,0);soc_L3_DEFIPm_field32_set(l2,&lpm_entry,ALG_BKT_PTR0f,
bucket_index);l189 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l189 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n");l133 = 
SOC_E_MEMORY;goto l195;}sal_memset(l189,0,sizeof(*l189));l175 = sal_alloc(
sizeof(alpm_pivot_t),"Payload for new Pivot");if(l175 == NULL){soc_cm_debug(
DK_ERR,"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n"
);l133 = SOC_E_MEMORY;goto l195;}sal_memset(l175,0,sizeof(*l175));
PIVOT_BUCKET_HANDLE(l175) = l189;if(l34){l133 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l175));}else{l133 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l175));}PIVOT_BUCKET_TRIE(l175)->trie = l183;
PIVOT_BUCKET_INDEX(l175) = bucket_index;l175->key[0] = l180[0];l175->key[1] = 
l180[1];l175->len = l179;sal_memset(&l144,0,sizeof(l144));l133 = 
trie_traverse(PIVOT_BUCKET_TRIE(l175),alpm_mem_prefix_array_cb,&l144,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n",prefix[1],prefix[2],prefix[3],
prefix[4]);goto l195;}l123 = sal_alloc(sizeof(*l123)*l144.count,
"Temp storage for location of prefixes in new bucket");if(l123 == NULL){l133 = 
SOC_E_MEMORY;goto l195;}sal_memset(l123,-1,sizeof(*l123)*l144.count);for(l161
= 0;l161<l144.count;l161++){payload_t*prefix = l144.prefix[l161];if(prefix->
index>0){l133 = soc_mem_read(l2,l24,MEM_BLOCK_ANY,prefix->index,l191);if(
SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to"
"read prefix 0x%08x 0x%08x 0x%08x 0x%08x for move\n",l144.prefix[l161]->key[1
],l144.prefix[l161]->key[2],l144.prefix[l161]->key[3],l144.prefix[l161]->key[
4]);goto l198;}if(SOC_URPF_STATUS_GET(l2)){l133 = soc_mem_read(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,prefix->index),l192);if(SOC_FAILURE(l133
)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to"
"read rpf prefix 0x%08x 0x%08x 0x%08x 0x%08x for move\n",l144.prefix[l161]->
key[1],l144.prefix[l161]->key[2],l144.prefix[l161]->key[3],l144.prefix[l161]
->key[4]);goto l198;}}l133 = _soc_alpm_insert_in_bkt(l2,l24,bucket_index,l135
,l191,l15,&l120,l34);if(SOC_SUCCESS(l133)){if(SOC_URPF_STATUS_GET(l2)){l133 = 
soc_mem_write(l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l120),l192);}}}else
{l133 = _soc_alpm_insert_in_bkt(l2,l24,bucket_index,l135,l136,l15,&l120,l34);
if(SOC_SUCCESS(l133)){l190 = l161;*l16 = l120;if(SOC_URPF_STATUS_GET(l2)){
l133 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l120),l142);
}}}l123[l161] = l120;if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Failed to"
"write prefix 0x%08x 0x%08x 0x%08x 0x%08x for move\n",l144.prefix[l161]->key[
1],l144.prefix[l161]->key[2],l144.prefix[l161]->key[3],l144.prefix[l161]->key
[4]);goto l198;}}l133 = l5(l2,&lpm_entry,&l146);if(SOC_FAILURE(l133)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Unable to add new""pivot to tcam\n");
goto l198;}l146 = soc_alpm_physical_idx(l2,L3_DEFIPm,l146,l34);l133 = l145(l2
,l146,l34,l147);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n",l146);goto l199;}
ALPM_TCAM_PIVOT(l2,l146<<(l34?1:0)) = l175;PIVOT_TCAM_INDEX(l175) = l146<<(
l34?1:0);l133 = l160(l2,l24,l34,&l144,l123);if(SOC_FAILURE(l133)){goto l199;}
sal_free(l123);if(l190 == -1){l133 = _soc_alpm_insert_in_bkt(l2,l24,
PIVOT_BUCKET_HANDLE(l177)->bucket_index,l135,l136,l15,&l120,l34);if(
SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n");
goto l195;}if(SOC_URPF_STATUS_GET(l2)){l133 = soc_mem_write(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l120),l142);}*l16 = l120;l186->index = 
l120;}}VRF_TRIE_ROUTES_INC(l2,l28,l34);if(l26){sal_free(l187);}
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l140,&l138,&
bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,INSERT_PROPAGATE,&l13,
&l140,&l138,&bucket_index));if(SOC_URPF_STATUS_GET(l2)){l179 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);l179+= 1;
soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l179);
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l140,&l138,&
bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l2,INSERT_PROPAGATE,&l13,
&l140,&l138,&bucket_index));}return l133;l199:l162 = l7(l2,&lpm_entry);if(
SOC_FAILURE(l162)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Failure to free new prefix""at %d\n",soc_alpm_logical_idx(
l2,L3_DEFIPm,l146,l34));}ALPM_TCAM_PIVOT(l2,l146<<(l34?1:0)) = NULL;l198:l176
= l177;for(l161 = 0;l161<l144.count;l161++){payload_t*prefix = l144.prefix[
l161];if(l123[l161]!= -1){if(l34){sal_memset(l191,0,sizeof(
defip_alpm_ipv6_64_entry_t));}else{sal_memset(l191,0,sizeof(
defip_alpm_ipv4_entry_t));}l162 = soc_mem_write(l2,l24,MEM_BLOCK_ANY,l123[
l161],l191);_soc_trident2_alpm_bkt_view_set(l2,l123[l161],INVALIDm);if(
SOC_FAILURE(l162)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: mem write failure"
"in bkt move rollback\n");}if(SOC_URPF_STATUS_GET(l2)){l162 = soc_mem_write(
l2,l24,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l123[l161]),l191);
_soc_trident2_alpm_bkt_view_set(l2,_soc_alpm_rpf_entry(l2,l123[l161]),
INVALIDm);if(SOC_FAILURE(l162)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: urpf mem write ""failure in bkt move rollback\n");}}}l184 = 
NULL;l162 = trie_delete(PIVOT_BUCKET_TRIE(l175),prefix->key,prefix->len,&l184
);l186 = (payload_t*)l184;if(SOC_FAILURE(l162)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n");}if(prefix->
index>0){l162 = trie_insert(PIVOT_BUCKET_TRIE(l176),prefix->key,NULL,prefix->
len,(trie_node_t*)l186);if(SOC_FAILURE(l162)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: trie reinsert failure""in bkt move rollback\n");}}else{if(
l186!= NULL){sal_free(l186);}}}if(l190 == -1){l184 = NULL;l162 = trie_delete(
PIVOT_BUCKET_TRIE(l176),prefix,l178,&l184);l186 = (payload_t*)l184;if(
SOC_FAILURE(l162)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n",prefix[0],prefix[1]);}if(l186!= 
NULL){sal_free(l186);}}l162 = alpm_bucket_release(l2,PIVOT_BUCKET_INDEX(l175)
,l34);if(SOC_FAILURE(l162)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: new bucket release ""failure: %d\n",PIVOT_BUCKET_INDEX(
l175));}trie_destroy(PIVOT_BUCKET_TRIE(l175));sal_free(l189);sal_free(l175);
sal_free(l123);l184 = NULL;l162 = trie_delete(l182,prefix,l178,&l184);l187 = 
(payload_t*)l184;if(SOC_FAILURE(l162)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n",prefix[0],prefix[1]);}if(l187){sal_free(l187)
;}return l133;l195:l184 = NULL;(void)trie_delete(l182,prefix,l178,&l184);l187
= (payload_t*)l184;if(bucket_index!= -1){(void)alpm_bucket_release(l2,
bucket_index,l34);}l194:l184 = NULL;(void)trie_delete(trie,prefix,l178,&l184)
;l186 = (payload_t*)l184;l193:if(l186!= NULL){sal_free(l186);}if(l187!= NULL)
{sal_free(l187);}return l133;}static int l30(int l21,uint32*key,int len,int
l28,int l10,defip_entry_t*lpm_entry,int l31,int l32){uint32 l200;if(l32){
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
VALID1f,1);if(len>= 32){l200 = 0xffffffff;soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,IP_ADDR_MASK1f,l200);l200 = ~(((len-32) == 32)?0:(0xffffffff)>>(len
-32));soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l200);}else{
l200 = ~(0xffffffff>>len);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l200);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,0);}}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,IP_ADDR0f,
key[0]);assert(len<= 32);l200 = (len == 32)?0xffffffff:~(0xffffffff>>len);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l200);}
soc_L3_DEFIPm_field32_set(l21,lpm_entry,VALID0f,1);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,MODE_MASK0f,(1<<soc_mem_field_length(l21,L3_DEFIPm,
MODE_MASK0f))-1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE_MASK1f,(1<<
soc_mem_field_length(l21,L3_DEFIPm,MODE_MASK1f))-1);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,(1<<soc_mem_field_length(l21,L3_DEFIPm,
ENTRY_TYPE_MASK0f))-1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
ENTRY_TYPE_MASK1f,(1<<soc_mem_field_length(l21,L3_DEFIPm,ENTRY_TYPE_MASK1f))-
1);return(SOC_E_NONE);}int _soc_alpm_delete_in_bkt(int l2,soc_mem_t l24,int
l201,int l135,void*l202,uint32*l15,int*l120,int l34){int l133;l133 = 
soc_mem_alpm_delete(l2,l24,l201,MEM_BLOCK_ALL,l135,l202,l15,l120);if(
SOC_SUCCESS(l133)){return l133;}if(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){return
soc_mem_alpm_delete(l2,l24,l201+1,MEM_BLOCK_ALL,l135,l202,l15,l120);}return
l133;}static int l203(int l2,void*l8,int bucket_index,int l138,int l120){
alpm_pivot_t*l175;defip_alpm_ipv4_entry_t l163,l164;
defip_alpm_ipv6_64_entry_t l165,l166;defip_alpm_ipv4_entry_t l204,l205;
defip_aux_scratch_entry_t l13;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t
l24;void*l191,*l202,*l192 = NULL;int l139;int l10;int l133 = SOC_E_NONE,l162;
uint32 l206[5],prefix[5];int l34,l28;uint32 l179;int l201;uint32 l11,l135;int
l140,l26 = 0;trie_t*trie,*l182;uint32 l207;defip_entry_t lpm_entry,*l208;
payload_t*l186 = NULL,*l209 = NULL,*l188 = NULL;trie_node_t*l184 = NULL,*l185
= NULL;l10 = l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);if(l34){if(!(
l34 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l139,&l28));if(l139!= 
SOC_L3_VRF_OVERRIDE){if(l28 == SOC_VRF_MAX(l2)+1){l11 = 0;
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l2,l135);}else{l11 = 2;
SOC_ALPM_GET_VRF_BANK_DISABLE(l2,l135);}l133 = l130(l2,l8,prefix,&l179,&l26);
if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: prefix create failed\n");return l133;}if(!
soc_alpm_mode_get(l2)){if(l139!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l2
,l28,l34)>1){if(l26){soc_cm_debug(DK_ERR,"VRF %d: Cannot delete default "
"route if other routes are present in ""this mode",l28);return SOC_E_PARAM;}}
}l11 = 2;}l24 = (l34)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l191 = ((l34
)?((uint32*)&(l165)):((uint32*)&(l163)));l202 = ((l34)?((uint32*)&(l205)):((
uint32*)&(l204)));SOC_ALPM_LPM_LOCK(l2);if(bucket_index == 0){l133 = l137(l2,
l8,l24,l191,&l138,&bucket_index,&l120);}else{l133 = l20(l2,l8,l191,0,l24,0,0)
;}sal_memcpy(l202,l191,l34?sizeof(l165):sizeof(l163));if(SOC_FAILURE(l133)){
SOC_ALPM_LPM_UNLOCK(l2);soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to find ""prefix for delete\n");return l133;}l201 = 
bucket_index;l175 = ALPM_TCAM_PIVOT(l2,l138);trie = PIVOT_BUCKET_TRIE(l175);
l133 = trie_delete(trie,prefix,l179,&l184);l186 = (payload_t*)l184;if(l133!= 
SOC_E_NONE){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Error prefix not present "
"in trie \n");SOC_ALPM_LPM_UNLOCK(l2);return l133;}if(l34){l182 = 
VRF_PREFIX_TRIE_IPV6(l2,l28);}else{l182 = VRF_PREFIX_TRIE_IPV4(l2,l28);}if(!
l26){l133 = trie_delete(l182,prefix,l179,&l184);l209 = (payload_t*)l184;if(
SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n",prefix[0
],prefix[1]);goto l210;}l185 = NULL;l133 = trie_find_lpm(l182,prefix,l179,&
l185);l188 = (payload_t*)l185;if(SOC_SUCCESS(l133)){payload_t*l211 = (
payload_t*)(l188->bkt_ptr);if(l211!= NULL){l207 = l211->len;}else{l207 = 0;}}
else{soc_cm_debug(DK_ERR,"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n",prefix[0],prefix[1]);goto l212;}sal_memcpy(
l206,prefix,sizeof(prefix));do{if(!(l34)){l206[0] = (((32-l179) == 32)?0:(
l206[1])<<(32-l179));l206[1] = 0;}else{int l196 = 64-l179;int l197;if(l196<32
){l197 = l206[3]<<l196;l197|= (((32-l196) == 32)?0:(l206[4])>>(32-l196));l206
[0] = l206[4]<<l196;l206[1] = l197;l206[2] = l206[3] = l206[4] = 0;}else{l206
[1] = (((l196-32) == 32)?0:(l206[4])<<(l196-32));l206[0] = l206[2] = l206[3] = 
l206[4] = 0;}}}while(0);l133 = l30(l2,prefix,l207,l28,l10,&lpm_entry,0,1);
l162 = l137(l2,&lpm_entry,l24,l191,&l138,&bucket_index,&l120);(void)l27(l2,
l191,l24,l10,l139,bucket_index,0,&lpm_entry);(void)l30(l2,l206,l179,l28,l10,&
lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l2)){if(SOC_SUCCESS(l133)){l192 = ((l34
)?((uint32*)&(l166)):((uint32*)&(l164)));l162 = soc_mem_read(l2,l24,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l2,l120),l192);}}if((l207 == 0)&&
SOC_FAILURE(l162)){l208 = l34?VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);sal_memcpy(&lpm_entry,l208,sizeof(
lpm_entry));l133 = l30(l2,l206,l179,l28,l10,&lpm_entry,0,1);}if(SOC_FAILURE(
l133)){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n",prefix[0],prefix[1]);goto l212;}l208 = 
&lpm_entry;}else{l185 = NULL;l133 = trie_find_lpm(l182,prefix,l179,&l185);
l188 = (payload_t*)l185;if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Could not find default ""route in the trie for vrf %d\n",
l28);goto l210;}l188->bkt_ptr = NULL;l207 = 0;l208 = l34?
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28):VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);}l133
= l9(l2,l208,l34,l11,l207,&l13);if(SOC_FAILURE(l133)){goto l212;}l133 = 
_soc_alpm_aux_op(l2,DELETE_PROPAGATE,&l13,&l140,&l138,&bucket_index);if(
SOC_FAILURE(l133)){goto l212;}if(SOC_URPF_STATUS_GET(l2)){uint32 l129;if(l192
!= NULL){l129 = soc_mem_field32_get(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf);
l129++;soc_mem_field32_set(l2,L3_DEFIP_AUX_SCRATCHm,&l13,DB_TYPEf,l129);l129 = 
soc_mem_field32_get(l2,l24,l192,SRC_DISCARDf);soc_mem_field32_set(l2,l24,&l13
,SRC_DISCARDf,l129);l129 = soc_mem_field32_get(l2,l24,l192,DEFAULTROUTEf);
soc_mem_field32_set(l2,l24,&l13,DEFAULTROUTEf,l129);l133 = _soc_alpm_aux_op(
l2,DELETE_PROPAGATE,&l13,&l140,&l138,&bucket_index);}if(SOC_FAILURE(l133)){
goto l212;}}sal_free(l186);if(!l26){sal_free(l209);}if((l175->len!= 0)&&(trie
->trie == NULL)){l30(l2,l175->key,l175->len,l28,l10,&lpm_entry,0,1);l133 = l7
(l2,&lpm_entry);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to ""delete pivot 0x%08x 0x%08x \n",l175->key[0],
l175->key[1]);}}l133 = _soc_alpm_delete_in_bkt(l2,l24,l201,l135,l202,l15,&
l120,l34);if(!SOC_SUCCESS(l133)){SOC_ALPM_LPM_UNLOCK(l2);l133 = SOC_E_FAIL;
return l133;}if(SOC_URPF_STATUS_GET(l2)){l133 = soc_mem_alpm_delete(l2,l24,
SOC_ALPM_RPF_BKT_IDX(l2,l201),MEM_BLOCK_ALL,l135,l202,l15,&l140);if(!
SOC_SUCCESS(l133)){SOC_ALPM_LPM_UNLOCK(l2);l133 = SOC_E_FAIL;return l133;}}if
((l175->len!= 0)&&(trie->trie == NULL)){l133 = alpm_bucket_release(l2,
PIVOT_BUCKET_INDEX(l175),l34);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to release""empty bucket: %d\n",PIVOT_BUCKET_INDEX(
l175));}trie_destroy(PIVOT_BUCKET_TRIE(l175));sal_free(PIVOT_BUCKET_HANDLE(
l175));sal_free(l175);_soc_trident2_alpm_bkt_view_set(l2,l201<<2,INVALIDm);if
(SOC_ALPM_V6_SCALE_CHECK(l2,l34)){_soc_trident2_alpm_bkt_view_set(l2,(l201+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l2,l28,l34);if(VRF_TRIE_ROUTES_CNT(l2,
l28,l34) == 0){l133 = l33(l2,l28,l34);}SOC_ALPM_LPM_UNLOCK(l2);return l133;
l212:l162 = trie_insert(l182,prefix,NULL,l179,(trie_node_t*)l209);if(
SOC_FAILURE(l162)){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into pfx trie\n",prefix[0],prefix[1]);}l210:l162 = 
trie_insert(trie,prefix,NULL,l179,(trie_node_t*)l186);if(SOC_FAILURE(l162)){
soc_cm_debug(DK_ERR,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into bkt trie\n",prefix[0],prefix[1]);}
SOC_ALPM_LPM_UNLOCK(l2);return l133;}int soc_alpm_init(int l2){int l161;int
l133 = SOC_E_NONE;l133 = l3(l2);SOC_IF_ERROR_RETURN(l133);l133 = l19(l2);for(
l161 = 0;l161<MAX_PIVOT_COUNT;l161++){ALPM_TCAM_PIVOT(l2,l161) = NULL;}if(
SOC_CONTROL(l2)->alpm_bulk_retry == NULL){SOC_CONTROL(l2)->alpm_bulk_retry = 
sal_sem_create("ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l2)->
alpm_lookup_retry == NULL){SOC_CONTROL(l2)->alpm_lookup_retry = 
sal_sem_create("ALPM lookup retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l2)->
alpm_insert_retry == NULL){SOC_CONTROL(l2)->alpm_insert_retry = 
sal_sem_create("ALPM insert retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l2)->
alpm_delete_retry == NULL){SOC_CONTROL(l2)->alpm_delete_retry = 
sal_sem_create("ALPM delete retry",sal_sem_BINARY,0);}l133 = 
soc_alpm_128_lpm_init(l2);SOC_IF_ERROR_RETURN(l133);return l133;}static int
l213(int l2){int l161,l133;alpm_pivot_t*l129;for(l161 = 0;l161<
MAX_PIVOT_COUNT;l161++){l129 = ALPM_TCAM_PIVOT(l2,l161);if(l129){if(
PIVOT_BUCKET_HANDLE(l129)){if(PIVOT_BUCKET_TRIE(l129)){l133 = trie_traverse(
PIVOT_BUCKET_TRIE(l129),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l133)){trie_destroy(PIVOT_BUCKET_TRIE(l129));}else{soc_cm_debug(
DK_ERR,"Unable to clear trie state for ""unit %d\n",l2);return l133;}}
sal_free(PIVOT_BUCKET_HANDLE(l129));}sal_free(ALPM_TCAM_PIVOT(l2,l161));
ALPM_TCAM_PIVOT(l2,l161) = NULL;}}for(l161 = 0;l161<= SOC_VRF_MAX(l2)+1;l161
++){l133 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l2,l161),alpm_delete_node_cb,
NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l133)){trie_destroy(
VRF_PREFIX_TRIE_IPV4(l2,l161));}else{soc_cm_debug(DK_ERR,
"unit: %d Unable to clear v4 pfx trie for ""vrf %d\n",l2,l161);return l133;}
l133 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l2,l161),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l133)){trie_destroy(
VRF_PREFIX_TRIE_IPV6(l2,l161));}else{soc_cm_debug(DK_ERR,
"unit: %d Unable to clear v6 pfx trie for ""vrf %d\n",l2,l161);return l133;}
if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l161)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l161));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l161
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l161));}sal_memset(&
alpm_vrf_handle[l2][l161],0,sizeof(alpm_vrf_handle_t));}if(
SOC_ALPM_BUCKET_BMAP(l2)!= NULL){sal_free(SOC_ALPM_BUCKET_BMAP(l2));}
sal_memset(&soc_alpm_bucket[l2],0,sizeof(soc_alpm_bucket_t));return SOC_E_NONE
;}int soc_alpm_deinit(int l2){l4(l2);SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_deinit(l2));SOC_IF_ERROR_RETURN(soc_alpm_128_state_clear(l2)
);SOC_IF_ERROR_RETURN(l213(l2));if(SOC_CONTROL(l2)->alpm_bulk_retry){
sal_sem_destroy(SOC_CONTROL(l2)->alpm_bulk_retry);SOC_CONTROL(l2)->
alpm_bulk_retry = NULL;}if(SOC_CONTROL(l2)->alpm_lookup_retry == NULL){
sal_sem_destroy(SOC_CONTROL(l2)->alpm_lookup_retry);SOC_CONTROL(l2)->
alpm_lookup_retry = NULL;}if(SOC_CONTROL(l2)->alpm_insert_retry == NULL){
sal_sem_destroy(SOC_CONTROL(l2)->alpm_insert_retry);SOC_CONTROL(l2)->
alpm_insert_retry = NULL;}if(SOC_CONTROL(l2)->alpm_delete_retry == NULL){
sal_sem_destroy(SOC_CONTROL(l2)->alpm_delete_retry);SOC_CONTROL(l2)->
alpm_delete_retry = NULL;}return SOC_E_NONE;}static int l214(int l2,int l28,
int l34){defip_entry_t*lpm_entry,l215;int l216;int index;int l133 = 
SOC_E_NONE;uint32 key[2] = {0,0};uint32 l179;alpm_bucket_handle_t*l189;
alpm_pivot_t*l175;payload_t*l209;trie_t*l217;if(l34 == 0){trie_init(
_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l2,l28));l217 = VRF_PREFIX_TRIE_IPV4(
l2,l28);}else{trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l2,l28));l217
= VRF_PREFIX_TRIE_IPV6(l2,l28);}lpm_entry = sal_alloc(sizeof(defip_entry_t),
"Default LPM entry");if(lpm_entry == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n");return
SOC_E_MEMORY;}l30(l2,key,0,l28,l34,lpm_entry,0,1);if(l34 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28) = lpm_entry;}if(l28 == SOC_VRF_MAX(l2)+1)
{soc_L3_DEFIPm_field32_set(l2,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l2,lpm_entry,DEFAULT_MISS0f,1);}l133 = 
alpm_bucket_assign(l2,&l216,l34);soc_L3_DEFIPm_field32_set(l2,lpm_entry,
ALG_BKT_PTR0f,l216);sal_memcpy(&l215,lpm_entry,sizeof(l215));l133 = l5(l2,&
l215,&index);l189 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l189 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n");
return SOC_E_NONE;}sal_memset(l189,0,sizeof(*l189));l175 = sal_alloc(sizeof(
alpm_pivot_t),"Payload for Pivot");if(l175 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n");
sal_free(l189);return SOC_E_MEMORY;}l209 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l209 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n");
sal_free(l189);sal_free(l175);return SOC_E_MEMORY;}sal_memset(l175,0,sizeof(*
l175));sal_memset(l209,0,sizeof(*l209));l179 = 0;PIVOT_BUCKET_HANDLE(l175) = 
l189;if(l34){trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l175));}else{
trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l175));}PIVOT_BUCKET_INDEX(l175
) = l216;PIVOT_TCAM_INDEX(l175) = index;l175->key[0] = l209->key[0] = key[0];
l175->key[1] = l209->key[1] = key[1];l175->len = l209->len = l179;l133 = 
trie_insert(l217,key,NULL,l179,&(l209->node));if(SOC_FAILURE(l133)){sal_free(
l209);sal_free(l175);sal_free(l189);return l133;}index = 
soc_alpm_physical_idx(l2,L3_DEFIPm,index,l34);if(l34 == 0){ALPM_TCAM_PIVOT(l2
,index) = l175;}else{ALPM_TCAM_PIVOT(l2,index<<1) = l175;}VRF_TRIE_INIT_DONE(
l2,l28,l34,1);return l133;}static int l33(int l2,int l28,int l34){
defip_entry_t*lpm_entry;int l216;int l126;int l133 = SOC_E_NONE;uint32 key[2]
= {0,0},l218[SOC_MAX_MEM_FIELD_WORDS];payload_t*l186;alpm_pivot_t*l219;
trie_node_t*l184;trie_t*l217;if(l34 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28);}else{lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6(l2,l28);}l216 = soc_L3_DEFIPm_field32_get(l2,
lpm_entry,ALG_BKT_PTR0f);l133 = alpm_bucket_release(l2,l216,l34);
_soc_trident2_alpm_bkt_view_set(l2,l216<<2,INVALIDm);l133 = l18(l2,lpm_entry,
(void*)l218,&l126);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n",l28
,l34);l126 = -1;}l126 = soc_alpm_physical_idx(l2,L3_DEFIPm,l126,l34);if(l34 == 
0){l219 = ALPM_TCAM_PIVOT(l2,l126);}else{l219 = ALPM_TCAM_PIVOT(l2,l126<<1);}
l133 = l7(l2,lpm_entry);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n",l28,l34);}sal_free(lpm_entry);if(l34
== 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l2,l28) = NULL;l217 = VRF_PREFIX_TRIE_IPV4(
l2,l28);VRF_PREFIX_TRIE_IPV4(l2,l28) = NULL;}else{VRF_TRIE_DEFAULT_ROUTE_IPV6
(l2,l28) = NULL;l217 = VRF_PREFIX_TRIE_IPV6(l2,l28);VRF_PREFIX_TRIE_IPV6(l2,
l28) = NULL;}VRF_TRIE_INIT_DONE(l2,l28,l34,0);l133 = trie_delete(l217,key,0,&
l184);l186 = (payload_t*)l184;if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"Unable to delete internal default for vrf "" %d/%d\n",l28,l34);}sal_free(
l186);(void)trie_destroy(l217);sal_free(PIVOT_BUCKET_HANDLE(l219));(void)
trie_destroy(PIVOT_BUCKET_TRIE(l219));sal_free(l219);return l133;}int
soc_alpm_insert(int l2,void*l6,uint32 l25,int l220,int l221){
defip_alpm_ipv4_entry_t l163,l164;defip_alpm_ipv6_64_entry_t l165,l166;
soc_mem_t l24;void*l191,*l202;int l139,l28;int index;int l10;int l133 = 
SOC_E_NONE;uint32 l26;l10 = soc_mem_field32_get(l2,L3_DEFIPm,l6,MODE0f);l24 = 
(l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l191 = ((l10)?((uint32*)&(
l165)):((uint32*)&(l163)));l202 = ((l10)?((uint32*)&(l166)):((uint32*)&(l164)
));SOC_IF_ERROR_RETURN(l20(l2,l6,l191,l202,l24,l25,&l26));SOC_IF_ERROR_RETURN
(soc_alpm_lpm_vrf_get(l2,l6,&l139,&l28));if(l139 == SOC_L3_VRF_OVERRIDE){l133
= l5(l2,l6,&index);return(l133);}else if(l28 == 0){if(soc_alpm_mode_get(l2)){
soc_cm_debug(DK_ERR,"Unit %d, VRF=0 cannot be added in Parallel ""mode\n",l2)
;return SOC_E_PARAM;}}if(l139!= SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l2))
{if(VRF_TRIE_ROUTES_CNT(l2,l28,l10) == 0){if(!l26){soc_cm_debug(DK_ERR,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n",
l139);return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){
soc_cm_debug(DK_VERBOSE,"soc_alpm_insert:VRF %d is not ""initialized\n",l28);
l133 = l214(l2,l28,l10);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n",l28,l10);return l133;}
soc_cm_debug(DK_VERBOSE,"soc_alpm_insert:VRF %d/%d trie init ""completed\n",
l28,l10);}if(l221&SOC_ALPM_LOOKUP_HIT){l133 = l141(l2,l6,l191,l202,l24,l220);
}else{if(l220 == -1){l220 = 0;}l133 = l174(l2,l6,l24,l191,l202,&index,
SOC_ALPM_BKT_ENTRY_TO_IDX(l220),l221);}if(l133!= SOC_E_NONE){soc_cm_debug(
DK_WARN,"unit %d :soc_alpm_insert: ""Route Insertion Failed :%s\n",l2,
soc_errmsg(l133));}return(l133);}int soc_alpm_lookup(int l2,void*l8,void*l15,
int*l16,int*l222){defip_alpm_ipv4_entry_t l163;defip_alpm_ipv6_64_entry_t l165
;soc_mem_t l24;int bucket_index;int l138;void*l191;int l139,l28;int l10,l119;
int l133 = SOC_E_NONE;l10 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,l8,&l139,&l28));l133 = l14(l2,l8,
l15,l16,&l119,&l10);if(SOC_SUCCESS(l133)){if(!l10&&(*l16&0x1)){l133 = 
soc_alpm_lpm_ip4entry1_to_0(l2,l15,l15,PRESERVE_HIT);}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l2,l15,&l139,&l28));if(l139 == SOC_L3_VRF_OVERRIDE){
return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){soc_cm_debug(
DK_VERBOSE,"soc_alpm_lookup:VRF %d is not ""initialized\n",l28);*l222 = 0;
return SOC_E_NOT_FOUND;}l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l191 = ((l10)?((uint32*)&(l165)):((uint32*)&(l163)));
SOC_ALPM_LPM_LOCK(l2);l133 = l137(l2,l8,l24,l191,&l138,&bucket_index,l16);
SOC_ALPM_LPM_UNLOCK(l2);if(SOC_FAILURE(l133)){*l222 = l138;*l16 = 
bucket_index<<2;return l133;}l133 = l27(l2,l191,l24,l10,l139,bucket_index,*
l16,l15);*l222 = SOC_ALPM_LOOKUP_HIT|l138;return(l133);}int soc_alpm_delete(
int l2,void*l8,int l220,int l221){int l139,l28;int l10;int l133 = SOC_E_NONE;
l10 = soc_mem_field32_get(l2,L3_DEFIPm,l8,MODE0f);SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l2,l8,&l139,&l28));if(l139 == SOC_L3_VRF_OVERRIDE){l133 = 
l7(l2,l8);return(l133);}else{if(!VRF_TRIE_INIT_COMPLETED(l2,l28,l10)){
soc_cm_debug(DK_VERBOSE,"soc_alpm_delete:VRF %d/%d is not ""initialized\n",
l28,l10);return SOC_E_NONE;}if(l220 == -1){l220 = 0;}l133 = l203(l2,l8,
SOC_ALPM_BKT_ENTRY_TO_IDX(l220),l221&~SOC_ALPM_LOOKUP_HIT,l220);}return(l133)
;}static int l19(int l2){int l223;l223 = soc_mem_index_count(l2,L3_DEFIPm)+
soc_mem_index_count(l2,L3_DEFIP_PAIR_128m)*2;if(SOC_URPF_STATUS_GET(l2)){l223
>>= 1;}SOC_ALPM_BUCKET_COUNT(l2) = l223*2;SOC_ALPM_BUCKET_BMAP_SIZE(l2) = 
SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(l2));SOC_ALPM_BUCKET_BMAP(l2) = 
sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l2),"alpm_shared_bucket_bitmap");if(
SOC_ALPM_BUCKET_BMAP(l2) == NULL){soc_cm_debug(DK_WARN,
"soc_alpm_shared_mem_init: Memory allocation for bucket bitmap management failed\n"
);return SOC_E_MEMORY;}sal_memset(SOC_ALPM_BUCKET_BMAP(l2),0,
SOC_ALPM_BUCKET_BMAP_SIZE(l2));alpm_bucket_assign(l2,&l223,1);return
SOC_E_NONE;}int alpm_bucket_assign(int l2,int*l216,int l34){int l161,l224 = 1
,l225 = 0;if(l34){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l224 = 
2;}}for(l161 = 0;l161<SOC_ALPM_BUCKET_COUNT(l2);l161+= l224){
SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l161,l224,l225);if(0 == l225){
break;}}if(l161 == SOC_ALPM_BUCKET_COUNT(l2)){return SOC_E_FULL;}
SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l161,l224);*l216 = l161;
SOC_ALPM_BUCKET_NEXT_FREE(l2) = l161;return SOC_E_NONE;}int
alpm_bucket_release(int l2,int l216,int l34){int l224 = 1,l225 = 0;if((l216<1
)||(l216>SOC_ALPM_BUCKET_MAX_INDEX(l2))){soc_cm_debug(DK_ERR,
"Unit %d\n, freeing invalid bucket index %d\n",l2,l216);return SOC_E_PARAM;}
if(l34){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l224 = 2;}}
SHR_BITTEST_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l216,l224,l225);if(!l225){return
SOC_E_PARAM;}SHR_BITCLR_RANGE(SOC_ALPM_BUCKET_BMAP(l2),l216,l224);return
SOC_E_NONE;}static void l107(int l2,void*l15,int index,l102 l108){if(index&(
0x8000)){l108[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l95[(l2)]->l61));l108[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l63));l108[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l95[(l2)]->l62));l108[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l64));if((!(SOC_IS_HURRICANE(l2)))&&(((l95[(l2)]->l77)!= NULL))){int
l226;(void)soc_alpm_lpm_vrf_get(l2,l15,(int*)&l108[4],&l226);}else{l108[4] = 
0;};}else{if(index&0x1){l108[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(l2)]->l62));l108[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l64));l108[2] = 0;l108[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&((
(l95[(l2)]->l78)!= NULL))){int l226;defip_entry_t l227;(void)
soc_alpm_lpm_ip4entry1_to_0(l2,l15,&l227,0);(void)soc_alpm_lpm_vrf_get(l2,&
l227,(int*)&l108[4],&l226);}else{l108[4] = 0;};}else{l108[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l61));l108[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l95[(l2)]->l63));l108[2] = 0;l108[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l2)))&&(((l95[(l2)]->l77)!= NULL))){int l226;(void)
soc_alpm_lpm_vrf_get(l2,l15,(int*)&l108[4],&l226);}else{l108[4] = 0;};}}}
static int l228(l102 l104,l102 l105){int l126;for(l126 = 0;l126<5;l126++){{if
((l104[l126])<(l105[l126])){return-1;}if((l104[l126])>(l105[l126])){return 1;
}};}return(0);}static void l229(int l2,void*l6,uint32 l230,uint32 l122,int
l119){l102 l231;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l95[(l2)]->l65))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l95[(l2)]->l76))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l95[(l2
)]->l75))){l231[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l95[(l2)]->l61));l231[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l95[(l2
)]->l63));l231[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l95[(l2)]->l62));l231[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l95[(l2
)]->l64));if((!(SOC_IS_HURRICANE(l2)))&&(((l95[(l2)]->l77)!= NULL))){int l226
;(void)soc_alpm_lpm_vrf_get(l2,l6,(int*)&l231[4],&l226);}else{l231[4] = 0;};
l121((l106[(l2)]),l228,l231,l119,l122,((uint16)l230<<1)|(0x8000));}}else{if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l95[(l2
)]->l75))){l231[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l95[(l2)]->l61));l231[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l95[(l2
)]->l63));l231[2] = 0;l231[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((
l95[(l2)]->l77)!= NULL))){int l226;(void)soc_alpm_lpm_vrf_get(l2,l6,(int*)&
l231[4],&l226);}else{l231[4] = 0;};l121((l106[(l2)]),l228,l231,l119,l122,((
uint16)l230<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l95[(l2)]->l76))){l231[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l6),(l95[(l2
)]->l62));l231[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l6),(l95[(l2)]->l64));l231[2] = 0;l231[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l2)))&&(((l95[(l2)]->l78)!= NULL))){int l226;defip_entry_t
l227;(void)soc_alpm_lpm_ip4entry1_to_0(l2,l6,&l227,0);(void)
soc_alpm_lpm_vrf_get(l2,&l227,(int*)&l231[4],&l226);}else{l231[4] = 0;};l121(
(l106[(l2)]),l228,l231,l119,l122,(((uint16)l230<<1)+1));}}}static void l232(
int l2,void*l8,uint32 l230){l102 l231;int l119 = -1;int l133;uint16 index;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2
)]->l65))){l231[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l95[(l2)]->l61));l231[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2
)]->l63));l231[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l95[(l2)]->l62));l231[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2
)]->l64));if((!(SOC_IS_HURRICANE(l2)))&&(((l95[(l2)]->l77)!= NULL))){int l226
;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&l231[4],&l226);}else{l231[4] = 0;};
index = (l230<<1)|(0x8000);}else{l231[0] = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2)]->l61));l231[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2
)]->l63));l231[2] = 0;l231[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((
l95[(l2)]->l77)!= NULL))){int l226;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&
l231[4],&l226);}else{l231[4] = 0;};index = l230;}l133 = l124((l106[(l2)]),
l228,l231,l119,index);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"\ndel  index: H %d error %d\n",index,l133);}}static int l233(int l2,void*l8,
int l119,int*l120){l102 l231;int l234;int l133;uint16 index = (0xFFFF);l234 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2
)]->l65));if(l234){l231[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l8),(l95[(l2)]->l61));l231[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2
)]->l63));l231[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l8),(l95[(l2)]->l62));l231[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2
)]->l64));if((!(SOC_IS_HURRICANE(l2)))&&(((l95[(l2)]->l77)!= NULL))){int l226
;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&l231[4],&l226);}else{l231[4] = 0;};}
else{l231[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)
),(l8),(l95[(l2)]->l61));l231[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2)]->l63));l231[2] = 0;l231[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l2)))&&(((l95[(l2)]->l77)!= NULL))){int l226
;(void)soc_alpm_lpm_vrf_get(l2,l8,(int*)&l231[4],&l226);}else{l231[4] = 0;};}
l133 = l116((l106[(l2)]),l228,l231,l119,&index);if(SOC_FAILURE(l133)){*l120 = 
0xFFFFFFFF;return(l133);}*l120 = index;return(SOC_E_NONE);}static uint16 l109
(uint8*l110,int l111){return(_shr_crc16b(0,l110,l111));}static int l112(int
l21,int l97,int l98,l101**l113){l101*l117;int index;if(l98>l97){return
SOC_E_MEMORY;}l117 = sal_alloc(sizeof(l101),"lpm_hash");if(l117 == NULL){
return SOC_E_MEMORY;}sal_memset(l117,0,sizeof(*l117));l117->l21 = l21;l117->
l97 = l97;l117->l98 = l98;l117->l99 = sal_alloc(l117->l98*sizeof(*(l117->l99)
),"hash_table");if(l117->l99 == NULL){sal_free(l117);return SOC_E_MEMORY;}
l117->l100 = sal_alloc(l117->l97*sizeof(*(l117->l100)),"link_table");if(l117
->l100 == NULL){sal_free(l117->l99);sal_free(l117);return SOC_E_MEMORY;}for(
index = 0;index<l117->l98;index++){l117->l99[index] = (0xFFFF);}for(index = 0
;index<l117->l97;index++){l117->l100[index] = (0xFFFF);}*l113 = l117;return
SOC_E_NONE;}static int l114(l101*l115){if(l115!= NULL){sal_free(l115->l99);
sal_free(l115->l100);sal_free(l115);}return SOC_E_NONE;}static int l116(l101*
l117,l103 l118,l102 entry,int l119,uint16*l120){int l2 = l117->l21;uint16 l235
;uint16 index;l235 = l109((uint8*)entry,(32*5))%l117->l98;index = l117->l99[
l235];;;while(index!= (0xFFFF)){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];l102 l108
;int l236;l236 = (index&(0x3FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,
MEM_BLOCK_ANY,l236,l15));l107(l2,l15,index,l108);if((*l118)(entry,l108) == 0)
{*l120 = (index&(0x3FFF))>>((index&(0x8000))?1:0);;return(SOC_E_NONE);}index = 
l117->l100[index&(0x3FFF)];;};return(SOC_E_NOT_FOUND);}static int l121(l101*
l117,l103 l118,l102 entry,int l119,uint16 l122,uint16 l123){int l2 = l117->
l21;uint16 l235;uint16 index;uint16 l237;l235 = l109((uint8*)entry,(32*5))%
l117->l98;index = l117->l99[l235];;;;l237 = (0xFFFF);if(l122!= (0xFFFF)){
while(index!= (0xFFFF)){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];l102 l108;int l236
;l236 = (index&(0x3FFF))>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,
MEM_BLOCK_ANY,l236,l15));l107(l2,l15,index,l108);if((*l118)(entry,l108) == 0)
{if(l123!= index){;if(l237 == (0xFFFF)){l117->l99[l235] = l123;l117->l100[
l123&(0x3FFF)] = l117->l100[index&(0x3FFF)];l117->l100[index&(0x3FFF)] = (
0xFFFF);}else{l117->l100[l237&(0x3FFF)] = l123;l117->l100[l123&(0x3FFF)] = 
l117->l100[index&(0x3FFF)];l117->l100[index&(0x3FFF)] = (0xFFFF);}};return(
SOC_E_NONE);}l237 = index;index = l117->l100[index&(0x3FFF)];;}}l117->l100[
l123&(0x3FFF)] = l117->l99[l235];l117->l99[l235] = l123;return(SOC_E_NONE);}
static int l124(l101*l117,l103 l118,l102 entry,int l119,uint16 l125){uint16
l235;uint16 index;uint16 l237;l235 = l109((uint8*)entry,(32*5))%l117->l98;
index = l117->l99[l235];;;l237 = (0xFFFF);while(index!= (0xFFFF)){if(l125 == 
index){;if(l237 == (0xFFFF)){l117->l99[l235] = l117->l100[l125&(0x3FFF)];l117
->l100[l125&(0x3FFF)] = (0xFFFF);}else{l117->l100[l237&(0x3FFF)] = l117->l100
[l125&(0x3FFF)];l117->l100[l125&(0x3FFF)] = (0xFFFF);}return(SOC_E_NONE);}
l237 = index;index = l117->l100[index&(0x3FFF)];;}return(SOC_E_NOT_FOUND);}
int _ipmask2pfx(uint32 l35,int*l36){*l36 = 0;while(l35&(1<<31)){*l36+= 1;l35
<<= 1;}return((l35)?SOC_E_PARAM:SOC_E_NONE);}int soc_alpm_lpm_ip4entry0_to_0(
int l2,void*l238,void*l239,int l240){uint32 l241;l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l75),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l65));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l65),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l61));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l61),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l63));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l63),(l241));if(((l95[(l2)]->l51)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l51));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l51),(l241));}if(((l95[(l2)]->l53)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l53));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l53),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l55));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l55),(l241));}else{l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l69));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l69),(l241));}l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l238),(l95[(l2)]->l71));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l71),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l73));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l73),(l241));if(((l95[(l2)]->l77)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l77),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l79),(l241));}if(((l95[(l2)]->l49)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l49),(l241));}if(l240){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l59),(l241));}l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l81),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l83));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l83),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l85),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l87));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l87),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l89));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l89),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l91));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l91),(l241));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_1(int l2,void*l238,void*l239,int l240){uint32 l241;
l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238)
,(l95[(l2)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l239),(l95[(l2)]->l76),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l66),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l62));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l62),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l64),(l241));if(((l95[(l2)]->
l52)!= NULL)){l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l238),(l95[(l2)]->l52));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l52),(l241));}if(((l95[(l2)]->
l54)!= NULL)){l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l238),(l95[(l2)]->l54));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l54),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l56),(l241));}else{l241 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l70));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l70),(l241));}l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l238),(l95[(l2)]->l72));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l72),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l74),(l241));if(((l95[(l2)]->l78)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l78),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l80),(l241));}if(((l95[(l2)]->l50)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l50),(l241));}if(l240){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l60),(l241));}l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l82),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l84),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l86),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l88));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l88),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l90));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l90),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l92));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l92),(l241));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_1(int l2,void*l238,void*l239,int l240){uint32 l241;
l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238)
,(l95[(l2)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l239),(l95[(l2)]->l76),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l65));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l66),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l61));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l62),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l63));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l64),(l241));if(!
SOC_IS_HURRICANE(l2)){l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l51));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l52),(l241));}if(((l95[(l2)]->
l53)!= NULL)){l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l238),(l95[(l2)]->l53));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l54),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l55));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l56),(l241));}else{l241 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l69));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l70),(l241));}l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l238),(l95[(l2)]->l71));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l72),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l73));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l74),(l241));if(((l95[(l2)]->l77)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l78),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l80),(l241));}if(((l95[(l2)]->l49)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l50),(l241));}if(l240){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l60),(l241));}l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l82),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l83));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l84),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l86),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l87));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l88),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l89));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l90),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l91));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l92),(l241));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_0(int l2,void*l238,void*l239,int l240){uint32 l241;
l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238)
,(l95[(l2)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l239),(l95[(l2)]->l75),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l65),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l62));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l61),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l63),(l241));if(!
SOC_IS_HURRICANE(l2)){l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l52));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l51),(l241));}if(((l95[(l2)]->
l54)!= NULL)){l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l238),(l95[(l2)]->l54));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l53),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l55),(l241));}else{l241 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l70));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l69),(l241));}l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l2,L3_DEFIPm)),(l238),(l95[(l2)]->l72));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l71),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l74));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l73),(l241));if(((l95[(l2)]->l78)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l78));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l77),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l80));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l79),(l241));}if(((l95[(l2)]->l50)!= NULL)){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l49),(l241));}if(l240){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l59),(l241));}l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l81),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l83),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l85),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(l2)]->l88));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(
l2)]->l87),(l241));l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2
,L3_DEFIPm)),(l238),(l95[(l2)]->l90));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l239),(l95[(l2)]->l89),(l241));l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l238),(l95[(
l2)]->l92));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(
l239),(l95[(l2)]->l91),(l241));return(SOC_E_NONE);}static int l242(int l2,
void*l15){return(SOC_E_NONE);}void l1(int l2){int l161;int l243;l243 = ((3*(
64+32+2+1))-1);if(!soc_cm_debug_check(DK_L3|DK_SOCMEM|DK_VERBOSE)){return;}
for(l161 = l243;l161>= 0;l161--){if((l161!= ((3*(64+32+2+1))-1))&&((l45[(l2)]
[(l161)].l38) == -1)){continue;}soc_cm_debug(DK_L3|DK_SOCMEM|DK_VERBOSE,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n",l161,(l45[
(l2)][(l161)].l40),(l45[(l2)][(l161)].next),(l45[(l2)][(l161)].l38),(l45[(l2)
][(l161)].l39),(l45[(l2)][(l161)].l41),(l45[(l2)][(l161)].l42));}
COMPILER_REFERENCE(l242);}static int l244(int l2,int index,uint32*l15){int
l245;int l10;uint32 l246;uint32 l247;int l248;if(!SOC_URPF_STATUS_GET(l2)){
return(SOC_E_NONE);}if(soc_feature(l2,soc_feature_l3_defip_hole)){l245 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l2)){l245 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1)+0x0400;}else{l245 = (
soc_mem_index_count(l2,L3_DEFIPm)>>1);}l10 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l65));if(((l95[(l2)]->l49)!= NULL)){soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(l2)]->l49),(0));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l50),(0));}l246 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l95[(l2)]->l63));l247 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(l2)]->l64));if(!l10){if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l75))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l15),(l95[(l2)]->l73),((!l246)?1:0));}if(soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(l2)]->l76))){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l74),((!l247)?1:0));}}else{l248 = ((!l246)&&(!l247))?1:0;l246 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l75));l247 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l95[(l2)]->l76));if(l246&&l247){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l73),(l248));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l95[(l2)]->l74),(l248));}}return l157(l2,MEM_BLOCK_ANY,
index+l245,index,l15);}static int l249(int l2,int l250,int l251){uint32 l15[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,
l250,l15));l229(l2,l15,l251,0x4000,0);SOC_IF_ERROR_RETURN(l157(l2,
MEM_BLOCK_ANY,l251,l250,l15));SOC_IF_ERROR_RETURN(l244(l2,l251,l15));do{int
l252,l253;l252 = soc_alpm_physical_idx((l2),L3_DEFIPm,(l250),1);l253 = 
soc_alpm_physical_idx((l2),L3_DEFIPm,(l251),1);ALPM_TCAM_PIVOT(l2,l253<<1) = 
ALPM_TCAM_PIVOT(l2,l252<<1);ALPM_TCAM_PIVOT(l2,(l253<<1)+1) = ALPM_TCAM_PIVOT
(l2,(l252<<1)+1);ALPM_TCAM_PIVOT(l2,l252<<1) = NULL;ALPM_TCAM_PIVOT(l2,(l252
<<1)+1) = NULL;}while(0);return(SOC_E_NONE);}static int l254(int l2,int l119,
int l10){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l250;int l251;uint32 l255,
l256;l251 = (l45[(l2)][(l119)].l39)+1;if(!l10){l250 = (l45[(l2)][(l119)].l39)
;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l250,l15));l255 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l75));l256 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l95[(l2)]->l76));if((l255 == 0)||(l256 == 0)){l229(l2,l15,
l251,0x4000,0);SOC_IF_ERROR_RETURN(l157(l2,MEM_BLOCK_ANY,l251,l250,l15));
SOC_IF_ERROR_RETURN(l244(l2,l251,l15));do{int l257 = soc_alpm_physical_idx((
l2),L3_DEFIPm,(l250),1)<<1;if((l256)){l257++;}ALPM_TCAM_PIVOT((l2),
soc_alpm_physical_idx((l2),L3_DEFIPm,(l251),1)*2+(l257&1)) = ALPM_TCAM_PIVOT(
(l2),l257);ALPM_TCAM_PIVOT((l2),l257) = NULL;}while(0);l251--;}}l250 = (l45[(
l2)][(l119)].l38);if(l250!= l251){SOC_IF_ERROR_RETURN(l249(l2,l250,l251));}(
l45[(l2)][(l119)].l38)+= 1;(l45[(l2)][(l119)].l39)+= 1;return(SOC_E_NONE);}
static int l258(int l2,int l119,int l10){uint32 l15[SOC_MAX_MEM_FIELD_WORDS];
int l250;int l251;int l259;uint32 l255,l256;l251 = (l45[(l2)][(l119)].l38)-1;
if((l45[(l2)][(l119)].l41) == 0){(l45[(l2)][(l119)].l38) = l251;(l45[(l2)][(
l119)].l39) = l251-1;return(SOC_E_NONE);}if((!l10)&&((l45[(l2)][(l119)].l39)
!= (l45[(l2)][(l119)].l38))){l250 = (l45[(l2)][(l119)].l39);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l250,l15));l255 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(
l2)]->l75));l256 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l95[(l2)]->l76));if((l255 == 0)||(l256 == 0)){l259 = l250-
1;SOC_IF_ERROR_RETURN(l249(l2,l259,l251));l229(l2,l15,l259,0x4000,0);
SOC_IF_ERROR_RETURN(l157(l2,MEM_BLOCK_ANY,l259,l250,l15));SOC_IF_ERROR_RETURN
(l244(l2,l259,l15));do{int l257 = soc_alpm_physical_idx((l2),L3_DEFIPm,(l250)
,1)<<1;if((l256)){l257++;}ALPM_TCAM_PIVOT((l2),soc_alpm_physical_idx((l2),
L3_DEFIPm,(l259),1)*2+(l257&1)) = ALPM_TCAM_PIVOT((l2),l257);ALPM_TCAM_PIVOT(
(l2),l257) = NULL;}while(0);}else{l229(l2,l15,l251,0x4000,0);
SOC_IF_ERROR_RETURN(l157(l2,MEM_BLOCK_ANY,l251,l250,l15));SOC_IF_ERROR_RETURN
(l244(l2,l251,l15));do{int l252,l253;l252 = soc_alpm_physical_idx((l2),
L3_DEFIPm,(l250),1);l253 = soc_alpm_physical_idx((l2),L3_DEFIPm,(l251),1);
ALPM_TCAM_PIVOT(l2,l253<<1) = ALPM_TCAM_PIVOT(l2,l252<<1);ALPM_TCAM_PIVOT(l2,
(l253<<1)+1) = ALPM_TCAM_PIVOT(l2,(l252<<1)+1);ALPM_TCAM_PIVOT(l2,l252<<1) = 
NULL;ALPM_TCAM_PIVOT(l2,(l252<<1)+1) = NULL;}while(0);}}else{l250 = (l45[(l2)
][(l119)].l39);SOC_IF_ERROR_RETURN(l249(l2,l250,l251));}(l45[(l2)][(l119)].
l38)-= 1;(l45[(l2)][(l119)].l39)-= 1;return(SOC_E_NONE);}static int l260(int
l2,int l119,int l10,void*l15,int*l261){int l262;int l263;int l264;int l265;
int l250;uint32 l255,l256;int l133;if((l45[(l2)][(l119)].l41) == 0){l265 = ((
3*(64+32+2+1))-1);if(soc_alpm_mode_get(l2) == SOC_ALPM_MODE_PARALLEL){if(l119
<= (((3*(64+32+2+1))/3)-1)){l265 = (((3*(64+32+2+1))/3)-1);}}while((l45[(l2)]
[(l265)].next)>l119){l265 = (l45[(l2)][(l265)].next);}l263 = (l45[(l2)][(l265
)].next);if(l263!= -1){(l45[(l2)][(l263)].l40) = l119;}(l45[(l2)][(l119)].
next) = (l45[(l2)][(l265)].next);(l45[(l2)][(l119)].l40) = l265;(l45[(l2)][(
l265)].next) = l119;(l45[(l2)][(l119)].l42) = ((l45[(l2)][(l265)].l42)+1)/2;(
l45[(l2)][(l265)].l42)-= (l45[(l2)][(l119)].l42);(l45[(l2)][(l119)].l38) = (
l45[(l2)][(l265)].l39)+(l45[(l2)][(l265)].l42)+1;(l45[(l2)][(l119)].l39) = (
l45[(l2)][(l119)].l38)-1;(l45[(l2)][(l119)].l41) = 0;}else if(!l10){l250 = (
l45[(l2)][(l119)].l38);if((l133 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l250,l15))<
0){return l133;}l255 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,
L3_DEFIPm)),(l15),(l95[(l2)]->l75));l256 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(l2)]->l76));if((l255 == 0)||(l256 == 
0)){*l261 = (l250<<1)+((l256 == 0)?1:0);return(SOC_E_NONE);}l250 = (l45[(l2)]
[(l119)].l39);if((l133 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l250,l15))<0){return
l133;}l255 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),
(l15),(l95[(l2)]->l75));l256 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l15),(l95[(l2)]->l76));if((l255 == 0)||(l256 == 
0)){*l261 = (l250<<1)+((l256 == 0)?1:0);return(SOC_E_NONE);}}l264 = l119;
while((l45[(l2)][(l264)].l42) == 0){l264 = (l45[(l2)][(l264)].next);if(l264 == 
-1){l264 = l119;break;}}while((l45[(l2)][(l264)].l42) == 0){l264 = (l45[(l2)]
[(l264)].l40);if(l264 == -1){if((l45[(l2)][(l119)].l41) == 0){l262 = (l45[(l2
)][(l119)].l40);l263 = (l45[(l2)][(l119)].next);if(-1!= l262){(l45[(l2)][(
l262)].next) = l263;}if(-1!= l263){(l45[(l2)][(l263)].l40) = l262;}}return(
SOC_E_FULL);}}while(l264>l119){l263 = (l45[(l2)][(l264)].next);
SOC_IF_ERROR_RETURN(l258(l2,l263,l10));(l45[(l2)][(l264)].l42)-= 1;(l45[(l2)]
[(l263)].l42)+= 1;l264 = l263;}while(l264<l119){SOC_IF_ERROR_RETURN(l254(l2,
l264,l10));(l45[(l2)][(l264)].l42)-= 1;l262 = (l45[(l2)][(l264)].l40);(l45[(
l2)][(l262)].l42)+= 1;l264 = l262;}(l45[(l2)][(l119)].l41)+= 1;(l45[(l2)][(
l119)].l42)-= 1;(l45[(l2)][(l119)].l39)+= 1;*l261 = (l45[(l2)][(l119)].l39)<<
((l10)?0:1);sal_memcpy(l15,soc_mem_entry_null(l2,L3_DEFIPm),
soc_mem_entry_words(l2,L3_DEFIPm)*4);return(SOC_E_NONE);}static int l266(int
l2,int l119,int l10,void*l15,int l267){int l262;int l263;int l250;int l251;
uint32 l268[SOC_MAX_MEM_FIELD_WORDS];uint32 l269[SOC_MAX_MEM_FIELD_WORDS];
uint32 l270[SOC_MAX_MEM_FIELD_WORDS];void*l271;int l133;int l272,l147;l250 = 
(l45[(l2)][(l119)].l39);l251 = l267;if(!l10){l251>>= 1;if((l133 = 
READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l250,l268))<0){return l133;}if((l133 = 
READ_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ANY,soc_alpm_physical_idx(l2,L3_DEFIPm,
l250,1),l269))<0){return l133;}if((l133 = READ_L3_DEFIP_AUX_TABLEm(l2,
MEM_BLOCK_ANY,soc_alpm_physical_idx(l2,L3_DEFIPm,l251,1),l270))<0){return l133
;}l271 = (l251 == l250)?l268:l15;if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(l268),(l95[(l2)]->l76))){l147 = 
soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,l269,BPM_LENGTH1f);if(l267&1){l133
= soc_alpm_lpm_ip4entry1_to_1(l2,l268,l271,PRESERVE_HIT);soc_mem_field32_set(
l2,L3_DEFIP_AUX_TABLEm,l270,BPM_LENGTH1f,l147);}else{l133 = 
soc_alpm_lpm_ip4entry1_to_0(l2,l268,l271,PRESERVE_HIT);soc_mem_field32_set(l2
,L3_DEFIP_AUX_TABLEm,l270,BPM_LENGTH0f,l147);}l272 = (l250<<1)+1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l268),(l95[(
l2)]->l76),(0));}else{l147 = soc_mem_field32_get(l2,L3_DEFIP_AUX_TABLEm,l269,
BPM_LENGTH0f);if(l267&1){l133 = soc_alpm_lpm_ip4entry0_to_1(l2,l268,l271,
PRESERVE_HIT);soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,l270,BPM_LENGTH1f,
l147);}else{l133 = soc_alpm_lpm_ip4entry0_to_0(l2,l268,l271,PRESERVE_HIT);
soc_mem_field32_set(l2,L3_DEFIP_AUX_TABLEm,l270,BPM_LENGTH0f,l147);}l272 = 
l250<<1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l268
),(l95[(l2)]->l75),(0));(l45[(l2)][(l119)].l41)-= 1;(l45[(l2)][(l119)].l42)+= 
1;(l45[(l2)][(l119)].l39)-= 1;}l272 = soc_alpm_physical_idx(l2,L3_DEFIPm,l272
,0);l267 = soc_alpm_physical_idx(l2,L3_DEFIPm,l267,0);ALPM_TCAM_PIVOT(l2,l267
) = ALPM_TCAM_PIVOT(l2,l272);ALPM_TCAM_PIVOT(l2,l272) = NULL;if((l133 = 
WRITE_L3_DEFIP_AUX_TABLEm(l2,MEM_BLOCK_ALL,soc_alpm_physical_idx(l2,L3_DEFIPm
,l251,1),l270))<0){return l133;}if(l251!= l250){l229(l2,l271,l251,0x4000,0);
if((l133 = l157(l2,MEM_BLOCK_ANY,l251,l251,l271))<0){return l133;}if((l133 = 
l244(l2,l251,l271))<0){return l133;}}l229(l2,l268,l250,0x4000,0);if((l133 = 
l157(l2,MEM_BLOCK_ANY,l250,l250,l268))<0){return l133;}if((l133 = l244(l2,
l250,l268))<0){return l133;}}else{(l45[(l2)][(l119)].l41)-= 1;(l45[(l2)][(
l119)].l42)+= 1;(l45[(l2)][(l119)].l39)-= 1;if(l251!= l250){if((l133 = 
READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,l250,l268))<0){return l133;}l229(l2,l268,l251
,0x4000,0);if((l133 = l157(l2,MEM_BLOCK_ANY,l251,l250,l268))<0){return l133;}
if((l133 = l244(l2,l251,l268))<0){return l133;}}l267 = soc_alpm_physical_idx(
l2,L3_DEFIPm,l251,1);l272 = soc_alpm_physical_idx(l2,L3_DEFIPm,l250,1);
ALPM_TCAM_PIVOT(l2,l267<<1) = ALPM_TCAM_PIVOT(l2,l272<<1);ALPM_TCAM_PIVOT(l2,
l272<<1) = NULL;sal_memcpy(l268,soc_mem_entry_null(l2,L3_DEFIPm),
soc_mem_entry_words(l2,L3_DEFIPm)*4);l229(l2,l268,l250,0x4000,0);if((l133 = 
l157(l2,MEM_BLOCK_ANY,l250,l250,l268))<0){return l133;}if((l133 = l244(l2,
l250,l268))<0){return l133;}}if((l45[(l2)][(l119)].l41) == 0){l262 = (l45[(l2
)][(l119)].l40);assert(l262!= -1);l263 = (l45[(l2)][(l119)].next);(l45[(l2)][
(l262)].next) = l263;(l45[(l2)][(l262)].l42)+= (l45[(l2)][(l119)].l42);(l45[(
l2)][(l119)].l42) = 0;if(l263!= -1){(l45[(l2)][(l263)].l40) = l262;}(l45[(l2)
][(l119)].next) = -1;(l45[(l2)][(l119)].l40) = -1;(l45[(l2)][(l119)].l38) = -
1;(l45[(l2)][(l119)].l39) = -1;}return(l133);}int soc_alpm_lpm_vrf_get(int l21
,void*lpm_entry,int*l28,int*l273){int l139;if(((l95[(l21)]->l79)!= NULL)){
l139 = soc_L3_DEFIPm_field32_get(l21,lpm_entry,VRF_ID_0f);*l273 = l139;if(
soc_L3_DEFIPm_field32_get(l21,lpm_entry,VRF_ID_MASK0f)){*l28 = l139;}else if(
!soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l21,L3_DEFIPm)),(lpm_entry)
,(l95[(l21)]->l81))){*l28 = SOC_L3_VRF_GLOBAL;*l273 = SOC_VRF_MAX(l21)+1;}
else{*l28 = SOC_L3_VRF_OVERRIDE;}}else{*l28 = SOC_L3_VRF_DEFAULT;}return(
SOC_E_NONE);}static int l274(int l2,void*entry,int*l17){int l119;int l133;int
l10;uint32 l241;int l139;int l275;l10 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l95[(l2)]->l65));if(l10){l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l95[
(l2)]->l63));if((l133 = _ipmask2pfx(l241,&l119))<0){return(l133);}l241 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry),(l95[
(l2)]->l64));if(l119){if(l241!= 0xffffffff){return(SOC_E_PARAM);}l119+= 32;}
else{if((l133 = _ipmask2pfx(l241,&l119))<0){return(l133);}}l119+= 33;}else{
l241 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(entry
),(l95[(l2)]->l63));if((l133 = _ipmask2pfx(l241,&l119))<0){return(l133);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l2,entry,&l139,&l133));l275 = 
soc_alpm_mode_get(l2);switch(l139){case SOC_L3_VRF_GLOBAL:if(l275 == 
SOC_ALPM_MODE_PARALLEL){*l17 = l119+((3*(64+32+2+1))/3);}else{*l17 = l119;}
break;case SOC_L3_VRF_OVERRIDE:*l17 = l119+2*((3*(64+32+2+1))/3);break;
default:if(l275 == SOC_ALPM_MODE_PARALLEL){*l17 = l119;}else{*l17 = l119+((3*
(64+32+2+1))/3);}break;}return(SOC_E_NONE);}static int l14(int l2,void*l8,
void*l15,int*l16,int*l17,int*l10){int l133;int l34;int l120;int l119 = 0;l34 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l2,L3_DEFIPm)),(l8),(l95[(l2
)]->l65));if(l34){if(!(l34 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l2,L3_DEFIPm)),(l8),(l95[(l2)]->l66)))){return(SOC_E_PARAM);}}*l10 = l34;
l274(l2,l8,&l119);*l17 = l119;if(l233(l2,l8,l119,&l120) == SOC_E_NONE){*l16 = 
l120;if((l133 = READ_L3_DEFIPm(l2,MEM_BLOCK_ANY,(*l10)?*l16:(*l16>>1),l15))<0
){return l133;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}static int
l3(int l2){int l243;int l161;int l223;int l276;uint32 l277,l275;if(!
soc_feature(l2,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}if((l275 = 
soc_property_get(l2,spn_L3_ALPM_ENABLE,0))){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l2,&l277));soc_reg_field_set(l2,
L3_DEFIP_RPF_CONTROLr,&l277,LPM_MODEf,1);if(l275 == SOC_ALPM_MODE_PARALLEL){
soc_reg_field_set(l2,L3_DEFIP_RPF_CONTROLr,&l277,LOOKUP_MODEf,1);}else{
soc_reg_field_set(l2,L3_DEFIP_RPF_CONTROLr,&l277,LOOKUP_MODEf,0);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l2,l277));l277 = 0;if(
SOC_URPF_STATUS_GET(l2)){soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l277,
URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l277,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l277,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l2,L3_DEFIP_KEY_SELr,&l277,
URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l2,l277))
;l277 = 0;if(l275 == SOC_ALPM_MODE_PARALLEL){if(SOC_URPF_STATUS_GET(l2)){
soc_reg_field_set(l2,L3_DEFIP_ALPM_CFGr,&l277,TCAM2_SELf,1);soc_reg_field_set
(l2,L3_DEFIP_ALPM_CFGr,&l277,TCAM3_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM4_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM5_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM6_SELf,3);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM7_SELf,3);}else{soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM4_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM5_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM6_SELf,1);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l2)){
soc_reg_field_set(l2,L3_DEFIP_ALPM_CFGr,&l277,TCAM4_SELf,2);soc_reg_field_set
(l2,L3_DEFIP_ALPM_CFGr,&l277,TCAM5_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM6_SELf,2);soc_reg_field_set(l2,
L3_DEFIP_ALPM_CFGr,&l277,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l2,l277));if(soc_property_get(l2,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l278 = 0;if(l275!= SOC_ALPM_MODE_PARALLEL
){uint32 l279;l279 = soc_property_get(l2,spn_NUM_IPV6_LPM_128B_ENTRIES,2048);
if(l279!= 2048){if(SOC_URPF_STATUS_GET(l2)){soc_cm_print(
"URPF supported in combined mode only""with 2048 v6-128 entries\n");return
SOC_E_PARAM;}if((l279!= 1024)&&(l279!= 3072)){soc_cm_print(
"Only supported values for v6-128 in"
"nonURPF combined mode are 1024 and 3072\n");return SOC_E_PARAM;}}}
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_KEY_SELr(l2,&l278));soc_reg_field_set(l2,
L3_DEFIP_KEY_SELr,&l278,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l2,
L3_DEFIP_KEY_SELr,&l278,V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l2,
L3_DEFIP_KEY_SELr,&l278,V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l2,
L3_DEFIP_KEY_SELr,&l278,V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_KEY_SELr(l2,l278));}}l243 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(
l2);l276 = sizeof(l43)*(l243);if((l45[(l2)]!= NULL)){if(soc_alpm_deinit(l2)<0
){SOC_ALPM_LPM_UNLOCK(l2);return SOC_E_INTERNAL;}}l95[l2] = sal_alloc(sizeof(
l93),"lpm_field_state");if(NULL == l95[l2]){SOC_ALPM_LPM_UNLOCK(l2);return(
SOC_E_MEMORY);}(l95[l2])->l47 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,CLASS_ID0f
);(l95[l2])->l48 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,CLASS_ID1f);(l95[l2])->
l49 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,DST_DISCARD0f);(l95[l2])->l50 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DST_DISCARD1f);(l95[l2])->l51 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP0f);(l95[l2])->l52 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP1f);(l95[l2])->l53 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_COUNT0f);(l95[l2])->l54 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_COUNT1f);(l95[l2])->l55 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_PTR0f);(l95[l2])->l56 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ECMP_PTR1f);(l95[l2])->l57 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_ROUTE0f);(l95[l2])->l58 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_ROUTE1f);(l95[l2])->l59 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,HIT0f);(l95[l2])->l60 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,HIT1f);(l95[l2])->l61 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR0f);(l95[l2])->l62 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR1f);(l95[l2])->l63 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR_MASK0f);(l95[l2])->l64 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,IP_ADDR_MASK1f);(l95[l2])->l65 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE0f);(l95[l2])->l66 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE1f);(l95[l2])->l67 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE_MASK0f);(l95[l2])->l68 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,MODE_MASK1f);(l95[l2])->l69 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,NEXT_HOP_INDEX0f);(l95[l2])->l70 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,NEXT_HOP_INDEX1f);(l95[l2])->l71 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,PRI0f);(l95[l2])->l72 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,PRI1f);(l95[l2])->l73 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,RPE0f);(l95[l2])->l74 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,RPE1f);(l95[l2])->l75 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VALID0f);(l95[l2])->l76 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VALID1f);(l95[l2])->l77 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_0f);(l95[l2])->l78 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_1f);(l95[l2])->l79 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_MASK0f);(l95[l2])->l80 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,VRF_ID_MASK1f);(l95[l2])->l81 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_HIGH0f);(l95[l2])->l82 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,GLOBAL_HIGH1f);(l95[l2])->l83 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_HIT_IDX0f);(l95[l2])->l84 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_HIT_IDX1f);(l95[l2])->l85 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_BKT_PTR0f);(l95[l2])->l86 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,ALG_BKT_PTR1f);(l95[l2])->l87 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DEFAULT_MISS0f);(l95[l2])->l88 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,DEFAULT_MISS1f);(l95[l2])->l89 = 
soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l95[l2])->
l90 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l95[l2
])->l91 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l95[l2]
)->l92 = soc_mem_fieldinfo_get(l2,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(l45[(l2)
]) = sal_alloc(l276,"LPM prefix info");if(NULL == (l45[(l2)])){sal_free(l95[
l2]);l95[l2] = NULL;SOC_ALPM_LPM_UNLOCK(l2);return(SOC_E_MEMORY);}sal_memset(
(l45[(l2)]),0,l276);for(l161 = 0;l161<l243;l161++){(l45[(l2)][(l161)].l38) = 
-1;(l45[(l2)][(l161)].l39) = -1;(l45[(l2)][(l161)].l40) = -1;(l45[(l2)][(l161
)].next) = -1;(l45[(l2)][(l161)].l41) = 0;(l45[(l2)][(l161)].l42) = 0;}l223 = 
soc_mem_index_count(l2,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l2)){l223>>= 1;}if(
l275 == SOC_ALPM_MODE_PARALLEL){(l45[(l2)][(((3*(64+32+2+1))-1))].l39) = (
l223>>1)-1;(l45[(l2)][(((((3*(64+32+2+1))/3)-1)))].l42) = l223>>1;(l45[(l2)][
((((3*(64+32+2+1))-1)))].l42) = (l223-(l45[(l2)][(((((3*(64+32+2+1))/3)-1)))]
.l42));}else{(l45[(l2)][((((3*(64+32+2+1))-1)))].l42) = l223;}if((l106[(l2)])
!= NULL){if(l114((l106[(l2)]))<0){SOC_ALPM_LPM_UNLOCK(l2);return
SOC_E_INTERNAL;}(l106[(l2)]) = NULL;}if(l112(l2,l223*2,l223,&(l106[(l2)]))<0)
{SOC_ALPM_LPM_UNLOCK(l2);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l2);return(
SOC_E_NONE);}static int l4(int l2){if(!soc_feature(l2,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l2);if((l106[(l2)])!= NULL){l114((
l106[(l2)]));(l106[(l2)]) = NULL;}if((l45[(l2)]!= NULL)){sal_free(l95[l2]);
l95[l2] = NULL;sal_free((l45[(l2)]));(l45[(l2)]) = NULL;}SOC_ALPM_LPM_UNLOCK(
l2);return(SOC_E_NONE);}static int l5(int l2,void*l6,int*l280){int l119;int
index;int l10;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int l133 = SOC_E_NONE;int
l281 = 0;sal_memcpy(l15,soc_mem_entry_null(l2,L3_DEFIPm),soc_mem_entry_words(
l2,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l2);l133 = l14(l2,l6,l15,&index,&l119,&l10
);if(l133 == SOC_E_NOT_FOUND){l133 = l260(l2,l119,l10,l15,&index);if(l133<0){
SOC_ALPM_LPM_UNLOCK(l2);return(l133);}}else{l281 = 1;}*l280 = index;if(l133 == 
SOC_E_NONE){if(!l10){if(index&1){l133 = soc_alpm_lpm_ip4entry0_to_1(l2,l6,l15
,PRESERVE_HIT);}else{l133 = soc_alpm_lpm_ip4entry0_to_0(l2,l6,l15,
PRESERVE_HIT);}if(l133<0){SOC_ALPM_LPM_UNLOCK(l2);return(l133);}l6 = (void*)
l15;index>>= 1;}l1(l2);soc_cm_debug(DK_L3|DK_SOCMEM,
"\nsoc_alpm_lpm_insert: %d %d\n",index,l119);if(!l281){l229(l2,l6,index,
0x4000,0);}l133 = l157(l2,MEM_BLOCK_ANY,index,index,l6);if(l133>= 0){l133 = 
l244(l2,index,l6);}}SOC_ALPM_LPM_UNLOCK(l2);return(l133);}static int l7(int l2
,void*l8){int l119;int index;int l10;uint32 l15[SOC_MAX_MEM_FIELD_WORDS];int
l133 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l2);l133 = l14(l2,l8,l15,&index,&l119,&
l10);if(l133 == SOC_E_NONE){soc_cm_debug(DK_L3|DK_SOCMEM,
"\nsoc_alpm_lpm_delete: %d %d\n",index,l119);l232(l2,l8,index);l133 = l266(l2
,l119,l10,l15,index);}l1(l2);SOC_ALPM_LPM_UNLOCK(l2);return(l133);}static int
l18(int l2,void*l8,void*l15,int*l16){int l119;int l133;int l10;
SOC_ALPM_LPM_LOCK(l2);l133 = l14(l2,l8,l15,l16,&l119,&l10);
SOC_ALPM_LPM_UNLOCK(l2);return(l133);}static int l9(int l21,void*l8,int l10,
int l11,int l12,defip_aux_scratch_entry_t*l13){uint32 l241;uint32 l282[4] = {
0,0,0,0};int l119 = 0;int l133 = SOC_E_NONE;l241 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,VALID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
VALIDf,l241);l241 = soc_mem_field32_get(l21,L3_DEFIPm,l8,MODE0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,MODEf,l241);l241 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ENTRY_TYPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ENTRY_TYPEf,0);l241 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,GLOBAL_ROUTE0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,GLOBAL_ROUTEf,l241);l241 = soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,ECMPf,l241);l241 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,ECMP_PTR0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,ECMP_PTRf,l241);l241 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,NEXT_HOP_INDEX0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,NEXT_HOP_INDEXf,l241);l241 = soc_mem_field32_get(l21,L3_DEFIPm,l8,PRI0f);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,PRIf,l241);l241 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,RPE0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,RPEf,l241);l241 =soc_mem_field32_get(l21,L3_DEFIPm,
l8,VRF_ID_0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,VRFf,l241);
soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,DB_TYPEf,l11);l241 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,DST_DISCARD0f);soc_mem_field32_set(l21,
L3_DEFIP_AUX_SCRATCHm,l13,DST_DISCARDf,l241);l241 = soc_mem_field32_get(l21,
L3_DEFIPm,l8,CLASS_ID0f);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
CLASS_IDf,l241);if(l10){l282[2] = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR0f);l282[3] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR1f);}else{
l282[0] = soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR0f);}soc_mem_field_set(
l21,L3_DEFIP_AUX_SCRATCHm,(uint32*)l13,IP_ADDRf,(uint32*)l282);if(l10){l241 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(
l241,&l119))<0){return(l133);}l241 = soc_mem_field32_get(l21,L3_DEFIPm,l8,
IP_ADDR_MASK1f);if(l119){if(l241!= 0xffffffff){return(SOC_E_PARAM);}l119+= 32
;}else{if((l133 = _ipmask2pfx(l241,&l119))<0){return(l133);}}}else{l241 = 
soc_mem_field32_get(l21,L3_DEFIPm,l8,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(
l241,&l119))<0){return(l133);}}soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,
l13,IP_LENGTHf,l119);soc_mem_field32_set(l21,L3_DEFIP_AUX_SCRATCHm,l13,
REPLACE_LENf,l12);return(SOC_E_NONE);}int _soc_alpm_aux_op(int l2,
_soc_aux_op_t l283,defip_aux_scratch_entry_t*l13,int*l140,int*l138,int*
bucket_index){uint32 l277,l284;int l285;soc_timeout_t l286;int l133 = 
SOC_E_NONE;int l287 = 0;SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_SCRATCHm(l2,
MEM_BLOCK_ANY,0,l13));l288:l277 = 0;switch(l283){case INSERT_PROPAGATE:l285 = 
0;break;case DELETE_PROPAGATE:l285 = 1;break;case PREFIX_LOOKUP:l285 = 2;
break;case HITBIT_REPLACE:l285 = 3;break;default:return SOC_E_PARAM;}
soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l277,OPCODEf,l285);soc_reg_field_set
(l2,L3_DEFIP_AUX_CTRLr,&l277,STARTf,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l2,l277));soc_timeout_init(&l286,50000,5);l285 = 0;
do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l2,&l277));l285 = 
soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l277,DONEf);if(l285 == 1){l133 = 
SOC_E_NONE;break;}if(soc_timeout_check(&l286)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l2,&l277));l285 = soc_reg_field_get(l2,
L3_DEFIP_AUX_CTRLr,l277,DONEf);if(l285 == 1){l133 = SOC_E_NONE;}else{
soc_cm_debug(DK_ERR,"unit %d : DEFIP AUX Operation timeout\n",l2);l133 = 
SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l133)){if(soc_reg_field_get(l2
,L3_DEFIP_AUX_CTRLr,l277,ERRORf)){soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&
l277,STARTf,0);soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l277,ERRORf,0);
soc_reg_field_set(l2,L3_DEFIP_AUX_CTRLr,&l277,DONEf,0);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l2,l277));soc_cm_debug(DK_WARN,
"unit %d: DEFIP AUX Operation encountered ""parity error !!\n",l2);l287++;if(
SOC_CONTROL(l2)->alpm_bulk_retry){sal_sem_take(SOC_CONTROL(l2)->
alpm_bulk_retry,1000000);}if(l287<5){soc_cm_debug(DK_WARN,
"unit %d: Retry DEFIP AUX Operation..\n",l2);goto l288;}else{soc_cm_debug(
DK_ERR,"unit %d: Aborting DEFIP AUX Operation "
"due to un-correctable error !!\n",l2);return SOC_E_INTERNAL;}}if(l283 == 
PREFIX_LOOKUP){if(l140&&l138){*l140 = soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr
,l277,HITf);*l138 = soc_reg_field_get(l2,L3_DEFIP_AUX_CTRLr,l277,BKT_INDEXf);
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRL_1r(l2,&l284));*bucket_index = 
soc_reg_field_get(l2,L3_DEFIP_AUX_CTRL_1r,l284,BKT_PTRf);}}}return l133;}
static int l20(int l21,void*lpm_entry,void*l22,void*l23,soc_mem_t l24,uint32
l25,uint32*l289){uint32 l241;uint32 l282[4] = {0,0};int l119 = 0;int l133 = 
SOC_E_NONE;int l10;uint32 l26 = 0;l10 = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,MODE0f);sal_memset(l22,0,soc_mem_entry_bytes(l21,l24));l241 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,HIT0f);soc_mem_field32_set(l21,
l24,l22,HITf,l241);l241 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,VALID0f
);soc_mem_field32_set(l21,l24,l22,VALIDf,l241);l241 = soc_mem_field32_get(l21
,L3_DEFIPm,lpm_entry,ECMP0f);soc_mem_field32_set(l21,l24,l22,ECMPf,l241);l241
= soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set
(l21,l24,l22,ECMP_PTRf,l241);l241 = soc_mem_field32_get(l21,L3_DEFIPm,
lpm_entry,NEXT_HOP_INDEX0f);soc_mem_field32_set(l21,l24,l22,NEXT_HOP_INDEXf,
l241);l241 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,PRI0f);
soc_mem_field32_set(l21,l24,l22,PRIf,l241);l241 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,RPE0f);soc_mem_field32_set(l21,l24,l22,RPEf,l241);l241 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(l21,l24,l22,DST_DISCARDf,l241);l241 = soc_mem_field32_get
(l21,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);soc_mem_field32_set(l21,l24,l22,
SRC_DISCARDf,l241);l241 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
CLASS_ID0f);soc_mem_field32_set(l21,l24,l22,CLASS_IDf,l241);l282[0] = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR0f);if(l10){l282[1] = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR1f);}soc_mem_field_set(l21
,l24,(uint32*)l22,KEYf,(uint32*)l282);if(l10){l241 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l133 = _ipmask2pfx(l241,&l119))<0){
return(l133);}l241 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f);if(l119){if(l241!= 0xffffffff){return(SOC_E_PARAM);}l119+= 32
;}else{if((l133 = _ipmask2pfx(l241,&l119))<0){return(l133);}}}else{l241 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l133 = 
_ipmask2pfx(l241,&l119))<0){return(l133);}}if((l119 == 0)&&(l282[0] == 0)&&(
l282[1] == 0)){l26 = 1;}if(l289!= NULL){*l289 = l26;}soc_mem_field32_set(l21,
l24,l22,LENGTHf,l119);if(l23 == NULL){return(SOC_E_NONE);}if(
SOC_URPF_STATUS_GET(l21)){sal_memset(l23,0,soc_mem_entry_bytes(l21,l24));
sal_memcpy(l23,l22,soc_mem_entry_bytes(l21,l24));soc_mem_field32_set(l21,l24,
l23,DST_DISCARDf,0);soc_mem_field32_set(l21,l24,l23,RPEf,0);
soc_mem_field32_set(l21,l24,l23,SRC_DISCARDf,l25&SOC_ALPM_RPF_SRC_DISCARD);
soc_mem_field32_set(l21,l24,l23,DEFAULTROUTEf,l26);}return(SOC_E_NONE);}
static int l27(int l21,void*l22,soc_mem_t l24,int l10,int l28,int l29,int
index,void*lpm_entry){uint32 l241;uint32 l282[4] = {0,0};uint32 l119 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_bytes(l21,L3_DEFIPm));l241 = 
soc_mem_field32_get(l21,l24,l22,HITf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,HIT0f,l241);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
HIT1f,l241);}l241 = soc_mem_field32_get(l21,l24,l22,VALIDf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VALID0f,l241);if(l10){
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VALID1f,l241);}l241 = 
soc_mem_field32_get(l21,l24,l22,ECMPf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,ECMP0f,l241);l241 = soc_mem_field32_get(l21,l24,l22,ECMP_PTRf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ECMP_PTR0f,l241);l241 = 
soc_mem_field32_get(l21,l24,l22,NEXT_HOP_INDEXf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f,l241);l241 = soc_mem_field32_get(l21,l24
,l22,PRIf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,PRI0f,l241);l241 = 
soc_mem_field32_get(l21,l24,l22,RPEf);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,RPE0f,l241);l241 = soc_mem_field32_get(l21,l24,l22,DST_DISCARDf);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,DST_DISCARD0f,l241);l241 = 
soc_mem_field32_get(l21,l24,l22,SRC_DISCARDf);soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,SRC_DISCARD0f,l241);l241 = soc_mem_field32_get(l21,l24,
l22,CLASS_IDf);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,CLASS_ID0f,l241);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ALG_BKT_PTR0f,l29);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ALG_HIT_IDX0f,index);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE_MASK0f,3);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l10){
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(l21,
l24,l22,KEYf,l282);if(l10){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR1f,l282[1]);}soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR0f,
l282[0]);l241 = soc_mem_field32_get(l21,l24,l22,LENGTHf);if(l10){if(l241>= 32
){l119 = 0xffffffff;soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l119);l119 = ~(((l241-32) == 32)?0:(0xffffffff)>>(l241-32));
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l119);}else{l119 = 
~(0xffffffff>>l241);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l119);}}else{assert(l241<= 32);l119 = ~(((l241) == 32)?0:(
0xffffffff)>>(l241));soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,l119);}if(l28 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l21,
L3_DEFIPm,lpm_entry,GLOBAL_HIGH0f,1);soc_mem_field32_set(l21,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else if(l28 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry
,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,0);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_0f,l28);
soc_mem_field32_set(l21,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(l21));}
return(SOC_E_NONE);}int soc_alpm_warmboot_pivot_add(int l21,int l10,void*
lpm_entry,int l290,int l291){int l133 = SOC_E_NONE;int l179;uint32 key[2] = {
0,0};alpm_pivot_t*l180 = NULL;alpm_bucket_handle_t*l189 = NULL;uint32 l200=0;
l290 = soc_alpm_physical_idx(l21,L3_DEFIPm,l290,l10);if(l10){l200 = 
soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l133 = 
_ipmask2pfx(l200,&l179))<0){return(l133);}l200 = soc_mem_field32_get(l21,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f);if(l179){if(l200!= 0xffffffff){return(
SOC_E_PARAM);}l179+= 32;}else{if((l133 = _ipmask2pfx(l200,&l179))<0){return(
l133);}}}else{l200= soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR_MASK0f);
if((l133 = _ipmask2pfx(l200,&l179))<0){return(l133);}}l189 = sal_alloc(sizeof
(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l189 == NULL){soc_cm_debug(
DK_ERR,"Unable to allocate memory for ""PIVOT trie node \n");return SOC_E_NONE
;}sal_memset(l189,0,sizeof(*l189));l180 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l180 == NULL){soc_cm_debug(DK_ERR,
"Unable to allocate memory for ""PIVOT trie node \n");sal_free(l189);return
SOC_E_MEMORY;}sal_memset(l180,0,sizeof(*l180));PIVOT_BUCKET_HANDLE(l180) = 
l189;if(l10){trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l180));key[0] = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR0f);key[1] = 
soc_L3_DEFIPm_field32_get(l21,lpm_entry,IP_ADDR1f);}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l180));key[0] = soc_L3_DEFIPm_field32_get
(l21,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l180) = l291;PIVOT_TCAM_INDEX(
l180) = l290;l180->key[0] = key[0];l180->key[1] = key[1];l180->len = l179;
ALPM_TCAM_PIVOT(l21,l290) = l180;return l133;}static int l292(int l21,int l10
,void*lpm_entry,void*l22,soc_mem_t l24,int l290,int l291,int l293){int l294;
int l28;int l133 = SOC_E_NONE;int l26 = 0;uint32 prefix[5] = {0,0,0,0,0};
uint32 l179;void*l295 = NULL;trie_t*l296 = NULL;trie_t*l182 = NULL;
trie_node_t*l184 = NULL;payload_t*l297 = NULL;payload_t*l187 = NULL;
alpm_pivot_t*l175 = NULL;if((NULL == lpm_entry)||(NULL == l22)){return
SOC_E_PARAM;}if(l10){if(!(l10 = soc_mem_field32_get(l21,L3_DEFIPm,lpm_entry,
MODE1f))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l21,
lpm_entry,&l294,&l28));l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m
;l295 = sal_alloc(sizeof(defip_entry_t),"Temp lpm_entr");if(NULL == l295){
return SOC_E_MEMORY;}SOC_IF_ERROR_RETURN(l27(l21,l22,l24,l10,l294,l291,l290,
l295));l133 = l130(l21,l295,prefix,&l179,&l26);if(SOC_FAILURE(l133)){
soc_cm_debug(DK_ERR,"prefix create failed\n");return l133;}sal_free(l295);
l175 = ALPM_TCAM_PIVOT(l21,l290);l296 = PIVOT_BUCKET_TRIE(l175);l297 = 
sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == l297){soc_cm_debug(
DK_ERR,"Unable to allocate memory for ""trie node.\n");return SOC_E_MEMORY;}
l187 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == 
l187){soc_cm_debug(DK_ERR,"Unable to allocate memory for ""pfx trie node\n");
sal_free(l297);return SOC_E_MEMORY;}sal_memset(l297,0,sizeof(*l297));
sal_memset(l187,0,sizeof(*l187));l297->key[0] = prefix[0];l297->key[1] = 
prefix[1];l297->key[2] = prefix[2];l297->key[3] = prefix[3];l297->key[4] = 
prefix[4];l297->len = l179;l297->index = l293;sal_memcpy(l187,l297,sizeof(*
l297));l133 = trie_insert(l296,prefix,NULL,l179,(trie_node_t*)l297);if(
SOC_FAILURE(l133)){goto l298;}if(l10){l182 = VRF_PREFIX_TRIE_IPV6(l21,l28);}
else{l182 = VRF_PREFIX_TRIE_IPV4(l21,l28);}if(!l26){l133 = trie_insert(l182,
prefix,NULL,l179,(trie_node_t*)l187);if(SOC_FAILURE(l133)){goto l194;}}return
l133;l194:(void)trie_delete(l296,prefix,l179,&l184);l297 = (payload_t*)l184;
l298:sal_free(l297);sal_free(l187);return l133;}static int l299(int l21,int
l34,int l28,int l126,int bkt_ptr){int l133 = SOC_E_NONE;uint32 l179;uint32 key
[2] = {0,0};trie_t*l300 = NULL;payload_t*l209 = NULL;defip_entry_t*lpm_entry = 
NULL;lpm_entry = sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(
lpm_entry == NULL){soc_cm_debug(DK_ERR,"unable to allocate memory for "
"LPM entry\n");return SOC_E_MEMORY;}l30(l21,key,0,l28,l34,lpm_entry,0,1);if(
l28 == SOC_VRF_MAX(l21)+1){soc_L3_DEFIPm_field32_set(l21,lpm_entry,
GLOBAL_ROUTE0f,1);}else{soc_L3_DEFIPm_field32_set(l21,lpm_entry,
DEFAULT_MISS0f,1);}soc_L3_DEFIPm_field32_set(l21,lpm_entry,ALG_BKT_PTR0f,
bkt_ptr);if(l34 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l21,l28) = lpm_entry;
trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l21,l28));l300 = 
VRF_PREFIX_TRIE_IPV4(l21,l28);}else{VRF_TRIE_DEFAULT_ROUTE_IPV6(l21,l28) = 
lpm_entry;trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l21,l28));l300 = 
VRF_PREFIX_TRIE_IPV6(l21,l28);}l209 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l209 == NULL){soc_cm_debug(DK_ERR,
"Unable to allocate memory for ""pfx trie node \n");return SOC_E_MEMORY;}
sal_memset(l209,0,sizeof(*l209));l179 = 0;l209->key[0] = key[0];l209->key[1] = 
key[1];l209->len = l179;l133 = trie_insert(l300,key,NULL,l179,&(l209->node));
if(SOC_FAILURE(l133)){sal_free(l209);return l133;}VRF_TRIE_INIT_DONE(l21,l28,
l34,1);return l133;}int soc_alpm_warmboot_prefix_insert(int l21,int l10,void*
lpm_entry,void*l22,int l290,int l291,int l293){int l294;int l28;int l133 = 
SOC_E_NONE;soc_mem_t l24;l290 = soc_alpm_physical_idx(l21,L3_DEFIPm,l290,l10)
;l24 = (l10)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l21,lpm_entry,&l294,&l28));if(l294 == 
SOC_L3_VRF_OVERRIDE){return(l133);}if(!VRF_TRIE_INIT_COMPLETED(l21,l28,l10)){
soc_cm_debug(DK_VERBOSE,"VRF %d is not ""initialized\n",l28);l133 = l299(l21,
l10,l28,l290,l291);if(SOC_FAILURE(l133)){soc_cm_debug(DK_ERR,
"VRF %d/%d trie init \n""failed\n",l28,l10);return l133;}soc_cm_debug(
DK_VERBOSE,"VRF %d/%d trie init ""completed\n",l28,l10);}l133 = l292(l21,l10,
lpm_entry,l22,l24,l290,l291,l293);if(l133!= SOC_E_NONE){soc_cm_debug(DK_WARN,
"unit %d : ""Route Insertion Failed :%s\n",l21,soc_errmsg(l133));return(l133)
;}VRF_TRIE_ROUTES_INC(l21,l28,l10);return(l133);}int
soc_alpm_warmboot_bucket_bitmap_set(int l2,int l34,int l220){int l224 = 1;if(
l34){if(!soc_alpm_mode_get(l2)&&!SOC_URPF_STATUS_GET(l2)){l224 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l2) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l2),l220,l224);return SOC_E_NONE;}int
soc_alpm_warmboot_lpm_reinit_done(int l21){int l126;int l301 = ((3*(64+32+2+1
))-1);int l223 = soc_mem_index_count(l21,L3_DEFIPm);if(SOC_URPF_STATUS_GET(
l21)){l223>>= 1;}if(!soc_alpm_mode_get(l21)){(l45[(l21)][(((3*(64+32+2+1))-1)
)].l40) = -1;for(l126 = ((3*(64+32+2+1))-1);l126>-1;l126--){if(-1 == (l45[(
l21)][(l126)].l38)){continue;}(l45[(l21)][(l126)].l40) = l301;(l45[(l21)][(
l301)].next) = l126;(l45[(l21)][(l301)].l42) = (l45[(l21)][(l126)].l38)-(l45[
(l21)][(l301)].l39)-1;l301 = l126;}(l45[(l21)][(l301)].next) = -1;(l45[(l21)]
[(l301)].l42) = l223-(l45[(l21)][(l301)].l39)-1;}else{(l45[(l21)][(((3*(64+32
+2+1))-1))].l40) = -1;for(l126 = ((3*(64+32+2+1))-1);l126>(((3*(64+32+2+1))-1
)/3);l126--){if(-1 == (l45[(l21)][(l126)].l38)){continue;}(l45[(l21)][(l126)]
.l40) = l301;(l45[(l21)][(l301)].next) = l126;(l45[(l21)][(l301)].l42) = (l45
[(l21)][(l126)].l38)-(l45[(l21)][(l301)].l39)-1;l301 = l126;}(l45[(l21)][(
l301)].next) = -1;(l45[(l21)][(l301)].l42) = l223-(l45[(l21)][(l301)].l39)-1;
l301 = (((3*(64+32+2+1))-1)/3);(l45[(l21)][((((3*(64+32+2+1))-1)/3))].l40) = 
-1;for(l126 = ((((3*(64+32+2+1))-1)/3)-1);l126>-1;l126--){if(-1 == (l45[(l21)
][(l126)].l38)){continue;}(l45[(l21)][(l126)].l40) = l301;(l45[(l21)][(l301)]
.next) = l126;(l45[(l21)][(l301)].l42) = (l45[(l21)][(l126)].l38)-(l45[(l21)]
[(l301)].l39)-1;l301 = l126;}(l45[(l21)][(l301)].next) = -1;(l45[(l21)][(l301
)].l42) = (l223>>1)-(l45[(l21)][(l301)].l39)-1;}return(SOC_E_NONE);}int
soc_alpm_warmboot_lpm_reinit(int l21,int l10,int l126,void*lpm_entry){int l17
;defip_entry_t*l302;if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l229(l21,lpm_entry,l126,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l274(l21,lpm_entry,&l17));if((l45[(l21)][(l17)].l41) == 0
){(l45[(l21)][(l17)].l38) = l126;(l45[(l21)][(l17)].l39) = l126;}else{(l45[(
l21)][(l17)].l39) = l126;}(l45[(l21)][(l17)].l41)++;if(l10){return(SOC_E_NONE
);}}else{if(soc_L3_DEFIPm_field32_get(l21,lpm_entry,VALID1f)){l302 = 
sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");soc_alpm_lpm_ip4entry1_to_0(
l21,lpm_entry,l302,TRUE);SOC_IF_ERROR_RETURN(l274(l21,l302,&l17));if((l45[(
l21)][(l17)].l41) == 0){(l45[(l21)][(l17)].l38) = l126;(l45[(l21)][(l17)].l39
) = l126;}else{(l45[(l21)][(l17)].l39) = l126;}sal_free(l302);(l45[(l21)][(
l17)].l41)++;}}return(SOC_E_NONE);}
#endif /* ALPM_ENABLE */
