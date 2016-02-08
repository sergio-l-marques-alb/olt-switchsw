/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename trapstr_captive_portal.c
*
* @purpose Trap Manager Captive Portal String File
*
* @component trapmgr
*
* @comments none
*
* @created 08/20/2007
*
* @author wjacobs, rjindal
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "trapstr_captive_portal.h"

L7_char8 captivePortalClientConnect_str[] = "CP Client Connected: MAC: %02x:%02x:%02x:%02x:%02x:%02x IP: %s SwMAC: %02x:%02x:%02x:%02x:%02x:%02x CPID: %d Interface: %d";
L7_char8 captivePortalClientDisconnect_str[] = "CP Client Disconnected: MAC: %02x:%02x:%02x:%02x:%02x:%02x IP: %s SwMAC: %02x:%02x:%02x:%02x:%02x:%02x CPID: %d Interface: %d";
L7_char8 captivePortalClientAuthFailure_str[] = "CP Client Auth Failure: MAC: %02x:%02x:%02x:%02x:%02x:%02x IP: %s SwMAC: %02x:%02x:%02x:%02x:%02x:%02x CPID: %d Interface: %d User: %s";
L7_char8 captivePortalConnectionDatabaseFull_str[]= "CP Client Authentication Database Full";
L7_char8 captivePortalAuthFailureLogWrapped_str[] = "CP Client Authentication Failure Log Wrapped";
L7_char8 captivePortalActivityLoggedWrapped_str[] = "CP Client Activity Log Wrapped";

