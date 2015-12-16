/* $Id: sand_pp_mac.h,v 1.5 Broadcom SDK $
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

#ifndef  SOC_SAND_DRIVER_MAC_H
#define SOC_SAND_DRIVER_MAC_H

/*************
* INCLUDES  *
*************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

/* } */
/*************
* DEFINES   *
*************/
/* { */


/* $Id: sand_pp_mac.h,v 1.5 Broadcom SDK $
 * number of characters in MAC address string.
 */
#define  SOC_SAND_PP_MAC_ADDRESS_STRING_LEN 12
/*
 * number of bytes in MAC address
 */
#define  SOC_SAND_PP_MAC_ADDRESS_NOF_U8 6
/*
 * number of bits in MAC address
 */
#define  SOC_SAND_PP_MAC_ADDRESS_NOF_BITS (SOC_SAND_PP_MAC_ADDRESS_NOF_U8 * SOC_SAND_NOF_BITS_IN_CHAR)
/*
 * number of longs in MAC address
 */
#define  SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S SOC_SAND_U64_NOF_UINT32S


/* } */

/*************
* MACROS    *
*************/
/* { */

#define SOC_SAND_PP_MAC_ADDRESS_IS_ZERO(_mac_)  \
    (((_mac_).address[0] | (_mac_).address[1] | (_mac_).address[2] | \
      (_mac_).address[3] | (_mac_).address[4] | (_mac_).address[5]) == 0) 

/* } */

/*************
* TYPE DEFS *
*************/
/* { */

/*
 *  MAC Address.mac[0] includes the lsb of the MAC address
 *  (network order).
 */
  typedef struct
  {
    SOC_SAND_MAGIC_NUM_VAR
    uint8  address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8];
  }SOC_SAND_PP_MAC_ADDRESS;


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

/*********************************************************************
* NAME:
*     soc_sand_pp_mac_address_struct_to_long
* TYPE:
*   PROC
* DATE:
*   Oct  13 2007
* FUNCTION:
*     Converts MAC address from struct to uint32
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac_add -
*    mac address as struct (6 chars)
*  SOC_SAND_OUT uint32            *mac_add_U32 -
*    mac address as 2 uint32s
* RETORNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_pp_mac_address_struct_to_long(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac_add,
    SOC_SAND_OUT uint32            mac_add_U32[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S]
  );

/*********************************************************************
* NAME:
*     soc_sand_pp_mac_address_long_to_struct
* TYPE:
*   PROC
* DATE:
*   Oct  13 2007
* FUNCTION:
*     Converts MAC address from an array of two uint32s to a struct
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32            mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S] -
*    mac address as long (2 uint32s)
*  SOC_SAND_OUT  SOC_SAND_PP_MAC_ADDRESS    *mac_add_struct -
*    mac address as struct (6 chars)
* RETORNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_pp_mac_address_long_to_struct(
    SOC_SAND_IN  uint32            mac_add_long[SOC_SAND_PP_MAC_ADDRESS_NOF_UINT32S],
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS    *mac_add_struct
  );


/*********************************************************************
* NAME:
*     soc_sand_pp_mac_address_inc
* TYPE:
*   PROC
* DATE:
*   Oct  13 2007
* FUNCTION:
*     calculate mac1 += 1
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac -
*    mac address to add to.
* RETORNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_pp_mac_address_inc(
    SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac
  );

/*********************************************************************
* NAME:
*     soc_sand_pp_mac_address_are_equal
* TYPE:
*   PROC
* DATE:
*   Oct  13 2007
* FUNCTION:
*     checks if the mac address are equal
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac1 -
*    mac address 1.
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS      *mac2 -
*    mac address 2.
* RETORNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_pp_mac_address_are_equal(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac2,
    SOC_SAND_OUT  uint8              *equal
  );

/*********************************************************************
* NAME:
*     soc_sand_pp_mac_address_is_smaller
* TYPE:
*   PROC
* DATE:
*   Oct  13 2007
* FUNCTION:
*     checks if the mac address are equal
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac1 -
*    mac address 1.
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS      *mac2 -
*    mac address 2.
* RETORNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_pp_mac_address_is_smaller(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac2,
    SOC_SAND_OUT  uint8              *is_smaller
  );

/*********************************************************************
* NAME:
*     soc_sand_pp_mac_address_add
* TYPE:
*   PROC
* DATE:
*   Oct  13 2007
* FUNCTION:
*     calculate mac1 += mac2
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac1 -
*    mac address to add to.
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS      *mac2 -
*    mac address to add.
* RETORNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_pp_mac_address_add(
    SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS      *mac2
  );


/*********************************************************************
* NAME:
*     soc_sand_pp_mac_address_sub
* TYPE:
*   PROC
* DATE:
*   Oct  13 2007
* FUNCTION:
*     Substract mac2 from mac1
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac1 -
*    mac address as struct (6 chars)
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *mac2 -
*    mac address as struct (6 chars)
* RETORNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_pp_mac_address_sub(
    SOC_SAND_INOUT  SOC_SAND_PP_MAC_ADDRESS   *mac1,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS      *mac2
  );


/*********************************************************************
* NAME:
*     soc_sand_pp_mac_address_string_parse
* TYPE:
*   PROC
* DATE:
*   Oct  13 2007
* FUNCTION:
*     parse an input string of mac address.
*     For example input "01234500" is parsed to 00:00:01:23:45:00
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN char mac[SOC_SAND_PP_MAC_ADDRESS_STRING_LEN] -
*    mac address as string, array of chars.
*  SOC_SAND_OUT  SOC_SAND_PP_MAC_ADDRESS   mac_addr -
*    mac address as struct (6 chars)
* REMARKS:
*  - the input string doesn't include 0x
*  - the string is padded with initial zeros if needed.
*  - the lsb in the MAC array (mac[0]) is the right most value in the string.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_pp_mac_address_string_parse(
    SOC_SAND_IN char               mac_string[SOC_SAND_PP_MAC_ADDRESS_STRING_LEN],
    SOC_SAND_OUT  SOC_SAND_PP_MAC_ADDRESS   *mac_addr
  );


/*********************************************************************
* NAME:
*     soc_sand_pp_mac_address_reverse
* TYPE:
*   PROC
* DATE:
*   Oct  13 2007
* FUNCTION:
*     Reverse the order of the mac address in bytes resolution.
*     Example: for in_mac_add 00:00:01:23:45:00 the out_mac_add is 00:45 23:01:00:00.
* INPUT:
*  SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *in_mac_add -
*    input mac address,
*  SOC_SAND_OUT  SOC_SAND_PP_MAC_ADDRESS   mac_addr -
*    reversed mac address
* REMARKS:
*  - the input string doesn't include 0x
*  - the string is padded with initial zeros if needed.
*  - the lsb in the MAC array (mac[0]) is the right most value in the string.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_sand_pp_mac_address_reverse(
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS    *in_mac_add,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS    *out_mac_add
  );

/*
 * }
 */

void
  soc_sand_SAND_PP_MAC_ADDRESS_clear(
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS *mac_addr
  );

uint32
  SOC_SAND_PP_MAC_ADDRESS_verify(
    SOC_SAND_IN SOC_SAND_PP_MAC_ADDRESS *mac_addr
  );

#if SOC_SAND_DEBUG
/* { */

/*
 *  Formatted print of the MAC address (00:00:01:23:45:00)
 *  print msb first (mac_addr[5])
 */
void
  soc_sand_SAND_PP_MAC_ADDRESS_print(
    SOC_SAND_IN SOC_SAND_PP_MAC_ADDRESS *mac_addr
  );

void
  soc_sand_SAND_PP_MAC_ADDRESS_array_print(
    SOC_SAND_IN uint8       mac_address[SOC_SAND_PP_MAC_ADDRESS_NOF_U8]
  );


void
  soc_sand_pp_mac_tests(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint8 silent
  );


/* } */
#endif


#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif
