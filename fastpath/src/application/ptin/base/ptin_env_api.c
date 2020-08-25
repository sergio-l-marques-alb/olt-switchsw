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

static unsigned int _board_hwver = 0;

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

    str = getenv("BOARD_HWVER");
    if (NULL == str)
    {
        PT_LOG_CRITIC(LOG_CTX_STARTUP, "BOARD_HWVER not found");
        return L7_FAILURE;
    }

    _board_hwver = atoi(str);//strtol(str, NULL, 10);

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
L7_uint32 ptin_env_board_hwver(void)
{
    return _board_hwver;
}

