/* $Id: petra_api_reg_access.c,v 1.7 Broadcom SDK $
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
* FILENAME:       soc_petra_reg_access.c
*
* MODULE PREFIX:  soc_petra_reg_access
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_framework.h>

#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
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

/*****************************************************
*NAME
*  soc_petra_read_fld
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Read a field from Soc_petra register
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field - pointer to the requested
*                               field descriptor in registers database
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32*         val - return value.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    val - the value of the requested field.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_read_fld(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD    *field,
    SOC_SAND_IN  uint32           instance_idx,
    SOC_SAND_OUT uint32            *val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_READ_FLD);

  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(val);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_read_fld_unsafe(
          unit,
          field,
          instance_idx,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_read_fld",0,0);
}

/*****************************************************
*NAME
*  soc_petra_write_fld
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Write to a field of Soc_petra register
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_FIELD*  field - pointer to the requested
*                               field descriptor in registers database
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32          val - the value to write.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_write_fld(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD *field,
    SOC_SAND_IN  uint32        instance_idx,
    SOC_SAND_IN  uint32         val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_WRITE_FLD);

  SOC_SAND_CHECK_NULL_INPUT(field);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_write_fld_unsafe(
          unit,
          field,
          instance_idx,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_write_fld",0,0);
}

/*****************************************************
*NAME
*  soc_petra_read_reg
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Read Soc_petra register
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_ADDR*   reg - pointer to the requested
*                               register descriptor in registers database
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32*         val - return value.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    val - the value of the requested register.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_read_reg(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR  *reg,
    SOC_SAND_IN  uint32        instance_idx,
    SOC_SAND_OUT uint32         *val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_READ_REG);
  SOC_SAND_CHECK_NULL_INPUT(reg);
  SOC_SAND_CHECK_NULL_INPUT(val);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_read_reg_unsafe(
          unit,
          reg,
          instance_idx,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_read_reg",0,0);
}

/*****************************************************
*NAME
*  soc_petra_write_reg
*TYPE:
*  PROC
*DATE:
*  07/08/2007
*FUNCTION:
*  Write Soc_petra register
*INPUT:
*    SOC_SAND_IN  int      unit - device identifier
*    SOC_SAND_IN  SOC_PETRA_REG_ADDR*   reg - pointer to the requested
*                               register descriptor in registers database
*    SOC_SAND_IN  uint32      instance_idx - if a block can have
*                               multiple instances at different offset,
*                               this is the index of the requested instance.
*                               For single instance - this value is 0.
*    SOC_SAND_OUT uint32          val - the register value to write.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_write_reg(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_REG_ADDR     *reg,
    SOC_SAND_IN  uint32           instance_idx,
    SOC_SAND_IN  uint32            val
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_WRITE_REG);
  SOC_SAND_CHECK_NULL_INPUT(reg);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_write_reg_unsafe(
          unit,
          reg,
          instance_idx,
          val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_write_reg",0,0);
}

/*********************************************************************
*     Preform polling on the value of a given register.
*     According to the poll_info structure, the polling is
*     first performed as busy-wait, repeatedly reading the
*     register. Then, a polling is performed with the
*     requested timer delay between consecutive reads. The
*     'success' parameter returns TRUE if the expected value
*     is read, and FALSE if all the polling iterations do not
*     read the expected value (timeout).
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_status_fld_poll(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_REG_FIELD           *field,
    SOC_SAND_IN  uint32                 instance_ndx,
    SOC_SAND_IN  SOC_PETRA_POLL_INFO           *poll_info,
    SOC_SAND_OUT uint8                 *success
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STATUS_FLD_POLL);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(field);
  SOC_SAND_CHECK_NULL_INPUT(poll_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_status_fld_poll_unsafe(
    unit,
    field,
    instance_ndx,
    poll_info,
    success
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_status_fld_poll()",0,0);
}

void
  soc_petra_PETRA_POLL_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_POLL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_POLL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

void
  soc_petra_PETRA_POLL_INFO_print(
    SOC_SAND_IN SOC_PETRA_POLL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_POLL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

