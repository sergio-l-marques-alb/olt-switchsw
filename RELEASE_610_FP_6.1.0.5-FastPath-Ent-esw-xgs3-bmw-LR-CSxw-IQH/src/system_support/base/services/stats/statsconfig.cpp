/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename statsconfig.cpp
*
* @purpose The purpose of this file is to implement the 
*		   StatsConfigurator class
*
* @component Statistics Manager
*
* @comments none
*
* @create 10/09/2000
*
* @author Archana Suthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



   
#include <statsinclude.h>


/*
**********************************************************************
*
* @function     statsCfgInit
*
* @purpose      This function allows the client to initialize the Statistics
*		        Manager
*          
* @parameters   none
*
* @return       L7_SUCCESS     The Statistics Manager could be successfully 
*					           initialized
* @return       L7_ERROR       Not Applicable
* @return       L7_FAILURE     The Statistics Manager could not be successfully
*					           initialized
*
* @notes        Implemented in src\application\stats\public\statsconfig.c
*
* @end
**********************************************************************
*/
L7_RC_t StatsConfigurator :: statsCfgInit ( )
{
    pCollector = new Collector(); 			// Instantiates a Collector
	pPresenter = new Presenter(pCollector);	// Instantiates a Presenter

	if (pCollector != NULL && pPresenter != NULL) 
		return L7_SUCCESS;  // Both the Presenter & Collector were 
							// successfully instantiated.

	else return L7_FAILURE; // Instantiation did not take place as 
							// expected.
}


