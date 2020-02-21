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
#include "ptin_globaldefs.h"




/******************************************************************************
 *                                SLOT ENTRIES                                *
 ******************************************************************************/

/* PTin added: new switch 56689 (Valkyrie2) - TG16G */

/*******************************************************************************
 * Broadcom Valkyrie unit with 24GB & 4 10GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_24_GIG_4_TENGIG_REV_1[] =
{
{ 0,  L7_TRUE,   28, 2},   /* physical slot with 28 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 30},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 38}    /* logical Router card */
};

HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 28},
  {0, 29},
  {0, 30},
  {0, 31}
};

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


/* PTin added: new switch 56843 (Trident) */

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

#ifdef L7_STACKING_PACKAGE
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_40_TENGIG_56843_REV_1[] =
{
  {0, 21},
  {0, 22},
  {0, 23},
  {0, 24}
};
#endif

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


/* PTin added: new switch 56846 (Trident-plus) */

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
};

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


/* PTin added: new switch 5664x (Triumph3) GPON */

/*******************************************************************************
 * Broadcom reference platform with 4 10GBE & 4 Dedicated stacking ports       *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_4_TENGIG_4_40GIG_REV_1[] =
{
{ 0,  L7_TRUE,   4,  1},   /* physical slot with 4 port card */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 30},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 38}    /* logical Router card */
};

#ifdef L7_STACKING_PACKAGE
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_4_TENGIG_4_40GIG_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 54},
  {0, 55},
  {0, 56},
  {0, 57}
};
#endif

HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_4_TENGIG_4_40GIG_56643_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_4_TENGIG_4_40GIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_4_TENGIG_4_40GIG_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_4_TENGIG_4_40GIG_SINGLE_CHIP_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_4_TENGIG_4_40GIG_SINGLE_CHIP_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};


/* PTin added: new switch 5664x (Triumph3) SF - CXO160G */

/*******************************************************************************
 * Broadcom Triumph3 unit with 4 10GB & 3 40GB fixed ports on a single slot.   *
 *                                                                             *
 ******************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_4_10G_3_40G_1_GIG_REV_1[] =
{
{ 0,  L7_TRUE,   8,  1},   /* physical slot with 8 port card (4x10G + 4x40G) */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 30},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 38}    /* logical Router card */
};

#ifdef L7_STACKING_PACKAGE
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_4_10G_3_40G_1_GIG_SINGIG_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0,  5},
  {0,  9},
  {0, 13}
};
#endif

HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_4_10G_3_40G_1_GIG_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_4_10G_3_40G_1_GIG_REV_1)/sizeof(mac_allocation_UNIT_BROAD_4_10G_3_40G_1_GIG_REV_1),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_4_10G_3_40G_1_GIG_SINGIG_SINGLE_CHIP_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};


/* PTin added: new switch 5664x (Triumph3) - TA48GE */

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

#ifdef L7_STACKING_PACKAGE
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 48},
  {0, 49},
  {0, 50},
  {0, 51}
};
#endif

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


/* PTin added: new switch 56170 (Hurricane3-MG/Greyhound2) */

/******************************************************************************************************
 * Broadcom Hurricane3-MG/Greyhound2 unit with 12x1GB + 12x10G + 8x10GB fixed ports on a single slot. *
 *                                                                                                    *
 ******************************************************************************************************/
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1[] =
{
{ 0,  L7_TRUE,   64, 1},   /* physical slot with 56 port card (24x1G + 24x10G + 8x10G) */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 70},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 78}    /* logical Router card */
};

/* PTin added: new switch 56170 (Hurricane3-MG/Greyhound2) */
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 62},
  {0, 63},
  {0, 64},
  {0, 65},
  {0, 56},
  {0, 57},
  {0, 52},
  {0, 53},
  {0, 58},
  {0, 59},
  {0, 60},
  {0, 61},
  {0, 54},
  {0, 55},
  {0, 50},
  {0, 51},
};

HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};


/* PTin added: new switch 56340 (Helix4), 56450 (Katana2) */

HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_REV_1[] =
{
{ 0,  L7_TRUE,   16, 1},   /* physical slot with 16 port card (12x1G + 4x10G + 1x1G +1x1G) */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 30},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 38}    /* logical Router card */
};

/* PTin added: new switch 56340 (Helix4) */
/***********************************************************************************
 * Broadcom Helix4 unit with 8x2.5GB + 4x1G + 4x10GB fixed ports on a single slot. *
 *                                                                                 *
 ***********************************************************************************/
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 50},
  {0, 51},
  {0, 52},
  {0, 53}
};

HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_SINGLE_CHIP_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_SINGLE_CHIP_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/* PTin added: new switch 56450 (Katana2) */
/***********************************************************************************
 * Broadcom Katana2 unit with 8x2.5GB + 4x1G + 4x10GB fixed ports on a single slot. *
 *                                                                                 *
 ***********************************************************************************/
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_K2_SINGLE_CHIP_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 27},
  {0, 25},
  {0, 33},
  {0, 36}
};

HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_12_GIG_4_TENGIG_1_K2_GS_SINGLE_CHIP_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_K2_SINGLE_CHIP_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};


/******************************************************************************
 *                                UNIT ENTRIES                                *
 ******************************************************************************/


HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_DEFAULT[] =
/* slotNum  portNum  bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  -------  ----------- -------- ------------ -------- */
{{ 0xFFFF,  0xFFFF,          0,        0,           1,      "" },
};

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_DEFAULT = {
    hapiBroadCpuCardInsert,hapiBroadCardRemove,
    dapiBroadCpuCardSlotMap_CARD_BROAD_DEFAULT,
    sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_DEFAULT)/sizeof(HAPI_CARD_SLOT_MAP_t),
    NULL, 0,
    0, NULL, 0
};

/* PTin added: new switch 56846 (Trident-plus) */
DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_64_TENGIG_REV_1 = {
    hapiBroadCpuCardInsert,hapiBroadCardRemove,
    dapiBroadCpuCardSlotMap_CARD_BROAD_DEFAULT,
    sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_DEFAULT)/sizeof(HAPI_CARD_SLOT_MAP_t),
    NULL, 0,
    0, NULL, 0,
#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
    NULL, 0,
    NULL, 0
#endif
};


HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  -------  ----------- -------- ------------ -------- */
{{ 0xFFFF,  0xFFFF,          0,       28,           1,      "" },
};

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1 = {
    hapiBroadCpuCardInsert,hapiBroadCardRemove,
    dapiBroadCpuCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1,
    sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
    NULL, 0,
    0, NULL, 0
};

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  -------  ----------- -------- ------------ -------- */
{{ 0xFFFF,  0xFFFF,          0,       28,           1,      "" },
};

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1 = {

    hapiBroadCpuCardInsert,hapiBroadCardRemove,
    dapiBroadCpuCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1,
    sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
    NULL, 0,
    0, NULL, 0
};


/******************************************************************************
 *                                UNIT ENTRIES                                *
 ******************************************************************************/

/* PTin added: new switch 56689 (Valkyrie2) */

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

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{
#if (PTIN_BOARD == PTIN_BOARD_TG16G)
{       0,       0,          0,      30,           1,      "" }, /* pon1 */
{       0,       1,          0,      31,           1,      "" },
{       0,       2,          0,      32,           1,      "" },
{       0,       3,          0,      33,           1,      "" },
{       0,       4,          0,      34,           1,      "" },
{       0,       5,          0,      35,           1,      "" },
{       0,       6,          0,      36,           1,      "" },
{       0,       7,          0,      37,           1,      "" },
{       0,       8,          0,      38,           1,      "" },
{       0,       9,          0,      39,           1,      "" },
{       0,      10,          0,      40,           1,      "" },
{       0,      11,          0,      41,           1,      "" },
{       0,      12,          0,      42,           1,      "" },
{       0,      13,          0,      43,           1,      "" },
{       0,      14,          0,      44,           1,      "" },
{       0,      15,          0,      45,           1,      "" }, /* pon16 */
{       0,      16,          0,      29,           1,      "" }, /* 10g_eth1 */
{       0,      17,          0,      27,           1,      "" },
{       0,      18,          0,      28,           1,      "" },
{       0,      19,          0,      26,           1,      "" }, /* 10g_eth4 */
{       0,      20,          0,      46,           1,      "" },
{       0,      21,          0,      47,           1,      "" },
{       0,      22,          0,      48,           1,      "" },
{       0,      23,          0,      49,           1,      "" },
{       0,      24,          0,      50,           1,      "" },
{       0,      25,          0,      51,           1,      "" },
{       0,      26,          0,      52,           1,      "" },
{       0,      27,          0,      53,           1,      "" },
#else /* TOLT8G */
{       0,       0,          0,      30,           1,      "" }, /* pon1 */
{       0,       1,          0,      31,           1,      "" },
{       0,       2,          0,      32,           1,      "" },
{       0,       3,          0,      33,           1,      "" },
{       0,       4,          0,      34,           1,      "" },
{       0,       5,          0,      35,           1,      "" },
{       0,       6,          0,      36,           1,      "" },
{       0,       7,          0,      37,           1,      "" }, /* pon8 */
{       0,       8,          0,      29,           1,      "" }, /* xe3 */
{       0,       9,          0,      27,           1,      "" }, /* xe1 */
{       0,      10,          0,      28,           1,      "" }, /* xe2 */
{       0,      11,          0,      26,           1,      "" }, /* xe0 */
{       0,      12,          0,      39,           1,      "" }, /* eth1 */
{       0,      13,          0,      43,           1,      "" },
{       0,      14,          0,      47,           1,      "" },
{       0,      15,          0,      53,           1,      "" },
{       0,      16,          0,      41,           1,      "" },
{       0,      17,          0,      45,           1,      "" },
{       0,      18,          0,      49,           1,      "" },
{       0,      19,          0,      51,           1,      "" }, /* eth8 */
{       0,      20,          0,      42,           1,      "" },
{       0,      21,          0,      44,           1,      "" },
{       0,      22,          0,      46,           1,      "" },
{       0,      23,          0,      48,           1,      "" },
{       0,      24,          0,      50,           1,      "" },
{       0,      25,          0,      52,           1,      "" },
{       0,      26,          0,      38,           1,      "" },
{       0,      27,          0,      40,           1,      "" },
#endif
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1[]= 
{
#if (PTIN_BOARD == PTIN_BOARD_TG16G)
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},  /* 12 ports (16xPON + 4x10G */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
#else /* TOLT8G */
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},  /* 12 ports (8xPON + 4x10G */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},{L7_PORT_DESC_BCOM_1G_NO_AN},
#endif
};


/* PTin added: new switch 56843 (Trident) */

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

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_40_TENGIG_56843_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{{      0,       0,          0,       2,           1,      "" },
{       0,       1,          0,       1,           1,      "" },
{       0,       2,          0,      16,           1,      "" },
{       0,       3,          0,      15,           1,      "" },
{       0,       4,          0,      14,           1,      "" },
{       0,       5,          0,      13,           1,      "" },
{       0,       6,          0,       8,           1,      "" },
{       0,       7,          0,       7,           1,      "" },
{       0,       8,          0,       6,           1,      "" },
{       0,       9,          0,       5,           1,      "" },
{       0,      10,          0,      20,           1,      "" },
{       0,      11,          0,      19,           1,      "" },
{       0,      12,          0,      18,           1,      "" },
{       0,      13,          0,      17,           1,      "" },
{       0,      14,          0,      12,           1,      "" },
{       0,      15,          0,      11,           1,      "" },
{       0,      16,          0,      10,           1,      "" },
{       0,      17,          0,       9,           1,      "" },
{       0,      18,          0,      32,           1,      "" },
{       0,      19,          0,      31,           1,      "" },
{       0,      20,          0,      30,           1,      "" },
{       0,      21,          0,      29,           1,      "" },
{       0,      22,          0,      24,           1,      "" },
{       0,      23,          0,      23,           1,      "" },
{       0,      24,          0,      22,           1,      "" },
{       0,      25,          0,      21,           1,      "" },
{       0,      26,          0,      36,           1,      "" },
{       0,      27,          0,      35,           1,      "" },
{       0,      28,          0,      34,           1,      "" },
{       0,      29,          0,      33,           1,      "" },
{       0,      30,          0,      28,           1,      "" },
{       0,      31,          0,      27,           1,      "" },
{       0,      32,          0,      26,           1,      "" },
{       0,      33,          0,      25,           1,      "" },
{       0,      34,          0,      40,           1,      "" },
{       0,      35,          0,      39,           1,      "" },
/* Port Expander ports NC */
{       0,      36,          0,      41,           1,      "" }, /* PTP interface */
{       0,      37,          0,       3,           1,      "" },
{       0,      38,          0,       4,           1,      "" },
{       0,      39,          0,      37,           1,      "" },
{       0,      40,          0,      38,           1,      "" },
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_40_TENGIG_56843_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_40_TENGIG_56843_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_40_TENGIG_56843_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_40_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_40_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

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


#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
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

#elif (PTIN_BOARD == PTIN_BOARD_CXO160G)

L7_uint32 dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56640_REV_1[]=
/*  Port mode / slot  *
 * ------------------ */
{ WC_SLOT_MODE_NONE,    /* Slot 1 */
  WC_SLOT_MODE_4x10G,   /* Slot 2 */
  WC_SLOT_MODE_4x10G,   /* Slot 3 */
  WC_SLOT_MODE_4x10G,   /* Slot 4 */
  WC_SLOT_MODE_NONE  }; /* Slot 5 */

/* PTin added: new switch 5664x (Triumph3) SF */
HAPI_WC_PORT_MAP_t dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[] =
/* portNum slotIdx  WC idx  WCLane Speed *
 * ------- ------- ------- ------- ----- */
{{      0,     -1,     -1,      0,    10 },
{       1,     -1,     -1,      0,    10 },
{       2,     -1,     -1,      0,    10 },
{       3,     -1,     -1,      0,    10 },
{       4,      2,     -1,      0,    10 },
{       5,      2,     -1,      1,    10 },
{       6,      2,     -1,      2,    10 },
{       7,      2,     -1,      3,    10 },
{       8,      3,     -1,      0,    10 },
{       9,      3,     -1,      1,    10 },
{      10,      3,     -1,      2,    10 },
{      11,      3,     -1,      3,    10 },
{      12,      4,     -1,      0,    10 },
{      13,      4,     -1,      1,    10 },
{      14,      4,     -1,      2,    10 },
{      15,      4,     -1,      3,    10 },
{      16,     -1,     -1,      0,     1 }};

#endif

/* PTin added: new switch 56846 (Trident-plus) */

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
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{{      0,       0,          0,       1,           1,      "" },
{       0,       1,          0,       2,           1,      "" },
{       0,       2,          0,       3,           1,      "" },
{       0,       3,          0,       4,           1,      "" },
{       0,       4,          0,       5,           1,      "" },
{       0,       5,          0,       6,           1,      "" },
{       0,       6,          0,       7,           1,      "" },
{       0,       7,          0,       8,           1,      "" },
{       0,       8,          0,       9,           1,      "" },
{       0,       9,          0,      10,           1,      "" },
{       0,      10,          0,      11,           1,      "" },
{       0,      11,          0,      12,           1,      "" },
{       0,      12,          0,      13,           1,      "" },
{       0,      13,          0,      14,           1,      "" },
{       0,      14,          0,      15,           1,      "" },
{       0,      15,          0,      16,           1,      "" },
{       0,      16,          0,      17,           1,      "" },
{       0,      17,          0,      18,           1,      "" },
{       0,      18,          0,      19,           1,      "" },
{       0,      19,          0,      20,           1,      "" },
{       0,      20,          0,      21,           1,      "" },
{       0,      21,          0,      22,           1,      "" },
{       0,      22,          0,      23,           1,      "" },
{       0,      23,          0,      24,           1,      "" },
{       0,      24,          0,      25,           1,      "" },
{       0,      25,          0,      26,           1,      "" },
{       0,      26,          0,      27,           1,      "" },
{       0,      27,          0,      28,           1,      "" },
{       0,      28,          0,      29,           1,      "" },
{       0,      29,          0,      30,           1,      "" },
{       0,      30,          0,      31,           1,      "" },
{       0,      31,          0,      32,           1,      "" },
{       0,      32,          0,      33,           1,      "" },
{       0,      33,          0,      34,           1,      "" },
{       0,      34,          0,      35,           1,      "" },
{       0,      35,          0,      36,           1,      "" },
{       0,      36,          0,      37,           1,      "" },
{       0,      37,          0,      38,           1,      "" },
{       0,      38,          0,      39,           1,      "" },
{       0,      39,          0,      40,           1,      "" },
{       0,      40,          0,      41,           1,      "" },
{       0,      41,          0,      42,           1,      "" },
{       0,      42,          0,      43,           1,      "" },
{       0,      43,          0,      44,           1,      "" },
{       0,      44,          0,      45,           1,      "" },
{       0,      45,          0,      46,           1,      "" },
{       0,      46,          0,      47,           1,      "" },
{       0,      47,          0,      48,           1,      "" },
{       0,      48,          0,      49,           1,      "" },
{       0,      49,          0,      50,           1,      "" },
{       0,      50,          0,      51,           1,      "" },
{       0,      51,          0,      52,           1,      "" },
{       0,      52,          0,      53,           1,      "" },
{       0,      53,          0,      54,           1,      "" },
{       0,      54,          0,      55,           1,      "" },
{       0,      55,          0,      56,           1,      "" },
{       0,      56,          0,      57,           1,      "" },
{       0,      57,          0,      58,           1,      "" },
{       0,      58,          0,      59,           1,      "" },
{       0,      59,          0,      60,           1,      "" },
{       0,      60,          0,      61,           1,      "" },
{       0,      61,          0,      62,           1,      "" },
{       0,      62,          0,      63,           1,      "" },
{       0,      63,          0,      64,           1,      "" },
};

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
};


/* PTin added: new switch 5664x (Triumph3) GPON */

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_4_TENGIG_4_40GIG_REV_1[] =
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
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_4_TENGIG_4_40GIG_56643_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{
{       0,       0,          0,      52,           1,      "" }, /* ETH 1 */
{       0,       1,          0,      53,           1,      "" },
{       0,       2,          0,      50,           1,      "" },
{       0,       3,          0,      51,           1,      "" },
{       0,       4,          0,      54,           1,      "" }, /* 40Gbps */
{       0,       5,          0,      55,           1,      "" },
{       0,       6,          0,      56,           1,      "" },
{       0,       7,          0,      57,           1,      "" },
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_4_TENGIG_4_40GIG_56643_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_4_TENGIG_4_40GIG_56643_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_4_TENGIG_4_40GIG_56643_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_4_TENGIG_4_40GIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_4_TENGIG_4_40GIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_4_TENGIG_4_40GIG_56643_REV_1[]= 
{
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},  /* 8 ports (4x10G + 4x40G) */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_40G_KR4},{L7_PORT_DESC_BCOM_40G_KR4},
{L7_PORT_DESC_BCOM_40G_KR4},{L7_PORT_DESC_BCOM_40G_KR4},
};


/* PTin modified: new switch 5664x (Triumph3) */

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

/* PTin added: new switch 5664x (Triumph3) - TA48GE */

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{
{       0,       0,          0,       2,           1,      "" }, /* ETH 1 */
{       0,       1,          0,       4,           1,      "" },
{       0,       2,          0,       1,           1,      "" },
{       0,       3,          0,       3,           1,      "" },
{       0,       4,          0,       6,           1,      "" },
{       0,       5,          0,       8,           1,      "" },
{       0,       6,          0,       5,           1,      "" },
{       0,       7,          0,       7,           1,      "" },
{       0,       8,          0,      14,           1,      "" },
{       0,       9,          0,      16,           1,      "" },
{       0,      10,          0,      13,           1,      "" },
{       0,      11,          0,      15,           1,      "" },
{       0,      12,          0,      18,           1,      "" },
{       0,      13,          0,      17,           1,      "" },
{       0,      14,          0,      20,           1,      "" },
{       0,      15,          0,      19,           1,      "" },
{       0,      16,          0,      10,           1,      "" },
{       0,      17,          0,       9,           1,      "" },
{       0,      18,          0,      12,           1,      "" },
{       0,      19,          0,      11,           1,      "" },
{       0,      20,          0,      22,           1,      "" },
{       0,      21,          0,      21,           1,      "" },
{       0,      22,          0,      24,           1,      "" },
{       0,      23,          0,      23,           1,      "" },
{       0,      24,          0,      30,           1,      "" },
{       0,      25,          0,      29,           1,      "" },
{       0,      26,          0,      26,           1,      "" },
{       0,      27,          0,      25,           1,      "" },
{       0,      28,          0,      32,           1,      "" },
{       0,      29,          0,      27,           1,      "" },
{       0,      30,          0,      28,           1,      "" },
{       0,      31,          0,      31,           1,      "" },
{       0,      32,          0,      34,           1,      "" },
{       0,      33,          0,      33,           1,      "" },
{       0,      34,          0,      36,           1,      "" },
{       0,      35,          0,      35,           1,      "" },
{       0,      36,          0,      42,           1,      "" },
{       0,      37,          0,      43,           1,      "" },
{       0,      38,          0,      44,           1,      "" },
{       0,      39,          0,      45,           1,      "" },
{       0,      40,          0,      38,           1,      "" },
{       0,      41,          0,      39,           1,      "" },
{       0,      42,          0,      40,           1,      "" },
{       0,      43,          0,      41,           1,      "" },
{       0,      44,          0,      47,           1,      "" },
{       0,      45,          0,      46,           1,      "" },
{       0,      46,          0,      49,           1,      "" },
{       0,      47,          0,      48,           1,      "" }, /* ETH 48*/
{       0,      48,          0,      54,           1,      "" }, /* 10G 1 */
{       0,      49,          0,      55,           1,      "" },
{       0,      50,          0,      56,           1,      "" },
{       0,      51,          0,      57,           1,      "" }, /* 10G 4*/
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_48_GIG_4_TENGIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

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


/* PTin added: new switch 5664x (Triumph3) SF - CXO160G */

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_4_10G_3_40G_1_GIG_REV_1[] =
/*portNum
 *------- */
{{    0},
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
{    16}
};

#if 0
/* PTin added: new switch 5664x (Triumph3) SF */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{
{       0,       0,          0,      17,           1,      "" }, /* 10G ETH */
{       0,       1,          0,      21,           1,      "" },
{       0,       2,          0,      25,           1,      "" },
{       0,       3,          0,      29,           1,      "" },
{       0,       4,          0,       5,           1,      "" }, /* 40G ETH */
{       0,       5,          0,       9,           1,      "" },
{       0,       6,          0,      13,           1,      "" },
{       0,       7,          0,       1,           1,      "" }, /* 1G ETH */
};
#else
/* PTin added: new switch 5664x (Triumph3) SF */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{
{       0,       0,          0,      17,           1,      "" }, /* 10G ETH */
{       0,       1,          0,      21,           1,      "" },
{       0,       2,          0,      25,           1,      "" },
{       0,       3,          0,      29,           1,      "" },
{       0,       4,          0,       5,           1,      "" }, /* 40G ETH */
{       0,       5,          0,       6,           1,      "" }, /* 40G ETH */
{       0,       6,          0,       7,           1,      "" }, /* 40G ETH */
{       0,       7,          0,       8,           1,      "" }, /* 40G ETH */
{       0,       8,          0,       9,           1,      "" },
{       0,       9,          0,      10,           1,      "" },
{       0,      10,          0,      11,           1,      "" },
{       0,      11,          0,      12,           1,      "" },
{       0,      12,          0,      13,           1,      "" },
{       0,      13,          0,      14,           1,      "" },
{       0,      14,          0,      15,           1,      "" },
{       0,      15,          0,      16,           1,      "" },
{       0,      16,          0,       1,           1,      "" }, /* 1G ETH */
};
#endif

/* PTin added: new switch 5664x (Triumph3) SF */
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_4_10G_3_40G_1_GIG_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_4_10G_3_40G_1_GIG_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
#if (PTIN_BOARD == PTIN_BOARD_CXO160G)
dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56640_REV_1, sizeof(dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56640_REV_1)/sizeof(L7_uint32),
dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1, sizeof(dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1)/sizeof(HAPI_WC_PORT_MAP_t)
#endif
};

/* PTin added: new switch 5664x (Triumph3) SF */
SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[]= 
{
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
/*{L7_PORT_DESC_BCOM_40G_KR4},{L7_PORT_DESC_BCOM_40G_KR4},{L7_PORT_DESC_BCOM_40G_KR4},*/
{L7_PORT_DESC_BCOM_1G_AN},
};


/* PTin added: new switch 56340 (Helix4) */

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_REV_1[] =
/*portNum
 *------- */
{{    0},
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
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{
{       0,       0,          0,       1,           1,      "" }, /* 2.5G ETH */
{       0,       1,          0,       5,           1,      "" },
{       0,       2,          0,       9,           1,      "" },
{       0,       3,          0,      13,           1,      "" },
{       0,       4,          0,      17,           1,      "" },
{       0,       5,          0,      21,           1,      "" },
{       0,       6,          0,      25,           1,      "" },
{       0,       7,          0,      29,           1,      "" },
{       0,       8,          0,      33,           1,      "" }, /* 1G ETH */
{       0,       9,          0,      41,           1,      "" },
{       0,      10,          0,      37,           1,      "" },
{       0,      11,          0,      45,           1,      "" },
{       0,      12,          0,      53,           1,      "" }, /* 10G ETH */
{       0,      13,          0,      52,           1,      "" },
{       0,      14,          0,      51,           1,      "" },
{       0,      15,          0,      50,           1,      "" },
{       0,      16,          0,      54,           1,      "" }, /* 1G port for FPGA */
{       0,      17,          0,      49,           1,      "" }, /* GS port */
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1[]= 
{
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},  /* 8xGPON + 4x1G ports + 4x10G + GS */
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_SGMII_1G},{L7_PORT_DESC_BCOM_1G_NO_AN},
};


/* PTin added: new switch 56450 (Katana2) - OLT1T0F */

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_K2_REV_1[] =
/*portNum
 *------- */
{{    0},
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
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{
{       0,       0,          0,       1,           1,      "" }, /* 2.5G ETH */
{       0,       1,          0,       2,           1,      "" },
{       0,       2,          0,       3,           1,      "" },
{       0,       3,          0,       4,           1,      "" },
{       0,       4,          0,       5,           1,      "" },
{       0,       5,          0,       6,           1,      "" },
{       0,       6,          0,       7,           1,      "" },
{       0,       7,          0,       8,           1,      "" },
{       0,       8,          0,      28,           1,      "" }, /* 1G ETH */
{       0,       9,          0,      29,           1,      "" },
{       0,      10,          0,      30,           1,      "" },
{       0,      11,          0,      31,           1,      "" },
{       0,      12,          0,      27,           1,      "" }, /* 10G ETH */
{       0,      13,          0,      25,           1,      "" },
{       0,      14,          0,      33,           1,      "" },
{       0,      15,          0,      36,           1,      "" },
{       0,      16,          0,      40,           1,      "" }, /* GS port for FPGA */
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_K2_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_K2_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1[]= 
{
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},  /* 8xGPON + 4x1G ports + 4x10G + GS */
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_2G5_NO_AN},
};


/* PTin added: new switch 56450 (Katana2) - TG16GF && AG16GA */

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_16_GIG_4_TENGIG_1_GS_K2_REV_1[] =
/*portNum
 *------- */
{{    0},
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
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_16_GIG_4_TENGIG_1_GS_56450_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{
{       0,       0,          0,       1,           1,      "" }, /* 2.5G ETH */
{       0,       1,          0,       2,           1,      "" },
{       0,       2,          0,       3,           1,      "" },
{       0,       3,          0,       4,           1,      "" },
{       0,       4,          0,       5,           1,      "" },
{       0,       5,          0,       6,           1,      "" },
{       0,       6,          0,       7,           1,      "" },
{       0,       7,          0,       8,           1,      "" },
{       0,       8,          0,       9,           1,      "" },
{       0,       9,          0,      10,           1,      "" },
{       0,      10,          0,      11,           1,      "" },
{       0,      11,          0,      12,           1,      "" },
{       0,      12,          0,      13,           1,      "" },
{       0,      13,          0,      14,           1,      "" },
{       0,      14,          0,      15,           1,      "" },
{       0,      15,          0,      16,           1,      "" },
{       0,      16,          0,      36,           1,      "" }, /* 10G ETH */
{       0,      17,          0,      33,           1,      "" },
{       0,      18,          0,      25,           1,      "" },
{       0,      19,          0,      27,           1,      "" },
{       0,      20,          0,      40,           1,      "" }, /* GS port for FPGA */
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_16_GIG_4_TENGIG_1_GS_56450_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_16_GIG_4_TENGIG_1_GS_56450_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_16_GIG_4_TENGIG_1_GS_56450_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_16_GIG_4_TENGIG_1_GS_K2_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_16_GIG_4_TENGIG_1_GS_K2_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_16_GIG_4_TENGIG_1_GS_56450_REV_1[]= 
{
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},{L7_PORT_DESC_BCOM_2G5_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},  /* 16xGPON + 4x10G + GS */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},
};


/* PTin added: new switch 56450 (Katana2) - TT04SXG */

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_8_TENGIG_1_GS_K2_REV_1[] =
/*portNum
 *------- */
{{    0},
{     1},
{     2},
{     3},
{     4},
{     5},
{     6},
{     7},
{     8},
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_8_TENGIG_1_GS_56450_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{
{       0,       0,          0,      27,           1,      "" }, /* 10G ETH to FPGA */
{       0,       1,          0,      25,           1,      "" },
{       0,       2,          0,      33,           1,      "" },
{       0,       3,          0,      36,           1,      "" },
{       0,       4,          0,      39,           1,      "" }, /* 10G ETH to Backplane (via Cortina) */
{       0,       5,          0,      30,           1,      "" },
{       0,       6,          0,      26,           1,      "" },
{       0,       7,          0,      28,           1,      "" },
{       0,       8,          0,       1,           1,      "" }, /* GS port for FPGA */
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_8_TENGIG_1_GS_56450_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_8_TENGIG_1_GS_56450_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_8_TENGIG_1_GS_56450_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_8_TENGIG_1_GS_K2_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_8_TENGIG_1_GS_56450_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_8_TENGIG_1_GS_56450_REV_1[]= 
{
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},  /* 4xNGPON + 4x10G + GS */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_2G5_NO_AN},
};


/* PTin added: new switch 56170 (Hurricane3-MG/Greyhound2) */

HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1[] =
/*portNum
 *------- */
{{    0},
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
};

HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1[] = 
/* slotNum  portNum bcm_cpuunit bcm_port is_hw_mapped portName *
 * -------  ------- ----------- -------- ------------ -------- */
{{      0,       0,          0,      6,           1,       "" },    /* SGMII_4P1:0 NIF 6 , bcm_port 6  */
{       0,       1,          0,      7,           1,       "" },    /* SGMII_4P1:1 NIF 7 , bcm_port 7  */
{       0,       2,          0,      8,           1,       "" },    /* SGMII_4P1:2 NIF 8 , bcm_port 8  */
{       0,       3,          0,      9,           1,       "" },    /* SGMII_4P1:3 NIF 9 , bcm_port 9  */
{       0,       4,          0,      2,           1,       "" },    /* SGMII_4P0:0 NIF 2 , bcm_port 2  */
{       0,       5,          0,      3,           1,       "" },    /* SGMII_4P0:1 NIF 3 , bcm_port 3  */
{       0,       6,          0,      4,           1,       "" },    /* SGMII_4P0:2 NIF 4 , bcm_port 4  */
{       0,       7,          0,      5,           1,       "" },    /* SGMII_4P0:3 NIF 5 , bcm_port 5  */
{       0,       8,          0,     14,           1,       "" },    /* SGMII_4P3:0 NIF 14, bcm_port 14 */
{       0,       9,          0,     15,           1,       "" },    /* SGMII_4P3:1 NIF 15, bcm_port 15 */
{       0,      10,          0,     16,           1,       "" },    /* SGMII_4P3:2 NIF 16, bcm_port 16 */
{       0,      11,          0,     17,           1,       "" },    /* SGMII_4P3:3 NIF 17, bcm_port 17 */
{       0,      12,          0,     10,           1,       "" },    /* SGMII_4P2:0 NIF 10, bcm_port 10 */
{       0,      13,          0,     11,           1,       "" },    /* SGMII_4P2:1 NIF 11, bcm_port 11 */
{       0,      14,          0,     12,           1,       "" },    /* SGMII_4P2:2 NIF 12, bcm_port 12 */
{       0,      15,          0,     13,           1,       "" },    /* SGMII_4P2:3 NIF 13, bcm_port 13 */
{       0,      16,          0,     34,           1,       "" },    /* TSC4E0:3 NIF 61, bcm_port 34 */
{       0,      17,          0,     25,           1,       "" },    /* SGMII_4P5:3 NIF 25, bcm_port 25 */
{       0,      18,          0,     35,           1,       "" },    /* TSC4E0:1 NIF 59, bcm_port 35 */
{       0,      19,          0,     24,           1,       "" },    /* SGMII_4P5:2 NIF 24, bcm_port 24 */
{       0,      20,          0,     36,           1,       "" },    /* TSC4E0:2 NIF 60, bcm_port 36 */
{       0,      21,          0,     23,           1,       "" },    /* SGMII_4P5:1 NIF 23, bcm_port 23 */
{       0,      22,          0,     37,           1,       "" },    /* TSC4E0:0 NIF 58, bcm_port 37 */
{       0,      23,          0,     22,           1,       "" },    /* SGMII_4P5:0 NIF 22, bcm_port 22 */
{       0,      24,          0,     38,           1,       "" },    /* TSC4E1:3 NIF 65, bcm_port 38 */
{       0,      25,          0,     21,           1,       "" },    /* SGMII_4P4:3 NIF 21, bcm_port 21 */
{       0,      26,          0,     39,           1,       "" },    /* TSC4E1:1 NIF 63, bcm_port 39 */
{       0,      27,          0,     20,           1,       "" },    /* SGMII_4P4:2 NIF 20, bcm_port 20 */
{       0,      28,          0,     40,           1,       "" },    /* TSC4E1:2 NIF 64, bcm_port 40 */
{       0,      29,          0,     19,           1,       "" },    /* SGMII_4P4:1 NIF 19, bcm_port 19 */
{       0,      30,          0,     41,           1,       "" },    /* TSC4E1:0 NIF 62, bcm_port 41 */
{       0,      31,          0,     18,           1,       "" },    /* SGMII_4P4:0 NIF 18, bcm_port 18 */
{       0,      32,          0,     42,           1,       "" },    /* TSC4E2:3 NIF 69, bcm_port 42 */
{       0,      33,          0,     26,           1,       "" },    /* TSC4Q0:0    NIF 26, bcm_port 26 */
{       0,      34,          0,     43,           1,       "" },    /* TSC4E2:1 NIF 67, bcm_port 43 */
{       0,      35,          0,     27,           1,       "" },    /* TSC4Q0:1    NIF 27, bcm_port 27 */
{       0,      36,          0,     44,           1,       "" },    /* TSC4E2:2 NIF 68, bcm_port 44 */
{       0,      37,          0,     28,           1,       "" },    /* TSC4Q0:2    NIF 28, bcm_port 28 */
{       0,      38,          0,     45,           1,       "" },    /* TSC4E2:0 NIF 66, bcm_port 45 */
{       0,      39,          0,     29,           1,       "" },    /* TSC4Q0:3    NIF 29, bcm_port 29 */
{       0,      40,          0,     46,           1,       "" },    /* TSC4E3:3 NIF 73, bcm_port 46 */
{       0,      41,          0,     30,           1,       "" },    /* TSC4Q1:0    NIF 42, bcm_port 30 */
{       0,      42,          0,     47,           1,       "" },    /* TSC4E3:1 NIF 71 (TX) / TSC4E3:2 NIF 72 (RX), bcm_port 47 */
{       0,      43,          0,     31,           1,       "" },    /* TSC4Q1:1    NIF 43, bcm_port 31 */
{       0,      44,          0,     48,           1,       "" },    /* TSC4E3:2 NIF 72 (TX) / TSC4E3:1 NIF 71 (RX), bcm_port 48 */
{       0,      45,          0,     32,           1,       "" },    /* TSC4Q1:2    NIF 44, bcm_port 32 */
{       0,      46,          0,     49,           1,       "" },    /* TSC4E3:0 NIF 70, bcm_port 49 */
{       0,      47,          0,     33,           1,       "" },    /* TSC4Q1:3    NIF 45, bcm_port 33 */
{       0,      48,          0,     62,           1,       "" },    /* W0: TSC4F:0  NIF 86, bcm_port 62 */
{       0,      49,          0,     63,           1,       "" },    /* W1: TSC4F:1  NIF 87, bcm_port 63 */
{       0,      50,          0,     64,           1,       "" },    /* W2: TSC4F:2  NIF 88, bcm_port 64 */
{       0,      51,          0,     65,           1,       "" },    /* W3: TSC4F:3  NIF 89, bcm_port 65 */
{       0,      52,          0,     56,           1,       "" },    /* W4: TSC4E5:0 NIF 78, bcm_port 56 */
{       0,      53,          0,     57,           1,       "" },    /* W5: TSC4E5:2 NIF 80, bcm_port 57 */
{       0,      54,          0,     52,           1,       "" },    /* W6: TSC4E4:0 NIF 74, bcm_port 52 */
{       0,      55,          0,     53,           1,       "" },    /* W7: TSC4E4:2 NIF 76, bcm_port 53 */
{       0,      56,          0,     58,           1,       "" },    /* P0: TSC4E6:3 NIF 85 (TX) / TSC4E6:0 NIF 82 (RX), bcm_port 58 */
{       0,      57,          0,     59,           1,       "" },    /* P1: TSC4E6:2 NIF 84, bcm_port 59 */
{       0,      58,          0,     60,           1,       "" },    /* P2: TSC4E6:1 NIF 83, bcm_port 60 */
{       0,      59,          0,     61,           1,       "" },    /* P3: TSC4E6:0 NIF 82 (TX) / TSC4E6:3 NIF 85 (RX), bcm_port 61 */
{       0,      60,          0,     54,           1,       "" },    /* P4: TSC4E5:1 NIF 79, bcm_port 54 */
{       0,      61,          0,     55,           1,       "" },    /* P5: TSC4E5:3 NIF 81, bcm_port 55 */
{       0,      62,          0,     50,           1,       "" },    /* P6: TSC4E4:1 NIF 75, bcm_port 50 */
{       0,      63,          0,     51,           1,       "" },    /* P7: TSC4E4:3 NIF 77, bcm_port 51 */
};

DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1 = {
hapiBroadPhysicalCardInsert,hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1[]= 
{
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1_MODE1[]= 
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
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
};

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_24PLUS8_TENGIG_56170_REV_1_MODE2[]= 
{
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_1G_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN}, {L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
};

