/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  broad_hpc_db.h
*
* @purpose   Declare and initialize storage for use in hpc card
*            and unit descriptor databases.
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
#include "sysbrds.h"
#include "dapi_db.h"

#ifndef INCLUDE_BROAD_HPC_DB_H
#define INCLUDE_BROAD_HPC_DB_H

extern int bcm_sys_sa_init_56624(const bcm_sys_board_t *brd, int base);
extern int bcm_sys_sa_init_56680(const bcm_sys_board_t *brd, int base);
extern int bcm_sys_sa_init_56820(const bcm_sys_board_t *brd, int base);

#ifdef PTIN_WC_SLOT_MAP
extern HAPI_WC_SLOT_MAP_t dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[];
extern HAPI_WC_PORT_MAP_t dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[];
extern HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[];
#endif

extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1[];   /* PTin added (2.5) */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_40_TENGIG_56843_REV_1[];         /* PTin added: new switch BCM56843 (trident) */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_64_TENGIG_56846_REV_1[];         /* PTin added: new platform BCM56846 OLT1T3 (trident) */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_20_TENGIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_TENGIG_4_GIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1[];

extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56680_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_TENGIG_4_GIG_56820_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_8_HIGIG_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_40_TENGIG_56843_REV_1[];                         /* PTin added: new switch BCM56843 (trident) */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_64_TENGIG_56846_REV_1[];                         /* PTin added: new platform BCM56846 OLT1T3 (trident) */

extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1;       // PTin added: new platform
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_40_TENGIG_56843_REV_1;             /* PTin added: new switch BCM56843 (trident) */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_64_TENGIG_56846_REV_1;             /* PTin added: new platform BCM56846 OLT1T3 (trident) */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_20_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_40_TENGIG_REV_1;                        /* PTin added: new switch BCM56843 (trident) */
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_64_TENGIG_REV_1;                        /* PTin added: new platform BCM56846 OLT1T3 (trident) */
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_20_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_8_HIGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_GIG_2_TENGIG_REV_2;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1;

extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_8_HIGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_2_TENGIG_REV_2;

/************************************************************************
 * Adding a COMPILE MACRO to get the flexibility in listing GE ports in 
 * the show commands.
 * [1] 4 GE Ports can be listed first in the port listing and than 10G ports follows
 *
 *     Define and Set SC_1G_PORTS_FIRST to 1.
 *
 * [2] 4 GE Ports can be listed last following 10G ports. {default}
 *
 *     Define and Set SC_1G_PORTS_FIRST to 1.
 *
 *  Note: Intended for Scorpion platforms.
 *
 ***********************************************************************/
#undef SC_1G_PORTS_FIRST

#endif
