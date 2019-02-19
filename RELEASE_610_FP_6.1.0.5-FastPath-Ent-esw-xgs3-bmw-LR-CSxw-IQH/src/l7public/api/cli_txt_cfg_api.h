/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 * @filename   cli_web_util.h
 *
 * @purpose    Cli Text configuration API file
 *
 * @component  cliWebUI component
 *
 * @comments   none
 *
 * @create     07/09/2007
 *
 * @author     M.Sudhir
 *
 * @end
 *
 **********************************************************************/

#ifndef CLI_TXT_CFG_API_H
#define CLI_TXT_CFG_API_H

#include "l7_common.h"

/* Data structure to hold the interested components information to be
 notified after text based config apply completion. */
typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID;
  L7_uint32 (*notify_txt_cfg_apply_complete)(L7_uint32 event);
} txtCfgApplyNotifyList_t;

typedef enum
{
 TXT_CFG_APPLY_FAILURE, /* Config apply failure event */
 TXT_CFG_APPLY_SUCCESS  /* Config apply success event */
}txtCfgApplyEvent_t;


/* Begin Function Prototypes */

/*********************************************************************
 * @purpose  Registers the interested components information to inform
 *           the config apply completion
 *
 * @param    registrar_ID: component ID who is interested
 *           notify      : The function name that will be called
 *
 * @returns  L7_SUCCESS  On proper registration
 *           L7_FAILURE  On registration failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t txtCfgApplyCompletionNotifyRegister(L7_uint32 registrar_ID,
                              L7_uint32 (*notify)(L7_uint32 event));

/*********************************************************************
 * @purpose  Deregisters the interested components information to inform
 *           the config apply completion
 *
 * @param    registrar_ID: component ID who is interested
 *
 * @returns  L7_SUCCESS  On proper registration
 *           L7_FAILURE  On registration failure
 *
 * @notes    none
 *
 * @end
 *********************************************************************/
L7_RC_t txtCfgApplyCompletionNotifyDeregister(L7_uint32 registrar_ID);

/*********************************************************************
*
* @purpose Returns whether the configuration is to be skipped
*
*
* @param void
*
* @returntype L7_BOOL
*
* @returns    L7_FALSE  if the configuration is to be applied.
*             L7_TRUE   if the configuration is not to be applied.
*
* @end
*
*********************************************************************/
L7_BOOL cliGlobalConfigurationSkipGet( void );

/*********************************************************************
*
* @purpose Sets whether the configuration is to be skipped
*
*
* @param   skip   @((input))  L7_TRUE if config application to be skipped
*
* @returntype none
*
* @end
*
*********************************************************************/
void cliGlobalConfigurationSkipSet( L7_BOOL skip );

/* End Function Prototypes */
#endif /* CLI_TXT_CFG_API_H */

