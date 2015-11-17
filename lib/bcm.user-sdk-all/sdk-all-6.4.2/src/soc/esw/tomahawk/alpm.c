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
);static int l4(int l1,void*l5,int*index);static int l6(int l1,void*key_data,
int l7,int l8,int l9,defip_aux_scratch_entry_t*l10);static int l11(int l1,
void*key_data,void*l12,int*l13,int*l14,int*l7);static int l15(int l1,void*
key_data,void*l12,int*l13);static int l16(int l1);static int l17(int unit,
void*lpm_entry,void*l18,void*l19,soc_mem_t l20,uint32 l21,uint32*l22);static
int l23(int unit,void*l18,soc_mem_t l20,int l7,int l24,int l25,int index,void
*lpm_entry);static int l26(int unit,uint32*key,int len,int l24,int l7,
defip_entry_t*lpm_entry,int l27,int l28);static int l29(int l1,int l24,int l30
);static int l31(alpm_pfx_info_t*l32,trie_t*l33,uint32*l34,uint32 l35,
trie_node_t*l36,defip_entry_t*lpm_entry,uint32*l37);int
soc_th_alpm_lpm_vrf_get(int unit,void*lpm_entry,int*l24,int*l38);static int
l39(alpm_pfx_info_t*l32,int*l40,int*l13);int _soc_th_alpm_rollback_bkt_move(
int l1,void*key_data,soc_mem_t l20,alpm_pivot_t*l41,alpm_pivot_t*l42,
alpm_mem_prefix_array_t*l43,int*l44,int l45);int soc_th_alpm_lpm_delete(int l1
,void*key_data);void _soc_th_alpm_rollback_pivot_add(int l1,defip_entry_t*l46
,void*key_data,int tcam_index,alpm_pivot_t*pivot_pyld);typedef struct l47{int
l48;int l49;int l50;int next;int l51;int l52;}l53,*l54;static l54 l55[
SOC_MAX_NUM_DEVICES];typedef struct l56{soc_field_info_t*l57;soc_field_info_t
*l58;soc_field_info_t*l59;soc_field_info_t*l60;soc_field_info_t*l61;
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
soc_field_info_t*l101;soc_field_info_t*l102;}l103,*l104;static l104 l105[
SOC_MAX_NUM_DEVICES];typedef struct l106{int unit;int l107;int l108;uint16*
l109;uint16*l110;}l111;typedef uint32 l112[5];typedef int(*l113)(l112 l114,
l112 l115);static l111*l116[SOC_MAX_NUM_DEVICES];static void l117(int l1,void
*l12,int index,l112 l118);static uint16 l119(uint8*l120,int l121);static int
l122(int unit,int l107,int l108,l111**l123);static int l124(l111*l125);static
int l126(l111*l127,l113 l128,l112 entry,int l129,uint16*l130);static int l131
(l111*l127,l113 l128,l112 entry,int l129,uint16 l132,uint16 l44);static int
l133(l111*l127,l113 l128,l112 entry,int l129,uint16 l134);extern
alpm_vrf_handle_t*alpm_vrf_handle[SOC_MAX_NUM_DEVICES];extern alpm_pivot_t**
tcam_pivot[SOC_MAX_NUM_DEVICES];extern int l135(int unit);extern int
soc_th_get_alpm_banks(int unit);int soc_th_alpm_mode_get(int l1){uint32 l136;
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l1,&l136));return
soc_reg_field_get(l1,L3_DEFIP_RPF_CONTROLr,l136,LOOKUP_MODEf);}int
_soc_th_alpm_rpf_entry(int l1,int l137){int l138;l138 = (l137>>2)&0x3fff;l138
+= SOC_ALPM_BUCKET_COUNT(l1);return(l137&~(0x3fff<<2))|(l138<<2);}int
soc_th_alpm_physical_idx(int l1,soc_mem_t l20,int index,int l139){int l140 = 
index&1;if(l139){return soc_trident2_l3_defip_index_map(l1,l20,index);}index
>>= 1;index = soc_trident2_l3_defip_index_map(l1,l20,index);index<<= 1;index
|= l140;return index;}int soc_th_alpm_logical_idx(int l1,soc_mem_t l20,int
index,int l139){int l140 = index&1;if(l139){return
soc_trident2_l3_defip_index_remap(l1,l20,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_remap(l1,l20,index);index<<= 1;index|= l140;
return index;}static int l141(int l1,void*entry,uint32*prefix,uint32*l14,int*
l22){int l142,l143,l7;int l129 = 0;int l144 = SOC_E_NONE;uint32 l145,l140;
prefix[0] = prefix[1] = prefix[2] = prefix[3] = prefix[4] = 0;l7 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,MODE0f);l142 = soc_mem_field32_get(l1,
L3_DEFIPm,entry,IP_ADDR0f);l143 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK0f);prefix[1] = l142;l142 = soc_mem_field32_get(l1,L3_DEFIPm,
entry,IP_ADDR1f);l143 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK1f
);prefix[0] = l142;if(l7){prefix[4] = prefix[1];prefix[3] = prefix[0];prefix[
1] = prefix[0] = 0;l143 = soc_mem_field32_get(l1,L3_DEFIPm,entry,
IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l143,&l129))<0){return(l144);}l143 = 
soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK1f);if(l129){if(l143!= 
0xffffffff){return(SOC_E_PARAM);}l129+= 32;}else{if((l144 = _ipmask2pfx(l143,
&l129))<0){return(l144);}}l145 = 64-l129;if(l145<32){prefix[4]>>= l145;l140 = 
(((32-l145) == 32)?0:(prefix[3])<<(32-l145));prefix[3]>>= l145;prefix[4]|= 
l140;}else{prefix[4] = (((l145-32) == 32)?0:(prefix[3])>>(l145-32));prefix[3]
= 0;}}else{l143 = soc_mem_field32_get(l1,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((
l144 = _ipmask2pfx(l143,&l129))<0){return(l144);}prefix[1] = (((32-l129) == 
32)?0:(prefix[1])>>(32-l129));prefix[0] = 0;}*l14 = l129;*l22 = (prefix[0] == 
0)&&(prefix[1] == 0)&&(l129 == 0);return SOC_E_NONE;}int
_soc_th_alpm_find_in_bkt(int l1,soc_mem_t l20,int bucket_index,int l146,
uint32*l12,void*alpm_data,int*l130,int l30){int l144;l144 = 
soc_mem_alpm_lookup(l1,l20,bucket_index,MEM_BLOCK_ANY,l146,l12,alpm_data,l130
);if(SOC_SUCCESS(l144)){return l144;}if(SOC_ALPM_V6_SCALE_CHECK(l1,l30)){
return soc_mem_alpm_lookup(l1,l20,bucket_index+1,MEM_BLOCK_ANY,l146,l12,
alpm_data,l130);}return l144;}static int l147(int l1,uint32*prefix,uint32 l35
,int l30,int l24,int*l148,int*tcam_index,int*bucket_index){int l144 = 
SOC_E_NONE;trie_t*l149;trie_node_t*l150 = NULL;alpm_pivot_t*pivot_pyld;if(l30
){l149 = VRF_PIVOT_TRIE_IPV6(l1,l24);}else{l149 = VRF_PIVOT_TRIE_IPV4(l1,l24)
;}l144 = trie_find_lpm(l149,prefix,l35,&l150);if(SOC_FAILURE(l144)){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Pivot find failed\n")));return l144;}
pivot_pyld = (alpm_pivot_t*)l150;*l148 = 1;*tcam_index = PIVOT_TCAM_INDEX(
pivot_pyld);*bucket_index = PIVOT_BUCKET_INDEX(pivot_pyld);return SOC_E_NONE;
}static int l151(int l1,void*key_data,soc_mem_t l20,void*alpm_data,int*
tcam_index,int*bucket_index,int*l13){uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int
l152,l24,l30;int l130;uint32 l146;int l144 = SOC_E_NONE;int l148 = 0;l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);if(l30){if(!(l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l152,&l24));if(l152
== 0){if(soc_th_alpm_mode_get(l1)){return SOC_E_PARAM;}}if(l24 == SOC_VRF_MAX
(l1)+1){if(soc_th_get_alpm_banks(l1)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l146);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l146);}}else{if(soc_th_get_alpm_banks(l1)
<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,l146);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l146);}}if(!(((l152 == SOC_L3_VRF_OVERRIDE)
||((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)&&(l152 == 
SOC_L3_VRF_GLOBAL))))){uint32 prefix[5],l35;int l22 = 0;l144 = l141(l1,
key_data,prefix,&l35,&l22);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: prefix create failed\n")));return l144;}l144
= l147(l1,prefix,l35,l30,l24,&l148,tcam_index,bucket_index);
SOC_IF_ERROR_RETURN(l144);if(l148){l17(l1,key_data,l12,0,l20,0,0);l144 = 
_soc_th_alpm_find_in_bkt(l1,l20,*bucket_index,l146,l12,alpm_data,&l130,l30);
if(SOC_SUCCESS(l144)){*l13 = l130;}}else{l144 = SOC_E_NOT_FOUND;}}return l144
;}static int l153(int l1,void*key_data,void*alpm_data,void*alpm_sip_data,
soc_mem_t l20,int l130){defip_aux_scratch_entry_t l10;int l152,l30,l24;int
bucket_index;uint32 l8,l146;int l144 = SOC_E_NONE;int l148 = 0,l140 = 0;int
tcam_index;l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);if(l30){if
(!(l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l152,
&l24));if(l24 == SOC_VRF_MAX(l1)+1){l8 = 0;if(soc_th_get_alpm_banks(l1)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l146);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l146);}}else{l8 = 2;if(
soc_th_get_alpm_banks(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,
l146);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l146);}}if(!soc_th_alpm_mode_get
(l1)){l8 = 2;}if(l152!= SOC_L3_VRF_OVERRIDE){sal_memset(&l10,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(l1,key_data,l30,l8,0,&l10)
);SOC_IF_ERROR_RETURN(soc_mem_write(l1,l20,MEM_BLOCK_ANY,l130,alpm_data));if(
SOC_URPF_STATUS_GET(l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,l20,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l130),alpm_sip_data));if(l144!= 
SOC_E_NONE){return SOC_E_FAIL;}}l140 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,REPLACE_LENf,l140);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l148,&tcam_index,&
bucket_index));if(SOC_URPF_STATUS_GET(l1)){l140 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf);l140+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf,l140);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l148,&tcam_index,&
bucket_index));}}return l144;}static int l154(int l1,int l40,int l30,int l37)
{int l144,l140,index;defip_aux_table_entry_t entry;index = l40>>(l30?0:1);
l144 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l144);if(l30){soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,
&entry,BPM_LENGTH0f,l37);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH1f,l37);l140 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE0f);}else{if(l40&1){soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH1f,l37);l140 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE1f);}else{soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,
BPM_LENGTH0f,l37);l140 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&entry,
DB_TYPE0f);}}l144 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,
&entry);SOC_IF_ERROR_RETURN(l144);if(SOC_URPF_STATUS_GET(l1)){l140++;if(l30){
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l37);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l37);}else{if(
l40&1){soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l37);}
else{soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l37);}}
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l140);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l140);index+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l144 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);}
return l144;}static int l155(int l1,int l156,void*entry,void*l157,int l158){
uint32 l140,l143,l30,l8,l159 = 0;soc_mem_t l20 = L3_DEFIPm;soc_mem_t l160 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l161;int l144 = SOC_E_NONE,l129,l162,l24,
l163;SOC_IF_ERROR_RETURN(soc_mem_read(l1,l160,MEM_BLOCK_ANY,l156,l157));l140 = 
soc_mem_field32_get(l1,l20,entry,VRF_ID_0f);soc_mem_field32_set(l1,l160,l157,
VRF0f,l140);l140 = soc_mem_field32_get(l1,l20,entry,VRF_ID_1f);
soc_mem_field32_set(l1,l160,l157,VRF1f,l140);l140 = soc_mem_field32_get(l1,
l20,entry,MODE0f);soc_mem_field32_set(l1,l160,l157,MODE0f,l140);l140 = 
soc_mem_field32_get(l1,l20,entry,MODE1f);soc_mem_field32_set(l1,l160,l157,
MODE1f,l140);l30 = l140;l140 = soc_mem_field32_get(l1,l20,entry,VALID0f);
soc_mem_field32_set(l1,l160,l157,VALID0f,l140);l140 = soc_mem_field32_get(l1,
l20,entry,VALID1f);soc_mem_field32_set(l1,l160,l157,VALID1f,l140);l140 = 
soc_mem_field32_get(l1,l20,entry,IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l140,
&l129))<0){return l144;}l143 = soc_mem_field32_get(l1,l20,entry,
IP_ADDR_MASK1f);if((l144 = _ipmask2pfx(l143,&l162))<0){return l144;}if(l30){
soc_mem_field32_set(l1,l160,l157,IP_LENGTH0f,l129+l162);soc_mem_field32_set(
l1,l160,l157,IP_LENGTH1f,l129+l162);}else{soc_mem_field32_set(l1,l160,l157,
IP_LENGTH0f,l129);soc_mem_field32_set(l1,l160,l157,IP_LENGTH1f,l162);}l140 = 
soc_mem_field32_get(l1,l20,entry,IP_ADDR0f);soc_mem_field32_set(l1,l160,l157,
IP_ADDR0f,l140);l140 = soc_mem_field32_get(l1,l20,entry,IP_ADDR1f);
soc_mem_field32_set(l1,l160,l157,IP_ADDR1f,l140);l140 = soc_mem_field32_get(
l1,l20,entry,ENTRY_TYPE0f);soc_mem_field32_set(l1,l160,l157,ENTRY_TYPE0f,l140
);l140 = soc_mem_field32_get(l1,l20,entry,ENTRY_TYPE1f);soc_mem_field32_set(
l1,l160,l157,ENTRY_TYPE1f,l140);if(!l30){sal_memcpy(&l161,entry,sizeof(l161))
;l144 = soc_th_alpm_lpm_vrf_get(l1,(void*)&l161,&l24,&l129);
SOC_IF_ERROR_RETURN(l144);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_ip4entry1_to_0(
l1,&l161,&l161,PRESERVE_HIT));l144 = soc_th_alpm_lpm_vrf_get(l1,(void*)&l161,
&l163,&l129);SOC_IF_ERROR_RETURN(l144);}else{l144 = soc_th_alpm_lpm_vrf_get(
l1,entry,&l24,&l129);}if(SOC_URPF_STATUS_GET(l1)){if(l158>= (
soc_mem_index_count(l1,L3_DEFIPm)>>1)){l159 = 1;}}switch(l24){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l1,l160,l157,VALID0f,0);l8 = 0;break;
case SOC_L3_VRF_GLOBAL:l8 = l159?1:0;break;default:l8 = l159?3:2;break;}
soc_mem_field32_set(l1,l160,l157,DB_TYPE0f,l8);if(!l30){switch(l163){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l1,l160,l157,VALID1f,0);l8 = 0;break;
case SOC_L3_VRF_GLOBAL:l8 = l159?1:0;break;default:l8 = l159?3:2;break;}
soc_mem_field32_set(l1,l160,l157,DB_TYPE1f,l8);}else{if(l24 == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l1,l160,l157,VALID1f,0);}
soc_mem_field32_set(l1,l160,l157,DB_TYPE1f,l8);}if(l159){l140 = 
soc_mem_field32_get(l1,l20,entry,ALG_BKT_PTR0f);if(l140){l140+= 
SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l20,entry,ALG_BKT_PTR0f,l140
);}if(!l30){l140 = soc_mem_field32_get(l1,l20,entry,ALG_BKT_PTR1f);if(l140){
l140+= SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l20,entry,
ALG_BKT_PTR1f,l140);}}}return SOC_E_NONE;}static int l164(int l1,int l165,int
index,int l166,void*entry){defip_aux_table_entry_t l157;l166 = 
soc_th_alpm_physical_idx(l1,L3_DEFIPm,l166,1);SOC_IF_ERROR_RETURN(l155(l1,
l166,entry,(void*)&l157,index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l1,
MEM_BLOCK_ANY,index,entry));index = soc_th_alpm_physical_idx(l1,L3_DEFIPm,
index,1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,index
,&l157));return SOC_E_NONE;}int _soc_th_alpm_insert_in_bkt(int l1,soc_mem_t
l20,int bucket_index,int l146,void*alpm_data,uint32*l12,int*l130,int l30){int
l144;l144 = soc_mem_alpm_insert(l1,l20,bucket_index,MEM_BLOCK_ANY,l146,
alpm_data,l12,l130);if(l144 == SOC_E_FULL){if(SOC_ALPM_V6_SCALE_CHECK(l1,l30)
){return soc_mem_alpm_insert(l1,l20,bucket_index+1,MEM_BLOCK_ANY,l146,
alpm_data,l12,l130);}}return l144;}int l167(int l1,soc_mem_t l20,int
bucket_index,int l168,uint32 l146,int*l169){int l170,l171 = 0;int l172[4] = {
0};int l173 = 0;int l174 = 0;int l175;int l30 = 0;int l176 = 6;int l177;int
l178 = 0;switch(l20){case L3_DEFIP_ALPM_IPV6_64m:l176 = 4;l30 = 1;break;case
L3_DEFIP_ALPM_IPV6_128m:l176 = 2;l30 = 1;break;default:break;}if(
SOC_ALPM_V6_SCALE_CHECK(l1,l30)){if(l168>= ALPM_RAW_BKT_COUNT*l176){
bucket_index++;l168-= ALPM_RAW_BKT_COUNT*l176;}}l177 = 4;if(
soc_th_get_alpm_banks(l1)<= 2){l177 = 2;}l178 = ((1<<l177)-1);l175 = l177-
_shr_popcount(l146&l178);if(bucket_index>= (1<<16)||l168>= l175*l176){return
SOC_E_FULL;}l174 = l168%l176;for(l170 = 0;l170<l177;l170++){if((1<<l170)&l146
){continue;}l172[l171++] = l170;}l173 = l172[l168/l176];*l169 = (l174<<16)|(
bucket_index<<2)|(l173);return SOC_E_NONE;}static int l179(int l1,soc_mem_t
l20,int l30,alpm_mem_prefix_array_t*l180,int*l44){int l170,l144 = SOC_E_NONE,
l181;defip_alpm_ipv4_entry_t l182,l183;defip_alpm_ipv6_64_entry_t l184,l185;
void*l186 = NULL,*l187 = NULL;int l188,l189;int*l190 = NULL;int l191 = FALSE;
if(l30){l188 = sizeof(l184);l189 = sizeof(l185);}else{l188 = sizeof(l182);
l189 = sizeof(l183);}l186 = sal_alloc(l188*l180->count,"rb_bufp");if(l186 == 
NULL){l144 = SOC_E_MEMORY;goto l192;}l187 = sal_alloc(l189*l180->count,
"rb_sip_bufp");if(l187 == NULL){l144 = SOC_E_MEMORY;goto l192;}l190 = 
sal_alloc(sizeof(*l190)*l180->count,"roll_back_index");if(l190 == NULL){l144 = 
SOC_E_MEMORY;goto l192;}sal_memset(l190,-1,sizeof(*l190)*l180->count);for(
l170 = 0;l170<l180->count;l170++){payload_t*prefix = l180->prefix[l170];if(
prefix->index>= 0){l144 = soc_mem_read(l1,l20,MEM_BLOCK_ANY,prefix->index,(
uint8*)l186+l170*l188);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x 0x%08x"" 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l170--;l191 = TRUE;
break;}if(SOC_URPF_STATUS_GET(l1)){l144 = soc_mem_read(l1,l20,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,prefix->index),(uint8*)l187+l170*l189);if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x"" 0x%08x 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l170--;l191 = TRUE;
break;}}l144 = soc_mem_write(l1,l20,MEM_BLOCK_ALL,prefix->index,
soc_mem_entry_null(l1,l20));if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old bkt for pfx 0x%08x 0x%08x\n"),prefix->key[0],prefix->key[
1]));l190[l170] = prefix->index;l191 = TRUE;break;}if(SOC_URPF_STATUS_GET(l1)
){l144 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,prefix
->index),soc_mem_entry_null(l1,l20));if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old urpf bkt for pfx 0x%08x"" 0x%08x\n"),prefix->key[0],
prefix->key[1]));l190[l170] = prefix->index;l191 = TRUE;break;}}}l190[l170] = 
prefix->index;prefix->index = l44[l170];}if(l191){for(;l170>= 0;l170--){
payload_t*prefix = l180->prefix[l170];prefix->index = l190[l170];if(l190[l170
]<0){continue;}l181 = soc_mem_write(l1,l20,MEM_BLOCK_ALL,l190[l170],(uint8*)
l186+l170*l188);if(SOC_FAILURE(l181)){break;}if(!SOC_URPF_STATUS_GET(l1)){
continue;}l181 = soc_mem_write(l1,l20,MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(l1
,l190[l170]),(uint8*)l187+l170*l189);if(SOC_FAILURE(l181)){break;}}}l192:if(
l190){sal_free(l190);l190 = NULL;}if(l187){sal_free(l187);l187 = NULL;}if(
l186){sal_free(l186);l186 = NULL;}return l144;}void
_soc_th_alpm_rollback_pivot_add(int l1,defip_entry_t*l46,void*key_data,int
tcam_index,alpm_pivot_t*pivot_pyld){int l144;trie_t*l149 = NULL;int l30,l24,
l152;trie_node_t*l193 = NULL;l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,
MODE0f);soc_th_alpm_lpm_vrf_get(l1,key_data,&l152,&l24);l144 = 
soc_th_alpm_lpm_delete(l1,l46);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failure to free new prefix"
"at %d\n"),soc_th_alpm_logical_idx(l1,L3_DEFIPm,tcam_index,l30)));}if(l30){
l149 = VRF_PIVOT_TRIE_IPV6(l1,l24);}else{l149 = VRF_PIVOT_TRIE_IPV4(l1,l24);}
if(ALPM_TCAM_PIVOT(l1,tcam_index<<(l30?1:0))!= NULL){l144 = trie_delete(l149,
pivot_pyld->key,pivot_pyld->len,&l193);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: trie delete failure "
"in bkt move rollback\n")));}}ALPM_TCAM_PIVOT(l1,tcam_index<<(l30?1:0)) = 
NULL;VRF_PIVOT_REF_DEC(l1,l24,l30);}int _soc_th_alpm_rollback_bkt_move(int l1
,void*key_data,soc_mem_t l20,alpm_pivot_t*l41,alpm_pivot_t*l42,
alpm_mem_prefix_array_t*l43,int*l44,int l45){trie_node_t*l193 = NULL;uint32
prefix[5],l35;trie_t*l33;int l30,l24,l152,l170,l22 = 0;;
defip_alpm_ipv4_entry_t l182;defip_alpm_ipv6_64_entry_t l184;void*l194;
payload_t*l195;int l144 = SOC_E_NONE;alpm_bucket_handle_t*l196;l196 = 
PIVOT_BUCKET_HANDLE(l42);l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,
MODE0f);if(l20 == L3_DEFIP_ALPM_IPV6_128m){l30 =L3_DEFIP_MODE_128;}l144 = 
l141(l1,key_data,prefix,&l35,&l22);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: prefix create failed\n")));
return l144;}if(l20 == L3_DEFIP_ALPM_IPV6_128m){l194 = ((uint32*)&(l184));}
else{l194 = ((l30)?((uint32*)&(l184)):((uint32*)&(l182)));}
soc_th_alpm_lpm_vrf_get(l1,key_data,&l152,&l24);if(l20 == 
L3_DEFIP_ALPM_IPV6_128m){l33 = VRF_PREFIX_TRIE_IPV6_128(l1,l24);}else if(l30)
{l33 = VRF_PREFIX_TRIE_IPV6(l1,l24);}else{l33 = VRF_PREFIX_TRIE_IPV4(l1,l24);
}for(l170 = 0;l170<l43->count;l170++){payload_t*l129 = l43->prefix[l170];if(
l44[l170]!= -1){if(l20 == L3_DEFIP_ALPM_IPV6_128m){sal_memset(l194,0,sizeof(
defip_alpm_ipv6_128_entry_t));}else if(l30){sal_memset(l194,0,sizeof(
defip_alpm_ipv6_64_entry_t));}else{sal_memset(l194,0,sizeof(
defip_alpm_ipv4_entry_t));}l144 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,l44[l170
],l194);_soc_trident2_alpm_bkt_view_set(l1,l44[l170],INVALIDm);if(SOC_FAILURE
(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l1)){l144 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l44[l170]),l194);_soc_trident2_alpm_bkt_view_set(l1
,_soc_th_alpm_rpf_entry(l1,l44[l170]),INVALIDm);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l193 = NULL;l144 = trie_delete(
PIVOT_BUCKET_TRIE(l42),l129->key,l129->len,&l193);l195 = (payload_t*)l193;if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(l129->
index>0){l144 = trie_insert(PIVOT_BUCKET_TRIE(l41),l129->key,NULL,l129->len,(
trie_node_t*)l195);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l195!= NULL){sal_free(l195);}}}if(l45 == 
-1){l193 = NULL;l144 = trie_delete(PIVOT_BUCKET_TRIE(l41),prefix,l35,&l193);
l195 = (payload_t*)l193;if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l195!= 
NULL){sal_free(l195);}}l144 = alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l42),
l30);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_insert: new bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(
l42)));}trie_destroy(PIVOT_BUCKET_TRIE(l42));sal_free(l196);sal_free(l42);
sal_free(l44);l193 = NULL;l144 = trie_delete(l33,prefix,l35,&l193);l195 = (
payload_t*)l193;if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_th_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l195){sal_free(
l195);}return l144;}int l197(int l1,trie_t*l33,trie_t*l198,payload_t*
new_pfx_pyld,int*l44,int bucket_index,int l30){trie_node_t*l193 = NULL;
payload_t*l199 = NULL;if(l44!= NULL){sal_free(l44);}l193 = NULL;(void)
trie_delete(l33,new_pfx_pyld->key,new_pfx_pyld->len,&l193);new_pfx_pyld = (
payload_t*)l193;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l30);}(void)trie_delete(l198,new_pfx_pyld->key,new_pfx_pyld->len
,&l193);l199 = (payload_t*)l193;if(l199!= NULL){sal_free(l199);}if(
new_pfx_pyld!= NULL){sal_free(new_pfx_pyld);}return SOC_E_NONE;}static int l31
(alpm_pfx_info_t*l32,trie_t*l33,uint32*l34,uint32 l35,trie_node_t*l36,
defip_entry_t*lpm_entry,uint32*l37){trie_node_t*l150 = NULL;int l30,l24,l152;
defip_alpm_ipv4_entry_t l182;defip_alpm_ipv6_64_entry_t l184;payload_t*l199 = 
NULL;int l200;void*l194;alpm_pivot_t*l201;alpm_bucket_handle_t*l196;int l144 = 
SOC_E_NONE;int unit;soc_mem_t l20;unit = l32->unit;l201 = l32->pivot_pyld;
l200 = l32->pivot_pyld->tcam_index;l30 = soc_mem_field32_get(l32->unit,
L3_DEFIPm,l32->key_data,MODE0f);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(
l32->unit,l32->key_data,&l152,&l24));l20 = (l30)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l194 = ((l30)?((uint32*)&(l184)):((uint32*)&(l182)));l150
= NULL;l144 = trie_find_lpm(l33,l34,l35,&l150);l199 = (payload_t*)l150;if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),unit,l34[0],l34[1],l34[2
],l34[3],l34[4],l35));return l144;}if(l199->bkt_ptr){if(l199->bkt_ptr == l32
->new_pfx_pyld){sal_memcpy(l194,l32->alpm_data,l30?sizeof(
defip_alpm_ipv6_64_entry_t):sizeof(defip_alpm_ipv4_entry_t));}else{l144 = 
soc_mem_read(unit,l20,MEM_BLOCK_ANY,((payload_t*)l199->bkt_ptr)->index,l194);
}if(SOC_FAILURE(l144)){return l144;}l144 = l23(unit,l194,l20,l30,l152,l32->
bucket_index,0,lpm_entry);if(SOC_FAILURE(l144)){return l144;}*l37 = ((
payload_t*)(l199->bkt_ptr))->len;}else{l144 = soc_mem_read(unit,L3_DEFIPm,
MEM_BLOCK_ANY,soc_th_alpm_logical_idx(unit,L3_DEFIPm,l200>>1,1),lpm_entry);if
((!l30)&&(l200&1)){l144 = soc_th_alpm_lpm_ip4entry1_to_0(unit,lpm_entry,
lpm_entry,0);}}l196 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l196 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n"))
);l144 = SOC_E_MEMORY;return l144;}sal_memset(l196,0,sizeof(*l196));l201 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");l32->pivot_pyld = 
l201;if(l201 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l144 = SOC_E_MEMORY;return l144;}sal_memset(l201,0,sizeof(*l201));
PIVOT_BUCKET_HANDLE(l201) = l196;if(l30){l144 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l201));}else{l144 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l201));}PIVOT_BUCKET_TRIE(l201)->trie = l36;
PIVOT_BUCKET_INDEX(l201) = l32->bucket_index;PIVOT_BUCKET_VRF(l201) = l24;
PIVOT_BUCKET_IPV6(l201) = l30;PIVOT_BUCKET_DEF(l201) = FALSE;(l201)->key[0] = 
l34[0];(l201)->key[1] = l34[1];(l201)->len = l35;(l201)->key[2] = l34[2];(
l201)->key[3] = l34[3];(l201)->key[4] = l34[4];do{if(!(l30)){l34[0] = (((32-
l35) == 32)?0:(l34[1])<<(32-l35));l34[1] = 0;}else{int l202 = 64-l35;int l203
;if(l202<32){l203 = l34[3]<<l202;l203|= (((32-l202) == 32)?0:(l34[4])>>(32-
l202));l34[0] = l34[4]<<l202;l34[1] = l203;l34[2] = l34[3] = l34[4] = 0;}else
{l34[1] = (((l202-32) == 32)?0:(l34[4])<<(l202-32));l34[0] = l34[2] = l34[3] = 
l34[4] = 0;}}}while(0);l26(unit,l34,l35,l24,l30,lpm_entry,0,0);
soc_L3_DEFIPm_field32_set(unit,lpm_entry,ALG_BKT_PTR0f,l32->bucket_index);l32
->pivot_pyld = l201;return l144;}static int l39(alpm_pfx_info_t*l32,int*l40,
int*l13){trie_node_t*l36;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l30,l24,l152
;uint32 l35,l37 = 0;uint32 l146 = 0;uint32 l34[5];int l130;
defip_alpm_ipv4_entry_t l182,l183;defip_alpm_ipv6_64_entry_t l184,l185;trie_t
*l33,*trie;void*l204;void*l194;alpm_pivot_t*l205 = l32->pivot_pyld;
defip_entry_t lpm_entry;soc_mem_t l20;trie_t*l149 = NULL;
alpm_mem_prefix_array_t l180;int*l44 = NULL;int l144 = SOC_E_NONE,l170,l45 = 
-1;l30 = soc_mem_field32_get(l32->unit,L3_DEFIPm,l32->key_data,MODE0f);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l32->unit,l32->key_data,&l152,&
l24));l20 = (l30)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l194 = ((l30)?((
uint32*)&(l184)):((uint32*)&(l182)));l204 = ((l30)?((uint32*)&(l185)):((
uint32*)&(l183)));if(l24 == SOC_VRF_MAX(l32->unit)+1){if(
soc_th_get_alpm_banks(l32->unit)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l32->unit,l146);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l32->unit,l146);}}else{if(
soc_th_get_alpm_banks(l32->unit)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l32->unit,l146);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l32->unit,l146);}}if(l30){l33 = 
VRF_PREFIX_TRIE_IPV6(l32->unit,l24);}else{l33 = VRF_PREFIX_TRIE_IPV4(l32->
unit,l24);}trie = PIVOT_BUCKET_TRIE(l32->pivot_pyld);l144 = 
alpm_bucket_assign(l32->unit,&l32->bucket_index,l30);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l32->unit,
"_soc_th_alpm_insert: Unable to allocate""new bucket for split\n")));l32->
bucket_index = -1;l197(l32->unit,l33,trie,l32->new_pfx_pyld,l44,l32->
bucket_index,l30);return l144;}l144 = trie_split(trie,l30?_MAX_KEY_LEN_144_:
_MAX_KEY_LEN_48_,FALSE,l34,&l35,&l36,NULL,FALSE);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l32->unit,
"_soc_th_alpm_insert: Could not split bucket")));l197(l32->unit,l33,trie,l32
->new_pfx_pyld,l44,l32->bucket_index,l30);return l144;}l144 = l31(l32,l33,l34
,l35,l36,&lpm_entry,&l37);if(l144!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l32->unit,"could not initialize pivot")));l197(l32->unit,l33,trie,
l32->new_pfx_pyld,l44,l32->bucket_index,l30);return l144;}sal_memset(&l180,0,
sizeof(l180));l144 = trie_traverse(PIVOT_BUCKET_TRIE(l32->pivot_pyld),
alpm_mem_prefix_array_cb,&l180,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l32->unit,
"_soc_th_alpm_insert: Bucket split failed")));l197(l32->unit,l33,trie,l32->
new_pfx_pyld,l44,l32->bucket_index,l30);return l144;}l44 = sal_alloc(sizeof(*
l44)*l180.count,"Temp storage for location of prefixes in new bucket");if(l44
== NULL){l144 = SOC_E_MEMORY;l197(l32->unit,l33,trie,l32->new_pfx_pyld,l44,
l32->bucket_index,l30);return l144;}sal_memset(l44,-1,sizeof(*l44)*l180.count
);for(l170 = 0;l170<l180.count;l170++){payload_t*l129 = l180.prefix[l170];if(
l129->index>0){l144 = soc_mem_read(l32->unit,l20,MEM_BLOCK_ANY,l129->index,
l194);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l32->unit,
"_soc_alpm_insert: Failed to""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l180.prefix[l170]->key[1],l180.
prefix[l170]->key[2],l180.prefix[l170]->key[3],l180.prefix[l170]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l32->unit,l32->key_data,l20,l205,l32->
pivot_pyld,&l180,l44,l45);return l144;}if(SOC_URPF_STATUS_GET(l32->unit)){
l144 = soc_mem_read(l32->unit,l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l32->
unit,l129->index),l204);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l32->unit,"_soc_th_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l180.prefix[l170]->key[1],l180.
prefix[l170]->key[2],l180.prefix[l170]->key[3],l180.prefix[l170]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l32->unit,l32->key_data,l20,l205,l32->
pivot_pyld,&l180,l44,l45);return l144;}}l144 = _soc_th_alpm_insert_in_bkt(l32
->unit,l20,l32->bucket_index,l146,l194,l12,&l130,l30);if(SOC_SUCCESS(l144)){
if(SOC_URPF_STATUS_GET(l32->unit)){l144 = soc_mem_write(l32->unit,l20,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l32->unit,l130),l204);}}}else{l144 = 
_soc_th_alpm_insert_in_bkt(l32->unit,l20,l32->bucket_index,l146,l32->
alpm_data,l12,&l130,l30);if(SOC_SUCCESS(l144)){l45 = l170;*l13 = l130;if(
SOC_URPF_STATUS_GET(l32->unit)){l144 = soc_mem_write(l32->unit,l20,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l32->unit,l130),l32->alpm_sip_data);}}}
l44[l170] = l130;if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l32->unit,"_soc_th_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l180.prefix[l170]->key[1],l180.
prefix[l170]->key[2],l180.prefix[l170]->key[3],l180.prefix[l170]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l32->unit,l32->key_data,l20,l205,l32->
pivot_pyld,&l180,l44,l45);return l144;}}l144 = l4(l32->unit,&lpm_entry,l40);
if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l32->unit,
"_soc_th_alpm_insert: Unable to add new""pivot to tcam\n")));if(l144 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l32->unit,l24,l30);}(void)
_soc_th_alpm_rollback_bkt_move(l32->unit,l32->key_data,l20,l205,l32->
pivot_pyld,&l180,l44,l45);return l144;}*l40 = soc_th_alpm_physical_idx(l32->
unit,L3_DEFIPm,*l40,l30);l144 = l154(l32->unit,*l40,l30,l37);if(SOC_FAILURE(
l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l32->unit,
"_soc_th_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l40));
_soc_th_alpm_rollback_pivot_add(l32->unit,&lpm_entry,l32->key_data,*l40,l32->
pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l32->unit,l32->key_data,l20,
l205,l32->pivot_pyld,&l180,l44,l45);return l144;}if(l30){l149 = 
VRF_PIVOT_TRIE_IPV6(l32->unit,l24);}else{l149 = VRF_PIVOT_TRIE_IPV4(l32->unit
,l24);}l144 = trie_insert(l149,l32->pivot_pyld->key,NULL,l32->pivot_pyld->len
,(trie_node_t*)l32->pivot_pyld);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l32->unit,"failed to insert into pivot trie\n")))
;(void)_soc_th_alpm_rollback_bkt_move(l32->unit,l32->key_data,l20,l205,l32->
pivot_pyld,&l180,l44,l45);return l144;}ALPM_TCAM_PIVOT(l32->unit,*l40<<(l30?1
:0)) = l32->pivot_pyld;PIVOT_TCAM_INDEX(l32->pivot_pyld) = *l40<<(l30?1:0);
VRF_PIVOT_REF_INC(l32->unit,l24,l30);l144 = l179(l32->unit,l20,l30,&l180,l44)
;if(SOC_FAILURE(l144)){_soc_th_alpm_rollback_pivot_add(l32->unit,&lpm_entry,
l32->key_data,*l40,l32->pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l32
->unit,l32->key_data,l20,l205,l32->pivot_pyld,&l180,l44,l45);sal_free(l44);
l44 = NULL;return l144;}sal_free(l44);if(l45 == -1){l144 = 
_soc_th_alpm_insert_in_bkt(l32->unit,l20,PIVOT_BUCKET_INDEX(l205),l146,l32->
alpm_data,l12,&l130,l30);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l32->unit,"_soc_th_alpm_insert: Could not insert new "
"prefix into trie after split\n")));l197(l32->unit,l33,trie,l32->new_pfx_pyld
,l44,l32->bucket_index,l30);return l144;}if(SOC_URPF_STATUS_GET(l32->unit)){
l144 = soc_mem_write(l32->unit,l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l32->
unit,l130),l32->alpm_sip_data);}*l13 = l130;l32->new_pfx_pyld->index = l130;}
PIVOT_BUCKET_ENT_CNT_UPDATE(l32->pivot_pyld);VRF_BUCKET_SPLIT_INC(l32->unit,
l24,l30);return l144;}static int l206(int l1,void*key_data,soc_mem_t l20,void
*l207,void*l208,int*l13,int bucket_index,int tcam_index){alpm_pivot_t*l201,*
l205;defip_aux_scratch_entry_t l10;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l209,l35;int l30,l24,l152;int l130;int l144 = SOC_E_NONE,l181;
uint32 l8,l146;int l148 =0;int l40;int l210 = 0;trie_t*trie,*l33;trie_node_t*
l150 = NULL,*l193 = NULL;payload_t*l195,*l211,*l199;int l22 = 0;
alpm_pfx_info_t l32;l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);
if(l30){if(!(l30 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE1f))){return
(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l152
,&l24));if(l24 == SOC_VRF_MAX(l1)+1){l8 = 0;if(soc_th_get_alpm_banks(l1)<= 2)
{SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l1,l146);}else{
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l146);}}else{l8 = 2;if(
soc_th_get_alpm_banks(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,
l146);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l146);}}l20 = (l30)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l144 = l141(l1,key_data,prefix,&
l35,&l22);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_insert: prefix create failed\n")));return l144;}sal_memset(&l10
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(l1,key_data,l30,
l8,0,&l10));if(bucket_index == 0){l144 = l147(l1,prefix,l35,l30,l24,&l148,&
tcam_index,&bucket_index);SOC_IF_ERROR_RETURN(l144);if(l148 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_insert: "
"Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l144 = 
_soc_th_alpm_insert_in_bkt(l1,l20,bucket_index,l146,l207,l12,&l130,l30);if(
l144 == SOC_E_NONE){*l13 = l130;if(SOC_URPF_STATUS_GET(l1)){l181 = 
soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l130),l208);if(
SOC_FAILURE(l181)){return l181;}}}if(l144 == SOC_E_FULL){l210 = 1;}l201 = 
ALPM_TCAM_PIVOT(l1,tcam_index);trie = PIVOT_BUCKET_TRIE(l201);l205 = l201;
l195 = sal_alloc(sizeof(payload_t),"Payload for Key");if(l195 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 payload entry\n")));
return SOC_E_MEMORY;}l211 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l211 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_vrf_add: unable to allocate memory for "
"pfx trie node\n")));sal_free(l195);return SOC_E_MEMORY;}sal_memset(l195,0,
sizeof(*l195));sal_memset(l211,0,sizeof(*l211));l195->key[0] = prefix[0];l195
->key[1] = prefix[1];l195->key[2] = prefix[2];l195->key[3] = prefix[3];l195->
key[4] = prefix[4];l195->len = l35;l195->index = l130;sal_memcpy(l211,l195,
sizeof(*l195));l211->bkt_ptr = l195;l144 = trie_insert(trie,prefix,NULL,l35,(
trie_node_t*)l195);if(SOC_FAILURE(l144)){if(l195!= NULL){sal_free(l195);}if(
l211!= NULL){sal_free(l211);}return l144;}if(l30){l33 = VRF_PREFIX_TRIE_IPV6(
l1,l24);}else{l33 = VRF_PREFIX_TRIE_IPV4(l1,l24);}if(!l22){l144 = trie_insert
(l33,prefix,NULL,l35,(trie_node_t*)l211);}else{l150 = NULL;l144 = 
trie_find_lpm(l33,0,0,&l150);l199 = (payload_t*)l150;if(SOC_SUCCESS(l144)){
l199->bkt_ptr = l195;}}l209 = l35;if(SOC_FAILURE(l144)){l193 = NULL;(void)
trie_delete(trie,prefix,l209,&l193);l199 = (payload_t*)l193;sal_free(l199);
sal_free(l211);return l144;}if(l210){l32.unit = l1;l32.key_data = key_data;
l32.new_pfx_pyld = l195;l32.pivot_pyld = l201;l32.alpm_data = l207;l32.
alpm_sip_data = l208;l32.bucket_index = bucket_index;l144 = l39(&l32,&l40,l13
);if(l144!= SOC_E_NONE){return l144;}l201 = l32.pivot_pyld;bucket_index = l32
.bucket_index;}VRF_TRIE_ROUTES_INC(l1,l24,l30);if(l22){sal_free(l211);}
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l148,&
tcam_index,&bucket_index));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
INSERT_PROPAGATE,&l10,FALSE,&l148,&tcam_index,&bucket_index));if(
SOC_URPF_STATUS_GET(l1)){l35 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&
l10,DB_TYPEf);l35+= 1;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l10,
DB_TYPEf,l35);SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&
l10,TRUE,&l148,&tcam_index,&bucket_index));SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l10,FALSE,&l148,&tcam_index,&
bucket_index));}PIVOT_BUCKET_ENT_CNT_UPDATE(l205);return l144;}static int l26
(int unit,uint32*key,int len,int l24,int l7,defip_entry_t*lpm_entry,int l27,
int l28){uint32 l212;if(l28){sal_memset(lpm_entry,0,sizeof(defip_entry_t));}
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
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VALID1f,1);if(len>= 32){l212 = 
0xffffffff;soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f,l212);
l212 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32));soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l212);}else{l212 = ~(0xffffffff>>len);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f,l212);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,0);}}else{
soc_L3_DEFIPm_field32_set(unit,lpm_entry,IP_ADDR0f,key[0]);assert(len<= 32);
l212 = (len == 32)?0xffffffff:~(0xffffffff>>len);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l212);}soc_L3_DEFIPm_field32_set(unit,
lpm_entry,VALID0f,1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE_MASK0f
,(1<<soc_mem_field_length(unit,L3_DEFIPm,MODE_MASK0f))-1);soc_mem_field32_set
(unit,L3_DEFIPm,lpm_entry,MODE_MASK1f,(1<<soc_mem_field_length(unit,L3_DEFIPm
,MODE_MASK1f))-1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
ENTRY_TYPE_MASK0f,(1<<soc_mem_field_length(unit,L3_DEFIPm,ENTRY_TYPE_MASK0f))
-1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK1f,(1<<
soc_mem_field_length(unit,L3_DEFIPm,ENTRY_TYPE_MASK1f))-1);return(SOC_E_NONE)
;}int _soc_th_alpm_delete_in_bkt(int l1,soc_mem_t l20,int l213,int l146,void*
l214,uint32*l12,int*l130,int l30){int l144;l144 = soc_mem_alpm_delete(l1,l20,
l213,MEM_BLOCK_ALL,l146,l214,l12,l130);if(SOC_SUCCESS(l144)){return l144;}if(
SOC_ALPM_V6_SCALE_CHECK(l1,l30)){return soc_mem_alpm_delete(l1,l20,l213+1,
MEM_BLOCK_ALL,l146,l214,l12,l130);}return l144;}static int l215(int l1,void*
key_data,int bucket_index,int tcam_index,int l130){alpm_pivot_t*pivot_pyld;
defip_alpm_ipv4_entry_t l182,l183;defip_alpm_ipv6_64_entry_t l184,l185;
defip_alpm_ipv4_entry_t l216,l217;defip_aux_scratch_entry_t l10;uint32 l12[
SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l20;void*l194,*l214,*l204 = NULL;int l152;
int l7;int l144 = SOC_E_NONE,l181;uint32 l218[5],prefix[5];int l30,l24;uint32
l35;int l213;uint32 l8,l146;int l148,l22 = 0;trie_t*trie,*l33;uint32 l219;
defip_entry_t lpm_entry,*l220;payload_t*l195 = NULL,*l221 = NULL,*l199 = NULL
;trie_node_t*l193 = NULL,*l150 = NULL;trie_t*l149 = NULL;l7 = l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);if(l30){if(!(l30 = 
soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l152,&l24));if(l152
!= SOC_L3_VRF_OVERRIDE){if(l24 == SOC_VRF_MAX(l1)+1){l8 = 0;if(
soc_th_get_alpm_banks(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(
l1,l146);}else{SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l146);}}else{l8 = 2;if(
soc_th_get_alpm_banks(l1)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l1,
l146);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l146);}}l144 = l141(l1,key_data,
prefix,&l35,&l22);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"_soc_alpm_delete: prefix create failed\n")));return l144;}if(!
soc_th_alpm_mode_get(l1)){if(l152!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT
(l1,l24,l30)>1){if(l22){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot delete default ""route if other routes are present in "
"this mode"),l24));return SOC_E_PARAM;}}}l8 = 2;}l20 = (l30)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l194 = ((l30)?((uint32*)&(l184)):(
(uint32*)&(l182)));l214 = ((l30)?((uint32*)&(l217)):((uint32*)&(l216)));
SOC_ALPM_LPM_LOCK(l1);if(bucket_index == 0){l144 = l151(l1,key_data,l20,l194,
&tcam_index,&bucket_index,&l130);}else{l144 = l17(l1,key_data,l194,0,l20,0,0)
;}sal_memcpy(l214,l194,l30?sizeof(l184):sizeof(l182));if(SOC_FAILURE(l144)){
SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to find prefix for delete\n")));return l144;}l213 = 
bucket_index;pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l144 = trie_delete(trie,prefix,l35,&l193);l195 = 
(payload_t*)l193;if(l144!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l144;}if(l30){l33 = VRF_PREFIX_TRIE_IPV6(l1,
l24);}else{l33 = VRF_PREFIX_TRIE_IPV4(l1,l24);}if(l30){l149 = 
VRF_PIVOT_TRIE_IPV6(l1,l24);}else{l149 = VRF_PIVOT_TRIE_IPV4(l1,l24);}if(!l22
){l144 = trie_delete(l33,prefix,l35,&l193);l221 = (payload_t*)l193;if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l222;}l150 = NULL;l144 = trie_find_lpm(l33,prefix,l35,&
l150);l199 = (payload_t*)l150;if(SOC_SUCCESS(l144)){payload_t*l223 = (
payload_t*)(l199->bkt_ptr);if(l223!= NULL){l219 = l223->len;}else{l219 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l224;}
sal_memcpy(l218,prefix,sizeof(prefix));do{if(!(l30)){l218[0] = (((32-l35) == 
32)?0:(l218[1])<<(32-l35));l218[1] = 0;}else{int l202 = 64-l35;int l203;if(
l202<32){l203 = l218[3]<<l202;l203|= (((32-l202) == 32)?0:(l218[4])>>(32-l202
));l218[0] = l218[4]<<l202;l218[1] = l203;l218[2] = l218[3] = l218[4] = 0;}
else{l218[1] = (((l202-32) == 32)?0:(l218[4])<<(l202-32));l218[0] = l218[2] = 
l218[3] = l218[4] = 0;}}}while(0);l144 = l26(l1,prefix,l219,l24,l7,&lpm_entry
,0,1);l181 = l151(l1,&lpm_entry,l20,l194,&tcam_index,&bucket_index,&l130);(
void)l23(l1,l194,l20,l7,l152,bucket_index,0,&lpm_entry);(void)l26(l1,l218,l35
,l24,l7,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(l144)){
l204 = ((l30)?((uint32*)&(l185)):((uint32*)&(l183)));l181 = soc_mem_read(l1,
l20,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l130),l204);}}if((l219 == 0)&&
SOC_FAILURE(l181)){l220 = l30?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l24);sal_memcpy(&lpm_entry,l220,sizeof(
lpm_entry));l144 = l26(l1,l218,l35,l24,l7,&lpm_entry,0,1);}if(SOC_FAILURE(
l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l224;}l220 = 
&lpm_entry;}else{l150 = NULL;l144 = trie_find_lpm(l33,prefix,l35,&l150);l199 = 
(payload_t*)l150;if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),l24));goto l222;}l199->bkt_ptr = NULL;l219 = 
0;l220 = l30?VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24):VRF_TRIE_DEFAULT_ROUTE_IPV4(
l1,l24);}l144 = l6(l1,l220,l30,l8,l219,&l10);if(SOC_FAILURE(l144)){goto l224;
}l144 = _soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,TRUE,&l148,&tcam_index,&
bucket_index);if(SOC_FAILURE(l144)){goto l224;}if(SOC_URPF_STATUS_GET(l1)){
uint32 l140;if(l204!= NULL){l140 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf);l140++;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf,l140);l140 = soc_mem_field32_get(l1,l20,
l204,SRC_DISCARDf);soc_mem_field32_set(l1,l20,&l10,SRC_DISCARDf,l140);l140 = 
soc_mem_field32_get(l1,l20,l204,DEFAULTROUTEf);soc_mem_field32_set(l1,l20,&
l10,DEFAULTROUTEf,l140);l144 = _soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,
TRUE,&l148,&tcam_index,&bucket_index);}if(SOC_FAILURE(l144)){goto l224;}}
sal_free(l195);if(!l22){sal_free(l221);}PIVOT_BUCKET_ENT_CNT_UPDATE(
pivot_pyld);if((pivot_pyld->len!= 0)&&(trie->trie == NULL)){uint32 l225[5];
sal_memcpy(l225,pivot_pyld->key,sizeof(l225));do{if(!(l7)){l225[0] = (((32-
pivot_pyld->len) == 32)?0:(l225[1])<<(32-pivot_pyld->len));l225[1] = 0;}else{
int l202 = 64-pivot_pyld->len;int l203;if(l202<32){l203 = l225[3]<<l202;l203
|= (((32-l202) == 32)?0:(l225[4])>>(32-l202));l225[0] = l225[4]<<l202;l225[1]
= l203;l225[2] = l225[3] = l225[4] = 0;}else{l225[1] = (((l202-32) == 32)?0:(
l225[4])<<(l202-32));l225[0] = l225[2] = l225[3] = l225[4] = 0;}}}while(0);
l26(l1,l225,pivot_pyld->len,l24,l7,&lpm_entry,0,1);l144 = 
soc_th_alpm_lpm_delete(l1,&lpm_entry);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_delete: Unable to "
"delete pivot 0x%08x 0x%08x \n"),pivot_pyld->key[0],pivot_pyld->key[1]));}}
l144 = _soc_th_alpm_delete_in_bkt(l1,l20,l213,l146,l214,l12,&l130,l30);if(!
SOC_SUCCESS(l144)){SOC_ALPM_LPM_UNLOCK(l1);l144 = SOC_E_FAIL;return l144;}if(
SOC_URPF_STATUS_GET(l1)){l144 = soc_mem_alpm_delete(l1,l20,
SOC_ALPM_RPF_BKT_IDX(l1,l213),MEM_BLOCK_ALL,l146,l214,l12,&l148);if(!
SOC_SUCCESS(l144)){SOC_ALPM_LPM_UNLOCK(l1);l144 = SOC_E_FAIL;return l144;}}if
((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l144 = alpm_bucket_release(l1,
PIVOT_BUCKET_INDEX(pivot_pyld),l30);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_delete: Unable to release"
"empty bucket: %d\n"),PIVOT_BUCKET_INDEX(pivot_pyld)));}l144 = trie_delete(
l149,pivot_pyld->key,pivot_pyld->len,&l193);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"could not delete pivot from pivot trie\n")));
}trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(
pivot_pyld));sal_free(pivot_pyld);_soc_trident2_alpm_bkt_view_set(l1,l213<<2,
INVALIDm);if(SOC_ALPM_V6_SCALE_CHECK(l1,l30)){_soc_trident2_alpm_bkt_view_set
(l1,(l213+1)<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l24,l30);if(
VRF_TRIE_ROUTES_CNT(l1,l24,l30) == 0){l144 = l29(l1,l24,l30);}
SOC_ALPM_LPM_UNLOCK(l1);return l144;l224:LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"recovering soc_th_alpm_vrf_delete failed\n ")));l181 = 
trie_insert(l33,prefix,NULL,l35,(trie_node_t*)l221);if(SOC_FAILURE(l181)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into pfx trie\n")
,prefix[0],prefix[1]));}l222:l181 = trie_insert(trie,prefix,NULL,l35,(
trie_node_t*)l195);if(SOC_FAILURE(l181)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into bkt trie\n"),prefix[0],prefix[1]));}
SOC_ALPM_LPM_UNLOCK(l1);return l144;}int soc_th_alpm_init(int l1){int l170;
int l144 = SOC_E_NONE;l144 = l2(l1);SOC_IF_ERROR_RETURN(l144);l144 = l16(l1);
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
);}l144 = soc_th_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l144);return l144;
}static int l226(int l1){int l170,l144;alpm_pivot_t*l140;for(l170 = 0;l170<
MAX_PIVOT_COUNT;l170++){l140 = ALPM_TCAM_PIVOT(l1,l170);if(l140){if(
PIVOT_BUCKET_HANDLE(l140)){if(PIVOT_BUCKET_TRIE(l140)){l144 = trie_traverse(
PIVOT_BUCKET_TRIE(l140),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l144)){trie_destroy(PIVOT_BUCKET_TRIE(l140));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Unable to clear trie state for unit %d\n"),l1
));return l144;}}sal_free(PIVOT_BUCKET_HANDLE(l140));}sal_free(
ALPM_TCAM_PIVOT(l1,l170));ALPM_TCAM_PIVOT(l1,l170) = NULL;}}for(l170 = 0;l170
<= SOC_VRF_MAX(l1)+1;l170++){l144 = trie_traverse(VRF_PREFIX_TRIE_IPV4(l1,
l170),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l144)){
trie_destroy(VRF_PREFIX_TRIE_IPV4(l1,l170));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l1,l170));
return l144;}l144 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l1,l170),
alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l144)){
trie_destroy(VRF_PREFIX_TRIE_IPV6(l1,l170));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l1,l170));
return l144;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l170)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l170));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l170
)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l170));}sal_memset(&
alpm_vrf_handle[l1][l170],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l1][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,1,1);
VRF_TRIE_INIT_DONE(l1,MAX_VRF_ID,2,1);if(SOC_ALPM_BUCKET_BMAP(l1)!= NULL){
sal_free(SOC_ALPM_BUCKET_BMAP(l1));}sal_memset(&soc_th_alpm_bucket[l1],0,
sizeof(soc_alpm_bucket_t));if(alpm_vrf_handle[l1])sal_free(alpm_vrf_handle[l1
]);if(tcam_pivot[l1])sal_free(tcam_pivot[l1]);alpm_vrf_handle[l1] = NULL;
tcam_pivot[l1] = NULL;return SOC_E_NONE;}int soc_th_alpm_deinit(int l1){l3(l1
);SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_deinit(l1));SOC_IF_ERROR_RETURN(
soc_th_alpm_128_state_clear(l1));SOC_IF_ERROR_RETURN(l226(l1));if(SOC_CONTROL
(l1)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l1)->alpm_bulk_retry);
SOC_CONTROL(l1)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_lookup_retry
);SOC_CONTROL(l1)->alpm_lookup_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_insert_retry
);SOC_CONTROL(l1)->alpm_insert_retry = NULL;}if(SOC_CONTROL(l1)->
alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l1)->alpm_delete_retry
);SOC_CONTROL(l1)->alpm_delete_retry = NULL;}return SOC_E_NONE;}static int
l227(int l1,int l24,int l30){defip_entry_t*lpm_entry,l228;int l229;int index;
int l144 = SOC_E_NONE;uint32 key[2] = {0,0};uint32 l35;alpm_bucket_handle_t*
l196;alpm_pivot_t*pivot_pyld;payload_t*l221;trie_t*l230;trie_t*l231 = NULL;if
(l30 == 0){trie_init(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(l1,l24));l231 = 
VRF_PIVOT_TRIE_IPV4(l1,l24);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6(l1,l24));l231 = VRF_PIVOT_TRIE_IPV6(l1,l24);}if(l30 == 0)
{trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(l1,l24));l230 = 
VRF_PREFIX_TRIE_IPV4(l1,l24);}else{trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6(l1,l24));l230 = VRF_PREFIX_TRIE_IPV6(l1,l24);}lpm_entry = 
sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n")));
return SOC_E_MEMORY;}l26(l1,key,0,l24,l30,lpm_entry,0,1);if(l30 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l24) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24) = lpm_entry;}if(l24 == SOC_VRF_MAX(l1)+1)
{soc_L3_DEFIPm_field32_set(l1,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l1,lpm_entry,DEFAULT_MISS0f,1);}l144 = 
alpm_bucket_assign(l1,&l229,l30);soc_L3_DEFIPm_field32_set(l1,lpm_entry,
ALG_BKT_PTR0f,l229);sal_memcpy(&l228,lpm_entry,sizeof(l228));l144 = l4(l1,&
l228,&index);l196 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l196 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_th_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")
));return SOC_E_NONE;}sal_memset(l196,0,sizeof(*l196));pivot_pyld = sal_alloc
(sizeof(alpm_pivot_t),"Payload for Pivot");if(pivot_pyld == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
sal_free(l196);return SOC_E_MEMORY;}l221 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l221 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_vrf_add: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l196);sal_free(pivot_pyld);return SOC_E_MEMORY
;}sal_memset(pivot_pyld,0,sizeof(*pivot_pyld));sal_memset(l221,0,sizeof(*l221
));l35 = 0;PIVOT_BUCKET_HANDLE(pivot_pyld) = l196;if(l30){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(pivot_pyld));}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(pivot_pyld));}PIVOT_BUCKET_INDEX(
pivot_pyld) = l229;PIVOT_BUCKET_VRF(pivot_pyld) = l24;PIVOT_BUCKET_IPV6(
pivot_pyld) = l30;PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;pivot_pyld->key[0] = 
l221->key[0] = key[0];pivot_pyld->key[1] = l221->key[1] = key[1];pivot_pyld->
len = l221->len = l35;l144 = trie_insert(l230,key,NULL,l35,&(l221->node));if(
SOC_FAILURE(l144)){sal_free(l221);sal_free(pivot_pyld);sal_free(l196);return
l144;}l144 = trie_insert(l231,key,NULL,l35,(trie_node_t*)pivot_pyld);if(
SOC_FAILURE(l144)){trie_node_t*l193 = NULL;(void)trie_delete(l230,key,l35,&
l193);sal_free(l221);sal_free(pivot_pyld);sal_free(l196);return l144;}index = 
soc_th_alpm_physical_idx(l1,L3_DEFIPm,index,l30);if(l30 == 0){ALPM_TCAM_PIVOT
(l1,index) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = index;}else{
ALPM_TCAM_PIVOT(l1,index<<1) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = 
index<<1;}VRF_PIVOT_REF_INC(l1,l24,l30);VRF_TRIE_INIT_DONE(l1,l24,l30,1);
return l144;}static int l29(int l1,int l24,int l30){defip_entry_t*lpm_entry;
int l229;int l137;int l144 = SOC_E_NONE;uint32 key[2] = {0,0},l232[
SOC_MAX_MEM_FIELD_WORDS];payload_t*l195;alpm_pivot_t*l233;trie_node_t*l193;
trie_t*l230;trie_t*l231 = NULL;if(l30 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l24);}else{lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24);}l229 = soc_L3_DEFIPm_field32_get(l1,
lpm_entry,ALG_BKT_PTR0f);l144 = alpm_bucket_release(l1,l229,l30);
_soc_trident2_alpm_bkt_view_set(l1,l229<<2,INVALIDm);l144 = l15(l1,lpm_entry,
(void*)l232,&l137);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_vrf_delete: unable to get internal"
" pivot idx for vrf %d/%d\n"),l24,l30));l137 = -1;}l137 = 
soc_th_alpm_physical_idx(l1,L3_DEFIPm,l137,l30);if(l30 == 0){l233 = 
ALPM_TCAM_PIVOT(l1,l137);}else{l233 = ALPM_TCAM_PIVOT(l1,l137<<1);}l144 = 
soc_th_alpm_lpm_delete(l1,lpm_entry);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),l24,l30));}sal_free(lpm_entry);if(
l30 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l1,l24) = NULL;l230 = 
VRF_PREFIX_TRIE_IPV4(l1,l24);VRF_PREFIX_TRIE_IPV4(l1,l24) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l1,l24) = NULL;l230 = VRF_PREFIX_TRIE_IPV6(l1,l24
);VRF_PREFIX_TRIE_IPV6(l1,l24) = NULL;}VRF_TRIE_INIT_DONE(l1,l24,l30,0);l144 = 
trie_delete(l230,key,0,&l193);l195 = (payload_t*)l193;if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal default for vrf "" %d/%d\n"),l24,l30));}sal_free(
l195);(void)trie_destroy(l230);if(l30 == 0){l231 = VRF_PIVOT_TRIE_IPV4(l1,l24
);VRF_PIVOT_TRIE_IPV4(l1,l24) = NULL;}else{l231 = VRF_PIVOT_TRIE_IPV6(l1,l24)
;VRF_PIVOT_TRIE_IPV6(l1,l24) = NULL;}l193 = NULL;l144 = trie_delete(l231,key,
0,&l193);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l24,l30));}(void)
trie_destroy(l231);sal_free(PIVOT_BUCKET_HANDLE(l233));(void)trie_destroy(
PIVOT_BUCKET_TRIE(l233));sal_free(l233);return l144;}int soc_th_alpm_insert(
int l1,void*l5,uint32 l21,int l234,int l235){defip_alpm_ipv4_entry_t l182,
l183;defip_alpm_ipv6_64_entry_t l184,l185;soc_mem_t l20;void*l194,*l214;int
l152,l24;int index;int l7;int l144 = SOC_E_NONE;uint32 l22;l7 = 
soc_mem_field32_get(l1,L3_DEFIPm,l5,MODE0f);l20 = (l7)?L3_DEFIP_ALPM_IPV6_64m
:L3_DEFIP_ALPM_IPV4m;l194 = ((l7)?((uint32*)&(l184)):((uint32*)&(l182)));l214
= ((l7)?((uint32*)&(l185)):((uint32*)&(l183)));SOC_IF_ERROR_RETURN(l17(l1,l5,
l194,l214,l20,l21,&l22));SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,l5,&
l152,&l24));if(((l152 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l152 == SOC_L3_VRF_GLOBAL)))){l144 = l4(l1,l5,&
index);if(SOC_SUCCESS(l144)){VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l7);
VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l7);}else if(l144 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID,l7);}return(l144);}else if(l24 == 0){if(
soc_th_alpm_mode_get(l1)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}}if(l152!= 
SOC_L3_VRF_GLOBAL){if(!soc_th_alpm_mode_get(l1)){if(VRF_TRIE_ROUTES_CNT(l1,
l24,l7) == 0){if(!l22){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
l152));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l24,l7)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_insert:VRF %d is not "
"initialized\n"),l24));l144 = l227(l1,l24,l7);if(SOC_FAILURE(l144)){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_alpm_insert:VRF %d/%d trie init \n"
"failed\n"),l24,l7));return l144;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),l24,l7));}if(l235&
SOC_ALPM_LOOKUP_HIT){l144 = l153(l1,l5,l194,l214,l20,l234);}else{if(l234 == -
1){l234 = 0;}l144 = l206(l1,l5,l20,l194,l214,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l234),l235);}if(l144!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l1,soc_errmsg(l144)
));}return(l144);}int soc_th_alpm_lookup(int l1,void*key_data,void*l12,int*
l13,int*l236){defip_alpm_ipv4_entry_t l182;defip_alpm_ipv6_64_entry_t l184;
soc_mem_t l20;int bucket_index;int tcam_index;void*l194;int l152,l24;int l7,
l129;int l144 = SOC_E_NONE;l7 = soc_mem_field32_get(l1,L3_DEFIPm,key_data,
MODE0f);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,key_data,&l152,&l24));
l144 = l11(l1,key_data,l12,l13,&l129,&l7);if(SOC_SUCCESS(l144)){if(!l7&&(*l13
&0x1)){l144 = soc_th_alpm_lpm_ip4entry1_to_0(l1,l12,l12,PRESERVE_HIT);}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,l12,&l152,&l24));if(l152 == 
SOC_L3_VRF_OVERRIDE){return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l24,
l7)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_lookup:VRF %d is not initialized\n"),l24));*l236 = 0;return
SOC_E_NOT_FOUND;}l20 = (l7)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l194 = 
((l7)?((uint32*)&(l184)):((uint32*)&(l182)));SOC_ALPM_LPM_LOCK(l1);l144 = 
l151(l1,key_data,l20,l194,&tcam_index,&bucket_index,l13);SOC_ALPM_LPM_UNLOCK(
l1);if(SOC_FAILURE(l144)){*l236 = tcam_index;*l13 = bucket_index<<2;return
l144;}l144 = l23(l1,l194,l20,l7,l152,bucket_index,*l13,l12);*l236 = 
SOC_ALPM_LOOKUP_HIT|tcam_index;return(l144);}int soc_th_alpm_delete(int l1,
void*key_data,int l234,int l235){int l152,l24;int l7;int l144 = SOC_E_NONE;l7
= soc_mem_field32_get(l1,L3_DEFIPm,key_data,MODE0f);SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l1,key_data,&l152,&l24));if(((l152 == 
SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)
&&(l152 == SOC_L3_VRF_GLOBAL)))){l144 = soc_th_alpm_lpm_delete(l1,key_data);
if(SOC_SUCCESS(l144)){VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l7);VRF_TRIE_ROUTES_DEC
(l1,MAX_VRF_ID,l7);}return(l144);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l24,l7)
){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_delete:VRF %d/%d is not initialized\n"),l24,l7));return SOC_E_NONE;
}if(l234 == -1){l234 = 0;}l144 = l215(l1,key_data,SOC_ALPM_BKT_ENTRY_TO_IDX(
l234),l235&~SOC_ALPM_LOOKUP_HIT,l234);}return(l144);}static int l16(int l1){
int l237;l237 = soc_mem_index_count(l1,L3_DEFIPm)+soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)*2;if(SOC_URPF_STATUS_GET(l1)){l237>>= 1;}
SOC_ALPM_BUCKET_COUNT(l1) = l237*2;SOC_ALPM_BUCKET_BMAP_SIZE(l1) = 
SHR_BITALLOCSIZE(SOC_ALPM_BUCKET_COUNT(l1));SOC_ALPM_BUCKET_BMAP(l1) = 
sal_alloc(SOC_ALPM_BUCKET_BMAP_SIZE(l1),"alpm_shared_bucket_bitmap");if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_shared_mem_init: Memory allocation for "
"bucket bitmap management failed\n")));return SOC_E_MEMORY;}sal_memset(
SOC_ALPM_BUCKET_BMAP(l1),0,SOC_ALPM_BUCKET_BMAP_SIZE(l1));alpm_bucket_assign(
l1,&l237,1);return SOC_E_NONE;}static void l117(int l1,void*l12,int index,
l112 l118){if(index&(0x8000)){l118[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(l1)]->l71));l118[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l73));l118[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l72));l118[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l74));if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int
l238;(void)soc_th_alpm_lpm_vrf_get(l1,l12,(int*)&l118[4],&l238);}else{l118[4]
= 0;};}else{if(index&0x1){l118[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(l1)]->l72));l118[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l74));l118[2] = 0;l118[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l105[(l1)]->l88)!= NULL))){int l238;defip_entry_t l239;(void)
soc_th_alpm_lpm_ip4entry1_to_0(l1,l12,&l239,0);(void)soc_th_alpm_lpm_vrf_get(
l1,&l239,(int*)&l118[4],&l238);}else{l118[4] = 0;};}else{l118[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l71));l118[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l73));l118[2] = 0;l118[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int l238;(void)
soc_th_alpm_lpm_vrf_get(l1,l12,(int*)&l118[4],&l238);}else{l118[4] = 0;};}}}
static int l240(l112 l114,l112 l115){int l137;for(l137 = 0;l137<5;l137++){{if
((l114[l137])<(l115[l137])){return-1;}if((l114[l137])>(l115[l137])){return 1;
}};}return(0);}static void l241(int l1,void*l5,uint32 l242,uint32 l132,int
l129){l112 l243;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l75))){if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(l1)]->l86))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l85))){l243[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l71));l243[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l73));l243[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l72));l243[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l74));if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int
l238;(void)soc_th_alpm_lpm_vrf_get(l1,l5,(int*)&l243[4],&l238);}else{l243[4] = 
0;};l131((l116[(l1)]),l240,l243,l129,l132,((uint16)l242<<1)|(0x8000));}}else{
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105
[(l1)]->l85))){l243[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l71));l243[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l73));l243[2] = 0;l243[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l1)))&&((
(l105[(l1)]->l87)!= NULL))){int l238;(void)soc_th_alpm_lpm_vrf_get(l1,l5,(int
*)&l243[4],&l238);}else{l243[4] = 0;};l131((l116[(l1)]),l240,l243,l129,l132,(
(uint16)l242<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l86))){l243[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l5),(l105[(
l1)]->l72));l243[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l5),(l105[(l1)]->l74));l243[2] = 0;l243[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l88)!= NULL))){int l238;defip_entry_t
l239;(void)soc_th_alpm_lpm_ip4entry1_to_0(l1,l5,&l239,0);(void)
soc_th_alpm_lpm_vrf_get(l1,&l239,(int*)&l243[4],&l238);}else{l243[4] = 0;};
l131((l116[(l1)]),l240,l243,l129,l132,(((uint16)l242<<1)+1));}}}static void
l244(int l1,void*key_data,uint32 l242){l112 l243;int l129 = -1;int l144;
uint16 index;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm
)),(key_data),(l105[(l1)]->l75))){l243[0] = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l71));l243[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l73));l243[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l72));l243[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l74));if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL)))
{int l238;(void)soc_th_alpm_lpm_vrf_get(l1,key_data,(int*)&l243[4],&l238);}
else{l243[4] = 0;};index = (l242<<1)|(0x8000);}else{l243[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l71));l243[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l73));l243[2] = 0;l243[3] = 0x80000001
;if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int l238;(void)
soc_th_alpm_lpm_vrf_get(l1,key_data,(int*)&l243[4],&l238);}else{l243[4] = 0;}
;index = l242;}l144 = l133((l116[(l1)]),l240,l243,l129,index);if(SOC_FAILURE(
l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\ndel  index: H %d error %d\n"),index,l144));}}static int l245(int l1,void*
key_data,int l129,int*l130){l112 l243;int l246;int l144;uint16 index = (
0xFFFF);l246 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)
),(key_data),(l105[(l1)]->l75));if(l246){l243[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l71));l243[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l73));l243[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l72));l243[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l74));if((!(SOC_IS_HURRICANE(l1)))&&((
(l105[(l1)]->l87)!= NULL))){int l238;(void)soc_th_alpm_lpm_vrf_get(l1,
key_data,(int*)&l243[4],&l238);}else{l243[4] = 0;};}else{l243[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l71));l243[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l73));l243[2] = 0;l243[3] = 0x80000001
;if((!(SOC_IS_HURRICANE(l1)))&&(((l105[(l1)]->l87)!= NULL))){int l238;(void)
soc_th_alpm_lpm_vrf_get(l1,key_data,(int*)&l243[4],&l238);}else{l243[4] = 0;}
;}l144 = l126((l116[(l1)]),l240,l243,l129,&index);if(SOC_FAILURE(l144)){*l130
= 0xFFFFFFFF;return(l144);}*l130 = index;return(SOC_E_NONE);}static uint16
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
l130){int l1 = l127->unit;uint16 l247;uint16 index;l247 = l119((uint8*)entry,
(32*5))%l127->l108;index = l127->l109[l247];;;while(index!= (0xFFFF)){uint32
l12[SOC_MAX_MEM_FIELD_WORDS];l112 l118;int l248;l248 = (index&(0x7FFF))>>1;
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l248,l12));l117(l1,l12,
index,l118);if((*l128)(entry,l118) == 0){*l130 = (index&(0x7FFF))>>((index&(
0x8000))?1:0);;return(SOC_E_NONE);}index = l127->l110[index&(0x7FFF)];;};
return(SOC_E_NOT_FOUND);}static int l131(l111*l127,l113 l128,l112 entry,int
l129,uint16 l132,uint16 l44){int l1 = l127->unit;uint16 l247;uint16 index;
uint16 l249;l247 = l119((uint8*)entry,(32*5))%l127->l108;index = l127->l109[
l247];;;;l249 = (0xFFFF);if(l132!= (0xFFFF)){while(index!= (0xFFFF)){uint32
l12[SOC_MAX_MEM_FIELD_WORDS];l112 l118;int l248;l248 = (index&(0x7FFF))>>1;
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l248,l12));l117(l1,l12,
index,l118);if((*l128)(entry,l118) == 0){if(l44!= index){;if(l249 == (0xFFFF)
){l127->l109[l247] = l44;l127->l110[l44&(0x7FFF)] = l127->l110[index&(0x7FFF)
];l127->l110[index&(0x7FFF)] = (0xFFFF);}else{l127->l110[l249&(0x7FFF)] = l44
;l127->l110[l44&(0x7FFF)] = l127->l110[index&(0x7FFF)];l127->l110[index&(
0x7FFF)] = (0xFFFF);}};return(SOC_E_NONE);}l249 = index;index = l127->l110[
index&(0x7FFF)];;}}l127->l110[l44&(0x7FFF)] = l127->l109[l247];l127->l109[
l247] = l44;return(SOC_E_NONE);}static int l133(l111*l127,l113 l128,l112 entry
,int l129,uint16 l134){uint16 l247;uint16 index;uint16 l249;l247 = l119((
uint8*)entry,(32*5))%l127->l108;index = l127->l109[l247];;;l249 = (0xFFFF);
while(index!= (0xFFFF)){if(l134 == index){;if(l249 == (0xFFFF)){l127->l109[
l247] = l127->l110[l134&(0x7FFF)];l127->l110[l134&(0x7FFF)] = (0xFFFF);}else{
l127->l110[l249&(0x7FFF)] = l127->l110[l134&(0x7FFF)];l127->l110[l134&(0x7FFF
)] = (0xFFFF);}return(SOC_E_NONE);}l249 = index;index = l127->l110[index&(
0x7FFF)];;}return(SOC_E_NOT_FOUND);}int soc_th_alpm_lpm_ip4entry0_to_0(int l1
,void*l250,void*l251,int l252){uint32 l253;l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l85),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l75));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l75),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l71));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l71),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l73),(l253));if(((l105[(l1)]->l61)!= NULL)){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l61));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l61),(l253));}if(((l105[(l1)]->l63)!= NULL)){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l63));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l63),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l65));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l65),(l253));}else{l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l79),(l253));}l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l81));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l81),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l83));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l83),(l253));if(((l105[(l1)]->l87)!= NULL)){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l87),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l89));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l89),(l253));}if(((l105[(l1)]->l59)!= NULL)){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l59),(l253));}if(l252){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l69));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l69),(l253));}l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l91),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l93));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l93),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l95),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l97));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l97),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l99));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l99),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l101));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l251),(l105[(l1)]->l101),(l253));return(SOC_E_NONE);}int
soc_th_alpm_lpm_ip4entry1_to_1(int l1,void*l250,void*l251,int l252){uint32
l253;l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l250),(l105[(l1)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l251),(l105[(l1)]->l86),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l76),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l72));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l72),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l74),(l253));if(((l105[(l1)]
->l62)!= NULL)){l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l250),(l105[(l1)]->l62));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l62),(l253));}if(((l105[(l1)]
->l64)!= NULL)){l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l250),(l105[(l1)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l64),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l66),(l253));}else{l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l80));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l80),(l253));}l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l82),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l84),(l253));if(((l105[(l1)]
->l88)!= NULL)){l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l250),(l105[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l88),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l90),(l253));}if(((l105[(l1)]->l60)!= NULL)){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l60),(l253));}if(l252){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l70),(l253));}l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l92),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l94));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l94),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l96),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l98));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l98),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l100));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l100),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l102));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l251),(l105[(l1)]->l102),(l253));return(SOC_E_NONE);}int
soc_th_alpm_lpm_ip4entry0_to_1(int l1,void*l250,void*l251,int l252){uint32
l253;l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l250),(l105[(l1)]->l85));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l251),(l105[(l1)]->l86),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l75));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l76),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l71));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l72),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l73));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l74),(l253));if(!
SOC_IS_HURRICANE(l1)){l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l61));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l62),(l253));}if(((l105[(l1)
]->l63)!= NULL)){l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l250),(l105[(l1)]->l63));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l64),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l65));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l66),(l253));}else{l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l79));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l80),(l253));}l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l81));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l82),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l83));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l84),(l253));if(((l105[(l1)]
->l87)!= NULL)){l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l250),(l105[(l1)]->l87));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l88),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l89));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l90),(l253));}if(((l105[(l1)]->l59)!= NULL)){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l59));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l60),(l253));}if(l252){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l69));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l70),(l253));}l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l91));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l92),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l93));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l94),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l95));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l96),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l97));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l98),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l99));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l100),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l101));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l251),(l105[(l1)]->l102),(l253));return(SOC_E_NONE);}int
soc_th_alpm_lpm_ip4entry1_to_0(int l1,void*l250,void*l251,int l252){uint32
l253;l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l250),(l105[(l1)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l251),(l105[(l1)]->l85),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l75),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l72));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l71),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l74));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l73),(l253));if(!
SOC_IS_HURRICANE(l1)){l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l62));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l61),(l253));}if(((l105[(l1)
]->l64)!= NULL)){l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l250),(l105[(l1)]->l64));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l63),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l66));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l65),(l253));}else{l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l80));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l79),(l253));}l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l81),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l84));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l83),(l253));if(((l105[(l1)]
->l88)!= NULL)){l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l250),(l105[(l1)]->l88));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l87),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l90));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l89),(l253));}if(((l105[(l1)]->l60)!= NULL)){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l60));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l59),(l253));}if(l252){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l70));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l69),(l253));}l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l92));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l91),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l94));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l93),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l96));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l251),(l105[(l1)]->l95),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[(l1)]->l98));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[
(l1)]->l97),(l253));l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l250),(l105[(l1)]->l100));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l1,L3_DEFIPm)),(l251),(l105[(l1)]->l99),(l253));l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l250),(l105[
(l1)]->l102));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm))
,(l251),(l105[(l1)]->l101),(l253));return(SOC_E_NONE);}static int l254(int l1
,void*l12){return(SOC_E_NONE);}void soc_th_alpm_lpm_state_dump(int l1){int
l170;int l255;l255 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,
bslSourceAlpm,bslSeverityVerbose,l1)){return;}for(l170 = l255;l170>= 0;l170--
){if((l170!= ((3*(64+32+2+1))-1))&&((l55[(l1)][(l170)].l48) == -1)){continue;
}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l170,(l55
[(l1)][(l170)].l50),(l55[(l1)][(l170)].next),(l55[(l1)][(l170)].l48),(l55[(l1
)][(l170)].l49),(l55[(l1)][(l170)].l51),(l55[(l1)][(l170)].l52)));}
COMPILER_REFERENCE(l254);}static int l256(int l1,int index,uint32*l12){int
l257;int l7;uint32 l258;uint32 l259;int l260;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l257 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l1)){l257 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1)+0x0400;}else{l257 = (
soc_mem_index_count(l1,L3_DEFIPm)>>1);}l7 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(l1)]->l75));if(((l105[(l1)]->l59)
!= NULL)){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l12),(l105[(l1)]->l59),(0));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l1,L3_DEFIPm)),(l12),(l105[(l1)]->l60),(0));}l258 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l73));l259 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l74));if(!l7){if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),
(l12),(l105[(l1)]->l83),((!l258)?1:0));}if(soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(l1)]->l86))){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l84),((!l259)?1:0));}}else{l260 = ((!l258)&&(!l259))?1:0;l258 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85));l259 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l86));if(l258&&l259){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l83),(l260));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l84),(l260));}}return l164(l1,MEM_BLOCK_ANY,
index+l257,index,l12);}static int l261(int l1,int l262,int l263){uint32 l12[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,
l262,l12));l241(l1,l12,l263,0x4000,0);SOC_IF_ERROR_RETURN(l164(l1,
MEM_BLOCK_ANY,l263,l262,l12));SOC_IF_ERROR_RETURN(l256(l1,l263,l12));do{int
l264,l265;l264 = soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l262),1);l265 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l263),1);ALPM_TCAM_PIVOT(l1,l265<<1)
= ALPM_TCAM_PIVOT(l1,l264<<1);ALPM_TCAM_PIVOT(l1,(l265<<1)+1) = 
ALPM_TCAM_PIVOT(l1,(l264<<1)+1);if(ALPM_TCAM_PIVOT((l1),l265<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l265<<1)) = l265<<1;}if(ALPM_TCAM_PIVOT
((l1),(l265<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(l265<<1)+1)) = (
l265<<1)+1;}ALPM_TCAM_PIVOT(l1,l264<<1) = NULL;ALPM_TCAM_PIVOT(l1,(l264<<1)+1
) = NULL;}while(0);return(SOC_E_NONE);}static int l266(int l1,int l129,int l7
){uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l262;int l263;uint32 l267,l268;l263
= (l55[(l1)][(l129)].l49)+1;if(!l7){l262 = (l55[(l1)][(l129)].l49);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l262,l12));l267 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85));l268 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l86));if((l267 == 0)||(l268 == 0)){l241(l1,l12
,l263,0x4000,0);SOC_IF_ERROR_RETURN(l164(l1,MEM_BLOCK_ANY,l263,l262,l12));
SOC_IF_ERROR_RETURN(l256(l1,l263,l12));do{int l269 = soc_th_alpm_physical_idx
((l1),L3_DEFIPm,(l262),1)<<1;int l203 = soc_th_alpm_physical_idx((l1),
L3_DEFIPm,(l263),1)*2+(l269&1);if((l268)){l269++;}ALPM_TCAM_PIVOT((l1),l203) = 
ALPM_TCAM_PIVOT((l1),l269);if(ALPM_TCAM_PIVOT((l1),l203)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT((l1),l203)) = l203;}ALPM_TCAM_PIVOT((l1),l269) = NULL;}while(
0);l263--;}}l262 = (l55[(l1)][(l129)].l48);if(l262!= l263){
SOC_IF_ERROR_RETURN(l261(l1,l262,l263));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l7)
;}(l55[(l1)][(l129)].l48)+= 1;(l55[(l1)][(l129)].l49)+= 1;return(SOC_E_NONE);
}static int l270(int l1,int l129,int l7){uint32 l12[SOC_MAX_MEM_FIELD_WORDS];
int l262;int l263;int l271;uint32 l267,l268;l263 = (l55[(l1)][(l129)].l48)-1;
if((l55[(l1)][(l129)].l51) == 0){(l55[(l1)][(l129)].l48) = l263;(l55[(l1)][(
l129)].l49) = l263-1;return(SOC_E_NONE);}if((!l7)&&((l55[(l1)][(l129)].l49)!= 
(l55[(l1)][(l129)].l48))){l262 = (l55[(l1)][(l129)].l49);SOC_IF_ERROR_RETURN(
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l262,l12));l267 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85));l268 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l86));if((l267 == 0)||(l268 == 0)){l271 = l262
-1;SOC_IF_ERROR_RETURN(l261(l1,l271,l263));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,
l7);l241(l1,l12,l271,0x4000,0);SOC_IF_ERROR_RETURN(l164(l1,MEM_BLOCK_ANY,l271
,l262,l12));SOC_IF_ERROR_RETURN(l256(l1,l271,l12));do{int l269 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l262),1)<<1;int l203 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l271),1)*2+(l269&1);if((l268)){l269
++;}ALPM_TCAM_PIVOT((l1),l203) = ALPM_TCAM_PIVOT((l1),l269);if(
ALPM_TCAM_PIVOT((l1),l203)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l203)) = 
l203;}ALPM_TCAM_PIVOT((l1),l269) = NULL;}while(0);}else{l241(l1,l12,l263,
0x4000,0);SOC_IF_ERROR_RETURN(l164(l1,MEM_BLOCK_ANY,l263,l262,l12));
SOC_IF_ERROR_RETURN(l256(l1,l263,l12));do{int l264,l265;l264 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l262),1);l265 = 
soc_th_alpm_physical_idx((l1),L3_DEFIPm,(l263),1);ALPM_TCAM_PIVOT(l1,l265<<1)
= ALPM_TCAM_PIVOT(l1,l264<<1);ALPM_TCAM_PIVOT(l1,(l265<<1)+1) = 
ALPM_TCAM_PIVOT(l1,(l264<<1)+1);if(ALPM_TCAM_PIVOT((l1),l265<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l265<<1)) = l265<<1;}if(ALPM_TCAM_PIVOT
((l1),(l265<<1)+1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),(l265<<1)+1)) = (
l265<<1)+1;}ALPM_TCAM_PIVOT(l1,l264<<1) = NULL;ALPM_TCAM_PIVOT(l1,(l264<<1)+1
) = NULL;}while(0);}}else{l262 = (l55[(l1)][(l129)].l49);SOC_IF_ERROR_RETURN(
l261(l1,l262,l263));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l7);}(l55[(l1)][(l129)]
.l48)-= 1;(l55[(l1)][(l129)].l49)-= 1;return(SOC_E_NONE);}static int l272(int
l1,int l129,int l7,void*l12,int*l273){int l274;int l275;int l276;int l277;int
l262;uint32 l267,l268;int l144;if((l55[(l1)][(l129)].l51) == 0){l277 = ((3*(
64+32+2+1))-1);if((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL)||(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)){if(l129<= (((3*(64+32+2
+1))/3)-1)){l277 = (((3*(64+32+2+1))/3)-1);}}while((l55[(l1)][(l277)].next)>
l129){l277 = (l55[(l1)][(l277)].next);}l275 = (l55[(l1)][(l277)].next);if(
l275!= -1){(l55[(l1)][(l275)].l50) = l129;}(l55[(l1)][(l129)].next) = (l55[(
l1)][(l277)].next);(l55[(l1)][(l129)].l50) = l277;(l55[(l1)][(l277)].next) = 
l129;(l55[(l1)][(l129)].l52) = ((l55[(l1)][(l277)].l52)+1)/2;(l55[(l1)][(l277
)].l52)-= (l55[(l1)][(l129)].l52);(l55[(l1)][(l129)].l48) = (l55[(l1)][(l277)
].l49)+(l55[(l1)][(l277)].l52)+1;(l55[(l1)][(l129)].l49) = (l55[(l1)][(l129)]
.l48)-1;(l55[(l1)][(l129)].l51) = 0;}else if(!l7){l262 = (l55[(l1)][(l129)].
l48);if((l144 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l262,l12))<0){return l144;}
l267 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),
(l105[(l1)]->l85));l268 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1
,L3_DEFIPm)),(l12),(l105[(l1)]->l86));if((l267 == 0)||(l268 == 0)){*l273 = (
l262<<1)+((l268 == 0)?1:0);return(SOC_E_NONE);}l262 = (l55[(l1)][(l129)].l49)
;if((l144 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l262,l12))<0){return l144;}l267 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l12),(l105[(
l1)]->l85));l268 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIPm)),(l12),(l105[(l1)]->l86));if((l267 == 0)||(l268 == 0)){*l273 = (
l262<<1)+((l268 == 0)?1:0);return(SOC_E_NONE);}}l276 = l129;while((l55[(l1)][
(l276)].l52) == 0){l276 = (l55[(l1)][(l276)].next);if(l276 == -1){l276 = l129
;break;}}while((l55[(l1)][(l276)].l52) == 0){l276 = (l55[(l1)][(l276)].l50);
if(l276 == -1){if((l55[(l1)][(l129)].l51) == 0){l274 = (l55[(l1)][(l129)].l50
);l275 = (l55[(l1)][(l129)].next);if(-1!= l274){(l55[(l1)][(l274)].next) = 
l275;}if(-1!= l275){(l55[(l1)][(l275)].l50) = l274;}}return(SOC_E_FULL);}}
while(l276>l129){l275 = (l55[(l1)][(l276)].next);SOC_IF_ERROR_RETURN(l270(l1,
l275,l7));(l55[(l1)][(l276)].l52)-= 1;(l55[(l1)][(l275)].l52)+= 1;l276 = l275
;}while(l276<l129){SOC_IF_ERROR_RETURN(l266(l1,l276,l7));(l55[(l1)][(l276)].
l52)-= 1;l274 = (l55[(l1)][(l276)].l50);(l55[(l1)][(l274)].l52)+= 1;l276 = 
l274;}(l55[(l1)][(l129)].l51)+= 1;(l55[(l1)][(l129)].l52)-= 1;(l55[(l1)][(
l129)].l49)+= 1;*l273 = (l55[(l1)][(l129)].l49)<<((l7)?0:1);sal_memcpy(l12,
soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);return(
SOC_E_NONE);}static int l278(int l1,int l129,int l7,void*l12,int l279){int
l274;int l275;int l262;int l263;uint32 l280[SOC_MAX_MEM_FIELD_WORDS];uint32
l281[SOC_MAX_MEM_FIELD_WORDS];uint32 l282[SOC_MAX_MEM_FIELD_WORDS];void*l283;
int l144;int l284,l37;l262 = (l55[(l1)][(l129)].l49);l263 = l279;if(!l7){l263
>>= 1;if((l144 = READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l262,l280))<0){return l144;}
if((l144 = READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,soc_th_alpm_physical_idx
(l1,L3_DEFIPm,l262,1),l281))<0){return l144;}if((l144 = 
READ_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,soc_th_alpm_physical_idx(l1,
L3_DEFIPm,l263,1),l282))<0){return l144;}l283 = (l263 == l262)?l280:l12;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l280),(l105[
(l1)]->l86))){l37 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l281,
BPM_LENGTH1f);if(l279&1){l144 = soc_th_alpm_lpm_ip4entry1_to_1(l1,l280,l283,
PRESERVE_HIT);soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l282,BPM_LENGTH1f,
l37);}else{l144 = soc_th_alpm_lpm_ip4entry1_to_0(l1,l280,l283,PRESERVE_HIT);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,l282,BPM_LENGTH0f,l37);}l284 = (
l262<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(
l280),(l105[(l1)]->l86),(0));}else{l37 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,l281,BPM_LENGTH0f);if(l279&1){l144 = 
soc_th_alpm_lpm_ip4entry0_to_1(l1,l280,l283,PRESERVE_HIT);soc_mem_field32_set
(l1,L3_DEFIP_AUX_TABLEm,l282,BPM_LENGTH1f,l37);}else{l144 = 
soc_th_alpm_lpm_ip4entry0_to_0(l1,l280,l283,PRESERVE_HIT);soc_mem_field32_set
(l1,L3_DEFIP_AUX_TABLEm,l282,BPM_LENGTH0f,l37);}l284 = l262<<1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIPm)),(l280),(l105[
(l1)]->l85),(0));(l55[(l1)][(l129)].l51)-= 1;(l55[(l1)][(l129)].l52)+= 1;(l55
[(l1)][(l129)].l49)-= 1;}l284 = soc_th_alpm_physical_idx(l1,L3_DEFIPm,l284,0)
;l279 = soc_th_alpm_physical_idx(l1,L3_DEFIPm,l279,0);ALPM_TCAM_PIVOT(l1,l279
) = ALPM_TCAM_PIVOT(l1,l284);if(ALPM_TCAM_PIVOT(l1,l279)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l1,l279)) = l279;}ALPM_TCAM_PIVOT(l1,l284) = NULL;if((l144 = 
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ALL,soc_th_alpm_physical_idx(l1,
L3_DEFIPm,l263,1),l282))<0){return l144;}if(l263!= l262){l241(l1,l283,l263,
0x4000,0);if((l144 = l164(l1,MEM_BLOCK_ANY,l263,l263,l283))<0){return l144;}
if((l144 = l256(l1,l263,l283))<0){return l144;}}l241(l1,l280,l262,0x4000,0);
if((l144 = l164(l1,MEM_BLOCK_ANY,l262,l262,l280))<0){return l144;}if((l144 = 
l256(l1,l262,l280))<0){return l144;}}else{(l55[(l1)][(l129)].l51)-= 1;(l55[(
l1)][(l129)].l52)+= 1;(l55[(l1)][(l129)].l49)-= 1;if(l263!= l262){if((l144 = 
READ_L3_DEFIPm(l1,MEM_BLOCK_ANY,l262,l280))<0){return l144;}l241(l1,l280,l263
,0x4000,0);if((l144 = l164(l1,MEM_BLOCK_ANY,l263,l262,l280))<0){return l144;}
if((l144 = l256(l1,l263,l280))<0){return l144;}}l279 = 
soc_th_alpm_physical_idx(l1,L3_DEFIPm,l263,1);l284 = soc_th_alpm_physical_idx
(l1,L3_DEFIPm,l262,1);ALPM_TCAM_PIVOT(l1,l279<<1) = ALPM_TCAM_PIVOT(l1,l284<<
1);ALPM_TCAM_PIVOT(l1,l284<<1) = NULL;if(ALPM_TCAM_PIVOT(l1,l279<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,l279<<1)) = l279<<1;}sal_memcpy(l280,
soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(l1,L3_DEFIPm)*4);l241(l1
,l280,l262,0x4000,0);if((l144 = l164(l1,MEM_BLOCK_ANY,l262,l262,l280))<0){
return l144;}if((l144 = l256(l1,l262,l280))<0){return l144;}}if((l55[(l1)][(
l129)].l51) == 0){l274 = (l55[(l1)][(l129)].l50);assert(l274!= -1);l275 = (
l55[(l1)][(l129)].next);(l55[(l1)][(l274)].next) = l275;(l55[(l1)][(l274)].
l52)+= (l55[(l1)][(l129)].l52);(l55[(l1)][(l129)].l52) = 0;if(l275!= -1){(l55
[(l1)][(l275)].l50) = l274;}(l55[(l1)][(l129)].next) = -1;(l55[(l1)][(l129)].
l50) = -1;(l55[(l1)][(l129)].l48) = -1;(l55[(l1)][(l129)].l49) = -1;}return(
l144);}int soc_th_alpm_lpm_vrf_get(int unit,void*lpm_entry,int*l24,int*l38){
int l152;if(((l105[(unit)]->l89)!= NULL)){l152 = soc_L3_DEFIPm_field32_get(
unit,lpm_entry,VRF_ID_0f);*l38 = l152;if(soc_L3_DEFIPm_field32_get(unit,
lpm_entry,VRF_ID_MASK0f)){*l24 = l152;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l105[(unit)]->l91))){*l24 = SOC_L3_VRF_GLOBAL;*l38 = SOC_VRF_MAX(unit)+1;}
else{*l24 = SOC_L3_VRF_OVERRIDE;}}else{*l24 = SOC_L3_VRF_DEFAULT;}return(
SOC_E_NONE);}static int l285(int l1,void*entry,int*l14){int l129;int l144;int
l7;uint32 l253;int l152;int l286;l7 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l105[(l1)]->l75));if(l7){l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l105
[(l1)]->l73));if((l144 = _ipmask2pfx(l253,&l129))<0){return(l144);}l253 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry),(l105
[(l1)]->l74));if(l129){if(l253!= 0xffffffff){return(SOC_E_PARAM);}l129+= 32;}
else{if((l144 = _ipmask2pfx(l253,&l129))<0){return(l144);}}l129+= 33;}else{
l253 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(entry
),(l105[(l1)]->l73));if((l144 = _ipmask2pfx(l253,&l129))<0){return(l144);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l1,entry,&l152,&l144));l286 = 
soc_th_alpm_mode_get(l1);switch(l152){case SOC_L3_VRF_GLOBAL:if((l286 == 
SOC_ALPM_MODE_PARALLEL)||(l286 == SOC_ALPM_MODE_TCAM_ALPM)){*l14 = l129+((3*(
64+32+2+1))/3);}else{*l14 = l129;}break;case SOC_L3_VRF_OVERRIDE:*l14 = l129+
2*((3*(64+32+2+1))/3);break;default:if((l286 == SOC_ALPM_MODE_PARALLEL)||(
l286 == SOC_ALPM_MODE_TCAM_ALPM)){*l14 = l129;}else{*l14 = l129+((3*(64+32+2+
1))/3);}break;}return(SOC_E_NONE);}static int l11(int l1,void*key_data,void*
l12,int*l13,int*l14,int*l7){int l144;int l30;int l130;int l129 = 0;l30 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(
l105[(l1)]->l75));if(l30){if(!(l30 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIPm)),(key_data),(l105[(l1)]->l76)))){return(
SOC_E_PARAM);}}*l7 = l30;l285(l1,key_data,&l129);*l14 = l129;if(l245(l1,
key_data,l129,&l130) == SOC_E_NONE){*l13 = l130;if((l144 = READ_L3_DEFIPm(l1,
MEM_BLOCK_ANY,(*l7)?*l13:(*l13>>1),l12))<0){return l144;}return(SOC_E_NONE);}
else{return(SOC_E_NOT_FOUND);}}static int l2(int l1){int l255;int l170;int
l237;int l287;uint32 l136,l286;if(!soc_feature(l1,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}if((l286 = soc_property_get(l1,spn_L3_ALPM_ENABLE,0)))
{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l1,&l136));soc_reg_field_set(
l1,L3_DEFIP_RPF_CONTROLr,&l136,LPM_MODEf,1);if(l286 == SOC_ALPM_MODE_PARALLEL
){soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l136,LOOKUP_MODEf,1);}else if(
l286 == SOC_ALPM_MODE_TCAM_ALPM){soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&
l136,LOOKUP_MODEf,2);}else{soc_reg_field_set(l1,L3_DEFIP_RPF_CONTROLr,&l136,
LOOKUP_MODEf,0);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l1,l136));
l136 = 0;if(SOC_URPF_STATUS_GET(l1)){soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&
l136,URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l136,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l136,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l1,L3_DEFIP_KEY_SELr,&l136,
URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l1,l136))
;l136 = 0;if(l286 == SOC_ALPM_MODE_PARALLEL){if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l136,TCAM2_SELf,1);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l136,TCAM3_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM4_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM6_SELf,3);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM7_SELf,3);}else{soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM4_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM5_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM6_SELf,1);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l1)){
soc_reg_field_set(l1,L3_DEFIP_ALPM_CFGr,&l136,TCAM4_SELf,2);soc_reg_field_set
(l1,L3_DEFIP_ALPM_CFGr,&l136,TCAM5_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM6_SELf,2);soc_reg_field_set(l1,
L3_DEFIP_ALPM_CFGr,&l136,TCAM7_SELf,2);}}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_ALPM_CFGr(l1,l136));if(soc_property_get(l1,
spn_IPV6_LPM_128B_ENABLE,1)){uint32 l288 = 0;if(l286!= SOC_ALPM_MODE_PARALLEL
){uint32 l289;l289 = soc_property_get(l1,spn_NUM_IPV6_LPM_128B_ENTRIES,2048);
if(l289!= 2048){if(SOC_URPF_STATUS_GET(l1)){LOG_CLI((BSL_META_U(l1,
"URPF supported in combined mode only""with 2048 v6-128 entries\n")));return
SOC_E_PARAM;}if((l289!= 1024)&&(l289!= 3072)){LOG_CLI((BSL_META_U(l1,
"Only supported values for v6-128 in"
"nonURPF combined mode are 1024 and 3072\n")));return SOC_E_PARAM;}}}
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_KEY_SELr(l1,&l288));soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l288,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l288,V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l288,V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l1,
L3_DEFIP_KEY_SELr,&l288,V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_KEY_SELr(l1,l288));}}l255 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(
l1);l287 = sizeof(l53)*(l255);if((l55[(l1)]!= NULL)){if(soc_th_alpm_deinit(l1
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
[(l1)]) = sal_alloc(l287,"LPM prefix info");if(NULL == (l55[(l1)])){sal_free(
l105[l1]);l105[l1] = NULL;SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_MEMORY);}
sal_memset((l55[(l1)]),0,l287);for(l170 = 0;l170<l255;l170++){(l55[(l1)][(
l170)].l48) = -1;(l55[(l1)][(l170)].l49) = -1;(l55[(l1)][(l170)].l50) = -1;(
l55[(l1)][(l170)].next) = -1;(l55[(l1)][(l170)].l51) = 0;(l55[(l1)][(l170)].
l52) = 0;}l237 = soc_mem_index_count(l1,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l1)
){l237>>= 1;}if((l286 == SOC_ALPM_MODE_PARALLEL)||(l286 == 
SOC_ALPM_MODE_TCAM_ALPM)){(l55[(l1)][(((3*(64+32+2+1))-1))].l49) = (l237>>1)-
1;(l55[(l1)][(((((3*(64+32+2+1))/3)-1)))].l52) = l237>>1;(l55[(l1)][((((3*(64
+32+2+1))-1)))].l52) = (l237-(l55[(l1)][(((((3*(64+32+2+1))/3)-1)))].l52));}
else{(l55[(l1)][((((3*(64+32+2+1))-1)))].l52) = l237;}if((l116[(l1)])!= NULL)
{if(l124((l116[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(
l116[(l1)]) = NULL;}if(l122(l1,l237*2,l237,&(l116[(l1)]))<0){
SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l1);return(
SOC_E_NONE);}static int l3(int l1){if(!soc_feature(l1,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);if((l116[(l1)])!= NULL){l124((
l116[(l1)]));(l116[(l1)]) = NULL;}if((l55[(l1)]!= NULL)){sal_free(l105[l1]);
l105[l1] = NULL;sal_free((l55[(l1)]));(l55[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK
(l1);return(SOC_E_NONE);}static int l4(int l1,void*l5,int*l290){int l129;int
index;int l7;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l144 = SOC_E_NONE;int
l291 = 0;sal_memcpy(l12,soc_mem_entry_null(l1,L3_DEFIPm),soc_mem_entry_words(
l1,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l1);l144 = l11(l1,l5,l12,&index,&l129,&l7)
;if(l144 == SOC_E_NOT_FOUND){l144 = l272(l1,l129,l7,l12,&index);if(l144<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l144);}}else{l291 = 1;}*l290 = index;if(l144 == 
SOC_E_NONE){if(!l7){if(index&1){l144 = soc_th_alpm_lpm_ip4entry0_to_1(l1,l5,
l12,PRESERVE_HIT);}else{l144 = soc_th_alpm_lpm_ip4entry0_to_0(l1,l5,l12,
PRESERVE_HIT);}if(l144<0){SOC_ALPM_LPM_UNLOCK(l1);return(l144);}l5 = (void*)
l12;index>>= 1;}soc_th_alpm_lpm_state_dump(l1);LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\nsoc_alpm_lpm_insert: %d %d\n"),index,l129));if(!l291){l241(
l1,l5,index,0x4000,0);}l144 = l164(l1,MEM_BLOCK_ANY,index,index,l5);if(l144>= 
0){l144 = l256(l1,index,l5);}}SOC_ALPM_LPM_UNLOCK(l1);return(l144);}int
soc_th_alpm_lpm_delete(int l1,void*key_data){int l129;int index;int l7;uint32
l12[SOC_MAX_MEM_FIELD_WORDS];int l144 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l144
= l11(l1,key_data,l12,&index,&l129,&l7);if(l144 == SOC_E_NONE){LOG_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_alpm_lpm_delete: %d %d\n"),index,l129))
;l244(l1,key_data,index);l144 = l278(l1,l129,l7,l12,index);}
soc_th_alpm_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l144);}static
int l15(int l1,void*key_data,void*l12,int*l13){int l129;int l144;int l7;
SOC_ALPM_LPM_LOCK(l1);l144 = l11(l1,key_data,l12,l13,&l129,&l7);
SOC_ALPM_LPM_UNLOCK(l1);return(l144);}static int l6(int unit,void*key_data,
int l7,int l8,int l9,defip_aux_scratch_entry_t*l10){uint32 l253;uint32 l292[4
] = {0,0,0,0};int l129 = 0;int l144 = SOC_E_NONE;l253 = soc_mem_field32_get(
unit,L3_DEFIPm,key_data,VALID0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l10,VALIDf,l253);l253 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,MODE0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10
,MODEf,l253);l253 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,ENTRY_TYPE0f)
;soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,ENTRY_TYPEf,0);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,GLOBAL_ROUTE0f);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,GLOBAL_ROUTEf,l253);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,ECMP0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l10,ECMPf,l253);l253 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,ECMP_PTR0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l10,ECMP_PTRf,l253);l253 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
NEXT_HOP_INDEX0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,
NEXT_HOP_INDEXf,l253);l253 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
PRI0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,PRIf,l253);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,RPE0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l10,RPEf,l253);l253 =soc_mem_field32_get(unit,L3_DEFIPm
,key_data,VRF_ID_0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,VRFf,
l253);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,DB_TYPEf,l8);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,DST_DISCARD0f);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,DST_DISCARDf,l253);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,CLASS_ID0f);soc_mem_field32_set(
unit,L3_DEFIP_AUX_SCRATCHm,l10,CLASS_IDf,l253);if(l7){l292[2] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR0f);l292[3] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR1f);}else{l292[0] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR0f);}soc_mem_field_set(
unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)l10,IP_ADDRf,(uint32*)l292);if(l7){l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR_MASK0f);if((l144 = 
_ipmask2pfx(l253,&l129))<0){return(l144);}l253 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,IP_ADDR_MASK1f);if(l129){if(l253!= 0xffffffff){return(
SOC_E_PARAM);}l129+= 32;}else{if((l144 = _ipmask2pfx(l253,&l129))<0){return(
l144);}}}else{l253 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l253,&l129))<0){return(l144);}}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,IP_LENGTHf,l129);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l10,REPLACE_LENf,l9);return(
SOC_E_NONE);}int _soc_th_alpm_aux_op(int l1,_soc_aux_op_t l293,
defip_aux_scratch_entry_t*l10,int l294,int*l148,int*tcam_index,int*
bucket_index){uint32 l136,l295;int l296;soc_timeout_t l297;int l144 = 
SOC_E_NONE;int l298 = 0;if(l294){SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_SCRATCHm(l1,MEM_BLOCK_ANY,0,l10));}l299:l136 = 0;switch(
l293){case INSERT_PROPAGATE:l296 = 0;break;case DELETE_PROPAGATE:l296 = 1;
break;case PREFIX_LOOKUP:l296 = 2;break;case HITBIT_REPLACE:l296 = 3;break;
default:return SOC_E_PARAM;}soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l136,
OPCODEf,l296);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l136,STARTf,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l136));soc_timeout_init(&l297
,50000,5);l296 = 0;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l1,&l136));
l296 = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l136,DONEf);if(l296 == 1){l144
= SOC_E_NONE;break;}if(soc_timeout_check(&l297)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l1,&l136));l296 = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRLr,l136,DONEf);if(l296 == 1){l144 = SOC_E_NONE;}else{LOG_WARN
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d : DEFIP AUX Operation timeout\n"),l1
));l144 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l144)){if(
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l136,ERRORf)){soc_reg_field_set(l1,
L3_DEFIP_AUX_CTRLr,&l136,STARTf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&
l136,ERRORf,0);soc_reg_field_set(l1,L3_DEFIP_AUX_CTRLr,&l136,DONEf,0);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l1,l136));LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: DEFIP AUX Operation encountered "
"parity error !!\n"),l1));l298++;if(SOC_CONTROL(l1)->alpm_bulk_retry){
sal_sem_take(SOC_CONTROL(l1)->alpm_bulk_retry,1000000);}if(l298<5){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d: Retry DEFIP AUX Operation..\n"),l1))
;goto l299;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l1));return SOC_E_INTERNAL;}}if(l293 == PREFIX_LOOKUP){if(l148&&tcam_index){*
l148 = soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l136,HITf);*tcam_index = 
soc_reg_field_get(l1,L3_DEFIP_AUX_CTRLr,l136,BKT_INDEXf);SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRL_1r(l1,&l295));*bucket_index = soc_reg_field_get(l1,
L3_DEFIP_AUX_CTRL_1r,l295,BKT_PTRf);}}}return l144;}static int l17(int unit,
void*lpm_entry,void*l18,void*l19,soc_mem_t l20,uint32 l21,uint32*l300){uint32
l253;uint32 l292[4] = {0,0};int l129 = 0;int l144 = SOC_E_NONE;int l7;uint32
l22 = 0;l7 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,MODE0f);sal_memset(
l18,0,soc_mem_entry_words(unit,l20)*4);l253 = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,HIT0f);soc_mem_field32_set(unit,l20,l18,HITf,l253);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,VALID0f);soc_mem_field32_set(
unit,l20,l18,VALIDf,l253);l253 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry
,ECMP0f);soc_mem_field32_set(unit,l20,l18,ECMPf,l253);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,ECMP_PTR0f);soc_mem_field32_set(
unit,l20,l18,ECMP_PTRf,l253);l253 = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,NEXT_HOP_INDEX0f);soc_mem_field32_set(unit,l20,l18,NEXT_HOP_INDEXf,
l253);l253 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,PRI0f);
soc_mem_field32_set(unit,l20,l18,PRIf,l253);l253 = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,RPE0f);soc_mem_field32_set(unit,l20,l18,RPEf,l253);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(unit,l20,l18,DST_DISCARDf,l253);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);
soc_mem_field32_set(unit,l20,l18,SRC_DISCARDf,l253);l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,CLASS_ID0f);soc_mem_field32_set(
unit,l20,l18,CLASS_IDf,l253);l292[0] = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,IP_ADDR0f);if(l7){l292[1] = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,IP_ADDR1f);}soc_mem_field_set(unit,l20,(uint32*)l18,KEYf,(uint32*)
l292);if(l7){l253 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l253,&l129))<0){return(l144);}l253 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f);if(l129){if(l253
!= 0xffffffff){return(SOC_E_PARAM);}l129+= 32;}else{if((l144 = _ipmask2pfx(
l253,&l129))<0){return(l144);}}}else{l253 = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l253,&l129))<0){
return(l144);}}if((l129 == 0)&&(l292[0] == 0)&&(l292[1] == 0)){l22 = 1;}if(
l300!= NULL){*l300 = l22;}soc_mem_field32_set(unit,l20,l18,LENGTHf,l129);if(
l19 == NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){sal_memset(l19
,0,soc_mem_entry_words(unit,l20)*4);sal_memcpy(l19,l18,soc_mem_entry_words(
unit,l20)*4);soc_mem_field32_set(unit,l20,l19,DST_DISCARDf,0);
soc_mem_field32_set(unit,l20,l19,RPEf,0);soc_mem_field32_set(unit,l20,l19,
SRC_DISCARDf,l21&SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(unit,l20,l19,
DEFAULTROUTEf,l22);}return(SOC_E_NONE);}static int l23(int unit,void*l18,
soc_mem_t l20,int l7,int l24,int l25,int index,void*lpm_entry){uint32 l253;
uint32 l292[4] = {0,0};uint32 l129 = 0;sal_memset(lpm_entry,0,
soc_mem_entry_words(unit,L3_DEFIPm)*4);l253 = soc_mem_field32_get(unit,l20,
l18,HITf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,HIT0f,l253);if(l7){
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,HIT1f,l253);}l253 = 
soc_mem_field32_get(unit,l20,l18,VALIDf);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,VALID0f,l253);if(l7){soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
VALID1f,l253);}l253 = soc_mem_field32_get(unit,l20,l18,ECMPf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ECMP0f,l253);l253 = 
soc_mem_field32_get(unit,l20,l18,ECMP_PTRf);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,ECMP_PTR0f,l253);l253 = soc_mem_field32_get(unit,l20,l18,
NEXT_HOP_INDEXf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
NEXT_HOP_INDEX0f,l253);l253 = soc_mem_field32_get(unit,l20,l18,PRIf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,PRI0f,l253);l253 = 
soc_mem_field32_get(unit,l20,l18,RPEf);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,RPE0f,l253);l253 = soc_mem_field32_get(unit,l20,l18,DST_DISCARDf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,DST_DISCARD0f,l253);l253 = 
soc_mem_field32_get(unit,l20,l18,SRC_DISCARDf);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,SRC_DISCARD0f,l253);l253 = soc_mem_field32_get(unit,l20,
l18,CLASS_IDf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,CLASS_ID0f,l253);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ALG_BKT_PTR0f,l25);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ALG_HIT_IDX0f,index);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE_MASK0f,3);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l7){
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(
unit,l20,l18,KEYf,l292);if(l7){soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR1f,l292[1]);}soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR0f,
l292[0]);l253 = soc_mem_field32_get(unit,l20,l18,LENGTHf);if(l7){if(l253>= 32
){l129 = 0xffffffff;soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l129);l129 = ~(((l253-32) == 32)?0:(0xffffffff)>>(l253-32));
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l129);}else{l129 = 
~(0xffffffff>>l253);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l129);}}else{assert(l253<= 32);l129 = ~(((l253) == 32)?0:(
0xffffffff)>>(l253));soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
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
*lpm_entry,int l301,int l302){int l144 = SOC_E_NONE;uint32 key[2] = {0,0};
alpm_pivot_t*l34 = NULL;alpm_bucket_handle_t*l196 = NULL;int l152 = 0,l24 = 0
;uint32 l303;trie_t*l231 = NULL;uint32 prefix[5] = {0};int l22 = 0;l144 = 
l141(unit,lpm_entry,prefix,&l303,&l22);SOC_IF_ERROR_RETURN(l144);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l152,&l24));l301 = 
soc_th_alpm_physical_idx(unit,L3_DEFIPm,l301,l7);l196 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l196 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l196,0,sizeof(*l196));
l34 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l34 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l196);return SOC_E_MEMORY;}sal_memset(l34,0,
sizeof(*l34));PIVOT_BUCKET_HANDLE(l34) = l196;if(l7){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l34));key[0] = soc_L3_DEFIPm_field32_get
(unit,lpm_entry,IP_ADDR0f);key[1] = soc_L3_DEFIPm_field32_get(unit,lpm_entry,
IP_ADDR1f);}else{trie_init(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l34));key[0] = 
soc_L3_DEFIPm_field32_get(unit,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l34) = 
l302;PIVOT_TCAM_INDEX(l34) = l301;if(l152!= SOC_L3_VRF_OVERRIDE){if(l7 == 0){
l231 = VRF_PIVOT_TRIE_IPV4(unit,l24);if(l231 == NULL){trie_init(
_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(unit,l24));l231 = VRF_PIVOT_TRIE_IPV4(
unit,l24);}}else{l231 = VRF_PIVOT_TRIE_IPV6(unit,l24);if(l231 == NULL){
trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(unit,l24));l231 = 
VRF_PIVOT_TRIE_IPV6(unit,l24);}}sal_memcpy(l34->key,prefix,sizeof(prefix));
l34->len = l303;l144 = trie_insert(l231,l34->key,NULL,l34->len,(trie_node_t*)
l34);if(SOC_FAILURE(l144)){sal_free(l196);sal_free(l34);return l144;}}
ALPM_TCAM_PIVOT(unit,l301) = l34;PIVOT_BUCKET_VRF(l34) = l24;
PIVOT_BUCKET_IPV6(l34) = l7;PIVOT_BUCKET_ENT_CNT_UPDATE(l34);if(key[0] == 0&&
key[1] == 0){PIVOT_BUCKET_DEF(l34) = TRUE;}VRF_PIVOT_REF_INC(unit,l24,l7);
return l144;}static int l304(int unit,int l7,void*lpm_entry,void*l18,
soc_mem_t l20,int l301,int l302,int l305){int l306;int l24;int l144 = 
SOC_E_NONE;int l22 = 0;uint32 prefix[5] = {0,0,0,0,0};uint32 l35;void*l307 = 
NULL;trie_t*l198 = NULL;trie_t*l33 = NULL;trie_node_t*l193 = NULL;payload_t*
l308 = NULL;payload_t*l211 = NULL;alpm_pivot_t*pivot_pyld = NULL;if((NULL == 
lpm_entry)||(NULL == l18)){return SOC_E_PARAM;}if(l7){if(!(l7 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l306,&l24));l20 = 
(l7)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l307 = sal_alloc(sizeof(
defip_entry_t),"Temp lpm_entr");if(NULL == l307){return SOC_E_MEMORY;}
SOC_IF_ERROR_RETURN(l23(unit,l18,l20,l7,l306,l302,l301,l307));l144 = l141(
unit,l307,prefix,&l35,&l22);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"prefix create failed\n")));return l144;}sal_free(l307);
pivot_pyld = ALPM_TCAM_PIVOT(unit,l301);l198 = PIVOT_BUCKET_TRIE(pivot_pyld);
l308 = sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == l308){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l211 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l211){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node\n")));sal_free(l308);return
SOC_E_MEMORY;}sal_memset(l308,0,sizeof(*l308));sal_memset(l211,0,sizeof(*l211
));l308->key[0] = prefix[0];l308->key[1] = prefix[1];l308->key[2] = prefix[2]
;l308->key[3] = prefix[3];l308->key[4] = prefix[4];l308->len = l35;l308->
index = l305;sal_memcpy(l211,l308,sizeof(*l308));l144 = trie_insert(l198,
prefix,NULL,l35,(trie_node_t*)l308);if(SOC_FAILURE(l144)){goto l309;}if(l7){
l33 = VRF_PREFIX_TRIE_IPV6(unit,l24);}else{l33 = VRF_PREFIX_TRIE_IPV4(unit,
l24);}if(!l22){l144 = trie_insert(l33,prefix,NULL,l35,(trie_node_t*)l211);if(
SOC_FAILURE(l144)){goto l310;}}return l144;l310:(void)trie_delete(l198,prefix
,l35,&l193);l308 = (payload_t*)l193;l309:sal_free(l308);sal_free(l211);return
l144;}static int l311(int unit,int l30,int l24,int l137,int bkt_ptr){int l144
= SOC_E_NONE;uint32 l35;uint32 key[2] = {0,0};trie_t*l312 = NULL;payload_t*
l221 = NULL;defip_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unable to allocate memory for LPM entry\n")
));return SOC_E_MEMORY;}l26(unit,key,0,l24,l30,lpm_entry,0,1);if(l24 == 
SOC_VRF_MAX(unit)+1){soc_L3_DEFIPm_field32_set(unit,lpm_entry,GLOBAL_ROUTE0f,
1);}else{soc_L3_DEFIPm_field32_set(unit,lpm_entry,DEFAULT_MISS0f,1);}
soc_L3_DEFIPm_field32_set(unit,lpm_entry,ALG_BKT_PTR0f,bkt_ptr);if(l30 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(unit,l24) = lpm_entry;trie_init(_MAX_KEY_LEN_48_,
&VRF_PREFIX_TRIE_IPV4(unit,l24));l312 = VRF_PREFIX_TRIE_IPV4(unit,l24);}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(unit,l24) = lpm_entry;trie_init(_MAX_KEY_LEN_144_
,&VRF_PREFIX_TRIE_IPV6(unit,l24));l312 = VRF_PREFIX_TRIE_IPV6(unit,l24);}l221
= sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(l221 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node \n")));return SOC_E_MEMORY;}
sal_memset(l221,0,sizeof(*l221));l35 = 0;l221->key[0] = key[0];l221->key[1] = 
key[1];l221->len = l35;l144 = trie_insert(l312,key,NULL,l35,&(l221->node));if
(SOC_FAILURE(l144)){sal_free(l221);return l144;}VRF_TRIE_INIT_DONE(unit,l24,
l30,1);return l144;}int soc_th_alpm_warmboot_prefix_insert(int unit,int l7,
void*lpm_entry,void*l18,int l301,int l302,int l305){int l306;int l24;int l144
= SOC_E_NONE;soc_mem_t l20;l301 = soc_th_alpm_physical_idx(unit,L3_DEFIPm,
l301,l7);l20 = (l7)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l306,&l24));if(
l306 == SOC_L3_VRF_OVERRIDE){return(l144);}if(!VRF_TRIE_INIT_COMPLETED(unit,
l24,l7)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d is not initialized\n"),l24));l144 = l311(unit,l7,l24,l301,l302);if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d/%d trie init \n""failed\n"),l24,l7));return l144;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d/%d trie init completed\n"),l24,l7));
}l144 = l304(unit,l7,lpm_entry,l18,l20,l301,l302,l305);if(l144!= SOC_E_NONE){
LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"unit %d : Route Insertion Failed :%s\n"),unit,soc_errmsg(l144)));return(l144
);}VRF_TRIE_ROUTES_INC(unit,l24,l7);return(l144);}int
soc_th_alpm_warmboot_bucket_bitmap_set(int l1,int l30,int l234){int l313 = 1;
if(l30){if(!soc_th_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l313 = 2;}}if
(SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l234,l313);return SOC_E_NONE;}int
soc_th_alpm_warmboot_lpm_reinit_done(int unit){int l137;int l314 = ((3*(64+32
+2+1))-1);int l237 = soc_mem_index_count(unit,L3_DEFIPm);if(
SOC_URPF_STATUS_GET(unit)){l237>>= 1;}if(!soc_th_alpm_mode_get(unit)){(l55[(
unit)][(((3*(64+32+2+1))-1))].l50) = -1;for(l137 = ((3*(64+32+2+1))-1);l137>-
1;l137--){if(-1 == (l55[(unit)][(l137)].l48)){continue;}(l55[(unit)][(l137)].
l50) = l314;(l55[(unit)][(l314)].next) = l137;(l55[(unit)][(l314)].l52) = (
l55[(unit)][(l137)].l48)-(l55[(unit)][(l314)].l49)-1;l314 = l137;}(l55[(unit)
][(l314)].next) = -1;(l55[(unit)][(l314)].l52) = l237-(l55[(unit)][(l314)].
l49)-1;}else{(l55[(unit)][(((3*(64+32+2+1))-1))].l50) = -1;for(l137 = ((3*(64
+32+2+1))-1);l137>(((3*(64+32+2+1))-1)/3);l137--){if(-1 == (l55[(unit)][(l137
)].l48)){continue;}(l55[(unit)][(l137)].l50) = l314;(l55[(unit)][(l314)].next
) = l137;(l55[(unit)][(l314)].l52) = (l55[(unit)][(l137)].l48)-(l55[(unit)][(
l314)].l49)-1;l314 = l137;}(l55[(unit)][(l314)].next) = -1;(l55[(unit)][(l314
)].l52) = l237-(l55[(unit)][(l314)].l49)-1;l314 = (((3*(64+32+2+1))-1)/3);(
l55[(unit)][((((3*(64+32+2+1))-1)/3))].l50) = -1;for(l137 = ((((3*(64+32+2+1)
)-1)/3)-1);l137>-1;l137--){if(-1 == (l55[(unit)][(l137)].l48)){continue;}(l55
[(unit)][(l137)].l50) = l314;(l55[(unit)][(l314)].next) = l137;(l55[(unit)][(
l314)].l52) = (l55[(unit)][(l137)].l48)-(l55[(unit)][(l314)].l49)-1;l314 = 
l137;}(l55[(unit)][(l314)].next) = -1;(l55[(unit)][(l314)].l52) = (l237>>1)-(
l55[(unit)][(l314)].l49)-1;}return(SOC_E_NONE);}int
soc_th_alpm_warmboot_lpm_reinit(int unit,int l7,int l137,void*lpm_entry){int
l14;defip_entry_t*l315;if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID0f)||
soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID1f)){l241(unit,lpm_entry,l137,
0x4000,0);}if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l285(unit,lpm_entry,&l14));if((l55[(unit)][(l14)].l51) == 
0){(l55[(unit)][(l14)].l48) = l137;(l55[(unit)][(l14)].l49) = l137;}else{(l55
[(unit)][(l14)].l49) = l137;}(l55[(unit)][(l14)].l51)++;if(l7){return(
SOC_E_NONE);}}else{if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID1f)){l315
= sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");
soc_th_alpm_lpm_ip4entry1_to_0(unit,lpm_entry,l315,TRUE);SOC_IF_ERROR_RETURN(
l285(unit,l315,&l14));if((l55[(unit)][(l14)].l51) == 0){(l55[(unit)][(l14)].
l48) = l137;(l55[(unit)][(l14)].l49) = l137;}else{(l55[(unit)][(l14)].l49) = 
l137;}sal_free(l315);(l55[(unit)][(l14)].l51)++;}}return(SOC_E_NONE);}typedef
struct l316{int v4;int v6_64;int v6_128;int l317;int l318;int l319;int l320;}
l321;typedef enum l322{l323 = 0,l324,l325,l326,l327,l328}l329;static void l330
(int l1,alpm_vrf_counter_t*l331){l331->v4 = soc_mem_index_count(l1,L3_DEFIPm)
*2;l331->v6_128 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(
soc_property_get(l1,spn_IPV6_LPM_128B_ENABLE,1)){l331->v6_64 = l331->v6_128;}
else{l331->v6_64 = l331->v4>>1;}if(SOC_URPF_STATUS_GET(l1)){l331->v4>>= 1;
l331->v6_128>>= 1;l331->v6_64>>= 1;}}static void l332(int l1,int l152,
alpm_vrf_handle_t*l333,l329 l334){alpm_vrf_counter_t*l335;int l170,l336,l337,
l338;int l291 = 0;alpm_vrf_counter_t l331;switch(l334){case l323:LOG_CLI((
BSL_META_U(l1,"\nAdd Counter:\n")));break;case l324:LOG_CLI((BSL_META_U(l1,
"\nDelete Counter:\n")));break;case l325:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - 1:\n")));break;case l326:l330(l1,&l331);LOG_CLI((
BSL_META_U(l1,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l331.v4,
l331.v6_64,l331.v6_128));break;case l327:LOG_CLI((BSL_META_U(l1,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l328:LOG_CLI((
BSL_META_U(l1,"\nInternal Debug Counter - LPM Full:\n")));break;default:break
;}LOG_CLI((BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n"))
);LOG_CLI((BSL_META_U(l1,"-----------------------------------------------\n")
));l336 = l337 = l338 = 0;for(l170 = 0;l170<MAX_VRF_ID+1;l170++){int l339,
l340,l341;if(l333[l170].init_done == 0&&l170!= MAX_VRF_ID){continue;}if(l152
!= -1&&l152!= l170){continue;}l291 = 1;switch(l334){case l323:l335 = &l333[
l170].add;break;case l324:l335 = &l333[l170].del;break;case l325:l335 = &l333
[l170].bkt_split;break;case l327:l335 = &l333[l170].lpm_shift;break;case l328
:l335 = &l333[l170].lpm_full;break;case l326:l335 = &l333[l170].pivot_used;
break;default:l335 = &l333[l170].pivot_used;break;}l339 = l335->v4;l340 = 
l335->v6_64;l341 = l335->v6_128;l336+= l339;l337+= l340;l338+= l341;do{
LOG_CLI((BSL_META_U(l1,"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l170 == 
MAX_VRF_ID?-1:l170),(l339),(l340),(l341),((l339+l340+l341)),(l170) == 
MAX_VRF_ID?"GHi":(l170) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l291 == 
0){LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));do{
LOG_CLI((BSL_META_U(l1,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l336),(
l337),(l338),((l336+l337+l338))));}while(0);}return;}int l342(int l1,int l152
,uint32 flags){int l170,l343,l291 = 0;l321*l344;l321 l345;l321 l346;if(l152>(
SOC_VRF_MAX(l1)+1)){return SOC_E_PARAM;}l343 = MAX_VRF_ID*sizeof(l321);l344 = 
sal_alloc(l343,"_alpm_dbg_cnt");if(l344 == NULL){return SOC_E_MEMORY;}
sal_memset(l344,0,l343);l345.v4 = ALPM_IPV4_BKT_COUNT;l345.v6_64 = 
ALPM_IPV6_64_BKT_COUNT;l345.v6_128 = ALPM_IPV6_128_BKT_COUNT;if(!
soc_th_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l345.v6_64<<= 1;l345.
v6_128<<= 1;}LOG_CLI((BSL_META_U(l1,"\nBucket Occupancy:\n")));if(flags&(
SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l1,
"\n  BKT/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));}for(
l170 = 0;l170<MAX_PIVOT_COUNT;l170++){alpm_pivot_t*l347 = ALPM_TCAM_PIVOT(l1,
l170);if(l347!= NULL){l321*l348;int l24 = PIVOT_BUCKET_VRF(l347);if(l24<0||
l24>(SOC_VRF_MAX(l1)+1)){continue;}if(l152!= -1&&l152!= l24){continue;}if(
flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l291 = 1;do{LOG_CLI((BSL_META_U(l1,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l170,l24,PIVOT_BUCKET_MIN(l347),
PIVOT_BUCKET_MAX(l347),PIVOT_BUCKET_COUNT(l347),PIVOT_BUCKET_DEF(l347)?"Def":
(l24) == SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}l348 = &l344[l24];if(
PIVOT_BUCKET_IPV6(l347) == L3_DEFIP_MODE_128){l348->v6_128+= 
PIVOT_BUCKET_COUNT(l347);l348->l319+= l345.v6_128;}else if(PIVOT_BUCKET_IPV6(
l347) == L3_DEFIP_MODE_64){l348->v6_64+= PIVOT_BUCKET_COUNT(l347);l348->l318
+= l345.v6_64;}else{l348->v4+= PIVOT_BUCKET_COUNT(l347);l348->l317+= l345.v4;
}l348->l320 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l291 == 0){
LOG_CLI((BSL_META_U(l1,"%9s\n"),"Specific VRF not found"));}}sal_memset(&l346
,0,sizeof(l346));l291 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){LOG_CLI((
BSL_META_U(l1,"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((
BSL_META_U(l1,"-----------------------------------------------\n")));for(l170
= 0;l170<MAX_VRF_ID;l170++){l321*l348;if(l344[l170].l320!= TRUE){continue;}if
(l152!= -1&&l152!= l170){continue;}l291 = 1;l348 = &l344[l170];do{(&l346)->v4
+= (l348)->v4;(&l346)->l317+= (l348)->l317;(&l346)->v6_64+= (l348)->v6_64;(&
l346)->l318+= (l348)->l318;(&l346)->v6_128+= (l348)->v6_128;(&l346)->l319+= (
l348)->l319;}while(0);do{LOG_CLI((BSL_META_U(l1,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l170),(l348
->l317)?(l348->v4)*100/(l348->l317):0,(l348->l317)?(l348->v4)*1000/(l348->
l317)%10:0,(l348->l318)?(l348->v6_64)*100/(l348->l318):0,(l348->l318)?(l348->
v6_64)*1000/(l348->l318)%10:0,(l348->l319)?(l348->v6_128)*100/(l348->l319):0,
(l348->l319)?(l348->v6_128)*1000/(l348->l319)%10:0,((l348->l317+l348->l318+
l348->l319))?((l348->v4+l348->v6_64+l348->v6_128))*100/((l348->l317+l348->
l318+l348->l319)):0,((l348->l317+l348->l318+l348->l319))?((l348->v4+l348->
v6_64+l348->v6_128))*1000/((l348->l317+l348->l318+l348->l319))%10:0,(l170) == 
SOC_VRF_MAX(l1)+1?"GLo":""));}while(0);}if(l291 == 0){LOG_CLI((BSL_META_U(l1,
"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l1,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l1,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l346)->l317)?((&l346)->v4)*100/((&l346)->l317):0,((&l346)->l317)?((&l346)->v4
)*1000/((&l346)->l317)%10:0,((&l346)->l318)?((&l346)->v6_64)*100/((&l346)->
l318):0,((&l346)->l318)?((&l346)->v6_64)*1000/((&l346)->l318)%10:0,((&l346)->
l319)?((&l346)->v6_128)*100/((&l346)->l319):0,((&l346)->l319)?((&l346)->
v6_128)*1000/((&l346)->l319)%10:0,(((&l346)->l317+(&l346)->l318+(&l346)->l319
))?(((&l346)->v4+(&l346)->v6_64+(&l346)->v6_128))*100/(((&l346)->l317+(&l346)
->l318+(&l346)->l319)):0,(((&l346)->l317+(&l346)->l318+(&l346)->l319))?(((&
l346)->v4+(&l346)->v6_64+(&l346)->v6_128))*1000/(((&l346)->l317+(&l346)->l318
+(&l346)->l319))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l332(l1,l152,alpm_vrf_handle[l1],l326);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_CNT
){l332(l1,l152,alpm_vrf_handle[l1],l323);l332(l1,l152,alpm_vrf_handle[l1],
l324);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l332(l1,l152,alpm_vrf_handle
[l1],l325);l332(l1,l152,alpm_vrf_handle[l1],l328);l332(l1,l152,
alpm_vrf_handle[l1],l327);}sal_free(l344);return SOC_E_NONE;}
#endif

#endif /* ALPM_ENABLE */
