/*
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is a utility module common for all error recovery features
 */
#ifndef _DNXC_ROLLBACK_JOURNAL_UTILS_H
/* { */
#define _DNXC_ROLLBACK_JOURNAL_UTILS_H

#ifdef BCM_DNX_SUPPORT
/*
 * \brief - enable or disable error recovery initialization at compile time.
 */
#define DNXC_ROLLBACK_JOURNAL_IS_ENABLED

/*
 * \brief - max length of rollback journal api name.
 */
#define DNXC_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH 100

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
/*
 * \brief - define the basic set of validation required for regression and pedantic validation
 */
#define DNX_ERR_RECOVERY_VALIDATION_BASIC

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
/*
 * \brief - Enables ER "regression testing" code at compile time.
 * Current strategy for sanitized branches is to allow "regression testing" without "validation".
 */
#define DNX_ERR_RECOVERY_REGRESSION_TESTING

#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */


/*
 * sanity checking flags
 */
#if !defined(DNX_ERR_RECOVERY_REGRESSION_TESTING) && !defined(DNX_ERR_RECOVERY_VALIDATION)
#error "DNX_ERR_RECOVERY_REGRESSION_TESTING or DNX_ERR_RECOVERY_VALIDATION must be ENABLED"
#endif

#if !defined(DNX_ERR_RECOVERY_REGRESSION_TESTING) && defined(DNX_ERR_RECOVERY_VALIDATION)
#error "DNX_ERR_RECOVERY_VALIDATION cannot be enabled without DNX_ERR_RECOVERY_REGRESSION_TESTING"
#endif
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

/**
 * \brief - define the type of journals.
 */
typedef enum
{
    DNXC_ROLLBACK_JOURNAL_TYPE_ROLLBACK = 0,
    DNXC_ROLLBACK_JOURNAL_TYPE_COMPARISON,
    DNXC_ROLLBACK_JOURNAL_TYPE_NOF,
} dnxc_rollback_journal_type_e;

/**
 * \brief - define the subtype of journals.
 */
typedef enum dnxc_rollback_journal_subtype_d
{
    DNXC_ROLLBACK_JOURNAL_SUBTYPE_INVALID,
    DNXC_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE,
    DNXC_ROLLBACK_JOURNAL_SUBTYPE_DBAL,
    DNXC_ROLLBACK_JOURNAL_SUBTYPE_GENERIC,
    DNXC_ROLLBACK_JOURNAL_SUBTYPE_ALL
} dnxc_rollback_journal_subtype_e;
#endif /* BCM_DNX_SUPPORT */

/* } */
#endif /* _DNXC_ROLLBACK_JOURNAL_UTILS_H */
