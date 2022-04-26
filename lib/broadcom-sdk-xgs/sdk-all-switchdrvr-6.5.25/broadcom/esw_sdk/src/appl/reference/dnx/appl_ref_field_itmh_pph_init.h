/** \file appl_ref_field_itmh_pph_init.h
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** Number of Field Groups needed (used as array size) */
#define ITMH_PPH_NOF_FG                      11

/**
 * \brief
 *  This function creates field group, with all
 *  relevant information to it and adding it to the context.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
typedef shr_error_e(
    *field_itmh_pph_config_cb) (
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info);

/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
typedef struct
{
    char *name;
    int context_ids;
    field_itmh_pph_config_cb config_cb;
} field_itmh_pph_fg_info_t;

/*
 * }
 */

/**
 * \brief
 * configure PMF context to handle TM program to parse ITMH packets
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_field_itmh_init(
    int unit);

/**
 * \brief
 * configure PMF context to handle TM program to parse PPH over ITMH packets
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_field_itmh_pph_init(
    int unit);
/*
 * }
 */
#endif /* APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED */
