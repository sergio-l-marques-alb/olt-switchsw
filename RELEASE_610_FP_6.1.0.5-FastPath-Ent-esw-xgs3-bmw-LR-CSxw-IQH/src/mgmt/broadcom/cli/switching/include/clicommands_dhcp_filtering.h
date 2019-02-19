/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_dhcp_filtering.h
*
* @purpose Dhcp Filtering commands header file
*
* @component Dhcp Filtering
*
* @comments none
*
* @create  03/15/2005
*
* @author  skoundinya
*
* @end
*
*********************************************************************/

#ifndef CLIDHCPFILTERING_H
#define CLIDHCPFILTERING_H

/**************************************
**
** BEGIN DHCP FILTERING FUNCTIONS HELP
**
***************************************/

#define CLI_MAX_SCROLL_LINES_DHCP_FILTER            (CLI_MAX_SCROLL_LINES-9)

/*************************************
**
** END DHCP FILTERING FUNCTIONS HELP
**
**************************************/

/*****************************************
**
** BEGIN DHCP FILTERING FUNCTIONS SYNTAX
**
*******************************************/

/*****************************************
**
** END DHCP FILTERING FUNCTIONS SYNTAX
**
******************************************/

/*********************************************************
**
** BEGIN DHCP FILTERING FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/
void buildTreeGlobalDhcpFiltering(EwsCliCommandP depth1);
void buildTreeIfIpDhcpFilteringTrust(EwsCliCommandP depth1);
void buildTreeShowIpDhcpFiltering(EwsCliCommandP depth1);

/*********************************************************
**
** END CARD FUNCTIONS TREE COMMAND STRUCTURE
**
**********************************************************/

/********************************************************************
**
** BEGIN DHCP FILTERING FUNCTIONS COMMANDS STRUCTURE - SHOW & CONFIG
**
*********************************************************************/

/* commands defined in cli_show_dhcp_filtering.c */
/*const char  *commandShowIpDhcpFiltering(EwsContext ewsContext, uintf argc, const char **argv, uintf index);*/
const char *commandShowIpDhcpFiltering(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/* commands defined in cli_config_dhcp_filtering.c */
const L7_char8 *commandIpDhcpFiltering(EwsContext ewsContext, L7_uint32 argc,const L7_char8 * * argv, L7_uint32 index);
const L7_char8 *commandIpDhcpFilteringTrust(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index);

/********************************************************************
**
** END DHCP FILTERING FUNCTIONS COMMANDS STRUCTURE - SHOW & CONFIG
**
*********************************************************************/

/*************************************************
**
** BEGIN DHCP FILTERING COMMANDS HELPER ROUTINES
**
**************************************************/

/* routines defined in cliutil_card.c */

/**************************************************
**
** END DHCP FILTERING COMMANDS HELPER ROUTINES
**
***************************************************/

#endif /* CLIDHCPFILTERING_H*/
