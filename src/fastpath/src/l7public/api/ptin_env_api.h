/*********************************************************************
*
* (C) Copyright Altice Labs 2019
*
**********************************************************************
*
* @filename ptin_env.h
*
* @purpose API to get environment variables contents
*
* @component env
*
* @comments none
* 
* @create    14/06/2019
*
* @author    Milton Ruas
* 
* @end
**********************************************************************/
#ifndef _PTIN_ENV_API_H
#define _PTIN_ENV_API_H

#include "ptin_globaldefs.h"

/**
 * Read all environment variables at the beginning
 * 
 * @author mruas (14/06/19)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
extern 
L7_RC_t ptin_env_init(void);

/**
 * Return board HW version
 * 
 * @author mruas (14/06/19)
 * 
 * @param void 
 * 
 * @return unsigned int : HW version
 */
extern 
L7_uint32 ptin_env_board_config_mode_get(void);

#endif /* _PTIN_ENV_API_H */
