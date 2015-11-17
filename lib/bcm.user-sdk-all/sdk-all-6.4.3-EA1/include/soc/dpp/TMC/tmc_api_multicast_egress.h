/* $Id$
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
*/

#ifndef __SOC_TMC_API_MULTICAST_EGRESS_INCLUDED__
/* { */
#define __SOC_TMC_API_MULTICAST_EGRESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id$
 *     In order to configure the egress replication bitmap in
 *     ARAD and above we need a bitmap of at least 256 bits,
 *     In Soc_petra-B we need 80 bits.
 */

#define SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_PETRA   SOC_SAND_DIV_ROUND_UP(SOC_TMC_NOF_FAP_PORTS_PETRA, SOC_SAND_REG_SIZE_BITS)
#define SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_ARAD    SOC_SAND_DIV_ROUND_UP(SOC_TMC_NOF_FAP_PORTS_ARAD, SOC_SAND_REG_SIZE_BITS)
#define SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_MAX     SOC_SAND_DIV_ROUND_UP(SOC_TMC_NOF_FAP_PORTS_MAX, SOC_SAND_REG_SIZE_BITS)

/* mark to mbcm_dpp_mult_copy_replications_from_arrays that a bitmap is being passed */
#define ARAD_MC_EGR_IS_BITMAP_BIT 0x80000000


/*
 *  Maximal number of MC Id-s used for VLAN membership MC
 */
#define SOC_TMC_MULT_EG_VLAN_NOF_IDS_MAX (4 * 1024)

/*
 *  Maximal number of MC Id-s used for VLAN membership MC in Soc_petra
 */

/*
 *  Maximal number of MC Id-s used for VLAN membership MC in ARAD 
 */
#define SOC_TMC_MULT_EG_VLAN_NOF_IDS_MAX_ARAD (8 * 1024)

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

typedef enum
{
  /*
   *  The entry is an OFP port.
   */
  SOC_TMC_MULT_EG_ENTRY_TYPE_OFP = 0,
  /*
   *  The entry is a pointer to an existing VLAN multicast
   *  group. Not valid for Soc_petra-A.
   */
  SOC_TMC_MULT_EG_ENTRY_TYPE_VLAN_PTR = 1,
  /*
   *  Total number of entry types.
   */
  SOC_TMC_MULT_EG_ENTRY_NOF_TYPES = 2
}SOC_TMC_MULT_EG_ENTRY_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The lowest value of MC-ID that is mapped to replication
   *  with bitmap. This range configures a continuous range of
   *  MC-IDs to be used for Vlan-Membership Multicast
   *  purposes. Range: 0.
   */
  uint32 mc_id_low;
  /*
   *  The upper value of MC-ID that is mapped to replication
   *  with bitmap. Range: 0 - 8K-1.
   */
  uint32 mc_id_high;

}SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Entry type: Port or VLAN Multicast group.
   */
  SOC_TMC_MULT_EG_ENTRY_TYPE type;
  /*
   *  Out Going FAP-Port Id. 0 to 79
   */
  SOC_TMC_FAP_PORT_ID port;
  /*
   *  VLAN multicast group Id which the linked list member
   *  points to. Range: 0 - 4K-1. Not valid for Soc_petra-A.
   */
  uint32 vlan_mc_id;
  /*
   *  Copy-Unique-Data for the replication copy (resp. copies,
   *  in case of a VLAN replication) of the packet. This is
   *  the value in the Outgoing TM Header (outlif) when the
   *  packet is sent out of a TM-type port. That is, user
   *  should first allocate the egress-cud in specific FAP,
   *  then add this value to the needed ingress-multicast
   *  groups. Ports of Packet Processing types use this field
   *  as the ARP-Pointer. Range: 0 - 65535.
   */
  uint32 cud;

}SOC_TMC_MULT_EG_ENTRY;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Array of 8 longs (8*32 = 256).
   *  Each bit in this array represents a port of the OFPs (0-255).
   *  (In Soc_petra-B only 0-79 are in use).
   *  The array is constructed from 8 longs, therefore:Ports 0-31 -
   *  represented by bitmap[0]. Ports 32-63 - represented by
   *  bitmap[1]. Ports 64-79 - represented by bitmap[2], and so on.
   *  The entry can be calculated by (PORT div 32).
   *  And the bit can be calculated by - (PORT mod 32).
   */
  uint32 bitmap[SOC_TMC_MULT_EG_NOF_UINT32S_IN_BITMAP_MAX]; 
 
}SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP;

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
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_clear(
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );

void
  SOC_TMC_MULT_EG_ENTRY_clear(
    SOC_SAND_OUT SOC_TMC_MULT_EG_ENTRY *info
  );

void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(
    SOC_SAND_OUT SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_MULT_EG_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_TMC_MULT_EG_ENTRY_TYPE enum_val
  );

void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE_print(
    SOC_SAND_IN SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_RANGE *info
  );

void
  SOC_TMC_MULT_EG_ENTRY_print(
    SOC_SAND_IN SOC_TMC_MULT_EG_ENTRY *info
  );

void
  SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_print(
    SOC_SAND_IN SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *info
  );

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_TMC_API_MULTICAST_EGRESS_INCLUDED__*/
#endif
