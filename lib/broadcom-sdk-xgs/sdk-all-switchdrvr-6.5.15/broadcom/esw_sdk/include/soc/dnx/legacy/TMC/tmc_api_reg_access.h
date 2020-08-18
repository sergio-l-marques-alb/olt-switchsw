/* $Id: soc_jer2_jer2_jer2_tmcapi_reg_access.h,v 1.3 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef __SOC_DNX_API_REG_ACCESS_H_INCLUDED__
/* { */
#define __SOC_DNX_API_REG_ACCESS_H_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dnx/legacy/TMC/tmc_api_general.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_DNX_DEFAULT_INSTANCE   0xFF

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* $Id: soc_jer2_jer2_jer2_tmcapi_reg_access.h,v 1.3 Broadcom SDK $
* calculate address from block base address, block index
* and the index of that block (can be non-zero
* for blocks with multiple instances.
*/

/*
* Dnx_soc_sand field manipulations based on jer2_jer2_jer2_tmc fields
* (must contain valid msb and lsb) {
*/
#define SOC_DNX_FLD_NOF_BITS(fld)                                               \
  ((fld).msb - (fld).lsb + 1)
#define SOC_DNX_FLD_LSB(fld)                                                    \
  (fld).lsb
#define SOC_DNX_FLD_MASK(fld)                                                   \
  (uint32)((DNX_SAND_BIT((fld).msb) - DNX_SAND_BIT((fld).lsb)) + DNX_SAND_BIT((fld).msb))
#define SOC_DNX_FLD_SHIFT(fld)                                                  \
  (fld).lsb
#define SOC_DNX_FLD_MAX(fld)                                                    \
  (SOC_DNX_FLD_MASK(fld)>>SOC_DNX_FLD_SHIFT(fld))
/*
* Take value and put it in its proper location within a 'long'
* register (other bits are zeroed).
*/
/*
* Get a value out of location within a 'long' register (and make sure it
* is not effected by bits outside its predefined mask).
*/

/*
* Take value from buff, and split it to two buffers (used when a field is split in HW
*/

/*
* Build value from two fields (used when a field is split in HW
*/

/*
* Dnx_soc_sand field manipulations }
*/

/*
* Within an array of registers with identical fields-
* get the index of the register to access.
*/

/*
* Within an array of registers with identical fields-
* get the index of the field (inside the register) to access.
*/

/*
* Set a variable of type SOC_DNX_REG_FIELD
*/
#define SOC_DNX_FLD_DEFINE(fld_var,fld_offset,fld_size) \
{                                                     \
  fld_var.lsb = fld_offset;                           \
  fld_var.msb = (fld_offset) + (fld_size) - 1;            \
}

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
typedef struct
{
  /*
   *  Base address of the register in the device address space
   *  - relative to the device start address. Units: Bytes.
   */
  uint32 base;
  /*
   *  A device internal block (i.e. Network Interface) may
   *  have multiple instances with identical register
   *  structure, but starting at different offsets inside the
   *  device address space. This field is the offset between
   *  two adjacent instances of the device internal blocks of
   *  the same type. Units: Bytes.
   */
  uint16 step;

} SOC_DNX_REG_ADDR;

typedef struct
{
  /*
   *  Register address of the register this field belongs to.
   */
  SOC_DNX_REG_ADDR addr;
  /*
   *  Field Most Significant Bit in the register.
   */
  uint8 msb;
  /*
   *  Field Least Significant Bit in the register.
   */
  uint8 lsb;

} SOC_DNX_REG_FIELD;

typedef struct
{
  /*
   *  A structure containing the device register address
   *  information - base and step.
   */
  SOC_DNX_REG_ADDR addr;
  /*
   *  Value of the specified register.
   */
  uint32 val;

} SOC_DNX_REG_INFO;

typedef struct
{
  /*
   *  The expected value. The polling occurs until the polled
   *  field indicates the expected value, or until timeout.
   */
  uint32 expected_value;
  /*
   *  Number of busy-wait iterations to poll for the
   *  indication.
   */
  uint32 busy_wait_nof_iters;
  /*
   *  Number of timer-delayed iterations to poll for the
   *  indication.
   */
  uint32 timer_nof_iters;
  /*
   *  The minimal delay of each timer-delayed iteration, in
   *  milliseconds.
   */
  uint32 timer_delay_msec;

} SOC_DNX_POLL_INFO;

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
*  soc_jer2_jer2_jer2_tmcfield_from_reg_get
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
*****************************************************/
uint32
  soc_jer2_jer2_jer2_tmcfield_from_reg_get(
    DNX_SAND_IN  uint32          *reg_buffer,
    DNX_SAND_IN  SOC_DNX_REG_FIELD     *field,
    DNX_SAND_OUT uint32          *fld_buffer
  );

void
  SOC_DNX_REG_ADDR_clear(
    DNX_SAND_OUT SOC_DNX_REG_ADDR *info
  );

void
  SOC_DNX_REG_FIELD_clear(
    DNX_SAND_OUT SOC_DNX_REG_FIELD *info
  );

void
  SOC_DNX_REG_INFO_clear(
    DNX_SAND_OUT SOC_DNX_REG_INFO *info
  );

void
  SOC_DNX_POLL_INFO_clear(
    DNX_SAND_OUT SOC_DNX_POLL_INFO *info
  );


void
  SOC_DNX_REG_ADDR_print(
    DNX_SAND_IN  SOC_DNX_REG_ADDR *info
  );

void
  SOC_DNX_REG_FIELD_print(
    DNX_SAND_IN  SOC_DNX_REG_FIELD *info
  );

void
  SOC_DNX_REG_INFO_print(
    DNX_SAND_IN  SOC_DNX_REG_INFO *info
  );

void
  SOC_DNX_POLL_INFO_print(
    DNX_SAND_IN  SOC_DNX_POLL_INFO *info
  );


/* } */


/* } __SOC_DNX_API_REG_ACCESS_H_INCLUDED__*/
#endif
