/*
 * $Id: alpm.c 1.21.2.2 Broadcom SDK $
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
 * File:    alpm.c
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
#include <soc/esw/trie.h>


typedef enum l1{l2,l3,l4,l5}l6;typedef struct l7{uint8*l8;int l9;int l10;int
l11;}l12;static l12 l13[SOC_MAX_NUM_DEVICES];typedef struct l14 l15;struct l14
{trie_node_t l16;l15*l17;unsigned int l18[BITS2WORDS((74))];unsigned int l19;
int l20;};void l21(int l22);static int l23(int l22);static int l24(int l22);
static int l25(int l22,void*l26,int*l20);static int l27(int l22,void*l28);
static int l29(int l22,void*l28,int l30,int l31,int l32,
defip_aux_scratch_entry_t*l33);static int l34(int l22,l6 l35,
defip_aux_scratch_entry_t*l33,int*l36,int*l37,int*l38);static int _ipmask2pfx
(uint32 l39,int*l40);static int l41(int l22,void*l28,void*l42,int*l43,int*l44
,int*l30);static int l45(int l22,void*l28,void*l42,int*l43);static int l46(
int l22);static int l47(int l48,void*l49,void*l50,void*l51,soc_mem_t l52,
uint32 l53,uint32*l54);static int l55(int l48,void*l50,soc_mem_t l52,int l30,
int l56,int l57,int l20,void*l49);static int l58(int l48,uint32*l18,int l19,
int l56,int l30,defip_entry_t*l49,int l59);static int l60(int l22,int*l61);
static int l62(int l22,int l61);static int l63(int l22,int l56);typedef struct
l64{int l65;int l66;int l67;int l17;int l68;int l69;}l70,*l71;static l71 l72[
SOC_MAX_NUM_DEVICES];typedef struct l73{soc_field_info_t*l74;soc_field_info_t
*l75;soc_field_info_t*l76;soc_field_info_t*l77;soc_field_info_t*l78;
soc_field_info_t*l79;soc_field_info_t*l80;soc_field_info_t*l81;
soc_field_info_t*l82;soc_field_info_t*l83;soc_field_info_t*l84;
soc_field_info_t*l85;soc_field_info_t*l86;soc_field_info_t*l87;
soc_field_info_t*l88;soc_field_info_t*l89;soc_field_info_t*l90;
soc_field_info_t*l91;soc_field_info_t*l92;soc_field_info_t*l93;
soc_field_info_t*l94;soc_field_info_t*l95;soc_field_info_t*l96;
soc_field_info_t*l97;soc_field_info_t*l98;soc_field_info_t*l99;
soc_field_info_t*l100;soc_field_info_t*l101;soc_field_info_t*l102;
soc_field_info_t*l103;soc_field_info_t*l104;soc_field_info_t*l105;
soc_field_info_t*l106;soc_field_info_t*l107;soc_field_info_t*l108;
soc_field_info_t*l109;soc_field_info_t*l110;soc_field_info_t*l111;
soc_field_info_t*l112;soc_field_info_t*l113;soc_field_info_t*l114;
soc_field_info_t*l115;soc_field_info_t*l116;soc_field_info_t*l117;
soc_field_info_t*l118;soc_field_info_t*l119;}l120,*l121;static l121 l122[
SOC_MAX_NUM_DEVICES];typedef struct l123{int l48;int l124;int l125;uint16*
l126;uint16*l127;}l128;typedef uint32 l129[5];typedef int(*l130)(l129 l131,
l129 l132);static l128*l133[SOC_MAX_NUM_DEVICES];static void l134(int l22,
void*l42,int l20,l129 l135);static uint16 l136(uint8*l137,int l138);static int
l139(int l48,int l124,int l125,l128**l140);static int l141(l128*l142);static
int l143(l128*l144,l130 l145,l129 entry,int l146,uint16*l147);static int l148
(l128*l144,l130 l145,l129 entry,int l146,uint16 l149,uint16 l150);static int
l151(l128*l144,l130 l145,l129 entry,int l146,uint16 l152);typedef struct l153
{trie_t*l154;trie_t*l155;trie_t*l156;trie_t*l157;defip_entry_t*l49;
defip_entry_t*l158;int count;int l159;}l160;static l160 l161[
SOC_MAX_NUM_DEVICES][(1<<(10))];typedef struct l162{trie_t*l163;int l38;}l164
;typedef struct l165{trie_node_t l16;l164*l57;unsigned int l18[BITS2WORDS((74
))];unsigned int l19;int l37;}l166;static l166*l167[SOC_MAX_NUM_DEVICES][
16384];typedef struct{l15*l168[24];int l169[24];int count;}l170;STATIC int
l171(int l22){uint32 l172;SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l22,
&l172));return soc_reg_field_get(l22,L3_DEFIP_RPF_CONTROLr,l172,LOOKUP_MODEf)
;}STATIC int l173(int l22,int l174){int l175;l175 = (l174>>2)&0x3fff;l175+= (
l13[l22].l10);return(l174&~(0x3fff<<2))|(l175<<2);}STATIC int l176(int l22,
void*entry,uint32*l168,uint32*l44,int*l54){int l177,l178,l30;int l146 = 0;int
l179 = SOC_E_NONE;uint32 l180,l181;*l168 = 0;l30 = soc_mem_field32_get(l22,
L3_DEFIPm,entry,MODE0f);l177 = soc_mem_field32_get(l22,L3_DEFIPm,entry,
IP_ADDR0f);l178 = soc_mem_field32_get(l22,L3_DEFIPm,entry,IP_ADDR_MASK0f);
l168[1] = l177;l177 = soc_mem_field32_get(l22,L3_DEFIPm,entry,IP_ADDR1f);l178
= soc_mem_field32_get(l22,L3_DEFIPm,entry,IP_ADDR_MASK1f);l168[0] = l177;if(
l30){l178 = soc_mem_field32_get(l22,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((l179 = 
_ipmask2pfx(l178,&l146))<0){return(l179);}l178 = soc_mem_field32_get(l22,
L3_DEFIPm,entry,IP_ADDR_MASK1f);if(l146){if(l178!= 0xffffffff){return(
SOC_E_PARAM);}l146+= 32;}else{if((l179 = _ipmask2pfx(l178,&l146))<0){return(
l179);}}l180 = 64-l146;if(l180<32){l168[1]>>= l180;l181 = l168[0]<<(32-l180);
l168[0]>>= l180;l168[1]|= l181;}else{l168[1] = l168[0]>>(32-l180);l168[0] = 0
;}}else{l178 = soc_mem_field32_get(l22,L3_DEFIPm,entry,IP_ADDR_MASK0f);if((
l179 = _ipmask2pfx(l178,&l146))<0){return(l179);}l168[1] = l168[1]>>(32-l146)
;l168[0] = 0;}*l44 = l146;*l54 = (l168[0] == 0)&&(l168[1] == 0)&&(l146 == 0);
return SOC_E_NONE;}STATIC int l182(int l22,void*l28,soc_mem_t l52,void*l183,
int*l37,int*l38,int*l43){defip_aux_scratch_entry_t l33;uint32 l42[
SOC_MAX_MEM_FIELD_WORDS];int l184,l56,l185;int l147;uint32 l31,l186;int l179 = 
SOC_E_NONE;int l36 = 0;l185 = soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE0f);
if(l185){if(!(l185 = soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l22,l28,&l184,&l56));
if(l56 == SOC_VRF_MAX(l22)+1){l31 = 0;do{SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l22,&(l186)));if((l186)){if(SOC_URPF_STATUS_GET((
l22))){(l186) = 0x3;}else{(l186) = 0;}}}while(0);}else{l31 = 2;do{
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l22,&(l186)));if((l186)){if(
SOC_URPF_STATUS_GET((l22))){(l186) = 0xC;}else{(l186) = 0;}}}while(0);}if(
l184!= SOC_L3_VRF_OVERRIDE){sal_memset(&l33,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l29(l22,l28,l185,l31,0,&l33))
;SOC_IF_ERROR_RETURN(l34(l22,l4,&l33,&l36,l37,l38));if(l36){l47(l22,l28,l42,0
,l52,0,0);l179 = soc_mem_alpm_lookup(l22,l52,*l38,MEM_BLOCK_ANY,l186,l42,l183
,&l147);if(SOC_SUCCESS(l179)){*l43 = l147;}}else{l179 = SOC_E_NOT_FOUND;}}
return l179;}STATIC int l187(int l22,void*l28,void*l183,void*l188,soc_mem_t
l52,int*l43){defip_aux_scratch_entry_t l33;uint32 l42[SOC_MAX_MEM_FIELD_WORDS
];int l184,l185,l56;int l147,l38;uint32 l31,l186;int l179 = SOC_E_NONE;int l36
= 0;int l37;l185 = soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE0f);if(l185){if(
!(l185 = soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE1f))){return(SOC_E_PARAM);
}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l22,l28,&l184,&l56));if(l56 == 
SOC_VRF_MAX(l22)+1){l31 = 0;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr
(l22,&(l186)));if((l186)){if(SOC_URPF_STATUS_GET((l22))){(l186) = 0x3;}else{(
l186) = 0;}}}while(0);}else{l31 = 2;do{SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l22,&(l186)));if((l186)){if(SOC_URPF_STATUS_GET((
l22))){(l186) = 0xC;}else{(l186) = 0;}}}while(0);}if(!l171(l22)){l31 = 2;}if(
l184!= SOC_L3_VRF_OVERRIDE){sal_memset(&l33,0,sizeof(
defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l29(l22,l28,l185,l31,0,&l33))
;SOC_IF_ERROR_RETURN(l34(l22,l4,&l33,&l36,&l37,&l38));if(l36){l179 = 
soc_mem_alpm_lookup(l22,l52,l38,MEM_BLOCK_ANY,l186,l183,l42,&l147);if(
SOC_SUCCESS(l179)){*l43 = l147;SOC_IF_ERROR_RETURN(soc_mem_write(l22,l52,
MEM_BLOCK_ANY,l147,l183));if(l179!= SOC_E_NONE){return SOC_E_FAIL;}if(
SOC_URPF_STATUS_GET(l22)){SOC_IF_ERROR_RETURN(soc_mem_write(l22,l52,
MEM_BLOCK_ANY,l173(l22,l147),l188));if(l179!= SOC_E_NONE){return SOC_E_FAIL;}
}}}else{return(SOC_E_NOT_FOUND);}}return l179;}static int l189(trie_node_t*
l16,void*l190){l170*l191 = (l170*)l190;if(l16->type == PAYLOAD){l191->l168[
l191->count] = (l15*)l16;l191->count++;}return SOC_E_NONE;}static int l192(
trie_node_t*l16,void*l190){if(l16!= NULL){sal_free(l16);}return SOC_E_NONE;}
STATIC int l193(int l22,int l194,void*entry,void*l195,int l196){uint32 l181,
l178,l185,l31,l197 = 0;soc_mem_t l52 = L3_DEFIPm;soc_mem_t l198 = 
L3_DEFIP_AUX_TABLEm;defip_entry_t l199;int l179 = SOC_E_NONE,l146,l200,l56,
l201;SOC_IF_ERROR_RETURN(soc_mem_read(l22,l198,MEM_BLOCK_ANY,l194,l195));l181
= soc_mem_field32_get(l22,l52,entry,VRF_ID_0f);soc_mem_field32_set(l22,l198,
l195,VRF0f,l181);l181 = soc_mem_field32_get(l22,l52,entry,VRF_ID_1f);
soc_mem_field32_set(l22,l198,l195,VRF1f,l181);l181 = soc_mem_field32_get(l22,
l52,entry,MODE0f);soc_mem_field32_set(l22,l198,l195,MODE0f,l181);l181 = 
soc_mem_field32_get(l22,l52,entry,MODE1f);soc_mem_field32_set(l22,l198,l195,
MODE1f,l181);l185 = l181;l181 = soc_mem_field32_get(l22,l52,entry,VALID0f);
soc_mem_field32_set(l22,l198,l195,VALID0f,l181);l181 = soc_mem_field32_get(
l22,l52,entry,VALID1f);soc_mem_field32_set(l22,l198,l195,VALID1f,l181);l181 = 
soc_mem_field32_get(l22,l52,entry,IP_ADDR_MASK0f);if((l179 = _ipmask2pfx(l181
,&l146))<0){return l179;}l178 = soc_mem_field32_get(l22,l52,entry,
IP_ADDR_MASK1f);if((l179 = _ipmask2pfx(l178,&l200))<0){return l179;}if(l185){
soc_mem_field32_set(l22,l198,l195,IP_LENGTH0f,l146+l200);}else{
soc_mem_field32_set(l22,l198,l195,IP_LENGTH0f,l146);soc_mem_field32_set(l22,
l198,l195,IP_LENGTH1f,l200);}l181 = soc_mem_field32_get(l22,l52,entry,
IP_ADDR0f);soc_mem_field32_set(l22,l198,l195,IP_ADDR0f,l181);l181 = 
soc_mem_field32_get(l22,l52,entry,IP_ADDR1f);soc_mem_field32_set(l22,l198,
l195,IP_ADDR1f,l181);l181 = soc_mem_field32_get(l22,l52,entry,ENTRY_TYPE0f);
soc_mem_field32_set(l22,l198,l195,ENTRY_TYPE0f,l181);l181 = 
soc_mem_field32_get(l22,l52,entry,ENTRY_TYPE1f);soc_mem_field32_set(l22,l198,
l195,ENTRY_TYPE1f,l181);if(!l185){sal_memcpy(&l199,entry,sizeof(l199));l179 = 
soc_alpm_lpm_vrf_get(l22,(void*)&l199,&l56,&l146);SOC_IF_ERROR_RETURN(l179);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_ip4entry1_to_0(l22,&l199,&l199,TRUE));l179 = 
soc_alpm_lpm_vrf_get(l22,(void*)&l199,&l201,&l146);SOC_IF_ERROR_RETURN(l179);
}else{l179 = soc_alpm_lpm_vrf_get(l22,entry,&l56,&l146);}if(
SOC_URPF_STATUS_GET(l22)){if(l196>= (soc_mem_index_count(l22,L3_DEFIPm)>>1)){
l197 = 1;}}switch(l56){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l22,l198,
l195,VALID0f,0);l31 = 0;break;case SOC_L3_VRF_GLOBAL:l31 = l197?1:0;break;
default:l31 = l197?3:2;break;}soc_mem_field32_set(l22,l198,l195,DB_TYPE0f,l31
);if(!l185){switch(l201){case SOC_L3_VRF_OVERRIDE:soc_mem_field32_set(l22,
l198,l195,VALID1f,0);l31 = 0;break;case SOC_L3_VRF_GLOBAL:l31 = l197?1:0;
break;default:l31 = l197?3:2;break;}soc_mem_field32_set(l22,l198,l195,
DB_TYPE1f,l31);}if(l197){l181 = soc_mem_field32_get(l22,l52,entry,
ALG_BKT_PTR0f);l181+= (l13[l22].l10);soc_mem_field32_set(l22,l52,entry,
ALG_BKT_PTR0f,l181);if(!l185){l181 = soc_mem_field32_get(l22,l52,entry,
ALG_BKT_PTR1f);l181+= (l13[l22].l10);soc_mem_field32_set(l22,l52,entry,
ALG_BKT_PTR1f,l181);}}return SOC_E_NONE;}STATIC int l202(int l22,int l203,int
l20,int l204,void*entry){defip_aux_table_entry_t l195;SOC_IF_ERROR_RETURN(
l193(l22,l204,entry,(void*)&l195,l20));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIPm(
l22,MEM_BLOCK_ANY,l20,entry));SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_TABLEm(
l22,MEM_BLOCK_ANY,l20,&l195));return SOC_E_NONE;}STATIC int l205(int l22,void
*l28,soc_mem_t l52,void*l183,void*l188,int*l43){l166*l206,*l207,*l208;
defip_aux_scratch_entry_t l33;uint32 l42[SOC_MAX_MEM_FIELD_WORDS];uint32 l168
[2],l209,l210;uint32 l211[2],l212[2];int l185,l56,l184;int l147,l38;int l179 = 
SOC_E_NONE,l213;uint32 l31,l186;int l36 =0;int l37,l214;int l215 = 0;trie_t*
trie,*l216;trie_node_t*l217,*l218;l15*l219,*l220;defip_entry_t l49;l164*l221;
int l222,l223 = -1,l54 = 0;l170 l191;defip_alpm_ipv4_entry_t l224,l225;
defip_alpm_ipv6_64_entry_t l226,l227;void*l228,*l229;int*l150,l230=0;l185 = 
soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE0f);if(l185){if(!(l185 = 
soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE1f))){return(SOC_E_PARAM);}}
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l22,l28,&l184,&l56));if(l56 == 
SOC_VRF_MAX(l22)+1){l31 = 0;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr
(l22,&(l186)));if((l186)){if(SOC_URPF_STATUS_GET((l22))){(l186) = 0x3;}else{(
l186) = 0;}}}while(0);}else{l31 = 2;do{SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_RPF_CONTROLr(l22,&(l186)));if((l186)){if(SOC_URPF_STATUS_GET((
l22))){(l186) = 0xC;}else{(l186) = 0;}}}while(0);}l52 = (l185)?
L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l228 = ((l185)?((uint32*)&(l226)):
((uint32*)&(l224)));l229 = ((l185)?((uint32*)&(l227)):((uint32*)&(l225)));
sal_memset(&l33,0,sizeof(defip_aux_scratch_entry_t));SOC_IF_ERROR_RETURN(l29(
l22,l28,l185,l31,0,&l33));SOC_IF_ERROR_RETURN(l34(l22,l4,&l33,&l36,&l37,&l38)
);if(l36 == 0){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not find bucket to"" insert prefix\n");return
SOC_E_NOT_FOUND;}l179 = soc_mem_alpm_insert(l22,l52,l38,MEM_BLOCK_ANY,l186,
l183,l42,&l147);if(l179 == SOC_E_NONE){*l43 = l147;if(SOC_URPF_STATUS_GET(l22
)){l213 = soc_mem_write(l22,l52,MEM_BLOCK_ANY,l173(l22,l147),l188);if(
SOC_FAILURE(l213)){return l213;}}}if(l179 == SOC_E_FULL){l215 = 1;}l179 = 
l176(l22,l28,l168,&l210,&l54);if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: prefix create failed\n");return l179;}l206 = l167[l22][l37
];trie = ((l206)->l57)->l163;l208 = l206;l219 = sal_alloc(sizeof(l15),
"Payload for Key");if(l219 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory for ""trie node \n");return
SOC_E_MEMORY;}l220 = sal_alloc(sizeof(l15),"Payload for pfx trie key");if(
l220 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory for ""pfx trie node \n");
sal_free(l219);return SOC_E_MEMORY;}l219->l18[0] = l168[0];l219->l18[1] = 
l168[1];l219->l19 = l210;l219->l20 = l147;sal_memcpy(l220,l219,sizeof(*l219))
;l179 = trie_insert(trie,l168,NULL,l210,(trie_node_t*)l219);if(SOC_FAILURE(
l179)){goto l231;}if(l185){l216 = l161[l22][l56].l157;}else{l216 = l161[l22][
l56].l156;}if(!l54){l179 = trie_insert(l216,l168,NULL,l210,(trie_node_t*)l220
);}l209 = l210;if(SOC_FAILURE(l179)){goto l232;}if(l215){l179 = l60(l22,&l38)
;if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate""new bucket for split\n");goto l233;}
l179 = trie_split(trie,l212,&l210,&l217,l211);if(SOC_FAILURE(l179)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Could not split bucket"
"for prefix 0x%08x 0x%08x\n",l168[0],l168[1]);l213 = l62(l22,l38);if(
SOC_FAILURE(l213)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not free back"" bucket id %d\n",l38);}goto l233;}do{
if(!(l185)){l212[0] = l212[1]<<(32-l210);l212[1] = 0;}else{int l234 = 64-l210
;int l235;if(l234<32){l235 = l212[0]<<l234;l235|= (l212[1]>>(32-l234));l212[0
] = l212[1]<<l234;l212[1] = l235;}else{l212[1] = l212[0]<<(32-l234);l212[0] = 
0;}}}while(0);l58(l22,l212,l210,l56,l185,&l49,0);soc_L3_DEFIPm_field32_set(
l22,&l49,ALG_BKT_PTR0f,l38);l221 = sal_alloc(sizeof(l164),
"ALPM Bucket Handle");if(l221 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n");l179 = 
SOC_E_MEMORY;goto l233;}l206 = sal_alloc(sizeof(l166),"Payload for new Pivot"
);if(l206 == NULL){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to allocate memory ""for PIVOT trie node \n");l179 = 
SOC_E_MEMORY;goto l233;}(l206)->l57 = l221;l179 = trie_init(&((l206)->l57)->
l163);((l206)->l57)->l163->trie = l217;((l206)->l57)->l38 = l38;l206->l18[0] = 
l212[0];l206->l18[1] = l212[1];l206->l19 = l210;sal_memset(&l191,0,sizeof(
l191));l179 = trie_traverse(((l206)->l57)->l163,l189,&l191,
_TRIE_INORDER_TRAVERSE);if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Bucket split failed""for prefix 0x%08x 0x%08x\n",l168[0],
l168[1]);goto l233;}l150 = sal_alloc(sizeof(*l150)*l191.count,
"Temp storage for location of prefixes in new bucket");if(l150 == NULL){l179 = 
SOC_E_MEMORY;goto l233;}sal_memset(l150,-1,sizeof(*l150)*l191.count);for(l222
= 0;l222<l191.count;l222++){l15*l168 = l191.l168[l222];if(l168->l20>0){l179 = 
soc_mem_read(l22,l52,MEM_BLOCK_ANY,l168->l20,l228);if(SOC_FAILURE(l179)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to"
"read prefix 0x%08x 0x%08x for move\n",l191.l168[l222]->l18[0],l191.l168[l222
]->l18[1]);goto l236;}if(SOC_URPF_STATUS_GET(l22)){l179 = soc_mem_read(l22,
l52,MEM_BLOCK_ANY,l173(l22,l168->l20),l229);if(SOC_FAILURE(l179)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to"
"read rpf prefix 0x%08x 0x%08x for move\n",l191.l168[l222]->l18[0],l191.l168[
l222]->l18[1]);goto l236;}}l179 = soc_mem_alpm_insert(l22,l52,l38,
MEM_BLOCK_ANY,l186,l228,l42,&l147);if(SOC_SUCCESS(l179)){if(
SOC_URPF_STATUS_GET(l22)){l179 = soc_mem_write(l22,l52,MEM_BLOCK_ANY,l173(l22
,l147),l229);}}}else{l179 = soc_mem_alpm_insert(l22,l52,l38,MEM_BLOCK_ANY,
l186,l183,l42,&l147);if(SOC_SUCCESS(l179)){l223 = l222;*l43 = l147;l220->l20 = 
l147;if(SOC_URPF_STATUS_GET(l22)){l179 = soc_mem_write(l22,l52,MEM_BLOCK_ANY,
l173(l22,l147),l188);}}}l150[l222] = l147;if(SOC_FAILURE(l179)){soc_cm_debug(
DK_ERR,"_soc_alpm_insert: Failed to""write prefix 0x%08x 0x%08x for move\n",
l191.l168[l222]->l18[0],l191.l168[l222]->l18[1]);goto l236;}}l179 = l25(l22,&
l49,&l214);if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Unable to add new""pivot to tcam\n");goto l236;}l167[l22][
l214<<(l185?1:0)] = l206;((l206)->l37) = l214<<(l185?1:0);l230 = 0;for(l222 = 
0;l222<l191.count;l222++){l15*l168 = l191.l168[l222];if(l168->l20>0){l179 = 
soc_mem_read(l22,l52,MEM_BLOCK_ANY,l168->l20,l228);soc_mem_field32_set(l22,
l52,l228,VALIDf,0);if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Failed to read"
"bkt entry for invalidate for pfx 0x%08x 0x%08x\n",l168->l18[0],l168->l18[1])
;goto l237;}if(SOC_URPF_STATUS_GET(l22)){l179 = soc_mem_read(l22,l52,
MEM_BLOCK_ANY,l173(l22,l168->l20),l229);soc_mem_field32_set(l22,l52,l229,
VALIDf,0);}l179 = soc_mem_write(l22,l52,MEM_BLOCK_ANY,l168->l20,l228);if(
SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,"_soc_alpm_insert: Failed to clear"
" prefixes from old bkt for pfx 0x%08x 0x%08x\n",l168->l18[0],l168->l18[1]);
goto l237;}if(SOC_URPF_STATUS_GET(l22)){l179 = soc_mem_write(l22,l52,
MEM_BLOCK_ANY,l173(l22,l168->l20),l229);}}}for(l222 = 0;l222<l191.count;l222
++){l15*l168 = l191.l168[l222];l168->l20 = l150[l222];}sal_free(l150);if(l223
== -1){l179 = soc_mem_alpm_insert(l22,l52,(l208)->l57->l38,MEM_BLOCK_ANY,l186
,l183,l42,&l147);if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Could not insert new ""prefix into trie after split\n");
goto l233;}if(SOC_URPF_STATUS_GET(l22)){l179 = soc_mem_write(l22,l52,
MEM_BLOCK_ANY,l173(l22,l147),l188);}*l43 = l147;l220->l20 = l219->l20 = l147;
}}l161[(l22)][(l56)].count++;if(l54){SOC_IF_ERROR_RETURN(l34(l22,l3,&l33,&l36
,&l37,&l38));sal_free(l220);}SOC_IF_ERROR_RETURN(l34(l22,l2,&l33,&l36,&l37,&
l38));if(SOC_URPF_STATUS_GET(l22)){l210 = soc_mem_field32_get(l22,
L3_DEFIP_AUX_SCRATCHm,&l33,DB_TYPEf);l210+= 1;soc_mem_field32_set(l22,
L3_DEFIP_AUX_SCRATCHm,&l33,DB_TYPEf,l210);SOC_IF_ERROR_RETURN(l34(l22,l3,&l33
,&l36,&l37,&l38));SOC_IF_ERROR_RETURN(l34(l22,l2,&l33,&l36,&l37,&l38));}
return l179;l237:for(l222 = 0;l222<l191.count;l222++){l15*l168 = l191.l168[
l222];if(l168->l20>0){l213 = soc_mem_read(l22,l52,MEM_BLOCK_ANY,l168->l20,
l228);if(SOC_FAILURE(l213)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Failure to read prefix""0x%08x 0x%08x for move back\n",
l168->l18[0],l168->l18[1]);break;}if(soc_mem_field32_get(l22,l52,l228,VALIDf)
){break;}soc_mem_field32_set(l22,l52,l228,VALIDf,1);l179 = soc_mem_write(l22,
l52,MEM_BLOCK_ALL,l168->l20,l228);if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Failure to write prefix""0x%08x 0x%08x for move back\n",
l168->l18[0],l168->l18[1]);break;}if(SOC_URPF_STATUS_GET(l22)){l213 = 
soc_mem_read(l22,l52,MEM_BLOCK_ANY,l173(l22,l168->l20),l228);
soc_mem_field32_set(l22,l52,l228,VALIDf,1);l213 = soc_mem_write(l22,l52,
MEM_BLOCK_ALL,l173(l22,l168->l20),l228);}}}l213 = l27(l22,&l49);if(
SOC_FAILURE(l213)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: Failure to free new prefix""at %d\n",l214);}l167[l22][l214
] = NULL;l236:l207 = l208;for(l222 = 0;l222<l191.count;l222++){l15*l168 = 
l191.l168[l222];if(l150[l222]!= -1){sal_memset(l228,0,sizeof(l183));l213 = 
soc_mem_write(l22,l52,MEM_BLOCK_ANY,l150[l222],l228);if(SOC_FAILURE(l213)){
soc_cm_debug(DK_ERR,"_soc_alpm_insert: mem write failure"
"in bkt move rollback\n");}}l213 = trie_delete(((l206)->l57)->l163,l168->l18,
l168->l19,&l218);l219 = (l15*)l218;if(SOC_FAILURE(l213)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: trie delete failure""in bkt move rollback\n");}if(l168->
l20>0){l213 = trie_insert(((l207)->l57)->l163,l168->l18,NULL,l168->l19,(
trie_node_t*)l219);if(SOC_FAILURE(l213)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: trie reinsert failure""in bkt move rollback\n");}}}l213 = 
l62(l22,((l206)->l57)->l38);if(SOC_FAILURE(l213)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: new bucket release ""failure: %d\n",((l206)->l57)->l38);}
trie_destroy(((l206)->l57)->l163);sal_free(l221);sal_free(l206);sal_free(l150
);l220 = NULL;l213 = trie_delete(l216,l168,l209,&l218);l220 = (l15*)l218;if(
SOC_FAILURE(l213)){soc_cm_debug(DK_ERR,
"_soc_alpm_insert: failed to delete new prefix"
"0x%08x 0x%08x from pfx trie\n",l168[0],l168[1]);}if(l220){sal_free(l220);}
return l179;l233:(void)trie_delete(l216,l168,l209,&l218);l220 = (l15*)l218;(
void)l62(l22,l38);l232:(void)trie_delete(trie,l168,l209,&l218);l219 = (l15*)
l218;l231:sal_free(l219);sal_free(l220);return l179;}static int l58(int l48,
uint32*l18,int l19,int l56,int l30,defip_entry_t*l49,int l59){uint32 l238;
sal_memset(l49,0,sizeof(defip_entry_t));soc_L3_DEFIPm_field32_set(l48,l49,
VRF_ID_0f,l56&SOC_VRF_MAX(l48));if(l56 == (SOC_VRF_MAX(l48)+1)){
soc_L3_DEFIPm_field32_set(l48,l49,VRF_ID_MASK0f,0);}else{
soc_L3_DEFIPm_field32_set(l48,l49,VRF_ID_MASK0f,SOC_VRF_MAX(l48));}if(l30){
soc_L3_DEFIPm_field32_set(l48,l49,IP_ADDR0f,l18[1]);soc_L3_DEFIPm_field32_set
(l48,l49,IP_ADDR1f,l18[0]);soc_L3_DEFIPm_field32_set(l48,l49,MODE0f,1);
soc_L3_DEFIPm_field32_set(l48,l49,VRF_ID_1f,l56&SOC_VRF_MAX(l48));if(l56 == (
SOC_VRF_MAX(l48)+1)){soc_L3_DEFIPm_field32_set(l48,l49,VRF_ID_MASK1f,0);}else
{soc_L3_DEFIPm_field32_set(l48,l49,VRF_ID_MASK1f,SOC_VRF_MAX(l48));}if(l19>= 
32){l238 = 0xffffffff;soc_mem_field32_set(l48,L3_DEFIPm,l49,IP_ADDR_MASK0f,
l238);l238 = 0xffffffff<<(l19-32);soc_mem_field32_set(l48,L3_DEFIPm,l49,
IP_ADDR_MASK1f,l238);soc_L3_DEFIPm_field32_set(l48,l49,VALID1f,1);}else{l238 = 
~(0xffffffff>>l19);soc_mem_field32_set(l48,L3_DEFIPm,l49,IP_ADDR_MASK0f,l238)
;}}else{soc_L3_DEFIPm_field32_set(l48,l49,IP_ADDR0f,l18[0]);assert(l19<= 32);
l238 = (l19 == 32)?0xffffffff:~(0xffffffff>>l19);soc_mem_field32_set(l48,
L3_DEFIPm,l49,IP_ADDR_MASK0f,l238);}soc_L3_DEFIPm_field32_set(l48,l49,VALID0f
,1);soc_mem_field32_set(l48,L3_DEFIPm,l49,MODE_MASK0f,(1<<
soc_mem_field_length(l48,L3_DEFIPm,MODE_MASK0f))-1);soc_mem_field32_set(l48,
L3_DEFIPm,l49,MODE_MASK1f,(1<<soc_mem_field_length(l48,L3_DEFIPm,MODE_MASK1f)
)-1);soc_mem_field32_set(l48,L3_DEFIPm,l49,ENTRY_TYPE_MASK0f,(1<<
soc_mem_field_length(l48,L3_DEFIPm,ENTRY_TYPE_MASK0f))-1);soc_mem_field32_set
(l48,L3_DEFIPm,l49,ENTRY_TYPE_MASK1f,(1<<soc_mem_field_length(l48,L3_DEFIPm,
ENTRY_TYPE_MASK1f))-1);if(l30){soc_alpm_lpm_ip4entry0_to_1(l48,l49,l49,TRUE);
}return(SOC_E_NONE);}STATIC int l239(int l22,void*l28){l166*l206;
defip_alpm_ipv4_entry_t l224,l225;defip_alpm_ipv6_64_entry_t l226,l227;
defip_alpm_ipv4_entry_t l240;defip_aux_scratch_entry_t l33;uint32 l42[
SOC_MAX_MEM_FIELD_WORDS];soc_mem_t l52;void*l228,*l241,*l229 = NULL;int l184;
int l30;int l179 = SOC_E_NONE,l213;uint32 l168[2];int l185,l56;uint32 l210;
int l147,l38;int l152,l242;uint32 l31,l186;int l37,l36,l54 = 0;trie_t*trie,*
l216;uint32 l243;defip_entry_t l49,*l244;l15*l219 = NULL,*l245 = NULL;
trie_node_t*l218;l30 = l185 = soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE0f);
if(l185){if(!(l185 = soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE1f))){return(
SOC_E_PARAM);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l22,l28,&l184,&l56));
if(l184!= SOC_L3_VRF_OVERRIDE){if(l56 == SOC_VRF_MAX(l22)+1){l31 = 0;do{
SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l22,&(l186)));if((l186)){if(
SOC_URPF_STATUS_GET((l22))){(l186) = 0x3;}else{(l186) = 0;}}}while(0);}else{
l31 = 2;do{SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l22,&(l186)));if((
l186)){if(SOC_URPF_STATUS_GET((l22))){(l186) = 0xC;}else{(l186) = 0;}}}while(
0);}l179 = l176(l22,l28,l168,&l210,&l54);if(SOC_FAILURE(l179)){soc_cm_debug(
DK_ERR,"_soc_alpm_delete: prefix create failed\n");return l179;}if(!l171(l22)
){if(l184!= SOC_L3_VRF_GLOBAL){if(l161[(l22)][(l56)].count>1){if(l54){
soc_cm_debug(DK_ERR,"VRF %d: Cannot delete default "
"route if other routes are present in ""this mode",l56);return SOC_E_PARAM;}}
}l31 = 2;}l52 = (l185)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l228 = ((
l185)?((uint32*)&(l226)):((uint32*)&(l224)));soc_mem_lock(l22,L3_DEFIPm);l179
= l182(l22,l28,l52,l228,&l37,&l38,&l147);sal_memcpy(&l240,l228,sizeof(l224));
l241 = &l240;if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to find ""prefix for delete\n");return l179;}l152 = 
l147;l242 = l38;l206 = l167[l22][l37];trie = ((l206)->l57)->l163;l179 = 
trie_delete(trie,l168,l210,&l218);l219 = (l15*)l218;if(l179!= SOC_E_NONE){
soc_cm_debug(DK_ERR,"_soc_alpm_delete: Error prefix not present ""in trie \n"
);goto l246;}if(l185){l216 = l161[l22][l56].l157;}else{l216 = l161[l22][l56].
l156;}if(!l54){l179 = trie_delete(l216,l168,l210,&l218);l245 = (l15*)l218;if(
SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Prefix not present in pfx""trie: 0x%08x 0x%08x\n",l168[0],
l168[1]);goto l247;}l179 = trie_find_prefix_bpm(l216,l168,l210,&l243);if(
SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Could not find replacement"
"bpm for prefix: 0x%08x 0x%08x\n",l168[0],l168[1]);goto l247;}l179 = l58(l22,
l168,l243,l56,l30,&l49,0);l179 = l182(l22,&l49,l52,l228,&l37,&l38,&l147);if(
SOC_URPF_STATUS_GET(l22)){if(SOC_SUCCESS(l179)){l229 = ((l185)?((uint32*)&(
l227)):((uint32*)&(l225)));l213 = soc_mem_read(l22,l52,MEM_BLOCK_ANY,l173(l22
,l147),l229);}}if(l243 == 0){l244 = l185?l161[l22][l56].l158:l161[l22][l56].
l49;sal_memcpy(&l49,l244,sizeof(l49));l179 = l58(l22,l168,l243,l56,l30,&l49,0
);}if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Could not find replacement"
" prefix for prefix: 0x%08x 0x%08x\n",l168[0],l168[1]);goto l247;}l244 = &l49
;}else{l243 = 0;l244 = l185?l161[l22][l56].l158:l161[l22][l56].l49;}l179 = 
l29(l22,l244,l185,l31,l243,&l33);if(SOC_FAILURE(l179)){goto l247;}l179 = l34(
l22,l3,&l33,&l36,&l37,&l38);if(SOC_FAILURE(l179)){goto l247;}if(
SOC_URPF_STATUS_GET(l22)){uint32 l181;if(l229!= NULL){l181 = 
soc_mem_field32_get(l22,L3_DEFIP_AUX_SCRATCHm,&l33,DB_TYPEf);l181++;
soc_mem_field32_set(l22,L3_DEFIP_AUX_SCRATCHm,&l33,DB_TYPEf,l181);l181 = 
soc_mem_field32_get(l22,l52,l229,SRC_DISCARDf);soc_mem_field32_set(l22,l52,&
l33,SRC_DISCARDf,l181);l181 = soc_mem_field32_get(l22,l52,l229,DEFAULTROUTEf)
;soc_mem_field32_set(l22,l52,&l33,DEFAULTROUTEf,l181);l179 = l34(l22,l3,&l33,
&l36,&l37,&l38);}if(SOC_FAILURE(l179)){goto l247;}}sal_free(l219);if(!l54){
sal_free(l245);}l179 = soc_mem_alpm_delete(l22,l52,l242,MEM_BLOCK_ALL,l186,
l241,l42,&l147);if(!SOC_SUCCESS(l179)){l179 = SOC_E_FAIL;return l179;}if(
SOC_URPF_STATUS_GET(l22)){l179 = soc_mem_alpm_delete(l22,l52,l242+(l13[l22].
l10),MEM_BLOCK_ALL,l186,l241,l42,&l36);if(!SOC_SUCCESS(l179)){l179 = 
SOC_E_FAIL;return l179;}}if((l206->l19!= 0)&&(trie->trie == NULL)){l58(l22,
l206->l18,l206->l19,l56,l30,&l49,0);l179 = l27(l22,&l49);if(SOC_FAILURE(l179)
){soc_cm_debug(DK_ERR,"_soc_alpm_delete: Unable to "
"delete pivot 0x%08x 0x%08x \n",l206->l18[0],l206->l18[1]);}l179 = l62(l22,((
l206)->l57)->l38);if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to release""empty bucket: %d\n",((l206)->l57)->l38)
;}trie_destroy(((l206)->l57)->l163);sal_free((l206)->l57);sal_free(l206);}}
l161[(l22)][(l56)].count--;if(l161[(l22)][(l56)].count == 0){l179 = l63(l22,
l56);}return l179;l247:l213 = trie_insert(l216,l168,NULL,l210,(trie_node_t*)
l245);if(SOC_FAILURE(l213)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into pfx trie\n",
l168[0],l168[1]);}l246:l213 = trie_insert(trie,l168,NULL,l210,(trie_node_t*)
l219);if(SOC_FAILURE(l213)){soc_cm_debug(DK_ERR,
"_soc_alpm_delete: Unable to reinsert""prefix 0x%08x 0x%08x into bkt trie\n",
l168[0],l168[1]);}return l179;}int soc_alpm_init(int l22){int l222;int l179 = 
SOC_E_NONE;l179 = l23(l22);l179 = l46(l22);for(l222 = 0;l222<16384;l222++){
l167[l22][l222] = NULL;}return SOC_E_NONE;}static int l248(int l22){int l222,
l179;l166*l181;sal_memset(l13,0,sizeof(l13));sal_memset(l161,0,sizeof(l161));
for(l222 = 0;l222<16384;l222++){l181 = l167[l22][l222];if(l181){if((l181)->
l57){if(((l181)->l57)->l163){l179 = trie_traverse(((l181)->l57)->l163,l192,
NULL,_TRIE_INORDER_TRAVERSE);if(SOC_SUCCESS(l179)){trie_destroy(((l181)->l57)
->l163);}else{soc_cm_debug(DK_ERR,"Unable to clear trie state for "
"unit %d\n",l22);return l179;}}sal_free((l181)->l57);}sal_free(l167[l22][l222
]);l167[l22][l222] = NULL;}}return SOC_E_NONE;}int soc_alpm_deinit(int l22){
l24(l22);SOC_IF_ERROR_RETURN(l248(l22));return SOC_E_NONE;}static int l249(
int l22,int l56){defip_entry_t*l49;defip_entry_t*l158;int l250,l251;int l252,
l253;int l179 = SOC_E_NONE;uint32 l18[2] = {0,0};uint32 l210;l164*l254;l164*
l255;l166*l206;l15*l245,*l256;trie_init(&l161[l22][l56].l156);trie_init(&l161
[l22][l56].l157);l49 = sal_alloc(sizeof(defip_entry_t),
"Default IPv4 LPM entry");if(l49 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv4 LPM entry\n");return
SOC_E_MEMORY;}l158 = sal_alloc(sizeof(defip_entry_t),"Default IPv6 LPM entry"
);if(l158 == NULL){sal_free(l49);soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: unable to allocate memory for ""IPv6 LPM entry\n");return
SOC_E_MEMORY;}l58(l22,l18,0,l56,0,l49,0);l58(l22,l18,0,l56,1,l158,0);l161[l22
][l56].l49 = l49;l161[l22][l56].l158 = l158;if(l56 == SOC_VRF_MAX(l22)+1){
soc_L3_DEFIPm_field32_set(l22,l49,GLOBAL_ROUTE0f,1);soc_L3_DEFIPm_field32_set
(l22,l158,GLOBAL_ROUTE0f,1);}else{soc_L3_DEFIPm_field32_set(l22,l49,
DEFAULT_MISS0f,1);soc_L3_DEFIPm_field32_set(l22,l158,DEFAULT_MISS0f,1);}l179 = 
l60(l22,&l250);soc_L3_DEFIPm_field32_set(l22,l49,ALG_BKT_PTR0f,l250);l179 = 
l60(l22,&l251);soc_L3_DEFIPm_field32_set(l22,l158,ALG_BKT_PTR0f,l251);l179 = 
l25(l22,l49,&l252);l179 = l25(l22,l158,&l253);l254 = sal_alloc(sizeof(l164),
"ALPM Bucket Handle");if(l254 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n");
return SOC_E_NONE;}l206 = sal_alloc(sizeof(l166),"Payload for Pivot");if(l206
== NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n");
sal_free(l254);return SOC_E_MEMORY;}l245 = sal_alloc(sizeof(l15),
"Payload for pfx trie key");if(l245 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n");
sal_free(l254);sal_free(l206);return SOC_E_MEMORY;}l256 = sal_alloc(sizeof(
l15),"Payload for pfx trie key");if(l256 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n");
sal_free(l245);sal_free(l254);sal_free(l206);return SOC_E_MEMORY;}l210 = 0;(
l206)->l57 = l254;trie_init(&((l206)->l57)->l163);((l206)->l57)->l38 = l250;(
(l206)->l37) = l252;l206->l18[0] = l245->l18[0] = l18[0];l206->l18[1] = l245
->l18[1] = l18[1];l206->l19 = l245->l19 = l210;l179 = trie_insert(l161[l22][
l56].l156,l18,NULL,l210,&(l245->l16));if(SOC_FAILURE(l179)){sal_free(l256);
sal_free(l245);sal_free(l206);sal_free(l254);return l179;}l167[l22][l252] = 
l206;l255 = sal_alloc(sizeof(l164),"ALPM Bucket Handle");if(l255 == NULL){
soc_cm_debug(DK_ERR,"soc_alpm_vrf_add: Unable to allocate memory for "
"PIVOT trie node \n");return SOC_E_NONE;}l206 = sal_alloc(sizeof(l166),
"Payload for Pivot");if(l206 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""PIVOT trie node \n");
sal_free(l255);return SOC_E_MEMORY;}l245 = sal_alloc(sizeof(l15),
"Payload for pfx trie key");if(l245 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n");
sal_free(l255);sal_free(l206);return SOC_E_MEMORY;}l256 = sal_alloc(sizeof(
l15),"Payload for pfx trie key");if(l256 == NULL){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_add: Unable to allocate memory for ""pfx trie node \n");
sal_free(l245);sal_free(l254);sal_free(l206);return SOC_E_MEMORY;}l210 = 0;(
l206)->l57 = l255;trie_init(&((l206)->l57)->l163);((l206)->l57)->l38 = l251;(
(l206)->l37) = l253<<1;l206->l18[0] = l245->l18[0] = l18[0];l206->l18[1] = 
l245->l18[1] = l18[1];l206->l19 = l245->l19 = l210;l179 = trie_insert(l161[
l22][l56].l157,l18,NULL,l210,&l245->l16);if(SOC_FAILURE(l179)){soc_cm_debug(
DK_ERR,"soc_alpm_vrf_add: Unable to add vrf default route"
" for v6_64 vrf: %d\n",l56);trie_delete(l161[l22][l56].l157,l18,l210,&l161[
l22][l56].l157->trie);sal_free(l255);sal_free(l256);sal_free(l206);sal_free(
l245);return l179;}l167[l22][l253<<1] = l206;l161[l22][l56].l159 = 1;return
l179;}static int l63(int l22,int l56){defip_entry_t*l49;defip_entry_t*l158;
int l61;int l257,l258;int l179 = SOC_E_NONE;uint32 l18[2] = {0,0},l259[
SOC_MAX_MEM_FIELD_WORDS];l15*l219;l166*l260,*l207;trie_node_t*l218;l49 = l161
[l22][l56].l49;l158 = l161[l22][l56].l158;l61 = soc_L3_DEFIPm_field32_get(l22
,l49,ALG_BKT_PTR0f);l179 = l62(l22,l61);l61 = soc_L3_DEFIPm_field32_get(l22,
l158,ALG_BKT_PTR0f);l179 = l62(l22,l61);l179 = l45(l22,l49,(void*)l259,&l257)
;if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_delete: unable to get internal v4"" pivot idx for vrf %d\n",l56
);l257 = -1;}l179 = l45(l22,l158,(void*)l259,&l258);if(SOC_FAILURE(l179)){
soc_cm_debug(DK_ERR,"soc_alpm_vrf_delete: unable to get internal v6"
" pivot idx for vrf %d\n",l56);l258 = -1;}l260 = l167[l22][l257];l207 = l167[
l22][l258<<1];l179 = l27(l22,l49);if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal v4 for vrf %d\n",l56);}l179 = l27(l22,l158);if(SOC_FAILURE(
l179)){soc_cm_debug(DK_ERR,"soc_alpm_vrf_delete: unable to delete lpm entry "
" for internal v6 for vrf %d\n",l56);}sal_free(l49);sal_free(l158);l161[l22][
l56].l49 = NULL;l161[l22][l56].l158 = NULL;l161[l22][l56].l159 = 0;l179 = 
trie_delete(l161[l22][l56].l156,l18,0,&l218);l219 = (l15*)l218;if(SOC_FAILURE
(l179)){soc_cm_debug(DK_ERR,"Unable to delete internal v4 default for vrf "
" %d\n",l56);}sal_free(l219);l179 = trie_delete(l161[l22][l56].l157,l18,0,&
l218);l219 = (l15*)l218;if(SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"Unable to delete internal v6 default for vrf "" %d\n",l56);}sal_free(l219);(
void)trie_destroy(l161[l22][l56].l156);(void)trie_destroy(l161[l22][l56].l157
);sal_free((l260)->l57);sal_free(l260);sal_free((l207)->l57);sal_free(l207);
return l179;}int soc_alpm_insert(int l22,void*l26,uint32 l53){
defip_alpm_ipv4_entry_t l224,l225;defip_alpm_ipv6_64_entry_t l226,l227;
soc_mem_t l52;void*l228,*l241;int l184,l56;int l20;int l30;int l179 = 
SOC_E_NONE;uint32 l54;l30 = soc_mem_field32_get(l22,L3_DEFIPm,l26,MODE0f);l52
= (l30)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l228 = ((l30)?((uint32*)&(
l226)):((uint32*)&(l224)));l241 = ((l30)?((uint32*)&(l227)):((uint32*)&(l225)
));SOC_IF_ERROR_RETURN(l47(l22,l26,l228,l241,l52,l53,&l54));
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l22,l26,&l184,&l56));if(l184 == 
SOC_L3_VRF_OVERRIDE){l179 = l25(l22,l26,&l20);return(l179);}if(l184!= 
SOC_L3_VRF_GLOBAL){if(!l171(l22)){if(l161[(l22)][(l56)].count == 0){if(!l54){
soc_cm_debug(DK_ERR,"VRF %d: First route in a VRF has to "
" be a default route in this mode\n",l184);return SOC_E_PARAM;}}}}if(!(l161[
l22][l56].l159 == 1)){soc_cm_debug(DK_VERBOSE,
"soc_alpm_insert:VRF %d is not ""initialized\n",l56);l179 = l249(l22,l56);if(
SOC_FAILURE(l179)){soc_cm_debug(DK_ERR,
"soc_alpm_insert:VRF %d trie init failed\n",l56);return l179;}soc_cm_debug(
DK_VERBOSE,"soc_alpm_insert:VRF %d trie init ""completed\n",l56);}l179 = l187
(l22,l26,l228,l241,l52,&l20);if(l179 == SOC_E_NOT_FOUND){l179 = l205(l22,l26,
l52,l228,l241,&l20);}if(l179!= SOC_E_NONE){soc_cm_debug(DK_WARN,
"unit %d :soc_alpm_insert: ""Route Insertion Failed :%s\n",l22,soc_errmsg(
l179));}return(l179);}int soc_alpm_lookup(int l22,void*l28,void*l42,int*l43){
defip_alpm_ipv4_entry_t l224;defip_alpm_ipv6_64_entry_t l226;soc_mem_t l52;
int l38;int l37;void*l228;int l184,l56;int l30,l146;int l179 = SOC_E_NONE;l30
= soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE0f);SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l22,l28,&l184,&l56));l179 = l41(l22,l28,l42,l43,&l146,&
l30);if(SOC_SUCCESS(l179)){if(!l30&&(*l43&0x1)){l179 = 
soc_alpm_lpm_ip4entry1_to_0(l22,l42,l42,TRUE);}SOC_IF_ERROR_RETURN(
soc_alpm_lpm_vrf_get(l22,l42,&l184,&l56));if(l184 == SOC_L3_VRF_OVERRIDE){
return SOC_E_NONE;}}if(!(l161[l22][l56].l159 == 1)){soc_cm_debug(DK_VERBOSE,
"soc_alpm_lookup:VRF %d is not ""initialized\n",l56);return SOC_E_NOT_FOUND;}
l52 = (l30)?L3_DEFIP_ALPM_IPV6_64m:L3_DEFIP_ALPM_IPV4m;l228 = ((l30)?((uint32
*)&(l226)):((uint32*)&(l224)));soc_mem_lock(l22,L3_DEFIPm);l179 = l182(l22,
l28,l52,l228,&l37,&l38,l43);soc_mem_unlock(l22,L3_DEFIPm);if(SOC_FAILURE(l179
)){return l179;}l179 = l55(l22,l228,l52,l30,l184,l38,*l43,l42);return(l179);}
int soc_alpm_delete(int l22,void*l28){int l184,l56;int l30;int l179 = 
SOC_E_NONE;l30 = soc_mem_field32_get(l22,L3_DEFIPm,l28,MODE0f);
SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l22,l28,&l184,&l56));if(l184 == 
SOC_L3_VRF_OVERRIDE){l179 = l27(l22,l28);return(l179);}else{if(!(l161[l22][
l56].l159 == 1)){soc_cm_debug(DK_VERBOSE,"soc_alpm_delete:VRF %d is not "
"initialized\n",l56);return SOC_E_NONE;}l179 = l239(l22,l28);}return(l179);}
STATIC int l46(int l22){int l261;int l222;l261 = soc_mem_index_count(l22,
L3_DEFIPm);if(SOC_URPF_STATUS_GET(l22)){l261>>= 1;}(l13[l22].l10) = l261*2;(
l13[l22].l9) = (l261*2)/8;(l13[l22].l8) = sal_alloc((l13[l22].l9),
"alpm_shared_bucket_bitmap");if((l13[l22].l8) == NULL){soc_cm_debug(DK_WARN,
"soc_alpm_shared_mem_init: Memory allocation for bucket bitmap management failed\n"
);return SOC_E_MEMORY;}for(l222 = 0;l222<(l13[l22].l9);l222++){(l13[l22].l8[
l222]) = 0xff;}(l13[l22].l11) = 1;return SOC_E_NONE;}static int l60(int l22,
int*l61){int l262,l263,l222;if(((l13[l22].l11) == 0)?1:0){return SOC_E_FULL;}
(l13[l22].l8[(l13[l22].l11)/8]&= (~(0x80>>((l13[l22].l11)%8))));*l61 = (l13[
l22].l11);l263 = 0;l262 = (l13[l22].l11);for(l222 = l262+1;l222<(l13[l22].l10
);l222++){if(!(l13[l22].l8[l222/8]&(0x80>>(l222%8)))?0:1){l263 = l222;break;}
}if(l263 == 0){for(l222 = 1;l222<l262;l222++){if(!(l13[l22].l8[l222/8]&(0x80
>>(l222%8)))?0:1){l263 = l222;break;}}}(l13[l22].l11) = l263;return SOC_E_NONE
;}static int l62(int l22,int l61){if((l61<1)&&(l61>(l13[l22].l10-1))){return
SOC_E_PARAM;}(l13[l22].l8[l61/8]|= (0x80>>(l61%8)));return SOC_E_NONE;}static
void l134(int l22,void*l42,int l20,l129 l135){if(l20&(0x8000)){l135[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l88));l135[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l42),(l122[(l22)]->l90));l135[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l89));l135[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l42),(l122[(l22)]->l91));if((!(SOC_IS_HURRICANE(l22)))&&(((l122[
(l22)]->l104)!= NULL))){int l264;(void)soc_alpm_lpm_vrf_get(l22,l42,(int*)&
l135[4],&l264);}else{l135[4] = 0;};}else{if(l20&0x1){l135[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l89));l135[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l42),(l122[(l22)]->l91));l135[2] = 0;l135[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l22)))&&(((l122[(l22)]->l105)!= NULL))){int l264;
defip_entry_t l265;(void)soc_alpm_lpm_ip4entry1_to_0(l22,l42,&l265,0);(void)
soc_alpm_lpm_vrf_get(l22,&l265,(int*)&l135[4],&l264);}else{l135[4] = 0;};}
else{l135[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l42),(l122[(l22)]->l88));l135[1] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[(l22)]->l90));l135[2] = 0;l135[3] = 
0x80000001;if((!(SOC_IS_HURRICANE(l22)))&&(((l122[(l22)]->l104)!= NULL))){int
l264;(void)soc_alpm_lpm_vrf_get(l22,l42,(int*)&l135[4],&l264);}else{l135[4] = 
0;};}}}static int l266(l129 l131,l129 l132){int l174;for(l174 = 0;l174<5;l174
++){{if((l131[l174])<(l132[l174])){return-1;}if((l131[l174])>(l132[l174])){
return 1;}};}return(0);}void l267(int l22,void*l26,uint32 l268,uint32 l149,
int l146){l129 l269;if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l26),(l122[(l22)]->l92))){if(soc_meminfo_fieldinfo_field32_get((
&SOC_MEM_INFO(l22,L3_DEFIPm)),(l26),(l122[(l22)]->l103))&&
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l26),(l122[
(l22)]->l102))){l269[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l26),(l122[(l22)]->l88));l269[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l26),(l122[
(l22)]->l90));l269[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l26),(l122[(l22)]->l89));l269[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l26),(l122[
(l22)]->l91));if((!(SOC_IS_HURRICANE(l22)))&&(((l122[(l22)]->l104)!= NULL))){
int l264;(void)soc_alpm_lpm_vrf_get(l22,l26,(int*)&l269[4],&l264);}else{l269[
4] = 0;};l148((l133[(l22)]),l266,l269,l146,l149,((uint16)l268<<1)|(0x8000));}
}else{if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l26),(l122[(l22)]->l102))){l269[0] = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l26),(l122[(l22)]->l88));l269[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l26),(l122[
(l22)]->l90));l269[2] = 0;l269[3] = 0x80000001;if((!(SOC_IS_HURRICANE(l22)))
&&(((l122[(l22)]->l104)!= NULL))){int l264;(void)soc_alpm_lpm_vrf_get(l22,l26
,(int*)&l269[4],&l264);}else{l269[4] = 0;};l148((l133[(l22)]),l266,l269,l146,
l149,((uint16)l268<<1));}if(soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l26),(l122[(l22)]->l103))){l269[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l26),(l122[
(l22)]->l89));l269[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l26),(l122[(l22)]->l91));l269[2] = 0;l269[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l22)))&&(((l122[(l22)]->l105)!= NULL))){int l264;
defip_entry_t l265;(void)soc_alpm_lpm_ip4entry1_to_0(l22,l26,&l265,0);(void)
soc_alpm_lpm_vrf_get(l22,&l265,(int*)&l269[4],&l264);}else{l269[4] = 0;};l148
((l133[(l22)]),l266,l269,l146,l149,(((uint16)l268<<1)+1));}}}void l270(int l22
,void*l28,uint32 l268){l129 l269;int l146 = -1;int l179;uint16 l20;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l28),(l122[
(l22)]->l92))){l269[0] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22
,L3_DEFIPm)),(l28),(l122[(l22)]->l88));l269[1] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l28),(l122[
(l22)]->l90));l269[2] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l28),(l122[(l22)]->l89));l269[3] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l28),(l122[
(l22)]->l91));if((!(SOC_IS_HURRICANE(l22)))&&(((l122[(l22)]->l104)!= NULL))){
int l264;(void)soc_alpm_lpm_vrf_get(l22,l28,(int*)&l269[4],&l264);}else{l269[
4] = 0;};l20 = (l268<<1)|(0x8000);}else{l269[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l28),(l122[
(l22)]->l88));l269[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l28),(l122[(l22)]->l90));l269[2] = 0;l269[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l22)))&&(((l122[(l22)]->l104)!= NULL))){int l264;(void)
soc_alpm_lpm_vrf_get(l22,l28,(int*)&l269[4],&l264);}else{l269[4] = 0;};l20 = 
l268;}l179 = l151((l133[(l22)]),l266,l269,l146,l20);if(SOC_FAILURE(l179)){
soc_cm_debug(DK_ERR,"\ndel  index: H %d error %d\n",l20,l179);}}int l271(int
l22,void*l28,int l146,int*l147){l129 l269;int l272;int l179;uint16 l20 = (
0xFFFF);l272 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l28),(l122[(l22)]->l92));if(l272){l269[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l28),(l122[
(l22)]->l88));l269[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l28),(l122[(l22)]->l90));l269[2] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l28),(l122[
(l22)]->l89));l269[3] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l28),(l122[(l22)]->l91));if((!(SOC_IS_HURRICANE(l22)))&&(((l122[
(l22)]->l104)!= NULL))){int l264;(void)soc_alpm_lpm_vrf_get(l22,l28,(int*)&
l269[4],&l264);}else{l269[4] = 0;};}else{l269[0] = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l28),(l122[
(l22)]->l88));l269[1] = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l28),(l122[(l22)]->l90));l269[2] = 0;l269[3] = 0x80000001;if((!(
SOC_IS_HURRICANE(l22)))&&(((l122[(l22)]->l104)!= NULL))){int l264;(void)
soc_alpm_lpm_vrf_get(l22,l28,(int*)&l269[4],&l264);}else{l269[4] = 0;};}l179 = 
l143((l133[(l22)]),l266,l269,l146,&l20);if(SOC_FAILURE(l179)){*l147 = 
0xFFFFFFFF;return(l179);}*l147 = l20;return(SOC_E_NONE);}static uint16 l136(
uint8*l137,int l138){return(_shr_crc16b(0,l137,l138));}static int l139(int l48
,int l124,int l125,l128**l140){l128*l144;int l20;if(l125>l124){return
SOC_E_MEMORY;}l144 = sal_alloc(sizeof(l128),"lpm_hash");if(l144 == NULL){
return SOC_E_MEMORY;}sal_memset(l144,0,sizeof(*l144));l144->l48 = l48;l144->
l124 = l124;l144->l125 = l125;l144->l126 = sal_alloc(l144->l125*sizeof(*(l144
->l126)),"hash_table");if(l144->l126 == NULL){sal_free(l144);return
SOC_E_MEMORY;}l144->l127 = sal_alloc(l144->l124*sizeof(*(l144->l127)),
"link_table");if(l144->l127 == NULL){sal_free(l144->l126);sal_free(l144);
return SOC_E_MEMORY;}for(l20 = 0;l20<l144->l125;l20++){l144->l126[l20] = (
0xFFFF);}for(l20 = 0;l20<l144->l124;l20++){l144->l127[l20] = (0xFFFF);}*l140 = 
l144;return SOC_E_NONE;}static int l141(l128*l142){if(l142!= NULL){sal_free(
l142->l126);sal_free(l142->l127);sal_free(l142);}return SOC_E_NONE;}static int
l143(l128*l144,l130 l145,l129 entry,int l146,uint16*l147){int l22 = l144->l48
;uint16 l273;uint16 l20;l273 = l136((uint8*)entry,(32*5))%l144->l125;l20 = 
l144->l126[l273];;;while(l20!= (0xFFFF)){uint32 l42[SOC_MAX_MEM_FIELD_WORDS];
l129 l135;int l274;l274 = (l20&(0x3FFF))>>1;SOC_IF_ERROR_RETURN(
READ_L3_DEFIPm(l22,MEM_BLOCK_ANY,l274,l42));l134(l22,l42,l20,l135);if((*l145)
(entry,l135) == 0){*l147 = (l20&(0x3FFF))>>((l20&(0x8000))?1:0);;return(
SOC_E_NONE);}l20 = l144->l127[l20&(0x3FFF)];;};return(SOC_E_NOT_FOUND);}
static int l148(l128*l144,l130 l145,l129 entry,int l146,uint16 l149,uint16
l150){int l22 = l144->l48;uint16 l273;uint16 l20;uint16 l275;l273 = l136((
uint8*)entry,(32*5))%l144->l125;l20 = l144->l126[l273];;;;l275 = (0xFFFF);if(
l149!= (0xFFFF)){while(l20!= (0xFFFF)){uint32 l42[SOC_MAX_MEM_FIELD_WORDS];
l129 l135;int l274;l274 = (l20&(0x3FFF))>>1;SOC_IF_ERROR_RETURN(
READ_L3_DEFIPm(l22,MEM_BLOCK_ANY,l274,l42));l134(l22,l42,l20,l135);if((*l145)
(entry,l135) == 0){if(l150!= l20){;if(l275 == (0xFFFF)){l144->l126[l273] = 
l150;l144->l127[l150&(0x3FFF)] = l144->l127[l20&(0x3FFF)];l144->l127[l20&(
0x3FFF)] = (0xFFFF);}else{l144->l127[l275&(0x3FFF)] = l150;l144->l127[l150&(
0x3FFF)] = l144->l127[l20&(0x3FFF)];l144->l127[l20&(0x3FFF)] = (0xFFFF);}};
return(SOC_E_NONE);}l275 = l20;l20 = l144->l127[l20&(0x3FFF)];;}}l144->l127[
l150&(0x3FFF)] = l144->l126[l273];l144->l126[l273] = l150;return(SOC_E_NONE);
}static int l151(l128*l144,l130 l145,l129 entry,int l146,uint16 l152){uint16
l273;uint16 l20;uint16 l275;l273 = l136((uint8*)entry,(32*5))%l144->l125;l20 = 
l144->l126[l273];;;l275 = (0xFFFF);while(l20!= (0xFFFF)){if(l152 == l20){;if(
l275 == (0xFFFF)){l144->l126[l273] = l144->l127[l152&(0x3FFF)];l144->l127[
l152&(0x3FFF)] = (0xFFFF);}else{l144->l127[l275&(0x3FFF)] = l144->l127[l152&(
0x3FFF)];l144->l127[l152&(0x3FFF)] = (0xFFFF);}return(SOC_E_NONE);}l275 = l20
;l20 = l144->l127[l20&(0x3FFF)];;}return(SOC_E_NOT_FOUND);}static int
_ipmask2pfx(uint32 l39,int*l40){*l40 = 0;while(l39&(1<<31)){*l40+= 1;l39<<= 1
;}return((l39)?SOC_E_PARAM:SOC_E_NONE);}int soc_alpm_lpm_ip4entry0_to_0(int
l22,void*l276,void*l277,int l278){uint32 l279;l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l102));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l102),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l92));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l92),(l279));l279 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l88));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l88),(l279));l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l90));soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122[(l22)]->l90),(l279));if(((l122[(
l22)]->l78)!= NULL)){l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l276),(l122[(l22)]->l78));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122[(l22)]->l78),(l279));}if(((l122[(
l22)]->l80)!= NULL)){l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l276),(l122[(l22)]->l80));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122[(l22)]->l80),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l82));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l82),(l279));}else{l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l96));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l96),(l279));}l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l98));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l98),(l279));l279 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l100));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l100),(l279));if(((l122[(l22)]->l104)!= NULL)){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l104));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l104),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l106));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l106),(l279));}if(((l122[(l22)]->l76)!= NULL
)){l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l276),(l122[(l22)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l277),(l122[(l22)]->l76),(l279));}if(l278){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l86),(l279));}l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l108));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l108),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l110));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l110),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l112));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l112),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l114));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l114),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l116));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l116),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l118));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l118),(l279));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_1(int l22,void*l276,void*l277,int l278){uint32 l279
;l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l276),(l122[(l22)]->l103));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l277),(l122[(l22)]->l103),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l93));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l93),(l279));l279 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l89));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l89),(l279));l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l91));soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122[(l22)]->l91),(l279));if(((l122[(
l22)]->l79)!= NULL)){l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l276),(l122[(l22)]->l79));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122[(l22)]->l79),(l279));}if(((l122[(
l22)]->l81)!= NULL)){l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l276),(l122[(l22)]->l81));soc_meminfo_fieldinfo_field32_set(
(&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122[(l22)]->l81),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l83));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l83),(l279));}else{l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l97));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l97),(l279));}l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l99));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l99),(l279));l279 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l101));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l101),(l279));if(((l122[(l22)]->l105)!= NULL)){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l105));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l105),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l107));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l107),(l279));}if(((l122[(l22)]->l77)!= NULL
)){l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l276),(l122[(l22)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l277),(l122[(l22)]->l77),(l279));}if(l278){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l87),(l279));}l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l109));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l109),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l111));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l111),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l113));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l113),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l115));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l115),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l117));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l117),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l119));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l119),(l279));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry0_to_1(int l22,void*l276,void*l277,int l278){uint32 l279
;l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l276),(l122[(l22)]->l102));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l277),(l122[(l22)]->l103),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l92));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l93),(l279));l279 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l88));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l89),(l279));l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l90));soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122[(l22)]->l91),(l279));if(!
SOC_IS_HURRICANE(l22)){l279 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l78));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l79),(l279));}if(((l122[(l22)]->l80)!= NULL)){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l80));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l81),(l279));l279 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l82));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l83),(l279));}else{l279 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l96));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l97),(l279));}l279 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l98));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l99),(l279));l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l100));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l101),(l279));if(((l122[(l22)]->l104)!= NULL)){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l104));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l105),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l106));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l107),(l279));}if(((l122[(l22)]->l76)!= NULL
)){l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l276),(l122[(l22)]->l76));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l277),(l122[(l22)]->l77),(l279));}if(l278){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l86));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l87),(l279));}l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l108));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l109),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l110));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l111),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l112));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l113),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l114));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l115),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l116));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l117),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l118));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l119),(l279));return(SOC_E_NONE);}int
soc_alpm_lpm_ip4entry1_to_0(int l22,void*l276,void*l277,int l278){uint32 l279
;l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l276),(l122[(l22)]->l103));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l277),(l122[(l22)]->l102),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l93));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l92),(l279));l279 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l89));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l88),(l279));l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l91));soc_meminfo_fieldinfo_field32_set
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122[(l22)]->l90),(l279));if(!
SOC_IS_HURRICANE(l22)){l279 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l79));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l78),(l279));}if(((l122[(l22)]->l81)!= NULL)){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l81));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l80),(l279));l279 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l83));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l82),(l279));}else{l279 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l97));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l96),(l279));}l279 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l99));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l98),(l279));l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO
(l22,L3_DEFIPm)),(l276),(l122[(l22)]->l101));
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l277),(l122
[(l22)]->l100),(l279));if(((l122[(l22)]->l105)!= NULL)){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l105));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l104),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l107));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l106),(l279));}if(((l122[(l22)]->l77)!= NULL
)){l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l276),(l122[(l22)]->l77));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(
l22,L3_DEFIPm)),(l277),(l122[(l22)]->l76),(l279));}if(l278){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l87));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm
)),(l277),(l122[(l22)]->l86),(l279));}l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l109));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l108),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l111));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l110),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l113));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l112),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l115));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l114),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l117));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l116),(l279));l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l276),(l122
[(l22)]->l119));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l277),(l122[(l22)]->l118),(l279));return(SOC_E_NONE);}static int
l280(int l22,void*l42){return(SOC_E_NONE);}void l21(int l22){int l222;int l281
;l281 = ((3*(64+32+2+1))-1);if(!soc_cm_debug_check(DK_L3|DK_SOCMEM|DK_VERBOSE
)){return;}for(l222 = l281;l222>= 0;l222--){if((l222!= ((3*(64+32+2+1))-1))&&
((l72[(l22)][(l222)].l65) == -1)){continue;}soc_cm_debug(DK_L3|DK_SOCMEM|
DK_VERBOSE,"PFX = %d P = %d N = %d START = %d END = %d VENT = %d FENT = %d\n"
,l222,(l72[(l22)][(l222)].l67),(l72[(l22)][(l222)].l17),(l72[(l22)][(l222)].
l65),(l72[(l22)][(l222)].l66),(l72[(l22)][(l222)].l68),(l72[(l22)][(l222)].
l69));}COMPILER_REFERENCE(l280);}static int l282(int l22,int l20,uint32*l42){
int l283;int l30;uint32 l284;uint32 l285;int l286;if(!SOC_URPF_STATUS_GET(l22
)){return(SOC_E_NONE);}if(soc_feature(l22,soc_feature_l3_defip_hole)){l283 = 
(soc_mem_index_count(l22,L3_DEFIPm)>>1);}else if(SOC_IS_APOLLO(l22)){l283 = (
soc_mem_index_count(l22,L3_DEFIPm)>>1)+0x0400;}else{l283 = (
soc_mem_index_count(l22,L3_DEFIPm)>>1);}l30 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l92));if(((l122[(l22)]->l76)!= NULL)){
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l76),(0));soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l42),(l122[(l22)]->l77),(0));}l284 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l90));l285 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l42),(l122[(l22)]->l91));if(!l30){if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l102))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l42),(l122[(l22)]->l100),((!l284)?1:0));}if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l103))){soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l42),(l122[(l22)]->l101),((!l285)?1:0));}}else{l286 = ((!l284)&&
(!l285))?1:0;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm))
,(l42),(l122[(l22)]->l100),(l286));soc_meminfo_fieldinfo_field32_set((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[(l22)]->l101),(l286));}return l202(
l22,MEM_BLOCK_ANY,l20+l283,l20,l42);}static int l287(int l22,int l288,int l289
){uint32 l42[SOC_MAX_MEM_FIELD_WORDS];SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l22,
MEM_BLOCK_ANY,l288,l42));l267(l22,l42,l289,0x4000,0);SOC_IF_ERROR_RETURN(l202
(l22,MEM_BLOCK_ANY,l289,l288,l42));SOC_IF_ERROR_RETURN(l282(l22,l289,l42));do
{l167[l22][(l289)<<1] = l167[l22][(l288)<<1];l167[l22][((l289)<<1)+1] = l167[
l22][((l288)<<1)+1];l167[l22][(l288)<<1] = NULL;l167[l22][((l288)<<1)+1] = 
NULL;}while(0);return(SOC_E_NONE);}static int l290(int l22,int l146,int l30){
uint32 l42[SOC_MAX_MEM_FIELD_WORDS];int l288;int l289;uint32 l291,l292;l289 = 
(l72[(l22)][(l146)].l66)+1;if(!l30){l288 = (l72[(l22)][(l146)].l66);
SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(l22,MEM_BLOCK_ANY,l288,l42));l291 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l102));l292 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l42),(l122[(l22)]->l103));if((l291 == 0)||(l292 == 0)){l267(l22,
l42,l289,0x4000,0);SOC_IF_ERROR_RETURN(l202(l22,MEM_BLOCK_ANY,l289,l288,l42))
;SOC_IF_ERROR_RETURN(l282(l22,l289,l42));do{int l293 = (l288)<<1;if((l292)){
l293++;}l167[(l22)][(l289)*2+(l293&1)] = l167[(l22)][l293];l167[(l22)][l293] = 
NULL;}while(0);l289--;}}l288 = (l72[(l22)][(l146)].l65);if(l288!= l289){
SOC_IF_ERROR_RETURN(l287(l22,l288,l289));}(l72[(l22)][(l146)].l65)+= 1;(l72[(
l22)][(l146)].l66)+= 1;return(SOC_E_NONE);}static int l294(int l22,int l146,
int l30){uint32 l42[SOC_MAX_MEM_FIELD_WORDS];int l288;int l289;int l295;
uint32 l291,l292;l289 = (l72[(l22)][(l146)].l65)-1;if((l72[(l22)][(l146)].l68
) == 0){(l72[(l22)][(l146)].l65) = l289;(l72[(l22)][(l146)].l66) = l289-1;
return(SOC_E_NONE);}if((!l30)&&((l72[(l22)][(l146)].l66)!= (l72[(l22)][(l146)
].l65))){l288 = (l72[(l22)][(l146)].l66);SOC_IF_ERROR_RETURN(READ_L3_DEFIPm(
l22,MEM_BLOCK_ANY,l288,l42));l291 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[(l22)]->l102));l292 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l103));if((l291 == 0)||(l292 == 0)){l295 = l288-1;SOC_IF_ERROR_RETURN
(l287(l22,l295,l289));l267(l22,l42,l295,0x4000,0);SOC_IF_ERROR_RETURN(l202(
l22,MEM_BLOCK_ANY,l295,l288,l42));SOC_IF_ERROR_RETURN(l282(l22,l295,l42));do{
int l293 = (l288)<<1;if((l292)){l293++;}l167[(l22)][(l295)*2+(l293&1)] = l167
[(l22)][l293];l167[(l22)][l293] = NULL;}while(0);}else{l267(l22,l42,l289,
0x4000,0);SOC_IF_ERROR_RETURN(l202(l22,MEM_BLOCK_ANY,l289,l288,l42));
SOC_IF_ERROR_RETURN(l282(l22,l289,l42));do{l167[l22][(l289)<<1] = l167[l22][(
l288)<<1];l167[l22][((l289)<<1)+1] = l167[l22][((l288)<<1)+1];l167[l22][(l288
)<<1] = NULL;l167[l22][((l288)<<1)+1] = NULL;}while(0);}}else{l288 = (l72[(
l22)][(l146)].l66);SOC_IF_ERROR_RETURN(l287(l22,l288,l289));}(l72[(l22)][(
l146)].l65)-= 1;(l72[(l22)][(l146)].l66)-= 1;return(SOC_E_NONE);}static int
l296(int l22,int l146,int l30,void*l42,int*l297){int l298;int l299;int l300;
int l301;int l288;uint32 l291,l292;int l179;if((l72[(l22)][(l146)].l68) == 0)
{l301 = ((3*(64+32+2+1))-1);if(l171(l22) == 1){if(l146>(((3*(64+32+2+1))/3)-1
)){l301 = (((3*(64+32+2+1))/3)-1);}}while((l72[(l22)][(l301)].l17)>l146){l301
= (l72[(l22)][(l301)].l17);}l299 = (l72[(l22)][(l301)].l17);if(l299!= -1){(
l72[(l22)][(l299)].l67) = l146;}(l72[(l22)][(l146)].l17) = (l72[(l22)][(l301)
].l17);(l72[(l22)][(l146)].l67) = l301;(l72[(l22)][(l301)].l17) = l146;(l72[(
l22)][(l146)].l69) = ((l72[(l22)][(l301)].l69)+1)/2;(l72[(l22)][(l301)].l69)
-= (l72[(l22)][(l146)].l69);(l72[(l22)][(l146)].l65) = (l72[(l22)][(l301)].
l66)+(l72[(l22)][(l301)].l69)+1;(l72[(l22)][(l146)].l66) = (l72[(l22)][(l146)
].l65)-1;(l72[(l22)][(l146)].l68) = 0;}else if(!l30){l288 = (l72[(l22)][(l146
)].l65);if((l179 = READ_L3_DEFIPm(l22,MEM_BLOCK_ANY,l288,l42))<0){return l179
;}l291 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l42),(l122[(l22)]->l102));l292 = soc_meminfo_fieldinfo_field32_get((&
SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[(l22)]->l103));if((l291 == 0)||(l292
== 0)){*l297 = (l288<<1)+((l292 == 0)?1:0);return(SOC_E_NONE);}l288 = (l72[(
l22)][(l146)].l66);if((l179 = READ_L3_DEFIPm(l22,MEM_BLOCK_ANY,l288,l42))<0){
return l179;}l291 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l42),(l122[(l22)]->l102));l292 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l42),(l122[
(l22)]->l103));if((l291 == 0)||(l292 == 0)){*l297 = (l288<<1)+((l292 == 0)?1:
0);return(SOC_E_NONE);}}l300 = l146;while((l72[(l22)][(l300)].l69) == 0){l300
= (l72[(l22)][(l300)].l17);if(l300 == -1){l300 = l146;break;}}while((l72[(l22
)][(l300)].l69) == 0){l300 = (l72[(l22)][(l300)].l67);if(l300 == -1){if((l72[
(l22)][(l146)].l68) == 0){l298 = (l72[(l22)][(l146)].l67);l299 = (l72[(l22)][
(l146)].l17);if(-1!= l298){(l72[(l22)][(l298)].l17) = l299;}if(-1!= l299){(
l72[(l22)][(l299)].l67) = l298;}}return(SOC_E_FULL);}}while(l300>l146){l299 = 
(l72[(l22)][(l300)].l17);SOC_IF_ERROR_RETURN(l294(l22,l299,l30));(l72[(l22)][
(l300)].l69)-= 1;(l72[(l22)][(l299)].l69)+= 1;l300 = l299;}while(l300<l146){
SOC_IF_ERROR_RETURN(l290(l22,l300,l30));(l72[(l22)][(l300)].l69)-= 1;l298 = (
l72[(l22)][(l300)].l67);(l72[(l22)][(l298)].l69)+= 1;l300 = l298;}(l72[(l22)]
[(l146)].l68)+= 1;(l72[(l22)][(l146)].l69)-= 1;(l72[(l22)][(l146)].l66)+= 1;*
l297 = (l72[(l22)][(l146)].l66)<<((l30)?0:1);sal_memcpy(l42,
soc_mem_entry_null(l22,L3_DEFIPm),soc_mem_entry_words(l22,L3_DEFIPm)*4);
return(SOC_E_NONE);}static int l302(int l22,int l146,int l30,void*l42,int l303
){int l298;int l299;int l288;int l289;uint32 l304[SOC_MAX_MEM_FIELD_WORDS];
void*l305;int l179;int l306;l288 = (l72[(l22)][(l146)].l66);l289 = l303;if(!
l30){l289>>= 1;if((l179 = READ_L3_DEFIPm(l22,MEM_BLOCK_ANY,l288,l304))<0){
return l179;}l305 = (l289 == l288)?l304:l42;if(
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l304),(l122
[(l22)]->l103))){if(l303&1){l179 = soc_alpm_lpm_ip4entry1_to_1(l22,l304,l305,
TRUE);}else{l179 = soc_alpm_lpm_ip4entry1_to_0(l22,l304,l305,TRUE);}l306 = (
l288<<1)+1;soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(
l304),(l122[(l22)]->l103),(0));}else{if(l303&1){l179 = 
soc_alpm_lpm_ip4entry0_to_1(l22,l304,l305,TRUE);}else{l179 = 
soc_alpm_lpm_ip4entry0_to_0(l22,l304,l305,TRUE);}l306 = l288<<1;
soc_meminfo_fieldinfo_field32_set((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l304),(l122
[(l22)]->l102),(0));(l72[(l22)][(l146)].l68)-= 1;(l72[(l22)][(l146)].l69)+= 1
;(l72[(l22)][(l146)].l66)-= 1;}l167[l22][l303] = l167[l22][l306];l167[l22][
l306] = NULL;if(l289!= l288){l267(l22,l305,l289,0x4000,0);if((l179 = l202(l22
,MEM_BLOCK_ANY,l289,l288,l305))<0){return l179;}if((l179 = l282(l22,l289,l305
))<0){return l179;}}l267(l22,l304,l288,0x4000,0);if((l179 = l202(l22,
MEM_BLOCK_ANY,l288,l288,l304))<0){return l179;}if((l179 = l282(l22,l288,l304)
)<0){return l179;}}else{(l72[(l22)][(l146)].l68)-= 1;(l72[(l22)][(l146)].l69)
+= 1;(l72[(l22)][(l146)].l66)-= 1;if(l289!= l288){if((l179 = READ_L3_DEFIPm(
l22,MEM_BLOCK_ANY,l288,l304))<0){return l179;}l267(l22,l304,l289,0x4000,0);if
((l179 = l202(l22,MEM_BLOCK_ANY,l289,l288,l304))<0){return l179;}if((l179 = 
l282(l22,l289,l304))<0){return l179;}}l167[l22][l289<<1] = l167[l22][l288<<1]
;l167[l22][l288<<1] = NULL;sal_memcpy(l304,soc_mem_entry_null(l22,L3_DEFIPm),
soc_mem_entry_words(l22,L3_DEFIPm)*4);l267(l22,l304,l288,0x4000,0);if((l179 = 
l202(l22,MEM_BLOCK_ANY,l288,l288,l304))<0){return l179;}if((l179 = l282(l22,
l288,l304))<0){return l179;}}if((l72[(l22)][(l146)].l68) == 0){l298 = (l72[(
l22)][(l146)].l67);assert(l298!= -1);l299 = (l72[(l22)][(l146)].l17);(l72[(
l22)][(l298)].l17) = l299;(l72[(l22)][(l298)].l69)+= (l72[(l22)][(l146)].l69)
;(l72[(l22)][(l146)].l69) = 0;if(l299!= -1){(l72[(l22)][(l299)].l67) = l298;}
(l72[(l22)][(l146)].l17) = -1;(l72[(l22)][(l146)].l67) = -1;(l72[(l22)][(l146
)].l65) = -1;(l72[(l22)][(l146)].l66) = -1;}return(l179);}int
soc_alpm_lpm_vrf_get(int l48,void*l49,int*l56,int*l307){int l184;if(((l122[(
l48)]->l106)!= NULL)){l184 = soc_L3_DEFIPm_field32_get(l48,l49,VRF_ID_0f);*
l307 = l184;if(soc_L3_DEFIPm_field32_get(l48,l49,VRF_ID_MASK0f)){*l56 = l184;
}else if(!soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l48,L3_DEFIPm)),(
l49),(l122[(l48)]->l108))){*l56 = SOC_L3_VRF_GLOBAL;*l307 = SOC_VRF_MAX(l48)+
1;}else{*l56 = SOC_L3_VRF_OVERRIDE;}}else{*l56 = SOC_L3_VRF_DEFAULT;}return(
SOC_E_NONE);}static int l308(int l22,void*entry,int*l44){int l146;int l179;
int l30;uint32 l279;int l184;int l309;l30 = soc_meminfo_fieldinfo_field32_get
((&SOC_MEM_INFO(l22,L3_DEFIPm)),(entry),(l122[(l22)]->l92));if(l30){l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(entry),(
l122[(l22)]->l90));if((l179 = _ipmask2pfx(l279,&l146))<0){return(l179);}l279 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(entry),(
l122[(l22)]->l91));if(l146){if(l279!= 0xffffffff){return(SOC_E_PARAM);}l146+= 
32;}else{if((l179 = _ipmask2pfx(l279,&l146))<0){return(l179);}}l146+= 33;}
else{l279 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),
(entry),(l122[(l22)]->l90));if((l179 = _ipmask2pfx(l279,&l146))<0){return(
l179);}}SOC_IF_ERROR_RETURN(soc_alpm_lpm_vrf_get(l22,entry,&l184,&l179));l309
= l171(l22);switch(l184){case SOC_L3_VRF_GLOBAL:if(l309 == 1){*l44 = l146+((3
*(64+32+2+1))/3);}else{*l44 = l146;}break;case SOC_L3_VRF_OVERRIDE:*l44 = 
l146+2*((3*(64+32+2+1))/3);break;default:if(l309 == 1){*l44 = l146;}else{*l44
= l146+((3*(64+32+2+1))/3);}break;}return(SOC_E_NONE);}static int l41(int l22
,void*l28,void*l42,int*l43,int*l44,int*l30){int l179;int l185;int l147;int
l146 = 0;l185 = soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,
L3_DEFIPm)),(l28),(l122[(l22)]->l92));if(l185){if(!(l185 = 
soc_meminfo_fieldinfo_field32_get((&SOC_MEM_INFO(l22,L3_DEFIPm)),(l28),(l122[
(l22)]->l93)))){return(SOC_E_PARAM);}}*l30 = l185;l308(l22,l28,&l146);*l44 = 
l146;if(l271(l22,l28,l146,&l147) == SOC_E_NONE){*l43 = l147;if((l179 = 
READ_L3_DEFIPm(l22,MEM_BLOCK_ANY,(*l30)?*l43:(*l43>>1),l42))<0){return l179;}
return(SOC_E_NONE);}else{return(SOC_E_NOT_FOUND);}}static int l23(int l22){
int l281;int l222;int l261;int l310;uint32 l172,l309;if(!soc_feature(l22,
soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}if((l309 = soc_property_get(l22
,spn_L3_ALPM_ENABLE,0))){SOC_IF_ERROR_RETURN(READ_L3_DEFIP_RPF_CONTROLr(l22,&
l172));soc_reg_field_set(l22,L3_DEFIP_RPF_CONTROLr,&l172,LPM_MODEf,1);if(l309
== 1){soc_reg_field_set(l22,L3_DEFIP_RPF_CONTROLr,&l172,LOOKUP_MODEf,1);}else
{soc_reg_field_set(l22,L3_DEFIP_RPF_CONTROLr,&l172,LOOKUP_MODEf,0);}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_RPF_CONTROLr(l22,l172));if(
SOC_URPF_STATUS_GET(l22)){l172 = 0;soc_reg_field_set(l22,L3_DEFIP_KEY_SELr,&
l172,URPF_LOOKUP_CAM4f,0x1);soc_reg_field_set(l22,L3_DEFIP_KEY_SELr,&l172,
URPF_LOOKUP_CAM5f,0x1);soc_reg_field_set(l22,L3_DEFIP_KEY_SELr,&l172,
URPF_LOOKUP_CAM6f,0x1);soc_reg_field_set(l22,L3_DEFIP_KEY_SELr,&l172,
URPF_LOOKUP_CAM7f,0x1);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_KEY_SELr(l22,l172))
;}l172 = 0;if(l309 == 1){if(SOC_URPF_STATUS_GET(l22)){soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM2_SELf,1);soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM3_SELf,1);soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM4_SELf,2);soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM5_SELf,2);soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM6_SELf,3);soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM7_SELf,3);}else{soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM4_SELf,1);soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM5_SELf,1);soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM6_SELf,1);soc_reg_field_set(l22,
L3_DEFIP_ALPM_CFGr,&l172,TCAM7_SELf,1);}}else{if(SOC_URPF_STATUS_GET(l22)){
soc_reg_field_set(l22,L3_DEFIP_ALPM_CFGr,&l172,TCAM4_SELf,2);
soc_reg_field_set(l22,L3_DEFIP_ALPM_CFGr,&l172,TCAM5_SELf,2);
soc_reg_field_set(l22,L3_DEFIP_ALPM_CFGr,&l172,TCAM6_SELf,2);
soc_reg_field_set(l22,L3_DEFIP_ALPM_CFGr,&l172,TCAM7_SELf,2);}}
SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_ALPM_CFGr(l22,l172));}l281 = (3*(64+32+2+1
));soc_mem_lock(l22,L3_DEFIPm);l310 = sizeof(l70)*(l281);if((l72[(l22)]!= 
NULL)){SOC_IF_ERROR_RETURN(soc_alpm_deinit(l22));}l122[l22] = sal_alloc(
sizeof(l120),"lpm_field_state");if(NULL == l122[l22]){return(SOC_E_MEMORY);}(
l122[l22])->l74 = soc_mem_fieldinfo_get(l22,L3_DEFIPm,CLASS_ID0f);(l122[l22])
->l75 = soc_mem_fieldinfo_get(l22,L3_DEFIPm,CLASS_ID1f);(l122[l22])->l76 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,DST_DISCARD0f);(l122[l22])->l77 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,DST_DISCARD1f);(l122[l22])->l78 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ECMP0f);(l122[l22])->l79 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ECMP1f);(l122[l22])->l80 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ECMP_COUNT0f);(l122[l22])->l81 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ECMP_COUNT1f);(l122[l22])->l82 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ECMP_PTR0f);(l122[l22])->l83 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ECMP_PTR1f);(l122[l22])->l84 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,GLOBAL_ROUTE0f);(l122[l22])->l85 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,GLOBAL_ROUTE1f);(l122[l22])->l86 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,HIT0f);(l122[l22])->l87 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,HIT1f);(l122[l22])->l88 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,IP_ADDR0f);(l122[l22])->l89 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,IP_ADDR1f);(l122[l22])->l90 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,IP_ADDR_MASK0f);(l122[l22])->l91 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,IP_ADDR_MASK1f);(l122[l22])->l92 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,MODE0f);(l122[l22])->l93 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,MODE1f);(l122[l22])->l94 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,MODE_MASK0f);(l122[l22])->l95 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,MODE_MASK1f);(l122[l22])->l96 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,NEXT_HOP_INDEX0f);(l122[l22])->l97 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,NEXT_HOP_INDEX1f);(l122[l22])->l98 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,PRI0f);(l122[l22])->l99 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,PRI1f);(l122[l22])->l100 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,RPE0f);(l122[l22])->l101 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,RPE1f);(l122[l22])->l102 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,VALID0f);(l122[l22])->l103 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,VALID1f);(l122[l22])->l104 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,VRF_ID_0f);(l122[l22])->l105 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,VRF_ID_1f);(l122[l22])->l106 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,VRF_ID_MASK0f);(l122[l22])->l107 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,VRF_ID_MASK1f);(l122[l22])->l108 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,GLOBAL_HIGH0f);(l122[l22])->l109 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,GLOBAL_HIGH1f);(l122[l22])->l110 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ALG_HIT_IDX0f);(l122[l22])->l111 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ALG_HIT_IDX1f);(l122[l22])->l112 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ALG_BKT_PTR0f);(l122[l22])->l113 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,ALG_BKT_PTR1f);(l122[l22])->l114 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,DEFAULT_MISS0f);(l122[l22])->l115 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,DEFAULT_MISS1f);(l122[l22])->l116 = 
soc_mem_fieldinfo_get(l22,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX0f);(l122[l22])
->l117 = soc_mem_fieldinfo_get(l22,L3_DEFIPm,FLEX_CTR_BASE_COUNTER_IDX1f);(
l122[l22])->l118 = soc_mem_fieldinfo_get(l22,L3_DEFIPm,FLEX_CTR_POOL_NUMBER0f
);(l122[l22])->l119 = soc_mem_fieldinfo_get(l22,L3_DEFIPm,
FLEX_CTR_POOL_NUMBER1f);(l72[(l22)]) = sal_alloc(l310,"LPM prefix info");if(
NULL == (l72[(l22)])){sal_free(l122[l22]);l122[l22] = NULL;return(
SOC_E_MEMORY);}soc_mem_lock(l22,L3_DEFIPm);sal_memset((l72[(l22)]),0,l310);
for(l222 = 0;l222<l281;l222++){(l72[(l22)][(l222)].l65) = -1;(l72[(l22)][(
l222)].l66) = -1;(l72[(l22)][(l222)].l67) = -1;(l72[(l22)][(l222)].l17) = -1;
(l72[(l22)][(l222)].l68) = 0;(l72[(l22)][(l222)].l69) = 0;}l261 = 
soc_mem_index_count(l22,L3_DEFIPm);if(SOC_URPF_STATUS_GET(l22)){l261>>= 1;}if
(l309 == 1){(l72[(l22)][((((3*(64+32+2+1))/3)-1))].l66) = (l261>>1)-1;(l72[(
l22)][(((((3*(64+32+2+1))/3)-1)))].l69) = l261>>1;(l72[(l22)][((((3*(64+32+2+
1))-1)))].l69) = (l261-(l72[(l22)][(((((3*(64+32+2+1))/3)-1)))].l69));}else{(
l72[(l22)][((((3*(64+32+2+1))-1)))].l69) = l261;}if((l133[(l22)])!= NULL){if(
l141((l133[(l22)]))<0){soc_mem_unlock(l22,L3_DEFIPm);return SOC_E_INTERNAL;}(
l133[(l22)]) = NULL;}if(l139(l22,l261*2,l261,&(l133[(l22)]))<0){
soc_mem_unlock(l22,L3_DEFIPm);return SOC_E_MEMORY;}soc_mem_unlock(l22,
L3_DEFIPm);return(SOC_E_NONE);}static int l24(int l22){if(!soc_feature(l22,
soc_feature_lpm_tcam)){return(SOC_E_UNAVAIL);}soc_mem_lock(l22,L3_DEFIPm);if(
(l133[(l22)])!= NULL){l141((l133[(l22)]));(l133[(l22)]) = NULL;}if((l72[(l22)
]!= NULL)){sal_free(l122[l22]);l122[l22] = NULL;sal_free((l72[(l22)]));(l72[(
l22)]) = NULL;}soc_mem_unlock(l22,L3_DEFIPm);return(SOC_E_NONE);}static int
l25(int l22,void*l26,int*l311){int l146;int l20;int l30;uint32 l42[
SOC_MAX_MEM_FIELD_WORDS];int l179 = SOC_E_NONE;int l312 = 0;sal_memcpy(l42,
soc_mem_entry_null(l22,L3_DEFIPm),soc_mem_entry_words(l22,L3_DEFIPm)*4);
soc_mem_lock(l22,L3_DEFIPm);l179 = l41(l22,l26,l42,&l20,&l146,&l30);if(l179 == 
SOC_E_NOT_FOUND){l179 = l296(l22,l146,l30,l42,&l20);if(l179<0){soc_mem_unlock
(l22,L3_DEFIPm);return(l179);}}else{l312 = 1;}*l311 = l20;if(l179 == 
SOC_E_NONE){if(!l30){if(l20&1){l179 = soc_alpm_lpm_ip4entry0_to_1(l22,l26,l42
,TRUE);}else{l179 = soc_alpm_lpm_ip4entry0_to_0(l22,l26,l42,TRUE);}if(l179<0)
{soc_mem_unlock(l22,L3_DEFIPm);return(l179);}l26 = (void*)l42;l20>>= 1;}l21(
l22);soc_cm_debug(DK_L3|DK_SOCMEM,"\nsoc_alpm_lpm_insert: %d %d\n",l20,l146);
if(!l312){l267(l22,l26,l20,0x4000,0);}l179 = l202(l22,MEM_BLOCK_ANY,l20,l20,
l26);if(l179>= 0){l179 = l282(l22,l20,l26);}}soc_mem_unlock(l22,L3_DEFIPm);
return(l179);}static int l27(int l22,void*l28){int l146;int l20;int l30;
uint32 l42[SOC_MAX_MEM_FIELD_WORDS];int l179 = SOC_E_NONE;soc_mem_lock(l22,
L3_DEFIPm);l179 = l41(l22,l28,l42,&l20,&l146,&l30);if(l179 == SOC_E_NONE){
soc_cm_debug(DK_L3|DK_SOCMEM,"\nsoc_alpm_lpm_delete: %d %d\n",l20,l146);l270(
l22,l28,l20);l179 = l302(l22,l146,l30,l42,l20);}l21(l22);soc_mem_unlock(l22,
L3_DEFIPm);return(l179);}static int l45(int l22,void*l28,void*l42,int*l43){
int l146;int l179;int l30;soc_mem_lock(l22,L3_DEFIPm);l179 = l41(l22,l28,l42,
l43,&l146,&l30);soc_mem_unlock(l22,L3_DEFIPm);return(l179);}static int l29(
int l48,void*l28,int l30,int l31,int l32,defip_aux_scratch_entry_t*l33){
uint32 l279;uint32 l313[4] = {0,0};int l146 = 0;int l179 = SOC_E_NONE;l279 = 
soc_mem_field32_get(l48,L3_DEFIPm,l28,VALID0f);soc_mem_field32_set(l48,
L3_DEFIP_AUX_SCRATCHm,l33,VALIDf,l279);l279 = soc_mem_field32_get(l48,
L3_DEFIPm,l28,MODE0f);soc_mem_field32_set(l48,L3_DEFIP_AUX_SCRATCHm,l33,MODEf
,l279);l279 = soc_mem_field32_get(l48,L3_DEFIPm,l28,ENTRY_TYPE0f);
soc_mem_field32_set(l48,L3_DEFIP_AUX_SCRATCHm,l33,ENTRY_TYPEf,0);l279 = 
soc_mem_field32_get(l48,L3_DEFIPm,l28,GLOBAL_ROUTE0f);soc_mem_field32_set(l48
,L3_DEFIP_AUX_SCRATCHm,l33,GLOBAL_ROUTEf,l279);l279 = soc_mem_field32_get(l48
,L3_DEFIPm,l28,ECMP0f);soc_mem_field32_set(l48,L3_DEFIP_AUX_SCRATCHm,l33,
ECMPf,l279);l279 = soc_mem_field32_get(l48,L3_DEFIPm,l28,ECMP_PTR0f);
soc_mem_field32_set(l48,L3_DEFIP_AUX_SCRATCHm,l33,ECMP_PTRf,l279);l279 = 
soc_mem_field32_get(l48,L3_DEFIPm,l28,NEXT_HOP_INDEX0f);soc_mem_field32_set(
l48,L3_DEFIP_AUX_SCRATCHm,l33,NEXT_HOP_INDEXf,l279);l279 = 
soc_mem_field32_get(l48,L3_DEFIPm,l28,PRI0f);soc_mem_field32_set(l48,
L3_DEFIP_AUX_SCRATCHm,l33,PRIf,l279);l279 = soc_mem_field32_get(l48,L3_DEFIPm
,l28,RPE0f);soc_mem_field32_set(l48,L3_DEFIP_AUX_SCRATCHm,l33,RPEf,l279);l279
=soc_mem_field32_get(l48,L3_DEFIPm,l28,VRF_ID_0f);soc_mem_field32_set(l48,
L3_DEFIP_AUX_SCRATCHm,l33,VRFf,l279);soc_mem_field32_set(l48,
L3_DEFIP_AUX_SCRATCHm,l33,DB_TYPEf,l31);l279 = soc_mem_field32_get(l48,
L3_DEFIPm,l28,DST_DISCARD0f);soc_mem_field32_set(l48,L3_DEFIP_AUX_SCRATCHm,
l33,DST_DISCARDf,l279);l279 = soc_mem_field32_get(l48,L3_DEFIPm,l28,
CLASS_ID0f);soc_mem_field32_set(l48,L3_DEFIP_AUX_SCRATCHm,l33,CLASS_IDf,l279)
;l313[0] = soc_mem_field32_get(l48,L3_DEFIPm,l28,IP_ADDR0f);if(l30){l313[1] = 
soc_mem_field32_get(l48,L3_DEFIPm,l28,IP_ADDR1f);}soc_mem_field_set(l48,
L3_DEFIP_AUX_SCRATCHm,(uint32*)l33,IP_ADDRf,(uint32*)l313);if(l30){l279 = 
soc_mem_field32_get(l48,L3_DEFIPm,l28,IP_ADDR_MASK0f);if((l179 = _ipmask2pfx(
l279,&l146))<0){return(l179);}l279 = soc_mem_field32_get(l48,L3_DEFIPm,l28,
IP_ADDR_MASK1f);if(l146){if(l279!= 0xffffffff){return(SOC_E_PARAM);}l146+= 32
;}else{if((l179 = _ipmask2pfx(l279,&l146))<0){return(l179);}}}else{l279 = 
soc_mem_field32_get(l48,L3_DEFIPm,l28,IP_ADDR_MASK0f);if((l179 = _ipmask2pfx(
l279,&l146))<0){return(l179);}}soc_mem_field32_set(l48,L3_DEFIP_AUX_SCRATCHm,
l33,IP_LENGTHf,l146);soc_mem_field32_set(l48,L3_DEFIP_AUX_SCRATCHm,l33,
REPLACE_LENf,l32);return(SOC_E_NONE);}static int l34(int l22,l6 l35,
defip_aux_scratch_entry_t*l33,int*l36,int*l37,int*l38){uint32 l172,l314;int
l315;soc_timeout_t l316;int l179 = SOC_E_NONE;SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_SCRATCHm(l22,MEM_BLOCK_ANY,0,l33));SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l22,&l172));if(soc_reg_field_get(l22,
L3_DEFIP_AUX_CTRLr,l172,STARTf)){return SOC_E_BUSY;}l172 = 0;soc_timeout_init
(&l316,50000,0);switch(l35){case l2:l315 = 0;break;case l3:l315 = 1;break;
case l4:l315 = 2;break;case l5:l315 = 3;break;default:return SOC_E_PARAM;}
soc_reg_field_set(l22,L3_DEFIP_AUX_CTRLr,&l172,OPCODEf,l315);
soc_reg_field_set(l22,L3_DEFIP_AUX_CTRLr,&l172,STARTf,1);SOC_IF_ERROR_RETURN(
WRITE_L3_DEFIP_AUX_CTRLr(l22,l172));l315 = 0;do{SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRLr(l22,&l172));l315 = soc_reg_field_get(l22,
L3_DEFIP_AUX_CTRLr,l172,DONEf);if(soc_timeout_check(&l316)){if(l315 == 1){
l179 = SOC_E_NONE;}else{soc_cm_debug(DK_WARN,
"unit %d : DEFIP AUX Operation timeout\n",l22);l179 = SOC_E_TIMEOUT;}break;}}
while(l315!= 1);if(SOC_SUCCESS(l179)){if(l35 == l4){if(l36&&l37){*l36 = 
soc_reg_field_get(l22,L3_DEFIP_AUX_CTRLr,l172,HITf);*l37 = soc_reg_field_get(
l22,L3_DEFIP_AUX_CTRLr,l172,BKT_INDEXf);SOC_IF_ERROR_RETURN(
READ_L3_DEFIP_AUX_CTRL_1r(l22,&l314));*l38 = soc_reg_field_get(l22,
L3_DEFIP_AUX_CTRL_1r,l314,BKT_PTRf);}}}soc_reg_field_set(l22,
L3_DEFIP_AUX_CTRLr,&l172,STARTf,0);soc_reg_field_set(l22,L3_DEFIP_AUX_CTRLr,&
l172,DONEf,0);SOC_IF_ERROR_RETURN(WRITE_L3_DEFIP_AUX_CTRLr(l22,l172));return
l179;}static int l47(int l48,void*l49,void*l50,void*l51,soc_mem_t l52,uint32
l53,uint32*l317){uint32 l279;uint32 l313[4] = {0,0};int l146 = 0;int l179 = 
SOC_E_NONE;int l30;uint32 l54 = 0;l30 = soc_mem_field32_get(l48,L3_DEFIPm,l49
,MODE0f);sal_memset(l50,0,soc_mem_entry_bytes(l48,l52));l279 = 
soc_mem_field32_get(l48,L3_DEFIPm,l49,HIT0f);soc_mem_field32_set(l48,l52,l50,
HITf,l279);l279 = soc_mem_field32_get(l48,L3_DEFIPm,l49,VALID0f);
soc_mem_field32_set(l48,l52,l50,VALIDf,l279);l279 = soc_mem_field32_get(l48,
L3_DEFIPm,l49,ECMP0f);soc_mem_field32_set(l48,l52,l50,ECMPf,l279);l279 = 
soc_mem_field32_get(l48,L3_DEFIPm,l49,ECMP_PTR0f);soc_mem_field32_set(l48,l52
,l50,ECMP_PTRf,l279);l279 = soc_mem_field32_get(l48,L3_DEFIPm,l49,
NEXT_HOP_INDEX0f);soc_mem_field32_set(l48,l52,l50,NEXT_HOP_INDEXf,l279);l279 = 
soc_mem_field32_get(l48,L3_DEFIPm,l49,PRI0f);soc_mem_field32_set(l48,l52,l50,
PRIf,l279);l279 = soc_mem_field32_get(l48,L3_DEFIPm,l49,RPE0f);
soc_mem_field32_set(l48,l52,l50,RPEf,l279);l279 = soc_mem_field32_get(l48,
L3_DEFIPm,l49,DST_DISCARD0f);soc_mem_field32_set(l48,l52,l50,DST_DISCARDf,
l279);l279 = soc_mem_field32_get(l48,L3_DEFIPm,l49,SRC_DISCARD0f);
soc_mem_field32_set(l48,l52,l50,SRC_DISCARDf,l279);l279 = soc_mem_field32_get
(l48,L3_DEFIPm,l49,CLASS_ID0f);soc_mem_field32_set(l48,l52,l50,CLASS_IDf,l279
);l313[0] = soc_mem_field32_get(l48,L3_DEFIPm,l49,IP_ADDR0f);if(l30){l313[1] = 
soc_mem_field32_get(l48,L3_DEFIPm,l49,IP_ADDR1f);}soc_mem_field_set(l48,l52,(
uint32*)l50,KEYf,(uint32*)l313);if(l30){l279 = soc_mem_field32_get(l48,
L3_DEFIPm,l49,IP_ADDR_MASK0f);if((l179 = _ipmask2pfx(l279,&l146))<0){return(
l179);}l279 = soc_mem_field32_get(l48,L3_DEFIPm,l49,IP_ADDR_MASK1f);if(l146){
if(l279!= 0xffffffff){return(SOC_E_PARAM);}l146+= 32;}else{if((l179 = 
_ipmask2pfx(l279,&l146))<0){return(l179);}}}else{l279 = soc_mem_field32_get(
l48,L3_DEFIPm,l49,IP_ADDR_MASK0f);if((l179 = _ipmask2pfx(l279,&l146))<0){
return(l179);}}if((l146 == 0)&&(l313[0] == 0)&&(l313[1] == 0)){l54 = 1;}if(
l317!= NULL){*l317 = l54;}soc_mem_field32_set(l48,l52,l50,LENGTHf,l146);if(
l51 == NULL){return(SOC_E_NONE);}if(SOC_URPF_STATUS_GET(l48)){sal_memcpy(l51,
l50,soc_mem_entry_bytes(l48,l52));soc_mem_field32_set(l48,l52,l51,
DST_DISCARDf,0);soc_mem_field32_set(l48,l52,l51,RPEf,0);soc_mem_field32_set(
l48,l52,l51,SRC_DISCARDf,l53&SOC_ALPM_RPF_SRC_DISCARD);soc_mem_field32_set(
l48,l52,l51,DEFAULTROUTEf,l54);}return(SOC_E_NONE);}static int l55(int l48,
void*l50,soc_mem_t l52,int l30,int l56,int l57,int l20,void*l49){uint32 l279;
uint32 l313[4] = {0,0};uint32 l146 = 0;sal_memset(l49,0,soc_mem_entry_bytes(
l48,L3_DEFIPm));l279 = soc_mem_field32_get(l48,l52,l50,HITf);
soc_mem_field32_set(l48,L3_DEFIPm,l49,HIT0f,l279);if(l30){soc_mem_field32_set
(l48,L3_DEFIPm,l49,HIT1f,l279);}l279 = soc_mem_field32_get(l48,l52,l50,VALIDf
);soc_mem_field32_set(l48,L3_DEFIPm,l49,VALID0f,l279);if(l30){
soc_mem_field32_set(l48,L3_DEFIPm,l49,VALID1f,l279);}l279 = 
soc_mem_field32_get(l48,l52,l50,ECMPf);soc_mem_field32_set(l48,L3_DEFIPm,l49,
ECMP0f,l279);l279 = soc_mem_field32_get(l48,l52,l50,ECMP_PTRf);
soc_mem_field32_set(l48,L3_DEFIPm,l49,ECMP_PTR0f,l279);l279 = 
soc_mem_field32_get(l48,l52,l50,NEXT_HOP_INDEXf);soc_mem_field32_set(l48,
L3_DEFIPm,l49,NEXT_HOP_INDEX0f,l279);l279 = soc_mem_field32_get(l48,l52,l50,
PRIf);soc_mem_field32_set(l48,L3_DEFIPm,l49,PRI0f,l279);l279 = 
soc_mem_field32_get(l48,l52,l50,RPEf);soc_mem_field32_set(l48,L3_DEFIPm,l49,
RPE0f,l279);l279 = soc_mem_field32_get(l48,l52,l50,DST_DISCARDf);
soc_mem_field32_set(l48,L3_DEFIPm,l49,DST_DISCARD0f,l279);l279 = 
soc_mem_field32_get(l48,l52,l50,SRC_DISCARDf);soc_mem_field32_set(l48,
L3_DEFIPm,l49,SRC_DISCARD0f,l279);l279 = soc_mem_field32_get(l48,l52,l50,
CLASS_IDf);soc_mem_field32_set(l48,L3_DEFIPm,l49,CLASS_ID0f,l279);
soc_mem_field32_set(l48,L3_DEFIPm,l49,ALG_BKT_PTR0f,l57);soc_mem_field32_set(
l48,L3_DEFIPm,l49,ALG_HIT_IDX0f,l20);soc_mem_field32_set(l48,L3_DEFIPm,l49,
MODE_MASK0f,3);soc_mem_field32_set(l48,L3_DEFIPm,l49,ENTRY_TYPE_MASK0f,1);if(
l30){soc_mem_field32_set(l48,L3_DEFIPm,l49,MODE0f,1);}soc_mem_field_get(l48,
l52,l50,KEYf,l313);if(l30){soc_mem_field32_set(l48,L3_DEFIPm,l49,IP_ADDR1f,
l313[1]);}soc_mem_field32_set(l48,L3_DEFIPm,l49,IP_ADDR0f,l313[0]);l279 = 
soc_mem_field32_get(l48,l52,l50,LENGTHf);if(l30){if(l279>= 32){l146 = 
0xffffffff;soc_mem_field32_set(l48,L3_DEFIPm,l49,IP_ADDR_MASK0f,l146);l146 = 
0xffffffff<<(l279-32);soc_mem_field32_set(l48,L3_DEFIPm,l49,IP_ADDR_MASK1f,
l146);}else{l146 = ~(0xffffffff>>l279);soc_mem_field32_set(l48,L3_DEFIPm,l49,
IP_ADDR_MASK0f,l146);}}else{assert(l279<= 32);l146 = ~(0xffffffff>>l279);
soc_mem_field32_set(l48,L3_DEFIPm,l49,IP_ADDR_MASK0f,l146);}if(l56 == 
SOC_L3_VRF_OVERRIDE){soc_mem_field32_set(l48,L3_DEFIPm,l49,GLOBAL_HIGH0f,1);
soc_mem_field32_set(l48,L3_DEFIPm,l49,GLOBAL_ROUTE0f,1);soc_mem_field32_set(
l48,L3_DEFIPm,l49,VRF_ID_0f,0);soc_mem_field32_set(l48,L3_DEFIPm,l49,
VRF_ID_MASK0f,0);}else if(l56 == SOC_L3_VRF_GLOBAL){soc_mem_field32_set(l48,
L3_DEFIPm,l49,GLOBAL_ROUTE0f,1);soc_mem_field32_set(l48,L3_DEFIPm,l49,
VRF_ID_0f,0);soc_mem_field32_set(l48,L3_DEFIPm,l49,VRF_ID_MASK0f,0);}else{
soc_mem_field32_set(l48,L3_DEFIPm,l49,VRF_ID_0f,l56);soc_mem_field32_set(l48,
L3_DEFIPm,l49,VRF_ID_MASK0f,SOC_VRF_MAX(l48));}return(SOC_E_NONE);}
#endif /* ALPM_ENABLE */
