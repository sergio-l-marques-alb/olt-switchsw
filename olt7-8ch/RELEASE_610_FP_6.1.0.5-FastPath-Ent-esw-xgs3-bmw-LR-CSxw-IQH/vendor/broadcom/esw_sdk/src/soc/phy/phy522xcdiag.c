/*
 * $Id: phy522xcdiag.c,v 1.1 2011/04/18 17:11:07 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * File:        phy522x.c
 * Purpose:	Broadcom 522x phy driver
 *    	Supports 5218, 5220/5221, 5226, 5228, 5238, 5248
 *      and phy5324(8 ports 10/100 PHY, bcm5324 built-in)
 */
#ifdef INCLUDE_PHY_522X
#include <sal/types.h>
#include <sal/core/thread.h>

#include <soc/phy.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/phyreg.h>

#include <soc/phy.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>

#include "phyident.h"
#include "phyreg.h"
#include "phyfege.h"
#include "phy522x.h"

static int cable_len[] = {0,0,0,0,0,0,0,4,8,12,15,19,22,26,29,32,35,38,41,44,
46,49,51,54,56,59,61,63,66,68,70,72,74,76,78,80,81,83,85,87,89,90,92,94,95,97
,99,100,102,104,105,107,109,110,112,114,115,117,119,121,122,124,126,128};int
phy_522x_cable_diag(int l1,soc_port_t l2,soc_port_cable_diag_t*l3){phy_ctrl_t
*l4;uint16 l5;int l6;int l7;l4 = EXT_PHY_SW_STATE(l1,l2);if(!(PHY_IS_BCM5248(
l4)||PHY_IS_BCM5348(l4)||PHY_IS_BCM5324(l4)||PHY_IS_BCM5324A1(l4)||
PHY_IS_BCM53242(l4)||PHY_IS_BCM53262(l4))){return SOC_E_UNAVAIL;}if(l3 == 
NULL){return SOC_E_PARAM;}SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4,0x1f,0x008b
));sal_usleep(1000);SOC_IF_ERROR_RETURN(READ_PHY_REG(l1,l4,0x14,&l5));if(l5&
0x0040){l3->npairs = 2;l3->fuzz_len = 10;l3->state = l3->pair_state[0] = l3->
pair_state[1] = SOC_PORT_CABLE_STATE_OK;if(!PHY_IS_BCM5248(l4)){
SOC_IF_ERROR_RETURN(READ_PHY_REG(l1,l4,0x1b,&l5));l5 = (l5>>12)&0x7;l3->
pair_len[0] = l3->pair_len[1] = 10+(l5*20);}else{for(l6 = 0,l7 = 0;l7<10;l7++
){SOC_IF_ERROR_RETURN(READ_PHY_REG(l1,l4,0x18,&l5));l5 = (l5>>9)&0x3f;l6+= l5
;}l6 = l6/10;l3->pair_len[0] = l3->pair_len[1] = cable_len[l6];}
SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4,0x1f,0x000b));return SOC_E_NONE;}
SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4,0x14,0x0000));SOC_IF_ERROR_RETURN(
WRITE_PHY_REG(l1,l4,0x13,0x0000));SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4,
0x13,0x0000));SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4,0x13,0x4824));
SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4,0x13,0x4000));SOC_IF_ERROR_RETURN(
WRITE_PHY_REG(l1,l4,0x13,0x0500));SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4,
0x13,0xc404));SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4,0x13,0x0100));
SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4,0x13,0x004c));SOC_IF_ERROR_RETURN(
WRITE_PHY_REG(l1,l4,0x13,0x8006));do{sal_usleep(1000);SOC_IF_ERROR_RETURN(
WRITE_PHY_REG(l1,l4,0x14,0x0000));SOC_IF_ERROR_RETURN(READ_PHY_REG(l1,l4,0x13
,&l5));}while(l5&0x0004);if((l5&0x03c0)!= 0x0300){SOC_IF_ERROR_RETURN(
WRITE_PHY_REG(l1,l4,0x1f,0x000b));return SOC_E_FAIL;}l3->npairs = 2;l3->
fuzz_len = 0;switch((l5>>10)&0x3){case 0:l3->pair_state[0] = 
SOC_PORT_CABLE_STATE_OK;break;case 1:l3->pair_state[0] = 
SOC_PORT_CABLE_STATE_OPEN;break;case 2:l3->pair_state[0] = 
SOC_PORT_CABLE_STATE_SHORT;break;default:l3->pair_state[0] = 
SOC_PORT_CABLE_STATE_UNKNOWN;break;}switch((l5>>12)&0x3){case 0:l3->
pair_state[1] = SOC_PORT_CABLE_STATE_OK;break;case 1:l3->pair_state[1] = 
SOC_PORT_CABLE_STATE_OPEN;break;case 2:l3->pair_state[1] = 
SOC_PORT_CABLE_STATE_SHORT;break;default:l3->pair_state[1] = 
SOC_PORT_CABLE_STATE_UNKNOWN;break;}switch((l5>>12)&0x3){case 0:l3->
pair_state[1] = SOC_PORT_CABLE_STATE_OK;break;case 1:l3->pair_state[1] = 
SOC_PORT_CABLE_STATE_OPEN;break;case 2:l3->pair_state[1] = 
SOC_PORT_CABLE_STATE_SHORT;break;default:l3->pair_state[1] = 
SOC_PORT_CABLE_STATE_UNKNOWN;break;}l3->state = l3->pair_state[0];if(l3->
pair_state[1]>l3->state){l3->state = l3->pair_state[1];}SOC_IF_ERROR_RETURN(
READ_PHY_REG(l1,l4,0x13,&l5));l3->pair_len[0] = ((l5&0xff)*80)/100;l3->
pair_len[1] = (((l5>>8)&0xff)*80)/100;SOC_IF_ERROR_RETURN(WRITE_PHY_REG(l1,l4
,0x1f,0x000b));return SOC_E_NONE;}
#endif /* INCLUDE_PHY_522X */
