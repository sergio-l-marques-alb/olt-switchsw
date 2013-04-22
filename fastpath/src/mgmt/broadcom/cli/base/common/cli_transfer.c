/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/cli_transfer.c
 *
 * @purpose transfer commands for the cli
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  07/16/2003
 *
 * @author  Srikrishna Saxena
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"

#include "datatypes.h"
#include "cli_config_script.h"
#include "sysapi_hpc_slotmapper.h"
#include "osapi_support.h"

#include "usmdb_dim_api.h"
#include "cli_web_exports.h"
#include "usmdb_sim_api.h"
#include "usmdb_slotmapper.h"
#include "usmdb_sshd_api.h"
#include "usmdb_sslt_api.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"
#include "usmdb_cda_api.h"

#include "usmdb_file_api.h"

#ifdef L7_MGMT_SECURITY_PACKAGE
#include "strlib_security_common.h"
#include "usmdb_sshc_api.h"
#endif /* L7_MGMT_SECURITY_PACKAGE */

#include "clicommands_card.h"
#include "sshd_exports.h"

static L7_RC_t setTransferMode(L7_char8 * transferMode);
static L7_RC_t setServerIP(L7_char8 * serverIp );
static const L7_char8 *transferUploadStart(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index, L7_char8 *host);
static L7_RC_t  setFileDatatype(L7_char8 * dataType);
static const L7_char8 *isTransferInProgress(EwsContext ewsContext, L7_uint32 unit, L7_uint32 argc,const L7_char8 * * argv, L7_BOOL upload, L7_uint32 index);
static const L7_char8 *transferDownloadStart(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index, L7_BOOL archive, L7_char8 *host);

static L7_char8 *cliGetErrSyntaxCopy (void)
{
  static char buf[512];
  L7_uint32       unit;
  unit = cliGetUnitId();

  if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) == L7_TRUE)
  {
#ifdef L7_MGMT_SECURITY_PACKAGE
    osapiSnprintf(buf, sizeof(buf), "%s", pStrErr_base_CopySecurityPkg);
#else
    osapiSnprintf(buf, sizeof(buf), "%s",  pStrErr_base_Copy_3);
#endif
  }
  else
  {
#ifdef L7_MGMT_SECURITY_PACKAGE
#if defined(FEAT_METRO_CPE_V1_0) /* If Services, then mask the "fastpath.cfg" as "Switch.cfg".*/
    osapiSnprintf(buf, sizeof(buf), "%s",  pStrErr_base_CopyTxSwitchCfg);
#else
    osapiSnprintf(buf, sizeof(buf), "%s",  pStrErr_base_CopyTx);
#endif
#else
#if defined(FEAT_METRO_CPE_V1_0) /* If Services, then mask the "fastpath.cfg" as "Switch.cfg".*/
    osapiSnprintf(buf, sizeof(buf), "%s",  pStrErr_base_CopySecurityPkgTxSwitchCfg);
#else
    osapiSnprintf(buf, sizeof(buf), "%s",  pStrErr_base_CopySecurityPkgTx);
#endif
#endif
  }
  return buf;
}

/*********************************************************************
 *
 * @purpose parse a string into xfer type. ip, dir and filename
 *
 * @param L7_char8 *str
 * @param L7_uint32 *unit
 * @param L7_char8 *fileName
 * @param L7_uint32 fileNameSize
 *
 * @returns L7_uint32
 *
 * @end
 *
 *********************************************************************/
L7_RC_t unitUrlParser(L7_char8 * str, L7_uint32 * unit,
    L7_char8 * fileName, L7_uint32 fileNameSize)
{
  int len = strlen(pStrInfo_base_Unit_4);
  char * pBuf;
  char * unitNum;
  char * temp;
  L7_int32 genericUnit;
  L7_uint32 imageId = 0;
  pBuf = strstr(str,pStrInfo_base_Unit_4);
  if(pBuf == NULL)
  {
    return L7_FAILURE;
  }

  pBuf += len;
  len = 0;

  /* get the unit number */
  unitNum = pBuf;

  pBuf = strstr(unitNum,"/");
  if(pBuf == NULL)
  {
    return L7_FAILURE;
  }

  len = ((unsigned int)pBuf - (unsigned int)unitNum);
  if(len != 1)
  {
    return L7_FAILURE;
  }

  if( *(unitNum) == '*')
  {
    /* all units */
    *unit = L7_USMDB_CDA_CDA_ALL_UNITS;
  }
  else
  {
    if ( (*(unitNum) < '0') || (*(unitNum) > '9') )
    {
      return L7_FAILURE;
    }

    *unit = (L7_uint32)(*(unitNum) - '0');
    genericUnit = *unit;
    if (cliIsUnitPresent((genericUnit)) != L7_TRUE)
    {
      return L7_FAILURE;
    }
  }

  pBuf += 1;

  /* check for directories */

  temp = strstr(pBuf, "/");

  if(temp != NULL)
  {
    return L7_FAILURE;
  }

  /* copy the file name */

   if ((usmDbImageFileNameValid(pBuf, &imageId) == L7_TRUE) ||
       (usmDbImageNameValid(pBuf) == L7_TRUE))
   {
    osapiStrncpySafe(fileName, pBuf, fileNameSize);
   }
   else
   {
    return L7_FAILURE;
   }

  return L7_SUCCESS;

}

#ifdef L7_MGMT_SECURITY_PACKAGE
/*********************************************************************
*
* @purpose parse a string into xfer type. ip, dir and filename
*
* @param L7_char8 *in_str
* @param L7_char8 *username
* @param L7_char8 *password
* @param L7_char8 *ipaddr
* @param L7_char8 *path
* @param L7_char8 *filename
* @param L7_char8 *xferToken
*
* @returns L7_uint32
*
* @end
*
*********************************************************************/
L7_uint32 urlSecureParser(L7_char8 *in_str, L7_char8 *username, L7_uint32 usernameSize, L7_char8 *ipaddr,
                        L7_uint32 ipaddrSize, L7_char8 *path, L7_uint32 pathSize, L7_char8 *filename,
                        L7_uint32 filenameSize, L7_char8 *xferToken, L7_uint32 xferTokenSize)
{
  L7_char8 *pbuf ;
  L7_char8 *ptmp ;
  L7_uint32 colonIndex;
  L7_uint32 fileIndex;
  L7_uint32 userIndex;
  L7_uint32 ipaddrIndex;
  L7_uint32 pathIndex;
  L7_uint32 tokenLen;

  if (strlen(in_str) < 1)
  {
    return L7_FAILURE;
  }

  *xferToken = '\0';
  *username = '\0';
  *ipaddr = '\0';
  *path = '\0';
  *filename = '\0';

  /* check for ':' */
  pbuf=strchr(in_str,':');

  if(pbuf == L7_NULL)  /* token separator not found */
  {
    return L7_FAILURE;
  }

  /*Location of first ':' */
  colonIndex=strlen(in_str) - strlen(pbuf);

  if (colonIndex  == strlen(pStrInfo_security_Sftp_1) )
  {
    tokenLen= colonIndex;
  }
  else if (colonIndex  == strlen(pStrInfo_security_Scp_1) )
  {
    tokenLen= colonIndex;
  }
  else
  {
    return L7_FAILURE;
  }

  if (strncmp(in_str, pStrInfo_security_Sftp_1, tokenLen) != 0 &&
      strncmp(in_str, pStrInfo_security_Scp_1, tokenLen) != 0)
  {
    return L7_FAILURE;
  }

  osapiStrncpySafe(xferToken, in_str, min((tokenLen+1), xferTokenSize));

  /*Get filename */
  /* check for last '/' */
  pbuf=strrchr(in_str,'/');

  if(pbuf == L7_NULL)
  {
    return L7_FAILURE; /* No filename */
  }
  else
  {
    /*filename start index */
    fileIndex = strlen(in_str) - strlen (pbuf);

    tokenLen = strlen(pbuf) - 1;

    if(tokenLen > 0)
    {
     osapiStrncpySafe(filename, in_str + fileIndex + 1, min((tokenLen+1), filenameSize));
    }
    else
    {
      return L7_FAILURE; /* No filename */
    }
  }


  /*Get path */
  /* check for first '/' after 'sftp://'*/
  ptmp = strstr(in_str, "//");
  if (ptmp == L7_NULL)
  {
    return L7_FAILURE;
  }

  /*to take care of sftp/scp::xxx input */
  /* strlen(in_str) - strlen(ptmp) is the index at "//" */
  /* colonIndex is the index at ":" */
  /* difference should be always '1', i.e no charecters between : and // */
  if((strlen(in_str) - strlen(ptmp) - colonIndex) != 1)
  {
     return L7_FAILURE;
  }


  pbuf=strchr(ptmp + 2, '/');

  if(pbuf == L7_NULL)
  {
    return L7_FAILURE; /* No filename */
  }
  else
  {
    /*path start index */
    pathIndex = strlen(in_str) - strlen (pbuf);

    if( pathIndex  == fileIndex )
    {
      strcpy(path, "./");  /*Default path current dir */
      tokenLen = 0;
    }
    else
    {
      tokenLen = fileIndex - pathIndex ;
    }

    if(tokenLen > 0)
    {
      osapiStrncpySafe(path, in_str + pathIndex + 1, min((tokenLen+1), pathSize));
    }
  }

  /*Get ip address */
  /* check for first '@' from last*/
  pbuf = strrchr(in_str, '@');
  if (pbuf == L7_NULL)
  {
    return L7_FAILURE;
  }
  else
  {
    /* ip addr start here */
   ipaddrIndex = strlen(in_str) - strlen (pbuf);

    tokenLen = pathIndex - ipaddrIndex;
    tokenLen = tokenLen -1;
    if(tokenLen > 0)
    {
     osapiStrncpySafe(ipaddr, in_str + ipaddrIndex + 1, min((tokenLen+1), ipaddrSize));
    }
    else
    {
      return L7_FAILURE; /* No ip address */
    }
  }

  /*Get user name */
  ptmp = strstr(in_str, "//");
  if (ptmp == L7_NULL)
  {
    return L7_FAILURE;
  }

  userIndex = strlen(in_str) - strlen (ptmp);
  tokenLen = ipaddrIndex - userIndex - 2;

  if(tokenLen > 0)
  {
    osapiStrncpySafe(username, in_str + userIndex + 2, min((tokenLen+1), usernameSize));
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
#endif /* L7_MGMT_SECURITY_PACKAGE */

  /*********************************************************************
   *
   * @purpose parse a string into xfer type. ip, dir and filename
   *
   * @param L7_char8 *in_str
   * @param L7_char8 *ipaddr
   * @param L7_uint32 ipaddrSize
   * @param L7_char8 *path
   * @param L7_uint32 pathSize
   * @param L7_char8 *filename
   * @param L7_uint32 filenameSize
   * @param L7_char8 *xferToken
   * @param L7_uint32 xferTokenSize
   *
   * @returns L7_uint32
   *
   * @end
   *
   *********************************************************************/
  L7_uint32 urlParser(L7_char8 *in_str, L7_char8 *ipaddr, L7_uint32 ipaddrSize,
                      L7_char8 *path, L7_uint32 pathSize, L7_char8 *filename, L7_uint32 filenameSize,
                      L7_char8 *xferToken, L7_uint32 xferTokenSize)
  {
    L7_char8 *pbuf ;
    L7_char8 *ptmp ;
    L7_uint32 colonIndex;
    L7_uint32 fileIndex;
    L7_uint32 pathIndex;
    L7_uint32 tokenLen = strlen("xmodem");

    if (strlen(in_str) < 1) /*At least tftp:x should be there*/
      return L7_FAILURE;

    if ((osapiStrncmp(in_str,"xmodem", tokenLen) == 0 ) ||
        (osapiStrncmp(in_str,"ymodem", tokenLen) == 0 ) ||
        (osapiStrncmp(in_str,"zmodem", tokenLen) == 0 ))
    {
      if(strlen(in_str) > strlen("ymodem"))
      {
        return L7_FAILURE;
      }
      else
      {
        osapiStrncpySafe(xferToken, in_str, xferTokenSize);
        *ipaddr = '\0';
        *path = '\0';
        *filename = '\0';
        return L7_SUCCESS;
      }
    }
    /* check for ':' */
    pbuf=strchr(in_str,':');

    if(pbuf == L7_NULL)  /* token separator not found */
      return L7_FAILURE;

    /*Location of first ':' */
    colonIndex=strlen(in_str) - strlen(pbuf);

    if(colonIndex  == strlen("tftp") )
      tokenLen= colonIndex ;
    else
      return L7_FAILURE;

  if (osapiStrncmp(in_str, "tftp", tokenLen) != 0)
  {
    return L7_FAILURE;
  }

  osapiStrncpySafe(xferToken, in_str, min((tokenLen+1), xferTokenSize));

  /*Get filename */
  /* check for last '/' */
  pbuf=strrchr(in_str,'/');

  if(pbuf == L7_NULL)
  {
    return L7_FAILURE; /* No filename */
  }
  else
  {
    /*filename start index */
    fileIndex = strlen(in_str) - strlen (pbuf);

    tokenLen = strlen(pbuf) - 1;

    if(tokenLen > 0)
    {
     osapiStrncpySafe(filename, in_str + fileIndex + 1, min((tokenLen+1), filenameSize));
    }
    else
    {
     return L7_FAILURE; /* No filename */
    }
  }


  /*Get path */
  /* check for first '/' after 'tftp://'*/
  ptmp = strstr(in_str, "//");
  if (ptmp == L7_NULL)
    return L7_FAILURE;

  /*to take care of tftp::xxx input */
  /* strlen(in_str) - strlen(ptmp) is the index at "//" */
  /* colonIndex is the index at ":" */
  /* difference should be always '1', i.e no charecters between : and // */
  if((strlen(in_str) - strlen(ptmp) - colonIndex) != 1)  
  {
     return L7_FAILURE;
  }

  pbuf=strchr(ptmp + 2, '/');

  if(pbuf == L7_NULL)
  {
    return L7_FAILURE; /* No filename */
  }
  else
  {
    /*path start index */
    pathIndex = strlen(in_str) - strlen (pbuf);

    if( pathIndex  == fileIndex )
    {
      strcpy(path, "./");  /*Default path current dir */
      tokenLen = 0;
    }
    else
    {
      tokenLen = fileIndex - pathIndex ;
    }

    if(tokenLen > 0)
    {
      osapiStrncpySafe(path, in_str + pathIndex + 1, min((tokenLen+1), pathSize));
    }
  }


  /* tftp xfer type */
  /* ip addres is from colonIndex to pathIndex */

  tokenLen =  pathIndex - colonIndex  - 3;

  if(tokenLen > 0)
  {
    osapiStrncpySafe(ipaddr, in_str + colonIndex + 3, min((tokenLen+1), ipaddrSize));
  }
  else
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose  sets the transfer mode for uploading from the switch
 *
 * @param L7_char8 *transferMode
 *
 * @returntype L7_RC_t
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t setTransferMode(L7_char8 * transferMode)
{
  L7_RC_t rc;
  L7_uint32 unit = 1;

  if (strcmp( transferMode, pStrInfo_base_Xmodem_1 ) == 0)
  {
    rc = usmDbTransferModeSet(unit, L7_TRANSFER_XMODEM);
  }
  else if (strcmp( transferMode, pStrInfo_common_Tftp_1 ) == 0)
  {
    rc = usmDbTransferModeSet(unit, L7_TRANSFER_TFTP);
  }
#ifdef L7_MGMT_SECURITY_PACKAGE
  else if (strcmp( transferMode, pStrInfo_security_Sftp_1 ) == 0)
  {
      rc = usmDbTransferModeSet(unit, L7_TRANSFER_SFTP);
  }
  else if (strcmp( transferMode, pStrInfo_security_Scp_1 ) == 0)
  {
      rc = usmDbTransferModeSet(unit, L7_TRANSFER_SCP);
  }
#endif /* L7_MGMT_SECURITY_PACKAGE */
#ifdef _L7_OS_LINUX_
  else if (strcmp( transferMode, pStrInfo_base_Ymodem_1 ) == 0)
  {
    rc = usmDbTransferModeSet(unit, L7_TRANSFER_YMODEM);
  }
  else if (strcmp( transferMode, pStrInfo_base_Zmodem_1 ) == 0)
  {
    rc = usmDbTransferModeSet(unit, L7_TRANSFER_ZMODEM);
  }
#endif /* _L7_OS_LINUX_ */
  else
  {
    rc  = L7_FAILURE;
  }
  return rc;
}

/*********************************************************************
 *
 * @purpose  sets the server IP for uploading from the switch
 *
 *
 * @param const L7_char8 *serverIp
 *
 * @returntype L7_RC_t
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t setServerIP(L7_char8 * serverIp )
{
  L7_uint32 rc;
  L7_inet_addr_t inetAddr;
  L7_uint32 unit = 1;

  if (strlen(serverIp) >= L7_DNS_HOST_NAME_LEN_MAX)
  {
    return L7_FAILURE;
  }
  if(usmDbParseInetAddrFromIPAddrHostNameStr(serverIp, &inetAddr) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* setting the address type of the tftp server */
  rc = usmDbTransferServerAddressTypeSet(unit, (L7_uint32)inetAddr.family);
  if(rc == L7_SUCCESS)
  {
    /* setting the tftp server's address */
    rc = usmDbTransferServerAddressSet(unit, (L7_uchar8 *)(&(inetAddr.addr)));
  }
  return rc;
}
/*********************************************************************
* @purpose  builds a valid url help string.
*
* @param    unit    Unit number
* @param    buf     The buffer to be filled with the help string.
* @param    bufSize Size of the output buffer.
*
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/
void cliCopyBuildUrlHelpStr(L7_uint32 unit, L7_char8 * buf, L7_uint32 bufSize)
{

  osapiStrncat(buf, pStrInfo_common_CrLf, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_base_InvalidUrl, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));

  osapiStrncat(buf, pStrInfo_base_SerialXmodem_1 , (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
#ifdef _L7_OS_LINUX_
  osapiStrncat(buf, pStrInfo_base_SerialYZmodem_1 , (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
#endif

  osapiStrncat(buf, pStrInfo_base_Pipe, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
  osapiStrncat(buf, pStrInfo_base_TftpUrl, (bufSize - strlen(buf) - 1));
  if (usmDbFeaturePresentCheck(unit,L7_FLEX_SSHD_COMPONENT_ID,
                               L7_SSHD_SECURE_TRANSFER_FEATURE_ID) == L7_TRUE) 
  {
    osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
    osapiStrncat(buf, pStrInfo_base_Pipe, (bufSize - strlen(buf) - 1));
    osapiStrncat(buf, pStrInfo_common_Space, (bufSize - strlen(buf) - 1));
    osapiStrncat(buf, pStrInfo_base_SecureUrl, (bufSize - strlen(buf) - 1));
  }
  osapiStrncat(buf, pStrInfo_common_Period, (bufSize - strlen(buf) - 1));

}
/*********************************************************************
 *
 * @purpose  Command to transfer the file
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes
 *
 * @cmdsyntax  copy { { nvram:startup-config | nvram:script | nvram:errorlog | nvram:log | nvram:valgrindLog | nvram:traplog } <url> }
 *            |  {<url> {<image-file-name> |nvram:script | nvram:startup-config } |
 *            {system:running-config nvram:startup-config})
 *
 * @cmdhelp
 *
 * @cmddescript
 *
 * @end
 *
 *********************************************************************/
const L7_char8 *commandCopy(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index)
{
  L7_uint32 rc, unit = 1, mgmtUnit = 1;
  static L7_BOOL upload = L7_FALSE;
  L7_char8 urlStr[4*L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 unitUrlStr[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 type[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 strIp[L7_DNS_HOST_NAME_LEN_MAX];
  L7_char8 filePath[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 fileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 localFileName[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 scriptLocalFilename[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 scriptRemoteFilename[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 * fileType;
  L7_char8 cmd[2*L7_CLI_MAX_STRING_LENGTH];
  L7_char8 buf[100];
  L7_BOOL stacking_pkg = L7_FALSE;
  L7_uint32 val;
  L7_ConfigScript_t configScriptData;
  L7_char8 ipv6Addr[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 * cp;
#ifdef L7_MGMT_SECURITY_PACKAGE
  L7_char8 username[L7_CLI_MAX_STRING_LENGTH];
#endif
  L7_uint32 input =0, ipAddr = 0;
  L7_IP_ADDRESS_TYPE_t addrType = L7_IP_ADDRESS_TYPE_UNKNOWN;
  L7_char8 urlHelpStr[4*L7_CLI_MAX_STRING_LENGTH];
  L7_char8 stat[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 imageId = 0;

  memset(type, 0, sizeof(type));
  memset(urlStr, 0, sizeof(urlStr));
  memset(urlHelpStr, 0, sizeof(urlHelpStr));
  memset(strIp, 0, sizeof(strIp));
  memset(filePath, 0, sizeof(filePath));
  memset(fileName, 0, sizeof(fileName));
  memset(scriptLocalFilename, 0, sizeof(scriptLocalFilename));
  memset(scriptRemoteFilename, 0, sizeof(scriptRemoteFilename));
  memset(localFileName, 0, sizeof(localFileName));
  memset(cmd, 0, sizeof(cmd));
  memset(buf, 0, sizeof(buf));
  fileType = L7_NULL;
  cp = L7_NULL;

#ifdef L7_STACKING_PACKAGE
  stacking_pkg = L7_TRUE;
#endif /* L7_STACKING_PACKAGE */

  usmDbUnitMgrNumberGet(&unit);
  usmDbUnitMgrMgrNumberGet(&mgmtUnit);

  /*check for file transfer already in progress*/
  cp = (L7_char8 *)isTransferInProgress(ewsContext, unit, argc,argv, upload, index);

  if(cp != L7_NULL)
  {
    return cp;
  }

  usmDbSimTransferBytesCompletedSet(0);

  /*special case to handle download of config script when its validation has failed*/
  input = cliGetCharInputID(); 
  if(cliNumFunctionArgsGet() == 3 && (input == 3)) 
  {
    if (input != CLI_INPUT_EMPTY) 
    {                                                                     /* if our question has been answered */
      if (tolower(cliGetCharInput()) == 'y')         /* yes */
      {
        if (usmDbTransferFileNameLocalGet(unit, fileName) == L7_SUCCESS)
        {
          if(readDownloadedConfigScript(TEMP_CONFIG_SCRIPT_FILE_NAME, fileName, &configScriptData) == L7_SUCCESS)
          {
            OSAPI_STRNCPY_SAFE(configScriptData.cfgHdr.filename, fileName);

            configScriptData.cfgHdr.dataChanged = L7_TRUE;

            if(writeConfigScriptData(&configScriptData) == L7_SUCCESS)
            {
              cliSyntaxTop(ewsContext);
              ewsTelnetWrite( ewsContext, pStrInfo_base_FileTransferCfgScriptCreated);
              rc = L7_SUCCESS;
            }
            else
            {
              ewsTelnetWrite(ewsContext, getLastError());
              rc = L7_ERROR;
            }

            /*release buffer*/
            releaseScriptBuffer(&configScriptData) ;
          }
        }
        else
        {
          cliSyntaxTop(ewsContext);
          ewsTelnetWriteAddBlanks (0, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_GettingFilename);
        }
      }
      else      /* no */
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWrite( ewsContext, pStrInfo_base_FileTransferCfgScriptDownLoadTrerminated);
      }
    }

    osapiFsDeleteFile (TEMP_CONFIG_SCRIPT_FILE_NAME);
    usmDbTransferInProgressSet(unit, L7_FALSE);
    usmDbTransferSuspendMgmtAccessSet(unit, L7_FALSE);
    usmDbTransferContextSet(unit, NULL);

    return cliSyntaxReturnPrompt (ewsContext, "");
  }

  /*reset again*/
  memset(fileName, 0, sizeof(fileName));
  upload = L7_FALSE;

  cliCopyBuildUrlHelpStr(unit, urlHelpStr, sizeof(urlHelpStr));

  if (cliNumFunctionArgsGet() < 1)
  {
    return cliSyntaxReturnPrompt (ewsContext, cliGetErrSyntaxCopy());
  }

  if (strcmp( argv[index+1], pStrInfo_base_NvramFastpathCfg ) == 0)
  {
    rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_CONFIG);
    upload = L7_TRUE;
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+2]);
    setFileDatatype(pStrInfo_base_Cfg_7);
  }
  else if (strcmp( argv[index+1], pStrInfo_base_NvramBackupCfg ) == 0)
  {
    if( strcmp( argv[index+2], pStrInfo_common_NvramStartupCfg ) == 0)
    {
      osapiFsCopyFile(SYSAPI_TXTCFG_BACKUP_FILENAME, SYSAPI_TXTCFG_FILENAME);
      osapiFsCopyFile(SYSAPI_CONFIG_BACKUP_FILENAME, SYSAPI_CONFIG_FILENAME);
      usmDbUnitMgrPropagateCfg();
      return cliPrompt(ewsContext);
    }
    else
    {
      return cliSyntaxReturnPrompt (ewsContext, cliGetErrSyntaxCopy());
    }
  }
  else if (strcmp( argv[index+1], pStrInfo_common_NvramStartupCfg ) == 0)
  {
    if( strcmp( argv[index+2], pStrInfo_base_NvramBackupCfg ) == 0)
    {
      osapiFsCopyFile(SYSAPI_TXTCFG_FILENAME, SYSAPI_TXTCFG_BACKUP_FILENAME);
      osapiFsCopyFile(SYSAPI_CONFIG_FILENAME, SYSAPI_CONFIG_BACKUP_FILENAME);
      usmDbUnitMgrPropagateCfg();
      return cliPrompt(ewsContext);
    }

    rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_TXTCFG);
    upload = L7_TRUE;
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+2]);
    setFileDatatype(pStrInfo_base_Txtcfg);
  }
  else if (strcmp( argv[index+1], pStrInfo_base_NvramErrorlog ) == 0)
  {
    rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_ERRORLOG);
    upload = L7_TRUE;
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+2]);
    setFileDatatype(pStrInfo_base_Cfg_7);
  }
#ifdef L7_TOOL_VALGRIND
  else if (strcmp( argv[index+1], pStrInfo_base_NvramValgrindlog ) == 0)
  {
    rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_VALGLOG);
    upload = L7_TRUE;
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+2]);
    setFileDatatype(pStrInfo_base_Cfg_7);
  }
#endif
  else if (strcmp( argv[index+1], pStrInfo_base_NvramLog ) == 0)
  {
    rc= usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_SYSTEM_MSGLOG);
    upload = L7_TRUE;
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+2]);
    setFileDatatype(pStrInfo_base_Cfg_7);
  }
  else if (strcmp( argv[index+1], pStrInfo_base_NvramTraplog ) == 0)
  {
    rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_TRAP_LOG);
    upload = L7_TRUE;
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+2]);
    setFileDatatype(pStrInfo_base_Cfg_7);
  }
  else if (strcmp( argv[index+1], pStrInfo_base_NvramScript ) == 0)
  {
    OSAPI_STRNCPY_SAFE(scriptLocalFilename, argv[index+2]);

    if(checkConfigScriptFiletype(scriptLocalFilename) != L7_SUCCESS ||
        checkConfigScriptPresent(scriptLocalFilename) != L7_SUCCESS)
    {
      cliSyntaxBottom(ewsContext);
      return cliSyntaxReturnPrompt (ewsContext, getLastError());
    }

    rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_CONFIG_SCRIPT);
    upload = L7_TRUE;
    setConfigScriptSourceFileName(scriptLocalFilename);
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+3]);
    setFileDatatype(pStrInfo_base_Script);

  }
  else if (strcmp( argv[index+1], pStrInfo_base_NvramClibanner ) == 0)
  {
    rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_CLI_BANNER);
    upload = L7_TRUE;
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+2]);
    setFileDatatype(pStrInfo_base_Banner);
  }
  else if ( strcmp( argv[index+1], pStrInfo_base_SysRunningCfg ) == 0)
  {
    if( strcmp( argv[index+2], pStrInfo_common_NvramStartupCfg ) == 0)
    {
      if ( cliGetCharInputID() == CLI_INPUT_EMPTY )
      {
        cliSetCharInputID(1, ewsContext, argv);

        osapiSnprintf(cmd, sizeof(cmd), pStrInfo_base_Copy_6, argv[index+ 1], argv[index+ 2]);
        cliAlternateCommandSet(cmd);
        return pStrErr_common_CopySysRunningCfgNvramStartupCfgMsg;
      }
      else if ( cliGetCharInputID() == 1 )
      {
        if ( tolower(cliGetCharInput()) == 'y' )
        {                                                    /* yes */
          usmDbSwDevCtrlSaveConfigurationSet(unit,1);        /*  inside usmdb now nvStoreSave(); saveConfigPermanently(); */
          ewsTelnetWrite(ewsContext, pStrInfo_common_MsgCfgSaved);
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        }
        else
        {                                                    /* no */
          ewsTelnetWrite(ewsContext, pStrErr_common_MsgCfgNotSaved);
          ewsCliDepth(ewsContext, cliPrevDepthGet(), argv);
        }
      }
      return cliPrompt(ewsContext);
    }
    else
    {
      return cliSyntaxReturnPrompt (ewsContext, cliGetErrSyntaxCopy());
    }
  }
  else if ((usmDbImageNameValid((char *)argv[index+1]) == L7_TRUE) ||
       (usmDbImageFileNameValid((char *)argv[index+1], &imageId) == L7_TRUE))
  {
   /* Verify the file exists */
   if(usmDbIsAnImage(unit, (L7_char8 *)(argv[index+1])) == L7_TRUE)
   {
    /* upload */

    rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_CODE);
    upload = L7_TRUE;

    OSAPI_STRNCPY_SAFE(localFileName, argv[index+1]);
    OSAPI_STRNCPY_SAFE(unitUrlStr, argv[index+2]);

    if (strcmp(unitUrlStr, argv[index+1]) == 0)
    {
      return cliSyntaxReturnPromptAddBlanks (1, 2, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_FileDescrDuplicate);
    }

    /* local file copy */

    if ((usmDbImageFileNameValid(unitUrlStr, &imageId) == L7_TRUE) ||
        (usmDbImageNameValid(unitUrlStr) == L7_TRUE))
    {
      osapiSnprintfAddBlanks (1, 0, 0, 1, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_CopyingTo, localFileName, unitUrlStr);

      cliWrite(buf);
      rc = usmDbFileCopy(mgmtUnit, localFileName, unitUrlStr,L7_TRUE);

      if (rc == L7_SUCCESS && stacking_pkg == L7_TRUE)
      {
        rc = usmDbFileCopy(L7_USMDB_CDA_CDA_ALL_UNITS,localFileName, unitUrlStr,L7_TRUE);
      }

      cliSyntaxBottom (ewsContext);

      if(rc != L7_SUCCESS)
      {
        ewsTelnetWrite( ewsContext, pStrInfo_base_CopyOperationFailed);
      }
      else
      {
        ewsTelnetWrite( ewsContext, pStrInfo_base_CopyOperationSuccess);
      }
      
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    /* copy from mgmt node to a stack member */

    if (stacking_pkg == L7_TRUE)
    {
      if (strstr(unitUrlStr, pStrInfo_base_Unit_3) != NULL)
        {
          if( unitUrlParser(unitUrlStr, &unit, fileName, sizeof(fileName)) != L7_SUCCESS)
          {
            return cliSyntaxReturnPrompt (ewsContext, cliGetErrSyntaxCopy());
          }

          if (unit == L7_USMDB_CDA_CDA_ALL_UNITS)
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_CopyingToOnAllUnits, localFileName, fileName);

          }
          else
          {
            osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, buf, sizeof(buf), pStrInfo_base_CopyingToOnUnit, localFileName, fileName, unit);
          }

          cliWrite(buf);

          rc = L7_SUCCESS;
          if ((L7_USMDB_CDA_CDA_ALL_UNITS == unit) && (strcmp(localFileName, fileName) != 0))
          {
              rc = usmDbFileCopy(mgmtUnit, localFileName, fileName, L7_TRUE);
          }

          if ( rc == L7_SUCCESS)
            rc = usmDbFileCopy(unit, localFileName, fileName, L7_TRUE);

          if(rc != L7_SUCCESS)
          {
            ewsTelnetWriteAddBlanks (1, 0, 0, 0, L7_NULLPTR, ewsContext, pStrInfo_base_CopyOperationFailed);
          }
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
    }

    /* upload */

    rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_CODE);
    upload = L7_TRUE;
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+2]);
    setFileDatatype(pStrInfo_base_Code_3);
    OSAPI_STRNCPY_SAFE(localFileName, argv[index+1]);
  }
  else
   {
     if(imageId == 0)
      osapiSnprintf(stat,sizeof(stat),pStrInfo_common_FindFailImage1);
     else
      osapiSnprintf(stat,sizeof(stat),pStrInfo_common_FindFailImage2);
     return cliSyntaxReturnPrompt (ewsContext, stat);
   }
 }
  else
  {
    upload = L7_FALSE;
    if (cliNumFunctionArgsGet() < 2)
    {
      return cliSyntaxReturnPrompt (ewsContext, cliGetErrSyntaxCopy());
    }

    if( strcmp( argv[index+2], pStrInfo_base_NvramFastpathCfg ) == 0)
    {
      rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_CONFIG);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_Cfg_7);
    }
    else if( strcmp( argv[index+2], pStrInfo_common_NvramStartupCfg ) == 0)
    {
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      if (usmDbFeaturePresentCheck(unit, L7_CLI_WEB_COMPONENT_ID, L7_TXT_CFG_FEATURE_ID) == L7_TRUE)
      {
        rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_TXTCFG);
        setFileDatatype(pStrInfo_base_Txtcfg);
      }
      else
      {
        rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_CONFIG);
        setFileDatatype(pStrInfo_base_Cfg_7);
      }
    }
    else if( strcmp( argv[index+2], pStrInfo_base_SysImage ) == 0)
    {
      usmDbImageFileNameGet(0, localFileName);
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_CODE);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_Code_3);
    }
    else if ((usmDbImageNameValid((char *)argv[index+2]) == L7_TRUE) ||
         (usmDbImageFileNameValid((char *)argv[index+2], &imageId) == L7_TRUE))
    {
      /* extract the local file name,
       * set the transfer type to code,
       * extract the url and proceed for URL validation
       */
      OSAPI_STRNCPY_SAFE(localFileName, argv[index+2]);
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_CODE);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_Code_3);

      if(stacking_pkg == L7_TRUE)
      {
        if (unit != mgmtUnit)
        {
          printf("attempting copy on non-management nodes\n\n");
          return cliSyntaxReturnPrompt (ewsContext, "");
        }
        unit = L7_USMDB_CDA_CDA_ALL_UNITS;
      }

      usmDbTransferUnitNumberSet(unit);

    }
    else if( strcmp( argv[index+2], pStrInfo_base_NvramScript ) == 0)
    {
      rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_CONFIG_SCRIPT);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      OSAPI_STRNCPY_SAFE(scriptLocalFilename, argv[index+3]);
      usmDbTransferFileNameLocalSet(unit, scriptLocalFilename);
      fileType = scriptLocalFilename;
      setFileDatatype(pStrInfo_base_Script);
    }
    else if (strcmp(argv[index+2], pStrInfo_base_IASUser) == 0)
    {
      rc = usmDbTransferUploadFileTypeSet(unit, L7_FILE_TYPE_IAS_USERS);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_IASUser);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_NvramClibanner ) == 0)
    {
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_CLI_BANNER);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_Banner);
    }
  else if (strcmp( argv[index+2], pStrInfo_base_Kernel ) == 0)
  {
    OSAPI_STRNCPY_SAFE(localFileName, argv[index+2]);
    rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_KERNEL);
    OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
    setFileDatatype(pStrInfo_base_Kernel);

  }
#ifdef L7_MGMT_SECURITY_PACKAGE
    else if( strcmp( argv[index+2], pStrInfo_base_NvramSshkeyRsa1 ) == 0)
    {
      if ((usmDbsshdAdminModeGet(unit, &val) == L7_SUCCESS) &&
          (val == L7_ENABLE))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DsblSshFirst);
      }

      if ((usmDbsshdNumSessionsGet(unit, &val) == L7_SUCCESS) &&
          (val > 0))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DisconnectSshSessionsFirst);
      }

      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_SSHKEY_RSA1);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_SshkeyRsa1);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_NvramSshkeyRsa2 ) == 0)
    {
      if ((usmDbsshdAdminModeGet(unit, &val) == L7_SUCCESS) &&
          (val == L7_ENABLE))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DsblSshFirst);
      }

      if ((usmDbsshdNumSessionsGet(unit, &val) == L7_SUCCESS) &&
          (val > 0))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DisconnectSshSessionsFirst);
      }

      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_SSHKEY_RSA2);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_SshkeyRsa2);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_NvramSshkeyDsa ) == 0)
    {

      if ((usmDbsshdAdminModeGet(unit, &val) == L7_SUCCESS) &&
          (val == L7_ENABLE))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DsblSshFirst);
      }

      if ((usmDbsshdNumSessionsGet(unit, &val) == L7_SUCCESS) &&
          (val > 0))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DisconnectSshSessionsFirst);
      }
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_SSHKEY_DSA);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_SshkeyDsa);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_NvramSslpemRoot ) == 0)
    {
      if ((usmDbssltAdminModeGet(unit, &val) == L7_SUCCESS) &&
          (val == L7_ENABLE))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DsblSslFirst);
      }
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_SSLPEM_ROOT);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_SslpemRoot);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_Nvramtr069acsSslpemRoot ) == 0)
    {
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_tr069_acs_SslpemRoot);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_Nvramtr069ClientSslPrivKey ) == 0)
    {
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_tr069ClientSslPrivKey);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_Nvramtr069ClientSslCert ) == 0)
    {
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_tr069ClientSslCert);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_NvramSslpemSrvr ) == 0)
    {
      if ((usmDbssltAdminModeGet(unit, &val) == L7_SUCCESS) &&
          (val == L7_ENABLE))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DsblSslFirst);
      }
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_SSLPEM_SERVER);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_SslpemSrvr);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_NvramSslpemDhweak ) == 0)
    {
      if ((usmDbssltAdminModeGet(unit, &val) == L7_SUCCESS) &&
          (val == L7_ENABLE))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DsblSslFirst);
      }
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_SSLPEM_DHWEAK);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_SslpemDhweak);
    }
    else if( strcmp( argv[index+2], pStrInfo_base_NvramSslpemDhstrong ) == 0)
    {
      if ((usmDbssltAdminModeGet(unit, &val) == L7_SUCCESS) &&
          (val == L7_ENABLE))
      {
        cliSyntaxTop(ewsContext);
        return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_DsblSslFirst);
      }
      rc = usmDbTransferUploadFileTypeSet(U_IDX, L7_FILE_TYPE_SSLPEM_DHSTRONG);
      OSAPI_STRNCPY_SAFE(urlStr, argv[index+1]);
      setFileDatatype(pStrInfo_base_SslpemDhstrong);
    }
#endif /* L7_MGMT_SECURITY_PACKAGE */
    else
    {
      OSAPI_STRNCPY_SAFE(unitUrlStr, argv[index+2]);

      return cliSyntaxReturnPrompt (ewsContext, cliGetErrSyntaxCopy());
    }
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (strncmp(urlStr, pStrInfo_security_Sftp_1, 4) == 0 ||
      strncmp(urlStr, pStrInfo_security_Scp_1, 3) == 0)
  {

    if (usmDbFeaturePresentCheck(unit, L7_FLEX_SSHD_COMPONENT_ID,
                                 L7_SSHD_SECURE_TRANSFER_FEATURE_ID) == L7_TRUE)
    {
    rc = urlSecureParser(urlStr, username, sizeof(username), strIp, sizeof(strIp),
                         filePath, sizeof(filePath), fileName, sizeof(fileName), type, sizeof(type));
  }
    else /* Secure transfer using sftp/scp is not supported. */
    {
      rc = L7_FAILURE;
    }
  }
  else
#endif /* L7_MGMT_SECURITY_PACKAGE */
  {
    rc = urlParser(urlStr, strIp, sizeof(strIp), filePath, sizeof(filePath),
      fileName, sizeof(fileName), type, sizeof(type));
  }

  if( rc != L7_SUCCESS)
  {
    cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPrompt (ewsContext, urlHelpStr);
  }
  
  if ((strcmp(argv[index+2], pStrInfo_base_IASUser) == 0) && 
#ifdef L7_MGMT_SECURITY_PACKAGE
      (strcmp(type, pStrInfo_security_Sftp_1) != 0) &&
      (strcmp(type, pStrInfo_security_Scp_1) != 0) &&
#endif /* L7_MGMT_SECURITY_PACKAGE */
      (strcmp(type, pStrInfo_common_Tftp_1) != 0))
  {
    cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPromptAddBlanks (1, 1, 0, 0, L7_NULLPTR,  ewsContext, pStrInfo_base_IASUsersDownloadError);
  }

  if( setTransferMode(type) != L7_SUCCESS)
  {
    cliSyntaxTop(ewsContext);
    return cliSyntaxReturnPrompt (ewsContext, urlHelpStr);
  }

#ifdef L7_MGMT_SECURITY_PACKAGE
  if (strcmp(type, pStrInfo_security_Sftp_1) == 0 || strcmp(type, pStrInfo_security_Scp_1) == 0)
  {
    if (usmDbSshcTransferRemoteUsernameSet(unit, username) != L7_SUCCESS)
    {
      return cliPrompt(ewsContext);
    }
    if (cliGetCharInputID() == CLI_INPUT_EMPTY)
    {
      /* special case for inputting secure transfer password */
      if (cliGetStringPassword() == L7_TRUE)
      {
        if(usmDbSshcTransferRemotePasswordSet(unit, cliGetStringInput()) != L7_SUCCESS)
        {
          return cliPrompt(ewsContext);
        }
      }
      else
      {
        if (cliNumFunctionArgsGet() > 2)
        {
          osapiSnprintf(cmd, sizeof(cmd), pStrInfo_base_Copy_7, argv[index+ 1], argv[index+ 2], argv[index+ 3]);
        }
        else
        {
          osapiSnprintf(cmd, sizeof(cmd), pStrInfo_base_Copy_6, argv[index+ 1], argv[index+ 2]);
        }
        cliAlternateCommandSet(cmd);
        cliSetStringInputID(1, ewsContext, argv);
        cliSetStringPassword();
        return pStrInfo_base_RemotePasswdPrompt;
      }
    }
  }
#endif /* L7_MGMT_SECURITY_PACKAGE */

  if( strcmp(type, pStrInfo_common_Tftp_1 ) == 0
#ifdef L7_MGMT_SECURITY_PACKAGE
      || strcmp(type, pStrInfo_security_Sftp_1 ) == 0
      || strcmp(type, pStrInfo_security_Scp_1 ) == 0
#endif /* L7_MGMT_SECURITY_PACKAGE */
      )
  {
    if (osapiInetPton(L7_AF_INET6, strIp, ipv6Addr) != L7_SUCCESS)
    {
      if (cliIPHostAddressValidate(ewsContext, strIp, &ipAddr, 
                                   &addrType) != L7_SUCCESS)
      {
        /* Invalid Host Address*/
        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_EmptyString);
      }
    }
    if( setServerIP(strIp) != L7_SUCCESS)
    {
      cliSyntaxTop(ewsContext);
      return cliSyntaxReturnPrompt (ewsContext, pStrErr_base_DnsLookupFailed);
    }
    else if(strlen(filePath) > L7_MAX_FILEPATH)
    {
      return cliSyntaxReturnPromptAddBlanks(2, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_IncorrectFilePathLength, L7_MAX_FILEPATH );
    }
    else if( usmDbTransferFilePathRemoteSet(unit, filePath) != L7_SUCCESS)
    {
      cliSyntaxTop(ewsContext);
      ewsTelnetWrite( ewsContext, urlHelpStr);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_CrLf);
    }
    else if(strlen(fileName) > L7_MAX_FILENAME)
    {
      return cliSyntaxReturnPromptAddBlanks(2, 1, 0, 0, pStrErr_common_Error,  ewsContext, pStrInfo_base_IncorrectFileNameLength, L7_MAX_FILENAME );
    }
    else if( usmDbTransferFileNameRemoteSet(unit, fileName) != L7_SUCCESS)
    {
      cliSyntaxTop(ewsContext);
      ewsTelnetWrite( ewsContext, urlHelpStr);
      return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_CrLf);
    }

    fileType = fileName;
  }

  if (upload == L7_TRUE)
  {
    rc = usmDbTransferUploadFileTypeGet(unit, &val);
  }
  else
  {
    rc = usmDbTransferDownloadFileTypeGet(unit, &val);
  }

  if (val == L7_FILE_TYPE_CONFIG_SCRIPT)
  {
    OSAPI_STRNCPY_SAFE(scriptRemoteFilename, fileName);
    usmDbTransferFileNameRemoteSet(unit, scriptRemoteFilename);
    usmDbTransferFileNameLocalSet(unit, scriptLocalFilename);
    setFileDatatype(pStrInfo_base_Script);

    if (checkConfigScriptFiletype(scriptLocalFilename) != L7_SUCCESS)
    {

      cliSyntaxBottom(ewsContext);
      return cliSyntaxReturnPrompt (ewsContext, getLastError());
    }
  }
  else if( val == L7_FILE_TYPE_CODE)
  {
    usmDbTransferFileNameLocalSet(unit, localFileName);
    setFileDatatype(pStrInfo_base_Code_3);
  }
  else if (val == L7_FILE_TYPE_IAS_USERS)
  {
    usmDbTransferFileNameRemoteSet(unit, fileName);
    usmDbTransferFileNameLocalSet(unit, fileName);
    setFileDatatype(pStrInfo_base_IASUser);
  }

  if (upload == L7_TRUE)
  {
    return transferUploadStart(ewsContext, argc, argv, index, strIp);
  }
  return transferDownloadStart(ewsContext, argc, argv, index, L7_FALSE, strIp);

}

/*********************************************************************
 *
 * @purpose  starts the upload from the switch
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax transfer upload start
 *
 * @cmdhelp Initiate upload.
 *
 * @cmddescript
 *   Uploading is the transfer of files from the switch to a remote
 *   server. The upload operation is initiated by this command.
 *
 * @end
 *
 *********************************************************************/
static const L7_char8 *transferUploadStart(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv, L7_uint32 index, L7_char8 *host)
{
  L7_uint32 unit = 1;
  L7_uint32 rc;
  L7_uint32 val;
  L7_uint32 transferMode;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cmd[2*L7_CLI_MAX_STRING_LENGTH];

  memset(cmd, 0, sizeof(cmd));
  rc = usmDbTransferModeGet(unit, &transferMode);

  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    /* if our question has been answered */
    if (tolower(cliGetCharInput()) == 'y')
    {
      /* yes */
      usmDbTransferContextSet(unit, (void *)ewsContext);

      rc = usmDbTransferUpStartSet(unit);
      if (rc == L7_SUCCESS)
      {
        ewsSuspend(ewsContext);
      }
      else if (rc == L7_IMAGE_IN_USE)
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWrite(ewsContext, pStrInfo_base_ImageInUse);
        cliSyntaxBottom(ewsContext);
      }
      else
      {
        cliSyntaxTop(ewsContext);
        ewsTelnetWrite(ewsContext, pStrInfo_base_FileTransferInProgress);
        cliSyntaxBottom(ewsContext); 
      } 
    }
    else
    {
      /* no */
      cliSyntaxTop(ewsContext);
      ewsTelnetWrite(ewsContext, pStrInfo_base_FileTransferCanceled);
      cliSyntaxBottom(ewsContext);
    }
    return cliPrompt(ewsContext);
  }
  else
  {
    cliSetCharInputID(1, ewsContext, argv);

    cliWrite(pStrInfo_common_CrLf);
    cliFormat(ewsContext,pStrInfo_common_Mode_1);
    rc = usmDbTransferModeGet(unit, &val);
    ewsTelnetWrite(ewsContext, strUtilTransferModeGet(val, L7_NULLPTR));

    if (transferMode == L7_TRANSFER_TFTP ||
        transferMode == L7_TRANSFER_SFTP ||
        transferMode == L7_TRANSFER_SCP)
    {
      cliFormat(ewsContext,pStrInfo_base_TransferSrvrIp);
      ewsTelnetWrite(ewsContext, host);

      cliFormat(ewsContext,pStrInfo_base_TftpPath);
      memset(buf, 0, sizeof(buf));
      rc = usmDbTransferFilePathRemoteGet(unit, buf);
      ewsTelnetWrite(ewsContext, buf);

      cliFormat(ewsContext,pStrInfo_base_TftpFilename);
      memset(buf, 0, sizeof(buf));
      rc = usmDbTransferFileNameRemoteGet(unit, buf);
      ewsTelnetWrite( ewsContext, buf);
    }

    cliFormat(ewsContext,pStrInfo_base_DataType);
    rc = usmDbTransferUploadFileTypeGet(unit, &val);
    ewsTelnetWrite( ewsContext, strUtilFileTypeGet(val, L7_NULLPTR));

    if (argc <= 3)            /* [0, 1, 2]  */
    {
      osapiSnprintf(cmd, sizeof(cmd), pStrInfo_base_Copy_6, argv[index+ 1], argv[index+ 2]);
    }
    else
    {
      osapiSnprintf(cmd, sizeof(cmd), pStrInfo_base_Copy_7, argv[index+ 1], argv[index+ 2], argv[index+ 3]);
    }

    if (val == L7_FILE_TYPE_CONFIG_SCRIPT)
    {
      cliFormat(ewsContext,pStrInfo_base_SrcFilename);
      ewsTelnetWrite( ewsContext, argv[index+ 2]);
    }

    cliSyntaxBottom(ewsContext);
    cliAlternateCommandSet(cmd);

    cliSyntaxTop(ewsContext);
    ewsTelnetWrite(ewsContext, pStrInfo_base_FileTransferPreConfirm);
    cliSyntaxTop(ewsContext);

    return pStrInfo_base_FileTransferConfirm;
  }

  return cliPrompt(ewsContext);
}

/*********************************************************************
 *
 * @purpose  sets the download data file type
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax transfer upload datatype <code/config>
 *
 * @cmdhelp Set File Type.
 *
 * @cmddescript
 *   Specify the file type to Download.
 *               Code (the default)  or
 *               Configuration
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t  setFileDatatype(L7_char8 * dataType)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uint32 unit = 1;

  if (strcmp( dataType, pStrInfo_base_Code_3 ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(unit, L7_FILE_TYPE_CODE);
  }
  else if (strcmp(dataType, pStrInfo_base_IASUser) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(unit, L7_FILE_TYPE_IAS_USERS);
  }
  else if (strcmp( dataType, pStrInfo_base_Cfg_7 ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(unit, L7_FILE_TYPE_CONFIG);
  }
  else if (strcmp( dataType, pStrInfo_base_Txtcfg ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(unit, L7_FILE_TYPE_TXTCFG);
  }
  else if (strcmp( dataType, pStrInfo_base_Script ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(unit, L7_FILE_TYPE_CONFIG_SCRIPT);
  }
  else if (strcmp( dataType, pStrInfo_base_Banner ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(unit, L7_FILE_TYPE_CLI_BANNER);
  }
  else if (strcmp( dataType, pStrInfo_base_SshkeyRsa1 ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_SSHKEY_RSA1);
  }
  else if (strcmp( dataType, pStrInfo_base_SshkeyRsa2 ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_SSHKEY_RSA2);
  }
  else if (strcmp( dataType, pStrInfo_base_SshkeyDsa ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_SSHKEY_DSA);
  }
  else if (strcmp( dataType, pStrInfo_base_SslpemRoot ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_SSLPEM_ROOT);
  }
  else if (strcmp( dataType, pStrInfo_base_tr069_acs_SslpemRoot ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT);
  }
  else if (strcmp( dataType, pStrInfo_base_tr069ClientSslPrivKey ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY);
  }
  else if (strcmp( dataType, pStrInfo_base_tr069ClientSslCert ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_TR069_CLIENT_SSL_CERT);
  }
  else if (strcmp( dataType, pStrInfo_base_SslpemSrvr ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_SSLPEM_SERVER);
  }
  else if (strcmp( dataType, pStrInfo_base_SslpemDhweak ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_SSLPEM_DHWEAK);
  }
  else if (strcmp( dataType, pStrInfo_base_SslpemDhstrong ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_SSLPEM_DHSTRONG);
  }
  else if (strcmp( dataType, pStrInfo_base_Kernel ) == 0)
  {
    rc = usmDbTransferDownloadFileTypeSet(U_IDX, L7_FILE_TYPE_KERNEL);
  }

  return rc;
}

/*********************************************************************
 *
 * @purpose  start the download
 *
 *
 * @param EwsContext ewsContext
 * @param L7_uint32 argc
 * @param const L7_char8 **argv
 * @param L7_uint32 index
 *
 *
 * @returntype const L7_char8  *
 * @returns cliPrompt(ewsContext)
 *
 * @notes none
 *
 * @cmdsyntax transfer upload start
 *
 * @cmdhelp Initiate download.
 *
 * @cmddescript
 *   Downloading is the transfer of files from a remote server into
 *   the switch. The download operation is initiated by this command.
 *
 * @end
 *
 *********************************************************************/
static const L7_char8 *transferDownloadStart(EwsContext ewsContext, L7_uint32 argc, const L7_char8 * * argv,
                              L7_uint32 index, L7_BOOL archive, L7_char8 *host)
{
  L7_uint32 unit = 1;
  L7_uint32 rc;
  L7_uint32 val;
  L7_uint32 transferMode;
  L7_char8 buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 cmd[2*L7_CLI_MAX_STRING_LENGTH];
  memset(cmd, 0, sizeof(cmd));
  ewsContext->unbufferedWrite = L7_TRUE;

  osapiSnprintf(cmd, sizeof(cmd), pStrInfo_base_Copy_6, argv[index+1], argv[index+2]);

  rc = usmDbTransferModeGet(unit, &transferMode);
  if (cliGetCharInputID() != CLI_INPUT_EMPTY)
  {
    /* if our question has been answered */
    if (tolower(cliGetCharInput()) == 'y')
    {
      /* yes */
      usmDbTransferContextSet(unit, (void *)ewsContext);
      rc = usmDbTransferDownStartSet(unit);

      /* check if there are any errors */

      if(rc != L7_SUCCESS)
      {

        cliSyntaxTop(ewsContext);

        if(rc == L7_REQUEST_DENIED)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_ActiveImage);
        }
        else if (rc == L7_ALREADY_CONFIGURED)
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_base_ActivateDbAckUp);
        }
        else 
        {
          ewsTelnetWriteAddBlanks (1, 0, 0, 0, pStrErr_common_Error,  ewsContext, pStrErr_common_FileTransferNotAllowed);
        }

        return cliSyntaxReturnPrompt (ewsContext, pStrInfo_common_CrLf);
      }

      ewsSuspend(ewsContext);
    }
    else
    {
      /* no */
      ewsTelnetWrite(ewsContext, pStrInfo_base_FileTransferCanceled);
      cliSyntaxBottom(ewsContext);
    }
    ewsContext->unbufferedWrite = L7_TRUE;
    return cliPrompt(ewsContext);
  }
  else
  {

    rc = usmDbTransferDownloadFileTypeGet(unit, &val);

    memset(buf, 0, sizeof(buf));
    rc = usmDbTransferFileNameLocalGet(unit, buf);

    /*check if file allowed*/
    if(val == L7_FILE_TYPE_CONFIG_SCRIPT && checkConfigScriptCountLimit(buf) != L7_SUCCESS)
    {
      cliSyntaxBottom(ewsContext);
      ewsTelnetWrite( ewsContext, getLastError());
      ewsContext->unbufferedWrite = L7_TRUE;
      return cliSyntaxReturnPrompt (ewsContext, "");
    }

    cliSetCharInputID(1, ewsContext, argv);

    cliSyntaxTop(ewsContext);

    cliFormat(ewsContext,pStrInfo_common_Mode_1);
    rc = usmDbTransferModeGet(unit, &val);
    ewsTelnetWrite(ewsContext, strUtilTransferModeGet(val, L7_NULLPTR));

    if (transferMode == L7_TRANSFER_TFTP ||
        transferMode == L7_TRANSFER_SFTP ||
        transferMode == L7_TRANSFER_SCP)
    {
      cliFormat(ewsContext,pStrInfo_base_TransferSrvrIp);
      ewsTelnetWrite(ewsContext, host);

      cliFormat(ewsContext,pStrInfo_base_TftpPath);
      memset(buf, 0, sizeof(buf));
      rc = usmDbTransferFilePathRemoteGet(unit, buf);
      ewsTelnetWrite( ewsContext, buf);

      cliFormat(ewsContext,pStrInfo_base_TftpFilename);
      memset(buf, 0, sizeof(buf));
      rc = usmDbTransferFileNameRemoteGet(unit, buf);
      ewsTelnetWrite( ewsContext, buf);
    }

    cliFormat(ewsContext,pStrInfo_base_DataType);
    rc = usmDbTransferDownloadFileTypeGet(unit, &val);
    ewsTelnetWrite( ewsContext, strUtilFileTypeGet(val, L7_NULLPTR));

    if (val == L7_FILE_TYPE_CONFIG)
    {
      cliSyntaxTop(ewsContext);
      ewsTelnetWrite(ewsContext, pStrInfo_base_FileTransferCfgDownLoad);
      cliSyntaxBottom(ewsContext);
    }
    else if (val == L7_FILE_TYPE_TXTCFG)
    {
      cliSyntaxTop(ewsContext);
      ewsTelnetWrite(ewsContext, pStrInfo_base_FileTransferTxtCfgDownLoad);
      cliSyntaxBottom(ewsContext);
    }
    else if(val == L7_FILE_TYPE_CONFIG_SCRIPT)
    {
      /*check for existing file*/
      osapiSnprintf(cmd, sizeof(cmd), pStrInfo_base_Copy_7, argv[index+ 1], argv[index+ 2], argv[index+ 3]);
      memset(buf, 0, sizeof(buf));
      rc = usmDbTransferFileNameLocalGet(unit, buf);

      cliFormat(ewsContext,pStrInfo_base_DstFilename);
      ewsTelnetWrite( ewsContext, buf);

      if (L7_SUCCESS == checkConfigScriptPresent(buf))
      {
        cliSyntaxBottom(ewsContext);
        cliSyntaxTop(ewsContext);
        ewsTelnetWrite(ewsContext, pStrWarn_base_FileTransferCfgScriptOverWrite);
        cliSyntaxBottom(ewsContext);
      }
    }
    else if(val == L7_FILE_TYPE_CODE)
    {
      /*check for existing file*/
      char activeFileName[DIM_MAX_FILENAME_SIZE];

      memset(buf, 0, sizeof(buf));
      rc = usmDbTransferFileNameLocalGet(unit, buf);

      cliFormat(ewsContext,pStrInfo_base_DstFilename);
      ewsTelnetWrite( ewsContext, buf);

      (void) usmDbActiveImageNameGet(unit, activeFileName);
      if (strcmp(activeFileName,buf) == 0)
      {
        cliSyntaxBottom(ewsContext);
        cliSyntaxTop(ewsContext);
        ewsTelnetWrite(ewsContext, pStrWarn_base_FileTransferCodeFileOverWrite);
        cliSyntaxBottom(ewsContext);
      }
    }

    if (argc <= 3)            /* [0, 1, 2]  */
    {
      osapiSnprintf(cmd, sizeof(cmd), pStrInfo_base_Copy_6, argv[index+ 1], argv[index+ 2]);
    }
    else
    {
      osapiSnprintf(cmd, sizeof(cmd), pStrInfo_base_Copy_7, argv[index+ 1], argv[index+ 2], argv[index+ 3]);
    }

    cliSetCharInputID(1, ewsContext, argv);

    cliAlternateCommandSet(cmd);
    cliSyntaxBottom(ewsContext);

    cliSyntaxTop(ewsContext);
    ewsTelnetWrite(ewsContext, pStrInfo_base_FileTransferPreConfirm);
    cliSyntaxTop(ewsContext);

    return pStrInfo_base_FileTransferConfirm;
  }
}

/*********************************************************************
 *
 * @purpose  check for already running file transfer in progress
 *
 * @param EwsContext ewsContext, L7_uint32 unit, const L7_char8 **argv, L7_BOOL upload
 *
 * @returns const L7_char8 *
 *
 * @notes none
 *
 * @cmddescript
 *     This function will check for file transfer already in progress.
 *     If there is any state change appropriate message is displayed.
 *     If the file transfer has completed it will display the message
 *     to user else it will suspend. This will also be generally be
 *     called when resumed from suspended state
 *
 * @end
 *
 *********************************************************************/

static const L7_char8 * isTransferInProgress(EwsContext ewsContext, L7_uint32 unit, L7_uint32 argc,const L7_char8 * * argv, L7_BOOL upload, L7_uint32 index)
{
  L7_uint32 rc, val;
  L7_char8  buf[MAX_STRING_LEN + 1];
  L7_char8  errBuf[MAX_STRING_LEN + 1];
  L7_char8  bufFileName[MAX_STRING_LEN + 1];
  L7_char8  cmd[2*L7_CLI_MAX_STRING_LENGTH];
#ifdef _L7_OS_VXWORKS_
  L7_uint32 transType, transMode, perc_val = 0;
  static L7_BOOL transferFlag = L7_FALSE;
  L7_char8 perc_val_buf[L7_CLI_MAX_STRING_LENGTH + 1];
  L7_uint32 fileType;
#endif
  L7_BOOL bufval;
  L7_uint32 filetype;
  L7_uint32 stkImageStatus;

  memset(bufFileName, 0, sizeof(bufFileName));
  memset(buf, 0, sizeof(buf));
  memset(errBuf, 0, sizeof(errBuf));
  memset(cmd, 0, sizeof(cmd));

  if (usmDbTransferInProgressGet(unit))
  {
    if (ewsContext != (EwsContext)usmDbTransferContextGet(unit))
    {
      cliSyntaxTop(ewsContext);
      ewsTelnetWrite(ewsContext, pStrInfo_base_FileTransferInProgress);
      cliSyntaxBottom(ewsContext);
    }
    else
    {
      /* Check transfer status and report */
      memset(buf, 0, sizeof(buf));
      rc = usmDbTransferResultGet(unit, &val, buf);

      if (val == L7_STATUS_CHANGED)
      {
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        ewsFlushAll(ewsContext);
      }
#ifdef _L7_OS_VXWORKS_
      /* Print dots periodically indicate transfer progress */
      rc = usmDbTransferModeGet(unit, &transMode);
      if ((transMode == L7_TRANSFER_TFTP) ||
          (transMode == L7_TRANSFER_FTP)  ||
          (transMode == L7_TRANSFER_SFTP) ||
          (transMode == L7_TRANSFER_SCP) )
      {
        if (upload == L7_FALSE)
        {
          rc = usmDbTransferDownloadResultCodeGet(unit, &transType);
        }
        else
        {
          rc = usmDbTransferUploadResultCodeGet(unit, &transType);
        }

        usmDbTransferDownloadFileTypeGet(unit, &fileType);
        if (transType == L7_TRANSFER_CODE_STARTING)
        {
          if (transferFlag == L7_FALSE)
          {
            cliSyntaxTop(ewsContext);
            transferFlag = L7_TRUE;
          }

          memset(perc_val_buf, 0x00, sizeof(perc_val_buf));
          perc_val = usmDbTransferCompleteStatus();
          osapiSnprintf(perc_val_buf, sizeof(perc_val_buf), "\r%d bytes transferred...", perc_val);
          ewsTelnetWrite(ewsContext, perc_val_buf);
          ewsFlushAll(ewsContext);
        }
        else if (transType == L7_TRANSFER_CODE_SUCCESS)
        {
          if (fileType != L7_FILE_TYPE_CODE || upload == L7_TRUE) 
          { 
            memset(perc_val_buf, 0x00, sizeof(perc_val_buf));
            perc_val = usmDbTransferCompleteStatus();
            osapiSnprintf(perc_val_buf, sizeof(perc_val_buf), "\r%d bytes transferred   ", perc_val);
            ewsTelnetWrite(ewsContext, perc_val_buf);
            cliSyntaxBottom(ewsContext);
            ewsFlushAll(ewsContext);
            transferFlag = L7_FALSE;
          }
        }
      }
#endif
      if (val >= L7_TRANSFER_FAILED)
      {
        if(upload == L7_FALSE) /*  transfer is a download*/
        {
          cliSyntaxTop(ewsContext);

          rc = usmDbTransferDownloadFileTypeGet(unit, &filetype);
        
          if ((val == L7_TRANSFER_SUCCESS) && (filetype == L7_FILE_TYPE_CONFIG_SCRIPT))
          {
            rc = L7_FAILURE;

            memset(bufFileName, 0, sizeof(bufFileName));
            if (usmDbTransferFileNameLocalGet(unit, bufFileName) == L7_SUCCESS)
            {
              rc = configScriptValidateAndDownload(ewsContext, TEMP_CONFIG_SCRIPT_FILE_NAME, bufFileName,argc);
            }

            if (rc == L7_SUCCESS)
            {
              cliSyntaxTop(ewsContext);
              ewsTelnetWrite(ewsContext, pStrInfo_base_FileTransferOperationCompletedSuccessfully);
            }
            else if (rc == L7_ERROR)
            {
              cliSyntaxTop(ewsContext);
              ewsTelnetWrite(ewsContext, pStrErr_common_FileTransferFailed);
            }
            else if (rc == L7_FAILURE) /*confirm from user*/
            {
              cliSetCharInputID(3, ewsContext, argv);
              osapiSnprintf(cmd, sizeof(cmd), "copy %s %s %s", argv[index + 1], argv[index + 2], argv[index + 3]);
              cliAlternateCommandSet(cmd);

              usmDbTransferInProgressSet(unit, L7_FALSE);
              usmDbTransferSuspendMgmtAccessSet(unit, L7_FALSE);
              usmDbTransferContextSet(unit, NULL);

              cliSyntaxTop(ewsContext);
              return pStrErr_base_FileTransferScriptDownLoadConfirm;
            }
            /*erase previous message*/
            memset(buf, 0, sizeof(buf));
          }
          if ((val != L7_TRANSFER_SUCCESS) && (filetype == L7_FILE_TYPE_CODE))
          {
            rc = usmDbTransferDownloadCodeStatusGet(unit, &stkImageStatus);
            if ((rc == L7_SUCCESS) && (stkImageStatus != STK_SUCCESS))
            {
              if (cliUtilGetStkErrorString(stkImageStatus, errBuf, sizeof(errBuf)) == L7_SUCCESS)
              {
                cliSyntaxTop(ewsContext);
                ewsTelnetWrite(ewsContext, errBuf);
              }
            }
          }
        }
        /* Transfer completed */
        usmDbTransferInProgressSet(unit, L7_FALSE);
        usmDbTransferSuspendMgmtAccessSet(unit, L7_FALSE);
        usmDbTransferContextSet(unit, NULL);
        bufval = ewsContext->unbufferedWrite; 
        ewsContext->unbufferedWrite = L7_TRUE;  
        cliSyntaxTop(ewsContext);
        ewsTelnetWrite(ewsContext, buf);
        cliSyntaxBottom(ewsContext);
        ewsContext->unbufferedWrite = bufval;  
      }
      else
      {
        /* Transfer is still in progress, wait a little longer */
        ewsSuspend( ewsContext );
      }
    }

    return cliPrompt(ewsContext);
  }

  return L7_NULL;
}
