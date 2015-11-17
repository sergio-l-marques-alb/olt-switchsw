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
#include <soc/tomahawk.h>
#include <shared/bsl.h>

#ifdef ALPM_ENABLE
#include <shared/util.h>
#include <shared/l3.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#include <soc/esw/trie.h>

#if 1
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
soc_field_info_t*l57;soc_field_info_t*l58;}l59,*l60;static l60 l61[
SOC_MAX_NUM_DEVICES];typedef struct l62{int unit;int l63;int l64;uint16*l65;
uint16*l66;}l67;typedef uint32 l68[5];typedef int(*l69)(l68 l70,l68 l71);
static l67*l72[SOC_MAX_NUM_DEVICES];soc_alpm_bucket_t soc_th_alpm_bucket[
SOC_MAX_NUM_DEVICES];int l3_alpm_ipv4_double_wide[SOC_MAX_NUM_DEVICES];
alpm_bkt_usg_t*bkt_usage[SOC_MAX_NUM_DEVICES];alpm_bkt_bmp_t*global_bkt_usage
[SOC_MAX_NUM_DEVICES][SOC_TH_MAX_ALPM_VIEWS];static void l73(int l74,void*l75
,int index,l68 l76);static uint16 l77(uint8*l78,int l79);static int l80(int
unit,int l63,int l64,l67**l81);static int l82(l67*l83);static int l84(l67*l85
,l69 l86,l68 entry,int l87,uint16*l88);static int l89(l67*l85,l69 l86,l68
entry,int l87,uint16 l90,uint16 l91);static int l92(l67*l85,l69 l86,l68 entry
,int l87,uint16 l93);static int l94(int l74);static int l95(int l74);static
int l96(int l74,void*l97,int*index);static int soc_th_alpm_lpm_delete(int l74
,void*key_data);static int l98(int l74,void*key_data,int l99,int l100,int l101
,defip_aux_scratch_entry_t*l102);static int l103(int l74,void*key_data,void*
l75,int*l104,int*l105,int*l99);static int l106(int l74,void*key_data,void*l75
,int*l104);static int l107(int l74);static int l108(int unit,void*lpm_entry,
void*l109,void*l110,soc_mem_t l111,uint32 l112,uint32*l113,int l114);static
int l115(int unit,void*l109,soc_mem_t l111,int l99,int l116,int l117,int index
,void*lpm_entry);static int l118(int unit,uint32*key,int len,int l116,int l99
,defip_entry_t*lpm_entry,int l119,int l120);static int l121(int l74,int l116,
int l122);static int l123(int l74,alpm_pfx_info_t*l124,trie_t*l125,uint32*
l126,uint32 l127,trie_node_t*,defip_entry_t*lpm_entry,uint32*l128);static int
l129(int l74,alpm_pfx_info_t*l124,int*l130,int*l104);static int
_soc_th_alpm_move_inval(int l74,soc_mem_t l111,int l122,
alpm_mem_prefix_array_t*l131,int*l91);int soc_th_alpm_lpm_vrf_get(int unit,
void*lpm_entry,int*l116,int*l132);int _soc_th_alpm_rollback_bkt_move(int l74,
void*key_data,soc_mem_t l111,alpm_pivot_t*l133,alpm_pivot_t*l134,
alpm_mem_prefix_array_t*l135,int*l91,int l136);int soc_th_alpm_lpm_delete(int
l74,void*key_data);void _soc_th_alpm_rollback_pivot_add(int l74,defip_entry_t
*l137,void*key_data,int tcam_index,alpm_pivot_t*pivot_pyld);extern
alpm_vrf_handle_t*alpm_vrf_handle[SOC_MAX_NUM_DEVICES];extern alpm_pivot_t**
tcam_pivot[SOC_MAX_NUM_DEVICES];extern int l138(int unit);extern int
soc_th_get_alpm_banks(int unit);extern int _alpm_128_prefix_create(int l74,
void*entry,uint32*l139,uint32*l105,int*l113);void soc_th_alpm_lpm_move_bu_upd
(int l74,int l114,int l140){alpm_bkt_usg_t*l141;int bkt_ptr = ALPM_BKT_IDX(
l114);int l142 = ALPM_BKT_SIDX(l114);l141 = &bkt_usage[l74][bkt_ptr];l141->
pivots[l142] = l140;}int soc_th_alpm_mode_get(int l74){uint32 l143;
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l74,&l143));return
soc_reg_field_get(l74,L3_DEFIP_RPF_CONTROLr,l143,LOOKUP_MODEf);}static int
l144(int l74,const void*entry,int*l87){int l145;uint32 l146;int l99;l99 = 
soc_mem_field32_get(l74,L3_DEFIPm,entry,MODE0f);if(l99){l146 = 
soc_mem_field32_get(l74,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l145 = 
_ipmask2pfx(l146,l87))<0){return(l145);}l146 = soc_mem_field32_get(l74,
L3_DEFIPm,entry,IP_ADDR_MASK1f);if(*l87){if(l146!= 0xffffffff){return(
SOC_E_PARAM);}*l87+= 32;}else{if((l145 = _ipmask2pfx(l146,l87))<0){return(
l145);}}}else{l146 = soc_mem_field32_get(l74,L3_DEFIPm,entry,IP_ADDR_MASK0f);
if((l145 = _ipmask2pfx(l146,l87))<0){return(l145);}}return SOC_E_NONE;}void
soc_th_alpm_bank_db_type_get(int l74,int l116,uint32*l147,uint32*l100){if(
l116 == SOC_VRF_MAX(l74)+1){if(l100){*l100 = 0;}if(soc_th_get_alpm_banks(l74)
<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l74,*l147);}else{
SOC_ALPM_TH_GET_GLOBAL_BANK_DISABLE(l74,*l147);}}else{if(l100){*l100 = 2;}if(
soc_th_get_alpm_banks(l74)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(
l74,*l147);}else{SOC_ALPM_TH_GET_VRF_BANK_DISABLE(l74,*l147);}}}int
_soc_th_alpm_rpf_entry(int l74,int l148){int l149;int l150 = 
soc_th_get_alpm_banks(l74)/2;l149 = (l148>>l150)&SOC_TH_ALPM_BKT_MASK;l149+= 
SOC_TH_ALPM_BUCKET_COUNT(l74);return(l148&~(SOC_TH_ALPM_BKT_MASK<<l150))|(
l149<<l150);}int soc_th_alpm_physical_idx(int l74,soc_mem_t l111,int index,
int l151){int l152 = index&1;if(l151){return soc_trident2_l3_defip_index_map(
l74,l111,index);}index>>= 1;index = soc_trident2_l3_defip_index_map(l74,l111,
index);index<<= 1;index|= l152;return index;}int soc_th_alpm_logical_idx(int
l74,soc_mem_t l111,int index,int l151){int l152 = index&1;if(l151){return
soc_trident2_l3_defip_index_remap(l74,l111,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_remap(l74,l111,index);index<<= 1;index|= l152;
return index;}static int l153(int l74,void*entry,uint32*prefix,uint32*l105,
int*l113){int l154,l155,l99;int l87 = 0;int l145 = SOC_E_NONE;uint32 l156,
l152;prefix[0] = prefix[1] = prefix[2] = prefix[3] = prefix[4] = 0;l99 = 
soc_mem_field32_get(l74,L3_DEFIPm,entry,MODE0f);l154 = soc_mem_field32_get(
l74,L3_DEFIPm,entry,IP_ADDR0f);l155 = soc_mem_field32_get(l74,L3_DEFIPm,entry
,IP_ADDR_MASK0f);prefix[1] = l154;l154 = soc_mem_field32_get(l74,L3_DEFIPm,
entry,IP_ADDR1f);l155 = soc_mem_field32_get(l74,L3_DEFIPm,entry,
IP_ADDR_MASK1f);prefix[0] = l154;if(l99){prefix[4] = prefix[1];prefix[3] = 
prefix[0];prefix[1] = prefix[0] = 0;l155 = soc_mem_field32_get(l74,L3_DEFIPm,
entry,IP_ADDR_MASK0f);if((l145 = _ipmask2pfx(l155,&l87))<0){return(l145);}
l155 = soc_mem_field32_get(l74,L3_DEFIPm,entry,IP_ADDR_MASK1f);if(l87){if(
l155!= 0xffffffff){return(SOC_E_PARAM);}l87+= 32;}else{if((l145 = _ipmask2pfx
(l155,&l87))<0){return(l145);}}l156 = 64-l87;if(l156<32){prefix[4]>>= l156;
l152 = (((32-l156) == 32)?0:(prefix[3])<<(32-l156));prefix[3]>>= l156;prefix[
4]|= l152;}else{prefix[4] = (((l156-32) == 32)?0:(prefix[3])>>(l156-32));
prefix[3] = 0;}}else{l155 = soc_mem_field32_get(l74,L3_DEFIPm,entry,
IP_ADDR_MASK0f);if((l145 = _ipmask2pfx(l155,&l87))<0){return(l145);}prefix[1]
= (((32-l87) == 32)?0:(prefix[1])>>(32-l87));prefix[0] = 0;}*l105 = l87;*l113
= (prefix[0] == 0)&&(prefix[1] == 0)&&(l87 == 0);return SOC_E_NONE;}int
_soc_th_alpm_find_in_bkt(int l74,soc_mem_t l111,int bktid,int l147,uint32*l75
,void*alpm_data,int*l88,int l122){int l145;l145 = soc_mem_alpm_lookup(l74,
l111,ALPM_BKT_IDX(bktid),MEM_BLOCK_ANY,l147,l75,alpm_data,l88);if(SOC_SUCCESS
(l145)){return l145;}if(SOC_TH_ALPM_SCALE_CHECK(l74,l122)){return
soc_mem_alpm_lookup(l74,l111,ALPM_BKT_IDX(bktid)+1,MEM_BLOCK_ANY,l147,l75,
alpm_data,l88);}return l145;}static int l157(int l74,uint32*prefix,uint32 l127
,int l122,int l116,int*l158,int*tcam_index,int*bktid){int l145 = SOC_E_NONE;
trie_t*l159;trie_node_t*l160 = NULL;alpm_pivot_t*pivot_pyld;if(l122){l159 = 
VRF_PIVOT_TRIE_IPV6(l74,l116);}else{l159 = VRF_PIVOT_TRIE_IPV4(l74,l116);}
l145 = trie_find_lpm(l159,prefix,l127,&l160);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"Pivot find failed\n")));return l145;}
pivot_pyld = (alpm_pivot_t*)l160;*l158 = 1;*tcam_index = PIVOT_TCAM_INDEX(
pivot_pyld);*bktid = PIVOT_BUCKET_INDEX(pivot_pyld);return SOC_E_NONE;}static
int _soc_th_alpm_find(int l74,void*key_data,soc_mem_t l111,void*alpm_data,int
*tcam_index,int*bktid,int*l104,int l161){uint32 l75[SOC_MAX_MEM_FIELD_WORDS];
int l162,l116,l122;int l88;uint32 l100,l147;int l145 = SOC_E_NONE;int l158 = 
0;l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);if(l122){if(!(
l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l162
,&l116));if(l162 == 0){if(soc_th_alpm_mode_get(l74)){return SOC_E_PARAM;}}
soc_th_alpm_bank_db_type_get(l74,l116,&l147,&l100);if(!(ALPM_PREFIX_IN_TCAM(
l74,l162))){if(l161){uint32 prefix[5],l127;int l113 = 0;l145 = l153(l74,
key_data,prefix,&l127,&l113);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l74,"_soc_alpm_insert: prefix create failed\n")));return l145;}
SOC_IF_ERROR_RETURN(l157(l74,prefix,l127,l122,l116,&l158,tcam_index,bktid));}
else{defip_aux_scratch_entry_t l102;sal_memset(&l102,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l98(l74,key_data,l122,l100,0,
&l102));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l74,PREFIX_LOOKUP,&l102,TRUE,
&l158,tcam_index,bktid));}if(l158){l108(l74,key_data,l75,0,l111,0,0,*bktid);
l145 = _soc_th_alpm_find_in_bkt(l74,l111,*bktid,l147,l75,alpm_data,&l88,l122)
;if(SOC_SUCCESS(l145)){*l104 = l88;}}else{l145 = SOC_E_NOT_FOUND;}}return l145
;}static int l163(int l74,void*key_data,void*alpm_data,void*alpm_sip_data,
soc_mem_t l111,int l88){defip_aux_scratch_entry_t l102;int l162,l122,l116;int
bktid;uint32 l100,l147;int l145 = SOC_E_NONE;int l158 = 0,l152 = 0;int
tcam_index,index;uint32 l164[SOC_MAX_MEM_FIELD_WORDS];l122 = 
soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);if(l122){if(!(l122 = 
soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l162,&l116));
soc_th_alpm_bank_db_type_get(l74,l116,&l147,&l100);if(!soc_th_alpm_mode_get(
l74)){l100 = 2;}if(l162!= SOC_L3_VRF_OVERRIDE){sal_memset(&l102,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l98(l74,key_data,l122,l100,0,
&l102));SOC_ALPM_LPM_LOCK(l74);l145 = _soc_th_alpm_find(l74,key_data,l111,
l164,&tcam_index,&bktid,&index,TRUE);SOC_ALPM_LPM_UNLOCK(l74);
SOC_IF_ERROR_RETURN(l145);soc_mem_field32_set(l74,l111,alpm_data,SUB_BKT_PTRf
,ALPM_BKT_SIDX(bktid));SOC_IF_ERROR_RETURN(soc_mem_write(l74,l111,
MEM_BLOCK_ANY,ALPM_ENT_INDEX(l88),alpm_data));if(SOC_URPF_STATUS_GET(l74)){
soc_mem_field32_set(l74,l111,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid))
;SOC_IF_ERROR_RETURN(soc_mem_write(l74,l111,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l74,ALPM_ENT_INDEX(l88)),alpm_sip_data));if(l145!= 
SOC_E_NONE){return SOC_E_FAIL;}}l152 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,IP_LENGTHf);soc_mem_field32_set(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,REPLACE_LENf,l152);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,DELETE_PROPAGATE,&l102,TRUE,&l158,&tcam_index,&bktid)
);if(SOC_URPF_STATUS_GET(l74)){l152 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf);l152+= 1;soc_mem_field32_set(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf,l152);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,DELETE_PROPAGATE,&l102,TRUE,&l158,&tcam_index,&bktid)
);}}return l145;}int soc_th_alpm_update_hit_bits(int l74,int count,int*l165,
int*l166){int l145 = SOC_E_NONE;int l167,l168,l169,index;soc_mem_t l111 = 
L3_DEFIP_AUX_HITBIT_UPDATEm;defip_aux_hitbit_update_entry_t l170;int l171 = 
soc_mem_index_count(l74,l111);l169 = (count+l171-1)/l171;for(l168 = 0;l168<
l169;l168++){for(l167 = l168*l171;l167<((count>(l168+1)*l171)?(l168+1)*l171:
count);l167++){index = l167%l171;if(l165[l167]<= 0){continue;}
SOC_IF_ERROR_RETURN(soc_mem_read(l74,l111,MEM_BLOCK_ANY,index,&l170));
soc_mem_field32_set(l74,l111,&l170,HITBIT_PTRf,l165[l167]);
soc_mem_field32_set(l74,l111,&l170,REPLACEMENT_PTRf,l166[l167]);
soc_mem_field32_set(l74,l111,&l170,VALIDf,1);SOC_IF_ERROR_RETURN(
soc_mem_write(l74,l111,MEM_BLOCK_ANY,index,&l170));}SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,HITBIT_REPLACE,NULL,FALSE,NULL,NULL,NULL));}return
l145;}int _soc_th_alpm_mem_prefix_array_cb(trie_node_t*node,void*l172){
alpm_mem_prefix_array_t*l173 = (alpm_mem_prefix_array_t*)l172;if(node->type == 
PAYLOAD){l173->prefix[l173->count] = (payload_t*)node;l173->count++;}return
SOC_E_NONE;}int _soc_th_alpm_bkt_entry_cnt(int l74,int l122){int l174 = 0;
switch(l122){case L3_DEFIP_MODE_V4:l174 = ALPM_IPV4_BKT_COUNT;break;case
L3_DEFIP_MODE_64:l174 = ALPM_IPV6_64_BKT_COUNT;break;case L3_DEFIP_MODE_128:
l174 = ALPM_IPV6_128_BKT_COUNT;break;default:l174 = ALPM_IPV4_BKT_COUNT;break
;}if(SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l174<<= 1;}if(soc_th_get_alpm_banks(
l74)<= 2){l174>>= 1;}if(soc_th_alpm_mode_get(l74) == 1&&SOC_URPF_STATUS_GET(
l74)){l174>>= 1;}return l174;}static int l175(int l74,int l2,alpm_bkt_bmp_t*
l176){int l167;for(l167 = l2+1;l167<SOC_TH_MAX_ALPM_BUCKETS;l167++){if(
SHR_BITGET(l176->bkt_bmp,l167)){return l167;}}return-1;}static int l177(int
l74,int l122,int l140,int bktid){int l145 = SOC_E_NONE;int l178;soc_mem_t l179
= L3_DEFIPm;defip_entry_t lpm_entry;int l180,l181;l180 = ALPM_BKT_IDX(bktid);
l181 = ALPM_BKT_SIDX(bktid);l178 = soc_th_alpm_logical_idx(l74,l179,l140>>1,1
);l145 = soc_mem_read(l74,l179,MEM_BLOCK_ANY,l178,&lpm_entry);
SOC_IF_ERROR_RETURN(l145);if((!l122)&&(l140&1)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(&lpm_entry)
,(l61[(l74)]->l50),(l180));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(&lpm_entry),(l61[(l74)]->l52),(l181));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(&lpm_entry)
,(l61[(l74)]->l49),(l180));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(&lpm_entry),(l61[(l74)]->l51),(l181));}l145 = soc_mem_write(
l74,l179,MEM_BLOCK_ANY,l178,&lpm_entry);SOC_IF_ERROR_RETURN(l145);if(
SOC_URPF_STATUS_GET(l74)){int l182 = soc_th_alpm_logical_idx(l74,l179,l140>>1
,1)+(soc_mem_index_count(l74,l179)>>1);l145 = READ_L3_DEFIPm(l74,
MEM_BLOCK_ANY,l182,&lpm_entry);SOC_IF_ERROR_RETURN(l145);if((!l122)&&(l182&1)
){soc_mem_field32_set(l74,l179,&lpm_entry,ALG_BKT_PTR1f,l180+
SOC_TH_ALPM_BUCKET_COUNT(l74));soc_mem_field32_set(l74,l179,&lpm_entry,
ALG_SUB_BKT_PTR1f,l181);}else{soc_mem_field32_set(l74,l179,&lpm_entry,
ALG_BKT_PTR0f,l180+SOC_TH_ALPM_BUCKET_COUNT(l74));soc_mem_field32_set(l74,
l179,&lpm_entry,ALG_SUB_BKT_PTR0f,l181);}l145 = WRITE_L3_DEFIPm(l74,
MEM_BLOCK_ANY,l182,&lpm_entry);SOC_IF_ERROR_RETURN(l145);}return l145;}static
int _soc_th_alpm_move_trie(int l74,int l122,int l183,int l184){int*l91 = NULL
,*l90 = NULL;int l145 = SOC_E_NONE,l185,l167,l186;int l88,l140;uint32 l147 = 
0;soc_mem_t l111;void*l187,*l188;trie_t*trie = NULL;payload_t*l87 = NULL;
alpm_mem_prefix_array_t*l131 = NULL;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];
defip_alpm_ipv4_entry_t l189,l190;defip_alpm_ipv6_64_entry_t l191,l192;int
l193,l194,l195,l196;l186 = sizeof(int)*MAX_PREFIX_PER_BUCKET;l91 = sal_alloc(
l186,"new_index_move");l90 = sal_alloc(l186,"old_index_move");l131 = 
sal_alloc(sizeof(alpm_mem_prefix_array_t),"prefix_array");if(l91 == NULL||l90
== NULL||l131 == NULL){l145 = SOC_E_MEMORY;goto l197;}l193 = ALPM_BKT_IDX(
l183);l195 = ALPM_BKT_SIDX(l183);l194 = ALPM_BKT_IDX(l184);l196 = 
ALPM_BKT_SIDX(l184);l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;
l187 = ((l122)?((uint32*)&(l191)):((uint32*)&(l189)));l188 = ((l122)?((uint32
*)&(l192)):((uint32*)&(l190)));l140 = SOC_ALPM_BS_BKT_USAGE_PIVOT(l74,l183);
trie = PIVOT_BUCKET_TRIE(ALPM_TCAM_PIVOT(l74,l140));
soc_th_alpm_bank_db_type_get(l74,SOC_ALPM_BS_BKT_USAGE_VRF(l74,l183),&l147,
NULL);sal_memset(l131,0,sizeof(*l131));l145 = trie_traverse(trie,
_soc_th_alpm_mem_prefix_array_cb,l131,_TRIE_INORDER_TRAVERSE);do{if((l145)<0)
{goto l197;}}while(0);sal_memset(l91,-1,l186);sal_memset(l90,-1,l186);for(
l167 = 0;l167<l131->count;l167++){l87 = l131->prefix[l167];if(l87->index>0){
l145 = soc_mem_read(l74,l111,MEM_BLOCK_ANY,l87->index,l187);if(SOC_FAILURE(
l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_mem_read index %d failed\n"),l87->index));goto l198;}soc_mem_field32_set
(l74,l111,l187,SUB_BKT_PTRf,l196);if(SOC_URPF_STATUS_GET(l74)){l145 = 
soc_mem_read(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l87->index),
l188);if(SOC_FAILURE(l145)){goto l198;}soc_mem_field32_set(l74,l111,l188,
SUB_BKT_PTRf,l196);}l145 = _soc_th_alpm_insert_in_bkt(l74,l111,l184,l147,l187
,l75,&l88,l122);if(SOC_SUCCESS(l145)){if(SOC_URPF_STATUS_GET(l74)){l145 = 
soc_mem_write(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),l188);if
(SOC_FAILURE(l145)){goto l198;}}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"insert to bucket %d failed\n"),l184));goto l198;}l91[l167] = l88;l90[
l167] = l87->index;}}l145 = l177(l74,l122,l140,l184);if(SOC_FAILURE(l145)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_move_relink failed, pivot %d bkt %d\n"),l140,l184));goto l198;}
l145 = _soc_th_alpm_move_inval(l74,l111,l122,l131,l91);if(SOC_FAILURE(l145)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_th_alpm_move_inval failed\n")
));goto l198;}l145 = soc_th_alpm_update_hit_bits(l74,l131->count,l90,l91);if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l131->count));l145 = 
SOC_E_NONE;}PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l74,l140)) = ALPM_BKTID(l194,
l196);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l74,l122,l193);
SOC_ALPM_BS_BKT_USAGE_SB_DEL(l74,l193,l195,TRUE,l131->count);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l74,l122,l193);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l74,l122,l194);SOC_ALPM_BS_BKT_USAGE_SB_ADD(
l74,l194,l196,l140,-1,l131->count);SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l74,l122,
l194);l198:if(SOC_FAILURE(l145)){l185 = l177(l74,l122,l140,l183);do{if((l185)
<0){goto l197;}}while(0);for(l167 = 0;l167<l131->count;l167++){if(l91[l167] == 
-1){continue;}l185 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,l91[l167],
soc_mem_entry_null(l74,l111));do{if((l185)<0){goto l197;}}while(0);if(
SOC_URPF_STATUS_GET(l74)){l185 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l74,l91[l167]),soc_mem_entry_null(l74,l111));do{if((
l185)<0){goto l197;}}while(0);}}}l197:if(l91!= NULL){sal_free(l91);}if(l90!= 
NULL){sal_free(l90);}if(l131!= NULL){sal_free(l131);}return l145;}int
soc_th_alpm_bs_merge(int l74,int l122,int l199,int l200){int l145 = 
SOC_E_NONE;alpm_bkt_usg_t*l201,*l202;int l203,l204;l201 = &bkt_usage[l74][
l199];l202 = &bkt_usage[l74][l200];for(l203 = 0;l203<SOC_TH_MAX_SUB_BUCKETS;
l203++){if(!(l201->sub_bkts&(1<<l203))){continue;}for(l204 = 0;l204<
SOC_TH_MAX_SUB_BUCKETS;l204++){if((l202->sub_bkts&(1<<l204))!= 0){continue;}
if(l122 == L3_DEFIP_MODE_128){l145 = _soc_th_alpm_128_move_trie(l74,l122,
ALPM_BKTID(l199,l203),ALPM_BKTID(l200,l204));}else{l145 = 
_soc_th_alpm_move_trie(l74,l122,ALPM_BKTID(l199,l203),ALPM_BKTID(l200,l204));
}SOC_IF_ERROR_RETURN(l145);break;}}return SOC_E_NONE;}static int l205(int l74
,int l206,int l207){if(l206 == l207){return 1;}if(l206 == (SOC_VRF_MAX(l74)+1
)||l207 == (SOC_VRF_MAX(l74)+1)){return 0;}return 1;}static int l208(int l74,
int*bktid,int l122){int l209,l210,l174;int l199,l200;alpm_bkt_bmp_t*l211,*
l212;alpm_bkt_usg_t*l201,*l202;l201 = l202 = NULL;l174 = 
_soc_th_alpm_bkt_entry_cnt(l74,l122);for(l209 = 1;l209<= (l174/2);l209++){
l211 = &global_bkt_usage[l74][l122][l209];if(l211->bkt_count<1){continue;}
l199 = l175(l74,-1,l211);if(l199 == ALPM_BKT_IDX(*bktid)){l199 = l175(l74,
l199,l211);if(l199 == -1){continue;}}l201 = &bkt_usage[l74][l199];l200 = l175
(l74,l199,l211);while(l200!= -1){l202 = &bkt_usage[l74][l200];if(!l205(l74,
l201->vrf_type[0],l202->vrf_type[0])){l200 = l175(l74,l200,l211);continue;}if
(_shr_popcount((uint32)l201->sub_bkts)+_shr_popcount((uint32)l202->sub_bkts)
<= SOC_TH_MAX_SUB_BUCKETS){break;}l200 = l175(l74,l200,l211);}if(l200!= -1){
SOC_IF_ERROR_RETURN(soc_th_alpm_bs_merge(l74,l122,l199,l200));*bktid = 
ALPM_BKTID(l199,0);return SOC_E_NONE;}for(l210 = l209+1;l210<
SOC_TH_MAX_BUCKET_ENTRIES;l210++){l212 = &global_bkt_usage[l74][l122][l210];
if(l212->bkt_count<= 0){continue;}if((l209+l210)>l174){break;}l200 = l175(l74
,-1,l212);while(l200!= -1){l202 = &bkt_usage[l74][l200];if((l200 == l199)||(!
l205(l74,l201->vrf_type[0],l202->vrf_type[0]))){l200 = l175(l74,l200,l212);
continue;}if(_shr_popcount((uint32)l201->sub_bkts)+_shr_popcount((uint32)l202
->sub_bkts)<= SOC_TH_MAX_SUB_BUCKETS){break;}l200 = l175(l74,l200,l212);}if(
l200!= -1){SOC_IF_ERROR_RETURN(soc_th_alpm_bs_merge(l74,l122,l199,l200));*
bktid = ALPM_BKTID(l199,0);return SOC_E_NONE;}}}return SOC_E_FULL;}int
soc_th_alpm_bs_alloc(int l74,int*bktid,int l116,int l122){int l145 = 
SOC_E_NONE;l145 = soc_th_alpm_bucket_assign(l74,bktid,l116,l122);if(l145!= 
SOC_E_FULL){*bktid = ALPM_BKTID(*bktid,0);return l145;}l145 = l208(l74,bktid,
l122);return l145;}int soc_th_alpm_bs_free(int l74,int bktid,int l116,int l122
){int l145 = SOC_E_NONE;alpm_bkt_usg_t*l141;int l213,l214;l213 = ALPM_BKT_IDX
(bktid);l214 = ALPM_BKT_SIDX(bktid);l141 = &bkt_usage[l74][l213];l141->
sub_bkts&= ~(1<<l214);l141->pivots[l214] = 0;if(l141->count == 0){l141->
vrf_type[l214] = 0;l145 = soc_th_alpm_bucket_release(l74,l213,l116,l122);}
return l145;}int soc_th_alpm_bu_upd(int l74,int l117,int tcam_index,int l215,
int l122,int l216){int l213,l214;l213 = ALPM_BKT_IDX(l117);l214 = 
ALPM_BKT_SIDX(l117);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l74,l122,l213);if(l216>0
){SOC_ALPM_BS_BKT_USAGE_SB_ADD(l74,l213,l214,tcam_index,l215,l216);}else if(
l216<0){SOC_ALPM_BS_BKT_USAGE_SB_DEL(l74,l213,l214,l215,-l216);}
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l74,l122,l213);return SOC_E_NONE;}int
soc_th_alpm_bs_dump_brief(int l74,const char*l87){int l167,l217,l218,l149,
l219 = 0;int l220 = 1,l221 = 0;alpm_bkt_bmp_t*l222 = NULL;alpm_bkt_usg_t*l141
= NULL;int*l126;char*l223[] = {"IPv4","IPv6-64","IPv6-128"};l126 = sal_alloc(
sizeof(int)*16384,"pivot");if(l126 == NULL){return SOC_E_MEMORY;}sal_memset(
l126,0,sizeof(int)*16384);for(l217 = 0;l217<SOC_TH_MAX_ALPM_VIEWS;l217++){
LOG_CLI((BSL_META_U(l74,"\n[- %8s -]\n"),l223[l217]));LOG_CLI((BSL_META_U(l74
,"=================================================\n")));LOG_CLI((BSL_META_U
(l74," %5s | %5s %10s\n"),"entry","count","1st-bktbmp"));LOG_CLI((BSL_META_U(
l74,"=================================================\n")));l149 = 0;for(
l218 = 0;l218<SOC_TH_MAX_BUCKET_ENTRIES;l218++){l222 = &global_bkt_usage[l74]
[l217][l218];if(l222->bkt_count == 0){continue;}l149++;LOG_CLI((BSL_META_U(
l74," %5d   %5d"),l218,l222->bkt_count));for(l167 = 0;l167<
SOC_TH_MAX_ALPM_BUCKETS;l167++){if(SHR_BITGET(l222->bkt_bmp,l167)){LOG_CLI((
BSL_META_U(l74," 0x%08x\n"),l222->bkt_bmp[l167/SHR_BITWID]));break;}}}if(l149
== 0){LOG_CLI((BSL_META_U(l74,"- None - \n")));}}l218 = 0;LOG_CLI((BSL_META_U
(l74,"%5s | %s\n"),"bkt","(sub-bkt-idx, tcam-idx)"));LOG_CLI((BSL_META_U(l74,
"=================================================\n")));for(l149 = 0;l149<
SOC_TH_MAX_ALPM_BUCKETS;l149++){l141 = &bkt_usage[l74][l149];if(l141->count == 
0){continue;}l218++;l219 = 0;LOG_CLI((BSL_META_U(l74,"%5d | "),l149));for(
l167 = 0;l167<4;l167++){if(l141->sub_bkts&(1<<l167)){LOG_CLI((BSL_META_U(l74,
"(%d, %5d) "),l167,l141->pivots[l167]));if(l126[l141->pivots[l167]]!= 0){l219
= l167+1;}else{l126[l141->pivots[l167]] = l149;}}}LOG_CLI((BSL_META_U(l74,
"\n")));if(l219){LOG_CLI((BSL_META_U(l74,
"Error: multi-buckets were linked to pivot %d, prev %d, curr %d\n"),l141->
pivots[l219-1],l126[l141->pivots[l219-1]],l149));}}if(l218 == 0){LOG_CLI((
BSL_META_U(l74,"- None - \n")));}LOG_CLI((BSL_META_U(l74,"\n")));if(
SOC_TH_ALPM_SCALE_CHECK(l74,1)){l220 = 2;}l149 = 0;for(l167 = 0;l167<
SOC_TH_ALPM_BUCKET_COUNT(l74);l167+= l220){SHR_BITTEST_RANGE(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l74),l167,l220,l221);if(l221){l149++;}}LOG_CLI((
BSL_META_U(l74,"VRF Route buckets: %5d\n"),l149));if(soc_th_alpm_mode_get(l74
) == 1&&SOC_URPF_STATUS_GET(l74)){l149 = 0;for(l167 = 0;l167<
SOC_TH_ALPM_BUCKET_COUNT(l74);l167+= l220){SHR_BITTEST_RANGE(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l74),l167,l220,l221);if(l221){l149++;}}LOG_CLI((
BSL_META_U(l74,"Global Route buckets: %5d\n"),l149));}sal_free(l126);return
SOC_E_NONE;}static int l224(int l74,int l130,int l122,int l128){int l145,l152
,index;defip_aux_table_entry_t entry;index = l130>>(l122?0:1);l145 = 
soc_mem_read(l74,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l145);if(l122){soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l128);soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l128);l152 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l130&1){soc_mem_field32_set(
l74,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l128);l152 = soc_mem_field32_get(
l74,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l128);l152 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l145 = soc_mem_write(l74,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l145);if(
SOC_URPF_STATUS_GET(l74)){l152++;if(l122){soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l128);soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l128);}else{if(l130&1){
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l128);}else{
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l128);}}
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l152);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l152);index+= (2
*soc_mem_index_count(l74,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l74,
L3_DEFIPm))/2;l145 = soc_mem_write(l74,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
index,&entry);}return l145;}static int l225(int l74,int l226,void*entry,void*
l227,int l228){uint32 l152,l155,l122,l100,l229 = 0;soc_mem_t l111 = L3_DEFIPm
;soc_mem_t l230 = L3_DEFIP_AUX_TABLEm;defip_entry_t l231;int l145 = 
SOC_E_NONE,l87,l232,l116,l207;SOC_IF_ERROR_RETURN(soc_mem_read(l74,l230,
MEM_BLOCK_ANY,l226,l227));l152 = soc_mem_field32_get(l74,l111,entry,VRF_ID_0f
);soc_mem_field32_set(l74,l230,l227,VRF0f,l152);l152 = soc_mem_field32_get(
l74,l111,entry,VRF_ID_1f);soc_mem_field32_set(l74,l230,l227,VRF1f,l152);l152 = 
soc_mem_field32_get(l74,l111,entry,MODE0f);soc_mem_field32_set(l74,l230,l227,
MODE0f,l152);l152 = soc_mem_field32_get(l74,l111,entry,MODE1f);
soc_mem_field32_set(l74,l230,l227,MODE1f,l152);l122 = l152;l152 = 
soc_mem_field32_get(l74,l111,entry,VALID0f);soc_mem_field32_set(l74,l230,l227
,VALID0f,l152);l152 = soc_mem_field32_get(l74,l111,entry,VALID1f);
soc_mem_field32_set(l74,l230,l227,VALID1f,l152);l152 = soc_mem_field32_get(
l74,l111,entry,IP_ADDR_MASK0f);if((l145 = _ipmask2pfx(l152,&l87))<0){return
l145;}l155 = soc_mem_field32_get(l74,l111,entry,IP_ADDR_MASK1f);if((l145 = 
_ipmask2pfx(l155,&l232))<0){return l145;}if(l122){soc_mem_field32_set(l74,
l230,l227,IP_LENGTH0f,l87+l232);soc_mem_field32_set(l74,l230,l227,IP_LENGTH1f
,l87+l232);}else{soc_mem_field32_set(l74,l230,l227,IP_LENGTH0f,l87);
soc_mem_field32_set(l74,l230,l227,IP_LENGTH1f,l232);}l152 = 
soc_mem_field32_get(l74,l111,entry,IP_ADDR0f);soc_mem_field32_set(l74,l230,
l227,IP_ADDR0f,l152);l152 = soc_mem_field32_get(l74,l111,entry,IP_ADDR1f);
soc_mem_field32_set(l74,l230,l227,IP_ADDR1f,l152);l152 = soc_mem_field32_get(
l74,l111,entry,ENTRY_TYPE0f);soc_mem_field32_set(l74,l230,l227,ENTRY_TYPE0f,
l152);l152 = soc_mem_field32_get(l74,l111,entry,ENTRY_TYPE1f);
soc_mem_field32_set(l74,l230,l227,ENTRY_TYPE1f,l152);if(!l122){sal_memcpy(&
l231,entry,sizeof(l231));l145 = soc_th_alpm_lpm_vrf_get(l74,(void*)&l231,&
l116,&l87);SOC_IF_ERROR_RETURN(l145);SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_ip4entry1_to_0(l74,&l231,&l231,PRESERVE_HIT));l145 = 
soc_th_alpm_lpm_vrf_get(l74,(void*)&l231,&l207,&l87);SOC_IF_ERROR_RETURN(l145
);}else{l145 = soc_th_alpm_lpm_vrf_get(l74,entry,&l116,&l87);}if(
SOC_URPF_STATUS_GET(l74)){if(l228>= (soc_mem_index_count(l74,L3_DEFIPm)>>1)){
l229 = 1;}}switch(l116){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l74,l230
,l227,VALID0f,0);l100 = 0;break;case SOC_L3_VRF_GLOBAL:l100 = l229?1:0;break;
default:l100 = l229?3:2;break;}soc_mem_field32_set(l74,l230,l227,DB_TYPE0f,
l100);if(!l122){switch(l207){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l74
,l230,l227,VALID1f,0);l100 = 0;break;case SOC_L3_VRF_GLOBAL:l100 = l229?1:0;
break;default:l100 = l229?3:2;break;}soc_mem_field32_set(l74,l230,l227,
DB_TYPE1f,l100);}else{if(l116 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l74
,l230,l227,VALID1f,0);}soc_mem_field32_set(l74,l230,l227,DB_TYPE1f,l100);}if(
l229){l152 = soc_mem_field32_get(l74,l111,entry,ALG_BKT_PTR0f);if(l152){l152
+= SOC_TH_ALPM_BUCKET_COUNT(l74);soc_mem_field32_set(l74,l111,entry,
ALG_BKT_PTR0f,l152);}l152 = soc_mem_field32_get(l74,l111,entry,
ALG_SUB_BKT_PTR0f);if(l152){soc_mem_field32_set(l74,l111,entry,
ALG_SUB_BKT_PTR0f,l152);}if(!l122){l152 = soc_mem_field32_get(l74,l111,entry,
ALG_BKT_PTR1f);if(l152){l152+= SOC_TH_ALPM_BUCKET_COUNT(l74);
soc_mem_field32_set(l74,l111,entry,ALG_BKT_PTR1f,l152);}l152 = 
soc_mem_field32_get(l74,l111,entry,ALG_SUB_BKT_PTR1f);if(l152){
soc_mem_field32_set(l74,l111,entry,ALG_SUB_BKT_PTR1f,l152);}}}return
SOC_E_NONE;}static int l233(int l74,int l234,int index,int l235,void*entry){
defip_aux_table_entry_t l227;l235 = soc_th_alpm_physical_idx(l74,L3_DEFIPm,
l235,1);SOC_IF_ERROR_RETURN(l225(l74,l235,entry,(void*)&l227,index));
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l74,MEM_BLOCK_ANY,index,entry));index = 
soc_th_alpm_physical_idx(l74,L3_DEFIPm,index,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l74,MEM_BLOCK_ANY,index,&l227));return SOC_E_NONE;}
int _soc_th_alpm_insert_in_bkt(int l74,soc_mem_t l111,int bktid,int l147,void
*alpm_data,uint32*l75,int*l88,int l122){int l145;l145 = soc_mem_alpm_insert(
l74,l111,ALPM_BKT_IDX(bktid),MEM_BLOCK_ANY,l147,alpm_data,l75,l88);if(l145 == 
SOC_E_FULL){if(SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l145 = soc_mem_alpm_insert(
l74,l111,ALPM_BKT_IDX(bktid)+1,MEM_BLOCK_ANY,l147,alpm_data,l75,l88);}}return
l145;}int _soc_th_alpm_mem_index(int l74,soc_mem_t l111,int bucket_index,int
l236,uint32 l147,int*l237){int l167,l180 = 0;int l238[4] = {0};int l239 = 0;
int l240 = 0;int l241;int l242 = 6;int l243;int l244;int l245;int l246 = 0;
switch(l111){case L3_DEFIP_ALPM_IPV6_64m:l242 = 4;break;case
L3_DEFIP_ALPM_IPV6_128m:l242 = 2;break;default:break;}if(
SOC_TH_ALPM_SCALE_CHECK(l74,(l111!= L3_DEFIP_ALPM_IPV4m))){if(l236>= 
ALPM_RAW_BKT_COUNT*l242){bucket_index++;l236-= ALPM_RAW_BKT_COUNT*l242;}}l243
= 4;l244 = 15;l245 = 2;if(soc_th_get_alpm_banks(l74)<= 2){l243 = 2;l244 = 14;
l245 = 1;}l246 = ((1<<l243)-1);l241 = l243-_shr_popcount(l147&l246);if(
bucket_index>= (1<<l244)||l236>= l241*l242){return SOC_E_PARAM;}l240 = l236%
l242;for(l167 = 0;l167<l243;l167++){if((1<<l167)&l147){continue;}l238[l180++]
= l167;}l239 = l238[l236/l242];*l237 = (l240<<l244)|(bucket_index<<l245)|(
l239);return SOC_E_NONE;}static int _soc_th_alpm_move_inval(int l74,soc_mem_t
l111,int l122,alpm_mem_prefix_array_t*l131,int*l91){int l167,l145 = 
SOC_E_NONE,l185;defip_alpm_ipv4_entry_t l189;defip_alpm_ipv6_64_entry_t l191;
int l247,l248;void*l249 = NULL,*l250 = NULL;int*l251 = NULL;int l252 = FALSE;
l247 = l122?sizeof(l191):sizeof(l189);l248 = l247*l131->count;l249 = 
sal_alloc(l248,"rb_bufp");l250 = sal_alloc(l248,"rb_sip_bufp");l251 = 
sal_alloc(sizeof(*l251)*l131->count,"roll_back_index");if(l249 == NULL||l250
== NULL||l251 == NULL){l145 = SOC_E_MEMORY;goto l253;}sal_memset(l251,-1,
sizeof(*l251)*l131->count);for(l167 = 0;l167<l131->count;l167++){payload_t*
prefix = l131->prefix[l167];if(prefix->index>= 0){l145 = soc_mem_read(l74,
l111,MEM_BLOCK_ANY,prefix->index,(uint8*)l249+l167*l247);if(SOC_FAILURE(l145)
){l167--;l252 = TRUE;break;}if(SOC_URPF_STATUS_GET(l74)){l145 = soc_mem_read(
l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,prefix->index),(uint8*)l250
+l167*l247);if(SOC_FAILURE(l145)){l167--;l252 = TRUE;break;}}l145 = 
soc_mem_write(l74,l111,MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null(l74,
l111));if(SOC_FAILURE(l145)){l251[l167] = prefix->index;l252 = TRUE;break;}if
(SOC_URPF_STATUS_GET(l74)){l145 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l74,prefix->index),soc_mem_entry_null(l74,l111));if(
SOC_FAILURE(l145)){l251[l167] = prefix->index;l252 = TRUE;break;}}}l251[l167]
= prefix->index;prefix->index = l91[l167];}if(l252){for(;l167>= 0;l167--){
payload_t*prefix = l131->prefix[l167];prefix->index = l251[l167];if(l251[l167
]<0){continue;}l185 = soc_mem_write(l74,l111,MEM_BLOCK_ALL,l251[l167],(uint8*
)l249+l167*l247);if(SOC_FAILURE(l185)){break;}if(!SOC_URPF_STATUS_GET(l74)){
continue;}l185 = soc_mem_write(l74,l111,MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(
l74,l251[l167]),(uint8*)l250+l167*l247);if(SOC_FAILURE(l185)){break;}}}l253:
if(l251!= NULL){sal_free(l251);}if(l250!= NULL){sal_free(l250);}if(l249!= 
NULL){sal_free(l249);}return l145;}void _soc_th_alpm_rollback_pivot_add(int
l74,defip_entry_t*l137,void*key_data,int tcam_index,alpm_pivot_t*pivot_pyld){
int l145;trie_t*l159 = NULL;int l122,l116,l162;trie_node_t*l254 = NULL;l122 = 
soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);(void)
soc_th_alpm_lpm_vrf_get(l74,key_data,&l162,&l116);l145 = 
soc_th_alpm_lpm_delete(l74,l137);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: Failure to free new prefix""at %d\n"),
soc_th_alpm_logical_idx(l74,L3_DEFIPm,tcam_index,l122)));}if(l122){l159 = 
VRF_PIVOT_TRIE_IPV6(l74,l116);}else{l159 = VRF_PIVOT_TRIE_IPV4(l74,l116);}if(
ALPM_TCAM_PIVOT(l74,tcam_index<<(l122?1:0))!= NULL){l145 = trie_delete(l159,
pivot_pyld->key,pivot_pyld->len,&l254);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_insert: trie delete failure "
"in bkt move rollback\n")));}}ALPM_TCAM_PIVOT(l74,tcam_index<<(l122?1:0)) = 
NULL;VRF_PIVOT_REF_DEC(l74,l116,l122);}int _soc_th_alpm_rollback_bkt_move(int
l74,void*key_data,soc_mem_t l111,alpm_pivot_t*l133,alpm_pivot_t*l134,
alpm_mem_prefix_array_t*l135,int*l91,int l136){trie_node_t*l254 = NULL;uint32
prefix[5],l127;trie_t*l125;int l122,l116,l162,l167,l113 = 0;;
defip_alpm_ipv4_entry_t l189;defip_alpm_ipv6_64_entry_t l191;
defip_alpm_ipv6_128_entry_t l255;void*l187;payload_t*l256;int l145 = 
SOC_E_NONE;alpm_bucket_handle_t*l257;l257 = PIVOT_BUCKET_HANDLE(l134);l122 = 
soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);if(l111 == 
L3_DEFIP_ALPM_IPV6_128m){l122 = L3_DEFIP_MODE_128;}if(l111 == 
L3_DEFIP_ALPM_IPV6_128m){l145 = _alpm_128_prefix_create(l74,key_data,prefix,&
l127,&l113);}else{l145 = l153(l74,key_data,prefix,&l127,&l113);}if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"prefix create failed\n")));return l145;}if(l111 == L3_DEFIP_ALPM_IPV6_128m){
l187 = ((uint32*)&(l255));(void)soc_th_alpm_128_lpm_vrf_get(l74,key_data,&
l162,&l116);}else{l187 = ((l122)?((uint32*)&(l191)):((uint32*)&(l189)));(void
)soc_th_alpm_lpm_vrf_get(l74,key_data,&l162,&l116);}if(l111 == 
L3_DEFIP_ALPM_IPV6_128m){l125 = VRF_PREFIX_TRIE_IPV6_128(l74,l116);}else if(
l122){l125 = VRF_PREFIX_TRIE_IPV6(l74,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4
(l74,l116);}for(l167 = 0;l167<l135->count;l167++){payload_t*l87 = l135->
prefix[l167];if(l91[l167]!= -1){if(l111 == L3_DEFIP_ALPM_IPV6_128m){
sal_memset(l187,0,sizeof(defip_alpm_ipv6_128_entry_t));}else if(l122){
sal_memset(l187,0,sizeof(defip_alpm_ipv6_64_entry_t));}else{sal_memset(l187,0
,sizeof(defip_alpm_ipv4_entry_t));}l145 = soc_mem_write(l74,l111,
MEM_BLOCK_ANY,l91[l167],l187);_soc_tomahawk_alpm_bkt_view_set(l74,l91[l167],
INVALIDm);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l74)){l145 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l74,l91[l167]),l187);_soc_tomahawk_alpm_bkt_view_set(
l74,_soc_th_alpm_rpf_entry(l74,l91[l167]),INVALIDm);if(SOC_FAILURE(l145)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l254 = NULL;l145 = trie_delete(
PIVOT_BUCKET_TRIE(l134),l87->key,l87->len,&l254);l256 = (payload_t*)l254;if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(l87->
index>0){l145 = trie_insert(PIVOT_BUCKET_TRIE(l133),l87->key,NULL,l87->len,(
trie_node_t*)l256);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l256!= NULL){sal_free(l256);}}}if(l136
== -1){l254 = NULL;l145 = trie_delete(PIVOT_BUCKET_TRIE(l133),prefix,l127,&
l254);l256 = (payload_t*)l254;if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l74,"_soc_th_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l256!= 
NULL){sal_free(l256);}}l145 = soc_th_alpm_bs_free(l74,PIVOT_BUCKET_INDEX(l134
),l116,l122);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: new bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(
l134)));}trie_destroy(PIVOT_BUCKET_TRIE(l134));sal_free(l257);sal_free(l134);
sal_free(l91);l254 = NULL;l145 = trie_delete(l125,prefix,l127,&l254);l256 = (
payload_t*)l254;if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"_soc_th_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l256){sal_free(
l256);}return l145;}int _soc_th_alpm_free_pfx_trie(int l74,trie_t*l125,trie_t
*l258,payload_t*new_pfx_pyld,int*l91,int bktid,int l116,int l122){int l145 = 
SOC_E_NONE;trie_node_t*l254 = NULL;payload_t*l259 = NULL;payload_t*l260 = 
NULL;if(l91!= NULL){sal_free(l91);}(void)trie_delete(l125,new_pfx_pyld->key,
new_pfx_pyld->len,&l254);l260 = (payload_t*)l254;if(l260!= NULL){(void)
trie_delete(l258,l260->key,l260->len,&l254);l259 = (payload_t*)l254;if(l259!= 
NULL){sal_free(l259);}sal_free(l260);}if(bktid!= -1){l145 = 
soc_th_alpm_bs_free(l74,bktid,l116,l122);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d Unable to free bucket_id %d"),l74,
bktid));}}return SOC_E_NONE;}static int l123(int l74,alpm_pfx_info_t*l124,
trie_t*l125,uint32*l126,uint32 l127,trie_node_t*l261,defip_entry_t*lpm_entry,
uint32*l128){trie_node_t*l160 = NULL;int l122,l116,l162;
defip_alpm_ipv4_entry_t l189;defip_alpm_ipv6_64_entry_t l191;payload_t*l262 = 
NULL;int l263;void*l187;alpm_pivot_t*l264;alpm_bucket_handle_t*l257;int l145 = 
SOC_E_NONE;soc_mem_t l111;l264 = l124->pivot_pyld;l263 = l264->tcam_index;
l122 = soc_mem_field32_get(l74,L3_DEFIPm,l124->key_data,MODE0f);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,l124->key_data,&l162,&l116));
l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l187 = ((l122)?((
uint32*)&(l191)):((uint32*)&(l189)));l160 = NULL;l145 = trie_find_lpm(l125,
l126,l127,&l160);l262 = (payload_t*)l160;if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l74,l126[0],l126[1],l126
[2],l126[3],l126[4],l127));return l145;}if(l262->bkt_ptr){if(l262->bkt_ptr == 
l124->new_pfx_pyld){sal_memcpy(l187,l124->alpm_data,l122?sizeof(
defip_alpm_ipv6_64_entry_t):sizeof(defip_alpm_ipv4_entry_t));}else{l145 = 
soc_mem_read(l74,l111,MEM_BLOCK_ANY,((payload_t*)l262->bkt_ptr)->index,l187);
}if(SOC_FAILURE(l145)){return l145;}l145 = l115(l74,l187,l111,l122,l162,l124
->bktid,0,lpm_entry);if(SOC_FAILURE(l145)){return l145;}*l128 = ((payload_t*)
(l262->bkt_ptr))->len;}else{l145 = soc_mem_read(l74,L3_DEFIPm,MEM_BLOCK_ANY,
soc_th_alpm_logical_idx(l74,L3_DEFIPm,l263>>1,1),lpm_entry);if((!l122)&&(l263
&1)){l145 = soc_th_alpm_lpm_ip4entry1_to_0(l74,lpm_entry,lpm_entry,0);}}l257 = 
sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l257 == NULL)
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l145 = SOC_E_MEMORY;return l145;}sal_memset(l257,0,sizeof(*l257));l264 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l264 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l145 = SOC_E_MEMORY;return l145;}sal_memset(l264,0,sizeof(*l264));
PIVOT_BUCKET_HANDLE(l264) = l257;if(l122){l145 = trie_init(_MAX_KEY_LEN_144_,
&PIVOT_BUCKET_TRIE(l264));}else{l145 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l264));}PIVOT_BUCKET_TRIE(l264)->trie = l261;
PIVOT_BUCKET_INDEX(l264) = l124->bktid;PIVOT_BUCKET_VRF(l264) = l116;
PIVOT_BUCKET_IPV6(l264) = l122;PIVOT_BUCKET_DEF(l264) = FALSE;(l264)->key[0] = 
l126[0];(l264)->key[1] = l126[1];(l264)->key[2] = l126[2];(l264)->key[3] = 
l126[3];(l264)->key[4] = l126[4];(l264)->len = l127;do{if(!(l122)){l126[0] = 
(((32-l127) == 32)?0:(l126[1])<<(32-l127));l126[1] = 0;}else{int l265 = 64-
l127;int l266;if(l265<32){l266 = l126[3]<<l265;l266|= (((32-l265) == 32)?0:(
l126[4])>>(32-l265));l126[0] = l126[4]<<l265;l126[1] = l266;l126[2] = l126[3]
= l126[4] = 0;}else{l126[1] = (((l265-32) == 32)?0:(l126[4])<<(l265-32));l126
[0] = l126[2] = l126[3] = l126[4] = 0;}}}while(0);l118(l74,l126,l127,l116,
l122,lpm_entry,0,0);soc_L3_DEFIPm_field32_set(l74,lpm_entry,ALG_BKT_PTR0f,
ALPM_BKT_IDX(l124->bktid));soc_L3_DEFIPm_field32_set(l74,lpm_entry,
ALG_SUB_BKT_PTR0f,ALPM_BKT_SIDX(l124->bktid));l124->pivot_pyld = l264;return
l145;}static int l129(int l74,alpm_pfx_info_t*l124,int*l130,int*l104){
trie_node_t*l261;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int l122,l116,l162;
uint32 l127,l128 = 0;uint32 l147 = 0;uint32 l126[5];int l88;
defip_alpm_ipv4_entry_t l189,l190;defip_alpm_ipv6_64_entry_t l191,l192;trie_t
*l125,*trie;void*l187,*l188;alpm_pivot_t*l267 = l124->pivot_pyld;
defip_entry_t lpm_entry;soc_mem_t l111;trie_t*l159 = NULL;
alpm_mem_prefix_array_t l173;int*l91 = NULL;int*l90 = NULL;int l145 = 
SOC_E_NONE,l167,l136 = -1;int tcam_index,l268,l269 = 0;int l270 = 0,l271 = 0;
l122 = soc_mem_field32_get(l74,L3_DEFIPm,l124->key_data,MODE0f);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,l124->key_data,&l162,&l116));
l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l187 = ((l122)?((
uint32*)&(l191)):((uint32*)&(l189)));l188 = ((l122)?((uint32*)&(l192)):((
uint32*)&(l190)));soc_th_alpm_bank_db_type_get(l74,l116,&l147,NULL);if(l122){
l125 = VRF_PREFIX_TRIE_IPV6(l74,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4(l74,
l116);}trie = PIVOT_BUCKET_TRIE(l124->pivot_pyld);l269 = l124->bktid;l268 = 
PIVOT_TCAM_INDEX(l267);l145 = soc_th_alpm_bs_alloc(l74,&l124->bktid,l116,l122
);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: Unable to allocate""new bucket for split\n")));l124->
bktid = -1;_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,l91,
l124->bktid,l116,l122);return l145;}l145 = trie_split(trie,l122?
_MAX_KEY_LEN_144_:_MAX_KEY_LEN_48_,FALSE,l126,&l127,&l261,NULL,FALSE);if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: Could not split bucket")));_soc_th_alpm_free_pfx_trie(
l74,l125,trie,l124->new_pfx_pyld,l91,l124->bktid,l116,l122);return l145;}l145
= l123(l74,l124,l125,l126,l127,l261,&lpm_entry,&l128);if(l145!= SOC_E_NONE){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"could not initialize pivot")));
_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,l91,l124->bktid,
l116,l122);return l145;}sal_memset(&l173,0,sizeof(l173));l145 = trie_traverse
(PIVOT_BUCKET_TRIE(l124->pivot_pyld),_soc_th_alpm_mem_prefix_array_cb,&l173,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"_soc_th_alpm_insert: Bucket split failed")));
_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,l91,l124->bktid,
l116,l122);return l145;}l91 = sal_alloc(sizeof(*l91)*l173.count,"new_index");
if(l91 == NULL){_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,
l91,l124->bktid,l116,l122);return SOC_E_MEMORY;}l90 = sal_alloc(sizeof(*l90)*
l173.count,"new_index");if(l90 == NULL){_soc_th_alpm_free_pfx_trie(l74,l125,
trie,l124->new_pfx_pyld,l91,l124->bktid,l116,l122);return SOC_E_MEMORY;}
sal_memset(l91,-1,sizeof(*l91)*l173.count);sal_memset(l90,-1,sizeof(*l90)*
l173.count);for(l167 = 0;l167<l173.count;l167++){payload_t*l87 = l173.prefix[
l167];if(l87->index>0){l145 = soc_mem_read(l74,l111,MEM_BLOCK_ANY,l87->index,
l187);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: Failed to""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l173.prefix[l167]->key[1],l173.
prefix[l167]->key[2],l173.prefix[l167]->key[3],l173.prefix[l167]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l267,l124->
pivot_pyld,&l173,l91,l136);sal_free(l90);return l145;}if(SOC_URPF_STATUS_GET(
l74)){l145 = soc_mem_read(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,
l87->index),l188);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l74,"_soc_th_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l173.prefix[l167]->key[1],l173.
prefix[l167]->key[2],l173.prefix[l167]->key[3],l173.prefix[l167]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l267,l124->
pivot_pyld,&l173,l91,l136);sal_free(l90);return l145;}}soc_mem_field32_set(
l74,l111,l187,SUB_BKT_PTRf,ALPM_BKT_SIDX(l124->bktid));soc_mem_field32_set(
l74,l111,l188,SUB_BKT_PTRf,ALPM_BKT_SIDX(l124->bktid));l145 = 
_soc_th_alpm_insert_in_bkt(l74,l111,l124->bktid,l147,l187,l75,&l88,l122);if(
SOC_SUCCESS(l145)){if(SOC_URPF_STATUS_GET(l74)){l145 = soc_mem_write(l74,l111
,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),l188);}l270++;l271++;}}else{
soc_mem_field32_set(l74,l111,l124->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(l124
->bktid));l145 = _soc_th_alpm_insert_in_bkt(l74,l111,l124->bktid,l147,l124->
alpm_data,l75,&l88,l122);if(SOC_SUCCESS(l145)){l136 = l167;*l104 = l88;if(
SOC_URPF_STATUS_GET(l74)){soc_mem_field32_set(l74,l111,l124->alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l124->bktid));l145 = soc_mem_write(l74,l111,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),l124->alpm_sip_data);}l271++;}}
l91[l167] = l88;if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"_soc_th_alpm_insert: Failed to ""insert prefix "
"0x%08x 0x%08x 0x%08x 0x%08x to bucket %d\n"),l173.prefix[l167]->key[1],l173.
prefix[l167]->key[2],l173.prefix[l167]->key[3],l173.prefix[l167]->key[4],l124
->bktid));(void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l267,
l124->pivot_pyld,&l173,l91,l136);sal_free(l90);return l145;}l90[l167] = l87->
index;}l145 = l96(l74,&lpm_entry,l130);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_th_alpm_insert: Unable to add new"
"pivot to tcam\n")));if(l145 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l74,l116,l122)
;}(void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l267,l124->
pivot_pyld,&l173,l91,l136);sal_free(l90);return l145;}*l130 = 
soc_th_alpm_physical_idx(l74,L3_DEFIPm,*l130,l122);l145 = l224(l74,*l130,l122
,l128);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l130));
_soc_th_alpm_rollback_pivot_add(l74,&lpm_entry,l124->key_data,*l130,l124->
pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l267
,l124->pivot_pyld,&l173,l91,l136);sal_free(l90);return l145;}if(l122){l159 = 
VRF_PIVOT_TRIE_IPV6(l74,l116);}else{l159 = VRF_PIVOT_TRIE_IPV4(l74,l116);}
l145 = trie_insert(l159,l124->pivot_pyld->key,NULL,l124->pivot_pyld->len,(
trie_node_t*)l124->pivot_pyld);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"failed to insert into pivot trie\n")));(void
)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l267,l124->pivot_pyld
,&l173,l91,l136);sal_free(l90);return l145;}tcam_index = *l130<<(l122?1:0);
ALPM_TCAM_PIVOT(l74,tcam_index) = l124->pivot_pyld;PIVOT_TCAM_INDEX(l124->
pivot_pyld) = tcam_index;VRF_PIVOT_REF_INC(l74,l116,l122);l145 = 
_soc_th_alpm_move_inval(l74,l111,l122,&l173,l91);if(SOC_FAILURE(l145)){
_soc_th_alpm_rollback_pivot_add(l74,&lpm_entry,l124->key_data,*l130,l124->
pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l267
,l124->pivot_pyld,&l173,l91,l136);sal_free(l91);l91 = NULL;sal_free(l90);
return l145;}l145 = soc_th_alpm_update_hit_bits(l74,l173.count,l90,l91);if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l173.count));l145 = 
SOC_E_NONE;}sal_free(l91);l91 = NULL;sal_free(l90);if(l136 == -1){
soc_mem_field32_set(l74,l111,l124->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(
PIVOT_BUCKET_INDEX(l267)));l145 = _soc_th_alpm_insert_in_bkt(l74,l111,
PIVOT_BUCKET_INDEX(l267),l147,l124->alpm_data,l75,&l88,l122);if(SOC_FAILURE(
l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: Could not insert new ""prefix into trie after split\n")
));_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,l91,l124->
bktid,l116,l122);return l145;}l270--;if(SOC_URPF_STATUS_GET(l74)){l145 = 
soc_mem_write(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),l124->
alpm_sip_data);}*l104 = l88;l124->new_pfx_pyld->index = l88;}
soc_th_alpm_bu_upd(l74,l269,l268,FALSE,l122,-l270);soc_th_alpm_bu_upd(l74,
l124->bktid,tcam_index,l116,l122,l271);PIVOT_BUCKET_ENT_CNT_UPDATE(l124->
pivot_pyld);VRF_BUCKET_SPLIT_INC(l74,l116,l122);return l145;}static int l272(
int l74,void*key_data,soc_mem_t l111,void*alpm_data,void*alpm_sip_data,int*
l104,int bktid,int tcam_index){alpm_pivot_t*l264,*l267;
defip_aux_scratch_entry_t l102;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l273,l127;int l122,l116,l162;int l88;int l145 = SOC_E_NONE,l185;
uint32 l100,l147;int l158 =0;int l130;int l274 = 0;trie_t*trie,*l125;
trie_node_t*l160 = NULL,*l254 = NULL;payload_t*l256,*l275,*l262;int l113 = 0;
alpm_pfx_info_t l124;l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f
);if(l122){if(!(l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE1f))){
return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,
key_data,&l162,&l116));soc_th_alpm_bank_db_type_get(l74,l116,&l147,&l100);
l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l145 = l153(l74,
key_data,prefix,&l127,&l113);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l74,"_soc_th_alpm_insert: prefix create failed\n")));return l145;
}sal_memset(&l102,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(
l98(l74,key_data,l122,l100,0,&l102));if(bktid == 0){l145 = l157(l74,prefix,
l127,l122,l116,&l158,&tcam_index,&bktid);SOC_IF_ERROR_RETURN(l145);
soc_mem_field32_set(l74,l111,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));}
l145 = _soc_th_alpm_insert_in_bkt(l74,l111,bktid,l147,alpm_data,l75,&l88,l122
);if(l145 == SOC_E_NONE){*l104 = l88;if(SOC_URPF_STATUS_GET(l74)){
soc_mem_field32_set(l74,l111,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid))
;l185 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),
alpm_sip_data);if(SOC_FAILURE(l185)){return l185;}}}if(l145 == SOC_E_FULL){if
(PIVOT_BUCKET_COUNT(ALPM_TCAM_PIVOT(l74,tcam_index))<= 3){int l276;l145 = 
soc_th_alpm_bs_alloc(l74,&l276,l116,l122);SOC_IF_ERROR_RETURN(l145);l145 = 
_soc_th_alpm_move_trie(l74,l122,bktid,l276);SOC_IF_ERROR_RETURN(l145);bktid = 
l276;soc_mem_field32_set(l74,l111,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid)
);l145 = _soc_th_alpm_insert_in_bkt(l74,l111,bktid,l147,alpm_data,l75,&l88,
l122);if(l145 == SOC_E_NONE){*l104 = l88;if(SOC_URPF_STATUS_GET(l74)){
soc_mem_field32_set(l74,l111,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid))
;l185 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),
alpm_sip_data);if(SOC_FAILURE(l185)){return l185;}}}}else{l274 = 1;}}l264 = 
ALPM_TCAM_PIVOT(l74,tcam_index);trie = PIVOT_BUCKET_TRIE(l264);l267 = l264;
l256 = sal_alloc(sizeof(payload_t),"Payload for Key");if(l256 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 payload entry\n")));
return SOC_E_MEMORY;}l275 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l275 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_vrf_add: unable to allocate memory for "
"pfx trie node\n")));sal_free(l256);return SOC_E_MEMORY;}sal_memset(l256,0,
sizeof(*l256));sal_memset(l275,0,sizeof(*l275));sal_memcpy(l256->key,prefix,
sizeof(prefix));l256->len = l127;l256->index = l88;sal_memcpy(l275,l256,
sizeof(*l256));l275->bkt_ptr = l256;l145 = trie_insert(trie,prefix,NULL,l127,
(trie_node_t*)l256);if(SOC_FAILURE(l145)){if(l256!= NULL){sal_free(l256);}if(
l275!= NULL){sal_free(l275);}return l145;}if(l122){l125 = 
VRF_PREFIX_TRIE_IPV6(l74,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4(l74,l116);}
if(!l113){l145 = trie_insert(l125,prefix,NULL,l127,(trie_node_t*)l275);}else{
l160 = NULL;l145 = trie_find_lpm(l125,0,0,&l160);l262 = (payload_t*)l160;if(
SOC_SUCCESS(l145)){l262->bkt_ptr = l256;}}l273 = l127;if(SOC_FAILURE(l145)){
l254 = NULL;(void)trie_delete(trie,prefix,l273,&l254);l262 = (payload_t*)l254
;sal_free(l262);sal_free(l275);return l145;}if(l274){l124.key_data = key_data
;l124.new_pfx_pyld = l256;l124.pivot_pyld = l264;l124.alpm_data = alpm_data;
l124.alpm_sip_data = alpm_sip_data;l124.bktid = bktid;l145 = l129(l74,&l124,&
l130,l104);if(l145!= SOC_E_NONE){return l145;}bktid = l124.bktid;tcam_index = 
PIVOT_TCAM_INDEX(l124.pivot_pyld);}else{soc_th_alpm_bu_upd(l74,bktid,
tcam_index,l116,l122,1);}VRF_TRIE_ROUTES_INC(l74,l116,l122);if(l113){sal_free
(l275);}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l74,DELETE_PROPAGATE,&l102,
TRUE,&l158,&tcam_index,&bktid));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l74,
INSERT_PROPAGATE,&l102,FALSE,&l158,&tcam_index,&bktid));if(
SOC_URPF_STATUS_GET(l74)){l127 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf);l127+= 1;soc_mem_field32_set(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf,l127);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,DELETE_PROPAGATE,&l102,TRUE,&l158,&tcam_index,&bktid)
);SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l74,INSERT_PROPAGATE,&l102,FALSE,&
l158,&tcam_index,&bktid));}PIVOT_BUCKET_ENT_CNT_UPDATE(l267);return l145;}
static int l118(int unit,uint32*key,int len,int l116,int l99,defip_entry_t*
lpm_entry,int l119,int l120){uint32 l277;if(l120){sal_memset(lpm_entry,0,
sizeof(defip_entry_t));}soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_0f,
l116&SOC_VRF_MAX(unit));if(l116 == (SOC_VRF_MAX(unit)+1)){
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_MASK0f,0);}else{
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(unit));}if
(l99){soc_L3_DEFIPm_field32_set(unit,lpm_entry,IP_ADDR0f,key[0]);
soc_L3_DEFIPm_field32_set(unit,lpm_entry,IP_ADDR1f,key[1]);
soc_L3_DEFIPm_field32_set(unit,lpm_entry,MODE0f,1);soc_L3_DEFIPm_field32_set(
unit,lpm_entry,MODE1f,1);soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_1f,
l116&SOC_VRF_MAX(unit));if(l116 == (SOC_VRF_MAX(unit)+1)){
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_MASK1f,0);}else{
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_MASK1f,SOC_VRF_MAX(unit));}
soc_L3_DEFIPm_field32_set(unit,lpm_entry,VALID1f,1);if(len>= 32){l277 = 
0xffffffff;soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f,l277);
l277 = ~(((len-32) == 32)?0:(0xffffffff)>>(len-32));soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l277);}else{l277 = ~(0xffffffff>>len);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f,l277);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,0);}}else{
soc_L3_DEFIPm_field32_set(unit,lpm_entry,IP_ADDR0f,key[0]);assert(len<= 32);
l277 = (len == 32)?0xffffffff:~(0xffffffff>>len);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l277);}soc_L3_DEFIPm_field32_set(unit,
lpm_entry,VALID0f,1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE_MASK0f
,(1<<soc_mem_field_length(unit,L3_DEFIPm,MODE_MASK0f))-1);soc_mem_field32_set
(unit,L3_DEFIPm,lpm_entry,MODE_MASK1f,(1<<soc_mem_field_length(unit,L3_DEFIPm
,MODE_MASK1f))-1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
ENTRY_TYPE_MASK0f,(1<<soc_mem_field_length(unit,L3_DEFIPm,ENTRY_TYPE_MASK0f))
-1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK1f,(1<<
soc_mem_field_length(unit,L3_DEFIPm,ENTRY_TYPE_MASK1f))-1);return(SOC_E_NONE)
;}int _soc_th_alpm_delete_in_bkt(int l74,soc_mem_t l111,int l278,int l147,
void*l279,uint32*l75,int*l88,int l122){int l145;l145 = soc_mem_alpm_delete(
l74,l111,ALPM_BKT_IDX(l278),MEM_BLOCK_ALL,l147,l279,l75,l88);if(SOC_SUCCESS(
l145)){return l145;}if(SOC_TH_ALPM_SCALE_CHECK(l74,l122)){return
soc_mem_alpm_delete(l74,l111,ALPM_BKT_IDX(l278)+1,MEM_BLOCK_ALL,l147,l279,l75
,l88);}return l145;}static int l280(int l74,void*key_data,int bktid,int
tcam_index,int l88){alpm_pivot_t*pivot_pyld;defip_alpm_ipv4_entry_t l189,l190
,l281;defip_alpm_ipv6_64_entry_t l191,l192,l282;defip_aux_scratch_entry_t l102
;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l111;void*l187,*l279,*l188 = 
NULL;int l162;int l99;int l145 = SOC_E_NONE,l185;uint32 l283[5],prefix[5];int
l122,l116;uint32 l127;int l278;uint32 l100,l147;int l158,l113 = 0;trie_t*trie
,*l125;uint32 l284;defip_entry_t lpm_entry,*l285;payload_t*l256 = NULL,*l286 = 
NULL,*l262 = NULL;trie_node_t*l254 = NULL,*l160 = NULL;trie_t*l159 = NULL;l99
= l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);if(l122){if(!(
l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l162
,&l116));if(l162!= SOC_L3_VRF_OVERRIDE){soc_th_alpm_bank_db_type_get(l74,l116
,&l147,&l100);l145 = l153(l74,key_data,prefix,&l127,&l113);if(SOC_FAILURE(
l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: prefix create failed\n")));return l145;}if(!
soc_th_alpm_mode_get(l74)){if(l162!= SOC_L3_VRF_GLOBAL){if(
VRF_TRIE_ROUTES_CNT(l74,l116,l122)>1){if(l113){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"VRF %d: Cannot delete default "
"route if other routes are present in ""this mode"),l116));return SOC_E_PARAM
;}}}l100 = 2;}l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l187 = 
((l122)?((uint32*)&(l191)):((uint32*)&(l189)));l279 = ((l122)?((uint32*)&(
l282)):((uint32*)&(l281)));SOC_ALPM_LPM_LOCK(l74);if(bktid == 0){l145 = 
_soc_th_alpm_find(l74,key_data,l111,l187,&tcam_index,&bktid,&l88,TRUE);}else{
l145 = l108(l74,key_data,l187,0,l111,0,0,bktid);}sal_memcpy(l279,l187,l122?
sizeof(l191):sizeof(l189));if(SOC_FAILURE(l145)){SOC_ALPM_LPM_UNLOCK(l74);
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Unable to find prefix for delete\n")));return l145;}l278 = 
bktid;pivot_pyld = ALPM_TCAM_PIVOT(l74,tcam_index);trie = PIVOT_BUCKET_TRIE(
pivot_pyld);l145 = trie_delete(trie,prefix,l127,&l254);l256 = (payload_t*)
l254;if(l145!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l74);return l145;}if(l122){l125 = VRF_PREFIX_TRIE_IPV6(
l74,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4(l74,l116);}if(l122){l159 = 
VRF_PIVOT_TRIE_IPV6(l74,l116);}else{l159 = VRF_PIVOT_TRIE_IPV4(l74,l116);}if(
!l113){l145 = trie_delete(l125,prefix,l127,&l254);l286 = (payload_t*)l254;if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l287;}l160 = NULL;l145 = trie_find_lpm(l125,prefix,l127,&
l160);l262 = (payload_t*)l160;if(SOC_SUCCESS(l145)){payload_t*l288 = (
payload_t*)(l262->bkt_ptr);if(l288!= NULL){l284 = l288->len;}else{l284 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l289;}
sal_memcpy(l283,prefix,sizeof(prefix));do{if(!(l122)){l283[0] = (((32-l127) == 
32)?0:(l283[1])<<(32-l127));l283[1] = 0;}else{int l265 = 64-l127;int l266;if(
l265<32){l266 = l283[3]<<l265;l266|= (((32-l265) == 32)?0:(l283[4])>>(32-l265
));l283[0] = l283[4]<<l265;l283[1] = l266;l283[2] = l283[3] = l283[4] = 0;}
else{l283[1] = (((l265-32) == 32)?0:(l283[4])<<(l265-32));l283[0] = l283[2] = 
l283[3] = l283[4] = 0;}}}while(0);l145 = l118(l74,prefix,l284,l116,l99,&
lpm_entry,0,1);l185 = _soc_th_alpm_find(l74,&lpm_entry,l111,l187,&tcam_index,
&bktid,&l88,TRUE);(void)l115(l74,l187,l111,l99,l162,bktid,0,&lpm_entry);(void
)l118(l74,l283,l127,l116,l99,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l74)){if(
SOC_SUCCESS(l145)){l188 = ((l122)?((uint32*)&(l192)):((uint32*)&(l190)));l185
= soc_mem_read(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,
ALPM_ENT_INDEX(l88)),l188);}}if((l284 == 0)&&SOC_FAILURE(l185)){l285 = l122?
VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l116):VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l116);
sal_memcpy(&lpm_entry,l285,sizeof(lpm_entry));l145 = l118(l74,l283,l127,l116,
l99,&lpm_entry,0,1);}if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l289;}l285 = 
&lpm_entry;}else{l160 = NULL;l145 = trie_find_lpm(l125,prefix,l127,&l160);
l262 = (payload_t*)l160;if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),l116));goto l287;}l262->bkt_ptr = NULL;l284
= 0;l285 = l122?VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l116):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l116);}l145 = l98(l74,l285,l122,l100,l284,&
l102);if(SOC_FAILURE(l145)){goto l289;}l145 = _soc_th_alpm_aux_op(l74,
DELETE_PROPAGATE,&l102,TRUE,&l158,&tcam_index,&bktid);if(SOC_FAILURE(l145)){
goto l289;}if(SOC_URPF_STATUS_GET(l74)){uint32 l152;if(l188!= NULL){l152 = 
soc_mem_field32_get(l74,L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf);l152++;
soc_mem_field32_set(l74,L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf,l152);l152 = 
soc_mem_field32_get(l74,l111,l188,SRC_DISCARDf);soc_mem_field32_set(l74,l111,
&l102,SRC_DISCARDf,l152);l152 = soc_mem_field32_get(l74,l111,l188,
DEFAULTROUTEf);soc_mem_field32_set(l74,l111,&l102,DEFAULTROUTEf,l152);l145 = 
_soc_th_alpm_aux_op(l74,DELETE_PROPAGATE,&l102,TRUE,&l158,&tcam_index,&bktid)
;}if(SOC_FAILURE(l145)){goto l289;}}sal_free(l256);if(!l113){sal_free(l286);}
PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);if((pivot_pyld->len!= 0)&&(trie->trie
== NULL)){uint32 l290[5];sal_memcpy(l290,pivot_pyld->key,sizeof(l290));do{if(
!(l99)){l290[0] = (((32-pivot_pyld->len) == 32)?0:(l290[1])<<(32-pivot_pyld->
len));l290[1] = 0;}else{int l265 = 64-pivot_pyld->len;int l266;if(l265<32){
l266 = l290[3]<<l265;l266|= (((32-l265) == 32)?0:(l290[4])>>(32-l265));l290[0
] = l290[4]<<l265;l290[1] = l266;l290[2] = l290[3] = l290[4] = 0;}else{l290[1
] = (((l265-32) == 32)?0:(l290[4])<<(l265-32));l290[0] = l290[2] = l290[3] = 
l290[4] = 0;}}}while(0);l118(l74,l290,pivot_pyld->len,l116,l99,&lpm_entry,0,1
);l145 = soc_th_alpm_lpm_delete(l74,&lpm_entry);if(SOC_FAILURE(l145)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_delete: Unable to "
"delete pivot 0x%08x 0x%08x \n"),pivot_pyld->key[0],pivot_pyld->key[1]));}}
l145 = _soc_th_alpm_delete_in_bkt(l74,l111,l278,l147,l279,l75,&l88,l122);if(
SOC_FAILURE(l145)){SOC_ALPM_LPM_UNLOCK(l74);return l145;}if(
SOC_URPF_STATUS_GET(l74)){l145 = soc_mem_alpm_delete(l74,l111,
SOC_TH_ALPM_RPF_BKT_IDX(l74,ALPM_BKT_IDX(l278)),MEM_BLOCK_ALL,l147,l279,l75,&
l158);if(SOC_FAILURE(l145)){SOC_ALPM_LPM_UNLOCK(l74);return l145;}}
soc_th_alpm_bu_upd(l74,PIVOT_BUCKET_INDEX(pivot_pyld),PIVOT_TCAM_INDEX(
pivot_pyld),((pivot_pyld->len!= 0)&&(trie->trie == NULL))?TRUE:FALSE,l122,-1)
;if((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l145 = soc_th_alpm_bs_free(
l74,PIVOT_BUCKET_INDEX(pivot_pyld),l116,l122);if(SOC_FAILURE(l145)){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_delete: Unable to release"
"empty bucket: %d\n"),PIVOT_BUCKET_INDEX(pivot_pyld)));}l145 = trie_delete(
l159,pivot_pyld->key,pivot_pyld->len,&l254);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"could not delete pivot from pivot trie\n")))
;}trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(
pivot_pyld));sal_free(pivot_pyld);l88 = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,
ALPM_BKT_IDX(l278));_soc_tomahawk_alpm_bkt_view_set(l74,l88,INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l88 = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,
ALPM_BKT_IDX(l278)+1);_soc_tomahawk_alpm_bkt_view_set(l74,l88,INVALIDm);}}}
VRF_TRIE_ROUTES_DEC(l74,l116,l122);if(VRF_TRIE_ROUTES_CNT(l74,l116,l122) == 0
){l145 = l121(l74,l116,l122);}SOC_ALPM_LPM_UNLOCK(l74);return l145;l289:
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"recovering soc_th_alpm_vrf_delete failed\n ")));l185 = trie_insert(l125,
prefix,NULL,l127,(trie_node_t*)l286);if(SOC_FAILURE(l185)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into pfx trie\n"),prefix[0],prefix[1]));}l287:l185 = 
trie_insert(trie,prefix,NULL,l127,(trie_node_t*)l256);if(SOC_FAILURE(l185)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into bkt trie\n")
,prefix[0],prefix[1]));}SOC_ALPM_LPM_UNLOCK(l74);return l145;}void l291(int
l74){int l167;if(alpm_vrf_handle[l74]!= NULL){sal_free(alpm_vrf_handle[l74]);
alpm_vrf_handle[l74] = NULL;}if(tcam_pivot[l74]!= NULL){sal_free(tcam_pivot[
l74]);tcam_pivot[l74] = NULL;}if(bkt_usage[l74]!= NULL){sal_free(bkt_usage[
l74]);bkt_usage[l74] = NULL;}for(l167 = 0;l167<SOC_TH_MAX_ALPM_VIEWS;l167++){
if(global_bkt_usage[l74][l167]!= NULL){sal_free(global_bkt_usage[l74][l167]);
global_bkt_usage[l74][l167] = NULL;}}}int soc_th_alpm_init(int l74){int l167;
int l145 = SOC_E_NONE;uint32 l248;l145 = l94(l74);SOC_IF_ERROR_RETURN(l145);
l145 = l107(l74);l291(l74);alpm_vrf_handle[l74] = sal_alloc((MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t),"ALPM VRF Handles");if(alpm_vrf_handle[l74] == NULL
){l145 = SOC_E_MEMORY;goto l197;}tcam_pivot[l74] = sal_alloc(MAX_PIVOT_COUNT*
sizeof(alpm_pivot_t*),"ALPM pivots");if(tcam_pivot[l74] == NULL){l145 = 
SOC_E_MEMORY;goto l197;}sal_memset(alpm_vrf_handle[l74],0,(MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t));sal_memset(tcam_pivot[l74],0,MAX_PIVOT_COUNT*
sizeof(alpm_pivot_t*));l248 = SOC_TH_MAX_ALPM_BUCKETS*sizeof(alpm_bkt_usg_t);
bkt_usage[l74] = sal_alloc(l248,"ALPM_BS");if(bkt_usage[l74] == NULL){l145 = 
SOC_E_MEMORY;goto l197;}sal_memset(bkt_usage[l74],0,l248);l248 = 
SOC_TH_MAX_BUCKET_ENTRIES*sizeof(alpm_bkt_bmp_t);for(l167 = 0;l167<
SOC_TH_MAX_ALPM_VIEWS;l167++){global_bkt_usage[l74][l167] = sal_alloc(l248,
"ALPM_BS");if(global_bkt_usage[l74][l167] == NULL){l145 = SOC_E_MEMORY;goto
l197;}sal_memset(global_bkt_usage[l74][l167],0,l248);}for(l167 = 0;l167<
MAX_PIVOT_COUNT;l167++){ALPM_TCAM_PIVOT(l74,l167) = NULL;}if(SOC_CONTROL(l74)
->alpm_bulk_retry == NULL){SOC_CONTROL(l74)->alpm_bulk_retry = sal_sem_create
("ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l74)->alpm_lookup_retry
== NULL){SOC_CONTROL(l74)->alpm_lookup_retry = sal_sem_create(
"ALPM lookup retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l74)->alpm_insert_retry
== NULL){SOC_CONTROL(l74)->alpm_insert_retry = sal_sem_create(
"ALPM insert retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l74)->alpm_delete_retry
== NULL){SOC_CONTROL(l74)->alpm_delete_retry = sal_sem_create(
"ALPM delete retry",sal_sem_BINARY,0);}l145 = soc_th_alpm_128_lpm_init(l74);
if(SOC_FAILURE(l145)){goto l197;}return l145;l197:l291(l74);return l145;}
static int l292(int l74){int l167,l145;alpm_pivot_t*l152;for(l167 = 0;l167<
MAX_PIVOT_COUNT;l167++){l152 = ALPM_TCAM_PIVOT(l74,l167);if(l152){if(
PIVOT_BUCKET_HANDLE(l152)){if(PIVOT_BUCKET_TRIE(l152)){l145 = trie_traverse(
PIVOT_BUCKET_TRIE(l152),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l145)){trie_destroy(PIVOT_BUCKET_TRIE(l152));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"Unable to clear trie state for unit %d\n"),
l74));return l145;}}sal_free(PIVOT_BUCKET_HANDLE(l152));}sal_free(
ALPM_TCAM_PIVOT(l74,l167));ALPM_TCAM_PIVOT(l74,l167) = NULL;}}for(l167 = 0;
l167<= SOC_VRF_MAX(l74)+1;l167++){l145 = trie_traverse(VRF_PREFIX_TRIE_IPV4(
l74,l167),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(
l145)){trie_destroy(VRF_PREFIX_TRIE_IPV4(l74,l167));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l74,l167));return l145;}
l145 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l74,l167),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l145)){trie_destroy(
VRF_PREFIX_TRIE_IPV6(l74,l167));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l74,l167));return
l145;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l167)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l167));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,
l167)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l167));}sal_memset(&
alpm_vrf_handle[l74][l167],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
alpm_vrf_handle[l74][MAX_VRF_ID],0,sizeof(alpm_vrf_handle_t));
VRF_TRIE_INIT_DONE(l74,MAX_VRF_ID,0,1);VRF_TRIE_INIT_DONE(l74,MAX_VRF_ID,1,1)
;VRF_TRIE_INIT_DONE(l74,MAX_VRF_ID,2,1);if(SOC_TH_ALPM_VRF_BUCKET_BMAP(l74)!= 
NULL){sal_free(SOC_TH_ALPM_VRF_BUCKET_BMAP(l74));}if(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l74)!= NULL&&SOC_TH_ALPM_GLB_BUCKET_BMAP(l74)!= 
SOC_TH_ALPM_VRF_BUCKET_BMAP(l74)){sal_free(SOC_TH_ALPM_GLB_BUCKET_BMAP(l74));
}sal_memset(&soc_th_alpm_bucket[l74],0,sizeof(soc_alpm_bucket_t));l291(l74);
return SOC_E_NONE;}int soc_th_alpm_deinit(int l74){l95(l74);
SOC_IF_ERROR_RETURN(soc_th_alpm_128_lpm_deinit(l74));SOC_IF_ERROR_RETURN(
soc_th_alpm_128_state_clear(l74));SOC_IF_ERROR_RETURN(l292(l74));if(
SOC_CONTROL(l74)->alpm_bulk_retry){sal_sem_destroy(SOC_CONTROL(l74)->
alpm_bulk_retry);SOC_CONTROL(l74)->alpm_bulk_retry = NULL;}if(SOC_CONTROL(l74
)->alpm_lookup_retry == NULL){sal_sem_destroy(SOC_CONTROL(l74)->
alpm_lookup_retry);SOC_CONTROL(l74)->alpm_lookup_retry = NULL;}if(SOC_CONTROL
(l74)->alpm_insert_retry == NULL){sal_sem_destroy(SOC_CONTROL(l74)->
alpm_insert_retry);SOC_CONTROL(l74)->alpm_insert_retry = NULL;}if(SOC_CONTROL
(l74)->alpm_delete_retry == NULL){sal_sem_destroy(SOC_CONTROL(l74)->
alpm_delete_retry);SOC_CONTROL(l74)->alpm_delete_retry = NULL;}return
SOC_E_NONE;}static int l293(int l74,int l116,int l122){defip_entry_t*
lpm_entry,l294;int l295 = 0;int index;int l145 = SOC_E_NONE;uint32 key[2] = {
0,0};uint32 l127;alpm_bucket_handle_t*l257;alpm_pivot_t*pivot_pyld;payload_t*
l286;trie_t*l296;trie_t*l297 = NULL;if(l122 == 0){trie_init(_MAX_KEY_LEN_48_,
&VRF_PIVOT_TRIE_IPV4(l74,l116));l297 = VRF_PIVOT_TRIE_IPV4(l74,l116);}else{
trie_init(_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(l74,l116));l297 = 
VRF_PIVOT_TRIE_IPV6(l74,l116);}if(l122 == 0){trie_init(_MAX_KEY_LEN_48_,&
VRF_PREFIX_TRIE_IPV4(l74,l116));l296 = VRF_PREFIX_TRIE_IPV4(l74,l116);}else{
trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(l74,l116));l296 = 
VRF_PREFIX_TRIE_IPV6(l74,l116);}lpm_entry = sal_alloc(sizeof(defip_entry_t),
"Default LPM entry");if(lpm_entry == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_vrf_add: unable to allocate memory for "
"IPv4 LPM entry\n")));return SOC_E_MEMORY;}l118(l74,key,0,l116,l122,lpm_entry
,0,1);if(l122 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l116) = lpm_entry;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l116) = lpm_entry;}if(l116 == SOC_VRF_MAX(l74
)+1){soc_L3_DEFIPm_field32_set(l74,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(l74,lpm_entry,DEFAULT_MISS0f,1);}l145 = 
soc_th_alpm_bs_alloc(l74,&l295,l116,l122);soc_L3_DEFIPm_field32_set(l74,
lpm_entry,ALG_BKT_PTR0f,ALPM_BKT_IDX(l295));soc_L3_DEFIPm_field32_set(l74,
lpm_entry,ALG_SUB_BKT_PTR0f,ALPM_BKT_SIDX(l295));sal_memcpy(&l294,lpm_entry,
sizeof(l294));l145 = l96(l74,&l294,&index);l257 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l257 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_th_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
return SOC_E_NONE;}sal_memset(l257,0,sizeof(*l257));pivot_pyld = sal_alloc(
sizeof(alpm_pivot_t),"Payload for Pivot");if(pivot_pyld == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
sal_free(l257);return SOC_E_MEMORY;}l286 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l286 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_vrf_add: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l257);sal_free(pivot_pyld);return SOC_E_MEMORY
;}sal_memset(pivot_pyld,0,sizeof(*pivot_pyld));sal_memset(l286,0,sizeof(*l286
));l127 = 0;PIVOT_BUCKET_HANDLE(pivot_pyld) = l257;if(l122){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(pivot_pyld));}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(pivot_pyld));}PIVOT_BUCKET_INDEX(
pivot_pyld) = l295;PIVOT_BUCKET_VRF(pivot_pyld) = l116;PIVOT_BUCKET_IPV6(
pivot_pyld) = l122;PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;pivot_pyld->key[0] = 
l286->key[0] = key[0];pivot_pyld->key[1] = l286->key[1] = key[1];pivot_pyld->
len = l286->len = l127;l145 = trie_insert(l296,key,NULL,l127,&(l286->node));
if(SOC_FAILURE(l145)){sal_free(l286);sal_free(pivot_pyld);sal_free(l257);
return l145;}l145 = trie_insert(l297,key,NULL,l127,(trie_node_t*)pivot_pyld);
if(SOC_FAILURE(l145)){trie_node_t*l254 = NULL;(void)trie_delete(l296,key,l127
,&l254);sal_free(l286);sal_free(pivot_pyld);sal_free(l257);return l145;}index
= soc_th_alpm_physical_idx(l74,L3_DEFIPm,index,l122);if(l122 == 0){
ALPM_TCAM_PIVOT(l74,index) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = index;
}else{ALPM_TCAM_PIVOT(l74,index<<1) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld)
= index<<1;}VRF_PIVOT_REF_INC(l74,l116,l122);VRF_TRIE_INIT_DONE(l74,l116,l122
,1);return l145;}static int l121(int l74,int l116,int l122){defip_entry_t*
lpm_entry;int l148;int l145 = SOC_E_NONE;uint32 key[2] = {0,0},l139[
SOC_MAX_MEM_FIELD_WORDS];payload_t*l256;alpm_pivot_t*l298;trie_node_t*l254;
trie_t*l296;trie_t*l297 = NULL;soc_mem_t l111;int tcam_index,bktid,index;
uint32 l164[SOC_MAX_MEM_FIELD_WORDS];if(l122 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l116);l111 = L3_DEFIP_ALPM_IPV4m;}else{
lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l116);l111 = 
L3_DEFIP_ALPM_IPV6_64m;}l145 = _soc_th_alpm_find(l74,lpm_entry,l111,l164,&
tcam_index,&bktid,&index,TRUE);l145 = soc_th_alpm_bs_free(l74,bktid,l116,l122
);index = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,ALPM_BKT_IDX(bktid));
_soc_tomahawk_alpm_bkt_view_set(l74,index,INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){index = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,
ALPM_BKT_IDX(bktid)+1);_soc_tomahawk_alpm_bkt_view_set(l74,index,INVALIDm);}
l145 = l106(l74,lpm_entry,(void*)l139,&l148);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"soc_alpm_vrf_delete: unable to get internal"
" pivot idx for vrf %d/%d\n"),l116,l122));l148 = -1;}l148 = 
soc_th_alpm_physical_idx(l74,L3_DEFIPm,l148,l122);if(l122 == 0){l298 = 
ALPM_TCAM_PIVOT(l74,l148);}else{l298 = ALPM_TCAM_PIVOT(l74,l148<<1);}l145 = 
soc_th_alpm_lpm_delete(l74,lpm_entry);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),l116,l122));}sal_free(lpm_entry);if(
l122 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l116) = NULL;l296 = 
VRF_PREFIX_TRIE_IPV4(l74,l116);VRF_PREFIX_TRIE_IPV4(l74,l116) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l116) = NULL;l296 = VRF_PREFIX_TRIE_IPV6(l74,
l116);VRF_PREFIX_TRIE_IPV6(l74,l116) = NULL;}VRF_TRIE_INIT_DONE(l74,l116,l122
,0);l145 = trie_delete(l296,key,0,&l254);l256 = (payload_t*)l254;if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Unable to delete internal default for vrf "" %d/%d\n"),l116,l122));}sal_free
(l256);(void)trie_destroy(l296);if(l122 == 0){l297 = VRF_PIVOT_TRIE_IPV4(l74,
l116);VRF_PIVOT_TRIE_IPV4(l74,l116) = NULL;}else{l297 = VRF_PIVOT_TRIE_IPV6(
l74,l116);VRF_PIVOT_TRIE_IPV6(l74,l116) = NULL;}l254 = NULL;l145 = 
trie_delete(l297,key,0,&l254);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l74,"Unable to delete internal pivot node for vrf"" %d/%d\n"),
l116,l122));}(void)trie_destroy(l297);(void)trie_destroy(PIVOT_BUCKET_TRIE(
l298));sal_free(PIVOT_BUCKET_HANDLE(l298));sal_free(l298);return l145;}int
soc_th_alpm_insert(int l74,void*l97,uint32 l112,int l213,int l299){
defip_alpm_ipv4_entry_t l189,l190;defip_alpm_ipv6_64_entry_t l191,l192;
soc_mem_t l111;void*l187,*l279;int l162,l116;int index;int l99;int l145 = 
SOC_E_NONE;uint32 l113;int l114 = 0;l99 = soc_mem_field32_get(l74,L3_DEFIPm,
l97,MODE0f);l111 = (l99)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l187 = ((
l99)?((uint32*)&(l191)):((uint32*)&(l189)));l279 = ((l99)?((uint32*)&(l192)):
((uint32*)&(l190)));if(l213!= -1){l114 = ALPM_BKTID(ALPM_BKT_IDX(
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l74,l213)),(l213>>ALPM_ENT_INDEX_BITS));}
SOC_IF_ERROR_RETURN(l108(l74,l97,l187,l279,l111,l112,&l113,l114));
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,l97,&l162,&l116));if(
ALPM_PREFIX_IN_TCAM(l74,l162)){l145 = l96(l74,l97,&index);if(SOC_SUCCESS(l145
)){VRF_PIVOT_REF_INC(l74,MAX_VRF_ID,l99);VRF_TRIE_ROUTES_INC(l74,MAX_VRF_ID,
l99);}else if(l145 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l74,MAX_VRF_ID,l99);}
return(l145);}else if(l116 == 0){if(soc_th_alpm_mode_get(l74)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"VRF=0 cannot be added in Parallel mode\n")))
;return SOC_E_PARAM;}}if(l162!= SOC_L3_VRF_GLOBAL){if(!soc_th_alpm_mode_get(
l74)){if(VRF_TRIE_ROUTES_CNT(l74,l116,l99) == 0){if(!l113){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n"),l162));return SOC_E_PARAM;}}}}if(!
VRF_TRIE_INIT_COMPLETED(l74,l116,l99)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_insert:VRF %d is not ""initialized\n"),l116));l145 = 
l293(l74,l116,l99);if(SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_insert:VRF %d/%d trie init \n""failed\n"),l116,l99))
;return l145;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),l116,l99));}if(l299&
SOC_ALPM_LOOKUP_HIT){l145 = l163(l74,l97,l187,l279,l111,l213);}else{if(l213 == 
-1){l213 = 0;}l213 = ALPM_BKTID(ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l74
,l213)),l213>>ALPM_ENT_INDEX_BITS);l145 = l272(l74,l97,l111,l187,l279,&index,
l213,l299);}if(l145!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l74,soc_errmsg(l145
)));}return(l145);}int soc_th_alpm_lookup(int l74,void*key_data,void*l75,int*
l104,int*l300){defip_alpm_ipv4_entry_t l189;defip_alpm_ipv6_64_entry_t l191;
soc_mem_t l111;int bktid = 0;int tcam_index;void*l187;int l162,l116;int l99,
l87;int l145 = SOC_E_NONE;l99 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,
MODE0f);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l162,&l116)
);l145 = l103(l74,key_data,l75,l104,&l87,&l99);if(SOC_SUCCESS(l145)){if(!l99
&&(*l104&0x1)){l145 = soc_th_alpm_lpm_ip4entry1_to_0(l74,l75,l75,PRESERVE_HIT
);}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,l75,&l162,&l116));if(l162
== SOC_L3_VRF_OVERRIDE){return SOC_E_NONE;}}if(!VRF_TRIE_INIT_COMPLETED(l74,
l116,l99)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_lookup:VRF %d is not initialized\n"),l116));*l300 = 0;return
SOC_E_NOT_FOUND;}l111 = (l99)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l187
= ((l99)?((uint32*)&(l191)):((uint32*)&(l189)));SOC_ALPM_LPM_LOCK(l74);l145 = 
_soc_th_alpm_find(l74,key_data,l111,l187,&tcam_index,&bktid,l104,TRUE);
SOC_ALPM_LPM_UNLOCK(l74);if(SOC_FAILURE(l145)){*l300 = tcam_index;*l104 = (
ALPM_BKT_SIDX(bktid)<<ALPM_ENT_INDEX_BITS)|SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,
bktid);return l145;}l145 = l115(l74,l187,l111,l99,l162,bktid,*l104,l75);*l300
= SOC_ALPM_LOOKUP_HIT|tcam_index;*l104 = (ALPM_BKT_SIDX(bktid)<<
ALPM_ENT_INDEX_BITS)|*l104;return(l145);}static int l301(int l74,void*
key_data,void*l75,int l116,int*tcam_index,int*bucket_index,int*l88,int l302){
int l145 = SOC_E_NONE;int l167,l303,l122,l158 = 0;uint32 l100,l147;
defip_aux_scratch_entry_t l102;int l304,l305;int index;soc_mem_t l111,l179;
int l306,l307;int l308;uint32 l309[SOC_MAX_MEM_FIELD_WORDS] = {0};int l310 = 
-1;int l311 = 0;soc_field_t l312[2] = {IP_ADDR0f,IP_ADDR1f,};l179 = L3_DEFIPm
;l122 = soc_mem_field32_get(l74,l179,key_data,MODE0f);l304 = 
soc_mem_field32_get(l74,l179,key_data,GLOBAL_ROUTE0f);l305 = 
soc_mem_field32_get(l74,l179,key_data,VRF_ID_0f);LOG_VERBOSE(BSL_LS_SOC_ALPM,
(BSL_META_U(l74,"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d:\n"),l116 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l122,l304,l305));if(l116 == SOC_L3_VRF_GLOBAL){l100 = l302?1:0
;soc_mem_field32_set(l74,l179,key_data,GLOBAL_ROUTE0f,1);soc_mem_field32_set(
l74,l179,key_data,VRF_ID_0f,0);}else{l100 = l302?3:2;}
soc_th_alpm_bank_db_type_get(l74,l116 == SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l74)+
1):l116,&l147,NULL);sal_memset(&l102,0,sizeof(defip_aux_scratch_entry_t));
SOC_IF_ERROR_RETURN(l98(l74,key_data,l122,l100,0,&l102));if(l116 == 
SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l74,l179,key_data,GLOBAL_ROUTE0f,l304)
;soc_mem_field32_set(l74,l179,key_data,VRF_ID_0f,l305);}SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,PREFIX_LOOKUP,&l102,TRUE,&l158,tcam_index,
bucket_index));if(l158 == 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Could not find bucket\n")));return SOC_E_NOT_FOUND;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"Hit in memory %s, index %d, "
"bucket [%d,%d]\n"),SOC_MEM_NAME(l74,l179),soc_th_alpm_logical_idx(l74,l179,(
*tcam_index)>>1,1),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index)))
;l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l145 = l144(l74,
key_data,&l307);if(SOC_FAILURE(l145)){return l145;}switch(l122){case
L3_DEFIP_MODE_V4:l308 = ALPM_IPV4_BKT_COUNT;break;case L3_DEFIP_MODE_64:l308 = 
ALPM_IPV6_64_BKT_COUNT;break;default:l308 = ALPM_IPV4_BKT_COUNT;break;}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l308<<= 1;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"Start searching mem %s bucket [%d,%d](count %d) "
"for Length %d\n"),SOC_MEM_NAME(l74,l111),ALPM_BKT_IDX(*bucket_index),
ALPM_BKT_SIDX(*bucket_index),l308,l307));for(l167 = 0;l167<l308;l167++){
uint32 l187[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l277[2] = {0};uint32 l313[2
] = {0};uint32 l314[2] = {0};int l315;l145 = _soc_th_alpm_mem_index(l74,l111,
ALPM_BKT_IDX(*bucket_index),l167,l147,&index);if(l145 == SOC_E_FULL){continue
;}SOC_IF_ERROR_RETURN(soc_mem_read(l74,l111,MEM_BLOCK_ANY,index,(void*)l187))
;l315 = soc_mem_field32_get(l74,l111,l187,VALIDf);l306 = soc_mem_field32_get(
l74,l111,l187,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Bucket [%d,%d] index %6d: Valid %d, Length %d\n"),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),index,l315,l306));if(!l315||(l306>l307)){
continue;}SHR_BITSET_RANGE(l277,(l122?64:32)-l306,l306);(void)
soc_mem_field_get(l74,l111,(uint32*)l187,KEYf,(uint32*)l313);l314[1] = 
soc_mem_field32_get(l74,l179,key_data,l312[1]);l314[0] = soc_mem_field32_get(
l74,l179,key_data,l312[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tmask %08x %08x\n\t key %08x %08x\n""\thost %08x %08x\n"),l277[1],l277[0],
l313[1],l313[0],l314[1],l314[0]));for(l303 = l122?1:0;l303>= 0;l303--){if((
l314[l303]&l277[l303])!= (l313[l303]&l277[l303])){break;}}if(l303>= 0){
continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Found a match in mem %s bucket [%d,%d], ""index %d\n"),SOC_MEM_NAME(l74,l111
),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index),index));if(l310 == 
-1||l310<l306){l310 = l306;l311 = index;sal_memcpy(l309,l187,sizeof(l187));}}
if(l310!= -1){l145 = l115(l74,l309,l111,l122,l116,*bucket_index,l311,l75);if(
SOC_SUCCESS(l145)){*l88 = l311;if(bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l74)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Hit mem %s bucket [%d,%d], index %d\n"),SOC_MEM_NAME(l74,l111),ALPM_BKT_IDX(
*bucket_index),ALPM_BKT_SIDX(*bucket_index),l311));}}return l145;}*l88 = 
soc_th_alpm_logical_idx(l74,l179,(*tcam_index)>>1,1);LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Miss in mem %s bucket [%d,%d], use associate data "
"in mem %s LOG index %d\n"),SOC_MEM_NAME(l74,l111),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),SOC_MEM_NAME(l74,l179),*l88));
SOC_IF_ERROR_RETURN(soc_mem_read(l74,l179,MEM_BLOCK_ANY,*l88,(void*)l75));if(
(!l122)&&((*tcam_index)&1)){l145 = soc_th_alpm_lpm_ip4entry1_to_0(l74,l75,l75
,PRESERVE_HIT);}return SOC_E_NONE;}int soc_th_alpm_find_best_match(int l74,
void*key_data,void*l75,int*l104,int l302){int l145 = SOC_E_NONE;int l167,l316
,l171;defip_entry_t l317;uint32 l318[2];uint32 l313[2];uint32 l319[2];uint32
l314[2];uint32 l320,l321;int l162,l116 = 0;int l322[2] = {0};int tcam_index,
bucket_index;soc_mem_t l179 = L3_DEFIPm;int l218,l122,l323,l324 = 0;
soc_field_t l325[] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l326[] = {
GLOBAL_ROUTE0f,GLOBAL_ROUTE1f};l122 = soc_mem_field32_get(l74,l179,key_data,
MODE0f);if(!SOC_URPF_STATUS_GET(l74)&&l302){return SOC_E_PARAM;}l316 = 
soc_mem_index_min(l74,l179);l171 = soc_mem_index_count(l74,l179);if(
SOC_URPF_STATUS_GET(l74)){l171>>= 1;}if(soc_th_alpm_mode_get(l74)){l171>>= 1;
l316+= l171;}if(l302){l316+= soc_mem_index_count(l74,l179)/2;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Launch LPM searching from index %d count %d\n"),l316,l171));for(l167 = l316;
l167<l316+l171;l167++){SOC_IF_ERROR_RETURN(soc_mem_read(l74,l179,
MEM_BLOCK_ANY,l167,(void*)&l317));l322[0] = soc_mem_field32_get(l74,l179,&
l317,VALID0f);l322[1] = soc_mem_field32_get(l74,l179,&l317,VALID1f);if(l322[0
] == 0&&l322[1] == 0){continue;}l323 = soc_mem_field32_get(l74,l179,&l317,
MODE0f);if(l323!= l122){continue;}for(l218 = 0;l218<(l122?1:2);l218++){if(
l322[l218] == 0){continue;}l320 = soc_mem_field32_get(l74,l179,&l317,l325[
l218]);l321 = soc_mem_field32_get(l74,l179,&l317,l326[l218]);if(!(
soc_th_alpm_mode_get(l74)!= 2&&l320&&l321)&&!(soc_th_alpm_mode_get(l74) == 2
&&l321)){continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Match a Global High route: ent %d\n"),l218));l318[0] = soc_mem_field32_get(
l74,l179,&l317,IP_ADDR_MASK0f);l318[1] = soc_mem_field32_get(l74,l179,&l317,
IP_ADDR_MASK1f);l313[0] = soc_mem_field32_get(l74,l179,&l317,IP_ADDR0f);l313[
1] = soc_mem_field32_get(l74,l179,&l317,IP_ADDR1f);l319[0] = 
soc_mem_field32_get(l74,l179,key_data,IP_ADDR_MASK0f);l319[1] = 
soc_mem_field32_get(l74,l179,key_data,IP_ADDR_MASK1f);l314[0] = 
soc_mem_field32_get(l74,l179,key_data,IP_ADDR0f);l314[1] = 
soc_mem_field32_get(l74,l179,key_data,IP_ADDR1f);LOG_VERBOSE(BSL_LS_SOC_ALPM,
(BSL_META_U(l74,"\thmsk %08x %08x\n\thkey %08x %08x\n"
"\tsmsk %08x %08x\n\tskey %08x %08x\n"),l318[1],l318[0],l313[1],l313[0],l319[
1],l319[0],l314[1],l314[0]));if(l122&&(((l318[1]&l319[1])!= l318[1])||((l318[
0]&l319[0])!= l318[0]))){continue;}if(!l122&&((l318[l218]&l319[0])!= l318[
l218])){continue;}if(l122&&((l314[0]&l318[0]) == (l313[0]&l318[0]))&&((l314[1
]&l318[1]) == (l313[1]&l318[1]))){l324 = TRUE;break;}if(!l122&&((l314[0]&l318
[l218]) == (l313[l218]&l318[l218]))){l324 = TRUE;break;}}if(!l324){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Hit Global High route in index = %d(%d)\n"),l167,l218));sal_memcpy(l75,&l317
,sizeof(l317));if(!l122&&l218 == 1){l145 = soc_th_alpm_lpm_ip4entry1_to_0(l74
,l75,l75,PRESERVE_HIT);}*l104 = l167;return l145;}LOG_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l74,"Global high lookup miss, use AUX engine to "
"search for VRF and Global Low routes\n")));SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l74,key_data,&l162,&l116));l145 = l301(l74,key_data,
l75,l116,&tcam_index,&bucket_index,l104,l302);if(l145 == SOC_E_NOT_FOUND){
l116 = SOC_L3_VRF_GLOBAL;LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Not found in VRF region, trying Global ""region\n")));l145 = l301(l74,
key_data,l75,l116,&tcam_index,&bucket_index,l104,l302);}if(SOC_SUCCESS(l145))
{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Hit in %s region in TCAM index %d, ""bucket [%d,%d]\n"),l116 == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",tcam_index,ALPM_BKT_IDX(bucket_index),
ALPM_BKT_SIDX(bucket_index)));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"Search miss for given address\n")));}return(l145);}int soc_th_alpm_delete
(int l74,void*key_data,int l213,int l299){int l327;int l162,l116;int l99;int
l145 = SOC_E_NONE;l99 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l162,&l116));if(
ALPM_PREFIX_IN_TCAM(l74,l162)){l145 = soc_th_alpm_lpm_delete(l74,key_data);if
(SOC_SUCCESS(l145)){VRF_PIVOT_REF_DEC(l74,MAX_VRF_ID,l99);VRF_TRIE_ROUTES_DEC
(l74,MAX_VRF_ID,l99);}return(l145);}else{if(!VRF_TRIE_INIT_COMPLETED(l74,l116
,l99)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_delete:VRF %d/%d is not initialized\n"),l116,l99));return SOC_E_NONE
;}if(l213 == -1){l213 = 0;}l327 = l213;l213 = ALPM_BKTID(ALPM_BKT_IDX(
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l74,l327)),l327>>ALPM_ENT_INDEX_BITS);l145 = 
l280(l74,key_data,l213,l299&~SOC_ALPM_LOOKUP_HIT,l327);}return(l145);}static
int l107(int l74){int l328;l328 = soc_mem_index_count(l74,L3_DEFIPm)+
soc_mem_index_count(l74,L3_DEFIP_PAIR_128m)*2;SOC_TH_ALPM_BUCKET_COUNT(l74) = 
SOC_TH_ALPM_MAX_BKTS;if(SOC_URPF_STATUS_GET(l74)){l328>>= 1;
SOC_TH_ALPM_BUCKET_COUNT(l74)>>= 1;}SOC_TH_ALPM_BUCKET_BMAP_SIZE(l74) = 
SHR_BITALLOCSIZE(SOC_TH_ALPM_BUCKET_COUNT(l74));SOC_TH_ALPM_VRF_BUCKET_BMAP(
l74) = sal_alloc(SOC_TH_ALPM_BUCKET_BMAP_SIZE(l74),
"alpm_vrf_shared_bucket_bitmap");if(SOC_TH_ALPM_VRF_BUCKET_BMAP(l74) == NULL)
{return SOC_E_MEMORY;}sal_memset(SOC_TH_ALPM_VRF_BUCKET_BMAP(l74),0,
SOC_TH_ALPM_BUCKET_BMAP_SIZE(l74));soc_th_alpm_bucket_assign(l74,&l328,1,1);
if(soc_property_get(l74,"l3_alpm_vrf_share_bucket",0)&&soc_th_alpm_mode_get(
l74) == 1&&SOC_URPF_STATUS_GET(l74)){SOC_TH_ALPM_GLB_BUCKET_BMAP(l74) = 
sal_alloc(SOC_TH_ALPM_BUCKET_BMAP_SIZE(l74),"alpm_glb_shared_bucket_bitmap");
if(SOC_TH_ALPM_GLB_BUCKET_BMAP(l74) == NULL){sal_free(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l74));return SOC_E_MEMORY;}sal_memset(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l74),0,SOC_TH_ALPM_BUCKET_BMAP_SIZE(l74));
soc_th_alpm_bucket_assign(l74,&l328,SOC_VRF_MAX(l74)+1,1);}else{
SOC_TH_ALPM_GLB_BUCKET_BMAP(l74) = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74);}return
SOC_E_NONE;}int soc_th_alpm_bucket_assign(int l74,int*l295,int l116,int l122)
{int l167,l220 = 1,l221 = 0;SHR_BITDCL*l329 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74
);if(l116 == SOC_VRF_MAX(l74)+1){l329 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l74);}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l220 = 2;}for(l167 = 0;l167<
SOC_TH_ALPM_BUCKET_COUNT(l74);l167+= l220){SHR_BITTEST_RANGE(l329,l167,l220,
l221);if(0 == l221){break;}}if(l167 == SOC_TH_ALPM_BUCKET_COUNT(l74)){return
SOC_E_FULL;}SHR_BITSET_RANGE(l329,l167,l220);*l295 = l167;
SOC_TH_ALPM_BUCKET_NEXT_FREE(l74) = l167;return SOC_E_NONE;}int
soc_th_alpm_bucket_release(int l74,int l295,int l116,int l122){int l220 = 1,
l221 = 0;SHR_BITDCL*l329 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74);if((l295<1)||(
l295>SOC_TH_ALPM_BUCKET_MAX_INDEX(l74))){return SOC_E_PARAM;}if(l116 == 
SOC_VRF_MAX(l74)+1){l329 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l74);}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l220 = 2;}SHR_BITTEST_RANGE(l329,l295,l220
,l221);if(!l221){return SOC_E_PARAM;}SHR_BITCLR_RANGE(l329,l295,l220);return
SOC_E_NONE;}int soc_th_alpm_bucket_is_assigned(int l74,int l330,int l116,int
l99,int*l221){int l220 = 1;SHR_BITDCL*l329 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74)
;if(l116 == SOC_VRF_MAX(l74)+1){l329 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l74);}if((
l330<1)||(l330>SOC_TH_ALPM_BUCKET_MAX_INDEX(l74))){return SOC_E_PARAM;}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l99)){l220 = 2;}SHR_BITTEST_RANGE(l329,l330,l220,
*l221);return SOC_E_NONE;}static void l73(int l74,void*l75,int index,l68 l76)
{if(index&(0x8000)){l76[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(l75),(l61[(l74)]->l25));l76[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l27));l76[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l75),(l61[(l74)]->l26));l76[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l28));if((!(SOC_IS_HURRICANE(l74)))&&(((l61[(l74)]->l41)!= NULL))){int
l331;(void)soc_th_alpm_lpm_vrf_get(l74,l75,(int*)&l76[4],&l331);}else{l76[4] = 
0;};}else{if(index&0x1){l76[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(l74)]->l26));l76[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l28));l76[2] = 0;l76[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l74)))&&((
(l61[(l74)]->l42)!= NULL))){int l331;defip_entry_t l332;(void)
soc_th_alpm_lpm_ip4entry1_to_0(l74,l75,&l332,0);(void)soc_th_alpm_lpm_vrf_get
(l74,&l332,(int*)&l76[4],&l331);}else{l76[4] = 0;};}else{l76[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l25));l76[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l75),(l61[(l74)]->l27));l76[2] = 0;l76[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l74)))&&(((l61[(l74)]->l41)!= NULL))){int l331;(void)
soc_th_alpm_lpm_vrf_get(l74,l75,(int*)&l76[4],&l331);}else{l76[4] = 0;};}}}
static int l333(l68 l70,l68 l71){int l148;for(l148 = 0;l148<5;l148++){if(l70[
l148]<l71[l148]){return-1;}if(l70[l148]>l71[l148]){return 1;}}return(0);}
static void l334(int l74,void*l97,uint32 l335,uint32 l90,int l87){l68 l336;if
(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l97),(l61[
(l74)]->l29))){if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l97),(l61[(l74)]->l40))&&soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l97),(l61[(l74)]->l39))){l336[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l97),(l61[(
l74)]->l25));l336[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l97),(l61[(l74)]->l27));l336[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l97),(l61[(
l74)]->l26));l336[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l97),(l61[(l74)]->l28));if((!(SOC_IS_HURRICANE(l74)))&&(((l61[(
l74)]->l41)!= NULL))){int l331;(void)soc_th_alpm_lpm_vrf_get(l74,l97,(int*)&
l336[4],&l331);}else{l336[4] = 0;};l89((l72[(l74)]),l333,l336,l87,l90,((
uint16)l335<<1)|(0x8000));}}else{if(soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l97),(l61[(l74)]->l39))){l336[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l97),(l61[(
l74)]->l25));l336[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l97),(l61[(l74)]->l27));l336[2] = 0;l336[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l74)))&&(((l61[(l74)]->l41)!= NULL))){int l331;(void)
soc_th_alpm_lpm_vrf_get(l74,l97,(int*)&l336[4],&l331);}else{l336[4] = 0;};l89
((l72[(l74)]),l333,l336,l87,l90,((uint16)l335<<1));}if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l97),(l61[(
l74)]->l40))){l336[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l97),(l61[(l74)]->l26));l336[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l97),(l61[(
l74)]->l28));l336[2] = 0;l336[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l74)))&&
(((l61[(l74)]->l42)!= NULL))){int l331;defip_entry_t l332;(void)
soc_th_alpm_lpm_ip4entry1_to_0(l74,l97,&l332,0);(void)soc_th_alpm_lpm_vrf_get
(l74,&l332,(int*)&l336[4],&l331);}else{l336[4] = 0;};l89((l72[(l74)]),l333,
l336,l87,l90,(((uint16)l335<<1)+1));}}}static void l337(int l74,void*key_data
,uint32 l335){l68 l336;int l87 = -1;int l145;uint16 index;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(
l61[(l74)]->l29))){l336[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(key_data),(l61[(l74)]->l25));l336[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(
l61[(l74)]->l27));l336[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(key_data),(l61[(l74)]->l26));l336[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(
l61[(l74)]->l28));if((!(SOC_IS_HURRICANE(l74)))&&(((l61[(l74)]->l41)!= NULL))
){int l331;(void)soc_th_alpm_lpm_vrf_get(l74,key_data,(int*)&l336[4],&l331);}
else{l336[4] = 0;};index = (l335<<1)|(0x8000);}else{l336[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(
l61[(l74)]->l25));l336[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(key_data),(l61[(l74)]->l27));l336[2] = 0;l336[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l74)))&&(((l61[(l74)]->l41)!= NULL))){int
l331;(void)soc_th_alpm_lpm_vrf_get(l74,key_data,(int*)&l336[4],&l331);}else{
l336[4] = 0;};index = l335;}l145 = l92((l72[(l74)]),l333,l336,l87,index);if(
SOC_FAILURE(l145)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\ndel  index: H %d error %d\n"),index,l145));}}static int l338(int l74,void*
key_data,int l87,int*l88){l68 l336;int l339;int l145;uint16 index = (0xFFFF);
l339 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(
key_data),(l61[(l74)]->l29));if(l339){l336[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(
l61[(l74)]->l25));l336[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(key_data),(l61[(l74)]->l27));l336[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(
l61[(l74)]->l26));l336[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(key_data),(l61[(l74)]->l28));if((!(SOC_IS_HURRICANE(l74)))&&
(((l61[(l74)]->l41)!= NULL))){int l331;(void)soc_th_alpm_lpm_vrf_get(l74,
key_data,(int*)&l336[4],&l331);}else{l336[4] = 0;};}else{l336[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(
l61[(l74)]->l25));l336[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(key_data),(l61[(l74)]->l27));l336[2] = 0;l336[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l74)))&&(((l61[(l74)]->l41)!= NULL))){int
l331;(void)soc_th_alpm_lpm_vrf_get(l74,key_data,(int*)&l336[4],&l331);}else{
l336[4] = 0;};}l145 = l84((l72[(l74)]),l333,l336,l87,&index);if(SOC_FAILURE(
l145)){*l88 = 0xFFFFFFFF;return(l145);}*l88 = index;return(SOC_E_NONE);}
static uint16 l77(uint8*l78,int l79){return(_shr_crc16b(0,l78,l79));}static
int l80(int unit,int l63,int l64,l67**l81){l67*l85;int index;if(l64>l63){
return SOC_E_MEMORY;}l85 = sal_alloc(sizeof(l67),"lpm_hash");if(l85 == NULL){
return SOC_E_MEMORY;}sal_memset(l85,0,sizeof(*l85));l85->unit = unit;l85->l63
= l63;l85->l64 = l64;l85->l65 = sal_alloc(l85->l64*sizeof(*(l85->l65)),
"hash_table");if(l85->l65 == NULL){sal_free(l85);return SOC_E_MEMORY;}l85->
l66 = sal_alloc(l85->l63*sizeof(*(l85->l66)),"link_table");if(l85->l66 == 
NULL){sal_free(l85->l65);sal_free(l85);return SOC_E_MEMORY;}for(index = 0;
index<l85->l64;index++){l85->l65[index] = (0xFFFF);}for(index = 0;index<l85->
l63;index++){l85->l66[index] = (0xFFFF);}*l81 = l85;return SOC_E_NONE;}static
int l82(l67*l83){if(l83!= NULL){sal_free(l83->l65);sal_free(l83->l66);
sal_free(l83);}return SOC_E_NONE;}static int l84(l67*l85,l69 l86,l68 entry,
int l87,uint16*l88){int l74 = l85->unit;uint16 l340;uint16 index;l340 = l77((
uint8*)entry,(32*5))%l85->l64;index = l85->l65[l340];;;while(index!= (0xFFFF)
){uint32 l75[SOC_MAX_MEM_FIELD_WORDS];l68 l76;int l341;l341 = (index&(0x7FFF)
)>>1;SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l74,MEM_BLOCK_ANY,l341,l75));l73(l74,
l75,index,l76);if((*l86)(entry,l76) == 0){*l88 = (index&(0x7FFF))>>((index&(
0x8000))?1:0);;return(SOC_E_NONE);}index = l85->l66[index&(0x7FFF)];;};return
(SOC_E_NOT_FOUND);}static int l89(l67*l85,l69 l86,l68 entry,int l87,uint16 l90
,uint16 l91){int l74 = l85->unit;uint16 l340;uint16 index;uint16 l342;l340 = 
l77((uint8*)entry,(32*5))%l85->l64;index = l85->l65[l340];;;;l342 = (0xFFFF);
if(l90!= (0xFFFF)){while(index!= (0xFFFF)){uint32 l75[SOC_MAX_MEM_FIELD_WORDS
];l68 l76;int l341;l341 = (index&(0x7FFF))>>1;SOC_IF_ERROR_RETURN(
READ_L3_DEFIPm(l74,MEM_BLOCK_ANY,l341,l75));l73(l74,l75,index,l76);if((*l86)(
entry,l76) == 0){if(l91!= index){;if(l342 == (0xFFFF)){l85->l65[l340] = l91;
l85->l66[l91&(0x7FFF)] = l85->l66[index&(0x7FFF)];l85->l66[index&(0x7FFF)] = 
(0xFFFF);}else{l85->l66[l342&(0x7FFF)] = l91;l85->l66[l91&(0x7FFF)] = l85->
l66[index&(0x7FFF)];l85->l66[index&(0x7FFF)] = (0xFFFF);}};return(SOC_E_NONE)
;}l342 = index;index = l85->l66[index&(0x7FFF)];;}}l85->l66[l91&(0x7FFF)] = 
l85->l65[l340];l85->l65[l340] = l91;return(SOC_E_NONE);}static int l92(l67*
l85,l69 l86,l68 entry,int l87,uint16 l93){uint16 l340;uint16 index;uint16 l342
;l340 = l77((uint8*)entry,(32*5))%l85->l64;index = l85->l65[l340];;;l342 = (
0xFFFF);while(index!= (0xFFFF)){if(l93 == index){;if(l342 == (0xFFFF)){l85->
l65[l340] = l85->l66[l93&(0x7FFF)];l85->l66[l93&(0x7FFF)] = (0xFFFF);}else{
l85->l66[l342&(0x7FFF)] = l85->l66[l93&(0x7FFF)];l85->l66[l93&(0x7FFF)] = (
0xFFFF);}return(SOC_E_NONE);}l342 = index;index = l85->l66[index&(0x7FFF)];;}
return(SOC_E_NOT_FOUND);}int soc_th_alpm_lpm_ip4entry0_to_0(int l74,void*l343
,void*l344,int l345){uint32 l146;l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l39));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l39),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l29));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l29),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l25));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l25),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l27));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l27),(l146));if(((l61[(l74)]->l15)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l15));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l15),(l146));}if(((l61[(l74)]->l17)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l17));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l17),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l19));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l19),(l146));}else{l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l33));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l33),(l146));}l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l35));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l35),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l37));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l37),(l146));if(((l61[(l74)]->l41)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l41));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l41),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l43));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l43),(l146));}if(((l61[(l74)]->l13)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l13));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l13),(l146));}if(l345){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l23));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l23),(l146));}l146 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l45));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l45),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l47));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l47),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l49),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l51));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l51),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l53));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l53),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l55));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l55),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l57));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l57),(l146));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry1_to_1(
int l74,void*l343,void*l344,int l345){uint32 l146;l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l40));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l40),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l30));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l30),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l26));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l26),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l28));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l28),(l146));if(((l61[(l74)]->l16)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l16));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l16),(l146));}if(((l61[(l74)]->l18)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l18));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l18),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l20));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l20),(l146));}else{l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l34));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l34),(l146));}l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l36));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l36),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l38));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l38),(l146));if(((l61[(l74)]->l42)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l42));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l42),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l44));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l44),(l146));}if(((l61[(l74)]->l14)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l14));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l14),(l146));}if(l345){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l24));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l24),(l146));}l146 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l46));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l46),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l48));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l48),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l50),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l52));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l52),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l54));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l54),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l56),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l58));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l58),(l146));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry0_to_1(
int l74,void*l343,void*l344,int l345){uint32 l146;l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l39));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l40),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l29));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l30),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l25));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l26),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l27));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l28),(l146));if(!SOC_IS_HURRICANE(l74)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l15));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l16),(l146));}if(((l61[(l74)]->l17)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l17));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l18),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l19));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l20),(l146));}else{l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l33));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l34),(l146));}l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l35));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l36),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l37));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l38),(l146));if(((l61[(l74)]->l41)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l41));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l42),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l43));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l44),(l146));}if(((l61[(l74)]->l13)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l13));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l14),(l146));}if(l345){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l23));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l24),(l146));}l146 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l45));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l46),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l47));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l48),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l50),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l51));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l52),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l53));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l54),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l55));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l56),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l57));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l58),(l146));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry1_to_0(
int l74,void*l343,void*l344,int l345){uint32 l146;l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l40));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l39),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l30));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l29),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l26));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l25),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l28));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l27),(l146));if(!SOC_IS_HURRICANE(l74)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l16));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l15),(l146));}if(((l61[(l74)]->l18)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l18));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l17),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l20));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l19),(l146));}else{l146 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l34));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l33),(l146));}l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l36));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l35),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l38));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l37),(l146));if(((l61[(l74)]->l42)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l42));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l41),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l44));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l43),(l146));}if(((l61[(l74)]->l14)!= NULL)){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l14));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l13),(l146));}if(l345){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l24));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l23),(l146));}l146 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l46));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l45),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l48));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l47),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l49),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l52));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l51),(l146));l146 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l54));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l53),(l146));l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l55),(l146));l146 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l58));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l57),(l146));return(SOC_E_NONE);}static int l346(int l74,void*l75){
return(SOC_E_NONE);}void soc_th_alpm_lpm_state_dump(int l74){int l167;int l347
;l347 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l74)){return;}for(l167 = l347;l167>= 0;l167--){if((l167!= 
((3*(64+32+2+1))-1))&&((l9[(l74)][(l167)].l2) == -1)){continue;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l167,(l9[
(l74)][(l167)].l4),(l9[(l74)][(l167)].next),(l9[(l74)][(l167)].l2),(l9[(l74)]
[(l167)].l3),(l9[(l74)][(l167)].l5),(l9[(l74)][(l167)].l6)));}
COMPILER_REFERENCE(l346);}static int l348(int l74,int index,uint32*l75){int
l349;int l99;uint32 l350;uint32 l351;int l352;if(!SOC_URPF_STATUS_GET(l74)){
return(SOC_E_NONE);}if(soc_feature(l74,soc_feature_l3_defip_hole)){l349 = (
soc_mem_index_count(l74,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l74)){l349 = (
soc_mem_index_count(l74,L3_DEFIPm)>>1)+0x0400;}else{l349 = (
soc_mem_index_count(l74,L3_DEFIPm)>>1);}l99 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l29));if(((l61[(l74)]->l13)!= NULL)){soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(l74)]->l13),(0));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l14),(0));}l350 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74
,L3_DEFIPm)),(l75),(l61[(l74)]->l27));l351 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l28));if(!l99){if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74
,L3_DEFIPm)),(l75),(l61[(l74)]->l39))){soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(l74)]->l37),((!l350)?1:0));}if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l40))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l75),(l61[(l74)]->l38),((!l351)?1:0));}}else{l352 = ((!l350)&&(!l351))?1:0
;l350 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75
),(l61[(l74)]->l39));l351 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l75),(l61[(l74)]->l40));if(l350&&l351){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l37),(l352));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l75),(l61[(l74)]->l38),(l352));}}return l233(l74,MEM_BLOCK_ANY,
index+l349,index,l75);}static int l353(int l74,int l354,int l355){uint32 l75[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l74,MEM_BLOCK_ANY
,l354,l75));l334(l74,l75,l355,0x4000,0);SOC_IF_ERROR_RETURN(l233(l74,
MEM_BLOCK_ANY,l355,l354,l75));SOC_IF_ERROR_RETURN(l348(l74,l355,l75));do{int
l356 = soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l354),1);int l357 = 
soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l355),1);ALPM_TCAM_PIVOT(l74,l357<<
1) = ALPM_TCAM_PIVOT(l74,l356<<1);ALPM_TCAM_PIVOT(l74,(l357<<1)+1) = 
ALPM_TCAM_PIVOT(l74,(l356<<1)+1);if(ALPM_TCAM_PIVOT((l74),l357<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l74),l357<<1)) = l357<<1;
soc_th_alpm_lpm_move_bu_upd(l74,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l74),(
l357<<1))),(l357<<1));}if(ALPM_TCAM_PIVOT((l74),(l357<<1)+1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l74),(l357<<1)+1)) = (l357<<1)+1;
soc_th_alpm_lpm_move_bu_upd(l74,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l74),(
l357<<1)+1)),(l357<<1)+1);}ALPM_TCAM_PIVOT(l74,l356<<1) = NULL;
ALPM_TCAM_PIVOT(l74,(l356<<1)+1) = NULL;}while(0);return(SOC_E_NONE);}static
int l358(int l74,int l87,int l99){uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int l354
;int l355;uint32 l359,l360;l355 = (l9[(l74)][(l87)].l3)+1;if(!l99){l354 = (l9
[(l74)][(l87)].l3);SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l74,MEM_BLOCK_ANY,l354,
l75));l359 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm))
,(l75),(l61[(l74)]->l39));l360 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(l74)]->l40));if((l359 == 0)||(l360 == 
0)){l334(l74,l75,l355,0x4000,0);SOC_IF_ERROR_RETURN(l233(l74,MEM_BLOCK_ANY,
l355,l354,l75));SOC_IF_ERROR_RETURN(l348(l74,l355,l75));do{int l356 = 
soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l354),1)*2;int l357 = 
soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l355),1)*2;if((l360)){l356++;}
ALPM_TCAM_PIVOT((l74),l357) = ALPM_TCAM_PIVOT((l74),l356);if(ALPM_TCAM_PIVOT(
(l74),l357)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l74),l357)) = l357;
soc_th_alpm_lpm_move_bu_upd(l74,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l74),l357
)),l357);}ALPM_TCAM_PIVOT((l74),l356) = NULL;}while(0);l355--;}}l354 = (l9[(
l74)][(l87)].l2);if(l354!= l355){SOC_IF_ERROR_RETURN(l353(l74,l354,l355));
VRF_PIVOT_SHIFT_INC(l74,MAX_VRF_ID,l99);}(l9[(l74)][(l87)].l2)+= 1;(l9[(l74)]
[(l87)].l3)+= 1;return(SOC_E_NONE);}static int l361(int l74,int l87,int l99){
uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int l354;int l355;int l362;uint32 l359,
l360;l355 = (l9[(l74)][(l87)].l2)-1;if((l9[(l74)][(l87)].l5) == 0){(l9[(l74)]
[(l87)].l2) = l355;(l9[(l74)][(l87)].l3) = l355-1;return(SOC_E_NONE);}if((!
l99)&&((l9[(l74)][(l87)].l3)!= (l9[(l74)][(l87)].l2))){l354 = (l9[(l74)][(l87
)].l3);SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l74,MEM_BLOCK_ANY,l354,l75));l359 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(
l74)]->l39));l360 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l75),(l61[(l74)]->l40));if((l359 == 0)||(l360 == 0)){l362 = l354
-1;SOC_IF_ERROR_RETURN(l353(l74,l362,l355));VRF_PIVOT_SHIFT_INC(l74,
MAX_VRF_ID,l99);l334(l74,l75,l362,0x4000,0);SOC_IF_ERROR_RETURN(l233(l74,
MEM_BLOCK_ANY,l362,l354,l75));SOC_IF_ERROR_RETURN(l348(l74,l362,l75));do{int
l356 = soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l354),1)*2;int l357 = 
soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l362),1)*2;if((l360)){l356++;}
ALPM_TCAM_PIVOT((l74),l357) = ALPM_TCAM_PIVOT((l74),l356);if(ALPM_TCAM_PIVOT(
(l74),l357)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l74),l357)) = l357;
soc_th_alpm_lpm_move_bu_upd(l74,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l74),l357
)),l357);}ALPM_TCAM_PIVOT((l74),l356) = NULL;}while(0);}else{l334(l74,l75,
l355,0x4000,0);SOC_IF_ERROR_RETURN(l233(l74,MEM_BLOCK_ANY,l355,l354,l75));
SOC_IF_ERROR_RETURN(l348(l74,l355,l75));do{int l356 = 
soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l354),1);int l357 = 
soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l355),1);ALPM_TCAM_PIVOT(l74,l357<<
1) = ALPM_TCAM_PIVOT(l74,l356<<1);ALPM_TCAM_PIVOT(l74,(l357<<1)+1) = 
ALPM_TCAM_PIVOT(l74,(l356<<1)+1);if(ALPM_TCAM_PIVOT((l74),l357<<1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l74),l357<<1)) = l357<<1;
soc_th_alpm_lpm_move_bu_upd(l74,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l74),(
l357<<1))),(l357<<1));}if(ALPM_TCAM_PIVOT((l74),(l357<<1)+1)){
PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l74),(l357<<1)+1)) = (l357<<1)+1;
soc_th_alpm_lpm_move_bu_upd(l74,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l74),(
l357<<1)+1)),(l357<<1)+1);}ALPM_TCAM_PIVOT(l74,l356<<1) = NULL;
ALPM_TCAM_PIVOT(l74,(l356<<1)+1) = NULL;}while(0);}}else{l354 = (l9[(l74)][(
l87)].l3);SOC_IF_ERROR_RETURN(l353(l74,l354,l355));VRF_PIVOT_SHIFT_INC(l74,
MAX_VRF_ID,l99);}(l9[(l74)][(l87)].l2)-= 1;(l9[(l74)][(l87)].l3)-= 1;return(
SOC_E_NONE);}static int l363(int l74,int l87,int l99,void*l75,int*l364){int
l365;int l366;int l367;int l368;int l354;uint32 l359,l360;int l145;if((l9[(
l74)][(l87)].l5) == 0){l368 = ((3*(64+32+2+1))-1);if((soc_th_alpm_mode_get(
l74) == SOC_ALPM_MODE_PARALLEL)||(soc_th_alpm_mode_get(l74) == 
SOC_ALPM_MODE_TCAM_ALPM)){if(l87<= (((3*(64+32+2+1))/3)-1)){l368 = (((3*(64+
32+2+1))/3)-1);}}while((l9[(l74)][(l368)].next)>l87){l368 = (l9[(l74)][(l368)
].next);}l366 = (l9[(l74)][(l368)].next);if(l366!= -1){(l9[(l74)][(l366)].l4)
= l87;}(l9[(l74)][(l87)].next) = (l9[(l74)][(l368)].next);(l9[(l74)][(l87)].
l4) = l368;(l9[(l74)][(l368)].next) = l87;(l9[(l74)][(l87)].l6) = ((l9[(l74)]
[(l368)].l6)+1)/2;(l9[(l74)][(l368)].l6)-= (l9[(l74)][(l87)].l6);(l9[(l74)][(
l87)].l2) = (l9[(l74)][(l368)].l3)+(l9[(l74)][(l368)].l6)+1;(l9[(l74)][(l87)]
.l3) = (l9[(l74)][(l87)].l2)-1;(l9[(l74)][(l87)].l5) = 0;}else if(!l99){l354 = 
(l9[(l74)][(l87)].l2);if((l145 = READ_L3_DEFIPm(l74,MEM_BLOCK_ANY,l354,l75))<
0){return l145;}l359 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l75),(l61[(l74)]->l39));l360 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(l74)]->l40));if((l359 == 0)||(
l360 == 0)){*l364 = (l354<<1)+((l360 == 0)?1:0);return(SOC_E_NONE);}l354 = (
l9[(l74)][(l87)].l3);if((l145 = READ_L3_DEFIPm(l74,MEM_BLOCK_ANY,l354,l75))<0
){return l145;}l359 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l75),(l61[(l74)]->l39));l360 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(l74)]->l40));if((l359 == 0)||(
l360 == 0)){*l364 = (l354<<1)+((l360 == 0)?1:0);return(SOC_E_NONE);}}l367 = 
l87;while((l9[(l74)][(l367)].l6) == 0){l367 = (l9[(l74)][(l367)].next);if(
l367 == -1){l367 = l87;break;}}while((l9[(l74)][(l367)].l6) == 0){l367 = (l9[
(l74)][(l367)].l4);if(l367 == -1){if((l9[(l74)][(l87)].l5) == 0){l365 = (l9[(
l74)][(l87)].l4);l366 = (l9[(l74)][(l87)].next);if(-1!= l365){(l9[(l74)][(
l365)].next) = l366;}if(-1!= l366){(l9[(l74)][(l366)].l4) = l365;}}return(
SOC_E_FULL);}}while(l367>l87){l366 = (l9[(l74)][(l367)].next);
SOC_IF_ERROR_RETURN(l361(l74,l366,l99));(l9[(l74)][(l367)].l6)-= 1;(l9[(l74)]
[(l366)].l6)+= 1;l367 = l366;}while(l367<l87){SOC_IF_ERROR_RETURN(l358(l74,
l367,l99));(l9[(l74)][(l367)].l6)-= 1;l365 = (l9[(l74)][(l367)].l4);(l9[(l74)
][(l365)].l6)+= 1;l367 = l365;}(l9[(l74)][(l87)].l5)+= 1;(l9[(l74)][(l87)].l6
)-= 1;(l9[(l74)][(l87)].l3)+= 1;*l364 = (l9[(l74)][(l87)].l3)<<((l99)?0:1);
sal_memcpy(l75,soc_mem_entry_null(l74,L3_DEFIPm),soc_mem_entry_words(l74,
L3_DEFIPm)*4);return(SOC_E_NONE);}static int l369(int l74,int l87,int l99,
void*l75,int l370){int l365;int l366;int l354;int l355;uint32 l371[
SOC_MAX_MEM_FIELD_WORDS];uint32 l372[SOC_MAX_MEM_FIELD_WORDS];uint32 l373[
SOC_MAX_MEM_FIELD_WORDS];void*l374;int l145;int l375,l128;l354 = (l9[(l74)][(
l87)].l3);l355 = l370;if(!l99){l355>>= 1;if((l145 = READ_L3_DEFIPm(l74,
MEM_BLOCK_ANY,l354,l371))<0){return l145;}if((l145 = READ_L3_DEFIP_AUX_TABLEm
(l74,MEM_BLOCK_ANY,soc_th_alpm_physical_idx(l74,L3_DEFIPm,l354,1),l372))<0){
return l145;}if((l145 = READ_L3_DEFIP_AUX_TABLEm(l74,MEM_BLOCK_ANY,
soc_th_alpm_physical_idx(l74,L3_DEFIPm,l355,1),l373))<0){return l145;}l374 = 
(l355 == l354)?l371:l75;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l371),(l61[(l74)]->l40))){l128 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_TABLEm,l372,BPM_LENGTH1f);if(l370&1){l145 = 
soc_th_alpm_lpm_ip4entry1_to_1(l74,l371,l374,PRESERVE_HIT);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,l373,BPM_LENGTH1f,l128);}else{
l145 = soc_th_alpm_lpm_ip4entry1_to_0(l74,l371,l374,PRESERVE_HIT);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,l373,BPM_LENGTH0f,l128);}l375 = (
l354<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(
l371),(l61[(l74)]->l40),(0));}else{l128 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_TABLEm,l372,BPM_LENGTH0f);if(l370&1){l145 = 
soc_th_alpm_lpm_ip4entry0_to_1(l74,l371,l374,PRESERVE_HIT);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,l373,BPM_LENGTH1f,l128);}else{
l145 = soc_th_alpm_lpm_ip4entry0_to_0(l74,l371,l374,PRESERVE_HIT);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,l373,BPM_LENGTH0f,l128);}l375 = 
l354<<1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(
l371),(l61[(l74)]->l39),(0));(l9[(l74)][(l87)].l5)-= 1;(l9[(l74)][(l87)].l6)
+= 1;(l9[(l74)][(l87)].l3)-= 1;}l375 = soc_th_alpm_physical_idx(l74,L3_DEFIPm
,l375,0);l370 = soc_th_alpm_physical_idx(l74,L3_DEFIPm,l370,0);
ALPM_TCAM_PIVOT(l74,l370) = ALPM_TCAM_PIVOT(l74,l375);if(ALPM_TCAM_PIVOT(l74,
l370)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l74,l370)) = l370;
soc_th_alpm_lpm_move_bu_upd(l74,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l74,l370))
,l370);}ALPM_TCAM_PIVOT(l74,l375) = NULL;if((l145 = WRITE_L3_DEFIP_AUX_TABLEm
(l74,MEM_BLOCK_ALL,soc_th_alpm_physical_idx(l74,L3_DEFIPm,l355,1),l373))<0){
return l145;}if(l355!= l354){l334(l74,l374,l355,0x4000,0);if((l145 = l233(l74
,MEM_BLOCK_ANY,l355,l355,l374))<0){return l145;}if((l145 = l348(l74,l355,l374
))<0){return l145;}}l334(l74,l371,l354,0x4000,0);if((l145 = l233(l74,
MEM_BLOCK_ANY,l354,l354,l371))<0){return l145;}if((l145 = l348(l74,l354,l371)
)<0){return l145;}}else{(l9[(l74)][(l87)].l5)-= 1;(l9[(l74)][(l87)].l6)+= 1;(
l9[(l74)][(l87)].l3)-= 1;if(l355!= l354){if((l145 = READ_L3_DEFIPm(l74,
MEM_BLOCK_ANY,l354,l371))<0){return l145;}l334(l74,l371,l355,0x4000,0);if((
l145 = l233(l74,MEM_BLOCK_ANY,l355,l354,l371))<0){return l145;}if((l145 = 
l348(l74,l355,l371))<0){return l145;}}l370 = soc_th_alpm_physical_idx(l74,
L3_DEFIPm,l355,1);l375 = soc_th_alpm_physical_idx(l74,L3_DEFIPm,l354,1);
ALPM_TCAM_PIVOT(l74,l370<<1) = ALPM_TCAM_PIVOT(l74,l375<<1);ALPM_TCAM_PIVOT(
l74,l375<<1) = NULL;if(ALPM_TCAM_PIVOT(l74,l370<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l74,l370<<1)) = l370<<1;soc_th_alpm_lpm_move_bu_upd(l74,
PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l74,l370<<1)),l370<<1);}sal_memcpy(l371,
soc_mem_entry_null(l74,L3_DEFIPm),soc_mem_entry_words(l74,L3_DEFIPm)*4);l334(
l74,l371,l354,0x4000,0);if((l145 = l233(l74,MEM_BLOCK_ANY,l354,l354,l371))<0)
{return l145;}if((l145 = l348(l74,l354,l371))<0){return l145;}}if((l9[(l74)][
(l87)].l5) == 0){l365 = (l9[(l74)][(l87)].l4);assert(l365!= -1);l366 = (l9[(
l74)][(l87)].next);(l9[(l74)][(l365)].next) = l366;(l9[(l74)][(l365)].l6)+= (
l9[(l74)][(l87)].l6);(l9[(l74)][(l87)].l6) = 0;if(l366!= -1){(l9[(l74)][(l366
)].l4) = l365;}(l9[(l74)][(l87)].next) = -1;(l9[(l74)][(l87)].l4) = -1;(l9[(
l74)][(l87)].l2) = -1;(l9[(l74)][(l87)].l3) = -1;}return(l145);}int
soc_th_alpm_lpm_vrf_get(int unit,void*lpm_entry,int*l116,int*l132){int l162;
if(((l61[(unit)]->l43)!= NULL)){l162 = soc_L3_DEFIPm_field32_get(unit,
lpm_entry,VRF_ID_0f);*l132 = l162;if(soc_L3_DEFIPm_field32_get(unit,lpm_entry
,VRF_ID_MASK0f)){*l116 = l162;}else if(!soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l61[(unit)]->l45))){*l116 = 
SOC_L3_VRF_GLOBAL;*l132 = SOC_VRF_MAX(unit)+1;}else{*l116 = 
SOC_L3_VRF_OVERRIDE;}}else{*l116 = SOC_L3_VRF_DEFAULT;}return(SOC_E_NONE);}
static int l376(int l74,void*entry,int*l105){int l87;int l145;int l99;uint32
l146;int l162;int l377;l99 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(entry),(l61[(l74)]->l29));if(l99){l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(entry),(l61
[(l74)]->l27));if((l145 = _ipmask2pfx(l146,&l87))<0){return(l145);}l146 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(entry),(l61
[(l74)]->l28));if(l87){if(l146!= 0xffffffff){return(SOC_E_PARAM);}l87+= 32;}
else{if((l145 = _ipmask2pfx(l146,&l87))<0){return(l145);}}l87+= 33;}else{l146
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(entry),(
l61[(l74)]->l27));if((l145 = _ipmask2pfx(l146,&l87))<0){return(l145);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,entry,&l162,&l145));l377 = 
soc_th_alpm_mode_get(l74);switch(l162){case SOC_L3_VRF_GLOBAL:if((l377 == 
SOC_ALPM_MODE_PARALLEL)||(l377 == SOC_ALPM_MODE_TCAM_ALPM)){*l105 = l87+((3*(
64+32+2+1))/3);}else{*l105 = l87;}break;case SOC_L3_VRF_OVERRIDE:*l105 = l87+
2*((3*(64+32+2+1))/3);break;default:if((l377 == SOC_ALPM_MODE_PARALLEL)||(
l377 == SOC_ALPM_MODE_TCAM_ALPM)){*l105 = l87;}else{*l105 = l87+((3*(64+32+2+
1))/3);}break;}return(SOC_E_NONE);}static int l103(int l74,void*key_data,void
*l75,int*l104,int*l105,int*l99){int l145;int l122;int l88;int l87 = 0;l122 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(
l61[(l74)]->l29));if(l122){if(!(l122 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(l61[(l74)]->l30)))){return(
SOC_E_PARAM);}}*l99 = l122;l376(l74,key_data,&l87);*l105 = l87;if(l338(l74,
key_data,l87,&l88) == SOC_E_NONE){*l104 = l88;if((l145 = READ_L3_DEFIPm(l74,
MEM_BLOCK_ANY,(*l99)?*l104:(*l104>>1),l75))<0){return l145;}return(SOC_E_NONE
);}else{return(SOC_E_NOT_FOUND);}}static int l94(int l74){int l347;int l167;
int l328;int l378;uint32 l143,l377;if(!soc_feature(l74,soc_feature_lpm_tcam))
{return(SOC_E_UNAVAIL);}if((l377 = soc_property_get(l74,spn_L3_ALPM_ENABLE,0)
)){SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l74,&l143));
soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr,&l143,LPM_MODEf,1);if(l377 == 1){
soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr,&l143,LOOKUP_MODEf,1);}else if(
l377 == 2){soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr,&l143,LOOKUP_MODEf,0);
l3_alpm_ipv4_double_wide[l74] = soc_property_get(l74,
"l3_alpm_ipv4_double_wide",0);if(l3_alpm_ipv4_double_wide[l74]){
soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr,&l143,
ENABLE_DOUBLE_WIDE_BUCKETS_FOR_IPV4f,1);}}else if(l377 == 3){
soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr,&l143,LOOKUP_MODEf,2);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l74,l143));l143 = 0;if(
SOC_URPF_STATUS_GET(l74)){soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l143,
URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l143,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l143,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l143,
URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l74,l143)
);l143 = 0;if(l377 == 1){if(SOC_URPF_STATUS_GET(l74)){soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM2_SELf,1);soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM3_SELf,1);soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM4_SELf,2);soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM5_SELf,2);soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM6_SELf,3);soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM7_SELf,3);}else{soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM4_SELf,1);soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM5_SELf,1);soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM6_SELf,1);soc_reg_field_set(l74,
L3_DEFIP_ALPM_CFGr,&l143,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l74)){
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l143,TCAM4_SELf,2);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l143,TCAM5_SELf,2);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l143,TCAM6_SELf,2);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l143,TCAM7_SELf,2);}}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_ALPM_CFGr(l74,l143));if(soc_property_get(
l74,spn_IPV6_LPM_128B_ENABLE,1)){uint32 l379 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l74,&l379));soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&
l379,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l379,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l379,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l379,
V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l74,l379)
);}}l347 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l74);l378 = sizeof(l7)*(l347);if
((l9[(l74)]!= NULL)){if(soc_th_alpm_deinit(l74)<0){SOC_ALPM_LPM_UNLOCK(l74);
return SOC_E_INTERNAL;}}l61[l74] = sal_alloc(sizeof(l59),"lpm_field_state");
if(NULL == l61[l74]){SOC_ALPM_LPM_UNLOCK(l74);return(SOC_E_MEMORY);}(l61[l74]
)->l11 = soc_mem_fieldinfo_get(l74,L3_DEFIPm,CLASS_ID0f);(l61[l74])->l12 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,CLASS_ID1f);(l61[l74])->l13 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,DST_DISCARD0f);(l61[l74])->l14 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,DST_DISCARD1f);(l61[l74])->l15 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ECMP0f);(l61[l74])->l16 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ECMP1f);(l61[l74])->l17 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ECMP_COUNT0f);(l61[l74])->l18 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ECMP_COUNT1f);(l61[l74])->l19 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ECMP_PTR0f);(l61[l74])->l20 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ECMP_PTR1f);(l61[l74])->l21 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,GLOBAL_ROUTE0f);(l61[l74])->l22 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,GLOBAL_ROUTE1f);(l61[l74])->l23 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,HIT0f);(l61[l74])->l24 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,HIT1f);(l61[l74])->l25 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,IP_ADDR0f);(l61[l74])->l26 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,IP_ADDR1f);(l61[l74])->l27 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,IP_ADDR_MASK0f);(l61[l74])->l28 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,IP_ADDR_MASK1f);(l61[l74])->l29 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,MODE0f);(l61[l74])->l30 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,MODE1f);(l61[l74])->l31 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,MODE_MASK0f);(l61[l74])->l32 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,MODE_MASK1f);(l61[l74])->l33 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,NEXT_HOP_INDEX0f);(l61[l74])->l34 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,NEXT_HOP_INDEX1f);(l61[l74])->l35 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,PRI0f);(l61[l74])->l36 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,PRI1f);(l61[l74])->l37 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,RPE0f);(l61[l74])->l38 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,RPE1f);(l61[l74])->l39 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,VALID0f);(l61[l74])->l40 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,VALID1f);(l61[l74])->l41 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,VRF_ID_0f);(l61[l74])->l42 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,VRF_ID_1f);(l61[l74])->l43 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,VRF_ID_MASK0f);(l61[l74])->l44 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,VRF_ID_MASK1f);(l61[l74])->l45 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,GLOBAL_HIGH0f);(l61[l74])->l46 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,GLOBAL_HIGH1f);(l61[l74])->l47 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ALG_HIT_IDX0f);(l61[l74])->l48 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ALG_HIT_IDX1f);(l61[l74])->l49 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ALG_BKT_PTR0f);(l61[l74])->l50 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ALG_BKT_PTR1f);(l61[l74])->l51 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ALG_SUB_BKT_PTR0f);(l61[l74])->l52 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,ALG_SUB_BKT_PTR1f);(l61[l74])->l53 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,DEFAULT_MISS0f);(l61[l74])->l54 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,DEFAULT_MISS1f);(l61[l74])->l55 = 
soc_mem_fieldinfo_get(l74,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l61[l74])->
l56 = soc_mem_fieldinfo_get(l74,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(l61[
l74])->l57 = soc_mem_fieldinfo_get(l74,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f);(l61
[l74])->l58 = soc_mem_fieldinfo_get(l74,L3_DEFIPm,FLEX_CTR_POOL_NUMBER1f);(l9
[(l74)]) = sal_alloc(l378,"LPM prefix info");if(NULL == (l9[(l74)])){sal_free
(l61[l74]);l61[l74] = NULL;SOC_ALPM_LPM_UNLOCK(l74);return(SOC_E_MEMORY);}
sal_memset((l9[(l74)]),0,l378);for(l167 = 0;l167<l347;l167++){(l9[(l74)][(
l167)].l2) = -1;(l9[(l74)][(l167)].l3) = -1;(l9[(l74)][(l167)].l4) = -1;(l9[(
l74)][(l167)].next) = -1;(l9[(l74)][(l167)].l5) = 0;(l9[(l74)][(l167)].l6) = 
0;}l328 = soc_mem_index_count(l74,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l74)){
l328>>= 1;}if((l377 == 1)||(l377 == 3)){(l9[(l74)][(((3*(64+32+2+1))-1))].l3)
= (l328>>1)-1;(l9[(l74)][(((((3*(64+32+2+1))/3)-1)))].l6) = l328>>1;(l9[(l74)
][((((3*(64+32+2+1))-1)))].l6) = (l328-(l9[(l74)][(((((3*(64+32+2+1))/3)-1)))
].l6));}else{(l9[(l74)][((((3*(64+32+2+1))-1)))].l6) = l328;}if((l72[(l74)])
!= NULL){if(l82((l72[(l74)]))<0){SOC_ALPM_LPM_UNLOCK(l74);return
SOC_E_INTERNAL;}(l72[(l74)]) = NULL;}if(l80(l74,l328*2,l328,&(l72[(l74)]))<0)
{SOC_ALPM_LPM_UNLOCK(l74);return SOC_E_MEMORY;}SOC_ALPM_LPM_UNLOCK(l74);
return(SOC_E_NONE);}static int l95(int l74){if(!soc_feature(l74,
soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}SOC_ALPM_LPM_LOCK(l74);if((l72[
(l74)])!= NULL){l82((l72[(l74)]));(l72[(l74)]) = NULL;}if((l9[(l74)]!= NULL))
{sal_free(l61[l74]);l61[l74] = NULL;sal_free((l9[(l74)]));(l9[(l74)]) = NULL;
}SOC_ALPM_LPM_UNLOCK(l74);return(SOC_E_NONE);}static int l96(int l74,void*l97
,int*l178){int l87;int index;int l99;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int
l145 = SOC_E_NONE;int l380 = 0;sal_memcpy(l75,soc_mem_entry_null(l74,
L3_DEFIPm),soc_mem_entry_words(l74,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l74);l145 = 
l103(l74,l97,l75,&index,&l87,&l99);if(l145 == SOC_E_NOT_FOUND){l145 = l363(
l74,l87,l99,l75,&index);if(l145<0){SOC_ALPM_LPM_UNLOCK(l74);return(l145);}}
else{l380 = 1;}*l178 = index;if(l145 == SOC_E_NONE){if(!l99){if(index&1){l145
= soc_th_alpm_lpm_ip4entry0_to_1(l74,l97,l75,PRESERVE_HIT);}else{l145 = 
soc_th_alpm_lpm_ip4entry0_to_0(l74,l97,l75,PRESERVE_HIT);}if(l145<0){
SOC_ALPM_LPM_UNLOCK(l74);return(l145);}l97 = (void*)l75;index>>= 1;}
soc_th_alpm_lpm_state_dump(l74);LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_lpm_insert: %d %d\n"),index,l87));if(!l380){l334(l74,l97,index,
0x4000,0);}l145 = l233(l74,MEM_BLOCK_ANY,index,index,l97);if(l145>= 0){l145 = 
l348(l74,index,l97);}}SOC_ALPM_LPM_UNLOCK(l74);return(l145);}static int
soc_th_alpm_lpm_delete(int l74,void*key_data){int l87;int index;int l99;
uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int l145 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(
l74);l145 = l103(l74,key_data,l75,&index,&l87,&l99);if(l145 == SOC_E_NONE){
LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"soc_alpm_lpm_delete: %d %d\n"),
index,l87));l337(l74,key_data,index);l145 = l369(l74,l87,l99,l75,index);}
soc_th_alpm_lpm_state_dump(l74);SOC_ALPM_LPM_UNLOCK(l74);return(l145);}static
int l106(int l74,void*key_data,void*l75,int*l104){int l87;int l145;int l99;
SOC_ALPM_LPM_LOCK(l74);l145 = l103(l74,key_data,l75,l104,&l87,&l99);
SOC_ALPM_LPM_UNLOCK(l74);return(l145);}static int l98(int unit,void*key_data,
int l99,int l100,int l101,defip_aux_scratch_entry_t*l102){uint32 l146;uint32
l381[4] = {0,0,0,0};int l87 = 0;int l145 = SOC_E_NONE;l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,VALID0f);soc_mem_field32_set(unit
,L3_DEFIP_AUX_SCRATCHm,l102,VALIDf,l146);l146 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,MODE0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,
l102,MODEf,l146);l146 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
ENTRY_TYPE0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,ENTRY_TYPEf
,0);l146 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,GLOBAL_ROUTE0f);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,GLOBAL_ROUTEf,l146);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,ECMP0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l102,ECMPf,l146);l146 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,ECMP_PTR0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l102,ECMP_PTRf,l146);l146 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
NEXT_HOP_INDEX0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,
NEXT_HOP_INDEXf,l146);l146 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
PRI0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,PRIf,l146);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,RPE0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l102,RPEf,l146);l146 =soc_mem_field32_get(unit,
L3_DEFIPm,key_data,VRF_ID_0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,
l102,VRFf,l146);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,DB_TYPEf,
l100);l146 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,DST_DISCARD0f);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,DST_DISCARDf,l146);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,CLASS_ID0f);soc_mem_field32_set(
unit,L3_DEFIP_AUX_SCRATCHm,l102,CLASS_IDf,l146);if(l99){l381[2] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR0f);l381[3] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR1f);}else{l381[0] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR0f);}soc_mem_field_set(
unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)l102,IP_ADDRf,(uint32*)l381);if(l99){l146
= soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR_MASK0f);if((l145 = 
_ipmask2pfx(l146,&l87))<0){return(l145);}l146 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,IP_ADDR_MASK1f);if(l87){if(l146!= 0xffffffff){return(
SOC_E_PARAM);}l87+= 32;}else{if((l145 = _ipmask2pfx(l146,&l87))<0){return(
l145);}}}else{l146 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
IP_ADDR_MASK0f);if((l145 = _ipmask2pfx(l146,&l87))<0){return(l145);}}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,IP_LENGTHf,l87);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,REPLACE_LENf,l101);return
(SOC_E_NONE);}int _soc_th_alpm_aux_op(int l74,_soc_aux_op_t l382,
defip_aux_scratch_entry_t*l102,int l383,int*l158,int*tcam_index,int*bktid){
uint32 l143,l384;int l385;soc_timeout_t l386;int l145 = SOC_E_NONE;int l387 = 
0;if(l383){SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_SCRATCHm(l74,MEM_BLOCK_ANY,
0,l102));}l388:l143 = 0;switch(l382){case INSERT_PROPAGATE:l385 = 0;break;
case DELETE_PROPAGATE:l385 = 1;break;case PREFIX_LOOKUP:l385 = 2;break;case
HITBIT_REPLACE:l385 = 3;break;default:return SOC_E_PARAM;}soc_reg_field_set(
l74,L3_DEFIP_AUX_CTRLr,&l143,OPCODEf,l385);soc_reg_field_set(l74,
L3_DEFIP_AUX_CTRLr,&l143,STARTf,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l74,l143));soc_timeout_init(&l386,50000,5);l385 = 0;
do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l74,&l143));l385 = 
soc_reg_field_get(l74,L3_DEFIP_AUX_CTRLr,l143,DONEf);if(l385 == 1){l145 = 
SOC_E_NONE;break;}if(soc_timeout_check(&l386)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l74,&l143));l385 = soc_reg_field_get(l74,
L3_DEFIP_AUX_CTRLr,l143,DONEf);if(l385 == 1){l145 = SOC_E_NONE;}else{LOG_WARN
(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d : DEFIP AUX Operation timeout\n"),
l74));l145 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l145)){if(
soc_reg_field_get(l74,L3_DEFIP_AUX_CTRLr,l143,ERRORf)){soc_reg_field_set(l74,
L3_DEFIP_AUX_CTRLr,&l143,STARTf,0);soc_reg_field_set(l74,L3_DEFIP_AUX_CTRLr,&
l143,ERRORf,0);soc_reg_field_set(l74,L3_DEFIP_AUX_CTRLr,&l143,DONEf,0);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l74,l143));LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d: DEFIP AUX Operation encountered "
"parity error !!\n"),l74));l387++;if(SOC_CONTROL(l74)->alpm_bulk_retry){
sal_sem_take(SOC_CONTROL(l74)->alpm_bulk_retry,1000000);}if(l387<5){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d: Retry DEFIP AUX Operation..\n"),l74
));goto l388;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l74));return SOC_E_INTERNAL;}}if(l382 == PREFIX_LOOKUP){if(l158&&tcam_index){
*l158 = soc_reg_field_get(l74,L3_DEFIP_AUX_CTRLr,l143,HITf);*tcam_index = 
soc_reg_field_get(l74,L3_DEFIP_AUX_CTRLr,l143,BKT_INDEXf);SOC_IF_ERROR_RETURN
(READ_L3_DEFIP_AUX_CTRL_1r(l74,&l384));*bktid = ALPM_BKTID(soc_reg_field_get(
l74,L3_DEFIP_AUX_CTRL_1r,l384,BKT_PTRf),soc_reg_field_get(l74,
L3_DEFIP_AUX_CTRL_1r,l384,SUB_BKT_PTRf));}}}return l145;}static int l108(int
unit,void*lpm_entry,void*l109,void*l110,soc_mem_t l111,uint32 l112,uint32*
l389,int l114){uint32 l146;uint32 l381[4] = {0,0};int l87 = 0;int l145 = 
SOC_E_NONE;int l99;uint32 l113 = 0;l99 = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,MODE0f);sal_memset(l109,0,soc_mem_entry_words(unit,l111)*4);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,HIT0f);soc_mem_field32_set(unit,
l111,l109,HITf,l146);l146 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,
VALID0f);soc_mem_field32_set(unit,l111,l109,VALIDf,l146);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,ECMP0f);soc_mem_field32_set(unit
,l111,l109,ECMPf,l146);l146 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,
ECMP_PTR0f);soc_mem_field32_set(unit,l111,l109,ECMP_PTRf,l146);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f);
soc_mem_field32_set(unit,l111,l109,NEXT_HOP_INDEXf,l146);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,PRI0f);soc_mem_field32_set(unit,
l111,l109,PRIf,l146);l146 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,
RPE0f);soc_mem_field32_set(unit,l111,l109,RPEf,l146);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(unit,l111,l109,DST_DISCARDf,l146);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);
soc_mem_field32_set(unit,l111,l109,SRC_DISCARDf,l146);l146 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,CLASS_ID0f);soc_mem_field32_set(
unit,l111,l109,CLASS_IDf,l146);soc_mem_field32_set(unit,l111,l109,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l114));l381[0] = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,IP_ADDR0f);if(l99){l381[1] = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,IP_ADDR1f);}soc_mem_field_set(unit,l111,(uint32*)l109,
KEYf,(uint32*)l381);if(l99){l146 = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,IP_ADDR_MASK0f);if((l145 = _ipmask2pfx(l146,&l87))<0){return(l145);
}l146 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f);if(l87){
if(l146!= 0xffffffff){return(SOC_E_PARAM);}l87+= 32;}else{if((l145 = 
_ipmask2pfx(l146,&l87))<0){return(l145);}}}else{l146 = soc_mem_field32_get(
unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l145 = _ipmask2pfx(l146,&l87))<0
){return(l145);}}if((l87 == 0)&&(l381[0] == 0)&&(l381[1] == 0)){l113 = 1;}if(
l389!= NULL){*l389 = l113;}soc_mem_field32_set(unit,l111,l109,LENGTHf,l87);if
(l110 == NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){sal_memset(
l110,0,soc_mem_entry_words(unit,l111)*4);sal_memcpy(l110,l109,
soc_mem_entry_words(unit,l111)*4);soc_mem_field32_set(unit,l111,l110,
DST_DISCARDf,0);soc_mem_field32_set(unit,l111,l110,RPEf,0);
soc_mem_field32_set(unit,l111,l110,SRC_DISCARDf,l112&SOC_ALPM_RPF_SRC_DISCARD
);soc_mem_field32_set(unit,l111,l110,DEFAULTROUTEf,l113);}return(SOC_E_NONE);
}static int l115(int unit,void*l109,soc_mem_t l111,int l99,int l116,int l117,
int index,void*lpm_entry){uint32 l146;uint32 l381[4] = {0,0};uint32 l87 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_words(unit,L3_DEFIPm)*4);l146 = 
soc_mem_field32_get(unit,l111,l109,HITf);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,HIT0f,l146);if(l99){soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
HIT1f,l146);}l146 = soc_mem_field32_get(unit,l111,l109,VALIDf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VALID0f,l146);if(l99){
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VALID1f,l146);}l146 = 
soc_mem_field32_get(unit,l111,l109,ECMPf);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,ECMP0f,l146);l146 = soc_mem_field32_get(unit,l111,l109,ECMP_PTRf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ECMP_PTR0f,l146);l146 = 
soc_mem_field32_get(unit,l111,l109,NEXT_HOP_INDEXf);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f,l146);l146 = soc_mem_field32_get(unit,
l111,l109,PRIf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,PRI0f,l146);l146
= soc_mem_field32_get(unit,l111,l109,RPEf);soc_mem_field32_set(unit,L3_DEFIPm
,lpm_entry,RPE0f,l146);l146 = soc_mem_field32_get(unit,l111,l109,DST_DISCARDf
);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,DST_DISCARD0f,l146);l146 = 
soc_mem_field32_get(unit,l111,l109,SRC_DISCARDf);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,SRC_DISCARD0f,l146);l146 = soc_mem_field32_get(unit,l111,
l109,CLASS_IDf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,CLASS_ID0f,l146)
;soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ALG_BKT_PTR0f,ALPM_BKT_IDX(l117
));soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ALG_SUB_BKT_PTR0f,
ALPM_BKT_SIDX(l117));soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
ALG_HIT_IDX0f,index);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE_MASK0f
,3);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l99)
{soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(
unit,l111,l109,KEYf,l381);if(l99){soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,IP_ADDR1f,l381[1]);}soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR0f,l381[0]);l146 = soc_mem_field32_get(unit,l111,l109,LENGTHf);if(l99)
{if(l146>= 32){l87 = 0xffffffff;soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l87);l87 = ~(((l146-32) == 32)?0:(0xffffffff)>>(l146-32));
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l87);}else{l87 = 
~(0xffffffff>>l146);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l87);}}else{assert(l146<= 32);l87 = ~(((l146) == 32)?0:(
0xffffffff)>>(l146));soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK0f,l87);}if(l116 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(unit
,L3_DEFIPm,lpm_entry,GLOBAL_HIGH0f,1);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else if(l116 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,GLOBAL_ROUTE0f,1);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
VRF_ID_0f,0);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,0);}
else{soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VRF_ID_0f,l116);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VRF_ID_MASK0f,SOC_VRF_MAX(unit))
;}return(SOC_E_NONE);}int soc_th_alpm_warmboot_pivot_add(int unit,int l99,
void*lpm_entry,int l390,int bktid){int l145 = SOC_E_NONE;uint32 key[2] = {0,0
};alpm_pivot_t*l126 = NULL;alpm_bucket_handle_t*l257 = NULL;int l162 = 0,l116
= 0;uint32 l391;trie_t*l297 = NULL;uint32 prefix[5] = {0};int l113 = 0;
SOC_IF_ERROR_RETURN(l153(unit,lpm_entry,prefix,&l391,&l113));
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l162,&l116));l390
= soc_th_alpm_physical_idx(unit,L3_DEFIPm,l390,l99);l257 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l257 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l257,0,sizeof(*l257));
l126 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l126 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l257);return SOC_E_MEMORY;}sal_memset(l126,0
,sizeof(*l126));PIVOT_BUCKET_HANDLE(l126) = l257;if(l99){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l126));key[0] = 
soc_L3_DEFIPm_field32_get(unit,lpm_entry,IP_ADDR0f);key[1] = 
soc_L3_DEFIPm_field32_get(unit,lpm_entry,IP_ADDR1f);}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l126));key[0] = soc_L3_DEFIPm_field32_get
(unit,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l126) = bktid;PIVOT_TCAM_INDEX
(l126) = l390;if(l162!= SOC_L3_VRF_OVERRIDE){if(l99 == 0){l297 = 
VRF_PIVOT_TRIE_IPV4(unit,l116);if(l297 == NULL){trie_init(_MAX_KEY_LEN_48_,&
VRF_PIVOT_TRIE_IPV4(unit,l116));l297 = VRF_PIVOT_TRIE_IPV4(unit,l116);}}else{
l297 = VRF_PIVOT_TRIE_IPV6(unit,l116);if(l297 == NULL){trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(unit,l116));l297 = VRF_PIVOT_TRIE_IPV6
(unit,l116);}}sal_memcpy(l126->key,prefix,sizeof(prefix));l126->len = l391;
l145 = trie_insert(l297,l126->key,NULL,l126->len,(trie_node_t*)l126);if(
SOC_FAILURE(l145)){sal_free(l257);sal_free(l126);return l145;}}
ALPM_TCAM_PIVOT(unit,l390) = l126;PIVOT_BUCKET_VRF(l126) = l116;
PIVOT_BUCKET_IPV6(l126) = l99;PIVOT_BUCKET_ENT_CNT_UPDATE(l126);if(key[0] == 
0&&key[1] == 0){PIVOT_BUCKET_DEF(l126) = TRUE;}VRF_PIVOT_REF_INC(unit,l116,
l99);return l145;}static int l392(int unit,int l99,void*lpm_entry,void*l109,
soc_mem_t l111,int l390,int bktid,int l393){int l394;int l116;int l145 = 
SOC_E_NONE;int l113 = 0;uint32 prefix[5] = {0,0,0,0,0};uint32 l127;
defip_entry_t l395;trie_t*l258 = NULL;trie_t*l125 = NULL;trie_node_t*l254 = 
NULL;payload_t*l396 = NULL;payload_t*l275 = NULL;alpm_pivot_t*pivot_pyld = 
NULL;if((NULL == lpm_entry)||(NULL == l109)){return SOC_E_PARAM;}if(l99){if(!
(l99 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&
l394,&l116));l111 = (l99)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;
SOC_IF_ERROR_RETURN(l115(unit,l109,l111,l99,l394,bktid,l390,&l395));l145 = 
l153(unit,&l395,prefix,&l127,&l113);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"prefix create failed\n")));return l145;}
pivot_pyld = ALPM_TCAM_PIVOT(unit,l390);l258 = PIVOT_BUCKET_TRIE(pivot_pyld);
l396 = sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == l396){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l275 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l275){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node\n")));sal_free(l396);return
SOC_E_MEMORY;}sal_memset(l396,0,sizeof(*l396));sal_memset(l275,0,sizeof(*l275
));l396->key[0] = prefix[0];l396->key[1] = prefix[1];l396->key[2] = prefix[2]
;l396->key[3] = prefix[3];l396->key[4] = prefix[4];l396->len = l127;l396->
index = l393;sal_memcpy(l275,l396,sizeof(*l396));l145 = trie_insert(l258,
prefix,NULL,l127,(trie_node_t*)l396);if(SOC_FAILURE(l145)){goto l397;}if(l99)
{l125 = VRF_PREFIX_TRIE_IPV6(unit,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4(
unit,l116);}if(!l113){l145 = trie_insert(l125,prefix,NULL,l127,(trie_node_t*)
l275);if(SOC_FAILURE(l145)){goto l398;}}return l145;l398:(void)trie_delete(
l258,prefix,l127,&l254);l396 = (payload_t*)l254;l397:sal_free(l396);sal_free(
l275);return l145;}static int l399(int unit,int l122,int l116,int l148,int
bktid){int l145 = SOC_E_NONE;uint32 l127;uint32 key[2] = {0,0};trie_t*l400 = 
NULL;payload_t*l286 = NULL;defip_entry_t*lpm_entry = NULL;lpm_entry = 
sal_alloc(sizeof(defip_entry_t),"Default LPM entry");if(lpm_entry == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"unable to allocate memory for LPM entry\n")));return SOC_E_MEMORY;}l118(unit
,key,0,l116,l122,lpm_entry,0,1);if(l116 == SOC_VRF_MAX(unit)+1){
soc_L3_DEFIPm_field32_set(unit,lpm_entry,GLOBAL_ROUTE0f,1);}else{
soc_L3_DEFIPm_field32_set(unit,lpm_entry,DEFAULT_MISS0f,1);}
soc_L3_DEFIPm_field32_set(unit,lpm_entry,ALG_BKT_PTR0f,ALPM_BKT_IDX(bktid));
soc_L3_DEFIPm_field32_set(unit,lpm_entry,ALG_SUB_BKT_PTR0f,ALPM_BKT_SIDX(
bktid));if(l122 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(unit,l116) = lpm_entry;
trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(unit,l116));l400 = 
VRF_PREFIX_TRIE_IPV4(unit,l116);}else{VRF_TRIE_DEFAULT_ROUTE_IPV6(unit,l116) = 
lpm_entry;trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(unit,l116));l400 = 
VRF_PREFIX_TRIE_IPV6(unit,l116);}l286 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l286 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l286,0,sizeof(*l286));l127 = 0;l286->key[0] = key[0]
;l286->key[1] = key[1];l286->len = l127;l145 = trie_insert(l400,key,NULL,l127
,&(l286->node));if(SOC_FAILURE(l145)){sal_free(l286);return l145;}
VRF_TRIE_INIT_DONE(unit,l116,l122,1);return l145;}int
soc_th_alpm_warmboot_prefix_insert(int unit,int l99,void*lpm_entry,void*l109,
int l390,int bktid,int l393){int l394;int l116;int l145 = SOC_E_NONE;
soc_mem_t l111;l390 = soc_th_alpm_physical_idx(unit,L3_DEFIPm,l390,l99);l111 = 
(l99)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l394,&l116));if(l394 == 
SOC_L3_VRF_OVERRIDE){return(l145);}if(!VRF_TRIE_INIT_COMPLETED(unit,l116,l99)
){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d is not initialized\n")
,l116));l145 = l399(unit,l99,l116,l390,bktid);if(SOC_FAILURE(l145)){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d/%d trie init \n""failed\n"),l116,
l99));return l145;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d/%d trie init completed\n"),l116,l99));}l145 = l392(unit,l99,lpm_entry
,l109,l111,l390,bktid,l393);if(l145!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"unit %d : Route Insertion Failed :%s\n"),unit,soc_errmsg(
l145)));return(l145);}soc_th_alpm_bu_upd(unit,bktid,l390,l116,l99,1);
VRF_TRIE_ROUTES_INC(unit,l116,l99);return(l145);}int
soc_th_alpm_warmboot_bucket_bitmap_set(int l74,int l116,int l122,int l213){
int l220 = 1;SHR_BITDCL*l329 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74);if(l116 == 
SOC_VRF_MAX(l74)+1){l329 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l74);}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l220 = 2;}SHR_BITSET_RANGE(l329,l213,l220)
;return SOC_E_NONE;}int soc_th_alpm_warmboot_lpm_reinit_done(int unit){int
l148;int l401 = ((3*(64+32+2+1))-1);int l402;int l328 = soc_mem_index_count(
unit,L3_DEFIPm);if(SOC_URPF_STATUS_GET(unit)){l328>>= 1;}l402 = 
soc_th_alpm_mode_get(unit);if(l402 == 0){(l9[(unit)][(((3*(64+32+2+1))-1))].
l4) = -1;for(l148 = ((3*(64+32+2+1))-1);l148>-1;l148--){if(-1 == (l9[(unit)][
(l148)].l2)){continue;}(l9[(unit)][(l148)].l4) = l401;(l9[(unit)][(l401)].
next) = l148;(l9[(unit)][(l401)].l6) = (l9[(unit)][(l148)].l2)-(l9[(unit)][(
l401)].l3)-1;l401 = l148;}(l9[(unit)][(l401)].next) = -1;(l9[(unit)][(l401)].
l6) = l328-(l9[(unit)][(l401)].l3)-1;}else if(l402 == 1){(l9[(unit)][(((3*(64
+32+2+1))-1))].l4) = -1;for(l148 = ((3*(64+32+2+1))-1);l148>(((3*(64+32+2+1))
-1)/3);l148--){if(-1 == (l9[(unit)][(l148)].l2)){continue;}(l9[(unit)][(l148)
].l4) = l401;(l9[(unit)][(l401)].next) = l148;(l9[(unit)][(l401)].l6) = (l9[(
unit)][(l148)].l2)-(l9[(unit)][(l401)].l3)-1;l401 = l148;}(l9[(unit)][(l401)]
.next) = -1;(l9[(unit)][(l401)].l6) = l328-(l9[(unit)][(l401)].l3)-1;l401 = (
((3*(64+32+2+1))-1)/3);(l9[(unit)][((((3*(64+32+2+1))-1)/3))].l4) = -1;for(
l148 = ((((3*(64+32+2+1))-1)/3)-1);l148>-1;l148--){if(-1 == (l9[(unit)][(l148
)].l2)){continue;}(l9[(unit)][(l148)].l4) = l401;(l9[(unit)][(l401)].next) = 
l148;(l9[(unit)][(l401)].l6) = (l9[(unit)][(l148)].l2)-(l9[(unit)][(l401)].l3
)-1;l401 = l148;}(l9[(unit)][(l401)].next) = -1;(l9[(unit)][(l401)].l6) = (
l328>>1)-(l9[(unit)][(l401)].l3)-1;}return(SOC_E_NONE);}int
soc_th_alpm_warmboot_lpm_reinit(int unit,int l99,int l148,void*lpm_entry){int
l105;defip_entry_t*l403;if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID0f)
||soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID1f)){l334(unit,lpm_entry,l148
,0x4000,0);}if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID0f)){
SOC_IF_ERROR_RETURN(l376(unit,lpm_entry,&l105));if((l9[(unit)][(l105)].l5) == 
0){(l9[(unit)][(l105)].l2) = l148;(l9[(unit)][(l105)].l3) = l148;}else{(l9[(
unit)][(l105)].l3) = l148;}(l9[(unit)][(l105)].l5)++;if(l99){return(
SOC_E_NONE);}}else{if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID1f)){l403
= sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");
soc_th_alpm_lpm_ip4entry1_to_0(unit,lpm_entry,l403,TRUE);SOC_IF_ERROR_RETURN(
l376(unit,l403,&l105));if((l9[(unit)][(l105)].l5) == 0){(l9[(unit)][(l105)].
l2) = l148;(l9[(unit)][(l105)].l3) = l148;}else{(l9[(unit)][(l105)].l3) = 
l148;}sal_free(l403);(l9[(unit)][(l105)].l5)++;}}return(SOC_E_NONE);}void
soc_th_alpm_dbg_urpf(int l74){int l145 = SOC_E_NONE;soc_mem_t l111;int l167,
l149;int index;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];uint32 l404[
SOC_MAX_MEM_FIELD_WORDS];int l405 = 0,l406 = 0;if(!SOC_URPF_STATUS_GET(l74)){
LOG_CLI((BSL_META_U(l74,"Only supported in uRPF mode\n")));return;}for(l149 = 
0;l149<= SOC_TH_MAX_ALPM_BUCKETS/2;l149++){index = 
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,ALPM_BKTID(l149,0));l111 = 
_soc_tomahawk_alpm_bkt_view_get(l74,index);for(l167 = 0;l167<
SOC_TH_MAX_BUCKET_ENTRIES-1;l167++){l145 = _soc_th_alpm_mem_index(l74,l111,
l149,l167,0,&index);if(SOC_FAILURE(l145)){continue;}sal_memset(l75,0,sizeof(
uint32)*SOC_MAX_MEM_FIELD_WORDS);sal_memset(l404,0,sizeof(uint32)*
SOC_MAX_MEM_FIELD_WORDS);l145 = soc_mem_read(l74,l111,MEM_BLOCK_ANY,index,l75
);if(SOC_FAILURE(l145)){continue;}if(soc_mem_field32_get(l74,l111,l75,VALIDf)
== 0){continue;}soc_mem_field32_set(l74,l111,l75,HITf,0);l145 = soc_mem_read(
l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,index),l404);if(SOC_FAILURE
(l145)){continue;}soc_mem_field32_set(l74,l111,l404,HITf,0);
soc_mem_field32_set(l74,l111,l404,RPEf,0);if(sal_memcmp(l75,l404,sizeof(
uint32)*SOC_MAX_MEM_FIELD_WORDS)!= 0){LOG_CLI((BSL_META_U(l74,
"uRPF sanity check failed: bucket %4d mem %s index %d uRPF index %d\n"),l149,
SOC_MEM_NAME(l74,l111),index,_soc_th_alpm_rpf_entry(l74,index)));l406++;}else
{l405++;}}}LOG_CLI((BSL_META_U(l74,
"uRPF Sanity check finished, good %d bad %d\n"),l405,l406));return;}typedef
struct l407{int v4;int v6_64;int v6_128;int l408;int l409;int l410;int l221;}
l411;typedef enum l412{l413 = 0,l414,l415,l416,l417,l418}l419;static void l420
(int l74,alpm_vrf_counter_t*l421){l421->v4 = soc_mem_index_count(l74,
L3_DEFIPm)*2;l421->v6_128 = soc_mem_index_count(l74,L3_DEFIP_PAIR_128m);if(
soc_property_get(l74,spn_IPV6_LPM_128B_ENABLE,1)){l421->v6_64 = l421->v6_128;
}else{l421->v6_64 = l421->v4>>1;}if(SOC_URPF_STATUS_GET(l74)){l421->v4>>= 1;
l421->v6_128>>= 1;l421->v6_64>>= 1;}}static void l422(int l74,int l162,
alpm_vrf_handle_t*l423,l419 l424){alpm_vrf_counter_t*l425;int l167,l426,l427,
l428;int l380 = 0;alpm_vrf_counter_t l421;switch(l424){case l413:LOG_CLI((
BSL_META_U(l74,"\nAdd Counter:\n")));break;case l414:LOG_CLI((BSL_META_U(l74,
"\nDelete Counter:\n")));break;case l415:LOG_CLI((BSL_META_U(l74,
"\nInternal Debug Counter - 1:\n")));break;case l416:l420(l74,&l421);LOG_CLI(
(BSL_META_U(l74,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l421.
v4,l421.v6_64,l421.v6_128));break;case l417:LOG_CLI((BSL_META_U(l74,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l418:LOG_CLI((
BSL_META_U(l74,"\nInternal Debug Counter - LPM Full:\n")));break;default:
break;}LOG_CLI((BSL_META_U(l74,
"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((BSL_META_U(l74
,"-----------------------------------------------\n")));l426 = l427 = l428 = 
0;for(l167 = 0;l167<MAX_VRF_ID+1;l167++){int l429,l430,l431;if(l423[l167].
init_done == 0&&l167!= MAX_VRF_ID){continue;}if(l162!= -1&&l162!= l167){
continue;}l380 = 1;switch(l424){case l413:l425 = &l423[l167].add;break;case
l414:l425 = &l423[l167].del;break;case l415:l425 = &l423[l167].bkt_split;
break;case l417:l425 = &l423[l167].lpm_shift;break;case l418:l425 = &l423[
l167].lpm_full;break;case l416:l425 = &l423[l167].pivot_used;break;default:
l425 = &l423[l167].pivot_used;break;}l429 = l425->v4;l430 = l425->v6_64;l431 = 
l425->v6_128;l426+= l429;l427+= l430;l428+= l431;do{LOG_CLI((BSL_META_U(l74,
"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l167 == MAX_VRF_ID?-1:l167),(l429),(
l430),(l431),((l429+l430+l431)),(l167) == MAX_VRF_ID?"GHi":(l167) == 
SOC_VRF_MAX(l74)+1?"GLo":""));}while(0);}if(l380 == 0){LOG_CLI((BSL_META_U(
l74,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l74,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l74,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l426),(l427),(l428),((l426+
l427+l428))));}while(0);}return;}int soc_th_alpm_debug_show(int l74,int l162,
uint32 flags){int l167,l432,l380 = 0;l411*l433;l411 l174;l411 l434;if(l162>(
SOC_VRF_MAX(l74)+1)){return SOC_E_PARAM;}l432 = MAX_VRF_ID*sizeof(l411);l433 = 
sal_alloc(l432,"_alpm_dbg_cnt");if(l433 == NULL){return SOC_E_MEMORY;}
sal_memset(l433,0,l432);l174.v4 = ALPM_IPV4_BKT_COUNT;l174.v6_64 = 
ALPM_IPV6_64_BKT_COUNT;l174.v6_128 = ALPM_IPV6_128_BKT_COUNT;if(!
soc_th_alpm_mode_get(l74)&&!SOC_URPF_STATUS_GET(l74)){l174.v6_64<<= 1;l174.
v6_128<<= 1;}LOG_CLI((BSL_META_U(l74,"\nBucket Occupancy:\n")));if(flags&(
SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l74,
"\nPivot/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l74,"-----------------------------------------------\n")));}for(
l167 = 0;l167<MAX_PIVOT_COUNT;l167++){alpm_pivot_t*l435 = ALPM_TCAM_PIVOT(l74
,l167);if(l435!= NULL){l411*l436;int l116 = PIVOT_BUCKET_VRF(l435);if(l116<0
||l116>(SOC_VRF_MAX(l74)+1)){continue;}if(l162!= -1&&l162!= l116){continue;}
if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l380 = 1;do{LOG_CLI((BSL_META_U(l74,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l167,l116,PIVOT_BUCKET_MIN(l435),
PIVOT_BUCKET_MAX(l435),PIVOT_BUCKET_COUNT(l435),PIVOT_BUCKET_DEF(l435)?"Def":
(l116) == SOC_VRF_MAX(l74)+1?"GLo":""));}while(0);}l436 = &l433[l116];if(
PIVOT_BUCKET_IPV6(l435) == L3_DEFIP_MODE_128){l436->v6_128+= 
PIVOT_BUCKET_COUNT(l435);l436->l410+= l174.v6_128;}else if(PIVOT_BUCKET_IPV6(
l435) == L3_DEFIP_MODE_64){l436->v6_64+= PIVOT_BUCKET_COUNT(l435);l436->l409
+= l174.v6_64;}else{l436->v4+= PIVOT_BUCKET_COUNT(l435);l436->l408+= l174.v4;
}l436->l221 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l380 == 0){
LOG_CLI((BSL_META_U(l74,"%9s\n"),"Specific VRF not found"));}}sal_memset(&
l434,0,sizeof(l434));l380 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){
LOG_CLI((BSL_META_U(l74,"\n      VRF  v4      v6-64   v6-128  |   Total\n")))
;LOG_CLI((BSL_META_U(l74,"-----------------------------------------------\n")
));for(l167 = 0;l167<MAX_VRF_ID;l167++){l411*l436;if(l433[l167].l221!= TRUE){
continue;}if(l162!= -1&&l162!= l167){continue;}l380 = 1;l436 = &l433[l167];do
{(&l434)->v4+= (l436)->v4;(&l434)->l408+= (l436)->l408;(&l434)->v6_64+= (l436
)->v6_64;(&l434)->l409+= (l436)->l409;(&l434)->v6_128+= (l436)->v6_128;(&l434
)->l410+= (l436)->l410;}while(0);do{LOG_CLI((BSL_META_U(l74,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l167),(l436
->l408)?(l436->v4)*100/(l436->l408):0,(l436->l408)?(l436->v4)*1000/(l436->
l408)%10:0,(l436->l409)?(l436->v6_64)*100/(l436->l409):0,(l436->l409)?(l436->
v6_64)*1000/(l436->l409)%10:0,(l436->l410)?(l436->v6_128)*100/(l436->l410):0,
(l436->l410)?(l436->v6_128)*1000/(l436->l410)%10:0,((l436->l408+l436->l409+
l436->l410))?((l436->v4+l436->v6_64+l436->v6_128))*100/((l436->l408+l436->
l409+l436->l410)):0,((l436->l408+l436->l409+l436->l410))?((l436->v4+l436->
v6_64+l436->v6_128))*1000/((l436->l408+l436->l409+l436->l410))%10:0,(l167) == 
SOC_VRF_MAX(l74)+1?"GLo":""));}while(0);}if(l380 == 0){LOG_CLI((BSL_META_U(
l74,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l74,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l74,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l434)->l408)?((&l434)->v4)*100/((&l434)->l408):0,((&l434)->l408)?((&l434)->v4
)*1000/((&l434)->l408)%10:0,((&l434)->l409)?((&l434)->v6_64)*100/((&l434)->
l409):0,((&l434)->l409)?((&l434)->v6_64)*1000/((&l434)->l409)%10:0,((&l434)->
l410)?((&l434)->v6_128)*100/((&l434)->l410):0,((&l434)->l410)?((&l434)->
v6_128)*1000/((&l434)->l410)%10:0,(((&l434)->l408+(&l434)->l409+(&l434)->l410
))?(((&l434)->v4+(&l434)->v6_64+(&l434)->v6_128))*100/(((&l434)->l408+(&l434)
->l409+(&l434)->l410)):0,(((&l434)->l408+(&l434)->l409+(&l434)->l410))?(((&
l434)->v4+(&l434)->v6_64+(&l434)->v6_128))*1000/(((&l434)->l408+(&l434)->l409
+(&l434)->l410))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l422(l74,l162,alpm_vrf_handle[l74],l416);}if(flags&
SOC_ALPM_DEBUG_SHOW_FLAG_CNT){l422(l74,l162,alpm_vrf_handle[l74],l413);l422(
l74,l162,alpm_vrf_handle[l74],l414);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG
){l422(l74,l162,alpm_vrf_handle[l74],l415);l422(l74,l162,alpm_vrf_handle[l74]
,l418);l422(l74,l162,alpm_vrf_handle[l74],l417);}sal_free(l433);return
SOC_E_NONE;}int soc_th_alpm_bucket_sanity_check(int l74,soc_mem_t l179,int
index){int l145 = SOC_E_NONE;int l167,l303,l311,l99,tcam_index = -1;int l162 = 
0,l116;uint32 l147 = 0;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];uint32 l437[
SOC_MAX_MEM_FIELD_WORDS];int l438,l439,l440;defip_entry_t lpm_entry;int l316,
l441;soc_mem_t l111;int l218,l308,l442,bkt_ptr,l142;int l443 = 0;soc_field_t
l444[2] = {VALID0f,VALID1f};soc_field_t l445[2] = {GLOBAL_HIGH0f,
GLOBAL_HIGH1f};soc_field_t l446[2] = {ALG_BKT_PTR0f,ALG_BKT_PTR1f};
soc_field_t l447[2] = {ALG_SUB_BKT_PTR0f,ALG_SUB_BKT_PTR1f};l316 = 
soc_mem_index_min(l74,l179);l441 = soc_mem_index_max(l74,l179);if((index>= 0)
&&(index<l316||index>l441)){return SOC_E_PARAM;}else if(index>= 0){l316 = 
index;l441 = index;}SOC_ALPM_LPM_LOCK(l74);for(l167 = l316;l167<= l441;l167++
){SOC_ALPM_LPM_UNLOCK(l74);SOC_ALPM_LPM_LOCK(l74);l145 = soc_mem_read(l74,
l179,MEM_BLOCK_ANY,l167,(void*)l75);if(SOC_FAILURE(l145)){continue;}l99 = 
soc_mem_field32_get(l74,l179,(void*)l75,MODE0f);if(l99){l308 = 1;l111 = 
L3_DEFIP_ALPM_IPV6_64m;l442 = 16;}else{l308 = 2;l111 = L3_DEFIP_ALPM_IPV4m;
l442 = 24;}for(l218 = 0;l218<l308;l218++){if(soc_mem_field32_get(l74,l179,(
void*)l75,l444[l218]) == 0||soc_mem_field32_get(l74,l179,(void*)l75,l445[l218
]) == 1){continue;}bkt_ptr = soc_mem_field32_get(l74,l179,(void*)l75,l446[
l218]);l142 = soc_mem_field32_get(l74,l179,(void*)l75,l447[l218]);if(l218 == 
1){soc_th_alpm_lpm_ip4entry1_to_0(l74,l75,l437,PRESERVE_HIT);}else{
soc_th_alpm_lpm_ip4entry0_to_0(l74,l75,l437,PRESERVE_HIT);}l145 = 
soc_th_alpm_lpm_vrf_get(l74,l437,&l162,&l116);if(SOC_FAILURE(l145)){continue;
}if(SOC_TH_ALPM_SCALE_CHECK(l74,l99)){l442<<= 1;}tcam_index = -1;for(l303 = 0
;l303<l442;l303++){l145 = _soc_th_alpm_mem_index(l74,l111,bkt_ptr,l303,l147,&
l311);if(SOC_FAILURE(l145)){continue;}l145 = soc_mem_read(l74,l111,
MEM_BLOCK_ANY,l311,(void*)l75);if(SOC_FAILURE(l145)){break;}if(!
soc_mem_field32_get(l74,l111,(void*)l75,VALIDf)){continue;}if(l142!= 
soc_mem_field32_get(l74,l111,(void*)l75,SUB_BKT_PTRf)){continue;}l145 = l115(
l74,(void*)l75,l111,l99,l162,ALPM_BKTID(bkt_ptr,l142),0,&lpm_entry);if(
SOC_FAILURE(l145)){continue;}l145 = _soc_th_alpm_find(l74,(void*)&lpm_entry,
l111,(void*)l75,&l438,&l439,&l440,FALSE);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"\tLaunched AUX operation for "
"index %d bucket [%d,%d] sanity check failed\n"),l167,bkt_ptr,l142));l443++;
continue;}if(l439!= ALPM_BKTID(bkt_ptr,l142)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"\tEntry at index %d does not belong "
"to bucket [%d,%d](from bucket [%d,%d])\n"),l440,bkt_ptr,l142,ALPM_BKT_IDX(
l439),ALPM_BKT_SIDX(l439)));l443++;}if(tcam_index == -1){tcam_index = l438;
continue;}if(tcam_index!= l438){int l448,l449;l448 = soc_th_alpm_logical_idx(
l74,l179,tcam_index>>1,1);l449 = soc_th_alpm_logical_idx(l74,l179,l438>>1,1);
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tAliased bucket [%d,%d](returned "
"bucket [%d,%d]) found from TCAM1 %d and TCAM2 %d\n"),bkt_ptr,l142,
ALPM_BKT_IDX(l439),ALPM_BKT_SIDX(l439),l448,l449));l443++;}}}}
SOC_ALPM_LPM_UNLOCK(l74);if(l443 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"\tMemory %s index %d Bucket sanity check passed\n"),SOC_MEM_NAME(l74,
l179),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l74,l179),index,l443));return
SOC_E_FAIL;}int soc_th_alpm_pivot_sanity_check(int l74,soc_mem_t l179,int
index){int l167,l218,l221 = 0;int l311,l316,l441;int l145 = SOC_E_NONE;int*
l450 = NULL;int l438,l439,l440;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int bkt_ptr
,l142,l99 = 0,l308 = 2;int l451,l452[2];soc_mem_t l453,l454;int l443 = 0;int
l116,l132;soc_field_t l444[2] = {VALID0f,VALID1f};soc_field_t l445[2] = {
GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l455[2] = {NEXT_HOP_INDEX0f,
NEXT_HOP_INDEX1f};soc_field_t l446[2] = {ALG_BKT_PTR0f,ALG_BKT_PTR1f};
soc_field_t l447[2] = {ALG_SUB_BKT_PTR0f,ALG_SUB_BKT_PTR1f};l316 = 
soc_mem_index_min(l74,l179);l441 = soc_mem_index_max(l74,l179);if((index>= 0)
&&(index<l316||index>l441)){return SOC_E_PARAM;}else if(index>= 0){l316 = 
index;l441 = index;}l167 = sizeof(int)*SOC_TH_MAX_ALPM_BUCKETS*
SOC_TH_MAX_SUB_BUCKETS;l450 = sal_alloc(l167,"Bucket index array");if(l450 == 
NULL){l145 = SOC_E_MEMORY;return l145;}sal_memset(l450,0xff,l167);
SOC_ALPM_LPM_LOCK(l74);for(l167 = l316;l167<= l441;l167++){
SOC_ALPM_LPM_UNLOCK(l74);SOC_ALPM_LPM_LOCK(l74);l145 = soc_mem_read(l74,l179,
MEM_BLOCK_ANY,l167,(void*)l75);if(SOC_FAILURE(l145)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"\tRead memory %s index %d (original) "
"return %d\n"),SOC_MEM_NAME(l74,l179),l167,l145));l443++;continue;}l99 = 
soc_mem_field32_get(l74,l179,(void*)l75,MODE0f);if(l99){l308 = 1;l454 = 
L3_DEFIP_ALPM_IPV6_64m;}else{l308 = 2;l454 = L3_DEFIP_ALPM_IPV4m;}for(l218 = 
0;l218<l308;l218++){if(soc_mem_field32_get(l74,l179,(void*)l75,l444[l218]) == 
0||soc_mem_field32_get(l74,l179,(void*)l75,l445[l218]) == 1){continue;}
bkt_ptr = soc_mem_field32_get(l74,l179,(void*)l75,l446[l218]);l142 = 
soc_mem_field32_get(l74,l179,(void*)l75,l447[l218]);(void)
soc_th_alpm_lpm_vrf_get(l74,l75,&l116,&l132);l145 = 
soc_th_alpm_bucket_is_assigned(l74,bkt_ptr,l132,l99,&l221);if(l145 == 
SOC_E_PARAM||l142>= SOC_TH_MAX_SUB_BUCKETS){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"\tInvalid bucket pointer [%d,%d] "
"detected, in memory %s index %d\n"),bkt_ptr,l142,SOC_MEM_NAME(l74,l179),l167
));l443++;continue;}if(bkt_ptr!= 0){if(l450[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+
l142] == -1){l450[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l142] = l167;}else{LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"\tDuplicated bucket pointer "
"[%d,%d] detected, in memory %s index1 %d ""and index2 %d\n"),bkt_ptr,l142,
SOC_MEM_NAME(l74,l179),l450[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l142],l167));l443
++;continue;}}else{continue;}if(l145>= 0&&l221 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),bkt_ptr,SOC_MEM_NAME(l74,l179),l167));
l443++;continue;}l453 = _soc_tomahawk_alpm_bkt_view_get(l74,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,ALPM_BKTID(bkt_ptr,0)));if(l454!= l453){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual"" %s\n"),bkt_ptr,SOC_MEM_NAME(l74,
l454),SOC_MEM_NAME(l74,l453)));l443++;continue;}l451 = soc_mem_field32_get(
l74,l179,(void*)l75,l455[l218]);if(l218 == 1){l145 = 
soc_th_alpm_lpm_ip4entry1_to_0(l74,l75,l75,PRESERVE_HIT);if(SOC_FAILURE(l145)
){continue;}}l438 = -1;l145 = _soc_th_alpm_find(l74,l75,l454,(void*)l75,&l438
,&l439,&l440,FALSE);if(l438 == -1){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tLaunched AUX operation for PIVOT ""index %d sanity check failed\n"),l167))
;l443++;continue;}l311 = soc_th_alpm_logical_idx(l74,l179,l438>>1,1);l145 = 
soc_mem_read(l74,l179,MEM_BLOCK_ANY,l311,(void*)l75);if(SOC_FAILURE(l145)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tRead memory %s index %d (nexthop) ""return %d\n"),SOC_MEM_NAME(l74,l179),
l311,l145));l443++;continue;}l452[0] = soc_mem_field32_get(l74,l179,(void*)
l75,l455[0]);l452[1] = soc_mem_field32_get(l74,l179,(void*)l75,l455[1]);if(
l451!= l452[l438&1]){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d entry1 %d, index2 %d ""entry2 %d\n"),l167,l218,
l311,l438&1));l443++;continue;}}}SOC_ALPM_LPM_UNLOCK(l74);sal_free(l450);if(
l443 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tMemory %s index %d Pivot sanity check passed\n"),SOC_MEM_NAME(l74,l179),
index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tMemory %s index %d Pivot sanity check failed, ""encountered %d error(s)\n"
),SOC_MEM_NAME(l74,l179),index,l443));return SOC_E_FAIL;}

#endif

#endif /* ALPM_ENABLE */
