/*! \file shrextend_debug.h
 *
 * Generic macros for tracing function call trees.
 *
 * The main principle is to establish a single point of exit for each
 * function, and then combine this with a standard method of logging
 * error conditions.
 *
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

#ifndef _SHREXTEND_DEBUG_H_INCLUDED
/* { */
#define _SHREXTEND_DEBUG_H_INCLUDED

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_error.h>
#include <sal/core/alloc.h>

/*
 * DNX layer/source log macros.
 * Convert from DNX (JR2) system to classic JR1 system
 * {
 */
/*
 * If NO_LOG_INFO_AND_DOWN is set to a non zero value then the
 * following macros are empty and do not print (or even calculate
 * their variables). This may be useful when the calculation of the
 * variables, for the formatting string, are time consuming.
 */
#define NO_LOG_INFO_AND_DOWN 0

#define LOG_FATAL_EX(ls_, stuff_)          LOG_FATAL(ls_, stuff_)
#define LOG_ERROR_EX(ls_, stuff_)          LOG_ERROR(ls_, stuff_)
#define LOG_WARN_EX(ls_, stuff_)           LOG_WARN(ls_, stuff_)
#if !NO_LOG_INFO_AND_DOWN
/* { */
#define LOG_INFO_EX(ls_, stuff_)           LOG_INFO(ls_, stuff_)
#define LOG_VERBOSE_EX(ls_, stuff_)        LOG_VERBOSE(ls_, stuff_)
/*
 * This macro prints input message (stuff_) plus 'prefix' of meta data
 * such as 'procedure name' or 'line no.' provided the severity level
 * for the specified 'layer'/'source' between 'bslSeverityDebug' and
 * 'bslSeverityFatal'.
 */
#define LOG_DEBUG_EX(ls_, stuff_)           \
  {                                         \
    bsl_severity_t sev_min, sev_max ;       \
    bsldnx_get_prefix_range_min_max(&sev_min,&sev_max) ; \
    bsldnx_set_prefix_range_min_max(bslSeverityOff + 1,bslSeverityDebug) ; \
    LOG_DEBUG(ls_, stuff_) ;                \
    bsldnx_set_prefix_range_min_max(sev_min,sev_max) ;  \
  }
/* } */
#else
/* { */
#define LOG_INFO_EX(ls_, stuff_)
#define LOG_VERBOSE_EX(ls_, stuff_)
#define LOG_DEBUG_EX(ls_, stuff_)
/* } */
#endif
/*
 * }
 */
/*
 * This macro is an empty string which is used to fill in the required four
 * parameters for the various macros below.
 */
#define EMPTY                              ""

/*!
 * \brief Function entry declarations.
 *
 * This macro must appear in each function right after the local
 * variable declarations.
 *
 * Example:
 *
 * \code{.c}
 * int my_func(int unit, int index, int value)
 * {
 *   int local_idx;
 *
 *   SHR_FUNC_INIT_VARS(unit) ;
 *   SHR_FUNC_ENTER("index %d, value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *   ...
 * }
 * \endcode
 *
 * \remark
 *  Indirect input:
 *    Standard error values (Specifically SHR_E_NONE)
 */
#define SHR_FUNC_INIT_VARS(_unit) \
int _func_unit = _unit ; \
int _func_rv = SHR_E_NONE


/*!
 * \brief Log function entry and selected input variables.
 *
 * This macro must appear in each function right after the local
 * variable declarations and SHR_FUNC_INIT_VARS.
 *
 * Example:
 *
 * \code{.c}
 * int my_func(int unit, int index, int value)
 * {
 *   int local_idx;
 *
 *   SHR_FUNC_INIT_VARS(unit) ;
 *   SHR_FUNC_ENTER("index %d, value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *   ...
 * }
 * \endcode
 *
 * \param [in] _unit Switch unit number.
 * \param [in] _formatting_string to display if print is not filtered. MUST be encapsulated in double quotes and have FOUR entries.
 * \param [in] _params Four parameters to display via the formatting string
 * \remark
 *  Indirect input:
 *    BSL_LOG_MODULE       - Module (layer/source) to report on. Defined at head of file.
 *    LOG_WARN             - Standard BSL (JR1) LOG macro
 *    _func_unit           - Value of 'unit' input parameter at entry
 */
#define SHR_FUNC_ENTER(_formatting_string,_param1,_param2,_param3,_param4) \
LOG_DEBUG_EX(BSL_LOG_MODULE, \
  (BSL_META_U(_func_unit," Enter. " _formatting_string),_param1,_param2,_param3,_param4)  ) ;

/*!
 * \brief Single point of exit code.
 *
 * This macro must appear at the very bottom of each function, and it
 * must be preceded an 'exit' label and optionally some resource
 * clean-up code (if any).
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit, int index, int value)
 * {
 *   int local_idx;
 *
 *   SHR_FUNC_INIT_VARS(unit) ;
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *   ...
 *
 * exit:
 *   SHR_FUNC_EXIT("local_idx %d %s%s%s\r\n",local_idx ,EMPTY,EMPTY,EMPTY) ;
 * }
 * \endcode
 * \param [in] _formatting_string to display if print is not filtered. MUST be encapsulated in double quotes and have FOUR entries.
 * \param [in] _params Four parameters to display via the formatting string
 * \remark
 *  Indirect input:
 *    BSL_LOG_MODULE       - Module (layer/source) to report on. Defined at head of file.
 *    LOG_WARN             - Standard BSL (JR1) LOG macro
 *    _func_unit           - Value of 'unit' input parameter at entry
 */
#define SHR_FUNC_EXIT(_formatting_string,_param1,_param2,_param3,_param4) \
LOG_DEBUG_EX(BSL_LOG_MODULE  , \
  (BSL_META_U(_func_unit," Exit. " _formatting_string),_param1,_param2,_param3,_param4 )) ; \
  return _func_rv
/*!
 * \brief Goto single point of exit.
 *
 * Go to label 'exit', which must be defined explicitly in each
 * function.
 *
 * This macro is normally not called directly, but it is used as part
 * of other error handling macros.
 */
#define SHR_EXIT() goto exit
/*!
 * \brief Error-exit on expression error with extended user log info.
 *
 * Evaluate an expression and if it evaluates to a standard error
 * code, then log an extended error message (with user-added info) and go
 * to the function's single point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log layer/source.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit, int index, int value)
 * {
 *   int local_idx, var1, var2;
 *
 *   SHR_FUNC_INIT_VARS(unit) ;
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *
 *   SHR_IF_ERR_EXIT_WITH_LOG(some_other_func(unit)," var1 %d var2 %s %s\r\n",var1,var2,EMPTY ) ;
 *   ...
 *
 * exit:
 *   SHR_FUNC_EXIT("local_idx %d %s%s%s\r\n",local_idx ,EMPTY,EMPTY,EMPTY);
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate. This is also _param1 of the formatting string
 * \param [in] _formatting_string to display if print is not filtered. MUST be encapsulated in double quotes.
 * \param [in] _params Extra three parameters (_param2,_param3,_param4) to display via the formatting string. Param1 is _expr above.
 * \remark
 *  Indirect input:
 *    BSL_LOG_MODULE       - Module (layer/source) to report on. Defined at head of file.
 *    LOG_ERROR            - Standard BSL (JR1) LOG macro
 *    _func_unit           - Value of 'unit' input parameter at entry
 */
#define SHR_IF_ERR_EXIT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4) \
{ \
  int _rv = _expr; \
  if (SHR_FAILURE(_rv)) \
  { \
    LOG_ERROR_EX(BSL_LOG_MODULE, \
      (BSL_META_U(_func_unit," Error: %s ; " _formatting_string), \
      shrextend_errmsg_get(_rv) ,_param2,_param3,_param4)) ; \
    _func_rv = _rv ; \
    SHR_EXIT() ; \
  } \
}
/*!
 * \brief Error-exit on expression error without user extended log.
 *
 * Evaluate an expression and if it evaluates to a standard error
 * code, then log a standard (fixed formatting string) error message and
 * go to the function's single point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log layer/source.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit, int index, int value)
 * {
 *   int local_idx;
 *
 *   SHR_FUNC_INIT_VARS(unit) ;
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *
 *   SHR_IF_ERR_EXIT(some_other_func(unit)) ;
 *   ...
 *
 * exit:
 *   SHR_FUNC_EXIT("local_idx %d\r\n",local_idx ,EMPTY,EMPTY,EMPTY);
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate. This is also _param1 of the formatting string
 * \remark
 *  Indirect input:
 *    BSL_LOG_MODULE            - Module (layer/source) to report on. Defined at head of file.
 *    SHR_IF_ERR_EXIT_WITH_LOG  - Used, with degenerated input, for implementation of this macro.
 *    _func_unit                - Value of 'unit' input parameter at entry
 */
#define SHR_IF_ERR_EXIT(_expr) \
{ \
  SHR_IF_ERR_EXIT_WITH_LOG(_expr,"%s%s%s",EMPTY,EMPTY,EMPTY) ; \
}
/*!
 * \brief Error-exit on verification procedure fail without user extended log.
 *
 * Evaluate an expression which is the result of invoking a verification procedure. If it evaluates to
 * a standard error code, then log a standard (fixed formatting string) error message and go to the
 * function's single point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log layer/source.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit, int index, int value)
 * {
 *   int local_idx ;
 *
 *   SHR_FUNC_INIT_VARS(unit) ;
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *   BCM_INVOKE_VERIFY_DNX(verification_procedure(unit,index,value)) ;
 *   ...
 *
 * exit:
 *   SHR_FUNC_EXIT("local_idx %d %s%s%s\r\n",local_idx ,EMPTY,EMPTY,EMPTY);
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate. This is also _param1 of the formatting string
 * \remark
 *  Indirect input:
 *    BSL_LOG_MODULE       - Module (layer/source) to report on. Defined at head of file.
 *    SHR_IF_ERR_EXIT      - Used, with degenerated input, for implementation of this macro.
 *    _func_unit           - Value of 'unit' input parameter at entry
 */
#define BCM_INVOKE_VERIFY_DNX(_expr) \
{ \
  SHR_IF_ERR_EXIT_WITH_LOG(_expr,"on VERIFY. %s%s%s",EMPTY,EMPTY,EMPTY) ; \
}
/*!
 * \brief Check for null-pointer.
 *
 * Check if a pointer is NULL, and if so, log an error (with some fixed extended
 * info) and exit. Do print input name of tested pointer.
 *
 * The macro is intended for both input parameter checks and memory
 * allocation errors.
 *
 * \param [in] _ptr Pointer to check.
 * \param [in] _rv Function return value to use if pointer is NULL.
 * \param [in] _str Ascii. Name of tested pointer. MUST be encapsulated in double quotes.
 * \remark
 *  Indirect input:
 *    BSL_LOG_MODULE           - Module to report on. Defined at head of file.
 *    SHR_IF_ERR_EXIT_WITH_LOG - Used, with degenerated input, for implementation of this macro.
 *    _func_unit               - Value of 'unit' input parameter at entry
 */
#define SHR_NULL_CHECK(_ptr, _rv, _str) \
{ \
  if ((_ptr) == NULL) \
  { \
    SHR_IF_ERR_EXIT_WITH_LOG(_rv," because %s is NULL %s%s\r\n",_str,EMPTY,EMPTY) ; \
  } \
}
/*!
 * \brief Goto single point of exit.
 *
 * Go to label 'exit', which must be defined explicitly in each
 * function.
 *
 * This macro is normally not called directly, but it is used as part
 * of other error handling macros.
 */
#define SHR_EXIT() goto exit

/*!
 * \brief Check for function error state.
 *
 * This macro is a Boolean expression, which evaluates to TRUE, if the
 * macro-based function return value (declared by \ref SHR_FUNC_ENTER)
 * is set to error.
 *
 * It can be used to clean up allocated resources in case of failure,
 * for example:
 *
 * \code{.c}
 * void my_func(int unit,int index, int value)
 * {
 *   int local_idx ;
 *
 *   SHR_FUNC_INIT_VARS(unit) ;
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *   ...
 *
 * exit:
 *   if (SHR_FUNC_ERROR())
 *   {
 *         ...
 *   }
 *   SHR_FUNC_EXIT("local_idx %d %s%s%s\r\n",local_idx ,EMPTY,EMPTY,EMPTY) ;
 * }
 * \endcode
 * \remark
 *  Indirect input:
 *    _func_rv           - Curent value of function's return value.
 *  See shr_error_e
 */
#define SHR_FUNC_ERR()    SHR_FAILURE(_func_rv)

/*!
 * \brief Assign function return value from expression and exit.
 *
 * The expression can be a function call or a fixed error code.
 *
 * See \ref SHR_FUNC_VAL_IS for an example.
 *
 * \param [in] _expr Expression to evaluate.
 */
#define SHR_RETURN_VAL_EXIT(_expr)          \
  {                                         \
    _func_rv = _expr ;                      \
    SHR_EXIT() ;                            \
  }
/*!
 * \brief Warn on expression error.
 *
 * Evaluate an expression and log a warning message if the expression
 * evaluates to a standard error code. The result is assigned to the
 * macro-based function return value (declared by \ref SHR_FUNC_ENTER),
 * but code execution continues immediately below the statement (i.e.,
 * no jump to the single point of exit).
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit,int index, int value)
 * {
 *   int local_idx ;
 *   int var1, var2 ;
 *
 *   SHR_FUNC_INIT_VARS(unit) ;
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *
 *   SHR_IF_ERR_CONT(some_other_func(unit)," var1: %d, var2: %d %s",var1,var2,EMPTY) ;
 *   ...
 *
 * exit:
 *   SHR_FUNC_EXIT("local_idx %d %s%s%s\r\n",local_idx ,EMPTY,EMPTY,EMPTY) ;
 * }
 * \endcode
 *
 * \param [in] _expr Expression to evaluate. This is also _param1 of the formatting string
 * \param [in] _formatting_string to display if print is not filtered. MUST be encapsulated in double quotes.
 * \param [in] _params Extra three parameters (_param2,_param3,_param4) to display via the formatting string. Param1 is _expr above.
 * \remark
 *  Indirect input:
 *    BSL_LOG_MODULE       - Module (layer/source) to report on. Defined at head of file.
 *    LOG_WARN             - Standard BSL (JR1) LOG macro
 *    _func_unit           - Value of 'unit' input parameter at entry
 */
#define SHR_IF_ERR_CONT(_expr,_formatting_string,_param2,_param3,_param4) \
  {                                                 \
    shr_error_e _rv = _expr ;                       \
    if (SHR_FAILURE(_rv))                           \
    {                                               \
      LOG_WARN_EX(BSL_LOG_MODULE,                   \
        (BSL_META_U(_func_unit," Error: %s ; " _formatting_string),shrextend_errmsg_get(_rv),_param2,_param3,_param4 )) ; \
        _func_rv = _rv ;                            \
    }                                               \
  }

/*!
 * \brief Error-exit on expression, BUT - with exception
 *
 * Evaluate an expression and if it evaluates to a standard error code
 * which differs from the exception value, then log a error message
 * and go to the function's single point of exit.
 *
 * The log message will use BSL_LOG_MODULE as the log source, so this
 * name must be assigned to a relevant BSL log source in advance.
 *
 * Typical exception error codes are SHR_E_NOT_FOUND and
 * SHR_E_UNAVAIL, because these error codes may be a simple reflection
 * of the device state or capabilties, which are used to control the
 * logical flow of the function.
 *
 *
 * Example:
 *
 * \code{.c}
 * void my_func(int unit,int index, int value)
 * {
 *   int rv, local_idx ;
 *   int var1, var2 ;
 *
 *   SHR_FUNC_INIT_VARS(unit) ;
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *
 *   rv = some_other_func(unit) ;
 *   SHR_IF_ERR_EXIT_EXCEPT_IF(rv, SHR_E_UNAVAIL," var1: %d, var2: %d %s",var1,var2,EMPTY) ;
 *   ...
 *
 * exit:
 *   SHR_FUNC_EXIT("local_idx %d %s%s%s\r\n",local_idx ,EMPTY,EMPTY,EMPTY) ;
 * }
 * \endcode
 * \param [in] _expr Expression to evaluate. This is also _param1 of the formatting string
 * \param [in] _rv_except Error code to ignore.
 * \param [in] _formatting_string to display if print is not filtered. MUST be encapsulated in double quotes.
 * \param [in] _params Extra three parameters (_param2,_param3,_param4) to display via the formatting string. Param1 is _expr above.
 * \remark
 *  Indirect input:
 *    BSL_LOG_MODULE       - Module (layer/source) to report on. Defined at head of file.
 *    LOG_ERROR            - Standard BSL (JR1) LOG macro
 *    _func_unit           - Value of 'unit' input parameter at entry
 */
#define SHR_IF_ERR_EXIT_EXCEPT_IF(_expr, _rv_except,_formatting_string,_param2,_param3,_param4)   \
   {                                                   \
      int _rv = _expr ;                                \
      if (SHR_FAILURE(_rv) && _rv != _rv_except)       \
      {                                                \
        LOG_ERROR_EX(BSL_LOG_MODULE,                   \
          (BSL_META_U(_func_unit," Error: %s ; " _formatting_string), \
          shrextend_errmsg_get(_rv) ,_param2,_param3,_param4)) ; \
        _func_rv = _rv ;                            \
        SHR_EXIT() ;                                \
      }                                             \
   }
/*!
 * \brief Check for specific function error value.
 *
 * This macro is a Boolean expression, which evaluates to TRUE, if the
 * macro-based function return value (declared by \ref SHR_FUNC_ENTER)
 * matches the specified value.
 *
 * It can be used to take special actions for specific error codes.
 *
 * \code{.c}
 * void my_func(int unit,int index, int value)
 * {
 *   int local_idx ;
 *
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *   ...
 *
 *   if (SHR_FUNC_VAL_IS(SHR_E_UNAVAIL))
 *   {
 *     SHR_RETURN_VAL_EXIT(SHR_E_NONE) ;
 *   }
 *   ...
 *
 * exit:
 *   SHR_FUNC_EXIT("local_idx %d %s%s%s\r\n",local_idx ,EMPTY,EMPTY,EMPTY) ;
 * }
 * \endcode
 *
 * \param [in] _rv Return value to check against.
 * \remark
 *  Indirect input:
 *    _func_rv           - Curent value of function's return value.
 */
#define SHR_FUNC_VAL_IS(_rv)     _func_rv == (_rv)

/*!
 * \brief Get current return value.
 *
 * This macro gets the current return value (declared by \ref SHR_FUNC_ENTER).
 *
 * It can be used to take special actions for specific error codes.
 *
 * \code{.c}
 * void my_func(int unit,int index, int value)
 * {
 *   int local_idx, ret_val ;
 *
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *   ...
 *
 *   ret_val = SHR_GET_CURRENT_ERR() ;
 *   if (ret_val == SHR_E_UNAVAIL)
 *   {
 *     SHR_RETURN_VAL_EXIT(SHR_E_NONE) ;
 *   }
 *   ...
 *
 * exit:
 *   SHR_FUNC_EXIT("local_idx %d %s%s%s\r\n",local_idx ,EMPTY,EMPTY,EMPTY) ;
 * }
 * \endcode
 *
 * \remark
 *  Indirect input:
 *    _func_rv           - Curent value of function's return value.
 */
#define SHR_GET_CURRENT_ERR()     _func_rv

/*!
 * \brief Set current return value.
 *
 * This macro sets the current return value (declared by \ref SHR_FUNC_ENTER).
 *
 * It can be used to take special actions for specific error codes.
 *
 * \code{.c}
 * void my_func(int unit,int index, int value)
 * {
 *   int local_idx, ret_val ;
 *
 *   SHR_FUNC_ENTER("Index %d, Value %d %s%s\r\n",index,value,EMPTY,EMPTY) ;
 *   ...
 *
 *   SHR_SET_CURRENT_ERR(SHR_E_UNAVAIL) ;
 *   ...
 *   if (ret_val == SHR_E_UNAVAIL)
 *   {
 *     SHR_RETURN_VAL_EXIT(SHR_E_NONE) ;
 *   }
 *   ...
 *
 * exit:
 *   SHR_FUNC_EXIT("local_idx %d %s%s%s\r\n",local_idx ,EMPTY,EMPTY,EMPTY) ;
 * }
 * \endcode
 *
 * \remark
 *  Indirect input:
 *    _func_rv           - Curent value of function's return value.
 */
#define SHR_SET_CURRENT_ERR(_rv)     _func_rv = _rv


/*!
 * \brief Memory allocation with leak check.
 *
 * This macro is intended to prevent memory leaks from premature
 * pointer recycling. This type of leak happens if a pointer to an
 * allocated block of memory is used for another memory allocation
 * before the first block is freed.
 *
 * If the above error is detected, the macro will log and error and
 * exit the current function, otherwise a normal memory allocation
 * will be done using sal_alloc.
 *
 * The macro depends on \ref SHR_FUNC_ENTER and may be used in
 * conjunction with \ref SHR_FREE.
 *
 * \param [in] _ptr Pointer variable to use for memory allocation.
 * \param [in] _sz Amount of memory to allocate (in bytes).
 * \param [in] _str Debug string to be passed to sal_alloc.
 * \param [in] _formatting_string to display if print is not filtered. MUST be encapsulated in double quotes.
 * \param [in] _params Extra three parameters (_param2,_param3,_param4) to display via the formatting string. Param1 is _str above.
 * \remark
 *  Indirect input:
 *    BSL_LOG_MODULE       - Module (layer/source) to report on. Defined at head of file.
 *    LOG_ERROR            - Standard BSL (JR1) LOG macro
 *    _func_unit           - Value of 'unit' input parameter at entry
 */
#define SHR_ALLOC(_ptr, _sz, _str, _formatting_string,_param2,_param3,_param4)  \
{                                                \
  if ((_ptr) == NULL)                            \
  {                                              \
    (_ptr) = sal_alloc(_sz, _str) ;              \
  }                                              \
  else                                           \
  {                                              \
    LOG_ERROR_EX(BSL_LOG_MODULE,                   \
      (BSL_META_U(_func_unit," Ptr is NOT NULL at alloc: mem id is '%s' ; " _formatting_string), \
      _str ,_param2,_param3,_param4)) ; \
    SHR_EXIT() ;                                 \
  }                                              \
}

/*!
 * \brief Free memory and reset pointer.
 *
 * This macro will free allocated memory and reset the associated
 * pointer to NULL. The memory will be freed using sal_free.
 *
 * The macro may be used in conjunction with \ref SHR_ALLOC.
 *
 * \param [in] _ptr Pointer to memory to be freed.
 */
#define SHR_FREE(_ptr)            \
{                                 \
  if ((_ptr) != NULL)             \
  {                               \
    sal_free((void *)(_ptr)) ;    \
    (_ptr) = NULL ;               \
  }                               \
}

/* } */
#endif  /* !_SHREXTEND_DEBUG_H_INCLUDED */
