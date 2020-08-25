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

#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
extern L7_uint32  dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56846_REV_1[];
extern HAPI_WC_SLOT_MAP_t dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_WORK[];
extern HAPI_WC_SLOT_MAP_t dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_PROT[];
extern HAPI_WC_PORT_MAP_t dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56846_REV_1[];
extern HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1[];
#elif (PTIN_BOARD == PTIN_BOARD_CXO160G)
extern L7_uint32  dapiBroadBaseWCSlotPortmodeMap_CARD_BROAD_64_TENGIG_56640_REV_1[];
extern HAPI_WC_PORT_MAP_t dapiBroadBaseWCPortMap_CARD_BROAD_64_TENGIG_56640_REV_1[];
extern HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[];
#endif

/* TC16SXG */
extern HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_32_TENGIG_1_GIG_56370_REV_1[];

/* OLT1T0F */
extern HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1[];
extern HAPI_CARD_SLOT_MAP_t dapiBroadBaseCardSlotMap_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1_HWV3[];

extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1[];        /* PTin added: new switch 56689 (Valkyrie2) */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[];      /* PTin added: new switch 5664x (Triumph3) SF */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_4_TENGIG_4_40GIG_56643_REV_1[];       /* PTin added: new switch 5664x (Triumph3) GPON */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1[];        /* PTin added: new switch 5664x (Triumph3) */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1[];   /* PTin added: new switch 56340 (Helix4) */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1[];   /* PTin added: new switch 56450 (Katana2) - OLT1T0F */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_8_TENGIG_1_GS_56450_REV_1[];          /* PTin added: new switch 56450 (Katana2) - TT04SGX */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_16_GIG_4_TENGIG_1_GS_56450_REV_1[];   /* PTin added: new switch 56450 (Katana2) - TG16GF */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_40_TENGIG_56843_REV_1[];              /* PTin added: new switch 56843 (Trident) */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_64_TENGIG_56846_REV_1[];              /* PTin added: new switch 56846 (Trident-plus) */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_32_TENGIG_1_GIG_56370_REV_1[];        /* PTin added: new switch 56370 (Trident3-X3) */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_20_TENGIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_TENGIG_4_GIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1[];

extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56680_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_4_10G_3_40G_1_GIG_56640_REV_1[];                 /* PTin added: new switch 5664x (Triumph3) SF */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_4_TENGIG_4_40GIG_56643_REV_1[];                  /* PTin added: new switch 5664x (Triumph3) GPON */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56643_REV_1[];                   /* PTin added: new switch 5664x (Triumph3) */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1[];              /* PTin added: new switch 56340 (Helix4) */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1[];              /* PTin added: new switch 56450 (Katana2) */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_TENGIG_4_GIG_56820_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_8_HIGIG_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1[];
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_40_TENGIG_56843_REV_1[];                         /* PTin added: new switch 56843 (Trident) */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_64_TENGIG_56846_REV_1[];                         /* PTin added: new switch 56846 (Trident-plus) */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_32_TENGIG_1_GIG_56370_REV_1[];                   /* PTin added: new switch 56370 (Trident3-X3) */

extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1;       /* PTin added: new switch 56689 (Valkyrie2) */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1;     /* PTin added: new switch 5664x (Triumph3) SF */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_4_TENGIG_4_40GIG_56643_REV_1;      /* PTin added: new switch 5664x (Triumph3) GPON */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1;       /* PTin added: new switch 5664x (Triumph3) */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56340_REV_1;  /* PTin added: new switch 56340 (Helix4) */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_12_GIG_4_TENGIG_1_GS_56450_REV_1;  /* PTin added: new switch 56340 (Katana2) - OLT1T0F */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_16_GIG_4_TENGIG_1_GS_56450_REV_1;  /* PTin added: new switch 56340 (Katana2) - TG16GF && AG16GA*/
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_8_TENGIG_1_GS_56450_REV_1;         /* PTin added: new switch 56340 (Katana2) - TT04SXG */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_40_TENGIG_56843_REV_1;             /* PTin added: new switch 56843 (Trident) */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_64_TENGIG_56846_REV_1;             /* PTin added: new switch 56846 (Trident-plus) */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_32_TENGIG_1_GIG_56370_REV_1;       /* PTin added: new switch 56370 (Trident3-X3) */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_20_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_40_TENGIG_REV_1;                        /* PTin added: new switch 56843 (Trident) */
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_64_TENGIG_REV_1;                        /* PTin added: new switch 56846 (Trident-plus) */
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_32_TENGIG_1_GIG_REV_1;                  /* PTin added: new switch 56370 (Trident3-X3) */
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_20_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_8_HIGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_GIG_2_TENGIG_REV_2;
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
