/*
 * $Id: $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: This module is a utility module common for all error recovery features
 */
#ifndef _DNXC_ROLLBACK_JOURNAL_UTILS_H
/* { */
#define _DNXC_ROLLBACK_JOURNAL_UTILS_H

#define DNXC_ROLLBACK_JOURNAL_API_NAME_MAX_LENGTH 100


/**
 * \brief - define the type of journals.
 */
typedef enum dnxc_rollback_journal_type_e
{
    DNXC_ROLLBACK_JOURNAL_TYPE_INVALID,
    DNXC_ROLLBACK_JOURNAL_TYPE_ROLLBACK,
    DNXC_ROLLBACK_JOURNAL_TYPE_COMPARISON
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

/* } */
#endif /* _DNXC_ROLLBACK_JOURNAL_UTILS_H */
