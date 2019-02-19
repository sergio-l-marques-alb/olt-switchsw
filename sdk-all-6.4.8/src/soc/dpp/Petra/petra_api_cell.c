/* $Id: petra_api_cell.c,v 1.6 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/Petra/petra_cell.h>
#include <soc/dpp/Petra/petra_api_cell.h>
#include <soc/dpp/Petra/petra_general.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

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


/* $Id: petra_api_cell.c,v 1.6 Broadcom SDK $
 * Allows the cpu to generate a direct read command
 */
uint32
  soc_petra_read_from_fe600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST        *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_READ_FROM_FE600);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_read_from_fe600_unsafe(
          unit,
          sr_link_list,
          size,
          offset,
          data_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_read_from_fe600()",0,0);
}
/*
 * Allows the cpu to generate a direct write command
 */
uint32
  soc_petra_write_to_fe600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST        *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_WRITE_FROM_FE600);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE ;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE ;

  res = soc_petra_write_to_fe600_unsafe(
          unit,
          sr_link_list,
          size,
          offset,
          data_in
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_write_to_fe600()",0,0);

}


/*
 * Allows the cpu to generate a indirect read command
 */
uint32
  soc_petra_indirect_read_from_fe600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INDIRECT_READ_FROM_FE600);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE ;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE ;

  res = soc_petra_indirect_read_from_fe600_unsafe(
          unit,
          sr_link_list,
          size,
          offset,
          data_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_indirect_read_from_fe600()",0,0);

}


/*
 * Allows the cpu to generate a indirect write command
 */
uint32
  soc_petra_indirect_write_to_fe600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST        *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INDIRECT_WRITE_FROM_FE600);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE ;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE ;

  res = soc_petra_indirect_write_to_fe600_unsafe(
          unit,
          sr_link_list,
          size,
          offset,
          data_in
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_indirect_write_fe600()",0,0);

}

/*
 * Allows the cpu to generate a write command
 */
uint32
  soc_petra_cpu2cpu_write(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        *data_in
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CPU2CPU_WITH_FE600_WRITE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE ;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE ;

  res = soc_petra_cpu2cpu_write_unsafe(
          unit,
          sr_link_list,
          size,
          data_in
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cpu2cpu_write()",0,0);

}

/*
 * Allows the cpu to generate a read command
 */
uint32
  soc_petra_cpu2cpu_read(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT uint32                        *data_out
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CPU2CPU_WITH_FE600_READ);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE ;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE ;

  res = soc_petra_cpu2cpu_read_unsafe(
          unit,
          data_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);


exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_cpu2cpu_read()",0,0);

}

/*********************************************************************
*     Configure an entry of the SOC_SAND_FE600 multicast tables via
 *     inband indirect write cells.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_cell_mc_tbl_write(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CELL_MC_TBL_WRITE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  res = soc_petra_cell_mc_tbl_write_verify(
          unit,
          mc_id_ndx,
          info,
          tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_cell_mc_tbl_write_unsafe(
          unit,
          mc_id_ndx,
          info,
          tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_cell_mc_tbl_write()",mc_id_ndx,0);
}

/*********************************************************************
*     Get an entry of the SOC_SAND_FE600 multicast tables via an inband
 *     indirect read cell.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_cell_mc_tbl_read(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info,
    SOC_SAND_OUT SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CELL_MC_TBL_READ);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  res = soc_petra_cell_mc_tbl_read_verify(
          unit,
          mc_id_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_cell_mc_tbl_read_unsafe(
          unit,
          mc_id_ndx,
          info,
          tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_cell_mc_tbl_read()",mc_id_ndx,0);
}

void
  soc_petra_PETRA_SR_CELL_LINK_LIST_clear(
    SOC_SAND_OUT SOC_PETRA_SR_CELL_LINK_LIST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_SR_CELL_LINK_LIST_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_CELL_MC_TBL_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_CELL_MC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CELL_MC_TBL_DATA_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_CELL_MC_TBL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_CELL_MC_TBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CELL_MC_TBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_CELL_FE_LOCATION_to_string(
    SOC_SAND_IN  SOC_PETRA_CELL_FE_LOCATION enum_val
  )
{
  return SOC_TMC_CELL_FE_LOCATION_to_string(enum_val);
}

void
  soc_petra_PETRA_SR_CELL_LINK_LIST_print(
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_SR_CELL_LINK_LIST_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_CELL_MC_TBL_DATA_print(
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_DATA *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CELL_MC_TBL_DATA_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_CELL_MC_TBL_INFO_print(
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_CELL_MC_TBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
