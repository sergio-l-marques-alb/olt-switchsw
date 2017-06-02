/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename  inventory_commdefs.h
*
* @purpose   Common defines, enums, and data structures for inventory
*
* @component 
*
* @comments
*
* @create    06/11/2003
*
* @author    rjindal
* @end
*
*********************************************************************/

/*************************************************************

*************************************************************/

#ifndef INVENTORY_COMMDEFS_H
#define INVENTORY_COMMDEFS_H

#include "l7_common.h"

/*-------------------------*/
/*   INVENTORY ENUMS        */
/*-------------------------*/

typedef enum
{
  L7_INVENTORY_TRAP_RESERVED = 0,
  L7_INVENTORY_TRAP_CARD_MISMATCH,
  L7_INVENTORY_TRAP_CARD_UNSUPPORTED,
  L7_INVENTORY_TRAP_LAST
  
} L7_INVENTORY_TRAP_t;

/*-------------------------*/
/*   INVENTORY TRAP TYPES   */
/*-------------------------*/

/* NOTE: Following definitions are provided to facilitate UI configuration 
 *       of inventory traps. Curently these traps are not configurable.
 *
 * Following flags are to be used by UI routines 
 * to enable/disable or get the mode of the inventory trap(s) 
 */
#define L7_INV_TRAP_ALL                0x3
#define L7_INV_TRAP_CARD_MISMATCH      0x1
#define L7_INV_TRAP_CARD_UNSUPPORTED   0x2

#endif /* INVENTORY_COMMDEFS_H */
