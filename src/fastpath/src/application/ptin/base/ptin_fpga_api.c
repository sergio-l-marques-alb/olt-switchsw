/*********************************************************************
*
* (C) Copyright PT Inova��o S.A. 2014-2014
*
**********************************************************************
*
* @filename ptin_fpga_api.c
*
* @purpose API to get the status of the CPLD  register on the FPGA
*
* @component fpga
*
* @comments none
* 
* @create    06/06/2014
*
* @author    Marcio Daniel Melo
* 
* @end
**********************************************************************/

#include "ptin_fpga_api.h"

#ifdef MAP_CPLD

#if (PTIN_BOARD_IS_MATRIX)
/**************************************************************************
*
* @purpose  Verify if this Matrix is Currently in Active State.
*           (Only for Matrix board)
*
* @param    void
*
* @returns  TRUE or FALSE
*
* @comments 
*
* @end
*
*************************************************************************/
L7_uint8 ptin_fgpa_mx_is_matrixactive(void)
{
  return (cpld_map->reg.mx_is_active != 0);
}

/**
 * Check if current Matrix is the Working one (slot 1) 
 * (Only for Matrix board)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fgpa_mx_is_matrix_in_workingslot(void)
{
  return (cpld_map->reg.slot_id == 0);
}
#endif // (PTIN_BOARD_IS_MATRIX)

#if (PTIN_BOARD_IS_LINECARD)
/**
 * Check if Active Matrix is in Working slot 
 * (Only for linecards)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fgpa_lc_is_matrixactive_in_workingslot(void)
{
  return ((cpld_map->reg.slot_matrix >> 4) & 1);
}
#endif // (PTIN_BOARD_IS_LINECARD)

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)
/**
 * Get slot position of Active Matrix 
 * (For all cards) 
 *  
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fgpa_matrixActive_slot(void)
{
  L7_uint8 slot = 0;
  L7_uint8 working_slot    = PTIN_SYS_MX1_SLOT;
  L7_uint8 protection_slot = PTIN_SYS_MX2_SLOT;

 #if (PTIN_BOARD_IS_LINECARD)
  slot = ((cpld_map->reg.slot_matrix & 0x0f) == 0) ? protection_slot : working_slot;

 #elif (PTIN_BOARD_IS_MATRIX)
  /* Working slot */
  if (cpld_map->reg.slot_id == 0)
  {
    slot = (cpld_map->reg.mx_is_active == 0) ? protection_slot : working_slot;
  }
  /* Protection slot */
  else
  {
    slot = (cpld_map->reg.mx_is_active == 0) ? working_slot : protection_slot;
  }
 #endif

  return slot;
}

/**
 * Get slot position of Inactive Matrix 
 * (For all cards) 
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fgpa_matrixInactive_slot(void)
{
  L7_uint8 slot = 0;

  L7_uint8 working_slot    = PTIN_SYS_MX1_SLOT;
  L7_uint8 protection_slot = PTIN_SYS_MX2_SLOT;

  /* Get slot of active matrix */
  slot = ptin_fgpa_matrixActive_slot();

  /* Invert it */
  if (slot == working_slot)
  {
    slot = protection_slot;
  }
  else
  {
    slot = working_slot;
  }

  return slot;
}

/**
 * Get slot id 
 * (For all cards)
 * 
 * @return L7_uint8 : slot id
 */
L7_uint8 ptin_fgpa_board_slot(void)
{
  L7_uint8 slot = 0;

 #if (PTIN_BOARD_IS_LINECARD)
  /* If high and low nibbles are equal, we are at a OLT1T3 system */
  if (((cpld_map->reg.slot_matrix >> 4) & 0x0f) == (cpld_map->reg.slot_matrix & 0x0f) )
  {
    slot = cpld_map->reg.slot_id + 2;
  }
  /* Otherwise, we are at a OLT1T1 system */
  else
  {
    /* Validate slot id */
    if (cpld_map->reg.slot_id > 4)
      return 0;
    slot = 4 - cpld_map->reg.slot_id;     /* Invert slot ids */
  }
 #elif (PTIN_BOARD_IS_MATRIX)
  slot = (cpld_map->reg.slot_id == 0) ? PTIN_SYS_MX1_SLOT : PTIN_SYS_MX2_SLOT;
 #endif

  return slot;
}
#endif // (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

#endif//MAP_CPLD

