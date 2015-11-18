/* $Id: petra_ssr.c,v 1.7 Broadcom SDK $
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
* FILENAME:       soc_petra_ssr.c
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

#include <soc/dpp/SAND/Management/sand_ssr.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_ssr.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/petra_ssr.h>
#include <soc/dpp/Petra/petra_api_ssr.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_reg_access.h>

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

typedef
  uint32
    (*SOC_PETRA_SSR_TRANSFORM_FUNC)(
      SOC_SAND_IN  uint8*,
      SOC_SAND_OUT uint8*
    );

typedef
  uint32
    (*SOC_PETRA_SSR_SW_DB_SIZE_FUNC)(
      SOC_SAND_IN  SOC_PETRA_SW_DB_CFG_VERSION_NUM,
      SOC_SAND_OUT uint32*);

typedef struct
{
  SOC_PETRA_SSR_TRANSFORM_FUNC   transform_func;

  SOC_PETRA_SSR_SW_DB_SIZE_FUNC  sw_db_size_func;

} SOC_PETRA_SSR_VERSION_SERVICES;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

uint32
  soc_petra_ssr_forbidden_ver_trans(
    SOC_SAND_IN   uint8                     *source_data,
    SOC_SAND_OUT  uint8                     *dest_data
  );

uint32
  soc_petra_ssr_forbidden_ver_size(
    SOC_SAND_IN SOC_PETRA_SW_DB_CFG_VERSION_NUM  sw_db_ver,
    SOC_SAND_OUT uint32                     *sw_db_data_size
  );

static
  SOC_PETRA_SSR_VERSION_SERVICES
    Soc_petra_ssr_version_services_array[SOC_PETRA_SW_DB_CFG_NOF_VALID_VERSIONS + 1] =
    {
      {
        soc_petra_ssr_forbidden_ver_trans,
        soc_petra_ssr_forbidden_ver_size
      },
    };

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_petra_ssr_forbidden_ver_size(
    SOC_SAND_IN  SOC_PETRA_SW_DB_CFG_VERSION_NUM  sw_db_ver,
    SOC_SAND_OUT uint32                      *sw_db_data_size
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_FORBIDDEN_VER_SIZE);

  SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SSR_FORBIDDEN_FUNCTION_CALL_ERR, 1, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_forbidden_ver_size", 0, 0);
}

uint32
  soc_petra_ssr_forbidden_ver_trans(
    SOC_SAND_IN   uint8                     *source_data,
    SOC_SAND_OUT  uint8                     *dest_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_FORBIDDEN_VER_TRANS);

  SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SSR_FORBIDDEN_FUNCTION_CALL_ERR, 1, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_forbidden_ver_trans", 0, 0);
}

uint32
  soc_petra_ssr_max_sw_db_size_get(
    SOC_SAND_OUT  uint32   *size
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    max_size = 0,
    ver_size = 0;
  uint32
    ver_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_MAX_SW_DB_SIZE_GET);

  for(ver_idx = SOC_PETRA_SW_DB_CFG_VERSION_NUM_0; ver_idx < SOC_PETRA_SW_DB_CFG_NOF_VERSIONS; ++ver_idx)
  {
    res = Soc_petra_ssr_version_services_array[ver_idx].sw_db_size_func(
            ver_idx,
            &ver_size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

    max_size = SOC_SAND_MAX(max_size, ver_size);
  }

  *size = max_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_max_sw_db_size_get", 0, 0);
}

uint32
  soc_petra_ssr_sw_db_size_get(
    SOC_SAND_IN  uint32  ver_idx,
    SOC_SAND_OUT uint32   *size
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ver_size = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_SW_DB_SIZE_GET);

  res = Soc_petra_ssr_version_services_array[ver_idx].sw_db_size_func(
          ver_idx,
          &ver_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

  *size = ver_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_sw_db_size_get", 0, 0);
}

uint32
  soc_petra_ssr_cfg_version_get(
    SOC_SAND_IN   uint32                      soc_sand_ver,
    SOC_SAND_OUT  SOC_PETRA_SW_DB_CFG_VERSION_NUM  *sw_db_version
  )
{
  SOC_PETRA_SW_DB_CFG_VERSION_NUM
    sw_db_ver_num = SOC_PETRA_SW_DB_CFG_VERSION_INVALID;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_CFG_VERSION_GET);

  SOC_SAND_CHECK_NULL_INPUT(sw_db_version);

  if (
      (soc_sand_ver >= SOC_PETRA_SSR_BASE_SAND_VER) &&
      (soc_sand_ver <= soc_get_sand_ver())
     )
  {
    sw_db_ver_num = SOC_PETRA_SW_DB_CFG_VERSION_NUM_0;
  }

  if (
      (sw_db_ver_num <= SOC_PETRA_SW_DB_CFG_VERSION_INVALID) ||
      (sw_db_ver_num > SOC_PETRA_SW_DB_CFG_NOF_VALID_VERSIONS)
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SSR_INCOMPATIBLE_SRC_VERSION_ERR, 1, exit);
  }

  *sw_db_version = sw_db_ver_num;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_get_cfg_version", 0, 0);
}

uint32
  soc_petra_ssr_transform_db2curr(
      SOC_SAND_IN  uint8                      *source_data,
      SOC_SAND_IN  SOC_PETRA_SW_DB_CFG_VERSION_NUM source_ver,
      SOC_SAND_OUT uint8                      *dest_data,
      SOC_SAND_IN  SOC_PETRA_SW_DB_CFG_VERSION_NUM dest_ver
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint8
    *buffer = NULL;
  SOC_PETRA_SW_DB_CFG_VERSION_NUM
    ver_idx = 0;
  uint32
    max_data_size,
    data_size = 0;
  uint32
    res = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_TRANSFORM_DB2CURR);

  SOC_SAND_CHECK_NULL_INPUT(source_data);
  SOC_SAND_CHECK_NULL_INPUT(dest_data);

  res = soc_petra_ssr_max_sw_db_size_get(
          &max_data_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  SOC_PETRA_ALLOC_ANY_SIZE(buffer, uint8, max_data_size);

  /* } Copy the source buffer to the allocated buffer { */
  res = Soc_petra_ssr_version_services_array[source_ver].sw_db_size_func(
          source_ver,
          &data_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  ret = SOC_SAND_OK; sal_memcpy(
          buffer,
          source_data,
          data_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 2, exit);

  /* } Suppose version changes are needed, do them now { */
  if (source_ver != dest_ver)
  {
    for (ver_idx = source_ver; ver_idx < dest_ver; ++ver_idx)
    {
      res = Soc_petra_ssr_version_services_array[ver_idx].transform_func(buffer, dest_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

      res = Soc_petra_ssr_version_services_array[ver_idx + 1].sw_db_size_func( (ver_idx + 1), &data_size);
      SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

      ret = SOC_SAND_OK; sal_memcpy(
              buffer,
              dest_data,
              data_size
            );
      SOC_SAND_CHECK_FUNC_RESULT(ret, 5, exit);
    }
  }

  /* } Copy the result to the destination buffer { */
  res = Soc_petra_ssr_version_services_array[source_ver].sw_db_size_func(
          dest_ver,
          &data_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

  ret = SOC_SAND_OK; sal_memcpy(
          dest_data,
          buffer,
          data_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 2, exit);
  /* } Finished */

exit:
  SOC_PETRA_FREE_ANY_SIZE(buffer);

  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_transform_db2curr", 0, 0);
}

uint32
  soc_petra_ssr_data_load(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN uint8         *ssr_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_DATA_LOAD);

  SOC_SAND_CHECK_NULL_INPUT(ssr_data);

  res = soc_petra_ssr_PETRA_SW_DB_SSR_V01_data_load(
          unit,
          ssr_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_data_load", 0, 0);
}

uint32
  soc_petra_ssr_data_save(
    SOC_SAND_IN  int        unit,
    SOC_SAND_OUT SOC_PETRA_SSR_DATA  *ssr_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_DATA_SAVE);

  SOC_SAND_CHECK_NULL_INPUT(ssr_data);

  res = soc_petra_ssr_PETRA_SW_DB_SSR_V01_data_save(
          unit,
          &(ssr_data->ssr_v01)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_data_save_unsafe", 0, 0);
}

uint32
  soc_petra_ssr_to_buff_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_OUT uint8        *store_buff,
    SOC_SAND_IN  uint32       buff_byte_size
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_SSR_BUFF
    *curr_ssr_buff = NULL;
  uint32
    fap_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_BUFFER_SAVE);

  SOC_SAND_CHECK_NULL_INPUT(store_buff);

  if(buff_byte_size != sizeof(SOC_PETRA_SSR_BUFF))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SW_DB_BUFF_SIZE_MISMATCH_ERR, 2, exit);
  }

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &fap_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  curr_ssr_buff = (SOC_PETRA_SSR_BUFF *)store_buff;

  curr_ssr_buff->header.buffer_size  = buff_byte_size;
  curr_ssr_buff->header.soc_sand_version = soc_get_sand_ver();

  curr_ssr_buff->device_header.fap_id = fap_id;
  curr_ssr_buff->device_header.prev_unit = unit;

  res = soc_petra_ssr_data_save(
          unit,
          &(curr_ssr_buff->info.ssr_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_to_buff", 0, 0);
}

uint32
  soc_petra_ssr_from_buff_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint8        *store_buff,
    SOC_SAND_IN  uint32       buff_byte_size
  )
{
  uint32
    res = SOC_SAND_OK;
  const SOC_PETRA_SSR_BUFF
    *saved_ssr_buff = NULL;
  uint8
    *curr_ssr_data  = NULL;
  uint32
    sw_db_size = 0,
    saved_sand_version = 0,
    curr_sand_version = 0;
  SOC_PETRA_SW_DB_CFG_VERSION_NUM
    saved_sw_db_version = SOC_PETRA_SW_DB_CFG_VERSION_INVALID,
    curr_sw_db_version = SOC_PETRA_SW_DB_CFG_VERSION_INVALID;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_BUFFER_LOAD);

  SOC_SAND_CHECK_NULL_INPUT(store_buff);

  saved_ssr_buff = (const SOC_PETRA_SSR_BUFF *)store_buff;

  if(buff_byte_size != saved_ssr_buff->header.buffer_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SW_DB_BUFF_SIZE_MISMATCH_ERR, 1, exit);
  }

  res = soc_sand_ssr_get_ver_from_header(
          &saved_ssr_buff->header,
          &saved_sand_version
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  res = soc_petra_ssr_cfg_version_get(
          saved_sand_version,
          &saved_sw_db_version
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

  curr_sand_version = soc_get_sand_ver();

  res = soc_petra_ssr_cfg_version_get(
          curr_sand_version,
          &curr_sw_db_version
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  if (saved_sw_db_version == curr_sw_db_version)
  {
    res = soc_petra_ssr_data_load(
            unit,
            saved_ssr_buff->info.stream
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  else /* saved_sw_db_version != curr_sw_db_version { */
  {
    res = soc_petra_ssr_sw_db_size_get(
            curr_sw_db_version,
            &sw_db_size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_PETRA_ALLOC_ANY_SIZE(curr_ssr_data, uint8, sw_db_size);

    res = soc_petra_ssr_transform_db2curr(
            saved_ssr_buff->info.stream,
            saved_sw_db_version,
            curr_ssr_data,
            curr_sw_db_version
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    res = soc_petra_ssr_data_load(
            unit,
            curr_ssr_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);
  } /* } saved_sw_db_version != curr_sw_db_version */

exit:
  SOC_PETRA_FREE_ANY_SIZE(curr_ssr_data);
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_petra_ssr_from_buff", 0, 0);
}

/*********************************************************************
*     Serves as a hot-start indication. The application can
*     call this API upon initialization, after the device
*     registration and before the driver initialization
*     sequence (any stage before init_phase1) to check if the
*     device initialization was already performed, meaning
*     this is a hot-start.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ssr_is_device_init_done_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *is_init_done
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  uint8
    is_done = FALSE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SSR_IS_DEVICE_INIT_DONE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(is_init_done);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->fdr.fdr_enablers_reg, fld_val, 5, exit);
  
  if(fld_val != SOC_PETRA_MGMT_FDR_TRFC_DISABLE)
  {
    is_done = TRUE;
  }
  else
  {
    /* Handle the case that the traffic is disabled, but the device is initialized */
    res = soc_petra_mgmt_ver_get(
            unit,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    is_done = SOC_SAND_NUM2BOOL(fld_val != 0);
  }

  *is_init_done = is_done;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ssr_is_device_init_done_unsafe()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
