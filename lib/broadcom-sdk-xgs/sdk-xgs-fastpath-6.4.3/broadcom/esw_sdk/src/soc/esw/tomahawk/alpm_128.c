/*
 * $Id: alpm_128.c,v 1.13 Broadcom SDK $
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
 * Purpose: Tomahawk Primitives for LPM management in ALPM - Mode for IPv6-128.
 * Requires:
 */

#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#ifdef ALPM_ENABLE

#include <shared/util.h>
#include <shared/l3.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#include <soc/esw/trie.h>

#if 1
void soc_th_alpm_128_lpm_state_dump(int l1);extern int soc_th_get_alpm_banks(
int unit);extern int soc_th_alpm_mode_get(int l1);static int l2(int l1,void*
l3,int*index);static int soc_th_alpm_128_lpm_delete(int l1,void*key_data);
static int l4(int l1,void*key_data,int l5,int l6,int l7,
defip_aux_scratch_entry_t*l8);static int l9(int l1,void*key_data,void*l10,int
*l11,int*l12,int*l5);static int l13(int l1,void*key_data,void*l10,int*l11);
static int l14(int unit,void*lpm_entry,void*l15,void*l16,soc_mem_t l17,uint32
l18,uint32*l19);static int l20(int unit,void*l15,soc_mem_t l17,int l5,int l21
,int l22,int index,void*lpm_entry);static int l23(int unit,uint32*key,int len
,int l21,int l5,defip_pair_128_entry_t*lpm_entry,int l24,int l25);static int
l26(int l1,int l21,int l27);static int l28(int l1,alpm_pfx_info_t*l29,trie_t*
l30,uint32*l31,uint32 l32,trie_node_t*l33,defip_pair_128_entry_t*lpm_entry,
uint32*l34);static int l35(int l1,alpm_pfx_info_t*l29,int*l36,int*l11);extern
int _soc_th_alpm_free_pfx_trie(int l1,trie_t*l30,trie_t*l37,payload_t*
new_pfx_pyld,int*l38,int bucket_index,int l27);extern int
_soc_th_alpm_rollback_bkt_move(int l1,void*key_data,soc_mem_t l17,
alpm_pivot_t*l39,alpm_pivot_t*l40,alpm_mem_prefix_array_t*l41,int*l38,int l42
);extern int soc_th_alpm_lpm_delete(int l1,void*key_data);void
_soc_th_alpm_rollback_pivot_add(int l1,defip_pair_128_entry_t*l43,void*
key_data,int tcam_index,alpm_pivot_t*pivot_pyld);typedef struct l44{int l45;
int l46;int l47;int next;int l48;int l49;}l50,*l51;static l51 l52[
SOC_MAX_NUM_DEVICES];typedef struct l53{soc_field_info_t*l54;soc_field_info_t
*l55;soc_field_info_t*l56;soc_field_info_t*l57;soc_field_info_t*l58;
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
soc_field_info_t*l95;soc_field_info_t*l96;soc_field_info_t*l97;}l98,*l99;
static l99 l100[SOC_MAX_NUM_DEVICES];typedef struct l101{int unit;int l102;
int l103;uint16*l104;uint16*l105;}l106;typedef uint32 l107[9];typedef int(*
l108)(l107 l109,l107 l110);static l106*l111[SOC_MAX_NUM_DEVICES];static void
l112(int l1,void*l10,int index,l107 l113);static uint16 l114(uint8*l115,int
l116);static int l117(int unit,int l102,int l103,l106**l118);static int l119(
l106*l120);static int l121(l106*l122,l108 l123,l107 entry,int l124,uint16*
l125);static int l126(l106*l122,l108 l123,l107 entry,int l124,uint16 l127,
uint16 l38);static int l128(l106*l122,l108 l123,l107 entry,int l124,uint16
l129);static int l130(int l1,const void*entry,int*l124){int l131,l132;int l133
[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,
IP_ADDR_MASK1_UPRf};uint32 l134;l134 = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,l133[0]);if((l132 = _ipmask2pfx(l134,l124))<0){
return(l132);}for(l131 = 1;l131<4;l131++){l134 = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,l133[l131]);if(*l124){if(l134!= 0xffffffff){return(
SOC_E_PARAM);}*l124+= 32;}else{if((l132 = _ipmask2pfx(l134,l124))<0){return(
l132);}}}return SOC_E_NONE;}static void l135(uint32*l136,int l32,int l27){
uint32 l137,l138,l45,prefix[5];int l131;sal_memcpy(prefix,l136,sizeof(uint32)
*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(l136,0,sizeof(uint32)*BITS2WORDS(
_MAX_KEY_LEN_144_));l137 = 128-l32;l45 = (l137+31)/32;if((l137%32) == 0){l45
++;}l137 = l137%32;for(l131 = l45;l131<= 4;l131++){prefix[l131]<<= l137;l138 = 
prefix[l131+1]&~(0xffffffff>>l137);l138 = (((32-l137) == 32)?0:(l138)>>(32-
l137));if(l131<4){prefix[l131]|= l138;}}for(l131 = l45;l131<= 4;l131++){l136[
3-(l131-l45)] = prefix[l131];}}static void l139(int unit,void*lpm_entry,int
l12){int l131;soc_field_t l140[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};for(l131 = 0;l131<4;l131++){
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l140[l131],0);}for(l131
= 0;l131<4;l131++){if(l12<= 32)break;soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,l140[3-l131],0xffffffff);l12-= 32;}
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l140[3-l131],~(((l12) == 
32)?0:(0xffffffff)>>(l12)));}int _alpm_128_prefix_create(int l1,void*entry,
uint32*l136,uint32*l12,int*l19){int l131;int l124 = 0,l45;int l132 = 
SOC_E_NONE;uint32 l137,l138;uint32 prefix[5];sal_memset(l136,0,sizeof(uint32)
*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(prefix,0,sizeof(uint32)*BITS2WORDS
(_MAX_KEY_LEN_144_));prefix[0] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR0_LWRf);prefix[1] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR1_LWRf);prefix[2] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR0_UPRf);prefix[3] = soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,
entry,IP_ADDR1_UPRf);l132 = l130(l1,entry,&l124);if(SOC_FAILURE(l132)){return
l132;}l137 = 128-l124;l45 = l137/32;l137 = l137%32;for(l131 = l45;l131<4;l131
++){prefix[l131]>>= l137;l138 = prefix[l131+1]&((1<<l137)-1);l138 = (((32-
l137) == 32)?0:(l138)<<(32-l137));prefix[l131]|= l138;}for(l131 = l45;l131<4;
l131++){l136[4-(l131-l45)] = prefix[l131];}*l12 = l124;if(l19!= NULL){*l19 = 
(prefix[0] == 0)&&(prefix[1] == 0)&&(prefix[2] == 0)&&(prefix[3] == 0)&&(l124
== 0);}return SOC_E_NONE;}int l141(int l1,uint32*prefix,uint32 l32,int l5,int
l21,int*l142,int*tcam_index,int*bucket_index){int l132 = SOC_E_NONE;trie_t*
l143;trie_node_t*l144 = NULL;alpm_pivot_t*pivot_pyld;l143 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l21);l132 = trie_find_lpm(l143,prefix,l32,&l144);
if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l132;}pivot_pyld = (alpm_pivot_t*)l144;*l142 = 
1;*tcam_index = PIVOT_TCAM_INDEX(pivot_pyld);*bucket_index = 
PIVOT_BUCKET_INDEX(pivot_pyld);return SOC_E_NONE;}static int l145(int l1,void
*key_data,soc_mem_t l17,void*alpm_data,int*tcam_index,int*bucket_index,int*
l11){uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l146,l21,l27;int l125;uint32 l6,
l147;int l132 = SOC_E_NONE;int l142 = 0;l27 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l146,&l21));if(
l146 == 0){if(soc_th_alpm_mode_get(l1)){return SOC_E_PARAM;}}
soc_th_alpm_bank_db_type_get(l1,l21,&l147,&l6);if(l146!= SOC_L3_VRF_OVERRIDE)
{uint32 prefix[5],l32;int l19 = 0;l132 = _alpm_128_prefix_create(l1,key_data,
prefix,&l32,&l19);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"_soc_alpm_insert: prefix create failed\n")));return l132;}l132 = l141(l1
,prefix,l32,l27,l21,&l142,tcam_index,bucket_index);SOC_IF_ERROR_RETURN(l132);
if(l142){l14(l1,key_data,l10,0,l17,0,0);l132 = _soc_th_alpm_find_in_bkt(l1,
l17,*bucket_index,l147,l10,alpm_data,&l125,l27);if(SOC_SUCCESS(l132)){*l11 = 
l125;}}else{l132 = SOC_E_NOT_FOUND;}}return l132;}static int l148(int l1,void
*key_data,void*alpm_data,void*alpm_sip_data,soc_mem_t l17,int l125){
defip_aux_scratch_entry_t l8;int l146,l27,l21;int bucket_index;uint32 l6,l147
;int l132 = SOC_E_NONE;int l142 = 0,l138 = 0;int tcam_index;l27 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data
,&l146,&l21));soc_th_alpm_bank_db_type_get(l1,l21,&l147,&l6);if(!
soc_th_alpm_mode_get(l1)){l6 = 2;}if(l146!= SOC_L3_VRF_OVERRIDE){sal_memset(&
l8,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,
l27,l6,0,&l8));SOC_IF_ERROR_RETURN(soc_mem_write(l1,l17,MEM_BLOCK_ANY,l125,
alpm_data));if(l132!= SOC_E_NONE){return SOC_E_FAIL;}if(SOC_URPF_STATUS_GET(
l1)){SOC_IF_ERROR_RETURN(soc_mem_write(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l125),alpm_sip_data));if(l132!= SOC_E_NONE){return
SOC_E_FAIL;}}l138 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l8,
IP_LENGTHf);soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,REPLACE_LENf,
l138);SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l8,TRUE,&
l142,&tcam_index,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l138 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf);l138+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf,l138);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l8,TRUE,&l142,&
tcam_index,&bucket_index));}}return l132;}static int l149(int l1,int l36,int
l34){int l132,l138,l150,l151;defip_aux_table_entry_t l152,l153;l150 = 
SOC_ALPM_128_ADDR_LWR(l36);l151 = SOC_ALPM_128_ADDR_UPR(l36);l132 = 
soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l150,&l152);
SOC_IF_ERROR_RETURN(l132);l132 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l151,&l153);SOC_IF_ERROR_RETURN(l132);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l152,BPM_LENGTH0f,l34);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l152,BPM_LENGTH1f,l34);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l153,BPM_LENGTH0f,l34);soc_mem_field32_set(l1,
L3_DEFIP_AUX_TABLEm,&l153,BPM_LENGTH1f,l34);l138 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_TABLEm,&l152,DB_TYPE0f);l132 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l150,&l152);SOC_IF_ERROR_RETURN(l132);l132 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l151,&l153);
SOC_IF_ERROR_RETURN(l132);if(SOC_URPF_STATUS_GET(l1)){l138++;
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l152,BPM_LENGTH0f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l152,BPM_LENGTH1f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l153,BPM_LENGTH0f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l153,BPM_LENGTH1f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l152,DB_TYPE0f,l138);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l152,DB_TYPE1f,l138);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l153,DB_TYPE0f,l138);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l153,DB_TYPE1f,l138);l150+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m))/2;l151+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m))/2;l132 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l150,&l152);SOC_IF_ERROR_RETURN(l132);l132 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l151,&l153);}return l132;}
static int l154(int l1,int l155,void*entry,defip_aux_table_entry_t*l156,int
l157){uint32 l138,l6,l158 = 0;soc_mem_t l17 = L3_DEFIP_PAIR_128m;soc_mem_t
l159 = L3_DEFIP_AUX_TABLEm;int l132 = SOC_E_NONE,l124,l21;void*l160,*l161;
l160 = (void*)l156;l161 = (void*)(l156+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1
,l159,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l155),l156));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l159,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l155),l156+1));l138 = 
soc_mem_field32_get(l1,l17,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l159,
l160,VRF0f,l138);l138 = soc_mem_field32_get(l1,l17,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l159,l160,VRF1f,l138);l138 = soc_mem_field32_get(l1,
l17,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l159,l161,VRF0f,l138);l138 = 
soc_mem_field32_get(l1,l17,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l159,
l161,VRF1f,l138);l138 = soc_mem_field32_get(l1,l17,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l159,l160,MODE0f,l138);l138 = soc_mem_field32_get(l1,
l17,entry,MODE1_LWRf);soc_mem_field32_set(l1,l159,l160,MODE1f,l138);l138 = 
soc_mem_field32_get(l1,l17,entry,MODE0_UPRf);soc_mem_field32_set(l1,l159,l161
,MODE0f,l138);l138 = soc_mem_field32_get(l1,l17,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l159,l161,MODE1f,l138);l138 = soc_mem_field32_get(l1,
l17,entry,VALID0_LWRf);soc_mem_field32_set(l1,l159,l160,VALID0f,l138);l138 = 
soc_mem_field32_get(l1,l17,entry,VALID1_LWRf);soc_mem_field32_set(l1,l159,
l160,VALID1f,l138);l138 = soc_mem_field32_get(l1,l17,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l159,l161,VALID0f,l138);l138 = soc_mem_field32_get(l1,
l17,entry,VALID1_UPRf);soc_mem_field32_set(l1,l159,l161,VALID1f,l138);l132 = 
l130(l1,entry,&l124);SOC_IF_ERROR_RETURN(l132);soc_mem_field32_set(l1,l159,
l160,IP_LENGTH0f,l124);soc_mem_field32_set(l1,l159,l160,IP_LENGTH1f,l124);
soc_mem_field32_set(l1,l159,l161,IP_LENGTH0f,l124);soc_mem_field32_set(l1,
l159,l161,IP_LENGTH1f,l124);l138 = soc_mem_field32_get(l1,l17,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l159,l160,IP_ADDR0f,l138);l138 = 
soc_mem_field32_get(l1,l17,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l159,
l160,IP_ADDR1f,l138);l138 = soc_mem_field32_get(l1,l17,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l159,l161,IP_ADDR0f,l138);l138 = soc_mem_field32_get(
l1,l17,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l159,l161,IP_ADDR1f,l138);
l138 = soc_mem_field32_get(l1,l17,entry,ENTRY_TYPE0_LWRf);soc_mem_field32_set
(l1,l159,l160,ENTRY_TYPE0f,l138);l138 = soc_mem_field32_get(l1,l17,entry,
ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,l159,l160,ENTRY_TYPE1f,l138);l138 = 
soc_mem_field32_get(l1,l17,entry,ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,
l159,l161,ENTRY_TYPE0f,l138);l138 = soc_mem_field32_get(l1,l17,entry,
ENTRY_TYPE1_UPRf);soc_mem_field32_set(l1,l159,l161,ENTRY_TYPE1f,l138);l132 = 
soc_th_alpm_128_lpm_vrf_get(l1,entry,&l21,&l124);SOC_IF_ERROR_RETURN(l132);if
(SOC_URPF_STATUS_GET(l1)){if(l157>= (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1)){l158 = 1;}}switch(l21){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l159,l160,VALID0f,0);soc_mem_field32_set(l1,l159,l160,
VALID1f,0);soc_mem_field32_set(l1,l159,l161,VALID0f,0);soc_mem_field32_set(l1
,l159,l161,VALID1f,0);l6 = 0;break;case SOC_L3_VRF_GLOBAL:l6 = l158?1:0;break
;default:l6 = l158?3:2;break;}soc_mem_field32_set(l1,l159,l160,DB_TYPE0f,l6);
soc_mem_field32_set(l1,l159,l160,DB_TYPE1f,l6);soc_mem_field32_set(l1,l159,
l161,DB_TYPE0f,l6);soc_mem_field32_set(l1,l159,l161,DB_TYPE1f,l6);if(l158){
l138 = soc_mem_field32_get(l1,l17,entry,ALG_BKT_PTRf);if(l138){l138+= 
SOC_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l17,entry,ALG_BKT_PTRf,l138)
;}}return SOC_E_NONE;}static int l162(int l1,int l163,int index,int l164,void
*entry){defip_aux_table_entry_t l156[2];l164 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l164,1);SOC_IF_ERROR_RETURN(l154(l1,l164,entry,&l156[0],
index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,
entry));index = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(index),l156));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(index),l156+
1));return SOC_E_NONE;}void l165(int l1,defip_pair_128_entry_t*l43,void*
key_data,int tcam_index,alpm_pivot_t*pivot_pyld){int l132;trie_t*l143 = NULL;
int l27,l21,l146;trie_node_t*l166 = NULL;l27 = L3_DEFIP_MODE_128;(void)
soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l146,&l21);l132 = 
soc_th_alpm_128_lpm_delete(l1,l43);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failure to free new prefix"
"at %d\n"),soc_th_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,tcam_index,l27)));}
l143 = VRF_PIVOT_TRIE_IPV6(l1,l21);if(ALPM_TCAM_PIVOT(l1,tcam_index<<(l27?1:0
))!= NULL){l132 = trie_delete(l143,pivot_pyld->key,pivot_pyld->len,&l166);if(
SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l1,tcam_index<<(l27?1:0)) = NULL;VRF_PIVOT_REF_DEC(l1,l21,l27
);}static int l167(int l1,soc_mem_t l17,alpm_mem_prefix_array_t*l168,int*l38)
{int l131,l132 = SOC_E_NONE,l169;defip_alpm_ipv6_128_entry_t l170,l171;void*
l172 = NULL,*l173 = NULL;int l174,l175;int*l176 = NULL;int l177 = FALSE;l174 = 
sizeof(l170);l175 = sizeof(l171);l172 = sal_alloc(l174*l168->count,"rb_bufp")
;if(l172 == NULL){l132 = SOC_E_MEMORY;goto l178;}l173 = sal_alloc(l175*l168->
count,"rb_sip_bufp");if(l173 == NULL){l132 = SOC_E_MEMORY;goto l178;}l176 = 
sal_alloc(sizeof(*l176)*l168->count,"roll_back_index");if(l176 == NULL){l132 = 
SOC_E_MEMORY;goto l178;}sal_memset(l176,-1,sizeof(*l176)*l168->count);for(
l131 = 0;l131<l168->count;l131++){payload_t*prefix = l168->prefix[l131];if(
prefix->index>= 0){l132 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,prefix->index,(
uint8*)l172+l131*l174);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x 0x%08x"" 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l131--;l177 = TRUE;
break;}if(SOC_URPF_STATUS_GET(l1)){l132 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,prefix->index),(uint8*)l173+l131*l175);if(
SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_split_inval: Failed to read"
" bkt entry for invalidate for pfx 0x%08x"" 0x%08x 0x%08x 0x%08x\n"),prefix->
key[1],prefix->key[2],prefix->key[3],prefix->key[4]));l131--;l177 = TRUE;
break;}}l132 = soc_mem_write(l1,l17,MEM_BLOCK_ALL,prefix->index,
soc_mem_entry_null(l1,l17));if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old bkt for pfx 0x%08x 0x%08x\n"),prefix->key[0],prefix->key[
1]));l176[l131] = prefix->index;l177 = TRUE;break;}if(SOC_URPF_STATUS_GET(l1)
){l132 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,prefix
->index),soc_mem_entry_null(l1,l17));if(SOC_FAILURE(l132)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_split_inval: Failed to remove"
" prefixes from old urpf bkt for pfx 0x%08x"" 0x%08x\n"),prefix->key[0],
prefix->key[1]));l176[l131] = prefix->index;l177 = TRUE;break;}}}l176[l131] = 
prefix->index;prefix->index = l38[l131];}if(l177){for(;l131>= 0;l131--){
payload_t*prefix = l168->prefix[l131];prefix->index = l176[l131];if(l176[l131
]<0){continue;}l169 = soc_mem_write(l1,l17,MEM_BLOCK_ALL,l176[l131],(uint8*)
l172+l131*l174);if(SOC_FAILURE(l169)){break;}if(!SOC_URPF_STATUS_GET(l1)){
continue;}l169 = soc_mem_write(l1,l17,MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(l1
,l176[l131]),(uint8*)l173+l131*l175);if(SOC_FAILURE(l169)){break;}}}l178:if(
l176){sal_free(l176);l176 = NULL;}if(l173){sal_free(l173);l173 = NULL;}if(
l172){sal_free(l172);l172 = NULL;}return l132;}static int l28(int l1,
alpm_pfx_info_t*l29,trie_t*l30,uint32*l31,uint32 l32,trie_node_t*l33,
defip_pair_128_entry_t*lpm_entry,uint32*l34){trie_node_t*l144 = NULL;int l27,
l21,l146;defip_alpm_ipv6_128_entry_t l170;payload_t*l179 = NULL;int l180;void
*l181;alpm_pivot_t*l182;int l132 = SOC_E_NONE;soc_mem_t l17;
alpm_bucket_handle_t*l183;l182 = l29->pivot_pyld;l180 = l182->tcam_index;l27 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l29->
key_data,&l146,&l21));l17 = L3_DEFIP_ALPM_IPV6_128m;l181 = ((uint32*)&(l170))
;l144 = NULL;l132 = trie_find_lpm(l30,l31,l32,&l144);l179 = (payload_t*)l144;
if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l31[0],l31[1],l31[2],
l31[3],l31[4],l32));return l132;}if(l179->bkt_ptr){if(l179->bkt_ptr == l29->
new_pfx_pyld){sal_memcpy(l181,l29->alpm_data,sizeof(
defip_alpm_ipv6_128_entry_t));}else{l132 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,
((payload_t*)l179->bkt_ptr)->index,l181);}if(SOC_FAILURE(l132)){return l132;}
l132 = l20(l1,l181,l17,l27,l146,l29->bucket_index,0,lpm_entry);if(SOC_FAILURE
(l132)){return l132;}*l34 = ((payload_t*)(l179->bkt_ptr))->len;}else{l132 = 
soc_mem_read(l1,L3_DEFIP_PAIR_128m,MEM_BLOCK_ANY,soc_th_alpm_logical_idx(l1,
L3_DEFIP_PAIR_128m,SOC_ALPM_128_DEFIP_TO_PAIR(l180>>1),1),lpm_entry);}l183 = 
sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l183 == NULL)
{LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l132 = SOC_E_MEMORY;return l132;}sal_memset(l183,0,sizeof(*l183));l182 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l182 == NULL){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l132 = SOC_E_MEMORY;return l132;}sal_memset(l182,0,sizeof(*l182));
PIVOT_BUCKET_HANDLE(l182) = l183;l132 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l182));PIVOT_BUCKET_TRIE(l182)->trie = l33;
PIVOT_BUCKET_INDEX(l182) = l29->bucket_index;PIVOT_BUCKET_VRF(l182) = l21;
PIVOT_BUCKET_IPV6(l182) = l27;PIVOT_BUCKET_DEF(l182) = FALSE;(l182)->key[0] = 
l31[0];(l182)->key[1] = l31[1];(l182)->key[2] = l31[2];(l182)->key[3] = l31[3
];(l182)->key[4] = l31[4];(l182)->len = l32;l135((l31),(l32),(l27));l23(l1,
l31,l32,l21,l27,lpm_entry,0,0);soc_L3_DEFIP_PAIR_128m_field32_set(l1,
lpm_entry,ALG_BKT_PTRf,l29->bucket_index);l29->pivot_pyld = l182;return l132;
}static int l35(int l1,alpm_pfx_info_t*l29,int*l36,int*l11){trie_node_t*l33;
uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l27,l21,l146;uint32 l32,l34 = 0;
uint32 l147 = 0;uint32 l31[5];int l125;defip_pair_128_entry_t l170,l171;
trie_t*l30,*trie;void*l181,*l184;alpm_pivot_t*l185 = l29->pivot_pyld;
defip_pair_128_entry_t lpm_entry;soc_mem_t l17;trie_t*l143 = NULL;
alpm_mem_prefix_array_t l168;int*l38 = NULL;int l132 = SOC_E_NONE,l131,l42 = 
-1;l27 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1
,l29->key_data,&l146,&l21));l17 = L3_DEFIP_ALPM_IPV6_128m;l181 = ((uint32*)&(
l170));l184 = ((uint32*)&(l171));soc_th_alpm_bank_db_type_get(l1,l21,&l147,
NULL);l30 = VRF_PREFIX_TRIE_IPV6_128(l1,l21);trie = PIVOT_BUCKET_TRIE(l29->
pivot_pyld);l132 = soc_th_alpm_bucket_assign(l1,&l29->bucket_index,l27);if(
SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate""new bucket for split\n")));l29->
bucket_index = -1;_soc_th_alpm_free_pfx_trie(l1,l30,trie,l29->new_pfx_pyld,
l38,l29->bucket_index,l27);return l132;}l132 = trie_split(trie,
_MAX_KEY_LEN_144_,FALSE,l31,&l32,&l33,NULL,FALSE);if(SOC_FAILURE(l132)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not split bucket")));_soc_th_alpm_free_pfx_trie(l1,
l30,trie,l29->new_pfx_pyld,l38,l29->bucket_index,l27);return l132;}l132 = l28
(l1,l29,l30,l31,l32,l33,&lpm_entry,&l34);if(l132!= SOC_E_NONE){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"could not initialize pivot")));
_soc_th_alpm_free_pfx_trie(l1,l30,trie,l29->new_pfx_pyld,l38,l29->
bucket_index,l27);return l132;}sal_memset(&l168,0,sizeof(l168));l132 = 
trie_traverse(PIVOT_BUCKET_TRIE(l29->pivot_pyld),
_soc_th_alpm_mem_prefix_array_cb,&l168,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE
(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Bucket split failed")));_soc_th_alpm_free_pfx_trie(l1,l30,
trie,l29->new_pfx_pyld,l38,l29->bucket_index,l27);return l132;}l38 = 
sal_alloc(sizeof(*l38)*l168.count,
"Temp storage for location of prefixes in new 128b bucket");if(l38 == NULL){
l132 = SOC_E_MEMORY;_soc_th_alpm_free_pfx_trie(l1,l30,trie,l29->new_pfx_pyld,
l38,l29->bucket_index,l27);return l132;}sal_memset(l38,-1,sizeof(*l38)*l168.
count);for(l131 = 0;l131<l168.count;l131++){payload_t*l124 = l168.prefix[l131
];if(l124->index>0){l132 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,l124->index,l181
);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Failed to""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l168.prefix[l131]->key[1],l168.
prefix[l131]->key[2],l168.prefix[l131]->key[3],l168.prefix[l131]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,l29->key_data,l17,l185,l29->pivot_pyld
,&l168,l38,l42);return l132;}if(SOC_URPF_STATUS_GET(l1)){l132 = soc_mem_read(
l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l124->index),l184);if(
SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l168.prefix[l131]->key[1],l168.
prefix[l131]->key[2],l168.prefix[l131]->key[3],l168.prefix[l131]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,l29->key_data,l17,l185,l29->pivot_pyld
,&l168,l38,l42);return l132;}}l132 = _soc_th_alpm_insert_in_bkt(l1,l17,l29->
bucket_index,l147,l181,l10,&l125,l27);if(SOC_SUCCESS(l132)){if(
SOC_URPF_STATUS_GET(l1)){l132 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l125),l184);}}}else{l132 = 
_soc_th_alpm_insert_in_bkt(l1,l17,l29->bucket_index,l147,l29->alpm_data,l10,&
l125,l27);if(SOC_SUCCESS(l132)){l42 = l131;*l11 = l125;if(SOC_URPF_STATUS_GET
(l1)){l132 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,
l125),l29->alpm_sip_data);}}}l38[l131] = l125;if(SOC_FAILURE(l132)){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failed to"
"read rpf prefix ""0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l168.prefix[l131]
->key[1],l168.prefix[l131]->key[2],l168.prefix[l131]->key[3],l168.prefix[l131
]->key[4]));(void)_soc_th_alpm_rollback_bkt_move(l1,l29->key_data,l17,l185,
l29->pivot_pyld,&l168,l38,l42);return l132;}}l132 = l2(l1,&lpm_entry,l36);if(
SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to add new""pivot to tcam\n")));if(l132 == 
SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l21,l27);}(void)
_soc_th_alpm_rollback_bkt_move(l1,l29->key_data,l17,l185,l29->pivot_pyld,&
l168,l38,l42);return l132;}*l36 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,*l36,l27);l132 = l149(l1,*l36,l34);if(SOC_FAILURE(l132)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l36));l165(l1,&
lpm_entry,l29->key_data,*l36,l29->pivot_pyld);(void)
_soc_th_alpm_rollback_bkt_move(l1,l29->key_data,l17,l185,l29->pivot_pyld,&
l168,l38,l42);return l132;}l143 = VRF_PIVOT_TRIE_IPV6_128(l1,l21);l132 = 
trie_insert(l143,(l29->pivot_pyld)->key,NULL,(l29->pivot_pyld)->len,(
trie_node_t*)l29->pivot_pyld);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"failed to insert into pivot trie\n")));(void)
_soc_th_alpm_rollback_bkt_move(l1,l29->key_data,l17,l185,l29->pivot_pyld,&
l168,l38,l42);return l132;}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(*l36)<<1)
= l29->pivot_pyld;PIVOT_TCAM_INDEX(l29->pivot_pyld) = SOC_ALPM_128_ADDR_LWR(*
l36)<<1;VRF_PIVOT_REF_INC(l1,l21,l27);l132 = l167(l1,l17,&l168,l38);if(
SOC_FAILURE(l132)){l165(l1,&lpm_entry,l29->key_data,*l36,l29->pivot_pyld);(
void)_soc_th_alpm_rollback_bkt_move(l1,l29->key_data,l17,l185,l29->pivot_pyld
,&l168,l38,l42);sal_free(l38);l38 = NULL;return l132;}sal_free(l38);if(l42 == 
-1){l132 = _soc_th_alpm_insert_in_bkt(l1,l17,PIVOT_BUCKET_INDEX(l185),l147,
l29->alpm_data,l10,&l125,l27);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_insert: Could not insert new "
"prefix into trie after split\n")));_soc_th_alpm_free_pfx_trie(l1,l30,trie,
l29->new_pfx_pyld,l38,l29->bucket_index,l27);return l132;}if(
SOC_URPF_STATUS_GET(l1)){l132 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l125),l29->alpm_sip_data);}*l11 = l125;l29->
new_pfx_pyld->index = l125;}PIVOT_BUCKET_ENT_CNT_UPDATE(l29->pivot_pyld);
VRF_BUCKET_SPLIT_INC(l1,l21,l27);return l132;}static int l186(int l1,void*
key_data,soc_mem_t l17,void*alpm_data,void*alpm_sip_data,int*l11,int
bucket_index,int tcam_index){alpm_pivot_t*pivot_pyld,*l185;
defip_aux_scratch_entry_t l8;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];uint32 prefix
[5],l32;int l27,l21,l146;int l125;int l132 = SOC_E_NONE,l169;uint32 l6,l147;
int l142 = 0;int l36;int l187 = 0;trie_t*trie,*l30;trie_node_t*l144 = NULL;
payload_t*l188,*l189,*l179;int l19 = 0;int*l38 = NULL;alpm_pfx_info_t l29;l27
= L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,
key_data,&l146,&l21));soc_th_alpm_bank_db_type_get(l1,l21,&l147,&l6);l17 = 
L3_DEFIP_ALPM_IPV6_128m;l132 = _alpm_128_prefix_create(l1,key_data,prefix,&
l32,&l19);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: prefix create failed\n")));return l132;}sal_memset(&l8
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l27,
l6,0,&l8));if(bucket_index == 0){l132 = l141(l1,prefix,l32,l27,l21,&l142,&
tcam_index,&bucket_index);SOC_IF_ERROR_RETURN(l132);if(l142 == 0){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_insert: "
" Could not find bucket to insert prefix\n")));return SOC_E_NOT_FOUND;}}l132 = 
_soc_th_alpm_insert_in_bkt(l1,l17,bucket_index,l147,alpm_data,l10,&l125,l27);
if(l132 == SOC_E_NONE){*l11 = l125;if(SOC_URPF_STATUS_GET(l1)){l169 = 
soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l125),
alpm_sip_data);if(SOC_FAILURE(l169)){return l169;}}}if(l132 == SOC_E_FULL){
l187 = 1;}pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l185 = pivot_pyld;l188 = sal_alloc(sizeof(
payload_t),"Payload for 128b Key");if(l188 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"trie node \n")));return SOC_E_MEMORY;}l189 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l189 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l188);return SOC_E_MEMORY;}sal_memset(l188,0,
sizeof(*l188));sal_memset(l189,0,sizeof(*l189));l188->key[0] = prefix[0];l188
->key[1] = prefix[1];l188->key[2] = prefix[2];l188->key[3] = prefix[3];l188->
key[4] = prefix[4];l188->len = l32;l188->index = l125;sal_memcpy(l189,l188,
sizeof(*l188));l189->bkt_ptr = l188;l132 = trie_insert(trie,prefix,NULL,l32,(
trie_node_t*)l188);if(SOC_FAILURE(l132)){if(l188!= NULL){sal_free(l188);}if(
l189!= NULL){sal_free(l189);}return l132;}l30 = VRF_PREFIX_TRIE_IPV6_128(l1,
l21);if(!l19){l132 = trie_insert(l30,prefix,NULL,l32,(trie_node_t*)l189);}
else{l144 = NULL;l132 = trie_find_lpm(l30,0,0,&l144);l179 = (payload_t*)l144;
if(SOC_SUCCESS(l132)){l179->bkt_ptr = l188;}}if(SOC_FAILURE(l132)){
_soc_th_alpm_free_pfx_trie(l1,l30,trie,l189,l38,bucket_index,l27);return l132
;}if(l187){l29.key_data = key_data;l29.new_pfx_pyld = l188;l29.pivot_pyld = 
pivot_pyld;l29.alpm_data = alpm_data;l29.alpm_sip_data = alpm_sip_data;l29.
bucket_index = bucket_index;l132 = l35(l1,&l29,&l36,l11);if(l132!= SOC_E_NONE
){return l132;}bucket_index = l29.bucket_index;}VRF_TRIE_ROUTES_INC(l1,l21,
l27);if(l19){sal_free(l189);}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
DELETE_PROPAGATE,&l8,TRUE,&l142,&tcam_index,&bucket_index));
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l8,FALSE,&l142,&
tcam_index,&bucket_index));if(SOC_URPF_STATUS_GET(l1)){l32 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf);l32+= 1;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf,l32);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l8,TRUE,&l142,&
tcam_index,&bucket_index));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
INSERT_PROPAGATE,&l8,FALSE,&l142,&tcam_index,&bucket_index));}
PIVOT_BUCKET_ENT_CNT_UPDATE(l185);return l132;}static int l23(int unit,uint32
*key,int len,int l21,int l5,defip_pair_128_entry_t*lpm_entry,int l24,int l25)
{uint32 l138;if(l25){sal_memset(lpm_entry,0,sizeof(defip_pair_128_entry_t));}
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VRF_ID_0_LWRf,l21&
SOC_VRF_MAX(unit));soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_1_LWRf,l21&SOC_VRF_MAX(unit));soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,VRF_ID_0_UPRf,l21&SOC_VRF_MAX(unit));
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VRF_ID_1_UPRf,l21&
SOC_VRF_MAX(unit));if(l21 == (SOC_VRF_MAX(unit)+1)){l138 = 0;}else{l138 = 
SOC_VRF_MAX(unit);}soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_MASK0_LWRf,l138);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_MASK1_LWRf,l138);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_MASK0_UPRf,l138);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_MASK1_UPRf,l138);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
IP_ADDR0_LWRf,key[0]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
IP_ADDR1_LWRf,key[1]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
IP_ADDR0_UPRf,key[2]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
IP_ADDR1_UPRf,key[3]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
MODE0_LWRf,3);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,MODE1_LWRf,3)
;soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,MODE0_UPRf,3);
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,MODE1_UPRf,3);l139(unit,(
void*)lpm_entry,len);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VALID0_LWRf,1);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VALID1_LWRf,
1);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VALID0_UPRf,1);
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VALID1_UPRf,1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,(1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,(1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,(1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,(1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,(
1<<soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,(
1<<soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_LWRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,(
1<<soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf))-1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,(
1<<soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_UPRf))-1);
return(SOC_E_NONE);}static int l190(int l1,void*key_data,int bucket_index,int
tcam_index,int l125){alpm_pivot_t*pivot_pyld;defip_alpm_ipv6_128_entry_t l170
,l191,l171;defip_aux_scratch_entry_t l8;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];
soc_mem_t l17;void*l181,*l192,*l184 = NULL;int l146;int l5;int l132 = 
SOC_E_NONE,l169 = SOC_E_NONE;uint32 l193[5],prefix[5];int l27,l21;uint32 l32;
int l194;uint32 l6,l147;int l142,l19 = 0;trie_t*trie,*l30;uint32 l195;
defip_pair_128_entry_t lpm_entry,*l196;payload_t*l188 = NULL,*l197 = NULL,*
l179 = NULL;trie_node_t*l166 = NULL,*l144 = NULL;trie_t*l143 = NULL;l5 = l27 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data
,&l146,&l21));if(l146!= SOC_L3_VRF_OVERRIDE){soc_th_alpm_bank_db_type_get(l1,
l21,&l147,&l6);l132 = _alpm_128_prefix_create(l1,key_data,prefix,&l32,&l19);
if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: prefix create failed\n")));return l132;}if(!
soc_th_alpm_mode_get(l1)){if(l146!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT
(l1,l21,l27)>1){if(l19){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: Cannot v6-128 delete ""default route if other routes are present "
"in this mode"),l21));return SOC_E_PARAM;}}}l6 = 2;}l17 = 
L3_DEFIP_ALPM_IPV6_128m;l181 = ((uint32*)&(l170));SOC_ALPM_LPM_LOCK(l1);if(
bucket_index == 0){l132 = l145(l1,key_data,l17,l181,&tcam_index,&bucket_index
,&l125);}else{l132 = l14(l1,key_data,l181,0,l17,0,0);}sal_memcpy(&l191,l181,
sizeof(l191));l192 = &l191;if(SOC_FAILURE(l132)){SOC_ALPM_LPM_UNLOCK(l1);
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: Unable to find ""prefix for delete\n")));return l132;}
l194 = bucket_index;pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l132 = trie_delete(trie,prefix,l32,&l166);l188 = 
(payload_t*)l166;if(l132!= SOC_E_NONE){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l132;}l30 = VRF_PREFIX_TRIE_IPV6_128(l1,l21);
l143 = VRF_PIVOT_TRIE_IPV6_128(l1,l21);if(!l19){l132 = trie_delete(l30,prefix
,l32,&l166);l197 = (payload_t*)l166;if(SOC_FAILURE(l132)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l198;}l144 = NULL;l132 = trie_find_lpm(l30,prefix,l32,&l144)
;l179 = (payload_t*)l144;if(SOC_SUCCESS(l132)){payload_t*l199 = (payload_t*)(
l179->bkt_ptr);if(l199!= NULL){l195 = l199->len;}else{l195 = 0;}}else{
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l200;}sal_memcpy(l193,prefix,sizeof(
prefix));l135((l193),(l32),(l27));l132 = l23(l1,l193,l195,l21,l5,&lpm_entry,0
,1);(void)l20(l1,l181,l17,l5,l146,bucket_index,0,&lpm_entry);(void)l23(l1,
l193,l32,l21,l5,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(
l132)){l184 = ((uint32*)&(l171));l169 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l125),l184);}}if((l195 == 0)&&SOC_FAILURE(l169)){
l196 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l21);sal_memcpy(&lpm_entry,l196,
sizeof(lpm_entry));l132 = l23(l1,prefix,l195,l21,l5,&lpm_entry,0,1);}if(
SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l200;}l196 = &lpm_entry;}else{l144 = 
NULL;l132 = trie_find_lpm(l30,prefix,l32,&l144);l179 = (payload_t*)l144;if(
SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),l21));goto l198;}l179->bkt_ptr = 0;
l195 = 0;l196 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l21);}l132 = l4(l1,l196,
l27,l6,l195,&l8);if(SOC_FAILURE(l132)){goto l200;}l132 = _soc_th_alpm_aux_op(
l1,DELETE_PROPAGATE,&l8,TRUE,&l142,&tcam_index,&bucket_index);if(SOC_FAILURE(
l132)){goto l200;}if(SOC_URPF_STATUS_GET(l1)){uint32 l138;if(l184!= NULL){
l138 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf);l138++;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf,l138);l138 = 
soc_mem_field32_get(l1,l17,l184,SRC_DISCARDf);soc_mem_field32_set(l1,l17,&l8,
SRC_DISCARDf,l138);l138 = soc_mem_field32_get(l1,l17,l184,DEFAULTROUTEf);
soc_mem_field32_set(l1,l17,&l8,DEFAULTROUTEf,l138);l132 = _soc_th_alpm_aux_op
(l1,DELETE_PROPAGATE,&l8,TRUE,&l142,&tcam_index,&bucket_index);}if(
SOC_FAILURE(l132)){goto l200;}}sal_free(l188);if(!l19){sal_free(l197);}
PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);if((pivot_pyld->len!= 0)&&(trie->trie
== NULL)){uint32 l201[5];sal_memcpy(l201,pivot_pyld->key,sizeof(l201));l135((
l201),(pivot_pyld->len),(l5));l23(l1,l201,pivot_pyld->len,l21,l5,&lpm_entry,0
,1);l132 = soc_th_alpm_128_lpm_delete(l1,&lpm_entry);if(SOC_FAILURE(l132)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),pivot_pyld->key[1],pivot_pyld
->key[2],pivot_pyld->key[3],pivot_pyld->key[4]));}}l132 = 
_soc_th_alpm_delete_in_bkt(l1,l17,l194,l147,l192,l10,&l125,l27);if(!
SOC_SUCCESS(l132)){SOC_ALPM_LPM_UNLOCK(l1);l132 = SOC_E_FAIL;return l132;}if(
SOC_URPF_STATUS_GET(l1)){l132 = soc_mem_alpm_delete(l1,l17,
SOC_ALPM_RPF_BKT_IDX(l1,l194),MEM_BLOCK_ALL,l147,l192,l10,&l142);if(!
SOC_SUCCESS(l132)){SOC_ALPM_LPM_UNLOCK(l1);l132 = SOC_E_FAIL;return l132;}}if
((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l132 = 
soc_th_alpm_bucket_release(l1,PIVOT_BUCKET_INDEX(pivot_pyld),l27);if(
SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Unable to release""empty bucket: %d\n"),
PIVOT_BUCKET_INDEX(pivot_pyld)));}l132 = trie_delete(l143,pivot_pyld->key,
pivot_pyld->len,&l166);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"could not delete pivot from pivot trie\n")));}trie_destroy(
PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(pivot_pyld));
sal_free(pivot_pyld);_soc_tomahawk_alpm_bkt_view_set(l1,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,l194),INVALIDm);if(SOC_TH_ALPM_V6_SCALE_CHECK
(l1,l27)){_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,
l194+1),INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l21,l27);if(VRF_TRIE_ROUTES_CNT(
l1,l21,l27) == 0){l132 = l26(l1,l21,l27);}SOC_ALPM_LPM_UNLOCK(l1);return l132
;l200:l169 = trie_insert(l30,prefix,NULL,l32,(trie_node_t*)l197);if(
SOC_FAILURE(l169)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l198:l169 = trie_insert(trie,prefix,NULL,l32,(
trie_node_t*)l188);if(SOC_FAILURE(l169)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}SOC_ALPM_LPM_UNLOCK(l1);return l132;}int
soc_th_alpm_128_init(int l1){int l132 = SOC_E_NONE;l132 = 
soc_th_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l132);return l132;}int
soc_th_alpm_128_state_clear(int l1){int l131,l132;for(l131 = 0;l131<= 
SOC_VRF_MAX(l1)+1;l131++){l132 = trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,
l131),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l132)){
trie_destroy(VRF_PREFIX_TRIE_IPV6_128(l1,l131));}else{LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n"),l1,l131));return
l132;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l131)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l131));}}return SOC_E_NONE;}int
soc_th_alpm_128_deinit(int l1){soc_th_alpm_128_lpm_deinit(l1);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_state_clear(l1));return SOC_E_NONE;}
static int l202(int l1,int l21,int l27){defip_pair_128_entry_t*lpm_entry,l203
;int l204;int index;int l132 = SOC_E_NONE;uint32 key[5] = {0,0,0,0,0};uint32
l32;alpm_bucket_handle_t*l183;alpm_pivot_t*pivot_pyld;payload_t*l197;trie_t*
l205;trie_t*l206 = NULL;trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(
l1,l21));l206 = VRF_PIVOT_TRIE_IPV6_128(l1,l21);trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6_128(l1,l21));l205 = VRF_PREFIX_TRIE_IPV6_128(l1,l21);
lpm_entry = sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(
lpm_entry == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: unable to allocate memory "
"for IPv6-128 LPM entry\n")));return SOC_E_MEMORY;}l23(l1,key,0,l21,l27,
lpm_entry,0,1);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l21) = lpm_entry;if(l21 == 
SOC_VRF_MAX(l1)+1){soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,
DEFAULT_MISSf,1);}l132 = soc_th_alpm_bucket_assign(l1,&l204,l27);
soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,ALG_BKT_PTRf,l204);sal_memcpy
(&l203,lpm_entry,sizeof(l203));l132 = l2(l1,&l203,&index);l183 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l183 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: Unable to allocate memory ""for bucket handle \n"))
);return SOC_E_NONE;}sal_memset(l183,0,sizeof(*l183));pivot_pyld = sal_alloc(
sizeof(alpm_pivot_t),"Payload for Pivot");if(pivot_pyld == NULL){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: Unable to allocate memory ""for PIVOT trie node \n"
)));sal_free(l183);return SOC_E_MEMORY;}l197 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l197 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_th_alpm_128_vrf_add: Unable to allocate memory "
"for pfx trie node \n")));sal_free(l183);sal_free(pivot_pyld);return
SOC_E_MEMORY;}sal_memset(pivot_pyld,0,sizeof(*pivot_pyld));sal_memset(l197,0,
sizeof(*l197));l32 = 0;PIVOT_BUCKET_HANDLE(pivot_pyld) = l183;trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(pivot_pyld));PIVOT_BUCKET_INDEX(
pivot_pyld) = l204;PIVOT_BUCKET_VRF(pivot_pyld) = l21;PIVOT_BUCKET_IPV6(
pivot_pyld) = l27;PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;pivot_pyld->key[0] = 
l197->key[0] = key[0];pivot_pyld->key[1] = l197->key[1] = key[1];pivot_pyld->
key[2] = l197->key[2] = key[2];pivot_pyld->key[3] = l197->key[3] = key[3];
pivot_pyld->key[4] = l197->key[4] = key[4];pivot_pyld->len = l197->len = l32;
l132 = trie_insert(l205,key,NULL,l32,&(l197->node));if(SOC_FAILURE(l132)){
sal_free(l197);sal_free(pivot_pyld);sal_free(l183);return l132;}l132 = 
trie_insert(l206,key,NULL,l32,(trie_node_t*)pivot_pyld);if(SOC_FAILURE(l132))
{trie_node_t*l166 = NULL;(void)trie_delete(l205,key,l32,&l166);sal_free(l197)
;sal_free(pivot_pyld);sal_free(l183);return l132;}index = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,l27);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(index)<<1) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = 
SOC_ALPM_128_ADDR_LWR(index)<<1;VRF_PIVOT_REF_INC(l1,l21,l27);
VRF_TRIE_INIT_DONE(l1,l21,l27,1);return l132;}static int l26(int l1,int l21,
int l27){defip_pair_128_entry_t*lpm_entry;int l204;int l207;int l132 = 
SOC_E_NONE;uint32 key[2] = {0,0},l136[SOC_MAX_MEM_FIELD_WORDS];payload_t*l188
;alpm_pivot_t*l208;trie_node_t*l166;trie_t*l205;trie_t*l206 = NULL;lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l21);l204 = 
soc_L3_DEFIP_PAIR_128m_field32_get(l1,lpm_entry,ALG_BKT_PTRf);l132 = 
soc_th_alpm_bucket_release(l1,l204,l27);_soc_tomahawk_alpm_bkt_view_set(l1,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,l204),INVALIDm);if(SOC_TH_ALPM_V6_SCALE_CHECK
(l1,l27)){_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,
l204+1),INVALIDm);}l132 = l13(l1,lpm_entry,(void*)l136,&l207);if(SOC_FAILURE(
l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n")
,l21,l27));l207 = -1;}l207 = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,
l207,l27);l208 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l207)<<1);l132 = 
soc_th_alpm_128_lpm_delete(l1,lpm_entry);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),l21,l27));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l21) = NULL;l205 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l21);VRF_PREFIX_TRIE_IPV6_128(l1,l21) = NULL;
VRF_TRIE_INIT_DONE(l1,l21,l27,0);l132 = trie_delete(l205,key,0,&l166);l188 = 
(payload_t*)l166;if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),l21,l27));}
sal_free(l188);(void)trie_destroy(l205);l206 = VRF_PIVOT_TRIE_IPV6_128(l1,l21
);VRF_PIVOT_TRIE_IPV6_128(l1,l21) = NULL;l166 = NULL;l132 = trie_delete(l206,
key,0,&l166);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l21,l27));}(void)
trie_destroy(l206);(void)trie_destroy(PIVOT_BUCKET_TRIE(l208));sal_free(
PIVOT_BUCKET_HANDLE(l208));sal_free(l208);return l132;}int
soc_th_alpm_128_insert(int l1,void*l3,uint32 l18,int l209,int l210){
defip_alpm_ipv6_128_entry_t l170,l171;soc_mem_t l17;void*l181,*l192;int l146,
l21;int index;int l5;int l132 = SOC_E_NONE;uint32 l19;l5 = L3_DEFIP_MODE_128;
l17 = L3_DEFIP_ALPM_IPV6_128m;l181 = ((uint32*)&(l170));l192 = ((uint32*)&(
l171));SOC_IF_ERROR_RETURN(l14(l1,l3,l181,l192,l17,l18,&l19));
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l3,&l146,&l21));if(((l146
== SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l146 == SOC_L3_VRF_GLOBAL)))){l132 = l2(l1,l3,&
index);if(SOC_SUCCESS(l132)){VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l5);
VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l5);}else if(l132 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l1,MAX_VRF_ID,l5);}return(l132);}else if(l21 == 0){if(
soc_th_alpm_mode_get(l1)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}}if(l146!= 
SOC_L3_VRF_GLOBAL){if(!soc_th_alpm_mode_get(l1)){if(VRF_TRIE_ROUTES_CNT(l1,
l21,l5) == 0){if(!l19){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
l146));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l21,l5)){
LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d is not ""initialized\n"),l21));l132 = l202(l1,
l21,l5);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),l21,l5));return
l132;}LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init ""completed\n"),l21,l5));}if(l210
&SOC_ALPM_LOOKUP_HIT){l132 = l148(l1,l3,l181,l192,l17,l209);}else{if(l209 == 
-1){l209 = 0;}l132 = l186(l1,l3,l17,l181,l192,&index,
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l209),l210);}if(l132!= SOC_E_NONE){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"unit %d :soc_th_alpm_128_insert: "
"Route Insertion Failed :%s\n"),l1,soc_errmsg(l132)));}return(l132);}int
soc_th_alpm_128_lookup(int l1,void*key_data,void*l10,int*l11,int*l211){
defip_alpm_ipv6_128_entry_t l170;soc_mem_t l17;int bucket_index = 0;int
tcam_index;void*l181;int l146,l21;int l5 = 2,l124;int l132 = SOC_E_NONE;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l146,&l21));l132
= l9(l1,key_data,l10,l11,&l124,&l5);if(SOC_SUCCESS(l132)){SOC_IF_ERROR_RETURN
(soc_th_alpm_128_lpm_vrf_get(l1,l10,&l146,&l21));if(l146 == 
SOC_L3_VRF_OVERRIDE){return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,l21,
l5)){LOG_BSL_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_lookup:VRF %d is not ""initialized\n"),l21));*l11 = 0;*l211 = 0;
return SOC_E_NOT_FOUND;}l17 = L3_DEFIP_ALPM_IPV6_128m;l181 = ((uint32*)&(l170
));SOC_ALPM_LPM_LOCK(l1);l132 = l145(l1,key_data,l17,l181,&tcam_index,&
bucket_index,l11);SOC_ALPM_LPM_UNLOCK(l1);if(SOC_FAILURE(l132)){*l211 = 
tcam_index;*l11 = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,bucket_index);return l132;}
l132 = l20(l1,l181,l17,l5,l146,bucket_index,*l11,l10);*l211 = 
SOC_ALPM_LOOKUP_HIT|tcam_index;return(l132);}int soc_th_alpm_128_delete(int l1
,void*key_data,int l209,int l210){int l146,l21;int l5;int l132 = SOC_E_NONE;
l5 = L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,
key_data,&l146,&l21));if(l146 == SOC_L3_VRF_OVERRIDE){l132 = 
soc_th_alpm_128_lpm_delete(l1,key_data);if(SOC_SUCCESS(l132)){
VRF_TRIE_ROUTES_DEC(l1,MAX_VRF_ID,l5);VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l5);}
return(l132);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l21,l5)){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_th_alpm_128_delete:VRF %d/%d is not "
"initialized\n"),l21,l5));return SOC_E_NONE;}if(l209 == -1){l209 = 0;}l132 = 
l190(l1,key_data,SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l209),l210&~
SOC_ALPM_LOOKUP_HIT,l209);}return(l132);}static void l112(int l1,void*l10,int
index,l107 l113){l113[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l100[(l1)]->l63));l113[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l100[(l1)]->l61));l113[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l100[(l1)]->l67));l113[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l100[(l1)]->l65));l113[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l100[(l1)]->l64));l113[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l100[(l1)]->l62));l113[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l100[(l1)]->l68));l113[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l100[(l1)]->l66));if((!(SOC_IS_HURRICANE(l1)))&&(((l100[(l1)]->l86)!= NULL
))){int l212;(void)soc_th_alpm_128_lpm_vrf_get(l1,l10,(int*)&l113[8],&l212);}
else{l113[8] = 0;};}static int l213(l107 l109,l107 l110){int l207;for(l207 = 
0;l207<9;l207++){{if((l109[l207])<(l110[l207])){return-1;}if((l109[l207])>(
l110[l207])){return 1;}};}return(0);}static void l214(int l1,void*l3,uint32
l215,uint32 l127,int l124){l107 l216;if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l100[(l1)]->l83))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l100[(l1)]->l82))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l100[(l1)]->l81))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l100[(l1)]->l80))){l216[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l100[(l1)]->l63));l216[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l100[(l1)]->l61));l216[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l100[(l1)]->l67));l216[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l100[(l1)]->l65));l216[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l100[(l1)]->l64));l216[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l100[(l1)]->l62));l216[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l100[(l1)]->l68));l216[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l100[(l1)]->l66));if((!(SOC_IS_HURRICANE(l1)))&&(((l100[(l1)]->l86)!= NULL)
)){int l212;(void)soc_th_alpm_128_lpm_vrf_get(l1,l3,(int*)&l216[8],&l212);}
else{l216[8] = 0;};l126((l111[(l1)]),l213,l216,l124,l127,l215);}}static void
l217(int l1,void*key_data,uint32 l215){l107 l216;int l124 = -1;int l132;
uint16 index;l216[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l100[(l1)]->l63));l216[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l100[(l1)]->l61));l216[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l100[(l1)]->l67));l216[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l100[(l1)]->l65));l216[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l100[(l1)]->l64));l216[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l100[(l1)]->l62));l216[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l100[(l1)]->l68));l216[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l100[(l1)]->l66));if((!(SOC_IS_HURRICANE(l1)))&&(((l100[(l1)]->l86
)!= NULL))){int l212;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l216[8],&l212);}else{l216[8] = 0;};index = l215;l132 = l128((l111[(l1)]),l213
,l216,l124,index);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"\ndel  index: H %d error %d\n"),index,l132));}}static int l218(int l1,
void*key_data,int l124,int*l125){l107 l216;int l132;uint16 index = (0xFFFF);
l216[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l100[(l1)]->l63));l216[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l100[(l1)]->l61));l216[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l100[(l1)]->l67));l216[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l100[(l1)]->l65));l216[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l100[(l1)]->l64));l216[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l100[(l1)]->l62));l216[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l100[(l1)]->l68));l216[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l100[(l1)]->l66));if((!(SOC_IS_HURRICANE(l1)))&&(((l100[(l1)]->l86
)!= NULL))){int l212;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l216[8],&l212);}else{l216[8] = 0;};l132 = l121((l111[(l1)]),l213,l216,l124,&
index);if(SOC_FAILURE(l132)){*l125 = 0xFFFFFFFF;return(l132);}*l125 = index;
return(SOC_E_NONE);}static uint16 l114(uint8*l115,int l116){return(
_shr_crc16b(0,l115,l116));}static int l117(int unit,int l102,int l103,l106**
l118){l106*l122;int index;if(l103>l102){return SOC_E_MEMORY;}l122 = sal_alloc
(sizeof(l106),"lpm_hash");if(l122 == NULL){return SOC_E_MEMORY;}sal_memset(
l122,0,sizeof(*l122));l122->unit = unit;l122->l102 = l102;l122->l103 = l103;
l122->l104 = sal_alloc(l122->l103*sizeof(*(l122->l104)),"hash_table");if(l122
->l104 == NULL){sal_free(l122);return SOC_E_MEMORY;}l122->l105 = sal_alloc(
l122->l102*sizeof(*(l122->l105)),"link_table");if(l122->l105 == NULL){
sal_free(l122->l104);sal_free(l122);return SOC_E_MEMORY;}for(index = 0;index<
l122->l103;index++){l122->l104[index] = (0xFFFF);}for(index = 0;index<l122->
l102;index++){l122->l105[index] = (0xFFFF);}*l118 = l122;return SOC_E_NONE;}
static int l119(l106*l120){if(l120!= NULL){sal_free(l120->l104);sal_free(l120
->l105);sal_free(l120);}return SOC_E_NONE;}static int l121(l106*l122,l108 l123
,l107 entry,int l124,uint16*l125){int l1 = l122->unit;uint16 l219;uint16 index
;l219 = l114((uint8*)entry,(32*9))%l122->l103;index = l122->l104[l219];;;
while(index!= (0xFFFF)){uint32 l10[SOC_MAX_MEM_FIELD_WORDS];l107 l113;int l220
;l220 = index;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,
l220,l10));l112(l1,l10,index,l113);if((*l123)(entry,l113) == 0){*l125 = index
;;return(SOC_E_NONE);}index = l122->l105[index&(0x3FFF)];;};return(
SOC_E_NOT_FOUND);}static int l126(l106*l122,l108 l123,l107 entry,int l124,
uint16 l127,uint16 l38){int l1 = l122->unit;uint16 l219;uint16 index;uint16
l221;l219 = l114((uint8*)entry,(32*9))%l122->l103;index = l122->l104[l219];;;
;l221 = (0xFFFF);if(l127!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];l107 l113;int l220;l220 = index;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l220,l10));l112(l1,l10,index,l113);
if((*l123)(entry,l113) == 0){if(l38!= index){;if(l221 == (0xFFFF)){l122->l104
[l219] = l38;l122->l105[l38&(0x3FFF)] = l122->l105[index&(0x3FFF)];l122->l105
[index&(0x3FFF)] = (0xFFFF);}else{l122->l105[l221&(0x3FFF)] = l38;l122->l105[
l38&(0x3FFF)] = l122->l105[index&(0x3FFF)];l122->l105[index&(0x3FFF)] = (
0xFFFF);}};return(SOC_E_NONE);}l221 = index;index = l122->l105[index&(0x3FFF)
];;}}l122->l105[l38&(0x3FFF)] = l122->l104[l219];l122->l104[l219] = l38;
return(SOC_E_NONE);}static int l128(l106*l122,l108 l123,l107 entry,int l124,
uint16 l129){uint16 l219;uint16 index;uint16 l221;l219 = l114((uint8*)entry,(
32*9))%l122->l103;index = l122->l104[l219];;;l221 = (0xFFFF);while(index!= (
0xFFFF)){if(l129 == index){;if(l221 == (0xFFFF)){l122->l104[l219] = l122->
l105[l129&(0x3FFF)];l122->l105[l129&(0x3FFF)] = (0xFFFF);}else{l122->l105[
l221&(0x3FFF)] = l122->l105[l129&(0x3FFF)];l122->l105[l129&(0x3FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l221 = index;index = l122->l105[index&(0x3FFF)];
;}return(SOC_E_NOT_FOUND);}static int l222(int l1,void*l10){return(SOC_E_NONE
);}void soc_th_alpm_128_lpm_state_dump(int l1){int l131;int l223;l223 = ((3*(
128+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){
return;}for(l131 = l223;l131>= 0;l131--){if((l131!= ((3*(128+2+1))-1))&&((l52
[(l1)][(l131)].l45) == -1)){continue;}LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"PFX = %d P = %d N = %d START = %d ""END = %d VENT = %d FENT = %d\n"),l131
,(l52[(l1)][(l131)].l47),(l52[(l1)][(l131)].next),(l52[(l1)][(l131)].l45),(
l52[(l1)][(l131)].l46),(l52[(l1)][(l131)].l48),(l52[(l1)][(l131)].l49)));}
COMPILER_REFERENCE(l222);}static int l224(int l1,int index,uint32*l10){int
l225;uint32 l226,l227,l228;uint32 l229;int l230;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l225 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}else if(SOC_IS_APOLLO(l1)){
l225 = (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l225 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}if(((l100[(l1)]->l55)!= NULL)
){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
l10),(l100[(l1)]->l55),(0));}l226 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l100[(l1)]->l67));l229 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l100[(l1)]->l68));l227 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l100[(l1)]->l65));l228 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l100[(l1)]->l66));l230 = ((!l226)&&(!l229)&&(!l227)&&(!l228))?1:0;l226 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l100[(l1)]->l82));l229 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l100[(l1)]->l80));l227 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l100[(l1)]->l81));l228 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l100[(l1)]->l81));if(l226&&l229&&l227&&l228){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l100[(l1)]->l79),(l230));}return l162(l1,MEM_BLOCK_ANY,index+l225,index,
l10);}static int l231(int l1,int l232,int l233){uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l232,l10));l214(l1,l10,l233,0x4000,0);SOC_IF_ERROR_RETURN(l162(
l1,MEM_BLOCK_ANY,l233,l232,l10));SOC_IF_ERROR_RETURN(l224(l1,l233,l10));do{
int l234,l235;l234 = soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l232),
1);l235 = soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l233),1);
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l235))<<1) = ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l234))<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR
((l235))<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l235
))<<1)) = SOC_ALPM_128_ADDR_LWR((l235))<<1;}ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR((l234))<<1) = NULL;}while(0);return(SOC_E_NONE);}static
int l236(int l1,int l124,int l5){int l232;int l233;l233 = (l52[(l1)][(l124)].
l46)+1;l232 = (l52[(l1)][(l124)].l45);if(l232!= l233){SOC_IF_ERROR_RETURN(
l231(l1,l232,l233));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l5);}(l52[(l1)][(l124)]
.l45)+= 1;(l52[(l1)][(l124)].l46)+= 1;return(SOC_E_NONE);}static int l237(int
l1,int l124,int l5){int l232;int l233;l233 = (l52[(l1)][(l124)].l45)-1;if((
l52[(l1)][(l124)].l48) == 0){(l52[(l1)][(l124)].l45) = l233;(l52[(l1)][(l124)
].l46) = l233-1;return(SOC_E_NONE);}l232 = (l52[(l1)][(l124)].l46);
SOC_IF_ERROR_RETURN(l231(l1,l232,l233));VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l5)
;(l52[(l1)][(l124)].l45)-= 1;(l52[(l1)][(l124)].l46)-= 1;return(SOC_E_NONE);}
static int l238(int l1,int l124,int l5,void*l10,int*l239){int l240;int l241;
int l242;int l243;if((l52[(l1)][(l124)].l48) == 0){l243 = ((3*(128+2+1))-1);
if((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL)||(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)){if(l124<= (((3*(128+2+1
))/3)-1)){l243 = (((3*(128+2+1))/3)-1);}}while((l52[(l1)][(l243)].next)>l124)
{l243 = (l52[(l1)][(l243)].next);}l241 = (l52[(l1)][(l243)].next);if(l241!= -
1){(l52[(l1)][(l241)].l47) = l124;}(l52[(l1)][(l124)].next) = (l52[(l1)][(
l243)].next);(l52[(l1)][(l124)].l47) = l243;(l52[(l1)][(l243)].next) = l124;(
l52[(l1)][(l124)].l49) = ((l52[(l1)][(l243)].l49)+1)/2;(l52[(l1)][(l243)].l49
)-= (l52[(l1)][(l124)].l49);(l52[(l1)][(l124)].l45) = (l52[(l1)][(l243)].l46)
+(l52[(l1)][(l243)].l49)+1;(l52[(l1)][(l124)].l46) = (l52[(l1)][(l124)].l45)-
1;(l52[(l1)][(l124)].l48) = 0;}l242 = l124;while((l52[(l1)][(l242)].l49) == 0
){l242 = (l52[(l1)][(l242)].next);if(l242 == -1){l242 = l124;break;}}while((
l52[(l1)][(l242)].l49) == 0){l242 = (l52[(l1)][(l242)].l47);if(l242 == -1){if
((l52[(l1)][(l124)].l48) == 0){l240 = (l52[(l1)][(l124)].l47);l241 = (l52[(l1
)][(l124)].next);if(-1!= l240){(l52[(l1)][(l240)].next) = l241;}if(-1!= l241)
{(l52[(l1)][(l241)].l47) = l240;}}return(SOC_E_FULL);}}while(l242>l124){l241 = 
(l52[(l1)][(l242)].next);SOC_IF_ERROR_RETURN(l237(l1,l241,l5));(l52[(l1)][(
l242)].l49)-= 1;(l52[(l1)][(l241)].l49)+= 1;l242 = l241;}while(l242<l124){
SOC_IF_ERROR_RETURN(l236(l1,l242,l5));(l52[(l1)][(l242)].l49)-= 1;l240 = (l52
[(l1)][(l242)].l47);(l52[(l1)][(l240)].l49)+= 1;l242 = l240;}(l52[(l1)][(l124
)].l48)+= 1;(l52[(l1)][(l124)].l49)-= 1;(l52[(l1)][(l124)].l46)+= 1;*l239 = (
l52[(l1)][(l124)].l46);sal_memcpy(l10,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);return(
SOC_E_NONE);}static int l244(int l1,int l124,int l5,void*l10,int l245){int
l240;int l241;int l232;int l233;uint32 l246[SOC_MAX_MEM_FIELD_WORDS];int l132
;int l138;l232 = (l52[(l1)][(l124)].l46);l233 = l245;(l52[(l1)][(l124)].l48)
-= 1;(l52[(l1)][(l124)].l49)+= 1;(l52[(l1)][(l124)].l46)-= 1;if(l233!= l232){
if((l132 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l232,l246))<0){return l132
;}l214(l1,l246,l233,0x4000,0);if((l132 = l162(l1,MEM_BLOCK_ANY,l233,l232,l246
))<0){return l132;}if((l132 = l224(l1,l233,l246))<0){return l132;}}l138 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l233,1);l245 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l232,1);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l138)<<1) = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
l245)<<1);if(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l138)<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l138)<<1)) = 
SOC_ALPM_128_ADDR_LWR(l138)<<1;}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l245
)<<1) = NULL;sal_memcpy(l246,soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),
soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);l214(l1,l246,l232,0x4000,0);if(
(l132 = l162(l1,MEM_BLOCK_ANY,l232,l232,l246))<0){return l132;}if((l132 = 
l224(l1,l232,l246))<0){return l132;}if((l52[(l1)][(l124)].l48) == 0){l240 = (
l52[(l1)][(l124)].l47);assert(l240!= -1);l241 = (l52[(l1)][(l124)].next);(l52
[(l1)][(l240)].next) = l241;(l52[(l1)][(l240)].l49)+= (l52[(l1)][(l124)].l49)
;(l52[(l1)][(l124)].l49) = 0;if(l241!= -1){(l52[(l1)][(l241)].l47) = l240;}(
l52[(l1)][(l124)].next) = -1;(l52[(l1)][(l124)].l47) = -1;(l52[(l1)][(l124)].
l45) = -1;(l52[(l1)][(l124)].l46) = -1;}return(l132);}int
soc_th_alpm_128_lpm_vrf_get(int unit,void*lpm_entry,int*l21,int*l247){int l146
;if(((l100[(unit)]->l90)!= NULL)){l146 = soc_L3_DEFIP_PAIR_128m_field32_get(
unit,lpm_entry,VRF_ID_0_LWRf);*l247 = l146;if(
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,VRF_ID_MASK0_LWRf)){*l21 = 
l146;}else if(!soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l100[(unit)]->l92))){*l21 = 
SOC_L3_VRF_GLOBAL;*l247 = SOC_VRF_MAX(unit)+1;}else{*l21 = 
SOC_L3_VRF_OVERRIDE;}}else{*l21 = SOC_L3_VRF_DEFAULT;}return(SOC_E_NONE);}
static int l248(int l1,void*entry,int*l12){int l124=0;int l132;int l146;int
l249;l132 = l130(l1,entry,&l124);if(l132<0){return l132;}l124+= 0;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,entry,&l146,&l132));l249 = 
soc_th_alpm_mode_get(l1);switch(l146){case SOC_L3_VRF_GLOBAL:if((l249 == 
SOC_ALPM_MODE_PARALLEL)||(l249 == SOC_ALPM_MODE_TCAM_ALPM)){*l12 = l124+((3*(
128+2+1))/3);}else{*l12 = l124;}break;case SOC_L3_VRF_OVERRIDE:*l12 = l124+2*
((3*(128+2+1))/3);break;default:if((l249 == SOC_ALPM_MODE_PARALLEL)||(l249 == 
SOC_ALPM_MODE_TCAM_ALPM)){*l12 = l124;}else{*l12 = l124+((3*(128+2+1))/3);}
break;}return(SOC_E_NONE);}static int l9(int l1,void*key_data,void*l10,int*
l11,int*l12,int*l5){int l132;int l125;int l124 = 0;*l5 = L3_DEFIP_MODE_128;
l248(l1,key_data,&l124);*l12 = l124;if(l218(l1,key_data,l124,&l125) == 
SOC_E_NONE){*l11 = l125;if((l132 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(
*l5)?*l11:(*l11>>1),l10))<0){return l132;}return(SOC_E_NONE);}else{return(
SOC_E_NOT_FOUND);}}int soc_th_alpm_128_lpm_init(int l1){int l223;int l131;int
l250;int l251;if(!soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL)
;}l223 = (3*(128+2+1));l251 = sizeof(l50)*(l223);if((l52[(l1)]!= NULL)){
SOC_IF_ERROR_RETURN(soc_th_alpm_128_deinit(l1));}l100[l1] = sal_alloc(sizeof(
l98),"lpm_128_field_state");if(NULL == l100[l1]){return(SOC_E_MEMORY);}(l100[
l1])->l54 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l100[l1])
->l55 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l100[l1])
->l56 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l100[l1])->l57 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l100[l1])->l58 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l100[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l100[l1])->l60 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l100[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l100[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l100[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l100[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l100[l1])->l67 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l100[l1])->
l65 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l100[
l1])->l68 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l100[l1])->l66 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l100[l1])->l71 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l100[l1])->l69 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l100[l1])->l72 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l100[l1])->l70 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l100[l1])->l75 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l100[l1])->l73 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l100[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l100[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l100[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l100[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l100[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l100[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l100[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l100[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l100[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l100[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l100[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l100[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l100[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l100[l1])->l90 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l100[l1])->
l88 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l100[l1
])->l91 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(
l100[l1])->l89 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
VRF_ID_MASK1_UPRf);(l100[l1])->l92 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l100[l1])->l93 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l100[l1])->l94 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l100[l1])->l95 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l100[l1])->l96 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf);(l100[l1])->l97 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_POOL_NUMBERf);(l52[(l1)]
) = sal_alloc(l251,"LPM 128 prefix info");if(NULL == (l52[(l1)])){sal_free(
l100[l1]);l100[l1] = NULL;return(SOC_E_MEMORY);}SOC_ALPM_LPM_LOCK(l1);
sal_memset((l52[(l1)]),0,l251);for(l131 = 0;l131<l223;l131++){(l52[(l1)][(
l131)].l45) = -1;(l52[(l1)][(l131)].l46) = -1;(l52[(l1)][(l131)].l47) = -1;(
l52[(l1)][(l131)].next) = -1;(l52[(l1)][(l131)].l48) = 0;(l52[(l1)][(l131)].
l49) = 0;}l250 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l1)){l250>>= 1;}if((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM
)){(l52[(l1)][(((3*(128+2+1))-1))].l46) = (l250>>1)-1;(l52[(l1)][(((((3*(128+
2+1))/3)-1)))].l49) = l250>>1;(l52[(l1)][((((3*(128+2+1))-1)))].l49) = (l250-
(l52[(l1)][(((((3*(128+2+1))/3)-1)))].l49));}else{(l52[(l1)][((((3*(128+2+1))
-1)))].l49) = l250;}if((l111[(l1)])!= NULL){if(l119((l111[(l1)]))<0){
SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(l111[(l1)]) = NULL;}if(l117(
l1,l250*2,l250,&(l111[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;
}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}int soc_th_alpm_128_lpm_deinit(
int l1){if(!soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}
SOC_ALPM_LPM_LOCK(l1);if((l111[(l1)])!= NULL){l119((l111[(l1)]));(l111[(l1)])
= NULL;}if((l52[(l1)]!= NULL)){sal_free(l100[l1]);l100[l1] = NULL;sal_free((
l52[(l1)]));(l52[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}
static int l2(int l1,void*l3,int*l252){int l124;int index;int l5;uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];int l132 = SOC_E_NONE;int l253 = 0;sal_memcpy(l10,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);SOC_ALPM_LPM_LOCK(l1);l132 = l9(l1,l3,l10,&index,&l124
,&l5);if(l132 == SOC_E_NOT_FOUND){l132 = l238(l1,l124,l5,l10,&index);if(l132<
0){SOC_ALPM_LPM_UNLOCK(l1);return(l132);}}else{l253 = 1;}*l252 = index;if(
l132 == SOC_E_NONE){soc_th_alpm_128_lpm_state_dump(l1);LOG_BSL_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_th_alpm_128_lpm_insert: %d %d\n"),index,
l124));if(!l253){l214(l1,l3,index,0x4000,0);}l132 = l162(l1,MEM_BLOCK_ANY,
index,index,l3);if(l132>= 0){l132 = l224(l1,index,l3);}}SOC_ALPM_LPM_UNLOCK(
l1);return(l132);}static int soc_th_alpm_128_lpm_delete(int l1,void*key_data)
{int l124;int index;int l5;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l132 = 
SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l132 = l9(l1,key_data,l10,&index,&l124,&l5);
if(l132 == SOC_E_NONE){LOG_BSL_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_lpm_delete: %d %d\n"),index,l124));l217(l1,key_data,index);l132 = 
l244(l1,l124,l5,l10,index);}soc_th_alpm_128_lpm_state_dump(l1);
SOC_ALPM_LPM_UNLOCK(l1);return(l132);}static int l13(int l1,void*key_data,
void*l10,int*l11){int l124;int l132;int l5;SOC_ALPM_LPM_LOCK(l1);l132 = l9(l1
,key_data,l10,l11,&l124,&l5);SOC_ALPM_LPM_UNLOCK(l1);return(l132);}static int
l4(int unit,void*key_data,int l5,int l6,int l7,defip_aux_scratch_entry_t*l8){
uint32 l134;uint32 l254[4] = {0,0,0,0};int l124 = 0;int l132 = SOC_E_NONE;
l134 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,VALID0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,VALIDf,l134);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,MODE0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,MODEf,l134);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,ENTRY_TYPE0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,ENTRY_TYPEf,0);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,GLOBAL_ROUTEf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,GLOBAL_ROUTEf,l134);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,ECMPf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,ECMPf,l134);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,ECMP_PTRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,ECMP_PTRf,l134);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,NEXT_HOP_INDEXf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,NEXT_HOP_INDEXf,l134);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,PRIf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,PRIf,l134);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,RPEf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,RPEf,l134);l134 =
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,VRF_ID_0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,VRFf,l134);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,DB_TYPEf,l6);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,DST_DISCARDf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,DST_DISCARDf,l134);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,CLASS_IDf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,CLASS_IDf,l134);l254[0] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR0_LWRf);l254[1] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR1_LWRf);l254[2] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR0_UPRf);l254[3] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR1_UPRf);
soc_mem_field_set(unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)l8,IP_ADDRf,(uint32*)
l254);l132 = l130(unit,key_data,&l124);if(SOC_FAILURE(l132)){return l132;}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,IP_LENGTHf,l124);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,REPLACE_LENf,l7);return(
SOC_E_NONE);}static int l14(int unit,void*lpm_entry,void*l15,void*l16,
soc_mem_t l17,uint32 l18,uint32*l255){uint32 l134;uint32 l254[4];int l124 = 0
;int l132 = SOC_E_NONE;uint32 l19 = 0;sal_memset(l15,0,soc_mem_entry_words(
unit,l17)*4);l134 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,
HITf);soc_mem_field32_set(unit,l17,l15,HITf,l134);l134 = soc_mem_field32_get(
unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf);soc_mem_field32_set(unit,l17,
l15,VALIDf,l134);l134 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry
,ECMPf);soc_mem_field32_set(unit,l17,l15,ECMPf,l134);l134 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf);
soc_mem_field32_set(unit,l17,l15,ECMP_PTRf,l134);l134 = soc_mem_field32_get(
unit,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf);soc_mem_field32_set(unit,
l17,l15,NEXT_HOP_INDEXf,l134);l134 = soc_mem_field32_get(unit,
L3_DEFIP_PAIR_128m,lpm_entry,PRIf);soc_mem_field32_set(unit,l17,l15,PRIf,l134
);l134 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,RPEf);
soc_mem_field32_set(unit,l17,l15,RPEf,l134);l134 = soc_mem_field32_get(unit,
L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf);soc_mem_field32_set(unit,l17,l15,
DST_DISCARDf,l134);l134 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,
lpm_entry,SRC_DISCARDf);soc_mem_field32_set(unit,l17,l15,SRC_DISCARDf,l134);
l134 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf);
soc_mem_field32_set(unit,l17,l15,CLASS_IDf,l134);l254[0] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf);l254[1] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf);l254[2] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf);l254[3] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf);
soc_mem_field_set(unit,l17,(uint32*)l15,KEYf,(uint32*)l254);l132 = l130(unit,
lpm_entry,&l124);if(SOC_FAILURE(l132)){return l132;}if((l124 == 0)&&(l254[0]
== 0)&&(l254[1] == 0)&&(l254[2] == 0)&&(l254[3] == 0)){l19 = 1;}if(l255!= 
NULL){*l255 = l19;}soc_mem_field32_set(unit,l17,l15,LENGTHf,l124);if(l16 == 
NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){sal_memset(l16,0,
soc_mem_entry_words(unit,l17)*4);sal_memcpy(l16,l15,soc_mem_entry_words(unit,
l17)*4);soc_mem_field32_set(unit,l17,l16,DST_DISCARDf,0);soc_mem_field32_set(
unit,l17,l16,RPEf,0);soc_mem_field32_set(unit,l17,l16,SRC_DISCARDf,l18&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(unit,l17,l16,DEFAULTROUTEf,l19)
;}return(SOC_E_NONE);}static int l20(int unit,void*l15,soc_mem_t l17,int l5,
int l21,int l22,int index,void*lpm_entry){uint32 l134;uint32 l254[4];uint32
l146,l256;sal_memset(lpm_entry,0,soc_mem_entry_words(unit,L3_DEFIP_PAIR_128m)
*4);l134 = soc_mem_field32_get(unit,l17,l15,HITf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,HITf,l134);l134 = soc_mem_field32_get(unit,l17,
l15,VALIDf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf
,l134);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_LWRf,l134
);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_UPRf,l134);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_UPRf,l134);l134 = 
soc_mem_field32_get(unit,l17,l15,ECMPf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,ECMPf,l134);l134 = soc_mem_field32_get(unit,l17,
l15,ECMP_PTRf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ECMP_PTRf,l134);l134 = soc_mem_field32_get(unit,l17,l15,NEXT_HOP_INDEXf);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf,l134);
l134 = soc_mem_field32_get(unit,l17,l15,PRIf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,PRIf,l134);l134 = soc_mem_field32_get(unit,l17,
l15,RPEf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,RPEf,l134);
l134 = soc_mem_field32_get(unit,l17,l15,DST_DISCARDf);soc_mem_field32_set(
unit,L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf,l134);l134 = 
soc_mem_field32_get(unit,l17,l15,SRC_DISCARDf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,SRC_DISCARDf,l134);l134 = soc_mem_field32_get(
unit,l17,l15,CLASS_IDf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry
,CLASS_IDf,l134);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ALG_BKT_PTRf,l22);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ALG_HIT_IDXf,index);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
MODE_MASK0_LWRf,3);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
MODE_MASK1_LWRf,3);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
MODE_MASK0_UPRf,3);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
MODE_MASK1_UPRf,3);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ENTRY_TYPE_MASK0_LWRf,1);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,
lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,1);soc_mem_field32_set(
unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf,1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_LWRf,3);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_LWRf,3);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_UPRf,3);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_UPRf,3);
soc_mem_field_get(unit,l17,l15,KEYf,l254);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf,l254[0]);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf,l254[1]);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf,l254[2]);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf,l254[3]);l254[0] = l254[1] = l254[
2] = l254[3] = 0;l134 = soc_mem_field32_get(unit,l17,l15,LENGTHf);l139(unit,
lpm_entry,l134);if(l21 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_HIGHf,1);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_ROUTEf,1);l146 = 0;l256 = 0;}else if(l21
== SOC_L3_VRF_GLOBAL){soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
GLOBAL_ROUTEf,1);l146 = 0;l256 = 0;}else{l146 = l21;l256 = SOC_VRF_MAX(unit);
}soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_LWRf,l146);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_LWRf,l146);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_UPRf,l146);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_UPRf,l146);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_LWRf,l256)
;soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_LWRf,l256
);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_UPRf,
l256);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_UPRf
,l256);return(SOC_E_NONE);}int soc_th_alpm_128_warmboot_pivot_add(int unit,
int l5,void*lpm_entry,int l257,int l258){int l132 = SOC_E_NONE;uint32 key[4] = 
{0,0,0,0};alpm_pivot_t*l31 = NULL;alpm_bucket_handle_t*l183 = NULL;int l146 = 
0,l21 = 0;uint32 l259;trie_t*l206 = NULL;uint32 prefix[5] = {0};int l19 = 0;
l132 = _alpm_128_prefix_create(unit,lpm_entry,prefix,&l259,&l19);
SOC_IF_ERROR_RETURN(l132);SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(
unit,lpm_entry,&l146,&l21));l257 = soc_th_alpm_physical_idx(unit,
L3_DEFIP_PAIR_128m,l257,l5);l183 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l183 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for PIVOT trie node \n")));return SOC_E_NONE;
}sal_memset(l183,0,sizeof(*l183));l31 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l31 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for PIVOT trie node \n")));sal_free(l183);
return SOC_E_MEMORY;}sal_memset(l31,0,sizeof(*l31));PIVOT_BUCKET_HANDLE(l31) = 
l183;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l31));key[0] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR0_LWRf);key[1] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR1_LWRf);key[2] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR0_UPRf);key[3] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR1_UPRf);
PIVOT_BUCKET_INDEX(l31) = l258;PIVOT_TCAM_INDEX(l31) = SOC_ALPM_128_ADDR_LWR(
l257)<<1;if(l146!= SOC_L3_VRF_OVERRIDE){l206 = VRF_PIVOT_TRIE_IPV6_128(unit,
l21);if(l206 == NULL){trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(
unit,l21));l206 = VRF_PIVOT_TRIE_IPV6_128(unit,l21);}sal_memcpy(l31->key,
prefix,sizeof(prefix));l31->len = l259;l132 = trie_insert(l206,l31->key,NULL,
l31->len,(trie_node_t*)l31);if(SOC_FAILURE(l132)){sal_free(l183);sal_free(l31
);return l132;}}ALPM_TCAM_PIVOT(unit,SOC_ALPM_128_ADDR_LWR(l257)<<1) = l31;
PIVOT_BUCKET_VRF(l31) = l21;PIVOT_BUCKET_IPV6(l31) = l5;
PIVOT_BUCKET_ENT_CNT_UPDATE(l31);if(key[0] == 0&&key[1] == 0&&key[2] == 0&&
key[3] == 0){PIVOT_BUCKET_DEF(l31) = TRUE;}VRF_PIVOT_REF_INC(unit,l21,l5);
return l132;}static int l260(int unit,int l5,void*lpm_entry,void*l15,
soc_mem_t l17,int l257,int l258,int l261){int l262;int l21;int l132 = 
SOC_E_NONE;int l19 = 0;uint32 prefix[5] = {0,0,0,0,0};uint32 l32;void*l263 = 
NULL;trie_t*l37 = NULL;trie_t*l30 = NULL;trie_node_t*l166 = NULL;payload_t*
l264 = NULL;payload_t*l189 = NULL;alpm_pivot_t*pivot_pyld = NULL;if((NULL == 
lpm_entry)||(NULL == l15)){return SOC_E_PARAM;}SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&l262,&l21));l17 = 
L3_DEFIP_ALPM_IPV6_128m;l263 = sal_alloc(sizeof(defip_pair_128_entry_t),
"Temp Defip Pair lpm_entry");if(NULL == l263){return SOC_E_MEMORY;}
SOC_IF_ERROR_RETURN(l20(unit,l15,l17,l5,l262,l258,l257,l263));l132 = 
_alpm_128_prefix_create(unit,l263,prefix,&l32,&l19);if(SOC_FAILURE(l132)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"prefix create failed\n")));return
l132;}sal_free(l263);l257 = soc_th_alpm_physical_idx(unit,L3_DEFIP_PAIR_128m,
l257,l5);pivot_pyld = ALPM_TCAM_PIVOT(unit,SOC_ALPM_128_ADDR_LWR(l257)<<1);
l37 = PIVOT_BUCKET_TRIE(pivot_pyld);l264 = sal_alloc(sizeof(payload_t),
"Payload for Key");if(NULL == l264){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}
l189 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == 
l189){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node\n")));sal_free(l264);return
SOC_E_MEMORY;}sal_memset(l264,0,sizeof(*l264));sal_memset(l189,0,sizeof(*l189
));l264->key[0] = prefix[0];l264->key[1] = prefix[1];l264->key[2] = prefix[2]
;l264->key[3] = prefix[3];l264->key[4] = prefix[4];l264->len = l32;l264->
index = l261;sal_memcpy(l189,l264,sizeof(*l264));l132 = trie_insert(l37,
prefix,NULL,l32,(trie_node_t*)l264);if(SOC_FAILURE(l132)){goto l265;}if(l5){
l30 = VRF_PREFIX_TRIE_IPV6_128(unit,l21);}if(!l19){l132 = trie_insert(l30,
prefix,NULL,l32,(trie_node_t*)l189);if(SOC_FAILURE(l132)){goto l266;}}return
l132;l266:(void)trie_delete(l37,prefix,l32,&l166);l264 = (payload_t*)l166;
l265:sal_free(l264);sal_free(l189);return l132;}static int l267(int unit,int
l27,int l21,int l207,int bkt_ptr){int l132 = SOC_E_NONE;uint32 l32;uint32 key
[5] = {0,0,0,0,0};trie_t*l268 = NULL;payload_t*l197 = NULL;
defip_pair_128_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_pair_128_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unable to allocate memory for LPM entry\n")
));return SOC_E_MEMORY;}l23(unit,key,0,l21,l27,lpm_entry,0,1);if(l21 == 
SOC_VRF_MAX(unit)+1){soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
DEFAULT_MISSf,1);}soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
ALG_BKT_PTRf,bkt_ptr);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(unit,l21) = lpm_entry;
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(unit,l21));l268 = 
VRF_PREFIX_TRIE_IPV6_128(unit,l21);l197 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l197 == NULL){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l197,0,sizeof(*l197));l32 = 0;l197->key[0] = key[0];
l197->key[1] = key[1];l197->len = l32;l132 = trie_insert(l268,key,NULL,l32,&(
l197->node));if(SOC_FAILURE(l132)){sal_free(l197);return l132;}
VRF_TRIE_INIT_DONE(unit,l21,l27,1);return l132;}int
soc_th_alpm_128_warmboot_prefix_insert(int unit,int l5,void*lpm_entry,void*
l15,int l257,int l258,int l261){int l262;int l21;int l132 = SOC_E_NONE;
soc_mem_t l17;l17 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&l262,&l21));if(l262 == 
SOC_L3_VRF_OVERRIDE){return(l132);}if(!VRF_TRIE_INIT_COMPLETED(unit,l21,l5)){
LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"soc_alpm_128_insert:VRF %d is not ""initialized\n"),l21));l132 = l267(unit,
l5,l21,l257,l258);if(SOC_FAILURE(l132)){LOG_BSL_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(unit,"VRF %d/%d trie init \n""failed\n"),l21,l5));return l132;}}l132 = l260(
unit,l5,lpm_entry,l15,l17,l257,l258,l261);if(l132!= SOC_E_NONE){LOG_BSL_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unit %d : ""Route Insertion Failed :%s\n"),
unit,soc_errmsg(l132)));return(l132);}VRF_TRIE_ROUTES_INC(unit,l21,l5);return
(l132);}int soc_th_alpm_128_warmboot_bucket_bitmap_set(int l1,int l27,int l209
){int l269 = 1;if(l27){if(!soc_th_alpm_mode_get(l1)&&!SOC_URPF_STATUS_GET(l1)
){l269 = 2;}}if(SOC_ALPM_BUCKET_BMAP(l1) == NULL){return SOC_E_INTERNAL;}
SHR_BITSET_RANGE(SOC_ALPM_BUCKET_BMAP(l1),l209,l269);return SOC_E_NONE;}int
soc_th_alpm_128_warmboot_lpm_reinit_done(int unit){int l207;int l270 = ((3*(
128+2+1))-1);int l250 = soc_mem_index_count(unit,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(unit)){l250>>= 1;}if(!soc_th_alpm_mode_get(unit)){(l52[(
unit)][(((3*(128+2+1))-1))].l47) = -1;for(l207 = ((3*(128+2+1))-1);l207>-1;
l207--){if(-1 == (l52[(unit)][(l207)].l45)){continue;}(l52[(unit)][(l207)].
l47) = l270;(l52[(unit)][(l270)].next) = l207;(l52[(unit)][(l270)].l49) = (
l52[(unit)][(l207)].l45)-(l52[(unit)][(l270)].l46)-1;l270 = l207;}(l52[(unit)
][(l270)].next) = -1;(l52[(unit)][(l270)].l49) = l250-(l52[(unit)][(l270)].
l46)-1;}else{(l52[(unit)][(((3*(128+2+1))-1))].l47) = -1;for(l207 = ((3*(128+
2+1))-1);l207>(((3*(128+2+1))-1)/3);l207--){if(-1 == (l52[(unit)][(l207)].l45
)){continue;}(l52[(unit)][(l207)].l47) = l270;(l52[(unit)][(l270)].next) = 
l207;(l52[(unit)][(l270)].l49) = (l52[(unit)][(l207)].l45)-(l52[(unit)][(l270
)].l46)-1;l270 = l207;}(l52[(unit)][(l270)].next) = -1;(l52[(unit)][(l270)].
l49) = l250-(l52[(unit)][(l270)].l46)-1;l270 = (((3*(128+2+1))-1)/3);(l52[(
unit)][((((3*(128+2+1))-1)/3))].l47) = -1;for(l207 = ((((3*(128+2+1))-1)/3)-1
);l207>-1;l207--){if(-1 == (l52[(unit)][(l207)].l45)){continue;}(l52[(unit)][
(l207)].l47) = l270;(l52[(unit)][(l270)].next) = l207;(l52[(unit)][(l270)].
l49) = (l52[(unit)][(l207)].l45)-(l52[(unit)][(l270)].l46)-1;l270 = l207;}(
l52[(unit)][(l270)].next) = -1;(l52[(unit)][(l270)].l49) = (l250>>1)-(l52[(
unit)][(l270)].l46)-1;}return(SOC_E_NONE);}int
soc_th_alpm_128_warmboot_lpm_reinit(int unit,int l5,int l207,void*lpm_entry){
int l12;l214(unit,lpm_entry,l207,0x4000,0);SOC_IF_ERROR_RETURN(l248(unit,
lpm_entry,&l12));if((l52[(unit)][(l12)].l48) == 0){(l52[(unit)][(l12)].l45) = 
l207;(l52[(unit)][(l12)].l46) = l207;}else{(l52[(unit)][(l12)].l46) = l207;}(
l52[(unit)][(l12)].l48)++;return(SOC_E_NONE);}
#endif

#endif /* ALPM_ENABLE */
