/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config_lldp.c
 *
 * @purpose show running config commands for LLDP
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  03/05/2007
 *
 * @author  Amit Kulkarni
 *
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "datatypes.h"
#include "commdefs.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_switching_cli.h"
#include "strlib_base_cli.h"

#include "usmdb_counters_api.h"
#include "cli_web_exports.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "usmdb_lldp_api.h"
#include "defaultconfig.h"

#include "ews.h"
#include "ews_cli.h"

#include "cliutil.h"
#include "cli_show_running_config.h"
#include "clicommands_card.h"


/*********************************************************************
 * @purpose  To print the running configuration of LLDP
 *
 * @param    EwsContext ewsContext@b((input))
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 * @end
 *********************************************************************/
L7_RC_t cliRunningConfigLLDPInfo(EwsContext ewsContext)
{
  L7_uint32 val, flag = 0;
  L7_RC_t rc = L7_SUCCESS;
  L7_BOOL port, name, desc, cap;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH], temp[L7_CLI_MAX_STRING_LENGTH];
  port = name = desc = cap = L7_FALSE;

  rc = usmDbLldpTxIntervalGet(&val);
  osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_LldpTimers_1);
  if (rc == L7_SUCCESS)
  {
    if (val != FD_LLDP_TX_INTERVAL)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, temp, sizeof(temp), pStrInfo_base_Intvl_1, val);
      OSAPI_STRNCAT(buf, temp);
      flag = 1;
    }
    memset (temp, 0, sizeof(temp));
  }
  rc = usmDbLldpTxHoldGet(&val);
  if (rc == L7_SUCCESS)
  {
    if (val != FD_LLDP_TX_HOLD_MULTIPLIER)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, temp, sizeof(temp), pStrInfo_base_Hold, val);
      OSAPI_STRNCAT(buf, temp);
      flag = 1;
    }
  }
  rc = usmDbLldpTxReinitDelayGet(&val);
  if (rc == L7_SUCCESS)
  {
    if (val != FD_LLDP_REINIT_DELAY)
    {
      osapiSnprintfAddBlanks (0, 0, 0, 1, L7_NULLPTR, temp, sizeof(temp), pStrInfo_base_Reinit, val);
      OSAPI_STRNCAT(buf, temp);
      flag = 1;
    }
    memset (temp, 0, sizeof(temp));
  }
  if (flag == 1)
  {
    EWSWRITEBUFFER(ewsContext, buf);
  }
  memset (buf, 0, sizeof(buf));
  rc = usmDbLldpNotificationIntervalGet(&val);
  if (val != FD_LLDP_NOTIFICATION_INTERVAL)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_LldpNotificationIntvl, val);
    EWSWRITEBUFFER(ewsContext, buf);
  }
  /* LLDP-MED configuration */
  memset(buf, 0, sizeof(buf));
  rc = usmDbLldpXMedFastStartRepeatCountGet(&val);
  if (val != FD_LLDP_MED_FASTSTART_REPEAT_COUNT)
  {
	osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_LldpMedFastStart, val);
	/*osapiSnprintf(buf, sizeof(buf), "\r\nlldp med faststartrepeatcount %d", val);*/
	EWSWRITEBUFFER(ewsContext, buf);
  }

  EWSWRITEBUFFER(ewsContext,pStrInfo_common_CrLf);
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of LLDP in Interface mode
 *
 * @param    EwsContext	ewsContext		@b((input))
 * @param    L7_uint32		interface			@b((input))		interface number
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/
L7_RC_t cliRunningInterfaceConfigLLDPInfo(EwsContext ewsContext, L7_uint32 interface)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 mode = 0;
  L7_uint32 flag = 0;
  L7_BOOL port, name, desc, cap, notif;
  L7_char8 buf [L7_CLI_MAX_STRING_LENGTH], temp[L7_CLI_MAX_STRING_LENGTH];
  lldpXMedCapabilities_t medcap;
  L7_BOOL bitSet = L7_FALSE;

  memset (temp, 0, sizeof(temp));
  port = name = desc = cap = L7_FALSE;

  rc = usmDbLldpIntfTxModeGet(interface, &mode);
  if (rc == L7_SUCCESS)
  {
    if (mode != L7_DISABLE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpTx_1);
    }
  }

  rc = usmDbLldpIntfRxModeGet(interface, &mode);
  if (rc == L7_SUCCESS)
  {
    if (mode != L7_DISABLE)
    {
      EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpReceive_1);
    }
  }
  rc = usmDbLldpIntfTxTLVsGet(interface, &port, &name, &desc, &cap);
  if (rc == L7_SUCCESS)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_LldpTxTlv);
    if (port != L7_FALSE)
    {
      OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_LldpPortDesc);
      flag = 1;
    }

    if (name != L7_FALSE)
    {
      OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_LldpSysname);
      flag = 1;
    }

    if (desc != L7_FALSE)
    {
      OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_LldpSysdesc);
      flag = 1;
    }

    if (cap != L7_FALSE)
    {
      OSAPI_STRNCAT_ADD_BLANKS (0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_LldpSyscap);
      flag = 1;
    }
    if (flag == 1)
    {
      EWSWRITEBUFFER(ewsContext, buf);
    }
  }

  rc = usmDbLldpIntfTxMgmtAddrGet(interface, &port);
  if (port != L7_FALSE)
  {
    EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpTxMgmt);
  }

  if(usmDbLldpIsValidIntf(interface) == L7_SUCCESS)
  {
    rc = usmDbLldpIntfNotificationModeGet(interface, &mode);
    if (( mode != FD_LLDP_PORT_NOTIFICATION_ADMIN_MODE) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
    {
      if ( mode == L7_ENABLE)
      {
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpNotification_1);
      }
      else
      {
        EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_NoLldpNotification);
      }
    }
  }

  /* LLDP-MED configuration */
  rc = usmDbLldpXMedPortAdminModeGet(interface, &mode);
  if (rc == L7_SUCCESS &&
	  mode != FD_LLDP_MED_PORT_ADMIN_MODE)
  {
	if (mode == L7_ENABLE)
	{
	  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMed_1);
	}
	else
	{
	  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMed_no);
	}
  }
  rc = usmDbLldpXMedPortConfigNotifEnableGet(interface, &notif);
  if (rc == L7_SUCCESS &&
	  notif != FD_LLDP_MED_PORT_NOTIFY_MODE)
  {
	if (notif == L7_TRUE)
	{
	  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedConfigNotif);
	}
	else
	{
	  EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedConfigNotif_no);
	}
  }
  memset(&medcap,  0,  sizeof(lldpXMedCapabilities_t));
  rc = usmDbLldpXMedPortConfigTLVsEnabledGet(interface, &medcap);
  if (rc == L7_SUCCESS && ((L7_uint32)(medcap.bitmap[1]) != FD_LLDP_MED_PORT_TVL_TX))
  {

	/* Some bits are differnt from the default */
	/* LLDP_MED_CAP_CAPABILITIES_BITMASK */
	bitSet = (LLDP_MED_CAP_CAPABILITIES_BITMASK & medcap.bitmap[1]) ? L7_TRUE:L7_FALSE;
	if ((LLDP_MED_CAP_CAPABILITIES_BITMASK & FD_LLDP_MED_PORT_TVL_TX))
	{
	  if (bitSet == L7_FALSE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvCap_no);
	  }
    }
	else
	{
	  if (bitSet == L7_TRUE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvCap);
	  }
	}

	/* LLDP_MED_CAP_NETWORKPOLICY_BITMASK */
	bitSet = (LLDP_MED_CAP_NETWORKPOLICY_BITMASK & medcap.bitmap[1]) ? L7_TRUE:L7_FALSE;
	if ((LLDP_MED_CAP_NETWORKPOLICY_BITMASK & FD_LLDP_MED_PORT_TVL_TX))
	{
	  if (bitSet == L7_FALSE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvNetPol_no);
	  }
    }
	else
	{
	  if (bitSet == L7_TRUE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvNetPol);
	  }
	}
	/* LLDP_MED_CAP_LOCATION_BITMASK */
	bitSet = (LLDP_MED_CAP_LOCATION_BITMASK & medcap.bitmap[1]) ? L7_TRUE:L7_FALSE;
	if ((LLDP_MED_CAP_LOCATION_BITMASK & FD_LLDP_MED_PORT_TVL_TX))
	{
	  if (bitSet == L7_FALSE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvLoc_no);
	  }
    }
	else
	{
	  if (bitSet == L7_TRUE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvLoc);
	  }
	}
	/* LLDP_MED_CAP_EXT_PSE_BITMASK */
	bitSet = (LLDP_MED_CAP_EXT_PSE_BITMASK & medcap.bitmap[1]) ? L7_TRUE:L7_FALSE;
	if ((LLDP_MED_CAP_EXT_PSE_BITMASK & FD_LLDP_MED_PORT_TVL_TX))
	{
	  if (bitSet == L7_FALSE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvExtPse_no);
	  }
    }
	else
	{
	  if (bitSet == L7_TRUE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvExtPse);
	  }
	}
	/* LLDP_MED_CAP_EXT_PD_BITMASK */
	bitSet = (LLDP_MED_CAP_EXT_PD_BITMASK & medcap.bitmap[1]) ? L7_TRUE:L7_FALSE;
	if ((LLDP_MED_CAP_EXT_PD_BITMASK & FD_LLDP_MED_PORT_TVL_TX))
	{
	  if (bitSet == L7_FALSE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvExtPd_no);
	  }
    }
	else
	{
	  if (bitSet == L7_TRUE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvExtPd);
	  }
	}
	/* LLDP_MED_CAP_INVENTORY_BITMASK */
	bitSet = (LLDP_MED_CAP_INVENTORY_BITMASK & medcap.bitmap[1]) ? L7_TRUE:L7_FALSE;
	if ((LLDP_MED_CAP_INVENTORY_BITMASK & FD_LLDP_MED_PORT_TVL_TX))
	{
	  if (bitSet == L7_FALSE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvInv_no);
	  }
    }
	else
	{
	  if (bitSet == L7_TRUE)
	  {
		EWSWRITEBUFFER_ADD_BLANKS (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_LldpMedTxTlvInv);
	  }
	}
  }

  return L7_SUCCESS;
}
