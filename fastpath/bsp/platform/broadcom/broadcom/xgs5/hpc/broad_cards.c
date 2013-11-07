/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  broad_cards.c
*
* @purpose   Declare and initialize descriptors for Broadcom cards
*            for unit descriptor databases.
*
* @component hpc
*
* @create    08/15/2005
*
* @author    bradyr 
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "dapi.h"
#include "dapi_struct.h"
#include "sysapi_hpc.h" 
#include "hpc_db.h"
#include "dapi_db.h"
#include "broad_init.h"
#include "broad_hpc_db.h"
#include "l7_platformspecs.h"   /* PTin added: defines PTIN_BOARD */



/******************************************************************************
 ******************************************************************************
 *                                SLOT ENTRIES                                *
 ******************************************************************************
 *****************************************************************************/

/*******************************************************************************
 * Broadcom reference platform with 24GB & 4 10GB fixed ports on a single slot.*
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1[] =
{
{ 0,  L7_TRUE,   28, 2},   /* physical slot with 28 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 30},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 38}    /* logical Router card */
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_REV_1[] = 
{
  {0, 24},
  {0, 25},
  {0, 26},
  {0, 27}
};


/*******************************************************************************
 * Broadcom reference platform with 20 10GB fixed ports on a single slot.      *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_20_TENGIG_REV_1[] =
{
{ 0,  L7_TRUE,   20, 2},   /* physical slot with 28 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 22},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 30}    /* logical Router card */
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_20_TENGIG_REV_1[] =
{
};

/*******************************************************************************
 * Broadcom reference platform with 24 10GB fixed ports on a single slot.      *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_24_TENGIG_4_GIG_REV_1[] =
{
{ 0,  L7_TRUE,   28, 2},   /* physical slot with 28 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 30},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 38}    /* logical Router card */
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_24_TENGIG_4_GIG_REV_1[] =
{
  {0, 21},
  {0, 22},
  {0, 23},
  {0, 24}
};

/* PTin added: new switch BCM56843 (trident) */
/*******************************************************************************
 * Broadcom reference platform with 40 10GB fixed ports on a single slot.      *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_40_TENGIG_56843_REV_1[] =
{
{ 0,  L7_TRUE,   41, 2},   /* physical slot with 36 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 43},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 51}    /* logical Router card */
};

/* PTin added: new platform BCM56846 (CXO640G) (trident) */
/*******************************************************************************
 * Broadcom reference platform with 64 10GB fixed ports on a single slot.      *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_64_TENGIG_56846_REV_1[] =
{
  { 0,  L7_TRUE,   64, 2},   /* physical slot with 36 port card */
  { 1,  L7_FALSE,  1,  0},   /* logical CPU card */
  { 2,  L7_TRUE,   8, 66},   /* logical LAG card */
  { 3,  L7_TRUE,   8, 74}    /* logical Router card */

#if 0 /* XXXX */
{ 0,  L7_TRUE,   64, 2},   /* physical slot with 64 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 66},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 74}    /* logical Router card */
#endif
};

//HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_40_TENGIG_56843_REV_1[] =
//{
//  {0, 21},
//  {0, 22},
//  {0, 23},
//  {0, 24}
//};


/*******************************************************************************
 * Broadcom reference platform with 48GE & 4 Dedicated stacking ports     *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1[] =
{
{ 0,  L7_TRUE,   48, 1},   /* physical slot with 48 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 30},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 38}    /* logical Router card */
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 24},
  {0, 25},
  {1, 24},
  {1, 25}
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 28},
  {0, 29},
  {0, 30},
  {0, 31}
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 26},
  {0, 27},
  {0, 28},
  {0, 29}
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 28},
  {0, 29},
  {0, 30},
  {0, 31}
};

/* PTin added: new platform BCM56643 */
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 48},
  {0, 49},
  {0, 50},
  {0, 51}
};


HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 26},
  {0, 27},
  {0, 28},
  {0, 29}
};

/* BCM 56636 reference platform. 25GE and 6 Higig (4 for stacking and 2 for 10G) */
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1[] =
{
{ 0,  L7_TRUE,   27, 2},   /* physical slot with 25+2 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 29},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 37}    /* logical Router card */
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 26},
  {0, 27},
  {0, 28},
  {0, 29}
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 26},
  {0, 27},
  {0, 28},
  {0, 29}
};


/******************************************************************************
 ******************************************************************************
 *                                UNIT ENTRIES                                *
 ******************************************************************************
 *****************************************************************************/

/*******************************************************************************
 * Broadcom Triumph unit with 48GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom Valkyrie unit with 24GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56680_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/* PTin added: new platform BCM56643 */
/*******************************************************************************
 * Broadcom Valkyrie unit with 48GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56643_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom Scorpion unit with 24 10GB fixed ports on a single slot.            *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_TENGIG_4_GIG_56820_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_24_TENGIG_4_GIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_24_TENGIG_4_GIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                 /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_24_TENGIG_4_GIG_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_24_TENGIG_4_GIG_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom Triumph2 unit with 48GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom Apollo unit with 24GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1[] = 
{
{
  mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom Triumph2 unit with 25GB & 6 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1,
  (sizeof(mac_allocation_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1) / 
   sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t)),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  (sizeof(stack_port_data_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1) / 
   sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t)),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom Enduro unit with 24GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1[] = 
{
{
  mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/* PTin added: new switch BCM56843 (trident) */
/*******************************************************************************
 * Broadcom Trident unit with 40 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_40_TENGIG_56843_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_40_TENGIG_56843_REV_1,
  (sizeof(mac_allocation_UNIT_BROAD_40_TENGIG_56843_REV_1) / 
   sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t)),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  (sizeof(stack_port_data_UNIT_BROAD_40_TENGIG_56843_REV_1) / 
   sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t)),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_40_TENGIG_56843_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom Trident unit with 64 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_64_TENGIG_56846_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_64_TENGIG_56846_REV_1,
  (sizeof(mac_allocation_UNIT_BROAD_64_TENGIG_56846_REV_1) / 
   sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t)),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  (sizeof(stack_port_data_UNIT_BROAD_64_TENGIG_56846_REV_1) / 
   sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t)),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_64_TENGIG_56846_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};
/* PTin end */

/*******************************************************************************
*
* @Data    dapiBroadBaseCardSlotMap
*
* @purpose The mapping of all potential slots in the box
*
* @end
*
*******************************************************************************/
/* Most of the BMW 24-port reference platforms have same slot map. */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,       0},
{       0,       1,          0,       1},
{       0,       2,          0,       2},
{       0,       3,          0,       3},
{       0,       4,          0,       4},
{       0,       5,          0,       5},
{       0,       6,          0,       6},
{       0,       7,          0,       7},
{       0,       8,          0,       8},
{       0,       9,          0,       9},
{       0,      10,          0,      10},
{       0,      11,          0,      11},
{       0,      12,          0,      12},
{       0,      13,          0,      13},
{       0,      14,          0,      14},
{       0,      15,          0,      15},
{       0,      16,          0,      16},
{       0,      17,          0,      17},
{       0,      18,          0,      18},
{       0,      19,          0,      19},
{       0,      20,          0,      20},
{       0,      21,          0,      21},
{       0,      22,          0,      22},
{       0,      23,          0,      23},
{       0,      24,          0,      24},
{       0,      25,          0,      25},
{       0,      26,          0,      26},
{       0,      27,          0,      27},
};

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port *
 * -------  -------  ----------- -------- */
{{ 0xFFFF,  0xFFFF,          0,      28},
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,       0},
{       0,       1,          0,       1},
{       0,       2,          0,       2},
{       0,       3,          0,       3},
{       0,       4,          0,       4},
{       0,       5,          0,       5},
{       0,       6,          0,       6},
{       0,       7,          0,       7},
{       0,       8,          0,       8},
{       0,       9,          0,       9},
{       0,      10,          0,      10},
{       0,      11,          0,      11},
{       0,      12,          0,      12},
{       0,      13,          0,      13},
{       0,      14,          0,      14},
{       0,      15,          0,      15},
{       0,      16,          0,      16},
{       0,      17,          0,      17},
{       0,      18,          0,      18},
{       0,      19,          0,      19},
{       0,      20,          0,      20},
{       0,      21,          0,      21},
{       0,      22,          0,      22},
{       0,      23,          0,      23},
{       0,      24,          1,       0},
{       0,      25,          1,       1},
{       0,      26,          1,       2},
{       0,      27,          1,       3},
{       0,      28,          1,       4},
{       0,      29,          1,       5},
{       0,      30,          1,       6},
{       0,      31,          1,       7},
{       0,      32,          1,       8},
{       0,      33,          1,       9},
{       0,      34,          1,      10},
{       0,      35,          1,      11},
{       0,      36,          1,      12},
{       0,      37,          1,      13},
{       0,      38,          1,      14},
{       0,      39,          1,      15},
{       0,      40,          1,      16},
{       0,      41,          1,      17},
{       0,      42,          1,      18},
{       0,      43,          1,      19},
{       0,      44,          1,      20},
{       0,      45,          1,      21},
{       0,      46,          1,      22},
{       0,      47,          1,      23},
{       0,      48,          0,      25},
{       0,      49,          0,      24},
{       0,      50,          1,      25},
{       0,      51,          1,      24},
};

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port *
 * -------  -------  ----------- -------- */
{{ 0xFFFF,  0xFFFF,          0,      28},
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,       2},
{       0,       1,          0,       3},
{       0,       2,          0,       4},
{       0,       3,          0,       5},
{       0,       4,          0,       6},
{       0,       5,          0,       7},
{       0,       6,          0,      18},
{       0,       7,          0,      19},
{       0,       8,          0,       8},
{       0,       9,          0,       9},
{       0,      10,          0,      10},
{       0,      11,          0,      11},
{       0,      12,          0,      12},
{       0,      13,          0,      13},
{       0,      14,          0,      20},
{       0,      15,          0,      21},
{       0,      16,          0,      14},
{       0,      17,          0,      15},
{       0,      18,          0,      16},
{       0,      19,          0,      17},
{       0,      20,          0,      22},
{       0,      21,          0,      23},
{       0,      22,          0,      24},
{       0,      23,          0,      25},
{       0,      24,          0,      37},
{       0,      25,          0,      38},
{       0,      26,          0,      39},
{       0,      27,          0,      40},
{       0,      28,          0,      26},
{       0,      29,          0,      32},
{       0,      30,          0,      33},
{       0,      31,          0,      34},
{       0,      32,          0,      41},
{       0,      33,          0,      42},
{       0,      34,          0,      48},
{       0,      35,          0,      49},
{       0,      36,          0,      50},
{       0,      37,          0,      51},
{       0,      38,          0,      52},
{       0,      39,          0,      53},
{       0,      40,          0,      35},
{       0,      41,          0,      36},
{       0,      42,          0,      46},
{       0,      43,          0,      47},
{       0,      44,          0,      27},
{       0,      45,          0,      43},
{       0,      46,          0,      44},
{       0,      47,          0,      45},
{       0,      48,          0,      28},
{       0,      49,          0,      29},
{       0,      50,          0,      30},
{       0,      51,          0,      31},
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,      30},
{       0,       1,          0,      31},
{       0,       2,          0,      32},
{       0,       3,          0,      33},
{       0,       4,          0,      34},
{       0,       5,          0,      35},
{       0,       6,          0,      36},
{       0,       7,          0,      37},
{       0,       8,          0,       2},
{       0,       9,          0,       3},
{       0,      10,          0,       4},
{       0,      11,          0,       5},
{       0,      12,          0,       6},
{       0,      13,          0,       7},
{       0,      14,          0,       8},
{       0,      15,          0,       9},
{       0,      16,          0,      38},
{       0,      17,          0,      39},
{       0,      18,          0,      40},
{       0,      19,          0,      41},
{       0,      20,          0,      10},
{       0,      21,          0,      11},
{       0,      22,          0,      12},
{       0,      23,          0,      13},
{       0,      24,          0,      14},
{       0,      25,          0,      15},
{       0,      26,          0,      16},
{       0,      27,          0,      17},
{       0,      28,          0,      42},
{       0,      29,          0,      43},
{       0,      30,          0,      44},
{       0,      31,          0,      45},
{       0,      32,          0,      18},
{       0,      33,          0,      19},
{       0,      34,          0,      20},
{       0,      35,          0,      21},
{       0,      36,          0,      22},
{       0,      37,          0,      23},
{       0,      38,          0,      24},
{       0,      39,          0,      25},
{       0,      40,          0,      46},
{       0,      41,          0,      47},
{       0,      42,          0,      48},
{       0,      43,          0,      49},
{       0,      44,          0,      50},
{       0,      45,          0,      51},
{       0,      46,          0,      52},
{       0,      47,          0,      53},
{       0,      48,          0,      26},
{       0,      49,          0,      27},
{       0,      50,          0,      28},
{       0,      51,          0,      29},
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,       0},
{       0,       1,          0,       1},
{       0,       2,          0,       2},
{       0,       3,          0,       3},
{       0,       4,          0,       4},
{       0,       5,          0,       5},
{       0,       6,          0,       6},
{       0,       7,          0,       7},
{       0,       8,          0,       8},
{       0,       9,          0,       9},
{       0,      10,          0,      10},
{       0,      11,          0,      11},
{       0,      12,          0,      12},
{       0,      13,          0,      13},
{       0,      14,          0,      14},
{       0,      15,          0,      15},
{       0,      16,          0,      16},
{       0,      17,          0,      17},
{       0,      18,          0,      18},
{       0,      19,          0,      19},
{       0,      20,          0,      20},
{       0,      21,          0,      21},
{       0,      22,          0,      22},
{       0,      23,          0,      23},
{       0,      24,          0,      24},
{       0,      25,          0,      25},
{       0,      26,          0,      26},
{       0,      27,          0,      27},
};

// PTin added: new platform
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{
#if (PTIN_BOARD == PTIN_BOARD_TOLT8G)
{       0,       0,          0,      30}, /* pon1 */
{       0,       1,          0,      31},
{       0,       2,          0,      32},
{       0,       3,          0,      33},
{       0,       4,          0,      34},
{       0,       5,          0,      35},
{       0,       6,          0,      36},
{       0,       7,          0,      37}, /* pon8 */
{       0,       8,          0,      29}, /* xe3 */
{       0,       9,          0,      27}, /* xe1 */
{       0,      10,          0,      28}, /* xe2 */
{       0,      11,          0,      26}, /* xe0 */
{       0,      12,          0,      39}, /* eth1 */
{       0,      13,          0,      43},
{       0,      14,          0,      47},
{       0,      15,          0,      53},
{       0,      16,          0,      41},
{       0,      17,          0,      45},
{       0,      18,          0,      49},
{       0,      19,          0,      51}, /* eth8 */
{       0,      20,          0,      42},
{       0,      21,          0,      44},
{       0,      22,          0,      46},
{       0,      23,          0,      48},
{       0,      24,          0,      50},
{       0,      25,          0,      52},
{       0,      26,          0,      38},
{       0,      27,          0,      40},
#elif (PTIN_BOARD == PTIN_BOARD_TG16G)
{       0,       0,          0,      30}, /* pon1 */
{       0,       1,          0,      31},
{       0,       2,          0,      32},
{       0,       3,          0,      33},
{       0,       4,          0,      34},
{       0,       5,          0,      35},
{       0,       6,          0,      36},
{       0,       7,          0,      37},
{       0,       8,          0,      38},
{       0,       9,          0,      39},
{       0,      10,          0,      40},
{       0,      11,          0,      41},
{       0,      12,          0,      42},
{       0,      13,          0,      43},
{       0,      14,          0,      44},
{       0,      15,          0,      45}, /* pon16 */
{       0,      16,          0,      29}, /* 10g_eth1 */
{       0,      17,          0,      27},
{       0,      18,          0,      28},
{       0,      19,          0,      26}, /* 10g_eth4 */
{       0,      20,          0,      46},
{       0,      21,          0,      47},
{       0,      22,          0,      48},
{       0,      23,          0,      49},
{       0,      24,          0,      50},
{       0,      25,          0,      51},
{       0,      26,          0,      52},
{       0,      27,          0,      53},
#else
{       0,       0,          0,      30}, /* pon1 */
{       0,       1,          0,      31},
{       0,       2,          0,      32},
{       0,       3,          0,      33},
{       0,       4,          0,      34},
{       0,       5,          0,      35},
{       0,       6,          0,      36},
{       0,       7,          0,      37}, /* pon8 */
{       0,       8,          0,      39}, /* eth1 */
{       0,       9,          0,      43},
{       0,      10,          0,      47},
{       0,      11,          0,      53},
{       0,      12,          0,      41},
{       0,      13,          0,      45},
{       0,      14,          0,      49},
{       0,      15,          0,      51}, /* eth8 */
{       0,      16,          0,      26}, /* xe0 */
{       0,      17,          0,      27}, /* xe1 */
{       0,      18,          0,      28}, /* xe2 */
{       0,      19,          0,      29}, /* xe3 */
{       0,      20,          0,      42},
{       0,      21,          0,      44},
{       0,      22,          0,      46},
{       0,      23,          0,      48},
{       0,      24,          0,      50},
{       0,      25,          0,      52},
{       0,      26,          0,      38},
{       0,      27,          0,      40},
#endif
};

/* PTin added: new platform BCM56643 */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{
{       0,       0,          0,       2}, /* ETH 1 */
{       0,       1,          0,       4},
{       0,       2,          0,       1},
{       0,       3,          0,       3},
{       0,       4,          0,       6},
{       0,       5,          0,       8},
{       0,       6,          0,       5},
{       0,       7,          0,       7},
{       0,       8,          0,      14},
{       0,       9,          0,      16},
{       0,      10,          0,      13},
{       0,      11,          0,      15},
{       0,      12,          0,      18},
{       0,      13,          0,      17},
{       0,      14,          0,      20},
{       0,      15,          0,      19},
{       0,      16,          0,      10},
{       0,      17,          0,       9},
{       0,      18,          0,      12},
{       0,      19,          0,      11},
{       0,      20,          0,      22},
{       0,      21,          0,      21},
{       0,      22,          0,      24},
{       0,      23,          0,      23},
{       0,      24,          0,      30},
{       0,      25,          0,      29},
{       0,      26,          0,      26},
{       0,      27,          0,      25},
{       0,      28,          0,      32},
{       0,      29,          0,      27},
{       0,      30,          0,      28},
{       0,      31,          0,      31},
{       0,      32,          0,      34},
{       0,      33,          0,      33},
{       0,      34,          0,      36},
{       0,      35,          0,      35},
{       0,      36,          0,      42},
{       0,      37,          0,      43},
{       0,      38,          0,      44},
{       0,      39,          0,      45},
{       0,      40,          0,      38},
{       0,      41,          0,      39},
{       0,      42,          0,      40},
{       0,      43,          0,      41},
{       0,      44,          0,      47},
{       0,      45,          0,      46},
{       0,      46,          0,      49},
{       0,      47,          0,      48}, /* ETH 48*/
{       0,      48,          0,      54}, /* 10G 1 */
{       0,      49,          0,      55},
{       0,      50,          0,      56},
{       0,      51,          0,      57}, /* 10G 4*/
};

/* 56843/56844 in CXO360G (OLT360) */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_40_TENGIG_56843_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,       2},
{       0,       1,          0,       1},
{       0,       2,          0,      16},
{       0,       3,          0,      15},
{       0,       4,          0,      14},
{       0,       5,          0,      13},
{       0,       6,          0,       8},
{       0,       7,          0,       7},
{       0,       8,          0,       6},
{       0,       9,          0,       5},
{       0,      10,          0,      20},
{       0,      11,          0,      19},
{       0,      12,          0,      18},
{       0,      13,          0,      17},
{       0,      14,          0,      12},
{       0,      15,          0,      11},
{       0,      16,          0,      10},
{       0,      17,          0,       9},
{       0,      18,          0,      32},
{       0,      19,          0,      31},
{       0,      20,          0,      30},
{       0,      21,          0,      29},
{       0,      22,          0,      24},
{       0,      23,          0,      23},
{       0,      24,          0,      22},
{       0,      25,          0,      21},
{       0,      26,          0,      36},
{       0,      27,          0,      35},
{       0,      28,          0,      34},
{       0,      29,          0,      33},
{       0,      30,          0,      28},
{       0,      31,          0,      27},
{       0,      32,          0,      26},
{       0,      33,          0,      25},
{       0,      34,          0,      40},
{       0,      35,          0,      39},
/* Port Expander ports NC */
{       0,      36,          0,      41}, /* PTP interface */
{       0,      37,          0,       3},
{       0,      38,          0,       4},
{       0,      39,          0,      37},
{       0,      40,          0,      38},
};

/* 56846 in CXO640G (OLT1T3) */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,       1},
{       0,       1,          0,       2},
{       0,       2,          0,       3},
{       0,       3,          0,       4},
{       0,       4,          0,       5},
{       0,       5,          0,       6},
{       0,       6,          0,       7},
{       0,       7,          0,       8},
{       0,       8,          0,       9},
{       0,       9,          0,      10},
{       0,      10,          0,      11},
{       0,      11,          0,      12},
{       0,      12,          0,      13},
{       0,      13,          0,      14},
{       0,      14,          0,      15},
{       0,      15,          0,      16},
{       0,      16,          0,      17},
{       0,      17,          0,      18},
{       0,      18,          0,      19},
{       0,      19,          0,      20},
{       0,      20,          0,      21},
{       0,      21,          0,      22},
{       0,      22,          0,      23},
{       0,      23,          0,      24},
{       0,      24,          0,      25},
{       0,      25,          0,      26},
{       0,      26,          0,      27},
{       0,      27,          0,      28},
{       0,      28,          0,      29},
{       0,      29,          0,      30},
{       0,      30,          0,      31},
{       0,      31,          0,      32},
{       0,      32,          0,      33},
{       0,      33,          0,      34},
{       0,      34,          0,      35},
{       0,      35,          0,      36},
{       0,      36,          0,      37},
{       0,      37,          0,      38},
{       0,      38,          0,      39},
{       0,      39,          0,      40},
{       0,      40,          0,      41},
{       0,      41,          0,      42},
{       0,      42,          0,      43},
{       0,      43,          0,      44},
{       0,      44,          0,      45},
{       0,      45,          0,      46},
{       0,      46,          0,      47},
{       0,      47,          0,      48},
#if 1 /* XXXX */
{       0,      48,          0,      49},
{       0,      49,          0,      50},
{       0,      50,          0,      51},
{       0,      51,          0,      52},
{       0,      52,          0,      53},
{       0,      53,          0,      54},
{       0,      54,          0,      55},
{       0,      55,          0,      56},
{       0,      56,          0,      57},
{       0,      57,          0,      58},
{       0,      58,          0,      59},
{       0,      59,          0,      60},
{       0,      60,          0,      61},
{       0,      61,          0,      62},
{       0,      62,          0,      63},
{       0,      63,          0,      64},
#endif
};

#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
/* 56846 in CXO640G (OLT1T3) */
HAPI_WC_SLOT_MAP_t dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_WORK[] =
/*  WC index  WC group  Inv.Lanes Inv.Pol. SlotIdx *
 * --------- --------- ---------  -------  -------*/
{{        0,        0,        1,       0,       6 },
{         1,        0,        1,       0,       2 },
{         2,        0,        1,       0,       3 },
{         3,        0,        1,       0,       5 },
{         4,        0,        1,       0,       4 },
{         5,        1,        1,       0,       7 },
{         6,        1,        1,       0,       8 },
{         7,        1,        1,       0,       9 },
{         8,        1,        1,       0,      10 },
{         9,        2,        1,       0,      11 },
{        10,        2,        1,       0,      12 },
{        11,        2,        1,       0,      13 },
{        12,        2,        1,       3,      18 },  /* RX and TX polarities inverted for WC 12: slot 18 */
{        13,        2,        1,       0,      19 },  
{        14,        3,        1,       0,      16 },
{        15,        3,        1,       0,      17 },
{        16,        3,        0,       0,      14 },  /* Lanes inverted for WC 16: slot 14 */ 
{        17,        3,        0,       0,      15 }}; /* Lanes inverted for WC 17: slot 15 */

HAPI_WC_SLOT_MAP_t dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_PROT[] =
/*  WC index  WC group  Inv.Lanes Inv.Pol. SlotIdx *
 * --------- --------- ---------  -------  -------*/
{{        0,        0,        1,       0,      15 },
{         1,        0,        1,       0,      14 },
{         2,        0,        1,       0,      17 },
{         3,        0,        1,       0,      16 },
{         4,        0,        1,       0,      19 },
{         5,        1,        1,       0,      18 },
{         6,        1,        1,       0,      13 },
{         7,        1,        1,       0,      12 },
{         8,        1,        1,       0,      11 },
{         9,        2,        1,       0,      10 },
{        10,        2,        1,       0,       9 },
{        11,        2,        1,       0,       8 },
{        12,        2,        1,       3,       7 },  /* RX and TX polarities inverted for WC 12: slot 18 */
{        13,        2,        1,       0,       4 },  
{        14,        3,        1,       0,       5 },
{        15,        3,        1,       0,       3 },
{        16,        3,        0,       0,       2 },  /* Lanes inverted for WC 16: slot 14 */ 
{        17,        3,        0,       0,       6 }}; /* Lanes inverted for WC 17: slot 15 */

L7_uint32 dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56846_REV_1[]=
/*  Port mode / slot  *
 * ------------------ */
{ WC_SLOT_MODE_NONE,    /* Slot 1  */
  WC_SLOT_MODE_2x10G,   /* Slot 2  */
  WC_SLOT_MODE_2x10G,   /* Slot 3  */
  WC_SLOT_MODE_4x10G,   /* Slot 4  */
  WC_SLOT_MODE_4x10G,   /* Slot 5  */
  WC_SLOT_MODE_2x10G,   /* Slot 6  */
  WC_SLOT_MODE_2x10G,   /* Slot 7  */
  WC_SLOT_MODE_4x10G,   /* Slot 8  */
  WC_SLOT_MODE_4x10G,   /* Slot 9  */
  WC_SLOT_MODE_4x10G,   /* Slot 10 */
  WC_SLOT_MODE_4x10G,   /* Slot 11 */
  WC_SLOT_MODE_4x10G,   /* Slot 12 */
  WC_SLOT_MODE_4x10G,   /* Slot 13 */
  WC_SLOT_MODE_2x10G,   /* Slot 14 */
  WC_SLOT_MODE_2x10G,   /* Slot 15 */
  WC_SLOT_MODE_4x10G,   /* Slot 16 */
  WC_SLOT_MODE_4x10G,   /* Slot 17 */
  WC_SLOT_MODE_2x10G,   /* Slot 18 */
  WC_SLOT_MODE_2x10G,   /* Slot 19 */
  WC_SLOT_MODE_NONE  }; /* Slot 20 */

/* 56846 in CXO640G (OLT1T3) */
HAPI_WC_PORT_MAP_t dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[] =
/* portNum slotIdx  WC idx  WCLane Speed *
 * ------- ------- ------- ------- ----- */
{{      0,      2,      1,      3,    10 },
{       1,      2,      1,      4,    10 },
{       2,      3,      2,      3,    10 },
{       3,      3,      2,      4,    10 },
{       4,      4,      4,      1,    10 },
{       5,      4,      4,      2,    10 },
{       6,      4,      4,      3,    10 },
{       7,      4,      4,      4,    10 },
{       8,      5,      3,      1,    10 },
{       9,      5,      3,      2,    10 },
{      10,      5,      3,      3,    10 },
{      11,      5,      3,      4,    10 },
{      12,      6,      0,      3,    10 },
{      13,      6,      0,      4,    10 },
{      14,      7,      5,      3,    10 },
{      15,      7,      5,      4,    10 },
{      16,      8,      6,      1,    10 },
{      17,      8,      6,      2,    10 },
{      18,      8,      6,      3,    10 },
{      19,      8,      6,      4,    10 },
{      20,      9,      7,      1,    10 },
{      21,      9,      7,      2,    10 },
{      22,      9,      7,      3,    10 },
{      23,      9,      7,      4,    10 },
{      24,     10,      8,      1,    10 },
{      25,     10,      8,      2,    10 },
{      26,     10,      8,      3,    10 },
{      27,     10,      8,      4,    10 },
{      28,     11,      9,      1,    10 },
{      29,     11,      9,      2,    10 },
{      30,     11,      9,      3,    10 },
{      31,     11,      9,      4,    10 },
{      32,     12,     10,      1,    10 },
{      33,     12,     10,      2,    10 },
{      34,     12,     10,      3,    10 },
{      35,     12,     10,      4,    10 },
{      36,     13,     11,      1,    10 },
{      37,     13,     11,      2,    10 },
{      38,     13,     11,      3,    10 },
{      39,     13,     11,      4,    10 },
{      40,     14,     16,      3,    10 },
{      41,     14,     16,      4,    10 },
{      42,     15,     17,      3,    10 },
{      43,     15,     17,      4,    10 },
{      44,     16,     14,      1,    10 },
{      45,     16,     14,      2,    10 },
{      46,     16,     14,      3,    10 },
{      47,     16,     14,      4,    10 },
{      48,     17,     15,      1,    10 },
{      49,     17,     15,      2,    10 },
{      50,     17,     15,      3,    10 },
{      51,     17,     15,      4,    10 },
{      52,     18,     12,      3,    10 },
{      53,     18,     12,      4,    10 },
{      54,     19,     13,      3,    10 },
{      55,     19,     13,      4,    10 },
{      56,      2,      1,      1,     1 },
{      57,      2,      1,      2,     1 },
{      58,      6,      0,      1,     1 },
{      59,      6,      0,      2,     1 },
{      60,     14,     16,      1,     1 },
{      61,     14,     16,      2,     1 },
{      62,     15,     17,      1,     1 },
{      63,     15,     17,      2,     1 }};
#endif

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_40_TENGIG_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port *
 * -------  -------  ----------- -------- */
{{ 0xFFFF,  0xFFFF,          0,       0},
};
// PTin end

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,       30},
{       0,       1,          0,       31},
{       0,       2,          0,       32},
{       0,       3,          0,       33},
{       0,       4,          0,       34},
{       0,       5,          0,       35},
{       0,       6,          0,       36},
{       0,       7,          0,       37},
{       0,       8,          0,      38},
{       0,       9,          0,      39},
{       0,      10,          0,      40},
{       0,      11,          0,      41},
{       0,      12,          0,      42},
{       0,      13,          0,      43},
{       0,      14,          0,      44},
{       0,      15,          0,      45},
{       0,      16,          0,      46},
{       0,      17,          0,      47},
{       0,      18,          0,      48},
{       0,      19,          0,      49},
{       0,      20,          0,      50},
{       0,      21,          0,      51},
{       0,      22,          0,      52},
{       0,      23,          0,      53},
{       0,      24,          0,      26},
{       0,      25,          0,      27},
{       0,      26,          0,      28},
{       0,      27,          0,      29},
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,      30},
{       0,       1,          0,      31},
{       0,       2,          0,      32},
{       0,       3,          0,      33},
{       0,       4,          0,      34},
{       0,       5,          0,      35},
{       0,       6,          0,      36},
{       0,       7,          0,      37},
{       0,       8,          0,       2},
{       0,       9,          0,       3},
{       0,      10,          0,       4},
{       0,      11,          0,       5},
{       0,      12,          0,       6},
{       0,      13,          0,       7},
{       0,      14,          0,       8},
{       0,      15,          0,       9},
{       0,      16,          0,      38},
{       0,      17,          0,      39},
{       0,      18,          0,      40},
{       0,      19,          0,      41},
{       0,      20,          0,      10},
{       0,      21,          0,      11},
{       0,      22,          0,      12},
{       0,      23,          0,      13},
{       0,      24,          0,       1},
{       0,      25,          0,      42},
{       0,      26,          0,      50},
{       0,      27,          0,      26},
{       0,      28,          0,      27},
{       0,      29,          0,      28},
{       0,      30,          0,      29},
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,      2},
{       0,       1,          0,      3},
{       0,       2,          0,      4},
{       0,       3,          0,      5},
{       0,       4,          0,      6},
{       0,       5,          0,      7},
{       0,       6,          0,      8},
{       0,       7,          0,      9},
{       0,       8,          0,      10},
{       0,       9,          0,      11},
{       0,      10,          0,      12},
{       0,      11,          0,      13},
{       0,      12,          0,      14},
{       0,      13,          0,      15},
{       0,      14,          0,      16},
{       0,      15,          0,      17},
{       0,      16,          0,      18},
{       0,      17,          0,      19},
{       0,      18,          0,      20},
{       0,      19,          0,      21},
{       0,      20,          0,      22},
{       0,      21,          0,      23},
{       0,      22,          0,      24},
{       0,      23,          0,      25},
{       0,      24,          0,      26},
{       0,      25,          0,      27},
{       0,      26,          0,      28},
{       0,      27,          0,      29},
};

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port *
 * -------  -------  ----------- -------- */
{{ 0xFFFF,  0xFFFF,          0,       0},
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_20_TENGIG_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{{      0,       0,          0,       0},
{       0,       1,          0,       1},
{       0,       2,          0,       2},
{       0,       3,          0,       3},
{       0,       4,          0,       4},
{       0,       5,          0,       5},
{       0,       6,          0,       6},
{       0,       7,          0,       7},
{       0,       8,          0,       8},
{       0,       9,          0,       9},
{       0,      10,          0,      10},
{       0,      11,          0,      11},
{       0,      12,          0,      12},
{       0,      13,          0,      13},
{       0,      14,          0,      14},
{       0,      15,          0,      15},
{       0,      16,          0,      16},
{       0,      17,          0,      17},
{       0,      18,          0,      18},
{       0,      19,          0,      19},
};

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_20_TENGIG_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port *
 * -------  -------  ----------- -------- */
{{ 0xFFFF,  0xFFFF,          0,      20},
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_TENGIG_4_GIG_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 * -------  ------- ----------- -------- */
{
#ifndef SC_1G_PORTS_FIRST
/* 24 X 10G ports */
{       0,       0,          0,       1},
{       0,       1,          0,       2},
{       0,       2,          0,       3},
{       0,       3,          0,       4},
{       0,       4,          0,       5},
{       0,       5,          0,       6},
{       0,       6,          0,       7},
{       0,       7,          0,       8},
{       0,       8,          0,       9},
{       0,       9,          0,      10},
{       0,      10,          0,      11},
{       0,      11,          0,      12},
{       0,      12,          0,      13},
{       0,      13,          0,      14},
{       0,      14,          0,      15},
{       0,      15,          0,      16},
{       0,      16,          0,      17},
{       0,      17,          0,      18},
{       0,      18,          0,      19},
{       0,      19,          0,      20},
{       0,      20,          0,      21},
{       0,      21,          0,      22},
{       0,      22,          0,      23},
{       0,      23,          0,      24},
/* 4 X 1GE ports */
{       0,      24,          0,      28},
{       0,      25,          0,      27},
{       0,      26,          0,      26},
{       0,      27,          0,      25},
#else
/* 4 X 1G ports */
{       0,       0,          0,       28},
{       0,       1,          0,       27},
{       0,       2,          0,       26},
{       0,       3,          0,       25},
/* 24 X 10G ports */
{       0,       4,          0,       1},
{       0,       5,          0,       2},
{       0,       6,          0,       3},
{       0,       7,          0,       4},
{       0,       8,          0,       5},
{       0,       9,          0,       6},
{       0,      10,          0,       7},
{       0,      11,          0,       8},
{       0,      12,          0,       9},
{       0,      13,          0,      10},
{       0,      14,          0,      11},
{       0,      15,          0,      12},
{       0,      16,          0,      13},
{       0,      17,          0,      14},
{       0,      18,          0,      15},
{       0,      19,          0,      16},
{       0,      20,          0,      17},
{       0,      21,          0,      18},
{       0,      22,          0,      19},
{       0,      23,          0,      20},
{       0,      24,          0,      21},
{       0,      25,          0,      22},
{       0,      26,          0,      23},
{       0,      27,          0,      24},
#endif
};

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_24_TENGIG_4_GIG_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port *
 * -------  -------  ----------- -------- */
{{ 0xFFFF,  0xFFFF,          0,      0},
};


/*******************************************************************************
*
* @Data    dapiBroadBaseCardPortMap
*
* @purpose A mapping of the ports in the base slot.  This determines the kind
*			     of ports, their phy addresses, port numbers internal, ...			
*
* @end
*
*******************************************************************************/
HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1[] =
/*portNum 
 *------- */
{{     0},
{     1},
{     2},
{     3},
{     4},
{     5},
{     6},
{     7},
{     8},
{     9},
{    10},
{    11},
{    12},
{    13},
{    14},
{    15},
{    16},
{    17},
{    18},
{    19},
{    20},
{    21},
{    22},
{    23},
{    24},
{    25},
{    26},
{    27},
};

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1[] =
/*portNum
 *------- */
{{     0},
{     1},
{     2},
{     3},
{     4},
{     5},
{     6},
{     7},
{     8},
{     9},
{    10},
{    11},
{    12},
{    13},
{    14},
{    15},
{    16},
{    17},
{    18},
{    19},
{    20},
{    21},
{    22},
{    23},
{    24},
{    25},
{    26},
{    27},
{    28},
{    29},
{    30},
{    31},
{    32},
{    33},
{    34},
{    35},
{    36},
{    37},
{    38},
{    39},
{    40},
{    41},
{    42},
{    43},
{    44},
{    45},
{    46},
{    47},
{    48},
{    49},
{    50},
{    51},
};

/* PTin added: new switch BCM56843 (trident) */
HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_40_TENGIG_REV_1[] =
/*portNum
 *------- */
{{     0},
{     1},
{     2},
{     3},
{     4},
{     5},
{     6},
{     7},
{     8},
{     9},
{    10},
{    11},
{    12},
{    13},
{    14},
{    15},
{    16},
{    17},
{    18},
{    19},
{    20},
{    21},
{    22},
{    23},
{    24},
{    25},
{    26},
{    27},
{    28},
{    29},
{    30},
{    31},
{    32},
{    33},
{    34},
{    35},
{    36},
{    37},
{    38},
{    39},
{    40}, /* PTP interface */
};

/* PTin added: new platform BCM56846 (trident) */
HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_64_TENGIG_REV_1[] =
/*portNum
 *------- */
{{     0},
{     1},
{     2},
{     3},
{     4},
{     5},
{     6},
{     7},
{     8},
{     9},
{    10},
{    11},
{    12},
{    13},
{    14},
{    15},
{    16},
{    17},
{    18},
{    19},
{    20},
{    21},
{    22},
{    23},
{    24},
{    25},
{    26},
{    27},
{    28},
{    29},
{    30},
{    31},
{    32},
{    33},
{    34},
{    35},
{    36},
{    37},
{    38},
{    39},
{    40},
{    41},
{    42},
{    43},
{    44},
{    45},
{    46},
{    47},
#if 1 /* XXXX */
{    48},
{    49},
{    50},
{    51},
{    52},
{    53},
{    54},
{    55},
{    56},
{    57},
{    58},
{    59},
{    60},
{    61},
{    62},
{    63},
#endif
};
/* PTin end */

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_20_TENGIG_REV_1[] =
/*portNum 
 *------- */
{{     0},
{     1},
{     2},
{     3},
{     4},
{     5},
{     6},
{     7},
{     8},
{     9},
{    10},
{    11},
{    12},
{    13},
{    14},
{    15},
{    16},
{    17},
{    18},
{    19},
};

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_24_TENGIG_4_GIG_REV_1[] =
/*portNum
 *------- */
{{     0},
{     1},
{     2},
{     3},
{     4},
{     5},
{     6},
{     7},
{     8},
{     9},
{    10},
{    11},
{    12},
{    13},
{    14},
{    15},
{    16},
{    17},
{    18},
{    19},
{    20},
{    21},
{    22},
{    23},
{    24},
{    25},
{    26},
{    27},
};

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_25_TENGIG_6_GIG__56636_REV_1[] =
/*portNum
 *------- */
{{     0},
{     1},
{     2},
{     3},
{     4},
{     5},
{     6},
{     7},
{     8},
{     9},
{    10},
{    11},
{    12},
{    13},
{    14},
{    15},
{    16},
{    17},
{    18},
{    19},
{    20},
{    21},
{    22},
{    23},
{    24},
{    25},
{    26},
{    27},
{    28},
{    29},
{    30},
};


/****************************************************************
*
*  Line Card Init Variables
*
****************************************************************/
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1 = {

hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t), 
dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

// PTin added: new platform
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};
/* PTin added: new platform BCM56643 */
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};
/* 56843 */
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_40_TENGIG_56843_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_40_TENGIG_56843_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_40_TENGIG_56843_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_40_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_40_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};
/* 56846 (OLT1T3) */
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_64_TENGIG_56846_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_64_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_64_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56846_REV_1, sizeof(dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56846_REV_1)/sizeof(L7_uint32),
dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1, sizeof(dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1)/sizeof(HAPI_WC_PORT_MAP_t)
#endif
};
// PTin end

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1 = {
 hapiBroadPhysicalCardInsert,
 hapiBroadCardRemove,
 dapiBroadBaseCardSlotMap_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1, 
 (sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1) / 
  sizeof(HAPI_CARD_SLOT_MAP_t)),
 dapiBroadBaseCardPortMap_CARD_BROAD_25_TENGIG_6_GIG__56636_REV_1, 
 (sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_25_TENGIG_6_GIG__56636_REV_1) / 
  sizeof(HAPI_CARD_PORT_MAP_t)),
 0, NULL, 0,
};


/****************************************************************
*
*  Line Card Init Variables
*
****************************************************************/
DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1,
  sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0
};

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1,
  sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0
};

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1,
  sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0
};

/****************************************************************
*
*  Line Card Init Variables
*
****************************************************************/
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_20_TENGIG_REV_1 = {

hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_20_TENGIG_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_20_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_20_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_20_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1 = {

hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_24_TENGIG_4_GIG_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_24_TENGIG_4_GIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_24_TENGIG_4_GIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_24_TENGIG_4_GIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

/****************************************************************
*
*  Line Card Init Variables
*
****************************************************************/
/* PTin added: new switch BCM56843 */
DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_40_TENGIG_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_40_TENGIG_REV_1,
  sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_40_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0
};
/* PTin added: new platform BCM56846 (OLT1T3) */
DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_64_TENGIG_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_40_TENGIG_REV_1,
  sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_40_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0,
#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
NULL, 0,
NULL, 0
#endif
};
/* PTin end */

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_20_TENGIG_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_20_TENGIG_REV_1,
  sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_20_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0
};

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_24_TENGIG_4_GIG_REV_1,
  sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_24_TENGIG_4_GIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0
};




/* PTin added (2.5G) */
SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1[]= 
{
#if (PTIN_BOARD == PTIN_BOARD_TOLT8G)
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},  /* 12 ports (8xPON + 4x10G */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
#elif (PTIN_BOARD == PTIN_BOARD_TG16G)
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},  /* 12 ports (16xPON + 4x10G */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
#else
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},  /* 18 ports (8xPON + 8xETH + 2x10G */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
#endif
};

/* PTin added: new platform BCM56643 */
SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1[]= 
{
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_40G_KR4},{L7_PORT_DESC_BCOM_40G_KR4},  /* 52 ports (48xETH + 4x10G */
{L7_PORT_DESC_BCOM_40G_KR4},{L7_PORT_DESC_BCOM_40G_KR4},
};

/* 56843 */
SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_40_TENGIG_56843_REV_1[]= 
{
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, /* PTP interface with FPGA */
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_64_TENGIG_56846_REV_1[]= 
{
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
/* CXO640G v1 */
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
#else
/* For the future */
/* CXO640G v2: slots 2,3,18,19 a 20G e o resto a 40G */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}
#endif
#if 0 /* XXXX */
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},{L7_PORT_DESC_BCOM_10G_KR},
#endif
};
/* PTin end */

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_REV_1[]= 
{
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}, {L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1[]= 
{
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}, {L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_REV_1[]=
{
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG}, {L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}, {L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}
};


SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_20_TENGIG_REV_1[]=
{
{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}, {L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G},
{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}, {L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G},
{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}, {L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G},
{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}, {L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G},
{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}, {L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_TENGIG_4_GIG_REV_1[]=
{
#ifdef SC_1G_PORTS_FIRST
{L7_PORT_DESC_BCOM_RJ45_GIG_FD_ONLY},{L7_PORT_DESC_BCOM_RJ45_GIG_FD_ONLY},{L7_PORT_DESC_BCOM_RJ45_GIG_FD_ONLY},{L7_PORT_DESC_BCOM_RJ45_GIG_FD_ONLY},
#endif
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G}, {L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G}, {L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G}, {L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G}, {L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G}, {L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G}, {L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
#ifndef SC_1G_PORTS_FIRST
{L7_PORT_DESC_BCOM_RJ45_GIG_FD_ONLY},{L7_PORT_DESC_BCOM_RJ45_GIG_FD_ONLY},{L7_PORT_DESC_BCOM_RJ45_GIG_FD_ONLY},{L7_PORT_DESC_BCOM_RJ45_GIG_FD_ONLY}
#endif
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1[]=
{
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}, {L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G},{L7_PORT_DESC_BCOM_XAUI_10G}
};
