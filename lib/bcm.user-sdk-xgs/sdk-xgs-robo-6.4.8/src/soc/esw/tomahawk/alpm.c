/*
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
#include <soc/tomahawk.h>
#include <shared/bsl.h>

#ifdef ALPM_ENABLE
#include <shared/util.h>
#include <shared/l3.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#include <soc/esw/trie.h>
typedef struct l1{int l2;int l3;int l4;int next;int l5;int l6;}l7,*l8;static
l8 l9[SOC_MAX_NUM_DEVICES];typedef struct l10{soc_field_info_t*l11;
soc_field_info_t*l12;soc_field_info_t*l13;soc_field_info_t*l14;
soc_field_info_t*l15;soc_field_info_t*l16;soc_field_info_t*l17;
soc_field_info_t*l18;soc_field_info_t*l19;soc_field_info_t*l20;
soc_field_info_t*l21;soc_field_info_t*l22;soc_field_info_t*l23;
soc_field_info_t*l24;soc_field_info_t*l25;soc_field_info_t*l26;
soc_field_info_t*l27;soc_field_info_t*l28;soc_field_info_t*l29;
soc_field_info_t*l30;soc_field_info_t*l31;soc_field_info_t*l32;
soc_field_info_t*l33;soc_field_info_t*l34;soc_field_info_t*l35;
soc_field_info_t*l36;soc_field_info_t*l37;soc_field_info_t*l38;
soc_field_info_t*l39;soc_field_info_t*l40;soc_field_info_t*l41;
soc_field_info_t*l42;soc_field_info_t*l43;soc_field_info_t*l44;
soc_field_info_t*l45;soc_field_info_t*l46;soc_field_info_t*l47;
soc_field_info_t*l48;soc_field_info_t*l49;soc_field_info_t*l50;
soc_field_info_t*l51;soc_field_info_t*l52;soc_field_info_t*l53;
soc_field_info_t*l54;soc_field_info_t*l55;soc_field_info_t*l56;
soc_field_info_t*l57;soc_field_info_t*l58;soc_field_info_t*l59;
soc_field_info_t*l60;soc_field_info_t*l61;soc_field_info_t*l62;
soc_field_info_t*l63;soc_field_info_t*l64;}l65,*l66;static l66 l67[
SOC_MAX_NUM_DEVICES];typedef struct l68{int unit;int l69;int l70;uint16*l71;
uint16*l72;}l73;typedef uint32 l74[5];typedef int(*l75)(l74 l76,l74 l77);
static l73*l78[SOC_MAX_NUM_DEVICES];soc_alpm_bucket_t soc_th_alpm_bucket[
SOC_MAX_NUM_DEVICES];int l3_alpm_ipv4_double_wide[SOC_MAX_NUM_DEVICES];extern
int l3_alpm_sw_prefix_lookup[SOC_MAX_NUM_DEVICES];alpm_bkt_usg_t*bkt_usage[
SOC_MAX_NUM_DEVICES];alpm_bkt_bmp_t*global_bkt_usage[SOC_MAX_NUM_DEVICES][
SOC_TH_MAX_ALPM_VIEWS];int alpm_split_count = 0;int alpm_merge_count = 0;
extern int alpm_128_split_count;static void l79(int l80,void*l81,int index,
l74 l82);static uint16 l83(uint8*l84,int l85);static int l86(int unit,int l69
,int l70,l73**l87);static int l88(l73*l89);static int l90(l73*l91,l75 l92,l74
entry,int l93,uint16*l94);static int l95(l73*l91,l75 l92,l74 entry,int l93,
uint16 l96,uint16 l97);static int l98(l73*l91,l75 l92,l74 entry,int l93,
uint16 l99);static int l100(l73*l91,l75 l92,l74 entry,int l93,uint16 index);
static int l101(int,int,void*,int*,int*,int*);static int l102(int l80);static
int l103(int l80);static int l104(int l80,void*l105,int*index);static int
soc_th_alpm_lpm_delete(int l80,void*key_data);static int l106(int l80,void*
key_data,int l107,int l108,int l109,int l110,defip_aux_scratch_entry_t*l111);
static int l112(int l80,void*key_data,void*l81,int*l113,int*l114,int*l107,int
*vrf_id,int*vrf);static int l115(int l80,void*key_data,void*l81,int*l113);
static int l116(int l80);static int l117(int unit,int l107,void*lpm_entry,
void*l118,void*l119,soc_mem_t l120,uint32 l121,uint32*l122,int l123);static
int l124(int unit,void*l118,soc_mem_t l120,int l107,int vrf,int l125,int index
,void*lpm_entry);static int l126(int unit,uint32*key,int len,int vrf,int l107
,defip_entry_t*lpm_entry,int l127,int l128);static int l129(int l80,int vrf,
int l130);static int _soc_th_alpm_move_inval(int l80,soc_mem_t l120,int l130,
alpm_mem_prefix_array_t*l131,int*l97);int soc_th_alpm_lpm_vrf_get(int unit,
void*lpm_entry,int*vrf,int*l132);int soc_th_alpm_lpm_delete(int l80,void*
key_data);extern alpm_vrf_handle_t*alpm_vrf_handle[SOC_MAX_NUM_DEVICES];
extern alpm_pivot_t**tcam_pivot[SOC_MAX_NUM_DEVICES];extern int
soc_tomahawk_alpm_mode_get(int unit);extern int soc_th_get_alpm_banks(int unit
);extern int _soc_th_alpm_128_prefix_create(int l80,void*entry,uint32*l133,
uint32*l114,int*l122);void soc_th_alpm_lpm_move_bu_upd(int l80,int l123,int
l134){alpm_bkt_usg_t*l135;int bkt_ptr = ALPM_BKT_IDX(l123);int l136 = 
ALPM_BKT_SIDX(l123);l135 = &bkt_usage[l80][bkt_ptr];l135->pivots[l136] = l134
;l135->vrf_type[l136] = PIVOT_BUCKET_VRF(ALPM_TCAM_PIVOT(l80,l134));}int
soc_th_alpm_mode_get(int l80){int l137;l137 = soc_tomahawk_alpm_mode_get(l80)
;switch(l137){case 1:return SOC_ALPM_MODE_PARALLEL;case 3:return
SOC_ALPM_MODE_TCAM_ALPM;case 2:default:return SOC_ALPM_MODE_COMBINED;}return
SOC_ALPM_MODE_COMBINED;}static int l138(int l80,int l107,void*entry,int*l93){
int l139;uint32 l140;if(l107){l140 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l27));if((l139 = 
_ipmask2pfx(l140,l93))<0){return(l139);}l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l28));if(*l93){if(l140!= 0xffffffff){return(SOC_E_PARAM);}*l93+= 32;
}else{if((l139 = _ipmask2pfx(l140,l93))<0){return(l139);}}}else{l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l27));if((l139 = _ipmask2pfx(l140,l93))<0){return(l139);}}return
SOC_E_NONE;}void soc_th_alpm_bank_disb_get(int l80,int vrf,uint32*l141){if(
vrf == SOC_VRF_MAX(l80)+1){if(soc_th_get_alpm_banks(l80)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l80,*l141);}else{
SOC_ALPM_TH_GET_GLOBAL_BANK_DISABLE(l80,*l141);}}else{if(
soc_th_get_alpm_banks(l80)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(
l80,*l141);}else{SOC_ALPM_TH_GET_VRF_BANK_DISABLE(l80,*l141);}}}int
_soc_th_alpm_rpf_entry(int l80,int l142){int l143;int l144 = 
soc_th_get_alpm_banks(l80)/2;l143 = (l142>>l144)&SOC_TH_ALPM_BKT_MASK;l143+= 
SOC_TH_ALPM_BUCKET_COUNT(l80);return(l142&~(SOC_TH_ALPM_BKT_MASK<<l144))|(
l143<<l144);}int soc_th_alpm_physical_idx(int l80,soc_mem_t l120,int index,
int l145){int l146 = index&1;if(l145){return soc_trident2_l3_defip_index_map(
l80,l120,index);}index>>= 1;index = soc_trident2_l3_defip_index_map(l80,l120,
index);index<<= 1;index|= l146;return index;}int soc_th_alpm_logical_idx(int
l80,soc_mem_t l120,int index,int l145){int l146 = index&1;if(l145){return
soc_trident2_l3_defip_index_remap(l80,l120,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_remap(l80,l120,index);index<<= 1;index|= l146;
return index;}void l147(int l80,void*lpm_entry,int len,int l107){uint32 l148 = 
0;if(l107){if(len>= 32){l148 = 0xffffffff;soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l28),(l148));l148 = ~(
((len-32) == 32)?0:(0xffffffff)>>(len-32));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l27),(l148));}else{
l148 = ~(0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l28),(l148));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l27),(0));}}else{assert(len<= 32);l148 = (len == 32)?0xffffffff:
~(0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l27),(l148));}}static int l149(int l80,
int l107,void*entry,uint32*prefix,uint32*l114,int*l122){int l150,l151;int l93
= 0;int l139 = SOC_E_NONE;uint32 l152,l146;prefix[0] = prefix[1] = prefix[2] = 
prefix[3] = prefix[4] = 0;l150 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l25));l151 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l27));prefix[1] = l150;l150 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l26));l151 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l28));prefix[0] = l150;if(l107){prefix[4] = prefix[1];prefix[3] = 
prefix[0];prefix[1] = prefix[0] = 0;l151 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l27));if((l139 = 
_ipmask2pfx(l151,&l93))<0){return(l139);}l151 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l28));if(l93){if(l151!= 0xffffffff){return(SOC_E_PARAM);}l93+= 32;}
else{if((l139 = _ipmask2pfx(l151,&l93))<0){return(l139);}}l152 = 64-l93;if(
l152<32){prefix[4]>>= l152;l146 = (((32-l152) == 32)?0:(prefix[3])<<(32-l152)
);prefix[3]>>= l152;prefix[4]|= l146;}else{prefix[4] = (((l152-32) == 32)?0:(
prefix[3])>>(l152-32));prefix[3] = 0;}}else{l151 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l27));if((l139 = _ipmask2pfx(l151,&l93))<0){return(l139);}prefix[1] = 
(((32-l93) == 32)?0:(prefix[1])>>(32-l93));prefix[0] = 0;}*l114 = l93;*l122 = 
(prefix[0] == 0)&&(prefix[1] == 0)&&(l93 == 0);return SOC_E_NONE;}int
_soc_th_alpm_find_in_bkt(int l80,soc_mem_t l120,int bktid,int l141,uint32*l81
,void*alpm_data,int*l94,int l130){int l139;l139 = soc_mem_alpm_lookup(l80,
l120,ALPM_BKT_IDX(bktid),MEM_BLOCK_ANY,l141,l81,alpm_data,l94);if(SOC_SUCCESS
(l139)){return l139;}if(SOC_TH_ALPM_SCALE_CHECK(l80,l130)){return
soc_mem_alpm_lookup(l80,l120,ALPM_BKT_IDX(bktid)+1,MEM_BLOCK_ANY,l141,l81,
alpm_data,l94);}return l139;}static int l153(int l80,uint32*prefix,uint32 l154
,int l130,int vrf,int*l155,int*tcam_index,int*bktid){int l139 = SOC_E_NONE;
trie_t*l156;trie_node_t*l157 = NULL;alpm_pivot_t*pivot_pyld;if(l130){l156 = 
VRF_PIVOT_TRIE_IPV6(l80,vrf);}else{l156 = VRF_PIVOT_TRIE_IPV4(l80,vrf);}l139 = 
trie_find_lpm(l156,prefix,l154,&l157);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Pivot find failed\n")));return l139;}
pivot_pyld = (alpm_pivot_t*)l157;*l155 = 1;*tcam_index = PIVOT_TCAM_INDEX(
pivot_pyld);*bktid = PIVOT_BUCKET_INDEX(pivot_pyld);return SOC_E_NONE;}static
int _soc_th_alpm_find(int l80,soc_mem_t l120,int l130,void*key_data,int vrf_id
,int vrf,void*alpm_data,int*tcam_index,int*bktid,int*l113,int l158){uint32 l81
[SOC_MAX_MEM_FIELD_WORDS];int l94;uint32 l108,l109,l141;int l139 = SOC_E_NONE
;int l155 = 0;if(vrf_id == 0){if(soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_PARALLEL||soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_TCAM_ALPM)
{return SOC_E_PARAM;}}soc_th_alpm_bank_disb_get(l80,vrf,&l141);
soc_alpm_db_ent_type_encoding(l80,vrf,&l108,&l109);if(!(ALPM_PREFIX_IN_TCAM(
l80,vrf_id))){if(l158){uint32 prefix[5],l154;int l122 = 0;l139 = l149(l80,
l130,key_data,prefix,&l154,&l122);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_insert: prefix create failed\n")))
;return l139;}SOC_IF_ERROR_RETURN(l153(l80,prefix,l154,l130,vrf,&l155,
tcam_index,bktid));}else{defip_aux_scratch_entry_t l111;sal_memset(&l111,0,
sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l106(l80,key_data,l130
,l108,l109,0,&l111));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l80,
PREFIX_LOOKUP,&l111,TRUE,&l155,tcam_index,bktid));}if(l155){l117(l80,l130,
key_data,l81,0,l120,0,0,*bktid);l139 = _soc_th_alpm_find_in_bkt(l80,l120,*
bktid,l141,l81,alpm_data,&l94,l130);if(SOC_SUCCESS(l139)){*l113 = l94;}}else{
l139 = SOC_E_NOT_FOUND;}}return l139;}static int l159(int l80,int l130,void*
key_data,void*alpm_data,void*alpm_sip_data,soc_mem_t l120,int l94){
defip_aux_scratch_entry_t l111;int vrf_id,vrf;int bktid;uint32 l108,l109,l141
;int l139 = SOC_E_NONE;int l155 = 0,l146 = 0;int tcam_index,index;uint32 l160
[SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,
key_data,&vrf_id,&vrf));soc_th_alpm_bank_disb_get(l80,vrf,&l141);
soc_alpm_db_ent_type_encoding(l80,vrf,&l108,&l109);if(!(ALPM_PREFIX_IN_TCAM(
l80,vrf_id))){sal_memset(&l111,0,sizeof(defip_aux_scratch_entry_t));
SOC_IF_ERROR_RETURN(l106(l80,key_data,l130,l108,l109,0,&l111));
SOC_ALPM_LPM_LOCK(l80);l139 = _soc_th_alpm_find(l80,l120,l130,key_data,vrf_id
,vrf,l160,&tcam_index,&bktid,&index,l3_alpm_sw_prefix_lookup[l80]);
SOC_ALPM_LPM_UNLOCK(l80);SOC_IF_ERROR_RETURN(l139);soc_mem_field32_set(l80,
l120,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));SOC_IF_ERROR_RETURN(
soc_mem_write(l80,l120,MEM_BLOCK_ANY,ALPM_ENT_INDEX(l94),alpm_data));if(
SOC_URPF_STATUS_GET(l80)){soc_mem_field32_set(l80,l120,alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));SOC_IF_ERROR_RETURN(soc_mem_write(l80,l120
,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,ALPM_ENT_INDEX(l94)),alpm_sip_data)
);if(l139!= SOC_E_NONE){return SOC_E_FAIL;}}l146 = soc_mem_field32_get(l80,
L3_DEFIP_AUX_SCRATCHm,&l111,IP_LENGTHf);soc_mem_field32_set(l80,
L3_DEFIP_AUX_SCRATCHm,&l111,REPLACE_LENf,l146);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l80,DELETE_PROPAGATE,&l111,TRUE,&l155,&tcam_index,&bktid)
);if(SOC_URPF_STATUS_GET(l80)){if(l146 == 0){soc_mem_field32_set(l80,
L3_DEFIP_AUX_SCRATCHm,&l111,RPEf,1);}else{soc_mem_field32_set(l80,
L3_DEFIP_AUX_SCRATCHm,&l111,RPEf,0);}soc_mem_field32_set(l80,
L3_DEFIP_AUX_SCRATCHm,&l111,DB_TYPEf,l108+1);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l80,DELETE_PROPAGATE,&l111,TRUE,&l155,&tcam_index,&bktid)
);}}return l139;}int soc_th_alpm_update_hit_bits(int l80,int count,int*l161,
int*l162){int l139 = SOC_E_NONE;int l163,l164,l165,l166,l167;int index;
defip_aux_hitbit_update_entry_t l168;soc_mem_t l120 = 
L3_DEFIP_AUX_HITBIT_UPDATEm;int l169 = soc_mem_index_count(l80,l120);l165 = (
count+l169-1)/l169;for(l164 = 0;l164<l165;l164++){l166 = l164*l169;l167 = (
count>(l164+1)*l169)?(l164+1)*l169:count;l167 = l167-1;for(l163 = l166;l163<= 
l167;l163++){index = l163%l169;if(l161[l163]<= 0){continue;}sal_memset(&l168,
0,sizeof(l168));soc_mem_field32_set(l80,l120,&l168,HITBIT_PTRf,l161[l163]);
soc_mem_field32_set(l80,l120,&l168,REPLACEMENT_PTRf,l162[l163]);
soc_mem_field32_set(l80,l120,&l168,VALIDf,1);SOC_IF_ERROR_RETURN(
soc_mem_write(l80,l120,MEM_BLOCK_ANY,index,&l168));}SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l80,HITBIT_REPLACE,NULL,FALSE,NULL,NULL,NULL));}if(!
SOC_URPF_STATUS_GET(l80)){return l139;}for(l164 = 0;l164<l165;l164++){l166 = 
l164*l169;l167 = (count>(l164+1)*l169)?(l164+1)*l169:count;l167 = l167-1;for(
l163 = l166;l163<= l167;l163++){index = l163%l169;if(l161[l163]<= 0){continue
;}sal_memset(&l168,0,sizeof(l168));soc_mem_field32_set(l80,l120,&l168,
HITBIT_PTRf,_soc_th_alpm_rpf_entry(l80,l161[l163]));soc_mem_field32_set(l80,
l120,&l168,REPLACEMENT_PTRf,_soc_th_alpm_rpf_entry(l80,l162[l163]));
soc_mem_field32_set(l80,l120,&l168,VALIDf,1);SOC_IF_ERROR_RETURN(
soc_mem_write(l80,l120,MEM_BLOCK_ANY,index,&l168));}SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l80,HITBIT_REPLACE,NULL,FALSE,NULL,NULL,NULL));}return
l139;}int _soc_th_alpm_mem_prefix_array_cb(trie_node_t*node,void*l170){
alpm_mem_prefix_array_t*l171 = (alpm_mem_prefix_array_t*)l170;if(node->type == 
PAYLOAD){l171->prefix[l171->count] = (payload_t*)node;l171->count++;}return
SOC_E_NONE;}int _soc_th_alpm_bkt_entry_cnt(int l80,int l130){int l172 = 0;
switch(l130){case L3_DEFIP_MODE_V4:l172 = ALPM_IPV4_BKT_COUNT;break;case
L3_DEFIP_MODE_64:l172 = ALPM_IPV6_64_BKT_COUNT;break;case L3_DEFIP_MODE_128:
l172 = ALPM_IPV6_128_BKT_COUNT;break;default:l172 = ALPM_IPV4_BKT_COUNT;break
;}if(SOC_TH_ALPM_SCALE_CHECK(l80,l130)){l172<<= 1;}if(soc_th_get_alpm_banks(
l80)<= 2){l172>>= 1;}if(soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_PARALLEL&&
SOC_URPF_STATUS_GET(l80)){l172>>= 1;}return l172;}static int l173(int l80,int
l2,alpm_bkt_bmp_t*l174){int l163;for(l163 = l2+1;l163<SOC_TH_MAX_ALPM_BUCKETS
;l163++){if(SHR_BITGET(l174->bkt_bmp,l163)){return l163;}}return-1;}static int
l175(int l80,int l130,int l134,int bktid){int l139 = SOC_E_NONE;int l176;
soc_mem_t l177 = L3_DEFIPm;defip_entry_t lpm_entry;int l178,l179;l178 = 
ALPM_BKT_IDX(bktid);l179 = ALPM_BKT_SIDX(bktid);l176 = 
soc_th_alpm_logical_idx(l80,l177,l134>>1,1);l139 = soc_mem_read(l80,l177,
MEM_BLOCK_ANY,l176,&lpm_entry);SOC_IF_ERROR_RETURN(l139);if((!l130)&&(l134&1)
){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(&
lpm_entry),(l67[(l80)]->l50),(l178));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(&lpm_entry),(l67[(l80)]->l52),(l179));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(&lpm_entry)
,(l67[(l80)]->l49),(l178));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(&lpm_entry),(l67[(l80)]->l51),(l179));}l139 = soc_mem_write(
l80,l177,MEM_BLOCK_ANY,l176,&lpm_entry);SOC_IF_ERROR_RETURN(l139);if(
SOC_URPF_STATUS_GET(l80)){int l180 = soc_th_alpm_logical_idx(l80,l177,l134>>1
,1)+(soc_mem_index_count(l80,l177)>>1);l139 = READ_L3_DEFIPm(l80,
MEM_BLOCK_ANY,l180,&lpm_entry);SOC_IF_ERROR_RETURN(l139);if((!l130)&&(l134&1)
){soc_mem_field32_set(l80,l177,&lpm_entry,ALG_BKT_PTR1f,l178+
SOC_TH_ALPM_BUCKET_COUNT(l80));soc_mem_field32_set(l80,l177,&lpm_entry,
ALG_SUB_BKT_PTR1f,l179);}else{soc_mem_field32_set(l80,l177,&lpm_entry,
ALG_BKT_PTR0f,l178+SOC_TH_ALPM_BUCKET_COUNT(l80));soc_mem_field32_set(l80,
l177,&lpm_entry,ALG_SUB_BKT_PTR0f,l179);}l139 = WRITE_L3_DEFIPm(l80,
MEM_BLOCK_ANY,l180,&lpm_entry);SOC_IF_ERROR_RETURN(l139);}return l139;}static
int _soc_th_alpm_move_trie(int l80,int l130,int l181,int l182){int*l97 = NULL
,*l96 = NULL;int l139 = SOC_E_NONE,l183,l163,l184;int l94,l134;uint32 l141 = 
0;soc_mem_t l120;void*l185,*l186;trie_t*trie = NULL;payload_t*l93 = NULL;
int16 vrf_type = 0;alpm_mem_prefix_array_t*l131 = NULL;uint32 l81[
SOC_MAX_MEM_FIELD_WORDS];defip_alpm_ipv4_entry_t l187,l188;
defip_alpm_ipv6_64_entry_t l189,l190;int l191,l192,l193,l194;l184 = sizeof(
int)*MAX_PREFIX_PER_BUCKET;l97 = sal_alloc(l184,"new_index_move");l96 = 
sal_alloc(l184,"old_index_move");l131 = sal_alloc(sizeof(
alpm_mem_prefix_array_t),"prefix_array");if(l97 == NULL||l96 == NULL||l131 == 
NULL){l139 = SOC_E_MEMORY;goto l195;}l191 = ALPM_BKT_IDX(l181);l193 = 
ALPM_BKT_SIDX(l181);l192 = ALPM_BKT_IDX(l182);l194 = ALPM_BKT_SIDX(l182);l120
= (l130)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l185 = ((l130)?((uint32*)
&(l189)):((uint32*)&(l187)));l186 = ((l130)?((uint32*)&(l190)):((uint32*)&(
l188)));l134 = SOC_ALPM_BS_BKT_USAGE_PIVOT(l80,l181);trie = PIVOT_BUCKET_TRIE
(ALPM_TCAM_PIVOT(l80,l134));vrf_type = SOC_ALPM_BS_BKT_USAGE_VRF(l80,l181);
soc_th_alpm_bank_disb_get(l80,vrf_type,&l141);sal_memset(l131,0,sizeof(*l131)
);l139 = trie_traverse(trie,_soc_th_alpm_mem_prefix_array_cb,l131,
_TRIE_INORDER_TRAVERSE);do{if((l139)<0){goto l195;}}while(0);sal_memset(l97,-
1,l184);sal_memset(l96,-1,l184);for(l163 = 0;l163<l131->count;l163++){l93 = 
l131->prefix[l163];if(l93->index>0){l139 = soc_mem_read(l80,l120,
MEM_BLOCK_ANY,l93->index,l185);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"soc_mem_read index %d failed\n"),l93->index)
);goto l196;}soc_mem_field32_set(l80,l120,l185,SUB_BKT_PTRf,l194);if(
SOC_URPF_STATUS_GET(l80)){l139 = soc_mem_read(l80,l120,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,l93->index),l186);if(SOC_FAILURE(l139)){goto l196;
}soc_mem_field32_set(l80,l120,l186,SUB_BKT_PTRf,l194);}l139 = 
_soc_th_alpm_insert_in_bkt(l80,l120,l182,l141,l185,l81,&l94,l130);if(
SOC_SUCCESS(l139)){if(SOC_URPF_STATUS_GET(l80)){l139 = soc_mem_write(l80,l120
,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),l186);if(SOC_FAILURE(l139)){
goto l196;}}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"insert to bucket %d failed\n"),l182));goto l196;}l97[l163] = l94;l96[l163] = 
l93->index;}}l139 = l175(l80,l130,l134,l182);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_th_alpm_move_relink failed, pivot %d bkt %d\n"),l134,l182));goto l196;}
l139 = _soc_th_alpm_move_inval(l80,l120,l130,l131,l97);if(SOC_FAILURE(l139)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_th_alpm_move_inval failed\n")
));goto l196;}l139 = soc_th_alpm_update_hit_bits(l80,l131->count,l96,l97);if(
SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l131->count));l139 = 
SOC_E_NONE;}PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l80,l134)) = ALPM_BKTID(l192,
l194);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l80,l130,l191);
SOC_ALPM_BS_BKT_USAGE_SB_DEL(l80,l191,l193,TRUE,l131->count);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l80,l130,l191);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l80,l130,l192);SOC_ALPM_BS_BKT_USAGE_SB_ADD(
l80,l192,l194,l134,vrf_type,l131->count);SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l80
,l130,l192);l196:if(SOC_FAILURE(l139)){l183 = l175(l80,l130,l134,l181);do{if(
(l183)<0){goto l195;}}while(0);for(l163 = 0;l163<l131->count;l163++){if(l97[
l163] == -1){continue;}l183 = soc_mem_write(l80,l120,MEM_BLOCK_ANY,l97[l163],
soc_mem_entry_null(l80,l120));do{if((l183)<0){goto l195;}}while(0);if(
SOC_URPF_STATUS_GET(l80)){l183 = soc_mem_write(l80,l120,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,l97[l163]),soc_mem_entry_null(l80,l120));do{if((
l183)<0){goto l195;}}while(0);}}}l195:if(l97!= NULL){sal_free(l97);}if(l96!= 
NULL){sal_free(l96);}if(l131!= NULL){sal_free(l131);}return l139;}int
soc_th_alpm_bs_merge(int l80,int l130,int l197,int l198){int l139 = 
SOC_E_NONE;alpm_bkt_usg_t*l199,*l200;int l201,l202;l199 = &bkt_usage[l80][
l197];l200 = &bkt_usage[l80][l198];for(l201 = 0;l201<SOC_TH_MAX_SUB_BUCKETS;
l201++){if(!(l199->sub_bkts&(1<<l201))){continue;}for(l202 = 0;l202<
SOC_TH_MAX_SUB_BUCKETS;l202++){if((l200->sub_bkts&(1<<l202))!= 0){continue;}
if(l130 == L3_DEFIP_MODE_128){l139 = _soc_th_alpm_128_move_trie(l80,l130,
ALPM_BKTID(l197,l201),ALPM_BKTID(l198,l202));}else{l139 = 
_soc_th_alpm_move_trie(l80,l130,ALPM_BKTID(l197,l201),ALPM_BKTID(l198,l202));
}SOC_IF_ERROR_RETURN(l139);break;}}alpm_merge_count++;return SOC_E_NONE;}
static int l203(int l80,int l204,int l205){if(l204 == l205){return 1;}if(l204
== (SOC_VRF_MAX(l80)+1)||l205 == (SOC_VRF_MAX(l80)+1)){return 0;}return 1;}
static int l206(int l80,int*bktid,int l130){int l207,l208,l172;int l197,l198,
l209;alpm_bkt_bmp_t*l210,*l211;alpm_bkt_usg_t*l199,*l200;for(l209 = 0;l209<
SOC_TH_MAX_ALPM_VIEWS;l209++){l199 = l200 = NULL;l172 = 
_soc_th_alpm_bkt_entry_cnt(l80,l209);for(l207 = 1;l207<= (l172/2);l207++){
l210 = &global_bkt_usage[l80][l209][l207];if(l210->bkt_count<1){continue;}
l197 = l173(l80,-1,l210);if(l197 == ALPM_BKT_IDX(*bktid)){l197 = l173(l80,
l197,l210);if(l197 == -1){continue;}}l199 = &bkt_usage[l80][l197];l198 = l173
(l80,l197,l210);while(l198!= -1){l200 = &bkt_usage[l80][l198];if(!l203(l80,
l199->vrf_type[0],l200->vrf_type[0])){l198 = l173(l80,l198,l210);continue;}if
(_shr_popcount((uint32)l199->sub_bkts)+_shr_popcount((uint32)l200->sub_bkts)
<= SOC_TH_MAX_SUB_BUCKETS){break;}l198 = l173(l80,l198,l210);}if(l198!= -1){
SOC_IF_ERROR_RETURN(soc_th_alpm_bs_merge(l80,l209,l197,l198));*bktid = 
ALPM_BKTID(l197,0);return SOC_E_NONE;}for(l208 = l207+1;l208<
SOC_TH_MAX_BUCKET_ENTRIES;l208++){l211 = &global_bkt_usage[l80][l209][l208];
if(l211->bkt_count<= 0){continue;}if((l207+l208)>l172){break;}l198 = l173(l80
,-1,l211);while(l198!= -1){l200 = &bkt_usage[l80][l198];if((l198 == l197)||(!
l203(l80,l199->vrf_type[0],l200->vrf_type[0]))){l198 = l173(l80,l198,l211);
continue;}if((_shr_popcount((uint32)l199->sub_bkts)+_shr_popcount((uint32)
l200->sub_bkts))<= SOC_TH_MAX_SUB_BUCKETS){break;}l198 = l173(l80,l198,l211);
}if(l198!= -1){SOC_IF_ERROR_RETURN(soc_th_alpm_bs_merge(l80,l209,l197,l198));
*bktid = ALPM_BKTID(l197,0);return SOC_E_NONE;}}}}l172 = 
_soc_th_alpm_bkt_entry_cnt(l80,l130);for(l207 = 1;l207<l172;l207++){l210 = &
global_bkt_usage[l80][l130][l207];if(l210->bkt_count<1){continue;}l197 = l173
(l80,-1,l210);if(l197 == ALPM_BKT_IDX(*bktid)){l197 = l173(l80,l197,l210);if(
l197 == -1){continue;}}l199 = &bkt_usage[l80][l197];for(l208 = 0;l208<
SOC_TH_MAX_SUB_BUCKETS;l208++){if(l199->sub_bkts&(1<<l208)){continue;}*bktid = 
ALPM_BKTID(l197,l208);return SOC_E_NONE;}}return SOC_E_FULL;}int
soc_th_alpm_bs_alloc(int l80,int*bktid,int vrf,int l130){int l139 = 
SOC_E_NONE;l139 = soc_th_alpm_bucket_assign(l80,bktid,vrf,l130);if(l139!= 
SOC_E_FULL){*bktid = ALPM_BKTID(*bktid,0);return l139;}l139 = l206(l80,bktid,
l130);return l139;}int soc_th_alpm_bs_free(int l80,int bktid,int vrf,int l130
){int l139 = SOC_E_NONE;alpm_bkt_usg_t*l135;int l212,l213;l212 = ALPM_BKT_IDX
(bktid);l213 = ALPM_BKT_SIDX(bktid);l135 = &bkt_usage[l80][l212];l135->
sub_bkts&= ~(1<<l213);l135->pivots[l213] = 0;if(l135->count == 0){l135->
vrf_type[l213] = 0;l139 = soc_th_alpm_bucket_release(l80,l212,vrf,l130);}
return l139;}int soc_th_alpm_bu_upd(int l80,int l125,int tcam_index,int l214,
int l130,int l215){int l212,l213;l212 = ALPM_BKT_IDX(l125);l213 = 
ALPM_BKT_SIDX(l125);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l80,l130,l212);if(l215>0
){SOC_ALPM_BS_BKT_USAGE_SB_ADD(l80,l212,l213,tcam_index,l214,l215);}else if(
l215<0){SOC_ALPM_BS_BKT_USAGE_SB_DEL(l80,l212,l213,l214,-l215);}
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l80,l130,l212);return SOC_E_NONE;}int
soc_th_alpm_bs_dump_brief(int l80,const char*l93){int l163,l216,l217,l143,
l218 = 0;int l219 = 1,l220 = 0;alpm_bkt_bmp_t*l221 = NULL;alpm_bkt_usg_t*l135
= NULL;int*l222;char*l223[] = {"IPv4","IPv6-64","IPv6-128"};l222 = sal_alloc(
sizeof(int)*16384,"pivot");if(l222 == NULL){return SOC_E_MEMORY;}sal_memset(
l222,0,sizeof(int)*16384);for(l216 = 0;l216<SOC_TH_MAX_ALPM_VIEWS;l216++){
LOG_CLI((BSL_META_U(l80,"\n[- %8s -]\n"),l223[l216]));LOG_CLI((BSL_META_U(l80
,"=================================================\n")));LOG_CLI((BSL_META_U
(l80," %5s | %5s %10s\n"),"entry","count","1st-bktbmp"));LOG_CLI((BSL_META_U(
l80,"=================================================\n")));l143 = 0;for(
l217 = 0;l217<SOC_TH_MAX_BUCKET_ENTRIES;l217++){l221 = &global_bkt_usage[l80]
[l216][l217];if(l221->bkt_count == 0){continue;}l143++;LOG_CLI((BSL_META_U(
l80," %5d   %5d"),l217,l221->bkt_count));for(l163 = 0;l163<
SOC_TH_MAX_ALPM_BUCKETS;l163++){if(SHR_BITGET(l221->bkt_bmp,l163)){LOG_CLI((
BSL_META_U(l80," 0x%08x\n"),l221->bkt_bmp[l163/SHR_BITWID]));break;}}}if(l143
== 0){LOG_CLI((BSL_META_U(l80,"- None - \n")));}}l217 = 0;LOG_CLI((BSL_META_U
(l80,"%5s | %s\n"),"bkt","(sub-bkt-idx, tcam-idx)"));LOG_CLI((BSL_META_U(l80,
"=================================================\n")));for(l143 = 0;l143<
SOC_TH_MAX_ALPM_BUCKETS;l143++){l135 = &bkt_usage[l80][l143];if(l135->count == 
0){continue;}l217++;l218 = 0;LOG_CLI((BSL_META_U(l80,"%5d | "),l143));for(
l163 = 0;l163<4;l163++){if(l135->sub_bkts&(1<<l163)){LOG_CLI((BSL_META_U(l80,
"(%d, %5d) "),l163,l135->pivots[l163]));if(l222[l135->pivots[l163]]!= 0){l218
= l163+1;}else{l222[l135->pivots[l163]] = l143;}}}LOG_CLI((BSL_META_U(l80,
"\n")));if(l218){LOG_CLI((BSL_META_U(l80,
"Error: multi-buckets were linked to pivot %d, prev %d, curr %d\n"),l135->
pivots[l218-1],l222[l135->pivots[l218-1]],l143));}}if(l217 == 0){LOG_CLI((
BSL_META_U(l80,"- None - \n")));}LOG_CLI((BSL_META_U(l80,"\n")));if(
SOC_TH_ALPM_SCALE_CHECK(l80,1)){l219 = 2;}l143 = 0;for(l163 = 0;l163<
SOC_TH_ALPM_BUCKET_COUNT(l80);l163+= l219){SHR_BITTEST_RANGE(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80),l163,l219,l220);if(l220){l143++;}}LOG_CLI((
BSL_META_U(l80,"VRF Route buckets: %5d\n"),l143));if(soc_th_alpm_mode_get(l80
) == SOC_ALPM_MODE_PARALLEL&&SOC_URPF_STATUS_GET(l80)){l143 = 0;for(l163 = 0;
l163<SOC_TH_ALPM_BUCKET_COUNT(l80);l163+= l219){SHR_BITTEST_RANGE(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l80),l163,l219,l220);if(l220){l143++;}}LOG_CLI((
BSL_META_U(l80,"Global Route buckets: %5d\n"),l143));}sal_free(l222);return
SOC_E_NONE;}static int l224(int l80,int l225,int l130,int l226){int l139,l146
,index;defip_aux_table_entry_t entry;index = l225>>(l130?0:1);l139 = 
soc_mem_read(l80,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l139);if(l130){soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l226);soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l226);l146 = soc_mem_field32_get(l80,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l225&1){soc_mem_field32_set(
l80,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l226);l146 = soc_mem_field32_get(
l80,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l226);l146 = soc_mem_field32_get(l80,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l139 = soc_mem_write(l80,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l139);if(
SOC_URPF_STATUS_GET(l80)){l146++;if(l130){soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l226);soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l226);}else{if(l225&1){
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l226);}else{
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l226);}}
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l146);
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l146);index+= (2
*soc_mem_index_count(l80,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l80,
L3_DEFIPm))/2;l139 = soc_mem_write(l80,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
index,&entry);}return l139;}static int l227(int l80,int l228,void*entry,void*
l229,int l230){uint32 l146,l151,l130,l108,l109,l231 = 0;soc_mem_t l232 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l233;int l139 = SOC_E_NONE,l93,l234,vrf,
l205;SOC_IF_ERROR_RETURN(soc_mem_read(l80,l232,MEM_BLOCK_ANY,l228,l229));l146
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(
l67[(l80)]->l41));soc_mem_field32_set(l80,l232,l229,VRF0f,l146);l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l42));soc_mem_field32_set(l80,l232,l229,VRF1f,l146);l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l29));soc_mem_field32_set(l80,l232,l229,MODE0f,l146);l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l30));soc_mem_field32_set(l80,l232,l229,MODE1f,l146);l130 = l146;
l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l39));soc_mem_field32_set(l80,l232,l229,VALID0f,l146);
l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l40));soc_mem_field32_set(l80,l232,l229,VALID1f,l146);
l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l27));if((l139 = _ipmask2pfx(l146,&l93))<0){return l139;}
l151 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l28));if((l139 = _ipmask2pfx(l151,&l234))<0){return l139;
}if(l130){soc_mem_field32_set(l80,l232,l229,IP_LENGTH0f,l93+l234);
soc_mem_field32_set(l80,l232,l229,IP_LENGTH1f,l93+l234);}else{
soc_mem_field32_set(l80,l232,l229,IP_LENGTH0f,l93);soc_mem_field32_set(l80,
l232,l229,IP_LENGTH1f,l234);}l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l25));soc_mem_field32_set(
l80,l232,l229,IP_ADDR0f,l146);l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l26));soc_mem_field32_set(
l80,l232,l229,IP_ADDR1f,l146);if(!l130){sal_memcpy(&l233,entry,sizeof(l233));
l139 = soc_th_alpm_lpm_vrf_get(l80,(void*)&l233,&vrf,&l93);
SOC_IF_ERROR_RETURN(l139);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_ip4entry1_to_0(
l80,&l233,&l233,PRESERVE_HIT));l139 = soc_th_alpm_lpm_vrf_get(l80,(void*)&
l233,&l205,&l234);SOC_IF_ERROR_RETURN(l139);}else{l139 = 
soc_th_alpm_lpm_vrf_get(l80,entry,&vrf,&l93);}if(SOC_URPF_STATUS_GET(l80)){if
(l230>= (soc_mem_index_count(l80,L3_DEFIPm)>>1)){l231 = 1;}}
soc_alpm_db_ent_type_encoding(l80,l93,&l108,&l109);if(vrf == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l80,l232,l229,VALID0f,0);l108 = 0;}
else{if(l231){l108+= 1;}}soc_mem_field32_set(l80,l232,l229,DB_TYPE0f,l108);
l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l63));soc_mem_field32_set(l80,l232,l229,ENTRY_TYPE0f,l146
|l109);if(!l130){soc_alpm_db_ent_type_encoding(l80,l234,&l108,&l109);if(l205
== SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l80,l232,l229,VALID1f,0);l108 = 0
;}else{if(l231){l108+= 1;}}soc_mem_field32_set(l80,l232,l229,DB_TYPE1f,l108);
l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l64));soc_mem_field32_set(l80,l232,l229,ENTRY_TYPE1f,l146
|l109);}else{if(vrf == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l80,l232,l229
,VALID1f,0);}soc_mem_field32_set(l80,l232,l229,DB_TYPE1f,l108);l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l64));soc_mem_field32_set(l80,l232,l229,ENTRY_TYPE1f,l146|l109);}if(
l231){l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm))
,(entry),(l67[(l80)]->l49));if(l146){l146+= SOC_TH_ALPM_BUCKET_COUNT(l80);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l49),(l146));}l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l51));if(l146){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l51),(l146));}if(!l130){l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l50));if(l146){l146+= 
SOC_TH_ALPM_BUCKET_COUNT(l80);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l50),(l146));}l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l52));if(l146){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(entry),(l67[(l80)]->l52),(l146));}}}return SOC_E_NONE;}static int
l235(int l80,int l236,int index,int l237,void*entry){defip_aux_table_entry_t
l229;l237 = soc_th_alpm_physical_idx(l80,L3_DEFIPm,l237,1);
SOC_IF_ERROR_RETURN(l227(l80,l237,entry,(void*)&l229,index));
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l80,MEM_BLOCK_ANY,index,entry));index = 
soc_th_alpm_physical_idx(l80,L3_DEFIPm,index,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l80,MEM_BLOCK_ANY,index,&l229));return SOC_E_NONE;}
int _soc_th_alpm_insert_in_bkt(int l80,soc_mem_t l120,int bktid,int l141,void
*alpm_data,uint32*l81,int*l94,int l130){int l139;l139 = soc_mem_alpm_insert(
l80,l120,ALPM_BKT_IDX(bktid),MEM_BLOCK_ANY,l141,alpm_data,l81,l94);if(l139 == 
SOC_E_FULL){if(SOC_TH_ALPM_SCALE_CHECK(l80,l130)){l139 = soc_mem_alpm_insert(
l80,l120,ALPM_BKT_IDX(bktid)+1,MEM_BLOCK_ANY,l141,alpm_data,l81,l94);}}return
l139;}int _soc_th_alpm_mem_index(int l80,soc_mem_t l120,int bucket_index,int
l238,uint32 l141,int*l239){int l163,l178 = 0;int l240[4] = {0};int l241 = 0;
int l242 = 0;int l243;int l244 = 6;int l245;int l246;int l247;int l248 = 0;
switch(l120){case L3_DEFIP_ALPM_IPV6_64m:l244 = 4;break;case
L3_DEFIP_ALPM_IPV6_128m:l244 = 2;break;default:break;}if(
SOC_TH_ALPM_SCALE_CHECK(l80,(l120!= L3_DEFIP_ALPM_IPV4m))){if(l238>= 
ALPM_RAW_BKT_COUNT*l244){bucket_index++;l238-= ALPM_RAW_BKT_COUNT*l244;}}l245
= 4;l246 = 15;l247 = 2;if(soc_th_get_alpm_banks(l80)<= 2){l245 = 2;l246 = 14;
l247 = 1;}l248 = ((1<<l245)-1);l243 = l245-_shr_popcount(l141&l248);if(
bucket_index>= (1<<l246)||l238>= l243*l244){return SOC_E_PARAM;}l242 = l238%
l244;for(l163 = 0;l163<l245;l163++){if((1<<l163)&l141){continue;}l240[l178++]
= l163;}l241 = l240[l238/l244];*l239 = (l242<<l246)|(bucket_index<<l247)|(
l241);return SOC_E_NONE;}static int _soc_th_alpm_move_inval(int l80,soc_mem_t
l120,int l130,alpm_mem_prefix_array_t*l131,int*l97){int l163,l139 = 
SOC_E_NONE,l183;defip_alpm_ipv4_entry_t l187;defip_alpm_ipv6_64_entry_t l189;
int l249,l250;void*l251 = NULL,*l252 = NULL;int*l253 = NULL;int l254 = FALSE;
l249 = l130?sizeof(l189):sizeof(l187);l250 = l249*l131->count;l251 = 
sal_alloc(l250,"rb_bufp");l252 = sal_alloc(l250,"rb_sip_bufp");l253 = 
sal_alloc(sizeof(*l253)*l131->count,"roll_back_index");if(l251 == NULL||l252
== NULL||l253 == NULL){l139 = SOC_E_MEMORY;goto l255;}sal_memset(l253,-1,
sizeof(*l253)*l131->count);for(l163 = 0;l163<l131->count;l163++){payload_t*
prefix = l131->prefix[l163];if(prefix->index>= 0){l139 = soc_mem_read(l80,
l120,MEM_BLOCK_ANY,prefix->index,(uint8*)l251+l163*l249);if(SOC_FAILURE(l139)
){l163--;l254 = TRUE;break;}if(SOC_URPF_STATUS_GET(l80)){l139 = soc_mem_read(
l80,l120,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,prefix->index),(uint8*)l252
+l163*l249);if(SOC_FAILURE(l139)){l163--;l254 = TRUE;break;}}l139 = 
soc_mem_write(l80,l120,MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null(l80,
l120));if(SOC_FAILURE(l139)){l253[l163] = prefix->index;l254 = TRUE;break;}if
(SOC_URPF_STATUS_GET(l80)){l139 = soc_mem_write(l80,l120,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,prefix->index),soc_mem_entry_null(l80,l120));if(
SOC_FAILURE(l139)){l253[l163] = prefix->index;l254 = TRUE;break;}}}l253[l163]
= prefix->index;prefix->index = l97[l163];}if(l254){for(;l163>= 0;l163--){
payload_t*prefix = l131->prefix[l163];prefix->index = l253[l163];if(l253[l163
]<0){continue;}l183 = soc_mem_write(l80,l120,MEM_BLOCK_ALL,l253[l163],(uint8*
)l251+l163*l249);if(SOC_FAILURE(l183)){break;}if(!SOC_URPF_STATUS_GET(l80)){
continue;}l183 = soc_mem_write(l80,l120,MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(
l80,l253[l163]),(uint8*)l252+l163*l249);if(SOC_FAILURE(l183)){break;}}}l255:
if(l253!= NULL){sal_free(l253);}if(l252!= NULL){sal_free(l252);}if(l251!= 
NULL){sal_free(l251);}return l139;}void _soc_th_alpm_rollback_pivot_add(int
l80,int l130,defip_entry_t*l256,void*key_data,int tcam_index,alpm_pivot_t*
pivot_pyld){int l139;trie_t*l156 = NULL;int vrf,vrf_id;trie_node_t*l257 = 
NULL;(void)soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf);l139 = 
soc_th_alpm_lpm_delete(l80,l256);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: Failure to free new prefix""at %d\n"),
soc_th_alpm_logical_idx(l80,L3_DEFIPm,tcam_index,l130)));}if(l130){l156 = 
VRF_PIVOT_TRIE_IPV6(l80,vrf);}else{l156 = VRF_PIVOT_TRIE_IPV4(l80,vrf);}if(
ALPM_TCAM_PIVOT(l80,tcam_index<<(l130?1:0))!= NULL){l139 = trie_delete(l156,
pivot_pyld->key,pivot_pyld->len,&l257);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_insert: trie delete failure "
"in bkt move rollback\n")));}}ALPM_TCAM_PIVOT(l80,tcam_index<<(l130?1:0)) = 
NULL;VRF_PIVOT_REF_DEC(l80,vrf,l130);}int _soc_th_alpm_rollback_bkt_move(int
l80,int l130,void*key_data,soc_mem_t l120,alpm_pivot_t*l258,alpm_pivot_t*l259
,alpm_mem_prefix_array_t*l260,int*l97,int l261){trie_node_t*l257 = NULL;
uint32 prefix[5],l154;trie_t*l262;int vrf,vrf_id,l163,l122 = 0;
defip_alpm_ipv4_entry_t l187;defip_alpm_ipv6_64_entry_t l189;
defip_alpm_ipv6_128_entry_t l263;void*l185;payload_t*l264;int l139 = 
SOC_E_NONE;alpm_bucket_handle_t*l265;l265 = PIVOT_BUCKET_HANDLE(l259);if(l120
== L3_DEFIP_ALPM_IPV6_128m){l139 = _soc_th_alpm_128_prefix_create(l80,
key_data,prefix,&l154,&l122);}else{l139 = l149(l80,l130,key_data,prefix,&l154
,&l122);}if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"prefix create failed\n")));return l139;}if(l120 == L3_DEFIP_ALPM_IPV6_128m){
l185 = ((uint32*)&(l263));(void)soc_th_alpm_128_lpm_vrf_get(l80,key_data,&
vrf_id,&vrf);}else{l185 = ((l130)?((uint32*)&(l189)):((uint32*)&(l187)));(
void)soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf);}if(l120 == 
L3_DEFIP_ALPM_IPV6_128m){l262 = VRF_PREFIX_TRIE_IPV6_128(l80,vrf);}else if(
l130){l262 = VRF_PREFIX_TRIE_IPV6(l80,vrf);}else{l262 = VRF_PREFIX_TRIE_IPV4(
l80,vrf);}for(l163 = 0;l163<l260->count;l163++){payload_t*l93 = l260->prefix[
l163];if(l97[l163]!= -1){if(l120 == L3_DEFIP_ALPM_IPV6_128m){sal_memset(l185,
0,sizeof(defip_alpm_ipv6_128_entry_t));}else if(l130){sal_memset(l185,0,
sizeof(defip_alpm_ipv6_64_entry_t));}else{sal_memset(l185,0,sizeof(
defip_alpm_ipv4_entry_t));}l139 = soc_mem_write(l80,l120,MEM_BLOCK_ANY,l97[
l163],l185);_soc_tomahawk_alpm_bkt_view_set(l80,l97[l163],INVALIDm);if(
SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l80)){l139 = soc_mem_write(l80,l120,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,l97[l163]),l185);_soc_tomahawk_alpm_bkt_view_set(
l80,_soc_th_alpm_rpf_entry(l80,l97[l163]),INVALIDm);if(SOC_FAILURE(l139)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l257 = NULL;l139 = trie_delete(
PIVOT_BUCKET_TRIE(l259),l93->key,l93->len,&l257);l264 = (payload_t*)l257;if(
SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(l93->
index>0){l139 = trie_insert(PIVOT_BUCKET_TRIE(l258),l93->key,NULL,l93->len,(
trie_node_t*)l264);if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l264!= NULL){sal_free(l264);}}}if(l261
== -1){l257 = NULL;l139 = trie_delete(PIVOT_BUCKET_TRIE(l258),prefix,l154,&
l257);l264 = (payload_t*)l257;if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l80,"Expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l264!= 
NULL){sal_free(l264);}}l139 = soc_th_alpm_bs_free(l80,PIVOT_BUCKET_INDEX(l259
),vrf,l130);if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"New bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(l259)));}trie_destroy(
PIVOT_BUCKET_TRIE(l259));sal_free(l265);sal_free(l259);sal_free(l97);l257 = 
NULL;l139 = trie_delete(l262,prefix,l154,&l257);l264 = (payload_t*)l257;if(
SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Failed to delete new prefix""0x%08x 0x%08x from pfx trie\n"),prefix[0],
prefix[1]));}if(l264){sal_free(l264);}return l139;}int
_soc_th_alpm_free_pfx_trie(int l80,trie_t*l262,trie_t*l266,payload_t*
new_pfx_pyld,int*l97,int bktid,int vrf,int l130){int l139 = SOC_E_NONE;
trie_node_t*l257 = NULL;payload_t*l267 = NULL;payload_t*l268 = NULL;if(l97!= 
NULL){sal_free(l97);}(void)trie_delete(l262,new_pfx_pyld->key,new_pfx_pyld->
len,&l257);l268 = (payload_t*)l257;if(l268!= NULL){(void)trie_delete(l266,
l268->key,l268->len,&l257);l267 = (payload_t*)l257;if(l267!= NULL){sal_free(
l267);}sal_free(l268);}if(bktid!= -1){l139 = soc_th_alpm_bs_free(l80,bktid,
vrf,l130);if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"unit %d Unable to free bucket_id %d"),l80,bktid));}}return SOC_E_NONE;}
static int l269(int l80,int l130,alpm_pfx_info_t*l270,trie_t*l262,uint32*l222
,uint32 l154,trie_node_t*l271,defip_entry_t*lpm_entry,uint32*l226){
trie_node_t*l157 = NULL;defip_alpm_ipv4_entry_t l187;
defip_alpm_ipv6_64_entry_t l189;payload_t*l272 = NULL;int l273;void*l185;
alpm_pivot_t*l274;alpm_bucket_handle_t*l265;int l139 = SOC_E_NONE;soc_mem_t
l120;l274 = l270->pivot_pyld;l273 = l274->tcam_index;l120 = (l130)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l185 = ((l130)?((uint32*)&(l189)):
((uint32*)&(l187)));l157 = NULL;l139 = trie_find_lpm(l262,l222,l154,&l157);
l272 = (payload_t*)l157;if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l80,l222[0],l222[1],l222
[2],l222[3],l222[4],l154));return l139;}if(l272->bkt_ptr){if(l272->bkt_ptr == 
l270->new_pfx_pyld){sal_memcpy(l185,l270->alpm_data,l130?sizeof(
defip_alpm_ipv6_64_entry_t):sizeof(defip_alpm_ipv4_entry_t));}else{l139 = 
soc_mem_read(l80,l120,MEM_BLOCK_ANY,((payload_t*)l272->bkt_ptr)->index,l185);
}if(SOC_FAILURE(l139)){return l139;}l139 = l124(l80,l185,l120,l130,l270->
vrf_id,l270->bktid,((payload_t*)l272->bkt_ptr)->index,lpm_entry);if(
SOC_FAILURE(l139)){return l139;}*l226 = ((payload_t*)(l272->bkt_ptr))->len;}
else{l139 = soc_mem_read(l80,L3_DEFIPm,MEM_BLOCK_ANY,soc_th_alpm_logical_idx(
l80,L3_DEFIPm,l273>>1,1),lpm_entry);if((!l130)&&(l273&1)){l139 = 
soc_th_alpm_lpm_ip4entry1_to_0(l80,lpm_entry,lpm_entry,0);}}l265 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l265 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l139 = SOC_E_MEMORY;return l139;}sal_memset(l265,0,sizeof(*l265));l274 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l274 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l139 = SOC_E_MEMORY;return l139;}sal_memset(l274,0,sizeof(*l274));
PIVOT_BUCKET_HANDLE(l274) = l265;if(l130){l139 = trie_init(_MAX_KEY_LEN_144_,
&PIVOT_BUCKET_TRIE(l274));}else{l139 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l274));}PIVOT_BUCKET_TRIE(l274)->trie = l271;
PIVOT_BUCKET_INDEX(l274) = l270->bktid;PIVOT_BUCKET_VRF(l274) = l270->vrf;
PIVOT_BUCKET_IPV6(l274) = l130;PIVOT_BUCKET_DEF(l274) = FALSE;(l274)->key[0] = 
l222[0];(l274)->key[1] = l222[1];(l274)->key[2] = l222[2];(l274)->key[3] = 
l222[3];(l274)->key[4] = l222[4];(l274)->len = l154;do{if(!(l130)){l222[0] = 
(((32-l154) == 32)?0:(l222[1])<<(32-l154));l222[1] = 0;}else{int l275 = 64-
l154;int l276;if(l275<32){l276 = l222[3]<<l275;l276|= (((32-l275) == 32)?0:(
l222[4])>>(32-l275));l222[0] = l222[4]<<l275;l222[1] = l276;l222[2] = l222[3]
= l222[4] = 0;}else{l222[1] = (((l275-32) == 32)?0:(l222[4])<<(l275-32));l222
[0] = l222[2] = l222[3] = l222[4] = 0;}}}while(0);l126(l80,l222,l154,l270->
vrf,l130,lpm_entry,0,0);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l49),(ALPM_BKT_IDX(l270->bktid)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l51),(ALPM_BKT_SIDX(l270->bktid)));l270->pivot_pyld = l274;
return l139;}static int l277(int l80,int l130,alpm_pfx_info_t*l270,int*l225,
int*l113){trie_node_t*l271;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];uint32 l154,
l226 = 0;uint32 l141 = 0;uint32 l222[5];int l94;defip_alpm_ipv4_entry_t l187,
l188;defip_alpm_ipv6_64_entry_t l189,l190;trie_t*l262,*trie;void*l185,*l186;
alpm_pivot_t*l278 = l270->pivot_pyld;defip_entry_t lpm_entry;soc_mem_t l120;
trie_t*l156 = NULL;alpm_mem_prefix_array_t l171;int*l97 = NULL;int*l96 = NULL
;int l139 = SOC_E_NONE,l163,l261 = -1;int tcam_index,l279,l280 = 0;int l281 = 
0,l282 = 0;l120 = (l130)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l185 = ((
l130)?((uint32*)&(l189)):((uint32*)&(l187)));l186 = ((l130)?((uint32*)&(l190)
):((uint32*)&(l188)));soc_th_alpm_bank_disb_get(l80,l270->vrf,&l141);if(l130)
{l262 = VRF_PREFIX_TRIE_IPV6(l80,l270->vrf);}else{l262 = VRF_PREFIX_TRIE_IPV4
(l80,l270->vrf);}trie = PIVOT_BUCKET_TRIE(l270->pivot_pyld);l280 = l270->
bktid;l279 = PIVOT_TCAM_INDEX(l278);l139 = soc_th_alpm_bs_alloc(l80,&l270->
bktid,l270->vrf,l130);if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"Unable to allocate ""new bucket for split\n")));l270->bktid = 
-1;_soc_th_alpm_free_pfx_trie(l80,l262,trie,l270->new_pfx_pyld,l97,l270->
bktid,l270->vrf,l130);return l139;}l163 = _soc_th_alpm_bkt_entry_cnt(l80,l130
);l163-= SOC_ALPM_BS_BKT_USAGE_COUNT(l80,ALPM_BKT_IDX(l270->bktid));l139 = 
trie_split(trie,l130?_MAX_KEY_LEN_144_:_MAX_KEY_LEN_48_,FALSE,l222,&l154,&
l271,NULL,FALSE,l163);if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"Could not split bucket")));_soc_th_alpm_free_pfx_trie(l80,
l262,trie,l270->new_pfx_pyld,l97,l270->bktid,l270->vrf,l130);return l139;}
l139 = l269(l80,l130,l270,l262,l222,l154,l271,&lpm_entry,&l226);if(l139!= 
SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"could not initialize pivot")));_soc_th_alpm_free_pfx_trie(l80,l262,trie,l270
->new_pfx_pyld,l97,l270->bktid,l270->vrf,l130);return l139;}sal_memset(&l171,
0,sizeof(l171));l139 = trie_traverse(PIVOT_BUCKET_TRIE(l270->pivot_pyld),
_soc_th_alpm_mem_prefix_array_cb,&l171,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE
(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Bucket split failed")));
_soc_th_alpm_free_pfx_trie(l80,l262,trie,l270->new_pfx_pyld,l97,l270->bktid,
l270->vrf,l130);return l139;}l97 = sal_alloc(sizeof(*l97)*l171.count,
"new_index");if(l97 == NULL){_soc_th_alpm_free_pfx_trie(l80,l262,trie,l270->
new_pfx_pyld,l97,l270->bktid,l270->vrf,l130);return SOC_E_MEMORY;}l96 = 
sal_alloc(sizeof(*l96)*l171.count,"new_index");if(l96 == NULL){
_soc_th_alpm_free_pfx_trie(l80,l262,trie,l270->new_pfx_pyld,l97,l270->bktid,
l270->vrf,l130);return SOC_E_MEMORY;}sal_memset(l97,-1,sizeof(*l97)*l171.
count);sal_memset(l96,-1,sizeof(*l96)*l171.count);for(l163 = 0;l163<l171.
count;l163++){payload_t*l93 = l171.prefix[l163];if(l93->index>0){l139 = 
soc_mem_read(l80,l120,MEM_BLOCK_ANY,l93->index,l185);if(SOC_FAILURE(l139)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_insert: Failed to"
"read prefix ""0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l171.prefix[l163]->
key[1],l171.prefix[l163]->key[2],l171.prefix[l163]->key[3],l171.prefix[l163]
->key[4]));(void)_soc_th_alpm_rollback_bkt_move(l80,l130,l270->key_data,l120,
l278,l270->pivot_pyld,&l171,l97,l261);sal_free(l96);return l139;}if(
SOC_URPF_STATUS_GET(l80)){l139 = soc_mem_read(l80,l120,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,l93->index),l186);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Failed to read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l171.prefix[l163]->key[1],l171.
prefix[l163]->key[2],l171.prefix[l163]->key[3],l171.prefix[l163]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l80,l130,l270->key_data,l120,l278,l270->
pivot_pyld,&l171,l97,l261);sal_free(l96);return l139;}}soc_mem_field32_set(
l80,l120,l185,SUB_BKT_PTRf,ALPM_BKT_SIDX(l270->bktid));soc_mem_field32_set(
l80,l120,l186,SUB_BKT_PTRf,ALPM_BKT_SIDX(l270->bktid));l139 = 
_soc_th_alpm_insert_in_bkt(l80,l120,l270->bktid,l141,l185,l81,&l94,l130);if(
SOC_SUCCESS(l139)){if(SOC_URPF_STATUS_GET(l80)){l139 = soc_mem_write(l80,l120
,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),l186);}l281++;l282++;}}else{
soc_mem_field32_set(l80,l120,l270->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(l270
->bktid));l139 = _soc_th_alpm_insert_in_bkt(l80,l120,l270->bktid,l141,l270->
alpm_data,l81,&l94,l130);if(SOC_SUCCESS(l139)){l261 = l163;*l113 = l94;if(
SOC_URPF_STATUS_GET(l80)){soc_mem_field32_set(l80,l120,l270->alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l270->bktid));l139 = soc_mem_write(l80,l120,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),l270->alpm_sip_data);}l282++;}}
l97[l163] = l94;if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l80,"Failed to insert prefix ""0x%08x 0x%08x 0x%08x 0x%08x to bucket %d\n"),
l171.prefix[l163]->key[1],l171.prefix[l163]->key[2],l171.prefix[l163]->key[3]
,l171.prefix[l163]->key[4],l270->bktid));(void)_soc_th_alpm_rollback_bkt_move
(l80,l130,l270->key_data,l120,l278,l270->pivot_pyld,&l171,l97,l261);sal_free(
l96);return l139;}l96[l163] = l93->index;}l139 = l104(l80,&lpm_entry,l225);if
(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Unable to add new pivot to tcam\n")));if(l139 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l80,l270->vrf,l130);}(void)_soc_th_alpm_rollback_bkt_move(
l80,l130,l270->key_data,l120,l278,l270->pivot_pyld,&l171,l97,l261);sal_free(
l96);return l139;}*l225 = soc_th_alpm_physical_idx(l80,L3_DEFIPm,*l225,l130);
l139 = l224(l80,*l225,l130,l226);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Unable to init bpm_len ""for index %d\n"),*
l225));_soc_th_alpm_rollback_pivot_add(l80,l130,&lpm_entry,l270->key_data,*
l225,l270->pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l80,l130,l270->
key_data,l120,l278,l270->pivot_pyld,&l171,l97,l261);sal_free(l96);return l139
;}if(l130){l156 = VRF_PIVOT_TRIE_IPV6(l80,l270->vrf);}else{l156 = 
VRF_PIVOT_TRIE_IPV4(l80,l270->vrf);}l139 = trie_insert(l156,l270->pivot_pyld
->key,NULL,l270->pivot_pyld->len,(trie_node_t*)l270->pivot_pyld);if(
SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"failed to insert into pivot trie\n")));(void)_soc_th_alpm_rollback_bkt_move(
l80,l130,l270->key_data,l120,l278,l270->pivot_pyld,&l171,l97,l261);sal_free(
l96);return l139;}tcam_index = *l225<<(l130?1:0);ALPM_TCAM_PIVOT(l80,
tcam_index) = l270->pivot_pyld;PIVOT_TCAM_INDEX(l270->pivot_pyld) = 
tcam_index;VRF_PIVOT_REF_INC(l80,l270->vrf,l130);l139 = 
_soc_th_alpm_move_inval(l80,l120,l130,&l171,l97);if(SOC_FAILURE(l139)){
_soc_th_alpm_rollback_pivot_add(l80,l130,&lpm_entry,l270->key_data,*l225,l270
->pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l80,l130,l270->key_data,
l120,l278,l270->pivot_pyld,&l171,l97,l261);sal_free(l97);l97 = NULL;sal_free(
l96);return l139;}l139 = soc_th_alpm_update_hit_bits(l80,l171.count,l96,l97);
if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l171.count));l139 = 
SOC_E_NONE;}sal_free(l97);l97 = NULL;sal_free(l96);if(l261 == -1){
soc_mem_field32_set(l80,l120,l270->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(
PIVOT_BUCKET_INDEX(l278)));l139 = _soc_th_alpm_insert_in_bkt(l80,l120,
PIVOT_BUCKET_INDEX(l278),l141,l270->alpm_data,l81,&l94,l130);if(SOC_FAILURE(
l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Could not insert new "
"prefix into trie after split\n")));_soc_th_alpm_free_pfx_trie(l80,l262,trie,
l270->new_pfx_pyld,l97,l270->bktid,l270->vrf,l130);return l139;}l281--;if(
SOC_URPF_STATUS_GET(l80)){l139 = soc_mem_write(l80,l120,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,l94),l270->alpm_sip_data);}*l113 = l94;l270->
new_pfx_pyld->index = l94;}soc_th_alpm_bu_upd(l80,l280,l279,FALSE,l130,-l281)
;soc_th_alpm_bu_upd(l80,l270->bktid,tcam_index,l270->vrf,l130,l282);
PIVOT_BUCKET_ENT_CNT_UPDATE(l270->pivot_pyld);VRF_BUCKET_SPLIT_INC(l80,l270->
vrf,l130);return l139;}static int l283(int l80,int l130,void*key_data,
soc_mem_t l120,void*alpm_data,void*alpm_sip_data,int*l113,int bktid,int
tcam_index){alpm_pivot_t*l274,*l278;defip_aux_scratch_entry_t l111;uint32 l81
[SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l284,l154;int vrf,vrf_id;int l94;
int l139 = SOC_E_NONE,l183;uint32 l108,l109,l141;int l155 =0;int l225;int l285
= 0;trie_t*trie,*l262;trie_node_t*l157 = NULL,*l257 = NULL;payload_t*l264,*
l286,*l272;int l122 = 0;alpm_pfx_info_t l270;SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf));soc_th_alpm_bank_disb_get
(l80,vrf,&l141);soc_alpm_db_ent_type_encoding(l80,vrf,&l108,&l109);l120 = (
l130)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l139 = l149(l80,l130,
key_data,prefix,&l154,&l122);if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l80,"_soc_th_alpm_insert: prefix create failed\n")));return l139;
}sal_memset(&l111,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(
l106(l80,key_data,l130,l108,l109,0,&l111));if(bktid == 0){if(
l3_alpm_sw_prefix_lookup[l80]){l139 = l153(l80,prefix,l154,l130,vrf,&l155,&
tcam_index,&bktid);}else{l139 = _soc_th_alpm_aux_op(l80,PREFIX_LOOKUP,&l111,
TRUE,&l155,&tcam_index,&bktid);}SOC_IF_ERROR_RETURN(l139);soc_mem_field32_set
(l80,l120,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));}l139 = 
_soc_th_alpm_insert_in_bkt(l80,l120,bktid,l141,alpm_data,l81,&l94,l130);if(
l139 == SOC_E_NONE){*l113 = l94;if(SOC_URPF_STATUS_GET(l80)){
soc_mem_field32_set(l80,l120,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid))
;l183 = soc_mem_write(l80,l120,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),
alpm_sip_data);if(SOC_FAILURE(l183)){return l183;}}}if(l139 == SOC_E_FULL){
l155 = _soc_th_alpm_bkt_entry_cnt(l80,l130);if(l155>4){l155 = 4;}if(
PIVOT_BUCKET_COUNT(ALPM_TCAM_PIVOT(l80,tcam_index))<l155){int l287 = bktid;
l139 = soc_th_alpm_bs_alloc(l80,&l287,vrf,l130);SOC_IF_ERROR_RETURN(l139);
l139 = _soc_th_alpm_move_trie(l80,l130,bktid,l287);SOC_IF_ERROR_RETURN(l139);
bktid = l287;soc_mem_field32_set(l80,l120,alpm_data,SUB_BKT_PTRf,
ALPM_BKT_SIDX(bktid));l139 = _soc_th_alpm_insert_in_bkt(l80,l120,bktid,l141,
alpm_data,l81,&l94,l130);if(SOC_SUCCESS(l139)){*l113 = l94;if(
SOC_URPF_STATUS_GET(l80)){soc_mem_field32_set(l80,l120,alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));l183 = soc_mem_write(l80,l120,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),alpm_sip_data);if(SOC_FAILURE(
l183)){return l183;}}}else{return l139;}}else{l285 = 1;}}l274 = 
ALPM_TCAM_PIVOT(l80,tcam_index);trie = PIVOT_BUCKET_TRIE(l274);l278 = l274;
l264 = sal_alloc(sizeof(payload_t),"Payload for Key");if(l264 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 payload entry\n")));
return SOC_E_MEMORY;}l286 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l286 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_vrf_add: unable to allocate memory for "
"pfx trie node\n")));sal_free(l264);return SOC_E_MEMORY;}sal_memset(l264,0,
sizeof(*l264));sal_memset(l286,0,sizeof(*l286));sal_memcpy(l264->key,prefix,
sizeof(prefix));l264->len = l154;l264->index = l94;sal_memcpy(l286,l264,
sizeof(*l264));l286->bkt_ptr = l264;l139 = trie_insert(trie,prefix,NULL,l154,
(trie_node_t*)l264);if(SOC_FAILURE(l139)){if(l264!= NULL){sal_free(l264);}if(
l286!= NULL){sal_free(l286);}return l139;}if(l130){l262 = 
VRF_PREFIX_TRIE_IPV6(l80,vrf);}else{l262 = VRF_PREFIX_TRIE_IPV4(l80,vrf);}if(
!l122){l139 = trie_insert(l262,prefix,NULL,l154,(trie_node_t*)l286);}else{
l157 = NULL;l139 = trie_find_lpm(l262,0,0,&l157);l272 = (payload_t*)l157;if(
SOC_SUCCESS(l139)){l272->bkt_ptr = l264;}}l284 = l154;if(SOC_FAILURE(l139)){
l257 = NULL;(void)trie_delete(trie,prefix,l284,&l257);l272 = (payload_t*)l257
;sal_free(l272);sal_free(l286);return l139;}if(l285){l270.key_data = key_data
;l270.new_pfx_pyld = l264;l270.pivot_pyld = l274;l270.alpm_data = alpm_data;
l270.alpm_sip_data = alpm_sip_data;l270.bktid = bktid;l270.vrf_id = vrf_id;
l270.vrf = vrf;l139 = l277(l80,l130,&l270,&l225,l113);if(l139!= SOC_E_NONE){
return l139;}bktid = l270.bktid;tcam_index = PIVOT_TCAM_INDEX(l270.pivot_pyld
);alpm_split_count++;}else{soc_th_alpm_bu_upd(l80,bktid,tcam_index,vrf,l130,1
);}VRF_TRIE_ROUTES_INC(l80,vrf,l130);if(l122){sal_free(l286);}
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,ALG_HIT_IDXf,*l113);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l80,INSERT_PROPAGATE,&l111,TRUE,&l155
,&tcam_index,&bktid));if(SOC_URPF_STATUS_GET(l80)){uint32 l288[4] = {0,0,0,0}
;l154 = soc_mem_field32_get(l80,L3_DEFIP_AUX_SCRATCHm,&l111,DB_TYPEf);l154+= 
1;soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,DB_TYPEf,l154);if(l122)
{soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,RPEf,1);}else{
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,RPEf,0);}
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,ALG_HIT_IDXf,
_soc_th_alpm_rpf_entry(l80,*l113));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(
l80,INSERT_PROPAGATE,&l111,TRUE,&l155,&tcam_index,&bktid));if(!l122){
soc_mem_field_set(l80,L3_DEFIP_AUX_SCRATCHm,(uint32*)&l111,IP_ADDRf,(uint32*)
l288);soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,IP_LENGTHf,0);
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,REPLACE_LENf,0);
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,RPEf,1);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l80,INSERT_PROPAGATE,&l111,TRUE,&l155
,&tcam_index,&bktid));}}PIVOT_BUCKET_ENT_CNT_UPDATE(l278);return l139;}static
int l126(int unit,uint32*key,int len,int vrf,int l107,defip_entry_t*lpm_entry
,int l127,int l128){uint32 l148;if(l128){sal_memset(lpm_entry,0,sizeof(
defip_entry_t));}soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l41),(vrf&SOC_VRF_MAX(unit)));if(vrf == 
(SOC_VRF_MAX(unit)+1)){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l43),(0));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l43),(SOC_VRF_MAX(unit)));}if(l107){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l25),(key[0]));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l26),(key[1]));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l29),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l30),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l42),(vrf&SOC_VRF_MAX(unit)));if(vrf == (SOC_VRF_MAX(unit)+1))
{soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry
),(l67[(unit)]->l44),(0));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l44),(SOC_VRF_MAX(
unit)));}soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l40),(1));if(len>= 32){l148 = 0xffffffff;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l28),(l148));l148 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32
));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l27),(l148));}else{l148 = ~(0xffffffff>>len);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l28),(l148));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l27),(0));}}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l25),(key[0]));assert(len<= 32);l148 = (len == 32)?0xffffffff:
~(0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l27),(l148));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l39),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l31),((1<<soc_mem_field_length(
unit,L3_DEFIPm,MODE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l32),((1<<
soc_mem_field_length(unit,L3_DEFIPm,MODE_MASK1f))-1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l61),((1<<soc_mem_field_length(unit,L3_DEFIPm,
ENTRY_TYPE_MASK0f))-1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit
,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l62),((1<<soc_mem_field_length(unit,
L3_DEFIPm,ENTRY_TYPE_MASK1f))-1));return(SOC_E_NONE);}int
_soc_th_alpm_delete_in_bkt(int l80,soc_mem_t l120,int l289,int l141,void*l290
,uint32*l81,int*l94,int l130){int l139;l139 = soc_mem_alpm_delete(l80,l120,
ALPM_BKT_IDX(l289),MEM_BLOCK_ALL,l141,l290,l81,l94);if(SOC_SUCCESS(l139)){
return l139;}if(SOC_TH_ALPM_SCALE_CHECK(l80,l130)){return soc_mem_alpm_delete
(l80,l120,ALPM_BKT_IDX(l289)+1,MEM_BLOCK_ALL,l141,l290,l81,l94);}return l139;
}static int l291(int l80,int l130,void*key_data,int bktid,int tcam_index,int
l94){alpm_pivot_t*pivot_pyld;defip_alpm_ipv4_entry_t l187,l188,l292;
defip_alpm_ipv6_64_entry_t l189,l190,l293;defip_aux_scratch_entry_t l111;
uint32 l81[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l120;void*l185,*l290,*l186 = 
NULL;int vrf_id;int l139 = SOC_E_NONE,l183;uint32 l294[5],prefix[5];int vrf;
uint32 l154;int l289;uint32 l108,l109,l141;int l155,l122 = 0;trie_t*trie,*
l262;uint32 l295;defip_entry_t lpm_entry,*l296;payload_t*l264 = NULL,*l297 = 
NULL,*l272 = NULL;trie_node_t*l257 = NULL,*l157 = NULL;trie_t*l156 = NULL;
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf));if(!(
ALPM_PREFIX_IN_TCAM(l80,vrf_id))){soc_th_alpm_bank_disb_get(l80,vrf,&l141);
soc_alpm_db_ent_type_encoding(l80,vrf,&l108,&l109);l139 = l149(l80,l130,
key_data,prefix,&l154,&l122);if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l80,"_soc_alpm_delete: prefix create failed\n")));return l139;}if
(soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_COMBINED){if(vrf_id!= 
SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l80,vrf,l130)>1){if(l122){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"VRF %d: Cannot delete default "
"route if other routes are present in ""this mode"),vrf));return SOC_E_PARAM;
}}}}l120 = (l130)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l185 = ((l130)?(
(uint32*)&(l189)):((uint32*)&(l187)));l290 = ((l130)?((uint32*)&(l293)):((
uint32*)&(l292)));SOC_ALPM_LPM_LOCK(l80);if(bktid == 0){l139 = 
_soc_th_alpm_find(l80,l120,l130,key_data,vrf_id,vrf,l185,&tcam_index,&bktid,&
l94,l3_alpm_sw_prefix_lookup[l80]);}else{l139 = l117(l80,l130,key_data,l185,0
,l120,0,0,bktid);}sal_memcpy(l290,l185,l130?sizeof(l189):sizeof(l187));if(
SOC_FAILURE(l139)){SOC_ALPM_LPM_UNLOCK(l80);LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"_soc_alpm_delete: Unable to find prefix for delete\n")));
return l139;}l289 = bktid;pivot_pyld = ALPM_TCAM_PIVOT(l80,tcam_index);trie = 
PIVOT_BUCKET_TRIE(pivot_pyld);l139 = trie_delete(trie,prefix,l154,&l257);l264
= (payload_t*)l257;if(l139!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l80);return l139;}if(l130){l262 = VRF_PREFIX_TRIE_IPV6(
l80,vrf);}else{l262 = VRF_PREFIX_TRIE_IPV4(l80,vrf);}if(l130){l156 = 
VRF_PIVOT_TRIE_IPV6(l80,vrf);}else{l156 = VRF_PIVOT_TRIE_IPV4(l80,vrf);}if(!
l122){l139 = trie_delete(l262,prefix,l154,&l257);l297 = (payload_t*)l257;if(
SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l298;}l157 = NULL;l139 = trie_find_lpm(l262,prefix,l154,&
l157);l272 = (payload_t*)l157;if(SOC_SUCCESS(l139)){payload_t*l299 = (
payload_t*)(l272->bkt_ptr);if(l299!= NULL){l295 = l299->len;}else{l295 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l300;}
sal_memcpy(l294,prefix,sizeof(prefix));do{if(!(l130)){l294[0] = (((32-l154) == 
32)?0:(l294[1])<<(32-l154));l294[1] = 0;}else{int l275 = 64-l154;int l276;if(
l275<32){l276 = l294[3]<<l275;l276|= (((32-l275) == 32)?0:(l294[4])>>(32-l275
));l294[0] = l294[4]<<l275;l294[1] = l276;l294[2] = l294[3] = l294[4] = 0;}
else{l294[1] = (((l275-32) == 32)?0:(l294[4])<<(l275-32));l294[0] = l294[2] = 
l294[3] = l294[4] = 0;}}}while(0);l139 = l126(l80,l294,l295,vrf,l130,&
lpm_entry,0,1);l183 = _soc_th_alpm_find(l80,l120,l130,&lpm_entry,vrf_id,vrf,
l185,&tcam_index,&bktid,&l94,l3_alpm_sw_prefix_lookup[l80]);if(l183){if(
vrf_id!= SOC_L3_VRF_GLOBAL&&soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_COMBINED){soc_mem_entry_dump_if_changed(l80,L3_DEFIPm,&
lpm_entry,"ALPM-DBG: ");LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Find associated data for next bpm failed."
"prefix: 0x%08x 0x%08x, length %d\n"),l294[0],l294[1],l295));}}(void)l124(l80
,l185,l120,l130,vrf_id,bktid,0,&lpm_entry);(void)l126(l80,l294,l154,vrf,l130,
&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l80)){if(SOC_SUCCESS(l139)){l186 = ((
l130)?((uint32*)&(l190)):((uint32*)&(l188)));l139 = soc_mem_read(l80,l120,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,ALPM_ENT_INDEX(l94)),l186);}}if((
l295 == 0)&&SOC_FAILURE(l183)){l296 = l130?VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,
vrf):VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf);sal_memcpy(&lpm_entry,l296,sizeof(
lpm_entry));l139 = l126(l80,l294,l154,vrf,l130,&lpm_entry,0,1);}if(
SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l300;}l296 = 
&lpm_entry;}else{l157 = NULL;l139 = trie_find_lpm(l262,prefix,l154,&l157);
l272 = (payload_t*)l157;if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),vrf));goto l298;}l272->bkt_ptr = NULL;l295 = 
0;l296 = l130?VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,vrf):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf);}l139 = l106(l80,l296,l130,l108,l109,
l295,&l111);if(SOC_FAILURE(l139)){goto l300;}soc_mem_field32_set(l80,
L3_DEFIP_AUX_SCRATCHm,&l111,ALG_HIT_IDXf,ALPM_ENT_INDEX(l94));l139 = 
_soc_th_alpm_aux_op(l80,DELETE_PROPAGATE,&l111,TRUE,&l155,&tcam_index,&bktid)
;if(SOC_FAILURE(l139)){goto l300;}if(SOC_URPF_STATUS_GET(l80)){uint32 l146;if
(l186!= NULL){soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,DB_TYPEf,
l108+1);soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l111,ALG_HIT_IDXf,
_soc_th_alpm_rpf_entry(l80,ALPM_ENT_INDEX(l94)));l146 = soc_mem_field32_get(
l80,l120,l186,SRC_DISCARDf);soc_mem_field32_set(l80,l120,&l111,SRC_DISCARDf,
l146);l146 = soc_mem_field32_get(l80,l120,l186,DEFAULTROUTEf);
soc_mem_field32_set(l80,l120,&l111,DEFAULTROUTEf,l146);l139 = 
_soc_th_alpm_aux_op(l80,DELETE_PROPAGATE,&l111,TRUE,&l155,&tcam_index,&bktid)
;}if(SOC_FAILURE(l139)){goto l300;}}sal_free(l264);if(!l122){sal_free(l297);}
PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);if((pivot_pyld->len!= 0)&&(trie->trie
== NULL)){uint32 l301[5];sal_memcpy(l301,pivot_pyld->key,sizeof(l301));do{if(
!(l130)){l301[0] = (((32-pivot_pyld->len) == 32)?0:(l301[1])<<(32-pivot_pyld
->len));l301[1] = 0;}else{int l275 = 64-pivot_pyld->len;int l276;if(l275<32){
l276 = l301[3]<<l275;l276|= (((32-l275) == 32)?0:(l301[4])>>(32-l275));l301[0
] = l301[4]<<l275;l301[1] = l276;l301[2] = l301[3] = l301[4] = 0;}else{l301[1
] = (((l275-32) == 32)?0:(l301[4])<<(l275-32));l301[0] = l301[2] = l301[3] = 
l301[4] = 0;}}}while(0);l126(l80,l301,pivot_pyld->len,vrf,l130,&lpm_entry,0,1
);l139 = soc_th_alpm_lpm_delete(l80,&lpm_entry);if(SOC_FAILURE(l139)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_delete: Unable to "
"delete pivot 0x%08x 0x%08x \n"),pivot_pyld->key[0],pivot_pyld->key[1]));}}
l139 = _soc_th_alpm_delete_in_bkt(l80,l120,l289,l141,l290,l81,&l94,l130);if(
SOC_FAILURE(l139)){SOC_ALPM_LPM_UNLOCK(l80);return l139;}if(
SOC_URPF_STATUS_GET(l80)){l139 = soc_mem_alpm_delete(l80,l120,
SOC_TH_ALPM_RPF_BKT_IDX(l80,ALPM_BKT_IDX(l289)),MEM_BLOCK_ALL,l141,l290,l81,&
l155);if(SOC_FAILURE(l139)){SOC_ALPM_LPM_UNLOCK(l80);return l139;}}
soc_th_alpm_bu_upd(l80,PIVOT_BUCKET_INDEX(pivot_pyld),PIVOT_TCAM_INDEX(
pivot_pyld),((pivot_pyld->len!= 0)&&(trie->trie == NULL))?TRUE:FALSE,l130,-1)
;if((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l139 = soc_th_alpm_bs_free(
l80,PIVOT_BUCKET_INDEX(pivot_pyld),vrf,l130);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_delete: Unable to release"
"empty bucket: %d\n"),PIVOT_BUCKET_INDEX(pivot_pyld)));}l139 = trie_delete(
l156,pivot_pyld->key,pivot_pyld->len,&l257);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"could not delete pivot from pivot trie\n")))
;}trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(
pivot_pyld));sal_free(pivot_pyld);l94 = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,
ALPM_BKT_IDX(l289));_soc_tomahawk_alpm_bkt_view_set(l80,l94,INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l80,l130)){l94 = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,
ALPM_BKT_IDX(l289)+1);_soc_tomahawk_alpm_bkt_view_set(l80,l94,INVALIDm);}}}
VRF_TRIE_ROUTES_DEC(l80,vrf,l130);if(VRF_TRIE_ROUTES_CNT(l80,vrf,l130) == 0){
l139 = l129(l80,vrf,l130);}SOC_ALPM_LPM_UNLOCK(l80);return l139;l300:
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"recovering soc_th_alpm_vrf_delete failed\n ")));l183 = trie_insert(l262,
prefix,NULL,l154,(trie_node_t*)l297);if(SOC_FAILURE(l183)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into pfx trie\n"),prefix[0],prefix[1]));}l298:l183 = 
trie_insert(trie,prefix,NULL,l154,(trie_node_t*)l264);if(SOC_FAILURE(l183)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into bkt trie\n")
,prefix[0],prefix[1]));}SOC_ALPM_LPM_UNLOCK(l80);return l139;}void l302(int
l80){int l163;if(alpm_vrf_handle[l80]!= NULL){sal_free(alpm_vrf_handle[l80]);
alpm_vrf_handle[l80] = NULL;}if(tcam_pivot[l80]!= NULL){sal_free(tcam_pivot[
l80]);tcam_pivot[l80] = NULL;}if(bkt_usage[l80]!= NULL){sal_free(bkt_usage[
l80]);bkt_usage[l80] = NULL;}for(l163 = 0;l163<SOC_TH_MAX_ALPM_VIEWS;l163++){
if(global_bkt_usage[l80][l163]!= NULL){sal_free(global_bkt_usage[l80][l163]);
global_bkt_usage[l80][l163] = NULL;}}}int soc_th_alpm_init(int l80){int l163;
int l139 = SOC_E_NONE;uint32 l250;l139 = l102(l80);SOC_IF_ERROR_RETURN(l139);
l139 = l116(l80);l302(l80);alpm_vrf_handle[l80] = sal_alloc((MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t),"ALPM VRF Handles");if(alpm_vrf_handle[l80] == NULL
){l139 = SOC_E_MEMORY;goto l195;}tcam_pivot[l80] = sal_alloc(MAX_PIVOT_COUNT*
sizeof(alpm_pivot_t*),"ALPM pivots");if(tcam_pivot[l80] == NULL){l139 = 
SOC_E_MEMORY;goto l195;}sal_memset(alpm_vrf_handle[l80],0,(MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t));sal_memset(tcam_pivot[l80],0,MAX_PIVOT_COUNT*
sizeof(alpm_pivot_t*));l250 = SOC_TH_MAX_ALPM_BUCKETS*sizeof(alpm_bkt_usg_t);
bkt_usage[l80] = sal_alloc(l250,"ALPM_BS");if(bkt_usage[l80] == NULL){l139 = 
SOC_E_MEMORY;goto l195;}sal_memset(bkt_usage[l80],0,l250);l250 = 
SOC_TH_MAX_BUCKET_ENTRIES*sizeof(alpm_bkt_bmp_t);for(l163 = 0;l163<
SOC_TH_MAX_ALPM_VIEWS;l163++){global_bkt_usage[l80][l163] = sal_alloc(l250,
"ALPM_BS");if(global_bkt_usage[l80][l163] == NULL){l139 = SOC_E_MEMORY;goto
l195;}sal_memset(global_bkt_usage[l80][l163],0,l250);}for(l163 = 0;l163<
MAX_PIVOT_COUNT;l163++){ALPM_TCAM_PIVOT(l80,l163) = NULL;}if(SOC_CONTROL(l80)
->alpm_bulk_retry == NULL){SOC_CONTROL(l80)->alpm_bulk_retry = sal_sem_create
("ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l80)->alpm_lookup_retry
== NULL){SOC_CONTROL(l80)->alpm_lookup_retry = sal_sem_create(
"ALPM lookup retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l80)->alpm_insert_retry
== NULL){SOC_CONTROL(l80)->alpm_insert_retry = sal_sem_create(
"ALPM insert retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l80)->alpm_delete_retry
== NULL){SOC_CONTROL(l80)->alpm_delete_retry = sal_sem_create(
"ALPM delete retry",sal_sem_BINARY,0);}l139 = soc_th_alpm_128_lpm_init(l80);
if(SOC_FAILURE(l139)){goto l195;}return l139;l195:l302(l80);return l139;}
static int l303(int l80){int l163,l139;alpm_pivot_t*l146;for(l163 = 0;l163<
MAX_PIVOT_COUNT;l163++){l146 = ALPM_TCAM_PIVOT(l80,l163);if(l146){if(
PIVOT_BUCKET_HANDLE(l146)){if(PIVOT_BUCKET_TRIE(l146)){l139 = trie_traverse(
PIVOT_BUCKET_TRIE(l146),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l139)){trie_destroy(PIVOT_BUCKET_TRIE(l146));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Unable to clear trie state for unit %d\n"),
l80));return l139;}}sal_free(PIVOT_BUCKET_HANDLE(l146));}sal_free(
ALPM_TCAM_PIVOT(l80,l163));ALPM_TCAM_PIVOT(l80,l163) = NULL;}}for(l163 = 0;
l163<= SOC_VRF_MAX(l80)+1;l163++){l139 = trie_traverse(VRF_PREFIX_TRIE_IPV4(
l80,l163),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(
l139)){trie_destroy(VRF_PREFIX_TRIE_IPV4(l80,l163));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l80,l163));return l139;}
l139 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l80,l163),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l139)){trie_destroy(
VRF_PREFIX_TRIE_IPV6(l80,l163));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l80,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l80,l163));return
l139;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,l163)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,l163));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,
l163)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,l163));}sal_memset(&
alpm_vrf_handle[l80][l163],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l80][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l80,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l80,MAX_VRF_ID,1,1)
;VRF_TRIE_INIT_DONE(l80,MAX_VRF_ID,2,1);if(SOC_TH_ALPM_VRF_BUCKET_BMAP(l80)!= 
NULL){sal_free(SOC_TH_ALPM_VRF_BUCKET_BMAP(l80));}if(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l80)!= NULL&&SOC_TH_ALPM_GLB_BUCKET_BMAP(l80)!= 
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80)){sal_free(SOC_TH_ALPM_GLB_BUCKET_BMAP(l80));
}sal_memset(&soc_th_alpm_bucket[l80],0,sizeof(soc_alpm_bucket_t));l302(l80);
return SOC_E_NONE;}int soc_th_alpm_deinit(int l80){l103(l80);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_deinit(l80));SOC_IF_ERROR_RETURN(
soc_th_alpm_128_state_clear(l80));SOC_IF_ERROR_RETURN(l303(l80));if(
SOC_CONTROL(l80)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l80)->
alpm_bulk_retry);SOC_CONTROL(l80)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l80
)->alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l80)->
alpm_lookup_retry);SOC_CONTROL(l80)->alpm_lookup_retry = NULL;}if(SOC_CONTROL
(l80)->alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l80)->
alpm_insert_retry);SOC_CONTROL(l80)->alpm_insert_retry = NULL;}if(SOC_CONTROL
(l80)->alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l80)->
alpm_delete_retry);SOC_CONTROL(l80)->alpm_delete_retry = NULL;}return
SOC_E_NONE;}static int l304(int l80,int vrf,int l130){defip_entry_t*lpm_entry
= NULL,l305;int l306 = 0;int index;int l139 = SOC_E_NONE;uint32 key[2] = {0,0
};uint32 l154;alpm_bucket_handle_t*l265 = NULL;alpm_pivot_t*pivot_pyld = NULL
;payload_t*l297 = NULL;trie_t*l307;trie_t*l308 = NULL;if(l130 == 0){trie_init
(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(l80,vrf));l308 = VRF_PIVOT_TRIE_IPV4(
l80,vrf);}else{trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(l80,vrf));
l308 = VRF_PIVOT_TRIE_IPV6(l80,vrf);}if(l130 == 0){trie_init(_MAX_KEY_LEN_48_
,&VRF_PREFIX_TRIE_IPV4(l80,vrf));l307 = VRF_PREFIX_TRIE_IPV4(l80,vrf);}else{
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l80,vrf));l307 = 
VRF_PREFIX_TRIE_IPV6(l80,vrf);}lpm_entry = sal_alloc(sizeof(defip_entry_t),
"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_vrf_add: unable to allocate memory for "
"IPv4 LPM entry\n")));return SOC_E_MEMORY;}l126(l80,key,0,vrf,l130,lpm_entry,
0,1);if(l130 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,vrf) = lpm_entry;}if(vrf == SOC_VRF_MAX(l80)+
1){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
lpm_entry),(l67[(l80)]->l21),(1));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l53),(1));}l139 = 
soc_th_alpm_bs_alloc(l80,&l306,vrf,l130);if(SOC_FAILURE(l139)){goto l309;}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l49),(ALPM_BKT_IDX(l306)));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l51),(ALPM_BKT_SIDX(
l306)));sal_memcpy(&l305,lpm_entry,sizeof(l305));l139 = l104(l80,&l305,&index
);if(SOC_FAILURE(l139)){goto l309;}l265 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l265 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_th_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
l139 = SOC_E_MEMORY;goto l309;}sal_memset(l265,0,sizeof(*l265));pivot_pyld = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(pivot_pyld == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
l139 = SOC_E_MEMORY;goto l309;}l297 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l297 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_vrf_add: Unable to allocate memory for "
"pfx trie node \n")));l139 = SOC_E_MEMORY;goto l309;}sal_memset(pivot_pyld,0,
sizeof(*pivot_pyld));sal_memset(l297,0,sizeof(*l297));l154 = 0;
PIVOT_BUCKET_HANDLE(pivot_pyld) = l265;if(l130){trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(pivot_pyld));}else{trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(pivot_pyld));}PIVOT_BUCKET_INDEX(pivot_pyld) = l306;
PIVOT_BUCKET_VRF(pivot_pyld) = vrf;PIVOT_BUCKET_IPV6(pivot_pyld) = l130;
PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;pivot_pyld->key[0] = l297->key[0] = key[0
];pivot_pyld->key[1] = l297->key[1] = key[1];pivot_pyld->len = l297->len = 
l154;l139 = trie_insert(l307,key,NULL,l154,&(l297->node));if(SOC_FAILURE(l139
)){goto l309;}l139 = trie_insert(l308,key,NULL,l154,(trie_node_t*)pivot_pyld)
;if(SOC_FAILURE(l139)){trie_node_t*l257 = NULL;(void)trie_delete(l307,key,
l154,&l257);goto l309;}index = soc_th_alpm_physical_idx(l80,L3_DEFIPm,index,
l130);if(l130 == 0){ALPM_TCAM_PIVOT(l80,index) = pivot_pyld;PIVOT_TCAM_INDEX(
pivot_pyld) = index;}else{ALPM_TCAM_PIVOT(l80,index<<1) = pivot_pyld;
PIVOT_TCAM_INDEX(pivot_pyld) = index<<1;}VRF_PIVOT_REF_INC(l80,vrf,l130);
VRF_TRIE_INIT_DONE(l80,vrf,l130,1);return l139;l309:if(l297!= NULL){sal_free(
l297);}if(pivot_pyld!= NULL){sal_free(pivot_pyld);}if(l265!= NULL){sal_free(
l265);}if(lpm_entry!= NULL){sal_free(lpm_entry);}if(l130 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf) = NULL;}else{VRF_TRIE_DEFAULT_ROUTE_IPV6
(l80,vrf) = NULL;}return l139;}static int l129(int l80,int vrf,int l130){
defip_entry_t*lpm_entry;int l142,vrf_id,l310;int l139 = SOC_E_NONE;uint32 key
[2] = {0,0},l133[SOC_MAX_MEM_FIELD_WORDS];payload_t*l264;alpm_pivot_t*l311;
trie_node_t*l257;trie_t*l307;trie_t*l308 = NULL;soc_mem_t l120;int tcam_index
,bktid,index;uint32 l160[SOC_MAX_MEM_FIELD_WORDS];if(l130 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf);l120 = L3_DEFIP_ALPM_IPV4m;}else{
lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,vrf);l120 = 
L3_DEFIP_ALPM_IPV6_64m;}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,
lpm_entry,&vrf_id,&l310));l139 = _soc_th_alpm_find(l80,l120,l130,lpm_entry,
vrf_id,l310,l160,&tcam_index,&bktid,&index,l3_alpm_sw_prefix_lookup[l80]);
l139 = soc_th_alpm_bs_free(l80,bktid,vrf,l130);index = 
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,ALPM_BKT_IDX(bktid));
_soc_tomahawk_alpm_bkt_view_set(l80,index,INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l80,l130)){index = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,
ALPM_BKT_IDX(bktid)+1);_soc_tomahawk_alpm_bkt_view_set(l80,index,INVALIDm);}
l139 = l115(l80,lpm_entry,(void*)l133,&l142);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"soc_alpm_vrf_delete: unable to get internal"
" pivot idx for vrf %d/%d\n"),vrf,l130));l142 = -1;}l142 = 
soc_th_alpm_physical_idx(l80,L3_DEFIPm,l142,l130);if(l130 == 0){l311 = 
ALPM_TCAM_PIVOT(l80,l142);}else{l311 = ALPM_TCAM_PIVOT(l80,l142<<1);}l139 = 
soc_th_alpm_lpm_delete(l80,lpm_entry);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),vrf,l130));}sal_free(lpm_entry);if(
l130 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf) = NULL;l307 = 
VRF_PREFIX_TRIE_IPV4(l80,vrf);VRF_PREFIX_TRIE_IPV4(l80,vrf) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,vrf) = NULL;l307 = VRF_PREFIX_TRIE_IPV6(l80,
vrf);VRF_PREFIX_TRIE_IPV6(l80,vrf) = NULL;}VRF_TRIE_INIT_DONE(l80,vrf,l130,0)
;l139 = trie_delete(l307,key,0,&l257);l264 = (payload_t*)l257;if(SOC_FAILURE(
l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Unable to delete internal default for vrf "" %d/%d\n"),vrf,l130));}sal_free(
l264);(void)trie_destroy(l307);if(l130 == 0){l308 = VRF_PIVOT_TRIE_IPV4(l80,
vrf);VRF_PIVOT_TRIE_IPV4(l80,vrf) = NULL;}else{l308 = VRF_PIVOT_TRIE_IPV6(l80
,vrf);VRF_PIVOT_TRIE_IPV6(l80,vrf) = NULL;}l257 = NULL;l139 = trie_delete(
l308,key,0,&l257);if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l80,"Unable to delete internal pivot node for vrf"" %d/%d\n"),vrf,l130));}(
void)trie_destroy(l308);(void)trie_destroy(PIVOT_BUCKET_TRIE(l311));sal_free(
PIVOT_BUCKET_HANDLE(l311));sal_free(l311);return l139;}int soc_th_alpm_insert
(int l80,void*l105,uint32 l121,int l212,int l312){defip_alpm_ipv4_entry_t l187
,l188;defip_alpm_ipv6_64_entry_t l189,l190;soc_mem_t l120;void*l185,*l290;int
vrf_id,vrf;int index;int l107;int l139 = SOC_E_NONE;uint32 l122;int l123 = 0;
l107 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l105
),(l67[(l80)]->l29));l120 = (l107)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m
;l185 = ((l107)?((uint32*)&(l189)):((uint32*)&(l187)));l290 = ((l107)?((
uint32*)&(l190)):((uint32*)&(l188)));if(l212!= -1){l123 = ALPM_BKTID(
ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l80,l212)),(l212>>
ALPM_ENT_INDEX_BITS));}SOC_IF_ERROR_RETURN(l117(l80,l107,l105,l185,l290,l120,
l121,&l122,l123));SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,l105,&
vrf_id,&vrf));if(ALPM_PREFIX_IN_TCAM(l80,vrf_id)){l139 = l104(l80,l105,&index
);if(SOC_SUCCESS(l139)){if(vrf_id == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_REF_INC(
l80,MAX_VRF_ID,l107);VRF_TRIE_ROUTES_INC(l80,MAX_VRF_ID,l107);}else{
VRF_PIVOT_REF_INC(l80,vrf,l107);VRF_TRIE_ROUTES_INC(l80,vrf,l107);}}else if(
l139 == SOC_E_FULL){if(vrf_id == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_FULL_INC(l80,
MAX_VRF_ID,l107);}else{VRF_PIVOT_FULL_INC(l80,vrf,l107);}}return(l139);}else
if(vrf == 0){if(soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_PARALLEL||
soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_TCAM_ALPM){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"VRF=0 cannot be added in current mode\n")));
return SOC_E_PARAM;}}if(vrf_id!= SOC_L3_VRF_GLOBAL){if(soc_th_alpm_mode_get(
l80) == SOC_ALPM_MODE_COMBINED){if(VRF_TRIE_ROUTES_CNT(l80,vrf,l107) == 0){if
(!l122){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
vrf_id));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l80,vrf,l107)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"soc_alpm_insert:VRF %d is not "
"initialized\n"),vrf));l139 = l304(l80,vrf,l107);if(SOC_FAILURE(l139)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n"),vrf,l107));return l139;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),vrf,l107));}if(l312&
SOC_ALPM_LOOKUP_HIT){l139 = l159(l80,l107,l105,l185,l290,l120,l212);}else{if(
l212 == -1){l212 = 0;}l212 = ALPM_BKTID(ALPM_BKT_IDX(
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l80,l212)),l212>>ALPM_ENT_INDEX_BITS);l139 = 
l283(l80,l107,l105,l120,l185,l290,&index,l212,l312);}if(l139!= SOC_E_NONE){
LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l80,soc_errmsg(l139
)));}return(l139);}int soc_th_alpm_lookup(int l80,void*key_data,void*l81,int*
l113,int*l313){defip_alpm_ipv4_entry_t l187;defip_alpm_ipv6_64_entry_t l189;
soc_mem_t l120;int bktid = 0;int tcam_index = -1;void*l185;int vrf_id,vrf;int
l107,l93;int l139 = SOC_E_NONE;*l113 = 0;l139 = l112(l80,key_data,l81,l113,&
l93,&l107,&vrf_id,&vrf);if(SOC_SUCCESS(l139)){if(!l107&&(*l113&0x1)){l139 = 
soc_th_alpm_lpm_ip4entry1_to_0(l80,l81,l81,PRESERVE_HIT);}}if(
ALPM_PREFIX_IN_TCAM(l80,vrf_id)){*l313 = 0;return l139;}if(!
VRF_TRIE_INIT_COMPLETED(l80,vrf,l107)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_lookup:VRF %d is not initialized\n"),vrf));*l313 = 0
;return SOC_E_NOT_FOUND;}l120 = (l107)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l185 = ((l107)?((uint32*)&(l189)):((uint32*)&(l187)));
SOC_ALPM_LPM_LOCK(l80);l139 = _soc_th_alpm_find(l80,l120,l107,key_data,vrf_id
,vrf,l185,&tcam_index,&bktid,l113,l3_alpm_sw_prefix_lookup[l80]);
SOC_ALPM_LPM_UNLOCK(l80);if(SOC_FAILURE(l139)){*l313 = tcam_index;*l113 = (
ALPM_BKT_SIDX(bktid)<<ALPM_ENT_INDEX_BITS)|SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,
bktid);return l139;}l139 = l124(l80,l185,l120,l107,vrf_id,bktid,*l113,l81);*
l313 = SOC_ALPM_LOOKUP_HIT|tcam_index;*l113 = (ALPM_BKT_SIDX(bktid)<<
ALPM_ENT_INDEX_BITS)|*l113;return(l139);}static int l314(int l80,void*
key_data,void*l81,int vrf,int*tcam_index,int*bucket_index,int*l94,int l315){
int l139 = SOC_E_NONE;int l163,l316,l130,l155 = 0;uint32 l108,l109,l141;
defip_aux_scratch_entry_t l111;int l317,l318;int index;soc_mem_t l120,l177;
int l319,l320;int l321;uint32 l322[SOC_MAX_MEM_FIELD_WORDS] = {0};int l323 = 
-1;int l324 = 0;soc_field_t l325[2] = {IP_ADDR0f,IP_ADDR1f,};l177 = L3_DEFIPm
;l130 = soc_mem_field32_get(l80,l177,key_data,MODE0f);l317 = 
soc_mem_field32_get(l80,l177,key_data,GLOBAL_ROUTE0f);l318 = 
soc_mem_field32_get(l80,l177,key_data,VRF_ID_0f);LOG_VERBOSE(BSL_LS_SOC_ALPM,
(BSL_META_U(l80,"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d:\n"),vrf == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l130,l317,l318));soc_th_alpm_bank_disb_get(l80,vrf == 
SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l80)+1):vrf,&l141);
soc_alpm_db_ent_type_encoding(l80,vrf == SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l80)+
1):l318,&l108,&l109);if(l315){l108+= 1;}if(vrf == SOC_L3_VRF_GLOBAL){
soc_mem_field32_set(l80,l177,key_data,GLOBAL_ROUTE0f,1);soc_mem_field32_set(
l80,l177,key_data,VRF_ID_0f,0);}sal_memset(&l111,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l106(l80,key_data,l130,l108,
l109,0,&l111));if(vrf == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l80,l177,
key_data,GLOBAL_ROUTE0f,l317);soc_mem_field32_set(l80,l177,key_data,VRF_ID_0f
,l318);}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l80,PREFIX_LOOKUP,&l111,TRUE,
&l155,tcam_index,bucket_index));if(l155 == 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"Could not find bucket\n")));return SOC_E_NOT_FOUND;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Hit in memory %s, index %d, "
"bucket [%d,%d]\n"),SOC_MEM_NAME(l80,l177),soc_th_alpm_logical_idx(l80,l177,(
*tcam_index)>>1,1),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index)))
;l120 = (l130)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l139 = l138(l80,
l130,key_data,&l320);if(SOC_FAILURE(l139)){return l139;}switch(l130){case
L3_DEFIP_MODE_V4:l321 = ALPM_IPV4_BKT_COUNT;break;case L3_DEFIP_MODE_64:l321 = 
ALPM_IPV6_64_BKT_COUNT;break;default:l321 = ALPM_IPV4_BKT_COUNT;break;}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l130)){l321<<= 1;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"Start searching mem %s bucket [%d,%d](count %d) "
"for Length %d\n"),SOC_MEM_NAME(l80,l120),ALPM_BKT_IDX(*bucket_index),
ALPM_BKT_SIDX(*bucket_index),l321,l320));for(l163 = 0;l163<l321;l163++){
uint32 l185[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l148[2] = {0};uint32 l326[2
] = {0};uint32 l327[2] = {0};int l328;l139 = _soc_th_alpm_mem_index(l80,l120,
ALPM_BKT_IDX(*bucket_index),l163,l141,&index);if(l139 == SOC_E_FULL){continue
;}SOC_IF_ERROR_RETURN(soc_mem_read(l80,l120,MEM_BLOCK_ANY,index,(void*)l185))
;l328 = soc_mem_field32_get(l80,l120,l185,VALIDf);l319 = soc_mem_field32_get(
l80,l120,l185,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Bucket [%d,%d] index %6d: Valid %d, Length %d\n"),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),index,l328,l319));if(!l328||(l319>l320)){
continue;}SHR_BITSET_RANGE(l148,(l130?64:32)-l319,l319);(void)
soc_mem_field_get(l80,l120,(uint32*)l185,KEYf,(uint32*)l326);l327[1] = 
soc_mem_field32_get(l80,l177,key_data,l325[1]);l327[0] = soc_mem_field32_get(
l80,l177,key_data,l325[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tmask %08x %08x\n\t key %08x %08x\n""\thost %08x %08x\n"),l148[1],l148[0],
l326[1],l326[0],l327[1],l327[0]));for(l316 = l130?1:0;l316>= 0;l316--){if((
l327[l316]&l148[l316])!= (l326[l316]&l148[l316])){break;}}if(l316>= 0){
continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Found a match in mem %s bucket [%d,%d], ""index %d\n"),SOC_MEM_NAME(l80,l120
),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index),index));if(l323 == 
-1||l323<l319){l323 = l319;l324 = index;sal_memcpy(l322,l185,sizeof(l185));}}
if(l323!= -1){l139 = l124(l80,l322,l120,l130,vrf,*bucket_index,l324,l81);if(
SOC_SUCCESS(l139)){*l94 = l324;if(bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l80)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Hit mem %s bucket [%d,%d], index %d\n"),SOC_MEM_NAME(l80,l120),ALPM_BKT_IDX(
*bucket_index),ALPM_BKT_SIDX(*bucket_index),l324));}}return l139;}*l94 = 
soc_th_alpm_logical_idx(l80,l177,(*tcam_index)>>1,1);LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Miss in mem %s bucket [%d,%d], use associate data "
"in mem %s LOG index %d\n"),SOC_MEM_NAME(l80,l120),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),SOC_MEM_NAME(l80,l177),*l94));
SOC_IF_ERROR_RETURN(soc_mem_read(l80,l177,MEM_BLOCK_ANY,*l94,(void*)l81));if(
(!l130)&&((*tcam_index)&1)){l139 = soc_th_alpm_lpm_ip4entry1_to_0(l80,l81,l81
,PRESERVE_HIT);}return SOC_E_NONE;}int soc_th_alpm_find_best_match(int l80,
void*key_data,void*l81,int*l113,int l315){int l139 = SOC_E_NONE;int l163,l329
,l169;defip_entry_t l330;uint32 l331[2];uint32 l326[2];uint32 l332[2];uint32
l327[2];uint32 l333,l334;int vrf_id,vrf = 0;int l335[2] = {0};int tcam_index,
bucket_index;soc_mem_t l177 = L3_DEFIPm;int l217,l130,l336,l337 = 0;
soc_field_t l338[] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l339[] = {
GLOBAL_ROUTE0f,GLOBAL_ROUTE1f};l130 = soc_mem_field32_get(l80,l177,key_data,
MODE0f);if(!SOC_URPF_STATUS_GET(l80)&&l315){return SOC_E_PARAM;}l329 = 
soc_mem_index_min(l80,l177);l169 = soc_mem_index_count(l80,l177);if(
SOC_URPF_STATUS_GET(l80)){l169>>= 1;}if(soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_PARALLEL||soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_TCAM_ALPM)
{l169>>= 1;l329+= l169;}if(l315){l329+= soc_mem_index_count(l80,l177)/2;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Launch LPM searching from index %d count %d\n"),l329,l169));for(l163 = l329;
l163<l329+l169;l163++){SOC_IF_ERROR_RETURN(soc_mem_read(l80,l177,
MEM_BLOCK_ANY,l163,(void*)&l330));l335[0] = soc_mem_field32_get(l80,l177,&
l330,VALID0f);l335[1] = soc_mem_field32_get(l80,l177,&l330,VALID1f);if(l335[0
] == 0&&l335[1] == 0){continue;}l336 = soc_mem_field32_get(l80,l177,&l330,
MODE0f);if(l336!= l130){continue;}for(l217 = 0;l217<(l130?1:2);l217++){if(
l335[l217] == 0){continue;}l333 = soc_mem_field32_get(l80,l177,&l330,l338[
l217]);l334 = soc_mem_field32_get(l80,l177,&l330,l339[l217]);if(!(
soc_th_alpm_mode_get(l80)!= SOC_ALPM_MODE_TCAM_ALPM&&l333&&l334)&&!(
soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_TCAM_ALPM&&l334)){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Match a Global High route: ent %d\n"),l217));l331[0] = soc_mem_field32_get(
l80,l177,&l330,IP_ADDR_MASK0f);l331[1] = soc_mem_field32_get(l80,l177,&l330,
IP_ADDR_MASK1f);l326[0] = soc_mem_field32_get(l80,l177,&l330,IP_ADDR0f);l326[
1] = soc_mem_field32_get(l80,l177,&l330,IP_ADDR1f);l332[0] = 
soc_mem_field32_get(l80,l177,key_data,IP_ADDR_MASK0f);l332[1] = 
soc_mem_field32_get(l80,l177,key_data,IP_ADDR_MASK1f);l327[0] = 
soc_mem_field32_get(l80,l177,key_data,IP_ADDR0f);l327[1] = 
soc_mem_field32_get(l80,l177,key_data,IP_ADDR1f);LOG_VERBOSE(BSL_LS_SOC_ALPM,
(BSL_META_U(l80,"\thmsk %08x %08x\n\thkey %08x %08x\n"
"\tsmsk %08x %08x\n\tskey %08x %08x\n"),l331[1],l331[0],l326[1],l326[0],l332[
1],l332[0],l327[1],l327[0]));if(l130&&(((l331[1]&l332[1])!= l331[1])||((l331[
0]&l332[0])!= l331[0]))){continue;}if(!l130&&((l331[l217]&l332[0])!= l331[
l217])){continue;}if(l130&&((l327[0]&l331[0]) == (l326[0]&l331[0]))&&((l327[1
]&l331[1]) == (l326[1]&l331[1]))){l337 = TRUE;break;}if(!l130&&((l327[0]&l331
[l217]) == (l326[l217]&l331[l217]))){l337 = TRUE;break;}}if(!l337){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Hit Global High route in index = %d(%d)\n"),l163,l217));sal_memcpy(l81,&l330
,sizeof(l330));if(!l130&&l217 == 1){l139 = soc_th_alpm_lpm_ip4entry1_to_0(l80
,l81,l81,PRESERVE_HIT);}*l113 = l163;return l139;}LOG_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l80,"Global high lookup miss, use AUX engine to "
"search for VRF and Global Low routes\n")));SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf));l139 = l314(l80,key_data,
l81,vrf,&tcam_index,&bucket_index,l113,l315);if(l139 == SOC_E_NOT_FOUND){vrf = 
SOC_L3_VRF_GLOBAL;LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Not found in VRF region, trying Global ""region\n")));l139 = l314(l80,
key_data,l81,vrf,&tcam_index,&bucket_index,l113,l315);}if(SOC_SUCCESS(l139)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Hit in %s region in TCAM index %d, ""bucket [%d,%d]\n"),vrf == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",tcam_index,ALPM_BKT_IDX(bucket_index),
ALPM_BKT_SIDX(bucket_index)));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l80,"Search miss for given address\n")));}return(l139);}int soc_th_alpm_delete
(int l80,void*key_data,int l212,int l312){int l340;int vrf_id,vrf;int l107;
int l139 = SOC_E_NONE;l107 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l29));SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf));if(ALPM_PREFIX_IN_TCAM(
l80,vrf_id)){l139 = soc_th_alpm_lpm_delete(l80,key_data);if(SOC_SUCCESS(l139)
){if(vrf_id == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_REF_DEC(l80,MAX_VRF_ID,l107);
VRF_TRIE_ROUTES_DEC(l80,MAX_VRF_ID,l107);}else{VRF_PIVOT_REF_DEC(l80,vrf,l107
);VRF_TRIE_ROUTES_DEC(l80,vrf,l107);}}return(l139);}else{int tcam_index;if(!
VRF_TRIE_INIT_COMPLETED(l80,vrf,l107)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_delete:VRF %d/%d is not initialized\n"),vrf,l107));
return SOC_E_NONE;}if(l212 == -1){l212 = 0;}l340 = l212;l212 = ALPM_BKTID(
ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l80,l340)),l340>>
ALPM_ENT_INDEX_BITS);tcam_index = l312&~(SOC_ALPM_LOOKUP_HIT|
SOC_ALPM_DELETE_ALL);l139 = l291(l80,l107,key_data,l212,tcam_index,l340);}
return(l139);}static int l116(int l80){int l341;l341 = soc_mem_index_count(
l80,L3_DEFIPm)+soc_mem_index_count(l80,L3_DEFIP_PAIR_128m)*2;
SOC_TH_ALPM_BUCKET_COUNT(l80) = SOC_TH_ALPM_MAX_BKTS;if(SOC_URPF_STATUS_GET(
l80)){l341>>= 1;SOC_TH_ALPM_BUCKET_COUNT(l80)>>= 1;}
SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80) = SHR_BITALLOCSIZE(SOC_TH_ALPM_BUCKET_COUNT
(l80));SOC_TH_ALPM_VRF_BUCKET_BMAP(l80) = sal_alloc(
SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80),"alpm_vrf_shared_bucket_bitmap");if(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80) == NULL){return SOC_E_MEMORY;}sal_memset(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80),0,SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80));
soc_th_alpm_bucket_assign(l80,&l341,1,1);if(soc_property_get(l80,
"l3_alpm_vrf_share_bucket",0)&&soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_PARALLEL&&SOC_URPF_STATUS_GET(l80)){SOC_TH_ALPM_GLB_BUCKET_BMAP
(l80) = sal_alloc(SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80),"alpm_glb_shr_bkt_bmap");
if(SOC_TH_ALPM_GLB_BUCKET_BMAP(l80) == NULL){sal_free(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80));return SOC_E_MEMORY;}sal_memset(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l80),0,SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80));
soc_th_alpm_bucket_assign(l80,&l341,SOC_VRF_MAX(l80)+1,1);}else{
SOC_TH_ALPM_GLB_BUCKET_BMAP(l80) = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80);}return
SOC_E_NONE;}int soc_th_alpm_bucket_assign(int l80,int*l306,int vrf,int l130){
int l163,l219 = 1,l220 = 0;SHR_BITDCL*l342 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80)
;if(vrf == SOC_VRF_MAX(l80)+1){l342 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l80);}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l130)){l219 = 2;}for(l163 = 0;l163<
SOC_TH_ALPM_BUCKET_COUNT(l80);l163+= l219){SHR_BITTEST_RANGE(l342,l163,l219,
l220);if(0 == l220){break;}}if(l163 == SOC_TH_ALPM_BUCKET_COUNT(l80)){return
SOC_E_FULL;}SHR_BITSET_RANGE(l342,l163,l219);*l306 = l163;
SOC_TH_ALPM_BUCKET_NEXT_FREE(l80) = l163;return SOC_E_NONE;}int
soc_th_alpm_bucket_release(int l80,int l306,int vrf,int l130){int l219 = 1,
l220 = 0;SHR_BITDCL*l342 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80);if((l306<1)||(
l306>SOC_TH_ALPM_BUCKET_MAX_INDEX(l80))){return SOC_E_PARAM;}if(vrf == 
SOC_VRF_MAX(l80)+1){l342 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l80);}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l130)){l219 = 2;}SHR_BITTEST_RANGE(l342,l306,l219
,l220);if(!l220){return SOC_E_PARAM;}SHR_BITCLR_RANGE(l342,l306,l219);return
SOC_E_NONE;}int soc_th_alpm_bucket_is_assigned(int l80,int l343,int vrf,int
l107,int*l220){int l219 = 1;SHR_BITDCL*l342 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80
);if(vrf == SOC_VRF_MAX(l80)+1){l342 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l80);}if((
l343<1)||(l343>SOC_TH_ALPM_BUCKET_MAX_INDEX(l80))){return SOC_E_PARAM;}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l107)){l219 = 2;}SHR_BITTEST_RANGE(l342,l343,l219
,*l220);return SOC_E_NONE;}static void l79(int l80,void*l81,int index,l74 l82
){if(index&(0x8000)){l82[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l25));l82[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l27));l82[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l26));l82[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l28));if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int
l344;(void)soc_th_alpm_lpm_vrf_get(l80,l81,(int*)&l82[4],&l344);}else{l82[4] = 
0;};}else{if(index&0x1){l82[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l26));l82[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l28));l82[2] = 0;l82[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l80)))&&((
(l67[(l80)]->l42)!= NULL))){int l344;defip_entry_t l345;(void)
soc_th_alpm_lpm_ip4entry1_to_0(l80,l81,&l345,0);(void)soc_th_alpm_lpm_vrf_get
(l80,&l345,(int*)&l82[4],&l344);}else{l82[4] = 0;};}else{l82[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l25));l82[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l27));l82[2] = 0;l82[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int l344;(void)
soc_th_alpm_lpm_vrf_get(l80,l81,(int*)&l82[4],&l344);}else{l82[4] = 0;};}}}
static int l346(l74 l76,l74 l77){int l142;for(l142 = 0;l142<5;l142++){if(l76[
l142]<l77[l142]){return-1;}if(l76[l142]>l77[l142]){return 1;}}return(0);}
static void l347(int l80,void*l105,uint32 l348,uint32 l96,int l93){l74 l349;
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l105),(
l67[(l80)]->l29))){if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l105),(l67[(l80)]->l40))&&soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l105),(l67[(l80)]->l39))){l349[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l105),(l67[
(l80)]->l25));l349[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l105),(l67[(l80)]->l27));l349[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l105),(l67[
(l80)]->l26));l349[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l105),(l67[(l80)]->l28));if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(
l80)]->l41)!= NULL))){int l344;(void)soc_th_alpm_lpm_vrf_get(l80,l105,(int*)&
l349[4],&l344);}else{l349[4] = 0;};l95((l78[(l80)]),l346,l349,l93,l96,((
uint16)l348<<1)|(0x8000));}}else{if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l105),(l67[(l80)]->l39))){l349[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l105),(l67[
(l80)]->l25));l349[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l105),(l67[(l80)]->l27));l349[2] = 0;l349[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int l344;(void)
soc_th_alpm_lpm_vrf_get(l80,l105,(int*)&l349[4],&l344);}else{l349[4] = 0;};
l95((l78[(l80)]),l346,l349,l93,l96,((uint16)l348<<1));}if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l105),(l67[
(l80)]->l40))){l349[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80
,L3_DEFIPm)),(l105),(l67[(l80)]->l26));l349[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l105),(l67[
(l80)]->l28));l349[2] = 0;l349[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l80)))
&&(((l67[(l80)]->l42)!= NULL))){int l344;defip_entry_t l345;(void)
soc_th_alpm_lpm_ip4entry1_to_0(l80,l105,&l345,0);(void)
soc_th_alpm_lpm_vrf_get(l80,&l345,(int*)&l349[4],&l344);}else{l349[4] = 0;};
l95((l78[(l80)]),l346,l349,l93,l96,(((uint16)l348<<1)+1));}}}static void l350
(int l80,void*key_data,uint32 l348){l74 l349;int l93 = -1;int l139;uint16
index;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
key_data),(l67[(l80)]->l29))){l349[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l25));l349[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l27));l349[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l26));l349[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l28));if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))
){int l344;(void)soc_th_alpm_lpm_vrf_get(l80,key_data,(int*)&l349[4],&l344);}
else{l349[4] = 0;};index = (l348<<1)|(0x8000);}else{l349[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l25));l349[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l27));l349[2] = 0;l349[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int
l344;(void)soc_th_alpm_lpm_vrf_get(l80,key_data,(int*)&l349[4],&l344);}else{
l349[4] = 0;};index = l348;}l139 = l98((l78[(l80)]),l346,l349,l93,index);if(
SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\ndel  index: H %d error %d\n"),index,l139));}}static int l351(int l80,void*
key_data,int l93,int*l94){l74 l349;int l352;int l139;uint16 index = (0xFFFF);
l352 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
key_data),(l67[(l80)]->l29));if(l352){l349[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l25));l349[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l27));l349[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l26));l349[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l28));if((!(SOC_IS_HURRICANE(l80)))&&
(((l67[(l80)]->l41)!= NULL))){int l344;(void)soc_th_alpm_lpm_vrf_get(l80,
key_data,(int*)&l349[4],&l344);}else{l349[4] = 0;};}else{l349[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l25));l349[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l27));l349[2] = 0;l349[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int
l344;(void)soc_th_alpm_lpm_vrf_get(l80,key_data,(int*)&l349[4],&l344);}else{
l349[4] = 0;};}l139 = l90((l78[(l80)]),l346,l349,l93,&index);if(SOC_FAILURE(
l139)){*l94 = 0xFFFFFFFF;return(l139);}*l94 = index;return(SOC_E_NONE);}
static int l353(int l80,void*key_data,uint32 l348){l74 l349;int l93 = -1;int
l139;uint16 index;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(key_data),(l67[(l80)]->l29))){l349[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l25));l349[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l27));l349[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l26));l349[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l28));if((!(SOC_IS_HURRICANE(l80)))&&
(((l67[(l80)]->l41)!= NULL))){int l344;(void)soc_th_alpm_lpm_vrf_get(l80,
key_data,(int*)&l349[4],&l344);}else{l349[4] = 0;};index = (l348<<1)|(0x8000)
;}else{l349[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(key_data),(l67[(l80)]->l25));l349[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l27));l349[2] = 0;l349[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l80
)))&&(((l67[(l80)]->l41)!= NULL))){int l344;(void)soc_th_alpm_lpm_vrf_get(l80
,key_data,(int*)&l349[4],&l344);}else{l349[4] = 0;};index = l348;}l139 = l100
((l78[(l80)]),l346,l349,l93,index);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"\nveri index: H %d error %d\n"),index,l139))
;}return l139;}static uint16 l83(uint8*l84,int l85){return(_shr_crc16b(0,l84,
l85));}static int l86(int unit,int l69,int l70,l73**l87){l73*l91;int index;if
(l70>l69){return SOC_E_MEMORY;}l91 = sal_alloc(sizeof(l73),"lpm_hash");if(l91
== NULL){return SOC_E_MEMORY;}sal_memset(l91,0,sizeof(*l91));l91->unit = unit
;l91->l69 = l69;l91->l70 = l70;l91->l71 = sal_alloc(l91->l70*sizeof(*(l91->
l71)),"hash_table");if(l91->l71 == NULL){sal_free(l91);return SOC_E_MEMORY;}
l91->l72 = sal_alloc(l91->l69*sizeof(*(l91->l72)),"link_table");if(l91->l72 == 
NULL){sal_free(l91->l71);sal_free(l91);return SOC_E_MEMORY;}for(index = 0;
index<l91->l70;index++){l91->l71[index] = (0xFFFF);}for(index = 0;index<l91->
l69;index++){l91->l72[index] = (0xFFFF);}*l87 = l91;return SOC_E_NONE;}static
int l88(l73*l89){if(l89!= NULL){sal_free(l89->l71);sal_free(l89->l72);
sal_free(l89);}return SOC_E_NONE;}static int l90(l73*l91,l75 l92,l74 entry,
int l93,uint16*l94){int l80 = l91->unit;uint16 l354;uint16 index;l354 = l83((
uint8*)entry,(32*5))%l91->l70;index = l91->l71[l354];;;while(index!= (0xFFFF)
){uint32 l81[SOC_MAX_MEM_FIELD_WORDS];l74 l82;int l355;l355 = (index&(0x7FFF)
)>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l355,l81));l79(l80,
l81,index,l82);if((*l92)(entry,l82) == 0){*l94 = (index&(0x7FFF))>>((index&(
0x8000))?1:0);;return(SOC_E_NONE);}index = l91->l72[index&(0x7FFF)];;};return
(SOC_E_NOT_FOUND);}static int l95(l73*l91,l75 l92,l74 entry,int l93,uint16 l96
,uint16 l97){int l80 = l91->unit;uint16 l354;uint16 index;uint16 l356;l354 = 
l83((uint8*)entry,(32*5))%l91->l70;index = l91->l71[l354];;;;l356 = (0xFFFF);
if(l96!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l81[SOC_MAX_MEM_FIELD_WORDS
];l74 l82;int l355;l355 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(
READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l355,l81));l79(l80,l81,index,l82);if((*l92)(
entry,l82) == 0){if(l97!= index){;if(l356 == (0xFFFF)){l91->l71[l354] = l97;
l91->l72[l97&(0x7FFF)] = l91->l72[index&(0x7FFF)];l91->l72[index&(0x7FFF)] = 
(0xFFFF);}else{l91->l72[l356&(0x7FFF)] = l97;l91->l72[l97&(0x7FFF)] = l91->
l72[index&(0x7FFF)];l91->l72[index&(0x7FFF)] = (0xFFFF);}};return(SOC_E_NONE)
;}l356 = index;index = l91->l72[index&(0x7FFF)];;}}l91->l72[l97&(0x7FFF)] = 
l91->l71[l354];l91->l71[l354] = l97;return(SOC_E_NONE);}static int l98(l73*
l91,l75 l92,l74 entry,int l93,uint16 l99){uint16 l354;uint16 index;uint16 l356
;l354 = l83((uint8*)entry,(32*5))%l91->l70;index = l91->l71[l354];;;l356 = (
0xFFFF);while(index!= (0xFFFF)){if(l99 == index){;if(l356 == (0xFFFF)){l91->
l71[l354] = l91->l72[l99&(0x7FFF)];l91->l72[l99&(0x7FFF)] = (0xFFFF);}else{
l91->l72[l356&(0x7FFF)] = l91->l72[l99&(0x7FFF)];l91->l72[l99&(0x7FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l356 = index;index = l91->l72[index&(0x7FFF)];;}
return(SOC_E_NOT_FOUND);}static int l100(l73*l91,l75 l92,l74 entry,int l93,
uint16 l357){uint16 l354;uint16 index;l354 = l83((uint8*)entry,(32*5))%l91->
l70;index = l91->l71[l354];;;while(index!= (0xFFFF)){if(l357 == index){;
return(SOC_E_NONE);}index = l91->l72[index&(0x7FFF)];;}return(SOC_E_NOT_FOUND
);}int soc_th_alpm_lpm_ip4entry0_to_0(int l80,void*l358,void*l359,int l360){
uint32 l140;l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l358),(l67[(l80)]->l39));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l39),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l29));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l29),(l140));if(((l67[(l80)]->l31)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l31));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l31),(l140));}if(((l67[(l80)]->l21)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l21));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l21),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l25));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l25),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l27));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l27),(l140));if(((l67[(l80)
]->l15)!= NULL)){l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l358),(l67[(l80)]->l15));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l15),(l140));}if(((l67[(l80)
]->l17)!= NULL)){l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l358),(l67[(l80)]->l17));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l17),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l19));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l19),(l140));}else{l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l33));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l33),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l35));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l35),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l37));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l37),(l140));if(((l67[(l80)
]->l41)!= NULL)){l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l358),(l67[(l80)]->l41));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l41),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l43));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l43),(l140));}if(((l67[(l80)]->l13)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l13));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l13),(l140));}if(((l67[(l80)]->l11)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l11));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l11),(l140));}if(l360){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l23));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l23),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l45));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l45),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l47));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l47),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l49),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l51));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l51),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l53));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l53),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l55));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l55),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l57));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l57),(l140));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry1_to_1(
int l80,void*l358,void*l359,int l360){uint32 l140;l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l40));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l40),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l30));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l30),(l140));if(((l67[(l80)]->l32)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l32));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l32),(l140));}if(((l67[(l80)]->l22)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l22));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l22),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l26));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l26),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l28));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l28),(l140));if(((l67[(l80)
]->l16)!= NULL)){l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l358),(l67[(l80)]->l16));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l16),(l140));}if(((l67[(l80)
]->l18)!= NULL)){l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l358),(l67[(l80)]->l18));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l18),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l20));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l20),(l140));}else{l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l34));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l34),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l36));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l36),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l38));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l38),(l140));if(((l67[(l80)
]->l42)!= NULL)){l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l358),(l67[(l80)]->l42));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l42),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l44));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l44),(l140));}if(((l67[(l80)]->l14)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l14));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l14),(l140));}if(((l67[(l80)]->l12)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l12));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l12),(l140));}if(l360){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l24));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l24),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l46));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l46),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l48));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l48),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l50),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l52));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l52),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l54));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l54),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l56),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l58));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l58),(l140));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry0_to_1(
int l80,void*l358,void*l359,int l360){uint32 l140;l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l39));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l40),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l29));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l30),(l140));if(((l67[(l80)]->l31)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l31));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l32),(l140));}if(((l67[(l80)]->l21)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l21));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l22),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l25));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l26),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l27));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l28),(l140));if(!
SOC_IS_HURRICANE(l80)){l140 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l15));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l16),(l140));}if(((l67[(l80)]->l17)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l17));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l18),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l19));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l20),(l140));}else{l140 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l33));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l34),(l140));}l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l35));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l36),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l37));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l38),(l140));if(((l67[(l80)]->l41)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l41));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l42),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l43));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l44),(l140));}if(((l67[(l80)]->l13)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l13));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l14),(l140));}if(((l67[(l80)]->l11)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l11));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l12),(l140));}if(l360){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l23));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l24),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l45));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l46),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l47));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l48),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l50),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l51));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l52),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l53));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l54),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l55));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l56),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l57));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l58),(l140));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry1_to_0(
int l80,void*l358,void*l359,int l360){uint32 l140;l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l40));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l39),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l30));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l29),(l140));if(((l67[(l80)]->l31)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l32));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l31),(l140));}if(((l67[(l80)]->l22)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l22));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l21),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l26));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l25),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l28));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l27),(l140));if(!
SOC_IS_HURRICANE(l80)){l140 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l16));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l15),(l140));}if(((l67[(l80)]->l18)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l18));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l17),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l20));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l19),(l140));}else{l140 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l34));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l33),(l140));}l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l36));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l35),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l38));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l37),(l140));if(((l67[(l80)]->l42)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l42));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l41),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l44));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l43),(l140));}if(((l67[(l80)]->l14)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l14));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l13),(l140));}if(((l67[(l80)]->l12)!= NULL)){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l12));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l11),(l140));}if(l360){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l24));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l23),(l140));}l140 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l46));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l45),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l48));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l47),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l49),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l52));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l51),(l140));l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l358),(l67[(l80)]->l54));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[(l80)]->l53),(l140));l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[
(l80)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l359),(l67[(l80)]->l55),(l140));l140 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l358),(l67[(l80)]->l58));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l359),(l67[
(l80)]->l57),(l140));return(SOC_E_NONE);}static int l361(int l80,void*l81){
return(SOC_E_NONE);}void soc_th_alpm_lpm_state_dump(int l80){int l163;int l362
;l362 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l80)){return;}for(l163 = l362;l163>= 0;l163--){if((l163!= 
((3*(64+32+2+1))-1))&&((l9[(l80)][(l163)].l2) == -1)){continue;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l163,(l9[
(l80)][(l163)].l4),(l9[(l80)][(l163)].next),(l9[(l80)][(l163)].l2),(l9[(l80)]
[(l163)].l3),(l9[(l80)][(l163)].l5),(l9[(l80)][(l163)].l6)));}
COMPILER_REFERENCE(l361);}static int l363(int l80,int index,uint32*l81){int
l364;int l107;uint32 l365;uint32 l366;int l367;if(!SOC_URPF_STATUS_GET(l80)){
return(SOC_E_NONE);}if(soc_feature(l80,soc_feature_l3_defip_hole)){l364 = (
soc_mem_index_count(l80,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l80)){l364 = (
soc_mem_index_count(l80,L3_DEFIPm)>>1)+0x0400;}else{l364 = (
soc_mem_index_count(l80,L3_DEFIPm)>>1);}l107 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l29));if(((l67[(l80)]->l13)!= NULL)){soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l13),(0));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l14),(0));}l365 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80
,L3_DEFIPm)),(l81),(l67[(l80)]->l27));l366 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l28));if(!l107){if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l81),(l67[(l80)]->l39))){soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l37),((!l365)?1:0));}if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l40))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l81),(l67[(l80)]->l38),((!l366)?1:0));}}else{l367 = ((!l365)&&(!l366))?1:0
;l365 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81
),(l67[(l80)]->l39));l366 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l81),(l67[(l80)]->l40));if(l365&&l366){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l37),(l367));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l38),(l367));}}return l235(l80,MEM_BLOCK_ANY,
index+l364,index,l81);}static int l368(int l80,int l369,int l370){uint32 l81[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l80,MEM_BLOCK_ANY
,l369,l81));l347(l80,l81,l370,0x4000,0);SOC_IF_ERROR_RETURN(l235(l80,
MEM_BLOCK_ANY,l370,l369,l81));SOC_IF_ERROR_RETURN(l363(l80,l370,l81));do{int
l371 = soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l369),1);int l372 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l370),1);ALPM_TCAM_PIVOT(l80,l372<<
1) = ALPM_TCAM_PIVOT(l80,l371<<1);ALPM_TCAM_PIVOT(l80,(l372<<1)+1) = 
ALPM_TCAM_PIVOT(l80,(l371<<1)+1);if(ALPM_TCAM_PIVOT((l80),l372<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),l372<<1)) = l372<<1;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),(
l372<<1))),(l372<<1));}if(ALPM_TCAM_PIVOT((l80),(l372<<1)+1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),(l372<<1)+1)) = (l372<<1)+1;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),(
l372<<1)+1)),(l372<<1)+1);}ALPM_TCAM_PIVOT(l80,l371<<1) = NULL;
ALPM_TCAM_PIVOT(l80,(l371<<1)+1) = NULL;}while(0);return(SOC_E_NONE);}static
int l373(int l80,int l93,int l107){uint32 l81[SOC_MAX_MEM_FIELD_WORDS];int
l369;int l370;uint32 l374,l375;l370 = (l9[(l80)][(l93)].l3)+1;if(!l107){l369 = 
(l9[(l80)][(l93)].l3);SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,
l369,l81));l374 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l39));l375 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l40));if((l374 == 0)||(
l375 == 0)){l347(l80,l81,l370,0x4000,0);SOC_IF_ERROR_RETURN(l235(l80,
MEM_BLOCK_ANY,l370,l369,l81));SOC_IF_ERROR_RETURN(l363(l80,l370,l81));do{int
l371 = soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l369),1)*2;int l372 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l370),1)*2;if((l375)){l371++;}
ALPM_TCAM_PIVOT((l80),l372) = ALPM_TCAM_PIVOT((l80),l371);if(ALPM_TCAM_PIVOT(
(l80),l372)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),l372)) = l372;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),l372
)),l372);}ALPM_TCAM_PIVOT((l80),l371) = NULL;}while(0);l370--;}}l369 = (l9[(
l80)][(l93)].l2);if(l369!= l370){SOC_IF_ERROR_RETURN(l368(l80,l369,l370));
VRF_PIVOT_SHIFT_INC(l80,MAX_VRF_ID,l107);}(l9[(l80)][(l93)].l2)+= 1;(l9[(l80)
][(l93)].l3)+= 1;return(SOC_E_NONE);}static int l376(int l80,int l93,int l107
){uint32 l81[SOC_MAX_MEM_FIELD_WORDS];int l369;int l370;int l377;uint32 l374,
l375;l370 = (l9[(l80)][(l93)].l2)-1;if((l9[(l80)][(l93)].l5) == 0){(l9[(l80)]
[(l93)].l2) = l370;(l9[(l80)][(l93)].l3) = l370-1;return(SOC_E_NONE);}if((!
l107)&&((l9[(l80)][(l93)].l3)!= (l9[(l80)][(l93)].l2))){l369 = (l9[(l80)][(
l93)].l3);SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l369,l81));
l374 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81)
,(l67[(l80)]->l39));l375 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l81),(l67[(l80)]->l40));if((l374 == 0)||(l375 == 0)){l377 = 
l369-1;SOC_IF_ERROR_RETURN(l368(l80,l377,l370));VRF_PIVOT_SHIFT_INC(l80,
MAX_VRF_ID,l107);l347(l80,l81,l377,0x4000,0);SOC_IF_ERROR_RETURN(l235(l80,
MEM_BLOCK_ANY,l377,l369,l81));SOC_IF_ERROR_RETURN(l363(l80,l377,l81));do{int
l371 = soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l369),1)*2;int l372 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l377),1)*2;if((l375)){l371++;}
ALPM_TCAM_PIVOT((l80),l372) = ALPM_TCAM_PIVOT((l80),l371);if(ALPM_TCAM_PIVOT(
(l80),l372)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),l372)) = l372;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),l372
)),l372);}ALPM_TCAM_PIVOT((l80),l371) = NULL;}while(0);}else{l347(l80,l81,
l370,0x4000,0);SOC_IF_ERROR_RETURN(l235(l80,MEM_BLOCK_ANY,l370,l369,l81));
SOC_IF_ERROR_RETURN(l363(l80,l370,l81));do{int l371 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l369),1);int l372 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l370),1);ALPM_TCAM_PIVOT(l80,l372<<
1) = ALPM_TCAM_PIVOT(l80,l371<<1);ALPM_TCAM_PIVOT(l80,(l372<<1)+1) = 
ALPM_TCAM_PIVOT(l80,(l371<<1)+1);if(ALPM_TCAM_PIVOT((l80),l372<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),l372<<1)) = l372<<1;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),(
l372<<1))),(l372<<1));}if(ALPM_TCAM_PIVOT((l80),(l372<<1)+1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),(l372<<1)+1)) = (l372<<1)+1;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),(
l372<<1)+1)),(l372<<1)+1);}ALPM_TCAM_PIVOT(l80,l371<<1) = NULL;
ALPM_TCAM_PIVOT(l80,(l371<<1)+1) = NULL;}while(0);}}else{l369 = (l9[(l80)][(
l93)].l3);SOC_IF_ERROR_RETURN(l368(l80,l369,l370));VRF_PIVOT_SHIFT_INC(l80,
MAX_VRF_ID,l107);}(l9[(l80)][(l93)].l2)-= 1;(l9[(l80)][(l93)].l3)-= 1;return(
SOC_E_NONE);}static int l378(int l80,int l93,int l107,void*l81,int*l379){int
l380;int l381;int l382;int l383;int l369;uint32 l374,l375;int l139;if((l9[(
l80)][(l93)].l5) == 0){l383 = ((3*(64+32+2+1))-1);if((soc_th_alpm_mode_get(
l80) == SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_TCAM_ALPM)){if(l93<= (((3*(64+32+2+1))/3)-1)){l383 = (((3*(64+
32+2+1))/3)-1);}}while((l9[(l80)][(l383)].next)>l93){l383 = (l9[(l80)][(l383)
].next);}l381 = (l9[(l80)][(l383)].next);if(l381!= -1){(l9[(l80)][(l381)].l4)
= l93;}(l9[(l80)][(l93)].next) = (l9[(l80)][(l383)].next);(l9[(l80)][(l93)].
l4) = l383;(l9[(l80)][(l383)].next) = l93;(l9[(l80)][(l93)].l6) = ((l9[(l80)]
[(l383)].l6)+1)/2;(l9[(l80)][(l383)].l6)-= (l9[(l80)][(l93)].l6);(l9[(l80)][(
l93)].l2) = (l9[(l80)][(l383)].l3)+(l9[(l80)][(l383)].l6)+1;(l9[(l80)][(l93)]
.l3) = (l9[(l80)][(l93)].l2)-1;(l9[(l80)][(l93)].l5) = 0;}else if(!l107){l369
= (l9[(l80)][(l93)].l2);if((l139 = READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l369,l81)
)<0){return l139;}l374 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80
,L3_DEFIPm)),(l81),(l67[(l80)]->l39));l375 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l40));if((l374 == 0)||(l375 == 0)){*l379 = (l369<<1)+((l375 == 0)?1:0)
;return(SOC_E_NONE);}l369 = (l9[(l80)][(l93)].l3);if((l139 = READ_L3_DEFIPm(
l80,MEM_BLOCK_ANY,l369,l81))<0){return l139;}l374 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l39));l375 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l40));if((l374 == 0)||(l375 == 0)){*l379 = (
l369<<1)+((l375 == 0)?1:0);return(SOC_E_NONE);}}l382 = l93;while((l9[(l80)][(
l382)].l6) == 0){l382 = (l9[(l80)][(l382)].next);if(l382 == -1){l382 = l93;
break;}}while((l9[(l80)][(l382)].l6) == 0){l382 = (l9[(l80)][(l382)].l4);if(
l382 == -1){if((l9[(l80)][(l93)].l5) == 0){l380 = (l9[(l80)][(l93)].l4);l381 = 
(l9[(l80)][(l93)].next);if(-1!= l380){(l9[(l80)][(l380)].next) = l381;}if(-1
!= l381){(l9[(l80)][(l381)].l4) = l380;}}return(SOC_E_FULL);}}while(l382>l93)
{l381 = (l9[(l80)][(l382)].next);SOC_IF_ERROR_RETURN(l376(l80,l381,l107));(l9
[(l80)][(l382)].l6)-= 1;(l9[(l80)][(l381)].l6)+= 1;l382 = l381;}while(l382<
l93){SOC_IF_ERROR_RETURN(l373(l80,l382,l107));(l9[(l80)][(l382)].l6)-= 1;l380
= (l9[(l80)][(l382)].l4);(l9[(l80)][(l380)].l6)+= 1;l382 = l380;}(l9[(l80)][(
l93)].l5)+= 1;(l9[(l80)][(l93)].l6)-= 1;(l9[(l80)][(l93)].l3)+= 1;*l379 = (l9
[(l80)][(l93)].l3)<<((l107)?0:1);sal_memcpy(l81,soc_mem_entry_null(l80,
L3_DEFIPm),soc_mem_entry_words(l80,L3_DEFIPm)*4);return(SOC_E_NONE);}static
int l384(int l80,int l93,int l107,void*l81,int l385){int l380;int l381;int
l369;int l370;uint32 l386[SOC_MAX_MEM_FIELD_WORDS];uint32 l387[
SOC_MAX_MEM_FIELD_WORDS];uint32 l388[SOC_MAX_MEM_FIELD_WORDS];void*l389;int
l139;int l390,l226;l369 = (l9[(l80)][(l93)].l3);l370 = l385;if(!l107){l370>>= 
1;if((l139 = READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l369,l386))<0){return l139;}if(
(l139 = READ_L3_DEFIP_AUX_TABLEm(l80,MEM_BLOCK_ANY,soc_th_alpm_physical_idx(
l80,L3_DEFIPm,l369,1),l387))<0){return l139;}if((l139 = 
READ_L3_DEFIP_AUX_TABLEm(l80,MEM_BLOCK_ANY,soc_th_alpm_physical_idx(l80,
L3_DEFIPm,l370,1),l388))<0){return l139;}l389 = (l370 == l369)?l386:l81;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l386),(l67[
(l80)]->l40))){l226 = soc_mem_field32_get(l80,L3_DEFIP_AUX_TABLEm,l387,
BPM_LENGTH1f);if(l385&1){l139 = soc_th_alpm_lpm_ip4entry1_to_1(l80,l386,l389,
PRESERVE_HIT);soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,l388,BPM_LENGTH1f,
l226);}else{l139 = soc_th_alpm_lpm_ip4entry1_to_0(l80,l386,l389,PRESERVE_HIT)
;soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,l388,BPM_LENGTH0f,l226);}l390 = 
(l369<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),
(l386),(l67[(l80)]->l40),(0));}else{l226 = soc_mem_field32_get(l80,
L3_DEFIP_AUX_TABLEm,l387,BPM_LENGTH0f);if(l385&1){l139 = 
soc_th_alpm_lpm_ip4entry0_to_1(l80,l386,l389,PRESERVE_HIT);
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,l388,BPM_LENGTH1f,l226);}else{
l139 = soc_th_alpm_lpm_ip4entry0_to_0(l80,l386,l389,PRESERVE_HIT);
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,l388,BPM_LENGTH0f,l226);}l390 = 
l369<<1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
l386),(l67[(l80)]->l39),(0));(l9[(l80)][(l93)].l5)-= 1;(l9[(l80)][(l93)].l6)
+= 1;(l9[(l80)][(l93)].l3)-= 1;}l390 = soc_th_alpm_physical_idx(l80,L3_DEFIPm
,l390,0);l385 = soc_th_alpm_physical_idx(l80,L3_DEFIPm,l385,0);
ALPM_TCAM_PIVOT(l80,l385) = ALPM_TCAM_PIVOT(l80,l390);if(ALPM_TCAM_PIVOT(l80,
l385)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l80,l385)) = l385;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l80,l385))
,l385);}ALPM_TCAM_PIVOT(l80,l390) = NULL;if((l139 = WRITE_L3_DEFIP_AUX_TABLEm
(l80,MEM_BLOCK_ALL,soc_th_alpm_physical_idx(l80,L3_DEFIPm,l370,1),l388))<0){
return l139;}if(l370!= l369){l347(l80,l389,l370,0x4000,0);if((l139 = l235(l80
,MEM_BLOCK_ANY,l370,l370,l389))<0){return l139;}if((l139 = l363(l80,l370,l389
))<0){return l139;}}l347(l80,l386,l369,0x4000,0);if((l139 = l235(l80,
MEM_BLOCK_ANY,l369,l369,l386))<0){return l139;}if((l139 = l363(l80,l369,l386)
)<0){return l139;}}else{(l9[(l80)][(l93)].l5)-= 1;(l9[(l80)][(l93)].l6)+= 1;(
l9[(l80)][(l93)].l3)-= 1;if(l370!= l369){if((l139 = READ_L3_DEFIPm(l80,
MEM_BLOCK_ANY,l369,l386))<0){return l139;}l347(l80,l386,l370,0x4000,0);if((
l139 = l235(l80,MEM_BLOCK_ANY,l370,l369,l386))<0){return l139;}if((l139 = 
l363(l80,l370,l386))<0){return l139;}}l385 = soc_th_alpm_physical_idx(l80,
L3_DEFIPm,l370,1);l390 = soc_th_alpm_physical_idx(l80,L3_DEFIPm,l369,1);
ALPM_TCAM_PIVOT(l80,l385<<1) = ALPM_TCAM_PIVOT(l80,l390<<1);ALPM_TCAM_PIVOT(
l80,l390<<1) = NULL;if(ALPM_TCAM_PIVOT(l80,l385<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l80,l385<<1)) = l385<<1;soc_th_alpm_lpm_move_bu_upd(l80,
PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l80,l385<<1)),l385<<1);}sal_memcpy(l386,
soc_mem_entry_null(l80,L3_DEFIPm),soc_mem_entry_words(l80,L3_DEFIPm)*4);l347(
l80,l386,l369,0x4000,0);if((l139 = l235(l80,MEM_BLOCK_ANY,l369,l369,l386))<0)
{return l139;}if((l139 = l363(l80,l369,l386))<0){return l139;}}if((l9[(l80)][
(l93)].l5) == 0){l380 = (l9[(l80)][(l93)].l4);assert(l380!= -1);l381 = (l9[(
l80)][(l93)].next);(l9[(l80)][(l380)].next) = l381;(l9[(l80)][(l380)].l6)+= (
l9[(l80)][(l93)].l6);(l9[(l80)][(l93)].l6) = 0;if(l381!= -1){(l9[(l80)][(l381
)].l4) = l380;}(l9[(l80)][(l93)].next) = -1;(l9[(l80)][(l93)].l4) = -1;(l9[(
l80)][(l93)].l2) = -1;(l9[(l80)][(l93)].l3) = -1;}return(l139);}int
soc_th_alpm_lpm_vrf_get(int unit,void*lpm_entry,int*vrf,int*l132){int vrf_id;
if(((l67[(unit)]->l43)!= NULL)){vrf_id = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l41));*l132 = vrf_id;
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l43))){*vrf = vrf_id;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l45))){*vrf = SOC_L3_VRF_GLOBAL;*l132 = SOC_VRF_MAX(unit)+1;}
else{*vrf = SOC_L3_VRF_OVERRIDE;}}else{*vrf = SOC_L3_VRF_DEFAULT;}return(
SOC_E_NONE);}static int l101(int l80,int l107,void*entry,int*l114,int*vrf_id,
int*vrf){int l93;int l139;uint32 l140;int l391;int l137;if(l107){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l27));if((l139 = _ipmask2pfx(l140,&l93))<0){return(l139);}l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l28));if(l93){if(l140!= 0xffffffff){return(SOC_E_PARAM);}l93+= 32;}
else{if((l139 = _ipmask2pfx(l140,&l93))<0){return(l139);}}l93+= 33;}else{l140
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(
l67[(l80)]->l27));if((l139 = _ipmask2pfx(l140,&l93))<0){return(l139);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,entry,&l391,&l139));if(vrf_id
!= NULL){*vrf_id = l391;}if(vrf!= NULL){*vrf = l139;}l137 = 
soc_th_alpm_mode_get(l80);switch(l391){case SOC_L3_VRF_GLOBAL:if((l137 == 
SOC_ALPM_MODE_PARALLEL)||(l137 == SOC_ALPM_MODE_TCAM_ALPM)){*l114 = l93+((3*(
64+32+2+1))/3);}else{*l114 = l93;}break;case SOC_L3_VRF_OVERRIDE:*l114 = l93+
2*((3*(64+32+2+1))/3);break;default:if((l137 == SOC_ALPM_MODE_PARALLEL)||(
l137 == SOC_ALPM_MODE_TCAM_ALPM)){*l114 = l93;}else{*l114 = l93+((3*(64+32+2+
1))/3);}break;}return(SOC_E_NONE);}static int l112(int l80,void*key_data,void
*l81,int*l113,int*l114,int*l107,int*vrf_id,int*vrf){int l139;int l130;int l94
;int l93 = 0;l130 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(key_data),(l67[(l80)]->l29));if(l130){if(!(l130 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l30)))){return(SOC_E_PARAM);}}*l107 = l130;l101(l80,l130,key_data
,&l93,vrf_id,vrf);*l114 = l93;if(l351(l80,key_data,l93,&l94) == SOC_E_NONE){*
l113 = l94;if((l139 = READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,(*l107)?*l113:(*l113>>
1),l81))<0){return l139;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}
static int l102(int l80){int l362;int l163;int l341;int l392;uint32 l393,l137
;if(!soc_feature(l80,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}if((l137 = 
soc_property_get(l80,spn_L3_ALPM_ENABLE,0))){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l80,&l393));soc_reg_field_set(l80,
L3_DEFIP_RPF_CONTROLr,&l393,LPM_MODEf,1);if(l137 == 1){soc_reg_field_set(l80,
L3_DEFIP_RPF_CONTROLr,&l393,LOOKUP_MODEf,1);}else if(l137 == 2){
soc_reg_field_set(l80,L3_DEFIP_RPF_CONTROLr,&l393,LOOKUP_MODEf,0);
l3_alpm_ipv4_double_wide[l80] = soc_property_get(l80,
"l3_alpm_ipv4_double_wide",1);if(l3_alpm_ipv4_double_wide[l80]){
soc_reg_field_set(l80,L3_DEFIP_RPF_CONTROLr,&l393,
ENABLE_DOUBLE_WIDE_BUCKETS_FOR_IPV4f,1);}}else if(l137 == 3){
l3_alpm_ipv4_double_wide[l80] = 1;soc_reg_field_set(l80,L3_DEFIP_RPF_CONTROLr
,&l393,LOOKUP_MODEf,2);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l80,
l393));l3_alpm_sw_prefix_lookup[l80] = soc_property_get(l80,
"l3_alpm_sw_prefix_lookup",1);l393 = 0;if(SOC_URPF_STATUS_GET(l80)){
soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l393,URPF_LOOKUP_CAM4f,0x1);
soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l393,URPF_LOOKUP_CAM5f,0x1);
soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l393,URPF_LOOKUP_CAM6f,0x1);
soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l393,URPF_LOOKUP_CAM7f,0x1);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l80,l393));l393 = 0;if(l137 == 1
||l137 == 3){if(SOC_URPF_STATUS_GET(l80)){soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM2_SELf,1);soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM3_SELf,1);soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM4_SELf,2);soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM5_SELf,2);soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM6_SELf,3);soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM7_SELf,3);}else{soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM4_SELf,1);soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM5_SELf,1);soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM6_SELf,1);soc_reg_field_set(l80,
L3_DEFIP_ALPM_CFGr,&l393,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l80)){
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l393,TCAM4_SELf,2);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l393,TCAM5_SELf,2);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l393,TCAM6_SELf,2);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l393,TCAM7_SELf,2);}}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_ALPM_CFGr(l80,l393));if(soc_property_get(
l80,spn_IPV6_LPM_128B_ENABLE,1)){uint32 l394 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l80,&l394));soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&
l394,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l394,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l394,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l394,
V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l80,l394)
);}}l362 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l80);l392 = sizeof(l7)*(l362);if
((l9[(l80)]!= NULL)){if(soc_th_alpm_deinit(l80)<0){SOC_ALPM_LPM_UNLOCK(l80);
return SOC_E_INTERNAL;}}l67[l80] = sal_alloc(sizeof(l65),"lpm_field_state");
if(NULL == l67[l80]){SOC_ALPM_LPM_UNLOCK(l80);return(SOC_E_MEMORY);}(l67[l80]
)->l11 = soc_mem_fieldinfo_get(l80,L3_DEFIPm,CLASS_ID0f);(l67[l80])->l12 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,CLASS_ID1f);(l67[l80])->l13 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,DST_DISCARD0f);(l67[l80])->l14 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,DST_DISCARD1f);(l67[l80])->l15 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ECMP0f);(l67[l80])->l16 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ECMP1f);(l67[l80])->l17 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ECMP_COUNT0f);(l67[l80])->l18 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ECMP_COUNT1f);(l67[l80])->l19 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ECMP_PTR0f);(l67[l80])->l20 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ECMP_PTR1f);(l67[l80])->l21 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,GLOBAL_ROUTE0f);(l67[l80])->l22 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,GLOBAL_ROUTE1f);(l67[l80])->l23 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,HIT0f);(l67[l80])->l24 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,HIT1f);(l67[l80])->l25 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,IP_ADDR0f);(l67[l80])->l26 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,IP_ADDR1f);(l67[l80])->l27 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,IP_ADDR_MASK0f);(l67[l80])->l28 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,IP_ADDR_MASK1f);(l67[l80])->l29 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,MODE0f);(l67[l80])->l30 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,MODE1f);(l67[l80])->l31 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,MODE_MASK0f);(l67[l80])->l32 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,MODE_MASK1f);(l67[l80])->l33 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,NEXT_HOP_INDEX0f);(l67[l80])->l34 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,NEXT_HOP_INDEX1f);(l67[l80])->l35 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,PRI0f);(l67[l80])->l36 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,PRI1f);(l67[l80])->l37 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,RPE0f);(l67[l80])->l38 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,RPE1f);(l67[l80])->l39 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,VALID0f);(l67[l80])->l40 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,VALID1f);(l67[l80])->l41 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,VRF_ID_0f);(l67[l80])->l42 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,VRF_ID_1f);(l67[l80])->l43 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,VRF_ID_MASK0f);(l67[l80])->l44 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,VRF_ID_MASK1f);(l67[l80])->l45 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,GLOBAL_HIGH0f);(l67[l80])->l46 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,GLOBAL_HIGH1f);(l67[l80])->l47 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ALG_HIT_IDX0f);(l67[l80])->l48 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ALG_HIT_IDX1f);(l67[l80])->l49 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ALG_BKT_PTR0f);(l67[l80])->l50 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ALG_BKT_PTR1f);(l67[l80])->l51 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ALG_SUB_BKT_PTR0f);(l67[l80])->l52 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ALG_SUB_BKT_PTR1f);(l67[l80])->l53 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,DEFAULT_MISS0f);(l67[l80])->l54 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,DEFAULT_MISS1f);(l67[l80])->l55 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l67[l80])->
l56 = soc_mem_fieldinfo_get(l80,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l67[
l80])->l57 = soc_mem_fieldinfo_get(l80,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l67
[l80])->l58 = soc_mem_fieldinfo_get(l80,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(
l67[l80])->l59 = soc_mem_fieldinfo_get(l80,L3_DEFIPm,SRC_DISCARD0f);(l67[l80]
)->l60 = soc_mem_fieldinfo_get(l80,L3_DEFIPm,SRC_DISCARD1f);(l67[l80])->l61 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ENTRY_TYPE_MASK0f);(l67[l80])->l62 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ENTRY_TYPE_MASK1f);(l67[l80])->l63 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ENTRY_TYPE0f);(l67[l80])->l64 = 
soc_mem_fieldinfo_get(l80,L3_DEFIPm,ENTRY_TYPE1f);(l9[(l80)]) = sal_alloc(
l392,"LPM prefix info");if(NULL == (l9[(l80)])){sal_free(l67[l80]);l67[l80] = 
NULL;SOC_ALPM_LPM_UNLOCK(l80);return(SOC_E_MEMORY);}sal_memset((l9[(l80)]),0,
l392);for(l163 = 0;l163<l362;l163++){(l9[(l80)][(l163)].l2) = -1;(l9[(l80)][(
l163)].l3) = -1;(l9[(l80)][(l163)].l4) = -1;(l9[(l80)][(l163)].next) = -1;(l9
[(l80)][(l163)].l5) = 0;(l9[(l80)][(l163)].l6) = 0;}l341 = 
soc_mem_index_count(l80,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l80)){l341>>= 1;}if
((l137 == 1)||(l137 == 3)){(l9[(l80)][(((3*(64+32+2+1))-1))].l3) = (l341>>1)-
1;(l9[(l80)][(((((3*(64+32+2+1))/3)-1)))].l6) = l341>>1;(l9[(l80)][((((3*(64+
32+2+1))-1)))].l6) = (l341-(l9[(l80)][(((((3*(64+32+2+1))/3)-1)))].l6));}else
{(l9[(l80)][((((3*(64+32+2+1))-1)))].l6) = l341;}if((l78[(l80)])!= NULL){if(
l88((l78[(l80)]))<0){SOC_ALPM_LPM_UNLOCK(l80);return SOC_E_INTERNAL;}(l78[(
l80)]) = NULL;}if(l86(l80,l341*2,l341,&(l78[(l80)]))<0){SOC_ALPM_LPM_UNLOCK(
l80);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l80);return(SOC_E_NONE);}static
int l103(int l80){if(!soc_feature(l80,soc_feature_lpm_tcam)){return(
SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l80);if((l78[(l80)])!= NULL){l88((l78[(l80)
]));(l78[(l80)]) = NULL;}if((l9[(l80)]!= NULL)){sal_free(l67[l80]);l67[l80] = 
NULL;sal_free((l9[(l80)]));(l9[(l80)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l80);
return(SOC_E_NONE);}static int l104(int l80,void*l105,int*l176){int l93;int
index;int l107;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];int l139 = SOC_E_NONE;int
l395 = 0;sal_memcpy(l81,soc_mem_entry_null(l80,L3_DEFIPm),soc_mem_entry_words
(l80,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l80);l139 = l112(l80,l105,l81,&index,&
l93,&l107,0,0);if(l139 == SOC_E_NOT_FOUND){l139 = l378(l80,l93,l107,l81,&
index);if(l139<0){SOC_ALPM_LPM_UNLOCK(l80);return(l139);}}else{l395 = 1;}*
l176 = index;if(l139 == SOC_E_NONE){if(!l107){if(index&1){l139 = 
soc_th_alpm_lpm_ip4entry0_to_1(l80,l105,l81,PRESERVE_HIT);}else{l139 = 
soc_th_alpm_lpm_ip4entry0_to_0(l80,l105,l81,PRESERVE_HIT);}if(l139<0){
SOC_ALPM_LPM_UNLOCK(l80);return(l139);}l105 = (void*)l81;index>>= 1;}
soc_th_alpm_lpm_state_dump(l80);LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_lpm_insert: %d %d\n"),index,l93));if(!l395){l347(l80,l105,index,
0x4000,0);}l139 = l235(l80,MEM_BLOCK_ANY,index,index,l105);if(l139>= 0){l139 = 
l363(l80,index,l105);}}SOC_ALPM_LPM_UNLOCK(l80);return(l139);}static int
soc_th_alpm_lpm_delete(int l80,void*key_data){int l93;int index;int l107;
uint32 l81[SOC_MAX_MEM_FIELD_WORDS];int l139 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(
l80);l139 = l112(l80,key_data,l81,&index,&l93,&l107,0,0);if(l139 == 
SOC_E_NONE){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_lpm_delete: %d %d\n"),index,l93));l350(l80,key_data,index);l139 = 
l384(l80,l93,l107,l81,index);}soc_th_alpm_lpm_state_dump(l80);
SOC_ALPM_LPM_UNLOCK(l80);return(l139);}static int l115(int l80,void*key_data,
void*l81,int*l113){int l93;int l139;int l107;SOC_ALPM_LPM_LOCK(l80);l139 = 
l112(l80,key_data,l81,l113,&l93,&l107,0,0);SOC_ALPM_LPM_UNLOCK(l80);return(
l139);}static int l106(int unit,void*key_data,int l107,int l108,int l109,int
l110,defip_aux_scratch_entry_t*l111){uint32 l140;uint32 l288[4] = {0,0,0,0};
int l93 = 0;int l139 = SOC_E_NONE;l140 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l39));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,VALIDf,l140);l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l29));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,MODEf
,l140);l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm
)),(key_data),(l67[(unit)]->l63));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l111,ENTRY_TYPEf,0);l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l21));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,
GLOBAL_ROUTEf,l140);l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l15));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l111,ECMPf,l140);l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l19));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,
ECMP_PTRf,l140);l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(key_data),(l67[(unit)]->l33));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l111,NEXT_HOP_INDEXf,l140);l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l35));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,PRIf,
l140);l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)
),(key_data),(l67[(unit)]->l37));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l111,RPEf,l140);l140 =soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l41));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,VRFf,l140);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,DB_TYPEf,l108);l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l13));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,
DST_DISCARDf,l140);l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l11));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l111,CLASS_IDf,l140);if(l107){l288[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l25));l288[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l26));}else{l288[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l25));}soc_mem_field_set(unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)
l111,IP_ADDRf,(uint32*)l288);if(l107){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l27));if((l139 = _ipmask2pfx(l140,&l93))<0){return(l139);}l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l28));if(l93){if(l140!= 0xffffffff){return(SOC_E_PARAM);}l93+= 
32;}else{if((l139 = _ipmask2pfx(l140,&l93))<0){return(l139);}}}else{l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l27));if((l139 = _ipmask2pfx(l140,&l93))<0){return(l139);}}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,IP_LENGTHf,l93);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l111,REPLACE_LENf,l110);return
(SOC_E_NONE);}int l396(int l80,int l397,int*index){char*l398 = NULL;soc_mem_t
l120 = L3_DEFIP_AUX_TABLEm;int l399,l400;uint32 l401;int l402 = 
soc_mem_index_count(l80,l120);int l403 = sizeof(uint32)*soc_mem_entry_words(
l80,l120);l399 = soc_mem_index_min(l80,l120);l400 = soc_mem_index_max(l80,
l120);l398 = soc_cm_salloc(l80,l402*l403,"auxtbl_dmabuf");if(l398 == NULL){
return SOC_E_MEMORY;}switch(l397){case 0:l401 = _SOC_MEM_ADDR_ACC_TYPE_PIPE_0
;break;case 1:l401 = _SOC_MEM_ADDR_ACC_TYPE_PIPE_1;break;case 2:l401 = 
_SOC_MEM_ADDR_ACC_TYPE_PIPE_2;break;case 3:l401 = 
_SOC_MEM_ADDR_ACC_TYPE_PIPE_3;break;default:l401 = 
_SOC_MEM_ADDR_ACC_TYPE_PIPE_0;break;}l401|= _SOC_SER_FLAG_DMA_ERR_RETURN;l401
|= _SOC_SER_FLAG_DISCARD_READ;l401|= _SOC_SER_FLAG_MULTI_PIPE;while(l399<= 
l400){int l404 = l399+(l400-l399)/2;if(soc_mem_ser_read_range(l80,l120,
MEM_BLOCK_ALL,l399,l404,l401,l398)<0){if(l400 == l399){*index = l399;break;}
l400 = l404;}else{l399 = l404+1;}}soc_cm_sfree(l80,l398);if(l399>l400){
LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"No corrupted index in AUX Table Pipe %d\n"),l397));return SOC_E_NOT_FOUND;}
else{LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Found corrupted Index %d in AUX Table Pipe %d\n"),*index,l397));return
SOC_E_NONE;}}int _soc_th_alpm_aux_op(int l80,_soc_aux_op_t l405,
defip_aux_scratch_entry_t*l111,int l406,int*l155,int*tcam_index,int*bktid){
soc_timeout_t l407;int l397,l139 = SOC_E_NONE;int l408 = 0;uint32 l393[
_TH_PIPES_PER_DEV];int l409[_TH_PIPES_PER_DEV];int l410[_TH_PIPES_PER_DEV];
uint32 l411;int l412;uint8 l413;if(l406){SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_SCRATCHm(l80,MEM_BLOCK_ANY,0,l111));}l414:switch(l405){
case INSERT_PROPAGATE:l409[0] = 0;break;case DELETE_PROPAGATE:l409[0] = 1;
break;case PREFIX_LOOKUP:l409[0] = 2;break;case HITBIT_REPLACE:l409[0] = 3;
break;default:return SOC_E_PARAM;}l393[0] = 0;soc_reg_field_set(l80,
L3_DEFIP_AUX_CTRLr,&l393[0],OPCODEf,l409[0]);soc_reg_field_set(l80,
L3_DEFIP_AUX_CTRLr,&l393[0],STARTf,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l80,l393[0]));soc_timeout_init(&l407,50000,5);
sal_memset(l409,0,sizeof(l409));sal_memset(l410,0,sizeof(l410));l411 = 
soc_reg_addr_get(l80,L3_DEFIP_AUX_CTRLr,REG_PORT_ANY,0,
SOC_REG_ADDR_OPTION_NONE,&l412,&l413);do{for(l397 = 0;l397<_TH_PIPES_PER_DEV;
l397++){SOC_IF_ERROR_RETURN(_soc_reg32_get(l80,l412,l397,l411,&l393[l397]));
l409[l397] = soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l393[l397],DONEf);l410[
l397] = soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l393[l397],ERRORf);if(l409[
l397]!= 1){break;}}if(l397>= _TH_PIPES_PER_DEV){l139 = SOC_E_NONE;break;}if(
soc_timeout_check(&l407)){sal_memset(l409,0,sizeof(l409));sal_memset(l410,0,
sizeof(l410));for(l397 = 0;l397<_TH_PIPES_PER_DEV;l397++){SOC_IF_ERROR_RETURN
(_soc_reg32_get(l80,l412,l397,l411,&l393[l397]));l409[l397] = 
soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l393[l397],DONEf);l410[l397] = 
soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l393[l397],ERRORf);if(l409[l397]!= 1
){break;}}if(l397>= _TH_PIPES_PER_DEV){l139 = SOC_E_NONE;}else{LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"unit %d : DEFIP AUX Operation timeout, "
"Pipe %d\n"),l80,l397));l139 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS
(l139)){for(l397 = 0;l397<_TH_PIPES_PER_DEV;l397++){if(l410[l397]){
soc_reg_field_set(l80,L3_DEFIP_AUX_CTRLr,&l393[l397],STARTf,0);
soc_reg_field_set(l80,L3_DEFIP_AUX_CTRLr,&l393[l397],ERRORf,0);
soc_reg_field_set(l80,L3_DEFIP_AUX_CTRLr,&l393[l397],DONEf,0);
SOC_IF_ERROR_RETURN(_soc_reg32_set(l80,l412,l397,l411,l393[l397]));LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"DEFIP AUX Operation encountered "
"parity error in Pipe %d!!\n"),l397));l408++;if(SOC_CONTROL(l80)->
alpm_bulk_retry){sal_sem_take(SOC_CONTROL(l80)->alpm_bulk_retry,1000000);}if(
l408<100){int l415;if(l396(l80,l397,&l415)>= 0){l139 = 
soc_mem_alpm_aux_table_correction(l80,l397,l415);if(l139<0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_th_alpm_aux_error_index "
"pipe %d index %d failed\n"),l397,l415));}}LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"Retry DEFIP AUX Operation in Pipe %d.\n"),l397));goto l414;}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l80));return SOC_E_INTERNAL;}}}if(l405 == PREFIX_LOOKUP){if(l155&&tcam_index)
{*l155 = soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l393[0],HITf);*tcam_index = 
soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l393[0],BKT_INDEXf);
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRL_1r(l80,&l393[1]));*bktid = 
ALPM_BKTID(soc_reg_field_get(l80,L3_DEFIP_AUX_CTRL_1r,l393[1],BKT_PTRf),
soc_reg_field_get(l80,L3_DEFIP_AUX_CTRL_1r,l393[1],SUB_BKT_PTRf));}}}return
l139;}static int l117(int unit,int l107,void*lpm_entry,void*l118,void*l119,
soc_mem_t l120,uint32 l121,uint32*l416,int l123){uint32 l140;uint32 l288[4] = 
{0,0};int l93 = 0;int l139 = SOC_E_NONE;uint32 l122 = 0;sal_memset(l118,0,
soc_mem_entry_words(unit,l120)*4);l140 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l23));
soc_mem_field32_set(unit,l120,l118,HITf,l140);l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l39));soc_mem_field32_set(unit,l120,l118,VALIDf,l140);l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l15));soc_mem_field32_set(unit,l120,l118,ECMPf,l140);l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l19));soc_mem_field32_set(unit,l120,l118,ECMP_PTRf,l140);l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l33));soc_mem_field32_set(unit,l120,l118,NEXT_HOP_INDEXf,l140)
;l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l35));soc_mem_field32_set(unit,l120,l118,PRIf,l140);
l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l37));soc_mem_field32_set(unit,l120,l118,RPEf,l140);
l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l13));soc_mem_field32_set(unit,l120,l118,
DST_DISCARDf,l140);l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l59));soc_mem_field32_set(unit,
l120,l118,SRC_DISCARDf,l140);l140 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l11));
soc_mem_field32_set(unit,l120,l118,CLASS_IDf,l140);soc_mem_field32_set(unit,
l120,l118,SUB_BKT_PTRf,ALPM_BKT_SIDX(l123));l288[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l25));if(l107){l288[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l26));}
soc_mem_field_set(unit,l120,(uint32*)l118,KEYf,(uint32*)l288);if(l107){l140 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l27));if((l139 = _ipmask2pfx(l140,&l93))<0){return(l139);}l140
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l28));if(l93){if(l140!= 0xffffffff){return(
SOC_E_PARAM);}l93+= 32;}else{if((l139 = _ipmask2pfx(l140,&l93))<0){return(
l139);}}}else{l140 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l27));if((l139 = _ipmask2pfx(l140,&l93)
)<0){return(l139);}}if((l93 == 0)&&(l288[0] == 0)&&(l288[1] == 0)){l122 = 1;}
if(l416!= NULL){*l416 = l122;}soc_mem_field32_set(unit,l120,l118,LENGTHf,l93)
;if(l119 == NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){
sal_memset(l119,0,soc_mem_entry_words(unit,l120)*4);sal_memcpy(l119,l118,
soc_mem_entry_words(unit,l120)*4);soc_mem_field32_set(unit,l120,l119,
DST_DISCARDf,0);soc_mem_field32_set(unit,l120,l119,RPEf,0);
soc_mem_field32_set(unit,l120,l119,SRC_DISCARDf,l121&SOC_ALPM_RPF_SRC_DISCARD
);soc_mem_field32_set(unit,l120,l119,DEFAULTROUTEf,l122);}return(SOC_E_NONE);
}static int l124(int unit,void*l118,soc_mem_t l120,int l107,int vrf,int l125,
int index,void*lpm_entry){uint32 l140;uint32 l288[4] = {0,0};uint32 l93 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_words(unit,L3_DEFIPm)*4);l140 = 
soc_mem_field32_get(unit,l120,l118,HITf);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l23),(l140));if(l107)
{soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry
),(l67[(unit)]->l24),(l140));}l140 = soc_mem_field32_get(unit,l120,l118,
VALIDf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l39),(l140));if(l107){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l40),(l140));}l140 = soc_mem_field32_get(unit,l120,l118,ECMPf)
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry
),(l67[(unit)]->l15),(l140));l140 = soc_mem_field32_get(unit,l120,l118,
ECMP_PTRf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),
(lpm_entry),(l67[(unit)]->l19),(l140));l140 = soc_mem_field32_get(unit,l120,
l118,NEXT_HOP_INDEXf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l33),(l140));l140 = soc_mem_field32_get
(unit,l120,l118,PRIf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l35),(l140));l140 = soc_mem_field32_get
(unit,l120,l118,RPEf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l37),(l140));l140 = soc_mem_field32_get
(unit,l120,l118,DST_DISCARDf);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l13),(l140));l140 = 
soc_mem_field32_get(unit,l120,l118,SRC_DISCARDf);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l59),(l140));l140 = soc_mem_field32_get(unit,l120,l118,
CLASS_IDf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),
(lpm_entry),(l67[(unit)]->l11),(l140));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l49),(ALPM_BKT_IDX(
l125)));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l51),(ALPM_BKT_SIDX(l125)));if(index>0){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l47),(index));}soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l31),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l61),(1));if(l107){soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l29),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l30),(1));}soc_mem_field_get(unit,l120,l118,KEYf,l288);if(l107
){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l26),(l288[1]));}soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l25),(l288[0]));l140
= soc_mem_field32_get(unit,l120,l118,LENGTHf);if(l107){if(l140>= 32){l93 = 
0xffffffff;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),
(lpm_entry),(l67[(unit)]->l28),(l93));l93 = ~(((l140-32) == 32)?0:(0xffffffff
)>>(l140-32));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm
)),(lpm_entry),(l67[(unit)]->l27),(l93));}else{l93 = ~(0xffffffff>>l140);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l28),(l93));}}else{assert(l140<= 32);l93 = ~(((l140) == 32)?0:
(0xffffffff)>>(l140));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l27),(l93));}if(vrf == 
SOC_L3_VRF_OVERRIDE){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l45),(1));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l21),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l41),(0));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l43),(0));}else if(vrf == SOC_L3_VRF_GLOBAL){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l21),(1));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l41),(0));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l43),(0));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l41),(vrf));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l43),(SOC_VRF_MAX(unit)));}return(SOC_E_NONE);}int
soc_th_alpm_warmboot_pivot_add(int unit,int l107,void*lpm_entry,int l417,int
bktid){int l139 = SOC_E_NONE;uint32 key[2] = {0,0};alpm_pivot_t*l222 = NULL;
alpm_bucket_handle_t*l265 = NULL;int vrf_id = 0,vrf = 0;uint32 l418;trie_t*
l308 = NULL;uint32 prefix[5] = {0};int l122 = 0;SOC_IF_ERROR_RETURN(l149(unit
,l107,lpm_entry,prefix,&l418,&l122));SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&vrf_id,&vrf));l417 = 
soc_th_alpm_physical_idx(unit,L3_DEFIPm,l417,l107);l265 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l265 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l265,0,sizeof(*l265));
l222 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l222 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l265);return SOC_E_MEMORY;}sal_memset(l222,0
,sizeof(*l222));PIVOT_BUCKET_HANDLE(l222) = l265;if(l107){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l222));key[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l25));key[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l26));}else{trie_init
(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l222));key[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l25));}PIVOT_BUCKET_INDEX(l222) = bktid;PIVOT_TCAM_INDEX(l222)
= l417;if(!(ALPM_PREFIX_IN_TCAM(unit,vrf_id))){if(l107 == 0){l308 = 
VRF_PIVOT_TRIE_IPV4(unit,vrf);if(l308 == NULL){trie_init(_MAX_KEY_LEN_48_,&
VRF_PIVOT_TRIE_IPV4(unit,vrf));l308 = VRF_PIVOT_TRIE_IPV4(unit,vrf);}}else{
l308 = VRF_PIVOT_TRIE_IPV6(unit,vrf);if(l308 == NULL){trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(unit,vrf));l308 = VRF_PIVOT_TRIE_IPV6(
unit,vrf);}}sal_memcpy(l222->key,prefix,sizeof(prefix));l222->len = l418;l139
= trie_insert(l308,l222->key,NULL,l222->len,(trie_node_t*)l222);if(
SOC_FAILURE(l139)){sal_free(l265);sal_free(l222);return l139;}}
ALPM_TCAM_PIVOT(unit,l417) = l222;PIVOT_BUCKET_VRF(l222) = vrf;
PIVOT_BUCKET_IPV6(l222) = l107;PIVOT_BUCKET_ENT_CNT_UPDATE(l222);if(key[0] == 
0&&key[1] == 0){PIVOT_BUCKET_DEF(l222) = TRUE;}VRF_PIVOT_REF_INC(unit,vrf,
l107);return l139;}static int l419(int unit,int l107,void*lpm_entry,void*l118
,soc_mem_t l120,int l417,int bktid,int l420){int l421;int vrf;int l139 = 
SOC_E_NONE;int l122 = 0;uint32 prefix[5] = {0,0,0,0,0};uint32 l154;
defip_entry_t l422;trie_t*l266 = NULL;trie_t*l262 = NULL;trie_node_t*l257 = 
NULL;payload_t*l423 = NULL;payload_t*l286 = NULL;alpm_pivot_t*pivot_pyld = 
NULL;if((NULL == lpm_entry)||(NULL == l118)){return SOC_E_PARAM;}if(l107){if(
!(l107 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l30)))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l421,&vrf));l120 = (l107)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(l124(unit,l118
,l120,l107,l421,bktid,l417,&l422));l139 = l149(unit,l107,&l422,prefix,&l154,&
l122);if(SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"prefix create failed\n")));return l139;}pivot_pyld = ALPM_TCAM_PIVOT(unit,
l417);l266 = PIVOT_BUCKET_TRIE(pivot_pyld);l423 = sal_alloc(sizeof(payload_t)
,"Payload for Key");if(NULL == l423){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}
l286 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == 
l286){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node\n")));sal_free(l423);return
SOC_E_MEMORY;}sal_memset(l423,0,sizeof(*l423));sal_memset(l286,0,sizeof(*l286
));l423->key[0] = prefix[0];l423->key[1] = prefix[1];l423->key[2] = prefix[2]
;l423->key[3] = prefix[3];l423->key[4] = prefix[4];l423->len = l154;l423->
index = l420;sal_memcpy(l286,l423,sizeof(*l423));l139 = trie_insert(l266,
prefix,NULL,l154,(trie_node_t*)l423);if(SOC_FAILURE(l139)){goto l424;}if(l107
){l262 = VRF_PREFIX_TRIE_IPV6(unit,vrf);}else{l262 = VRF_PREFIX_TRIE_IPV4(
unit,vrf);}if(!l122){l139 = trie_insert(l262,prefix,NULL,l154,(trie_node_t*)
l286);if(SOC_FAILURE(l139)){goto l425;}}return l139;l425:(void)trie_delete(
l266,prefix,l154,&l257);l423 = (payload_t*)l257;l424:sal_free(l423);sal_free(
l286);return l139;}static int l426(int unit,int l130,int vrf,int l142,int
bktid){int l139 = SOC_E_NONE;uint32 l154;uint32 key[2] = {0,0};trie_t*l427 = 
NULL;payload_t*l297 = NULL;defip_entry_t*lpm_entry = NULL;lpm_entry = 
sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"unable to allocate memory for LPM entry\n")));return SOC_E_MEMORY;}l126(unit
,key,0,vrf,l130,lpm_entry,0,1);if(vrf == SOC_VRF_MAX(unit)+1){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l21),(1));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l53),(1));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l49),(ALPM_BKT_IDX(bktid)));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l51),(ALPM_BKT_SIDX
(bktid)));if(l130 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(unit,vrf) = lpm_entry;
trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(unit,vrf));l427 = 
VRF_PREFIX_TRIE_IPV4(unit,vrf);}else{VRF_TRIE_DEFAULT_ROUTE_IPV6(unit,vrf) = 
lpm_entry;trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(unit,vrf));l427 = 
VRF_PREFIX_TRIE_IPV6(unit,vrf);}l297 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l297 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l297,0,sizeof(*l297));l154 = 0;l297->key[0] = key[0]
;l297->key[1] = key[1];l297->len = l154;l139 = trie_insert(l427,key,NULL,l154
,&(l297->node));if(SOC_FAILURE(l139)){sal_free(l297);return l139;}
VRF_TRIE_INIT_DONE(unit,vrf,l130,1);return l139;}int
soc_th_alpm_warmboot_prefix_insert(int unit,int l107,void*lpm_entry,void*l118
,int l417,int bktid,int l420){int l421;int vrf;int l139 = SOC_E_NONE;
soc_mem_t l120;l417 = soc_th_alpm_physical_idx(unit,L3_DEFIPm,l417,l107);l120
= (l107)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l421,&vrf));if(ALPM_PREFIX_IN_TCAM(
unit,l421)){return(l139);}if(!VRF_TRIE_INIT_COMPLETED(unit,vrf,l107)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d is not initialized\n"),
vrf));l139 = l426(unit,l107,vrf,l417,bktid);if(SOC_FAILURE(l139)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d/%d trie init \n""failed\n"),vrf,l107
));return l139;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d/%d trie init completed\n"),vrf,l107));}l139 = l419(unit,l107,
lpm_entry,l118,l120,l417,bktid,l420);if(l139!= SOC_E_NONE){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unit %d : Route Insertion Failed :%s\n"),
unit,soc_errmsg(l139)));return(l139);}soc_th_alpm_bu_upd(unit,bktid,l417,vrf,
l107,1);VRF_TRIE_ROUTES_INC(unit,vrf,l107);return(l139);}int
soc_th_alpm_warmboot_bucket_bitmap_set(int l80,int vrf,int l130,int l212){int
l219 = 1;SHR_BITDCL*l342 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80);if(vrf == 
SOC_VRF_MAX(l80)+1){l342 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l80);}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l130)){l219 = 2;}SHR_BITSET_RANGE(l342,l212,l219)
;return SOC_E_NONE;}int soc_th_alpm_warmboot_lpm_reinit_done(int unit){int
l142;int l428 = ((3*(64+32+2+1))-1);int l429;int l341 = soc_mem_index_count(
unit,L3_DEFIPm);if(SOC_URPF_STATUS_GET(unit)){l341>>= 1;}l429 = 
soc_th_alpm_mode_get(unit);if(l429 == SOC_ALPM_MODE_COMBINED){(l9[(unit)][(((
3*(64+32+2+1))-1))].l4) = -1;for(l142 = ((3*(64+32+2+1))-1);l142>-1;l142--){
if(-1 == (l9[(unit)][(l142)].l2)){continue;}(l9[(unit)][(l142)].l4) = l428;(
l9[(unit)][(l428)].next) = l142;(l9[(unit)][(l428)].l6) = (l9[(unit)][(l142)]
.l2)-(l9[(unit)][(l428)].l3)-1;l428 = l142;}(l9[(unit)][(l428)].next) = -1;(
l9[(unit)][(l428)].l6) = l341-(l9[(unit)][(l428)].l3)-1;}else if(l429 == 1){(
l9[(unit)][(((3*(64+32+2+1))-1))].l4) = -1;for(l142 = ((3*(64+32+2+1))-1);
l142>(((3*(64+32+2+1))-1)/3);l142--){if(-1 == (l9[(unit)][(l142)].l2)){
continue;}(l9[(unit)][(l142)].l4) = l428;(l9[(unit)][(l428)].next) = l142;(l9
[(unit)][(l428)].l6) = (l9[(unit)][(l142)].l2)-(l9[(unit)][(l428)].l3)-1;l428
= l142;}(l9[(unit)][(l428)].next) = -1;(l9[(unit)][(l428)].l6) = l341-(l9[(
unit)][(l428)].l3)-1;l428 = (((3*(64+32+2+1))-1)/3);(l9[(unit)][((((3*(64+32+
2+1))-1)/3))].l4) = -1;for(l142 = ((((3*(64+32+2+1))-1)/3)-1);l142>-1;l142--)
{if(-1 == (l9[(unit)][(l142)].l2)){continue;}(l9[(unit)][(l142)].l4) = l428;(
l9[(unit)][(l428)].next) = l142;(l9[(unit)][(l428)].l6) = (l9[(unit)][(l142)]
.l2)-(l9[(unit)][(l428)].l3)-1;l428 = l142;}(l9[(unit)][(l428)].next) = -1;(
l9[(unit)][(l428)].l6) = (l341>>1)-(l9[(unit)][(l428)].l3)-1;}return(
SOC_E_NONE);}int soc_th_alpm_warmboot_lpm_reinit(int unit,int l107,int l142,
void*lpm_entry){int l114;defip_entry_t*l430;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l39))||soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l40))){l347(unit,lpm_entry,l142,0x4000,
0);}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l39))){SOC_IF_ERROR_RETURN(l101(unit,l107,lpm_entry,
&l114,0,0));if((l9[(unit)][(l114)].l5) == 0){(l9[(unit)][(l114)].l2) = l142;(
l9[(unit)][(l114)].l3) = l142;}else{(l9[(unit)][(l114)].l3) = l142;}(l9[(unit
)][(l114)].l5)++;if(l107){return(SOC_E_NONE);}}else{if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l40))){l430 = sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");
soc_th_alpm_lpm_ip4entry1_to_0(unit,lpm_entry,l430,TRUE);SOC_IF_ERROR_RETURN(
l101(unit,l107,l430,&l114,0,0));if((l9[(unit)][(l114)].l5) == 0){(l9[(unit)][
(l114)].l2) = l142;(l9[(unit)][(l114)].l3) = l142;}else{(l9[(unit)][(l114)].
l3) = l142;}sal_free(l430);(l9[(unit)][(l114)].l5)++;}}return(SOC_E_NONE);}
void soc_th_alpm_dbg_urpf(int l80){int l139 = SOC_E_NONE;soc_mem_t l120;int
l163,l143;int index;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];uint32 l431[
SOC_MAX_MEM_FIELD_WORDS];int l432 = 0,l433 = 0;if(!SOC_URPF_STATUS_GET(l80)){
LOG_CLI((BSL_META_U(l80,"Only supported in uRPF mode\n")));return;}for(l143 = 
0;l143<= SOC_TH_MAX_ALPM_BUCKETS/2;l143++){index = 
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,ALPM_BKTID(l143,0));l120 = 
_soc_tomahawk_alpm_bkt_view_get(l80,index);for(l163 = 0;l163<
SOC_TH_MAX_BUCKET_ENTRIES-1;l163++){l139 = _soc_th_alpm_mem_index(l80,l120,
l143,l163,0,&index);if(SOC_FAILURE(l139)){continue;}sal_memset(l81,0,sizeof(
uint32)*SOC_MAX_MEM_FIELD_WORDS);sal_memset(l431,0,sizeof(uint32)*
SOC_MAX_MEM_FIELD_WORDS);l139 = soc_mem_read(l80,l120,MEM_BLOCK_ANY,index,l81
);if(SOC_FAILURE(l139)){continue;}if(soc_mem_field32_get(l80,l120,l81,VALIDf)
== 0){continue;}soc_mem_field32_set(l80,l120,l81,HITf,0);l139 = soc_mem_read(
l80,l120,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,index),l431);if(SOC_FAILURE
(l139)){continue;}soc_mem_field32_set(l80,l120,l431,HITf,0);
soc_mem_field32_set(l80,l120,l431,RPEf,0);if(sal_memcmp(l81,l431,sizeof(
uint32)*SOC_MAX_MEM_FIELD_WORDS)!= 0){LOG_CLI((BSL_META_U(l80,
"uRPF sanity check failed: bucket %4d mem %s index %d uRPF index %d\n"),l143,
SOC_MEM_NAME(l80,l120),index,_soc_th_alpm_rpf_entry(l80,index)));l433++;}else
{l432++;}}}LOG_CLI((BSL_META_U(l80,
"uRPF Sanity check finished, good %d bad %d\n"),l432,l433));return;}typedef
struct l434{int v4;int v6_64;int v6_128;int l435;int l436;int l437;int l220;}
l438;typedef enum l439{l440 = 0,l441,l442,l443,l444,l445}l446;static void l447
(int l80,alpm_vrf_counter_t*l448){l448->v4 = soc_mem_index_count(l80,
L3_DEFIPm)*2;l448->v6_128 = soc_mem_index_count(l80,L3_DEFIP_PAIR_128m);if(
soc_property_get(l80,spn_IPV6_LPM_128B_ENABLE,1)){l448->v6_64 = l448->v6_128;
}else{l448->v6_64 = l448->v4>>1;}if(SOC_URPF_STATUS_GET(l80)){l448->v4>>= 1;
l448->v6_128>>= 1;l448->v6_64>>= 1;}}static void l449(int l80,int vrf_id,
alpm_vrf_handle_t*l450,l446 l451){alpm_vrf_counter_t*l452;int l163,l453,l454,
l455;int l395 = 0;alpm_vrf_counter_t l448;switch(l451){case l440:LOG_CLI((
BSL_META_U(l80,"\nAdd Counter:\n")));break;case l441:LOG_CLI((BSL_META_U(l80,
"\nDelete Counter:\n")));break;case l442:LOG_CLI((BSL_META_U(l80,
"\nInternal Debug Counter - 1:\n")));break;case l443:l447(l80,&l448);LOG_CLI(
(BSL_META_U(l80,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l448.
v4,l448.v6_64,l448.v6_128));break;case l444:LOG_CLI((BSL_META_U(l80,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l445:LOG_CLI((
BSL_META_U(l80,"\nInternal Debug Counter - LPM Full:\n")));break;default:
break;}LOG_CLI((BSL_META_U(l80,
"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((BSL_META_U(l80
,"-----------------------------------------------\n")));l453 = l454 = l455 = 
0;for(l163 = 0;l163<MAX_VRF_ID+1;l163++){int l456,l457,l458;if(l450[l163].
init_done == 0&&l163!= MAX_VRF_ID){continue;}if(vrf_id!= -1&&vrf_id!= l163){
continue;}l395 = 1;switch(l451){case l440:l452 = &l450[l163].add;break;case
l441:l452 = &l450[l163].del;break;case l442:l452 = &l450[l163].bkt_split;
break;case l444:l452 = &l450[l163].lpm_shift;break;case l445:l452 = &l450[
l163].lpm_full;break;case l443:l452 = &l450[l163].pivot_used;break;default:
l452 = &l450[l163].pivot_used;break;}l456 = l452->v4;l457 = l452->v6_64;l458 = 
l452->v6_128;l453+= l456;l454+= l457;l455+= l458;do{LOG_CLI((BSL_META_U(l80,
"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l163 == MAX_VRF_ID?-1:l163),(l456),(
l457),(l458),((l456+l457+l458)),(l163) == MAX_VRF_ID?"GHi":(l163) == 
SOC_VRF_MAX(l80)+1?"GLo":""));}while(0);}if(l395 == 0){LOG_CLI((BSL_META_U(
l80,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l80,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l80,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l453),(l454),(l455),((l453+
l454+l455))));}while(0);}return;}int soc_th_alpm_debug_show(int l80,int vrf_id
,uint32 flags){int l163,l459,l395 = 0;l438*l460;l438 l172;l438 l461;if(vrf_id
>(SOC_VRF_MAX(l80)+1)){return SOC_E_PARAM;}l459 = MAX_VRF_ID*sizeof(l438);
l460 = sal_alloc(l459,"_alpm_dbg_cnt");if(l460 == NULL){return SOC_E_MEMORY;}
sal_memset(l460,0,l459);l172.v4 = ALPM_IPV4_BKT_COUNT;l172.v6_64 = 
ALPM_IPV6_64_BKT_COUNT;l172.v6_128 = ALPM_IPV6_128_BKT_COUNT;if((
soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_COMBINED||soc_th_alpm_mode_get(l80
) == SOC_ALPM_MODE_TCAM_ALPM)&&!SOC_URPF_STATUS_GET(l80)){l172.v6_64<<= 1;
l172.v6_128<<= 1;}LOG_CLI((BSL_META_U(l80,"\nBucket Occupancy:\n")));if(flags
&(SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l80,
"\nPivot/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l80,"-----------------------------------------------\n")));}for(
l163 = 0;l163<MAX_PIVOT_COUNT;l163++){alpm_pivot_t*l462 = ALPM_TCAM_PIVOT(l80
,l163);if(l462!= NULL){l438*l463;int vrf = PIVOT_BUCKET_VRF(l462);if(vrf<0||
vrf>(SOC_VRF_MAX(l80)+1)){continue;}if(vrf_id!= -1&&vrf_id!= vrf){continue;}
if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l395 = 1;do{LOG_CLI((BSL_META_U(l80,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l163,vrf,PIVOT_BUCKET_MIN(l462),
PIVOT_BUCKET_MAX(l462),PIVOT_BUCKET_COUNT(l462),PIVOT_BUCKET_DEF(l462)?"Def":
(vrf) == SOC_VRF_MAX(l80)+1?"GLo":""));}while(0);}l463 = &l460[vrf];if(
PIVOT_BUCKET_IPV6(l462) == L3_DEFIP_MODE_128){l463->v6_128+= 
PIVOT_BUCKET_COUNT(l462);l463->l437+= l172.v6_128;}else if(PIVOT_BUCKET_IPV6(
l462) == L3_DEFIP_MODE_64){l463->v6_64+= PIVOT_BUCKET_COUNT(l462);l463->l436
+= l172.v6_64;}else{l463->v4+= PIVOT_BUCKET_COUNT(l462);l463->l435+= l172.v4;
}l463->l220 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l395 == 0){
LOG_CLI((BSL_META_U(l80,"%9s\n"),"Specific VRF not found"));}}sal_memset(&
l461,0,sizeof(l461));l395 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){
LOG_CLI((BSL_META_U(l80,"\n      VRF  v4      v6-64   v6-128  |   Total\n")))
;LOG_CLI((BSL_META_U(l80,"-----------------------------------------------\n")
));for(l163 = 0;l163<MAX_VRF_ID;l163++){l438*l463;if(l460[l163].l220!= TRUE){
continue;}if(vrf_id!= -1&&vrf_id!= l163){continue;}l395 = 1;l463 = &l460[l163
];do{(&l461)->v4+= (l463)->v4;(&l461)->l435+= (l463)->l435;(&l461)->v6_64+= (
l463)->v6_64;(&l461)->l436+= (l463)->l436;(&l461)->v6_128+= (l463)->v6_128;(&
l461)->l437+= (l463)->l437;}while(0);do{LOG_CLI((BSL_META_U(l80,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l163),(l463
->l435)?(l463->v4)*100/(l463->l435):0,(l463->l435)?(l463->v4)*1000/(l463->
l435)%10:0,(l463->l436)?(l463->v6_64)*100/(l463->l436):0,(l463->l436)?(l463->
v6_64)*1000/(l463->l436)%10:0,(l463->l437)?(l463->v6_128)*100/(l463->l437):0,
(l463->l437)?(l463->v6_128)*1000/(l463->l437)%10:0,((l463->l435+l463->l436+
l463->l437))?((l463->v4+l463->v6_64+l463->v6_128))*100/((l463->l435+l463->
l436+l463->l437)):0,((l463->l435+l463->l436+l463->l437))?((l463->v4+l463->
v6_64+l463->v6_128))*1000/((l463->l435+l463->l436+l463->l437))%10:0,(l163) == 
SOC_VRF_MAX(l80)+1?"GLo":""));}while(0);}if(l395 == 0){LOG_CLI((BSL_META_U(
l80,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l80,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l80,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l461)->l435)?((&l461)->v4)*100/((&l461)->l435):0,((&l461)->l435)?((&l461)->v4
)*1000/((&l461)->l435)%10:0,((&l461)->l436)?((&l461)->v6_64)*100/((&l461)->
l436):0,((&l461)->l436)?((&l461)->v6_64)*1000/((&l461)->l436)%10:0,((&l461)->
l437)?((&l461)->v6_128)*100/((&l461)->l437):0,((&l461)->l437)?((&l461)->
v6_128)*1000/((&l461)->l437)%10:0,(((&l461)->l435+(&l461)->l436+(&l461)->l437
))?(((&l461)->v4+(&l461)->v6_64+(&l461)->v6_128))*100/(((&l461)->l435+(&l461)
->l436+(&l461)->l437)):0,(((&l461)->l435+(&l461)->l436+(&l461)->l437))?(((&
l461)->v4+(&l461)->v6_64+(&l461)->v6_128))*1000/(((&l461)->l435+(&l461)->l436
+(&l461)->l437))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l449(l80,vrf_id,alpm_vrf_handle[l80],l443);}if(flags&
SOC_ALPM_DEBUG_SHOW_FLAG_CNT){l449(l80,vrf_id,alpm_vrf_handle[l80],l440);l449
(l80,vrf_id,alpm_vrf_handle[l80],l441);}if(flags&
SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l449(l80,vrf_id,alpm_vrf_handle[l80],l442);
l449(l80,vrf_id,alpm_vrf_handle[l80],l445);l449(l80,vrf_id,alpm_vrf_handle[
l80],l444);}sal_free(l460);return SOC_E_NONE;}int soc_th_alpm_debug_brief_show
(int l80,int l125,int index){int l163,l316,l139 = SOC_E_NONE;uint32 l393,l464
= 0,l137 = 0,l231,l465,l466,l467;int l468 = 0,l469;int l470 = 0,l471;int l472
= 0,l473;int l474 = 0,l475;char*l476 = NULL;defip_entry_t*lpm_entry;
defip_pair_128_entry_t*l477;int l107,bkt_ptr,l478,l479,l480;int*l481 = NULL;
int l482 = 0,l483 = 0;soc_mem_t l177;int l484 = 0,l485 = 0,l486 = 0,l487 = 0,
l488;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];soc_field_t l489,l490,l491,l492;char
*l493[] = {"LPM","ALPM-Combined","ALPM-Parallel","ALPM-TCAM_ALPM",
"ALPM-Invalid"};SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l80,&l393));
l464 = soc_reg_field_get(l80,L3_DEFIP_RPF_CONTROLr,l393,LPM_MODEf);if(l464!= 
0){l137 = soc_reg_field_get(l80,L3_DEFIP_RPF_CONTROLr,l393,LOOKUP_MODEf);}
l231 = soc_reg_field_get(l80,L3_DEFIP_RPF_CONTROLr,l393,DEFIP_RPF_ENABLEf);
LOG_CLI(("\n"));LOG_CLI(("%-24s%s\n","Mode:",l493[l464+l137]));LOG_CLI((
"%-24s%s\n","uRPF:",l231?"Enable":"Disable"));l465 = !!soc_mem_index_count(
l80,L3_DEFIP_PAIR_128m);LOG_CLI(("%-24s%d\n","IPv6 Bits:",l465?128:64));if(
l464 == 0){return SOC_E_NONE;}if(l137 == 0&&!l231){l466 = soc_reg_field_get(
l80,L3_DEFIP_RPF_CONTROLr,l393,ENABLE_DOUBLE_WIDE_BUCKETS_FOR_IPV4f);}else if
(l137 == 2){l466 = 1;}else{l466 = 0;}LOG_CLI(("%-24s%s\n","IPv4 Wide:",l466?
"Double":"Single"));if(l137 == 0&&!l231){l466 = 1;}else if(l137 == 2){l466 = 
1;}else{l466 = 0;}LOG_CLI(("%-24s%s\n","IPv6 Wide:",l466?"Double":"Single"));
SOC_IF_ERROR_RETURN(READ_ISS_BANK_CONFIGr(l80,&l393));l467 = 
soc_reg_field_get(l80,ISS_BANK_CONFIGr,l393,ALPM_ENTRY_BANK_CONFIGf);LOG_CLI(
("%-24s%d(0x%x)\n","Banks:",_shr_popcount(l467),l467));l469 = 2*
soc_mem_index_count(l80,L3_DEFIPm);l471 = l465?soc_mem_index_count(l80,
L3_DEFIP_PAIR_128m):soc_mem_index_count(l80,L3_DEFIPm);l473 = 
soc_mem_index_count(l80,L3_DEFIP_PAIR_128m);l475 = SOC_TH_MAX_ALPM_BUCKETS;if
(l231){l469>>= 1;l471>>= 1;l473>>= 1;l475>>= 1;}l481 = sal_alloc(l475*sizeof(
int),"bktref");if(l481 == NULL){return SOC_E_MEMORY;}sal_memset(l481,0,l475*
sizeof(int));l177 = L3_DEFIPm;l488 = WORDS2BYTES(soc_mem_entry_words(l80,l177
))*soc_mem_index_count(l80,l177);l476 = soc_cm_salloc(l80,l488,"lpm_tbl");if(
l476 == NULL){l139 = SOC_E_MEMORY;goto l494;}sal_memset(l476,0,l488);if(l469
&&soc_mem_read_range(l80,l177,MEM_BLOCK_ANY,soc_mem_index_min(l80,l177),
soc_mem_index_max(l80,l177),l476)<0){l139 = (SOC_E_INTERNAL);goto l494;}for(
l163 = 0;l163<(l469/2);l163++){lpm_entry = soc_mem_table_idx_to_pointer(l80,
l177,defip_entry_t*,l476,l163);l107 = soc_mem_field32_get(l80,l177,lpm_entry,
MODE0f);for(l316 = 0;l316<(l107?1:2);l316++){if(l316 == 1){l489 = VALID1f;}
else{l489 = VALID0f;}if(!soc_mem_field32_get(l80,l177,lpm_entry,l489)){
continue;}if(l107){l470++;}else{l468++;}if(l316 == 0){l491 = GLOBAL_HIGH0f;
l490 = ALG_BKT_PTR0f;l492 = GLOBAL_ROUTE0f;}else{l491 = GLOBAL_HIGH1f;l490 = 
ALG_BKT_PTR1f;l492 = GLOBAL_ROUTE1f;}l478 = soc_mem_field32_get(l80,l177,
lpm_entry,l491);l479 = soc_mem_field32_get(l80,l177,lpm_entry,l492);if(l479){
if(l478){l482++;l480 = 0;}else{l483++;l480 = (l137 == 2)?0:1;}}else{l480 = 1;
}if(l480){bkt_ptr = soc_mem_field32_get(l80,l177,lpm_entry,l490);if(bkt_ptr>
l475){LOG_CLI(("Invalid bucket pointer in %s index %d\n",SOC_MEM_NAME(l80,
l177),l163));}else{l481[bkt_ptr]++;}}}}soc_cm_sfree(l80,l476);l476 = NULL;
l177 = L3_DEFIP_PAIR_128m;l488 = WORDS2BYTES(soc_mem_entry_words(l80,l177))*
soc_mem_index_count(l80,l177);l476 = soc_cm_salloc(l80,l488,"lpm_tbl_p128");
if(l476 == NULL){l139 = SOC_E_MEMORY;goto l494;}sal_memset(l476,0,l488);if(
l473&&soc_mem_read_range(l80,l177,MEM_BLOCK_ANY,soc_mem_index_min(l80,l177),
soc_mem_index_max(l80,l177),l476)<0){l139 = (SOC_E_INTERNAL);goto l494;}for(
l163 = 0;l163<l473;l163++){l477 = soc_mem_table_idx_to_pointer(l80,l177,
defip_pair_128_entry_t*,l476,l163);if(0x03!= soc_mem_field32_get(l80,l177,
l477,MODE1_UPRf)){continue;};if(!soc_mem_field32_get(l80,l177,l477,
VALID1_LWRf)||!soc_mem_field32_get(l80,l177,l477,VALID0_LWRf)||!
soc_mem_field32_get(l80,l177,l477,VALID1_UPRf)||!soc_mem_field32_get(l80,l177
,l477,VALID0_UPRf)){continue;};l472++;l478 = soc_mem_field32_get(l80,l177,
l477,GLOBAL_HIGHf);l479 = soc_mem_field32_get(l80,l177,l477,GLOBAL_ROUTEf);if
(l479){if(l478){l482++;l480 = 0;}else{l483++;l480 = (l137 == 2)?0:1;}}else{
l480 = 1;}if(l480){bkt_ptr = soc_mem_field32_get(l80,l177,l477,ALG_BKT_PTRf);
if(bkt_ptr>l475){LOG_CLI(("Invalid bucket pointer in %s index %d\n",
SOC_MEM_NAME(l80,l177),l163));}else{l481[bkt_ptr]++;}}}for(l163 = 0;l163<l475
;l163++){if(l481[l163]!= 0){l474++;}if(l481[l163] == 4){l487++;}if(l481[l163]
== 3){l486++;}if(l481[l163] == 2){l485++;}if(l481[l163] == 1){l484++;}}if(
SOC_TH_ALPM_SCALE_CHECK(l80,0)){l475>>= 1;}LOG_CLI(("\n"));LOG_CLI((
"%-24s%d/%-4d\n","IPv4 Pivots:",l468,l469));LOG_CLI(("%-24s%d/%-4d\n",
"IPv6-64 Pivots:",l470,l471));LOG_CLI(("%-24s%d/%-4d\n","IPv6-128 Pivots:",
l472,l473));LOG_CLI(("%-24s%d/%-4d\n","Buckets:",l474,l475));LOG_CLI((
"%-24s%d\n","Global High:",l482));LOG_CLI(("%-24s%d\n","Global Low:",l483));
LOG_CLI(("\n"));LOG_CLI(("%s\n","Bucket Sharing:"));LOG_CLI(("%-24s%d\n",
"1->1:",l484));LOG_CLI(("%-24s%d\n","2->1:",l485));LOG_CLI(("%-24s%d\n",
"3->1:",l486));LOG_CLI(("%-24s%d\n","4->1:",l487));LOG_CLI(("\n"));LOG_CLI((
"%-24s%d\n","Split count:",alpm_split_count+alpm_128_split_count));LOG_CLI((
"%-24s%d\n","Merge count:",alpm_merge_count));LOG_CLI(("\n\n"));l494:if(l481)
{sal_free(l481);l481 = NULL;}if(l476){soc_cm_sfree(l80,l476);l476 = NULL;}if(
l125!= -1){int l495,l496,l146;l177 = _soc_tomahawk_alpm_bkt_view_get(l80,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,ALPM_BKTID(l125,0)));LOG_CLI((
"Bucket = %4d (View %s)\n",l125,l177 == -1?"INVALID":SOC_MEM_NAME(l80,l177)))
;LOG_CLI(("------------------------------------------------------------\n"));
LOG_CLI(("Index distribution (* indicates occupied)\n"));switch(l177){case
L3_DEFIP_ALPM_IPV4m:l496 = 6;break;case L3_DEFIP_ALPM_IPV6_64m:l496 = 4;break
;case L3_DEFIP_ALPM_IPV6_128m:l496 = 2;break;default:l177 = 
L3_DEFIP_ALPM_IPV4m;l496 = 6;break;}for(l495 = 0;l495<4;l495++){if(!(l467&(1
<<l495))){continue;}LOG_CLI(("Bank%d: ",l495));for(l163 = 0;l163<l496;l163++)
{l146 = (_shr_popcount(l467)+1)/2;l316 = (l163<<(l146+SOC_TH_ALPM_BUCKET_BITS
))|(l125<<l146)|l495;l139 = soc_mem_read(l80,l177,MEM_BLOCK_ANY,l316,l81);if(
l139 == 0&&soc_mem_field32_get(l80,l177,l81,VALIDf)){l146 = 1;}else{l146 = 0;
}if(l146){LOG_CLI(("%d(*) ",l316));}else{LOG_CLI(("%d( ) ",l316));}}LOG_CLI((
"\n"));}}LOG_CLI(("\n"));if(index!= -1){int bktid;LOG_CLI(("Index = %8d\n",
index));LOG_CLI((
"------------------------------------------------------------\n"));l177 = 
_soc_tomahawk_alpm_bkt_view_get(l80,index);LOG_CLI(("%-24s%s\n","Type",l177 == 
-1?"INVALID":SOC_MEM_NAME(l80,l177)));bktid = SOC_TH_ALPM_BKT_ENTRY_TO_IDX(
l80,index);LOG_CLI(("%-24s%d\n","Bucket:",ALPM_BKT_IDX(bktid)));LOG_CLI((
"%-24s%d\n","Sub-Bucket:",ALPM_BKT_SIDX(bktid)));LOG_CLI(("%-24s%d\n","Bank:"
,index&(_shr_popcount(l467)-1)));bktid = (_shr_popcount(l467)+1)/2+
SOC_TH_ALPM_BUCKET_BITS;LOG_CLI(("%-24s%d\n","Entry:",index>>bktid));LOG_CLI(
("\n"));}return SOC_E_NONE;}STATIC int _soc_th_alpm_sanity_check(int l80,int
l399,int l400,int l497,int*l498){int l139 = SOC_E_NONE;int l499,l500,l501,
l420,l428 = 0;int l163,l316,l107,l502 = 0,l503 = 0;int vrf_id,vrf;int l504,
l505,l506;int l217,l321,bkt_ptr,l136;int l507 = 0,l508 = 0,l509 = 0,l510 = 0;
int l220;int l226,l511;int l512,l122;int l488;uint32 l141 = 0;uint32 l81[
SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l229[SOC_MAX_MEM_FIELD_WORDS] = {0};
uint32 prefix[5],l154;trie_t*l262 = NULL;trie_node_t*l157 = NULL;payload_t*
l272 = NULL;uint32 l513[10] = {0};uint32 l514[10] = {0};char*l515 = NULL;
defip_entry_t*lpm_entry,l516,l517;soc_mem_t l518;soc_mem_t l519;l488 = sizeof
(defip_entry_t)*(l400-l399+1);l515 = soc_cm_salloc(l80,l488,"lpm_tbl");if(
l515 == NULL){return(SOC_E_MEMORY);}sal_memset(l515,0,l488);SOC_ALPM_LPM_LOCK
(l80);if(soc_mem_read_range(l80,L3_DEFIPm,MEM_BLOCK_ANY,l399,l400,l515)<0){
soc_cm_sfree(l80,l515);SOC_ALPM_LPM_UNLOCK(l80);return(SOC_E_INTERNAL);}for(
l499 = l399;l499<= l400;l499++){lpm_entry = soc_mem_table_idx_to_pointer(l80,
L3_DEFIPm,defip_entry_t*,l515,l499-l399);l107 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l29));if(l107){l519 = L3_DEFIP_ALPM_IPV6_64m;if(
SOC_TH_ALPM_SCALE_CHECK(l80,l107)){l321 = 32;}else{l321 = 16;}}else{l519 = 
L3_DEFIP_ALPM_IPV4m;l321 = 24;}for(l163 = 0;l163<= (l107?0:1);l163++){if(l507
){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tL3_DEFIP index:[%d,%d] check failed. ipv6 %d error count %d\n"),l428,l503,
l502,l507));l509 = 1;}else if(l510){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tL3_DEFIP index:[%d,%d] check passed. ipv6 %d success count %d\n"),l428,
l503,l502,l510));}l507 = l508 = l510 = 0;l502 = l107;l428 = l499;l503 = l163;
if(l163 == 1){soc_th_alpm_lpm_ip4entry1_to_0(l80,lpm_entry,lpm_entry,
PRESERVE_HIT);}bkt_ptr = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80
,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l49));if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l39))){continue;}if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l45))||(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l21))&&soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_TCAM_ALPM)){if
(bkt_ptr!= 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tWrong bkt_ptr %d\n"),bkt_ptr));l507++;}continue;}if(bkt_ptr == 0){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"\tWrong bkt_ptr2 %d\n"),bkt_ptr));
l507++;}l316 = 0;l513[l316++] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l15));l513[l316++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l19));l513[l316++] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l33));l513[l316++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l35));l513[l316++] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l37));l513[l316++] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l13));l513[l316++] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l11));l136 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l51));if(l498){if(l498[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l136] == -
1){l498[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l136] = l499;if(
SOC_TH_ALPM_SCALE_CHECK(l80,l107)){l498[(bkt_ptr+1)*SOC_TH_MAX_SUB_BUCKETS+
l136] = l499;}l510++;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tConflict bucket pointer [%d,%d]: ""was %d now %d\n"),bkt_ptr,l136,l498[
bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l136],l499));l507++;}}l500 = 
soc_th_alpm_physical_idx(l80,L3_DEFIPm,l499,1);l139 = soc_mem_read(l80,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,l500,l229);l226 = -1;if(SOC_SUCCESS(l139)){
if(l163 == 0){l226 = soc_mem_field32_get(l80,L3_DEFIP_AUX_TABLEm,l229,
BPM_LENGTH0f);l511 = soc_mem_field32_get(l80,L3_DEFIP_AUX_TABLEm,l229,
IP_LENGTH0f);}else{l226 = soc_mem_field32_get(l80,L3_DEFIP_AUX_TABLEm,l229,
BPM_LENGTH1f);l511 = soc_mem_field32_get(l80,L3_DEFIP_AUX_TABLEm,l229,
IP_LENGTH1f);}if(l226>l511){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tAUX Table corrupted\n")));l507++;}else{l510++;}if(l226 == 0&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l53))){continue;}sal_memcpy(&l517,lpm_entry,sizeof(defip_entry_t
));l147(l80,&l517,l226,l107);(void)soc_th_alpm_lpm_vrf_get(l80,lpm_entry,&
vrf_id,&vrf);l504 = l505 = l506 = -1;l139 = _soc_th_alpm_find(l80,l519,l107,&
l517,vrf_id,vrf,l81,&l504,&l505,&l506,FALSE);if(SOC_SUCCESS(l139)){l316 = 0;
l514[l316++] = soc_mem_field32_get(l80,l519,l81,ECMPf);l514[l316++] = 
soc_mem_field32_get(l80,l519,l81,ECMP_PTRf);l514[l316++] = 
soc_mem_field32_get(l80,l519,l81,NEXT_HOP_INDEXf);l514[l316++] = 
soc_mem_field32_get(l80,l519,l81,PRIf);l514[l316++] = soc_mem_field32_get(l80
,l519,l81,RPEf);l514[l316++] = soc_mem_field32_get(l80,l519,l81,DST_DISCARDf)
;l514[l316++] = soc_mem_field32_get(l80,l519,l81,CLASS_IDf);if(sal_memcmp(
l513,l514,sizeof(l513))!= 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tData mismatch: ""lpm_idx %d find_lpm_idx %d "
"find_alpm_idx %d find_bkt_ptr %d\n"),l499,l504,l506,l505));l507++;}else{l510
++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\t_soc_th_alpm_find rv=%d: ""lpm_idx %d find_lpm_idx %d "
"find_alpm_idx %d find_bkt_ptr %d\n"),l139,l499,l504,l506,l505));l507++;}}for
(l217 = 0;l217<l321;l217++){(void)_soc_th_alpm_mem_index(l80,l519,bkt_ptr,
l217,l141,&l420);l139 = soc_mem_read(l80,l519,MEM_BLOCK_ANY,l420,l81);if(
SOC_FAILURE(l139)){continue;}if(!soc_mem_field32_get(l80,l519,l81,VALIDf)){
continue;}if(l136!= soc_mem_field32_get(l80,l519,l81,SUB_BKT_PTRf)){continue;
}(void)l124(l80,l81,l519,l107,vrf_id,ALPM_BKTID(bkt_ptr,l136),0,&l516);l501 = 
l504 = l505 = l506 = -1;l139 = _soc_th_alpm_find(l80,l519,l107,&l516,vrf_id,
vrf,l81,&l504,&l505,&l506,FALSE);if(SOC_SUCCESS(l139)){l501 = 
soc_th_alpm_logical_idx(l80,L3_DEFIPm,l504>>1,1);}if(SOC_FAILURE(l139)||l505
!= ALPM_BKTID(bkt_ptr,l136)||l501!= l499||l504>>1!= l500||l506!= l420){l507++
;l508++;LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\t_soc_th_alpm_find2 rv=%d: ""fl [%d,%d] fa %d fb [%d,%d] "
"l [%d,%d] a %d b [%d,%d]\n"),l139,l504>>1,l501,l506,ALPM_BKT_IDX(l505),
ALPM_BKT_SIDX(l505),l500,l499,l420,bkt_ptr,l136));}}if(l508 == 0){l510++;}if(
!l497){continue;}l139 = l353(l80,lpm_entry,l107?l499:((l499<<1)+l163));if(
SOC_FAILURE(l139)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tLPM Hash check failed: rv %d ""lpm_idx %d\n"),l139,l499));l507++;}else{
l510++;}l139 = soc_th_alpm_bucket_is_assigned(l80,bkt_ptr,vrf,l107,&l220);if(
SOC_FAILURE(l139)||l136>= SOC_TH_MAX_SUB_BUCKETS){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"\tInvalid bucket pointer [%d,%d] "
"detected, in memory %s index %d\n"),bkt_ptr,l136,SOC_MEM_NAME(l80,L3_DEFIPm)
,l499));l507++;}else if(l220 == 0){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tFreed bucket pointer %d ""detected, in memory %s index %d\n"),bkt_ptr,
SOC_MEM_NAME(l80,L3_DEFIPm),l499));l507++;}else{l510++;}l518 = 
_soc_tomahawk_alpm_bkt_view_get(l80,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,
ALPM_BKTID(bkt_ptr,0)));if(l519!= l518){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l80,"\tMismatched alpm view ""in bucket %d, expected %s, was %s\n"),bkt_ptr,
SOC_MEM_NAME(l80,l519),SOC_MEM_NAME(l80,l518)));l507++;}else{l510++;}l139 = 
l149(l80,l107,lpm_entry,prefix,&l154,&l122);if(SOC_SUCCESS(l139)){if(l107){
l262 = VRF_PREFIX_TRIE_IPV6(l80,vrf);}else{l262 = VRF_PREFIX_TRIE_IPV4(l80,
vrf);}l157 = NULL;if(l262){l139 = trie_find_lpm(l262,prefix,l154,&l157);}if(
SOC_SUCCESS(l139)&&l157){l272 = (payload_t*)l157;l512 = ((payload_t*)(l272->
bkt_ptr))->len;if(l512!= l226){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tBPM len mismatch: lpm_idx %d"" alpm_idx %d bpm_len %d trie_bpm_len %d\n"),
l499,l420,l226,l512));l507++;}else{l510++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"\ttrie_find_lpm failed: ""lpm_idx %d alpm_idx %d lpmp %p\n"),
l499,l420,l157));l507++;}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tPrefix creating failed: ""lpm_idx %d alpm_idx %d\n"),l499,l420));l507++;}}
}if(l507){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tL3_DEFIP index:[%d,%d] check failed. ipv6 %d error count %d\n"),l428,l503,
l502,l507));l509 = 1;}else if(l510){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tL3_DEFIP index:[%d,%d] check passed. ipv6 %d success count %d\n"),l428,
l503,l502,l510));}SOC_ALPM_LPM_UNLOCK(l80);soc_cm_sfree(l80,l515);return(l509
?SOC_E_FAIL:SOC_E_NONE);}extern int _soc_th_alpm_128_sanity_check(int l80,int
l399,int l400,int l497,int*l498);int soc_th_alpm_sanity_check(int l80,
soc_mem_t l120,int index,int l497){int l488;int l139 = SOC_E_NONE;int l183 = 
SOC_E_NONE;int l329 = -1,l520 = -1;int l521 = -1,l522 = -1;int*l498 = NULL;if
((l120 == L3_DEFIPm||index == -2)&&soc_mem_index_max(l80,L3_DEFIPm)!= -1){
l329 = soc_mem_index_min(l80,L3_DEFIPm);l520 = soc_mem_index_max(l80,
L3_DEFIPm);if(SOC_URPF_STATUS_GET(l80)){l520>>= 1;}if(index<-2||index>l520){
return SOC_E_PARAM;}if(index>= l329&&index<= l520){l329 = index;l520 = index;
}}if((l120 == L3_DEFIP_PAIR_128m||index == -2)&&soc_mem_index_max(l80,
L3_DEFIP_PAIR_128m)!= -1){l521 = soc_mem_index_min(l80,L3_DEFIP_PAIR_128m);
l522 = soc_mem_index_max(l80,L3_DEFIP_PAIR_128m);if(SOC_URPF_STATUS_GET(l80))
{l522>>= 1;}if(index<-2||index>l522){return SOC_E_PARAM;}if(index>= l521&&
index<= l522){l521 = index;l522 = index;}}if(l520 == -1&&l522 == -1){return
SOC_E_PARAM;}l488 = sizeof(int)*SOC_TH_MAX_ALPM_BUCKETS*
SOC_TH_MAX_SUB_BUCKETS;l498 = sal_alloc(l488,"Bucket ptr array");if(l498 == 
NULL){return SOC_E_MEMORY;}sal_memset(l498,0xff,l488);if(l520!= -1){LOG_CLI((
"Processing ALPM sanity check L3_DEFIP ""from index:%d to index:%d\n",l329,
l520));l139 = _soc_th_alpm_sanity_check(l80,l329,l520,l497,l498);LOG_CLI((
"ALPM sanity check L3_DEFIP %s. \n\n",SOC_SUCCESS(l139)?"passed":"failed"));}
if(l522!= -1){LOG_CLI(("Processing ALPM sanity check L3_DEFIP_PAIR_128 "
"from index:%d to index:%d\n",l521,l522));l183 = 
_soc_th_alpm_128_sanity_check(l80,l521,l522,l497,l498);LOG_CLI((
"ALPM sanity check L3_DEFIP_PAIR_128 %s. \n",SOC_SUCCESS(l183)?"passed":
"failed"));}sal_free(l498);return SOC_SUCCESS(l139)?l183:l139;}int
soc_th_alpm_route_capacity_get(int unit,int l523,int*l524,int*l525){int l139 = 
SOC_E_NONE;int l526,l527,l528;int l529,l530,l531;if(l524 == NULL&&l525 == 
NULL){return SOC_E_PARAM;}l531 = 4/soc_th_get_alpm_banks(unit);l528 = 
SOC_TH_ALPM_MAX_BKTS/2;l530 = soc_mem_index_count(unit,L3_DEFIPm)+
soc_mem_index_count(unit,L3_DEFIP_PAIR_128m)*2;if(l523 == L3_DEFIP_MODE_V4){
l529 = ALPM_IPV4_BKT_COUNT/l531;if(SOC_URPF_STATUS_GET(unit)){if(
soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_COMBINED){l526 = l528*l529;l527 = 
l528*(l529*2/3);}else if(soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_PARALLEL
){l526 = l528*l529/2*2;l527 = l528*(l529/2/3)*2;}else{l526 = l528*l529+l530/2
;l527 = l528*(l529/3)+l530/2;}}else{if(soc_th_alpm_mode_get(unit) == 
SOC_ALPM_MODE_COMBINED){if(l3_alpm_ipv4_double_wide[unit]){l526 = l528*l529*2
;l527 = l528*(l529*2*85/100);}else{l526 = l528*2*l529;l527 = l528*2*(l529*2/3
);}}else if(soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_PARALLEL){l526 = l528
*l529*2;l527 = l528*(l529*2/3)*2;}else{l526 = l528*l529*2+l530;l527 = l528*(
l529*2*2/3)+l530;}}}else if(l523 == L3_DEFIP_MODE_64){l529 = 
ALPM_IPV6_64_BKT_COUNT/l531;if(SOC_URPF_STATUS_GET(unit)){if(
soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_COMBINED){l526 = l528*l529;l527 = 
l528*(l529/3);}else if(soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_PARALLEL){
l526 = l528/2*l529/2*2;l527 = l528/2*(l529/2/3)*2;}else{l526 = l528/2*l529+
l530/2/2;l527 = l528/2*(l529/3)+l530/2/2;}}else{if(soc_th_alpm_mode_get(unit)
== SOC_ALPM_MODE_COMBINED){l526 = l528*l529*2;l527 = l528*(l529*2*2/3);}else
if(soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_PARALLEL){l526 = l528*l529*2;
l527 = l528*(l529/3)*2;}else{l526 = l528*l529*2+l530/2;l527 = l528*(l529*2/3)
+l530/2;}}}else if(l523 == L3_DEFIP_MODE_128){l529 = ALPM_IPV6_128_BKT_COUNT/
l531;if(SOC_URPF_STATUS_GET(unit)){if(soc_th_alpm_mode_get(unit) == 
SOC_ALPM_MODE_COMBINED){l526 = l528/2*l529;l527 = l528/2*(l529/3);}else if(
soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_PARALLEL){l526 = l528/4*l529/2*2;
l527 = l528/4*(l529/2/3)*2;}else{l526 = l528/4*l529+l530/8;l527 = l528/4*(
l529/3)+l530/8;}}else{if(soc_th_alpm_mode_get(unit) == SOC_ALPM_MODE_COMBINED
){l526 = l528*l529*2;l527 = l528*(l529*2/3);}else if(soc_th_alpm_mode_get(
unit) == SOC_ALPM_MODE_PARALLEL){l526 = l528/2*l529*2;l527 = l528/2*(l529/3)*
2;}else{l526 = l528/2*l529*2+l530/4;l527 = l528/2*(l529*2/3)+l530/4;}}}else{
return SOC_E_UNAVAIL;}if(l524!= NULL){*l524 = l526;}if(l525!= NULL){*l525 = 
l527;}return l139;}
#endif /* ALPM_ENABLE */
