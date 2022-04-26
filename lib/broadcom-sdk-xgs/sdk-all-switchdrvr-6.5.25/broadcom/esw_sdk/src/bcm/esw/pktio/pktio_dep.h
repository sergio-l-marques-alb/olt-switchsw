/*
 *
 * PKTIO dependencies.
 *
 * Shim layer for resolving compatibilities between SDK6 and imported modules.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef _PKTIO_DEP_H_
#define _PKTIO_DEP_H_

#include <assert.h>
#include <sal/types.h>
#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/appl/sal.h>
#include <shared/error.h>
#include <shared/bsl.h>
#include <shared/bitop.h>
#include <shared/shr_pb.h>
#include <shared/shr_thread.h>
#include <shared/shr_timeout.h>
#include <shared/shr_util_endian.h>

#include <shared/bsltypes.h>
#include <shared/bslenum.h>
#include <sal/appl/io.h>
#include <sal/core/time.h>
/*
 * Basic types
 */
typedef long long unsigned int uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef enum { false = 0, true = 1 } bool;

typedef unsigned int dma_addr_t;

/*! Ethernet MAC address length. */
#define SHR_MAC_ADDR_LEN                6
/*! Ethernet MAC address type. */
typedef uint8_t shr_mac_t[SHR_MAC_ADDR_LEN];

/*
 * SAL
 */
#define SAL_SEM_FOREVER             sal_sem_FOREVER
#define SAL_SEM_BINARY              sal_sem_BINARY
#define SAL_SEM_COUNTING            sal_sem_COUNTING
#define SAL_MUTEX_FOREVER           sal_mutex_FOREVER
#define SAL_MUTEX_NOWAIT            sal_mutex_NOWAIT

#define PRIu32                      "u"
#define PRIu64                      "llu"

#define SAL_CONFIG_MEMORY_BARRIER   ;

/*
 * SHR
 */
typedef _shr_error_t                shr_error_t;
#define SHR_E_NONE                  _SHR_E_NONE
#define SHR_E_INTERNAL              _SHR_E_INTERNAL
#define SHR_E_MEMORY                _SHR_E_MEMORY
#define SHR_E_UNIT                  _SHR_E_UNIT
#define SHR_E_PARAM                 _SHR_E_PARAM
#define SHR_E_EMPTY                 _SHR_E_EMPTY
#define SHR_E_FULL                  _SHR_E_FULL
#define SHR_E_NOT_FOUND             _SHR_E_NOT_FOUND
#define SHR_E_EXISTS                _SHR_E_EXISTS
#define SHR_E_TIMEOUT               _SHR_E_TIMEOUT
#define SHR_E_BUSY                  _SHR_E_BUSY
#define SHR_E_FAIL                  _SHR_E_FAIL
#define SHR_E_DISABLED              _SHR_E_DISABLED
#define SHR_E_BADID                 _SHR_E_BADID
#define SHR_E_RESOURCE              _SHR_E_RESOURCE
#define SHR_E_CONFIG                _SHR_E_CONFIG
#define SHR_E_UNAVAIL               _SHR_E_UNAVAIL
#define SHR_E_INIT                  _SHR_E_INIT
#define SHR_E_PORT                  _SHR_E_PORT
#define SHR_E_IO                    _SHR_E_IO
#define SHR_E_ACCESS                _SHR_E_ACCESS
#define SHR_E_NO_HANDLER            _SHR_E_NO_HANDLER
#define SHR_E_PARTIAL               _SHR_E_PARTIAL


#ifdef LOG_ERROR_EX
#undef LOG_ERROR_EX
#endif
#define LOG_ERROR_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)    \
  if (bsl_fast_check(_log_module|BSL_ERROR)) \
  { \
    sal_printf("\r\n") ; \
  } \
  LOG_ERROR(_log_module,                                             \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4)  )

#ifdef LOG_WARN_EX
#undef LOG_WARN_EX
#endif
#define LOG_WARN_EX(_log_module,_format_string, _p1,_p2,_p3,_p4)     \
  if (bsl_fast_check(_log_module|BSL_WARN)) \
  {                                         \
    sal_printf("\r\n") ;                    \
  }                                         \
  LOG_WARN(_log_module,                                              \
    (BSL_META_U(_func_unit,_format_string),_p1,_p2,_p3,_p4)  )

#ifdef SHR_FUNC_INIT_VARS
#undef SHR_FUNC_INIT_VARS
#endif
#define SHR_FUNC_INIT_VARS(_unit)    \
  int _func_unit = _unit ;           \
  int _func_rv = _SHR_E_NONE ;       \
  COMPILER_REFERENCE(_func_unit);

#define SHR_FUNC_ENTER(_unit)       SHR_FUNC_INIT_VARS(_unit)

#undef SHR_FUNC_EXIT
#define SHR_FUNC_EXIT()             return _func_rv;

#define SHR_RETURN_VAL_EXIT(_expr)              \
    do {                                        \
        _func_rv = _expr;                       \
        SHR_EXIT();                             \
    } while (0)

#ifdef SHR_IF_ERR_EXIT
#undef SHR_IF_ERR_EXIT
#endif
#define SHR_IF_ERR_EXIT(_expr)      \
do                      \
{                       \
  int _rv = _expr;      \
  if (_rv < 0) \
  {                     \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%d' indicated ; %s%s%s\r\n" ,           \
      _rv ,"","","") ;  \
    _func_rv = _rv ;  \
    SHR_EXIT() ;        \
  }                     \
} while (0)

#ifdef SHR_IF_ERR_EXIT_EXCEPT_IF
#undef SHR_IF_ERR_EXIT_EXCEPT_IF
#endif
#define SHR_IF_ERR_EXIT_EXCEPT_IF(_expr, _rv_except)   \
{                                                  \
  int _rv = _expr ;                                \
  if (SHR_FAILURE(_rv) && _rv != _rv_except)       \
  {                                                \
    LOG_ERROR_EX(BSL_LOG_MODULE,                       \
      " Error '%d' indicated ; %s%s%s\r\n" ,           \
      _rv ,"","","") ;  \
    _func_rv = _rv ;                            \
    SHR_EXIT() ;                                \
  }                                             \
}

#ifdef SHR_EXIT
#undef SHR_EXIT
#endif
#define SHR_EXIT() goto exit


#define SHR_IF_ERR_MSG_EXIT(_expr, _stuff)               \
    do {                                                 \
        int _tmp_rv = _expr;                             \
        if (_tmp_rv < 0) {                      \
            LOG_ERROR(BSL_LOG_MODULE, _stuff);           \
            _func_rv = _tmp_rv;                          \
            SHR_EXIT();                                  \
        }                                                \
    } while(0)

#define SHR_IF_ERR_VERBOSE_EXIT(_expr)                   \
    do {                                                 \
        int _tmp_rv = _expr;                             \
        if (_tmp_rv < 0) {                      \
            LOG_VERBOSE(BSL_LOG_MODULE,                  \
                        (BSL_META_U(_func_unit, "%d\n"), \
                         _tmp_rv));         \
            _func_rv = _tmp_rv;                          \
            SHR_EXIT();                                  \
        }                                                \
    } while(0)

#define SHR_IF_ERR_VERBOSE_MSG_EXIT(_expr, _stuff)       \
    do {                                                 \
        int _tmp_rv = _expr;                             \
        if (_tmp_rv < 0) {                      \
            LOG_VERBOSE(BSL_LOG_MODULE, _stuff);         \
            _func_rv = _tmp_rv;                          \
            SHR_EXIT();                                  \
        }                                                \
    } while(0)

#define SHR_NULL_VERBOSE_CHECK(_ptr, _rv)       \
    do {                                        \
        if ((_ptr) == NULL) {                   \
            LOG_VERBOSE(BSL_LOG_MODULE,         \
                      (BSL_META_U(_func_unit,   \
                                  "null\n")));  \
            _func_rv = _rv;                     \
            SHR_EXIT();                         \
        }                                       \
    } while (0)

#undef SHR_NULL_CHECK
#define SHR_NULL_CHECK(_ptr, _rv)               \
    do {                                        \
        if ((_ptr) == NULL) {                   \
            LOG_ERROR(BSL_LOG_MODULE,           \
                      (BSL_META_U(_func_unit,   \
                                  "null\n")));  \
            _func_rv = _rv;                     \
            SHR_EXIT();                         \
        }                                       \
        /* Coverity: the dead code is empty */  \
        /* coverity[dead_error_line:FALSE] */   \
    } while (0)

#undef SHR_ALLOC
#define SHR_ALLOC(_ptr, _sz, _str)                      \
    do {                                                \
        if ((_ptr) == NULL) {                           \
            (_ptr) = sal_alloc(_sz, _str);              \
        } else {                                        \
            LOG_ERROR(BSL_LOG_MODULE,                   \
                      (BSL_META_U(_func_unit,           \
                                  "not null (%s)\n"),   \
                       _str));                          \
            SHR_EXIT();                                 \
        }                                               \
    } while (0)

#ifdef SHR_FREE
#undef SHR_FREE
#endif
#define SHR_FREE(_ptr)          \
{                                 \
  if ((_ptr) != NULL)             \
  {                               \
    sal_free((void *)(_ptr)) ;    \
    (_ptr) = NULL ;               \
  }                               \
}

#ifdef SHR_FAILURE
#undef SHR_FAILURE
#endif
#define SHR_FAILURE(_expr) ((_expr) < 0)

#ifdef SHR_SUCCESS
#undef SHR_SUCCESS
#endif
#define SHR_SUCCESS(_expr) ((_expr) >= 0)

#ifdef SHR_FUNC_ERR
#undef SHR_FUNC_ERR
#endif
#define SHR_FUNC_ERR()    SHR_FAILURE(_func_rv)

#ifdef SHR_FUNC_VAL_IS
#undef SHR_FUNC_VAL_IS
#endif
#define SHR_FUNC_VAL_IS(_rv)     (_func_rv == (_rv))

#ifdef SHR_IF_ERR_CONT
#undef SHR_IF_ERR_CONT
#endif
#define SHR_IF_ERR_CONT(_expr)      \
{                       \
  int _rv = _expr;      \
  if (SHR_FAILURE(_rv)) \
  {                     \
    LOG_WARN_EX(BSL_LOG_MODULE,                       \
      " Warning '%d' indicated ; %s%s%s\r\n" ,             \
      _rv ,"","","") ;  \
    _func_rv = _rv ;  \
  }                     \
}

typedef struct shr_enum_map_s {

    /*! Enum name. */
    char *name;

    /*! Enum value. */
    int val;

} shr_enum_map_t;

/*
 * BSL
 */
#define LOG_CHECK_VERBOSE(ls_)      LOG_CHECK(ls_|BSL_VERBOSE)

#endif /* _PKTIO_DEP_H_ */
