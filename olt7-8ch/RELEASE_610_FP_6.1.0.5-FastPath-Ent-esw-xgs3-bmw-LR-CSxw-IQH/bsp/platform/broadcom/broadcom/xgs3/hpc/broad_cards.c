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

/*********************************************************************************
 *  Broadcom reference platform with 16 G + 4 10GB fixed ports on a single slot. *
 *                                                                               *
 *********************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_16_GIG_4_TENGIG_REV_1[] =
{
{ 0,  L7_TRUE,   20, 2},   /* physical slot with 28 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 22},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 30}    /* logical Router card */
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_16_GIG_4_TENGIG_REV_1[] =
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

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 28},
  {0, 29},
  {0, 30},
  {0, 31}
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 28},
  {0, 29},
  {0, 30},
  {0, 31}
};


/******************************************************************************
 ******************************************************************************
 *                                UNIT ENTRIES                                *
 ******************************************************************************
 *****************************************************************************/

/*******************************************************************************
 * Broadcom FireBolt unit with 24GB & 4 10GB fixed ports on a single slot.     *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56504_REV_1[] = 
{
{
  mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  28,                                                 /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom FireBolt unit with 48GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56504_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  2,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  28,                                                 /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};


/*******************************************************************************
 * Broadcom FireBolt-2 unit with 24GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56514_REV_1[] = 
{
{
  mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  28,                                                 /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom FireBolt-2 unit with 48GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56514_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  2,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  28,                                                 /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom Helix unit with 24GB & 4 10GB fixed ports on a single slot.        *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56304_REV_1[] = 
{
{
  mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  28,                                                 /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/*******************************************************************************
 * Broadcom Bradley unit with 20 10GB fixed ports on a single slot.            *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_20_TENGIG_56800_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_20_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_20_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  28,                                                 /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_20_TENGIG_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_20_TENGIG_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/***********************************************************************************
 *  * Broadcom GoldWing unit with 16 G + 4 10GB fixed ports on a single slot.      *
 *   *                                                                             *
 *    ******************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_16_GIG_4_TENGIG_56580_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_16_GIG_4_TENGIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_16_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  28,                                                 /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_16_GIG_4_TENGIG_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_16_GIG_4_TENGIG_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

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

/* FB-2 24-port BMW reference platforms have 10G ports numbered other way */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56514_REV_1[] =
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
{       0,      24,          0,      27},
{       0,      25,          0,      26},
{       0,      26,          0,      25},
{       0,      27,          0,      24},
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

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
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

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
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
{       0,       8,          0,      14},
{       0,       9,          0,      15},
{       0,      10,          0,      16},
{       0,      11,          0,      17},
{       0,      12,          0,      26},
{       0,      13,          0,      32},
{       0,      14,          0,      33},
{       0,      15,          0,      34},
{       0,      16,          0,      35},
{       0,      17,          0,      36},
{       0,      18,          0,      46},
{       0,      19,          0,      47},
{       0,      20,          0,      27},
{       0,      21,          0,      43},
{       0,      22,          0,      44},
{       0,      23,          0,      45},
{       0,      24,          0,      28},
{       0,      25,          0,      29},
{       0,      26,          0,      30},
{       0,      27,          0,      31},
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

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_16_GIG_4_TENGIG_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port *
 *  * -------  ------- ----------- -------- */
{{      0,       0,          0,       0},
{       0,       1,          0,       1},
{       0,       2,          0,       2},
{       0,       3,          0,       3},
{       0,       4,          0,       4},
{       0,       5,          0,       5},
{       0,       6,          0,       6},
{       0,       7,          0,       7},
{       0,       8,          0,       8},
{       0,       9,          0,      10},
{       0,      10,          0,      12},
{       0,      11,          0,      15},
{       0,      12,          0,       9},
{       0,      13,          0,      11},
{       0,      14,          0,      13},
{       0,      15,          0,      14},
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

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_16_GIG_4_TENGIG_REV_1[] =
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

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_16_GIG_4_TENGIG_REV_1[] =
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

/* FB2 24-port BMW reference platforms have 10G ports numbers other way around */
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56514_REV_1 = {

hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56514_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56514_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
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


DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_16_GIG_4_TENGIG_REV_1 = {

hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_16_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_16_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_16_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_16_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
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
DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_20_TENGIG_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_20_TENGIG_REV_1,
  sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_20_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0
};

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_16_GIG_4_TENGIG_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_16_GIG_4_TENGIG_REV_1,
  sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_16_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
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





SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_REV_1[]= 
{
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},
{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG},{L7_PORT_DESC_BCOM_RJ45_GIG_SFP},{L7_PORT_DESC_BCOM_RJ45_GIG_SFP},
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

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_16_GIG_4_TENGIG_REV_1[]=
{
{L7_PORT_DESC_BCOM_SERDES_2G5}, {L7_PORT_DESC_BCOM_SERDES_2G5}, {L7_PORT_DESC_BCOM_SERDES_2G5}, {L7_PORT_DESC_BCOM_SERDES_2G5},
{L7_PORT_DESC_BCOM_SERDES_2G5}, {L7_PORT_DESC_BCOM_SERDES_2G5}, {L7_PORT_DESC_BCOM_SERDES_2G5}, {L7_PORT_DESC_BCOM_SERDES_2G5},
{L7_PORT_DESC_BCOM_SERDES_GIG}, {L7_PORT_DESC_BCOM_SERDES_GIG}, {L7_PORT_DESC_BCOM_SERDES_GIG}, {L7_PORT_DESC_BCOM_SERDES_GIG},
{L7_PORT_DESC_BCOM_SERDES_GIG}, {L7_PORT_DESC_BCOM_SERDES_GIG}, {L7_PORT_DESC_BCOM_SERDES_GIG}, {L7_PORT_DESC_BCOM_SERDES_GIG},
{L7_PORT_DESC_BCOM_SERDES_10G}, {L7_PORT_DESC_BCOM_SERDES_10G}, {L7_PORT_DESC_BCOM_SERDES_10G}, {L7_PORT_DESC_BCOM_SERDES_10G}
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
