/*! \file dbx_file.h
 * Purpose:    Routines used when working with data IO
 */
/*
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
 */

#ifndef DBX_FILE_H_INCLUDED
#define DBX_FILE_H_INCLUDED

#include <shared/shrextend/shrextend_error.h>
#include <shared/dbx/dbx_xml.h>

typedef enum
{
  DBX_FILE_NONE,                /* No .ext for in filename         */
  DBX_FILE_UKNOWN,              /* There is an extension but not the one known */
  DBX_FILE_CSV,                 /* Comma separated format         */
  DBX_FILE_XML,                 /* XML format                     */
  DBX_FILE_VERILOG,             /* Verilog format                 */
  DBX_FILE_MAX                  /* Must come last                 */
} DBX_FILE_TYPE_E;

/*!
 * \brief Return file type according to extension.
 * String after last dot is compared to predefined set of extensions
 * \par DIRECT INPUT
 *   \param [in] filename - string representing filename to be checked
 * \par DIRECT OUTPUT:
 *   \retval file type as per DBX_FILE_TYPE_E
 */
DBX_FILE_TYPE_E dbx_file_get_type(
  char *filename);

/*!
 * \brief Check directory existence. Return TRUE if exists, FALSE if not
 * \par DIRECT INPUT
 *   \param [in] dir directory path - relative or absolute
 *   \param [in] filename name of the file
 *   \param [in,out] filepath pointer to string where resulting file path should be copied
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE if path was successfully created
 *   \retval _SHR_E_PARAM if there was some problem with parameters
 */
shr_error_e dbx_file_get_path(
  char *dir,
  char *filename,
  char *filepath);

/*!
 * \brief Create full file path based on directory and file name
 * \par DIRECT INPUT
 *   \param [in] dirpath directory path - relative or absolute
 * \par DIRECT OUTPUT:
 *   \retval FALSE if file do not exists
 *   \retval TRUE if file exists
 */
int dbx_file_dir_exists(
  char *dirpath);

/*!
 * \brief Check file existence. Return TRUE if exists, FALSE if not
 * Oriented for xml files only
 * \par DIRECT INPUT
 *   \param [in] filename name of the files which existence should be checked
 * \par DIRECT OUTPUT:
 *   \retval FALSE if file do not exists
 *   \retval TRUE if file exists
 * \remark
 *   If filename has no xml extension, it is added before existence check
 */
int dbx_file_exists(
  char *filename);

/*!
 * \brief Get handle to the top node of XML file
 * \par DIRECT INPUT
 *   \param [in] chip_db directory inside DB which contains files relevant to the chip
 *   \param [in] filename of XML to be opened
 *   \param [in] topname name of top node inside XML file
 *   \param [in] flags options for XML file opening
 * \par DIRECT OUTPUT:
 *   \retval handle to the node if operation succeeds
 *   \retval NULL on failure
 * \remark
 *   Failure may be either file does not exist or top name is wrong
 */
xml_node dbx_file_get_xml_top(
  char *chip_db,
  char *filename,
  char *topname,
  int flags);

#endif /* DBX_FILE_H_INCLUDED */
