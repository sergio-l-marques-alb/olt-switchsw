/* $Id: ui_ram_defi.h,v 1.2 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
#ifndef UI_RAM_DEFI_INCLUDED
/* { */
#define UI_RAM_DEFI_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/****************************************************************
*NAME
*  HISTORY_FIFO
*TYPE: BUFFER
*DATE: 31/JAN/2002
*FUNCTION:
*  STRUCTURE CONTAINING information on
*  the history fifo of the CLI.
*ORGANIZATION:
*  STRUCTURE OF TYPE 'HISTORY_FIFO':
    unsigned int index -
      Next index to load in '*history_array'.
      Range: 0 -> (elements_in_array - 1)
    unsigned int num_loaded -
      Number of meaningful entries in '*history_array'.
      Can be up to 'elements_in_array'. Newest entry
      is pointed by (index - 1) [modulo the size of
      the array].
    unsigned int elements_in_array -
      Number of entries in 'history_array'.
    CURRENT_LINE **history_array -
      Array of pointers to structures of
      type CURRENT_LINE.
*USAGE:
*  See ORGANIZATION.
*REMARKS:
*  None.
*SEE ALSO:
*
****************************************************************/
EXTERN
  HISTORY_FIFO
    History_fifo ;
/****************************************************************
*NAME
*  CURRENT_LINE_PTR
*TYPE: BUFFER
*DATE: 15/JAN/2002
*FUNCTION:
*  POINTER TO STRUCTURE CONTAINING information on
*  the current line being entered using CLI.
*ORGANIZATION:
*  POINTER TO STRUCTURE OF TYPE 'CURRENT_LINE':
*USAGE:
*  T. B. D.
*REMARKS:
*  This structure is updated as the user types in characters
*  on the current CLI line. It is cleared when 'enter' is hit
*  (on a line without errors) or when changing mode
*  (telnet/local terminal)
*SEE ALSO:
*
****************************************************************/
EXTERN
  CURRENT_LINE
    *Current_line_ptr ;
/********************************************************
*NAME
*  Subjects_list
*TYPE: BUFFER
*DATE: 21/JAN/2002
*FUNCTION:
*  Pointer to array containing the  list of
*  subject and related parameters.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'SUBJECT':
*    int   subject_id -
*      Identifier of this subject.
*    char *subj_name -
*      Ascii presentation of subject.
*    PARAM *allowed_params -
*      Array of structures of type PARAM. This is
*      the list of parameters allowed for that
*      subject.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  This pointer initially points to the initial list
*  of subjects in rom. At later stages, it may point to
*  a RAM array, containing subjects added on line.
*SEE ALSO:
*
********************************************************/
EXTERN
  SUBJECT
    *Subjects_list ;
/********************************************************
*NAME
*  Subjects_ascii_list
*TYPE: BUFFER
*DATE: 21/JAN/2002
*FUNCTION:
*  Pointer to ascii array (null terminated)containing
*  the names of all subjects, separated by CR/LF.
*ORGANIZATION:
*  Standard 'C' string.
*USAGE:
*  SEE FUNCTION.
*REMARKS:
*  This pointer initially points to the initial list
*  prepared from subjects in rom (using malloc). If
*  subjects are added on line, memory should be released
*  and reallocated for updated list.
*SEE ALSO:
*
********************************************************/
EXTERN
  char
    *Subjects_ascii_list ;
/********************************************************
*NAME
*  Num_subjects
*TYPE: BUFFER
*DATE: 21/JAN/2002
*FUNCTION:
*  Number of subjects in the system.
*ORGANIZATION:
*  Unsigned int.
*USAGE:
*  SEE FUNCTION.
*REMARKS:
*  This number is initially deduced from the initial list
*  prepared from subjects in rom. If subjects are added
*  on line, this number must be updated.
*SEE ALSO:
*
********************************************************/
EXTERN
  unsigned int
    Num_subjects ;
/********************************************************
*NAME
*  Memory_map
*TYPE: BUFFER
*DATE: 24/FEB/2002
*FUNCTION:
*  Pointer to array containing the  list of
*  memory blocks and related parameters.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'MEMORY_MAP':
*    See ui_pure_defi.h.
*USAGE:
*  See ui_pure_defi.h.
*REMARKS:
*  None.
*SEE ALSO:
*
********************************************************/
EXTERN
  MEMORY_MAP
    *Memory_map ;
/* } */
#endif
