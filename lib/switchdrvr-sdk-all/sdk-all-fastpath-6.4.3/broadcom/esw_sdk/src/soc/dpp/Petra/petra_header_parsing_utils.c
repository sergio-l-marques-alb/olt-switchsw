/* $Id: petra_header_parsing_utils.c,v 1.7 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_header_parsing_utils.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_header_parsing_utils.c,v 1.7 Broadcom SDK $
 *  ITMH-Base
 */
#define SOC_PETRA_HPU_ITMH_PP_HEADER_PRESENT_LSB  30
#define SOC_PETRA_HPU_ITMH_PP_HEADER_PRESENT_MSB  30

#define SOC_PETRA_HPU_ITMH_OUT_MIRROR_EN_LSB      29
#define SOC_PETRA_HPU_ITMH_OUT_MIRROR_EN_MSB      29

#define SOC_PETRA_HPU_ITMH_SNOOP_CMD_NDX_LSB      24
#define SOC_PETRA_HPU_ITMH_SNOOP_CMD_NDX_MSB      27

#define SOC_PETRA_HPU_ITMH_EXCLUDE_SRC_LSB        23
#define SOC_PETRA_HPU_ITMH_EXCLUDE_SRC_MSB        23

#define SOC_PETRA_HPU_ITMH_TR_CLS_LSB             20
#define SOC_PETRA_HPU_ITMH_TR_CLS_MSB             22

#define SOC_PETRA_HPU_ITMH_DP_LSB                 18
#define SOC_PETRA_HPU_ITMH_DP_MSB                 19

/*
 *  ITMH-Extension-source-port
 */
#define SOC_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_LSB    13
#define SOC_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_MSB    13

#define SOC_PETRA_HPU_ITMH_EXT_SRC_PORT_LSB       0
#define SOC_PETRA_HPU_ITMH_EXT_SRC_PORT_MSB       12

/*
 *  FTMH-Base
 */
#define SOC_PETRA_HPU_FTMH_PKT_SIZE_LSB           (32 - SOC_SAND_REG_SIZE_BITS)
#define SOC_PETRA_HPU_FTMH_PKT_SIZE_MSB           (45 - SOC_SAND_REG_SIZE_BITS)

#define SOC_PETRA_HPU_FTMH_TR_CLS_LSB             29
#define SOC_PETRA_HPU_FTMH_TR_CLS_MSB             31

#define SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_TYPE_LSB  28
#define SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_TYPE_MSB  28

#define SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LSB       16
#define SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_MSB       27

#define SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_MEMBER_LSB       24
#define SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_MEMBER_MSB       27

#define SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_LSB       16
#define SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_MSB       23

#define SOC_PETRA_HPU_FTMH_OFP_LSB                8
#define SOC_PETRA_HPU_FTMH_OFP_MSB                15

#define SOC_PETRA_HPU_FTMH_DP_LSB                 6
#define SOC_PETRA_HPU_FTMH_DP_MSB                 7

#define SOC_PETRA_HPU_FTMH_SIGNATURE_LSB          4
#define SOC_PETRA_HPU_FTMH_SIGNATURE_MSB          5

#define SOC_PETRA_HPU_FTMH_PP_HEADER_PRESENT_LSB  3
#define SOC_PETRA_HPU_FTMH_PP_HEADER_PRESENT_MSB  3

#define SOC_PETRA_HPU_FTMH_OUT_MIRROR_EN_LSB      2
#define SOC_PETRA_HPU_FTMH_OUT_MIRROR_EN_MSB      2

#define SOC_PETRA_HPU_FTMH_EXCLUDE_SRC_LSB        1
#define SOC_PETRA_HPU_FTMH_EXCLUDE_SRC_MSB        1

#define SOC_PETRA_HPU_FTMH_SYS_MC_LSB             0
#define SOC_PETRA_HPU_FTMH_SYS_MC_MSB             0

/*
 *  FTMH-Extension-Outlif
 */
#define SOC_PETRA_HPU_FTMH_EXT_OUTLIF_LSB         0
#define SOC_PETRA_HPU_FTMH_EXT_OUTLIF_MSB         15

/*
 *  OTMH-Base
 */
#define SOC_PETRA_HPU_OTMH_PP_HEADER_PRESENT_LSB  14
#define SOC_PETRA_HPU_OTMH_PP_HEADER_PRESENT_MSB  14

#define SOC_PETRA_HPU_OTMH_SIGNATURE_LSB          12
#define SOC_PETRA_HPU_OTMH_SIGNATURE_MSB          13

#define SOC_PETRA_HPU_OTMH_SYS_MC_LSB             11
#define SOC_PETRA_HPU_OTMH_SYS_MC_MSB             11

#define SOC_PETRA_HPU_OTMH_DP_LSB                 8
#define SOC_PETRA_HPU_OTMH_DP_MSB                 9

#define SOC_PETRA_HPU_OTMH_TR_CLS_LSB             4
#define SOC_PETRA_HPU_OTMH_TR_CLS_MSB             6

/*
 *  OTMH-Extension-Outlif
 */
#define SOC_PETRA_HPU_OTMH_EXT_OUTLIF_LSB         0
#define SOC_PETRA_HPU_OTMH_EXT_OUTLIF_MSB         15

/*
 *  OTMH-Extension-Source Port
 */
#define SOC_PETRA_HPU_OTMH_SRC_PORT_LSB           0
#define SOC_PETRA_HPU_OTMH_SRC_PORT_MSB           11
#define SOC_PETRA_HPU_OTMH_SRC_SYS_PORT_TYPE_LSB  12
#define SOC_PETRA_HPU_OTMH_SRC_SYS_PORT_TYPE_MSB  12
#define SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_MEM_MSB   11
#define SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_MEM_LSB   8

/*
 *  OTMH-Extension-Destination Port
 */
#define SOC_PETRA_HPU_OTMH_EXT_DEST_PORT_LSB      0
#define SOC_PETRA_HPU_OTMH_EXT_DEST_PORT_MSB      15

/* } */

/*************
 *  MACROS   *
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

/*********************************************************************
*     This function receives the fields that construct the
*     Incoming TM Header and builds a ITMH header (base +
*     extension).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_itmh_build_verify(
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH            *info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_HPU_ITMH_BUILD_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_drop_precedence_verify(info->base.dp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_traffic_class_verify(info->base.tr_cls);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->base.snoop_cmd_ndx, SOC_PETRA_MAX_SNOOP_COMMAND_INDEX,
    SOC_PETRA_SNOOP_COMMAND_INDEX_OUT_OF_RANGE_ERR, 30, exit
  );

  res = soc_petra_dest_info_verify(
          &(info->base.destination)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (info->extension.enable == TRUE)
  {
    if (info->extension.is_src_sys_port == TRUE)
    {
      res = soc_petra_dest_sys_port_info_verify(&(info->extension.src_port));
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
    else
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        info->extension.src_port.id, SOC_PETRA_MAX_SYSTEM_PORT_ID,
        SOC_PETRA_SYSTEM_PORT_OUT_OF_RANGE_ERR, 60, exit
      );
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_hpu_itmh_build_verify()",0,0);
}

/*********************************************************************
*     This function receives the fields that construct the
*     Incoming TM Header and builds a 32-bit (or 48-bit with
*     extension) stream header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_itmh_build(
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH            *info,
    SOC_SAND_OUT SOC_PETRA_HPU_ITMH_HDR        *itmh
  )
{
  uint32
    res,
    sys_port,
    dest;
  uint32
    itmh_hdr_base = 0x0,
    itmh_hdr_ext_src_port = 0x0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_HPU_ITMH_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(itmh);

  res = soc_petra_hpu_itmh_build_verify(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10,exit);

  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.pp_header_present,
      SOC_PETRA_HPU_ITMH_PP_HEADER_PRESENT_MSB,
      SOC_PETRA_HPU_ITMH_PP_HEADER_PRESENT_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.out_mirror_dis,
      SOC_PETRA_HPU_ITMH_OUT_MIRROR_EN_MSB,
      SOC_PETRA_HPU_ITMH_OUT_MIRROR_EN_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.snoop_cmd_ndx,
      SOC_PETRA_HPU_ITMH_SNOOP_CMD_NDX_MSB,
      SOC_PETRA_HPU_ITMH_SNOOP_CMD_NDX_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.exclude_src,
      SOC_PETRA_HPU_ITMH_EXCLUDE_SRC_MSB,
      SOC_PETRA_HPU_ITMH_EXCLUDE_SRC_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.tr_cls,
      SOC_PETRA_HPU_ITMH_TR_CLS_MSB,
      SOC_PETRA_HPU_ITMH_TR_CLS_LSB
    );
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.dp,
      SOC_PETRA_HPU_ITMH_DP_MSB,
      SOC_PETRA_HPU_ITMH_DP_LSB
    );

  switch(info->base.destination.type)
  {
  case SOC_PETRA_DEST_TYPE_QUEUE:
    dest = info->base.destination.id | ( 3 << 16);
    break;
  case SOC_PETRA_DEST_TYPE_MULTICAST:
    dest = info->base.destination.id | ( 2 << 16);
    break;
  case SOC_PETRA_DEST_TYPE_SYS_PHY_PORT:
    soc_petra_ports_logical_sys_id_build(FALSE, 0, 0, info->base.destination.id, &dest);
    break;
  case SOC_PETRA_DEST_TYPE_LAG:
    soc_petra_ports_logical_sys_id_build(TRUE, info->base.destination.id, 0, 0, &dest);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
    break;
  }
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      dest,
      SOC_PETRA_HPU_ITMH_DESTINATION_MSB,
      SOC_PETRA_HPU_ITMH_DESTINATION_LSB
    );

  if (info->extension.enable == TRUE)
  {
    itmh_hdr_ext_src_port |=
      SOC_SAND_SET_BITS_RANGE(
        info->extension.is_src_sys_port,
        SOC_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_MSB,
        SOC_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_LSB
      );

    if (info->extension.src_port.type == SOC_PETRA_DEST_SYS_PORT_TYPE_LAG)
    {
      res = soc_petra_ports_logical_sys_id_build(
              TRUE,
              info->extension.src_port.id,
              info->extension.src_port.member_id,
              0,
              &sys_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    else  /* destination-type == sys_phy_port*/
    {
      res = soc_petra_ports_logical_sys_id_build(
              FALSE,
              0,
              0,
              info->extension.src_port.id,
              &sys_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }

    itmh_hdr_ext_src_port |=
      SOC_SAND_SET_BITS_RANGE(
        sys_port,
        SOC_PETRA_HPU_ITMH_EXT_SRC_PORT_MSB,
        SOC_PETRA_HPU_ITMH_EXT_SRC_PORT_LSB
      );
  }

  itmh->extention_src_port = itmh_hdr_ext_src_port;
  itmh->base               = itmh_hdr_base;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_hpu_itmh_build()",0,0);
}

/*********************************************************************
*     This function retrieves the fields that construct the
*     Incoming TM Header given an ITMH header (base +
*     extension).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_itmh_parse(
    SOC_SAND_IN  SOC_PETRA_HPU_ITMH_HDR        *itmh,
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH            *info
  )
{
  uint32
    res,
    itmh_hdr_base,
    itmh_hdr_ext_src_port,
    src_port,
    dest,
    lag_id,
    lag_member_id,
    sys_phys_port_id;
  uint8
    is_lag_not_phys;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_HPU_ITMH_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(itmh);
  SOC_SAND_CHECK_NULL_INPUT(info);

  itmh_hdr_base = itmh->base;
  itmh_hdr_ext_src_port = itmh->extention_src_port;

  info->base.pp_header_present =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_base,
        SOC_PETRA_HPU_ITMH_PP_HEADER_PRESENT_MSB,
        SOC_PETRA_HPU_ITMH_PP_HEADER_PRESENT_LSB
      )
    );
  info->base.out_mirror_dis =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_base,
        SOC_PETRA_HPU_ITMH_OUT_MIRROR_EN_MSB,
        SOC_PETRA_HPU_ITMH_OUT_MIRROR_EN_LSB
      )
    );
  info->base.snoop_cmd_ndx =
    SOC_SAND_GET_BITS_RANGE(
      itmh_hdr_base,
      SOC_PETRA_HPU_ITMH_SNOOP_CMD_NDX_MSB,
      SOC_PETRA_HPU_ITMH_SNOOP_CMD_NDX_LSB
    );
  info->base.exclude_src =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_base,
        SOC_PETRA_HPU_ITMH_EXCLUDE_SRC_MSB,
        SOC_PETRA_HPU_ITMH_EXCLUDE_SRC_LSB
      )
    );
  info->base.tr_cls =
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_base,
        SOC_PETRA_HPU_ITMH_TR_CLS_MSB,
        SOC_PETRA_HPU_ITMH_TR_CLS_LSB
      );
  info->base.dp =
    SOC_SAND_GET_BITS_RANGE(
      itmh_hdr_base,
      SOC_PETRA_HPU_ITMH_DP_MSB,
      SOC_PETRA_HPU_ITMH_DP_LSB
    );

  dest =
    SOC_SAND_GET_BITS_RANGE(
      itmh_hdr_base,
      SOC_PETRA_HPU_ITMH_DESTINATION_MSB,
      SOC_PETRA_HPU_ITMH_DESTINATION_LSB
    );
  info->base.destination.id = SOC_SAND_GET_BITS_RANGE(dest,15,0);
  switch(dest >> 16)
  {
  case 3:
    info->base.destination.type = SOC_PETRA_DEST_TYPE_QUEUE;
    break;
  case 2:
    info->base.destination.type = SOC_PETRA_DEST_TYPE_MULTICAST;
    break;
  default:
    res = soc_petra_ports_logical_sys_id_parse(
      info->base.destination.id, &is_lag_not_phys, &lag_id, &lag_member_id, &sys_phys_port_id);
    if(is_lag_not_phys)
    {
      info->base.destination.type = SOC_PETRA_DEST_TYPE_LAG;
      info->base.destination.id = lag_id;
    }
    else
    {
      info->base.destination.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;
      info->base.destination.id = sys_phys_port_id;
    }
    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
    break;
  }
  itmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
    dest,
    SOC_PETRA_HPU_ITMH_DESTINATION_MSB,
    SOC_PETRA_HPU_ITMH_DESTINATION_LSB
    );


  info->extension.is_src_sys_port =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        itmh_hdr_ext_src_port,
        SOC_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_MSB,
        SOC_PETRA_HPU_ITMH_EXT_IS_SYS_PORT_LSB
      )
    );

  src_port =
    SOC_SAND_GET_BITS_RANGE(
      itmh_hdr_ext_src_port,
      SOC_PETRA_HPU_ITMH_EXT_SRC_PORT_MSB,
      SOC_PETRA_HPU_ITMH_EXT_SRC_PORT_LSB
    );

   res = soc_petra_ports_logical_sys_id_parse(
           src_port,
           &is_lag_not_phys,
           &lag_id,
           &lag_member_id,
           &sys_phys_port_id
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

   if (is_lag_not_phys)
   {
     info->extension.src_port.type = SOC_PETRA_DEST_SYS_PORT_TYPE_LAG;
     info->extension.src_port.id = lag_id;
     info->extension.src_port.member_id = lag_member_id;
   }
   else /* type == sys_phy_port*/
   {
     info->extension.src_port.type = SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
     info->extension.src_port.id = sys_phys_port_id;
     info->extension.src_port.member_id = 0;
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_hpu_itmh_parse()",0,0);
}

/*********************************************************************
*     This function receives the fields that construct the
*     Fabric TM Header and builds a 48-bit (or 64-bit with
*     extension) stream header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_ftmh_build_verify(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH            *info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_HPU_FTMH_BUILD_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_drop_precedence_verify(info->base.dp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_traffic_class_verify(info->base.tr_cls);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_fap_port_id_verify(SOC_PETRA_DEVICE_ID_IRRELEVANT, info->base.ofp, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->base.signature, SOC_PETRA_MAX_SIGNATURE,
    SOC_PETRA_SIGNATURE_OUT_OF_RANGE_ERR, 40, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->base.packet_size, SOC_PETRA_MAX_PACKET_BYTES,
    SOC_PETRA_PACKET_SIZE_OUT_OF_RANGE_ERR, 50, exit
  );

  /*
  res = soc_petra_dest_sys_port_info_verify(&(info->base.src_sys_port));
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);*/
  

  if (info->extension.enable == TRUE)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->extension.outlif, SOC_PETRA_MAX_CUD_ID,
      SOC_PETRA_CUD_ID_OUT_OF_RANGE_ERR, 60, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_hpu_ftmh_build_verify()",0,0);
}

/*********************************************************************
*     This function receives the fields that construct the
*     Fabric TM Header and builds a FTMH header (base +
*     extension).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_ftmh_build(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH            *info,
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH_HDR        *ftmh
  )
{
  uint32
    res;
  uint32
    ftmh_hdr_base[SOC_PETRA_HPU_FTMH_SIZE_UINT32S],
    ftmh_hdr_ext_outlif = 0x0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_HPU_FTMH_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(ftmh);

  SOC_PETRA_CLEAR(&(ftmh_hdr_base[0]), uint32, SOC_PETRA_HPU_FTMH_SIZE_UINT32S);

  res = soc_petra_hpu_ftmh_build_verify(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10,exit);

  ftmh_hdr_base[0] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.multicast,
      SOC_PETRA_HPU_FTMH_SYS_MC_MSB,
      SOC_PETRA_HPU_FTMH_SYS_MC_LSB
    );
  ftmh_hdr_base[0] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.exclude_src,
      SOC_PETRA_HPU_FTMH_EXCLUDE_SRC_MSB,
      SOC_PETRA_HPU_FTMH_EXCLUDE_SRC_LSB
    );
  ftmh_hdr_base[0] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.out_mirror_dis,
      SOC_PETRA_HPU_FTMH_OUT_MIRROR_EN_MSB,
      SOC_PETRA_HPU_FTMH_OUT_MIRROR_EN_LSB
    );
  ftmh_hdr_base[0] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.pp_header_present,
      SOC_PETRA_HPU_FTMH_PP_HEADER_PRESENT_MSB,
      SOC_PETRA_HPU_FTMH_PP_HEADER_PRESENT_LSB
    );
  ftmh_hdr_base[0] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.signature,
      SOC_PETRA_HPU_FTMH_SIGNATURE_MSB,
      SOC_PETRA_HPU_FTMH_SIGNATURE_LSB
    );
  ftmh_hdr_base[0] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.dp,
      SOC_PETRA_HPU_FTMH_DP_MSB,
      SOC_PETRA_HPU_FTMH_DP_LSB
    );
  ftmh_hdr_base[0] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.ofp,
      SOC_PETRA_HPU_FTMH_OFP_MSB,
      SOC_PETRA_HPU_FTMH_OFP_LSB
    );
  ftmh_hdr_base[0] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.src_sys_port.type,
      SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_TYPE_MSB,
      SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_TYPE_LSB
    );

  if(info->base.src_sys_port.type == SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT)
  {
    ftmh_hdr_base[0] |=
      SOC_SAND_SET_BITS_RANGE(
        info->base.src_sys_port.id,
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_MSB,
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LSB
      );
  }
  else
  {
    /* LAG */
    ftmh_hdr_base[0] |=
      SOC_SAND_SET_BITS_RANGE(
        info->base.src_sys_port.member_id,
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_MEMBER_MSB,
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_MEMBER_LSB
      );

    ftmh_hdr_base[0] |=
      SOC_SAND_SET_BITS_RANGE(
        info->base.src_sys_port.id,
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_MSB,
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_LSB
      );
  }
  ftmh_hdr_base[0] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.tr_cls,
      SOC_PETRA_HPU_FTMH_TR_CLS_MSB,
      SOC_PETRA_HPU_FTMH_TR_CLS_LSB
    );

  ftmh_hdr_base[1] |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.packet_size,
      SOC_PETRA_HPU_FTMH_PKT_SIZE_MSB,
      SOC_PETRA_HPU_FTMH_PKT_SIZE_LSB
    );

  if (info->extension.enable == TRUE)
  {
    ftmh_hdr_ext_outlif |=
      SOC_SAND_SET_BITS_RANGE(
        info->extension.outlif,
        SOC_PETRA_HPU_FTMH_EXT_OUTLIF_MSB,
        SOC_PETRA_HPU_FTMH_EXT_OUTLIF_LSB
      );
  }

  ftmh->extension_outlif = ftmh_hdr_ext_outlif;
  SOC_PETRA_COPY(&(ftmh->base[0]), &(ftmh_hdr_base[0]), uint32, SOC_PETRA_HPU_FTMH_SIZE_UINT32S);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_hpu_ftmh_build()",0,0);
}

/*********************************************************************
*     This function retrieves the fields that construct the
*     Fabric TM Header given an FTMH header (base +
*     extension).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_ftmh_parse(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH_HDR        *ftmh,
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH            *info
  )
{
  uint32
    res,
    ftmh_hdr_base[SOC_PETRA_HPU_FTMH_SIZE_UINT32S],
    ftmh_hdr_ext_outlif;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_HPU_FTMH_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(ftmh);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_COPY(&(ftmh_hdr_base[0]), &(ftmh->base[0]), uint32, SOC_PETRA_HPU_FTMH_SIZE_UINT32S);
  ftmh_hdr_ext_outlif = ftmh->extension_outlif;

  info->base.multicast =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        ftmh_hdr_base[0],
        SOC_PETRA_HPU_FTMH_SYS_MC_MSB,
        SOC_PETRA_HPU_FTMH_SYS_MC_LSB
      )
    );
  info->base.exclude_src =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        ftmh_hdr_base[0],
        SOC_PETRA_HPU_FTMH_EXCLUDE_SRC_MSB,
        SOC_PETRA_HPU_FTMH_EXCLUDE_SRC_LSB
      )
    );
  info->base.out_mirror_dis =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        ftmh_hdr_base[0],
        SOC_PETRA_HPU_FTMH_OUT_MIRROR_EN_MSB,
        SOC_PETRA_HPU_FTMH_OUT_MIRROR_EN_LSB
      )
    );
  info->base.pp_header_present =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        ftmh_hdr_base[0],
        SOC_PETRA_HPU_FTMH_PP_HEADER_PRESENT_MSB,
        SOC_PETRA_HPU_FTMH_PP_HEADER_PRESENT_LSB
      )
    );
  info->base.signature =
    SOC_SAND_GET_BITS_RANGE(
      ftmh_hdr_base[0],
      SOC_PETRA_HPU_FTMH_SIGNATURE_MSB,
      SOC_PETRA_HPU_FTMH_SIGNATURE_LSB
    );
  info->base.dp =
    SOC_SAND_GET_BITS_RANGE(
      ftmh_hdr_base[0],
      SOC_PETRA_HPU_FTMH_DP_MSB,
      SOC_PETRA_HPU_FTMH_DP_LSB
    );
  info->base.ofp =
    SOC_SAND_GET_BITS_RANGE(
      ftmh_hdr_base[0],
      SOC_PETRA_HPU_FTMH_OFP_MSB,
      SOC_PETRA_HPU_FTMH_OFP_LSB
    );
  info->base.src_sys_port.type =
    SOC_SAND_GET_BITS_RANGE(
      ftmh_hdr_base[0],
      SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_TYPE_MSB,
      SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_TYPE_LSB
    );
  if(info->base.src_sys_port.type == SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT)
  {
    info->base.src_sys_port.id =
      SOC_SAND_GET_BITS_RANGE(
        ftmh_hdr_base[0],
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_MSB,
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LSB
      );
    info->base.src_sys_port.member_id = 0;
  }
  else
  {
    /* LAG */
    info->base.src_sys_port.member_id =
      SOC_SAND_GET_BITS_RANGE(
        ftmh_hdr_base[0],
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_MEMBER_MSB,
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_MEMBER_LSB
      );

    info->base.src_sys_port.id =
      SOC_SAND_GET_BITS_RANGE(
        ftmh_hdr_base[0],
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_MSB,
        SOC_PETRA_HPU_FTMH_SRC_SYS_PORT_LAG_LSB
      );
  }
  info->base.tr_cls =
    SOC_SAND_GET_BITS_RANGE(
      ftmh_hdr_base[0],
      SOC_PETRA_HPU_FTMH_TR_CLS_MSB,
      SOC_PETRA_HPU_FTMH_TR_CLS_LSB
    );
  info->base.packet_size =
    SOC_SAND_GET_BITS_RANGE(
      ftmh_hdr_base[1],
      SOC_PETRA_HPU_FTMH_PKT_SIZE_MSB,
      SOC_PETRA_HPU_FTMH_PKT_SIZE_LSB
    );

  info->extension.outlif =
    SOC_SAND_GET_BITS_RANGE(
      ftmh_hdr_ext_outlif,
      SOC_PETRA_HPU_FTMH_EXT_OUTLIF_MSB,
      SOC_PETRA_HPU_FTMH_EXT_OUTLIF_LSB
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_hpu_ftmh_parse()",0,0);
}

/*********************************************************************
*     This function receives the fields that construct the
*     Outgoing TM Header and builds a 16-bit (or 32/48/64-bit
*     with extensions) stream header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_otmh_build_verify(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH            *info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_HPU_OTMH_BUILD_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_drop_precedence_verify(info->base.dp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_traffic_class_verify(info->base.tr_cls);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->base.signature, SOC_PETRA_MAX_SIGNATURE,
    SOC_PETRA_SIGNATURE_OUT_OF_RANGE_ERR, 40, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->base.packet_size, SOC_PETRA_MAX_PACKET_BYTES,
    SOC_PETRA_PACKET_SIZE_OUT_OF_RANGE_ERR, 50, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->base.outlif, SOC_PETRA_MAX_CUD_ID,
    SOC_PETRA_CUD_ID_OUT_OF_RANGE_ERR, 60, exit
    );

  if (info->extension.outlif.enable == TRUE)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
        info->extension.outlif.outlif, SOC_PETRA_MAX_CUD_ID,
        SOC_PETRA_CUD_ID_OUT_OF_RANGE_ERR, 70, exit
      );
  }
  if (info->extension.src.enable == TRUE)
  {
    res = soc_petra_dest_sys_port_info_verify(&(info->extension.src.src_sys_port));
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  }
  if (info->extension.dest.enable == TRUE)
  {
    res = soc_petra_dest_sys_port_info_verify(&(info->extension.dest.dest_sys_port));
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_hpu_otmh_build_verify()",0,0);
}

/*********************************************************************
*     This function receives the fields that construct the
*     Outgoing TM Header and builds a OTMH header (base +
*     extensions).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_otmh_build(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH            *info,
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_HDR        *otmh
  )
{
  uint32
    res;
  uint32
    otmh_hdr_base = 0x0,
    otmh_hdr_ext_outlif = 0x0,
    otmh_hdr_ext_src_port = 0x0,
    otmh_hdr_ext_dest_port = 0x0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_HPU_OTMH_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(otmh);

  res = soc_petra_hpu_otmh_build_verify(info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10,exit);

  otmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.pp_header_present,
      SOC_PETRA_HPU_OTMH_PP_HEADER_PRESENT_MSB,
      SOC_PETRA_HPU_OTMH_PP_HEADER_PRESENT_LSB
    );

  otmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.signature,
      SOC_PETRA_HPU_OTMH_SIGNATURE_MSB,
      SOC_PETRA_HPU_OTMH_SIGNATURE_LSB
    );

  otmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.multicast,
      SOC_PETRA_HPU_OTMH_SYS_MC_MSB,
      SOC_PETRA_HPU_OTMH_SYS_MC_LSB
    );

  otmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.dp,
      SOC_PETRA_HPU_OTMH_DP_MSB,
      SOC_PETRA_HPU_OTMH_DP_LSB
    );

  otmh_hdr_base |=
    SOC_SAND_SET_BITS_RANGE(
      info->base.tr_cls,
      SOC_PETRA_HPU_OTMH_TR_CLS_MSB,
      SOC_PETRA_HPU_OTMH_TR_CLS_LSB
    );

  if (info->extension.outlif.enable == TRUE)
  {
    otmh_hdr_ext_outlif |=
      SOC_SAND_SET_BITS_RANGE(
        info->extension.outlif.outlif,
        SOC_PETRA_HPU_OTMH_EXT_OUTLIF_MSB,
        SOC_PETRA_HPU_OTMH_EXT_OUTLIF_LSB
        );
  }

  if (info->extension.src.enable == TRUE)
  {
    otmh_hdr_ext_src_port |=
      SOC_SAND_SET_BITS_RANGE(
        info->extension.src.src_sys_port.type,
        SOC_PETRA_HPU_OTMH_SRC_SYS_PORT_TYPE_MSB,
        SOC_PETRA_HPU_OTMH_SRC_SYS_PORT_TYPE_LSB
      );

    if (info->extension.src.src_sys_port.type == SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT)
    {
      otmh_hdr_ext_src_port |=
        SOC_SAND_SET_BITS_RANGE(
          info->extension.src.src_sys_port.id,
          SOC_PETRA_HPU_OTMH_SRC_PORT_MSB,
          SOC_PETRA_HPU_OTMH_SRC_PORT_LSB
        );
    }
    else /* Lag */
    {
      otmh_hdr_ext_src_port |=
        SOC_SAND_SET_BITS_RANGE(
          info->extension.src.src_sys_port.member_id,
          SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_MEM_MSB,
          SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_MEM_LSB
        );

      otmh_hdr_ext_src_port |=
        SOC_SAND_SET_BITS_RANGE(
          info->extension.src.src_sys_port.id,
          SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_ID_MSB,
          SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_ID_LSB
        );
    }
  }

  if (info->extension.dest.enable == TRUE)
  {
    otmh_hdr_ext_dest_port |=
      SOC_SAND_SET_BITS_RANGE(
        info->extension.dest.dest_sys_port.id,
        SOC_PETRA_HPU_OTMH_EXT_DEST_PORT_MSB,
        SOC_PETRA_HPU_OTMH_EXT_DEST_PORT_LSB
      );
  }

  otmh->base                = otmh_hdr_base;
  otmh->extension_outlif    = otmh_hdr_ext_outlif;
  otmh->extension_src_port  = otmh_hdr_ext_src_port;
  otmh->extension_dest_port = otmh_hdr_ext_dest_port;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_hpu_otmh_build()",0,0);
}

/*********************************************************************
*     This function retrieves the fields that construct the
*     Outgoing TM Header given an OTMH header (base +
*     extension).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_otmh_parse(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH_HDR        *otmh,
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH            *info
  )
{
  uint32
    otmh_hdr_base = 0x0,
    otmh_hdr_ext_outlif = 0x0,
    otmh_hdr_ext_src_port = 0x0,
    otmh_hdr_ext_dest_port = 0x0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_HPU_OTMH_PARSE);

  SOC_SAND_CHECK_NULL_INPUT(otmh);
  SOC_SAND_CHECK_NULL_INPUT(info);


  otmh_hdr_base          = otmh->base;
  otmh_hdr_ext_outlif    = otmh->extension_outlif;
  otmh_hdr_ext_src_port  = otmh->extension_src_port;
  otmh_hdr_ext_dest_port = otmh->extension_dest_port;

  info->base.pp_header_present =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        otmh_hdr_base,
        SOC_PETRA_HPU_OTMH_PP_HEADER_PRESENT_MSB,
        SOC_PETRA_HPU_OTMH_PP_HEADER_PRESENT_LSB
      )
    );

  info->base.signature =
    SOC_SAND_GET_BITS_RANGE(
    otmh_hdr_base,
    SOC_PETRA_HPU_OTMH_SIGNATURE_MSB,
    SOC_PETRA_HPU_OTMH_SIGNATURE_LSB
    );

  info->base.multicast =
    SOC_SAND_NUM2BOOL(
      SOC_SAND_GET_BITS_RANGE(
        otmh_hdr_base,
        SOC_PETRA_HPU_OTMH_SYS_MC_MSB,
        SOC_PETRA_HPU_OTMH_SYS_MC_LSB
      )
    );

  info->base.dp =
    SOC_SAND_GET_BITS_RANGE(
    otmh_hdr_base,
    SOC_PETRA_HPU_OTMH_DP_MSB,
    SOC_PETRA_HPU_OTMH_DP_LSB
    );

  info->base.tr_cls =
    SOC_SAND_GET_BITS_RANGE(
    otmh_hdr_base,
    SOC_PETRA_HPU_OTMH_TR_CLS_MSB,
    SOC_PETRA_HPU_OTMH_TR_CLS_LSB
    );

  if (otmh_hdr_ext_outlif)
  {
    info->extension.outlif.enable = TRUE;
    info->extension.outlif.outlif=
      SOC_SAND_GET_BITS_RANGE(
      otmh_hdr_ext_outlif,
      SOC_PETRA_HPU_OTMH_SRC_PORT_MSB,
      SOC_PETRA_HPU_OTMH_SRC_PORT_LSB
      );
  }

  if (otmh_hdr_ext_src_port)
  {
    info->extension.src.enable = TRUE;
    info->extension.src.src_sys_port.type =
      SOC_SAND_GET_BITS_RANGE(
        otmh_hdr_ext_src_port,
        SOC_PETRA_HPU_OTMH_SRC_SYS_PORT_TYPE_MSB,
        SOC_PETRA_HPU_OTMH_SRC_SYS_PORT_TYPE_LSB
      );


    if (info->extension.src.src_sys_port.type == SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT)
    {
      info->extension.src.src_sys_port.id =
        SOC_SAND_GET_BITS_RANGE(
        otmh_hdr_ext_src_port,
        SOC_PETRA_HPU_OTMH_EXT_DEST_PORT_MSB,
        SOC_PETRA_HPU_OTMH_EXT_DEST_PORT_LSB
       );
    }
    else /* Lag */
    {
      info->extension.src.src_sys_port.member_id =
        SOC_SAND_GET_BITS_RANGE(
        otmh_hdr_ext_src_port,
        SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_MEM_MSB,
        SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_MEM_LSB
        );
      info->extension.src.src_sys_port.id =
        SOC_SAND_GET_BITS_RANGE(
        otmh_hdr_ext_src_port,
        SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_ID_MSB,
        SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_ID_LSB
        );
    }
  }

  if (otmh_hdr_ext_dest_port)
  {
    info->extension.dest.enable = TRUE;
    info->extension.dest.dest_sys_port.type = SOC_PETRA_DEST_SYS_PORT_TYPE_SYS_PHY_PORT;
    info->extension.dest.dest_sys_port.id =
      SOC_SAND_GET_BITS_RANGE(
      otmh_hdr_ext_dest_port,
      SOC_PETRA_HPU_OTMH_EXT_OUTLIF_MSB,
      SOC_PETRA_HPU_OTMH_EXT_OUTLIF_LSB
      );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_hpu_otmh_parse()",0,0);
}

void
  soc_petra_PETRA_HPU_ITMH_HDR_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_ITMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_ITMH_HDR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_FTMH_BASE_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_BASE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_FTMH_EXT_OUTLIF_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH_EXT_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_EXT_OUTLIF_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_FTMH_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_FTMH_HDR_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_HDR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_BASE_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_BASE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_EXT_DEST_PORT_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_EXT_DEST_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_EXTENSIONS_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_EXTENSIONS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXTENSIONS_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_HDR_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_HDR_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

void
  soc_petra_PETRA_HPU_ITMH_HDR_print(
    SOC_SAND_IN  SOC_PETRA_HPU_ITMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_ITMH_HDR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_FTMH_BASE_print(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_BASE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_FTMH_EXT_OUTLIF_print(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH_EXT_OUTLIF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_EXT_OUTLIF_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_FTMH_print(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_FTMH_HDR_print(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_FTMH_HDR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_BASE_print(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH_BASE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_BASE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_EXT_SRC_PORT_print(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH_EXT_SRC_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXT_SRC_PORT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_EXT_DEST_PORT_print(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH_EXT_DEST_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXT_DEST_PORT_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_EXTENSIONS_print(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH_EXTENSIONS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_EXTENSIONS_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_print(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HPU_OTMH_HDR_print(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH_HDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_HPU_OTMH_HDR_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

