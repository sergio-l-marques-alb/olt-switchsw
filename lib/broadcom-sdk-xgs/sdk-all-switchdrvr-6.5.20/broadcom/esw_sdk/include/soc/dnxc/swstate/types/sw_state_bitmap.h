/** \file sw_state_bitmap.h
 * This file contains functions and definitions to utilize sw state bitmaps
 */
/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _DNXC_SW_STATE_BITMAP_H
/* { */
#define _DNXC_SW_STATE_BITMAP_H

/*
 * Include files
 * {
 */
#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
/*
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * \brief -  Bitmap alloc. Allocate one more byte to save the num of extra bit allocated
 */
#define DNX_SW_STATE_ALLOC_BITMAP(unit, module_id, location, nof_bits_to_alloc, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_alloc(unit, module_id, (uint8 **)(&location), nof_bits_to_alloc, flags, dbg_string));\
    } while(0)

/**
 * \brief -  bitmap, bit set
 */
#define DNX_SW_STATE_BIT_SET(_unit, _module_id, _flags, _sw_state_bmp, __bit_num)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_set(_unit, _module_id, _sw_state_bmp, __bit_num, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, bit clear
 */
#define DNX_SW_STATE_BIT_CLEAR(_unit, _module_id, _flags ,_sw_state_bmp, __bit_num)\
     do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_clear(_unit, _module_id, _sw_state_bmp, __bit_num, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, bit get
 */
#define DNX_SW_STATE_BIT_GET(_unit, _module_id, _flags, _sw_state_bmp, __bit_num , _bit_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_get(_unit, _module_id, _sw_state_bmp, __bit_num, _bit_result, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range read
 */
#define DNX_SW_STATE_BIT_RANGE_READ(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, _result_first, __range, _result_bmp)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_read(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _result_bmp, _result_first, __range, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range write
 */
#define DNX_SW_STATE_BIT_RANGE_WRITE(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, _input_bmp_first, __range, _input_bmp)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_write(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, _input_bmp_first, __range, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range and
 */
#define DNX_SW_STATE_BIT_RANGE_AND(_unit, _module_id, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_and(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, __count, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range or
 */
#define DNX_SW_STATE_BIT_RANGE_OR(_unit, _module_id, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_or(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, __count, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range xor
 */
#define DNX_SW_STATE_BIT_RANGE_XOR(_unit, _module_id, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_xor(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, __count, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range remove
 */
#define DNX_SW_STATE_BIT_RANGE_REMOVE(_unit, _module_id, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_remove(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, __count, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range negate
 */
#define DNX_SW_STATE_BIT_RANGE_NEGATE(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_negate(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range clear
 */
#define DNX_SW_STATE_BIT_RANGE_CLEAR(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_clear(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range set
 */
#define DNX_SW_STATE_BIT_RANGE_SET(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_set(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range null
 */
#define DNX_SW_STATE_BIT_RANGE_NULL(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_null(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range test
 */
#define DNX_SW_STATE_BIT_RANGE_TEST(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_test(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range eq
 */
#define DNX_SW_STATE_BIT_RANGE_EQ(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _input_bmp, _bool_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_eq(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, input_bmp, __count, _bool_result, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, range count
 */
#define DNX_SW_STATE_BIT_RANGE_COUNT(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __range, _int_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_count(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __range, _int_result, _flags, NULL));\
    } while(0)

/**
 * \brief -  bitmap, default value set
 */
#define DNX_SW_STATE_BITMAP_DEFAULT_VALUE_SET(ptr, _nof_bits_to_alloc)\
    do{\
        memset((void *)(ptr), 0xFF, SHR_BITALLOCSIZE(_nof_bits_to_alloc) + 1);\
    } while(0)

/*
 * }
 */

/**
 * \brief
 *  Allocates new dynamic bitmap
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to bitmap structure
 * \param [in] nof_bits_to_alloc - Number of bits to allocate
 * \param [in] flags - Allocation flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_alloc(
    int unit,
    uint32 module_id,
    uint8 **location,
    uint32 nof_bits_to_alloc,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Set bitmap bit at index
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to bitmap structure
 * \param [in] idx_bit - Bit index
 * \param [in] flags - Set flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_set(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 idx_bit,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Clear bitmap bit at index
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to bitmap structure
 * \param [in] idx_bit - Bit index
 * \param [in] flags - Clear flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_clear(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 idx_bit,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Get bitmap bit at index
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to bitmap structure
 * \param [in] idx_bit - Bit index
 * \param [out] bit_result - Pointer to location of result
 * \param [in] flags - Get flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_get(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 idx_bit,
    uint8 *bit_result,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Read a range of bits from sw state bitmap
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to source bitmap structure
 * \param [in] source_first - Offset in source bitmap
 * \param [in] result_location - Pointer to result bitmap structure 
 * \param [in] result_first - Offset in result bitmap
 * \param [in] range - Bit count to read
 * \param [in] flags - Read flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_read(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 source_first,
    SHR_BITDCL * result_location,
    uint32 result_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Write a range of bits from sw state bitmap
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to source bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] source_location - Pointer to source bitmap structure 
 * \param [in] source_first - Offset in source bitmap
 * \param [in] range - Bit count to read
 * \param [in] flags - Read flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_write(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 source_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Performs logical 'AND' between a range of bits of two bitmaps. Result is stored in the destination bitmap.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] source_location - Pointer to source bitmap structure 
 * \param [in] range - Bit count
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_and(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Performs logical 'OR' between a range of bits of two bitmaps. Result is stored in the destination bitmap.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] source_location - Pointer to source bitmap structure 
 * \param [in] range - Bit count
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_or(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Performs logical 'XOR' between a range of bits of two bitmaps. Result is stored in the destination bitmap.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] source_location - Pointer to source bitmap structure 
 * \param [in] range - Bit count
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_xor(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Performs 'REMOVE' between a range of bits of two bitmaps. Result is stored in the destination bitmap.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] source_location - Pointer to source bitmap structure
 * \param [in] range - Bit count
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_remove(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Negates a range of bits in a bitmap.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] range - Bit count
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_negate(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Clears a range of bits in a bitmap.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] range - Bit count
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_clear(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Sets a range of bits in a bitmap.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] range - Bit count
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_set(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Check if a range of bits are NULL.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] range - Bit count
 * \param [in] result - Bit range operation result
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_null(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Test a range of bits.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] range - Bit count
 * \param [in] result - Bit range operation result
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_test(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Check if a range of bits are equal of two bitmaps.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] source_location - Pointer to source bitmap structure
 * \param [in] range - Bit count
 * \param [in] result - Bit range operation result
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_eq(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

/**
 * \brief
 *  Count a range of bits.
 * \param [in] unit - Device Id
 * \param [in] module_id - Module Id
 * \param [in] location - Pointer to destination bitmap structure
 * \param [in] dest_first - Offset in dest bitmap
 * \param [in] range - Bit count
 * \param [in] result - Bit range operation result
 * \param [in] flags - Flags
 * \param [in] dbg_string - Debug string
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnxc_sw_state_bitmap_range_count(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    int *result,
    uint32 flags,
    char *dbg_string);

#endif /* _DNXC_SW_STATE_BITMAP_H */
