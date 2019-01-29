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
 * EmWeb/Server url hooks definitions
 *
 */

#ifndef _EWS_MULTIHOOKS_H_
#define _EWS_MULTIHOOKS_H_

#ifdef EW_CONFIG_OPTION_URL_HOOK_LIST

/*
 * EwaUrlHook
 *   Prototype for a URL Hook function.
 *   These are registered with the ewsUrlHookRegister routine to be
 *   added to the end of the list of active URL rewrite hooks.
 */
typedef char* EwaUrlHook( EwsContext context, char* url );

/*
 * ewsUrlHookRegisterPosition
 *   Adds a URL Hook function to the URL Hook List.
 *   The location in the list to add it can be specified by the user.
 *   You should not register a function that is already registered as a URL
 *   Hook, even though that is semantically reasonable.  Use a wrapper function
 *   to ensure that each registration can be distinguished in
 *   ewsUrlHookUnRegister.
 *   Can be called from within a URL Hook.
 */
typedef enum {
  ewsHookPositionFirst,
  ewsHookPositionLast,
  ewsHookPositionBefore,
  ewsHookPositionAfter
} EwsHookPosition;

EwsStatus ewsUrlHookRegisterPosition( EwaUrlHook*     urlHook
                                     ,EwsHookPosition position
                                     ,EwaUrlHook*     base
                                    );

/*
 * ewsUrlHookRegister
 *   Adds a URL Hook function to the end of the URL Hook List.
 */
#define ewsUrlHookRegister(urlHook)  \
        ewsUrlHookRegisterPosition( urlHook, ewsHookPositionLast, \
                                    (EwaUrlHook *) NULL )

/*
 * ewsUrlHookUnRegister
 *   Removes a URL Hook function from the URL Hook List.
 *   Suspended requests whose next action is to call the URL Hook function
 *   being deleted will aborted.
 *   (This may not be semantically perfect, but it tends to eliminate error-
 *   prone cases.  And the higher levels of the client have to deal with
 *   unexpectedly aborted requests anyway.)
 *   Can be called from within a URL hook (ugh), but a hook must not
 *   unregister itself.
 */
EwsStatus ewsUrlHookUnRegister( EwaUrlHook* urlHook );

/*
 * ewsUrlHookContinue
 *   Called from within a URL Hook function to indicate that URL Hook
 *   processing should continue with the next URL Hook function on the list,
 *   despite that this URL Hook function will be returning a non-NULL value
 *   (which gives the URL that this function translated the URL into).
 */
void ewsUrlHookContinue( EwsContext context );

/*
 * ewsUrlHook
 *   Calls the routines on the URL Hook List until one returns
 *   a pointer not equal to NULL; this value is then returned by
 *   this routine.
 */
char* ewsUrlHook( EwsContext context, char* url );

#endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */
#endif /* _EWS_MULTIHOOKS_H_ */
