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

extern int bcm_sys_sa_init_88650(const bcm_sys_board_t *brd, int base);

extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_12_ETH_4_BCK_88650_REV_1[];           /* PTin added: new switch ARAD */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_8_ETH_3_BCK_88650_REV_1[];            /* PTin added: new switch ARAD */
extern SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_BROAD_1_ETH_3_BCK_88650_REV_1[];            /* PTin added: new switch ARAD */

extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_12_ETH_4_BCK_88650_REV_1[];                          /* PTin added: new switch ARAD */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_8_ETH_3_BCK_88650_REV_1[];                           /* PTin added: new switch ARAD */
extern HPC_BROAD_UNIT_DATA_t hpc_data_UNIT_BROAD_1_ETH_3_BCK_88650_REV_1[];                           /* PTin added: new switch ARAD */

extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_12_ETH_4_BCK_88650_REV_1;              /* PTin added: new switch ARAD */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_8_ETH_3_BCK_88650_REV_1;               /* PTin added: new switch ARAD */
extern DAPI_CARD_ENTRY_t dapiBroadPhysicalCardEntry_CARD_BROAD_1_ETH_3_BCK_88650_REV_1;               /* PTin added: new switch ARAD */

extern DAPI_CARD_ENTRY_t dapiBroadCpuCardEntry_CARD_BROAD_GENERIC_REV_1;                              /* PTin added: new switch ARAD */

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
