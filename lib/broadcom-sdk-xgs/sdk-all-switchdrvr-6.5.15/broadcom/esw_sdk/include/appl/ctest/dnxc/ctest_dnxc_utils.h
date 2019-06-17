/*
 * ! \file ctest_dnxc_util.h Contains common dnxc ctest utilities
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef CTEST_DNXC_UTILS_H_INCLUDED
#define CTEST_DNXC_UTILS_H_INCLUDED

#include <shared/utilex/utilex_rhlist.h>

extern sh_sand_cmd_t dnxc_framework_test_cmds[];

shr_error_e ctest_dnxc_init_deinit(
    int unit,
    char *command);

/**
 * \brief Control structure for command keyword definition, provided by framework
 */
typedef struct
{
    char *property;
    char *value;
} ctest_soc_property_t;

typedef struct
{
    rhentry_t entry;
    char *value;
} ctest_soc_entry_t;

/**
 * \brief Modify soc properties according to provided list of names/values and perform deinit/init to bring them into
 *              action
 * \param [in] unit - device ID
 * \param [in] ctest_soc_property_a - pointer to the list of properties - name/value pair,
 *                                                                              last one name should be NULL or empty
 * \param [out] ctest_soc_set_h - place where handle to be used in restore will be put, if the handle is NULL,
 *                                                                              no changes in soc properties happened
 * \return
 *   usual shr_error_e
 * \remark
 *   ctest_dnxc_restore_soc_properties should always be called if the handle is not NULL, it restore the previous state
 *   and frees the resources, it may be called even if it is NULL, nothing bad will happen
 * \see
 *   ctest_dnxc_restore_soc_properties
 */
shr_error_e ctest_dnxc_set_soc_properties(
    int unit,
    ctest_soc_property_t * ctest_soc_property_a,
    rhhandle_t * ctest_soc_set_h);

/**
 * \brief Return soc properties to the state previous to ctest_dnxc_set_soc_properties and invoke deinit/init
 * \param [in] unit - device ID
 * \param [out] ctest_soc_set_h - handle that points to the structure keeping record of previous so property state
 * \return
 *   usual shr_error_e
 * \remark
 * \see
 *   ctest_dnxc_set_soc_properties
 */
shr_error_e ctest_dnxc_restore_soc_properties(
    int unit,
    rhhandle_t ctest_soc_set_h);

#endif /* CTEST_DNXC_UTILS_H_INCLUDED */
