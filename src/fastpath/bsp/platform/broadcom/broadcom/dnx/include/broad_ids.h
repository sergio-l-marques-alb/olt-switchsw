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

#define UNIT_BROAD_12_ETH_4_BCK_88360_REV_1_ID  0x83600001 /* PTin added: new switch ARAD */
#define UNIT_BROAD_8_ETH_3_BCK_88360_REV_1_ID   0x83600002 /* PTin added: new switch ARAD */

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
#define CARD_BROAD_12_ETH_4_BCK_88360_REV_1_ID   0x88360001   /* PTin added: new switch ARAD */
#define CARD_BROAD_8_ETH_3_BCK_88360_REV_1_ID    0x88360002   /* PTin added: new switch ARAD */

/* Logical CPU cards */
#define L7_LOGICAL_CARD_CPU_GENERIC_REV_1_ID                 0xFF010008

#endif
