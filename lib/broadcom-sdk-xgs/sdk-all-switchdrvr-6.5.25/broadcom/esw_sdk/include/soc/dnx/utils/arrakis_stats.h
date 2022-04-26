/*
 * $Id: arrakis_stats.h
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * Description: arrakis_stats.h and arrakis_stats.c implements API for storing data in an SQL DB to be later used in Arrakis Web Application Stats module.
 * Purpose: This API is a wrapper for dnxc_regression_utils_print(). It relieves the user from having to
 *            format the samples string. Also providing validation to check that user input conforms with the Stat Family description in stats_syntax.xml.
 */

#ifndef ARRAKIS_STATS_H_INCLUDED
#define ARRAKIS_STATS_H_INCLUDED

#include <sal/types.h>  /* For uint32 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>

/*
 * \brief
 * Maximum length of a string consisting of dimension name followed by all of its samples
 */
#define ARRAKIS_STATS_DIMENSION_AND_SAMPLES_STR_MAX_LEN             200
#define ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN                20
#define ARRAKIS_STATS_SAMPLES_STR_MAX_LEN                             ARRAKIS_STATS_DIMENSION_AND_SAMPLES_STR_MAX_LEN - ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN
#define ARRAKIS_STATS_FAMILY_NAME_MAX_LEN                             100
#define ARRAKIS_STATS_DIMENSIONS_MAX_NOF                            10
#define ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN                        50

/**
 * \brief
 * Holds data to be committed to SQL DB via dnxc_regression_utils_print()
 */
typedef struct
{
    /*
     * \brief
     * The Stat Family name as described in stats_syntax.xml. Input by user via arrakis_stat_family_init()
     */
    char name[ARRAKIS_STATS_FAMILY_NAME_MAX_LEN];
    /*
     * \brief
     * Dimensions names are filled by the user.
     * Note that dimensions names must be the same set of dimensions that are described in stats_syntax.xml (Except Date dimension which should not be filled)
     */
    char dimensions[ARRAKIS_STATS_DIMENSIONS_MAX_NOF][ARRAKIS_STATS_DIMENSION_NAME_MAX_LEN];
    /*
     * \brief
     * Dimension may or may not have a parsing name. Filled without user input by parsing stats_syntax.xml
     */
    char dimensions_parsing_name[ARRAKIS_STATS_DIMENSIONS_MAX_NOF][ARRAKIS_STATS_DIMENSION_PARSING_NAME_MAX_LEN];
    /*
     * \brief
     * A counter to track number of dimensions already filled by user. Updates without user intervention whenever a new dimension is added via arrakis_stat_family_fill() 
     * or arrakis_stat_family_multiple_fill().
     */
    int dimensions_nof;
    /*
     * \brief
     * Each array is a set of samples that is associated with a dimension with the same index.
     * For example, samples[2] is the set of samples associated with dimensions[2].
     * samples are filled with arrakis_stat_family_fill() or arrakis_stat_family_multiple_fill().
     */
    char samples[ARRAKIS_STATS_DIMENSIONS_MAX_NOF][ARRAKIS_STATS_SAMPLES_STR_MAX_LEN];
    /*
     * \brief
     * Each value in the array is the current number of samples associated with the dimension of the same index. Updates automatically.
     * When calling arrakis_stat_family_commit(), number of samples for each dimension must be equal.
     */
    int samples_nof[ARRAKIS_STATS_DIMENSIONS_MAX_NOF];
} arrakis_stat_family_t;

/**
* \brief
*  Init stat_family
* \param [in] unit - Device id
* \param [in] stat_family - a pointer to an empty arrakis_stat_family_t
* \param [in] stat_family_name - The Stat Family name from stats_syntax.xml
* \return
*   shr_error_e - Error Type
* \remark
*   * If stat_family_name does not exists in stats_syntax.xml, arrakis_stat_family_commit() will fail
*/
shr_error_e arrakis_stat_family_init(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *stat_family_name);

/**
* \brief
*  Add a single sample of a single dimension
* \param [in] unit  - Device id
* \param [in] stat_family - A pointer to a stat_family struct that was initiated via arrakis_stat_family_init()
* \param [in] dimension - Dimension name existing in the Stat Family description in stats_syntax.xml
* \param [in] sample - A single sample to be associated with the dimension.
* \return
*   shr_error_e - Error Type
*/
shr_error_e arrakis_stat_family_float_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    float sample);

/**
 * \brief
 * See arrakis_stat_family_float_fill documentation
*/
shr_error_e arrakis_stat_family_int_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    int sample);

/**
 * \brief
 * See arrakis_stat_family_float_fill documentation
*/
shr_error_e arrakis_stat_family_uint32_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char *dimension,
    uint32 sample);

/**
* \brief
*  Add multiple samples for multiple dimensions
* \param [in] unit  - Device id
* \param [in] stat_family  - A pointer to a stat_family struct that was initiated via arrakis_stat_family_init()
* \param [in] dimensions - Dimension names existing in the Stat Family description in stats_syntax.xml.
*                           Same dimension can appear more than once.
* \param [in] dimensions_nof - Number of dimensions (and also samples).
* \param [in] samples  - An array of samples. Each samples associates with the dimension with the same index.
*                         For example, Sample[2] associates with dimension[2] 
* \return
*   shr_error_e - Error Type
*/
shr_error_e arrakis_stat_family_float_multiple_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    float *samples);

/**
 * \brief
 * See arrakis_stat_family_float_multiple_fill documentation
*/
shr_error_e arrakis_stat_family_int_multiple_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    int *samples);

/**
 * \brief
 * See arrakis_stat_family_float_multiple_fill documentation
*/
shr_error_e arrakis_stat_family_uint32_multiple_fill(
    int unit,
    arrakis_stat_family_t * stat_family,
    char **dimensions,
    int dimensions_nof,
    uint32 *samples);

/**
* \brief
*      Commit all samples that were filled previously in stat_family to DB.
* \param [in] unit - Device id
* \param [in] stat_family - a arrakis_stat_family_t struct filled with list of dimensions and samples
* \return
*   shr_error_e - Error Type
* \remark
*   * Below conditions must be met:
*        - stat_family_name input in arrakis_stat_fmaily_init() exists in stats_syntax.xml
*        - At least one dimension and one sample input via arrakis_stat_family_fill or arrakis_stat_family_mulitple_fill
*        - All dimensions input via *_fill() APIs exists in the Stat Family description in stats_syntax.xml
*        - Number of samples of each dimensions added via *_fill() APIs are equal across all dimensions
*        - All data provided by the user do not exceeds the size limit according to the defines at the top of arrakis_stats.h
*/
shr_error_e arrakis_stat_family_commit(
    int unit,
    arrakis_stat_family_t * stat_family);

#endif
