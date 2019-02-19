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
 * EmWeb/Server form processing
 *
 */

#include "ew_types.h"
#include "ew_config.h"
#include "ews_optck.h"
#include "ews_sys.h"
#include "ews_pars.h"


/*
 * Load field type table by defining EW_DEFINE_FIELD_TYPE_TABLE before
 * including ew_form.h
 */

#if defined(EW_CONFIG_OPTION_FORM) || defined(EW_CONFIG_OPTION_STRING_TYPED)
#define EW_DEFINE_FIELD_TYPE_TABLE
#include "ews_form.h"
#endif /* EW_CONFIG_OPTION_FORM | EW_CONFIG_OPTION_STRING_TYPED */

#ifdef EW_CONFIG_OPTION_FORM

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
boolean ewBuildRepeatForm( EwsContext context
                          ,EwsDocumentContextP doc_context
                          ,const EwFormEntry *entryp);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

#if defined(EW_CONFIG_OPTION_FILE_SYSTEM_LIST) &&\
    defined(EW_CONFIG_OPTION_FIELDTYPE_FILE)
/*
 * Find file system handle for the file, close the file
 * and update use count for the file system
 */
static void
fs_fileClose
  (EwsContext context, EwaFileHandle handle, boolean closeFile)
{
  EwsInputFileSystemEntry entry;

  if (!(EWS_LINK_IS_EMPTY(&(context->inputFile_system_list))))
    {
      entry = (EwsInputFileSystemEntry)EWS_LINK_HEAD(context->inputFile_system_list);
      while (entry->fileHandle != handle)
        {
          /* keep going */
          entry = (EwsInputFileSystemEntry)(EWS_LINK_NEXT(entry->link));
          if (entry == (EwsInputFileSystemEntry)EWS_LINK_HEAD(context->inputFile_system_list))
            {
              /* we wrapped around , no file system for the handle */
              entry = NULL;
            }
        }
      if (entry != NULL)
        {
          /*
           * If passing form to user, user owns responsibility for closing
           * the file, so don't abort here.  Otherwise, we are aborting
           * form submission and must close file.
           */
          (entry->fileSystem->use_cnt)--;

          if (closeFile)
            {
              if (entry->fileSystem->fs.fileClose == NULL)
                {
                  EMWEB_ERROR(("fs_fileClose: NULL fileClose function\n"));
                  return;
                }
              entry->fileSystem->fs.fileClose(handle, EWS_STATUS_ABORTED);
            }
          EWS_LINK_DELETE(&(entry->link));
          ewaFree(entry);
        }
    }
}
#endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */

/*
 * ewsFormServe
 * Serve document form node.  This function determines from the node index the
 * appropriate form serving action and writes values to the data stream.
 *
 * context      - context of request
 * node_index   - document node index
 *
 * No return value
 */
void
ewsFormServe
  (EwsContext context, EwsDocumentContextP doc_context, uint32 node_index )
{
  uintf form = EW_FORM_NODE_INDEX_TO_FORM_INDEX(node_index);
  uintf element = EW_FORM_NODE_INDEX_TO_ELEMENT_INDEX(node_index);
  const EwFormEntry *entryp = &doc_context->document->archive->emweb_form_table[form];
  const EwFormField *fieldp;
  uintf value_size = 0;
  uint8 *p1 = NULL, *p2;
  uint8 *sp;
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
  uintf rpt_cnt = 0;            /* repeat count from form structure */
  EwFormRepeatField *rpt_fieldp = NULL;
  const EwFormField *start_rpt_fieldp = NULL;
  uintf start_rpt_element = 0;
  boolean empty_rpt_body = FALSE;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

#   ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
    /*
     * If restarting a form after suspension, the form structure will have
     * previously been allocated and initialized.  Simply call the application
     * form serve function
     */
    if (element == EW_FORM_ELEMENT_START && context->form != NULL)
      {
        (*entryp->serve_f)(context, context->form);
      }
    else
#   endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */

    /*
     * If starting a form, then allocate an application form structure,
     * fill in default values, attach to context, and return.
     */
    if (element == EW_FORM_ELEMENT_START)
      {
        /*
         * Save entry and index
         */
        context->form_index = form;
        context->form_entry = entryp;
        context->form_enum_table =
          doc_context->document->archive->emweb_form_enum_table;
        context->form_enum_table_size =
          doc_context->document->archive->emweb_form_enum_table_size;

        /*
         * Allocate application-specific form structure
         */
        if ((context->form = (void *) ewaAlloc(entryp->form_size)) == NULL)
          {
            ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
            ewsNetHTTPAbort(context);
            return;
          }
#       ifdef EW_CONFIG_OPTION_FORM_REPEAT
        context->base_form = context->form_status = context->form;
        /*
         * allocate application specific sub-structures
         * if needed
         */
        if (doc_context->nodep->attributes & EW_DOCUMENT_NODE_REPEAT_BODY)
          {
            if ( !ewBuildRepeatForm(context, doc_context, entryp) )
              {
                ewaLogHook(context, EWS_LOG_STATUS_NO_RESOURCES);
                ewsNetHTTPAbort(context);
                return;
              }
          }

        doc_context->rpt_body_number = 0; /* indicates which <EMWB_REPEAT> is processed */
        doc_context->rpt_body_iteration = 0; /* indicates which iteration is processed */
#       endif /* EW_CONFIG_OPTION_FORM_REPEAT */

        /*
         * Initialize each element with default value
         */
        for (element = 0, fieldp = entryp->field_list;
             element < entryp->field_count;
             element++, fieldp++)
          {
            /*
             * Handle by field element type
             */
            switch (fieldp->field_type)
              {
                /*
                 * Special cases: these entries are used for serving the
                 * form, not for initializing and writing values.  (There is
                 * a one to N relationship of fields to entries of these
                 * types).
                 */
#               ifdef EW_CONFIG_OPTION_FIELDTYPE_RADIO
                case ewFieldTypeRadioField:
#               ifdef EW_CONFIG_OPTION_FORM_REPEAT
                case ewFieldTypeFormRadioField:
#               endif /* EW_CONFIG_OPTION_FORM_REPEAT */
#               endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO */
                case ewFieldTypeSelectField:
                  continue;

                case ewFieldTypeDynamicSingleSelect:
                case ewFieldTypeDynamicMultiSelect:
                  /*
                   * Need to clear status field (done below).
                   * note that p1 is set to NULL explicitly because
                   * the default_value field is used for other
                   * reasons (the OPTION generator), and there
                   * is no default for dynamic selects.
                   */
                  p1 = NULL;
                  break;


                /*
                 * In text fields, the default value is just a pointer
                 * to a null-terminated string.  We copy the pointer, not
                 * the string.
                 */
                case ewFieldTypeTextOverride:
                case ewFieldTypeText:
                  p1 = (uint8 *) &fieldp->default_value;
                  value_size = sizeof(char *);
                  break;

#               ifdef EW_CONFIG_OPTION_FORM_REPEAT
                /*
                 * for fields describing repeat body, destination
                 * for default value and status is n-th substructure
                 * where n is a number from 0 to number of body repetitions.
                 * So for the fields between
                 *   ewFieldTypeRepeat.....ewFieldTypeRepeatEnd
                 * we have to loop to initialize all substructures
                 * which are in the array pointed to by
                 *    context->form + fieldp->value_offset and
                 *    context->form + fieldp->status_offset
                 */
                case ewFieldTypeRepeat:         /* START EMWEB_REPEAT */
                  if (rpt_cnt == 0)
                    {
                      rpt_fieldp = (EwFormRepeatField *)(fieldp->default_value);
                      rpt_cnt =
                        *(uintf *)((uint8 *) context->form + rpt_fieldp->repeat_offset);
                      if (rpt_cnt == 0)
                        {
                          empty_rpt_body = TRUE;    /* repeat count 0: no fields */
                          continue;                 /* do not repeat */
                        }

                      /*
                       * remember the start of the repeat.
                       * set base_form to point to the form structure
                       * set form_status to point to the first entry in the
                       *   array of "status" structures for the repeated fields
                       * set form to point to the first entry in the array of
                       *   "value" structures for the repeated fields.
                       */
                      start_rpt_fieldp = fieldp;
                      start_rpt_element = element;
                      context->base_form = context->form;
                      context->form_status = *(uint8 **)((uint8 *)context->form
                                                         + fieldp->status_offset);
                      context->form = *(uint8 **)((uint8 *)context->form
                                                  + fieldp->value_offset);
                    }
                 continue;
                 break;

                case ewFieldTypeRepeatEnd:          /* EMWEB_REPEAT END */
                  if (empty_rpt_body == TRUE)
                    {
                      empty_rpt_body = FALSE;   /* reset this flag */
                      continue;
                    }

                  rpt_cnt--;

                  if (rpt_cnt != 0)         /* we must repeat... */
                    {
                      /*
                       * do next iteration
                       */
                      element = start_rpt_element;
                      fieldp= start_rpt_fieldp;
                      context->form_status = (uint8 *)context->form_status +
                        rpt_fieldp->status_substruct_size;
                      context->form = (uint8 *)context->form +
                        rpt_fieldp->value_substruct_size;
                    }
                  else                      /* done with repetition */
                    {
                      /*
                       * restore base form info and move on
                       */
                      context->form = context->base_form;
                      context->form_status = context->base_form;
                    }
                  continue;
                  break;

#               ifdef EW_CONFIG_OPTION_FIELDTYPE_RADIO
                /*
                 * it this case, the default_value points to the structure
                 * with default_value
                 */
                case ewFieldTypeFormRadio:
                  p1 = (uint8 *)
                       ((EwFormRepeatRadioFormField *)fieldp->default_value)
                        ->default_value;
                  value_size =
                    ews_state->form_field_table[fieldp->field_type].field_size;
                  break;
#               endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO */
#               endif /* EW_CONFIG_OPTION_FORM_REPEAT */

                /*
                 * All others: the default value points to the data to be
                 * copied into the form, and the amount of data is
                 * determined by the field type.
                 */
                default:
                  p1 = (uint8 *) fieldp->default_value;
                  value_size =
                    ews_state->form_field_table[fieldp->field_type].field_size;
                  break;
              }

            /*
             * If initialization value available, set initialized bit in
             * status word and copy default value into form template
             */
#           ifdef EW_CONFIG_OPTION_FORM_REPEAT
            if (empty_rpt_body == TRUE)
              {
                continue;
              }
#           ifdef EW_CONFIG_OPTION_FIELDTYPE_RADIO
            if ((fieldp->field_type == ewFieldTypeFormRadio) ||
                (fieldp->field_type == ewFieldTypeFormRadioField))
              {
                sp = ((uint8 *) context->base_form) + fieldp->status_offset;
              }
            else
#           endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO */
              {
                sp = ((uint8 *) context->form_status) + fieldp->status_offset;
              }
#           else        /* !EW_CONFIG_OPTION_FORM_REPEAT */
            sp = ((uint8 *) context->form) + fieldp->status_offset;
#           endif /* EW_CONFIG_OPTION_FORM_REPEAT */
            if (p1 != NULL)
              {
                if ((fieldp->field_type == ewFieldTypeText) ||
                    (fieldp->field_type == ewFieldTypeTextOverride))
                  {
                    if (fieldp->default_value != NULL)
                      {
                        *sp = EW_FORM_INITIALIZED;
                      }
                    else
                      {
                        *sp = 0;
                      }
                  }
                else if (fieldp->field_type != ewFieldTypeDecimalOverride)
                  {
                    *sp = EW_FORM_INITIALIZED;
                  }
#               ifdef EW_CONFIG_OPTION_FORM_REPEAT
#               ifdef EW_CONFIG_OPTION_FIELDTYPE_RADIO
                if ((fieldp->field_type == ewFieldTypeFormRadio) ||
                    (fieldp->field_type == ewFieldTypeFormRadioField))
                  {
                    p2 = ((uint8 *) context->base_form) + fieldp->value_offset;
                  }
                else
#               endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO */
#               endif /* EW_CONFIG_OPTION_FORM_REPEAT */
                p2 = ((uint8 *) context->form) + fieldp->value_offset;
                EMWEB_MEMCPY(p2, p1, value_size);
              }

            /*
             * Otherwise clear status byte indicating no initial value
             */
            else if (fieldp->field_type != ewFieldTypeDecimalOverride)
              {
                *sp = 0;
              }

          } /* end FOR each element */

        /*
         * Invoke application-specific form serve function to give application
         * a chance to override the default form values.
         */
        (*entryp->serve_f)(context, context->form);

      } /* end IF first form element */

    /*
     * If ending a form, release the application form structure resources
     */
    else if (element == EW_FORM_ELEMENT_END)
      {
        ewsFormFree(context);
      }

    /*
     * Output a form value according to the element
     */
    else
      {
        /*
         * Perform type-specific conversion to network data from form
         */
        fieldp = &entryp->field_list[element - 1];
        if (ews_state->form_field_table[fieldp->field_type].to_f != NULL)
          {
            /*
             * If not override and not initialized, return empty string
             */
            if (fieldp->field_type != ewFieldTypeDecimalOverride)
              {
#               ifdef EW_CONFIG_OPTION_FORM_REPEAT
#               ifdef EW_CONFIG_OPTION_FIELDTYPE_RADIO
                if ((fieldp->field_type == ewFieldTypeFormRadio) ||
                    (fieldp->field_type == ewFieldTypeFormRadioField))
                  {
                    sp = ((uint8 *)context->base_form + fieldp->status_offset);
                  }
                else
#               endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO */
                    sp = ((uint8 *)context->form_status + fieldp->status_offset);
#               else /* ! EW_CONFIG_OPTION_FORM_REPEAT */
                    sp = ((uint8 *)context->form + fieldp->status_offset);
#               endif /* EW_CONFIG_OPTION_FORM_REPEAT */
                if ((*sp & EW_FORM_INITIALIZED) == 0)
                  {
                    return;
                  }
              }

            /*
             * Otherwise convert application value to HTML
             */
            ews_state->form_field_table[fieldp->field_type].to_f( context
                                                                  ,fieldp
                                                                  );
          }

#       ifdef EMWEB_SANITY
        /*
         * If conversion function was excluded by configuration from the
         * server build, but the feature was used in a soure HTML form
         * document, write an error attribute to keep the browser happy
         * and inform the user of a problem.
         */
        else
          {
            EMWEB_WARN(("ewFieldToUnsupported: EMWEB_TYPE unsupported\n"));
            (void) ewsBodySend(context, "EMWEB_UNSUPPORTED");
          }
#       endif /* EMWEB_SANITY */
      }
}

/*
 * ewsFormSubmitStart
 * Start form submission.  This function allocates and initializes a form
 * structure and attaches it to the context.
 *
 * context      - context of request
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewsFormSubmitStart ( EwsContext context )
{
  const EwFormEntry *entryp = context->form_entry;
  uintf element;
  const EwFormField *fieldp;
  uint8 *p;
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
  uint8 *vp;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

#   ifdef EW_CONFIG_OPTION_NS_SET
    /*
     * If namespace form, invoke SetStart function and save handle.
     */
    if (context->form_index == EW_FORM_INDEX_NAMESPACE)
      {
        context->form_handle = (*context->form_ns->set_start_f)
          (context);
        if (context->form_handle == NULL)
          {
            return EWS_STATUS_ABORTED;
          }
        return EWS_STATUS_OK;
      }
#   endif /* EW_CONFIG_OPTION_NS_SET */

    /*
     * Allocate application-specific form structure
     */
    context->form = (void *) ewaAlloc(entryp->form_size);
    if (context->form == NULL)
      {
        return EWS_STATUS_NO_RESOURCES;
      }
    EMWEB_MEMSET(context->form, 0, entryp->form_size);
#   ifdef EW_CONFIG_OPTION_FORM_REPEAT
    context->base_form = context->form_status = context->form;
#   endif /* EW_CONFIG_OPTION_FORM_REPEAT */

    /*
     * For each entry with status, clear the status byte
     * and for each repeat field, set value to NULL
     */
    for (element = 0, fieldp = entryp->field_list;
         element < entryp->field_count;
         element++, fieldp++)
       {
#        ifdef EW_CONFIG_OPTION_FORM_REPEAT
         /*
          * for repeat body, status is set to 0 when
          * the status substructure is allocated
          */
         if (fieldp->field_type == ewFieldTypeRepeat)
           {
             vp = ((uint8 *) context->form) + fieldp->value_offset;
             EMWEB_MEMSET(vp, 0, sizeof(void *));

             element += (((EwFormRepeatField *)fieldp->default_value)->subtable_field_cnt-1);
             fieldp += (((EwFormRepeatField *)fieldp->default_value)->subtable_field_cnt-1);
             continue;
           }
#        endif /* EW_CONFIG_OPTION_FORM_REPEAT */
         if (fieldp->status_offset != 0)
           {
             p = ((uint8 *) context->form) + fieldp->status_offset;
             *p = 0;
           }
       }

    return EWS_STATUS_OK;
}

#ifdef EW_CONFIG_OPTION_FORM_REPEAT

static boolean
ewFormPointerMagic(EwsContext context, EwsRptName *rptP,
                   EwFormRepeatField *rpt_fieldp,
                   const EwFormField *fieldp)
{
  uint8 **vpp;
  uint8 **spp;
  uintf *rpp;

  /*
   * do pointer magic
   */

  /*
   * make sure submitted prefix is acceptable.
   * Verify total number of repeats and instance.
   * If instance value is bad, breake connection.
   * If number of repeats too big, use predefined
   * value.
   */
  if (rptP->total_cnt == 0)
    {
      ewaLogHook(context, EWS_LOG_STATUS_BAD_FORM);
      ewsNetHTTPAbort(context);
      return(FALSE);
    }
  if (rpt_fieldp->max > 0)
    {
      /*
       * check number of repeats against value
       * provided by EMWEB_MAX
       */
      if (rptP->total_cnt > rpt_fieldp->max)
        {
          rptP->total_cnt = rpt_fieldp->max;
        }
    }
  else
    {
      /*
       * check against server's own max value
       */
      if (rptP->total_cnt > EWS_FORM_REPEAT_MAX_REPEATS)
        {
          rptP->total_cnt = EWS_FORM_REPEAT_MAX_REPEATS;
        }
    }
  if (rptP->iteration > rptP->total_cnt)
    {
      ewaLogHook(context, EWS_LOG_STATUS_BAD_FORM);
      ewsNetHTTPAbort(context);
      return(FALSE);
    }

  if (fieldp->field_type != ewFieldTypeRepeat)
    {
      return(TRUE);
    }

  if (rpt_fieldp->value_substruct_size == 0)
    {
      return(TRUE);
    }

  /*
   * if substructure does not exist yet, allocate it
   */
  vpp = (uint8 **)((uint8 *) (context->base_form) + fieldp->value_offset);
  if ((void *)(*vpp) == NULL)
    {
      spp = (uint8 **)((uint8 *) (context->base_form) + fieldp->status_offset);
      rpp = ((uintf *)((uint8 *) (context->base_form) +
                          rpt_fieldp->repeat_offset));
      if ((*vpp = (uint8 *)ewaAlloc( rptP->total_cnt *
                          (rpt_fieldp->value_substruct_size +
                           rpt_fieldp->status_substruct_size)))
          == NULL)
        {
          EMWEB_TRACE(("ewFormPointerMagic: no resources\n"));
          return(FALSE);
        }
      *spp = *vpp + (rptP->total_cnt * rpt_fieldp->value_substruct_size);
      *rpp = rptP->total_cnt;

      /*
       * clear status field
       */
      EMWEB_MEMSET((*spp),
                   0,
                   (rptP->total_cnt * rpt_fieldp->status_substruct_size));
     }
  else
     {
       /*
        * substructure exists, but is it the same repeat count
        */
       rpp = ((uintf *)((uint8 *) (context->base_form) +
                          rpt_fieldp->repeat_offset));
       if (rptP->total_cnt != *rpp)
         {
           ewaLogHook(context, EWS_LOG_STATUS_BAD_FORM);
           ewsNetHTTPAbort(context);
           return(FALSE);
         }
     }

  /*
   * do form pointers magic in context
   */
  vpp = (uint8 **) ((uint8 *) (context->base_form) + fieldp->value_offset);
  spp = (uint8 **) ((uint8 *) (context->base_form) + fieldp->status_offset);
  context->form = (*vpp) + rptP->iteration * rpt_fieldp->value_substruct_size;
  context->form_status =
    (*spp) + rptP->iteration * rpt_fieldp->status_substruct_size;
  return(TRUE);
}
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

/*
 * ewsFormSubmitSet
 * Given a name/value pair from POST data (x-www-urlencoded or
 * multipart/form-data), update the form structure appropriately.
 *
 * context      - context of request
 * name         - name in name/value pair
 * value        - value in name/value pair,
 *                  CAN BE NULL (if field type is file)
 *
 * return EWS_STATUS_OK or error
 */
EwsStatus
ewsFormSubmitSet ( EwsContext context, EwsStringP name, EwsStringP value )
{
  const EwFormEntry *entryp = context->form_entry;
  uintf element;
  const EwFormField *fieldp;
  uint8 *sp;
  EwsStatus  returnValue = EWS_STATUS_OK;


# ifdef EW_CONFIG_OPTION_FORM_REPEAT
  uint32 rpt_cnt = 0;
  EwsRptName rpt;
  EwFormRepeatField *rpt_fieldp = NULL;
  boolean name_prefix = FALSE;
# endif /* EW_CONFIG_OPTION_FORM_REPEAT */


# ifdef EW_CONFIG_OPTION_NS_SET
  /*
   * If namespace submission
   */
  if (context->form_index == EW_FORM_INDEX_NAMESPACE)
    {
      char *tmp, *cp_oid, *cp_param, *cp_value;
      uintf nodeIndex, len;

      nodeIndex = 0;
      context->currentElement = NULL;

      /*
       * Allocate temporary storage for contiguous strings
       */
      tmp = (char *) ewaAlloc(ewsStringLength(name) +
                              ewsStringLength(value) +
                              2);
      if (tmp == NULL)
        {
          EMWEB_WARN(("ewsFormSubmitSet: can't allocate memory for Namespace set.\n"));
          ewsNetHTTPAbort( context );
          return EWS_STATUS_NO_RESOURCES;
        }
      cp_oid = tmp;
      cp_value = tmp + ewsStringLength(name) + 1;

      /*
       * Copy name and value into contiguous memory
       */
      ewsStringCopyOut(cp_oid, name, ewsStringLength(name)+1);
      ewsStringCopyOut(cp_value, value, ewsStringLength(value)+1);

      /*
       * Check for a schema-related EmWeb name.
       * We mangle names that come from object that use schemas
       * to describe the characteristics of the object.  In this
       * case, the NAME= attribute is in the following format:
       *      EmWeb_ns:nsname:#[:otherstuff][.params]
       * Where
       * EmWeb_ns: is the string "EmWeb_ns:"
       * nsname: is the namespace name (valid C identifier chars)
       *   followed by a :
       * # is the schema node index in the schema document
       *   node list.  1's based integer.
       * [:otherstuff] is optional future-proofing, we ignore anything
       *   after the # if it starts with ':', up to the optional params.
       *   note that future uses of this area should NOT allow .'s
       * params - optional constant parameter string
       *
       * Demangle this name to setup the schema info before invoking
       * the namespace set function.
       */

      /*
       * Split name into object and parameter (at the '.')
       */
      for (cp_param = cp_oid;
           (   *cp_param
            && *cp_param != '.'
           );
           cp_param++)
        ;
      if (*cp_param)
        {
          *cp_param++ = '\0';
        }
      else
        {
          cp_param = NULL;    /* NO params */
        }

      /* check for name mangling as described above */

      EMWEB_STRLEN( len, cp_oid );
      if (len > 11)   /* > strlen("EmWeb_ns:x:y")  min ns mangling */
        {
          boolean EmWebNS;

          EMWEB_STRNCMP( EmWebNS, "EmWeb_ns:", cp_oid, 9 );
          if (!EmWebNS)
            {
              const char *p;

              for (p = cp_oid + 9;  /* skip namespace */
                   *p && *p != ':';
                   p++
                   )
                ;

              if (*p++ == ':')    /* verify ends in : */
                {
                  while (*p && *p != ':')      /* convert # to int */
                    {
                      if (*p < '0' || '9' < *p)
                        {
                          nodeIndex = 0;  /* not a # char */
                          break;
                        }
                      nodeIndex = (nodeIndex * 10) + *p - '0';
                      p++;
                    }
                }
            }
        }

      /*
       * if we matched, then nodeIndex != 0, verify that it is a valid index
       */

      if (   nodeIndex != 0
#         ifdef EMWEB_SANITY
          && context->document != NULL        /* if schema info present */
          && context->document->schema_document != NULL   /* in archive */
#         endif
          && nodeIndex <= (unsigned)EW_BYTES_TO_UINT16(context->document->schema_document->node_count)
         )
        {
          context->archiveData = context->document->archive_data;
          context->schemaNodes = context->document->schema_nodes;
          context->currentElement = (const EwsDocumentNode *)
              (  ((const char *)context->schemaNodes)
               + (SIZEOF_EWS_DOCUMENT_NODE * (nodeIndex - 1))
              );
        }

      /* element must be present and it MUST be a START_ELEMENT node, else error! */

      if (   context->currentElement == NULL
          || context->currentElement->type != EW_DOCUMENT_NODE_TYPE_START_ELEMENT
         )
        {
          EMWEB_WARN(( "ewsFormSubmitSet: invalid namespace object name: [%s]\n"
                       "    Aborting Submission!\n"
                       ,(cp_oid) ? cp_oid : "<NULL>"
                    ));
          ewaFree(tmp);
          ewsNetHTTPAbort( context );
          return EWS_STATUS_ABORTED;
        }

      /*
       * Notify application namespace set function
       */

      (*context->form_ns->set_f)(context
                                 ,context->form_handle
                                 ,(const char *) &context->archiveData[EW_BYTES_TO_UINT32(context->currentElement->index)]
                                 ,cp_param
                                 ,cp_value);

      context->currentElement = NULL;   /* reset schema info */

      /*
       * Release memory and return
       */
      ewaFree(tmp);
      return EWS_STATUS_OK;
    }
# endif /* EW_CONFIG_OPTION_NS_SET */

  /*
   * Scan field table for matching name
   */

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
  /*
   * check the name for prefix
   */
  context->base_form = context->form;
  context->form_status = context->form;

  if (ewCheckPrefix(name, &rpt) == TRUE)
    {
      /*
       * find proper repeat field and serch for name only within
       * repeat body.
       * Do also form structure pointer magic so value and status offset
       * match structure/substructure
       */
      for (element = 0, fieldp = entryp->field_list, rpt_cnt = 1;
           element < entryp->field_count;
           element++, fieldp++)
        {
          if (fieldp->field_type == ewFieldTypeRepeat)
            {
              rpt_fieldp = (EwFormRepeatField *)(fieldp->default_value);
              if (rpt_cnt == rpt.number)
                {
                   if (ewFormPointerMagic(context, &rpt,
                                          rpt_fieldp, fieldp) == FALSE)
                     {
                       return EWS_STATUS_OK;
                     }
                  name_prefix = TRUE;
                  break;
                }
              else
                {
                  rpt_cnt++;
                  element += (rpt_fieldp->subtable_field_cnt - 1);
                  fieldp += (rpt_fieldp->subtable_field_cnt - 1);
                  continue;
                }
            }
        }
      if (element == entryp->field_count)
        {
          EMWEB_TRACE(("ewsFormSubmitSet: unknown name/value pair received\n"));
          return EWS_STATUS_NOT_FOUND;
        }
    }
  else
    {
      element = 0;
      fieldp = entryp->field_list;
    }
#else
  element = 0;
  fieldp = entryp->field_list;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

  for (;
       element < entryp->field_count;
       element++, fieldp++)
    {
      EwsString nstmp;
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
      if ((fieldp->field_type == ewFieldTypeRepeatEnd) &&
          (name_prefix == TRUE))
        {
          break;
        }
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
      /*
       * If field has name, name matches (to exact length)...
       */
      if (fieldp->name != NULL
          && (nstmp = *name, ewsStringCompare(&nstmp
                                              ,fieldp->name
                                              ,ewsStringCompareCaseSensitive
                                              ,NULL))
          && (nstmp.length == 0)
          )
        {
          /*
           * Special case: for multiple select, value must also match for
           * this field to be selected
           */
          if (fieldp->field_type == ewFieldTypeSelectMulti)
            {
              EwsString vstmp;

              if (value == NULL
                  || (vstmp = *value,
                      ewsStringCompare(&vstmp
                                        ,fieldp->sel_value
                                        ,ewsStringCompareCaseSensitive
                                        ,NULL) == FALSE)
                  || vstmp.length != 0)
                {
                  continue;         /* no match, try next */
                }
            }
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
          if (fieldp->field_type == ewFieldTypeFormRadio)
            {
              if ((value != NULL) && ((ewCheckPrefix(value, &rpt)) == TRUE))
                {
                  if (rpt_fieldp != NULL)
                    {
                      if (ewFormPointerMagic(context, &rpt,
                                             rpt_fieldp, fieldp) == FALSE)
                        {
                          return EWS_STATUS_OK;
                        }
                    }
                }
            }
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
          break;
        }
    }

  /*
   * If no match found, ignore name/value pair
   */
  if ((element == entryp->field_count)
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
      || (fieldp->field_type == ewFieldTypeRepeatEnd)
#endif
     )
    {
      EMWEB_TRACE(("ewsFormSubmitSet: unknown name/value pair received\n"));
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
      context->form = context->base_form;
#endif
      return EWS_STATUS_NOT_FOUND;
    }

  /*
   * Invoke the appropriate conversion function
   */
  if (ews_state->form_field_table[fieldp->field_type].from_f != NULL)
    {
      returnValue = 
        (*ews_state->form_field_table[fieldp->field_type].from_f)
          (context, fieldp, value);

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
      /*
       * if we deal with prefixed value,
       * set ..._instance in the form structure
       */
      if ((fieldp->field_type == ewFieldTypeFormRadio) ||
          (fieldp->field_type == ewFieldTypeFormRadioField))
        {
          uintf *ip;
          EwFormRepeatRadioFormField *subfieldp;

          subfieldp = (EwFormRepeatRadioFormField *)(fieldp->default_value);
          ip = (uintf *)((uint8 *)context->base_form +
                                  (subfieldp->instance_offset));
          *ip = (uintf)rpt.iteration;
         }
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

    }

  /*
   * If conversion function was excluded by configuration from the server
   * build, but the feature was used in a soure HTML form document,
   * mark field as unparsable and continue form processing
   */
  else
    {
      EMWEB_WARN(("ewFieldFormUnsupported: EMWEB_TYPE unsupported\n"));
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
      if ((fieldp->field_type != ewFieldTypeFormRadio) &&
          (fieldp->field_type != ewFieldTypeFormRadioField))
        sp = ((uint8 *)context->form_status) + fieldp->status_offset;
      else
        sp = ((uint8 *)context->base_form) + fieldp->status_offset;
#else
      sp = ((uint8 *)context->form) + fieldp->status_offset;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
      *sp = EW_FORM_PARSE_ERROR;
      returnValue = EWS_STATUS_FATAL_ERROR;

    }
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
   /*
    * restore pointers
    */
   context->form = context->base_form;
#endif

   if (returnValue != EWS_STATUS_OK)
     return returnValue;
   else
     return EWS_STATUS_OK;

}

/*
 * ewsFormSubmitAction
 * Pass the form to the application and process
 *
 * context      - context of request
 *
 * Returns NULL for default action, or URL string for redirect.
 */
EW_NSC char *
ewsFormSubmitAction ( EwsContext context )
{
  const EwFormEntry *entryp = context->form_entry;
  EW_NSC char *cp = NULL;
# ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
  uintf element;
  const EwFormField *fieldp;
# endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

# ifdef EW_CONFIG_OPTION_NS_SET
    /*
     * If namespace set, commit
     */
    if (context->form_index == EW_FORM_INDEX_NAMESPACE)
      {
        cp = (*context->form_ns->set_finish_f)(context
                                            ,context->form_handle
                                            ,EWS_STATUS_OK);

#       ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
        if (context->schedulingState != ewsContextSuspended)
#       endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
          {
            context->form_handle = NULL;
          }

        return cp;
      }
    else
# endif /* EW_CONFIG_OPTION_NS_SET */

# ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
    /*
     * For each INPUT field of TYPE=FILE, make sure that
     * the dynamic bit is cleared out - it is now the responsibility
     * of the submit function to close the file
     */
    for (element = 0, fieldp = entryp->field_list;
         element < entryp->field_count;
         element++, fieldp++)
      if (fieldp->field_type == ewFieldTypeFile)
        {
#         ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
          /*
           * for each file, update use count
           */
          EwaFileHandle handle;

          handle = *(EwaFileHandle*)((uint8 *)context->form + fieldp->value_offset);
          fs_fileClose(context, handle, FALSE);
#         endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
          *(((uint8 *)context->form) + fieldp->status_offset) &= ~EW_FORM_DYNAMIC;
        }
# endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

    /*
     * Simply transfer control to application-specific function and return
     */
    cp = (*entryp->submit_f)(context, context->form);
    return cp;
}

/*
 * ewsFormFree
 * Release resources associated with a form
 *
 * context      - context of request
 *
 * No return value
 */
void
ewsFormFree ( EwsContext context )
{
  const EwFormEntry *entryp;
  uintf element;
  const EwFormField *fieldp;
  uint8 *p1;
  void *p;

# ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
  EwFormFieldDynamicMultiSelect *msubfieldP;
  EwFormFieldDynamicSingleSelect *ssubfieldP;
  int indx;

# ifdef EW_CONFIG_OPTION_FIELDTYPE_TEXT
  char **ppch;
# endif /* EW_CONFIG_OPTION_FIELDTYPE_TEXT */

# ifdef EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING
  EwsFormFieldHexString *phex;
# endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */

# ifdef EW_CONFIG_OPTION_FIELDTYPE_OID
  EwsFormFieldObjectID *poid;
# endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */
# endif /* EW_CONFIG_OPTION_FIELDTYPE_Select_DYNAMIC*/

# ifdef EW_CONFIG_OPTION_FORM_REPEAT
  uint32 iteration = 0, repeat_cnt = 0;
  uintf element_save = 0;
  const EwFormField *fieldp_save = 0;
  const EwFormRepeatField *rpt_fieldp = NULL;
  boolean repeat_body = FALSE;
# endif /* EW_CONFIG_OPTION_FORM_REPEAT */

# ifdef EW_CONFIG_OPTION_NS_SET
  /*
   * It is possible that this is a namespace form.  If so, return now as
   * there is not actually a form structure attached to this context.
   */
  if (context->form_index == EW_FORM_INDEX_NAMESPACE)
    {
      return;
    }
# endif /* EW_CONFIG_OPTION_NS_SET */


# ifdef EW_CONFIG_OPTION_FORM_REPEAT
  /*
   * if base_form is NOT NULL, that means we are freeing a repeating
   * form.  Since this free routine can be called while the form
   * serve or form submit is in progress, then the context->form pointer
   * may not be pointing to the true form structure, but one of the repeated
   * substructures instead.  So, restore context->form from
   * base_form before proceeding to free
   */
  if (context->base_form != NULL)
    {
      context->form = context->base_form;
    }
# endif /* EW_CONFIG_OPTION_FORM_REPEAT */

  entryp = context->form_entry;
  if (entryp)

    {

      /*
       * Check each element for dynamically allocated data
       */
      for (element = 0, fieldp = entryp->field_list;
           element < entryp->field_count;
           element++, fieldp++)
        {
#         ifdef EW_CONFIG_OPTION_FORM_REPEAT
          /*
           * consider repeat bodies
           */
          if ((fieldp->field_type == ewFieldTypeRepeat) &&
              (iteration == 0))
            {
              rpt_fieldp = (EwFormRepeatField *)(fieldp->default_value);
              if (rpt_fieldp->value_substruct_size == 0)
                {
                  /*
                   * this body has no substructures
                   */
                  while (fieldp->field_type != ewFieldTypeRepeatEnd)
                    {
                      element++;
                      fieldp++;
                    }
                  continue;
                }

              /*
               * save information about beginning of the body
               */
              repeat_body = TRUE;
              element_save = element;
              fieldp_save = fieldp;
              /*
               * set up value and status substructures' pointers
               */
              repeat_cnt = *(uintf *)((uint8 *) (context->form) +
                                      rpt_fieldp->repeat_offset);
              if ((void *)(*(uint8 **)((uint8 *)
                                       (context->form) + fieldp->value_offset)) != NULL)
                {
                  context->base_form = context->form;
                  context->form_status = *(uint8 **)((uint8 *) context->form
                                                     + fieldp->status_offset);
                  context->form = *(uint8 **)((uint8 *) context->form
                                              + fieldp->value_offset);
                  iteration++;
                }
              else
                {
                  element += rpt_fieldp->subtable_field_cnt - 1;
                  fieldp += rpt_fieldp->subtable_field_cnt - 1;
                }
              continue;
            }
          if (fieldp->field_type == ewFieldTypeRepeatEnd)
            {
              iteration++;
              if (iteration <= repeat_cnt)
                {
                  /*
                   * next iteration,
                   * restore beginning of body
                   */
                  element = element_save;
                  fieldp = fieldp_save;
                  /*
                   * move substructures' pointers
                   */
                  context->form_status = (uint8 *)context->form_status +
                    rpt_fieldp->status_substruct_size;
                  context->form = (uint8 *)context->form +
                    rpt_fieldp->value_substruct_size;
                }
              else
                {
                  iteration = 0;
                  /*
                   * free substructure
                   */
                  if (repeat_cnt != 0)
                    {
                      ewaFree(*(uint8 **)((uint8 *) (context->base_form)
                                          + fieldp_save->value_offset));
                    }
                  /*
                   * restore form pointer
                   */
                  repeat_body = FALSE;
                  context->form = context->base_form;
                  context->form_status = context->base_form;
                }
              continue;
            }

          if ((repeat_body == TRUE) && (repeat_cnt == 0))
            {
               if ((ews_state->form_field_table[fieldp->field_type].free_f != NULL))
              {
                /*
                 * Read status byte.  If marked as dynamic, invoke free fn.
                 */
                #ifdef EW_CONFIG_OPTION_FORM_REPEAT
                if (repeat_body == TRUE)
                {
                  p1 = ((uint8 *) context->form_status) + fieldp->status_offset;
                }
                else
                {
                  p1 = ((uint8 *) context->form) + fieldp->status_offset;
                }
                #else
                p1 = ((uint8 *) context->form) + fieldp->status_offset;
                #endif /* EW_CONFIG_OPTION_FORM_REPEAT */
                
                if ((*p1 & EW_FORM_DYNAMIC) != 0)
                {
                  p = (void *)(((uint8*)context->form) + fieldp->value_offset);
                  (*ews_state->form_field_table[fieldp->field_type].free_f)(p);
                }
              } 
             
              continue;
            }

#         endif /* EW_CONFIG_OPTION_FORM_REPEAT */
          /*
           * If type might be dynamically allocated
           */
#         ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC

          /* If dynamic multiselect field
           * free all allocated substructures
           */

          if (fieldp->field_type == ewFieldTypeDynamicMultiSelect)
            {
#             ifdef EW_CONFIG_OPTION_FORM_REPEAT
              if (repeat_body == TRUE)
                {
                  p1 = ((uint8 *) context->form_status) + fieldp->status_offset;
                }
              else
                {
                  p1 = ((uint8 *) context->form) + fieldp->status_offset;
                }
#             else
              p1 = ((uint8 *) context->form) + fieldp->status_offset;
#             endif /* EW_CONFIG_OPTION_FORM_REPEAT */
              if ((*p1 & EW_FORM_DYNAMIC) != 0)
                {
                  msubfieldP = (EwFormFieldDynamicMultiSelect *)fieldp->default_value;
                  indx = *(uint32 *)(((uint8*)context->form) + msubfieldP->choices_offset) - 1;
                  if (ews_state->form_field_table[msubfieldP->field_type].free_f != NULL)
                    {
                      /*
                       * individual elements of the substructure have to be freed
                       */
                      p = (void *)(((uint8*)context->form) + fieldp->value_offset);
#                     ifdef EW_CONFIG_OPTION_FIELDTYPE_TEXT
                      if (msubfieldP->field_type == ewFieldTypeText)
                        {                         /* array of (char *)                */
                          ppch = *(char ***)p;     /* pointer to the array of pointers */
                          while ( indx >= 0 )
                            {

                              (*ews_state->form_field_table[msubfieldP->field_type].free_f)
                                (ppch+indx);
                              indx--;
                            }
                        }
#                     endif /* EW_CONFIG_OPTION_FIELDTYPE_TEXT */
#                     ifdef EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING
                      else if (msubfieldP->field_type == ewFieldTypeHexString)
                        {
                          phex = *(EwsFormFieldHexString **)p;
                          while ( indx >= 0 )
                            {

                              (*ews_state->form_field_table[msubfieldP->field_type].free_f)
                                (phex+indx);
                              indx--;
                            }
                        }
#                     endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */
#                     ifdef EW_CONFIG_OPTION_FIELDTYPE_OID
                      else if (msubfieldP->field_type == ewFieldTypeObjectID)
                        {
                          poid = *(EwsFormFieldObjectID **)p;
                          while ( indx >= 0 )
                            {

                              (*ews_state->form_field_table[msubfieldP->field_type].free_f)
                                (poid+indx);
                              indx--;
                            }
                        }
#                     endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */
                    }
                  /*
                   * free substructure
                   */
                  if (ews_state->form_field_table[fieldp->field_type].free_f != NULL)
                    {
                      p = (void *)(((uint8*)context->form) + fieldp->value_offset);
                      (*ews_state->form_field_table[fieldp->field_type].free_f)(p);
                    }
                }
            }
          else if (fieldp->field_type == ewFieldTypeDynamicSingleSelect)
            {
#             ifdef EW_CONFIG_OPTION_FORM_REPEAT
              if (repeat_body == TRUE)
                {
                  p1 = ((uint8 *) context->form_status) + fieldp->status_offset;
                }
              else
                {
                  p1 = ((uint8 *) context->form) + fieldp->status_offset;
                }
#             else
              p1 = ((uint8 *) context->form) + fieldp->status_offset;
#             endif /* EW_CONFIG_OPTION_FORM_REPEAT */
              if ((*p1 & EW_FORM_DYNAMIC) != 0)
                {
                  ssubfieldP = (EwFormFieldDynamicSingleSelect *)fieldp->default_value;
                  if (ews_state->form_field_table[ssubfieldP->field_type].free_f != NULL)
                    {
                      p = (void *)(((uint8*)context->form) + fieldp->value_offset);
                      (*ews_state->form_field_table[ssubfieldP->field_type].free_f)(p);
                    }
                }
            }
          else
#         endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */
            if (ews_state->form_field_table[fieldp->field_type].free_f != NULL)
              {
                /*
                 * Read status byte.  If marked as dynamic, invoke free fn.
                 */
#               ifdef EW_CONFIG_OPTION_FORM_REPEAT
                if (repeat_body == TRUE)
                  {
                    p1 = ((uint8 *) context->form_status) + fieldp->status_offset;
                  }
                else
                  {
                    p1 = ((uint8 *) context->form) + fieldp->status_offset;
                  }
#               else
                p1 = ((uint8 *) context->form) + fieldp->status_offset;
#               endif /* EW_CONFIG_OPTION_FORM_REPEAT */
                if ((*p1 & EW_FORM_DYNAMIC) != 0)
                  {
                    p = (void *)(((uint8*)context->form) + fieldp->value_offset);
                    (*ews_state->form_field_table[fieldp->field_type].free_f)(p);
                  }
              }
        }
    }

  /*
   * Release form
   */

  if (context->form)
    {
      ewaFree(context->form);
      context->form = NULL;
    }
}



/******************************************************************************
 * Field-Specific conversions
 *
 * To functions: Convert to HTML from application-defined form fields
 *
 * From functions: Convert from HTML to application-defined form fields
 *****************************************************************************/

#ifdef EW_CONFIG_OPTION_FIELDTYPE_RADIO
/*
 * ewFieldToRadioField
 * Write "CHECKED" to network if this radio field is selected
 *
 * context      - context of request
 * fieldp       - radio serve field
 *
 * No return value
 */
void
ewFieldToRadioField ( EwsContext context, const EwFormField *fieldp )
{
  int *ip;

    ip = (int *) ((uint8 *)context->form + fieldp->value_offset);
    if (*ip == fieldp->enum_value)
      {
        (void) ewsBodySend(context, "CHECKED");
      }
}
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
/*
 * ewFieldToFormRadioField
 * Write "CHECKED" to network if this radio field is selected
 * Remeber to specificly use base_form, not form.
 *
 * context      - context of request
 * fieldp       - radio serve field
 *
 * No return value
 */
void
ewFieldToFormRadioField ( EwsContext context, const EwFormField *fieldp )
{
  int *ip;
  EwFormRepeatRadioFormField *subfieldp;
  uintf iteration;

    subfieldp = (EwFormRepeatRadioFormField *)(fieldp->default_value);
    iteration = (uintf)(context->doc_stack_tail->rpt_body_iteration);
    ip = (int *) ((uint8 *)context->base_form + fieldp->value_offset);
    if ((*ip == fieldp->enum_value) &&
        (iteration == *(uintf *)((uint8 *)context->base_form +
                                 (subfieldp->instance_offset))))
      {
        (void) ewsBodySend(context, "CHECKED");
      }
}
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
#endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO */

#if defined (EW_CONFIG_OPTION_FIELDTYPE_RADIO)\
 || defined (EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE)
/*
 * ewFieldFromRadio
 * Parse radio selection into form structure enumerated type
 *
 * context      - context of request
 * fieldp       - radio submit field
 * stringp      - value of selection
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromRadio
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  int i = 0;
  int *ip;
  const int *ep;
  uint8 *sp;
  EwsString stmp;

  ep = fieldp->enum_list;
  ip = (int *) ((uint8 *)context->form + fieldp->value_offset);

  /*
   * Scan field-specific enum list for matching value
   */
  stmp = *stringp;
  while (*ep != 0)
    {
#     ifdef EMWEB_SANITY
      if ((uint32) *ep > context->form_enum_table_size)
        {
          EMWEB_ERROR(("ewFieldFormRadio: bad enum index\n"));
          return EWS_STATUS_FATAL_ERROR;
        }
#     endif /* EMWEB_SANITY */

      if (ewsStringCompare(&stmp
                           ,context->form_enum_table[*ep - 1]
                           ,ewsStringCompareCaseSensitive
                           ,NULL))
        {
          if (stmp.length == 0)
            {
              *ip = *ep;
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
              sp = ((uint8 *)context->form_status + fieldp->status_offset);
#else
              sp = ((uint8 *)context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
              *sp = EW_FORM_RETURNED;
              break;
            }
          else
            {
              stmp = *stringp;
            }
        }
      ep++;
      i++;
    }

  return EWS_STATUS_OK;

}

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
/*
 * ewFieldFromFormRadio
 * Parse radio selection into form structure enumerated type
 * Remember to use base_form.
 *
 * context      - context of request
 * fieldp       - radio submit field
 * stringp      - value of selection
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromFormRadio
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  int i = 0;
  int *ip;
  const int *ep;
  uint8 *sp;
  EwsString stmp;

  ep = fieldp->enum_list;
  ip = (int *) ((uint8 *)context->base_form + fieldp->value_offset);

  /*
   * Scan field-specific enum list for matching value
   */
  stmp = *stringp;
  while (*ep != 0)
    {
#     ifdef EMWEB_SANITY
      if ((uint32) *ep > context->form_enum_table_size)
        {
          EMWEB_ERROR(("ewFieldFormRadio: bad enum index\n"));
          return EWS_STATUS_FATAL_ERROR;
        }
#     endif /* EMWEB_SANITY */

      if (ewsStringCompare(&stmp
                           ,context->form_enum_table[*ep - 1]
                           ,ewsStringCompareCaseSensitive
                           ,NULL))
        {
          if (stmp.length == 0)
            {
              *ip = *ep;
              sp = ((uint8 *)context->base_form + fieldp->status_offset);
              *sp = EW_FORM_RETURNED;
              break;
            }
          else
            {
              stmp = *stringp;
            }
        }
      ep++;
      i++;
    }
  return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

/*
 * ewsFormEnumToString
 * Lookup string corresponding to enum from context
 *
 * context      - context of request
 * enum         - value of enumerator
 *
 * Returns string from corresponding HTML VALUE= field, or NULL if out of
 * bounds.
 */
const char *
ewsFormEnumToString( EwsContext context, int value )
{
    if (value > (int) context->form_enum_table_size || value <= 0)
      {
        return NULL;
      }
    return context->form_enum_table[value - 1];
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO | SELECT_SINGLE */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE
/*
 * ewFieldToSelectField
 * Write "SELECTED" to network if this is selected in a single select box.
 *
 * context      - context of request
 * fieldp       - radio serve field
 *
 * No return value
 */
void
ewFieldToSelectField ( EwsContext context, const EwFormField *fieldp )
{
  int *ip;

    ip = (int *) ((uint8 *)context->form + fieldp->value_offset);
    if (*ip == fieldp->enum_value)
      {
        (void) ewsBodySend(context, "SELECTED");
      }
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_MULTIPLE
/*
 * ewFieldToSelectMultiple
 * Write "SELECTED" to network if this value is  selected in a multiple
 * selection box.
 *
 * context      - context of request
 * fieldp       - select option field
 *
 * No return value
 */
void
ewFieldToSelectMulti ( EwsContext context, const EwFormField *fieldp )
{
  boolean *bp;

    bp = (boolean *) ((uint8 *)context->form + fieldp->value_offset);
    if (*bp)
      {
        (void) ewsBodySend(context, "SELECTED");
      }
}

/*
 * ewFieldFromSelectMulti
 * Parse multiple selection field into form structure boolean field
 *
 * context      - context of request
 * fieldp       - radio submit field
 * stringp      - value of selection
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromSelectMulti
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  boolean *bp;
  uint8 *sp;

  if (ewsStringCompare(stringp
                       ,fieldp->sel_value
                       ,ewsStringCompareCaseSensitive
                       ,NULL))
    {
      if (stringp->length == 0)
        {
          bp = (boolean *) ((uint8 *)context->form + fieldp->value_offset);
          *bp = TRUE;
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
          sp = ((uint8 *)context->form_status + fieldp->status_offset);
#else
          sp = ((uint8 *)context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
          *sp = EW_FORM_RETURNED;
        }
    }

  return EWS_STATUS_OK;

}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_MULTIPLE */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_CHECKBOX
/*
 * ewFieldToCheckbox
 * Write CHECKED to network if checkbox value is TRUE
 *
 * context      - context of request
 * fieldp       - checkbox field
 *
 * No return value
 */
void
ewFieldToCheckbox ( EwsContext context, const EwFormField *fieldp )
{
  boolean *bp;

    bp = (boolean *) ((uint8 *)context->form + fieldp->value_offset);
    if (*bp)
      {
        (void) ewsBodySend(context, "CHECKED");
      }
}

/*
 * ewFieldFromCheckbox
 * Set checkbox field in form to TRUE
 *
 * context      - context of request
 * fieldp       - checkbox field
 * stringp      - value (not used -- if present, checkbox is true)
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromCheckbox
  ( EwsContext context, const EwFormField *fieldp, EwsStringP string )
{
  boolean *bp;
  uint8 *sp;
  EW_UNUSED(string);

  /*
   * Field is present, so mark true and set returned status
   */
  bp = (boolean *) ((uint8 *)context->form + fieldp->value_offset);
  *bp = TRUE;
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
  sp = ((uint8 *) context->form_status + fieldp->status_offset);
#else
  sp = ((uint8 *) context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
  *sp = EW_FORM_RETURNED;

  return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_CHECKBOX */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_TEXT) || defined(EW_CONFIG_OPTION_FIELDTYPE_FILE)
/*
 * ewFieldToText
 * Write value of text field to network
 *
 * context      - context of request
 * fieldp       - text field
 *
 * No return value
 *
 * Used to output a string value literally.  That is, any contained
 * octets which are special for HTML/XML (i.e., [<>&'"])
 * are turned into character entities/references.  Note that octets
 * 128 and higher are *not* transformed, and CR and LF are *not*
 * transformed.
 * Note: This transformation assumes that the input encoding is '8-bit
 * ASCII' or some compatible character set, since the testing and
 * replacement are done on an octet basis.  In practice, USASCII,
 * ISO-8859-* (including Latin-1), and UTF-8 are compatible with this
 * scheme.  Two-byte encodings (including UTF-2/16 and some Asian
 * encodings) are not compatible with this scheme, but EmWeb does not
 * handle those encodings well anyway.
 * Note: If the string value contains octets which are not codes for legal
 * characters in XML documents (HT, CR, LF, SP-0xFF), the converted string
 * will not be legal as character content in XML documents.  No error will
 * be signalled in this case.
 * Note: Since CR and LF are represented by themselves in the output text
 * (as opposed to &#13; and &#10;) means that ends-of-lines will be
 * normalized by any recipient XML processor.  This is the correct
 * behavior if the information is "text" (as opposed to "binary"), since
 * it causes ends-of-lines to be normalized during transmission, to be
 * converted to locale-specific format by the receiving application.
 * "Text" interpretation was chosen because it is the more common case,
 * alternative encodings (hex and base64) are available for binary data,
 * and sending binary (even as character references) is not valid in XML
 * (since, e.g., &#1; may not appear in an XML document).
 */
void
ewFieldToText ( EwsContext context, const EwFormField *fieldp )
{
  char *cp;
  char s[2];

  /*
   * Insert quoted string into outbound data stream, quoting special
   * characters as needed
   */
  s[1] = '\0';
  cp = *(char **) ((uint8 *)context->form + fieldp->value_offset);
  if (cp == NULL)
    {
      return;
    }
  while (*cp != '\0')
    {
#ifndef EW_CONFIG_OPTION_NO_ENTITY_TRANSLATION
      if (*cp == '&')
        {
          ewsBodySend(context, "&amp;");
        }
      else if (*cp == '<')
        {
          ewsBodySend(context, "&lt;");
        }
      else if (*cp == '>')
        {
          ewsBodySend(context, "&gt;");
        }
      else if (*cp == '"')
        {
          ewsBodySend(context, "&quot;");
        }
      else if (*cp == '\'')
        {
          ewsBodySend(context, "&apos;");
        }
      else
#endif /*EW_CONFIG_OPTION_NO_ENTITY_TRANSLATION */
        {
          s[0] = *cp;
          ewsBodySend(context, s);
        }
      cp++;
    }
}

/*
 * ewFieldFromText
 * Read value of text field into form structure
 *
 * context      - context of request
 * fieldp       - text field
 * stringp      - value of text field
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromText ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  char **cpp;
  uint8 *sp;
  uintf len = ewsStringLength(stringp);

  /*
   * Field is present, so mark true and set returned status
   */
  cpp = (char **) ((uint8 *)context->form + fieldp->value_offset);
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
  sp = ((uint8 *) context->form_status + fieldp->status_offset);
#else
  sp = ((uint8 *) context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
  if (*sp & EW_FORM_RETURNED)
    {
      *sp |= EW_FORM_PARSE_ERROR;
      return EWS_STATUS_ALREADY_EXISTS;
    }
  *cpp = (char *) ewaAlloc(len + 1);
  if (*cpp == NULL)
    {
      *sp |= EW_FORM_PARSE_ERROR;
    }
  else
    {
      ewsStringCopyOut(*cpp, stringp, len);
      *sp = EW_FORM_RETURNED | EW_FORM_DYNAMIC;
    }

  return EWS_STATUS_OK;
}

/*
 * ewFieldFreeText
 * Release resources allocated by text field submissiojn
 *
 * p    - pointer to text value
 *
 * No return value
 */
void
ewFieldFreeText ( void * p )
{
  char **cpp = (char **) p;
    ewaFree(*cpp);
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_TEXT/FILE */

#endif /* EW_CONFIG_OPTION_FORM */

#if defined(EW_CONFIG_OPTION_FORM) ||\
    defined(EW_CONFIG_OPTION_STRING_TYPED)
/*
 * ewFieldToDecimalUint
 * Write unsigned integer value as text to network from form structure
 *
 * context      - context of request
 * fieldp       - unsigned integer field
 *
 * No return value
 */
void
ewFieldToDecimalUint ( EwsContext context, const EwFormField *fieldp )
{
  uint32 *up;
  char *ltoa_ptr;
  char ltoa_string[12];

    up = (uint32 *) ((uint8 *) context->form + fieldp->value_offset);
    EMWEB_LTOA(ltoa_ptr, *up, ltoa_string, sizeof(ltoa_string));
    ewsBodySend(context, ltoa_ptr);
}

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT)
/*
 * ewFieldToDecimalInt
 * Write signed integer value as text to network from form structure
 *
 * context      - context of request
 * fieldp       - unsigned integer field
 *
 * No return value
 */
void
ewFieldToDecimalInt ( EwsContext context, const EwFormField *fieldp )
{
  int32 *up;
  char *ltoa_ptr;
  char ltoa_string[12];

    up = (int32 *) ((uint8 *) context->form + fieldp->value_offset);
    if (*up < 0)
      {
        ewsBodySend(context, "-");
        *up = -*up;
      }
    EMWEB_LTOA(ltoa_ptr, (uint32) *up, ltoa_string, sizeof(ltoa_string));
    ewsBodySend(context, ltoa_ptr);
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_HEX_INT)
/*
 * ewFieldToHexInt
 * Write hex integer value as text to network from form structure
 *
 * context      - context of request
 * fieldp       - unsigned integer field
 *
 * No return value
 */
void
ewFieldToHexInt ( EwsContext context, const EwFormField *fieldp )
{
  uint32 *up;
  char *ltoa_ptr;
  char ltoa_string[12];

    up = (uint32 *) ((uint8 *) context->form + fieldp->value_offset);
    EMWEB_LTOAX(ltoa_ptr, (uint32) *up, ltoa_string, sizeof(ltoa_string));
    ewsBodySend(context, ltoa_ptr);
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_INT */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP)
/*
 * ewFieldToDottedIP
 * Write dotted IP to network from 32-bit IP address in form
 *
 * context      - context of request
 * fieldp       - dotted IP field
 *
 * No return value
 */
void
ewFieldToDottedIP ( EwsContext context, const EwFormField *fieldp )
{
  uint32 *ipp;
  uint32 ip;
  char *ltoa_ptr;
  char ltoa_string[4];

    /*
     * Format: a.b.c.d
     */
    ipp = (uint32 *) ((uint8 *) context->form + fieldp->value_offset);
    ip = *ipp;
    EMWEB_LTOA(ltoa_ptr, ip >> 24, ltoa_string, sizeof(ltoa_string));
    ewsBodySend(context, ltoa_ptr);
    ewsBodySend(context, ".");
    EMWEB_LTOA(ltoa_ptr, (ip >> 16) & 0xff, ltoa_string, sizeof(ltoa_string));
    ewsBodySend(context, ltoa_ptr);
    ewsBodySend(context, ".");
    EMWEB_LTOA(ltoa_ptr, (ip >> 8) & 0xff, ltoa_string, sizeof(ltoa_string));
    ewsBodySend(context, ltoa_ptr);
    ewsBodySend(context, ".");
    EMWEB_LTOA(ltoa_ptr, ip & 0xff, ltoa_string, sizeof(ltoa_string));
    ewsBodySend(context, ltoa_ptr);
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING)
/*
 * ewFieldToHexString
 * Write hex string to network from array of octets in form
 *
 * context      - context of request
 * fieldp       - hex string field
 *
 * No return value
 */
void
ewFieldToHexString ( EwsContext context, const EwFormField *fieldp )
{
  EwsFormFieldHexString *hsp;

  hsp = (EwsFormFieldHexStringP) ((uint8 *)context->form +
          fieldp->value_offset);
  if (hsp == NULL) return;
  ewsFormWriteHex(context, hsp->datap, hsp->length, '\0');
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC)\
 || defined(EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
/*
 * ewFieldToIEEEMAC
 * Write IEEE MAC to network from 6-byte MAC address in form
 *
 * context      - context of request
 * fieldp       - IEEE MAC field
 *
 * No return value
 */
void
ewFieldToIEEEMAC ( EwsContext context, const EwFormField *fieldp )
{
  uint8 *mp;

    mp = ((uint8 *)context->form + fieldp->value_offset);
    ewsFormWriteHex(context, mp, 6, ':');
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_IEEE|STD_MAC */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC)
/*
 * ewFieldToFDDIMAC
 * Write FDDI MAC to network from 6-byte MAC address in form
 *
 * context      - context of request
 * fieldp       - FDDI MAC field
 *
 * No return value
 */
void
ewFieldToFDDIMAC ( EwsContext context, const EwFormField *fieldp )
{
  uint8 *mp;

    mp = ((uint8 *)context->form + fieldp->value_offset);
    ewsFormWriteHex(context, mp, 6, '-');
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV)
/*
 * ewFieldToDecnetIV
 * Write dotted decnet address to network from 16-bit address in form
 *
 * context      - context of request
 * fieldp       - dotted decnet iv field
 *
 * No return value
 */
void
ewFieldToDecnetIV ( EwsContext context, const EwFormField *fieldp )
{
  uint16 *decp;
  uint32 dec;
  char *ltoa_ptr;
  char ltoa_string[5];

    decp = (uint16 *) ((uint8 *) context->form + fieldp->value_offset);
    dec = ((*decp >> 8) | (*decp << 8)) & 0xffff;
    EMWEB_LTOA(ltoa_ptr, dec >> 10, ltoa_string, sizeof(ltoa_string));
    ewsBodySend(context, ltoa_ptr);
    ewsBodySend(context, ".");
    EMWEB_LTOA(ltoa_ptr, dec & 0x3ff, ltoa_string, sizeof(ltoa_string));
    ewsBodySend(context, ltoa_ptr);
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV */

#if defined (EW_CONFIG_OPTION_FIELDTYPE_OID)
/*
 * ewFieldToObjectID
 * Write dotted object ID to network from EwsFormFieldObjectID in form
 *
 * context      - context of request
 * fieldp       - object id field
 *
 * No return value
 */
void
ewFieldToObjectID ( EwsContext context, const EwFormField *fieldp )
{
  EwsFormFieldObjectIDP oidp;
  char ltoa_string[11];
  char *ltoa_ptr;
  uint32 n;

    oidp = (EwsFormFieldObjectIDP)
           ((uint8 *) context->form + fieldp->value_offset);
    for (n = 0; n < oidp->length; n++)
      {
        EMWEB_LTOA(ltoa_ptr, oidp->datap[n], ltoa_string, sizeof(ltoa_string));
        ewsBodySend(context, ltoa_ptr);
        if (n != oidp->length - 1)
          {
            ewsBodySend(context, ".");
          }
      }
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */

#if defined(EW_CONFIG_OPTION_SOAP)
/*
 * ewuFieldToBoolean
 * Write UPnP boolean value as text to network from form structure
 *
 * context      - context of request
 * fieldp       - boolean field
 *
 * No return value
 *
 * Used for true/false values.
 * Allowed values on input are: 0, f, false, n, no; 1, t, true, y, yes
 * (case-insensitive) (set of values may be enlarged over time).
 * Generated values are: 0; 1 (for compatibility with Windows Me UPnP,
 * which takes less than the required set of values).
 */
void
ewuFieldToBoolean ( EwsContext context, const EwFormField *fieldp )
{
  boolean *bp;

  bp = (boolean *) ((uint8 *) context->form + fieldp->value_offset);
  /* Send either 1 or 0, because those are the only encodings that Windows Me
   * can deal with. */
  ewsBodySend(context, *bp ? "1" : "0");
}


/*
 * ewuFieldToUUID
 * Write UUID value as text to network from form structure
 *
 * context      - context of request
 * fieldp       - unsigned integer field
 *
 * No return value
 *
 * Used to represent a 16-octet binary value in UUID format.
 * On input, converts case-insensitive hex characters to octets, ignoring
 * any embedded hyphens.
 * On output, converts the octets to lower-case hex characters and inserts
 * hyphens in the standard locations.
 * The internal (binary) form of the UUID is a sequence of 16 octets.
 * This does not match the structure in draft-leach-uuids-guids-01.txt,
 * but converting the byte-order to the structure format would involve
 * byte-order conversions for which EmWeb does not have configuration
 * information.
 * The standard hyphenation is:
 *      00000000-0000-0000-0000-000000000000
 */
void
ewuFieldToUUID ( EwsContext context, const EwFormField *fieldp )
{
  EwsFormFieldUUIDP uuidp;

  uuidp = (EwsFormFieldUUIDP) ((uint8 *) context->form + fieldp->value_offset);

  /* Write in the format:
   *    f81d4fae-7dec-11d0-a765-00a0c91e6bf6
   */
  ewsFormWriteHex(context, (uint8 *) uuidp, 4, '\0');
  ewsBodySend(context, "-");
  ewsFormWriteHex(context, ((uint8 *) uuidp) + 4, 2, '\0');
  ewsBodySend(context, "-");
  ewsFormWriteHex(context, ((uint8 *) uuidp) + 6, 2, '\0');
  ewsBodySend(context, "-");
  ewsFormWriteHex(context, ((uint8 *) uuidp) + 8, 2, '\0');
  ewsBodySend(context, "-");
  ewsFormWriteHex(context, ((uint8 *) uuidp) + 10, 6, '\0');
}

/*
 * ewFieldToBase64
 * Write bytes to network using base 64 conversion from form structure.
 *
 * context      - context of request
 * fieldp       - base 64 field
 *
 * No return value
 *
 * Used to represent a sequence of octets using base64 notation.
 * On input, allows padding with whitespace (SP, HT, CR, LF).
 * (All other non-base64 characters are ignored for decoding purposes
 * (per RFC 1341 section 5.2), but may be flagged as an error.)
 * On output, generates lines of at most 76 characters (ibid), separated
 * or ended by CR-LF.
 */
void
ewuFieldToBase64 ( EwsContext context, const EwFormField *fieldp )
{
  EwsFormFieldHexString *hsp;
  const uint8 *pointer;
  int32 length;
  int groups_on_line, j;
  uint8 byte1, byte2, byte3;
  char chars[5];

  hsp = (EwsFormFieldHexStringP) ((uint8 *)context->form +
          fieldp->value_offset);
  if (hsp == NULL) return;

  pointer = hsp->datap;
  length = hsp->length;
  chars[4] = '\0';

  /* Work through the bytes in groups of 3.  One last iteration of this loop
   * for a group of 1 or 2 bytes at the end of processing.
   */
  for (groups_on_line = 0; length > 0;
       length -= 3, pointer += 3, groups_on_line++)
    {
      /* Write CR-LF if we have written 19 groups to the line and will write
       * another one. */
      if (groups_on_line >= 19)
        {
          ewsBodySend(context, "\r\n");
          groups_on_line = 0;
        }
      /* Get the byte group. */
      byte1 = pointer[0];
      byte2 = (length >= 2) ? pointer[1] : 0;
      byte3 = (length >= 3) ? pointer[2] : 0;
      /* Compose the 6-bit values. */
      chars[0] = (byte1 >> 2) & 0x3F;
      chars[1] = ((byte1 << 4) | (byte2 >> 4)) & 0x3F;
      chars[2] = ((byte2 << 2) | (byte3 >> 6)) & 0x3F;
      chars[3] = byte3 & 0x3F;
      /* Translate the 6-bit values to characters. */
      for (j = 0; j < 4; j++)
        {
          chars[j] =
            (chars[j] <= 25) ? chars[j] + 'A' :
            (chars[j] <= 51) ? chars[j] + 'a' - 26 :
            (chars[j] <= 61) ? chars[j] + '0' - 52 :
            (chars[j] == 62) ? '+' :
            '/';
        }
      /* Turn the last characters to '=' if this is the end of the string. */
      switch (length)
        {
        case 1:
          chars[2] = '=';
          /* fall through */
        case 2:
          chars[3] = '=';
          break;
        default:
          ; /* no action */
        }
      /* Write the characters out. */
      ewsBodySend(context, chars);
    }
}
#endif /* EW_CONFIG_OPTION_SOAP */

#if defined(EW_CONFIG_OPTION_FORM) || defined(EW_CONFIG_SCHEMA_ARCHIVE)
/*
 * ewFieldToDynamic
 * Write bytes to network using dynamic type
 *
 * context      - context of request
 * fieldp       - indirect type, dispatches to some other type as specified
 *                by the value structure.
 *
 * No return value
 *
 * Indirectly dispatches to another ewFieldToXXX routine based on the
 * type specified
 */
void
ewFieldToDynamic ( EwsContext context, const EwFormField *fieldp )
{
  EwsFormFieldDynamic *dp;
  EwFormField *writable_fieldp = (EwFormField *) fieldp;
  int type;

  dp = (EwsFormFieldDynamicP) ((uint8 *)context->form + fieldp->value_offset);
  type = ewsRendering2Field(dp->type);
  if (type < 0)
    {
      EMWEB_WARN(("ewFieldToDynamic: bad type %d\n", dp->type));
      ewsBodySend(context, "EMWEB_BAD_TYPE");
      return;
    }
  /*
   * Text fields are handled specially.  In all other cases, the data pointer
   * of the dynamic field data structure points to a memory location which is
   * in the format of a form structure entry for that data type.  For text,
   * the data pointer points to the string characters.  That is, the address
   * of the data pointer "points to a memory location which is in the format
   * of a form structure entry for that data type".
   */
  writable_fieldp->value_offset =   (ewFieldTypeText == type ?
                                       (uint8 *) &dp->datap :
                                       (uint8 *)  dp->datap
                                    ) - ((uint8 *) context->form);
  if (ews_state->form_field_table[type].to_f != NULL)
    {
      ews_state->form_field_table[type].to_f(context, fieldp);
    }
  else
    {
      EMWEB_WARN(("ewFieldToDynamic: type %d not configured\n", dp->type));
      ewsBodySend(context, "EMWEB_UNSUPPORTED");
    }
}
#endif /* EW_CONFIG_OPTION_FORM || EW_CONFIG_SCHEMA_ARCHIVE */

#if defined (EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING)\
 || defined (EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC)\
 || defined (EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC)\
 || defined (EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
/*
 * ewsFormWriteHex
 * Write hex into network buffers with seperator character between bytes,
 * used by hex string and MAC address "To" functions.
 *
 * dst          - context->res_str_end: estring of outbound buffers
 * src          - source of raw data to be converted to hexadecimal
 * length       - number of bytes of data
 * sep          - seperator character, or '\0' for none.
 *
 * No return value
 */
void
ewsFormWriteHex ( EwsContext context, const uint8 *src, uint32 length, char sep )
{
  uintf nybble;
  char string[4];

    string[2] = sep;
    string[3] = '\0';
    while (length > 0)
      {
        nybble = *src >> 4;
        string[0] = nybble + (nybble > 9? 'A'-10 : '0');
        nybble = *src & 0x0f;
        string[1] = nybble + (nybble > 9? 'A'-10 : '0');
        if (length == 1) string[2] = '\0';
        ewsBodySend(context, string);
        length--;
        src++;
      }
}
#endif /* HEX_INT|HEX_STRING|IEEE_MAC|FDDI_MAC|STD_MAC */

#endif /* EW_CONFIG_OPTION_FORM | EW_CONFIG_OPTION_STRING_TYPED */

#ifdef EW_CONFIG_OPTION_FORM

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT)
/*
 * ewFieldFromDecimal
 * Read an signed integer value from a text field into the form structure.
 * Valid range is  -2147483648 .. 2147483647
 *
 * context      - context of request
 * fieldp       - unsigned integer text field
 * stringp      - value of text field
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromDecimal
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint32 value;
  int32 *up;
  uint8 *sp;
  EwsString temp;   /* copy of stringp to preserve original */
  boolean minus;    /* TRUE if first char is "-" */

# ifdef EW_CONFIG_OPTION_FORM_REPEAT
  sp = ((uint8 *) context->form_status) + fieldp->status_offset;
# else
  sp = ((uint8 *) context->form) + fieldp->status_offset;
# endif /* EW_CONFIG_OPTION_FORM_REPEAT */
  up = (int32 *) ((uint8 *) context->form + fieldp->value_offset);

  temp = *stringp;
  minus = (ewsStringGetChar( &temp ) == '-');

  /*
   * note on conversion:
   * ewsStringGetInteger will verify that, if a '-' is present, that the
   * result is no less than -2147483648.  So the lowest neg bound is
   * checked by ewsStringGetInteger.
   * ewsStringGetInteger will accept the text "4294967296" as a valid
   * integer (assuming unsigned).  This is correct for most users of
   * ewsStringGetInteger... except this routine!
   * The text "4294967296" is undefined for a SIGNED number, and should
   * be a parse error.  As a matter of fact, the largest positive signed
   * 32 bit number is 2147483647.  If the user attempts to enter an
   * number greater than this, it is a parse error.
   * Note the shifting in the check below.  This is done to prevent
   * compiler warnings on compilers whose int is 16 bits (trust me).
   * Also, I treat the number as unsigned until I am ready to return
   * it, so this range check works.
   */
  if (   ewsStringGetInteger(stringp, 10, &value)   /* if converts OK */
      && (ewsStringLength(stringp) == 0)            /* and no trailing crap */
      && (   minus                               /* if neg, OK, else check max pos */
          || (value <= ((((uint32)0x7FFF)<<16) | 0xFFFF)) /* 2147483647 */
         )
     )
    {
      *sp |= EW_FORM_RETURNED;
      *up = (int32) value;
      return EWS_STATUS_OK;
    }
  *sp |= EW_FORM_PARSE_ERROR;

  return EWS_STATUS_BAD_SYNTAX;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_UINT)\
 || defined(EW_CONFIG_OPTION_FIELDTYPE_IMAGE)
/*
 * ewFieldFromUDecimal
 * Read an unsigned integer value from a text field into the form structure.
 * Valid range is 0..4294967296
 *
 * context      - context of request
 * fieldp       - unsigned integer text field
 * stringp      - value of text field
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromUDecimal
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint32 value;
  uint32 *up;
  uint8 *sp;
  EwsString temp;

#   ifdef EW_CONFIG_OPTION_FORM_REPEAT
    sp = ((uint8 *) context->form_status) + fieldp->status_offset;
#   else
    sp = ((uint8 *) context->form) + fieldp->status_offset;
#   endif /* EW_CONFIG_OPTION_FORM_REPEAT */
    up = (uint32 *) ((uint8 *) context->form + fieldp->value_offset);

    /*
     * verify that the user did not enter a "-" sign, and
     * attempt to convert the whole integer
     */
    temp = *stringp;   /* make temp copy to not disturb original */

    if (   ('-' != ewsStringGetChar( &temp ))          /* if not negative */
        && (ewsStringGetInteger(stringp, 10, &value))  /* and successful conversion */
        && (ewsStringLength(stringp) == 0)             /* and no trailing text */
       )
      {
        *sp |= EW_FORM_RETURNED;
        *up = value;
        return EWS_STATUS_OK;
      }

    *sp |= EW_FORM_PARSE_ERROR;
    return EWS_STATUS_BAD_SYNTAX;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_UINT | IMAGE */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_HEX_INT)
/*
 * ewFieldFromHexInt
 * Read a hex integer value from a text field into the form structure
 *
 * context      - context of request
 * fieldp       - unsigned integer text field
 * stringp      - value of text field
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromHexInt
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint32 value;
  uint32 *up;
  uint8 *sp;

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
    sp = ((uint8 *) context->form_status) + fieldp->status_offset;
#else
    sp = ((uint8 *) context->form) + fieldp->status_offset;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
    up = (uint32 *) ((uint8 *) context->form + fieldp->value_offset);
    if (ewsStringLength(stringp) <= 8 &&
        ewsStringGetInteger(stringp, 16, &value))
      {
        if (ewsStringLength(stringp) == 0)
          {
            *sp |= EW_FORM_RETURNED;
            *up = value;
            return EWS_STATUS_OK;
          }
      }
    *sp |= EW_FORM_PARSE_ERROR;
    return EWS_STATUS_BAD_SYNTAX;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_INT */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP)
/*
 * ewFieldFromDottedIP
 * Read a dotted ip value from a text field into the form structure
 *
 * context      - context of request
 * fieldp       - dotted ip text field
 * stringp      - value of text field
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromDottedIP
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint32 octet;
  uint8 *sp;
  uint32 *ipp;
  uintf count;

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
    sp = ((uint8 *) context->form_status) + fieldp->status_offset;
#else
    sp = ((uint8 *) context->form) + fieldp->status_offset;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
    ipp = (uint32 *)(((uint8 *)context->form) + fieldp->value_offset);


      /*
       * For each byte
       */
      for (count = 4; count > 0; count--)
        {
          /*
           * Read a component
           */
          if (!ewsStringGetInteger(stringp, 10, &octet))
            {
              *sp |= EW_FORM_PARSE_ERROR;
              return EWS_STATUS_BAD_SYNTAX;
            }

          /*
           * Must be within a byte value
           */
          if (octet > 255)
            {
              *sp |= EW_FORM_PARSE_ERROR;
              return EWS_STATUS_BAD_SYNTAX;
            }

          /*
           * Seperated by '.'
           */
          if (count != 1 && ewsStringGetChar(stringp) != '.')
            {
              *sp |= EW_FORM_PARSE_ERROR;
              return EWS_STATUS_BAD_SYNTAX;
            }
          *ipp = ((*ipp) << 8) | octet;
        }

    if (ewsStringLength(stringp) != 0)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_BAD_SYNTAX;
      }
    *sp |= EW_FORM_RETURNED;
    return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING)
/*
 * ewFieldFromHexString
 * Read a hex string value from a text field into the form structure
 *
 * context      - context of request
 * fieldp       - unsigned hex string text field
 * stringp      - value of text field
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromHexString
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint8 *sp;
  EwsFormFieldHexString *hsp;
  uint8 *dp;
  uint32 length;
  char sep;

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
    sp = ((uint8 *) context->form_status + fieldp->status_offset);
#else
    sp = ((uint8 *) context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
    if (*sp & EW_FORM_RETURNED)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_ALREADY_EXISTS;
      }
    hsp = (EwsFormFieldHexStringP) ((uint8 *)context->form +
            fieldp->value_offset);
    length = ewsStringLength(stringp)/2 + 1;
    hsp->datap = dp = (uint8 *) ewaAlloc(length);
    if (dp == NULL)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_NO_RESOURCES;
      }
    hsp->length = ewsFormReadHex(dp, stringp, length, &sep, TRUE);
    if (   ((int) hsp->length) < 0
        || (sep != '\0' && sep != ':' && sep != '-' && sep != ' ')
       )
      {
        *sp |= EW_FORM_PARSE_ERROR;
        ewaFree(dp);
        return EWS_STATUS_BAD_SYNTAX;
      }

    *sp |= EW_FORM_RETURNED | EW_FORM_DYNAMIC;
    return EWS_STATUS_OK;
}

void
ewFieldFreeHexString ( void * p )
{
  EwsFormFieldHexString *hsp = (EwsFormFieldHexStringP) p;

    ewaFree((void*)hsp->datap);
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_OID
void
ewFieldFreeObjectID ( void * p )
{
  EwsFormFieldObjectID *oidp = (EwsFormFieldObjectIDP) p;
    ewaFree(oidp->datap);
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
void
ewFieldFreeDynamicMultiSel ( void * p )
{
  uint8 *memP;

  memP = *(uint8 **)p;
  ewaFree(memP);
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC)\
 || defined(EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC)\
 || defined(EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
/*
 * ewFieldFromMAC
 * Read a MAC value from a text field into the form structure
 *
 * context      - context of request
 * fieldp       - MAC address text field
 * stringp      - value of text field
 *
 *  Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromMAC
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint8 *sp;
  uint8 *mp;
  char sep;

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
    sp = ((uint8 *)context->form_status + fieldp->status_offset);
#else
    sp = ((uint8 *)context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
    mp = ((uint8 *)context->form + fieldp->value_offset);
    if (ewsFormReadHex(mp, stringp, 6, &sep, TRUE) != 6 ||
          (sep != '\0' && sep != ':' && sep != '-'))
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_BAD_SYNTAX;

      }
    else
      {
        *sp |= EW_FORM_RETURNED;
        return EWS_STATUS_OK;
      }
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_IEEE|FDDI|STD_MAC */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
/*
 * ewFieldFromStdMAC
 * Read a MAC value from a text field into the form structure and bit-swap
 * if little-endian
 *
 * context      - context of request
 * fieldp       - MAC address text field
 * stringp      - value of text field
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromStdMAC
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint8 *sp;
  uint8 *mp;
  char sep;
  uintf i;
  static const uint8 nybble_map[16] = {0x0,0x8,0x4,0xc,0x2,0xa,0x6,0xe,
                                       0x1,0x9,0x5,0xd,0x3,0xb,0x7,0xf};

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
    sp = ((uint8 *)context->form_status + fieldp->status_offset);
#else
    sp = ((uint8 *)context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
    mp = ((uint8 *)context->form + fieldp->value_offset);
    if (ewsFormReadHex(mp, stringp, 6, &sep, TRUE) != 6 ||
          (sep != '\0' && sep != ':' && sep != '-'))
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_BAD_SYNTAX;
      }
    if (sep == '-')
      {
        for (i = 6; i > 0; i--)
          {
            *mp = nybble_map[*mp >> 4] | (nybble_map[*mp & 15] << 4);
            mp++;
          }
      }
    *sp |= EW_FORM_RETURNED;
    return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_STD_MAC */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV)
/*
 * ewFieldFromDecnetIV
 * Read a decnet address value from a text field into the form structure
 *
 * context      - context of request
 * fieldp       - decnet field
 * stringp      - value of text field
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromDecnetIV
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint32 area;
  uint32 node;
  uint8 *sp;
  uint16 *decp;
  uint16 dec;

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
    sp = ((uint8 *) context->form_status) + fieldp->status_offset;
#else
    sp = ((uint8 *) context->form) + fieldp->status_offset;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
    decp = (uint16 *)(((uint8 *)context->form) + fieldp->value_offset);
    if (!ewsStringGetInteger(stringp, 10, &area))
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_BAD_SYNTAX;
      }
    if (area > 63)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_BAD_SYNTAX;
      }
    if (ewsStringGetChar(stringp) != '.')
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_BAD_SYNTAX;
      }
    if (!ewsStringGetInteger(stringp, 10, &node))
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_BAD_SYNTAX;
      }
    if (node > 1023)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_BAD_SYNTAX;
      }
    if (ewsStringLength(stringp) != 0)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_BAD_SYNTAX;
      }
    dec = (uint16)((area << 10)|node);
    *decp = (dec >> 8) | (dec << 8);
    *sp |= EW_FORM_RETURNED;
    return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_OID)
EwsStatus
ewFieldFromObjectID
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint8 *sp;
  EwsFormFieldObjectID *oidp;
  uint32 *dp;
  uint32 length;
  uint32 data;
  uintf c;

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
    sp = ((uint8 *) context->form_status + fieldp->status_offset);
#else
    sp = ((uint8 *) context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
    if (*sp & EW_FORM_RETURNED)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_ALREADY_EXISTS;
      }
    oidp = (EwsFormFieldObjectIDP) ((uint8 *)context->form +
            fieldp->value_offset);
    length = ewsStringLength(stringp);
    length = length / 2 + 2;
    oidp->datap = dp = (uint32 *) ewaAlloc(length * sizeof(uint32));
    oidp->length = 0;
    if (dp == NULL)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_NO_RESOURCES;
      }
    while (ewsStringLength(stringp) != 0)
      {
        if (!ewsStringGetInteger(stringp, 10, &data))
          {
            *sp |= EW_FORM_PARSE_ERROR;
            ewaFree(oidp->datap);
            return EWS_STATUS_BAD_SYNTAX;
          }
        *dp++ = data;
        oidp->length++;
        if (ewsStringLength(stringp) != 0)
          {
            c = ewsStringGetChar(stringp);
            if (c != '.' && c != EWS_STRING_EOF)
              {
                *sp |= EW_FORM_PARSE_ERROR;
                ewaFree(oidp->datap);
                return EWS_STATUS_BAD_SYNTAX;
              }
          }
      }
    *sp = EW_FORM_RETURNED | EW_FORM_DYNAMIC;
    return EWS_STATUS_OK;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
/*
 * ewFieldFromDynamicSingleSel
 *
 * This function converts value from the incomming string
 * and saves it in the form structure
 *
 */
EwsStatus
ewFieldFromDynamicSingleSel
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  EwFormFieldDynamicSingleSelect *single_fieldP;

  single_fieldP =( EwFormFieldDynamicSingleSelect *)fieldp->default_value;
  if (ews_state->form_field_table[single_fieldP->field_type].from_f != NULL)
    (*ews_state->form_field_table[single_fieldP->field_type].from_f)
      (context, fieldp, stringp);
  return EWS_STATUS_OK;
}

/*
 * ewFieldFromDynamicMultiSel
 *
 * This function converts value from the incomming string
 * and saves it in the fotm structure
 *
 */
EwsStatus
ewFieldFromDynamicMultiSel
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  EwFormFieldDynamicMultiSelect *multi_fieldP;
  uint8 *sp, sptmp;
  char **cpp;
  EwsStatus returnValue;
#ifdef EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING
  EwsFormFieldHexString *phex;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */
#ifdef EW_CONFIG_OPTION_FIELDTYPE_OID
  EwsFormFieldObjectID *poid;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */
#if defined (EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC)\
 || defined (EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC)\
 || defined (EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
  uint8 *mp;
#endif
  EwFormField field;
  uintf max_options;
  EwFieldType sub_type;

  multi_fieldP =( EwFormFieldDynamicMultiSelect *)fieldp->default_value;
  sub_type = multi_fieldP->field_type;

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
  sp = ((uint8 *) context->form_status + fieldp->status_offset);
#else
  sp = ((uint8 *) context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
  cpp = (char **) ((uint8 *)context->form + fieldp->value_offset);

  if (ews_state->form_field_table[sub_type].from_f == NULL)
    {
    /*
     * If conversion function was excluded by configuration from the
     * server build, mark an error.
     */
     EMWEB_WARN(("ewFieldFromUnsupported: EMWEB_TYPE unsupported\n"));
     *sp = EW_FORM_PARSE_ERROR;
     return EWS_STATUS_FATAL_ERROR;
     }

  max_options = multi_fieldP->max_options;
  if (max_options == 0)
    {
#     ifdef EW_CONFIG_OPTION_DYNAMIC_SELECT_MAX_OPTS
      max_options = EW_CONFIG_OPTION_DYNAMIC_SELECT_MAX_OPTS;
#     else
      max_options = EWS_DYNAMIC_SELECT_MAX_CHOICES;
#     endif
    }

  if (*sp == 0)
    {                           /* allocate an array of types */
      *cpp = (char *) ewaAlloc(
                        (ews_state->form_field_table[sub_type].field_size)
                        * max_options);
      if (*cpp == NULL)
        {
          *sp = EW_FORM_PARSE_ERROR;
          return EWS_STATUS_NO_RESOURCES;
        }
      *(uintf *) ((uint8 *) context->form + multi_fieldP->choices_offset) = 0;
      *sp = EW_FORM_DYNAMIC;
    }
  else
    {
      if (*sp & EW_FORM_PARSE_ERROR)
        return EWS_STATUS_BAD_SYNTAX;
      if (*((uintf *) ((uint8 *) context->form + multi_fieldP->choices_offset))
         == max_options)
        {
          *sp |= EW_FORM_PARSE_ERROR;
          return EWS_STATUS_NO_RESOURCES;
        }
    }
#ifdef EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING
  if (sub_type == ewFieldTypeHexString)
    {
      phex = *(EwsFormFieldHexString **)cpp + (*(uintf *) ((uint8 *) context->form
                           + multi_fieldP->choices_offset));
      field.value_offset = ((uint8 *) (phex)) - ((uint8 *) context->form);
    }
  else
#endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */
#ifdef EW_CONFIG_OPTION_FIELDTYPE_OID
  if (sub_type == ewFieldTypeObjectID)
    {
      poid = *(EwsFormFieldObjectID **)cpp + (*(uintf *) ((uint8 *) context->form
                           + multi_fieldP->choices_offset));
      field.value_offset = ((uint8 *) (poid)) - ((uint8 *) context->form);
    }
  else
#endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */
#ifdef EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC
  if (sub_type == ewFieldTypeIEEEMAC)
    {
      mp = (uint8 *)(*(EwsFormFieldMAC **)cpp
                       + (*(uintf *) ((uint8 *) context->form
                       + multi_fieldP->choices_offset)));
      field.value_offset = ((uint8 *) (mp)) - ((uint8 *) context->form);
    }
  else
#endif /* EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC */
#ifdef EW_CONFIG_OPTION_FIELDTYPE_STD_MAC
  if (sub_type == ewFieldTypeStdMAC)
    {
      mp = (uint8 *)(*(EwsFormFieldMAC **)cpp
                       + (*(uintf *) ((uint8 *) context->form
                       + multi_fieldP->choices_offset)));
      field.value_offset = ((uint8 *) (mp)) - ((uint8 *) context->form);
    }
  else
#endif /* EW_CONFIG_OPTION_FIELDTYPE_STD_MAC */
#ifdef EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC
  if (sub_type == ewFieldTypeFDDIMAC)
    {
      mp = (uint8 *)(*(EwsFormFieldMAC **)cpp
                       + (*(uintf *) ((uint8 *) context->form
                       + multi_fieldP->choices_offset)));
      field.value_offset = ((uint8 *) (mp)) - ((uint8 *) context->form);
    }
  else
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC */
    {
      cpp = *(char ***)cpp + (*(uintf *) ((uint8 *) context->form
                           + multi_fieldP->choices_offset));
      field.value_offset = ((uint8 *) (cpp)) - ((uint8 *) context->form);
    }
  field.status_offset =  fieldp->status_offset;

  /*
   * we alredy checked, that .from_f exists:
   *   if (ews_state->form_field_table[sub_type].from_f == NULL) parse error;
   */
  sptmp = *sp;
  *sp = 0;
  returnValue = (*ews_state->form_field_table[sub_type].from_f)
                  (context, &field, stringp);
  *sp |= sptmp;
  if ( !(*sp & EW_FORM_PARSE_ERROR) )
    {
      (*((uintf *) ((uint8 *) context->form + multi_fieldP->choices_offset))) =
        (*((uintf *) ((uint8 *) context->form + multi_fieldP->choices_offset))) + 1;
      return EWS_STATUS_OK;
    }
  else
    {
      return ( returnValue == EWS_STATUS_OK
                 ? EWS_STATUS_BAD_SYNTAX 
                 : returnValue 
             );
    }
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

#if defined (EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING)\
 || defined (EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC)\
 || defined (EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC)\
 || defined (EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
/*
 * ewsFormReadHex
 * Read hexadecimal characters from network buffers into raw data buffer.
 *
 * dst          - raw data buffer to receive translated data
 * src          - estring representing hexadecimal digits
 * length       - size of raw data buffer
 * sepp         - output: Overwritten by the separator char that
 *                that is seen, if any are present.  NUL if no sep chars seen.
 *                All separator chars present must be the same.
 * sep_pad      - TRUE if presence of a separator char forces output to the
 *                next byte boundary.  FALSE if they have no such effect.
 *                FALSE also allows adjacent separators.
 *
 * Returns number of raw data bytes processed, or -1 on parse error.
 */
int32
ewsFormReadHex
  ( uint8      *dst
  , EwsStringP  src
  , uint32      length
  , char       *sepp
  , boolean     sep_pad
  )
{
  EwsString e_begin  = *src;
  EwsString e_end    = *src;
  uint32    e_length = ewsStringLength(src);
  uintf     nybbles = 0;
  uintf     match_c;
  uintf     hex;
  sintf     length_out;
  boolean   odd = FALSE;

    *sepp = '\0';
    length_out = 0;
    while (e_length > 0 && length > 0)
      {
        /*
         * Search remaining string for non-HEX or EOF
         */
        ewsStringLengthSet(&e_begin, 0);
        match_c = ewsStringSearch( &e_begin
                                  ,&e_end
                                  ,"0123456789abcdefABCDEF"
                                  ,ewsStringSearchExclude
                                  ,&e_length );

        /*
         * If separator found, enforce that separators must be the same
         */
        if ( match_c != EWS_STRING_EOF )
          {
            if (*sepp != '\0')
              {
                if (*sepp != (char) match_c)
                  {
                    return -1;
                  }
              }
            else
              {
                *sepp = (char) match_c;
              }
          }

        /*
         * We now know how many HEX nybbles there are until the next
         * separator character or end-of-string.  Start writing them
         * out...
         */
        nybbles = ewsStringLength(&e_begin);
        if (sep_pad)
          {
            odd = ((nybbles & 1) == 1);
            *dst = 0;
          }
        while (nybbles > 0 && length > 0)
          {
            hex = ewsStringGetChar(&e_begin);
            nybbles--;
            if (hex >= '0' && hex <= '9')
              {
                hex = hex - '0';
              }
            else if (hex >= 'a' && hex <= 'f')
              {
                hex = (hex - 'a') + 10;
              }
            else
              {
                hex = (hex - 'A') + 10;
              }
            *dst = (*dst << 4) | hex;
            if (odd)
              {
                dst++;
                length_out++;
                length--;
              }
            odd = !odd;
          }

        /*
         * Skip separator and continue
         */
        if (match_c != EWS_STRING_EOF)
          {
            ewsStringGetChar(&e_end);
            e_length--;
          }
        e_begin = e_end;
      }

    /*
     * Make sure we could read all the data into the buffer
     */
    if (e_length > 0 || nybbles > 0)
      {
        return -1;
      }

    return length_out;
}
#endif /* HEX_INT|HEX_STRING|IEEE_MAC|FDDI_MAC|STD_MAC */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
/*
 * ewFieldFromFile
 *
 * Open a file using information gathered from a multipart/form-data
 * POST.   This "conversion" routine is called when all the headers
 * for the file field have been read in and stored in the
 * context->inputFile->formDataHeaders structure, but BEFORE the file data
 * itself is reached (we have to open the file before we write data to
 * it!).
 *
 * NOTE: If the file is opened successfully, we set the EW_FORM_DYNAMIC bit
 * in the status field.  This bit is then _cleared_ _prior_ to calling the
 * submit function - it is the application's job to close the file in the
 * submit function.  (see ewsFormSubmitAction, this file).
 *
 * context      - context of request
 * fieldp       - ewFieldTypeFile field
 * stringp      - NULL
 *
 * Returns EWS_STATUS_OK on success, else error code.
 */
EwsStatus
ewFieldFromFile
( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  EwsFormInputFileState *file = context->inputFile;
  EwsString filename;
  uint32 len;
  EwsFileParams params;
  EwaFileHandle *hptr;
  uintf i;
  char *mptr = NULL;
  char *cptr;
  EW_UNUSED(stringp);
  /*
   * first, try to find the optional filename=xxx attribute
   */

  ewsStringLengthSet( &filename, 0 );
  ewsParseAttributeFromHeader(
                       &file->formDataHeaders[ewsFormDataHeaderContentDisposition]
                       ,(char *)"filename"
                       ,&filename
                       ,ewsParseValue
                       ,';'
                       ,ewsHdrUpdateNo );

  /* set up params */

  EMWEB_MEMSET( &params,
                0,
                sizeof( params ));
  params.fileField.contentLength = -1;

  /*
   * allocate memory for each string, and copy in each parameter
   * into param block.  If we fail to allocate memory, warn, but
   * continue the file transfer.  At worst, the application won't
   * validate the file correctly. A good app design would take into
   * account a mangled file....
   */

  len = ewsStringLength( &filename );

  for (i = ewsFormDataHeaderUnknown + 1;
       i < ewsFormDataHeaderCount;
       i++)
    {
      if (i == ewsFormDataHeaderContentLength)  /* skip the integer */
        continue;

      len += ewsStringLength( &file->formDataHeaders[i] );
    }

  if (len != 0)                     /* well, we found some headers */
    {
      /* allocate additional bytes for string terminators */

      mptr = (char *) ewaAlloc( len + ewsFormDataHeaderCount );

      if (mptr != NULL)
        {
          cptr = mptr;

          /* special case the file name */

          if ((len = ewsStringLength( &filename )) != 0)
            {
              ewsStringCopyOut( cptr, &filename, len );
              params.fileField.fileName = cptr;
              cptr += len + 1;
            }

          for (i = ewsFormDataHeaderUnknown + 1;
               i < ewsFormDataHeaderCount;
               i++)
            {
              if ((len = ewsStringLength( &file->formDataHeaders[i] )) != 0)
                {
                  switch (i)
                    {
                    case ewsFormDataHeaderContentLength:
                      ewsStringGetInteger( &file->formDataHeaders[i]
                                          ,10
                                          ,(uint32 *)&params.fileField.contentLength );
                      continue;

                    case ewsFormDataHeaderContentType:
                      params.fileField.contentType = cptr;
                      break;

                    case ewsFormDataHeaderContentDisposition:
                      params.fileField.contentDisposition = cptr;
                      break;

                    case ewsFormDataHeaderTransferEncoding:
                      params.fileField.contentEncoding = cptr;
                      break;

#                   ifdef EMWEB_SANITY
                    default:
                      EMWEB_ERROR(("ewFieldFromFile: unknown header type.\n"));
                      continue;
#                   endif /* EMWEB_SANITY */
                    }

                  ewsStringCopyOut( cptr, &file->formDataHeaders[i], len );
                  cptr += len + 1;
                }
            }
        }
    }

  /* now call the post routine to grab a file handle
   * from the application and set it in the form structure.
   */

  hptr = (EwaFileHandle *) ((uint8 *)context->form + fieldp->value_offset);
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
  context->inputFile->fileStatusp = ((uint8 *) context->form_status + fieldp->status_offset);
#else
  context->inputFile->fileStatusp = ((uint8 *) context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
  if (*context->inputFile->fileStatusp & EW_FORM_RETURNED)
    {
      *context->inputFile->fileStatusp |= EW_FORM_FILE_ERROR;
      if (mptr != NULL)
        ewaFree( mptr );
      return EWS_STATUS_ALREADY_EXISTS;
    }

  *hptr = context->inputFile->formFile = 
    ewaFilePost( context
                , &params
#               ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
                , &(context->inputFile->fileSystem)
#               endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                );
# ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
  if (context->schedulingState == ewsContextSuspended)
    {
      if (mptr != NULL)
        ewaFree( mptr );
      return EWS_STATUS_OK;
    }
# endif /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
  if (EWA_FILE_HANDLE_NULL != *hptr)
    {
#     ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
      EwsInputFileSystemEntry newEntry;

      if (context->inputFile->fileSystem != EWS_FILE_SYSTEM_HANDLE_NULL)
        {
          newEntry = (EwsInputFileSystemEntry)ewaAlloc(sizeof(EwsInputFileSystemEntry_t));
          if (newEntry != NULL)
            {
              newEntry->fileHandle = context->inputFile->formFile;
              newEntry->fileSystem = context->inputFile->fileSystem;
              EWS_LINK_INSERT(&(context->inputFile_system_list), &(newEntry->link));
              (context->inputFile->fileSystem->use_cnt)++;
            }
          else
            {
              EMWEB_WARN(("ewFieldFromFile: can't allocate memory\n"));
            }
         }
       else
        {
          EMWEB_WARN(("ewFieldFromFile: ewaFilePost returned EWS_FILE_SYSTEM_HANDLE_NULL\n"));
        }
#     endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
      *context->inputFile->fileStatusp = EW_FORM_RETURNED | EW_FORM_DYNAMIC;
    }
  else      /* unable to set file handle */
    {
      *context->inputFile->fileStatusp = EW_FORM_FILE_ERROR;
    }

  /* free any allocated resources */

  if (mptr != NULL)
    ewaFree( mptr );
  return EWS_STATUS_OK;
}


/*
 * ewFieldFreeFile
 *
 * This function frees up the opened file by closing it (see above function)
 * when the request has been aborted prior to form submit processing.
 * Under normal operation, the submit function will close the file.  However,
 * if the context is aborted prior to the submission, then this function is
 * called during cleanup.
 *
 * p    - pointer to the EwaFileHandle value in the form structure.
 *
 *
 * No return value
 */
void
ewFieldFreeFile ( void * p )
{
# ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
  EwsContext context = ews_state->context_current;

  if (context != NULL)
    {
      fs_fileClose(context, *(EwaFileHandle *)p, TRUE);
    }
# else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
  ewaFileClose( *(EwaFileHandle *)p, EWS_STATUS_ABORTED );
# endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

#ifdef EW_CONFIG_OPTION_FORM_REPEAT

/*
 * Scan field table for ewFieldTypeRepeat, call repeat count
 * code fragment, allocate memory and link structures.
 */

boolean
ewBuildRepeatForm ( EwsContext context, EwsDocumentContextP doc_context
                   ,const EwFormEntry *entryp )
{
  uintf element;
  const EwFormField *fieldp;
  const EwFormRepeatField *rpt_fieldp;
  uint8 *memblock;
  uint8 **vp;
  uint8 **sp;
  uintf value_str_size = 0, status_str_size = 0;
  uintf *rpt_cntp;
  boolean fault_flg = FALSE;

  for (element = 0, fieldp = entryp->field_list;
       element < entryp->field_count;
       element++, fieldp++)
    {
      if (fieldp->field_type == ewFieldTypeRepeat)
        {
          rpt_fieldp = (EwFormRepeatField *)(fieldp->default_value);
          /*
           * call code fragment to get repeat count
           */
          rpt_cntp = (uintf *)((uint8 *) (context->form) + rpt_fieldp->repeat_offset);
          if (fault_flg == TRUE)
            {
              *rpt_cntp = 0;
            }
          else
            {
              *rpt_cntp = *((uintf *)((*doc_context->document->archive->emweb_string)
                            (context, rpt_fieldp->index)));
            }
          vp = (uint8 **)((uint8 *) (context->form) + fieldp->value_offset);
          sp = (uint8 **)((uint8 *) (context->form) + fieldp->status_offset);
          /*
           * if supplied repeat count exceeds max values, it is  user's
           * programming error.
           */
          if (rpt_fieldp->max > 0 && *rpt_cntp > rpt_fieldp->max)
            {
              *rpt_cntp = rpt_fieldp->max;
              EMWEB_WARN(("ewBuildRepeatForm: repeates exceeds maximum\n"));
            }
          else if (rpt_fieldp->max == 0 && *rpt_cntp > EWS_FORM_REPEAT_MAX_REPEATS)
            {
              *rpt_cntp = EWS_FORM_REPEAT_MAX_REPEATS;
              EMWEB_WARN(("ewBuildRepeatForm: repeates exceeds maximum\n"));
            }

          if (*rpt_cntp == 0)
            {
              *vp = NULL;
              *sp = NULL;
            }
          else
            {
              /*
               * allocate memory.
               */
              value_str_size = rpt_fieldp->value_substruct_size;
              status_str_size = rpt_fieldp->status_substruct_size;
              if (value_str_size != 0)
                {
                  if ((memblock = (uint8 *)ewaAlloc(
                                           *rpt_cntp * value_str_size +
                                           *rpt_cntp * status_str_size))
                      == NULL)
                    {
                      return(FALSE);
                    }
                  /*
                   * we have memory, set up pointers
                   */
                  *vp = memblock;
                  *sp = memblock + (*rpt_cntp * value_str_size);
                }
            }
        }
    }
  return(!fault_flg);
}
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

#if defined(EW_CONFIG_OPTION_SOAP)

/*
 * ewuFieldFromBoolean
 * Read a Boolean value from a text field into the form structure
 *
 * context      - context of request
 * fieldp       - boolean field structure
 * stringp      - value of text field
 *
 * No return value
 */
EwsStatus
ewuFieldFromBoolean
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  boolean *up;
  uint8 *sp;

# ifdef EW_CONFIG_OPTION_FORM_REPEAT
  sp = ((uint8 *) context->form_status) + fieldp->status_offset;
# else
  sp = ((uint8 *) context->form) + fieldp->status_offset;
# endif /* EW_CONFIG_OPTION_FORM_REPEAT */
  up = (boolean *) ((uint8 *) context->form + fieldp->value_offset);

  /* Compare the supplied value with the various valid values. */
  if (ewsStringCompare(stringp, "0",
                       ewsStringCompareCaseInsensitive, NULL) ||
      ewsStringCompare(stringp, "f",
                       ewsStringCompareCaseInsensitive, NULL) ||
      ewsStringCompare(stringp, "false",
                       ewsStringCompareCaseInsensitive, NULL) ||
      ewsStringCompare(stringp, "n",
                       ewsStringCompareCaseInsensitive, NULL) ||
      ewsStringCompare(stringp, "no",
                       ewsStringCompareCaseInsensitive, NULL))
    {
      *sp |= EW_FORM_RETURNED;
      *up = 0;
    }
  else if (ewsStringCompare(stringp, "1",
                            ewsStringCompareCaseInsensitive, NULL) ||
           ewsStringCompare(stringp, "t",
                            ewsStringCompareCaseInsensitive, NULL) ||
           ewsStringCompare(stringp, "true",
                            ewsStringCompareCaseInsensitive, NULL) ||
           ewsStringCompare(stringp, "y",
                            ewsStringCompareCaseInsensitive, NULL) ||
           ewsStringCompare(stringp, "yes",
                            ewsStringCompareCaseInsensitive, NULL))
    {
      *sp |= EW_FORM_RETURNED;
      *up = 1;
    }
  else
    {
      /* All other values are invalid. */
      *sp |= EW_FORM_PARSE_ERROR;
      return EWS_STATUS_BAD_SYNTAX;
    }
  return EWS_STATUS_OK;
}

/*
 * ewuFieldFromUUID
 * Read a UUID value from a text field into the form structure
 *
 * context      - context of request
 * fieldp       - unsigned hex string text field
 * stringp      - value of text field
 *
 * No return value
 */
EwsStatus
ewuFieldFromUUID
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint8 *sp;
  EwsFormFieldHexString *uuidp;
  char sep;

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
  sp = ((uint8 *) context->form_status + fieldp->status_offset);
#else
  sp = ((uint8 *) context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
  uuidp = (EwsFormFieldHexString *) ((uint8 *)context->form +
                                     fieldp->value_offset);

  if (ewsFormReadHex((uint8 *) uuidp, stringp, 16, &sep, FALSE) != 16 ||
      !(sep == '\0' || sep == '-'))
    {
      *sp |= EW_FORM_PARSE_ERROR;
      return EWS_STATUS_BAD_SYNTAX;
    }

  *sp |= EW_FORM_RETURNED;
  return EWS_STATUS_OK;
}

/*
 * ewUFieldFromBase64
 * Read a bin base64 string value from a text field into the form structure
 *
 * context      - context of request
 * fieldp       - dotted ip text field
 * stringp      - value of text field
 *
 * No return value
 */
EwsStatus
ewuFieldFromBase64
  ( EwsContext context, const EwFormField *fieldp, EwsStringP stringp )
{
  uint8 *sp;
  EwsFormFieldHexString *hsp;
  uint8 *dp;
  uint32 length;
  int chars_in;                 /* Number of characters in this group of 4
                                 * that have already been processed.
                                 * value = 0, 1, 2, or 3 */
  uint16 partial_value;
  uintf c = 0;                  /* the next input character */
  uintf v;
  uintf slen;                   /* Number of characters remaining to be
                                 * processed in the input string, including
                                 * the character now in 'c', so it is usually
                                 * one greater than stringp->length. */

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
    sp = ((uint8 *) context->form_status + fieldp->status_offset);
#else
    sp = ((uint8 *) context->form + fieldp->status_offset);
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
    if (*sp & EW_FORM_RETURNED)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_ALREADY_EXISTS;
      }
    hsp = (EwsFormFieldHexStringP) ((uint8 *)context->form +
            fieldp->value_offset);
    length = ewsStringLength(stringp);
    /* Calculate the input string length in quartets and allow 3 output bytes
     * for each quartet.  But be careful to round up, since length of a bad
     * string might not be a multiple of 4!
     */
    length = ((length + 3) / 4) * 3;
    hsp->datap = dp = (uint8 *) ewaAlloc(length + 1);
    if (dp == NULL)
      {
        *sp |= EW_FORM_PARSE_ERROR;
        return EWS_STATUS_NO_RESOURCES;
      }

  chars_in = 0;
  partial_value = 0;

  for (slen = ewsStringLength(stringp); 0 < slen; slen--)
    {
      c = ewsStringGetChar(stringp);
      /* slen now matches c. */

      if (' ' == c || '\t' == c || '\n' == c || '\r' == c)
        {
          /* A valid pad. */
          continue;
        }
      if ('=' == c)
        {
          /* '=' signals the end of the string. */
          break;
        }
      if ('A' <= c && 'Z' >= c)
        {
          v = c - 'A';
        }
      else if ('a' <= c && 'z' >= c)
        {
          v = c - 'a' + 26;
        }
      else if ('0' <= c && '9' >= c)
        {
          v = c - '0' + 52;
        }
      else if ('+' == c)
        {
          v = 62;
        }
      else if ('/' == c)
        {
          v = 63;
        }
      else
        {
          *sp |= EW_FORM_PARSE_ERROR;
          ewaFree((void*)hsp->datap);
          return EWS_STATUS_BAD_SYNTAX;
        }
      /* Incorporate the new bits into the current partial value
       * and write any complete bytes out. */
      partial_value = (partial_value << 6) | v;
      switch (chars_in)
        {
        case 0:
          chars_in = 1;
          break;
        case 1:
          /* 12 bits are in partial_value */
          *dp++ = (partial_value >> 4) & 0xFF;
          partial_value &= 0x0F;
          chars_in = 2;
          break;
        case 2:
          /* 10 bits are in partial_value */
          *dp++ = (partial_value >> 2) & 0xFF;
          partial_value &= 0x03;
          chars_in = 3;
          break;
        case 3:
          /* 8 bits are in partial_value */
          *dp++ = partial_value & 0xFF;
          partial_value = 0;
          chars_in = 0;
          break;
        }
    }

  /* Check that the end of the data characters are done correctly. */
  if (1 == chars_in || 0 != partial_value)
    {
      *sp |= EW_FORM_PARSE_ERROR;
      ewaFree((void*)hsp->datap);
      return EWS_STATUS_BAD_SYNTAX;
    }

  /* Process any '='s that follow the data characters. */
  for (; 0 < slen;
       c = ewsStringGetChar(stringp), slen-- /* Adjust slen to match c. */
       )
    {
      if (' ' == c || '\t' == c || '\n' == c || '\r' == c)
        {
          /* A valid pad. */
        }
      else if ('=' == c && 0 < chars_in)
        {
          /* An '=' is only valid if there's an unfilled quad. */
          chars_in = (chars_in + 1) % 4;
        }
      else
        {
          *sp |= EW_FORM_PARSE_ERROR;
          ewaFree((void*)hsp->datap);
          return EWS_STATUS_BAD_SYNTAX;
        }
    }

  /* The last quad should be filled out. */
  if (0 != chars_in)
    {
          *sp |= EW_FORM_PARSE_ERROR;
          ewaFree((void*)hsp->datap);
          return EWS_STATUS_BAD_SYNTAX;
    }

  /* Now that we know the length of the data, save it. */
  hsp->length = dp - hsp->datap;
  *sp |= EW_FORM_RETURNED | EW_FORM_DYNAMIC;
  return EWS_STATUS_OK;
}

/*
 * ewuFieldFreeBase64
 * Read a bin base64 string value from a text field into the form structure
 *
 * p      - pointer to the pointer to string to be freed
 *
 * No return value
 */
void
ewuFieldFreeBase64 ( void * p )
{
  char **cpp = (char **) p;

  ewaFree(*cpp);
}

#endif /* defined(EW_CONFIG_OPTION_SOAP) */

#endif /* EW_CONFIG_OPTION_FORM */

#if defined (EW_CONFIG_SCHEMA_ARCHIVE) || defined (EW_CONFIG_OPTION_FORM)
int
ewsField2Rendering ( EwFieldType field_type )
{
  switch (field_type)
    {
      case ewFieldTypeHexString: return ewsSchemaRenderHexString;
      case ewFieldTypeText: return ewsSchemaRenderText;
      case ewFieldTypeDecimalUint: return ewsSchemaRenderDecimalUint;
      case ewFieldTypeDecimalInt: return ewsSchemaRenderDecimalInt;
      case ewFieldTypeHexInt: return ewsSchemaRenderHexInt;
      case ewFieldTypeDottedIP: return ewsSchemaRenderDottedIP;
      case ewFieldTypeIEEEMAC: return ewsSchemaRenderIEEEMAC;
      case ewFieldTypeDecnetIV: return ewsSchemaRenderDecnetIV;
      case ewFieldTypeObjectID: return ewsSchemaRenderObjectID;
      case ewFieldTypeBoolean: return ewsSchemaRenderBoolean;
      case ewFieldTypeBase64: return ewsSchemaRenderBase64;
      case ewFieldTypeUUID: return ewsSchemaRenderUUID;
      default:
        EMWEB_WARN(("ewsField2Rendering: bad type %d\n", field_type));
        return -1;
    }
}

int
ewsRendering2Field ( EwsSchemaRenderingCode rendering_code )
{
  switch (rendering_code)
    {
      case ewsSchemaRenderHexString: return ewFieldTypeHexString;
      case ewsSchemaRenderText: return ewFieldTypeText;
      case ewsSchemaRenderDecimalUint: return ewFieldTypeDecimalUint;
      case ewsSchemaRenderDecimalInt: return ewFieldTypeDecimalInt;
      case ewsSchemaRenderHexInt: return ewFieldTypeHexInt;
      case ewsSchemaRenderDottedIP: return ewFieldTypeDottedIP;
      case ewsSchemaRenderIEEEMAC: return ewFieldTypeIEEEMAC;
      case ewsSchemaRenderDecnetIV: return ewFieldTypeDecnetIV;
      case ewsSchemaRenderObjectID: return ewFieldTypeObjectID;
      case ewsSchemaRenderBoolean: return ewFieldTypeBoolean;
      case ewsSchemaRenderBase64: return ewFieldTypeBase64;
      case ewsSchemaRenderUUID: return ewFieldTypeUUID;
      default:
        EMWEB_WARN(("ewsRendering2Field: bad code %d\n", rendering_code));
        return -1;
    }
}
#endif /* (EW_CONFIG_SCHEMA_ARCHIVE) || (EW_CONFIG_OPTION_FORM) */

