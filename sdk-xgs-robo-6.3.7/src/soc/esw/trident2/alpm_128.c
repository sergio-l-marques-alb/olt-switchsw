/*
 * $Id: b418d9e65817875a831784a974af5daa20227acf $
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
 * File:    alpm_128.c
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
void soc_alpm_128_lpm_state_dump(int l1);static int l2(int l1,void*l3,int*
index);static int l4(int l1,void*l5);static int l6(int l1,void*l5,int l7,int
l8,int l9,defip_aux_scratch_entry_t*l10);static int l11(int l1,void*l5,void*
l12,int*l13,int*l14,int*l7);static int l15(int l1,void*l5,void*l12,int*l13);
static int l16(int l17,void*lpm_entry,void*l18,void*l19,soc_mem_t l20,uint32
l21,uint32*l22);static int l23(int l17,void*l18,soc_mem_t l20,int l7,int l24,
int l25,int index,void*lpm_entry);static int l26(int l17,uint32*key,int len,
int l24,int l7,defip_pair_128_entry_t*lpm_entry,int l27,int l28);static int
l29(int l1,int l24,int l30);typedef struct l31{int l32;int l33;int l34;int
next;int l35;int l36;}l37,*l38;static l38 l39[SOC_MAX_NUM_DEVICES];typedef
struct l40{soc_field_info_t*l41;soc_field_info_t*l42;soc_field_info_t*l43;
soc_field_info_t*l44;soc_field_info_t*l45;soc_field_info_t*l46;
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
soc_field_info_t*l83;soc_field_info_t*l84;}l85,*l86;static l86 l87[
SOC_MAX_NUM_DEVICES];typedef struct l88{int l17;int l89;int l90;uint16*l91;
uint16*l92;}l93;typedef uint32 l94[9];typedef int(*l95)(l94 l96,l94 l97);
static l93*l98[SOC_MAX_NUM_DEVICES];static void l99(int l1,void*l12,int index
,l94 l100);static uint16 l101(uint8*l102,int l103);static int l104(int l17,
int l89,int l90,l93**l105);static int l106(l93*l107);static int l108(l93*l109
,l95 l110,l94 entry,int l111,uint16*l112);static int l113(l93*l109,l95 l110,
l94 entry,int l111,uint16 l114,uint16 l115);static int l116(l93*l109,l95 l110
,l94 entry,int l111,uint16 l117);static int l118(int l1,const void*entry,int*
l111){int l119,l120;int l121[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};uint32 l122;l122 = soc_mem_field32_get
(l1,L3_DEFIP_PAIR_128m,entry,l121[0]);if((l120 = _ipmask2pfx(l122,l111))<0){
return(l120);}for(l119 = 1;l119<4;l119++){l122 = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,l121[l119]);if(*l111){if(l122!= 0xffffffff){return(
SOC_E_PARAM);}*l111+= 32;}else{if((l120 = _ipmask2pfx(l122,l111))<0){return(
l120);}}}return SOC_E_NONE;}static void l123(uint32*l124,int l125,int l30){
uint32 l126,l127,l32,prefix[5];int l119;sal_memcpy(prefix,l124,sizeof(uint32)
*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(l124,0,sizeof(uint32)*BITS2WORDS(
_MAX_KEY_LEN_144_));l126 = 128-l125;l32 = (l126+31)/32;if((l126%32) == 0){l32
++;}l126 = l126%32;for(l119 = l32;l119<= 4;l119++){prefix[l119]<<= l126;l127 = 
prefix[l119+1]&~(0xffffffff>>l126);l127 = (((32-l126) == 32)?0:(l127)>>(32-
l126));if(l119<4){prefix[l119]|= l127;}}for(l119 = l32;l119<= 4;l119++){l124[
3-(l119-l32)] = prefix[l119];}}static void l128(int l17,void*lpm_entry,int l14
){int l119;soc_field_t l129[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};for(l119 = 0;l119<4;l119++){
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,l129[l119],0);}for(l119 = 
0;l119<4;l119++){if(l14<= 32)break;soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m
,lpm_entry,l129[3-l119],0xffffffff);l14-= 32;}soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,l129[3-l119],~(((l14) == 32)?0:(0xffffffff)>>(
l14)));}static int l130(int l1,void*entry,uint32*l124,uint32*l14,int*l22){int
l119;int l111 = 0,l32;int l120 = SOC_E_NONE;uint32 l126,l127;uint32 prefix[5]
;sal_memset(l124,0,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(
prefix,0,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));prefix[0] = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,IP_ADDR0_LWRf);prefix[1] = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,IP_ADDR1_LWRf);prefix[2] = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,IP_ADDR0_UPRf);prefix[3] = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,IP_ADDR1_UPRf);if(l22!= NULL)
{*l22 = (prefix[0] == 0)&&(prefix[1] == 0)&&(prefix[2] == 0)&&(prefix[3] == 0
)&&(l111 == 0);}l120 = l118(l1,entry,&l111);if(SOC_FAILURE(l120)){return l120
;}l126 = 128-l111;l32 = l126/32;l126 = l126%32;for(l119 = l32;l119<4;l119++){
prefix[l119]>>= l126;l127 = prefix[l119+1]&((1<<l126)-1);l127 = (((32-l126) == 
32)?0:(l127)<<(32-l126));prefix[l119]|= l127;}for(l119 = l32;l119<4;l119++){
l124[4-(l119-l32)] = prefix[l119];}*l14 = l111;return SOC_E_NONE;}static int
l131(int l1,void*l5,soc_mem_t l20,void*l132,int*l133,int*bucket_index,int*l13
){defip_aux_scratch_entry_t l10;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l134,
l24,l30;int l112;uint32 l8,l135;int l120 = SOC_E_NONE;int l136 = 0;l30 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l5,&l134,&
l24));if(l134 == 0){if(soc_alpm_mode_get(l1)){return SOC_E_PARAM;}}if(l24 == 
SOC_VRF_MAX(l1)+1){l8 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l135);}else{l8 = 
2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l135);}if(l134!= SOC_L3_VRF_OVERRIDE){
sal_memset(&l10,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(
l1,l5,l30,l8,0,&l10));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&
l10,&l136,l133,bucket_index));if(l136){l16(l1,l5,l12,0,l20,0,0);l120 = 
_soc_alpm_find_in_bkt(l1,l20,*bucket_index,l135,l12,l132,&l112,l30);if(
SOC_SUCCESS(l120)){*l13 = l112;}}else{l120 = SOC_E_NOT_FOUND;}}return l120;}
static int l137(int l1,void*l5,void*l132,void*l138,soc_mem_t l20,int l112){
defip_aux_scratch_entry_t l10;int l134,l30,l24;int bucket_index;uint32 l8,
l135;int l120 = SOC_E_NONE;int l136 = 0,l127 = 0;int l133;l30 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l5,&l134,&
l24));if(l24 == SOC_VRF_MAX(l1)+1){l8 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1
,l135);}else{l8 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l135);}if(!
soc_alpm_mode_get(l1)){l8 = 2;}if(l134!= SOC_L3_VRF_OVERRIDE){sal_memset(&l10
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(l1,l5,l30,l8,0,&
l10));SOC_IF_ERROR_RETURN(soc_mem_write(l1,l20,MEM_BLOCK_ANY,l112,l132));if(
l120!= SOC_E_NONE){return SOC_E_FAIL;}if(SOC_URPF_STATUS_GET(l1)){
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1
,l112),l138));if(l120!= SOC_E_NONE){return SOC_E_FAIL;}}l127 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l10,IP_LENGTHf);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l10,REPLACE_LENf,l127);
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,&l136,&l133,&
bucket_index));if(SOC_URPF_STATUS_GET(l1)){l127 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf);l127+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf,l127);SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,&l136,&l133,&bucket_index));}}
return l120;}static int l139(int l1,int l140,int l141){int l120,l127,l142,
l143;defip_aux_table_entry_t l144,l145;l142 = SOC_ALPM_128_ADDR_LWR(l140);
l143 = SOC_ALPM_128_ADDR_UPR(l140);l120 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm
,MEM_BLOCK_ANY,l142,&l144);SOC_IF_ERROR_RETURN(l120);l120 = soc_mem_read(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l143,&l145);SOC_IF_ERROR_RETURN(l120);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l144,BPM_LENGTH0f,l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l144,BPM_LENGTH1f,l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l145,BPM_LENGTH0f,l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l145,BPM_LENGTH1f,l141);l127 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&l144,DB_TYPE0f);l120 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l142,&l144);
SOC_IF_ERROR_RETURN(l120);l120 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l143,&l145);SOC_IF_ERROR_RETURN(l120);if(SOC_URPF_STATUS_GET(l1
)){l127++;soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l144,BPM_LENGTH0f,l141)
;soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l144,BPM_LENGTH1f,l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l145,BPM_LENGTH0f,l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l145,BPM_LENGTH1f,l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l144,DB_TYPE0f,l127);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l144,DB_TYPE1f,l127);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l145,DB_TYPE0f,l127);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l145,DB_TYPE1f,l127);l142+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l143+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(
l1,L3_DEFIPm))/2;l120 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
l142,&l144);SOC_IF_ERROR_RETURN(l120);l120 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l143,&l145);}return l120;}static int l146(
int l1,int l147,void*entry,defip_aux_table_entry_t*l148,int l149){uint32 l127
,l8,l150 = 0;soc_mem_t l20 = L3_DEFIP_PAIR_128m;soc_mem_t l151 = 
L3_DEFIP_AUX_TABLEm;int l120 = SOC_E_NONE,l111,l24;void*l152,*l153;l152 = (
void*)l148;l153 = (void*)(l148+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l151,
MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l147),l148));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l151,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l147),l148+1));l127 = 
soc_mem_field32_get(l1,l20,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l151,
l152,VRF0f,l127);l127 = soc_mem_field32_get(l1,l20,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l151,l152,VRF1f,l127);l127 = soc_mem_field32_get(l1,
l20,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l151,l153,VRF0f,l127);l127 = 
soc_mem_field32_get(l1,l20,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l151,
l153,VRF1f,l127);l127 = soc_mem_field32_get(l1,l20,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l151,l152,MODE0f,l127);l127 = soc_mem_field32_get(l1,
l20,entry,MODE1_LWRf);soc_mem_field32_set(l1,l151,l152,MODE1f,l127);l127 = 
soc_mem_field32_get(l1,l20,entry,MODE0_UPRf);soc_mem_field32_set(l1,l151,l153
,MODE0f,l127);l127 = soc_mem_field32_get(l1,l20,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l151,l153,MODE1f,l127);l127 = soc_mem_field32_get(l1,
l20,entry,VALID0_LWRf);soc_mem_field32_set(l1,l151,l152,VALID0f,l127);l127 = 
soc_mem_field32_get(l1,l20,entry,VALID1_LWRf);soc_mem_field32_set(l1,l151,
l152,VALID1f,l127);l127 = soc_mem_field32_get(l1,l20,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l151,l153,VALID0f,l127);l127 = soc_mem_field32_get(l1,
l20,entry,VALID1_UPRf);soc_mem_field32_set(l1,l151,l153,VALID1f,l127);l120 = 
l118(l1,entry,&l111);SOC_IF_ERROR_RETURN(l120);soc_mem_field32_set(l1,l151,
l152,IP_LENGTH0f,l111);soc_mem_field32_set(l1,l151,l152,IP_LENGTH1f,l111);
soc_mem_field32_set(l1,l151,l153,IP_LENGTH0f,l111);soc_mem_field32_set(l1,
l151,l153,IP_LENGTH1f,l111);l127 = soc_mem_field32_get(l1,l20,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l151,l152,IP_ADDR0f,l127);l127 = 
soc_mem_field32_get(l1,l20,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l151,
l152,IP_ADDR1f,l127);l127 = soc_mem_field32_get(l1,l20,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l151,l153,IP_ADDR0f,l127);l127 = soc_mem_field32_get(
l1,l20,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l151,l153,IP_ADDR1f,l127);
l127 = soc_mem_field32_get(l1,l20,entry,ENTRY_TYPE0_LWRf);soc_mem_field32_set
(l1,l151,l152,ENTRY_TYPE0f,l127);l127 = soc_mem_field32_get(l1,l20,entry,
ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,l151,l152,ENTRY_TYPE1f,l127);l127 = 
soc_mem_field32_get(l1,l20,entry,ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,
l151,l153,ENTRY_TYPE0f,l127);l127 = soc_mem_field32_get(l1,l20,entry,
ENTRY_TYPE1_UPRf);soc_mem_field32_set(l1,l151,l153,ENTRY_TYPE1f,l127);l120 = 
soc_alpm_128_lpm_vrf_get(l1,entry,&l24,&l111);SOC_IF_ERROR_RETURN(l120);if(
SOC_URPF_STATUS_GET(l1)){if(l149>= (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m
)>>1)){l150 = 1;}}switch(l24){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l1
,l151,l152,VALID0f,0);soc_mem_field32_set(l1,l151,l152,VALID1f,0);
soc_mem_field32_set(l1,l151,l153,VALID0f,0);soc_mem_field32_set(l1,l151,l153,
VALID1f,0);l8 = 0;break;case SOC_L3_VRF_GLOBAL:l8 = l150?1:0;break;default:l8
= l150?3:2;break;}soc_mem_field32_set(l1,l151,l152,DB_TYPE0f,l8);
soc_mem_field32_set(l1,l151,l152,DB_TYPE1f,l8);soc_mem_field32_set(l1,l151,
l153,DB_TYPE0f,l8);soc_mem_field32_set(l1,l151,l153,DB_TYPE1f,l8);if(l150){
l127 = soc_mem_field32_get(l1,l20,entry,ALG_BKT_PTRf);if(l127){l127+= 
SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l20,entry,ALG_BKT_PTRf,l127)
;}}return SOC_E_NONE;}static int l154(int l1,int l155,int index,int l156,void
*entry){defip_aux_table_entry_t l148[2];l156 = soc_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l156,1);SOC_IF_ERROR_RETURN(l146(l1,l156,entry,&l148[0],
index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,
entry));index = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(index),l148));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(index),l148+
1));return SOC_E_NONE;}static int l157(int l1,soc_mem_t l20,
alpm_mem_prefix_array_t*l158,int*l115){int l119,l120 = SOC_E_NONE,l159;
defip_alpm_ipv6_128_entry_t l160,l161;void*l162 = NULL,*l163 = NULL;int l164,
l165;int*l166 = NULL;int l167 = FALSE;l164 = sizeof(l160);l165 = sizeof(l161)
;l162 = sal_alloc(l164*l158->count,"rb_bufp");if(l162 == NULL){l120 = 
SOC_E_MEMORY;goto l168;}l163 = sal_alloc(l165*l158->count,"rb_sip_bufp");if(
l163 == NULL){l120 = SOC_E_MEMORY;goto l168;}l166 = sal_alloc(sizeof(*l166)*
l158->count,"roll_back_index");if(l166 == NULL){l120 = SOC_E_MEMORY;goto l168
;}sal_memset(l166,-1,sizeof(*l166)*l158->count);for(l119 = 0;l119<l158->count
;l119++){payload_t*prefix = l158->prefix[l119];if(prefix->index>= 0){l120 = 
soc_mem_read(l1,l20,MEM_BLOCK_ANY,prefix->index,(uint8*)l162+l119*l164);if(
SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x 0x%08x"" 0x%08x 0x%08x\n",prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]);l119--;l167 = TRUE;break
;}if(SOC_URPF_STATUS_GET(l1)){l120 = soc_mem_read(l1,l20,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,prefix->index),(uint8*)l163+l119*l165);if(SOC_FAILURE(
l120)){soc_cm_debug(DK_ERR,"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x"" 0x%08x 0x%08x 0x%08x\n",prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]);l119--;l167 = TRUE;break
;}}l120 = soc_mem_write(l1,l20,MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null
(l1,l20));if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_split_inval: Failed to remove"
" prefixes from old bkt for pfx 0x%08x 0x%08x\n",prefix->key[0],prefix->key[1
]);l166[l119] = prefix->index;l167 = TRUE;break;}if(SOC_URPF_STATUS_GET(l1)){
l120 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,prefix->
index),soc_mem_entry_null(l1,l20));if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_split_inval: Failed to remove"
" prefixes from old urpf bkt for pfx 0x%08x"" 0x%08x\n",prefix->key[0],prefix
->key[1]);l166[l119] = prefix->index;l167 = TRUE;break;}}}l166[l119] = prefix
->index;prefix->index = l115[l119];}if(l167){for(;l119>= 0;l119--){payload_t*
prefix = l158->prefix[l119];prefix->index = l166[l119];if(l166[l119]<0){
continue;}l159 = soc_mem_write(l1,l20,MEM_BLOCK_ALL,l166[l119],(uint8*)l162+
l119*l164);if(SOC_FAILURE(l159)){break;}if(!SOC_URPF_STATUS_GET(l1)){continue
;}l159 = soc_mem_write(l1,l20,MEM_BLOCK_ALL,_soc_alpm_rpf_entry(l1,l166[l119]
),(uint8*)l163+l119*l165);if(SOC_FAILURE(l159)){break;}}}l168:if(l166){
sal_free(l166);l166 = NULL;}if(l163){sal_free(l163);l163 = NULL;}if(l162){
sal_free(l162);l162 = NULL;}return l120;}static int l169(int l1,void*l5,
soc_mem_t l20,void*l132,void*l138,int*l13,int bucket_index,int l133){
alpm_pivot_t*l170,*l171,*l172;defip_aux_scratch_entry_t l10;uint32 l12[
SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l173,l125;uint32 l174[5];int l30,
l24,l134;int l112;int l120 = SOC_E_NONE,l159;uint32 l8,l135,l141 = 0;int l136
=0;int l140;int l175 = 0;trie_t*trie,*l176;trie_node_t*l177,*l178 = NULL,*
l179 = NULL;payload_t*l180,*l181,*l182;defip_pair_128_entry_t lpm_entry;
alpm_bucket_handle_t*l183;int l119,l184 = -1,l22 = 0;alpm_mem_prefix_array_t
l158;defip_alpm_ipv6_128_entry_t l160,l161;void*l185,*l186;int*l115;l30 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l5,&l134,&
l24));if(l24 == SOC_VRF_MAX(l1)+1){l8 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1
,l135);}else{l8 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l135);}l20 = 
L3_DEFIP_ALPM_IPV6_128m;l185 = ((uint32*)&(l160));l186 = ((uint32*)&(l161));
sal_memset(&l10,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l6(
l1,l5,l30,l8,0,&l10));if(bucket_index == 0){SOC_IF_ERROR_RETURN(
_soc_alpm_aux_op(l1,PREFIX_LOOKUP,&l10,&l136,&l133,&bucket_index));if(l136 == 
0){soc_cm_debug(DK_ERR,"_soc_alpm_128_insert: "
" Could not find bucket to insert prefix\n");return SOC_E_NOT_FOUND;}}l120 = 
_soc_alpm_insert_in_bkt(l1,l20,bucket_index,l135,l132,l12,&l112,l30);if(l120
== SOC_E_NONE){*l13 = l112;if(SOC_URPF_STATUS_GET(l1)){l159 = soc_mem_write(
l1,l20,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l112),l138);if(SOC_FAILURE(l159))
{return l159;}}}if(l120 == SOC_E_FULL){l175 = 1;}l120 = l130(l1,l5,prefix,&
l125,&l22);if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: prefix create failed\n");return l120;}l170 = 
ALPM_TCAM_PIVOT(l1,l133);trie = PIVOT_BUCKET_TRIE(l170);l172 = l170;l180 = 
sal_alloc(sizeof(payload_t),"Payload for 128b Key");if(l180 == NULL){
soc_cm_debug(DK_ERR,"_soc_alpm_128_insert: Unable to allocate memory for "
"trie node \n");return SOC_E_MEMORY;}l181 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l181 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: Unable to allocate memory for ""pfx trie node \n");
sal_free(l180);return SOC_E_MEMORY;}sal_memset(l180,0,sizeof(*l180));
sal_memset(l181,0,sizeof(*l181));l180->key[0] = prefix[0];l180->key[1] = 
prefix[1];l180->key[2] = prefix[2];l180->key[3] = prefix[3];l180->key[4] = 
prefix[4];l180->len = l125;l180->index = l112;sal_memcpy(l181,l180,sizeof(*
l180));l181->bkt_ptr = l180;l120 = trie_insert(trie,prefix,NULL,l125,(
trie_node_t*)l180);if(SOC_FAILURE(l120)){goto l187;}l176 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l24);if(!l22){l120 = trie_insert(l176,prefix,NULL
,l125,(trie_node_t*)l181);}else{l179 = NULL;l120 = trie_find_lpm(l176,0,0,&
l179);l182 = (payload_t*)l179;if(SOC_SUCCESS(l120)){l182->bkt_ptr = l180;}}
l173 = l125;if(SOC_FAILURE(l120)){goto l188;}if(l175){l120 = 
alpm_bucket_assign(l1,&bucket_index,l30);if(SOC_FAILURE(l120)){soc_cm_debug(
DK_ERR,"_soc_alpm_128_insert: Unable to allocate""new bucket for split\n");
bucket_index = -1;goto l189;}l120 = trie_split(trie,_MAX_KEY_LEN_144_,FALSE,
l174,&l125,&l177,NULL,FALSE);if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: Could not split bucket"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n",prefix[1],prefix[2],prefix[3],
prefix[4]);goto l189;}l179 = NULL;l120 = trie_find_lpm(l176,l174,l125,&l179);
l182 = (payload_t*)l179;if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n",l1,l174[0],l174[1],l174[2
],l174[3],l174[4],l125);goto l189;}if(l182->index){if(l182->bkt_ptr == l180){
sal_memcpy(l185,l132,sizeof(defip_alpm_ipv6_128_entry_t));}else{l120 = 
soc_mem_read(l1,l20,MEM_BLOCK_ANY,((payload_t*)l182->bkt_ptr)->index,l185);}
if(SOC_FAILURE(l120)){goto l189;}l120 = l23(l1,l185,l20,l30,l134,bucket_index
,0,&lpm_entry);if(SOC_FAILURE(l120)){goto l189;}l141 = ((payload_t*)(l182->
bkt_ptr))->len;}else{l120 = soc_mem_read(l1,L3_DEFIP_PAIR_128m,MEM_BLOCK_ANY,
soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,SOC_ALPM_128_DEFIP_TO_PAIR(l133>>1
),1),&lpm_entry);}l123((l174),(l125),(l30));l26(l1,l174,l125,l24,l30,&
lpm_entry,0,0);soc_L3_DEFIP_PAIR_128m_field32_set(l1,&lpm_entry,ALG_BKT_PTRf,
bucket_index);l183 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM 128 Bucket Handle");if(l183 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: Unable to allocate "" memory for PIVOT trie node \n");
l120 = SOC_E_MEMORY;goto l189;}sal_memset(l183,0,sizeof(*l183));l170 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new 128b Pivot");if(l170 == NULL)
{soc_cm_debug(DK_ERR,"_soc_alpm_128_insert: Unable to allocate "
"memory for PIVOT trie node \n");l120 = SOC_E_MEMORY;goto l189;}sal_memset(
l170,0,sizeof(*l170));PIVOT_BUCKET_HANDLE(l170) = l183;l120 = trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l170));PIVOT_BUCKET_TRIE(l170)->trie = 
l177;PIVOT_BUCKET_INDEX(l170) = bucket_index;l170->key[0] = l174[0];l170->key
[1] = l174[1];l170->key[2] = l174[2];l170->key[3] = l174[3];l170->key[4] = 
l174[4];l170->len = l125;sal_memset(&l158,0,sizeof(l158));l120 = 
trie_traverse(PIVOT_BUCKET_TRIE(l170),alpm_mem_prefix_array_cb,&l158,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_128_alpm_insert: Bucket split failed"
"for prefix 0x%08x 0x%08x 0x%08x 0x%08x\n",prefix[1],prefix[2],prefix[3],
prefix[4]);goto l189;}l115 = sal_alloc(sizeof(*l115)*l158.count,
"Temp storage for location of prefixes in new 128b bucket");if(l115 == NULL){
l120 = SOC_E_MEMORY;goto l189;}sal_memset(l115,-1,sizeof(*l115)*l158.count);
for(l119 = 0;l119<l158.count;l119++){payload_t*prefix = l158.prefix[l119];if(
prefix->index>0){l120 = soc_mem_read(l1,l20,MEM_BLOCK_ANY,prefix->index,l185)
;if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,"_soc_128_alpm_insert: Failed to"
"read prefix 0x%08x 0x%08x 0x%08x 0x%08x for move\n",l158.prefix[l119]->key[1
],l158.prefix[l119]->key[2],l158.prefix[l119]->key[3],l158.prefix[l119]->key[
4]);goto l190;}if(SOC_URPF_STATUS_GET(l1)){l120 = soc_mem_read(l1,l20,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,prefix->index),l186);if(SOC_FAILURE(l120
)){soc_cm_debug(DK_ERR,"_soc_alpm_128_insert: Failed to"
"read rpf prefix 0x%08x 0x%08x 0x%08x 0x%08x for move\n",l158.prefix[l119]->
key[1],l158.prefix[l119]->key[2],l158.prefix[l119]->key[3],l158.prefix[l119]
->key[4]);goto l190;}}l120 = _soc_alpm_insert_in_bkt(l1,l20,bucket_index,l135
,l185,l12,&l112,l30);if(SOC_SUCCESS(l120)){if(SOC_URPF_STATUS_GET(l1)){l120 = 
soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l112),l186);}}}else
{l120 = _soc_alpm_insert_in_bkt(l1,l20,bucket_index,l135,l132,l12,&l112,l30);
if(SOC_SUCCESS(l120)){l184 = l119;*l13 = l112;if(SOC_URPF_STATUS_GET(l1)){
l120 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l112),l138);
}}}l115[l119] = l112;if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: Failed to"
"write prefix 0x%08x 0x%08x 0x%08x 0x%08x for move\n",l158.prefix[l119]->key[
1],l158.prefix[l119]->key[2],l158.prefix[l119]->key[3],l158.prefix[l119]->key
[4]);goto l190;}}l120 = l2(l1,&lpm_entry,&l140);if(SOC_FAILURE(l120)){
soc_cm_debug(DK_ERR,"_soc_alpm_128_insert: Unable to add new"
"pivot to tcam\n");goto l190;}l140 = soc_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l140,l30);l120 = l139(l1,l140,l141);if(SOC_FAILURE(l120)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Unable to init bpm_len "
"for index %d\n",l140);goto l191;}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
l140)<<1) = l170;PIVOT_TCAM_INDEX(l170) = SOC_ALPM_128_ADDR_LWR(l140)<<1;l120
= l157(l1,l20,&l158,l115);if(SOC_FAILURE(l120)){goto l191;}sal_free(l115);if(
l184 == -1){l120 = _soc_alpm_insert_in_bkt(l1,l20,PIVOT_BUCKET_HANDLE(l172)->
bucket_index,l135,l132,l12,&l112,l30);if(SOC_FAILURE(l120)){soc_cm_debug(
DK_ERR,"_soc_alpm_128_insert: Could not insert new "
"prefix into trie after split\n");goto l189;}if(SOC_URPF_STATUS_GET(l1)){l120
= soc_mem_write(l1,l20,MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l112),l138);}*l13
= l112;l180->index = l112;}}VRF_TRIE_ROUTES_INC(l1,l24,l30);if(l22){sal_free(
l181);}SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,&l136,&
l133,&bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE
,&l10,&l136,&l133,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l125 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf);l125+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf,l125);
SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,DELETE_PROPAGATE,&l10,&l136,&l133,&
bucket_index));SOC_IF_ERROR_RETURN(_soc_alpm_aux_op(l1,INSERT_PROPAGATE,&l10,
&l136,&l133,&bucket_index));}return l120;l191:l159 = l4(l1,&lpm_entry);if(
SOC_FAILURE(l159)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: Failure to free new prefix""at %d\n",
soc_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,l140,l30));}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l140)<<1) = NULL;l190:l171 = l172;for(l119 = 0;l119<
l158.count;l119++){payload_t*prefix = l158.prefix[l119];if(l115[l119]!= -1){
sal_memset(l185,0,sizeof(defip_alpm_ipv6_128_entry_t));l159 = soc_mem_write(
l1,l20,MEM_BLOCK_ANY,l115[l119],l185);_soc_trident2_alpm_bkt_view_set(l1,l115
[l119],INVALIDm);if(SOC_FAILURE(l159)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: mem write failure""in bkt move rollback\n");}if(
SOC_URPF_STATUS_GET(l1)){l159 = soc_mem_write(l1,l20,MEM_BLOCK_ANY,
_soc_alpm_rpf_entry(l1,l115[l119]),l185);_soc_trident2_alpm_bkt_view_set(l1,
_soc_alpm_rpf_entry(l1,l115[l119]),INVALIDm);if(SOC_FAILURE(l159)){
soc_cm_debug(DK_ERR,"_soc_alpm_128_insert: urpf mem write "
"failure in bkt move rollback\n");}}}l178 = NULL;l159 = trie_delete(
PIVOT_BUCKET_TRIE(l170),prefix->key,prefix->len,&l178);l180 = (payload_t*)
l178;if(SOC_FAILURE(l159)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: trie delete failure""in bkt move rollback\n");}if(
prefix->index>0){l159 = trie_insert(PIVOT_BUCKET_TRIE(l171),prefix->key,NULL,
prefix->len,(trie_node_t*)l180);if(SOC_FAILURE(l159)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: trie reinsert failure""in bkt move rollback\n");}}else
{if(l180!= NULL){sal_free(l180);}}}if(l184 == -1){l178 = NULL;l159 = 
trie_delete(PIVOT_BUCKET_TRIE(l171),prefix,l173,&l178);l180 = (payload_t*)
l178;if(SOC_FAILURE(l159)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n",prefix[0],prefix[1]);}if(l180!= 
NULL){sal_free(l180);}}l159 = alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l170)
,l30);if(SOC_FAILURE(l159)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: new bucket release ""failure: %d\n",PIVOT_BUCKET_INDEX
(l170));}trie_destroy(PIVOT_BUCKET_TRIE(l170));sal_free(l183);sal_free(l170);
sal_free(l115);l178 = NULL;l159 = trie_delete(l176,prefix,l173,&l178);l181 = 
(payload_t*)l178;if(SOC_FAILURE(l159)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n",prefix[0],prefix[1]);}if(l181){sal_free(l181)
;}return l120;l189:l178 = NULL;(void)trie_delete(l176,prefix,l173,&l178);l181
= (payload_t*)l178;if(bucket_index!= -1){(void)alpm_bucket_release(l1,
bucket_index,l30);}l188:l178 = NULL;(void)trie_delete(trie,prefix,l173,&l178)
;l180 = (payload_t*)l178;l187:if(l180!= NULL){sal_free(l180);}if(l181!= NULL)
{sal_free(l181);}return l120;}static int l26(int l17,uint32*key,int len,int
l24,int l7,defip_pair_128_entry_t*lpm_entry,int l27,int l28){uint32 l127;if(
l28){sal_memset(lpm_entry,0,sizeof(defip_pair_128_entry_t));}
soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,VRF_ID_0_LWRf,l24&
SOC_VRF_MAX(l17));soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
VRF_ID_1_LWRf,l24&SOC_VRF_MAX(l17));soc_L3_DEFIP_PAIR_128m_field32_set(l17,
lpm_entry,VRF_ID_0_UPRf,l24&SOC_VRF_MAX(l17));
soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,VRF_ID_1_UPRf,l24&
SOC_VRF_MAX(l17));if(l24 == (SOC_VRF_MAX(l17)+1)){l127 = 0;}else{l127 = 
SOC_VRF_MAX(l17);}soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
VRF_ID_MASK0_LWRf,l127);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
VRF_ID_MASK1_LWRf,l127);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
VRF_ID_MASK0_UPRf,l127);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
VRF_ID_MASK1_UPRf,l127);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
IP_ADDR0_LWRf,key[0]);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
IP_ADDR1_LWRf,key[1]);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
IP_ADDR0_UPRf,key[2]);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
IP_ADDR1_UPRf,key[3]);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
MODE0_LWRf,3);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,MODE1_LWRf,3);
soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,MODE0_UPRf,3);
soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,MODE1_UPRf,3);l128(l17,(void
*)lpm_entry,len);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,VALID0_LWRf
,1);soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,VALID1_LWRf,1);
soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,VALID0_UPRf,1);
soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,VALID1_UPRf,1);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,(1<<
soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf))-1);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,(1<<
soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf))-1);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,(1<<
soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf))-1);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,(1<<
soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf))-1);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,(1
<<soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf))-1);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,(1
<<soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_LWRf))-1);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,(1
<<soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf))-1);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,(1
<<soc_mem_field_length(l17,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_UPRf))-1);
return(SOC_E_NONE);}static int l192(int l1,void*l5,int bucket_index,int l133,
int l112){alpm_pivot_t*l170;defip_alpm_ipv6_128_entry_t l160,l193,l161;
defip_aux_scratch_entry_t l10;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t
l20;void*l185,*l194,*l186 = NULL;int l134;int l7;int l120 = SOC_E_NONE,l159 = 
SOC_E_NONE;uint32 l195[5],prefix[5];int l30,l24;uint32 l125;int l196;uint32 l8
,l135;int l136,l22 = 0;trie_t*trie,*l176;uint32 l197;defip_pair_128_entry_t
lpm_entry,*l198;payload_t*l180 = NULL,*l199 = NULL,*l182 = NULL;trie_node_t*
l178,*l179;l7 = l30 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l5,&l134,&l24));if(l134!= SOC_L3_VRF_OVERRIDE){if
(l24 == SOC_VRF_MAX(l1)+1){l8 = 0;SOC_ALPM_GET_GLOBAL_BANK_DISABLE(l1,l135);}
else{l8 = 2;SOC_ALPM_GET_VRF_BANK_DISABLE(l1,l135);}l120 = l130(l1,l5,prefix,
&l125,&l22);if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_delete: prefix create ""failed\n");return l120;}if(!
soc_alpm_mode_get(l1)){if(l134!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1
,l24,l30)>1){if(l22){soc_cm_debug(DK_ERR,"VRF %d: Cannot v6-128 delete "
"default route if other routes are present ""in this mode",l24);return
SOC_E_PARAM;}}}l8 = 2;}l20 = L3_DEFIP_ALPM_IPV6_128m;l185 = ((uint32*)&(l160)
);SOC_ALPM_LPM_LOCK(l1);if(bucket_index == 0){l120 = l131(l1,l5,l20,l185,&
l133,&bucket_index,&l112);}else{l120 = l16(l1,l5,l185,0,l20,0,0);}sal_memcpy(
&l193,l185,sizeof(l193));l194 = &l193;if(SOC_FAILURE(l120)){
SOC_ALPM_LPM_UNLOCK(l1);soc_cm_debug(DK_ERR,
"_soc_alpm_128_delete: Unable to find ""prefix for delete\n");return l120;}
l196 = bucket_index;l170 = ALPM_TCAM_PIVOT(l1,l133);trie = PIVOT_BUCKET_TRIE(
l170);l120 = trie_delete(trie,prefix,l125,&l178);l180 = (payload_t*)l178;if(
l120!= SOC_E_NONE){soc_cm_debug(DK_ERR,
"_soc_alpm_128_delete: Error prefix not ""present in trie \n");
SOC_ALPM_LPM_UNLOCK(l1);return l120;}l176 = VRF_PREFIX_TRIE_IPV6_128(l1,l24);
if(!l22){l120 = trie_delete(l176,prefix,l125,&l178);l199 = (payload_t*)l178;
if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n",prefix[1],prefix[2],prefix[3],
prefix[4]);goto l200;}l179 = NULL;l120 = trie_find_lpm(l176,prefix,l125,&l179
);l182 = (payload_t*)l179;if(SOC_SUCCESS(l120)){payload_t*l201 = (payload_t*)
(l182->bkt_ptr);if(l201!= NULL){l197 = l201->len;}else{l197 = 0;}}else{
soc_cm_debug(DK_ERR,"_soc_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n",prefix[1],
prefix[2],prefix[3],prefix[4]);goto l202;}sal_memcpy(l195,prefix,sizeof(
prefix));l123((l195),(l125),(l30));l120 = l26(l1,prefix,l197,l24,l7,&
lpm_entry,0,1);(void)l23(l1,l185,l20,l7,l134,bucket_index,0,&lpm_entry);(void
)l26(l1,l195,l125,l24,l7,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(
SOC_SUCCESS(l120)){l186 = ((uint32*)&(l161));l159 = soc_mem_read(l1,l20,
MEM_BLOCK_ANY,_soc_alpm_rpf_entry(l1,l112),l186);}}if((l197 == 0)&&
SOC_FAILURE(l159)){l198 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24);sal_memcpy(
&lpm_entry,l198,sizeof(lpm_entry));l120 = l26(l1,prefix,l197,l24,l7,&
lpm_entry,0,1);}if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n",prefix[1],
prefix[2],prefix[3],prefix[4]);goto l202;}l198 = &lpm_entry;}else{l179 = NULL
;l120 = trie_find_lpm(l176,prefix,l125,&l179);l182 = (payload_t*)l179;if(
SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n",l24);goto l200;}l182->bkt_ptr = 0;
l197 = 0;l198 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24);}l120 = l6(l1,l198,
l30,l8,l197,&l10);if(SOC_FAILURE(l120)){goto l202;}l120 = _soc_alpm_aux_op(l1
,DELETE_PROPAGATE,&l10,&l136,&l133,&bucket_index);if(SOC_FAILURE(l120)){goto
l202;}if(SOC_URPF_STATUS_GET(l1)){uint32 l127;if(l186!= NULL){l127 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf);l127++;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l10,DB_TYPEf,l127);l127 = 
soc_mem_field32_get(l1,l20,l186,SRC_DISCARDf);soc_mem_field32_set(l1,l20,&l10
,SRC_DISCARDf,l127);l127 = soc_mem_field32_get(l1,l20,l186,DEFAULTROUTEf);
soc_mem_field32_set(l1,l20,&l10,DEFAULTROUTEf,l127);l120 = _soc_alpm_aux_op(
l1,DELETE_PROPAGATE,&l10,&l136,&l133,&bucket_index);}if(SOC_FAILURE(l120)){
goto l202;}}sal_free(l180);if(!l22){sal_free(l199);}if((l170->len!= 0)&&(trie
->trie == NULL)){l26(l1,l170->key,l170->len,l24,l7,&lpm_entry,0,1);l120 = l4(
l1,&lpm_entry);if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n",l170->key[1],l170->key[2],l170
->key[3],l170->key[4]);}}l120 = _soc_alpm_delete_in_bkt(l1,l20,l196,l135,l194
,l12,&l112,l30);if(!SOC_SUCCESS(l120)){SOC_ALPM_LPM_UNLOCK(l1);l120 = 
SOC_E_FAIL;return l120;}if(SOC_URPF_STATUS_GET(l1)){l120 = 
soc_mem_alpm_delete(l1,l20,SOC_ALPM_RPF_BKT_IDX(l1,l196),MEM_BLOCK_ALL,l135,
l194,l12,&l136);if(!SOC_SUCCESS(l120)){SOC_ALPM_LPM_UNLOCK(l1);l120 = 
SOC_E_FAIL;return l120;}}if((l170->len!= 0)&&(trie->trie == NULL)){l120 = 
alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(l170),l30);if(SOC_FAILURE(l120)){
soc_cm_debug(DK_ERR,"_soc_alpm_128_delete: Unable to release"
"empty bucket: %d\n",PIVOT_BUCKET_INDEX(l170));}trie_destroy(
PIVOT_BUCKET_TRIE(l170));sal_free(PIVOT_BUCKET_HANDLE(l170));sal_free(l170);
_soc_trident2_alpm_bkt_view_set(l1,l196<<2,INVALIDm);if(
SOC_ALPM_V6_SCALE_CHECK(l1,l30)){_soc_trident2_alpm_bkt_view_set(l1,(l196+1)
<<2,INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l24,l30);if(VRF_TRIE_ROUTES_CNT(l1,
l24,l30) == 0){l120 = l29(l1,l24,l30);}SOC_ALPM_LPM_UNLOCK(l1);return l120;
l202:l159 = trie_insert(l176,prefix,NULL,l125,(trie_node_t*)l199);if(
SOC_FAILURE(l159)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n",prefix[1],prefix[2],
prefix[3],prefix[4]);}l200:l159 = trie_insert(trie,prefix,NULL,l125,(
trie_node_t*)l180);if(SOC_FAILURE(l159)){soc_cm_debug(DK_ERR,
"_soc_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n",prefix[1],prefix[2],
prefix[3],prefix[4]);}SOC_ALPM_LPM_UNLOCK(l1);return l120;}int
soc_alpm_128_init(int l1){int l120 = SOC_E_NONE;l120 = soc_alpm_128_lpm_init(
l1);SOC_IF_ERROR_RETURN(l120);return l120;}int soc_alpm_128_state_clear(int l1
){int l119,l120;for(l119 = 0;l119<= SOC_VRF_MAX(l1)+1;l119++){l120 = 
trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,l119),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l120)){trie_destroy(
VRF_PREFIX_TRIE_IPV6_128(l1,l119));}else{soc_cm_debug(DK_ERR,
"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n",l1,l119);return l120
;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l119)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l119));}}return SOC_E_NONE;}int
soc_alpm_128_deinit(int l1){soc_alpm_128_lpm_deinit(l1);SOC_IF_ERROR_RETURN(
soc_alpm_128_state_clear(l1));return SOC_E_NONE;}static int l203(int l1,int
l24,int l30){defip_pair_128_entry_t*lpm_entry,l204;int l205;int index;int l120
= SOC_E_NONE;uint32 key[5] = {0,0,0,0,0};uint32 l125;alpm_bucket_handle_t*
l183;alpm_pivot_t*l170;payload_t*l199;trie_t*l206;trie_init(_MAX_KEY_LEN_144_
,&VRF_PREFIX_TRIE_IPV6_128(l1,l24));l206 = VRF_PREFIX_TRIE_IPV6_128(l1,l24);
lpm_entry = sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(
lpm_entry == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_128_vrf_add: unable to allocate memory ""for IPv6-128 LPM entry\n")
;return SOC_E_MEMORY;}l26(l1,key,0,l24,l30,lpm_entry,0,1);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24) = lpm_entry;if(l24 == SOC_VRF_MAX(l1)
+1){soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,GLOBAL_ROUTEf,1);}else{
soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,DEFAULT_MISSf,1);}l120 = 
alpm_bucket_assign(l1,&l205,l30);soc_L3_DEFIP_PAIR_128m_field32_set(l1,
lpm_entry,ALG_BKT_PTRf,l205);sal_memcpy(&l204,lpm_entry,sizeof(l204));l120 = 
l2(l1,&l204,&index);l183 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l183 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_128_vrf_add: Unable to allocate memory ""for bucket handle \n");
return SOC_E_NONE;}sal_memset(l183,0,sizeof(*l183));l170 = sal_alloc(sizeof(
alpm_pivot_t),"Payload for Pivot");if(l170 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_128_vrf_add: Unable to allocate memory ""for PIVOT trie node \n");
sal_free(l183);return SOC_E_MEMORY;}l199 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l199 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_128_vrf_add: Unable to allocate memory ""for pfx trie node \n");
sal_free(l183);sal_free(l170);return SOC_E_MEMORY;}sal_memset(l170,0,sizeof(*
l170));sal_memset(l199,0,sizeof(*l199));l125 = 0;PIVOT_BUCKET_HANDLE(l170) = 
l183;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l170));PIVOT_BUCKET_INDEX
(l170) = l205;PIVOT_TCAM_INDEX(l170) = index;l170->key[0] = l199->key[0] = 
key[0];l170->key[1] = l199->key[1] = key[1];l170->key[2] = l199->key[2] = key
[2];l170->key[3] = l199->key[3] = key[3];l170->key[4] = l199->key[4] = key[4]
;l170->len = l199->len = l125;l120 = trie_insert(l206,key,NULL,l125,&(l199->
node));if(SOC_FAILURE(l120)){sal_free(l199);sal_free(l170);sal_free(l183);
return l120;}index = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,l30);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(index)<<1) = l170;VRF_TRIE_INIT_DONE
(l1,l24,l30,1);return l120;}static int l29(int l1,int l24,int l30){
defip_pair_128_entry_t*lpm_entry;int l205;int l207;int l120 = SOC_E_NONE;
uint32 key[2] = {0,0},l124[SOC_MAX_MEM_FIELD_WORDS];payload_t*l180;
alpm_pivot_t*l208;trie_node_t*l178;trie_t*l206;lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24);l205 = 
soc_L3_DEFIP_PAIR_128m_field32_get(l1,lpm_entry,ALG_BKT_PTRf);l120 = 
alpm_bucket_release(l1,l205,l30);_soc_trident2_alpm_bkt_view_set(l1,l205<<2,
INVALIDm);if(SOC_ALPM_V6_SCALE_CHECK(l1,l30)){_soc_trident2_alpm_bkt_view_set
(l1,(l205+1)<<2,INVALIDm);}l120 = l15(l1,lpm_entry,(void*)l124,&l207);if(
SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n",l24
,l30);l207 = -1;}l207 = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l207,l30)
;l208 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l207)<<1);l120 = l4(l1,
lpm_entry);if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"soc_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n",l24,l30);}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l24) = NULL;l206 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l24);VRF_PREFIX_TRIE_IPV6_128(l1,l24) = NULL;
VRF_TRIE_INIT_DONE(l1,l24,l30,0);l120 = trie_delete(l206,key,0,&l178);l180 = 
(payload_t*)l178;if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"Unable to delete internal default for 128b vrf "" %d/%d\n",l24,l30);}
sal_free(l180);(void)trie_destroy(l206);sal_free(PIVOT_BUCKET_HANDLE(l208));(
void)trie_destroy(PIVOT_BUCKET_TRIE(l208));sal_free(l208);return l120;}int
soc_alpm_128_insert(int l1,void*l3,uint32 l21,int l209,int l210){
defip_alpm_ipv6_128_entry_t l160,l161;soc_mem_t l20;void*l185,*l194;int l134,
l24;int index;int l7;int l120 = SOC_E_NONE;uint32 l22;l7 = L3_DEFIP_MODE_128;
l20 = L3_DEFIP_ALPM_IPV6_128m;l185 = ((uint32*)&(l160));l194 = ((uint32*)&(
l161));SOC_IF_ERROR_RETURN(l16(l1,l3,l185,l194,l20,l21,&l22));
SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l3,&l134,&l24));if(l134 == 
SOC_L3_VRF_OVERRIDE){l120 = l2(l1,l3,&index);return(l120);}else if(l24 == 0){
if(soc_alpm_mode_get(l1)){soc_cm_debug(DK_ERR,
"Unit %d, VRF=0 cannot be added in Parallel ""mode\n",l1);return SOC_E_PARAM;
}}if(l134!= SOC_L3_VRF_GLOBAL){if(!soc_alpm_mode_get(l1)){if(
VRF_TRIE_ROUTES_CNT(l1,l24,l7) == 0){if(!l22){soc_cm_debug(DK_ERR,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n",
l134);return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l24,l7)){
soc_cm_debug(DK_VERBOSE,"soc_alpm_128_insert:VRF %d is not ""initialized\n",
l24);l120 = l203(l1,l24,l7);if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"soc_alpm_128_insert:VRF %d/%d trie init \n""failed\n",l24,l7);return l120;}
soc_cm_debug(DK_VERBOSE,"soc_alpm_128_insert:VRF %d/%d trie init "
"completed\n",l24,l7);}if(l210&SOC_ALPM_LOOKUP_HIT){l120 = l137(l1,l3,l185,
l194,l20,l209);}else{if(l209 == -1){l209 = 0;}l120 = l169(l1,l3,l20,l185,l194
,&index,SOC_ALPM_BKT_ENTRY_TO_IDX(l209),l210);}if(l120!= SOC_E_NONE){
soc_cm_debug(DK_WARN,"unit %d :soc_alpm_128_insert: "
"Route Insertion Failed :%s\n",l1,soc_errmsg(l120));}return(l120);}int
soc_alpm_128_lookup(int l1,void*l5,void*l12,int*l13,int*l211){
defip_alpm_ipv6_128_entry_t l160;soc_mem_t l20;int bucket_index;int l133;void
*l185;int l134,l24;int l7 = 2,l111;int l120 = SOC_E_NONE;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l1,l5,&l134,&l24));l120 = l11(l1,l5,l12,l13,&l111,&
l7);if(SOC_SUCCESS(l120)){SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l12
,&l134,&l24));if(l134 == SOC_L3_VRF_OVERRIDE){return SOC_E_NONE;}}if(!
VRF_TRIE_INIT_COMPLETED(l1,l24,l7)){soc_cm_debug(DK_VERBOSE,
"soc_alpm_lookup:VRF %d is not ""initialized\n",l24);*l13 = 0;*l211 = 0;
return SOC_E_NOT_FOUND;}l20 = L3_DEFIP_ALPM_IPV6_128m;l185 = ((uint32*)&(l160
));SOC_ALPM_LPM_LOCK(l1);l120 = l131(l1,l5,l20,l185,&l133,&bucket_index,l13);
SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l120)){*l211 = l133;*l13 = 
bucket_index<<2;return l120;}l120 = l23(l1,l185,l20,l7,l134,bucket_index,*l13
,l12);*l211 = SOC_ALPM_LOOKUP_HIT|l133;return(l120);}int soc_alpm_128_delete(
int l1,void*l5,int l209,int l210){int l134,l24;int l7;int l120 = SOC_E_NONE;
l7 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,l5,&
l134,&l24));if(l134 == SOC_L3_VRF_OVERRIDE){l120 = l4(l1,l5);return(l120);}
else{if(!VRF_TRIE_INIT_COMPLETED(l1,l24,l7)){soc_cm_debug(DK_VERBOSE,
"soc_alpm_128_delete:VRF %d/%d is not ""initialized\n",l24,l7);return
SOC_E_NONE;}if(l209 == -1){l209 = 0;}l120 = l192(l1,l5,
SOC_ALPM_BKT_ENTRY_TO_IDX(l209),l210&~SOC_ALPM_LOOKUP_HIT,l209);}return(l120)
;}static void l99(int l1,void*l12,int index,l94 l100){l100[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l87[(l1)]->l50));l100[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l87[(l1)]->l48));l100[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l87[(l1)]->l54));l100[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l87[(l1)]->l52));l100[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l87[(l1)]->l51));l100[5] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l87[(l1)]->l49));l100[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l87[(l1)]->l55));l100[7] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l87[(l1)]->l53));if((!(
SOC_IS_HURRICANE(l1)))&&(((l87[(l1)]->l73)!= NULL))){int l212;(void)
soc_alpm_128_lpm_vrf_get(l1,l12,(int*)&l100[8],&l212);}else{l100[8] = 0;};}
static int l213(l94 l96,l94 l97){int l207;for(l207 = 0;l207<9;l207++){{if((
l96[l207])<(l97[l207])){return-1;}if((l96[l207])>(l97[l207])){return 1;}};}
return(0);}static void l214(int l1,void*l3,uint32 l215,uint32 l114,int l111){
l94 l216;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l87[(l1)]->l70))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l87[(l1)]->l69))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l87[(l1)]->l68))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l87[(l1)]->l67))){l216[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l87[(l1)]->l50));l216[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l87[(l1)]->l48));l216[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l87[(l1)]->l54));l216[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l87[(l1)]->l52));l216[4] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l87[(l1)]->l51));l216[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l87[(l1)]->l49));l216[6] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l3),(l87[(l1)]->l55));l216[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l87[(l1)]->l53));if((!(SOC_IS_HURRICANE(l1)))&&(((l87[(l1)]->l73)!= NULL)))
{int l212;(void)soc_alpm_128_lpm_vrf_get(l1,l3,(int*)&l216[8],&l212);}else{
l216[8] = 0;};l113((l98[(l1)]),l213,l216,l111,l114,l215);}}static void l217(
int l1,void*l5,uint32 l215){l94 l216;int l111 = -1;int l120;uint16 index;l216
[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)
),(l5),(l87[(l1)]->l50));l216[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5),(l87[(l1)]->l48));l216[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l87[(l1)]->l54));l216[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l87[(l1)]->l52));l216[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l87[(l1)]->l51));l216[5] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l87[(l1)]->l49));l216[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l87[(l1)]->l55));l216[7] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l87[(l1)]->l53));if((!(SOC_IS_HURRICANE(l1)))
&&(((l87[(l1)]->l73)!= NULL))){int l212;(void)soc_alpm_128_lpm_vrf_get(l1,l5,
(int*)&l216[8],&l212);}else{l216[8] = 0;};index = l215;l120 = l116((l98[(l1)]
),l213,l216,l111,index);if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"\ndel  index: H %d error %d\n",index,l120);}}static int l218(int l1,void*l5,
int l111,int*l112){l94 l216;int l120;uint16 index = (0xFFFF);l216[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l87[(l1)]->l50));l216[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l87[(l1)]->l48));l216[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l87[(l1)]->l54));l216[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l87[(l1)]->l52));l216[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l87[(l1)]->l51));l216[5] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l87[(l1)]->l49));l216[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l5)
,(l87[(l1)]->l55));l216[7] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l1,L3_DEFIP_PAIR_128m)),(l5),(l87[(l1)]->l53));if((!(SOC_IS_HURRICANE(l1)))
&&(((l87[(l1)]->l73)!= NULL))){int l212;(void)soc_alpm_128_lpm_vrf_get(l1,l5,
(int*)&l216[8],&l212);}else{l216[8] = 0;};l120 = l108((l98[(l1)]),l213,l216,
l111,&index);if(SOC_FAILURE(l120)){*l112 = 0xFFFFFFFF;return(l120);}*l112 = 
index;return(SOC_E_NONE);}static uint16 l101(uint8*l102,int l103){return(
_shr_crc16b(0,l102,l103));}static int l104(int l17,int l89,int l90,l93**l105)
{l93*l109;int index;if(l90>l89){return SOC_E_MEMORY;}l109 = sal_alloc(sizeof(
l93),"lpm_hash");if(l109 == NULL){return SOC_E_MEMORY;}sal_memset(l109,0,
sizeof(*l109));l109->l17 = l17;l109->l89 = l89;l109->l90 = l90;l109->l91 = 
sal_alloc(l109->l90*sizeof(*(l109->l91)),"hash_table");if(l109->l91 == NULL){
sal_free(l109);return SOC_E_MEMORY;}l109->l92 = sal_alloc(l109->l89*sizeof(*(
l109->l92)),"link_table");if(l109->l92 == NULL){sal_free(l109->l91);sal_free(
l109);return SOC_E_MEMORY;}for(index = 0;index<l109->l90;index++){l109->l91[
index] = (0xFFFF);}for(index = 0;index<l109->l89;index++){l109->l92[index] = 
(0xFFFF);}*l105 = l109;return SOC_E_NONE;}static int l106(l93*l107){if(l107!= 
NULL){sal_free(l107->l91);sal_free(l107->l92);sal_free(l107);}return
SOC_E_NONE;}static int l108(l93*l109,l95 l110,l94 entry,int l111,uint16*l112)
{int l1 = l109->l17;uint16 l219;uint16 index;l219 = l101((uint8*)entry,(32*9)
)%l109->l90;index = l109->l91[l219];;;while(index!= (0xFFFF)){uint32 l12[
SOC_MAX_MEM_FIELD_WORDS];l94 l100;int l220;l220 = index;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l220,l12));l99(l1,l12,index,l100);if
((*l110)(entry,l100) == 0){*l112 = index;;return(SOC_E_NONE);}index = l109->
l92[index&(0x3FFF)];;};return(SOC_E_NOT_FOUND);}static int l113(l93*l109,l95
l110,l94 entry,int l111,uint16 l114,uint16 l115){int l1 = l109->l17;uint16
l219;uint16 index;uint16 l221;l219 = l101((uint8*)entry,(32*9))%l109->l90;
index = l109->l91[l219];;;;l221 = (0xFFFF);if(l114!= (0xFFFF)){while(index!= 
(0xFFFF)){uint32 l12[SOC_MAX_MEM_FIELD_WORDS];l94 l100;int l220;l220 = index;
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l220,l12));l99(
l1,l12,index,l100);if((*l110)(entry,l100) == 0){if(l115!= index){;if(l221 == 
(0xFFFF)){l109->l91[l219] = l115;l109->l92[l115&(0x3FFF)] = l109->l92[index&(
0x3FFF)];l109->l92[index&(0x3FFF)] = (0xFFFF);}else{l109->l92[l221&(0x3FFF)] = 
l115;l109->l92[l115&(0x3FFF)] = l109->l92[index&(0x3FFF)];l109->l92[index&(
0x3FFF)] = (0xFFFF);}};return(SOC_E_NONE);}l221 = index;index = l109->l92[
index&(0x3FFF)];;}}l109->l92[l115&(0x3FFF)] = l109->l91[l219];l109->l91[l219]
= l115;return(SOC_E_NONE);}static int l116(l93*l109,l95 l110,l94 entry,int
l111,uint16 l117){uint16 l219;uint16 index;uint16 l221;l219 = l101((uint8*)
entry,(32*9))%l109->l90;index = l109->l91[l219];;;l221 = (0xFFFF);while(index
!= (0xFFFF)){if(l117 == index){;if(l221 == (0xFFFF)){l109->l91[l219] = l109->
l92[l117&(0x3FFF)];l109->l92[l117&(0x3FFF)] = (0xFFFF);}else{l109->l92[l221&(
0x3FFF)] = l109->l92[l117&(0x3FFF)];l109->l92[l117&(0x3FFF)] = (0xFFFF);}
return(SOC_E_NONE);}l221 = index;index = l109->l92[index&(0x3FFF)];;}return(
SOC_E_NOT_FOUND);}static int l222(int l1,void*l12){return(SOC_E_NONE);}void
soc_alpm_128_lpm_state_dump(int l1){int l119;int l223;l223 = ((3*(128+2+1))-1
);if(!soc_cm_debug_check(DK_L3|DK_SOCMEM|DK_VERBOSE)){return;}for(l119 = l223
;l119>= 0;l119--){if((l119!= ((3*(128+2+1))-1))&&((l39[(l1)][(l119)].l32) == 
-1)){continue;}soc_cm_debug(DK_L3|DK_SOCMEM|DK_VERBOSE,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n",l119,(l39[
(l1)][(l119)].l34),(l39[(l1)][(l119)].next),(l39[(l1)][(l119)].l32),(l39[(l1)
][(l119)].l33),(l39[(l1)][(l119)].l35),(l39[(l1)][(l119)].l36));}
COMPILER_REFERENCE(l222);}static int l224(int l1,int index,uint32*l12){int
l225;uint32 l226,l227,l228;uint32 l229;int l230;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l225 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}else if(SOC_IS_APOLLO(l1)){
l225 = (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l225 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}if(((l87[(l1)]->l42)!= NULL))
{soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
l12),(l87[(l1)]->l42),(0));}l226 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12),(l87[(l1)]->l54));l229 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l87[(l1)]->l55));l227 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l12),(l87[(l1)]->l52));l228 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l87[(l1)]->l53));l230 = ((!l226)&&(!l229)&&(!l227)&&(!l228))?1:0;l226 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l87[(l1)]->l69));l229 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l12),(l87[(l1)]->l67));l227 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l87[(l1)]->l68));l228 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l12),(l87[(l1)]->l68));if(l226&&l229&&l227&&l228){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l12
),(l87[(l1)]->l66),(l230));}return l154(l1,MEM_BLOCK_ANY,index+l225,index,l12
);}static int l231(int l1,int l232,int l233){uint32 l12[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l232,l12));l214(l1,l12,l233,0x4000,0);SOC_IF_ERROR_RETURN(l154(
l1,MEM_BLOCK_ANY,l233,l232,l12));SOC_IF_ERROR_RETURN(l224(l1,l233,l12));do{
int l234,l235;l234 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l232),1);
l235 = soc_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l233),1);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l235))<<1) = ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l234))<<1);ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((
l234))<<1) = NULL;}while(0);return(SOC_E_NONE);}static int l236(int l1,int
l111,int l7){int l232;int l233;l233 = (l39[(l1)][(l111)].l33)+1;l232 = (l39[(
l1)][(l111)].l32);if(l232!= l233){SOC_IF_ERROR_RETURN(l231(l1,l232,l233));}(
l39[(l1)][(l111)].l32)+= 1;(l39[(l1)][(l111)].l33)+= 1;return(SOC_E_NONE);}
static int l237(int l1,int l111,int l7){int l232;int l233;l233 = (l39[(l1)][(
l111)].l32)-1;if((l39[(l1)][(l111)].l35) == 0){(l39[(l1)][(l111)].l32) = l233
;(l39[(l1)][(l111)].l33) = l233-1;return(SOC_E_NONE);}l232 = (l39[(l1)][(l111
)].l33);SOC_IF_ERROR_RETURN(l231(l1,l232,l233));(l39[(l1)][(l111)].l32)-= 1;(
l39[(l1)][(l111)].l33)-= 1;return(SOC_E_NONE);}static int l238(int l1,int l111
,int l7,void*l12,int*l239){int l240;int l241;int l242;int l243;if((l39[(l1)][
(l111)].l35) == 0){l243 = ((3*(128+2+1))-1);if(soc_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL){if(l111<= (((3*(128+2+1))/3)-1)){l243 = (((3*(128+2+1
))/3)-1);}}while((l39[(l1)][(l243)].next)>l111){l243 = (l39[(l1)][(l243)].
next);}l241 = (l39[(l1)][(l243)].next);if(l241!= -1){(l39[(l1)][(l241)].l34) = 
l111;}(l39[(l1)][(l111)].next) = (l39[(l1)][(l243)].next);(l39[(l1)][(l111)].
l34) = l243;(l39[(l1)][(l243)].next) = l111;(l39[(l1)][(l111)].l36) = ((l39[(
l1)][(l243)].l36)+1)/2;(l39[(l1)][(l243)].l36)-= (l39[(l1)][(l111)].l36);(l39
[(l1)][(l111)].l32) = (l39[(l1)][(l243)].l33)+(l39[(l1)][(l243)].l36)+1;(l39[
(l1)][(l111)].l33) = (l39[(l1)][(l111)].l32)-1;(l39[(l1)][(l111)].l35) = 0;}
l242 = l111;while((l39[(l1)][(l242)].l36) == 0){l242 = (l39[(l1)][(l242)].
next);if(l242 == -1){l242 = l111;break;}}while((l39[(l1)][(l242)].l36) == 0){
l242 = (l39[(l1)][(l242)].l34);if(l242 == -1){if((l39[(l1)][(l111)].l35) == 0
){l240 = (l39[(l1)][(l111)].l34);l241 = (l39[(l1)][(l111)].next);if(-1!= l240
){(l39[(l1)][(l240)].next) = l241;}if(-1!= l241){(l39[(l1)][(l241)].l34) = 
l240;}}return(SOC_E_FULL);}}while(l242>l111){l241 = (l39[(l1)][(l242)].next);
SOC_IF_ERROR_RETURN(l237(l1,l241,l7));(l39[(l1)][(l242)].l36)-= 1;(l39[(l1)][
(l241)].l36)+= 1;l242 = l241;}while(l242<l111){SOC_IF_ERROR_RETURN(l236(l1,
l242,l7));(l39[(l1)][(l242)].l36)-= 1;l240 = (l39[(l1)][(l242)].l34);(l39[(l1
)][(l240)].l36)+= 1;l242 = l240;}(l39[(l1)][(l111)].l35)+= 1;(l39[(l1)][(l111
)].l36)-= 1;(l39[(l1)][(l111)].l33)+= 1;*l239 = (l39[(l1)][(l111)].l33);
sal_memcpy(l12,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(
l1,L3_DEFIP_PAIR_128m)*4);return(SOC_E_NONE);}static int l244(int l1,int l111
,int l7,void*l12,int l245){int l240;int l241;int l232;int l233;uint32 l246[
SOC_MAX_MEM_FIELD_WORDS];int l120;int l127;l232 = (l39[(l1)][(l111)].l33);
l233 = l245;(l39[(l1)][(l111)].l35)-= 1;(l39[(l1)][(l111)].l36)+= 1;(l39[(l1)
][(l111)].l33)-= 1;if(l233!= l232){if((l120 = READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l232,l246))<0){return l120;}l214(l1,l246,l233,0x4000,0);if((
l120 = l154(l1,MEM_BLOCK_ANY,l233,l232,l246))<0){return l120;}if((l120 = l224
(l1,l233,l246))<0){return l120;}}l127 = soc_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l233,1);l245 = soc_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m
,l232,1);ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l127)<<1) = ALPM_TCAM_PIVOT
(l1,SOC_ALPM_128_ADDR_LWR(l245)<<1);ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
l245)<<1) = NULL;sal_memcpy(l246,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),
soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);l214(l1,l246,l232,0x4000,0);if(
(l120 = l154(l1,MEM_BLOCK_ANY,l232,l232,l246))<0){return l120;}if((l120 = 
l224(l1,l232,l246))<0){return l120;}if((l39[(l1)][(l111)].l35) == 0){l240 = (
l39[(l1)][(l111)].l34);assert(l240!= -1);l241 = (l39[(l1)][(l111)].next);(l39
[(l1)][(l240)].next) = l241;(l39[(l1)][(l240)].l36)+= (l39[(l1)][(l111)].l36)
;(l39[(l1)][(l111)].l36) = 0;if(l241!= -1){(l39[(l1)][(l241)].l34) = l240;}(
l39[(l1)][(l111)].next) = -1;(l39[(l1)][(l111)].l34) = -1;(l39[(l1)][(l111)].
l32) = -1;(l39[(l1)][(l111)].l33) = -1;}return(l120);}int
soc_alpm_128_lpm_vrf_get(int l17,void*lpm_entry,int*l24,int*l247){int l134;if
(((l87[(l17)]->l77)!= NULL)){l134 = soc_L3_DEFIP_PAIR_128m_field32_get(l17,
lpm_entry,VRF_ID_0_LWRf);*l247 = l134;if(soc_L3_DEFIP_PAIR_128m_field32_get(
l17,lpm_entry,VRF_ID_MASK0_LWRf)){*l24 = l134;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l17,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l87[(l17)]->l79))){*l24 = SOC_L3_VRF_GLOBAL;*l247 = SOC_VRF_MAX(
l17)+1;}else{*l24 = SOC_L3_VRF_OVERRIDE;}}else{*l24 = SOC_L3_VRF_DEFAULT;}
return(SOC_E_NONE);}static int l248(int l1,void*entry,int*l14){int l111=0;int
l120;int l134;int l249;l120 = l118(l1,entry,&l111);if(l120<0){return l120;}
l111+= 0;SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l1,entry,&l134,&l120));
l249 = soc_alpm_mode_get(l1);switch(l134){case SOC_L3_VRF_GLOBAL:if(l249 == 
SOC_ALPM_MODE_PARALLEL){*l14 = l111+((3*(128+2+1))/3);}else{*l14 = l111;}
break;case SOC_L3_VRF_OVERRIDE:*l14 = l111+2*((3*(128+2+1))/3);break;default:
if(l249 == SOC_ALPM_MODE_PARALLEL){*l14 = l111;}else{*l14 = l111+((3*(128+2+1
))/3);}break;}return(SOC_E_NONE);}static int l11(int l1,void*l5,void*l12,int*
l13,int*l14,int*l7){int l120;int l112;int l111 = 0;*l7 = L3_DEFIP_MODE_128;
l248(l1,l5,&l111);*l14 = l111;if(l218(l1,l5,l111,&l112) == SOC_E_NONE){*l13 = 
l112;if((l120 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(*l7)?*l13:(*l13>>1)
,l12))<0){return l120;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}int
soc_alpm_128_lpm_init(int l1){int l223;int l119;int l250;int l251;if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}l223 = (3*(128+2
+1));l251 = sizeof(l37)*(l223);if((l39[(l1)]!= NULL)){SOC_IF_ERROR_RETURN(
soc_alpm_128_deinit(l1));}l87[l1] = sal_alloc(sizeof(l85),
"lpm_128_field_state");if(NULL == l87[l1]){return(SOC_E_MEMORY);}(l87[l1])->
l41 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l87[l1])->l42 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l87[l1])->l43 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l87[l1])->l44 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l87[l1])->l45 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l87[l1])->l46 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l87[l1])->l47 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l87[l1])->l50 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l87[l1])->l48 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l87[l1])->l51 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l87[l1])->l49 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l87[l1])->l54 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l87[l1])->
l52 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l87[l1
])->l55 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l87[l1])->l53 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l87[l1])->l58 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l87[l1])->l56 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l87[l1])->l59 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l87[l1])->l57 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l87[l1])->l62 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l87[l1])->l60 = soc_mem_fieldinfo_get(l1
,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l87[l1])->l63 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l87[l1])->l61 = soc_mem_fieldinfo_get
(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l87[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l87[l1])->l65 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l87[l1])->l66 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l87[l1])->l69 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l87[l1])->l67 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l87[l1])->l70 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l87[l1])->l68 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l87[l1])->l73 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l87[l1])->l71 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l87[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l87[l1])->l72 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l87[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l87[l1])->l75
= soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l87[l1])->
l78 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(l87[l1]
)->l76 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_UPRf);(l87[
l1])->l79 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l87[l1
])->l80 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l87[l1])
->l81 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l87[l1])->
l82 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l87[l1])->
l83 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf)
;(l87[l1])->l84 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
FLEX_CTR_POOL_NUMBERf);(l39[(l1)]) = sal_alloc(l251,"LPM 128 prefix info");if
(NULL == (l39[(l1)])){sal_free(l87[l1]);l87[l1] = NULL;return(SOC_E_MEMORY);}
SOC_ALPM_LPM_LOCK(l1);sal_memset((l39[(l1)]),0,l251);for(l119 = 0;l119<l223;
l119++){(l39[(l1)][(l119)].l32) = -1;(l39[(l1)][(l119)].l33) = -1;(l39[(l1)][
(l119)].l34) = -1;(l39[(l1)][(l119)].next) = -1;(l39[(l1)][(l119)].l35) = 0;(
l39[(l1)][(l119)].l36) = 0;}l250 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)
;if(SOC_URPF_STATUS_GET(l1)){l250>>= 1;}if(soc_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL){(l39[(l1)][(((3*(128+2+1))-1))].l33) = (l250>>1)-1;(
l39[(l1)][(((((3*(128+2+1))/3)-1)))].l36) = l250>>1;(l39[(l1)][((((3*(128+2+1
))-1)))].l36) = (l250-(l39[(l1)][(((((3*(128+2+1))/3)-1)))].l36));}else{(l39[
(l1)][((((3*(128+2+1))-1)))].l36) = l250;}if((l98[(l1)])!= NULL){if(l106((l98
[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(l98[(l1)]) = NULL
;}if(l104(l1,l250*2,l250,&(l98[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}int
soc_alpm_128_lpm_deinit(int l1){if(!soc_feature(l1,soc_feature_lpm_tcam)){
return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);if((l98[(l1)])!= NULL){l106((l98
[(l1)]));(l98[(l1)]) = NULL;}if((l39[(l1)]!= NULL)){sal_free(l87[l1]);l87[l1]
= NULL;sal_free((l39[(l1)]));(l39[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l1);
return(SOC_E_NONE);}static int l2(int l1,void*l3,int*l252){int l111;int index
;int l7;uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l120 = SOC_E_NONE;int l253 = 
0;sal_memcpy(l12,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),
soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);SOC_ALPM_LPM_LOCK(l1);l120 = 
l11(l1,l3,l12,&index,&l111,&l7);if(l120 == SOC_E_NOT_FOUND){l120 = l238(l1,
l111,l7,l12,&index);if(l120<0){SOC_ALPM_LPM_UNLOCK(l1);return(l120);}}else{
l253 = 1;}*l252 = index;if(l120 == SOC_E_NONE){soc_alpm_128_lpm_state_dump(l1
);soc_cm_debug(DK_L3|DK_SOCMEM,"\nsoc_alpm_128_lpm_insert: %d %d\n",index,
l111);if(!l253){l214(l1,l3,index,0x4000,0);}l120 = l154(l1,MEM_BLOCK_ANY,
index,index,l3);if(l120>= 0){l120 = l224(l1,index,l3);}}SOC_ALPM_LPM_UNLOCK(
l1);return(l120);}static int l4(int l1,void*l5){int l111;int index;int l7;
uint32 l12[SOC_MAX_MEM_FIELD_WORDS];int l120 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(
l1);l120 = l11(l1,l5,l12,&index,&l111,&l7);if(l120 == SOC_E_NONE){
soc_cm_debug(DK_L3|DK_SOCMEM,"\nsoc_alpm_lpm_delete: %d %d\n",index,l111);
l217(l1,l5,index);l120 = l244(l1,l111,l7,l12,index);}
soc_alpm_128_lpm_state_dump(l1);SOC_ALPM_LPM_UNLOCK(l1);return(l120);}static
int l15(int l1,void*l5,void*l12,int*l13){int l111;int l120;int l7;
SOC_ALPM_LPM_LOCK(l1);l120 = l11(l1,l5,l12,l13,&l111,&l7);SOC_ALPM_LPM_UNLOCK
(l1);return(l120);}static int l6(int l17,void*l5,int l7,int l8,int l9,
defip_aux_scratch_entry_t*l10){uint32 l122;uint32 l254[4] = {0,0,0,0};int l111
= 0;int l120 = SOC_E_NONE;l122 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,
l5,VALID0_LWRf);soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,VALIDf,l122
);l122 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,MODE0_LWRf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,MODEf,l122);l122 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,ENTRY_TYPE0_LWRf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,ENTRY_TYPEf,0);l122 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,GLOBAL_ROUTEf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,GLOBAL_ROUTEf,l122);l122 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,ECMPf);soc_mem_field32_set(l17,
L3_DEFIP_AUX_SCRATCHm,l10,ECMPf,l122);l122 = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,ECMP_PTRf);soc_mem_field32_set(l17,
L3_DEFIP_AUX_SCRATCHm,l10,ECMP_PTRf,l122);l122 = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,NEXT_HOP_INDEXf);soc_mem_field32_set(l17,
L3_DEFIP_AUX_SCRATCHm,l10,NEXT_HOP_INDEXf,l122);l122 = soc_mem_field32_get(
l17,L3_DEFIP_PAIR_128m,l5,PRIf);soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm
,l10,PRIf,l122);l122 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,RPEf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,RPEf,l122);l122 =
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,VRF_ID_0_LWRf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,VRFf,l122);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,DB_TYPEf,l8);l122 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,DST_DISCARDf);
soc_mem_field32_set(l17,L3_DEFIP_AUX_SCRATCHm,l10,DST_DISCARDf,l122);l122 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,l5,CLASS_IDf);soc_mem_field32_set(
l17,L3_DEFIP_AUX_SCRATCHm,l10,CLASS_IDf,l122);l254[0] = soc_mem_field32_get(
l17,L3_DEFIP_PAIR_128m,l5,IP_ADDR0_LWRf);l254[1] = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,IP_ADDR1_LWRf);l254[2] = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,IP_ADDR0_UPRf);l254[3] = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,l5,IP_ADDR1_UPRf);soc_mem_field_set(l17,
L3_DEFIP_AUX_SCRATCHm,(uint32*)l10,IP_ADDRf,(uint32*)l254);l120 = l118(l17,l5
,&l111);if(SOC_FAILURE(l120)){return l120;}soc_mem_field32_set(l17,
L3_DEFIP_AUX_SCRATCHm,l10,IP_LENGTHf,l111);soc_mem_field32_set(l17,
L3_DEFIP_AUX_SCRATCHm,l10,REPLACE_LENf,l9);return(SOC_E_NONE);}static int l16
(int l17,void*lpm_entry,void*l18,void*l19,soc_mem_t l20,uint32 l21,uint32*
l255){uint32 l122;uint32 l254[4];int l111 = 0;int l120 = SOC_E_NONE;uint32 l22
= 0;sal_memset(l18,0,soc_mem_entry_bytes(l17,l20));l122 = soc_mem_field32_get
(l17,L3_DEFIP_PAIR_128m,lpm_entry,HITf);soc_mem_field32_set(l17,l20,l18,HITf,
l122);l122 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf
);soc_mem_field32_set(l17,l20,l18,VALIDf,l122);l122 = soc_mem_field32_get(l17
,L3_DEFIP_PAIR_128m,lpm_entry,ECMPf);soc_mem_field32_set(l17,l20,l18,ECMPf,
l122);l122 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf);
soc_mem_field32_set(l17,l20,l18,ECMP_PTRf,l122);l122 = soc_mem_field32_get(
l17,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf);soc_mem_field32_set(l17,l20
,l18,NEXT_HOP_INDEXf,l122);l122 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,
lpm_entry,PRIf);soc_mem_field32_set(l17,l20,l18,PRIf,l122);l122 = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,RPEf);
soc_mem_field32_set(l17,l20,l18,RPEf,l122);l122 = soc_mem_field32_get(l17,
L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf);soc_mem_field32_set(l17,l20,l18,
DST_DISCARDf,l122);l122 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,
lpm_entry,SRC_DISCARDf);soc_mem_field32_set(l17,l20,l18,SRC_DISCARDf,l122);
l122 = soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf);
soc_mem_field32_set(l17,l20,l18,CLASS_IDf,l122);l254[0] = soc_mem_field32_get
(l17,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf);l254[1] = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf);l254[2] = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf);l254[3] = 
soc_mem_field32_get(l17,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf);
soc_mem_field_set(l17,l20,(uint32*)l18,KEYf,(uint32*)l254);l120 = l118(l17,
lpm_entry,&l111);if(SOC_FAILURE(l120)){return l120;}if((l111 == 0)&&(l254[0]
== 0)&&(l254[1] == 0)&&(l254[2] == 0)&&(l254[3] == 0)){l22 = 1;}if(l255!= 
NULL){*l255 = l22;}soc_mem_field32_set(l17,l20,l18,LENGTHf,l111);if(l19 == 
NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(l17)){sal_memset(l19,0,
soc_mem_entry_bytes(l17,l20));sal_memcpy(l19,l18,soc_mem_entry_bytes(l17,l20)
);soc_mem_field32_set(l17,l20,l19,DST_DISCARDf,0);soc_mem_field32_set(l17,l20
,l19,RPEf,0);soc_mem_field32_set(l17,l20,l19,SRC_DISCARDf,l21&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(l17,l20,l19,DEFAULTROUTEf,l22);
}return(SOC_E_NONE);}static int l23(int l17,void*l18,soc_mem_t l20,int l7,int
l24,int l25,int index,void*lpm_entry){uint32 l122;uint32 l254[4];uint32 l134,
l256;sal_memset(lpm_entry,0,soc_mem_entry_bytes(l17,L3_DEFIP_PAIR_128m));l122
= soc_mem_field32_get(l17,l20,l18,HITf);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,HITf,l122);l122 = soc_mem_field32_get(l17,l20,
l18,VALIDf);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf,
l122);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_LWRf,l122);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_UPRf,l122);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_UPRf,l122);l122 = 
soc_mem_field32_get(l17,l20,l18,ECMPf);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,ECMPf,l122);l122 = soc_mem_field32_get(l17,l20,
l18,ECMP_PTRf);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf
,l122);l122 = soc_mem_field32_get(l17,l20,l18,NEXT_HOP_INDEXf);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf,l122);
l122 = soc_mem_field32_get(l17,l20,l18,PRIf);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,PRIf,l122);l122 = soc_mem_field32_get(l17,l20,
l18,RPEf);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,RPEf,l122);
l122 = soc_mem_field32_get(l17,l20,l18,DST_DISCARDf);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf,l122);l122 = soc_mem_field32_get(
l17,l20,l18,SRC_DISCARDf);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,
lpm_entry,SRC_DISCARDf,l122);l122 = soc_mem_field32_get(l17,l20,l18,CLASS_IDf
);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf,l122);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ALG_BKT_PTRf,l25);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ALG_HIT_IDXf,index);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,1)
;soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,1
);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,
1);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf
,1);soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_LWRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_LWRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_UPRf,3);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_UPRf,3);
soc_mem_field_get(l17,l20,l18,KEYf,l254);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf,l254[0]);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf,l254[1]);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf,l254[2]);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf,l254[3]);l254[0] = l254[1] = l254[
2] = l254[3] = 0;l122 = soc_mem_field32_get(l17,l20,l18,LENGTHf);l128(l17,
lpm_entry,l122);if(l24 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_HIGHf,1);soc_mem_field32_set(l17,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_ROUTEf,1);l134 = 0;l256 = 0;}else if(l24
== SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,
GLOBAL_ROUTEf,1);l134 = 0;l256 = 0;}else{l134 = l24;l256 = SOC_VRF_MAX(l17);}
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_LWRf,l134);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_LWRf,l134);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_UPRf,l134);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_UPRf,l134);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_LWRf,l256);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_LWRf,l256);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_UPRf,l256);
soc_mem_field32_set(l17,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_UPRf,l256);
return(SOC_E_NONE);}int soc_alpm_128_warmboot_pivot_add(int l17,int l7,void*
lpm_entry,int l257,int l258){int l120 = SOC_E_NONE;int l125;uint32 key[4] = {
0,0,0,0};alpm_pivot_t*l174 = NULL;alpm_bucket_handle_t*l183 = NULL;l257 = 
soc_alpm_physical_idx(l17,L3_DEFIP_PAIR_128m,l257,l7);l118(l17,lpm_entry,&
l125);l183 = sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(
l183 == NULL){soc_cm_debug(DK_ERR,"Unable to allocate memory for "
"PIVOT trie node \n");return SOC_E_NONE;}sal_memset(l183,0,sizeof(*l183));
l174 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l174 == NULL){
soc_cm_debug(DK_ERR,"Unable to allocate memory for ""PIVOT trie node \n");
sal_free(l183);return SOC_E_MEMORY;}sal_memset(l174,0,sizeof(*l174));
PIVOT_BUCKET_HANDLE(l174) = l183;trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l174));key[0] = soc_L3_DEFIP_PAIR_128m_field32_get(l17,
lpm_entry,IP_ADDR0_LWRf);key[1] = soc_L3_DEFIP_PAIR_128m_field32_get(l17,
lpm_entry,IP_ADDR1_LWRf);key[2] = soc_L3_DEFIP_PAIR_128m_field32_get(l17,
lpm_entry,IP_ADDR0_UPRf);key[3] = soc_L3_DEFIP_PAIR_128m_field32_get(l17,
lpm_entry,IP_ADDR1_UPRf);PIVOT_BUCKET_INDEX(l174) = l258;PIVOT_TCAM_INDEX(
l174) = l257;l174->key[0] = key[0];l174->key[1] = key[1];l174->key[2] = key[2
];l174->key[3] = key[3];l174->len = l125;ALPM_TCAM_PIVOT(l17,
SOC_ALPM_128_ADDR_LWR(l257)<<1) = l174;return l120;}static int l259(int l17,
int l7,void*lpm_entry,void*l18,soc_mem_t l20,int l257,int l258,int l260){int
l261;int l24;int l120 = SOC_E_NONE;int l22 = 0;uint32 prefix[5] = {0,0,0,0,0}
;uint32 l125;void*l262 = NULL;trie_t*l263 = NULL;trie_t*l176 = NULL;
trie_node_t*l178 = NULL;payload_t*l264 = NULL;payload_t*l181 = NULL;
alpm_pivot_t*l170 = NULL;if((NULL == lpm_entry)||(NULL == l18)){return
SOC_E_PARAM;}SOC_IF_ERROR_RETURN(soc_alpm_128_lpm_vrf_get(l17,lpm_entry,&l261
,&l24));l20 = L3_DEFIP_ALPM_IPV6_128m;l262 = sal_alloc(sizeof(
defip_pair_128_entry_t),"Temp Defip Pair lpm_entry");if(NULL == l262){return
SOC_E_MEMORY;}SOC_IF_ERROR_RETURN(l23(l17,l18,l20,l7,l261,l258,l257,l262));
l120 = l130(l17,l262,prefix,&l125,&l22);if(SOC_FAILURE(l120)){soc_cm_debug(
DK_ERR,"prefix create failed\n");return l120;}sal_free(l262);l257 = 
soc_alpm_physical_idx(l17,L3_DEFIP_PAIR_128m,l257,l7);l170 = ALPM_TCAM_PIVOT(
l17,SOC_ALPM_128_ADDR_LWR(l257)<<1);l263 = PIVOT_BUCKET_TRIE(l170);l264 = 
sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == l264){soc_cm_debug(
DK_ERR,"Unable to allocate memory for ""trie node.\n");return SOC_E_MEMORY;}
l181 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == 
l181){soc_cm_debug(DK_ERR,"Unable to allocate memory for ""pfx trie node\n");
sal_free(l264);return SOC_E_MEMORY;}sal_memset(l264,0,sizeof(*l264));
sal_memset(l181,0,sizeof(*l181));l264->key[0] = prefix[0];l264->key[1] = 
prefix[1];l264->key[2] = prefix[2];l264->key[3] = prefix[3];l264->key[4] = 
prefix[4];l264->len = l125;l264->index = l260;sal_memcpy(l181,l264,sizeof(*
l264));l120 = trie_insert(l263,prefix,NULL,l125,(trie_node_t*)l264);if(
SOC_FAILURE(l120)){goto l265;}if(l7){l176 = VRF_PREFIX_TRIE_IPV6_128(l17,l24)
;}if(!l22){l120 = trie_insert(l176,prefix,NULL,l125,(trie_node_t*)l181);if(
SOC_FAILURE(l120)){goto l188;}}return l120;l188:(void)trie_delete(l263,prefix
,l125,&l178);l264 = (payload_t*)l178;l265:sal_free(l264);sal_free(l181);
return l120;}static int l266(int l17,int l30,int l24,int l207,int bkt_ptr){
int l120 = SOC_E_NONE;uint32 l125;uint32 key[5] = {0,0,0,0,0};trie_t*l267 = 
NULL;payload_t*l199 = NULL;defip_pair_128_entry_t*lpm_entry = NULL;lpm_entry = 
sal_alloc(sizeof(defip_pair_128_entry_t),"Default LPM entry");if(lpm_entry == 
NULL){soc_cm_debug(DK_ERR,"unable to allocate memory for ""LPM entry\n");
return SOC_E_MEMORY;}l26(l17,key,0,l24,l30,lpm_entry,0,1);if(l24 == 
SOC_VRF_MAX(l17)+1){soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
DEFAULT_MISSf,1);}soc_L3_DEFIP_PAIR_128m_field32_set(l17,lpm_entry,
ALG_BKT_PTRf,bkt_ptr);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l17,l24) = lpm_entry;
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(l17,l24));l267 = 
VRF_PREFIX_TRIE_IPV6_128(l17,l24);l199 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l199 == NULL){soc_cm_debug(DK_ERR,
"Unable to allocate memory for ""pfx trie node \n");return SOC_E_MEMORY;}
sal_memset(l199,0,sizeof(*l199));l125 = 0;l199->key[0] = key[0];l199->key[1] = 
key[1];l199->len = l125;l120 = trie_insert(l267,key,NULL,l125,&(l199->node));
if(SOC_FAILURE(l120)){sal_free(l199);return l120;}VRF_TRIE_INIT_DONE(l17,l24,
l30,1);return l120;}int soc_alpm_128_warmboot_prefix_insert(int l17,int l7,
void*lpm_entry,void*l18,int l257,int l258,int l260){int l261;int l24;int l120
= SOC_E_NONE;soc_mem_t l20;l20 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(
soc_alpm_128_lpm_vrf_get(l17,lpm_entry,&l261,&l24));if(l261 == 
SOC_L3_VRF_OVERRIDE){return(l120);}if(!VRF_TRIE_INIT_COMPLETED(l17,l24,l7)){
soc_cm_debug(DK_VERBOSE,"VRF %d is not ""initialized\n",l24);l120 = l266(l17,
l7,l24,l257,l258);if(SOC_FAILURE(l120)){soc_cm_debug(DK_ERR,
"VRF %d/%d trie init \n""failed\n",l24,l7);return l120;}soc_cm_debug(
DK_VERBOSE,"VRF %d/%d trie init ""completed\n",l24,l7);}l120 = l259(l17,l7,
lpm_entry,l18,l20,l257,l258,l260);if(l120!= SOC_E_NONE){soc_cm_debug(DK_WARN,
"unit %d : ""Route Insertion Failed :%s\n",l17,soc_errmsg(l120));return(l120)
;}VRF_TRIE_ROUTES_INC(l17,l24,l7);return(l120);}int
soc_alpm_128_warmboot_bucket_bitmap_set(int l1,int l30,int l209){int l268 = 1
;if(l30){if(!soc_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)){l268 = 2;}}if(
SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}SHR_BITSET_RANGE(
SOC_ALPM_BUCKET_BMAP(l1),l209,l268);return SOC_E_NONE;}int
soc_alpm_128_warmboot_lpm_reinit_done(int l17){int l207;int l269 = ((3*(128+2
+1))-1);int l250 = soc_mem_index_count(l17,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l17)){l250>>= 1;}if(!soc_alpm_mode_get(l17)){(l39[(l17)][
(((3*(128+2+1))-1))].l34) = -1;for(l207 = ((3*(128+2+1))-1);l207>-1;l207--){
if(-1 == (l39[(l17)][(l207)].l32)){continue;}(l39[(l17)][(l207)].l34) = l269;
(l39[(l17)][(l269)].next) = l207;(l39[(l17)][(l269)].l36) = (l39[(l17)][(l207
)].l32)-(l39[(l17)][(l269)].l33)-1;l269 = l207;}(l39[(l17)][(l269)].next) = -
1;(l39[(l17)][(l269)].l36) = l250-(l39[(l17)][(l269)].l33)-1;}else{(l39[(l17)
][(((3*(128+2+1))-1))].l34) = -1;for(l207 = ((3*(128+2+1))-1);l207>(((3*(128+
2+1))-1)/3);l207--){if(-1 == (l39[(l17)][(l207)].l32)){continue;}(l39[(l17)][
(l207)].l34) = l269;(l39[(l17)][(l269)].next) = l207;(l39[(l17)][(l269)].l36)
= (l39[(l17)][(l207)].l32)-(l39[(l17)][(l269)].l33)-1;l269 = l207;}(l39[(l17)
][(l269)].next) = -1;(l39[(l17)][(l269)].l36) = l250-(l39[(l17)][(l269)].l33)
-1;l269 = (((3*(128+2+1))-1)/3);(l39[(l17)][((((3*(128+2+1))-1)/3))].l34) = -
1;for(l207 = ((((3*(128+2+1))-1)/3)-1);l207>-1;l207--){if(-1 == (l39[(l17)][(
l207)].l32)){continue;}(l39[(l17)][(l207)].l34) = l269;(l39[(l17)][(l269)].
next) = l207;(l39[(l17)][(l269)].l36) = (l39[(l17)][(l207)].l32)-(l39[(l17)][
(l269)].l33)-1;l269 = l207;}(l39[(l17)][(l269)].next) = -1;(l39[(l17)][(l269)
].l36) = (l250>>1)-(l39[(l17)][(l269)].l33)-1;}return(SOC_E_NONE);}int
soc_alpm_128_warmboot_lpm_reinit(int l17,int l7,int l207,void*lpm_entry){int
l14;l214(l17,lpm_entry,l207,0x4000,0);SOC_IF_ERROR_RETURN(l248(l17,lpm_entry,
&l14));if((l39[(l17)][(l14)].l35) == 0){(l39[(l17)][(l14)].l32) = l207;(l39[(
l17)][(l14)].l33) = l207;}else{(l39[(l17)][(l14)].l33) = l207;}(l39[(l17)][(
l14)].l35)++;return(SOC_E_NONE);}
#endif /* ALPM_ENABLE */
