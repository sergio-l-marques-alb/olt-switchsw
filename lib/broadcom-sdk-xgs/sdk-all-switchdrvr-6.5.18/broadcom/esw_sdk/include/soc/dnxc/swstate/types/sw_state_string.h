/** \file sw_state_string.h
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __SW_STATE_STRING_H__
#define __SW_STATE_STRING_H__

/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>

#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnx_sw_state_logging.h>
#include <soc/dnxc/swstate/sw_state_diagnostics.h>

/*
 * MACROs
 */
#define SW_STATE_STRING_STRINGNCAT(module_id, string, src, size)\
        SHR_IF_ERR_EXIT(sw_state_string_strncat(unit, module_id, string, size, src))

#define SW_STATE_STRING_STRINGNCMP(module_id, string, cmp_string, result, size)\
        SHR_IF_ERR_EXIT(sw_state_string_strncmp(unit, module_id, string, size, cmp_string, result))

#define SW_STATE_STRING_STRINGNCPY(module_id, string, src, size)\
        SHR_IF_ERR_EXIT(sw_state_string_strncpy(unit, module_id, string, size, src))

#define SW_STATE_STRING_STRINGLEN(module_id, string, size)\
        SHR_IF_ERR_EXIT(sw_state_string_strlen(unit, module_id, string, size))

#define SW_STATE_STRING_STRINGGET(module_id, string, dest)\
        SHR_IF_ERR_EXIT(sw_state_stringget(unit, module_id, string, dest))

#define SW_STATE_STRING_STRING_PRINT(unit, module_id, string)\
        SHR_IF_ERR_EXIT(sw_state_string_print(unit, module_id, *(string)))

/*
 * TYPEDEFs
 */

typedef char sw_state_string_t;
/*
 * FUNCTIONs
 */

/**
* \brief
*   Add a string to the string structure value.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] string - String type structure.
*  \param [in] size - Size.
*  \param [in] src - String to be added.
*  \return
*     shr_error_e
*  \remark
*    None
*****************************************************/
shr_error_e sw_state_string_strncat(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 size,
    char *src);

/**
* \brief
*   Compare the string value with the given string.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] string - String type structure.
*  \param [in] size - Size.
*  \param [in] cmp_string - String to be compared with the strings struct value.
*  \param [out] result - Result of the compare.
*  \return
*     shr_error_e
*  \remark
*    None
*****************************************************/
shr_error_e sw_state_string_strncmp(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 size,
    char *cmp_string,
    int *result);

/**
* \brief
*   Replace the value of the string structure with the input.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] string - String type structure.
*  \param [in] size - Size.
*  \param [in] src - String to be saved.
*  \return
*     shr_error_e
*  \remark
*    None
*****************************************************/
shr_error_e sw_state_string_strncpy(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 size,
    char *src);

/**
* \brief
*   Gets the size that was used during the create of the string.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] string - String type structure.
*  \param [out] size - Size of the string value.
*  \return
*     shr_error_e
*  \remark
*    None
*****************************************************/
shr_error_e sw_state_string_strlen(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 *size);

/**
* \brief
*   Gets the string value.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] string - String type structure.
*  \param [out] dest - Pointer to a memory where the string value will be saved.
*  \return
*     shr_error_e
*  \remark
*    None
*****************************************************/
shr_error_e sw_state_stringget(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    char *dest);

/**
* \brief
*   Prints the sw state string type variable.
*
*  \param [in] unit - Relevant unit.
*  \param [in] module_id - Module ID.
*  \param [in] string - String type structure.
*  \return
*     shr_error_e
*  \remark
*    None
*****************************************************/
shr_error_e sw_state_string_print(
    int unit,
    uint32 module_id,
    sw_state_string_t * string);
#endif /* __SW_STATE_STRING_H__ */
