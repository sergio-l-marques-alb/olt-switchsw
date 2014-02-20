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

    /* 56624 board */
    {
        BCM_SYS_BOARD_56624,
        /* *soc_brd_info */
	1, 
	{ 
            BCM56624_DEVICE_ID,
	},
        bcm_sys_id_defl,
        bcm_sys_sa_init_56624,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1_ID,
	__BROADCOM_56624_ID,
        NO_CONNECTION_LIST
    },
    /* 56680 board */
    {
        BCM_SYS_BOARD_56680,
        /* *soc_brd_info */
	1, 
	{ 
            BCM56680_DEVICE_ID,
	},
        bcm_sys_id_defl,
        bcm_sys_sa_init_56680,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_24_GIG_4_TENGIG_56680_REV_1_ID,
	__BROADCOM_56680_ID,
        NO_CONNECTION_LIST
    },
    /* PTin added: new switch 56689 (Valkyrie2) */
    /* 56689 board */
    {
        BCM_SYS_BOARD_56689,
        /* *soc_brd_info */
    1, 
    { 
        BCM56689_DEVICE_ID,
    },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID,
        __BROADCOM_56685_ID,
        NO_CONNECTION_LIST
    },
    /* PTin added: new switch 56643 (Triumph3) */
    /* 56643 board */
    {
        BCM_SYS_BOARD_56643,
        /* *soc_brd_info */
    1, 
    { 
        BCM56643_DEVICE_ID,
    },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_48_GIG_4_TENGIG_56643_REV_1_ID,
        __BROADCOM_56643_ID,
        NO_CONNECTION_LIST
    },
    /* 56843 board */
    {
        BCM_SYS_BOARD_56843,
        /* *soc_brd_info */
    1, 
    { 
        BCM56843_DEVICE_ID,
    },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_40_TENGIG_56843_REV_1_ID,
        __BROADCOM_56843_ID,
        NO_CONNECTION_LIST
    },
#if (PTIN_BOARD == PTIN_BOARD_CXP360G)
    /* 56844 board */
    {
        BCM_SYS_BOARD_56844,
        /* *soc_brd_info */
    1, 
    { 
        BCM56844_DEVICE_ID,
    },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_40_TENGIG_56843_REV_1_ID,
        __BROADCOM_56843_ID,
        NO_CONNECTION_LIST
    },
#elif (PTIN_BOARD == PTIN_BOARD_CXO640G)
    /* 56846 board */
    {
        BCM_SYS_BOARD_56846,	/* XXXX */
        /* *soc_brd_info */
    1, 
    { 
        BCM56846_DEVICE_ID, /* XXXX */
    },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_64_TENGIG_56846_REV_1_ID,
        __BROADCOM_56843_ID,
        NO_CONNECTION_LIST
    },
#endif
    /* PTin end */
    /* 56820 board */
    {
        BCM_SYS_BOARD_56820,
        /* *soc_brd_info */
        1,
        {
            BCM56820_DEVICE_ID,
        },
        bcm_sys_id_defl,
        bcm_sys_sa_init_56820,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_24_TENGIG_4_GIG_56820_REV_1_ID,
	__BROADCOM_56820_ID,
        NO_CONNECTION_LIST
    },
    /* 56634 board */
    {
        BCM_SYS_BOARD_56634,
        /* *soc_brd_info */
	1, 
	{ 
            BCM56634_DEVICE_ID,
	},
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1_ID,
	__BROADCOM_56634_ID,
        NO_CONNECTION_LIST
    },
    /* 56524 board */
    {
        BCM_SYS_BOARD_56524,
        /* *soc_brd_info */
	1, 
	{ 
            BCM56524_DEVICE_ID,
	},
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
	UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1_ID,
	__BROADCOM_56524_ID,
        NO_CONNECTION_LIST
    },
    /* 56636 SVK */
    {
        BCM_SYS_BOARD_56636,
        /* *soc_brd_info */
        1,
        {
            BCM56636_DEVICE_ID,
        },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1_ID,
        __BROADCOM_56636_ID,
        NO_CONNECTION_LIST
    },
    /* 56538 SVK - identical feature set as 56634 device */
        {
        BCM_SYS_BOARD_56538,
        /* *soc_brd_info */
        1,
        {
            BCM56538_DEVICE_ID,
        },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_48_GIG_4_TENGIG_56538_REV_1_ID,
        __BROADCOM_56634_ID,  /* Treat this as 56634 device */
        NO_CONNECTION_LIST
    },
    /* 56334 board */
    {
      BCM_SYS_BOARD_56334,
      /* *soc_brd_info */
      1, 
      { 
        BCM56334_DEVICE_ID,
      },
      bcm_sys_id_defl,
      bcm_sys_sa_init_defl,
      bcm_sys_pre_stack_defl,
      UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1_ID,
      __BROADCOM_56334_ID,
      NO_CONNECTION_LIST
    },
/* }; */

#endif /* HPC_BOARDS_H */
