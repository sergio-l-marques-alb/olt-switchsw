/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/security/dot1x/clicommands_dot1x.c
 *
 * @purpose create the tree for CLI Authentication commands
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  22/07/2003
 *
 * @author  chinmoyb
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_security_common.h"
#include "strlib_security_cli.h"
#include "clicommands_dot1x.h"
#include "usmdb_util_api.h"
#include "dot1x_exports.h"
#include "usmdb_common.h"

/*********************************************************************
*
* @purpose  Build the dot1x global configuration commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalDot1xConfig(EwsCliCommandP depth1)
{
  /* depth1 = Global Config Mode */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Dot1x_2, pStrInfo_security_Dot1x_1, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  /* depth3 = ewsCliAddNode(depth2, pStrInfo_security_DeflLogin, pStrInfo_security_Dot1xDeflLogin, commandDot1xDefaultLogin, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Listname, pStrInfo_security_Dot1xListName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Login_1, pStrInfo_security_Dot1xLogin, commandDot1xLogin, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Usr, pStrInfo_common_AuthUsrName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Listname, pStrInfo_security_Dot1xListName, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS); */

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_PortCntrl, pStrInfo_security_Dot1xPortCntrl, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_security_Dot1xPortCntrlAll, commandDot1xPortControlAll, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Auto2, pStrInfo_security_Dot1xPortCntrlAuto, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_ForceAuthorizedDot1x, pStrInfo_security_Dot1xPortCntrlForceAuth, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_ForceUnAuthorizedDot1x, pStrInfo_security_Dot1xPortCntrlForceUnAuth, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)==L7_TRUE) 
  {
      depth5 = ewsCliAddNode(depth4, pStrInfo_security_MacBasedDot1x, pStrInfo_security_Dot1xPortCntrlMacBasedAuth, NULL, L7_NO_OPTIONAL_PARAMS);
      depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }


  depth3 = ewsCliAddNode(depth2, pStrInfo_security_SysAuthCntrl, pStrInfo_security_Dot1xAdminMode, commandDot1xSystemAuthControl, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_SysAuthCntrlMonitor, pStrInfo_security_Dot1xMonitorMode, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_LogFacilityUsr, pStrInfo_security_Dot1xUsr, commandDot1xUser, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Usr, pStrInfo_common_AuthUsrName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_security_Dot1xUsrAll, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOT1X_COMPONENT_ID, L7_DOT1X_DYNAMIC_VLANASSIGN_FEATURE_ID) == L7_TRUE)
  {
    depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dynamic_Vlan, pStrInfo_security_Dot1xDynamicVlanCreation, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_Enbl_2, pStrInfo_security_Dot1xDynamicVlanConfiguration, commandDot1xRadiusDynamicVlanCreation, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
}

/*********************************************************************
*
* @purpose  Build the dot1x interface configuration commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeInterfaceDot1xConfig(EwsCliCommandP depth1)
{
  /* depth1 = Interface Config Mode */
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Dot1x_2, pStrInfo_security_IntfDot1x, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_GuestVlan, pStrInfo_security_Dot1xGuestVlan, commandDot1xGuestVlanId, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_VlanId,pStrInfo_security_Dot1xVlan,NULL,L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_MaxReq, pStrInfo_security_Dot1xMaxReq, commandDot1xMaxRequests, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range1to10, pStrInfo_security_Dot1xMaxReqCount, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_MaxUsers,pStrInfo_security_Dot1xMaxUsers, commandDot1xMaxUserInterface,2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Range1to16,pStrInfo_security_Dot1xMaxUsersCount, NULL,L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL,L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_PortCntrl, pStrInfo_security_Dot1xPortCntrl, commandDot1xPortControl, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Auto2, pStrInfo_security_Dot1xPortCntrlAuto, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_ForceAuthorizedDot1x, pStrInfo_security_Dot1xPortCntrlForceAuth, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_ForceUnAuthorizedDot1x, pStrInfo_security_Dot1xPortCntrlForceUnAuth, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  if (cnfgrIsFeaturePresent(L7_DOT1X_COMPONENT_ID,L7_DOT1X_MACBASEDAUTH_FEATURE_ID)==L7_TRUE)
  {
      depth4 = ewsCliAddNode(depth3, pStrInfo_security_MacBasedDot1x, pStrInfo_security_Dot1xPortCntrlMacBasedAuth, NULL, L7_NO_OPTIONAL_PARAMS);
      depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Timeout_2, pStrInfo_security_Dot1xTimeout, commandDot1xTimeout, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Dot1xGuestPeriod, pStrInfo_security_Dot1xTimeoutGuestVlanTime, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Seconds_1, pStrInfo_security_Dot1xTimeoutGuestVlanTimeVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Dot1xQuietPeriod, pStrInfo_security_Dot1xTimeoutQperiod, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Seconds_1, pStrInfo_security_Dot1xTimeoutQperiodVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Dot1xReauthPeriod, pStrInfo_security_Dot1xTimeoutReAuthPer, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Seconds_1, pStrInfo_security_Dot1xTimeoutReAuthPerVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Dot1xSrvrTimeout, pStrInfo_security_Dot1xTimeoutSvr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Seconds_1, pStrInfo_security_Dot1xTimeoutSvrVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Dot1xSuppTimeout, pStrInfo_security_Dot1xTimeoutSupp, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Seconds_1, pStrInfo_security_Dot1xTimeoutSvrVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Dot1xTxPeriod, pStrInfo_security_Dot1xTimeoutTxPer, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Seconds_1, pStrInfo_security_Dot1xTimeoutSvrVal, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_unauthenticatedVlan, pStrInfo_security_Dot1xUnauthenticatedVlan, commandDot1xUnatuthenticatedVlanId, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_VlanId,pStrInfo_security_Dot1xVlan,NULL,L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  /* Port PAE capability CLI commands */
  depth3 = ewsCliAddNode(depth2, pStrInfo_security_PaeCapability, pStrInfo_security_Dot1xPaeCapability,
                     commandDot1xPaeCapability,L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_security_PaeSupplicant,pStrInfo_security_Dot1xPortPaeSupplicant,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_security_PaeAuthenticator, pStrInfo_security_Dot1xPortPaeAuthenticator,
                         NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_ReAuth, pStrInfo_security_Dot1xReAuth_1, commandDot1xReauthentication, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

/* Supplicant Parameters related CLI commands */

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_PaeSupplicant, pStrInfo_security_Dot1xSupplicantParams,NULL,
                         2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_LogFacilityUsr, pStrInfo_security_Dot1xSupplicantUser,
                         commandDot1xSupplicantUser,2,L7_NO_COMMAND_SUPPORTED,L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Usr, pStrInfo_common_AuthUsrName, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
   ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_security_PortCntrl, pStrInfo_security_Dot1xPortCntrl,
                    commandDot1xSupplicantPortControl, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Auto2, pStrInfo_security_Dot1xPortCntrlAuto, NULL, L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_security_ForceAuthorizedDot1x, pStrInfo_security_Dot1xPortCntrlForceAuth, NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_security_ForceUnAuthorizedDot1x,
                        pStrInfo_security_Dot1xPortCntrlForceUnAuth, NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_security_MaxStart, pStrInfo_security_Dot1xMaxStart, commandDot1xMaxStarts, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Range1to10, pStrInfo_security_Dot1xMaxStartCount, NULL, L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Timeout_2, pStrInfo_security_Dot1xTimeout,
                         commandDot1xSupplicantTimeout,2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);

  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Dot1xStartPeriod, pStrInfo_security_Dot1xTimeoutStartPeriod,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_security_Seconds_1, pStrInfo_security_Dot1xTimeoutStartPeriodVal, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Dot1xHeldPeriod, pStrInfo_security_Dot1xTimeoutHeldPeriod,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_security_Seconds_1, pStrInfo_security_Dot1xTimeoutHeldPeriodVal, NULL,
                         L7_NO_OPTIONAL_PARAMS);
  ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth5 = ewsCliAddNode(depth4, pStrInfo_security_Dot1xAuthPeriod, pStrInfo_security_Dot1xTimeoutAuthPeriod,
                         NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth6 = ewsCliAddNode(depth5, pStrInfo_security_Seconds_1, pStrInfo_security_Dot1xTimeoutAuthPeriodVal, NULL,
                         L7_NO_OPTIONAL_PARAMS);
   ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);



  



}
/*********************************************************************
*
* @purpose  Build the dot1x clear commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivDot1xClear(EwsCliCommandP depth2)
{
  /* depth2 = clear */
  EwsCliCommandP depth3, depth4, depth5, depth6;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dot1x_2, pStrInfo_security_ClrDot1xStats, NULL, L7_NO_OPTIONAL_PARAMS);
  buildTreeDot1xAuthHistoryClear(depth3);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dot1xShowStats, pStrInfo_security_ClrDot1xStats, commandClearDot1xStatistics, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_security_ClrDot1xAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the dot1x privileged exec config commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivDot1xConfig(EwsCliCommandP depth1)
{
  /* depth1 = clear */
  L7_uint32 unit;
  EwsCliCommandP depth2, depth3, depth4, depth5, depth6;

  /* get switch ID based on presence/absence of STACKING package */
  unit = cliGetUnitId();
  if (unit == 0)
  {
    return;
  }

  depth2 = ewsCliAddNode(depth1, pStrInfo_common_Dot1x_2, pStrInfo_security_Dot1xPriv, NULL, L7_NO_OPTIONAL_PARAMS);
  if ( usmDbFeaturePresentCheck(unit, L7_DOT1X_COMPONENT_ID, L7_DOT1X_SET_CONTROLDIR_FEATURE_ID) == L7_TRUE  )
  {

    depth3 = ewsCliAddNode(depth2, pStrInfo_security_CntrlDir, pStrInfo_security_Dot1xPrivCntrlDir, commandDot1xControlDir, L7_NO_OPTIONAL_PARAMS);
    depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Both_1, pStrInfo_security_Dot1xCntrlDirBoth, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclInStr, pStrInfo_security_Dot1xCntrlDirIn, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth4 = ewsCliAddNode(depth3, pStrInfo_common_All, pStrInfo_security_ClrDot1xAll, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_Both_1, pStrInfo_security_Dot1xCntrlDirBoth, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
    depth5 = ewsCliAddNode(depth4, pStrInfo_common_AclInStr, pStrInfo_security_Dot1xCntrlDirIn, NULL, L7_NO_OPTIONAL_PARAMS);
    depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  }
  depth3 = ewsCliAddNode(depth2, pStrInfo_security_Initialize_1, pStrInfo_security_Dot1xPortInitialize, commandDot1xInitialize, L7_NO_OPTIONAL_PARAMS);
  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth3 = ewsCliAddNode(depth2, pStrInfo_security_ReAuthenticate, pStrInfo_security_Dot1xReAuth, commandDot1xReauthenticate, L7_NO_OPTIONAL_PARAMS);
  depth4 = buildTreeInterfaceHelp(depth3, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

}

/*********************************************************************
*
* @purpose  Build the dot1x privileged exec show commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreePrivDot1xShow(EwsCliCommandP depth2)
{
  /* depth2 = show */
  EwsCliCommandP depth3, depth4, depth5, depth6, depth7, depth8;

  depth3 = ewsCliAddNode(depth2, pStrInfo_common_Dot1x_2, pStrInfo_security_ShowDot1x, commandShowDot1x, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_security_ShowDot1xDetail, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dot1xShowStats, pStrInfo_security_ShowDot1xStats, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dot1xShowSummary, pStrInfo_security_ShowDot1xSummary, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_security_ClrDot1xAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Usrs_1, pStrInfo_security_ShowDot1xUsrs, commandShowDot1xUsers, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth4 = ewsCliAddNode(depth3, pStrInfo_security_Clients_1, pStrInfo_security_ShowDot1xClients, commandShowDot1xClients, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_security_ClrDot1xAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dot1xShowAuthHistory, pStrInfo_security_ShowDot1xAuthHistory, commandShowDot1xHistoryLogInfo, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_security_ShowAuthHistoryDetail, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowFailedAuthOnly, pStrInfo_security_ShowFailedAuthOnly, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_security_ShowAuthHistoryDetail, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_All, pStrInfo_security_ClrDot1xAll, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_security_ShowAuthHistoryDetail, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Dot1xShowFailedAuthOnly, pStrInfo_security_ShowFailedAuthOnly, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth7 = ewsCliAddNode(depth6, pStrInfo_common_IgmpProxyGrpsDetail, pStrInfo_security_ShowAuthHistoryDetail, NULL, L7_NO_OPTIONAL_PARAMS);
  depth8 = ewsCliAddNode(depth7, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}

/*********************************************************************
*
* @purpose  Build the authorization global configuration commands tree
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeGlobalAuthorizationConfig(EwsCliCommandP depth1)
{
  /* depth1 = Global Config Mode */
  EwsCliCommandP depth2, depth3, depth4, depth5;

  depth2 = ewsCliAddNode(depth1, pStrInfo_security_Authorization, pStrInfo_security_Dot1xAuthOr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth3 = ewsCliAddNode(depth2, pStrInfo_common_WsShowRunningNwMode, pStrInfo_security_Dot1xAuthOr, NULL, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth4 = ewsCliAddNode(depth3, pStrInfo_common_RadiusAuth, pStrInfo_security_Dot1xAuthOr, commandDot1xRadiusVlanAssignment, 2, L7_NO_COMMAND_SUPPORTED, L7_STATUS_BOTH);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}


/*********************************************************************
*
* @purpose  Build the dot1x clear commands tree for Dot1x Auth History
*
* @param struct EwsCliCommandP
*
* @returntype void
*
* @notes none
*
* @end
*
*********************************************************************/
void buildTreeDot1xAuthHistoryClear(EwsCliCommandP depth3)
{
  /* depth1 = clear */
  EwsCliCommandP depth6, depth4, depth5;

  depth4 = ewsCliAddNode(depth3, pStrInfo_common_Dot1xShowAuthHistory, pStrInfo_security_ClrDot1xAuthHistory,
                                       commandDot1xAuthenticationHistoryClear, L7_NO_OPTIONAL_PARAMS);
  depth5 = ewsCliAddNode(depth4, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
  depth5 = buildTreeInterfaceHelp(depth4, L7_NO_OPTIONAL_PARAMS, L7_NO_OPTION, L7_FIRST_COMPONENT_ID, L7_STATUS_NORMAL_ONLY);
  depth6 = ewsCliAddNode(depth5, pStrInfo_common_Cr, pStrInfo_common_NewLine, NULL, L7_NO_OPTIONAL_PARAMS);
}


