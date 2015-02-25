/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2014-2014
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
L7_uint8 ptin_fgpa_mx_is_matrixactive_rt(void)
{
  if (ptin_fgpa_mx_get_matrixactive()==PTIN_SLOT_WORK && !ptin_fgpa_mx_is_matrix_in_workingslot())
  {
    /* Running on Stanby matrix */
    return 0;
  }
  if (ptin_fgpa_mx_get_matrixactive()==PTIN_SLOT_PROT && ptin_fgpa_mx_is_matrix_in_workingslot())
  {
    /* Running on Stanby matrix */
    return 0;
  }

  /* Running on Active matrix */
  return 1;
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
  return ((cpld_map->reg.slot_matrix & 0x0f) != 0);
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
  L7_uint8 working_slot, protection_slot;

 #if (PTIN_BOARD_IS_LINECARD)
  L7_BOOL  olt1t1_backplane;

  /* Condition for OLT1T1 backplane */
  olt1t1_backplane = (((cpld_map->reg.slot_matrix >> 4) & 0x0f) != (cpld_map->reg.slot_matrix & 0x0f));

  working_slot    = PTIN_SYS_MX1_SLOT;
  protection_slot = (olt1t1_backplane) ? PTIN_SYS_OLT1T1_SLOTS_MAX : PTIN_SYS_OLT1T3_SLOTS_MAX;

  slot = ((cpld_map->reg.slot_matrix & 0x0f) == 0) ? protection_slot : working_slot;

 #elif (PTIN_BOARD_IS_MATRIX)
  working_slot    = PTIN_SYS_MX1_SLOT;
  protection_slot = PTIN_SYS_MX2_SLOT;

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
  L7_uint8 working_slot, protection_slot;

 #if (PTIN_BOARD_IS_LINECARD)
  L7_BOOL  olt1t1_backplane;

  /* Condition for OLT1T1 backplane */
  olt1t1_backplane = (((cpld_map->reg.slot_matrix >> 4) & 0x0f) != (cpld_map->reg.slot_matrix & 0x0f));

  working_slot    = PTIN_SYS_MX1_SLOT;
  protection_slot = (olt1t1_backplane) ? PTIN_SYS_OLT1T1_SLOTS_MAX : PTIN_SYS_OLT1T3_SLOTS_MAX;

  slot = ((cpld_map->reg.slot_matrix & 0x0f) == 0) ? working_slot : protection_slot;

 #elif (PTIN_BOARD_IS_MATRIX)
  working_slot    = PTIN_SYS_MX1_SLOT;
  protection_slot = PTIN_SYS_MX2_SLOT;

  /* Working slot */
  if (cpld_map->reg.slot_id == 0)
  {
    slot = (cpld_map->reg.mx_is_active == 0) ? working_slot : protection_slot;
  }
  /* Protection slot */
  else
  {
    slot = (cpld_map->reg.mx_is_active == 0) ? protection_slot : working_slot;
  }
 #endif

  return slot;
}

/**************************************************************************
*
* @purpose  Verify if this Matrix is Currently in Active State.
*           (Only for Matrix board)
*
* @param    void
*
* @returns  TRUE or FALSE
*
* @comments This funcion should be used only for dataplane crossover and not for control crossover
*
* @end
*
*************************************************************************/
#include <stdio.h>
#include <logger.h>
L7_uint8 ptin_fgpa_mx_get_matrixactive(void)
{
  int current_state = -1;
  static int previous_state = -1;

#if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
  L7_BOOL olt1t1_backplane = L7_FALSE;
  /* Condition for OLT1T1 backplane */
  olt1t1_backplane = (((cpld_map->reg.slot_matrix >> 4) & 0x0f) != (cpld_map->reg.slot_matrix & 0x0f));
#endif

  if((cpld_map->reg.mx_get_active & 0x03) == 0x02) {
      //Master Matrix was set to active
      current_state = PTIN_SLOT_WORK;
  }
  else if ((cpld_map->reg.mx_get_active & 0x03) == 0x01) {
      // Slave Matrix was set to active
      current_state = PTIN_SLOT_PROT;
  }
  else
  {
      current_state = previous_state;
  }

  previous_state = current_state;

#if ( PTIN_BOARD == PTIN_BOARD_TA48GE )
  /* Note: the register 18h is not affected by the backplane, however lanes/data plane is inverted */
  if (olt1t1_backplane)
  {
      return !current_state;
  }
#endif

  return current_state;
}

#endif // (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

#endif//MAP_CPLD

/**
 * Get slot id 
 * (For all cards)
 * 
 * @return L7_uint8 : slot id
 */
L7_uint8 ptin_fgpa_board_slot(void)
{
  L7_uint8 slot = 0;

#ifdef MAP_CPLD
 #if (PTIN_BOARD_IS_LINECARD)
  L7_BOOL  olt1t1_backplane;

  /* Condition for OLT1T1 backplane */
  olt1t1_backplane = (((cpld_map->reg.slot_matrix >> 4) & 0x0f) != (cpld_map->reg.slot_matrix & 0x0f));

  /* If high and low nibbles are equal, we are at a OLT1T3 system */
  if (!olt1t1_backplane)
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
#endif

  return slot;
}

