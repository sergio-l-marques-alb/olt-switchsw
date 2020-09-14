/** \file dnxc_regression_utils.h
 *
 * Add information to regression DB.
 *
 */
#ifndef __DNXC_REGRESSION_UTILS_H__
#define __DNXC_REGRESSION_UTILS_H__

/*
 * Maximum string len.
 */
#define DNXC_REGRESSION_UTILS_STRING_LEN 1024

/*
 * Macro to print the relevant data with user specified format.
 */
#define DNXC_REGRESSION_UTILS_ADVANCED_PRINT(label, format, ...) \
    do { \
        dnxc_regression_utils_advanced_print(label, format, __VA_ARGS__); \
    } while(0)

/**
* \brief
*   Prints the relevant data with pre-defined format, so the regression can add it to the DB
*
*  \param [in] label - Label.
*  \param [in] data  - Data.
*
*  \return
*    None
*  \remark
*    None
*  \see
*    None
*****************************************************/
void dnxc_regression_utils_print(
    char *label,
    char *data);

/**
* \brief
*   Prints the relevant data with used specified data format.
*
*  \param [in] label - Label.
*  \param [in] format  - Data format.
*
*  \return
*    None
*  \remark
*    None
*  \see
*    None
*****************************************************/
void dnxc_regression_utils_advanced_print(
    char *label,
    char *format,
    ...);

#endif /* __DNXC_REGRESSION_UTILS_H__ */
