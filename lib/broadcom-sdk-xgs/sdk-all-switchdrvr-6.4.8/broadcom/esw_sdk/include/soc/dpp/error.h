/* $Id:  $
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

#ifndef __DPP_ERROR_H__
#define __DPP_ERROR_H__

#include <soc/error.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

/*
 *   Function
 *      translate_sand_success_failure
 *   Purpose
 *      Translate a SOC_SAND_SUCCESS_FAILURE into a Broadcom result code
 *   Parameters
 *      (in)  result = SOC_SAND result to translate
 *   Returns
 *      soc_error_t = Closest reasonable Broadcom result
 *   Notes
 *      If there is no reasonably close error, it returns
 *      SOC_E_FAIL instead of something more specific.
 */
int
translate_sand_success_failure(const SOC_SAND_SUCCESS_FAILURE result);

/*
 *   Function
 *      translate_sand_result
 *   Purpose
 *      Translate soc_sand eror code into a Broadcom result code, and print
 *      (under debug flags) soc_sand erorr information.
 *   Parameters
 *      (in)  result = SOC_SAND error code to translate (as returned from soc_sand function
 *   Returns
 *       SOC_E_NONE soc_sand function returned no error
 *       SOC_E_INTERNAL soc_sand function returned an error
 *   Notes
 *      None.
 */
int
handle_sand_result(uint32 soc_sand_result);

#define SOC_SAND_IF_ERR_RETURN(_sand_ret) \
    do { \
        soc_error_t _rv = handle_sand_result(_sand_ret); \
        if (_rv != SOC_E_NONE) { \
            return _rv; \
        } \
    } while (0)

#define SOC_SAND_IF_ERR_EXIT(_sand_ret) \
    do { \
        soc_error_t _rv = handle_sand_result(_sand_ret); \
        if (_rv != SOC_E_NONE) { \
            goto exit; \
        } \
    } while (0)


#define SOC_SAND_IF_FAIL_RETURN(_sand_success) \
    do { \
        _rv = translate_sand_success_failure(_sand_success); \
        if (_rv != SOC_E_NONE) { \
            return _rv; \
        } \
    } while (0)


#define SOC_SAND_IF_FAIL_EXIT(_sand_success) \
    do { \
        _rv = translate_sand_success_failure(_sand_success); \
        if (_rv != SOC_E_NONE) { \
            goto exit; \
        } \
    } while (0)


#define SOC_SAND_IF_ERR_OR_FAIL_RETURN(_sand_ret, _sand_success) \
    do { \
        SOC_SAND_IF_ERR_RETURN(_sand_ret); \
        SOC_SAND_IF_FAIL_RETURN(_sand_success); \
    } while (0)

#define SOC_SAND_FAILURE(_sand_ret) \
    ((handle_sand_result(_sand_ret)) < 0)
        
#define SOC_SAND_IF_ERROR_RETURN(_sand_ret)                                                \
    do {                                                                               \
        soc_error_t _rv = handle_sand_result(_sand_ret);                               \
        if (_rv != SOC_E_NONE) {                                                       \
	        _bsl_warn(_BSL_SOCDNX_MSG("%d"),_rv);                                       \
            return _rv;                                                                \
        }                                                                              \
    } while (0)

#endif /* __DPP_ERROR_H__ */
