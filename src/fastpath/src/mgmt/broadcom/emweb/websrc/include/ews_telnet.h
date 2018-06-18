/*
 * Product: EmWeb/Telnet
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
 * EmWeb/Telnet application interfaces
 *
 */

#ifndef _EWS_TELNET_H_
#define _EWS_TELNET_H_ 1 /* version */

#include <stdarg.h>

#include "ew_types.h"
#include "ew_config.h"

#ifdef EW_CONFIG_OPTION_TELNET
/*
 * ewsNetTelnetStart
 * Use instead of ewsNetHTTPStart to signal a telnet connection.
 *
 *   handle - network handle
 *   issue  - issue message
 *   prompt - prompt to use after send replies
 *   flags  - flags (0 = default, or one or more flags below)
 */
#      define EWS_TELNET_SERIAL_PORT  1  /* direct serial connection */
/*
 * Returns context, or NULL on error
 */
EwsContext ewsNetTelnetStart ( EwaNetHandle handle
                              ,const char   *issue
                              ,const char   *prompt
                              ,uintf        flags );

/*
 * ewaNetTelnetData
 * Called to pass raw telnet data up to application.
 *
 *   handle - network handle
 *   data   - pointer to data
 *   bytesp - (input/output) pointer to number of bytes, decremented by
 *            number of bytes consumed.
 *
 * Returns string to write back to telnet.  May also call
 * ewsContextSendReply(), in which case the return value is ignored.
 */
char * ewaNetTelnetData( EwaNetHandle handle
                        ,char         *datap
                        ,uintf        *bytesp );

/*
 * ewsSetTelnetPrompt
 * Change the prompt to use after send reply
 *
 *   context - request context
 *   prompt  - new prompt
 */
#define ewsNetTelnetPrompt ewsSetTelnetPrompt /* backward compatibility */

void ewsSetTelnetPrompt ( EwsContext context, const char *prompt );

void ewsSetTelnetStackingTrapMsg(EwsContext context, const char *trapString);

/*
 * ewsGetTelnetPrompt
 * Read current prompt
 *
 * context - request context
 */
const char *ewsGetTelnetPrompt ( EwsContext context );

/* ewsTelnetWritePaginate : Only used in EWSWRITEBUFFER_MAIN for show config
 * Write text to client
 *
 * context - request context
 * text    - null terminated string of content to write to port
 */
void
ewsTelnetWritePaginate ( EwsContext context, const char * text );

/*
 * ewsTelnetWrite
 * Write text to client
 *
 * context - request context
 * text    - null terminated string of content to write to port
 */
void ewsTelnetWrite ( EwsContext context, const char * text );
void ewsTelnetPrintf ( EwsContext context, const char * fmt,... );

/*
 * ewsTelnetWriteDataInsert
 * Write third party text to client.
 *
 * context - request context
 * text    - null terminated string of content to write to port
 */
void
ewsTelnetWriteDataInsert ( EwsContext context, const char * text );

/*
 * ewsTelnetSessionValidate
 * Determine if the telnet session pointer is valid.
 *
 * context      - Context of request
 */
boolean
ewsTelnetSessionValidate ( EwsContext context );

#endif /* EW_CONFIG_OPTION_TELNET */

#endif /* _EWS_TELNET_H_ */
