/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     sshd_cfg.c
*
* @purpose      SSH configuration code
*
* @component    sshd
*
* @comments     none
*
* @create       09/15/2003
*
* @author       dcbii
*
* @end
*
**********************************************************************/

#include <stdio.h>

#include "sysapi.h"
#include "defaultconfig.h"

#include "sshd_include.h"
#include "sshd_exports.h"

extern void             *sshdExitSema;
extern void             *sshdGlobalSema;
extern sshdCnfgrState_t sshdCnfgrState;

extern void sshd_var_init(void);
extern void ssh_var_init(void);

sshdGlobal_t            sshdGlobal;
sshdCfg_t               *sshdCfg;

sshdDeregister_t        sshdDeregister = {L7_FALSE, L7_FALSE, L7_FALSE};

#ifdef FEAT_METRO_CPE_V1_0
static osapiTimerDescr_t *t_sshd = L7_NULLPTR;
static L7_BOOL sshdAutoEnableTimerRunning = L7_FALSE;
#endif
/*********************************************************************
*
* @purpose  Save the SSHD configuration
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_RC_t
sshdSave(void)
{
    L7_RC_t rc = L7_SUCCESS;

    if (sshdHasDataChanged() == L7_TRUE)
    {
        sshdCfg->hdr.dataChanged = L7_FALSE;

        sshdCfg->checkSum = nvStoreCrc32((L7_uchar8 *)sshdCfg,
                                         (L7_uint32)(sizeof(sshdCfg_t) -
                                                     sizeof(sshdCfg->checkSum)));

        if ((rc = sysapiCfgFileWrite(L7_FLEX_SSHD_COMPONENT_ID, SSHD_CFG_FILENAME,
                                     (L7_char8 *)sshdCfg, (L7_int32)sizeof(sshdCfg_t)))
            != L7_SUCCESS )
        {
            rc = L7_ERROR;
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_SSHD_COMPONENT_ID, "sshdSave: Error on call to sysapiCfgFileWrite"
                                                   " for config file %s. Failed to create the SSHD config file %s.\n",
                                                    SSHD_CFG_FILENAME, SSHD_CFG_FILENAME);
        }
    }

    return rc;

} /* sshdSave */

/*********************************************************************
*
* @purpose  Check if the SSHD user configuration data has changed.
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*
*********************************************************************/

L7_BOOL
sshdHasDataChanged(void)
{
    return sshdCfg->hdr.dataChanged;

} /* sshdHasDataChanged */
void sshdResetDataChanged(void)
{
  sshdCfg->hdr.dataChanged = L7_FALSE;
  return;
}

/*********************************************************************
*
* @purpose  Build the SSHD configuration from defaults.
*
* @param    ver @b{(input)} version of configuration file for SSHD
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/

void
sshdBuildDefaultConfigData(L7_uint32 ver)
{
    /* Setup file header */

    sshdCfg->hdr.version = ver;
    sshdCfg->hdr.componentID = L7_FLEX_SSHD_COMPONENT_ID;
    sshdCfg->hdr.type = L7_CFG_DATA;
    sshdCfg->hdr.length = (L7_uint32)sizeof(sshdCfg_t);

    strcpy((L7_char8 *)sshdCfg->hdr.filename, SSHD_CFG_FILENAME);
    sshdCfg->hdr.dataChanged = L7_FALSE;

    /* Zero overlay */

    bzero((L7_char8 *)&sshdCfg->cfg, sizeof(sshdCfg->cfg));

    /*
    Default values for the sshd configuration structure
    */

    sshdCfg->cfg.sshdAdminMode   = L7_SSHD_ADMIN_MODE;
    sshdCfg->cfg.sshdProtoLevel  = L7_SSHD_PROTO_LEVEL;
    sshdCfg->cfg.sshdMaxSessions = L7_SSHD_DEFAULT_MAX_CONNECTIONS;
    sshdCfg->cfg.sshdIdleTimeout = L7_SSHD_DEFAULT_IDLE_TIMEOUT;
    sshdCfg->cfg.sshdPort        = L7_SSHD_PORT;
    sshdCfg->cfg.sshdPubKeyAuthMode = L7_SSHD_PUBKEY_AUTH_MODE;

    return;

} /* sshdBuildDefaultConfigData */

#ifdef FEAT_METRO_CPE_V1_0
/*********************************************************************
*
* @purpose  Enables SSH when atleast one of RSA or DSA key is present
*           Creates RSA/DSA key when they are found to be absent
*
* @param    none
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/
static void sshdAutoAdminModeSet()
{
    /* Generate RSA/DSA keys if they do not exist */
    if (sshdKeyExists(SSHD_KEY_TYPE_RSA) == L7_SUCCESS ||
        sshdKeyExists(SSHD_KEY_TYPE_DSA) == L7_SUCCESS)
    {
      if (sshdAdminModeSet(L7_ENABLE) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSHD_COMPONENT_ID,
                "sshdApplyConfigData: Failed to enable SSH");
      }
    }

    if (sshdKeyExists(SSHD_KEY_TYPE_RSA) != L7_SUCCESS)
    {
      (void)sshdKeyGenerate(SSHD_KEY_TYPE_RSA);
    }
    else if (sshdKeyExists(SSHD_KEY_TYPE_DSA) != L7_SUCCESS)
    {
      (void)sshdKeyGenerate(SSHD_KEY_TYPE_DSA);
    }
    else
    {
      if (t_sshd != L7_NULLPTR)
      {
        osapiTimerFree(t_sshd);
      }
      t_sshd = L7_NULLPTR;
      sshdAutoEnableTimerRunning = L7_FALSE;
      return;
    }
    /* Add timer func to check and enable SSH upon completion of
       key generation */
    sshdAutoEnableTimerRunning = L7_TRUE;
    osapiTimerAdd((void *)sshdAutoAdminModeSet, L7_NULL, L7_NULL,
                  60*1000, &t_sshd);
}
#endif
/*********************************************************************
*
* @purpose  Apply the current SSHD configuration.
*
* @returns  void
*
* @comments none
*
* @end
*
*********************************************************************/

void
sshdApplyConfigData(void)
{
    /*
    Clear the SSHD global structure and apply the configured values
    */

    osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);

    bzero((L7_char8 *)&sshdGlobal, (L7_int32)sizeof(sshdGlobal));

    sshdGlobal.sshdProtoLevel = sshdCfg->cfg.sshdProtoLevel;


    sshdGlobal.sshdIdleTimeout = sshdCfg->cfg.sshdIdleTimeout;
    sshdGlobal.sshdMaxSessions = sshdCfg->cfg.sshdMaxSessions;
    sshdGlobal.sshdPort = sshdCfg->cfg.sshdPort;
    sshdEventAdminModeSet(sshdCfg->cfg.sshdAdminMode);

    osapiSemaGive(sshdGlobalSema);

    /*
    Set data for SSH that is not 0 default
    */

    osapiSemaTake(sshdExitSema, L7_WAIT_FOREVER);
    osapiSemaTake(sshdGlobalSema, L7_WAIT_FOREVER);

    sshd_var_init();
    ssh_var_init();

    osapiSemaGive(sshdGlobalSema);
    osapiSemaGive(sshdExitSema);

    sshdCfg->hdr.dataChanged = L7_FALSE;
#ifdef FEAT_METRO_CPE_V1_0
    if (sshdAutoEnableTimerRunning == L7_FALSE)
    {
      sshdAutoAdminModeSet();
    }
#endif

    return;

} /* sshdApplyConfigData */


#define UNUSED __attribute__ ((__unused__))

/*********************************************************************
* @purpose  Migrate old SSHD config to new
*
* @param    oldVer  @b{(input)} Old software version of Config Data
* @param    ver     @b{(input)} Expected software version of Config Data
* @param    *buffer @b{(input)} Outdated config buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/

void
sshdMigrateBuild(L7_uint32 UNUSED oldVer, L7_uint32 UNUSED ver, L7_char8 UNUSED *buffer)
{
    return;

} /* sshdMigrateBuild */
