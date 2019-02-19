/*
 * $Id: diag_sand_prt.c,v 1.00 Broadcom SDK $
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
 * File:    diag_sand_prt.c
 * Purpose:    Beautified cli output routine
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>

#include <appl/diag/sand/diag_sand_prt.h>

static void
diag_sand_prt_row(
  int num,
  char ch)
{
  while (num--)
    cli_out("%c", ch);
  cli_out("\n");
  return;
}

void
diag_sand_prt(
  rhlist_t * prt_list,
  int column_num)
{
  int i, k, n;

  prt_column_t *column;
  int column_width;
  int total_width = 1;
  rhhandle_t prt_row_start;
  char *prt_row_offset;
  rhentry_t *entry;
  int row_num;

  if (prt_list == NULL)
  {
    cli_out("Null List\n");
    return;
  }

  if (column_num == 0)
  {
    cli_out("No columns\n");
    PRT_FREE;
    return;
  }

  row_num = RHLNUM(prt_list);
  column = sal_alloc(sizeof(prt_column_t) * column_num, "column format");
  memset(column, 0, column_num * sizeof(prt_column_t));

  RHITERATOR(prt_row_start, prt_list)
  {
    prt_row_offset = ((char *) prt_row_start) + sizeof(rhentry_t);
    for (k = 0; k < column_num; k++)
    {
      column_width = sal_strlen(prt_row_offset);
      /*
       * remove trailing white spaces - keep just 4 of them max 
       */
      for (n = column_width - 1; n > 4; n--)
        if (prt_row_offset[n] == ' ')
          prt_row_offset[n] = 0;
        else
          break;
      if (sal_strlen(prt_row_offset) > column[k].width)
        column[k].width = sal_strlen(prt_row_offset);
      prt_row_offset += PRT_COLUMN_WIDTH;
    }
  }

  for (k = 0; k < column_num; k++)
  {
    /*
     * aggregate total row size
     */
    if (column[k].width == 0)
    {
      /*
       * Just add | without spaces - it is intended for special separation 
       */
      total_width += column[k].width + 1;       /* for | between fields and spaces before and after */
      /*
       * create format string for each column; 
       */
      sprintf(column[k].format, "|%ss", "%");
    }
    else
    {
      total_width += column[k].width + 3;       /* for | between fields and spaces before and after */
      /*
       * create format string for each column; 
       */
      sprintf(column[k].format, "| %s-%ds", "%", column[k].width + 1);
    }
  }

  /*
   * Print Header - row with index 0
   */
  diag_sand_prt_row(total_width, '=');

  i = 0;
  RHITERATOR(prt_row_start, prt_list)
  {
    entry = (rhentry_t *) prt_row_start;
    prt_row_offset = ((char *) prt_row_start) + sizeof(rhentry_t);
    for (k = 0; k < column_num; k++)
    {
      cli_out(column[k].format, prt_row_offset);
      prt_row_offset += PRT_COLUMN_WIDTH;
    }
    cli_out("|\n");
    if ((i == row_num - 1) || (entry->mode == PRT_ROW_SEP_EQUAL))
      diag_sand_prt_row(total_width, '=');
    else if (entry->mode == PRT_ROW_SEP_UNDERSCORE)
    {
      diag_sand_prt_row(total_width, '-');
    }
    i++;
  }

  sal_free(column);
  PRT_FREE;
  return;
}
