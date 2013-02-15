/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    keypair.h
* @purpose     API's and data structures of statsconfig.c
* @component   stats
* @comments    
* @create      18/06/2007
* @author      YSaritha 
* @end
*             
**********************************************************************/


#ifndef INCLUDE_STATS_CONFIG_H
#define INCLUDE_STATS_CONFIG_H

typedef struct StatsConfigurator_s
{
  Collector *pCollector;
  Presenter *pPresenter;
} StatsConfigurator;

L7_RC_t stat_statsCfgInit (StatsConfigurator * pStatsCfg);

#endif /* INCLUDE_STATS_CONFIG_H  */
