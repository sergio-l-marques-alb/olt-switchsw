/* $Id: sand_code_hamming.h,v 1.4 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_sand_code_hamming.h
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_SAND_CODE_HAMMING_H_INCLUDED__
/* { */
#define __SOC_SAND_CODE_HAMMING_H_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_framework.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */


/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
 * See details in soc_sand_code_hamming.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_code_hamming_check_data_no_check(
    SOC_SAND_INOUT uint32  data[],
    SOC_SAND_IN    uint32  data_bit_wide,
    SOC_SAND_IN    uint32  data_nof_longs,
    SOC_SAND_IN    uint32  rotated_generation_matrix[],
    SOC_SAND_IN    uint32  generation_matrix_bit_search[],
    SOC_SAND_IN    uint32  p_bit_wide,
    SOC_SAND_IN    uint32  e,
    SOC_SAND_INOUT uint32  work_data[],
    SOC_SAND_OUT   uint32* number_of_fixed_errors
  );

/*****************************************************
 * See details in soc_sand_code_hamming.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_code_hamming_check_data(
    SOC_SAND_INOUT uint32  data[],
    SOC_SAND_IN    uint32  data_bit_wide,
    SOC_SAND_IN    uint32  rotated_generation_matrix[],
    SOC_SAND_IN    uint32  generation_matrix_bit_search[],
    SOC_SAND_IN    uint32  p_bit_wide,
    SOC_SAND_IN    uint32  e,
    SOC_SAND_OUT   uint32* number_of_fixed_errors
  );

/*****************************************************
 * See details in soc_sand_code_hamming.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_code_hamming_get_p(
    SOC_SAND_IN  uint32  data[],
    SOC_SAND_IN  uint32  data_bit_wide,
    SOC_SAND_IN  uint32  generation_matrix[],
    SOC_SAND_IN  uint32  m,
    SOC_SAND_OUT uint32* s
  );

/*****************************************************
 * See details in soc_sand_code_hamming.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_code_hamming_get_p_no_check(
    SOC_SAND_IN    uint32  data[],
    SOC_SAND_IN    uint32  data_bit_wide,
    SOC_SAND_IN    uint32  data_nof_longs,
    SOC_SAND_IN    uint32  generation_matrix[],
    SOC_SAND_IN    uint32  p,
    SOC_SAND_INOUT uint32  work_data[],
    SOC_SAND_OUT   uint32* s
  );

/*****************************************************
 * See details in soc_sand_code_hamming.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_code_hamming_get_p_bit_wide(
    SOC_SAND_IN  uint32  data_bit_wide,
    SOC_SAND_OUT uint32* p
  );


/*****************************************************
 * See details in soc_sand_code_hamming.h
 *****************************************************/
SOC_SAND_RET
  soc_sand_code_hamming_generate_gen_matrix(
    SOC_SAND_IN  uint32 data_bit_wide,
    SOC_SAND_IN  uint32 search_gen_mat_nof_entires,
    SOC_SAND_OUT uint32 gen_mat[],
    SOC_SAND_OUT uint32 search_gen_mat[]
  );

/* } */


#if defined (SOC_SAND_DEBUG)
/* { */


/*****************************************************
 * See details in soc_sand_code_hamming.h
 *****************************************************/
uint32
  soc_sand_code_hamming_TEST(
    SOC_SAND_IN uint32 silent
  );

/*****************************************************
 * See details in soc_sand_code_hamming.h
 *****************************************************/
void
  soc_sand_code_hamming_print_gen_matrix(
    SOC_SAND_IN uint32 data_bit_wide
  );


/* } */
#endif

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_SAND_CODE_HAMMING_H_INCLUDED__*/
#endif
