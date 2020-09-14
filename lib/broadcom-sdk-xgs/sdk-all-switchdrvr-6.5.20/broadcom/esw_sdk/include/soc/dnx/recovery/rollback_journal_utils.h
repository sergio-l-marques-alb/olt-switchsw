/*
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is a utility module common for all error recovery features
 */
#ifndef _DNX_ROLLBACK_JOURNAL_UTILS_H
/* { */
#define _DNX_ROLLBACK_JOURNAL_UTILS_H

#ifdef BCM_DNX_SUPPORT
/*
 * \brief - enable or disable error recovery initialization at compile time.
 */

/*
 * \brief - max length of rollback journal api name.
 */
#define DNX_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH 100

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

/**
 * \brief - define the type of journals.
 */
typedef enum
{
    DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK = 0,
    DNX_ROLLBACK_JOURNAL_TYPE_COMPARISON,
    DNX_ROLLBACK_JOURNAL_TYPE_NOF
} dnx_rollback_journal_type_e;

/**
 * \brief - define the subtype of journals.
 */
typedef enum dnx_rollback_journal_subtype_d
{
    DNX_ROLLBACK_JOURNAL_SUBTYPE_INVALID,
    DNX_ROLLBACK_JOURNAL_SUBTYPE_SWSTATE,
    DNX_ROLLBACK_JOURNAL_SUBTYPE_DBAL,
    DNX_ROLLBACK_JOURNAL_SUBTYPE_GENERIC,
    DNX_ROLLBACK_JOURNAL_SUBTYPE_ALL
} dnx_rollback_journal_subtype_e;
#endif /* BCM_DNX_SUPPORT */

/* } */
#endif /* _DNX_ROLLBACK_JOURNAL_UTILS_H */
