/* $Id: pb_pp_frwrd_ip_tcam.c,v 1.8 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>

#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv6.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ip_tcam.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ipv6.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV4_MC                         (0)
#define SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_UC                         (1)
#define SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_MC                         (2)

#define SOC_PB_PP_FRWRD_IP_TCAM_FLAG_IPV4_MC                       (1 << SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV4_MC)
#define SOC_PB_PP_FRWRD_IP_TCAM_FLAG_IPV6_UC                       (1 << SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_UC)
#define SOC_PB_PP_FRWRD_IP_TCAM_FLAG_IPV6_MC                       (1 << SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_MC)

#define SOC_PB_PP_FRWRD_IP_TCAM_CYCLE_DFLT                         (0)
#define SOC_PB_PP_FRWRD_IP_TCAM_MIN_BANKS                          (1)





#define SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(input_bitstream, nof_bits, output_stream) \
  res = soc_sand_bitstream_set_any_field(     \
    input_bitstream,                        \
    start_bit,                              \
    nof_bits,                               \
    output_stream                           \
  );                                      \
  SOC_SAND_CHECK_FUNC_RESULT(res,  23, exit); \
  start_bit += nof_bits;

#define SOC_PB_PP_IPV6_TCAM_ENTRY_TO_VRF_AND_SUBNET(key_param, vrf_ndx_param, subnet_param) \
  if(key_param->type == SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC)                   \
  {                                                                   \
    vrf_ndx_param = &key_param->vrf_ndx;                                     \
    subnet_param = &key_param->key.ipv6_uc.subnet;                                      \
  }                                                                   \
  else if(key->type == SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN)  \
  {                                                       \
    vrf_ndx_param = &key_param->vrf_ndx;                         \
    subnet_param = &key_param->key.ipv6_vpn.subnet;                          \
  } \
  else \
  { \
    vrf_ndx_param = NULL; \
    subnet_param = NULL; \
  }

/* $Id: pb_pp_frwrd_ip_tcam.c,v 1.8 Broadcom SDK $
 * entry type for tcam mgmt key_to_id multiset,
 * distinquishes between ipv6 mc and ipv6 uc/vpn entries
 */
#define SOC_PB_PP_IP_TCAM_ENTRY_KEY_TYPE_IPV6_UC_OR_VPN 0
#define SOC_PB_PP_IP_TCAM_ENTRY_KEY_TYPE_IPV6_MC 1
#define SOC_PB_PP_IP_TCAM_ENTRY_KEY_TYPE_NOF_BITS 1

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

/*
 *  Internal functions
 */

STATIC
  uint32
    soc_pb_pp_frwrd_ip_tcam_callback(
      SOC_SAND_IN int unit,
      SOC_SAND_IN uint32  user_data
    )
{
  uint32
    res = SOC_SAND_OK,
    bitmap = 0;
  uint32
    bank_id;
  uint8
    is_used;
  SOC_PB_TCAM_PREFIX
    prefix;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  prefix.bits   = 0x0;
  prefix.length = 4;

  for (bank_id = 0; bank_id < SOC_PB_TCAM_NOF_BANKS; ++bank_id)
  {
    bitmap <<= 1;
    res = soc_pb_tcam_db_is_bank_used_unsafe(
            unit,
            user_data,
            SOC_PB_TCAM_NOF_BANKS - bank_id - 1,
            &is_used
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (is_used)
    {
      bitmap |= 0x1;
      res = soc_pb_tcam_db_bank_prefix_get_unsafe(
              unit,
              user_data,
              SOC_PB_TCAM_NOF_BANKS - bank_id - 1,
              &prefix
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

  if (bitmap != 0x0)
  {
    switch (user_data)
    {
    case SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV4_MC:
      res = soc_pb_pp_lem_access_ipv4_mc_bank_bitmap_set(
              unit,
              bitmap
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      res = soc_pb_pp_lem_access_ipv4_mc_tcam_prefix_set(
              unit,
              prefix.bits,
              prefix.length
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      break;

    case SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_UC:
      res = soc_pb_pp_lem_access_ipv6_bank_bitmap_set(
              unit,
              SOC_PB_TCAM_USER_FWDING_IPV6_UC,
              bitmap
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = soc_pb_pp_lem_access_ipv6_tcam_prefix_set(
              unit,
              SOC_PB_TCAM_USER_FWDING_IPV6_UC,
              prefix.bits,
              prefix.length
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      break;

    case SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_MC:
      res = soc_pb_pp_lem_access_ipv6_bank_bitmap_set(
              unit,
              SOC_PB_TCAM_USER_FWDING_IPV6_MC,
              bitmap
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      res = soc_pb_pp_lem_access_ipv6_tcam_prefix_set(
              unit,
              SOC_PB_TCAM_USER_FWDING_IPV6_MC,
              prefix.bits,
              prefix.length
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
      break;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ip_tcam_callback()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ip_tcam_init_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32  required_uses
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_SUCCESS_FAILURE
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IP_TCAM_INIT_UNSAFE);

  /*
   *  Create TCAM databases
   */
  if (required_uses & SOC_PB_PP_FRWRD_IP_TCAM_FLAG_IPV4_MC)
  {
    res = soc_pb_tcam_db_create_unsafe(
            unit,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV4_MC,
            SOC_PB_TCAM_BANK_ENTRY_SIZE_72_BITS,
            0,
            SOC_PB_TCAM_DB_PRIO_MODE_BANK
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_pb_tcam_access_profile_create_unsafe(
            unit,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV4_MC,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV4_MC,
            SOC_PB_PP_FRWRD_IP_TCAM_CYCLE_DFLT,
            TRUE,
            SOC_PB_PP_FRWRD_IP_TCAM_MIN_BANKS,
            soc_pb_pp_frwrd_ip_tcam_callback,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV4_MC,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if (required_uses & SOC_PB_PP_FRWRD_IP_TCAM_FLAG_IPV6_UC)
  {
    res = soc_pb_tcam_db_create_unsafe(
            unit,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_UC,
            SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS,
            4,
            SOC_PB_TCAM_DB_PRIO_MODE_BANK
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_pb_tcam_access_profile_create_unsafe(
            unit,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_UC,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_UC,
            SOC_PB_PP_FRWRD_IP_TCAM_CYCLE_DFLT,
            TRUE,
            SOC_PB_PP_FRWRD_IP_TCAM_MIN_BANKS,
            soc_pb_pp_frwrd_ip_tcam_callback,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_UC,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (required_uses & SOC_PB_PP_FRWRD_IP_TCAM_FLAG_IPV6_MC)
  {
    res = soc_pb_tcam_db_create_unsafe(
            unit,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_MC,
            SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS,
            4,
            SOC_PB_TCAM_DB_PRIO_MODE_BANK
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    res = soc_pb_tcam_access_profile_create_unsafe(
            unit,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_MC,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_MC,
            SOC_PB_PP_FRWRD_IP_TCAM_CYCLE_DFLT,
            TRUE,
            SOC_PB_PP_FRWRD_IP_TCAM_MIN_BANKS,
            soc_pb_pp_frwrd_ip_tcam_callback,
            SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_MC,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ip_tcam_init_unsafe()", 0, 0);
}
STATIC
  void
    soc_pb_pp_ipv6_prefix_to_mask(
      SOC_SAND_IN  uint32 prefix,
      SOC_SAND_OUT uint32 mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S]
    )
{
  int32
    counter = prefix,
    word = 3;

  mask[0] = mask[1] = mask[2] = mask[3] = 0;

  while (counter >= 32)
  {
    mask[word--] = 0xffffffff;
    counter -= 32;
  }
  if (counter > 0)
  {
    mask[word] = SOC_SAND_BITS_MASK(31, 32 - counter);
  }
}

STATIC uint16
  soc_pb_pp_ip_tcam_route_to_prio(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY  *key
  )
{
  uint16
    prio = SOC_SAND_U16_MAX;

  switch(key->type)
  {
  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV4_MC:
    prio = prio - ((uint16)32 * key->key.ipv4_mc.inrif_valid);
    prio = prio - (uint16)key->key.ipv4_mc.source.prefix_len;
    break;
  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC:
    prio = prio - (uint16)soc_sand_bitstream_get_nof_on_bits(&key->key.ipv6_mc.inrif.mask, 1);
    break;
  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC:
    prio = prio - (uint16) key->key.ipv6_uc.subnet.prefix_len;
    break;
  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
    prio = prio - (uint16) key->key.ipv6_vpn.subnet.prefix_len;
    break;
  default:
    break;
  }
  
  return prio;
}

STATIC
  uint32
    soc_pb_pp_ip_tcam_key_to_stream(
      SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY  *key,
      SOC_SAND_OUT uint8                  stream[SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES]
  )
{
  uint32
    tmp,
    long_stream[5];

  const uint32
    *vrf_ndx;
  uint8
    start_bit;
  uint32
    res;
  const SOC_SAND_PP_IPV6_SUBNET
    *subnet;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  start_bit = 0;
  tmp = 0;

  res = SOC_SAND_OK; sal_memset(
          long_stream,
          0x0,
          sizeof(uint32) * 5
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,  20, exit);

  switch(key->type)
  {
    case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV4_MC:
      tmp = key->key.ipv4_mc.inrif_valid;
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, 1, long_stream);

      tmp = key->key.ipv4_mc.inrif;
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_DPP_IN_RIF_NOF_BITS_PETRAB, long_stream);

      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&key->key.ipv4_mc.source.ip_address, 32, long_stream);

      tmp = key->key.ipv4_mc.source.prefix_len;
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, 8, long_stream);

      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&key->key.ipv4_mc.group, 28, long_stream);
      break;

    case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC:
      tmp = SOC_PB_PP_IP_TCAM_ENTRY_KEY_TYPE_IPV6_MC;
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_PB_PP_IP_TCAM_ENTRY_KEY_TYPE_NOF_BITS, long_stream);

      tmp = key->key.ipv6_mc.inrif.mask;
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_DPP_IN_RIF_NOF_BITS_PETRAB, long_stream);

      tmp = key->key.ipv6_mc.inrif.val;
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_DPP_IN_RIF_NOF_BITS_PETRAB, long_stream);

      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(key->key.ipv6_mc.group.address, SOC_PB_PP_IPV6_MC_GROUP_NOF_BITS, long_stream);
      break;

    case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC:
    case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
      tmp = SOC_PB_PP_IP_TCAM_ENTRY_KEY_TYPE_IPV6_UC_OR_VPN;
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_PB_PP_IP_TCAM_ENTRY_KEY_TYPE_NOF_BITS, long_stream);

      SOC_PB_PP_IPV6_TCAM_ENTRY_TO_VRF_AND_SUBNET(key, vrf_ndx, subnet);
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(vrf_ndx, SOC_DPP_VRF_NOF_BITS_PETRAB, long_stream);
      
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(subnet->ipv6_address.address, SOC_SAND_PP_IPV6_ADDRESS_NOF_BITS, long_stream);

      tmp = subnet->prefix_len;
      SOC_PB_PP_IP_BIT_STREAM_SET_ANY_FIELD(&tmp, SOC_SAND_NOF_BITS_IN_CHAR, long_stream);
      break;
    default:
      break;
  }

  /*
   *  Add key type
   */
  tmp = key->type;
  soc_sand_bitstream_set_any_field(&tmp, 157, 3, long_stream);

  res = soc_sand_U32_to_U8(
          long_stream,
          SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES,
          stream
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,  100, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_ip_tcam_key_to_stream()",0,0);
}

STATIC
  void
    soc_pb_pp_ip_tcam_stream_to_key(
      SOC_SAND_IN  uint8                 stream[SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES],
      SOC_SAND_OUT SOC_PB_PP_IP_TCAM_ENTRY_KEY *key
    )
{
  uint32
    long_stream[5],
    tmp = 0;

  sal_memset(
    key,
    0x0,
    sizeof(SOC_PB_PP_IP_TCAM_ENTRY_KEY)
  );
 /* * COVERITY * 
   * overun was not found. */
 /* coverity[overrun-buffer-arg] */
  soc_sand_U8_to_U32(
    stream,
    SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES,
    long_stream
  );

  soc_sand_bitstream_get_any_field(long_stream, 157, 3, &tmp);
  key->type = tmp;
  switch (key->type)
  {
  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV4_MC:
    soc_sand_bitstream_get_any_field(long_stream, 0, 1, &tmp);
    key->key.ipv4_mc.inrif_valid = SOC_SAND_NUM2BOOL(tmp);

    soc_sand_bitstream_get_any_field(long_stream, 1, 12, &tmp);
    key->key.ipv4_mc.inrif = tmp;
 
    soc_sand_bitstream_get_any_field(long_stream, 13, 32, &key->key.ipv4_mc.source.ip_address);

    soc_sand_bitstream_get_any_field(long_stream, 45, 8, &tmp);
    key->key.ipv4_mc.source.prefix_len = (uint8) tmp;

    soc_sand_bitstream_get_any_field(long_stream, 53, 28, &key->key.ipv4_mc.group);
    key->key.ipv4_mc.group |= SOC_SAND_PP_IPV4_MC_ADDR_PREFIX;
    break;

  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC:
    soc_sand_bitstream_get_any_field(long_stream, 1, 12, &tmp);
    key->key.ipv6_mc.inrif.mask = tmp;

    soc_sand_bitstream_get_any_field(long_stream, 13, 12, &tmp);
    key->key.ipv6_mc.inrif.val = tmp;

    soc_sand_bitstream_get_any_field(long_stream, 25, SOC_PB_PP_IPV6_MC_GROUP_NOF_BITS, key->key.ipv6_mc.group.address);
    key->key.ipv6_mc.group.address[3] |= 0xff000000;
    break;

  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC:
    soc_sand_bitstream_get_any_field(long_stream, 1, 8, &key->vrf_ndx);
    
    soc_sand_bitstream_get_any_field(long_stream, 9, 128, key->key.ipv6_uc.subnet.ipv6_address.address);

    soc_sand_bitstream_get_any_field(long_stream, 137, SOC_SAND_NOF_BITS_IN_CHAR, &tmp);
    key->key.ipv6_uc.subnet.prefix_len = (uint8) tmp;
    break;

  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
    soc_sand_bitstream_get_any_field(long_stream, 1, 8, &key->vrf_ndx);

    soc_sand_bitstream_get_any_field(long_stream, 9, 128, key->key.ipv6_vpn.subnet.ipv6_address.address);

    soc_sand_bitstream_get_any_field(long_stream, 137, SOC_SAND_NOF_BITS_IN_CHAR, &tmp);
    key->key.ipv6_vpn.subnet.prefix_len = (uint8) tmp;
    break;

  default:
    break;
  }
}

STATIC
  void
    soc_pb_pp_ip_tcam_entry_build(
      SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY *key,
      SOC_SAND_OUT SOC_PB_TCAM_ENTRY           *entry
  )
{
  uint32
    mask,
    ipv6_mask[SOC_SAND_PP_IPV6_ADDRESS_NOF_UINT32S];

  SOC_PB_TCAM_ENTRY_clear(entry);

  switch(key->type)
  {
  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV4_MC:
    mask = SOC_SAND_BITS_MASK(27, 0);
    soc_sand_bitstream_set_any_field(&key->key.ipv4_mc.group, 0, 28, entry->value);
    soc_sand_bitstream_set_any_field(&mask,                   0, 28, entry->mask);

    mask = (key->key.ipv4_mc.source.prefix_len > 0) ?
             SOC_SAND_BITS_MASK(31, 32 - key->key.ipv4_mc.source.prefix_len) : 0;
    soc_sand_bitstream_set_any_field(&key->key.ipv4_mc.source.ip_address, 28, 32, entry->value);
    soc_sand_bitstream_set_any_field(&mask,                               28, 32, entry->mask);

    mask = (key->key.ipv4_mc.inrif_valid) ? SOC_SAND_BITS_MASK(SOC_DPP_IN_RIF_NOF_BITS_PETRAB - 1, 0) : 0;
    soc_sand_bitstream_set_any_field(&key->key.ipv4_mc.inrif, 60, SOC_DPP_IN_RIF_NOF_BITS_PETRAB, entry->value);
    soc_sand_bitstream_set_any_field(&mask,                   60, SOC_DPP_IN_RIF_NOF_BITS_PETRAB, entry->mask);
    break;

  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC:
    soc_pb_pp_ipv6_prefix_to_mask(key->key.ipv6_uc.subnet.prefix_len, ipv6_mask);
    soc_sand_bitstream_set_any_field(key->key.ipv6_uc.subnet.ipv6_address.address, 0, 128, entry->value);
    soc_sand_bitstream_set_any_field(ipv6_mask,                                    0, 128, entry->mask);

    mask = SOC_SAND_BITS_MASK(SOC_DPP_VRF_NOF_BITS_PETRAB - 1, 0);
    soc_sand_bitstream_set_any_field(&key->vrf_ndx, 128, SOC_DPP_VRF_NOF_BITS_PETRAB, entry->value);
    soc_sand_bitstream_set_any_field(&mask,         128, SOC_DPP_VRF_NOF_BITS_PETRAB, entry->mask);
    break;

  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
    soc_pb_pp_ipv6_prefix_to_mask(key->key.ipv6_vpn.subnet.prefix_len, ipv6_mask);
    soc_sand_bitstream_set_any_field(key->key.ipv6_vpn.subnet.ipv6_address.address, 0, 128, entry->value);
    soc_sand_bitstream_set_any_field(ipv6_mask,                                     0, 128, entry->mask);

    mask = SOC_SAND_BITS_MASK(SOC_DPP_VRF_NOF_BITS_PETRAB - 1, 0);
    soc_sand_bitstream_set_any_field(&key->vrf_ndx, 128, SOC_DPP_VRF_NOF_BITS_PETRAB, entry->value);
    soc_sand_bitstream_set_any_field(&mask,         128, SOC_DPP_VRF_NOF_BITS_PETRAB, entry->mask);
    break;

  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC:
    soc_sand_bitstream_set_any_field(key->key.ipv6_mc.group.address, 0, 120, entry->value);
    soc_sand_bitstream_set_bit_range(entry->mask, 0, 119);

    soc_sand_bitstream_set_any_field(&key->key.ipv6_mc.inrif.val,  120, 8, entry->value);
    soc_sand_bitstream_set_any_field(&key->key.ipv6_mc.inrif.mask, 120, 8, entry->mask);
    break;
  default:
    break;
  }
}

STATIC
  uint32
    soc_pb_pp_frwrd_ip_tcam_ip_db_id_get(
      SOC_SAND_IN SOC_PB_IP_TCAM_ENTRY_TYPE entry_type
    )
{
  uint32
    tcam_db_id;

  switch (entry_type)
  {
  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV4_MC:
    tcam_db_id = SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV4_MC;
    break;

  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_UC:
  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN:
    tcam_db_id = SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_UC;
    break;

  case SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_MC:
    tcam_db_id = SOC_PB_PP_FRWRD_IP_TCAM_ID_IPV6_MC;
    break;

  default:
    tcam_db_id = SOC_PB_TCAM_MAX_NOF_LISTS;
    break;
  }

  return tcam_db_id;
}

uint32
  soc_pb_pp_frwrd_ip_tcam_route_add_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  uint32                                      action,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint16
	  priority;
  uint8
	  stream[SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES];
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  uint8
	  found,
    entry_added;
  uint32
    tcam_db_id;
  SOC_PB_TCAM_ENTRY
    entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_ADD_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(success);

  *success = SOC_SAND_SUCCESS;

  /*
   *  Search for the route_key in the route_key -> entry_id hash table
   */
  hash_tbl = soc_pb_sw_db_frwrd_ip_route_key_to_entry_id_get(
               unit
             );
  res = soc_pb_pp_ip_tcam_key_to_stream(
	        route_key,
	        stream
	      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_tbl,
          stream,
          &data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!found)
  {
    /*
     *  Insert the rule to the route_key -> entry_id table
     */
    res = soc_sand_hash_table_entry_add(
            unit,
            hash_tbl,
            stream,
            &data_indx,
            &entry_added
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    entry_added = TRUE;
  }

  if (entry_added)
  {
    /*
     *  Remove the old entry, if applicable
     */
    tcam_db_id = soc_pb_pp_frwrd_ip_tcam_ip_db_id_get(
                   route_key->type
                 );
    res = soc_pb_tcam_db_entry_remove_unsafe(
            unit,
            tcam_db_id,
            data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

    /*
     *  Insert the new rule to the TCAM
     */
    priority = soc_pb_pp_ip_tcam_route_to_prio(
                 unit,
                 route_key
               );
    soc_pb_pp_ip_tcam_entry_build(
      route_key,
      &entry
    );
    res = soc_pb_tcam_managed_db_entry_add_unsafe(
            unit,
            tcam_db_id,
            data_indx,
            priority,
            &entry,
            action,
            success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    if (*success != SOC_SAND_SUCCESS)
    {
      res = soc_sand_hash_table_entry_remove_by_index(
              unit,
              hash_tbl,
              data_indx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }
  else
  {
    *success = SOC_SAND_FAILURE_OUT_OF_RESOURCES;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ip_tcam_route_add_unsafe()", 0, 0);
}


uint32
  soc_pb_pp_frwrd_ip_tcam_route_get_unsafe(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY *route_key,
    SOC_SAND_IN  uint8               exact_match,
    SOC_SAND_OUT uint32                *action,
    SOC_SAND_OUT uint8               *found
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint8
	  stream[SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES];
  uint32
    entry_id;
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  uint32
    tcam_db_id;
  uint16
    priority;
  SOC_PB_TCAM_ENTRY
    entry;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *  Determine the database ID
   */
  tcam_db_id = soc_pb_pp_frwrd_ip_tcam_ip_db_id_get(
                 route_key->type
               );

  if (exact_match)
  {
    /*
     *  Search for the route_key in the route_key -> entry_id hash table
     */
    hash_tbl = soc_pb_sw_db_frwrd_ip_route_key_to_entry_id_get(
                 unit
               );
    res = soc_pb_pp_ip_tcam_key_to_stream(
	          route_key,
	          stream
	        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    res = soc_sand_hash_table_entry_lookup(
            unit,
            hash_tbl,
            stream,
            &data_indx,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    entry_id = data_indx;
  }
  else
  {
    /*
     *  Search the TCAM for the first match
     */
    soc_pb_pp_ip_tcam_entry_build(
      route_key,
      &entry
    );

    res = soc_pb_tcam_db_entry_search_unsafe(
            unit,
            tcam_db_id,
            &entry,
            &entry_id,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (*found)
  {
    res = soc_pb_tcam_db_entry_get_unsafe(
            unit,
            tcam_db_id,
            entry_id,
            &priority,
            &entry,
            action,
            found
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ip_tcam_route_get_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_frwrd_ip_tcam_route_get_block_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_INOUT  SOC_PB_PP_IP_ROUTING_TABLE_RANGE               *block_range_key,
    SOC_SAND_OUT  SOC_PB_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_OUT uint32                                      *fec_id,
    SOC_SAND_OUT uint32                                      *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK,
    arr_indx,
    data_indx;
  uint32
    nof_scanned,
    nof_retrieved,
    tcam_db_id;
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  SOC_SAND_HASH_TABLE_ITER
    iter;
  uint8
    stream[SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES];
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;
  uint16
    priority;
  SOC_PB_TCAM_ENTRY
    entry;
  uint8
    found;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_GET_BLOCK_UNSAFE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(block_range_key);
  SOC_SAND_CHECK_NULL_INPUT(route_key);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  hash_tbl = soc_pb_sw_db_frwrd_ip_route_key_to_entry_id_get(
               unit
             );

  iter = block_range_key->start.payload.arr[0];
  res = soc_sand_hash_table_get_next(
          unit,
          hash_tbl,
          &iter,
          stream,
          &data_indx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  nof_scanned   = 0;
  nof_retrieved = 0;
  arr_indx      = 0;
  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter)
          && (nof_scanned < block_range_key->entries_to_scan)
          && (nof_retrieved < block_range_key->entries_to_act))
  {
    ++nof_scanned;
    soc_pb_pp_ip_tcam_stream_to_key(
      stream,
      &key
    );
    if (key.type == route_key[arr_indx].type)
    {
      if (key.type != SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN || key.vrf_ndx == route_key[arr_indx].vrf_ndx)
      {
        route_key[arr_indx] = key;
        tcam_db_id = soc_pb_pp_frwrd_ip_tcam_ip_db_id_get(
                       key.type
                     );
        res = soc_pb_tcam_db_entry_get_unsafe(
                unit,
                tcam_db_id,
                data_indx,
                &priority,
                &entry,
                &fec_id[arr_indx],
                &found
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

        /*
         *  If found == FALSE we should indicate an internal error
         */

        ++nof_retrieved;
        ++arr_indx;
      }
    }
    res = soc_sand_hash_table_get_next(
            unit,
            hash_tbl,
            &iter,
            stream,
            &data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  *nof_entries = arr_indx;
  
  block_range_key->entries_to_act = nof_retrieved;
  block_range_key->entries_to_scan = nof_scanned;

  if(SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
  {
    block_range_key->start.payload.arr[0] = SOC_SAND_UINT_MAX;
    block_range_key->start.payload.arr[1] = SOC_SAND_UINT_MAX;
  }
  else
  {
    block_range_key->start.payload.arr[0] = iter;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_frwrd_ip_route_get_block_unsafe()",0,0);
}

uint32
  soc_pb_pp_frwrd_ip_tcam_route_remove_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY                       *route_key
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  uint8
    found;
  uint8
	  stream[SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES];
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  uint32
    tcam_db_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   *  Search for the route_key in the route_key -> entry_id hash table
   */
  hash_tbl = soc_pb_sw_db_frwrd_ip_route_key_to_entry_id_get(
               unit
             );
  res = soc_pb_pp_ip_tcam_key_to_stream(
	        route_key,
	        stream
	      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_sand_hash_table_entry_lookup(
          unit,
          hash_tbl,
          stream,
          &data_indx,
          &found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (!found)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_FRWRD_IP_TCAM_ENTRY_DOESNT_EXIST_ERR, 30, exit);
  }

  res = soc_sand_hash_table_entry_remove_by_index(
          unit,
          hash_tbl,
          data_indx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   *  Determine the database ID
   */
  tcam_db_id = soc_pb_pp_frwrd_ip_tcam_ip_db_id_get(
                 route_key->type
               );

  res = soc_pb_tcam_managed_db_entry_remove_unsafe(
          unit,
          tcam_db_id,
          data_indx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ip_tcam_route_remove_unsafe()", 0, 0);
}


 
uint32
  soc_pb_pp_frwrd_ip_tcam_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  SOC_PB_PP_IP_TCAM_ENTRY_KEY                       *route_key,
    SOC_SAND_IN  uint8                                    clear_all_vrf
  )
{
  uint32
    res = SOC_SAND_OK,
    data_indx;
  SOC_SAND_HASH_TABLE_INFO
    *hash_tbl;
  SOC_SAND_HASH_TABLE_ITER
    iter;
  uint8
    stream[SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES];
  SOC_PB_PP_IP_TCAM_ENTRY_KEY
    key;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  hash_tbl = soc_pb_sw_db_frwrd_ip_route_key_to_entry_id_get(
               unit
             );

  SOC_SAND_HASH_TABLE_ITER_SET_BEGIN(&iter);
  res = soc_sand_hash_table_get_next(
          unit,
          hash_tbl,
          &iter,
          stream,
          &data_indx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  while (!SOC_SAND_HASH_TABLE_ITER_IS_END(&iter))
  {
    soc_pb_pp_ip_tcam_stream_to_key(
      stream,
      &key
    );
    if (key.type == route_key->type)
    {
      if (route_key->type != SOC_PB_IP_TCAM_ENTRY_TYPE_IPV6_VPN
           || clear_all_vrf || key.vrf_ndx == route_key->vrf_ndx)
      {
        res = soc_pb_pp_frwrd_ip_tcam_route_remove_unsafe(
                unit,
                &key
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
    }

    res = soc_sand_hash_table_get_next(
            unit,
            hash_tbl,
            &iter,
            stream,
            &data_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_frwrd_ip_tcam_routing_table_clear_unsafe()", 0, 0);
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
