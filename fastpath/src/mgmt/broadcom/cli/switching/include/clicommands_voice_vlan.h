
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename clicommands_voice_vlan.h
*
* @purpose header for voice vlan commands in clicommands.c
*
* @component user interface
*
* @comments none
*
* @create  
*
* @author  
* @end
*
**********************************************************************/

#ifndef CLICOMMANDS_VOICE_VLAN_H
#define CLICOMMANDS_VOICE_VLAN_H

#include "l7_common.h"

/*****************************
**
** BEGIN VOICE VLAN FUNCTIONS HELP
**
******************************/
/* general voice vlan help strings */

/* string help for the config functions */
#define CLI_VOICE_VLAN_MODE_HELP              "Enable/Disable Voice VLAN on the admin mode."
#define CLI_VOICE_VLAN_HELP                   "Configure Voice VLAN Parameters. "
#define CLI_VOICE_VLAN_ID_HELP                "Enter the Voice VLAN Id. "
#define CLI_VOICE_VLAN_PRIORITY_HELP          "Configure Voice VLAN 802.1p priority tagging for voice traffic. "
#define CLI_VOICE_VLAN_NONE_HELP              "Allow the IP phone to use its own configuration to send untagged voice traffic"
#define CLI_VOICE_VLAN_UNTAG_HELP             "Configure the phone to send untagged voice traffic. "
#define CLI_VOICE_VLAN_DATA_PRIORITY_HELP     "Enable/Disable trust or untrust the data traffic arriving on the voice vlan port"
#define CLI_VOICE_VLAN_DATA_PRIORITY_UNTRUST  "Do not trust the COS of the data traffic arriving on the voice vlan port"
#define CLI_VOICE_VLAN_DATA_PRIORITY_TRUST    "Trust the COS of the data traffic arriving on the voice vlan port"

#define CLISYNTAX_VOICEVLANMODE               "\r\nIncorrect input! Use 'voice vlan'."    
#define CLISYNTAX_VOICEVLANMODE_NO            "\r\nIncorrect input! Use 'no voice vlan'."
#define CLISYNTAX_CONFIGPORTVOICEVLAN_SYNTAX  "\r\nIncorrect input! Use 'voice vlan <vlan-id>'."
#define CLISYNTAX_CONFIGVOICEVLANNO_SYNTAX    "\r\nIncorrect input! Use 'no voice vlan'."
#define CLISYNTAX_CONFIGPORTVOICEVLANPRIORITY_SYNTAX   "\r\nIncorrect input! Use 'voice vlan dot1p <priority>'."
#define CLISYNTAX_CONFIGPORTVOICEVLANNOPRIORITY_SYNTAX "\r\nIncorrect input! Use 'no voice vlan dot1p <priority>'."
#define CLISYNTAX_VOICEVLANNONE_SYNTAX         "\r\nIncorrect input! Use 'voice vlan none'."        
#define CLISYNTAX_VOICEVLANNONENO_SYNTAX       "\r\nIncorrect input! Use 'no voice vlan none'."
#define CLISYNTAX_VOICEVLANUNTAG_SYNTAX        "\r\nIncorrect input! Use 'voice vlan untagging'."
#define CLISYNTAX_VOICEVLANUNTAGNO_SYNTAX      "\r\nIncorrect input! Use 'no voice vlan untagging'."
#define CLISYNTAX_VOICEVLANCOS_OVERRIDE_SYNTAX "\r\nIncorrect input! Use 'voice vlan data priority <untrust | trust>'."
#define CLISYNTAX_VOICEVLANCOS_OVERRIDENO_SYNTAX "\r\nIncorrect input! Use 'no voice vlan data priority '."

#define CLISYNTAX_CONFIGPORTVOICEVLAN         "\r\nIncorrect input! Use '[no] voice vlan <%d-%d>'."
   
#define CLIVOICEVLANADMINMODE_ERR             "\r\nError: Unable to set the voice vlan on the admin mode."
#define CLIPORTVOICEVLAN_FAILURE              "\r\nError: Unable to set the voice vlan."
#define CLIPORTVOICEVLAN_ERR                  "\r\nError: VLAN id does not exist in DOT1Q database."  
#define CLIPORTVOICEVLANPRIORITY_ERR          "\r\nError: Unable to set the voice vlan priority."
#define CLIVOICEVLANNONE_ERR                  "\r\nError: Unable to set the voice vlan none."
#define CLIVOICEVLANUNTAG_ERR                 "\r\nError: Unable to set the voice vlan untag."
#define CLIVOICEVLANCOSOVERRIDE_ERR           "\r\nError: Unable to override the default cos priority for the data traffic."
#define CLIVOICEVLANPORTMODEINVALID_ERR       "\r\nError: Unable to get the port mode for voice vlan."

#define CLISHOWVOICEVLANADMINMODE_ERR         "\r\nError: Unable to display the voice vlan administrative mode."
#define CLISHOWVOICEVLANMODE_ERR              "\r\nError: Unable to display the voice vlan port mode."
#define CLISHOWVOICEVLANCOSOVERRIDE_ERR       "\r\nError: Unable to display the voice vlan cos override mode."


#define CLIVOICEVLAN_INTERFACE                "Interface."
#define CLIVOICEVLAN_MODE                     "Voice VLAN Interface Mode."
#define CLIVOICEVLAN_ID                       "Voice VLAN ID."
#define CLIVOICEVLAN_PRIORITY                 "Voice VLAN Priority."
#define CLIVOICEVLAN_NONE                     "Voice VLAN None Mode."
#define CLIVOICEVLANUNTAG                     "Voice VLAN Untagging."
#define CLIVOICEVLANCOSOVERRIDE               "Voice VLAN COS Override."
#define CLIVOICEVLAN_STATUS                   "Voice VLAN Port Status."

#define CLISYNTAX_SHOWVOICEVLANPORTDETAIL     "\r\nIncorrect input! Use 'show voice vlan interfaces <%s|all>'."
#define CLIVOICEVLANSHOW_HELP                 "Display Voice VLAN Mode and settings."

void buildTreeGlobalConfigVoiceVlan(EwsCliCommandP depth1);
void buildTreeInterfaceConfigVoiceVlan(EwsCliCommandP depth1);
void buildTreeUserExecVoiceVlan(EwsCliCommandP depth1);


const L7_char8  *commandVoiceVlan(EwsContext ewsContext, 
                                  L7_uint32 argc, 
                                  const L7_char8 **argv, 
                                  L7_uint32 index);
const L7_char8  *commandVoiceVlanId(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 **argv,
                                         L7_uint32 index);   
const L7_char8  *commandVoiceVlanPriority(EwsContext ewsContext,
                                         L7_uint32 argc,
                                         const L7_char8 **argv,
                                         L7_uint32 index);
const L7_char8  *commandVoiceVlanUntagged(EwsContext ewsContext, L7_uint32 argc, const L7_char8 **argv, L7_uint32 index);
const L7_char8  *commandVoiceVlanNone(EwsContext ewsContext, 
                                      L7_uint32 argc, 
                                      const L7_char8 **argv, 
                                      L7_uint32 index);

const L7_char8  *commandVoiceVlanDataPriority(EwsContext ewsContext, 
                                              L7_uint32 argc, 
                                              const L7_char8 **argv, 
                                              L7_uint32 index);


#endif


