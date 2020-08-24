/** \file sw_state_pbmp.h
 * This file contains functions and definitions to utilize sw state port bitmaps
 */
/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _DNXC_SW_STATE_PBMP_H
/* { */
#define _DNXC_SW_STATE_PBMP_H

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/pbmp.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * \brief -  Check if source and destination port bitmaps are equal.
 */
#define DNX_SW_STATE_PBMP_EQ(unit, module_id, path, input_pbmp, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_eq(unit, module_id, &path, input_pbmp, result, flags, debug_string));\
} while(0)

/**
 * \brief -  Check if source and destination port bitmaps are NOT equal.
 */
#define DNX_SW_STATE_PBMP_NEQ(unit, module_id, path, input_pbmp, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_eq(unit, module_id, &path, input_pbmp, result, flags, debug_string));\
    *result = !(*result);\
} while(0)

/**
 * \brief -  Port bitmap check if input port is a member of a port bitmap.
 */
#define DNX_SW_STATE_PBMP_MEMBER(unit, module_id, path, input_port, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_member(unit, module_id, &path, input_port, result, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap check if NULL.
 */
#define DNX_SW_STATE_PBMP_IS_NULL(unit, module_id, path, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_is_null(unit, module_id, &path, result, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap check if NOT NULL.
 */
#define DNX_SW_STATE_PBMP_NOT_NULL(unit, module_id, path, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_is_null(unit, module_id, &path, result, flags, debug_string));\
    *result = !(*result);\
} while(0)

/**
 * \brief -  Port bitmap get member count.
 */
#define DNX_SW_STATE_PBMP_COUNT(unit, module_id, path, result, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_count(unit, module_id, &path, result, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap get.
 */
#define DNX_SW_STATE_PBMP_GET(unit, module_id, path, output_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_get(unit, module_id, &path, output_pbmp, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap assign.
 */
#define DNX_SW_STATE_PBMP_ASSIGN(unit, module_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_assign(unit, module_id, &path, input_pbmp, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap and, save result in swstate.
 */
#define DNX_SW_STATE_PBMP_AND(unit, module_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_and(unit, module_id, &path, input_pbmp, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap OR between two instances, save result in swstate.
 */
#define DNX_SW_STATE_PBMP_OR(unit, module_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_or(unit, module_id, &path, input_pbmp, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap XOR between two instances, save result in swstate.
 */
#define DNX_SW_STATE_PBMP_XOR(unit, module_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_xor(unit, module_id, &path, input_pbmp, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap remove.
 */
#define DNX_SW_STATE_PBMP_REMOVE(unit, module_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_remove(unit, module_id, &path, input_pbmp, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap negate, save result in swstate.
 */
#define DNX_SW_STATE_PBMP_NEGATE(unit, module_id, path, input_pbmp, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_negate(unit, module_id, &path, input_pbmp, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap clear.
 */
#define DNX_SW_STATE_PBMP_CLEAR(unit, module_id, path, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_clear(unit, module_id, &path, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap PORT SET.
 */
#define DNX_SW_STATE_PBMP_PORT_SET(unit, module_id, path, input_port, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_set(unit, module_id, &path, input_port, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap add port.
 */
#define DNX_SW_STATE_PBMP_PORT_ADD(unit, module_id, path, input_port, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_add(unit, module_id, &path, input_port, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap remove port.
 */
#define DNX_SW_STATE_PBMP_PORT_REMOVE(unit, module_id, path, input_port, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_remove(unit, module_id, &path, input_port, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap flip port.
 */
#define DNX_SW_STATE_PBMP_PORT_FLIP(unit, module_id, path, input_port, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_flip(unit, module_id, &path, input_port, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap add range of ports.
 */
#define DNX_SW_STATE_PBMP_PORTS_RANGE_ADD(unit, module_id, path, input_port, range, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_port_range_add(unit, module_id, &path, input_port, range, flags, debug_string));\
} while(0)

/**
 * \brief -  Port bitmap FTM.
 */
#define DNX_SW_STATE_PBMP_FMT(unit, module_id, path, buffer, flags, debug_string)\
do {\
    SHR_IF_ERR_EXIT(dnxc_sw_state_pbmp_fmt(unit, module_id, &path, buffer, flags, debug_string));\
} while(0)

/**
 * \brief -  Swstate port bitmap type
 */
typedef _shr_pbmp_t dnxc_sw_state_pbmp;

/*
 * }
 */

/**
 * \brief
 *  Check if swstate port bitmap is equal to an input port bitmap.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_bitmap - Source port bitmap
 * \param [out] result - Bit result
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_eq(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_bitmap,
    uint8 *result,
    uint32 flags,
    char *dbg_string);


/**
 * \brief
 *  Check if input port is a member of a port bitmap
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to pointer bitmap
 * \param [in] input_port - Input port
 * \param [out] result - Bit result
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_member(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Check if port bitmap is null
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to pointer bitmap
 * \param [out] result - Bit result
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_is_null(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    uint8 *result,
    uint32 flags,
    char *dbg_string);


/**
 * \brief
 *  Count number of members in a pbmp
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to pointer bitmap
 * \param [out] result - Count result
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_count(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    int *result,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Get port bitmap
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to source port bitmap
 * \param [in] result - Pointer to destination port bitmap
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_get(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp * result,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Assign port bitmap
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] src - Source port bitmap
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_assign(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp src,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap and, save result in swstate.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_pbmp - Source port bitmap
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_and(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap or, save result in swstate.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_pbmp - Pointer to source port bitmap
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_or(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap remove
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_pbmp - Source port bitmap
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_xor(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap remove
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_pbmp - Source port bitmap
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_remove(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_pbmp,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap negate, save result in swstate.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_bitmap - Source port bitmap 
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_negate(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    dnxc_sw_state_pbmp input_bitmap,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap clear.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_clear(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap port set.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_port - Input port
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_port_set(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap port add.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_port - Input port
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_port_add(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap port remove.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_port - Input port
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_port_remove(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap port flip.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_port - Input port
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_port_flip(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Port bitmap port range add.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] pbmp - Pointer to destination port bitmap
 * \param [in] input_port - Input port
 * \param [in] range - Port range
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_pbmp_port_range_add(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    uint32 input_port,
    uint32 range,
    uint32 flags,
    char *dbg_string);


int dnxc_sw_state_pbmp_fmt(
    int unit,
    uint32 module_id,
    dnxc_sw_state_pbmp * pbmp,
    char *buffer,
    uint32 flags,
    char *dbg_string);

#endif /* _DNXC_SW_STATE_PBMP_H */
