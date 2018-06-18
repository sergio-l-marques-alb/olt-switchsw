/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_show_running_config_lacp.c
 *
 * @purpose show running config commands for the cli
 *
 * @component user interface
 *
 * @comments
 *
 * @create  01/07/2003
 *
 * @author  ARR
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include "l7_common.h"
#include "cli_web_exports.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_util_api.h"
#include "comm_mask.h"
#include "config_script_api.h"
#include "datatypes.h"
#include "cli_show_running_config.h"
#include "dot3ad_exports.h"
#ifndef _L7_OS_LINUX_
  #include <inetLib.h>    /* for converting from IP to L7_int32 */
#endif /* _L7_OS_LINUX_ */

#include "clicommands_lacp.h"

/*********************************************************************
 * @purpose  To print the running configuration of Dot3 Info
 *
 * @param    EwsContext ewsContext
 * @param    L7_uint32 unit
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes
 *
 @end
 *********************************************************************/

L7_RC_t cliRunningConfigDot3Info(EwsContext ewsContext, L7_uint32 unit)
{
  L7_uint32 interface, nextInterface;
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat1[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_int32 u=1;
  L7_int32 hashMode;
  L7_int32 s, p;
  L7_uint32 numOfArg;
  L7_uint32 val,val1;
  L7_ushort16 count;
  L7_uint32 listMember[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32 portFactoryDefaultSpeed;

  /* Begin display port-channel system priority */

  L7_uint32 priorityValue = 0;
  rc = usmDbDot3adAggPortActorSystemPriorityGet(unit, 0, &priorityValue);
  if ( rc == L7_SUCCESS && (( priorityValue != FD_DOT3AD_ACTOR_DEFAULT_SYS_PRIORITY) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LagSysPri, priorityValue);
    EWSWRITEBUFFER(ewsContext, stat);
  }

 /* Begin display port-channel system hash mode */
  if (usmDbDot3adSystemHashModeGet(unit, &hashMode) ==  L7_SUCCESS)
  {
    if ( hashMode != FD_DOT3AD_HASH_MODE )
    {
       OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_LagLoadBalance);
       osapiSnprintf(stat, sizeof(stat), "%d %s", hashMode, pStrInfo_common_All);
       OSAPI_STRNCAT(buf, stat);
       EWSWRITEBUFFER(ewsContext,buf);
    }
  }
  /* End of port-channel system priority */
  memset(stat, 0, sizeof(stat));
  rc = usmDbDot3adIfIndexGetFirst(unit, &nextInterface);
  if ( rc == L7_SUCCESS )
  {
    rc = usmDbIntIfNumFromExtIfNum(nextInterface, &interface);
  }

  while ( rc == L7_SUCCESS )
  {
    if ( usmDbDot3adIsConfigured(unit, interface) == L7_TRUE )
    {
      if (usmDbUnitSlotPortGet(interface, &u, &s, &p) != L7_SUCCESS)
      {
        break;
      }

      if (usmDbDot3adNameGet(unit, interface, stat) ==  L7_SUCCESS)
      {
        if (cnfgrIsFeaturePresent(L7_DOT3AD_COMPONENT_ID, L7_DOT3AD_LAG_PRECREATE_FEATURE_ID) )
        {
          osapiSnprintf(buf, sizeof(buf), "%s%d", FD_DOT3AD_DEFAULT_LAG_NAME, p);
          if (strcmp(buf, stat) != 0)
          {
            OSAPI_STRNCPY_SAFE_ADD_BLANKS(1, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Lag_4);
            OSAPI_STRNCAT_ADD_BLANKS(0, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_name);
            OSAPI_STRNCAT_ADD_BLANKS(0, 0, 0, 1, L7_NULLPTR, buf, cliDisplayInterfaceHelp(u,s,p));
            OSAPI_STRNCAT_ADD_BLANKS(0, 0, 0, 1, L7_NULLPTR, buf, stat);
            EWSWRITEBUFFER(ewsContext,buf);
          }
        }
        else if (( strcmp(stat, FD_DOT3AD_NAME) != 0 )|| EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)
        {
          OSAPI_STRNCPY_SAFE_ADD_BLANKS (1, 0, 0, 1, L7_NULLPTR, buf, pStrInfo_base_Lag_4);
          OSAPI_STRNCAT(buf, stat);
          EWSWRITEBUFFER(ewsContext,buf);
        }
      }

      if (usmDbDot3adLinkTrapGet(unit, interface, &val) == L7_SUCCESS)
      {
        cliShowCmdStr(ewsContext,val,FD_DOT3AD_LINK_TRAP_MODE,pStrInfo_base_LagLinktrap,cliDisplayInterfaceHelp(u,s,p));
      }

      numOfArg = L7_MAX_MEMBERS_PER_LAG;
      if (usmDbDot3adMemberListGet(unit, interface, &numOfArg,  listMember) == L7_SUCCESS)
      {
        for (count=0; count < numOfArg; count++)
        {
          memset(buf, 0, sizeof(buf));
          if (usmDbUnitSlotPortGet(listMember[count], &unit, &s, &p) == L7_SUCCESS)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Intf_4, cliDisplayInterfaceHelp(unit, s, p));
            EWSWRITEBUFFER(ewsContext, stat);
            /*************************************************************************/
            if (usmDbIfAutoNegAdminStatusGet(unit, listMember[count], &val) == L7_SUCCESS)
            {
              cliShowCmdEnable (ewsContext, val, FD_NIM_AUTONEG_ENABLE, pStrInfo_base_AutoNegotiate);
            }
                 
            if (usmDbIfTypeGet(unit, listMember[count], &val) == L7_SUCCESS)
            {
              switch (val)
              {
                case L7_IANA_FAST_ETHERNET:
                  portFactoryDefaultSpeed = FD_NIM_FAST_ENET_SPEED;
                  break;
        
                case L7_IANA_FAST_ETHERNET_FX:
                  portFactoryDefaultSpeed = FD_NIM_FAST_ENET_FX_SPEED;
                  break;
        
                case L7_IANA_GIGABIT_ETHERNET:
                  portFactoryDefaultSpeed = FD_NIM_GIG_ENET_SPEED;
                  break;
        
                /* PTin added: Speed 2.5G */
                case L7_IANA_2G5_ETHERNET:
                  portFactoryDefaultSpeed = FD_NIM_2G5_ENET_SPEED;
                  break;
                /* PTin end */

                case L7_IANA_10G_ETHERNET:
                  portFactoryDefaultSpeed = FD_NIM_10G_ENET_SPEED;
                  break;
        
                /* PTin added: Speed 40G */
                case L7_IANA_40G_ETHERNET:
                  portFactoryDefaultSpeed = FD_NIM_40G_ENET_SPEED;
                  break;

                /* PTin added: Speed 100G */
                case L7_IANA_100G_ETHERNET:
                  portFactoryDefaultSpeed = FD_NIM_100G_ENET_SPEED;
                  break;
                /* PTin end */
        
                default:
                  portFactoryDefaultSpeed = FD_NIM_OTHER_PORT_TYPE_SPEED;
                  break;
              }
        
              if (usmDbIfAutoNegAdminStatusGet(unit, listMember[count], &val1) == L7_SUCCESS)
              {
                if (val1 != FD_NIM_AUTONEG_ENABLE) /* If not the default value. */
                {
                  /* print out the correct speed */
                  if ((usmDbIfDefaultSpeedGet(unit, listMember[count], &val) == L7_SUCCESS)&&
                      ((val != portFactoryDefaultSpeed)|| (EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT)))
                  {
                    switch ( val )
                    {
                      /* PTin added: Speed 100G */
                      case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
                        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat1, sizeof(stat1), pStrInfo_base_Speed100gFullDuplex);
                        break;
                      /* PTin added: Speed 40G */
                      case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
                        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat1, sizeof(stat1), pStrInfo_base_Speed40gFullDuplex);
                        break;
                      /* PTin end */
                      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
                        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat1, sizeof(stat1), pStrInfo_base_Speed10gFullDuplex);
                        break;
                      /* PTin added: Speed 2.5G *** Note: 1000SX was also added! */
                      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
                        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat1, sizeof(stat1), pStrInfo_base_Speed2500FullDuplex);
                        break;
                      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
                        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat1, sizeof(stat1), pStrInfo_base_Speed1000FullDuplex);
                        break;
                      /* PTin end */
                      case L7_PORTCTRL_PORTSPEED_HALF_100TX:
                        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat1, sizeof(stat1), pStrInfo_base_Speed100HalfDuplex);
                        break;
                      case L7_PORTCTRL_PORTSPEED_FULL_100TX:
                        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat1, sizeof(stat1), pStrInfo_base_Speed100FullDuplex);
                        break;
                      case L7_PORTCTRL_PORTSPEED_HALF_10T:
                        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat1, sizeof(stat1), pStrInfo_base_Speed10HalfDuplex);
                        break;
                      case L7_PORTCTRL_PORTSPEED_FULL_10T:
                        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat1, sizeof(stat1), pStrInfo_base_Speed10FullDuplex);
                        break;
                      default:
                        /* Do nothing */
                        break;
                    }
                    EWSWRITEBUFFER(ewsContext,stat1);
                  }
                }
              }
            }
            /*************************************************************************/
            if (usmDbUnitSlotPortGet(interface, &u, &s, &p) == L7_SUCCESS)
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_Addport_1, cliDisplayInterfaceHelp(u,s,p));
              EWSWRITEBUFFER(ewsContext, stat);
            }
            EWSWRITEBUFFER_ADD_BLANKS (1, 1, 0, 0, L7_NULLPTR, ewsContext,pStrInfo_common_Exit); /*exit from interface mode*/
          }
        }
      }
    }

    if (usmDbDot3adIfIndexGetNext(unit, nextInterface, &nextInterface) == L7_SUCCESS)
    {
      rc = usmDbIntIfNumFromExtIfNum(nextInterface, &interface);
    }
    else
    {
      rc = L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  To print the running configuration of LACP in Interface mode
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
L7_RC_t cliRunningInterfaceConfigLACPInfo(EwsContext ewsContext, L7_uint32 unit, L7_uint32 interface)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 priorityValue = 0;
  L7_uint32 keyValue = 0;
  L7_uchar8 adminState = 0;
  L7_uchar8 sysID[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 itype;
  L7_BOOL defBit, curBit;

  if (usmDbDot3adIsConfigured(unit, interface) == L7_TRUE)
  {
    /* lacp admin key */
    rc = usmDbDot3adAggActorAdminKeyGet(unit, interface, &keyValue);
    if(rc == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext, keyValue, interface, pStrInfo_base_LacpAdminKey_1);
    }

    /* lacp collector max_delay */
    rc = usmDbDot3adAggCollectorMaxDelayGet(unit, interface, &keyValue);
    if(rc == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext, keyValue, FD_DOT3AD_DEFAULT_LACP_MAXDELAY, pStrInfo_base_LacpCollectorMaxDelay_1);
    }
  }

  if ( (usmDbIntfTypeGet(interface, &itype) == L7_SUCCESS) && (itype == L7_PHYSICAL_INTF) )
  {
#if 0
    /* lacp actor system priority   */
    rc = usmDbDot3adAggPortActorSystemPriorityGet(unit, interface, &priorityValue);
    if(rc == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext, priorityValue, FD_DOT3AD_ACTOR_DEFAULT_SYS_PRIORITY, pStrInfo_base_LacpActorSysPri_1);
    }
#endif
    /* acp actor admin key  */
    rc = usmDbDot3adAggPortActorAdminKeyGet(unit, interface, &keyValue);
    if(rc == L7_SUCCESS)
    {
      cliShowCmdInt(ewsContext, keyValue, L7_NULL, pStrInfo_base_LacpActorAdminKey_1);
    }

    /* lacp actor admin state */

    rc = usmDbDot3adAggPortActorAdminStateGet(unit, interface, &adminState);
	defBit = (FD_DOT3AD_ACTOR_ADMIN_PORT_STATE & DOT3AD_STATE_LACP_ACTIVITY) ? L7_TRUE : L7_FALSE;
	curBit =(adminState & DOT3AD_STATE_LACP_ACTIVITY) ? L7_TRUE : L7_FALSE;
	memset(stat, 0, sizeof(stat));
    if (rc == L7_SUCCESS &&
        (( defBit != curBit) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      if( (adminState & DOT3AD_STATE_LACP_ACTIVITY) == DOT3AD_STATE_LACP_ACTIVITY )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoLacpActorAdminStatePassive);
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpActorAdminStatePassive);
      }
      EWSWRITEBUFFER(ewsContext, stat);
	}
	defBit = (FD_DOT3AD_ACTOR_ADMIN_PORT_STATE & DOT3AD_STATE_LACP_TIMEOUT) ? L7_TRUE : L7_FALSE;
	curBit =(adminState & DOT3AD_STATE_LACP_TIMEOUT) ? L7_TRUE : L7_FALSE;
	memset(stat, 0, sizeof(stat));
	if (rc == L7_SUCCESS &&
        (( defBit != curBit) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      if(  (adminState & DOT3AD_STATE_LACP_TIMEOUT) == DOT3AD_STATE_LACP_TIMEOUT )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoLacpActorAdminStateLongtimeout);
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpActorAdminStateLongtimeout);
      }
      EWSWRITEBUFFER(ewsContext, stat);
	}
	defBit = (FD_DOT3AD_ACTOR_ADMIN_PORT_STATE & DOT3AD_STATE_AGGREGATION) ? L7_TRUE : L7_FALSE;
	curBit =(adminState & DOT3AD_STATE_AGGREGATION) ? L7_TRUE : L7_FALSE;
	memset(stat, 0, sizeof(stat));
	if (rc == L7_SUCCESS &&
        (( defBit != curBit) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      if( (adminState & DOT3AD_STATE_AGGREGATION) == DOT3AD_STATE_AGGREGATION )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoLacpActorAdminStateIndividual);
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpActorAdminStateIndividual);
      }
      EWSWRITEBUFFER(ewsContext, stat);
    }

    /* lacp actor port priority */
    priorityValue = 0;
    rc = usmDbDot3adAggPortActorPortPriorityGet(unit, interface, &priorityValue);
    if ( rc == L7_SUCCESS && (( priorityValue != FD_DOT3AD_ACTOR_DEFAULT_PORT_PRIORITY) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpActorPortPri_1, priorityValue);
      EWSWRITEBUFFER(ewsContext, stat);
    }

    /* lacp partner system priorirty */
    priorityValue = 0;
    rc = usmDbDot3adAggPortPartnerAdminSystemPriorityGet(unit, interface, &priorityValue);
    if ( rc == L7_SUCCESS && (( priorityValue != FD_DOT3AD_PARTNER_DEFAULT_PORT_PRIORITY) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpPartnerSysPri_1, priorityValue);
      EWSWRITEBUFFER(ewsContext, stat);
    }

    /* lacp partner system-id */
    memset(sysID, 0, sizeof(sysID));
    rc = usmDbDot3adAggPortPartnerAdminSystemIDGet(unit, interface, sysID);
    if ( rc == L7_SUCCESS && ((!(sysID[0] == 0x00 && sysID[1] == 0x00 && sysID[2] == 0x00 && sysID[3] == 0x00 && sysID[4] == 0x00 && sysID[5] == 0x00) ) ||
          EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      memset(stat, 0, sizeof(stat));
      osapiSnprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X ",
          sysID[0], sysID[1], sysID[2], sysID[3], sysID[4], sysID[5]);
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpPartnerSysId_1, buf);
      EWSWRITEBUFFER(ewsContext, stat);
    }

    /* lacp partner admin key */
    keyValue = 0;
    rc = usmDbDot3adAggPortPartnerAdminKeyGet(unit, interface, &keyValue);
    if ( rc == L7_SUCCESS && (( keyValue != FD_DOT3AD_PARTNER_DEFAULT_ADMINKEY) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpPartnerAdminKey_1, keyValue);
      EWSWRITEBUFFER(ewsContext, stat);
    }

    /* lacp partner admin state  */
    adminState = 0;
    rc = usmDbDot3adAggPortPartnerAdminStateGet(unit, interface, &adminState);

    memset(stat, 0, sizeof(stat));
	defBit = (FD_DOT3AD_PARTNER_ADMIN_PORT_STATE & DOT3AD_STATE_LACP_ACTIVITY) ? L7_TRUE : L7_FALSE;
	curBit =(adminState & DOT3AD_STATE_LACP_ACTIVITY) ? L7_TRUE : L7_FALSE;
    if (rc == L7_SUCCESS &&
		(( defBit != curBit) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
	{
      if( (adminState & DOT3AD_STATE_LACP_ACTIVITY) == DOT3AD_STATE_LACP_ACTIVITY )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_NoLacpPartnerAdminStatePassive);
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpPartnerAdminStatePassive);
      }
      EWSWRITEBUFFER(ewsContext, stat);
	}
	memset(stat, 0, sizeof(stat));
	defBit = (FD_DOT3AD_PARTNER_ADMIN_PORT_STATE & DOT3AD_STATE_LACP_TIMEOUT) ? L7_TRUE : L7_FALSE;
	curBit =(adminState & DOT3AD_STATE_LACP_TIMEOUT) ? L7_TRUE : L7_FALSE;
	if (rc == L7_SUCCESS &&
		(( defBit != curBit) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
	{
      if(  (adminState & DOT3AD_STATE_LACP_TIMEOUT) == DOT3AD_STATE_LACP_TIMEOUT )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),pStrInfo_base_NoLacpPartnerAdminStateLongtimeout);
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),pStrInfo_base_LacpPartnerAdminStateLongtimeout);
      }
      EWSWRITEBUFFER(ewsContext, stat);
	}
	defBit = (FD_DOT3AD_PARTNER_ADMIN_PORT_STATE & DOT3AD_STATE_AGGREGATION) ? L7_TRUE : L7_FALSE;
	curBit =(adminState & DOT3AD_STATE_AGGREGATION) ? L7_TRUE : L7_FALSE;
	memset(stat, 0, sizeof(stat));
	if (rc == L7_SUCCESS &&
		(( defBit != curBit) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
	{
      if( (adminState & DOT3AD_STATE_AGGREGATION) == DOT3AD_STATE_AGGREGATION )
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),pStrInfo_base_NoLacpPartnerAdminStateIndividual);
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat),pStrInfo_base_LacpPartnerAdminStateIndividual);
      }
      EWSWRITEBUFFER(ewsContext, stat);
    }

    /* lacp partner port id  */
    priorityValue = 0;
    rc = usmDbDot3adAggPortPartnerAdminPortGet(unit, interface, &keyValue);
    if ( rc == L7_SUCCESS && (( keyValue != FD_DOT3AD_PARTNER_DEFAULT_PORTID) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpPartnerPortId_1, keyValue);
      EWSWRITEBUFFER(ewsContext, stat);
    }

    /* lacp partner port priority  */
    priorityValue = 0;
    rc = usmDbDot3adAggPortPartnerAdminPortPriorityGet(unit, interface, &priorityValue);
    if ( rc == L7_SUCCESS && (( priorityValue != FD_DOT3AD_PARTNER_DEFAULT_PORT_PRIORITY) || EWSSHOWALL(ewsContext) == L7_SHOW_DEFAULT))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, stat, sizeof(stat), pStrInfo_base_LacpPartnerPortPri_1, priorityValue);
      EWSWRITEBUFFER(ewsContext, stat);
    }
  }

  /*EWSWRITEBUFFER(ewsContext,"\r\n");*/
  return L7_SUCCESS;
}



