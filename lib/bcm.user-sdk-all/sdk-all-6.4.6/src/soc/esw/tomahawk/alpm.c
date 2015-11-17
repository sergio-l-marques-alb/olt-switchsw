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
SOC_MAX_NUM_DEVICES];int l3_alpm_ipv4_double_wide[SOC_MAX_NUM_DEVICES];
alpm_bkt_usg_t*bkt_usage[SOC_MAX_NUM_DEVICES];alpm_bkt_bmp_t*global_bkt_usage
[SOC_MAX_NUM_DEVICES][SOC_TH_MAX_ALPM_VIEWS];int alpm_split_count = 0;int
alpm_merge_count = 0;extern int alpm_128_split_count;static void l79(int l80,
void*l81,int index,l74 l82);static uint16 l83(uint8*l84,int l85);static int
l86(int unit,int l69,int l70,l73**l87);static int l88(l73*l89);static int l90
(l73*l91,l75 l92,l74 entry,int l93,uint16*l94);static int l95(l73*l91,l75 l92
,l74 entry,int l93,uint16 l96,uint16 l97);static int l98(l73*l91,l75 l92,l74
entry,int l93,uint16 l99);static int l100(int,int,void*,int*,int*,int*);
static int l101(int l80);static int l102(int l80);static int l103(int l80,
void*l104,int*index);static int soc_th_alpm_lpm_delete(int l80,void*key_data)
;static int l105(int l80,void*key_data,int l106,int l107,int l108,
defip_aux_scratch_entry_t*l109);static int l110(int l80,void*key_data,void*
l81,int*l111,int*l112,int*l106,int*vrf_id,int*vrf);static int l113(int l80,
void*key_data,void*l81,int*l111);static int l114(int l80);static int l115(int
unit,int l106,void*lpm_entry,void*l116,void*l117,soc_mem_t l118,uint32 l119,
uint32*l120,int l121);static int l122(int unit,void*l116,soc_mem_t l118,int
l106,int vrf,int l123,int index,void*lpm_entry);static int l124(int unit,
uint32*key,int len,int vrf,int l106,defip_entry_t*lpm_entry,int l125,int l126
);static int l127(int l80,int vrf,int l128);static int _soc_th_alpm_move_inval
(int l80,soc_mem_t l118,int l128,alpm_mem_prefix_array_t*l129,int*l97);int
soc_th_alpm_lpm_vrf_get(int unit,void*lpm_entry,int*vrf,int*l130);int
soc_th_alpm_lpm_delete(int l80,void*key_data);extern alpm_vrf_handle_t*
alpm_vrf_handle[SOC_MAX_NUM_DEVICES];extern alpm_pivot_t**tcam_pivot[
SOC_MAX_NUM_DEVICES];extern int soc_tomahawk_alpm_mode_get(int unit);extern
int soc_th_get_alpm_banks(int unit);extern int _soc_th_alpm_128_prefix_create
(int l80,void*entry,uint32*l131,uint32*l112,int*l120);void
soc_th_alpm_lpm_move_bu_upd(int l80,int l121,int l132){alpm_bkt_usg_t*l133;
int bkt_ptr = ALPM_BKT_IDX(l121);int l134 = ALPM_BKT_SIDX(l121);l133 = &
bkt_usage[l80][bkt_ptr];l133->pivots[l134] = l132;}int soc_th_alpm_mode_get(
int l80){int l135;l135 = soc_tomahawk_alpm_mode_get(l80);switch(l135){case 1:
return SOC_ALPM_MODE_PARALLEL;case 3:return SOC_ALPM_MODE_TCAM_ALPM;case 2:
default:return SOC_ALPM_MODE_COMBINED;}return SOC_ALPM_MODE_COMBINED;}static
int l136(int l80,int l106,void*entry,int*l93){int l137;uint32 l138;if(l106){
l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l27));if((l137 = _ipmask2pfx(l138,l93))<0){return(l137);}
l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l28));if(*l93){if(l138!= 0xffffffff){return(SOC_E_PARAM);
}*l93+= 32;}else{if((l137 = _ipmask2pfx(l138,l93))<0){return(l137);}}}else{
l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l27));if((l137 = _ipmask2pfx(l138,l93))<0){return(l137);}
}return SOC_E_NONE;}void soc_th_alpm_bank_db_type_get(int l80,int vrf,uint32*
l139,uint32*l107){if(vrf == SOC_VRF_MAX(l80)+1){if(l107){*l107 = 0;}if(
soc_th_get_alpm_banks(l80)<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE
(l80,*l139);}else{SOC_ALPM_TH_GET_GLOBAL_BANK_DISABLE(l80,*l139);}}else{if(
l107){*l107 = 2;}if(soc_th_get_alpm_banks(l80)<= 2){
SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(l80,*l139);}else{
SOC_ALPM_TH_GET_VRF_BANK_DISABLE(l80,*l139);}}}int _soc_th_alpm_rpf_entry(int
l80,int l140){int l141;int l142 = soc_th_get_alpm_banks(l80)/2;l141 = (l140>>
l142)&SOC_TH_ALPM_BKT_MASK;l141+= SOC_TH_ALPM_BUCKET_COUNT(l80);return(l140&~
(SOC_TH_ALPM_BKT_MASK<<l142))|(l141<<l142);}int soc_th_alpm_physical_idx(int
l80,soc_mem_t l118,int index,int l143){int l144 = index&1;if(l143){return
soc_trident2_l3_defip_index_map(l80,l118,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_map(l80,l118,index);index<<= 1;index|= l144;
return index;}int soc_th_alpm_logical_idx(int l80,soc_mem_t l118,int index,
int l143){int l144 = index&1;if(l143){return soc_trident2_l3_defip_index_remap
(l80,l118,index);}index>>= 1;index = soc_trident2_l3_defip_index_remap(l80,
l118,index);index<<= 1;index|= l144;return index;}static int l145(int l80,int
l106,void*entry,uint32*prefix,uint32*l112,int*l120){int l146,l147;int l93 = 0
;int l137 = SOC_E_NONE;uint32 l148,l144;prefix[0] = prefix[1] = prefix[2] = 
prefix[3] = prefix[4] = 0;l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l25));l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l27));prefix[1] = l146;l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l26));l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l28));prefix[0] = l146;if(l106){prefix[4] = prefix[1];prefix[3] = 
prefix[0];prefix[1] = prefix[0] = 0;l147 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l27));if((l137 = 
_ipmask2pfx(l147,&l93))<0){return(l137);}l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l28));if(l93){if(l147!= 0xffffffff){return(SOC_E_PARAM);}l93+= 32;}
else{if((l137 = _ipmask2pfx(l147,&l93))<0){return(l137);}}l148 = 64-l93;if(
l148<32){prefix[4]>>= l148;l144 = (((32-l148) == 32)?0:(prefix[3])<<(32-l148)
);prefix[3]>>= l148;prefix[4]|= l144;}else{prefix[4] = (((l148-32) == 32)?0:(
prefix[3])>>(l148-32));prefix[3] = 0;}}else{l147 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l27));if((l137 = _ipmask2pfx(l147,&l93))<0){return(l137);}prefix[1] = 
(((32-l93) == 32)?0:(prefix[1])>>(32-l93));prefix[0] = 0;}*l112 = l93;*l120 = 
(prefix[0] == 0)&&(prefix[1] == 0)&&(l93 == 0);return SOC_E_NONE;}int
_soc_th_alpm_find_in_bkt(int l80,soc_mem_t l118,int bktid,int l139,uint32*l81
,void*alpm_data,int*l94,int l128){int l137;l137 = soc_mem_alpm_lookup(l80,
l118,ALPM_BKT_IDX(bktid),MEM_BLOCK_ANY,l139,l81,alpm_data,l94);if(SOC_SUCCESS
(l137)){return l137;}if(SOC_TH_ALPM_SCALE_CHECK(l80,l128)){return
soc_mem_alpm_lookup(l80,l118,ALPM_BKT_IDX(bktid)+1,MEM_BLOCK_ANY,l139,l81,
alpm_data,l94);}return l137;}static int l149(int l80,uint32*prefix,uint32 l150
,int l128,int vrf,int*l151,int*tcam_index,int*bktid){int l137 = SOC_E_NONE;
trie_t*l152;trie_node_t*l153 = NULL;alpm_pivot_t*pivot_pyld;if(l128){l152 = 
VRF_PIVOT_TRIE_IPV6(l80,vrf);}else{l152 = VRF_PIVOT_TRIE_IPV4(l80,vrf);}l137 = 
trie_find_lpm(l152,prefix,l150,&l153);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Pivot find failed\n")));return l137;}
pivot_pyld = (alpm_pivot_t*)l153;*l151 = 1;*tcam_index = PIVOT_TCAM_INDEX(
pivot_pyld);*bktid = PIVOT_BUCKET_INDEX(pivot_pyld);return SOC_E_NONE;}static
int _soc_th_alpm_find(int l80,soc_mem_t l118,int l128,void*key_data,int vrf_id
,int vrf,void*alpm_data,int*tcam_index,int*bktid,int*l111,int l154){uint32 l81
[SOC_MAX_MEM_FIELD_WORDS];int l94;uint32 l107,l139;int l137 = SOC_E_NONE;int
l151 = 0;if(vrf_id == 0){if(soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_PARALLEL||soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_TCAM_ALPM)
{return SOC_E_PARAM;}}soc_th_alpm_bank_db_type_get(l80,vrf,&l139,&l107);if(!(
ALPM_PREFIX_IN_TCAM(l80,vrf_id))){if(l154){uint32 prefix[5],l150;int l120 = 0
;l137 = l145(l80,l128,key_data,prefix,&l150,&l120);if(SOC_FAILURE(l137)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: prefix create failed\n")));return l137;}
SOC_IF_ERROR_RETURN(l149(l80,prefix,l150,l128,vrf,&l151,tcam_index,bktid));}
else{defip_aux_scratch_entry_t l109;sal_memset(&l109,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l105(l80,key_data,l128,l107,0
,&l109));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l80,PREFIX_LOOKUP,&l109,TRUE
,&l151,tcam_index,bktid));}if(l151){l115(l80,l128,key_data,l81,0,l118,0,0,*
bktid);l137 = _soc_th_alpm_find_in_bkt(l80,l118,*bktid,l139,l81,alpm_data,&
l94,l128);if(SOC_SUCCESS(l137)){*l111 = l94;}}else{l137 = SOC_E_NOT_FOUND;}}
return l137;}static int l155(int l80,int l128,void*key_data,void*alpm_data,
void*alpm_sip_data,soc_mem_t l118,int l94){defip_aux_scratch_entry_t l109;int
vrf_id,vrf;int bktid;uint32 l107,l139;int l137 = SOC_E_NONE;int l151 = 0,l144
= 0;int tcam_index,index;uint32 l156[SOC_MAX_MEM_FIELD_WORDS];
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf));
soc_th_alpm_bank_db_type_get(l80,vrf,&l139,&l107);if(soc_th_alpm_mode_get(l80
) == SOC_ALPM_MODE_COMBINED||soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_TCAM_ALPM){l107 = 2;}if(!(ALPM_PREFIX_IN_TCAM(l80,vrf_id))){
sal_memset(&l109,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(
l105(l80,key_data,l128,l107,0,&l109));SOC_ALPM_LPM_LOCK(l80);l137 = 
_soc_th_alpm_find(l80,l118,l128,key_data,vrf_id,vrf,l156,&tcam_index,&bktid,&
index,TRUE);SOC_ALPM_LPM_UNLOCK(l80);SOC_IF_ERROR_RETURN(l137);
soc_mem_field32_set(l80,l118,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));
SOC_IF_ERROR_RETURN(soc_mem_write(l80,l118,MEM_BLOCK_ANY,ALPM_ENT_INDEX(l94),
alpm_data));if(SOC_URPF_STATUS_GET(l80)){soc_mem_field32_set(l80,l118,
alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));SOC_IF_ERROR_RETURN(
soc_mem_write(l80,l118,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,
ALPM_ENT_INDEX(l94)),alpm_sip_data));if(l137!= SOC_E_NONE){return SOC_E_FAIL;
}}l144 = soc_mem_field32_get(l80,L3_DEFIP_AUX_SCRATCHm,&l109,IP_LENGTHf);
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,REPLACE_LENf,l144);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l80,DELETE_PROPAGATE,&l109,TRUE,&l151
,&tcam_index,&bktid));if(SOC_URPF_STATUS_GET(l80)){if(l144 == 0){
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,RPEf,1);}else{
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,RPEf,0);}l144 = 
soc_mem_field32_get(l80,L3_DEFIP_AUX_SCRATCHm,&l109,DB_TYPEf);l144+= 1;
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,DB_TYPEf,l144);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l80,DELETE_PROPAGATE,&l109,TRUE,&l151
,&tcam_index,&bktid));}}return l137;}int soc_th_alpm_update_hit_bits(int l80,
int count,int*l157,int*l158){int l137 = SOC_E_NONE;int l159,l160,l161,l162,
l163;int index;defip_aux_hitbit_update_entry_t l164;soc_mem_t l118 = 
L3_DEFIP_AUX_HITBIT_UPDATEm;int l165 = soc_mem_index_count(l80,l118);l161 = (
count+l165-1)/l165;for(l160 = 0;l160<l161;l160++){l162 = l160*l165;l163 = (
count>(l160+1)*l165)?(l160+1)*l165:count;l163 = l163-1;for(l159 = l162;l159<= 
l163;l159++){index = l159%l165;if(l157[l159]<= 0){continue;}sal_memset(&l164,
0,sizeof(l164));soc_mem_field32_set(l80,l118,&l164,HITBIT_PTRf,l157[l159]);
soc_mem_field32_set(l80,l118,&l164,REPLACEMENT_PTRf,l158[l159]);
soc_mem_field32_set(l80,l118,&l164,VALIDf,1);SOC_IF_ERROR_RETURN(
soc_mem_write(l80,l118,MEM_BLOCK_ANY,index,&l164));}SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l80,HITBIT_REPLACE,NULL,FALSE,NULL,NULL,NULL));}if(!
SOC_URPF_STATUS_GET(l80)){return l137;}for(l160 = 0;l160<l161;l160++){l162 = 
l160*l165;l163 = (count>(l160+1)*l165)?(l160+1)*l165:count;l163 = l163-1;for(
l159 = l162;l159<= l163;l159++){index = l159%l165;if(l157[l159]<= 0){continue
;}sal_memset(&l164,0,sizeof(l164));soc_mem_field32_set(l80,l118,&l164,
HITBIT_PTRf,_soc_th_alpm_rpf_entry(l80,l157[l159]));soc_mem_field32_set(l80,
l118,&l164,REPLACEMENT_PTRf,_soc_th_alpm_rpf_entry(l80,l158[l159]));
soc_mem_field32_set(l80,l118,&l164,VALIDf,1);SOC_IF_ERROR_RETURN(
soc_mem_write(l80,l118,MEM_BLOCK_ANY,index,&l164));}SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l80,HITBIT_REPLACE,NULL,FALSE,NULL,NULL,NULL));}return
l137;}int _soc_th_alpm_mem_prefix_array_cb(trie_node_t*node,void*l166){
alpm_mem_prefix_array_t*l167 = (alpm_mem_prefix_array_t*)l166;if(node->type == 
PAYLOAD){l167->prefix[l167->count] = (payload_t*)node;l167->count++;}return
SOC_E_NONE;}int _soc_th_alpm_bkt_entry_cnt(int l80,int l128){int l168 = 0;
switch(l128){case L3_DEFIP_MODE_V4:l168 = ALPM_IPV4_BKT_COUNT;break;case
L3_DEFIP_MODE_64:l168 = ALPM_IPV6_64_BKT_COUNT;break;case L3_DEFIP_MODE_128:
l168 = ALPM_IPV6_128_BKT_COUNT;break;default:l168 = ALPM_IPV4_BKT_COUNT;break
;}if(SOC_TH_ALPM_SCALE_CHECK(l80,l128)){l168<<= 1;}if(soc_th_get_alpm_banks(
l80)<= 2){l168>>= 1;}if(soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_PARALLEL&&
SOC_URPF_STATUS_GET(l80)){l168>>= 1;}return l168;}static int l169(int l80,int
l2,alpm_bkt_bmp_t*l170){int l159;for(l159 = l2+1;l159<SOC_TH_MAX_ALPM_BUCKETS
;l159++){if(SHR_BITGET(l170->bkt_bmp,l159)){return l159;}}return-1;}static int
l171(int l80,int l128,int l132,int bktid){int l137 = SOC_E_NONE;int l172;
soc_mem_t l173 = L3_DEFIPm;defip_entry_t lpm_entry;int l174,l175;l174 = 
ALPM_BKT_IDX(bktid);l175 = ALPM_BKT_SIDX(bktid);l172 = 
soc_th_alpm_logical_idx(l80,l173,l132>>1,1);l137 = soc_mem_read(l80,l173,
MEM_BLOCK_ANY,l172,&lpm_entry);SOC_IF_ERROR_RETURN(l137);if((!l128)&&(l132&1)
){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(&
lpm_entry),(l67[(l80)]->l50),(l174));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(&lpm_entry),(l67[(l80)]->l52),(l175));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(&lpm_entry)
,(l67[(l80)]->l49),(l174));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(&lpm_entry),(l67[(l80)]->l51),(l175));}l137 = soc_mem_write(
l80,l173,MEM_BLOCK_ANY,l172,&lpm_entry);SOC_IF_ERROR_RETURN(l137);if(
SOC_URPF_STATUS_GET(l80)){int l176 = soc_th_alpm_logical_idx(l80,l173,l132>>1
,1)+(soc_mem_index_count(l80,l173)>>1);l137 = READ_L3_DEFIPm(l80,
MEM_BLOCK_ANY,l176,&lpm_entry);SOC_IF_ERROR_RETURN(l137);if((!l128)&&(l176&1)
){soc_mem_field32_set(l80,l173,&lpm_entry,ALG_BKT_PTR1f,l174+
SOC_TH_ALPM_BUCKET_COUNT(l80));soc_mem_field32_set(l80,l173,&lpm_entry,
ALG_SUB_BKT_PTR1f,l175);}else{soc_mem_field32_set(l80,l173,&lpm_entry,
ALG_BKT_PTR0f,l174+SOC_TH_ALPM_BUCKET_COUNT(l80));soc_mem_field32_set(l80,
l173,&lpm_entry,ALG_SUB_BKT_PTR0f,l175);}l137 = WRITE_L3_DEFIPm(l80,
MEM_BLOCK_ANY,l176,&lpm_entry);SOC_IF_ERROR_RETURN(l137);}return l137;}static
int _soc_th_alpm_move_trie(int l80,int l128,int l177,int l178){int*l97 = NULL
,*l96 = NULL;int l137 = SOC_E_NONE,l179,l159,l180;int l94,l132;uint32 l139 = 
0;soc_mem_t l118;void*l181,*l182;trie_t*trie = NULL;payload_t*l93 = NULL;
alpm_mem_prefix_array_t*l129 = NULL;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];
defip_alpm_ipv4_entry_t l183,l184;defip_alpm_ipv6_64_entry_t l185,l186;int
l187,l188,l189,l190;l180 = sizeof(int)*MAX_PREFIX_PER_BUCKET;l97 = sal_alloc(
l180,"new_index_move");l96 = sal_alloc(l180,"old_index_move");l129 = 
sal_alloc(sizeof(alpm_mem_prefix_array_t),"prefix_array");if(l97 == NULL||l96
== NULL||l129 == NULL){l137 = SOC_E_MEMORY;goto l191;}l187 = ALPM_BKT_IDX(
l177);l189 = ALPM_BKT_SIDX(l177);l188 = ALPM_BKT_IDX(l178);l190 = 
ALPM_BKT_SIDX(l178);l118 = (l128)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;
l181 = ((l128)?((uint32*)&(l185)):((uint32*)&(l183)));l182 = ((l128)?((uint32
*)&(l186)):((uint32*)&(l184)));l132 = SOC_ALPM_BS_BKT_USAGE_PIVOT(l80,l177);
trie = PIVOT_BUCKET_TRIE(ALPM_TCAM_PIVOT(l80,l132));
soc_th_alpm_bank_db_type_get(l80,SOC_ALPM_BS_BKT_USAGE_VRF(l80,l177),&l139,
NULL);sal_memset(l129,0,sizeof(*l129));l137 = trie_traverse(trie,
_soc_th_alpm_mem_prefix_array_cb,l129,_TRIE_INORDER_TRAVERSE);do{if((l137)<0)
{goto l191;}}while(0);sal_memset(l97,-1,l180);sal_memset(l96,-1,l180);for(
l159 = 0;l159<l129->count;l159++){l93 = l129->prefix[l159];if(l93->index>0){
l137 = soc_mem_read(l80,l118,MEM_BLOCK_ANY,l93->index,l181);if(SOC_FAILURE(
l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_mem_read index %d failed\n"),l93->index));goto l192;}soc_mem_field32_set
(l80,l118,l181,SUB_BKT_PTRf,l190);if(SOC_URPF_STATUS_GET(l80)){l137 = 
soc_mem_read(l80,l118,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l93->index),
l182);if(SOC_FAILURE(l137)){goto l192;}soc_mem_field32_set(l80,l118,l182,
SUB_BKT_PTRf,l190);}l137 = _soc_th_alpm_insert_in_bkt(l80,l118,l178,l139,l181
,l81,&l94,l128);if(SOC_SUCCESS(l137)){if(SOC_URPF_STATUS_GET(l80)){l137 = 
soc_mem_write(l80,l118,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),l182);if
(SOC_FAILURE(l137)){goto l192;}}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l80,"insert to bucket %d failed\n"),l178));goto l192;}l97[l159] = l94;l96[
l159] = l93->index;}}l137 = l171(l80,l128,l132,l178);if(SOC_FAILURE(l137)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_th_alpm_move_relink failed, pivot %d bkt %d\n"),l132,l178));goto l192;}
l137 = _soc_th_alpm_move_inval(l80,l118,l128,l129,l97);if(SOC_FAILURE(l137)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_th_alpm_move_inval failed\n")
));goto l192;}l137 = soc_th_alpm_update_hit_bits(l80,l129->count,l96,l97);if(
SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l129->count));l137 = 
SOC_E_NONE;}PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l80,l132)) = ALPM_BKTID(l188,
l190);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l80,l128,l187);
SOC_ALPM_BS_BKT_USAGE_SB_DEL(l80,l187,l189,TRUE,l129->count);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l80,l128,l187);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l80,l128,l188);SOC_ALPM_BS_BKT_USAGE_SB_ADD(
l80,l188,l190,l132,-1,l129->count);SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l80,l128,
l188);l192:if(SOC_FAILURE(l137)){l179 = l171(l80,l128,l132,l177);do{if((l179)
<0){goto l191;}}while(0);for(l159 = 0;l159<l129->count;l159++){if(l97[l159] == 
-1){continue;}l179 = soc_mem_write(l80,l118,MEM_BLOCK_ANY,l97[l159],
soc_mem_entry_null(l80,l118));do{if((l179)<0){goto l191;}}while(0);if(
SOC_URPF_STATUS_GET(l80)){l179 = soc_mem_write(l80,l118,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,l97[l159]),soc_mem_entry_null(l80,l118));do{if((
l179)<0){goto l191;}}while(0);}}}l191:if(l97!= NULL){sal_free(l97);}if(l96!= 
NULL){sal_free(l96);}if(l129!= NULL){sal_free(l129);}return l137;}int
soc_th_alpm_bs_merge(int l80,int l128,int l193,int l194){int l137 = 
SOC_E_NONE;alpm_bkt_usg_t*l195,*l196;int l197,l198;l195 = &bkt_usage[l80][
l193];l196 = &bkt_usage[l80][l194];for(l197 = 0;l197<SOC_TH_MAX_SUB_BUCKETS;
l197++){if(!(l195->sub_bkts&(1<<l197))){continue;}for(l198 = 0;l198<
SOC_TH_MAX_SUB_BUCKETS;l198++){if((l196->sub_bkts&(1<<l198))!= 0){continue;}
if(l128 == L3_DEFIP_MODE_128){l137 = _soc_th_alpm_128_move_trie(l80,l128,
ALPM_BKTID(l193,l197),ALPM_BKTID(l194,l198));}else{l137 = 
_soc_th_alpm_move_trie(l80,l128,ALPM_BKTID(l193,l197),ALPM_BKTID(l194,l198));
}SOC_IF_ERROR_RETURN(l137);break;}}alpm_merge_count++;return SOC_E_NONE;}
static int l199(int l80,int l200,int l201){if(l200 == l201){return 1;}if(l200
== (SOC_VRF_MAX(l80)+1)||l201 == (SOC_VRF_MAX(l80)+1)){return 0;}return 1;}
static int l202(int l80,int*bktid,int l128){int l203,l204,l168;int l193,l194,
l205;alpm_bkt_bmp_t*l206,*l207;alpm_bkt_usg_t*l195,*l196;for(l205 = 0;l205<
SOC_TH_MAX_ALPM_VIEWS;l205++){l195 = l196 = NULL;l168 = 
_soc_th_alpm_bkt_entry_cnt(l80,l205);for(l203 = 1;l203<= (l168/2);l203++){
l206 = &global_bkt_usage[l80][l205][l203];if(l206->bkt_count<1){continue;}
l193 = l169(l80,-1,l206);if(l193 == ALPM_BKT_IDX(*bktid)){l193 = l169(l80,
l193,l206);if(l193 == -1){continue;}}l195 = &bkt_usage[l80][l193];l194 = l169
(l80,l193,l206);while(l194!= -1){l196 = &bkt_usage[l80][l194];if(!l199(l80,
l195->vrf_type[0],l196->vrf_type[0])){l194 = l169(l80,l194,l206);continue;}if
(_shr_popcount((uint32)l195->sub_bkts)+_shr_popcount((uint32)l196->sub_bkts)
<= SOC_TH_MAX_SUB_BUCKETS){break;}l194 = l169(l80,l194,l206);}if(l194!= -1){
SOC_IF_ERROR_RETURN(soc_th_alpm_bs_merge(l80,l205,l193,l194));*bktid = 
ALPM_BKTID(l193,0);return SOC_E_NONE;}for(l204 = l203+1;l204<
SOC_TH_MAX_BUCKET_ENTRIES;l204++){l207 = &global_bkt_usage[l80][l205][l204];
if(l207->bkt_count<= 0){continue;}if((l203+l204)>l168){break;}l194 = l169(l80
,-1,l207);while(l194!= -1){l196 = &bkt_usage[l80][l194];if((l194 == l193)||(!
l199(l80,l195->vrf_type[0],l196->vrf_type[0]))){l194 = l169(l80,l194,l207);
continue;}if((_shr_popcount((uint32)l195->sub_bkts)+_shr_popcount((uint32)
l196->sub_bkts))<= SOC_TH_MAX_SUB_BUCKETS){break;}l194 = l169(l80,l194,l207);
}if(l194!= -1){SOC_IF_ERROR_RETURN(soc_th_alpm_bs_merge(l80,l205,l193,l194));
*bktid = ALPM_BKTID(l193,0);return SOC_E_NONE;}}}}l168 = 
_soc_th_alpm_bkt_entry_cnt(l80,l128);for(l203 = 1;l203<l168;l203++){l206 = &
global_bkt_usage[l80][l128][l203];if(l206->bkt_count<1){continue;}l193 = l169
(l80,-1,l206);if(l193 == ALPM_BKT_IDX(*bktid)){l193 = l169(l80,l193,l206);if(
l193 == -1){continue;}}l195 = &bkt_usage[l80][l193];for(l204 = 0;l204<
SOC_TH_MAX_SUB_BUCKETS;l204++){if(l195->sub_bkts&(1<<l204)){continue;}*bktid = 
ALPM_BKTID(l193,l204);return SOC_E_NONE;}}return SOC_E_FULL;}int
soc_th_alpm_bs_alloc(int l80,int*bktid,int vrf,int l128){int l137 = 
SOC_E_NONE;l137 = soc_th_alpm_bucket_assign(l80,bktid,vrf,l128);if(l137!= 
SOC_E_FULL){*bktid = ALPM_BKTID(*bktid,0);return l137;}l137 = l202(l80,bktid,
l128);return l137;}int soc_th_alpm_bs_free(int l80,int bktid,int vrf,int l128
){int l137 = SOC_E_NONE;alpm_bkt_usg_t*l133;int l208,l209;l208 = ALPM_BKT_IDX
(bktid);l209 = ALPM_BKT_SIDX(bktid);l133 = &bkt_usage[l80][l208];l133->
sub_bkts&= ~(1<<l209);l133->pivots[l209] = 0;if(l133->count == 0){l133->
vrf_type[l209] = 0;l137 = soc_th_alpm_bucket_release(l80,l208,vrf,l128);}
return l137;}int soc_th_alpm_bu_upd(int l80,int l123,int tcam_index,int l210,
int l128,int l211){int l208,l209;l208 = ALPM_BKT_IDX(l123);l209 = 
ALPM_BKT_SIDX(l123);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l80,l128,l208);if(l211>0
){SOC_ALPM_BS_BKT_USAGE_SB_ADD(l80,l208,l209,tcam_index,l210,l211);}else if(
l211<0){SOC_ALPM_BS_BKT_USAGE_SB_DEL(l80,l208,l209,l210,-l211);}
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l80,l128,l208);return SOC_E_NONE;}int
soc_th_alpm_bs_dump_brief(int l80,const char*l93){int l159,l212,l213,l141,
l214 = 0;int l215 = 1,l216 = 0;alpm_bkt_bmp_t*l217 = NULL;alpm_bkt_usg_t*l133
= NULL;int*l218;char*l219[] = {"IPv4","IPv6-64","IPv6-128"};l218 = sal_alloc(
sizeof(int)*16384,"pivot");if(l218 == NULL){return SOC_E_MEMORY;}sal_memset(
l218,0,sizeof(int)*16384);for(l212 = 0;l212<SOC_TH_MAX_ALPM_VIEWS;l212++){
LOG_CLI((BSL_META_U(l80,"\n[- %8s -]\n"),l219[l212]));LOG_CLI((BSL_META_U(l80
,"=================================================\n")));LOG_CLI((BSL_META_U
(l80," %5s | %5s %10s\n"),"entry","count","1st-bktbmp"));LOG_CLI((BSL_META_U(
l80,"=================================================\n")));l141 = 0;for(
l213 = 0;l213<SOC_TH_MAX_BUCKET_ENTRIES;l213++){l217 = &global_bkt_usage[l80]
[l212][l213];if(l217->bkt_count == 0){continue;}l141++;LOG_CLI((BSL_META_U(
l80," %5d   %5d"),l213,l217->bkt_count));for(l159 = 0;l159<
SOC_TH_MAX_ALPM_BUCKETS;l159++){if(SHR_BITGET(l217->bkt_bmp,l159)){LOG_CLI((
BSL_META_U(l80," 0x%08x\n"),l217->bkt_bmp[l159/SHR_BITWID]));break;}}}if(l141
== 0){LOG_CLI((BSL_META_U(l80,"- None - \n")));}}l213 = 0;LOG_CLI((BSL_META_U
(l80,"%5s | %s\n"),"bkt","(sub-bkt-idx, tcam-idx)"));LOG_CLI((BSL_META_U(l80,
"=================================================\n")));for(l141 = 0;l141<
SOC_TH_MAX_ALPM_BUCKETS;l141++){l133 = &bkt_usage[l80][l141];if(l133->count == 
0){continue;}l213++;l214 = 0;LOG_CLI((BSL_META_U(l80,"%5d | "),l141));for(
l159 = 0;l159<4;l159++){if(l133->sub_bkts&(1<<l159)){LOG_CLI((BSL_META_U(l80,
"(%d, %5d) "),l159,l133->pivots[l159]));if(l218[l133->pivots[l159]]!= 0){l214
= l159+1;}else{l218[l133->pivots[l159]] = l141;}}}LOG_CLI((BSL_META_U(l80,
"\n")));if(l214){LOG_CLI((BSL_META_U(l80,
"Error: multi-buckets were linked to pivot %d, prev %d, curr %d\n"),l133->
pivots[l214-1],l218[l133->pivots[l214-1]],l141));}}if(l213 == 0){LOG_CLI((
BSL_META_U(l80,"- None - \n")));}LOG_CLI((BSL_META_U(l80,"\n")));if(
SOC_TH_ALPM_SCALE_CHECK(l80,1)){l215 = 2;}l141 = 0;for(l159 = 0;l159<
SOC_TH_ALPM_BUCKET_COUNT(l80);l159+= l215){SHR_BITTEST_RANGE(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80),l159,l215,l216);if(l216){l141++;}}LOG_CLI((
BSL_META_U(l80,"VRF Route buckets: %5d\n"),l141));if(soc_th_alpm_mode_get(l80
) == SOC_ALPM_MODE_PARALLEL&&SOC_URPF_STATUS_GET(l80)){l141 = 0;for(l159 = 0;
l159<SOC_TH_ALPM_BUCKET_COUNT(l80);l159+= l215){SHR_BITTEST_RANGE(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l80),l159,l215,l216);if(l216){l141++;}}LOG_CLI((
BSL_META_U(l80,"Global Route buckets: %5d\n"),l141));}sal_free(l218);return
SOC_E_NONE;}static int l220(int l80,int l221,int l128,int l222){int l137,l144
,index;defip_aux_table_entry_t entry;index = l221>>(l128?0:1);l137 = 
soc_mem_read(l80,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l137);if(l128){soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l222);soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l222);l144 = soc_mem_field32_get(l80,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l221&1){soc_mem_field32_set(
l80,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l222);l144 = soc_mem_field32_get(
l80,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l222);l144 = soc_mem_field32_get(l80,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l137 = soc_mem_write(l80,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l137);if(
SOC_URPF_STATUS_GET(l80)){l144++;if(l128){soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l222);soc_mem_field32_set(l80,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l222);}else{if(l221&1){
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l222);}else{
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l222);}}
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l144);
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l144);index+= (2
*soc_mem_index_count(l80,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l80,
L3_DEFIPm))/2;l137 = soc_mem_write(l80,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
index,&entry);}return l137;}static int l223(int l80,int l224,void*entry,void*
l225,int l226){uint32 l144,l147,l128,l107,l227 = 0;soc_mem_t l228 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l229;int l137 = SOC_E_NONE,l93,l230,vrf,
l201;SOC_IF_ERROR_RETURN(soc_mem_read(l80,l228,MEM_BLOCK_ANY,l224,l225));l144
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(
l67[(l80)]->l41));soc_mem_field32_set(l80,l228,l225,VRF0f,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l42));soc_mem_field32_set(l80,l228,l225,VRF1f,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l29));soc_mem_field32_set(l80,l228,l225,MODE0f,l144);l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l30));soc_mem_field32_set(l80,l228,l225,MODE1f,l144);l128 = l144;
l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l39));soc_mem_field32_set(l80,l228,l225,VALID0f,l144);
l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l40));soc_mem_field32_set(l80,l228,l225,VALID1f,l144);
l144 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l27));if((l137 = _ipmask2pfx(l144,&l93))<0){return l137;}
l147 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
entry),(l67[(l80)]->l28));if((l137 = _ipmask2pfx(l147,&l230))<0){return l137;
}if(l128){soc_mem_field32_set(l80,l228,l225,IP_LENGTH0f,l93+l230);
soc_mem_field32_set(l80,l228,l225,IP_LENGTH1f,l93+l230);}else{
soc_mem_field32_set(l80,l228,l225,IP_LENGTH0f,l93);soc_mem_field32_set(l80,
l228,l225,IP_LENGTH1f,l230);}l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l25));soc_mem_field32_set(
l80,l228,l225,IP_ADDR0f,l144);l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l26));soc_mem_field32_set(
l80,l228,l225,IP_ADDR1f,l144);l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l63));soc_mem_field32_set(
l80,l228,l225,ENTRY_TYPE0f,l144);l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l64));soc_mem_field32_set(
l80,l228,l225,ENTRY_TYPE1f,l144);if(!l128){sal_memcpy(&l229,entry,sizeof(l229
));l137 = soc_th_alpm_lpm_vrf_get(l80,(void*)&l229,&vrf,&l93);
SOC_IF_ERROR_RETURN(l137);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_ip4entry1_to_0(
l80,&l229,&l229,PRESERVE_HIT));l137 = soc_th_alpm_lpm_vrf_get(l80,(void*)&
l229,&l201,&l93);SOC_IF_ERROR_RETURN(l137);}else{l137 = 
soc_th_alpm_lpm_vrf_get(l80,entry,&vrf,&l93);}if(SOC_URPF_STATUS_GET(l80)){if
(l226>= (soc_mem_index_count(l80,L3_DEFIPm)>>1)){l227 = 1;}}switch(vrf){case
SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l80,l228,l225,VALID0f,0);l107 = 0;
break;case SOC_L3_VRF_GLOBAL:l107 = l227?1:0;break;default:l107 = l227?3:2;
break;}soc_mem_field32_set(l80,l228,l225,DB_TYPE0f,l107);if(!l128){switch(
l201){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l80,l228,l225,VALID1f,0);
l107 = 0;break;case SOC_L3_VRF_GLOBAL:l107 = l227?1:0;break;default:l107 = 
l227?3:2;break;}soc_mem_field32_set(l80,l228,l225,DB_TYPE1f,l107);}else{if(
vrf == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l80,l228,l225,VALID1f,0);}
soc_mem_field32_set(l80,l228,l225,DB_TYPE1f,l107);}if(l227){l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l49));if(l144){l144+= SOC_TH_ALPM_BUCKET_COUNT(l80);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l49),(l144));}l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l51));if(l144){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l51),(l144));}if(!l128){l144 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l50));if(l144){l144+= 
SOC_TH_ALPM_BUCKET_COUNT(l80);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67[(l80)]->l50),(l144));}l144 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l52));if(l144){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(entry),(l67[(l80)]->l52),(l144));}}}return SOC_E_NONE;}static int
l231(int l80,int l232,int index,int l233,void*entry){defip_aux_table_entry_t
l225;l233 = soc_th_alpm_physical_idx(l80,L3_DEFIPm,l233,1);
SOC_IF_ERROR_RETURN(l223(l80,l233,entry,(void*)&l225,index));
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l80,MEM_BLOCK_ANY,index,entry));index = 
soc_th_alpm_physical_idx(l80,L3_DEFIPm,index,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l80,MEM_BLOCK_ANY,index,&l225));return SOC_E_NONE;}
int _soc_th_alpm_insert_in_bkt(int l80,soc_mem_t l118,int bktid,int l139,void
*alpm_data,uint32*l81,int*l94,int l128){int l137;l137 = soc_mem_alpm_insert(
l80,l118,ALPM_BKT_IDX(bktid),MEM_BLOCK_ANY,l139,alpm_data,l81,l94);if(l137 == 
SOC_E_FULL){if(SOC_TH_ALPM_SCALE_CHECK(l80,l128)){l137 = soc_mem_alpm_insert(
l80,l118,ALPM_BKT_IDX(bktid)+1,MEM_BLOCK_ANY,l139,alpm_data,l81,l94);}}return
l137;}int _soc_th_alpm_mem_index(int l80,soc_mem_t l118,int bucket_index,int
l234,uint32 l139,int*l235){int l159,l174 = 0;int l236[4] = {0};int l237 = 0;
int l238 = 0;int l239;int l240 = 6;int l241;int l242;int l243;int l244 = 0;
switch(l118){case L3_DEFIP_ALPM_IPV6_64m:l240 = 4;break;case
L3_DEFIP_ALPM_IPV6_128m:l240 = 2;break;default:break;}if(
SOC_TH_ALPM_SCALE_CHECK(l80,(l118!= L3_DEFIP_ALPM_IPV4m))){if(l234>= 
ALPM_RAW_BKT_COUNT*l240){bucket_index++;l234-= ALPM_RAW_BKT_COUNT*l240;}}l241
= 4;l242 = 15;l243 = 2;if(soc_th_get_alpm_banks(l80)<= 2){l241 = 2;l242 = 14;
l243 = 1;}l244 = ((1<<l241)-1);l239 = l241-_shr_popcount(l139&l244);if(
bucket_index>= (1<<l242)||l234>= l239*l240){return SOC_E_PARAM;}l238 = l234%
l240;for(l159 = 0;l159<l241;l159++){if((1<<l159)&l139){continue;}l236[l174++]
= l159;}l237 = l236[l234/l240];*l235 = (l238<<l242)|(bucket_index<<l243)|(
l237);return SOC_E_NONE;}static int _soc_th_alpm_move_inval(int l80,soc_mem_t
l118,int l128,alpm_mem_prefix_array_t*l129,int*l97){int l159,l137 = 
SOC_E_NONE,l179;defip_alpm_ipv4_entry_t l183;defip_alpm_ipv6_64_entry_t l185;
int l245,l246;void*l247 = NULL,*l248 = NULL;int*l249 = NULL;int l250 = FALSE;
l245 = l128?sizeof(l185):sizeof(l183);l246 = l245*l129->count;l247 = 
sal_alloc(l246,"rb_bufp");l248 = sal_alloc(l246,"rb_sip_bufp");l249 = 
sal_alloc(sizeof(*l249)*l129->count,"roll_back_index");if(l247 == NULL||l248
== NULL||l249 == NULL){l137 = SOC_E_MEMORY;goto l251;}sal_memset(l249,-1,
sizeof(*l249)*l129->count);for(l159 = 0;l159<l129->count;l159++){payload_t*
prefix = l129->prefix[l159];if(prefix->index>= 0){l137 = soc_mem_read(l80,
l118,MEM_BLOCK_ANY,prefix->index,(uint8*)l247+l159*l245);if(SOC_FAILURE(l137)
){l159--;l250 = TRUE;break;}if(SOC_URPF_STATUS_GET(l80)){l137 = soc_mem_read(
l80,l118,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,prefix->index),(uint8*)l248
+l159*l245);if(SOC_FAILURE(l137)){l159--;l250 = TRUE;break;}}l137 = 
soc_mem_write(l80,l118,MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null(l80,
l118));if(SOC_FAILURE(l137)){l249[l159] = prefix->index;l250 = TRUE;break;}if
(SOC_URPF_STATUS_GET(l80)){l137 = soc_mem_write(l80,l118,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,prefix->index),soc_mem_entry_null(l80,l118));if(
SOC_FAILURE(l137)){l249[l159] = prefix->index;l250 = TRUE;break;}}}l249[l159]
= prefix->index;prefix->index = l97[l159];}if(l250){for(;l159>= 0;l159--){
payload_t*prefix = l129->prefix[l159];prefix->index = l249[l159];if(l249[l159
]<0){continue;}l179 = soc_mem_write(l80,l118,MEM_BLOCK_ALL,l249[l159],(uint8*
)l247+l159*l245);if(SOC_FAILURE(l179)){break;}if(!SOC_URPF_STATUS_GET(l80)){
continue;}l179 = soc_mem_write(l80,l118,MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(
l80,l249[l159]),(uint8*)l248+l159*l245);if(SOC_FAILURE(l179)){break;}}}l251:
if(l249!= NULL){sal_free(l249);}if(l248!= NULL){sal_free(l248);}if(l247!= 
NULL){sal_free(l247);}return l137;}void _soc_th_alpm_rollback_pivot_add(int
l80,int l128,defip_entry_t*l252,void*key_data,int tcam_index,alpm_pivot_t*
pivot_pyld){int l137;trie_t*l152 = NULL;int vrf,vrf_id;trie_node_t*l253 = 
NULL;(void)soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf);l137 = 
soc_th_alpm_lpm_delete(l80,l252);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: Failure to free new prefix""at %d\n"),
soc_th_alpm_logical_idx(l80,L3_DEFIPm,tcam_index,l128)));}if(l128){l152 = 
VRF_PIVOT_TRIE_IPV6(l80,vrf);}else{l152 = VRF_PIVOT_TRIE_IPV4(l80,vrf);}if(
ALPM_TCAM_PIVOT(l80,tcam_index<<(l128?1:0))!= NULL){l137 = trie_delete(l152,
pivot_pyld->key,pivot_pyld->len,&l253);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_insert: trie delete failure "
"in bkt move rollback\n")));}}ALPM_TCAM_PIVOT(l80,tcam_index<<(l128?1:0)) = 
NULL;VRF_PIVOT_REF_DEC(l80,vrf,l128);}int _soc_th_alpm_rollback_bkt_move(int
l80,int l128,void*key_data,soc_mem_t l118,alpm_pivot_t*l254,alpm_pivot_t*l255
,alpm_mem_prefix_array_t*l256,int*l97,int l257){trie_node_t*l253 = NULL;
uint32 prefix[5],l150;trie_t*l258;int vrf,vrf_id,l159,l120 = 0;
defip_alpm_ipv4_entry_t l183;defip_alpm_ipv6_64_entry_t l185;
defip_alpm_ipv6_128_entry_t l259;void*l181;payload_t*l260;int l137 = 
SOC_E_NONE;alpm_bucket_handle_t*l261;l261 = PIVOT_BUCKET_HANDLE(l255);if(l118
== L3_DEFIP_ALPM_IPV6_128m){l137 = _soc_th_alpm_128_prefix_create(l80,
key_data,prefix,&l150,&l120);}else{l137 = l145(l80,l128,key_data,prefix,&l150
,&l120);}if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"prefix create failed\n")));return l137;}if(l118 == L3_DEFIP_ALPM_IPV6_128m){
l181 = ((uint32*)&(l259));(void)soc_th_alpm_128_lpm_vrf_get(l80,key_data,&
vrf_id,&vrf);}else{l181 = ((l128)?((uint32*)&(l185)):((uint32*)&(l183)));(
void)soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf);}if(l118 == 
L3_DEFIP_ALPM_IPV6_128m){l258 = VRF_PREFIX_TRIE_IPV6_128(l80,vrf);}else if(
l128){l258 = VRF_PREFIX_TRIE_IPV6(l80,vrf);}else{l258 = VRF_PREFIX_TRIE_IPV4(
l80,vrf);}for(l159 = 0;l159<l256->count;l159++){payload_t*l93 = l256->prefix[
l159];if(l97[l159]!= -1){if(l118 == L3_DEFIP_ALPM_IPV6_128m){sal_memset(l181,
0,sizeof(defip_alpm_ipv6_128_entry_t));}else if(l128){sal_memset(l181,0,
sizeof(defip_alpm_ipv6_64_entry_t));}else{sal_memset(l181,0,sizeof(
defip_alpm_ipv4_entry_t));}l137 = soc_mem_write(l80,l118,MEM_BLOCK_ANY,l97[
l159],l181);_soc_tomahawk_alpm_bkt_view_set(l80,l97[l159],INVALIDm);if(
SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l80)){l137 = soc_mem_write(l80,l118,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,l97[l159]),l181);_soc_tomahawk_alpm_bkt_view_set(
l80,_soc_th_alpm_rpf_entry(l80,l97[l159]),INVALIDm);if(SOC_FAILURE(l137)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l253 = NULL;l137 = trie_delete(
PIVOT_BUCKET_TRIE(l255),l93->key,l93->len,&l253);l260 = (payload_t*)l253;if(
SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(l93->
index>0){l137 = trie_insert(PIVOT_BUCKET_TRIE(l254),l93->key,NULL,l93->len,(
trie_node_t*)l260);if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l260!= NULL){sal_free(l260);}}}if(l257
== -1){l253 = NULL;l137 = trie_delete(PIVOT_BUCKET_TRIE(l254),prefix,l150,&
l253);l260 = (payload_t*)l253;if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l80,"Expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l260!= 
NULL){sal_free(l260);}}l137 = soc_th_alpm_bs_free(l80,PIVOT_BUCKET_INDEX(l255
),vrf,l128);if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"New bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(l255)));}trie_destroy(
PIVOT_BUCKET_TRIE(l255));sal_free(l261);sal_free(l255);sal_free(l97);l253 = 
NULL;l137 = trie_delete(l258,prefix,l150,&l253);l260 = (payload_t*)l253;if(
SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Failed to delete new prefix""0x%08x 0x%08x from pfx trie\n"),prefix[0],
prefix[1]));}if(l260){sal_free(l260);}return l137;}int
_soc_th_alpm_free_pfx_trie(int l80,trie_t*l258,trie_t*l262,payload_t*
new_pfx_pyld,int*l97,int bktid,int vrf,int l128){int l137 = SOC_E_NONE;
trie_node_t*l253 = NULL;payload_t*l263 = NULL;payload_t*l264 = NULL;if(l97!= 
NULL){sal_free(l97);}(void)trie_delete(l258,new_pfx_pyld->key,new_pfx_pyld->
len,&l253);l264 = (payload_t*)l253;if(l264!= NULL){(void)trie_delete(l262,
l264->key,l264->len,&l253);l263 = (payload_t*)l253;if(l263!= NULL){sal_free(
l263);}sal_free(l264);}if(bktid!= -1){l137 = soc_th_alpm_bs_free(l80,bktid,
vrf,l128);if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"unit %d Unable to free bucket_id %d"),l80,bktid));}}return SOC_E_NONE;}
static int l265(int l80,int l128,alpm_pfx_info_t*l266,trie_t*l258,uint32*l218
,uint32 l150,trie_node_t*l267,defip_entry_t*lpm_entry,uint32*l222){
trie_node_t*l153 = NULL;defip_alpm_ipv4_entry_t l183;
defip_alpm_ipv6_64_entry_t l185;payload_t*l268 = NULL;int l269;void*l181;
alpm_pivot_t*l270;alpm_bucket_handle_t*l261;int l137 = SOC_E_NONE;soc_mem_t
l118;l270 = l266->pivot_pyld;l269 = l270->tcam_index;l118 = (l128)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l181 = ((l128)?((uint32*)&(l185)):
((uint32*)&(l183)));l153 = NULL;l137 = trie_find_lpm(l258,l218,l150,&l153);
l268 = (payload_t*)l153;if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l80,l218[0],l218[1],l218
[2],l218[3],l218[4],l150));return l137;}if(l268->bkt_ptr){if(l268->bkt_ptr == 
l266->new_pfx_pyld){sal_memcpy(l181,l266->alpm_data,l128?sizeof(
defip_alpm_ipv6_64_entry_t):sizeof(defip_alpm_ipv4_entry_t));}else{l137 = 
soc_mem_read(l80,l118,MEM_BLOCK_ANY,((payload_t*)l268->bkt_ptr)->index,l181);
}if(SOC_FAILURE(l137)){return l137;}l137 = l122(l80,l181,l118,l128,l266->
vrf_id,l266->bktid,((payload_t*)l268->bkt_ptr)->index,lpm_entry);if(
SOC_FAILURE(l137)){return l137;}*l222 = ((payload_t*)(l268->bkt_ptr))->len;}
else{l137 = soc_mem_read(l80,L3_DEFIPm,MEM_BLOCK_ANY,soc_th_alpm_logical_idx(
l80,L3_DEFIPm,l269>>1,1),lpm_entry);if((!l128)&&(l269&1)){l137 = 
soc_th_alpm_lpm_ip4entry1_to_0(l80,lpm_entry,lpm_entry,0);}}l261 = sal_alloc(
sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l261 == NULL){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l137 = SOC_E_MEMORY;return l137;}sal_memset(l261,0,sizeof(*l261));l270 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l270 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l137 = SOC_E_MEMORY;return l137;}sal_memset(l270,0,sizeof(*l270));
PIVOT_BUCKET_HANDLE(l270) = l261;if(l128){l137 = trie_init(_MAX_KEY_LEN_144_,
&PIVOT_BUCKET_TRIE(l270));}else{l137 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l270));}PIVOT_BUCKET_TRIE(l270)->trie = l267;
PIVOT_BUCKET_INDEX(l270) = l266->bktid;PIVOT_BUCKET_VRF(l270) = l266->vrf;
PIVOT_BUCKET_IPV6(l270) = l128;PIVOT_BUCKET_DEF(l270) = FALSE;(l270)->key[0] = 
l218[0];(l270)->key[1] = l218[1];(l270)->key[2] = l218[2];(l270)->key[3] = 
l218[3];(l270)->key[4] = l218[4];(l270)->len = l150;do{if(!(l128)){l218[0] = 
(((32-l150) == 32)?0:(l218[1])<<(32-l150));l218[1] = 0;}else{int l271 = 64-
l150;int l272;if(l271<32){l272 = l218[3]<<l271;l272|= (((32-l271) == 32)?0:(
l218[4])>>(32-l271));l218[0] = l218[4]<<l271;l218[1] = l272;l218[2] = l218[3]
= l218[4] = 0;}else{l218[1] = (((l271-32) == 32)?0:(l218[4])<<(l271-32));l218
[0] = l218[2] = l218[3] = l218[4] = 0;}}}while(0);l124(l80,l218,l150,l266->
vrf,l128,lpm_entry,0,0);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l49),(ALPM_BKT_IDX(l266->bktid)));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l51),(ALPM_BKT_SIDX(l266->bktid)));l266->pivot_pyld = l270;
return l137;}static int l273(int l80,int l128,alpm_pfx_info_t*l266,int*l221,
int*l111){trie_node_t*l267;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];uint32 l150,
l222 = 0;uint32 l139 = 0;uint32 l218[5];int l94;defip_alpm_ipv4_entry_t l183,
l184;defip_alpm_ipv6_64_entry_t l185,l186;trie_t*l258,*trie;void*l181,*l182;
alpm_pivot_t*l274 = l266->pivot_pyld;defip_entry_t lpm_entry;soc_mem_t l118;
trie_t*l152 = NULL;alpm_mem_prefix_array_t l167;int*l97 = NULL;int*l96 = NULL
;int l137 = SOC_E_NONE,l159,l257 = -1;int tcam_index,l275,l276 = 0;int l277 = 
0,l278 = 0;l118 = (l128)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l181 = ((
l128)?((uint32*)&(l185)):((uint32*)&(l183)));l182 = ((l128)?((uint32*)&(l186)
):((uint32*)&(l184)));soc_th_alpm_bank_db_type_get(l80,l266->vrf,&l139,NULL);
if(l128){l258 = VRF_PREFIX_TRIE_IPV6(l80,l266->vrf);}else{l258 = 
VRF_PREFIX_TRIE_IPV4(l80,l266->vrf);}trie = PIVOT_BUCKET_TRIE(l266->
pivot_pyld);l276 = l266->bktid;l275 = PIVOT_TCAM_INDEX(l274);l137 = 
soc_th_alpm_bs_alloc(l80,&l266->bktid,l266->vrf,l128);if(SOC_FAILURE(l137)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Unable to allocate "
"new bucket for split\n")));l266->bktid = -1;_soc_th_alpm_free_pfx_trie(l80,
l258,trie,l266->new_pfx_pyld,l97,l266->bktid,l266->vrf,l128);return l137;}
l159 = _soc_th_alpm_bkt_entry_cnt(l80,l128);l159-= 
SOC_ALPM_BS_BKT_USAGE_COUNT(l80,ALPM_BKT_IDX(l266->bktid));l137 = trie_split(
trie,l128?_MAX_KEY_LEN_144_:_MAX_KEY_LEN_48_,FALSE,l218,&l150,&l267,NULL,
FALSE,l159);if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Could not split bucket")));_soc_th_alpm_free_pfx_trie(l80,l258,trie,l266->
new_pfx_pyld,l97,l266->bktid,l266->vrf,l128);return l137;}l137 = l265(l80,
l128,l266,l258,l218,l150,l267,&lpm_entry,&l222);if(l137!= SOC_E_NONE){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"could not initialize pivot")));
_soc_th_alpm_free_pfx_trie(l80,l258,trie,l266->new_pfx_pyld,l97,l266->bktid,
l266->vrf,l128);return l137;}sal_memset(&l167,0,sizeof(l167));l137 = 
trie_traverse(PIVOT_BUCKET_TRIE(l266->pivot_pyld),
_soc_th_alpm_mem_prefix_array_cb,&l167,_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE
(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Bucket split failed")));
_soc_th_alpm_free_pfx_trie(l80,l258,trie,l266->new_pfx_pyld,l97,l266->bktid,
l266->vrf,l128);return l137;}l97 = sal_alloc(sizeof(*l97)*l167.count,
"new_index");if(l97 == NULL){_soc_th_alpm_free_pfx_trie(l80,l258,trie,l266->
new_pfx_pyld,l97,l266->bktid,l266->vrf,l128);return SOC_E_MEMORY;}l96 = 
sal_alloc(sizeof(*l96)*l167.count,"new_index");if(l96 == NULL){
_soc_th_alpm_free_pfx_trie(l80,l258,trie,l266->new_pfx_pyld,l97,l266->bktid,
l266->vrf,l128);return SOC_E_MEMORY;}sal_memset(l97,-1,sizeof(*l97)*l167.
count);sal_memset(l96,-1,sizeof(*l96)*l167.count);for(l159 = 0;l159<l167.
count;l159++){payload_t*l93 = l167.prefix[l159];if(l93->index>0){l137 = 
soc_mem_read(l80,l118,MEM_BLOCK_ANY,l93->index,l181);if(SOC_FAILURE(l137)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_insert: Failed to"
"read prefix ""0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l167.prefix[l159]->
key[1],l167.prefix[l159]->key[2],l167.prefix[l159]->key[3],l167.prefix[l159]
->key[4]));(void)_soc_th_alpm_rollback_bkt_move(l80,l128,l266->key_data,l118,
l274,l266->pivot_pyld,&l167,l97,l257);sal_free(l96);return l137;}if(
SOC_URPF_STATUS_GET(l80)){l137 = soc_mem_read(l80,l118,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,l93->index),l182);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Failed to read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l167.prefix[l159]->key[1],l167.
prefix[l159]->key[2],l167.prefix[l159]->key[3],l167.prefix[l159]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l80,l128,l266->key_data,l118,l274,l266->
pivot_pyld,&l167,l97,l257);sal_free(l96);return l137;}}soc_mem_field32_set(
l80,l118,l181,SUB_BKT_PTRf,ALPM_BKT_SIDX(l266->bktid));soc_mem_field32_set(
l80,l118,l182,SUB_BKT_PTRf,ALPM_BKT_SIDX(l266->bktid));l137 = 
_soc_th_alpm_insert_in_bkt(l80,l118,l266->bktid,l139,l181,l81,&l94,l128);if(
SOC_SUCCESS(l137)){if(SOC_URPF_STATUS_GET(l80)){l137 = soc_mem_write(l80,l118
,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),l182);}l277++;l278++;}}else{
soc_mem_field32_set(l80,l118,l266->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(l266
->bktid));l137 = _soc_th_alpm_insert_in_bkt(l80,l118,l266->bktid,l139,l266->
alpm_data,l81,&l94,l128);if(SOC_SUCCESS(l137)){l257 = l159;*l111 = l94;if(
SOC_URPF_STATUS_GET(l80)){soc_mem_field32_set(l80,l118,l266->alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l266->bktid));l137 = soc_mem_write(l80,l118,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),l266->alpm_sip_data);}l278++;}}
l97[l159] = l94;if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l80,"Failed to insert prefix ""0x%08x 0x%08x 0x%08x 0x%08x to bucket %d\n"),
l167.prefix[l159]->key[1],l167.prefix[l159]->key[2],l167.prefix[l159]->key[3]
,l167.prefix[l159]->key[4],l266->bktid));(void)_soc_th_alpm_rollback_bkt_move
(l80,l128,l266->key_data,l118,l274,l266->pivot_pyld,&l167,l97,l257);sal_free(
l96);return l137;}l96[l159] = l93->index;}l137 = l103(l80,&lpm_entry,l221);if
(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Unable to add new pivot to tcam\n")));if(l137 == SOC_E_FULL){
VRF_PIVOT_FULL_INC(l80,l266->vrf,l128);}(void)_soc_th_alpm_rollback_bkt_move(
l80,l128,l266->key_data,l118,l274,l266->pivot_pyld,&l167,l97,l257);sal_free(
l96);return l137;}*l221 = soc_th_alpm_physical_idx(l80,L3_DEFIPm,*l221,l128);
l137 = l220(l80,*l221,l128,l222);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Unable to init bpm_len ""for index %d\n"),*
l221));_soc_th_alpm_rollback_pivot_add(l80,l128,&lpm_entry,l266->key_data,*
l221,l266->pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l80,l128,l266->
key_data,l118,l274,l266->pivot_pyld,&l167,l97,l257);sal_free(l96);return l137
;}if(l128){l152 = VRF_PIVOT_TRIE_IPV6(l80,l266->vrf);}else{l152 = 
VRF_PIVOT_TRIE_IPV4(l80,l266->vrf);}l137 = trie_insert(l152,l266->pivot_pyld
->key,NULL,l266->pivot_pyld->len,(trie_node_t*)l266->pivot_pyld);if(
SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"failed to insert into pivot trie\n")));(void)_soc_th_alpm_rollback_bkt_move(
l80,l128,l266->key_data,l118,l274,l266->pivot_pyld,&l167,l97,l257);sal_free(
l96);return l137;}tcam_index = *l221<<(l128?1:0);ALPM_TCAM_PIVOT(l80,
tcam_index) = l266->pivot_pyld;PIVOT_TCAM_INDEX(l266->pivot_pyld) = 
tcam_index;VRF_PIVOT_REF_INC(l80,l266->vrf,l128);l137 = 
_soc_th_alpm_move_inval(l80,l118,l128,&l167,l97);if(SOC_FAILURE(l137)){
_soc_th_alpm_rollback_pivot_add(l80,l128,&lpm_entry,l266->key_data,*l221,l266
->pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l80,l128,l266->key_data,
l118,l274,l266->pivot_pyld,&l167,l97,l257);sal_free(l97);l97 = NULL;sal_free(
l96);return l137;}l137 = soc_th_alpm_update_hit_bits(l80,l167.count,l96,l97);
if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l167.count));l137 = 
SOC_E_NONE;}sal_free(l97);l97 = NULL;sal_free(l96);if(l257 == -1){
soc_mem_field32_set(l80,l118,l266->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(
PIVOT_BUCKET_INDEX(l274)));l137 = _soc_th_alpm_insert_in_bkt(l80,l118,
PIVOT_BUCKET_INDEX(l274),l139,l266->alpm_data,l81,&l94,l128);if(SOC_FAILURE(
l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Could not insert new "
"prefix into trie after split\n")));_soc_th_alpm_free_pfx_trie(l80,l258,trie,
l266->new_pfx_pyld,l97,l266->bktid,l266->vrf,l128);return l137;}l277--;if(
SOC_URPF_STATUS_GET(l80)){l137 = soc_mem_write(l80,l118,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,l94),l266->alpm_sip_data);}*l111 = l94;l266->
new_pfx_pyld->index = l94;}soc_th_alpm_bu_upd(l80,l276,l275,FALSE,l128,-l277)
;soc_th_alpm_bu_upd(l80,l266->bktid,tcam_index,l266->vrf,l128,l278);
PIVOT_BUCKET_ENT_CNT_UPDATE(l266->pivot_pyld);VRF_BUCKET_SPLIT_INC(l80,l266->
vrf,l128);return l137;}static int l279(int l80,int l128,void*key_data,
soc_mem_t l118,void*alpm_data,void*alpm_sip_data,int*l111,int bktid,int
tcam_index){alpm_pivot_t*l270,*l274;defip_aux_scratch_entry_t l109;uint32 l81
[SOC_MAX_MEM_FIELD_WORDS];uint32 prefix[5],l280,l150;int vrf,vrf_id;int l94;
int l137 = SOC_E_NONE,l179;uint32 l107,l139;int l151 =0;int l221;int l281 = 0
;trie_t*trie,*l258;trie_node_t*l153 = NULL,*l253 = NULL;payload_t*l260,*l282,
*l268;int l120 = 0;alpm_pfx_info_t l266;SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf));
soc_th_alpm_bank_db_type_get(l80,vrf,&l139,&l107);l118 = (l128)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l137 = l145(l80,l128,key_data,
prefix,&l150,&l120);if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"_soc_th_alpm_insert: prefix create failed\n")));return l137;}
sal_memset(&l109,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(
l105(l80,key_data,l128,l107,0,&l109));if(bktid == 0){l137 = l149(l80,prefix,
l150,l128,vrf,&l151,&tcam_index,&bktid);SOC_IF_ERROR_RETURN(l137);
soc_mem_field32_set(l80,l118,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));}
l137 = _soc_th_alpm_insert_in_bkt(l80,l118,bktid,l139,alpm_data,l81,&l94,l128
);if(l137 == SOC_E_NONE){*l111 = l94;if(SOC_URPF_STATUS_GET(l80)){
soc_mem_field32_set(l80,l118,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid))
;l179 = soc_mem_write(l80,l118,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),
alpm_sip_data);if(SOC_FAILURE(l179)){return l179;}}}if(l137 == SOC_E_FULL){
l151 = _soc_th_alpm_bkt_entry_cnt(l80,l128);if(l151>4){l151 = 4;}if(
PIVOT_BUCKET_COUNT(ALPM_TCAM_PIVOT(l80,tcam_index))<l151){int l283;l137 = 
soc_th_alpm_bs_alloc(l80,&l283,vrf,l128);SOC_IF_ERROR_RETURN(l137);l137 = 
_soc_th_alpm_move_trie(l80,l128,bktid,l283);SOC_IF_ERROR_RETURN(l137);bktid = 
l283;soc_mem_field32_set(l80,l118,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid)
);l137 = _soc_th_alpm_insert_in_bkt(l80,l118,bktid,l139,alpm_data,l81,&l94,
l128);if(SOC_SUCCESS(l137)){*l111 = l94;if(SOC_URPF_STATUS_GET(l80)){
soc_mem_field32_set(l80,l118,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid))
;l179 = soc_mem_write(l80,l118,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,l94),
alpm_sip_data);if(SOC_FAILURE(l179)){return l179;}}}else{return l137;}}else{
l281 = 1;}}l270 = ALPM_TCAM_PIVOT(l80,tcam_index);trie = PIVOT_BUCKET_TRIE(
l270);l274 = l270;l260 = sal_alloc(sizeof(payload_t),"Payload for Key");if(
l260 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 payload entry\n")));
return SOC_E_MEMORY;}l282 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l282 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_vrf_add: unable to allocate memory for "
"pfx trie node\n")));sal_free(l260);return SOC_E_MEMORY;}sal_memset(l260,0,
sizeof(*l260));sal_memset(l282,0,sizeof(*l282));sal_memcpy(l260->key,prefix,
sizeof(prefix));l260->len = l150;l260->index = l94;sal_memcpy(l282,l260,
sizeof(*l260));l282->bkt_ptr = l260;l137 = trie_insert(trie,prefix,NULL,l150,
(trie_node_t*)l260);if(SOC_FAILURE(l137)){if(l260!= NULL){sal_free(l260);}if(
l282!= NULL){sal_free(l282);}return l137;}if(l128){l258 = 
VRF_PREFIX_TRIE_IPV6(l80,vrf);}else{l258 = VRF_PREFIX_TRIE_IPV4(l80,vrf);}if(
!l120){l137 = trie_insert(l258,prefix,NULL,l150,(trie_node_t*)l282);}else{
l153 = NULL;l137 = trie_find_lpm(l258,0,0,&l153);l268 = (payload_t*)l153;if(
SOC_SUCCESS(l137)){l268->bkt_ptr = l260;}}l280 = l150;if(SOC_FAILURE(l137)){
l253 = NULL;(void)trie_delete(trie,prefix,l280,&l253);l268 = (payload_t*)l253
;sal_free(l268);sal_free(l282);return l137;}if(l281){l266.key_data = key_data
;l266.new_pfx_pyld = l260;l266.pivot_pyld = l270;l266.alpm_data = alpm_data;
l266.alpm_sip_data = alpm_sip_data;l266.bktid = bktid;l266.vrf_id = vrf_id;
l266.vrf = vrf;l137 = l273(l80,l128,&l266,&l221,l111);if(l137!= SOC_E_NONE){
return l137;}bktid = l266.bktid;tcam_index = PIVOT_TCAM_INDEX(l266.pivot_pyld
);alpm_split_count++;}else{soc_th_alpm_bu_upd(l80,bktid,tcam_index,vrf,l128,1
);}VRF_TRIE_ROUTES_INC(l80,vrf,l128);if(l120){sal_free(l282);}
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,ALG_HIT_IDXf,*l111);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l80,INSERT_PROPAGATE,&l109,TRUE,&l151
,&tcam_index,&bktid));if(SOC_URPF_STATUS_GET(l80)){uint32 l284[4] = {0,0,0,0}
;l150 = soc_mem_field32_get(l80,L3_DEFIP_AUX_SCRATCHm,&l109,DB_TYPEf);l150+= 
1;soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,DB_TYPEf,l150);if(l120)
{soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,RPEf,1);}else{
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,RPEf,0);}
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,ALG_HIT_IDXf,
_soc_th_alpm_rpf_entry(l80,*l111));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(
l80,INSERT_PROPAGATE,&l109,TRUE,&l151,&tcam_index,&bktid));if(!l120){
soc_mem_field_set(l80,L3_DEFIP_AUX_SCRATCHm,(uint32*)&l109,IP_ADDRf,(uint32*)
l284);soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,IP_LENGTHf,0);
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,REPLACE_LENf,0);
soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,RPEf,1);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l80,INSERT_PROPAGATE,&l109,TRUE,&l151
,&tcam_index,&bktid));}}PIVOT_BUCKET_ENT_CNT_UPDATE(l274);return l137;}static
int l124(int unit,uint32*key,int len,int vrf,int l106,defip_entry_t*lpm_entry
,int l125,int l126){uint32 l285;if(l126){sal_memset(lpm_entry,0,sizeof(
defip_entry_t));}soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l41),(vrf&SOC_VRF_MAX(unit)));if(vrf == 
(SOC_VRF_MAX(unit)+1)){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l43),(0));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l43),(SOC_VRF_MAX(unit)));}if(l106){
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
lpm_entry),(l67[(unit)]->l40),(1));if(len>= 32){l285 = 0xffffffff;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l28),(l285));l285 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32
));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l27),(l285));}else{l285 = ~(0xffffffff>>len);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l28),(l285));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l27),(0));}}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l25),(key[0]));assert(len<= 32);l285 = (len == 32)?0xffffffff:
~(0xffffffff>>len);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l27),(l285));}
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
_soc_th_alpm_delete_in_bkt(int l80,soc_mem_t l118,int l286,int l139,void*l287
,uint32*l81,int*l94,int l128){int l137;l137 = soc_mem_alpm_delete(l80,l118,
ALPM_BKT_IDX(l286),MEM_BLOCK_ALL,l139,l287,l81,l94);if(SOC_SUCCESS(l137)){
return l137;}if(SOC_TH_ALPM_SCALE_CHECK(l80,l128)){return soc_mem_alpm_delete
(l80,l118,ALPM_BKT_IDX(l286)+1,MEM_BLOCK_ALL,l139,l287,l81,l94);}return l137;
}static int l288(int l80,int l128,void*key_data,int bktid,int tcam_index,int
l94){alpm_pivot_t*pivot_pyld;defip_alpm_ipv4_entry_t l183,l184,l289;
defip_alpm_ipv6_64_entry_t l185,l186,l290;defip_aux_scratch_entry_t l109;
uint32 l81[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l118;void*l181,*l287,*l182 = 
NULL;int vrf_id;int l137 = SOC_E_NONE,l179;uint32 l291[5],prefix[5];int vrf;
uint32 l150;int l286;uint32 l107,l139;int l151,l120 = 0;trie_t*trie,*l258;
uint32 l292;defip_entry_t lpm_entry,*l293;payload_t*l260 = NULL,*l294 = NULL,
*l268 = NULL;trie_node_t*l253 = NULL,*l153 = NULL;trie_t*l152 = NULL;
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf));if(!(
ALPM_PREFIX_IN_TCAM(l80,vrf_id))){soc_th_alpm_bank_db_type_get(l80,vrf,&l139,
&l107);l137 = l145(l80,l128,key_data,prefix,&l150,&l120);if(SOC_FAILURE(l137)
){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: prefix create failed\n")));return l137;}if(
soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_COMBINED){if(vrf_id!= 
SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l80,vrf,l128)>1){if(l120){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"VRF %d: Cannot delete default "
"route if other routes are present in ""this mode"),vrf));return SOC_E_PARAM;
}}}l107 = 2;}l118 = (l128)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l181 = 
((l128)?((uint32*)&(l185)):((uint32*)&(l183)));l287 = ((l128)?((uint32*)&(
l290)):((uint32*)&(l289)));SOC_ALPM_LPM_LOCK(l80);if(bktid == 0){l137 = 
_soc_th_alpm_find(l80,l118,l128,key_data,vrf_id,vrf,l181,&tcam_index,&bktid,&
l94,TRUE);}else{l137 = l115(l80,l128,key_data,l181,0,l118,0,0,bktid);}
sal_memcpy(l287,l181,l128?sizeof(l185):sizeof(l183));if(SOC_FAILURE(l137)){
SOC_ALPM_LPM_UNLOCK(l80);LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Unable to find prefix for delete\n")));return l137;}l286 = 
bktid;pivot_pyld = ALPM_TCAM_PIVOT(l80,tcam_index);trie = PIVOT_BUCKET_TRIE(
pivot_pyld);l137 = trie_delete(trie,prefix,l150,&l253);l260 = (payload_t*)
l253;if(l137!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l80);return l137;}if(l128){l258 = VRF_PREFIX_TRIE_IPV6(
l80,vrf);}else{l258 = VRF_PREFIX_TRIE_IPV4(l80,vrf);}if(l128){l152 = 
VRF_PIVOT_TRIE_IPV6(l80,vrf);}else{l152 = VRF_PIVOT_TRIE_IPV4(l80,vrf);}if(!
l120){l137 = trie_delete(l258,prefix,l150,&l253);l294 = (payload_t*)l253;if(
SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l295;}l153 = NULL;l137 = trie_find_lpm(l258,prefix,l150,&
l153);l268 = (payload_t*)l153;if(SOC_SUCCESS(l137)){payload_t*l296 = (
payload_t*)(l268->bkt_ptr);if(l296!= NULL){l292 = l296->len;}else{l292 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l297;}
sal_memcpy(l291,prefix,sizeof(prefix));do{if(!(l128)){l291[0] = (((32-l150) == 
32)?0:(l291[1])<<(32-l150));l291[1] = 0;}else{int l271 = 64-l150;int l272;if(
l271<32){l272 = l291[3]<<l271;l272|= (((32-l271) == 32)?0:(l291[4])>>(32-l271
));l291[0] = l291[4]<<l271;l291[1] = l272;l291[2] = l291[3] = l291[4] = 0;}
else{l291[1] = (((l271-32) == 32)?0:(l291[4])<<(l271-32));l291[0] = l291[2] = 
l291[3] = l291[4] = 0;}}}while(0);l137 = l124(l80,prefix,l292,vrf,l128,&
lpm_entry,0,1);l179 = _soc_th_alpm_find(l80,l118,l128,&lpm_entry,vrf_id,vrf,
l181,&tcam_index,&bktid,&l94,TRUE);(void)l122(l80,l181,l118,l128,vrf_id,bktid
,0,&lpm_entry);(void)l124(l80,l291,l150,vrf,l128,&lpm_entry,0,0);if(
SOC_URPF_STATUS_GET(l80)){if(SOC_SUCCESS(l137)){l182 = ((l128)?((uint32*)&(
l186)):((uint32*)&(l184)));l179 = soc_mem_read(l80,l118,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l80,ALPM_ENT_INDEX(l94)),l182);}}if((l292 == 0)&&
SOC_FAILURE(l179)){l293 = l128?VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,vrf):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf);sal_memcpy(&lpm_entry,l293,sizeof(
lpm_entry));l137 = l124(l80,l291,l150,vrf,l128,&lpm_entry,0,1);}if(
SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l297;}l293 = 
&lpm_entry;}else{l153 = NULL;l137 = trie_find_lpm(l258,prefix,l150,&l153);
l268 = (payload_t*)l153;if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),vrf));goto l295;}l268->bkt_ptr = NULL;l292 = 
0;l293 = l128?VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,vrf):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf);}l137 = l105(l80,l293,l128,l107,l292,&
l109);if(SOC_FAILURE(l137)){goto l297;}soc_mem_field32_set(l80,
L3_DEFIP_AUX_SCRATCHm,&l109,ALG_HIT_IDXf,ALPM_ENT_INDEX(l94));l137 = 
_soc_th_alpm_aux_op(l80,DELETE_PROPAGATE,&l109,TRUE,&l151,&tcam_index,&bktid)
;if(SOC_FAILURE(l137)){goto l297;}if(SOC_URPF_STATUS_GET(l80)){uint32 l144;if
(l182!= NULL){l144 = soc_mem_field32_get(l80,L3_DEFIP_AUX_SCRATCHm,&l109,
DB_TYPEf);l144++;soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,DB_TYPEf
,l144);soc_mem_field32_set(l80,L3_DEFIP_AUX_SCRATCHm,&l109,ALG_HIT_IDXf,
_soc_th_alpm_rpf_entry(l80,ALPM_ENT_INDEX(l94)));l144 = soc_mem_field32_get(
l80,l118,l182,SRC_DISCARDf);soc_mem_field32_set(l80,l118,&l109,SRC_DISCARDf,
l144);l144 = soc_mem_field32_get(l80,l118,l182,DEFAULTROUTEf);
soc_mem_field32_set(l80,l118,&l109,DEFAULTROUTEf,l144);l137 = 
_soc_th_alpm_aux_op(l80,DELETE_PROPAGATE,&l109,TRUE,&l151,&tcam_index,&bktid)
;}if(SOC_FAILURE(l137)){goto l297;}}sal_free(l260);if(!l120){sal_free(l294);}
PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);if((pivot_pyld->len!= 0)&&(trie->trie
== NULL)){uint32 l298[5];sal_memcpy(l298,pivot_pyld->key,sizeof(l298));do{if(
!(l128)){l298[0] = (((32-pivot_pyld->len) == 32)?0:(l298[1])<<(32-pivot_pyld
->len));l298[1] = 0;}else{int l271 = 64-pivot_pyld->len;int l272;if(l271<32){
l272 = l298[3]<<l271;l272|= (((32-l271) == 32)?0:(l298[4])>>(32-l271));l298[0
] = l298[4]<<l271;l298[1] = l272;l298[2] = l298[3] = l298[4] = 0;}else{l298[1
] = (((l271-32) == 32)?0:(l298[4])<<(l271-32));l298[0] = l298[2] = l298[3] = 
l298[4] = 0;}}}while(0);l124(l80,l298,pivot_pyld->len,vrf,l128,&lpm_entry,0,1
);l137 = soc_th_alpm_lpm_delete(l80,&lpm_entry);if(SOC_FAILURE(l137)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_delete: Unable to "
"delete pivot 0x%08x 0x%08x \n"),pivot_pyld->key[0],pivot_pyld->key[1]));}}
l137 = _soc_th_alpm_delete_in_bkt(l80,l118,l286,l139,l287,l81,&l94,l128);if(
SOC_FAILURE(l137)){SOC_ALPM_LPM_UNLOCK(l80);return l137;}if(
SOC_URPF_STATUS_GET(l80)){l137 = soc_mem_alpm_delete(l80,l118,
SOC_TH_ALPM_RPF_BKT_IDX(l80,ALPM_BKT_IDX(l286)),MEM_BLOCK_ALL,l139,l287,l81,&
l151);if(SOC_FAILURE(l137)){SOC_ALPM_LPM_UNLOCK(l80);return l137;}}
soc_th_alpm_bu_upd(l80,PIVOT_BUCKET_INDEX(pivot_pyld),PIVOT_TCAM_INDEX(
pivot_pyld),((pivot_pyld->len!= 0)&&(trie->trie == NULL))?TRUE:FALSE,l128,-1)
;if((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l137 = soc_th_alpm_bs_free(
l80,PIVOT_BUCKET_INDEX(pivot_pyld),vrf,l128);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_delete: Unable to release"
"empty bucket: %d\n"),PIVOT_BUCKET_INDEX(pivot_pyld)));}l137 = trie_delete(
l152,pivot_pyld->key,pivot_pyld->len,&l253);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"could not delete pivot from pivot trie\n")))
;}trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(
pivot_pyld));sal_free(pivot_pyld);l94 = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,
ALPM_BKT_IDX(l286));_soc_tomahawk_alpm_bkt_view_set(l80,l94,INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l80,l128)){l94 = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,
ALPM_BKT_IDX(l286)+1);_soc_tomahawk_alpm_bkt_view_set(l80,l94,INVALIDm);}}}
VRF_TRIE_ROUTES_DEC(l80,vrf,l128);if(VRF_TRIE_ROUTES_CNT(l80,vrf,l128) == 0){
l137 = l127(l80,vrf,l128);}SOC_ALPM_LPM_UNLOCK(l80);return l137;l297:
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"recovering soc_th_alpm_vrf_delete failed\n ")));l179 = trie_insert(l258,
prefix,NULL,l150,(trie_node_t*)l294);if(SOC_FAILURE(l179)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into pfx trie\n"),prefix[0],prefix[1]));}l295:l179 = 
trie_insert(trie,prefix,NULL,l150,(trie_node_t*)l260);if(SOC_FAILURE(l179)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into bkt trie\n")
,prefix[0],prefix[1]));}SOC_ALPM_LPM_UNLOCK(l80);return l137;}void l299(int
l80){int l159;if(alpm_vrf_handle[l80]!= NULL){sal_free(alpm_vrf_handle[l80]);
alpm_vrf_handle[l80] = NULL;}if(tcam_pivot[l80]!= NULL){sal_free(tcam_pivot[
l80]);tcam_pivot[l80] = NULL;}if(bkt_usage[l80]!= NULL){sal_free(bkt_usage[
l80]);bkt_usage[l80] = NULL;}for(l159 = 0;l159<SOC_TH_MAX_ALPM_VIEWS;l159++){
if(global_bkt_usage[l80][l159]!= NULL){sal_free(global_bkt_usage[l80][l159]);
global_bkt_usage[l80][l159] = NULL;}}}int soc_th_alpm_init(int l80){int l159;
int l137 = SOC_E_NONE;uint32 l246;l137 = l101(l80);SOC_IF_ERROR_RETURN(l137);
l137 = l114(l80);l299(l80);alpm_vrf_handle[l80] = sal_alloc((MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t),"ALPM VRF Handles");if(alpm_vrf_handle[l80] == NULL
){l137 = SOC_E_MEMORY;goto l191;}tcam_pivot[l80] = sal_alloc(MAX_PIVOT_COUNT*
sizeof(alpm_pivot_t*),"ALPM pivots");if(tcam_pivot[l80] == NULL){l137 = 
SOC_E_MEMORY;goto l191;}sal_memset(alpm_vrf_handle[l80],0,(MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t));sal_memset(tcam_pivot[l80],0,MAX_PIVOT_COUNT*
sizeof(alpm_pivot_t*));l246 = SOC_TH_MAX_ALPM_BUCKETS*sizeof(alpm_bkt_usg_t);
bkt_usage[l80] = sal_alloc(l246,"ALPM_BS");if(bkt_usage[l80] == NULL){l137 = 
SOC_E_MEMORY;goto l191;}sal_memset(bkt_usage[l80],0,l246);l246 = 
SOC_TH_MAX_BUCKET_ENTRIES*sizeof(alpm_bkt_bmp_t);for(l159 = 0;l159<
SOC_TH_MAX_ALPM_VIEWS;l159++){global_bkt_usage[l80][l159] = sal_alloc(l246,
"ALPM_BS");if(global_bkt_usage[l80][l159] == NULL){l137 = SOC_E_MEMORY;goto
l191;}sal_memset(global_bkt_usage[l80][l159],0,l246);}for(l159 = 0;l159<
MAX_PIVOT_COUNT;l159++){ALPM_TCAM_PIVOT(l80,l159) = NULL;}if(SOC_CONTROL(l80)
->alpm_bulk_retry == NULL){SOC_CONTROL(l80)->alpm_bulk_retry = sal_sem_create
("ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l80)->alpm_lookup_retry
== NULL){SOC_CONTROL(l80)->alpm_lookup_retry = sal_sem_create(
"ALPM lookup retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l80)->alpm_insert_retry
== NULL){SOC_CONTROL(l80)->alpm_insert_retry = sal_sem_create(
"ALPM insert retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l80)->alpm_delete_retry
== NULL){SOC_CONTROL(l80)->alpm_delete_retry = sal_sem_create(
"ALPM delete retry",sal_sem_BINARY,0);}l137 = soc_th_alpm_128_lpm_init(l80);
if(SOC_FAILURE(l137)){goto l191;}return l137;l191:l299(l80);return l137;}
static int l300(int l80){int l159,l137;alpm_pivot_t*l144;for(l159 = 0;l159<
MAX_PIVOT_COUNT;l159++){l144 = ALPM_TCAM_PIVOT(l80,l159);if(l144){if(
PIVOT_BUCKET_HANDLE(l144)){if(PIVOT_BUCKET_TRIE(l144)){l137 = trie_traverse(
PIVOT_BUCKET_TRIE(l144),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l137)){trie_destroy(PIVOT_BUCKET_TRIE(l144));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Unable to clear trie state for unit %d\n"),
l80));return l137;}}sal_free(PIVOT_BUCKET_HANDLE(l144));}sal_free(
ALPM_TCAM_PIVOT(l80,l159));ALPM_TCAM_PIVOT(l80,l159) = NULL;}}for(l159 = 0;
l159<= SOC_VRF_MAX(l80)+1;l159++){l137 = trie_traverse(VRF_PREFIX_TRIE_IPV4(
l80,l159),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(
l137)){trie_destroy(VRF_PREFIX_TRIE_IPV4(l80,l159));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l80,l159));return l137;}
l137 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l80,l159),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l137)){trie_destroy(
VRF_PREFIX_TRIE_IPV6(l80,l159));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l80,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l80,l159));return
l137;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,l159)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,l159));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,
l159)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,l159));}sal_memset(&
alpm_vrf_handle[l80][l159],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l80][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l80,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l80,MAX_VRF_ID,1,1)
;VRF_TRIE_INIT_DONE(l80,MAX_VRF_ID,2,1);if(SOC_TH_ALPM_VRF_BUCKET_BMAP(l80)!= 
NULL){sal_free(SOC_TH_ALPM_VRF_BUCKET_BMAP(l80));}if(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l80)!= NULL&&SOC_TH_ALPM_GLB_BUCKET_BMAP(l80)!= 
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80)){sal_free(SOC_TH_ALPM_GLB_BUCKET_BMAP(l80));
}sal_memset(&soc_th_alpm_bucket[l80],0,sizeof(soc_alpm_bucket_t));l299(l80);
return SOC_E_NONE;}int soc_th_alpm_deinit(int l80){l102(l80);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_deinit(l80));SOC_IF_ERROR_RETURN(
soc_th_alpm_128_state_clear(l80));SOC_IF_ERROR_RETURN(l300(l80));if(
SOC_CONTROL(l80)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l80)->
alpm_bulk_retry);SOC_CONTROL(l80)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l80
)->alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l80)->
alpm_lookup_retry);SOC_CONTROL(l80)->alpm_lookup_retry = NULL;}if(SOC_CONTROL
(l80)->alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l80)->
alpm_insert_retry);SOC_CONTROL(l80)->alpm_insert_retry = NULL;}if(SOC_CONTROL
(l80)->alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l80)->
alpm_delete_retry);SOC_CONTROL(l80)->alpm_delete_retry = NULL;}return
SOC_E_NONE;}static int l301(int l80,int vrf,int l128){defip_entry_t*lpm_entry
= NULL,l302;int l303 = 0;int index;int l137 = SOC_E_NONE;uint32 key[2] = {0,0
};uint32 l150;alpm_bucket_handle_t*l261 = NULL;alpm_pivot_t*pivot_pyld = NULL
;payload_t*l294 = NULL;trie_t*l304;trie_t*l305 = NULL;if(l128 == 0){trie_init
(_MAX_KEY_LEN_48_,&VRF_PIVOT_TRIE_IPV4(l80,vrf));l305 = VRF_PIVOT_TRIE_IPV4(
l80,vrf);}else{trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(l80,vrf));
l305 = VRF_PIVOT_TRIE_IPV6(l80,vrf);}if(l128 == 0){trie_init(_MAX_KEY_LEN_48_
,&VRF_PREFIX_TRIE_IPV4(l80,vrf));l304 = VRF_PREFIX_TRIE_IPV4(l80,vrf);}else{
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l80,vrf));l304 = 
VRF_PREFIX_TRIE_IPV6(l80,vrf);}lpm_entry = sal_alloc(sizeof(defip_entry_t),
"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_vrf_add: unable to allocate memory for "
"IPv4 LPM entry\n")));return SOC_E_MEMORY;}l124(l80,key,0,vrf,l128,lpm_entry,
0,1);if(l128 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,vrf) = lpm_entry;}if(vrf == SOC_VRF_MAX(l80)+
1){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
lpm_entry),(l67[(l80)]->l21),(1));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l53),(1));}l137 = 
soc_th_alpm_bs_alloc(l80,&l303,vrf,l128);if(SOC_FAILURE(l137)){goto l306;}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),
(l67[(l80)]->l49),(ALPM_BKT_IDX(l303)));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(lpm_entry),(l67[(l80)]->l51),(ALPM_BKT_SIDX(
l303)));sal_memcpy(&l302,lpm_entry,sizeof(l302));l137 = l103(l80,&l302,&index
);if(SOC_FAILURE(l137)){goto l306;}l261 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l261 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_th_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
l137 = SOC_E_MEMORY;goto l306;}sal_memset(l261,0,sizeof(*l261));pivot_pyld = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(pivot_pyld == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
l137 = SOC_E_MEMORY;goto l306;}l294 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l294 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_vrf_add: Unable to allocate memory for "
"pfx trie node \n")));l137 = SOC_E_MEMORY;goto l306;}sal_memset(pivot_pyld,0,
sizeof(*pivot_pyld));sal_memset(l294,0,sizeof(*l294));l150 = 0;
PIVOT_BUCKET_HANDLE(pivot_pyld) = l261;if(l128){trie_init(_MAX_KEY_LEN_144_,&
PIVOT_BUCKET_TRIE(pivot_pyld));}else{trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(pivot_pyld));}PIVOT_BUCKET_INDEX(pivot_pyld) = l303;
PIVOT_BUCKET_VRF(pivot_pyld) = vrf;PIVOT_BUCKET_IPV6(pivot_pyld) = l128;
PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;pivot_pyld->key[0] = l294->key[0] = key[0
];pivot_pyld->key[1] = l294->key[1] = key[1];pivot_pyld->len = l294->len = 
l150;l137 = trie_insert(l304,key,NULL,l150,&(l294->node));if(SOC_FAILURE(l137
)){goto l306;}l137 = trie_insert(l305,key,NULL,l150,(trie_node_t*)pivot_pyld)
;if(SOC_FAILURE(l137)){trie_node_t*l253 = NULL;(void)trie_delete(l304,key,
l150,&l253);goto l306;}index = soc_th_alpm_physical_idx(l80,L3_DEFIPm,index,
l128);if(l128 == 0){ALPM_TCAM_PIVOT(l80,index) = pivot_pyld;PIVOT_TCAM_INDEX(
pivot_pyld) = index;}else{ALPM_TCAM_PIVOT(l80,index<<1) = pivot_pyld;
PIVOT_TCAM_INDEX(pivot_pyld) = index<<1;}VRF_PIVOT_REF_INC(l80,vrf,l128);
VRF_TRIE_INIT_DONE(l80,vrf,l128,1);return l137;l306:if(l294!= NULL){sal_free(
l294);}if(pivot_pyld!= NULL){sal_free(pivot_pyld);}if(l261!= NULL){sal_free(
l261);}if(lpm_entry!= NULL){sal_free(lpm_entry);}if(l128 == 0){
VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf) = NULL;}else{VRF_TRIE_DEFAULT_ROUTE_IPV6
(l80,vrf) = NULL;}return l137;}static int l127(int l80,int vrf,int l128){
defip_entry_t*lpm_entry;int l140,vrf_id,l307;int l137 = SOC_E_NONE;uint32 key
[2] = {0,0},l131[SOC_MAX_MEM_FIELD_WORDS];payload_t*l260;alpm_pivot_t*l308;
trie_node_t*l253;trie_t*l304;trie_t*l305 = NULL;soc_mem_t l118;int tcam_index
,bktid,index;uint32 l156[SOC_MAX_MEM_FIELD_WORDS];if(l128 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf);l118 = L3_DEFIP_ALPM_IPV4m;}else{
lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,vrf);l118 = 
L3_DEFIP_ALPM_IPV6_64m;}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,
lpm_entry,&vrf_id,&l307));l137 = _soc_th_alpm_find(l80,l118,l128,lpm_entry,
vrf_id,l307,l156,&tcam_index,&bktid,&index,TRUE);l137 = soc_th_alpm_bs_free(
l80,bktid,vrf,l128);index = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,ALPM_BKT_IDX(
bktid));_soc_tomahawk_alpm_bkt_view_set(l80,index,INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l80,l128)){index = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,
ALPM_BKT_IDX(bktid)+1);_soc_tomahawk_alpm_bkt_view_set(l80,index,INVALIDm);}
l137 = l113(l80,lpm_entry,(void*)l131,&l140);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"soc_alpm_vrf_delete: unable to get internal"
" pivot idx for vrf %d/%d\n"),vrf,l128));l140 = -1;}l140 = 
soc_th_alpm_physical_idx(l80,L3_DEFIPm,l140,l128);if(l128 == 0){l308 = 
ALPM_TCAM_PIVOT(l80,l140);}else{l308 = ALPM_TCAM_PIVOT(l80,l140<<1);}l137 = 
soc_th_alpm_lpm_delete(l80,lpm_entry);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),vrf,l128));}sal_free(lpm_entry);if(
l128 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l80,vrf) = NULL;l304 = 
VRF_PREFIX_TRIE_IPV4(l80,vrf);VRF_PREFIX_TRIE_IPV4(l80,vrf) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l80,vrf) = NULL;l304 = VRF_PREFIX_TRIE_IPV6(l80,
vrf);VRF_PREFIX_TRIE_IPV6(l80,vrf) = NULL;}VRF_TRIE_INIT_DONE(l80,vrf,l128,0)
;l137 = trie_delete(l304,key,0,&l253);l260 = (payload_t*)l253;if(SOC_FAILURE(
l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Unable to delete internal default for vrf "" %d/%d\n"),vrf,l128));}sal_free(
l260);(void)trie_destroy(l304);if(l128 == 0){l305 = VRF_PIVOT_TRIE_IPV4(l80,
vrf);VRF_PIVOT_TRIE_IPV4(l80,vrf) = NULL;}else{l305 = VRF_PIVOT_TRIE_IPV6(l80
,vrf);VRF_PIVOT_TRIE_IPV6(l80,vrf) = NULL;}l253 = NULL;l137 = trie_delete(
l305,key,0,&l253);if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l80,"Unable to delete internal pivot node for vrf"" %d/%d\n"),vrf,l128));}(
void)trie_destroy(l305);(void)trie_destroy(PIVOT_BUCKET_TRIE(l308));sal_free(
PIVOT_BUCKET_HANDLE(l308));sal_free(l308);return l137;}int soc_th_alpm_insert
(int l80,void*l104,uint32 l119,int l208,int l309){defip_alpm_ipv4_entry_t l183
,l184;defip_alpm_ipv6_64_entry_t l185,l186;soc_mem_t l118;void*l181,*l287;int
vrf_id,vrf;int index;int l106;int l137 = SOC_E_NONE;uint32 l120;int l121 = 0;
l106 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l104
),(l67[(l80)]->l29));l118 = (l106)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m
;l181 = ((l106)?((uint32*)&(l185)):((uint32*)&(l183)));l287 = ((l106)?((
uint32*)&(l186)):((uint32*)&(l184)));if(l208!= -1){l121 = ALPM_BKTID(
ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l80,l208)),(l208>>
ALPM_ENT_INDEX_BITS));}SOC_IF_ERROR_RETURN(l115(l80,l106,l104,l181,l287,l118,
l119,&l120,l121));SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,l104,&
vrf_id,&vrf));if(ALPM_PREFIX_IN_TCAM(l80,vrf_id)){l137 = l103(l80,l104,&index
);if(SOC_SUCCESS(l137)){if(vrf_id == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_REF_INC(
l80,MAX_VRF_ID,l106);VRF_TRIE_ROUTES_INC(l80,MAX_VRF_ID,l106);}else{
VRF_PIVOT_REF_INC(l80,vrf,l106);VRF_TRIE_ROUTES_INC(l80,vrf,l106);}}else if(
l137 == SOC_E_FULL){if(vrf_id == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_FULL_INC(l80,
MAX_VRF_ID,l106);}else{VRF_PIVOT_FULL_INC(l80,vrf,l106);}}return(l137);}else
if(vrf == 0){if(soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_PARALLEL||
soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_TCAM_ALPM){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"VRF=0 cannot be added in Parallel mode\n")))
;return SOC_E_PARAM;}}if(vrf_id!= SOC_L3_VRF_GLOBAL){if(soc_th_alpm_mode_get(
l80) == SOC_ALPM_MODE_COMBINED){if(VRF_TRIE_ROUTES_CNT(l80,vrf,l106) == 0){if
(!l120){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"VRF %d: First route in a VRF has to "" be a default route in this mode\n"),
vrf_id));return SOC_E_PARAM;}}}}if(!VRF_TRIE_INIT_COMPLETED(l80,vrf,l106)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"soc_alpm_insert:VRF %d is not "
"initialized\n"),vrf));l137 = l301(l80,vrf,l106);if(SOC_FAILURE(l137)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_insert:VRF %d/%d trie init \n""failed\n"),vrf,l106));return l137;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),vrf,l106));}if(l309&
SOC_ALPM_LOOKUP_HIT){l137 = l155(l80,l106,l104,l181,l287,l118,l208);}else{if(
l208 == -1){l208 = 0;}l208 = ALPM_BKTID(ALPM_BKT_IDX(
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l80,l208)),l208>>ALPM_ENT_INDEX_BITS);l137 = 
l279(l80,l106,l104,l118,l181,l287,&index,l208,l309);}if(l137!= SOC_E_NONE){
LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l80,soc_errmsg(l137
)));}return(l137);}int soc_th_alpm_lookup(int l80,void*key_data,void*l81,int*
l111,int*l310){defip_alpm_ipv4_entry_t l183;defip_alpm_ipv6_64_entry_t l185;
soc_mem_t l118;int bktid = 0;int tcam_index;void*l181;int vrf_id,vrf;int l106
,l93;int l137 = SOC_E_NONE;l137 = l110(l80,key_data,l81,l111,&l93,&l106,&
vrf_id,&vrf);if(SOC_SUCCESS(l137)){if(!l106&&(*l111&0x1)){l137 = 
soc_th_alpm_lpm_ip4entry1_to_0(l80,l81,l81,PRESERVE_HIT);}if(
ALPM_PREFIX_IN_TCAM(l80,vrf_id)){return SOC_E_NONE;}}if(!
VRF_TRIE_INIT_COMPLETED(l80,vrf,l106)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_lookup:VRF %d is not initialized\n"),vrf));*l310 = 0
;return SOC_E_NOT_FOUND;}l118 = (l106)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l181 = ((l106)?((uint32*)&(l185)):((uint32*)&(l183)));
SOC_ALPM_LPM_LOCK(l80);l137 = _soc_th_alpm_find(l80,l118,l106,key_data,vrf_id
,vrf,l181,&tcam_index,&bktid,l111,TRUE);SOC_ALPM_LPM_UNLOCK(l80);if(
SOC_FAILURE(l137)){*l310 = tcam_index;*l111 = (ALPM_BKT_SIDX(bktid)<<
ALPM_ENT_INDEX_BITS)|SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,bktid);return l137;}
l137 = l122(l80,l181,l118,l106,vrf_id,bktid,*l111,l81);*l310 = 
SOC_ALPM_LOOKUP_HIT|tcam_index;*l111 = (ALPM_BKT_SIDX(bktid)<<
ALPM_ENT_INDEX_BITS)|*l111;return(l137);}static int l311(int l80,void*
key_data,void*l81,int vrf,int*tcam_index,int*bucket_index,int*l94,int l312){
int l137 = SOC_E_NONE;int l159,l313,l128,l151 = 0;uint32 l107,l139;
defip_aux_scratch_entry_t l109;int l314,l315;int index;soc_mem_t l118,l173;
int l316,l317;int l318;uint32 l319[SOC_MAX_MEM_FIELD_WORDS] = {0};int l320 = 
-1;int l321 = 0;soc_field_t l322[2] = {IP_ADDR0f,IP_ADDR1f,};l173 = L3_DEFIPm
;l128 = soc_mem_field32_get(l80,l173,key_data,MODE0f);l314 = 
soc_mem_field32_get(l80,l173,key_data,GLOBAL_ROUTE0f);l315 = 
soc_mem_field32_get(l80,l173,key_data,VRF_ID_0f);LOG_VERBOSE(BSL_LS_SOC_ALPM,
(BSL_META_U(l80,"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d:\n"),vrf == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l128,l314,l315));if(vrf == SOC_L3_VRF_GLOBAL){l107 = l312?1:0;
soc_mem_field32_set(l80,l173,key_data,GLOBAL_ROUTE0f,1);soc_mem_field32_set(
l80,l173,key_data,VRF_ID_0f,0);}else{l107 = l312?3:2;}
soc_th_alpm_bank_db_type_get(l80,vrf == SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l80)+1
):vrf,&l139,NULL);sal_memset(&l109,0,sizeof(defip_aux_scratch_entry_t));
SOC_IF_ERROR_RETURN(l105(l80,key_data,l128,l107,0,&l109));if(vrf == 
SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l80,l173,key_data,GLOBAL_ROUTE0f,l314)
;soc_mem_field32_set(l80,l173,key_data,VRF_ID_0f,l315);}SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l80,PREFIX_LOOKUP,&l109,TRUE,&l151,tcam_index,
bucket_index));if(l151 == 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Could not find bucket\n")));return SOC_E_NOT_FOUND;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"Hit in memory %s, index %d, "
"bucket [%d,%d]\n"),SOC_MEM_NAME(l80,l173),soc_th_alpm_logical_idx(l80,l173,(
*tcam_index)>>1,1),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index)))
;l118 = (l128)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l137 = l136(l80,
l128,key_data,&l317);if(SOC_FAILURE(l137)){return l137;}switch(l128){case
L3_DEFIP_MODE_V4:l318 = ALPM_IPV4_BKT_COUNT;break;case L3_DEFIP_MODE_64:l318 = 
ALPM_IPV6_64_BKT_COUNT;break;default:l318 = ALPM_IPV4_BKT_COUNT;break;}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l128)){l318<<= 1;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"Start searching mem %s bucket [%d,%d](count %d) "
"for Length %d\n"),SOC_MEM_NAME(l80,l118),ALPM_BKT_IDX(*bucket_index),
ALPM_BKT_SIDX(*bucket_index),l318,l317));for(l159 = 0;l159<l318;l159++){
uint32 l181[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l285[2] = {0};uint32 l323[2
] = {0};uint32 l324[2] = {0};int l325;l137 = _soc_th_alpm_mem_index(l80,l118,
ALPM_BKT_IDX(*bucket_index),l159,l139,&index);if(l137 == SOC_E_FULL){continue
;}SOC_IF_ERROR_RETURN(soc_mem_read(l80,l118,MEM_BLOCK_ANY,index,(void*)l181))
;l325 = soc_mem_field32_get(l80,l118,l181,VALIDf);l316 = soc_mem_field32_get(
l80,l118,l181,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Bucket [%d,%d] index %6d: Valid %d, Length %d\n"),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),index,l325,l316));if(!l325||(l316>l317)){
continue;}SHR_BITSET_RANGE(l285,(l128?64:32)-l316,l316);(void)
soc_mem_field_get(l80,l118,(uint32*)l181,KEYf,(uint32*)l323);l324[1] = 
soc_mem_field32_get(l80,l173,key_data,l322[1]);l324[0] = soc_mem_field32_get(
l80,l173,key_data,l322[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tmask %08x %08x\n\t key %08x %08x\n""\thost %08x %08x\n"),l285[1],l285[0],
l323[1],l323[0],l324[1],l324[0]));for(l313 = l128?1:0;l313>= 0;l313--){if((
l324[l313]&l285[l313])!= (l323[l313]&l285[l313])){break;}}if(l313>= 0){
continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Found a match in mem %s bucket [%d,%d], ""index %d\n"),SOC_MEM_NAME(l80,l118
),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index),index));if(l320 == 
-1||l320<l316){l320 = l316;l321 = index;sal_memcpy(l319,l181,sizeof(l181));}}
if(l320!= -1){l137 = l122(l80,l319,l118,l128,vrf,*bucket_index,l321,l81);if(
SOC_SUCCESS(l137)){*l94 = l321;if(bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l80)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Hit mem %s bucket [%d,%d], index %d\n"),SOC_MEM_NAME(l80,l118),ALPM_BKT_IDX(
*bucket_index),ALPM_BKT_SIDX(*bucket_index),l321));}}return l137;}*l94 = 
soc_th_alpm_logical_idx(l80,l173,(*tcam_index)>>1,1);LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Miss in mem %s bucket [%d,%d], use associate data "
"in mem %s LOG index %d\n"),SOC_MEM_NAME(l80,l118),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),SOC_MEM_NAME(l80,l173),*l94));
SOC_IF_ERROR_RETURN(soc_mem_read(l80,l173,MEM_BLOCK_ANY,*l94,(void*)l81));if(
(!l128)&&((*tcam_index)&1)){l137 = soc_th_alpm_lpm_ip4entry1_to_0(l80,l81,l81
,PRESERVE_HIT);}return SOC_E_NONE;}int soc_th_alpm_find_best_match(int l80,
void*key_data,void*l81,int*l111,int l312){int l137 = SOC_E_NONE;int l159,l326
,l165;defip_entry_t l327;uint32 l328[2];uint32 l323[2];uint32 l329[2];uint32
l324[2];uint32 l330,l331;int vrf_id,vrf = 0;int l332[2] = {0};int tcam_index,
bucket_index;soc_mem_t l173 = L3_DEFIPm;int l213,l128,l333,l334 = 0;
soc_field_t l335[] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l336[] = {
GLOBAL_ROUTE0f,GLOBAL_ROUTE1f};l128 = soc_mem_field32_get(l80,l173,key_data,
MODE0f);if(!SOC_URPF_STATUS_GET(l80)&&l312){return SOC_E_PARAM;}l326 = 
soc_mem_index_min(l80,l173);l165 = soc_mem_index_count(l80,l173);if(
SOC_URPF_STATUS_GET(l80)){l165>>= 1;}if(soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_PARALLEL||soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_TCAM_ALPM)
{l165>>= 1;l326+= l165;}if(l312){l326+= soc_mem_index_count(l80,l173)/2;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Launch LPM searching from index %d count %d\n"),l326,l165));for(l159 = l326;
l159<l326+l165;l159++){SOC_IF_ERROR_RETURN(soc_mem_read(l80,l173,
MEM_BLOCK_ANY,l159,(void*)&l327));l332[0] = soc_mem_field32_get(l80,l173,&
l327,VALID0f);l332[1] = soc_mem_field32_get(l80,l173,&l327,VALID1f);if(l332[0
] == 0&&l332[1] == 0){continue;}l333 = soc_mem_field32_get(l80,l173,&l327,
MODE0f);if(l333!= l128){continue;}for(l213 = 0;l213<(l128?1:2);l213++){if(
l332[l213] == 0){continue;}l330 = soc_mem_field32_get(l80,l173,&l327,l335[
l213]);l331 = soc_mem_field32_get(l80,l173,&l327,l336[l213]);if(!(
soc_th_alpm_mode_get(l80)!= SOC_ALPM_MODE_TCAM_ALPM&&l330&&l331)&&!(
soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_TCAM_ALPM&&l331)){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Match a Global High route: ent %d\n"),l213));l328[0] = soc_mem_field32_get(
l80,l173,&l327,IP_ADDR_MASK0f);l328[1] = soc_mem_field32_get(l80,l173,&l327,
IP_ADDR_MASK1f);l323[0] = soc_mem_field32_get(l80,l173,&l327,IP_ADDR0f);l323[
1] = soc_mem_field32_get(l80,l173,&l327,IP_ADDR1f);l329[0] = 
soc_mem_field32_get(l80,l173,key_data,IP_ADDR_MASK0f);l329[1] = 
soc_mem_field32_get(l80,l173,key_data,IP_ADDR_MASK1f);l324[0] = 
soc_mem_field32_get(l80,l173,key_data,IP_ADDR0f);l324[1] = 
soc_mem_field32_get(l80,l173,key_data,IP_ADDR1f);LOG_VERBOSE(BSL_LS_SOC_ALPM,
(BSL_META_U(l80,"\thmsk %08x %08x\n\thkey %08x %08x\n"
"\tsmsk %08x %08x\n\tskey %08x %08x\n"),l328[1],l328[0],l323[1],l323[0],l329[
1],l329[0],l324[1],l324[0]));if(l128&&(((l328[1]&l329[1])!= l328[1])||((l328[
0]&l329[0])!= l328[0]))){continue;}if(!l128&&((l328[l213]&l329[0])!= l328[
l213])){continue;}if(l128&&((l324[0]&l328[0]) == (l323[0]&l328[0]))&&((l324[1
]&l328[1]) == (l323[1]&l328[1]))){l334 = TRUE;break;}if(!l128&&((l324[0]&l328
[l213]) == (l323[l213]&l328[l213]))){l334 = TRUE;break;}}if(!l334){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Hit Global High route in index = %d(%d)\n"),l159,l213));sal_memcpy(l81,&l327
,sizeof(l327));if(!l128&&l213 == 1){l137 = soc_th_alpm_lpm_ip4entry1_to_0(l80
,l81,l81,PRESERVE_HIT);}*l111 = l159;return l137;}LOG_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l80,"Global high lookup miss, use AUX engine to "
"search for VRF and Global Low routes\n")));SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf));l137 = l311(l80,key_data,
l81,vrf,&tcam_index,&bucket_index,l111,l312);if(l137 == SOC_E_NOT_FOUND){vrf = 
SOC_L3_VRF_GLOBAL;LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Not found in VRF region, trying Global ""region\n")));l137 = l311(l80,
key_data,l81,vrf,&tcam_index,&bucket_index,l111,l312);}if(SOC_SUCCESS(l137)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"Hit in %s region in TCAM index %d, ""bucket [%d,%d]\n"),vrf == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",tcam_index,ALPM_BKT_IDX(bucket_index),
ALPM_BKT_SIDX(bucket_index)));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l80,"Search miss for given address\n")));}return(l137);}int soc_th_alpm_delete
(int l80,void*key_data,int l208,int l309){int l337;int vrf_id,vrf;int l106;
int l137 = SOC_E_NONE;l106 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l29));SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l80,key_data,&vrf_id,&vrf));if(ALPM_PREFIX_IN_TCAM(
l80,vrf_id)){l137 = soc_th_alpm_lpm_delete(l80,key_data);if(SOC_SUCCESS(l137)
){if(vrf_id == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_REF_DEC(l80,MAX_VRF_ID,l106);
VRF_TRIE_ROUTES_DEC(l80,MAX_VRF_ID,l106);}else{VRF_PIVOT_REF_DEC(l80,vrf,l106
);VRF_TRIE_ROUTES_DEC(l80,vrf,l106);}}return(l137);}else{if(!
VRF_TRIE_INIT_COMPLETED(l80,vrf,l106)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"soc_alpm_delete:VRF %d/%d is not initialized\n"),vrf,l106));
return SOC_E_NONE;}if(l208 == -1){l208 = 0;}l337 = l208;l208 = ALPM_BKTID(
ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l80,l337)),l337>>
ALPM_ENT_INDEX_BITS);l137 = l288(l80,l106,key_data,l208,l309&~
SOC_ALPM_LOOKUP_HIT,l337);}return(l137);}static int l114(int l80){int l338;
l338 = soc_mem_index_count(l80,L3_DEFIPm)+soc_mem_index_count(l80,
L3_DEFIP_PAIR_128m)*2;SOC_TH_ALPM_BUCKET_COUNT(l80) = SOC_TH_ALPM_MAX_BKTS;if
(SOC_URPF_STATUS_GET(l80)){l338>>= 1;SOC_TH_ALPM_BUCKET_COUNT(l80)>>= 1;}
SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80) = SHR_BITALLOCSIZE(SOC_TH_ALPM_BUCKET_COUNT
(l80));SOC_TH_ALPM_VRF_BUCKET_BMAP(l80) = sal_alloc(
SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80),"alpm_vrf_shared_bucket_bitmap");if(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80) == NULL){return SOC_E_MEMORY;}sal_memset(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80),0,SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80));
soc_th_alpm_bucket_assign(l80,&l338,1,1);if(soc_property_get(l80,
"l3_alpm_vrf_share_bucket",0)&&soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_PARALLEL&&SOC_URPF_STATUS_GET(l80)){SOC_TH_ALPM_GLB_BUCKET_BMAP
(l80) = sal_alloc(SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80),"alpm_glb_shr_bkt_bmap");
if(SOC_TH_ALPM_GLB_BUCKET_BMAP(l80) == NULL){sal_free(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l80));return SOC_E_MEMORY;}sal_memset(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l80),0,SOC_TH_ALPM_BUCKET_BMAP_SIZE(l80));
soc_th_alpm_bucket_assign(l80,&l338,SOC_VRF_MAX(l80)+1,1);}else{
SOC_TH_ALPM_GLB_BUCKET_BMAP(l80) = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80);}return
SOC_E_NONE;}int soc_th_alpm_bucket_assign(int l80,int*l303,int vrf,int l128){
int l159,l215 = 1,l216 = 0;SHR_BITDCL*l339 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80)
;if(vrf == SOC_VRF_MAX(l80)+1){l339 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l80);}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l128)){l215 = 2;}for(l159 = 0;l159<
SOC_TH_ALPM_BUCKET_COUNT(l80);l159+= l215){SHR_BITTEST_RANGE(l339,l159,l215,
l216);if(0 == l216){break;}}if(l159 == SOC_TH_ALPM_BUCKET_COUNT(l80)){return
SOC_E_FULL;}SHR_BITSET_RANGE(l339,l159,l215);*l303 = l159;
SOC_TH_ALPM_BUCKET_NEXT_FREE(l80) = l159;return SOC_E_NONE;}int
soc_th_alpm_bucket_release(int l80,int l303,int vrf,int l128){int l215 = 1,
l216 = 0;SHR_BITDCL*l339 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80);if((l303<1)||(
l303>SOC_TH_ALPM_BUCKET_MAX_INDEX(l80))){return SOC_E_PARAM;}if(vrf == 
SOC_VRF_MAX(l80)+1){l339 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l80);}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l128)){l215 = 2;}SHR_BITTEST_RANGE(l339,l303,l215
,l216);if(!l216){return SOC_E_PARAM;}SHR_BITCLR_RANGE(l339,l303,l215);return
SOC_E_NONE;}int soc_th_alpm_bucket_is_assigned(int l80,int l340,int vrf,int
l106,int*l216){int l215 = 1;SHR_BITDCL*l339 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80
);if(vrf == SOC_VRF_MAX(l80)+1){l339 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l80);}if((
l340<1)||(l340>SOC_TH_ALPM_BUCKET_MAX_INDEX(l80))){return SOC_E_PARAM;}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l106)){l215 = 2;}SHR_BITTEST_RANGE(l339,l340,l215
,*l216);return SOC_E_NONE;}static void l79(int l80,void*l81,int index,l74 l82
){if(index&(0x8000)){l82[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l25));l82[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l27));l82[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l26));l82[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l28));if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int
l341;(void)soc_th_alpm_lpm_vrf_get(l80,l81,(int*)&l82[4],&l341);}else{l82[4] = 
0;};}else{if(index&0x1){l82[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l26));l82[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l28));l82[2] = 0;l82[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l80)))&&((
(l67[(l80)]->l42)!= NULL))){int l341;defip_entry_t l342;(void)
soc_th_alpm_lpm_ip4entry1_to_0(l80,l81,&l342,0);(void)soc_th_alpm_lpm_vrf_get
(l80,&l342,(int*)&l82[4],&l341);}else{l82[4] = 0;};}else{l82[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l25));l82[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l27));l82[2] = 0;l82[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int l341;(void)
soc_th_alpm_lpm_vrf_get(l80,l81,(int*)&l82[4],&l341);}else{l82[4] = 0;};}}}
static int l343(l74 l76,l74 l77){int l140;for(l140 = 0;l140<5;l140++){if(l76[
l140]<l77[l140]){return-1;}if(l76[l140]>l77[l140]){return 1;}}return(0);}
static void l344(int l80,void*l104,uint32 l345,uint32 l96,int l93){l74 l346;
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l104),(
l67[(l80)]->l29))){if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l104),(l67[(l80)]->l40))&&soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l104),(l67[(l80)]->l39))){l346[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l104),(l67[
(l80)]->l25));l346[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l104),(l67[(l80)]->l27));l346[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l104),(l67[
(l80)]->l26));l346[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l104),(l67[(l80)]->l28));if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(
l80)]->l41)!= NULL))){int l341;(void)soc_th_alpm_lpm_vrf_get(l80,l104,(int*)&
l346[4],&l341);}else{l346[4] = 0;};l95((l78[(l80)]),l343,l346,l93,l96,((
uint16)l345<<1)|(0x8000));}}else{if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l104),(l67[(l80)]->l39))){l346[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l104),(l67[
(l80)]->l25));l346[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l104),(l67[(l80)]->l27));l346[2] = 0;l346[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int l341;(void)
soc_th_alpm_lpm_vrf_get(l80,l104,(int*)&l346[4],&l341);}else{l346[4] = 0;};
l95((l78[(l80)]),l343,l346,l93,l96,((uint16)l345<<1));}if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l104),(l67[
(l80)]->l40))){l346[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80
,L3_DEFIPm)),(l104),(l67[(l80)]->l26));l346[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l104),(l67[
(l80)]->l28));l346[2] = 0;l346[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l80)))
&&(((l67[(l80)]->l42)!= NULL))){int l341;defip_entry_t l342;(void)
soc_th_alpm_lpm_ip4entry1_to_0(l80,l104,&l342,0);(void)
soc_th_alpm_lpm_vrf_get(l80,&l342,(int*)&l346[4],&l341);}else{l346[4] = 0;};
l95((l78[(l80)]),l343,l346,l93,l96,(((uint16)l345<<1)+1));}}}static void l347
(int l80,void*key_data,uint32 l345){l74 l346;int l93 = -1;int l137;uint16
index;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
key_data),(l67[(l80)]->l29))){l346[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l25));l346[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l27));l346[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l26));l346[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l28));if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))
){int l341;(void)soc_th_alpm_lpm_vrf_get(l80,key_data,(int*)&l346[4],&l341);}
else{l346[4] = 0;};index = (l345<<1)|(0x8000);}else{l346[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l25));l346[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l27));l346[2] = 0;l346[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int
l341;(void)soc_th_alpm_lpm_vrf_get(l80,key_data,(int*)&l346[4],&l341);}else{
l346[4] = 0;};index = l345;}l137 = l98((l78[(l80)]),l343,l346,l93,index);if(
SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\ndel  index: H %d error %d\n"),index,l137));}}static int l348(int l80,void*
key_data,int l93,int*l94){l74 l346;int l349;int l137;uint16 index = (0xFFFF);
l349 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
key_data),(l67[(l80)]->l29));if(l349){l346[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l25));l346[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l27));l346[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l26));l346[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l28));if((!(SOC_IS_HURRICANE(l80)))&&
(((l67[(l80)]->l41)!= NULL))){int l341;(void)soc_th_alpm_lpm_vrf_get(l80,
key_data,(int*)&l346[4],&l341);}else{l346[4] = 0;};}else{l346[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l25));l346[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(key_data),(l67[(l80)]->l27));l346[2] = 0;l346[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l80)))&&(((l67[(l80)]->l41)!= NULL))){int
l341;(void)soc_th_alpm_lpm_vrf_get(l80,key_data,(int*)&l346[4],&l341);}else{
l346[4] = 0;};}l137 = l90((l78[(l80)]),l343,l346,l93,&index);if(SOC_FAILURE(
l137)){*l94 = 0xFFFFFFFF;return(l137);}*l94 = index;return(SOC_E_NONE);}
static uint16 l83(uint8*l84,int l85){return(_shr_crc16b(0,l84,l85));}static
int l86(int unit,int l69,int l70,l73**l87){l73*l91;int index;if(l70>l69){
return SOC_E_MEMORY;}l91 = sal_alloc(sizeof(l73),"lpm_hash");if(l91 == NULL){
return SOC_E_MEMORY;}sal_memset(l91,0,sizeof(*l91));l91->unit = unit;l91->l69
= l69;l91->l70 = l70;l91->l71 = sal_alloc(l91->l70*sizeof(*(l91->l71)),
"hash_table");if(l91->l71 == NULL){sal_free(l91);return SOC_E_MEMORY;}l91->
l72 = sal_alloc(l91->l69*sizeof(*(l91->l72)),"link_table");if(l91->l72 == 
NULL){sal_free(l91->l71);sal_free(l91);return SOC_E_MEMORY;}for(index = 0;
index<l91->l70;index++){l91->l71[index] = (0xFFFF);}for(index = 0;index<l91->
l69;index++){l91->l72[index] = (0xFFFF);}*l87 = l91;return SOC_E_NONE;}static
int l88(l73*l89){if(l89!= NULL){sal_free(l89->l71);sal_free(l89->l72);
sal_free(l89);}return SOC_E_NONE;}static int l90(l73*l91,l75 l92,l74 entry,
int l93,uint16*l94){int l80 = l91->unit;uint16 l350;uint16 index;l350 = l83((
uint8*)entry,(32*5))%l91->l70;index = l91->l71[l350];;;while(index!= (0xFFFF)
){uint32 l81[SOC_MAX_MEM_FIELD_WORDS];l74 l82;int l351;l351 = (index&(0x7FFF)
)>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l351,l81));l79(l80,
l81,index,l82);if((*l92)(entry,l82) == 0){*l94 = (index&(0x7FFF))>>((index&(
0x8000))?1:0);;return(SOC_E_NONE);}index = l91->l72[index&(0x7FFF)];;};return
(SOC_E_NOT_FOUND);}static int l95(l73*l91,l75 l92,l74 entry,int l93,uint16 l96
,uint16 l97){int l80 = l91->unit;uint16 l350;uint16 index;uint16 l352;l350 = 
l83((uint8*)entry,(32*5))%l91->l70;index = l91->l71[l350];;;;l352 = (0xFFFF);
if(l96!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l81[SOC_MAX_MEM_FIELD_WORDS
];l74 l82;int l351;l351 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(
READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l351,l81));l79(l80,l81,index,l82);if((*l92)(
entry,l82) == 0){if(l97!= index){;if(l352 == (0xFFFF)){l91->l71[l350] = l97;
l91->l72[l97&(0x7FFF)] = l91->l72[index&(0x7FFF)];l91->l72[index&(0x7FFF)] = 
(0xFFFF);}else{l91->l72[l352&(0x7FFF)] = l97;l91->l72[l97&(0x7FFF)] = l91->
l72[index&(0x7FFF)];l91->l72[index&(0x7FFF)] = (0xFFFF);}};return(SOC_E_NONE)
;}l352 = index;index = l91->l72[index&(0x7FFF)];;}}l91->l72[l97&(0x7FFF)] = 
l91->l71[l350];l91->l71[l350] = l97;return(SOC_E_NONE);}static int l98(l73*
l91,l75 l92,l74 entry,int l93,uint16 l99){uint16 l350;uint16 index;uint16 l352
;l350 = l83((uint8*)entry,(32*5))%l91->l70;index = l91->l71[l350];;;l352 = (
0xFFFF);while(index!= (0xFFFF)){if(l99 == index){;if(l352 == (0xFFFF)){l91->
l71[l350] = l91->l72[l99&(0x7FFF)];l91->l72[l99&(0x7FFF)] = (0xFFFF);}else{
l91->l72[l352&(0x7FFF)] = l91->l72[l99&(0x7FFF)];l91->l72[l99&(0x7FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l352 = index;index = l91->l72[index&(0x7FFF)];;}
return(SOC_E_NOT_FOUND);}int soc_th_alpm_lpm_ip4entry0_to_0(int l80,void*l353
,void*l354,int l355){uint32 l138;l138 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l39));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l39),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l29));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l29),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l25));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l25),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l27));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l27),(l138));if(((l67[(l80)]->l15)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l15));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l15),(l138));}if(((l67[(l80)]->l17)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l17));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l17),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l19));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l19),(l138));}else{l138 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l33));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l33),(l138));}l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l35));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l35),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l37));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l37),(l138));if(((l67[(l80)]->l41)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l41));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l41),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l43));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l43),(l138));}if(((l67[(l80)]->l13)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l13));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l13),(l138));}if(l355){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l23));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l23),(l138));}l138 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l45));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l45),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l47));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l47),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l49),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l51));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l51),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l53));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l53),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l55));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l55),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l57));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l57),(l138));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry1_to_1(
int l80,void*l353,void*l354,int l355){uint32 l138;l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l40));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l40),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l30));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l30),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l26));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l26),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l28));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l28),(l138));if(((l67[(l80)]->l16)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l16));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l16),(l138));}if(((l67[(l80)]->l18)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l18));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l18),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l20));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l20),(l138));}else{l138 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l34));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l34),(l138));}l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l36));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l36),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l38));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l38),(l138));if(((l67[(l80)]->l42)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l42));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l42),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l44));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l44),(l138));}if(((l67[(l80)]->l14)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l14));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l14),(l138));}if(l355){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l24));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l24),(l138));}l138 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l46));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l46),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l48));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l48),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l50),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l52));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l52),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l54));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l54),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l56),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l58));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l58),(l138));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry0_to_1(
int l80,void*l353,void*l354,int l355){uint32 l138;l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l39));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l40),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l29));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l30),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l25));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l26),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l27));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l28),(l138));if(!SOC_IS_HURRICANE(l80)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l15));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l16),(l138));}if(((l67[(l80)]->l17)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l17));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l18),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l19));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l20),(l138));}else{l138 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l33));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l34),(l138));}l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l35));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l36),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l37));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l38),(l138));if(((l67[(l80)]->l41)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l41));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l42),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l43));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l44),(l138));}if(((l67[(l80)]->l13)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l13));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l14),(l138));}if(l355){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l23));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l24),(l138));}l138 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l45));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l46),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l47));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l48),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l50),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l51));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l52),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l53));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l54),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l55));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l56),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l57));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l58),(l138));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry1_to_0(
int l80,void*l353,void*l354,int l355){uint32 l138;l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l40));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l39),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l30));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l29),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l26));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l25),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l28));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l27),(l138));if(!SOC_IS_HURRICANE(l80)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l16));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l15),(l138));}if(((l67[(l80)]->l18)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l18));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l17),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l20));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l19),(l138));}else{l138 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l34));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l33),(l138));}l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l36));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l35),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l38));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l37),(l138));if(((l67[(l80)]->l42)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l42));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l41),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l44));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l43),(l138));}if(((l67[(l80)]->l14)!= NULL)){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l14));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l13),(l138));}if(l355){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l24));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l23),(l138));}l138 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l46));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l45),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l48));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l47),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l49),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l52));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l51),(l138));l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l353),(l67[(l80)]->l54));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[(l80)]->l53),(l138));l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[
(l80)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l354),(l67[(l80)]->l55),(l138));l138 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l353),(l67[(l80)]->l58));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l354),(l67[
(l80)]->l57),(l138));return(SOC_E_NONE);}static int l356(int l80,void*l81){
return(SOC_E_NONE);}void soc_th_alpm_lpm_state_dump(int l80){int l159;int l357
;l357 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l80)){return;}for(l159 = l357;l159>= 0;l159--){if((l159!= 
((3*(64+32+2+1))-1))&&((l9[(l80)][(l159)].l2) == -1)){continue;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l159,(l9[
(l80)][(l159)].l4),(l9[(l80)][(l159)].next),(l9[(l80)][(l159)].l2),(l9[(l80)]
[(l159)].l3),(l9[(l80)][(l159)].l5),(l9[(l80)][(l159)].l6)));}
COMPILER_REFERENCE(l356);}static int l358(int l80,int index,uint32*l81){int
l359;int l106;uint32 l360;uint32 l361;int l362;if(!SOC_URPF_STATUS_GET(l80)){
return(SOC_E_NONE);}if(soc_feature(l80,soc_feature_l3_defip_hole)){l359 = (
soc_mem_index_count(l80,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l80)){l359 = (
soc_mem_index_count(l80,L3_DEFIPm)>>1)+0x0400;}else{l359 = (
soc_mem_index_count(l80,L3_DEFIPm)>>1);}l106 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l29));if(((l67[(l80)]->l13)!= NULL)){soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l13),(0));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l14),(0));}l360 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80
,L3_DEFIPm)),(l81),(l67[(l80)]->l27));l361 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l28));if(!l106){if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l81),(l67[(l80)]->l39))){soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l37),((!l360)?1:0));}if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l40))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)
),(l81),(l67[(l80)]->l38),((!l361)?1:0));}}else{l362 = ((!l360)&&(!l361))?1:0
;l360 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81
),(l67[(l80)]->l39));l361 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l81),(l67[(l80)]->l40));if(l360&&l361){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l37),(l362));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l38),(l362));}}return l231(l80,MEM_BLOCK_ANY,
index+l359,index,l81);}static int l363(int l80,int l364,int l365){uint32 l81[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l80,MEM_BLOCK_ANY
,l364,l81));l344(l80,l81,l365,0x4000,0);SOC_IF_ERROR_RETURN(l231(l80,
MEM_BLOCK_ANY,l365,l364,l81));SOC_IF_ERROR_RETURN(l358(l80,l365,l81));do{int
l366 = soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l364),1);int l367 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l365),1);ALPM_TCAM_PIVOT(l80,l367<<
1) = ALPM_TCAM_PIVOT(l80,l366<<1);ALPM_TCAM_PIVOT(l80,(l367<<1)+1) = 
ALPM_TCAM_PIVOT(l80,(l366<<1)+1);if(ALPM_TCAM_PIVOT((l80),l367<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),l367<<1)) = l367<<1;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),(
l367<<1))),(l367<<1));}if(ALPM_TCAM_PIVOT((l80),(l367<<1)+1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),(l367<<1)+1)) = (l367<<1)+1;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),(
l367<<1)+1)),(l367<<1)+1);}ALPM_TCAM_PIVOT(l80,l366<<1) = NULL;
ALPM_TCAM_PIVOT(l80,(l366<<1)+1) = NULL;}while(0);return(SOC_E_NONE);}static
int l368(int l80,int l93,int l106){uint32 l81[SOC_MAX_MEM_FIELD_WORDS];int
l364;int l365;uint32 l369,l370;l365 = (l9[(l80)][(l93)].l3)+1;if(!l106){l364 = 
(l9[(l80)][(l93)].l3);SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,
l364,l81));l369 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l39));l370 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(l80)]->l40));if((l369 == 0)||(
l370 == 0)){l344(l80,l81,l365,0x4000,0);SOC_IF_ERROR_RETURN(l231(l80,
MEM_BLOCK_ANY,l365,l364,l81));SOC_IF_ERROR_RETURN(l358(l80,l365,l81));do{int
l366 = soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l364),1)*2;int l367 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l365),1)*2;if((l370)){l366++;}
ALPM_TCAM_PIVOT((l80),l367) = ALPM_TCAM_PIVOT((l80),l366);if(ALPM_TCAM_PIVOT(
(l80),l367)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),l367)) = l367;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),l367
)),l367);}ALPM_TCAM_PIVOT((l80),l366) = NULL;}while(0);l365--;}}l364 = (l9[(
l80)][(l93)].l2);if(l364!= l365){SOC_IF_ERROR_RETURN(l363(l80,l364,l365));
VRF_PIVOT_SHIFT_INC(l80,MAX_VRF_ID,l106);}(l9[(l80)][(l93)].l2)+= 1;(l9[(l80)
][(l93)].l3)+= 1;return(SOC_E_NONE);}static int l371(int l80,int l93,int l106
){uint32 l81[SOC_MAX_MEM_FIELD_WORDS];int l364;int l365;int l372;uint32 l369,
l370;l365 = (l9[(l80)][(l93)].l2)-1;if((l9[(l80)][(l93)].l5) == 0){(l9[(l80)]
[(l93)].l2) = l365;(l9[(l80)][(l93)].l3) = l365-1;return(SOC_E_NONE);}if((!
l106)&&((l9[(l80)][(l93)].l3)!= (l9[(l80)][(l93)].l2))){l364 = (l9[(l80)][(
l93)].l3);SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l364,l81));
l369 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81)
,(l67[(l80)]->l39));l370 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l80,L3_DEFIPm)),(l81),(l67[(l80)]->l40));if((l369 == 0)||(l370 == 0)){l372 = 
l364-1;SOC_IF_ERROR_RETURN(l363(l80,l372,l365));VRF_PIVOT_SHIFT_INC(l80,
MAX_VRF_ID,l106);l344(l80,l81,l372,0x4000,0);SOC_IF_ERROR_RETURN(l231(l80,
MEM_BLOCK_ANY,l372,l364,l81));SOC_IF_ERROR_RETURN(l358(l80,l372,l81));do{int
l366 = soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l364),1)*2;int l367 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l372),1)*2;if((l370)){l366++;}
ALPM_TCAM_PIVOT((l80),l367) = ALPM_TCAM_PIVOT((l80),l366);if(ALPM_TCAM_PIVOT(
(l80),l367)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),l367)) = l367;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),l367
)),l367);}ALPM_TCAM_PIVOT((l80),l366) = NULL;}while(0);}else{l344(l80,l81,
l365,0x4000,0);SOC_IF_ERROR_RETURN(l231(l80,MEM_BLOCK_ANY,l365,l364,l81));
SOC_IF_ERROR_RETURN(l358(l80,l365,l81));do{int l366 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l364),1);int l367 = 
soc_th_alpm_physical_idx((l80),L3_DEFIPm,(l365),1);ALPM_TCAM_PIVOT(l80,l367<<
1) = ALPM_TCAM_PIVOT(l80,l366<<1);ALPM_TCAM_PIVOT(l80,(l367<<1)+1) = 
ALPM_TCAM_PIVOT(l80,(l366<<1)+1);if(ALPM_TCAM_PIVOT((l80),l367<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),l367<<1)) = l367<<1;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),(
l367<<1))),(l367<<1));}if(ALPM_TCAM_PIVOT((l80),(l367<<1)+1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l80),(l367<<1)+1)) = (l367<<1)+1;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l80),(
l367<<1)+1)),(l367<<1)+1);}ALPM_TCAM_PIVOT(l80,l366<<1) = NULL;
ALPM_TCAM_PIVOT(l80,(l366<<1)+1) = NULL;}while(0);}}else{l364 = (l9[(l80)][(
l93)].l3);SOC_IF_ERROR_RETURN(l363(l80,l364,l365));VRF_PIVOT_SHIFT_INC(l80,
MAX_VRF_ID,l106);}(l9[(l80)][(l93)].l2)-= 1;(l9[(l80)][(l93)].l3)-= 1;return(
SOC_E_NONE);}static int l373(int l80,int l93,int l106,void*l81,int*l374){int
l375;int l376;int l377;int l378;int l364;uint32 l369,l370;int l137;if((l9[(
l80)][(l93)].l5) == 0){l378 = ((3*(64+32+2+1))-1);if((soc_th_alpm_mode_get(
l80) == SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l80) == 
SOC_ALPM_MODE_TCAM_ALPM)){if(l93<= (((3*(64+32+2+1))/3)-1)){l378 = (((3*(64+
32+2+1))/3)-1);}}while((l9[(l80)][(l378)].next)>l93){l378 = (l9[(l80)][(l378)
].next);}l376 = (l9[(l80)][(l378)].next);if(l376!= -1){(l9[(l80)][(l376)].l4)
= l93;}(l9[(l80)][(l93)].next) = (l9[(l80)][(l378)].next);(l9[(l80)][(l93)].
l4) = l378;(l9[(l80)][(l378)].next) = l93;(l9[(l80)][(l93)].l6) = ((l9[(l80)]
[(l378)].l6)+1)/2;(l9[(l80)][(l378)].l6)-= (l9[(l80)][(l93)].l6);(l9[(l80)][(
l93)].l2) = (l9[(l80)][(l378)].l3)+(l9[(l80)][(l378)].l6)+1;(l9[(l80)][(l93)]
.l3) = (l9[(l80)][(l93)].l2)-1;(l9[(l80)][(l93)].l5) = 0;}else if(!l106){l364
= (l9[(l80)][(l93)].l2);if((l137 = READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l364,l81)
)<0){return l137;}l369 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80
,L3_DEFIPm)),(l81),(l67[(l80)]->l39));l370 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l40));if((l369 == 0)||(l370 == 0)){*l374 = (l364<<1)+((l370 == 0)?1:0)
;return(SOC_E_NONE);}l364 = (l9[(l80)][(l93)].l3);if((l137 = READ_L3_DEFIPm(
l80,MEM_BLOCK_ANY,l364,l81))<0){return l137;}l369 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l81),(l67[(
l80)]->l39));l370 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(l81),(l67[(l80)]->l40));if((l369 == 0)||(l370 == 0)){*l374 = (
l364<<1)+((l370 == 0)?1:0);return(SOC_E_NONE);}}l377 = l93;while((l9[(l80)][(
l377)].l6) == 0){l377 = (l9[(l80)][(l377)].next);if(l377 == -1){l377 = l93;
break;}}while((l9[(l80)][(l377)].l6) == 0){l377 = (l9[(l80)][(l377)].l4);if(
l377 == -1){if((l9[(l80)][(l93)].l5) == 0){l375 = (l9[(l80)][(l93)].l4);l376 = 
(l9[(l80)][(l93)].next);if(-1!= l375){(l9[(l80)][(l375)].next) = l376;}if(-1
!= l376){(l9[(l80)][(l376)].l4) = l375;}}return(SOC_E_FULL);}}while(l377>l93)
{l376 = (l9[(l80)][(l377)].next);SOC_IF_ERROR_RETURN(l371(l80,l376,l106));(l9
[(l80)][(l377)].l6)-= 1;(l9[(l80)][(l376)].l6)+= 1;l377 = l376;}while(l377<
l93){SOC_IF_ERROR_RETURN(l368(l80,l377,l106));(l9[(l80)][(l377)].l6)-= 1;l375
= (l9[(l80)][(l377)].l4);(l9[(l80)][(l375)].l6)+= 1;l377 = l375;}(l9[(l80)][(
l93)].l5)+= 1;(l9[(l80)][(l93)].l6)-= 1;(l9[(l80)][(l93)].l3)+= 1;*l374 = (l9
[(l80)][(l93)].l3)<<((l106)?0:1);sal_memcpy(l81,soc_mem_entry_null(l80,
L3_DEFIPm),soc_mem_entry_words(l80,L3_DEFIPm)*4);return(SOC_E_NONE);}static
int l379(int l80,int l93,int l106,void*l81,int l380){int l375;int l376;int
l364;int l365;uint32 l381[SOC_MAX_MEM_FIELD_WORDS];uint32 l382[
SOC_MAX_MEM_FIELD_WORDS];uint32 l383[SOC_MAX_MEM_FIELD_WORDS];void*l384;int
l137;int l385,l222;l364 = (l9[(l80)][(l93)].l3);l365 = l380;if(!l106){l365>>= 
1;if((l137 = READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,l364,l381))<0){return l137;}if(
(l137 = READ_L3_DEFIP_AUX_TABLEm(l80,MEM_BLOCK_ANY,soc_th_alpm_physical_idx(
l80,L3_DEFIPm,l364,1),l382))<0){return l137;}if((l137 = 
READ_L3_DEFIP_AUX_TABLEm(l80,MEM_BLOCK_ANY,soc_th_alpm_physical_idx(l80,
L3_DEFIPm,l365,1),l383))<0){return l137;}l384 = (l365 == l364)?l381:l81;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(l381),(l67[
(l80)]->l40))){l222 = soc_mem_field32_get(l80,L3_DEFIP_AUX_TABLEm,l382,
BPM_LENGTH1f);if(l380&1){l137 = soc_th_alpm_lpm_ip4entry1_to_1(l80,l381,l384,
PRESERVE_HIT);soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,l383,BPM_LENGTH1f,
l222);}else{l137 = soc_th_alpm_lpm_ip4entry1_to_0(l80,l381,l384,PRESERVE_HIT)
;soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,l383,BPM_LENGTH0f,l222);}l385 = 
(l364<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),
(l381),(l67[(l80)]->l40),(0));}else{l222 = soc_mem_field32_get(l80,
L3_DEFIP_AUX_TABLEm,l382,BPM_LENGTH0f);if(l380&1){l137 = 
soc_th_alpm_lpm_ip4entry0_to_1(l80,l381,l384,PRESERVE_HIT);
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,l383,BPM_LENGTH1f,l222);}else{
l137 = soc_th_alpm_lpm_ip4entry0_to_0(l80,l381,l384,PRESERVE_HIT);
soc_mem_field32_set(l80,L3_DEFIP_AUX_TABLEm,l383,BPM_LENGTH0f,l222);}l385 = 
l364<<1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l80,L3_DEFIPm)),(
l381),(l67[(l80)]->l39),(0));(l9[(l80)][(l93)].l5)-= 1;(l9[(l80)][(l93)].l6)
+= 1;(l9[(l80)][(l93)].l3)-= 1;}l385 = soc_th_alpm_physical_idx(l80,L3_DEFIPm
,l385,0);l380 = soc_th_alpm_physical_idx(l80,L3_DEFIPm,l380,0);
ALPM_TCAM_PIVOT(l80,l380) = ALPM_TCAM_PIVOT(l80,l385);if(ALPM_TCAM_PIVOT(l80,
l380)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l80,l380)) = l380;
soc_th_alpm_lpm_move_bu_upd(l80,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l80,l380))
,l380);}ALPM_TCAM_PIVOT(l80,l385) = NULL;if((l137 = WRITE_L3_DEFIP_AUX_TABLEm
(l80,MEM_BLOCK_ALL,soc_th_alpm_physical_idx(l80,L3_DEFIPm,l365,1),l383))<0){
return l137;}if(l365!= l364){l344(l80,l384,l365,0x4000,0);if((l137 = l231(l80
,MEM_BLOCK_ANY,l365,l365,l384))<0){return l137;}if((l137 = l358(l80,l365,l384
))<0){return l137;}}l344(l80,l381,l364,0x4000,0);if((l137 = l231(l80,
MEM_BLOCK_ANY,l364,l364,l381))<0){return l137;}if((l137 = l358(l80,l364,l381)
)<0){return l137;}}else{(l9[(l80)][(l93)].l5)-= 1;(l9[(l80)][(l93)].l6)+= 1;(
l9[(l80)][(l93)].l3)-= 1;if(l365!= l364){if((l137 = READ_L3_DEFIPm(l80,
MEM_BLOCK_ANY,l364,l381))<0){return l137;}l344(l80,l381,l365,0x4000,0);if((
l137 = l231(l80,MEM_BLOCK_ANY,l365,l364,l381))<0){return l137;}if((l137 = 
l358(l80,l365,l381))<0){return l137;}}l380 = soc_th_alpm_physical_idx(l80,
L3_DEFIPm,l365,1);l385 = soc_th_alpm_physical_idx(l80,L3_DEFIPm,l364,1);
ALPM_TCAM_PIVOT(l80,l380<<1) = ALPM_TCAM_PIVOT(l80,l385<<1);ALPM_TCAM_PIVOT(
l80,l385<<1) = NULL;if(ALPM_TCAM_PIVOT(l80,l380<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l80,l380<<1)) = l380<<1;soc_th_alpm_lpm_move_bu_upd(l80,
PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l80,l380<<1)),l380<<1);}sal_memcpy(l381,
soc_mem_entry_null(l80,L3_DEFIPm),soc_mem_entry_words(l80,L3_DEFIPm)*4);l344(
l80,l381,l364,0x4000,0);if((l137 = l231(l80,MEM_BLOCK_ANY,l364,l364,l381))<0)
{return l137;}if((l137 = l358(l80,l364,l381))<0){return l137;}}if((l9[(l80)][
(l93)].l5) == 0){l375 = (l9[(l80)][(l93)].l4);assert(l375!= -1);l376 = (l9[(
l80)][(l93)].next);(l9[(l80)][(l375)].next) = l376;(l9[(l80)][(l375)].l6)+= (
l9[(l80)][(l93)].l6);(l9[(l80)][(l93)].l6) = 0;if(l376!= -1){(l9[(l80)][(l376
)].l4) = l375;}(l9[(l80)][(l93)].next) = -1;(l9[(l80)][(l93)].l4) = -1;(l9[(
l80)][(l93)].l2) = -1;(l9[(l80)][(l93)].l3) = -1;}return(l137);}int
soc_th_alpm_lpm_vrf_get(int unit,void*lpm_entry,int*vrf,int*l130){int vrf_id;
if(((l67[(unit)]->l43)!= NULL)){vrf_id = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l41));*l130 = vrf_id;
if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l43))){*vrf = vrf_id;}else if(!
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l45))){*vrf = SOC_L3_VRF_GLOBAL;*l130 = SOC_VRF_MAX(unit)+1;}
else{*vrf = SOC_L3_VRF_OVERRIDE;}}else{*vrf = SOC_L3_VRF_DEFAULT;}return(
SOC_E_NONE);}static int l100(int l80,int l106,void*entry,int*l112,int*vrf_id,
int*vrf){int l93;int l137;uint32 l138;int l386;int l135;if(l106){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l27));if((l137 = _ipmask2pfx(l138,&l93))<0){return(l137);}l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(l67
[(l80)]->l28));if(l93){if(l138!= 0xffffffff){return(SOC_E_PARAM);}l93+= 32;}
else{if((l137 = _ipmask2pfx(l138,&l93))<0){return(l137);}}l93+= 33;}else{l138
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(entry),(
l67[(l80)]->l27));if((l137 = _ipmask2pfx(l138,&l93))<0){return(l137);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l80,entry,&l386,&l137));if(vrf_id
!= NULL){*vrf_id = l386;}if(vrf!= NULL){*vrf = l137;}l135 = 
soc_th_alpm_mode_get(l80);switch(l386){case SOC_L3_VRF_GLOBAL:if((l135 == 
SOC_ALPM_MODE_PARALLEL)||(l135 == SOC_ALPM_MODE_TCAM_ALPM)){*l112 = l93+((3*(
64+32+2+1))/3);}else{*l112 = l93;}break;case SOC_L3_VRF_OVERRIDE:*l112 = l93+
2*((3*(64+32+2+1))/3);break;default:if((l135 == SOC_ALPM_MODE_PARALLEL)||(
l135 == SOC_ALPM_MODE_TCAM_ALPM)){*l112 = l93;}else{*l112 = l93+((3*(64+32+2+
1))/3);}break;}return(SOC_E_NONE);}static int l110(int l80,void*key_data,void
*l81,int*l111,int*l112,int*l106,int*vrf_id,int*vrf){int l137;int l128;int l94
;int l93 = 0;l128 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,
L3_DEFIPm)),(key_data),(l67[(l80)]->l29));if(l128){if(!(l128 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l80,L3_DEFIPm)),(key_data),(
l67[(l80)]->l30)))){return(SOC_E_PARAM);}}*l106 = l128;l100(l80,l128,key_data
,&l93,vrf_id,vrf);*l112 = l93;if(l348(l80,key_data,l93,&l94) == SOC_E_NONE){*
l111 = l94;if((l137 = READ_L3_DEFIPm(l80,MEM_BLOCK_ANY,(*l106)?*l111:(*l111>>
1),l81))<0){return l137;}return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}
static int l101(int l80){int l357;int l159;int l338;int l387;uint32 l388,l135
;if(!soc_feature(l80,soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}if((l135 = 
soc_property_get(l80,spn_L3_ALPM_ENABLE,0))){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l80,&l388));soc_reg_field_set(l80,
L3_DEFIP_RPF_CONTROLr,&l388,LPM_MODEf,1);if(l135 == 1){soc_reg_field_set(l80,
L3_DEFIP_RPF_CONTROLr,&l388,LOOKUP_MODEf,1);}else if(l135 == 2){
soc_reg_field_set(l80,L3_DEFIP_RPF_CONTROLr,&l388,LOOKUP_MODEf,0);
l3_alpm_ipv4_double_wide[l80] = soc_property_get(l80,
"l3_alpm_ipv4_double_wide",1);if(l3_alpm_ipv4_double_wide[l80]){
soc_reg_field_set(l80,L3_DEFIP_RPF_CONTROLr,&l388,
ENABLE_DOUBLE_WIDE_BUCKETS_FOR_IPV4f,1);}}else if(l135 == 3){
l3_alpm_ipv4_double_wide[l80] = 1;soc_reg_field_set(l80,L3_DEFIP_RPF_CONTROLr
,&l388,LOOKUP_MODEf,2);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l80,
l388));l388 = 0;if(SOC_URPF_STATUS_GET(l80)){soc_reg_field_set(l80,
L3_DEFIP_KEY_SELr,&l388,URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l80,
L3_DEFIP_KEY_SELr,&l388,URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l80,
L3_DEFIP_KEY_SELr,&l388,URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l80,
L3_DEFIP_KEY_SELr,&l388,URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_KEY_SELr(l80,l388));l388 = 0;if(l135 == 1||l135 == 3){if(
SOC_URPF_STATUS_GET(l80)){soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,
TCAM2_SELf,1);soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM3_SELf,1);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM4_SELf,2);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM5_SELf,2);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM6_SELf,3);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM7_SELf,3);}else{
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM4_SELf,1);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM5_SELf,1);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM6_SELf,1);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM7_SELf,1);}}else{if(
SOC_URPF_STATUS_GET(l80)){soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,
TCAM4_SELf,2);soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM5_SELf,2);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM6_SELf,2);
soc_reg_field_set(l80,L3_DEFIP_ALPM_CFGr,&l388,TCAM7_SELf,2);}}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_ALPM_CFGr(l80,l388));if(soc_property_get(
l80,spn_IPV6_LPM_128B_ENABLE,1)){uint32 l389 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l80,&l389));soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&
l389,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l389,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l389,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l80,L3_DEFIP_KEY_SELr,&l389,
V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l80,l389)
);}}l357 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l80);l387 = sizeof(l7)*(l357);if
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
l387,"LPM prefix info");if(NULL == (l9[(l80)])){sal_free(l67[l80]);l67[l80] = 
NULL;SOC_ALPM_LPM_UNLOCK(l80);return(SOC_E_MEMORY);}sal_memset((l9[(l80)]),0,
l387);for(l159 = 0;l159<l357;l159++){(l9[(l80)][(l159)].l2) = -1;(l9[(l80)][(
l159)].l3) = -1;(l9[(l80)][(l159)].l4) = -1;(l9[(l80)][(l159)].next) = -1;(l9
[(l80)][(l159)].l5) = 0;(l9[(l80)][(l159)].l6) = 0;}l338 = 
soc_mem_index_count(l80,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l80)){l338>>= 1;}if
((l135 == 1)||(l135 == 3)){(l9[(l80)][(((3*(64+32+2+1))-1))].l3) = (l338>>1)-
1;(l9[(l80)][(((((3*(64+32+2+1))/3)-1)))].l6) = l338>>1;(l9[(l80)][((((3*(64+
32+2+1))-1)))].l6) = (l338-(l9[(l80)][(((((3*(64+32+2+1))/3)-1)))].l6));}else
{(l9[(l80)][((((3*(64+32+2+1))-1)))].l6) = l338;}if((l78[(l80)])!= NULL){if(
l88((l78[(l80)]))<0){SOC_ALPM_LPM_UNLOCK(l80);return SOC_E_INTERNAL;}(l78[(
l80)]) = NULL;}if(l86(l80,l338*2,l338,&(l78[(l80)]))<0){SOC_ALPM_LPM_UNLOCK(
l80);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l80);return(SOC_E_NONE);}static
int l102(int l80){if(!soc_feature(l80,soc_feature_lpm_tcam)){return(
SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l80);if((l78[(l80)])!= NULL){l88((l78[(l80)
]));(l78[(l80)]) = NULL;}if((l9[(l80)]!= NULL)){sal_free(l67[l80]);l67[l80] = 
NULL;sal_free((l9[(l80)]));(l9[(l80)]) = NULL;}SOC_ALPM_LPM_UNLOCK(l80);
return(SOC_E_NONE);}static int l103(int l80,void*l104,int*l172){int l93;int
index;int l106;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];int l137 = SOC_E_NONE;int
l390 = 0;sal_memcpy(l81,soc_mem_entry_null(l80,L3_DEFIPm),soc_mem_entry_words
(l80,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l80);l137 = l110(l80,l104,l81,&index,&
l93,&l106,0,0);if(l137 == SOC_E_NOT_FOUND){l137 = l373(l80,l93,l106,l81,&
index);if(l137<0){SOC_ALPM_LPM_UNLOCK(l80);return(l137);}}else{l390 = 1;}*
l172 = index;if(l137 == SOC_E_NONE){if(!l106){if(index&1){l137 = 
soc_th_alpm_lpm_ip4entry0_to_1(l80,l104,l81,PRESERVE_HIT);}else{l137 = 
soc_th_alpm_lpm_ip4entry0_to_0(l80,l104,l81,PRESERVE_HIT);}if(l137<0){
SOC_ALPM_LPM_UNLOCK(l80);return(l137);}l104 = (void*)l81;index>>= 1;}
soc_th_alpm_lpm_state_dump(l80);LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_lpm_insert: %d %d\n"),index,l93));if(!l390){l344(l80,l104,index,
0x4000,0);}l137 = l231(l80,MEM_BLOCK_ANY,index,index,l104);if(l137>= 0){l137 = 
l358(l80,index,l104);}}SOC_ALPM_LPM_UNLOCK(l80);return(l137);}static int
soc_th_alpm_lpm_delete(int l80,void*key_data){int l93;int index;int l106;
uint32 l81[SOC_MAX_MEM_FIELD_WORDS];int l137 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(
l80);l137 = l110(l80,key_data,l81,&index,&l93,&l106,0,0);if(l137 == 
SOC_E_NONE){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"soc_alpm_lpm_delete: %d %d\n"),index,l93));l347(l80,key_data,index);l137 = 
l379(l80,l93,l106,l81,index);}soc_th_alpm_lpm_state_dump(l80);
SOC_ALPM_LPM_UNLOCK(l80);return(l137);}static int l113(int l80,void*key_data,
void*l81,int*l111){int l93;int l137;int l106;SOC_ALPM_LPM_LOCK(l80);l137 = 
l110(l80,key_data,l81,l111,&l93,&l106,0,0);SOC_ALPM_LPM_UNLOCK(l80);return(
l137);}static int l105(int unit,void*key_data,int l106,int l107,int l108,
defip_aux_scratch_entry_t*l109){uint32 l138;uint32 l284[4] = {0,0,0,0};int l93
= 0;int l137 = SOC_E_NONE;l138 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l39));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,VALIDf,l138);l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l29));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,MODEf
,l138);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm
)),(key_data),(l67[(unit)]->l63));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l109,ENTRY_TYPEf,0);l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l21));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,
GLOBAL_ROUTEf,l138);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l15));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l109,ECMPf,l138);l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l19));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,
ECMP_PTRf,l138);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(key_data),(l67[(unit)]->l33));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l109,NEXT_HOP_INDEXf,l138);l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l35));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,PRIf,
l138);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)
),(key_data),(l67[(unit)]->l37));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l109,RPEf,l138);l138 =soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l41));
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,VRFf,l138);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,DB_TYPEf,l107);l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l13));soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,
DST_DISCARDf,l138);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l11));soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l109,CLASS_IDf,l138);if(l106){l284[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l25));l284[3] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),(l67[(unit)]->l26));}else{l284[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l25));}soc_mem_field_set(unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)
l109,IP_ADDRf,(uint32*)l284);if(l106){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l27));if((l137 = _ipmask2pfx(l138,&l93))<0){return(l137);}l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l28));if(l93){if(l138!= 0xffffffff){return(SOC_E_PARAM);}l93+= 
32;}else{if((l137 = _ipmask2pfx(l138,&l93))<0){return(l137);}}}else{l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(key_data),
(l67[(unit)]->l27));if((l137 = _ipmask2pfx(l138,&l93))<0){return(l137);}}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,IP_LENGTHf,l93);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l109,REPLACE_LENf,l108);return
(SOC_E_NONE);}int _soc_th_alpm_aux_op(int l80,_soc_aux_op_t l391,
defip_aux_scratch_entry_t*l109,int l392,int*l151,int*tcam_index,int*bktid){
uint32 l388,l393;int l394;soc_timeout_t l395;int l137 = SOC_E_NONE;int l396 = 
0;if(l392){SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_SCRATCHm(l80,MEM_BLOCK_ANY,
0,l109));}l397:l388 = 0;switch(l391){case INSERT_PROPAGATE:l394 = 0;break;
case DELETE_PROPAGATE:l394 = 1;break;case PREFIX_LOOKUP:l394 = 2;break;case
HITBIT_REPLACE:l394 = 3;break;default:return SOC_E_PARAM;}soc_reg_field_set(
l80,L3_DEFIP_AUX_CTRLr,&l388,OPCODEf,l394);soc_reg_field_set(l80,
L3_DEFIP_AUX_CTRLr,&l388,STARTf,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l80,l388));soc_timeout_init(&l395,50000,5);l394 = 0;
do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l80,&l388));l394 = 
soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l388,DONEf);if(l394 == 1){l137 = 
SOC_E_NONE;break;}if(soc_timeout_check(&l395)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l80,&l388));l394 = soc_reg_field_get(l80,
L3_DEFIP_AUX_CTRLr,l388,DONEf);if(l394 == 1){l137 = SOC_E_NONE;}else{LOG_WARN
(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"unit %d : DEFIP AUX Operation timeout\n"),
l80));l137 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l137)){if(
soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l388,ERRORf)){soc_reg_field_set(l80,
L3_DEFIP_AUX_CTRLr,&l388,STARTf,0);soc_reg_field_set(l80,L3_DEFIP_AUX_CTRLr,&
l388,ERRORf,0);soc_reg_field_set(l80,L3_DEFIP_AUX_CTRLr,&l388,DONEf,0);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l80,l388));LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"unit %d: DEFIP AUX Operation encountered "
"parity error !!\n"),l80));l396++;if(SOC_CONTROL(l80)->alpm_bulk_retry){
sal_sem_take(SOC_CONTROL(l80)->alpm_bulk_retry,1000000);}if(l396<5){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"unit %d: Retry DEFIP AUX Operation..\n"),l80
));goto l397;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l80));return SOC_E_INTERNAL;}}if(l391 == PREFIX_LOOKUP){if(l151&&tcam_index){
*l151 = soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l388,HITf);*tcam_index = 
soc_reg_field_get(l80,L3_DEFIP_AUX_CTRLr,l388,BKT_INDEXf);SOC_IF_ERROR_RETURN
(READ_L3_DEFIP_AUX_CTRL_1r(l80,&l393));*bktid = ALPM_BKTID(soc_reg_field_get(
l80,L3_DEFIP_AUX_CTRL_1r,l393,BKT_PTRf),soc_reg_field_get(l80,
L3_DEFIP_AUX_CTRL_1r,l393,SUB_BKT_PTRf));}}}return l137;}static int l115(int
unit,int l106,void*lpm_entry,void*l116,void*l117,soc_mem_t l118,uint32 l119,
uint32*l398,int l121){uint32 l138;uint32 l284[4] = {0,0};int l93 = 0;int l137
= SOC_E_NONE;uint32 l120 = 0;sal_memset(l116,0,soc_mem_entry_words(unit,l118)
*4);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),
(lpm_entry),(l67[(unit)]->l23));soc_mem_field32_set(unit,l118,l116,HITf,l138)
;l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l39));soc_mem_field32_set(unit,l118,l116,VALIDf,l138
);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l15));soc_mem_field32_set(unit,l118,l116,ECMPf,l138)
;l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l19));soc_mem_field32_set(unit,l118,l116,ECMP_PTRf,
l138);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)
),(lpm_entry),(l67[(unit)]->l33));soc_mem_field32_set(unit,l118,l116,
NEXT_HOP_INDEXf,l138);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l35));soc_mem_field32_set(unit,
l118,l116,PRIf,l138);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l37));soc_mem_field32_set(unit,
l118,l116,RPEf,l138);l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l13));soc_mem_field32_set(unit,
l118,l116,DST_DISCARDf,l138);l138 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l59));
soc_mem_field32_set(unit,l118,l116,SRC_DISCARDf,l138);l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l11));soc_mem_field32_set(unit,l118,l116,CLASS_IDf,l138);
soc_mem_field32_set(unit,l118,l116,SUB_BKT_PTRf,ALPM_BKT_SIDX(l121));l284[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l25));if(l106){l284[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l26));}
soc_mem_field_set(unit,l118,(uint32*)l116,KEYf,(uint32*)l284);if(l106){l138 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l27));if((l137 = _ipmask2pfx(l138,&l93))<0){return(l137);}l138
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l28));if(l93){if(l138!= 0xffffffff){return(
SOC_E_PARAM);}l93+= 32;}else{if((l137 = _ipmask2pfx(l138,&l93))<0){return(
l137);}}}else{l138 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l27));if((l137 = _ipmask2pfx(l138,&l93)
)<0){return(l137);}}if((l93 == 0)&&(l284[0] == 0)&&(l284[1] == 0)){l120 = 1;}
if(l398!= NULL){*l398 = l120;}soc_mem_field32_set(unit,l118,l116,LENGTHf,l93)
;if(l117 == NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){
sal_memset(l117,0,soc_mem_entry_words(unit,l118)*4);sal_memcpy(l117,l116,
soc_mem_entry_words(unit,l118)*4);soc_mem_field32_set(unit,l118,l117,
DST_DISCARDf,0);soc_mem_field32_set(unit,l118,l117,RPEf,0);
soc_mem_field32_set(unit,l118,l117,SRC_DISCARDf,l119&SOC_ALPM_RPF_SRC_DISCARD
);soc_mem_field32_set(unit,l118,l117,DEFAULTROUTEf,l120);}return(SOC_E_NONE);
}static int l122(int unit,void*l116,soc_mem_t l118,int l106,int vrf,int l123,
int index,void*lpm_entry){uint32 l138;uint32 l284[4] = {0,0};uint32 l93 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_words(unit,L3_DEFIPm)*4);l138 = 
soc_mem_field32_get(unit,l118,l116,HITf);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l23),(l138));if(l106)
{soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry
),(l67[(unit)]->l24),(l138));}l138 = soc_mem_field32_get(unit,l118,l116,
VALIDf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l39),(l138));if(l106){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l40),(l138));}l138 = soc_mem_field32_get(unit,l118,l116,ECMPf)
;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry
),(l67[(unit)]->l15),(l138));l138 = soc_mem_field32_get(unit,l118,l116,
ECMP_PTRf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),
(lpm_entry),(l67[(unit)]->l19),(l138));l138 = soc_mem_field32_get(unit,l118,
l116,NEXT_HOP_INDEXf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l33),(l138));l138 = soc_mem_field32_get
(unit,l118,l116,PRIf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l35),(l138));l138 = soc_mem_field32_get
(unit,l118,l116,RPEf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l37),(l138));l138 = soc_mem_field32_get
(unit,l118,l116,DST_DISCARDf);soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l13),(l138));l138 = 
soc_mem_field32_get(unit,l118,l116,SRC_DISCARDf);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l59),(l138));l138 = soc_mem_field32_get(unit,l118,l116,
CLASS_IDf);soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),
(lpm_entry),(l67[(unit)]->l11),(l138));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l49),(ALPM_BKT_IDX(
l123)));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l51),(ALPM_BKT_SIDX(l123)));if(index>0){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l47),(index));}soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l31),(3));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l61),(1));if(l106){soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l29),(1));}
soc_mem_field_get(unit,l118,l116,KEYf,l284);if(l106){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l26),(l284[1]));}soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l25),(l284[0]));l138 = 
soc_mem_field32_get(unit,l118,l116,LENGTHf);if(l106){if(l138>= 32){l93 = 
0xffffffff;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),
(lpm_entry),(l67[(unit)]->l28),(l93));l93 = ~(((l138-32) == 32)?0:(0xffffffff
)>>(l138-32));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm
)),(lpm_entry),(l67[(unit)]->l27),(l93));}else{l93 = ~(0xffffffff>>l138);
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l28),(l93));}}else{assert(l138<= 32);l93 = ~(((l138) == 32)?0:
(0xffffffff)>>(l138));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,
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
soc_th_alpm_warmboot_pivot_add(int unit,int l106,void*lpm_entry,int l399,int
bktid){int l137 = SOC_E_NONE;uint32 key[2] = {0,0};alpm_pivot_t*l218 = NULL;
alpm_bucket_handle_t*l261 = NULL;int vrf_id = 0,vrf = 0;uint32 l400;trie_t*
l305 = NULL;uint32 prefix[5] = {0};int l120 = 0;SOC_IF_ERROR_RETURN(l145(unit
,l106,lpm_entry,prefix,&l400,&l120));SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&vrf_id,&vrf));l399 = 
soc_th_alpm_physical_idx(unit,L3_DEFIPm,l399,l106);l261 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l261 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l261,0,sizeof(*l261));
l218 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l218 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l261);return SOC_E_MEMORY;}sal_memset(l218,0
,sizeof(*l218));PIVOT_BUCKET_HANDLE(l218) = l261;if(l106){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l218));key[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l25));key[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l26));}else{trie_init
(_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l218));key[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l25));}PIVOT_BUCKET_INDEX(l218) = bktid;PIVOT_TCAM_INDEX(l218)
= l399;if(!(ALPM_PREFIX_IN_TCAM(unit,vrf_id))){if(l106 == 0){l305 = 
VRF_PIVOT_TRIE_IPV4(unit,vrf);if(l305 == NULL){trie_init(_MAX_KEY_LEN_48_,&
VRF_PIVOT_TRIE_IPV4(unit,vrf));l305 = VRF_PIVOT_TRIE_IPV4(unit,vrf);}}else{
l305 = VRF_PIVOT_TRIE_IPV6(unit,vrf);if(l305 == NULL){trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(unit,vrf));l305 = VRF_PIVOT_TRIE_IPV6(
unit,vrf);}}sal_memcpy(l218->key,prefix,sizeof(prefix));l218->len = l400;l137
= trie_insert(l305,l218->key,NULL,l218->len,(trie_node_t*)l218);if(
SOC_FAILURE(l137)){sal_free(l261);sal_free(l218);return l137;}}
ALPM_TCAM_PIVOT(unit,l399) = l218;PIVOT_BUCKET_VRF(l218) = vrf;
PIVOT_BUCKET_IPV6(l218) = l106;PIVOT_BUCKET_ENT_CNT_UPDATE(l218);if(key[0] == 
0&&key[1] == 0){PIVOT_BUCKET_DEF(l218) = TRUE;}VRF_PIVOT_REF_INC(unit,vrf,
l106);return l137;}static int l401(int unit,int l106,void*lpm_entry,void*l116
,soc_mem_t l118,int l399,int bktid,int l402){int l403;int vrf;int l137 = 
SOC_E_NONE;int l120 = 0;uint32 prefix[5] = {0,0,0,0,0};uint32 l150;
defip_entry_t l404;trie_t*l262 = NULL;trie_t*l258 = NULL;trie_node_t*l253 = 
NULL;payload_t*l405 = NULL;payload_t*l282 = NULL;alpm_pivot_t*pivot_pyld = 
NULL;if((NULL == lpm_entry)||(NULL == l116)){return SOC_E_PARAM;}if(l106){if(
!(l106 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l30)))){return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l403,&vrf));l118 = (l106)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(l122(unit,l116
,l118,l106,l403,bktid,l399,&l404));l137 = l145(unit,l106,&l404,prefix,&l150,&
l120);if(SOC_FAILURE(l137)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"prefix create failed\n")));return l137;}pivot_pyld = ALPM_TCAM_PIVOT(unit,
l399);l262 = PIVOT_BUCKET_TRIE(pivot_pyld);l405 = sal_alloc(sizeof(payload_t)
,"Payload for Key");if(NULL == l405){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
unit,"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}
l282 = sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == 
l282){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node\n")));sal_free(l405);return
SOC_E_MEMORY;}sal_memset(l405,0,sizeof(*l405));sal_memset(l282,0,sizeof(*l282
));l405->key[0] = prefix[0];l405->key[1] = prefix[1];l405->key[2] = prefix[2]
;l405->key[3] = prefix[3];l405->key[4] = prefix[4];l405->len = l150;l405->
index = l402;sal_memcpy(l282,l405,sizeof(*l405));l137 = trie_insert(l262,
prefix,NULL,l150,(trie_node_t*)l405);if(SOC_FAILURE(l137)){goto l406;}if(l106
){l258 = VRF_PREFIX_TRIE_IPV6(unit,vrf);}else{l258 = VRF_PREFIX_TRIE_IPV4(
unit,vrf);}if(!l120){l137 = trie_insert(l258,prefix,NULL,l150,(trie_node_t*)
l282);if(SOC_FAILURE(l137)){goto l407;}}return l137;l407:(void)trie_delete(
l262,prefix,l150,&l253);l405 = (payload_t*)l253;l406:sal_free(l405);sal_free(
l282);return l137;}static int l408(int unit,int l128,int vrf,int l140,int
bktid){int l137 = SOC_E_NONE;uint32 l150;uint32 key[2] = {0,0};trie_t*l409 = 
NULL;payload_t*l294 = NULL;defip_entry_t*lpm_entry = NULL;lpm_entry = 
sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"unable to allocate memory for LPM entry\n")));return SOC_E_MEMORY;}l124(unit
,key,0,vrf,l128,lpm_entry,0,1);if(vrf == SOC_VRF_MAX(unit)+1){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l21),(1));}else{soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l53),(1));}
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l49),(ALPM_BKT_IDX(bktid)));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l51),(ALPM_BKT_SIDX
(bktid)));if(l128 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(unit,vrf) = lpm_entry;
trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(unit,vrf));l409 = 
VRF_PREFIX_TRIE_IPV4(unit,vrf);}else{VRF_TRIE_DEFAULT_ROUTE_IPV6(unit,vrf) = 
lpm_entry;trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(unit,vrf));l409 = 
VRF_PREFIX_TRIE_IPV6(unit,vrf);}l294 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l294 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l294,0,sizeof(*l294));l150 = 0;l294->key[0] = key[0]
;l294->key[1] = key[1];l294->len = l150;l137 = trie_insert(l409,key,NULL,l150
,&(l294->node));if(SOC_FAILURE(l137)){sal_free(l294);return l137;}
VRF_TRIE_INIT_DONE(unit,vrf,l128,1);return l137;}int
soc_th_alpm_warmboot_prefix_insert(int unit,int l106,void*lpm_entry,void*l116
,int l399,int bktid,int l402){int l403;int vrf;int l137 = SOC_E_NONE;
soc_mem_t l118;l399 = soc_th_alpm_physical_idx(unit,L3_DEFIPm,l399,l106);l118
= (l106)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l403,&vrf));if(ALPM_PREFIX_IN_TCAM(
unit,l403)){return(l137);}if(!VRF_TRIE_INIT_COMPLETED(unit,vrf,l106)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d is not initialized\n"),
vrf));l137 = l408(unit,l106,vrf,l399,bktid);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d/%d trie init \n""failed\n"),vrf,l106
));return l137;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d/%d trie init completed\n"),vrf,l106));}l137 = l401(unit,l106,
lpm_entry,l116,l118,l399,bktid,l402);if(l137!= SOC_E_NONE){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"unit %d : Route Insertion Failed :%s\n"),
unit,soc_errmsg(l137)));return(l137);}soc_th_alpm_bu_upd(unit,bktid,l399,vrf,
l106,1);VRF_TRIE_ROUTES_INC(unit,vrf,l106);return(l137);}int
soc_th_alpm_warmboot_bucket_bitmap_set(int l80,int vrf,int l128,int l208){int
l215 = 1;SHR_BITDCL*l339 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l80);if(vrf == 
SOC_VRF_MAX(l80)+1){l339 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l80);}if(
SOC_TH_ALPM_SCALE_CHECK(l80,l128)){l215 = 2;}SHR_BITSET_RANGE(l339,l208,l215)
;return SOC_E_NONE;}int soc_th_alpm_warmboot_lpm_reinit_done(int unit){int
l140;int l410 = ((3*(64+32+2+1))-1);int l411;int l338 = soc_mem_index_count(
unit,L3_DEFIPm);if(SOC_URPF_STATUS_GET(unit)){l338>>= 1;}l411 = 
soc_th_alpm_mode_get(unit);if(l411 == SOC_ALPM_MODE_COMBINED){(l9[(unit)][(((
3*(64+32+2+1))-1))].l4) = -1;for(l140 = ((3*(64+32+2+1))-1);l140>-1;l140--){
if(-1 == (l9[(unit)][(l140)].l2)){continue;}(l9[(unit)][(l140)].l4) = l410;(
l9[(unit)][(l410)].next) = l140;(l9[(unit)][(l410)].l6) = (l9[(unit)][(l140)]
.l2)-(l9[(unit)][(l410)].l3)-1;l410 = l140;}(l9[(unit)][(l410)].next) = -1;(
l9[(unit)][(l410)].l6) = l338-(l9[(unit)][(l410)].l3)-1;}else if(l411 == 1){(
l9[(unit)][(((3*(64+32+2+1))-1))].l4) = -1;for(l140 = ((3*(64+32+2+1))-1);
l140>(((3*(64+32+2+1))-1)/3);l140--){if(-1 == (l9[(unit)][(l140)].l2)){
continue;}(l9[(unit)][(l140)].l4) = l410;(l9[(unit)][(l410)].next) = l140;(l9
[(unit)][(l410)].l6) = (l9[(unit)][(l140)].l2)-(l9[(unit)][(l410)].l3)-1;l410
= l140;}(l9[(unit)][(l410)].next) = -1;(l9[(unit)][(l410)].l6) = l338-(l9[(
unit)][(l410)].l3)-1;l410 = (((3*(64+32+2+1))-1)/3);(l9[(unit)][((((3*(64+32+
2+1))-1)/3))].l4) = -1;for(l140 = ((((3*(64+32+2+1))-1)/3)-1);l140>-1;l140--)
{if(-1 == (l9[(unit)][(l140)].l2)){continue;}(l9[(unit)][(l140)].l4) = l410;(
l9[(unit)][(l410)].next) = l140;(l9[(unit)][(l410)].l6) = (l9[(unit)][(l140)]
.l2)-(l9[(unit)][(l410)].l3)-1;l410 = l140;}(l9[(unit)][(l410)].next) = -1;(
l9[(unit)][(l410)].l6) = (l338>>1)-(l9[(unit)][(l410)].l3)-1;}return(
SOC_E_NONE);}int soc_th_alpm_warmboot_lpm_reinit(int unit,int l106,int l140,
void*lpm_entry){int l112;defip_entry_t*l412;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l39))||soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,
L3_DEFIPm)),(lpm_entry),(l67[(unit)]->l40))){l344(unit,lpm_entry,l140,0x4000,
0);}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(
lpm_entry),(l67[(unit)]->l39))){SOC_IF_ERROR_RETURN(l100(unit,l106,lpm_entry,
&l112,0,0));if((l9[(unit)][(l112)].l5) == 0){(l9[(unit)][(l112)].l2) = l140;(
l9[(unit)][(l112)].l3) = l140;}else{(l9[(unit)][(l112)].l3) = l140;}(l9[(unit
)][(l112)].l5)++;if(l106){return(SOC_E_NONE);}}else{if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry)
,(l67[(unit)]->l40))){l412 = sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");
soc_th_alpm_lpm_ip4entry1_to_0(unit,lpm_entry,l412,TRUE);SOC_IF_ERROR_RETURN(
l100(unit,l106,l412,&l112,0,0));if((l9[(unit)][(l112)].l5) == 0){(l9[(unit)][
(l112)].l2) = l140;(l9[(unit)][(l112)].l3) = l140;}else{(l9[(unit)][(l112)].
l3) = l140;}sal_free(l412);(l9[(unit)][(l112)].l5)++;}}return(SOC_E_NONE);}
void soc_th_alpm_dbg_urpf(int l80){int l137 = SOC_E_NONE;soc_mem_t l118;int
l159,l141;int index;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];uint32 l413[
SOC_MAX_MEM_FIELD_WORDS];int l414 = 0,l415 = 0;if(!SOC_URPF_STATUS_GET(l80)){
LOG_CLI((BSL_META_U(l80,"Only supported in uRPF mode\n")));return;}for(l141 = 
0;l141<= SOC_TH_MAX_ALPM_BUCKETS/2;l141++){index = 
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,ALPM_BKTID(l141,0));l118 = 
_soc_tomahawk_alpm_bkt_view_get(l80,index);for(l159 = 0;l159<
SOC_TH_MAX_BUCKET_ENTRIES-1;l159++){l137 = _soc_th_alpm_mem_index(l80,l118,
l141,l159,0,&index);if(SOC_FAILURE(l137)){continue;}sal_memset(l81,0,sizeof(
uint32)*SOC_MAX_MEM_FIELD_WORDS);sal_memset(l413,0,sizeof(uint32)*
SOC_MAX_MEM_FIELD_WORDS);l137 = soc_mem_read(l80,l118,MEM_BLOCK_ANY,index,l81
);if(SOC_FAILURE(l137)){continue;}if(soc_mem_field32_get(l80,l118,l81,VALIDf)
== 0){continue;}soc_mem_field32_set(l80,l118,l81,HITf,0);l137 = soc_mem_read(
l80,l118,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l80,index),l413);if(SOC_FAILURE
(l137)){continue;}soc_mem_field32_set(l80,l118,l413,HITf,0);
soc_mem_field32_set(l80,l118,l413,RPEf,0);if(sal_memcmp(l81,l413,sizeof(
uint32)*SOC_MAX_MEM_FIELD_WORDS)!= 0){LOG_CLI((BSL_META_U(l80,
"uRPF sanity check failed: bucket %4d mem %s index %d uRPF index %d\n"),l141,
SOC_MEM_NAME(l80,l118),index,_soc_th_alpm_rpf_entry(l80,index)));l415++;}else
{l414++;}}}LOG_CLI((BSL_META_U(l80,
"uRPF Sanity check finished, good %d bad %d\n"),l414,l415));return;}typedef
struct l416{int v4;int v6_64;int v6_128;int l417;int l418;int l419;int l216;}
l420;typedef enum l421{l422 = 0,l423,l424,l425,l426,l427}l428;static void l429
(int l80,alpm_vrf_counter_t*l430){l430->v4 = soc_mem_index_count(l80,
L3_DEFIPm)*2;l430->v6_128 = soc_mem_index_count(l80,L3_DEFIP_PAIR_128m);if(
soc_property_get(l80,spn_IPV6_LPM_128B_ENABLE,1)){l430->v6_64 = l430->v6_128;
}else{l430->v6_64 = l430->v4>>1;}if(SOC_URPF_STATUS_GET(l80)){l430->v4>>= 1;
l430->v6_128>>= 1;l430->v6_64>>= 1;}}static void l431(int l80,int vrf_id,
alpm_vrf_handle_t*l432,l428 l433){alpm_vrf_counter_t*l434;int l159,l435,l436,
l437;int l390 = 0;alpm_vrf_counter_t l430;switch(l433){case l422:LOG_CLI((
BSL_META_U(l80,"\nAdd Counter:\n")));break;case l423:LOG_CLI((BSL_META_U(l80,
"\nDelete Counter:\n")));break;case l424:LOG_CLI((BSL_META_U(l80,
"\nInternal Debug Counter - 1:\n")));break;case l425:l429(l80,&l430);LOG_CLI(
(BSL_META_U(l80,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l430.
v4,l430.v6_64,l430.v6_128));break;case l426:LOG_CLI((BSL_META_U(l80,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l427:LOG_CLI((
BSL_META_U(l80,"\nInternal Debug Counter - LPM Full:\n")));break;default:
break;}LOG_CLI((BSL_META_U(l80,
"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((BSL_META_U(l80
,"-----------------------------------------------\n")));l435 = l436 = l437 = 
0;for(l159 = 0;l159<MAX_VRF_ID+1;l159++){int l438,l439,l440;if(l432[l159].
init_done == 0&&l159!= MAX_VRF_ID){continue;}if(vrf_id!= -1&&vrf_id!= l159){
continue;}l390 = 1;switch(l433){case l422:l434 = &l432[l159].add;break;case
l423:l434 = &l432[l159].del;break;case l424:l434 = &l432[l159].bkt_split;
break;case l426:l434 = &l432[l159].lpm_shift;break;case l427:l434 = &l432[
l159].lpm_full;break;case l425:l434 = &l432[l159].pivot_used;break;default:
l434 = &l432[l159].pivot_used;break;}l438 = l434->v4;l439 = l434->v6_64;l440 = 
l434->v6_128;l435+= l438;l436+= l439;l437+= l440;do{LOG_CLI((BSL_META_U(l80,
"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l159 == MAX_VRF_ID?-1:l159),(l438),(
l439),(l440),((l438+l439+l440)),(l159) == MAX_VRF_ID?"GHi":(l159) == 
SOC_VRF_MAX(l80)+1?"GLo":""));}while(0);}if(l390 == 0){LOG_CLI((BSL_META_U(
l80,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l80,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l80,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l435),(l436),(l437),((l435+
l436+l437))));}while(0);}return;}int soc_th_alpm_debug_show(int l80,int vrf_id
,uint32 flags){int l159,l441,l390 = 0;l420*l442;l420 l168;l420 l443;if(vrf_id
>(SOC_VRF_MAX(l80)+1)){return SOC_E_PARAM;}l441 = MAX_VRF_ID*sizeof(l420);
l442 = sal_alloc(l441,"_alpm_dbg_cnt");if(l442 == NULL){return SOC_E_MEMORY;}
sal_memset(l442,0,l441);l168.v4 = ALPM_IPV4_BKT_COUNT;l168.v6_64 = 
ALPM_IPV6_64_BKT_COUNT;l168.v6_128 = ALPM_IPV6_128_BKT_COUNT;if((
soc_th_alpm_mode_get(l80) == SOC_ALPM_MODE_COMBINED||soc_th_alpm_mode_get(l80
) == SOC_ALPM_MODE_TCAM_ALPM)&&!SOC_URPF_STATUS_GET(l80)){l168.v6_64<<= 1;
l168.v6_128<<= 1;}LOG_CLI((BSL_META_U(l80,"\nBucket Occupancy:\n")));if(flags
&(SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l80,
"\nPivot/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l80,"-----------------------------------------------\n")));}for(
l159 = 0;l159<MAX_PIVOT_COUNT;l159++){alpm_pivot_t*l444 = ALPM_TCAM_PIVOT(l80
,l159);if(l444!= NULL){l420*l445;int vrf = PIVOT_BUCKET_VRF(l444);if(vrf<0||
vrf>(SOC_VRF_MAX(l80)+1)){continue;}if(vrf_id!= -1&&vrf_id!= vrf){continue;}
if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l390 = 1;do{LOG_CLI((BSL_META_U(l80,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l159,vrf,PIVOT_BUCKET_MIN(l444),
PIVOT_BUCKET_MAX(l444),PIVOT_BUCKET_COUNT(l444),PIVOT_BUCKET_DEF(l444)?"Def":
(vrf) == SOC_VRF_MAX(l80)+1?"GLo":""));}while(0);}l445 = &l442[vrf];if(
PIVOT_BUCKET_IPV6(l444) == L3_DEFIP_MODE_128){l445->v6_128+= 
PIVOT_BUCKET_COUNT(l444);l445->l419+= l168.v6_128;}else if(PIVOT_BUCKET_IPV6(
l444) == L3_DEFIP_MODE_64){l445->v6_64+= PIVOT_BUCKET_COUNT(l444);l445->l418
+= l168.v6_64;}else{l445->v4+= PIVOT_BUCKET_COUNT(l444);l445->l417+= l168.v4;
}l445->l216 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l390 == 0){
LOG_CLI((BSL_META_U(l80,"%9s\n"),"Specific VRF not found"));}}sal_memset(&
l443,0,sizeof(l443));l390 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){
LOG_CLI((BSL_META_U(l80,"\n      VRF  v4      v6-64   v6-128  |   Total\n")))
;LOG_CLI((BSL_META_U(l80,"-----------------------------------------------\n")
));for(l159 = 0;l159<MAX_VRF_ID;l159++){l420*l445;if(l442[l159].l216!= TRUE){
continue;}if(vrf_id!= -1&&vrf_id!= l159){continue;}l390 = 1;l445 = &l442[l159
];do{(&l443)->v4+= (l445)->v4;(&l443)->l417+= (l445)->l417;(&l443)->v6_64+= (
l445)->v6_64;(&l443)->l418+= (l445)->l418;(&l443)->v6_128+= (l445)->v6_128;(&
l443)->l419+= (l445)->l419;}while(0);do{LOG_CLI((BSL_META_U(l80,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l159),(l445
->l417)?(l445->v4)*100/(l445->l417):0,(l445->l417)?(l445->v4)*1000/(l445->
l417)%10:0,(l445->l418)?(l445->v6_64)*100/(l445->l418):0,(l445->l418)?(l445->
v6_64)*1000/(l445->l418)%10:0,(l445->l419)?(l445->v6_128)*100/(l445->l419):0,
(l445->l419)?(l445->v6_128)*1000/(l445->l419)%10:0,((l445->l417+l445->l418+
l445->l419))?((l445->v4+l445->v6_64+l445->v6_128))*100/((l445->l417+l445->
l418+l445->l419)):0,((l445->l417+l445->l418+l445->l419))?((l445->v4+l445->
v6_64+l445->v6_128))*1000/((l445->l417+l445->l418+l445->l419))%10:0,(l159) == 
SOC_VRF_MAX(l80)+1?"GLo":""));}while(0);}if(l390 == 0){LOG_CLI((BSL_META_U(
l80,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l80,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l80,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l443)->l417)?((&l443)->v4)*100/((&l443)->l417):0,((&l443)->l417)?((&l443)->v4
)*1000/((&l443)->l417)%10:0,((&l443)->l418)?((&l443)->v6_64)*100/((&l443)->
l418):0,((&l443)->l418)?((&l443)->v6_64)*1000/((&l443)->l418)%10:0,((&l443)->
l419)?((&l443)->v6_128)*100/((&l443)->l419):0,((&l443)->l419)?((&l443)->
v6_128)*1000/((&l443)->l419)%10:0,(((&l443)->l417+(&l443)->l418+(&l443)->l419
))?(((&l443)->v4+(&l443)->v6_64+(&l443)->v6_128))*100/(((&l443)->l417+(&l443)
->l418+(&l443)->l419)):0,(((&l443)->l417+(&l443)->l418+(&l443)->l419))?(((&
l443)->v4+(&l443)->v6_64+(&l443)->v6_128))*1000/(((&l443)->l417+(&l443)->l418
+(&l443)->l419))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l431(l80,vrf_id,alpm_vrf_handle[l80],l425);}if(flags&
SOC_ALPM_DEBUG_SHOW_FLAG_CNT){l431(l80,vrf_id,alpm_vrf_handle[l80],l422);l431
(l80,vrf_id,alpm_vrf_handle[l80],l423);}if(flags&
SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG){l431(l80,vrf_id,alpm_vrf_handle[l80],l424);
l431(l80,vrf_id,alpm_vrf_handle[l80],l427);l431(l80,vrf_id,alpm_vrf_handle[
l80],l426);}sal_free(l442);return SOC_E_NONE;}int soc_th_alpm_debug_brief_show
(int l80,int l123,int index){int l159,l313,l137 = SOC_E_NONE;uint32 l388,l446
= 0,l135 = 0,l227,l447,l448,l449;int l450 = 0,l451;int l452 = 0,l453;int l454
= 0,l455;int l456 = 0,l457;char*l458 = NULL;defip_entry_t*lpm_entry;
defip_pair_128_entry_t*l459;int l106,bkt_ptr,l460,l461,l462;int*l463 = NULL;
int l464 = 0,l465 = 0;soc_mem_t l173;int l466 = 0,l467 = 0,l468 = 0,l469 = 0,
l470;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];soc_field_t l471,l472,l473,l474;char
*l475[] = {"LPM","ALPM-Combined","ALPM-Parallel","ALPM-TCAM_ALPM",
"ALPM-Invalid"};SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l80,&l388));
l446 = soc_reg_field_get(l80,L3_DEFIP_RPF_CONTROLr,l388,LPM_MODEf);if(l446!= 
0){l135 = soc_reg_field_get(l80,L3_DEFIP_RPF_CONTROLr,l388,LOOKUP_MODEf);}
l227 = soc_reg_field_get(l80,L3_DEFIP_RPF_CONTROLr,l388,DEFIP_RPF_ENABLEf);
LOG_CLI(("\n"));LOG_CLI(("%-24s%s\n","Mode:",l475[l446+l135]));LOG_CLI((
"%-24s%s\n","uRPF:",l227?"Enable":"Disable"));l447 = !!soc_mem_index_count(
l80,L3_DEFIP_PAIR_128m);LOG_CLI(("%-24s%d\n","IPv6 Bits:",l447?128:64));if(
l446 == 0){return SOC_E_NONE;}if(l135 == 0&&!l227){l448 = soc_reg_field_get(
l80,L3_DEFIP_RPF_CONTROLr,l388,ENABLE_DOUBLE_WIDE_BUCKETS_FOR_IPV4f);}else if
(l135 == 2){l448 = 1;}else{l448 = 0;}LOG_CLI(("%-24s%s\n","IPv4 Wide:",l448?
"Double":"Single"));if(l135 == 0&&!l227){l448 = 1;}else if(l135 == 2){l448 = 
1;}else{l448 = 0;}LOG_CLI(("%-24s%s\n","IPv6 Wide:",l448?"Double":"Single"));
SOC_IF_ERROR_RETURN(READ_ISS_BANK_CONFIGr(l80,&l388));l449 = 
soc_reg_field_get(l80,ISS_BANK_CONFIGr,l388,ALPM_ENTRY_BANK_CONFIGf);LOG_CLI(
("%-24s%d(0x%x)\n","Banks:",_shr_popcount(l449),l449));l451 = 2*
soc_mem_index_count(l80,L3_DEFIPm);l453 = l447?soc_mem_index_count(l80,
L3_DEFIP_PAIR_128m):soc_mem_index_count(l80,L3_DEFIPm);l455 = 
soc_mem_index_count(l80,L3_DEFIP_PAIR_128m);l457 = SOC_TH_MAX_ALPM_BUCKETS;if
(l227){l451>>= 1;l453>>= 1;l455>>= 1;l457>>= 1;}l463 = sal_alloc(l457*sizeof(
int),"bktref");if(l463 == NULL){return SOC_E_MEMORY;}sal_memset(l463,0,l457*
sizeof(int));l173 = L3_DEFIPm;l470 = WORDS2BYTES(soc_mem_entry_words(l80,l173
))*soc_mem_index_count(l80,l173);l458 = soc_cm_salloc(l80,l470,"lpm_tbl");if(
l458 == NULL){l137 = SOC_E_MEMORY;goto l476;}sal_memset(l458,0,l470);if(l451
&&soc_mem_read_range(l80,l173,MEM_BLOCK_ANY,soc_mem_index_min(l80,l173),
soc_mem_index_max(l80,l173),l458)<0){l137 = (SOC_E_INTERNAL);goto l476;}for(
l159 = 0;l159<(l451/2);l159++){lpm_entry = soc_mem_table_idx_to_pointer(l80,
l173,defip_entry_t*,l458,l159);l106 = soc_mem_field32_get(l80,l173,lpm_entry,
MODE0f);for(l313 = 0;l313<(l106?1:2);l313++){if(l313 == 1){l471 = VALID1f;}
else{l471 = VALID0f;}if(!soc_mem_field32_get(l80,l173,lpm_entry,l471)){
continue;}if(l106){l452++;}else{l450++;}if(l313 == 0){l473 = GLOBAL_HIGH0f;
l472 = ALG_BKT_PTR0f;l474 = GLOBAL_ROUTE0f;}else{l473 = GLOBAL_HIGH1f;l472 = 
ALG_BKT_PTR1f;l474 = GLOBAL_ROUTE1f;}l460 = soc_mem_field32_get(l80,l173,
lpm_entry,l473);l461 = soc_mem_field32_get(l80,l173,lpm_entry,l474);if(l461){
if(l460){l464++;l462 = 0;}else{l465++;l462 = (l135 == 2)?0:1;}}else{l462 = 1;
}if(l462){bkt_ptr = soc_mem_field32_get(l80,l173,lpm_entry,l472);if(bkt_ptr>
l457){LOG_CLI(("Invalid bucket pointer in %s index %d\n",SOC_MEM_NAME(l80,
l173),l159));}else{l463[bkt_ptr]++;}}}}soc_cm_sfree(l80,l458);l458 = NULL;
l173 = L3_DEFIP_PAIR_128m;l470 = WORDS2BYTES(soc_mem_entry_words(l80,l173))*
soc_mem_index_count(l80,l173);l458 = soc_cm_salloc(l80,l470,"lpm_tbl_p128");
if(l458 == NULL){l137 = SOC_E_MEMORY;goto l476;}sal_memset(l458,0,l470);if(
l455&&soc_mem_read_range(l80,l173,MEM_BLOCK_ANY,soc_mem_index_min(l80,l173),
soc_mem_index_max(l80,l173),l458)<0){l137 = (SOC_E_INTERNAL);goto l476;}for(
l159 = 0;l159<l455;l159++){l459 = soc_mem_table_idx_to_pointer(l80,l173,
defip_pair_128_entry_t*,l458,l159);if(0x03!= soc_mem_field32_get(l80,l173,
l459,MODE1_UPRf)){continue;};if(!soc_mem_field32_get(l80,l173,l459,
VALID1_LWRf)||!soc_mem_field32_get(l80,l173,l459,VALID0_LWRf)||!
soc_mem_field32_get(l80,l173,l459,VALID1_UPRf)||!soc_mem_field32_get(l80,l173
,l459,VALID0_UPRf)){continue;};l454++;l460 = soc_mem_field32_get(l80,l173,
l459,GLOBAL_HIGHf);l461 = soc_mem_field32_get(l80,l173,l459,GLOBAL_ROUTEf);if
(l461){if(l460){l464++;l462 = 0;}else{l465++;l462 = (l135 == 2)?0:1;}}else{
l462 = 1;}if(l462){bkt_ptr = soc_mem_field32_get(l80,l173,l459,ALG_BKT_PTRf);
if(bkt_ptr>l457){LOG_CLI(("Invalid bucket pointer in %s index %d\n",
SOC_MEM_NAME(l80,l173),l159));}else{l463[bkt_ptr]++;}}}for(l159 = 0;l159<l457
;l159++){if(l463[l159]!= 0){l456++;}if(l463[l159] == 4){l469++;}if(l463[l159]
== 3){l468++;}if(l463[l159] == 2){l467++;}if(l463[l159] == 1){l466++;}}if(
SOC_TH_ALPM_SCALE_CHECK(l80,0)){l457>>= 1;}LOG_CLI(("\n"));LOG_CLI((
"%-24s%d/%-4d\n","IPv4 Pivots:",l450,l451));LOG_CLI(("%-24s%d/%-4d\n",
"IPv6-64 Pivots:",l452,l453));LOG_CLI(("%-24s%d/%-4d\n","IPv6-128 Pivots:",
l454,l455));LOG_CLI(("%-24s%d/%-4d\n","Buckets:",l456,l457));LOG_CLI((
"%-24s%d\n","Global High:",l464));LOG_CLI(("%-24s%d\n","Global Low:",l465));
LOG_CLI(("\n"));LOG_CLI(("%s\n","Bucket Sharing:"));LOG_CLI(("%-24s%d\n",
"1->1:",l466));LOG_CLI(("%-24s%d\n","2->1:",l467));LOG_CLI(("%-24s%d\n",
"3->1:",l468));LOG_CLI(("%-24s%d\n","4->1:",l469));LOG_CLI(("\n"));LOG_CLI((
"%-24s%d\n","Split count:",alpm_split_count+alpm_128_split_count));LOG_CLI((
"%-24s%d\n","Merge count:",alpm_merge_count));LOG_CLI(("\n\n"));l476:if(l463)
{sal_free(l463);l463 = NULL;}if(l458){soc_cm_sfree(l80,l458);l458 = NULL;}if(
l123!= -1){int l477,l478,l144;l173 = _soc_tomahawk_alpm_bkt_view_get(l80,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,ALPM_BKTID(l123,0)));LOG_CLI((
"Bucket = %4d (View %s)\n",l123,l173 == -1?"INVALID":SOC_MEM_NAME(l80,l173)))
;LOG_CLI(("------------------------------------------------------------\n"));
LOG_CLI(("Index distribution (* indicates occupied)\n"));switch(l173){case
L3_DEFIP_ALPM_IPV4m:l478 = 6;break;case L3_DEFIP_ALPM_IPV6_64m:l478 = 4;break
;case L3_DEFIP_ALPM_IPV6_128m:l478 = 2;break;default:l173 = 
L3_DEFIP_ALPM_IPV4m;l478 = 6;break;}for(l477 = 0;l477<4;l477++){if(!(l449&(1
<<l477))){continue;}LOG_CLI(("Bank%d: ",l477));for(l159 = 0;l159<l478;l159++)
{l144 = (_shr_popcount(l449)+1)/2;l313 = (l159<<(l144+SOC_TH_ALPM_BUCKET_BITS
))|(l123<<l144)|l477;l137 = soc_mem_read(l80,l173,MEM_BLOCK_ANY,l313,l81);if(
l137 == 0&&soc_mem_field32_get(l80,l173,l81,VALIDf)){l144 = 1;}else{l144 = 0;
}if(l144){LOG_CLI(("%d(*) ",l313));}else{LOG_CLI(("%d( ) ",l313));}}LOG_CLI((
"\n"));}}LOG_CLI(("\n"));if(index!= -1){int bktid;LOG_CLI(("Index = %8d\n",
index));LOG_CLI((
"------------------------------------------------------------\n"));l173 = 
_soc_tomahawk_alpm_bkt_view_get(l80,index);LOG_CLI(("%-24s%s\n","Type",l173 == 
-1?"INVALID":SOC_MEM_NAME(l80,l173)));bktid = SOC_TH_ALPM_BKT_ENTRY_TO_IDX(
l80,index);LOG_CLI(("%-24s%d\n","Bucket:",ALPM_BKT_IDX(bktid)));LOG_CLI((
"%-24s%d\n","Sub-Bucket:",ALPM_BKT_SIDX(bktid)));LOG_CLI(("%-24s%d\n","Bank:"
,index&(_shr_popcount(l449)-1)));bktid = (_shr_popcount(l449)+1)/2+
SOC_TH_ALPM_BUCKET_BITS;LOG_CLI(("%-24s%d\n","Entry:",index>>bktid));LOG_CLI(
("\n"));}return SOC_E_NONE;}int soc_th_alpm_bucket_sanity_check(int l80,
soc_mem_t l173,int index){int l137 = SOC_E_NONE;int l159,l313,l321,l106,
tcam_index = -1;int vrf_id = 0,vrf;uint32 l139 = 0;uint32 l81[
SOC_MAX_MEM_FIELD_WORDS];uint32 l479[SOC_MAX_MEM_FIELD_WORDS];int l480,l481,
l482;defip_entry_t lpm_entry;int l326,l483;soc_mem_t l118;int l213,l318,l484,
bkt_ptr,l134;int l485 = 0;soc_field_t l486[3] = {VALID0f,VALID1f,INVALIDf};
soc_field_t l487[3] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f,INVALIDf};soc_field_t l488
[3] = {ALG_BKT_PTR0f,ALG_BKT_PTR1f,INVALIDf};soc_field_t l489[3] = {
ALG_SUB_BKT_PTR0f,ALG_SUB_BKT_PTR1f,INVALIDf};l326 = soc_mem_index_min(l80,
l173);l483 = soc_mem_index_max(l80,l173);if((index>= 0)&&(index<l326||index>
l483)){return SOC_E_PARAM;}else if(index>= 0){l326 = index;l483 = index;}
SOC_ALPM_LPM_LOCK(l80);for(l159 = l326;l159<= l483;l159++){
SOC_ALPM_LPM_UNLOCK(l80);SOC_ALPM_LPM_LOCK(l80);l137 = soc_mem_read(l80,l173,
MEM_BLOCK_ANY,l159,(void*)l81);if(SOC_FAILURE(l137)){continue;}l106 = 
soc_mem_field32_get(l80,l173,(void*)l81,MODE0f);if(l106){l318 = 1;l118 = 
L3_DEFIP_ALPM_IPV6_64m;l484 = 16;}else{l318 = 2;l118 = L3_DEFIP_ALPM_IPV4m;
l484 = 24;}for(l213 = 0;l213<l318;l213++){if(soc_mem_field32_get(l80,l173,(
void*)l81,l486[l213]) == 0||soc_mem_field32_get(l80,l173,(void*)l81,l487[l213
]) == 1){continue;}bkt_ptr = soc_mem_field32_get(l80,l173,(void*)l81,l488[
l213]);l134 = soc_mem_field32_get(l80,l173,(void*)l81,l489[l213]);if(l213 == 
1){soc_th_alpm_lpm_ip4entry1_to_0(l80,l81,l479,PRESERVE_HIT);}else{
soc_th_alpm_lpm_ip4entry0_to_0(l80,l81,l479,PRESERVE_HIT);}l137 = 
soc_th_alpm_lpm_vrf_get(l80,l479,&vrf_id,&vrf);if(SOC_FAILURE(l137)){continue
;}if(SOC_TH_ALPM_SCALE_CHECK(l80,l106)){l484<<= 1;}tcam_index = -1;for(l313 = 
0;l313<l484;l313++){l137 = _soc_th_alpm_mem_index(l80,l118,bkt_ptr,l313,l139,
&l321);if(SOC_FAILURE(l137)){continue;}l137 = soc_mem_read(l80,l118,
MEM_BLOCK_ANY,l321,(void*)l81);if(SOC_FAILURE(l137)){break;}if(!
soc_mem_field32_get(l80,l118,(void*)l81,VALIDf)){continue;}if(l134!= 
soc_mem_field32_get(l80,l118,(void*)l81,SUB_BKT_PTRf)){continue;}l137 = l122(
l80,(void*)l81,l118,l106,vrf_id,ALPM_BKTID(bkt_ptr,l134),0,&lpm_entry);if(
SOC_FAILURE(l137)){continue;}l137 = _soc_th_alpm_find(l80,l118,l106,(void*)&
lpm_entry,vrf_id,vrf,(void*)l81,&l480,&l481,&l482,FALSE);if(SOC_FAILURE(l137)
){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"\tLaunched AUX operation for "
"index %d bucket [%d,%d] sanity check failed\n"),l159,bkt_ptr,l134));l485++;
continue;}if(l481!= ALPM_BKTID(bkt_ptr,l134)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"\tEntry at index %d does not belong "
"to bucket [%d,%d](from bucket [%d,%d])\n"),l482,bkt_ptr,l134,ALPM_BKT_IDX(
l481),ALPM_BKT_SIDX(l481)));l485++;}if(tcam_index == -1){tcam_index = l480;
continue;}if(tcam_index!= l480){int l490,l491;l490 = soc_th_alpm_logical_idx(
l80,l173,tcam_index>>1,1);l491 = soc_th_alpm_logical_idx(l80,l173,l480>>1,1);
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tAliased bucket [%d,%d](returned "
"bucket [%d,%d]) found from TCAM1 %d and TCAM2 %d\n"),bkt_ptr,l134,
ALPM_BKT_IDX(l481),ALPM_BKT_SIDX(l481),l490,l491));l485++;}}}}
SOC_ALPM_LPM_UNLOCK(l80);if(l485 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(
l80,"\tMemory %s index %d Bucket sanity check passed\n"),SOC_MEM_NAME(l80,
l173),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l80,l173),index,l485));return
SOC_E_FAIL;}int soc_th_alpm_pivot_sanity_check(int l80,soc_mem_t l173,int
index){int l159,l213,l216 = 0;int l321,l326,l483;int l137 = SOC_E_NONE;int*
l492 = NULL;int l480,l481,l482;uint32 l81[SOC_MAX_MEM_FIELD_WORDS];int bkt_ptr
,l134,l106 = 0,l318 = 2;int l493,l494[2];soc_mem_t l495,l496;int l485 = 0;int
vrf,l130;soc_field_t l486[3] = {VALID0f,VALID1f,INVALIDf};soc_field_t l487[3]
= {GLOBAL_HIGH0f,GLOBAL_HIGH1f,INVALIDf};soc_field_t l497[3] = {
NEXT_HOP_INDEX0f,NEXT_HOP_INDEX1f,INVALIDf};soc_field_t l488[3] = {
ALG_BKT_PTR0f,ALG_BKT_PTR1f,INVALIDf};soc_field_t l489[3] = {
ALG_SUB_BKT_PTR0f,ALG_SUB_BKT_PTR1f,INVALIDf};l326 = soc_mem_index_min(l80,
l173);l483 = soc_mem_index_max(l80,l173);if((index>= 0)&&(index<l326||index>
l483)){return SOC_E_PARAM;}else if(index>= 0){l326 = index;l483 = index;}l159
= sizeof(int)*SOC_TH_MAX_ALPM_BUCKETS*SOC_TH_MAX_SUB_BUCKETS;l492 = sal_alloc
(l159,"Bucket index array");if(l492 == NULL){l137 = SOC_E_MEMORY;return l137;
}sal_memset(l492,0xff,l159);SOC_ALPM_LPM_LOCK(l80);for(l159 = l326;l159<= 
l483;l159++){SOC_ALPM_LPM_UNLOCK(l80);SOC_ALPM_LPM_LOCK(l80);l137 = 
soc_mem_read(l80,l173,MEM_BLOCK_ANY,l159,(void*)l81);if(SOC_FAILURE(l137)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l80,l173),
l159,l137));l485++;continue;}l106 = soc_mem_field32_get(l80,l173,(void*)l81,
MODE0f);if(l106){l318 = 1;l496 = L3_DEFIP_ALPM_IPV6_64m;}else{l318 = 2;l496 = 
L3_DEFIP_ALPM_IPV4m;}for(l213 = 0;l213<l318;l213++){if(soc_mem_field32_get(
l80,l173,(void*)l81,l486[l213]) == 0||soc_mem_field32_get(l80,l173,(void*)l81
,l487[l213]) == 1){continue;}bkt_ptr = soc_mem_field32_get(l80,l173,(void*)
l81,l488[l213]);l134 = soc_mem_field32_get(l80,l173,(void*)l81,l489[l213]);(
void)soc_th_alpm_lpm_vrf_get(l80,l81,&vrf,&l130);l137 = 
soc_th_alpm_bucket_is_assigned(l80,bkt_ptr,l130,l106,&l216);if(l137 == 
SOC_E_PARAM||l134>= SOC_TH_MAX_SUB_BUCKETS){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"\tInvalid bucket pointer [%d,%d] "
"detected, in memory %s index %d\n"),bkt_ptr,l134,SOC_MEM_NAME(l80,l173),l159
));l485++;continue;}if(bkt_ptr!= 0){if(l492[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+
l134] == -1){l492[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l134] = l159;}else{LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"\tDuplicated bucket pointer "
"[%d,%d] detected, in memory %s index1 %d ""and index2 %d\n"),bkt_ptr,l134,
SOC_MEM_NAME(l80,l173),l492[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l134],l159));l485
++;continue;}}else{continue;}if(l137>= 0&&l216 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),bkt_ptr,SOC_MEM_NAME(l80,l173),l159));
l485++;continue;}l495 = _soc_tomahawk_alpm_bkt_view_get(l80,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l80,ALPM_BKTID(bkt_ptr,0)));if(l496!= l495){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual"" %s\n"),bkt_ptr,SOC_MEM_NAME(l80,
l496),SOC_MEM_NAME(l80,l495)));l485++;continue;}l493 = soc_mem_field32_get(
l80,l173,(void*)l81,l497[l213]);if(l213 == 1){l137 = 
soc_th_alpm_lpm_ip4entry1_to_0(l80,l81,l81,PRESERVE_HIT);if(SOC_FAILURE(l137)
){continue;}}l480 = -1;l137 = _soc_th_alpm_find(l80,l496,l106,l81,vrf,l130,(
void*)l81,&l480,&l481,&l482,FALSE);if(l480 == -1){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l80,"\tLaunched AUX operation for PIVOT "
"index %d sanity check failed\n"),l159));l485++;continue;}l321 = 
soc_th_alpm_logical_idx(l80,l173,l480>>1,1);l137 = soc_mem_read(l80,l173,
MEM_BLOCK_ANY,l321,(void*)l81);if(SOC_FAILURE(l137)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l80,"\tRead memory %s index %d (nexthop) "
"return %d\n"),SOC_MEM_NAME(l80,l173),l321,l137));l485++;continue;}l494[0] = 
soc_mem_field32_get(l80,l173,(void*)l81,l497[0]);l494[1] = 
soc_mem_field32_get(l80,l173,(void*)l81,l497[1]);if(l493!= l494[l480&1]){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d entry1 %d, index2 %d ""entry2 %d\n"),l159,l213,
l321,l480&1));l485++;continue;}}}SOC_ALPM_LPM_UNLOCK(l80);sal_free(l492);if(
l485 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tMemory %s index %d Pivot sanity check passed\n"),SOC_MEM_NAME(l80,l173),
index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l80,
"\tMemory %s index %d Pivot sanity check failed, ""encountered %d error(s)\n"
),SOC_MEM_NAME(l80,l173),index,l485));return SOC_E_FAIL;}
#endif /* ALPM_ENABLE */
