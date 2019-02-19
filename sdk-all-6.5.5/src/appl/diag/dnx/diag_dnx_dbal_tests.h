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
 *
 * File:    diag_dnx_dbal_tests.h
 * Purpose: Extern declarations for command functions and
 *          their associated usage strings.
 */

#ifndef DIAG_DNX_DBAL_TESTS_H_INCLUDED
#  define DIAG_DNX_DBAL_TESTS_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#  include <src/soc/dnx/dbal/dbal.h>
#  include <shared/shrextend/shrextend_debug.h>

/*************
 * FUNCTIONS *
 *************/
/*! ****************************************************
* \brief
* load a spesific dbal xml files and cmpare the parsed results to an hard coded expected results.
*
*  \par DIRECT INPUT:
*    \param [in] unit
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
cmd_result_t diag_dbal_test_xml_parsing(
  int unit);

/*! ****************************************************
* \brief
* a basic logical tests for a given logical table
* the test insert entries to the table, retrieve them and compare the retrieved entry to the expected one
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] table_id - \n
*       table_id
*    \param [in] mode - \n
*       test mode (0-basic, 1-advance)
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
cmd_result_t diag_dbal_test_logical_table(
  int unit,
  dbal_tables_e table_id,
  int mode);

/*! ****************************************************
* \brief
* group of dbal usage examples, each mode is a different example
*
*  \par DIRECT INPUT:
*    \param [in] unit
*    \param [in] mode - \n
*       test mode
*  \par DIRECT OUTPUT:
*    cmd_result_t - \n
*      result cmd
*****************************************************/
cmd_result_t dbal_jer2_example(
  int unit,
  int mode);

#endif /*  DIAG_DNX_DBAL_TESTS_H_INCLUDED */
