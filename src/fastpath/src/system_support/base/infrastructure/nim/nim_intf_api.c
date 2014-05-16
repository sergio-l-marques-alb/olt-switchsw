/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_intf_api.c
*
* @purpose    Interface management API for all other components
*
* @component  NIM
*
* @comments   none
*
* @create     05/07/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "simapi.h"
#include "l7_ip_api.h"
#include "nim_data.h"
#include "nim_util.h"
#include "nim_outcalls.h"
#include "dtlapi.h"
#include "nim_cnfgr.h"
#include <string.h>
#include "dot3ad_api.h"

#include "link_dependency_api.h"
#include "nim_exports.h"

#if L7_FEAT_SF10GBT
extern L7_BOOL  cliIsStackingSupported(void);
#include "nim_port_fw.h"
#include "usmdb_sim_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_unitmgr_api.h"
#include "hpc_sf_fwupdate.h"
#endif
/*
 * Local macro for checking if a given parameter can be set.
 * Must be used where the intInfNum ("i") has been validated.
 * Used by externalized version nimIntfParmCanSet().
 */
#define NIM_PARM_CANSET(i, p) \
        ((nimCtlBlk_g->nimPorts[(i)].operInfo.settableParms & (p)) == (p))

#if L7_FEAT_SF10GBT
extern void* nimFwRspQ;

/*
  Zero-filled firmware revision identifier
  is obtained when no module firmware is present,
  or if module firmware is not running
 */
nimIntfPortFWRev_t nimIntfPortFWRevUndefined =
{
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  {0x00,0x00,0x00,0x00, 0x00}
};
#endif


/*********************************************************************
* @purpose  Returns the internal interface type
*           associated with the internal interface number
*
* @param    intIfNum    internal interface number
* @param    sysIntfType pointer to internal interface type,
*                       (@b{Returns: Internal Interface Type}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfType(L7_uint32 intIfNum, L7_INTF_TYPES_t *sysIntfType)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase %d for operation\n", nimConfigPhaseGet());
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum, rc);

    if (rc == L7_SUCCESS)
    {
      *sysIntfType = nimCtlBlk_g->nimPorts[intIfNum].sysIntfType;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Returns the internal interface iftype
*           associated with the internal interface number
*
* @param    intIfNum    internal interface number
* @param    ifType      pointer to internal interface type,
*                       (@b{Returns: Internal Interface Type}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfifType(L7_uint32 intIfNum, L7_uint32 *ifType)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *ifType = nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  set the internal interface iftype
*
* @param    intIfNum    internal interface number
* @param    ifType      pointer to internal interface type,
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfifType(L7_uint32 intIfNum, L7_uint32 *ifType)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType = *ifType;
    }

    NIM_CRIT_SEC_WRITE_EXIT();
  }
  return(rc);
}

/*********************************************************************
* @purpose  Sets the speed of the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    intfSpeed   Interface Speed,
*                       (@b{  L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX,
*                             L7_PORTCTRL_PORTSPEED_UNKNOWN})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfSpeed(L7_uint32 intIfNum, L7_uint32 intfSpeed)
{
  L7_uint32 portCapability;
  L7_uint32 newintfSpeed;

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS && !NIM_PARM_CANSET(intIfNum, L7_INTF_PARM_SPEED))
    {
      rc = L7_FAILURE;
    }
    NIM_CRIT_SEC_READ_EXIT();

    newintfSpeed = intfSpeed;
    if (cnfgrIsFeaturePresent(L7_NIM_COMPONENT_ID,L7_NIM_AUTONEG_WITH_SPEED_DUPLEX_FEATURE_ID) == L7_TRUE)
    {
      if (nimConvertoOldSpeedvalue(intIfNum, newintfSpeed, &intfSpeed)!= L7_SUCCESS)
         return rc;
    }

    if (rc == L7_SUCCESS)
    {
      switch (nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType)
      {
        case L7_IANA_FAST_ETHERNET:
          if (!(intfSpeed == L7_PORTCTRL_PORTSPEED_FULL_100TX ||
                intfSpeed == L7_PORTCTRL_PORTSPEED_HALF_100TX ||
                intfSpeed == L7_PORTCTRL_PORTSPEED_FULL_10T   ||
                intfSpeed == L7_PORTCTRL_PORTSPEED_HALF_10T))
          {
            rc = (L7_FAILURE);
          }
          break;

        case L7_IANA_FAST_ETHERNET_FX:
          if (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_100FX)
          {
            rc = (L7_FAILURE);
          }
          break;

        case L7_IANA_GIGABIT_ETHERNET:
          rc = nimGetIntfPhyCapability(intIfNum, &portCapability);

          if (((portCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_PHY_CAP_PORTSPEED_AUTO_NEG) &&
              (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_1000SX) && (rc != L7_SUCCESS))
          {
            rc = (L7_FAILURE);
          }
          break;

        /* PTin added: Speed 2.5G */
        case L7_IANA_2G5_ETHERNET:
          rc = nimGetIntfPhyCapability(intIfNum, &portCapability);

          if (((portCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_PHY_CAP_PORTSPEED_AUTO_NEG) &&
              (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_2P5FX) && (rc != L7_SUCCESS))
          {
            rc = (L7_FAILURE);
          }
          break;
        /* PTin end */

        case L7_IANA_10G_ETHERNET:
          if (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_10GSX)
          {
            return(L7_FAILURE);
          }
          break;

        /* PTin added: Speed 40G */
        case L7_IANA_40G_ETHERNET:
          rc = nimGetIntfPhyCapability(intIfNum, &portCapability);

          if (((portCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_PHY_CAP_PORTSPEED_AUTO_NEG) &&
              (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_40G_KR4) && (rc != L7_SUCCESS))
          {
            rc = (L7_FAILURE);
          }
          break;

        /* PTin added: Speed 100G */
        case L7_IANA_100G_ETHERNET:
          rc = nimGetIntfPhyCapability(intIfNum, &portCapability);

          if (((portCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_PHY_CAP_PORTSPEED_AUTO_NEG) &&
              (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_100G_BKP) && (rc != L7_SUCCESS))
          {
            rc = (L7_FAILURE);
          }
          break;
        /* PTin end */

        default:
          rc = (L7_FAILURE);
      break;
      }

      if (rc == L7_SUCCESS)
      {
    NIM_CRIT_SEC_WRITE_ENTER();

    if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed != newintfSpeed )
    {
      nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed = newintfSpeed;

      nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
    }

        if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.negoCapabilities != 0)
        {
          rc = (L7_SUCCESS);

      NIM_CRIT_SEC_WRITE_EXIT();
        }
        else
        {
      NIM_CRIT_SEC_WRITE_EXIT();

          if (dtlIntfSpeedSet(intIfNum, intfSpeed) == L7_SUCCESS)
          {
            rc = (nimNotifyIntfChange(intIfNum, L7_SPEED_CHANGE));
          }
          else
          {
            rc = (L7_FAILURE);
          }
        }

      }
    } /* interface present */

  }

  return(rc);
}

/*********************************************************************
* @purpose  Sets the default speed of the specified interface -- NO it doesn't
*           This function really parameter checks the speed asked for vs. port
*           capabilities.
*
* @param    intIfNum    Internal Interface Number
* @param    intfSpeed   Interface Speed,
*                       (@b{  L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX,
*                             L7_PORTCTRL_PORTSPEED_UNKNOWN})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    Unlike nimSetIntfSpeed(), this function does not
*           call DTL to actually set the speed of the interface.
*           It only sets the ifSpeed field in the NIM port config structure.
*
* @end
*********************************************************************/
L7_RC_t nimSetDefaultIntfSpeed(L7_uint32 intIfNum, L7_uint32 intfSpeed)
{
  L7_uint32 phyCapability;
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS && !NIM_PARM_CANSET(intIfNum, L7_INTF_PARM_SPEED))
    {
      rc = L7_FAILURE;
    }
    NIM_CRIT_SEC_READ_EXIT();

    if (rc == L7_SUCCESS)
    {
      switch (nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType)
      {
        case L7_IANA_FAST_ETHERNET:
          if (!(intfSpeed == L7_PORTCTRL_PORTSPEED_FULL_100TX ||
                intfSpeed == L7_PORTCTRL_PORTSPEED_HALF_100TX ||
                intfSpeed == L7_PORTCTRL_PORTSPEED_FULL_10T   ||
                intfSpeed == L7_PORTCTRL_PORTSPEED_HALF_10T))
          {
            rc = (L7_FAILURE);
          }
          break;

        case L7_IANA_FAST_ETHERNET_FX:
          if (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_100FX)
          {
            rc = (L7_FAILURE);
          }
          break;

        case L7_IANA_GIGABIT_ETHERNET:
          rc = nimGetIntfPhyCapability(intIfNum, &phyCapability);

          if ((rc != L7_SUCCESS) && (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_1000SX) &&
              ((phyCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_PHY_CAP_PORTSPEED_AUTO_NEG))
          {
            rc = (L7_FAILURE);
          }
          break;

        /* PTin added: Speed 2.5G */
        case L7_IANA_2G5_ETHERNET:
          rc = nimGetIntfPhyCapability(intIfNum, &phyCapability);

          if ((rc != L7_SUCCESS) && (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_2P5FX) &&
              ((phyCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_PHY_CAP_PORTSPEED_AUTO_NEG))
          {
            rc = (L7_FAILURE);
          }
          break;
        /* PTin end */

        case L7_IANA_10G_ETHERNET:
          if (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_10GSX)
          {
            return(L7_FAILURE);
          }
          break;

        /* PTin added: Speed 40G */
        case L7_IANA_40G_ETHERNET:
          rc = nimGetIntfPhyCapability(intIfNum, &phyCapability);

          if ((rc != L7_SUCCESS) && (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_40G_KR4) &&
              ((phyCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_PHY_CAP_PORTSPEED_AUTO_NEG))
          {
            rc = (L7_FAILURE);
          }
          break;

        /* PTin added: Speed 100G */
        case L7_IANA_100G_ETHERNET:
          rc = nimGetIntfPhyCapability(intIfNum, &phyCapability);

          if ((rc != L7_SUCCESS) && (intfSpeed != L7_PORTCTRL_PORTSPEED_FULL_100G_BKP) &&
              ((phyCapability & L7_PHY_CAP_PORTSPEED_AUTO_NEG) != L7_PHY_CAP_PORTSPEED_AUTO_NEG))
          {
            rc = (L7_FAILURE);
          }
          break;
        /* PTin end */

        default:
          rc = (L7_FAILURE);
          break;

      }

      if (rc == L7_SUCCESS)
      {
    NIM_CRIT_SEC_WRITE_ENTER();

    if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed != intfSpeed )
    {
      nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed = intfSpeed;

      nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
    }

    NIM_CRIT_SEC_WRITE_EXIT();
      }

    } /* interface exists */
  }

  return(rc);
}

/*********************************************************************
* @purpose  Sets the management administrative state of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfMgmtAdminState(L7_uint32 intIfNum, L7_uint32 adminState)
{
  L7_RC_t  rc = L7_FAILURE;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum, rc);

    if (rc == L7_SUCCESS)
    {
      if ((adminState == L7_ENABLE) || (adminState == L7_DISABLE))
      {
        if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.mgmtAdminState != adminState )
        {
          nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.mgmtAdminState = adminState;

          nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
        }

        NIM_CRIT_SEC_WRITE_EXIT();
      }
      else
      {
        NIM_CRIT_SEC_WRITE_EXIT();

        NIM_LOG_MSG("adminState %ud is not valid\n",adminState);
        rc = (L7_FAILURE);
      }
    }
  }

  return( rc );
}
/*********************************************************************
* @purpose  Sets the administrative state of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfAdminState(L7_uint32 intIfNum, L7_uint32 adminState)
{
  L7_RC_t  rc = L7_FAILURE;
  L7_uint32 tmpAdminState = adminState;
  L7_uint32 ldIntfNum = intIfNum;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    if (L7_TRUE == dot3adIsLagMember(intIfNum))
    {
      if (L7_SUCCESS != dot3adWhoisOwnerLag(intIfNum, &ldIntfNum))
      {
        return L7_FAILURE;
      }
    }

    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      do
      {
        if ((adminState == L7_ENABLE) || (adminState == L7_DISABLE) || (adminState == L7_DIAG_DISABLE))
        {
          if ( ( L7_ENABLE == adminState ) &&
               ( L7_FALSE  == linkDependencyIntfCanBeEnabled( ldIntfNum ) ) )
          {
            NIM_CRIT_SEC_WRITE_EXIT();
            return( L7_SUCCESS ); /* Do not return an error here -- CLI
                                   * commands will fail. */
          }

          nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.adminState = adminState;

          NIM_CRIT_SEC_WRITE_EXIT();
        }
        else
        {
          NIM_CRIT_SEC_WRITE_EXIT();

          NIM_LOG_MSG("adminState %ud is not valid\n",adminState);
          rc = (L7_FAILURE);
          break; /* goto end of while */
        }


        if (nimIsMacroPort(intIfNum))
        {
          /* As far as the hardware is concerned there is functionally no difference between
           * an interface which is admin disabled and diag disabled. For it both are the same
           * hence change the adminState to L7_DIABLE if it is L7_DIAG_DISABLE
           */
          if (adminState == L7_DIAG_DISABLE)
          {
            adminState = L7_DISABLE;
          }
          rc = dtlIntfAdminStateSet(intIfNum, adminState);
          if ( (tmpAdminState == L7_DIAG_DISABLE) &&
               (dot3adIsLag(intIfNum) == L7_TRUE)
             )
          {
            rc = dot3adAdminModeSet(intIfNum, adminState, L7_FALSE);
          }
        }


        if (rc == L7_SUCCESS)
        {
          if (adminState == L7_ENABLE)
          {
            rc = (nimNotifyIntfChange(intIfNum, L7_PORT_ENABLE));
          }
          else
          {
             if ( (tmpAdminState == L7_DIAG_DISABLE) &&
                  (dot3adIsLag(intIfNum) == L7_TRUE)
                )
             {
               /* DOT3AD should announce the DISABLE event
                  rc will be still success
               */
               break;
             }

            /* As far as rest of the apps are concerned there is no difference between
             * a disabled port and a diag disabled port
             */
            rc = (nimNotifyIntfChange(intIfNum, L7_PORT_DISABLE));
          }
        }

      } while (0);
    }
    else
    {
      NIM_CRIT_SEC_WRITE_EXIT();
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Sets the locally administered address
*           of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    macAddr     Pointer to LAA MAC Address.
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfLAA(L7_uint32 intIfNum, L7_uchar8 *macAddr)
{
  L7_uchar8 laa[6];

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS && !NIM_PARM_CANSET(intIfNum, L7_INTF_PARM_MACADDR))
    {
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {

      memcpy(laa, macAddr, L7_MAC_ADDR_LEN);

      /*****************************************************************************
       *  The locally adminitered MAC address must be a valid locally administered
       *  individual address.  Hence the LSB of the first byte must be 0 to indicate
       *  an individual address, and the next least significant bit must be 1 to
       *  indicate a locally administered address
       *****************************************************************************/
      if ((laa[0] & 0x03) != 0x02)
      {
        NIM_LOG_MSG("NIM: locally administered address is not valid\n");
        rc = (L7_FAILURE);
      }
      else
      {
        memcpy(nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.LAAMacAddr.addr, macAddr, L7_MAC_ADDR_LEN);

        nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
      }
    } /* interface present */

    NIM_CRIT_SEC_WRITE_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Sets either the burned-in or locally administered address
*           of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    addrType    address type,
*                       (@b{  L7_BIA or
*                             L7_LAA})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfAddrType(L7_uint32 intIfNum, L7_uint32 addrType)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS && !NIM_PARM_CANSET(intIfNum, L7_INTF_PARM_MACADDR))
    {
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      if ((addrType == L7_SYSMAC_BIA) || (addrType == L7_SYSMAC_LAA))
      {
    if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.addrType != addrType )
    {
        nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.addrType = addrType;
      nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
    }

        rc = dtlFdbMacAddrTypeSet(intIfNum, addrType);
      }
      else
      {
        NIM_LOG_MSG("NIM: addrtype %ud is not valid\n",addrType);
        rc = (L7_FAILURE);
      }
    } /* interface exists */

    NIM_CRIT_SEC_WRITE_EXIT();
  }

  return(rc);
}


/*********************************************************************
* @purpose  Sets the link trap configuration of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    trapState   trap state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine should call SNMP trap manager routine.  It is
*           here for completeness and to refer the user to that routine.
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfLinkTrap(L7_uint32 intIfNum, L7_uint32 trapState)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS && !NIM_PARM_CANSET(intIfNum, L7_INTF_PARM_LINKTRAP))
    {
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      if ((trapState == L7_ENABLE) || (trapState == L7_DISABLE))
      {
    if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.trapState != trapState )
    {
        nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.trapState = trapState;
      nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
    }
      }
      else
      {
        NIM_LOG_MSG("NIM: trapState %ud is not valid\n",trapState);
        rc = (L7_FAILURE);
      }
    } /* interface present */
    NIM_CRIT_SEC_WRITE_EXIT();
  }

  /* need to fix */
  /* call to snmp trap manager */
  return(rc);
}

/*********************************************************************
* @purpose  Sets the ifAlias name of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    ifAlias     pointer to string containing alias name
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfifAlias(L7_uint32 intIfNum, L7_uchar8 *ifAlias)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {

      memset(( void * )nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifAlias, 0, L7_NIM_IF_ALIAS_SIZE+1);

      if (strlen((L7_uchar8*)ifAlias) <= L7_NIM_IF_ALIAS_SIZE)
        strcpy( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifAlias, (L7_uchar8*)ifAlias );
      else
        strncpy( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifAlias, (L7_uchar8*)ifAlias, (L7_NIM_IF_ALIAS_SIZE) );

      nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;

    }
    NIM_CRIT_SEC_WRITE_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Sets either the system or alias name
*           of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    nameType    name type,
*                       (@b{  L7_SYSNAME or
*                             L7_ALIASNAME})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfNameType(L7_uint32 intIfNum, L7_uint32 nameType)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {

      if ((nameType == L7_SYSNAME) || (nameType == L7_ALIASNAME))
      {
    if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.nameType != nameType )
    {
        nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.nameType = nameType;
      nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
    }
      }
      else
      {
        NIM_LOG_MSG("NIM: nameType %ud is not valid\n",nameType);
        rc = (L7_FAILURE);
      }
    }
    NIM_CRIT_SEC_WRITE_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the physical link state of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    intfSpeed   pointer to Interface Speed,
*                       (@b{  L7_PORTCTRL_PORTSPEED_AUTO_NEG   = 1,
*                             L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                             L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                             L7_PORTCTRL_PORTSPEED_HALF_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_10T,
*                             L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                             L7_PORTCTRL_PORTSPEED_FULL_1000SX,
*                             L7_PORTCTRL_PORTSPEED_UNKNOWN})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfSpeedStatus(L7_uint32 intIfNum, L7_uint32 *intfSpeed)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 nimifSpeed;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      if (nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.negoCapabilities != 0)
      {
        /* need to exit in case the dtl code is using NIM api's */
        NIM_CRIT_SEC_READ_EXIT();
        rc = dtlIntfSpeedGet(intIfNum, intfSpeed);
      }
      else
      {
       if (cnfgrIsFeaturePresent(L7_NIM_COMPONENT_ID,
                                    L7_NIM_AUTONEG_WITH_SPEED_DUPLEX_FEATURE_ID) == L7_TRUE)
       {
         nimifSpeed = (L7_uint32)nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed;
         rc = nimConvertoOldSpeedvalue(intIfNum, nimifSpeed, intfSpeed);

       }
       else
       {
        *intfSpeed = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed;
       }
       NIM_CRIT_SEC_READ_EXIT();
      }
    }
    else
    {
      /* interface not present, let go of resources */
      NIM_CRIT_SEC_READ_EXIT();
    }
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Gets the status of the speed if congigured to auto.
*
* @param intIfNum  L7_uint32 the internal interface number
* @param *val      L7_uint32 pointer to link configured state
*
* @returns  L7_TRUE, if speed is configured to auto
* @returns  L7_FALSE, if other failure
*
* @end
*********************************************************************/
L7_BOOL nimIsIntfSpeedAuto(L7_uint32 intIfNum)
{
  L7_uint32 nimifSpeed = 0;
  L7_BOOL rc;

  if (cnfgrIsFeaturePresent(L7_NIM_COMPONENT_ID,L7_NIM_AUTONEG_WITH_SPEED_DUPLEX_FEATURE_ID) != L7_TRUE)
  {
    return L7_FALSE;
  }
  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    return L7_FALSE;
  }

  NIM_CRIT_SEC_READ_ENTER();
  IS_INTIFNUM_PRESENT(intIfNum,rc);

  if (rc == L7_SUCCESS)
  {
    nimifSpeed = (L7_uint32)nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed;
  }
  NIM_CRIT_SEC_READ_EXIT();

  if((nimifSpeed & L7_PORT_CAPABILITY_SPEED_AUTO) != 0)
  {
    return L7_TRUE;
  }
  else
  {
    return L7_FALSE;
  }
}
/*********************************************************************
* @purpose  Gets the link configured state of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    intfSpeed   pointer to Interface Speed,
*                       (@b{Returns:   L7_PORTCTRL_PORTSPEED_AUTO_NEG,
*                                      L7_PORTCTRL_PORTSPEED_FULL_100TX,
*                                      L7_PORTCTRL_PORTSPEED_HALF_100TX,
*                                      L7_PORTCTRL_PORTSPEED_FULL_10T,
*                                      L7_PORTCTRL_PORTSPEED_HALF_10T,
*                                      L7_PORTCTRL_PORTSPEED_FULL_100FX,
*                                      L7_PORTCTRL_PORTSPEED_FULL_1000SX})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfSpeed(L7_uint32 intIfNum, L7_uint32 *intfSpeed)
{

  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 nimifSpeed;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (cnfgrIsFeaturePresent(L7_NIM_COMPONENT_ID,
                                 L7_NIM_AUTONEG_WITH_SPEED_DUPLEX_FEATURE_ID) == L7_TRUE)
    {
      nimifSpeed = (L7_uint32)nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed;
      rc = nimConvertoOldSpeedvalue(intIfNum, nimifSpeed, intfSpeed);
    }
    else if (rc == L7_SUCCESS)
    {
      *intfSpeed = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed;
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the auto-negotiation admin status of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    intfSpeed   pointer to Auto-negotiation admin status,
*                       (@b{Returns:   L7_ENABLE,
*                                      L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfAutoNegAdminStatus(L7_uint32 intIfNum, L7_uint32 *autoNegAdminStatus)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      if (nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.negoCapabilities == 0)
      {
        *autoNegAdminStatus = L7_DISABLE;
      }
      else
      {
        *autoNegAdminStatus = L7_ENABLE;
      }
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the physical link simplex data rate of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    intfRate    pointer to Interface Data Rate
*                       (@b{Returns: link data rate in Mbps})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The data rate is determined from the ifSpeed encode maintained
*           by NIM.
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfSpeedSimplexDataRate(L7_uint32 intIfNum, L7_uint32 *intfSpeed)
{

  L7_uint32 speedStatus;
  L7_uint32 dataRate=0;
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }

  else
  {

    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    NIM_CRIT_SEC_READ_EXIT();

    if (rc == L7_SUCCESS)
    {
      if ((rc = nimGetIntfSpeedStatus(intIfNum, &speedStatus)) == L7_SUCCESS)
      {

        switch (speedStatus)
        {
          case L7_PORTCTRL_PORTSPEED_FULL_10T:
          case L7_PORTCTRL_PORTSPEED_HALF_10T:
            dataRate = 10;
            break;

          case L7_PORTCTRL_PORTSPEED_FULL_100TX:
          case L7_PORTCTRL_PORTSPEED_HALF_100TX:
          case L7_PORTCTRL_PORTSPEED_FULL_100FX:
            dataRate = 100;
            break;

          case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
            dataRate = 1000;
            break;

          /* PTin added: Speed 2.5G */
          case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:
            dataRate = 2500;
            break;
          /* PTin end */

          case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
            dataRate = 10000;
            break;

          /* PTin added: Speed 40G */
          case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:
            dataRate = 40000;
            break;

          /* PTin added: Speed 100G */
          case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP:
            dataRate = 100000;
            break;
          /* PTin end */

          case L7_PORTCTRL_PORTSPEED_LAG:
            /* rely on dot3ad component to provide data rate of a LAG
             *
             * NOTE: will recursively call this same NIM function, but
             *       for a physical interface instead of a LAG interface
             */
            if (dot3adLagDataRateGet(intIfNum, &dataRate) == L7_SUCCESS)
            {
              /* function always returns full duplex rate, so halve it */
              dataRate /= 2;
            }
            else
            {
              rc = (L7_FAILURE);
            }
            break;

          default:
            rc = (L7_FAILURE);
            break;
        }

        if (rc == L7_SUCCESS)
        {
          *intfSpeed = dataRate;
        }
      }
    } /* interface exists */
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the physical link data rate of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    intfRate    pointer to Interface Data Rate
*                       (@b{Returns: link data rate in Mbps})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The data rate is determined from the ifSpeed encode maintained
*           by NIM.  Full duplex links are reported as having twice the
*           data rate as their half duplex counterparts.
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfSpeedDataRate(L7_uint32 intIfNum, L7_uint32 *intfSpeed)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else if ((rc = nimGetIntfSpeedSimplexDataRate(intIfNum,intfSpeed)) == L7_SUCCESS)
  {
    if (nimIsIntfSpeedFullDuplex(intIfNum) == L7_TRUE)
    {
      *intfSpeed *= 2;
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Indicates whether a physical link is full duplex
*
* @param    intIfNum    Internal Interface Number
*
* @returns  L7_TRUE     if link is full duplex
* @returns  L7_FALSE    if link is half duplex, or if interface is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL nimIsIntfSpeedFullDuplex(L7_uint32 intIfNum)
{
  L7_uint32 speedStatus;
  L7_BOOL rc;
  L7_INTF_TYPES_t  type;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FALSE);
  }
  if (nimGetIntfType(intIfNum, &type) == L7_SUCCESS)
  {
    if (type == L7_LAG_INTF || type == L7_LOOPBACK_INTF ||
        type == L7_TUNNEL_INTF)
    {
      /* Return L7_TRUE for a these interfaces as nim queries the h/w
       * for this interface and the h/w typically returns a failure for
       * lags.  By definition these have to be full duplex.
       */
      return (L7_TRUE);
    }
  }
  else
  {
    return (L7_FALSE);
  }
  if (nimGetIntfSpeedStatus(intIfNum, &speedStatus) != L7_SUCCESS)
  {
    rc = (L7_FALSE);
  }
  else
  {
    switch (speedStatus)
    {
      case L7_PORTCTRL_PORTSPEED_FULL_10T:
      case L7_PORTCTRL_PORTSPEED_FULL_100TX:
      case L7_PORTCTRL_PORTSPEED_FULL_100FX:
      case L7_PORTCTRL_PORTSPEED_FULL_1000SX:
      case L7_PORTCTRL_PORTSPEED_FULL_2P5FX:    /* PTin added: Speed 2.5G */
      case L7_PORTCTRL_PORTSPEED_FULL_10GSX:
      case L7_PORTCTRL_PORTSPEED_FULL_40G_KR4:  /* PTin added: Speed 40G */
      case L7_PORTCTRL_PORTSPEED_FULL_100G_BKP: /* PTin added: Speed 100G */
        rc = (L7_TRUE);
        break;

      default:
        rc = L7_FALSE;
        break;
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Retrieves the duplex value of the port i.e., auto | full | half
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum    Internal Interface Number
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimIntfDuplexGet(L7_uint32 intIfNum, L7_uint32 *duplexvalue)
{
  L7_RC_t rc;
  L7_uint32 nimifSpeed;

  if (cnfgrIsFeaturePresent(L7_NIM_COMPONENT_ID, L7_NIM_AUTONEG_WITH_SPEED_DUPLEX_FEATURE_ID) != L7_TRUE)
     rc = L7_ERROR;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FALSE);
  }

  NIM_CRIT_SEC_READ_ENTER();
  IS_INTIFNUM_PRESENT(intIfNum,rc);

  if (rc == L7_SUCCESS)
  {
    nimifSpeed = (L7_uint32)nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed;
    *duplexvalue = (nimifSpeed & NIM_AUTONEG_FIXED_DUPLEX_MASK);
  }
  NIM_CRIT_SEC_READ_EXIT();

  return rc;
}

/*********************************************************************
* @purpose  Retrieves the speed value of the port
*
* @param UnitIndex  L7_uint32 the unit for this operation
* @param intIfNum    Internal Interface Number
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimIntfSpeedGet(L7_uint32 intIfNum, L7_uint32 *speedvalue)
{
  L7_RC_t rc;
  L7_uint32 nimifSpeed;

  if (cnfgrIsFeaturePresent(L7_NIM_COMPONENT_ID, L7_NIM_AUTONEG_WITH_SPEED_DUPLEX_FEATURE_ID) != L7_TRUE)
     rc = L7_ERROR;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FALSE);
  }

  NIM_CRIT_SEC_READ_ENTER();
  IS_INTIFNUM_PRESENT(intIfNum,rc);

  if (rc == L7_SUCCESS)
  {
    nimifSpeed = (L7_uint32)nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed;
    *speedvalue = (nimifSpeed & NIM_AUTONEG_FIXED_SPEED_MASK);
  }
  NIM_CRIT_SEC_READ_EXIT();

  return rc;
}

/*********************************************************************
* @purpose  Gets the negotiated Tx and Rx pause status of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    rxPauseStatus  L7_TRUE if autonegotiation result allows Rx pause
* @param    txPauseStatus  L7_TRUE if autonegotiation result allows Tx pause
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfPauseStatus(L7_uint32 intIfNum, L7_BOOL *rxPauseStatus, L7_BOOL *txPauseStatus)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      /* need to exit in case the dtl code is using NIM api's */
      NIM_CRIT_SEC_READ_EXIT();
      rc = dtlIntfPauseStateGet(intIfNum, rxPauseStatus, txPauseStatus);
    }
    else
    {
      /* interface not present, let go of resources */
      NIM_CRIT_SEC_READ_EXIT();
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the management administrative state of the specified interface.
*
* @param    intIfNum Internal Interface Number
*
* @param    adminState admin state,
*                       (@b{  L7_ENABLE or
*                             L7_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfMgmtAdminState(L7_uint32 intIfNum, L7_uint32 *adminState)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
/*
    NIM_CRIT_SEC_ENTER();
*/
    IS_INTIFNUM_PRESENT(intIfNum, rc);

    if (L7_SUCCESS == rc)
    {
      *adminState = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.mgmtAdminState;
    }
/*
    NIM_CRIT_SEC_EXIT();
*/
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the admin state of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    adminState  pointer to Admin State,
*                       (@b{Returns: L7_DISABLE,
*                                    L7_ENABLE
*                                    or L7_DIAG_DISABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfAdminState(L7_uint32 intIfNum, L7_uint32 *adminState)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *adminState = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.adminState;
    }
    NIM_CRIT_SEC_READ_EXIT();
  }
  return(rc);
}

/*********************************************************************
* @purpose  Gets the link state of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    linkState   pointer to Link State,
*                       (@b{Returns: L7_UP
*                                    or L7_DOWN})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    A physical port has link up when the PHY has link up.
* @notes    A LAG has link up when at least one of the member ports has link up.
* @notes    A VLAN interface has link up when at least one of the member ports of the VLAN has link up.
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfLinkState(L7_uint32 intIfNum, L7_uint32 *linkState)
{
  L7_uint32 result;
  L7_RC_t rc = L7_FAILURE;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {

      switch (nimCtlBlk_g->nimPorts[intIfNum].sysIntfType)
      {
        case L7_PHYSICAL_INTF:
        case L7_LAG_INTF:
        case L7_LOGICAL_VLAN_INTF:
        case L7_LOOPBACK_INTF:
        case L7_TUNNEL_INTF:
        case L7_WIRELESS_INTF:
        case L7_CAPWAP_TUNNEL_INTF:
          result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->linkStateMask, intIfNum);
          if (result != L7_NIM_UNUSED_PARAMETER)
          {
            *linkState = L7_UP;
          }
          else
          {
            *linkState = L7_DOWN;
          }
          rc = L7_SUCCESS;
          break;

        /* PTin added: virtual ports */
        case L7_VLAN_PORT_INTF:
          *linkState = L7_UP;     /* If interface exists, link is up */
          break;

        default:
          break;
      }
    } /* interface present */

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the active state of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    activeState pointer to Active State,
*                       (@b{Returns: L7_ACTIVE
*                                    or L7_INACTIVE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @comments An interface is considered active if it has link up, is in
*           forwarding state and all other components concur vis the
*           outcall that the interface is in ACTIVE state.
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfActiveState(L7_uint32 intIfNum, L7_uint32 *activeState)
{
  L7_uint32 result;
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = L7_FAILURE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    /* default to inactive */
    *activeState = L7_INACTIVE;

    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->linkStateMask, intIfNum);
      if (result != L7_NIM_UNUSED_PARAMETER)
      {
        result = NIM_INTF_ISMASKBITSET(nimCtlBlk_g->forwardStateMask, intIfNum);
        if (result != L7_NIM_UNUSED_PARAMETER)
        {
          /* return the resource before making the outcall */
      NIM_CRIT_SEC_READ_EXIT();
          /* Hooks for interactions with other components */
          if (((rc = nimGetIntfActiveStateOutcall(intIfNum, &result)) == L7_SUCCESS) &&
              (result == L7_YES))
          {
            *activeState = L7_ACTIVE;
          } /* both forwarding and linkUp */
        }
        else
        {
          /* not forwarding */
      NIM_CRIT_SEC_READ_EXIT();
        }
      }
      else
      {
        /* Not linked, return resource */
    NIM_CRIT_SEC_READ_EXIT();
      }
    }
    else
    {
      /* interface doesn't exist, return resources */
      NIM_CRIT_SEC_READ_EXIT();
    }
  } /* L7_CNFGR_STATE_E */

  return(rc);
}

/*********************************************************************
* @purpose  Gets the active of the specified interface.
*
* @param    intIfNum    @b{(input)}  Internal Interface Number
*
* @returns  NIM interface state
*
* @notes    This is an API for internal function nimUtilIntfStateGet
*
* @end
*********************************************************************/
L7_INTF_STATES_t nimGetIntfState(L7_uint32 intIfNum)
{
    return nimUtilIntfStateGet(intIfNum);
}

/*********************************************************************
*
* @purpose  Gets the max payload length of an L2 frame on a given interface.
*
* @param    intIfNum    Internal Interface Number
* @param    mtuSize     pointer to mtu Size,
*                       (@b{Returns: mtu size})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfMtuSize(L7_uint32 intIfNum, L7_uint32 *mtuSize)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *mtuSize = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ipMtu;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}


/*********************************************************************
* @purpose  Gets the (Layer 2) max frame size of the specified interface
*
* @param    intIfNum       Internal Interface Number
* @param    maxFrameSize   pointer to Max Frame Size,
*                         (@b{Returns: max frame size})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfMaxFrameSize(L7_uint32 intIfNum, L7_uint32 *maxFrameSize)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *maxFrameSize = nimCtlBlk_g->nimPorts[intIfNum].operInfo.frameSize.largestFrameSize;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the configured (Layer 2) max frame size of the specified interface
*
* @param    intIfNum       Internal Interface Number
* @param    maxFrameSize   pointer to Max Frame Size,
*                         (@b{Returns: max frame size})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfConfigMaxFrameSize(L7_uint32 intIfNum, L7_uint32 *maxFrameSize)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum, rc);

    if (rc == L7_SUCCESS)
    {
      *maxFrameSize = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.cfgMaxFrameSize;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Sets the (Layer 2) max frame size of the specified interface
*
* @param    intIfNum       Internal Interface Number
* @param    maxFrameSize   pointer to Max Frame Size,
*                         (@b{Returns: max frame size})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfConfigMaxFrameSize(L7_uint32 intIfNum, L7_uint32 maxFrameSize)
{

  L7_RC_t rc = L7_SUCCESS;

  do
  {
    if (nimPhaseStatusCheck() != L7_TRUE)
    {
      rc = (L7_FAILURE);
      NIM_LOG_MSG("NIM: incorrect phase for operation\n");
      break; /* goto while 0 */
    }
    else
    {
      IS_INTIFNUM_PRESENT(intIfNum, rc);

      if (rc == L7_SUCCESS &&
          !NIM_PARM_CANSET(intIfNum, L7_INTF_PARM_FRAMESIZE))
      {
        rc = L7_FAILURE;
      }

      if (rc != L7_SUCCESS)
        break;  /* goto while 0 due to interface not existing or wrong type */

      if (maxFrameSize < L7_MIN_FRAME_SIZE || maxFrameSize > PLAT_MAX_FRAME_SIZE)
      {
        rc = L7_FAILURE;
        break; /* goto while 0 */
      }

      /* check if it part of lag */
      if (dot3adIsLagActiveMember(intIfNum)  == L7_TRUE)
      {
        NIM_CRIT_SEC_WRITE_ENTER();
        /* just change the configuration value */
        if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.cfgMaxFrameSize != maxFrameSize )
        {
          nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.cfgMaxFrameSize = maxFrameSize;
          nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
        }
        rc = L7_SUCCESS;

        NIM_CRIT_SEC_WRITE_EXIT();

        /* Calculate new limit on IP MTU (even if a LAG member). */
        nimCalcMtuSize(intIfNum);
        break;  /* goto while 0 */
      }

      NIM_CRIT_SEC_WRITE_ENTER();

      if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.cfgMaxFrameSize != maxFrameSize )
      {
        nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.cfgMaxFrameSize = maxFrameSize;
        nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
      }

      NIM_CRIT_SEC_WRITE_EXIT();

      /* Calculate new limit on IP MTU. */
      nimCalcMtuSize(intIfNum);

      if ((rc = dtlIntfMaxFrameSizeSet(intIfNum, maxFrameSize)) != L7_SUCCESS)
        rc = L7_FAILURE;
    }
  } while ( 0 );

  return rc;
}


/*********************************************************************
* @purpose  Gets either the burned-in or locally administered address
*           of the specified interface, as requested
*
* @param    intIfNum    Internal Interface Number
* @param    addrType    address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured
*                       MAC Address
* @param    macAddr     pointer to MAC Address,
*                       (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfAddress(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {

    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      if (addrType == L7_NULL)
      {
        if (nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.addrType == L7_SYSMAC_BIA)
        {
          memcpy(macAddr, nimCtlBlk_g->nimPorts[intIfNum].operInfo.macAddr.addr, L7_MAC_ADDR_LEN);
        }
        else
        {
          memcpy(macAddr, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.LAAMacAddr.addr, L7_MAC_ADDR_LEN);
        }
      }
      else if (addrType == L7_SYSMAC_BIA)
      {
        memcpy(macAddr, nimCtlBlk_g->nimPorts[intIfNum].operInfo.macAddr.addr, L7_MAC_ADDR_LEN);
      }
      else
      {
        memcpy(macAddr, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.LAAMacAddr.addr, L7_MAC_ADDR_LEN);
      }
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/* PTin added: MAC address */
#if 1
/*********************************************************************
* @purpose  Sets a new MAC address to the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    addrType    address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured
*                       MAC Address
* @param    macAddr     pointer to MAC Address,
*                       (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfAddress(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      if (addrType == L7_NULL)
      {
        if (nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.addrType == L7_SYSMAC_BIA)
        {
          memcpy(nimCtlBlk_g->nimPorts[intIfNum].operInfo.macAddr.addr, macAddr, L7_MAC_ADDR_LEN);
        }
        else
        {
          memcpy(nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.LAAMacAddr.addr, macAddr, L7_MAC_ADDR_LEN);
        }
      }
      else if (addrType == L7_SYSMAC_BIA)
      {
        memcpy(nimCtlBlk_g->nimPorts[intIfNum].operInfo.macAddr.addr, macAddr, L7_MAC_ADDR_LEN);
      }
      else
      {
        memcpy(nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.LAAMacAddr.addr, macAddr, L7_MAC_ADDR_LEN);
      }
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Sets a new L3 MAC address to the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    addrType    address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured
*                       MAC Address
* @param    macAddr     pointer to MAC Address,
*                       (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfL3MacAddress(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      if (addrType == L7_NULL)
      {
        if (nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.addrType == L7_SYSMAC_BIA)
        {
          memcpy(nimCtlBlk_g->nimPorts[intIfNum].operInfo.l3MacAddr.addr, macAddr, L7_MAC_ADDR_LEN);
        }
        else
        {
          memcpy(nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.LAAMacAddr.addr, macAddr, L7_MAC_ADDR_LEN);
        }
      }
      else if (addrType == L7_SYSMAC_BIA)
      {
        memcpy(nimCtlBlk_g->nimPorts[intIfNum].operInfo.l3MacAddr.addr, macAddr, L7_MAC_ADDR_LEN);
      }
      else
      {
        memcpy(nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.LAAMacAddr.addr, macAddr, L7_MAC_ADDR_LEN);
      }
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}
#endif

/*********************************************************************
* @purpose  Gets the L3 router MAC Address of the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    addrType    address type requested (L7_SYSMAC_BIA, L7_SYSMAC_LAA,
*                       or L7_NULL) L7_NULL will return currently configured
*                       MAC Address
* @param    macAddr     pointer to MAC Address,
*                       (@b{Returns: 6 byte mac address})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    L3 applications use the router MAC address as the source address.
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfL3MacAddress(L7_uint32 intIfNum, L7_uint32 addrType, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
    return (L7_FAILURE);
  }

  NIM_CRIT_SEC_READ_ENTER();

  IS_INTIFNUM_PRESENT(intIfNum,rc);

  if (rc == L7_SUCCESS)
  {
    if (addrType == L7_NULL)
    {
      if (nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.addrType == L7_SYSMAC_BIA)
      {
        memcpy(macAddr, nimCtlBlk_g->nimPorts[intIfNum].operInfo.l3MacAddr.addr, L7_MAC_ADDR_LEN);
      }
      else
      {
        /* If LAA is configured, use that as router MAC address */
        memcpy(macAddr, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.LAAMacAddr.addr, L7_MAC_ADDR_LEN);
      }
    }
    else if (addrType == L7_SYSMAC_BIA)
    {
      memcpy(macAddr, nimCtlBlk_g->nimPorts[intIfNum].operInfo.l3MacAddr.addr, L7_MAC_ADDR_LEN);
    }
    else
    {
      memcpy(macAddr, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.LAAMacAddr.addr, L7_MAC_ADDR_LEN);
    }
  }

  NIM_CRIT_SEC_READ_EXIT();

  return rc;

}


/*********************************************************************
* @purpose  Gets the address type being used, either the burned-in or
*           locally administered address of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    addrType    address type,
*                       (@b{  Returns: L7_BIA or
*                             L7_LAA})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfAddrType(L7_uint32 intIfNum, L7_uint32 *addrType)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *addrType = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.addrType;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
*
* @purpose  Gets either the system name or alias name
*           of the specified interface, as requested
*
* @param    intIfNum    Internal Interface Number
* @param    nameType    name type requested (L7_SYSNAME, L7_ALIASNAME or L7_NULL)
*                       L7_NULL will return currently configured ifName
* @param    ifName      pointer to Interface Name,
*                       (@b{Returns: NIM_MAX_INTF_NAME byte interface name})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfName(L7_uint32 intIfNum, L7_uint32 nameType, L7_uchar8 *ifName)
{
  L7_RC_t rc = L7_SUCCESS;

  if (ifName == L7_NULL)
  {
      return L7_FAILURE;
  }

  /*
  Note that if nameType == L7_NULL, then the string passed in must be
  at least L7_NIM_IF_ALIAS_SIZE + 1 in length, as that could be the
  longest string copied to the passed-in string, and can be longer
  than L7_NIM_IFNAME_SIZE
  */

  osapiStrncpySafe(ifName, "IntfNameNotFound", L7_NIM_IFNAME_SIZE);

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    /* Don't log bad phase. Failure return should be enough for caller to
     * discover the problem. May actually be done intentionally, for example
     * by app in WMU processing checkpoint data. */

    osapiStrncpySafe(ifName, "NoIntfNameNIMBadPhase", L7_NIM_IFNAME_SIZE);
    rc = (L7_FAILURE);
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      if (nameType == L7_NULL)
      {
        if (nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.nameType == L7_SYSNAME)
        {
          memcpy(ifName, nimCtlBlk_g->nimPorts[intIfNum].operInfo.ifName, L7_NIM_IFNAME_SIZE);
        }
        else if (nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.nameType == L7_SYSNAME_LONG)
        {
          memcpy(ifName, nimCtlBlk_g->nimPorts[intIfNum].operInfo.ifLongName, L7_NIM_IFNAME_SIZE);
        }
        else
        {
          memcpy(ifName, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifAlias, L7_NIM_IF_ALIAS_SIZE+1);
        }
      }
      else if (nameType == L7_SYSNAME)
      {
        memcpy(ifName, nimCtlBlk_g->nimPorts[intIfNum].operInfo.ifName, L7_NIM_IFNAME_SIZE);
      }
      else if (nameType == L7_SYSNAME_LONG)
      {
        memcpy(ifName, nimCtlBlk_g->nimPorts[intIfNum].operInfo.ifLongName, L7_NIM_IFNAME_SIZE);
      }
      else
      {
        memcpy(ifName, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifAlias, L7_NIM_IF_ALIAS_SIZE+1);
      }
    }
    else
    {
        osapiStrncpySafe(ifName, "IntfNotPresent", L7_NIM_IFNAME_SIZE);
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the link trap configuration of the specified interface.
*
* @param    intIfNum    Internal Interface Number
* @param    trapState   pointer to Trap State,
*                       (@b{Returns: L7_DISABLE
*                                    or L7_ENABLE})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    This routine should call SNMP trap manager routine.  It is
*           here for completeness and to refer the user to that routine.
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfLinkTrap(L7_uint32 intIfNum, L7_uint32 *trapState)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *trapState = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.trapState;
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);

}


/*********************************************************************
* @purpose  Get phy capability of the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    macAddr     pointer to phyCapability,
*                       (@b{Returns: L7_PHY_CAPABILITIES_t})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfPhyCapability(L7_uint32 intIfNum, L7_uint32 *phyCapability)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *phyCapability = nimCtlBlk_g->nimPorts[intIfNum].operInfo.phyCapability;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Set phy capability of the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    macAddr     pointer to phyCapability,
*                       (@b{Returns: L7_PHY_CAPABILITIES_t})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetIntfPhyCapability(L7_uint32 intIfNum, L7_uint32 phyCapability)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      nimCtlBlk_g->nimPorts[intIfNum].operInfo.phyCapability = phyCapability;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Get connector type for the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    macAddr     pointer to connectorType,
*                       (@b{Returns: L7_CONNECTOR_TYPE_t})
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIntfConnectorType(L7_uint32 intIfNum, L7_uint32 *connectorType)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *connectorType = nimCtlBlk_g->nimPorts[intIfNum].operInfo.connectorType;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Release an interface from a macroport.
*
* @param    intIfNum      internal interface number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimResetMacroPortAssignment(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      memset(&nimCtlBlk_g->nimPorts[intIfNum].operInfo.macroPort, 0, sizeof( nimMacroPort_t ));
    }

    NIM_CRIT_SEC_WRITE_EXIT();
  }
  return(rc);
}


/*********************************************************************
* @purpose  Assigns an interface to a macroport.
*
* @param    intIfNum      internal interface number
* @param    macroPortIntf pointer to interface type,
*                         (@b{Returns: Internal Interface Type}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimSetMacroPortAssignment(L7_uint32 intIfNum, nimMacroPort_t *macroPortIntf)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(macroPortIntf->macroPort,rc);
    if (rc == L7_SUCCESS)
    {
      IS_INTIFNUM_PRESENT(intIfNum,rc);

      if (rc == L7_SUCCESS &&
          !NIM_PARM_CANSET(intIfNum, L7_INTF_PARM_MACROPORT))
      {
        rc = L7_FAILURE;
      }

      if (rc == L7_SUCCESS)
      {
        bcopy((char *)macroPortIntf, (L7_uchar8*)&nimCtlBlk_g->nimPorts[intIfNum].operInfo.macroPort, (int)sizeof(nimMacroPort_t));
      } /* interface present */
    } /* macroport present */

    NIM_CRIT_SEC_WRITE_EXIT();
  }

  return(rc);
}


/*********************************************************************
* @purpose  Get interface macroport information
*
* @param    intIfNum      internal interface number
* @param    macroPortIntf pointer to interface type,
*                         (@b{Returns: Internal Interface Type}).
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetMacroPortAssignment(L7_uint32 intIfNum, nimMacroPort_t *macroPortIntf)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      bcopy( (L7_uchar8*)&nimCtlBlk_g->nimPorts[intIfNum].operInfo.macroPort,
             (char *)macroPortIntf,(int)sizeof(nimMacroPort_t));
    } /* interface present */

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}


/*********************************************************************
* @purpose  Clear all counter for an intIfNum
*
* @param    intIfNum Internal Interface Number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimClearCounters(L7_uint32 intIfNum)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_ERROR);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {


    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      NIM_CRIT_SEC_WRITE_ENTER();
      nimCtlBlk_g->nimPorts[intIfNum].resetTime = osapiUpTimeRaw();

      NIM_CRIT_SEC_WRITE_EXIT();

      rc = nimResetInterfaceCounters(intIfNum);
      if (rc == L7_SUCCESS)
      {
        rc = nimNotifyIntfChange(intIfNum, L7_PORT_STATS_RESET);
        if (rc != L7_SUCCESS)
        {
            L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
            nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_NIM_COMPONENT_ID,
                    "\nnimClearCounters: failed to notify interface %u, %s stats reset event", intIfNum, ifName);
        }
      }
    }
  }

  return(rc);
}

/*********************************************************************
* @purpose  Obtain time of last reset of counters
*
* @param    intIfNum    Internal Interface Number
* @param    *ts         Pointer to time structure L7_timespec
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimCountersLastResetTime(L7_uint32 intIfNum, L7_timespec *ts)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_ERROR);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      osapiConvertRawUpTime ( osapiUpTimeRaw() - nimCtlBlk_g->nimPorts[intIfNum].resetTime, ts );
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Obtain raw time of last link change, in seconds
*
* @param    intIfNum    Internal Interface Number
* @param    *seconds    Pointer to value to hold time
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetRawLinkChangeTime(L7_uint32 intIfNum, L7_uint32 *seconds)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *seconds = nimCtlBlk_g->nimPorts[intIfNum].linkChangeTime;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }
  return(rc);
}

/*********************************************************************
*
* @purpose  Returns a textual string containing information about the
*           interface
*
* @param    *maxintf  L7_MAX_INTERFACE_COUNT
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t nimGetIfDescr(L7_uint32 intIfNum, L7_uchar8 *descr)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      memcpy(descr, nimCtlBlk_g->nimPorts[intIfNum].operInfo.ifDescr, L7_NIM_INTF_DESCR_SIZE);
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Returns the internal interface type (IANAifType)
*
* @param    intIfNum    internal interface number
* @param    IfType      pointer to type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    The type of interface, distinguished according to
*           the physical/link protocol(s) immediately `below'
*           the network layer in the protocol stack."
*
* @end
*********************************************************************/
L7_RC_t nimGetIfType(L7_uint32 intIfNum, L7_uint32 *IfType)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *IfType = nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }
  return(L7_SUCCESS);
}

#if L7_FEAT_SF10GBT

/*********************************************************************
* @purpose  Returns the interface transceiver firmware revision
*
* @param    UnitIndex    unit for operation
* @param    intIfNum    internal interface number
* @param    ifFWRevision      pointer to FW revision
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist, or operation not supported
* @returns  L7_FAILURE  if other failure
*
* @notes    Is valid only for 10G non-stacking ports with
*           Solarflare SFX7101 transceiver
*           This function works also for stacking configuration,
*           i.e. can obtain FW revision from port on slave unit
*
* @end
*********************************************************************/
L7_RC_t nimGetIfTransceiverFwRevision(L7_uint32 UnitIndex, L7_uint32 intIfNum, nimIntfPortFWRev_t* ifFWRevision)
{
  L7_RC_t rc = L7_FAILURE;
  usmDbUnitMgrStatus_t status;
  nimHpcMsgFwGetRevision_t msg;
  L7_uint32    local_unit_number;

  bzero((void*)&msg, sizeof(msg));

  (void)usmDbUnitMgrUnitStatusGet(UnitIndex, &status);
  if (status == L7_USMDB_UNITMGR_UNIT_OK)
  {
      usmDbUnitMgrNumberGet(&local_unit_number);
      if ((cliIsStackingSupported() == L7_TRUE) && (UnitIndex != local_unit_number))
      {
        nimUSP_t usp;

        msg.hdr.msgFamily = NIM_HPC_FIRMWARE;
        msg.hdr.msgID     = NIM_HPC_FW_GET_REVISION;
        msg.hdr.bResponse = L7_FALSE;
        msg.hdr.result    = L7_FAILURE;
        msg.hdr.srcUnit   = local_unit_number;
        msg.hdr.targetUnit = UnitIndex;

        if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
        {
            return L7_FAILURE;
        }

        msg.hdr.port  = usp.port;

        if( (rc = sysapiHpcMessageSend(L7_NIM_COMPONENT_ID, UnitIndex, sizeof(msg), (L7_uchar8*)&msg)) == L7_SUCCESS)
        {
          /* receive response */
          if ( (rc = osapiMessageReceive(nimFwRspQ, &msg,sizeof(msg),NIM_HPC_FWMSG_WAIT)) != L7_SUCCESS)
          {
            rc = L7_FAILURE;
          }
          else
          {
            rc = msg.hdr.result;
            if (rc == L7_SUCCESS)
            {
              memcpy(ifFWRevision, &msg.ifFWRevision, sizeof(nimIntfPortFWRev_t));
            }
          }
        } else
        {
            rc = L7_FAILURE;
        }

      }
      else
      {
          DAPI_USP_t           ddUsp;
          nimUSP_t             usp;

          if ( (rc = nimGetUnitSlotPort( intIfNum, &usp )) == L7_SUCCESS  )
          {
            ddUsp.unit = usp.unit;
            ddUsp.slot = usp.slot;
            ddUsp.port = usp.port - 1;

            rc = hpcSF_GetFirmwareRevision(
                                          &ddUsp,
                                          ifFWRevision->ifFwPartNumber,
                                          ifFWRevision->ifFwRevision);
          }
      }
  }

  return rc;
}
#endif /* L7_FEAT_SF10GBT */

/*********************************************************************
* @purpose  Returns the internal interface physical type
*
* @param    intIfNum    internal interface number
* @param    IfType      pointer to type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetIfPhyType(L7_uint32 intIfNum, L7_uint32 *IfType)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *IfType = nimCtlBlk_g->nimPorts[intIfNum].operInfo.ianaType;
    }

    NIM_CRIT_SEC_READ_EXIT();

  }

  return(rc);
}

/*********************************************************************
* @purpose  Set the interface in loopback mode.
*
* @param    intIfNum    Internal Interface Number
* @param    mode        link state
*                       (@b{  L7_PORTCTRL_LOOPBACK_MAC,
*                             L7_PORTCTRL_LOOPBACK_PHY,
*                             L7_PORTCTRL_LOOPBACK_NONE} }
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
*
* @notes    None.
*
* @end
*********************************************************************/
L7_RC_t nimSetLoopbackMode(L7_uint32 intIfNum, L7_uint32 mode)
{
  /* Invoke nimLinkChangeCallback */
  nimUSP_t usp;
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);


    if (rc == L7_SUCCESS &&
        !NIM_PARM_CANSET(intIfNum, L7_INTF_PARM_LOOPBACKMODE))
    {
      rc = L7_FAILURE;
    }
    NIM_CRIT_SEC_READ_EXIT();

    if (rc == L7_SUCCESS)
    {

      if (dtlIntfLoopbackModeSet(intIfNum, mode, usp) == L7_SUCCESS )
      {
    NIM_CRIT_SEC_WRITE_ENTER();
        nimCtlBlk_g->nimPorts[intIfNum].currentLoopbackState = mode;
    NIM_CRIT_SEC_WRITE_EXIT();
      }
      else
      {
        rc = L7_FAILURE;
      }
    } /* interface present */
  }
  return(rc);
}

/*********************************************************************
* @purpose  Get the interface in loopback mode.
*
* @param    intIfNum    Internal Interface Number
* @param    *mode       mode
*                       (@b{  L7_PORTCTRL_LOOPBACK_MAC,
*                             L7_PORTCTRL_LOOPBACK_PHY,
*                             L7_PORTCTRL_LOOPBACK_NONE} }
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
*
* @notes    In release 1, mode will be L7_ENABLE or L7_DISABLE.
*
* @end
*********************************************************************/
L7_RC_t nimGetLoopbackMode(L7_uint32 intIfNum, L7_uint32 *mode)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    /* Invoke nimLinkChangeCallback */
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *mode =  nimCtlBlk_g->nimPorts[intIfNum].currentLoopbackState;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }
  return(L7_SUCCESS);
}

/*--------------------------------------------------------------------------*/


/*********************************************************************
* @purpose  return the highest assigned inteface number
*
* @param    highestIntfNumber   pointer to return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t nimGetHighestIntfNumber(L7_uint32 *highestIntfNumber)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    *highestIntfNumber = 0;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    *highestIntfNumber = nimCtlBlk_g->nimHighestIntfNumber;
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Gets the broadcast MAC address for an interface
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *bcastAddr  @b{(output)}Broadcast MAC address
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimGetBroadcastMACAddr(L7_uint32 intIfNum, L7_uchar8 *bcastAddr)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      memcpy(bcastAddr, nimCtlBlk_g->nimPorts[intIfNum].operInfo.bcastMacAddr.addr, L7_MAC_ADDR_LEN);
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}
/*********************************************************************
* @purpose  Gets the encapsulation type
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    *encapType  @b{(output)}Encapsulation Type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimEncapsulationTypeGet(L7_uint32 intIfNum, L7_uint32 *encapType)
{
  L7_RC_t rc = L7_ERROR;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS)
    {
      *encapType = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.encapsType;
    }
    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}


/*********************************************************************
* @purpose  Sets the encapsulation type
*
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    encapType  @b{(output)}Encapsulation Type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nimEncapsulationTypeSet(L7_uint32 intIfNum, L7_uint32 encapType)
{

  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc == L7_SUCCESS && !NIM_PARM_CANSET(intIfNum, L7_INTF_PARM_ENCAPTYPE))
    {
      rc = L7_FAILURE;
    }

    if (rc == L7_SUCCESS)
    {
      if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.encapsType != encapType )
      {
      nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.encapsType = encapType;
      nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
      }
      /* release resource before going to ipmap */
      NIM_CRIT_SEC_WRITE_EXIT();

      rc = ipMapRtrIntfEncapsTypeSet( intIfNum,  encapType);
    }
    else
    {
      rc = L7_ERROR;
      /* interface doesnt exist, release resource */
      NIM_CRIT_SEC_WRITE_EXIT();
    }
  }

  if (rc == L7_SUCCESS)
  {
    /* Changing the encaps type could change the IP MTU. Recalc the IP MTU. */
    nimCalcMtuSize(intIfNum);
  }

  return(rc);
}

/*********************************************************************
* @purpose  This function provides information about a component.
*           The use of this function is optional.
*
* @param    pQueryData   - @b{(inputoutput)}pointer to data where the
*                                           component will place
*                                           the requested information
*                                           for a component.
*
* @returns  L7_SUCCESS   - function completed succesfully. pQueryData
*                          contains valid information
* @returns  L7_ERROR     - function failed. pQueryData contain reason
*                        - for failure.
*
* @notes    The following are valid error reason code:
* NIM_ERR_RC_INVALID_INTF
* NIM_ERR_RC_INTERNAL       : Could be fatal
* NIM_ERR_RC_INVALID_DATA
* NIM_ERR_RC_INVALID_RSQT
*
* @notes    This function excecute synchronously.
*
* @end
*********************************************************************/
L7_RC_t nimIntfQuery(L7_NIM_QUERY_DATA_t *pQueryData)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_INTF_STATES_t state;

  if (pQueryData == L7_NULLPTR)
  {
    rc = L7_FAILURE;
    /* cannot set pQueryData->reason = NIM_ERR_RC_INVALID_DATA since ptr is null */
    NIM_LOG_MSG("NIM: Invalid data passed to nimIntfQueury\n");
  }
  else if (nimPhaseStatusCheck() != L7_TRUE)
  {
    /* should not get here */
    pQueryData->reason = NIM_ERR_RC_INTERNAL;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
    rc = (L7_FAILURE);
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();

    if ((pQueryData->intIfNum > 0) && (pQueryData->intIfNum <= platIntfTotalMaxCountGet()))
    {
      state = nimCtlBlk_g->nimPorts[pQueryData->intIfNum].intfState;

      switch (pQueryData->request)
      {
        case L7_NIM_QRY_RQST_STATE:
          pQueryData->data.state = state;
          break;

        case L7_NIM_QRY_RQST_PRESENT:
          if ((state == L7_INTF_ATTACHED)  ||
              (state == L7_INTF_CREATED)   ||
              (state == L7_INTF_ATTACHING) ||
              (state == L7_INTF_DETACHING))
          {
            pQueryData->data.present = L7_TRUE;
          }
          else
            pQueryData->data.present = L7_FALSE;
          break;

        default:
          rc = L7_FAILURE;
          pQueryData->reason = NIM_ERR_RC_INVALID_RQST;
          break;
      }
    }
    else
    {
      rc = L7_FAILURE;
      pQueryData->reason = NIM_ERR_RC_INVALID_INTF;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(rc);
}

/*********************************************************************
* @purpose  Deteremine whether a set of parameters are all settable
*           for a given interface.
*
* @param    intIfNum      @b{(input)}  Internal Interface Number
* @param    setParms      @b{(input)}  Mask of L7_INTF_PARM_TYPES_t values
*
* @returns  L7_TRUE  - All parameters are settable.
*   @returns  L7_FALSE - At least one parameter is not settable.
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL nimIntfParmCanSet(L7_uint32 intIfNum, L7_uint32 setParms)
{
  L7_BOOL canset = L7_TRUE;
  L7_RC_t rc;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    canset = L7_FALSE;
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_READ_ENTER();
    IS_INTIFNUM_PRESENT(intIfNum,rc);

    if (rc != L7_SUCCESS || !NIM_PARM_CANSET(intIfNum, setParms))
    {
      canset = L7_FALSE;
    }

    NIM_CRIT_SEC_READ_EXIT();
  }

  return(canset);
}

/*********************************************************************
*
* @purpose  Sets a textual string containing information about the
*           interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t nimIfDescrSet(L7_uint32 intIfNum, L7_uchar8 *descr)
{
  L7_RC_t rc = L7_SUCCESS;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    rc = (L7_FAILURE);
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
  }
  else
  {
    NIM_CRIT_SEC_WRITE_ENTER();

    IS_INTIFNUM_PRESENT(intIfNum, rc);

    if (rc == L7_SUCCESS)
    {
      memcpy(nimCtlBlk_g->nimPorts[intIfNum].operInfo.ifDescr, descr, L7_NIM_INTF_DESCR_SIZE);
    }

    NIM_CRIT_SEC_WRITE_EXIT();
  }
  return rc;
}

/***********************************************************************
* @purpose  Get auto-negotiation status and the negotiation capabilities
*           of the specified interface
*
* @param    intIfNum    Internal Interface Number
* @param    negotiation pointer to negotiation capabilities
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist or not a physical interface
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*******************************************************************************/
L7_RC_t nimGetIntfAutoNegoStatusCapabilities(L7_uint32 intIfNum, L7_uint32 *negotiation)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 type = L7_MAX_INTF_TYPE_VALUE;

  /* Is the interface ready to process the requests? */
  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
    return L7_FAILURE;
  }

  /* Is the interface type physical? Auto-negotiation applies to physical
     interfaces only */
  if (nimGetIntfType(intIfNum, &type) == L7_SUCCESS)
  {
    if (type >= L7_MAX_PHYS_INTF_VALUE)
    {
      NIM_LOG_MSG("NIM: Auto-negotiation does not apply to non-physical interfaces.\n");
      return L7_ERROR;
    }
  }
  else
  {
    NIM_LOG_MSG("NIM: Failed to get the interface type.\n");
    return L7_FAILURE;
  }

  /* Now get the value */
  NIM_CRIT_SEC_READ_ENTER();
  IS_INTIFNUM_PRESENT(intIfNum,rc);

  if (rc == L7_SUCCESS)
  {
    *negotiation = nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.negoCapabilities;
  }
  NIM_CRIT_SEC_READ_EXIT();

  return(rc);
}

/*******************************************************************************
* @purpose  Enable/disable auto-negotiation and set the negotiation capabilities
*           of the specified interface
*
* @param    intIfNum           Internal Interface Number
* @param    negoCapabilities   negotiation capabilities
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist or not a physical interface
* @returns  L7_FAILURE  if other failure
*
* @notes    none
*
* @end
*********************************************************************************/
L7_RC_t nimSetIntfAutoNegoStatusCapabilities(L7_uint32 intIfNum, L7_uint32 negoCapabilities)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 type = L7_MAX_INTF_TYPE_VALUE;

  /* Is the interface ready to process the requests? */
  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    NIM_LOG_MSG("NIM: incorrect phase for operation\n");
    return L7_FAILURE;
  }

  /* Is the interface type physical? Auto-negotiation applies to physical
     interfaces only */
  if (nimGetIntfType(intIfNum, &type) == L7_SUCCESS)
  {
    if (type >= L7_MAX_PHYS_INTF_VALUE)
    {
      NIM_LOG_MSG("NIM: Auto-negotiation does not apply to non-physical interfaces.\n");
      return L7_ERROR;
    }
  }
  else
  {
    NIM_LOG_MSG("NIM: Failed to get the interface type.\n");
    return L7_FAILURE;
  }

  if (nimIntfParmCanSet(intIfNum, L7_INTF_PARM_AUTONEG) != L7_TRUE)
  {
    return L7_FAILURE;
  }
  /* Now set the value */

  NIM_CRIT_SEC_READ_ENTER();
  /* check if interface is present */
  IS_INTIFNUM_PRESENT(intIfNum,rc);
  NIM_CRIT_SEC_READ_EXIT();

  if (rc == L7_SUCCESS)
  {
    NIM_CRIT_SEC_WRITE_ENTER();
    if ( nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.negoCapabilities != negoCapabilities )
    {
      nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.negoCapabilities = negoCapabilities;
      nimCtlBlk_g->nimConfigData->cfgHdr.dataChanged = L7_TRUE;
    }
    NIM_CRIT_SEC_WRITE_EXIT();

    /* Set the negotiation capabilities in the hardware and enable/disable auto-negotiation. */
    rc = dtlIntfNegotiationCapabilitiesSet(intIfNum, negoCapabilities);

    if (rc != L7_SUCCESS)
    {
      NIM_LOG_MSG("NIM: Failed to set the auto-negotiation for interface %d.\n", intIfNum);
      return L7_FAILURE;
    }

    if (negoCapabilities == 0)
    {
      /* Auto-negotiation is turned off. Set the speed to the configured value. */
      rc = nimSetIntfSpeed(intIfNum, nimCtlBlk_g->nimPorts[intIfNum].configPort.cfgInfo.ifSpeed);
    }
  }
  return(rc);
}
