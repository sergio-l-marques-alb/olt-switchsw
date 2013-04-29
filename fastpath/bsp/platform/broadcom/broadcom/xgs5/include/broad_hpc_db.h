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


extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_GIG_4_TENGIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1[];   /* PTin added: new platform BCM56643 */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_48_GIG_4_TENGIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_20_TENGIG_REV_1[];
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_24_TENGIG_4_GIG_REV_1[];

extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_48_GIG_4_TENGIG_56643_REV_1[];                   /* PTin added: new platform BCM56643 */

extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_GIG_4_TENGIG_SINGLE_CHIP_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_48_GIG_4_TENGIG_56643_REV_1;       /* PTin added: new platform BCM56643 */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_20_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_40_TENGIG_REV_1;                        /* PTin added: new switch BCM56843 (trident) */
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_64_TENGIG_REV_1;                        /* PTin added: new platform BCM56846 OLT1T3 (trident) */
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_20_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_TENGIG_4_GIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_24_GIG_2_TENGIG_REV_2;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_REV_1;
extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1;

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
