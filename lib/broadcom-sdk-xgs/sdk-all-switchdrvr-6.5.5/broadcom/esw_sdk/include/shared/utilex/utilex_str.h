/*! \file utilex_str.h
 * Purpose:    Misc. routines for string/buffer handling
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

#ifndef UTILEX_STR_H_INCLUDED
#define UTILEX_STR_H_INCLUDED

#include <sal/core/alloc.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/error.h>

/* Misc defines */
#define  RHFILE_MAX_SIZE      128
#define  RHNAME_MAX_SIZE      64
#define  RHKEYWORD_MAX_SIZE   12

/*!
 * \brief Check the pointer and the content. If it is not NULL and the not empty return true
 * \par DIRECT INPUT
 *   \param [in] name_macro pointer to the string to be checked
 */
#define ISEMPTY(name_macro)     ((name_macro == NULL) || (name_macro[0] == 0))

/*!
 * \brief Make string empty one
 * \par DIRECT INPUT
 *   \param [in] name_macro pointer to the string to be empty
 */
#define SET_EMPTY(name_macro)   (name_macro[0] = 0)

/*!
 * \brief Return uint32 value hidden in the string
 * \par DIRECT INPUT
 *   \param [in] value_string pointer to the uint32 value
 */
#define VALUE(value_string)     (*((uint32 *)value_string))

/*!
 * \brief Fill certain number of characters in string with character
 * \par DIRECT INPUT
 *   \param [in,out] string pointer to the start of string to be filled
 *   \param [in] num number of bytes to be filled
 *   \param [in] ch character that will be put into the string
 */
void utilex_str_fill(
  char *string,
  int num,
  char ch);

/*!
 * \brief Split input string into tokens - multiple strings separated by certain character
 * \par DIRECT INPUT
 * \param [in] string input string for splitting
 * \param [in] delim  delimiter character for splitting
 * \param [in] maxtokens maximum number of expected token
 * \param [in,out] realtokens  pointer to the variable that discovered count of tokens will be filled by
 * \par DIRECT OUTPUT
 *   \retval pointer to string array containing list of tokens
 * \par INDIRECT OUTPUT
 *   *realtokens - number of tokens found in the string
 * \remark
 *   String array should be freed once not needed through utilex_str_split_free
 */
char **utilex_str_split(
  char *string,
  char *delim,
  uint32 maxtokens,
  uint32 * realtokens);

/*!
 * \brief Free array of tokens allocated by utilex_str_split
 * \par DIRECT INPUT
 *   \param [in] tokens pointer to token array
 *   \param [in] token_num delim  delimiter character for splitting
 */
void utilex_str_split_free(
  char **tokens,
  uint32 token_num);

/*!
 * \brief Swap bytes inside uint32
 * \par DIRECT INPUT
 *   \param [in] buffer pointer to buffer
 *   \param [in] uint32_num number of words to be swapped
 * \par INDIRECT OUTPUT
 *   *buffer buffer content will present bytes in reversed order
 */
void utilex_str_swap_long(
  uint8 * buffer,
  int uint32_num);

/*!
 * \brief Replace white space by underscore in source and put it in destination
 * \par DIRECT INPUT
 *   \param [in] dest buffer where result will be placed
 *   \param [in] source pointer to string with white spaces
 * \par INDIRECT OUTPUT
 *   *dest  buffer where result of replace operation will be placed
 */
void utilex_str_replace_whitespace(
  char *dest,
  char *source);

/*!
 * \brief Convert string to unsigned int
 * \par DIRECT INPUT
 *   \param [in] str pointer to the string to be converted
 *   \param [in] value_p pointer to the value
 * \par INDIRECT OUTPUT
 *   *value_p value obtained from the string
 */
uint32 utilex_str_stoul(
  char *str,
  uint32 * value_p);

void *utilex_alloc(
  unsigned int size);

int utilex_pow(
  int n,
  int k);

#endif /* UTILEX_STR_H_INCLUDED */
