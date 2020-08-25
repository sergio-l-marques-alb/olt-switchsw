/*
 * Product: EmWeb
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 * EmWeb/Server Distributed Primary/Secondary Interfaces
 *
 */
#ifndef _EWS_DIST_H_
#define _EWS_DIST_H_

#include "ew_types.h"
#include "ew_config.h"
#include "ews_def.h"

#ifdef EW_CONFIG_OPTION_PRIMARY

typedef struct EwsSecondaryHandle_s *EwsSecondaryHandle;
#define EWS_SECONDARY_HANDLE_NULL ((EwsSecondaryHandle)NULL)

/*
 * ewsSecondaryRegister
 * Register a secondary with the primary -- exchange handles.   This connection
 * will not be used by EmWeb/Server until the application indicates that it is
 * open by calling ewsSecondaryOpened().
 */
EwsSecondaryHandle
ewsSecondaryRegister ( EwaSecondaryHandle handle );

/*
 * ewsSecondaryUnregister
 * Unregister a previously registered secondary.  If any requests are waiting
 * for the secondary (because the connection is not open), then the
 * ewaURLHook() will be reinvoked for these requests.  This gives the
 * application a second chance to redirect to a different secondary, or to
 * handle the error locally.
 *
 * Following this call, the given EwsSecondaryHandle value is no longer
 * valid and all resources associated with it will have been released.
 */
EwsStatus
ewsSecondaryUnregister ( EwsSecondaryHandle handle );

/*
 * ewsSecondaryOpened
 * Notify EmWeb that a secondary connection has opened
 */
EwsStatus
ewsSecondaryOpened ( EwsSecondaryHandle handle );

/*
 * ewsSecondaryClosed
 * Notify EmWeb that a secondary connection has closed
 */
EwsStatus
ewsSecondaryClosed ( EwsSecondaryHandle handle );

/*
 * ewsSecondaryRedirect
 * Called by application's ewaURLHook() to redirect a request to a secondary
 * for processing.  In addition to this redirection, the application may
 * rewrite the URL as usual.
 */
EwsStatus
ewsSecondaryRedirect ( EwsContext context, EwsSecondaryHandle handle );

/*
 * ewsSecondaryData
 * Pass data received from secondary connection to primary.  This data is
 * forwarded to the corresponding user agent.
 */
EwsStatus
ewsSecondaryData ( EwsSecondaryHandle handle    /* secondary handle */
                  ,const uint8        *datap    /* pointer to buffer */
                  ,uintf              bytes     /* size of buffer */
                 );

/*
 * ewaSecondaryData
 * Application-provided function invoked by EmWeb/Server to forward data
 * to the secondary.
 */
sintf
ewaSecondaryData ( EwaSecondaryHandle handle    /* secondary handle */
                  ,EwsContext         context   /* current request */
                  ,const uint8        *datap    /* pointer to buffer */
                  ,uintf              bytes     /* size of buffer */
                 );

/*
 * ewaSecondaryFlowControl
 * Application-provided function invoked by EmWeb/Server to inhibit further
 * data from the secondary.
 */
void
ewaSecondaryFlowControl ( EwaSecondaryHandle handle );

/*
 * ewaSecondaryUnFlowControl
 * Application-provided function invoked by EmWeb/Server to resume previously
 * flow controlled connection to the secondary.
 */
void
ewaSecondaryUnFlowControl ( EwaSecondaryHandle handle );

/*
 * ewaSecondaryClose
 * Application-provided function invoked by EmWeb/Server to indicate that
 * a secondary connection must be closed (and eventually reopened).
 */
EwaStatus
ewaSecondaryClose ( EwaSecondaryHandle handle );

#endif /* EW_CONFIG_OPTION_PRIMARY */

#endif /* _EWS_DIST_H_ */
