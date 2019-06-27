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

#define UNIT_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID           0xB6890001 /* PTin added: new switch 56689 (Valkyrie2) */
#define UNIT_BROAD_4_10G_3_40G_1_GIG_56640_REV_1_ID         0xB64014E4 /* PTin added: new switch 5664x (Triumph3) SF */
#define UNIT_BROAD_48_GIG_4_TENGIG_4_40GIG_56643_REV_1_ID   0xB64314E4 /* PTin added: new switch 5664x (Triumph3) */
#define UNIT_BROAD_48_GIG_4_TENGIG_56340_REV_1_ID           0xB3400001 /* PTin added: new switch 56340 (Helix4) */
#define UNIT_BROAD_48_GIG_4_TENGIG_56450_REV_1_ID           0xB4500001 /* PTin added: new switch 56450 (Katana2) */
#define UNIT_BROAD_40_TENGIG_56843_REV_1_ID                 0xB8430001 /* PTin added: new switch 56843 (Trident) */
#define UNIT_BROAD_64_TENGIG_56846_REV_1_ID                 0xB8460001 /* PTin added: new switch 56846 (Trident-plus) */

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
#define CARD_BROAD_24_GIG_4_TENGIG_56689_REV_1_ID         0x56689001   /* PTin added: new switch 56689 (Valkyrie2) */
#define CARD_BROAD_4_10G_3_40G_1_GIG_56640_REV_1_ID       0x56640001   /* PTin added: new switch 5664x (Triumph3) SF */
#define CARD_BROAD_48_GIG_4_TENGIG_4_40GIG_56643_REV_1_ID 0x56643001   /* PTin added: new switch 5664x (Triumph3) */
#define CARD_BROAD_48_GIG_4_TENGIG_56340_REV_1_ID         0x56340001   /* PTin added: new switch 56340 (Helix4) */
#define CARD_BROAD_48_GIG_4_TENGIG_56450_REV_1_ID         0x56450001   /* PTin added: new switch 56450 (Katana2) */
#define CARD_BROAD_40_TENGIG_56843_REV_1_ID               0x56843001   /* PTin added: new switch 56843 (Trident) */
#define CARD_BROAD_64_TENGIG_56846_REV_1_ID               0x56846001   /* PTin added: new switch 56846 (Trident-plus) */
#define CARD_BROAD_48_GIG_4_TENGIG_56646_REV_1_ID         0x56646001   /* PTin added: new switch 56646 (?) */

/* Logical CPU cards */
#define L7_LOGICAL_CARD_CPU_DEFAULT_ID                              0xFF010000  /* PTin added: Default CPU at interface 0 */
#define L7_LOGICAL_CARD_CPU_24_GIG_4_TENGIG_REV_1_ID                0xFF010008
#define L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_4_40GIG_REV_1_ID        0xFF010009  /* PTin modified: new switch 5664x (Triumph3) */
#define L7_LOGICAL_CARD_CPU_64_TENGIG_REV_1_ID                      0xFF010013  /* PTin added: new switch 56846 (Trident-plus) */

#endif
