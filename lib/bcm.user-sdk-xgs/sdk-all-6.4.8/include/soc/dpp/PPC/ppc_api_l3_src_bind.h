/* $Id: ppc_api_lag.h,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/ppc/include/soc_ppc_api_l3_src_bind.h
*
* MODULE PREFIX:  soc_ppc_src_bind
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_PPC_API_L3_SRC_BIND_INCLUDED__
/* { */
#define __SOC_PPC_API_L3_SRC_BIND_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>

/* } */
/*************
 * DEFINES   *
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

#define SOC_PPC_IP6_COMPRESSION_DIP 0x01 /* indicate DIP compression */
#define SOC_PPC_IP6_COMPRESSION_SIP 0x02 /* indicate SIP compression */
#define SOC_PPC_IP6_COMPRESSION_TCAM 0x04 /* TCAM entry  */
#define SOC_PPC_IP6_COMPRESSION_LEM  0x08 /* LEM entry */

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Source MAC address
   */
  SOC_SAND_PP_MAC_ADDRESS smac;
  /*
   *  If set to FALSE then SMAC is masked.
   */
  uint8 smac_valid;
  /*
   *  Packet SIP (Source IP address). 
   */
  uint32 sip;
  /*
   *  Logical Interface ID. 
   */
  SOC_PPC_LIF_ID lif_ndx;
  /*
   *  Number of bits to consider in the IP address starting 
   *  from the msb. Range: 0 - 32.Example for key ip_address 
   *  192.168.1.0 and prefix_len 24 would match any IP Address 
   *  of the form 192.168.1.x                                 
   */
  uint8 prefix_len;
  /*
   *  Set to TRUE means the entry is for network IP anti-spoofing.
   */
  uint8 is_network;
  
  SOC_PPC_VSI_ID  vsi_id;
} SOC_PPC_SRC_BIND_IPV4_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Source MAC address
   */
  SOC_SAND_PP_MAC_ADDRESS smac;
  /*
   *  If set to FALSE then SMAC is masked.
   */
  uint8 smac_valid;
  /*
   *  Packet SIP (Source IPv6 address). 
   */
  SOC_SAND_PP_IPV6_ADDRESS sip6;

  /*
   *  Logical Interface ID. 
   */
  SOC_PPC_LIF_ID lif_ndx;
  /*
   *  Number of bits to consider in the IP address starting 
   *  from the msb.Range: 0 - 128.                            
   */
  uint8 prefix_len;
  /*
   *  Set to TRUE means the entry is for network IP anti-spoofing.
   */
  uint8 is_network;
} SOC_PPC_SRC_BIND_IPV6_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /* Indicate the compression IP whether it's SIP or DIP */
  uint32 flags;
  /*  Compression IPv6 address. */
  SOC_SAND_PP_IPV6_SUBNET ip6;
  
  /*
   *  Logical Interface ID, same with FEC ID. 
   */
  SOC_PPC_LIF_ID lif_ndx;

  /* tt result compression result */
  uint16 ip6_tt_compressed_result;

  /* Ip6 comprssion result, it will be used in PMF */
  uint32 ip6_compressed_result;
  /* Indicate it's IP6 spoof compression or DIP compression  */
  uint8 is_spoof;
  
  SOC_PPC_VSI_ID  vsi_id;
} SOC_PPC_IPV6_COMPRESSED_ENTRY;

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

void
  SOC_PPC_SRC_BIND_IPV4_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV4_ENTRY     *info
  );

void
  SOC_PPC_SRC_BIND_IPV6_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV6_ENTRY     *info
  );

void
  SOC_PPC_SRC_BIND_IPV4_ENTRY_print(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV4_ENTRY     *info
  );

void
  SOC_PPC_SRC_BIND_IPV6_ENTRY_print(
    SOC_SAND_OUT SOC_PPC_SRC_BIND_IPV6_ENTRY     *info
  );


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_L3_SRC_BIND_INCLUDED__*/
#endif

