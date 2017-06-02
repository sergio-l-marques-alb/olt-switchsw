/*
 * Product:     EmWeb/Server
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
 *      Interfaces to the skeleton serve services of EmWeb
 *
 */

#ifndef _EWS_SKEL_H_
#define _EWS_SKEL_H_  1  /* version # */


#ifndef _EWS_SKEL_C_
#   define _EXTERN_EWS_SKEL extern
#else
#   define _EXTERN_EWS_SKEL
#endif

/* ================================================================
 * Types & Constants
 * ================================================================ */

/* ================================================================
 * External Data
 * ================================================================ */

/* ================================================================
 * Interfaces
 * ================================================================ */

_EXTERN_EWS_SKEL
void ewsServeSkeletonBeginConvert( EwsContext context );

_EXTERN_EWS_SKEL
void ewsServeSkeletonEndConvert( EwsContext context );

_EXTERN_EWS_SKEL
int32 ewsServeSkeletonCDATABegin( EwsContext context
                                                                  ,EwsDocumentContextP doc_context
                                                                  ,int32 available
                                                                  );

_EXTERN_EWS_SKEL
void ewsServeSkeletonCDATAEnd( EwsContext context
                                                           ,EwsDocumentContextP doc_context
                                                           );

_EXTERN_EWS_SKEL
boolean ewsServeSkeletonEmWebString( EwsContext context
                                     ,EwsDocumentContextP doc_context
                                     );

_EXTERN_EWS_SKEL
boolean ewsServeSkeletonEmWebNamespace( EwsContext context
                                        ,EwsDocumentContextP doc_context
                                        );

/* ewsContextServeSkeleton and ewsContextServingSkeleton are declared in
 * include/ews_ctxt.h. */


/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */
#endif /* _EWS_SKEL_H_ */
