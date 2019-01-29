/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_card.h
*
* @purpose Slot commands header file
*
* @component User Interface
*
* @comments
*
* @create  07/31/2003
*
* @author  rjindal
* @end
*
*********************************************************************/

#ifndef CLICOMMANDSCARD_H
#define CLICOMMANDSCARD_H

/**********************************************
**
** BEGIN CARD FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************/

/* commands defined in clicommands_card.c */
void buildTreeUserExecShowSlot(EwsCliCommandP depth1);
void buildTreeUserExecShowSupported(EwsCliCommandP depth1);
void buildTreeGlobalSlotCard(EwsCliCommandP depth1);
void buildTreeGlobalSlotSet(EwsCliCommandP depth1);
EwsCliCommandP buildTreeSlotHelper(EwsCliCommandP depth, EwsCliNodeNoFormStatus noFormStatus);
EwsCliCommandP buildTreeSlotAllHelper(EwsCliCommandP depth, EwsCliNodeNoFormStatus noFormStatus);

/**********************************************
**
** END CARD FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************/

/**********************************************************
**
** BEGIN CARD FUNCTIONS COMMANDS STRUCTURE - SHOW & CONFIG
**
**********************************************************/

/* commands defined in cli_show_card.c */
const L7_char8 *commandShowSlot(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSupportedCardType(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandShowSupportedCard(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* commands defined in cli_config_card.c */
const L7_char8 *commandSlot(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSlotDisable(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandSetSlotPower(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/**********************************************************
**
** END CARD FUNCTIONS COMMANDS STRUCTURE - SHOW & CONFIG
**
**********************************************************/

/**************************************
**
** BEGIN CARD COMMANDS HELPER ROUTINES
**
**************************************/

/* routines defined in cliutil_card.c */
L7_char8 *cliSyntaxSlot(void);
L7_char8 *cliSyntaxAllSlot(void);
L7_char8 *cliDisplaySlotHelp(L7_uint32 unit, L7_uint32 slot);
L7_RC_t  cliValidUnitSlotCheck(const L7_char8 * buf, L7_uint32 * unit, L7_uint32 * slot, L7_BOOL * allUS);
L7_RC_t  cliValidSpecificUnitSlotCheck(const L7_char8 * buf, L7_uint32 * unit, L7_uint32 * slot);
void     cliUnitSlotDisplay(EwsContext ewsContext, L7_uint32 unit, L7_uint32 slot);
L7_BOOL  cliIsStackingSupported(void);
L7_BOOL  cliIsUnitPresent(L7_uint32 unit);

/**************************************
**
** END CARD COMMANDS HELPER ROUTINES
**
**************************************/

#endif /* CLICOMMANDSCARD_H */
