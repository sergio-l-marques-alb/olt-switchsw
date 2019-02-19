/* $Id: soc_pb_multicast_fabric.c,v 1.5 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_TM/pb_multicast_fabric.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_mgmt.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_MULT_NOF_INGRESS_SHAPINGS             (2)
#define SOC_PB_MULT_TC_MAPPING_FABRIC_MULT_NO_IS     (6)
#define SOC_PB_MULT_TC_MAPPING_FABRIC_MULT_WITH_IS   (7)

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

/* $Id: soc_pb_multicast_fabric.c,v 1.5 Broadcom SDK $
 *  Verify upon group creation / update or entry addition
 *  if the range is not reserved for single replication multicast ids
 */
uint32
  soc_pb_mult_fabric_ingress_single_copy_range_b0_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mc_id_ndx,
    SOC_SAND_IN  uint32                  mc_group_size
  )
{
  uint32
    res;
  SOC_PB_MGMT_B0_INFO
    b0_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_FABRIC_INGRESS_SINGLE_COPY_RANGE_B0_VERIFY);

  if (SOC_PB_REV_A1_OR_BELOW == FALSE)
  {
    SOC_PB_MGMT_B0_INFO_clear(&b0_info);
    res = soc_pb_mgmt_rev_b0_get_unsafe(
            unit,
            &b0_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /* Verify if the range is up for this MC-Id (16 ranges of 1K) */
    if (SOC_SAND_GET_BIT(b0_info.single_copy_mc_rng_bmp, (mc_id_ndx / 1024)) == TRUE)
    {
      /* Verify the MC-group size */
      if (mc_group_size > 1)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PB_FABRIC_MULT_INGRESS_MULT_NOT_SINGLE_COPY_ERR, 20, exit);
     }
    }
  }
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_fabric_ingress_single_copy_range_b0_verify()",0,0);
}


/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS              tr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_FABRIC_CLS     new_mult_cls
  )
{
  uint32
    entry_offset,
    res;
  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA
    tbl_data;
  int32
    is_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_SET_UNSAFE);

  for (is_ndx = 0; is_ndx < SOC_PB_MULT_NOF_INGRESS_SHAPINGS; ++is_ndx)
  {
    if (is_ndx == 0)
    {
      entry_offset = SOC_PB_MULT_TC_MAPPING_FABRIC_MULT_NO_IS;
    }
    else
    {
      entry_offset = SOC_PB_MULT_TC_MAPPING_FABRIC_MULT_WITH_IS;
    }

    res = soc_pb_irr_traffic_class_mapping_tbl_get_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    tbl_data.traffic_class_mapping[tr_cls_ndx] = new_mult_cls;

    res = soc_pb_irr_traffic_class_mapping_tbl_set_unsafe(
            unit,
            entry_offset,
            &tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_fabric_traffic_class_to_multicast_cls_map_set_unsafe()",0,0);
}

/*********************************************************************
*     Maps the embedded traffic class in the packet header to
*     a multicast class (0..3). This multicast class will be
*     further used for egress/fabric replication.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  SOC_PETRA_TR_CLS           tr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_FABRIC_CLS  *new_mult_cls
  )
{
  uint32
    entry_offset,
    res;
  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_FABRIC_TRAFFIC_CLASS_TO_MULTICAST_CLS_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(new_mult_cls);

  entry_offset = SOC_PB_MULT_TC_MAPPING_FABRIC_MULT_NO_IS;

  res = soc_pb_irr_traffic_class_mapping_tbl_get_unsafe(
          unit,
          entry_offset,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *new_mult_cls = tbl_data.traffic_class_mapping[tr_cls_ndx];

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_fabric_traffic_class_to_multicast_cls_map_get_unsafe()",0,0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>
