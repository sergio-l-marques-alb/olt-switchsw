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
l4(int l1,void*key_data,int l5,int l6,int l7,int l8,defip_aux_scratch_entry_t
*l9);static int l10(int l1,void*key_data,void*l11,int*l12,int*l13,int*l5,int*
vrf_id,int*vrf);static int l14(int l1,void*key_data,void*l11,int*l12);static
int l15(int unit,void*lpm_entry,void*l16,void*l17,soc_mem_t l18,uint32 l19,
uint32*l20,int l21);static int l22(int unit,void*l16,soc_mem_t l18,int l5,int
vrf,int l23,int index,void*lpm_entry);static int l24(int unit,uint32*key,int
len,int vrf,int l5,defip_pair_128_entry_t*lpm_entry,int l25,int l26);static
int l27(int l1,int vrf,int l28);static int l29(int l1,alpm_pfx_info_t*l30,
trie_t*l31,uint32*l32,uint32 l33,trie_node_t*l34,defip_pair_128_entry_t*
lpm_entry,uint32*l35);static int _soc_th_alpm_128_move_inval(int l1,soc_mem_t
l18,alpm_mem_prefix_array_t*l36,int*l37);extern int soc_th_get_alpm_banks(int
unit);extern int soc_th_alpm_mode_get(int l1);extern int
_soc_th_alpm_free_pfx_trie(int l1,trie_t*l31,trie_t*l38,payload_t*
new_pfx_pyld,int*l37,int bktid,int vrf,int l28);extern int
_soc_th_alpm_rollback_bkt_move(int l1,int l28,void*key_data,soc_mem_t l18,
alpm_pivot_t*l39,alpm_pivot_t*l40,alpm_mem_prefix_array_t*l41,int*l37,int l42
);extern void soc_th_alpm_lpm_move_bu_upd(int l1,int l21,int l43);extern int
soc_th_alpm_lpm_delete(int l1,void*key_data);int alpm_128_split_count;typedef
struct l44{int l45;int l46;int l47;int next;int l48;int l49;}l50,*l51;static
l51 l52[SOC_MAX_NUM_DEVICES];typedef struct l53{soc_field_info_t*l54;
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
soc_field_info_t*l106;soc_field_info_t*l107;}l108,*l109;static l109 l110[
SOC_MAX_NUM_DEVICES];typedef struct l111{int unit;int l112;int l113;uint16*
l114;uint16*l115;}l116;typedef uint32 l117[9];typedef int(*l118)(l117 l119,
l117 l120);static l116*l121[SOC_MAX_NUM_DEVICES];static void l122(int l1,void
*l11,int index,l117 l123);static uint16 l124(uint8*l125,int l126);static int
l127(int unit,int l112,int l113,l116**l128);static int l129(l116*l130);static
int l131(l116*l132,l118 l133,l117 entry,int l134,uint16*l135);static int l136
(l116*l132,l118 l133,l117 entry,int l134,uint16 l137,uint16 l37);static int
l138(l116*l132,l118 l133,l117 entry,int l134,uint16 l139);static int l140(
l116*l132,l118 l133,l117 entry,int l134,uint16 l141);static int l142(int,void
*,int*,int*,int*);static int l143(int l1,const void*entry,int*l134){int l144,
l145;int l146[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,IP_ADDR_MASK0_UPRf,
IP_ADDR_MASK1_UPRf};uint32 l147;l147 = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,l146[0]);if((l145 = _ipmask2pfx(l147,l134))<0){
return(l145);}for(l144 = 1;l144<4;l144++){l147 = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,l146[l144]);if(*l134){if(l147!= 0xffffffff){return(
SOC_E_PARAM);}*l134+= 32;}else{if((l145 = _ipmask2pfx(l147,l134))<0){return(
l145);}}}return SOC_E_NONE;}static void l148(uint32*l149,int l33,int l28){
uint32 l150,l151,l45,prefix[6];int l144;sal_memcpy(prefix,l149,sizeof(uint32)
*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(l149,0,sizeof(uint32)*BITS2WORDS(
_MAX_KEY_LEN_144_));l150 = 128-l33;l45 = (l150+31)/32;if((l150%32) == 0){l45
++;}l150 = l150%32;for(l144 = l45;l144<= 4;l144++){prefix[l144]<<= l150;l151 = 
prefix[l144+1]&~(0xffffffff>>l150);l151 = (((32-l150) == 32)?0:(l151)>>(32-
l150));if(l144<4){prefix[l144]|= l151;}}for(l144 = l45;l144<= 4;l144++){l149[
3-(l144-l45)] = prefix[l144];}}static void l152(int unit,void*lpm_entry,int
l13){int l144;soc_field_t l153[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};for(l144 = 0;l144<4;l144++){
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l153[l144],0);}for(l144
= 0;l144<4;l144++){if(l13<= 32)break;soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,l153[3-l144],0xffffffff);l13-= 32;}
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l153[3-l144],~(((l13) == 
32)?0:(0xffffffff)>>(l13)));}int _soc_th_alpm_128_prefix_create(int l1,void*
entry,uint32*l149,uint32*l13,int*l20){int l144;int l134 = 0,l45;int l145 = 
SOC_E_NONE;uint32 l150,l151;uint32 prefix[5];sal_memset(l149,0,sizeof(uint32)
*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(prefix,0,sizeof(uint32)*BITS2WORDS
(_MAX_KEY_LEN_144_));prefix[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(entry),(l110[(l1)]->l64));prefix[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
entry),(l110[(l1)]->l65));prefix[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(entry),(l110[(l1)]->l62));prefix[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
entry),(l110[(l1)]->l63));l145 = l143(l1,entry,&l134);if(SOC_FAILURE(l145)){
return l145;}l150 = 128-l134;l45 = l150/32;l150 = l150%32;for(l144 = l45;l144
<4;l144++){prefix[l144]>>= l150;l151 = prefix[l144+1]&((1<<l150)-1);l151 = ((
(32-l150) == 32)?0:(l151)<<(32-l150));prefix[l144]|= l151;}for(l144 = l45;
l144<4;l144++){l149[4-(l144-l45)] = prefix[l144];}*l13 = l134;if(l20!= NULL){
*l20 = (prefix[0] == 0)&&(prefix[1] == 0)&&(prefix[2] == 0)&&(prefix[3] == 0)
&&(l134 == 0);}return SOC_E_NONE;}int l154(int l1,uint32*prefix,uint32 l33,
int l5,int vrf,int*l155,int*tcam_index,int*bktid){int l145 = SOC_E_NONE;
trie_t*l156;trie_node_t*l157 = NULL;alpm_pivot_t*pivot_pyld;l156 = 
VRF_PIVOT_TRIE_IPV6_128(l1,vrf);l145 = trie_find_lpm(l156,prefix,l33,&l157);
if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l145;}pivot_pyld = (alpm_pivot_t*)l157;*l155 = 
1;*tcam_index = PIVOT_TCAM_INDEX(pivot_pyld);*bktid = PIVOT_BUCKET_INDEX(
pivot_pyld);return SOC_E_NONE;}static int _soc_th_alpm_128_find(int l1,
soc_mem_t l18,void*key_data,int vrf_id,int vrf,void*alpm_data,int*tcam_index,
int*bktid,int*l12,int l158){uint32 l11[SOC_MAX_MEM_FIELD_WORDS];int l28;int
l135;uint32 l6,l7,l159;int l145 = SOC_E_NONE;int l155 = 0;l28 = 
L3_DEFIP_MODE_128;if(vrf_id == 0){if(soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL||soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){
return SOC_E_PARAM;}}soc_th_alpm_bank_disb_get(l1,vrf,&l159);
soc_alpm_db_ent_type_encoding(l1,vrf,&l6,&l7);if(!(((vrf_id == 
SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)
&&(vrf_id == SOC_L3_VRF_GLOBAL))))){if(l158){uint32 prefix[5],l33;int l20 = 0
;l145 = _soc_th_alpm_128_prefix_create(l1,key_data,prefix,&l33,&l20);if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: prefix create failed\n")));return l145;}l145 = l154(l1,
prefix,l33,l28,vrf,&l155,tcam_index,bktid);SOC_IF_ERROR_RETURN(l145);}else{
defip_aux_scratch_entry_t l9;sal_memset(&l9,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l28,l6,l7,0,&
l9));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,PREFIX_LOOKUP,&l9,TRUE,&l155,
tcam_index,bktid));}if(l155){l15(l1,key_data,l11,0,l18,0,0,*bktid);l145 = 
_soc_th_alpm_find_in_bkt(l1,l18,*bktid,l159,l11,alpm_data,&l135,l28);if(
SOC_SUCCESS(l145)){*l12 = l135;}}else{l145 = SOC_E_NOT_FOUND;}}return l145;}
static int l160(int l1,void*key_data,void*alpm_data,void*alpm_sip_data,
soc_mem_t l18,int l135){defip_aux_scratch_entry_t l9;int vrf_id,l28,vrf;int
bktid;uint32 l6,l7,l159;int l145 = SOC_E_NONE;int l155 = 0,l151 = 0;int
tcam_index,index;uint32 l161[SOC_MAX_MEM_FIELD_WORDS];l28 = L3_DEFIP_MODE_128
;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&vrf_id,&vrf));
soc_th_alpm_bank_disb_get(l1,vrf,&l159);soc_alpm_db_ent_type_encoding(l1,vrf,
&l6,&l7);if(!(((vrf_id == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == SOC_L3_VRF_GLOBAL))))){sal_memset(&l9,0,
sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l28,l6,
l7,0,&l9));SOC_ALPM_LPM_LOCK(l1);l145 = _soc_th_alpm_128_find(l1,l18,key_data
,vrf_id,vrf,l161,&tcam_index,&bktid,&index,TRUE);SOC_ALPM_LPM_UNLOCK(l1);
SOC_IF_ERROR_RETURN(l145);soc_mem_field32_set(l1,l18,alpm_data,SUB_BKT_PTRf,
ALPM_BKT_SIDX(bktid));SOC_IF_ERROR_RETURN(soc_mem_write(l1,l18,MEM_BLOCK_ANY,
ALPM_ENT_INDEX(l135),alpm_data));if(SOC_URPF_STATUS_GET(l1)){
soc_mem_field32_set(l1,l18,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry
(l1,ALPM_ENT_INDEX(l135)),alpm_sip_data));}l151 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l9,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l9,REPLACE_LENf,l151);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l9,TRUE,&l155,&tcam_index,&bktid));
if(SOC_URPF_STATUS_GET(l1)){if(l151 == 0){soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l9,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l9,RPEf,0);}soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l9,DB_TYPEf,l6+1);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l9,TRUE,&l155,&tcam_index,&bktid));}
}return l145;}static int l162(int l1,int l163,int l35){int l145,l151,l164,
l165;defip_aux_table_entry_t l166,l167;l164 = SOC_ALPM_128_ADDR_LWR(l163);
l165 = SOC_ALPM_128_ADDR_UPR(l163);l145 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm
,MEM_BLOCK_ANY,l164,&l166);SOC_IF_ERROR_RETURN(l145);l145 = soc_mem_read(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l165,&l167);SOC_IF_ERROR_RETURN(l145);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,BPM_LENGTH0f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,BPM_LENGTH1f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l167,BPM_LENGTH0f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l167,BPM_LENGTH1f,l35);l151 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&l166,DB_TYPE0f);l145 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l164,&l166);
SOC_IF_ERROR_RETURN(l145);l145 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l165,&l167);SOC_IF_ERROR_RETURN(l145);if(SOC_URPF_STATUS_GET(l1
)){l151++;soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,BPM_LENGTH0f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,BPM_LENGTH1f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l167,BPM_LENGTH0f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l167,BPM_LENGTH1f,l35);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,DB_TYPE0f,l151);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l166,DB_TYPE1f,l151);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l167,DB_TYPE0f,l151);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l167,DB_TYPE1f,l151);l164+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,L3_DEFIPm))
/2;l165+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(
l1,L3_DEFIPm))/2;l145 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
l164,&l166);SOC_IF_ERROR_RETURN(l145);l145 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l165,&l167);}return l145;}static int l168(
int l1,int l169,void*entry,defip_aux_table_entry_t*l170,int l171){uint32 l151
,l6,l7,l172 = 0;soc_mem_t l18 = L3_DEFIP_PAIR_128m;soc_mem_t l173 = 
L3_DEFIP_AUX_TABLEm;int l145 = SOC_E_NONE,l134,vrf;void*l174,*l175;l174 = (
void*)l170;l175 = (void*)(l170+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l173,
MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l169),l170));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l173,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l169),l170+1));l151 = 
soc_mem_field32_get(l1,l18,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l173,
l174,VRF0f,l151);l151 = soc_mem_field32_get(l1,l18,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l173,l174,VRF1f,l151);l151 = soc_mem_field32_get(l1,
l18,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l173,l175,VRF0f,l151);l151 = 
soc_mem_field32_get(l1,l18,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l173,
l175,VRF1f,l151);l151 = soc_mem_field32_get(l1,l18,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l173,l174,MODE0f,l151);l151 = soc_mem_field32_get(l1,
l18,entry,MODE1_LWRf);soc_mem_field32_set(l1,l173,l174,MODE1f,l151);l151 = 
soc_mem_field32_get(l1,l18,entry,MODE0_UPRf);soc_mem_field32_set(l1,l173,l175
,MODE0f,l151);l151 = soc_mem_field32_get(l1,l18,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l173,l175,MODE1f,l151);l151 = soc_mem_field32_get(l1,
l18,entry,VALID0_LWRf);soc_mem_field32_set(l1,l173,l174,VALID0f,l151);l151 = 
soc_mem_field32_get(l1,l18,entry,VALID1_LWRf);soc_mem_field32_set(l1,l173,
l174,VALID1f,l151);l151 = soc_mem_field32_get(l1,l18,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l173,l175,VALID0f,l151);l151 = soc_mem_field32_get(l1,
l18,entry,VALID1_UPRf);soc_mem_field32_set(l1,l173,l175,VALID1f,l151);l145 = 
l143(l1,entry,&l134);SOC_IF_ERROR_RETURN(l145);soc_mem_field32_set(l1,l173,
l174,IP_LENGTH0f,l134);soc_mem_field32_set(l1,l173,l174,IP_LENGTH1f,l134);
soc_mem_field32_set(l1,l173,l175,IP_LENGTH0f,l134);soc_mem_field32_set(l1,
l173,l175,IP_LENGTH1f,l134);l151 = soc_mem_field32_get(l1,l18,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l173,l174,IP_ADDR0f,l151);l151 = 
soc_mem_field32_get(l1,l18,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l173,
l174,IP_ADDR1f,l151);l151 = soc_mem_field32_get(l1,l18,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l173,l175,IP_ADDR0f,l151);l151 = soc_mem_field32_get(
l1,l18,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l173,l175,IP_ADDR1f,l151);
l145 = soc_th_alpm_128_lpm_vrf_get(l1,entry,&vrf,&l134);SOC_IF_ERROR_RETURN(
l145);if(SOC_URPF_STATUS_GET(l1)){if(l171>= (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1)){l172 = 1;}}soc_alpm_db_ent_type_encoding(l1,l134,&l6
,&l7);if(vrf == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l1,l173,l174,VALID0f
,0);soc_mem_field32_set(l1,l173,l174,VALID1f,0);soc_mem_field32_set(l1,l173,
l175,VALID0f,0);soc_mem_field32_set(l1,l173,l175,VALID1f,0);l6 = 0;}else{if(
l172){l6++;}}soc_mem_field32_set(l1,l173,l174,DB_TYPE0f,l6);
soc_mem_field32_set(l1,l173,l174,DB_TYPE1f,l6);soc_mem_field32_set(l1,l173,
l175,DB_TYPE0f,l6);soc_mem_field32_set(l1,l173,l175,DB_TYPE1f,l6);l151 = 
soc_mem_field32_get(l1,l18,entry,ENTRY_TYPE0_LWRf);soc_mem_field32_set(l1,
l173,l174,ENTRY_TYPE0f,l151|l7);l151 = soc_mem_field32_get(l1,l18,entry,
ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,l173,l174,ENTRY_TYPE1f,l151|l7);l151
= soc_mem_field32_get(l1,l18,entry,ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,
l173,l175,ENTRY_TYPE0f,l151|l7);l151 = soc_mem_field32_get(l1,l18,entry,
ENTRY_TYPE1_UPRf);soc_mem_field32_set(l1,l173,l175,ENTRY_TYPE1f,l151|l7);if(
l172){l151 = soc_mem_field32_get(l1,l18,entry,ALG_BKT_PTRf);if(l151){l151+= 
SOC_TH_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l18,entry,ALG_BKT_PTRf,
l151);}l151 = soc_mem_field32_get(l1,l18,entry,ALG_SUB_BKT_PTRf);if(l151){
soc_mem_field32_set(l1,l18,entry,ALG_SUB_BKT_PTRf,l151);}}return SOC_E_NONE;}
static int l176(int l1,int l177,int index,int l178,void*entry){
defip_aux_table_entry_t l170[2];l178 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l178,1);SOC_IF_ERROR_RETURN(l168(l1,l178,entry,&l170[0],
index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,
entry));index = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(index),l170));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(index),l170+
1));return SOC_E_NONE;}static int l179(int l1,int l28,int l43,int bktid){int
l145 = SOC_E_NONE;int l180;soc_mem_t l181 = L3_DEFIP_PAIR_128m;
defip_pair_128_entry_t lpm_entry;int l182,l183;l182 = ALPM_BKT_IDX(bktid);
l183 = ALPM_BKT_SIDX(bktid);l180 = soc_th_alpm_logical_idx(l1,l181,
SOC_ALPM_128_DEFIP_TO_PAIR(l43>>1),1);l145 = soc_mem_read(l1,l181,
MEM_BLOCK_ANY,l180,&lpm_entry);SOC_IF_ERROR_RETURN(l145);soc_mem_field32_set(
l1,l181,&lpm_entry,ALG_BKT_PTRf,l182);soc_mem_field32_set(l1,l181,&lpm_entry,
ALG_SUB_BKT_PTRf,l183);l145 = soc_mem_write(l1,l181,MEM_BLOCK_ANY,l180,&
lpm_entry);SOC_IF_ERROR_RETURN(l145);if(SOC_URPF_STATUS_GET(l1)){int l184 = (
soc_mem_index_count(l1,l181)>>1)+soc_th_alpm_logical_idx(l1,l181,
SOC_ALPM_128_DEFIP_TO_PAIR(l43>>1),1);l145 = READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l184,&lpm_entry);SOC_IF_ERROR_RETURN(l145);soc_mem_field32_set(
l1,l181,&lpm_entry,ALG_BKT_PTRf,l182+SOC_TH_ALPM_BUCKET_COUNT(l1));
soc_mem_field32_set(l1,l181,&lpm_entry,ALG_SUB_BKT_PTRf,l183);l145 = 
WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l184,&lpm_entry);
SOC_IF_ERROR_RETURN(l145);}return l145;}int _soc_th_alpm_128_move_trie(int l1
,int l28,int l185,int l186){int*l37 = NULL,*l137 = NULL;int l145 = SOC_E_NONE
,l187,l144,l188;int l135,l43;uint32 l159 = 0;soc_mem_t l18;void*l189,*l190;
uint32 l11[SOC_MAX_MEM_FIELD_WORDS];trie_t*trie = NULL;payload_t*l134 = NULL;
int16 vrf_type = 0;alpm_mem_prefix_array_t*l36 = NULL;
defip_alpm_ipv6_128_entry_t l191,l192;int l193,l194,l195,l196;l188 = sizeof(
int)*MAX_PREFIX_PER_BUCKET;l37 = sal_alloc(l188,"new_index_move");l137 = 
sal_alloc(l188,"old_index_move");l36 = sal_alloc(sizeof(
alpm_mem_prefix_array_t),"prefix_array");if(l37 == NULL||l137 == NULL||l36 == 
NULL){l145 = SOC_E_MEMORY;goto l197;}l193 = ALPM_BKT_IDX(l185);l195 = 
ALPM_BKT_SIDX(l185);l194 = ALPM_BKT_IDX(l186);l196 = ALPM_BKT_SIDX(l186);l18 = 
L3_DEFIP_ALPM_IPV6_128m;l189 = &l191;l190 = &l192;l43 = 
SOC_ALPM_BS_BKT_USAGE_PIVOT(l1,l185);trie = PIVOT_BUCKET_TRIE(ALPM_TCAM_PIVOT
(l1,l43));vrf_type = SOC_ALPM_BS_BKT_USAGE_VRF(l1,l185);
soc_th_alpm_bank_disb_get(l1,vrf_type,&l159);sal_memset(l36,0,sizeof(*l36));
l145 = trie_traverse(trie,_soc_th_alpm_mem_prefix_array_cb,l36,
_TRIE_INORDER_TRAVERSE);do{if((l145)<0){goto l197;}}while(0);sal_memset(l37,-
1,l188);sal_memset(l137,-1,l188);for(l144 = 0;l144<l36->count;l144++){l134 = 
l36->prefix[l144];if(l134->index>0){l145 = soc_mem_read(l1,l18,MEM_BLOCK_ANY,
l134->index,l189);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"soc_mem_read index %d failed\n"),l134->index));goto l198;}
soc_mem_field32_set(l1,l18,l189,SUB_BKT_PTRf,l196);if(SOC_URPF_STATUS_GET(l1)
){l145 = soc_mem_read(l1,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l134->
index),l190);if(SOC_FAILURE(l145)){goto l198;}soc_mem_field32_set(l1,l18,l190
,SUB_BKT_PTRf,l196);}l145 = _soc_th_alpm_insert_in_bkt(l1,l18,l186,l159,l189,
l11,&l135,l28);if(SOC_SUCCESS(l145)){if(SOC_URPF_STATUS_GET(l1)){l145 = 
soc_mem_write(l1,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l135),l190);if(
SOC_FAILURE(l145)){goto l198;}}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1
,"insert to bucket %d failed\n"),l186));goto l198;}l37[l144] = l135;l137[l144
] = l134->index;}}l145 = l179(l1,l28,l43,l186);if(SOC_FAILURE(l145)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_move_relink failed, pivot %d bkt %d\n"),l43,l186));goto l198
;}l145 = _soc_th_alpm_128_move_inval(l1,l18,l36,l37);if(SOC_FAILURE(l145)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_move_inval failed\n")));goto l198;}l145 = 
soc_th_alpm_update_hit_bits(l1,l36->count,l137,l37);if(SOC_FAILURE(l145)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l36->count));l145 = 
SOC_E_NONE;}PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l1,l43)) = ALPM_BKTID(l194,
l196);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l1,l28,l193);
SOC_ALPM_BS_BKT_USAGE_SB_DEL(l1,l193,l195,TRUE,l36->count);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l1,l28,l193);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l1,l28,l194);SOC_ALPM_BS_BKT_USAGE_SB_ADD(l1
,l194,l196,l43,vrf_type,l36->count);SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l1,l28,
l194);l198:if(SOC_FAILURE(l145)){l187 = l179(l1,l28,l43,l185);do{if((l187)<0)
{goto l197;}}while(0);for(l144 = 0;l144<l36->count;l144++){if(l37[l144] == -1
){continue;}l187 = soc_mem_write(l1,l18,MEM_BLOCK_ANY,l37[l144],
soc_mem_entry_null(l1,l18));do{if((l187)<0){goto l197;}}while(0);if(
SOC_URPF_STATUS_GET(l1)){l187 = soc_mem_write(l1,l18,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l37[l144]),soc_mem_entry_null(l1,l18));do{if((l187)
<0){goto l197;}}while(0);}}}l197:if(l37!= NULL){sal_free(l37);}if(l137!= NULL
){sal_free(l137);}if(l36!= NULL){sal_free(l36);}return l145;}static void l199
(int l1,defip_pair_128_entry_t*l200,void*key_data,int tcam_index,alpm_pivot_t
*pivot_pyld){int l145;trie_t*l156 = NULL;int l28,vrf,vrf_id;trie_node_t*l201 = 
NULL;l28 = L3_DEFIP_MODE_128;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,&
vrf_id,&vrf);l145 = soc_th_alpm_128_lpm_delete(l1,l200);if(SOC_FAILURE(l145))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Failure to free new prefix""at %d\n"),
soc_th_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,tcam_index,l28)));}l156 = 
VRF_PIVOT_TRIE_IPV6(l1,vrf);if(ALPM_TCAM_PIVOT(l1,tcam_index<<(l28?1:0))!= 
NULL){l145 = trie_delete(l156,pivot_pyld->key,pivot_pyld->len,&l201);if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l1,tcam_index<<(l28?1:0)) = NULL;VRF_PIVOT_REF_DEC(l1,vrf,l28
);}static int _soc_th_alpm_128_move_inval(int l1,soc_mem_t l18,
alpm_mem_prefix_array_t*l36,int*l37){int l144,l145 = SOC_E_NONE,l187;
defip_alpm_ipv6_128_entry_t l191;void*l202 = NULL,*l203 = NULL;int l204;int*
l205 = NULL;int l206 = FALSE;l204 = sizeof(l191);l202 = sal_alloc(l204*l36->
count,"rb_bufp");l203 = sal_alloc(l204*l36->count,"rb_sip_bufp");l205 = 
sal_alloc(sizeof(*l205)*l36->count,"roll_back_index");if(l202 == NULL||l203 == 
NULL||l205 == NULL){l145 = SOC_E_MEMORY;goto l207;}sal_memset(l205,-1,sizeof(
*l205)*l36->count);for(l144 = 0;l144<l36->count;l144++){payload_t*prefix = 
l36->prefix[l144];if(prefix->index>= 0){l145 = soc_mem_read(l1,l18,
MEM_BLOCK_ANY,prefix->index,(uint8*)l202+l144*l204);if(SOC_FAILURE(l145)){
l144--;l206 = TRUE;break;}if(SOC_URPF_STATUS_GET(l1)){l145 = soc_mem_read(l1,
l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,prefix->index),(uint8*)l203+l144*
l204);if(SOC_FAILURE(l145)){l144--;l206 = TRUE;break;}}l145 = soc_mem_write(
l1,l18,MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null(l1,l18));if(SOC_FAILURE
(l145)){l205[l144] = prefix->index;l206 = TRUE;break;}if(SOC_URPF_STATUS_GET(
l1)){l145 = soc_mem_write(l1,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,
prefix->index),soc_mem_entry_null(l1,l18));if(SOC_FAILURE(l145)){l205[l144] = 
prefix->index;l206 = TRUE;break;}}}l205[l144] = prefix->index;prefix->index = 
l37[l144];}if(l206){for(;l144>= 0;l144--){payload_t*prefix = l36->prefix[l144
];prefix->index = l205[l144];if(l205[l144]<0){continue;}l187 = soc_mem_write(
l1,l18,MEM_BLOCK_ALL,l205[l144],(uint8*)l202+l144*l204);if(SOC_FAILURE(l187))
{break;}if(!SOC_URPF_STATUS_GET(l1)){continue;}l187 = soc_mem_write(l1,l18,
MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(l1,l205[l144]),(uint8*)l203+l144*l204);
if(SOC_FAILURE(l187)){break;}}}l207:if(l205!= NULL){sal_free(l205);}if(l203!= 
NULL){sal_free(l203);}if(l202!= NULL){sal_free(l202);}return l145;}static int
l29(int l1,alpm_pfx_info_t*l30,trie_t*l31,uint32*l32,uint32 l33,trie_node_t*
l34,defip_pair_128_entry_t*lpm_entry,uint32*l35){trie_node_t*l157 = NULL;int
l28;defip_alpm_ipv6_128_entry_t l191;payload_t*l208 = NULL;int l209;void*l189
;alpm_pivot_t*l210;int l145 = SOC_E_NONE;soc_mem_t l18;alpm_bucket_handle_t*
l211;l210 = l30->pivot_pyld;l209 = l210->tcam_index;l28 = L3_DEFIP_MODE_128;
l18 = L3_DEFIP_ALPM_IPV6_128m;l189 = ((uint32*)&(l191));l157 = NULL;l145 = 
trie_find_lpm(l31,l32,l33,&l157);l208 = (payload_t*)l157;if(SOC_FAILURE(l145)
){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l32[0],l32[1],l32[2],
l32[3],l32[4],l33));return l145;}if(l208->bkt_ptr){if(l208->bkt_ptr == l30->
new_pfx_pyld){sal_memcpy(l189,l30->alpm_data,sizeof(
defip_alpm_ipv6_128_entry_t));}else{l145 = soc_mem_read(l1,l18,MEM_BLOCK_ANY,
((payload_t*)l208->bkt_ptr)->index,l189);}if(SOC_FAILURE(l145)){return l145;}
l145 = l22(l1,l189,l18,l28,l30->vrf_id,l30->bktid,((payload_t*)l208->bkt_ptr)
->index,lpm_entry);if(SOC_FAILURE(l145)){return l145;}*l35 = ((payload_t*)(
l208->bkt_ptr))->len;}else{l145 = soc_mem_read(l1,L3_DEFIP_PAIR_128m,
MEM_BLOCK_ANY,soc_th_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,
SOC_ALPM_128_DEFIP_TO_PAIR(l209>>1),1),lpm_entry);}l211 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l211 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Unable to allocate memory "
"for PIVOT trie node \n")));l145 = SOC_E_MEMORY;return l145;}sal_memset(l211,
0,sizeof(*l211));l210 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for new Pivot");if(l210 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Unable to allocate memory "
"for PIVOT trie node \n")));l145 = SOC_E_MEMORY;return l145;}sal_memset(l210,
0,sizeof(*l210));PIVOT_BUCKET_HANDLE(l210) = l211;l145 = trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l210));PIVOT_BUCKET_TRIE(l210)->trie = 
l34;PIVOT_BUCKET_INDEX(l210) = l30->bktid;PIVOT_BUCKET_VRF(l210) = l30->vrf;
PIVOT_BUCKET_IPV6(l210) = l28;PIVOT_BUCKET_DEF(l210) = FALSE;(l210)->key[0] = 
l32[0];(l210)->key[1] = l32[1];(l210)->key[2] = l32[2];(l210)->key[3] = l32[3
];(l210)->key[4] = l32[4];(l210)->len = l33;l148((l32),(l33),(l28));l24(l1,
l32,l33,l30->vrf,l28,lpm_entry,0,0);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(l1)]->l95),(
ALPM_BKT_IDX(l30->bktid)));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(l1)]->l96),(ALPM_BKT_SIDX(l30->
bktid)));l30->pivot_pyld = l210;return l145;}static int l212(int l1,
alpm_pfx_info_t*l30,int*l163,int*l12){trie_node_t*l34;uint32 l11[
SOC_MAX_MEM_FIELD_WORDS];int l28;uint32 l33,l35 = 0;uint32 l159 = 0;uint32 l32
[5];int l135;defip_pair_128_entry_t l191,l192;trie_t*l31,*trie;void*l189,*
l190;alpm_pivot_t*l213 = l30->pivot_pyld;defip_pair_128_entry_t lpm_entry;
soc_mem_t l18;trie_t*l156 = NULL;alpm_mem_prefix_array_t l214;int*l37 = NULL;
int*l137 = NULL;int l145 = SOC_E_NONE,l144,l42 = -1;int tcam_index,l215,l216 = 
0;int l217 = 0,l218 = 0;l28 = L3_DEFIP_MODE_128;l18 = L3_DEFIP_ALPM_IPV6_128m
;l189 = ((uint32*)&(l191));l190 = ((uint32*)&(l192));
soc_th_alpm_bank_disb_get(l1,l30->vrf,&l159);l31 = VRF_PREFIX_TRIE_IPV6_128(
l1,l30->vrf);trie = PIVOT_BUCKET_TRIE(l30->pivot_pyld);l216 = l30->bktid;l215
= PIVOT_TCAM_INDEX(l213);l145 = soc_th_alpm_bs_alloc(l1,&l30->bktid,l30->vrf,
l28);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate""new bucket for split\n")));l30->bktid = 
-1;_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->new_pfx_pyld,l37,l30->bktid,
l30->vrf,l28);return l145;}l144 = _soc_th_alpm_bkt_entry_cnt(l1,l28);l144-= 
SOC_ALPM_BS_BKT_USAGE_COUNT(l1,ALPM_BKT_IDX(l30->bktid));l145 = trie_split(
trie,_MAX_KEY_LEN_144_,FALSE,l32,&l33,&l34,NULL,FALSE,l144);if(SOC_FAILURE(
l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not split bucket")));_soc_th_alpm_free_pfx_trie(l1,
l31,trie,l30->new_pfx_pyld,l37,l30->bktid,l30->vrf,l28);return l145;}l145 = 
l29(l1,l30,l31,l32,l33,l34,&lpm_entry,&l35);if(l145!= SOC_E_NONE){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"could not initialize pivot")));
_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->new_pfx_pyld,l37,l30->bktid,l30->
vrf,l28);return l145;}sal_memset(&l214,0,sizeof(l214));l145 = trie_traverse(
PIVOT_BUCKET_TRIE(l30->pivot_pyld),_soc_th_alpm_mem_prefix_array_cb,&l214,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Bucket split failed")));
_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->new_pfx_pyld,l37,l30->bktid,l30->
vrf,l28);return l145;}l37 = sal_alloc(sizeof(*l37)*l214.count,"new_index");if
(l37 == NULL){l145 = SOC_E_MEMORY;_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30
->new_pfx_pyld,l37,l30->bktid,l30->vrf,l28);return l145;}l137 = sal_alloc(
sizeof(*l137)*l214.count,"old_index");if(l137 == NULL){l145 = SOC_E_MEMORY;
_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->new_pfx_pyld,l137,l30->bktid,l30
->vrf,l28);return l145;}sal_memset(l37,-1,sizeof(*l37)*l214.count);sal_memset
(l137,-1,sizeof(*l137)*l214.count);for(l144 = 0;l144<l214.count;l144++){
payload_t*l134 = l214.prefix[l144];if(l134->index>0){l145 = soc_mem_read(l1,
l18,MEM_BLOCK_ANY,l134->index,l189);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failed to ""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l214.prefix[l144]->key[1],l214.
prefix[l144]->key[2],l214.prefix[l144]->key[3],l214.prefix[l144]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l30->key_data,l18,
l213,l30->pivot_pyld,&l214,l37,l42);sal_free(l137);return l145;}if(
SOC_URPF_STATUS_GET(l1)){l145 = soc_mem_read(l1,l18,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l134->index),l190);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failed to"
"read rpf prefix ""0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l214.prefix[l144]
->key[1],l214.prefix[l144]->key[2],l214.prefix[l144]->key[3],l214.prefix[l144
]->key[4]));(void)_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l30->
key_data,l18,l213,l30->pivot_pyld,&l214,l37,l42);sal_free(l137);return l145;}
}soc_mem_field32_set(l1,l18,l189,SUB_BKT_PTRf,ALPM_BKT_SIDX(l30->bktid));
soc_mem_field32_set(l1,l18,l190,SUB_BKT_PTRf,ALPM_BKT_SIDX(l30->bktid));l145 = 
_soc_th_alpm_insert_in_bkt(l1,l18,l30->bktid,l159,l189,l11,&l135,l28);if(
SOC_SUCCESS(l145)){if(SOC_URPF_STATUS_GET(l1)){l145 = soc_mem_write(l1,l18,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l135),l190);}l217++;l218++;}}else{
soc_mem_field32_set(l1,l18,l30->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(l30->
bktid));l145 = _soc_th_alpm_insert_in_bkt(l1,l18,l30->bktid,l159,l30->
alpm_data,l11,&l135,l28);if(SOC_SUCCESS(l145)){l42 = l144;*l12 = l135;if(
SOC_URPF_STATUS_GET(l1)){soc_mem_field32_set(l1,l18,l30->alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l30->bktid));l145 = soc_mem_write(l1,l18,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l135),l30->alpm_sip_data);}l218++;}}
l37[l144] = l135;if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l214.prefix[l144]->key[1],l214.
prefix[l144]->key[2],l214.prefix[l144]->key[3],l214.prefix[l144]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l30->key_data,l18,
l213,l30->pivot_pyld,&l214,l37,l42);sal_free(l137);return l145;}l137[l144] = 
l134->index;}l145 = l2(l1,&lpm_entry,l163);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Unable to add new"
"pivot to tcam\n")));if(l145 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l30->vrf,
l28);}(void)_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l30->key_data
,l18,l213,l30->pivot_pyld,&l214,l37,l42);sal_free(l137);return l145;}*l163 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,*l163,l28);l145 = l162(l1,*
l163,l35);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l163));l199(l1,
&lpm_entry,l30->key_data,*l163,l30->pivot_pyld);(void)
_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l30->key_data,l18,l213,
l30->pivot_pyld,&l214,l37,l42);sal_free(l137);return l145;}l156 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l30->vrf);l145 = trie_insert(l156,(l30->pivot_pyld
)->key,NULL,(l30->pivot_pyld)->len,(trie_node_t*)l30->pivot_pyld);if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));(void)_soc_th_alpm_rollback_bkt_move(
l1,L3_DEFIP_MODE_128,l30->key_data,l18,l213,l30->pivot_pyld,&l214,l37,l42);
sal_free(l137);return l145;}tcam_index = SOC_ALPM_128_ADDR_LWR(*l163)<<1;
ALPM_TCAM_PIVOT(l1,tcam_index) = l30->pivot_pyld;PIVOT_TCAM_INDEX(l30->
pivot_pyld) = tcam_index;VRF_PIVOT_REF_INC(l1,l30->vrf,l28);l145 = 
_soc_th_alpm_128_move_inval(l1,l18,&l214,l37);if(SOC_FAILURE(l145)){l199(l1,&
lpm_entry,l30->key_data,*l163,l30->pivot_pyld);(void)
_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l30->key_data,l18,l213,
l30->pivot_pyld,&l214,l37,l42);sal_free(l37);l37 = NULL;sal_free(l137);return
l145;}l145 = soc_th_alpm_update_hit_bits(l1,l214.count,l137,l37);if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l214.count));l145 = 
SOC_E_NONE;}sal_free(l37);l37 = NULL;sal_free(l137);if(l42 == -1){
soc_mem_field32_set(l1,l18,l30->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(
PIVOT_BUCKET_INDEX(l213)));l145 = _soc_th_alpm_insert_in_bkt(l1,l18,
PIVOT_BUCKET_INDEX(l213),l159,l30->alpm_data,l11,&l135,l28);if(SOC_FAILURE(
l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n")));
_soc_th_alpm_free_pfx_trie(l1,l31,trie,l30->new_pfx_pyld,l37,l30->bktid,l30->
vrf,l28);return l145;}l217--;if(SOC_URPF_STATUS_GET(l1)){l145 = soc_mem_write
(l1,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l135),l30->alpm_sip_data);}*
l12 = l135;l30->new_pfx_pyld->index = l135;}soc_th_alpm_bu_upd(l1,l216,l215,
FALSE,l28,-l217);soc_th_alpm_bu_upd(l1,l30->bktid,tcam_index,l30->vrf,l28,
l218);PIVOT_BUCKET_ENT_CNT_UPDATE(l30->pivot_pyld);VRF_BUCKET_SPLIT_INC(l1,
l30->vrf,l28);return l145;}static int l219(int l1,void*key_data,soc_mem_t l18
,void*alpm_data,void*alpm_sip_data,int*l12,int bktid,int tcam_index){
alpm_pivot_t*pivot_pyld,*l213;defip_aux_scratch_entry_t l9;uint32 l11[
SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l33;int l28,vrf,vrf_id;int l135;int
l145 = SOC_E_NONE,l187;uint32 l6,l7,l159;int l155 = 0;int l163;int l220 = 0;
trie_t*trie,*l31;trie_node_t*l157 = NULL;payload_t*l221,*l222,*l208;int l20 = 
0;int*l37 = NULL;alpm_pfx_info_t l30;l28 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&vrf_id,&vrf));
soc_th_alpm_bank_disb_get(l1,vrf,&l159);soc_alpm_db_ent_type_encoding(l1,vrf,
&l6,&l7);l18 = L3_DEFIP_ALPM_IPV6_128m;l145 = _soc_th_alpm_128_prefix_create(
l1,key_data,prefix,&l33,&l20);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: prefix create failed\n")));return l145
;}sal_memset(&l9,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(
l1,key_data,l28,l6,l7,0,&l9));if(bktid == 0){l145 = l154(l1,prefix,l33,l28,
vrf,&l155,&tcam_index,&bktid);SOC_IF_ERROR_RETURN(l145);soc_mem_field32_set(
l1,l18,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));}l145 = 
_soc_th_alpm_insert_in_bkt(l1,l18,bktid,l159,alpm_data,l11,&l135,l28);if(l145
== SOC_E_NONE){*l12 = l135;if(SOC_URPF_STATUS_GET(l1)){soc_mem_field32_set(l1
,l18,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));l187 = soc_mem_write(l1
,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l135),alpm_sip_data);if(
SOC_FAILURE(l187)){return l187;}}}if(l145 == SOC_E_FULL){l155 = 
_soc_th_alpm_bkt_entry_cnt(l1,L3_DEFIP_MODE_128);if(l155>4){l155 = 4;}if(
PIVOT_BUCKET_COUNT(ALPM_TCAM_PIVOT(l1,tcam_index))<l155){int l223 = bktid;
l145 = soc_th_alpm_bs_alloc(l1,&l223,vrf,l28);SOC_IF_ERROR_RETURN(l145);l145 = 
_soc_th_alpm_128_move_trie(l1,l28,bktid,l223);SOC_IF_ERROR_RETURN(l145);bktid
= l223;soc_mem_field32_set(l1,l18,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid)
);l145 = _soc_th_alpm_insert_in_bkt(l1,l18,bktid,l159,alpm_data,l11,&l135,l28
);if(SOC_SUCCESS(l145)){*l12 = l135;if(SOC_URPF_STATUS_GET(l1)){
soc_mem_field32_set(l1,l18,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));
l187 = soc_mem_write(l1,l18,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l135),
alpm_sip_data);if(SOC_FAILURE(l187)){return l187;}}}else{return l145;}}else{
l220 = 1;}}pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l213 = pivot_pyld;l221 = sal_alloc(sizeof(
payload_t),"Payload for 128b Key");if(l221 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"trie node \n")));return SOC_E_MEMORY;}l222 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l222 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l221);return SOC_E_MEMORY;}sal_memset(l221,0,
sizeof(*l221));sal_memset(l222,0,sizeof(*l222));l221->key[0] = prefix[0];l221
->key[1] = prefix[1];l221->key[2] = prefix[2];l221->key[3] = prefix[3];l221->
key[4] = prefix[4];l221->len = l33;l221->index = l135;sal_memcpy(l222,l221,
sizeof(*l221));l222->bkt_ptr = l221;l145 = trie_insert(trie,prefix,NULL,l33,(
trie_node_t*)l221);if(SOC_FAILURE(l145)){if(l221!= NULL){sal_free(l221);}if(
l222!= NULL){sal_free(l222);}return l145;}l31 = VRF_PREFIX_TRIE_IPV6_128(l1,
vrf);if(!l20){l145 = trie_insert(l31,prefix,NULL,l33,(trie_node_t*)l222);}
else{l157 = NULL;l145 = trie_find_lpm(l31,0,0,&l157);l208 = (payload_t*)l157;
if(SOC_SUCCESS(l145)){l208->bkt_ptr = l221;}}if(SOC_FAILURE(l145)){
_soc_th_alpm_free_pfx_trie(l1,l31,trie,l222,l37,bktid,vrf,l28);return l145;}
if(l220){l30.key_data = key_data;l30.new_pfx_pyld = l221;l30.pivot_pyld = 
pivot_pyld;l30.alpm_data = alpm_data;l30.alpm_sip_data = alpm_sip_data;l30.
bktid = bktid;l30.vrf_id = vrf_id;l30.vrf = vrf;l145 = l212(l1,&l30,&l163,l12
);if(l145!= SOC_E_NONE){return l145;}bktid = l30.bktid;tcam_index = 
PIVOT_TCAM_INDEX(l30.pivot_pyld);alpm_128_split_count++;}else{
soc_th_alpm_bu_upd(l1,bktid,tcam_index,vrf,l28,1);}VRF_TRIE_ROUTES_INC(l1,vrf
,l28);if(l20){sal_free(l222);}soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&
l9,ALG_HIT_IDXf,*l12);SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
INSERT_PROPAGATE,&l9,TRUE,&l155,&tcam_index,&bktid));if(SOC_URPF_STATUS_GET(
l1)){uint32 l224[4] = {0,0,0,0};soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,
&l9,DB_TYPEf,l6+1);if(l20){soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,
RPEf,1);}else{soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,RPEf,0);}
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,ALG_HIT_IDXf,
_soc_th_alpm_rpf_entry(l1,*l12));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
INSERT_PROPAGATE,&l9,TRUE,&l155,&tcam_index,&bktid));if(!l20){
soc_mem_field_set(l1,L3_DEFIP_AUX_SCRATCHm,(uint32*)&l9,IP_ADDRf,(uint32*)
l224);soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,IP_LENGTHf,0);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,REPLACE_LENf,0);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,RPEf,1);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l9,TRUE,&l155,&tcam_index,&bktid));}
}PIVOT_BUCKET_ENT_CNT_UPDATE(l213);return l145;}static int l24(int unit,
uint32*key,int len,int vrf,int l5,defip_pair_128_entry_t*lpm_entry,int l25,
int l26){uint32 l151;if(l26){sal_memset(lpm_entry,0,sizeof(
defip_pair_128_entry_t));}soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l87),(vrf&SOC_VRF_MAX(
unit)));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l88),(vrf&SOC_VRF_MAX(unit)))
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l85),(vrf&SOC_VRF_MAX(unit)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l86),(vrf&SOC_VRF_MAX(unit)));if(vrf == (
SOC_VRF_MAX(unit)+1)){l151 = 0;}else{l151 = SOC_VRF_MAX(unit);}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l91),(l151));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l92),(l151)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l110[(unit)]->l89),(l151));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l90),(l151)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l110[(unit)]->l64),(key[0]));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l65),(key[
1]));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m
)),(lpm_entry),(l110[(unit)]->l62),(key[2]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l63),(key[3]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l72),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l73),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l70),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l71),(3));l152(unit,(void*)lpm_entry,len);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l83),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l84),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l81),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l82),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l76),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l77),((1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l74),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l75),((1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l100),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l101),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l102),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l103),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_UPRf))-1));return(SOC_E_NONE);}static int
l225(int l1,void*key_data,int vrf_id,int vrf,int bktid,int tcam_index,int l135
){alpm_pivot_t*pivot_pyld;defip_alpm_ipv6_128_entry_t l191,l226,l192;
defip_aux_scratch_entry_t l9;uint32 l11[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l18
;void*l189,*l227,*l190 = NULL;int l145 = SOC_E_NONE,l187 = SOC_E_NONE;uint32
l228[5],prefix[5];int l28;uint32 l33;int l229;uint32 l6,l7,l159;int l155,l20 = 
0;trie_t*trie,*l31;uint32 l230;defip_pair_128_entry_t lpm_entry,*l231;
payload_t*l221 = NULL,*l232 = NULL,*l208 = NULL;trie_node_t*l201 = NULL,*l157
= NULL;trie_t*l156 = NULL;l28 = L3_DEFIP_MODE_128;if(!(((vrf_id == 
SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)
&&(vrf_id == SOC_L3_VRF_GLOBAL))))){soc_th_alpm_bank_disb_get(l1,vrf,&l159);
soc_alpm_db_ent_type_encoding(l1,vrf,&l6,&l7);l145 = 
_soc_th_alpm_128_prefix_create(l1,key_data,prefix,&l33,&l20);if(SOC_FAILURE(
l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: prefix create failed\n")));return l145;}if(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_COMBINED){if(vrf_id!= 
SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1,vrf,l28)>1){if(l20){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"VRF %d: Cannot v6-128 delete "
"default route if other routes are present ""in this mode"),vrf));return
SOC_E_PARAM;}}}}l18 = L3_DEFIP_ALPM_IPV6_128m;l189 = ((uint32*)&(l191));
SOC_ALPM_LPM_LOCK(l1);if(bktid == 0){l145 = _soc_th_alpm_128_find(l1,l18,
key_data,vrf_id,vrf,l189,&tcam_index,&bktid,&l135,TRUE);}else{l145 = l15(l1,
key_data,l189,0,l18,0,0,bktid);}sal_memcpy(&l226,l189,sizeof(l226));l227 = &
l226;if(SOC_FAILURE(l145)){SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l1,"_soc_alpm_128_delete: Unable to find ""prefix for delete\n"))
);return l145;}l229 = bktid;pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l145 = trie_delete(trie,prefix,l33,&l201);l221 = 
(payload_t*)l201;if(l145!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l145;}l31 = VRF_PREFIX_TRIE_IPV6_128(l1,vrf);
l156 = VRF_PIVOT_TRIE_IPV6_128(l1,vrf);if(!l20){l145 = trie_delete(l31,prefix
,l33,&l201);l232 = (payload_t*)l201;if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l233;}l157 = NULL;l145 = trie_find_lpm(l31,prefix,l33,&l157)
;l208 = (payload_t*)l157;if(SOC_SUCCESS(l145)){payload_t*l234 = (payload_t*)(
l208->bkt_ptr);if(l234!= NULL){l230 = l234->len;}else{l230 = 0;}}else{
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l235;}sal_memcpy(l228,prefix,sizeof(
prefix));l148((l228),(l33),(l28));l145 = l24(l1,l228,l230,vrf,l28,&lpm_entry,
0,1);l187 = _soc_th_alpm_128_find(l1,l18,&lpm_entry,vrf_id,vrf,l189,&
tcam_index,&bktid,&l135,TRUE);if(l187<0){if(vrf_id!= SOC_L3_VRF_GLOBAL&&
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_COMBINED){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"Find128 associated data for next bpm failed."
"prefix: 0x%08x 0x%08x 0x%08x 0x%08x, length %d\n"),l228[0],l228[1],l228[2],
l228[3],l230));}}(void)l22(l1,l189,l18,l28,vrf_id,bktid,0,&lpm_entry);(void)
l24(l1,l228,l33,vrf,l28,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l1)){if(
SOC_SUCCESS(l145)){l190 = ((uint32*)&(l192));l145 = soc_mem_read(l1,l18,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,ALPM_ENT_INDEX(l135)),l190);}}if((
l230 == 0)&&SOC_FAILURE(l187)){l231 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf)
;sal_memcpy(&lpm_entry,l231,sizeof(lpm_entry));l145 = l24(l1,prefix,l230,vrf,
l28,&lpm_entry,0,1);}if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l235;}l231 = &lpm_entry;}else{l157 = 
NULL;l145 = trie_find_lpm(l31,prefix,l33,&l157);l208 = (payload_t*)l157;if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),vrf));goto l233;}l208->bkt_ptr = 0;
l230 = 0;l231 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf);}l145 = l4(l1,l231,
l28,l6,l7,l230,&l9);if(SOC_FAILURE(l145)){goto l235;}soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l9,ALG_HIT_IDXf,ALPM_ENT_INDEX(l135));l145 = 
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l9,TRUE,&l155,&tcam_index,&bktid);if
(SOC_FAILURE(l145)){goto l235;}if(SOC_URPF_STATUS_GET(l1)){uint32 l151;if(
l190!= NULL){soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,DB_TYPEf,l6+1);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l9,ALG_HIT_IDXf,
_soc_th_alpm_rpf_entry(l1,ALPM_ENT_INDEX(l135)));l151 = soc_mem_field32_get(
l1,l18,l190,SRC_DISCARDf);soc_mem_field32_set(l1,l18,&l9,SRC_DISCARDf,l151);
l151 = soc_mem_field32_get(l1,l18,l190,DEFAULTROUTEf);soc_mem_field32_set(l1,
l18,&l9,DEFAULTROUTEf,l151);l145 = _soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&
l9,TRUE,&l155,&tcam_index,&bktid);}if(SOC_FAILURE(l145)){goto l235;}}sal_free
(l221);if(!l20){sal_free(l232);}PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);if((
pivot_pyld->len!= 0)&&(trie->trie == NULL)){uint32 l236[5];sal_memcpy(l236,
pivot_pyld->key,sizeof(l236));l148((l236),(pivot_pyld->len),(l28));l24(l1,
l236,pivot_pyld->len,vrf,l28,&lpm_entry,0,1);l145 = 
soc_th_alpm_128_lpm_delete(l1,&lpm_entry);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),pivot_pyld->key[1],pivot_pyld
->key[2],pivot_pyld->key[3],pivot_pyld->key[4]));}}l145 = 
_soc_th_alpm_delete_in_bkt(l1,l18,l229,l159,l227,l11,&l135,l28);if(
SOC_FAILURE(l145)){SOC_ALPM_LPM_UNLOCK(l1);return l145;}if(
SOC_URPF_STATUS_GET(l1)){l145 = soc_mem_alpm_delete(l1,l18,
SOC_TH_ALPM_RPF_BKT_IDX(l1,ALPM_BKT_IDX(l229)),MEM_BLOCK_ALL,l159,l227,l11,&
l155);if(SOC_FAILURE(l145)){SOC_ALPM_LPM_UNLOCK(l1);return l145;}}
soc_th_alpm_bu_upd(l1,PIVOT_BUCKET_INDEX(pivot_pyld),PIVOT_TCAM_INDEX(
pivot_pyld),((pivot_pyld->len!= 0)&&(trie->trie == NULL))?TRUE:FALSE,l28,-1);
if((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l145 = soc_th_alpm_bs_free(l1
,PIVOT_BUCKET_INDEX(pivot_pyld),vrf,l28);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to release"
"empty bucket: %d\n"),PIVOT_BUCKET_INDEX(pivot_pyld)));}l145 = trie_delete(
l156,pivot_pyld->key,pivot_pyld->len,&l201);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"could not delete pivot from pivot trie\n")));
}trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(
pivot_pyld));sal_free(pivot_pyld);_soc_tomahawk_alpm_bkt_view_set(l1,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,ALPM_BKT_IDX(l229)),INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l1,l28)){int l237 = ALPM_BKT_IDX(l229)+1;
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,l237),
INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,vrf,l28);if(VRF_TRIE_ROUTES_CNT(l1,vrf,
l28) == 0){l145 = l27(l1,vrf,l28);}SOC_ALPM_LPM_UNLOCK(l1);return l145;l235:
l187 = trie_insert(l31,prefix,NULL,l33,(trie_node_t*)l232);if(SOC_FAILURE(
l187)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l233:l187 = trie_insert(trie,prefix,NULL,l33,(
trie_node_t*)l221);if(SOC_FAILURE(l187)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}SOC_ALPM_LPM_UNLOCK(l1);return l145;}int
soc_th_alpm_128_init(int l1){int l145 = SOC_E_NONE;l145 = 
soc_th_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l145);return l145;}int
soc_th_alpm_128_state_clear(int l1){int l144,l145;for(l144 = 0;l144<= 
SOC_VRF_MAX(l1)+1;l144++){l145 = trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,
l144),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l145)){
trie_destroy(VRF_PREFIX_TRIE_IPV6_128(l1,l144));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n"),l1,l144));return
l145;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l144)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l144));}}return SOC_E_NONE;}int
soc_th_alpm_128_deinit(int l1){soc_th_alpm_128_lpm_deinit(l1);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_state_clear(l1));return SOC_E_NONE;}
static int l238(int l1,int vrf,int l28){defip_pair_128_entry_t*lpm_entry = 
NULL,l239;int l240 = 0;int index;int l145 = SOC_E_NONE;uint32 key[5] = {0,0,0
,0,0};uint32 l33;alpm_bucket_handle_t*l211 = NULL;alpm_pivot_t*pivot_pyld = 
NULL;payload_t*l232 = NULL;trie_t*l241;trie_t*l242 = NULL;trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(l1,vrf));l242 = 
VRF_PIVOT_TRIE_IPV6_128(l1,vrf);trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6_128(l1,vrf));l241 = VRF_PREFIX_TRIE_IPV6_128(l1,vrf);
lpm_entry = sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(
lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: unable to allocate memory "
"for IPv6-128 LPM entry\n")));return SOC_E_MEMORY;}l24(l1,key,0,vrf,l28,
lpm_entry,0,1);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf) = lpm_entry;if(vrf == 
SOC_VRF_MAX(l1)+1){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(l1)]->l60),(1));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(l1)]->l97),(1));}l145 = soc_th_alpm_bs_alloc(l1,&l240,vrf,
l28);if(SOC_FAILURE(l145)){goto l243;}soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(l1)]->l95),(
ALPM_BKT_IDX(l240)));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(l1)]->l96),(ALPM_BKT_SIDX(l240)));
sal_memcpy(&l239,lpm_entry,sizeof(l239));l145 = l2(l1,&l239,&index);if(
SOC_FAILURE(l145)){goto l243;}l211 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l211 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_th_alpm_128_vrf_add: Unable to allocate memory "
"for bucket handle \n")));l145 = SOC_E_MEMORY;goto l243;}sal_memset(l211,0,
sizeof(*l211));pivot_pyld = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(pivot_pyld == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_th_alpm_128_vrf_add: Unable to allocate memory "
"for PIVOT trie node \n")));l145 = SOC_E_MEMORY;goto l243;}l232 = sal_alloc(
sizeof(payload_t),"Payload for pfx trie key");if(l232 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: Unable to allocate memory ""for pfx trie node \n"))
);l145 = SOC_E_MEMORY;goto l243;}sal_memset(pivot_pyld,0,sizeof(*pivot_pyld))
;sal_memset(l232,0,sizeof(*l232));l33 = 0;PIVOT_BUCKET_HANDLE(pivot_pyld) = 
l211;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(pivot_pyld));
PIVOT_BUCKET_INDEX(pivot_pyld) = l240;PIVOT_BUCKET_VRF(pivot_pyld) = vrf;
PIVOT_BUCKET_IPV6(pivot_pyld) = l28;PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;
pivot_pyld->key[0] = l232->key[0] = key[0];pivot_pyld->key[1] = l232->key[1] = 
key[1];pivot_pyld->key[2] = l232->key[2] = key[2];pivot_pyld->key[3] = l232->
key[3] = key[3];pivot_pyld->key[4] = l232->key[4] = key[4];pivot_pyld->len = 
l232->len = l33;l145 = trie_insert(l241,key,NULL,l33,&(l232->node));if(
SOC_FAILURE(l145)){goto l243;}l145 = trie_insert(l242,key,NULL,l33,(
trie_node_t*)pivot_pyld);if(SOC_FAILURE(l145)){trie_node_t*l201 = NULL;(void)
trie_delete(l241,key,l33,&l201);goto l243;}index = soc_th_alpm_physical_idx(
l1,L3_DEFIP_PAIR_128m,index,l28);ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
index)<<1) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = SOC_ALPM_128_ADDR_LWR(
index)<<1;VRF_PIVOT_REF_INC(l1,vrf,l28);VRF_TRIE_INIT_DONE(l1,vrf,l28,1);
return l145;l243:if(l232!= NULL){sal_free(l232);}if(pivot_pyld!= NULL){
sal_free(pivot_pyld);}if(l211!= NULL){sal_free(l211);}if(lpm_entry!= NULL){
sal_free(lpm_entry);}VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf) = NULL;return
l145;}static int l27(int l1,int vrf,int l28){defip_pair_128_entry_t*lpm_entry
;int l244,vrf_id,l245;int l145 = SOC_E_NONE;uint32 key[2] = {0,0},l149[
SOC_MAX_MEM_FIELD_WORDS];payload_t*l221;alpm_pivot_t*l246;trie_node_t*l201;
trie_t*l241;trie_t*l242 = NULL;soc_mem_t l18;int tcam_index,bktid,index;
uint32 l161[SOC_MAX_MEM_FIELD_WORDS];lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf);l18 = L3_DEFIP_ALPM_IPV6_128m;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,lpm_entry,&vrf_id,&l245));
l145 = _soc_th_alpm_128_find(l1,l18,lpm_entry,vrf_id,l245,l161,&tcam_index,&
bktid,&index,TRUE);l145 = soc_th_alpm_bs_free(l1,bktid,vrf,l28);
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,
ALPM_BKT_IDX(bktid)),INVALIDm);if(SOC_TH_ALPM_SCALE_CHECK(l1,l28)){
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,
ALPM_BKT_IDX(bktid)+1),INVALIDm);}l145 = l14(l1,lpm_entry,(void*)l149,&l244);
if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n")
,vrf,l28));l244 = -1;}l244 = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,
l244,l28);l246 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l244)<<1);l145 = 
soc_th_alpm_128_lpm_delete(l1,lpm_entry);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),vrf,l28));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf) = NULL;l241 = 
VRF_PREFIX_TRIE_IPV6_128(l1,vrf);VRF_PREFIX_TRIE_IPV6_128(l1,vrf) = NULL;
VRF_TRIE_INIT_DONE(l1,vrf,l28,0);l145 = trie_delete(l241,key,0,&l201);l221 = 
(payload_t*)l201;if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),vrf,l28));}
sal_free(l221);(void)trie_destroy(l241);l242 = VRF_PIVOT_TRIE_IPV6_128(l1,vrf
);VRF_PIVOT_TRIE_IPV6_128(l1,vrf) = NULL;l201 = NULL;l145 = trie_delete(l242,
key,0,&l201);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),vrf,l28));}(void)
trie_destroy(l242);(void)trie_destroy(PIVOT_BUCKET_TRIE(l246));sal_free(
PIVOT_BUCKET_HANDLE(l246));sal_free(l246);return l145;}int
soc_th_alpm_128_insert(int l1,void*l3,uint32 l19,int l247,int l248){
defip_alpm_ipv6_128_entry_t l191,l192;soc_mem_t l18;void*l189,*l227;int vrf_id
,vrf;int index;int l5;int l145 = SOC_E_NONE;uint32 l20;int l21 = 0;l5 = 
L3_DEFIP_MODE_128;l18 = L3_DEFIP_ALPM_IPV6_128m;l189 = ((uint32*)&(l191));
l227 = ((uint32*)&(l192));if(l247!= -1){l21 = ALPM_BKTID(ALPM_BKT_IDX(
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l247)),(l247>>ALPM_ENT_INDEX_BITS));}
SOC_IF_ERROR_RETURN(l15(l1,l3,l189,l227,l18,l19,&l20,l21));
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l3,&vrf_id,&vrf));if(((
vrf_id == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == SOC_L3_VRF_GLOBAL)))){l145 = l2(l1,l3,&
index);if(SOC_SUCCESS(l145)){if(vrf_id == SOC_L3_VRF_OVERRIDE){
VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l5);VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l5);}
else{VRF_TRIE_ROUTES_INC(l1,vrf,l5);VRF_PIVOT_REF_INC(l1,vrf,l5);}}else if(
l145 == SOC_E_FULL){if(vrf_id == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_FULL_INC(l1,
MAX_VRF_ID,l5);}else{VRF_PIVOT_FULL_INC(l1,vrf,l5);}}return(l145);}else if(
vrf == 0){if(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL||
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"VRF=0 cannot be added in current mode\n")));
return SOC_E_PARAM;}}if(vrf_id!= SOC_L3_VRF_GLOBAL){if(soc_th_alpm_mode_get(
l1) == SOC_ALPM_MODE_COMBINED){if(VRF_TRIE_ROUTES_CNT(l1,vrf,l5) == 0){if(!
l20){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
vrf_id));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,vrf,l5)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d is not ""initialized\n"),vrf));l145 = l238(l1,
vrf,l5);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),vrf,l5));return
l145;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init ""completed\n"),vrf,l5));}if(l248
&SOC_ALPM_LOOKUP_HIT){l145 = l160(l1,l3,l189,l227,l18,l247);}else{if(l247 == 
-1){l247 = 0;}l247 = ALPM_BKTID(ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,
l247)),l247>>ALPM_ENT_INDEX_BITS);l145 = l219(l1,l3,l18,l189,l227,&index,l247
,l248);}if(l145!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_th_alpm_128_insert: ""Route Insertion Failed :%s\n"),l1,
soc_errmsg(l145)));}return(l145);}int soc_th_alpm_128_lookup(int l1,void*
key_data,void*l11,int*l12,int*l249){defip_alpm_ipv6_128_entry_t l191;
soc_mem_t l18;int bktid = 0;int tcam_index = -1;void*l189;int vrf_id,vrf;int
l5 = 2,l134;int l145 = SOC_E_NONE;*l12 = 0;l145 = l10(l1,key_data,l11,l12,&
l134,&l5,&vrf_id,&vrf);if(((vrf_id == SOC_L3_VRF_OVERRIDE)||((
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == 
SOC_L3_VRF_GLOBAL)))){*l249 = 0;return l145;}if(!VRF_TRIE_INIT_COMPLETED(l1,
vrf,l5)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_lookup:VRF %d is not ""initialized\n"),vrf));*l12 = 0;*l249 = 
0;return SOC_E_NOT_FOUND;}l18 = L3_DEFIP_ALPM_IPV6_128m;l189 = ((uint32*)&(
l191));SOC_ALPM_LPM_LOCK(l1);l145 = _soc_th_alpm_128_find(l1,l18,key_data,
vrf_id,vrf,l189,&tcam_index,&bktid,l12,TRUE);SOC_ALPM_LPM_UNLOCK(l1);if(
SOC_FAILURE(l145)){*l249 = tcam_index;*l12 = (ALPM_BKT_SIDX(bktid)<<
ALPM_ENT_INDEX_BITS)|SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,bktid);return l145;}l145
= l22(l1,l189,l18,l5,vrf_id,bktid,*l12,l11);*l249 = SOC_ALPM_LOOKUP_HIT|
tcam_index;*l12 = (ALPM_BKT_SIDX(bktid)<<ALPM_ENT_INDEX_BITS)|*l12;return(
l145);}int l250(int l1,void*key_data,void*l11,int vrf,int*tcam_index,int*
bucket_index,int*l135,int l251){int l145 = SOC_E_NONE;int l144,l252,l28,l155 = 
0;uint32 l6,l7,l159;defip_aux_scratch_entry_t l9;int l253,l254;int index;
soc_mem_t l18,l181;int l255,l256,l257;soc_field_t l258[4] = {IP_ADDR0_LWRf,
IP_ADDR1_LWRf,IP_ADDR0_UPRf,IP_ADDR1_UPRf};uint32 l259[
SOC_MAX_MEM_FIELD_WORDS] = {0};int l260 = -1;int l261 = 0;l28 = 
L3_DEFIP_MODE_128;l181 = L3_DEFIP_PAIR_128m;l253 = soc_mem_field32_get(l1,
l181,key_data,GLOBAL_ROUTEf);l254 = soc_mem_field32_get(l1,l181,key_data,
VRF_ID_0_LWRf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d\n"),vrf == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l28,l253,l254));soc_th_alpm_bank_disb_get(l1,vrf == 
SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l1)+1):vrf,&l159);
soc_alpm_db_ent_type_encoding(l1,vrf == SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l1)+1)
:l254,&l6,&l7);if(l251){l6+= 1;}if(vrf == SOC_L3_VRF_GLOBAL){
soc_mem_field32_set(l1,l181,key_data,GLOBAL_ROUTEf,1);soc_mem_field32_set(l1,
l181,key_data,VRF_ID_0_LWRf,0);}sal_memset(&l9,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l28,l6,l7,0,&
l9));if(vrf == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l1,l181,key_data,
GLOBAL_ROUTEf,l253);soc_mem_field32_set(l1,l181,key_data,VRF_ID_0_LWRf,l254);
}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,PREFIX_LOOKUP,&l9,TRUE,&l155,
tcam_index,bucket_index));if(l155 == 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Could not find bucket\n")));return SOC_E_NOT_FOUND;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit in memory %s, index %d, "
"bucket [%d,%d]\n"),SOC_MEM_NAME(l1,l181),soc_th_alpm_logical_idx(l1,l181,
SOC_ALPM_128_DEFIP_TO_PAIR((*tcam_index)>>1),1),ALPM_BKT_IDX(*bucket_index),
ALPM_BKT_SIDX(*bucket_index)));l18 = L3_DEFIP_ALPM_IPV6_128m;l145 = l143(l1,
key_data,&l256);if(SOC_FAILURE(l145)){return l145;}l257 = 
SOC_TH_ALPM_SCALE_CHECK(l1,l28)?16:8;LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Start searching mem %s bucket [%d,%d](count %d) ""for Length %d\n"),
SOC_MEM_NAME(l1,l18),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index)
,l257,l256));for(l144 = 0;l144<l257;l144++){uint32 l189[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l262[4] = {0};uint32 l263[4] = {0};
uint32 l264[4] = {0};int l265;l145 = _soc_th_alpm_mem_index(l1,l18,
ALPM_BKT_IDX(*bucket_index),l144,l159,&index);if(l145 == SOC_E_FULL){continue
;}SOC_IF_ERROR_RETURN(soc_mem_read(l1,l18,MEM_BLOCK_ANY,index,(void*)&l189));
l265 = soc_mem_field32_get(l1,l18,&l189,VALIDf);l255 = soc_mem_field32_get(l1
,l18,&l189,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket [%d,%d] index %6d: valid %d, length %d\n"),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),index,l265,l255));if(!l265||(l255>l256)){
continue;}SHR_BITSET_RANGE(l262,128-l255,l255);(void)soc_mem_field_get(l1,l18
,(uint32*)&l189,KEYf,(uint32*)l263);l264[3] = soc_mem_field32_get(l1,l181,
key_data,l258[3]);l264[2] = soc_mem_field32_get(l1,l181,key_data,l258[2]);
l264[1] = soc_mem_field32_get(l1,l181,key_data,l258[1]);l264[0] = 
soc_mem_field32_get(l1,l181,key_data,l258[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tmask %08x %08x %08x %08x \n""\t key %08x %08x %08x %08x \n"
"\thost %08x %08x %08x %08x \n"),l262[3],l262[2],l262[1],l262[0],l263[3],l263
[2],l263[1],l263[0],l264[3],l264[2],l264[1],l264[0]));for(l252 = 3;l252>= 0;
l252--){if((l264[l252]&l262[l252])!= (l263[l252]&l262[l252])){break;}}if(l252
>= 0){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket [%d,%d], ""index %d\n"),SOC_MEM_NAME(l1,l18),
ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index),index));if(l260 == -
1||l260<l255){l260 = l255;l261 = index;sal_memcpy(l259,l189,sizeof(l189));}}
if(l260!= -1){l145 = l22(l1,&l259,l18,l28,vrf,*bucket_index,l261,l11);if(
SOC_SUCCESS(l145)){*l135 = l261;if(bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l1)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit mem %s bucket [%d,%d], index %d\n"),SOC_MEM_NAME(l1,l18),ALPM_BKT_IDX(*
bucket_index),ALPM_BKT_SIDX(*bucket_index),l261));}}return l145;}*l135 = 
soc_th_alpm_logical_idx(l1,l181,SOC_ALPM_128_DEFIP_TO_PAIR((*tcam_index)>>1),
1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l181,MEM_BLOCK_ANY,*l135,(void*)l11));
return SOC_E_NONE;}int soc_th_alpm_128_find_best_match(int l1,void*key_data,
void*l11,int*l12,int l251){int l145 = SOC_E_NONE;int l144,l252;int l266,l267;
defip_pair_128_entry_t l268;uint32 l269,l263,l264;int l255,l256;int l270,l271
;int vrf_id,vrf = 0;int tcam_index,bucket_index;soc_mem_t l181;soc_field_t
l272[4] = {IP_ADDR_MASK1_UPRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_LWRf};soc_field_t l273[4] = {IP_ADDR1_UPRf,IP_ADDR0_UPRf,
IP_ADDR1_LWRf,IP_ADDR0_LWRf};l181 = L3_DEFIP_PAIR_128m;if(!
SOC_URPF_STATUS_GET(l1)&&l251){return SOC_E_PARAM;}l266 = soc_mem_index_min(
l1,l181);l267 = soc_mem_index_count(l1,l181);if(SOC_URPF_STATUS_GET(l1)){l267
>>= 1;}if(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL||
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){l267>>= 1;l266+= l267;}
if(l251){l266+= soc_mem_index_count(l1,l181)/2;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Start LPM searchng from %d, count %d\n"),l266,l267));for(l144 = 
l266;l144<l266+l267;l144++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l181,
MEM_BLOCK_ANY,l144,(void*)&l268));if(!soc_mem_field32_get(l1,l181,&l268,
VALID0_LWRf)){continue;}l270 = soc_mem_field32_get(l1,l181,&l268,GLOBAL_HIGHf
);l271 = soc_mem_field32_get(l1,l181,&l268,GLOBAL_ROUTEf);if(!(
soc_th_alpm_mode_get(l1)!= SOC_ALPM_MODE_TCAM_ALPM&&l270&&l271)&&!(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM&&l271)){continue;}l145 = 
l143(l1,key_data,&l256);l145 = l143(l1,&l268,&l255);if(SOC_FAILURE(l145)||(
l255>l256)){continue;}for(l252 = 0;l252<4;l252++){l269 = soc_mem_field32_get(
l1,l181,&l268,l272[l252]);l263 = soc_mem_field32_get(l1,l181,&l268,l273[l252]
);l264 = soc_mem_field32_get(l1,l181,key_data,l273[l252]);if((l264&l269)!= (
l263&l269)){break;}}if(l252<4){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Hit Global High route in index = %d\n"),l252));sal_memcpy(l11,
&l268,sizeof(l268));*l12 = l144;return SOC_E_NONE;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for Global Low and VRF routes\n")));SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(l1,key_data,&vrf_id,&vrf));l145 = l250(l1,
key_data,l11,vrf,&tcam_index,&bucket_index,l12,l251);if(l145 == 
SOC_E_NOT_FOUND){vrf = SOC_L3_VRF_GLOBAL;LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Not found in VRF region, try Global ""region\n")));l145 = l250
(l1,key_data,l11,vrf,&tcam_index,&bucket_index,l12,l251);}if(SOC_SUCCESS(l145
)){tcam_index = soc_th_alpm_logical_idx(l1,l181,SOC_ALPM_128_DEFIP_TO_PAIR(
tcam_index>>1),1);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in %s region in TCAM index %d, ""bucket [%d,%d]\n"),vrf == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",tcam_index,ALPM_BKT_IDX(bucket_index),
ALPM_BKT_SIDX(bucket_index)));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Search miss for given address\n")));}return(l145);}int
soc_th_alpm_128_delete(int l1,void*key_data,int l247,int l248){int vrf_id,vrf
;int l5;int l145 = SOC_E_NONE;int l274;l5 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&vrf_id,&vrf));if
(((vrf_id == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == SOC_L3_VRF_GLOBAL)))){l145 = 
soc_th_alpm_128_lpm_delete(l1,key_data);if(SOC_SUCCESS(l145)){if(vrf_id == 
SOC_L3_VRF_OVERRIDE){VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l5);VRF_TRIE_ROUTES_DEC(
l1,MAX_VRF_ID,l5);}else{VRF_PIVOT_REF_DEC(l1,vrf,l5);VRF_TRIE_ROUTES_DEC(l1,
vrf,l5);}}return(l145);}else{int tcam_index;if(!VRF_TRIE_INIT_COMPLETED(l1,
vrf,l5)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_delete:VRF %d/%d is not ""initialized\n"),vrf,l5));return
SOC_E_NONE;}if(l247 == -1){l247 = 0;}l274 = l247;l247 = ALPM_BKTID(
ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l274)),l274>>ALPM_ENT_INDEX_BITS
);tcam_index = l248&~(SOC_ALPM_LOOKUP_HIT|SOC_ALPM_DELETE_ALL);l145 = l225(l1
,key_data,vrf_id,vrf,l247,tcam_index,l247);}return(l145);}static void l122(
int l1,void*l11,int index,l117 l123){l123[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l64));l123[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11),(l110[(l1)]->l62));l123[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l68));l123[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11),(l110[(l1)]->l66));l123[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l65));l123[5] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11),(l110[(l1)]->l63));l123[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l69));l123[7] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11),(l110[(l1)]->l67));if((!(
SOC_IS_HURRICANE(l1)))&&(((l110[(l1)]->l87)!= NULL))){int l275;(void)
soc_th_alpm_128_lpm_vrf_get(l1,l11,(int*)&l123[8],&l275);}else{l123[8] = 0;};
}static int l276(l117 l119,l117 l120){int l244;for(l244 = 0;l244<9;l244++){{
if((l119[l244])<(l120[l244])){return-1;}if((l119[l244])>(l120[l244])){return 1
;}};}return(0);}static void l277(int l1,void*l3,uint32 l278,uint32 l137,int
l134){l117 l279;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l110[(l1)]->l84))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l110[(l1)]->l83))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l110[(l1)]->l82))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l110[(l1)]->l81))){l279[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l110[(l1)]->l64));l279[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l110[(l1)]->l62));l279[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l110[(l1)]->l68));l279[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l110[(l1)]->l66));l279[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l110[(l1)]->l65));l279[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l110[(l1)]->l63));l279[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l110[(l1)]->l69));l279[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l110[(l1)]->l67));if((!(SOC_IS_HURRICANE(l1)))&&(((l110[(l1)]->l87)!= NULL)
)){int l275;(void)soc_th_alpm_128_lpm_vrf_get(l1,l3,(int*)&l279[8],&l275);}
else{l279[8] = 0;};l136((l121[(l1)]),l276,l279,l134,l137,l278);}}static void
l280(int l1,void*key_data,uint32 l278){l117 l279;int l134 = -1;int l145;
uint16 index;l279[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l64));l279[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l62));l279[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l68));l279[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l66));l279[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l65));l279[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l63));l279[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l69));l279[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l67));if((!(SOC_IS_HURRICANE(l1)))&&(((l110[(l1)]->l87
)!= NULL))){int l275;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l279[8],&l275);}else{l279[8] = 0;};index = l278;l145 = l138((l121[(l1)]),l276
,l279,l134,index);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"\ndel  index: H %d error %d\n"),index,l145));}}static int l281(int l1,
void*key_data,int l134,int*l135){l117 l279;int l145;uint16 index = (0xFFFF);
l279[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l64));l279[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l62));l279[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l68));l279[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l66));l279[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l65));l279[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l63));l279[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l69));l279[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l67));if((!(SOC_IS_HURRICANE(l1)))&&(((l110[(l1)]->l87
)!= NULL))){int l275;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l279[8],&l275);}else{l279[8] = 0;};l145 = l131((l121[(l1)]),l276,l279,l134,&
index);if(SOC_FAILURE(l145)){*l135 = 0xFFFFFFFF;return(l145);}*l135 = index;
return(SOC_E_NONE);}static int l282(int l1,void*key_data,uint32 l278){l117
l279;int l134 = -1;int l145;uint16 index;l279[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l64));l279[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l62));l279[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l68));l279[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l66));l279[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l65));l279[5] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l63));l279[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(l1)]->l69));l279[7] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l110[(l1)]->l67));if((!(
SOC_IS_HURRICANE(l1)))&&(((l110[(l1)]->l87)!= NULL))){int l275;(void)
soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&l279[8],&l275);}else{l279[8] = 
0;};index = l278;l145 = l140((l121[(l1)]),l276,l279,l134,index);if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\nveri index: H %d error %d\n"),index,l145));}return l145;}static uint16 l124
(uint8*l125,int l126){return(_shr_crc16b(0,l125,l126));}static int l127(int
unit,int l112,int l113,l116**l128){l116*l132;int index;if(l113>l112){return
SOC_E_MEMORY;}l132 = sal_alloc(sizeof(l116),"lpm_hash");if(l132 == NULL){
return SOC_E_MEMORY;}sal_memset(l132,0,sizeof(*l132));l132->unit = unit;l132
->l112 = l112;l132->l113 = l113;l132->l114 = sal_alloc(l132->l113*sizeof(*(
l132->l114)),"hash_table");if(l132->l114 == NULL){sal_free(l132);return
SOC_E_MEMORY;}l132->l115 = sal_alloc(l132->l112*sizeof(*(l132->l115)),
"link_table");if(l132->l115 == NULL){sal_free(l132->l114);sal_free(l132);
return SOC_E_MEMORY;}for(index = 0;index<l132->l113;index++){l132->l114[index
] = (0xFFFF);}for(index = 0;index<l132->l112;index++){l132->l115[index] = (
0xFFFF);}*l128 = l132;return SOC_E_NONE;}static int l129(l116*l130){if(l130!= 
NULL){sal_free(l130->l114);sal_free(l130->l115);sal_free(l130);}return
SOC_E_NONE;}static int l131(l116*l132,l118 l133,l117 entry,int l134,uint16*
l135){int l1 = l132->unit;uint16 l283;uint16 index;l283 = l124((uint8*)entry,
(32*9))%l132->l113;index = l132->l114[l283];;;while(index!= (0xFFFF)){uint32
l11[SOC_MAX_MEM_FIELD_WORDS];l117 l123;int l284;l284 = index;
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l284,l11));l122(
l1,l11,index,l123);if((*l133)(entry,l123) == 0){*l135 = index;;return(
SOC_E_NONE);}index = l132->l115[index&(0x3FFF)];;};return(SOC_E_NOT_FOUND);}
static int l136(l116*l132,l118 l133,l117 entry,int l134,uint16 l137,uint16 l37
){int l1 = l132->unit;uint16 l283;uint16 index;uint16 l285;l283 = l124((uint8
*)entry,(32*9))%l132->l113;index = l132->l114[l283];;;;l285 = (0xFFFF);if(
l137!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l11[SOC_MAX_MEM_FIELD_WORDS];
l117 l123;int l284;l284 = index;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(
l1,MEM_BLOCK_ANY,l284,l11));l122(l1,l11,index,l123);if((*l133)(entry,l123) == 
0){if(l37!= index){;if(l285 == (0xFFFF)){l132->l114[l283] = l37;l132->l115[
l37&(0x3FFF)] = l132->l115[index&(0x3FFF)];l132->l115[index&(0x3FFF)] = (
0xFFFF);}else{l132->l115[l285&(0x3FFF)] = l37;l132->l115[l37&(0x3FFF)] = l132
->l115[index&(0x3FFF)];l132->l115[index&(0x3FFF)] = (0xFFFF);}};return(
SOC_E_NONE);}l285 = index;index = l132->l115[index&(0x3FFF)];;}}l132->l115[
l37&(0x3FFF)] = l132->l114[l283];l132->l114[l283] = l37;return(SOC_E_NONE);}
static int l138(l116*l132,l118 l133,l117 entry,int l134,uint16 l139){uint16
l283;uint16 index;uint16 l285;l283 = l124((uint8*)entry,(32*9))%l132->l113;
index = l132->l114[l283];;;l285 = (0xFFFF);while(index!= (0xFFFF)){if(l139 == 
index){;if(l285 == (0xFFFF)){l132->l114[l283] = l132->l115[l139&(0x3FFF)];
l132->l115[l139&(0x3FFF)] = (0xFFFF);}else{l132->l115[l285&(0x3FFF)] = l132->
l115[l139&(0x3FFF)];l132->l115[l139&(0x3FFF)] = (0xFFFF);}return(SOC_E_NONE);
}l285 = index;index = l132->l115[index&(0x3FFF)];;}return(SOC_E_NOT_FOUND);}
static int l140(l116*l132,l118 l133,l117 entry,int l134,uint16 l141){uint16
l283;uint16 index;l283 = l124((uint8*)entry,(32*9))%l132->l113;index = l132->
l114[l283];;;while(index!= (0xFFFF)){if(l141 == index){;return(SOC_E_NONE);}
index = l132->l115[index&(0x3FFF)];;}return(SOC_E_NOT_FOUND);}static int l286
(int l1,void*l11){return(SOC_E_NONE);}void soc_th_alpm_128_lpm_state_dump(int
l1){int l144;int l287;l287 = ((3*(128+2+1))-1);if(!bsl_check(bslLayerSoc,
bslSourceAlpm,bslSeverityVerbose,l1)){return;}for(l144 = l287;l144>= 0;l144--
){if((l144!= ((3*(128+2+1))-1))&&((l52[(l1)][(l144)].l45) == -1)){continue;}
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"PFX = %d P = %d N = %d START = %d "
"END = %d VENT = %d FENT = %d\n"),l144,(l52[(l1)][(l144)].l47),(l52[(l1)][(
l144)].next),(l52[(l1)][(l144)].l45),(l52[(l1)][(l144)].l46),(l52[(l1)][(l144
)].l48),(l52[(l1)][(l144)].l49)));}COMPILER_REFERENCE(l286);}static int l288(
int l1,int index,uint32*l11){int l289;uint32 l290,l291,l292;uint32 l293;int
l294;if(!SOC_URPF_STATUS_GET(l1)){return(SOC_E_NONE);}if(soc_feature(l1,
soc_feature_l3_defip_hole)){l289 = (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m
)>>1);}else if(SOC_IS_APOLLO(l1)){l289 = (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l289 = (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1);}if(((l110[(l1)]->l55)!= NULL)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l55),(0));}l290 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11),(l110[(l1)]->l68));l293 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l69));l291 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l11),(l110[(l1)]->l66));l292 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l67));l294 = ((!l290)&&(!l293)&&(!l291)&&(!l292))?1:0;l290 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l83));l293 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l11),(l110[(l1)]->l81));l291 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l82));l292 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l11),(l110[(l1)]->l82));if(l290&&l293&&l291&&l292){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l11
),(l110[(l1)]->l80),(l294));}return l176(l1,MEM_BLOCK_ANY,index+l289,index,
l11);}static int l295(int l1,int l296,int l297){uint32 l11[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l296,l11));l277(l1,l11,l297,0x4000,0);SOC_IF_ERROR_RETURN(l176(
l1,MEM_BLOCK_ANY,l297,l296,l11));SOC_IF_ERROR_RETURN(l288(l1,l297,l11));do{
int l298 = soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l296),1);int l299
= soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l297),1);int l300 = 
SOC_ALPM_128_ADDR_LWR((l299))<<1;ALPM_TCAM_PIVOT((l1),l300) = ALPM_TCAM_PIVOT
((l1),SOC_ALPM_128_ADDR_LWR((l298))<<1);if(ALPM_TCAM_PIVOT((l1),l300)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l300)) = l300;
soc_th_alpm_lpm_move_bu_upd((l1),PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l1),l300
)),l300);}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l298))<<1) = NULL;}while(
0);return(SOC_E_NONE);}static int l301(int l1,int l134,int l5){int l296;int
l297;l297 = (l52[(l1)][(l134)].l46)+1;l296 = (l52[(l1)][(l134)].l45);if(l296
!= l297){SOC_IF_ERROR_RETURN(l295(l1,l296,l297));VRF_PIVOT_SHIFT_INC(l1,
MAX_VRF_ID,l5);}(l52[(l1)][(l134)].l45)+= 1;(l52[(l1)][(l134)].l46)+= 1;
return(SOC_E_NONE);}static int l302(int l1,int l134,int l5){int l296;int l297
;l297 = (l52[(l1)][(l134)].l45)-1;if((l52[(l1)][(l134)].l48) == 0){(l52[(l1)]
[(l134)].l45) = l297;(l52[(l1)][(l134)].l46) = l297-1;return(SOC_E_NONE);}
l296 = (l52[(l1)][(l134)].l46);SOC_IF_ERROR_RETURN(l295(l1,l296,l297));
VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l5);(l52[(l1)][(l134)].l45)-= 1;(l52[(l1)][
(l134)].l46)-= 1;return(SOC_E_NONE);}static int l303(int l1,int l134,int l5,
void*l11,int*l304){int l305;int l306;int l307;int l308;if((l52[(l1)][(l134)].
l48) == 0){l308 = ((3*(128+2+1))-1);if((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM
)){if(l134<= (((3*(128+2+1))/3)-1)){l308 = (((3*(128+2+1))/3)-1);}}while((l52
[(l1)][(l308)].next)>l134){l308 = (l52[(l1)][(l308)].next);}l306 = (l52[(l1)]
[(l308)].next);if(l306!= -1){(l52[(l1)][(l306)].l47) = l134;}(l52[(l1)][(l134
)].next) = (l52[(l1)][(l308)].next);(l52[(l1)][(l134)].l47) = l308;(l52[(l1)]
[(l308)].next) = l134;(l52[(l1)][(l134)].l49) = ((l52[(l1)][(l308)].l49)+1)/2
;(l52[(l1)][(l308)].l49)-= (l52[(l1)][(l134)].l49);(l52[(l1)][(l134)].l45) = 
(l52[(l1)][(l308)].l46)+(l52[(l1)][(l308)].l49)+1;(l52[(l1)][(l134)].l46) = (
l52[(l1)][(l134)].l45)-1;(l52[(l1)][(l134)].l48) = 0;}l307 = l134;while((l52[
(l1)][(l307)].l49) == 0){l307 = (l52[(l1)][(l307)].next);if(l307 == -1){l307 = 
l134;break;}}while((l52[(l1)][(l307)].l49) == 0){l307 = (l52[(l1)][(l307)].
l47);if(l307 == -1){if((l52[(l1)][(l134)].l48) == 0){l305 = (l52[(l1)][(l134)
].l47);l306 = (l52[(l1)][(l134)].next);if(-1!= l305){(l52[(l1)][(l305)].next)
= l306;}if(-1!= l306){(l52[(l1)][(l306)].l47) = l305;}}return(SOC_E_FULL);}}
while(l307>l134){l306 = (l52[(l1)][(l307)].next);SOC_IF_ERROR_RETURN(l302(l1,
l306,l5));(l52[(l1)][(l307)].l49)-= 1;(l52[(l1)][(l306)].l49)+= 1;l307 = l306
;}while(l307<l134){SOC_IF_ERROR_RETURN(l301(l1,l307,l5));(l52[(l1)][(l307)].
l49)-= 1;l305 = (l52[(l1)][(l307)].l47);(l52[(l1)][(l305)].l49)+= 1;l307 = 
l305;}(l52[(l1)][(l134)].l48)+= 1;(l52[(l1)][(l134)].l49)-= 1;(l52[(l1)][(
l134)].l46)+= 1;*l304 = (l52[(l1)][(l134)].l46);sal_memcpy(l11,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);return(SOC_E_NONE);}static int l309(int l1,int l134,
int l5,void*l11,int l310){int l305;int l306;int l296;int l297;uint32 l311[
SOC_MAX_MEM_FIELD_WORDS];int l145;int l151;l296 = (l52[(l1)][(l134)].l46);
l297 = l310;(l52[(l1)][(l134)].l48)-= 1;(l52[(l1)][(l134)].l49)+= 1;(l52[(l1)
][(l134)].l46)-= 1;if(l297!= l296){if((l145 = READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l296,l311))<0){return l145;}l277(l1,l311,l297,0x4000,0);if((
l145 = l176(l1,MEM_BLOCK_ANY,l297,l296,l311))<0){return l145;}if((l145 = l288
(l1,l297,l311))<0){return l145;}}l151 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l297,1);l310 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l296,1);ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l151)<<1)
= ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l310)<<1);if(ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l151)<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l151)<<1)) = SOC_ALPM_128_ADDR_LWR(l151)<<1;
soc_th_alpm_lpm_move_bu_upd(l1,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l151)<<1)),SOC_ALPM_128_ADDR_LWR(l151)<<1);}
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l310)<<1) = NULL;sal_memcpy(l311,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);l277(l1,l311,l296,0x4000,0);if((l145 = l176(l1,
MEM_BLOCK_ANY,l296,l296,l311))<0){return l145;}if((l145 = l288(l1,l296,l311))
<0){return l145;}if((l52[(l1)][(l134)].l48) == 0){l305 = (l52[(l1)][(l134)].
l47);assert(l305!= -1);l306 = (l52[(l1)][(l134)].next);(l52[(l1)][(l305)].
next) = l306;(l52[(l1)][(l305)].l49)+= (l52[(l1)][(l134)].l49);(l52[(l1)][(
l134)].l49) = 0;if(l306!= -1){(l52[(l1)][(l306)].l47) = l305;}(l52[(l1)][(
l134)].next) = -1;(l52[(l1)][(l134)].l47) = -1;(l52[(l1)][(l134)].l45) = -1;(
l52[(l1)][(l134)].l46) = -1;}return(l145);}int soc_th_alpm_128_lpm_vrf_get(
int unit,void*lpm_entry,int*vrf,int*l312){int vrf_id;if(((l110[(unit)]->l91)
!= NULL)){vrf_id = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l87));*l312 = vrf_id;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l91))){*vrf = vrf_id;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l93))){*vrf = SOC_L3_VRF_GLOBAL;*l312 = SOC_VRF_MAX
(unit)+1;}else{*vrf = SOC_L3_VRF_OVERRIDE;}}else{*vrf = SOC_L3_VRF_DEFAULT;}
return(SOC_E_NONE);}static int l142(int l1,void*entry,int*l13,int*vrf_id,int*
vrf){int l134=0;int l145;int l313;int l314;l145 = l143(l1,entry,&l134);if(
l145<0){return l145;}l134+= 0;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get
(l1,entry,&l313,&l145));if(vrf_id!= NULL){*vrf_id = l313;}if(vrf!= NULL){*vrf
= l145;}l314 = soc_th_alpm_mode_get(l1);switch(l313){case SOC_L3_VRF_GLOBAL:
if((l314 == SOC_ALPM_MODE_PARALLEL)||(l314 == SOC_ALPM_MODE_TCAM_ALPM)){*l13 = 
l134+((3*(128+2+1))/3);}else{*l13 = l134;}break;case SOC_L3_VRF_OVERRIDE:*l13
= l134+2*((3*(128+2+1))/3);break;default:if((l314 == SOC_ALPM_MODE_PARALLEL)
||(l314 == SOC_ALPM_MODE_TCAM_ALPM)){*l13 = l134;}else{*l13 = l134+((3*(128+2
+1))/3);}break;}return(SOC_E_NONE);}static int l10(int l1,void*key_data,void*
l11,int*l12,int*l13,int*l5,int*vrf_id,int*vrf){int l145;int l135;int l134 = 0
;*l5 = L3_DEFIP_MODE_128;l142(l1,key_data,&l134,vrf_id,vrf);*l13 = l134;if(
l281(l1,key_data,l134,&l135) == SOC_E_NONE){*l12 = l135;if((l145 = 
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(*l5)?*l12:(*l12>>1),l11))<0){return
l145;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}int
soc_th_alpm_128_lpm_init(int l1){int l287;int l144;int l315;int l316;if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}l287 = (3*(128+2
+1));l316 = sizeof(l50)*(l287);if((l52[(l1)]!= NULL)){SOC_IF_ERROR_RETURN(
soc_th_alpm_128_deinit(l1));}l110[l1] = sal_alloc(sizeof(l108),
"lpm_128_field_state");if(NULL == l110[l1]){return(SOC_E_MEMORY);}(l110[l1])
->l54 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l110[l1])->
l55 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l110[l1])->
l56 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,SRC_DISCARDf);(l110[l1])->
l57 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l110[l1])->l58 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l110[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l110[l1])->l60 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l110[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l110[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l110[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l110[l1])->l65 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l110[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l110[l1])->l68 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l110[l1])->
l66 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l110[
l1])->l69 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l110[l1])->l67 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l110[l1])->l72 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l110[l1])->l70 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l110[l1])->l73 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l110[l1])->l71 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l110[l1])->l76 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l110[l1])->l74 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l110[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l110[l1])->l75 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l110[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l110[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l110[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l110[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l110[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l110[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l110[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l110[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l110[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l110[l1])->l88 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l110[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l110[l1])->l91 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l110[l1])->
l89 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l110[l1
])->l92 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(
l110[l1])->l90 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
VRF_ID_MASK1_UPRf);(l110[l1])->l93 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l110[l1])->l94 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l110[l1])->l95 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l110[l1])->l96 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_SUB_BKT_PTRf);(l110[l1])->l97 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l110[l1])->l98 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf);(l110[l1])->l99 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_POOL_NUMBERf);(l110[l1])
->l100 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf);(
l110[l1])->l101 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
ENTRY_TYPE_MASK1_LWRf);(l110[l1])->l102 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf);(l110[l1])->l103 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_UPRf);(l110[l1])
->l104 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE0_LWRf);(l110[
l1])->l105 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE1_LWRf);(
l110[l1])->l106 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
ENTRY_TYPE0_UPRf);(l110[l1])->l107 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ENTRY_TYPE1_UPRf);(l52[(l1)]) = sal_alloc(l316,
"LPM 128 prefix info");if(NULL == (l52[(l1)])){sal_free(l110[l1]);l110[l1] = 
NULL;return(SOC_E_MEMORY);}SOC_ALPM_LPM_LOCK(l1);sal_memset((l52[(l1)]),0,
l316);for(l144 = 0;l144<l287;l144++){(l52[(l1)][(l144)].l45) = -1;(l52[(l1)][
(l144)].l46) = -1;(l52[(l1)][(l144)].l47) = -1;(l52[(l1)][(l144)].next) = -1;
(l52[(l1)][(l144)].l48) = 0;(l52[(l1)][(l144)].l49) = 0;}l315 = 
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(SOC_URPF_STATUS_GET(l1)){l315
>>= 1;}if((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL)||(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)){(l52[(l1)][(((3*(128+2+
1))-1))].l46) = (l315>>1)-1;(l52[(l1)][(((((3*(128+2+1))/3)-1)))].l49) = l315
>>1;(l52[(l1)][((((3*(128+2+1))-1)))].l49) = (l315-(l52[(l1)][(((((3*(128+2+1
))/3)-1)))].l49));}else{(l52[(l1)][((((3*(128+2+1))-1)))].l49) = l315;}if((
l121[(l1)])!= NULL){if(l129((l121[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}(l121[(l1)]) = NULL;}if(l127(l1,l315*2,l315,&(l121[(l1)]))<0)
{SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l1);return(
SOC_E_NONE);}int soc_th_alpm_128_lpm_deinit(int l1){if(!soc_feature(l1,
soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);if((l121[
(l1)])!= NULL){l129((l121[(l1)]));(l121[(l1)]) = NULL;}if((l52[(l1)]!= NULL))
{sal_free(l110[l1]);l110[l1] = NULL;sal_free((l52[(l1)]));(l52[(l1)]) = NULL;
}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}static int l2(int l1,void*l3,int
*l180){int l134;int index;int l5;uint32 l11[SOC_MAX_MEM_FIELD_WORDS];int l145
= SOC_E_NONE;int l317 = 0;sal_memcpy(l11,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);
SOC_ALPM_LPM_LOCK(l1);l145 = l10(l1,l3,l11,&index,&l134,&l5,0,0);if(l145 == 
SOC_E_NOT_FOUND){l145 = l303(l1,l134,l5,l11,&index);if(l145<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l145);}}else{l317 = 1;}*l180 = index;if(l145 == 
SOC_E_NONE){soc_th_alpm_128_lpm_state_dump(l1);LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_th_alpm_128_lpm_insert: %d %d\n"),index,l134));if(!l317){
l277(l1,l3,index,0x4000,0);}l145 = l176(l1,MEM_BLOCK_ANY,index,index,l3);if(
l145>= 0){l145 = l288(l1,index,l3);}}SOC_ALPM_LPM_UNLOCK(l1);return(l145);}
static int soc_th_alpm_128_lpm_delete(int l1,void*key_data){int l134;int index
;int l5;uint32 l11[SOC_MAX_MEM_FIELD_WORDS];int l145 = SOC_E_NONE;
SOC_ALPM_LPM_LOCK(l1);l145 = l10(l1,key_data,l11,&index,&l134,&l5,0,0);if(
l145 == SOC_E_NONE){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_lpm_delete: %d %d\n"),index,l134));l280(l1,key_data,index);l145 = 
l309(l1,l134,l5,l11,index);}soc_th_alpm_128_lpm_state_dump(l1);
SOC_ALPM_LPM_UNLOCK(l1);return(l145);}static int l14(int l1,void*key_data,
void*l11,int*l12){int l134;int l145;int l5;SOC_ALPM_LPM_LOCK(l1);l145 = l10(
l1,key_data,l11,l12,&l134,&l5,0,0);SOC_ALPM_LPM_UNLOCK(l1);return(l145);}
static int l4(int unit,void*key_data,int l5,int l6,int l7,int l8,
defip_aux_scratch_entry_t*l9){uint32 l147;uint32 l224[4] = {0,0,0,0};int l134
= 0;int l145 = SOC_E_NONE;l147 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l110[(unit)]->l83));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,VALIDf,l147);l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(unit)]->l72));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l9,MODEf,l147);l147 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(key_data),(l110[(unit)]->l104));soc_mem_field32_set(
unit,L3_DEFIP_AUX_SCRATCHm,l9,ENTRY_TYPEf,l7);l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(unit)]->l60));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l9,GLOBAL_ROUTEf,l147);l147 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l110[(unit)]->l57));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,ECMPf,l147);l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(unit)]->l59));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l9,ECMP_PTRf,l147);l147 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
unit,L3_DEFIP_PAIR_128m)),(key_data),(l110[(unit)]->l78));soc_mem_field32_set
(unit,L3_DEFIP_AUX_SCRATCHm,l9,NEXT_HOP_INDEXf,l147);l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(unit)]->l79));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l9,PRIf,l147);l147 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(key_data),(l110[(unit)]->l80));soc_mem_field32_set(unit
,L3_DEFIP_AUX_SCRATCHm,l9,RPEf,l147);l147 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l110[(unit)]->l87));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,VRFf,l147);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,DB_TYPEf,l6);l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(unit)]->l55));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l9,DST_DISCARDf,l147);l147 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l110[(unit)]->l54));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,CLASS_IDf,l147);l224[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l110[(unit)]->l64));l224[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l110[(unit)]->l65));l224[2
] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)
),(key_data),(l110[(unit)]->l62));l224[3] = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l110[(unit)]->l63));
soc_mem_field_set(unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)l9,IP_ADDRf,(uint32*)
l224);l145 = l143(unit,key_data,&l134);if(SOC_FAILURE(l145)){return l145;}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,IP_LENGTHf,l134);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l9,REPLACE_LENf,l8);return(
SOC_E_NONE);}static int l15(int unit,void*lpm_entry,void*l16,void*l17,
soc_mem_t l18,uint32 l19,uint32*l318,int l21){uint32 l147;uint32 l224[4];int
l134 = 0;int l145 = SOC_E_NONE;uint32 l20 = 0;sal_memset(l16,0,
soc_mem_entry_words(unit,l18)*4);l147 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l61));
soc_mem_field32_set(unit,l18,l16,HITf,l147);l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l83));soc_mem_field32_set(unit,l18,l16,VALIDf,l147)
;l147 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l57));soc_mem_field32_set(
unit,l18,l16,ECMPf,l147);l147 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l59));
soc_mem_field32_set(unit,l18,l16,ECMP_PTRf,l147);l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l78));soc_mem_field32_set(unit,l18,l16,
NEXT_HOP_INDEXf,l147);l147 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l79));
soc_mem_field32_set(unit,l18,l16,PRIf,l147);l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l80));soc_mem_field32_set(unit,l18,l16,RPEf,l147);
l147 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l55));soc_mem_field32_set(
unit,l18,l16,DST_DISCARDf,l147);l147 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l56));
soc_mem_field32_set(unit,l18,l16,SRC_DISCARDf,l147);l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l54));soc_mem_field32_set(unit,l18,l16,CLASS_IDf,
l147);soc_mem_field32_set(unit,l18,l16,SUB_BKT_PTRf,ALPM_BKT_SIDX(l21));l224[
0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m
)),(lpm_entry),(l110[(unit)]->l64));l224[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l65));l224[2] = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l62));l224
[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l63));soc_mem_field_set(unit,
l18,(uint32*)l16,KEYf,(uint32*)l224);l145 = l143(unit,lpm_entry,&l134);if(
SOC_FAILURE(l145)){return l145;}if((l134 == 0)&&(l224[0] == 0)&&(l224[1] == 0
)&&(l224[2] == 0)&&(l224[3] == 0)){l20 = 1;}if(l318!= NULL){*l318 = l20;}
soc_mem_field32_set(unit,l18,l16,LENGTHf,l134);if(l17 == NULL){return(
SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){sal_memset(l17,0,
soc_mem_entry_words(unit,l18)*4);sal_memcpy(l17,l16,soc_mem_entry_words(unit,
l18)*4);soc_mem_field32_set(unit,l18,l17,DST_DISCARDf,0);soc_mem_field32_set(
unit,l18,l17,RPEf,0);soc_mem_field32_set(unit,l18,l17,SRC_DISCARDf,l19&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(unit,l18,l17,DEFAULTROUTEf,l20)
;}return(SOC_E_NONE);}static int l22(int unit,void*l16,soc_mem_t l18,int l5,
int vrf,int l23,int index,void*lpm_entry){uint32 l147;uint32 l224[4];uint32
vrf_id,l319;sal_memset(lpm_entry,0,soc_mem_entry_words(unit,
L3_DEFIP_PAIR_128m)*4);l147 = soc_mem_field32_get(unit,l18,l16,HITf);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l61),(l147));l147 = soc_mem_field32_get(unit,l18,
l16,VALIDf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l83),(l147));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l84),(l147));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l81),(l147)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l110[(unit)]->l82),(l147));l147 = soc_mem_field32_get(unit,l18,
l16,ECMPf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l57),(l147));l147 = 
soc_mem_field32_get(unit,l18,l16,ECMP_PTRf);soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l59),(
l147));l147 = soc_mem_field32_get(unit,l18,l16,NEXT_HOP_INDEXf);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l78),(l147));l147 = soc_mem_field32_get(unit,l18,
l16,PRIf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l79),(l147));l147 = 
soc_mem_field32_get(unit,l18,l16,RPEf);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l80),(l147)
);l147 = soc_mem_field32_get(unit,l18,l16,DST_DISCARDf);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l55),(l147));l147 = soc_mem_field32_get(unit,l18,
l16,SRC_DISCARDf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l56),(l147));l147 = 
soc_mem_field32_get(unit,l18,l16,CLASS_IDf);soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l54),(
l147));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l95),(ALPM_BKT_IDX(l23)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l96),(ALPM_BKT_SIDX(l23)));if(index>0){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l94),(index));}soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l76),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l77),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l74),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l75),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l100),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l101),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l102),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l103),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l72),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l73),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l70),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l71),(3));soc_mem_field_get(unit,l18,l16,KEYf,l224)
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l64),(l224[0]));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l65),(l224
[1]));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l62),(l224[2]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l63),(l224[3]));l224[0] = l224[1] = l224[2] = l224[
3] = 0;l147 = soc_mem_field32_get(unit,l18,l16,LENGTHf);l152(unit,lpm_entry,
l147);if(vrf == SOC_L3_VRF_OVERRIDE){soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l93),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l60),(1));vrf_id = 0;l319 = 0;}else if(vrf == 
SOC_L3_VRF_GLOBAL){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l60),(1));vrf_id = 0;l319 = 0
;}else{vrf_id = vrf;l319 = SOC_VRF_MAX(unit);}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l87),(vrf_id));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l88),(
vrf_id));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l85),(vrf_id));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l86),(vrf_id));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l91),(l319)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l110[(unit)]->l92),(l319));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l89),(l319)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l110[(unit)]->l90),(l319));return(SOC_E_NONE);}int
soc_th_alpm_128_warmboot_pivot_add(int unit,int l5,void*lpm_entry,int l320,
int bktid){int l145 = SOC_E_NONE;uint32 key[4] = {0,0,0,0};alpm_pivot_t*l32 = 
NULL;alpm_bucket_handle_t*l211 = NULL;int vrf_id = 0,vrf = 0;uint32 l321;
trie_t*l242 = NULL;uint32 prefix[5] = {0};int l20 = 0;l145 = 
_soc_th_alpm_128_prefix_create(unit,lpm_entry,prefix,&l321,&l20);
SOC_IF_ERROR_RETURN(l145);SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(
unit,lpm_entry,&vrf_id,&vrf));l320 = soc_th_alpm_physical_idx(unit,
L3_DEFIP_PAIR_128m,l320,l5);l211 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l211 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for PIVOT trie node \n")));return SOC_E_NONE;
}sal_memset(l211,0,sizeof(*l211));l32 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l32 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for PIVOT trie node \n")));sal_free(l211);
return SOC_E_MEMORY;}sal_memset(l32,0,sizeof(*l32));PIVOT_BUCKET_HANDLE(l32) = 
l211;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l32));key[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l64));key[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l65));key[2
] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)
),(lpm_entry),(l110[(unit)]->l62));key[3] = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l63));
PIVOT_BUCKET_INDEX(l32) = bktid;PIVOT_TCAM_INDEX(l32) = SOC_ALPM_128_ADDR_LWR
(l320)<<1;if(!(((vrf_id == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(unit)
== SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == SOC_L3_VRF_GLOBAL))))){l242 = 
VRF_PIVOT_TRIE_IPV6_128(unit,vrf);if(l242 == NULL){trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(unit,vrf));l242 = 
VRF_PIVOT_TRIE_IPV6_128(unit,vrf);}sal_memcpy(l32->key,prefix,sizeof(prefix))
;l32->len = l321;l145 = trie_insert(l242,l32->key,NULL,l32->len,(trie_node_t*
)l32);if(SOC_FAILURE(l145)){sal_free(l211);sal_free(l32);return l145;}}
ALPM_TCAM_PIVOT(unit,SOC_ALPM_128_ADDR_LWR(l320)<<1) = l32;PIVOT_BUCKET_VRF(
l32) = vrf;PIVOT_BUCKET_IPV6(l32) = l5;PIVOT_BUCKET_ENT_CNT_UPDATE(l32);if(
key[0] == 0&&key[1] == 0&&key[2] == 0&&key[3] == 0){PIVOT_BUCKET_DEF(l32) = 
TRUE;}VRF_PIVOT_REF_INC(unit,vrf,l5);return l145;}static int l322(int unit,
int l5,void*lpm_entry,void*l16,soc_mem_t l18,int l320,int bktid,int l323){int
l324;int vrf;int l145 = SOC_E_NONE;int l20 = 0;uint32 prefix[5] = {0,0,0,0,0}
;uint32 l33;defip_pair_128_entry_t l325;trie_t*l38 = NULL;trie_t*l31 = NULL;
trie_node_t*l201 = NULL;payload_t*l326 = NULL;payload_t*l222 = NULL;
alpm_pivot_t*pivot_pyld = NULL;if((NULL == lpm_entry)||(NULL == l16)){return
SOC_E_PARAM;}SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&
l324,&vrf));l18 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(l22(unit,l16,
l18,l5,l324,bktid,l320,&l325));l145 = _soc_th_alpm_128_prefix_create(unit,&
l325,prefix,&l33,&l20);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"prefix create failed\n")));return l145;}pivot_pyld = 
ALPM_TCAM_PIVOT(unit,l320);l38 = PIVOT_BUCKET_TRIE(pivot_pyld);l326 = 
sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == l326){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for trie node.\n"
)));return SOC_E_MEMORY;}l222 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(NULL == l222){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node\n")));sal_free(
l326);return SOC_E_MEMORY;}sal_memset(l326,0,sizeof(*l326));sal_memset(l222,0
,sizeof(*l222));sal_memcpy(l326->key,prefix,sizeof(l326->key));l326->len = 
l33;l326->index = l323;sal_memcpy(l222,l326,sizeof(*l326));l145 = trie_insert
(l38,prefix,NULL,l33,(trie_node_t*)l326);if(SOC_FAILURE(l145)){goto l327;}if(
l5){l31 = VRF_PREFIX_TRIE_IPV6_128(unit,vrf);}if(!l20){l145 = trie_insert(l31
,prefix,NULL,l33,(trie_node_t*)l222);if(SOC_FAILURE(l145)){goto l328;}}return
l145;l328:(void)trie_delete(l38,prefix,l33,&l201);l326 = (payload_t*)l201;
l327:sal_free(l326);sal_free(l222);return l145;}static int l329(int unit,int
l28,int vrf,int l244,int bkt_ptr){int l145 = SOC_E_NONE;uint32 l33;uint32 key
[5] = {0,0,0,0,0};trie_t*l330 = NULL;payload_t*l232 = NULL;
defip_pair_128_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_pair_128_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unable to allocate memory for LPM entry\n")
));return SOC_E_MEMORY;}l24(unit,key,0,vrf,l28,lpm_entry,0,1);if(vrf == 
SOC_VRF_MAX(unit)+1){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l60),(1));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(unit)]->l97),(1));}soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l95),(
ALPM_BKT_IDX(bkt_ptr)));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit
,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(unit)]->l96),(ALPM_BKT_SIDX(bkt_ptr)
));VRF_TRIE_DEFAULT_ROUTE_IPV6_128(unit,vrf) = lpm_entry;trie_init(
_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(unit,vrf));l330 = 
VRF_PREFIX_TRIE_IPV6_128(unit,vrf);l232 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l232 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l232,0,sizeof(*l232));l232->key[0] = key[0];l232->
key[1] = key[1];l232->len = l33 = 0;l145 = trie_insert(l330,key,NULL,l33,&(
l232->node));if(SOC_FAILURE(l145)){sal_free(l232);return l145;}
VRF_TRIE_INIT_DONE(unit,vrf,l28,1);return l145;}int
soc_th_alpm_128_warmboot_prefix_insert(int unit,int l5,void*lpm_entry,void*
l16,int l320,int bktid,int l323){int l324;int vrf;int l145 = SOC_E_NONE;
soc_mem_t l18;l320 = soc_th_alpm_physical_idx(unit,L3_DEFIP_PAIR_128m,l320,l5
);l320 = SOC_ALPM_128_ADDR_LWR(l320)<<1;l18 = L3_DEFIP_ALPM_IPV6_128m;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&l324,&vrf));
if(((l324 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(unit) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l324 == SOC_L3_VRF_GLOBAL)))){return(l145);}if(!
VRF_TRIE_INIT_COMPLETED(unit,vrf,l5)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U
(unit,"VRF %d is not initialized\n"),vrf));l145 = l329(unit,l5,vrf,l320,bktid
);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d/%d trie init \n""failed\n"),vrf,l5));return l145;}}l145 = l322(unit,
l5,lpm_entry,l16,l18,l320,bktid,l323);if(l145!= SOC_E_NONE){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unit %d : ""Route Insertion Failed :%s\n"),
unit,soc_errmsg(l145)));return(l145);}soc_th_alpm_bu_upd(unit,bktid,l320,vrf,
l5,1);VRF_TRIE_ROUTES_INC(unit,vrf,l5);return(l145);}int
soc_th_alpm_128_warmboot_bucket_bitmap_set(int l1,int vrf,int l28,int l247){
int l331 = 1;SHR_BITDCL*l332 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l1);if(vrf == 
SOC_VRF_MAX(l1)+1){l332 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l1);}if(
SOC_TH_ALPM_SCALE_CHECK(l1,l28)){l331 = 2;}SHR_BITSET_RANGE(l332,l247,l331);
return SOC_E_NONE;}int soc_th_alpm_128_warmboot_lpm_reinit_done(int unit){int
l244;int l333 = ((3*(128+2+1))-1);int l315 = soc_mem_index_count(unit,
L3_DEFIP_PAIR_128m);if(SOC_URPF_STATUS_GET(unit)){l315>>= 1;}if(
soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_COMBINED){(l52[(unit)][(((3*(128+
2+1))-1))].l47) = -1;for(l244 = ((3*(128+2+1))-1);l244>-1;l244--){if(-1 == (
l52[(unit)][(l244)].l45)){continue;}(l52[(unit)][(l244)].l47) = l333;(l52[(
unit)][(l333)].next) = l244;(l52[(unit)][(l333)].l49) = (l52[(unit)][(l244)].
l45)-(l52[(unit)][(l333)].l46)-1;l333 = l244;}(l52[(unit)][(l333)].next) = -1
;(l52[(unit)][(l333)].l49) = l315-(l52[(unit)][(l333)].l46)-1;}else{(l52[(
unit)][(((3*(128+2+1))-1))].l47) = -1;for(l244 = ((3*(128+2+1))-1);l244>(((3*
(128+2+1))-1)/3);l244--){if(-1 == (l52[(unit)][(l244)].l45)){continue;}(l52[(
unit)][(l244)].l47) = l333;(l52[(unit)][(l333)].next) = l244;(l52[(unit)][(
l333)].l49) = (l52[(unit)][(l244)].l45)-(l52[(unit)][(l333)].l46)-1;l333 = 
l244;}(l52[(unit)][(l333)].next) = -1;(l52[(unit)][(l333)].l49) = l315-(l52[(
unit)][(l333)].l46)-1;l333 = (((3*(128+2+1))-1)/3);(l52[(unit)][((((3*(128+2+
1))-1)/3))].l47) = -1;for(l244 = ((((3*(128+2+1))-1)/3)-1);l244>-1;l244--){if
(-1 == (l52[(unit)][(l244)].l45)){continue;}(l52[(unit)][(l244)].l47) = l333;
(l52[(unit)][(l333)].next) = l244;(l52[(unit)][(l333)].l49) = (l52[(unit)][(
l244)].l45)-(l52[(unit)][(l333)].l46)-1;l333 = l244;}(l52[(unit)][(l333)].
next) = -1;(l52[(unit)][(l333)].l49) = (l315>>1)-(l52[(unit)][(l333)].l46)-1;
}return(SOC_E_NONE);}int soc_th_alpm_128_warmboot_lpm_reinit(int unit,int l5,
int l244,void*lpm_entry){int l13;l277(unit,lpm_entry,l244,0x4000,0);
SOC_IF_ERROR_RETURN(l142(unit,lpm_entry,&l13,0,0));if((l52[(unit)][(l13)].l48
) == 0){(l52[(unit)][(l13)].l45) = l244;(l52[(unit)][(l13)].l46) = l244;}else
{(l52[(unit)][(l13)].l46) = l244;}(l52[(unit)][(l13)].l48)++;return(
SOC_E_NONE);}int _soc_th_alpm_128_sanity_check(int l1,int l334,int l335,int
l336,int*l337){int l145 = SOC_E_NONE;int l338,l339,l340,l323;int l252,l5;int
vrf_id,vrf;int l341,l342,l343;int l344,l257,bkt_ptr,l345;int l346 = 0,l347 = 
0,l348 = 0,l349 = 0;int l350;int l35,l351;int l352,l20;int l353;uint32 l159 = 
0;uint32 l11[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l170[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 prefix[5],l33;trie_t*l31 = NULL;
trie_node_t*l157 = NULL;payload_t*l208 = NULL;uint32 l354[10] = {0};uint32
l355[10] = {0};char*l356 = NULL;defip_pair_128_entry_t*lpm_entry,l357,l358;
soc_mem_t l359;soc_mem_t l360 = L3_DEFIP_ALPM_IPV6_128m;l353 = sizeof(
defip_pair_128_entry_t)*(l335-l334+1);l356 = soc_cm_salloc(l1,l353,"lpm_tbl")
;if(l356 == NULL){return(SOC_E_MEMORY);}sal_memset(l356,0,l353);
SOC_ALPM_LPM_LOCK(l1);if(soc_mem_read_range(l1,L3_DEFIP_PAIR_128m,
MEM_BLOCK_ANY,l334,l335,l356)<0){soc_cm_sfree(l1,l356);SOC_ALPM_LPM_UNLOCK(l1
);return(SOC_E_INTERNAL);}l5 = L3_DEFIP_MODE_128;l257 = 8;if(
SOC_TH_ALPM_SCALE_CHECK(l1,l5)){l257<<= 1;}for(l338 = l334;l338<= l335;l338++
){if(l346){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP_PAIR_128 index:%d check failed. error count %d\n"),l338-1,l346));
l348 = 1;}else if(l349){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP_PAIR_128 index:%d check passed. success count %d\n"),l338-1,l349)
);}l346 = l347 = l349 = 0;lpm_entry = soc_mem_table_idx_to_pointer(l1,
L3_DEFIP_PAIR_128m,defip_pair_128_entry_t*,l356,l338-l334);bkt_ptr = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(l1)]->l95));if(!soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(l1)]->l83))){continue
;}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m))
,(lpm_entry),(l110[(l1)]->l93))||(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(l1)]->l60))&&
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)){if(bkt_ptr!= 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tWrong bkt_ptr %d\n"),bkt_ptr));
l346++;}continue;}if(bkt_ptr == 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tWrong bkt_ptr2 %d\n"),bkt_ptr));l346++;}l252 = 0;l354[l252++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(l1)]->l57));l354[l252++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(l1)]->l59));l354[l252++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(l1)]->l78));l354[l252++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(l1)]->l79));l354[l252++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(l1)]->l80));l354[l252++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(l1)]->l55));l354[l252++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l110[(l1)]->l54));l345 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(l1)]->l96));if(l337){
if(l337[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l345] == -1){l337[bkt_ptr*
SOC_TH_MAX_SUB_BUCKETS+l345] = l338;if(SOC_TH_ALPM_SCALE_CHECK(l1,l5)){l337[(
bkt_ptr+1)*SOC_TH_MAX_SUB_BUCKETS+l345] = l338;}l349++;}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tConflict bucket pointer [%d,%d]: "
"was %d now %d\n"),bkt_ptr,l345,l337[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l345],
l338));l346++;}}l339 = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,l338,1)
;l145 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(l339),l170);l35 = -1;if(SOC_SUCCESS(l145)){l35 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l170,BPM_LENGTH0f);l351 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,l170,IP_LENGTH0f);if(l35>l351){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tAUX Table corrupted\n")));l346++;
}else{l349++;}if(l35 == 0&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l110[(l1)]->l97))){continue;}sal_memcpy(
&l358,lpm_entry,sizeof(defip_pair_128_entry_t));l152(l1,&l358,l35);(void)
soc_th_alpm_128_lpm_vrf_get(l1,lpm_entry,&vrf_id,&vrf);l341 = l342 = l343 = -
1;l145 = _soc_th_alpm_128_find(l1,l360,&l358,vrf_id,vrf,l11,&l341,&l342,&l343
,FALSE);if(SOC_SUCCESS(l145)){l252 = 0;l355[l252++] = soc_mem_field32_get(l1,
l360,l11,ECMPf);l355[l252++] = soc_mem_field32_get(l1,l360,l11,ECMP_PTRf);
l355[l252++] = soc_mem_field32_get(l1,l360,l11,NEXT_HOP_INDEXf);l355[l252++] = 
soc_mem_field32_get(l1,l360,l11,PRIf);l355[l252++] = soc_mem_field32_get(l1,
l360,l11,RPEf);l355[l252++] = soc_mem_field32_get(l1,l360,l11,DST_DISCARDf);
l355[l252++] = soc_mem_field32_get(l1,l360,l11,CLASS_IDf);if(sal_memcmp(l354,
l355,sizeof(l354))!= 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tData mismatch: ""lpm_idx %d find_lpm_idx %d "
"find_alpm_idx %d find_bkt_ptr %d\n"),l338,l341,l343,l342));l346++;}else{l349
++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\t_soc_th_alpm_128_find rv=%d: ""lpm_idx %d find_lpm_idx %d "
"find_alpm_idx %d find_bkt_ptr %d\n"),l145,l338,l341,l343,l342));l346++;}}for
(l344 = 0;l344<l257;l344++){(void)_soc_th_alpm_mem_index(l1,l360,bkt_ptr,l344
,l159,&l323);l145 = soc_mem_read(l1,l360,MEM_BLOCK_ANY,l323,l11);if(
SOC_FAILURE(l145)){continue;}if(!soc_mem_field32_get(l1,l360,l11,VALIDf)){
continue;}if(l345!= soc_mem_field32_get(l1,l360,l11,SUB_BKT_PTRf)){continue;}
(void)l22(l1,l11,l360,l5,vrf_id,ALPM_BKTID(bkt_ptr,l345),0,&l357);l340 = l341
= l342 = l343 = -1;l145 = _soc_th_alpm_128_find(l1,l360,&l357,vrf_id,vrf,l11,
&l341,&l342,&l343,FALSE);if(SOC_SUCCESS(l145)){l340 = soc_th_alpm_logical_idx
(l1,L3_DEFIP_PAIR_128m,SOC_ALPM_128_DEFIP_TO_PAIR(l341>>1),1);}if(SOC_FAILURE
(l145)||l342!= ALPM_BKTID(bkt_ptr,l345)||l340!= l338||
SOC_ALPM_128_DEFIP_TO_PAIR(l341>>1)!= l339||l343!= l323){l346++;l347++;
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\t_soc_th_alpm_128_find2 rv=%d: "
"fl [%d,%d] fa %d fb [%d,%d] ""l [%d,%d] a %d b [%d,%d]\n"),l145,
SOC_ALPM_128_DEFIP_TO_PAIR(l341>>1),l340,l343,ALPM_BKT_IDX(l342),
ALPM_BKT_SIDX(l342),l339,l338,l323,bkt_ptr,l345));}}if(l347 == 0){l349++;}if(
!l336){continue;}l145 = l282(l1,lpm_entry,l338);if(SOC_FAILURE(l145)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tLPM Hash check failed: rv %d "
"key_index %d lpm_idx %d\n"),l145,l350,l338));l346++;}else{l349++;}l145 = 
soc_th_alpm_bucket_is_assigned(l1,bkt_ptr,vrf,l5,&l350);if(SOC_FAILURE(l145)
||l345>= SOC_TH_MAX_SUB_BUCKETS){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tInvalid bucket pointer [%d,%d] ""detected, in memory %s index %d\n"),
bkt_ptr,l345,SOC_MEM_NAME(l1,L3_DEFIP_PAIR_128m),l338));l346++;}else if(l350
== 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),bkt_ptr,SOC_MEM_NAME(l1,
L3_DEFIP_PAIR_128m),l338));l346++;}else{l349++;}l359 = 
_soc_tomahawk_alpm_bkt_view_get(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,ALPM_BKTID
(bkt_ptr,0)));if(l360!= l359){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMismatched alpm view ""in bucket %d, expected %s, was %s\n"),bkt_ptr,
SOC_MEM_NAME(l1,l360),SOC_MEM_NAME(l1,l359)));l346++;}else{l349++;}l145 = 
_soc_th_alpm_128_prefix_create(l1,lpm_entry,prefix,&l33,&l20);if(SOC_SUCCESS(
l145)){l31 = VRF_PREFIX_TRIE_IPV6_128(l1,vrf);l157 = NULL;if(l31){l145 = 
trie_find_lpm(l31,prefix,l33,&l157);}if(SOC_SUCCESS(l145)&&l157){l208 = (
payload_t*)l157;l352 = ((payload_t*)(l208->bkt_ptr))->len;if(l352!= l35){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tBPM len mismatch: lpm_idx %d"
" alpm_idx %d bpm_len %d trie_bpm_len %d\n"),l338,l323,l35,l352));l346++;}
else{l349++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\ttrie_find_lpm failed: ""lpm_idx %d alpm_idx %d lpmp %p\n"),l338,l323,l157)
);l346++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tPrefix creating failed: ""lpm_idx %d alpm_idx %d\n"),l338,l323));l346++;}}
if(l346){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP_PAIR_128 index:%d check failed. error count %d\n"),l338-1,l346));
l348 = 1;}else if(l349){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tL3_DEFIP_PAIR_128 index:%d check passed. success count %d\n"),l338-1,l349)
);}SOC_ALPM_LPM_UNLOCK(l1);soc_cm_sfree(l1,l356);return(l348?SOC_E_FAIL:
SOC_E_NONE);}
#endif /* ALPM_ENABLE */
