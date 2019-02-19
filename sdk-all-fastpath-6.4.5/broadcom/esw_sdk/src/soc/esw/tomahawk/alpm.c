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
[SOC_MAX_NUM_DEVICES][SOC_TH_MAX_ALPM_VIEWS];int alpm_split_count = 0;int
alpm_merge_count = 0;extern int alpm_128_split_count;static void l73(int l74,
void*l75,int index,l68 l76);static uint16 l77(uint8*l78,int l79);static int
l80(int unit,int l63,int l64,l67**l81);static int l82(l67*l83);static int l84
(l67*l85,l69 l86,l68 entry,int l87,uint16*l88);static int l89(l67*l85,l69 l86
,l68 entry,int l87,uint16 l90,uint16 l91);static int l92(l67*l85,l69 l86,l68
entry,int l87,uint16 l93);static int l94(int l74);static int l95(int l74);
static int l96(int l74,void*l97,int*index);static int soc_th_alpm_lpm_delete(
int l74,void*key_data);static int l98(int l74,void*key_data,int l99,int l100,
int l101,defip_aux_scratch_entry_t*l102);static int l103(int l74,void*
key_data,void*l75,int*l104,int*l105,int*l99);static int l106(int l74,void*
key_data,void*l75,int*l104);static int l107(int l74);static int l108(int unit
,void*lpm_entry,void*l109,void*l110,soc_mem_t l111,uint32 l112,uint32*l113,
int l114);static int l115(int unit,void*l109,soc_mem_t l111,int l99,int l116,
int l117,int index,void*lpm_entry);static int l118(int unit,uint32*key,int len
,int l116,int l99,defip_entry_t*lpm_entry,int l119,int l120);static int l121(
int l74,int l116,int l122);static int l123(int l74,alpm_pfx_info_t*l124,
trie_t*l125,uint32*l126,uint32 l127,trie_node_t*,defip_entry_t*lpm_entry,
uint32*l128);static int l129(int l74,alpm_pfx_info_t*l124,int*l130,int*l104);
static int _soc_th_alpm_move_inval(int l74,soc_mem_t l111,int l122,
alpm_mem_prefix_array_t*l131,int*l91);int soc_th_alpm_lpm_vrf_get(int unit,
void*lpm_entry,int*l116,int*l132);int _soc_th_alpm_rollback_bkt_move(int l74,
void*key_data,soc_mem_t l111,alpm_pivot_t*l133,alpm_pivot_t*l134,
alpm_mem_prefix_array_t*l135,int*l91,int l136);int soc_th_alpm_lpm_delete(int
l74,void*key_data);void _soc_th_alpm_rollback_pivot_add(int l74,defip_entry_t
*l137,void*key_data,int tcam_index,alpm_pivot_t*pivot_pyld);extern
alpm_vrf_handle_t*alpm_vrf_handle[SOC_MAX_NUM_DEVICES];extern alpm_pivot_t**
tcam_pivot[SOC_MAX_NUM_DEVICES];extern int soc_tomahawk_alpm_mode_get(int unit
);extern int soc_th_get_alpm_banks(int unit);extern int
_alpm_128_prefix_create(int l74,void*entry,uint32*l138,uint32*l105,int*l113);
void soc_th_alpm_lpm_move_bu_upd(int l74,int l114,int l139){alpm_bkt_usg_t*
l140;int bkt_ptr = ALPM_BKT_IDX(l114);int l141 = ALPM_BKT_SIDX(l114);l140 = &
bkt_usage[l74][bkt_ptr];l140->pivots[l141] = l139;}int soc_th_alpm_mode_get(
int l74){int l142;l142 = soc_tomahawk_alpm_mode_get(l74);switch(l142){case 1:
return SOC_ALPM_MODE_PARALLEL;case 3:return SOC_ALPM_MODE_TCAM_ALPM;case 2:
default:return SOC_ALPM_MODE_COMBINED;}return SOC_ALPM_MODE_COMBINED;}static
int l143(int l74,const void*entry,int*l87){int l144;uint32 l145;int l99;l99 = 
soc_mem_field32_get(l74,L3_DEFIPm,entry,MODE0f);if(l99){l145 = 
soc_mem_field32_get(l74,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l144 = 
_ipmask2pfx(l145,l87))<0){return(l144);}l145 = soc_mem_field32_get(l74,
L3_DEFIPm,entry,IP_ADDR_MASK1f);if(*l87){if(l145!= 0xffffffff){return(
SOC_E_PARAM);}*l87+= 32;}else{if((l144 = _ipmask2pfx(l145,l87))<0){return(
l144);}}}else{l145 = soc_mem_field32_get(l74,L3_DEFIPm,entry,IP_ADDR_MASK0f);
if((l144 = _ipmask2pfx(l145,l87))<0){return(l144);}}return SOC_E_NONE;}void
soc_th_alpm_bank_db_type_get(int l74,int l116,uint32*l146,uint32*l100){if(
l116 == SOC_VRF_MAX(l74)+1){if(l100){*l100 = 0;}if(soc_th_get_alpm_banks(l74)
<= 2){SOC_ALPM_TH_GET_2BANKMODE_GLOBAL_BANK_DISABLE(l74,*l146);}else{
SOC_ALPM_TH_GET_GLOBAL_BANK_DISABLE(l74,*l146);}}else{if(l100){*l100 = 2;}if(
soc_th_get_alpm_banks(l74)<= 2){SOC_ALPM_TH_GET_2BANKMODE_VRF_BANK_DISABLE(
l74,*l146);}else{SOC_ALPM_TH_GET_VRF_BANK_DISABLE(l74,*l146);}}}int
_soc_th_alpm_rpf_entry(int l74,int l147){int l148;int l149 = 
soc_th_get_alpm_banks(l74)/2;l148 = (l147>>l149)&SOC_TH_ALPM_BKT_MASK;l148+= 
SOC_TH_ALPM_BUCKET_COUNT(l74);return(l147&~(SOC_TH_ALPM_BKT_MASK<<l149))|(
l148<<l149);}int soc_th_alpm_physical_idx(int l74,soc_mem_t l111,int index,
int l150){int l151 = index&1;if(l150){return soc_trident2_l3_defip_index_map(
l74,l111,index);}index>>= 1;index = soc_trident2_l3_defip_index_map(l74,l111,
index);index<<= 1;index|= l151;return index;}int soc_th_alpm_logical_idx(int
l74,soc_mem_t l111,int index,int l150){int l151 = index&1;if(l150){return
soc_trident2_l3_defip_index_remap(l74,l111,index);}index>>= 1;index = 
soc_trident2_l3_defip_index_remap(l74,l111,index);index<<= 1;index|= l151;
return index;}static int l152(int l74,void*entry,uint32*prefix,uint32*l105,
int*l113){int l153,l154,l99;int l87 = 0;int l144 = SOC_E_NONE;uint32 l155,
l151;prefix[0] = prefix[1] = prefix[2] = prefix[3] = prefix[4] = 0;l99 = 
soc_mem_field32_get(l74,L3_DEFIPm,entry,MODE0f);l153 = soc_mem_field32_get(
l74,L3_DEFIPm,entry,IP_ADDR0f);l154 = soc_mem_field32_get(l74,L3_DEFIPm,entry
,IP_ADDR_MASK0f);prefix[1] = l153;l153 = soc_mem_field32_get(l74,L3_DEFIPm,
entry,IP_ADDR1f);l154 = soc_mem_field32_get(l74,L3_DEFIPm,entry,
IP_ADDR_MASK1f);prefix[0] = l153;if(l99){prefix[4] = prefix[1];prefix[3] = 
prefix[0];prefix[1] = prefix[0] = 0;l154 = soc_mem_field32_get(l74,L3_DEFIPm,
entry,IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l154,&l87))<0){return(l144);}
l154 = soc_mem_field32_get(l74,L3_DEFIPm,entry,IP_ADDR_MASK1f);if(l87){if(
l154!= 0xffffffff){return(SOC_E_PARAM);}l87+= 32;}else{if((l144 = _ipmask2pfx
(l154,&l87))<0){return(l144);}}l155 = 64-l87;if(l155<32){prefix[4]>>= l155;
l151 = (((32-l155) == 32)?0:(prefix[3])<<(32-l155));prefix[3]>>= l155;prefix[
4]|= l151;}else{prefix[4] = (((l155-32) == 32)?0:(prefix[3])>>(l155-32));
prefix[3] = 0;}}else{l154 = soc_mem_field32_get(l74,L3_DEFIPm,entry,
IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l154,&l87))<0){return(l144);}prefix[1]
= (((32-l87) == 32)?0:(prefix[1])>>(32-l87));prefix[0] = 0;}*l105 = l87;*l113
= (prefix[0] == 0)&&(prefix[1] == 0)&&(l87 == 0);return SOC_E_NONE;}int
_soc_th_alpm_find_in_bkt(int l74,soc_mem_t l111,int bktid,int l146,uint32*l75
,void*alpm_data,int*l88,int l122){int l144;l144 = soc_mem_alpm_lookup(l74,
l111,ALPM_BKT_IDX(bktid),MEM_BLOCK_ANY,l146,l75,alpm_data,l88);if(SOC_SUCCESS
(l144)){return l144;}if(SOC_TH_ALPM_SCALE_CHECK(l74,l122)){return
soc_mem_alpm_lookup(l74,l111,ALPM_BKT_IDX(bktid)+1,MEM_BLOCK_ANY,l146,l75,
alpm_data,l88);}return l144;}static int l156(int l74,uint32*prefix,uint32 l127
,int l122,int l116,int*l157,int*tcam_index,int*bktid){int l144 = SOC_E_NONE;
trie_t*l158;trie_node_t*l159 = NULL;alpm_pivot_t*pivot_pyld;if(l122){l158 = 
VRF_PIVOT_TRIE_IPV6(l74,l116);}else{l158 = VRF_PIVOT_TRIE_IPV4(l74,l116);}
l144 = trie_find_lpm(l158,prefix,l127,&l159);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"Pivot find failed\n")));return l144;}
pivot_pyld = (alpm_pivot_t*)l159;*l157 = 1;*tcam_index = PIVOT_TCAM_INDEX(
pivot_pyld);*bktid = PIVOT_BUCKET_INDEX(pivot_pyld);return SOC_E_NONE;}static
int _soc_th_alpm_find(int l74,void*key_data,soc_mem_t l111,void*alpm_data,int
*tcam_index,int*bktid,int*l104,int l160){uint32 l75[SOC_MAX_MEM_FIELD_WORDS];
int l161,l116,l122;int l88;uint32 l100,l146;int l144 = SOC_E_NONE;int l157 = 
0;l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);if(l122){if(!(
l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l161
,&l116));if(l161 == 0){if(soc_th_alpm_mode_get(l74) == SOC_ALPM_MODE_PARALLEL
||soc_th_alpm_mode_get(l74) == SOC_ALPM_MODE_TCAM_ALPM){return SOC_E_PARAM;}}
soc_th_alpm_bank_db_type_get(l74,l116,&l146,&l100);if(!(ALPM_PREFIX_IN_TCAM(
l74,l161))){if(l160){uint32 prefix[5],l127;int l113 = 0;l144 = l152(l74,
key_data,prefix,&l127,&l113);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l74,"_soc_alpm_insert: prefix create failed\n")));return l144;}
SOC_IF_ERROR_RETURN(l156(l74,prefix,l127,l122,l116,&l157,tcam_index,bktid));}
else{defip_aux_scratch_entry_t l102;sal_memset(&l102,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l98(l74,key_data,l122,l100,0,
&l102));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l74,PREFIX_LOOKUP,&l102,TRUE,
&l157,tcam_index,bktid));}if(l157){l108(l74,key_data,l75,0,l111,0,0,*bktid);
l144 = _soc_th_alpm_find_in_bkt(l74,l111,*bktid,l146,l75,alpm_data,&l88,l122)
;if(SOC_SUCCESS(l144)){*l104 = l88;}}else{l144 = SOC_E_NOT_FOUND;}}return l144
;}static int l162(int l74,void*key_data,void*alpm_data,void*alpm_sip_data,
soc_mem_t l111,int l88){defip_aux_scratch_entry_t l102;int l161,l122,l116;int
bktid;uint32 l100,l146;int l144 = SOC_E_NONE;int l157 = 0,l151 = 0;int
tcam_index,index;uint32 l163[SOC_MAX_MEM_FIELD_WORDS];l122 = 
soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);if(l122){if(!(l122 = 
soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l161,&l116));
soc_th_alpm_bank_db_type_get(l74,l116,&l146,&l100);if(soc_th_alpm_mode_get(
l74) == SOC_ALPM_MODE_COMBINED||soc_th_alpm_mode_get(l74) == 
SOC_ALPM_MODE_TCAM_ALPM){l100 = 2;}if(!(ALPM_PREFIX_IN_TCAM(l74,l161))){
sal_memset(&l102,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l98
(l74,key_data,l122,l100,0,&l102));SOC_ALPM_LPM_LOCK(l74);l144 = 
_soc_th_alpm_find(l74,key_data,l111,l163,&tcam_index,&bktid,&index,TRUE);
SOC_ALPM_LPM_UNLOCK(l74);SOC_IF_ERROR_RETURN(l144);soc_mem_field32_set(l74,
l111,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));SOC_IF_ERROR_RETURN(
soc_mem_write(l74,l111,MEM_BLOCK_ANY,ALPM_ENT_INDEX(l88),alpm_data));if(
SOC_URPF_STATUS_GET(l74)){soc_mem_field32_set(l74,l111,alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));SOC_IF_ERROR_RETURN(soc_mem_write(l74,l111
,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,ALPM_ENT_INDEX(l88)),alpm_sip_data)
);if(l144!= SOC_E_NONE){return SOC_E_FAIL;}}l151 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,IP_LENGTHf);soc_mem_field32_set(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,REPLACE_LENf,l151);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,DELETE_PROPAGATE,&l102,TRUE,&l157,&tcam_index,&bktid)
);if(SOC_URPF_STATUS_GET(l74)){if(l151 == 0){soc_mem_field32_set(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,RPEf,1);}else{soc_mem_field32_set(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,RPEf,0);}l151 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf);l151+= 1;soc_mem_field32_set(l74,
L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf,l151);SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,DELETE_PROPAGATE,&l102,TRUE,&l157,&tcam_index,&bktid)
);}}return l144;}int soc_th_alpm_update_hit_bits(int l74,int count,int*l164,
int*l165){int l144 = SOC_E_NONE;int l166,l167,l168,index;soc_mem_t l111 = 
L3_DEFIP_AUX_HITBIT_UPDATEm;defip_aux_hitbit_update_entry_t l169;int l170 = 
soc_mem_index_count(l74,l111);l168 = (count+l170-1)/l170;for(l167 = 0;l167<
l168;l167++){for(l166 = l167*l170;l166<((count>(l167+1)*l170)?(l167+1)*l170:
count);l166++){index = l166%l170;if(l164[l166]<= 0){continue;}
SOC_IF_ERROR_RETURN(soc_mem_read(l74,l111,MEM_BLOCK_ANY,index,&l169));
soc_mem_field32_set(l74,l111,&l169,HITBIT_PTRf,l164[l166]);
soc_mem_field32_set(l74,l111,&l169,REPLACEMENT_PTRf,l165[l166]);
soc_mem_field32_set(l74,l111,&l169,VALIDf,1);SOC_IF_ERROR_RETURN(
soc_mem_write(l74,l111,MEM_BLOCK_ANY,index,&l169));}SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,HITBIT_REPLACE,NULL,FALSE,NULL,NULL,NULL));}return
l144;}int _soc_th_alpm_mem_prefix_array_cb(trie_node_t*node,void*l171){
alpm_mem_prefix_array_t*l172 = (alpm_mem_prefix_array_t*)l171;if(node->type == 
PAYLOAD){l172->prefix[l172->count] = (payload_t*)node;l172->count++;}return
SOC_E_NONE;}int _soc_th_alpm_bkt_entry_cnt(int l74,int l122){int l173 = 0;
switch(l122){case L3_DEFIP_MODE_V4:l173 = ALPM_IPV4_BKT_COUNT;break;case
L3_DEFIP_MODE_64:l173 = ALPM_IPV6_64_BKT_COUNT;break;case L3_DEFIP_MODE_128:
l173 = ALPM_IPV6_128_BKT_COUNT;break;default:l173 = ALPM_IPV4_BKT_COUNT;break
;}if(SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l173<<= 1;}if(soc_th_get_alpm_banks(
l74)<= 2){l173>>= 1;}if(soc_th_alpm_mode_get(l74) == SOC_ALPM_MODE_PARALLEL&&
SOC_URPF_STATUS_GET(l74)){l173>>= 1;}return l173;}static int l174(int l74,int
l2,alpm_bkt_bmp_t*l175){int l166;for(l166 = l2+1;l166<SOC_TH_MAX_ALPM_BUCKETS
;l166++){if(SHR_BITGET(l175->bkt_bmp,l166)){return l166;}}return-1;}static int
l176(int l74,int l122,int l139,int bktid){int l144 = SOC_E_NONE;int l177;
soc_mem_t l178 = L3_DEFIPm;defip_entry_t lpm_entry;int l179,l180;l179 = 
ALPM_BKT_IDX(bktid);l180 = ALPM_BKT_SIDX(bktid);l177 = 
soc_th_alpm_logical_idx(l74,l178,l139>>1,1);l144 = soc_mem_read(l74,l178,
MEM_BLOCK_ANY,l177,&lpm_entry);SOC_IF_ERROR_RETURN(l144);if((!l122)&&(l139&1)
){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(&
lpm_entry),(l61[(l74)]->l50),(l179));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(&lpm_entry),(l61[(l74)]->l52),(l180));}else{
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(&lpm_entry)
,(l61[(l74)]->l49),(l179));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(&lpm_entry),(l61[(l74)]->l51),(l180));}l144 = soc_mem_write(
l74,l178,MEM_BLOCK_ANY,l177,&lpm_entry);SOC_IF_ERROR_RETURN(l144);if(
SOC_URPF_STATUS_GET(l74)){int l181 = soc_th_alpm_logical_idx(l74,l178,l139>>1
,1)+(soc_mem_index_count(l74,l178)>>1);l144 = READ_L3_DEFIPm(l74,
MEM_BLOCK_ANY,l181,&lpm_entry);SOC_IF_ERROR_RETURN(l144);if((!l122)&&(l181&1)
){soc_mem_field32_set(l74,l178,&lpm_entry,ALG_BKT_PTR1f,l179+
SOC_TH_ALPM_BUCKET_COUNT(l74));soc_mem_field32_set(l74,l178,&lpm_entry,
ALG_SUB_BKT_PTR1f,l180);}else{soc_mem_field32_set(l74,l178,&lpm_entry,
ALG_BKT_PTR0f,l179+SOC_TH_ALPM_BUCKET_COUNT(l74));soc_mem_field32_set(l74,
l178,&lpm_entry,ALG_SUB_BKT_PTR0f,l180);}l144 = WRITE_L3_DEFIPm(l74,
MEM_BLOCK_ANY,l181,&lpm_entry);SOC_IF_ERROR_RETURN(l144);}return l144;}static
int _soc_th_alpm_move_trie(int l74,int l122,int l182,int l183){int*l91 = NULL
,*l90 = NULL;int l144 = SOC_E_NONE,l184,l166,l185;int l88,l139;uint32 l146 = 
0;soc_mem_t l111;void*l186,*l187;trie_t*trie = NULL;payload_t*l87 = NULL;
alpm_mem_prefix_array_t*l131 = NULL;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];
defip_alpm_ipv4_entry_t l188,l189;defip_alpm_ipv6_64_entry_t l190,l191;int
l192,l193,l194,l195;l185 = sizeof(int)*MAX_PREFIX_PER_BUCKET;l91 = sal_alloc(
l185,"new_index_move");l90 = sal_alloc(l185,"old_index_move");l131 = 
sal_alloc(sizeof(alpm_mem_prefix_array_t),"prefix_array");if(l91 == NULL||l90
== NULL||l131 == NULL){l144 = SOC_E_MEMORY;goto l196;}l192 = ALPM_BKT_IDX(
l182);l194 = ALPM_BKT_SIDX(l182);l193 = ALPM_BKT_IDX(l183);l195 = 
ALPM_BKT_SIDX(l183);l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;
l186 = ((l122)?((uint32*)&(l190)):((uint32*)&(l188)));l187 = ((l122)?((uint32
*)&(l191)):((uint32*)&(l189)));l139 = SOC_ALPM_BS_BKT_USAGE_PIVOT(l74,l182);
trie = PIVOT_BUCKET_TRIE(ALPM_TCAM_PIVOT(l74,l139));
soc_th_alpm_bank_db_type_get(l74,SOC_ALPM_BS_BKT_USAGE_VRF(l74,l182),&l146,
NULL);sal_memset(l131,0,sizeof(*l131));l144 = trie_traverse(trie,
_soc_th_alpm_mem_prefix_array_cb,l131,_TRIE_INORDER_TRAVERSE);do{if((l144)<0)
{goto l196;}}while(0);sal_memset(l91,-1,l185);sal_memset(l90,-1,l185);for(
l166 = 0;l166<l131->count;l166++){l87 = l131->prefix[l166];if(l87->index>0){
l144 = soc_mem_read(l74,l111,MEM_BLOCK_ANY,l87->index,l186);if(SOC_FAILURE(
l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_mem_read index %d failed\n"),l87->index));goto l197;}soc_mem_field32_set
(l74,l111,l186,SUB_BKT_PTRf,l195);if(SOC_URPF_STATUS_GET(l74)){l144 = 
soc_mem_read(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l87->index),
l187);if(SOC_FAILURE(l144)){goto l197;}soc_mem_field32_set(l74,l111,l187,
SUB_BKT_PTRf,l195);}l144 = _soc_th_alpm_insert_in_bkt(l74,l111,l183,l146,l186
,l75,&l88,l122);if(SOC_SUCCESS(l144)){if(SOC_URPF_STATUS_GET(l74)){l144 = 
soc_mem_write(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),l187);if
(SOC_FAILURE(l144)){goto l197;}}}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"insert to bucket %d failed\n"),l183));goto l197;}l91[l166] = l88;l90[
l166] = l87->index;}}l144 = l176(l74,l122,l139,l183);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_move_relink failed, pivot %d bkt %d\n"),l139,l183));goto l197;}
l144 = _soc_th_alpm_move_inval(l74,l111,l122,l131,l91);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_th_alpm_move_inval failed\n")
));goto l197;}l144 = soc_th_alpm_update_hit_bits(l74,l131->count,l90,l91);if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l131->count));l144 = 
SOC_E_NONE;}PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l74,l139)) = ALPM_BKTID(l193,
l195);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l74,l122,l192);
SOC_ALPM_BS_BKT_USAGE_SB_DEL(l74,l192,l194,TRUE,l131->count);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l74,l122,l192);
SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l74,l122,l193);SOC_ALPM_BS_BKT_USAGE_SB_ADD(
l74,l193,l195,l139,-1,l131->count);SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l74,l122,
l193);l197:if(SOC_FAILURE(l144)){l184 = l176(l74,l122,l139,l182);do{if((l184)
<0){goto l196;}}while(0);for(l166 = 0;l166<l131->count;l166++){if(l91[l166] == 
-1){continue;}l184 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,l91[l166],
soc_mem_entry_null(l74,l111));do{if((l184)<0){goto l196;}}while(0);if(
SOC_URPF_STATUS_GET(l74)){l184 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l74,l91[l166]),soc_mem_entry_null(l74,l111));do{if((
l184)<0){goto l196;}}while(0);}}}l196:if(l91!= NULL){sal_free(l91);}if(l90!= 
NULL){sal_free(l90);}if(l131!= NULL){sal_free(l131);}return l144;}int
soc_th_alpm_bs_merge(int l74,int l122,int l198,int l199){int l144 = 
SOC_E_NONE;alpm_bkt_usg_t*l200,*l201;int l202,l203;l200 = &bkt_usage[l74][
l198];l201 = &bkt_usage[l74][l199];for(l202 = 0;l202<SOC_TH_MAX_SUB_BUCKETS;
l202++){if(!(l200->sub_bkts&(1<<l202))){continue;}for(l203 = 0;l203<
SOC_TH_MAX_SUB_BUCKETS;l203++){if((l201->sub_bkts&(1<<l203))!= 0){continue;}
if(l122 == L3_DEFIP_MODE_128){l144 = _soc_th_alpm_128_move_trie(l74,l122,
ALPM_BKTID(l198,l202),ALPM_BKTID(l199,l203));}else{l144 = 
_soc_th_alpm_move_trie(l74,l122,ALPM_BKTID(l198,l202),ALPM_BKTID(l199,l203));
}SOC_IF_ERROR_RETURN(l144);break;}}alpm_merge_count++;return SOC_E_NONE;}
static int l204(int l74,int l205,int l206){if(l205 == l206){return 1;}if(l205
== (SOC_VRF_MAX(l74)+1)||l206 == (SOC_VRF_MAX(l74)+1)){return 0;}return 1;}
static int l207(int l74,int*bktid,int l122){int l208,l209,l173;int l198,l199;
alpm_bkt_bmp_t*l210,*l211;alpm_bkt_usg_t*l200,*l201;l200 = l201 = NULL;l173 = 
_soc_th_alpm_bkt_entry_cnt(l74,l122);for(l208 = 1;l208<= (l173/2);l208++){
l210 = &global_bkt_usage[l74][l122][l208];if(l210->bkt_count<1){continue;}
l198 = l174(l74,-1,l210);if(l198 == ALPM_BKT_IDX(*bktid)){l198 = l174(l74,
l198,l210);if(l198 == -1){continue;}}l200 = &bkt_usage[l74][l198];l199 = l174
(l74,l198,l210);while(l199!= -1){l201 = &bkt_usage[l74][l199];if(!l204(l74,
l200->vrf_type[0],l201->vrf_type[0])){l199 = l174(l74,l199,l210);continue;}if
(_shr_popcount((uint32)l200->sub_bkts)+_shr_popcount((uint32)l201->sub_bkts)
<= SOC_TH_MAX_SUB_BUCKETS){break;}l199 = l174(l74,l199,l210);}if(l199!= -1){
SOC_IF_ERROR_RETURN(soc_th_alpm_bs_merge(l74,l122,l198,l199));*bktid = 
ALPM_BKTID(l198,0);return SOC_E_NONE;}for(l209 = l208+1;l209<
SOC_TH_MAX_BUCKET_ENTRIES;l209++){l211 = &global_bkt_usage[l74][l122][l209];
if(l211->bkt_count<= 0){continue;}if((l208+l209)>l173){break;}l199 = l174(l74
,-1,l211);while(l199!= -1){l201 = &bkt_usage[l74][l199];if((l199 == l198)||(!
l204(l74,l200->vrf_type[0],l201->vrf_type[0]))){l199 = l174(l74,l199,l211);
continue;}if(_shr_popcount((uint32)l200->sub_bkts)+_shr_popcount((uint32)l201
->sub_bkts)<= SOC_TH_MAX_SUB_BUCKETS){break;}l199 = l174(l74,l199,l211);}if(
l199!= -1){SOC_IF_ERROR_RETURN(soc_th_alpm_bs_merge(l74,l122,l198,l199));*
bktid = ALPM_BKTID(l198,0);return SOC_E_NONE;}}}return SOC_E_FULL;}int
soc_th_alpm_bs_alloc(int l74,int*bktid,int l116,int l122){int l144 = 
SOC_E_NONE;l144 = soc_th_alpm_bucket_assign(l74,bktid,l116,l122);if(l144!= 
SOC_E_FULL){*bktid = ALPM_BKTID(*bktid,0);return l144;}l144 = l207(l74,bktid,
l122);return l144;}int soc_th_alpm_bs_free(int l74,int bktid,int l116,int l122
){int l144 = SOC_E_NONE;alpm_bkt_usg_t*l140;int l212,l213;l212 = ALPM_BKT_IDX
(bktid);l213 = ALPM_BKT_SIDX(bktid);l140 = &bkt_usage[l74][l212];l140->
sub_bkts&= ~(1<<l213);l140->pivots[l213] = 0;if(l140->count == 0){l140->
vrf_type[l213] = 0;l144 = soc_th_alpm_bucket_release(l74,l212,l116,l122);}
return l144;}int soc_th_alpm_bu_upd(int l74,int l117,int tcam_index,int l214,
int l122,int l215){int l212,l213;l212 = ALPM_BKT_IDX(l117);l213 = 
ALPM_BKT_SIDX(l117);SOC_ALPM_BS_GLOBAL_BKT_USAGE_DEC(l74,l122,l212);if(l215>0
){SOC_ALPM_BS_BKT_USAGE_SB_ADD(l74,l212,l213,tcam_index,l214,l215);}else if(
l215<0){SOC_ALPM_BS_BKT_USAGE_SB_DEL(l74,l212,l213,l214,-l215);}
SOC_ALPM_BS_GLOBAL_BKT_USAGE_INC(l74,l122,l212);return SOC_E_NONE;}int
soc_th_alpm_bs_dump_brief(int l74,const char*l87){int l166,l216,l217,l148,
l218 = 0;int l219 = 1,l220 = 0;alpm_bkt_bmp_t*l221 = NULL;alpm_bkt_usg_t*l140
= NULL;int*l126;char*l222[] = {"IPv4","IPv6-64","IPv6-128"};l126 = sal_alloc(
sizeof(int)*16384,"pivot");if(l126 == NULL){return SOC_E_MEMORY;}sal_memset(
l126,0,sizeof(int)*16384);for(l216 = 0;l216<SOC_TH_MAX_ALPM_VIEWS;l216++){
LOG_CLI((BSL_META_U(l74,"\n[- %8s -]\n"),l222[l216]));LOG_CLI((BSL_META_U(l74
,"=================================================\n")));LOG_CLI((BSL_META_U
(l74," %5s | %5s %10s\n"),"entry","count","1st-bktbmp"));LOG_CLI((BSL_META_U(
l74,"=================================================\n")));l148 = 0;for(
l217 = 0;l217<SOC_TH_MAX_BUCKET_ENTRIES;l217++){l221 = &global_bkt_usage[l74]
[l216][l217];if(l221->bkt_count == 0){continue;}l148++;LOG_CLI((BSL_META_U(
l74," %5d   %5d"),l217,l221->bkt_count));for(l166 = 0;l166<
SOC_TH_MAX_ALPM_BUCKETS;l166++){if(SHR_BITGET(l221->bkt_bmp,l166)){LOG_CLI((
BSL_META_U(l74," 0x%08x\n"),l221->bkt_bmp[l166/SHR_BITWID]));break;}}}if(l148
== 0){LOG_CLI((BSL_META_U(l74,"- None - \n")));}}l217 = 0;LOG_CLI((BSL_META_U
(l74,"%5s | %s\n"),"bkt","(sub-bkt-idx, tcam-idx)"));LOG_CLI((BSL_META_U(l74,
"=================================================\n")));for(l148 = 0;l148<
SOC_TH_MAX_ALPM_BUCKETS;l148++){l140 = &bkt_usage[l74][l148];if(l140->count == 
0){continue;}l217++;l218 = 0;LOG_CLI((BSL_META_U(l74,"%5d | "),l148));for(
l166 = 0;l166<4;l166++){if(l140->sub_bkts&(1<<l166)){LOG_CLI((BSL_META_U(l74,
"(%d, %5d) "),l166,l140->pivots[l166]));if(l126[l140->pivots[l166]]!= 0){l218
= l166+1;}else{l126[l140->pivots[l166]] = l148;}}}LOG_CLI((BSL_META_U(l74,
"\n")));if(l218){LOG_CLI((BSL_META_U(l74,
"Error: multi-buckets were linked to pivot %d, prev %d, curr %d\n"),l140->
pivots[l218-1],l126[l140->pivots[l218-1]],l148));}}if(l217 == 0){LOG_CLI((
BSL_META_U(l74,"- None - \n")));}LOG_CLI((BSL_META_U(l74,"\n")));if(
SOC_TH_ALPM_SCALE_CHECK(l74,1)){l219 = 2;}l148 = 0;for(l166 = 0;l166<
SOC_TH_ALPM_BUCKET_COUNT(l74);l166+= l219){SHR_BITTEST_RANGE(
SOC_TH_ALPM_VRF_BUCKET_BMAP(l74),l166,l219,l220);if(l220){l148++;}}LOG_CLI((
BSL_META_U(l74,"VRF Route buckets: %5d\n"),l148));if(soc_th_alpm_mode_get(l74
) == SOC_ALPM_MODE_PARALLEL&&SOC_URPF_STATUS_GET(l74)){l148 = 0;for(l166 = 0;
l166<SOC_TH_ALPM_BUCKET_COUNT(l74);l166+= l219){SHR_BITTEST_RANGE(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l74),l166,l219,l220);if(l220){l148++;}}LOG_CLI((
BSL_META_U(l74,"Global Route buckets: %5d\n"),l148));}sal_free(l126);return
SOC_E_NONE;}static int l223(int l74,int l130,int l122,int l128){int l144,l151
,index;defip_aux_table_entry_t entry;index = l130>>(l122?0:1);l144 = 
soc_mem_read(l74,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);
SOC_IF_ERROR_RETURN(l144);if(l122){soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l128);soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l128);l151 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}else{if(l130&1){soc_mem_field32_set(
l74,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l128);l151 = soc_mem_field32_get(
l74,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f);}else{soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l128);l151 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f);}}l144 = soc_mem_write(l74,
L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,index,&entry);SOC_IF_ERROR_RETURN(l144);if(
SOC_URPF_STATUS_GET(l74)){l151++;if(l122){soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l128);soc_mem_field32_set(l74,
L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l128);}else{if(l130&1){
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH1f,l128);}else{
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,&entry,BPM_LENGTH0f,l128);}}
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE0f,l151);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,&entry,DB_TYPE1f,l151);index+= (2
*soc_mem_index_count(l74,L3_DEFIP_PAIR_128m)+soc_mem_index_count(l74,
L3_DEFIPm))/2;l144 = soc_mem_write(l74,L3_DEFIP_AUX_TABLEm,MEM_BLOCK_ANY,
index,&entry);}return l144;}static int l224(int l74,int l225,void*entry,void*
l226,int l227){uint32 l151,l154,l122,l100,l228 = 0;soc_mem_t l111 = L3_DEFIPm
;soc_mem_t l229 = L3_DEFIP_AUX_TABLEm;defip_entry_t l230;int l144 = 
SOC_E_NONE,l87,l231,l116,l206;SOC_IF_ERROR_RETURN(soc_mem_read(l74,l229,
MEM_BLOCK_ANY,l225,l226));l151 = soc_mem_field32_get(l74,l111,entry,VRF_ID_0f
);soc_mem_field32_set(l74,l229,l226,VRF0f,l151);l151 = soc_mem_field32_get(
l74,l111,entry,VRF_ID_1f);soc_mem_field32_set(l74,l229,l226,VRF1f,l151);l151 = 
soc_mem_field32_get(l74,l111,entry,MODE0f);soc_mem_field32_set(l74,l229,l226,
MODE0f,l151);l151 = soc_mem_field32_get(l74,l111,entry,MODE1f);
soc_mem_field32_set(l74,l229,l226,MODE1f,l151);l122 = l151;l151 = 
soc_mem_field32_get(l74,l111,entry,VALID0f);soc_mem_field32_set(l74,l229,l226
,VALID0f,l151);l151 = soc_mem_field32_get(l74,l111,entry,VALID1f);
soc_mem_field32_set(l74,l229,l226,VALID1f,l151);l151 = soc_mem_field32_get(
l74,l111,entry,IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l151,&l87))<0){return
l144;}l154 = soc_mem_field32_get(l74,l111,entry,IP_ADDR_MASK1f);if((l144 = 
_ipmask2pfx(l154,&l231))<0){return l144;}if(l122){soc_mem_field32_set(l74,
l229,l226,IP_LENGTH0f,l87+l231);soc_mem_field32_set(l74,l229,l226,IP_LENGTH1f
,l87+l231);}else{soc_mem_field32_set(l74,l229,l226,IP_LENGTH0f,l87);
soc_mem_field32_set(l74,l229,l226,IP_LENGTH1f,l231);}l151 = 
soc_mem_field32_get(l74,l111,entry,IP_ADDR0f);soc_mem_field32_set(l74,l229,
l226,IP_ADDR0f,l151);l151 = soc_mem_field32_get(l74,l111,entry,IP_ADDR1f);
soc_mem_field32_set(l74,l229,l226,IP_ADDR1f,l151);l151 = soc_mem_field32_get(
l74,l111,entry,ENTRY_TYPE0f);soc_mem_field32_set(l74,l229,l226,ENTRY_TYPE0f,
l151);l151 = soc_mem_field32_get(l74,l111,entry,ENTRY_TYPE1f);
soc_mem_field32_set(l74,l229,l226,ENTRY_TYPE1f,l151);if(!l122){sal_memcpy(&
l230,entry,sizeof(l230));l144 = soc_th_alpm_lpm_vrf_get(l74,(void*)&l230,&
l116,&l87);SOC_IF_ERROR_RETURN(l144);SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_ip4entry1_to_0(l74,&l230,&l230,PRESERVE_HIT));l144 = 
soc_th_alpm_lpm_vrf_get(l74,(void*)&l230,&l206,&l87);SOC_IF_ERROR_RETURN(l144
);}else{l144 = soc_th_alpm_lpm_vrf_get(l74,entry,&l116,&l87);}if(
SOC_URPF_STATUS_GET(l74)){if(l227>= (soc_mem_index_count(l74,L3_DEFIPm)>>1)){
l228 = 1;}}switch(l116){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l74,l229
,l226,VALID0f,0);l100 = 0;break;case SOC_L3_VRF_GLOBAL:l100 = l228?1:0;break;
default:l100 = l228?3:2;break;}soc_mem_field32_set(l74,l229,l226,DB_TYPE0f,
l100);if(!l122){switch(l206){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l74
,l229,l226,VALID1f,0);l100 = 0;break;case SOC_L3_VRF_GLOBAL:l100 = l228?1:0;
break;default:l100 = l228?3:2;break;}soc_mem_field32_set(l74,l229,l226,
DB_TYPE1f,l100);}else{if(l116 == SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l74
,l229,l226,VALID1f,0);}soc_mem_field32_set(l74,l229,l226,DB_TYPE1f,l100);}if(
l228){l151 = soc_mem_field32_get(l74,l111,entry,ALG_BKT_PTR0f);if(l151){l151
+= SOC_TH_ALPM_BUCKET_COUNT(l74);soc_mem_field32_set(l74,l111,entry,
ALG_BKT_PTR0f,l151);}l151 = soc_mem_field32_get(l74,l111,entry,
ALG_SUB_BKT_PTR0f);if(l151){soc_mem_field32_set(l74,l111,entry,
ALG_SUB_BKT_PTR0f,l151);}if(!l122){l151 = soc_mem_field32_get(l74,l111,entry,
ALG_BKT_PTR1f);if(l151){l151+= SOC_TH_ALPM_BUCKET_COUNT(l74);
soc_mem_field32_set(l74,l111,entry,ALG_BKT_PTR1f,l151);}l151 = 
soc_mem_field32_get(l74,l111,entry,ALG_SUB_BKT_PTR1f);if(l151){
soc_mem_field32_set(l74,l111,entry,ALG_SUB_BKT_PTR1f,l151);}}}return
SOC_E_NONE;}static int l232(int l74,int l233,int index,int l234,void*entry){
defip_aux_table_entry_t l226;l234 = soc_th_alpm_physical_idx(l74,L3_DEFIPm,
l234,1);SOC_IF_ERROR_RETURN(l224(l74,l234,entry,(void*)&l226,index));
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(l74,MEM_BLOCK_ANY,index,entry));index = 
soc_th_alpm_physical_idx(l74,L3_DEFIPm,index,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_TABLEm(l74,MEM_BLOCK_ANY,index,&l226));return SOC_E_NONE;}
int _soc_th_alpm_insert_in_bkt(int l74,soc_mem_t l111,int bktid,int l146,void
*alpm_data,uint32*l75,int*l88,int l122){int l144;l144 = soc_mem_alpm_insert(
l74,l111,ALPM_BKT_IDX(bktid),MEM_BLOCK_ANY,l146,alpm_data,l75,l88);if(l144 == 
SOC_E_FULL){if(SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l144 = soc_mem_alpm_insert(
l74,l111,ALPM_BKT_IDX(bktid)+1,MEM_BLOCK_ANY,l146,alpm_data,l75,l88);}}return
l144;}int _soc_th_alpm_mem_index(int l74,soc_mem_t l111,int bucket_index,int
l235,uint32 l146,int*l236){int l166,l179 = 0;int l237[4] = {0};int l238 = 0;
int l239 = 0;int l240;int l241 = 6;int l242;int l243;int l244;int l245 = 0;
switch(l111){case L3_DEFIP_ALPM_IPV6_64m:l241 = 4;break;case
L3_DEFIP_ALPM_IPV6_128m:l241 = 2;break;default:break;}if(
SOC_TH_ALPM_SCALE_CHECK(l74,(l111!= L3_DEFIP_ALPM_IPV4m))){if(l235>= 
ALPM_RAW_BKT_COUNT*l241){bucket_index++;l235-= ALPM_RAW_BKT_COUNT*l241;}}l242
= 4;l243 = 15;l244 = 2;if(soc_th_get_alpm_banks(l74)<= 2){l242 = 2;l243 = 14;
l244 = 1;}l245 = ((1<<l242)-1);l240 = l242-_shr_popcount(l146&l245);if(
bucket_index>= (1<<l243)||l235>= l240*l241){return SOC_E_PARAM;}l239 = l235%
l241;for(l166 = 0;l166<l242;l166++){if((1<<l166)&l146){continue;}l237[l179++]
= l166;}l238 = l237[l235/l241];*l236 = (l239<<l243)|(bucket_index<<l244)|(
l238);return SOC_E_NONE;}static int _soc_th_alpm_move_inval(int l74,soc_mem_t
l111,int l122,alpm_mem_prefix_array_t*l131,int*l91){int l166,l144 = 
SOC_E_NONE,l184;defip_alpm_ipv4_entry_t l188;defip_alpm_ipv6_64_entry_t l190;
int l246,l247;void*l248 = NULL,*l249 = NULL;int*l250 = NULL;int l251 = FALSE;
l246 = l122?sizeof(l190):sizeof(l188);l247 = l246*l131->count;l248 = 
sal_alloc(l247,"rb_bufp");l249 = sal_alloc(l247,"rb_sip_bufp");l250 = 
sal_alloc(sizeof(*l250)*l131->count,"roll_back_index");if(l248 == NULL||l249
== NULL||l250 == NULL){l144 = SOC_E_MEMORY;goto l252;}sal_memset(l250,-1,
sizeof(*l250)*l131->count);for(l166 = 0;l166<l131->count;l166++){payload_t*
prefix = l131->prefix[l166];if(prefix->index>= 0){l144 = soc_mem_read(l74,
l111,MEM_BLOCK_ANY,prefix->index,(uint8*)l248+l166*l246);if(SOC_FAILURE(l144)
){l166--;l251 = TRUE;break;}if(SOC_URPF_STATUS_GET(l74)){l144 = soc_mem_read(
l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,prefix->index),(uint8*)l249
+l166*l246);if(SOC_FAILURE(l144)){l166--;l251 = TRUE;break;}}l144 = 
soc_mem_write(l74,l111,MEM_BLOCK_ALL,prefix->index,soc_mem_entry_null(l74,
l111));if(SOC_FAILURE(l144)){l250[l166] = prefix->index;l251 = TRUE;break;}if
(SOC_URPF_STATUS_GET(l74)){l144 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l74,prefix->index),soc_mem_entry_null(l74,l111));if(
SOC_FAILURE(l144)){l250[l166] = prefix->index;l251 = TRUE;break;}}}l250[l166]
= prefix->index;prefix->index = l91[l166];}if(l251){for(;l166>= 0;l166--){
payload_t*prefix = l131->prefix[l166];prefix->index = l250[l166];if(l250[l166
]<0){continue;}l184 = soc_mem_write(l74,l111,MEM_BLOCK_ALL,l250[l166],(uint8*
)l248+l166*l246);if(SOC_FAILURE(l184)){break;}if(!SOC_URPF_STATUS_GET(l74)){
continue;}l184 = soc_mem_write(l74,l111,MEM_BLOCK_ALL,_soc_th_alpm_rpf_entry(
l74,l250[l166]),(uint8*)l249+l166*l246);if(SOC_FAILURE(l184)){break;}}}l252:
if(l250!= NULL){sal_free(l250);}if(l249!= NULL){sal_free(l249);}if(l248!= 
NULL){sal_free(l248);}return l144;}void _soc_th_alpm_rollback_pivot_add(int
l74,defip_entry_t*l137,void*key_data,int tcam_index,alpm_pivot_t*pivot_pyld){
int l144;trie_t*l158 = NULL;int l122,l116,l161;trie_node_t*l253 = NULL;l122 = 
soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);(void)
soc_th_alpm_lpm_vrf_get(l74,key_data,&l161,&l116);l144 = 
soc_th_alpm_lpm_delete(l74,l137);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: Failure to free new prefix""at %d\n"),
soc_th_alpm_logical_idx(l74,L3_DEFIPm,tcam_index,l122)));}if(l122){l158 = 
VRF_PIVOT_TRIE_IPV6(l74,l116);}else{l158 = VRF_PIVOT_TRIE_IPV4(l74,l116);}if(
ALPM_TCAM_PIVOT(l74,tcam_index<<(l122?1:0))!= NULL){l144 = trie_delete(l158,
pivot_pyld->key,pivot_pyld->len,&l253);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_insert: trie delete failure "
"in bkt move rollback\n")));}}ALPM_TCAM_PIVOT(l74,tcam_index<<(l122?1:0)) = 
NULL;VRF_PIVOT_REF_DEC(l74,l116,l122);}int _soc_th_alpm_rollback_bkt_move(int
l74,void*key_data,soc_mem_t l111,alpm_pivot_t*l133,alpm_pivot_t*l134,
alpm_mem_prefix_array_t*l135,int*l91,int l136){trie_node_t*l253 = NULL;uint32
prefix[5],l127;trie_t*l125;int l122,l116,l161,l166,l113 = 0;
defip_alpm_ipv4_entry_t l188;defip_alpm_ipv6_64_entry_t l190;
defip_alpm_ipv6_128_entry_t l254;void*l186;payload_t*l255;int l144 = 
SOC_E_NONE;alpm_bucket_handle_t*l256;l256 = PIVOT_BUCKET_HANDLE(l134);l122 = 
soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);if(l111 == 
L3_DEFIP_ALPM_IPV6_128m){l122 = L3_DEFIP_MODE_128;}if(l111 == 
L3_DEFIP_ALPM_IPV6_128m){l144 = _alpm_128_prefix_create(l74,key_data,prefix,&
l127,&l113);}else{l144 = l152(l74,key_data,prefix,&l127,&l113);}if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"prefix create failed\n")));return l144;}if(l111 == L3_DEFIP_ALPM_IPV6_128m){
l186 = ((uint32*)&(l254));(void)soc_th_alpm_128_lpm_vrf_get(l74,key_data,&
l161,&l116);}else{l186 = ((l122)?((uint32*)&(l190)):((uint32*)&(l188)));(void
)soc_th_alpm_lpm_vrf_get(l74,key_data,&l161,&l116);}if(l111 == 
L3_DEFIP_ALPM_IPV6_128m){l125 = VRF_PREFIX_TRIE_IPV6_128(l74,l116);}else if(
l122){l125 = VRF_PREFIX_TRIE_IPV6(l74,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4
(l74,l116);}for(l166 = 0;l166<l135->count;l166++){payload_t*l87 = l135->
prefix[l166];if(l91[l166]!= -1){if(l111 == L3_DEFIP_ALPM_IPV6_128m){
sal_memset(l186,0,sizeof(defip_alpm_ipv6_128_entry_t));}else if(l122){
sal_memset(l186,0,sizeof(defip_alpm_ipv6_64_entry_t));}else{sal_memset(l186,0
,sizeof(defip_alpm_ipv4_entry_t));}l144 = soc_mem_write(l74,l111,
MEM_BLOCK_ANY,l91[l166],l186);_soc_tomahawk_alpm_bkt_view_set(l74,l91[l166],
INVALIDm);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: mem write failure""in bkt move rollback\n")));}if(
SOC_URPF_STATUS_GET(l74)){l144 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,
_soc_th_alpm_rpf_entry(l74,l91[l166]),l186);_soc_tomahawk_alpm_bkt_view_set(
l74,_soc_th_alpm_rpf_entry(l74,l91[l166]),INVALIDm);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_insert: urpf mem write "
"failure in bkt move rollback\n")));}}}l253 = NULL;l144 = trie_delete(
PIVOT_BUCKET_TRIE(l134),l87->key,l87->len,&l253);l255 = (payload_t*)l253;if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n")));}if(l87->
index>0){l144 = trie_insert(PIVOT_BUCKET_TRIE(l133),l87->key,NULL,l87->len,(
trie_node_t*)l255);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"_soc_alpm_insert: trie reinsert failure"
"in bkt move rollback\n")));}}else{if(l255!= NULL){sal_free(l255);}}}if(l136
== -1){l253 = NULL;l144 = trie_delete(PIVOT_BUCKET_TRIE(l133),prefix,l127,&
l253);l255 = (payload_t*)l253;if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l74,"_soc_th_alpm_insert: expected to clear prefix"
" 0x%08x 0x%08x\n from old trie. Failed\n"),prefix[0],prefix[1]));}if(l255!= 
NULL){sal_free(l255);}}l144 = soc_th_alpm_bs_free(l74,PIVOT_BUCKET_INDEX(l134
),l116,l122);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: new bucket release failure: %d\n"),PIVOT_BUCKET_INDEX(
l134)));}trie_destroy(PIVOT_BUCKET_TRIE(l134));sal_free(l256);sal_free(l134);
sal_free(l91);l253 = NULL;l144 = trie_delete(l125,prefix,l127,&l253);l255 = (
payload_t*)l253;if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"_soc_th_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n"),prefix[0],prefix[1]));}if(l255){sal_free(
l255);}return l144;}int _soc_th_alpm_free_pfx_trie(int l74,trie_t*l125,trie_t
*l257,payload_t*new_pfx_pyld,int*l91,int bktid,int l116,int l122){int l144 = 
SOC_E_NONE;trie_node_t*l253 = NULL;payload_t*l258 = NULL;payload_t*l259 = 
NULL;if(l91!= NULL){sal_free(l91);}(void)trie_delete(l125,new_pfx_pyld->key,
new_pfx_pyld->len,&l253);l259 = (payload_t*)l253;if(l259!= NULL){(void)
trie_delete(l257,l259->key,l259->len,&l253);l258 = (payload_t*)l253;if(l258!= 
NULL){sal_free(l258);}sal_free(l259);}if(bktid!= -1){l144 = 
soc_th_alpm_bs_free(l74,bktid,l116,l122);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d Unable to free bucket_id %d"),l74,
bktid));}}return SOC_E_NONE;}static int l123(int l74,alpm_pfx_info_t*l124,
trie_t*l125,uint32*l126,uint32 l127,trie_node_t*l260,defip_entry_t*lpm_entry,
uint32*l128){trie_node_t*l159 = NULL;int l122,l116,l161;
defip_alpm_ipv4_entry_t l188;defip_alpm_ipv6_64_entry_t l190;payload_t*l261 = 
NULL;int l262;void*l186;alpm_pivot_t*l263;alpm_bucket_handle_t*l256;int l144 = 
SOC_E_NONE;soc_mem_t l111;l263 = l124->pivot_pyld;l262 = l263->tcam_index;
l122 = soc_mem_field32_get(l74,L3_DEFIPm,l124->key_data,MODE0f);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,l124->key_data,&l161,&l116));
l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l186 = ((l122)?((
uint32*)&(l190)):((uint32*)&(l188)));l159 = NULL;l144 = trie_find_lpm(l125,
l126,l127,&l159);l261 = (payload_t*)l159;if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d Unable to find lpm for pivot: "
"0x%08x 0x%08x\n 0x%08x 0x%08x 0x%08x length: %d\n"),l74,l126[0],l126[1],l126
[2],l126[3],l126[4],l127));return l144;}if(l261->bkt_ptr){if(l261->bkt_ptr == 
l124->new_pfx_pyld){sal_memcpy(l186,l124->alpm_data,l122?sizeof(
defip_alpm_ipv6_64_entry_t):sizeof(defip_alpm_ipv4_entry_t));}else{l144 = 
soc_mem_read(l74,l111,MEM_BLOCK_ANY,((payload_t*)l261->bkt_ptr)->index,l186);
}if(SOC_FAILURE(l144)){return l144;}l144 = l115(l74,l186,l111,l122,l161,l124
->bktid,0,lpm_entry);if(SOC_FAILURE(l144)){return l144;}*l128 = ((payload_t*)
(l261->bkt_ptr))->len;}else{l144 = soc_mem_read(l74,L3_DEFIPm,MEM_BLOCK_ANY,
soc_th_alpm_logical_idx(l74,L3_DEFIPm,l262>>1,1),lpm_entry);if((!l122)&&(l262
&1)){l144 = soc_th_alpm_lpm_ip4entry1_to_0(l74,lpm_entry,lpm_entry,0);}}l256 = 
sal_alloc(sizeof(alpm_bucket_handle_t),"ALPM Bucket Handle");if(l256 == NULL)
{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l144 = SOC_E_MEMORY;return l144;}sal_memset(l256,0,sizeof(*l256));l263 = 
sal_alloc(sizeof(alpm_pivot_t),"Payload for new Pivot");if(l263 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n")));
l144 = SOC_E_MEMORY;return l144;}sal_memset(l263,0,sizeof(*l263));
PIVOT_BUCKET_HANDLE(l263) = l256;if(l122){l144 = trie_init(_MAX_KEY_LEN_144_,
&PIVOT_BUCKET_TRIE(l263));}else{l144 = trie_init(_MAX_KEY_LEN_48_,&
PIVOT_BUCKET_TRIE(l263));}PIVOT_BUCKET_TRIE(l263)->trie = l260;
PIVOT_BUCKET_INDEX(l263) = l124->bktid;PIVOT_BUCKET_VRF(l263) = l116;
PIVOT_BUCKET_IPV6(l263) = l122;PIVOT_BUCKET_DEF(l263) = FALSE;(l263)->key[0] = 
l126[0];(l263)->key[1] = l126[1];(l263)->key[2] = l126[2];(l263)->key[3] = 
l126[3];(l263)->key[4] = l126[4];(l263)->len = l127;do{if(!(l122)){l126[0] = 
(((32-l127) == 32)?0:(l126[1])<<(32-l127));l126[1] = 0;}else{int l264 = 64-
l127;int l265;if(l264<32){l265 = l126[3]<<l264;l265|= (((32-l264) == 32)?0:(
l126[4])>>(32-l264));l126[0] = l126[4]<<l264;l126[1] = l265;l126[2] = l126[3]
= l126[4] = 0;}else{l126[1] = (((l264-32) == 32)?0:(l126[4])<<(l264-32));l126
[0] = l126[2] = l126[3] = l126[4] = 0;}}}while(0);l118(l74,l126,l127,l116,
l122,lpm_entry,0,0);soc_L3_DEFIPm_field32_set(l74,lpm_entry,ALG_BKT_PTR0f,
ALPM_BKT_IDX(l124->bktid));soc_L3_DEFIPm_field32_set(l74,lpm_entry,
ALG_SUB_BKT_PTR0f,ALPM_BKT_SIDX(l124->bktid));l124->pivot_pyld = l263;return
l144;}static int l129(int l74,alpm_pfx_info_t*l124,int*l130,int*l104){
trie_node_t*l260;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int l122,l116,l161;
uint32 l127,l128 = 0;uint32 l146 = 0;uint32 l126[5];int l88;
defip_alpm_ipv4_entry_t l188,l189;defip_alpm_ipv6_64_entry_t l190,l191;trie_t
*l125,*trie;void*l186,*l187;alpm_pivot_t*l266 = l124->pivot_pyld;
defip_entry_t lpm_entry;soc_mem_t l111;trie_t*l158 = NULL;
alpm_mem_prefix_array_t l172;int*l91 = NULL;int*l90 = NULL;int l144 = 
SOC_E_NONE,l166,l136 = -1;int tcam_index,l267,l268 = 0;int l269 = 0,l270 = 0;
l122 = soc_mem_field32_get(l74,L3_DEFIPm,l124->key_data,MODE0f);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,l124->key_data,&l161,&l116));
l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l186 = ((l122)?((
uint32*)&(l190)):((uint32*)&(l188)));l187 = ((l122)?((uint32*)&(l191)):((
uint32*)&(l189)));soc_th_alpm_bank_db_type_get(l74,l116,&l146,NULL);if(l122){
l125 = VRF_PREFIX_TRIE_IPV6(l74,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4(l74,
l116);}trie = PIVOT_BUCKET_TRIE(l124->pivot_pyld);l268 = l124->bktid;l267 = 
PIVOT_TCAM_INDEX(l266);l144 = soc_th_alpm_bs_alloc(l74,&l124->bktid,l116,l122
);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: Unable to allocate""new bucket for split\n")));l124->
bktid = -1;_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,l91,
l124->bktid,l116,l122);return l144;}l144 = trie_split(trie,l122?
_MAX_KEY_LEN_144_:_MAX_KEY_LEN_48_,FALSE,l126,&l127,&l260,NULL,FALSE);if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: Could not split bucket")));_soc_th_alpm_free_pfx_trie(
l74,l125,trie,l124->new_pfx_pyld,l91,l124->bktid,l116,l122);return l144;}l144
= l123(l74,l124,l125,l126,l127,l260,&lpm_entry,&l128);if(l144!= SOC_E_NONE){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"could not initialize pivot")));
_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,l91,l124->bktid,
l116,l122);return l144;}sal_memset(&l172,0,sizeof(l172));l144 = trie_traverse
(PIVOT_BUCKET_TRIE(l124->pivot_pyld),_soc_th_alpm_mem_prefix_array_cb,&l172,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"_soc_th_alpm_insert: Bucket split failed")));
_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,l91,l124->bktid,
l116,l122);return l144;}l91 = sal_alloc(sizeof(*l91)*l172.count,"new_index");
if(l91 == NULL){_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,
l91,l124->bktid,l116,l122);return SOC_E_MEMORY;}l90 = sal_alloc(sizeof(*l90)*
l172.count,"new_index");if(l90 == NULL){_soc_th_alpm_free_pfx_trie(l74,l125,
trie,l124->new_pfx_pyld,l91,l124->bktid,l116,l122);return SOC_E_MEMORY;}
sal_memset(l91,-1,sizeof(*l91)*l172.count);sal_memset(l90,-1,sizeof(*l90)*
l172.count);for(l166 = 0;l166<l172.count;l166++){payload_t*l87 = l172.prefix[
l166];if(l87->index>0){l144 = soc_mem_read(l74,l111,MEM_BLOCK_ANY,l87->index,
l186);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_insert: Failed to""read prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l172.prefix[l166]->key[1],l172.
prefix[l166]->key[2],l172.prefix[l166]->key[3],l172.prefix[l166]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l266,l124->
pivot_pyld,&l172,l91,l136);sal_free(l90);return l144;}if(SOC_URPF_STATUS_GET(
l74)){l144 = soc_mem_read(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,
l87->index),l187);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l74,"_soc_th_alpm_insert: Failed to""read rpf prefix "
"0x%08x 0x%08x 0x%08x 0x%08x for move\n"),l172.prefix[l166]->key[1],l172.
prefix[l166]->key[2],l172.prefix[l166]->key[3],l172.prefix[l166]->key[4]));(
void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l266,l124->
pivot_pyld,&l172,l91,l136);sal_free(l90);return l144;}}soc_mem_field32_set(
l74,l111,l186,SUB_BKT_PTRf,ALPM_BKT_SIDX(l124->bktid));soc_mem_field32_set(
l74,l111,l187,SUB_BKT_PTRf,ALPM_BKT_SIDX(l124->bktid));l144 = 
_soc_th_alpm_insert_in_bkt(l74,l111,l124->bktid,l146,l186,l75,&l88,l122);if(
SOC_SUCCESS(l144)){if(SOC_URPF_STATUS_GET(l74)){l144 = soc_mem_write(l74,l111
,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),l187);}l269++;l270++;}}else{
soc_mem_field32_set(l74,l111,l124->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(l124
->bktid));l144 = _soc_th_alpm_insert_in_bkt(l74,l111,l124->bktid,l146,l124->
alpm_data,l75,&l88,l122);if(SOC_SUCCESS(l144)){l136 = l166;*l104 = l88;if(
SOC_URPF_STATUS_GET(l74)){soc_mem_field32_set(l74,l111,l124->alpm_sip_data,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l124->bktid));l144 = soc_mem_write(l74,l111,
MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),l124->alpm_sip_data);}l270++;}}
l91[l166] = l88;if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"_soc_th_alpm_insert: Failed to ""insert prefix "
"0x%08x 0x%08x 0x%08x 0x%08x to bucket %d\n"),l172.prefix[l166]->key[1],l172.
prefix[l166]->key[2],l172.prefix[l166]->key[3],l172.prefix[l166]->key[4],l124
->bktid));(void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l266,
l124->pivot_pyld,&l172,l91,l136);sal_free(l90);return l144;}l90[l166] = l87->
index;}l144 = l96(l74,&lpm_entry,l130);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_th_alpm_insert: Unable to add new"
"pivot to tcam\n")));if(l144 == SOC_E_FULL){VRF_PIVOT_FULL_INC(l74,l116,l122)
;}(void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l266,l124->
pivot_pyld,&l172,l91,l136);sal_free(l90);return l144;}*l130 = 
soc_th_alpm_physical_idx(l74,L3_DEFIPm,*l130,l122);l144 = l223(l74,*l130,l122
,l128);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: Unable to init bpm_len ""for index %d\n"),*l130));
_soc_th_alpm_rollback_pivot_add(l74,&lpm_entry,l124->key_data,*l130,l124->
pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l266
,l124->pivot_pyld,&l172,l91,l136);sal_free(l90);return l144;}if(l122){l158 = 
VRF_PIVOT_TRIE_IPV6(l74,l116);}else{l158 = VRF_PIVOT_TRIE_IPV4(l74,l116);}
l144 = trie_insert(l158,l124->pivot_pyld->key,NULL,l124->pivot_pyld->len,(
trie_node_t*)l124->pivot_pyld);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"failed to insert into pivot trie\n")));(void
)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l266,l124->pivot_pyld
,&l172,l91,l136);sal_free(l90);return l144;}tcam_index = *l130<<(l122?1:0);
ALPM_TCAM_PIVOT(l74,tcam_index) = l124->pivot_pyld;PIVOT_TCAM_INDEX(l124->
pivot_pyld) = tcam_index;VRF_PIVOT_REF_INC(l74,l116,l122);l144 = 
_soc_th_alpm_move_inval(l74,l111,l122,&l172,l91);if(SOC_FAILURE(l144)){
_soc_th_alpm_rollback_pivot_add(l74,&lpm_entry,l124->key_data,*l130,l124->
pivot_pyld);(void)_soc_th_alpm_rollback_bkt_move(l74,l124->key_data,l111,l266
,l124->pivot_pyld,&l172,l91,l136);sal_free(l91);l91 = NULL;sal_free(l90);
return l144;}l144 = soc_th_alpm_update_hit_bits(l74,l172.count,l90,l91);if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_th_alpm_update_hit_bits failed, count %d\n"),l172.count));l144 = 
SOC_E_NONE;}sal_free(l91);l91 = NULL;sal_free(l90);if(l136 == -1){
soc_mem_field32_set(l74,l111,l124->alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(
PIVOT_BUCKET_INDEX(l266)));l144 = _soc_th_alpm_insert_in_bkt(l74,l111,
PIVOT_BUCKET_INDEX(l266),l146,l124->alpm_data,l75,&l88,l122);if(SOC_FAILURE(
l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_th_alpm_insert: Could not insert new ""prefix into trie after split\n")
));_soc_th_alpm_free_pfx_trie(l74,l125,trie,l124->new_pfx_pyld,l91,l124->
bktid,l116,l122);return l144;}l269--;if(SOC_URPF_STATUS_GET(l74)){l144 = 
soc_mem_write(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),l124->
alpm_sip_data);}*l104 = l88;l124->new_pfx_pyld->index = l88;}
soc_th_alpm_bu_upd(l74,l268,l267,FALSE,l122,-l269);soc_th_alpm_bu_upd(l74,
l124->bktid,tcam_index,l116,l122,l270);PIVOT_BUCKET_ENT_CNT_UPDATE(l124->
pivot_pyld);VRF_BUCKET_SPLIT_INC(l74,l116,l122);return l144;}static int l271(
int l74,void*key_data,soc_mem_t l111,void*alpm_data,void*alpm_sip_data,int*
l104,int bktid,int tcam_index){alpm_pivot_t*l263,*l266;
defip_aux_scratch_entry_t l102;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];uint32
prefix[5],l272,l127;int l122,l116,l161;int l88;int l144 = SOC_E_NONE,l184;
uint32 l100,l146;int l157 =0;int l130;int l273 = 0;trie_t*trie,*l125;
trie_node_t*l159 = NULL,*l253 = NULL;payload_t*l255,*l274,*l261;int l113 = 0;
alpm_pfx_info_t l124;l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f
);if(l122){if(!(l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE1f))){
return(SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,
key_data,&l161,&l116));soc_th_alpm_bank_db_type_get(l74,l116,&l146,&l100);
l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l144 = l152(l74,
key_data,prefix,&l127,&l113);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l74,"_soc_th_alpm_insert: prefix create failed\n")));return l144;
}sal_memset(&l102,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(
l98(l74,key_data,l122,l100,0,&l102));if(bktid == 0){l144 = l156(l74,prefix,
l127,l122,l116,&l157,&tcam_index,&bktid);SOC_IF_ERROR_RETURN(l144);
soc_mem_field32_set(l74,l111,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid));}
l144 = _soc_th_alpm_insert_in_bkt(l74,l111,bktid,l146,alpm_data,l75,&l88,l122
);if(l144 == SOC_E_NONE){*l104 = l88;if(SOC_URPF_STATUS_GET(l74)){
soc_mem_field32_set(l74,l111,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid))
;l184 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),
alpm_sip_data);if(SOC_FAILURE(l184)){return l184;}}}if(l144 == SOC_E_FULL){
l157 = _soc_th_alpm_bkt_entry_cnt(l74,l122);if(l157>4){l157 = 4;}if(
PIVOT_BUCKET_COUNT(ALPM_TCAM_PIVOT(l74,tcam_index))<l157){int l275;l144 = 
soc_th_alpm_bs_alloc(l74,&l275,l116,l122);SOC_IF_ERROR_RETURN(l144);l144 = 
_soc_th_alpm_move_trie(l74,l122,bktid,l275);SOC_IF_ERROR_RETURN(l144);bktid = 
l275;soc_mem_field32_set(l74,l111,alpm_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid)
);l144 = _soc_th_alpm_insert_in_bkt(l74,l111,bktid,l146,alpm_data,l75,&l88,
l122);if(SOC_SUCCESS(l144)){*l104 = l88;if(SOC_URPF_STATUS_GET(l74)){
soc_mem_field32_set(l74,l111,alpm_sip_data,SUB_BKT_PTRf,ALPM_BKT_SIDX(bktid))
;l184 = soc_mem_write(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,l88),
alpm_sip_data);if(SOC_FAILURE(l184)){return l184;}}}else{return l144;}}else{
l273 = 1;}}l263 = ALPM_TCAM_PIVOT(l74,tcam_index);trie = PIVOT_BUCKET_TRIE(
l263);l266 = l263;l255 = sal_alloc(sizeof(payload_t),"Payload for Key");if(
l255 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 payload entry\n")));
return SOC_E_MEMORY;}l274 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l274 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_vrf_add: unable to allocate memory for "
"pfx trie node\n")));sal_free(l255);return SOC_E_MEMORY;}sal_memset(l255,0,
sizeof(*l255));sal_memset(l274,0,sizeof(*l274));sal_memcpy(l255->key,prefix,
sizeof(prefix));l255->len = l127;l255->index = l88;sal_memcpy(l274,l255,
sizeof(*l255));l274->bkt_ptr = l255;l144 = trie_insert(trie,prefix,NULL,l127,
(trie_node_t*)l255);if(SOC_FAILURE(l144)){if(l255!= NULL){sal_free(l255);}if(
l274!= NULL){sal_free(l274);}return l144;}if(l122){l125 = 
VRF_PREFIX_TRIE_IPV6(l74,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4(l74,l116);}
if(!l113){l144 = trie_insert(l125,prefix,NULL,l127,(trie_node_t*)l274);}else{
l159 = NULL;l144 = trie_find_lpm(l125,0,0,&l159);l261 = (payload_t*)l159;if(
SOC_SUCCESS(l144)){l261->bkt_ptr = l255;}}l272 = l127;if(SOC_FAILURE(l144)){
l253 = NULL;(void)trie_delete(trie,prefix,l272,&l253);l261 = (payload_t*)l253
;sal_free(l261);sal_free(l274);return l144;}if(l273){l124.key_data = key_data
;l124.new_pfx_pyld = l255;l124.pivot_pyld = l263;l124.alpm_data = alpm_data;
l124.alpm_sip_data = alpm_sip_data;l124.bktid = bktid;l144 = l129(l74,&l124,&
l130,l104);if(l144!= SOC_E_NONE){return l144;}bktid = l124.bktid;tcam_index = 
PIVOT_TCAM_INDEX(l124.pivot_pyld);alpm_split_count++;}else{soc_th_alpm_bu_upd
(l74,bktid,tcam_index,l116,l122,1);}VRF_TRIE_ROUTES_INC(l74,l116,l122);if(
l113){sal_free(l274);}SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l74,
DELETE_PROPAGATE,&l102,TRUE,&l157,&tcam_index,&bktid));SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,INSERT_PROPAGATE,&l102,FALSE,&l157,&tcam_index,&bktid
));if(SOC_URPF_STATUS_GET(l74)){uint32 l276[4] = {0,0,0,0};l127 = 
soc_mem_field32_get(l74,L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf);l127+= 1;
soc_mem_field32_set(l74,L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf,l127);if(l113){
soc_mem_field32_set(l74,L3_DEFIP_AUX_SCRATCHm,&l102,RPEf,1);}else{
soc_mem_field32_set(l74,L3_DEFIP_AUX_SCRATCHm,&l102,RPEf,0);}
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l74,DELETE_PROPAGATE,&l102,TRUE,&l157
,&tcam_index,&bktid));SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l74,
INSERT_PROPAGATE,&l102,FALSE,&l157,&tcam_index,&bktid));if(!l113){
soc_mem_field_set(l74,L3_DEFIP_AUX_SCRATCHm,(uint32*)&l102,IP_ADDRf,(uint32*)
l276);soc_mem_field32_set(l74,L3_DEFIP_AUX_SCRATCHm,&l102,IP_LENGTHf,0);
soc_mem_field32_set(l74,L3_DEFIP_AUX_SCRATCHm,&l102,REPLACE_LENf,0);
soc_mem_field32_set(l74,L3_DEFIP_AUX_SCRATCHm,&l102,RPEf,1);
SOC_IF_ERROR_RETURN(_soc_th_alpm_aux_op(l74,INSERT_PROPAGATE,&l102,TRUE,&l157
,&tcam_index,&bktid));}}PIVOT_BUCKET_ENT_CNT_UPDATE(l266);return l144;}static
int l118(int unit,uint32*key,int len,int l116,int l99,defip_entry_t*lpm_entry
,int l119,int l120){uint32 l277;if(l120){sal_memset(lpm_entry,0,sizeof(
defip_entry_t));}soc_L3_DEFIPm_field32_set(unit,lpm_entry,VRF_ID_0f,l116&
SOC_VRF_MAX(unit));if(l116 == (SOC_VRF_MAX(unit)+1)){
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
;}int _soc_th_alpm_delete_in_bkt(int l74,soc_mem_t l111,int l278,int l146,
void*l279,uint32*l75,int*l88,int l122){int l144;l144 = soc_mem_alpm_delete(
l74,l111,ALPM_BKT_IDX(l278),MEM_BLOCK_ALL,l146,l279,l75,l88);if(SOC_SUCCESS(
l144)){return l144;}if(SOC_TH_ALPM_SCALE_CHECK(l74,l122)){return
soc_mem_alpm_delete(l74,l111,ALPM_BKT_IDX(l278)+1,MEM_BLOCK_ALL,l146,l279,l75
,l88);}return l144;}static int l280(int l74,void*key_data,int bktid,int
tcam_index,int l88){alpm_pivot_t*pivot_pyld;defip_alpm_ipv4_entry_t l188,l189
,l281;defip_alpm_ipv6_64_entry_t l190,l191,l282;defip_aux_scratch_entry_t l102
;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l111;void*l186,*l279,*l187 = 
NULL;int l161;int l99;int l144 = SOC_E_NONE,l184;uint32 l283[5],prefix[5];int
l122,l116;uint32 l127;int l278;uint32 l100,l146;int l157,l113 = 0;trie_t*trie
,*l125;uint32 l284;defip_entry_t lpm_entry,*l285;payload_t*l255 = NULL,*l286 = 
NULL,*l261 = NULL;trie_node_t*l253 = NULL,*l159 = NULL;trie_t*l158 = NULL;l99
= l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);if(l122){if(!(
l122 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l161
,&l116));if(!(ALPM_PREFIX_IN_TCAM(l74,l161))){soc_th_alpm_bank_db_type_get(
l74,l116,&l146,&l100);l144 = l152(l74,key_data,prefix,&l127,&l113);if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: prefix create failed\n")));return l144;}if(
soc_th_alpm_mode_get(l74) == SOC_ALPM_MODE_COMBINED){if(l161!= 
SOC_L3_VRF_GLOBAL){if(VRF_TRIE_ROUTES_CNT(l74,l116,l122)>1){if(l113){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"VRF %d: Cannot delete default "
"route if other routes are present in ""this mode"),l116));return SOC_E_PARAM
;}}}l100 = 2;}l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l186 = 
((l122)?((uint32*)&(l190)):((uint32*)&(l188)));l279 = ((l122)?((uint32*)&(
l282)):((uint32*)&(l281)));SOC_ALPM_LPM_LOCK(l74);if(bktid == 0){l144 = 
_soc_th_alpm_find(l74,key_data,l111,l186,&tcam_index,&bktid,&l88,TRUE);}else{
l144 = l108(l74,key_data,l186,0,l111,0,0,bktid);}sal_memcpy(l279,l186,l122?
sizeof(l190):sizeof(l188));if(SOC_FAILURE(l144)){SOC_ALPM_LPM_UNLOCK(l74);
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Unable to find prefix for delete\n")));return l144;}l278 = 
bktid;pivot_pyld = ALPM_TCAM_PIVOT(l74,tcam_index);trie = PIVOT_BUCKET_TRIE(
pivot_pyld);l144 = trie_delete(trie,prefix,l127,&l253);l255 = (payload_t*)
l253;if(l144!= SOC_E_NONE){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Error prefix not present in trie \n")));
SOC_ALPM_LPM_UNLOCK(l74);return l144;}if(l122){l125 = VRF_PREFIX_TRIE_IPV6(
l74,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4(l74,l116);}if(l122){l158 = 
VRF_PIVOT_TRIE_IPV6(l74,l116);}else{l158 = VRF_PIVOT_TRIE_IPV4(l74,l116);}if(
!l113){l144 = trie_delete(l125,prefix,l127,&l253);l286 = (payload_t*)l253;if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n"),prefix[
0],prefix[1]));goto l287;}l159 = NULL;l144 = trie_find_lpm(l125,prefix,l127,&
l159);l261 = (payload_t*)l159;if(SOC_SUCCESS(l144)){payload_t*l288 = (
payload_t*)(l261->bkt_ptr);if(l288!= NULL){l284 = l288->len;}else{l284 = 0;}}
else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l289;}
sal_memcpy(l283,prefix,sizeof(prefix));do{if(!(l122)){l283[0] = (((32-l127) == 
32)?0:(l283[1])<<(32-l127));l283[1] = 0;}else{int l264 = 64-l127;int l265;if(
l264<32){l265 = l283[3]<<l264;l265|= (((32-l264) == 32)?0:(l283[4])>>(32-l264
));l283[0] = l283[4]<<l264;l283[1] = l265;l283[2] = l283[3] = l283[4] = 0;}
else{l283[1] = (((l264-32) == 32)?0:(l283[4])<<(l264-32));l283[0] = l283[2] = 
l283[3] = l283[4] = 0;}}}while(0);l144 = l118(l74,prefix,l284,l116,l99,&
lpm_entry,0,1);l184 = _soc_th_alpm_find(l74,&lpm_entry,l111,l186,&tcam_index,
&bktid,&l88,TRUE);(void)l115(l74,l186,l111,l99,l161,bktid,0,&lpm_entry);(void
)l118(l74,l283,l127,l116,l99,&lpm_entry,0,0);if(SOC_URPF_STATUS_GET(l74)){if(
SOC_SUCCESS(l144)){l187 = ((l122)?((uint32*)&(l191)):((uint32*)&(l189)));l184
= soc_mem_read(l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,
ALPM_ENT_INDEX(l88)),l187);}}if((l284 == 0)&&SOC_FAILURE(l184)){l285 = l122?
VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l116):VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l116);
sal_memcpy(&lpm_entry,l285,sizeof(lpm_entry));l144 = l118(l74,l283,l127,l116,
l99,&lpm_entry,0,1);}if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n"),prefix[0],prefix[1]));goto l289;}l285 = 
&lpm_entry;}else{l159 = NULL;l144 = trie_find_lpm(l125,prefix,l127,&l159);
l261 = (payload_t*)l159;if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"_soc_alpm_delete: Could not find default "
"route in the trie for vrf %d\n"),l116));goto l287;}l261->bkt_ptr = NULL;l284
= 0;l285 = l122?VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l116):
VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l116);}l144 = l98(l74,l285,l122,l100,l284,&
l102);if(SOC_FAILURE(l144)){goto l289;}l144 = _soc_th_alpm_aux_op(l74,
DELETE_PROPAGATE,&l102,TRUE,&l157,&tcam_index,&bktid);if(SOC_FAILURE(l144)){
goto l289;}if(SOC_URPF_STATUS_GET(l74)){uint32 l151;if(l187!= NULL){l151 = 
soc_mem_field32_get(l74,L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf);l151++;
soc_mem_field32_set(l74,L3_DEFIP_AUX_SCRATCHm,&l102,DB_TYPEf,l151);l151 = 
soc_mem_field32_get(l74,l111,l187,SRC_DISCARDf);soc_mem_field32_set(l74,l111,
&l102,SRC_DISCARDf,l151);l151 = soc_mem_field32_get(l74,l111,l187,
DEFAULTROUTEf);soc_mem_field32_set(l74,l111,&l102,DEFAULTROUTEf,l151);l144 = 
_soc_th_alpm_aux_op(l74,DELETE_PROPAGATE,&l102,TRUE,&l157,&tcam_index,&bktid)
;}if(SOC_FAILURE(l144)){goto l289;}}sal_free(l255);if(!l113){sal_free(l286);}
PIVOT_BUCKET_ENT_CNT_UPDATE(pivot_pyld);if((pivot_pyld->len!= 0)&&(trie->trie
== NULL)){uint32 l290[5];sal_memcpy(l290,pivot_pyld->key,sizeof(l290));do{if(
!(l99)){l290[0] = (((32-pivot_pyld->len) == 32)?0:(l290[1])<<(32-pivot_pyld->
len));l290[1] = 0;}else{int l264 = 64-pivot_pyld->len;int l265;if(l264<32){
l265 = l290[3]<<l264;l265|= (((32-l264) == 32)?0:(l290[4])>>(32-l264));l290[0
] = l290[4]<<l264;l290[1] = l265;l290[2] = l290[3] = l290[4] = 0;}else{l290[1
] = (((l264-32) == 32)?0:(l290[4])<<(l264-32));l290[0] = l290[2] = l290[3] = 
l290[4] = 0;}}}while(0);l118(l74,l290,pivot_pyld->len,l116,l99,&lpm_entry,0,1
);l144 = soc_th_alpm_lpm_delete(l74,&lpm_entry);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_delete: Unable to "
"delete pivot 0x%08x 0x%08x \n"),pivot_pyld->key[0],pivot_pyld->key[1]));}}
l144 = _soc_th_alpm_delete_in_bkt(l74,l111,l278,l146,l279,l75,&l88,l122);if(
SOC_FAILURE(l144)){SOC_ALPM_LPM_UNLOCK(l74);return l144;}if(
SOC_URPF_STATUS_GET(l74)){l144 = soc_mem_alpm_delete(l74,l111,
SOC_TH_ALPM_RPF_BKT_IDX(l74,ALPM_BKT_IDX(l278)),MEM_BLOCK_ALL,l146,l279,l75,&
l157);if(SOC_FAILURE(l144)){SOC_ALPM_LPM_UNLOCK(l74);return l144;}}
soc_th_alpm_bu_upd(l74,PIVOT_BUCKET_INDEX(pivot_pyld),PIVOT_TCAM_INDEX(
pivot_pyld),((pivot_pyld->len!= 0)&&(trie->trie == NULL))?TRUE:FALSE,l122,-1)
;if((pivot_pyld->len!= 0)&&(trie->trie == NULL)){l144 = soc_th_alpm_bs_free(
l74,PIVOT_BUCKET_INDEX(pivot_pyld),l116,l122);if(SOC_FAILURE(l144)){LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_delete: Unable to release"
"empty bucket: %d\n"),PIVOT_BUCKET_INDEX(pivot_pyld)));}l144 = trie_delete(
l158,pivot_pyld->key,pivot_pyld->len,&l253);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"could not delete pivot from pivot trie\n")))
;}trie_destroy(PIVOT_BUCKET_TRIE(pivot_pyld));sal_free(PIVOT_BUCKET_HANDLE(
pivot_pyld));sal_free(pivot_pyld);l88 = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,
ALPM_BKT_IDX(l278));_soc_tomahawk_alpm_bkt_view_set(l74,l88,INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l88 = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,
ALPM_BKT_IDX(l278)+1);_soc_tomahawk_alpm_bkt_view_set(l74,l88,INVALIDm);}}}
VRF_TRIE_ROUTES_DEC(l74,l116,l122);if(VRF_TRIE_ROUTES_CNT(l74,l116,l122) == 0
){l144 = l121(l74,l116,l122);}SOC_ALPM_LPM_UNLOCK(l74);return l144;l289:
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"recovering soc_th_alpm_vrf_delete failed\n ")));l184 = trie_insert(l125,
prefix,NULL,l127,(trie_node_t*)l286);if(SOC_FAILURE(l184)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"_soc_alpm_delete: Unable to reinsert"
"prefix 0x%08x 0x%08x into pfx trie\n"),prefix[0],prefix[1]));}l287:l184 = 
trie_insert(trie,prefix,NULL,l127,(trie_node_t*)l255);if(SOC_FAILURE(l184)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into bkt trie\n")
,prefix[0],prefix[1]));}SOC_ALPM_LPM_UNLOCK(l74);return l144;}void l291(int
l74){int l166;if(alpm_vrf_handle[l74]!= NULL){sal_free(alpm_vrf_handle[l74]);
alpm_vrf_handle[l74] = NULL;}if(tcam_pivot[l74]!= NULL){sal_free(tcam_pivot[
l74]);tcam_pivot[l74] = NULL;}if(bkt_usage[l74]!= NULL){sal_free(bkt_usage[
l74]);bkt_usage[l74] = NULL;}for(l166 = 0;l166<SOC_TH_MAX_ALPM_VIEWS;l166++){
if(global_bkt_usage[l74][l166]!= NULL){sal_free(global_bkt_usage[l74][l166]);
global_bkt_usage[l74][l166] = NULL;}}}int soc_th_alpm_init(int l74){int l166;
int l144 = SOC_E_NONE;uint32 l247;l144 = l94(l74);SOC_IF_ERROR_RETURN(l144);
l144 = l107(l74);l291(l74);alpm_vrf_handle[l74] = sal_alloc((MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t),"ALPM VRF Handles");if(alpm_vrf_handle[l74] == NULL
){l144 = SOC_E_MEMORY;goto l196;}tcam_pivot[l74] = sal_alloc(MAX_PIVOT_COUNT*
sizeof(alpm_pivot_t*),"ALPM pivots");if(tcam_pivot[l74] == NULL){l144 = 
SOC_E_MEMORY;goto l196;}sal_memset(alpm_vrf_handle[l74],0,(MAX_VRF_ID+1)*
sizeof(alpm_vrf_handle_t));sal_memset(tcam_pivot[l74],0,MAX_PIVOT_COUNT*
sizeof(alpm_pivot_t*));l247 = SOC_TH_MAX_ALPM_BUCKETS*sizeof(alpm_bkt_usg_t);
bkt_usage[l74] = sal_alloc(l247,"ALPM_BS");if(bkt_usage[l74] == NULL){l144 = 
SOC_E_MEMORY;goto l196;}sal_memset(bkt_usage[l74],0,l247);l247 = 
SOC_TH_MAX_BUCKET_ENTRIES*sizeof(alpm_bkt_bmp_t);for(l166 = 0;l166<
SOC_TH_MAX_ALPM_VIEWS;l166++){global_bkt_usage[l74][l166] = sal_alloc(l247,
"ALPM_BS");if(global_bkt_usage[l74][l166] == NULL){l144 = SOC_E_MEMORY;goto
l196;}sal_memset(global_bkt_usage[l74][l166],0,l247);}for(l166 = 0;l166<
MAX_PIVOT_COUNT;l166++){ALPM_TCAM_PIVOT(l74,l166) = NULL;}if(SOC_CONTROL(l74)
->alpm_bulk_retry == NULL){SOC_CONTROL(l74)->alpm_bulk_retry = sal_sem_create
("ALPM bulk retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l74)->alpm_lookup_retry
== NULL){SOC_CONTROL(l74)->alpm_lookup_retry = sal_sem_create(
"ALPM lookup retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l74)->alpm_insert_retry
== NULL){SOC_CONTROL(l74)->alpm_insert_retry = sal_sem_create(
"ALPM insert retry",sal_sem_BINARY,0);}if(SOC_CONTROL(l74)->alpm_delete_retry
== NULL){SOC_CONTROL(l74)->alpm_delete_retry = sal_sem_create(
"ALPM delete retry",sal_sem_BINARY,0);}l144 = soc_th_alpm_128_lpm_init(l74);
if(SOC_FAILURE(l144)){goto l196;}return l144;l196:l291(l74);return l144;}
static int l292(int l74){int l166,l144;alpm_pivot_t*l151;for(l166 = 0;l166<
MAX_PIVOT_COUNT;l166++){l151 = ALPM_TCAM_PIVOT(l74,l166);if(l151){if(
PIVOT_BUCKET_HANDLE(l151)){if(PIVOT_BUCKET_TRIE(l151)){l144 = trie_traverse(
PIVOT_BUCKET_TRIE(l151),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(
SOC_SUCCESS(l144)){trie_destroy(PIVOT_BUCKET_TRIE(l151));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"Unable to clear trie state for unit %d\n"),
l74));return l144;}}sal_free(PIVOT_BUCKET_HANDLE(l151));}sal_free(
ALPM_TCAM_PIVOT(l74,l166));ALPM_TCAM_PIVOT(l74,l166) = NULL;}}for(l166 = 0;
l166<= SOC_VRF_MAX(l74)+1;l166++){l144 = trie_traverse(VRF_PREFIX_TRIE_IPV4(
l74,l166),alpm_delete_node_cb,NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(
l144)){trie_destroy(VRF_PREFIX_TRIE_IPV4(l74,l166));}else{LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l74,l166));return l144;}
l144 = trie_traverse(VRF_PREFIX_TRIE_IPV6(l74,l166),alpm_delete_node_cb,NULL,
_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l144)){trie_destroy(
VRF_PREFIX_TRIE_IPV6(l74,l166));}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"unit: %d Unable to clear v4 pfx trie for vrf %d\n"),l74,l166));return
l144;}if(VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l166)!= NULL){sal_free(
VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l166));}if(VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,
l166)!= NULL){sal_free(VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l166));}sal_memset(&
alpm_vrf_handle[l74][l166],0,sizeof(alpm_vrf_handle_t));}sal_memset(&
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
lpm_entry,l294;int l295 = 0;int index;int l144 = SOC_E_NONE;uint32 key[2] = {
0,0};uint32 l127;alpm_bucket_handle_t*l256;alpm_pivot_t*pivot_pyld;payload_t*
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
soc_L3_DEFIPm_field32_set(l74,lpm_entry,DEFAULT_MISS0f,1);}l144 = 
soc_th_alpm_bs_alloc(l74,&l295,l116,l122);if(SOC_FAILURE(l144)){return l144;}
soc_L3_DEFIPm_field32_set(l74,lpm_entry,ALG_BKT_PTR0f,ALPM_BKT_IDX(l295));
soc_L3_DEFIPm_field32_set(l74,lpm_entry,ALG_SUB_BKT_PTR0f,ALPM_BKT_SIDX(l295)
);sal_memcpy(&l294,lpm_entry,sizeof(l294));l144 = l96(l74,&l294,&index);if(
SOC_FAILURE(l144)){return l144;}l256 = sal_alloc(sizeof(alpm_bucket_handle_t)
,"ALPM Bucket Handle");if(l256 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U
(l74,"soc_th_alpm_vrf_add: Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l256,0,sizeof(*l256));
pivot_pyld = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(
pivot_pyld == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n")));
sal_free(l256);return SOC_E_MEMORY;}l286 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l286 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_vrf_add: Unable to allocate memory for "
"pfx trie node \n")));sal_free(l256);sal_free(pivot_pyld);return SOC_E_MEMORY
;}sal_memset(pivot_pyld,0,sizeof(*pivot_pyld));sal_memset(l286,0,sizeof(*l286
));l127 = 0;PIVOT_BUCKET_HANDLE(pivot_pyld) = l256;if(l122){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(pivot_pyld));}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(pivot_pyld));}PIVOT_BUCKET_INDEX(
pivot_pyld) = l295;PIVOT_BUCKET_VRF(pivot_pyld) = l116;PIVOT_BUCKET_IPV6(
pivot_pyld) = l122;PIVOT_BUCKET_DEF(pivot_pyld) = TRUE;pivot_pyld->key[0] = 
l286->key[0] = key[0];pivot_pyld->key[1] = l286->key[1] = key[1];pivot_pyld->
len = l286->len = l127;l144 = trie_insert(l296,key,NULL,l127,&(l286->node));
if(SOC_FAILURE(l144)){sal_free(l286);sal_free(pivot_pyld);sal_free(l256);
return l144;}l144 = trie_insert(l297,key,NULL,l127,(trie_node_t*)pivot_pyld);
if(SOC_FAILURE(l144)){trie_node_t*l253 = NULL;(void)trie_delete(l296,key,l127
,&l253);sal_free(l286);sal_free(pivot_pyld);sal_free(l256);return l144;}index
= soc_th_alpm_physical_idx(l74,L3_DEFIPm,index,l122);if(l122 == 0){
ALPM_TCAM_PIVOT(l74,index) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld) = index;
}else{ALPM_TCAM_PIVOT(l74,index<<1) = pivot_pyld;PIVOT_TCAM_INDEX(pivot_pyld)
= index<<1;}VRF_PIVOT_REF_INC(l74,l116,l122);VRF_TRIE_INIT_DONE(l74,l116,l122
,1);return l144;}static int l121(int l74,int l116,int l122){defip_entry_t*
lpm_entry;int l147;int l144 = SOC_E_NONE;uint32 key[2] = {0,0},l138[
SOC_MAX_MEM_FIELD_WORDS];payload_t*l255;alpm_pivot_t*l298;trie_node_t*l253;
trie_t*l296;trie_t*l297 = NULL;soc_mem_t l111;int tcam_index,bktid,index;
uint32 l163[SOC_MAX_MEM_FIELD_WORDS];if(l122 == 0){lpm_entry = 
VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l116);l111 = L3_DEFIP_ALPM_IPV4m;}else{
lpm_entry = VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l116);l111 = 
L3_DEFIP_ALPM_IPV6_64m;}l144 = _soc_th_alpm_find(l74,lpm_entry,l111,l163,&
tcam_index,&bktid,&index,TRUE);l144 = soc_th_alpm_bs_free(l74,bktid,l116,l122
);index = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,ALPM_BKT_IDX(bktid));
_soc_tomahawk_alpm_bkt_view_set(l74,index,INVALIDm);if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){index = SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,
ALPM_BKT_IDX(bktid)+1);_soc_tomahawk_alpm_bkt_view_set(l74,index,INVALIDm);}
l144 = l106(l74,lpm_entry,(void*)l138,&l147);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"soc_alpm_vrf_delete: unable to get internal"
" pivot idx for vrf %d/%d\n"),l116,l122));l147 = -1;}l147 = 
soc_th_alpm_physical_idx(l74,L3_DEFIPm,l147,l122);if(l122 == 0){l298 = 
ALPM_TCAM_PIVOT(l74,l147);}else{l298 = ALPM_TCAM_PIVOT(l74,l147<<1);}l144 = 
soc_th_alpm_lpm_delete(l74,lpm_entry);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal default for vrf %d/%d\n"),l116,l122));}sal_free(lpm_entry);if(
l122 == 0){VRF_TRIE_DEFAULT_ROUTE_IPV4(l74,l116) = NULL;l296 = 
VRF_PREFIX_TRIE_IPV4(l74,l116);VRF_PREFIX_TRIE_IPV4(l74,l116) = NULL;}else{
VRF_TRIE_DEFAULT_ROUTE_IPV6(l74,l116) = NULL;l296 = VRF_PREFIX_TRIE_IPV6(l74,
l116);VRF_PREFIX_TRIE_IPV6(l74,l116) = NULL;}VRF_TRIE_INIT_DONE(l74,l116,l122
,0);l144 = trie_delete(l296,key,0,&l253);l255 = (payload_t*)l253;if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Unable to delete internal default for vrf "" %d/%d\n"),l116,l122));}sal_free
(l255);(void)trie_destroy(l296);if(l122 == 0){l297 = VRF_PIVOT_TRIE_IPV4(l74,
l116);VRF_PIVOT_TRIE_IPV4(l74,l116) = NULL;}else{l297 = VRF_PIVOT_TRIE_IPV6(
l74,l116);VRF_PIVOT_TRIE_IPV6(l74,l116) = NULL;}l253 = NULL;l144 = 
trie_delete(l297,key,0,&l253);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM
,(BSL_META_U(l74,"Unable to delete internal pivot node for vrf"" %d/%d\n"),
l116,l122));}(void)trie_destroy(l297);(void)trie_destroy(PIVOT_BUCKET_TRIE(
l298));sal_free(PIVOT_BUCKET_HANDLE(l298));sal_free(l298);return l144;}int
soc_th_alpm_insert(int l74,void*l97,uint32 l112,int l212,int l299){
defip_alpm_ipv4_entry_t l188,l189;defip_alpm_ipv6_64_entry_t l190,l191;
soc_mem_t l111;void*l186,*l279;int l161,l116;int index;int l99;int l144 = 
SOC_E_NONE;uint32 l113;int l114 = 0;l99 = soc_mem_field32_get(l74,L3_DEFIPm,
l97,MODE0f);l111 = (l99)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l186 = ((
l99)?((uint32*)&(l190)):((uint32*)&(l188)));l279 = ((l99)?((uint32*)&(l191)):
((uint32*)&(l189)));if(l212!= -1){l114 = ALPM_BKTID(ALPM_BKT_IDX(
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l74,l212)),(l212>>ALPM_ENT_INDEX_BITS));}
SOC_IF_ERROR_RETURN(l108(l74,l97,l186,l279,l111,l112,&l113,l114));
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,l97,&l161,&l116));if(
ALPM_PREFIX_IN_TCAM(l74,l161)){l144 = l96(l74,l97,&index);if(SOC_SUCCESS(l144
)){if(l161 == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_REF_INC(l74,MAX_VRF_ID,l99);
VRF_TRIE_ROUTES_INC(l74,MAX_VRF_ID,l99);}else{VRF_PIVOT_REF_INC(l74,l116,l99)
;VRF_TRIE_ROUTES_INC(l74,l116,l99);}}else if(l144 == SOC_E_FULL){if(l161 == 
SOC_L3_VRF_OVERRIDE){VRF_PIVOT_FULL_INC(l74,MAX_VRF_ID,l99);}else{
VRF_PIVOT_FULL_INC(l74,l116,l99);}}return(l144);}else if(l116 == 0){if(
soc_th_alpm_mode_get(l74) == SOC_ALPM_MODE_PARALLEL||soc_th_alpm_mode_get(l74
) == SOC_ALPM_MODE_TCAM_ALPM){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"VRF=0 cannot be added in Parallel mode\n")));return SOC_E_PARAM;}}if(l161!= 
SOC_L3_VRF_GLOBAL){if(soc_th_alpm_mode_get(l74) == SOC_ALPM_MODE_COMBINED){if
(VRF_TRIE_ROUTES_CNT(l74,l116,l99) == 0){if(!l113){LOG_ERROR(BSL_LS_SOC_ALPM,
(BSL_META_U(l74,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n"),l161));return SOC_E_PARAM;}}}}if(!
VRF_TRIE_INIT_COMPLETED(l74,l116,l99)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_insert:VRF %d is not ""initialized\n"),l116));l144 = 
l293(l74,l116,l99);if(SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_insert:VRF %d/%d trie init \n""failed\n"),l116,l99))
;return l144;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_insert:VRF %d/%d trie init ""completed\n"),l116,l99));}if(l299&
SOC_ALPM_LOOKUP_HIT){l144 = l162(l74,l97,l186,l279,l111,l212);}else{if(l212 == 
-1){l212 = 0;}l212 = ALPM_BKTID(ALPM_BKT_IDX(SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l74
,l212)),l212>>ALPM_ENT_INDEX_BITS);l144 = l271(l74,l97,l111,l186,l279,&index,
l212,l299);}if(l144!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"unit %d :soc_alpm_insert: Route Insertion Failed :%s\n"),l74,soc_errmsg(l144
)));}return(l144);}int soc_th_alpm_lookup(int l74,void*key_data,void*l75,int*
l104,int*l300){defip_alpm_ipv4_entry_t l188;defip_alpm_ipv6_64_entry_t l190;
soc_mem_t l111;int bktid = 0;int tcam_index;void*l186;int l161,l116;int l99,
l87;int l144 = SOC_E_NONE;l99 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,
MODE0f);SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l161,&l116)
);l144 = l103(l74,key_data,l75,l104,&l87,&l99);if(SOC_SUCCESS(l144)){if(!l99
&&(*l104&0x1)){l144 = soc_th_alpm_lpm_ip4entry1_to_0(l74,l75,l75,PRESERVE_HIT
);}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,l75,&l161,&l116));if(
ALPM_PREFIX_IN_TCAM(l74,l161)){return SOC_E_NONE;}}if(!
VRF_TRIE_INIT_COMPLETED(l74,l116,l99)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"soc_alpm_lookup:VRF %d is not initialized\n"),l116));*l300 = 
0;return SOC_E_NOT_FOUND;}l111 = (l99)?L3_DEFIP_ALPM_IPV6_64m:
L3_DEFIP_ALPM_IPV4m;l186 = ((l99)?((uint32*)&(l190)):((uint32*)&(l188)));
SOC_ALPM_LPM_LOCK(l74);l144 = _soc_th_alpm_find(l74,key_data,l111,l186,&
tcam_index,&bktid,l104,TRUE);SOC_ALPM_LPM_UNLOCK(l74);if(SOC_FAILURE(l144)){*
l300 = tcam_index;*l104 = (ALPM_BKT_SIDX(bktid)<<ALPM_ENT_INDEX_BITS)|
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,bktid);return l144;}l144 = l115(l74,l186,
l111,l99,l161,bktid,*l104,l75);*l300 = SOC_ALPM_LOOKUP_HIT|tcam_index;*l104 = 
(ALPM_BKT_SIDX(bktid)<<ALPM_ENT_INDEX_BITS)|*l104;return(l144);}static int
l301(int l74,void*key_data,void*l75,int l116,int*tcam_index,int*bucket_index,
int*l88,int l302){int l144 = SOC_E_NONE;int l166,l303,l122,l157 = 0;uint32
l100,l146;defip_aux_scratch_entry_t l102;int l304,l305;int index;soc_mem_t
l111,l178;int l306,l307;int l308;uint32 l309[SOC_MAX_MEM_FIELD_WORDS] = {0};
int l310 = -1;int l311 = 0;soc_field_t l312[2] = {IP_ADDR0f,IP_ADDR1f,};l178 = 
L3_DEFIPm;l122 = soc_mem_field32_get(l74,l178,key_data,MODE0f);l304 = 
soc_mem_field32_get(l74,l178,key_data,GLOBAL_ROUTE0f);l305 = 
soc_mem_field32_get(l74,l178,key_data,VRF_ID_0f);LOG_VERBOSE(BSL_LS_SOC_ALPM,
(BSL_META_U(l74,"Prefare AUX Scratch for searching TCAM in "
"%s region, Key data: v6 %d global %d vrf %d:\n"),l116 == SOC_L3_VRF_GLOBAL?
"Global":"VRF",l122,l304,l305));if(l116 == SOC_L3_VRF_GLOBAL){l100 = l302?1:0
;soc_mem_field32_set(l74,l178,key_data,GLOBAL_ROUTE0f,1);soc_mem_field32_set(
l74,l178,key_data,VRF_ID_0f,0);}else{l100 = l302?3:2;}
soc_th_alpm_bank_db_type_get(l74,l116 == SOC_L3_VRF_GLOBAL?(SOC_VRF_MAX(l74)+
1):l116,&l146,NULL);sal_memset(&l102,0,sizeof(defip_aux_scratch_entry_t));
SOC_IF_ERROR_RETURN(l98(l74,key_data,l122,l100,0,&l102));if(l116 == 
SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l74,l178,key_data,GLOBAL_ROUTE0f,l304)
;soc_mem_field32_set(l74,l178,key_data,VRF_ID_0f,l305);}SOC_IF_ERROR_RETURN(
_soc_th_alpm_aux_op(l74,PREFIX_LOOKUP,&l102,TRUE,&l157,tcam_index,
bucket_index));if(l157 == 0){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Could not find bucket\n")));return SOC_E_NOT_FOUND;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"Hit in memory %s, index %d, "
"bucket [%d,%d]\n"),SOC_MEM_NAME(l74,l178),soc_th_alpm_logical_idx(l74,l178,(
*tcam_index)>>1,1),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index)))
;l111 = (l122)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l144 = l143(l74,
key_data,&l307);if(SOC_FAILURE(l144)){return l144;}switch(l122){case
L3_DEFIP_MODE_V4:l308 = ALPM_IPV4_BKT_COUNT;break;case L3_DEFIP_MODE_64:l308 = 
ALPM_IPV6_64_BKT_COUNT;break;default:l308 = ALPM_IPV4_BKT_COUNT;break;}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l308<<= 1;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"Start searching mem %s bucket [%d,%d](count %d) "
"for Length %d\n"),SOC_MEM_NAME(l74,l111),ALPM_BKT_IDX(*bucket_index),
ALPM_BKT_SIDX(*bucket_index),l308,l307));for(l166 = 0;l166<l308;l166++){
uint32 l186[SOC_MAX_MEM_FIELD_WORDS] = {0};uint32 l277[2] = {0};uint32 l313[2
] = {0};uint32 l314[2] = {0};int l315;l144 = _soc_th_alpm_mem_index(l74,l111,
ALPM_BKT_IDX(*bucket_index),l166,l146,&index);if(l144 == SOC_E_FULL){continue
;}SOC_IF_ERROR_RETURN(soc_mem_read(l74,l111,MEM_BLOCK_ANY,index,(void*)l186))
;l315 = soc_mem_field32_get(l74,l111,l186,VALIDf);l306 = soc_mem_field32_get(
l74,l111,l186,LENGTHf);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Bucket [%d,%d] index %6d: Valid %d, Length %d\n"),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),index,l315,l306));if(!l315||(l306>l307)){
continue;}SHR_BITSET_RANGE(l277,(l122?64:32)-l306,l306);(void)
soc_mem_field_get(l74,l111,(uint32*)l186,KEYf,(uint32*)l313);l314[1] = 
soc_mem_field32_get(l74,l178,key_data,l312[1]);l314[0] = soc_mem_field32_get(
l74,l178,key_data,l312[0]);LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tmask %08x %08x\n\t key %08x %08x\n""\thost %08x %08x\n"),l277[1],l277[0],
l313[1],l313[0],l314[1],l314[0]));for(l303 = l122?1:0;l303>= 0;l303--){if((
l314[l303]&l277[l303])!= (l313[l303]&l277[l303])){break;}}if(l303>= 0){
continue;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Found a match in mem %s bucket [%d,%d], ""index %d\n"),SOC_MEM_NAME(l74,l111
),ALPM_BKT_IDX(*bucket_index),ALPM_BKT_SIDX(*bucket_index),index));if(l310 == 
-1||l310<l306){l310 = l306;l311 = index;sal_memcpy(l309,l186,sizeof(l186));}}
if(l310!= -1){l144 = l115(l74,l309,l111,l122,l116,*bucket_index,l311,l75);if(
SOC_SUCCESS(l144)){*l88 = l311;if(bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l74)){LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Hit mem %s bucket [%d,%d], index %d\n"),SOC_MEM_NAME(l74,l111),ALPM_BKT_IDX(
*bucket_index),ALPM_BKT_SIDX(*bucket_index),l311));}}return l144;}*l88 = 
soc_th_alpm_logical_idx(l74,l178,(*tcam_index)>>1,1);LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Miss in mem %s bucket [%d,%d], use associate data "
"in mem %s LOG index %d\n"),SOC_MEM_NAME(l74,l111),ALPM_BKT_IDX(*bucket_index
),ALPM_BKT_SIDX(*bucket_index),SOC_MEM_NAME(l74,l178),*l88));
SOC_IF_ERROR_RETURN(soc_mem_read(l74,l178,MEM_BLOCK_ANY,*l88,(void*)l75));if(
(!l122)&&((*tcam_index)&1)){l144 = soc_th_alpm_lpm_ip4entry1_to_0(l74,l75,l75
,PRESERVE_HIT);}return SOC_E_NONE;}int soc_th_alpm_find_best_match(int l74,
void*key_data,void*l75,int*l104,int l302){int l144 = SOC_E_NONE;int l166,l316
,l170;defip_entry_t l317;uint32 l318[2];uint32 l313[2];uint32 l319[2];uint32
l314[2];uint32 l320,l321;int l161,l116 = 0;int l322[2] = {0};int tcam_index,
bucket_index;soc_mem_t l178 = L3_DEFIPm;int l217,l122,l323,l324 = 0;
soc_field_t l325[] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f};soc_field_t l326[] = {
GLOBAL_ROUTE0f,GLOBAL_ROUTE1f};l122 = soc_mem_field32_get(l74,l178,key_data,
MODE0f);if(!SOC_URPF_STATUS_GET(l74)&&l302){return SOC_E_PARAM;}l316 = 
soc_mem_index_min(l74,l178);l170 = soc_mem_index_count(l74,l178);if(
SOC_URPF_STATUS_GET(l74)){l170>>= 1;}if(soc_th_alpm_mode_get(l74) == 
SOC_ALPM_MODE_PARALLEL||soc_th_alpm_mode_get(l74) == SOC_ALPM_MODE_TCAM_ALPM)
{l170>>= 1;l316+= l170;}if(l302){l316+= soc_mem_index_count(l74,l178)/2;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Launch LPM searching from index %d count %d\n"),l316,l170));for(l166 = l316;
l166<l316+l170;l166++){SOC_IF_ERROR_RETURN(soc_mem_read(l74,l178,
MEM_BLOCK_ANY,l166,(void*)&l317));l322[0] = soc_mem_field32_get(l74,l178,&
l317,VALID0f);l322[1] = soc_mem_field32_get(l74,l178,&l317,VALID1f);if(l322[0
] == 0&&l322[1] == 0){continue;}l323 = soc_mem_field32_get(l74,l178,&l317,
MODE0f);if(l323!= l122){continue;}for(l217 = 0;l217<(l122?1:2);l217++){if(
l322[l217] == 0){continue;}l320 = soc_mem_field32_get(l74,l178,&l317,l325[
l217]);l321 = soc_mem_field32_get(l74,l178,&l317,l326[l217]);if(!(
soc_th_alpm_mode_get(l74)!= SOC_ALPM_MODE_TCAM_ALPM&&l320&&l321)&&!(
soc_th_alpm_mode_get(l74) == SOC_ALPM_MODE_TCAM_ALPM&&l321)){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Match a Global High route: ent %d\n"),l217));l318[0] = soc_mem_field32_get(
l74,l178,&l317,IP_ADDR_MASK0f);l318[1] = soc_mem_field32_get(l74,l178,&l317,
IP_ADDR_MASK1f);l313[0] = soc_mem_field32_get(l74,l178,&l317,IP_ADDR0f);l313[
1] = soc_mem_field32_get(l74,l178,&l317,IP_ADDR1f);l319[0] = 
soc_mem_field32_get(l74,l178,key_data,IP_ADDR_MASK0f);l319[1] = 
soc_mem_field32_get(l74,l178,key_data,IP_ADDR_MASK1f);l314[0] = 
soc_mem_field32_get(l74,l178,key_data,IP_ADDR0f);l314[1] = 
soc_mem_field32_get(l74,l178,key_data,IP_ADDR1f);LOG_VERBOSE(BSL_LS_SOC_ALPM,
(BSL_META_U(l74,"\thmsk %08x %08x\n\thkey %08x %08x\n"
"\tsmsk %08x %08x\n\tskey %08x %08x\n"),l318[1],l318[0],l313[1],l313[0],l319[
1],l319[0],l314[1],l314[0]));if(l122&&(((l318[1]&l319[1])!= l318[1])||((l318[
0]&l319[0])!= l318[0]))){continue;}if(!l122&&((l318[l217]&l319[0])!= l318[
l217])){continue;}if(l122&&((l314[0]&l318[0]) == (l313[0]&l318[0]))&&((l314[1
]&l318[1]) == (l313[1]&l318[1]))){l324 = TRUE;break;}if(!l122&&((l314[0]&l318
[l217]) == (l313[l217]&l318[l217]))){l324 = TRUE;break;}}if(!l324){continue;}
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Hit Global High route in index = %d(%d)\n"),l166,l217));sal_memcpy(l75,&l317
,sizeof(l317));if(!l122&&l217 == 1){l144 = soc_th_alpm_lpm_ip4entry1_to_0(l74
,l75,l75,PRESERVE_HIT);}*l104 = l166;return l144;}LOG_VERBOSE(BSL_LS_SOC_ALPM
,(BSL_META_U(l74,"Global high lookup miss, use AUX engine to "
"search for VRF and Global Low routes\n")));SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(l74,key_data,&l161,&l116));l144 = l301(l74,key_data,
l75,l116,&tcam_index,&bucket_index,l104,l302);if(l144 == SOC_E_NOT_FOUND){
l116 = SOC_L3_VRF_GLOBAL;LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Not found in VRF region, trying Global ""region\n")));l144 = l301(l74,
key_data,l75,l116,&tcam_index,&bucket_index,l104,l302);}if(SOC_SUCCESS(l144))
{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"Hit in %s region in TCAM index %d, ""bucket [%d,%d]\n"),l116 == 
SOC_L3_VRF_GLOBAL?"Global Low":"VRF",tcam_index,ALPM_BKT_IDX(bucket_index),
ALPM_BKT_SIDX(bucket_index)));}else{LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"Search miss for given address\n")));}return(l144);}int soc_th_alpm_delete
(int l74,void*key_data,int l212,int l299){int l327;int l161,l116;int l99;int
l144 = SOC_E_NONE;l99 = soc_mem_field32_get(l74,L3_DEFIPm,key_data,MODE0f);
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,key_data,&l161,&l116));if(
ALPM_PREFIX_IN_TCAM(l74,l161)){l144 = soc_th_alpm_lpm_delete(l74,key_data);if
(SOC_SUCCESS(l144)){if(l161 == SOC_L3_VRF_OVERRIDE){VRF_PIVOT_REF_DEC(l74,
MAX_VRF_ID,l99);VRF_TRIE_ROUTES_DEC(l74,MAX_VRF_ID,l99);}else{
VRF_PIVOT_REF_DEC(l74,l116,l99);VRF_TRIE_ROUTES_DEC(l74,l116,l99);}}return(
l144);}else{if(!VRF_TRIE_INIT_COMPLETED(l74,l116,l99)){LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_delete:VRF %d/%d is not initialized\n"),l116,l99));return SOC_E_NONE
;}if(l212 == -1){l212 = 0;}l327 = l212;l212 = ALPM_BKTID(ALPM_BKT_IDX(
SOC_TH_ALPM_BKT_ENTRY_TO_IDX(l74,l327)),l327>>ALPM_ENT_INDEX_BITS);l144 = 
l280(l74,key_data,l212,l299&~SOC_ALPM_LOOKUP_HIT,l327);}return(l144);}static
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
l74) == SOC_ALPM_MODE_PARALLEL&&SOC_URPF_STATUS_GET(l74)){
SOC_TH_ALPM_GLB_BUCKET_BMAP(l74) = sal_alloc(SOC_TH_ALPM_BUCKET_BMAP_SIZE(l74
),"alpm_glb_shr_bkt_bmap");if(SOC_TH_ALPM_GLB_BUCKET_BMAP(l74) == NULL){
sal_free(SOC_TH_ALPM_VRF_BUCKET_BMAP(l74));return SOC_E_MEMORY;}sal_memset(
SOC_TH_ALPM_GLB_BUCKET_BMAP(l74),0,SOC_TH_ALPM_BUCKET_BMAP_SIZE(l74));
soc_th_alpm_bucket_assign(l74,&l328,SOC_VRF_MAX(l74)+1,1);}else{
SOC_TH_ALPM_GLB_BUCKET_BMAP(l74) = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74);}return
SOC_E_NONE;}int soc_th_alpm_bucket_assign(int l74,int*l295,int l116,int l122)
{int l166,l219 = 1,l220 = 0;SHR_BITDCL*l329 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74
);if(l116 == SOC_VRF_MAX(l74)+1){l329 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l74);}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l219 = 2;}for(l166 = 0;l166<
SOC_TH_ALPM_BUCKET_COUNT(l74);l166+= l219){SHR_BITTEST_RANGE(l329,l166,l219,
l220);if(0 == l220){break;}}if(l166 == SOC_TH_ALPM_BUCKET_COUNT(l74)){return
SOC_E_FULL;}SHR_BITSET_RANGE(l329,l166,l219);*l295 = l166;
SOC_TH_ALPM_BUCKET_NEXT_FREE(l74) = l166;return SOC_E_NONE;}int
soc_th_alpm_bucket_release(int l74,int l295,int l116,int l122){int l219 = 1,
l220 = 0;SHR_BITDCL*l329 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74);if((l295<1)||(
l295>SOC_TH_ALPM_BUCKET_MAX_INDEX(l74))){return SOC_E_PARAM;}if(l116 == 
SOC_VRF_MAX(l74)+1){l329 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l74);}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l219 = 2;}SHR_BITTEST_RANGE(l329,l295,l219
,l220);if(!l220){return SOC_E_PARAM;}SHR_BITCLR_RANGE(l329,l295,l219);return
SOC_E_NONE;}int soc_th_alpm_bucket_is_assigned(int l74,int l330,int l116,int
l99,int*l220){int l219 = 1;SHR_BITDCL*l329 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74)
;if(l116 == SOC_VRF_MAX(l74)+1){l329 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l74);}if((
l330<1)||(l330>SOC_TH_ALPM_BUCKET_MAX_INDEX(l74))){return SOC_E_PARAM;}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l99)){l219 = 2;}SHR_BITTEST_RANGE(l329,l330,l219,
*l220);return SOC_E_NONE;}static void l73(int l74,void*l75,int index,l68 l76)
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
static int l333(l68 l70,l68 l71){int l147;for(l147 = 0;l147<5;l147++){if(l70[
l147]<l71[l147]){return-1;}if(l70[l147]>l71[l147]){return 1;}}return(0);}
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
,uint32 l335){l68 l336;int l87 = -1;int l144;uint16 index;if(
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
l336[4] = 0;};index = l335;}l144 = l92((l72[(l74)]),l333,l336,l87,index);if(
SOC_FAILURE(l144)){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\ndel  index: H %d error %d\n"),index,l144));}}static int l338(int l74,void*
key_data,int l87,int*l88){l68 l336;int l339;int l144;uint16 index = (0xFFFF);
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
l336[4] = 0;};}l144 = l84((l72[(l74)]),l333,l336,l87,&index);if(SOC_FAILURE(
l144)){*l88 = 0xFFFFFFFF;return(l144);}*l88 = index;return(SOC_E_NONE);}
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
,void*l344,int l345){uint32 l145;l145 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l39));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l39),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l29));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l29),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l25));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l25),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l27));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l27),(l145));if(((l61[(l74)]->l15)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l15));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l15),(l145));}if(((l61[(l74)]->l17)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l17));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l17),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l19));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l19),(l145));}else{l145 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l33));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l33),(l145));}l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l35));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l35),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l37));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l37),(l145));if(((l61[(l74)]->l41)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l41));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l41),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l43));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l43),(l145));}if(((l61[(l74)]->l13)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l13));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l13),(l145));}if(l345){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l23));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l23),(l145));}l145 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l45));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l45),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l47));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l47),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l49),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l51));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l51),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l53));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l53),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l55));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l55),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l57));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l57),(l145));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry1_to_1(
int l74,void*l343,void*l344,int l345){uint32 l145;l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l40));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l40),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l30));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l30),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l26));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l26),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l28));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l28),(l145));if(((l61[(l74)]->l16)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l16));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l16),(l145));}if(((l61[(l74)]->l18)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l18));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l18),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l20));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l20),(l145));}else{l145 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l34));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l34),(l145));}l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l36));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l36),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l38));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l38),(l145));if(((l61[(l74)]->l42)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l42));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l42),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l44));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l44),(l145));}if(((l61[(l74)]->l14)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l14));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l14),(l145));}if(l345){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l24));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l24),(l145));}l145 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l46));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l46),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l48));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l48),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l50),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l52));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l52),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l54));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l54),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l56),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l58));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l58),(l145));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry0_to_1(
int l74,void*l343,void*l344,int l345){uint32 l145;l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l39));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l40),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l29));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l30),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l25));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l26),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l27));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l28),(l145));if(!SOC_IS_HURRICANE(l74)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l15));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l16),(l145));}if(((l61[(l74)]->l17)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l17));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l18),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l19));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l20),(l145));}else{l145 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l33));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l34),(l145));}l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l35));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l36),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l37));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l38),(l145));if(((l61[(l74)]->l41)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l41));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l42),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l43));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l44),(l145));}if(((l61[(l74)]->l13)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l13));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l14),(l145));}if(l345){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l23));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l24),(l145));}l145 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l45));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l46),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l47));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l48),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l49));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l50),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l51));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l52),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l53));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l54),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l55));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l56),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l57));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l58),(l145));return(SOC_E_NONE);}int soc_th_alpm_lpm_ip4entry1_to_0(
int l74,void*l343,void*l344,int l345){uint32 l145;l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l40));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l39),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l30));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l29),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l26));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l25),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l28));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l27),(l145));if(!SOC_IS_HURRICANE(l74)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l16));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l15),(l145));}if(((l61[(l74)]->l18)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l18));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l17),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l20));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l19),(l145));}else{l145 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l34));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l33),(l145));}l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l36));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l35),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l38));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l37),(l145));if(((l61[(l74)]->l42)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l42));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l41),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l44));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l43),(l145));}if(((l61[(l74)]->l14)!= NULL)){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l14));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l13),(l145));}if(l345){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l24));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l23),(l145));}l145 = soc_meminfo_fieldinfo_field32_get(
(&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l46));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l45),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l48));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l47),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l50));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l49),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l52));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l51),(l145));l145 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l343),(l61[(l74)]->l54));soc_meminfo_fieldinfo_field32_set((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[(l74)]->l53),(l145));l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[
(l74)]->l56));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)
),(l344),(l61[(l74)]->l55),(l145));l145 = soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l74,L3_DEFIPm)),(l343),(l61[(l74)]->l58));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l344),(l61[
(l74)]->l57),(l145));return(SOC_E_NONE);}static int l346(int l74,void*l75){
return(SOC_E_NONE);}void soc_th_alpm_lpm_state_dump(int l74){int l166;int l347
;l347 = ((3*(64+32+2+1))-1);if(!bsl_check(bslLayerSoc,bslSourceAlpm,
bslSeverityVerbose,l74)){return;}for(l166 = l347;l166>= 0;l166--){if((l166!= 
((3*(64+32+2+1))-1))&&((l9[(l74)][(l166)].l2) == -1)){continue;}LOG_VERBOSE(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"),l166,(l9[
(l74)][(l166)].l4),(l9[(l74)][(l166)].next),(l9[(l74)][(l166)].l2),(l9[(l74)]
[(l166)].l3),(l9[(l74)][(l166)].l5),(l9[(l74)][(l166)].l6)));}
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
L3_DEFIPm)),(l75),(l61[(l74)]->l38),(l352));}}return l232(l74,MEM_BLOCK_ANY,
index+l349,index,l75);}static int l353(int l74,int l354,int l355){uint32 l75[
SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l74,MEM_BLOCK_ANY
,l354,l75));l334(l74,l75,l355,0x4000,0);SOC_IF_ERROR_RETURN(l232(l74,
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
0)){l334(l74,l75,l355,0x4000,0);SOC_IF_ERROR_RETURN(l232(l74,MEM_BLOCK_ANY,
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
MAX_VRF_ID,l99);l334(l74,l75,l362,0x4000,0);SOC_IF_ERROR_RETURN(l232(l74,
MEM_BLOCK_ANY,l362,l354,l75));SOC_IF_ERROR_RETURN(l348(l74,l362,l75));do{int
l356 = soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l354),1)*2;int l357 = 
soc_th_alpm_physical_idx((l74),L3_DEFIPm,(l362),1)*2;if((l360)){l356++;}
ALPM_TCAM_PIVOT((l74),l357) = ALPM_TCAM_PIVOT((l74),l356);if(ALPM_TCAM_PIVOT(
(l74),l357)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT((l74),l357)) = l357;
soc_th_alpm_lpm_move_bu_upd(l74,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT((l74),l357
)),l357);}ALPM_TCAM_PIVOT((l74),l356) = NULL;}while(0);}else{l334(l74,l75,
l355,0x4000,0);SOC_IF_ERROR_RETURN(l232(l74,MEM_BLOCK_ANY,l355,l354,l75));
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
l365;int l366;int l367;int l368;int l354;uint32 l359,l360;int l144;if((l9[(
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
(l9[(l74)][(l87)].l2);if((l144 = READ_L3_DEFIPm(l74,MEM_BLOCK_ANY,l354,l75))<
0){return l144;}l359 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
L3_DEFIPm)),(l75),(l61[(l74)]->l39));l360 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l74,L3_DEFIPm)),(l75),(l61[(l74)]->l40));if((l359 == 0)||(
l360 == 0)){*l364 = (l354<<1)+((l360 == 0)?1:0);return(SOC_E_NONE);}l354 = (
l9[(l74)][(l87)].l3);if((l144 = READ_L3_DEFIPm(l74,MEM_BLOCK_ANY,l354,l75))<0
){return l144;}l359 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,
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
SOC_MAX_MEM_FIELD_WORDS];void*l374;int l144;int l375,l128;l354 = (l9[(l74)][(
l87)].l3);l355 = l370;if(!l99){l355>>= 1;if((l144 = READ_L3_DEFIPm(l74,
MEM_BLOCK_ANY,l354,l371))<0){return l144;}if((l144 = READ_L3_DEFIP_AUX_TABLEm
(l74,MEM_BLOCK_ANY,soc_th_alpm_physical_idx(l74,L3_DEFIPm,l354,1),l372))<0){
return l144;}if((l144 = READ_L3_DEFIP_AUX_TABLEm(l74,MEM_BLOCK_ANY,
soc_th_alpm_physical_idx(l74,L3_DEFIPm,l355,1),l373))<0){return l144;}l374 = 
(l355 == l354)?l371:l75;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l74,L3_DEFIPm)),(l371),(l61[(l74)]->l40))){l128 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_TABLEm,l372,BPM_LENGTH1f);if(l370&1){l144 = 
soc_th_alpm_lpm_ip4entry1_to_1(l74,l371,l374,PRESERVE_HIT);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,l373,BPM_LENGTH1f,l128);}else{
l144 = soc_th_alpm_lpm_ip4entry1_to_0(l74,l371,l374,PRESERVE_HIT);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,l373,BPM_LENGTH0f,l128);}l375 = (
l354<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(
l371),(l61[(l74)]->l40),(0));}else{l128 = soc_mem_field32_get(l74,
L3_DEFIP_AUX_TABLEm,l372,BPM_LENGTH0f);if(l370&1){l144 = 
soc_th_alpm_lpm_ip4entry0_to_1(l74,l371,l374,PRESERVE_HIT);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,l373,BPM_LENGTH1f,l128);}else{
l144 = soc_th_alpm_lpm_ip4entry0_to_0(l74,l371,l374,PRESERVE_HIT);
soc_mem_field32_set(l74,L3_DEFIP_AUX_TABLEm,l373,BPM_LENGTH0f,l128);}l375 = 
l354<<1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l74,L3_DEFIPm)),(
l371),(l61[(l74)]->l39),(0));(l9[(l74)][(l87)].l5)-= 1;(l9[(l74)][(l87)].l6)
+= 1;(l9[(l74)][(l87)].l3)-= 1;}l375 = soc_th_alpm_physical_idx(l74,L3_DEFIPm
,l375,0);l370 = soc_th_alpm_physical_idx(l74,L3_DEFIPm,l370,0);
ALPM_TCAM_PIVOT(l74,l370) = ALPM_TCAM_PIVOT(l74,l375);if(ALPM_TCAM_PIVOT(l74,
l370)){PIVOT_TCAM_INDEX(ALPM_TCAM_PIVOT(l74,l370)) = l370;
soc_th_alpm_lpm_move_bu_upd(l74,PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l74,l370))
,l370);}ALPM_TCAM_PIVOT(l74,l375) = NULL;if((l144 = WRITE_L3_DEFIP_AUX_TABLEm
(l74,MEM_BLOCK_ALL,soc_th_alpm_physical_idx(l74,L3_DEFIPm,l355,1),l373))<0){
return l144;}if(l355!= l354){l334(l74,l374,l355,0x4000,0);if((l144 = l232(l74
,MEM_BLOCK_ANY,l355,l355,l374))<0){return l144;}if((l144 = l348(l74,l355,l374
))<0){return l144;}}l334(l74,l371,l354,0x4000,0);if((l144 = l232(l74,
MEM_BLOCK_ANY,l354,l354,l371))<0){return l144;}if((l144 = l348(l74,l354,l371)
)<0){return l144;}}else{(l9[(l74)][(l87)].l5)-= 1;(l9[(l74)][(l87)].l6)+= 1;(
l9[(l74)][(l87)].l3)-= 1;if(l355!= l354){if((l144 = READ_L3_DEFIPm(l74,
MEM_BLOCK_ANY,l354,l371))<0){return l144;}l334(l74,l371,l355,0x4000,0);if((
l144 = l232(l74,MEM_BLOCK_ANY,l355,l354,l371))<0){return l144;}if((l144 = 
l348(l74,l355,l371))<0){return l144;}}l370 = soc_th_alpm_physical_idx(l74,
L3_DEFIPm,l355,1);l375 = soc_th_alpm_physical_idx(l74,L3_DEFIPm,l354,1);
ALPM_TCAM_PIVOT(l74,l370<<1) = ALPM_TCAM_PIVOT(l74,l375<<1);ALPM_TCAM_PIVOT(
l74,l375<<1) = NULL;if(ALPM_TCAM_PIVOT(l74,l370<<1)){PIVOT_TCAM_INDEX(
ALPM_TCAM_PIVOT(l74,l370<<1)) = l370<<1;soc_th_alpm_lpm_move_bu_upd(l74,
PIVOT_BUCKET_INDEX(ALPM_TCAM_PIVOT(l74,l370<<1)),l370<<1);}sal_memcpy(l371,
soc_mem_entry_null(l74,L3_DEFIPm),soc_mem_entry_words(l74,L3_DEFIPm)*4);l334(
l74,l371,l354,0x4000,0);if((l144 = l232(l74,MEM_BLOCK_ANY,l354,l354,l371))<0)
{return l144;}if((l144 = l348(l74,l354,l371))<0){return l144;}}if((l9[(l74)][
(l87)].l5) == 0){l365 = (l9[(l74)][(l87)].l4);assert(l365!= -1);l366 = (l9[(
l74)][(l87)].next);(l9[(l74)][(l365)].next) = l366;(l9[(l74)][(l365)].l6)+= (
l9[(l74)][(l87)].l6);(l9[(l74)][(l87)].l6) = 0;if(l366!= -1){(l9[(l74)][(l366
)].l4) = l365;}(l9[(l74)][(l87)].next) = -1;(l9[(l74)][(l87)].l4) = -1;(l9[(
l74)][(l87)].l2) = -1;(l9[(l74)][(l87)].l3) = -1;}return(l144);}int
soc_th_alpm_lpm_vrf_get(int unit,void*lpm_entry,int*l116,int*l132){int l161;
if(((l61[(unit)]->l43)!= NULL)){l161 = soc_L3_DEFIPm_field32_get(unit,
lpm_entry,VRF_ID_0f);*l132 = l161;if(soc_L3_DEFIPm_field32_get(unit,lpm_entry
,VRF_ID_MASK0f)){*l116 = l161;}else if(!soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(unit,L3_DEFIPm)),(lpm_entry),(l61[(unit)]->l45))){*l116 = 
SOC_L3_VRF_GLOBAL;*l132 = SOC_VRF_MAX(unit)+1;}else{*l116 = 
SOC_L3_VRF_OVERRIDE;}}else{*l116 = SOC_L3_VRF_DEFAULT;}return(SOC_E_NONE);}
static int l376(int l74,void*entry,int*l105){int l87;int l144;int l99;uint32
l145;int l161;int l142;l99 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l74,L3_DEFIPm)),(entry),(l61[(l74)]->l29));if(l99){l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(entry),(l61
[(l74)]->l27));if((l144 = _ipmask2pfx(l145,&l87))<0){return(l144);}l145 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(entry),(l61
[(l74)]->l28));if(l87){if(l145!= 0xffffffff){return(SOC_E_PARAM);}l87+= 32;}
else{if((l144 = _ipmask2pfx(l145,&l87))<0){return(l144);}}l87+= 33;}else{l145
= soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(entry),(
l61[(l74)]->l27));if((l144 = _ipmask2pfx(l145,&l87))<0){return(l144);}}
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(l74,entry,&l161,&l144));l142 = 
soc_th_alpm_mode_get(l74);switch(l161){case SOC_L3_VRF_GLOBAL:if((l142 == 
SOC_ALPM_MODE_PARALLEL)||(l142 == SOC_ALPM_MODE_TCAM_ALPM)){*l105 = l87+((3*(
64+32+2+1))/3);}else{*l105 = l87;}break;case SOC_L3_VRF_OVERRIDE:*l105 = l87+
2*((3*(64+32+2+1))/3);break;default:if((l142 == SOC_ALPM_MODE_PARALLEL)||(
l142 == SOC_ALPM_MODE_TCAM_ALPM)){*l105 = l87;}else{*l105 = l87+((3*(64+32+2+
1))/3);}break;}return(SOC_E_NONE);}static int l103(int l74,void*key_data,void
*l75,int*l104,int*l105,int*l99){int l144;int l122;int l88;int l87 = 0;l122 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(
l61[(l74)]->l29));if(l122){if(!(l122 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l74,L3_DEFIPm)),(key_data),(l61[(l74)]->l30)))){return(
SOC_E_PARAM);}}*l99 = l122;l376(l74,key_data,&l87);*l105 = l87;if(l338(l74,
key_data,l87,&l88) == SOC_E_NONE){*l104 = l88;if((l144 = READ_L3_DEFIPm(l74,
MEM_BLOCK_ANY,(*l99)?*l104:(*l104>>1),l75))<0){return l144;}return(SOC_E_NONE
);}else{return(SOC_E_NOT_FOUND);}}static int l94(int l74){int l347;int l166;
int l328;int l377;uint32 l378,l142;if(!soc_feature(l74,soc_feature_lpm_tcam))
{return(SOC_E_UNAVAIL);}if((l142 = soc_property_get(l74,spn_L3_ALPM_ENABLE,0)
)){SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l74,&l378));
soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr,&l378,LPM_MODEf,1);if(l142 == 1){
soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr,&l378,LOOKUP_MODEf,1);}else if(
l142 == 2){soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr,&l378,LOOKUP_MODEf,0);
l3_alpm_ipv4_double_wide[l74] = soc_property_get(l74,
"l3_alpm_ipv4_double_wide",0);if(l3_alpm_ipv4_double_wide[l74]){
soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr,&l378,
ENABLE_DOUBLE_WIDE_BUCKETS_FOR_IPV4f,1);}}else if(l142 == 3){
l3_alpm_ipv4_double_wide[l74] = 1;soc_reg_field_set(l74,L3_DEFIP_RPF_CONTROLr
,&l378,LOOKUP_MODEf,2);}SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l74,
l378));l378 = 0;if(SOC_URPF_STATUS_GET(l74)){soc_reg_field_set(l74,
L3_DEFIP_KEY_SELr,&l378,URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l74,
L3_DEFIP_KEY_SELr,&l378,URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l74,
L3_DEFIP_KEY_SELr,&l378,URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l74,
L3_DEFIP_KEY_SELr,&l378,URPF_LOOKUP_CAM7f,0x1);}SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_KEY_SELr(l74,l378));l378 = 0;if(l142 == 1||l142 == 3){if(
SOC_URPF_STATUS_GET(l74)){soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,
TCAM2_SELf,1);soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM3_SELf,1);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM4_SELf,2);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM5_SELf,2);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM6_SELf,3);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM7_SELf,3);}else{
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM4_SELf,1);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM5_SELf,1);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM6_SELf,1);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM7_SELf,1);}}else{if(
SOC_URPF_STATUS_GET(l74)){soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,
TCAM4_SELf,2);soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM5_SELf,2);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM6_SELf,2);
soc_reg_field_set(l74,L3_DEFIP_ALPM_CFGr,&l378,TCAM7_SELf,2);}}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_ALPM_CFGr(l74,l378));if(soc_property_get(
l74,spn_IPV6_LPM_128B_ENABLE,1)){uint32 l379 = 0;SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_KEY_SELr(l74,&l379));soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&
l379,V6_KEY_SEL_CAM0_1f,0x1);soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l379,
V6_KEY_SEL_CAM2_3f,0x1);soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l379,
V6_KEY_SEL_CAM4_5f,0x1);soc_reg_field_set(l74,L3_DEFIP_KEY_SELr,&l379,
V6_KEY_SEL_CAM6_7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l74,l379)
);}}l347 = (3*(64+32+2+1));SOC_ALPM_LPM_LOCK(l74);l377 = sizeof(l7)*(l347);if
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
[(l74)]) = sal_alloc(l377,"LPM prefix info");if(NULL == (l9[(l74)])){sal_free
(l61[l74]);l61[l74] = NULL;SOC_ALPM_LPM_UNLOCK(l74);return(SOC_E_MEMORY);}
sal_memset((l9[(l74)]),0,l377);for(l166 = 0;l166<l347;l166++){(l9[(l74)][(
l166)].l2) = -1;(l9[(l74)][(l166)].l3) = -1;(l9[(l74)][(l166)].l4) = -1;(l9[(
l74)][(l166)].next) = -1;(l9[(l74)][(l166)].l5) = 0;(l9[(l74)][(l166)].l6) = 
0;}l328 = soc_mem_index_count(l74,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l74)){
l328>>= 1;}if((l142 == 1)||(l142 == 3)){(l9[(l74)][(((3*(64+32+2+1))-1))].l3)
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
,int*l177){int l87;int index;int l99;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int
l144 = SOC_E_NONE;int l380 = 0;sal_memcpy(l75,soc_mem_entry_null(l74,
L3_DEFIPm),soc_mem_entry_words(l74,L3_DEFIPm)*4);SOC_ALPM_LPM_LOCK(l74);l144 = 
l103(l74,l97,l75,&index,&l87,&l99);if(l144 == SOC_E_NOT_FOUND){l144 = l363(
l74,l87,l99,l75,&index);if(l144<0){SOC_ALPM_LPM_UNLOCK(l74);return(l144);}}
else{l380 = 1;}*l177 = index;if(l144 == SOC_E_NONE){if(!l99){if(index&1){l144
= soc_th_alpm_lpm_ip4entry0_to_1(l74,l97,l75,PRESERVE_HIT);}else{l144 = 
soc_th_alpm_lpm_ip4entry0_to_0(l74,l97,l75,PRESERVE_HIT);}if(l144<0){
SOC_ALPM_LPM_UNLOCK(l74);return(l144);}l97 = (void*)l75;index>>= 1;}
soc_th_alpm_lpm_state_dump(l74);LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"soc_alpm_lpm_insert: %d %d\n"),index,l87));if(!l380){l334(l74,l97,index,
0x4000,0);}l144 = l232(l74,MEM_BLOCK_ANY,index,index,l97);if(l144>= 0){l144 = 
l348(l74,index,l97);}}SOC_ALPM_LPM_UNLOCK(l74);return(l144);}static int
soc_th_alpm_lpm_delete(int l74,void*key_data){int l87;int index;int l99;
uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int l144 = SOC_E_NONE;SOC_ALPM_LPM_LOCK(
l74);l144 = l103(l74,key_data,l75,&index,&l87,&l99);if(l144 == SOC_E_NONE){
LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"soc_alpm_lpm_delete: %d %d\n"),
index,l87));l337(l74,key_data,index);l144 = l369(l74,l87,l99,l75,index);}
soc_th_alpm_lpm_state_dump(l74);SOC_ALPM_LPM_UNLOCK(l74);return(l144);}static
int l106(int l74,void*key_data,void*l75,int*l104){int l87;int l144;int l99;
SOC_ALPM_LPM_LOCK(l74);l144 = l103(l74,key_data,l75,l104,&l87,&l99);
SOC_ALPM_LPM_UNLOCK(l74);return(l144);}static int l98(int unit,void*key_data,
int l99,int l100,int l101,defip_aux_scratch_entry_t*l102){uint32 l145;uint32
l276[4] = {0,0,0,0};int l87 = 0;int l144 = SOC_E_NONE;l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,VALID0f);soc_mem_field32_set(unit
,L3_DEFIP_AUX_SCRATCHm,l102,VALIDf,l145);l145 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,MODE0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,
l102,MODEf,l145);l145 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
ENTRY_TYPE0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,ENTRY_TYPEf
,0);l145 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,GLOBAL_ROUTE0f);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,GLOBAL_ROUTEf,l145);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,ECMP0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l102,ECMPf,l145);l145 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,ECMP_PTR0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm
,l102,ECMP_PTRf,l145);l145 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
NEXT_HOP_INDEX0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,
NEXT_HOP_INDEXf,l145);l145 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
PRI0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,PRIf,l145);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,RPE0f);soc_mem_field32_set(unit,
L3_DEFIP_AUX_SCRATCHm,l102,RPEf,l145);l145 =soc_mem_field32_get(unit,
L3_DEFIPm,key_data,VRF_ID_0f);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,
l102,VRFf,l145);soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,DB_TYPEf,
l100);l145 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,DST_DISCARD0f);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,DST_DISCARDf,l145);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,CLASS_ID0f);soc_mem_field32_set(
unit,L3_DEFIP_AUX_SCRATCHm,l102,CLASS_IDf,l145);if(l99){l276[2] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR0f);l276[3] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR1f);}else{l276[0] = 
soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR0f);}soc_mem_field_set(
unit,L3_DEFIP_AUX_SCRATCHm,(uint32*)l102,IP_ADDRf,(uint32*)l276);if(l99){l145
= soc_mem_field32_get(unit,L3_DEFIPm,key_data,IP_ADDR_MASK0f);if((l144 = 
_ipmask2pfx(l145,&l87))<0){return(l144);}l145 = soc_mem_field32_get(unit,
L3_DEFIPm,key_data,IP_ADDR_MASK1f);if(l87){if(l145!= 0xffffffff){return(
SOC_E_PARAM);}l87+= 32;}else{if((l144 = _ipmask2pfx(l145,&l87))<0){return(
l144);}}}else{l145 = soc_mem_field32_get(unit,L3_DEFIPm,key_data,
IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l145,&l87))<0){return(l144);}}
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,IP_LENGTHf,l87);
soc_mem_field32_set(unit,L3_DEFIP_AUX_SCRATCHm,l102,REPLACE_LENf,l101);return
(SOC_E_NONE);}int _soc_th_alpm_aux_op(int l74,_soc_aux_op_t l381,
defip_aux_scratch_entry_t*l102,int l382,int*l157,int*tcam_index,int*bktid){
uint32 l378,l383;int l384;soc_timeout_t l385;int l144 = SOC_E_NONE;int l386 = 
0;if(l382){SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_SCRATCHm(l74,MEM_BLOCK_ANY,
0,l102));}l387:l378 = 0;switch(l381){case INSERT_PROPAGATE:l384 = 0;break;
case DELETE_PROPAGATE:l384 = 1;break;case PREFIX_LOOKUP:l384 = 2;break;case
HITBIT_REPLACE:l384 = 3;break;default:return SOC_E_PARAM;}soc_reg_field_set(
l74,L3_DEFIP_AUX_CTRLr,&l378,OPCODEf,l384);soc_reg_field_set(l74,
L3_DEFIP_AUX_CTRLr,&l378,STARTf,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l74,l378));soc_timeout_init(&l385,50000,5);l384 = 0;
do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_AUX_CTRLr(l74,&l378));l384 = 
soc_reg_field_get(l74,L3_DEFIP_AUX_CTRLr,l378,DONEf);if(l384 == 1){l144 = 
SOC_E_NONE;break;}if(soc_timeout_check(&l385)){SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l74,&l378));l384 = soc_reg_field_get(l74,
L3_DEFIP_AUX_CTRLr,l378,DONEf);if(l384 == 1){l144 = SOC_E_NONE;}else{LOG_WARN
(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d : DEFIP AUX Operation timeout\n"),
l74));l144 = SOC_E_TIMEOUT;}break;}}while(1);if(SOC_SUCCESS(l144)){if(
soc_reg_field_get(l74,L3_DEFIP_AUX_CTRLr,l378,ERRORf)){soc_reg_field_set(l74,
L3_DEFIP_AUX_CTRLr,&l378,STARTf,0);soc_reg_field_set(l74,L3_DEFIP_AUX_CTRLr,&
l378,ERRORf,0);soc_reg_field_set(l74,L3_DEFIP_AUX_CTRLr,&l378,DONEf,0);
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l74,l378));LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d: DEFIP AUX Operation encountered "
"parity error !!\n"),l74));l386++;if(SOC_CONTROL(l74)->alpm_bulk_retry){
sal_sem_take(SOC_CONTROL(l74)->alpm_bulk_retry,1000000);}if(l386<5){LOG_WARN(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"unit %d: Retry DEFIP AUX Operation..\n"),l74
));goto l387;}else{LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"unit %d: Aborting DEFIP AUX Operation ""due to un-correctable error !!\n"),
l74));return SOC_E_INTERNAL;}}if(l381 == PREFIX_LOOKUP){if(l157&&tcam_index){
*l157 = soc_reg_field_get(l74,L3_DEFIP_AUX_CTRLr,l378,HITf);*tcam_index = 
soc_reg_field_get(l74,L3_DEFIP_AUX_CTRLr,l378,BKT_INDEXf);SOC_IF_ERROR_RETURN
(READ_L3_DEFIP_AUX_CTRL_1r(l74,&l383));*bktid = ALPM_BKTID(soc_reg_field_get(
l74,L3_DEFIP_AUX_CTRL_1r,l383,BKT_PTRf),soc_reg_field_get(l74,
L3_DEFIP_AUX_CTRL_1r,l383,SUB_BKT_PTRf));}}}return l144;}static int l108(int
unit,void*lpm_entry,void*l109,void*l110,soc_mem_t l111,uint32 l112,uint32*
l388,int l114){uint32 l145;uint32 l276[4] = {0,0};int l87 = 0;int l144 = 
SOC_E_NONE;int l99;uint32 l113 = 0;l99 = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,MODE0f);sal_memset(l109,0,soc_mem_entry_words(unit,l111)*4);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,HIT0f);soc_mem_field32_set(unit,
l111,l109,HITf,l145);l145 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,
VALID0f);soc_mem_field32_set(unit,l111,l109,VALIDf,l145);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,ECMP0f);soc_mem_field32_set(unit
,l111,l109,ECMPf,l145);l145 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,
ECMP_PTR0f);soc_mem_field32_set(unit,l111,l109,ECMP_PTRf,l145);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f);
soc_mem_field32_set(unit,l111,l109,NEXT_HOP_INDEXf,l145);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,PRI0f);soc_mem_field32_set(unit,
l111,l109,PRIf,l145);l145 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,
RPE0f);soc_mem_field32_set(unit,l111,l109,RPEf,l145);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,DST_DISCARD0f);
soc_mem_field32_set(unit,l111,l109,DST_DISCARDf,l145);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,SRC_DISCARD0f);
soc_mem_field32_set(unit,l111,l109,SRC_DISCARDf,l145);l145 = 
soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,CLASS_ID0f);soc_mem_field32_set(
unit,l111,l109,CLASS_IDf,l145);soc_mem_field32_set(unit,l111,l109,
SUB_BKT_PTRf,ALPM_BKT_SIDX(l114));l276[0] = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,IP_ADDR0f);if(l99){l276[1] = soc_mem_field32_get(unit,
L3_DEFIPm,lpm_entry,IP_ADDR1f);}soc_mem_field_set(unit,l111,(uint32*)l109,
KEYf,(uint32*)l276);if(l99){l145 = soc_mem_field32_get(unit,L3_DEFIPm,
lpm_entry,IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l145,&l87))<0){return(l144);
}l145 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK1f);if(l87){
if(l145!= 0xffffffff){return(SOC_E_PARAM);}l87+= 32;}else{if((l144 = 
_ipmask2pfx(l145,&l87))<0){return(l144);}}}else{l145 = soc_mem_field32_get(
unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f);if((l144 = _ipmask2pfx(l145,&l87))<0
){return(l144);}}if((l87 == 0)&&(l276[0] == 0)&&(l276[1] == 0)){l113 = 1;}if(
l388!= NULL){*l388 = l113;}soc_mem_field32_set(unit,l111,l109,LENGTHf,l87);if
(l110 == NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(unit)){sal_memset(
l110,0,soc_mem_entry_words(unit,l111)*4);sal_memcpy(l110,l109,
soc_mem_entry_words(unit,l111)*4);soc_mem_field32_set(unit,l111,l110,
DST_DISCARDf,0);soc_mem_field32_set(unit,l111,l110,RPEf,0);
soc_mem_field32_set(unit,l111,l110,SRC_DISCARDf,l112&SOC_ALPM_RPF_SRC_DISCARD
);soc_mem_field32_set(unit,l111,l110,DEFAULTROUTEf,l113);}return(SOC_E_NONE);
}static int l115(int unit,void*l109,soc_mem_t l111,int l99,int l116,int l117,
int index,void*lpm_entry){uint32 l145;uint32 l276[4] = {0,0};uint32 l87 = 0;
sal_memset(lpm_entry,0,soc_mem_entry_words(unit,L3_DEFIPm)*4);l145 = 
soc_mem_field32_get(unit,l111,l109,HITf);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,HIT0f,l145);if(l99){soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
HIT1f,l145);}l145 = soc_mem_field32_get(unit,l111,l109,VALIDf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VALID0f,l145);if(l99){
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,VALID1f,l145);}l145 = 
soc_mem_field32_get(unit,l111,l109,ECMPf);soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,ECMP0f,l145);l145 = soc_mem_field32_get(unit,l111,l109,ECMP_PTRf);
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ECMP_PTR0f,l145);l145 = 
soc_mem_field32_get(unit,l111,l109,NEXT_HOP_INDEXf);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,NEXT_HOP_INDEX0f,l145);l145 = soc_mem_field32_get(unit,
l111,l109,PRIf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,PRI0f,l145);l145
= soc_mem_field32_get(unit,l111,l109,RPEf);soc_mem_field32_set(unit,L3_DEFIPm
,lpm_entry,RPE0f,l145);l145 = soc_mem_field32_get(unit,l111,l109,DST_DISCARDf
);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,DST_DISCARD0f,l145);l145 = 
soc_mem_field32_get(unit,l111,l109,SRC_DISCARDf);soc_mem_field32_set(unit,
L3_DEFIPm,lpm_entry,SRC_DISCARD0f,l145);l145 = soc_mem_field32_get(unit,l111,
l109,CLASS_IDf);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,CLASS_ID0f,l145)
;soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ALG_BKT_PTR0f,ALPM_BKT_IDX(l117
));soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ALG_SUB_BKT_PTR0f,
ALPM_BKT_SIDX(l117));soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
ALG_HIT_IDX0f,index);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE_MASK0f
,3);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,ENTRY_TYPE_MASK0f,1);if(l99)
{soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,MODE0f,1);}soc_mem_field_get(
unit,l111,l109,KEYf,l276);if(l99){soc_mem_field32_set(unit,L3_DEFIPm,
lpm_entry,IP_ADDR1f,l276[1]);}soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR0f,l276[0]);l145 = soc_mem_field32_get(unit,l111,l109,LENGTHf);if(l99)
{if(l145>= 32){l87 = 0xffffffff;soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l87);l87 = ~(((l145-32) == 32)?0:(0xffffffff)>>(l145-32));
soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,IP_ADDR_MASK0f,l87);}else{l87 = 
~(0xffffffff>>l145);soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
IP_ADDR_MASK1f,l87);}}else{assert(l145<= 32);l87 = ~(((l145) == 32)?0:(
0xffffffff)>>(l145));soc_mem_field32_set(unit,L3_DEFIPm,lpm_entry,
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
void*lpm_entry,int l389,int bktid){int l144 = SOC_E_NONE;uint32 key[2] = {0,0
};alpm_pivot_t*l126 = NULL;alpm_bucket_handle_t*l256 = NULL;int l161 = 0,l116
= 0;uint32 l390;trie_t*l297 = NULL;uint32 prefix[5] = {0};int l113 = 0;
SOC_IF_ERROR_RETURN(l152(unit,lpm_entry,prefix,&l390,&l113));
SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l161,&l116));l389
= soc_th_alpm_physical_idx(unit,L3_DEFIPm,l389,l99);l256 = sal_alloc(sizeof(
alpm_bucket_handle_t),"ALPM Bucket Handle");if(l256 == NULL){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));return SOC_E_NONE;}sal_memset(l256,0,sizeof(*l256));
l126 = sal_alloc(sizeof(alpm_pivot_t),"Payload for Pivot");if(l126 == NULL){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"Unable to allocate memory for "
"PIVOT trie node \n")));sal_free(l256);return SOC_E_MEMORY;}sal_memset(l126,0
,sizeof(*l126));PIVOT_BUCKET_HANDLE(l126) = l256;if(l99){trie_init(
_MAX_KEY_LEN_144_,&PIVOT_BUCKET_TRIE(l126));key[0] = 
soc_L3_DEFIPm_field32_get(unit,lpm_entry,IP_ADDR0f);key[1] = 
soc_L3_DEFIPm_field32_get(unit,lpm_entry,IP_ADDR1f);}else{trie_init(
_MAX_KEY_LEN_48_,&PIVOT_BUCKET_TRIE(l126));key[0] = soc_L3_DEFIPm_field32_get
(unit,lpm_entry,IP_ADDR0f);}PIVOT_BUCKET_INDEX(l126) = bktid;PIVOT_TCAM_INDEX
(l126) = l389;if(!(ALPM_PREFIX_IN_TCAM(unit,l161))){if(l99 == 0){l297 = 
VRF_PIVOT_TRIE_IPV4(unit,l116);if(l297 == NULL){trie_init(_MAX_KEY_LEN_48_,&
VRF_PIVOT_TRIE_IPV4(unit,l116));l297 = VRF_PIVOT_TRIE_IPV4(unit,l116);}}else{
l297 = VRF_PIVOT_TRIE_IPV6(unit,l116);if(l297 == NULL){trie_init(
_MAX_KEY_LEN_144_,&VRF_PIVOT_TRIE_IPV6(unit,l116));l297 = VRF_PIVOT_TRIE_IPV6
(unit,l116);}}sal_memcpy(l126->key,prefix,sizeof(prefix));l126->len = l390;
l144 = trie_insert(l297,l126->key,NULL,l126->len,(trie_node_t*)l126);if(
SOC_FAILURE(l144)){sal_free(l256);sal_free(l126);return l144;}}
ALPM_TCAM_PIVOT(unit,l389) = l126;PIVOT_BUCKET_VRF(l126) = l116;
PIVOT_BUCKET_IPV6(l126) = l99;PIVOT_BUCKET_ENT_CNT_UPDATE(l126);if(key[0] == 
0&&key[1] == 0){PIVOT_BUCKET_DEF(l126) = TRUE;}VRF_PIVOT_REF_INC(unit,l116,
l99);return l144;}static int l391(int unit,int l99,void*lpm_entry,void*l109,
soc_mem_t l111,int l389,int bktid,int l392){int l393;int l116;int l144 = 
SOC_E_NONE;int l113 = 0;uint32 prefix[5] = {0,0,0,0,0};uint32 l127;
defip_entry_t l394;trie_t*l257 = NULL;trie_t*l125 = NULL;trie_node_t*l253 = 
NULL;payload_t*l395 = NULL;payload_t*l274 = NULL;alpm_pivot_t*pivot_pyld = 
NULL;if((NULL == lpm_entry)||(NULL == l109)){return SOC_E_PARAM;}if(l99){if(!
(l99 = soc_mem_field32_get(unit,L3_DEFIPm,lpm_entry,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&
l393,&l116));l111 = (l99)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;
SOC_IF_ERROR_RETURN(l115(unit,l109,l111,l99,l393,bktid,l389,&l394));l144 = 
l152(unit,&l394,prefix,&l127,&l113);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"prefix create failed\n")));return l144;}
pivot_pyld = ALPM_TCAM_PIVOT(unit,l389);l257 = PIVOT_BUCKET_TRIE(pivot_pyld);
l395 = sal_alloc(sizeof(payload_t),"Payload for Key");if(NULL == l395){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for trie node.\n")));return SOC_E_MEMORY;}l274 = 
sal_alloc(sizeof(payload_t),"Payload for pfx trie key");if(NULL == l274){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"Unable to allocate memory for pfx trie node\n")));sal_free(l395);return
SOC_E_MEMORY;}sal_memset(l395,0,sizeof(*l395));sal_memset(l274,0,sizeof(*l274
));l395->key[0] = prefix[0];l395->key[1] = prefix[1];l395->key[2] = prefix[2]
;l395->key[3] = prefix[3];l395->key[4] = prefix[4];l395->len = l127;l395->
index = l392;sal_memcpy(l274,l395,sizeof(*l395));l144 = trie_insert(l257,
prefix,NULL,l127,(trie_node_t*)l395);if(SOC_FAILURE(l144)){goto l396;}if(l99)
{l125 = VRF_PREFIX_TRIE_IPV6(unit,l116);}else{l125 = VRF_PREFIX_TRIE_IPV4(
unit,l116);}if(!l113){l144 = trie_insert(l125,prefix,NULL,l127,(trie_node_t*)
l274);if(SOC_FAILURE(l144)){goto l397;}}return l144;l397:(void)trie_delete(
l257,prefix,l127,&l253);l395 = (payload_t*)l253;l396:sal_free(l395);sal_free(
l274);return l144;}static int l398(int unit,int l122,int l116,int l147,int
bktid){int l144 = SOC_E_NONE;uint32 l127;uint32 key[2] = {0,0};trie_t*l399 = 
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
trie_init(_MAX_KEY_LEN_48_,&VRF_PREFIX_TRIE_IPV4(unit,l116));l399 = 
VRF_PREFIX_TRIE_IPV4(unit,l116);}else{VRF_TRIE_DEFAULT_ROUTE_IPV6(unit,l116) = 
lpm_entry;trie_init(_MAX_KEY_LEN_144_,&VRF_PREFIX_TRIE_IPV6(unit,l116));l399 = 
VRF_PREFIX_TRIE_IPV6(unit,l116);}l286 = sal_alloc(sizeof(payload_t),
"Payload for pfx trie key");if(l286 == NULL){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"Unable to allocate memory for pfx trie node \n")));return
SOC_E_MEMORY;}sal_memset(l286,0,sizeof(*l286));l127 = 0;l286->key[0] = key[0]
;l286->key[1] = key[1];l286->len = l127;l144 = trie_insert(l399,key,NULL,l127
,&(l286->node));if(SOC_FAILURE(l144)){sal_free(l286);return l144;}
VRF_TRIE_INIT_DONE(unit,l116,l122,1);return l144;}int
soc_th_alpm_warmboot_prefix_insert(int unit,int l99,void*lpm_entry,void*l109,
int l389,int bktid,int l392){int l393;int l116;int l144 = SOC_E_NONE;
soc_mem_t l111;l389 = soc_th_alpm_physical_idx(unit,L3_DEFIPm,l389,l99);l111 = 
(l99)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;SOC_IF_ERROR_RETURN(
soc_th_alpm_lpm_vrf_get(unit,lpm_entry,&l393,&l116));if(ALPM_PREFIX_IN_TCAM(
unit,l393)){return(l144);}if(!VRF_TRIE_INIT_COMPLETED(unit,l116,l99)){
LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d is not initialized\n"),
l116));l144 = l398(unit,l99,l116,l389,bktid);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(unit,"VRF %d/%d trie init \n""failed\n"),l116,l99
));return l144;}LOG_VERBOSE(BSL_LS_SOC_ALPM,(BSL_META_U(unit,
"VRF %d/%d trie init completed\n"),l116,l99));}l144 = l391(unit,l99,lpm_entry
,l109,l111,l389,bktid,l392);if(l144!= SOC_E_NONE){LOG_WARN(BSL_LS_SOC_ALPM,(
BSL_META_U(unit,"unit %d : Route Insertion Failed :%s\n"),unit,soc_errmsg(
l144)));return(l144);}soc_th_alpm_bu_upd(unit,bktid,l389,l116,l99,1);
VRF_TRIE_ROUTES_INC(unit,l116,l99);return(l144);}int
soc_th_alpm_warmboot_bucket_bitmap_set(int l74,int l116,int l122,int l212){
int l219 = 1;SHR_BITDCL*l329 = SOC_TH_ALPM_VRF_BUCKET_BMAP(l74);if(l116 == 
SOC_VRF_MAX(l74)+1){l329 = SOC_TH_ALPM_GLB_BUCKET_BMAP(l74);}if(
SOC_TH_ALPM_SCALE_CHECK(l74,l122)){l219 = 2;}SHR_BITSET_RANGE(l329,l212,l219)
;return SOC_E_NONE;}int soc_th_alpm_warmboot_lpm_reinit_done(int unit){int
l147;int l400 = ((3*(64+32+2+1))-1);int l401;int l328 = soc_mem_index_count(
unit,L3_DEFIPm);if(SOC_URPF_STATUS_GET(unit)){l328>>= 1;}l401 = 
soc_th_alpm_mode_get(unit);if(l401 == SOC_ALPM_MODE_COMBINED){(l9[(unit)][(((
3*(64+32+2+1))-1))].l4) = -1;for(l147 = ((3*(64+32+2+1))-1);l147>-1;l147--){
if(-1 == (l9[(unit)][(l147)].l2)){continue;}(l9[(unit)][(l147)].l4) = l400;(
l9[(unit)][(l400)].next) = l147;(l9[(unit)][(l400)].l6) = (l9[(unit)][(l147)]
.l2)-(l9[(unit)][(l400)].l3)-1;l400 = l147;}(l9[(unit)][(l400)].next) = -1;(
l9[(unit)][(l400)].l6) = l328-(l9[(unit)][(l400)].l3)-1;}else if(l401 == 1){(
l9[(unit)][(((3*(64+32+2+1))-1))].l4) = -1;for(l147 = ((3*(64+32+2+1))-1);
l147>(((3*(64+32+2+1))-1)/3);l147--){if(-1 == (l9[(unit)][(l147)].l2)){
continue;}(l9[(unit)][(l147)].l4) = l400;(l9[(unit)][(l400)].next) = l147;(l9
[(unit)][(l400)].l6) = (l9[(unit)][(l147)].l2)-(l9[(unit)][(l400)].l3)-1;l400
= l147;}(l9[(unit)][(l400)].next) = -1;(l9[(unit)][(l400)].l6) = l328-(l9[(
unit)][(l400)].l3)-1;l400 = (((3*(64+32+2+1))-1)/3);(l9[(unit)][((((3*(64+32+
2+1))-1)/3))].l4) = -1;for(l147 = ((((3*(64+32+2+1))-1)/3)-1);l147>-1;l147--)
{if(-1 == (l9[(unit)][(l147)].l2)){continue;}(l9[(unit)][(l147)].l4) = l400;(
l9[(unit)][(l400)].next) = l147;(l9[(unit)][(l400)].l6) = (l9[(unit)][(l147)]
.l2)-(l9[(unit)][(l400)].l3)-1;l400 = l147;}(l9[(unit)][(l400)].next) = -1;(
l9[(unit)][(l400)].l6) = (l328>>1)-(l9[(unit)][(l400)].l3)-1;}return(
SOC_E_NONE);}int soc_th_alpm_warmboot_lpm_reinit(int unit,int l99,int l147,
void*lpm_entry){int l105;defip_entry_t*l402;if(soc_L3_DEFIPm_field32_get(unit
,lpm_entry,VALID0f)||soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID1f)){l334(
unit,lpm_entry,l147,0x4000,0);}if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,
VALID0f)){SOC_IF_ERROR_RETURN(l376(unit,lpm_entry,&l105));if((l9[(unit)][(
l105)].l5) == 0){(l9[(unit)][(l105)].l2) = l147;(l9[(unit)][(l105)].l3) = 
l147;}else{(l9[(unit)][(l105)].l3) = l147;}(l9[(unit)][(l105)].l5)++;if(l99){
return(SOC_E_NONE);}}else{if(soc_L3_DEFIPm_field32_get(unit,lpm_entry,VALID1f
)){l402 = sal_alloc(sizeof(defip_entry_t),"lpm_entry_hi");
soc_th_alpm_lpm_ip4entry1_to_0(unit,lpm_entry,l402,TRUE);SOC_IF_ERROR_RETURN(
l376(unit,l402,&l105));if((l9[(unit)][(l105)].l5) == 0){(l9[(unit)][(l105)].
l2) = l147;(l9[(unit)][(l105)].l3) = l147;}else{(l9[(unit)][(l105)].l3) = 
l147;}sal_free(l402);(l9[(unit)][(l105)].l5)++;}}return(SOC_E_NONE);}void
soc_th_alpm_dbg_urpf(int l74){int l144 = SOC_E_NONE;soc_mem_t l111;int l166,
l148;int index;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];uint32 l403[
SOC_MAX_MEM_FIELD_WORDS];int l404 = 0,l405 = 0;if(!SOC_URPF_STATUS_GET(l74)){
LOG_CLI((BSL_META_U(l74,"Only supported in uRPF mode\n")));return;}for(l148 = 
0;l148<= SOC_TH_MAX_ALPM_BUCKETS/2;l148++){index = 
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,ALPM_BKTID(l148,0));l111 = 
_soc_tomahawk_alpm_bkt_view_get(l74,index);for(l166 = 0;l166<
SOC_TH_MAX_BUCKET_ENTRIES-1;l166++){l144 = _soc_th_alpm_mem_index(l74,l111,
l148,l166,0,&index);if(SOC_FAILURE(l144)){continue;}sal_memset(l75,0,sizeof(
uint32)*SOC_MAX_MEM_FIELD_WORDS);sal_memset(l403,0,sizeof(uint32)*
SOC_MAX_MEM_FIELD_WORDS);l144 = soc_mem_read(l74,l111,MEM_BLOCK_ANY,index,l75
);if(SOC_FAILURE(l144)){continue;}if(soc_mem_field32_get(l74,l111,l75,VALIDf)
== 0){continue;}soc_mem_field32_set(l74,l111,l75,HITf,0);l144 = soc_mem_read(
l74,l111,MEM_BLOCK_ANY,_soc_th_alpm_rpf_entry(l74,index),l403);if(SOC_FAILURE
(l144)){continue;}soc_mem_field32_set(l74,l111,l403,HITf,0);
soc_mem_field32_set(l74,l111,l403,RPEf,0);if(sal_memcmp(l75,l403,sizeof(
uint32)*SOC_MAX_MEM_FIELD_WORDS)!= 0){LOG_CLI((BSL_META_U(l74,
"uRPF sanity check failed: bucket %4d mem %s index %d uRPF index %d\n"),l148,
SOC_MEM_NAME(l74,l111),index,_soc_th_alpm_rpf_entry(l74,index)));l405++;}else
{l404++;}}}LOG_CLI((BSL_META_U(l74,
"uRPF Sanity check finished, good %d bad %d\n"),l404,l405));return;}typedef
struct l406{int v4;int v6_64;int v6_128;int l407;int l408;int l409;int l220;}
l410;typedef enum l411{l412 = 0,l413,l414,l415,l416,l417}l418;static void l419
(int l74,alpm_vrf_counter_t*l420){l420->v4 = soc_mem_index_count(l74,
L3_DEFIPm)*2;l420->v6_128 = soc_mem_index_count(l74,L3_DEFIP_PAIR_128m);if(
soc_property_get(l74,spn_IPV6_LPM_128B_ENABLE,1)){l420->v6_64 = l420->v6_128;
}else{l420->v6_64 = l420->v4>>1;}if(SOC_URPF_STATUS_GET(l74)){l420->v4>>= 1;
l420->v6_128>>= 1;l420->v6_64>>= 1;}}static void l421(int l74,int l161,
alpm_vrf_handle_t*l422,l418 l423){alpm_vrf_counter_t*l424;int l166,l425,l426,
l427;int l380 = 0;alpm_vrf_counter_t l420;switch(l423){case l412:LOG_CLI((
BSL_META_U(l74,"\nAdd Counter:\n")));break;case l413:LOG_CLI((BSL_META_U(l74,
"\nDelete Counter:\n")));break;case l414:LOG_CLI((BSL_META_U(l74,
"\nInternal Debug Counter - 1:\n")));break;case l415:l419(l74,&l420);LOG_CLI(
(BSL_META_U(l74,"\nPivot Occupancy: Max v4/v6-64/v6-128 = %d/%d/%d\n"),l420.
v4,l420.v6_64,l420.v6_128));break;case l416:LOG_CLI((BSL_META_U(l74,
"\nInternal Debug Counter - LPM Shift:\n")));break;case l417:LOG_CLI((
BSL_META_U(l74,"\nInternal Debug Counter - LPM Full:\n")));break;default:
break;}LOG_CLI((BSL_META_U(l74,
"\n      VRF  v4      v6-64   v6-128  |   Total\n")));LOG_CLI((BSL_META_U(l74
,"-----------------------------------------------\n")));l425 = l426 = l427 = 
0;for(l166 = 0;l166<MAX_VRF_ID+1;l166++){int l428,l429,l430;if(l422[l166].
init_done == 0&&l166!= MAX_VRF_ID){continue;}if(l161!= -1&&l161!= l166){
continue;}l380 = 1;switch(l423){case l412:l424 = &l422[l166].add;break;case
l413:l424 = &l422[l166].del;break;case l414:l424 = &l422[l166].bkt_split;
break;case l416:l424 = &l422[l166].lpm_shift;break;case l417:l424 = &l422[
l166].lpm_full;break;case l415:l424 = &l422[l166].pivot_used;break;default:
l424 = &l422[l166].pivot_used;break;}l428 = l424->v4;l429 = l424->v6_64;l430 = 
l424->v6_128;l425+= l428;l426+= l429;l427+= l430;do{LOG_CLI((BSL_META_U(l74,
"%9d  %-7d %-7d %-7d |   %-7d %s\n"),(l166 == MAX_VRF_ID?-1:l166),(l428),(
l429),(l430),((l428+l429+l430)),(l166) == MAX_VRF_ID?"GHi":(l166) == 
SOC_VRF_MAX(l74)+1?"GLo":""));}while(0);}if(l380 == 0){LOG_CLI((BSL_META_U(
l74,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l74,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l74,"%9s  %-7d %-7d %-7d |   %-7d \n"),"Total",(l425),(l426),(l427),((l425+
l426+l427))));}while(0);}return;}int soc_th_alpm_debug_show(int l74,int l161,
uint32 flags){int l166,l431,l380 = 0;l410*l432;l410 l173;l410 l433;if(l161>(
SOC_VRF_MAX(l74)+1)){return SOC_E_PARAM;}l431 = MAX_VRF_ID*sizeof(l410);l432 = 
sal_alloc(l431,"_alpm_dbg_cnt");if(l432 == NULL){return SOC_E_MEMORY;}
sal_memset(l432,0,l431);l173.v4 = ALPM_IPV4_BKT_COUNT;l173.v6_64 = 
ALPM_IPV6_64_BKT_COUNT;l173.v6_128 = ALPM_IPV6_128_BKT_COUNT;if((
soc_th_alpm_mode_get(l74) == SOC_ALPM_MODE_COMBINED||soc_th_alpm_mode_get(l74
) == SOC_ALPM_MODE_TCAM_ALPM)&&!SOC_URPF_STATUS_GET(l74)){l173.v6_64<<= 1;
l173.v6_128<<= 1;}LOG_CLI((BSL_META_U(l74,"\nBucket Occupancy:\n")));if(flags
&(SOC_ALPM_DEBUG_SHOW_FLAG_BKT)){do{LOG_CLI((BSL_META_U(l74,
"\nPivot/VRF  Min     Max     Cur     |   Comment\n")));}while(0);LOG_CLI((
BSL_META_U(l74,"-----------------------------------------------\n")));}for(
l166 = 0;l166<MAX_PIVOT_COUNT;l166++){alpm_pivot_t*l434 = ALPM_TCAM_PIVOT(l74
,l166);if(l434!= NULL){l410*l435;int l116 = PIVOT_BUCKET_VRF(l434);if(l116<0
||l116>(SOC_VRF_MAX(l74)+1)){continue;}if(l161!= -1&&l161!= l116){continue;}
if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){l380 = 1;do{LOG_CLI((BSL_META_U(l74,
"%5d/%-4d %-7d %-7d %-7d |   %-7s\n"),l166,l116,PIVOT_BUCKET_MIN(l434),
PIVOT_BUCKET_MAX(l434),PIVOT_BUCKET_COUNT(l434),PIVOT_BUCKET_DEF(l434)?"Def":
(l116) == SOC_VRF_MAX(l74)+1?"GLo":""));}while(0);}l435 = &l432[l116];if(
PIVOT_BUCKET_IPV6(l434) == L3_DEFIP_MODE_128){l435->v6_128+= 
PIVOT_BUCKET_COUNT(l434);l435->l409+= l173.v6_128;}else if(PIVOT_BUCKET_IPV6(
l434) == L3_DEFIP_MODE_64){l435->v6_64+= PIVOT_BUCKET_COUNT(l434);l435->l408
+= l173.v6_64;}else{l435->v4+= PIVOT_BUCKET_COUNT(l434);l435->l407+= l173.v4;
}l435->l220 = TRUE;}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKT){if(l380 == 0){
LOG_CLI((BSL_META_U(l74,"%9s\n"),"Specific VRF not found"));}}sal_memset(&
l433,0,sizeof(l433));l380 = 0;if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_BKTUSG){
LOG_CLI((BSL_META_U(l74,"\n      VRF  v4      v6-64   v6-128  |   Total\n")))
;LOG_CLI((BSL_META_U(l74,"-----------------------------------------------\n")
));for(l166 = 0;l166<MAX_VRF_ID;l166++){l410*l435;if(l432[l166].l220!= TRUE){
continue;}if(l161!= -1&&l161!= l166){continue;}l380 = 1;l435 = &l432[l166];do
{(&l433)->v4+= (l435)->v4;(&l433)->l407+= (l435)->l407;(&l433)->v6_64+= (l435
)->v6_64;(&l433)->l408+= (l435)->l408;(&l433)->v6_128+= (l435)->v6_128;(&l433
)->l409+= (l435)->l409;}while(0);do{LOG_CLI((BSL_META_U(l74,
"%9d  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% %5s\n"),(l166),(l435
->l407)?(l435->v4)*100/(l435->l407):0,(l435->l407)?(l435->v4)*1000/(l435->
l407)%10:0,(l435->l408)?(l435->v6_64)*100/(l435->l408):0,(l435->l408)?(l435->
v6_64)*1000/(l435->l408)%10:0,(l435->l409)?(l435->v6_128)*100/(l435->l409):0,
(l435->l409)?(l435->v6_128)*1000/(l435->l409)%10:0,((l435->l407+l435->l408+
l435->l409))?((l435->v4+l435->v6_64+l435->v6_128))*100/((l435->l407+l435->
l408+l435->l409)):0,((l435->l407+l435->l408+l435->l409))?((l435->v4+l435->
v6_64+l435->v6_128))*1000/((l435->l407+l435->l408+l435->l409))%10:0,(l166) == 
SOC_VRF_MAX(l74)+1?"GLo":""));}while(0);}if(l380 == 0){LOG_CLI((BSL_META_U(
l74,"%9s\n"),"Specific VRF not found"));}else{LOG_CLI((BSL_META_U(l74,
"-----------------------------------------------\n")));do{LOG_CLI((BSL_META_U
(l74,"%9s  %02d.%d%%   %02d.%d%%   %02d.%d%%   |   %02d.%d%% \n"),"Total",((&
l433)->l407)?((&l433)->v4)*100/((&l433)->l407):0,((&l433)->l407)?((&l433)->v4
)*1000/((&l433)->l407)%10:0,((&l433)->l408)?((&l433)->v6_64)*100/((&l433)->
l408):0,((&l433)->l408)?((&l433)->v6_64)*1000/((&l433)->l408)%10:0,((&l433)->
l409)?((&l433)->v6_128)*100/((&l433)->l409):0,((&l433)->l409)?((&l433)->
v6_128)*1000/((&l433)->l409)%10:0,(((&l433)->l407+(&l433)->l408+(&l433)->l409
))?(((&l433)->v4+(&l433)->v6_64+(&l433)->v6_128))*100/(((&l433)->l407+(&l433)
->l408+(&l433)->l409)):0,(((&l433)->l407+(&l433)->l408+(&l433)->l409))?(((&
l433)->v4+(&l433)->v6_64+(&l433)->v6_128))*1000/(((&l433)->l407+(&l433)->l408
+(&l433)->l409))%10:0));}while(0);}}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_PVT){
l421(l74,l161,alpm_vrf_handle[l74],l415);}if(flags&
SOC_ALPM_DEBUG_SHOW_FLAG_CNT){l421(l74,l161,alpm_vrf_handle[l74],l412);l421(
l74,l161,alpm_vrf_handle[l74],l413);}if(flags&SOC_ALPM_DEBUG_SHOW_FLAG_INTDBG
){l421(l74,l161,alpm_vrf_handle[l74],l414);l421(l74,l161,alpm_vrf_handle[l74]
,l417);l421(l74,l161,alpm_vrf_handle[l74],l416);}sal_free(l432);return
SOC_E_NONE;}int soc_th_alpm_debug_brief_show(int l74,int l117,int index){int
l166,l303,l144 = SOC_E_NONE;uint32 l378,l436 = 0,l142 = 0,l228,l437,l438,l439
;int l440 = 0,l441;int l442 = 0,l443;int l444 = 0,l445;int l446 = 0,l447;char
*l448 = NULL;defip_entry_t*lpm_entry;defip_pair_128_entry_t*l449;int l99,
bkt_ptr,l450,l451,l452;int*l453 = NULL;int l454 = 0,l455 = 0;soc_mem_t l178;
int l456 = 0,l457 = 0,l458 = 0,l459 = 0,l460;uint32 l75[
SOC_MAX_MEM_FIELD_WORDS];soc_field_t l461,l462,l463,l464;char*l465[] = {"LPM"
,"ALPM-Combined","ALPM-Parallel","ALPM-TCAM_ALPM","ALPM-Invalid"};
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l74,&l378));l436 = 
soc_reg_field_get(l74,L3_DEFIP_RPF_CONTROLr,l378,LPM_MODEf);if(l436!= 0){l142
= soc_reg_field_get(l74,L3_DEFIP_RPF_CONTROLr,l378,LOOKUP_MODEf);}l228 = 
soc_reg_field_get(l74,L3_DEFIP_RPF_CONTROLr,l378,DEFIP_RPF_ENABLEf);LOG_CLI((
"\n"));LOG_CLI(("%-24s%s\n","Mode:",l465[l436+l142]));LOG_CLI(("%-24s%s\n",
"uRPF:",l228?"Enable":"Disable"));l437 = !!soc_mem_index_count(l74,
L3_DEFIP_PAIR_128m);LOG_CLI(("%-24s%d\n","IPv6 Bits:",l437?128:64));if(l436 == 
0){return SOC_E_NONE;}if(l142 == 0&&!l228){l438 = soc_reg_field_get(l74,
L3_DEFIP_RPF_CONTROLr,l378,ENABLE_DOUBLE_WIDE_BUCKETS_FOR_IPV4f);}else if(
l142 == 2){l438 = 1;}else{l438 = 0;}LOG_CLI(("%-24s%s\n","IPv4 Wide:",l438?
"Double":"Single"));if(l142 == 0&&!l228){l438 = 1;}else if(l142 == 2){l438 = 
1;}else{l438 = 0;}LOG_CLI(("%-24s%s\n","IPv6 Wide:",l438?"Double":"Single"));
SOC_IF_ERROR_RETURN(READ_ISS_BANK_CONFIGr(l74,&l378));l439 = 
soc_reg_field_get(l74,ISS_BANK_CONFIGr,l378,ALPM_ENTRY_BANK_CONFIGf);LOG_CLI(
("%-24s%d(0x%x)\n","Banks:",_shr_popcount(l439),l439));l441 = 2*
soc_mem_index_count(l74,L3_DEFIPm);l443 = l437?soc_mem_index_count(l74,
L3_DEFIP_PAIR_128m):soc_mem_index_count(l74,L3_DEFIPm);l445 = 
soc_mem_index_count(l74,L3_DEFIP_PAIR_128m);l447 = SOC_TH_MAX_ALPM_BUCKETS;if
(l228){l441>>= 1;l443>>= 1;l445>>= 1;l447>>= 1;}l453 = sal_alloc(l447*sizeof(
int),"bktref");if(l453 == NULL){return SOC_E_MEMORY;}sal_memset(l453,0,l447*
sizeof(int));l178 = L3_DEFIPm;l460 = WORDS2BYTES(soc_mem_entry_words(l74,l178
))*soc_mem_index_count(l74,l178);l448 = soc_cm_salloc(l74,l460,"lpm_tbl");if(
l448 == NULL){l144 = SOC_E_MEMORY;goto l466;}sal_memset(l448,0,l460);if(l441
&&soc_mem_read_range(l74,l178,MEM_BLOCK_ANY,soc_mem_index_min(l74,l178),
soc_mem_index_max(l74,l178),l448)<0){l144 = (SOC_E_INTERNAL);goto l466;}for(
l166 = 0;l166<(l441/2);l166++){lpm_entry = soc_mem_table_idx_to_pointer(l74,
l178,defip_entry_t*,l448,l166);l99 = soc_mem_field32_get(l74,l178,lpm_entry,
MODE0f);for(l303 = 0;l303<(l99?1:2);l303++){if(l303 == 1){l461 = VALID1f;}
else{l461 = VALID0f;}if(!soc_mem_field32_get(l74,l178,lpm_entry,l461)){
continue;}if(l99){l442++;}else{l440++;}if(l303 == 0){l463 = GLOBAL_HIGH0f;
l462 = ALG_BKT_PTR0f;l464 = GLOBAL_ROUTE0f;}else{l463 = GLOBAL_HIGH1f;l462 = 
ALG_BKT_PTR1f;l464 = GLOBAL_ROUTE1f;}l450 = soc_mem_field32_get(l74,l178,
lpm_entry,l463);l451 = soc_mem_field32_get(l74,l178,lpm_entry,l464);if(l451){
if(l450){l454++;l452 = 0;}else{l455++;l452 = (l142 == 2)?0:1;}}else{l452 = 1;
}if(l452){bkt_ptr = soc_mem_field32_get(l74,l178,lpm_entry,l462);if(bkt_ptr>
l447){LOG_CLI(("Invalid bucket pointer in %s index %d\n",SOC_MEM_NAME(l74,
l178),l166));}else{l453[bkt_ptr]++;}}}}soc_cm_sfree(l74,l448);l448 = NULL;
l178 = L3_DEFIP_PAIR_128m;l460 = WORDS2BYTES(soc_mem_entry_words(l74,l178))*
soc_mem_index_count(l74,l178);l448 = soc_cm_salloc(l74,l460,"lpm_tbl_p128");
if(l448 == NULL){l144 = SOC_E_MEMORY;goto l466;}sal_memset(l448,0,l460);if(
l445&&soc_mem_read_range(l74,l178,MEM_BLOCK_ANY,soc_mem_index_min(l74,l178),
soc_mem_index_max(l74,l178),l448)<0){l144 = (SOC_E_INTERNAL);goto l466;}for(
l166 = 0;l166<l445;l166++){l449 = soc_mem_table_idx_to_pointer(l74,l178,
defip_pair_128_entry_t*,l448,l166);if(0x03!= soc_mem_field32_get(l74,l178,
l449,MODE1_UPRf)){continue;};if(!soc_mem_field32_get(l74,l178,l449,
VALID1_LWRf)||!soc_mem_field32_get(l74,l178,l449,VALID0_LWRf)||!
soc_mem_field32_get(l74,l178,l449,VALID1_UPRf)||!soc_mem_field32_get(l74,l178
,l449,VALID0_UPRf)){continue;};l444++;l450 = soc_mem_field32_get(l74,l178,
l449,GLOBAL_HIGHf);l451 = soc_mem_field32_get(l74,l178,l449,GLOBAL_ROUTEf);if
(l451){if(l450){l454++;l452 = 0;}else{l455++;l452 = (l142 == 2)?0:1;}}else{
l452 = 1;}if(l452){bkt_ptr = soc_mem_field32_get(l74,l178,l449,ALG_BKT_PTRf);
if(bkt_ptr>l447){LOG_CLI(("Invalid bucket pointer in %s index %d\n",
SOC_MEM_NAME(l74,l178),l166));}else{l453[bkt_ptr]++;}}}for(l166 = 0;l166<l447
;l166++){if(l453[l166]!= 0){l446++;}if(l453[l166] == 4){l459++;}if(l453[l166]
== 3){l458++;}if(l453[l166] == 2){l457++;}if(l453[l166] == 1){l456++;}}
LOG_CLI(("\n"));LOG_CLI(("%-24s%d/%-4d\n","IPv4 Pivots:",l440,l441));LOG_CLI(
("%-24s%d/%-4d\n","IPv6-64 Pivots:",l442,l443));LOG_CLI(("%-24s%d/%-4d\n",
"IPv6-128 Pivots:",l444,l445));LOG_CLI(("%-24s%d/%-4d\n","Buckets:",l446,l447
));LOG_CLI(("%-24s%d\n","Global High:",l454));LOG_CLI(("%-24s%d\n",
"Global Low:",l455));LOG_CLI(("\n"));LOG_CLI(("%s\n","Bucket Sharing:"));
LOG_CLI(("%-24s%d\n","1->1:",l456));LOG_CLI(("%-24s%d\n","2->1:",l457));
LOG_CLI(("%-24s%d\n","3->1:",l458));LOG_CLI(("%-24s%d\n","4->1:",l459));
LOG_CLI(("\n"));LOG_CLI(("%-24s%d\n","Split count:",alpm_split_count+
alpm_128_split_count));LOG_CLI(("%-24s%d\n","Merge count:",alpm_merge_count))
;LOG_CLI(("\n\n"));l466:if(l453){sal_free(l453);l453 = NULL;}if(l448){
soc_cm_sfree(l74,l448);l448 = NULL;}if(l117!= -1){int l467,l468,l151;l178 = 
_soc_tomahawk_alpm_bkt_view_get(l74,SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,
ALPM_BKTID(l117,0)));LOG_CLI(("Bucket = %4d (View %s)\n",l117,l178 == -1?
"INVALID":SOC_MEM_NAME(l74,l178)));LOG_CLI((
"------------------------------------------------------------\n"));LOG_CLI((
"Index distribution (* indicates occupied)\n"));switch(l178){case
L3_DEFIP_ALPM_IPV4m:l468 = 6;break;case L3_DEFIP_ALPM_IPV6_64m:l468 = 4;break
;case L3_DEFIP_ALPM_IPV6_128m:l468 = 2;break;default:l178 = 
L3_DEFIP_ALPM_IPV4m;l468 = 6;break;}for(l467 = 0;l467<4;l467++){if(!(l439&(1
<<l467))){continue;}LOG_CLI(("Bank%d: ",l467));for(l166 = 0;l166<l468;l166++)
{l151 = (_shr_popcount(l439)+1)/2;l303 = (l166<<(l151+SOC_TH_ALPM_BUCKET_BITS
))|(l117<<l151)|l467;l144 = soc_mem_read(l74,l178,MEM_BLOCK_ANY,l303,l75);if(
l144 == 0&&soc_mem_field32_get(l74,l178,l75,VALIDf)){l151 = 1;}else{l151 = 0;
}if(l151){LOG_CLI(("%d(*) ",l303));}else{LOG_CLI(("%d( ) ",l303));}}LOG_CLI((
"\n"));}}LOG_CLI(("\n"));if(index!= -1){int bktid;LOG_CLI(("Index = %8d\n",
index));LOG_CLI((
"------------------------------------------------------------\n"));l178 = 
_soc_tomahawk_alpm_bkt_view_get(l74,index);LOG_CLI(("%-24s%s\n","Type",l178 == 
-1?"INVALID":SOC_MEM_NAME(l74,l178)));bktid = SOC_TH_ALPM_BKT_ENTRY_TO_IDX(
l74,index);LOG_CLI(("%-24s%d\n","Bucket:",ALPM_BKT_IDX(bktid)));LOG_CLI((
"%-24s%d\n","Sub-Bucket:",ALPM_BKT_SIDX(bktid)));LOG_CLI(("%-24s%d\n","Bank:"
,index&(_shr_popcount(l439)-1)));bktid = (_shr_popcount(l439)+1)/2+
SOC_TH_ALPM_BUCKET_BITS;LOG_CLI(("%-24s%d\n","Entry:",index>>bktid));LOG_CLI(
("\n"));}return SOC_E_NONE;}int soc_th_alpm_bucket_sanity_check(int l74,
soc_mem_t l178,int index){int l144 = SOC_E_NONE;int l166,l303,l311,l99,
tcam_index = -1;int l161 = 0,l116;uint32 l146 = 0;uint32 l75[
SOC_MAX_MEM_FIELD_WORDS];uint32 l469[SOC_MAX_MEM_FIELD_WORDS];int l470,l471,
l472;defip_entry_t lpm_entry;int l316,l473;soc_mem_t l111;int l217,l308,l474,
bkt_ptr,l141;int l475 = 0;soc_field_t l476[3] = {VALID0f,VALID1f,INVALIDf};
soc_field_t l477[3] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f,INVALIDf};soc_field_t l478
[3] = {ALG_BKT_PTR0f,ALG_BKT_PTR1f,INVALIDf};soc_field_t l479[3] = {
ALG_SUB_BKT_PTR0f,ALG_SUB_BKT_PTR1f,INVALIDf};l316 = soc_mem_index_min(l74,
l178);l473 = soc_mem_index_max(l74,l178);if((index>= 0)&&(index<l316||index>
l473)){return SOC_E_PARAM;}else if(index>= 0){l316 = index;l473 = index;}
SOC_ALPM_LPM_LOCK(l74);for(l166 = l316;l166<= l473;l166++){
SOC_ALPM_LPM_UNLOCK(l74);SOC_ALPM_LPM_LOCK(l74);l144 = soc_mem_read(l74,l178,
MEM_BLOCK_ANY,l166,(void*)l75);if(SOC_FAILURE(l144)){continue;}l99 = 
soc_mem_field32_get(l74,l178,(void*)l75,MODE0f);if(l99){l308 = 1;l111 = 
L3_DEFIP_ALPM_IPV6_64m;l474 = 16;}else{l308 = 2;l111 = L3_DEFIP_ALPM_IPV4m;
l474 = 24;}for(l217 = 0;l217<l308;l217++){if(soc_mem_field32_get(l74,l178,(
void*)l75,l476[l217]) == 0||soc_mem_field32_get(l74,l178,(void*)l75,l477[l217
]) == 1){continue;}bkt_ptr = soc_mem_field32_get(l74,l178,(void*)l75,l478[
l217]);l141 = soc_mem_field32_get(l74,l178,(void*)l75,l479[l217]);if(l217 == 
1){soc_th_alpm_lpm_ip4entry1_to_0(l74,l75,l469,PRESERVE_HIT);}else{
soc_th_alpm_lpm_ip4entry0_to_0(l74,l75,l469,PRESERVE_HIT);}l144 = 
soc_th_alpm_lpm_vrf_get(l74,l469,&l161,&l116);if(SOC_FAILURE(l144)){continue;
}if(SOC_TH_ALPM_SCALE_CHECK(l74,l99)){l474<<= 1;}tcam_index = -1;for(l303 = 0
;l303<l474;l303++){l144 = _soc_th_alpm_mem_index(l74,l111,bkt_ptr,l303,l146,&
l311);if(SOC_FAILURE(l144)){continue;}l144 = soc_mem_read(l74,l111,
MEM_BLOCK_ANY,l311,(void*)l75);if(SOC_FAILURE(l144)){break;}if(!
soc_mem_field32_get(l74,l111,(void*)l75,VALIDf)){continue;}if(l141!= 
soc_mem_field32_get(l74,l111,(void*)l75,SUB_BKT_PTRf)){continue;}l144 = l115(
l74,(void*)l75,l111,l99,l161,ALPM_BKTID(bkt_ptr,l141),0,&lpm_entry);if(
SOC_FAILURE(l144)){continue;}l144 = _soc_th_alpm_find(l74,(void*)&lpm_entry,
l111,(void*)l75,&l470,&l471,&l472,FALSE);if(SOC_FAILURE(l144)){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"\tLaunched AUX operation for "
"index %d bucket [%d,%d] sanity check failed\n"),l166,bkt_ptr,l141));l475++;
continue;}if(l471!= ALPM_BKTID(bkt_ptr,l141)){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"\tEntry at index %d does not belong "
"to bucket [%d,%d](from bucket [%d,%d])\n"),l472,bkt_ptr,l141,ALPM_BKT_IDX(
l471),ALPM_BKT_SIDX(l471)));l475++;}if(tcam_index == -1){tcam_index = l470;
continue;}if(tcam_index!= l470){int l480,l481;l480 = soc_th_alpm_logical_idx(
l74,l178,tcam_index>>1,1);l481 = soc_th_alpm_logical_idx(l74,l178,l470>>1,1);
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tAliased bucket [%d,%d](returned "
"bucket [%d,%d]) found from TCAM1 %d and TCAM2 %d\n"),bkt_ptr,l141,
ALPM_BKT_IDX(l471),ALPM_BKT_SIDX(l471),l480,l481));l475++;}}}}
SOC_ALPM_LPM_UNLOCK(l74);if(l475 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(
l74,"\tMemory %s index %d Bucket sanity check passed\n"),SOC_MEM_NAME(l74,
l178),index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tMemory %s index %d Bucket sanity check failed, "
"encountered %d error(s)\n"),SOC_MEM_NAME(l74,l178),index,l475));return
SOC_E_FAIL;}int soc_th_alpm_pivot_sanity_check(int l74,soc_mem_t l178,int
index){int l166,l217,l220 = 0;int l311,l316,l473;int l144 = SOC_E_NONE;int*
l482 = NULL;int l470,l471,l472;uint32 l75[SOC_MAX_MEM_FIELD_WORDS];int bkt_ptr
,l141,l99 = 0,l308 = 2;int l483,l484[2];soc_mem_t l485,l486;int l475 = 0;int
l116,l132;soc_field_t l476[3] = {VALID0f,VALID1f,INVALIDf};soc_field_t l477[3
] = {GLOBAL_HIGH0f,GLOBAL_HIGH1f,INVALIDf};soc_field_t l487[3] = {
NEXT_HOP_INDEX0f,NEXT_HOP_INDEX1f,INVALIDf};soc_field_t l478[3] = {
ALG_BKT_PTR0f,ALG_BKT_PTR1f,INVALIDf};soc_field_t l479[3] = {
ALG_SUB_BKT_PTR0f,ALG_SUB_BKT_PTR1f,INVALIDf};l316 = soc_mem_index_min(l74,
l178);l473 = soc_mem_index_max(l74,l178);if((index>= 0)&&(index<l316||index>
l473)){return SOC_E_PARAM;}else if(index>= 0){l316 = index;l473 = index;}l166
= sizeof(int)*SOC_TH_MAX_ALPM_BUCKETS*SOC_TH_MAX_SUB_BUCKETS;l482 = sal_alloc
(l166,"Bucket index array");if(l482 == NULL){l144 = SOC_E_MEMORY;return l144;
}sal_memset(l482,0xff,l166);SOC_ALPM_LPM_LOCK(l74);for(l166 = l316;l166<= 
l473;l166++){SOC_ALPM_LPM_UNLOCK(l74);SOC_ALPM_LPM_LOCK(l74);l144 = 
soc_mem_read(l74,l178,MEM_BLOCK_ANY,l166,(void*)l75);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tRead memory %s index %d (original) ""return %d\n"),SOC_MEM_NAME(l74,l178),
l166,l144));l475++;continue;}l99 = soc_mem_field32_get(l74,l178,(void*)l75,
MODE0f);if(l99){l308 = 1;l486 = L3_DEFIP_ALPM_IPV6_64m;}else{l308 = 2;l486 = 
L3_DEFIP_ALPM_IPV4m;}for(l217 = 0;l217<l308;l217++){if(soc_mem_field32_get(
l74,l178,(void*)l75,l476[l217]) == 0||soc_mem_field32_get(l74,l178,(void*)l75
,l477[l217]) == 1){continue;}bkt_ptr = soc_mem_field32_get(l74,l178,(void*)
l75,l478[l217]);l141 = soc_mem_field32_get(l74,l178,(void*)l75,l479[l217]);(
void)soc_th_alpm_lpm_vrf_get(l74,l75,&l116,&l132);l144 = 
soc_th_alpm_bucket_is_assigned(l74,bkt_ptr,l132,l99,&l220);if(l144 == 
SOC_E_PARAM||l141>= SOC_TH_MAX_SUB_BUCKETS){LOG_ERROR(BSL_LS_SOC_ALPM,(
BSL_META_U(l74,"\tInvalid bucket pointer [%d,%d] "
"detected, in memory %s index %d\n"),bkt_ptr,l141,SOC_MEM_NAME(l74,l178),l166
));l475++;continue;}if(bkt_ptr!= 0){if(l482[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+
l141] == -1){l482[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l141] = l166;}else{LOG_ERROR
(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"\tDuplicated bucket pointer "
"[%d,%d] detected, in memory %s index1 %d ""and index2 %d\n"),bkt_ptr,l141,
SOC_MEM_NAME(l74,l178),l482[bkt_ptr*SOC_TH_MAX_SUB_BUCKETS+l141],l166));l475
++;continue;}}else{continue;}if(l144>= 0&&l220 == 0){LOG_ERROR(
BSL_LS_SOC_ALPM,(BSL_META_U(l74,"\tFreed bucket pointer %d "
"detected, in memory %s index %d\n"),bkt_ptr,SOC_MEM_NAME(l74,l178),l166));
l475++;continue;}l485 = _soc_tomahawk_alpm_bkt_view_get(l74,
SOC_TH_ALPM_BKT_IDX_TO_ENTRY(l74,ALPM_BKTID(bkt_ptr,0)));if(l486!= l485){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,"\tMismatched bucket entry memory "
"type in bucket %d, expected %s, actual"" %s\n"),bkt_ptr,SOC_MEM_NAME(l74,
l486),SOC_MEM_NAME(l74,l485)));l475++;continue;}l483 = soc_mem_field32_get(
l74,l178,(void*)l75,l487[l217]);if(l217 == 1){l144 = 
soc_th_alpm_lpm_ip4entry1_to_0(l74,l75,l75,PRESERVE_HIT);if(SOC_FAILURE(l144)
){continue;}}l470 = -1;l144 = _soc_th_alpm_find(l74,l75,l486,(void*)l75,&l470
,&l471,&l472,FALSE);if(l470 == -1){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tLaunched AUX operation for PIVOT ""index %d sanity check failed\n"),l166))
;l475++;continue;}l311 = soc_th_alpm_logical_idx(l74,l178,l470>>1,1);l144 = 
soc_mem_read(l74,l178,MEM_BLOCK_ANY,l311,(void*)l75);if(SOC_FAILURE(l144)){
LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tRead memory %s index %d (nexthop) ""return %d\n"),SOC_MEM_NAME(l74,l178),
l311,l144));l475++;continue;}l484[0] = soc_mem_field32_get(l74,l178,(void*)
l75,l487[0]);l484[1] = soc_mem_field32_get(l74,l178,(void*)l75,l487[1]);if(
l483!= l484[l470&1]){LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tDefault nexthop was not populated "
"correctly, TCAM index1 %d entry1 %d, index2 %d ""entry2 %d\n"),l166,l217,
l311,l470&1));l475++;continue;}}}SOC_ALPM_LPM_UNLOCK(l74);sal_free(l482);if(
l475 == 0){LOG_INFO(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tMemory %s index %d Pivot sanity check passed\n"),SOC_MEM_NAME(l74,l178),
index));return SOC_E_NONE;}LOG_ERROR(BSL_LS_SOC_ALPM,(BSL_META_U(l74,
"\tMemory %s index %d Pivot sanity check failed, ""encountered %d error(s)\n"
),SOC_MEM_NAME(l74,l178),index,l475));return SOC_E_FAIL;}
#endif /* ALPM_ENABLE */
