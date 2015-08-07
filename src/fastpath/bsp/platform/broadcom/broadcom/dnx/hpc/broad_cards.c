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
 * Broadcom reference platform with 48GE & 4 Dedicated stacking ports     *
 *                                                                             *
 ******************************************************************************/

/* PTin added: new switch ARAD */
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_12_ETH_4_BCK_REV_1[] =
{
{ 0,  L7_TRUE,   16, 1},   /* physical slot with 16 port card (12x10G + 4x10G) */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 30},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 38}    /* logical Router card */
};

/* PTin added: new switch ARAD */
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_12_ETH_4_BCK_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 13},
  {0, 14},
  {0, 15},
  {0, 16}
};


/******************************************************************************
 ******************************************************************************
 *                                UNIT ENTRIES                                *
 ******************************************************************************
 *****************************************************************************/


/* PTin added: new switch ARAD */
/***********************************************************************************
 * Broadcom ARAD unit with 12x10GB + 4x10GB fixed ports on a single slot.          *
 *                                                                                 *
 ***********************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_12_ETH_4_BCK_88650_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_12_ETH_4_BCK_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_12_ETH_4_BCK_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_12_ETH_4_BCK_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_12_ETH_4_BCK_REV_1     /* stk_port_list */
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

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_12_ETH_4_BCK_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port portName *
 * -------  -------  ----------- -------- -------- */
{{ 0xFFFF,  0xFFFF,          0,        0,       "" },
};


/* PTin added: new switch ARAD */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_12_ETH_4_BCK_88650_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port portName *
 * -------  ------- ----------- -------- -------- */
{
{       0,       0,          0,      12,       "" }, /* 10G ETH FRONT */
{       0,       1,          0,      11,       "" },
{       0,       2,          0,      10,       "" },
{       0,       3,          0,       9,       "" },
{       0,       4,          0,       5,       "" },
{       0,       5,          0,       6,       "" },
{       0,       6,          0,       7,       "" },
{       0,       7,          0,       8,       "" },
{       0,       8,          0,       1,       "" },
{       0,       9,          0,       2,       "" },
{       0,      10,          0,       3,       "" },
{       0,      11,          0,       4,       "" },
{       0,      12,          0,      17,       "" }, /* 10G ETH BACKPLANE */
{       0,      13,          0,      18,       "" },
{       0,      14,          0,      19,       "" },
{       0,      15,          0,      20,       "" }
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

/* PTin added: new switch 56340 (Helix4) */
HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_12_ETH_4_BCK_REV_1[] =
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
{    15}
};

/****************************************************************
*
*  Line Card Init Variables
*
****************************************************************/
/* PTin added: new switch 56340 (Helix4) */
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_12_ETH_4_BCK_88650_REV_1 = {
hapiBroadPhysicalCardInsert, hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_12_ETH_4_BCK_88650_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_12_ETH_4_BCK_88650_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_12_ETH_4_BCK_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_12_ETH_4_BCK_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

/****************************************************************
*
*  Line Card Init Variables
*
****************************************************************/

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_12_ETH_4_BCK_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_12_ETH_4_BCK_REV_1,
sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_12_ETH_4_BCK_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0
};


/* PTin added: new switch ARAD */
SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_12_ETH_4_BCK_88650_REV_1[]= 
{
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},  /* 12x10G_ETH + 4x10G_BCK */
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
};

