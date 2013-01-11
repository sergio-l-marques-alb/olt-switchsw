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
 *      Serve EmWeb document (probably XML) inside a TEXTAREA hack
 */


#include "ews.h"
#include "ews_sys.h"
#include "ews_serv.h"
#include "ews_ctxt.h"
#include "ews_send.h"

#define _EWS_TAH_C_
#include "ews_tah.h"

/* START-STRIP-EMWEB-LIGHT */

/* ================================================================
 * Types & Constants
 * ================================================================ */

/* ================================================================
 * Static Data
 * ================================================================ */

/* ================================================================
 * Static Function Declarations
 * ================================================================ */

/* ================================================================
 * External Interfaces
 * ================================================================ */

#if defined(EW_CONFIG_OPTION_CONVERT_TEXTAREA)

void
ewsServeTextareaBeginConvert( EwsContext context, EwsDocument document )
{
  /* Escape character as a string. */
  static char textarea_escape_char[2] = { TEXTAREA_ESCAPE_CHAR, 0 };
  EW_UNUSED(document);

  /*
   * No processing if not serving in TEXTAREA mode.
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA))
    {
      return;
    }

  /* This is the version number of the TEXTAREA boilerplate code,
   * represented as a character string. */
# define TEXTAREA_VERSION "1"

  ewsBodySend(context,
"<HTML>\n\
<HEAD>\n\
<SCRIPT LANGUAGE=\"javascript\" SRC=\"/EmWeb/textarea"
                          TEXTAREA_VERSION ".js\"></SCRIPT>\n\
<TITLE></TITLE>\n\
</HEAD>\n\
<BODY onLoad=\"EmWebStart" TEXTAREA_VERSION "(window, this);\">\n\
<DIV ID=\"AsiContentDiv\" class=asictd>\n\
<SPACER TYPE=BLOCK HEIGHT=1700 WIDTH=800></DIV>\n\
<DIV ID=\"AsiXMLDiv\" class=asicd></DIV>\n\
<IFRAME ID=\"AsiXMLIFrame\" NAME=\"AsiXMLIFrame\" class=asicd></IFRAME>\n\
<DIV ID=\"AsiConfigDiv\" class=asicd>\n\
<FORM NAME=\"AsiConfigForm\">\n\
<TEXTAREA>");
  /* Now send (not converted or quoted) the escape character.  This allows
   * the client-side code to decode regardless of what we choose as the
   * escape character. */
  ewsBodySend(context, textarea_escape_char);
  /* Set the flag to tell ewsBody[N]Send to tell ewsString[N]CopyIn
   * to quote special characters in the text. */
  context->flags |= EWS_CONTEXT_FLAGS_BODY_QUOTE;
}

void
ewsServeTextareaEndConvert( EwsContext context )
{
  /*
   * No processing if not serving in TEXTAREA mode.
   */
  if (!(context->flags & EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA))
    {
      return;
    }

  /* Turn off quoting of special characters. */
  context->flags &= ~EWS_CONTEXT_FLAGS_BODY_QUOTE;
  ewsBodySend(context,
"</TEXTAREA>\n\
</FORM>\n\
</DIV>\n\
</BODY>\n\
</HTML>\n");
}


/**********************************************************************
 *
 *  ewsContextServeTextarea
 *
 *  Purpose: causes the current document to be served in a TEXTAREA hack
 *
 *  Inputs:  current context
 *
 *  Outputs: modifies current context
 *
 *  Returns: TRUE on success (always successful at this point).
 *
 */
boolean
  ewsContextServeTextarea( EwsContext context )
{
  context->flags |= EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA;
  return TRUE;
}

/**********************************************************************
 *
 *  ewsContextServingTextarea
 *
 *  Purpose: Boolean check to determine if the current document is
 *          being served in TEXTAREA mode.
 *
 *  Inputs:  current context
 *
 *  Outputs: none
 *
 *  Returns: true if TEXTAREA mode is in effect, else false
 */
boolean
  ewsContextServingTextarea( EwsContext context )
{
  return (context->flags & EWS_CONTEXT_FLAGS_CONVERT_TEXTAREA) != 0;
}
#endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */

/* END-STRIP-EMWEB-LIGHT */


/* ================================================================
 * Static Functions
 * ================================================================ */

#ifndef EW_CONFIG_OPTION_CONVERT_TEXTAREA
const char ews_textarea_dummy = 0;      /* keep compiler from issuing an error for empty file */
#endif


/*
*** Local Variables: ***
*** mode: c ***
*** tab-width: 4 ***
*** End: ***
 */
