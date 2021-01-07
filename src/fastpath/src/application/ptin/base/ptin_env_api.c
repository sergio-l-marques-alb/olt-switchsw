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

/* Assume V1 when environment variable isn't there*/
static unsigned int _board_hwver = 1;

/* Assume board mode  when environment variable isn't there*/
static unsigned int _board_mode = 1;

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

    PT_LOG_NOTICE(LOG_CTX_STARTUP, "Getting shell variable %s...",
                  BOARD_HWVER_STR);

    str = getenv(BOARD_HWVER_STR);
    if (NULL == str || '\0'==str[0])
    {
        PT_LOG_CRITIC(LOG_CTX_STARTUP, "%s not found", BOARD_HWVER_STR);
#if 1  /*(PTIN_BOARD == PTIN_BOARD_TC16SXG)*/
        /* Assume V1 when environment variable isn't there (check definition up)
           (Agreed with FW_CTRL, as some V1 have this problem.)*/
        _board_hwver = 1;
#endif
        if (NULL != str) 
        {
            PT_LOG_NOTICE(LOG_CTX_STARTUP,
                          "\"%s\" = getenv(\"%s\") => _board_hwver=%u",
                          str, BOARD_HWVER_STR, _board_hwver);
        }
        else 
        {
            PT_LOG_NOTICE(LOG_CTX_STARTUP, "assuming _board_hwver=%u",
                          _board_hwver);
        }

        return L7_FAILURE;
    }

    _board_mode = atoi(str);
    PT_LOG_NOTICE(LOG_CTX_STARTUP, "\"%s\" = getenv(\"%s\") => _board_mode=%u",
                  str, BOARD_HWVER_STR, _board_mode);

    str = getenv(BOARD_MODE_STR);
    if (NULL == str || '\0'==str[0])
    {
        PT_LOG_CRITIC(LOG_CTX_STARTUP, "%s not found", BOARD_MODE_STR);

        _board_mode = 1;
        if (NULL != str)
        {
            PT_LOG_NOTICE(LOG_CTX_STARTUP,
                          "\"%s\" = getenv(\"%s\") => _board_mode=%u",
                          str, BOARD_MODE_STR, _board_mode);
        }
        else 
        {
            PT_LOG_NOTICE(LOG_CTX_STARTUP, "assuming _board_mode=%u",
                          _board_mode);
        }

        return L7_FAILURE;
    }

    _board_mode = atoi(str);
    PT_LOG_NOTICE(LOG_CTX_STARTUP, "\"%s\" = getenv(\"%s\") => _board_mode=%u",
                  str, BOARD_MODE_STR, _board_mode);

    return L7_SUCCESS;
}


/**
 * Read board_mode environment variable
 *   
 * @author rfernandes (14/06/19)
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_env_board_mode_read(void)
{
    char *str;

    PT_LOG_NOTICE(LOG_CTX_STARTUP, "\"%s\" = getenv(\"%s\") => _board_mode=%u",
                  str, BOARD_HWVER_STR, _board_mode);

    str = getenv(BOARD_MODE_STR);
    if (NULL == str || '\0'==str[0])
    {
        PT_LOG_CRITIC(LOG_CTX_STARTUP, "%s not found", BOARD_MODE_STR);

        _board_mode = 1;
        if (NULL != str)
        {
            PT_LOG_NOTICE(LOG_CTX_STARTUP,
                          "\"%s\" = getenv(\"%s\") => _board_mode=%u",
                          str, BOARD_MODE_STR, _board_mode);
        }
        else 
        {
            PT_LOG_NOTICE(LOG_CTX_STARTUP, "assuming _board_mode=%u",
                          _board_mode);
        }

        return L7_FAILURE;
    }

    _board_mode = atoi(str);
    PT_LOG_NOTICE(LOG_CTX_STARTUP, "\"%s\" = getenv(\"%s\") => _board_mode=%u",
                  str, BOARD_MODE_STR, _board_mode);

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


/**
 * Return board _board_mode
 * 
 * @author rfernandes (06/01/21)
 * 
 * @param void 
 * 
 * @return unsigned int : board_mode
 */
L7_uint32 ptin_env_board_mode_get(void)
{
    return (_board_mode-1);
}

