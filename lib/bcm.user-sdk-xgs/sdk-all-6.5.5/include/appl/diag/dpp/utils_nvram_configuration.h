/* $Id: utils_nvram_configuration.h,v 1.4 Broadcom SDK $
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
#ifndef _UTILS_NVRAM_CONFIGURATION_
#define _UTILS_NVRAM_CONFIGURATION_


#if !DUNE_BCM
#include "eeprom.h"
#endif

/********************************************************
* Documentation at utils_nvram_configuration.c
********************************************************/
STATUS
  nvRamGet(
    char *string,    /* where to copy non-volatile RAM    */
    int strLen,      /* maximum number of bytes to copy   */
    int offset       /* byte offset into non-volatile RAM */
    );
/********************************************************
* Documentation at utils_nvram_configuration.c
********************************************************/
STATUS
  nvRamSet(
    char *string,     /* string to be copied into non-volatile RAM */
    int strLen,       /* maximum number of bytes to copy           */
    int offset        /* byte offset into non-volatile RAM         */
    );

/********************************************************
* Documentation at utils_nvram_configuration.c
********************************************************/
int
  host_eeprom_front_card_type_from_nv(
    HOST_EEPROM_FRONT_CARD_TYPE  *host_eeprom_front_card_type_en_ptr
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
********************************************************/
int
  host_eeprom_front_card_type_to_nv(
    HOST_EEPROM_FRONT_CARD_TYPE  host_eeprom_front_card_type_en
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
********************************************************/
int
  host_run_val_of_front_card_type(
    HOST_EEPROM_FRONT_CARD_TYPE  *host_eeprom_front_card_type_en_ptr
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
********************************************************/
const char *
  get_front_card_type_name(
    HOST_EEPROM_FRONT_CARD_TYPE  host_eeprom_front_card_type_en
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
********************************************************/
unsigned int
  is_front_end_ixp2800_connected(
    void
  );
/********************************************************
* Documentation at utils_nvram_configuration.c
********************************************************/
unsigned int
  is_front_end_x10_connected(
    void
  );

/*
 * HOST_DB board related procedures.
 * {
 */
int
  host_db_board_cat_number_from_nv(
    char  *board_serial_num_ptr
    ) ;
int
  host_db_board_description_from_nv(
    char  *board_description_ptr
    ) ;
int
  host_db_board_version_from_nv(
    char  *board_version_ptr
    ) ;
int
  host_db_board_sn_from_nv(
    unsigned short  *board_sn_ptr
    ) ;
int
  host_db_board_param_i_from_nv(
    unsigned int i,
    char  *board_param_i_ptr
    ) ;
int
  host_db_board_type_from_nv(
    unsigned short  *board_board_type_ptr
    ) ;
/*
 * }
 */

int
  host_front_end_type_from_nv(
    SOC_BSP_CARDS_DEFINES  *board_type_ptr
    );

#endif /* _UTILS_NVRAM_CONFIGURATION_ */
