/* $Id: pb_multicast_egress.c,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/soc_pb_tm/src/soc_pb_multicast_egress.h
*
* MODULE PREFIX:  soc_pb_mult_eg
*
* FILE DESCRIPTION: refer to H file.
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/Petra/PB_TM/pb_multicast_egress.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_MULT_EG_UNOCCUPIED_PORT_INDICATION               0x7F
#define SOC_PB_MULT_EG_UNOCCUPIED_BITMAP_INDICATION             0

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
*     This procedure Enters the multicast group part
*     into the specified entry - entry b.
*********************************************************************/
uint32
soc_pb_mult_eg_multicast_group_entry_b_to_tbl(
  SOC_SAND_IN  int                 unit,
  SOC_SAND_IN  uint32                 in_group_index,
  SOC_SAND_IN  uint32                  offset,
  SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
  SOC_SAND_IN  uint32                  mc_group_size,
  SOC_SAND_IN  uint16                  next_entry
  )
{
  uint32
    res;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA
    irr_egress_replication_multicast_db_format_b_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_MULTICAST_GROUP_ENTRY_B_TO_TBL);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_format_b_tbl_data,
    0,
    sizeof(SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (in_group_index < mc_group_size)
  {
    irr_egress_replication_multicast_db_format_b_tbl_data.out_port =
      mc_group[in_group_index].eg_entry.port;
    irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[0] =
      mc_group[in_group_index].eg_entry.cud;
  }
  else
  {
    /* 'fill occupied but empty' entry port with 0x7F */
    irr_egress_replication_multicast_db_format_b_tbl_data.out_port =
      SOC_PB_MULT_EG_UNOCCUPIED_PORT_INDICATION;  /* 0x7F */
    irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[0] =
      SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION;
  }

  if (in_group_index + 1 < mc_group_size)
  {
    irr_egress_replication_multicast_db_format_b_tbl_data.multicast_bitmap_ptr =
      mc_group[in_group_index + 1].eg_entry.vlan_mc_id;
    irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[1] =
      mc_group[in_group_index + 1].eg_entry.cud;
  }
  else
  {
    /* 'fill occupied but empty' entry bitmap with 0 */
    irr_egress_replication_multicast_db_format_b_tbl_data.multicast_bitmap_ptr =
      SOC_PB_MULT_EG_UNOCCUPIED_BITMAP_INDICATION;  /* 0 */
    irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[1] =
      SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION;
  }
  irr_egress_replication_multicast_db_format_b_tbl_data.format_select =
      SOC_PB_MULT_EG_FORMAT_SELECT_B;

  irr_egress_replication_multicast_db_format_b_tbl_data.link_ptr =
    next_entry;

  /* Write entry to HW*/

  res = soc_pb_irr_mcdb_egress_format_b_tbl_set_unsafe(
    unit,
    offset,
    &irr_egress_replication_multicast_db_format_b_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_multicast_group_entry_c_to_tbl()",0,0);
}

/*********************************************************************
*     This procedure Enters the multicast group part
*     into the specified entry - entry c.
*********************************************************************/
uint32
soc_pb_mult_eg_multicast_group_entry_c_to_tbl(
  SOC_SAND_IN  int                 unit,
  SOC_SAND_IN  uint32                 in_group_index,
  SOC_SAND_IN  uint32                  offset,
  SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
  SOC_SAND_IN  uint32                  mc_group_size,
  SOC_SAND_IN  uint16                  next_entry
  )
{
  uint32
    res;
  uint32
    in_group_i,
    indx,
    progress_index_by;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA
    irr_egress_replication_multicast_db_format_c_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_MULTICAST_GROUP_ENTRY_C_TO_TBL);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_format_c_tbl_data,
    0,
    sizeof(SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  progress_index_by = SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY;

  for (in_group_i = in_group_index; in_group_i < (in_group_index + progress_index_by); in_group_i++)
  {
    indx = in_group_i - in_group_index;

    if (in_group_i < mc_group_size)
    {
      irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[indx] =
        mc_group[in_group_i].eg_entry.vlan_mc_id;
      irr_egress_replication_multicast_db_format_c_tbl_data.out_lif[indx] =
        mc_group[in_group_i].eg_entry.cud;
    }
    else
    {
      /* 'fill occupied but empty' entry bitmap with 0 */
      irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[indx] =
        SOC_PB_MULT_EG_UNOCCUPIED_BITMAP_INDICATION;  /* 0 */
      irr_egress_replication_multicast_db_format_c_tbl_data.out_lif[indx] =
        SOC_PETRA_MULT_OCCUPIED_BUT_EMPTY_ENTRY_CUD_INDICATION;
    }

  }

  irr_egress_replication_multicast_db_format_c_tbl_data.format_select =
    SOC_PB_MULT_EG_FORMAT_SELECT_C;

  irr_egress_replication_multicast_db_format_c_tbl_data.link_ptr =
    next_entry;

  /* Write entry to HW*/

  res = soc_pb_irr_mcdb_egress_format_c_tbl_set_unsafe(
    unit,
    offset,
    &irr_egress_replication_multicast_db_format_c_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_multicast_group_entry_c_to_tbl()",0,0);
}

/*********************************************************************
*     Enters the multicast group part into the specified entry.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
soc_pb_mult_eg_multicast_group_entry_to_tbl(
  SOC_SAND_IN  int                 unit,
  SOC_SAND_IN  uint32                 in_group_index,
  SOC_SAND_IN  uint32                  offset,
  SOC_SAND_IN  SOC_PETRA_MULT_ENTRY          *mc_group,
  SOC_SAND_IN  uint32                  mc_group_size,
  SOC_SAND_IN  SOC_PETRA_MULT_EG_FORMAT_TYPE  format_type,
  SOC_SAND_IN  uint16                  next_entry
  )
{
  uint32
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_MULTICAST_GROUP_ENTRY_TO_TBL);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  
  if (format_type == SOC_PETRA_MULT_EG_FORMAT_B)
  {
    res = soc_pb_mult_eg_multicast_group_entry_b_to_tbl(
      unit,
      in_group_index,
      offset,
      mc_group,
      mc_group_size,
      next_entry
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (format_type == SOC_PETRA_MULT_EG_FORMAT_C)
  {
    res = soc_pb_mult_eg_multicast_group_entry_c_to_tbl(
      unit,
      in_group_index,
      offset,
      mc_group,
      mc_group_size,
      next_entry
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else
  {
    /* Invalid format */
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MULT_EG_TABLE_FORMAT_INVALID_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_multicast_group_entry_to_tbl()",0,0);
}

/*********************************************************************
*     Returns a multicast group containing the elements of a mc-id
*     entry in the multicast table - entry format b.
*********************************************************************/
uint32
  soc_pb_mult_eg_entry_content_format_b_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group,
    SOC_SAND_OUT uint32                *mc_group_size,
    SOC_SAND_OUT uint16                 *next_entry
  )
{
  uint32
    res;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA
    irr_egress_replication_multicast_db_format_b_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_ENTRY_CONTENT_FORMAT_B_GET);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_format_b_tbl_data,
    0,
    sizeof(SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *mc_group_size = SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY;

  res = soc_pb_irr_mcdb_egress_format_b_tbl_get_unsafe(
    unit,
    multicast_id_ndx,
    &irr_egress_replication_multicast_db_format_b_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  if (irr_egress_replication_multicast_db_format_b_tbl_data.out_port ==
    SOC_PB_MULT_EG_UNOCCUPIED_PORT_INDICATION)
  {
    *mc_group_size = *mc_group_size - 1 ;
  }
  mc_group[0].eg_entry.type = SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP;
  mc_group[0].eg_entry.port =
    irr_egress_replication_multicast_db_format_b_tbl_data.out_port;
  mc_group[0].eg_entry.cud =
    irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[0];
  
  if (irr_egress_replication_multicast_db_format_b_tbl_data.multicast_bitmap_ptr ==
    SOC_PB_MULT_EG_UNOCCUPIED_BITMAP_INDICATION)
  {
    *mc_group_size = *mc_group_size - 1 ;
  }
  mc_group[1].eg_entry.type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
  mc_group[1].eg_entry.vlan_mc_id =
    irr_egress_replication_multicast_db_format_b_tbl_data.multicast_bitmap_ptr;
  mc_group[1].eg_entry.cud =
    irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[1];
 
  *next_entry =
    (uint16)irr_egress_replication_multicast_db_format_b_tbl_data.link_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_entry_content_format_b_get()",0,0);
}

/*********************************************************************
*     Returns a multicast group containing the elements of a mc-id
*     entry in the multicast table - entry format c.
*********************************************************************/
uint32
  soc_pb_mult_eg_entry_content_format_c_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group,
    SOC_SAND_OUT uint32                *mc_group_size,
    SOC_SAND_OUT uint16                 *next_entry
  )
{
  uint32
    res;
  uint32
    indx,
    progress_index;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA
    irr_egress_replication_multicast_db_format_c_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_ENTRY_CONTENT_FORMAT_C_GET);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_format_c_tbl_data,
    0,
    sizeof(SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  progress_index = SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY;

  *mc_group_size = progress_index;

  res = soc_pb_irr_mcdb_egress_format_c_tbl_get_unsafe(
    unit,
    multicast_id_ndx,
    &irr_egress_replication_multicast_db_format_c_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  for (indx = 0 ; indx < progress_index; indx++)
  {
    if (irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[indx] ==
      SOC_PB_MULT_EG_UNOCCUPIED_BITMAP_INDICATION)
    {
      *mc_group_size = *mc_group_size - 1 ;
    }
   
    mc_group[indx].eg_entry.type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
    mc_group[indx].eg_entry.vlan_mc_id =
      irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[indx];
    mc_group[indx].eg_entry.cud =
      irr_egress_replication_multicast_db_format_c_tbl_data.out_lif[indx];
  }

  *next_entry =
    (uint16)irr_egress_replication_multicast_db_format_c_tbl_data.link_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_entry_content_format_c_get()",0,0);
}

/*********************************************************************
*     Returns a multicast group containing the elements of a mc-id
*     entry in the multicast table.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mult_eg_entry_content_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_MULT_ID            multicast_id_ndx,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_FORMAT_TYPE format_type,
    SOC_SAND_OUT SOC_PETRA_MULT_ENTRY         *mc_group,
    SOC_SAND_OUT uint32                *mc_group_size,
    SOC_SAND_OUT uint16                 *next_entry
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_ENTRY_CONTENT_GET);

  SOC_SAND_CHECK_NULL_INPUT(mc_group);
  SOC_SAND_CHECK_NULL_INPUT(mc_group_size);
  SOC_SAND_CHECK_NULL_INPUT(next_entry);
  
  if (format_type == SOC_PETRA_MULT_EG_FORMAT_B)
  {
    res = soc_pb_mult_eg_entry_content_format_b_get(
      unit,
      multicast_id_ndx,
      mc_group,
      mc_group_size,
      next_entry
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (format_type == SOC_PETRA_MULT_EG_FORMAT_C)
  {
    res = soc_pb_mult_eg_entry_content_format_c_get(
      unit,
      multicast_id_ndx,
      mc_group,
      mc_group_size,
      next_entry
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_entry_content_get()",0,0);

}

/*********************************************************************
*     This function returns the number of elements that are used per entry.
*     - entry format b
*********************************************************************/
uint32
  soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry_b(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint16                 tbl_entry_id,
    SOC_SAND_OUT uint32                *size
  )
{
  uint32
    res;
  uint32
    tmp_size = 0;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA
    irr_egress_replication_multicast_db_format_b_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_NOF_OCCUPIED_ELEMENTS_IN_TBL_EMTRY_B);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_format_b_tbl_data,
    0,
    sizeof(SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_irr_mcdb_egress_format_b_tbl_get_unsafe(
    unit,
    tbl_entry_id,
    &irr_egress_replication_multicast_db_format_b_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (irr_egress_replication_multicast_db_format_b_tbl_data.out_port !=
      SOC_PB_MULT_EG_UNOCCUPIED_PORT_INDICATION)
  {
      /*this part of entry is occupied*/
      tmp_size++;
  }
 

if (irr_egress_replication_multicast_db_format_b_tbl_data.multicast_bitmap_ptr !=
    SOC_PB_MULT_EG_UNOCCUPIED_BITMAP_INDICATION)
  {
      /*this part of entry is occupied*/
      tmp_size++;
  }

  *size = tmp_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry_b()",0,0);
}

/*********************************************************************
*     This function returns the number of elements that are used per entry.
*     - entry format c
*********************************************************************/
uint32
soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry_c(
  SOC_SAND_IN  int                unit,
  SOC_SAND_IN  uint16                 tbl_entry_id,
  SOC_SAND_OUT uint32                *size
  )
{
  uint32
    res;
  uint32
    indx,
    progress_index,
    tmp_size = 0;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA
    irr_egress_replication_multicast_db_format_c_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_NOF_OCCUPIED_ELEMENTS_IN_TBL_EMTRY_C);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_format_c_tbl_data,
    0,
    sizeof(SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_irr_mcdb_egress_format_c_tbl_get_unsafe(
    unit,
    tbl_entry_id,
    &irr_egress_replication_multicast_db_format_c_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  progress_index = SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY;

  for (indx = 0;
    indx < progress_index;
    indx++)
  {
    if (irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[indx]!=
      SOC_PB_MULT_EG_UNOCCUPIED_BITMAP_INDICATION)
    {
      /*this part of entry is occupied*/
      tmp_size++;
    }
  }

  *size = tmp_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry_c()",0,0);
}

/*********************************************************************
*     This function returns the number of elements that are used per entry.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint16                 tbl_entry_id,
    SOC_SAND_IN  SOC_PETRA_MULT_EG_FORMAT_TYPE format_type,
    SOC_SAND_OUT uint32                *size
  )
{
  uint32
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_NOF_OCCUPIED_ELEMENTS_IN_TBL_ENTRY);

  SOC_SAND_CHECK_NULL_INPUT(size);
  
  if (format_type == SOC_PETRA_MULT_EG_FORMAT_B)
  {
    res = soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry_b(
      unit,
      tbl_entry_id,
      size
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else if (format_type == SOC_PETRA_MULT_EG_FORMAT_C)
  {
    res = soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry_c(
      unit,
      tbl_entry_id,
      size
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  }
  else
  {
    /* Invalid format */
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MULT_EG_TABLE_FORMAT_INVALID_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_nof_occupied_elements_in_tbl_entry()",0,0);
}

/*********************************************************************
*     Gets the multicast entry with the specified table entry, format C.
*********************************************************************/
uint32
  soc_pb_mult_eg_fill_in_entry_format_c(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id,
    SOC_SAND_IN  uint32                  entry_size,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_entry
  )
{
  uint32
    indx;
  uint32
    res,
    nof_entries;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA
    irr_egress_replication_multicast_db_format_c_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_FILL_IN_ENTRY_FORMAT_C);
  
  res = soc_pb_irr_mcdb_egress_format_c_tbl_get_unsafe(
    unit,
    tbl_entry_id,
    &irr_egress_replication_multicast_db_format_c_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  nof_entries = SOC_SAND_MIN(entry_size,SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY);
  for (indx = 0; indx < nof_entries; indx++)
  {
    mc_entry[indx].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
    mc_entry[indx].vlan_mc_id = irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[indx];
    mc_entry[indx].cud = irr_egress_replication_multicast_db_format_c_tbl_data.out_lif[indx];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_fill_in_entry_format_c()",0,0);
}

/*********************************************************************
*     Gets the multicast entry with the specified table entry, format B.
*********************************************************************/
uint32
  soc_pb_mult_eg_fill_in_entry_format_b(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id,
    SOC_SAND_IN  uint32                  entry_size,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_entry
  )
{
  uint32
    res;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA
    irr_egress_replication_multicast_db_format_b_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_FILL_IN_ENTRY_FORMAT_B);
  
  res = soc_pb_irr_mcdb_egress_format_b_tbl_get_unsafe(
    unit,
    tbl_entry_id,
    &irr_egress_replication_multicast_db_format_b_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (entry_size > 0)
  {
    mc_entry[0].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP;
    mc_entry[0].port = irr_egress_replication_multicast_db_format_b_tbl_data.out_port;
    mc_entry[0].cud = irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[0];
  }
  if (entry_size > 1)
  {
    mc_entry[1].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR;
    mc_entry[1].vlan_mc_id = irr_egress_replication_multicast_db_format_b_tbl_data.multicast_bitmap_ptr;
    mc_entry[1].cud = irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[1];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_fill_in_entry_format_b()",0,0);
}

/*********************************************************************
*     This function fill the content of the mc group from table
*     into the mc group given, limited by mc group size.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mult_eg_fill_in_group(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id,
    SOC_SAND_IN  uint32                  mc_group_size,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_ENTRY       *mc_group
  )
{
  uint32
    res;
  uint16
    next_entry_id;
  uint32
    indx,
    indx2,
    indx_in_entry,
    progress_index_by = 0,
    nof_entries;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_egress_replication_multicast_db_tbl_data;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_FILL_IN_GROUP);
  
  next_entry_id = tbl_entry_id;

  for (indx = 0 ; indx < mc_group_size ; indx+= progress_index_by)
  {
    res = soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe(
      unit,
      next_entry_id,
      &irr_egress_replication_multicast_db_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    
    if (irr_egress_replication_multicast_db_tbl_data.port[0] == SOC_PB_MULT_EG_FORMAT_SELECT_B)
    {
      nof_entries = SOC_SAND_MIN(SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY,mc_group_size-indx);
      res = soc_pb_mult_eg_fill_in_entry_format_b(
        unit,
        next_entry_id,
        nof_entries,
        &mc_group[indx]
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
      progress_index_by = SOC_PETRA_MULT_EG_FORMAT_B_PER_ENTRY;
    }
    else if (irr_egress_replication_multicast_db_tbl_data.port[0] == SOC_PB_MULT_EG_FORMAT_SELECT_C)
    {
      nof_entries = SOC_SAND_MIN(SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY,mc_group_size-indx);
      res = soc_pb_mult_eg_fill_in_entry_format_c(
        unit,
        next_entry_id,
        nof_entries,
        &mc_group[indx]
      );
      SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
      progress_index_by = SOC_PETRA_MULT_EG_FORMAT_C_PER_ENTRY;
    }
    else
    {
      nof_entries = SOC_SAND_MIN(SOC_PETRA_MULT_EG_FORMAT_A_PER_ENTRY,mc_group_size-indx);
      for (indx2 = indx; indx2 < indx+nof_entries; indx2 ++)
      {
        indx_in_entry = indx2-indx;
        mc_group[indx2].type = SOC_PETRA_MULT_EG_ENTRY_TYPE_OFP;
        mc_group[indx2].port =
          irr_egress_replication_multicast_db_tbl_data.port[indx_in_entry];
        mc_group[indx2].cud =
          irr_egress_replication_multicast_db_tbl_data.out_lif[indx_in_entry];
      }
      progress_index_by = SOC_PETRA_MULT_EG_FORMAT_A_PER_ENTRY;
    }
    
    next_entry_id =
        (uint16)irr_egress_replication_multicast_db_tbl_data.link_ptr;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_fill_in_group()",0,0);
}

/*********************************************************************
*     This function adds the content of the entry into the last entry of
*     mc-group link list, if the entry has an unoccupied space.
*********************************************************************/
uint32
  soc_pb_mult_eg_fill_in_last_entry(
    SOC_SAND_IN int               unit,
    SOC_SAND_IN uint16                tbl_entry_id,
    SOC_SAND_IN SOC_PETRA_MULT_ENTRY        *entry,
    SOC_SAND_OUT uint8              *success
  )
{
  uint32
    res;
  uint32
    indx;
  uint8
    succeeded = FALSE;
  SOC_PETRA_MULT_EG_FORMAT_TYPE
    format_type = SOC_PETRA_MULT_EG_FORMAT_A;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_egress_replication_multicast_db_tbl_data;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA
    irr_egress_replication_multicast_db_format_b_tbl_data;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA
    irr_egress_replication_multicast_db_format_c_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MULT_EG_FILL_IN_LAST_ENTRY);

  SOC_SAND_CHECK_NULL_INPUT(entry);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_tbl_data,
    0,
    sizeof(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_format_b_tbl_data,
    0,
    sizeof(SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_format_c_tbl_data,
    0,
    sizeof(SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe(
    unit,
    tbl_entry_id,
    &irr_egress_replication_multicast_db_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (irr_egress_replication_multicast_db_tbl_data.port[0] ==
    SOC_PB_MULT_EG_FORMAT_SELECT_C)
  {
    res = soc_pb_irr_mcdb_egress_format_c_tbl_get_unsafe(
      unit,
      tbl_entry_id,
      &irr_egress_replication_multicast_db_format_c_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    if (irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[1]
      == SOC_PB_MULT_EG_UNOCCUPIED_BITMAP_INDICATION)
    {
       if (entry->eg_entry.type == SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR)
       {
         /* Adding another vlan pointer: from V_ => VV */
         irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[1]
          = entry->eg_entry.vlan_mc_id;
         irr_egress_replication_multicast_db_format_c_tbl_data.out_lif[1]
          = entry->eg_entry.cud;
          format_type = SOC_PETRA_MULT_EG_FORMAT_C;
       }
       else
       {
         /* Adding another port: from V_ => PV change format c->b. */
         irr_egress_replication_multicast_db_format_b_tbl_data.out_port = entry->eg_entry.port;
         irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[0] = entry->eg_entry.cud;
         irr_egress_replication_multicast_db_format_b_tbl_data.multicast_bitmap_ptr
           = irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[0];
         irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[1]
           = irr_egress_replication_multicast_db_format_c_tbl_data.out_lif[0];
         irr_egress_replication_multicast_db_format_b_tbl_data.format_select = SOC_PB_MULT_EG_FORMAT_SELECT_B;
         format_type = SOC_PETRA_MULT_EG_FORMAT_B;
       }
      succeeded = TRUE;
    }
  }
  /* if last entry is format B, meaning entry is full. */
  /* last entry is format A */
  if (irr_egress_replication_multicast_db_tbl_data.port[0] != SOC_PB_MULT_EG_FORMAT_SELECT_B &&
    irr_egress_replication_multicast_db_tbl_data.port[0] != SOC_PB_MULT_EG_FORMAT_SELECT_C)
  {
    if (entry->eg_entry.type != SOC_PETRA_MULT_EG_ENTRY_TYPE_VLAN_PTR)
    {
      /* Adding another port: if possible add to next available place - same as Soc_petraA*/
      for (indx = 0;
        indx < SOC_PETRA_MULT_EG_FORMAT_A_PER_ENTRY;
        indx++)
      {
        if (!succeeded)
        {
          if (irr_egress_replication_multicast_db_tbl_data.port[indx]==
            SOC_PB_MULT_EG_UNOCCUPIED_PORT_INDICATION)
          {
            /*this part of entry is not-occupied*/
            irr_egress_replication_multicast_db_tbl_data.port[indx] =
              entry->eg_entry.port;
            irr_egress_replication_multicast_db_tbl_data.out_lif[indx] =
              entry->eg_entry.cud;
            format_type = SOC_PETRA_MULT_EG_FORMAT_A;
            succeeded = TRUE;
          }
        }
      }
    }
    else
    {
      /* Adding vlan pointer part */
      if (irr_egress_replication_multicast_db_tbl_data.port[0] ==
        SOC_PB_MULT_EG_UNOCCUPIED_PORT_INDICATION)
      {
        /* Adding vlan pointer: from ___ => V_ change format a->c */
        irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[0] =
          entry->eg_entry.vlan_mc_id;
        irr_egress_replication_multicast_db_format_c_tbl_data.out_lif[0] =
          entry->eg_entry.cud;
        irr_egress_replication_multicast_db_format_c_tbl_data.multicast_bitmap_ptr[1] =
          SOC_PB_MULT_EG_UNOCCUPIED_BITMAP_INDICATION;
        irr_egress_replication_multicast_db_format_c_tbl_data.format_select = SOC_PB_MULT_EG_FORMAT_SELECT_C;
        format_type = SOC_PETRA_MULT_EG_FORMAT_C;
        succeeded = TRUE;
      }
      
      else if (irr_egress_replication_multicast_db_tbl_data.port[1] ==
          SOC_PB_MULT_EG_UNOCCUPIED_PORT_INDICATION)
      {
        /* Adding vlan pointer: from P__ => PV change format a->b */
        irr_egress_replication_multicast_db_format_b_tbl_data.out_port =
          irr_egress_replication_multicast_db_tbl_data.port[0];
        irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[0] =
          irr_egress_replication_multicast_db_tbl_data.out_lif[0];
        irr_egress_replication_multicast_db_format_b_tbl_data.multicast_bitmap_ptr =
          entry->eg_entry.vlan_mc_id;
        irr_egress_replication_multicast_db_format_b_tbl_data.out_lif[1] =
          entry->eg_entry.cud;
        irr_egress_replication_multicast_db_format_b_tbl_data.format_select = SOC_PB_MULT_EG_FORMAT_SELECT_B;
        format_type = SOC_PETRA_MULT_EG_FORMAT_B;
        succeeded = TRUE;
      }
      else if (irr_egress_replication_multicast_db_tbl_data.port[2] ==
        SOC_PB_MULT_EG_UNOCCUPIED_PORT_INDICATION)
      {
        /* This case is not valid */
      }
                  
    }
      
  }
   
  irr_egress_replication_multicast_db_tbl_data.link_ptr = soc_petra_mc_null_id(unit, FALSE);
  /* Write entry to HW*/

  switch (format_type)
  {
  case SOC_PETRA_MULT_EG_FORMAT_A:
    res = soc_petra_irr_egress_replication_multicast_db_tbl_set_unsafe(
      unit,
      tbl_entry_id,
      &irr_egress_replication_multicast_db_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  	break;
  case SOC_PETRA_MULT_EG_FORMAT_B:
    res = soc_pb_irr_mcdb_egress_format_b_tbl_set_unsafe(
      unit,
      tbl_entry_id,
      &irr_egress_replication_multicast_db_format_b_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    break;
  case SOC_PETRA_MULT_EG_FORMAT_C:
    res = soc_pb_irr_mcdb_egress_format_c_tbl_set_unsafe(
      unit,
      tbl_entry_id,
      &irr_egress_replication_multicast_db_format_c_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    break;
  /* must default. Otherwise - compilation error */
  /* coverity[dead_error_begin:FALSE] */
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MULT_EG_TABLE_FORMAT_INVALID_ERR, 90, exit);
    break;
  }
  
  *success = succeeded;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mult_eg_fill_in_last_entry()",0,0);
}

/*********************************************************************
*     This function returns entry format of the entry given.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mult_eg_tbl_entry_format_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint16                  tbl_entry_id,
    SOC_SAND_OUT SOC_PETRA_MULT_EG_FORMAT_TYPE *entry_format
  )
{
  uint32
    res;
  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_egress_replication_multicast_db_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MULT_EG_TBL_ENTRY_FORMAT_GET);

  SOC_SAND_CHECK_NULL_INPUT(entry_format);

  res = SOC_SAND_OK; sal_memset(
    &irr_egress_replication_multicast_db_tbl_data,
    0,
    sizeof(SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL_DATA)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_irr_egress_replication_multicast_db_tbl_get_unsafe(
    unit,
    tbl_entry_id,
    &irr_egress_replication_multicast_db_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  
  *entry_format = SOC_PETRA_MULT_EG_FORMAT_A;

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    if (irr_egress_replication_multicast_db_tbl_data.port[0]
      == SOC_PB_MULT_EG_FORMAT_SELECT_B)
    {
      *entry_format = SOC_PETRA_MULT_EG_FORMAT_B;
    }
    if (irr_egress_replication_multicast_db_tbl_data.port[0]
      == SOC_PB_MULT_EG_FORMAT_SELECT_C)
    {
      *entry_format = SOC_PETRA_MULT_EG_FORMAT_C;
    }
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mult_eg_tbl_entry_format_get()",0,0);
}



#include <soc/dpp/SAND/Utils/sand_footer.h>
