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

/* PTin added: new switch Q-MX 144 */
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_CXO2T4_REV_1[] =
{
{ 0, L7_TRUE,  144,   1},   /* physical slot with 72 port card */
{ 1, L7_FALSE,   1,   0},   /* logical CPU card */
{ 2, L7_TRUE,    8, 150},   /* logical LAG card */
{ 3, L7_TRUE,    8, 158}    /* logical Router card */
};

/* PTin added: new switch ARAD 12+3 */
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_12_ETH_4_BCK_REV_1[] =
{
{ 0,  L7_TRUE,   15, 1},   /* physical slot with 16 port card (12x10G + 4x10G) */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 20},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 28}    /* logical Router card */
};

/* PTin added: new switch ARAD 8+3 */
HPC_BROAD_SLOT_MAC_ALLOCATION_t mac_allocation_UNIT_BROAD_8_ETH_3_BCK_REV_1[] =
{
{ 0,  L7_TRUE,   11, 1},   /* physical slot with 12 port card (8x10G + 4x10G) */
{ 1,  L7_FALSE,  1,  0},   /* logical CPU card */
{ 2,  L7_TRUE,   8, 20},   /* logical LAG card */
{ 3,  L7_TRUE,   8, 28}    /* logical Router card */
};


/* PTin added: new switch Q-MX 144 */
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_CXO2T4_REV_1[] =
{
 /* bcm_unit, bcm_port */
};

/* PTin added: new switch ARAD 12+3 */
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_12_ETH_4_BCK_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 17},
  {0, 18},
  {0, 19}
};

/* PTin added: new switch ARAD 8+3 */
HPC_BROAD_STK_PORT_TABLE_ENTRY_t stack_port_data_UNIT_BROAD_8_ETH_3_BCK_REV_1[] =
{
 /* bcm_unit, bcm_port */
  {0, 17},
  {0, 18},
  {0, 19}
};

/******************************************************************************
 ******************************************************************************
 *                                UNIT ENTRIES                                *
 ******************************************************************************
 *****************************************************************************/

/* PTin added: new switch Q-MX 144 ports */
/***********************************************************************************
 * Broadcom ARAD unit with 12x10GB + 3x40GB fixed ports on a single slot.          *
 *                                                                                 *
 ***********************************************************************************/
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_CXO2T4_88370_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_CXO2T4_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_CXO2T4_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  2,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_CXO2T4_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_CXO2T4_REV_1     /* stk_port_list */
#else
  0,
  L7_NULLPTR
#endif
}
};

/* PTin added: new switch ARAD 12+3 */
/***********************************************************************************
 * Broadcom ARAD unit with 12x10GB + 3x40GB fixed ports on a single slot.          *
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

/* PTin added: new switch ARAD 8+3 */
HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_8_ETH_3_BCK_88650_REV_1[] =
{
{
  mac_allocation_UNIT_BROAD_8_ETH_3_BCK_REV_1,
  sizeof(mac_allocation_UNIT_BROAD_8_ETH_3_BCK_REV_1)/sizeof(HPC_BROAD_SLOT_MAC_ALLOCATION_t),

  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                  /* mod_ids_req   */
  0,                                                  /* slot_id       */
  1,                                                  /* num_units     */
  0,                                                  /* dest_unit     */
  0,                                                  /* dest_port     */
#ifdef L7_STACKING_PACKAGE
  sizeof(stack_port_data_UNIT_BROAD_8_ETH_3_BCK_REV_1)/sizeof(HPC_BROAD_STK_PORT_TABLE_ENTRY_t),  /* num_stk_ports */
  stack_port_data_UNIT_BROAD_8_ETH_3_BCK_REV_1     /* stk_port_list */
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

HAPI_CARD_SLOT_MAP_t dapiBroadCpuCardSlotMap_CARD_BROAD_GENERIC_REV_1[] =
/* slotNum  portNum  bcm_cpuunit bcm_port portName *
 * -------  -------  ----------- -------- -------- */
{{ 0xFFFF,  0xFFFF,          0,        0,       "" },
};

/* PTin added: new switch Q-MX 144 ports */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_CXO2T4_88370_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port portName *
 * -------  ------- ----------- -------- -------- */
{
{       0,       0,          1,       1,       "" },
{       0,       1,          1,       2,       "" },
{       0,       2,          1,       3,       "" },
{       0,       3,          1,       4,       "" },
{       0,       4,          0,       1,       "" },
{       0,       5,          0,       2,       "" },
{       0,       6,          0,       3,       "" },
{       0,       7,          0,       4,       "" },
{       0,       8,          1,       5,       "" },
{       0,       9,          1,       6,       "" },
{       0,      10,          1,       7,       "" },
{       0,      11,          1,       8,       "" },
{       0,      12,          0,       5,       "" },
{       0,      13,          0,       6,       "" },
{       0,      14,          0,       7,       "" },
{       0,      15,          0,       8,       "" },
{       0,      16,          1,       9,       "" },
{       0,      17,          1,      10,       "" },
{       0,      18,          1,      11,       "" },
{       0,      19,          1,      12,       "" },
{       0,      20,          0,       9,       "" },
{       0,      21,          0,      10,       "" },
{       0,      22,          0,      11,       "" },
{       0,      23,          0,      12,       "" },
{       0,      24,          1,      13,       "" },
{       0,      25,          1,      14,       "" },
{       0,      26,          1,      15,       "" },
{       0,      27,          1,      16,       "" },
{       0,      28,          0,      13,       "" },
{       0,      29,          0,      14,       "" },
{       0,      30,          0,      15,       "" },
{       0,      31,          0,      16,       "" },
{       0,      32,          1,      17,       "" },
{       0,      33,          1,      18,       "" },
{       0,      34,          1,      19,       "" },
{       0,      35,          1,      20,       "" },
{       0,      36,          0,      17,       "" },
{       0,      37,          0,      18,       "" },
{       0,      38,          0,      19,       "" },
{       0,      39,          0,      20,       "" },
{       0,      40,          1,      21,       "" },
{       0,      41,          1,      22,       "" },
{       0,      42,          1,      23,       "" },
{       0,      43,          1,      24,       "" },
{       0,      44,          0,      21,       "" },
{       0,      45,          0,      22,       "" },
{       0,      46,          0,      23,       "" },
{       0,      47,          0,      24,       "" },
{       0,      48,          1,      25,       "" },
{       0,      49,          1,      26,       "" },
{       0,      50,          1,      27,       "" },
{       0,      51,          1,      28,       "" },
{       0,      52,          0,      25,       "" },
{       0,      53,          0,      26,       "" },
{       0,      54,          0,      27,       "" },
{       0,      55,          0,      28,       "" },
{       0,      56,          1,      29,       "" },
{       0,      57,          1,      30,       "" },
{       0,      58,          1,      31,       "" },
{       0,      59,          1,      32,       "" },
{       0,      60,          0,      29,       "" },
{       0,      61,          0,      30,       "" },
{       0,      62,          0,      31,       "" },
{       0,      63,          0,      32,       "" },
{       0,      64,          1,      33,       "" },
{       0,      65,          1,      34,       "" },
{       0,      66,          1,      35,       "" },
{       0,      67,          1,      36,       "" },
{       0,      68,          0,      33,       "" },
{       0,      69,          0,      34,       "" },
{       0,      70,          0,      35,       "" },
{       0,      71,          0,      36,       "" },
{       0,      72,          1,      37,       "" },
{       0,      73,          1,      38,       "" },
{       0,      74,          1,      39,       "" },
{       0,      75,          1,      40,       "" },
{       0,      76,          0,      37,       "" },
{       0,      77,          0,      38,       "" },
{       0,      78,          0,      39,       "" },
{       0,      79,          0,      40,       "" },
{       0,      80,          1,      41,       "" },
{       0,      81,          1,      42,       "" },
{       0,      82,          1,      43,       "" },
{       0,      83,          1,      44,       "" },
{       0,      84,          0,      41,       "" },
{       0,      85,          0,      42,       "" },
{       0,      86,          0,      43,       "" },
{       0,      87,          0,      44,       "" },
{       0,      88,          1,      45,       "" },
{       0,      89,          1,      46,       "" },
{       0,      90,          1,      47,       "" },
{       0,      91,          1,      48,       "" },
{       0,      92,          0,      45,       "" },
{       0,      93,          0,      46,       "" },
{       0,      94,          0,      47,       "" },
{       0,      95,          0,      48,       "" },
{       0,      96,          1,      49,       "" },
{       0,      97,          1,      50,       "" },
{       0,      98,          1,      51,       "" },
{       0,      99,          1,      52,       "" },
{       0,     100,          0,      49,       "" },
{       0,     101,          0,      50,       "" },
{       0,     102,          0,      51,       "" },
{       0,     103,          0,      52,       "" },
{       0,     104,          1,      53,       "" },
{       0,     105,          1,      54,       "" },
{       0,     106,          1,      55,       "" },
{       0,     107,          1,      56,       "" },
{       0,     108,          0,      53,       "" },
{       0,     109,          0,      54,       "" },
{       0,     110,          0,      55,       "" },
{       0,     111,          0,      56,       "" },
{       0,     112,          1,      57,       "" },
{       0,     113,          1,      58,       "" },
{       0,     114,          1,      59,       "" },
{       0,     115,          1,      60,       "" },
{       0,     116,          0,      57,       "" },
{       0,     117,          0,      58,       "" },
{       0,     118,          0,      59,       "" },
{       0,     119,          0,      60,       "" },
{       0,     120,          1,      61,       "" },
{       0,     121,          1,      62,       "" },
{       0,     122,          1,      63,       "" },
{       0,     123,          1,      64,       "" },
{       0,     124,          0,      61,       "" },
{       0,     125,          0,      62,       "" },
{       0,     126,          0,      63,       "" },
{       0,     127,          0,      64,       "" },
{       0,     128,          1,      65,       "" },
{       0,     129,          1,      66,       "" },
{       0,     130,          1,      67,       "" },
{       0,     131,          1,      68,       "" },
{       0,     132,          0,      65,       "" },
{       0,     133,          0,      66,       "" },
{       0,     134,          0,      67,       "" },
{       0,     135,          0,      68,       "" },
{       0,     136,          1,      69,       "" },
{       0,     137,          1,      70,       "" },
{       0,     138,          1,      71,       "" },
{       0,     139,          1,      72,       "" },
{       0,     140,          0,      69,       "" },
{       0,     141,          0,      70,       "" },
{       0,     142,          0,      71,       "" },
{       0,     143,          0,      72,       "" },
};

/* PTin added: new switch ARAD 12+3 */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_12_ETH_4_BCK_88650_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port portName *
 * -------  ------- ----------- -------- -------- */
{
{       0,       0,          0,       9,       "" }, /* 10G ETH FRONT */
{       0,       1,          0,      10,       "" },
{       0,       2,          0,      11,       "" },
{       0,       3,          0,      12,       "" },
{       0,       4,          0,       8,       "" },
{       0,       5,          0,       7,       "" },
{       0,       6,          0,       6,       "" },
{       0,       7,          0,       5,       "" },
{       0,       8,          0,       4,       "" },
{       0,       9,          0,       3,       "" },
{       0,      10,          0,       2,       "" },
{       0,      11,          0,       1,       "" },
{       0,      12,          0,      17,       "" }, /* 40G ETH BACKPLANE */
{       0,      13,          0,      18,       "" },
{       0,      14,          0,      19,       "" }
};

/* PTin added: new switch ARAD 8+3 */
HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_8_ETH_3_BCK_88650_REV_1[] =
/* slotNum  portNum bcm_cpuunit bcm_port portName *
 * -------  ------- ----------- -------- -------- */
{
{       0,       0,          0,       5,       "" }, /* 10G ETH FRONT */
{       0,       1,          0,       6,       "" },
{       0,       2,          0,       7,       "" },
{       0,       3,          0,       8,       "" },
{       0,       4,          0,       1,       "" },
{       0,       5,          0,       2,       "" },
{       0,       6,          0,       3,       "" },
{       0,       7,          0,       4,       "" },
{       0,       8,          0,      17,       "" }, /* 40G ETH BACKPLANE */
{       0,       9,          0,      18,       "" },
{       0,      10,          0,      19,       "" }
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

/* PTin added: new switch Q-MX 144 ports */
HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_CXO2T4_REV_1[] =
/*portNum
 *------- */
{{   0}, {   1}, {   2}, {   3},
 {   4}, {   5}, {   6}, {   7},
 {   8}, {   9}, {  10}, {  11},
 {  12}, {  13}, {  14}, {  15},
 {  16}, {  17}, {  18}, {  19},
 {  20}, {  21}, {  22}, {  23},
 {  24}, {  25}, {  26}, {  27},
 {  28}, {  29}, {  30}, {  31},
 {  32}, {  33}, {  34}, {  35},
 {  36}, {  37}, {  38}, {  39},
 {  40}, {  41}, {  42}, {  43},
 {  44}, {  45}, {  46}, {  47},
 {  48}, {  49}, {  50}, {  51},
 {  52}, {  53}, {  54}, {  55},
 {  56}, {  57}, {  58}, {  59},
 {  60}, {  61}, {  62}, {  63},
 {  64}, {  65}, {  66}, {  67},
 {  68}, {  69}, {  70}, {  71},
 {  72}, {  73}, {  74}, {  75},
 {  76}, {  77}, {  78}, {  79},
 {  80}, {  81}, {  82}, {  83},
 {  84}, {  85}, {  86}, {  87},
 {  88}, {  89}, {  90}, {  91},
 {  92}, {  93}, {  94}, {  95},
 {  96}, {  97}, {  98}, {  99},
 { 100}, { 101}, { 102}, { 103},
 { 104}, { 105}, { 106}, { 107},
 { 108}, { 109}, { 110}, { 111},
 { 112}, { 113}, { 114}, { 115},
 { 116}, { 117}, { 118}, { 119},
 { 120}, { 121}, { 122}, { 123},
 { 124}, { 125}, { 126}, { 127},
 { 128}, { 129}, { 130}, { 131},
 { 132}, { 133}, { 134}, { 135},
 { 136}, { 137}, { 138}, { 139},
 { 140}, { 141}, { 142}, { 143},
};

/* PTin added: new switch ARAD 12+3 */
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
{    14}
};

/* PTin added: new switch ARAD 8+3 */
HAPI_CARD_PORT_MAP_t dapiBroadBaseCardPortMap_CARD_BROAD_8_ETH_3_BCK_REV_1[] =
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
{    10}
};

/****************************************************************
*
*  Line Card Init Variables
*
****************************************************************/

/* PTin added: new switch Q-MX 144 ports */
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_CXO2T4_88370_REV_1 = {
hapiBroadPhysicalCardInsert, hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_CXO2T4_88370_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_CXO2T4_88370_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_CXO2T4_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_CXO2T4_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

/* PTin added: new switch ARAD 12+3 */
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_12_ETH_4_BCK_88650_REV_1 = {
hapiBroadPhysicalCardInsert, hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_12_ETH_4_BCK_88650_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_12_ETH_4_BCK_88650_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_12_ETH_4_BCK_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_12_ETH_4_BCK_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

/* PTin added: new switch ARAD 8+3 */
DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_8_ETH_3_BCK_88650_REV_1 = {
hapiBroadPhysicalCardInsert, hapiBroadCardRemove,
dapiBroadBaseCardSlotMap_CARD_BROAD_8_ETH_3_BCK_88650_REV_1, sizeof(dapiBroadBaseCardSlotMap_CARD_BROAD_8_ETH_3_BCK_88650_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
dapiBroadBaseCardPortMap_CARD_BROAD_8_ETH_3_BCK_REV_1, sizeof(dapiBroadBaseCardPortMap_CARD_BROAD_8_ETH_3_BCK_REV_1)/sizeof(HAPI_CARD_PORT_MAP_t),
0, NULL, 0,
};

/****************************************************************
*
*  Line Card Init Variables
*
****************************************************************/

DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_GENERIC_REV_1 = {

hapiBroadCpuCardInsert,hapiBroadCardRemove,
dapiBroadCpuCardSlotMap_CARD_BROAD_GENERIC_REV_1,
sizeof(dapiBroadCpuCardSlotMap_CARD_BROAD_GENERIC_REV_1)/sizeof(HAPI_CARD_SLOT_MAP_t),
NULL, 0,
0, NULL, 0
};


/* PTin added: new switch Q-MX 144 ports */
SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_CXO2T4_88370_REV_1[]= 
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
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},{L7_PORT_DESC_BCOM_XAUI_10G_NO_AN},
};

/* PTin added: new switch ARAD 12+3 */
SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_12_ETH_4_BCK_88650_REV_1[]= 
{
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_40G_XLAUI}  ,{L7_PORT_DESC_BCOM_40G_XLAUI}  ,{L7_PORT_DESC_BCOM_40G_XLAUI}   /* 12x10G_ETH + 3x40G_BCK */
};

/* PTin added: new switch ARAD 8+3 */
SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_8_ETH_3_BCK_88650_REV_1[]= 
{
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},{L7_PORT_DESC_BCOM_XAUI_10G_1G},
{L7_PORT_DESC_BCOM_40G_XLAUI}  ,{L7_PORT_DESC_BCOM_40G_XLAUI}  ,{L7_PORT_DESC_BCOM_40G_XLAUI},  /* 8x10G_ETH + 3x40G_BCK */
};

