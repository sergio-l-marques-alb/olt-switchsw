/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  hpc_boards.h
*
* @purpose   Initialize storage for use in hpc board database.
*
* @component hpc
*
* @create    05/23/2008
*
* @author    bradyr 
* @end
*
*********************************************************************/

#ifndef HPC_BOARDS_H
#define HPC_BOARDS_H

#include "broad_ids.h"
/*******************************************************************************
*
* @Data    bcm_sys_boards
*
* @purpose An array of descriptors with a descriptor entry for each board
*          supported.
*
* @end
*
*******************************************************************************/
/* bcm_sys_board_t bcm_sys_boards[] = { */

    /* PTin added: new switch ARAD */
    /* 88360 board */
    {
        BCM_SYS_BOARD_88650,
        /* *soc_brd_info */
    1, 
    { 
        BCM88360_DEVICE_ID,
    },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
#if (PTIN_BOARD == PTIN_BOARD_TA12XG)
        UNIT_BROAD_12_ETH_4_BCK_88360_REV_1_ID,
#else
        UNIT_BROAD_8_ETH_3_BCK_88360_REV_1_ID,
#endif
        __BROADCOM_88650_ID,
        NO_CONNECTION_LIST
    },

/* }; */

#endif /* HPC_BOARDS_H */
