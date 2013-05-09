
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
*
* @filename     captive_portal_main.c
*
* @purpose      Captive portal main functions
*
* @component    captive portal
*
* @comments     none
*
* @create       06/25/2005
*
* @author       dcaugherty
*
* @end
*
**********************************************************************/

#include <string.h>

#include "log.h"
#include "sysapi.h"
#include "intf_cb_api.h"
#include "captive_portal_common.h"
#include "captive_portal_cnfgr.h"
#include "captive_portal_config.h"
#include "captive_portal_debug.h"
#include "cpcm_api.h"
#include "cpdm_api.h"
#include "cpdm_connstatus_api.h"
#include "cpdm_web_api.h"
#include "cpim_api.h"
#include "cpdm_clustering_api.h"
#include "clustering_commdefs.h"
#include "cp_cluster_msg.h"
#include "clustering_api.h"
#include "nimapi.h"
#include "simapi.h"
#include "l7_ip_api.h"

#ifdef L7_NSF_PACKAGE
#include "cp_ckpt.h"
#endif

extern cpdmOprData_t *cpdmOprData;
extern avlTree_t      peerSwTree;

/* Syntactic sugar */
#define SEMA_TAKE(access)   \
  if ( CPDM_##access##_LOCK_TAKE(cpdmSema, L7_WAIT_FOREVER) != L7_SUCCESS)  LOG_ERROR(0);
#define SEMA_GIVE(access)   \
  if ( CPDM_##access##_LOCK_GIVE(cpdmSema) != L7_SUCCESS)  LOG_ERROR(0);

#define CAPTIVE_PORTAL_QUEUE_WAIT_INTERVAL   10000 /* in milliseconds */

/* Using "camel case" for publicly exported functions */

L7_int32    captivePortalTaskId(void);
L7_RC_t     captivePortalTaskStart(void);
L7_BOOL     captivePortalTaskActive(void);
L7_int32    captivePortalStackSize(void);
L7_int32    captivePortalTaskPriority(void);
L7_int32    captivePortalTaskSlice(void);

/* Functions that ONLY this module should ever call (usually in the context
 * of the CP task).
 */

extern L7_RC_t cpimIntfFinishWork(void);
extern void    cpdmMonitorSessions(void);
extern L7_BOOL captivePortalCnfgrComplete(void);


static int       captive_portal_exitflag   = 0; /* may need to be volatile? */

static L7_int32  cpTaskId                  = L7_ERROR;
static char      *captive_portal_task_name = "tCptvPrtl";

void             *pCaptivePortalMsgQueue   = L7_NULLPTR;

void             *pCPStatsQ                = L7_NULLPTR;

static L7_RC_t captive_portal_init(void);
static void    captive_portal_dolisten(void);


/*********************************************************************
* @purpose  This function returns the task ID of the Captive
*           Portal task.
*
* @param    void
*
* @returns  L7_ERROR   - If task does not exist, could
*                        not be started, or initialization failed.
* @returns  L7_int32 representing task ID (duh) otherwise.
*
* @end
*********************************************************************/

L7_int32
captivePortalTaskId(void)
{
  return cpTaskId;
}


/*********************************************************************
* @purpose  This function returns the stack size of the Captive
*           Portal task.
*
* @param    void
*
* @returns  L7_int32 representing stack size
*
* @end
*********************************************************************/

L7_int32    captivePortalStackSize(void)
{
  return L7_DEFAULT_STACK_SIZE * 2 ;
}


/*********************************************************************
* @purpose  This function returns the priority of the Captive
*           Portal task.
*
* @param    void
*
* @returns  L7_int32 representing priority
*
* @end
*********************************************************************/

L7_int32    captivePortalTaskPriority(void)
{
  return L7_DEFAULT_TASK_PRIORITY ;
}


/*********************************************************************
* @purpose  This function returns the task slice of the Captive
*           Portal task.
*
* @param    void
*
* @returns  L7_int32 representing task slice
*
* @end
*********************************************************************/

L7_int32    captivePortalTaskSlice(void)
{
  return L7_DEFAULT_TASK_SLICE ;
}


/*********************************************************************
* @purpose  This function is the main message loop of the Captive
*           Portal task.
*
* @param    void
*
* @returns  void
*
* @end
*********************************************************************/

/* First, some macros for dealing with the wrapping of unsigned integer
 * time values.  We know that our timer values have wrapped (that is,
 * our current time is very large and our wake-up time is very small)
 * if the sign bit of one value differs from that of the other value.
 * In other words, if our current time and next time were expressed as
 * signed values, they would "straddle" zero.  The next macro offers
 * a test for this condition.
 *
 * For the record, you can test these macros and the values they
 * manipulate by changing "L7_int32" or "L7_uint32" to "L7_short16"
 * or "L7_ushort16", respectively.  This will reduce the maximum
 * time-of-day value from over 4 million seconds to about 65 seconds.
 *
 */

#define STRADDLES_ZERO(u32_a, u32_b) \
    ( (((L7_int32) (u32_a)) >= 0) != (((L7_short16) (u32_b)) >= 0))

/* Is the later value really later than the earlier one? */
#define IS_LATER(earlier_u32, later_u32) \
    ( STRADDLES_ZERO((earlier_u32), (later_u32)) ? \
         ((later_u32) < (earlier_u32)) : \
         ((earlier_u32) < (later_u32)))

/* Calculate the difference between an earlier and later time,
 * taking the wrap-around effect into account
 */

#define DELTA(old_u32, new_u32) \
    ( (STRADDLES_ZERO((old_u32), (new_u32)) && (new_u32 < old_u32)) ? \
        ( (L7_int32) -1 - (old_u32)) + (new_u32) + 1 : \
        ((new_u32) - (old_u32)))

/*********************************************************************
* @purpose  Handle CP client authentications
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void captivePortalClientAuthHandle(void)
{
  (void) captivePortalTaskWake();
}


static void
captive_portal_dolisten(void)
{
  L7_uint32            sleep_time, curr_time, next_time;
  L7_RC_t              msg_rx_rc = L7_SUCCESS;
  static L7_uchar8     *label = "Captive Portal: ";
  static L7_BOOL       force = L7_TRUE; /* first time through flag */
  CP_CLUSTER_FLAG_t    flag = 0;
  CP_CLUSTER_RESEND_FLAG_t resendFlag = 0;
  L7_enetMacAddr_t     macAddr;
  L7_uint32            cpId; /*, cpMode = L7_DISABLE; */
  L7_uint32            start_time, end_time;

  /* Keep this guy off the call stack in case he gains weight */
  captivePortalTaskMsg msg;

  /* Calculate first wake time, set sleep interval to
   * CAPTIVE_PORTAL_QUEUE_WAIT_INTERVAL here
   */

  curr_time  = next_time = osapiTimeMillisecondsGet();
  next_time += CAPTIVE_PORTAL_QUEUE_WAIT_INTERVAL;

  start_time = osapiUpTimeRaw();
  while (0 == captive_portal_exitflag)
  {
    memset(&msg, 0, sizeof(captivePortalTaskMsg));

    /* Set sleep time to difference between current time and
     * calculated wake time, or to default interval if longer.
     */

    sleep_time = DELTA(curr_time, next_time);

    msg_rx_rc = osapiMessageReceive(pCaptivePortalMsgQueue,
                                    (void *) &msg, sizeof(msg),
                                    sleep_time);

    /* Just in case flag is set while we're waiting for a message.
     * We should quit IMMEDIATELY!
     */
    if (0 != captive_portal_exitflag)
      break;


    /* Real work starts here */
    do
    {
      L7_uint32 mode = L7_DISABLE;
      /* See if any Configurator events are pending for the Captive Portal.  */
      captivePortalCnfgrParse();

      /* Are there CP-capable interfaces out there? */
      if (L7_SUCCESS != cpimIntfFinishWork())
      {
        CP_DLOG(CPD_LEVEL_LOG, "%serror with ntwk intf capabilities\n", label);
        break;
      }

      if (L7_TRUE != captivePortalCnfgrComplete())
      {
          break; /* Not done with initialization yet, so go away */
      }

      /*
       * Don't wait for the Configurator because it may take 15 minutes.
       * This is especially irriating to the CLI user who is show running config.
       */
      if (L7_TRUE == cpConfigHasDataChanged())
      {
        cpConfigSave();
      }

      #ifdef L7_NSF_PACKAGE
      if ((cpdmOprData->cpBackupElected == L7_TRUE) &&
          (cpdmOprData->cpCkptReqd == L7_TRUE) &&
          (cpdmOprData->cpCkptInProg != L7_TRUE))
      {
        cpCkptClientInfoCkpt();
      }
      #endif


      /* If we're not configured to enable CP, leave.  Else, check our
       * Check IP Address.  If we're still enabled after the check, execute
       * other timer task code
       */
      if (L7_SUCCESS != cpdmCfgGlobalModeGet(&mode))
        break;

      if (L7_DISABLE == mode)
      {
        L7_uchar8 junk[8]; /* any size will do */

        /* handle no more transactions until we're enabled: flush the queue */
        cpdmClientConnStatusNextTransPurge();

        /* purge our cluster stats queue here */
        while(L7_SUCCESS == osapiMessageReceive(pCPStatsQ, (void *) &junk, sizeof(junk),
                                                L7_NO_WAIT))
        {   /* repeat until q is empty */
        }
      }


      if (L7_ENABLE == mode)
      {
        L7_CP_MODE_REASON_t  rCode = L7_CP_MODE_REASON_NONE;
        L7_IP_ADDR_t         newAddr = 0, currAddr = 0;
        L7_uint32            newMask = 0, currMask = 0;
        L7_RC_t              rc = L7_FAILURE;
        L7_uchar8            intfMac[L7_ENET_MAC_ADDR_LEN];

        /* Try to refresh our IP address, save new one. */
        (void) cpdmGlobalCPIPAddressGet(&currAddr, &currMask);
        newAddr = currAddr;
        newMask = currMask;

#ifdef L7_ROUTING_PACKAGE
        {
          cpdmGlobalStatus_t * pGS = &(cpdmOprData->cpGlobalStatus);
          L7_uint32 rtrMode, intIfNum = L7_NULL;
          L7_BOOL   loopbackInUse;

          SEMA_TAKE(WRITE);
          intIfNum = pGS->cpIntIfNum;
          loopbackInUse = pGS->cpLoopBackIFInUse;
          rtrMode = pGS->cpRoutingMode;
          rc = cpIPAddressCheck(&newAddr, &newMask, &rtrMode, &intIfNum, &loopbackInUse, &rCode);
          pGS->cpIntIfNum = intIfNum;
          pGS->cpLoopBackIFInUse = loopbackInUse;
          pGS->cpRoutingMode = rtrMode;
          SEMA_GIVE(WRITE);
        }
#else
        rc = cpIPAddressCheck(&newAddr, &newMask, L7_NULLPTR, L7_NULLPTR, L7_NULLPTR, &rCode);
#endif
        if (L7_FAILURE == rc)
        {
          newAddr = (L7_IP_ADDR_t) 0;
          mode = L7_DISABLE;
        }
        cpdmGlobalCPDisableReasonSet(rCode);

        /* TODO: Determine if second condition is really necessary. */
        if ((newAddr != currAddr) || (newMask != currMask) || ((L7_IP_ADDR_t) 0 == newAddr))
        {
          cpdmGlobalCPIPAddressSet(newAddr, newMask);
          /* If our global address changes, take everything down first.
           * We'll re-enable on the next go-round.
           */
          mode = L7_DISABLE;
        }

        /* check interface MAC address */
        if (simGetSystemIPAddr() == newAddr)
        {
            if (simGetSystemIPMacType() == L7_SYSMAC_BIA)
            {
                simGetSystemIPBurnedInMac(intfMac);
            }
            else
            {
                simGetSystemIPLocalAdminMac(intfMac);
            }
        }
        else
        {
#ifdef L7_ROUTING_PACKAGE
            L7_uint32 servIntIfNum;
            if (ipMapRouterIfResolve(newAddr, &servIntIfNum) != L7_SUCCESS)
            {
                break;
            }

            /* Get the MAC address of the interface */
            if (nimGetIntfL3MacAddress(servIntIfNum, L7_NULL, intfMac) != L7_SUCCESS)
            {
                break;
            }
#endif
        }

        if (cpdmGlobalCPMACAddressCheck(&intfMac[0]) != L7_SUCCESS)
        {
            cpdmGlobalCPMACAddressSet(&intfMac[0]);
            mode = L7_DISABLE;
        }
      }

      if (L7_SUCCESS == cpdmGlobalModeApply(mode,force))
      {
        force = L7_FALSE;
      }
      else
      {
         CP_DLOG(CPD_LEVEL_LOG, "Can't apply new mode!?");
      }

      if ((L7_SUCCESS == cpdmGlobalModeGet(&mode)) &&
          (L7_CP_MODE_ENABLED == mode))
      {
        cpcmPrepareToAuthenticate(); /* Check for any prep work for client auth */
        cpdmMonitorSessions();       /* Check on session progress */
        if (L7_SUCCESS != cpcmTryAuthentication())
        {
          CP_DLOG(CPD_LEVEL_LOG, "%sauthentication error\n", label);
          break;
        }
        else
        {
          if (clusterSupportGet() == L7_SUCCESS)
          {
            L7_enetMacAddr_t client, swMac;
            L7_uint32  trType;
            L7_RC_t transRC;

            memset(&client, 0, sizeof(L7_enetMacAddr_t));
            memset(&swMac,  0, sizeof(L7_enetMacAddr_t));
            while (L7_SUCCESS == cpdmClientConnStatusNextTransTypeGet(&trType))
            {
              switch (trType)
              {
                case CP_CONN_NEW:
                  transRC = cpdmClientConnStatusNewGet(&client);
                  break;
                case CP_CONN_DELETE:
                  transRC = cpdmClientConnStatusDeleteGet(&client);
                  break;
                case CP_CONN_DEAUTH:
                  transRC = cpdmClientConnStatusDeauthGet(&swMac, &client);
                  break;
                default:
                  transRC = L7_FAILURE;
                  break;
              }

              if (L7_SUCCESS != transRC)
              {
                CP_DLOG(CPD_LEVEL_LOG, "%s: Transaction type %d either faulty, or queue corrupted!",
                        __FUNCTION__, trType);
                continue;
              }

              /* If we get here, we have the transaction type and data */
              switch (trType)
              {
              case CP_CONN_NEW:
                  {
                      if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
                      {
                        /* send connected client update msg to cluster controller */
                        if (L7_SUCCESS !=
                            cpClusterConnClientsUpdateMsgSend(client, L7_TRUE,
                                                              cpdmOprData->cpCtrlMacAddr))
                        {
                          CP_DLOG(CPD_LEVEL_LOG, "%s: Can't send connected client %02x:%02x:%02x:%02x:%02x:%02x update msg",
                                  __FUNCTION__, client.addr[0], client.addr[1],
                                  client.addr[2], client.addr[3], client.addr[4], client.addr[5]);
                        }
    
                        /* no need to send instance interface update msg to cluster controller here,
                         * it will be sent when instance/interface is enabled. Nor is there a need
                         * to send statistics update msg to cluster controller here, it will be
                         * sent periodically, based on 'peer stats reporting interval'.
                         */
                      }
    
                      if (peerSwTree.count > 0)
                      {
                        /* send client notification msg to peer switches, if any */
                        if (L7_SUCCESS != cpClusterClientNotificationMsgSend(client,L7_TRUE))
                        {
                          CP_DLOG(CPD_LEVEL_LOG, "%s: Can't send client %02x:%02x:%02x:%02x:%02x:%02x notif msg",
                                  __FUNCTION__, client.addr[0], client.addr[1], client.addr[2],
                                  client.addr[3], client.addr[4], client.addr[5]);
                        }
                      }
                  break;
                  }


              case CP_CONN_DELETE:
                  {
                      if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
                      {
                        /* send client delete msg to cluster controller */
                        if (L7_SUCCESS !=
                            cpClusterConnClientsUpdateMsgSend(client, L7_FALSE, cpdmOprData->cpCtrlMacAddr))
                        {
                          CP_DLOG(CPD_LEVEL_LOG, "%s: Can't send client %02x:%02x:%02x:%02x:%02x:%02x delete msg",
                                  __FUNCTION__, client.addr[0], client.addr[1], client.addr[2],
                                  client.addr[3], client.addr[4], client.addr[5]);
                        }
                      }
    
                      /* send client notification msg to peer switches, if any */
                      if (peerSwTree.count > 0)
                      {
                        if (cpClusterClientNotificationMsgSend(client, L7_FALSE) != L7_SUCCESS)
                        {
                          CP_DLOG(CPD_LEVEL_LOG, "%s: Can't send client %02x:%02x:%02x:%02x:%02x:%02x notif msg",
                                  __FUNCTION__, client.addr[0], client.addr[1], client.addr[2],
                                  client.addr[3], client.addr[4], client.addr[5]);
                        }
                      }
    
                      if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
                      {
                        if (cpClusterSwitchClientSwitchMACGet(client, &swMac) != L7_SUCCESS)
                        {
                          CP_DLOG(CPD_LEVEL_LOG, "%s: Can't get peer client switch", __FUNCTION__);
                        }
                        else if (cpClusterSwitchClientEntryDelete(client, swMac) != L7_SUCCESS)
                        {
                          CP_DLOG(CPD_LEVEL_LOG, "%s: Can't delete peer client internally",  __FUNCTION__);
                        }
                      }
                  break;    
                  }


              case CP_CONN_DEAUTH:
                  {
                      while (L7_SUCCESS == cpdmClientConnStatusDeauthGet(&swMac, &client))
                      {
                        if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
                        {
                          /* send controller command to peer switch */
                          if (cpClusterControllerCmdMsgSend(swMac, client, 0,
                                                            0, L7_FALSE) != L7_SUCCESS)
                          {
                            CP_DLOG(CPD_LEVEL_LOG, "%s: Can't send deauth client contrllr cmd.", __FUNCTION__);
                          }
                        }
                      }
                  break;
                  }

                /* The following default: block is compiled out to satisfy coverty */
#if 0
                default: /* Earlier switch statement keeps us from getting here */
                break;
#endif
              }
            }
          }
        }


        if (clusterSupportGet() == L7_SUCCESS)
        {
          if (cpClusterFlagGet(&flag) == L7_SUCCESS)
          {
            if (flag & CP_CLUSTER_PEER_JOINED)
            {
              if (cpClusterClientNotificationMsgPeerSend(cpdmOprData->cpNewPeer) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send client notification msg to peer %02x:%02x:%02x:%02x:%02x:%02x.\n",
                        cpdmOprData->cpNewPeer.addr[0], cpdmOprData->cpNewPeer.addr[1], cpdmOprData->cpNewPeer.addr[2],
                        cpdmOprData->cpNewPeer.addr[3], cpdmOprData->cpNewPeer.addr[4], cpdmOprData->cpNewPeer.addr[5]);
              }
              memset(&cpdmOprData->cpNewPeer, 0, sizeof(L7_enetMacAddr_t));
              if (cpClusterFlagClear(CP_CLUSTER_PEER_JOINED) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear peer joined flag.\n");
              }
            }


            if (flag & CP_CLUSTER_CONTROLLER_ELECTED)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
              {
                /* send connected client update msg to cluster controller */
                memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
                if (cpClusterConnClientsUpdateMsgSend(macAddr, L7_TRUE,
                                                      cpdmOprData->cpCtrlMacAddr) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send connected clients update msg to controller.\n");
                }

                /* send instance interface update msg to cluster controller */
                cpId = 0;
                if (cpClusterInstIntfUpdateMsgSend(cpId, 0, cpdmOprData->cpCtrlMacAddr) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send instance interface update msg.\n");
                }

                /* no need to send statistics update msg to cluster controller here,
                 * it will be sent periodically, based on 'peer stats reporting interval'.
                 */
              }
              if (cpClusterFlagClear(CP_CLUSTER_CONTROLLER_ELECTED) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear controller elected flag.\n");
              }
            }

            if (flag & CP_CLUSTER_INST_STATUS_SEND)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
              {
                /* send instance update to cluster controller */
                if (cpClusterInstIntfUpdateMsgSend(cpdmOprData->cpLocalSw.cpId, 0,
                                                   cpdmOprData->cpCtrlMacAddr) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send cpId %d instance update.\n",
                          cpdmOprData->cpLocalSw.cpId);
                }
              }
              cpdmOprData->cpLocalSw.cpId = 0;
              if (cpClusterFlagClear(CP_CLUSTER_INST_STATUS_SEND) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear instance status send flag.\n");
              }
            }


            if (flag & CP_CLUSTER_INST_INTF_STATUS_SEND)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
              {
                /* send interface update to cluster controller */
                if (cpClusterInstIntfUpdateMsgSend(cpdmOprData->cpLocalSw.cpId, cpdmOprData->cpLocalSw.intf,
                                                   cpdmOprData->cpCtrlMacAddr) != L7_SUCCESS)
                {
                  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
                  nimGetIntfName(cpdmOprData->cpLocalSw.intf, L7_SYSNAME, ifName);

                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send interface %d, %s update.\n",
                          cpdmOprData->cpLocalSw.intf, ifName);
                }
              }
              cpdmOprData->cpLocalSw.cpId = 0;
              cpdmOprData->cpLocalSw.intf = 0;
              if (cpClusterFlagClear(CP_CLUSTER_INST_INTF_STATUS_SEND) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear instance interface status send flag.\n");
              }
            }


            if (flag & CP_CLUSTER_DEAUTH_INST_CLIENTS_SEND)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
              {
                /* send controller command to peer switches */
                memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
                if (cpClusterControllerCmdMsgSend(macAddr, macAddr, cpdmOprData->cpLocalSw.cpId,
                                                  0, L7_FALSE) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send deauth instance %d clients controller command.\n",
                          cpdmOprData->cpLocalSw.cpId);
                }
              }
              cpdmOprData->cpLocalSw.cpId = 0;
              if (cpClusterFlagClear(CP_CLUSTER_DEAUTH_INST_CLIENTS_SEND) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear deauth instance clients send flag.\n");
              }
            }


            if (flag & CP_CLUSTER_DEAUTH_ALL_CLIENTS_SEND)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
              {
                /* send controller command to peer switches */
                memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
                if (cpClusterControllerCmdMsgSend(macAddr, macAddr, 0, 0, L7_FALSE) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to send deauth all clients controller command.", __FUNCTION__);
                }

                if (cpdmClientConnStatusPeerPurge() != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to purge internal map of all clients", __FUNCTION__);
                }

              }
              if (cpClusterFlagClear(CP_CLUSTER_DEAUTH_ALL_CLIENTS_SEND) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "%s: Failed to clear deauth all clients send flag.", __FUNCTION__);
              }
            }


            if (flag & CP_CLUSTER_INST_BLOCK_STATUS_SEND)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
              {
                /* send controller command to peer switches */
                memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
                if (cpClusterControllerCmdMsgSend(macAddr, macAddr, 0, cpdmOprData->cpLocalSw.cpId,
                                                  L7_TRUE) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send instance %d block status controller command.\n",
                          cpdmOprData->cpLocalSw.cpId);
                }
              }
              cpdmOprData->cpLocalSw.cpId = 0;
              if (cpClusterFlagClear(CP_CLUSTER_INST_BLOCK_STATUS_SEND) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear instance block status send flag.\n");
              }
            }


            if (flag & CP_CLUSTER_CLIENT_AUTH_REPLY_SEND)
            {
              /* send client auth reply msg to peer switch */
              if (cpClusterClientAuthReplyMsgSend(cpdmOprData->cpLocalSw.peer,
                                                  cpdmOprData->cpLocalSw.authClient) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send client auth reply msg.\n");
              }
              memset(&cpdmOprData->cpLocalSw.peer, 0, sizeof(L7_enetMacAddr_t));
              memset(&cpdmOprData->cpLocalSw.authClient, 0, sizeof(L7_enetMacAddr_t));
              if (cpClusterFlagClear(CP_CLUSTER_CLIENT_AUTH_REPLY_SEND) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear client auth reply send flag.\n");
              }
            }
          }


          if (cpClusterResendFlagGet(&resendFlag) == L7_SUCCESS)
          {
            if (resendFlag & CP_CLUSTER_RESEND_REQ_CLIENT_NOTIFY)
            {
              if (cpClusterResendMsgSend(CP_CLUSTER_RESEND_CLIENT_NOTIFICATION) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send client notification resend request msg.\n");
              }
              if (cpClusterResendFlagClear(CP_CLUSTER_RESEND_REQ_CLIENT_NOTIFY) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear client notification resend request flag.\n");
              }
            }


            if (resendFlag & CP_CLUSTER_RESEND_REQ_CONN_CLIENTS)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
              {
                if (cpClusterResendMsgSend(CP_CLUSTER_RESEND_CONN_CLIENTS_UPDATE) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send connected clients resend request msg.\n");
                }
              }
              if (cpClusterResendFlagClear(CP_CLUSTER_RESEND_REQ_CONN_CLIENTS) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear connected clients resend request flag.\n");
              }
            }


            if (resendFlag & CP_CLUSTER_RESEND_REQ_INST_INTF)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_CONTROLLER)
              {
                if (cpClusterResendMsgSend(CP_CLUSTER_RESEND_INST_INTF_UPDATE) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send instance interface resend request msg.\n");
                }
              }
              if (cpClusterResendFlagClear(CP_CLUSTER_RESEND_REQ_INST_INTF) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear instance interface resend request flag.\n");
              }
            }


            if (resendFlag & CP_CLUSTER_RESEND_CLIENT_NOTIFY)
            {
              if (cpClusterClientNotificationMsgPeerSend(cpdmOprData->cpNewPeer) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to resend client notification msg to peer %02x:%02x:%02x:%02x:%02x:%02x.\n",
                        cpdmOprData->cpNewPeer.addr[0], cpdmOprData->cpNewPeer.addr[1], cpdmOprData->cpNewPeer.addr[2],
                        cpdmOprData->cpNewPeer.addr[3], cpdmOprData->cpNewPeer.addr[4], cpdmOprData->cpNewPeer.addr[5]);
              }
              memset(&cpdmOprData->cpNewPeer, 0, sizeof(L7_enetMacAddr_t));
              if (cpClusterResendFlagClear(CP_CLUSTER_RESEND_CLIENT_NOTIFY) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear client notification resend flag.\n");
              }
            }


            if (resendFlag & CP_CLUSTER_RESEND_CONN_CLIENTS)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
              {
                memset(&macAddr, 0, sizeof(L7_enetMacAddr_t));
                if (cpClusterConnClientsUpdateMsgSend(macAddr, L7_TRUE,
                                                      cpdmOprData->cpCtrlMacAddr) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to resend connected clients update msg.\n");
                }
              }
              if (cpClusterResendFlagClear(CP_CLUSTER_RESEND_CONN_CLIENTS) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear connected clients resend flag.\n");
              }
            }


            if (resendFlag & CP_CLUSTER_RESEND_INST_INTF)
            {
              if (cpdmOprData->cpLocalSw.status == CP_CLUSTER_SWITCH_MEMBER)
              {
                cpId = 0;
                if (cpClusterInstIntfUpdateMsgSend(cpId, 0, cpdmOprData->cpCtrlMacAddr) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to resend instance interface update msg.\n");
                }
              }
              if (cpClusterResendFlagClear(CP_CLUSTER_RESEND_INST_INTF) != L7_SUCCESS)
              {
                CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear instance interface resend flag.\n");
              }
            }
          }


          /* send client statistics msg to cluster controller */
          switch (cpdmOprData->cpLocalSw.status)
          {
            case CP_CLUSTER_SWITCH_MEMBER:
              end_time = osapiUpTimeRaw();
              if ((end_time - start_time) >= cpdmCfgData->cpGlobalData.peerSwitchStatsReportInterval)
              {
                if (cpClusterStatsUpdateMsgSend(cpdmOprData->cpCtrlMacAddr) != L7_SUCCESS)
                {
                  CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send stats update msg.\n");
                }
                start_time = osapiUpTimeRaw();
              }
              break;
            case CP_CLUSTER_SWITCH_CONTROLLER:
              {
                static cpClusterStatsUpdateMsg_t smsg;

                while (L7_TRUE)
                {
                  memset(&smsg, 0, sizeof(smsg));
                  if (L7_SUCCESS !=
                      osapiMessageReceive(pCPStatsQ, (void *) &smsg, sizeof(smsg),
                                          L7_NO_WAIT))
                    break;
                   cpClusterStatsUpdateMsgProcess(smsg.buf, smsg.msgLen);
                }
              }
              break;
            default:
              break;
          }
        }
      }

      if (clusterSupportGet() == L7_SUCCESS)
      {
        if (cpClusterFlagGet(&flag) == L7_SUCCESS)
        {
          if (flag & CP_CLUSTER_CONFIG_SEND)
          {
            if (cpClusterConfigSyncMsgSend(cpdmOprData->cpCfgPeerTx) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to send config sync msg.\n");
            }
            memset(&cpdmOprData->cpCfgPeerTx, 0, sizeof(L7_enetMacAddr_t));
            if (cpClusterFlagClear(CP_CLUSTER_CONFIG_SEND) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear CP config send flag.\n");
            }
          }

          if (flag & CP_CLUSTER_CONFIG_RX)
          {
            cpClusterConfigSyncCmdProcess(cpdmOprData->cpConfigCmd.cmd);
            if (cpClusterFlagClear(CP_CLUSTER_CONFIG_RX) != L7_SUCCESS)
            {
              CP_DLOG(CPD_LEVEL_LOG, "captive_portal_dolisten: Failed to clear CP config received flag.\n");
            }
          }
        }
      }

    } while(0);

    /* Real work ends here */

    curr_time = next_time = osapiTimeMillisecondsGet();

    next_time += CAPTIVE_PORTAL_QUEUE_WAIT_INTERVAL;
  }
}



/*********************************************************************
* @purpose  This function initializes the private structures of the
*           Captive Portal task, and responds to the asynchronous
*           call that kicked off the creation of the task.
*
* @param    pCBData - pointer to a callback data type
*                     (L7_CNFGR_CB_DATA_t).  This function ASSUMES
*                     that the correlator value within the structure
*                     pointed to is VALID.  There is NOTHING we can
*                     do otherwise!
*
* @returns  L7_ERROR   - If initialization failed.
*
* @end
*********************************************************************/
static L7_RC_t captive_portal_init()
{
  L7_RC_t rc = L7_FAILURE;

  rc = cpNewOprData();
  if (L7_SUCCESS == rc)
  {
    rc = cpNewCfgData();
  }

  return rc;
}



/*********************************************************************
* @purpose  This function is the main entry point of the Captive
*           Portal task, as far as the OS is concerned.
*
*
* @param    int argc - number of arguments
* @param    char *argv[] - array of arguments, argc elements long
*
* @returns  integer - 0 implies error-free return (e.g. graceful
*           shutdown), non-zero value implies an error.
*
* @notes    This function MUST be invoked with 2 arguments, the
*           second of which (that is, argv[1]), MUST be a string
*           representing an unsigned integer.
*           This integer MUST be equal to the correlator used by
*           the configurator to invoke the Captive Portal component's
*           callback function.
*
* @end
*********************************************************************/

static int
captive_portal_main(int argc, char *argv[])
{
  int rc = 0, my_pid;

  osapiTaskInitDone(L7_CAPTIVE_PORTAL_TASK_SYNC);
  my_pid = (int) osapiTaskIdSelf();

  do
  {
    /* Try to initialize the task's internal data */
    if (L7_SUCCESS != captive_portal_init())
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: init failed\n", __FUNCTION__);
      rc = 1;
      break;
    }
    rc = 0;
  } while ( 0 );

  if (0 == rc) {
    captive_portal_dolisten(); /* Wait for new business. */
  }

  CP_DLOG(CPD_LEVEL_LOG, "%s: task 0x%08x exiting (rc=%d)\n", __FUNCTION__, my_pid, rc);
  LOG_ERROR(0xDEADBEEF); /* We should NEVER get here */
  return rc;
}

/*********************************************************************
* @purpose  This function starts the captive portal task once it has
*           initialized its 1-message queue.
*
* @param    ASYNC_CORRELATOR_t   *pReason    @b{(output)}  Reason if L7_ERROR.
*
* @returns  L7_SUCCESS - There were no errors.
* @returns  L7_ERROR   - There were errors. Reason code is available.
*
* @notes    The following are valid error reason codes:
*           L7_CNFGR_ERR_RC_FATAL
*           L7_CNFGR_ERR_RC_LACK_OF_RESOURCES
* @end
*********************************************************************/

L7_RC_t
captivePortalTaskStart(void)

{
  L7_uint32 queueSize;
  L7_RC_t   rc = L7_FAILURE;

  do
  {
    if (L7_ERROR != cpTaskId) /* Task already exists, sorry. */
      break;

    queueSize = (CP_WIO_MSG_COUNT + CP_WLAN_MSG_COUNT + 1);
    pCPStatsQ = (void *)osapiMsgQueueCreate("cpStatsQ",
                                                queueSize, sizeof(cpClusterStatsUpdateMsg_t));

    if (L7_NULLPTR == pCPStatsQ)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: osapiMsgQueueCreate failed", __FUNCTION__);
      break;
    }

    queueSize = 2;
    pCaptivePortalMsgQueue = (void *)osapiMsgQueueCreate("captivePortalMsgQueue",
                                                         queueSize,
                                                         sizeof(captivePortalTaskMsg));
    if (L7_NULLPTR == pCaptivePortalMsgQueue)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: osapiMsgQueueCreate failed", __FUNCTION__);
      break;
    }


    cpTaskId =
        osapiTaskCreate(captive_portal_task_name, captive_portal_main, 1,
                        captive_portal_task_name, captivePortalStackSize(),
                        captivePortalTaskPriority(),
                        captivePortalTaskSlice());

    if (L7_ERROR == cpTaskId)
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: osapiTaskCreate failed.\n", __FUNCTION__);
      LOG_ERROR(0xDEAFBABE);   /* No real point in continuing. */
      break;
    }
    if (osapiWaitForTaskInit(L7_CAPTIVE_PORTAL_TASK_SYNC, L7_WAIT_FOREVER))
    {
      CP_DLOG(CPD_LEVEL_LOG, "%s: wait for task init failed.\n", __FUNCTION__);
      break;
    }
    rc = L7_SUCCESS;
  } while ( 0 );

  return rc;
}




/*********************************************************************
* @purpose  To wake up the Captive Portal task to do immediate work.
*
* @param    void
*
* @returns  L7_RC_t - L7_ERROR or L7_SUCCESS
*
* @end
*********************************************************************/
L7_RC_t
captivePortalTaskWake(void)
{
  /* Keep this guy off the call stack in case he gains weight */

  L7_RC_t rc;


  static captivePortalTaskMsg msg;
  if (L7_NULLPTR == pCaptivePortalMsgQueue)
  {
    return L7_ERROR;
  }
  rc =  osapiMessageSend(pCaptivePortalMsgQueue, &msg, sizeof(msg),
                         L7_NO_WAIT, L7_MSG_PRIORITY_HI);

  return rc;
}



/*********************************************************************
* @purpose  To provide status on the CP task
*
* @param    void
*
* @returns  L7_BOOL - L7_TRUE if task is active
*
* @end
*********************************************************************/

L7_BOOL
captivePortalTaskActive(void)
{
  return (0 == captive_portal_exitflag);
}



