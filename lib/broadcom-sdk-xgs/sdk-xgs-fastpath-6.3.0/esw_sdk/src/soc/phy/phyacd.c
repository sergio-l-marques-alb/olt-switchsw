/*
 * $Id: phyacd.c 1.5 Broadcom SDK $
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
 *
 * File:        phyacd.c
 * Purpose:     PHY ACD routines
 */

#include <sal/core/thread.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/phy/phyctrl.h>
#include "phyreg.h"

int phy_acd_cable_diag_init(int l1,soc_port_t port){phy_ctrl_t*l2;l2 = 
EXT_PHY_SW_STATE(l1,port);SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00
,0x0f00|((0x90)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((
l1),(l2),0x00,0x0f00|((0xC7)&0xff),0x15,(0xA01A)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xC8)&0xff),0x15,(0x0000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xC9)&0xff),0x15
,(0x00EF)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xCC
)&0xff),0x15,(0x200)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,
0x0f00|((0xCE)&0xff),0x15,(0x4000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l2),0x00,0x0f00|((0xCF)&0xff),0x15,(0x3000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE0)&0xff),0x15,(0x0010)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE1)&0xff),0x15
,(0xD0D)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE2)
&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,
0x0f00|((0xE3)&0xff),0x15,(0x1000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l2),0x00,0x0f00|((0xE4)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE7)&0xff),0x15,(0xA0)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xEF)&0xff),0x15
,(0x409F)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xCD
)&0xff),0x15,(0x1000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,
0x0f00|((0xCD)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(
l2),0x00,0x0f00|((0xE0)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE1)&0xff),0x15,(0x0)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE2)&0xff),0x15
,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE3)&
0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00
|((0xE4)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),
0x00,0x0f00|((0xE7)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((
l1),(l2),0x00,0x0f00|((0xEF)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE0)&0xff),0x15,(0x3600)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE1)&0xff),0x15
,(0xC)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE1)&
0xff),0x15,(0x343A)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,
0x0f00|((0xE2)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(
l2),0x00,0x0f00|((0xE3)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE4)&0xff),0x15,(0x8000)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE5)&0xff),0x15
,(0x000C)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE7
)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,
0x0f00|((0xE9)&0xff),0x15,(0x0400)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1
),(l2),0x00,0x0f00|((0xED)&0xff),0x15,(0x0000)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xEF)&0xff),0x15,(0xA1BF)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xCD)&0xff),0x15
,(0x1000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xCD
)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,
0x0f00|((0xE0)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(
l2),0x00,0x0f00|((0xE1)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE2)&0xff),0x15,(0x0)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE3)&0xff),0x15
,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE4)&
0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00
|((0xE5)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),
0x00,0x0f00|((0xE7)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((
l1),(l2),0x00,0x0f00|((0xE8)&0xff),0x15,(0x0)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xE9)&0xff),0x15,(0x0)));
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xED)&0xff),0x15
,(0x0)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xEF)&
0xff),0x15,(0x0)));return SOC_E_NONE;}static int l3(int l1,phy_ctrl_t*l2,
uint16 l4[]){SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1),(l2),0x00,0x0f00|((0xc0
)&0xff),0x15,(&l4[0])));SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1),(l2),0x00,
0x0f00|((0xc1)&0xff),0x15,(&l4[1])));SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1)
,(l2),0x00,0x0f00|((0xc2)&0xff),0x15,(&l4[2])));SOC_IF_ERROR_RETURN(
phy_reg_ge_read((l1),(l2),0x00,0x0f00|((0xc3)&0xff),0x15,(&l4[3])));
SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1),(l2),0x00,0x0f00|((0xc4)&0xff),0x15,
(&l4[4])));SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1),(l2),0x00,0x0f00|((0xc5)&
0xff),0x15,(&l4[5])));SOC_IF_ERROR_RETURN(phy_reg_ge_read((l1),(l2),0x00,
0x0f00|((0xc6)&0xff),0x15,(&l4[6])));SOC_DEBUG_PRINT((DK_PHY,
"u=%d p=%d EXP_C C0=%04x C1=%04x C2=%04x C3=%04x C4=%04x C5=%04x C6=%04x\n",
l1,l2->port,l4[0],l4[1],l4[2],l4[3],l4[4],l4[5],l4[6]));return SOC_E_NONE;}
static int l5(int l1,phy_ctrl_t*l2,int*l6,uint16*l7,int*l8,int*l9){uint16 l10
[7];int l11,l12,l13;l13 = *l6;l10[0] = l10[1] = l10[2] = l10[3] = l10[4] = 
l10[5] = l10[6] = 0;SOC_IF_ERROR_RETURN(l3(l1,l2,l10));if((l10[0]&0x0800)){
SOC_DEBUG_PRINT((DK_PHY,"ACD Engine still busy u=%d p=%d\n",l1,l2->port));}
SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xC0)&0xff),0x15
,(0x2000)));SOC_IF_ERROR_RETURN(phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xC0
)&0xff),0x15,(0xA000)));l12 = 0;for(l11 = 0;l11<300;l11++){
SOC_IF_ERROR_RETURN(l3(l1,l2,l10));if(l10[0]&0x4){l12 = 1;}if(!(l10[0]&0x0800
)){break;}}if(l11>= 300){l13|= 0x0001;SOC_DEBUG_PRINT((DK_PHY,
"u=%d p=%d ERROR_ACD_BUSY_ON_DEMAND\n",l1,l2->port));}if(!l12){l13|= 0x0008;
SOC_DEBUG_PRINT((DK_PHY,"u=%d p=%d ERROR_ACD_NO_NEW_RESULT\n",l1,l2->port));}
if(l10[0]&0x8){l13|= 0x0004;SOC_DEBUG_PRINT((DK_PHY,
"u=%d p=%d ERROR_ACD_INVALID\n",l1,l2->port));}*l7 = l10[1];*l8 = (l10[6]+l10
[4])/2;*l9 = (l10[5]+l10[3])/2;return l13;}int phy_acd_cable_diag(int l1,
soc_port_t port,soc_port_cable_diag_t*l14){phy_ctrl_t*l2;uint16 l7;int l6,l8,
l9;l2 = EXT_PHY_SW_STATE(l1,port);l6 = 0;SOC_IF_ERROR_RETURN(phy_reg_ge_write
((l1),(l2),0x00,0x0f00|((0xA4)&0xff),0x15,(0x0008)));SOC_IF_ERROR_RETURN(
phy_reg_ge_write((l1),(l2),0x00,0x0f00|((0xA4)&0xff),0x15,(0x4008)));l6 = l5(
l1,l2,&l6,&l7,&l8,&l9);if(l6){SOC_DEBUG_PRINT((DK_WARN,
"u=%d p=%d cable diag test failed error_flag = 0x%04x\n",l1,l2->port,l6));
return SOC_E_FAIL;}l14->npairs = 1;l14->fuzz_len = 0;SOC_DEBUG_PRINT((DK_PHY,
"u=%d p=%d fault = %x\n",l1,l2->port,l7));if(l7 == 0x2222){l14->state = 
SOC_PORT_CABLE_STATE_OPEN;l14->pair_state[0] = SOC_PORT_CABLE_STATE_OPEN;l14
->pair_len[0] = (l8*1000)/1325;}else if(l7 == 0x3333){l14->state = 
SOC_PORT_CABLE_STATE_SHORT;l14->pair_state[0] = SOC_PORT_CABLE_STATE_SHORT;
l14->pair_len[0] = (l8*1000)/1325;}else{l14->state = SOC_PORT_CABLE_STATE_OK;
l14->pair_state[0] = SOC_PORT_CABLE_STATE_OK;l14->pair_len[0] = 0;}return
SOC_E_NONE;}
