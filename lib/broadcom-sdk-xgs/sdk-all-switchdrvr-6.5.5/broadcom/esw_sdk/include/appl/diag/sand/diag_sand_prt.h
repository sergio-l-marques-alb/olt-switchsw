/*! \file diag_sand_prt.h
 *
 * Set of macros and routines to print beautified tabular output
 *
 * Main purpose to provide standard output format for all tabular data in bcm shell
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

#ifndef DIAG_SAND_PRT_H
#define DIAG_SAND_PRT_H

#if !defined(__KERNEL__)
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#endif

#include <shared/utilex/utilex_rhlist.h>
#include <shared/utilex/utilex_str.h>
#include <shared/shrextend/shrextend_debug.h>

typedef struct
{
  uint32 width;
  char format[RHKEYWORD_MAX_SIZE];
} prt_column_t;

/*!
 * \brief Actual printing of prepared data
 *
 * \par DIRECT INPUT:
 *   \param prt_list pointer to the list of rows, constituting the table
 *   \param column_num number of columns in the table
 * \par INDIRECT OUTPUT:
 *   printing or formatted output to the standard output
 * \remark
 *   automatically frees the list
 */
void diag_sand_prt(
  rhlist_t * prt_list,
  int column_num);

/* Modes for list entries */
typedef enum {
  PRT_ROW_SEP_NONE,
  PRT_ROW_SEP_UNDERSCORE,
  PRT_ROW_SEP_EQUAL
} PRT_ROW_SEP_MODE;

/* Facilities for pretty print */
#define PRT_COLUMN_WIDTH    (RHNAME_MAX_SIZE + 1)
#define PRT_COLUMN_MAX_NUM     24

/*!
 * \brief Initialize variables used by pretty printing
 *
 * \par INDIRECT INPUT:
 *   PRT_INIT_ROW_VARS - initialization of per row local variables
 * \par INDIRECT OUTPUT:
 *   char *prt_row_offset - offset(pointer) of next available cell in the row
 *   void *prt_row_start  - pointer to beginning of the row
 */
#define PRT_INIT_ROW_VARS            char *prt_row_offset = NULL;                                                      \
                                     void *prt_row_start = NULL;                                                       \

/*!
 * \brief Initialize variables used by pretty printing
 *
 * \par INDIRECT INPUT:
 *   PRT_INIT_ROW_VARS - initialization of per row local variables
 * \par INDIRECT OUTPUT:
 *   rhlist_t *prt_list - pointer to linked list of rows
 *   int prt_column_num - number of columns in the output
 */
#define PRT_INIT_VARS                PRT_INIT_ROW_VARS                                                                 \
                                     rhlist_t *prt_list = NULL;                                                        \
                                     int prt_column_num = 0

/*!
 * \brief Free the list
 *
 * \par INDIRECT INPUT:
 *   prt_list       - pointer to the list of rows, constituting the table
 * \remark
 *   used when leaving the routine after at least one PRT_ROW_ADD but without invocation of PRT_COMMIT
 */
#define PRT_FREE                     utilex_rhlist_free_all(prt_list);

/*!
 * \brief Actual printing of prepared data
 *
 * \par INDIRECT INPUT:
 *   prt_list       - pointer to the list of rows, constituting the table
 *   prt_column_num - number of columns in the table
 * \par INDIRECT OUTPUT:
 *   printing or formatted output to the standard output
 * \remark
 *   automatically frees the list
 */
#define PRT_COMMIT                   diag_sand_prt(prt_list, prt_column_num);

/*!
 * \brief Fill cell with formatted data
 *
 * \par  DIRECT INPUT
 *   \param [in] mc_format - printf style format to al
 * \par  INDIRECT INPUT
 *   prt_column_num  - number of columns in the table
 *   prt_row_offset - current place in the row to be written (cell pointer)
 *   prt_row_start  - pointer to the beginning of row
 * \par  INDIRECT OUTPUT
 *   prt_row_offset - offset is updated, so to reflect that cell was utilized
 * \remark
 *  MACRO verifies the number of cells in the row has not groen bigger than column number
 */
#define _PRT_CELL_SET(mc_format,...) { if(prt_column_num && (prt_row_offset >= ((char *)prt_row_start +                \
                                                            sizeof(rhentry_t) + prt_column_num * PRT_COLUMN_WIDTH))) { \
                                         SHR_ERR_EXIT(_SHR_E_INTERNAL,                                                 \
                                            "Cell number in row exceeded column number:%d\n", prt_column_num);         \
                                       }                                                                               \
                                       else {                                                                          \
                                         snprintf(prt_row_offset, PRT_COLUMN_WIDTH, mc_format "%s", __VA_ARGS__);      \
                                         prt_row_offset[PRT_COLUMN_WIDTH - 1] = 0;                                     \
                                         prt_row_offset += PRT_COLUMN_WIDTH;                                           \
                                       }                                                                               \
                                     }
#define PRT_CELL_SET(...)            _PRT_CELL_SET(__VA_ARGS__, "")

/*!
 * \brief When filling table allows to skip cell
 *
 * \par  DIRECT INPUT
 *   \param [in] mc_skip_num number of cells to be skipped
 * \par  INDIRECT INPUT
 *   prt_column_num  - number of columns in the table
 *   prt_row_offset - current place in the row to be written (cell pointer)
 *   prt_row_start  - pointer to the beginning of row
 * \par  INDIRECT OUTPUT
 *   prt_row_offset - offset is updated, so to reflect that cell was utilized
 * \remark
 *  MACRO verifies the number of cells in the row has not groen bigger than column number
 */
#define PRT_CELL_SKIP(mc_skip_num)   { if(prt_column_num && (prt_row_offset >= ((char *)prt_row_start +                \
                                                            sizeof(rhentry_t) + prt_column_num * PRT_COLUMN_WIDTH))) { \
                                         SHR_ERR_EXIT(_SHR_E_INTERNAL,                                                 \
                                            "Cell number in row exceeded column number:%d\n", prt_column_num);         \
                                       }                                                                               \
                                       else {                                                                          \
                                         prt_row_offset += (mc_skip_num) * (PRT_COLUMN_WIDTH);                         \
                                       }                                                                               \
                                     }

/*!
 * \brief Return the number of cells being set in the current row
 *
 *  Used to derive number of columns in the table after compliting header row
 *
 * \par  INDIRECT INPUT
 *   prt_row_offset - current place in the row to be written (cell pointer)
 *   prt_row_start  - pointer to the beginning of row
 * \par  DIRECT OUTPUT
 *   \retval - number of columns
 */
#define PRT_COLUMN_GET               (prt_row_offset - ((char *)prt_row_start + sizeof(rhentry_t)))/PRT_COLUMN_WIDTH;

/*!
 * \brief Return the number of cells being set in the current row
 *
 *  Used to derive number of columns in the table after compliting header row
 *
 * \par  INDIRECT INPUT
 *   PRT_COLUMN_GET - macro allowing to obtain current offset
 * \par  INDIRECT OUTPUT
 *   prt_column_num - number of columns usually assigned after header row
 */
#define PRT_COLUMN_SET               { if(prt_column_num != 0) {                                                       \
                                         SHR_ERR_EXIT(_SHR_E_INTERNAL, "Column number was already set to:%d\n",        \
                                                prt_column_num);                                                       \
                                       }                                                                               \
                                       prt_column_num = PRT_COLUMN_GET;                                                \
                                       if(prt_column_num >= PRT_COLUMN_MAX_NUM) {                                      \
                                         SHR_ERR_EXIT(_SHR_E_INTERNAL, "Column number:%d exceeded maximum:%d\n",       \
                                                prt_column_num, PRT_COLUMN_MAX_NUM);                                   \
                                       }                                                                               \
                                     }

/*!
 * \brief Add new row in the table
 *
 * \par  DIRECT INPUT
 *   \param [in] mc_mode Points to the type of separator from previous line. See PRT_ROW_SEP_MODE
 * \par  INDIRECT INPUT
 *   prt_list        - pointer to the list of rows, that this one will be added to
 *   prt_row_offset - current place in the row to be written (cell pointer)
 *   prt_row_start  - pointer to the beginning of row
 * \par  INDIRECT OUTPUT
 *   prt_list        - new row will be added to this list
 *   prt_row_start   - pointer to the beginning of new row
 *   prt_row_offset  - set to the beginning of new row skipping control info
 * \remark
 *  First row filled followed by ROW_COLUMN_SET is considered of being header one
 */
#define PRT_ROW_ADD(mc_mode)         { if(prt_list == NULL) {                                                          \
                                         if((prt_list = utilex_rhlist_create("prt_print",                              \
                                               sizeof(rhentry_t) + PRT_COLUMN_WIDTH * PRT_COLUMN_MAX_NUM, 0)) == NULL) \
                                           SHR_ERR_EXIT(_SHR_E_MEMORY,"prt_list create %s", "failed\n");               \
                                       }                                                                               \
                                       if(utilex_rhlist_entry_add_tail(prt_list,                                       \
                                               NULL, RHID_TO_BE_GENERATED, &prt_row_start) == _SHR_E_MEMORY) {         \
                                           PRT_FREE;                                                                   \
                                           SHR_ERR_EXIT(_SHR_E_MEMORY,"add entry to prt_list  %s", "failed\n");        \
                                       }                                                                               \
                                       ((rhentry_t *)prt_row_start)->mode = mc_mode;                                   \
                                       prt_row_offset = (char *)prt_row_start + sizeof(rhentry_t);                     \
                                     }
#endif /* DIAG_SAND_PRT_H */
