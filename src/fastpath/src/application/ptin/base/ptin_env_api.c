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
#include <stdlib.h>
#include "ptin_env_api.h"
#include "logger.h"

static unsigned int _board_config_mode = 0;

/**
 * Read all environment variables at the beginning
 * 
 * @author mruas (14/06/19)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_env_init(void)
{
    char *str;

    PT_LOG_NOTICE(LOG_CTX_STARTUP, "Getting shell variable BOARD_HWVER...");

    str = getenv("BOARD_CONFIG_MODE");
    if (NULL == str)
    {
        _board_config_mode = 0;
        PT_LOG_WARN(LOG_CTX_STARTUP, "BOARD_HWVER not found");
    }
    else
    {
        _board_config_mode = atoi(str);//strtol(str, NULL, 10);
    }

    /* Board config mode can only be 0, 1 or 2 */
    if (_board_config_mode > 2)
    {
        _board_config_mode = 0;
    }
    
    PT_LOG_NOTICE(LOG_CTX_STARTUP, "BOARD_HWVER: %u", _board_config_mode);

    return L7_SUCCESS;
}

/**
 * Return board HW version
 * 
 * @author mruas (14/06/19)
 * 
 * @param void 
 * 
 * @return unsigned int : HW version
 */
L7_uint32 ptin_env_board_config_mode_get(void)
{
    return _board_config_mode;
}

