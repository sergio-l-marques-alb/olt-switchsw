/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src\usmdb\usmdb_sim.c
 *
 * @purpose Provide interface to sim API's for unitmgr components
 *
 * @component unitmgr
 *
 * @comments none
 *
 * @create 08/28/2000
 *
 * @author bmutz
 * @end
 *
 **********************************************************************/

#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_registry_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_user_mgmt_api.h"
#include "usmdb_dot1q_api.h"
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
#include "usmdb_dot1ad.h"
#include "usmdb_metro_dot1ad_api.h"
#endif
#include "usmdb_mib_vlan_api.h"

#include "simapi.h"
#include "nimapi.h"
#include "osapi.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "sysapi.h"
#include "cli_web_mgr_api.h"
#include "cli_txt_cfg_api.h"
#include "log.h"
#include "fdb_api.h"
#include "log.h"
#include "unitmgr_api.h"
#include "sim_debug_api.h"
#include <string.h>
/* @p1261 start */
#include "sysapi.h"
#include "default_cnfgr.h"
/* @p1261 end */

#include "usmdb_dim_api.h"
#include "usmdb_tr069_api.h"
#include "bspapi.h"
#include "usmdb_util_api.h"
#include "dhcp_client_api.h"
#include "usmdb_1213_api.h"

#if L7_FEAT_DNI8541_BLADESERVER
#include "bspapi_blade.h"
#endif

#if defined(FEAT_METRO_CPE_V1_0)
extern int systemInit(int unit);
#endif

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
L7_RC_t usmDbAgentBasicConfigNetworkConfigProtocolDesiredGet(L7_uint32 UnitIndex,
    L7_uint32 *val)
{
  *val = simGetSystemConfigMode();

  return L7_SUCCESS;
}

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
L7_RC_t usmDbAgentBasicConfigNetworkConfigProtocolCurrentGet(L7_uint32 UnitIndex,
    L7_uint32 *val)
{
  *val = simGetSystemCurrentConfigMode();

  return L7_SUCCESS;
}

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
L7_RC_t usmDbAgentBasicConfigProtocolDesiredSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSetSystemConfigMode((L7_SYSCFG_MODE_t) val);

  return L7_SUCCESS;
}

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
L7_RC_t usmDbAgentBasicConfigServPortConfigProtocolDesiredGet(L7_uint32 UnitIndex,
    L7_uint32 *val)
{
  *val = simGetServPortConfigMode();

  return L7_SUCCESS;
}

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
L7_RC_t usmDbServPortAdminStateGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetServPortAdminState();

  return L7_SUCCESS;

}

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
L7_RC_t usmDbServPortAdminStateSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSetServPortAdminState(val);

  return L7_SUCCESS;
}

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
L7_RC_t usmDbServPortLinkStateGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetServPortLinkState();

  return L7_SUCCESS;
}

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
L7_RC_t usmDbAgentBasicConfigServPortConfigProtocolCurrentGet(L7_uint32 UnitIndex,
    L7_uint32 *val)
{
  *val = simGetServPortCurrentConfigMode();

  return L7_SUCCESS;
}

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
L7_RC_t usmDbAgentBasicConfigServPortProtocolDesiredSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSetServPortConfigMode((L7_SYSCFG_MODE_t) val);

  return L7_SUCCESS;
}

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
L7_RC_t usmDbSwDevCtrlBurnedInMacAddrGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simGetSystemIPBurnedInMac(buf);

  return L7_SUCCESS;

}

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
L7_RC_t usmDbSwDevCtrlLocalAdminAddrGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simGetSystemIPLocalAdminMac(buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbSwDevCtrlMacAddrTypeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetSystemIPMacType();

  return L7_SUCCESS;
}

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
L7_RC_t usmDbSwDevCtrlMacAddrTypeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSetSystemIPMacType(val);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetTransferMode();
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSetTransferMode(val);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferServerAddressTypeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSetTransferServerAddressType(val);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferServerAddressTypeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  simGetTransferServerAddressType(val);
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Retrieve the Ip Address of the Tftp Server
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uchar8 *val      @b((output)) pointer to Ip Address data
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbTransferServerAddressGet(L7_uint32 UnitIndex, L7_uchar8 *val)
{
  simGetTransferServerIp(val);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferServerAddressSet(L7_uint32 UnitIndex, L7_uchar8 *val)
{
  simSetTransferServerIp(val);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferFilePathLocalGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simGetTransferFilePathLocal(buf);
  return L7_SUCCESS;
}


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
L7_RC_t usmDbTransferFilePathLocalSet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simSetTransferFilePathLocal(buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferFileNameLocalGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simGetTransferFileNameLocal(buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferFileNameLocalSet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  if (strlen(buf) <= L7_MAX_FILENAME)
    simSetTransferFileNameLocal(buf);
  else
    return L7_FAILURE;

  return L7_SUCCESS;
}


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
L7_RC_t usmDbTransferFilePathRemoteGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simGetTransferFilePathRemote(buf);
  return L7_SUCCESS;
}


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
L7_RC_t usmDbTransferFilePathRemoteSet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simSetTransferFilePathRemote(buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferFileNameRemoteGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simGetTransferFileNameRemote(buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferFileNameRemoteSet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  if (strlen(buf) <= L7_MAX_FILENAME)
    simSetTransferFileNameRemote(buf);
  else
    return L7_FAILURE;

  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferUnitNumberGet(L7_uint32 *unit)
{
  return simGetTransferUnitNumber(unit);
}

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
L7_RC_t usmDbTransferUnitNumberSet(L7_uint32 unit)
{
  return simSetTransferUnitNumber(unit);
}

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
L7_RC_t usmDbTransferUploadFileTypeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetTransferUploadFileType();
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the transfer upload file type
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
L7_RC_t usmDbTransferUploadFileTypeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSetTransferUploadFileType(val);
  return L7_SUCCESS;
}


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
L7_RC_t usmDbTransferDownloadFileTypeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetTransferDownloadFileType();
  return L7_SUCCESS;
}


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
L7_RC_t usmDbTransferDownloadFileTypeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSetTransferDownloadFileType(val);
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Start a download transfer
 *
 * @param L7_uint32 UnitIndex @b((input)) The unit for this operation
 *
 * @returns L7_SUCCESS on success
 * @returns  L7_REQUEST_DENIED   if the specified image is the active image
 * @returns  L7_NOT_SUPPORTED    if system is not allowed to download code
 * @return   L7_ALREADY_CONFIGURED if the backup image is already activated
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbTransferDownStartSet(L7_uint32 UnitIndex)
{
  char dnldName[L7_MAX_FILENAME];
  L7_RC_t retCode;
#ifdef L7_TR069_PACKAGE
  L7_BOOL  upgradesManaged = L7_FALSE;
#endif
  /* for code files, ensure that the file can be added to the
   * list of images
   */

  if(simGetTransferDownloadFileType() == L7_FILE_TYPE_CODE)
  {
#ifdef L7_TR069_PACKAGE
  if (usmdbTr069ACSUpgradesManagedGet(&upgradesManaged) == L7_SUCCESS
                                      && upgradesManaged == L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
  }
#endif

    simGetTransferFileNameLocal(&dnldName[0]);

    retCode = usmDbImageDownloadValidate(UnitIndex, dnldName);

    if(retCode != L7_SUCCESS)
      return retCode;
  }

  /* start the transfer */
  simTransferDirectionSet(L7_TRANSFER_DIRECTION_DOWN);
  return L7_transfer_start(L7_TRANSFER_DIRECTION_DOWN);
}

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
L7_RC_t usmDbTransferUpStartSet(L7_uint32 UnitIndex)
{
  simTransferDirectionSet(L7_TRANSFER_DIRECTION_UP);
  return L7_transfer_start(L7_TRANSFER_DIRECTION_UP);
}

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
L7_RC_t usmDbTransferResultGet(L7_uint32 UnitIndex, L7_uint32 *val, L7_char8 *buf)
{
  transfer_get_status(val, buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferUploadResultCodeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  transfer_get_upload_status_code(val);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferDownloadResultCodeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  transfer_get_download_status_code(val);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferDownloadCodeStatusGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  transfer_get_download_code_status(val);
  return L7_SUCCESS;
}

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
L7_BOOL usmDbTransferInProgressGet(L7_uint32 UnitIndex)
{
  return simTransferInProgressGet();
}

/*********************************************************************
 *
 * @purpose Set the status of the transfer
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_BOOL val @b((input)) The context for the transfer
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbTransferInProgressSet(L7_uint32 UnitIndex, L7_BOOL val)
{
  return simTransferInProgressSet(val);
}

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
void* usmDbTransferContextGet(L7_uint32 UnitIndex)
{
  return simTransferContextGet();
}

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
void usmDbTransferContextSet(L7_uint32 UnitIndex, void *context)
{
  simTransferContextSet(context);
  return;
}

/* BEGIN agentIpIf Family */

/*********************************************************************
 *
 * @purpose Retrieve the Unit's System IP Address
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 *val      @b((output)) IP Address
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentIpIfAddressGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetSystemIPAddr();

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Get the configured IPv4 address on the network port
 *
 * @param   networkPortIpAddr   @b((output)) IP Address
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentConfiguredIpIfAddressGet(L7_uint32 *val)
{
  *val = simConfiguredSystemIPAddrGet();

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Retrieve the Unit's System NetMask
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 *val      @b((output)) Returns the System NetMask
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentIpIfNetMaskGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  /*  note that the declaration for this f(x) is in usmdb_ip_api.h */
  *val = simGetSystemIPNetMask();

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Retrieve the configured IPv4 network mask for the network port
 *
 * @param   netmask  @b((output)) Returns the network port NetMask
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentConfiguredIpIfNetMaskGet(L7_uint32 *netmask)
{
  *netmask = simConfiguredSystemIPNetMaskGet();

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Retrieve the configured IPv4 default gateway for the network port
 *
 * @param   netmask  @b((output)) Returns the network port NetMask
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentConfiguredIpIfDefaultRouterGet(L7_uint32 *defGw)
{
  *defGw = simConfiguredSystemIPGatewayGet();

  return L7_SUCCESS;
}


/*********************************************************************
 *
 * @purpose Retrieve the Unit's System Gateway
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 *val      @b((output)) System Gateway
 *
 * @returns L7_SUCCESS
 *
 * @notes System Gateway is 4 bytes long
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentIpIfDefaultRouterGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  /*  note that the declaration for this f(x) is in usmdb_ip_api.h */
  *val = simGetSystemIPGateway();

  return L7_SUCCESS;
}

/* @p1261 start */
/*********************************************************************
 *
 * @purpose Set the IPv4 Address and NetMask on the network port
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 ipAddr    @b((input))  IP Address
 * @param L7_uint32 NetMask   @b((input))  NetMask
 * @param L7_uint32 errorNum  @b((output)) Localized error message number 
 *                                         to display if L7_FAILURE is returned
 *
 * @returns L7_SUCCESS
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbSystemIPAndNetMaskSet(L7_uint32 UnitIndex, L7_uint32 ipAddr, 
                                   L7_uint32 netMask, L7_uint32 *errorNum)
{
  L7_RC_t rc = L7_FAILURE;

  /* check for conflicts in the system */
  if (usmDbIPConfigConflict(UnitIndex, FD_CNFGR_NIM_MIN_CPU_INTF_NUM, ipAddr, 
                            netMask, errorNum) == L7_TRUE)
    return rc;

  rc = simConfigureSystemIPAddrWithMask(ipAddr, netMask);

  if (rc != L7_SUCCESS)       /* One of the IP Set's failed */
  {
    *errorNum = 7248;         /* 7248: "Operation Failed" */
  }

  return rc;
}
/* @p1261 end */

#if defined(L7_IPV6_PACKAGE) || defined(L7_IPV6_MGMT_PACKAGE)
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
L7_RC_t usmDbAgentBasicConfigServPortIPv6ProtocolSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return simSetServPortIPv6ConfigMode(val);
}

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
L7_RC_t usmDbAgentBasicConfigServPortIPv6ConfigProtocolGet(L7_uint32 UnitIndex,
    L7_uint32 *val)
{
  *val = simGetServPortIPv6ConfigMode();

  return L7_SUCCESS;
}

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
L7_RC_t usmDbServPortIPv6AddrAutoConfigGet(L7_uint32 *val)
{
  return simServPortIPv6AddrAutoConfigGet(val);
}

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
L7_RC_t usmDbServPortIPv6AddrAutoConfigSet(L7_uint32 val)
{
  return simServPortIPv6AddrAutoConfigSet(val);
}

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
L7_RC_t usmDbAgentBasicIPv6ConfigProtocolSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return simSetSystemIPv6ConfigMode(val);
}

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
L7_RC_t usmDbAgentBasicConfigNetworkIPv6ConfigProtocolGet(L7_uint32 UnitIndex,
    L7_uint32 *val)
{
  *val = simGetSystemIPv6ConfigMode();

  return L7_SUCCESS;
}

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
L7_RC_t usmDbNetworkIPv6AddrAutoConfigGet(L7_uint32 *val)
{
  return simSystemIPv6AddrAutoConfigGet(val);
}

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
L7_RC_t usmDbNetworkIPv6AddrAutoConfigSet(L7_uint32 val)
{
  return simSystemIPv6AddrAutoConfigSet(val);
}

/**************************************************************************
 *
 * @purpose  Get IPv6 Admin Mode for the network port interface
 *
 * @param      none
 *
 * @returns    adminMode   L7_ENABLE or L7_DISABLE
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_uint32 usmDbAgentIpIfIPV6AdminModeGet( void )
{
  return simGetSystemIPV6AdminMode();
}

/**************************************************************************
 *
 * @purpose  Set IPv6 Admin Mode for the network port interface
 *
 * @param    adminMode   L7_ENABLE or L7_DISABLE
 *
 * @returns     none
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_uint32 usmDbAgentIpIfIPV6AdminModeSet( L7_uint32 adminMode )
{
  return simSetSystemIPV6AdminMode(adminMode);
}

/**************************************************************************
 *
 * @purpose  Get list of ipv6 addresses on network port interface
 *
 * @param    addrs   pointer to prefix array
 * @param    acount  point to count (in = max, out = actual)
 *
 * @returns     none
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbAgentIpIfIPV6AddrsGet( L7_in6_prefix_t *addrs, L7_uint32 *acount)
{
  return simGetSystemIPV6Addrs( addrs, acount );
}

/**************************************************************************
 * @purpose  Get next of the Unit's network port IPv6 configured prefixes.
 *
 * @param    ip6Addr        @b{(input/output)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input/output)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input/output)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    Set ip6Addr to 0's in order to get first IPv6 prefix.
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbAgentIpIfIPV6PrefixGetNext (L7_in6_addr_t *ip6Addr,
    L7_uint32 *ip6PrefixLen,
    L7_uint32 *eui_flag)
{
  return simGetNextSystemIPV6Prefix( ip6Addr, ip6PrefixLen, eui_flag );
}

/**************************************************************************
 * @purpose  Sets one of the Unit's network port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbAgentIpIfIPV6PrefixAdd (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
    L7_uint32 eui_flag)
{
  return simSetSystemIPV6Prefix(ip6Addr, ip6PrefixLen, eui_flag, L7_FALSE);
}

/**************************************************************************
 * @purpose  Removes one of the Unit's network port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbAgentIpIfIPV6PrefixRemove (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
    L7_uint32 eui_flag)
{
  return simDeleteSystemIPV6Prefix(ip6Addr, ip6PrefixLen, eui_flag);
}

/**************************************************************************
 *
 * @purpose  Get list of ipv6 default routers on network port interface
 *
 * @param    addrs   pointer to prefix array
 * @param    acount  point to count (in = max, out = actual)
 *
 * @returns     none
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbAgentIpIfIPV6DefaultRoutersGet( L7_in6_addr_t *addrs, L7_uint32 *acount)
{
  return simGetSystemIPV6DefaultRouters( addrs, acount );
}

/**************************************************************************
 *
 * @purpose  Get configured IPv6 gateway on network port interface
 *
 * @param    gateway   pointer to gateway address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbAgentIpIfIPV6GatewayGet( L7_in6_addr_t *gateway)
{
  return simGetSystemIPV6Gateway( gateway );
}

/**************************************************************************
 *
 * @purpose  Set configured IPv6 gateway on network port interface
 *
 * @param    gateway   pointer to gateway address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbAgentIpIfIPV6GatewaySet( L7_in6_addr_t *gateway)
{
  return simSetSystemIPV6Gateway( gateway, L7_FALSE );
}

/*********************************************************************
 * @purpose  get network port NDisc info from stack
 *
 * @param    ipv6NetAddress    {in/out} ipv6 neighbor address
 * @param    ipv6PhysAddrLen   {in/out} ipv6 neighbor mac length
 * @param    ipv6PhysAddress   {out} ipv6 neighbor mac
 * @param    ipv6Type          {out} ipv6 neighbor mac type
 * @param    ipv6State         {out} ipv6 neighbor state
 * @param    ipv6LastUpdated   {out} ipv6 neighbor last update
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentIpIfIPV6NdpGet( L7_in6_addr_t   *ipv6NetAddress,
    L7_uint32       *ipv6PhysAddrLen,
    L7_uchar8       *ipv6PhysAddress,
    L7_uint32       *ipv6Type,
    L7_uint32       *ipv6State,
    L7_uint32       *ipv6LastUpdated,
    L7_BOOL         *ipv6IsRtr)
{
  return simGetSystemIPV6Ndp(ipv6NetAddress,
      ipv6PhysAddrLen,
      ipv6PhysAddress,
      ipv6Type,
      ipv6State,
      ipv6LastUpdated,
      ipv6IsRtr);
}

/**************************************************************************
 *
 * @purpose  Get IPv6 Admin Mode for the service port interface
 *
 * @param      none
 *
 * @returns    adminMode   L7_ENABLE or L7_DISABLE
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_uint32 usmDbServicePortIPV6AdminModeGet( void )
{
  return simGetServPortIPV6AdminMode();
}

/**************************************************************************
 *
 * @purpose  Set IPv6 Admin Mode for the service port interface
 *
 * @param    adminMode   L7_ENABLE or L7_DISABLE
 *
 * @returns     none
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_uint32 usmDbServicePortIPV6AdminModeSet( L7_uint32 adminMode )
{
  return simSetServPortIPV6AdminMode(adminMode);
}

/**************************************************************************
 *
 * @purpose  Get list of ipv6 addresses on service port interface
 *
 * @param    addrs   pointer to prefix array
 * @param    acount  point to count (in = max, out = actual)
 *
 * @returns     none
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbServicePortIPV6AddrsGet( L7_in6_prefix_t *addrs, L7_uint32 *acount)
{
  return simGetServPortIPV6Addrs( addrs, acount );
}

/**************************************************************************
 * @purpose  Get next of the Unit's service port IPv6 configured prefixes.
 *
 * @param    ip6Addr        @b{(input/output)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input/output)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input/output)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    Set ip6Addr to 0's in order to get first IPv6 prefix.
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbServicePortIPV6PrefixGetNext (L7_in6_addr_t *ip6Addr,
    L7_uint32 *ip6PrefixLen,
    L7_uint32 *eui_flag)
{
  return simGetNextServPortIPV6Prefix( ip6Addr, ip6PrefixLen, eui_flag );
}

/**************************************************************************
 * @purpose  Sets one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbServicePortIPV6PrefixAdd (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
    L7_uint32 eui_flag)
{
  return simSetServPortIPV6Prefix(ip6Addr, ip6PrefixLen, eui_flag, L7_FALSE);
}

/**************************************************************************
 * @purpose  Removes one of the Unit's service port IPv6 prefixes.
 *
 * @param    ip6Addr        @b{(input)}   IPv6 prefix address
 * @param    ip6PrefixLen   @b{(input)}   IPv6 prefix mask length
 * @param    eui_flag       @b{(input)}   EUI-64 address format indicator
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *************************************************************************/
L7_RC_t usmDbServicePortIPV6PrefixRemove (L7_in6_addr_t *ip6Addr, L7_uint32 ip6PrefixLen,
    L7_uint32 eui_flag)
{
  return simDeleteServPortIPV6Prefix(ip6Addr, ip6PrefixLen, eui_flag);
}

/**************************************************************************
 *
 * @purpose  Get list of ipv6 default routers on service port interface
 *
 * @param    addrs   pointer to prefix array
 * @param    acount  point to count (in = max, out = actual)
 *
 * @returns     none
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbServicePortIPV6DefaultRoutersGet( L7_in6_addr_t *addrs, L7_uint32 *acount)
{
  return simGetServPortIPV6DefaultRouters( addrs, acount );
}

/**************************************************************************
 *
 * @purpose  Get configured IPv6 gateway on service port interface
 *
 * @param    gateway   pointer to gateway address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbServicePortIPV6GatewayGet( L7_in6_addr_t *gateway)
{
  return simGetServPortIPV6Gateway( gateway );
}

/**************************************************************************
 *
 * @purpose  Set configured IPv6 gateway on service port interface
 *
 * @param    gateway   pointer to gateway address
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments    none.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbServicePortIPV6GatewaySet( L7_in6_addr_t *gateway)
{
  return simSetServPortIPV6Gateway( gateway, L7_FALSE );
}

/**********************************************************************
 * @purpose  get service port NDisc info from stack
 *
 * @param    ipv6NetAddress    {in/out} ipv6 neighbor address
 * @param    ipv6PhysAddrLen   {in/out} ipv6 neighbor mac length
 * @param    ipv6PhysAddress   {out} ipv6 neighbor mac
 * @param    ipv6Type          {out} ipv6 neighbor mac type
 * @param    ipv6State         {out} ipv6 neighbor state
 * @param    ipv6LastUpdated   {out} ipv6 neighbor last update
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortIPV6NdpGet( L7_in6_addr_t   *ipv6NetAddress,
    L7_uint32       *ipv6PhysAddrLen,
    L7_uchar8       *ipv6PhysAddress,
    L7_uint32       *ipv6Type,
    L7_uint32       *ipv6State,
    L7_uint32       *ipv6LastUpdated,
    L7_BOOL         *ipv6IsRtr)
{
  return simGetServPortIPV6Ndp(ipv6NetAddress,
      ipv6PhysAddrLen,
      ipv6PhysAddress,
      ipv6Type,
      ipv6State,
      ipv6LastUpdated,
      ipv6IsRtr);
}

#endif /* L7_IPV6_PACKAGE || L7_IPV6_MGMT_PACKAGE */

/*********************************************************************
 *
 * @purpose Set the Unit's System IP Address
 *
 * @param L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param L7_uint32 ipAddr1   @b((input)) IP Address
 *
 * @returns L7_SUCCESS
 *
 * @notes IP Address is 4 bytes long
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentIpIfAddressSet(L7_uint32 UnitIndex, L7_uint32 ipAddr1)
{
  simSystemIPAddrConfigure(ipAddr1);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the Unit's System NetMask
 *
 * @param L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param L7_uint32 val       @b((input)) System NetMask
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentIpIfNetMaskSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSystemIPNetMaskConfigure(val);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the Unit's System Gateway
 *
 * @param L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param L7_uint32 val       @b((input)) System Gateway
 *
 * @returns L7_SUCCESS
 *
 * @notes System Gateway is 4 bytes long
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentIpIfDefaultRouterSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simSystemIPGatewayConfigure(val);

  return L7_SUCCESS;
}
/* END agentIpIf Family */

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
L7_RC_t usmDbCommandPromptSet(L7_uint32 UnitIndex, L7_char8 *prompt)
{
  if (strlen(prompt) > L7_PROMPT_SIZE-1)
  {
    return L7_FAILURE;
  }
  else
  {
    cliWebSetSystemCommandPrompt(prompt);
    return usmDb1213SysNameSet(UnitIndex, prompt);
  }
}

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
L7_RC_t usmDbCommandPromptGet(L7_uint32 UnitIndex, L7_char8 *prompt)
{
  cliWebGetSystemCommandPrompt(prompt);

  return L7_SUCCESS;
}

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
L7_RC_t usmDbAgentBasicConfigSerialBaudRateSet(L7_uint32 UnitIndex, L7_uint32 baudRate)
{
  L7_RC_t rc = L7_FAILURE;
  rc = simSetSerialBaudRate(baudRate);
  return rc;
}

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
L7_RC_t usmDbAgentBasicConfigSerialBaudRateGet(L7_uint32 UnitIndex, L7_uint32 *baudRate)
{
  *baudRate = simGetSerialBaudRate();
  return L7_SUCCESS;
}

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
L7_RC_t usmDbSerialTimeOutSet(L7_uint32 UnitIndex, L7_uint32 timeOut)
{
  simSetSerialTimeOut(timeOut);

  return L7_SUCCESS;
}

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
L7_RC_t usmDbSerialTimeOutGet(L7_uint32 UnitIndex, L7_uint32 *timeOut)
{
  *timeOut = simGetSerialTimeOut();
  return L7_SUCCESS;
}

/*********************************************************************
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
L7_RC_t usmDbSerialPortParmGet(L7_uint32 UnitIndex, L7_uint32 parm, L7_uint32 *result)
{
  L7_RC_t rc = bspapiSerialPortParmGet(parm, result);
  return rc;
}

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
L7_RC_t usmDbAgentMibCapabilityDescrGet(L7_uint32 UnitIndex, L7_uint32 Index, L7_char8 *buf)
{
  L7_RC_t rc;
  rc = simMibDescription(Index, buf);
  return rc;
}

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
L7_RC_t usmDbSwDevCtrlSaveConfigurationGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = 1;
  return L7_SUCCESS;
}

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
L7_RC_t usmDbSwDevCtrlWebMgmtModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = cliWebGetSystemWebMode();
  return L7_SUCCESS;
}


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
  L7_RC_t
usmDbCliWebHttpNumSessionsSet(L7_uint32 val)
{
  return cliWebHttpNumSessionsSet(val);
}

/*********************************************************************
 * @purpose  Get the maximum number of web sessions
 *
 * @returns  Return the maximum number of web sessions
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
usmDbCliWebHttpNumSessionsGet(L7_uint32 *val)
{
  return cliWebHttpNumSessionsGet(val);
}

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
  L7_RC_t
usmDbCliWebHttpSessionHardTimeOutSet(L7_uint32 val)
{
  return cliWebHttpSessionHardTimeOutSet(val);
}

/*********************************************************************
 * @purpose  Get the http session hard timeout (in hours)
 *
 * @returns  Return the http session hard timeout
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
usmDbCliWebHttpSessionHardTimeOutGet(L7_uint32 *val)
{
  return cliWebHttpSessionHardTimeOutGet(val);
}

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
  L7_RC_t
usmDbCliWebHttpSessionSoftTimeOutSet(L7_uint32 val)
{
  return cliWebHttpSessionSoftTimeOutSet(val);
}

/*********************************************************************
 * @purpose  Get the http session soft timeout (in minutes)
 *
 * @returns  Return the http session soft timeout
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
  L7_RC_t
usmDbCliWebHttpSessionSoftTimeOutGet(L7_uint32 *val)
{
  return cliWebHttpSessionSoftTimeOutGet(val);
}

/*********************************************************************
 * @purpose  Set Unit's System Rate control
 *
 * @param    L7_uint32 UnitIndex  @b((input)) The unit for this operation
 * @param    L7_uint32 val        @b((input)) Rate control
 *
 * @returns  L7_SUCCESS   if success
 * @returns  L7_FAILURE   if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbSwDevCtrlRateControlSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  /* read only, ignore*/
  return L7_SUCCESS;
}

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
L7_RC_t usmDbSwDevCtrlSaveConfigurationSet(L7_uint32 UnitIndex, L7_SAVE_CONFIG_TYPE_t val)
{
  L7_RC_t rc;

  rc = nvStoreSave(val);

  if(usmDbComponentPresentCheck(UnitIndex,L7_TRAPMGR_COMPONENT_ID))
  {
    if ((rc == L7_SUCCESS) && (val == L7_SAVE_CONFIG_TYPE_STARTUP))
    {
     rc = usmDbTrapMgrConfigChangedTrapSend("","");
    }
  }
  return rc;
}

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
void usmDbSwDevCtrlComponentConfigAllSave(void)
{
  L7_uint32     i;

  for (i = 0; i < L7_LAST_COMPONENT_ID; i++)
  {
    nvStoreComponentSave(i);
  }
}

/*********************************************************************
 * @purpose  Resets the system
 *
 * @param    L7_uint32 UnitIndex  @b((input)) The unit for this operation
 * @param    L7_uint32 val        @b((input)) (No-op)
 *
 * @returns  L7_SUCCESS   if success
 * @returns  L7_FAILURE   if failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbSwDevCtrlResetSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_LOG_EVENT(0);
#if defined(FEAT_METRO_CPE_V1_0)
  /* Initialize ASIC to defaults so no traffic flows while system is reloaded,
   * This is a temporary fix, TODO-Need a cleaner way to do it. */
  systemInit(0);
#endif
#if L7_FEAT_DNI8541_BLADESERVER
  bspapiIomFaultSet();
#endif
  bspapiSwitchReset();
  osapiSleep(30);
  return L7_FAILURE; /* If we get here, I guess that's failure... */
}

/*********************************************************************
 * @purpose  Sets the Unit's System Locally Administered Mac Address
 *
 * @param    L7_uint32 UnitIndex  @b((input)) The unit for this operation
 * @param    L7_char8  *buf       @b((input)) Pointer to system local admin mac address
 *
 * @returns  L7_SUCCESS
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbSwDevCtrlLocalAdminAddrSet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simSetSystemIPLocalAdminMac(buf);
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  Sets the Unit's System Web Mode
 *
 * @param    L7_uint32 UnitIndex  @b((input)) The unit for this operation
 * @param    L7_uint32 val        @b((input)) System Web Mode (L7_ENABLE or L7_DISABLE)
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbSwDevCtrlWebMgmtModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{

  cliWebSetSystemWebMode(val);

  return L7_SUCCESS;
}

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
L7_RC_t usmDbResetConfigActionSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  L7_RC_t rc;
  L7_CNFGR_CMD_DATA_t cmdData,
              *pCmdData = &cmdData;

  /* Invalidate the run-time configuration.
   */
  sysapiCfgFileInvalidate ();

  cnfgrApiSystemStartupReasonSet(L7_STARTUP_AUTO_COLD);    /* will initiate a cold start */

  /* Initiate the unconfiguration cycle.
   */
  pCmdData->cbHandle         = L7_CNFGR_NO_HANDLE;
  pCmdData->command          = L7_CNFGR_CMD_UNCONFIGURE;
  pCmdData->correlator       = L7_NULL;
  pCmdData->type             = L7_CNFGR_EVNT;
  pCmdData->u.evntData.event = L7_CNFGR_EVNT_U_START;
  pCmdData->u.evntData.data  = L7_LAST_COMPONENT_ID;

  rc = cnfgrApiCommand(pCmdData);
  if (rc != L7_SUCCESS)
  {
    L7_LOG_ERROR (rc);
  }
 
  sysapiClearConfigFlagSet(L7_TRUE);


  /* The system will come back up without further intervention.
   */
  /* After clear configuration we should not apply the configuration. That's why we have set the following flag */
#ifdef CLI_WEB_PRESENT
  cliGlobalConfigurationSkipSet(L7_TRUE);
#endif
  return L7_SUCCESS;
}

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
void usmDbSysapiClearConfigFlagGet(L7_BOOL *val)
{
  sysapiClearConfigFlagGet(val);
}

/*********************************************************************
 * @purpose  Check if user config data has changed
 *
 *
 * @param    L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param    *hasChanged  @b((output)) Pointer to changed flag (L7_TRUE/L7_FALSE)
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbUnsavedConfigCheck(L7_uint32 UnitIndex, L7_BOOL *hasChanged)
{
  *hasChanged = nvStoreHasDataChanged();
  return  L7_SUCCESS;
}


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
L7_RC_t usmDbTimeSinceLastSwitchStatsResetGet(L7_uint32 UnitIndex, usmDbTimeSpec_t *ts)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 interface;

  if (usmDbMgtSwitchintIfNumGet(UnitIndex, &interface) == L7_SUCCESS) {
    rc = usmDbTimeSinceLastStatsResetGet(UnitIndex, interface, ts);
  }

  return rc;
}

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
L7_RC_t usmDbArpInfoGetNext(L7_uint32 UnitIndex, L7_long32 *index, L7_ulong32 *ipAddr, L7_uchar8 *mac)
{
  L7_uchar8 nullMac[L7_MAC_ADDR_LEN];
  L7_RC_t rc = osapiM2IpArpTblEntryGet( index, ipAddr, mac );
  
  memset (nullMac, 0, sizeof(nullMac));

  while ((rc == L7_SUCCESS) && (memcmp(nullMac, mac, L7_MAC_ADDR_LEN) == 0)) 
  {
    rc = osapiM2IpArpTblEntryGet(index, ipAddr, mac);
  }
  return rc;
}

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
L7_RC_t usmDbArpEntryGet(L7_uint32 UnitIndex, L7_char8 *mac, L7_uint32 *intIfNum)
{
  if (fdbMacToIntfGet(mac, intIfNum) == L7_SUCCESS)
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

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
*********************************************************************/
L7_RC_t usmDbArpSwitchClear(L7_uint32 UnitIndex) 
{
  return simArpSwitchClear(); 
}

/*********************************************************************
 *
 * @purpose Set the service port gateway
 *
 * @param   L7_uint32 UnitIndex  @b((input)) The unit for this operation
 * @param   L7_uint32 val        @b((input)) Service port gateway
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortGatewaySet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simServPortIPGatewayConfigure(val);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Retrieve the service port Gateway
 *
 * @param   L7_uint32 UnitIndex  @b((input))  The unit for this operation
 * @param   L7_uint32 *val       @b((output)) Service port Gateway
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortGatewayGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetServPortIPGateway();

  return L7_SUCCESS;
}

/* @p1261 start */
/*********************************************************************
 *
 * @purpose Set the Unit's Service Port IP Address and NetMask
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 ipAddr    @b((input))  IP Address
 * @param L7_uint32 netMask   @b((input))  NetMask
 * @param L7_uint32 errorNum  @b((output)) Localized error message number to display
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes IP Address is 4 bytes long
 * @notes if netmask set fails, will attempt to set IP address to old value
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortIPAndNetMaskSet(L7_uint32 UnitIndex, L7_uint32 ipAddr, 
                                        L7_uint32 netMask, L7_uint32 *errorNum)
{
  L7_RC_t rc = L7_FAILURE;

  /* check for conflicts in the system */
  if (usmDbIPConfigConflict(UnitIndex, FD_CNFGR_NIM_MIN_SERV_PORT_INTF_NUM, ipAddr, 
                            netMask, errorNum) == L7_TRUE)
    return rc;

  rc = simConfigureServPortIPAddrWithMask(ipAddr, netMask);

  if (rc != L7_SUCCESS)                                        /* One of the IP Set's failed */
  {
    *errorNum = 7248;                                           /* 7248: "Operation Failed" */
  }

  return rc;
}
/* @p1261 end */

/*********************************************************************
 *
 * @purpose Configure an IPv4 address on the service port.
 *
 * @param L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param L7_uint32 val       @b((input)) IP Address
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortIPAddrSet(L7_uint32 UnitIndex, L7_uint32 ipAddr1)
{
  simServPortIPAddrConfigure(ipAddr1);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Retrieve the active IPv4 address on the service port
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 *val      @b((output)) Service port IP Addr
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortIPAddrGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetServPortIPAddr();

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Get the configured IPv4 address on the service port
 *
 * @param   networkPortIpAddr   @b((output)) IP Address
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortConfiguredIpAddrGet(L7_uint32 *val)
{
  *val = simConfiguredServPortIPAddrGet();

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Configure a network mask for the IPv4 address on the service port.
 *
 * @param L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param L7_uint32 val       @b((input)) Service port NetMask
 *
 * @returns L7_SUCCESS
 *
 * @notes  
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortNetMaskSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  simServPortIPNetMaskConfigure(val);

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Retrieve the network mask for the IPv4 address on the service port
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 *val      @b((output)) Service port NetMask
 *
 * @returns L7_SUCCESS
 *
 * @notes  
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortNetMaskGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simGetServPortIPNetMask();
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Retrieve the configured IPv4 network mask for the service port
 *
 * @param   netmask  @b((output)) Returns the service port network mask
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortConfiguredNetMaskGet(L7_uint32 *netmask)
{
  *netmask = simConfiguredServPortIPNetMaskGet();

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose Retrieve the configured IPv4 default gateway for the service port
 *
 * @param   defGw  @b((output)) IPv4 address of the default gateway
 *
 * @returns L7_SUCCESS
 *
 * @notes 
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbServicePortConfiguredGatewayGet(L7_uint32 *defGw)
{
  *defGw = simConfiguredServPortIPGatewayGet();

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Retrieve Access mode
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 index     @b((input))  Index value
 * @param L7_uint32 val       @b((output)) Snmp community access level
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAccessModeGet(L7_uint32 UnitIndex, L7_uint32 index, L7_uint32 *val)
{
  if (index == 0)
    *val = L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_WRITE;
  else
    *val = L7_SNMP_COMMUNITY_ACCESS_LEVEL_READ_ONLY;
  return L7_SUCCESS;
}

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
L7_RC_t usmDbWebJavaModeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = cliWebGetJavaWebMode();
  return L7_SUCCESS;
}


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
L7_RC_t usmDbWebJavaModeSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return cliWebSetJavaWebMode(val);
}


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
L7_RC_t usmDbAgentTelnetTimeoutGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = cliWebGetSystemTelnetTimeout();
  return L7_SUCCESS;
}


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
L7_RC_t usmDbAgentTelnetNumSessionsGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = cliWebGetSystemTelnetNumSessions();
  return L7_SUCCESS;
}


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
L7_RC_t usmDbAgentTelnetNewSessionsGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = cliWebGetSystemTelnetNewSessions();
  return L7_SUCCESS;
}


/*********************************************************************
 *
 * @purpose Set the number of new telnet sessions
 *
 * @param L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param L7_uint32 *val      @b((input)) Number of new sessions
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes  none
 *
 * @end
 *********************************************************************/
L7_RC_t usmDbAgentTelnetNewSessionsSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return cliWebSetSystemTelnetNewSessions(val);
}


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
L7_RC_t usmDbAgentTelnetNumSessionsSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return cliWebSetSystemTelnetNumSessions(val);
}


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
L7_RC_t usmDbAgentTelnetTimeoutSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return cliWebSetSystemTelnetTimeout(val);
}

/*********************************************************************
 * @purpose  ping operation
 *
 *
 * @param    L7_uint32 UnitIndex @b((input)) The unit for this operation
 * @param    L7_char8  *ipHost @b((input)) Pointer to the hosts IP address
 * @param    L7_uint32 count   @b((input)) Number of times to issue a ping request
 *
 * @returns  Number of packets echoed
 *
 * @notes    none
 *
 * @end
 *
 *********************************************************************/
L7_uint32 usmDBPing(L7_uint32 UnitIndex, L7_char8 *ipHost, L7_uint32 count)
{
  return sysapiPing(ipHost, count);
}

/*********************************************************************
 * @purpose  Gets the service port burned-in mac address
 *
 *
 * @param    L7_uint32 UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbServPortMacAddressGet(L7_uint32 UnitIndex, L7_uchar8 *macAddr)
{
  simGetServicePortBurnedInMac(macAddr);
  return L7_SUCCESS;
}


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
L7_RC_t usmDbPortsPerBoxGet(L7_uint32 UnitIndex, L7_uint32 *maxNumPorts)
{
  L7_uint32 i;
  L7_uint32 slotPop[L7_MAX_SLOTS_PER_UNIT];
  L7_uint32 numPorts;
  *maxNumPorts = 0;

  usmDbSlotsPopulatedGet(UnitIndex, slotPop);
  for (i=0; i<L7_MAX_SLOTS_PER_UNIT; i++)
  {
    if (slotPop[i] == L7_TRUE)
    {
      if (usmDbPortsPerSlotGet(UnitIndex, i, &numPorts) == L7_SUCCESS)
      {
    *maxNumPorts += numPorts;
      }
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Get the management VLAN ID
 *
 * @param  L7_uint32 UnitIndex    @b((input))  The unit for this operation
 * @param  L7_uint32 *val         @b((output)) vlan Id
 *
 * @returns  L7_SUCCCESS
 * @returns  L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbMgmtVlanIdGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simMgmtVlanIdGet();
  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Set the management VLAN ID
 *
 * @param  L7_uint32 UnitIndex    @b((input))  The unit for this operation
 * @param  L7_uint32 val          @b((input))  Vlan Id
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *          L7_ALREADY_CONFIGURED if the VLAN ID is already in use internally
 *                                 (for example, for a port-based routing interface)
 *
 * @notes Assumes valid VLAN ID
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbMgmtVlanIdSet(L7_uint32 UnitIndex, L7_uint32 val)
{
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE) && defined(FEAT_METRO_CPE_V1_0)
  L7_RC_t    rc;
  L7_uint32  intfType, intIfNum, oldVlan;
  L7_BOOL    flag = L7_FALSE;
  L7_uint32  type;

  rc = usmDbVlanIDGet(UnitIndex, val);

  if (rc == L7_SUCCESS)
  {
    /* Check if same mgmt vlan is being set */
    if ((oldVlan = simMgmtVlanIdGet()) == val)
    {
      return L7_SUCCESS;
    }

    /* Check if any service exists with this VLAN ID */
    if (usmDbDot1adServiceVidIsConfigured(val, &flag) == L7_SUCCESS && flag == L7_TRUE)
    {
      return L7_FAILURE;
    }

    rc = usmDbValidIntIfNumFirstGet(&intIfNum);
    while (rc == L7_SUCCESS)
    {
      if(usmDbDot1adInterfaceTypeGet(UnitIndex,intIfNum, &intfType) == L7_SUCCESS)
      {
        /* Include NNI ports in the mgmt vlan */
        if (intfType == DOT1AD_INTFERFACE_TYPE_NNI)
        {
          type = L7_DOT1Q_FIXED;
          /* Set PVID of all interfaces to mgmt VLAN */
          if (usmDbQportsPVIDSet(UnitIndex, intIfNum, val) != L7_SUCCESS)
          {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USMDB_COMPONENT_ID,
                    "usmDbMgmtVlanIdSet: Failed to set pvid of intf %s to vlan %d", ifName, val);
          }
        } /* Exclude UNI port from mgmt VLAN */
        else
        {
          type = L7_DOT1Q_FORBIDDEN;
        }

        if (usmDbVlanMemberSet(UnitIndex, val, intIfNum, type, DOT1Q_SWPORT_MODE_GENERAL)  != L7_SUCCESS)
        {
          L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
          nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USMDB_COMPONENT_ID,
                  "usmDbMgmtVlanIdSet: Failed to set membership of intf %s to vlan %d", ifName, val);
        }

        if (usmDbVlanMemberSet(UnitIndex, oldVlan, intIfNum, L7_DOT1Q_NORMAL_REGISTRATION, DOT1Q_SWPORT_MODE_GENERAL) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_USMDB_COMPONENT_ID,
                  "usmDbMgmtVlanIdSet: Failed to remove membership of intf %s to vlan %d", ifName, oldVlan);
        }

      }
      rc = usmDbValidIntIfNumNext(intIfNum, &intIfNum);
    }

    /* Modify the mgmtvlan */
    rc = simMgmtVlanIdSet(val);

    return rc;
  }
  return L7_FAILURE;
#else
  return (simMgmtVlanIdSet(val));
#endif
}

/*********************************************************************
 *
 * @purpose Get the management Port
 *
 * @param  L7_uint32 UnitIndex    @b((input))  The unit for this operation
 * @param  L7_uint32 *val         @b((output)) Internal interface
 *
 * @returns  L7_SUCCCESS
 * @returns  L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbMgmtPortGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = simMgmtPortGet();
  return L7_SUCCESS;
}

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
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbMgmtPortSet(L7_uint32 UnitIndex, L7_uint32 val)
{
  return (simMgmtPortSet(val));
}

/* @p1261 start */
/*********************************************************************
 * @purpose  checks IP Address and NetMask for subnet conflicts on the system
 *
 * @param    L7_uint32 UnitIndex     @b((input)) The unit for this operation
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
L7_BOOL usmDbIPConfigConflict(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 ifIpAddress, L7_uint32 ifNetMask, L7_uint32 *errorMsg)
{
  return sysapiIPConfigConflict(interface, ifIpAddress, ifNetMask, errorMsg);
}

/*********************************************************************
 * @purpose  Finds first valid IP address on this system
 *
 * @param    ipAddress    local address
 *
 * @notes    returns 0.0.0.0 if no valid IP addresses are found
 *
 * @end
 *********************************************************************/
void usmDbIPFirstLocalAddress(L7_uint32 *ipAddress)
{
  return sysapiIPFirstLocalAddress(ipAddress);
}

/*********************************************************************
 *
 * @purpose Retrieve the status of a SNMP save config request
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 * @param L7_uint32 *val      @b((output)) Enum L7_SAVE_CONFIG_CODE_t
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t usmDbSaveConfigResultCodeGet(L7_uint32 UnitIndex, L7_uint32 *val)
{
  *val = sysapiWriteConfigToFlashStatus();
  return L7_SUCCESS;
}

/* @p1261 end */

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
L7_uint32 usmDbThisUnitGet(void)
{
  return simGetThisUnit();
}

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
L7_RC_t usmDbStackingInterfaceModeSet(L7_uint32 UnitIndex,
    L7_uint32 intIfNum, L7_uint32 mode)
{
  /* QSCAN_RJ: DEFINITION NEEDED !!! */

  return L7_SUCCESS;
}

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
L7_BOOL usmDbPingPacketDebugTraceFlagGet()
{
  return pingDebugPacketTraceFlagGet();
}

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
L7_RC_t usmDbPingPacketDebugTraceFlagSet(L7_BOOL flag)
{
  return pingDebugPacketTraceFlagSet(flag);
}
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
L7_RC_t usmDbHttpProcessTransferFile(L7_uint32 unit, L7_uint32 fileType)
{
  L7_RC_t rc = L7_SUCCESS;

  if(L7_http_process_transferred_file(fileType) == L7_FAILURE)
   rc = L7_FAILURE;

  return rc;
}

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
L7_RC_t usmDbHttpTransferFilePrepare(L7_uint32 unit)
{
  return httpTransferFilePrepare();
}

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
L7_RC_t usmDbHttpTransferFileCleanup(L7_uint32 unit)
{
  return httpTransferFileCleanup();
}
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
L7_RC_t usmDbHttpTransferFileRename(L7_uint32 unit,
                                    L7_FILE_TYPES_t fileType,
                                    L7_char8 *srcFile,
                                    L7_char8 *dstFile)
{
  return httpTransferFileRename(fileType, srcFile, dstFile);
}


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
L7_RC_t usmDbHttpProcessConfigFile (L7_char8 *dstFile)
{
  return httpProcess_config_finish(dstFile);
}


/*********************************************************************
*
* @purpose Get the transfer complete percentage
*
* @param L7_uint32 Unit @b((input))  The unit for this operation
*
* @returns The percentage complete value
*
* @notes none
*
* @end
*
*********************************************************************/
L7_uint32 usmDbTransferCompleteStatus()
{
  L7_uint32 val = 0;

  simTransferBytesCompletedGet(&val);

  return val;
}

/*********************************************************************
*
* @purpose Get the transfer complete status
*
* @param L7_uint32 Unit @b((input))  The unit for this operation
* @param L7_uint32 *val @b((output))
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferCompleteResultGet(L7_BOOL *val)
{
  simTransferCompletionStatusGet(val);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the transfer complete status
*
* @param L7_uint32 unit @b((input))  The unit for this operation
* @param L7_uint32 val  @b((input))  Transfer status
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTransferCompleteResultSet(L7_BOOL val)
{
  simTransferCompletionStatusSet(val);

  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferDirectionGet(L7_uint32 *direction)
{
  simTransferDirectionGet(direction);

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Set the history buffer size for the Serial Interface
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t usmDbSerialHistoryBufferSizeSet(L7_uint32 UnitIndex, L7_uint32 historyBufferSize)
{
  return simSerialHistoryBufferSizeSet(historyBufferSize);
}

/*********************************************************************
*
* @purpose Get history buffer size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSerialHistoryBufferSizeGet(L7_uint32 UnitIndex, L7_uint32 *bufferSize)
{
  return simSerialHistoryBufferSizeGet(bufferSize);
}

/*********************************************************************
*
* @purpose  serial history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSerialHistoryBufferEnableSet(L7_uint32 enable)
{
  return simSerialHistoryBufferEnableSet(enable);
}

/*********************************************************************
*
* @purpose serial history buffer
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSerialHistoryBufferEnableGet(L7_uint32 *enable)
{
  return simSerialHistoryBufferEnableGet(enable);
}

/*********************************************************************
*
* @purpose Set the history buffer size for telnet
*
* @param L7_uint32 UnitIndex @b((input))  The unit for this operation
* @param L7_uint32 historyBufferSize
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*********************************************************************/
L7_RC_t usmDbTelnetHistoryBufferSizeSet(L7_uint32 UnitIndex, L7_uint32 historyBufferSize)
{
  return simTelnetHistoryBufferSizeSet(historyBufferSize);
}

/*********************************************************************
*
* @purpose Get history buffer size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbTelnetHistoryBufferSizeGet(L7_uint32 UnitIndex, L7_uint32 *bufferSize)
{
  return simTelnetHistoryBufferSizeGet(bufferSize);
}

/*********************************************************************
*
* @purpose set history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbTelnetHistoryBufferEnableSet(L7_uint32 enable)
{
  return simTelnetHistoryBufferEnableSet(enable);
}

/*********************************************************************
*
* @purpose get history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbTelnetHistoryBufferEnableGet(L7_uint32 *enable)
{
  return simTelnetHistoryBufferEnableGet(enable);
}

/*********************************************************************
*
* @purpose Set the Timeout for the Serial Interface
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
*
* @end
*********************************************************************/

L7_RC_t usmDbSshHistoryBufferSizeSet(L7_uint32 UnitIndex, L7_uint32 historyBufferSize)
{
  return simSshHistoryBufferSizeSet(historyBufferSize);
}

/*********************************************************************
*
* @purpose Get history buffer size value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSshHistoryBufferSizeGet(L7_uint32 UnitIndex, L7_uint32 *bufferSize)
{
  return simSshHistoryBufferSizeGet(bufferSize);
}

/*********************************************************************
*
* @purpose set history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSshHistoryBufferEnableSet(L7_uint32 enable)
{
  return simSshHistoryBufferEnableSet(enable);
}

/*********************************************************************
*
* @purpose get history buffer enable
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes  none
*
* @end
*********************************************************************/
L7_RC_t usmDbSshHistoryBufferEnableGet(L7_uint32 *enable)
{
  return simSshHistoryBufferEnableGet(enable);
}

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
void usmDbSimTransferBytesCompletedGet(L7_uint32 *bytes)
{
  simTransferBytesCompletedGet(bytes);
}

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
void usmDbSimTransferBytesCompletedSet(L7_uint32 bytes)
{
  simTransferBytesCompletedSet(bytes);
}

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
L7_RC_t usmDbTransferFTPUserNameSet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simSetTransferFTPUserName(buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferFTPUserNameGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simGetTransferFTPUserName(buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferFTPPasswordSet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simSetTransferFTPPassword(buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbTransferFTPPasswordGet(L7_uint32 UnitIndex, L7_char8 *buf)
{
  simGetTransferFTPPassword(buf);
  return L7_SUCCESS;
}

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
L7_RC_t usmDbSwDevCtrlWebMgmtPortNumSet(L7_uint32 unit, L7_uint32 port, L7_BOOL flag)
{
  return cliWebHttpPortSet(port, flag);
}

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
* @notes  apply doesn't have any effect right now. It is as good as L7_FALSE.
*         But it can be enhanced on the similar lines as
*         usmDbSwDevCtrlWebMgmtPortNumSet
*
* @end
*********************************************************************/
L7_RC_t usmDbSwDevCtrlTelnetMgmtPortNumSet(L7_uint32 unit, L7_uint32 port, L7_BOOL flag)
{
  return cliWebTelnetPortSet(port, flag);
}

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
L7_RC_t usmDbSwDevCtrlWebMgmtPortNumGet(L7_uint32 unit, L7_uint32 *port)
{
  return  cliWebHttpPortGet(port);
}

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
L7_RC_t usmDbSwDevCtrlTelnetMgmtPortNumGet(L7_uint32 unit, L7_uint32 *port)
{
  return  cliWebTelnetPortGet(port);
}

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
void usmDbTransferCompletionNotifyUsers(L7_TRANSFER_STATUS_t status)
{
  simTransferCompleteNotification(status);
}

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
void usmDbTransferCompletionHandlerSet(void *notifyFuncPtr)
{
  simSetTransferCompleteNotification(notifyFuncPtr);
  return;
}

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
L7_BOOL usmDbTransferSuspendMgmtAccessGet(L7_uint32 UnitIndex)
{
  return simTransferMgmtAccessGet();
}

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
void usmDbTransferSuspendMgmtAccessSet(L7_uint32 UnitIndex, L7_BOOL val)
{
  simTransferMgmtAccessSet(val);
  return;
}
/****************************************************************
 *
 * @purpose Set the Transfer Direction as "Down" for HTTP transfer
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 *
 * @returns L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *
*****************************************************************/
L7_RC_t usmDbHttpTransferDownStartSet(L7_uint32 UnitIndex)
{
   simTransferDirectionSet(L7_TRANSFER_DIRECTION_DOWN);
   return L7_SUCCESS;
}


/****************************************************************
 *
 * @purpose Set the Transfer Direction as "UP" for HTTP transfer
 *
 * @param L7_uint32 UnitIndex @b((input))  The unit for this operation
 *
 * @returns L7_SUCCESS
 *
 * @notes none
 *
 * @end
 *
****************************************************************/
L7_RC_t usmDbHttpTransferUpStartSet(L7_uint32 UnitIndex)
{
   simTransferDirectionSet(L7_TRANSFER_DIRECTION_UP);
   return L7_SUCCESS;
}

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
L7_RC_t usmDbIPAddrConflictDetectRun(void)
{
  return simIPAddrConflictDetectRun();
}

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
L7_RC_t usmDbIPAddrConflictDetectStatusClear(void)
{
  return simIPAddrConflictDetectStatusClear();
}

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
L7_RC_t usmDbIPAddrConflictDetectStatusGet(L7_BOOL *conflictDetectStatus)
{
  return simIPAddrConflictDetectStatusGet(conflictDetectStatus);
}

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
L7_RC_t usmDbIPAddrConflictLastDetectIPGet(L7_uint32 *conflictIP)
{
  return simIPAddrConflictLastDetectIPGet(conflictIP);
}

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
L7_RC_t usmDbIPAddrConflictLastDetectMACGet(L7_uchar8 *conflictMAC)
{
  return simIPAddrConflictLastDetectMACGet(conflictMAC);
}

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
L7_RC_t usmDbIPAddrConflictLastDetectTimeGet(L7_uint32 *conflictDetectTime)
{
  return simIPAddrConflictLastDetectTimeGet(conflictDetectTime);
}

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
L7_RC_t usmDbImageSemaTake(L7_uint32 wait)
{
  return simImageSemaTake(wait);
}

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
L7_RC_t usmDbImageSemaGive()
{
  return simImageSemaGive();
}

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
                             L7_BOOL actImmediate)
{
  if (mgmtPortType == L7_MGMT_IPPORT)
  {
#ifdef L7_ROUTING_PACKAGE
    return ipMapRtrIntfIpAddressMethodSet (intIfNum, method, actImmediate);
#endif /* L7_ROUTING_PACKAGE */
  }
  else
  {
    return dhcpClientIPAddressMethodSet (intIfNum, method, mgmtPortType,
                                         actImmediate);
  }
  return L7_FAILURE;
}

