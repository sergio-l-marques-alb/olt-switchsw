/*! \file date.h
 *
 * Time API.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef SAL_DATE_H
#define SAL_DATE_H

/*! Buffer size required for holding date string. */
#define SAL_DATE_BUF_MAX        32

/*! String to return in case of error. */
#define SAL_DATE_NULL           "<nodate>"

/*! Structure to hold date string. */
typedef struct sal_date_s {
    char buf[SAL_DATE_BUF_MAX]; /*!< Date string. */
} sal_date_t;

/*!
 * \brief Get time and date string.
 *
 * This function should return the local system time as a
 * null-terminated ASCII string.
 *
 * The output is intended for informational messages only.
 *
 * \param [out] date Date string structure.
 *
 * \return Pointer to date string (normally \c date->buf).
 */
extern char *
sal_date(sal_date_t *date);

#endif /* SAL_DATE_H */
