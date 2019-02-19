#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include <stdio.h>
#include <string.h>
#include "cli_txtcfg_util.h"
#include "zlib.h"
#include "osapi_file.h"
#include "sysapi.h"

#define CLI_TXT_CFG_COMPRESS_MAGIC 0xDEADC0DE

typedef struct
{
  L7_uint32 dataLen;
  L7_uint32 magic;
} cliTxtCfgCompHdr_t;

static char * delim = "\n";
static int delimLen = 1;
static int debug = 0;
void cliTxtCfgUtilDebug (int val)
{
  debug = val;
}

static void dtrace (const char * fmt, ...)
{
  va_list args;

  if (debug != 0)
  {
    va_start (args, fmt);
    vprintf (fmt, args);
    va_end (args);
  }
}
/*********************************************************************
* @purpose  This function open the text based configuration file and
   intialize command pointer structure.
*
* @param    L7_char8*        filename  text configuration file name
* @param    cliTxtCfgCmd_t*  cmdp      command pointer.
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliTxtCfgCmdInit (L7_char8 * filename, cliTxtCfgCmd_t * cmdp)
{
  memset (cmdp, 0, sizeof (*cmdp));
  dtrace ("cliTxtCfgCmdInit: %s\n", filename);

  /* open config script file */
  strncpy (cmdp->filename, filename, sizeof (cmdp->filename));
  cmdp->filedesc = osapiFsOpen (filename);
  if (L7_ERROR == cmdp->filedesc)
  {
    dtrace ("Failed to open %s\n", filename);
    cmdp->filedesc = -1;
    return L7_ERROR;
  }

  cmdp->offset = 0;
  memset (cmdp->cmdBuf, 0, sizeof (cmdp->cmdBuf));
  cmdp->length = sizeof (cmdp->cmdBuf) - cmdp->offset - 1;

  return L7_SUCCESS;

}
/*********************************************************************
* @purpose  This function closes the text based configuration file
*
* @param    cliTxtCfgCmd_t*  cmdp      command pointer.
*
* @returns  void
*
* @notes none
*
* @end
*********************************************************************/
void cliTxtCfgCmdEnd (cliTxtCfgCmd_t * cmdp)
{
  if (cmdp != L7_NULLPTR && cmdp->filedesc != -1)
  {
    osapiFsClose (cmdp->filedesc);
    cmdp->filedesc = -1;
  }
}
/*********************************************************************
* @purpose  This function truncate the junk characters(ctrl M's)
*           both sides of the command to be executed.
*
* @param    L7_char8* str.
*
* @returns  L7_char8* pointer to truncated command.
*
* @notes none
*
* @end
*********************************************************************/
static L7_char8 *cliTxtCfgTrim (L7_char8 * str)
{
  if (str && str[0])
  {
    L7_char8 * temp = str + strlen (str) - 1;
    while ((*temp == ' ' || *temp == '\t' || *temp == '\r' || *temp == '\n') && temp >= str)
    {
      *temp-- = 0;
    }

    while (str && (str[0] == ' ' || str[0] == '\t' || *temp == '\r' || *temp == '\n'))
    {
      str++;
    }
  }
  return str;
}
/*********************************************************************
* @purpose  This function reads the specified number of bytes from the
*           text based configuration file into buffer.
*
* @param    L7_uint32  fd    text based config file descriptior.
* @param    L7_char8*  buf   buffer that needs to be filled.
* @param    L7_uint32* lenp  number of bytes to be read.
*
* @returns  L7_RC_t
*
* @notes none
*
* @end
*********************************************************************/
static L7_RC_t cliTxtCfgReadFile (L7_uint32 fd, L7_char8 * buf, L7_uint32 * lenp)
{
  L7_char8 dataBuf[CONFIG_SCRIPT_MAX_COMMAND_SIZE];
  cliTxtCfgCompHdr_t hdr;
  L7_uint32 rv, len;
  L7_RC_t rc;

  dtrace ("cliTxtCfgReadFile: buffer size = %d\n", *lenp);
  len = sizeof (hdr);
  rc = osapiFileReadWithLen (fd, (L7_char8 *) &hdr, &len);
  if (rc != L7_SUCCESS)
  {
    dtrace ("cliTxtCfgReadFile: failed osapiFileReadWithLen\n");
    *lenp = 0;
    return rc;
  }

  if (len == 0)
  {
    *lenp = len;
  }
  else if (hdr.magic != CLI_TXT_CFG_COMPRESS_MAGIC)
  {
    dtrace ("cliTxtCfgReadFile: startup-config is not compressed\n");
    memcpy (buf, &hdr, sizeof (hdr));
    len = *lenp - sizeof (hdr);
    osapiFileReadWithLen (fd, &buf[sizeof (hdr)], &len);
    *lenp = len;
    *lenp += len ? sizeof (hdr) : 0;
  }
  else
  {
    dtrace ("cliTxtCfgReadFile: startup-config is compressed\n");
    if (hdr.dataLen > CONFIG_SCRIPT_MAX_COMMAND_SIZE || *lenp < hdr.dataLen)
    {
      return L7_ERROR;
    }
    rc = osapiFileRead (fd, dataBuf, hdr.dataLen);
    if (rc != L7_SUCCESS)
    {
      dtrace ("cliTxtCfgReadFile: failed osapiFileReadWithLen\n");
      *lenp = 0;
      return rc;
    }
    rv = uncompress (buf, (void *) lenp, dataBuf, hdr.dataLen);
    if (rv != 0)
    {
      dtrace ("failed to uncomress errcode = %d\n", rv);
      return L7_ERROR;
    }
  }
  dtrace ("cliTxtCfgReadFile: success len = %d\n", *lenp);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function reads command one by one from command blocks
*
* @param    cliTxtCfgCmd_t*  cmdp        command pointer.
* @param    L7_BOOL          skipComment tells commented commands
*                                        skipped or not.
* @returns  L7_char8*
*
* @notes none
*
* @end
*********************************************************************/
L7_char8 *cliTxtCfgCmdGet (cliTxtCfgCmd_t * cmdp, L7_BOOL skipComment)
{
  L7_char8 * ptok;
  L7_char8 * cmdPtr;
  L7_char8 * cmdPtr2;

  if (cmdp->filedesc == -1)
  {
    return L7_NULLPTR;
  }

  /* Read Config Commands */
  do
  {
    cmdPtr = &cmdp->cmdBuf[cmdp->offset];
    while ((ptok = strstr (cmdPtr, delim)))
    {
      *ptok = 0;                /* Terminate the command */
      cmdPtr2 = cliTxtCfgTrim (cmdPtr);
      if (!(L7_TRUE == skipComment && cmdPtr2[0] == '!') && cmdPtr2[0] != 0)
      {
        dtrace ("cliTxtCfgCmdGet: returns '%s'\n", cmdPtr2);
        cmdp->offset = ptok - cmdp->cmdBuf + delimLen;
        return cmdPtr2;
      }
      cmdPtr = ptok + delimLen;
    }

    cmdp->offset = strlen (cmdPtr);
    cmdp->length = sizeof (cmdp->cmdBuf) - cmdp->offset - 1;
    memmove (cmdp->cmdBuf, cmdPtr, cmdp->offset);
    memset (&cmdp->cmdBuf[cmdp->offset], 0, cmdp->length);
    cliTxtCfgReadFile (cmdp->filedesc, &cmdp->cmdBuf[cmdp->offset], &cmdp->length);
    cmdp->cmdBuf[cmdp->offset + cmdp->length] = 0;
    cmdp->offset = 0;
  }
  while (cmdp->cmdBuf[cmdp->offset] != 0 && cmdp->length != 0);

  dtrace ("cliTxtCfgCmdGet: no more commands to read\n");
  return L7_NULLPTR;
}
/*********************************************************************
* @purpose  This function writes configuration commnds into text based
*           configuration file block by block.
*
* @param    L7_uint32  fd      text based config file descriptior.
* @param    L7_char8*  block   In this block commands are stored.
* @param    L7_uint32* lenp    number of bytes to be write.
* @param    L7_BOOL    useComp compression should be used or not.
*
* @returns  L7_RC_t
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliTxtCfgWriteBlock (L7_int32 fd, L7_char8 * block, L7_uint32 * lenp, L7_BOOL useComp)
{
  L7_uint32 rc;
  cliTxtCfgCompHdr_t hdr;
  L7_char8 scrCompBuf[CONFIG_SCRIPT_MAX_COMMAND_SIZE * 2];

  dtrace ("cliTxtCfgWriteBlock: length = %d\n", *lenp);

  if (useComp == L7_FALSE)
  {
    return osapiFsWriteNoClose (fd, block, *lenp);
  }

  hdr.magic = CLI_TXT_CFG_COMPRESS_MAGIC;
  hdr.dataLen = sizeof (scrCompBuf);
  rc = compress (scrCompBuf, (void *) &hdr.dataLen, block, *lenp);
  if (rc != 0)
  {
    dtrace ("cliTxtCfgWriteBlock: compress failed = %d\n", rc);
    return L7_ERROR;
  }
  if (L7_SUCCESS != osapiFsWriteNoClose (fd, (L7_char8 *) &hdr, sizeof (hdr)))
  {
    dtrace ("cliTxtCfgWriteBlock: failed to write header\n");
    return L7_ERROR;
  }
  *lenp = hdr.dataLen + sizeof (hdr);
  dtrace ("cliTxtCfgWriteBlock: writing compressed data size %d\n", hdr.dataLen);
  return osapiFsWriteNoClose (fd, scrCompBuf, hdr.dataLen);
}
/*********************************************************************
* @purpose  This function writes text based configuration from one
*           file to other
*
* @param    L7_char8* readFile   Read file name.
* @param    L7_char8* writeFile  Write file name
* @param    L7_BOOL   useComp compression should be used or not.
*
* @returns  L7_RC_t
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t cliTxtCfgCopyConfig (L7_char8 * readFile, L7_char8 * writeFile, L7_BOOL useComp)
{
  L7_int32 fd;
  L7_uint32 len = 0;
  L7_uint32 index = 0;
  cliTxtCfgCmd_t cmd, * cmdp = &cmd;
  L7_char8 * command = L7_NULLPTR;
  L7_char8 cmdString[CONFIG_SCRIPT_MAX_COMMAND_SIZE];

  if (readFile == L7_NULLPTR)
  {
    dtrace ("cliTxtCfgCopyConfig: Error input file\n");
    return L7_ERROR;
  }

  writeFile = writeFile ? writeFile : SYSAPI_TXTCFG_FILENAME;

  dtrace ("cliTxtCfgCopyConfig: copying from %s to %s\n", readFile, writeFile);

  osapiFsDeleteFile (writeFile);
  osapiFsFileCreate (writeFile, &fd);

  if(cliTxtCfgCmdInit (readFile, cmdp) == L7_ERROR)
  {
    dtrace ("cliTxtCfgCopyConfig: Error not able to intialize input file.\n");
    return L7_ERROR;
  }

  memset (cmdString, 0, sizeof (cmdString));
  while (L7_NULLPTR != (command = cliTxtCfgCmdGet (cmdp, L7_FALSE)))
  {
    dtrace ("cliTxtCfgCopyConfig: cmd read is %s\n", command);
    len = strlen (command);
    if (index + len + 2 >= sizeof (cmdString))
    {
      dtrace ("cliTxtCfgCopyConfig: block data %s\n", cmdString);
      cliTxtCfgWriteBlock (fd, cmdString, &index, useComp);
      index = 0;
      cmdString[0] = 0;
    }
    strcat (&cmdString[index], command);
    strcat (&cmdString[index + len], "\n\n");
    index = index + len + 2;
  }
  dtrace ("cliTxtCfgCopyConfig: final block %s\n", cmdString);
  cliTxtCfgWriteBlock (fd, cmdString, &index, useComp);

  cliTxtCfgCmdEnd (cmdp);

  return osapiFsClose (fd);
}

#ifdef FP_CFG_COMP_TOOL
int main (int argc, char * * argv)
{
  char * command;
  cliTxtCfgCmd_t cmd, * cmdp = &cmd;

  dtrace ("Converting config\n");
  cliTxtCfgCopyConfig (argv[1], argv[2], L7_TRUE);

  dtrace ("testing config\n");
  cliTxtCfgCmdInit (argv[2], cmdp);
  while (L7_NULLPTR != (command = cliTxtCfgCmdGet (cmdp, L7_TRUE)))
  {
    dtrace ("CMD: %s\n", command);
  }
  return 0;
}
#endif
