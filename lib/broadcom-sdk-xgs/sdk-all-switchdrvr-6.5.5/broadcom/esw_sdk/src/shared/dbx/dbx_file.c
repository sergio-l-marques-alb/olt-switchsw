/*
 * $Id: dbx_file.c,v 1.00 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:    dbx_file.c
 * Purpose:    Misc. routines used by export/import/show facilities
 */

#include <sal/core/libc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>

#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

#if !defined(__KERNEL__)
#include <stdlib.h>

DBX_FILE_TYPE_E
dbx_file_get_type(
  char *filename)
{
  char **tokens;
  uint32 realtokens = 0;
  DBX_FILE_TYPE_E ret = DBX_FILE_MAX;
  char *extension;

  if ((tokens = utilex_str_split(filename, ".", 10, &realtokens)) == NULL)
  {
    LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(0, "Failed to split:%s\n"), filename));
    return ret;
  }

  if ((realtokens == 1) || (realtokens == 0))
  {
    ret = DBX_FILE_NONE;
  }
  else
  {
    extension = tokens[realtokens - 1];
    if (!sal_strcmp(extension, "csv") || !sal_strcmp(extension, "txt"))
      ret = DBX_FILE_CSV;
    else if (!sal_strcmp(extension, "xml"))
      ret = DBX_FILE_XML;
    else if (!sal_strcmp(extension, "v"))
      ret = DBX_FILE_VERILOG;
    else
      ret = DBX_FILE_UKNOWN;
  }

  utilex_str_split_free(tokens, realtokens);
  return ret;
}

shr_error_e
dbx_file_get_path(
  char *dir,
  char *filename,
  char *filepath)
{
  int length = 6;

  if (!ISEMPTY(dir))
    length += sal_strlen(dir);

  if (!ISEMPTY(filename))
    length += sal_strlen(filename);
  else
  {
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(0, "Filename cannot be empty\n")));
    return _SHR_E_PARAM;
  }

  if (length >= RHFILE_MAX_SIZE)
  {
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(0, "Filename length:%d exceeded limit:%d\n"), length, RHFILE_MAX_SIZE));
    return _SHR_E_PARAM;
  }

  if (dir != NULL)
  {
    sal_strcpy(filepath, dir);
    sal_strcat(filepath, "/");
  }
  else  /* Make it NULL string */
    filepath[0] = 0;

  sal_strcat(filepath, filename);
  if (dbx_file_get_type(filename) != DBX_FILE_XML)
  {
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(0, "xml extension added to %s\n"), filename));
    strcat(filepath, ".xml");
  }
  return _SHR_E_NONE;
}

int
dbx_file_dir_exists(
  char *dirpath)
{
  SAL_DIR *dir;

  dir = sal_opendir(dirpath);
  if (!dir)
  {
    /*
     * No such directory
     */
    return FALSE;
  }

  sal_closedir(dir);
  return TRUE;
}

int
dbx_file_exists(
  char *filename)
{
  char filepath[RHFILE_MAX_SIZE];
  FILE *in;

  if (dbx_file_get_path(NULL, filename, filepath) != _SHR_E_NONE)
  {
    return FALSE;
  }

  in = sal_fopen(filepath, "r");
  if (!in)
  {
    /*
     * No such file - not an error - just warning 
     */
    return FALSE;
  }

  sal_fclose(in);
  return TRUE;
}

xml_node
dbx_file_get_xml_top(
  char *descr,
  char *filename,
  char *topname,
  int flags)
{
  void *curTop = NULL;
  char filepath[RHFILE_MAX_SIZE];
  char dir[RHFILE_MAX_SIZE];
  int unit = 0;
  char *db_dir = NULL;

  if (ISEMPTY(filename))
  {
    LOG_ERROR(BSL_LOG_MODULE, (BSL_META_U(unit, "DB Resource not defined\n")));
    goto out;
  }

  db_dir = getenv("DPP_DB_PATH");
  if (ISEMPTY(db_dir) || (dbx_file_dir_exists(db_dir) == FALSE))
  {
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Signal DB was not found in %s\n"), db_dir));
    db_dir = "./db";
  }

  if (!ISEMPTY(db_dir))
  {
    if (dbx_file_dir_exists(db_dir) == FALSE)
    {
      LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Signal DB was not found in %s\n"), db_dir));
    }
  }

  if (sal_strlen(db_dir) > (RHFILE_MAX_SIZE - 5))
  {
    LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "PATH %s is too long\n"), db_dir));
    goto out;
  }

  if (dbx_file_dir_exists(db_dir) == FALSE)
  {
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Signal DB was not found in %s\n"), db_dir));
    goto out;
  }

  strcpy(dir, db_dir);
  if (!ISEMPTY(descr))
  {
    strcat(dir, "/");
    if (sal_strlen(descr) > (RHFILE_MAX_SIZE - (sal_strlen(db_dir) + 5)))
    {
      LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "Directory %s is too long\n"), descr));
      goto out;
    }

    strcat(dir, descr);
  }

  if (dbx_file_dir_exists(dir) == FALSE)
  {
    LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "Device DB was not found in %s\n"), db_dir));
    goto out;
  }

  if (dbx_file_get_path(dir, filename, filepath) != _SHR_E_NONE)
  {
    goto out;
  }

  if ((curTop = dbx_xml_top_get(filepath, topname, flags)) == NULL)
  {
    LOG_WARN(BSL_LOG_MODULE, (BSL_META_U(unit, "Cannot open %s\n"), filepath));
    goto out;
  }

out:
  return curTop;
}
#else
xml_node
dbx_file_get_xml_top(
  char *descr,
  char *filename,
  char *topname,
  int flags)
{
  return NULL;
}
#endif /* !defined(__KERNEL__) */
