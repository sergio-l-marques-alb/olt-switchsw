/** \file sw_state_cb.h
 * 
 * $Id: $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * INFO: Sw State CB bd 
 *
 */
#ifndef _DNX_SW_STATE_CB_DB_H
/* { */
#define _DNX_SW_STATE_CB_DB_H

/* must be first */
#include <soc/dnxc/swstate/sw_state_features.h>

#include <shared/error.h>
#include <shared/bsl.h>

#define SW_STATE_CB_DB_NAME_STR_SIZE 256

typedef struct
{
    char function_name[SW_STATE_CB_DB_NAME_STR_SIZE];
} sw_state_cb_t;

/**
* \brief
*  The function will print to the screen a representation of the
*  callback
* \param [in] unit    - Device Id 
* \param [in] cb  - callback
* \return
*   int - Error Type
* \remark
*   * none
* \see
*   * None
*/
int sw_state_cb_db_print(
    int unit,
    sw_state_cb_t * cb);

/**
* \brief
*  The function will be used by the ctest swstate callback.
*
* \param [in] unit    - Device Id 
* \return
*   uint8 - 0
* \remark
*   * none
* \see
*   * None
*/
uint8 dnx_sw_state_callback_test_first_function(
    int unit);

/**
* \brief
*  The function will be used by the ctest swstate callback.
*
* \param [in] unit    - Device Id 
* \return
*   uint8 - 0
* \remark
*   * none
* \see
*   * None
*/
uint8 dnx_sw_state_callback_test_second_function(
    int unit);

#define SW_STATE_CB_DB_REGISTER_CB(module_id, cb_db, name, get_cb_function_cb) \
    do { \
        sal_strncpy(cb_db.function_name, name, SW_STATE_CB_DB_NAME_STR_SIZE-1);\
        if(!(get_cb_function_cb(&cb_db, 1, NULL)))\
        {\
            return _SHR_E_NONE;\
        }\
        else\
        {\
            sal_memset(cb_db.function_name, 0, SW_STATE_CB_DB_NAME_STR_SIZE-1);\
            return _SHR_E_NOT_FOUND;\
        }\
    } while(0)

#define SW_STATE_CB_DB_UNREGISTER_CB(module_id, cb) \
    do { \
        sal_memset(cb.function_name, 0, SW_STATE_CB_DB_NAME_STR_SIZE-1);\
    } while(0)

#define SW_STATE_CB_DB_GET_CB(module_id, cb_db, cb, get_cb_function_cb) \
    do { \
        get_cb_function_cb(&cb_db, 0, cb);\
    } while(0)

#endif /* _DNX_SW_STATE_CB_DB_H */
