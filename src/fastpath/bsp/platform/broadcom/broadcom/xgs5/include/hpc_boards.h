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
    /* PTin added: new switch 5664x (Triumph3) SF */
    /* 56640 board */
    {
        BCM_SYS_BOARD_56640,
        /* *soc_brd_info */
    1, 
    { 
        BCM56640_DEVICE_ID,
    },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_4_10G_3_40G_1_GIG_56640_REV_1_ID,
        __BROADCOM_56640_ID,
        NO_CONNECTION_LIST
    },
    /* PTin added: new switch 5664x (Triumph3) */
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
        UNIT_BROAD_48_GIG_4_TENGIG_4_40GIG_56643_REV_1_ID,
        __BROADCOM_56640_ID,
        NO_CONNECTION_LIST
    },
    /* PTin added: new switch 56340 (Helix4) */
    /* 56340 board */
    {
        BCM_SYS_BOARD_56340,
        /* *soc_brd_info */
    1, 
    { 
        BCM56340_DEVICE_ID,
    },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_48_GIG_4_TENGIG_56340_REV_1_ID,
        __BROADCOM_56340_ID,
        NO_CONNECTION_LIST
    },
    /* PTin added: new switch 56450 (Katana2) */
    /* 56450 board */
    {
        BCM_SYS_BOARD_56450,
        /* *soc_brd_info */
    1, 
    { 
        BCM56450_DEVICE_ID,
    },
        bcm_sys_id_defl,
        bcm_sys_sa_init_defl,
        bcm_sys_pre_stack_defl,
        UNIT_BROAD_48_GIG_4_TENGIG_56450_REV_1_ID,
        __BROADCOM_56450_ID,
        NO_CONNECTION_LIST
    },
    /* PTin added: new switch 56843 (Trident) */
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
    /* PTin added: new switch 56843 (Trident) */
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
    /* PTin added: new switch 56846 (Trident-plus) */
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
/* }; */

#endif /* HPC_BOARDS_H */
