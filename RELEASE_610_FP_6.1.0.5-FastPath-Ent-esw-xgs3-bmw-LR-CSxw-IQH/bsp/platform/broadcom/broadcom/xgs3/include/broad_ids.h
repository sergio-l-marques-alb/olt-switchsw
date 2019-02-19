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

#define UNIT_BROAD_24_GIG_4_TENGIG_56304_REV_1_ID 0x63040001 /* One 56304  */
#define UNIT_BROAD_48_GIG_4_TENGIG_56304_REV_2_ID 0x63040002 /* Two 56304s or 56404s */
#define UNIT_BROAD_24_GIG_4_TENGIG_56314_REV_1_ID 0x63140001 /* One 56314  */
#define UNIT_BROAD_48_GIG_4_TENGIG_56314_REV_2_ID 0x63140002 /* Two 56314s */
#define UNIT_BROAD_24_GIG_4_TENGIG_56504_REV_1_ID 0x65040001 /* One 56504  */
#define UNIT_BROAD_48_GIG_4_TENGIG_56504_REV_1_ID 0x75040001
#define UNIT_BROAD_24_GIG_4_TENGIG_56514_REV_1_ID 0x65140001 /* One 56514  */
#define UNIT_BROAD_48_GIG_4_TENGIG_56514_REV_1_ID 0x25140002 /* two 56514s */
#define UNIT_BROAD_20_TENGIG_56800_REV_1_ID       0x68000001 /* One 56800  */
#define UNIT_BROAD_16_GIG_4_TENGIG_56580_REV_1_ID 0xB5800001 /* One 56580  */

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
#define CARD_BROAD_24_GIG_4_TENGIG_56304_REV_1_ID  0x56304001
#define CARD_BROAD_24_GIG_4_TENGIG_56314_REV_1_ID  0x56314001
#define CARD_BROAD_24_GIG_4_TENGIG_56504_REV_1_ID  0x56504001
#define CARD_BROAD_48_GIG_4_TENGIG_56504_REV_1_ID  0x56504401
#define CARD_BROAD_24_GIG_4_TENGIG_56514_REV_1_ID  0x56514001
#define CARD_BROAD_48_GIG_4_TENGIG_56514_REV_1_ID  0x56514101
#define CARD_BROAD_20_TENGIG_56800_REV_1_ID        0x56800001
#define CARD_BROAD_16_GIG_4_TENGIG_56580_REV_1_ID  0x56580001

/* Logical CPU cards */
#define L7_LOGICAL_CARD_CPU_24_GIG_4_TENGIG_REV_1_ID 0xFF010008
#define L7_LOGICAL_CARD_CPU_48_GIG_4_TENGIG_REV_1_ID 0xFF010009
#define L7_LOGICAL_CARD_CPU_20_TENGIG_REV_1_ID       0xFF01000c
#define L7_LOGICAL_CARD_CPU_16_GIG_4_TENGIG_REV_1_ID 0xFF01000d

#endif
