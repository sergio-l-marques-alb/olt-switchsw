/*
 * $Id: alpm_128.c,v 1.13 Broadcom SDK $
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
 * File:    alpm_128.c
 * Purpose: Primitives for LPM management in ALPM - Mode for IPv6-128.
 * Requires:
 */

#include <shared/bsl.h>

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
void soc_alpm_128_lpm_state_dump(int l1);static int l2(int l1,void*l3,int*
index,int l4);static int l5(int l1,void*l6);static int l7(int l1,void*l6,int
l8,int l9,int l10,int l11,defip_aux_scratch_entry_t*l12);static int l13(int l1
,void*l6,void*l14,int*l15,int*l16,int*l8);static int l17(int l1,void*l6,void*
l14,int*l15);static int l18(int l19,void*lpm_entry,void*l20,void*l21,
soc_mem_t l22,uint32 l23,uint32*l24);static int l25(int l19,void*l20,
soc_mem_t l22,int l8,int l26,int l27,int index,void*lpm_entry);static int l28
(int l19,uint32*key,int len,int l26,int l8,defip_pair_128_entry_t*lpm_entry,
int l29,int l30);static int l31(int l1,int l26,int l32);typedef struct l33{
int l34;int l35;int l36;int next;int l37;int l38;}l39,*l40;static l40 l41[
SOC_MAX_NUM_DEVICES];typedef struct l42{soc_field_info_t*l43;soc_field_info_t
*l44;soc_field_info_t*l45;soc_field_info_t*l46;soc_field_info_t*l47;
soc_field_info_t*l48;soc_field_info_t*l49;soc_field_info_t*l50;
soc_field_info_t*l51;soc_field_info_t*l52;soc_field_info_t*l53;
soc_field_info_t*l54;soc_field_info_t*l55;soc_field_info_t*l56;
soc_field_info_t*l57;soc_field_info_t*l58;soc_field_info_t*l59;
soc_field_info_t*l60;soc_field_info_t*l61;soc_field_info_t*l62;
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
soc_field_info_t*l96;}l97,*l98;static l98 l99[SOC_MAX_NUM_DEVICES];typedef
struct l100{int l19;int l101;int l102;uint16*l103;uint16*l104;}l105;typedef
uint32 l106[9];typedef int(*l107)(l106 l108,l106 l109);static l105*l110[
SOC_MAX_NUM_DEVICES];static void l111(int l1,void*l14,int index,l106 l112);
static uint16 l113(uint8*l114,int l115);static int l116(int l19,int l101,int
l102,l105**l117);static int l118(l105*l119);static int l120(l105*l121,l107
l122,l106 entry,int l123,uint16*l124);static int l125(l105*l121,l107 l122,
l106 entry,int l123,uint16 l126,uint16 l127);static int l128(l105*l121,l107
l122,l106 entry,int l123,uint16 l129);static int l130(l105*l121,l107 l122,
l106 entry,int l123,uint16 index);static int l131(int l1,const void*entry,int
*l123){int l132,l133;int l134[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};uint32 l135;l135 = soc_mem_field32_get
(l1,L3_DEFIP_PAIR_128m,entry,l134[0]);if((l133 = _ipmask2pfx(l135,l123))<0){
return(l133);}for(l132 = 1;l132<4;l132++){l135 = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,l134[l132]);if(*l123){if(l135!= 0xffffffff){return(
SOC_E_PARAM);}*l123+= 32;}else{if((l133 = _ipmask2pfx(l135,l123))<0){return(
l133);}}}return SOC_E_NONE;}static void l136(uint32*l137,int l138,int l32){
uint32 l139,l140,l34,prefix[5];int l132;sal_memcpy(prefix,l137,sizeof(uint32)
*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(l137,0,sizeof(uint32)*BITS2WORDS(
_MAX_KEY_LEN_144_));l139 = 128-l138;l34 = (l139+31)/32;if((l139%32) == 0){l34
++;}l139 = l139%32;for(l132 = l34;l132<= 4;l132++){prefix[l132]<<= l139;if(
l132<4){l140 = prefix[l132+1]&~(0xffffffff>>l139);l140 = (((32-l139) == 32)?0
:(l140)>>(32-l139));prefix[l132]|= l140;}}for(l132 = l34;l132<= 4;l132++){
l137[3-(l132-l34)] = prefix[l132];}}static void l141(int l19,void*lpm_entry,
int l16){int l132;soc_field_t l142[4] = {IP_ADDR_MASK0_LWRf,
IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};for(l132 = 0;l132<4
;l132++){soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,l142[l132],0);}
for(l132 = 0;l132<4;l132++){if(l16<= 32)break;soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,l142[3-l132],0xffffffff);l16-= 32;}
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,l142[3-l132],~(((l16) == 
32)?0:(0xffffffff)>>(l16)));}static int l143(int l1,void*entry,uint32*l137,
uint32*l16,int*l24){int l132;int l123 = 0,l34;int l133 = SOC_E_NONE;uint32
l139,l140;uint32 prefix[5];sal_memset(l137,0,sizeof(uint32)*BITS2WORDS(
_MAX_KEY_LEN_144_));sal_memset(prefix,0,sizeof(uint32)*BITS2WORDS(
_MAX_KEY_LEN_144_));prefix[0] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR0_LWRf);prefix[1] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR1_LWRf);prefix[2] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR0_UPRf);prefix[3] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR1_UPRf);l133 = l131(l1,entry,&l123);if(SOC_FAILURE(l133)){return
l133;}l139 = 128-l123;l34 = l139/32;l139 = l139%32;for(l132 = l34;l132<4;l132
++){prefix[l132]>>= l139;l140 = prefix[l132+1]&((1<<l139)-1);l140 = (((32-
l139) == 32)?0:(l140)<<(32-l139));prefix[l132]|= l140;}for(l132 = l34;l132<4;
l132++){l137[4-(l132-l34)] = prefix[l132];}*l16 = l123;if(l24!= NULL){*l24 = 
(prefix[0] == 0)&&(prefix[1] == 0)&&(prefix[2] == 0)&&(prefix[3] == 0)&&(l123
== 0);}return SOC_E_NONE;}int l144(int l1,uint32*prefix,uint32 l138,int l8,
int l26,int*l145,int*tcam_index,int*bucket_index){int l133 = SOC_E_NONE;
trie_t*l146;trie_node_t*l147 = NULL;alpm_pivot_t*l148;l146 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l26);l133 = trie_find_lpm(l146,prefix,l138,&l147);
if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l133;}l148 = (alpm_pivot_t*)l147;*l145 = 1;*
tcam_index = PIVOT_TCAM_INDEX(l148);*bucket_index = PIVOT_BUCKET_INDEX(l148);
return SOC_E_NONE;}static int l149(int l1,void*l6,soc_mem_t l22,void*l150,int
*tcam_index,int*bucket_index,int*l15,int l151){uint32 l14[
SOC_MAX_MEM_FIELD_WORDS];int l152,l26,l32;int l124;uint32 l9,l10,l153;int l133
= SOC_E_NONE;int l145 = 0;l32 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l6,&l152,&l26));if(l152 == 0){if(
soc_alpm_mode_get(l1)){return SOC_E_PARAM;}}soc_alpm_db_ent_type_encoding(l1,
l26,&l9,&l10);if(l26 == SOC_VRF_MAX(l1)+1){SOC_ALPM_GET_GLOBAL_BANK_DISABLE(
l1,l153);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l153);}if(l152!= 
SOC_L3_VRF_OVERRIDE){if(l151){uint32 prefix[5],l138;int l24 = 0;l133 = l143(
l1,l6,prefix,&l138,&l24);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: prefix create failed\n")));return l133;}l133
= l144(l1,prefix,l138,l32,l26,&l145,tcam_index,bucket_index);
SOC_IF_ERROR_RETURN(l133);}else{defip_aux_scratch_entry_t l12;sal_memset(&l12
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l7(l1,l6,l32,l9,l10
,0,&l12));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l12,TRUE,&
l145,tcam_index,bucket_index));}if(l145){l18(l1,l6,l14,0,l22,0,0);l133 = 
_soc_alpm_find_in_bkt(l1,l22,*bucket_index,l153,l14,l150,&l124,l32);if(
SOC_SUCCESS(l133)){*l15 = l124;}}else{l133 = SOC_E_NOT_FOUND;}}return l133;}
static int l154(int l1,void*l6,void*l150,void*l155,soc_mem_t l22,int l124){
defip_aux_scratch_entry_t l12;int l152,l32,l26;int bucket_index;uint32 l9,l10
,l153;int l145 = 0,l140 = 0;int tcam_index;l32 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l152,&l26));
soc_alpm_db_ent_type_encoding(l1,l26,&l9,&l10);if(l26 == SOC_VRF_MAX(l1)+1){
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l153);}else{SOC_ALPM_GET_VRF_BANK_DISABLE
(l1,l153);}if(l152!= SOC_L3_VRF_OVERRIDE){sal_memset(&l12,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l7(l1,l6,l32,l9,l10,0,&l12));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l22,MEM_BLOCK_ANY,l124,l150));if(
SOC_URPF_STATUS_GET(l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,l22,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l124),l155));}l140 = soc_mem_field32_get
(l1,L3_DEFIP_AUX_SCRATCHm,&l12,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,REPLACE_LENf,l140);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l145,&tcam_index,&
bucket_index));if(SOC_URPF_STATUS_GET(l1)){if(l140 == 0){soc_mem_field32_set(
l1,L3_DEFIP_AUX_SCRATCHm,&l12,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,RPEf,0);}soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l9+1);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l145,&tcam_index,&
bucket_index));}}return SOC_E_NONE;}static int l156(int l1,int l157,int l158)
{int l133,l140,l159,l160;defip_aux_table_entry_t l161,l162;l159 = 
SOC_ALPM_128_ADDR_LWR(l157);l160 = SOC_ALPM_128_ADDR_UPR(l157);l133 = 
soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l159,&l161);
SOC_IF_ERROR_RETURN(l133);l133 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l160,&l162);SOC_IF_ERROR_RETURN(l133);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l161,BPM_LENGTH0f,l158);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l161,BPM_LENGTH1f,l158);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l162,BPM_LENGTH0f,l158);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l162,BPM_LENGTH1f,l158);l140 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,&l161,DB_TYPE0f);l133 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l159,&l161);SOC_IF_ERROR_RETURN(l133);l133 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l160,&l162);
SOC_IF_ERROR_RETURN(l133);if(SOC_URPF_STATUS_GET(l1)){l140++;
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,BPM_LENGTH0f,l158);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,BPM_LENGTH1f,l158);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l162,BPM_LENGTH0f,l158);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l162,BPM_LENGTH1f,l158);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,DB_TYPE0f,l140);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l161,DB_TYPE1f,l140);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l162,DB_TYPE0f,l140);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l162,DB_TYPE1f,l140);l159+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l160+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(
l1,L3_DEFIPm))/2;l133 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
l159,&l161);SOC_IF_ERROR_RETURN(l133);l133 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l160,&l162);}return l133;}static int l163(
int l1,int l164,void*entry,defip_aux_table_entry_t*l165,int l166){uint32 l140
,l9,l10,l167 = 0;soc_mem_t l22 = L3_DEFIP_PAIR_128m;soc_mem_t l168 = 
L3_DEFIP_AUX_TABLEm;int l133 = SOC_E_NONE,l123,l26;void*l169,*l170;l169 = (
void*)l165;l170 = (void*)(l165+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l168,
MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l164),l165));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l168,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l164),l165+1));l140 = 
soc_mem_field32_get(l1,l22,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l168,
l169,VRF0f,l140);l140 = soc_mem_field32_get(l1,l22,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l168,l169,VRF1f,l140);l140 = soc_mem_field32_get(l1,
l22,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l168,l170,VRF0f,l140);l140 = 
soc_mem_field32_get(l1,l22,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l168,
l170,VRF1f,l140);l140 = soc_mem_field32_get(l1,l22,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l168,l169,MODE0f,l140);l140 = soc_mem_field32_get(l1,
l22,entry,MODE1_LWRf);soc_mem_field32_set(l1,l168,l169,MODE1f,l140);l140 = 
soc_mem_field32_get(l1,l22,entry,MODE0_UPRf);soc_mem_field32_set(l1,l168,l170
,MODE0f,l140);l140 = soc_mem_field32_get(l1,l22,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l168,l170,MODE1f,l140);l140 = soc_mem_field32_get(l1,
l22,entry,VALID0_LWRf);soc_mem_field32_set(l1,l168,l169,VALID0f,l140);l140 = 
soc_mem_field32_get(l1,l22,entry,VALID1_LWRf);soc_mem_field32_set(l1,l168,
l169,VALID1f,l140);l140 = soc_mem_field32_get(l1,l22,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l168,l170,VALID0f,l140);l140 = soc_mem_field32_get(l1,
l22,entry,VALID1_UPRf);soc_mem_field32_set(l1,l168,l170,VALID1f,l140);l133 = 
l131(l1,entry,&l123);SOC_IF_ERROR_RETURN(l133);soc_mem_field32_set(l1,l168,
l169,IP_LENGTH0f,l123);soc_mem_field32_set(l1,l168,l169,IP_LENGTH1f,l123);
soc_mem_field32_set(l1,l168,l170,IP_LENGTH0f,l123);soc_mem_field32_set(l1,
l168,l170,IP_LENGTH1f,l123);l140 = soc_mem_field32_get(l1,l22,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l168,l169,IP_ADDR0f,l140);l140 = 
soc_mem_field32_get(l1,l22,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l168,
l169,IP_ADDR1f,l140);l140 = soc_mem_field32_get(l1,l22,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l168,l170,IP_ADDR0f,l140);l140 = soc_mem_field32_get(
l1,l22,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l168,l170,IP_ADDR1f,l140);
l133 = soc_alpm_128_lpm_vrf_get(l1,entry,&l26,&l123);SOC_IF_ERROR_RETURN(l133
);if(SOC_URPF_STATUS_GET(l1)){if(l166>= (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1)){l167 = 1;}}soc_alpm_db_ent_type_encoding(l1,l123,&l9
,&l10);if(l26 == SOC_L3_VRF_OVERRIDE||(SOC_MEM_FIELD_VALID(l1,l22,
MULTICAST_ROUTEf)&&soc_mem_field32_get(l1,l22,entry,MULTICAST_ROUTEf))){
soc_mem_field32_set(l1,l168,l169,VALID0f,0);soc_mem_field32_set(l1,l168,l169,
VALID1f,0);soc_mem_field32_set(l1,l168,l170,VALID0f,0);soc_mem_field32_set(l1
,l168,l170,VALID1f,0);l9 = 0;}else{if(l167){l9++;}}soc_mem_field32_set(l1,
l168,l169,DB_TYPE0f,l9);soc_mem_field32_set(l1,l168,l169,DB_TYPE1f,l9);
soc_mem_field32_set(l1,l168,l170,DB_TYPE0f,l9);soc_mem_field32_set(l1,l168,
l170,DB_TYPE1f,l9);l140 = soc_mem_field32_get(l1,l22,entry,ENTRY_TYPE0_LWRf);
soc_mem_field32_set(l1,l168,l169,ENTRY_TYPE0f,l140|l10);l140 = 
soc_mem_field32_get(l1,l22,entry,ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,
l168,l169,ENTRY_TYPE1f,l140|l10);l140 = soc_mem_field32_get(l1,l22,entry,
ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,l168,l170,ENTRY_TYPE0f,l140|l10);
l140 = soc_mem_field32_get(l1,l22,entry,ENTRY_TYPE1_UPRf);soc_mem_field32_set
(l1,l168,l170,ENTRY_TYPE1f,l140|l10);if(l167){l140 = soc_mem_field32_get(l1,
l22,entry,ALG_BKT_PTRf);if(l140){l140+= SOC_ALPM_BUCKET_COUNT(l1);
soc_mem_field32_set(l1,l22,entry,ALG_BKT_PTRf,l140);}}return SOC_E_NONE;}
static int l171(int l1,int l172,int index,int l173,void*entry){
defip_aux_table_entry_t l165[2];l173 = soc_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l173,1);SOC_IF_ERROR_RETURN(l163(l1,l173,entry,&l165[0],
index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,
entry));index = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(index),l165));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(index),l165+
1));return SOC_E_NONE;}static int l174(int l1,void*l6,soc_mem_t l22,void*l150
,void*l155,int*l15,int bucket_index,int tcam_index){alpm_pivot_t*l148,*l175,*
l176;defip_aux_scratch_entry_t l12;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l177,l138;uint32 l178[5];int l32,l26,l152;int l124;int l133 = 
SOC_E_NONE,l179;uint32 l9,l10,l153,l158 = 0;int l145 =0;int l157;int l180 = 0
;trie_t*trie,*l181;trie_node_t*l182,*l183 = NULL,*l147 = NULL;payload_t*l184,
*l185,*l186;defip_pair_128_entry_t lpm_entry;alpm_bucket_handle_t*l187;int
l132,l188 = -1,l24 = 0,l189 = 0;alpm_mem_prefix_array_t l190;
defip_alpm_ipv6_128_entry_t l191,l192;void*l193,*l194;int*l127 = NULL;int l195
= 0;trie_t*l146 = NULL;int l196;int l197;void*l198 = NULL;void*l199 = NULL;
void*l200;void*l201;void*l202;int l203 = 0;int l204 = 0;l32 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l152,&
l26));soc_alpm_db_ent_type_encoding(l1,l26,&l9,&l10);if(l26 == SOC_VRF_MAX(l1
)+1){SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l153);}else{
SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l153);}l22 = L3_DEFIP_ALPM_IPV6_128m;l193 = 
((uint32*)&(l191));l194 = ((uint32*)&(l192));l133 = l143(l1,l6,prefix,&l138,&
l24);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: prefix create failed\n")));return l133;}sal_memset(&
l12,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l7(l1,l6,l32,l9,
l10,0,&l12));if(bucket_index == 0){l133 = l144(l1,prefix,l138,l32,l26,&l145,&
tcam_index,&bucket_index);SOC_IF_ERROR_RETURN(l133);if(l145 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: "
" Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l133 = 
_soc_alpm_insert_in_bkt(l1,l22,bucket_index,l153,l150,l14,&l124,l32);if(l133
== SOC_E_NONE){*l15 = l124;if(SOC_URPF_STATUS_GET(l1)){l179 = soc_mem_write(
l1,l22,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l124),l155);if(SOC_FAILURE(l179))
{return l179;}}}if(l133 == SOC_E_FULL){l180 = 1;}l148 = ALPM_TCAM_PIVOT(l1,
tcam_index);if(l148 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"ins128: pivot index %d bkt %d is not valid \n"),tcam_index,bucket_index));
return SOC_E_PARAM;}trie = PIVOT_BUCKET_TRIE(l148);l176 = l148;l184 = 
sal_alloc(sizeof(payload_t),"Payload for 128b Key");if(l184 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Unable to allocate memory for ""trie node \n")));
return SOC_E_MEMORY;}l185 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l185 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l184);return SOC_E_MEMORY;}sal_memset(l184,0,
sizeof(*l184));sal_memset(l185,0,sizeof(*l185));l184->key[0] = prefix[0];l184
->key[1] = prefix[1];l184->key[2] = prefix[2];l184->key[3] = prefix[3];l184->
key[4] = prefix[4];l184->len = l138;l184->index = l124;sal_memcpy(l185,l184,
sizeof(*l184));l185->bkt_ptr = l184;l133 = trie_insert(trie,prefix,NULL,l138,
(trie_node_t*)l184);if(SOC_FAILURE(l133)){goto l205;}l181 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l26);if(!l24){l133 = trie_insert(l181,prefix,NULL
,l138,(trie_node_t*)l185);}else{l147 = NULL;l133 = trie_find_lpm(l181,0,0,&
l147);l186 = (payload_t*)l147;if(SOC_SUCCESS(l133)){l186->bkt_ptr = l184;}}
l177 = l138;if(SOC_FAILURE(l133)){goto l206;}if(l180){l133 = 
alpm_bucket_assign(l1,&bucket_index,l32);if(SOC_FAILURE(l133)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate"
"new bucket for split\n")));bucket_index = -1;goto l207;}l133 = trie_split(
trie,_MAX_KEY_LEN_144_,FALSE,l178,&l138,&l182,NULL,FALSE,1024);if(SOC_FAILURE
(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Could not split bucket"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l207;}l147 = NULL;l133 = trie_find_lpm(l181,l178,l138,&l147)
;l186 = (payload_t*)l147;if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l178[0],l178[1],l178[
2],l178[3],l178[4],l138));goto l207;}if(l186->bkt_ptr){if(l186->bkt_ptr == 
l184){sal_memcpy(l193,l150,sizeof(defip_alpm_ipv6_128_entry_t));}else{l133 = 
soc_mem_read(l1,l22,MEM_BLOCK_ANY,((payload_t*)l186->bkt_ptr)->index,l193);if
(SOC_FAILURE(l133)){goto l207;}if(SOC_URPF_STATUS_GET(l1)){l133 = 
soc_mem_read(l1,l22,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,((payload_t*)l186->
bkt_ptr)->index),l194);l189 = soc_mem_field32_get(l1,l22,l194,DEFAULTROUTEf);
}}if(SOC_FAILURE(l133)){goto l207;}l133 = l25(l1,l193,l22,l32,l152,
bucket_index,0,&lpm_entry);if(SOC_FAILURE(l133)){goto l207;}l158 = ((
payload_t*)(l186->bkt_ptr))->len;}else{l133 = soc_mem_read(l1,
L3_DEFIP_PAIR_128m,MEM_BLOCK_ANY,soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,
SOC_ALPM_128_DEFIP_TO_PAIR(tcam_index>>1),1),&lpm_entry);}l187 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM 128 Bucket Handle");if(l187 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Unable to allocate "" memory for PIVOT trie node \n"))
);l133 = SOC_E_MEMORY;goto l207;}sal_memset(l187,0,sizeof(*l187));l148 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new 128b Pivot");if(l148 == NULL)
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Unable to allocate ""memory for PIVOT trie node \n")))
;l133 = SOC_E_MEMORY;goto l207;}sal_memset(l148,0,sizeof(*l148));
PIVOT_BUCKET_HANDLE(l148) = l187;l133 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l148));PIVOT_BUCKET_TRIE(l148)->trie = l182;
PIVOT_BUCKET_INDEX(l148) = bucket_index;PIVOT_BUCKET_VRF(l148) = l26;
PIVOT_BUCKET_IPV6(l148) = l32;PIVOT_BUCKET_DEF(l148) = FALSE;l148->key[0] = 
l178[0];l148->key[1] = l178[1];l148->key[2] = l178[2];l148->key[3] = l178[3];
l148->key[4] = l178[4];l148->len = l138;l146 = VRF_PIVOT_TRIE_IPV6_128(l1,l26
);l136((l178),(l138),(l32));(void)l28(l1,l178,l138,l26,l32,&lpm_entry,0,0);
soc_L3_DEFIP_PAIR_128m_field32_set(l1,&lpm_entry,ALG_BKT_PTRf,bucket_index);
sal_memset(&l190,0,sizeof(l190));l133 = trie_traverse(PIVOT_BUCKET_TRIE(l148)
,alpm_mem_prefix_array_cb,&l190,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l133))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_128_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l207;}l127 = sal_alloc(sizeof(*l127)*l190.count,
"Temp storage for location of prefixes in new 128b bucket");if(l127 == NULL){
l133 = SOC_E_MEMORY;goto l207;}sal_memset(l127,-1,sizeof(*l127)*l190.count);
l196 = sizeof(defip_alpm_raw_entry_t);l197 = l196*ALPM_RAW_BKT_COUNT_DW;l199 = 
sal_alloc(4*l197,"Raw memory buffer");if(l199 == NULL){l133 = SOC_E_MEMORY;
goto l207;}sal_memset(l199,0,4*l197);l200 = (uint8*)l199+l197;l201 = (uint8*)
l199+l197*2;l202 = (uint8*)l199+l197*3;l133 = _soc_alpm_raw_bucket_read(l1,
l22,PIVOT_BUCKET_INDEX(l176),l199,l200);if(SOC_FAILURE(l133)){goto l207;}for(
l132 = 0;l132<l190.count;l132++){payload_t*l123 = l190.prefix[l132];if(l123->
index>0){SOC_ALPM_RAW_INDEX_DECODE(l1,l32,l123->index,l203,l204);l198 = (
uint8*)l199+l203*l196;_soc_alpm_raw_mem_read(l1,l22,l198,l204,l193);
_soc_alpm_raw_mem_write(l1,l22,l198,l204,soc_mem_entry_null(l1,l22));if(
SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE(l1,l32,_soc_alpm_rpf_entry
(l1,l123->index),l203,l204);l198 = (uint8*)l200+l203*l196;
_soc_alpm_raw_mem_read(l1,l22,l198,l204,l194);_soc_alpm_raw_mem_write(l1,l22,
l198,l204,soc_mem_entry_null(l1,l22));}l133 = _soc_alpm_mem_index(l1,l22,
bucket_index,l132,l153,&l124);if(SOC_SUCCESS(l133)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l32,l124,l203,l204);l198 = (uint8*)l201+l203*l196;_soc_alpm_raw_mem_write
(l1,l22,l198,l204,l193);if(SOC_URPF_STATUS_GET(l1)){SOC_ALPM_RAW_INDEX_DECODE
(l1,l32,_soc_alpm_rpf_entry(l1,l124),l203,l204);l198 = (uint8*)l202+l203*l196
;_soc_alpm_raw_mem_write(l1,l22,l198,l204,l194);}}}else{l133 = 
_soc_alpm_mem_index(l1,l22,bucket_index,l132,l153,&l124);if(SOC_SUCCESS(l133)
){l188 = l132;*l15 = l124;_soc_alpm_raw_parity_set(l1,l22,l150);
SOC_ALPM_RAW_INDEX_DECODE(l1,l32,l124,l203,l204);l198 = (uint8*)l201+l203*
l196;_soc_alpm_raw_mem_write(l1,l22,l198,l204,l150);if(SOC_URPF_STATUS_GET(l1
)){_soc_alpm_raw_parity_set(l1,l22,l155);SOC_ALPM_RAW_INDEX_DECODE(l1,l32,
_soc_alpm_rpf_entry(l1,l124),l203,l204);l198 = (uint8*)l202+l203*l196;
_soc_alpm_raw_mem_write(l1,l22,l198,l204,l155);}}}l127[l132] = l124;}l133 = 
_soc_alpm_raw_bucket_write(l1,l22,bucket_index,l153,(void*)l201,(void*)l202,
l190.count);if(SOC_FAILURE(l133)){goto l208;}l133 = l2(l1,&lpm_entry,&l157,
l189);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Unable to add new ""pivot to tcam\n")));if(l133 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l26,l32);}goto l208;}l157 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l157,l32);l133 = l156(l1,l157,
l158);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),l157));goto l209
;}l133 = trie_insert(l146,l148->key,NULL,l148->len,(trie_node_t*)l148);if(
SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));goto l209;}l195 = 1;ALPM_TCAM_PIVOT(
l1,SOC_ALPM_128_ADDR_LWR(l157)<<1) = l148;PIVOT_TCAM_INDEX(l148) = 
SOC_ALPM_128_ADDR_LWR(l157)<<1;VRF_PIVOT_REF_INC(l1,l26,l32);for(l132 = 0;
l132<l190.count;l132++){l190.prefix[l132]->index = l127[l132];}sal_free(l127)
;l127 = NULL;l133 = _soc_alpm_raw_bucket_write(l1,l22,PIVOT_BUCKET_INDEX(l176
),l153,(void*)l199,(void*)l200,-1);if(SOC_FAILURE(l133)){goto l209;}sal_free(
l199);if(l188 == -1){l133 = _soc_alpm_insert_in_bkt(l1,l22,
PIVOT_BUCKET_HANDLE(l176)->bucket_index,l153,l150,l14,&l124,l32);if(
SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: Could not insert new ""prefix into trie after split\n"
)));goto l207;}if(SOC_URPF_STATUS_GET(l1)){l133 = soc_mem_write(l1,l22,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l124),l155);}*l15 = l124;l184->index = 
l124;}PIVOT_BUCKET_ENT_CNT_UPDATE(l148);VRF_BUCKET_SPLIT_INC(l1,l26,l32);}
VRF_TRIE_ROUTES_INC(l1,l26,l32);if(l24){sal_free(l185);}if(SOC_IS_TRIDENT2(l1
)){SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l145,&
tcam_index,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,
INSERT_PROPAGATE,&l12,FALSE,&l145,&tcam_index,&bucket_index));}else{
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE,&l12,TRUE,&l145,&
tcam_index,&bucket_index));}if(SOC_URPF_STATUS_GET(l1)){l138 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf);l138+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l138);if(l24){
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,RPEf,1);}else{
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,RPEf,0);}if(SOC_IS_TRIDENT2
(l1)){SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&
l145,&tcam_index,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,
INSERT_PROPAGATE,&l12,FALSE,&l145,&tcam_index,&bucket_index));}else{
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE,&l12,TRUE,&l145,&
tcam_index,&bucket_index));}}PIVOT_BUCKET_ENT_CNT_UPDATE(l176);return l133;
l209:l179 = l5(l1,&lpm_entry);if(SOC_FAILURE(l179)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: Failure to free new prefix""at %d\n"),
soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,l157,l32)));}if(l195){l179 = 
trie_delete(l146,l148->key,l148->len,&l183);if(SOC_FAILURE(l179)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: trie delete failure"
"in bkt move rollback\n")));}}VRF_PIVOT_REF_DEC(l1,l26,l32);l208:l175 = l176;
for(l132 = 0;l132<l190.count;l132++){payload_t*prefix = l190.prefix[l132];if(
l127[l132]!= -1){sal_memset(l193,0,sizeof(defip_alpm_ipv6_128_entry_t));l179 = 
soc_mem_write(l1,l22,MEM_BLOCK_ANY,l127[l132],l193);
_soc_trident2_alpm_bkt_view_set(l1,l127[l132],INVALIDm);if(SOC_FAILURE(l179))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l1)){l179 = soc_mem_write(l1,l22,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l127[l132]),l193);_soc_trident2_alpm_bkt_view_set(l1,
_soc_alpm_rpf_entry(l1,l127[l132]),INVALIDm);if(SOC_FAILURE(l179)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l183 = NULL;l179 = trie_delete(
PIVOT_BUCKET_TRIE(l148),prefix->key,prefix->len,&l183);l184 = (payload_t*)
l183;if(SOC_FAILURE(l179)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: trie delete failure""in bkt move rollback\n")));}if(
prefix->index>0){l179 = trie_insert(PIVOT_BUCKET_TRIE(l175),prefix->key,NULL,
prefix->len,(trie_node_t*)l184);if(SOC_FAILURE(l179)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l184!= NULL){sal_free(l184);}}}if(l188
== -1){l183 = NULL;l179 = trie_delete(PIVOT_BUCKET_TRIE(l175),prefix,l177,&
l183);l184 = (payload_t*)l183;if(SOC_FAILURE(l179)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l184!= 
NULL){sal_free(l184);}}l179 = alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l148)
,l32);if(SOC_FAILURE(l179)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: new bucket release ""failure: %d\n"),
PIVOT_BUCKET_INDEX(l148)));}trie_destroy(PIVOT_BUCKET_TRIE(l148));sal_free(
l187);sal_free(l148);if(l127!= NULL){sal_free(l127);}sal_free(l199);l183 = 
NULL;l179 = trie_delete(l181,prefix,l177,&l183);l185 = (payload_t*)l183;if(
SOC_FAILURE(l179)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l185){sal_free(
l185);}return l133;l207:if(l127!= NULL){sal_free(l127);}if(l199!= NULL){
sal_free(l199);}l183 = NULL;(void)trie_delete(l181,prefix,l177,&l183);l185 = 
(payload_t*)l183;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l32);}l206:l183 = NULL;(void)trie_delete(trie,prefix,l177,&l183)
;l184 = (payload_t*)l183;l205:if(l184!= NULL){sal_free(l184);}if(l185!= NULL)
{sal_free(l185);}return l133;}static int l28(int l19,uint32*key,int len,int
l26,int l8,defip_pair_128_entry_t*lpm_entry,int l29,int l30){uint32 l140;if(
l30){sal_memset(lpm_entry,0,sizeof(defip_pair_128_entry_t));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l81),(l26&SOC_VRF_MAX(l19)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l82),(l26&SOC_VRF_MAX(l19)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l79),(l26&SOC_VRF_MAX(l19)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l80),(l26&SOC_VRF_MAX(l19)));if(l26 == (SOC_VRF_MAX(
l19)+1)){l140 = 0;}else{l140 = SOC_VRF_MAX(l19);}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l85),(l140));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l86),(l140));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l83),(l140));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l84),(l140));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l58),(key[0]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l59),(key[1]))
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l56),(key[2]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l57),(key[3]))
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l66),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l67),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l64),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l65),(3));l141
(l19,(void*)lpm_entry,len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l77),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l78),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l75),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l76),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l70),((1<<
soc_mem_field_length(l19,L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l71),((1<<soc_mem_field_length(l19,L3_DEFIP_PAIR_128m
,MODE_MASK1_LWRf))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l68),((1<<soc_mem_field_length(
l19,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l69),((1<<soc_mem_field_length(l19,L3_DEFIP_PAIR_128m
,MODE_MASK1_UPRf))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l93),((1<<soc_mem_field_length(
l19,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l94),((1<<soc_mem_field_length(l19,L3_DEFIP_PAIR_128m
,ENTRY_TYPE_MASK1_LWRf))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO
(l19,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l19)]->l95),((1<<
soc_mem_field_length(l19,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l96),((1<<soc_mem_field_length(l19,L3_DEFIP_PAIR_128m
,ENTRY_TYPE_MASK1_UPRf))-1));return(SOC_E_NONE);}static int l210(int l1,void*
l6,int bucket_index,int tcam_index,int l124,int l211){alpm_pivot_t*l148;
defip_alpm_ipv6_128_entry_t l191,l212,l192;defip_aux_scratch_entry_t l12;
uint32 l14[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l22;void*l193,*l213,*l194 = 
NULL;int l152;int l8;int l133 = SOC_E_NONE,l179 = SOC_E_NONE;uint32 l214[5],
prefix[5];int l32,l26;uint32 l138;int l215;uint32 l9,l10,l153;int l145,l24 = 
0;trie_t*trie,*l181;uint32 l216;defip_pair_128_entry_t lpm_entry,*l217;
payload_t*l184 = NULL,*l218 = NULL,*l186 = NULL;trie_node_t*l183 = NULL,*l147
= NULL;uint32 l219 = 0,l220 = 0;trie_t*l146 = NULL;l8 = l32 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l152,&
l26));soc_alpm_db_ent_type_encoding(l1,l26,&l9,&l10);if(l152!= 
SOC_L3_VRF_OVERRIDE){if(l26 == SOC_VRF_MAX(l1)+1){
SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l153);}else{SOC_ALPM_GET_VRF_BANK_DISABLE
(l1,l153);}l133 = l143(l1,l6,prefix,&l138,&l24);if(SOC_FAILURE(l133)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: prefix create failed\n")));return l133;}if(!
soc_alpm_mode_get(l1)){if(l152!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1
,l26,l32)>1){if(l24){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot v6-128 delete ""default route if other routes are present "
"in this mode"),l26));return SOC_E_PARAM;}}}}l22 = L3_DEFIP_ALPM_IPV6_128m;
l193 = ((uint32*)&(l191));SOC_ALPM_LPM_LOCK(l1);if(bucket_index == 0){l133 = 
l149(l1,l6,l22,l193,&tcam_index,&bucket_index,&l124,TRUE);}else{l133 = l18(l1
,l6,l193,0,l22,0,0);}sal_memcpy(&l212,l193,sizeof(l212));l213 = &l212;if(
SOC_FAILURE(l133)){SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_delete: Unable to find ""prefix for delete\n")))
;return l133;}l215 = bucket_index;l148 = ALPM_TCAM_PIVOT(l1,tcam_index);if(
l148 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"del128: pivot index %d bkt %d is not valid \n"),tcam_index,bucket_index));
SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_PARAM;}trie = PIVOT_BUCKET_TRIE(l148);
l133 = trie_delete(trie,prefix,l138,&l183);l184 = (payload_t*)l183;if(l133!= 
SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l133;}l181 = VRF_PREFIX_TRIE_IPV6_128(l1,l26);
l146 = VRF_PIVOT_TRIE_IPV6_128(l1,l26);if(!l24){l133 = trie_delete(l181,
prefix,l138,&l183);l218 = (payload_t*)l183;if(SOC_FAILURE(l133)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l221;}if(l211){l147 = NULL;l133 = trie_find_lpm(l181,prefix,
l138,&l147);l186 = (payload_t*)l147;if(SOC_SUCCESS(l133)){payload_t*l222 = (
payload_t*)(l186->bkt_ptr);if(l222!= NULL){l216 = l222->len;}else{l216 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l223;}sal_memcpy(l214,prefix,sizeof(
prefix));l136((l214),(l138),(l32));(void)l28(l1,l214,l216,l26,l8,&lpm_entry,0
,1);l133 = l149(l1,&lpm_entry,l22,l193,&tcam_index,&bucket_index,&l124,TRUE);
if(SOC_SUCCESS(l133)){(void)l25(l1,l193,l22,l8,l152,bucket_index,0,&lpm_entry
);(void)l28(l1,l214,l138,l26,l8,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){
l194 = ((uint32*)&(l192));l133 = soc_mem_read(l1,l22,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l124),l194);l219 = soc_mem_field32_get(l1,l22,l194,
SRC_DISCARDf);l220 = soc_mem_field32_get(l1,l22,l194,DEFAULTROUTEf);}}else if
(l216 == 0){l217 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l26);sal_memcpy(&
lpm_entry,l217,sizeof(lpm_entry));(void)l28(l1,l214,l138,l26,l8,&lpm_entry,0,
0);if(SOC_URPF_STATUS_GET(l1)){l219 = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,&lpm_entry,SRC_DISCARDf);l220 = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,&lpm_entry,RPEf);}l133 = SOC_E_NONE;}if(SOC_FAILURE(l133))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Could not find ""replacement prefix for "
"prefix: 0x%08x 0x%08x 0x%08x 0x%08x "
"prop_pfx: 0x%08x 0x%08x 0x%08x 0x%08x ""bmp_len %d\n"),prefix[1],prefix[2],
prefix[3],prefix[4],l214[0],l214[1],l214[2],l214[3],l216));goto l223;}l217 = 
&lpm_entry;}}else{l147 = NULL;l133 = trie_find_lpm(l181,prefix,l138,&l147);
l186 = (payload_t*)l147;if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),l26));goto l221;}l186->bkt_ptr = 0;
l216 = 0;l217 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l26);}if(l211){l133 = l7(
l1,l217,l32,l9,l10,l216,&l12);if(SOC_FAILURE(l133)){goto l223;}l133 = 
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l145,&tcam_index,&
bucket_index);if(SOC_FAILURE(l133)){goto l223;}if(SOC_URPF_STATUS_GET(l1)){
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DB_TYPEf,l9+1);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,SRC_DISCARDf,l219);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l12,DEFAULTROUTEf,l220);l133 = 
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l12,TRUE,&l145,&tcam_index,&
bucket_index);if(SOC_FAILURE(l133)){goto l223;}}}sal_free(l184);if(!l24){
sal_free(l218);}PIVOT_BUCKET_ENT_CNT_UPDATE(l148);if((l148->len!= 0)&&(trie->
trie == NULL)){uint32 l224[5];sal_memcpy(l224,l148->key,sizeof(l224));l136((
l224),(l148->len),(l8));(void)l28(l1,l224,l148->len,l26,l8,&lpm_entry,0,1);
l133 = l5(l1,&lpm_entry);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),l148->key[1],l148->key[2],l148
->key[3],l148->key[4]));}}l133 = _soc_alpm_delete_in_bkt(l1,l22,l215,l153,
l213,l14,&l124,l32);if(!SOC_SUCCESS(l133)){SOC_ALPM_LPM_UNLOCK(l1);l133 = 
SOC_E_FAIL;return l133;}if(SOC_URPF_STATUS_GET(l1)){l133 = 
soc_mem_alpm_delete(l1,l22,SOC_ALPM_RPF_BKT_IDX(l1,l215),MEM_BLOCK_ALL,l153,
l213,l14,&l145);if(!SOC_SUCCESS(l133)){SOC_ALPM_LPM_UNLOCK(l1);l133 = 
SOC_E_FAIL;return l133;}}if((l148->len!= 0)&&(trie->trie == NULL)){l133 = 
alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l148),l32);if(SOC_FAILURE(l133)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(l148)));}l133 = trie_delete(l146,l148->key,l148->len,&l183
);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not delete pivot from pivot trie\n")));}trie_destroy(PIVOT_BUCKET_TRIE
(l148));sal_free(PIVOT_BUCKET_HANDLE(l148));sal_free(l148);
_soc_trident2_alpm_bkt_view_set(l1,l215<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l32)){_soc_trident2_alpm_bkt_view_set(l1,(l215+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l26,l32);if(VRF_TRIE_ROUTES_CNT(l1,
l26,l32) == 0){l133 = l31(l1,l26,l32);}SOC_ALPM_LPM_UNLOCK(l1);return l133;
l223:l179 = trie_insert(l181,prefix,NULL,l138,(trie_node_t*)l218);if(
SOC_FAILURE(l179)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l221:l179 = trie_insert(trie,prefix,NULL,l138,(
trie_node_t*)l184);if(SOC_FAILURE(l179)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}SOC_ALPM_LPM_UNLOCK(l1);return l133;}int
soc_alpm_128_init(int l1){int l133 = SOC_E_NONE;l133 = soc_alpm_128_lpm_init(
l1);SOC_IF_ERROR_RETURN(l133);return l133;}int soc_alpm_128_state_clear(int l1
){int l132,l133;for(l132 = 0;l132<= SOC_VRF_MAX(l1)+1;l132++){l133 = 
trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,l132),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l133)){trie_destroy(
VRF_PREFIX_TRIE_IPV6_128(l1,l132));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n"),l1,
l132));return l133;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l132)!= NULL){
sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l132));}}return SOC_E_NONE;}int
soc_alpm_128_deinit(int l1){soc_alpm_128_lpm_deinit(l1);SOC_IF_ERROR_RETURN(
soc_alpm_128_state_clear(l1));return SOC_E_NONE;}static int l225(int l1,int
l26,int l32){defip_pair_128_entry_t*lpm_entry = NULL,l226;int l227;int index;
int l133 = SOC_E_NONE;uint32 key[5] = {0,0,0,0,0};uint32 l138;
alpm_bucket_handle_t*l187 = NULL;alpm_pivot_t*l148 = NULL;payload_t*l218 = 
NULL;trie_t*l228 = NULL,*l229 = NULL;int l230 = 0;trie_t*l231 = NULL;int l232
= 0;l133 = alpm_bucket_assign(l1,&l227,l32);if(SOC_FAILURE(l133)){return l133
;}l187 = sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l187
== NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: Unable to allocate memory ""for bucket handle \n")));
l133 = SOC_E_MEMORY;goto l233;}sal_memset(l187,0,sizeof(*l187));l148 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l148 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: Unable to allocate memory ""for PIVOT trie node \n")))
;l133 = SOC_E_MEMORY;goto l233;}sal_memset(l148,0,sizeof(*l148));
PIVOT_BUCKET_HANDLE(l148) = l187;l218 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l218 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_alpm_128_vrf_add: Unable to allocate memory "
"for pfx trie node \n")));l133 = SOC_E_MEMORY;goto l233;}sal_memset(l218,0,
sizeof(*l218));l133 = trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(l1
,l26));l231 = VRF_PIVOT_TRIE_IPV6_128(l1,l26);if(SOC_FAILURE(l133)){goto l233
;}l133 = trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(l1,l26));l228 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l26);if(SOC_FAILURE(l133)){goto l233;}l133 = 
trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l148));l229 = 
PIVOT_BUCKET_TRIE(l148);if(SOC_FAILURE(l133)){goto l233;}PIVOT_BUCKET_INDEX(
l148) = l227;PIVOT_BUCKET_VRF(l148) = l26;PIVOT_BUCKET_IPV6(l148) = l32;
PIVOT_BUCKET_DEF(l148) = TRUE;l138 = 0;l148->key[0] = l218->key[0] = key[0];
l148->key[1] = l218->key[1] = key[1];l148->key[2] = l218->key[2] = key[2];
l148->key[3] = l218->key[3] = key[3];l148->key[4] = l218->key[4] = key[4];
l148->len = l218->len = l138;l133 = trie_insert(l228,key,NULL,l138,&(l218->
node));if(SOC_FAILURE(l133)){goto l233;}l230 = 1;l133 = trie_insert(l231,key,
NULL,l138,(trie_node_t*)l148);if(SOC_FAILURE(l133)){goto l233;}l232 = 1;(void
)l28(l1,key,0,l26,l32,&l226,0,1);if(l26 == SOC_VRF_MAX(l1)+1){
soc_L3_DEFIP_PAIR_128m_field32_set(l1,&l226,GLOBAL_ROUTEf,1);}else{
soc_L3_DEFIP_PAIR_128m_field32_set(l1,&l226,DEFAULT_MISSf,1);}
soc_L3_DEFIP_PAIR_128m_field32_set(l1,&l226,ALG_BKT_PTRf,l227);lpm_entry = 
sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(lpm_entry == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_add: unable to allocate memory ""for IPv6-128 LPM entry\n")
));l133 = SOC_E_MEMORY;goto l233;}sal_memcpy(lpm_entry,&l226,sizeof(l226));
l133 = l2(l1,&l226,&index,0);if(SOC_FAILURE(l133)){goto l233;}
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l26) = lpm_entry;index = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,l32);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(index)<<1) = l148;PIVOT_TCAM_INDEX(l148) = 
SOC_ALPM_128_ADDR_LWR(index)<<1;VRF_PIVOT_REF_INC(l1,l26,l32);
VRF_TRIE_INIT_DONE(l1,l26,l32,1);return SOC_E_NONE;l233:if(lpm_entry){
sal_free(lpm_entry);}if(l232){trie_node_t*l183 = NULL;(void)trie_delete(l231,
key,l138,&l183);}if(l230){trie_node_t*l183 = NULL;(void)trie_delete(l228,key,
l138,&l183);}if(l229){(void)trie_destroy(l229);PIVOT_BUCKET_TRIE(l148) = NULL
;}if(l228){(void)trie_destroy(l228);VRF_PREFIX_TRIE_IPV6_128(l1,l26) = NULL;}
if(l231){(void)trie_destroy(l231);VRF_PIVOT_TRIE_IPV6_128(l1,l26) = NULL;}if(
l218){sal_free(l218);}if(l148){sal_free(l148);}if(l187){sal_free(l187);}(void
)alpm_bucket_release(l1,l227,l32);return l133;}static int l31(int l1,int l26,
int l32){defip_pair_128_entry_t*lpm_entry;int l227;int l234;int l133 = 
SOC_E_NONE;uint32 key[2] = {0,0},l137[SOC_MAX_MEM_FIELD_WORDS];payload_t*l184
;alpm_pivot_t*l235;trie_node_t*l183;trie_t*l236;trie_t*l231 = NULL;lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l26);l227 = 
soc_L3_DEFIP_PAIR_128m_field32_get(l1,lpm_entry,ALG_BKT_PTRf);l133 = 
alpm_bucket_release(l1,l227,l32);_soc_trident2_alpm_bkt_view_set(l1,l227<<2,
INVALIDm);if(SOC_ALPM_V6_SCALE_CHECK(l1,l32)){_soc_trident2_alpm_bkt_view_set
(l1,(l227+1)<<2,INVALIDm);}l133 = l17(l1,lpm_entry,(void*)l137,&l234);if(
SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n"),
l26,l32));l234 = -1;}l234 = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l234,
l32);l235 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l234)<<1);l133 = l5(l1,
lpm_entry);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),l26,l32));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l26) = NULL;l236 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l26);VRF_PREFIX_TRIE_IPV6_128(l1,l26) = NULL;
VRF_TRIE_INIT_DONE(l1,l26,l32,0);l133 = trie_delete(l236,key,0,&l183);l184 = 
(payload_t*)l183;if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),l26,l32));}
sal_free(l184);(void)trie_destroy(l236);l231 = VRF_PIVOT_TRIE_IPV6_128(l1,l26
);VRF_PIVOT_TRIE_IPV6_128(l1,l26) = NULL;l183 = NULL;l133 = trie_delete(l231,
key,0,&l183);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l26,l32));}(void)
trie_destroy(l231);(void)trie_destroy(PIVOT_BUCKET_TRIE(l235));sal_free(
PIVOT_BUCKET_HANDLE(l235));sal_free(l235);return l133;}int soc_alpm_128_insert
(int l1,void*l3,uint32 l23,int l237,int l238){defip_alpm_ipv6_128_entry_t l191
,l192;soc_mem_t l22;void*l193,*l213;int l152,l26;int index;int l8;int l133 = 
SOC_E_NONE;uint32 l24;int l239 = 0;l8 = L3_DEFIP_MODE_128;l22 = 
L3_DEFIP_ALPM_IPV6_128m;l193 = ((uint32*)&(l191));l213 = ((uint32*)&(l192));
SOC_IF_ERROR_RETURN(l18(l1,l3,l193,l213,l22,l23,&l24));SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l3,&l152,&l26));if(soc_feature(l1,
soc_feature_ipmc_defip)){l239 = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,l3,
MULTICAST_ROUTEf);}if((l152!= SOC_L3_VRF_OVERRIDE)&&(l26 == 0)&&
soc_alpm_mode_get(l1)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}if((l152 == 
SOC_L3_VRF_OVERRIDE)||(l239)){l133 = l2(l1,l3,&index,0);if(SOC_SUCCESS(l133))
{VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l8);VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l8);}
else if(l133 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID,l8);}return(l133
);}if(l152!= SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l1)){if(
VRF_TRIE_ROUTES_CNT(l1,l26,l8) == 0){if(!l24){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n"),l152));return SOC_E_PARAM;}}}}if(!
VRF_TRIE_INIT_COMPLETED(l1,l26,l8)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_128_insert:VRF %d is not ""initialized\n"),l26));l133 = l225(l1,
l26,l8);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),l26,l8));return l133;
}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_insert:VRF %d/%d trie init ""completed\n"),l26,l8));}if(l238&
SOC_ALPM_LOOKUP_HIT){l133 = l154(l1,l3,l193,l213,l22,l237);}else{if(l237 == -
1){l237 = 0;}l133 = l174(l1,l3,l22,l193,l213,&index,SOC_ALPM_BKT_ENTRY_TO_IDX
(l237),l238);}if(l133!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_alpm_128_insert: ""Route Insertion Failed :%s\n"),l1,soc_errmsg
(l133)));}return(l133);}int soc_alpm_128_lookup(int l1,void*l6,void*l14,int*
l15,int*l240){defip_alpm_ipv6_128_entry_t l191;soc_mem_t l22;int bucket_index
;int tcam_index = -1;void*l193;int l152,l26;int l8 = 2,l123;int l133 = 
SOC_E_NONE;int l239 = 0;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&
l152,&l26));*l15 = 0;l133 = l13(l1,l6,l14,l15,&l123,&l8);if(SOC_SUCCESS(l133)
){if(soc_feature(l1,soc_feature_ipmc_defip)){l239 = 
soc_L3_DEFIP_PAIR_128m_field32_get(l1,l14,MULTICAST_ROUTEf);}}if((l152 == 
SOC_L3_VRF_OVERRIDE)||(l239)){*l240 = 0;return l133;}if(!
VRF_TRIE_INIT_COMPLETED(l1,l26,l8)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_alpm_lookup:VRF %d is not ""initialized\n"),l26));*l15 = 0;*l240 = 0;
return SOC_E_NOT_FOUND;}l22 = L3_DEFIP_ALPM_IPV6_128m;l193 = ((uint32*)&(l191
));SOC_ALPM_LPM_LOCK(l1);l133 = l149(l1,l6,l22,l193,&tcam_index,&bucket_index
,l15,TRUE);SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l133)){*l240 = tcam_index;*
l15 = bucket_index<<2;return l133;}l133 = l25(l1,l193,l22,l8,l152,
bucket_index,*l15,l14);*l240 = SOC_ALPM_LOOKUP_HIT|tcam_index;return(l133);}
static int l241(int l1,void*l6,void*l14,int l26,int*tcam_index,int*
bucket_index,int*l124,int l242){int l133 = SOC_E_NONE;int l132,l243,l32,l145 = 
0;uint32 l9,l10,l153;defip_aux_scratch_entry_t l12;int l244,l245;int index;
soc_mem_t l22,l246;int l247,l248,l249;soc_field_t l250[4] = {IP_ADDR0_LWRf,
IP_ADDR1_LWRf,IP_ADDR0_UPRf,IP_ADDR1_UPRf};uint32 l251[
SOC_MAX_MEM_FIELD_WORDS] = {0};int l252 = -1;int l253 = 0;l32 = 
L3_DEFIP_MODE_128;l246 = L3_DEFIP_PAIR_128m;l244 = soc_mem_field32_get(l1,
l246,l6,GLOBAL_ROUTEf);l245 = soc_mem_field32_get(l1,l246,l6,VRF_ID_0_LWRf);
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d\n"),l26 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l32,l244,l245));soc_alpm_db_ent_type_encoding(l1,l26 == 
SOC_L3_VRF_GLOBAL?SOC_VRF_MAX(l1)+1:l245,&l9,&l10);if(l26 == 
SOC_L3_VRF_GLOBAL){SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l153);
soc_mem_field32_set(l1,l246,l6,GLOBAL_ROUTEf,1);soc_mem_field32_set(l1,l246,
l6,VRF_ID_0_LWRf,0);}else{SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l153);}if(l242){l9
++;}sal_memset(&l12,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(
l7(l1,l6,l32,l9,l10,0,&l12));if(l26 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set
(l1,l246,l6,GLOBAL_ROUTEf,l244);soc_mem_field32_set(l1,l246,l6,VRF_ID_0_LWRf,
l245);}SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l12,TRUE,&l145,
tcam_index,bucket_index));if(l145 == 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Could not find bucket\n")));return SOC_E_NOT_FOUND;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit in memory %s, index %d, "
"bucket_index %d\n"),SOC_MEM_NAME(l1,l246),soc_alpm_logical_idx(l1,l246,
SOC_ALPM_128_DEFIP_TO_PAIR((*tcam_index)>>1),1),*bucket_index));l22 = 
L3_DEFIP_ALPM_IPV6_128m;l133 = l131(l1,l6,&l248);if(SOC_FAILURE(l133)){return
l133;}l249 = SOC_ALPM_V6_SCALE_CHECK(l1,l32)?16:8;LOG_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"Start searching mem %s bucket %d(count %d) "
"for Length %d\n"),SOC_MEM_NAME(l1,l22),*bucket_index,l249,l248));for(l132 = 
0;l132<l249;l132++){uint32 l193[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l254[4]
= {0};uint32 l255[4] = {0};uint32 l256[4] = {0};int l257;l133 = 
_soc_alpm_mem_index(l1,l22,*bucket_index,l132,l153,&index);if(l133 == 
SOC_E_FULL){continue;}SOC_IF_ERROR_RETURN(soc_mem_read(l1,l22,MEM_BLOCK_ANY,
index,(void*)&l193));l257 = soc_mem_field32_get(l1,l22,&l193,VALIDf);l247 = 
soc_mem_field32_get(l1,l22,&l193,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Bucket %5d Index %6d: Valid %d, Length %d\n"),*bucket_index,
index,l257,l247));if(!l257||(l247>l248)){continue;}SHR_BITSET_RANGE(l254,128-
l247,l247);(void)soc_mem_field_get(l1,l22,(uint32*)&l193,KEYf,(uint32*)l255);
l256[3] = soc_mem_field32_get(l1,l246,l6,l250[3]);l256[2] = 
soc_mem_field32_get(l1,l246,l6,l250[2]);l256[1] = soc_mem_field32_get(l1,l246
,l6,l250[1]);l256[0] = soc_mem_field32_get(l1,l246,l6,l250[0]);LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tmask %08x %08x %08x %08x \n"
"\t key %08x %08x %08x %08x \n""\thost %08x %08x %08x %08x \n"),l254[3],l254[
2],l254[1],l254[0],l255[3],l255[2],l255[1],l255[0],l256[3],l256[2],l256[1],
l256[0]));for(l243 = 3;l243>= 0;l243--){if((l256[l243]&l254[l243])!= (l255[
l243]&l254[l243])){break;}}if(l243>= 0){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"Found a match in mem %s bucket %d, ""index %d\n"),
SOC_MEM_NAME(l1,l22),*bucket_index,index));if(l252 == -1||l252<l247){l252 = 
l247;l253 = index;sal_memcpy(l251,l193,sizeof(l193));}}if(l252!= -1){l133 = 
l25(l1,&l251,l22,l32,l26,*bucket_index,l253,l14);if(SOC_SUCCESS(l133)){*l124 = 
l253;if(bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit mem %s bucket %d, index %d\n"
),SOC_MEM_NAME(l1,l22),*bucket_index,l253));}}return l133;}*l124 = 
soc_alpm_logical_idx(l1,l246,SOC_ALPM_128_DEFIP_TO_PAIR((*tcam_index)>>1),1);
SOC_IF_ERROR_RETURN(soc_mem_read(l1,l246,MEM_BLOCK_ANY,*l124,(void*)l14));
return SOC_E_NONE;}int soc_alpm_128_find_best_match(int l1,void*l6,void*l14,
int*l15,int l242){int l133 = SOC_E_NONE;int l132,l243;int l258,l259;
defip_pair_128_entry_t l260;uint32 l261,l255,l256;int l247,l248;int l262,l263
;int l152,l26 = 0;int tcam_index,bucket_index;soc_mem_t l246;soc_field_t l264
[4] = {IP_ADDR_MASK1_UPRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_LWRf};soc_field_t l265[4] = {IP_ADDR1_UPRf,IP_ADDR0_UPRf,
IP_ADDR1_LWRf,IP_ADDR0_LWRf};l246 = L3_DEFIP_PAIR_128m;if(!
SOC_URPF_STATUS_GET(l1)&&l242){return SOC_E_PARAM;}l258 = soc_mem_index_min(
l1,l246);l259 = soc_mem_index_count(l1,l246);if(SOC_URPF_STATUS_GET(l1)){l259
>>= 1;}if(soc_alpm_mode_get(l1)){l259>>= 1;l258+= l259;}if(l242){l258+= 
soc_mem_index_count(l1,l246)/2;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Start LPM searchng from %d, count %d\n"),l258,l259));for(l132 = l258;l132<
l258+l259;l132++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l246,MEM_BLOCK_ANY,l132
,(void*)&l260));if(!soc_mem_field32_get(l1,l246,&l260,VALID0_LWRf)){continue;
}l262 = soc_mem_field32_get(l1,l246,&l260,GLOBAL_HIGHf);l263 = 
soc_mem_field32_get(l1,l246,&l260,GLOBAL_ROUTEf);if(!l263||!l262){continue;}
l133 = l131(l1,l6,&l248);l133 = l131(l1,&l260,&l247);if(SOC_FAILURE(l133)||(
l247>l248)){continue;}for(l243 = 0;l243<4;l243++){l261 = soc_mem_field32_get(
l1,l246,&l260,l264[l243]);l255 = soc_mem_field32_get(l1,l246,&l260,l265[l243]
);l256 = soc_mem_field32_get(l1,l246,l6,l265[l243]);if((l256&l261)!= (l255&
l261)){break;}}if(l243<4){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Hit Global High route in index = %d\n"),l243));sal_memcpy(l14,&l260,
sizeof(l260));*l15 = l132;return SOC_E_NONE;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for Global Low and VRF routes\n")));SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l6,&l152,&l26));l133 = l241(l1,l6,l14,l26,&
tcam_index,&bucket_index,l15,l242);if(l133 == SOC_E_NOT_FOUND){l26 = 
SOC_L3_VRF_GLOBAL;LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Not found in VRF region, try Global ""region\n")));l133 = l241(l1,l6,l14,l26
,&tcam_index,&bucket_index,l15,l242);}if(SOC_SUCCESS(l133)){tcam_index = 
soc_alpm_logical_idx(l1,l246,SOC_ALPM_128_DEFIP_TO_PAIR(tcam_index>>1),1);
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in %s region in TCAM index %d, ""buckekt_index %d\n"),l26 == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",tcam_index,bucket_index));}else{
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Search miss for given address\n")
));}return(l133);}int soc_alpm_128_delete(int l1,void*l6,int l237,int l238){
int l152,l26;int l8;int l133 = SOC_E_NONE;int l211 = 0;int l239 = 0;l8 = 
L3_DEFIP_MODE_128;if(soc_feature(l1,soc_feature_ipmc_defip)){l239 = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,l6,MULTICAST_ROUTEf);}
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l6,&l152,&l26));if((l152 == 
SOC_L3_VRF_OVERRIDE)||(l239)){l133 = l5(l1,l6);if(SOC_SUCCESS(l133)){
VRF_TRIE_ROUTES_DEC(l1,MAX_VRF_ID,l8);VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l8);}
return(l133);}else{int tcam_index;if(!VRF_TRIE_INIT_COMPLETED(l1,l26,l8)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_alpm_128_delete:VRF %d/%d is not ""initialized\n"),l26,l8));return
SOC_E_NONE;}if(l237 == -1){l237 = 0;}l211 = !(l238&SOC_ALPM_DELETE_ALL);
tcam_index = l238&~(SOC_ALPM_LOOKUP_HIT|SOC_ALPM_DELETE_ALL);l133 = l210(l1,
l6,SOC_ALPM_BKT_ENTRY_TO_IDX(l237),tcam_index,l237,l211);}return(l133);}
static void l111(int l1,void*l14,int index,l106 l112){l112[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l58));l112[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l99[(l1)]->l56));l112[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l62));l112[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l99[(l1)]->l60));l112[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l59));l112[5] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l99[(l1)]->l57));l112[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l63));l112[7] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l99[(l1)]->l61));if((!(
SOC_IS_HURRICANE(l1)))&&(((l99[(l1)]->l81)!= NULL))){int l266;(void)
soc_alpm_128_lpm_vrf_get(l1,l14,(int*)&l112[8],&l266);}else{l112[8] = 0;};}
static int l267(l106 l108,l106 l109){int l234;for(l234 = 0;l234<9;l234++){{if
((l108[l234])<(l109[l234])){return-1;}if((l108[l234])>(l109[l234])){return 1;
}};}return(0);}static void l268(int l1,void*l3,uint32 l269,uint32 l126,int
l123){l106 l270;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l99[(l1)]->l78))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l99[(l1)]->l77))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l99[(l1)]->l76))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l99[(l1)]->l75))){l270[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l99[(l1)]->l58));l270[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l99[(l1)]->l56));l270[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l99[(l1)]->l62));l270[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l99[(l1)]->l60));l270[4] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l99[(l1)]->l59));l270[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l99[(l1)]->l57));l270[6] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l99[(l1)]->l63));l270[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l99[(l1)]->l61));if((!(SOC_IS_HURRICANE(l1)))&&(((l99[(l1)]->l81)!= NULL)))
{int l266;(void)soc_alpm_128_lpm_vrf_get(l1,l3,(int*)&l270[8],&l266);}else{
l270[8] = 0;};l125((l110[(l1)]),l267,l270,l123,l126,l269);}}static void l271(
int l1,void*l6,uint32 l269){l106 l270;int l123 = -1;int l133;uint16 index;
l270[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l58));l270[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l56));l270[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l62));l270[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l60));l270[4] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l59));l270[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l57));l270[6] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l63));l270[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l61));if((!(SOC_IS_HURRICANE(l1)))&&(((l99[(l1)]->l81)!= NULL)))
{int l266;(void)soc_alpm_128_lpm_vrf_get(l1,l6,(int*)&l270[8],&l266);}else{
l270[8] = 0;};index = l269;l133 = l128((l110[(l1)]),l267,l270,l123,index);if(
SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\ndel  index: H %d error %d\n"),index,l133));}}static int l272(int l1,void*
l6,int l123,int*l124){l106 l270;int l133;uint16 index = (0xFFFF);l270[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l58));l270[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l56));l270[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l62));l270[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l60));l270[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l59));l270[5] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l57));l270[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l63));l270[7] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l61));if((!(SOC_IS_HURRICANE(l1)))
&&(((l99[(l1)]->l81)!= NULL))){int l266;(void)soc_alpm_128_lpm_vrf_get(l1,l6,
(int*)&l270[8],&l266);}else{l270[8] = 0;};l133 = l120((l110[(l1)]),l267,l270,
l123,&index);if(SOC_FAILURE(l133)){*l124 = 0xFFFFFFFF;return(l133);}*l124 = 
index;return(SOC_E_NONE);}static int l273(int l1,void*l6,uint32 l269){l106
l270;int l123 = -1;int l133;uint16 index;l270[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l58));l270[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l56));l270[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l62));l270[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l60));l270[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l59));l270[5] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l57));l270[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l6)
,(l99[(l1)]->l63));l270[7] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l6),(l99[(l1)]->l61));if((!(SOC_IS_HURRICANE(l1)))
&&(((l99[(l1)]->l81)!= NULL))){int l266;(void)soc_alpm_128_lpm_vrf_get(l1,l6,
(int*)&l270[8],&l266);}else{l270[8] = 0;};index = l269;l133 = l130((l110[(l1)
]),l267,l270,l123,index);if(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\nveri index: H %d error %d\n"),index,l133));}return l133;}
static uint16 l113(uint8*l114,int l115){return(_shr_crc16b(0,l114,l115));}
static int l116(int l19,int l101,int l102,l105**l117){l105*l121;int index;if(
l102>l101){return SOC_E_MEMORY;}l121 = sal_alloc(sizeof(l105),"lpm_hash");if(
l121 == NULL){return SOC_E_MEMORY;}sal_memset(l121,0,sizeof(*l121));l121->l19
= l19;l121->l101 = l101;l121->l102 = l102;l121->l103 = sal_alloc(l121->l102*
sizeof(*(l121->l103)),"hash_table");if(l121->l103 == NULL){sal_free(l121);
return SOC_E_MEMORY;}l121->l104 = sal_alloc(l121->l101*sizeof(*(l121->l104)),
"link_table");if(l121->l104 == NULL){sal_free(l121->l103);sal_free(l121);
return SOC_E_MEMORY;}for(index = 0;index<l121->l102;index++){l121->l103[index
] = (0xFFFF);}for(index = 0;index<l121->l101;index++){l121->l104[index] = (
0xFFFF);}*l117 = l121;return SOC_E_NONE;}static int l118(l105*l119){if(l119!= 
NULL){sal_free(l119->l103);sal_free(l119->l104);sal_free(l119);}return
SOC_E_NONE;}static int l120(l105*l121,l107 l122,l106 entry,int l123,uint16*
l124){int l1 = l121->l19;uint16 l274;uint16 index;l274 = l113((uint8*)entry,(
32*9))%l121->l102;index = l121->l103[l274];;;while(index!= (0xFFFF)){uint32
l14[SOC_MAX_MEM_FIELD_WORDS];l106 l112;int l275;l275 = index;
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l275,l14));l111(
l1,l14,index,l112);if((*l122)(entry,l112) == 0){*l124 = index;;return(
SOC_E_NONE);}index = l121->l104[index&(0x3FFF)];;};return(SOC_E_NOT_FOUND);}
static int l125(l105*l121,l107 l122,l106 entry,int l123,uint16 l126,uint16
l127){int l1 = l121->l19;uint16 l274;uint16 index;uint16 l276;l274 = l113((
uint8*)entry,(32*9))%l121->l102;index = l121->l103[l274];;;;l276 = (0xFFFF);
if(l126!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l14[
SOC_MAX_MEM_FIELD_WORDS];l106 l112;int l275;l275 = index;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l275,l14));l111(l1,l14,index,l112);
if((*l122)(entry,l112) == 0){if(l127!= index){;if(l276 == (0xFFFF)){l121->
l103[l274] = l127;l121->l104[l127&(0x3FFF)] = l121->l104[index&(0x3FFF)];l121
->l104[index&(0x3FFF)] = (0xFFFF);}else{l121->l104[l276&(0x3FFF)] = l127;l121
->l104[l127&(0x3FFF)] = l121->l104[index&(0x3FFF)];l121->l104[index&(0x3FFF)]
= (0xFFFF);}};return(SOC_E_NONE);}l276 = index;index = l121->l104[index&(
0x3FFF)];;}}l121->l104[l127&(0x3FFF)] = l121->l103[l274];l121->l103[l274] = 
l127;return(SOC_E_NONE);}static int l128(l105*l121,l107 l122,l106 entry,int
l123,uint16 l129){uint16 l274;uint16 index;uint16 l276;l274 = l113((uint8*)
entry,(32*9))%l121->l102;index = l121->l103[l274];;;l276 = (0xFFFF);while(
index!= (0xFFFF)){if(l129 == index){;if(l276 == (0xFFFF)){l121->l103[l274] = 
l121->l104[l129&(0x3FFF)];l121->l104[l129&(0x3FFF)] = (0xFFFF);}else{l121->
l104[l276&(0x3FFF)] = l121->l104[l129&(0x3FFF)];l121->l104[l129&(0x3FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l276 = index;index = l121->l104[index&(0x3FFF)];
;}return(SOC_E_NOT_FOUND);}static int l130(l105*l121,l107 l122,l106 entry,int
l123,uint16 l277){uint16 l274;uint16 index;l274 = l113((uint8*)entry,(32*9))%
l121->l102;index = l121->l103[l274];;;while(index!= (0xFFFF)){if(l277 == 
index){;return(SOC_E_NONE);}index = l121->l104[index&(0x3FFF)];;}return(
SOC_E_NOT_FOUND);}static int l278(int l1,void*l14){return(SOC_E_NONE);}void
soc_alpm_128_lpm_state_dump(int l1){int l132;int l279;l279 = ((3*(128+2+1))-1
);if(!bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){return;}for
(l132 = l279;l132>= 0;l132--){if((l132!= ((3*(128+2+1))-1))&&((l41[(l1)][(
l132)].l34) == -1)){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"PFX = %d P = %d N = %d START = %d ""END = %d VENT = %d FENT = %d\n"),l132,(
l41[(l1)][(l132)].l36),(l41[(l1)][(l132)].next),(l41[(l1)][(l132)].l34),(l41[
(l1)][(l132)].l35),(l41[(l1)][(l132)].l37),(l41[(l1)][(l132)].l38)));}
COMPILER_REFERENCE(l278);}static int l280(int l1,int l281,int index,uint32*
l14,int l4){int l282;uint32 l283,l284,l285;uint32 l286;int l287;uint32 l288[
SOC_MAX_MEM_FIELD_WORDS] = {0};if(!SOC_URPF_STATUS_GET(l1)){return(SOC_E_NONE
);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l282 = (soc_mem_index_count(
l1,L3_DEFIP_PAIR_128m)>>1);}else if(SOC_IS_APOLLO(l1)){l282 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l282 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}if(l4>= 2){
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l281+l282,l288))
;l4 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)
),(l288),(l99[(l1)]->l74));}if(((l99[(l1)]->l44)!= NULL)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l44),(0));}l283 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14),(l99[(l1)]->l62));l286 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l63));l284 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l14),(l99[(l1)]->l60));l285 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l61));l287 = ((!l283)&&(!l286)&&(!l284)&&(!l285))?1:0;l283 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l77));l286 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l14),(l99[(l1)]->l75));l284 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l76));l285 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l14),(l99[(l1)]->l76));if(l283&&l286&&l284&&l285){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l14
),(l99[(l1)]->l74),(l4|l287));}return l171(l1,MEM_BLOCK_ANY,index+l282,index,
l14);}static int l289(int l1,int l290,int l291){uint32 l14[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l290,l14));l268(l1,l14,l291,0x4000,0);SOC_IF_ERROR_RETURN(l171(
l1,MEM_BLOCK_ANY,l291,l290,l14));SOC_IF_ERROR_RETURN(l280(l1,l290,l291,l14,2)
);do{int l292,l293;l292 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l290
),1);l293 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l291),1);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l293))<<1) = ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l292))<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR
((l293))<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l293
))<<1)) = SOC_ALPM_128_ADDR_LWR((l293))<<1;}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l292))<<1) = NULL;}while(0);return(SOC_E_NONE);}static
int l294(int l1,int l123,int l8){int l290;int l291;l291 = (l41[(l1)][(l123)].
l35)+1;l290 = (l41[(l1)][(l123)].l34);if(l290!= l291){SOC_IF_ERROR_RETURN(
l289(l1,l290,l291));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l8);}(l41[(l1)][(l123)]
.l34)+= 1;(l41[(l1)][(l123)].l35)+= 1;return(SOC_E_NONE);}static int l295(int
l1,int l123,int l8){int l290;int l291;l291 = (l41[(l1)][(l123)].l34)-1;if((
l41[(l1)][(l123)].l37) == 0){(l41[(l1)][(l123)].l34) = l291;(l41[(l1)][(l123)
].l35) = l291-1;return(SOC_E_NONE);}l290 = (l41[(l1)][(l123)].l35);
SOC_IF_ERROR_RETURN(l289(l1,l290,l291));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l8)
;(l41[(l1)][(l123)].l34)-= 1;(l41[(l1)][(l123)].l35)-= 1;return(SOC_E_NONE);}
static int l296(int l1,int l123,int l8,void*l14,int*l297){int l298;int l299;
int l300;int l301;if((l41[(l1)][(l123)].l37) == 0){l301 = ((3*(128+2+1))-1);
if(soc_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL){if(l123<= (((3*(128+2+1))
/3)-1)){l301 = (((3*(128+2+1))/3)-1);}}while((l41[(l1)][(l301)].next)>l123){
l301 = (l41[(l1)][(l301)].next);}l299 = (l41[(l1)][(l301)].next);if(l299!= -1
){(l41[(l1)][(l299)].l36) = l123;}(l41[(l1)][(l123)].next) = (l41[(l1)][(l301
)].next);(l41[(l1)][(l123)].l36) = l301;(l41[(l1)][(l301)].next) = l123;(l41[
(l1)][(l123)].l38) = ((l41[(l1)][(l301)].l38)+1)/2;(l41[(l1)][(l301)].l38)-= 
(l41[(l1)][(l123)].l38);(l41[(l1)][(l123)].l34) = (l41[(l1)][(l301)].l35)+(
l41[(l1)][(l301)].l38)+1;(l41[(l1)][(l123)].l35) = (l41[(l1)][(l123)].l34)-1;
(l41[(l1)][(l123)].l37) = 0;}l300 = l123;while((l41[(l1)][(l300)].l38) == 0){
l300 = (l41[(l1)][(l300)].next);if(l300 == -1){l300 = l123;break;}}while((l41
[(l1)][(l300)].l38) == 0){l300 = (l41[(l1)][(l300)].l36);if(l300 == -1){if((
l41[(l1)][(l123)].l37) == 0){l298 = (l41[(l1)][(l123)].l36);l299 = (l41[(l1)]
[(l123)].next);if(-1!= l298){(l41[(l1)][(l298)].next) = l299;}if(-1!= l299){(
l41[(l1)][(l299)].l36) = l298;}}return(SOC_E_FULL);}}while(l300>l123){l299 = 
(l41[(l1)][(l300)].next);SOC_IF_ERROR_RETURN(l295(l1,l299,l8));(l41[(l1)][(
l300)].l38)-= 1;(l41[(l1)][(l299)].l38)+= 1;l300 = l299;}while(l300<l123){
SOC_IF_ERROR_RETURN(l294(l1,l300,l8));(l41[(l1)][(l300)].l38)-= 1;l298 = (l41
[(l1)][(l300)].l36);(l41[(l1)][(l298)].l38)+= 1;l300 = l298;}(l41[(l1)][(l123
)].l37)+= 1;(l41[(l1)][(l123)].l38)-= 1;(l41[(l1)][(l123)].l35)+= 1;*l297 = (
l41[(l1)][(l123)].l35);sal_memcpy(l14,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);return(
SOC_E_NONE);}static int l302(int l1,int l123,int l8,void*l14,int l303){int
l298;int l299;int l290;int l291;uint32 l304[SOC_MAX_MEM_FIELD_WORDS];int l133
;int l140;l290 = (l41[(l1)][(l123)].l35);l291 = l303;(l41[(l1)][(l123)].l37)
-= 1;(l41[(l1)][(l123)].l38)+= 1;(l41[(l1)][(l123)].l35)-= 1;if(l291!= l290){
if((l133 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l290,l304))<0){return l133
;}l268(l1,l304,l291,0x4000,0);if((l133 = l171(l1,MEM_BLOCK_ANY,l291,l290,l304
))<0){return l133;}if((l133 = l280(l1,l290,l291,l304,2))<0){return l133;}}
l140 = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l291,1);l303 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l290,1);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l140)<<1) = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
l303)<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l140)<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l140)<<1)) = 
SOC_ALPM_128_ADDR_LWR(l140)<<1;}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l303
)<<1) = NULL;sal_memcpy(l304,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),
soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);l291 = l290;l268(l1,l304,l291,
0x4000,0);if((l133 = l171(l1,MEM_BLOCK_ANY,l291,l290,l304))<0){return l133;}
if((l133 = l280(l1,l290,l290,l304,0))<0){return l133;}if((l41[(l1)][(l123)].
l37) == 0){l298 = (l41[(l1)][(l123)].l36);assert(l298!= -1);l299 = (l41[(l1)]
[(l123)].next);(l41[(l1)][(l298)].next) = l299;(l41[(l1)][(l298)].l38)+= (l41
[(l1)][(l123)].l38);(l41[(l1)][(l123)].l38) = 0;if(l299!= -1){(l41[(l1)][(
l299)].l36) = l298;}(l41[(l1)][(l123)].next) = -1;(l41[(l1)][(l123)].l36) = -
1;(l41[(l1)][(l123)].l34) = -1;(l41[(l1)][(l123)].l35) = -1;}return(l133);}
int soc_alpm_128_lpm_vrf_get(int l19,void*lpm_entry,int*l26,int*l305){int l152
;if(((l99[(l19)]->l85)!= NULL)){l152 = soc_L3_DEFIP_PAIR_128m_field32_get(l19
,lpm_entry,VRF_ID_0_LWRf);*l305 = l152;if(soc_L3_DEFIP_PAIR_128m_field32_get(
l19,lpm_entry,VRF_ID_MASK0_LWRf)){*l26 = l152;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l19,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l19)]->l87))){*l26 = SOC_L3_VRF_GLOBAL;*l305 = SOC_VRF_MAX(
l19)+1;}else{*l26 = SOC_L3_VRF_OVERRIDE;}}else{*l26 = SOC_L3_VRF_DEFAULT;}
return(SOC_E_NONE);}static int l306(int l1,void*entry,int*l16){int l123=0;int
l133;int l152;int l307;l133 = l131(l1,entry,&l123);if(l133<0){return l133;}
l123+= 0;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,entry,&l152,&l133));
l307 = soc_alpm_mode_get(l1);switch(l152){case SOC_L3_VRF_GLOBAL:if(l307 == 
SOC_ALPM_MODE_PARALLEL){*l16 = l123+((3*(128+2+1))/3);}else{*l16 = l123;}
break;case SOC_L3_VRF_OVERRIDE:*l16 = l123+2*((3*(128+2+1))/3);break;default:
if(l307 == SOC_ALPM_MODE_PARALLEL){*l16 = l123;}else{*l16 = l123+((3*(128+2+1
))/3);}break;}return(SOC_E_NONE);}static int l13(int l1,void*l6,void*l14,int*
l15,int*l16,int*l8){int l133;int l124;int l123 = 0;*l8 = L3_DEFIP_MODE_128;
l306(l1,l6,&l123);*l16 = l123;if(l272(l1,l6,l123,&l124) == SOC_E_NONE){*l15 = 
l124;if((l133 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(*l8)?*l15:(*l15>>1)
,l14))<0){return l133;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}int
soc_alpm_128_ipmc_war(int l19,int l308){int index = -1;defip_pair_128_entry_t
lpm_entry;soc_mem_t l22 = L3_DEFIP_PAIR_128m;int l309 = 63;if(!(soc_feature(
l19,soc_feature_alpm)&&soc_feature(l19,soc_feature_td2p_a0_sw_war)&&
soc_property_get(l19,spn_L3_ALPM_ENABLE,0))){return SOC_E_NONE;}if(
soc_mem_index_count(l19,L3_DEFIP_PAIR_128m)<= 0){return SOC_E_NONE;}if(!
SOC_IS_TRIDENT2PLUS(l19)){return SOC_E_NONE;}if(SOC_WARM_BOOT(l19)){return
SOC_E_NONE;}l309 = soc_mem_field_length(l19,ING_ACTIVE_L3_IIF_PROFILEm,
RPA_ID_PROFILEf);sal_memset(&lpm_entry,0,soc_mem_entry_words(l19,l22)*4);
soc_mem_field32_set(l19,l22,&lpm_entry,VALID0_LWRf,1);soc_mem_field32_set(l19
,l22,&lpm_entry,VALID1_LWRf,1);soc_mem_field32_set(l19,l22,&lpm_entry,
VALID0_UPRf,1);soc_mem_field32_set(l19,l22,&lpm_entry,VALID1_UPRf,1);
soc_mem_field32_set(l19,l22,&lpm_entry,MODE0_LWRf,3);soc_mem_field32_set(l19,
l22,&lpm_entry,MODE1_LWRf,3);soc_mem_field32_set(l19,l22,&lpm_entry,
MODE0_UPRf,3);soc_mem_field32_set(l19,l22,&lpm_entry,MODE1_UPRf,3);
soc_mem_field32_set(l19,l22,&lpm_entry,MODE_MASK0_LWRf,3);soc_mem_field32_set
(l19,l22,&lpm_entry,MODE_MASK1_LWRf,3);soc_mem_field32_set(l19,l22,&lpm_entry
,MODE_MASK0_UPRf,3);soc_mem_field32_set(l19,l22,&lpm_entry,MODE_MASK1_UPRf,3)
;soc_mem_field32_set(l19,l22,&lpm_entry,ENTRY_TYPE_MASK0_LWRf,1);
soc_mem_field32_set(l19,l22,&lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);
soc_mem_field32_set(l19,l22,&lpm_entry,ENTRY_TYPE_MASK0_UPRf,1);
soc_mem_field32_set(l19,l22,&lpm_entry,ENTRY_TYPE_MASK1_UPRf,1);
soc_mem_field32_set(l19,l22,&lpm_entry,MULTICAST_ROUTEf,1);
soc_mem_field32_set(l19,l22,&lpm_entry,GLOBAL_ROUTEf,1);soc_mem_field32_set(
l19,l22,&lpm_entry,RPA_IDf,l309-1);soc_mem_field32_set(l19,l22,&lpm_entry,
EXPECTED_L3_IIFf,16383);soc_mem_field32_set(l19,l22,&lpm_entry,IP_ADDR0_LWRf,
0);soc_mem_field32_set(l19,l22,&lpm_entry,IP_ADDR1_LWRf,0);
soc_mem_field32_set(l19,l22,&lpm_entry,IP_ADDR0_UPRf,0);soc_mem_field32_set(
l19,l22,&lpm_entry,IP_ADDR1_UPRf,0xff000000);soc_mem_field32_set(l19,l22,&
lpm_entry,IP_ADDR_MASK0_LWRf,0);soc_mem_field32_set(l19,l22,&lpm_entry,
IP_ADDR_MASK1_UPRf,0);soc_mem_field32_set(l19,l22,&lpm_entry,
IP_ADDR_MASK0_LWRf,0);soc_mem_field32_set(l19,l22,&lpm_entry,
IP_ADDR_MASK1_UPRf,0xff000000);if(l308){SOC_IF_ERROR_RETURN(l2(l19,&lpm_entry
,&index,0));}else{SOC_IF_ERROR_RETURN(l5(l19,&lpm_entry));}return SOC_E_NONE;
}int soc_alpm_128_lpm_init(int l1){int l279;int l132;int l310;int l311;if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}l279 = (3*(128+2
+1));l311 = sizeof(l39)*(l279);if((l41[(l1)]!= NULL)){SOC_IF_ERROR_RETURN(
soc_alpm_128_deinit(l1));}l99[l1] = sal_alloc(sizeof(l97),
"lpm_128_field_state");if(NULL == l99[l1]){return(SOC_E_MEMORY);}(l99[l1])->
l43 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l99[l1])->l44 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l99[l1])->l45 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l99[l1])->l46 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l99[l1])->l47 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l99[l1])->l48 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l99[l1])->l49 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MULTICAST_ROUTEf);(l99[l1])->l50 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPA_IDf);(l99[l1])->l51 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,EXPECTED_L3_IIFf);(l99[l1])->l52 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf);(l99[l1])->l53 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf);(l99[l1])->l54 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,L3MC_INDEXf);(l99[l1])->l55 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l99[l1])->l58 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l99[l1])->l56 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l99[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l99[l1])->l57 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l99[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l99[l1])->
l60 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l99[l1
])->l63 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l99[l1])->l61 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l99[l1])->l66 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l99[l1])->l64 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l99[l1])->l67 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l99[l1])->l65 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l99[l1])->l70 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l99[l1])->l68 = soc_mem_fieldinfo_get(l1
,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l99[l1])->l71 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l99[l1])->l69 = soc_mem_fieldinfo_get
(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l99[l1])->l72 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l99[l1])->l73 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l99[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l99[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l99[l1])->l75 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l99[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l99[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l99[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l99[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l99[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l99[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l99[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l99[l1])->l83
= soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l99[l1])->
l86 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(l99[l1]
)->l84 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_UPRf);(l99[
l1])->l87 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l99[l1
])->l88 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l99[l1])
->l89 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l99[l1])->
l90 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l99[l1])->
l91 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf)
;(l99[l1])->l92 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
FLEX_CTR_POOL_NUMBERf);(l99[l1])->l93 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf);(l99[l1])->l94 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_LWRf);(l99[l1])
->l95 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf);(
l99[l1])->l96 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
ENTRY_TYPE_MASK1_UPRf);(l41[(l1)]) = sal_alloc(l311,"LPM 128 prefix info");if
(NULL == (l41[(l1)])){sal_free(l99[l1]);l99[l1] = NULL;return(SOC_E_MEMORY);}
SOC_ALPM_LPM_LOCK(l1);sal_memset((l41[(l1)]),0,l311);for(l132 = 0;l132<l279;
l132++){(l41[(l1)][(l132)].l34) = -1;(l41[(l1)][(l132)].l35) = -1;(l41[(l1)][
(l132)].l36) = -1;(l41[(l1)][(l132)].next) = -1;(l41[(l1)][(l132)].l37) = 0;(
l41[(l1)][(l132)].l38) = 0;}l310 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)
;if(SOC_URPF_STATUS_GET(l1)){l310>>= 1;}if(soc_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL){(l41[(l1)][(((3*(128+2+1))-1))].l35) = (l310>>1)-1;(
l41[(l1)][(((((3*(128+2+1))/3)-1)))].l38) = l310>>1;(l41[(l1)][((((3*(128+2+1
))-1)))].l38) = (l310-(l41[(l1)][(((((3*(128+2+1))/3)-1)))].l38));}else{(l41[
(l1)][((((3*(128+2+1))-1)))].l38) = l310;}if((l110[(l1)])!= NULL){if(l118((
l110[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(l110[(l1)]) = 
NULL;}if(l116(l1,l310*2,l310,&(l110[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_MEMORY;}(void)soc_alpm_128_ipmc_war(l1,TRUE);SOC_ALPM_LPM_UNLOCK(l1);
return(SOC_E_NONE);}int soc_alpm_128_lpm_deinit(int l1){if(!soc_feature(l1,
soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);(void)
soc_alpm_128_ipmc_war(l1,FALSE);if((l110[(l1)])!= NULL){l118((l110[(l1)]));(
l110[(l1)]) = NULL;}if((l41[(l1)]!= NULL)){sal_free(l99[l1]);l99[l1] = NULL;
sal_free((l41[(l1)]));(l41[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l1);return(
SOC_E_NONE);}static int l2(int l1,void*l3,int*l312,int l4){int l123;int index
;int l8;uint32 l14[SOC_MAX_MEM_FIELD_WORDS];int l133 = SOC_E_NONE;int l313 = 
0;if(l4>= 2){return SOC_E_INTERNAL;}sal_memcpy(l14,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);
SOC_ALPM_LPM_LOCK(l1);l133 = l13(l1,l3,l14,&index,&l123,&l8);if(l133 == 
SOC_E_NOT_FOUND){l133 = l296(l1,l123,l8,l14,&index);if(l133<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l133);}}else{l313 = 1;}*l312 = index;if(l133 == 
SOC_E_NONE){soc_alpm_128_lpm_state_dump(l1);LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\nsoc_alpm_128_lpm_insert: %d %d\n"),index,l123));if(!l313){
l268(l1,l3,index,0x4000,0);}l133 = l171(l1,MEM_BLOCK_ANY,index,index,l3);if(
l133>= 0){l133 = l280(l1,index,index,l3,l4);}}SOC_ALPM_LPM_UNLOCK(l1);return(
l133);}static int l5(int l1,void*l6){int l123;int index;int l8;uint32 l14[
SOC_MAX_MEM_FIELD_WORDS];int l133 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l133 = 
l13(l1,l6,l14,&index,&l123,&l8);if(l133 == SOC_E_NONE){LOG_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\nsoc_alpm_lpm_delete: %d %d\n"),index,l123))
;l271(l1,l6,index);l133 = l302(l1,l123,l8,l14,index);}
soc_alpm_128_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l133);}static
int l17(int l1,void*l6,void*l14,int*l15){int l123;int l133;int l8;
SOC_ALPM_LPM_LOCK(l1);l133 = l13(l1,l6,l14,l15,&l123,&l8);SOC_ALPM_LPM_UNLOCK
(l1);return(l133);}static int l7(int l19,void*l6,int l8,int l9,int l10,int l11
,defip_aux_scratch_entry_t*l12){uint32 l135;uint32 l314[4] = {0,0,0,0};int
l123 = 0;int l133 = SOC_E_NONE;l135 = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,l6,VALID0_LWRf);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,VALIDf,l135);l135 = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,l6,MODE0_LWRf);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,MODEf,l135);l135 = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,l6,ENTRY_TYPE0_LWRf);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,ENTRY_TYPEf,l10);l135 = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,l6,GLOBAL_ROUTEf);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,GLOBAL_ROUTEf,l135);if(soc_feature(l19,
soc_feature_ipmc_defip)){l135 = soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6
,EXPECTED_L3_IIFf);soc_mem_field32_set(l19,L3_DEFIP_AUX_SCRATCHm,l12,
EXPECTED_L3_IIFf,l135);l135 = soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6,
RPA_IDf);soc_mem_field32_set(l19,L3_DEFIP_AUX_SCRATCHm,l12,RPA_IDf,l135);l135
= soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6,
IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,IPMC_EXPECTED_L3_IIF_MISMATCH_TOCPUf,l135);l135 = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6,
IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,IPMC_EXPECTED_L3_IIF_MISMATCH_DROPf,l135);l135 = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6,L3MC_INDEXf);
soc_mem_field32_set(l19,L3_DEFIP_AUX_SCRATCHm,l12,L3MC_INDEXf,l135);}l135 = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6,ECMPf);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,ECMPf,l135);l135 = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,l6,ECMP_PTRf);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,ECMP_PTRf,l135);l135 = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,l6,NEXT_HOP_INDEXf);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,NEXT_HOP_INDEXf,l135);l135 = soc_mem_field32_get(
l19,L3_DEFIP_PAIR_128m,l6,PRIf);soc_mem_field32_set(l19,L3_DEFIP_AUX_SCRATCHm
,l12,PRIf,l135);l135 = soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6,RPEf);
soc_mem_field32_set(l19,L3_DEFIP_AUX_SCRATCHm,l12,RPEf,l135);l135 =
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6,VRF_ID_0_LWRf);
soc_mem_field32_set(l19,L3_DEFIP_AUX_SCRATCHm,l12,VRFf,l135);
soc_mem_field32_set(l19,L3_DEFIP_AUX_SCRATCHm,l12,DB_TYPEf,l9);l135 = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6,DST_DISCARDf);
soc_mem_field32_set(l19,L3_DEFIP_AUX_SCRATCHm,l12,DST_DISCARDf,l135);l135 = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,l6,CLASS_IDf);soc_mem_field32_set(
l19,L3_DEFIP_AUX_SCRATCHm,l12,CLASS_IDf,l135);l314[0] = soc_mem_field32_get(
l19,L3_DEFIP_PAIR_128m,l6,IP_ADDR0_LWRf);l314[1] = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,l6,IP_ADDR1_LWRf);l314[2] = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,l6,IP_ADDR0_UPRf);l314[3] = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,l6,IP_ADDR1_UPRf);soc_mem_field_set(l19,
L3_DEFIP_AUX_SCRATCHm,(uint32*)l12,IP_ADDRf,(uint32*)l314);l133 = l131(l19,l6
,&l123);if(SOC_FAILURE(l133)){return l133;}soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,IP_LENGTHf,l123);soc_mem_field32_set(l19,
L3_DEFIP_AUX_SCRATCHm,l12,REPLACE_LENf,l11);return(SOC_E_NONE);}static int l18
(int l19,void*lpm_entry,void*l20,void*l21,soc_mem_t l22,uint32 l23,uint32*
l315){uint32 l135;uint32 l314[4];int l123 = 0;int l133 = SOC_E_NONE;uint32 l24
= 0;sal_memset(l20,0,soc_mem_entry_words(l19,l22)*4);l135 = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf);
soc_mem_field32_set(l19,l22,l20,VALIDf,l135);l135 = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,lpm_entry,ECMPf);soc_mem_field32_set(l19,l22,l20,ECMPf,
l135);l135 = soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf);
soc_mem_field32_set(l19,l22,l20,ECMP_PTRf,l135);l135 = soc_mem_field32_get(
l19,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf);soc_mem_field32_set(l19,l22
,l20,NEXT_HOP_INDEXf,l135);l135 = soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,
lpm_entry,PRIf);soc_mem_field32_set(l19,l22,l20,PRIf,l135);l135 = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,lpm_entry,RPEf);
soc_mem_field32_set(l19,l22,l20,RPEf,l135);l135 = soc_mem_field32_get(l19,
L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf);soc_mem_field32_set(l19,l22,l20,
DST_DISCARDf,l135);l135 = soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,
lpm_entry,SRC_DISCARDf);soc_mem_field32_set(l19,l22,l20,SRC_DISCARDf,l135);
l135 = soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf);
soc_mem_field32_set(l19,l22,l20,CLASS_IDf,l135);l314[0] = soc_mem_field32_get
(l19,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf);l314[1] = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf);l314[2] = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf);l314[3] = 
soc_mem_field32_get(l19,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf);
soc_mem_field_set(l19,l22,(uint32*)l20,KEYf,(uint32*)l314);l133 = l131(l19,
lpm_entry,&l123);if(SOC_FAILURE(l133)){return l133;}if((l123 == 0)&&(l314[0]
== 0)&&(l314[1] == 0)&&(l314[2] == 0)&&(l314[3] == 0)){l24 = 1;}if(l315!= 
NULL){*l315 = l24;}soc_mem_field32_set(l19,l22,l20,LENGTHf,l123);if(l21 == 
NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(l19)){sal_memset(l21,0,
soc_mem_entry_words(l19,l22)*4);sal_memcpy(l21,l20,soc_mem_entry_words(l19,
l22)*4);soc_mem_field32_set(l19,l22,l21,DST_DISCARDf,0);soc_mem_field32_set(
l19,l22,l21,RPEf,0);soc_mem_field32_set(l19,l22,l21,SRC_DISCARDf,l23&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(l19,l22,l21,DEFAULTROUTEf,l24);
}return(SOC_E_NONE);}static int l25(int l19,void*l20,soc_mem_t l22,int l8,int
l26,int l27,int index,void*lpm_entry){uint32 l135;uint32 l314[4];uint32 l152,
l316;sal_memset(lpm_entry,0,soc_mem_entry_words(l19,L3_DEFIP_PAIR_128m)*4);
l135 = soc_mem_field32_get(l19,l22,l20,VALIDf);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf,l135);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,VALID1_LWRf,l135);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,VALID0_UPRf,l135);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,VALID1_UPRf,l135);l135 = soc_mem_field32_get(l19
,l22,l20,ECMPf);soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,ECMPf,
l135);l135 = soc_mem_field32_get(l19,l22,l20,ECMP_PTRf);soc_mem_field32_set(
l19,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf,l135);l135 = soc_mem_field32_get(
l19,l22,l20,NEXT_HOP_INDEXf);soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,
lpm_entry,NEXT_HOP_INDEXf,l135);l135 = soc_mem_field32_get(l19,l22,l20,PRIf);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,PRIf,l135);l135 = 
soc_mem_field32_get(l19,l22,l20,RPEf);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,RPEf,l135);l135 = soc_mem_field32_get(l19,l22,
l20,DST_DISCARDf);soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,
DST_DISCARDf,l135);l135 = soc_mem_field32_get(l19,l22,l20,SRC_DISCARDf);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,SRC_DISCARDf,l135);l135 = 
soc_mem_field32_get(l19,l22,l20,CLASS_IDf);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf,l135);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,ALG_BKT_PTRf,l27);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,ALG_HIT_IDXf,index);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,3);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,3);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,3);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,3);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,1);soc_mem_field32_set(l19
,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);soc_mem_field32_set(
l19,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,1);soc_mem_field32_set
(l19,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,1);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_LWRf,3);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_LWRf,3);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_UPRf,3);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_UPRf,3);
soc_mem_field_get(l19,l22,l20,KEYf,l314);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf,l314[0]);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf,l314[1]);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf,l314[2]);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf,l314[3]);l314[0] = l314[1] = l314[
2] = l314[3] = 0;l135 = soc_mem_field32_get(l19,l22,l20,LENGTHf);l141(l19,
lpm_entry,l135);if(l26 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_HIGHf,1);soc_mem_field32_set(l19,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_ROUTEf,1);l152 = 0;l316 = 0;}else if(l26
== SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,
GLOBAL_ROUTEf,1);l152 = 0;l316 = 0;}else{l152 = l26;l316 = SOC_VRF_MAX(l19);}
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_LWRf,l152);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_LWRf,l152);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_UPRf,l152);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_UPRf,l152);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_LWRf,l316);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_LWRf,l316);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_UPRf,l316);
soc_mem_field32_set(l19,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_UPRf,l316);
return(SOC_E_NONE);}int soc_alpm_128_warmboot_pivot_add(int l19,int l8,void*
lpm_entry,int l317,int l318){int l133 = SOC_E_NONE;uint32 key[4] = {0,0,0,0};
alpm_pivot_t*l178 = NULL;alpm_bucket_handle_t*l187 = NULL;int l152 = 0,l26 = 
0;uint32 l319;trie_t*l231 = NULL;uint32 prefix[5] = {0};int l24 = 0;l133 = 
l143(l19,lpm_entry,prefix,&l319,&l24);SOC_IF_ERROR_RETURN(l133);
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l19,lpm_entry,&l152,&l26));l317 = 
soc_alpm_physical_idx(l19,L3_DEFIP_PAIR_128m,l317,l8);l187 = sal_alloc(sizeof
(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l187 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l19,
"Unable to allocate memory for PIVOT trie node \n")));return SOC_E_NONE;}
sal_memset(l187,0,sizeof(*l187));l178 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l178 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l19,"Unable to allocate memory for PIVOT trie node \n")));sal_free(l187);
return SOC_E_MEMORY;}sal_memset(l178,0,sizeof(*l178));PIVOT_BUCKET_HANDLE(
l178) = l187;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l178));key[0] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l19,lpm_entry,IP_ADDR0_LWRf);key[1] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l19,lpm_entry,IP_ADDR1_LWRf);key[2] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l19,lpm_entry,IP_ADDR0_UPRf);key[3] = 
soc_L3_DEFIP_PAIR_128m_field32_get(l19,lpm_entry,IP_ADDR1_UPRf);
PIVOT_BUCKET_INDEX(l178) = l318;PIVOT_TCAM_INDEX(l178) = 
SOC_ALPM_128_ADDR_LWR(l317)<<1;if(l152!= SOC_L3_VRF_OVERRIDE){l231 = 
VRF_PIVOT_TRIE_IPV6_128(l19,l26);if(l231 == NULL){trie_init(_MAX_KEY_LEN_144_
,&VRF_PIVOT_TRIE_IPV6_128(l19,l26));l231 = VRF_PIVOT_TRIE_IPV6_128(l19,l26);}
sal_memcpy(l178->key,prefix,sizeof(prefix));l178->len = l319;l133 = 
trie_insert(l231,l178->key,NULL,l178->len,(trie_node_t*)l178);if(SOC_FAILURE(
l133)){sal_free(l187);sal_free(l178);return l133;}}ALPM_TCAM_PIVOT(l19,
SOC_ALPM_128_ADDR_LWR(l317)<<1) = l178;PIVOT_BUCKET_VRF(l178) = l26;
PIVOT_BUCKET_IPV6(l178) = l8;PIVOT_BUCKET_ENT_CNT_UPDATE(l178);if(key[0] == 0
&&key[1] == 0&&key[2] == 0&&key[3] == 0){PIVOT_BUCKET_DEF(l178) = TRUE;}
VRF_PIVOT_REF_INC(l19,l26,l8);return l133;}static int l320(int l19,int l8,
void*lpm_entry,void*l20,soc_mem_t l22,int l317,int l318,int l321){int l322;
int l26;int l133 = SOC_E_NONE;int l24 = 0;uint32 prefix[5] = {0,0,0,0,0};
uint32 l138;void*l323 = NULL;trie_t*l324 = NULL;trie_t*l181 = NULL;
trie_node_t*l183 = NULL;payload_t*l325 = NULL;payload_t*l185 = NULL;
alpm_pivot_t*l148 = NULL;if((NULL == lpm_entry)||(NULL == l20)){return
SOC_E_PARAM;}SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l19,lpm_entry,&l322
,&l26));l22 = L3_DEFIP_ALPM_IPV6_128m;l323 = sal_alloc(sizeof(
defip_pair_128_entry_t),"Temp Defip Pair lpm_entry");if(NULL == l323){return
SOC_E_MEMORY;}SOC_IF_ERROR_RETURN(l25(l19,l20,l22,l8,l322,l318,l317,l323));
l133 = l143(l19,l323,prefix,&l138,&l24);if(SOC_FAILURE(l133)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l19,"prefix create failed\n")));return l133;}
sal_free(l323);l317 = soc_alpm_physical_idx(l19,L3_DEFIP_PAIR_128m,l317,l8);
l148 = ALPM_TCAM_PIVOT(l19,SOC_ALPM_128_ADDR_LWR(l317)<<1);l324 = 
PIVOT_BUCKET_TRIE(l148);l325 = sal_alloc(sizeof(payload_t),"Payload for Key")
;if(NULL == l325){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l19,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l185 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l185){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l19,
"Unable to allocate memory for pfx trie node\n")));sal_free(l325);return
SOC_E_MEMORY;}sal_memset(l325,0,sizeof(*l325));sal_memset(l185,0,sizeof(*l185
));l325->key[0] = prefix[0];l325->key[1] = prefix[1];l325->key[2] = prefix[2]
;l325->key[3] = prefix[3];l325->key[4] = prefix[4];l325->len = l138;l325->
index = l321;sal_memcpy(l185,l325,sizeof(*l325));l133 = trie_insert(l324,
prefix,NULL,l138,(trie_node_t*)l325);if(SOC_FAILURE(l133)){goto l326;}if(l8){
l181 = VRF_PREFIX_TRIE_IPV6_128(l19,l26);}if(!l24){l133 = trie_insert(l181,
prefix,NULL,l138,(trie_node_t*)l185);if(SOC_FAILURE(l133)){goto l206;}}return
l133;l206:(void)trie_delete(l324,prefix,l138,&l183);l325 = (payload_t*)l183;
l326:sal_free(l325);sal_free(l185);return l133;}static int l327(int l19,int
l32,int l26,int l234,int bkt_ptr){int l133 = SOC_E_NONE;uint32 l138;uint32 key
[5] = {0,0,0,0,0};trie_t*l328 = NULL;payload_t*l218 = NULL;
defip_pair_128_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_pair_128_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l19,"unable to allocate memory for LPM entry\n"))
);return SOC_E_MEMORY;}(void)l28(l19,key,0,l26,l32,lpm_entry,0,1);if(l26 == 
SOC_VRF_MAX(l19)+1){soc_L3_DEFIP_PAIR_128m_field32_set(l19,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(l19,lpm_entry,
DEFAULT_MISSf,1);}soc_L3_DEFIP_PAIR_128m_field32_set(l19,lpm_entry,
ALG_BKT_PTRf,bkt_ptr);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l19,l26) = lpm_entry;
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(l19,l26));l328 = 
VRF_PREFIX_TRIE_IPV6_128(l19,l26);l218 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l218 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l19,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l218,0,sizeof(*l218));l138 = 0;l218->key[0] = key[0]
;l218->key[1] = key[1];l218->len = l138;l133 = trie_insert(l328,key,NULL,l138
,&(l218->node));if(SOC_FAILURE(l133)){sal_free(l218);return l133;}
VRF_TRIE_INIT_DONE(l19,l26,l32,1);return l133;}int
soc_alpm_128_warmboot_prefix_insert(int l19,int l8,void*lpm_entry,void*l20,
int l317,int l318,int l321){int l322;int l26;int l133 = SOC_E_NONE;soc_mem_t
l22;l22 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l19,lpm_entry,&l322,&l26));if(l322 == 
SOC_L3_VRF_OVERRIDE){return(l133);}if(!VRF_TRIE_INIT_COMPLETED(l19,l26,l8)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l19,"VRF %d is not initialized\n"),
l26));l133 = l327(l19,l8,l26,l317,l318);if(SOC_FAILURE(l133)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l19,"VRF %d/%d trie init \n""failed\n"),l26,l8));
return l133;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l19,
"VRF %d/%d trie init ""completed\n"),l26,l8));}l133 = l320(l19,l8,lpm_entry,
l20,l22,l317,l318,l321);if(l133!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l19,"unit %d : Route Insertion Failed :%s\n"),l19,soc_errmsg(l133)
));return(l133);}VRF_TRIE_ROUTES_INC(l19,l26,l8);return(l133);}int
soc_alpm_128_warmboot_bucket_bitmap_set(int l1,int l32,int l237){int l329 = 1
;if(l32){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l329 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l237,l329);return SOC_E_NONE;}int
soc_alpm_128_warmboot_lpm_reinit_done(int l19){int l234;int l330 = ((3*(128+2
+1))-1);int l310 = soc_mem_index_count(l19,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l19)){l310>>= 1;}if(!soc_alpm_mode_get(l19)){(l41[(l19)][
(((3*(128+2+1))-1))].l36) = -1;for(l234 = ((3*(128+2+1))-1);l234>-1;l234--){
if(-1 == (l41[(l19)][(l234)].l34)){continue;}(l41[(l19)][(l234)].l36) = l330;
(l41[(l19)][(l330)].next) = l234;(l41[(l19)][(l330)].l38) = (l41[(l19)][(l234
)].l34)-(l41[(l19)][(l330)].l35)-1;l330 = l234;}(l41[(l19)][(l330)].next) = -
1;(l41[(l19)][(l330)].l38) = l310-(l41[(l19)][(l330)].l35)-1;}else{(l41[(l19)
][(((3*(128+2+1))-1))].l36) = -1;for(l234 = ((3*(128+2+1))-1);l234>(((3*(128+
2+1))-1)/3);l234--){if(-1 == (l41[(l19)][(l234)].l34)){continue;}(l41[(l19)][
(l234)].l36) = l330;(l41[(l19)][(l330)].next) = l234;(l41[(l19)][(l330)].l38)
= (l41[(l19)][(l234)].l34)-(l41[(l19)][(l330)].l35)-1;l330 = l234;}(l41[(l19)
][(l330)].next) = -1;(l41[(l19)][(l330)].l38) = l310-(l41[(l19)][(l330)].l35)
-1;l330 = (((3*(128+2+1))-1)/3);(l41[(l19)][((((3*(128+2+1))-1)/3))].l36) = -
1;for(l234 = ((((3*(128+2+1))-1)/3)-1);l234>-1;l234--){if(-1 == (l41[(l19)][(
l234)].l34)){continue;}(l41[(l19)][(l234)].l36) = l330;(l41[(l19)][(l330)].
next) = l234;(l41[(l19)][(l330)].l38) = (l41[(l19)][(l234)].l34)-(l41[(l19)][
(l330)].l35)-1;l330 = l234;}(l41[(l19)][(l330)].next) = -1;(l41[(l19)][(l330)
].l38) = (l310>>1)-(l41[(l19)][(l330)].l35)-1;}return(SOC_E_NONE);}int
soc_alpm_128_warmboot_lpm_reinit(int l19,int l8,int l234,void*lpm_entry){int
l16;l268(l19,lpm_entry,l234,0x4000,0);SOC_IF_ERROR_RETURN(l306(l19,lpm_entry,
&l16));if((l41[(l19)][(l16)].l37) == 0){(l41[(l19)][(l16)].l34) = l234;(l41[(
l19)][(l16)].l35) = l234;}else{(l41[(l19)][(l16)].l35) = l234;}(l41[(l19)][(
l16)].l37)++;return(SOC_E_NONE);}int _soc_alpm_128_sanity_check(int l1,int
l331,int l332,int l333,int*l334){int l133 = SOC_E_NONE;int l335,l336,l337,
l321;int l243,l8;int l152,l26;int l338,l339,l340;int l341,l249,bkt_ptr;int
l342 = 0,l343 = 0,l344 = 0,l345 = 0;int l346;int l158,l347;int l348,l24;int
l349;uint32 l153 = 0;uint32 l14[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l165[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 prefix[5],l138;trie_t*l181 = NULL;
trie_node_t*l147 = NULL;payload_t*l186 = NULL;uint32 l350[10] = {0};uint32
l351[10] = {0};char*l352 = NULL;defip_pair_128_entry_t*lpm_entry,l353,l354;
soc_mem_t l355;soc_mem_t l356 = L3_DEFIP_ALPM_IPV6_128m;l349 = sizeof(
defip_pair_128_entry_t)*(l332-l331+1);l352 = soc_cm_salloc(l1,l349,"lpm_tbl")
;if(l352 == NULL){return(SOC_E_MEMORY);}sal_memset(l352,0,l349);
SOC_ALPM_LPM_LOCK(l1);if(soc_mem_read_range(l1,L3_DEFIP_PAIR_128m,
MEM_BLOCK_ANY,l331,l332,l352)<0){soc_cm_sfree(l1,l352);SOC_ALPM_LPM_UNLOCK(l1
);return(SOC_E_INTERNAL);}l8 = L3_DEFIP_MODE_128;l249 = 8;if(
SOC_ALPM_V6_SCALE_CHECK(l1,l8)){l249<<= 1;}for(l335 = l331;l335<= l332;l335++
){if(l342){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP_PAIR_128 index:%d check failed. error count %d\n"),l335-1,l342));
l344 = 1;}else if(l345){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP_PAIR_128 index:%d check passed. success count %d\n"),l335-1,l345)
);}l342 = l343 = l345 = 0;lpm_entry = soc_mem_table_idx_to_pointer(l1,
L3_DEFIP_PAIR_128m,defip_pair_128_entry_t*,l352,l335-l331);bkt_ptr = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l1)]->l89));if(!soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l1)]->l77))){continue;
}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l99[(l1)]->l87))){if(bkt_ptr!= 0){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tWrong bkt_ptr %d\n"),bkt_ptr));l342++;}continue;}if(bkt_ptr
== 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tWrong bkt_ptr2 %d\n"),
bkt_ptr));l342++;}l243 = 0;l350[l243++] = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l1)]->l45));l350[l243
++] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)
),(lpm_entry),(l99[(l1)]->l47));l350[l243++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l1)]->l72));l350[l243++] = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l1)]->l73));l350[
l243++] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l1)]->l74));l350[l243++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l1)]->l44));l350[l243++] = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l99[(l1)]->l43));bkt_ptr
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l1)]->l89));if(l334){if(l334[bkt_ptr] == -1){l334[bkt_ptr] = 
l335;if(SOC_ALPM_V6_SCALE_CHECK(l1,l8)){l334[bkt_ptr+1] = l335;}l345++;}else{
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tConflict bucket pointer %d: "
"was %d now %d\n"),bkt_ptr,l334[bkt_ptr],l335));l342++;}}l336 = 
soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l335,1);l133 = soc_mem_read(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l336),l165);l158 = -1
;if(SOC_SUCCESS(l133)){l158 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l165
,BPM_LENGTH0f);l347 = soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l165,
IP_LENGTH0f);if(l158>l347){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tAUX Table corrupted\n")));l342++;}else{l345++;}if(l158 == 0&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l99[(l1)]->l90))){continue;}sal_memcpy(&l354,lpm_entry,sizeof(
defip_pair_128_entry_t));l141(l1,&l354,l158);l338 = l339 = l340 = -1;l133 = 
l149(l1,&l354,l356,l14,&l338,&l339,&l340,FALSE);if(SOC_SUCCESS(l133)){l243 = 
0;l351[l243++] = soc_mem_field32_get(l1,l356,l14,ECMPf);l351[l243++] = 
soc_mem_field32_get(l1,l356,l14,ECMP_PTRf);l351[l243++] = soc_mem_field32_get
(l1,l356,l14,NEXT_HOP_INDEXf);l351[l243++] = soc_mem_field32_get(l1,l356,l14,
PRIf);l351[l243++] = soc_mem_field32_get(l1,l356,l14,RPEf);l351[l243++] = 
soc_mem_field32_get(l1,l356,l14,DST_DISCARDf);l351[l243++] = 
soc_mem_field32_get(l1,l356,l14,CLASS_IDf);if(sal_memcmp(l350,l351,sizeof(
l350))!= 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tData mismatch: "
"lpm_idx %d find_lpm_idx %d ""find_alpm_idx %d find_bkt_ptr %d\n"),l335,l338,
l340,l339));l342++;}else{l345++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"\t_soc_alpm_128_find rv=%d: ""lpm_idx %d find_lpm_idx %d "
"find_alpm_idx %d find_bkt_ptr %d\n"),l133,l335,l338,l340,l339));l342++;}}(
void)soc_alpm_128_lpm_vrf_get(l1,lpm_entry,&l152,&l26);for(l341 = 0;l341<l249
;l341++){(void)_soc_alpm_mem_index(l1,l356,bkt_ptr,l341,l153,&l321);l133 = 
soc_mem_read(l1,l356,MEM_BLOCK_ANY,l321,l14);if(SOC_FAILURE(l133)){continue;}
if(!soc_mem_field32_get(l1,l356,l14,VALIDf)){continue;}(void)l25(l1,l14,l356,
l8,l152,bkt_ptr,0,&l353);l337 = l338 = l339 = l340 = -1;l133 = l149(l1,&l353,
l356,l14,&l338,&l339,&l340,FALSE);if(SOC_SUCCESS(l133)){l337 = 
soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,SOC_ALPM_128_DEFIP_TO_PAIR(l338>>1
),1);}if(SOC_FAILURE(l133)||l339!= bkt_ptr||l337!= l335||
SOC_ALPM_128_DEFIP_TO_PAIR(l338>>1)!= l336||l340!= l321){l342++;l343++;
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\t_soc_alpm_128_find2 rv=%d: "
"fl [%d,%d] fa %d fb %d ""l [%d,%d] a %d b %d\n"),l133,
SOC_ALPM_128_DEFIP_TO_PAIR(l338>>1),l337,l340,l339,l336,l335,l321,bkt_ptr));}
}if(l343 == 0){l345++;}if(!l333){continue;}l133 = l273(l1,lpm_entry,l335);if(
SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tLPM Hash check failed: rv %d ""key_index %d lpm_idx %d\n"),l133,l346,l335)
);l342++;}else{l345++;}l133 = alpm_bucket_is_assigned(l1,bkt_ptr,l8,&l346);if
(SOC_FAILURE(l133)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tInvalid bucket pointer %d ""detected, in memory %s index %d\n"),bkt_ptr,
SOC_MEM_NAME(l1,L3_DEFIP_PAIR_128m),l335));l342++;}else if(l346 == 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),bkt_ptr,SOC_MEM_NAME(l1,
L3_DEFIP_PAIR_128m),l335));l342++;}else{l345++;}l355 = 
_soc_trident2_alpm_bkt_view_get(l1,bkt_ptr<<2);if(l356!= l355){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tMismatched alpm view "
"in bucket %d, expected %s, was %s\n"),bkt_ptr,SOC_MEM_NAME(l1,l356),
SOC_MEM_NAME(l1,l355)));l342++;}else{l345++;}l133 = l143(l1,lpm_entry,prefix,
&l138,&l24);if(SOC_SUCCESS(l133)){l181 = VRF_PREFIX_TRIE_IPV6_128(l1,l26);
l147 = NULL;if(l181){l133 = trie_find_lpm(l181,prefix,l138,&l147);}if(
SOC_SUCCESS(l133)&&l147){l186 = (payload_t*)l147;l348 = ((payload_t*)(l186->
bkt_ptr))->len;if(l348!= l158){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tBPM len mismatch: lpm_idx %d"" alpm_idx %d bpm_len %d trie_bpm_len %d\n"),
l335,l321,l158,l348));l342++;}else{l345++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\ttrie_find_lpm failed: ""lpm_idx %d alpm_idx %d lpmp %p\n"),
l335,l321,l147));l342++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tPrefix creating failed: ""lpm_idx %d alpm_idx %d\n"),l335,l321));l342++;}}
if(l342){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP_PAIR_128 index:%d check failed. error count %d\n"),l335-1,l342));
l344 = 1;}else if(l345){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP_PAIR_128 index:%d check passed. success count %d\n"),l335-1,l345)
);}SOC_ALPM_LPM_UNLOCK(l1);soc_cm_sfree(l1,l352);return(l344?SOC_E_FAIL:
SOC_E_NONE);}
#endif /* ALPM_ENABLE */
