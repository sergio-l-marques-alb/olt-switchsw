/*********************************************************************
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/config_script_api.c
 *
 * @purpose Implementation functions for configuration script.
 *
 * @component user interface
 *
 * @comments
 *
 * @create  01/15/2004
 *
 * @author  Sarvesh Kumar
 * @end
 *
 **********************************************************************/
#include "cliapi.h"
#include "strlib_common_common.h"
#include "strlib_common_cli.h"
#include "strlib_base_common.h"
#include "strlib_base_cli.h"
#include <string.h>
#include "clicommands.h"
#include "ews_clix.h"
#include "cli_config_script.h"
#include "nvstoreapi.h"
#include "osapi.h"

#include <fcntl.h>
#include <unistd.h>
#include "transfer_exports.h"

static L7_char8 lastErrorMessage[256];
static L7_char8 configScriptSourceFileName[L7_MAX_FILENAME];

extern L7_uint32 showRunningConfigAllFlag;

/*********************************************************************
*
* @purpose  Function to return pointer to next non " " address
*
*
* @param char *str
*
* @returntype char*
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_char8 * trimLeft(L7_char8 * str)
{
  if(strlen(str)>0)
  {
    while (((strncmp(str, " ", 1) == 0) || (strncmp(str, "\t", 1) == 0)) && strlen(str) > 0)  /* Strip off leading blanks */
    {
      str++;
    }
  }

  return str;
}
/*********************************************************************
*
* @purpose  Function to return pointer to next non 
*           carriage return and non new line. 
*
*
* @param char *str
*
* @returntype char*
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_char8 * trimLeftBlanks(L7_char8 * str)
{
  if(strlen(str)>0)
  {
    while (((strncmp(str, "\r", 1) == 0) || (strncmp(str, "\n", 1) == 0)) && strlen(str) > 0)  /* Strip off leading blanks */
    {
      str++;
    }
  }

  return str;
}
/*********************************************************************
*
* @purpose  Function to trims space and tab char from right side of string
*
*
* @param char *str
*
* @returntype
*
* @notes
*
*
* @end
*
*********************************************************************/
void trimRight(L7_char8 * str)
{
  char * temp;

  if(strlen(str)>0)
  {
    temp = str + strlen(str) ;
    temp--;

    while (((strncmp(temp, " ", 1) == 0) || (strncmp(temp, "\t", 1) == 0)) && temp >= str)  /* Strip off leading blanks */
    {
      temp[0] = '\0';
      temp--;
    }
  }
}

/*********************************************************************
*
* @purpose  Function to trims carriage return and new line char from 
*           right side of string
*
*
* @param char *str
*
* @returntype
*
* @notes
*
*
* @end
*
*********************************************************************/
void trimRightBlanks(L7_char8 * str)
{
  char * temp;

  if(strlen(str)>0)
  {
    temp = str + strlen(str) ;
    temp--;

    while (((strncmp(temp, "\r", 1) == 0) || (strncmp(temp, "\n", 1) == 0)) && temp >= str)  /* Strip off leading blanks */
    {
      temp[0] = '\0';
      temp--;
    }
  }
}

/*********************************************************************
*
* @purpose  Function to trims space and tab char from both side of string
*
*
* @param char *str
*
* @returntype char*
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_char8 * allTrim(L7_char8 * str)
{
  trimRight(str);
  return trimLeft(str);
}

/*********************************************************************
*
* @purpose  Function to set next command in script buffer
*
*
* @param const L7_char8 *command
* @param L7_ConfigScript_t *configScriptData
*
* @returntype L7_RC_t
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t setNextConfigScriptCommand(const L7_char8 * command, L7_ConfigScript_t * configScriptData)
{
  cfgCommand_t * cfgDataTemp;
  L7_uint32 i, len, save;
  L7_char8 errMsg[128];

  /*Initialise*/
  cfgDataTemp = L7_NULL;

  if(showRunningConfigAllFlag == 0)
  {
    if(configScriptData->size >= CONFIG_SCRIPTS_MAX_SIZE_LIMIT)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, errMsg, sizeof(errMsg),
      pStrInfo_base_ScriptingOperationCantContinueMaxAvailableSizeKbytesLimitExceeded,
                            (L7_uint32) (CONFIG_SCRIPTS_MAX_SIZE_LIMIT/1024));
      setLastError(errMsg);
      return L7_FAILURE;
    }
  }
  else
  {  
    if(configScriptData->size >= SHOW_RUNNING_CONFIG_ALL_MAX_SIZE_LIMIT)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, errMsg, sizeof(errMsg), 
      pStrInfo_base_ScriptingOperationCantContinueMaxAvailableSizeKbytesLimitExceeded,
                            (L7_uint32) (SHOW_RUNNING_CONFIG_ALL_MAX_SIZE_LIMIT/1024));
      setLastError(errMsg);
      return L7_FAILURE;
    }
  }

  if(configScriptData->cfgData == L7_NULL)
  {
    /*allocate memory*/
    cfgDataTemp = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, sizeof(cfgCommand_t));
    if(cfgDataTemp == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    cfgDataTemp->nextWritePos = 0;
    cfgDataTemp->noOfCmds = 0;
    cfgDataTemp->next = L7_NULLPTR;
    configScriptData->cfgData =  cfgDataTemp;
    configScriptData->last =  cfgDataTemp;
    configScriptData->listCount = 1;
  }
  else
  {
    cfgDataTemp = configScriptData->last;
  }

  len = strlen(command);
  if (len >= CONFIG_SCRIPT_MAX_COMMAND_SIZE)
  {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, errMsg, sizeof(errMsg),
      pStrInfo_base_ScriptingOperationCantContinueAsMaxCommandSizeExceeded,
                            (L7_uint32) (CONFIG_SCRIPT_MAX_COMMAND_SIZE));
      setLastError(errMsg);
      return L7_FAILURE;
  }

  if(cfgDataTemp->nextWritePos + len >= CONFIG_SCRIPT_MAX_COMMAND_SIZE)
  {
    cfgDataTemp->next = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, sizeof(cfgCommand_t));
    if(cfgDataTemp->next == L7_NULLPTR)
    {
      return L7_FAILURE;
    }

    cfgDataTemp = cfgDataTemp->next;
    configScriptData->last = cfgDataTemp;
    cfgDataTemp->nextWritePos = 0;
    cfgDataTemp->noOfCmds = 0;
    cfgDataTemp->next = L7_NULLPTR;
    configScriptData->listCount++;
  }

  memcpy(cfgDataTemp->cmdString + cfgDataTemp->nextWritePos, command, len);
  save = cfgDataTemp->nextWritePos;
  cfgDataTemp->nextWritePos += len;
  configScriptData->size += len;
  memcpy(cfgDataTemp->cmdString + cfgDataTemp->nextWritePos, "\0", 1);
  cfgDataTemp->nextWritePos++;
  cfgDataTemp->noOfCmds++;
  configScriptData->size++;

  /* replace carriage return */
  for (i = save; i < cfgDataTemp->nextWritePos; i++)
  {
    if(cfgDataTemp->cmdString[i] == '\r')
    {
      cfgDataTemp->cmdString[i] = ' ';
    }
    else if (cfgDataTemp->cmdString[i] == '\n')
    {
      cfgDataTemp->cmdString[i] = '\0';
      cfgDataTemp->noOfCmds++;
    }
  }

  configScriptData->cfgHdr.dataChanged = L7_TRUE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Function to script command at specified line number
*
*
* @param L7_uint32 lineNumber
* @param L7_ConfigScript_t *configScriptData
* @param L7_char8* * command
*
* @returntype L7_RC_t
* @returns L7_SUCCESS or L7_FAILURE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t getConfigScriptCommand(L7_uint32 lineNumber, L7_ConfigScript_t * configScriptData, L7_uchar8 * * command)
{
  L7_RC_t rc = L7_FAILURE;
  cfgCommand_t * cfgDataTemp;
  L7_uint32 cmdCount;

  /*Initialise*/
  cfgDataTemp = L7_NULL;

  if(configScriptData->cfgData != L7_NULL && lineNumber > 0)
  {
    cfgDataTemp = configScriptData->cfgData;
    cmdCount = 0;

    /*get the last link*/
    while(cfgDataTemp != L7_NULL)
    {
      if((cmdCount + cfgDataTemp->noOfCmds) >=  lineNumber)
      {
        break;
      }

      cmdCount += cfgDataTemp->noOfCmds;

      cfgDataTemp = cfgDataTemp->next;
    }

    /*this link contains the command*/
    if(cfgDataTemp != L7_NULL)
    {
      *command = cfgDataTemp->cmdString;
      cmdCount++;

      while(cmdCount < lineNumber)
      {
        *command += (strlen(*command) + 1);
        cmdCount++;
      }

      rc = L7_SUCCESS;
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Gets the file names and size into buffer
*
* @param    SYSAPI_DIRECTORY_t *tree
*
* @returns  Number of files
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 getConfigScriptList(SYSAPI_DIRECTORY_t * tree)
{
  L7_char8 buf[SYSAPI_MAX_FILE_ENTRIES * L7_MAX_FILENAME];
  L7_char8 * pbuf, * ptok;
  L7_uint32 num_files;
  L7_uint32 num_cfg_files;
  L7_char8 *ptr;
  int fileNameLen;

  num_cfg_files = 0;

  pbuf = buf;
  /* Get the directory listing */
  if ( osapiFsDir(pbuf, (L7_uint32)sizeof(buf)) != L7_SUCCESS )
  {
	L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_CLI_WEB_COMPONENT_ID, "osapiFsDir failed. Failed to obtain "
							  "the directory information from a volume's directory.");
  }

  ptok = strtok(pbuf, "\n");     /* Toss the '.' and '..' entries */
  ptok = strtok((L7_char8 *)'\0', "\n");

  num_files = 0;
  do
  {
    ptok = strtok((L7_char8 *)'\0', "\n");    /* Pull the filenames, one at a time */
    if ( ptok != NULL )
    {

      /* check file type*/
      if ((strstr(ptok, CONFIG_SCRIPT_FILE_EXTENSION)) != NULL )
      {
        /* Determine the length of the file name */
        ptr = strstr(ptok," ");
        if (ptr == L7_NULLPTR)
        {
          continue;
        }
        fileNameLen = (int)ptr - (int)ptok + 1;
        if (fileNameLen >= L7_MAX_FILENAME)
          fileNameLen = L7_MAX_FILENAME-1;
        osapiStrncpySafe(tree[num_files].filename, ptok, fileNameLen);

        /* Get the file size */
        while ( strncmp(ptr, " ", 1) == 0 )  /* Strip off leading blanks */
        {
          ptr++;
        }
        tree[num_files].filesize = (L7_uint32)atoi(ptr);

        num_files++;
      }
    }
  } while ( ptok != NULL);

  return num_files;
}

/*********************************************************************
* @purpose  Checks the file name for 'scr' extension
*
* @param    const L7_char8 *filename
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t checkConfigScriptFiletype(const L7_char8 * filename)
{
  L7_char8 * pExtn;
  L7_char8 errMsg[128];

  if(strlen(filename) < L7_MAX_FILENAME)
  {
    pExtn = strstr(filename, CONFIG_SCRIPT_FILE_EXTENSION );

    /* check file type*/
    if (pExtn != NULL && strlen(pExtn) == 4)
    {
      return L7_SUCCESS;
    }
    else
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrInfo_base_FileExtensionOtherThanIsNotAllowedPleaseUseFileExtension,
                              filename, CONFIG_SCRIPT_FILE_EXTENSION, CONFIG_SCRIPT_FILE_EXTENSION);
      setLastError(errMsg);
      return L7_FAILURE;
    }
  }
  else
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrInfo_base_MaxLenOfFileNameIs, L7_MAX_FILENAME - 1);
    setLastError(errMsg);
    return L7_FAILURE;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Checks the file name
*
* @param    const L7_char8 *filename
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t checkConfigScriptFilename(const L7_char8 * filename)
{
  L7_char8 errMsg[128];

  if(strcmp(filename, TEMP_CONFIG_SCRIPT_FILE_NAME) == 0 || strcmp(filename, TEMP_RUNNINGCONFIG_SCRIPT_FILE_NAME) == 0)
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrInfo_base_FileNameIsReservedPleaseUseDifferentFileName, filename);
    setLastError(errMsg);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks whether a file is already present of not
*
* @param    Pointer to L7_char8
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t checkConfigScriptPresent(const L7_char8 * filename)
{
  L7_uint32 rc, num_files, i;
  L7_char8 errMsg[128];
  SYSAPI_DIRECTORY_t tree[SYSAPI_MAX_FILE_ENTRIES];

  /*Initialise*/
  num_files = 0;
  rc = L7_FAILURE;

  if ( sysapiTreeMalloc(tree) == L7_SUCCESS )
  {
    num_files = getConfigScriptList(tree);

    for(i =0; i < num_files; i++)
    {
      if(strcmp(filename, tree[i].filename) == 0)
      {
        rc = L7_SUCCESS;
        break;
      }
    }

    if(rc != L7_SUCCESS)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrInfo_base_CfgScriptNotFound, filename);
      setLastError(errMsg);
    }

    sysapiTreeFree(tree);
  }

  return rc;
}

/*********************************************************************
* @purpose  Puts the file contents into buffer
*
* @param    const L7_char8 *filename
* @param    L7_ConfigScript_t *configScriptData
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t readConfigScriptData(const L7_char8 * filename, L7_ConfigScript_t * configScriptData)
{
  L7_RC_t rc = L7_FAILURE;
  L7_int32 filedesc;
  L7_int32 i, ctr=0;
  L7_uchar8 * readConfigScript;
  L7_uchar8 tempBuffer[CONFIG_SCRIPT_MAX_COMMAND_SIZE * 2];
  L7_uint32 size;
  L7_char8 * ptok, * pnexttok;
  L7_char8 strFileName[L7_MAX_FILENAME];
  L7_char8 errMsg[128];
  L7_int32 charsRead = 0;
  L7_int32 charsProcessed = 0;

  /*Initialise*/
  memset (strFileName, 0, sizeof(strFileName));
  memset (tempBuffer, 0, sizeof(tempBuffer));
  OSAPI_STRNCPY_SAFE(strFileName,filename);

  if(checkConfigScriptFiletype(filename) != L7_SUCCESS ||
     checkConfigScriptPresent(filename) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  if(initialiseScriptBuffer(strFileName, configScriptData)== L7_SUCCESS)
  {
    size = sizeof(L7_fileHdr_t);

    readConfigScript = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, size);

    if(readConfigScript != L7_NULL)
    {
      /* open config script file */
      if (osapiFsOpen (strFileName, &filedesc) == L7_ERROR)
      {
         osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_InOpeningFile, strFileName);
         setLastError(errMsg);
         return L7_ERROR; 
      }

      memset (readConfigScript, 0,size);
      /*First call to read header only*/
      if (osapiFileRead(filedesc, (L7_char8 *)readConfigScript, size) != L7_ERROR)
      {
        /* copy the header to the configScriptData */
        memcpy(&configScriptData->cfgHdr, readConfigScript, sizeof(L7_fileHdr_t));

        if(osapiFsFileSizeGet(strFileName, &size) != L7_SUCCESS)
        {
           osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_InReadingFileSize, strFileName);
           setLastError(errMsg);
           return L7_ERROR; 
	}

        size -= (sizeof(L7_fileHdr_t) + sizeof(L7_uint32) + 1);

        /*Free previous memory allocation*/
        osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);

        ctr = CONFIG_SCRIPT_MAX_COMMAND_SIZE;

        /*allocate memory as per counter size of file*/
        readConfigScript = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, ctr);

        i = size/(CONFIG_SCRIPT_MAX_COMMAND_SIZE-1) + 1;

        if(readConfigScript != L7_NULL)
        {
          while (i >= 0)
          {
            i--;

            memset (readConfigScript, 0,ctr);
            /*second call to read couter size file*/
            if (osapiFileRead(filedesc, (L7_char8 *)readConfigScript, ctr - 1) != L7_ERROR)
            {
              charsRead = strlen(readConfigScript);
              charsProcessed=0;

              if(charsRead == 0)
              {
                break;
              }

              if(readConfigScript[0] == '\n')
              {
                if(strlen((char *)tempBuffer) > 0)
                {
                  if(setNextConfigScriptCommand((char *)tempBuffer, configScriptData) == L7_FAILURE)
                  {
                    osapiFsClose(filedesc);
                    /* free the memory */
                    osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
                    releaseScriptBuffer(configScriptData);
                    return L7_FAILURE;
                  }

                  memset (tempBuffer, 0, CONFIG_SCRIPT_MAX_COMMAND_SIZE);
                }
              }

              ptok = strtok ((L7_uchar8 *) readConfigScript, "\n");

              while (ptok != NULL)
              {
                /*Get next token*/
                pnexttok = strtok ((L7_char8 *)'\0', "\n");
                charsProcessed += strlen(ptok) + CONFIG_SCRIPT_DELIMITER_LEN;

                if( pnexttok == NULL)   /*check if current entry is last entry*/
                {
                  if((charsRead > charsProcessed) || (readConfigScript[charsRead - 1] == '\0') || (readConfigScript[charsRead - 1] == '\n'))
                  {
                    if(strlen(ptok) > 0)
                    {
                      strcat(tempBuffer, ptok);
                      if(setNextConfigScriptCommand(tempBuffer, configScriptData) == L7_FAILURE)
                      {
                        osapiFsClose(filedesc);
                        /* free the memory */
                        osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
                        releaseScriptBuffer(configScriptData);
                        return L7_FAILURE;
                      }
                      memset (tempBuffer, 0, CONFIG_SCRIPT_MAX_COMMAND_SIZE);
                    }
                  }
                  else
                  {
                    memset (tempBuffer, 0, CONFIG_SCRIPT_MAX_COMMAND_SIZE);
                    memcpy(&tempBuffer,ptok,strlen(ptok));
                  }
                  break;
                }
                else
                {
                  if(strlen(tempBuffer) > 0)
                  {
                    OSAPI_STRNCAT(tempBuffer, ptok);
                    if(setNextConfigScriptCommand(tempBuffer, configScriptData) == L7_FAILURE)
                    {
                      osapiFsClose(filedesc);
                      /* free the memory */
                      osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
                      releaseScriptBuffer(configScriptData);
                      return L7_FAILURE;
                    }
                    memset (tempBuffer, 0, CONFIG_SCRIPT_MAX_COMMAND_SIZE);
                  }
                  else
                  {
                    if(strlen(ptok) > 0)
                    {
                      if(setNextConfigScriptCommand(ptok, configScriptData) == L7_FAILURE)
                      {
                        osapiFsClose(filedesc);
                        /* free the memory */
                        osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
                        releaseScriptBuffer(configScriptData);
                        return L7_FAILURE;
                      }
                    }
                  }
                }

                ptok = pnexttok;
              }
            }
            else
            {
              osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_InReadingFile, strFileName);
              setLastError(errMsg);
              osapiFsClose(filedesc);
              /* free the memory */
              osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
              releaseScriptBuffer(configScriptData);
              return L7_FAILURE;
            }
          }  /*end of while */
        }
        else
        {
          setLastErrorAddBlanks (1, 0, 0, 0, pStrErr_common_Error, pStrInfo_base_InGettingMemory);
          return L7_FAILURE;
        }
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_InReadingFile, strFileName);
        setLastError(errMsg);
        osapiFsClose(filedesc);
        /*Free previous memory allocation*/
        osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);
        releaseScriptBuffer(configScriptData);
        return L7_FAILURE;
      }

      /* free the memory */
      osapiFree(L7_CLI_WEB_COMPONENT_ID, readConfigScript);

      osapiFsClose(filedesc);

      rc = L7_SUCCESS;

    }
    else
    {
      setLastErrorAddBlanks (1, 0, 0, 0, pStrErr_common_Error, pStrInfo_base_InGettingMemory);
      rc = L7_FAILURE;
    }
  }
  else
  {
    setLastErrorAddBlanks (1, 0, 0, 0, pStrErr_common_CouldNot, pStrInfo_base_InitialiseScriptingBuf);
    rc =  L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Writes the buffer data into file
*
* @param    L7_ConfigScript_t *configScriptData
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t writeConfigScriptData(L7_ConfigScript_t * configScriptData)
{
  L7_uint32 i, j;
  L7_uchar8 * readConfigScript, * originalReadConfigScript;
  L7_uint32 size;
  L7_int32 filedesc;
  L7_uint32 ctr=0;
  L7_uchar8 errMsg[128];
  L7_uchar8 * scriptCommand = L7_NULL;
  L7_uchar8 * scriptCommandMod;
  L7_uchar8 * scriptCommandModOrig;
  cfgCommand_t * cfgData;

  if (configScriptData->cfgHdr.dataChanged != L7_TRUE)
  {
    setLastErrorAddBlanks (1, 0, 0, 0, L7_NULLPTR, pStrInfo_base_NothingToWriteInFile);
  }

  if(checkConfigScriptCountLimit(configScriptData->cfgHdr.filename) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /*calculate size*/
  size = sizeof(L7_fileHdr_t) + sizeof(L7_uint32) +  CONFIG_SCRIPT_DELIMITER_LEN;

  cfgData = configScriptData->cfgData;

  for(i=0; i<configScriptData->listCount && cfgData != L7_NULL; i++)
  {
    /* add the length of string to size */
    size += cfgData->nextWritePos;
    cfgData = cfgData->next;
  }
  if (size == (sizeof(L7_fileHdr_t) + sizeof(L7_uint32) +  CONFIG_SCRIPT_DELIMITER_LEN))
  {
     setLastErrorAddBlanks (1, 0, 0, 0, pStrErr_common_Error, pStrInfo_base_InReadingDownloadedFile);
     return L7_FAILURE;
  }
  if(checkConfigScriptSizeLimit(size, configScriptData->cfgHdr.filename) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ctr = CONFIG_SCRIPT_MAX_COMMAND_SIZE;
  j = size/CONFIG_SCRIPT_MAX_COMMAND_SIZE + 1;
  i=0;

  readConfigScript = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, ctr);
  scriptCommandModOrig = osapiMalloc(L7_CLI_WEB_COMPONENT_ID, CONFIG_SCRIPT_MAX_COMMAND_SIZE);

  if(readConfigScript != NULL && scriptCommandModOrig != NULL)
  {
    originalReadConfigScript = readConfigScript;
    memset (readConfigScript, 0,ctr);
    if (osapiFsFileCreate(configScriptData->cfgHdr.filename, &filedesc) == L7_SUCCESS)
    {
      /*set the length in header*/
      configScriptData->cfgHdr.length = size;

      /* copy the header to the readConfigScript */
      memcpy(readConfigScript, &configScriptData->cfgHdr, sizeof(L7_fileHdr_t));
      readConfigScript += sizeof(L7_fileHdr_t);

      osapiStrncat(readConfigScript,"\n",(ctr-(readConfigScript-originalReadConfigScript)-1));
      readConfigScript++;

      /* call save NVStore routine */
      if (osapiFsWriteNoClose(filedesc, (L7_char8 *)originalReadConfigScript, sizeof(L7_fileHdr_t)+1) == L7_ERROR)
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_InCreatingFile, configScriptData->cfgHdr.filename);
        setLastError(errMsg);
        osapiFsClose(filedesc);
        /* free the memory */
        osapiFree(L7_CLI_WEB_COMPONENT_ID, originalReadConfigScript);
        osapiFsDeleteFile(configScriptData->cfgHdr.filename);
        return L7_FAILURE;
      }
    }
    while (j >= 0)
    {
      j--;

      readConfigScript = originalReadConfigScript;

      memset (readConfigScript, 0,ctr);
      while(getConfigScriptCommand(i+1, configScriptData, &scriptCommand) == L7_SUCCESS)
      {
        /*trim after copy original data*/
        /*original data is a pointer to data contained in link list*/

        scriptCommandMod = scriptCommandModOrig;
        memset (scriptCommandMod, 0, CONFIG_SCRIPT_MAX_COMMAND_SIZE);
        memcpy(scriptCommandMod, scriptCommand, strlen(scriptCommand));
        scriptCommandMod = allTrim(scriptCommandMod);

        if (strlen(originalReadConfigScript) + strlen(scriptCommandMod) + CONFIG_SCRIPT_DELIMITER_LEN >= ctr)
        {
          break;
        }

        if(strlen(scriptCommandMod) > 0)
        {
          /* copy the cfgData to the readConfigScript */
          /* NOTE: already checked if enough room in destination buffer, so setting 'n' to size of src string here */
          osapiStrncpy(readConfigScript, scriptCommandMod, strlen(scriptCommandMod));
          osapiStrncat(readConfigScript, CONFIG_SCRIPT_DELIMITER, CONFIG_SCRIPT_DELIMITER_LEN);

          readConfigScript += (strlen(scriptCommandMod) + CONFIG_SCRIPT_DELIMITER_LEN);
        }

        i++;
      }

      if (strlen(originalReadConfigScript) > 0)
      {
        /* call save NVStore routine */
        if (osapiFsWriteNoClose(filedesc, (L7_char8 *)originalReadConfigScript, strlen(originalReadConfigScript)) == L7_ERROR)
        {
          osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof(errMsg), pStrInfo_base_InCreatingFile, configScriptData->cfgHdr.filename);
          setLastError(errMsg);
          osapiFsClose(filedesc);
          /* free the memory */
          osapiFree(L7_CLI_WEB_COMPONENT_ID, originalReadConfigScript);
          osapiFree(L7_CLI_WEB_COMPONENT_ID, scriptCommandModOrig);
          osapiFsDeleteFile(configScriptData->cfgHdr.filename);
          return L7_FAILURE;
        }
      }
      else
      {
        break;
      }
    } /* end of while */

    configScriptData->cfgHdr.dataChanged = L7_FALSE;

    /* free the memory */
    osapiFree(L7_CLI_WEB_COMPONENT_ID, scriptCommandModOrig);
    osapiFree(L7_CLI_WEB_COMPONENT_ID, originalReadConfigScript);

    osapiFsClose(filedesc);

  }
  else
  {
    setLastErrorAddBlanks (1, 0, 0, 0, pStrErr_common_Error, pStrInfo_base_InGettingMemory);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialise the buffer data
*
* @param    L7_char8 *filename
* @param    L7_ConfigScript_t *configScriptData
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t initialiseScriptBuffer(L7_char8 * filename, L7_ConfigScript_t * configScriptData)
{
  /*Set header info*/
  if (filename != L7_NULLPTR)
  {
    OSAPI_STRNCPY_SAFE(configScriptData->cfgHdr.filename,filename);
  }
  configScriptData->cfgHdr.version = 0;
  configScriptData->cfgHdr.componentID = L7_CONFIG_SCRIPT_COMPONENT_ID;
  configScriptData->cfgHdr.type = L7_CFG_DATA;
  configScriptData->cfgHdr.length = 0;
  configScriptData->cfgHdr.dataChanged = L7_FALSE;

  /*set other info*/
  configScriptData->listCount = 0;
  configScriptData->cfgData = L7_NULL;

  configScriptData->size = (sizeof(L7_fileHdr_t) + sizeof(L7_uint32) + 1);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Release the buffer data
*
* @param    L7_ConfigScript_t *configScriptData
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/

L7_RC_t releaseScriptBuffer(L7_ConfigScript_t * configScriptData)
{
  cfgCommand_t * cfgDataTemp = L7_NULL;
  cfgCommand_t * cfgDataNext = L7_NULL;

  if(configScriptData == L7_NULL)
    return L7_FAILURE;

  cfgDataTemp =  configScriptData->cfgData;

  for (; cfgDataTemp != L7_NULL;)
  {
    cfgDataNext =  cfgDataTemp->next;

    /*Free memory*/
    osapiFree(L7_CLI_WEB_COMPONENT_ID, cfgDataTemp);

    cfgDataTemp =  cfgDataNext;
  }

  /*reinitialise buffer*/
  return initialiseScriptBuffer(L7_NULLPTR, configScriptData);
}

/*********************************************************************
* @purpose  Sets the last error
*
* @param    L7_char8* errMsg
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void setLastError(L7_char8 * errMsg)
{

  OSAPI_STRNCPY_SAFE(lastErrorMessage, errMsg);
}

/*********************************************************************
* @purpose  Get the last error
*
* @param    none
*
* @returns  const L7_char8*
*
* @notes    none
*
* @end
*********************************************************************/
const L7_char8 * getLastError()
{
  return lastErrorMessage;
}

/*********************************************************************
* @purpose  Sets the last error to blank
*
* @param    none
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void resetLastError()
{
  OSAPI_STRNCPY_SAFE(lastErrorMessage, L7_EMPTYSTRING);
}

/*********************************************************************
* @purpose  Get the source filename for copy command (upload)
*
* @param    filename      source file name output location
* @param    filenameSize  output buffer size
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t getConfigScriptSourceFileName(L7_char8 * filename, L7_uint32 filenameSize)
{
  osapiStrncpySafe(filename, configScriptSourceFileName, filenameSize);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the source filename for copy command (upload)
*
* @param    filenname     source file name to copy
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t setConfigScriptSourceFileName(const L7_char8 * filename)
{
  OSAPI_STRNCPY_SAFE(configScriptSourceFileName, filename);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  checks that config scripts size is within limit
*
* @param    L7_uint32 newFileSize
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t checkConfigScriptSizeLimit(L7_uint32 newFileSize, L7_char8 * filename)
{
  L7_uint32 rc, num_files, i, size;
  L7_char8 errMsg[128];
  SYSAPI_DIRECTORY_t tree[SYSAPI_MAX_FILE_ENTRIES];

  /*Initialise*/
  num_files = 0;
  rc = L7_FAILURE;

  if(strcmp(filename, TEMP_CONFIG_SCRIPT_FILE_NAME) == 0 || strcmp(filename, TEMP_RUNNINGCONFIG_SCRIPT_FILE_NAME) == 0)
  {
    return L7_SUCCESS;
  }

  if ( sysapiTreeMalloc(tree) == L7_SUCCESS )
  {
    num_files = getConfigScriptList(tree);

    size = newFileSize;

    for(i =0; i < num_files; i++)
    {
      /*get size for all files, except for the one with same file name*/
      if(strcmp(filename, tree[i].filename) != 0 &&
         strcmp(tree[i].filename, TEMP_CONFIG_SCRIPT_FILE_NAME) != 0 &&
         strcmp(tree[i].filename, TEMP_RUNNINGCONFIG_SCRIPT_FILE_NAME) != 0)
      {
        size += tree[i].filesize;
      }
    }

    rc =  size <= CONFIG_SCRIPTS_MAX_SIZE_LIMIT ? L7_SUCCESS : L7_FAILURE;

    if(rc != L7_SUCCESS)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrInfo_base_MaxSizeAllowedForAllCfgScriptVoilatedPleaseDelExistingScriptAndTryAgain);
      setLastError(errMsg);
    }

    sysapiTreeFree(tree);
  }

  return rc;
}

/*********************************************************************
* @purpose  Calculates the size of all scripts present in NVRAM
*
* @param    L7_char8* filename,
* @param    L7_uint32 *filesSize
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t getConfigScriptsSize(L7_char8 * filename, L7_uint32 * filesSize)
{
  L7_uint32 rc, num_files, i;
  SYSAPI_DIRECTORY_t tree[SYSAPI_MAX_FILE_ENTRIES];

  /*Initialise*/
  num_files = 0;
  rc = L7_FAILURE;
  *filesSize = 0;

  if ( sysapiTreeMalloc(tree) == L7_SUCCESS )
  {
    num_files = getConfigScriptList(tree);

    for(i =0; i < num_files; i++)
    {
      if (filename != L7_NULLPTR)
      {
        /*get size for all files, except for the one with same file name*/
        if(strcmp(filename, tree[i].filename) != 0 &&
           strcmp(tree[i].filename, TEMP_CONFIG_SCRIPT_FILE_NAME) != 0 &&
           strcmp(tree[i].filename, TEMP_RUNNINGCONFIG_SCRIPT_FILE_NAME) != 0)
        {
          *filesSize += tree[i].filesize;
        }
      }
    }

    sysapiTreeFree(tree);

    rc =  L7_SUCCESS;
  }

  return rc;
}

/*********************************************************************
* @purpose  checks that config script count is within limit
*
* @param  L7_char8* filename
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t checkConfigScriptCountLimit(L7_char8 * filename)
{
  L7_uint32 rc, num_files, i, ignoreFileCount;
  L7_char8 errMsg[128];
  SYSAPI_DIRECTORY_t tree[SYSAPI_MAX_FILE_ENTRIES];

  /*Initialise*/
  num_files = 0;
  ignoreFileCount = 0;
  rc = L7_FAILURE;

  if(strcmp(filename, TEMP_CONFIG_SCRIPT_FILE_NAME) == 0 || strcmp(filename, TEMP_RUNNINGCONFIG_SCRIPT_FILE_NAME) == 0)
  {
    return L7_SUCCESS;
  }

  if ( sysapiTreeMalloc(tree) == L7_SUCCESS )
  {
    num_files = getConfigScriptList(tree);

    for(i =0; i < num_files; i++)
    {
      /*check if overwriting the file*/
      if(strcmp(filename, tree[i].filename) == 0 ||
         strcmp(tree[i].filename, TEMP_CONFIG_SCRIPT_FILE_NAME) == 0 ||
         strcmp(tree[i].filename, TEMP_RUNNINGCONFIG_SCRIPT_FILE_NAME) == 0)
      {
        ignoreFileCount++;
      }
    }

    num_files -=  ignoreFileCount;

    rc =  num_files < CONFIG_SCRIPT_MAX_COUNT_LIMIT ? L7_SUCCESS : L7_FAILURE;

    if(rc != L7_SUCCESS)
    {
      osapiSnprintfAddBlanks (1, 0, 0, 0, L7_NULLPTR, errMsg, sizeof(errMsg), pStrInfo_base_MaxNumOfAllowedCfgScriptsAlreadyPresentPleaseDelExistingScriptsAndTryAgain);
      setLastError(errMsg);
    }

    sysapiTreeFree(tree);
  }

  return rc;
}

/**************************************************************************
 *
 * @purpose: Creates a text file in the RAM disk from config script
 *
 * @param    cliCfgScriptCopy   @b{(input)} name of file to create
 *
 * @returns  L7_SUCCESS if successful
 * @returns  L7_ERROR if file can not be opened
 *
 * @end
 *
 *************************************************************************/
L7_RC_t cliCfgScriptCopy (L7_char8 * outputFileName)
{
  /* We're only called on one task's thread; use static buffers to save
     stack space */
  L7_ConfigScript_t configScriptData;
  static L7_char8 sourceFilename[L7_MAX_FILENAME];
  static L7_char8 errMsg[256];
  static L7_uchar8 tempConfigScriptBuffer[CONFIG_SCRIPT_MAX_COMMAND_SIZE + 1];
  L7_uint32 inFiledesc = -1;
  L7_uint32 outFiledesc = -1;
  L7_int32 size, charsRead = 0;

  if (getConfigScriptSourceFileName (sourceFilename, sizeof (sourceFilename)) != L7_SUCCESS)
  {
    /* indicate preparation failure */
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof (errMsg), pStrInfo_base_WhileGettingSrcfilenameForTransfer);
    setLastError (errMsg);
    return L7_ERROR;
  }

  if (osapiFsOpen(sourceFilename, &inFiledesc) == L7_ERROR)
  {
    /* indicate preparation failure */
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof (errMsg), pStrInfo_base_WhileOpeningSrcFileForTransfer);
    setLastError (errMsg);
    return L7_ERROR;
  }

  /*remove file */
  osapiFsDeleteFile (outputFileName);

  /*create temp file */
  osapiFsCreateFile (outputFileName);
  if (osapiFsOpen (outputFileName, &outFiledesc) == L7_ERROR)
  {
    /* indicate preparation failure */
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof (errMsg), pStrInfo_base_WhileOpeningOutFileForTransfer);
    setLastError (errMsg);
    osapiFsClose (inFiledesc);
    return L7_ERROR;
  }

  memset (tempConfigScriptBuffer, 0, sizeof (tempConfigScriptBuffer));
  if (osapiFileRead (inFiledesc, (L7_char8 *) tempConfigScriptBuffer, sizeof (L7_fileHdr_t) + 1) !=
      L7_ERROR)
  {
    memcpy (&configScriptData.cfgHdr, tempConfigScriptBuffer, sizeof (L7_fileHdr_t));

    osapiFsFileSizeGet(sourceFilename, &size);

    size -= (sizeof (L7_fileHdr_t) + sizeof (L7_uint32) + 1);

    while (size > 0)
    {
      memset (tempConfigScriptBuffer, 0, sizeof (tempConfigScriptBuffer));
      /*second call to read couter size file */
      if (osapiFileRead (inFiledesc, (L7_char8 *) tempConfigScriptBuffer,
                         CONFIG_SCRIPT_MAX_COMMAND_SIZE) != L7_ERROR)
      {
        charsRead = strlen (tempConfigScriptBuffer);

        if (charsRead == 0)
        {
          break;
        }

        osapiFsWriteNoClose (outFiledesc, (L7_char8 *) tempConfigScriptBuffer, charsRead);
        size -= charsRead;
      }
      else
      {
        osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof (errMsg), pStrInfo_base_InReadingFile, sourceFilename);
        setLastError (errMsg);
        osapiFsClose (inFiledesc);
        osapiFsClose (outFiledesc);
        return L7_ERROR;
      }
    }                           /* end of while */
  }
  else
  {
    osapiSnprintfAddBlanks (1, 0, 0, 0, pStrErr_common_Error, errMsg, sizeof (errMsg), pStrInfo_base_InReadingFile, sourceFilename);
    setLastError (errMsg);
    osapiFsClose (inFiledesc);
    osapiFsClose (outFiledesc);
    return L7_ERROR;
  }

  osapiFsClose (inFiledesc);
  osapiFsClose (outFiledesc);
  return L7_SUCCESS;
}
