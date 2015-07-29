/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  transfer.c
*
* @purpose   This file implements file transfer functionality.
*
* @component os
*
* @comments  none
*
* @create    08/28/2003 (rewrite)
*
* @author    John W. Linville
*
* @end
*
*********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "l7_common.h"
#include "iplsec.h"
#include "osapi.h"
#include "log_ext.h"
#include "config_script_api.h"
#include "cli_web_mgr_api.h"

#include "usmdb_sim_api.h"
#include "usmdb_cda_api.h"
#include "usmdb_tr069_api.h"

#include "bspapi.h"
#include "stk.h"
#include "osapi_support.h"
#include "l7_tftp.h"

#include "sshd_exports.h"
#include "sslt_exports.h"
#include "tr069_exports.h"
#include "trapapi.h"
#include "sshd_api.h"

extern L7_RC_t logWriteInMemoryLogToFile(L7_char8 * fileName);
extern void cliTxtCfgSaveTransConfig (L7_char8 * filename);
extern L7_RC_t cliTxtCfgCopyConfig (L7_char8 * readFile, L7_char8 * writeFile, L7_BOOL useComp);
extern L7_RC_t IASUsersScriptValidateAndApply(L7_char8 * scriptFileName, L7_char8 * newFilename);
extern L7_RC_t trapMgrCreateTrapLogFile (L7_char8 * filename);
extern L7_int32 osapiSystem(const char *fmt, ...);
extern L7_RC_t usmDbDsTftpDownloadStatus(L7_uint32 direction, L7_BOOL status);
extern L7_RC_t osapiSwapFile(L7_char8 * filename);
extern L7_ushort16 file_crc_compute(L7_uchar8 *file_name, L7_uint32 file_size);
#ifdef L7_MGMT_SECURITY_PACKAGE
extern int sshcTransferSftp(L7_char8 *host, L7_int32 address_family,
                            L7_uint32 clisocket, L7_char8 *remote_file,
                            L7_char8 *local_file, L7_uint32 cmd);
extern int sshcTransferScp(L7_char8 *hostname, L7_int32 address_family,
                           L7_uint32 clisocket, L7_char8 *remote_file,
                           L7_char8 *local_file, L7_uint32 updownflag);
#endif /* L7_MGMT_SECURITY_PACKAGE */

L7_uint32 L7_http_dl_data_last_seen;
static osapiTimerDescr_t *pHttpTimer         = L7_NULLPTR;

L7_uint32 L7_http_dl_data_last_seen = 0;

static pthread_once_t transfer_once = PTHREAD_ONCE_INIT;

static void *L7_transferSignalSem;
static void *L7_transferParmSem;
static void *L7_transferStatSem;

/* Obviously, all these global variables lead to reentrancy restrictions... */

static L7_int32 transfer_started=0;
static L7_int32 transfer_ftype;
static L7_int32 transfer_direction;
static L7_int32 transfer_type=0;
static L7_char8 transfer_fname_local[130];
static L7_char8 transfer_fname_remote[130];
static L7_inet_addr_t transfer_ip_addr;
static L7_char8 transfer_string_status[0x80];
static L7_char8 transfer_temp_string_status[0x80];
static L7_int32 transfer_code_upload_status = L7_TRANSFER_CODE_NONE;
static L7_int32 transfer_code_download_status = L7_TRANSFER_CODE_NONE;
static L7_int32 code_status = STK_SUCCESS;
static L7_int32 transfer_result;



/**************************************************************************
 *
 * @purpose: Creates a text file in the RAM disk from valgrind log file
 *
 * @param    valgrindLogCopy   @b{(input)} name of file to create
 *
 * @returns  L7_SUCCESS if successful
 * @returns  L7_ERROR if file can not be opened
 *
 * @end
 *
 *************************************************************************/
#ifdef L7_TOOL_VALGRIND
L7_RC_t valgrindLogCopy (L7_char8 * outputFileName)
{

  static L7_char8 buf[0x80];

  osapiSnprintf(buf,sizeof(buf), "%s%s",VALG_LOG_PATH, VALG_LOG_FILE_NAME);
  osapiFsCopyFile(buf,outputFileName);


  return L7_SUCCESS;
}
#endif


/**************************************************************************
 *  NAME: transfer_init
 *        This function inits the tranfer code
 *
 *  RETURNS:
 *
 *************************************************************************/
static void transfer_init(void)
{
   /* Create semaphore to signal transfer commencement... */
   L7_transferSignalSem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);

   /* Create semaphore to lock transfer parameter access... */
   L7_transferParmSem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);

   /* Create semaphore to lock transfer status access... */
   L7_transferStatSem = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
}

/**************************************************************************
 *  NAME: set_result
 *        This function sets the up/down result.
 *
 *  RETURNS:
 *
 *************************************************************************/
static void set_result(L7_int32 result)
{
   L7_RC_t rc;

   pthread_once(&transfer_once, transfer_init);

   /* lock transfer parameters... */
   if ((rc = osapiSemaTake(L7_transferStatSem, L7_WAIT_FOREVER))
    == L7_SUCCESS) {

      transfer_result = result;

      if ((rc = osapiSemaGive(L7_transferStatSem)) != L7_SUCCESS) {

         /* couldn't give lock? */
         L7_LOG_ERROR(rc);

      }

   } else {

      /* lock doesn't exist? */
      L7_LOG_ERROR(rc);

   }
}

/**************************************************************************
 *  NAME: set_result_string
 *        This function sets the up/down result string.
 *
 *  RETURNS:
 *
 *************************************************************************/
static void set_result_string(L7_char8 *string)
{
   L7_RC_t rc;

   pthread_once(&transfer_once, transfer_init);

   /* lock transfer parameters... */
   if ((rc = osapiSemaTake(L7_transferStatSem, L7_WAIT_FOREVER))
    == L7_SUCCESS) {

      osapiStrncpySafe(transfer_string_status, string,
              sizeof(transfer_string_status));

      if ((rc = osapiSemaGive(L7_transferStatSem)) != L7_SUCCESS) {

         /* couldn't give lock? */
         L7_LOG_ERROR(rc);

      }

   } else {

      /* lock doesn't exist? */
      L7_LOG_ERROR(rc);

   }
}

/**************************************************************************
 *  NAME: set_transfer_string
 *        This function sets the transfer string.
 *
 *  RETURNS:
 *
 *************************************************************************/
static void set_transfer_string(L7_char8 *string, L7_uint32 maxlen)
{
   switch (transfer_type) {

      case L7_TRANSFER_TFTP:
         snprintf(string, maxlen, "TFTP ");
         break;

      case L7_TRANSFER_SFTP:
         snprintf(string, maxlen, "SFTP ");
         break;

      case L7_TRANSFER_SCP:
         snprintf(string, maxlen, "SCP ");
         break;

     case L7_TRANSFER_XMODEM:
         snprintf(string, maxlen, "Xmodem ");
         break;

      case L7_TRANSFER_YMODEM:
         snprintf(string, maxlen, "Ymodem ");
         break;

      case L7_TRANSFER_ZMODEM:
         snprintf(string, maxlen, "Zmodem ");
         break;

      case L7_TRANSFER_HTTP:
         snprintf(string, maxlen, "HTTP ");
         break;

      default:
         snprintf(string, maxlen, "Unknown protocol ");
         break;

   }

   switch (transfer_ftype) {

      case L7_FILE_TYPE_CODE:
         strncat(string, "Code ", (maxlen - strlen(string)));
         break;
#ifdef L7_DHCP_SNOOPING_PACKAGE
     case L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS:
         strncat(string, "Code ", (maxlen - strlen(string)));
     break;
#endif


      case L7_FILE_TYPE_CONFIG:
         strncat(string, "Config ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_IAS_USERS:
         strncat(string, "IAS Users ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_TXTCFG:
         strncat(string, "Text Config ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_ERRORLOG:
         strncat(string, "Error Log ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_SYSTEM_MSGLOG:
         strncat(string, "Message Log ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_TRAP_LOG:
         strncat(string, "Trap Log ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_SSHKEY_RSA1:
      case L7_FILE_TYPE_SSHKEY_RSA2:
      case L7_FILE_TYPE_SSHKEY_DSA:
        strncat(string, "Host key ", (maxlen - strlen(string)));
        break;

      case L7_FILE_TYPE_SSLPEM_ROOT:
      case L7_FILE_TYPE_SSLPEM_SERVER:
      case L7_FILE_TYPE_SSLPEM_DHWEAK:
      case L7_FILE_TYPE_SSLPEM_DHSTRONG:
      case L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
         strncat(string, "Client key ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT :
      case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
         strncat(string, "Certificate ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_CONFIG_SCRIPT:
         strncat(string, "Configuration Script ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_CLI_BANNER:
          strncat(string, "CLI Banner ", (maxlen - strlen(string)));
          break;

      case L7_FILE_TYPE_KERNEL:
         strncat(string, "Kernel ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_VALGLOG:
         strncat(string, "Valgrind Log ", (maxlen - strlen(string)));
         break;

      case L7_FILE_TYPE_AUTO_INSTALL_SCRIPT:
         strncat(string, "Auto-Install ", (maxlen - strlen(string)));
         break;

      default:
         strncat(string, "Unknown filetype ", (maxlen - strlen(string)));
         break;

   }

   strncat(string, "transfer starting...", (maxlen - strlen(string)));

   set_result_string(string);
}

/**************************************************************************
 *  NAME: result_string
 *        This function takes the string given to it and uses XIO functions
 *        to set the up/down result string.
 *
 *  RETURNS:
 *
 *************************************************************************/
void result_string(L7_char8 *string)
{
  set_result_string(string);
}

/**************************************************************************
 *  NAME: set_upload_result_code
 *        This function sets the upload result code.
 *
 *  RETURNS:
 *
 *************************************************************************/
static void set_upload_result_code(L7_int32 code)
{
   L7_RC_t rc;

   pthread_once(&transfer_once, transfer_init);

   /* lock transfer parameters... */
   if ((rc = osapiSemaTake(L7_transferStatSem, L7_WAIT_FOREVER))
    == L7_SUCCESS) {

      transfer_code_upload_status = code;

      if ((rc = osapiSemaGive(L7_transferStatSem)) != L7_SUCCESS) {

         /* couldn't give lock? */
         L7_LOG_ERROR(rc);

      }

   } else {

      /* lock doesn't exist? */
      L7_LOG_ERROR(rc);

   }
}

/**************************************************************************
 *  NAME: set_download_result_code
 *        This function sets the download result code.
 *
 *  RETURNS:
 *
 *************************************************************************/
static void set_download_result_code(L7_int32 code)
{
   L7_RC_t rc;

   pthread_once(&transfer_once, transfer_init);

   /* lock transfer parameters... */
   if ((rc = osapiSemaTake(L7_transferStatSem, L7_WAIT_FOREVER))
    == L7_SUCCESS) {

      transfer_code_download_status = code;

      if ((rc = osapiSemaGive(L7_transferStatSem)) != L7_SUCCESS) {

         /* couldn't give lock? */
         L7_LOG_ERROR(rc);

      }

   } else {

      /* lock doesn't exist? */
      L7_LOG_ERROR(rc);

   }
}

/**************************************************************************
 *  NAME: transfer_tftp
 *        This function transfers a file using tftp
 *
 *  RETURNS:
 *
 *************************************************************************/
static L7_RC_t transfer_tftp(L7_inet_addr_t *inet_address, L7_char8 *remote_filename,
                             L7_char8 *local_filename, L7_int32 direction)
{
   /* We're only called on one task's thread; use static buffers to save
      stack space */
   static char buf[0x200];
   static L7_char8 server_ip[100];
   L7_RC_t rc = L7_SUCCESS;

   bzero(server_ip, sizeof(server_ip));

   if (inet_address->family == L7_AF_INET)
   {
      if (osapiInetNtop(L7_AF_INET, (L7_uchar8 *)&(inet_address->addr.ipv4),
                          (L7_uchar8 *)server_ip, sizeof(server_ip))
          == L7_NULL)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
                "osapiInetNtop error for %s\n",server_ip);
        return L7_FAILURE;
      }
   }
   else if (inet_address->family == L7_AF_INET6)
   {
      if (osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&(inet_address->addr.ipv6.in6),
                          (L7_uchar8 *)server_ip, sizeof(server_ip))
          == L7_NULL)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
                "osapiInetNtop error for %s\n",server_ip);
        return L7_FAILURE;
      }

   }
   else
   {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
                "Address Family is not Supported \n");
        return L7_FAILURE;
   }
   snprintf(buf, sizeof(buf), "tftp %s -r %s -l %s %s >/dev/null 2>&1",
              ((direction == L7_TRANSFER_DIRECTION_UP) ? "-p" : "-g"),
              remote_filename, local_filename, server_ip);

   if (WEXITSTATUS(system(buf)) != 0)
   {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
             "tftp command failed for %s\n",server_ip);
     return (L7_FAILURE);
   }
   return rc;
}

#if defined(FEAT_METRO_CPE_V1_0)
/**************************************************************************
 *  NAME: transfer_download_memcheck
 *        This function checks the Kernel Free memory before Transfer
 *        download utility called on ROBO
 *
 *  RETURNS:
 *
 *************************************************************************/
static L7_RC_t transfer_download_memcheck(L7_uint32 *freeMem)
{
#define LINUX_MEM_INFO_FILE_NAME "/proc/meminfo"

  L7_int32 filedesc;
  L7_uint32 readCnt =0;
  L7_char8 buf[26];
  L7_char8 *tmpBuf = L7_NULLPTR;
  L7_char8 *tmpBuf2 = L7_NULLPTR;
  L7_int32 res=0;

  if (osapiFsOpen(LINUX_MEM_INFO_FILE_NAME, &filedesc) != L7_ERROR)
  {
    while (readCnt < 2)
    {
      res=osapiFileRead(filedesc,buf,sizeof(buf));
      if (res == L7_ERROR)
      {
        osapiFsClose(filedesc);
        return L7_FAILURE;
      }
      tmpBuf = strtok (buf, "\r\n");
      if(readCnt == 1)
      {
        tmpBuf2 =  strtok (tmpBuf, " ");
        tmpBuf2 =  strtok (NULL, " ");
        if (convertTo32BitUnsignedInteger(tmpBuf2, (L7_uint32 *)freeMem) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
                  "Kernel Free Memory Check failure occured.%s %d\n",__FUNCTION__,__LINE__);
          osapiFsClose(filedesc);
          return L7_FAILURE;
        }
      }
      readCnt++;
    }
    osapiFsClose(filedesc);
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
#endif


/**************************************************************************
 *  NAME: transfer_serial
 *        This function transfers a file using serial
 *
 *  RETURNS:
 *
 *************************************************************************/
static L7_RC_t transfer_serial(L7_int32 type, L7_char8 *filename,
                               L7_int32 direction)
{
  /* We're only called on one task's thread; use static buffers to save
     stack space */
   static char buf[0x80];
   int orig_console_fd;
   L7_RC_t rc = L7_FAILURE;

#ifdef L7_CODE_UPDATE_TO_FLASH
   /* If performing a y/z modem download directly to flash, remove the   */
   /* destination file name to ensure enough storage, create a temporary */
   /* directory so the file can easily be renamed later.                 */
   buf[0] = '\0';
   if ((direction == L7_TRANSFER_DIRECTION_DOWN) &&
       ((type == L7_TRANSFER_YMODEM) || (type == L7_TRANSFER_ZMODEM))) {

      snprintf(buf, sizeof(buf), "rm -f %s; mkdir download >/dev/null 2>&1",
               filename);

      if (WEXITSTATUS(system(buf)) != 0) rc = L7_FAILURE;

      snprintf(buf, sizeof(buf), "cd download ; ");
   }
#else
   snprintf(buf, sizeof(buf), "cd %s ; ", DOWNLOAD_PATH);
#endif

   switch (direction) {

      case L7_TRANSFER_DIRECTION_DOWN:
         strncat(buf, "r", (sizeof(buf) - strlen(buf)));
         break;

      case L7_TRANSFER_DIRECTION_UP:
         strncat(buf, "s", (sizeof(buf) - strlen(buf)));
         break;

      default:
         return (L7_FAILURE);
         /* no need to break;, because of return */

   }

   switch (type) {

      case L7_TRANSFER_XMODEM:
         strncat(buf, "x", (sizeof(buf) - strlen(buf)));
         break;

      case L7_TRANSFER_YMODEM:
         strncat(buf, "b", (sizeof(buf) - strlen(buf)));
         break;

      case L7_TRANSFER_ZMODEM:
         strncat(buf, "z", (sizeof(buf) - strlen(buf)));
         break;

      default:
         return (L7_FAILURE);
         /* no need to break;, because of return */

   }

   if ((direction == L7_TRANSFER_DIRECTION_UP)
    || (type == L7_TRANSFER_XMODEM)) {

      strncat(buf, " ", (sizeof(buf) - strlen(buf)));
      strncat(buf, filename, (sizeof(buf) - strlen(buf)));

   }

   /* No looking for console breakins during transfer */
   orig_console_fd = bspapiConsoleFdGet();
   bspapiConsoleFdSet(0);

   if (WEXITSTATUS(system(buf)) == 0)
   {

     rc = L7_SUCCESS;
     if ((direction == L7_TRANSFER_DIRECTION_DOWN) &&
         ((type == L7_TRANSFER_YMODEM) || (type == L7_TRANSFER_ZMODEM))) {

#ifdef L7_CODE_UPDATE_TO_FLASH
        snprintf(buf, sizeof(buf), "cd download ; mv * %s ; cd - ; rm -rf download >/dev/null 2>&1",
                 filename);
#else
        snprintf(buf, sizeof(buf), "cd %s ; mv * %s >/dev/null 2>&1",
                 DOWNLOAD_PATH, filename);
#endif

        if (WEXITSTATUS(system(buf)) != 0) rc = L7_FAILURE;

     }
   }

   bspapiConsoleFdSet(orig_console_fd);
   return (rc);
}

/**************************************************************************
*
* @purpose  Creates an ASCII readable event log
*
* @param    event_log_file_name   @b{(input)} name of file to create
*
* @returns  0 if successful
* @returns  1 if file can not be opened
*
* @end
*
*************************************************************************/
L7_int32 create_ascii_event_log(L7_char8 *event_log_file_name)
{
  FILE *logfd;
  /* This function is called on only one thread. Make the buffers static to
     conserve stack space. */
  static char string[L7_CLI_MAX_STRING_LENGTH*2];
  static L7_char8 buf[1024];
  L7_uint32 index, stringLength, NumberOfBytesWritten;
  L7_int32 pad;
  L7_char8 pad_char = ' ';
  char *cr_location;

  if ((logfd = fopen(event_log_file_name, "w")) == NULL )
  {
     return (1);
  }

  NumberOfBytesWritten = 0;
  memset(string, 0, L7_CLI_MAX_STRING_LENGTH*2);

  index = L7_NULL;
  index = L7_event_log_get_next(L7_NULLPTR, index, string);

  if (index == L7_NULL)
  {
      NumberOfBytesWritten += fprintf(logfd,"\r\nMessage Log Empty");
  }
  else
  {
      NumberOfBytesWritten += fprintf(logfd, "                                             Time\r\n");
      NumberOfBytesWritten += fprintf(logfd, "         File   Line  TaskID    Code      d  h  m  s\r\n");

      while (index != L7_NULL)
      {
          /* Make sure string ends in \r\n */
          stringLength = strlen(string);
          if (stringLength < ((L7_CLI_MAX_STRING_LENGTH*2)-1)) {
              cr_location = strrchr(string, '\r');
              if (NULL == cr_location) {
                  strcat(string, "\r\n");
              } else {
                  *(cr_location+1) = '\n';
                  *(cr_location+2) = '\0';
              }
          } else {
              /* Truncate to fit in the \r\n */
              string[(L7_CLI_MAX_STRING_LENGTH*2)-3] = '\r';
              string[(L7_CLI_MAX_STRING_LENGTH*2)-2] = '\n';
              string[(L7_CLI_MAX_STRING_LENGTH*2)-1] = '\0';
          }
          NumberOfBytesWritten += fprintf(logfd, "%s", string);
          index = L7_event_log_get_next(L7_NULLPTR, index, string);
      }
  }

  /*
  pad is necessary for modem protocols, especially xmodem
  */

  pad = 1024 - (NumberOfBytesWritten % 1024);
  if (pad != 1024)
  {
      memset(buf, pad_char, sizeof(buf));
      fwrite(buf, sizeof(pad_char), (size_t)pad, logfd);
  }
  fclose(logfd);
  return 0;
};

/**************************************************************************
*
* @purpose  Transfer upload a file
*
* @param    void
*
* @end
*
*************************************************************************/
static void transfer_upload(void)
{
   /* We're only called on one task's thread; use static buffers to save
      stack space */
   static char buf[0x80];
   struct stat stat_buf;
   int rc;
   static char full_fname[0x80];
   L7_char8 stkFileName[DIM_MAX_FILENAME_SIZE+1];

#ifdef L7_MGMT_SECURITY_PACKAGE
   L7_uchar8 inet_address_display[IPV6_DISP_ADDR_LEN];
#endif /* L7_MGMT_SECURITY_PACKAGE */

   /* prepare files... */
   switch (transfer_ftype) {

      case L7_FILE_TYPE_CODE:
         memset(stkFileName,0x0,sizeof(stkFileName));
         bspapiImageFileNameResolve(transfer_fname_local,stkFileName);
         snprintf(full_fname, sizeof(full_fname), "%s%s",
                  CONFIG_PATH, stkFileName);
         break;
#ifdef L7_DHCP_SNOOPING_PACKAGE
    case L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS:
         rc = stat(CONFIG_PATH DHCP_SNOOPING_UPLOAD_FILE_NAME, &stat_buf);
         if ((rc != 0) || (stat_buf.st_size == L7_NULL))
         {
           /* indicate preparation file not found failure */
           set_result_string("Error while preparing file for transfer: File not found!");
           set_upload_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
           set_result(L7_TRANSFER_FAILED);
           return;
         }

         snprintf(full_fname, sizeof(full_fname),
                  CONFIG_PATH DHCP_SNOOPING_UPLOAD_FILE_NAME);
         break;
#endif



      case L7_FILE_TYPE_CONFIG:
         rc = stat(CONFIG_PATH SYSAPI_BIG_CONFIG_FILENAME, &stat_buf);
         if ((rc != 0) || (stat_buf.st_size == L7_NULL))
         {
           /* indicate preparation file not found failure */
           set_result_string("Error while preparing file for transfer: File not found!");
           set_upload_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
           set_result(L7_TRANSFER_FAILED);
           return;
         }

         snprintf(full_fname, sizeof(full_fname),
                  CONFIG_PATH SYSAPI_BIG_CONFIG_FILENAME);
         break;

      case L7_FILE_TYPE_ERRORLOG:

          /* generate ascii error log file */

        snprintf(buf, sizeof(buf), "%s/%s",
                 DOWNLOAD_PATH, ASCII_LOG_FILE_NAME);

        if (create_ascii_event_log(buf) != 0)
        {
              /* indicate preparation failure */

              set_result_string("Error while preparing file for transfer!");
              set_upload_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
              set_result(L7_TRANSFER_FAILED);
              return;
        }

       snprintf(full_fname, sizeof(full_fname),
                DOWNLOAD_PATH ASCII_LOG_FILE_NAME);
       break;
#ifdef L7_TOOL_VALGRIND
   case L7_FILE_TYPE_VALGLOG:

       /* generate ascii error log file */
     snprintf(buf, sizeof(buf), "%s/%s",
              DOWNLOAD_PATH, VALG_LOG_FILE_NAME);

     if (valgrindLogCopy(buf) != 0)
     {
           /* indicate preparation failure */

           set_result_string("Error while preparing file for transfer!");
           set_upload_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
           set_result(L7_TRANSFER_FAILED);
           return;
     }

    snprintf(full_fname, sizeof(full_fname),
             DOWNLOAD_PATH VALG_LOG_FILE_NAME);
    break;
#endif
   case L7_FILE_TYPE_SYSTEM_MSGLOG:

         /* generate msg log file */
         snprintf(buf, sizeof(buf), "%s/%s",
                  DOWNLOAD_PATH, MSG_LOG_FILE_NAME);

         if (logWriteInMemoryLogToFile(buf) != L7_SUCCESS)
         {
            /* indicate preparation failure */
            set_result_string("Error while preparing file for transfer!");
            set_upload_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
            set_result(L7_TRANSFER_FAILED);
            return;
         }

         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH MSG_LOG_FILE_NAME);

         break;

      case L7_FILE_TYPE_TRAP_LOG:

         /* generate trap log file */
         snprintf(buf, sizeof(buf), "%s/%s",
                  DOWNLOAD_PATH, TRAP_LOG_FILE_NAME);

         if (trapMgrCreateTrapLogFile(buf) != L7_SUCCESS)
         {
            /* indicate preparation failure */
            set_result_string("Error while preparing file for transfer!");
            set_upload_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
            set_result(L7_TRANSFER_FAILED);
            return;
         }

         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH TRAP_LOG_FILE_NAME);

         break;

#ifdef L7_CLI_PACKAGE
   case L7_FILE_TYPE_TXTCFG:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH TEMP_CONFIG_SCRIPT_FILE_NAME);
         if(L7_SUCCESS != cliTxtCfgCopyConfig (SYSAPI_TXTCFG_FILENAME, full_fname, L7_FALSE))
         {
           /* indicate preparation copy failure */
            set_result_string("Error while preparing file for transfer: File not found!");
            set_upload_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
            set_result(L7_TRANSFER_FAILED);
            return;
         }

         break;

      case L7_FILE_TYPE_CONFIG_SCRIPT:
          snprintf(buf, sizeof(buf), "%s/%s",
                   DOWNLOAD_PATH, TEMP_CONFIG_SCRIPT_FILE_NAME);

          if (cliCfgScriptCopy(buf) != L7_SUCCESS) {
             /* indicate preparation failure */
             set_upload_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
             set_result(L7_TRANSFER_FAILED);
             return;
          }

          snprintf(full_fname, sizeof(full_fname),
                   DOWNLOAD_PATH TEMP_CONFIG_SCRIPT_FILE_NAME);

          break;

      case L7_FILE_TYPE_CLI_BANNER:

          snprintf(full_fname, sizeof(full_fname),
                   CONFIG_PATH CLI_BANNER_FILE_NAME);

          break;
#endif /* L7_CLI_PACKAGE */

      default:

         /* indicate unknown transfer filetype */
         set_result_string("File type unknown!");
         set_upload_result_code(L7_TRANSFER_CODE_WRONG_FILE_TYPE);
         set_result(L7_TRANSFER_FAILED);
         return;

         /* no need to break;, because of return */

   }

   /* transfer files... */
#ifdef L7_MGMT_SECURITY_PACKAGE
   if (transfer_type == L7_TRANSFER_SFTP || transfer_type == L7_TRANSFER_SCP)
   {
     osapiInetNtop(transfer_ip_addr.family, (L7_uchar8 *)(&(transfer_ip_addr.addr)), inet_address_display, sizeof(inet_address_display));

     if (transfer_type == L7_TRANSFER_SFTP)
     {
       rc = sshcTransferSftp(inet_address_display, transfer_ip_addr.family,
                       0, transfer_fname_remote, full_fname, L7_TRANSFER_DIRECTION_UP);
     }
     else /* SCP */
     {

       rc = sshcTransferScp(inet_address_display, transfer_ip_addr.family,
                     0, transfer_fname_remote, full_fname, L7_TRANSFER_DIRECTION_UP);
     }
     if (rc != L7_SUCCESS)
     {
       /* indicate transfer failure */
       set_result_string("File transfer failed!");
       set_upload_result_code(L7_TRANSFER_CODE_FAILURE);
       set_result(L7_TRANSFER_FAILED);
       return;
     }
   }
   else
#endif /* L7_MGMT_SECURITY_PACKAGE */
   if (transfer_type == L7_TRANSFER_TFTP) {

      trapMgrTFTPStartLogTrap(transfer_fname_remote);
      if (transfer_tftp(&transfer_ip_addr, transfer_fname_remote,
                        full_fname, L7_TRANSFER_DIRECTION_UP)
       != L7_SUCCESS) {

         /* indicate upload failure */
         set_result_string("File transfer failed!");
         set_upload_result_code(L7_TRANSFER_CODE_FAILURE);
         set_result(L7_TRANSFER_FAILED);
         trapMgrTFTPAbortLogTrap(L7_TRANSFER_CODE_FAILURE, transfer_fname_remote);
         return;

      }
      trapMgrTFTPEndLogTrap(L7_TRANSFER_SUCCESS, transfer_fname_remote);

   } else if (transfer_type < L7_TRANSFER_LAST) { /* X/Y/Zmodem */

      if (transfer_serial(transfer_type, full_fname,
                          L7_TRANSFER_DIRECTION_UP) != L7_SUCCESS) {

         /* indicate upload failure */
         set_result_string("File transfer failed!");
         set_upload_result_code(L7_TRANSFER_CODE_FAILURE);
         set_result(L7_TRANSFER_FAILED);
         return;

      }

   } else {

      /* indicate unknown transfer type */
      set_result_string("Unknown transfer type!");
      set_upload_result_code(L7_TRANSFER_CODE_WRONG_FILE_TYPE);
      set_result(L7_TRANSFER_FAILED);
      return;

   }

   set_result_string("File transfer operation completed "
                     "successfully.");
   set_upload_result_code(L7_TRANSFER_CODE_SUCCESS);
   set_result(L7_TRANSFER_SUCCESS);

   return;
}

/**************************************************************************
*
* @purpose  Process a transferred code file
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t transfer_process_code(char *fileName)
{
  /* We're only called on one task's thread; use static buffers to save
     stack space */
  static L7_char8 buf[0x80];
  L7_char8 stkFileName[DIM_MAX_FILENAME_SIZE+1];

  L7_int32 rc;

#ifdef L7_STACKING_PACKAGE
  usmDbCdaCodeLoadStatus_t status;
#endif

  snprintf(buf, sizeof(buf), "%s%s",
      DOWNLOAD_PATH, fileName);
  bspapiImageFileNameResolve(fileName, stkFileName);

  rc = bspapiValidateImage(buf,L7_TRUE);

  if (rc != STK_SUCCESS)
  {
    set_download_code_status(rc);
    set_download_result_code(L7_TRANSFER_CODE_FAILURE);
    set_result(L7_OTHER_FILE_TYPE_FAILED);
    return (L7_FAILURE);
  }

  /* add this image to dual image manager */
  if (bspapiImageAdd(buf, stkFileName) != L7_SUCCESS)
  {
    set_result_string("Failure updating code!");
    set_download_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
    return (L7_FAILURE);
  }

#ifdef L7_STACKING_PACKAGE

   /* do STK post processing here */

   rc = cdaUnpackSTK(stkFileName);
   if (rc != L7_SUCCESS)
   {
     set_result_string("File transfer failed. Possible CRC error in file.");
     set_download_result_code(L7_TRANSFER_CODE_FAILURE);
     set_result(L7_TRANSFER_FAILED);

     return (L7_FAILURE);
   }

   set_result_string("Attempting to send the STK file to other units in the stack...");

   /* Update all other units with the new code.
   */
   rc = usmdbCdaCodeUpdate(CDA_ALL_UNITS, stkFileName, transfer_fname_local);

   if (rc == L7_SUCCESS)
   {
     do
     {
       osapiSleep(3);
       rc = usmdbCdaUpdateStatusGet(START_DOWNLOAD, &status);
     } while ((status == L7_USMDB_CDA_IN_PROGRESS ) && (rc == L7_SUCCESS));

     if ((rc == L7_SUCCESS) && (status == L7_USMDB_CDA_FINISHED_WITH_SUCCESS))
     {
       set_result_string("STK file transfer operation successful. All units updated code.");
     }
     else
     {
       set_result_string("STK file transfer operation successful. Code update failed on some units.");
     }
   }
   else
   {
     set_result_string("STK file transfer operation successful. No other units updated code.");
   }

#endif

   set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
   set_result(L7_TRANSFER_SUCCESS);

   return (L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Process a transferred Config file
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
static L7_RC_t transfer_process_config(void)
{
  /* We're only called on one task's thread; use static buffers to save
     stack space */
   static char buf[0x200];
   char *filename = SYSAPI_CONFIG_FILENAME;

   set_result_string("Applying configuration...");
   set_download_result_code(L7_TRANSFER_CODE_UPDATE_CONFIG);

   /* deploy configuration */
   snprintf(buf, sizeof(buf), "%s/%s", DOWNLOAD_PATH, filename);
   if (sysapiCfgFileDeploy(buf) != L7_SUCCESS)
   {
      set_result_string("Unable to apply configuration!");
      set_download_result_code(L7_TRANSFER_CODE_INVALID_CONFIG);
      set_result(L7_OTHER_FILE_TYPE_FAILED);
      return (L7_FAILURE);
   }

   set_result_string("Writing configuration...");
   set_download_result_code(L7_TRANSFER_CODE_UPDATE_CONFIG);

   /* write configuration */
   snprintf(buf, sizeof(buf), "cp -f %s/%s %s/%s >/dev/null 2>&1",
            DOWNLOAD_PATH, filename, CONFIG_PATH, filename);

   if (WEXITSTATUS(system(buf)) != 0)
   {
      /* indicate write failure */
      set_result_string("Unable to store configuration file!");
      set_download_result_code(L7_TRANSFER_CODE_INVALID_CONFIG);
      set_result(L7_OTHER_FILE_TYPE_FAILED);
      return (L7_FAILURE);
   }

   return (L7_SUCCESS);
}

/**************************************************************************
*
* @purpose  Process a transferred Kernel file
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
static L7_RC_t transfer_process_kernel(void)
{

  L7_int32 rc;

  rc = osapiKernelUpdate();

#ifdef L7_STACKING_PACKAGE
  if (rc == L7_SUCCESS)
  {
    /* We're only called on one task's thread; use static buffers to save
    stack space */
    static char buf[0x200];
    usmDbCdaCodeLoadStatus_t status;
    L7_int32 fileSize;

    set_result_string("Attempting to send the kernel to other units in the stack...");

    /* Update all other units with the new kernel. */
    snprintf(buf, sizeof(buf), "%s%s", DOWNLOAD_PATH, SYSAPI_KERNEL_FILENAME);
    if (osapiFsFileSizeGet(buf, &fileSize) != L7_SUCCESS)
    {
      return (L7_FAILURE);
    }

    rc = usmdbCdaKernelUpdate(CDA_ALL_UNITS, buf, fileSize);
    if (rc == L7_SUCCESS)
    {
      do
      {
        osapiSleep(3);
        rc = usmdbCdaUpdateStatusGet(UPDATE_KERNEL, &status);
      } while ((status == L7_USMDB_CDA_IN_PROGRESS ) && (rc == L7_SUCCESS));

      if ((rc == L7_SUCCESS) && (status == L7_USMDB_CDA_FINISHED_WITH_SUCCESS))
      {
        set_result_string("Kernel transfer operation successful. All units updated code.");
      }
      else
      {
        set_result_string("Kernel transfer operation successful. Kernel update failed on some units.");
      }
    }
    else
    {
      set_result_string("Kernel transfer operation successful. No other units updated kernel.");
    }
  }
#endif

  return (rc);
}
/**************************************************************************
*
* @purpose  Process a transferred PEM/KEY file
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
static L7_RC_t transfer_process_keys_and_certs(void)
{
  /* We're only called on one task's thread; use static buffers to save
     stack space */
   static char buf[0x80];

   set_download_result_code(L7_TRANSFER_CODE_UPDATE_CONFIG);

   /* copy file */
   switch (transfer_ftype)
   {
   case L7_FILE_TYPE_SSHKEY_RSA1:
     set_result_string("Updating SSH RSA1 host key...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_SSHD_NAME_SERVER_PRIVKEY, CONFIG_PATH);
     break;

   case L7_FILE_TYPE_SSHKEY_RSA2:
     set_result_string("Updating SSH RSA2 host key...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_SSHD_NAME_SERVER_PRIVKEY_RSA, CONFIG_PATH);
     break;

   case L7_FILE_TYPE_SSHKEY_DSA:
     set_result_string("Updating SSH DSA host key...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_SSHD_NAME_SERVER_PRIVKEY_DSA, CONFIG_PATH);
     break;

   case L7_FILE_TYPE_SSLPEM_ROOT:
     set_result_string("Updating SSL root certificate...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_SSLT_ROOT_PEM, CONFIG_PATH);
     break;

   case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
     set_result_string("Updating TR-069 ACS root certificate...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_TR069_ACS_SSLT_ROOT_PEM, CONFIG_PATH);
     break;
   case L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
     set_result_string("Updating TR-069 Client Private Key...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_TR069_CLIENT_SSL_PRIV_KEY, CONFIG_PATH);
     break;
   case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT:
     set_result_string("Updating TR-069 Client certificate...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_TR069_CLIENT_SSL_CERT, CONFIG_PATH);
     break;

   case L7_FILE_TYPE_SSLPEM_SERVER:
     set_result_string("Updating SSL server certificate...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_SSLT_SERVER_PEM, CONFIG_PATH);
     break;

   case L7_FILE_TYPE_SSLPEM_DHWEAK:
     set_result_string("Updating SSL Diffie-Hellman file...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_SSLT_DHWEAK_PEM, CONFIG_PATH);
     break;

   case L7_FILE_TYPE_SSLPEM_DHSTRONG:
     set_result_string("Updating SSL Diffie-Hellman file...");
     snprintf(buf, sizeof(buf), "cp %s/%s %s >/dev/null 2>&1",
              DOWNLOAD_PATH, L7_SSLT_DHSTRONG_PEM, CONFIG_PATH);
     break;

   default:
     set_result_string("Invalid file type");
     set_download_result_code(L7_TRANSFER_CODE_FAILURE);
     set_result(L7_TRANSFER_FAILED);
     return (L7_FAILURE);
     /* no need to break;, because of return */
   }

   if (WEXITSTATUS(system(buf)) != 0)
   {
      /* indicate copy failure... */
      set_result_string("Failure copying file");
      set_download_result_code(L7_TRANSFER_CODE_FAILURE);
      set_result(L7_TRANSFER_FAILED);
      return (L7_FAILURE);
   }

   return (L7_SUCCESS);
}

/**************************************************************************
*
* @purpose Download transfer a file
*
* @param    void
*
* @end
*
*************************************************************************/
static void transfer_download(void)
{
   /* We're only called on one task's thread; use static buffers to save
      stack space */
   static char buf[0x200];
   L7_RC_t rc = L7_SUCCESS;

//#ifdef L7_CLI_PACKAGE
   L7_uint32 file_size=0;
//#endif /* L7_CLI_PACKAGE */

   static char full_fname[0x80];
   L7_char8 tmpFileName[(L7_MAX_FILENAME > DIM_MAX_FILENAME_SIZE ? L7_MAX_FILENAME : DIM_MAX_FILENAME_SIZE) + 1];
#ifdef L7_MGMT_SECURITY_PACKAGE
   L7_uchar8 inet_address_display[IPV6_DISP_ADDR_LEN];
#endif /* L7_MGMT_SECURITY_PACKAGE */

#if defined(FEAT_METRO_CPE_V1_0)
  L7_uint32 kernMemFreeSize =0;
#define KERNEL_FREE_MEMORY 5000 /* KB */
#endif

   /* determine local filename... */
   switch (transfer_ftype) {

      case L7_FILE_TYPE_CODE:

      if (L7_TRANSFER_LOCAL == transfer_ftype)
        break;

      OSAPI_STRNCPY_SAFE(tmpFileName,transfer_fname_local);


#ifdef L7_CODE_UPDATE_TO_FLASH
         snprintf(full_fname, sizeof(full_fname), "%s%s",
                  CONFIG_PATH, tmpFileName);
#else
         snprintf(full_fname, sizeof(full_fname), "%s%s",
                  DOWNLOAD_PATH, tmpFileName);
#endif
         break;
#ifdef L7_DHCP_SNOOPING_PACKAGE
    case L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS:
         snprintf(full_fname, sizeof(full_fname), "%s%s",
                  CONFIG_PATH, DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
    break;
#endif


      case L7_FILE_TYPE_CONFIG:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH SYSAPI_CONFIG_FILENAME);
         break;

      case L7_FILE_TYPE_SSHKEY_RSA1:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_SSHD_NAME_SERVER_PRIVKEY);
         break;

      case L7_FILE_TYPE_SSHKEY_RSA2:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_SSHD_NAME_SERVER_PRIVKEY_RSA);
         break;

      case L7_FILE_TYPE_SSHKEY_DSA:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_SSHD_NAME_SERVER_PRIVKEY_DSA);
         break;

      case L7_FILE_TYPE_SSLPEM_ROOT:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_SSLT_ROOT_PEM);
         break;

      case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_TR069_ACS_SSLT_ROOT_PEM);
         break;

      case L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_TR069_CLIENT_SSL_PRIV_KEY);
         break;

      case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_TR069_CLIENT_SSL_CERT);
         break;

      case L7_FILE_TYPE_SSLPEM_SERVER:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_SSLT_SERVER_PEM);
         break;

      case L7_FILE_TYPE_SSLPEM_DHWEAK:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_SSLT_DHWEAK_PEM);
         break;

      case L7_FILE_TYPE_SSLPEM_DHSTRONG:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH L7_SSLT_DHSTRONG_PEM);
         break;

      case L7_FILE_TYPE_CONFIG_SCRIPT:
      case L7_FILE_TYPE_TXTCFG:
      case L7_FILE_TYPE_IAS_USERS:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH TEMP_CONFIG_SCRIPT_FILE_NAME);
         break;

      case L7_FILE_TYPE_CLI_BANNER:
         osapiFsDeleteFile(DOWNLOAD_PATH"/"CLI_BANNER_FILE_NAME);
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH CLI_BANNER_FILE_NAME);
        break;

      case L7_FILE_TYPE_KERNEL:
         snprintf(full_fname, sizeof(full_fname),
                  DOWNLOAD_PATH SYSAPI_KERNEL_FILENAME);
         break;

#ifdef L7_AUTO_INSTALL_PACKAGE
     case L7_FILE_TYPE_AUTO_INSTALL_SCRIPT:
         snprintf(full_fname, sizeof(full_fname), "%s%s",
                  CONFIG_PATH, AUTO_INSTALL_TEMP_SCRIPT_FILENAME);
     break;
#endif

      default:
         /* indicate unknown transfer filetype */
         set_result_string("File type unknown!");
         set_download_result_code(L7_TRANSFER_CODE_WRONG_FILE_TYPE);
         set_result(L7_TRANSFER_FAILED);
         return;

         /* no need to break;, because of return */

   }

   /* transfer files... */
#ifdef L7_MGMT_SECURITY_PACKAGE
   if (transfer_type == L7_TRANSFER_SFTP || transfer_type == L7_TRANSFER_SCP)
   {
     osapiInetNtop(transfer_ip_addr.family, (L7_uchar8 *)(&(transfer_ip_addr.addr)), inet_address_display, sizeof(inet_address_display));
   }
   if (transfer_type == L7_TRANSFER_SFTP)
   {
     rc = sshcTransferSftp(inet_address_display, transfer_ip_addr.family,
                     0, transfer_fname_remote, full_fname, L7_TRANSFER_DIRECTION_DOWN);
   }
   else if (transfer_type == L7_TRANSFER_SCP)
   {
     rc = sshcTransferScp(inet_address_display, transfer_ip_addr.family,
                   0, transfer_fname_remote, full_fname, L7_TRANSFER_DIRECTION_DOWN);
   }
   else
#endif /* L7_MGMT_SECURITY_PACKAGE */
   if (transfer_type == L7_TRANSFER_TFTP) {
#if defined(FEAT_METRO_CPE_V1_0)
      rc = transfer_download_memcheck(&kernMemFreeSize);
      if(rc == L7_SUCCESS && kernMemFreeSize > KERNEL_FREE_MEMORY)
#endif
      {
        rc = transfer_tftp(&transfer_ip_addr, transfer_fname_remote,
                           full_fname, L7_TRANSFER_DIRECTION_DOWN);
      }

   } else if (transfer_type < L7_TRANSFER_LAST) { /* X/Y/Zmodem */

      rc = transfer_serial(transfer_type, full_fname,
                           L7_TRANSFER_DIRECTION_DOWN);

   } else if (L7_TRANSFER_LOCAL == transfer_type) {
      rc = osapiFsCopyFile(transfer_fname_local, transfer_fname_remote);
      if (rc == L7_SUCCESS)
       {
         set_result(L7_TRANSFER_SUCCESS);
         set_result_string("Copy operation successful!");
         set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
       }
       else
       {
         set_result(L7_OTHER_FILE_TYPE_FAILED);
         set_download_result_code(L7_TRANSFER_CODE_FAILURE);
         set_result_string("Copy operation failed!");
       }

   } else {

      /* indicate unknown transfer type */
      set_result_string("Unknown transfer type!");
      set_download_result_code(L7_TRANSFER_CODE_WRONG_FILE_TYPE);
      set_result(L7_TRANSFER_FAILED);
      return;

   }

   if (rc == L7_SUCCESS) {

      /* process files... */
      switch (transfer_ftype) {

         case L7_FILE_TYPE_CODE:
            if (transfer_process_code(tmpFileName) == L7_SUCCESS) {
               set_result_string("File transfer operation completed successfully.");
               set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
               set_result(L7_TRANSFER_SUCCESS);
            }
            break;
#ifdef L7_DHCP_SNOOPING_PACKAGE
    case L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS:
               set_result_string("File transfer operation completed successfully.");
               set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
               set_result(L7_TRANSFER_SUCCESS);
    break;
#endif

#ifdef L7_AUTO_INSTALL_PACKAGE
    case L7_FILE_TYPE_AUTO_INSTALL_SCRIPT:
               set_result_string("File transfer operation completed successfully.");
               set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
               set_result(L7_TRANSFER_SUCCESS);
    break;
#endif

         case L7_FILE_TYPE_CONFIG:
            if (transfer_process_config() == L7_SUCCESS) {
               set_result_string("File transfer complete...storing configuration...");
               set_download_result_code(L7_TRANSFER_CODE_WRITING_FLASH);
               set_result(L7_TRANSFER_SUCCESS);

               /* sync here to write new config ... */

               sync();

               set_result_string("Configuration update completed successfully.");
               set_download_result_code(L7_TRANSFER_CODE_SUCCESS);

               /*
               No further action is needed here, as the config
               will now be applied as a result of
               transfer_process_config()
               */
            }
            break;

         case L7_FILE_TYPE_SSHKEY_RSA1:
         case L7_FILE_TYPE_SSHKEY_RSA2:
         case L7_FILE_TYPE_SSHKEY_DSA:
             if (transfer_process_keys_and_certs() == L7_SUCCESS) {
#ifdef L7_MGMT_SECURITY_PACKAGE
               if (sshdKeyValidate(transfer_ftype) != L7_SUCCESS)
               {
                   sshdKeyDelete(transfer_ftype);
                   set_result_string("Key file not valid!");
                   set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                   set_result(L7_OTHER_FILE_TYPE_FAILED);
               }
               else
               {
#endif /* L7_MGMT_SECURITY_PACKAGE */
                   set_result_string("Host key file transfer operation completed successfully.");
                   set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
                   set_result(L7_TRANSFER_SUCCESS);
#ifdef L7_MGMT_SECURITY_PACKAGE
               }
#endif /* L7_MGMT_SECURITY_PACKAGE */
             }
           break;

         case L7_FILE_TYPE_SSLPEM_ROOT:
         case L7_FILE_TYPE_SSLPEM_SERVER:
         case L7_FILE_TYPE_SSLPEM_DHWEAK:
         case L7_FILE_TYPE_SSLPEM_DHSTRONG:
         case   L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
         case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT:
         case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
           if (transfer_process_keys_and_certs() == L7_SUCCESS)
           {
#ifdef L7_TR069_PACKAGE
              switch(transfer_ftype)
              {
                 case L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
                 case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT:
                 case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
                      usmdbTr069SSLCertsLoad();
                      break;
                 default:
                      break;
             }
#endif
             set_result_string("Certificate file transfer operation completed successfully.");
             set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
             set_result(L7_TRANSFER_SUCCESS);
           }
           break;

      case L7_FILE_TYPE_CONFIG_SCRIPT:
            snprintf(buf, sizeof(buf), "cp %s/%s %s/%s >/dev/null 2>&1",
                   DOWNLOAD_PATH, TEMP_CONFIG_SCRIPT_FILE_NAME,
                   CONFIG_PATH, TEMP_CONFIG_SCRIPT_FILE_NAME);

            if (WEXITSTATUS(system(buf)) != 0)
            {
                set_result_string("File transfer error : Failed to prepare file for validation.");
                set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                set_result(L7_TRANSFER_FAILED);
                break;
            }

            set_result_string("File transfer successful...");
            set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
            set_result(L7_TRANSFER_SUCCESS);
            break;

      case L7_FILE_TYPE_IAS_USERS:
            snprintf(buf, sizeof(buf), "cp %s/%s %s/%s >/dev/null 2>&1",
                     DOWNLOAD_PATH, TEMP_CONFIG_SCRIPT_FILE_NAME,
                     CONFIG_PATH, TEMP_CONFIG_SCRIPT_FILE_NAME);

            if (WEXITSTATUS(system(buf)) != 0)
            {
                set_result_string("File transfer error : Failed to prepare IAS file for validation.");
                set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                set_result(L7_TRANSFER_FAILED);
                break;
            }

            snprintf(buf, sizeof(buf), "%s/%s", CONFIG_PATH, TEMP_CONFIG_SCRIPT_FILE_NAME);
            file_size = 0;
            if (osapiFsFileSizeGet(buf,&file_size)==L7_SUCCESS)
            {
              set_result_string("Validating and updating IAS users database file...");
              osapiSleep(3);
              rc = IASUsersScriptValidateAndApply(TEMP_CONFIG_SCRIPT_FILE_NAME, transfer_fname_local);
              if (rc == L7_SUCCESS)
              {
                set_result_string("Updated IAS users database successfully.");
                set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
                set_result(L7_TRANSFER_FAILED);
              }
              else
              {
                set_result_string("IAS users database update failed.");
                set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
                set_result(L7_TRANSFER_SUCCESS);
              }
            }
            else
            {
              set_result_string("File transfer failed!");
              set_download_result_code(L7_TRANSFER_CODE_FAILURE);
              set_result(L7_TRANSFER_FAILED);
            }
            osapiFsDeleteFile(buf);
            break;

#ifdef L7_CLI_PACKAGE
      case L7_FILE_TYPE_TXTCFG:
            snprintf(buf, sizeof(buf), "%s/%s", DOWNLOAD_PATH, TEMP_CONFIG_SCRIPT_FILE_NAME);
            file_size = 0;
            if (osapiFsFileSizeGet(buf,&file_size)==L7_SUCCESS)
            {
               if (file_size > SHOW_RUNNING_CONFIG_ALL_MAX_SIZE_LIMIT)
               {
                  set_result_string("File transfer error: Configuration file is too big.");
                  set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                  set_result(L7_TRANSFER_FAILED);
                  osapiFsDeleteFile(buf);
               }
               else
               {
                 /* NOTE: Downloading a script to the startup-config slot saves it to the
                  *       file system, but it is not applied until the platform is rebooted.
                  *       This prevents active users from being logged out of management interfaces.
                  *
                  *       Hence, cliTxtCfgSaveTransConfig() is replaced with cliTxtCfgCopyConfig() below.
                  */
                 (void)cliTxtCfgCopyConfig(buf, L7_NULLPTR, L7_TRUE);
                 set_result_string("File transfer successful...");
                 set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
                 set_result(L7_TRANSFER_SUCCESS);
               }

            }
            else
            {
              set_result_string("File transfer error : Failed to download the configuration file.");
              set_download_result_code(L7_TRANSFER_CODE_FAILURE);
              set_result(L7_TRANSFER_FAILED);
              osapiFsDeleteFile(buf);
            }
            break;

      case L7_FILE_TYPE_CLI_BANNER:

            /* Check the size of the file */
            if (osapiFsFileSizeGet(DOWNLOAD_PATH"/"CLI_BANNER_FILE_NAME,&file_size)==L7_SUCCESS)
            {
              /* TBD:Align the hardcode value with the banner macros */
              if (file_size > BANNER_MAX_FILE_SIZE)
              {
                set_result_string("File transfer error: CLI Banner file is too big to display.");
                set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                set_result(L7_TRANSFER_FAILED);
              }
              else
              {
                if(cliWebSystemReadBannerFile(DOWNLOAD_PATH"/"CLI_BANNER_FILE_NAME) == L7_SUCCESS)
                {
                  snprintf(buf, sizeof(buf), "cp %s/%s %s/%s >/dev/null 2>&1",
                    DOWNLOAD_PATH, CLI_BANNER_FILE_NAME,
                    CONFIG_PATH, CLI_BANNER_FILE_NAME);

                  if (WEXITSTATUS(system(buf)) != 0)
                  {
                    cliWebSystemReadBannerFile(CLI_BANNER_FILE_NAME);
                    set_result_string("File transfer error : Failed to copy the CLI banner file.");
                    set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                    set_result(L7_TRANSFER_FAILED);
                  }
                  else
                  {
                    set_result_string("File transfer successful...");
                    set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
                    set_result(L7_TRANSFER_SUCCESS);
                  }
                }
                else
                {
                  cliWebSystemReadBannerFile(CLI_BANNER_FILE_NAME);
                  set_result_string("File transfer error : Failed to copy the CLI banner file.");
                  set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                  set_result(L7_TRANSFER_FAILED);
                }
              }/* End:file_size */
            } /* End:osapiFsFileSizeGet */
            else
            {
              set_result_string("File transfer error : Failed to copy the CLI banner file.");
              set_download_result_code(L7_TRANSFER_CODE_FAILURE);
              set_result(L7_TRANSFER_FAILED);
            }
            osapiFsDeleteFile(DOWNLOAD_PATH"/"CLI_BANNER_FILE_NAME);

            break;
#endif /* L7_CLI_PACKAGE */

         case L7_FILE_TYPE_KERNEL:
            if (transfer_process_kernel() == L7_SUCCESS) {
               set_result_string("Kernel transfer operation completed successfully.");
               set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
               set_result(L7_TRANSFER_SUCCESS);
            }
            else
            {
              set_result_string("File transfer error : Failed to update kernel.");
              set_download_result_code(L7_TRANSFER_CODE_FAILURE);
              set_result(L7_TRANSFER_FAILED);
            }
            break;

         default:

            /* indicate unknown transfer filetype */
            set_result_string("File type unknown!");
            set_download_result_code(L7_TRANSFER_CODE_WRONG_FILE_TYPE);
            set_result(L7_TRANSFER_FAILED);
            return;

            /* no need to break;, because of return */

      }

   } else {

      /* indicate download failure */
      set_result_string("File transfer failed!");
      set_download_result_code(L7_TRANSFER_CODE_FAILURE);
      set_result(L7_TRANSFER_FAILED);
      return;

   }

   return;
}

/***************************************************************************
** NAME: transfer_get_status
**
**  Get the status of the Transfer Task
**
***************************************************************************/
void transfer_get_status(L7_int32 *currentStatus, L7_char8 *buf)
{
   L7_RC_t rc;

   pthread_once(&transfer_once, transfer_init);

   /* lock transfer parameters... */
   if ((rc = osapiSemaTake(L7_transferStatSem, L7_WAIT_FOREVER))
    == L7_SUCCESS) {

      if (transfer_string_status[0] == L7_NULL) {

         *currentStatus = L7_NO_STATUS_CHANGE;

      } else {

         strcpy(buf, transfer_string_status);
         strcpy(transfer_temp_string_status, transfer_string_status);
         transfer_string_status[0] = L7_NULL;
         *currentStatus = L7_STATUS_CHANGED;

      }
         
      if (transfer_started == L7_FALSE) {

         *currentStatus = transfer_result;

      }
      else
      {            
        if(*currentStatus == L7_NO_STATUS_CHANGE)
        {
          strcpy(buf, transfer_temp_string_status);
        }
      }      

      if ((rc = osapiSemaGive(L7_transferStatSem)) != L7_SUCCESS) {

         /* couldn't give lock? */
         L7_LOG_ERROR(rc);

      }

   } else {

      /* lock doesn't exist? */
      L7_LOG_ERROR(rc);

   }
}

/*********************************************************************
*
* @purpose Gets the file upload transfer status
*
* @param currentStatus
*
* @returns none
*
* @notes  none
*
* @end
*********************************************************************/
void transfer_get_upload_status_code(L7_int32 *currentStatus)
{
   L7_RC_t rc;

   pthread_once(&transfer_once, transfer_init);

   /* lock transfer parameters... */
   if ((rc = osapiSemaTake(L7_transferStatSem, L7_WAIT_FOREVER))
    == L7_SUCCESS) {

      *currentStatus = transfer_code_upload_status;

      if ((rc = osapiSemaGive(L7_transferStatSem)) != L7_SUCCESS) {

         /* couldn't give lock? */
         L7_LOG_ERROR(rc);

      }

   } else {

      /* lock doesn't exist? */
      L7_LOG_ERROR(rc);

   }
}

/*********************************************************************
*
* @purpose Gets the file download transfer status
*
* @param currentStatus
*
* @returns none
*
* @notes  none
*
* @end
*********************************************************************/
void transfer_get_download_status_code(L7_int32 *currentStatus)
{
   L7_RC_t rc;

   pthread_once(&transfer_once, transfer_init);

   /* lock transfer parameters... */
   if ((rc = osapiSemaTake(L7_transferStatSem, L7_WAIT_FOREVER))
    == L7_SUCCESS) {

      *currentStatus = transfer_code_download_status;

      if ((rc = osapiSemaGive(L7_transferStatSem)) != L7_SUCCESS) {

         /* couldn't give lock? */
         L7_LOG_ERROR(rc);

      }

   } else {

      /* lock doesn't exist? */
      L7_LOG_ERROR(rc);

   }
}
/**************************************************************************
 *  NAME: set_code_validation_status
 *        This function takes the code given to it and uses XIO functions
 *        to set the STK file validation result code.
 *
 *  RETURNS:
 *
 *************************************************************************/
void set_download_code_status(L7_uint32 stkRc)
{
  L7_RC_t rc;
   
  pthread_once(&transfer_once, transfer_init);

  /* lock transfer parameters... */
  if ((rc = osapiSemaTake(L7_transferStatSem, L7_WAIT_FOREVER))
       == L7_SUCCESS) 
  {
    code_status = stkRc;
    if ((rc = osapiSemaGive(L7_transferStatSem)) != L7_SUCCESS) 
    {
      /* couldn't give lock? */
      L7_LOG_ERROR(rc);
    }
  } 
  else 
  {
    /* lock doesn't exist? */
    L7_LOG_ERROR(rc);
  }
}

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
void transfer_get_download_code_status(L7_int32 *currentStatus)
{
  L7_RC_t rc;

  pthread_once(&transfer_once, transfer_init);

  /* lock transfer parameters... */
  if ((rc = osapiSemaTake(L7_transferStatSem, L7_WAIT_FOREVER))
       == L7_SUCCESS)
  {
    *currentStatus = code_status;
    code_status = STK_SUCCESS;
    if ((rc = osapiSemaGive(L7_transferStatSem)) != L7_SUCCESS)
    {
      /* couldn't give lock? */
      L7_LOG_ERROR(rc);
    }
  }
  else
  {
    /* lock doesn't exist? */
    L7_LOG_ERROR(rc);
  }
}

/***************************************************************************
** NAME: L7_transfer_start
**
**  Gives semaphore to transfer Task
**
***************************************************************************/
L7_RC_t L7_transfer_start(L7_uint32 direction)
{
   L7_char8 buf[80];
   L7_RC_t rc;
   L7_uint32 family;

   memset(transfer_string_status, 0, sizeof(transfer_string_status));
   memset(transfer_temp_string_status, 0, sizeof(transfer_temp_string_status));

   pthread_once(&transfer_once, transfer_init);

   /* lock transfer parameters... */
   if ((rc = osapiSemaTake(L7_transferParmSem, L7_WAIT_FOREVER))
    == L7_SUCCESS) {

      if (!usmDbTransferInProgressGet(1)) {

          if (usmDbTransferInProgressSet(1,L7_TRUE)!= L7_SUCCESS)
          {
            if ((rc = osapiSemaGive(L7_transferParmSem)) != L7_SUCCESS)
            {
              /* couldn't give lock? */
              L7_LOG_ERROR(rc);

            }
            return L7_IMAGE_IN_USE;
          }

          transfer_direction = direction;

          if (direction == L7_TRANSFER_DIRECTION_DOWN) {

             usmDbTransferDownloadFileTypeGet(simGetThisUnit(),
                                              &transfer_ftype);

          } else {

             usmDbTransferUploadFileTypeGet(simGetThisUnit(),
                                            &transfer_ftype);

          }

          /* Mgmt Access is suspended only for transfers originating from CLI or HTTP
           * For Auto Install, Mgmt access should not be affected */
          if (transfer_ftype == L7_FILE_TYPE_AUTO_INSTALL_SCRIPT)
          {
              usmDbTransferSuspendMgmtAccessSet(1,L7_FALSE);
          }
          else
          {
              usmDbTransferSuspendMgmtAccessSet(1,L7_TRUE);
          }

          usmDbTransferModeGet(simGetThisUnit(), &transfer_type);

          usmDbTransferFilePathLocalGet(simGetThisUnit(), buf);
          strncpy(transfer_fname_local, buf, sizeof(buf));
          buf[(sizeof(buf) - 1)] = '\0';

          usmDbTransferFileNameLocalGet(simGetThisUnit(), buf);
          strncat(transfer_fname_local, buf, (sizeof(buf) - strlen(buf)));
          buf[(sizeof(buf) - 1)] = '\0';

          usmDbTransferFilePathRemoteGet(simGetThisUnit(), buf);
          strncpy(transfer_fname_remote, buf, sizeof(buf));
          buf[(sizeof(buf) - 1)] = '\0';

          usmDbTransferFileNameRemoteGet(simGetThisUnit(), buf);
          strncat(transfer_fname_remote, buf, (sizeof(buf) - strlen(buf)));
          buf[(sizeof(buf) - 1)] = '\0';

          usmDbTransferServerAddressTypeGet(simGetThisUnit(), &family);
          transfer_ip_addr.family = (L7_uchar8)family;
          usmDbTransferServerAddressGet(simGetThisUnit(), (L7_uchar8 *)(&(transfer_ip_addr.addr)));

          transfer_result = L7_NO_STATUS_CHANGE;
          transfer_started = L7_TRUE;
          
          if (direction == L7_TRANSFER_DIRECTION_DOWN)
          {
            /* set result info... */
            set_transfer_string(buf, sizeof(buf));
            set_download_result_code(L7_TRANSFER_CODE_STARTING); 
          }
          else
          {
            /* set result info... */
            set_transfer_string(buf, sizeof(buf));
            set_upload_result_code(L7_TRANSFER_CODE_STARTING); 
          }

          osapiSemaGive(L7_transferSignalSem);

      }

      if ((rc = osapiSemaGive(L7_transferParmSem)) != L7_SUCCESS) {

         /* couldn't give lock? */
         L7_LOG_ERROR(rc);

      }

   } else {

      /* lock doesn't exist? */
      L7_LOG_ERROR(rc);

   }
   return L7_SUCCESS;
}

/***************************************************************************
** NAME: L7_transfer_task
**
**  Loops forever checking to see if a file transfer needs to be performed
**
***************************************************************************/
L7_int32 L7_transfer_task()
{
   L7_RC_t rc;
 #ifdef L7_TOOL_VALGRIND
   extern L7_BOOL globValgrindRunning;
 #endif
   pthread_once(&transfer_once, transfer_init);

   for (;;) {

      if ((rc = osapiSemaTake(L7_transferSignalSem, L7_WAIT_FOREVER))
       == L7_SUCCESS) {
 #ifdef L7_TOOL_VALGRIND
        if (globValgrindRunning == L7_FALSE)
        {
 #endif
          if (transfer_type != L7_TRANSFER_HTTP)
          {
            /* Unmount tmpfs first. */
            if(0 > system("umount " DOWNLOAD_PATH " >/dev/null 2>&1")){}

             /* mount tmpfs directory for temporary storage... */
             /* I could write code for this, but easier to call external... */
             if (WEXITSTATUS(system("mount -t tmpfs tmpfs " DOWNLOAD_PATH
                                    " >/dev/null 2>&1")) != 0) {

               set_result_string("Make sure /tmp/ directory exists!");        /* PTin modified: paths */
               set_upload_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
               set_download_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
               set_result(L7_TRANSFER_FAILED);
               transfer_started = L7_FALSE;
                /* indicate preparation failure */
                continue;
           }
          }
#ifdef L7_TOOL_VALGRIND

        }
#endif
         /* lock transfer parameters... */
         if ((rc = osapiSemaTake(L7_transferParmSem, L7_WAIT_FOREVER))
          == L7_SUCCESS) {

            if (transfer_type == L7_TRANSFER_HTTP)
            {
              switch ( transfer_direction )
              {
                case L7_TRANSFER_DIRECTION_DOWN:

                  if(httpTransferFileRename(transfer_ftype, transfer_fname_remote, transfer_fname_local)!= L7_SUCCESS)
                  {
                    set_result(L7_TRANSFER_FAILED);
                    set_result_string("Copy operation failed!");
                    set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                    break;
                  }

                  if (transfer_process_code(transfer_fname_local) == L7_SUCCESS)
                  {
                    set_result_string("File transfer operation completed successfully.");
                    set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
                    set_result(L7_TRANSFER_SUCCESS);
                  }
                  else
                  {
                    set_result(L7_TRANSFER_FAILED);
                    set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                    break;
                  }
                  break;

                case L7_TRANSFER_DIRECTION_UP:
                default:
                  set_result_string("Unknown TFTP direction request!");
                  set_upload_result_code(L7_TRANSFER_CODE_UNKNOWN_DIRECTION);
                  set_download_result_code(L7_TRANSFER_CODE_UNKNOWN_DIRECTION);
                  set_result(L7_TRANSFER_FAILED);
                  transfer_started = L7_FALSE;
                  break;
              }
            } else {

              if (transfer_ftype == L7_TRANSFER_TFTP)
                trapMgrTFTPStartLogTrap(transfer_fname_remote);

              if (transfer_direction == L7_TRANSFER_DIRECTION_UP) {

                 transfer_upload();

              } else if (transfer_direction == L7_TRANSFER_DIRECTION_DOWN) {

                 transfer_download();

              } else {

                 /* indicate unknown direction */
                 set_result_string("Unknown transfer direction request!");
                 set_upload_result_code(L7_TRANSFER_CODE_UNKNOWN_DIRECTION);
                 set_download_result_code(L7_TRANSFER_CODE_UNKNOWN_DIRECTION);
                 set_result(L7_TRANSFER_FAILED);

              }

              if (transfer_ftype == L7_TRANSFER_TFTP)
              {
                if (transfer_code_upload_status == L7_TRANSFER_CODE_SUCCESS)
                {
                  trapMgrTFTPEndLogTrap(L7_TRANSFER_SUCCESS, transfer_fname_remote);
                }
                else
                {
                  trapMgrTFTPAbortLogTrap(L7_TRANSFER_CODE_FAILURE, transfer_fname_remote);
                }
              }
            }


            if ((rc = osapiSemaGive(L7_transferParmSem)) != L7_SUCCESS) {

               /* couldn't give lock? */
               L7_LOG_ERROR(rc);

            }

         } else {

            /* couldn't get lock? */
            L7_LOG_ERROR(rc);

         }

         /* flush fs buffers... */
         sync();

         if (transfer_type != L7_TRANSFER_HTTP)
         {
           /* umount tmpfs directory... */
           /* I could write code for this, but easier to call external... */
           if(0 > system("umount " DOWNLOAD_PATH " >/dev/null 2>&1")){}
         }

      } else {

         /* couldn't get signal? */
         L7_LOG_ERROR(rc);

      }

      /* Null Context means the Transfer is from an http client. */
      if (NULL == usmDbTransferContextGet(1)) {

         /* Only reset the status for http, cli will be reset in
          * cli_transfer code.
          */
         httpTransferFileCleanup();
         if ((usmDbTransferInProgressGet(simGetThisUnit()) == L7_TRUE) &&
             (transfer_type == L7_TRANSFER_HTTP))
         {
           sysapiPrintf("\nHTTP File Transfer is completed. Management interfaces are released. \n");
         }

         usmDbTransferInProgressSet(1,L7_FALSE);
         usmDbTransferSuspendMgmtAccessSet(1,L7_FALSE);

         /* Notify the originator of the transaction, if needed.*/
         usmDbTransferCompletionNotifyUsers(transfer_result);

      }
#ifdef L7_DHCP_SNOOPING_PACKAGE
    if ( (transfer_direction == L7_TRANSFER_DIRECTION_DOWN) &&
         (transfer_ftype == L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS)
       )
    {
      if  ( transfer_result == L7_TRANSFER_SUCCESS)
      {
        usmDbDsTftpDownloadStatus(L7_FALSE, L7_TRUE);
      }
      else
      {
        usmDbDsTftpDownloadStatus(L7_FALSE,L7_FALSE);
      }
    }
    else if ( (transfer_direction == L7_TRANSFER_DIRECTION_UP) &&
         (transfer_ftype == L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS)
       )
    {
      if  ( transfer_result == L7_TRANSFER_SUCCESS)
      {
        usmDbDsTftpDownloadStatus(L7_TRUE,L7_TRUE);
      }
      else
      {
        usmDbDsTftpDownloadStatus(L7_TRUE,L7_FALSE);
      }
    }

    if ( (transfer_direction == L7_TRANSFER_DIRECTION_DOWN) &&
         (transfer_ftype == L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS)
       )
    {
       osapiFsDeleteFile(DHCP_SNOOPING_UPLOAD_FILE_NAME);

    }
#endif


      transfer_started = L7_FALSE;

   }

   /* should not get here... */
   L7_LOG_ERROR(0xdeadbeef);

   return(L7_FAILURE);
}



#if L7_FEAT_BOOTCODE_UPDATE
/*********************************************************************
* @purpose  Updates the bootloader from the supplied Image file.
*
* @param    fileName Image file to fetch the bootloader from
*
* @returns  L7_SUCCESS on success
*           L7_FAILURE otherwise
*
* @comments Ideally this function would be in the bsp. However it
*           references too many local variables. In order to retain
*           consistancy with the VxWorks version, this function is
*           consciously left in this file despite its name.
*
* @end
*********************************************************************/
L7_uint32 bspapiBootCodeUpdate(char *fileName)
{

   char buf[0x80];

   /* check update script validity */
   snprintf(buf, sizeof(buf), "cd %s ; md5sum -c %s >/dev/null 2>&1",
            EXEC_PATH, UPDATE_BOOTROM_SCRIPT_NAME ".md5sum");

   if (WEXITSTATUS(system(buf)) != 0) {

      /* indicate CRC failure... */
      set_result_string("Failure extracting upgrade instructions! ");
      set_download_result_code(L7_TRANSFER_CODE_ERROR_STARTING);
      set_result(L7_TRANSFER_BAD_CRC);
      return (L7_FAILURE);

   }

   set_result_string("Updating bootrom...");
   set_download_result_code(L7_TRANSFER_CODE_WRITING_FLASH);

   /* execute update bootrom script */
   snprintf(buf, sizeof(buf), "cd %s ; sh %s %s -q", /* do NOT direct to null */
            EXEC_PATH, UPDATE_BOOTROM_SCRIPT_NAME, fileName);

   switch (WEXITSTATUS(system(buf))) {

      case 0:
         /* good! */
         break;

      case 1:
         /* indicate bad option */
         set_result_string("Invalid option passed to " UPDATE_SCRIPT_NAME "! ");
         set_download_result_code(L7_TRANSFER_CODE_FLASH_FAILED);
         set_result(L7_FLASH_FAILED);
         return (L7_FAILURE);

         /* no need to break;, because of return */

      case 2:
         /* indicate code file not found */
         set_result_string("Bootrom file not found! ");
         set_download_result_code(L7_TRANSFER_CODE_FLASH_FAILED);
         set_result(L7_FLASH_FAILED);
         return (L7_FAILURE);

         /* no need to break;, because of return */

      case 3:
         /* indicate checksum failure */
         set_result_string("MD5 checksum failure! ");
         set_download_result_code(L7_TRANSFER_CODE_CRC_FAILURE);
         set_result(L7_TRANSFER_BAD_CRC);
         return (L7_FAILURE);

         /* no need to break;, because of return */

      case 4:
         /* indicate erase error */
         set_result_string("Flash erase failed! ");
         set_download_result_code(L7_TRANSFER_CODE_FLASH_FAILED);
         set_result(L7_FLASH_FAILED);
         return (L7_FAILURE);

         /* no need to break;, because of return */

      case 5:
         /* indicate write error */
         set_result_string("File save failed! ");
         set_download_result_code(L7_TRANSFER_CODE_FLASH_FAILED);
         set_result(L7_FLASH_FAILED);
         return (L7_FAILURE);

         /* no need to break;, because of return */

      default:
         /* indicate unknown error */
         set_result_string("Unknown error occurred! ");
         set_download_result_code(L7_TRANSFER_CODE_FLASH_FAILED);
         set_result(L7_TRANSFER_FAILED);
         return (L7_FAILURE);

         /* no need to break;, because of return */

   }

  return L7_SUCCESS;
}
#endif

/**************************************************************************
*
* @purpose  Process a transferred Kernel file
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t osapiKernelUpdate(void)
{
  /* We're only called on one task's thread; use static buffers to save
     stack space */
   static char buf[0x200];

#if defined(FEAT_METRO_CPE_V1_0)
   snprintf(buf, sizeof(buf), "cd %s; tar -xf %s  >/dev/null 2>&1",
            DOWNLOAD_PATH, SYSAPI_KERNEL_FILENAME);
   if (WEXITSTATUS(system(buf)) != 0)
   {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
              "Unable to extract Kernel \n");
      /* indicate write failure */
      set_result_string("Unable to extract kernel!");
      set_download_result_code(L7_TRANSFER_CODE_INVALID_CONFIG);
      set_result(L7_OTHER_FILE_TYPE_FAILED);
      return (L7_FAILURE);
   }

   if(L7_SUCCESS != osapiSwapFile(USYSTEM_FILE))
   {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
              "Unable to Swap Kernel \n");
      /* indicate write failure */
      set_result_string("Unable to swap kernel!");
      set_download_result_code(L7_TRANSFER_CODE_INVALID_CONFIG);
      set_result(L7_OTHER_FILE_TYPE_FAILED);
      return (L7_FAILURE);
   }
#else
   /* extract the UPDATE_KERNEL script from the kernel tarball */
   snprintf(buf, sizeof(buf), "cd %s; tar -xf %s %s >/dev/null 2>&1",
            DOWNLOAD_PATH, SYSAPI_KERNEL_FILENAME, UPDATE_KERNEL_SCRIPT_NAME);

   if (WEXITSTATUS(system(buf)) != 0)
   {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
              "Unable to extract UPDATE script \n");
      /* indicate write failure */
      set_result_string("Unable to extract update script!");
      set_download_result_code(L7_TRANSFER_CODE_INVALID_CONFIG);
      set_result(L7_OTHER_FILE_TYPE_FAILED);
      return (L7_FAILURE);
   }
#endif
  /* execute the UPDATE_KERNEL script */
   snprintf(buf, sizeof(buf), "cd %s; ./%s >/dev/null 2>&1",
            DOWNLOAD_PATH, UPDATE_KERNEL_SCRIPT_NAME);
   if (WEXITSTATUS(system(buf)) != 0)
   {
      /* indicate write failure */
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSAPI_COMPONENT_ID,
              "Error updating kernel! \n");
      set_result_string("Error updating kernel!");
      set_download_result_code(L7_TRANSFER_CODE_INVALID_CONFIG);
      set_result(L7_OTHER_FILE_TYPE_FAILED);
      return (L7_FAILURE);
   }
   return (L7_SUCCESS);
}

/***************************************************************************
*
* @purpose  Check for stale RAM buffer, free if necessary
*
* @param    None
*
* @returns  None
*
* @notes    If download still in progress reschedules itself via osapiTimer.
*
* @end
*
***************************************************************************/
void L7_http_stale_buffer_check(L7_uint32 dummy1, L7_uint32 dummy2)
{
  L7_uint32 now;

  now = osapiUpTimeRaw();
  /* If 5 minutes have gone by without any data being received, this
     download is no good, free the buffer */
  if (( (now - L7_http_dl_data_last_seen) >= 300) &&
      (usmDbTransferInProgressGet(simGetThisUnit()) == L7_FALSE))
  {
    httpTransferFileCleanup();
    osapiTimerFree(pHttpTimer);
    return;
  }
  /* Not stale yet - check again in 5 minutes */
  osapiTimerAdd(L7_http_stale_buffer_check, 0, 0, 300000, &pHttpTimer);
}


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
L7_RC_t L7_http_process_transferred_file(L7_uint32 fileType)
{
   L7_RC_t    rc;
   L7_uint32  unit;
//#ifdef L7_CLI_PACKAGE
   L7_char8   buf[0x200];
   L7_uint32 file_size=0;
//#endif /* L7_CLI_PACKAGE */

   unit = usmDbThisUnitGet();

   memset (buf, 0x0, sizeof(buf));
   file_size=0;

   rc = usmDbTransferFileNameLocalGet(unit, transfer_fname_local);
   transfer_ftype = fileType;

   /* determine local filename... */
   switch (fileType)
   {
     case L7_FILE_TYPE_CODE:
       if (transfer_process_code(transfer_fname_local) == L7_SUCCESS)
       {
         set_result_string("File transfer operation completed successfully.");
         set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
         set_result(L7_TRANSFER_SUCCESS);
       }
       break;

     case L7_FILE_TYPE_KERNEL:
       if (transfer_process_kernel() == L7_SUCCESS)
       {
         set_result_string("Kernel transfer operation completed successfully.");
         set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
         set_result(L7_TRANSFER_SUCCESS);
       }
       break;

     case L7_FILE_TYPE_CONFIG:
       if (transfer_process_config() == L7_SUCCESS)
       {
         set_result_string("File transfer complete...storing configuration...");
         set_download_result_code(L7_TRANSFER_CODE_WRITING_FLASH);
         set_result(L7_TRANSFER_SUCCESS);

         /* sync here to write new config ... */

         sync();

         set_result_string("Configuration update completed successfully.");
         set_download_result_code(L7_TRANSFER_CODE_SUCCESS);

         /*
           No further action is needed here, as the config
           will now be applied as a result of
           transfer_process_config()
         */
        }
        break;
      case L7_FILE_TYPE_SSHKEY_RSA1:
      case L7_FILE_TYPE_SSHKEY_RSA2:
      case L7_FILE_TYPE_SSHKEY_DSA:
          if (transfer_process_keys_and_certs() == L7_SUCCESS)
          {
#ifdef L7_MGMT_SECURITY_PACKAGE
            if (sshdKeyValidate(fileType) != L7_SUCCESS)
            {
                sshdKeyDelete(fileType);
                set_result_string("Key file not valid!");
                set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                set_result(L7_OTHER_FILE_TYPE_FAILED);
            }
            else
            {
#endif /* L7_MGMT_SECURITY_PACKAGE */
                set_result_string("Host key file transfer operation completed successfully.");
                set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
                set_result(L7_TRANSFER_SUCCESS);
#ifdef L7_MGMT_SECURITY_PACKAGE
            }
#endif /* L7_MGMT_SECURITY_PACKAGE */
          }
        break;
      case L7_FILE_TYPE_SSLPEM_ROOT:
      case L7_FILE_TYPE_SSLPEM_SERVER:
      case L7_FILE_TYPE_SSLPEM_DHWEAK:
      case L7_FILE_TYPE_SSLPEM_DHSTRONG:
      case   L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
      case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT:
      case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
        if (transfer_process_keys_and_certs() == L7_SUCCESS)
    {
#ifdef L7_TR069_PACKAGE
          switch(transfer_ftype)
          {
            case L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
            case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT:
            case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
                 usmdbTr069SSLCertsLoad();
                 break;
            default:
                break;
          }
#endif
          set_result_string("Certificate file transfer operation completed successfully.");
          set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
          set_result(L7_TRANSFER_SUCCESS);
        }
        break;
#ifdef L7_CLI_PACKAGE
      case L7_FILE_TYPE_TXTCFG:
            snprintf(buf, sizeof(buf), "%s/%s", DOWNLOAD_PATH, TEMP_CONFIG_SCRIPT_FILE_NAME);
            /* NOTE: Downloading a script to the startup-config slot saves it on the
             *       file system, but it is not applied until the platform is rebooted.
             *       This prevents active users from being logged out of management interfaces.
             *
             *       Hence, cliTxtCfgSaveTransConfig() is replaced with cliTxtCfgCopyConfig() below.
             */
            (void)cliTxtCfgCopyConfig(buf, L7_NULLPTR, L7_TRUE);
            set_result_string("File transfer successful...");
            set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
            set_result(L7_TRANSFER_SUCCESS);
            break;

      case L7_FILE_TYPE_CLI_BANNER:

        /* Check the size of the file */
        if (osapiFsFileSizeGet(DOWNLOAD_PATH"/"CLI_BANNER_FILE_NAME,&file_size)==L7_SUCCESS)
        {
          /* TBD:Align the hardcode value with the banner macros */
          if (file_size > BANNER_MAX_FILE_SIZE)
          {
            set_result_string("File transfer error: CLI Banner file is too big to display.");
            set_download_result_code(L7_TRANSFER_CODE_FAILURE);
            set_result(L7_TRANSFER_FAILED);
          }
          else
          {
            if(cliWebSystemReadBannerFile(DOWNLOAD_PATH"/"CLI_BANNER_FILE_NAME) == L7_SUCCESS)
            {
              snprintf(buf, sizeof(buf), "cp %s/%s %s/%s >/dev/null 2>&1",
                       DOWNLOAD_PATH, CLI_BANNER_FILE_NAME,
                       CONFIG_PATH, CLI_BANNER_FILE_NAME);

              if (WEXITSTATUS(system(buf)) != 0)
              {
                cliWebSystemReadBannerFile(CLI_BANNER_FILE_NAME);
                set_result_string("File transfer error : Failed to copy the CLI banner file.");
                set_download_result_code(L7_TRANSFER_CODE_FAILURE);
                set_result(L7_TRANSFER_FAILED);
              }
              else
              {
                set_result_string("File transfer successful...");
                set_download_result_code(L7_TRANSFER_CODE_SUCCESS);
                set_result(L7_TRANSFER_SUCCESS);
              }
            }
            else
            {
              cliWebSystemReadBannerFile(CLI_BANNER_FILE_NAME);
              set_result_string("File transfer error : Failed to copy the CLI banner file.");
              set_download_result_code(L7_TRANSFER_CODE_FAILURE);
              set_result(L7_TRANSFER_FAILED);
            }
          }/* End:file_size */
        } /* End:osapiFsFileSizeGet */
        else
        {
          set_result_string("File transfer error : Failed to copy the CLI banner file.");
          set_download_result_code(L7_TRANSFER_CODE_FAILURE);
          set_result(L7_TRANSFER_FAILED);
        }
        osapiFsDeleteFile(DOWNLOAD_PATH"/"CLI_BANNER_FILE_NAME);
        break;
#endif /* L7_CLI_PACKAGE */
      default:
        /* indicate unknown transfer filetype */
         rc =  L7_FAILURE;
         break;
   }

   transfer_result=L7_NO_STATUS_CHANGE;
   set_result_string("  ");
   set_download_result_code(L7_TRANSFER_CODE_NONE);

   return(rc);
}

/*********************************************************************
*
* @purpose Prepare the system for a HTTP file download
*
* @param
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t httpTransferFilePrepare(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* Unmount tmpfs first. */
  if(0 > system("umount " DOWNLOAD_PATH " >/dev/null 2>&1")){}

  /* mount tmpfs directory for temporary storage... */
  if (WEXITSTATUS(system("mount -t tmpfs tmpfs " DOWNLOAD_PATH
                         " >/dev/null 2>&1")) != 0)
  {
    rc = L7_FAILURE;
  }

  if (pHttpTimer != NULL)
  {
    osapiTimerFree(pHttpTimer);
    pHttpTimer = NULL;
  }
  osapiTimerAdd(L7_http_stale_buffer_check, 0, 0, 300000, &pHttpTimer);

  return rc;
}
/*********************************************************************
*
* @purpose Cleanup the system after a HTTP file download
*
* @param
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t httpTransferFileCleanup(void)
{
  L7_RC_t rc = L7_SUCCESS;

  /* flush fs buffers... */
  sync();

  /* umount tmpfs directory... */
  if(0 > system("umount " DOWNLOAD_PATH " >/dev/null 2>&1")){}

  return rc;
}

/*********************************************************************
*
* @purpose After a HTTP file download, rename the temporary file to
*          the destination file name.
*
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
L7_RC_t httpTransferFileRename(L7_FILE_TYPES_t fileType,
                               L7_char8 *srcFile,
                               L7_char8 *dstFile)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 buf[256];

  memset(buf, 0, sizeof(buf));

  switch (fileType)
  {
    case L7_FILE_TYPE_CODE:
#ifdef L7_CODE_UPDATE_TO_FLASH
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               CONFIG_PATH, srcFile, CONFIG_PATH, dstFile);

#else
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, dstFile);
#endif
    simSetTransferFileNameLocal(dstFile);
    break;

    case L7_FILE_TYPE_CONFIG:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, SYSAPI_CONFIG_FILENAME);

    break;

    case L7_FILE_TYPE_KERNEL:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, SYSAPI_KERNEL_FILENAME);

    break;

    case L7_FILE_TYPE_SSHKEY_RSA1:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_SSHD_NAME_SERVER_PRIVKEY);

      break;

    case L7_FILE_TYPE_SSHKEY_RSA2:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_SSHD_NAME_SERVER_PRIVKEY_RSA);
      break;

    case L7_FILE_TYPE_SSHKEY_DSA:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_SSHD_NAME_SERVER_PRIVKEY_DSA);
      break;

    case L7_FILE_TYPE_SSLPEM_ROOT:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_SSLT_ROOT_PEM);
      break;

    case L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_TR069_ACS_SSLT_ROOT_PEM);
      break;

    case L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_TR069_CLIENT_SSL_PRIV_KEY);
      break;

    case L7_FILE_TYPE_TR069_CLIENT_SSL_CERT:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_TR069_CLIENT_SSL_CERT);
      break;

    case L7_FILE_TYPE_SSLPEM_SERVER:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_SSLT_SERVER_PEM);
      break;

    case L7_FILE_TYPE_SSLPEM_DHWEAK:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_SSLT_DHWEAK_PEM);
      break;

    case L7_FILE_TYPE_SSLPEM_DHSTRONG:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, L7_SSLT_DHSTRONG_PEM);
      break;

    case L7_FILE_TYPE_CONFIG_SCRIPT:
    case L7_FILE_TYPE_TXTCFG:
    case L7_FILE_TYPE_IAS_USERS:
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, TEMP_CONFIG_SCRIPT_FILE_NAME);
      break;

    case L7_FILE_TYPE_CLI_BANNER:
      osapiFsDeleteFile(DOWNLOAD_PATH"/"CLI_BANNER_FILE_NAME);
      snprintf(buf, sizeof(buf), "mv %s/%s %s/%s >/dev/null 2>&1",
               DOWNLOAD_PATH, srcFile, DOWNLOAD_PATH, CLI_BANNER_FILE_NAME);
      break;
    default:
      rc = L7_FAILURE;
      break;

  }

  if (rc == L7_SUCCESS)
  {
    if (WEXITSTATUS(system(buf)) != 0)
    {
      rc = L7_FAILURE;
    }
  }

  return rc;
}

/**************************************************************************
 *
 * @purpose  Use TFTP client to get soc script
 *
 * @param    file_name   @b{(input)} name string of file on remote server
 * @param    inet_address  @b{(input)} inet-address of remote server
 *
 * @returns UPD_TFTP_DONE - Operational code file is received.
 * @returns UPD_TFTP_FAILED - TFTP failed.
 *
 * @end
 *
 *************************************************************************/
L7_uint32 upd_get_socscript_via_tftp (L7_char8 * file_name,
                                      L7_inet_addr_t * inet_address)
{
  printf("\nThe function %s does not exist for Linux. Please create it.\n", __func__);

  return(L7_ERROR);
}

/**************************************************************************
 *
 * @purpose  Use TFTP client to get soc script
 *
 * @param    file_name   @b{(input)} name string of file on remote server
 * @param    inet_address  @b{(input)} inet-address of remote server
 *
 * @returns UPD_TFTP_DONE - Operational code file is received.
 * @returns UPD_TFTP_FAILED - TFTP failed.
 *
 * @end
 *
 *************************************************************************/
L7_uint32 upd_put_socscript_via_tftp (L7_char8 * file_name,
                                      L7_inet_addr_t * inet_address)
{
  printf("\nThe function %s does not exist for Linux. Please create it.\n", __func__);

  return(L7_ERROR);
}
