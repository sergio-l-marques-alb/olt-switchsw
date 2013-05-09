/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src/mgmt/cli/base/clicommands_poe.h
*
* @purpose header for POE commands in clicommands_poe.h
*
* @component user interface
*
* @comments none
*
* @create  18/04/2007
*
* @author  drajendra
* @end
*
***********************************************************************/

#ifndef CLICOMMANDS_POE_H
#define CLICOMMANDS_POE_H

#include "clicommands_card.h"

/*****************************************************************/

/* Defining constants */
/* begin */

#define  CLIPOESHOWCONF_HELP                "Display PoE Port Configuration parameters."
#define CLISYNTAX_CONFIGPOE_LIMIT           "Use 'poe limit {dot3af | none | user-defined [<%u-%u]}'"
#define CLISYNTAX_CONFIGPOE_LIMIT_NO        "Use 'no poe limit [user-defined]'"
#define CLI_POE_LIMIT_RANGE_ERR             "PoE limit must be from %u to %u."
#define CLI_POE_USAGE_THRESHOLD_RANGE_ERR   "PoE usage threshold must be from %u to %u."

#define CLISYNTAX_CONFIGPOE_PRIORITY        "Use 'poe priority <crit/high/low>'"
#define CLISYNTAX_CONFIGPOE_PRIORITY_NO     "Use 'no poe priority'"

#define CLISYNTAX_CONFIGUSAGE_THRESHOLD     "Use 'poe usagethreshold <%u-%u>'"
#define CLISYNTAX_CONFIGUSAGE_THRESHOLD_NO  "Use 'no poe usagethreshold'"

#define CLISYNTAX_CONFIGPOWER_MGMT          "Use 'poe power management <static/dynamic>'"
#define CLISYNTAX_CONFIGPOWER_MGMT_NO       "Use 'no poe power management'"

#define CLISYNTAX_CONFIGDETECTION           "Use 'poe detection <2ptdot3af | \
2ptdot3af+legacy | 4ptdot3af | 4ptdot3af+lefacy | legacy>'"
#define CLISYNTAX_CONFIGDETECTION_NO        "Use 'no poe detection'"

#define CLISYNTAX_CONFIGPOE_HIGHPWR         "Use 'poe high-power'. "
#define CLISYNTAX_CONFIGPOE_PORTMAP         "Use 'poe port-map'."
#define CLISYNTAX_CONFIGPOE_PORTMAP_NO      "Use 'no poe port-map'."
#define CLISYNTAX_CONFIGPOE_PWRSRC          "use 'poe power source <toal power>'."
#define CLISYNTAX_CONFIGPOE_PWRSRC_NO       "use 'no poe power source'." 
#define CLI_PWRSRC_BAD                      "Could not set power source for PoE sub system."
#define CLI_PWRSRC_BADVALUE                 "Total power must be valid integer"
#define CLISYNTAX_CONFIGPOE_HIGHPWR_NO      "Use 'no poe high-power'. "
#define CLISYNTAX_CONFIGPOE                 "Use 'poe'. "
#define CLISYNTAX_RESET_AUTO                "Use 'poe reset auto'. "
#define CLISYNTAX_RESET_AUTO_NO             "Use 'no poe reset'. "
#define CLISYNTAX_RESET                     "Use 'poe reset'. "
#define CLISYNTAX_AUTO_POWERUP              "Use 'poe auto-powerup'."
#define CLISYNTAX_AUTO_POWERUP_NO           "Use 'no poe auto-powerup'."
#define CLISYNTAX_CONFIGPOE_NO              "Use 'no poe'. "
#define CLI_POE_HIGHPWR                     "Could not enable PoE high power mode on atleast one PoE port."
#define CLI_POE_HIGHPWR_NO                  "Could not disable PoE high power mode on atleast one PoE port."
#define CLI_POE_PORTMAP                     "Could not enable PoE logical port map on PoE subsystem."
#define CLI_POE_PORTMAP_NO                  "Could not disable PoE logical port map on PoE subsystem."

#define CLI_POE_PORTHIGHPWR                 "Could not enable PoE high power mode on PoE port %s."
#define CLI_POE_PORTHIGHPWR_NO              "Could not disable PoE high power mode on PoE port %s."

#define CLIPOEPRIORITY_ERR                  "Could not set the port PoE priority for atleast one PoE port."

#define CLI_PORT_POE                        "PoE could not be enabled on port %s."
#define CLI_PORT_POE_NO                     "PoE could not be disabled on port %s."
#define CLI_PORT_ERR_AUTO_POWERUP           "PoE auto power-up mode could not be enabled on port %s."
#define CLI_PORT_ERR_AUTO_POWERUP_NO        "PoE auto power-up mode could not be disabled on port %s."
#define CLI_POE_PORTRESET                   "Could not reset PoE port %s."
#define CLI_POERESET                        "Atleast one PoE port could not be reset."
#define CLI_POERESET_AUTO                   "Could not set auto reset mode."
#define CLI_POE                             "PoE could not be enabled on atleast one port."
#define CLI_POE_NO                          "PoE could not be disabled on atleast one port."
#define CLISYNTAX_POE_BAD_PRIO              "Priority must be <crit/high/low>."
#define CLIPOEPORTPRIORITY_ERR              "Could not set the port PoE priority for PoE port %s."
#define CLIPOEPORTLIMIT_ERR                 "Could not set the port PoE limit for PoE port %s."
#define CLIPOEPORTLIMIT_TYPE__ERR           "Could not set the port PoE limit type for PoE port %s."
#define CLIPOELIMIT_ERR                     "Could not set the port PoE limit for atleast one PoE port."
#define CLIPOELIMIT_TYPE_ERR                "Could not set the port PoE limit type for atleast one PoE port."
#define CLIPOE_CLEAR_STATS_ERR              "Could not clear the error statistics for PoE port %s."

#define CLIPOEPORTLIMIT_TYPE_ERR            "\r\nError: Could not set the port PoE limit type."

#define CLISYNTAX_POE_BAD_PORT              "PoE is not supported on the port %s"

#define CLIPOEPORTINVALID_ERR               "\r\nError: The specified port is not valid for use with PoE."
#define CLIPOEUSAGETHRESHOLD_ERR            "Could not set the usage threshold."
#define CLISYNTAX_SHOWPOE_PORT_INFO         "Use 'show poe port info {<%s> | all}'."
#define CLISYNTAX_SHOWPOEINFO               "Use 'show poe'."

#define CLISYNTAX_SHOWPOEPORT_CONFIG        "Use 'show poe port configuration {<%s> | all}"

#define CLISYNTAX_POE_BAD_PWR_MGMT          "Power management must be <static/dynamic>."
#define CLIPOEPWR_MGMT_ERR                  "Could not set the PoE power management mode."

#define CLISYNTAX_POE_BAD_DETECTION         "Poe detection mode must be <2ptdot3af/2ptdot3af+legacy/4ptdot3af/4ptdot3af+lefacy/legacy>."
#define CLIPOEDET_ERR                       "Could not set the PoE port detection mdoe for atleast one PoE port."
#define CLIPOEPORTDET_ERR                   "Could not set the PoE port detection mdoe for PoE port %s."


#define CLIPOE_HIGHPWR                      "high-power"
#define CLIPOE_PORTMAP                      "port-map"
#define CLIPOE_PORTMAP_VAL                  "<value>"
#define CLISYNTAX_PORT_MAP                  "Use 'poe port-map <value>'."
#define CLISYNTAX_PORT_MAP_NO               "Use 'no poe port-map'."
#define CLI_POE_VAL_ERR                     "Invalid value, value must be a valid logical port number."
#define CLISYNTAX_POE_PORTMAP_ERR           "Could not map logical port to physical port %s."
#define CLIPOE_PORTMAP_VAL_HELP             "Configures Logical Port to Physical Port Mapping on the PoE subsystem."
#define CLIPOE_PORTMAP_HELP                 "Configure PoE logical port mapping."
#define CLIPOE_PORTPAIR_HELP                "Configure PoE port pair."
#define CLISYNTAX_BAD_PAIR                  "Enter a valid PoE port power pair."
#define CLISYNTAX_POE_PAIR_ERR              "Could not set the PoE port power alternative for port %s."


#define CLIPOE_DISCONNECT_DC                "dc"
#define CLIPOE_DISCONNECT                   "disconnect"
#define CLIPOE_DISCONNECT_AC                "ac"
#define CLIPOE_DISCONNECT_NONE              "none"
#define CLIPOE_DISCONNEC_HELP               "Configure PoE port disconnect type."
#define CLIPOE_DISCONNECT_DC_HELP           "Configure PoE port disconnect type as DC."
#define CLIPOE_DISCONNECT_AC_HELP           "Configure PoE port disconnect type as AC."
#define CLIPOE_DISCONNECT_NONE_HELP         "Configure PoE port disconnect type as NONE."
#define CLISYNTAX_POE_DISCONNECT            "Use 'poe disconnect ac|dc|none'."
#define CLISYNTAX_POE_DISCONNECT_NO         "Use 'no poe disconnect'."
#define CLISYNTAX_POE_DISCONNECT_ERR        "Could not set the PoE port disconnect type for port %s."
#define CLISYNTAX_BAD_DISCONNECT_TYPE       "Enter a valid PoE power disconnect type."     

#define CLIPOE_HIGHPWR_HELP                 "Configure PoE port high power mode."
#define CLIPOE_POWER_DOT3AF_HELP            "Configure PoE class based Port Power limit."
#define CLIPOE_POWER_NONE_HELP              "Configure PoE Port power limit to none."
#define CLIPOE_POWER_USER_HELP              "Configure PoE user defined Port Power limit."

#define CLIPOE_POWER_HELP                   "Configure PoE power parameters."
#define CLIPOE_DETECTION_HELP               "Configure PoE detection mode."
#define CLIPOE_POWER_MGMT_HELP              "Configure PoE power management mode."
#define CLIPOE_POWER_MGMT_STATIC_HELP       "Static Power Management."
#define CLIPOE_POWER_MGMT_DYN_HELP          "Dynamic Power Management."
#define CLIPOE_2PTDOT3AF_HELP               "2 Point dot3af Detection."
#define CLIPOE_4PTDOT3AF_HELP               "4 Point dot3af Detection."
#define CLIPOE_2PTDOT3AF_LEG_HELP           "2 Point dot3af Detection followed by Legacy Detection."
#define CLIPOE_4PTDOT3AF_LEG_HELP           "4 Point dot3af Detection followed by Legacy Detection."
#define CLIPOE_LEGACY_HELP                  "Legacy Detection."
#define CLIPOERESET_HELP                    "Reset PoE port."
#define CLIPOE_CLEAR_STATS                  "Clears PoE port error statistics."
#define CLIPOE_STATISTICS                   "statistics"                  
#define CLIPOERESET_AUTO_HELP               "Configure PoE Auto Reset mode."


#define CLIPOE_MANAGEMENT                   "management"
#define CLIPOE_DETECTION                    "detection"
#define CLIPOE_DOT3AF                       "dot3af"
#define CLIPOE_POWER_NONE                   "none"
#define CLIPOE_USER_DEF                     "user-defined"
#define CLIPOE_2PTDOT3AF                    "2ptdot3af"
#define CLIPOE_4PTDOT3AF                    "4ptdot3af"
#define CLIPOE_2PTDOT3AF_LEG                "2ptdot3af+legacy"
#define CLIPOE_4PTDOT3AF_LEG                "4ptdot3af+legacy"
#define CLIPOE_LEGACY                       "legacy"
#define CLIPOE_AUTO_POWER                   "auto-powerup"
#define CLIPOE_AUTO_POWER_HELP              "Configure PoE auto power up mode."

#define CLIPOE_ALTER_A                      "alternative-a"
#define CLIPOE_ALTER_B                      "alternative-b"

#define CLIPOE_ALTER_A_HELP                 "Sets alternative A pair for power delivering."
#define CLIPOE_ALTER_B_HELP                 "Sets alternative B pair for power delivering."
#define CLISYNTAX_PORT_PAIR                 "use 'poe port-pair alternative-a | alternative-b'."
#define CLISYNTAX_PORT_PAIR_NO              "use 'no poe port-pair'."


#define CLIPOE_HIGH                         "High Priority"
#define CLIPOE_CRIT                         "Critical Priority"
#define CLIPOE_LOW                          "Low Priority"

#define CLI_POE_TOTAL_POWER_AVAILABLE       "Total Power Available"
#define CLI_POE_MAIN_OPER_STATUS            "PSE Main Operational Status"
#define CLI_POE_TOTAL_POWER_CONSUMED        "Total Power Consumed"
#define CLI_POE_USAGE_THRESHOLD             "Usage Threshold"
#define CLI_POE_POWER_MANAGEMENT_MODE       "Power Management Mode"
#define CLI_POE_AUTO_RESET_MODE             "Configure port Auto Reset Mode."
#define CLI_POE_PORT_MAP_MODE               "Logical Port Mapping Mode"
#define CLI_POE_REQUESTING_POWER            "Requesting Power"
#define CLI_POE_SEARCHING                   "Searching"
#define CLI_POE_DELIVERING_POWER            "Delivering Power"
#define CLI_POE_FAULT                       "Fault"
#define CLI_POE_OTHER_FAULT                 "Other Fault"
#define CLI_POE_TEST                        "Test"
#define CLIPOE_HIGH_2                       "High"
#define CLIPOE_CRIT_2                       "Crit"
#define CLIPOE_LOW_2                        "Low"
#define CLIPOE_HIGH_1                       "high"
#define CLIPOE_CRIT_1                       "crit"
#define CLIPOE_LOW_1                        "low"
#define CLI_POE_CLASS_BASED                 "Class Based"
#define CLI_POE_USER_DEFINED                "User Defined"
#define CLI_POE_LEGACY                      "Legacy"
#define CLI_POE_2PT_DOT3AF                  "2Pt-Dot3af"
#define CLI_POE_4PT_DOT3AF                  "4Pt-Dot3af"
#define CLI_POE_2PT_DOT3AF_LEG              "2Pt-Dot3af + Legacy"
#define CLI_POE_4PT_DOT3AF_LEG              "4Pt-Dot3af + Legacy"
#define CLI_POE_OPER_STATUS_ON              "ON."
#define CLI_POE_OPER_STATUS_OFF             "OFF."
#define CLI_POE_OPER_STATUS_FAULTY          "Faulty."
/* end */

/*****************************************************************/

/* BEGIN POE TREE COMMAND STRUCTURE */
extern void buildTreeInterfacePoe(EwsCliCommandP depth1);
extern void buildTreeGlobalPoe(EwsCliCommandP depth1);
extern void buildTreePrivPoeShow(EwsCliCommandP depth2);

/* BEGIN POE ACTION COMMANDS DECL */
extern const L7_char8  *commandGlobalPoeReset(EwsContext ewsContext, L7_uint32 argc,
                                              const L7_char8 **argv, L7_uint32 index);
extern const L7_char8  *commandGlobalPoe(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 **argv,
                                         L7_uint32 index);
extern const L7_char8  *commandInterfacePoeReset(EwsContext ewsContext,
                                                 L7_uint32 argc,
                                                 const L7_char8 **argv,
                                                 L7_uint32 index);
extern const L7_char8  *commandInterfacePoe(EwsContext ewsContext,
                                            L7_uint32 argc,
                                            const L7_char8 **argv,
                                            L7_uint32 index);
extern const L7_char8  *commandGlobalPoeLimit(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 **argv,
                                              L7_uint32 index);
extern const L7_char8  *commandInterfacePoeLimit(EwsContext ewsContext,
                                                 L7_uint32 argc,
                                                 const L7_char8 **argv,
                                                 L7_uint32 index);
extern const L7_char8  *commandGlobalPoePriority(EwsContext ewsContext,
                                                 L7_uint32 argc,
                                                 const L7_char8 **argv,
                                                 L7_uint32 index);
extern const L7_char8  *commandInterfacePoePriority(EwsContext ewsContext,
                                                    L7_uint32 argc,
                                                    const L7_char8 **argv,
                                                    L7_uint32 index);
extern const L7_char8  *commandConfigPoeUsageThreshold(EwsContext ewsContext,
                                                       L7_uint32 argc,
                                                       const L7_char8 **argv,
                                                       L7_uint32 index);

const L7_char8  *commandGlobalPoePowerMgmt(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandGlobalPoePowerSource( EwsContext ewsContext, L7_uint32 argc,
                                               const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandGlobalPoePortMap(EwsContext ewsContext, L7_uint32 argc, 
                                           const L7_char8 **argv, L7_uint32 index);

const L7_char8  *commandGlobalPoeHighPower(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandGlobalPoeAutoReset(EwsContext ewsContext, L7_uint32 argc,
                                           const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandGlobalPoeDetection(EwsContext ewsContext, L7_uint32 argc,
                                          const L7_char8 **argv, L7_uint32 index);

const L7_char8  *commandInterfacePoeHighPower(EwsContext ewsContext, L7_uint32 argc,
                                              const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandInterfacePoeDetection(EwsContext ewsContext, L7_uint32 argc,
                                              const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandInterfacePoeDisconnect(EwsContext ewsContext, L7_uint32 argc,
                                              const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandInterfacePoePortPair(EwsContext ewsContext, L7_uint32 argc,
                                                  const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandInterfacePoeAutoPowerup(EwsContext ewsContext, L7_uint32 argc, 
                                              const L7_char8 **argv, L7_uint32 index);

const char  *commandShowPoeInfo(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 **argv, L7_uint32 index);
const char  *commandShowPoePort(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 **argv, L7_uint32 index);
const char  *commandShowPoePortConfig(EwsContext ewsContext, L7_uint32 argc,
                                const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandInterfacePoePortMap(EwsContext ewsContext, L7_uint32 argc,
                                                          const L7_char8 **argv, L7_uint32 index);

/* END POE ACTION COMMANDS DECL */
#endif
