/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename     statsconfig.h
*
* @purpose      The purpose of this file is to define the StatsConfigurator
*		        class
*
* @component    Statistics Manager
*
* @comments     none
*
* @create       10/09/2000
*
* @author       Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_STATS_CONFIG_H
#define INCLUDE_STATS_CONFIG_H

/*
***********************************************************************
*                STATS CONFIGURATOR CLASS
***********************************************************************
*/

/*
**********************************************************************
*
* @class        StatsConfigurator
*
* @purpose      The StatsConfigurator is responsible for initializing 
*		        individual components of the Statistics Manager.
*
* @attribute    Collector *pCollector
* @attribute    Presenter *pPresenter
*		   
*  
* @members      statsCfgInit  Initializes the Presenter and the Collector 
*
* @note         Implemented in src\application\stats\private\statsconfig.cpp
*
* @end
*
**********************************************************************
*/

class StatsConfigurator
{
// The following functions have to be declared as friends so that they
// can access the private data of the StatsConfigurator.
friend L7_RC_t statsCreate ( L7_uint32 listSize, 
						           pStatsParm_list_t pStatsParmList);
friend L7_RC_t statsGet ( L7_uint32 listSize, 
		     			     pCounterValue_list_t pCounterValueList);
friend L7_RC_t statsReset ( L7_uint32 listSize, 
						   pCounterValue_list_t pCounterValueList);
friend L7_RC_t statsIncrement ( L7_uint32  listSize, 
						  pCounterValue_list_t  pCounterValueList);
friend L7_RC_t statsDecrement ( L7_uint32 listSize, 
		                   pCounterValue_list_t pCounterValueList);
friend L7_RC_t statsMutlingsAdd (L7_uint32 listSize, 
                                 pStatsParm_list_t pStatsParmList);
friend L7_RC_t statsMutlingsDel (L7_uint32 listSize, 
                                 pStatsParm_list_t pStatsParmList);

friend L7_RC_t statsDelete ( L7_uint32 listSize, 
                      pStatsParm_list_t pStatsParmList);
private:
    Collector  *pCollector;  // This is a pointer to the Stats 
						     // Configurator's Collector
	Presenter  *pPresenter;  // This is a pointer to the Stats
							 // Configurator's Presenter

public:
	StatsConfigurator() { // Constructor
    pCollector = NULL;
    pPresenter = NULL;
  }   	  

	virtual ~StatsConfigurator() {} // Destructor

/*
**********************************************************************
*
* @function     statsCfgInit
*
* @purpose      This function allows the client to initialize the Statistics
*		        Manager
*          
* @parameter    None
*
* @return       L7_SUCCESS     The Statistics Manager could be successfully 
*					           initialized
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     The Statistics Manager could not be successfully
*		     		           initialized
*
* @notes        Implemented in src\application\stats\public\statsconfig.c
*
* @end
*
**********************************************************************
*/
L7_RC_t statsCfgInit ( );

};

/*
/------------------------------------------------------------------\
*            END OF STATS CONFIGURATOR DEFINITION                  *
\------------------------------------------------------------------/
*/

#endif	// INCLUDE_STATS_CONFIG_H

