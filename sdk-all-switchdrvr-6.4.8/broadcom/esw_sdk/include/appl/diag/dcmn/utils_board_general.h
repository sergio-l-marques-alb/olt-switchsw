/* $Id: utils_board_general.h,v 1.3 Broadcom SDK $
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


#ifndef __UTILS_BOARD_GEN_H_INCLUDED__
/* { */
#define __UTILS_BOARD_GEN_H_INCLUDED__

#include <appl/diag/dcmn/bsp_cards_consts.h> 

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * Sometimes, the application need to know the device type,
 * before the driver is initialized.
 * D_BOARD_CHIP_TYPE holds the expected device type, according
 * to the board information,
 */
typedef enum
{
  D_BOARD_CHIP_TYPE_FE200,
  D_BOARD_CHIP_TYPE_FAP10M_A,
  D_BOARD_CHIP_TYPE_FAP10M_B,
  D_BOARD_CHIP_TYPE_FAP20V_A,
  D_BOARD_CHIP_TYPE_FAP20V_B,
  D_BOARD_CHIP_TYPE_FAP20M,
  D_BOARD_CHIP_TYPE_FAP21V,
  D_BOARD_CHIP_TYPE_PETRA,
  D_BOARD_CHIP_TYPE_FE600,
  D_BOARD_CHIP_TYPE_T20E,
  D_BOARD_CHIP_TYPE_PB,
  D_BOARD_CHIP_TYPE_LAST

} D_BOARD_CHIP_TYPE;

void
  board_set_device_type(
    D_BOARD_CHIP_TYPE board_chip_type
  );
void
  board_get_device_type(
    D_BOARD_CHIP_TYPE *board_chip_type
  );
const char*
  utils_D_BOARD_CHIP_TYPE_to_string(
    D_BOARD_CHIP_TYPE board_chip_type
  );
int
  utils_print_board_info(
    void
  );

int
  host_board_type_from_nv(
    SOC_BSP_CARDS_DEFINES  *board_board_type_ptr
    ) ;

#ifdef  __cplusplus
}
#endif


/* } __UTILS_BOARD_GEN_H_INCLUDED__*/
#endif

