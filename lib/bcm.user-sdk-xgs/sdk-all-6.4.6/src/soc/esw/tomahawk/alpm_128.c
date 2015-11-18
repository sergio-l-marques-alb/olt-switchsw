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
l4(int l1,void*key_data,int l5,int l6,int l7,defip_aux_scratch_entry_t*l8);
static int l9(int l1,void*key_data,void*l10,int*l11,int*l12,int*l5,int*vrf_id
,int*vrf);static int l13(int l1,void*key_data,void*l10,int*l11);static int l14
(int unit,void*lpm_entry,void*l15,void*l16,soc_mem_t l17,uint32 l18,uint32*
l19,int l20);static int l21(int unit,void*l15,soc_mem_t l17,int l5,int vrf,
int l22,int index,void*lpm_entry);static int l23(int unit,uint32*key,int len,
int vrf,int l5,defip_pair_128_entry_t*lpm_entry,int l24,int l25);static int
l26(int l1,int vrf,int l27);static int l28(int l1,alpm_pfx_info_t*l29,trie_t*
l30,uint32*l31,uint32 l32,trie_node_t*l33,defip_pair_128_entry_t*lpm_entry,
uint32*l34);static int _soc_th_alpm_128_move_inval(int l1,soc_mem_t l17,
alpm_mem_prefix_array_t*l35,int*l36);extern int soc_th_get_alpm_banks(int unit
);extern int soc_th_alpm_mode_get(int l1);extern int
_soc_th_alpm_free_pfx_trie(int l1,trie_t*l30,trie_t*l37,payload_t*
new_pfx_pyld,int*l36,int bktid,int vrf,int l27);extern int
_soc_th_alpm_rollback_bkt_move(int l1,int l27,void*key_data,soc_mem_t l17,
alpm_pivot_t*l38,alpm_pivot_t*l39,alpm_mem_prefix_array_t*l40,int*l36,int l41
);extern void soc_th_alpm_lpm_move_bu_upd(int l1,int l20,int l42);extern int
soc_th_alpm_lpm_delete(int l1,void*key_data);int alpm_128_split_count;typedef
struct l43{int l44;int l45;int l46;int next;int l47;int l48;}l49,*l50;static
l50 l51[SOC_MAX_NUM_DEVICES];typedef struct l52{soc_field_info_t*l53;
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
soc_field_info_t*l96;soc_field_info_t*l97;soc_field_info_t*l98;
soc_field_info_t*l99;soc_field_info_t*l100;soc_field_info_t*l101;
soc_field_info_t*l102;soc_field_info_t*l103;soc_field_info_t*l104;
soc_field_info_t*l105;soc_field_info_t*l106;}l107,*l108;static l108 l109[
SOC_MAX_NUM_DEVICES];typedef struct l110{int unit;int l111;int l112;uint16*
l113;uint16*l114;}l115;typedef uint32 l116[9];typedef int(*l117)(l116 l118,
l116 l119);static l115*l120[SOC_MAX_NUM_DEVICES];static void l121(int l1,void
*l10,int index,l116 l122);static uint16 l123(uint8*l124,int l125);static int
l126(int unit,int l111,int l112,l115**l127);static int l128(l115*l129);static
int l130(l115*l131,l117 l132,l116 entry,int l133,uint16*l134);static int l135
(l115*l131,l117 l132,l116 entry,int l133,uint16 l136,uint16 l36);static int
l137(l115*l131,l117 l132,l116 entry,int l133,uint16 l138);static int l139(int
,void*,int*,int*,int*);static int l140(int l1,const void*entry,int*l133){int
l141,l142;int l143[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};uint32 l144;l144 = soc_mem_field32_get
(l1,L3_DEFIP_PAIR_128m,entry,l143[0]);if((l142 = _ipmask2pfx(l144,l133))<0){
return(l142);}for(l141 = 1;l141<4;l141++){l144 = soc_mem_field32_get(l1,
L3_DEFIP_PAIR_128m,entry,l143[l141]);if(*l133){if(l144!= 0xffffffff){return(
SOC_E_PARAM);}*l133+= 32;}else{if((l142 = _ipmask2pfx(l144,l133))<0){return(
l142);}}}return SOC_E_NONE;}static void l145(uint32*l146,int l32,int l27){
uint32 l147,l148,l44,prefix[6];int l141;sal_memcpy(prefix,l146,sizeof(uint32)
*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(l146,0,sizeof(uint32)*BITS2WORDS(
_MAX_KEY_LEN_144_));l147 = 128-l32;l44 = (l147+31)/32;if((l147%32) == 0){l44
++;}l147 = l147%32;for(l141 = l44;l141<= 4;l141++){prefix[l141]<<= l147;l148 = 
prefix[l141+1]&~(0xffffffff>>l147);l148 = (((32-l147) == 32)?0:(l148)>>(32-
l147));if(l141<4){prefix[l141]|= l148;}}for(l141 = l44;l141<= 4;l141++){l146[
3-(l141-l44)] = prefix[l141];}}static void l149(int unit,void*lpm_entry,int
l12){int l141;soc_field_t l150[4] = {IP_ADDR_MASK0_LWRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_UPRf};for(l141 = 0;l141<4;l141++){
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l150[l141],0);}for(l141
= 0;l141<4;l141++){if(l12<= 32)break;soc_mem_field32_set(unit,
L3_DEFIP_PAIR_128m,lpm_entry,l150[3-l141],0xffffffff);l12-= 32;}
soc_mem_field32_set(unit,L3_DEFIP_PAIR_128m,lpm_entry,l150[3-l141],~(((l12) == 
32)?0:(0xffffffff)>>(l12)));}int _soc_th_alpm_128_prefix_create(int l1,void*
entry,uint32*l146,uint32*l12,int*l19){int l141;int l133 = 0,l44;int l142 = 
SOC_E_NONE;uint32 l147,l148;uint32 prefix[5];sal_memset(l146,0,sizeof(uint32)
*BITS2WORDS(_MAX_KEY_LEN_144_));sal_memset(prefix,0,sizeof(uint32)*BITS2WORDS
(_MAX_KEY_LEN_144_));prefix[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(entry),(l109[(l1)]->l63));prefix[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
entry),(l109[(l1)]->l64));prefix[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(entry),(l109[(l1)]->l61));prefix[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
entry),(l109[(l1)]->l62));l142 = l140(l1,entry,&l133);if(SOC_FAILURE(l142)){
return l142;}l147 = 128-l133;l44 = l147/32;l147 = l147%32;for(l141 = l44;l141
<4;l141++){prefix[l141]>>= l147;l148 = prefix[l141+1]&((1<<l147)-1);l148 = ((
(32-l147) == 32)?0:(l148)<<(32-l147));prefix[l141]|= l148;}for(l141 = l44;
l141<4;l141++){l146[4-(l141-l44)] = prefix[l141];}*l12 = l133;if(l19!= NULL){
*l19 = (prefix[0] == 0)&&(prefix[1] == 0)&&(prefix[2] == 0)&&(prefix[3] == 0)
&&(l133 == 0);}return SOC_E_NONE;}int l151(int l1,uint32*prefix,uint32 l32,
int l5,int vrf,int*l152,int*tcam_index,int*bktid){int l142 = SOC_E_NONE;
trie_t*l153;trie_node_t*l154 = NULL;alpm_pivot_t*pivot_pyld;l153 = 
VRF_PIVOT_TRIE_IPV6_128(l1,vrf);l142 = trie_find_lpm(l153,prefix,l32,&l154);
if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Pivot find failed\n")));return l142;}pivot_pyld = (alpm_pivot_t*)l154;*l152 = 
1;*tcam_index = PIVOT_TCAM_INDEX(pivot_pyld);*bktid = PIVOT_BUCKET_INDEX(
pivot_pyld);return SOC_E_NONE;}static int _soc_th_alpm_128_find(int l1,
soc_mem_t l17,void*key_data,int vrf_id,int vrf,void*alpm_data,int*tcam_index,
int*bktid,int*l11,int l155){uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l27;int
l134;uint32 l6,l156;int l142 = SOC_E_NONE;int l152 = 0;l27 = 
L3_DEFIP_MODE_128;if(vrf_id == 0){if(soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL||soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){
return SOC_E_PARAM;}}soc_th_alpm_bank_db_type_get(l1,vrf,&l156,&l6);if(!(((
vrf_id == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == SOC_L3_VRF_GLOBAL))))){if(l155){uint32
prefix[5],l32;int l19 = 0;l142 = _soc_th_alpm_128_prefix_create(l1,key_data,
prefix,&l32,&l19);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"_soc_alpm_insert: prefix create failed\n")));return l142;}l142 = l151(l1
,prefix,l32,l27,vrf,&l152,tcam_index,bktid);SOC_IF_ERROR_RETURN(l142);}else{
defip_aux_scratch_entry_t l8;sal_memset(&l8,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l27,l6,0,&l8))
;SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,PREFIX_LOOKUP,&l8,TRUE,&l152,
tcam_index,bktid));}if(l152){l14(l1,key_data,l10,0,l17,0,0,*bktid);l142 = 
_soc_th_alpm_find_in_bkt(l1,l17,*bktid,l156,l10,alpm_data,&l134,l27);if(
SOC_SUCCESS(l142)){*l11 = l134;}}else{l142 = SOC_E_NOT_FOUND;}}return l142;}
static int l157(int l1,void*key_data,void*alpm_data,void*alpm_sip_data,
soc_mem_t l17,int l134){defip_aux_scratch_entry_t l8;int vrf_id,l27,vrf;int
bktid;uint32 l6,l156;int l142 = SOC_E_NONE;int l152 = 0,l148 = 0;int
tcam_index,index;uint32 l158[SOC_MAX_MEM_FIELD_WORDS];l27 = L3_DEFIP_MODE_128
;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&vrf_id,&vrf));
soc_th_alpm_bank_db_type_get(l1,vrf,&l156,&l6);if(soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_COMBINED||soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){
l6 = 2;}if(!(((vrf_id == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == SOC_L3_VRF_GLOBAL))))){sal_memset(&l8,0,
sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l27,l6,
0,&l8));SOC_ALPM_LPM_LOCK(l1);l142 = _soc_th_alpm_128_find(l1,l17,key_data,
vrf_id,vrf,l158,&tcam_index,&bktid,&index,TRUE);SOC_ALPM_LPM_UNLOCK(l1);
SOC_IF_ERROR_RETURN(l142);soc_mem_field32_set(l1,l17,alpm_data,SUB_BKT_PTRf,
ALPM_BKT_SIDX(bktid));SOC_IF_ERROR_RETURN(soc_mem_write(l1,l17,MEM_BLOCK_ANY,
ALPM_ENT_INDEX(l134),alpm_data));if(SOC_URPF_STATUS_GET(l1)){
soc_mem_field32_set(l1,l17,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));
SOC_IF_ERROR_RETURN(soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry
(l1,ALPM_ENT_INDEX(l134)),alpm_sip_data));}l148 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,IP_LENGTHf);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,REPLACE_LENf,l148);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l8,TRUE,&l152,&tcam_index,&bktid));
if(SOC_URPF_STATUS_GET(l1)){if(l148 == 0){soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,0);}l148 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf);l148+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf,l148);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l8,TRUE,&l152,&tcam_index,&bktid));}
}return l142;}static int l159(int l1,int l160,int l34){int l142,l148,l161,
l162;defip_aux_table_entry_t l163,l164;l161 = SOC_ALPM_128_ADDR_LWR(l160);
l162 = SOC_ALPM_128_ADDR_UPR(l160);l142 = soc_mem_read(l1,L3_DEFIP_AUX_TABLEm
,MEM_BLOCK_ANY,l161,&l163);SOC_IF_ERROR_RETURN(l142);l142 = soc_mem_read(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l162,&l164);SOC_IF_ERROR_RETURN(l142);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l163,BPM_LENGTH0f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l163,BPM_LENGTH1f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l164,BPM_LENGTH0f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l164,BPM_LENGTH1f,l34);l148 = 
soc_mem_field32_get(l1,L3_DEFIP_AUX_TABLEm,&l163,DB_TYPE0f);l142 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l161,&l163);
SOC_IF_ERROR_RETURN(l142);l142 = soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,
MEM_BLOCK_ANY,l162,&l164);SOC_IF_ERROR_RETURN(l142);if(SOC_URPF_STATUS_GET(l1
)){l148++;soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l163,BPM_LENGTH0f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l163,BPM_LENGTH1f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l164,BPM_LENGTH0f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l164,BPM_LENGTH1f,l34);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l163,DB_TYPE0f,l148);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l163,DB_TYPE1f,l148);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l164,DB_TYPE0f,l148);
soc_mem_field32_set(l1,L3_DEFIP_AUX_TABLEm,&l164,DB_TYPE1f,l148);l161+= (2*
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m))/2;l162+= (2*soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)+
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m))/2;l142 = soc_mem_write(l1,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l161,&l163);SOC_IF_ERROR_RETURN(l142);l142 = 
soc_mem_write(l1,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l162,&l164);}return l142;}
static int l165(int l1,int l166,void*entry,defip_aux_table_entry_t*l167,int
l168){uint32 l148,l6,l169 = 0;soc_mem_t l17 = L3_DEFIP_PAIR_128m;soc_mem_t
l170 = L3_DEFIP_AUX_TABLEm;int l142 = SOC_E_NONE,l133,vrf;void*l171,*l172;
l171 = (void*)l167;l172 = (void*)(l167+1);SOC_IF_ERROR_RETURN(soc_mem_read(l1
,l170,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_LWR(l166),l167));SOC_IF_ERROR_RETURN(
soc_mem_read(l1,l170,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(l166),l167+1));l148 = 
soc_mem_field32_get(l1,l17,entry,VRF_ID_0_LWRf);soc_mem_field32_set(l1,l170,
l171,VRF0f,l148);l148 = soc_mem_field32_get(l1,l17,entry,VRF_ID_1_LWRf);
soc_mem_field32_set(l1,l170,l171,VRF1f,l148);l148 = soc_mem_field32_get(l1,
l17,entry,VRF_ID_0_UPRf);soc_mem_field32_set(l1,l170,l172,VRF0f,l148);l148 = 
soc_mem_field32_get(l1,l17,entry,VRF_ID_1_UPRf);soc_mem_field32_set(l1,l170,
l172,VRF1f,l148);l148 = soc_mem_field32_get(l1,l17,entry,MODE0_LWRf);
soc_mem_field32_set(l1,l170,l171,MODE0f,l148);l148 = soc_mem_field32_get(l1,
l17,entry,MODE1_LWRf);soc_mem_field32_set(l1,l170,l171,MODE1f,l148);l148 = 
soc_mem_field32_get(l1,l17,entry,MODE0_UPRf);soc_mem_field32_set(l1,l170,l172
,MODE0f,l148);l148 = soc_mem_field32_get(l1,l17,entry,MODE1_UPRf);
soc_mem_field32_set(l1,l170,l172,MODE1f,l148);l148 = soc_mem_field32_get(l1,
l17,entry,VALID0_LWRf);soc_mem_field32_set(l1,l170,l171,VALID0f,l148);l148 = 
soc_mem_field32_get(l1,l17,entry,VALID1_LWRf);soc_mem_field32_set(l1,l170,
l171,VALID1f,l148);l148 = soc_mem_field32_get(l1,l17,entry,VALID0_UPRf);
soc_mem_field32_set(l1,l170,l172,VALID0f,l148);l148 = soc_mem_field32_get(l1,
l17,entry,VALID1_UPRf);soc_mem_field32_set(l1,l170,l172,VALID1f,l148);l142 = 
l140(l1,entry,&l133);SOC_IF_ERROR_RETURN(l142);soc_mem_field32_set(l1,l170,
l171,IP_LENGTH0f,l133);soc_mem_field32_set(l1,l170,l171,IP_LENGTH1f,l133);
soc_mem_field32_set(l1,l170,l172,IP_LENGTH0f,l133);soc_mem_field32_set(l1,
l170,l172,IP_LENGTH1f,l133);l148 = soc_mem_field32_get(l1,l17,entry,
IP_ADDR0_LWRf);soc_mem_field32_set(l1,l170,l171,IP_ADDR0f,l148);l148 = 
soc_mem_field32_get(l1,l17,entry,IP_ADDR1_LWRf);soc_mem_field32_set(l1,l170,
l171,IP_ADDR1f,l148);l148 = soc_mem_field32_get(l1,l17,entry,IP_ADDR0_UPRf);
soc_mem_field32_set(l1,l170,l172,IP_ADDR0f,l148);l148 = soc_mem_field32_get(
l1,l17,entry,IP_ADDR1_UPRf);soc_mem_field32_set(l1,l170,l172,IP_ADDR1f,l148);
l148 = soc_mem_field32_get(l1,l17,entry,ENTRY_TYPE0_LWRf);soc_mem_field32_set
(l1,l170,l171,ENTRY_TYPE0f,l148);l148 = soc_mem_field32_get(l1,l17,entry,
ENTRY_TYPE1_LWRf);soc_mem_field32_set(l1,l170,l171,ENTRY_TYPE1f,l148);l148 = 
soc_mem_field32_get(l1,l17,entry,ENTRY_TYPE0_UPRf);soc_mem_field32_set(l1,
l170,l172,ENTRY_TYPE0f,l148);l148 = soc_mem_field32_get(l1,l17,entry,
ENTRY_TYPE1_UPRf);soc_mem_field32_set(l1,l170,l172,ENTRY_TYPE1f,l148);l142 = 
soc_th_alpm_128_lpm_vrf_get(l1,entry,&vrf,&l133);SOC_IF_ERROR_RETURN(l142);if
(SOC_URPF_STATUS_GET(l1)){if(l168>= (soc_mem_index_count(l1,
L3_DEFIP_PAIR_128m)>>1)){l169 = 1;}}switch(vrf){case SOC_L3_VRF_OVERRIDE:
soc_mem_field32_set(l1,l170,l171,VALID0f,0);soc_mem_field32_set(l1,l170,l171,
VALID1f,0);soc_mem_field32_set(l1,l170,l172,VALID0f,0);soc_mem_field32_set(l1
,l170,l172,VALID1f,0);l6 = 0;break;case SOC_L3_VRF_GLOBAL:l6 = l169?1:0;break
;default:l6 = l169?3:2;break;}soc_mem_field32_set(l1,l170,l171,DB_TYPE0f,l6);
soc_mem_field32_set(l1,l170,l171,DB_TYPE1f,l6);soc_mem_field32_set(l1,l170,
l172,DB_TYPE0f,l6);soc_mem_field32_set(l1,l170,l172,DB_TYPE1f,l6);if(l169){
l148 = soc_mem_field32_get(l1,l17,entry,ALG_BKT_PTRf);if(l148){l148+= 
SOC_TH_ALPM_BUCKET_COUNT(l1);soc_mem_field32_set(l1,l17,entry,ALG_BKT_PTRf,
l148);}l148 = soc_mem_field32_get(l1,l17,entry,ALG_SUB_BKT_PTRf);if(l148){
soc_mem_field32_set(l1,l17,entry,ALG_SUB_BKT_PTRf,l148);}}return SOC_E_NONE;}
static int l173(int l1,int l174,int index,int l175,void*entry){
defip_aux_table_entry_t l167[2];l175 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l175,1);SOC_IF_ERROR_RETURN(l165(l1,l175,entry,&l167[0],
index));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,index,
entry));index = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,index,1);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,
SOC_ALPM_128_ADDR_LWR(index),l167));SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l1,MEM_BLOCK_ANY,SOC_ALPM_128_ADDR_UPR(index),l167+
1));return SOC_E_NONE;}static int l176(int l1,int l27,int l42,int bktid){int
l142 = SOC_E_NONE;int l177;soc_mem_t l178 = L3_DEFIP_PAIR_128m;
defip_pair_128_entry_t lpm_entry;int l179,l180;l179 = ALPM_BKT_IDX(bktid);
l180 = ALPM_BKT_SIDX(bktid);l177 = soc_th_alpm_logical_idx(l1,l178,
SOC_ALPM_128_DEFIP_TO_PAIR(l42>>1),1);l142 = soc_mem_read(l1,l178,
MEM_BLOCK_ANY,l177,&lpm_entry);SOC_IF_ERROR_RETURN(l142);soc_mem_field32_set(
l1,l178,&lpm_entry,ALG_BKT_PTRf,l179);soc_mem_field32_set(l1,l178,&lpm_entry,
ALG_SUB_BKT_PTRf,l180);l142 = soc_mem_write(l1,l178,MEM_BLOCK_ANY,l177,&
lpm_entry);SOC_IF_ERROR_RETURN(l142);if(SOC_URPF_STATUS_GET(l1)){int l181 = (
soc_mem_index_count(l1,l178)>>1)+soc_th_alpm_logical_idx(l1,l178,
SOC_ALPM_128_DEFIP_TO_PAIR(l42>>1),1);l142 = READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l181,&lpm_entry);SOC_IF_ERROR_RETURN(l142);soc_mem_field32_set(
l1,l178,&lpm_entry,ALG_BKT_PTRf,l179+SOC_TH_ALPM_BUCKET_COUNT(l1));
soc_mem_field32_set(l1,l178,&lpm_entry,ALG_SUB_BKT_PTRf,l180);l142 = 
WRITE_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l181,&lpm_entry);
SOC_IF_ERROR_RETURN(l142);}return l142;}int _soc_th_alpm_128_move_trie(int l1
,int l27,int l182,int l183){int*l36 = NULL,*l136 = NULL;int l142 = SOC_E_NONE
,l184,l141,l185;int l134,l42;uint32 l156 = 0;soc_mem_t l17;void*l186,*l187;
uint32 l10[SOC_MAX_MEM_FIELD_WORDS];trie_t*trie = NULL;payload_t*l133 = NULL;
alpm_mem_prefix_array_t*l35 = NULL;defip_alpm_ipv6_128_entry_t l188,l189;int
l190,l191,l192,l193;l185 = sizeof(int)*MAX_PREFIX_PER_BUCKET;l36 = sal_alloc(
l185,"new_index_move");l136 = sal_alloc(l185,"old_index_move");l35 = 
sal_alloc(sizeof(alpm_mem_prefix_array_t),"prefix_array");if(l36 == NULL||
l136 == NULL||l35 == NULL){l142 = SOC_E_MEMORY;goto l194;}l190 = ALPM_BKT_IDX
(l182);l192 = ALPM_BKT_SIDX(l182);l191 = ALPM_BKT_IDX(l183);l193 = 
ALPM_BKT_SIDX(l183);l17 = L3_DEFIP_ALPM_IPV6_128m;l186 = &l188;l187 = &l189;
l42 = SOC_ALPM_BS_BKT_USAGE_PIVOT(l1,l182);trie = PIVOT_BUCKET_TRIE(
ALPM_TCAM_PIVOT(l1,l42));soc_th_alpm_bank_db_type_get(l1,
SOC_ALPM_BS_BKT_USAGE_VRF(l1,l182),&l156,NULL);sal_memset(l35,0,sizeof(*l35))
;l142 = trie_traverse(trie,_soc_th_alpm_mem_prefix_array_cb,l35,
_TRIE_INORDER_TRAVERSE);do{if((l142)<0){goto l194;}}while(0);sal_memset(l36,-
1,l185);sal_memset(l136,-1,l185);for(l141 = 0;l141<l35->count;l141++){l133 = 
l35->prefix[l141];if(l133->index>0){l142 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,
l133->index,l186);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"soc_mem_read index %d failed\n"),l133->index));goto l195;}
soc_mem_field32_set(l1,l17,l186,SUB_BKT_PTRf,l193);if(SOC_URPF_STATUS_GET(l1)
){l142 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l133->
index),l187);if(SOC_FAILURE(l142)){goto l195;}soc_mem_field32_set(l1,l17,l187
,SUB_BKT_PTRf,l193);}l142 = _soc_th_alpm_insert_in_bkt(l1,l17,l183,l156,l186,
l10,&l134,l27);if(SOC_SUCCESS(l142)){if(SOC_URPF_STATUS_GET(l1)){l142 = 
soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l134),l187);if(
SOC_FAILURE(l142)){goto l195;}}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1
,"insert to bucket %d failed\n"),l183));goto l195;}l36[l141] = l134;l136[l141
] = l133->index;}}l142 = l176(l1,l27,l42,l183);if(SOC_FAILURE(l142)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_move_relink failed, pivot %d bkt %d\n"),l42,l183));goto l195
;}l142 = _soc_th_alpm_128_move_inval(l1,l17,l35,l36);if(SOC_FAILURE(l142)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_move_inval failed\n")));goto l195;}l142 = 
soc_th_alpm_update_hit_bits(l1,l35->count,l136,l36);if(SOC_FAILURE(l142)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l35->count));l142 = 
SOC_E_NONE;}PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l1,l42)) = ALPM_BKTID(l191,
l193);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l1,l27,l190);
SOC_ALPM_BS_BKT_USAGE_SB_DEL(l1,l190,l192,TRUE,l35->count);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l1,l27,l190);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l1,l27,l191);SOC_ALPM_BS_BKT_USAGE_SB_ADD(l1
,l191,l193,l42,-1,l35->count);SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l1,l27,l191);
l195:if(SOC_FAILURE(l142)){l184 = l176(l1,l27,l42,l182);do{if((l184)<0){goto
l194;}}while(0);for(l141 = 0;l141<l35->count;l141++){if(l36[l141] == -1){
continue;}l184 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,l36[l141],
soc_mem_entry_null(l1,l17));do{if((l184)<0){goto l194;}}while(0);if(
SOC_URPF_STATUS_GET(l1)){l184 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l36[l141]),soc_mem_entry_null(l1,l17));do{if((l184)
<0){goto l194;}}while(0);}}}l194:if(l36!= NULL){sal_free(l36);}if(l136!= NULL
){sal_free(l136);}if(l35!= NULL){sal_free(l35);}return l142;}static void l196
(int l1,defip_pair_128_entry_t*l197,void*key_data,int tcam_index,alpm_pivot_t
*pivot_pyld){int l142;trie_t*l153 = NULL;int l27,vrf,vrf_id;trie_node_t*l198 = 
NULL;l27 = L3_DEFIP_MODE_128;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,&
vrf_id,&vrf);l142 = soc_th_alpm_128_lpm_delete(l1,l197);if(SOC_FAILURE(l142))
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Failure to free new prefix""at %d\n"),
soc_th_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,tcam_index,l27)));}l153 = 
VRF_PIVOT_TRIE_IPV6(l1,vrf);if(ALPM_TCAM_PIVOT(l1,tcam_index<<(l27?1:0))!= 
NULL){l142 = trie_delete(l153,pivot_pyld->key,pivot_pyld->len,&l198);if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: trie delete failure ""in bkt move rollback\n")));}}
ALPM_TCAM_PIVOT(l1,tcam_index<<(l27?1:0)) = NULL;VRF_PIVOT_REF_DEC(l1,vrf,l27
);}static int _soc_th_alpm_128_move_inval(int l1,soc_mem_t l17,
alpm_mem_prefix_array_t*l35,int*l36){int l141,l142 = SOC_E_NONE,l184;
defip_alpm_ipv6_128_entry_t l188;void*l199 = NULL,*l200 = NULL;int l201;int*
l202 = NULL;int l203 = FALSE;l201 = sizeof(l188);l199 = sal_alloc(l201*l35->
count,"rb_bufp");l200 = sal_alloc(l201*l35->count,"rb_sip_bufp");l202 = 
sal_alloc(sizeof(*l202)*l35->count,"roll_back_index");if(l199 == NULL||l200 == 
NULL||l202 == NULL){l142 = SOC_E_MEMORY;goto l204;}sal_memset(l202,-1,sizeof(
*l202)*l35->count);for(l141 = 0;l141<l35->count;l141++){payload_t*prefix = 
l35->prefix[l141];if(prefix->index>= 0){l142 = soc_mem_read(l1,l17,
MEM_BLOCK_ANY,prefix->index,(uint8*)l199+l141*l201);if(SOC_FAILURE(l142)){
l141--;l203 = TRUE;break;}if(SOC_URPF_STATUS_GET(l1)){l142 = soc_mem_read(l1,
l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,prefix->index),(uint8*)l200+l141*
l201);if(SOC_FAILURE(l142)){l141--;l203 = TRUE;break;}}l142 = soc_mem_write(
l1,l17,MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null(l1,l17));if(SOC_FAILURE
(l142)){l202[l141] = prefix->index;l203 = TRUE;break;}if(SOC_URPF_STATUS_GET(
l1)){l142 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,
prefix->index),soc_mem_entry_null(l1,l17));if(SOC_FAILURE(l142)){l202[l141] = 
prefix->index;l203 = TRUE;break;}}}l202[l141] = prefix->index;prefix->index = 
l36[l141];}if(l203){for(;l141>= 0;l141--){payload_t*prefix = l35->prefix[l141
];prefix->index = l202[l141];if(l202[l141]<0){continue;}l184 = soc_mem_write(
l1,l17,MEM_BLOCK_ALL,l202[l141],(uint8*)l199+l141*l201);if(SOC_FAILURE(l184))
{break;}if(!SOC_URPF_STATUS_GET(l1)){continue;}l184 = soc_mem_write(l1,l17,
MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(l1,l202[l141]),(uint8*)l200+l141*l201);
if(SOC_FAILURE(l184)){break;}}}l204:if(l202!= NULL){sal_free(l202);}if(l200!= 
NULL){sal_free(l200);}if(l199!= NULL){sal_free(l199);}return l142;}static int
l28(int l1,alpm_pfx_info_t*l29,trie_t*l30,uint32*l31,uint32 l32,trie_node_t*
l33,defip_pair_128_entry_t*lpm_entry,uint32*l34){trie_node_t*l154 = NULL;int
l27;defip_alpm_ipv6_128_entry_t l188;payload_t*l205 = NULL;int l206;void*l186
;alpm_pivot_t*l207;int l142 = SOC_E_NONE;soc_mem_t l17;alpm_bucket_handle_t*
l208;l207 = l29->pivot_pyld;l206 = l207->tcam_index;l27 = L3_DEFIP_MODE_128;
l17 = L3_DEFIP_ALPM_IPV6_128m;l186 = ((uint32*)&(l188));l154 = NULL;l142 = 
trie_find_lpm(l30,l31,l32,&l154);l205 = (payload_t*)l154;if(SOC_FAILURE(l142)
){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l1,l31[0],l31[1],l31[2],
l31[3],l31[4],l32));return l142;}if(l205->bkt_ptr){if(l205->bkt_ptr == l29->
new_pfx_pyld){sal_memcpy(l186,l29->alpm_data,sizeof(
defip_alpm_ipv6_128_entry_t));}else{l142 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,
((payload_t*)l205->bkt_ptr)->index,l186);}if(SOC_FAILURE(l142)){return l142;}
l142 = l21(l1,l186,l17,l27,l29->vrf_id,l29->bktid,((payload_t*)l205->bkt_ptr)
->index,lpm_entry);if(SOC_FAILURE(l142)){return l142;}*l34 = ((payload_t*)(
l205->bkt_ptr))->len;}else{l142 = soc_mem_read(l1,L3_DEFIP_PAIR_128m,
MEM_BLOCK_ANY,soc_th_alpm_logical_idx(l1,L3_DEFIP_PAIR_128m,
SOC_ALPM_128_DEFIP_TO_PAIR(l206>>1),1),lpm_entry);}l208 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l208 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Unable to allocate memory "
"for PIVOT trie node \n")));l142 = SOC_E_MEMORY;return l142;}sal_memset(l208,
0,sizeof(*l208));l207 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for new Pivot");if(l207 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Unable to allocate memory "
"for PIVOT trie node \n")));l142 = SOC_E_MEMORY;return l142;}sal_memset(l207,
0,sizeof(*l207));PIVOT_BUCKET_HANDLE(l207) = l208;l142 = trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l207));PIVOT_BUCKET_TRIE(l207)->trie = 
l33;PIVOT_BUCKET_INDEX(l207) = l29->bktid;PIVOT_BUCKET_VRF(l207) = l29->vrf;
PIVOT_BUCKET_IPV6(l207) = l27;PIVOT_BUCKET_DEF(l207) = FALSE;(l207)->key[0] = 
l31[0];(l207)->key[1] = l31[1];(l207)->key[2] = l31[2];(l207)->key[3] = l31[3
];(l207)->key[4] = l31[4];(l207)->len = l32;l145((l31),(l32),(l27));l23(l1,
l31,l32,l29->vrf,l27,lpm_entry,0,0);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(l1)]->l94),(
ALPM_BKT_IDX(l29->bktid)));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(l1)]->l95),(ALPM_BKT_SIDX(l29->
bktid)));l29->pivot_pyld = l207;return l142;}static int l209(int l1,
alpm_pfx_info_t*l29,int*l160,int*l11){trie_node_t*l33;uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];int l27;uint32 l32,l34 = 0;uint32 l156 = 0;uint32 l31
[5];int l134;defip_pair_128_entry_t l188,l189;trie_t*l30,*trie;void*l186,*
l187;alpm_pivot_t*l210 = l29->pivot_pyld;defip_pair_128_entry_t lpm_entry;
soc_mem_t l17;trie_t*l153 = NULL;alpm_mem_prefix_array_t l211;int*l36 = NULL;
int*l136 = NULL;int l142 = SOC_E_NONE,l141,l41 = -1;int tcam_index,l212,l213 = 
0;int l214 = 0,l215 = 0;l27 = L3_DEFIP_MODE_128;l17 = L3_DEFIP_ALPM_IPV6_128m
;l186 = ((uint32*)&(l188));l187 = ((uint32*)&(l189));
soc_th_alpm_bank_db_type_get(l1,l29->vrf,&l156,NULL);l30 = 
VRF_PREFIX_TRIE_IPV6_128(l1,l29->vrf);trie = PIVOT_BUCKET_TRIE(l29->
pivot_pyld);l213 = l29->bktid;l212 = PIVOT_TCAM_INDEX(l210);l142 = 
soc_th_alpm_bs_alloc(l1,&l29->bktid,l29->vrf,l27);if(SOC_FAILURE(l142)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to allocate""new bucket for split\n")));l29->bktid = 
-1;_soc_th_alpm_free_pfx_trie(l1,l30,trie,l29->new_pfx_pyld,l36,l29->bktid,
l29->vrf,l27);return l142;}l141 = _soc_th_alpm_bkt_entry_cnt(l1,l27);l141-= 
SOC_ALPM_BS_BKT_USAGE_COUNT(l1,ALPM_BKT_IDX(l29->bktid));l142 = trie_split(
trie,_MAX_KEY_LEN_144_,FALSE,l31,&l32,&l33,NULL,FALSE,l141);if(SOC_FAILURE(
l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not split bucket")));_soc_th_alpm_free_pfx_trie(l1,
l30,trie,l29->new_pfx_pyld,l36,l29->bktid,l29->vrf,l27);return l142;}l142 = 
l28(l1,l29,l30,l31,l32,l33,&lpm_entry,&l34);if(l142!= SOC_E_NONE){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"could not initialize pivot")));
_soc_th_alpm_free_pfx_trie(l1,l30,trie,l29->new_pfx_pyld,l36,l29->bktid,l29->
vrf,l27);return l142;}sal_memset(&l211,0,sizeof(l211));l142 = trie_traverse(
PIVOT_BUCKET_TRIE(l29->pivot_pyld),_soc_th_alpm_mem_prefix_array_cb,&l211,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_insert: Bucket split failed")));
_soc_th_alpm_free_pfx_trie(l1,l30,trie,l29->new_pfx_pyld,l36,l29->bktid,l29->
vrf,l27);return l142;}l36 = sal_alloc(sizeof(*l36)*l211.count,"new_index");if
(l36 == NULL){l142 = SOC_E_MEMORY;_soc_th_alpm_free_pfx_trie(l1,l30,trie,l29
->new_pfx_pyld,l36,l29->bktid,l29->vrf,l27);return l142;}l136 = sal_alloc(
sizeof(*l136)*l211.count,"old_index");if(l136 == NULL){l142 = SOC_E_MEMORY;
_soc_th_alpm_free_pfx_trie(l1,l30,trie,l29->new_pfx_pyld,l136,l29->bktid,l29
->vrf,l27);return l142;}sal_memset(l36,-1,sizeof(*l36)*l211.count);sal_memset
(l136,-1,sizeof(*l136)*l211.count);for(l141 = 0;l141<l211.count;l141++){
payload_t*l133 = l211.prefix[l141];if(l133->index>0){l142 = soc_mem_read(l1,
l17,MEM_BLOCK_ANY,l133->index,l186);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failed to ""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l211.prefix[l141]->key[1],l211.
prefix[l141]->key[2],l211.prefix[l141]->key[3],l211.prefix[l141]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l29->key_data,l17,
l210,l29->pivot_pyld,&l211,l36,l41);sal_free(l136);return l142;}if(
SOC_URPF_STATUS_GET(l1)){l142 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l133->index),l187);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Failed to"
"read rpf prefix ""0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l211.prefix[l141]
->key[1],l211.prefix[l141]->key[2],l211.prefix[l141]->key[3],l211.prefix[l141
]->key[4]));(void)_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l29->
key_data,l17,l210,l29->pivot_pyld,&l211,l36,l41);sal_free(l136);return l142;}
}soc_mem_field32_set(l1,l17,l186,SUB_BKT_PTRf,ALPM_BKT_SIDX(l29->bktid));
soc_mem_field32_set(l1,l17,l187,SUB_BKT_PTRf,ALPM_BKT_SIDX(l29->bktid));l142 = 
_soc_th_alpm_insert_in_bkt(l1,l17,l29->bktid,l156,l186,l10,&l134,l27);if(
SOC_SUCCESS(l142)){if(SOC_URPF_STATUS_GET(l1)){l142 = soc_mem_write(l1,l17,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l134),l187);}l214++;l215++;}}else{
soc_mem_field32_set(l1,l17,l29->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(l29->
bktid));l142 = _soc_th_alpm_insert_in_bkt(l1,l17,l29->bktid,l156,l29->
alpm_data,l10,&l134,l27);if(SOC_SUCCESS(l142)){l41 = l141;*l11 = l134;if(
SOC_URPF_STATUS_GET(l1)){soc_mem_field32_set(l1,l17,l29->alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l29->bktid));l142 = soc_mem_write(l1,l17,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l134),l29->alpm_sip_data);}l215++;}}
l36[l141] = l134;if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l211.prefix[l141]->key[1],l211.
prefix[l141]->key[2],l211.prefix[l141]->key[3],l211.prefix[l141]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l29->key_data,l17,
l210,l29->pivot_pyld,&l211,l36,l41);sal_free(l136);return l142;}l136[l141] = 
l133->index;}l142 = l2(l1,&lpm_entry,l160);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_alpm_insert: Unable to add new"
"pivot to tcam\n")));if(l142 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l1,l29->vrf,
l27);}(void)_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l29->key_data
,l17,l210,l29->pivot_pyld,&l211,l36,l41);sal_free(l136);return l142;}*l160 = 
soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,*l160,l27);l142 = l159(l1,*
l160,l34);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l160));l196(l1,
&lpm_entry,l29->key_data,*l160,l29->pivot_pyld);(void)
_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l29->key_data,l17,l210,
l29->pivot_pyld,&l211,l36,l41);sal_free(l136);return l142;}l153 = 
VRF_PIVOT_TRIE_IPV6_128(l1,l29->vrf);l142 = trie_insert(l153,(l29->pivot_pyld
)->key,NULL,(l29->pivot_pyld)->len,(trie_node_t*)l29->pivot_pyld);if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"failed to insert into pivot trie\n")));(void)_soc_th_alpm_rollback_bkt_move(
l1,L3_DEFIP_MODE_128,l29->key_data,l17,l210,l29->pivot_pyld,&l211,l36,l41);
sal_free(l136);return l142;}tcam_index = SOC_ALPM_128_ADDR_LWR(*l160)<<1;
ALPM_TCAM_PIVOT(l1,tcam_index) = l29->pivot_pyld;PIVOT_TCAM_INDEX(l29->
pivot_pyld) = tcam_index;VRF_PIVOT_REF_INC(l1,l29->vrf,l27);l142 = 
_soc_th_alpm_128_move_inval(l1,l17,&l211,l36);if(SOC_FAILURE(l142)){l196(l1,&
lpm_entry,l29->key_data,*l160,l29->pivot_pyld);(void)
_soc_th_alpm_rollback_bkt_move(l1,L3_DEFIP_MODE_128,l29->key_data,l17,l210,
l29->pivot_pyld,&l211,l36,l41);sal_free(l36);l36 = NULL;sal_free(l136);return
l142;}l142 = soc_th_alpm_update_hit_bits(l1,l211.count,l136,l36);if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l211.count));l142 = 
SOC_E_NONE;}sal_free(l36);l36 = NULL;sal_free(l136);if(l41 == -1){
soc_mem_field32_set(l1,l17,l29->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(
PIVOT_BUCKET_INDEX(l210)));l142 = _soc_th_alpm_insert_in_bkt(l1,l17,
PIVOT_BUCKET_INDEX(l210),l156,l29->alpm_data,l10,&l134,l27);if(SOC_FAILURE(
l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n")));
_soc_th_alpm_free_pfx_trie(l1,l30,trie,l29->new_pfx_pyld,l36,l29->bktid,l29->
vrf,l27);return l142;}l214--;if(SOC_URPF_STATUS_GET(l1)){l142 = soc_mem_write
(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l134),l29->alpm_sip_data);}*
l11 = l134;l29->new_pfx_pyld->index = l134;}soc_th_alpm_bu_upd(l1,l213,l212,
FALSE,l27,-l214);soc_th_alpm_bu_upd(l1,l29->bktid,tcam_index,l29->vrf,l27,
l215);PIVOT_BUCKET_ENT_CNT_UPDATE(l29->pivot_pyld);VRF_BUCKET_SPLIT_INC(l1,
l29->vrf,l27);return l142;}static int l216(int l1,void*key_data,soc_mem_t l17
,void*alpm_data,void*alpm_sip_data,int*l11,int bktid,int tcam_index){
alpm_pivot_t*pivot_pyld,*l210;defip_aux_scratch_entry_t l8;uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l32;int l27,vrf,vrf_id;int l134;int
l142 = SOC_E_NONE,l184;uint32 l6,l156;int l152 = 0;int l160;int l217 = 0;
trie_t*trie,*l30;trie_node_t*l154 = NULL;payload_t*l218,*l219,*l205;int l19 = 
0;int*l36 = NULL;alpm_pfx_info_t l29;l27 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&vrf_id,&vrf));
soc_th_alpm_bank_db_type_get(l1,vrf,&l156,&l6);l17 = L3_DEFIP_ALPM_IPV6_128m;
l142 = _soc_th_alpm_128_prefix_create(l1,key_data,prefix,&l32,&l19);if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_insert: prefix create failed\n")));return l142;}sal_memset(&l8
,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l4(l1,key_data,l27,
l6,0,&l8));if(bktid == 0){l142 = l151(l1,prefix,l32,l27,vrf,&l152,&tcam_index
,&bktid);SOC_IF_ERROR_RETURN(l142);soc_mem_field32_set(l1,l17,alpm_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));}l142 = _soc_th_alpm_insert_in_bkt(l1,l17,
bktid,l156,alpm_data,l10,&l134,l27);if(l142 == SOC_E_NONE){*l11 = l134;if(
SOC_URPF_STATUS_GET(l1)){soc_mem_field32_set(l1,l17,alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));l184 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l1,l134),alpm_sip_data);if(SOC_FAILURE(l184)){return
l184;}}}if(l142 == SOC_E_FULL){l152 = _soc_th_alpm_bkt_entry_cnt(l1,
L3_DEFIP_MODE_128);if(l152>4){l152 = 4;}if(PIVOT_BUCKET_COUNT(ALPM_TCAM_PIVOT
(l1,tcam_index))<l152){int l220;l142 = soc_th_alpm_bs_alloc(l1,&l220,vrf,l27)
;SOC_IF_ERROR_RETURN(l142);l142 = _soc_th_alpm_128_move_trie(l1,l27,bktid,
l220);SOC_IF_ERROR_RETURN(l142);bktid = l220;soc_mem_field32_set(l1,l17,
alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));l142 = 
_soc_th_alpm_insert_in_bkt(l1,l17,bktid,l156,alpm_data,l10,&l134,l27);if(
SOC_SUCCESS(l142)){*l11 = l134;if(SOC_URPF_STATUS_GET(l1)){
soc_mem_field32_set(l1,l17,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));
l184 = soc_mem_write(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,l134),
alpm_sip_data);if(SOC_FAILURE(l184)){return l184;}}}else{return l142;}}else{
l217 = 1;}}pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l210 = pivot_pyld;l218 = sal_alloc(sizeof(
payload_t),"Payload for 128b Key");if(l218 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"trie node \n")));return SOC_E_MEMORY;}l219 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie 128b key");if(l219 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_alpm_128_insert: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l218);return SOC_E_MEMORY;}sal_memset(l218,0,
sizeof(*l218));sal_memset(l219,0,sizeof(*l219));l218->key[0] = prefix[0];l218
->key[1] = prefix[1];l218->key[2] = prefix[2];l218->key[3] = prefix[3];l218->
key[4] = prefix[4];l218->len = l32;l218->index = l134;sal_memcpy(l219,l218,
sizeof(*l218));l219->bkt_ptr = l218;l142 = trie_insert(trie,prefix,NULL,l32,(
trie_node_t*)l218);if(SOC_FAILURE(l142)){if(l218!= NULL){sal_free(l218);}if(
l219!= NULL){sal_free(l219);}return l142;}l30 = VRF_PREFIX_TRIE_IPV6_128(l1,
vrf);if(!l19){l142 = trie_insert(l30,prefix,NULL,l32,(trie_node_t*)l219);}
else{l154 = NULL;l142 = trie_find_lpm(l30,0,0,&l154);l205 = (payload_t*)l154;
if(SOC_SUCCESS(l142)){l205->bkt_ptr = l218;}}if(SOC_FAILURE(l142)){
_soc_th_alpm_free_pfx_trie(l1,l30,trie,l219,l36,bktid,vrf,l27);return l142;}
if(l217){l29.key_data = key_data;l29.new_pfx_pyld = l218;l29.pivot_pyld = 
pivot_pyld;l29.alpm_data = alpm_data;l29.alpm_sip_data = alpm_sip_data;l29.
bktid = bktid;l29.vrf_id = vrf_id;l29.vrf = vrf;l142 = l209(l1,&l29,&l160,l11
);if(l142!= SOC_E_NONE){return l142;}bktid = l29.bktid;tcam_index = 
PIVOT_TCAM_INDEX(l29.pivot_pyld);alpm_128_split_count++;}else{
soc_th_alpm_bu_upd(l1,bktid,tcam_index,vrf,l27,1);}VRF_TRIE_ROUTES_INC(l1,vrf
,l27);if(l19){sal_free(l219);}soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&
l8,ALG_HIT_IDXf,*l11);SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
INSERT_PROPAGATE,&l8,TRUE,&l152,&tcam_index,&bktid));if(SOC_URPF_STATUS_GET(
l1)){uint32 l221[4] = {0,0,0,0};l32 = soc_mem_field32_get(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf);l32+= 1;soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf,l32);if(l19){soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,1);}else{soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,0);}soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,ALG_HIT_IDXf,_soc_th_alpm_rpf_entry(l1,*l11));
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,INSERT_PROPAGATE,&l8,TRUE,&l152,&
tcam_index,&bktid));if(!l19){soc_mem_field_set(l1,L3_DEFIP_AUX_SCRATCHm,(
uint32*)&l8,IP_ADDRf,(uint32*)l221);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,IP_LENGTHf,0);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,REPLACE_LENf,0);soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,RPEf,1);SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l1,
INSERT_PROPAGATE,&l8,TRUE,&l152,&tcam_index,&bktid));}}
PIVOT_BUCKET_ENT_CNT_UPDATE(l210);return l142;}static int l23(int unit,uint32
*key,int len,int vrf,int l5,defip_pair_128_entry_t*lpm_entry,int l24,int l25)
{uint32 l148;if(l25){sal_memset(lpm_entry,0,sizeof(defip_pair_128_entry_t));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l86),(vrf&SOC_VRF_MAX(unit)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l87),(vrf&SOC_VRF_MAX(unit)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l84),(vrf&SOC_VRF_MAX(unit)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l85),(vrf&SOC_VRF_MAX(unit)));if(vrf == (
SOC_VRF_MAX(unit)+1)){l148 = 0;}else{l148 = SOC_VRF_MAX(unit);}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l90),(l148));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l91),(l148)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l109[(unit)]->l88),(l148));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l89),(l148)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l109[(unit)]->l63),(key[0]));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l64),(key[
1]));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m
)),(lpm_entry),(l109[(unit)]->l61),(key[2]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l62),(key[3]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l71),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l72),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l69),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l70),(3));l149(unit,(void*)lpm_entry,len);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l82),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l83),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l80),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l81),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l75),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l76),((1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l73),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l74),((1<<
soc_mem_field_length(unit,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l99),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l100),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_LWRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l101),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l102),((1<<soc_mem_field_length(unit,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_UPRf))-1));return(SOC_E_NONE);}static int
l222(int l1,void*key_data,int vrf_id,int vrf,int bktid,int tcam_index,int l134
){alpm_pivot_t*pivot_pyld;defip_alpm_ipv6_128_entry_t l188,l223,l189;
defip_aux_scratch_entry_t l8;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l17
;void*l186,*l224,*l187 = NULL;int l142 = SOC_E_NONE,l184 = SOC_E_NONE;uint32
l225[5],prefix[5];int l27;uint32 l32;int l226;uint32 l6,l156;int l152,l19 = 0
;trie_t*trie,*l30;uint32 l227;defip_pair_128_entry_t lpm_entry,*l228;
payload_t*l218 = NULL,*l229 = NULL,*l205 = NULL;trie_node_t*l198 = NULL,*l154
= NULL;trie_t*l153 = NULL;l27 = L3_DEFIP_MODE_128;if(!(((vrf_id == 
SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)
&&(vrf_id == SOC_L3_VRF_GLOBAL))))){soc_th_alpm_bank_db_type_get(l1,vrf,&l156
,&l6);l142 = _soc_th_alpm_128_prefix_create(l1,key_data,prefix,&l32,&l19);if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_alpm_128_delete: prefix create failed\n")));return l142;}if(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_COMBINED){if(vrf_id!= 
SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l1,vrf,l27)>1){if(l19){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"VRF %d: Cannot v6-128 delete "
"default route if other routes are present ""in this mode"),vrf));return
SOC_E_PARAM;}}}l6 = 2;}l17 = L3_DEFIP_ALPM_IPV6_128m;l186 = ((uint32*)&(l188)
);SOC_ALPM_LPM_LOCK(l1);if(bktid == 0){l142 = _soc_th_alpm_128_find(l1,l17,
key_data,vrf_id,vrf,l186,&tcam_index,&bktid,&l134,TRUE);}else{l142 = l14(l1,
key_data,l186,0,l17,0,0,bktid);}sal_memcpy(&l223,l186,sizeof(l223));l224 = &
l223;if(SOC_FAILURE(l142)){SOC_ALPM_LPM_UNLOCK(l1);LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l1,"_soc_alpm_128_delete: Unable to find ""prefix for delete\n"))
);return l142;}l226 = bktid;pivot_pyld = ALPM_TCAM_PIVOT(l1,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l142 = trie_delete(trie,prefix,l32,&l198);l218 = 
(payload_t*)l198;if(l142!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"_soc_alpm_128_delete: Error prefix not ""present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l1);return l142;}l30 = VRF_PREFIX_TRIE_IPV6_128(l1,vrf);
l153 = VRF_PIVOT_TRIE_IPV6_128(l1,vrf);if(!l19){l142 = trie_delete(l30,prefix
,l32,&l198);l229 = (payload_t*)l198;if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Prefix not present "
"in pfx trie: 0x%08x 0x%08x 0x%08x 0x%08x\n"),prefix[1],prefix[2],prefix[3],
prefix[4]));goto l230;}l154 = NULL;l142 = trie_find_lpm(l30,prefix,l32,&l154)
;l205 = (payload_t*)l154;if(SOC_SUCCESS(l142)){payload_t*l231 = (payload_t*)(
l205->bkt_ptr);if(l231!= NULL){l227 = l231->len;}else{l227 = 0;}}else{
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find"
" replacement bpm for prefix: 0x%08x 0x%08x ""0x%08x 0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l232;}sal_memcpy(l225,prefix,sizeof(
prefix));l145((l225),(l32),(l27));l142 = l23(l1,l225,l227,vrf,l27,&lpm_entry,
0,1);l184 = _soc_th_alpm_128_find(l1,l17,&lpm_entry,vrf_id,vrf,l186,&
tcam_index,&bktid,&l134,TRUE);(void)l21(l1,l186,l17,l27,vrf_id,bktid,0,&
lpm_entry);(void)l23(l1,l225,l32,vrf,l27,&lpm_entry,0,0);if(
SOC_URPF_STATUS_GET(l1)){if(SOC_SUCCESS(l142)){l187 = ((uint32*)&(l189));l184
= soc_mem_read(l1,l17,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l1,ALPM_ENT_INDEX(
l134)),l187);}}if((l227 == 0)&&SOC_FAILURE(l184)){l228 = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf);sal_memcpy(&lpm_entry,l228,sizeof(
lpm_entry));l142 = l23(l1,prefix,l227,vrf,l27,&lpm_entry,0,1);}if(SOC_FAILURE
(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"replacement prefix for prefix: 0x%08x 0x%08x 0x%08x ""0x%08x\n"),prefix[1],
prefix[2],prefix[3],prefix[4]));goto l232;}l228 = &lpm_entry;}else{l154 = 
NULL;l142 = trie_find_lpm(l30,prefix,l32,&l154);l205 = (payload_t*)l154;if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Could not find "
"default route in the trie for vrf %d\n"),vrf));goto l230;}l205->bkt_ptr = 0;
l227 = 0;l228 = VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf);}l142 = l4(l1,l228,
l27,l6,l227,&l8);if(SOC_FAILURE(l142)){goto l232;}soc_mem_field32_set(l1,
L3_DEFIP_AUX_SCRATCHm,&l8,ALG_HIT_IDXf,ALPM_ENT_INDEX(l134));l142 = 
_soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&l8,TRUE,&l152,&tcam_index,&bktid);if
(SOC_FAILURE(l142)){goto l232;}if(SOC_URPF_STATUS_GET(l1)){uint32 l148;if(
l187!= NULL){l148 = soc_mem_field32_get(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf
);l148++;soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,DB_TYPEf,l148);
soc_mem_field32_set(l1,L3_DEFIP_AUX_SCRATCHm,&l8,ALG_HIT_IDXf,
_soc_th_alpm_rpf_entry(l1,ALPM_ENT_INDEX(l134)));l148 = soc_mem_field32_get(
l1,l17,l187,SRC_DISCARDf);soc_mem_field32_set(l1,l17,&l8,SRC_DISCARDf,l148);
l148 = soc_mem_field32_get(l1,l17,l187,DEFAULTROUTEf);soc_mem_field32_set(l1,
l17,&l8,DEFAULTROUTEf,l148);l142 = _soc_th_alpm_aux_op(l1,DELETE_PROPAGATE,&
l8,TRUE,&l152,&tcam_index,&bktid);}if(SOC_FAILURE(l142)){goto l232;}}sal_free
(l218);if(!l19){sal_free(l229);}PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);if((
pivot_pyld->len!= 0)&&(trie->trie == NULL)){uint32 l233[5];sal_memcpy(l233,
pivot_pyld->key,sizeof(l233));l145((l233),(pivot_pyld->len),(l27));l23(l1,
l233,pivot_pyld->len,vrf,l27,&lpm_entry,0,1);l142 = 
soc_th_alpm_128_lpm_delete(l1,&lpm_entry);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to "
"delete pivot 0x%08x 0x%08x 0x%08x 0x%08x \n"),pivot_pyld->key[1],pivot_pyld
->key[2],pivot_pyld->key[3],pivot_pyld->key[4]));}}l142 = 
_soc_th_alpm_delete_in_bkt(l1,l17,l226,l156,l224,l10,&l134,l27);if(
SOC_FAILURE(l142)){SOC_ALPM_LPM_UNLOCK(l1);return l142;}if(
SOC_URPF_STATUS_GET(l1)){l142 = soc_mem_alpm_delete(l1,l17,
SOC_TH_ALPM_RPF_BKT_IDX(l1,ALPM_BKT_IDX(l226)),MEM_BLOCK_ALL,l156,l224,l10,&
l152);if(SOC_FAILURE(l142)){SOC_ALPM_LPM_UNLOCK(l1);return l142;}}
soc_th_alpm_bu_upd(l1,PIVOT_BUCKET_INDEX(pivot_pyld),PIVOT_TCAM_INDEX(
pivot_pyld),((pivot_pyld->len!= 0)&&(trie->trie == NULL))?TRUE:FALSE,l27,-1);
if((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l142 = soc_th_alpm_bs_free(l1
,PIVOT_BUCKET_INDEX(pivot_pyld),vrf,l27);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to release"
"empty bucket: %d\n"),PIVOT_BUCKET_INDEX(pivot_pyld)));}l142 = trie_delete(
l153,pivot_pyld->key,pivot_pyld->len,&l198);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"could not delete pivot from pivot trie\n")));
}trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(
pivot_pyld));sal_free(pivot_pyld);_soc_tomahawk_alpm_bkt_view_set(l1,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,ALPM_BKT_IDX(l226)),INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l1,l27)){int l234 = ALPM_BKT_IDX(l226)+1;
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,l234),
INVALIDm);}}}VRF_TRIE_ROUTES_DEC(l1,vrf,l27);if(VRF_TRIE_ROUTES_CNT(l1,vrf,
l27) == 0){l142 = l26(l1,vrf,l27);}SOC_ALPM_LPM_UNLOCK(l1);return l142;l232:
l184 = trie_insert(l30,prefix,NULL,l32,(trie_node_t*)l229);if(SOC_FAILURE(
l184)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}l230:l184 = trie_insert(trie,prefix,NULL,l32,(
trie_node_t*)l218);if(SOC_FAILURE(l184)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"_soc_th_alpm_128_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x 0x%08x 0x%08x into pfx trie\n"),prefix[1],prefix[2],
prefix[3],prefix[4]));}SOC_ALPM_LPM_UNLOCK(l1);return l142;}int
soc_th_alpm_128_init(int l1){int l142 = SOC_E_NONE;l142 = 
soc_th_alpm_128_lpm_init(l1);SOC_IF_ERROR_RETURN(l142);return l142;}int
soc_th_alpm_128_state_clear(int l1){int l141,l142;for(l141 = 0;l141<= 
SOC_VRF_MAX(l1)+1;l141++){l142 = trie_traverse(VRF_PREFIX_TRIE_IPV6_128(l1,
l141),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l142)){
trie_destroy(VRF_PREFIX_TRIE_IPV6_128(l1,l141));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit: %d Unable to clear v6_128 pfx trie for ""vrf %d\n"),l1,l141));return
l142;}if(VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l141)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,l141));}}return SOC_E_NONE;}int
soc_th_alpm_128_deinit(int l1){soc_th_alpm_128_lpm_deinit(l1);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_state_clear(l1));return SOC_E_NONE;}
static int l235(int l1,int vrf,int l27){defip_pair_128_entry_t*lpm_entry = 
NULL,l236;int l237 = 0;int index;int l142 = SOC_E_NONE;uint32 key[5] = {0,0,0
,0,0};uint32 l32;alpm_bucket_handle_t*l208 = NULL;alpm_pivot_t*pivot_pyld = 
NULL;payload_t*l229 = NULL;trie_t*l238;trie_t*l239 = NULL;trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(l1,vrf));l239 = 
VRF_PIVOT_TRIE_IPV6_128(l1,vrf);trie_init(_MAX_KEY_LEN_144_,&
VRF_PREFIX_TRIE_IPV6_128(l1,vrf));l238 = VRF_PREFIX_TRIE_IPV6_128(l1,vrf);
lpm_entry = sal_alloc(sizeof(*lpm_entry),"Default 128 LPM entry");if(
lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: unable to allocate memory "
"for IPv6-128 LPM entry\n")));return SOC_E_MEMORY;}l23(l1,key,0,vrf,l27,
lpm_entry,0,1);VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf) = lpm_entry;if(vrf == 
SOC_VRF_MAX(l1)+1){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(l1)]->l59),(1));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(l1)]->l96),(1));}l142 = soc_th_alpm_bs_alloc(l1,&l237,vrf,
l27);if(SOC_FAILURE(l142)){goto l240;}soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(l1)]->l94),(
ALPM_BKT_IDX(l237)));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(l1)]->l95),(ALPM_BKT_SIDX(l237)));
sal_memcpy(&l236,lpm_entry,sizeof(l236));l142 = l2(l1,&l236,&index);if(
SOC_FAILURE(l142)){goto l240;}l208 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l208 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"soc_th_alpm_128_vrf_add: Unable to allocate memory "
"for bucket handle \n")));l142 = SOC_E_MEMORY;goto l240;}sal_memset(l208,0,
sizeof(*l208));pivot_pyld = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(pivot_pyld == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_th_alpm_128_vrf_add: Unable to allocate memory "
"for PIVOT trie node \n")));l142 = SOC_E_MEMORY;goto l240;}l229 = sal_alloc(
sizeof(payload_t),"Payload for pfx trie key");if(l229 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_add: Unable to allocate memory ""for pfx trie node \n"))
);l142 = SOC_E_MEMORY;goto l240;}sal_memset(pivot_pyld,0,sizeof(*pivot_pyld))
;sal_memset(l229,0,sizeof(*l229));l32 = 0;PIVOT_BUCKET_HANDLE(pivot_pyld) = 
l208;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(pivot_pyld));
PIVOT_BUCKET_INDEX(pivot_pyld) = l237;PIVOT_BUCKET_VRF(pivot_pyld) = vrf;
PIVOT_BUCKET_IPV6(pivot_pyld) = l27;PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;
pivot_pyld->key[0] = l229->key[0] = key[0];pivot_pyld->key[1] = l229->key[1] = 
key[1];pivot_pyld->key[2] = l229->key[2] = key[2];pivot_pyld->key[3] = l229->
key[3] = key[3];pivot_pyld->key[4] = l229->key[4] = key[4];pivot_pyld->len = 
l229->len = l32;l142 = trie_insert(l238,key,NULL,l32,&(l229->node));if(
SOC_FAILURE(l142)){goto l240;}l142 = trie_insert(l239,key,NULL,l32,(
trie_node_t*)pivot_pyld);if(SOC_FAILURE(l142)){trie_node_t*l198 = NULL;(void)
trie_delete(l238,key,l32,&l198);goto l240;}index = soc_th_alpm_physical_idx(
l1,L3_DEFIP_PAIR_128m,index,l27);ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(
index)<<1) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = SOC_ALPM_128_ADDR_LWR(
index)<<1;VRF_PIVOT_REF_INC(l1,vrf,l27);VRF_TRIE_INIT_DONE(l1,vrf,l27,1);
return l142;l240:if(l229!= NULL){sal_free(l229);}if(pivot_pyld!= NULL){
sal_free(pivot_pyld);}if(l208!= NULL){sal_free(l208);}if(lpm_entry!= NULL){
sal_free(lpm_entry);}VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf) = NULL;return
l142;}static int l26(int l1,int vrf,int l27){defip_pair_128_entry_t*lpm_entry
;int l241,vrf_id,l242;int l142 = SOC_E_NONE;uint32 key[2] = {0,0},l146[
SOC_MAX_MEM_FIELD_WORDS];payload_t*l218;alpm_pivot_t*l243;trie_node_t*l198;
trie_t*l238;trie_t*l239 = NULL;soc_mem_t l17;int tcam_index,bktid,index;
uint32 l158[SOC_MAX_MEM_FIELD_WORDS];lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf);l17 = L3_DEFIP_ALPM_IPV6_128m;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,lpm_entry,&vrf_id,&l242));
l142 = _soc_th_alpm_128_find(l1,l17,lpm_entry,vrf_id,l242,l158,&tcam_index,&
bktid,&index,TRUE);l142 = soc_th_alpm_bs_free(l1,bktid,vrf,l27);
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,
ALPM_BKT_IDX(bktid)),INVALIDm);if(SOC_TH_ALPM_SCALE_CHECK(l1,l27)){
_soc_tomahawk_alpm_bkt_view_set(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,
ALPM_BKT_IDX(bktid)+1),INVALIDm);}l142 = l13(l1,lpm_entry,(void*)l146,&l241);
if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_vrf_delete: unable to get internal"" pivot idx for vrf %d/%d\n")
,vrf,l27));l241 = -1;}l241 = soc_th_alpm_physical_idx(l1,L3_DEFIP_PAIR_128m,
l241,l27);l243 = ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l241)<<1);l142 = 
soc_th_alpm_128_lpm_delete(l1,lpm_entry);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_vrf_delete: unable to delete lpm "
"entry for internal default for vrf %d/%d\n"),vrf,l27));}sal_free(lpm_entry);
VRF_TRIE_DEFAULT_ROUTE_IPV6_128(l1,vrf) = NULL;l238 = 
VRF_PREFIX_TRIE_IPV6_128(l1,vrf);VRF_PREFIX_TRIE_IPV6_128(l1,vrf) = NULL;
VRF_TRIE_INIT_DONE(l1,vrf,l27,0);l142 = trie_delete(l238,key,0,&l198);l218 = 
(payload_t*)l198;if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Unable to delete internal default for 128b vrf "" %d/%d\n"),vrf,l27));}
sal_free(l218);(void)trie_destroy(l238);l239 = VRF_PIVOT_TRIE_IPV6_128(l1,vrf
);VRF_PIVOT_TRIE_IPV6_128(l1,vrf) = NULL;l198 = NULL;l142 = trie_delete(l239,
key,0,&l198);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Unable to delete internal pivot node for vrf"" %d/%d\n"),vrf,l27));}(void)
trie_destroy(l239);(void)trie_destroy(PIVOT_BUCKET_TRIE(l243));sal_free(
PIVOT_BUCKET_HANDLE(l243));sal_free(l243);return l142;}int
soc_th_alpm_128_insert(int l1,void*l3,uint32 l18,int l244,int l245){
defip_alpm_ipv6_128_entry_t l188,l189;soc_mem_t l17;void*l186,*l224;int vrf_id
,vrf;int index;int l5;int l142 = SOC_E_NONE;uint32 l19;int l20 = 0;l5 = 
L3_DEFIP_MODE_128;l17 = L3_DEFIP_ALPM_IPV6_128m;l186 = ((uint32*)&(l188));
l224 = ((uint32*)&(l189));if(l244!= -1){l20 = ALPM_BKTID(ALPM_BKT_IDX(
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l244)),(l244>>ALPM_ENT_INDEX_BITS));}
SOC_IF_ERROR_RETURN(l14(l1,l3,l186,l224,l17,l18,&l19,l20));
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,l3,&vrf_id,&vrf));if(((
vrf_id == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == SOC_L3_VRF_GLOBAL)))){l142 = l2(l1,l3,&
index);if(SOC_SUCCESS(l142)){if(vrf_id == SOC_L3_VRF_OVERRIDE){
VRF_TRIE_ROUTES_INC(l1,MAX_VRF_ID,l5);VRF_PIVOT_REF_INC(l1,MAX_VRF_ID,l5);}
else{VRF_TRIE_ROUTES_INC(l1,vrf,l5);VRF_PIVOT_REF_INC(l1,vrf,l5);}}else if(
l142 == SOC_E_FULL){if(vrf_id == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_FULL_INC(l1,
MAX_VRF_ID,l5);}else{VRF_PIVOT_FULL_INC(l1,vrf,l5);}}return(l142);}else if(
vrf == 0){if(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL||
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"VRF=0 cannot be added in Parallel mode\n")));
return SOC_E_PARAM;}}if(vrf_id!= SOC_L3_VRF_GLOBAL){if(soc_th_alpm_mode_get(
l1) == SOC_ALPM_MODE_COMBINED){if(VRF_TRIE_ROUTES_CNT(l1,vrf,l5) == 0){if(!
l19){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
vrf_id));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l1,vrf,l5)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d is not ""initialized\n"),vrf));l142 = l235(l1,
vrf,l5);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init \n""failed\n"),vrf,l5));return
l142;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_insert:VRF %d/%d trie init ""completed\n"),vrf,l5));}if(l245
&SOC_ALPM_LOOKUP_HIT){l142 = l157(l1,l3,l186,l224,l17,l244);}else{if(l244 == 
-1){l244 = 0;}l244 = ALPM_BKTID(ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,
l244)),l244>>ALPM_ENT_INDEX_BITS);l142 = l216(l1,l3,l17,l186,l224,&index,l244
,l245);}if(l142!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"unit %d :soc_th_alpm_128_insert: ""Route Insertion Failed :%s\n"),l1,
soc_errmsg(l142)));}return(l142);}int soc_th_alpm_128_lookup(int l1,void*
key_data,void*l10,int*l11,int*l246){defip_alpm_ipv6_128_entry_t l188;
soc_mem_t l17;int bktid = 0;int tcam_index;void*l186;int vrf_id,vrf;int l5 = 
2,l133;int l142 = SOC_E_NONE;l142 = l9(l1,key_data,l10,l11,&l133,&l5,&vrf_id,
&vrf);if(SOC_SUCCESS(l142)){SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(
l1,l10,&vrf_id,&vrf));if(((vrf_id == SOC_L3_VRF_OVERRIDE)||((
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == 
SOC_L3_VRF_GLOBAL)))){return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l1,vrf,
l5)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_lookup:VRF %d is not ""initialized\n"),vrf));*l11 = 0;*l246 = 
0;return SOC_E_NOT_FOUND;}l17 = L3_DEFIP_ALPM_IPV6_128m;l186 = ((uint32*)&(
l188));SOC_ALPM_LPM_LOCK(l1);l142 = _soc_th_alpm_128_find(l1,l17,key_data,
vrf_id,vrf,l186,&tcam_index,&bktid,l11,TRUE);SOC_ALPM_LPM_UNLOCK(l1);if(
SOC_FAILURE(l142)){*l246 = tcam_index;*l11 = (ALPM_BKT_SIDX(bktid)<<
ALPM_ENT_INDEX_BITS)|SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,bktid);return l142;}l142
= l21(l1,l186,l17,l5,vrf_id,bktid,*l11,l10);*l246 = SOC_ALPM_LOOKUP_HIT|
tcam_index;*l11 = (ALPM_BKT_SIDX(bktid)<<ALPM_ENT_INDEX_BITS)|*l11;return(
l142);}int l247(int l1,void*key_data,void*l10,int vrf,int*tcam_index,int*
bucket_index,int*l134,int l248){int l142 = SOC_E_NONE;int l141,l249,l27,l152 = 
0;uint32 l6,l156;defip_aux_scratch_entry_t l8;int l250,l251;int index;
soc_mem_t l17,l178;int l252,l253,l254;soc_field_t l255[4] = {IP_ADDR0_LWRf,
IP_ADDR1_LWRf,IP_ADDR0_UPRf,IP_ADDR1_UPRf};uint32 l256[
SOC_MAX_MEM_FIELD_WORDS] = {0};int l257 = -1;int l258 = 0;l27 = 
L3_DEFIP_MODE_128;l178 = L3_DEFIP_PAIR_128m;l250 = soc_mem_field32_get(l1,
l178,key_data,GLOBAL_ROUTEf);l251 = soc_mem_field32_get(l1,l178,key_data,
VRF_ID_0_LWRf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d\n"),vrf == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l27,l250,l251));if(vrf == SOC_L3_VRF_GLOBAL){l6 = l248?1:0;
soc_mem_field32_set(l1,l178,key_data,GLOBAL_ROUTEf,1);soc_mem_field32_set(l1,
l178,key_data,VRF_ID_0_LWRf,0);}else{l6 = l248?3:2;}
soc_th_alpm_bank_db_type_get(l1,vrf == SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l1)+1):
vrf,&l156,NULL);sal_memset(&l8,0,sizeof(defip_aux_scratch_entry_t));
SOC_IF_ERROR_RETURN(l4(l1,key_data,l27,l6,0,&l8));if(vrf == SOC_L3_VRF_GLOBAL
){soc_mem_field32_set(l1,l178,key_data,GLOBAL_ROUTEf,l250);
soc_mem_field32_set(l1,l178,key_data,VRF_ID_0_LWRf,l251);}SOC_IF_ERROR_RETURN
(_soc_th_alpm_aux_op(l1,PREFIX_LOOKUP,&l8,TRUE,&l152,tcam_index,bucket_index)
);if(l152 == 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Could not find bucket\n")));return SOC_E_NOT_FOUND;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Hit in memory %s, index %d, "
"bucket [%d,%d]\n"),SOC_MEM_NAME(l1,l178),soc_th_alpm_logical_idx(l1,l178,
SOC_ALPM_128_DEFIP_TO_PAIR((*tcam_index)>>1),1),ALPM_BKT_IDX(*bucket_index),
ALPM_BKT_SIDX(*bucket_index)));l17 = L3_DEFIP_ALPM_IPV6_128m;l142 = l140(l1,
key_data,&l253);if(SOC_FAILURE(l142)){return l142;}l254 = 
SOC_TH_ALPM_SCALE_CHECK(l1,l27)?16:8;LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Start searching mem %s bucket [%d,%d](count %d) ""for Length %d\n"),
SOC_MEM_NAME(l1,l17),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index)
,l254,l253));for(l141 = 0;l141<l254;l141++){uint32 l186[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l259[4] = {0};uint32 l260[4] = {0};
uint32 l261[4] = {0};int l262;l142 = _soc_th_alpm_mem_index(l1,l17,
ALPM_BKT_IDX(*bucket_index),l141,l156,&index);if(l142 == SOC_E_FULL){continue
;}SOC_IF_ERROR_RETURN(soc_mem_read(l1,l17,MEM_BLOCK_ANY,index,(void*)&l186));
l262 = soc_mem_field32_get(l1,l17,&l186,VALIDf);l252 = soc_mem_field32_get(l1
,l17,&l186,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Bucket [%d,%d] index %6d: valid %d, length %d\n"),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),index,l262,l252));if(!l262||(l252>l253)){
continue;}SHR_BITSET_RANGE(l259,128-l252,l252);(void)soc_mem_field_get(l1,l17
,(uint32*)&l186,KEYf,(uint32*)l260);l261[3] = soc_mem_field32_get(l1,l178,
key_data,l255[3]);l261[2] = soc_mem_field32_get(l1,l178,key_data,l255[2]);
l261[1] = soc_mem_field32_get(l1,l178,key_data,l255[1]);l261[0] = 
soc_mem_field32_get(l1,l178,key_data,l255[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tmask %08x %08x %08x %08x \n""\t key %08x %08x %08x %08x \n"
"\thost %08x %08x %08x %08x \n"),l259[3],l259[2],l259[1],l259[0],l260[3],l260
[2],l260[1],l260[0],l261[3],l261[2],l261[1],l261[0]));for(l249 = 3;l249>= 0;
l249--){if((l261[l249]&l259[l249])!= (l260[l249]&l259[l249])){break;}}if(l249
>= 0){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Found a match in mem %s bucket [%d,%d], ""index %d\n"),SOC_MEM_NAME(l1,l17),
ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index),index));if(l257 == -
1||l257<l252){l257 = l252;l258 = index;sal_memcpy(l256,l186,sizeof(l186));}}
if(l257!= -1){l142 = l21(l1,&l256,l17,l27,vrf,*bucket_index,l258,l10);if(
SOC_SUCCESS(l142)){*l134 = l258;if(bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l1)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit mem %s bucket [%d,%d], index %d\n"),SOC_MEM_NAME(l1,l17),ALPM_BKT_IDX(*
bucket_index),ALPM_BKT_SIDX(*bucket_index),l258));}}return l142;}*l134 = 
soc_th_alpm_logical_idx(l1,l178,SOC_ALPM_128_DEFIP_TO_PAIR((*tcam_index)>>1),
1);SOC_IF_ERROR_RETURN(soc_mem_read(l1,l178,MEM_BLOCK_ANY,*l134,(void*)l10));
return SOC_E_NONE;}int soc_th_alpm_128_find_best_match(int l1,void*key_data,
void*l10,int*l11,int l248){int l142 = SOC_E_NONE;int l141,l249;int l263,l264;
defip_pair_128_entry_t l265;uint32 l266,l260,l261;int l252,l253;int l267,l268
;int vrf_id,vrf = 0;int tcam_index,bucket_index;soc_mem_t l178;soc_field_t
l269[4] = {IP_ADDR_MASK1_UPRf,IP_ADDR_MASK0_UPRf,IP_ADDR_MASK1_LWRf,
IP_ADDR_MASK0_LWRf};soc_field_t l270[4] = {IP_ADDR1_UPRf,IP_ADDR0_UPRf,
IP_ADDR1_LWRf,IP_ADDR0_LWRf};l178 = L3_DEFIP_PAIR_128m;if(!
SOC_URPF_STATUS_GET(l1)&&l248){return SOC_E_PARAM;}l263 = soc_mem_index_min(
l1,l178);l264 = soc_mem_index_count(l1,l178);if(SOC_URPF_STATUS_GET(l1)){l264
>>= 1;}if(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL||
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM){l264>>= 1;l263+= l264;}
if(l248){l263+= soc_mem_index_count(l1,l178)/2;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Start LPM searchng from %d, count %d\n"),l263,l264));for(l141 = 
l263;l141<l263+l264;l141++){SOC_IF_ERROR_RETURN(soc_mem_read(l1,l178,
MEM_BLOCK_ANY,l141,(void*)&l265));if(!soc_mem_field32_get(l1,l178,&l265,
VALID0_LWRf)){continue;}l267 = soc_mem_field32_get(l1,l178,&l265,GLOBAL_HIGHf
);l268 = soc_mem_field32_get(l1,l178,&l265,GLOBAL_ROUTEf);if(!(
soc_th_alpm_mode_get(l1)!= SOC_ALPM_MODE_TCAM_ALPM&&l267&&l268)&&!(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM&&l268)){continue;}l142 = 
l140(l1,key_data,&l253);l142 = l140(l1,&l265,&l252);if(SOC_FAILURE(l142)||(
l252>l253)){continue;}for(l249 = 0;l249<4;l249++){l266 = soc_mem_field32_get(
l1,l178,&l265,l269[l249]);l260 = soc_mem_field32_get(l1,l178,&l265,l270[l249]
);l261 = soc_mem_field32_get(l1,l178,key_data,l270[l249]);if((l261&l266)!= (
l260&l266)){break;}}if(l249<4){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Hit Global High route in index = %d\n"),l249));sal_memcpy(l10,
&l265,sizeof(l265));*l11 = l141;return SOC_E_NONE;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"Global high lookup miss, use AUX engine to "
"search for Global Low and VRF routes\n")));SOC_IF_ERROR_RETURN(
soc_th_alpm_128_lpm_vrf_get(l1,key_data,&vrf_id,&vrf));l142 = l247(l1,
key_data,l10,vrf,&tcam_index,&bucket_index,l11,l248);if(l142 == 
SOC_E_NOT_FOUND){vrf = SOC_L3_VRF_GLOBAL;LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"Not found in VRF region, try Global ""region\n")));l142 = l247
(l1,key_data,l10,vrf,&tcam_index,&bucket_index,l11,l248);}if(SOC_SUCCESS(l142
)){tcam_index = soc_th_alpm_logical_idx(l1,l178,SOC_ALPM_128_DEFIP_TO_PAIR(
tcam_index>>1),1);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"Hit in %s region in TCAM index %d, ""bucket [%d,%d]\n"),vrf == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",tcam_index,ALPM_BKT_IDX(bucket_index),
ALPM_BKT_SIDX(bucket_index)));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"Search miss for given address\n")));}return(l142);}int
soc_th_alpm_128_delete(int l1,void*key_data,int l244,int l245){int vrf_id,vrf
;int l5;int l142 = SOC_E_NONE;int l271;l5 = L3_DEFIP_MODE_128;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(l1,key_data,&vrf_id,&vrf));if
(((vrf_id == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == SOC_L3_VRF_GLOBAL)))){l142 = 
soc_th_alpm_128_lpm_delete(l1,key_data);if(SOC_SUCCESS(l142)){if(vrf_id == 
SOC_L3_VRF_OVERRIDE){VRF_PIVOT_REF_DEC(l1,MAX_VRF_ID,l5);VRF_TRIE_ROUTES_DEC(
l1,MAX_VRF_ID,l5);}else{VRF_PIVOT_REF_DEC(l1,vrf,l5);VRF_TRIE_ROUTES_DEC(l1,
vrf,l5);}}return(l142);}else{if(!VRF_TRIE_INIT_COMPLETED(l1,vrf,l5)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_128_delete:VRF %d/%d is not ""initialized\n"),vrf,l5));return
SOC_E_NONE;}if(l244 == -1){l244 = 0;}l271 = l244;l244 = ALPM_BKTID(
ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l1,l271)),l271>>ALPM_ENT_INDEX_BITS
);l142 = l222(l1,key_data,vrf_id,vrf,l244,l245&~SOC_ALPM_LOOKUP_HIT,l244);}
return(l142);}static void l121(int l1,void*l10,int index,l116 l122){l122[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l109[(l1)]->l63));l122[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l109[(l1)]->l61));l122[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l109[(l1)]->l67));l122[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l109[(l1)]->l65));l122[4] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l109[(l1)]->l64));l122[5] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l109[(l1)]->l62));l122[6] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l109[(l1)]->l68));l122[7] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l109[(l1)]->l66));if((!(
SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l86)!= NULL))){int l272;(void)
soc_th_alpm_128_lpm_vrf_get(l1,l10,(int*)&l122[8],&l272);}else{l122[8] = 0;};
}static int l273(l116 l118,l116 l119){int l241;for(l241 = 0;l241<9;l241++){{
if((l118[l241])<(l119[l241])){return-1;}if((l118[l241])>(l119[l241])){return 1
;}};}return(0);}static void l274(int l1,void*l3,uint32 l275,uint32 l136,int
l133){l116 l276;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l109[(l1)]->l83))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l109[(l1)]->l82))&&soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(l3),(l109[(l1)]->l81))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l109[(l1)]->l80))){l276[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l109[(l1)]->l63));l276[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l109[(l1)]->l61));l276[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l109[(l1)]->l67));l276[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l109[(l1)]->l65));l276[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l109[(l1)]->l64));l276[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l109[(l1)]->l62));l276[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3),(l109[(l1)]->l68));l276[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l3)
,(l109[(l1)]->l66));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l86)!= NULL)
)){int l272;(void)soc_th_alpm_128_lpm_vrf_get(l1,l3,(int*)&l276[8],&l272);}
else{l276[8] = 0;};l135((l120[(l1)]),l273,l276,l133,l136,l275);}}static void
l277(int l1,void*key_data,uint32 l275){l116 l276;int l133 = -1;int l142;
uint16 index;l276[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l109[(l1)]->l63));l276[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(l1)]->l61));l276[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l109[(l1)]->l67));l276[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(l1)]->l65));l276[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l109[(l1)]->l64));l276[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(l1)]->l62));l276[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l109[(l1)]->l68));l276[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(l1)]->l66));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l86
)!= NULL))){int l272;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l276[8],&l272);}else{l276[8] = 0;};index = l275;l142 = l137((l120[(l1)]),l273
,l276,l133,index);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l1,"\ndel  index: H %d error %d\n"),index,l142));}}static int l278(int l1,
void*key_data,int l133,int*l134){l116 l276;int l142;uint16 index = (0xFFFF);
l276[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,
L3_DEFIP_PAIR_128m)),(key_data),(l109[(l1)]->l63));l276[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(l1)]->l61));l276[2] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l109[(l1)]->l67));l276[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(l1)]->l65));l276[4] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l109[(l1)]->l64));l276[5] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(l1)]->l62));l276[6] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(key_data),(l109[(l1)]->l68));l276[7] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(l1)]->l66));if((!(SOC_IS_HURRICANE(l1)))&&(((l109[(l1)]->l86
)!= NULL))){int l272;(void)soc_th_alpm_128_lpm_vrf_get(l1,key_data,(int*)&
l276[8],&l272);}else{l276[8] = 0;};l142 = l130((l120[(l1)]),l273,l276,l133,&
index);if(SOC_FAILURE(l142)){*l134 = 0xFFFFFFFF;return(l142);}*l134 = index;
return(SOC_E_NONE);}static uint16 l123(uint8*l124,int l125){return(
_shr_crc16b(0,l124,l125));}static int l126(int unit,int l111,int l112,l115**
l127){l115*l131;int index;if(l112>l111){return SOC_E_MEMORY;}l131 = sal_alloc
(sizeof(l115),"lpm_hash");if(l131 == NULL){return SOC_E_MEMORY;}sal_memset(
l131,0,sizeof(*l131));l131->unit = unit;l131->l111 = l111;l131->l112 = l112;
l131->l113 = sal_alloc(l131->l112*sizeof(*(l131->l113)),"hash_table");if(l131
->l113 == NULL){sal_free(l131);return SOC_E_MEMORY;}l131->l114 = sal_alloc(
l131->l111*sizeof(*(l131->l114)),"link_table");if(l131->l114 == NULL){
sal_free(l131->l113);sal_free(l131);return SOC_E_MEMORY;}for(index = 0;index<
l131->l112;index++){l131->l113[index] = (0xFFFF);}for(index = 0;index<l131->
l111;index++){l131->l114[index] = (0xFFFF);}*l127 = l131;return SOC_E_NONE;}
static int l128(l115*l129){if(l129!= NULL){sal_free(l129->l113);sal_free(l129
->l114);sal_free(l129);}return SOC_E_NONE;}static int l130(l115*l131,l117 l132
,l116 entry,int l133,uint16*l134){int l1 = l131->unit;uint16 l279;uint16 index
;l279 = l123((uint8*)entry,(32*9))%l131->l112;index = l131->l113[l279];;;
while(index!= (0xFFFF)){uint32 l10[SOC_MAX_MEM_FIELD_WORDS];l116 l122;int l280
;l280 = index;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,
l280,l10));l121(l1,l10,index,l122);if((*l132)(entry,l122) == 0){*l134 = index
;;return(SOC_E_NONE);}index = l131->l114[index&(0x3FFF)];;};return(
SOC_E_NOT_FOUND);}static int l135(l115*l131,l117 l132,l116 entry,int l133,
uint16 l136,uint16 l36){int l1 = l131->unit;uint16 l279;uint16 index;uint16
l281;l279 = l123((uint8*)entry,(32*9))%l131->l112;index = l131->l113[l279];;;
;l281 = (0xFFFF);if(l136!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];l116 l122;int l280;l280 = index;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,l280,l10));l121(l1,l10,index,l122);
if((*l132)(entry,l122) == 0){if(l36!= index){;if(l281 == (0xFFFF)){l131->l113
[l279] = l36;l131->l114[l36&(0x3FFF)] = l131->l114[index&(0x3FFF)];l131->l114
[index&(0x3FFF)] = (0xFFFF);}else{l131->l114[l281&(0x3FFF)] = l36;l131->l114[
l36&(0x3FFF)] = l131->l114[index&(0x3FFF)];l131->l114[index&(0x3FFF)] = (
0xFFFF);}};return(SOC_E_NONE);}l281 = index;index = l131->l114[index&(0x3FFF)
];;}}l131->l114[l36&(0x3FFF)] = l131->l113[l279];l131->l113[l279] = l36;
return(SOC_E_NONE);}static int l137(l115*l131,l117 l132,l116 entry,int l133,
uint16 l138){uint16 l279;uint16 index;uint16 l281;l279 = l123((uint8*)entry,(
32*9))%l131->l112;index = l131->l113[l279];;;l281 = (0xFFFF);while(index!= (
0xFFFF)){if(l138 == index){;if(l281 == (0xFFFF)){l131->l113[l279] = l131->
l114[l138&(0x3FFF)];l131->l114[l138&(0x3FFF)] = (0xFFFF);}else{l131->l114[
l281&(0x3FFF)] = l131->l114[l138&(0x3FFF)];l131->l114[l138&(0x3FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l281 = index;index = l131->l114[index&(0x3FFF)];
;}return(SOC_E_NOT_FOUND);}static int l282(int l1,void*l10){return(SOC_E_NONE
);}void soc_th_alpm_128_lpm_state_dump(int l1){int l141;int l283;l283 = ((3*(
128+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,bslSeverityVerbose,l1)){
return;}for(l141 = l283;l141>= 0;l141--){if((l141!= ((3*(128+2+1))-1))&&((l51
[(l1)][(l141)].l44) == -1)){continue;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l1,"PFX = %d P = %d N = %d START = %d ""END = %d VENT = %d FENT = %d\n"),l141
,(l51[(l1)][(l141)].l46),(l51[(l1)][(l141)].next),(l51[(l1)][(l141)].l44),(
l51[(l1)][(l141)].l45),(l51[(l1)][(l141)].l47),(l51[(l1)][(l141)].l48)));}
COMPILER_REFERENCE(l282);}static int l284(int l1,int index,uint32*l10){int
l285;uint32 l286,l287,l288;uint32 l289;int l290;if(!SOC_URPF_STATUS_GET(l1)){
return(SOC_E_NONE);}if(soc_feature(l1,soc_feature_l3_defip_hole)){l285 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}else if(SOC_IS_APOLLO(l1)){
l285 = (soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1)+0x0400;}else{l285 = (
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m)>>1);}if(((l109[(l1)]->l54)!= NULL)
){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(
l10),(l109[(l1)]->l54),(0));}l286 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10),(l109[(l1)]->l67));l289 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l109[(l1)]->l68));l287 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l109[(l1)]->l65));l288 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l109[(l1)]->l66));l290 = ((!l286)&&(!l289)&&(!l287)&&(!l288))?1:0;l286 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l109[(l1)]->l82));l289 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l109[(l1)]->l80));l287 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l109[(l1)]->l81));l288 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l1,L3_DEFIP_PAIR_128m)),(l10),(l109[(l1)]->l81));if(l286&&l289&&l287&&l288){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l1,L3_DEFIP_PAIR_128m)),(l10
),(l109[(l1)]->l79),(l290));}return l173(l1,MEM_BLOCK_ANY,index+l285,index,
l10);}static int l291(int l1,int l292,int l293){uint32 l10[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l292,l10));l274(l1,l10,l293,0x4000,0);SOC_IF_ERROR_RETURN(l173(
l1,MEM_BLOCK_ANY,l293,l292,l10));SOC_IF_ERROR_RETURN(l284(l1,l293,l10));do{
int l294 = soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l292),1);int l295
= soc_th_alpm_physical_idx((l1),L3_DEFIP_PAIR_128m,(l293),1);int l296 = 
SOC_ALPM_128_ADDR_LWR((l295))<<1;ALPM_TCAM_PIVOT((l1),l296) = ALPM_TCAM_PIVOT
((l1),SOC_ALPM_128_ADDR_LWR((l294))<<1);if(ALPM_TCAM_PIVOT((l1),l296)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l1),l296)) = l296;
soc_th_alpm_lpm_move_bu_upd((l1),PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l1),l296
)),l296);}ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR((l294))<<1) = NULL;}while(
0);return(SOC_E_NONE);}static int l297(int l1,int l133,int l5){int l292;int
l293;l293 = (l51[(l1)][(l133)].l45)+1;l292 = (l51[(l1)][(l133)].l44);if(l292
!= l293){SOC_IF_ERROR_RETURN(l291(l1,l292,l293));VRF_PIVOT_SHIFT_INC(l1,
MAX_VRF_ID,l5);}(l51[(l1)][(l133)].l44)+= 1;(l51[(l1)][(l133)].l45)+= 1;
return(SOC_E_NONE);}static int l298(int l1,int l133,int l5){int l292;int l293
;l293 = (l51[(l1)][(l133)].l44)-1;if((l51[(l1)][(l133)].l47) == 0){(l51[(l1)]
[(l133)].l44) = l293;(l51[(l1)][(l133)].l45) = l293-1;return(SOC_E_NONE);}
l292 = (l51[(l1)][(l133)].l45);SOC_IF_ERROR_RETURN(l291(l1,l292,l293));
VRF_PIVOT_SHIFT_INC(l1,MAX_VRF_ID,l5);(l51[(l1)][(l133)].l44)-= 1;(l51[(l1)][
(l133)].l45)-= 1;return(SOC_E_NONE);}static int l299(int l1,int l133,int l5,
void*l10,int*l300){int l301;int l302;int l303;int l304;if((l51[(l1)][(l133)].
l47) == 0){l304 = ((3*(128+2+1))-1);if((soc_th_alpm_mode_get(l1) == 
SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM
)){if(l133<= (((3*(128+2+1))/3)-1)){l304 = (((3*(128+2+1))/3)-1);}}while((l51
[(l1)][(l304)].next)>l133){l304 = (l51[(l1)][(l304)].next);}l302 = (l51[(l1)]
[(l304)].next);if(l302!= -1){(l51[(l1)][(l302)].l46) = l133;}(l51[(l1)][(l133
)].next) = (l51[(l1)][(l304)].next);(l51[(l1)][(l133)].l46) = l304;(l51[(l1)]
[(l304)].next) = l133;(l51[(l1)][(l133)].l48) = ((l51[(l1)][(l304)].l48)+1)/2
;(l51[(l1)][(l304)].l48)-= (l51[(l1)][(l133)].l48);(l51[(l1)][(l133)].l44) = 
(l51[(l1)][(l304)].l45)+(l51[(l1)][(l304)].l48)+1;(l51[(l1)][(l133)].l45) = (
l51[(l1)][(l133)].l44)-1;(l51[(l1)][(l133)].l47) = 0;}l303 = l133;while((l51[
(l1)][(l303)].l48) == 0){l303 = (l51[(l1)][(l303)].next);if(l303 == -1){l303 = 
l133;break;}}while((l51[(l1)][(l303)].l48) == 0){l303 = (l51[(l1)][(l303)].
l46);if(l303 == -1){if((l51[(l1)][(l133)].l47) == 0){l301 = (l51[(l1)][(l133)
].l46);l302 = (l51[(l1)][(l133)].next);if(-1!= l301){(l51[(l1)][(l301)].next)
= l302;}if(-1!= l302){(l51[(l1)][(l302)].l46) = l301;}}return(SOC_E_FULL);}}
while(l303>l133){l302 = (l51[(l1)][(l303)].next);SOC_IF_ERROR_RETURN(l298(l1,
l302,l5));(l51[(l1)][(l303)].l48)-= 1;(l51[(l1)][(l302)].l48)+= 1;l303 = l302
;}while(l303<l133){SOC_IF_ERROR_RETURN(l297(l1,l303,l5));(l51[(l1)][(l303)].
l48)-= 1;l301 = (l51[(l1)][(l303)].l46);(l51[(l1)][(l301)].l48)+= 1;l303 = 
l301;}(l51[(l1)][(l133)].l47)+= 1;(l51[(l1)][(l133)].l48)-= 1;(l51[(l1)][(
l133)].l45)+= 1;*l300 = (l51[(l1)][(l133)].l45);sal_memcpy(l10,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);return(SOC_E_NONE);}static int l305(int l1,int l133,
int l5,void*l10,int l306){int l301;int l302;int l292;int l293;uint32 l307[
SOC_MAX_MEM_FIELD_WORDS];int l142;int l148;l292 = (l51[(l1)][(l133)].l45);
l293 = l306;(l51[(l1)][(l133)].l47)-= 1;(l51[(l1)][(l133)].l48)+= 1;(l51[(l1)
][(l133)].l45)-= 1;if(l293!= l292){if((l142 = READ_L3_DEFIP_PAIR_128m(l1,
MEM_BLOCK_ANY,l292,l307))<0){return l142;}l274(l1,l307,l293,0x4000,0);if((
l142 = l173(l1,MEM_BLOCK_ANY,l293,l292,l307))<0){return l142;}if((l142 = l284
(l1,l293,l307))<0){return l142;}}l148 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l293,1);l306 = soc_th_alpm_physical_idx(l1,
L3_DEFIP_PAIR_128m,l292,1);ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l148)<<1)
= ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l306)<<1);if(ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l148)<<1)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l148)<<1)) = SOC_ALPM_128_ADDR_LWR(l148)<<1;
soc_th_alpm_lpm_move_bu_upd(l1,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l1,
SOC_ALPM_128_ADDR_LWR(l148)<<1)),SOC_ALPM_128_ADDR_LWR(l148)<<1);}
ALPM_TCAM_PIVOT(l1,SOC_ALPM_128_ADDR_LWR(l306)<<1) = NULL;sal_memcpy(l307,
soc_mem_entry_null(l1,L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,
L3_DEFIP_PAIR_128m)*4);l274(l1,l307,l292,0x4000,0);if((l142 = l173(l1,
MEM_BLOCK_ANY,l292,l292,l307))<0){return l142;}if((l142 = l284(l1,l292,l307))
<0){return l142;}if((l51[(l1)][(l133)].l47) == 0){l301 = (l51[(l1)][(l133)].
l46);assert(l301!= -1);l302 = (l51[(l1)][(l133)].next);(l51[(l1)][(l301)].
next) = l302;(l51[(l1)][(l301)].l48)+= (l51[(l1)][(l133)].l48);(l51[(l1)][(
l133)].l48) = 0;if(l302!= -1){(l51[(l1)][(l302)].l46) = l301;}(l51[(l1)][(
l133)].next) = -1;(l51[(l1)][(l133)].l46) = -1;(l51[(l1)][(l133)].l44) = -1;(
l51[(l1)][(l133)].l45) = -1;}return(l142);}int soc_th_alpm_128_lpm_vrf_get(
int unit,void*lpm_entry,int*vrf,int*l308){int vrf_id;if(((l109[(unit)]->l90)
!= NULL)){vrf_id = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l86));*l308 = vrf_id;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l90))){*vrf = vrf_id;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l92))){*vrf = SOC_L3_VRF_GLOBAL;*l308 = SOC_VRF_MAX
(unit)+1;}else{*vrf = SOC_L3_VRF_OVERRIDE;}}else{*vrf = SOC_L3_VRF_DEFAULT;}
return(SOC_E_NONE);}static int l139(int l1,void*entry,int*l12,int*vrf_id,int*
vrf){int l133=0;int l142;int l309;int l310;l142 = l140(l1,entry,&l133);if(
l142<0){return l142;}l133+= 0;SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get
(l1,entry,&l309,&l142));if(vrf_id!= NULL){*vrf_id = l309;}if(vrf!= NULL){*vrf
= l142;}l310 = soc_th_alpm_mode_get(l1);switch(l309){case SOC_L3_VRF_GLOBAL:
if((l310 == SOC_ALPM_MODE_PARALLEL)||(l310 == SOC_ALPM_MODE_TCAM_ALPM)){*l12 = 
l133+((3*(128+2+1))/3);}else{*l12 = l133;}break;case SOC_L3_VRF_OVERRIDE:*l12
= l133+2*((3*(128+2+1))/3);break;default:if((l310 == SOC_ALPM_MODE_PARALLEL)
||(l310 == SOC_ALPM_MODE_TCAM_ALPM)){*l12 = l133;}else{*l12 = l133+((3*(128+2
+1))/3);}break;}return(SOC_E_NONE);}static int l9(int l1,void*key_data,void*
l10,int*l11,int*l12,int*l5,int*vrf_id,int*vrf){int l142;int l134;int l133 = 0
;*l5 = L3_DEFIP_MODE_128;l139(l1,key_data,&l133,vrf_id,vrf);*l12 = l133;if(
l278(l1,key_data,l133,&l134) == SOC_E_NONE){*l11 = l134;if((l142 = 
READ_L3_DEFIP_PAIR_128m(l1,MEM_BLOCK_ANY,(*l5)?*l11:(*l11>>1),l10))<0){return
l142;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}int
soc_th_alpm_128_lpm_init(int l1){int l283;int l141;int l311;int l312;if(!
soc_feature(l1,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}l283 = (3*(128+2
+1));l312 = sizeof(l49)*(l283);if((l51[(l1)]!= NULL)){SOC_IF_ERROR_RETURN(
soc_th_alpm_128_deinit(l1));}l109[l1] = sal_alloc(sizeof(l107),
"lpm_128_field_state");if(NULL == l109[l1]){return(SOC_E_MEMORY);}(l109[l1])
->l53 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,CLASS_IDf);(l109[l1])->
l54 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,DST_DISCARDf);(l109[l1])->
l55 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,SRC_DISCARDf);(l109[l1])->
l56 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMPf);(l109[l1])->l57 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_COUNTf);(l109[l1])->l58 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ECMP_PTRf);(l109[l1])->l59 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,GLOBAL_ROUTEf);(l109[l1])->l60 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,HITf);(l109[l1])->l63 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_LWRf);(l109[l1])->l61 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR0_UPRf);(l109[l1])->l64 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_LWRf);(l109[l1])->l62 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR1_UPRf);(l109[l1])->l67 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_LWRf);(l109[l1])->
l65 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK0_UPRf);(l109[
l1])->l68 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,IP_ADDR_MASK1_LWRf);(
l109[l1])->l66 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
IP_ADDR_MASK1_UPRf);(l109[l1])->l71 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_LWRf);(l109[l1])->l69 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE0_UPRf);(l109[l1])->l72 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_LWRf);(l109[l1])->l70 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE1_UPRf);(l109[l1])->l75 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,MODE_MASK0_LWRf);(l109[l1])->l73 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,MODE_MASK0_UPRf);(l109[l1])->l76 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_LWRf);(l109[l1])->l74 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,MODE_MASK1_UPRf);(l109[l1])->l77 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,NEXT_HOP_INDEXf);(l109[l1])->l78 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,PRIf);(l109[l1])->l79 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,RPEf);(l109[l1])->l82 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_LWRf);(l109[l1])->l80 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID0_UPRf);(l109[l1])->l83 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_LWRf);(l109[l1])->l81 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VALID1_UPRf);(l109[l1])->l86 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_LWRf);(l109[l1])->l84 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_0_UPRf);(l109[l1])->l87 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_LWRf);(l109[l1])->l85 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_1_UPRf);(l109[l1])->l90 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_LWRf);(l109[l1])->
l88 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK0_UPRf);(l109[l1
])->l91 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,VRF_ID_MASK1_LWRf);(
l109[l1])->l89 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
VRF_ID_MASK1_UPRf);(l109[l1])->l92 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,GLOBAL_HIGHf);(l109[l1])->l93 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_HIT_IDXf);(l109[l1])->l94 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_BKT_PTRf);(l109[l1])->l95 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ALG_SUB_BKT_PTRf);(l109[l1])->l96 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,DEFAULT_MISSf);(l109[l1])->l97 = soc_mem_fieldinfo_get(
l1,L3_DEFIP_PAIR_128m,FLEX_CTR_BASE_COUNTER_IDXf);(l109[l1])->l98 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,FLEX_CTR_POOL_NUMBERf);(l109[l1])
->l99 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_LWRf);(
l109[l1])->l100 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
ENTRY_TYPE_MASK1_LWRf);(l109[l1])->l101 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK0_UPRf);(l109[l1])->l102 = 
soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE_MASK1_UPRf);(l109[l1])
->l103 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE0_LWRf);(l109[
l1])->l104 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,ENTRY_TYPE1_LWRf);(
l109[l1])->l105 = soc_mem_fieldinfo_get(l1,L3_DEFIP_PAIR_128m,
ENTRY_TYPE0_UPRf);(l109[l1])->l106 = soc_mem_fieldinfo_get(l1,
L3_DEFIP_PAIR_128m,ENTRY_TYPE1_UPRf);(l51[(l1)]) = sal_alloc(l312,
"LPM 128 prefix info");if(NULL == (l51[(l1)])){sal_free(l109[l1]);l109[l1] = 
NULL;return(SOC_E_MEMORY);}SOC_ALPM_LPM_LOCK(l1);sal_memset((l51[(l1)]),0,
l312);for(l141 = 0;l141<l283;l141++){(l51[(l1)][(l141)].l44) = -1;(l51[(l1)][
(l141)].l45) = -1;(l51[(l1)][(l141)].l46) = -1;(l51[(l1)][(l141)].next) = -1;
(l51[(l1)][(l141)].l47) = 0;(l51[(l1)][(l141)].l48) = 0;}l311 = 
soc_mem_index_count(l1,L3_DEFIP_PAIR_128m);if(SOC_URPF_STATUS_GET(l1)){l311
>>= 1;}if((soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_PARALLEL)||(
soc_th_alpm_mode_get(l1) == SOC_ALPM_MODE_TCAM_ALPM)){(l51[(l1)][(((3*(128+2+
1))-1))].l45) = (l311>>1)-1;(l51[(l1)][(((((3*(128+2+1))/3)-1)))].l48) = l311
>>1;(l51[(l1)][((((3*(128+2+1))-1)))].l48) = (l311-(l51[(l1)][(((((3*(128+2+1
))/3)-1)))].l48));}else{(l51[(l1)][((((3*(128+2+1))-1)))].l48) = l311;}if((
l120[(l1)])!= NULL){if(l128((l120[(l1)]))<0){SOC_ALPM_LPM_UNLOCK(l1);return
SOC_E_INTERNAL;}(l120[(l1)]) = NULL;}if(l126(l1,l311*2,l311,&(l120[(l1)]))<0)
{SOC_ALPM_LPM_UNLOCK(l1);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l1);return(
SOC_E_NONE);}int soc_th_alpm_128_lpm_deinit(int l1){if(!soc_feature(l1,
soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l1);if((l120[
(l1)])!= NULL){l128((l120[(l1)]));(l120[(l1)]) = NULL;}if((l51[(l1)]!= NULL))
{sal_free(l109[l1]);l109[l1] = NULL;sal_free((l51[(l1)]));(l51[(l1)]) = NULL;
}SOC_ALPM_LPM_UNLOCK(l1);return(SOC_E_NONE);}static int l2(int l1,void*l3,int
*l177){int l133;int index;int l5;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l142
= SOC_E_NONE;int l313 = 0;sal_memcpy(l10,soc_mem_entry_null(l1,
L3_DEFIP_PAIR_128m),soc_mem_entry_words(l1,L3_DEFIP_PAIR_128m)*4);
SOC_ALPM_LPM_LOCK(l1);l142 = l9(l1,l3,l10,&index,&l133,&l5,0,0);if(l142 == 
SOC_E_NOT_FOUND){l142 = l299(l1,l133,l5,l10,&index);if(l142<0){
SOC_ALPM_LPM_UNLOCK(l1);return(l142);}}else{l313 = 1;}*l177 = index;if(l142 == 
SOC_E_NONE){soc_th_alpm_128_lpm_state_dump(l1);LOG_INFO(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"soc_th_alpm_128_lpm_insert: %d %d\n"),index,l133));if(!l313){
l274(l1,l3,index,0x4000,0);}l142 = l173(l1,MEM_BLOCK_ANY,index,index,l3);if(
l142>= 0){l142 = l284(l1,index,l3);}}SOC_ALPM_LPM_UNLOCK(l1);return(l142);}
static int soc_th_alpm_128_lpm_delete(int l1,void*key_data){int l133;int index
;int l5;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l142 = SOC_E_NONE;
SOC_ALPM_LPM_LOCK(l1);l142 = l9(l1,key_data,l10,&index,&l133,&l5,0,0);if(l142
== SOC_E_NONE){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"soc_th_alpm_lpm_delete: %d %d\n"),index,l133));l277(l1,key_data,index);l142 = 
l305(l1,l133,l5,l10,index);}soc_th_alpm_128_lpm_state_dump(l1);
SOC_ALPM_LPM_UNLOCK(l1);return(l142);}static int l13(int l1,void*key_data,
void*l10,int*l11){int l133;int l142;int l5;SOC_ALPM_LPM_LOCK(l1);l142 = l9(l1
,key_data,l10,l11,&l133,&l5,0,0);SOC_ALPM_LPM_UNLOCK(l1);return(l142);}static
int l4(int unit,void*key_data,int l5,int l6,int l7,defip_aux_scratch_entry_t*
l8){uint32 l144;uint32 l221[4] = {0,0,0,0};int l133 = 0;int l142 = SOC_E_NONE
;l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(key_data),(l109[(unit)]->l82));soc_mem_field32_set(unit
,L3_DEFIP_AUX_SCRATCHm,l8,VALIDf,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(unit)]->l71));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l8,MODEf,l144);l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(key_data),(l109[(unit)]->l103));soc_mem_field32_set(
unit,L3_DEFIP_AUX_SCRATCHm,l8,ENTRY_TYPEf,0);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(unit)]->l59));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l8,GLOBAL_ROUTEf,l144);l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l109[(unit)]->l56));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,ECMPf,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(unit)]->l58));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l8,ECMP_PTRf,l144);l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
unit,L3_DEFIP_PAIR_128m)),(key_data),(l109[(unit)]->l77));soc_mem_field32_set
(unit,L3_DEFIP_AUX_SCRATCHm,l8,NEXT_HOP_INDEXf,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(unit)]->l78));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l8,PRIf,l144);l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(key_data),(l109[(unit)]->l79));soc_mem_field32_set(unit
,L3_DEFIP_AUX_SCRATCHm,l8,RPEf,l144);l144 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l109[(unit)]->l86));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,VRFf,l144);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,DB_TYPEf,l6);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(unit)]->l54));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l8,DST_DISCARDf,l144);l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l109[(unit)]->l53));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,CLASS_IDf,l144);l221[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
key_data),(l109[(unit)]->l63));l221[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l109[(unit)]->l64));l221[2
] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)
),(key_data),(l109[(unit)]->l61));l221[3] = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(key_data),(l109[(unit)]->l62));
soc_mem_field_set(unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)l8,IP_ADDRf,(uint32*)
l221);l142 = l140(unit,key_data,&l133);if(SOC_FAILURE(l142)){return l142;}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,IP_LENGTHf,l133);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l8,REPLACE_LENf,l7);return(
SOC_E_NONE);}static int l14(int unit,void*lpm_entry,void*l15,void*l16,
soc_mem_t l17,uint32 l18,uint32*l314,int l20){uint32 l144;uint32 l221[4];int
l133 = 0;int l142 = SOC_E_NONE;uint32 l19 = 0;sal_memset(l15,0,
soc_mem_entry_words(unit,l17)*4);l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l60));
soc_mem_field32_set(unit,l17,l15,HITf,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l82));soc_mem_field32_set(unit,l17,l15,VALIDf,l144)
;l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l56));soc_mem_field32_set(
unit,l17,l15,ECMPf,l144);l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l58));
soc_mem_field32_set(unit,l17,l15,ECMP_PTRf,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l77));soc_mem_field32_set(unit,l17,l15,
NEXT_HOP_INDEXf,l144);l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l78));
soc_mem_field32_set(unit,l17,l15,PRIf,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l79));soc_mem_field32_set(unit,l17,l15,RPEf,l144);
l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l54));soc_mem_field32_set(
unit,l17,l15,DST_DISCARDf,l144);l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l55));
soc_mem_field32_set(unit,l17,l15,SRC_DISCARDf,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l53));soc_mem_field32_set(unit,l17,l15,CLASS_IDf,
l144);soc_mem_field32_set(unit,l17,l15,SUB_BKT_PTRf,ALPM_BKT_SIDX(l20));l221[
0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m
)),(lpm_entry),(l109[(unit)]->l63));l221[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l64));l221[2] = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l61));l221
[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l62));soc_mem_field_set(unit,
l17,(uint32*)l15,KEYf,(uint32*)l221);l142 = l140(unit,lpm_entry,&l133);if(
SOC_FAILURE(l142)){return l142;}if((l133 == 0)&&(l221[0] == 0)&&(l221[1] == 0
)&&(l221[2] == 0)&&(l221[3] == 0)){l19 = 1;}if(l314!= NULL){*l314 = l19;}
soc_mem_field32_set(unit,l17,l15,LENGTHf,l133);if(l16 == NULL){return(
SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){sal_memset(l16,0,
soc_mem_entry_words(unit,l17)*4);sal_memcpy(l16,l15,soc_mem_entry_words(unit,
l17)*4);soc_mem_field32_set(unit,l17,l16,DST_DISCARDf,0);soc_mem_field32_set(
unit,l17,l16,RPEf,0);soc_mem_field32_set(unit,l17,l16,SRC_DISCARDf,l18&
SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(unit,l17,l16,DEFAULTROUTEf,l19)
;}return(SOC_E_NONE);}static int l21(int unit,void*l15,soc_mem_t l17,int l5,
int vrf,int l22,int index,void*lpm_entry){uint32 l144;uint32 l221[4];uint32
vrf_id,l315;sal_memset(lpm_entry,0,soc_mem_entry_words(unit,
L3_DEFIP_PAIR_128m)*4);l144 = soc_mem_field32_get(unit,l17,l15,HITf);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l60),(l144));l144 = soc_mem_field32_get(unit,l17,
l15,VALIDf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l82),(l144));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l83),(l144));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l80),(l144)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l109[(unit)]->l81),(l144));l144 = soc_mem_field32_get(unit,l17,
l15,ECMPf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l56),(l144));l144 = 
soc_mem_field32_get(unit,l17,l15,ECMP_PTRf);soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l58),(
l144));l144 = soc_mem_field32_get(unit,l17,l15,NEXT_HOP_INDEXf);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l77),(l144));l144 = soc_mem_field32_get(unit,l17,
l15,PRIf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l78),(l144));l144 = 
soc_mem_field32_get(unit,l17,l15,RPEf);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l79),(l144)
);l144 = soc_mem_field32_get(unit,l17,l15,DST_DISCARDf);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l54),(l144));l144 = soc_mem_field32_get(unit,l17,
l15,SRC_DISCARDf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l55),(l144));l144 = 
soc_mem_field32_get(unit,l17,l15,CLASS_IDf);soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l53),(
l144));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l94),(ALPM_BKT_IDX(l22)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l95),(ALPM_BKT_SIDX(l22)));if(index>0){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l93),(index));}soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l75),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l76),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l73),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l74),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l99),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l100),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l101),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l102),(1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l71),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l72),(3));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l69),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l70),(3));soc_mem_field_get(unit,l17,l15,KEYf,l221)
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l63),(l221[0]));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l64),(l221
[1]));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l61),(l221[2]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l62),(l221[3]));l221[0] = l221[1] = l221[2] = l221[
3] = 0;l144 = soc_mem_field32_get(unit,l17,l15,LENGTHf);l149(unit,lpm_entry,
l144);if(vrf == SOC_L3_VRF_OVERRIDE){soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l92),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l59),(1));vrf_id = 0;l315 = 0;}else if(vrf == 
SOC_L3_VRF_GLOBAL){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l59),(1));vrf_id = 0;l315 = 0
;}else{vrf_id = vrf;l315 = SOC_VRF_MAX(unit);}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l86),(vrf_id));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l87),(
vrf_id));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l84),(vrf_id));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l85),(vrf_id));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l90),(l315)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l109[(unit)]->l91),(l315));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l88),(l315)
);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),
(lpm_entry),(l109[(unit)]->l89),(l315));return(SOC_E_NONE);}int
soc_th_alpm_128_warmboot_pivot_add(int unit,int l5,void*lpm_entry,int l316,
int bktid){int l142 = SOC_E_NONE;uint32 key[4] = {0,0,0,0};alpm_pivot_t*l31 = 
NULL;alpm_bucket_handle_t*l208 = NULL;int vrf_id = 0,vrf = 0;uint32 l317;
trie_t*l239 = NULL;uint32 prefix[5] = {0};int l19 = 0;l142 = 
_soc_th_alpm_128_prefix_create(unit,lpm_entry,prefix,&l317,&l19);
SOC_IF_ERROR_RETURN(l142);SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(
unit,lpm_entry,&vrf_id,&vrf));l316 = soc_th_alpm_physical_idx(unit,
L3_DEFIP_PAIR_128m,l316,l5);l208 = sal_alloc(sizeof(alpm_bucket_handle_t),
"ALPM Bucket Handle");if(l208 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for PIVOT trie node \n")));return SOC_E_NONE;
}sal_memset(l208,0,sizeof(*l208));l31 = sal_alloc(sizeof(alpm_pivot_t),
"Payload for Pivot");if(l31 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for PIVOT trie node \n")));sal_free(l208);
return SOC_E_MEMORY;}sal_memset(l31,0,sizeof(*l31));PIVOT_BUCKET_HANDLE(l31) = 
l208;trie_init(_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l31));key[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l63));key[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l64));key[2
] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)
),(lpm_entry),(l109[(unit)]->l61));key[3] = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l62));
PIVOT_BUCKET_INDEX(l31) = bktid;PIVOT_TCAM_INDEX(l31) = SOC_ALPM_128_ADDR_LWR
(l316)<<1;if(!(((vrf_id == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(unit)
== SOC_ALPM_MODE_TCAM_ALPM)&&(vrf_id == SOC_L3_VRF_GLOBAL))))){l239 = 
VRF_PIVOT_TRIE_IPV6_128(unit,vrf);if(l239 == NULL){trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6_128(unit,vrf));l239 = 
VRF_PIVOT_TRIE_IPV6_128(unit,vrf);}sal_memcpy(l31->key,prefix,sizeof(prefix))
;l31->len = l317;l142 = trie_insert(l239,l31->key,NULL,l31->len,(trie_node_t*
)l31);if(SOC_FAILURE(l142)){sal_free(l208);sal_free(l31);return l142;}}
ALPM_TCAM_PIVOT(unit,SOC_ALPM_128_ADDR_LWR(l316)<<1) = l31;PIVOT_BUCKET_VRF(
l31) = vrf;PIVOT_BUCKET_IPV6(l31) = l5;PIVOT_BUCKET_ENT_CNT_UPDATE(l31);if(
key[0] == 0&&key[1] == 0&&key[2] == 0&&key[3] == 0){PIVOT_BUCKET_DEF(l31) = 
TRUE;}VRF_PIVOT_REF_INC(unit,vrf,l5);return l142;}static int l318(int unit,
int l5,void*lpm_entry,void*l15,soc_mem_t l17,int l316,int bktid,int l319){int
l320;int vrf;int l142 = SOC_E_NONE;int l19 = 0;uint32 prefix[5] = {0,0,0,0,0}
;uint32 l32;defip_pair_128_entry_t l321;trie_t*l37 = NULL;trie_t*l30 = NULL;
trie_node_t*l198 = NULL;payload_t*l322 = NULL;payload_t*l219 = NULL;
alpm_pivot_t*pivot_pyld = NULL;if((NULL == lpm_entry)||(NULL == l15)){return
SOC_E_PARAM;}SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&
l320,&vrf));l17 = L3_DEFIP_ALPM_IPV6_128m;SOC_IF_ERROR_RETURN(l21(unit,l15,
l17,l5,l320,bktid,l316,&l321));l142 = _soc_th_alpm_128_prefix_create(unit,&
l321,prefix,&l32,&l19);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"prefix create failed\n")));return l142;}pivot_pyld = 
ALPM_TCAM_PIVOT(unit,l316);l37 = PIVOT_BUCKET_TRIE(pivot_pyld);l322 = 
sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == l322){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for trie node.\n"
)));return SOC_E_MEMORY;}l219 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(NULL == l219){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node\n")));sal_free(
l322);return SOC_E_MEMORY;}sal_memset(l322,0,sizeof(*l322));sal_memset(l219,0
,sizeof(*l219));sal_memcpy(l322->key,prefix,sizeof(l322->key));l322->len = 
l32;l322->index = l319;sal_memcpy(l219,l322,sizeof(*l322));l142 = trie_insert
(l37,prefix,NULL,l32,(trie_node_t*)l322);if(SOC_FAILURE(l142)){goto l323;}if(
l5){l30 = VRF_PREFIX_TRIE_IPV6_128(unit,vrf);}if(!l19){l142 = trie_insert(l30
,prefix,NULL,l32,(trie_node_t*)l219);if(SOC_FAILURE(l142)){goto l324;}}return
l142;l324:(void)trie_delete(l37,prefix,l32,&l198);l322 = (payload_t*)l198;
l323:sal_free(l322);sal_free(l219);return l142;}static int l325(int unit,int
l27,int vrf,int l241,int bkt_ptr){int l142 = SOC_E_NONE;uint32 l32;uint32 key
[5] = {0,0,0,0,0};trie_t*l326 = NULL;payload_t*l229 = NULL;
defip_pair_128_entry_t*lpm_entry = NULL;lpm_entry = sal_alloc(sizeof(
defip_pair_128_entry_t),"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unable to allocate memory for LPM entry\n")
));return SOC_E_MEMORY;}l23(unit,key,0,vrf,l27,lpm_entry,0,1);if(vrf == 
SOC_VRF_MAX(unit)+1){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l59),(1));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(
lpm_entry),(l109[(unit)]->l96),(1));}soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l94),(
ALPM_BKT_IDX(bkt_ptr)));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit
,L3_DEFIP_PAIR_128m)),(lpm_entry),(l109[(unit)]->l95),(ALPM_BKT_SIDX(bkt_ptr)
));VRF_TRIE_DEFAULT_ROUTE_IPV6_128(unit,vrf) = lpm_entry;trie_init(
_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6_128(unit,vrf));l326 = 
VRF_PREFIX_TRIE_IPV6_128(unit,vrf);l229 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l229 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l229,0,sizeof(*l229));l229->key[0] = key[0];l229->
key[1] = key[1];l229->len = l32 = 0;l142 = trie_insert(l326,key,NULL,l32,&(
l229->node));if(SOC_FAILURE(l142)){sal_free(l229);return l142;}
VRF_TRIE_INIT_DONE(unit,vrf,l27,1);return l142;}int
soc_th_alpm_128_warmboot_prefix_insert(int unit,int l5,void*lpm_entry,void*
l15,int l316,int bktid,int l319){int l320;int vrf;int l142 = SOC_E_NONE;
soc_mem_t l17;l316 = soc_th_alpm_physical_idx(unit,L3_DEFIP_PAIR_128m,l316,l5
);l316 = SOC_ALPM_128_ADDR_LWR(l316)<<1;l17 = L3_DEFIP_ALPM_IPV6_128m;
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_vrf_get(unit,lpm_entry,&l320,&vrf));
if(((l320 == SOC_L3_VRF_OVERRIDE)||((soc_th_alpm_mode_get(unit) == 
SOC_ALPM_MODE_TCAM_ALPM)&&(l320 == SOC_L3_VRF_GLOBAL)))){return(l142);}if(!
VRF_TRIE_INIT_COMPLETED(unit,vrf,l5)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U
(unit,"VRF %d is not initialized\n"),vrf));l142 = l325(unit,l5,vrf,l316,bktid
);if(SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d/%d trie init \n""failed\n"),vrf,l5));return l142;}}l142 = l318(unit,
l5,lpm_entry,l15,l17,l316,bktid,l319);if(l142!= SOC_E_NONE){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unit %d : ""Route Insertion Failed :%s\n"),
unit,soc_errmsg(l142)));return(l142);}soc_th_alpm_bu_upd(unit,bktid,l316,vrf,
l5,1);VRF_TRIE_ROUTES_INC(unit,vrf,l5);return(l142);}int
soc_th_alpm_128_warmboot_bucket_bitmap_set(int l1,int vrf,int l27,int l244){
int l327 = 1;SHR_BITDCL*l328 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l1);if(vrf == 
SOC_VRF_MAX(l1)+1){l328 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l1);}if(
SOC_TH_ALPM_SCALE_CHECK(l1,l27)){l327 = 2;}SHR_BITSET_RANGE(l328,l244,l327);
return SOC_E_NONE;}int soc_th_alpm_128_warmboot_lpm_reinit_done(int unit){int
l241;int l329 = ((3*(128+2+1))-1);int l311 = soc_mem_index_count(unit,
L3_DEFIP_PAIR_128m);if(SOC_URPF_STATUS_GET(unit)){l311>>= 1;}if(
soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_COMBINED){(l51[(unit)][(((3*(128+
2+1))-1))].l46) = -1;for(l241 = ((3*(128+2+1))-1);l241>-1;l241--){if(-1 == (
l51[(unit)][(l241)].l44)){continue;}(l51[(unit)][(l241)].l46) = l329;(l51[(
unit)][(l329)].next) = l241;(l51[(unit)][(l329)].l48) = (l51[(unit)][(l241)].
l44)-(l51[(unit)][(l329)].l45)-1;l329 = l241;}(l51[(unit)][(l329)].next) = -1
;(l51[(unit)][(l329)].l48) = l311-(l51[(unit)][(l329)].l45)-1;}else{(l51[(
unit)][(((3*(128+2+1))-1))].l46) = -1;for(l241 = ((3*(128+2+1))-1);l241>(((3*
(128+2+1))-1)/3);l241--){if(-1 == (l51[(unit)][(l241)].l44)){continue;}(l51[(
unit)][(l241)].l46) = l329;(l51[(unit)][(l329)].next) = l241;(l51[(unit)][(
l329)].l48) = (l51[(unit)][(l241)].l44)-(l51[(unit)][(l329)].l45)-1;l329 = 
l241;}(l51[(unit)][(l329)].next) = -1;(l51[(unit)][(l329)].l48) = l311-(l51[(
unit)][(l329)].l45)-1;l329 = (((3*(128+2+1))-1)/3);(l51[(unit)][((((3*(128+2+
1))-1)/3))].l46) = -1;for(l241 = ((((3*(128+2+1))-1)/3)-1);l241>-1;l241--){if
(-1 == (l51[(unit)][(l241)].l44)){continue;}(l51[(unit)][(l241)].l46) = l329;
(l51[(unit)][(l329)].next) = l241;(l51[(unit)][(l329)].l48) = (l51[(unit)][(
l241)].l44)-(l51[(unit)][(l329)].l45)-1;l329 = l241;}(l51[(unit)][(l329)].
next) = -1;(l51[(unit)][(l329)].l48) = (l311>>1)-(l51[(unit)][(l329)].l45)-1;
}return(SOC_E_NONE);}int soc_th_alpm_128_warmboot_lpm_reinit(int unit,int l5,
int l241,void*lpm_entry){int l12;l274(unit,lpm_entry,l241,0x4000,0);
SOC_IF_ERROR_RETURN(l139(unit,lpm_entry,&l12,0,0));if((l51[(unit)][(l12)].l47
) == 0){(l51[(unit)][(l12)].l44) = l241;(l51[(unit)][(l12)].l45) = l241;}else
{(l51[(unit)][(l12)].l45) = l241;}(l51[(unit)][(l12)].l47)++;return(
SOC_E_NONE);}int soc_th_alpm_128_bucket_sanity_check(int l1,soc_mem_t l178,
int index){int l142 = SOC_E_NONE;int l141,l249,l258,l5,tcam_index = -1;int
vrf_id = 0,vrf;uint32 l156 = 0;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int l330,
l331,l332;defip_pair_128_entry_t lpm_entry;int l263,l333;soc_mem_t l17 = 
L3_DEFIP_ALPM_IPV6_128m;int l334 = 8,bkt_ptr,l335;int l336 = 0;soc_field_t
l337 = VALID0_LWRf;soc_field_t l338 = GLOBAL_HIGHf;soc_field_t l339 = 
ALG_BKT_PTRf;soc_field_t l340 = ALG_SUB_BKT_PTRf;l5 = L3_DEFIP_MODE_128;l263 = 
soc_mem_index_min(l1,l178);l333 = soc_mem_index_max(l1,l178);if((index>= 0)&&
(index<l263||index>l333)){return SOC_E_PARAM;}else if(index>= 0){l263 = index
;l333 = index;}SOC_ALPM_LPM_LOCK(l1);for(l141 = l263;l141<= l333;l141++){l142
= soc_mem_read(l1,l178,MEM_BLOCK_ANY,l141,(void*)l10);if(SOC_FAILURE(l142)){
continue;}if(soc_mem_field32_get(l1,l178,(void*)l10,l337) == 0||
soc_mem_field32_get(l1,l178,(void*)l10,l338) == 1){continue;}bkt_ptr = 
soc_mem_field32_get(l1,l178,(void*)l10,l339);l335 = soc_mem_field32_get(l1,
l178,(void*)l10,l340);l142 = soc_th_alpm_128_lpm_vrf_get(l1,l10,&vrf_id,&vrf)
;if(SOC_FAILURE(l142)){continue;}if(SOC_TH_ALPM_SCALE_CHECK(l1,l5)){l334<<= 1
;}tcam_index = -1;for(l249 = 0;l249<l334;l249++){l142 = 
_soc_th_alpm_mem_index(l1,l17,bkt_ptr,l249,l156,&l258);if(SOC_FAILURE(l142)){
continue;}l142 = soc_mem_read(l1,l17,MEM_BLOCK_ANY,l258,(void*)l10);if(
SOC_FAILURE(l142)){break;}if(!soc_mem_field32_get(l1,l17,(void*)l10,VALIDf)){
continue;}if(l335!= soc_mem_field32_get(l1,l17,(void*)l10,SUB_BKT_PTRf)){
continue;}l142 = l21(l1,(void*)l10,l17,l5,vrf_id,ALPM_BKTID(bkt_ptr,l335),0,&
lpm_entry);if(SOC_FAILURE(l142)){continue;}l142 = _soc_th_alpm_128_find(l1,
l17,(void*)&lpm_entry,vrf_id,vrf,(void*)l10,&l330,&l331,&l332,FALSE);if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tLaunched AUX operation for "
"index %d bucket [%d,%d] sanity check failed\n"),l141,bkt_ptr,l335));l336++;
continue;}if(l331!= ALPM_BKTID(bkt_ptr,l335)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tEntry at index %d does not belong "
"to bucket [%d,%d](from bucket [%d,%d])\n"),l332,bkt_ptr,l335,ALPM_BKT_IDX(
l331),ALPM_BKT_SIDX(l331)));l336++;}if(tcam_index == -1){tcam_index = l330;
continue;}if(tcam_index!= l330){int l161,l162;l161 = soc_th_alpm_logical_idx(
l1,l178,SOC_ALPM_128_DEFIP_TO_PAIR(tcam_index>>1),1);l162 = 
soc_th_alpm_logical_idx(l1,l178,SOC_ALPM_128_DEFIP_TO_PAIR(l330>>1),1);
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tAliased bucket [%d,%d](returned "
"bucket [%d,%d]) found from TCAM1 %d and TCAM2 %d\n"),bkt_ptr,l335,
ALPM_BKT_IDX(l331),ALPM_BKT_SIDX(l331),l161,l162));l336++;}}}
SOC_ALPM_LPM_UNLOCK(l1);if(l336 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l1
,"\tMemory %s index %d Bucket sanity check passed\n"),SOC_MEM_NAME(l1,l178),
index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l1,l178),index,l336));return l336;}
int soc_th_alpm_128_pivot_sanity_check(int l1,soc_mem_t l178,int index){int
l141,l341;int l258,l263,l333;int l142 = SOC_E_NONE;int*l342 = NULL;int l330,
l331,l332;uint32 l10[SOC_MAX_MEM_FIELD_WORDS];int bkt_ptr,l335,l343,l344;
soc_mem_t l345;int vrf,l346;soc_mem_t l347 = L3_DEFIP_ALPM_IPV6_128m;int l5 = 
L3_DEFIP_MODE_128;int l336 = 0;soc_field_t l337 = VALID0_LWRf;soc_field_t l338
= GLOBAL_HIGHf;soc_field_t l348 = NEXT_HOP_INDEXf;soc_field_t l339 = 
ALG_BKT_PTRf;soc_field_t l340 = ALG_SUB_BKT_PTRf;l263 = soc_mem_index_min(l1,
l178);l333 = soc_mem_index_max(l1,l178);if((index>= 0)&&(index<l263||index>
l333)){return SOC_E_PARAM;}else if(index>= 0){l263 = index;l333 = index;}l141
= sizeof(int)*SOC_TH_MAX_ALPM_BUCKETS*SOC_TH_MAX_SUB_BUCKETS;l342 = sal_alloc
(l141,"Bucket index array 128");if(l342 == NULL){l142 = SOC_E_MEMORY;return
l142;}sal_memset(l342,0xff,l141);SOC_ALPM_LPM_LOCK(l1);for(l141 = l263;l141<= 
l333;l141++){l142 = soc_mem_read(l1,l178,MEM_BLOCK_ANY,l141,(void*)l10);if(
SOC_FAILURE(l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l1,l178),
l141,l142));l336 = l142;continue;}if(soc_mem_field32_get(l1,l178,(void*)l10,
l337) == 0||soc_mem_field32_get(l1,l178,(void*)l10,l338) == 1){continue;}
bkt_ptr = soc_mem_field32_get(l1,l178,(void*)l10,l339);l335 = 
soc_mem_field32_get(l1,l178,(void*)l10,l340);(void)soc_th_alpm_lpm_vrf_get(l1
,l10,&vrf,&l346);l142 = soc_th_alpm_bucket_is_assigned(l1,bkt_ptr,l346,l5,&
l341);if(l142 == SOC_E_PARAM||l335>= SOC_TH_MAX_SUB_BUCKETS){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tInvalid bucket pointer [%d,%d] "
"detected, in memory %s index %d\n"),bkt_ptr,l335,SOC_MEM_NAME(l1,l178),l141)
);l336 = l142;continue;}if(bkt_ptr!= 0){if(l342[bkt_ptr*
SOC_TH_MAX_SUB_BUCKETS+l335] == -1){l342[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l335]
= l141;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tDuplicated bucket pointer ""[%d,%d]detected, in memory %s index1 %d and "
"index2 %d\n"),bkt_ptr,l335,SOC_MEM_NAME(l1,l178),l342[bkt_ptr*
SOC_TH_MAX_SUB_BUCKETS+l335],l141));l336 = l142;continue;}}else{continue;}if(
l142>= 0&&l341 == 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tFreed bucket pointer %d ""detected, in memory %s index %d\n"),bkt_ptr,
SOC_MEM_NAME(l1,l178),l141));l336 = l142;continue;}l345 = 
_soc_tomahawk_alpm_bkt_view_get(l1,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l1,ALPM_BKTID
(bkt_ptr,0)));if(l345!= L3_DEFIP_ALPM_IPV6_128m){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual %s\n"),bkt_ptr,SOC_MEM_NAME(l1,
L3_DEFIP_ALPM_IPV6_128m),SOC_MEM_NAME(l1,l345)));l336 = l142;continue;}l330 = 
-1;l343 = soc_mem_field32_get(l1,l178,(void*)l10,l348);l142 = 
_soc_th_alpm_128_find(l1,l347,l10,vrf,l346,(void*)l10,&l330,&l331,&l332,FALSE
);if(l330 == -1){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l1,
"\tLaunched AUX operation for PIVOT ""index %d sanity check failed\n"),l141))
;l336 = l142;continue;}l258 = soc_th_alpm_logical_idx(l1,l178,
SOC_ALPM_128_DEFIP_TO_PAIR(l330>>1),1);l142 = soc_mem_read(l1,l178,
MEM_BLOCK_ANY,l258,(void*)l10);if(SOC_FAILURE(l142)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tRead memory %s index %d (nexthop) "
"return %d\n"),SOC_MEM_NAME(l1,l178),l258,l142));l336 = l142;continue;}l344 = 
soc_mem_field32_get(l1,l178,(void*)l10,l348);if(l343!= l344){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l1,"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d index2 %d \n"),l141,l258));l336 = l142;continue;}}
SOC_ALPM_LPM_UNLOCK(l1);sal_free(l342);if(l336 == 0){LOG_INFO(BSL_LS_SOC_ALPM
,(BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check passed\n"),
SOC_MEM_NAME(l1,l178),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l1,"\tMemory %s index %d Pivot sanity check "
"failed, encountered %d error(s)\n"),SOC_MEM_NAME(l1,l178),index,l336));
return SOC_E_FAIL;}
#endif /* ALPM_ENABLE */
