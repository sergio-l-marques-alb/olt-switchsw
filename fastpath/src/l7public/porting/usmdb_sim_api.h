/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\l7public\porting\usmdb_sim_api.h
*
* @purpose externs for USMDB layer
*
* @component unitmgr
*
* @comments none
*
* @create 01/10/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#ifndef USMDB_SIM_API_H
#define USMDB_SIM_API_H

#include "usmdb_common.h"
#include "osapi.h"
#include "usmdb_ip_base_api.h"
#include "file_exports.h"


/*********************************************************************
*
* @purpose Retrieve the Unit's Current System Config Mode
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to the Current System Config Mode
*
* @returns L7_SUCCESS
*
* @notes Valid System Config Modes:
* @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
*        @row{@cell{L7_SYSCONFIG_MODE_BOOTP}}
*        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigNetworkConfigProtocolCurrentGet(L7_uint32 UnitIndex, L7_uint32 *val);


/*********************************************************************
*
* @purpose Retrieve the Unit's System Config Mode
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to the System Config Mode
*
* @returns L7_SUCCESS
*
* @notes Valid System Config Modes:
* @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
*        @row{@cell{L7_SYSCONFIG_MODE_BOOTP}}
*        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Retrieve the Unit's System IPv6 Config Mode
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 *val      @b((output)) Pointer to the System Config Mode
 *
 * @returns L7_SUCCESS
 *
 * @notes Valid System Config Modes:
 * @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
 *        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigNetworkIPv6ConfigProtocolGet(L7_uint32 UnitIndex,
                                                                 L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Retrieve the IPv6 AutoConfig Mode of Network port
 *
 * @param L7_uint32 *val   @b((output)) Whether autoconfiguration is
 *                                      enabled or disabled
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbNetworkIPv6AddrAutoConfigGet(L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Set the IPv6 AutoConfig Mode of Network port
 *
 * @param L7_uint32 val   @b((input)) Whether autoconfiguration is
 *                                    enabled or disabled
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbNetworkIPv6AddrAutoConfigSet(L7_uint32 val);

/*********************************************************************
*
* @purpose Set the Unit's System Config Mode
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) System Config Mode
*
* @returns L7_SUCCESS
*
* @notes Valid System Config Modes:
* @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
*        @row{@cell{L7_SYSCONFIG_MODE_BOOTP}}
*        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigProtocolDesiredSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
 *
 * @purpose Set the Unit's System IPv6 Config Mode
 *
 * @param L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param L7_uint32 val       @b((input)) System Config Mode
 *
 * @returns L7_SUCCESS
 *
 * @notes Valid System Config Modes:
 * @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
 *        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbAgentBasicIPv6ConfigProtocolSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Retrieve the Unit's Service Port Config Mode
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to the Service Port Config Mode
*
* @returns L7_SUCCESS
*
* @notes Valid Service Port Config Modes:
* @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
*        @row{@cell{L7_SYSCONFIG_MODE_BOOTP}}
*        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(L7_uint32 UnitIndex,
                                                                     L7_uint32 *val);

/*********************************************************************
*
* @purpose Retrieve the Unit's Service Port admin state
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to the Service Port admin state
*
* @returns L7_SUCCESS
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t usmDbServPortAdminStateGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the Unit's Service Port admin state
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) Service Port admin state
*
* @returns L7_SUCCESS
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t usmDbServPortAdminStateSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Retrieve the Unit's Service Port link state
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to the Service Port link status
*
* @returns L7_SUCCESS
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t usmDbServPortLinkStateGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Retrieve the Unit's Service Port IPv6 Config Mode
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 *val      @b((output)) Pointer to the Service Port Config Mode
 *
 * @returns L7_SUCCESS
 *
 * @notes Valid Service Port Config Modes:
 * @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
 *        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigServPortIPv6ConfigProtocolGet(L7_uint32 UnitIndex,
                                                                  L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Retrieve the IPv6 AutoConfig Mode of Service port
 *
 * @param L7_uint32 *val   @b((output)) Whether autoconfiguration is
 *                                      enabled or disabled
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbServPortIPv6AddrAutoConfigGet(L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Set the IPv6 AutoConfig Mode of Service port
 *
 * @param L7_uint32 val   @b((input)) Whether autoconfiguration is
 *                                    enabled or disabled
 *
 * @returns L7_SUCCESS
 *
 * @notes
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbServPortIPv6AddrAutoConfigSet(L7_uint32 val);

/*********************************************************************
*
* @purpose Retrieve the Unit's Current Service Port Config Mode
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to the Current Service Port Config Mode
*
* @returns L7_SUCCESS
*
* @notes Valid Service Port Config Modes:
* @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
*        @row{@cell{L7_SYSCONFIG_MODE_BOOTP}}
*        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigServPortConfigProtocolCurrentGet(L7_uint32 UnitIndex,
                                                                     L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the Unit's Service Port Config Mode
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) Service Port Config Mode
*
* @returns L7_SUCCESS
*
* @notes Valid Service Port Config Modes:
* @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
*        @row{@cell{L7_SYSCONFIG_MODE_BOOTP}}
*        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigServPortProtocolDesiredSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
 *
 * @purpose Set the Unit's Service Port IPv6 Config Mode
 *
 * @param L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param L7_uint32 val       @b((input)) Service Port Config Mode
 *
 * @returns L7_SUCCESS
 *
 * @notes Valid Service Port Config Modes:
 * @table{@row{@cell{L7_SYSCONFIG_MODE_NONE}}
 *        @row{@cell{L7_SYSCONFIG_MODE_DHCP}}}
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigServPortIPv6ProtocolSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Get MIB capability description
*
* @param    L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param    L7_uint32 Index     @b((input))  Interface
* @param    L7_char8  buf       @b((output)) Description
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if error
* @returns  L7_FAILURE  if failure
*
* @notes    none.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentMibCapabilityDescrGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf);

/*********************************************************************
*
* @purpose Returns the Unit's System Burned in MAC Address
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_char8  *buf      @b((output)) Buffer returning the MAC Address
*
* @returns L7_SUCCESS
*
* @notes MAC Address is L7_MAC_ADDR_LEN bytes long
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlBurnedInMacAddrGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Retrieve the Unit's System Locally Administered Mac Address
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_char8  *buf      @b((output)) Buffer returning the MAC Address
*
* @returns L7_SUCCESS
*
* @notes MAC Address is L7_MAC_ADDR_LEN bytes long
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlLocalAdminAddrGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
* @purpose  Sets the Unit's System Locally Administered Mac Address
*
* @param    UnitIndex Unit Index
* @param    *buf      pointer to system local admin mac address
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlLocalAdminAddrSet(L7_uint32 UnitIndex, L7_char8 *buf);


/*********************************************************************
*
* @purpose Retrieve the Unit's System Mac Address Type
*
* @param L7_uint32 UnitIndex @b((input))  Unit for this operation
* @param L7_uint32 *val      @b((output)) System Mac Address Type
*
* @returns L7_SUCCESS
*
* @notes Valid System Mac Address Types:
* @table{@row{@cell{L7_SYSMAC_BIA}}
*        @row{@cell{L7_SYSMAC_LAA}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMacAddrTypeGet(L7_uint32 UnitIndex, L7_uint32 *val);


/*********************************************************************
*
* @purpose Set the Unit's System Mac Address Type
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) System Mac Address Type
*
* @returns L7_SUCCESS
*
* @notes Valid System Mac Address Types:
* @table{@row{@cell{L7_SYSMAC_BIA}}
*        @row{@cell{L7_SYSMAC_LAA}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlMacAddrTypeSet(L7_uint32 UnitIndex, L7_uint32 val);


/*********************************************************************
* @purpose  Set Unit's System Rate control
*
* @param    UnitIndex   unit index
* @param    val       rate control
*
* @returns  L7_SUCCESS   if success
* @returns  L7_FAILURE   if failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlRateControlSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Resets the system
*
* @param    UnitIndex   unit index
* @param    val
*
* @returns  L7_SUCCESS   if success
* @returns  L7_FAILURE   if failure
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlResetSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Get the Save Applied Changes Result
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to applied changes result
*
* @returns usmWebBuffer with the Save Applied Changes Result if applicable
*
* @notes  none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlSaveConfigurationGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
 * @purpose  Saves the configuration.
 *
 * @param    L7_uint32             UnitIndex  @b((input)) The unit for this operation
 * @param    L7_SAVE_CONFIG_TYPE_t val        @b((input)) Type of configuration
 *
 * @returns  L7_SUCCESS   if success
 * @returns  L7_FAILURE   if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
extern L7_RC_t usmDbSwDevCtrlSaveConfigurationSet(L7_uint32 UnitIndex, L7_SAVE_CONFIG_TYPE_t val);


/*********************************************************************
* @purpose  Saves the configuration.
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
extern void usmDbSwDevCtrlComponentConfigAllSave(void);

/*********************************************************************
* @purpose  Returns the Unit's System Web Mode
*
* @param    L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param    L7_uint32 *val       @b((output)) Pointer to mode
*
* @returns  System Web Mode (L7_ENABLE or L7_DISABLE)
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlWebMgmtModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Sets the Unit's System Web Mode
*
* @param    UnitIndex   Unit Index
* @param    val         System Web Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSwDevCtrlWebMgmtModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Sets maximum number of web sessions
*
* @param    val  maximum allowable number of web sessions
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbCliWebHttpNumSessionsSet(L7_uint32 val);

/*********************************************************************
* @purpose  Get the maximum number of web sessions
*
* @returns  Return the maximum number of web sessions
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbCliWebHttpNumSessionsGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Sets http session hard timeout (in hours)
*
* @param    val  http session hard timeout
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbCliWebHttpSessionHardTimeOutSet(L7_uint32 val);

/*********************************************************************
* @purpose  Get the http session hard timeout (in hours)
*
* @returns  Return the http session hard timeout
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbCliWebHttpSessionHardTimeOutGet(L7_uint32 *val);

/*********************************************************************
* @purpose  Sets http session soft timeout (in minutes)
*
* @param    val  http session soft timeout
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbCliWebHttpSessionSoftTimeOutSet(L7_uint32 val);

/*********************************************************************
* @purpose  Get the http session soft timeout (in minutes)
*
* @returns  Return the http session soft timeout
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbCliWebHttpSessionSoftTimeOutGet(L7_uint32 *val);


/*********************************************************************
*
* @purpose Get web java mode
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 val       @b((output)) Pointer to java mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbWebJavaModeGet(L7_uint32 UnitIndex, L7_uint32 *val);


/*********************************************************************
*
* @purpose Set web java mode
*
* @param L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param L7_uint32 val        @b((input)) java mode
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbWebJavaModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Retrieve the transfer mode
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Enum L7_TRANSFER_TYPES_t
*
* @returns L7_SUCCESS
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the transfer mode
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) Enum L7_TRANSFER_TYPES_t
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Set the TFTP server's IP address type
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 val       @b((output)) Ip Address Type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferServerAddressTypeSet(L7_uint32 UnitIndex,
                                                 L7_uint32 val);

/*********************************************************************
*
* @purpose Retrieve the TFTP server's IP address type
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Ip Address Type
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferServerAddressTypeGet(L7_uint32 UnitIndex,
                                                 L7_uint32 *val);

/*********************************************************************
*
* @purpose Retrieve the Ip Address of the Tftp Server
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Ip Address
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferServerAddressGet(L7_uint32 UnitIndex, L7_uchar8 *val);

/*********************************************************************
*
* @purpose Set the Ip Address of the Tftp Server
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uchar8 *val      @b((input)) pointer to Ip Address data
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferServerAddressSet(L7_uint32 UnitIndex, L7_uchar8 *val);

/*********************************************************************
*
* @purpose Retrieve the transfer file path
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) File Path
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFilePathGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Set the transfer file path
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) File Path
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFilePathSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Retrieve the transfer file name
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) File name
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFileNameGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Set the transfer file name
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uchar8 *buf      @b((input)) File name
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFileNameSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose  Get the unit number for the transfer
*
* @param    *unit  @b{(output)} transfer unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferUnitNumberGet(L7_uint32 *unit);

/*********************************************************************
*
* @purpose  Set the unit number for the transfer
* 
* @param    unit  @b{(input)} transfer unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return value is not used currently
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferUnitNumberSet(L7_uint32 unit);

/*********************************************************************
*
* @purpose Retrieve the local transfer file path
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) File Path
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFilePathLocalGet(L7_uint32 UnitIndex, L7_char8 *buf);


/*********************************************************************
*
* @purpose Set the local transfer file path
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) File Path
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFilePathLocalSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Retrieve the local transfer file name
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) File name
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFileNameLocalGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Set the local transfer file name
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uchar8 *buf      @b((input)) File name
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFileNameLocalSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Retrieve the remote transfer file path
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) File Path
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFilePathRemoteGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Set the remote transfer file path
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) File Path
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFilePathRemoteSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Retrieve the remote transfer file name
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) File name
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFileNameRemoteGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Set the remote transfer file name
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uchar8 *buf      @b((input)) File name
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferFileNameRemoteSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Retrieve the transfer upload file type
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Enum L7_FILE_TYPES_t
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferUploadFileTypeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the transfer upload file type
*
* @param UnitIndex L7_uint32 the unit for this operation
* @param val L7_uint32 enum L7_FILE_TYPES_t
*
* @returns L7_RC_t  L7_SUCCESS
* @returns L7_RC_t  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferUploadFileTypeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Retrieve the transfer download file type
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Enum L7_FILE_TYPES_t
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferDownloadFileTypeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the transfer download file type
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) Enum L7_FILE_TYPES_t
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferDownloadFileTypeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Start a download transfer
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferDownStartSet(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose Start a upload transfer
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferUpStartSet(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose Retrieve the status of a transfer
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Enum L7_TRANSFER_STATUS_t
* @param L7_char8  *buf      @b((output)) Printable string of transfer status
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferResultGet(L7_uint32 UnitIndex, L7_uint32 *val, L7_char8 *buf);

/*********************************************************************
*
* @purpose Retrieve the status of a transfer
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Enum L7_TRANSFER_CODE_t
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferUploadResultCodeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Retrieve the status of a transfer
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Enum L7_TRANSFER_CODE_t
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferDownloadResultCodeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
 *
 * @purpose Retrieve the downloaded stk image status
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 *val      @b((output)) Enum STK_RC_t
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
extern L7_RC_t usmDbTransferDownloadCodeStatusGet(L7_uint32 UnitIndex, L7_uint32 *val);


/*********************************************************************
*
* @purpose Get the status of the transfer
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_BOOL usmDbTransferInProgressGet(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose Set the status of the transfer
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_BOOL val @b((input)) The context for the transfer
*
* @notes L7_SUCCESS/L7_FAILURE
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferInProgressSet(L7_uint32 UnitIndex, L7_BOOL val);

/*********************************************************************
*
* @purpose Gets the context of the transfer
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
*
* @returns pointer of the context for this transfer
* @returns NULL
*
* @notes none
*
* @end
*
*********************************************************************/
extern void* usmDbTransferContextGet(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose Start a upload transfer
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param void *context @b((input)) The context of the transfer
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern void usmDbTransferContextSet(L7_uint32 UnitIndex, void *context);

/*********************************************************************
* @purpose  Clear the switch configuration to factory defaults
*
* @param    L7_uint32 UnitIndex  @b((input)) The unit for this operation
* @param    L7_uint32 val        @b((input)) (No-op)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  You can reset the configuration to factory default values without
* @notes  powering off the switch.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbResetConfigActionSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Check if user config data has changed
*
* @param    UnitIndex    @b((input))L7_uint32 the unit for this operation
* @param    *hasChanged  @b((output)) Pointer to changed flag (L7_TRUE/L7_FALSE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbUnsavedConfigCheck(L7_uint32 UnitIndex, L7_BOOL *hasChanged);

/*********************************************************************
* @purpose  Get time since the last time the switch status was reset
*
* @param    L7_uint32       UnitIndex  @b((input))  The unit for this operation
* @param    usmDbTimeSpec_T *ts        @b((output)) Pointer to L7_timespec structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbTimeSinceLastSwitchStatsResetGet(L7_uint32 UnitIndex, usmDbTimeSpec_t *ts);

/*********************************************************************
* @purpose  Get the next arp Entry
*
* @param    L7_uint32  UnitIndex  @b((input))  The unit for this operation
* @param    L7_long32  *index     @b((input/output)) Starting index to find next
* @param    L7_ulong32 *ipaAddr   @b((output)) Entry's ip address
* @param    L7_uchar8  *mac       @b((output)) Entry's mac address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if the next entry does not exist
*
* @comments Index of zero is used to start search if successful
*           index is next index
*
* @end
*********************************************************************/
extern L7_RC_t usmDbArpInfoGetNext(L7_uint32 UnitIndex, L7_long32 *index, L7_ulong32 *ipAddr, L7_uchar8 *mac);

/*********************************************************************
* @purpose  Clears the IP stack's ARP cache entries
*
* @param    UnitIndex  @b{(input)} the unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All the IP stack's ARP cache entries are cleared.
*
* @end
**********************************************************************/
extern L7_RC_t usmDbArpSwitchClear(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose  Gets the internal interface number based on the mac address
*           of an entry in the arp table.
*
* @param    L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param    L7_char8  *mac       @b((input))  Mac address
* @param    L7_uint32 *intIfNum  @b((output)) Internal interface number
* 
* @returns  L7_SUCCESS  if entry is found
* @returns  L7_FAILURE  if entry not found
*
* @notes    none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbArpEntryGet(L7_uint32 UnitIndex, L7_char8 *mac, L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose Set the Command Prompt
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_char8 *prompt @b((input)) Command Prompt
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Max length is L7_COMMANDPROMPT_SIZE
*
* @end
*********************************************************************/
extern L7_RC_t usmDbCommandPromptSet(L7_uint32 UnitIndex, L7_char8 *prompt);

/*********************************************************************
*
* @purpose Retrieve the Command Prompt
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_char8 *prompt @b((input)) Command Prompt
*
* @returns L7_SUCCESS
*
* @notes Max length is L7_COMMANDPROMPT_SIZE
*
* @end
*********************************************************************/
extern L7_RC_t usmDbCommandPromptGet(L7_uint32 UnitIndex, L7_char8 *prompt);

/*********************************************************************
*
* @purpose Set the Baud Rate for the Serial Interface
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 baudRate  @b((input)) Baud rate
*
* @returns L7_SUCCESS
* @returns L7_ERROR
*
* @notes Valid Baud Rates:
* @table{@row{@cell{L7_BAUDRATE_1200}}
*        @row{@cell{L7_BAUDRATE_2400}}
*        @row{@cell{L7_BAUDRATE_4800}}
*        @row{@cell{L7_BAUDRATE_9600}}
*        @row{@cell{L7_BAUDRATE_19200}}
*        @row{@cell{L7_BAUDRATE_38400}}
*        @row{@cell{L7_BAUDRATE_57600}}
*        @row{@cell{L7_BAUDRATE_115200}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigSerialBaudRateSet(L7_uint32 UnitIndex, L7_uint32 baudRate);

/*********************************************************************
*
* @purpose Get the Baud Rate for the Serial Interface
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *baudRate @b((output)) Baud rate
*
* @returns L7_SUCCESS
* @returns L7_ERROR
*
* @notes Valid Baud Rates:
* @table{@row{@cell{L7_BAUDRATE_1200}}
*        @row{@cell{L7_BAUDRATE_2400}}
*        @row{@cell{L7_BAUDRATE_4800}}
*        @row{@cell{L7_BAUDRATE_9600}}
*        @row{@cell{L7_BAUDRATE_19200}}
*        @row{@cell{L7_BAUDRATE_38400}}
*        @row{@cell{L7_BAUDRATE_57600}}
*        @row{@cell{L7_BAUDRATE_115200}}}
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentBasicConfigSerialBaudRateGet(L7_uint32 UnitIndex, L7_uint32 *baudRate);

/*********************************************************************
*
* @purpose Set the Timeout for the Serial Interface
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 timeout @b((input)) Timeout value in minutes
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes Range of L7_SERIAL_TIMEOUT_MIN - L7_SERIAL_TIMEOUT_MAX
*
* @end
*********************************************************************/
extern L7_RC_t usmDbSerialTimeOutSet(L7_uint32 UnitIndex, L7_uint32 timeOut);

/*********************************************************************
*
* @purpose Get the Timeout for the Serial Interface
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *timeout @b((output)) Timeout value in minutes
*
* @returns L7_SUCCESS
*
* @notes Range of L7_SERIAL_TIMEOUT_MIN - L7_SERIAL_TIMEOUT_MAX
*
* @end
***********************************************************************/
extern L7_RC_t usmDbSerialTimeOutGet(L7_uint32 UnitIndex, L7_uint32 *timeOut);

/*********************************************************************
*
* @purpose  Get the Serial Port Parameter
*
* @param    L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param    L7_uint32 parm    @b((input))  Serial port parameter
* @param    L7_uint32 *result @b((output)) Port parameter value
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
***********************************************************************/
extern L7_RC_t usmDbSerialPortParmGet(L7_uint32 UnitIndex, L7_uint32 parm, L7_uint32 *result);

/*********************************************************************
* @purpose  Gets the service port burned-in mac address
*
* @param    L7_uint32  UnitIndex  @b((input))  The unit for this operation
* @param    L7_uchar8 *macAddr  @b((output)) Pointer to MAC Address,
*                               (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbServPortMacAddressGet(L7_uint32 UnitIndex, L7_uchar8 *macAddr);

/*********************************************************************
*
* @purpose Get the total number of ports per box
*
* @param L7_uint32 UnitIndex    @b((input))  The unit for this operation
* @param L7_uint32 *maxNumPorts @b((output)) Total number of ports per box
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes The slotPop[] array will be filled in with L7_TRUE or L7_FALSE
* @notes slotPop[n] = L7_TRUE if slot n is populated with a feature card
* @notes slotPop[n] = L7_FALSE if slot n is not populated
* @notes The slot numbers are 0-based, ranging from 0 to L7_MAX_SLOTS_PER_UNIT-1
* @notes numPorts will give the number of ports per slot[n]
*
* @end
*********************************************************************/
extern L7_RC_t usmDbPortsPerBoxGet(L7_uint32 UnitIndex, L7_uint32 *maxNumPorts);

/*********************************************************************
*
* @purpose Get number of new telnet sessions
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to number of new sessions
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentTelnetNewSessionsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set number of telnet sessions
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 *val      @b((input)) Number of telnet sessions
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentTelnetNewSessionsSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Get telnet timeout value
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to timeout value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentTelnetTimeoutGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set telnet timeout value
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 val       @b((input)) Timeout value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentTelnetTimeoutSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Get number of telnet sessions
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 *val      @b((output)) Pointer to number of telnet sessions
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentTelnetNumSessionsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set number of telnet sessions
*
* @param L7_uint32 UnitIndex @b((input)) The unit for this operation
* @param L7_uint32 *val      @b((input)) Number of telnet sessions
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbAgentTelnetNumSessionsSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Gives a semaphore to transfer task
*
* @param direction
*
* @returns none
*
* @notes  none
*
* @end
*********************************************************************/
extern L7_RC_t L7_transfer_start(L7_uint32 direction);

/*********************************************************************
*
* @purpose Gets the file upload transfer status code
*
* @param currentStatus
*
* @returns none
*
* @notes  none
*
* @end
*********************************************************************/
extern void transfer_get_upload_status_code(L7_int32 *currentStatus);

/*********************************************************************
*
* @purpose Gets the file download transfer status code
*
* @param currentStatus
*
* @returns none
*
* @notes  none
*
* @end
*********************************************************************/
extern void transfer_get_download_status_code(L7_int32 *currentStatus);

/**************************************************************************
 *  NAME: set_code_validation_status
 *        This function takes the code given to it and uses XIO functions
 *        to set the STK file validation result code.
 *
 *  RETURNS:
 *
 *************************************************************************/
extern void set_download_code_status(L7_uint32 stkRc);

/*********************************************************************
 *
 * @purpose Gets the STK image validation result code
 *
 * @param currentStatus
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
extern void transfer_get_download_code_status(L7_int32 *currentStatus);

/*********************************************************************
*
* @purpose Gets the file transfer status
*
* @param currentStatus
* @param buf
*
* @returns none
*
* @notes  none
*
* @end
*********************************************************************/
extern void transfer_get_status(L7_int32 *currentStatus, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the management VLAN ID
*
* @param  L7_uint32 UnitIndex    @b((input))  The unit for this operation
* @param  L7_uint32 *val         @b((output)) vlan Id
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgmtVlanIdGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the management VLAN ID
*
* @param  L7_uint32 UnitIndex    @b((input))  The unit for this operation
* @param  L7_uint32 val          @b((input))  Vlan Id
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgmtVlanIdSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
*
* @purpose Get the management Port
*
* @param  L7_uint32 UnitIndex    @b((input))  The unit for this operation
* @param  L7_uint32 *val         @b((output)) Internal interface
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgmtPortGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
*
* @purpose Set the management Port
*
* @param  L7_uint32 UnitIndex    @b((input))  The unit for this operation
* @param  L7_uint32 val          @b((input))  Internal interface
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbMgmtPortSet(L7_uint32 UnitIndex, L7_uint32 val);

/* @p1261 end */
/*********************************************************************
* @purpose  checks IP Address and NetMask for subnet conflicts on the system
*
* @param    L7_uint32  UnitIndex    @b((input))  The unit for this operation
* @param    L7_uint32 interface     @b((input))  Iinternal interface index of the port ifIpAddress and ifNetMask are associated with
* @param    L7_uint32 ifIpAddress   @b((input))  IP Address to check for conflict
* @param    L7_uint32 ifNetMask     @b((input))  NetMask for ifIpAddress
* @param    L7_uint32 *errorMsg     @b((output)) Pointer to hold localized error message number if L7_TRUE is returned.
*
*
* @returns  L7_TRUE       if there is a conflict
* @returns  L7_FALSE      if there are no conflicts
*
* @notes    use FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM for Service port interface
* @notes    use FD_CNFGR_NIM_MIN_CPU_INTF_NUM for CPU interface
*
* @end
*********************************************************************/
L7_BOOL usmDbIPConfigConflict(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 ifIpAddress, L7_uint32 ifNetMask, L7_uint32 *errorMsg);

/*********************************************************************
* @purpose  Finds first valid IP address on this system
*
* @param    ipAddress    local address
*
* @notes    returns 0.0.0.0 if no valid IP addresses are found
*
* @end
*********************************************************************/
void usmDbIPFirstLocalAddress(L7_uint32 *ipAddress);

/* @p1261 start */

/*********************************************************************
*
* @purpose Retrieve the Current Unit's ID
*
* @param void
*
* @returns The Unit ID
*
* @notes none
*
* @end
*********************************************************************/
extern L7_uint32 usmDbThisUnitGet(void);

/*********************************************************************
* @purpose  Configure the interface stacking mode
*
* @param  UnitIndex  @b((input))  the unit for this operation
* @param  intIfNum   @b((input))  internal interface number
* @param  mode       @b((input))  interface mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbStackingInterfaceModeSet(L7_uint32 UnitIndex,
                              L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the current status of displaying ping packet debug info
*            
* @param    none
*                      
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL usmDbPingPacketDebugTraceFlagGet();

/*********************************************************************
* @purpose  Turns on/off the displaying of ping packet debug info
*            
* @param    flag         new value of the Packet Debug flag
*                      
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbPingPacketDebugTraceFlagSet(L7_BOOL flag);

/*********************************************************************
* @purpose  Gets the Clear Config Flag.
*
* @param    none
*
* @returns
*
* @notes The clearConfig flag will be having TRUE when user clears the configuration
         and will be having FALSE when user saves the configuration to startup-config.
*
* @end
*
*********************************************************************/
extern void usmDbSysapiClearConfigFlagGet(L7_BOOL *val);

/***************************************************************************
*
* @purpose  Process a file after download is complete
*
* @param    file_type   @b{(input)} file type to update
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*

***************************************************************************/
extern L7_RC_t L7_http_process_transferred_file(L7_uint32 fileType);

/*********************************************************************
*
* @purpose Process the downloaded file
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 fileType @b((input)) The type of file downloded
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbHttpProcessTransferFile(L7_uint32 unit, L7_uint32 fileType);

/*********************************************************************
*
* @purpose Prepare the system for a HTTP file download 
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t httpTransferFilePrepare(void);
extern L7_RC_t usmDbHttpTransferFilePrepare(L7_uint32 unit);

/*********************************************************************
*
* @purpose Cleanup the system after a HTTP file download 
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/

extern L7_RC_t httpTransferFileCleanup(void);
extern L7_RC_t usmDbHttpTransferFileCleanup(L7_uint32 unit);


/*********************************************************************
*
* @purpose After a HTTP file download, rename the temporary file to
*          the destination file name. 
*
* @param L7_uint32       UnitIndex @b((input))  The unit for this operation
* @param L7_FILE_TYPES_t fileType @b((input))   The type of the file 
* @param L7_char8*       srcFile @b((input))    The file to be renamed
* @param L7_char8*       dstFile @b((input))    New name for the file
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes dstFile is only valid for L7_FILE_TYPE_CODE.
*
* @end
*
*********************************************************************/
extern L7_RC_t httpTransferFileRename(L7_FILE_TYPES_t fileType, 
                                      L7_char8 *srcFile, 
                                      L7_char8* dstFile);
extern L7_RC_t usmDbHttpTransferFileRename(L7_uint32 unit, 
                                           L7_FILE_TYPES_t fileType,
                                           L7_char8 *srcFile,
                                           L7_char8* dstFile);

extern L7_RC_t usmDbTelnetHistoryAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);
extern L7_RC_t usmDbSshHistoryAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);
extern L7_RC_t usmDbSerialHistoryAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);


extern L7_RC_t usmDbSerialHistoryBufferEnable();
extern L7_RC_t usmDbTelnetHistoryBufferEnable();
extern L7_RC_t usmDbSshHistoryBufferEnable();

extern L7_RC_t usmDbSerialHistoryBufferDisable();
extern L7_RC_t usmDbTelnetHistoryBufferDisable();
extern L7_RC_t usmDbSshHistoryBufferDisable();

extern L7_RC_t usmDbSerialHistoryBufferSizeSet(L7_uint32 UnitIndex, L7_uint32 historyBufferSize);
extern L7_RC_t usmDbTelnetHistoryBufferSizeSet(L7_uint32 UnitIndex, L7_uint32 historyBufferSize);
extern L7_RC_t usmDbSshHistoryBufferSizeSet(L7_uint32 UnitIndex, L7_uint32 historyBufferSize);


extern L7_RC_t usmDbTelnetHistoryBufferSizeGet(L7_uint32 UnitIndex, L7_uint32 *val);
extern L7_RC_t usmDbSshHistoryBufferSizeGet(L7_uint32 UnitIndex, L7_uint32 *val);
extern L7_RC_t usmDbSerialHistoryBufferSizeGet(L7_uint32 UnitIndex, L7_uint32 *val);
extern L7_RC_t usmDbSerialHistoryBufferEnableSet(L7_uint32 enable);
extern L7_RC_t usmDbSerialHistoryBufferEnableGet(L7_uint32 *enable);
extern L7_RC_t usmDbTelnetHistoryBufferEnableSet(L7_uint32 enable);
extern L7_RC_t usmDbTelnetHistoryBufferEnableGet(L7_uint32 *enable);
extern L7_RC_t usmDbSshHistoryBufferEnableSet(L7_uint32 enable);
extern L7_RC_t usmDbSshHistoryBufferEnableGet(L7_uint32 *enable);

/*********************************************************************
*
* @purpose Set the port-number for HTTP Access
*
* @param L7_uint32 unit  @b((input)) The unit for this operation
* @param L7_uint32 port  @b((input)) Port-Number
* @param L7_BOOL   flag  @b((input)) Flag to indicate if the port number
*                                    needs to be applied immediately
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSwDevCtrlWebMgmtPortNumSet(L7_uint32 unit, L7_uint32 port,L7_BOOL flag);

/*********************************************************************
*
* @purpose Set the port-number for HTTP Access
*
* @param L7_uint32 unit  @b((input)) The unit for this operation
* @param L7_uint32 port  @b((input)) Port-Number
* @param L7_BOOL   flag  @b((input)) Flag to indicate if the port number
*                                    needs to be applied immediately
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSwDevCtrlTelnetMgmtPortNumSet(L7_uint32 unit, L7_uint32 port,L7_BOOL flag);

/*********************************************************************
*
* @purpose Get Port-Number of Access for HTTP
*
* @param L7_uint32 unit  @b((input))  The unit for this operation
* @param L7_uint32 port  @b((output)) Pointer to Layer4 HTTP Port-Num
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSwDevCtrlWebMgmtPortNumGet(L7_uint32 unit, L7_uint32 *port);

/*********************************************************************
*
* @purpose Get Port-Number of Access for Telnet
*
* @param L7_uint32 unit  @b((input))  The unit for this operation
* @param L7_uint32 port  @b((output)) Pointer to Telnet Port-Num
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSwDevCtrlTelnetMgmtPortNumGet(L7_uint32 unit, L7_uint32 *port);

/*********************************************************************
*
* @purpose Get the number of bytes transferred for the file transfer
*
* @param 
*
* @returns The percentage complete value
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_uint32 usmDbTransferCompleteStatus();

/*********************************************************************
*
* @purpose Get the transfer complete status
*
* @param L7_uint32 Unit @b((input))  The unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferCompleteResultGet(L7_BOOL *val);

/*********************************************************************
*
* @purpose Set the transfer complete status
*
* @param L7_uint32 Unit @b((input))  The unit for this operation
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferCompleteResultSet(L7_BOOL val);

/*********************************************************************
*
* @purpose Gets the direction of the transfer
*
* @param L7_uint32 direction  @b((input))  Transfer status
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTransferDirectionGet(L7_uint32 *direction);

/*********************************************************************
*
* @purpose Set the username for FTP Server
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) FTP Password
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferFTPUserNameSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the username for FTP Server
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) FTP Username
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferFTPUserNameGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Set the password for FTP Server
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) FPAssword for FTP Server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferFTPPasswordSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the password for FTP Server
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) Password for FTP Server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferFTPPasswordGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
 *
 * @purpose Gets bytes transferred for the file
 *
 * @param bytes
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
void usmDbSimTransferBytesCompletedGet(L7_uint32 *bytes);

/*********************************************************************
 *
 * @purpose Sets bytes transferred for the file
 *
 * @param bytes
 *
 * @returns none
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
void usmDbSimTransferBytesCompletedSet(L7_uint32 bytes);

/*********************************************************************
*
* @purpose Set the username for FTP Server
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) FTP Password
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferFTPUserNameSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the username for FTP Server
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) FTP Username
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferFTPUserNameGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Set the password for FTP Server
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) FPAssword for FTP Server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferFTPPasswordSet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get the password for FTP Server
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uchar8 *buf      @b((output)) Password for FTP Server
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferFTPPasswordGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
*
* @purpose Get Port-Number of Access for HTTP
*
* @param L7_uint32 unit  @b((input))  The unit for this operation
* @param L7_uint32 port  @b((output)) Pointer to Layer4 HTTP Port-Num
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSwDevCtrlWebMgmtPortNumGet(L7_uint32 unit, L7_uint32 *port);

/*********************************************************************
*
* @purpose Get Port-Number of Access for Telnet
*
* @param L7_uint32 unit  @b((input))  The unit for this operation
* @param L7_uint32 port  @b((output)) Pointer to Telnet Port-Num
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSwDevCtrlTelnetMgmtPortNumGet(L7_uint32 unit, L7_uint32 *port);

/*********************************************************************
*
* @purpose Set the port-number for HTTP Access
*
* @param L7_uint32 unit  @b((input)) The unit for this operation
* @param L7_uint32 port  @b((input)) Port-Number
* @param L7_BOOL   flag  @b((input)) Flag to indicate if the port number
*                                    needs to be applied immediately
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSwDevCtrlWebMgmtPortNumSet(L7_uint32 unit, L7_uint32 port, L7_BOOL flag);

/*********************************************************************
*
* @purpose Set the port-number for Telnet Access
*
* @param L7_uint32 unit  @b((input)) The unit for this operation
* @param L7_uint32 port  @b((input)) Port-Number
* @param L7_BOOL   flag  @b((input)) Flag to indicate if the port number
*                                    needs to be applied immediately
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSwDevCtrlTelnetMgmtPortNumSet(L7_uint32 unit, L7_uint32 port, L7_BOOL flag);

/*********************************************************************
*
* @purpose Get Notification function handler.
*
* @param L7_uint32 unit  @b((input))  The unit for this operation
* @param L7_uint32 port  @b((output)) Pointer to Telnet Port-Num
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
void usmDbTransferCompletionNotifyUsers(L7_TRANSFER_STATUS_t status);

/*********************************************************************
*
* @purpose Set Notification function handler.
*
* @param L7_uint32 unit  @b((input))  The unit for this operation
* @param L7_uint32 port  @b((output)) Pointer to Telnet Port-Num
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
void usmDbTransferCompletionHandlerSet(void *notifyFuncPtr);

/*********************************************************************
 *
 * @purpose Get the status of Mgmt Access
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 *
 * @returns L7_TRUE
 * @returns L7_FALSE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_BOOL usmDbTransferSuspendMgmtAccessGet(L7_uint32 UnitIndex);

/*********************************************************************
 *
 * @purpose Set the status of Mgmt Access
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_BOOL val @b((input)) The context for the transfer
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void usmDbTransferSuspendMgmtAccessSet(L7_uint32 UnitIndex, L7_BOOL val);
/*********************************************************************
 *
 * @purpose Set the Transfer Direction as "DOWN" for HTTP
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 *
 * @notes none
 *
 * @end
 *
**********************************************************************/
L7_RC_t usmDbHttpTransferDownStartSet(L7_uint32 UnitIndex);

/**********************************************************************
 *
 * @purpose Set the Transfer Direction as "UP" for HTTP

 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 
 * @notes none
 *
 * @end
 *
***********************************************************************/
L7_RC_t usmDbHttpTransferUpStartSet(L7_uint32 UnitIndex);

/*********************************************************************
*
* @purpose After a HTTP file download, process the configuration files
* 
* 
* @param L7_char8* dstFile @b((input)) Configuration file to be copied as
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes dstFile is only valid for   L7_FILE_TYPE_CONFIG_SCRIPT,.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbHttpProcessConfigFile (L7_char8 *dstFile);

/*********************************************************************
*
* @purpose After a HTTP file download, process the configuration files
* 
* 
* @param L7_char8* dstFile @b((input)) Configuration file to be copied as
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes dstFile is only valid for   L7_FILE_TYPE_CONFIG_SCRIPT,.
*
* @end
*
*********************************************************************/
extern L7_RC_t httpProcess_config_finish (L7_char8 *dstFile);

/*********************************************************************
*
* @purpose  Trigger the Active Address Conflict Detection
*          
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIPAddrConflictDetectRun(void);

/*********************************************************************
*
* @purpose  Clear the Address Conflict Detection Status
*          
* @param    none
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIPAddrConflictDetectStatusClear(void);

/*********************************************************************
*
* @purpose  Get the Address Conflict Detection Status
*          
* @param    conflictDetectStatus   @b{(output)} conflict detection Status
*
* @returns  L7_SUCCESS
*
* @comments status would be returned as L7_TRUE if there was at least
*           one conflict detected since last reset of the status.
*           Else status is returned as L7_FALSE.
* @end
*
*********************************************************************/
L7_RC_t usmDbIPAddrConflictDetectStatusGet(L7_BOOL *conflictDetectStatus);

/*********************************************************************
*
* @purpose  Get the IP Address of the last detected address Conflict
*          
* @param    conflictIP   @b{(output)} last detected conflicting IP
*
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIPAddrConflictLastDetectIPGet(L7_uint32 *conflictIP);

/*********************************************************************
*
* @purpose  Get the MAC Address of the last detected address conflict
*          
* @param    conflictMAC   @b{(output)} MAC of last detected
*                                      conflicting host
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIPAddrConflictLastDetectMACGet(L7_uchar8 *conflictMAC);

/*********************************************************************
*
* @purpose  Get the time in seconds since the last address conflict
*           was detected
*          
* @param    conflictDetectTime   @b{(output)} time since the last
*                                             detected conflict
* @returns  L7_SUCCESS
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIPAddrConflictLastDetectTimeGet(L7_uint32 *conflictDetectTime);

/*********************************************************************
*
* @purpose  Get the Image semaphore
*
* @param    wait         wait time
*
* @returns  L7_SUCCESS/L7_FAILURE/L7_ERROR
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t usmDbImageSemaTake(L7_uint32 wait);

/*********************************************************************
*
* @purpose  Release the Image semaphore
*
* @param    None
*
* @returns  L7_SUCCESS/L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/

L7_RC_t usmDbImageSemaGive();

/*********************************************************************
* @purpose  Set the IP Address Configuration Method
*
* @param    intIfNum @b{(input)}  Internal Interface Number
* @param    method   @b{(input)}  IP Address Method of router
*                                 interface
* @param    mgmtPortType @b{(input)} Management Port Type
* @param    actImmediate @b((input)) Immediate action flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    If actImmediate is True, the setting will be applied
*           immediately without posting an event to the DHCP Client
*           task.  Use this option only when there are synchronization
*           issues, especially with SETS.
*           NOTE: Set actImmediate flag is currently supported for
*                 Release event only.  Use it cautiously and only in
*                 case of need.
*                                                   
* @end
*********************************************************************/
L7_RC_t
usmDbIntfIpAddressMethodSet (L7_uint32 intIfNum,
                             L7_INTF_IP_ADDR_METHOD_t method,
                             L7_MGMT_PORT_TYPE_t mgmtPortType,
                             L7_BOOL actImmediate);

#endif /* USMDB_SIM_API_H */
