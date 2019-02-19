/*
 *         
 * $Id: huracan_pkg_cfg.c 2014/04/02 vsrivast Exp $
 * 
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
 *         
 *     
 *
 */
/*
  Includes
*/
#include "huracan_cfg_seq.h"
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
    /*
      { 
            chip_id,
            pkg_lane_num, 
            lane_type,{
            
                        if_side_line { 
                                       
                                        tx {die,lane},
                                                      
                                        rx{die,lane}
                                     } ,
                                       
                        if_side_sys {
     
                                       tx{die,lane},
                                                       
                                       rx{die,lane}
                                                    
                                    }
                     }
                                   
       }
     */

const HURACAN_PKG_LANE_CFG_t glb_lanes_82109[] = {
                                   
                        /*<-------line--------->,<------system---------->*/
                              /*<-tx-> <-rx->*/      
    {0x82109,0,SIMPLEX_TX,{ { { {0,0}, {0,0} } },{ { {0,0}, {0,0} } } } },
    {0x82109,1,SIMPLEX_TX,{ { { {0,1}, {0,1} } },{ { {0,1}, {0,1} } } } },
    {0x82109,2,SIMPLEX_TX,{ { { {0,2}, {0,2} } },{ { {0,2}, {0,2} } } } },
    {0x82109,3,SIMPLEX_TX,{ { { {0,3}, {0,3} } },{ { {0,3}, {0,3} } } } },
    {0x82109,4,SIMPLEX_TX,{ { { {1,0}, {1,0} } },{ { {1,0}, {1,0} } } } },
    {0x82109,5,SIMPLEX_TX,{ { { {1,1}, {1,1} } },{ { {1,1}, {1,1} } } } },
    {0x82109,6,SIMPLEX_TX,{ { { {1,2}, {1,2} } },{ { {1,2}, {1,2} } } } },
    {0x82109,7,SIMPLEX_TX,{ { { {1,3}, {1,3} } },{ { {1,3}, {1,3} } } } },
};

const HURACAN_PKG_LANE_CFG_t glb_lanes_82181[8] = {
    {0x82181,0,DUPLEX_XCVR,{ { { {1,0}, {0,3} } },{ { {0,3}, {1,0} } } } },
    {0x82181,1,DUPLEX_XCVR,{ { { {1,1}, {0,2} } },{ { {0,2}, {1,1} } } } },
    {0x82181,2,DUPLEX_XCVR,{ { { {1,2}, {0,1} } },{ { {0,1}, {1,2} } } } },
    {0x82181,3,DUPLEX_XCVR,{ { { {1,3}, {0,0} } },{ { {0,0}, {1,3} } } } },
    {0x82181,4,DUPLEX_XCVR,{ { { {3,0}, {2,3} } },{ { {2,3}, {3,0} } } } },
    {0x82181,5,DUPLEX_XCVR,{ { { {3,1}, {2,2} } },{ { {2,2}, {3,1} } } } },
    {0x82181,6,DUPLEX_XCVR,{ { { {3,2}, {2,1} } },{ { {2,1}, {3,2} } } } },
    {0x82181,7,DUPLEX_XCVR,{ { { {3,3}, {2,0} } },{ { {2,0}, {3,3} } } } },
};
const HURACAN_PKG_LANE_CFG_t* const glb_pkg_array[MAX_NUM_PACKAGES] = {
    glb_lanes_82109,
    glb_lanes_82181,
    NULL
}; 

const chip_id_quad_info chipid_quad_info[MAX_NUM_PACKAGES-1] = {
    {
        0x82109,
        2,
        {0,1}
    },
      
    {
        0x82181,
        4,
        {0,1,2,3,}
    }
};


