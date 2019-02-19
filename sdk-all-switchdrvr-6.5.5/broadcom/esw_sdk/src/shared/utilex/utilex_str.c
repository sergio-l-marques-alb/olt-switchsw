/*
 * $Id: utilex_str.c,v 1.00 Broadcom SDK $
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
 * File:    utilex_str.c
 * Purpose:    Misc. routines used by export/import/show facilities
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/types.h>
#include <shared/util.h>
#include <shared/error.h>
#include <shared/utilex/utilex_str.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SHAREDDNX_UTILEXDNX

void
utilex_str_fill(
  char *str,
  int num,
  char ch)
{
  int i;
  for (i = 0; i < num; i++)
    str[i] = ch;

  str[num] = 0;
}

char **
utilex_str_split(
  char *string,
  char *delim,
  uint32 maxtokens,
  uint32 * realtokens)
{
  char **tokens = NULL;
  int i = 0;
  char *next;

  if (ISEMPTY(string))
    goto exit;

  tokens = sal_alloc(sizeof(char *) * maxtokens, "tokens");
  if (tokens == NULL)
    goto exit;

  memset(tokens, 0, sizeof(char *) * maxtokens);

  for (i = 0; i < maxtokens; i++)
    tokens[i] = NULL;

  i = 0;
  while (((next = strstr(string, delim)) != NULL) && (i < (maxtokens - 1)))
  {
    tokens[i] = sal_alloc(sal_strlen(string) + 1, "token");
    if (tokens[i] == NULL)
      goto exit;
    sal_memset(tokens[i], 0, sal_strlen(string) + 1);
    /*
     * Copy strictly number of characters until next 
     */
    sal_memcpy(tokens[i++], string, next - string);
    /*
     * move string to after the delimiter 
     */
    string = next + sal_strlen(delim);
  }
  /*
   * Once end of line or maxtokens achieved - string will keep the last part 
   */
  tokens[i] = sal_alloc(sal_strlen(string) + 1, "token");
  if (tokens[i] == NULL)
    goto exit;
  sal_memset(tokens[i], 0, sal_strlen(string) + 1);
  strcpy(tokens[i++], string);

exit:
  *realtokens = i;
  return tokens;
}

void
utilex_str_split_free(
  char **tokens,
  uint32 token_num)
{
  int i = 0;

  if (tokens == NULL)
  {
    return;
  }

  for (i = 0; i < token_num; i++)
  {
    if (tokens[i] != NULL)
      sal_free(tokens[i]);
  }

  sal_free(tokens);
  return;
}

void
utilex_str_swap_long(
  uint8 * buffer,
  int uint32_num)
{
  uint32 tmp_value;
  uint8 *tmp_array = (uint8 *) (&tmp_value);
  int i;

  for (i = 0; i < uint32_num; i++)
  {
    tmp_value = *((uint32 *) buffer);
    buffer[0] = tmp_array[3];
    buffer[1] = tmp_array[2];
    buffer[2] = tmp_array[1];
    buffer[3] = tmp_array[0];
    buffer += 4;
  }
  return;
}

void
utilex_str_replace_whitespace(
  char *dest,
  char *source)
{
  int i;
  for (i = 0; i < sal_strlen(source); i++)
  {
    if (source[i] == ' ')
      dest[i] = '_';
    else
      dest[i] = source[i];
  }
  dest[i] = 0;
  return;
}

uint32
utilex_str_stoul(
  char *str,
  uint32 * value_p)
{
  int i;
  char ch;

  for (i = 0; i < strlen(str); i++)
  {
    ch = str[i];
    if (((ch >= '0') && (ch <= '9')) || ((ch >= 'a') && (ch <= 'f')) || ((ch >= 'A') && (ch <= 'F')))
      continue;
    else
      return _SHR_E_PARAM;
  }

  *value_p = _shr_ctoi(str);

  return _SHR_E_NONE;
}

void *
utilex_alloc(
  unsigned int size)
{
  void *mem;
  if ((mem = sal_alloc(size, "utilex")) != NULL)
    sal_memset(mem, 0, size);

  return mem;
}

int
utilex_pow(
  int n,
  int k)
{
  int i, result = 1;
  for (i = 0; i < k; i++)
    result *= n;

  return result;
}
