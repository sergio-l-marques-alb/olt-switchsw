/*
 * Release: R6_2_0
 *
 * EmWeb POSIX Reference Port
 *   Definitions of routines to be provided by the application
 *   for the POSIX port (not for the entire EmWeb/Server).
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
 */

#ifndef _EWA_ROUTINES_H_
#define _EWA_ROUTINES_H_

#ifndef _EWA_ROUTINES_C_
#   define _EXTERN_EWA_ROUTINES extern
#else
#   define _EXTERN_EWA_ROUTINES
#endif

#if defined(_WIN32_WCE) || defined(_WIN32_WCE_DEMO)
#   define _DLLEXPORT   __declspec( dllexport )
#else
#   define _DLLEXPORT
#endif

#ifndef _EWMAIN_C_
#   if defined(_WIN32_WCE) || defined(_WIN32_WCE_DEMO)
#       define _EXTERN_EMAIN __cdecl extern
#   else
#       define _EXTERN_EMAIN extern
#   endif
#else
#   define _EXTERN_EMAIN
#endif

/* **************************************************************** */
/*                         ewmain.c routines                        */
/* **************************************************************** */

_DLLEXPORT
_EXTERN_EMAIN
int
ewmain( void );

/*
 * ewaStartSlowShutdown()
 * Call this function to initiate slow server shutdown - don't
 * accept new connections and exit the main loop when all existing
 * connections close
 */
_DLLEXPORT
_EXTERN_EMAIN
void ewaStartSlowShutdown( void );

/* **************************************************************** */
/*                          ewsys.c routines                        */
/* **************************************************************** */

/*
 * ewaTick is called once per tick by the server main loop (ewmain.c)
 */
_EXTERN_EWA_ROUTINES
void ewaTick( void );

/*
 * ewaInstallArchives
 *   Called at init time - it should make any calls
 *   to ewsDocumentInstallArchive that are needed.
 */
_EXTERN_EWA_ROUTINES
EwaStatus ewaInstallArchives( void );

/*
 * ewaConnectionInit
 *   Called when a new connection is allocated by the POSIX main loop.
 *   Should initialize any application-specific field in the net handle.
 */
_EXTERN_EWA_ROUTINES
void ewaConnectionInit( EwaNetHandle connection );

/*
 * ewaArchiveDate
 *   Return date on which the archive was compiled
 */
_EXTERN_EWA_ROUTINES
const char* ewaArchiveDate( void );

#ifdef EW_CONFIG_OPTION_SMTP
/*
 * EmWeb/Mail support - this is an optional package
 */

_EXTERN_EWA_ROUTINES
void ewaSMTPConfigure( uint32      ip     /* 0    == leave unchanged  */
                      ,uint16      port   /* 0    == use default (25) */
                      ,const char* domain /* NULL == leave unchanged  */
                      );

_EXTERN_EWA_ROUTINES
void ewaSMTPGetAddress( uint32*      ip
                       ,uint16*      port
                       );

/*
 * ewaSMTPStart
 * This function is called by the EmWeb/Mail package to initiate a TCP
 * connection with the SMTP mail gateway.  We simply create a socket,
 * connect to the gateway, and return a network handle.
 */
_EXTERN_EWA_ROUTINES
EwaNetHandle ewaSMTPStart( EwsContext context, EwaSMTPHandle handle );
/* returns new network handle, or EWA_NET_HANDLE_NULL on error */

/*
 * ewaSMTPDomainName
 * This function simply returns the name of the SMTP domain.
 */
_EXTERN_EWA_ROUTINES
char* ewaSMTPDomainName ( EwsContext context, EwaSMTPHandle handle );
#endif /* EW_CONFIG_OPTION_SMTP */

#ifdef EW_CONFIG_OPTION_IN_FLOW
/*
 * ewaNetFlowControl and ewaNetUnFlowControl
 * Regulate inbound buffers - the application may call these when,
 * for example, suspending processing of a context to prevent TCP
 * from passing more buffers in to the server.  TCP flow control
 * will automatically propogate this to the sender.
 *
 * The EmWeb/Server does not call these itself.
 */
_EXTERN_EWA_ROUTINES
void ewaNetFlowControl( EwaNetHandle connection );

_EXTERN_EWA_ROUTINES
void ewaNetUnFlowControl( EwaNetHandle connection );
#endif /* EW_CONFIG_OPTION_IN_FLOW */


/*
 *** Local Variables: ***
 *** mode: c ***
 *** tab-width: 4 ***
 *** End: ***
 */
#endif /* _EWA_ROUTINES_H_ */
