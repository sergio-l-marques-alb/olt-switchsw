/* $Id: petra_serdes_low_level_access.c,v 1.7 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
*/
#include<soc/dpp/SAND/Utils/sand_header.h>
#include<soc/dpp/Petra/petra_serdes_low_level_access.h>
#include<soc/dpp/Petra/petra_reg_access.h>
#include<soc/dpp/Petra/petra_serdes_regs.h>
#include<soc/dpp/Petra/petra_serdes.h>
#include<soc/dpp/Petra/petra_general.h>
#include<soc/dpp/Petra/petra_sw_db.h>
#include<soc/dpp/SAND/Utils/sand_bitstream.h>
#include<soc/dpp/SAND/Utils/sand_os_interface.h>
#include<soc/dpp/SAND/Management/sand_low_level.h>
#define l37 typedef
#define l89 enum
#define l7 SOC_PETRA_SRD_EPB_OP_READ
#define l26 SOC_PETRA_SRD_EPB_OP_WRITE
#define l48 SOC_PETRA_SRD_EPB_OP
#define l69 struct
#define la uint32
#define l13 access_op
#define lu reg_addr
#define ln channel
#define ls element
#define l12 qrtt_id
#define lj uint8
#define ll data
#define l6 error
#define l20 valid
#define l0 SOC_PETRA_SRD_EPB_CMD_AND_STAT
#define l17 static
#define l44 SOC_SAND_MAX_DEVICE
#define l34 void
#define l86 soc_petra_srd_brdcst_all_lanes_clear
#define l74 for
#define l15 FALSE
#define l67 soc_petra_srd_brdcst_all_lanes_set
#define lc SOC_SAND_IN
#define le unit
#define l39 enable
#define l70 soc_petra_srd_brdcst_all_lanes_get
#define l27 return
#define l43 soc_petra_srd_epb_status_parse
#define l10 SOC_SAND_OUT
#define l9 SOC_PETRA_REGS
#define lb regs
#define l11 soc_petra_regs
#define lx SOC_PETRA_FLD_FROM_PLACE
#define lg SOC_PETRA_REG_DB_ACC
#define ld serdes
#define lq srd_epb_rd_reg
#define l76 wr_op
#define l49 addr_channel
#define l53 addr_element
#define l46 macro_sel
#define l55 addr_reg
#define l41 SOC_SAND_NUM2BOOL
#define l54 SOC_SAND_OK
#define l24 soc_petra_srd_epb_cmd_build
#define lv SOC_PETRA_FLD_IN_PLACE
#define lm srd_epb_op_reg
#define l61 wr_en
#define lo if
#define l59 SOC_PETRA_SRD_CHANNEL_IPU
#define l72 addr_ipu_cs
#define l87 soc_petra_sw_db_srd_is_irq_disable_get
#define l16 TRUE
#define l85 int_req
#define l83 addr_global
#define l25 soc_petra_srd_epb_access_validate_done
#define li res
#define l3 expected_value
#define l21 SOC_SAND_INIT_ERROR_DEFINITIONS
#define l62 SOC_PETRA_SRD_EPB_ACCESS_VALIDATE_DONE
#define l58 do
#define l78 SOC_PETRA_REG_IGET
#define lk exit
#define l1 SOC_SAND_CHECK_FUNC_RESULT
#define l47 sal_msleep
#define l35 else
#define l57 SOC_SAND_SET_ERROR_CODE
#define l82 SOC_PETRA_SRD_EPB_TIMEOUT_ERR
#define l42 while
#define l77 SOC_PETRA_SRD_LLA_EPB_CMD_READ_ERR
#define l18 SOC_SAND_EXIT_AND_SEND_ERROR
#define l68 soc_petra_srd_lla_epb_write
#define l38 SOC_PETRA_SRD_STAR_ID
#define l40 SOC_PETRA_DEFAULT_INSTANCE
#define l88 SOC_PETRA_SRD_LLA_EPB_WRITE
#define l52 SOC_PETRA_SRD_STAR2INSTANCE
#define l45 SOC_PETRA_REG_ISET
#define l79 soc_petra_srd_lla_epb_read
#define l81 SOC_PETRA_SRD_LLA_EPB_READ
#define l73 soc_petra_srd_status_fld_poll_unsafe
#define l65 SOC_PETRA_SRD_ENTITY_TYPE
#define l63 SOC_PETRA_SRD_REGS_FIELD
#define l75 SOC_PETRA_POLL_INFO
#define l84 SOC_PETRA_SRD_STATUS_FLD_POLL_UNSAFE
#define l30 SOC_SAND_CHECK_NULL_INPUT
#define l80 soc_petra_srd_fld_read
#define l56 busy_wait_nof_iters
#define l66 timer_nof_iters
#define l64 timer_delay_msec
#define l71 SOC_SAND_NUM2BOOL_INVERSE
l37 l89{l7=0,l26=1}l48;l37 l69{la l13;la lu;la ln;la ls;la l12;lj ll;
lj l6;lj l20;}l0;l17 lj l14[l44];l34 l86(l34){la l8;l74(l8=0;l8<l44;
l8++){l14[l8]=l15;}}l34 l67(lc la le,lc lj l39){l14[le]=l39;}lj l70(
lc la le){l27 l14[le];}l17 la l43(lc la le,lc la lf,l10 l0*lt){l9*lb;
lb=l11();lt->l13=lx(lf,lg(lb->ld.lq.l76));lt->ln=lx(lf,lg(lb->ld.lq.
l49));lt->ls=lx(lf,lg(lb->ld.lq.l53));lt->l12=lx(lf,lg(lb->ld.lq.l46));
lt->lu=lx(lf,lg(lb->ld.lq.l55));lt->ll=(lj)lx(lf,lg(lb->ld.lq.ll));lt
->l6=l41(lx(lf,lg(lb->ld.lq.l6)));lt->l20=l41(lx(lf,lg(lb->ld.lq.l20)));
l27 l54;}l17 la l24(lc la le,lc l0*lr,l10 la*lf){la lz;l9*lb;lb=l11();
lz=0;lz|=lv(lr->l13,lg(lb->ld.lm.l61))|lv(lr->ln,lg(lb->ld.lm.l49))|
lv(lr->ls,lg(lb->ld.lm.l53))|lv(lr->l12,lg(lb->ld.lm.l46))|lv(lr->lu,
lg(lb->ld.lm.l55))|lv(lr->ll,lg(lb->ld.lm.ll));lo(lr->ln==l59){lz|=lv
(0x1,lg(lb->ld.lm.l72));}lo(!((lr->ln==l59)||(l87(le)==l16))){lz|=lv(
0x1,lg(lb->ld.lm.l85));}lo(l14[le]==l16){lz|=lv(0x1,lg(lb->ld.lm.l83));
} *lf=lz;l27 l54;}l17 la l25(lc la le,lc la lp,lc l48 l29,l10 lj*l36){
la l28,li;l9*lb;la ly=0;l0 l4;lj l23,l3;l21(l62);lb=l11();l3=(l29==l7
)?l16:l15;l58{l78(lb->ld.lq,l28,lp,10,lk);li=l43(le,l28,&l4);l1(li,20
,lk);l23=(l29==l7)?l4.l20:l4.l6;lo(l23!=l3){lo(ly>10){lo(ly<(10+1)){
l47(50);}l35{l57(l82,30,lk);}}}ly++;}l42(l23!=l3);lo(l4.l6){l57(l77,
40,lk);} *l36=l4.ll;lk:l18("\x65\x72\x72\x6f\x72\x20\x69\x6e\x20\x73"
"\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x65\x70\x62\x5f"
"\x61\x63\x63\x65\x73\x73\x5f\x76\x61\x6c\x69\x64\x61\x74\x65\x5f\x64"
"\x6f\x6e\x65\x28\x29",0,0);}la l68(lc la le,lc l38 l5,lc la l2,lc la
ls,lc la ln,lc la lu,lc lj ll){la li;lj l50;l0 lh;la lf;l9*lb;la lp=
l40;l21(l88);lb=l11();lh.ls=ls;lh.ln=ln;lh.lu=lu;lh.l13=l26;lh.l12=l2
;lh.ll=ll;lp=l52(l5);li=l24(le,&lh,&lf);l1(li,10,lk);l45(lb->ld.lm,lf
,lp,20,lk);li=l25(le,lp,l26,&l50);l1(li,50,lk);lk:l18("\x65\x72\x72"
"\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73"
"\x72\x64\x5f\x6c\x6c\x61\x5f\x65\x70\x62\x5f\x77\x72\x69\x74\x65\x28"
"\x29",l5,l2);}la l79(lc la le,lc l38 l5,lc la l2,lc la ls,lc la ln,
lc la lu,l10 lj*ll){la li;l0 lh;la lf;l9*lb;la lp=l40;l21(l81);lb=l11
();lh.ls=ls;lh.ln=ln;lh.lu=lu;lh.l13=l7;lh.l12=l2;lh.ll=0x0;lp=l52(l5
);li=l24(le,&lh,&lf);l1(li,10,lk);l45(lb->ld.lm,lf,lp,20,lk);li=l25(
le,lp,l7,ll);l1(li,40,lk);lk:l18("\x65\x72\x72\x6f\x72\x20\x69\x6e"
"\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61\x5f\x73\x72\x64\x5f\x6c\x6c"
"\x61\x5f\x65\x70\x62\x5f\x72\x65\x61\x64\x28\x29",l5,l2);}la l73(lc
la le,lc l65 l51,lc la l60,lc l63*l33,lc l75*lw,l10 lj*l32){la li;lj
l19;la ly=0;lj l22=l15,l31;l21(l84);l30(l33);l30(lw);l30(l32);l31=l15
;lo(!l31){l58{li=l80(le,l51,l60,l33,&l19);l1(li,10,lk);lo(l19!=lw->l3
){lo(ly>lw->l56){lo(ly<(lw->l56+lw->l66)){l47(lw->l64);}l35{l22=l16;}
}}ly++;}l42((l19!=lw->l3)&&(l22==l15));} *l32=l71(l22);lk:l18("\x65"
"\x72\x72\x6f\x72\x20\x69\x6e\x20\x73\x6f\x63\x5f\x70\x65\x74\x72\x61"
"\x5f\x73\x72\x64\x5f\x73\x74\x61\x74\x75\x73\x5f\x66\x6c\x64\x5f\x70"
"\x6f\x6c\x6c\x5f\x75\x6e\x73\x61\x66\x65\x28\x29",0,0);}
#undef l37
#undef l89
#undef l7
#undef l26
#undef l48
#undef l69
#undef la
#undef l13
#undef lu
#undef ln
#undef ls
#undef l12
#undef lj
#undef ll
#undef l6
#undef l20
#undef l0
#undef l17
#undef l44
#undef l34
#undef l86
#undef l74
#undef l15
#undef l67
#undef lc
#undef le
#undef l39
#undef l70
#undef l27
#undef l43
#undef l10
#undef l9
#undef lb
#undef l11
#undef lx
#undef lg
#undef ld
#undef lq
#undef l76
#undef l49
#undef l53
#undef l46
#undef l55
#undef l41
#undef l54
#undef l24
#undef lv
#undef lm
#undef l61
#undef lo
#undef l59
#undef l72
#undef l87
#undef l16
#undef l85
#undef l83
#undef l25
#undef li
#undef l3
#undef l21
#undef l62
#undef l58
#undef l78
#undef lk
#undef l1
#undef l47
#undef l35
#undef l57
#undef l82
#undef l42
#undef l77
#undef l18
#undef l68
#undef l38
#undef l40
#undef l88
#undef l52
#undef l45
#undef l79
#undef l81
#undef l73
#undef l65
#undef l63
#undef l75
#undef l84
#undef l30
#undef l80
#undef l56
#undef l66
#undef l64
#undef l71
#include<soc/dpp/SAND/Utils/sand_footer.h>
