/* $Id: ui_cli_files.h,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
#ifndef UI_CLI_FILES_INCLUDED
/* { */
#define UI_CLI_FILES_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>

#ifdef __VXWORKS__
  #include <vxWorks.h>
#if !DUNE_BCM
  #include <drv/mem/eeprom.h>
#endif
#endif
#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>


/*
 * When in 'cli input file' mode, count the time and occasionally
 * delay the system (to let watchdog relax. This
 * is a simulation of the user's typing in at a human rate.
 */
#define NUM_10MS_BETWEEN_DELAY    50
#define NUM_TICKS_IN_CLI_DELAY    4
/*
 * Maximal number of ram CLI files supported by this system.
 * Note that each file takes SIZEOF_CLI_FILE bytes of RAM memory.
 */
#define NUM_CLI_FILES             5
/*
 * Maximal number of bytes allowed per one CLI file.
 */
#if LINK_FE600_LIBRARIES
  /* SOC_SAND_FE600 cpu has less memory, and the RBF files are not as big as in Soc_petra,
   * so maximal file size is smaller */
  #define SIZEOF_CLI_FILE           2000000
#else
  #define SIZEOF_CLI_FILE           8000000
#endif

/*
 * Maximal number of characters in name of CLI
 * file (on downloading host and on this system) which
 * contains CLI 'program'.
 * This is a string which must be null terminated.
 */
#define CLI_FILE_NAME_LEN         25
/*****************************************************
*NAME
*  file_via_tftp_send
*TYPE: PROC
*FUNCTION:
*  Send a file through TFTP.
*INPUT:
*  SOC_SAND_DIRECT:
*    char *file_name -
*      Pointer to name of file on this system. This
*      is a null terminated string  whose size
*      must be less than (CLI_FILE_NAME_LEN+1)
*    char * -
*      If an error is encountered then, on output,
*      this program will load pointed memory by
*      a null terminated string with a description
*      of the error.
*      Calling program is supposed to prepare
*      a memry block, for that, of size 320 or more.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non zero then some error has occurred.
*  SOC_SAND_INDIRECT:
*    CLI static structures.
*    err_msg
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
uint32
  file_via_tftp_send(
    uint8   *buffer,
    uint32  buffer_size,
    uint8   *file_name
  );
int
  init_cli_files(
    void
  ) ;
int
  get_num_loaded_cli_files(
    int *num_loaded_ptr
  ) ;
unsigned int
  get_max_num_cli_files(
    void
  ) ;
unsigned int
  get_max_cli_file_size(
    void
  ) ;
unsigned int
  get_max_cli_filename_size(
    void
  ) ;
int
  get_cli_file_loaded(
    int   handle,
    void  *val
  ) ;
int
  get_cli_file_name(
    int   handle,
    void  *val
  ) ;
int
  get_cli_file_mem_base(
    int   handle,
    void  *val
  ) ;
int
  get_cli_file_size(
    int   handle,
    void  *val
  ) ;
int
  get_cli_file_info(
    int   handle,
    char  *description
  ) ;
int
  get_cli_file_system_info(
    char  *description
  ) ;
int
  get_first_free_cli_file_handle(
    int *handle
  ) ;
int
  get_cli_file_name_handle(
    const char *file_name,
          char *err_msg,
          int  *handle
  ) ;

int
  download_cli_file(
    const char *file_name,
          char *err_msg,
  const unsigned long host_ip
  );
int
  load_cli_file_from_memory(
    char *file_name,
    char *inp_mem_base,
    int   inp_mem_size,
    char *err_msg
  );
int
  erase_cli_file(
    const char *file_name,
          char *err_msg
  );

int
  start_cli_file_running(
    int  handle,
    char *err_msg
  );

/* } */
#endif

