/*
 * $Id: dnx_pp_programmability_utils.h
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef DNX_PP_PROGRAMMABILITY_UTILS_H_INCLUDED

#define DNX_PP_PROGRAMMABILITY_UTILS_H_INCLUDED

/*
 * \brief - This function returns the current device image
 * \param [in] unit - Device Id
 * \param [out] device_image_name - pointer to string, will hold the device image name
 * \return
 *   int - Error Type
 * \remark
 *   * Must be aligned with dnx_pp_prgm_device_image_name_legacy_get
 * \see
 *   * None
 */
shr_error_e dnx_pp_prgm_device_image_name_get(
    int unit,
    char **device_image_name);

/*
 * \brief - This function returns the current device image using legacy soc property get function
 * \param [in] unit - Device Id
 * \param [out] device_image_name - pointer to string, will hold the device image name
 * \return
 *   int - Error Type
 * \remark
 *   * This function is useful when image name is required before dnx data is ready to be used
 * \see
 *   * None
 */
shr_error_e dnx_pp_prgm_device_image_name_legacy_get(
    int unit,
    char **device_image_name);

/*
 * \brief - This function check if current image is the default image ("standard" image)
 *          If not standard image, it means bare-metal mode.
 * \param [in] unit - Device Id
 * \param [out] is_default - A boolean indication whether image is the standard image or not. True means standard, FALSE means bare-metal
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_pp_prgm_default_image_check(
    int unit,
    uint8 *is_default);

/*
 * \brief - This function check if a given image (string - image_to_check) is the current image.
 * \param [in] unit - Device Id
 * \param [in] image_to_check - an image to check
 * \param [out] is_image - True is image_to_check is the current image, False otherwise
 * \return
 *   int - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_pp_prgm_current_image_check(
    int unit,
    char *image_to_check,
    uint8 *is_image);
#endif
