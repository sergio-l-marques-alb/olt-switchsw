/* $Id: jer_pp_kaps_xpt.h, hagayco Exp $
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

#ifndef __JER_PP_KAPS_XPT_INCLUDED__
/* { */
#define __JER_PP_KAPS_XPT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/kbp/alg_kbp/include/xpt_kaps.h>
#include <soc/kbp/alg_kbp/include/device_kaps.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 * ENUMS     *
 *************/
/* { */

/* } */

/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

struct jericho_data {
    int unit;
};

typedef struct kaps_jericho_xpt {
    struct kaps_xpt jer_kaps_xpt;
    struct jericho_data jer_data; 
} JER_KAPS_XPT;


/* } */

/*************
 * GLOBALS   *
 *************/
/* { */
    


/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32 jer_pp_xpt_init(int unit, void **xpt);

uint32 jer_pp_xpt_deinit(int unit, void *xpt);

kbp_status jer_pp_kaps_read_command(void *xpt,  
                                    uint32 blk_id, 
                                    uint32 cmd,
                                    uint32 func,
                                    uint32 offset,
                                    uint32 n_result_bytes, 
                                    uint8 *result_bytes);

kbp_status jer_pp_kaps_write_command(void *xpt,  
                                     uint8 blk_id, 
                                     uint32 cmd,
                                     uint32 func,
                                     uint32 offset,
                                     uint32 nbytes, 
                                     uint8 *bytes);

uint32 jer_pp_xpt_dma_state(int unit, uint32 print_status, uint32 enable_dma_thread);

/*
 * KAPS ARM Functions
 */
uint32 jer_pp_xpt_arm_init(int unit);
uint32 jer_pp_xpt_arm_start(int unit);
uint32 jer_pp_xpt_arm_load_file_entry(int unit, uint32 *input_32, int entry_num);
uint32 jer_pp_xpt_wait_arm_done(int unit);

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* } __JER_PP_KAPS_XPT_INCLUDED__ */

