/*
 *
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
 * EmWeb/Server multiple URL hooks management functions
 *
 */

#define _EWSURLHOOKLIST_C_

#include "ew_types.h"
#include "ew_config.h"
#include "ews_urlhooks.h"
#include "ews_serv.h"

/* START-STRIP-EMWEB-LIGHT */

#ifdef EW_CONFIG_OPTION_URL_HOOK_LIST

typedef struct EwsUrlHookEntry_s
{
  EwsLink       link;
  EwaUrlHook    *hook_f;
} EwsUrlHookEntry;

/*
 * ewsUrlHookRegister
 *   Adds a URL Hook function to the URL Hook List.
 *   The location in the list to add it can be specified by the user.
 *
 * urlHook - call to be added to the list
 *
 * position -
 *            ewsHookPositionFirst      insert as first of list
 *            ewsHookPositionLast       insert as last of list
 *            ewsHookPositionBefore     insert before hook 'base'
 *            ewsHookPositionAfter      insert after hook 'base'
 *
 * base    - used if position is ewsHookPosition{Before,After}
 *           Hook function whose entry in the URL Hook list is used to position
 *           the new entry.
 *
 * return  - EWS_STATUS_OK
 *           EWS_STATUS_NO_RESOURCES
 *           EWS_STATUS_NOT_FOUND
 *           EWS_STATUS_BAD_REQUEST
 *
 */
EwsStatus ewsUrlHookRegisterPosition( EwaUrlHook*     urlHook
                                     ,EwsHookPosition position
                                     ,EwaUrlHook*     base
                                     )
{
  EwsUrlHookEntry *newHook, *thisHook;

  /* Build list element to record the URL hook. */
  newHook = (EwsUrlHookEntry *)ewaAlloc(sizeof(EwsUrlHookEntry));
  if ( newHook == NULL )
    {
      return EWS_STATUS_NO_RESOURCES;
    }
  newHook->hook_f = urlHook;

  /* Switch on the location parameter. */
  switch (position)
    {
    case ewsHookPositionLast:
      /* Insert as last in URL hook list. */
      EWS_LINK_INSERT(&(ews_state->url_hook_list), &(newHook->link));
      break;

    case ewsHookPositionFirst:
      /* Insert as first in URL hook list. */
      EWS_LINK_PUSH(&(ews_state->url_hook_list), &(newHook->link));
      break;

    case ewsHookPositionBefore:
    case ewsHookPositionAfter:
      /* Insert before/after a particular URL hook function. */
      /* Search for the right location. */
      for (thisHook =
             (EwsUrlHookEntry *)EWS_LINK_HEAD(ews_state->url_hook_list);
           thisHook != (EwsUrlHookEntry *)&(ews_state->url_hook_list);
           thisHook = (EwsUrlHookEntry *)EWS_LINK_NEXT(thisHook->link))
        {
          if (thisHook->hook_f == base)
            {
              /* We have found the right location.  Insert before or after
               * as appropriate. */
              if (position == ewsHookPositionBefore)
                {
                  EWS_LINK_INSERT(&(thisHook->link), &(newHook->link));
                }
              else
                {
                  EWS_LINK_PUSH(&(thisHook->link), &(newHook->link));
                }
              /* Note this is the normal way to exit this loop.
               * It will lead to 'return EWS_STATUS_OK'.
               * (A 'break' will not work, because of the enclosing 'for'
               * loop.) */
              return EWS_STATUS_OK;
            }
        }
      /* Fell out of the 'for' loop, which means we didn't find the URL
       * hook function that gives the insertion position. */
      return EWS_STATUS_NOT_FOUND;

    default:
      /* Unknown 'position' argument. */
      return EWS_STATUS_BAD_REQUEST;
    }

  /* Normal return. */
  return EWS_STATUS_OK;
}

/*
 * ewsUrlHookUnRegister
 *   Removes a URL Hook function from the URL Hook List.
 *
 * urlHook - call to be removed from the list
 *
 * return  - EWS_STATUS_OK
 *           EWS_STATUS_NOT_FOUND
 *
 */
EwsStatus ewsUrlHookUnRegister( EwaUrlHook *urlHook )
{
  EwsUrlHookEntry* thisHook;
  EwsContext cont;

  if (EWS_LINK_IS_EMPTY(&(ews_state->url_hook_list)))
    return(EWS_STATUS_NOT_FOUND);

  thisHook = (EwsUrlHookEntry *)EWS_LINK_HEAD(ews_state->url_hook_list);
  while (thisHook->hook_f != urlHook)
    {
      /* keep going */
      thisHook = (EwsUrlHookEntry *)(EWS_LINK_NEXT(thisHook->link));
      if (thisHook == (EwsUrlHookEntry *)EWS_LINK_HEAD(ews_state->url_hook_list))
        {
          /* we wrapped around */
          return(EWS_STATUS_NOT_FOUND);
        }
    }

  EWS_LINK_DELETE(&(thisHook->link));  /* delete hook from the list */

  /* Having removed the hook from the list, find and abort all requests
   * whose next URL hook is the hook being removed.
   * In theory, we could advance these requests' next_url_hook pointers to
   * the next URL hook on the list, but these requeses probably have some
   * non-trivial pending interaction with the URL hook function being deleted.
   * In any case, the next higher protocol level in the client must be able
   * to cope with unexpected request aborts.
   */
  /* Search the 'suspended' list. */
  for ( cont = (EwsContext)ews_state->context_wait_list.next;
        cont != (EwsContext)&ews_state->context_wait_list;
        cont = (EwsContext)cont->link.next
        )
    {
      if (cont->state == ewsContextStateFindDocument &&
          (EwsUrlHookEntry *)cont->next_url_hook == thisHook)
        {
          ewsNetHTTPAbort(cont);
        }
    }
  /* Search the 'servable' list. */
  for ( cont = (EwsContext)ews_state->context_serve_list.next;
        cont != (EwsContext)&ews_state->context_serve_list;
        cont = (EwsContext)cont->link.next
        )
    {
      if (cont->state == ewsContextStateFindDocument &&
          (EwsUrlHookEntry *)cont->next_url_hook == thisHook)
        {
          ewsNetHTTPAbort(cont);
        }
    }

  /* Now we can safely delete the link that points to the hook. */
  ewaFree(thisHook);

  return(EWS_STATUS_OK);
}

/*
 * ewsUrlHookContinue
 *   Called from within a URL Hook function to indicate that URL Hook
 *   processing should continue with the next URL Hook function on the list,
 *   despite that this URL Hook function will be returning a non-NULL value
 *   (which gives the URL that this function translated the URL into).
 */
void ewsUrlHookContinue( EwsContext context )
{
  context->url_continue = TRUE;
}

/*
 * ewsUrlHook
 *   Calls the routines on the URL Hook List until one returns
 *   a pointer not equal to NULL; this value is then returned by
 *   this routine.
 *   Handles various messy details, including continuing processing if
 *   hook returns non-NULL value but calls ewsUrlHookContinue,
 *   testing for suspend and resume, and copying the returned URL pointers
 *   into memory owned by the server (rather than the application).
 */
char* ewsUrlHook( EwsContext context, char* url )
{
  EwsUrlHookEntry* thisHook;
  char* resultUrl;

  resultUrl = NULL;

  /* Execute the list of URL hooks.
   * Start at the correct URL hook:
   *    - if next_url_hook is NULL, start with the first URL hook
   *    - if next_url_hook is not NULL, start with that URL hook
   *    - if next_url_hook is the header node, execute no URL hooks
   * Exit when:
   *    - a returned resultUrl is not NULL and context->url_continue
   *      has not been set
   *    - we reach the end of the list
   *    - a URL hook calls ewsSuspend()
   */
  if (context->next_url_hook == NULL)
    {
      /* next_url_hook == NULL means start with the first URL hook. */
      thisHook = (EwsUrlHookEntry *)EWS_LINK_HEAD(ews_state->url_hook_list);
    }
  else if (context->next_url_hook ==
           EWS_LINK_HEAD(ews_state->url_hook_list))
    {
      /* next_url_hook == the header means all URL hooks have been processed. */
      return url;
    }
  else
    {
      /* Any other value is the link of the first URL hook to process. */
      thisHook = (EwsUrlHookEntry *)context->next_url_hook;
    }
  /* Have to check up front whether there are any hooks to run. */
  if (thisHook != (EwsUrlHookEntry *)&(ews_state->url_hook_list))
    {
      for (;;)
        {
          EMWEB_TRACE(("ewsUrlHook(%p) calling URL hook %p on URL '%s'\n",
                       context, thisHook->hook_f, url));

          context->url_continue = FALSE;
          resultUrl = thisHook->hook_f(context, url);

          /* Test for suspend. */
#         ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
          if (context->schedulingState == ewsContextSuspended)
            {
              break;
            }
#         endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
          /* Advance to the next URL hook, now that we know we aren't
           * suspended.
           * Also, don't extract the 'next' link from this URL hook's node
           * until after it has executed -- the hook might unregister the next
           * URL hook in the list (yuck). */
          thisHook = (EwsUrlHookEntry *)EWS_LINK_NEXT(thisHook->link);
          context->next_url_hook = (EwsLink *)thisHook;
          /* Test for remaining exit conditions, after next_url_hook was
           * advanced. */
          if ((resultUrl != NULL && !context->url_continue) ||
              thisHook == (EwsUrlHookEntry *)&(ews_state->url_hook_list)
#             ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
              || context->schedulingState == ewsContextSuspended
#             endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
              )
            {
              break;
            }

          /* If this URL hook executed but we are going to execute another one,
           * we have to copy the URL it returned into memory that the server
           * controls.
           * Note that if we are not going to execute another URL hook, either
           * because this is the last one, or because the request suspended,
           * we do not relocate the URL here, but rather our caller
           * (ewsServeFindDocument) does if it is necessary.
           * In part, this complication is necessary because the returned value
           * of any application routine is ignored if the routine suspends the
           * request.
           */
          if (resultUrl != NULL)
            {
              if ((url = ewsRelocateURL( context, url, resultUrl )) == NULL)
                {
                  /* Unable to allocate memory to copy the URL into --
                   * abort request by returning NULL. */
                  return NULL;
                }
              /* Note ewsRelocatURL has stored the new URL pointer into the
               * context, so we do not have to worry about losing it if there
               * is a suspend, etc.
               * Do not have to set resultUrl = NULL here, because it will be
               * written over in call to thisHook->hook_f on next iteration. */
            }
        }
    }

  /* Return the current value for the URL.  If we exited the loop because
   * a hook returned a non-NULL value, we return that value ('resultUrl')
   * to our caller.
   * (If the hook suspended the request, this value will be ignored.)
   * Otherwise, the latest value for the URL is in 'url'. */
  return ( ( NULL == resultUrl ) ? url : resultUrl );
}

/*
 * ewsUrlHookShutdown
 *   Perform appropriate shutdown activities for multiple URL hooks.
 */
void ewsUrlHookShutdown( void )
{
  /* Remove all entries on the URL hook list.
   * This is not a particularly efficient technique, but it eliminates
   * all concerns about race conditions, since it uses ewsUrlHookUnRegister,
   * which handles race conditions correctly.
   */
  /* While the list of URL hooks is not empty. */
  while (!EWS_LINK_IS_EMPTY(&(ews_state->url_hook_list)))
    {
      /* Remove the first item on the list by find the hook function
         and ordering it deleted. */
      ewsUrlHookUnRegister(((EwsUrlHookEntry *)
                            EWS_LINK_HEAD(ews_state->url_hook_list))->
                           hook_f);
    }
}

#endif /* EW_CONFIG_OPTION_URL_HOOK_LIST */

/* END-STRIP-EMWEB-LIGHT */
