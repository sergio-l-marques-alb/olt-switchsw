/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    statsconfig.c
* @purpose     Implements the initialisation API's of stats comp
* @component   stats
* @comments    Provides an interface to collector from client
* @create      18/06/2007
* @author      YSaritha 
* @end
*             
**********************************************************************/

#include <string.h>
#include <stdio.h>

#include "commdefs.h"           /* lvl7 common definitions   */
#include "datatypes.h"          /* lvl7 data types definition */
#include "osapi.h"              /* lvl7 operating system apis */
#include "statsapi.h"           /* stats public methods */
#include "counter64.h"
#include "collector.h"
#include "presenter.h"
#include "statsconfig.h"
#include "log.h"


/* Removeme */

Collector *collKey;

/*
**********************************************************************
*
* @function     statsCfgInit
*
* @purpose      This function allows the client to initialize the Statistics
*               Manager
*          
* @parameters   none
*
* @return       L7_SUCCESS     The Statistics Manager could be successfully 
*                              initialized
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     The Statistics Manager could not be successfully
*                              initialized
*
* @notes        Implemented in src\application\stats\public\statsconfig.c
*
* @end
**********************************************************************
*/
L7_RC_t stat_statsCfgInit (StatsConfigurator * pStatsCfg)
{
  pStatsCfg->pCollector = (Collector *) osapiMalloc (L7_STATSMGR_COMPONENT_ID, sizeof (Collector));
  collKey = pStatsCfg->pCollector;
  if (pStatsCfg->pCollector == L7_NULL)
  {
    printf ("error!! pStatsCfg->pCollector is NULL\n");
    return L7_FAILURE;
  }
  memset (pStatsCfg->pCollector, 0, sizeof (*pStatsCfg->pCollector));
  collector_init (pStatsCfg->pCollector);
  pStatsCfg->pPresenter = (Presenter *) osapiMalloc (L7_STATSMGR_COMPONENT_ID, sizeof (Presenter));


  if (pStatsCfg->pPresenter == L7_NULL)
  {
    printf ("error!! pStatsCfg->pPresenter is NULL\n");
    return L7_FAILURE;
  }
  memset (pStatsCfg->pPresenter, 0, sizeof (*pStatsCfg->pPresenter));
  pStatsCfg->pPresenter->pCollector = pStatsCfg->pCollector;

  return L7_SUCCESS;
}
