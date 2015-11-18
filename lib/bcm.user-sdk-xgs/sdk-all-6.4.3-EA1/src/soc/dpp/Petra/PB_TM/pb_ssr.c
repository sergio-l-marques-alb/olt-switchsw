/* $Id: pb_ssr.c,v 1.8 Broadcom SDK $
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

#include <soc/dpp/SAND/Management/sand_ssr.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_ssr.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_ssr.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ssr.h>
#include <soc/dpp/Petra/PB_TM/pb_general.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>

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
    (*SOC_PB_SSR_TRANSFORM_FUNC)(
      SOC_SAND_IN  uint8*,
      SOC_SAND_OUT uint8*
    );

typedef
  uint32
    (*SOC_PB_SSR_SW_DB_SIZE_FUNC)(
      SOC_SAND_IN  SOC_PB_SW_DB_CFG_VERSION_NUM,
      SOC_SAND_OUT uint32*);

typedef struct
{
  SOC_PB_SSR_TRANSFORM_FUNC   transform_func;

  SOC_PB_SSR_SW_DB_SIZE_FUNC  sw_db_size_func;

} SOC_PB_SSR_VERSION_SERVICES;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

uint32
  soc_pb_ssr_forbidden_ver_trans(
    SOC_SAND_IN   uint8                     *source_data,
    SOC_SAND_OUT  uint8                     *dest_data
  );

uint32
  soc_pb_ssr_forbidden_ver_size(
    SOC_SAND_IN SOC_PB_SW_DB_CFG_VERSION_NUM      sw_db_ver,
    SOC_SAND_OUT uint32                     *sw_db_data_size
  );

uint32
  soc_pb_ssr_get_sw_db_size_v01(
    SOC_SAND_IN SOC_PB_SW_DB_CFG_VERSION_NUM ,
    SOC_SAND_OUT uint32*
  );

uint32
  soc_pb_ssr_transform_v01_v02(
    SOC_SAND_IN unsigned char*,
    SOC_SAND_OUT unsigned char*
  );

uint32
  soc_pb_ssr_get_sw_db_size_v02(
    SOC_SAND_IN SOC_PB_SW_DB_CFG_VERSION_NUM ,
    SOC_SAND_OUT uint32*
  );

static
  SOC_PB_SSR_VERSION_SERVICES
    Soc_pb_ssr_version_services_array[SOC_PB_SW_DB_CFG_NOF_VALID_VERSIONS + 1] =
    {
      {soc_pb_ssr_forbidden_ver_trans,     soc_pb_ssr_forbidden_ver_size}, /* version 0 is not valid */
      {soc_pb_ssr_transform_v01_v02,       soc_pb_ssr_get_sw_db_size_v01}, /* can not transform last version to next */
      {soc_pb_ssr_forbidden_ver_trans,     soc_pb_ssr_get_sw_db_size_v02}
    };

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_ssr_forbidden_ver_size(
    SOC_SAND_IN  SOC_PB_SW_DB_CFG_VERSION_NUM      sw_db_ver,
    SOC_SAND_OUT uint32                      *sw_db_data_size
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_FORBIDDEN_VER_SIZE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_SSR_FORBIDDEN_FUNCTION_CALL_ERR, 1, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_forbidden_ver_size", 0, 0);
}

uint32
  soc_pb_ssr_forbidden_ver_trans(
    SOC_SAND_IN   uint8                     *source_data,
    SOC_SAND_OUT  uint8                     *dest_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_FORBIDDEN_VER_TRANS);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_SSR_FORBIDDEN_FUNCTION_CALL_ERR, 1, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_forbidden_ver_trans", 0, 0);
}

uint32
  soc_pb_ssr_max_sw_db_size_get(
    SOC_SAND_OUT  uint32   *size
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    max_size = 0;
    /*ver_size = 0;
  uint32
    ver_idx = 0;*/

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_MAX_SW_DB_SIZE_GET);

  max_size = 0xfffff;

  *size = max_size;
  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_max_sw_db_size_get", 0, 0);
}

uint32
  soc_pb_ssr_sw_db_size_get(
    SOC_SAND_IN  uint32  ver_idx,
    SOC_SAND_OUT uint32   *size
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ver_size = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_SW_DB_SIZE_GET);

  res = Soc_pb_ssr_version_services_array[ver_idx].sw_db_size_func(
          ver_idx,
          &ver_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

  *size = ver_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_sw_db_size_get", 0, 0);
}


uint32
  soc_pb_ssr_get_sw_db_size_v01(
    SOC_SAND_IN SOC_PB_SW_DB_CFG_VERSION_NUM sw_db_ver,
    SOC_SAND_OUT uint32* sw_db_data_size
  )
{
  /* error handling */
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if ( (sw_db_ver <= SOC_PB_SW_DB_CFG_VERSION_INVALID) ||
       (sw_db_ver > SOC_PB_SW_DB_CFG_NOF_VALID_VERSIONS))
  {
    SOC_SAND_SET_ERROR_CODE(0, 1, exit);
  }

  *sw_db_data_size = 0xfffff;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("fap20v_ssr_get_sw_db_size_v01", 0, 0);
}

uint32
  soc_pb_ssr_get_sw_db_size_v02(
    SOC_SAND_IN SOC_PB_SW_DB_CFG_VERSION_NUM sw_db_ver,
    SOC_SAND_OUT uint32* sw_db_data_size
  )
{
  /* error handling */
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if ( (sw_db_ver <= SOC_PB_SW_DB_CFG_VERSION_INVALID) ||
       (sw_db_ver > SOC_PB_SW_DB_CFG_NOF_VALID_VERSIONS))
  {
    SOC_SAND_SET_ERROR_CODE(0, 1, exit);
  }

  *sw_db_data_size = 0xfffff;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("fap20v_ssr_get_sw_db_size_v01", 0, 0);
}


uint32
  soc_pb_ssr_cfg_version_get(
    SOC_SAND_IN   uint32                      soc_sand_ver,
    SOC_SAND_OUT  SOC_PB_SW_DB_CFG_VERSION_NUM  *sw_db_version
  )
{
  SOC_PB_SW_DB_CFG_VERSION_NUM
    sw_db_ver_num = SOC_PB_SW_DB_CFG_VERSION_INVALID;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_CFG_VERSION_GET);

  SOC_SAND_CHECK_NULL_INPUT(sw_db_version);

  if (
      (soc_sand_ver >= SOC_PB_SSR_BASE_SAND_VER) &&
      (soc_sand_ver <= soc_get_sand_ver())
     )
  {
    sw_db_ver_num = SOC_PB_SW_DB_CFG_VERSION_NUM_0;
  }

  if (
      (sw_db_ver_num <= SOC_PB_SW_DB_CFG_VERSION_INVALID) ||
      (sw_db_ver_num > SOC_PB_SW_DB_CFG_NOF_VALID_VERSIONS)
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_SSR_INCOMPATIBLE_SRC_VERSION_ERR, 1, exit);
  }

  *sw_db_version = sw_db_ver_num;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_get_cfg_version", 0, 0);
}

uint32
  soc_pb_ssr_transform_db2curr(
      SOC_SAND_IN  uint8                        *full_source_ssr_stream,
      SOC_SAND_IN  SOC_PB_SW_DB_CFG_VERSION_NUM       source_ver,
      SOC_SAND_IN  uint32                       source_data_size,
      SOC_SAND_OUT uint8                        *dest_data,
      SOC_SAND_IN  SOC_PB_SW_DB_CFG_VERSION_NUM       dest_ver
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint8
    *buffer = NULL;
  SOC_PB_SW_DB_CFG_VERSION_NUM
    ver_idx = 0;
  uint32
    max_data_size,
    data_size = 0;
  uint32
    res = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_TRANSFORM_DB2CURR);

  SOC_SAND_CHECK_NULL_INPUT(full_source_ssr_stream);
  SOC_SAND_CHECK_NULL_INPUT(dest_data);

  res = soc_pb_ssr_max_sw_db_size_get(
          &max_data_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  SOC_PETRA_ALLOC_ANY_SIZE(buffer, uint8, max_data_size);

  ret = SOC_SAND_OK; sal_memcpy(
          buffer,
          full_source_ssr_stream,
          source_data_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 2, exit);

  /* } Suppose version changes are needed, do them now { */
  /* incrementally apply changes from version to version up to current version */
  if (source_ver != dest_ver)
  {
    for (ver_idx = source_ver; ver_idx < dest_ver; ++ver_idx)
    {
      res = Soc_pb_ssr_version_services_array[ver_idx].transform_func(buffer, dest_data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

      res = Soc_pb_ssr_version_services_array[ver_idx + 1].sw_db_size_func( (ver_idx + 1), &data_size);
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
  res = Soc_pb_ssr_version_services_array[source_ver].sw_db_size_func(
          dest_ver,
          &data_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

  ret = SOC_SAND_OK; sal_memcpy(
          dest_data,
          buffer,
          data_size /*suppose to be fffff for now*/
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 2, exit);
  /* } Finished */

exit:
  SOC_PETRA_FREE_ANY_SIZE(buffer);

  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_transform_db2curr", 0, 0);
}

uint32
  soc_pb_ssr_data_load(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN uint8         *ssr_data,
    SOC_SAND_IN uint8         *additional_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_DATA_LOAD);

  SOC_SAND_CHECK_NULL_INPUT(ssr_data);

  res = soc_pb_ssr_PB_SW_DB_SSR_data_load(
          unit,
          ssr_data,
          sizeof(SOC_PB_SSR_BUFF),
          additional_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_data_load", 0, 0);
}

STATIC uint32
  soc_pb_ssr_data_save(
    SOC_SAND_IN  int          unit,
    SOC_SAND_OUT SOC_PB_SSR_DATA    *ssr_data,
    SOC_SAND_OUT uint8          *additional_data,
    SOC_SAND_IN  uint32          additional_data_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_DATA_SAVE);

  SOC_SAND_CHECK_NULL_INPUT(ssr_data);

  res = soc_pb_ssr_PB_SW_DB_SSR_data_save(
          unit,
          &(ssr_data->ssr_v01),
          additional_data,
          additional_data_size);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_data_save_unsafe", 0, 0);
}

uint32
  soc_pb_ssr_to_buff_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_OUT uint8        *store_buff,
    SOC_SAND_IN  uint32       buff_byte_size
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_SSR_BUFF
    *curr_ssr_buff = NULL;
  uint32
    fap_id;
  uint8
    *additional_buff;
  uint32
      additional_buff_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_BUFFER_SAVE);

  SOC_SAND_CHECK_NULL_INPUT(store_buff);

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
          unit,
          &fap_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  if(sizeof(SOC_PB_SSR_BUFF) > buff_byte_size) {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_WARMBOOT_SCACHE_SIZE_TOO_SMALL_ERR, 5, exit);        
  }

  curr_ssr_buff = (SOC_PB_SSR_BUFF *)store_buff;

  curr_ssr_buff->header.buffer_size  = buff_byte_size;
  curr_ssr_buff->header.soc_sand_version = soc_get_sand_ver();

  curr_ssr_buff->device_header.fap_id = fap_id;
  curr_ssr_buff->device_header.prev_unit = unit;


  additional_buff = store_buff + sizeof(SOC_PB_SSR_BUFF);
  
  additional_buff_size = buff_byte_size - sizeof(SOC_PB_SSR_BUFF);

  res = soc_pb_ssr_data_save(unit,
                         &(curr_ssr_buff->info.ssr_data),
                         additional_buff, /*pointer to additional data buff*/
                         additional_buff_size);
  SOC_SAND_CHECK_FUNC_RESULT(res, 1, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_to_buff", 0, 0);
}

uint32
  soc_pb_ssr_from_buff_unsafe(
    SOC_SAND_IN  int      unit,
    SOC_SAND_IN  uint8        *store_buff,
    SOC_SAND_IN  uint32       buff_byte_size
  )
{
  uint32
    res = SOC_SAND_OK;
  const SOC_PB_SSR_BUFF
    *saved_ssr_buff = NULL;
  uint8
    *curr_ssr_data  = NULL;
  const uint8
    *additional_data;
  uint32
    sw_db_size = 0,
    saved_sand_version = 0,
    curr_sand_version = 0;
  SOC_PB_SW_DB_CFG_VERSION_NUM
    saved_sw_db_version = SOC_PB_SW_DB_CFG_VERSION_INVALID,
    curr_sw_db_version = SOC_PB_SW_DB_CFG_VERSION_INVALID;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_BUFFER_LOAD);

  SOC_SAND_CHECK_NULL_INPUT(store_buff);

  saved_ssr_buff = (const SOC_PB_SSR_BUFF *)store_buff;

  if(buff_byte_size != saved_ssr_buff->header.buffer_size)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_SW_DB_BUFF_SIZE_MISMATCH_ERR, 1, exit);
  }

  res = soc_sand_ssr_get_ver_from_header(
          &saved_ssr_buff->header,
          &saved_sand_version
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  res = soc_pb_ssr_cfg_version_get(
          saved_sand_version,
          &saved_sw_db_version
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

  curr_sand_version = soc_get_sand_ver();

  res = soc_pb_ssr_cfg_version_get(
          curr_sand_version,
          &curr_sw_db_version
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

  if (saved_sw_db_version == curr_sw_db_version)
  {
    additional_data = store_buff + sizeof(SOC_PB_SSR_BUFF);

    res = soc_pb_ssr_data_load(
            unit,
            saved_ssr_buff->info.stream,
            additional_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }
  else /* } saved_sw_db_version != curr_sw_db_version { */
  {
    res = soc_pb_ssr_sw_db_size_get(
            curr_sw_db_version,
            &sw_db_size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_PETRA_ALLOC_ANY_SIZE(curr_ssr_data, uint8, sw_db_size);

    res = soc_pb_ssr_transform_db2curr(
            store_buff,
            saved_sw_db_version,
            saved_ssr_buff->header.buffer_size,
            curr_ssr_data,
            curr_sw_db_version
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    additional_data = curr_ssr_data + sizeof(SOC_PB_SSR_BUFF);
    saved_ssr_buff = (const SOC_PB_SSR_BUFF *)curr_ssr_data;

    res = soc_pb_ssr_data_load(
            unit,
            saved_ssr_buff->info.stream,
            additional_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

  } /* } saved_sw_db_version != curr_sw_db_version */

exit:
  SOC_PETRA_FREE_ANY_SIZE(curr_ssr_data);
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_from_buff", 0, 0);
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
  soc_pb_ssr_is_device_init_done_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *is_init_done
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SSR_IS_DEVICE_INIT_DONE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(is_init_done);

  regs = soc_petra_regs();
  SOC_PETRA_REG_GET(regs->fdr.fdr_enablers_reg, fld_val, 5, exit);
  
  if(
      (fld_val == SOC_PETRA_MGMT_DEVICE_WAS_INITIALIZED_1) ||
      (fld_val == SOC_PETRA_MGMT_DEVICE_WAS_INITIALIZED_2) ||
      (fld_val == SOC_PETRA_MGMT_DEVICE_WAS_INITIALIZED_3) ||
      (fld_val == SOC_PETRA_MGMT_DEVICE_WAS_INITIALIZED_4)
    )
  {
    *is_init_done = TRUE;
  }
  else
  {
    SOC_PETRA_FLD_GET(regs->eci.spare_flags_reg.hot_init_done, fld_val, 30, exit);

    if(fld_val == 0x1)
    {
      *is_init_done = TRUE;
    }
    else
    {
      *is_init_done = FALSE;
    }
  }
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_ssr_is_device_init_done_unsafe()",0,0);
}

uint32
  soc_pb_ssr_transform_v01_v02(
      SOC_SAND_IN   unsigned char* source_data,
      SOC_SAND_OUT  unsigned char* dest_data
  )
{
  uint32
    pp_port_ndx,
    tcam_bank_indx;
  uint32
    res;
  uint32
    act_size,
    size;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  const SOC_PB_SW_DB_DEVICE_SSR_V01
    *source_ssr_data = NULL;
  SOC_PB_SW_DB_DEVICE_SSR_V02
    *ssr_data = NULL;
  const SOC_PB_SW_DB_TCAM_MGMT_INFO
    *tcam_info_source = NULL;
  SOC_PB_SW_DB_TCAM_MGMT_INFO
    *tcam_info_dest = NULL;
  const uint8
    *dest_ssr_stream,
    *source_ssr_stream,
    *additional_data_source = source_data + sizeof(SOC_PB_SSR_BUFF_V01);
  uint8
    *additional_data_dest = dest_data + sizeof(SOC_PB_SSR_BUFF_V02);
  const SOC_PB_SSR_BUFF_V01
    *saved_ssr_buff = NULL;
  SOC_PB_SSR_BUFF_V02
    *to_be_saved_ssr_buff = NULL;
  SOC_PETRA_SW_DB_DEVICE_SSR_V02
    *tmp_sw_db = NULL;

  /* error handling */
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(source_data);
  SOC_SAND_CHECK_NULL_INPUT(dest_data);

  saved_ssr_buff = (const SOC_PB_SSR_BUFF_V01 *)source_data;
  source_ssr_stream = saved_ssr_buff->info.stream;

  tcam_info_source = (const SOC_PB_SW_DB_TCAM_MGMT_INFO *) additional_data_source;

  additional_data_source += sizeof(SOC_PB_SW_DB_TCAM_MGMT_INFO);

  source_ssr_data = (const SOC_PB_SW_DB_DEVICE_SSR_V01*)source_ssr_stream;

  SOC_PETRA_ALLOC_ANY_SIZE(tmp_sw_db, SOC_PETRA_SW_DB_DEVICE_SSR_V02, 1);
  
  /****************************************************************************************************/
  /*  replicates device load of V01 but do it into V02 struct will add new field in save              */
  /****************************************************************************************************/

  /* copy whole db field by field */
  tmp_sw_db->op_mode = source_ssr_data->soc_petra_ssr.op_mode;
  tmp_sw_db->dev_version = source_ssr_data->soc_petra_ssr.dev_version;
  tmp_sw_db->fat_pipe = source_ssr_data->soc_petra_ssr.fat_pipe;
  tmp_sw_db->lag = source_ssr_data->soc_petra_ssr.lag;
  tmp_sw_db->soc_petra_sw_db_egr_ports = source_ssr_data->soc_petra_ssr.soc_petra_sw_db_egr_ports;
  tmp_sw_db->soc_petra_sw_db_scheduler = source_ssr_data->soc_petra_ssr.soc_petra_sw_db_scheduler;
  tmp_sw_db->soc_petra_sw_db_multicast = source_ssr_data->soc_petra_ssr.soc_petra_sw_db_multicast;
  tmp_sw_db->soc_petra_api_auto_queue_flow_mngmnt = source_ssr_data->soc_petra_ssr.soc_petra_api_auto_queue_flow_mngmnt;
  tmp_sw_db->serdes = source_ssr_data->soc_petra_ssr.serdes;
  tmp_sw_db->cell = source_ssr_data->soc_petra_ssr.cell;
  tmp_sw_db->lbg = source_ssr_data->soc_petra_ssr.lbg;
  tmp_sw_db->dram = source_ssr_data->soc_petra_ssr.dram;
  tmp_sw_db->qdr = source_ssr_data->soc_petra_ssr.qdr;
  tmp_sw_db->is_interrupt_masked_on = source_ssr_data->soc_petra_ssr.is_interrupt_masked_on;
  tmp_sw_db->mac = source_ssr_data->soc_petra_ssr.mac;
  tmp_sw_db->synce = source_ssr_data->soc_petra_ssr.synce;

  tmp_sw_db->tcam_info = NULL;
  SOC_PETRA_ALLOC_ANY_SIZE(tmp_sw_db->tcam_info, SOC_PB_SW_DB_TCAM_MGMT_INFO, 1);

  *(tmp_sw_db->tcam_info) = *tcam_info_source;
  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    res = soc_sand_occ_bm_load(
            &additional_data_source,
            &(tmp_sw_db->tcam_info->banks[tcam_bank_indx].entries_use)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  tmp_sw_db->tcam_db = source_ssr_data->soc_petra_ssr.tcam_db;
  tmp_sw_db->tcam = source_ssr_data->soc_petra_ssr.tcam;
  tmp_sw_db->tcam_mgmt = source_ssr_data->soc_petra_ssr.tcam_mgmt;
  
  tmp_sw_db->frwrd_ip = source_ssr_data->soc_petra_ssr.frwrd_ip;
  res = soc_sand_hash_table_load(
          &additional_data_source,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          NULL,
          NULL,
          &tmp_sw_db->frwrd_ip.route_key_to_entry_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  tmp_sw_db->soc_pb_lbg = source_ssr_data->soc_petra_ssr.soc_pb_lbg;
  tmp_sw_db->soc_pb_multicast = source_ssr_data->soc_petra_ssr.soc_pb_multicast;
  
  tmp_sw_db->egr_editor = source_ssr_data->soc_petra_ssr.egr_editor;
  res = soc_sand_multi_set_load(
          &additional_data_source,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &tmp_sw_db->egr_editor.egr_editor_profiles_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  res = soc_sand_multi_set_load(
          &additional_data_source,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &tmp_sw_db->egr_editor.egr_editor_stk_prune_bmps_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  tmp_sw_db->egr_acl = source_ssr_data->soc_petra_ssr.egr_acl;

  tmp_sw_db->core_freq_hi_res.self_freq = source_ssr_data->soc_petra_ssr.core_freq_hi_res.self_freq;
  tmp_sw_db->core_freq_hi_res.min_fap_freq_in_system = source_ssr_data->soc_petra_ssr.core_freq_hi_res.min_fap_freq_in_system;

  tmp_sw_db->pmf = source_ssr_data->soc_petra_ssr.pmf;
  res = soc_sand_multi_set_load(
          &additional_data_source,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &tmp_sw_db->pmf.header_profiles_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  res = soc_sand_multi_set_load(
          &additional_data_source,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &tmp_sw_db->pmf.pp_port_profiles_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  res = soc_sand_multi_set_load(
          &additional_data_source,
          soc_pb_sw_db_buffer_set_entry,
          soc_pb_sw_db_buffer_get_entry,
          &tmp_sw_db->pmf.pgm_profiles_multi_set
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  tmp_sw_db->soc_pb_egr_mirror = source_ssr_data->soc_petra_ssr.soc_pb_egr_mirror;
  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; pp_port_ndx++)
  {
    tmp_sw_db->port[pp_port_ndx] = source_ssr_data->soc_petra_ssr.port[pp_port_ndx];
  }

  SOC_SAND_CHECK_FUNC_RESULT(ret, 90, exit);


  /************************************************************************/
  /*    replicates save for V02 based on V01 with one added field         */
  /************************************************************************/

  to_be_saved_ssr_buff = (SOC_PB_SSR_BUFF_V02 *)dest_data;
  dest_ssr_stream = to_be_saved_ssr_buff->info.stream;
  ssr_data = (SOC_PB_SW_DB_DEVICE_SSR_V02*)dest_ssr_stream;

  tcam_info_dest = (SOC_PB_SW_DB_TCAM_MGMT_INFO *)additional_data_dest;
  additional_data_dest += sizeof(SOC_PB_SW_DB_TCAM_MGMT_INFO);

  if (tmp_sw_db == NULL)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_NULL_POINTER_ERR, 10, exit);
  }

  /* copy whole db field by field */
  ssr_data->soc_petra_ssr.op_mode = tmp_sw_db->op_mode;
  ssr_data->soc_petra_ssr.dev_version = tmp_sw_db->dev_version;
  ssr_data->soc_petra_ssr.fat_pipe = tmp_sw_db->fat_pipe;
  ssr_data->soc_petra_ssr.lag = tmp_sw_db->lag;
  ssr_data->soc_petra_ssr.soc_petra_sw_db_egr_ports = tmp_sw_db->soc_petra_sw_db_egr_ports;
  ssr_data->soc_petra_ssr.soc_petra_sw_db_scheduler = tmp_sw_db->soc_petra_sw_db_scheduler;
  ssr_data->soc_petra_ssr.soc_petra_sw_db_multicast = tmp_sw_db->soc_petra_sw_db_multicast;
  ssr_data->soc_petra_ssr.soc_petra_api_auto_queue_flow_mngmnt = tmp_sw_db->soc_petra_api_auto_queue_flow_mngmnt;
  ssr_data->soc_petra_ssr.serdes = tmp_sw_db->serdes;
  ssr_data->soc_petra_ssr.cell = tmp_sw_db->cell;
  ssr_data->soc_petra_ssr.lbg = tmp_sw_db->lbg;
  ssr_data->soc_petra_ssr.dram = tmp_sw_db->dram;
  ssr_data->soc_petra_ssr.qdr = tmp_sw_db->qdr;
  ssr_data->soc_petra_ssr.is_interrupt_masked_on = tmp_sw_db->is_interrupt_masked_on;
  ssr_data->soc_petra_ssr.mac = tmp_sw_db->mac;
  ssr_data->soc_petra_ssr.synce = tmp_sw_db->synce;
  ssr_data->soc_petra_ssr.tcam_info = 0x0;

  /* !!!!!!!! the difference between versions is this field !!!!! */
  ssr_data->soc_petra_ssr.new_field_for_test_ssr_version_update = 0x2109;

  *tcam_info_dest = *(tmp_sw_db->tcam_info);
  /* patch zeros in places that contain pointers. these pointers wont be needed in the restoration */
  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    sal_memset(&tcam_info_dest->banks[tcam_bank_indx].entries_use, 0x0, sizeof(SOC_SAND_OCC_BM_PTR));
  }

  for (tcam_bank_indx = 0; tcam_bank_indx <SOC_PB_TCAM_NOF_BANKS ; ++tcam_bank_indx )
  {
    res = soc_sand_occ_bm_get_size_for_save(
            tmp_sw_db->tcam_info->banks[tcam_bank_indx].entries_use,
            &size
          );

    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    res = soc_sand_occ_bm_save(
            tmp_sw_db->tcam_info->banks[tcam_bank_indx].entries_use,
            additional_data_dest,
            size,
            &act_size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    additional_data_dest += act_size;
  }

  ssr_data->soc_petra_ssr.tcam_db = tmp_sw_db->tcam_db;
  ssr_data->soc_petra_ssr.tcam = tmp_sw_db->tcam;
  ssr_data->soc_petra_ssr.tcam_mgmt = tmp_sw_db->tcam_mgmt;
    
  /*frwrd_ip*/
  ssr_data->soc_petra_ssr.frwrd_ip = tmp_sw_db->frwrd_ip;
  res = soc_sand_hash_table_get_size_for_save(
          &tmp_sw_db->frwrd_ip.route_key_to_entry_id,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  res = soc_sand_hash_table_save(
          &(tmp_sw_db->frwrd_ip.route_key_to_entry_id),
          additional_data_dest,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    
  additional_data_dest += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.frwrd_ip.route_key_to_entry_id, 0x0, sizeof(SOC_SAND_HASH_TABLE_INFO));

  ssr_data->soc_petra_ssr.soc_pb_lbg = tmp_sw_db->soc_pb_lbg;
  ssr_data->soc_petra_ssr.soc_pb_multicast = tmp_sw_db->soc_pb_multicast;

  /*egr_editor - 2 multiset*/
  ssr_data->soc_petra_ssr.egr_editor = tmp_sw_db->egr_editor;
  res = soc_sand_multi_set_get_size_for_save(
          &tmp_sw_db->egr_editor.egr_editor_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  res = soc_sand_multi_set_save(
          &tmp_sw_db->egr_editor.egr_editor_profiles_multi_set,
          additional_data_dest,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  
  additional_data_dest += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.egr_editor.egr_editor_profiles_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));
  res = soc_sand_multi_set_get_size_for_save(
          &tmp_sw_db->egr_editor.egr_editor_stk_prune_bmps_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  res = soc_sand_multi_set_save(
          &tmp_sw_db->egr_editor.egr_editor_stk_prune_bmps_set,
          additional_data_dest,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  additional_data_dest += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.egr_editor.egr_editor_stk_prune_bmps_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));

  ssr_data->soc_petra_ssr.egr_acl = tmp_sw_db->egr_acl;
  
  ssr_data->soc_petra_ssr.core_freq_hi_res.self_freq = tmp_sw_db->core_freq_hi_res.self_freq;
  ssr_data->soc_petra_ssr.core_freq_hi_res.min_fap_freq_in_system = tmp_sw_db->core_freq_hi_res.min_fap_freq_in_system;

  /*pmf  -  3 multi sets*/
  ssr_data->soc_petra_ssr.pmf = tmp_sw_db->pmf;
  res = soc_sand_multi_set_get_size_for_save(
          &tmp_sw_db->pmf.header_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  res = soc_sand_multi_set_save(
          &tmp_sw_db->pmf.header_profiles_multi_set,
          additional_data_dest,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  additional_data_dest += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.pmf.header_profiles_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));

  res = soc_sand_multi_set_get_size_for_save(
          &tmp_sw_db->pmf.pp_port_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  res = soc_sand_multi_set_save(
          &tmp_sw_db->pmf.pp_port_profiles_multi_set,
          additional_data_dest,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  additional_data_dest += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.pmf.pp_port_profiles_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));

  res = soc_sand_multi_set_get_size_for_save(
          &tmp_sw_db->pmf.pgm_profiles_multi_set,
          &size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  res = soc_sand_multi_set_save(
          &tmp_sw_db->pmf.pgm_profiles_multi_set,
          additional_data_dest,
          size,
          &act_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  additional_data_dest += act_size;
  sal_memset(&ssr_data->soc_petra_ssr.pmf.pgm_profiles_multi_set, 0x0, sizeof(SOC_SAND_MULTI_SET_INFO));

  ssr_data->soc_petra_ssr.soc_pb_egr_mirror = tmp_sw_db->soc_pb_egr_mirror;
  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; pp_port_ndx++)
  {
    ssr_data->soc_petra_ssr.port[pp_port_ndx] = tmp_sw_db->port[pp_port_ndx];
  }

exit:
  SOC_PETRA_FREE_ANY_SIZE(tmp_sw_db);
  SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_ssr_transform_v01_v02", 0, 0);
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
