/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename broad_ids.h
*
* @purpose Define unique board ID's for the Broadcom reference platforms
*
* @component hpc
*
* @comments none
*
* @create 06/03/2008
*
* @author bradyr
* @end
*
**********************************************************************/
#ifndef INCLUDE_BROAD_IDS
#define INCLUDE_BROAD_IDS


/*******************************************************************************
*
* @purpose Maintain all unit types that are supported as well as their
*          individual unit type ID
*
* @end
*
*******************************************************************************/

#define UNIT_BROAD_48_GIG_4_TENGIG_56624_REV_1_ID 0xB6240001 
#define UNIT_BROAD_24_GIG_4_TENGIG_56680_REV_1_ID 0xB6800001
#define UNIT_BROAD_24_TENGIG_4_GIG_56820_REV_1_ID 0x68200001 /* One 56820  */
#define UNIT_BROAD_24_GIG_4_TENGIG_56524_REV_1_ID 0xB5240001 /* One 56524  */
#define UNIT_BROAD_48_GIG_4_TENGIG_56634_REV_1_ID 0xB6340001
#define UNIT_BROAD_25_GIG_6_TENGIG_56636_REV_1_ID 0xB6360001
#define UNIT_BROAD_24_GIG_4_TENGIG_56334_REV_1_ID 0xB3340001 /* One 56334  */
#define UNIT_BROAD_48_GIG_4_TENGIG_56538_REV_1_ID 0xB5380001
#define UNIT_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID 0xB6890001 /* PTin added: new switch 56689 (Valkyrie2) */
#define UNIT_BROAD_40_TENGIG_56843_REV_1_ID       0xB8430001 /* PTin added: new switch 56843 (Trident) */
#define UNIT_BROAD_64_TENGIG_56846_REV_1_ID       0xB8460001 /* PTin added: new switch 56846 (Trident-plus) */

/*******************************************************************************
*
* @enums   CARDS_SUPPORTED
*
* @purpose Maintain all cards that are supported as well as their
*          individual card ID
*
* @end
*
*******************************************************************************/

/* Broadcom Reference platforms */
#define CARD_BROAD_48_GIG_4_TENGIG_56624_REV_1_ID  0x56624101
#define CARD_BROAD_24_GIG_4_TENGIG_56680_REV_1_ID  0x56680101
#define CARD_BROAD_24_TENGIG_4_GIG_56820_REV_1_ID  0x56820001
#define CARD_BROAD_24_GIG_4_TENGIG_56524_REV_1_ID  0x56524001
#define CARD_BROAD_48_GIG_4_TENGIG_56634_REV_1_ID  0x56634101
#define CARD_BROAD_25_GIG_6_TENGIG_56636_REV_1_ID  0x56636001
#define CARD_BROAD_24_GIG_4_TENGIG_56334_REV_1_ID  0x56334001
#define CARD_BROAD_48_GIG_4_TENGIG_56538_REV_1_ID  0x56538101
#define CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID  0x56689001   /* PTin added: new switch 56689 (Valkyrie2) */
#define CARD_BROAD_40_TENGIG_56843_REV_1_ID        0x56843001   /* PTin added: new switch 56843 (Trident) */
#define CARD_BROAD_64_TENGIG_56846_REV_1_ID        0x56846001   /* PTin added: new switch 56846 (Trident-plus) */

/* Logical CPU cards */
#define L7_LOGICAL_CARD_CPU_24_GIG_4_TENGIG_REV_1_ID 0xFF010008
#define L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_REV_1_ID 0xFF010009
#define L7_LOGICAL_CARD_CPU_20_TENGIG_REV_1_ID       0xFF01000c
#define L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_SINGLE_CHIP_REV_1_ID 0xFF01000F
#define L7_LOGICAL_CARD_SCORPION_24_TENGIG_4_GIG_REV_1_ID 0xFF010011
#define L7_LOGICAL_CARD_CPU_40_TENGIG_REV_1_ID       0xFF010012 /* PTin added: new switch 56843 (Trident) */
#define L7_LOGICAL_CARD_CPU_64_TENGIG_REV_1_ID       0xFF010013 /* PTin added: new switch 56846 (Trident-plus) */

#endif
