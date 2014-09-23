/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename sslt_cfg.c
*
* @purpose SSL Tunnel configuration code
*
* @component sslt
*
* @comments none
*
* @create 07/15/2003
*
* @author spetriccione
*
* @end
*
**********************************************************************/
#include "sslt_include.h"
#include "sslt_exports.h"

ssltGlobal_t ssltGlobal;
ssltCfg_t *ssltCfg;

ssltDeregister_t ssltDeregister = {L7_FALSE, L7_FALSE, L7_FALSE};

extern ssltCnfgrState_t ssltCnfgrState;

/*********************************************************************
*
* @purpose Save the SSLT configuration
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t ssltSave(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (ssltHasDataChanged() == L7_TRUE)
  {
    ssltCfg->hdr.dataChanged = L7_FALSE;

    ssltCfg->checkSum = nvStoreCrc32((L7_uchar8 *)ssltCfg,
                                     (L7_uint32)(sizeof(ssltCfg_t) - sizeof(ssltCfg->checkSum)));

    if ((rc = sysapiCfgFileWrite(L7_FLEX_SSLT_COMPONENT_ID,
                                 SSLT_CFG_FILENAME,
                                 (L7_char8 *)ssltCfg,
                                 (L7_int32)sizeof(ssltCfg_t))) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_SSLT_COMPONENT_ID,
              "SSLT: osapiFsWrite returns error, config file %s\n", SSLT_CFG_FILENAME);
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose Check if the SSLT user configuration data has changed.
*
* @returns L7_TRUE
* @returns L7_FALSE
*
* @comments
*
* @end
*
*********************************************************************/
L7_BOOL ssltHasDataChanged(void)
{
  return ssltCfg->hdr.dataChanged;
}
void ssltResetDataChanged(void)
{
  ssltCfg->hdr.dataChanged = L7_FALSE;
}
/*********************************************************************
*
* @purpose Build the SSLT configuration from defaults.
*
* @param ver @b{(input)} version of configuration file for SSLT
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltBuildDefaultConfigData(L7_uint32 ver)
{
  /* Setup file header */
  ssltCfg->hdr.version = ver;
  ssltCfg->hdr.componentID = L7_FLEX_SSLT_COMPONENT_ID;
  ssltCfg->hdr.type = L7_CFG_DATA;
  ssltCfg->hdr.length = (L7_uint32)sizeof(ssltCfg_t);

  osapiStrncpySafe((L7_char8 *)ssltCfg->hdr.filename, SSLT_CFG_FILENAME, sizeof(ssltCfg->hdr.filename));
  ssltCfg->hdr.dataChanged = L7_FALSE;

  /* Zero overlay */
  bzero((L7_char8 *)&ssltCfg->cfg, sizeof(ssltCfg->cfg));

  /*
  ** Default values for the sslt configuration structure
  */
  ssltCfg->cfg.ssltAdminMode = L7_SSLT_ADMIN_MODE;

  ssltCfg->cfg.ssltSecurePort = L7_SSLT_SECURE_PORT;

  osapiStrncpySafe(ssltCfg->cfg.ssltPemPassPhrase, L7_SSLT_PEM_PASS_PHRASE, sizeof(ssltCfg->cfg.ssltPemPassPhrase));

  ssltCfg->cfg.ssltUnSecurePort = L7_SSLT_UNSECURE_PORT;
  ssltCfg->cfg.ssltUnSecureServerAddr = L7_SSLT_UNSECURE_SERVER_ADDR;

  ssltCfg->cfg.ssltProtocolSSL30 = L7_SSLT_SSL30_MODE;
  ssltCfg->cfg.ssltProtocolTLS10 = L7_SSLT_TLS10_MODE;

  ssltCfg->cfg.ssltSessionHardTimeOut = FD_SECURE_HTTP_SESSION_HARD_TIMEOUT_DEFAULT;
  ssltCfg->cfg.ssltSessionSoftTimeOut = FD_SECURE_HTTP_SESSION_SOFT_TIMEOUT_DEFAULT;
  ssltCfg->cfg.ssltNumSessions = FD_SECURE_HTTP_DEFAULT_MAX_CONNECTIONS;

  ssltCfg->cfg.ssltCertificateNumber = L7_SSLT_CERT_NUMBER_ACTIVE;

  return;
}

/*********************************************************************
*
* @purpose Apply the current SSLT configuration.
*
* @returns void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltApplyConfigData(void)
{
  /*
  ** Clear the SSLT global structure and apply the configured values
  */

  bzero((L7_char8 *)&ssltGlobal, (L7_int32)sizeof(ssltGlobal));

  ssltGlobal.ssltSecurePort = ssltCfg->cfg.ssltSecurePort;

  osapiStrncpySafe(ssltGlobal.ssltPemPassPhrase, ssltCfg->cfg.ssltPemPassPhrase, sizeof(ssltGlobal.ssltPemPassPhrase));

  ssltGlobal.ssltUnSecurePort = ssltCfg->cfg.ssltUnSecurePort;
  ssltGlobal.ssltUnSecureServerAddr = ssltCfg->cfg.ssltUnSecureServerAddr;

  /*
  ** Set the non-configurable default options and protocols for SSL Tunnel use
  ** SSL_OP_ALL - All of the SSL bug workarounds
  ** SSL_OP_NO_SSLv2 - Do not use the SSL version 2 protocol
  ** SSL_OP_SINGLE_DH_USE - Always create a new key when using Diffie Hellman
  */
  ssltGlobal.ssltProtocolLevel = (SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_SINGLE_DH_USE | SSL_OP_CIPHER_SERVER_PREFERENCE);

  /* Set the configurable options and protocols for SSL Tunnel use */
  ssltEventProtocolLevelSet(L7_SSLT_PROTOCOL_SSL30, ssltCfg->cfg.ssltProtocolSSL30);
  ssltEventProtocolLevelSet(L7_SSLT_PROTOCOL_TLS10, ssltCfg->cfg.ssltProtocolTLS10);

  /* Start up SSLT if configured to do so */
  ssltEventAdminModeSet(ssltCfg->cfg.ssltAdminMode);

  /* Make sure the default PEM-encoded files are present and create them if not */
  /* TBD
  ssltDefaultPemFileCreate(L7_FILE_TYPE_SSLPEM_ROOT);
  ssltDefaultPemFileCreate(L7_FILE_TYPE_SSLPEM_SERVER);
  ssltDefaultPemFileCreate(L7_FILE_TYPE_SSLPEM_DHWEAK);
  ssltDefaultPemFileCreate(L7_FILE_TYPE_SSLPEM_DHSTRONG);
  */

  ssltGlobal.ssltDebugLevel = SSLT_DEBUG_LVL0;

  ssltGlobal.ssltCertificateNumber = ssltCfg->cfg.ssltCertificateNumber;

  ssltCfg->hdr.dataChanged = L7_FALSE;

  return;
}

/*********************************************************************
* @purpose  Migrate old SSLT config to new
*
* @param   oldVer  @b{(input)} Old software version of Config Data
* @param   ver     @b{(input)} Expected software version of Config Data
* @param   *buffer @b{(input)} Outdated config buffer
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void ssltMigrateBuild(L7_uint32 oldVer,
                      L7_uint32 ver,
                      L7_char8 *buffer)
{
  return;
}


/*============================================================================*/
/*========================  START OF CONFIG MIGRATION DEBUG CHANGES ==========*/
/*============================================================================*/



/*********************************************************************
* @purpose  Build non-default  config data
*
* @param    void
*
* @returns  void
*
* @notes    This routine is based on xxxBuildDefaultConfigData.
*
* @end
*********************************************************************/
void ssltBuildTestConfigData(void)
{

  /*-------------------------------*/
  /* Build Non-Default Config Data */
  /*-------------------------------*/

   ssltCfg->cfg.ssltAdminMode = L7_ENABLE;

   ssltCfg->cfg.ssltSecurePort = 688;
   osapiStrncpySafe(ssltCfg->cfg.ssltPemPassPhrase, "TestPhrase", sizeof(ssltCfg->cfg.ssltPemPassPhrase));

   ssltCfg->cfg.ssltUnSecurePort = 484;
   ssltCfg->cfg.ssltUnSecureServerAddr = 0x0A0A0A0B;

   ssltCfg->cfg.ssltProtocolSSL30 = L7_DISABLE;
   ssltCfg->cfg.ssltProtocolTLS10 = L7_DISABLE;

 /* End of Component's Test Non-default configuration Data */


   /* Force write of config file */
   ssltCfg->hdr.dataChanged = L7_TRUE;
   sysapiPrintf("Built test config data\n");


}





/*********************************************************************
*
* @purpose  Dump the contents of the config data.
*
* @param    void
*
* @returns  void
*
* @comments
*
* @end
*
*********************************************************************/
void ssltConfigDataTestShow(void)
{

    L7_fileHdr_t  *pFileHdr;


    /*-----------------------------*/
    /* Config File Header Contents */
    /*-----------------------------*/
    pFileHdr = &(ssltCfg->hdr);

    sysapiCfgFileHeaderDump (pFileHdr);

   /*-----------------------------*/
   /* cfgParms                    */
   /*-----------------------------*/

   sysapiPrintf( "ssltCfg->cfg.ssltAdminMode           = %d\n", ssltCfg->cfg.ssltAdminMode);
   sysapiPrintf( "ssltCfg->cfg.ssltSecurePort          = %d\n", ssltCfg->cfg.ssltSecurePort);
   sysapiPrintf( "ssltCfg->cfg.ssltPemPassPhrase       = %s\n", ssltCfg->cfg.ssltPemPassPhrase);
   sysapiPrintf( "ssltCfg->cfg.ssltUnSecurePort        = %d\n", ssltCfg->cfg.ssltUnSecurePort);
   sysapiPrintf( "ssltCfg->cfg.ssltUnSecureServerAddr  = %d\n", ssltCfg->cfg.ssltUnSecureServerAddr);
   sysapiPrintf( "ssltCfg->cfg.ssltProtocolSSL30       = %d\n", ssltCfg->cfg.ssltProtocolSSL30);
   sysapiPrintf( "ssltCfg->cfg.ssltProtocolTLS10       = %d\n", ssltCfg->cfg.ssltProtocolTLS10);


   /*-------------------------------*/
   /* Scaling Constants             */
   /*-------------------------------*/

    sysapiPrintf( "Scaling Constants\n");
    sysapiPrintf( "-----------------\n");


    sysapiPrintf( "None - \n");



    /*-----------------------------*/
    /* Checksum                    */
    /*-----------------------------*/
    sysapiPrintf("ssltCfg->checkSum : %u\n", ssltCfg->checkSum);


}
void
ssltShowGlobals(void)
{
    printf("\nssltAdminMode:        0x%08x\n", ssltGlobal.ssltAdminMode);
    printf("ssltOperMode:           0x%08x\n", ssltGlobal.ssltOperMode);
    printf("ssltListenTaskId:       0x%08x\n", ssltGlobal.ssltListenTaskId);
    printf("ssltSecurePort:         0x%08x\n", ssltGlobal.ssltSecurePort);
    printf("\n");
    printf("ssltAuxOperMode:        0x%08x\n", ssltGlobal.ssltAuxOperMode);
    printf("ssltAuxSecurePort1:     0x%08x\n", ssltGlobal.ssltAuxSecurePort1);
    printf("ssltAuxSecurePort2:     0x%08x\n", ssltGlobal.ssltAuxSecurePort2);
    printf("ssltAuxListenTaskId1:   0x%08x\n", ssltGlobal.ssltAuxListenTaskId1);
    printf("ssltAuxListenTaskId2:   0x%08x\n", ssltGlobal.ssltAuxListenTaskId2);
    printf("\n");
    printf("ssltUnSecurePort:       0x%08x\n", ssltGlobal.ssltUnSecurePort);
    printf("ssltProtocolLevel:      0x%08x\n", ssltGlobal.ssltProtocolLevel);
    printf("ssltUnSecureServerAddr: 0x%08x\n\n", ssltGlobal.ssltUnSecureServerAddr);

    return;

} /* sshdShowGlobals */




/*============================================================================*/
/*========================  END OF CONFIG MIGRATION DEBUG CHANGES ============*/
/*============================================================================*/





