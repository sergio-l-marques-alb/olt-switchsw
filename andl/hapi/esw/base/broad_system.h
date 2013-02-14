/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_system.h
*
* @purpose   This file contains the hapi interface for system parameters
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_SYSTEM_H
#define INCLUDE_BROAD_SYSTEM_H

#include "dapi.h"

#define IGMP_QUERY          0x11
#define IGMP_V2_REPORT      0x16  /* version 2 */
#define IGMP_V1_REPORT      0x12  /* version 1 */
#define IGMP_LEAVE          0x17

#define IGMP_PROT        2 /* protocol type for IGMP */

/* DA bytes offsets */

#define DEST_MAC_OUI_1      0x00
#define DEST_MAC_OUI_2      0x01
#define DEST_MAC_OUI_3      0x02
#define DEST_MAC_OUI_4      0x03
#define DEST_MAC_OUI_5      0x04
#define DEST_MAC_OUI_6      0x05

/* first three bytes in DA of IGMP frame */

#define IGMP_MAC_OUI_1      0x01
#define IGMP_MAC_OUI_2      0x00
#define IGMP_MAC_OUI_3      0x5e

/* packet related defines */
#define ETHER_PKT          0
#define PKT_802_3          1
#define PROT_TYPE_OFFSET   16 
#define IP_PROT_OFFSET     27 
#define IGMP_MSG_OFFSET    38

#define BCM_PMUX_PR_MCST_FM (BCM_PMUX_PR_MCST | BCM_PMUX_PR_FM)
#define BCM_PMUX_XGS_PR_MCST_FM (BCM_PMUX_PR_L2MC_MISS | BCM_PMUX_PR_FFP)



/*********************************************************************
*
* @purpose Hooks in basic dapi cmds
*
* @param   DAPI_PORT_t *dapiPortPtr - used to hook in commands for a port
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadStdPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose Configures source and probe ports
*
* @param   DAPI_USP_t *fromUsp    - identifies the source port
* @param   DAPI_USP_t *toUsp      - identifies the probe port
* @param   DAPI_t     *dapi_g     - the driver object
* @param   L7_BOOL    add         - action (add/remove)
* @param   L7_uint32  probeType   - specifies mirroring mode (ingress/egress/both)
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemMirroringSet(DAPI_USP_t *fromUsp,
                                    DAPI_USP_t *toUsp,
                                    DAPI_t *dapi_g,
                                    L7_BOOL add,
                                    L7_uint32 probeType);

/*********************************************************************
*
* @purpose Implements mirroring
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_MIRRORING
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.mirroring
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemMirroring(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Modifies mirrored ports list
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_MIRRORING
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.mirroringPortModify
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemMirroringPortModify(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Adds/removes probe port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_t     *dapi_g - the driver object
* @param   add                - indicates add or remove probe
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemMirroringProbeSet(DAPI_USP_t *usp, DAPI_t *dapi_g, L7_BOOL add);

/*********************************************************************
*
* @purpose Returns Probe Status Active/Inactive 
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_BOOL result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadSystemMirroringUSPIsProbe(DAPI_USP_t *usp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Adds/removes mirror port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_t     *dapi_g - the driver object
* @param   add                - indicates add or remove mirror
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemMirroringMirrorSet(DAPI_USP_t *usp, DAPI_t *dapi_g, L7_BOOL add, L7_uint32 probeType);

/*********************************************************************
*
* @purpose Sets the IP address
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_SYSTEM_IP_ADDRESS
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.systemIpAddress
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   Used for L2 ONLY PRODUCTS
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemIpAddress(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Configures the System MAC address and Management Vlan
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp 
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_SYSTEM_MAC_ADDRESS
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.systemMacAddress
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemMacAddress(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Configures broadcast control for the system
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.broadcastControl
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemBroadcastControlModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Configures storm control for an interface
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BROADCAST_CONTROL_MODE_SET
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.broadcastControl
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfBroadcastControlModeSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, 
                                             DAPI_t *dapi_g) ;

/*********************************************************************
*
* @purpose Sets the flow control thresholds for the ports specified.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_FLOW_CONTROL
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.flowControl
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadInterfaceFlowControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Sets the flow control thresholds
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_FLOW_CONTROL
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.flowControl
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemFlowControl(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Sets the speed of the interface
*
* @param   DAPI_USP_t *usp    - usp for the current interface
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_SPEED_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.portSpeedConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfSpeedConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Sets the autonegotiation capabilities of an interface
*
* @param   DAPI_USP_t *usp    - usp for the current interface
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_AUTO_NEGOTIATE_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.autoNegotiateConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfAutoNegotiateConfig(DAPI_USP_t *usp,
                                DAPI_CMD_t cmd, void *data,
                                DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Registers a callback for a particular family
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_STP_STATE
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.stpState
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfStpState(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Configure Loopback for the interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_LOOPBACK_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.portLoopbackConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   DEBUG mode for now.  The application does not call this yet.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfLoopbackConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Configure Isolate bit in Phy for the interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_ISOLATE_PHY_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.portIsolatePhyConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIsolatePhyConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Configure IGMP/MLD Snooping
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_IGMP_SNOOP_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.snoopConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemSnoopConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/* PTin added: DHCP snooping */
#if 1
/*********************************************************************
*
* @purpose Configure DHCP Snooping
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_IGMP_SNOOP_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.snoopConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDhcpConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);
#endif

/*********************************************************************
*
* @purpose Get board family (Strata for 5615 and Draco(XGS) for 5690)
*
* @param   DAPI_USP_t *board_family - pointer to board_family
*
* @returns L7_RC_t result and 
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGetSystemBoardFamily(bcm_chip_family_t *board_family);

/*********************************************************************
*
* @functions hapiBroadIntfMaxFrameSizeConfig
*
* @purpose config physical port layer 2 max frame size
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns DAPI_RESULT result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfMaxFrameSizeConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @functions hapiBroadIntfPfcConfig
*
* @purpose Configuration of Priority Based Flow Control
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns DAPI_RESULT result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfPfcConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Get a PFC statistic for an interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_PFC_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.pfcConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns 
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfPfcStatGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Clear PFC statistics for an interface
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_PFC_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.pfcConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns 
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfPfcStatsClear(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose To configure Denial of Service Functions
*
* @param   DAPI_USP_t *usp    - usp of the port on which ipv6 provisioning
*                               feature is being enabled or disabled
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_DOS_CONFIG
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dosControlConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDosControlConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose To configure Per Port Denial of Service Functions
*
* @param   DAPI_USP_t *usp    - usp of the port on which ipv6 provisioning
*                               feature is being enabled or disabled
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_DOS_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.dosControlConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDosControlConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Set the BPDU FIletring
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BPDU_FILTERING
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.bpduFiltering
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfBpduFiltering(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Set the BPDU Guard mode for the port
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BPDU_GUARD
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.bpduGuardMode
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfBpduGuard(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, 
                               DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Set the BPDU Flood for the port
*
* @param   DAPI_USP_t *usp    - not used in this function
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_BPDU_FLOOD
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.bpduFlood
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfBpduFlood(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @functions hapiBroadIntfCableTest
*
* @purpose check cable status
*
* @param DAPI_USP_t *usp
* @param DAPI_CMD_t cmd
* @param void *data
* @param DAPI_t *dapi_g
*
* @returns DAPI_RESULT result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfCableTest(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Functions to add or delete a PING flooding filter to FFP
*
* @param   DAPI_USP_t *usp    - usp of the port on which DOS Ping flooding filter
*                               feature is being enabled or disabled
* @param   DAPI_t     *dapi_g - the driver object
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dosControlConfig
* @param   L7_uint32  enableFilter  - L7_ENABLE:  To add a filter entry
*                                     L7_DISABLE: To delete a filter entry
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function is called to add or delete an DOS entry.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDosPingFloodingFilter(DAPI_USP_t *usp, DAPI_t *dapi_g, void *data,
        L7_uint32 enableFilter);

/*********************************************************************
*
* @purpose Functions to add or delete a Smurf attack filter to FFP
*
* @param   DAPI_USP_t *usp    - usp of the port on which DOS smurf attack
*                               feature is being enabled or disabled
* @param   DAPI_t     *dapi_g - the driver object
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dosControlConfig
* @param   L7_uint32  enableFilter - L7_ENABLE:  To add a filter entry
*                                    L7_DISABLE: To delete a filter entry
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function is called to add or delete an DOS entry.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDosSmurfAttackFilter(DAPI_USP_t *usp, DAPI_t *dapi_g, void *data,
        L7_uint32 enableFilter);


/*********************************************************************
*
* @purpose Functions to add or delete a invalid tcp filter to FFP
*
* @param   DAPI_USP_t *usp    - usp of the port on which DOS smurf attack
*                               feature is being enabled or disabled
* @param   DAPI_t     *dapi_g - the driver object
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dosControlConfig
* @param   L7_uint32  enableFilter - L7_ENABLE:  To add a filter entry
*                                    L7_DISABLE: To delete a filter entry
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function is called to add or delete an DOS entry.
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadSystemInvalidTcpFlagFilter(DAPI_USP_t *usp, DAPI_t *dapi_g, void *data,
                                              L7_uint32 enableFilter);


/*********************************************************************
*
* @purpose Functions to add or delete a SYN ACK flooding filter to FFP
*
* @param   DAPI_USP_t *usp    - usp of the port on which SYN ACK Flooding
*                               filter feature is being enabled or disabled
* @param   DAPI_t     *dapi_g - the driver object
* @param   void       *data   - DAPI_SYSTEM_CMD_t.cmdData.dosControlConfig
* @param   L7_uint32  enableFilter - L7_ENABLE:  To add a filter entry
*                                    L7_DISABLE: To delete a filter entry
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes   This function is called to add or delete an DOS entry.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDosSynAckFloodingFilter(DAPI_USP_t *usp, DAPI_t *dapi_g, void *data,
                                              L7_uint32 enableFilter);


/*********************************************************************
*
* @purpose Sets the priority used for sampled packets sent to CPU (for sFlow)
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_CPU_SAMPLE_PRIORITY
* @param   void       *data   - DAPI_SYSTEM_CMD_t->cmdData.sFlowConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemCpuSamplePriority(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Sets the seed for the random number generator that determines if a packet
*          should be sampled (for sFlow)
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_SYSTEM_SAMPLE_RANDOM_SEED
* @param   void       *data   - DAPI_SYSTEM_CMD_t->cmdData.sFlowConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemSampleRandomSeed(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Sets the sampling rate of packets ingressing or egressing a port (for sFlow)
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_SAMPLE_RATE
* @param   void       *data   - DAPI_SYSTEM_CMD_t->cmdData.sFlowConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfSampleRate(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
 *
 * @purpose  Used to Diagnostic Cable on a fiber interface.
 *
 * @param   *usp           @b{(input)} The USP of the port that is to be acted upon
 * @param    cmd           @b{(input)} DAPI_CMD_INTF_CABLE_DIAG_NAME
 * @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.CableDiagStatus
 * @param   *dapi_g        @b{(input)} The driver object
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIntfFiberDiagTest(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data,
        DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Enables/disables the Blinking of port LEDs
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_BLINK_SET 
* @param   void       *data   - DAPI_SYSTEM_CMD_t->cmdData.ledConfig
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfLedBlinkSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

#if L7_FEAT_CUSTOM_LED_BLINK
/*********************************************************************
* @purpose  Enable/Disable LED blinking per customer requirements
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t hapiBroadLedBlinkEnableDisable(L7_BOOL blink);

#endif 

/*********************************************************************
*
* @purpose Sets the ISDP policy for the port, given the ISDP
*          config and the LLPF config
*
* @returns 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIsdpPortUpdate(DAPI_USP_t *usp, 
                                L7_BOOL     oldLlpfEnabled,
                                L7_BOOL     newLlpfEnabled,
                                L7_BOOL     oldIsdpEnabled,
                                L7_BOOL     newIsdpEnabled,
                                DAPI_t     *dapi_g);

/*********************************************************************
*
* @purpose Sets the ISDP policy for the port, given the voice VLAN
*          config and the LLPF config
*
* @returns 
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIsdpPolicySet(DAPI_USP_t *usp, 
                               L7_BOOL     oldLlpfEnabled,
                               L7_BOOL     newLlpfEnabled,
                               L7_BOOL     oldVoiceVlanEnabled,
                               L7_BOOL     newVoiceVlanEnabled,
                               DAPI_t     *dapi_g);

/*********************************************************************
 *
 * @purpose To Enable/Disable ISDP on a  port.
 *
 * @param   DAPI_USP_t *usp    - needs to be a valid usp
 * @param   DAPI_CMD_t  cmd    - DAPI_CMD_ISDP_INTF_STATUS_SET
 * @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
 * @param   DAPI_t     *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes   We do not support specification of the destination ports
 *          on Broadcom.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIsdpStatusSet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

#endif

