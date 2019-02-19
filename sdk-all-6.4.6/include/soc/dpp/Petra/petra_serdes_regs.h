/* $Id: soc_petra_serdes_regs.h,v 1.6 Broadcom SDK $
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
*/
/*
   Obfuscated by COBF (Version 1.06 2006-01-07 by BB) at Thu May 05 17:59:04 2011
*/
#define l9 SOC_PETRA_SRD_CHANNEL_CMU
#define l104 SOC_PETRA_SRD_CHANNEL_IPU
#define l10 SOC_PETRA_SRD_CHANNEL_MAX
#define l408 SOC_PETRA_SRD_CHANNEL_INVALID
#define l285 SOC_PETRA_SRD_REG_ADDR_MAX
#define l572 SOC_PETRA_SRD_FLD_SIZE_MAX
#define le typedef
#define lb struct
#define ld SOC_PETRA_SRD_REGS_ADDR
#define lc addr
#define la SOC_PETRA_SRD_REGS_FIELD
#define l406 pdr2clk
#define l123 rxinvo
#define l117 rxinvf
#define l392 rxdro
#define l410 rxdrf
#define l513 rxswrst
#define l473 rxpwrdn
#define l61 rxlsctrl0
#define l438 ldopt
#define l459 bamode
#define l231 rxporsto
#define l35 rxporstf
#define l157 rxloso
#define l155 rxlosf
#define l546 refckpt
#define l290 rxckdgo
#define l62 rxlsctrl1
#define l588 rclkrls
#define l462 rxppm
#define l330 trimmode
#define l224 rxppmpd
#define l435 rxllko
#define l434 rxllkf
#define l65 rxlsppm
#define l107 rxtxtag
#define l373 rxllk
#define l534 rxlos
#define l306 rxlsstatus
#define l259 rxlosmode
#define l89 rxlosdly
#define l98 atbsel
#define l388 atbctrl
#define lv SOC_PETRA_SRD_REGS_RX
#define l331 vtrmsel
#define l346 rxthreshold
#define l230 rxhsctrl0
#define l84 rxdt
#define l76 rxdb
#define l437 rxdmen
#define l366 rxdmuxsel
#define l262 rxhsctrl1
#define l374 itrimslv
#define l568 itrimmtr
#define l45 capip
#define l376 rxmux
#define l263 rxhsctrl2
#define l162 capdac
#define l80 tseldacmtr
#define l574 tdacmtr
#define l506 vcdldac0
#define l298 stepby8
#define l504 tseldacslv
#define l382 tdacslv
#define l507 vcdldac1
#define l381 regswb
#define l578 ivcdl
#define l238 islvgain
#define l508 vcdldac2
#define l292 stepby2
#define l122 relock
#define l294 stepby4
#define l234 vcdlloop
#define l439 vcdlrst
#define l41 vcdlctrl0
#define l37 igforce
#define l536 pgforce
#define l109 pgstart
#define l51 pglimit
#define l38 vcdlctrl1
#define l102 igstart
#define l88 iglimit
#define l24 vcdlctrl2
#define l280 rxlpbken
#define l200 psdel
#define l21 vcdlctrl3
#define l505 roampd
#define l479 isdel
#define l18 vcdlctrl4
#define l516 isstep
#define l211 psstep
#define l17 vcdlctrl5
#define l269 iregmsb
#define l15 vcdlctrl6
#define l265 dm2ld
#define l396 iiup
#define l569 icdrup
#define l540 disckpat
#define l557 disdfepat
#define l363 qlock
#define l14 vcdlctrl7
#define l22 rxidleo
#define l566 rxtdacselfb
#define l295 dec4
#define l23 vcdlloso
#define l354 encdet
#define l39 rxidlef
#define l458 oneshot
#define l317 baswrst
#define l528 bactrl
#define l168 modectrl
#define l565 comdetn
#define l87 rxtdacdone
#define l324 wordsync
#define l515 comdet
#define l67 rxhsstatus
#define l401 ipgn3
#define l400 ipgn2
#define l399 ipgn1
#define l398 ipgn0
#define l318 ipgainctrl
#define l131 ckbfbs1p25x
#define l365 ckbfbs0p75x
#define l71 pdovrpd
#define l166 pdbfbisttrm1
#define l165 pdbfbisttrm0
#define l144 pdbiastrm1
#define l315 pdbiastrm0
#define l264 rxhsctrl3
#define l8 SOC_PETRA_SRD_REGS_RX_HS
#define l77 pdxck2
#define l196 txinvo
#define l194 txinvf
#define l28 txdro
#define l26 txdrf
#define l383 txswrst
#define l361 txpwrdn
#define l517 txlsctrl0
#define l74 lscktest
#define l220 txasync
#define l150 txobpdo
#define l149 txobpdf
#define l267 fifosel
#define l143 twcsel
#define l371 xckipt
#define l187 txckdgo
#define l518 txlsctrl1
#define l378 rateasync
#define l277 ckdlysel
#define l533 nsbilb
#define l119 bstest
#define l171 npilb
#define l167 ipilb
#define l563 nsilb
#define l490 loopback
#define l92 lfsrerr
#define l83 rxdr
#define l139 txdr
#define l232 fifouf
#define l556 fifoof
#define l495 txlsstatus
#define ll SOC_PETRA_SRD_REGS_TX
#define l203 ampfdr
#define l296 ampfdren
#define l177 txfdrctrl0
#define l176 amphdr
#define l185 amphdren
#define l134 txhdrctrl0
#define l304 ampqdr
#define l460 ampqdren
#define l114 txqdrctrl0
#define l184 slewfdr
#define l75 txfdrctrl1
#define l152 slewhdr
#define l297 txhdrctrl1
#define l270 slewqdr
#define l105 txqdrctrl1
#define lr iprefdr
#define l413 iprefdren
#define l33 ipstfdr
#define l153 ipstfdren
#define l179 txfdrctrl2
#define l1 iprehdr
#define l333 iprehdren
#define l25 ipsthdr
#define l85 ipsthdren
#define l299 txhdrctrl2
#define lq ipreqdr
#define l47 ipreqdren
#define l129 ipstqdr
#define l343 ipstqdren
#define l56 txqdrctrl2
#define l474 imainfdr
#define l493 imainfdren
#define l180 txfdrctrl3
#define l433 imainhdr
#define l411 imainhdren
#define l300 txhdrctrl3
#define l577 imainqdr
#define l78 imainqdren
#define l241 txqdrctrl3
#define lp tapset
#define l140 firlut0
#define l141 firlut1
#define l466 mode4q
#define l221 mode2q
#define l178 vcomhigh
#define l302 txselfen
#define l226 txckmuxsel
#define l127 txtermhi
#define l389 txhsctrl
#define l350 txhsbnctrl0
#define l351 txhsbnctrl1
#define l223 txdccen
#define l217 mode2h
#define l436 firmode
#define l130 txhsctrl1
#define l2 SOC_PETRA_SRD_REGS_TX_HS
#define l543 tsrst
#define l245 trm
#define l52 sseyemode
#define l571 sseye
#define l426 xlcnt
#define l303 xhcnt
#define l338 eyeadc
#define l422 hlcnt
#define l198 ctrl1
#define l476 emstart
#define l457 dcross
#define l560 eqlth
#define l207 eyemon
#define l69 rselintp
#define l209 rphintp
#define l326 eqintctrl
#define l580 csrst
#define l544 dmuxslct
#define l586 dswap
#define l480 trimdone
#define l199 ctrl2
#define l440 phdel_test
#define l512 rvctrl
#define ln cntload
#define l407 margin
#define l573 cmargin
#define l86 dmuxctrl
#define l283 partrim
#define l68 trimdacmtr
#define l530 pdspare1
#define l529 pdspare0
#define l138 ldoutctrl1
#define l44 rdso
#define l133 csr
#define l154 cetl
#define l214 lcross
#define l202 ctrl3
#define l108 pdndet
#define l95 dfelthfdr
#define l103 dfelth
#define l470 pdly
#define l358 tlthfdr
#define l514 tlth
#define l249 shdr
#define l225 sqdr
#define l432 stdet
#define l261 mrstdet
#define l323 rlclkph
#define l322 capsrint
#define l425 sro
#define l579 vctrl
#define l7 srf
#define l591 ftim1
#define l590 ftim0
#define l50 flavg
#define l208 fextrim
#define l387 fpatrim
#define l552 flxtrim
#define l274 frxaeq
#define l532 fcharz
#define l412 sdebug
#define l509 capint
#define l402 vstat0
#define l239 capsr
#define l403 vstat1
#define l32 chp
#define l287 hrdchid
#define l372 etl
#define l164 txporstn
#define l535 cmudone
#define l352 dmux
#define l469 tl
#define l276 emdone
#define l328 dstat
#define l12 ldout
#define l581 ldoutub
#define l501 ldoutlb
#define lh ecnt
#define l511 ecnthi
#define l539 ecntlo
#define l334 dm1
#define l332 dm0
#define l118 dmux0
#define l335 dm3
#define l337 dm2
#define l115 dmux1
#define l353 sthdr
#define l496 stqdr
#define l524 stmacropdn
#define l393 strxlos
#define l584 strxporst
#define l101 stmacrorst
#define l195 strimrst
#define l215 sbits0
#define l307 sfr
#define l329 shr
#define l415 sqr
#define l291 ser
#define l216 sbits1
#define l6 dtalth
#define l158 zcntfdr
#define l499 zfr
#define l125 zcnthdr
#define l521 zhr
#define l451 zcntqdr
#define l106 zqr
#define l170 g1cntfdr
#define l526 z1cntfdr
#define l542 gfr
#define l142 g1cnthdr
#define l481 z1cnthdr
#define l559 ghr
#define l243 g1cntqdr
#define l55 z1cntqdr
#define l112 gqr
#define l64 dfelthhdr
#define l173 dfelthhr
#define l183 dfelthqdr
#define l385 dfelthqr
#define l313 tlthhdr
#define l59 tlthhr
#define l454 tlthqdr
#define l137 tlthqr
#define l310 mcstat1
#define l311 mcstat2
#define l151 eqhltack
#define l570 eqhlt
#define l537 blind
#define l186 preset
#define l567 steadystate
#define l344 rate
#define l90 startupeq1
#define l319 interconnect
#define l191 nyqattn
#define l91 startupeq2
#define l494 lxtstat
#define l255 tdacstat
#define l60 dfestat
#define l356 eqstat
#define l336 tstat
#define l386 patstat
#define l442 spd_en
#define l242 rsbitsen
#define l486 eqstatus
#define l11 SOC_PETRA_SRD_REGS_SM_CONTROL
#define l583 drxbpd
#define l219 txratersten
#define l193 drxblosc
#define l375 dbnmode
#define l420 rxbbctrl0
#define l27 enrstd
#define l282 bsrstd
#define l169 enrxbcn
#define l475 bsrxbcn
#define l279 enrxmacpd
#define l549 bsrxmacpd
#define l116 rstdp2en
#define l421 rxbbctrl1
#define l564 drxbp2
#define l73 drxren
#define l527 drxpllpdb
#define l510 drxdmuxpdb
#define l233 rxbcn
#define l301 rxbbstatus
#define l347 rxrateclken
#define l417 rxratersten
#define l247 drxreno
#define l362 drxrenf
#define l181 entxmacpd
#define l431 bstxmacpd
#define l488 synctxidle
#define l145 txbbctrl
#define l576 entxbeaconen
#define l391 bstxbeaconen
#define l443 entxidle
#define l582 bstxidle
#define l427 entxrdeten
#define l397 bstxrdeten
#define l278 enrxdetected
#define l120 bsrxdetected
#define l252 txbbctrl2
#define l491 dtxrdeten
#define l416 dtxbeaconen
#define l538 dtxidleen
#define l500 rxdetected
#define l424 dtxckbufpdb
#define l48 dtxserpdb
#define l82 dtxobpdb
#define l100 dtxpllpdb
#define l485 txbbstatus
#define ly SOC_PETRA_SRD_REGS_BEACON
#define l461 cfdone
#define l113 vcoslow
#define l235 cmullk
#define l355 cmustatus
#define l20 cmult
#define l228 cmulocktime
#define l548 dccpd
#define l253 pllpd
#define l156 pllrst
#define l423 dpllbp
#define l553 porstmode
#define l309 cfmode
#define l54 kickmode
#define l467 debugmode
#define l237 cmuctrl0
#define l96 hsbufrpd
#define l340 hsbuflpd
#define l380 vco2x
#define l547 vregadj
#define l419 cmuctrl1
#define l429 divo
#define l359 refdivf
#define l286 vcodivf
#define l379 cmuctrl2
#define l189 lfctrl
#define l70 cpadj
#define l320 cmuctrl3
#define l418 cftrimbyp
#define l522 cftrim
#define l42 clk8051pd
#define l312 refckopd
#define l519 cmuatbus
#define l246 cmuctrl4
#define l414 trimview
#define l3 cfctrl
#define l367 xckipd
#define l110 scifselcsen
#define l555 pextselcsen
#define l36 trimo
#define l40 trimf
#define l314 rcsel
#define l94 rotm
#define l428 testctrl
#define l357 cmusporo
#define lw cmuppm
#define l124 cmuppmmode
#define l484 cmuppmpd
#define l503 cmullko
#define l482 cmullkf
#define l364 cmuckdly
#define l465 cmutag
#define l72 cmuversion
#define l384 trimselfo
#define l404 trimselff
#define l34 slaveo
#define l93 slavef
#define l293 mtrckopd
#define l229 crb
#define l240 cmuctrl5
#define lo SOC_PETRA_SRD_REGS_CMU_CONTROL
#define l126 parityerr_status
#define l222 int8051_intrpt
#define l29 e8051req_intrpt
#define l79 scifintreq_intrpt
#define l452 eram_rb_ena
#define l531 eram_we
#define l273 romrb_ena
#define l483 romd_ena
#define l146 ctr0
#define l97 hxack
#define l349 gp
#define l390 sciftest
#define l562 clkdivider
#define l132 clk8051off
#define l587 clksel
#define l147 ctr1
#define lu addrlo
#define lz addrhi
#define l13 romdata
#define lm ramdata
#define l163 fxack
#define lj gpio
#define l175 timeout
#define l525 timeren
#define l360 macrosel
#define l288 stat0
#define l218 ackint8051
#define l289 stat1
#define l63 ckexed
#define l472 ckpassbr
#define l463 ckpassap
#define l316 cksumerr
#define l66 lxti
#define l554 arbchecken
#define l30 cefg
#define l368 alltrim
#define l441 mpreg0
#define l161 auxpram
#define l49 cksumbr
#define l550 cksumap
#define l405 fdebug
#define l321 fskpatrim
#define l160 fsklxtrim
#define l558 fskrxeq
#define l497 fskdmeminit
#define l444 mpreg1
#define l370 fstp1
#define l369 fstp0
#define l453 cksumblk0
#define l445 mpreg2
#define l266 ftrimate
#define l489 ftdacate
#define l455 cksumblk1
#define l446 mpreg3
#define l545 fdebug_cksum_bo
#define l447 mpreg4
#define l345 fskiraminit
#define l31 fwdterr
#define l250 pmemerr
#define l339 dmemerr
#define l148 iramerr
#define l523 pmemdis
#define l589 dmemdis
#define l174 iramdis
#define l448 mpreg5
#define l395 prgtrac1
#define l394 prgtrac0
#define l192 parityindiso
#define l190 parityindisf
#define l377 parityeno
#define l172 parityenf
#define l498 parityerrforceo
#define l487 parityerrforcef
#define l449 mpreg6
#define l206 enint8051
#define l492 forceint8051
#define l57 scifintreqmask
#define l456 scifintreq8051
#define l19 ene8051req
#define l308 forcee8051req
#define l275 enscifppexden
#define l341 forcescif
#define l477 ifarb0
#define l348 rst8051o
#define l342 rst8051f
#define l478 ifarb1
#define lt SOC_PETRA_SRD_REGS_IPU_CTRL
#define l271 pcerrpt
#define l227 pgudpext
#define l135 pgpnsel
#define l409 pgudpsel
#define l268 pgerr
#define l520 pgsel
#define l236 genctrl
#define lf pgudp
#define l244 genudp0
#define l257 genudp1
#define l251 genudp2
#define l254 genudp3
#define l256 genudp4
#define l551 togsel
#define l260 pcpnsel
#define l204 pcscale
#define l128 pccapture
#define l464 run4ever
#define l182 pcsel
#define l43 chkctrl
#define l541 pcword
#define l327 chkword
#define lg pcerr
#define l210 chkerror0
#define l212 chkerror1
#define l197 t4err
#define l159 t2err
#define l430 t1err
#define l502 pcworduf
#define l258 pcerrof
#define l305 pclos
#define l53 pcinsync
#define l585 pcdone
#define l272 chkstatus
#define l213 chkerror2
#define l5 SOC_PETRA_SRD_REGS_PRBS
#define l592 scifintreq
#define l468 e8051ctrlo
#define l99 e8051ctrlf
#define l561 scifctrlo
#define l575 scifctrlf
#define l281 arbctrl
#define l4 SOC_PETRA_SRD_REGS_IF_ARBITRATION
#define lx control
#define li SOC_PETRA_SRD_REGS_IPU
#define l248 if_arbitration
#define lk SOC_PETRA_SRD_REGS_CMU
#define l450 rx
#define l471 tx
#define l121 prbs
#define l284 beacon
#define l188 rx_hs
#define l325 tx_hs
#define l46 sm_control
#define ls SOC_PETRA_SRD_REGS_LANE
#define l111 lane
#define l16 ipu
#define l81 cmu
#define l0 SOC_PETRA_SRD_REGS
#define l205 uint32
#define l58 soc_petra_srd_regs_init
#define l136 soc_petra_srd_regs

#if ! defined __COBF__
#ifndef __SOC_PETRA_SERDES_REGS_INCLUDED__
#define __SOC_PETRA_SERDES_REGS_INCLUDED__
#endif
#include <soc/dpp/SAND/Utils/sand_header.h>


#include <soc/dpp/Petra/petra_api_serdes.h>
#include <soc/dpp/Petra/petra_api_serdes_utils.h>
#include <soc/dpp/Petra/petra_chip_regs.h>

#include <soc/dpp/Petra/petra_api_framework.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>

#define SOC_PETRA_SRD_CHANNEL_CMU 7
#define SOC_PETRA_SRD_CHANNEL_IPU 6
#define SOC_PETRA_SRD_CHANNEL_MAX SOC_PETRA_SRD_CHANNEL_CMU
#define SOC_PETRA_SRD_CHANNEL_INVALID ( SOC_PETRA_SRD_CHANNEL_MAX + 1)
#define SOC_PETRA_SRD_REG_ADDR_MAX 63
#define SOC_PETRA_SRD_FLD_SIZE_MAX 8
le lb{lb{ld lc;la l406;la l123;la l117;la l392;la l410;la l513;la l473
;}l61;lb{ld lc;la l438;la l459;la l231;la l35;la l157;la l155;la l546
;la l290;}l62;lb{ld lc;la l588;la l462;la l330;la l224;la l435;la l434
;}l65;lb{ld lc;la l107;la l373;la l534;}l306;lb{ld lc;la l259;la l89;
la l98;}l388;}lv;le lb{lb{ld lc;la l331;la l346;}l230;lb{ld lc;la l84
;la l76;la l437;la l366;}l262;lb{ld lc;la l374;la l568;la l45;la l376
;}l263;lb{ld lc;la l162;la l80;la l574;}l506;lb{ld lc;la l298;la l504
;la l382;}l507;lb{ld lc;la l381;la l578;la l238;}l508;lb{ld lc;la l292
;la l122;la l294;la l234;la l439;}l41;lb{ld lc;la l37;la l536;la l109
;la l51;}l38;lb{ld lc;la l102;la l88;}l24;lb{ld lc;la l280;la l200;}
l21;lb{ld lc;la l505;la l479;}l18;lb{ld lc;la l516;la l211;}l17;lb{ld
lc;la l269;}l15;lb{ld lc;la l265;la l396;la l569;la l540;la l557;la
l363;}l14;lb{ld lc;la l22;la l566;la l295;la l23;la l354;la l39;la
l458;la l317;}l528;lb{ld lc;la l168;la l565;la l87;la l324;la l515;}
l67;lb{ld lc;la l401;la l400;la l399;la l398;}l318;lb{ld lc;la l131;
la l365;la l71;la l166;la l165;la l144;la l315;}l264;}l8;le lb{lb{ld
lc;la l77;la l196;la l194;la l28;la l26;la l383;la l361;}l517;lb{ld lc
;la l74;la l220;la l150;la l149;la l267;la l143;la l371;la l187;}l518
;lb{ld lc;la l378;la l277;la l533;la l119;la l171;la l167;la l563;}
l490;lb{ld lc;la l92;la l83;la l139;la l232;la l556;}l495;}ll;le lb{
lb{ld lc;la l203;la l296;}l177;lb{ld lc;la l176;la l185;}l134;lb{ld lc
;la l304;la l460;}l114;lb{ld lc;la l184;}l75;lb{ld lc;la l152;}l297;
lb{ld lc;la l270;}l105;lb{ld lc;la lr;la l413;la l33;la l153;}l179;lb
{ld lc;la l1;la l333;la l25;la l85;}l299;lb{ld lc;la lq;la l47;la l129
;la l343;}l56;lb{ld lc;la l474;la l493;la lr;}l180;lb{ld lc;la l433;
la l411;la l1;}l300;lb{ld lc;la l577;la l78;la lq;}l241;lb{ld lc;la lp
;}l140;lb{ld lc;la lp;}l141;lb{ld lc;la l466;la l221;la l178;la l302;
la l226;la l127;}l389;lb{ld lc;}l350;lb{ld lc;}l351;lb{ld lc;la l223;
la l217;la l436;}l130;}l2;le lb{lb{ld lc;la l543;la l245;la l52;la
l571;la l426;la l303;la l338;la l422;}l198;lb{ld lc;la l476;la l457;
la l560;}l207;lb{ld lc;la l69;la l209;}l326;lb{ld lc;la l580;la l544;
la l586;la l480;la l201;}l199;lb{ld lc;la l440;}l512;lb{ld lc;la ln;}
ln;lb{ld lc;la l407;la l573;la l86;la l283;la l68;la l530;la l529;}
l138;lb{ld lc;la l44;la l133;la l154;la l214;}l202;lb{ld lc;la l108;
la l95;}l103;lb{ld lc;la l470;la l358;}l514;lb{ld lc;la l249;la l225;
la l432;la l261;la l323;la l322;la l425;}l579;lb{ld lc;la l7;}l7;lb{
ld lc;la l591;la l590;la l50;la l208;la l387;la l552;la l274;la l532;
}l412;lb{ld lc;la l509;}l402;lb{ld lc;la l239;}l403;lb{ld lc;la l32;
la l287;la l372;la l164;}l535;lb{ld lc;la l352;la l469;la l276;}l328;
lb{ld lc;la l12;}l581;lb{ld lc;la l12;}l501;lb{ld lc;la lh;}l511;lb{
ld lc;la lh;}l539;lb{ld lc;la l334;la l332;}l118;lb{ld lc;la l335;la
l337;}l115;lb{ld lc;la l353;la l496;la l524;la l393;la l584;la l101;
la l195;}l215;lb{ld lc;la l307;la l329;la l415;la l291;}l216;lb{ld lc
;la l6;}l6;lb{ld lc;la l158;}l499;lb{ld lc;la l125;}l521;lb{ld lc;la
l451;}l106;lb{ld lc;la l170;la l526;}l542;lb{ld lc;la l142;la l481;}
l559;lb{ld lc;la l243;la l55;}l112;lb{ld lc;la l64;}l173;lb{ld lc;la
l183;}l385;lb{ld lc;la l313;}l59;lb{ld lc;la l454;}l137;lb{ld lc;}
l310;lb{ld lc;}l311;lb{ld lc;la l151;la l570;la l537;la l186;la l567;
la l344;}l90;lb{ld lc;la l319;la l191;}l91;lb{ld lc;la l494;la l255;
la l60;la l356;la l336;la l386;la l442;la l242;}l486;}l11;le lb{lb{ld
lc;la l583;la l219;la l193;la l375;}l420;lb{ld lc;la l27;la l282;la
l169;la l475;la l279;la l549;la l116;}l421;lb{ld lc;la l564;la l73;la
l527;la l510;la l233;}l301;lb{ld lc;la l347;la l417;la l247;la l362;
la l181;la l431;la l488;}l145;lb{ld lc;la l576;la l391;la l443;la l582
;la l427;la l397;la l278;la l120;}l252;lb{ld lc;la l491;la l416;la
l538;la l500;la l424;la l48;la l82;la l100;}l485;}ly;le lb{lb{ld lc;
la l461;la l113;la l235;}l355;lb{ld lc;la l20;}l228;lb{ld lc;la l548;
la l253;la l156;la l423;la l553;la l309;la l54;la l467;}l237;lb{ld lc
;la l96;la l340;la l380;la l547;}l419;lb{ld lc;la l429;la l359;la l286
;}l379;lb{ld lc;la l189;la l70;}l320;lb{ld lc;la l418;la l522;la l42;
la l312;la l519;}l246;lb{ld lc;la l414;la l3;}l3;lb{ld lc;la l367;la
l110;la l555;la l36;la l40;la l314;la l94;}l428;lb{ld lc;la l357;la lw
;la l124;la l484;la l503;la l482;}lw;lb{ld lc;}l364;lb{ld lc;la l465;
}l72;lb{ld lc;la l384;la l404;la l34;la l93;la l293;la l229;}l240;}lo
;le lb{lb{ld lc;la l126;la l222;la l29;la l79;la l452;la l531;la l273
;la l483;}l146;lb{ld lc;la l97;la l349;la l390;la l562;la l132;la l587
;}l147;lb{ld lc;la lu;}lu;lb{ld lc;la lz;}lz;lb{ld lc;la l13;}l13;lb{
ld lc;la lm;}lm;lb{ld lc;la l163;la lj;la l175;la l525;la l360;}l288;
lb{ld lc;la lj;la l218;}l289;lb{ld lc;la l63;la l472;la l463;la l316;
la l66;la l554;la l30;la l368;}l441;lb{ld lc;la l161;la l49;la l550;
la l405;la l321;la l160;la l558;la l497;}l444;lb{ld lc;la l370;la l369
;la l453;}l445;lb{ld lc;la l266;la l489;la l455;}l446;lb{ld lc;la l545
;}l447;lb{ld lc;la l345;la l31;la l250;la l339;la l148;la l523;la l589
;la l174;}l448;lb{ld lc;la l395;la l394;la l192;la l190;la l377;la
l172;la l498;la l487;}l449;lb{ld lc;la l206;la l492;la l57;la l456;la
l19;la l308;la l275;la l341;}l477;lb{ld lc;la l348;la l342;}l478;}lt;
le lb{lb{ld lc;la l271;la l227;la l135;la l409;la l268;la l520;}l236;
lb{ld lc;la lf;}l244;lb{ld lc;la lf;}l257;lb{ld lc;la lf;}l251;lb{ld
lc;la lf;}l254;lb{ld lc;la lf;}l256;lb{ld lc;la l551;la l260;la l204;
la l128;la l464;la l182;}l43;lb{ld lc;la l541;}l327;lb{ld lc;la lg;}
l210;lb{ld lc;la lg;}l212;lb{ld lc;la l197;la l159;la l430;la l502;la
l258;la l305;la l53;la l585;}l272;lb{ld lc;la lg;}l213;}l5;le lb{lb{
ld lc;la l592;la l468;la l99;la l561;la l575;}l281;}l4;le lb{lt lx;}
li;le lb{lo lx;l4 l248;}lk;le lb{lv l450;ll l471;l5 l121;ly l284;l8
l188;l2 l325;l11 l46;}ls;le lb{ls l111;li l16;lk l81;}l0;l205 l58(void);
l0*l136(void);
/*
   Obfuscated by COBF (Version 1.06 2006-01-07 by BB) at Thu May 05 17:59:04 2011
*/
#undef l9
#undef l104
#undef l10
#undef l408
#undef l285
#undef l572
#undef le
#undef lb
#undef ld
#undef lc
#undef la
#undef l406
#undef l123
#undef l117
#undef l392
#undef l410
#undef l513
#undef l473
#undef l61
#undef l438
#undef l459
#undef l231
#undef l35
#undef l157
#undef l155
#undef l546
#undef l290
#undef l62
#undef l588
#undef l462
#undef l330
#undef l224
#undef l435
#undef l434
#undef l65
#undef l107
#undef l373
#undef l534
#undef l306
#undef l259
#undef l89
#undef l98
#undef l388
#undef lv
#undef l331
#undef l346
#undef l230
#undef l84
#undef l76
#undef l437
#undef l366
#undef l262
#undef l374
#undef l568
#undef l45
#undef l376
#undef l263
#undef l162
#undef l80
#undef l574
#undef l506
#undef l298
#undef l504
#undef l382
#undef l507
#undef l381
#undef l578
#undef l238
#undef l508
#undef l292
#undef l122
#undef l294
#undef l234
#undef l439
#undef l41
#undef l37
#undef l536
#undef l109
#undef l51
#undef l38
#undef l102
#undef l88
#undef l24
#undef l280
#undef l200
#undef l21
#undef l505
#undef l479
#undef l18
#undef l516
#undef l211
#undef l17
#undef l269
#undef l15
#undef l265
#undef l396
#undef l569
#undef l540
#undef l557
#undef l363
#undef l14
#undef l22
#undef l566
#undef l295
#undef l23
#undef l354
#undef l39
#undef l458
#undef l317
#undef l528
#undef l168
#undef l565
#undef l87
#undef l324
#undef l515
#undef l67
#undef l401
#undef l400
#undef l399
#undef l398
#undef l318
#undef l131
#undef l365
#undef l71
#undef l166
#undef l165
#undef l144
#undef l315
#undef l264
#undef l8
#undef l77
#undef l196
#undef l194
#undef l28
#undef l26
#undef l383
#undef l361
#undef l517
#undef l74
#undef l220
#undef l150
#undef l149
#undef l267
#undef l143
#undef l371
#undef l187
#undef l518
#undef l378
#undef l277
#undef l533
#undef l119
#undef l171
#undef l167
#undef l563
#undef l490
#undef l92
#undef l83
#undef l139
#undef l232
#undef l556
#undef l495
#undef ll
#undef l203
#undef l296
#undef l177
#undef l176
#undef l185
#undef l134
#undef l304
#undef l460
#undef l114
#undef l184
#undef l75
#undef l152
#undef l297
#undef l270
#undef l105
#undef lr
#undef l413
#undef l33
#undef l153
#undef l179
#undef l1
#undef l333
#undef l25
#undef l85
#undef l299
#undef lq
#undef l47
#undef l129
#undef l343
#undef l56
#undef l474
#undef l493
#undef l180
#undef l433
#undef l411
#undef l300
#undef l577
#undef l78
#undef l241
#undef lp
#undef l140
#undef l141
#undef l466
#undef l221
#undef l178
#undef l302
#undef l226
#undef l127
#undef l389
#undef l350
#undef l351
#undef l223
#undef l217
#undef l436
#undef l130
#undef l2
#undef l543
#undef l245
#undef l52
#undef l571
#undef l426
#undef l303
#undef l338
#undef l422
#undef l198
#undef l476
#undef l457
#undef l560
#undef l207
#undef l69
#undef l209
#undef l326
#undef l580
#undef l544
#undef l586
#undef l480
#undef l199
#undef l440
#undef l512
#undef ln
#undef l407
#undef l573
#undef l86
#undef l283
#undef l68
#undef l530
#undef l529
#undef l138
#undef l44
#undef l133
#undef l154
#undef l214
#undef l202
#undef l108
#undef l95
#undef l103
#undef l470
#undef l358
#undef l514
#undef l249
#undef l225
#undef l432
#undef l261
#undef l323
#undef l322
#undef l425
#undef l579
#undef l7
#undef l591
#undef l590
#undef l50
#undef l208
#undef l387
#undef l552
#undef l274
#undef l532
#undef l412
#undef l509
#undef l402
#undef l239
#undef l403
#undef l32
#undef l287
#undef l372
#undef l164
#undef l535
#undef l352
#undef l469
#undef l276
#undef l328
#undef l12
#undef l581
#undef l501
#undef lh
#undef l511
#undef l539
#undef l334
#undef l332
#undef l118
#undef l335
#undef l337
#undef l115
#undef l353
#undef l496
#undef l524
#undef l393
#undef l584
#undef l101
#undef l195
#undef l215
#undef l307
#undef l329
#undef l415
#undef l291
#undef l216
#undef l6
#undef l158
#undef l499
#undef l125
#undef l521
#undef l451
#undef l106
#undef l170
#undef l526
#undef l542
#undef l142
#undef l481
#undef l559
#undef l243
#undef l55
#undef l112
#undef l64
#undef l173
#undef l183
#undef l385
#undef l313
#undef l59
#undef l454
#undef l137
#undef l310
#undef l311
#undef l151
#undef l570
#undef l537
#undef l186
#undef l567
#undef l344
#undef l90
#undef l319
#undef l191
#undef l91
#undef l494
#undef l255
#undef l60
#undef l356
#undef l336
#undef l386
#undef l442
#undef l242
#undef l486
#undef l11
#undef l583
#undef l219
#undef l193
#undef l375
#undef l420
#undef l27
#undef l282
#undef l169
#undef l475
#undef l279
#undef l549
#undef l116
#undef l421
#undef l564
#undef l73
#undef l527
#undef l510
#undef l233
#undef l301
#undef l347
#undef l417
#undef l247
#undef l362
#undef l181
#undef l431
#undef l488
#undef l145
#undef l576
#undef l391
#undef l443
#undef l582
#undef l427
#undef l397
#undef l278
#undef l120
#undef l252
#undef l491
#undef l416
#undef l538
#undef l500
#undef l424
#undef l48
#undef l82
#undef l100
#undef l485
#undef ly
#undef l461
#undef l113
#undef l235
#undef l355
#undef l20
#undef l228
#undef l548
#undef l253
#undef l156
#undef l423
#undef l553
#undef l309
#undef l54
#undef l467
#undef l237
#undef l96
#undef l340
#undef l380
#undef l547
#undef l419
#undef l429
#undef l359
#undef l286
#undef l379
#undef l189
#undef l70
#undef l320
#undef l418
#undef l522
#undef l42
#undef l312
#undef l519
#undef l246
#undef l414
#undef l3
#undef l367
#undef l110
#undef l555
#undef l36
#undef l40
#undef l314
#undef l94
#undef l428
#undef l357
#undef lw
#undef l124
#undef l484
#undef l503
#undef l482
#undef l364
#undef l465
#undef l72
#undef l384
#undef l404
#undef l34
#undef l93
#undef l293
#undef l229
#undef l240
#undef lo
#undef l126
#undef l222
#undef l29
#undef l79
#undef l452
#undef l531
#undef l273
#undef l483
#undef l146
#undef l97
#undef l349
#undef l390
#undef l562
#undef l132
#undef l587
#undef l147
#undef lu
#undef lz
#undef l13
#undef lm
#undef l163
#undef lj
#undef l175
#undef l525
#undef l360
#undef l288
#undef l218
#undef l289
#undef l63
#undef l472
#undef l463
#undef l316
#undef l66
#undef l554
#undef l30
#undef l368
#undef l441
#undef l161
#undef l49
#undef l550
#undef l405
#undef l321
#undef l160
#undef l558
#undef l497
#undef l444
#undef l370
#undef l369
#undef l453
#undef l445
#undef l266
#undef l489
#undef l455
#undef l446
#undef l545
#undef l447
#undef l345
#undef l31
#undef l250
#undef l339
#undef l148
#undef l523
#undef l589
#undef l174
#undef l448
#undef l395
#undef l394
#undef l192
#undef l190
#undef l377
#undef l172
#undef l498
#undef l487
#undef l449
#undef l206
#undef l492
#undef l57
#undef l456
#undef l19
#undef l308
#undef l275
#undef l341
#undef l477
#undef l348
#undef l342
#undef l478
#undef lt
#undef l271
#undef l227
#undef l135
#undef l409
#undef l268
#undef l520
#undef l236
#undef lf
#undef l244
#undef l257
#undef l251
#undef l254
#undef l256
#undef l551
#undef l260
#undef l204
#undef l128
#undef l464
#undef l182
#undef l43
#undef l541
#undef l327
#undef lg
#undef l210
#undef l212
#undef l197
#undef l159
#undef l430
#undef l502
#undef l258
#undef l305
#undef l53
#undef l585
#undef l272
#undef l213
#undef l5
#undef l592
#undef l468
#undef l99
#undef l561
#undef l575
#undef l281
#undef l4
#undef lx
#undef li
#undef l248
#undef lk
#undef l450
#undef l471
#undef l121
#undef l284
#undef l188
#undef l325
#undef l46
#undef ls
#undef l111
#undef l16
#undef l81
#undef l0
#undef l205
#undef l58
#undef l136

#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif
