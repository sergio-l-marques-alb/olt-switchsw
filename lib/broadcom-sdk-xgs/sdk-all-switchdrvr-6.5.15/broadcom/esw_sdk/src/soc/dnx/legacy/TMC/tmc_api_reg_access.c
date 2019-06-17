/* $Id: jer2_tmc_api_reg_access.c,v 1.6 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
/******************************************************************
*
* FILENAME:       soc_jer2_tmcreg_access.c
*
* MODULE PREFIX:  soc_jer2_tmcreg_access
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
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_GENERAL

/*************
 * INCLUDES  *
 *************/
/* { */


#include <shared/bsl.h>


#include <soc/dnx/legacy/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dnx/legacy/SAND/SAND_FM/sand_trigger.h>
#include <soc/dnx/legacy/SAND/Utils/sand_bitstream.h>
#include <soc/dnx/legacy/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>

#include <soc/dnx/legacy/TMC/tmc_api_reg_access.h>
#include <soc/dnx/legacy/TMC/tmc_api_general.h>

#include <soc/dnxc/legacy/error.h>

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
*  soc_jer2_tmcfield_from_reg_get
*TYPE:
*  PROC
*DATE:
*  01/10/2007
*FUNCTION:
*  Gets field bits from an input buffer and puts them
*  in the output buffer
*INPUT:
*  DNX_SAND_IN  uint32          *reg_buffer,
*    Input buffer from which the function reads -
*    the register to read
*  DNX_SAND_IN  SOC_DNX_REG_FIELD  *field,
*    The field from which the bits are taken
*  DNX_SAND_IN  uint32          *fld_buffer
*    Output buffer to which the function writes -
*    the field to write.
*OUTPUT:
*REMARKS:
*  The fld_buffer is zerrowed before writing to it.
*  In case of a 32-bit buffer (single register),
*  the return value will not be dependant on the previous
*  value of the buffer. Otherwise - if the buffer size
*  is larger then the field size,
*  it should be zerrowed by the user.
*****************************************************/
int
  soc_jer2_tmcfield_from_reg_get(
    DNX_SAND_IN  uint32                                *reg_buffer,
    DNX_SAND_IN  SOC_DNX_REG_FIELD                           *field,
    DNX_SAND_OUT uint32                                *fld_buffer
  )
{

  uint32
    out_buff = 0,
    res = _SHR_E_NONE;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;

  SHR_NULL_CHECK(reg_buffer, _SHR_E_PARAM, "reg_buffer");
  SHR_NULL_CHECK(field, _SHR_E_PARAM, "field");
  SHR_NULL_CHECK(fld_buffer, _SHR_E_PARAM, "fld_buffer");

  res = dnx_sand_bitstream_get_any_field(
    reg_buffer,
    SOC_DNX_FLD_LSB(*field),
    SOC_DNX_FLD_NOF_BITS(*field),
    &out_buff
    );
  SHR_IF_ERR_EXIT(res);

  *fld_buffer = out_buff;

exit:
  SHR_FUNC_EXIT;
}

/*****************************************************
*NAME
*  soc_jer2_tmcfield_from_reg_set
*TYPE:
*  PROC
*DATE:
*  01/10/2007
*FUNCTION:
*  Sets field bits in an output buffer after read from
*  an input buffer.
*INPUT:
*  DNX_SAND_IN  uint32          *fld_buffer,
*    Input buffer from which the function reads -
*    the field to set.
*  DNX_SAND_IN  SOC_DNX_REG_FIELD  *field,
*    The field from which the bits are taken
*  DNX_SAND_IN  uint32          *reg_buffer
*    Output buffer to which the function writes -
*    the register to set with the field
*OUTPUT:
* None.
*REMARKS:
* Only the relevant bits in the output reg_buffer are changed -
* the rest is left as is.
*
*****************************************************/
int
  soc_jer2_tmcfield_from_reg_set(
    DNX_SAND_IN  uint32                                *fld_buffer,
    DNX_SAND_IN  SOC_DNX_REG_FIELD                           *field,
    DNX_SAND_OUT uint32                                *reg_buffer
  )
{
  uint32
    res = _SHR_E_NONE;

  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;

  SHR_NULL_CHECK(fld_buffer, _SHR_E_PARAM, "fld_buffer");
  SHR_NULL_CHECK(field, _SHR_E_PARAM, "field");
  SHR_NULL_CHECK(reg_buffer, _SHR_E_PARAM, "reg_buffer");

  res = dnx_sand_bitstream_set_any_field(
    fld_buffer,
    SOC_DNX_FLD_LSB(*field),
    SOC_DNX_FLD_NOF_BITS(*field),
    reg_buffer
    );
  SHR_IF_ERR_EXIT(res);

exit:
  SHR_FUNC_EXIT;
}

void
  SOC_DNX_REG_ADDR_clear(
    DNX_SAND_OUT SOC_DNX_REG_ADDR *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_REG_ADDR));
  info->base = 0;
  info->step = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_REG_FIELD_clear(
    DNX_SAND_OUT SOC_DNX_REG_FIELD *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_REG_FIELD));
  SOC_DNX_REG_ADDR_clear(&(info->addr));
  info->msb = 0;
  info->lsb = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_REG_INFO_clear(
    DNX_SAND_OUT SOC_DNX_REG_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_REG_INFO));
  SOC_DNX_REG_ADDR_clear(&(info->addr));
  info->val = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_POLL_INFO_clear(
    DNX_SAND_OUT SOC_DNX_POLL_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  sal_memset(info, 0x0, sizeof(SOC_DNX_POLL_INFO));
  info->expected_value = 0;
  info->busy_wait_nof_iters = 0;
  info->timer_nof_iters = 0;
  info->timer_delay_msec = 0;
exit:
  SHR_VOID_FUNC_EXIT;
}


void
  SOC_DNX_REG_ADDR_print(
    DNX_SAND_IN  SOC_DNX_REG_ADDR *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "base: %u[Bytes]\n\r"),info->base));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "step: %u[Bytes]\n\r"),info->step));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_REG_FIELD_print(
    DNX_SAND_IN  SOC_DNX_REG_FIELD *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "addr:")));
  SOC_DNX_REG_ADDR_print(&(info->addr));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "msb: %u\n\r"), info->msb));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "lsb: %u\n\r"), info->lsb));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_REG_INFO_print(
    DNX_SAND_IN  SOC_DNX_REG_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "addr:")));
  SOC_DNX_REG_ADDR_print(&(info->addr));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "val: %u\n\r"),info->val));
exit:
  SHR_VOID_FUNC_EXIT;
}

void
  SOC_DNX_POLL_INFO_print(
    DNX_SAND_IN  SOC_DNX_POLL_INFO *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
    DNXC_LEGACY_FIXME_ASSERT;
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  LOG_CLI((BSL_META_U(NO_UNIT,
                      "expected_value: %u\n\r"),info->expected_value));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "busy_wait_nof_iters: %u\n\r"),info->busy_wait_nof_iters));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "timer_nof_iters: %u\n\r"),info->timer_nof_iters));
  LOG_CLI((BSL_META_U(NO_UNIT,
                      "timer_delay_msec: %u\n\r"),info->timer_delay_msec));
exit:
  SHR_VOID_FUNC_EXIT;
}


/* } */


