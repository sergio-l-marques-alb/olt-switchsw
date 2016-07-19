/* $Id: utils_board_general.c,v 1.7 Broadcom SDK $
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


#include <shared/bsl.h>

#include <sal/types.h>
#include <soc/drv.h>

#include <soc/dcmn/dcmn_utils_eeprom.h>

#include <appl/diag/dcmn/utils_board_general.h>
#include <appl/diag/dcmn/bsp_cards_consts.h>

static D_BOARD_CHIP_TYPE Board_chip_type=D_BOARD_CHIP_TYPE_LAST;

#if (defined(LINUX) || defined(UNIX)) || defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__) || defined(BCM_DPP_SUPPORT)
int
  host_board_type_from_nv(
    SOC_BSP_CARDS_DEFINES  *board_board_type_ptr
    )
{
    int ret = 0;

#ifdef PLISIM
    *board_board_type_ptr = LINE_CARD_GFA_PETRA_B_INTERLAKEN;
#else
#if defined(__DUNE_GTO_BCM_CPU__) || defined(__DUNE_WRX_BCM_CPU__)
    ret = eeprom_read(NEGEV_CHASSIS_CARD_TYPE, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE, NEGEV_CHASSIS_EEPROM_BYTE_SIZE_BOARD_TYPE, (int *)board_board_type_ptr);
    if (ret != 0) {
      cli_out("Error in %s(): eeprom_read(). FAILED !!!\n", FUNCTION_NAME());
    }
#endif
#endif

    return ret;
}
#endif

void
  board_set_device_type(
    D_BOARD_CHIP_TYPE board_chip_type
  )
{
  Board_chip_type = board_chip_type;
  return;
}

void
  board_get_device_type(
    D_BOARD_CHIP_TYPE *board_chip_type
  )
{
  *board_chip_type = Board_chip_type;
  return;
}


const char*
  utils_D_BOARD_CHIP_TYPE_to_string(
    D_BOARD_CHIP_TYPE board_chip_type
  )
{
  const char* str = 0;
  switch(board_chip_type)
  {
  case D_BOARD_CHIP_TYPE_FE200:
    str = "D_BOARD_CHIP_TYPE_FE200";
    break;
  case D_BOARD_CHIP_TYPE_FAP10M_A:
    str = "D_BOARD_CHIP_TYPE_FAP10M_A";
    break;
  case D_BOARD_CHIP_TYPE_FAP10M_B:
    str = "D_BOARD_CHIP_TYPE_FAP10M_B";
    break;
  case D_BOARD_CHIP_TYPE_FAP20V_A:
    str = "D_BOARD_CHIP_TYPE_FAP20V_A";
    break;
  case D_BOARD_CHIP_TYPE_FAP20V_B:
    str = "D_BOARD_CHIP_TYPE_FAP20V_B";
    break;
  case D_BOARD_CHIP_TYPE_FAP21V:
    str = "D_BOARD_CHIP_TYPE_FAP21V";
    break;
  case D_BOARD_CHIP_TYPE_PETRA:
    str = "D_BOARD_CHIP_TYPE_PETRA";
    break;
  case D_BOARD_CHIP_TYPE_PB:
    str = "D_BOARD_CHIP_TYPE_PB";
    break;
  case D_BOARD_CHIP_TYPE_LAST:
    str = "D_BOARD_CHIP_TYPE_LAST";
    break;
  default:
    str = "***utils_D_BOARD_CHIP_TYPE_to_string got error in input***";
  }
  return str;
}

int
  utils_print_board_info(
    void
  )
{
  D_BOARD_CHIP_TYPE
    board_chip_type;

  board_get_device_type(&board_chip_type);

  return 0;
}
