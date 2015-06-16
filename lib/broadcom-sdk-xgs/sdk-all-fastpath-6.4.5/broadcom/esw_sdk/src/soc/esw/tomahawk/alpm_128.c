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
void soc_th_alpm_128_lpm_state_dump(int l1);static int l2(int l1,void*l3,int*
index);static int soc_th_alpm_128_lpm_delete(int l1,void*key_data);static int
l4(int l1,void*key_data,int l5,int l6,int l7,defip_aux_scratch_entry_t*l8);
static int l9(int l1,void*key_data,void*l10,int*l11,int*l12,int*l5);static int
l13(int l1,void*key_data,void*l10,int*l11);static int l14(int unit,void*
lpm_entry,void*l15,void*l16,soc_mem_t l17,uint32 l18,uint32*l19,int l20);
static int l21(int unit,void*l15,soc_mem_t l17,int l5,int l22,int l23,int
index,void*lpm_entry);static int l24(int unit,uint32*key,int len,int l22,int
l5,defip_pair_128_entry_t*lpm_entry,int l25,int l26);static int l27(int l1,
int l22,int l28);static int l29(int l1,alpm_pfx_info_t*l30,trie_t*l31,uint32*
l32,uint32 l33,trie_node_t*l34,defip_pair_128_entry_t*lpm_entry,uint32*l35);
static int l36(int l1,alpm_pfx_info_t*l30,int*l37,int*l11);static int
_soc_th_alpm_128_move_inval(int l1,soc_mem_t l17,alpm_mem_prefix_array_t*l38,
int*l39);extern int soc_th_get_alpm_banks(int unit);extern int
soc_th_alpm_mode_get(int l1);extern int _soc_th_alpm_free_pfx_trie(int l1,
trie_t*l31,trie_t*l40,payload_t*new_pfx_pyld,int*l39,int bktid,int l22,int l28
);extern int _soc_th_alpm_rollback_bkt_move(int l1,void*key_data,soc_mem_t l17
,alpm_pivot_t*l41,alpm_pivot_t*l42,alpm_mem_prefix_array_t*l43,int*l39,int l44
);extern void soc_th_alpm_lpm_move_bu_upd(int l1,int l20,int l45);extern int
soc_th_alpm_lpm_delete(int l1,void*key_data);int alpm_128_split_count;typedef
struct l46{int l47;int l48;int l49;int next;int l50;int l51;}l52,*l53;static
l53 l54[SOC_MAX_NUM_DEVICES];typedef struct l55{soc_field_info_t*l56;
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
soc_field_info_t*l96;soc_field_info_t*l97;soc_field_info_t*l98;
soc_field_info_t*l99;soc_field_info_t*l100;}l101,*l102;static l102 l103[
SOC_MAX_NUM_DEVICES];typedef struct l104{int unit;int l105;int l106;uint16*
l107;uint16*l108;}l109;typedef uint32 l110[9];typedef int(*l111)(l110 l112,
l110 l113);static l109*l114[SOC_MAX_NUM_DEVICES];static void l115(int l1,void
*l10,int index,l110 l116);static uint16 l117(uint8*l118,int l119);static int
l120(int unit,int l105,int l106,l109**l121);static int l122(l109*l123);static
int l124(l109*l125,l111 l126,l110 entry,int l127,uint16*l128);static int l129
(l109*l125,l111 l126,l110 entry,int l127,uint16 l130,uint16 l39);static int
l131(l109*l125,l111 l126,l110 entry,int l127,uint16 l132);static int l133(int
l1,const void*entry,int*l127){int l134,l135;int l136[4] = {IP_ADDR_MASK0_LWRf
,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};uint32 l137;l137 = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l136[0]);if((l135 = 
_ipmask2pfx(l137,l127))<0){return(l135);}for(l134 = 1;l134<4;l134++){l137 = 
soc_mem_field32_get(l1,L3_DEFIP_PAIR_128m,entry,l136[l134]);if(*l127){if(l137
!= 0xffffffff){return(SOC_E_PARAM);}*l127+= 32;}else{if((l135 = _ipmask2pfx(
l137,l127))<0){return(l135);}}}return SOC_E_NONE;}static void l138(uint32*
l139,int l33,int l28){uint32 l140,l141,l47,prefix[6];int l134;sal_memcpy(
prefix,l139,sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(l139,0,
sizeof(uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));l140 = 128-l33;l47 = (l140+31)/
32;if((l140%32) == 0){l47++;}l140 = l140%32;for(l134 = l47;l134<= 4;l134++){
prefix[l134]<<= l140;l141 = prefix[l134+1]&~(0xffffffff>>l140);l141 = (((32-
l140) == 32)?0:(l141)>>(32-l140));if(l134<4){prefix[l134]|= l141;}}for(l134 = 
l47;l134<= 4;l134++){l139[3-(l134-l47)] = prefix[l134];}}static void l142(int
unit,void*lpm_entry,int l12){int l134;soc_field_t l143[4] = {
IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};
for(l134 = 0;l134<4;l134++){soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,
lpm_entry,l143[l134],0);}for(l134 = 0;l134<4;l134++){if(l12<= 32)break;
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l143[3-l134],0xffffffff
);l12-= 32;}soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l143[3-l134
],~(((l12) == 32)?0:(0xffffffff)>>(l12)));}int _alpm_128_prefix_create(int l1
,void*entry,uint32*l139,uint32*l12,int*l19){int l134;int l127 = 0,l47;int l135
= SOC_E_NONE;uint32 l140,l141;uint32 prefix[5];sal_memset(l139,0,sizeof(
uint32)*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(prefix,0,sizeof(uint32)*
BITS2WORDS(_MAX_KEY_LEN_144_));prefix[0] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR0_LWRf);prefix[1] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR1_LWRf);prefix[2] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR0_UPRf);prefix[3] = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,IP_ADDR1_UPRf);l135 = l133(l1,entry,&l127);if(
SOC_FAILURE(l135)){return l135;}l140 = 128-l127;l47 = l140/32;l140 = l140%32;
for(l134 = l47;l134<4;l134++){prefix[l134]>>= l140;l141 = prefix[l134+1]&((1
<<l140)-1);l141 = (((32-l140) == 32)?0:(l141)<<(32-l140));prefix[l134]|= l141
;}for(l134 = l47;l134<4;l134++){l139[4-(l134-l47)] = prefix[l134];}*l12 = 
l127;if(l19!= NULL){*l19 = (prefix[0] == 0)&&(prefix[1] == 0)&&(prefix[2] == 
0)&&(prefix[3] == 0)&&(l127 == 0);}return SOC_E_NONE;}int l144(int l1,uint32*
prefix,uint32 l33,int l5,int l22,int*l145,int*tcam_index,int*bktid){int l135 = 
SOC_E_NONE;trie_t*l146;trie_node_t*l147 = NULL;alpm_pivot_t*pivot_pyld;l146 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l22);l135 = trie_find_lpm(l146,prefix,l33,&l147);
if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l135;}pivot_pyld = (alpm_pivot_t*)l147;*l145 = 
1;*tcam_index = PIVOT_TCAM_INDEX(pivot_pyld);*bktid = PIVOT_BUCKET_INDEX(
pivot_pyld);return SOC_E_NONE;}static int _soc_th_alpm_128_find(int l1,void*
key_data,soc_mem_t l17,void*alpm_data,int*tcam_index,int*bktid,int*l11,int
l148){uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l149,l22,l28;int l128;uint32 l6
,l150;int l135 = SOC_E_NONE;int l145 = 0;l28 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l149,&l22));if(
l149 == 0){if(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL||
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){return SOC_E_PARAM;}}
soc_th_alpm_bank_db_type_get(l1,l22,&l150,&l6);if(!(((l149 == 
SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)
&&(l149 == SOC_L3_VRF_GLOBAL))))){if(l148){uint32 prefix[5],l33;int l19 = 0;
l135 = _alpm_128_prefix_create(l1,key_data,prefix,&l33,&l19);if(SOC_FAILURE(
l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l135;}l135 = l144(l1,
prefix,l33,l28,l22,&l145,tcam_index,bktid);SOC_IF_ERROR_RETURN(l135);}else{
defip_aux_scratch_entry_t l8;sal_memset(&l8,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l28,l6,0,&l8))
;SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,PREFIX_LOOKUP,&l8,TRUE,&l145,
tcam_index,bktid));}if(l145){l14(l1,key_data,l10,0,l17,0,0,*bktid);l135 = 
_soc_th_alpm_find_in_bkt(l1,l17,*bktid,l150,l10,alpm_data,&l128,l28);if(
SOC_SUCCESS(l135)){*l11 = l128;}}else{l135 = SOC_E_NOT_FOUND;}}return l135;}
static int l151(int l1,void*key_data,void*alpm_data,void*alpm_sip_data,
soc_mem_t l17,int l128){defip_aux_scratch_entry_t l8;int l149,l28,l22;int
bktid;uint32 l6,l150;int l135 = SOC_E_NONE;int l145 = 0,l141 = 0;int
tcam_index,index;uint32 l152[SOC_MAX_MEM_FIELD_WORDS];l28 = L3_DEFIP_MODE_128
;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l149,&l22));
soc_th_alpm_bank_db_type_get(l1,l22,&l150,&l6);if(soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_COMBINED||soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){
l6 = 2;}if(!(((l149 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l149 == SOC_L3_VRF_GLOBAL))))){sal_memset(&l8,0,
sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l28,l6,
0,&l8));SOC_ALPM_LPM_LOCK(l1);l135 = _soc_th_alpm_128_find(l1,key_data,l17,
l152,&tcam_index,&bktid,&index,TRUE);SOC_ALPM_LPM_UNLOCK(l1);
SOC_IF_ERROR_RETURN(l135);soc_mem_field32_set(l1,l17,alpm_data,SUB_BKT_PTRf,
ALPM_BKT_SIDX(bktid));SOC_IF_ERROR_RETURN(soc_mem_write(l1,l17,MEM_BLOCK_ANY,
ALPM_ENT_INDEX(l128),alpm_data));if(SOC_URPF_STATUS_GET(l1)){
soc_mem_field32_set(l1,l17,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry
(l1,ALPM_ENT_INDEX(l128)),alpm_sip_data));}l141 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,REPLACE_LENf,l141);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l8,TRUE,&l145,&tcam_index,&bktid));
if(SOC_URPF_STATUS_GET(l1)){if(l141 == 0){soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,0);}l141 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf);l141+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf,l141);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l8,TRUE,&l145,&tcam_index,&bktid));}
}return l135;}static int l153(int l1,int l37,int l35){int l135,l141,l154,l155
;defip_aux_table_entry_t l156,l157;l154 = SOC_ALPM_128_ADDR_LWR(l37);l155 = 
SOC_ALPM_128_ADDR_UPR(l37);l135 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l154,&l156);SOC_IF_ERROR_RETURN(l135);l135 = soc_mem_read(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l155,&l157);SOC_IF_ERROR_RETURN(l135);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH0f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH1f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH0f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH1f,l35);l141 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&l156,DB_TYPE0f);l135 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l154,&l156);
SOC_IF_ERROR_RETURN(l135);l135 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l155,&l157);SOC_IF_ERROR_RETURN(l135);if(SOC_URPF_STATUS_GET(l1
)){l141++;soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH0f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,BPM_LENGTH1f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH0f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,BPM_LENGTH1f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,DB_TYPE0f,l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l156,DB_TYPE1f,l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,DB_TYPE0f,l141);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l157,DB_TYPE1f,l141);l154+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m))/2;l155+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m))/2;l135 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l154,&l156);SOC_IF_ERROR_RETURN(l135);l135 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l155,&l157);}return l135;}
static int l158(int l1,int l159,void*entry,defip_aux_table_entry_t*l160,int
l161){uint32 l141,l6,l162 = 0;soc_mem_t l17 = L3_DEFIP_PAIR_128m;soc_mem_t
l163 = L3_DEFIP_AUX_TABLEm;int l135 = SOC_E_NONE,l127,l22;void*l164,*l165;
l164 = (void*)l160;l165 = (void*)(l160+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1
,l163,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l159),l160));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l163,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l159),l160+1));l141 = 
soc_mem_field32_get(l1,l17,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l163,
l164,VRF0f,l141);l141 = soc_mem_field32_get(l1,l17,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l163,l164,VRF1f,l141);l141 = soc_mem_field32_get(l1,
l17,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l163,l165,VRF0f,l141);l141 = 
soc_mem_field32_get(l1,l17,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l163,
l165,VRF1f,l141);l141 = soc_mem_field32_get(l1,l17,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l163,l164,MODE0f,l141);l141 = soc_mem_field32_get(l1,
l17,entry,MODE1_LWRf);soc_mem_field32_set(l1,l163,l164,MODE1f,l141);l141 = 
soc_mem_field32_get(l1,l17,entry,MODE0_UPRf);soc_mem_field32_set(l1,l163,l165
,MODE0f,l141);l141 = soc_mem_field32_get(l1,l17,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l163,l165,MODE1f,l141);l141 = soc_mem_field32_get(l1,
l17,entry,VALID0_LWRf);soc_mem_field32_set(l1,l163,l164,VALID0f,l141);l141 = 
soc_mem_field32_get(l1,l17,entry,VALID1_LWRf);soc_mem_field32_set(l1,l163,
l164,VALID1f,l141);l141 = soc_mem_field32_get(l1,l17,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l163,l165,VALID0f,l141);l141 = soc_mem_field32_get(l1,
l17,entry,VALID1_UPRf);soc_mem_field32_set(l1,l163,l165,VALID1f,l141);l135 = 
l133(l1,entry,&l127);SOC_IF_ERROR_RETURN(l135);soc_mem_field32_set(l1,l163,
l164,IP_LENGTH0f,l127);soc_mem_field32_set(l1,l163,l164,IP_LENGTH1f,l127);
soc_mem_field32_set(l1,l163,l165,IP_LENGTH0f,l127);soc_mem_field32_set(l1,
l163,l165,IP_LENGTH1f,l127);l141 = soc_mem_field32_get(l1,l17,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l163,l164,IP_ADDR0f,l141);l141 = 
soc_mem_field32_get(l1,l17,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l163,
l164,IP_ADDR1f,l141);l141 = soc_mem_field32_get(l1,l17,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l163,l165,IP_ADDR0f,l141);l141 = soc_mem_field32_get(
l1,l17,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l163,l165,IP_ADDR1f,l141);
l141 = soc_mem_field32_get(l1,l17,entry,ENTRY_TYPE0_LWRf);soc_mem_field32_set
(l1,l163,l164,ENTRY_TYPE0f,l141);l141 = soc_mem_field32_get(l1,l17,entry,
ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,l163,l164,ENTRY_TYPE1f,l141);l141 = 
soc_mem_field32_get(l1,l17,entry,ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,
l163,l165,ENTRY_TYPE0f,l141);l141 = soc_mem_field32_get(l1,l17,entry,
ENTRY_TYPE1_UPRf);soc_mem_field32_set(l1,l163,l165,ENTRY_TYPE1f,l141);l135 = 
soc_th_alpm_128_lpm_vrf_get(l1,entry,&l22,&l127);SOC_IF_ERROR_RETURN(l135);if
(SOC_URPF_STATUS_GET(l1)){if(l161>= (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1)){l162 = 1;}}switch(l22){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l163,l164,VALID0f,0);soc_mem_field32_set(l1,l163,l164,
VALID1f,0);soc_mem_field32_set(l1,l163,l165,VALID0f,0);soc_mem_field32_set(l1
,l163,l165,VALID1f,0);l6 = 0;break;case SOC_L3_VRF_GLOBAL:l6 = l162?1:0;break
;default:l6 = l162?3:2;break;}soc_mem_field32_set(l1,l163,l164,DB_TYPE0f,l6);
soc_mem_field32_set(l1,l163,l164,DB_TYPE1f,l6);soc_mem_field32_set(l1,l163,
l165,DB_TYPE0f,l6);soc_mem_field32_set(l1,l163,l165,DB_TYPE1f,l6);if(l162){
l141 = soc_mem_field32_get(l1,l17,entry,ALG_BKT_PTRf);if(l141){l141+= 
SOC_TH_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l17,entry,ALG_BKT_PTRf,
l141);}l141 = soc_mem_field32_get(l1,l17,entry,ALG_SUB_BKT_PTRf);if(l141){
soc_mem_field32_set(l1,l17,entry,ALG_SUB_BKT_PTRf,l141);}}return SOC_E_NONE;}
static int l166(int l1,int l167,int index,int l168,void*entry){
defip_aux_table_entry_t l160[2];l168 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l168,1);SOC_IF_ERROR_RETURN(l158(l1,l168,entry,&l160[0],
index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,
entry));index = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(index),l160));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(index),l160+
1));return SOC_E_NONE;}static int l169(int l1,int l28,int l45,int bktid){int
l135 = SOC_E_NONE;int l170;soc_mem_t l171 = L3_DEFIP_PAIR_128m;
defip_pair_128_entry_t lpm_entry;int l172,l173;l172 = ALPM_BKT_IDX(bktid);
l173 = ALPM_BKT_SIDX(bktid);l170 = soc_th_alpm_logical_idx(l1,l171,
SOC_ALPM_128_DEFIP_TO_PAIR(l45>>1),1);l135 = soc_mem_read(l1,l171,
MEM_BLOCK_ANY,l170,&lpm_entry);SOC_IF_ERROR_RETURN(l135);soc_mem_field32_set(
l1,l171,&lpm_entry,ALG_BKT_PTRf,l172);soc_mem_field32_set(l1,l171,&lpm_entry,
ALG_SUB_BKT_PTRf,l173);l135 = soc_mem_write(l1,l171,MEM_BLOCK_ANY,l170,&
lpm_entry);SOC_IF_ERROR_RETURN(l135);if(SOC_URPF_STATUS_GET(l1)){int l174 = (
soc_mem_index_count(l1,l171)>>1)+soc_th_alpm_logical_idx(l1,l171,
SOC_ALPM_128_DEFIP_TO_PAIR(l45>>1),1);l135 = READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l174,&lpm_entry);SOC_IF_ERROR_RETURN(l135);soc_mem_field32_set(
l1,l171,&lpm_entry,ALG_BKT_PTRf,l172+SOC_TH_ALPM_BUCKET_COUNT(l1));
soc_mem_field32_set(l1,l171,&lpm_entry,ALG_SUB_BKT_PTRf,l173);l135 = 
WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l174,&lpm_entry);
SOC_IF_ERROR_RETURN(l135);}return l135;}int _soc_th_alpm_128_move_trie(int l1
,int l28,int l175,int l176){int*l39 = NULL,*l130 = NULL;int l135 = SOC_E_NONE
,l177,l134,l178;int l128,l45;uint32 l150 = 0;soc_mem_t l17;void*l179,*l180;
uint32 l10[SOC_MAX_MEM_FIELD_WORDS];trie_t*trie = NULL;payload_t*l127 = NULL;
alpm_mem_prefix_array_t*l38 = NULL;defip_alpm_ipv6_128_entry_t l181,l182;int
l183,l184,l185,l186;l178 = sizeof(int)*MAX_PREFIX_PER_BUCKET;l39 = sal_alloc(
l178,"new_index_move");l130 = sal_alloc(l178,"old_index_move");l38 = 
sal_alloc(sizeof(alpm_mem_prefix_array_t),"prefix_array");if(l39 == NULL||
l130 == NULL||l38 == NULL){l135 = SOC_E_MEMORY;goto l187;}l183 = ALPM_BKT_IDX
(l175);l185 = ALPM_BKT_SIDX(l175);l184 = ALPM_BKT_IDX(l176);l186 = 
ALPM_BKT_SIDX(l176);l17 = L3_DEFIP_ALPM_IPV6_128m;l179 = &l181;l180 = &l182;
l45 = SOC_ALPM_BS_BKT_USAGE_PIVOT(l1,l175);trie = PIVOT_BUCKET_TRIE(
ALPM_TCAM_PIVOT(l1,l45));soc_th_alpm_bank_db_type_get(l1,
SOC_ALPM_BS_BKT_USAGE_VRF(l1,l175),&l150,NULL);sal_memset(l38,0,sizeof(*l38))
;l135 = trie_traverse(trie,_soc_th_alpm_mem_prefix_array_cb,l38,
_TRIE_INORDER_TRAVERSE);do{if((l135)<0){goto l187;}}while(0);sal_memset(l39,-
1,l178);sal_memset(l130,-1,l178);for(l134 = 0;l134<l38->count;l134++){l127 = 
l38->prefix[l134];if(l127->index>0){l135 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,
l127->index,l179);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"soc_mem_read index %d failed\n"),l127->index));goto l188;}
soc_mem_field32_set(l1,l17,l179,SUB_BKT_PTRf,l186);if(SOC_URPF_STATUS_GET(l1)
){l135 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l127->
index),l180);if(SOC_FAILURE(l135)){goto l188;}soc_mem_field32_set(l1,l17,l180
,SUB_BKT_PTRf,l186);}l135 = _soc_th_alpm_insert_in_bkt(l1,l17,l176,l150,l179,
l10,&l128,l28);if(SOC_SUCCESS(l135)){if(SOC_URPF_STATUS_GET(l1)){l135 = 
soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l128),l180);if(
SOC_FAILURE(l135)){goto l188;}}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1
,"insert to bucket %d failed\n"),l176));goto l188;}l39[l134] = l128;l130[l134
] = l127->index;}}l135 = l169(l1,l28,l45,l176);if(SOC_FAILURE(l135)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_move_relink failed, pivot %d bkt %d\n"),l45,l176));goto l188
;}l135 = _soc_th_alpm_128_move_inval(l1,l17,l38,l39);if(SOC_FAILURE(l135)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_move_inval failed\n")));goto l188;}l135 = 
soc_th_alpm_update_hit_bits(l1,l38->count,l130,l39);if(SOC_FAILURE(l135)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l38->count));l135 = 
SOC_E_NONE;}PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l1,l45)) = ALPM_BKTID(l184,
l186);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l1,l28,l183);
SOC_ALPM_BS_BKT_USAGE_SB_DEL(l1,l183,l185,TRUE,l38->count);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l1,l28,l183);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l1,l28,l184);SOC_ALPM_BS_BKT_USAGE_SB_ADD(l1
,l184,l186,l45,-1,l38->count);SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l1,l28,l184);
l188:if(SOC_FAILURE(l135)){l177 = l169(l1,l28,l45,l175);do{if((l177)<0){goto
l187;}}while(0);for(l134 = 0;l134<l38->count;l134++){if(l39[l134] == -1){
continue;}l177 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,l39[l134],
soc_mem_entry_null(l1,l17));do{if((l177)<0){goto l187;}}while(0);if(
SOC_URPF_STATUS_GET(l1)){l177 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l39[l134]),soc_mem_entry_null(l1,l17));do{if((l177)
<0){goto l187;}}while(0);}}}l187:if(l39!= NULL){sal_free(l39);}if(l130!= NULL
){sal_free(l130);}if(l38!= NULL){sal_free(l38);}return l135;}static void l189
(int l1,defip_pair_128_entry_t*l190,void*key_data,int tcam_index,alpm_pivot_t
*pivot_pyld){int l135;trie_t*l146 = NULL;int l28,l22,l149;trie_node_t*l191 = 
NULL;l28 = L3_DEFIP_MODE_128;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,&
l149,&l22);l135 = soc_th_alpm_128_lpm_delete(l1,l190);if(SOC_FAILURE(l135)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Failure to free new prefix""at %d\n"),
soc_th_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,tcam_index,l28)));}l146 = 
VRF_PIVOT_TRIE_IPV6(l1,l22);if(ALPM_TCAM_PIVOT(l1,tcam_index<<(l28?1:0))!= 
NULL){l135 = trie_delete(l146,pivot_pyld->key,pivot_pyld->len,&l191);if(
SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l1,tcam_index<<(l28?1:0)) = NULL;VRF_PIVOT_REF_DEC(l1,l22,l28
);}static int _soc_th_alpm_128_move_inval(int l1,soc_mem_t l17,
alpm_mem_prefix_array_t*l38,int*l39){int l134,l135 = SOC_E_NONE,l177;
defip_alpm_ipv6_128_entry_t l181;void*l192 = NULL,*l193 = NULL;int l194;int*
l195 = NULL;int l196 = FALSE;l194 = sizeof(l181);l192 = sal_alloc(l194*l38->
count,"rb_bufp");l193 = sal_alloc(l194*l38->count,"rb_sip_bufp");l195 = 
sal_alloc(sizeof(*l195)*l38->count,"roll_back_index");if(l192 == NULL||l193 == 
NULL||l195 == NULL){l135 = SOC_E_MEMORY;goto l197;}sal_memset(l195,-1,sizeof(
*l195)*l38->count);for(l134 = 0;l134<l38->count;l134++){payload_t*prefix = 
l38->prefix[l134];if(prefix->index>= 0){l135 = soc_mem_read(l1,l17,
MEM_BLOCK_ANY,prefix->index,(uint8*)l192+l134*l194);if(SOC_FAILURE(l135)){
l134--;l196 = TRUE;break;}if(SOC_URPF_STATUS_GET(l1)){l135 = soc_mem_read(l1,
l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,prefix->index),(uint8*)l193+l134*
l194);if(SOC_FAILURE(l135)){l134--;l196 = TRUE;break;}}l135 = soc_mem_write(
l1,l17,MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null(l1,l17));if(SOC_FAILURE
(l135)){l195[l134] = prefix->index;l196 = TRUE;break;}if(SOC_URPF_STATUS_GET(
l1)){l135 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,
prefix->index),soc_mem_entry_null(l1,l17));if(SOC_FAILURE(l135)){l195[l134] = 
prefix->index;l196 = TRUE;break;}}}l195[l134] = prefix->index;prefix->index = 
l39[l134];}if(l196){for(;l134>= 0;l134--){payload_t*prefix = l38->prefix[l134
];prefix->index = l195[l134];if(l195[l134]<0){continue;}l177 = soc_mem_write(
l1,l17,MEM_BLOCK_ALL,l195[l134],(uint8*)l192+l134*l194);if(SOC_FAILURE(l177))
{break;}if(!SOC_URPF_STATUS_GET(l1)){continue;}l177 = soc_mem_write(l1,l17,
MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(l1,l195[l134]),(uint8*)l193+l134*l194);
if(SOC_FAILURE(l177)){break;}}}l197:if(l195!= NULL){sal_free(l195);}if(l193!= 
NULL){sal_free(l193);}if(l192!= NULL){sal_free(l192);}return l135;}static int
l29(int l1,alpm_pfx_info_t*l30,trie_t*l31,uint32*l32,uint32 l33,trie_node_t*
l34,defip_pair_128_entry_t*lpm_entry,uint32*l35){trie_node_t*l147 = NULL;int
l28,l22,l149;defip_alpm_ipv6_128_entry_t l181;payload_t*l198 = NULL;int l199;
void*l179;alpm_pivot_t*l200;int l135 = SOC_E_NONE;soc_mem_t l17;
alpm_bucket_handle_t*l201;l200 = l30->pivot_pyld;l199 = l200->tcam_index;l28 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l30->
key_data,&l149,&l22));l17 = L3_DEFIP_ALPM_IPV6_128m;l179 = ((uint32*)&(l181))
;l147 = NULL;l135 = trie_find_lpm(l31,l32,l33,&l147);l198 = (payload_t*)l147;
if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l32[0],l32[1],l32[2],
l32[3],l32[4],l33));return l135;}if(l198->bkt_ptr){if(l198->bkt_ptr == l30->
new_pfx_pyld){sal_memcpy(l179,l30->alpm_data,sizeof(
defip_alpm_ipv6_128_entry_t));}else{l135 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,
((payload_t*)l198->bkt_ptr)->index,l179);}if(SOC_FAILURE(l135)){return l135;}
l135 = l21(l1,l179,l17,l28,l149,l30->bktid,0,lpm_entry);if(SOC_FAILURE(l135))
{return l135;}*l35 = ((payload_t*)(l198->bkt_ptr))->len;}else{l135 = 
soc_mem_read(l1,L3_DEFIP_PAIR_128m,MEM_BLOCK_ANY,soc_th_alpm_logical_idx(l1,
L3_DEFIP_PAIR_128m,SOC_ALPM_128_DEFIP_TO_PAIR(l199>>1),1),lpm_entry);}l201 = 
sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l201 == NULL)
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l135 = SOC_E_MEMORY;return l135;}sal_memset(l201,0,sizeof(*l201));l200 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l200 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l135 = SOC_E_MEMORY;return l135;}sal_memset(l200,0,sizeof(*l200));
PIVOT_BUCKET_HANDLE(l200) = l201;l135 = trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(l200));PIVOT_BUCKET_TRIE(l200)->trie = l34;
PIVOT_BUCKET_INDEX(l200) = l30->bktid;PIVOT_BUCKET_VRF(l200) = l22;
PIVOT_BUCKET_IPV6(l200) = l28;PIVOT_BUCKET_DEF(l200) = FALSE;(l200)->key[0] = 
l32[0];(l200)->key[1] = l32[1];(l200)->key[2] = l32[2];(l200)->key[3] = l32[3
];(l200)->key[4] = l32[4];(l200)->len = l33;l138((l32),(l33),(l28));l24(l1,
l32,l33,l22,l28,lpm_entry,0,0);soc_L3_DEFIP_PAIR_128m_field32_set(l1,
lpm_entry,ALG_BKT_PTRf,ALPM_BKT_IDX(l30->bktid));
soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,ALG_SUB_BKT_PTRf,
ALPM_BKT_SIDX(l30->bktid));l30->pivot_pyld = l200;return l135;}static int l36
(int l1,alpm_pfx_info_t*l30,int*l37,int*l11){trie_node_t*l34;uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];int l28,l22,l149;uint32 l33,l35 = 0;uint32 l150 = 0;
uint32 l32[5];int l128;defip_pair_128_entry_t l181,l182;trie_t*l31,*trie;void
*l179,*l180;alpm_pivot_t*l202 = l30->pivot_pyld;defip_pair_128_entry_t
lpm_entry;soc_mem_t l17;trie_t*l146 = NULL;alpm_mem_prefix_array_t l203;int*
l39 = NULL;int*l130 = NULL;int l135 = SOC_E_NONE,l134,l44 = -1;int tcam_index
,l204,l205 = 0;int l206 = 0,l207 = 0;l28 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l30->key_data,&l149,&l22))
;l17 = L3_DEFIP_ALPM_IPV6_128m;l179 = ((uint32*)&(l181));l180 = ((uint32*)&(
l182));soc_th_alpm_bank_db_type_get(l1,l22,&l150,NULL);l31 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l22);trie = PIVOT_BUCKET_TRIE(l30->pivot_pyld);
l205 = l30->bktid;l204 = PIVOT_TCAM_INDEX(l202);l135 = soc_th_alpm_bs_alloc(
l1,&l30->bktid,l22,l28);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Unable to allocate""new bucket for split\n")
));l30->bktid = -1;_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->new_pfx_pyld,
l39,l30->bktid,l22,l28);return l135;}l135 = trie_split(trie,_MAX_KEY_LEN_144_
,FALSE,l32,&l33,&l34,NULL,FALSE);if(SOC_FAILURE(l135)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Could not split bucket")));
_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->new_pfx_pyld,l39,l30->bktid,l22,
l28);return l135;}l135 = l29(l1,l30,l31,l32,l33,l34,&lpm_entry,&l35);if(l135
!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"could not initialize pivot")));_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->
new_pfx_pyld,l39,l30->bktid,l22,l28);return l135;}sal_memset(&l203,0,sizeof(
l203));l135 = trie_traverse(PIVOT_BUCKET_TRIE(l30->pivot_pyld),
_soc_th_alpm_mem_prefix_array_cb,&l203,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE
(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Bucket split failed")));_soc_th_alpm_free_pfx_trie(l1,l31,
trie,l30->new_pfx_pyld,l39,l30->bktid,l22,l28);return l135;}l39 = sal_alloc(
sizeof(*l39)*l203.count,"new_index");if(l39 == NULL){l135 = SOC_E_MEMORY;
_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->new_pfx_pyld,l39,l30->bktid,l22,
l28);return l135;}l130 = sal_alloc(sizeof(*l130)*l203.count,"old_index");if(
l130 == NULL){l135 = SOC_E_MEMORY;_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30
->new_pfx_pyld,l130,l30->bktid,l22,l28);return l135;}sal_memset(l39,-1,sizeof
(*l39)*l203.count);sal_memset(l130,-1,sizeof(*l130)*l203.count);for(l134 = 0;
l134<l203.count;l134++){payload_t*l127 = l203.prefix[l134];if(l127->index>0){
l135 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,l127->index,l179);if(SOC_FAILURE(
l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Failed to ""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l203.prefix[l134]->key[1],l203.
prefix[l134]->key[2],l203.prefix[l134]->key[3],l203.prefix[l134]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,l30->key_data,l17,l202,l30->pivot_pyld
,&l203,l39,l44);sal_free(l130);return l135;}if(SOC_URPF_STATUS_GET(l1)){l135 = 
soc_mem_read(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l127->index),l180
);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l203.prefix[l134]->key[1],l203.
prefix[l134]->key[2],l203.prefix[l134]->key[3],l203.prefix[l134]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,l30->key_data,l17,l202,l30->pivot_pyld
,&l203,l39,l44);sal_free(l130);return l135;}}soc_mem_field32_set(l1,l17,l179,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l30->bktid));soc_mem_field32_set(l1,l17,l180,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l30->bktid));l135 = _soc_th_alpm_insert_in_bkt(l1,
l17,l30->bktid,l150,l179,l10,&l128,l28);if(SOC_SUCCESS(l135)){if(
SOC_URPF_STATUS_GET(l1)){l135 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l128),l180);}l206++;l207++;}}else{
soc_mem_field32_set(l1,l17,l30->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(l30->
bktid));l135 = _soc_th_alpm_insert_in_bkt(l1,l17,l30->bktid,l150,l30->
alpm_data,l10,&l128,l28);if(SOC_SUCCESS(l135)){l44 = l134;*l11 = l128;if(
SOC_URPF_STATUS_GET(l1)){soc_mem_field32_set(l1,l17,l30->alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l30->bktid));l135 = soc_mem_write(l1,l17,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l128),l30->alpm_sip_data);}l207++;}}
l39[l134] = l128;if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l203.prefix[l134]->key[1],l203.
prefix[l134]->key[2],l203.prefix[l134]->key[3],l203.prefix[l134]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,l30->key_data,l17,l202,l30->pivot_pyld
,&l203,l39,l44);sal_free(l130);return l135;}l130[l134] = l127->index;}l135 = 
l2(l1,&lpm_entry,l37);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Unable to add new""pivot to tcam\n")));if(
l135 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l22,l28);}(void)
_soc_th_alpm_rollback_bkt_move(l1,l30->key_data,l17,l202,l30->pivot_pyld,&
l203,l39,l44);sal_free(l130);return l135;}*l37 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,*l37,l28);l135 = l153(l1,*l37,l35);if(SOC_FAILURE(l135)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l37));l189(l1,&
lpm_entry,l30->key_data,*l37,l30->pivot_pyld);(void)
_soc_th_alpm_rollback_bkt_move(l1,l30->key_data,l17,l202,l30->pivot_pyld,&
l203,l39,l44);sal_free(l130);return l135;}l146 = VRF_PIVOT_TRIE_IPV6_128(l1,
l22);l135 = trie_insert(l146,(l30->pivot_pyld)->key,NULL,(l30->pivot_pyld)->
len,(trie_node_t*)l30->pivot_pyld);if(SOC_FAILURE(l135)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"failed to insert into pivot trie\n")));(void)
_soc_th_alpm_rollback_bkt_move(l1,l30->key_data,l17,l202,l30->pivot_pyld,&
l203,l39,l44);sal_free(l130);return l135;}tcam_index = SOC_ALPM_128_ADDR_LWR(
*l37)<<1;ALPM_TCAM_PIVOT(l1,tcam_index) = l30->pivot_pyld;PIVOT_TCAM_INDEX(
l30->pivot_pyld) = tcam_index;VRF_PIVOT_REF_INC(l1,l22,l28);l135 = 
_soc_th_alpm_128_move_inval(l1,l17,&l203,l39);if(SOC_FAILURE(l135)){l189(l1,&
lpm_entry,l30->key_data,*l37,l30->pivot_pyld);(void)
_soc_th_alpm_rollback_bkt_move(l1,l30->key_data,l17,l202,l30->pivot_pyld,&
l203,l39,l44);sal_free(l39);l39 = NULL;sal_free(l130);return l135;}l135 = 
soc_th_alpm_update_hit_bits(l1,l203.count,l130,l39);if(SOC_FAILURE(l135)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l203.count));l135 = 
SOC_E_NONE;}sal_free(l39);l39 = NULL;sal_free(l130);if(l44 == -1){
soc_mem_field32_set(l1,l17,l30->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(
PIVOT_BUCKET_INDEX(l202)));l135 = _soc_th_alpm_insert_in_bkt(l1,l17,
PIVOT_BUCKET_INDEX(l202),l150,l30->alpm_data,l10,&l128,l28);if(SOC_FAILURE(
l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n")));
_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->new_pfx_pyld,l39,l30->bktid,l22,
l28);return l135;}l206--;if(SOC_URPF_STATUS_GET(l1)){l135 = soc_mem_write(l1,
l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l128),l30->alpm_sip_data);}*l11 = 
l128;l30->new_pfx_pyld->index = l128;}soc_th_alpm_bu_upd(l1,l205,l204,FALSE,
l28,-l206);soc_th_alpm_bu_upd(l1,l30->bktid,tcam_index,l22,l28,l207);
PIVOT_BUCKET_ENT_CNT_UPDATE(l30->pivot_pyld);VRF_BUCKET_SPLIT_INC(l1,l22,l28)
;return l135;}static int l208(int l1,void*key_data,soc_mem_t l17,void*
alpm_data,void*alpm_sip_data,int*l11,int bktid,int tcam_index){alpm_pivot_t*
pivot_pyld,*l202;defip_aux_scratch_entry_t l8;uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l33;int l28,l22,l149;int l128;int
l135 = SOC_E_NONE,l177;uint32 l6,l150;int l145 = 0;int l37;int l209 = 0;
trie_t*trie,*l31;trie_node_t*l147 = NULL;payload_t*l210,*l211,*l198;int l19 = 
0;int*l39 = NULL;alpm_pfx_info_t l30;l28 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l149,&l22));
soc_th_alpm_bank_db_type_get(l1,l22,&l150,&l6);l17 = L3_DEFIP_ALPM_IPV6_128m;
l135 = _alpm_128_prefix_create(l1,key_data,prefix,&l33,&l19);if(SOC_FAILURE(
l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: prefix create failed\n")));return l135;}sal_memset(&l8
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l28,
l6,0,&l8));if(bktid == 0){l135 = l144(l1,prefix,l33,l28,l22,&l145,&tcam_index
,&bktid);SOC_IF_ERROR_RETURN(l135);soc_mem_field32_set(l1,l17,alpm_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));}l135 = _soc_th_alpm_insert_in_bkt(l1,l17,
bktid,l150,alpm_data,l10,&l128,l28);if(l135 == SOC_E_NONE){*l11 = l128;if(
SOC_URPF_STATUS_GET(l1)){soc_mem_field32_set(l1,l17,alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));l177 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l128),alpm_sip_data);if(SOC_FAILURE(l177)){return
l177;}}}if(l135 == SOC_E_FULL){l145 = _soc_th_alpm_bkt_entry_cnt(l1,
L3_DEFIP_MODE_128);if(l145>4){l145 = 4;}if(PIVOT_BUCKET_COUNT(ALPM_TCAM_PIVOT
(l1,tcam_index))<l145){int l212;l135 = soc_th_alpm_bs_alloc(l1,&l212,l22,l28)
;SOC_IF_ERROR_RETURN(l135);l135 = _soc_th_alpm_128_move_trie(l1,l28,bktid,
l212);SOC_IF_ERROR_RETURN(l135);bktid = l212;soc_mem_field32_set(l1,l17,
alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));l135 = 
_soc_th_alpm_insert_in_bkt(l1,l17,bktid,l150,alpm_data,l10,&l128,l28);if(
SOC_SUCCESS(l135)){*l11 = l128;if(SOC_URPF_STATUS_GET(l1)){
soc_mem_field32_set(l1,l17,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));
l177 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l128),
alpm_sip_data);if(SOC_FAILURE(l177)){return l177;}}}else{return l135;}}else{
l209 = 1;}}pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l202 = pivot_pyld;l210 = sal_alloc(sizeof(
payload_t),"Payload for 128b Key");if(l210 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"trie node \n")));return SOC_E_MEMORY;}l211 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l211 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l210);return SOC_E_MEMORY;}sal_memset(l210,0,
sizeof(*l210));sal_memset(l211,0,sizeof(*l211));l210->key[0] = prefix[0];l210
->key[1] = prefix[1];l210->key[2] = prefix[2];l210->key[3] = prefix[3];l210->
key[4] = prefix[4];l210->len = l33;l210->index = l128;sal_memcpy(l211,l210,
sizeof(*l210));l211->bkt_ptr = l210;l135 = trie_insert(trie,prefix,NULL,l33,(
trie_node_t*)l210);if(SOC_FAILURE(l135)){if(l210!= NULL){sal_free(l210);}if(
l211!= NULL){sal_free(l211);}return l135;}l31 = VRF_PREFIX_TRIE_IPV6_128(l1,
l22);if(!l19){l135 = trie_insert(l31,prefix,NULL,l33,(trie_node_t*)l211);}
else{l147 = NULL;l135 = trie_find_lpm(l31,0,0,&l147);l198 = (payload_t*)l147;
if(SOC_SUCCESS(l135)){l198->bkt_ptr = l210;}}if(SOC_FAILURE(l135)){
_soc_th_alpm_free_pfx_trie(l1,l31,trie,l211,l39,bktid,l22,l28);return l135;}
if(l209){l30.key_data = key_data;l30.new_pfx_pyld = l210;l30.pivot_pyld = 
pivot_pyld;l30.alpm_data = alpm_data;l30.alpm_sip_data = alpm_sip_data;l30.
bktid = bktid;l135 = l36(l1,&l30,&l37,l11);if(l135!= SOC_E_NONE){return l135;
}bktid = l30.bktid;tcam_index = PIVOT_TCAM_INDEX(l30.pivot_pyld);
alpm_128_split_count++;}else{soc_th_alpm_bu_upd(l1,bktid,tcam_index,l22,l28,1
);}VRF_TRIE_ROUTES_INC(l1,l22,l28);if(l19){sal_free(l211);}
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l8,TRUE,&l145,&
tcam_index,&bktid));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
INSERT_PROPAGATE,&l8,FALSE,&l145,&tcam_index,&bktid));if(SOC_URPF_STATUS_GET(
l1)){uint32 l213[4] = {0,0,0,0};l33 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf);l33+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf,l33);if(l19){soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,0);}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1
,DELETE_PROPAGATE,&l8,TRUE,&l145,&tcam_index,&bktid));SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l8,FALSE,&l145,&tcam_index,&bktid));
if(!l19){soc_mem_field_set(l1,L3_DEFIP_AUX_SCRATCHm,(uint32*)&l8,IP_ADDRf,(
uint32*)l213);soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,IP_LENGTHf,0);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,REPLACE_LENf,0);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,1);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l8,TRUE,&l145,&tcam_index,&bktid));}
}PIVOT_BUCKET_ENT_CNT_UPDATE(l202);return l135;}static int l24(int unit,
uint32*key,int len,int l22,int l5,defip_pair_128_entry_t*lpm_entry,int l25,
int l26){uint32 l141;if(l26){sal_memset(lpm_entry,0,sizeof(
defip_pair_128_entry_t));}soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_0_LWRf,l22&SOC_VRF_MAX(unit));soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,VRF_ID_1_LWRf,l22&SOC_VRF_MAX(unit));
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,VRF_ID_0_UPRf,l22&
SOC_VRF_MAX(unit));soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
VRF_ID_1_UPRf,l22&SOC_VRF_MAX(unit));if(l22 == (SOC_VRF_MAX(unit)+1)){l141 = 
0;}else{l141 = SOC_VRF_MAX(unit);}soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,VRF_ID_MASK0_LWRf,l141);soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,VRF_ID_MASK1_LWRf,l141);soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,VRF_ID_MASK0_UPRf,l141);soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,VRF_ID_MASK1_UPRf,l141);soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,IP_ADDR0_LWRf,key[0]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,IP_ADDR1_LWRf,key[1]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,IP_ADDR0_UPRf,key[2]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,IP_ADDR1_UPRf,key[3]);soc_L3_DEFIP_PAIR_128m_field32_set(unit,
lpm_entry,MODE0_LWRf,3);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,
MODE1_LWRf,3);soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,MODE0_UPRf,3)
;soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,MODE1_UPRf,3);l142(unit,(
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
return(SOC_E_NONE);}static int l214(int l1,void*key_data,int bktid,int
tcam_index,int l128){alpm_pivot_t*pivot_pyld;defip_alpm_ipv6_128_entry_t l181
,l215,l182;defip_aux_scratch_entry_t l8;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];
soc_mem_t l17;void*l179,*l216,*l180 = NULL;int l149;int l135 = SOC_E_NONE,
l177 = SOC_E_NONE;uint32 l217[5],prefix[5];int l28,l22;uint32 l33;int l218;
uint32 l6,l150;int l145,l19 = 0;trie_t*trie,*l31;uint32 l219;
defip_pair_128_entry_t lpm_entry,*l220;payload_t*l210 = NULL,*l221 = NULL,*
l198 = NULL;trie_node_t*l191 = NULL,*l147 = NULL;trie_t*l146 = NULL;l28 = 
L3_DEFIP_MODE_128;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data
,&l149,&l22));if(!(((l149 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1)
== SOC_ALPM_MODE_TCAM_ALPM)&&(l149 == SOC_L3_VRF_GLOBAL))))){
soc_th_alpm_bank_db_type_get(l1,l22,&l150,&l6);l135 = _alpm_128_prefix_create
(l1,key_data,prefix,&l33,&l19);if(SOC_FAILURE(l135)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_128_delete: prefix create failed\n"
)));return l135;}if(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_COMBINED){if(
l149!= SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1,l22,l28)>1){if(l19){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"VRF %d: Cannot v6-128 delete "
"default route if other routes are present ""in this mode"),l22));return
SOC_E_PARAM;}}}l6 = 2;}l17 = L3_DEFIP_ALPM_IPV6_128m;l179 = ((uint32*)&(l181)
);SOC_ALPM_LPM_LOCK(l1);if(bktid == 0){l135 = _soc_th_alpm_128_find(l1,
key_data,l17,l179,&tcam_index,&bktid,&l128,TRUE);}else{l135 = l14(l1,key_data
,l179,0,l17,0,0,bktid);}sal_memcpy(&l215,l179,sizeof(l215));l216 = &l215;if(
SOC_FAILURE(l135)){SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_delete: Unable to find ""prefix for delete\n")))
;return l135;}l218 = bktid;pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l135 = trie_delete(trie,prefix,l33,&l191);l210 = 
(payload_t*)l191;if(l135!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l135;}l31 = VRF_PREFIX_TRIE_IPV6_128(l1,l22);
l146 = VRF_PIVOT_TRIE_IPV6_128(l1,l22);if(!l19){l135 = trie_delete(l31,prefix
,l33,&l191);l221 = (payload_t*)l191;if(SOC_FAILURE(l135)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l222;}l147 = NULL;l135 = trie_find_lpm(l31,prefix,l33,&l147)
;l198 = (payload_t*)l147;if(SOC_SUCCESS(l135)){payload_t*l223 = (payload_t*)(
l198->bkt_ptr);if(l223!= NULL){l219 = l223->len;}else{l219 = 0;}}else{
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l224;}sal_memcpy(l217,prefix,sizeof(
prefix));l138((l217),(l33),(l28));l135 = l24(l1,l217,l219,l22,l28,&lpm_entry,
0,1);l177 = _soc_th_alpm_128_find(l1,&lpm_entry,l17,l179,&tcam_index,&bktid,&
l128,TRUE);(void)l21(l1,l179,l17,l28,l149,bktid,0,&lpm_entry);(void)l24(l1,
l217,l33,l22,l28,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(
l135)){l180 = ((uint32*)&(l182));l177 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,ALPM_ENT_INDEX(l128)),l180);}}if((l219 == 0)&&
SOC_FAILURE(l177)){l220 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22);sal_memcpy(
&lpm_entry,l220,sizeof(lpm_entry));l135 = l24(l1,prefix,l219,l22,l28,&
lpm_entry,0,1);}if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_th_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l224;}l220 = &lpm_entry;}else{l147 = 
NULL;l135 = trie_find_lpm(l31,prefix,l33,&l147);l198 = (payload_t*)l147;if(
SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),l22));goto l222;}l198->bkt_ptr = 0;
l219 = 0;l220 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22);}l135 = l4(l1,l220,
l28,l6,l219,&l8);if(SOC_FAILURE(l135)){goto l224;}l135 = _soc_th_alpm_aux_op(
l1,DELETE_PROPAGATE,&l8,TRUE,&l145,&tcam_index,&bktid);if(SOC_FAILURE(l135)){
goto l224;}if(SOC_URPF_STATUS_GET(l1)){uint32 l141;if(l180!= NULL){l141 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf);l141++;
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf,l141);l141 = 
soc_mem_field32_get(l1,l17,l180,SRC_DISCARDf);soc_mem_field32_set(l1,l17,&l8,
SRC_DISCARDf,l141);l141 = soc_mem_field32_get(l1,l17,l180,DEFAULTROUTEf);
soc_mem_field32_set(l1,l17,&l8,DEFAULTROUTEf,l141);l135 = _soc_th_alpm_aux_op
(l1,DELETE_PROPAGATE,&l8,TRUE,&l145,&tcam_index,&bktid);}if(SOC_FAILURE(l135)
){goto l224;}}sal_free(l210);if(!l19){sal_free(l221);}
PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);if((pivot_pyld->len!= 0)&&(trie->trie
== NULL)){uint32 l225[5];sal_memcpy(l225,pivot_pyld->key,sizeof(l225));l138((
l225),(pivot_pyld->len),(l28));l24(l1,l225,pivot_pyld->len,l22,l28,&lpm_entry
,0,1);l135 = soc_th_alpm_128_lpm_delete(l1,&lpm_entry);if(SOC_FAILURE(l135)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),pivot_pyld->key[1],pivot_pyld
->key[2],pivot_pyld->key[3],pivot_pyld->key[4]));}}l135 = 
_soc_th_alpm_delete_in_bkt(l1,l17,l218,l150,l216,l10,&l128,l28);if(
SOC_FAILURE(l135)){SOC_ALPM_LPM_UNLOCK(l1);return l135;}if(
SOC_URPF_STATUS_GET(l1)){l135 = soc_mem_alpm_delete(l1,l17,
SOC_TH_ALPM_RPF_BKT_IDX(l1,ALPM_BKT_IDX(l218)),MEM_BLOCK_ALL,l150,l216,l10,&
l145);if(SOC_FAILURE(l135)){SOC_ALPM_LPM_UNLOCK(l1);return l135;}}
soc_th_alpm_bu_upd(l1,PIVOT_BUCKET_INDEX(pivot_pyld),PIVOT_TCAM_INDEX(
pivot_pyld),((pivot_pyld->len!= 0)&&(trie->trie == NULL))?TRUE:FALSE,l28,-1);
if((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l135 = soc_th_alpm_bs_free(l1
,PIVOT_BUCKET_INDEX(pivot_pyld),l22,l28);if(SOC_FAILURE(l135)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to release"
"empty bucket: %d\n"),PIVOT_BUCKET_INDEX(pivot_pyld)));}l135 = trie_delete(
l146,pivot_pyld->key,pivot_pyld->len,&l191);if(SOC_FAILURE(l135)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"could not delete pivot from pivot trie\n")));
}trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(
pivot_pyld));sal_free(pivot_pyld);_soc_tomahawk_alpm_bkt_view_set(l1,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,ALPM_BKT_IDX(l218)),INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l1,l28)){int l226 = ALPM_BKT_IDX(l218)+1;
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,l226),
INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,l22,l28);if(VRF_TRIE_ROUTES_CNT(l1,l22,
l28) == 0){l135 = l27(l1,l22,l28);}SOC_ALPM_LPM_UNLOCK(l1);return l135;l224:
l177 = trie_insert(l31,prefix,NULL,l33,(trie_node_t*)l221);if(SOC_FAILURE(
l177)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l222:l177 = trie_insert(trie,prefix,NULL,l33,(
trie_node_t*)l210);if(SOC_FAILURE(l177)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}SOC_ALPM_LPM_UNLOCK(l1);return l135;}int
soc_th_alpm_128_init(int l1){int l135 = SOC_E_NONE;l135 = 
soc_th_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l135);return l135;}int
soc_th_alpm_128_state_clear(int l1){int l134,l135;for(l134 = 0;l134<= 
SOC_VRF_MAX(l1)+1;l134++){l135 = trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,
l134),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l135)){
trie_destroy(VRF_PREFIX_TRIE_IPV6_128(l1,l134));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n"),l1,l134));return
l135;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l134)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l134));}}return SOC_E_NONE;}int
soc_th_alpm_128_deinit(int l1){soc_th_alpm_128_lpm_deinit(l1);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_state_clear(l1));return SOC_E_NONE;}
static int l227(int l1,int l22,int l28){defip_pair_128_entry_t*lpm_entry,l228
;int l229 = 0;int index;int l135 = SOC_E_NONE;uint32 key[5] = {0,0,0,0,0};
uint32 l33;alpm_bucket_handle_t*l201;alpm_pivot_t*pivot_pyld;payload_t*l221;
trie_t*l230;trie_t*l231 = NULL;trie_init(_MAX_KEY_LEN_144_,&
VRF_PIVOT_TRIE_IPV6_128(l1,l22));l231 = VRF_PIVOT_TRIE_IPV6_128(l1,l22);
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(l1,l22));l230 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l22);lpm_entry = sal_alloc(sizeof(*lpm_entry),
"Default 128 LPM entry");if(lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_th_alpm_128_vrf_add: unable to allocate memory "
"for IPv6-128 LPM entry\n")));return SOC_E_MEMORY;}l24(l1,key,0,l22,l28,
lpm_entry,0,1);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22) = lpm_entry;if(l22 == 
SOC_VRF_MAX(l1)+1){soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,
GLOBAL_ROUTEf,1);}else{soc_L3_DEFIP_PAIR_128m_field32_set(l1,lpm_entry,
DEFAULT_MISSf,1);}l135 = soc_th_alpm_bs_alloc(l1,&l229,l22,l28);if(
SOC_FAILURE(l135)){return l135;}soc_L3_DEFIP_PAIR_128m_field32_set(l1,
lpm_entry,ALG_BKT_PTRf,ALPM_BKT_IDX(l229));soc_L3_DEFIP_PAIR_128m_field32_set
(l1,lpm_entry,ALG_SUB_BKT_PTRf,ALPM_BKT_SIDX(l229));sal_memcpy(&l228,
lpm_entry,sizeof(l228));l135 = l2(l1,&l228,&index);if(SOC_FAILURE(l135)){
return l135;}l201 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l201 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_th_alpm_128_vrf_add: Unable to allocate memory "
"for bucket handle \n")));return SOC_E_NONE;}sal_memset(l201,0,sizeof(*l201))
;pivot_pyld = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(
pivot_pyld == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: Unable to allocate memory ""for PIVOT trie node \n"
)));sal_free(l201);return SOC_E_MEMORY;}l221 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l221 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_th_alpm_128_vrf_add: Unable to allocate memory "
"for pfx trie node \n")));sal_free(l201);sal_free(pivot_pyld);return
SOC_E_MEMORY;}sal_memset(pivot_pyld,0,sizeof(*pivot_pyld));sal_memset(l221,0,
sizeof(*l221));l33 = 0;PIVOT_BUCKET_HANDLE(pivot_pyld) = l201;trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(pivot_pyld));PIVOT_BUCKET_INDEX(
pivot_pyld) = l229;PIVOT_BUCKET_VRF(pivot_pyld) = l22;PIVOT_BUCKET_IPV6(
pivot_pyld) = l28;PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;pivot_pyld->key[0] = 
l221->key[0] = key[0];pivot_pyld->key[1] = l221->key[1] = key[1];pivot_pyld->
key[2] = l221->key[2] = key[2];pivot_pyld->key[3] = l221->key[3] = key[3];
pivot_pyld->key[4] = l221->key[4] = key[4];pivot_pyld->len = l221->len = l33;
l135 = trie_insert(l230,key,NULL,l33,&(l221->node));if(SOC_FAILURE(l135)){
sal_free(l221);sal_free(pivot_pyld);sal_free(l201);return l135;}l135 = 
trie_insert(l231,key,NULL,l33,(trie_node_t*)pivot_pyld);if(SOC_FAILURE(l135))
{trie_node_t*l191 = NULL;(void)trie_delete(l230,key,l33,&l191);sal_free(l221)
;sal_free(pivot_pyld);sal_free(l201);return l135;}index = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,l28);ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(index)<<1) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = 
SOC_ALPM_128_ADDR_LWR(index)<<1;VRF_PIVOT_REF_INC(l1,l22,l28);
VRF_TRIE_INIT_DONE(l1,l22,l28,1);return l135;}static int l27(int l1,int l22,
int l28){defip_pair_128_entry_t*lpm_entry;int l232;int l135 = SOC_E_NONE;
uint32 key[2] = {0,0},l139[SOC_MAX_MEM_FIELD_WORDS];payload_t*l210;
alpm_pivot_t*l233;trie_node_t*l191;trie_t*l230;trie_t*l231 = NULL;soc_mem_t
l17;int tcam_index,bktid,index;uint32 l152[SOC_MAX_MEM_FIELD_WORDS];lpm_entry
= VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22);l17 = L3_DEFIP_ALPM_IPV6_128m;l135 = 
_soc_th_alpm_128_find(l1,lpm_entry,l17,l152,&tcam_index,&bktid,&index,TRUE);
l135 = soc_th_alpm_bs_free(l1,bktid,l22,l28);_soc_tomahawk_alpm_bkt_view_set(
l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,ALPM_BKT_IDX(bktid)),INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l1,l28)){_soc_tomahawk_alpm_bkt_view_set(l1,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,ALPM_BKT_IDX(bktid)+1),INVALIDm);}l135 = l13(
l1,lpm_entry,(void*)l139,&l232);if(SOC_FAILURE(l135)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n")
,l22,l28));l232 = -1;}l232 = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,
l232,l28);l233 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l232)<<1);l135 = 
soc_th_alpm_128_lpm_delete(l1,lpm_entry);if(SOC_FAILURE(l135)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),l22,l28));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l22) = NULL;l230 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l22);VRF_PREFIX_TRIE_IPV6_128(l1,l22) = NULL;
VRF_TRIE_INIT_DONE(l1,l22,l28,0);l135 = trie_delete(l230,key,0,&l191);l210 = 
(payload_t*)l191;if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),l22,l28));}
sal_free(l210);(void)trie_destroy(l230);l231 = VRF_PIVOT_TRIE_IPV6_128(l1,l22
);VRF_PIVOT_TRIE_IPV6_128(l1,l22) = NULL;l191 = NULL;l135 = trie_delete(l231,
key,0,&l191);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),l22,l28));}(void)
trie_destroy(l231);(void)trie_destroy(PIVOT_BUCKET_TRIE(l233));sal_free(
PIVOT_BUCKET_HANDLE(l233));sal_free(l233);return l135;}int
soc_th_alpm_128_insert(int l1,void*l3,uint32 l18,int l234,int l235){
defip_alpm_ipv6_128_entry_t l181,l182;soc_mem_t l17;void*l179,*l216;int l149,
l22;int index;int l5;int l135 = SOC_E_NONE;uint32 l19;int l20 = 0;l5 = 
L3_DEFIP_MODE_128;l17 = L3_DEFIP_ALPM_IPV6_128m;l179 = ((uint32*)&(l181));
l216 = ((uint32*)&(l182));if(l234!= -1){l20 = ALPM_BKTID(ALPM_BKT_IDX(
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l234)),(l234>>ALPM_ENT_INDEX_BITS));}
SOC_IF_ERROR_RETURN(l14(l1,l3,l179,l216,l17,l18,&l19,l20));
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l3,&l149,&l22));if(((l149
== SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l149 == SOC_L3_VRF_GLOBAL)))){l135 = l2(l1,l3,&
index);if(SOC_SUCCESS(l135)){if(l149 == SOC_L3_VRF_OVERRIDE){
VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l5);VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l5);}
else{VRF_TRIE_ROUTES_INC(l1,l22,l5);VRF_PIVOT_REF_INC(l1,l22,l5);}}else if(
l135 == SOC_E_FULL){if(l149 == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_FULL_INC(l1,
MAX_VRF_ID,l5);}else{VRF_PIVOT_FULL_INC(l1,l22,l5);}}return(l135);}else if(
l22 == 0){if(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL||
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"VRF=0 cannot be added in Parallel mode\n")));
return SOC_E_PARAM;}}if(l149!= SOC_L3_VRF_GLOBAL){if(soc_th_alpm_mode_get(l1)
== SOC_ALPM_MODE_COMBINED){if(VRF_TRIE_ROUTES_CNT(l1,l22,l5) == 0){if(!l19){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
l149));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,l22,l5)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d is not ""initialized\n"),l22));l135 = l227(l1,
l22,l5);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),l22,l5));return
l135;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init ""completed\n"),l22,l5));}if(l235
&SOC_ALPM_LOOKUP_HIT){l135 = l151(l1,l3,l179,l216,l17,l234);}else{if(l234 == 
-1){l234 = 0;}l234 = ALPM_BKTID(ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,
l234)),l234>>ALPM_ENT_INDEX_BITS);l135 = l208(l1,l3,l17,l179,l216,&index,l234
,l235);}if(l135!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_th_alpm_128_insert: ""Route Insertion Failed :%s\n"),l1,
soc_errmsg(l135)));}return(l135);}int soc_th_alpm_128_lookup(int l1,void*
key_data,void*l10,int*l11,int*l236){defip_alpm_ipv6_128_entry_t l181;
soc_mem_t l17;int bktid = 0;int tcam_index;void*l179;int l149,l22;int l5 = 2,
l127;int l135 = SOC_E_NONE;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1
,key_data,&l149,&l22));l135 = l9(l1,key_data,l10,l11,&l127,&l5);if(
SOC_SUCCESS(l135)){SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l10,&
l149,&l22));if(((l149 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l149 == SOC_L3_VRF_GLOBAL)))){return SOC_E_NONE;}}
if(!VRF_TRIE_INIT_COMPLETED(l1,l22,l5)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_th_alpm_128_lookup:VRF %d is not ""initialized\n"),l22));*
l11 = 0;*l236 = 0;return SOC_E_NOT_FOUND;}l17 = L3_DEFIP_ALPM_IPV6_128m;l179 = 
((uint32*)&(l181));SOC_ALPM_LPM_LOCK(l1);l135 = _soc_th_alpm_128_find(l1,
key_data,l17,l179,&tcam_index,&bktid,l11,TRUE);SOC_ALPM_LPM_UNLOCK(l1);if(
SOC_FAILURE(l135)){*l236 = tcam_index;*l11 = (ALPM_BKT_SIDX(bktid)<<
ALPM_ENT_INDEX_BITS)|SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,bktid);return l135;}l135
= l21(l1,l179,l17,l5,l149,bktid,*l11,l10);*l236 = SOC_ALPM_LOOKUP_HIT|
tcam_index;*l11 = (ALPM_BKT_SIDX(bktid)<<ALPM_ENT_INDEX_BITS)|*l11;return(
l135);}int l237(int l1,void*key_data,void*l10,int l22,int*tcam_index,int*
bucket_index,int*l128,int l238){int l135 = SOC_E_NONE;int l134,l239,l28,l145 = 
0;uint32 l6,l150;defip_aux_scratch_entry_t l8;int l240,l241;int index;
soc_mem_t l17,l171;int l242,l243,l244;soc_field_t l245[4] = {IP_ADDR0_LWRf,
IP_ADDR1_LWRf,IP_ADDR0_UPRf,IP_ADDR1_UPRf};uint32 l246[
SOC_MAX_MEM_FIELD_WORDS] = {0};int l247 = -1;int l248 = 0;l28 = 
L3_DEFIP_MODE_128;l171 = L3_DEFIP_PAIR_128m;l240 = soc_mem_field32_get(l1,
l171,key_data,GLOBAL_ROUTEf);l241 = soc_mem_field32_get(l1,l171,key_data,
VRF_ID_0_LWRf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d\n"),l22 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l28,l240,l241));if(l22 == SOC_L3_VRF_GLOBAL){l6 = l238?1:0;
soc_mem_field32_set(l1,l171,key_data,GLOBAL_ROUTEf,1);soc_mem_field32_set(l1,
l171,key_data,VRF_ID_0_LWRf,0);}else{l6 = l238?3:2;}
soc_th_alpm_bank_db_type_get(l1,l22 == SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l1)+1):
l22,&l150,NULL);sal_memset(&l8,0,sizeof(defip_aux_scratch_entry_t));
SOC_IF_ERROR_RETURN(l4(l1,key_data,l28,l6,0,&l8));if(l22 == SOC_L3_VRF_GLOBAL
){soc_mem_field32_set(l1,l171,key_data,GLOBAL_ROUTEf,l240);
soc_mem_field32_set(l1,l171,key_data,VRF_ID_0_LWRf,l241);}SOC_IF_ERROR_RETURN
(_soc_th_alpm_aux_op(l1,PREFIX_LOOKUP,&l8,TRUE,&l145,tcam_index,bucket_index)
);if(l145 == 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Could not find bucket\n")));return SOC_E_NOT_FOUND;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit in memory %s, index %d, "
"bucket [%d,%d]\n"),SOC_MEM_NAME(l1,l171),soc_th_alpm_logical_idx(l1,l171,
SOC_ALPM_128_DEFIP_TO_PAIR((*tcam_index)>>1),1),ALPM_BKT_IDX(*bucket_index),
ALPM_BKT_SIDX(*bucket_index)));l17 = L3_DEFIP_ALPM_IPV6_128m;l135 = l133(l1,
key_data,&l243);if(SOC_FAILURE(l135)){return l135;}l244 = 
SOC_TH_ALPM_SCALE_CHECK(l1,l28)?16:8;LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Start searching mem %s bucket [%d,%d](count %d) ""for Length %d\n"),
SOC_MEM_NAME(l1,l17),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index)
,l244,l243));for(l134 = 0;l134<l244;l134++){uint32 l179[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l249[4] = {0};uint32 l250[4] = {0};
uint32 l251[4] = {0};int l252;l135 = _soc_th_alpm_mem_index(l1,l17,
ALPM_BKT_IDX(*bucket_index),l134,l150,&index);if(l135 == SOC_E_FULL){continue
;}SOC_IF_ERROR_RETURN(soc_mem_read(l1,l17,MEM_BLOCK_ANY,index,(void*)&l179));
l252 = soc_mem_field32_get(l1,l17,&l179,VALIDf);l242 = soc_mem_field32_get(l1
,l17,&l179,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket [%d,%d] index %6d: valid %d, length %d\n"),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),index,l252,l242));if(!l252||(l242>l243)){
continue;}SHR_BITSET_RANGE(l249,128-l242,l242);(void)soc_mem_field_get(l1,l17
,(uint32*)&l179,KEYf,(uint32*)l250);l251[3] = soc_mem_field32_get(l1,l171,
key_data,l245[3]);l251[2] = soc_mem_field32_get(l1,l171,key_data,l245[2]);
l251[1] = soc_mem_field32_get(l1,l171,key_data,l245[1]);l251[0] = 
soc_mem_field32_get(l1,l171,key_data,l245[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tmask %08x %08x %08x %08x \n""\t key %08x %08x %08x %08x \n"
"\thost %08x %08x %08x %08x \n"),l249[3],l249[2],l249[1],l249[0],l250[3],l250
[2],l250[1],l250[0],l251[3],l251[2],l251[1],l251[0]));for(l239 = 3;l239>= 0;
l239--){if((l251[l239]&l249[l239])!= (l250[l239]&l249[l239])){break;}}if(l239
>= 0){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket [%d,%d], ""index %d\n"),SOC_MEM_NAME(l1,l17),
ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index),index));if(l247 == -
1||l247<l242){l247 = l242;l248 = index;sal_memcpy(l246,l179,sizeof(l179));}}
if(l247!= -1){l135 = l21(l1,&l246,l17,l28,l22,*bucket_index,l248,l10);if(
SOC_SUCCESS(l135)){*l128 = l248;if(bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l1)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit mem %s bucket [%d,%d], index %d\n"),SOC_MEM_NAME(l1,l17),ALPM_BKT_IDX(*
bucket_index),ALPM_BKT_SIDX(*bucket_index),l248));}}return l135;}*l128 = 
soc_th_alpm_logical_idx(l1,l171,SOC_ALPM_128_DEFIP_TO_PAIR((*tcam_index)>>1),
1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l171,MEM_BLOCK_ANY,*l128,(void*)l10));
return SOC_E_NONE;}int soc_th_alpm_128_find_best_match(int l1,void*key_data,
void*l10,int*l11,int l238){int l135 = SOC_E_NONE;int l134,l239;int l253,l254;
defip_pair_128_entry_t l255;uint32 l256,l250,l251;int l242,l243;int l257,l258
;int l149,l22 = 0;int tcam_index,bucket_index;soc_mem_t l171;soc_field_t l259
[4] = {IP_ADDR_MASK1_UPRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_LWRf};soc_field_t l260[4] = {IP_ADDR1_UPRf,IP_ADDR0_UPRf,
IP_ADDR1_LWRf,IP_ADDR0_LWRf};l171 = L3_DEFIP_PAIR_128m;if(!
SOC_URPF_STATUS_GET(l1)&&l238){return SOC_E_PARAM;}l253 = soc_mem_index_min(
l1,l171);l254 = soc_mem_index_count(l1,l171);if(SOC_URPF_STATUS_GET(l1)){l254
>>= 1;}if(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL||
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){l254>>= 1;l253+= l254;}
if(l238){l253+= soc_mem_index_count(l1,l171)/2;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Start LPM searchng from %d, count %d\n"),l253,l254));for(l134 = 
l253;l134<l253+l254;l134++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l171,
MEM_BLOCK_ANY,l134,(void*)&l255));if(!soc_mem_field32_get(l1,l171,&l255,
VALID0_LWRf)){continue;}l257 = soc_mem_field32_get(l1,l171,&l255,GLOBAL_HIGHf
);l258 = soc_mem_field32_get(l1,l171,&l255,GLOBAL_ROUTEf);if(!(
soc_th_alpm_mode_get(l1)!= SOC_ALPM_MODE_TCAM_ALPM&&l257&&l258)&&!(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM&&l258)){continue;}l135 = 
l133(l1,key_data,&l243);l135 = l133(l1,&l255,&l242);if(SOC_FAILURE(l135)||(
l242>l243)){continue;}for(l239 = 0;l239<4;l239++){l256 = soc_mem_field32_get(
l1,l171,&l255,l259[l239]);l250 = soc_mem_field32_get(l1,l171,&l255,l260[l239]
);l251 = soc_mem_field32_get(l1,l171,key_data,l260[l239]);if((l251&l256)!= (
l250&l256)){break;}}if(l239<4){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Hit Global High route in index = %d\n"),l239));sal_memcpy(l10,
&l255,sizeof(l255));*l11 = l134;return SOC_E_NONE;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for Global Low and VRF routes\n")));SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l149,&l22));l135 = l237(l1,key_data,
l10,l22,&tcam_index,&bucket_index,l11,l238);if(l135 == SOC_E_NOT_FOUND){l22 = 
SOC_L3_VRF_GLOBAL;LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Not found in VRF region, try Global ""region\n")));l135 = l237(l1,key_data,
l10,l22,&tcam_index,&bucket_index,l11,l238);}if(SOC_SUCCESS(l135)){tcam_index
= soc_th_alpm_logical_idx(l1,l171,SOC_ALPM_128_DEFIP_TO_PAIR(tcam_index>>1),1
);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in %s region in TCAM index %d, ""bucket [%d,%d]\n"),l22 == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",tcam_index,ALPM_BKT_IDX(bucket_index),
ALPM_BKT_SIDX(bucket_index)));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Search miss for given address\n")));}return(l135);}int
soc_th_alpm_128_delete(int l1,void*key_data,int l234,int l235){int l149,l22;
int l5;int l135 = SOC_E_NONE;int l261;l5 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&l149,&l22));if((
(l149 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l149 == SOC_L3_VRF_GLOBAL)))){l135 = 
soc_th_alpm_128_lpm_delete(l1,key_data);if(SOC_SUCCESS(l135)){if(l149 == 
SOC_L3_VRF_OVERRIDE){VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l5);VRF_TRIE_ROUTES_DEC(
l1,MAX_VRF_ID,l5);}else{VRF_PIVOT_REF_DEC(l1,l22,l5);VRF_TRIE_ROUTES_DEC(l1,
l22,l5);}}return(l135);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,l22,l5)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_delete:VRF %d/%d is not ""initialized\n"),l22,l5));return
SOC_E_NONE;}if(l234 == -1){l234 = 0;}l261 = l234;l234 = ALPM_BKTID(
ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l261)),l261>>ALPM_ENT_INDEX_BITS
);l135 = l214(l1,key_data,l234,l235&~SOC_ALPM_LOOKUP_HIT,l234);}return(l135);
}static void l115(int l1,void*l10,int index,l110 l116){l116[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l103[(l1)]->l65));l116[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l103[(l1)]->l63));l116[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l103[(l1)]->l69));l116[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l103[(l1)]->l67));l116[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l103[(l1)]->l66));l116[5] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l103[(l1)]->l64));l116[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l103[(l1)]->l70));l116[7] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l103[(l1)]->l68));if((!(
SOC_IS_HURRICANE(l1)))&&(((l103[(l1)]->l88)!= NULL))){int l262;(void)
soc_th_alpm_128_lpm_vrf_get(l1,l10,(int*)&l116[8],&l262);}else{l116[8] = 0;};
}static int l263(l110 l112,l110 l113){int l232;for(l232 = 0;l232<9;l232++){{
if((l112[l232])<(l113[l232])){return-1;}if((l112[l232])>(l113[l232])){return 1
;}};}return(0);}static void l264(int l1,void*l3,uint32 l265,uint32 l130,int
l127){l110 l266;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l103[(l1)]->l85))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l103[(l1)]->l84))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l103[(l1)]->l83))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l103[(l1)]->l82))){l266[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l103[(l1)]->l65));l266[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l103[(l1)]->l63));l266[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l103[(l1)]->l69));l266[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l103[(l1)]->l67));l266[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l103[(l1)]->l66));l266[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l103[(l1)]->l64));l266[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l103[(l1)]->l70));l266[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l103[(l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l103[(l1)]->l88)!= NULL)
)){int l262;(void)soc_th_alpm_128_lpm_vrf_get(l1,l3,(int*)&l266[8],&l262);}
else{l266[8] = 0;};l129((l114[(l1)]),l263,l266,l127,l130,l265);}}static void
l267(int l1,void*key_data,uint32 l265){l110 l266;int l127 = -1;int l135;
uint16 index;l266[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l103[(l1)]->l65));l266[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l103[(l1)]->l63));l266[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l103[(l1)]->l69));l266[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l103[(l1)]->l67));l266[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l103[(l1)]->l66));l266[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l103[(l1)]->l64));l266[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l103[(l1)]->l70));l266[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l103[(l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l103[(l1)]->l88
)!= NULL))){int l262;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l266[8],&l262);}else{l266[8] = 0;};index = l265;l135 = l131((l114[(l1)]),l263
,l266,l127,index);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"\ndel  index: H %d error %d\n"),index,l135));}}static int l268(int l1,
void*key_data,int l127,int*l128){l110 l266;int l135;uint16 index = (0xFFFF);
l266[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l103[(l1)]->l65));l266[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l103[(l1)]->l63));l266[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l103[(l1)]->l69));l266[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l103[(l1)]->l67));l266[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l103[(l1)]->l66));l266[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l103[(l1)]->l64));l266[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l103[(l1)]->l70));l266[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l103[(l1)]->l68));if((!(SOC_IS_HURRICANE(l1)))&&(((l103[(l1)]->l88
)!= NULL))){int l262;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l266[8],&l262);}else{l266[8] = 0;};l135 = l124((l114[(l1)]),l263,l266,l127,&
index);if(SOC_FAILURE(l135)){*l128 = 0xFFFFFFFF;return(l135);}*l128 = index;
return(SOC_E_NONE);}static uint16 l117(uint8*l118,int l119){return(
_shr_crc16b(0,l118,l119));}static int l120(int unit,int l105,int l106,l109**
l121){l109*l125;int index;if(l106>l105){return SOC_E_MEMORY;}l125 = sal_alloc
(sizeof(l109),"lpm_hash");if(l125 == NULL){return SOC_E_MEMORY;}sal_memset(
l125,0,sizeof(*l125));l125->unit = unit;l125->l105 = l105;l125->l106 = l106;
l125->l107 = sal_alloc(l125->l106*sizeof(*(l125->l107)),"hash_table");if(l125
->l107 == NULL){sal_free(l125);return SOC_E_MEMORY;}l125->l108 = sal_alloc(
l125->l105*sizeof(*(l125->l108)),"link_table");if(l125->l108 == NULL){
sal_free(l125->l107);sal_free(l125);return SOC_E_MEMORY;}for(index = 0;index<
l125->l106;index++){l125->l107[index] = (0xFFFF);}for(index = 0;index<l125->
l105;index++){l125->l108[index] = (0xFFFF);}*l121 = l125;return SOC_E_NONE;}
static int l122(l109*l123){if(l123!= NULL){sal_free(l123->l107);sal_free(l123
->l108);sal_free(l123);}return SOC_E_NONE;}static int l124(l109*l125,l111 l126
,l110 entry,int l127,uint16*l128){int l1 = l125->unit;uint16 l269;uint16 index
;l269 = l117((uint8*)entry,(32*9))%l125->l106;index = l125->l107[l269];;;
while(index!= (0xFFFF)){uint32 l10[SOC_MAX_MEM_FIELD_WORDS];l110 l116;int l270
;l270 = index;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,
l270,l10));l115(l1,l10,index,l116);if((*l126)(entry,l116) == 0){*l128 = index
;;return(SOC_E_NONE);}index = l125->l108[index&(0x3FFF)];;};return(
SOC_E_NOT_FOUND);}static int l129(l109*l125,l111 l126,l110 entry,int l127,
uint16 l130,uint16 l39){int l1 = l125->unit;uint16 l269;uint16 index;uint16
l271;l269 = l117((uint8*)entry,(32*9))%l125->l106;index = l125->l107[l269];;;
;l271 = (0xFFFF);if(l130!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];l110 l116;int l270;l270 = index;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l270,l10));l115(l1,l10,index,l116);
if((*l126)(entry,l116) == 0){if(l39!= index){;if(l271 == (0xFFFF)){l125->l107
[l269] = l39;l125->l108[l39&(0x3FFF)] = l125->l108[index&(0x3FFF)];l125->l108
[index&(0x3FFF)] = (0xFFFF);}else{l125->l108[l271&(0x3FFF)] = l39;l125->l108[
l39&(0x3FFF)] = l125->l108[index&(0x3FFF)];l125->l108[index&(0x3FFF)] = (
0xFFFF);}};return(SOC_E_NONE);}l271 = index;index = l125->l108[index&(0x3FFF)
];;}}l125->l108[l39&(0x3FFF)] = l125->l107[l269];l125->l107[l269] = l39;
return(SOC_E_NONE);}static int l131(l109*l125,l111 l126,l110 entry,int l127,
uint16 l132){uint16 l269;uint16 index;uint16 l271;l269 = l117((uint8*)entry,(
32*9))%l125->l106;index = l125->l107[l269];;;l271 = (0xFFFF);while(index!= (
0xFFFF)){if(l132 == index){;if(l271 == (0xFFFF)){l125->l107[l269] = l125->
l108[l132&(0x3FFF)];l125->l108[l132&(0x3FFF)] = (0xFFFF);}else{l125->l108[
l271&(0x3FFF)] = l125->l108[l132&(0x3FFF)];l125->l108[l132&(0x3FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l271 = index;index = l125->l108[index&(0x3FFF)];
;}return(SOC_E_NOT_FOUND);}static int l272(int l1,void*l10){return(SOC_E_NONE
);}void soc_th_alpm_128_lpm_state_dump(int l1){int l134;int l273;l273 = ((3*(
128+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){
return;}for(l134 = l273;l134>= 0;l134--){if((l134!= ((3*(128+2+1))-1))&&((l54
[(l1)][(l134)].l47) == -1)){continue;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"PFX = %d P = %d N = %d START = %d ""END = %d VENT = %d FENT = %d\n"),l134
,(l54[(l1)][(l134)].l49),(l54[(l1)][(l134)].next),(l54[(l1)][(l134)].l47),(
l54[(l1)][(l134)].l48),(l54[(l1)][(l134)].l50),(l54[(l1)][(l134)].l51)));}
COMPILER_REFERENCE(l272);}static int l274(int l1,int index,uint32*l10){int
l275;uint32 l276,l277,l278;uint32 l279;int l280;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l275 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}else if(SOC_IS_APOLLO(l1)){
l275 = (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l275 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}if(((l103[(l1)]->l57)!= NULL)
){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
l10),(l103[(l1)]->l57),(0));}l276 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l103[(l1)]->l69));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l103[(l1)]->l70));l277 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l103[(l1)]->l67));l278 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l103[(l1)]->l68));l280 = ((!l276)&&(!l279)&&(!l277)&&(!l278))?1:0;l276 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l103[(l1)]->l84));l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l103[(l1)]->l82));l277 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l103[(l1)]->l83));l278 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l103[(l1)]->l83));if(l276&&l279&&l277&&l278){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l103[(l1)]->l81),(l280));}return l166(l1,MEM_BLOCK_ANY,index+l275,index,
l10);}static int l281(int l1,int l282,int l283){uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l282,l10));l264(l1,l10,l283,0x4000,0);SOC_IF_ERROR_RETURN(l166(
l1,MEM_BLOCK_ANY,l283,l282,l10));SOC_IF_ERROR_RETURN(l274(l1,l283,l10));do{
int l284 = soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l282),1);int l285
= soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l283),1);int l286 = 
SOC_ALPM_128_ADDR_LWR((l285))<<1;ALPM_TCAM_PIVOT((l1),l286) = ALPM_TCAM_PIVOT
((l1),SOC_ALPM_128_ADDR_LWR((l284))<<1);if(ALPM_TCAM_PIVOT((l1),l286)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l286)) = l286;
soc_th_alpm_lpm_move_bu_upd((l1),PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l1),l286
)),l286);}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l284))<<1) = NULL;}while(
0);return(SOC_E_NONE);}static int l287(int l1,int l127,int l5){int l282;int
l283;l283 = (l54[(l1)][(l127)].l48)+1;l282 = (l54[(l1)][(l127)].l47);if(l282
!= l283){SOC_IF_ERROR_RETURN(l281(l1,l282,l283));VRF_PIVOT_SHIFT_INC(l1,
MAX_VRF_ID,l5);}(l54[(l1)][(l127)].l47)+= 1;(l54[(l1)][(l127)].l48)+= 1;
return(SOC_E_NONE);}static int l288(int l1,int l127,int l5){int l282;int l283
;l283 = (l54[(l1)][(l127)].l47)-1;if((l54[(l1)][(l127)].l50) == 0){(l54[(l1)]
[(l127)].l47) = l283;(l54[(l1)][(l127)].l48) = l283-1;return(SOC_E_NONE);}
l282 = (l54[(l1)][(l127)].l48);SOC_IF_ERROR_RETURN(l281(l1,l282,l283));
VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l5);(l54[(l1)][(l127)].l47)-= 1;(l54[(l1)][
(l127)].l48)-= 1;return(SOC_E_NONE);}static int l289(int l1,int l127,int l5,
void*l10,int*l290){int l291;int l292;int l293;int l294;if((l54[(l1)][(l127)].
l50) == 0){l294 = ((3*(128+2+1))-1);if((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM
)){if(l127<= (((3*(128+2+1))/3)-1)){l294 = (((3*(128+2+1))/3)-1);}}while((l54
[(l1)][(l294)].next)>l127){l294 = (l54[(l1)][(l294)].next);}l292 = (l54[(l1)]
[(l294)].next);if(l292!= -1){(l54[(l1)][(l292)].l49) = l127;}(l54[(l1)][(l127
)].next) = (l54[(l1)][(l294)].next);(l54[(l1)][(l127)].l49) = l294;(l54[(l1)]
[(l294)].next) = l127;(l54[(l1)][(l127)].l51) = ((l54[(l1)][(l294)].l51)+1)/2
;(l54[(l1)][(l294)].l51)-= (l54[(l1)][(l127)].l51);(l54[(l1)][(l127)].l47) = 
(l54[(l1)][(l294)].l48)+(l54[(l1)][(l294)].l51)+1;(l54[(l1)][(l127)].l48) = (
l54[(l1)][(l127)].l47)-1;(l54[(l1)][(l127)].l50) = 0;}l293 = l127;while((l54[
(l1)][(l293)].l51) == 0){l293 = (l54[(l1)][(l293)].next);if(l293 == -1){l293 = 
l127;break;}}while((l54[(l1)][(l293)].l51) == 0){l293 = (l54[(l1)][(l293)].
l49);if(l293 == -1){if((l54[(l1)][(l127)].l50) == 0){l291 = (l54[(l1)][(l127)
].l49);l292 = (l54[(l1)][(l127)].next);if(-1!= l291){(l54[(l1)][(l291)].next)
= l292;}if(-1!= l292){(l54[(l1)][(l292)].l49) = l291;}}return(SOC_E_FULL);}}
while(l293>l127){l292 = (l54[(l1)][(l293)].next);SOC_IF_ERROR_RETURN(l288(l1,
l292,l5));(l54[(l1)][(l293)].l51)-= 1;(l54[(l1)][(l292)].l51)+= 1;l293 = l292
;}while(l293<l127){SOC_IF_ERROR_RETURN(l287(l1,l293,l5));(l54[(l1)][(l293)].
l51)-= 1;l291 = (l54[(l1)][(l293)].l49);(l54[(l1)][(l291)].l51)+= 1;l293 = 
l291;}(l54[(l1)][(l127)].l50)+= 1;(l54[(l1)][(l127)].l51)-= 1;(l54[(l1)][(
l127)].l48)+= 1;*l290 = (l54[(l1)][(l127)].l48);sal_memcpy(l10,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);return(SOC_E_NONE);}static int l295(int l1,int l127,
int l5,void*l10,int l296){int l291;int l292;int l282;int l283;uint32 l297[
SOC_MAX_MEM_FIELD_WORDS];int l135;int l141;l282 = (l54[(l1)][(l127)].l48);
l283 = l296;(l54[(l1)][(l127)].l50)-= 1;(l54[(l1)][(l127)].l51)+= 1;(l54[(l1)
][(l127)].l48)-= 1;if(l283!= l282){if((l135 = READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l282,l297))<0){return l135;}l264(l1,l297,l283,0x4000,0);if((
l135 = l166(l1,MEM_BLOCK_ANY,l283,l282,l297))<0){return l135;}if((l135 = l274
(l1,l283,l297))<0){return l135;}}l141 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l283,1);l296 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l282,1);ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l141)<<1)
= ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l296)<<1);if(ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l141)<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l141)<<1)) = SOC_ALPM_128_ADDR_LWR(l141)<<1;
soc_th_alpm_lpm_move_bu_upd(l1,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l141)<<1)),SOC_ALPM_128_ADDR_LWR(l141)<<1);}
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l296)<<1) = NULL;sal_memcpy(l297,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);l264(l1,l297,l282,0x4000,0);if((l135 = l166(l1,
MEM_BLOCK_ANY,l282,l282,l297))<0){return l135;}if((l135 = l274(l1,l282,l297))
<0){return l135;}if((l54[(l1)][(l127)].l50) == 0){l291 = (l54[(l1)][(l127)].
l49);assert(l291!= -1);l292 = (l54[(l1)][(l127)].next);(l54[(l1)][(l291)].
next) = l292;(l54[(l1)][(l291)].l51)+= (l54[(l1)][(l127)].l51);(l54[(l1)][(
l127)].l51) = 0;if(l292!= -1){(l54[(l1)][(l292)].l49) = l291;}(l54[(l1)][(
l127)].next) = -1;(l54[(l1)][(l127)].l49) = -1;(l54[(l1)][(l127)].l47) = -1;(
l54[(l1)][(l127)].l48) = -1;}return(l135);}int soc_th_alpm_128_lpm_vrf_get(
int unit,void*lpm_entry,int*l22,int*l298){int l149;if(((l103[(unit)]->l92)!= 
NULL)){l149 = soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,VRF_ID_0_LWRf
);*l298 = l149;if(soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,
VRF_ID_MASK0_LWRf)){*l22 = l149;}else if(!soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l103[(unit)]->l94))){*
l22 = SOC_L3_VRF_GLOBAL;*l298 = SOC_VRF_MAX(unit)+1;}else{*l22 = 
SOC_L3_VRF_OVERRIDE;}}else{*l22 = SOC_L3_VRF_DEFAULT;}return(SOC_E_NONE);}
static int l299(int l1,void*entry,int*l12){int l127=0;int l135;int l149;int
l300;l135 = l133(l1,entry,&l127);if(l135<0){return l135;}l127+= 0;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,entry,&l149,&l135));l300 = 
soc_th_alpm_mode_get(l1);switch(l149){case SOC_L3_VRF_GLOBAL:if((l300 == 
SOC_ALPM_MODE_PARALLEL)||(l300 == SOC_ALPM_MODE_TCAM_ALPM)){*l12 = l127+((3*(
128+2+1))/3);}else{*l12 = l127;}break;case SOC_L3_VRF_OVERRIDE:*l12 = l127+2*
((3*(128+2+1))/3);break;default:if((l300 == SOC_ALPM_MODE_PARALLEL)||(l300 == 
SOC_ALPM_MODE_TCAM_ALPM)){*l12 = l127;}else{*l12 = l127+((3*(128+2+1))/3);}
break;}return(SOC_E_NONE);}static int l9(int l1,void*key_data,void*l10,int*
l11,int*l12,int*l5){int l135;int l128;int l127 = 0;*l5 = L3_DEFIP_MODE_128;
l299(l1,key_data,&l127);*l12 = l127;if(l268(l1,key_data,l127,&l128) == 
SOC_E_NONE){*l11 = l128;if((l135 = READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(
*l5)?*l11:(*l11>>1),l10))<0){return l135;}return(SOC_E_NONE);}else{return(
SOC_E_NOT_FOUND);}}int soc_th_alpm_128_lpm_init(int l1){int l273;int l134;int
l301;int l302;if(!soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL)
;}l273 = (3*(128+2+1));l302 = sizeof(l52)*(l273);if((l54[(l1)]!= NULL)){
SOC_IF_ERROR_RETURN(soc_th_alpm_128_deinit(l1));}l103[l1] = sal_alloc(sizeof(
l101),"lpm_128_field_state");if(NULL == l103[l1]){return(SOC_E_MEMORY);}(l103
[l1])->l56 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l103[l1]
)->l57 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l103[l1])
->l58 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l103[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l103[l1])->l60 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l103[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l103[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l103[l1])->l65 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l103[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l103[l1])->l66 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l103[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l103[l1])->l69 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l103[l1])->
l67 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l103[
l1])->l70 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l103[l1])->l68 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l103[l1])->l73 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l103[l1])->l71 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l103[l1])->l74 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l103[l1])->l72 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l103[l1])->l77 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l103[l1])->l75 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l103[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l103[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l103[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l103[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l103[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l103[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l103[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l103[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l103[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l103[l1])->l88 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l103[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l103[l1])->l89 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l103[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l103[l1])->l92 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l103[l1])->
l90 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l103[l1
])->l93 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(
l103[l1])->l91 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
VRF_ID_MASK1_UPRf);(l103[l1])->l94 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l103[l1])->l95 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l103[l1])->l96 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l103[l1])->l97 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_SUB_BKT_PTRf);(l103[l1])->l98 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l103[l1])->l99 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf);(l103[l1])->l100 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_POOL_NUMBERf);(l54[(l1)]
) = sal_alloc(l302,"LPM 128 prefix info");if(NULL == (l54[(l1)])){sal_free(
l103[l1]);l103[l1] = NULL;return(SOC_E_MEMORY);}SOC_ALPM_LPM_LOCK(l1);
sal_memset((l54[(l1)]),0,l302);for(l134 = 0;l134<l273;l134++){(l54[(l1)][(
l134)].l47) = -1;(l54[(l1)][(l134)].l48) = -1;(l54[(l1)][(l134)].l49) = -1;(
l54[(l1)][(l134)].next) = -1;(l54[(l1)][(l134)].l50) = 0;(l54[(l1)][(l134)].
l51) = 0;}l301 = soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(
SOC_URPF_STATUS_GET(l1)){l301>>= 1;}if((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM
)){(l54[(l1)][(((3*(128+2+1))-1))].l48) = (l301>>1)-1;(l54[(l1)][(((((3*(128+
2+1))/3)-1)))].l51) = l301>>1;(l54[(l1)][((((3*(128+2+1))-1)))].l51) = (l301-
(l54[(l1)][(((((3*(128+2+1))/3)-1)))].l51));}else{(l54[(l1)][((((3*(128+2+1))
-1)))].l51) = l301;}if((l114[(l1)])!= NULL){if(l122((l114[(l1)]))<0){
SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_INTERNAL;}(l114[(l1)]) = NULL;}if(l120(
l1,l301*2,l301,&(l114[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;
}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}int soc_th_alpm_128_lpm_deinit(
int l1){if(!soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}
SOC_ALPM_LPM_LOCK(l1);if((l114[(l1)])!= NULL){l122((l114[(l1)]));(l114[(l1)])
= NULL;}if((l54[(l1)]!= NULL)){sal_free(l103[l1]);l103[l1] = NULL;sal_free((
l54[(l1)]));(l54[(l1)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}
static int l2(int l1,void*l3,int*l170){int l127;int index;int l5;uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];int l135 = SOC_E_NONE;int l303 = 0;sal_memcpy(l10,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);SOC_ALPM_LPM_LOCK(l1);l135 = l9(l1,l3,l10,&index,&l127
,&l5);if(l135 == SOC_E_NOT_FOUND){l135 = l289(l1,l127,l5,l10,&index);if(l135<
0){SOC_ALPM_LPM_UNLOCK(l1);return(l135);}}else{l303 = 1;}*l170 = index;if(
l135 == SOC_E_NONE){soc_th_alpm_128_lpm_state_dump(l1);LOG_INFO(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"soc_th_alpm_128_lpm_insert: %d %d\n"),index,
l127));if(!l303){l264(l1,l3,index,0x4000,0);}l135 = l166(l1,MEM_BLOCK_ANY,
index,index,l3);if(l135>= 0){l135 = l274(l1,index,l3);}}SOC_ALPM_LPM_UNLOCK(
l1);return(l135);}static int soc_th_alpm_128_lpm_delete(int l1,void*key_data)
{int l127;int index;int l5;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l135 = 
SOC_E_NONE;SOC_ALPM_LPM_LOCK(l1);l135 = l9(l1,key_data,l10,&index,&l127,&l5);
if(l135 == SOC_E_NONE){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_lpm_delete: %d %d\n"),index,l127));l267(l1,key_data,index);l135 = 
l295(l1,l127,l5,l10,index);}soc_th_alpm_128_lpm_state_dump(l1);
SOC_ALPM_LPM_UNLOCK(l1);return(l135);}static int l13(int l1,void*key_data,
void*l10,int*l11){int l127;int l135;int l5;SOC_ALPM_LPM_LOCK(l1);l135 = l9(l1
,key_data,l10,l11,&l127,&l5);SOC_ALPM_LPM_UNLOCK(l1);return(l135);}static int
l4(int unit,void*key_data,int l5,int l6,int l7,defip_aux_scratch_entry_t*l8){
uint32 l137;uint32 l213[4] = {0,0,0,0};int l127 = 0;int l135 = SOC_E_NONE;
l137 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,VALID0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,VALIDf,l137);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,MODE0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,MODEf,l137);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,ENTRY_TYPE0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,ENTRY_TYPEf,0);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,GLOBAL_ROUTEf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,GLOBAL_ROUTEf,l137);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,ECMPf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,ECMPf,l137);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,ECMP_PTRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,ECMP_PTRf,l137);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,NEXT_HOP_INDEXf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,NEXT_HOP_INDEXf,l137);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,PRIf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,PRIf,l137);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,RPEf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,RPEf,l137);l137 =
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,VRF_ID_0_LWRf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,VRFf,l137);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,DB_TYPEf,l6);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,DST_DISCARDf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,DST_DISCARDf,l137);l137 = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,CLASS_IDf);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,CLASS_IDf,l137);l213[0] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR0_LWRf);l213[1] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR1_LWRf);l213[2] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR0_UPRf);l213[3] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,key_data,IP_ADDR1_UPRf);
soc_mem_field_set(unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)l8,IP_ADDRf,(uint32*)
l213);l135 = l133(unit,key_data,&l127);if(SOC_FAILURE(l135)){return l135;}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,IP_LENGTHf,l127);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,REPLACE_LENf,l7);return(
SOC_E_NONE);}static int l14(int unit,void*lpm_entry,void*l15,void*l16,
soc_mem_t l17,uint32 l18,uint32*l304,int l20){uint32 l137;uint32 l213[4];int
l127 = 0;int l135 = SOC_E_NONE;uint32 l19 = 0;sal_memset(l15,0,
soc_mem_entry_words(unit,l17)*4);l137 = soc_mem_field32_get(unit,
L3_DEFIP_PAIR_128m,lpm_entry,HITf);soc_mem_field32_set(unit,l17,l15,HITf,l137
);l137 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf);
soc_mem_field32_set(unit,l17,l15,VALIDf,l137);l137 = soc_mem_field32_get(unit
,L3_DEFIP_PAIR_128m,lpm_entry,ECMPf);soc_mem_field32_set(unit,l17,l15,ECMPf,
l137);l137 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,ECMP_PTRf)
;soc_mem_field32_set(unit,l17,l15,ECMP_PTRf,l137);l137 = soc_mem_field32_get(
unit,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf);soc_mem_field32_set(unit,
l17,l15,NEXT_HOP_INDEXf,l137);l137 = soc_mem_field32_get(unit,
L3_DEFIP_PAIR_128m,lpm_entry,PRIf);soc_mem_field32_set(unit,l17,l15,PRIf,l137
);l137 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,RPEf);
soc_mem_field32_set(unit,l17,l15,RPEf,l137);l137 = soc_mem_field32_get(unit,
L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf);soc_mem_field32_set(unit,l17,l15,
DST_DISCARDf,l137);l137 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,
lpm_entry,SRC_DISCARDf);soc_mem_field32_set(unit,l17,l15,SRC_DISCARDf,l137);
l137 = soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,CLASS_IDf);
soc_mem_field32_set(unit,l17,l15,CLASS_IDf,l137);soc_mem_field32_set(unit,l17
,l15,SUB_BKT_PTRf,ALPM_BKT_SIDX(l20));l213[0] = soc_mem_field32_get(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf);l213[1] = soc_mem_field32_get(
unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf);l213[2] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf);l213[3] = 
soc_mem_field32_get(unit,L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf);
soc_mem_field_set(unit,l17,(uint32*)l15,KEYf,(uint32*)l213);l135 = l133(unit,
lpm_entry,&l127);if(SOC_FAILURE(l135)){return l135;}if((l127 == 0)&&(l213[0]
== 0)&&(l213[1] == 0)&&(l213[2] == 0)&&(l213[3] == 0)){l19 = 1;}if(l304!= 
NULL){*l304 = l19;}soc_mem_field32_set(unit,l17,l15,LENGTHf,l127);if(l16 == 
NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){sal_memset(l16,0,
soc_mem_entry_words(unit,l17)*4);sal_memcpy(l16,l15,soc_mem_entry_words(unit,
l17)*4);soc_mem_field32_set(unit,l17,l16,DST_DISCARDf,0);soc_mem_field32_set(
unit,l17,l16,RPEf,0);soc_mem_field32_set(unit,l17,l16,SRC_DISCARDf,l18&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(unit,l17,l16,DEFAULTROUTEf,l19)
;}return(SOC_E_NONE);}static int l21(int unit,void*l15,soc_mem_t l17,int l5,
int l22,int l23,int index,void*lpm_entry){uint32 l137;uint32 l213[4];uint32
l149,l305;sal_memset(lpm_entry,0,soc_mem_entry_words(unit,L3_DEFIP_PAIR_128m)
*4);l137 = soc_mem_field32_get(unit,l17,l15,HITf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,HITf,l137);l137 = soc_mem_field32_get(unit,l17,
l15,VALIDf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_LWRf
,l137);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_LWRf,l137
);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID0_UPRf,l137);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VALID1_UPRf,l137);l137 = 
soc_mem_field32_get(unit,l17,l15,ECMPf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,ECMPf,l137);l137 = soc_mem_field32_get(unit,l17,
l15,ECMP_PTRf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ECMP_PTRf,l137);l137 = soc_mem_field32_get(unit,l17,l15,NEXT_HOP_INDEXf);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,NEXT_HOP_INDEXf,l137);
l137 = soc_mem_field32_get(unit,l17,l15,PRIf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,PRIf,l137);l137 = soc_mem_field32_get(unit,l17,
l15,RPEf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,RPEf,l137);
l137 = soc_mem_field32_get(unit,l17,l15,DST_DISCARDf);soc_mem_field32_set(
unit,L3_DEFIP_PAIR_128m,lpm_entry,DST_DISCARDf,l137);l137 = 
soc_mem_field32_get(unit,l17,l15,SRC_DISCARDf);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,SRC_DISCARDf,l137);l137 = soc_mem_field32_get(
unit,l17,l15,CLASS_IDf);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry
,CLASS_IDf,l137);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
ALG_BKT_PTRf,ALPM_BKT_IDX(l23));soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,
lpm_entry,ALG_SUB_BKT_PTRf,ALPM_BKT_SIDX(l23));soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,ALG_HIT_IDXf,index);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_LWRf,3);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_LWRf,3);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK0_UPRf,3);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,MODE_MASK1_UPRf,3);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_LWRf,1);soc_mem_field32_set(
unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_LWRf,1);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK0_UPRf,1
);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,ENTRY_TYPE_MASK1_UPRf
,1);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_LWRf,3);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_LWRf,3);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE0_UPRf,3);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,MODE1_UPRf,3);
soc_mem_field_get(unit,l17,l15,KEYf,l213);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_LWRf,l213[0]);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_LWRf,l213[1]);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR0_UPRf,l213[2]);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,IP_ADDR1_UPRf,l213[3]);l213[0] = l213[1] = l213[
2] = l213[3] = 0;l137 = soc_mem_field32_get(unit,l17,l15,LENGTHf);l142(unit,
lpm_entry,l137);if(l22 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_HIGHf,1);soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,GLOBAL_ROUTEf,1);l149 = 0;l305 = 0;}else if(l22
== SOC_L3_VRF_GLOBAL){soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,
GLOBAL_ROUTEf,1);l149 = 0;l305 = 0;}else{l149 = l22;l305 = SOC_VRF_MAX(unit);
}soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_LWRf,l149);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_LWRf,l149);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_0_UPRf,l149);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_1_UPRf,l149);
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_LWRf,l305)
;soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_LWRf,l305
);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK0_UPRf,
l305);soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,VRF_ID_MASK1_UPRf
,l305);return(SOC_E_NONE);}int soc_th_alpm_128_warmboot_pivot_add(int unit,
int l5,void*lpm_entry,int l306,int bktid){int l135 = SOC_E_NONE;uint32 key[4]
= {0,0,0,0};alpm_pivot_t*l32 = NULL;alpm_bucket_handle_t*l201 = NULL;int l149
= 0,l22 = 0;uint32 l307;trie_t*l231 = NULL;uint32 prefix[5] = {0};int l19 = 0
;l135 = _alpm_128_prefix_create(unit,lpm_entry,prefix,&l307,&l19);
SOC_IF_ERROR_RETURN(l135);SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(
unit,lpm_entry,&l149,&l22));l306 = soc_th_alpm_physical_idx(unit,
L3_DEFIP_PAIR_128m,l306,l5);l201 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l201 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for PIVOT trie node \n")));return SOC_E_NONE;
}sal_memset(l201,0,sizeof(*l201));l32 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l32 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for PIVOT trie node \n")));sal_free(l201);
return SOC_E_MEMORY;}sal_memset(l32,0,sizeof(*l32));PIVOT_BUCKET_HANDLE(l32) = 
l201;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l32));key[0] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR0_LWRf);key[1] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR1_LWRf);key[2] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR0_UPRf);key[3] = 
soc_L3_DEFIP_PAIR_128m_field32_get(unit,lpm_entry,IP_ADDR1_UPRf);
PIVOT_BUCKET_INDEX(l32) = bktid;PIVOT_TCAM_INDEX(l32) = SOC_ALPM_128_ADDR_LWR
(l306)<<1;if(!(((l149 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(unit) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l149 == SOC_L3_VRF_GLOBAL))))){l231 = 
VRF_PIVOT_TRIE_IPV6_128(unit,l22);if(l231 == NULL){trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(unit,l22));l231 = 
VRF_PIVOT_TRIE_IPV6_128(unit,l22);}sal_memcpy(l32->key,prefix,sizeof(prefix))
;l32->len = l307;l135 = trie_insert(l231,l32->key,NULL,l32->len,(trie_node_t*
)l32);if(SOC_FAILURE(l135)){sal_free(l201);sal_free(l32);return l135;}}
ALPM_TCAM_PIVOT(unit,SOC_ALPM_128_ADDR_LWR(l306)<<1) = l32;PIVOT_BUCKET_VRF(
l32) = l22;PIVOT_BUCKET_IPV6(l32) = l5;PIVOT_BUCKET_ENT_CNT_UPDATE(l32);if(
key[0] == 0&&key[1] == 0&&key[2] == 0&&key[3] == 0){PIVOT_BUCKET_DEF(l32) = 
TRUE;}VRF_PIVOT_REF_INC(unit,l22,l5);return l135;}static int l308(int unit,
int l5,void*lpm_entry,void*l15,soc_mem_t l17,int l306,int bktid,int l309){int
l310;int l22;int l135 = SOC_E_NONE;int l19 = 0;uint32 prefix[5] = {0,0,0,0,0}
;uint32 l33;defip_pair_128_entry_t l311;trie_t*l40 = NULL;trie_t*l31 = NULL;
trie_node_t*l191 = NULL;payload_t*l312 = NULL;payload_t*l211 = NULL;
alpm_pivot_t*pivot_pyld = NULL;if((NULL == lpm_entry)||(NULL == l15)){return
SOC_E_PARAM;}SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&
l310,&l22));l17 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(l21(unit,l15,
l17,l5,l310,bktid,l306,&l311));l135 = _alpm_128_prefix_create(unit,&l311,
prefix,&l33,&l19);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(unit,"prefix create failed\n")));return l135;}pivot_pyld = ALPM_TCAM_PIVOT(
unit,l306);l40 = PIVOT_BUCKET_TRIE(pivot_pyld);l312 = sal_alloc(sizeof(
payload_t),"Payload for Key");if(NULL == l312){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for trie node.\n")));return
SOC_E_MEMORY;}l211 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");
if(NULL == l211){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node\n")));sal_free(l312);return
SOC_E_MEMORY;}sal_memset(l312,0,sizeof(*l312));sal_memset(l211,0,sizeof(*l211
));sal_memcpy(l312->key,prefix,sizeof(l312->key));l312->len = l33;l312->index
= l309;sal_memcpy(l211,l312,sizeof(*l312));l135 = trie_insert(l40,prefix,NULL
,l33,(trie_node_t*)l312);if(SOC_FAILURE(l135)){goto l313;}if(l5){l31 = 
VRF_PREFIX_TRIE_IPV6_128(unit,l22);}if(!l19){l135 = trie_insert(l31,prefix,
NULL,l33,(trie_node_t*)l211);if(SOC_FAILURE(l135)){goto l314;}}return l135;
l314:(void)trie_delete(l40,prefix,l33,&l191);l312 = (payload_t*)l191;l313:
sal_free(l312);sal_free(l211);return l135;}static int l315(int unit,int l28,
int l22,int l232,int bkt_ptr){int l135 = SOC_E_NONE;uint32 l33;uint32 key[5] = 
{0,0,0,0,0};trie_t*l316 = NULL;payload_t*l221 = NULL;defip_pair_128_entry_t*
lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(defip_pair_128_entry_t),
"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"unable to allocate memory for LPM entry\n")));return
SOC_E_MEMORY;}l24(unit,key,0,l22,l28,lpm_entry,0,1);if(l22 == SOC_VRF_MAX(
unit)+1){soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,GLOBAL_ROUTEf,1);}
else{soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,DEFAULT_MISSf,1);}
soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,ALG_BKT_PTRf,ALPM_BKT_IDX(
bkt_ptr));soc_L3_DEFIP_PAIR_128m_field32_set(unit,lpm_entry,ALG_SUB_BKT_PTRf,
ALPM_BKT_SIDX(bkt_ptr));VRF_TRIE_DEFAULT_ROUTE_IPV6_128(unit,l22) = lpm_entry
;trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(unit,l22));l316 = 
VRF_PREFIX_TRIE_IPV6_128(unit,l22);l221 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l221 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l221,0,sizeof(*l221));l221->key[0] = key[0];l221->
key[1] = key[1];l221->len = l33 = 0;l135 = trie_insert(l316,key,NULL,l33,&(
l221->node));if(SOC_FAILURE(l135)){sal_free(l221);return l135;}
VRF_TRIE_INIT_DONE(unit,l22,l28,1);return l135;}int
soc_th_alpm_128_warmboot_prefix_insert(int unit,int l5,void*lpm_entry,void*
l15,int l306,int bktid,int l309){int l310;int l22;int l135 = SOC_E_NONE;
soc_mem_t l17;l306 = soc_th_alpm_physical_idx(unit,L3_DEFIP_PAIR_128m,l306,l5
);l306 = SOC_ALPM_128_ADDR_LWR(l306)<<1;l17 = L3_DEFIP_ALPM_IPV6_128m;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&l310,&l22));
if(((l310 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(unit) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l310 == SOC_L3_VRF_GLOBAL)))){return(l135);}if(!
VRF_TRIE_INIT_COMPLETED(unit,l22,l5)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U
(unit,"VRF %d is not initialized\n"),l22));l135 = l315(unit,l5,l22,l306,bktid
);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d/%d trie init \n""failed\n"),l22,l5));return l135;}}l135 = l308(unit,
l5,lpm_entry,l15,l17,l306,bktid,l309);if(l135!= SOC_E_NONE){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unit %d : ""Route Insertion Failed :%s\n"),
unit,soc_errmsg(l135)));return(l135);}soc_th_alpm_bu_upd(unit,bktid,l306,l22,
l5,1);VRF_TRIE_ROUTES_INC(unit,l22,l5);return(l135);}int
soc_th_alpm_128_warmboot_bucket_bitmap_set(int l1,int l22,int l28,int l234){
int l317 = 1;SHR_BITDCL*l318 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l1);if(l22 == 
SOC_VRF_MAX(l1)+1){l318 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l1);}if(
SOC_TH_ALPM_SCALE_CHECK(l1,l28)){l317 = 2;}SHR_BITSET_RANGE(l318,l234,l317);
return SOC_E_NONE;}int soc_th_alpm_128_warmboot_lpm_reinit_done(int unit){int
l232;int l319 = ((3*(128+2+1))-1);int l301 = soc_mem_index_count(unit,
L3_DEFIP_PAIR_128m);if(SOC_URPF_STATUS_GET(unit)){l301>>= 1;}if(
soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_COMBINED){(l54[(unit)][(((3*(128+
2+1))-1))].l49) = -1;for(l232 = ((3*(128+2+1))-1);l232>-1;l232--){if(-1 == (
l54[(unit)][(l232)].l47)){continue;}(l54[(unit)][(l232)].l49) = l319;(l54[(
unit)][(l319)].next) = l232;(l54[(unit)][(l319)].l51) = (l54[(unit)][(l232)].
l47)-(l54[(unit)][(l319)].l48)-1;l319 = l232;}(l54[(unit)][(l319)].next) = -1
;(l54[(unit)][(l319)].l51) = l301-(l54[(unit)][(l319)].l48)-1;}else{(l54[(
unit)][(((3*(128+2+1))-1))].l49) = -1;for(l232 = ((3*(128+2+1))-1);l232>(((3*
(128+2+1))-1)/3);l232--){if(-1 == (l54[(unit)][(l232)].l47)){continue;}(l54[(
unit)][(l232)].l49) = l319;(l54[(unit)][(l319)].next) = l232;(l54[(unit)][(
l319)].l51) = (l54[(unit)][(l232)].l47)-(l54[(unit)][(l319)].l48)-1;l319 = 
l232;}(l54[(unit)][(l319)].next) = -1;(l54[(unit)][(l319)].l51) = l301-(l54[(
unit)][(l319)].l48)-1;l319 = (((3*(128+2+1))-1)/3);(l54[(unit)][((((3*(128+2+
1))-1)/3))].l49) = -1;for(l232 = ((((3*(128+2+1))-1)/3)-1);l232>-1;l232--){if
(-1 == (l54[(unit)][(l232)].l47)){continue;}(l54[(unit)][(l232)].l49) = l319;
(l54[(unit)][(l319)].next) = l232;(l54[(unit)][(l319)].l51) = (l54[(unit)][(
l232)].l47)-(l54[(unit)][(l319)].l48)-1;l319 = l232;}(l54[(unit)][(l319)].
next) = -1;(l54[(unit)][(l319)].l51) = (l301>>1)-(l54[(unit)][(l319)].l48)-1;
}return(SOC_E_NONE);}int soc_th_alpm_128_warmboot_lpm_reinit(int unit,int l5,
int l232,void*lpm_entry){int l12;l264(unit,lpm_entry,l232,0x4000,0);
SOC_IF_ERROR_RETURN(l299(unit,lpm_entry,&l12));if((l54[(unit)][(l12)].l50) == 
0){(l54[(unit)][(l12)].l47) = l232;(l54[(unit)][(l12)].l48) = l232;}else{(l54
[(unit)][(l12)].l48) = l232;}(l54[(unit)][(l12)].l50)++;return(SOC_E_NONE);}
int soc_th_alpm_128_bucket_sanity_check(int l1,soc_mem_t l171,int index){int
l135 = SOC_E_NONE;int l134,l239,l248,l5,tcam_index = -1;int l149 = 0,l22;
uint32 l150 = 0;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l320,l321,l322;
defip_pair_128_entry_t lpm_entry;int l253,l323;soc_mem_t l17 = 
L3_DEFIP_ALPM_IPV6_128m;int l324 = 8,bkt_ptr,l325;int l326 = 0;soc_field_t
l327 = VALID0_LWRf;soc_field_t l328 = GLOBAL_HIGHf;soc_field_t l329 = 
ALG_BKT_PTRf;soc_field_t l330 = ALG_SUB_BKT_PTRf;l5 = L3_DEFIP_MODE_128;l253 = 
soc_mem_index_min(l1,l171);l323 = soc_mem_index_max(l1,l171);if((index>= 0)&&
(index<l253||index>l323)){return SOC_E_PARAM;}else if(index>= 0){l253 = index
;l323 = index;}SOC_ALPM_LPM_LOCK(l1);for(l134 = l253;l134<= l323;l134++){l135
= soc_mem_read(l1,l171,MEM_BLOCK_ANY,l134,(void*)l10);if(SOC_FAILURE(l135)){
continue;}if(soc_mem_field32_get(l1,l171,(void*)l10,l327) == 0||
soc_mem_field32_get(l1,l171,(void*)l10,l328) == 1){continue;}bkt_ptr = 
soc_mem_field32_get(l1,l171,(void*)l10,l329);l325 = soc_mem_field32_get(l1,
l171,(void*)l10,l330);l135 = soc_th_alpm_128_lpm_vrf_get(l1,l10,&l149,&l22);
if(SOC_FAILURE(l135)){continue;}if(SOC_TH_ALPM_SCALE_CHECK(l1,l5)){l324<<= 1;
}tcam_index = -1;for(l239 = 0;l239<l324;l239++){l135 = _soc_th_alpm_mem_index
(l1,l17,bkt_ptr,l239,l150,&l248);if(SOC_FAILURE(l135)){continue;}l135 = 
soc_mem_read(l1,l17,MEM_BLOCK_ANY,l248,(void*)l10);if(SOC_FAILURE(l135)){
break;}if(!soc_mem_field32_get(l1,l17,(void*)l10,VALIDf)){continue;}if(l325!= 
soc_mem_field32_get(l1,l17,(void*)l10,SUB_BKT_PTRf)){continue;}l135 = l21(l1,
(void*)l10,l17,l5,l149,ALPM_BKTID(bkt_ptr,l325),0,&lpm_entry);if(SOC_FAILURE(
l135)){continue;}l135 = _soc_th_alpm_128_find(l1,(void*)&lpm_entry,l17,(void*
)l10,&l320,&l321,&l322,FALSE);if(SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"\tLaunched AUX operation for "
"index %d bucket [%d,%d] sanity check failed\n"),l134,bkt_ptr,l325));l326++;
continue;}if(l321!= ALPM_BKTID(bkt_ptr,l325)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tEntry at index %d does not belong "
"to bucket [%d,%d](from bucket [%d,%d])\n"),l322,bkt_ptr,l325,ALPM_BKT_IDX(
l321),ALPM_BKT_SIDX(l321)));l326++;}if(tcam_index == -1){tcam_index = l320;
continue;}if(tcam_index!= l320){int l154,l155;l154 = soc_th_alpm_logical_idx(
l1,l171,SOC_ALPM_128_DEFIP_TO_PAIR(tcam_index>>1),1);l155 = 
soc_th_alpm_logical_idx(l1,l171,SOC_ALPM_128_DEFIP_TO_PAIR(l320>>1),1);
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tAliased bucket [%d,%d](returned "
"bucket [%d,%d]) found from TCAM1 %d and TCAM2 %d\n"),bkt_ptr,l325,
ALPM_BKT_IDX(l321),ALPM_BKT_SIDX(l321),l154,l155));l326++;}}}
SOC_ALPM_LPM_UNLOCK(l1);if(l326 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1
,"\tMemory %s index %d Bucket sanity check passed\n"),SOC_MEM_NAME(l1,l171),
index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l1,l171),index,l326));return l326;}
int soc_th_alpm_128_pivot_sanity_check(int l1,soc_mem_t l171,int index){int
l134,l331;int l248,l253,l323;int l135 = SOC_E_NONE;int*l332 = NULL;int l320,
l321,l322;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int bkt_ptr,l325,l333,l334;
soc_mem_t l335;int l22,l336;soc_mem_t l337 = L3_DEFIP_ALPM_IPV6_128m;int l5 = 
L3_DEFIP_MODE_128;int l326 = 0;soc_field_t l327 = VALID0_LWRf;soc_field_t l328
= GLOBAL_HIGHf;soc_field_t l338 = NEXT_HOP_INDEXf;soc_field_t l329 = 
ALG_BKT_PTRf;soc_field_t l330 = ALG_SUB_BKT_PTRf;l253 = soc_mem_index_min(l1,
l171);l323 = soc_mem_index_max(l1,l171);if((index>= 0)&&(index<l253||index>
l323)){return SOC_E_PARAM;}else if(index>= 0){l253 = index;l323 = index;}l134
= sizeof(int)*SOC_TH_MAX_ALPM_BUCKETS*SOC_TH_MAX_SUB_BUCKETS;l332 = sal_alloc
(l134,"Bucket index array 128");if(l332 == NULL){l135 = SOC_E_MEMORY;return
l135;}sal_memset(l332,0xff,l134);SOC_ALPM_LPM_LOCK(l1);for(l134 = l253;l134<= 
l323;l134++){l135 = soc_mem_read(l1,l171,MEM_BLOCK_ANY,l134,(void*)l10);if(
SOC_FAILURE(l135)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l1,l171),
l134,l135));l326 = l135;continue;}if(soc_mem_field32_get(l1,l171,(void*)l10,
l327) == 0||soc_mem_field32_get(l1,l171,(void*)l10,l328) == 1){continue;}
bkt_ptr = soc_mem_field32_get(l1,l171,(void*)l10,l329);l325 = 
soc_mem_field32_get(l1,l171,(void*)l10,l330);(void)soc_th_alpm_lpm_vrf_get(l1
,l10,&l22,&l336);l135 = soc_th_alpm_bucket_is_assigned(l1,bkt_ptr,l336,l5,&
l331);if(l135 == SOC_E_PARAM||l325>= SOC_TH_MAX_SUB_BUCKETS){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tInvalid bucket pointer [%d,%d] "
"detected, in memory %s index %d\n"),bkt_ptr,l325,SOC_MEM_NAME(l1,l171),l134)
);l326 = l135;continue;}if(bkt_ptr!= 0){if(l332[bkt_ptr*
SOC_TH_MAX_SUB_BUCKETS+l325] == -1){l332[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l325]
= l134;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tDuplicated bucket pointer ""[%d,%d]detected, in memory %s index1 %d and "
"index2 %d\n"),bkt_ptr,l325,SOC_MEM_NAME(l1,l171),l332[bkt_ptr*
SOC_TH_MAX_SUB_BUCKETS+l325],l134));l326 = l135;continue;}}else{continue;}if(
l135>= 0&&l331 == 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tFreed bucket pointer %d ""detected, in memory %s index %d\n"),bkt_ptr,
SOC_MEM_NAME(l1,l171),l134));l326 = l135;continue;}l335 = 
_soc_tomahawk_alpm_bkt_view_get(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,ALPM_BKTID
(bkt_ptr,0)));if(l335!= L3_DEFIP_ALPM_IPV6_128m){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual %s\n"),bkt_ptr,SOC_MEM_NAME(l1,
L3_DEFIP_ALPM_IPV6_128m),SOC_MEM_NAME(l1,l335)));l326 = l135;continue;}l320 = 
-1;l333 = soc_mem_field32_get(l1,l171,(void*)l10,l338);l135 = 
_soc_th_alpm_128_find(l1,l10,l337,(void*)l10,&l320,&l321,&l322,FALSE);if(l320
== -1){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tLaunched AUX operation for PIVOT ""index %d sanity check failed\n"),l134))
;l326 = l135;continue;}l248 = soc_th_alpm_logical_idx(l1,l171,
SOC_ALPM_128_DEFIP_TO_PAIR(l320>>1),1);l135 = soc_mem_read(l1,l171,
MEM_BLOCK_ANY,l248,(void*)l10);if(SOC_FAILURE(l135)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tRead memory %s index %d (nexthop) "
"return %d\n"),SOC_MEM_NAME(l1,l171),l248,l135));l326 = l135;continue;}l334 = 
soc_mem_field32_get(l1,l171,(void*)l10,l338);if(l333!= l334){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d index2 %d \n"),l134,l248));l326 = l135;continue;}}
SOC_ALPM_LPM_UNLOCK(l1);sal_free(l332);if(l326 == 0){LOG_INFO(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check passed\n"),
SOC_MEM_NAME(l1,l171),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check "
"failed, encountered %d error(s)\n"),SOC_MEM_NAME(l1,l171),index,l326));
return SOC_E_FAIL;}
#endif /* ALPM_ENABLE */
