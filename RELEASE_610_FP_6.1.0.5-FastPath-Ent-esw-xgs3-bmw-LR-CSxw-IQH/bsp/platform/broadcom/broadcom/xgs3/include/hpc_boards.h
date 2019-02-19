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

    /* 56304 board */
    {
        BCM_SYS_BOARD_56304,
        /* *soc_brd_info */
	1, 
	{
            BCM56304_DEVICE_ID,
	},
        bcm_sys_id_helix,
        bcm_sys_sa_init_56304,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_24_GIG_4_TENGIG_56304_REV_1_ID,
        __BROADCOM_56304_ID,
        NO_CONNECTION_LIST

    },
    {
        BCM_SYS_BOARD_56314,
        /* *soc_brd_info */
	1, 
	{
            BCM56314_DEVICE_ID,
	},
        bcm_sys_id_defl,
        bcm_sys_sa_init_56304,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_24_GIG_4_TENGIG_56314_REV_1_ID,
	__BROADCOM_56314_ID,
        NO_CONNECTION_LIST
    },
    /* 56504 board */
    {
        BCM_SYS_BOARD_56504,
        /* *soc_brd_info */
	1, 
	{ 
            BCM56504_DEVICE_ID,
	},
        bcm_sys_id_defl,
        bcm_sys_sa_init_56504,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_24_GIG_4_TENGIG_56504_REV_1_ID,
	__BROADCOM_56504_ID,
        NO_CONNECTION_LIST
    },
    /* dual 56504 board */
    {
        BCM_SYS_BOARD_56504_P48,
        /* *soc_brd_info */
	2, 
	{
            BCM56504_DEVICE_ID,
            BCM56504_DEVICE_ID,
	},
        bcm_sys_id_defl,
        bcm_sys_sa_init_56504,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_48_GIG_4_TENGIG_56504_REV_1_ID,
	__BROADCOM_56504_ID,
        4, /* Number of elements in the following connection list */
      {
        {0,26,1,27,L7_12G_INTERCONNECT },
        {0,27,1,26,L7_12G_INTERCONNECT },
        {1,26,0,27,L7_12G_INTERCONNECT },
        {1,27,0,26,L7_12G_INTERCONNECT }
      }
    },
    /* 56514 board */
    {
        BCM_SYS_BOARD_56514,
        /* *soc_brd_info */
	1, 
	{ 
            BCM56514_DEVICE_ID,
	},
        bcm_sys_id_defl,
        bcm_sys_sa_init_56504,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_24_GIG_4_TENGIG_56514_REV_1_ID,
	__BROADCOM_56514_ID,
        NO_CONNECTION_LIST
    },
    /* dual 56514 board */
    {
        BCM_SYS_BOARD_56514_P48,
        /* *soc_brd_info */
        2,
        {
            BCM56514_DEVICE_ID,
            BCM56514_DEVICE_ID,
        },
        bcm_sys_id_defl,
        bcm_sys_sa_init_56504,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_48_GIG_4_TENGIG_56514_REV_1_ID,
        __BROADCOM_56514_ID,
        4, /* Number of elements in the following connection list */
      {
        {0,26,1,27,L7_12G_INTERCONNECT },
        {0,27,1,26,L7_12G_INTERCONNECT },
        {1,26,0,27,L7_12G_INTERCONNECT },
        {1,27,0,26,L7_12G_INTERCONNECT }
      }
    },
    /* 56800 board */
    {
        BCM_SYS_BOARD_56800,
        /* *soc_brd_info */
        1,
        {
            BCM56800_DEVICE_ID,
        },
        bcm_sys_id_defl,
        bcm_sys_sa_init_56800,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_20_TENGIG_56800_REV_1_ID,
	__BROADCOM_56800_ID,
        NO_CONNECTION_LIST
    },
    /* 56580 board */
    {
        BCM_SYS_BOARD_56580,
        /* *soc_brd_info */
        1,
        {
            BCM56580_DEVICE_ID,
        },
        bcm_sys_id_defl,
        bcm_sys_sa_init_56800,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_16_GIG_4_TENGIG_56580_REV_1_ID,
        __BROADCOM_56800_ID,
        NO_CONNECTION_LIST
    },

/* }; */

#endif /* HPC_BOARDS_H */
