/*
 *         
 * $Id: furia_pkg_cfg.c 2014/04/02 palanivk Exp $
 * 
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
 *         
 *     
 *
 */
/*
  Includes
*/
#include "furia_pkg_cfg.h"
#include "furia_types.h"
/*
* Defines
*/

/*
* Types
*/

/*
* Macros
*/

/*
* Global Variables
*/

    /* No package configuration defined. */
    /* Using Plain Duplex with no pkg swap */ 

    /* chip_id, pkg_lane_num, lane_type, die_addr, die_lane_num, slice_rd_val, slice_wr_val,
     * sideB(RX), sideA(TX), inv_in_data, inv_out_data
     */

const FURIA_PKG_LANE_CFG_t glb_lanes_82208[8] = {
    {0x82208, 0, SIMPLEX_TX, 0, 2, 2, 1 << 2, SYS, LINE, 1, 1},
    {0x82208, 1, SIMPLEX_TX, 0, 3, 3, 1 << 3, SYS, LINE, 0, 0},
    {0x82208, 2, SIMPLEX_TX, 0, 3, 3, 1 << 3, LINE, SYS, 1, 1},
    {0x82208, 3, SIMPLEX_TX, 0, 2, 2, 1 << 2, LINE, SYS, 1, 1},
    {0x82208, 4, SIMPLEX_TX, 0, 1, 1, 1 << 1, LINE, SYS, 1, 1},
    {0x82208, 5, SIMPLEX_TX, 0, 0, 0, 1 << 0, LINE, SYS, 0, 0},
    {0x82208, 6, SIMPLEX_TX, 0, 0, 0, 1 << 0, SYS, LINE, 1, 1},
    {0x82208, 7, SIMPLEX_TX, 0, 1, 1, 1 << 1, SYS, LINE, 0, 0}
};

const FURIA_PKG_LANE_CFG_t glb_lanes_82209[8] = {
    {0x82209, 0, SIMPLEX_TX, 0, 1, 1, 1 << 1, SYS, LINE, 0, 0},
    {0x82209, 1, SIMPLEX_TX, 0, 0, 0, 1 << 0, SYS, LINE, 0, 0},
    {0x82209, 2, SIMPLEX_TX, 0, 0, 0, 1 << 0, LINE, SYS, 1, 1},
    {0x82209, 3, SIMPLEX_TX, 0, 1, 1, 1 << 1, LINE, SYS, 1, 1},
    {0x82209, 4, SIMPLEX_TX, 0, 2, 2, 1 << 2, LINE, SYS, 0, 0},
    {0x82209, 5, SIMPLEX_TX, 0, 3, 3, 1 << 3, LINE, SYS, 0, 0},
    {0x82209, 6, SIMPLEX_TX, 0, 3, 3, 1 << 3, SYS, LINE, 0, 0},
    {0x82209, 7, SIMPLEX_TX, 0, 2, 2, 1 << 2, SYS, LINE, 1, 1}
};

const FURIA_PKG_LANE_CFG_t glb_lanes_82073[8] = {
    {0x82073, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82073, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82073, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82073, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
    {0x82073, 4, DUPLEX_XCVR, 1, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82073, 5, DUPLEX_XCVR, 1, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82073, 6, DUPLEX_XCVR, 1, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82073, 7, DUPLEX_XCVR, 1, 0, 0, 1<<0, SYS, LINE, 0, 0}
};

const FURIA_PKG_LANE_CFG_t glb_lanes_82381[8] = {
    {0x82381, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82381, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82381, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82381, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
    {0x82381, 4, DUPLEX_XCVR, 1, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82381, 5, DUPLEX_XCVR, 1, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82381, 6, DUPLEX_XCVR, 1, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82381, 7, DUPLEX_XCVR, 1, 0, 0, 1<<0, SYS, LINE, 0, 0}
};
const FURIA_PKG_LANE_CFG_t glb_lanes_82071[4] = {
    {0x82071, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82071, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82071, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82071, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
};
const FURIA_PKG_LANE_CFG_t glb_lanes_82380[4] = {
    {0x82380, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82380, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82380, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82380, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
};
const FURIA_PKG_LANE_CFG_t glb_lanes_82072[8] = {
    {0x82072, 0, DUPLEX_XCVR, 0, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82072, 1, DUPLEX_XCVR, 0, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82072, 2, DUPLEX_XCVR, 0, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82072, 3, DUPLEX_XCVR, 0, 0, 0, 1<<0, SYS, LINE, 0, 0},
    {0x82072, 4, DUPLEX_XCVR, 1, 3, 3, 1<<3, SYS, LINE, 0, 0},
    {0x82072, 5, DUPLEX_XCVR, 1, 2, 2, 1<<2, SYS, LINE, 0, 0},
    {0x82072, 6, DUPLEX_XCVR, 1, 1, 1, 1<<1, SYS, LINE, 0, 0},
    {0x82072, 7, DUPLEX_XCVR, 1, 0, 0, 1<<0, SYS, LINE, 0, 0}
};
const FURIA_PKG_LANE_CFG_t* const glb_package_array[MAX_NUM_PACKAGES] = {
    glb_lanes_82208,
    glb_lanes_82209,
    glb_lanes_82073,
    glb_lanes_82381,
    glb_lanes_82071,
    glb_lanes_82380,
    glb_lanes_82072,
    NULL
}; 

FURIA_PHY_LIST_t glb_phy_list[MAX_NUM_PHYS];


