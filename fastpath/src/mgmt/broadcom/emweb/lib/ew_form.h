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
 * EmWeb Form definitions between server and generated code.  These are
 * also used in typed EMWEB_STRINGs, etc.
 *
 */
#ifndef _EW_FORM_H
#define _EW_FORM_H

#include "ew_types.h"
#include "ews_str.h"
#include "ew_common.h"
#include "ews_def.h"

/*
 * Application Form Serve Function
 * Fills in form template with default values.  (We comment out the prototype
 * to avoid compiler warnings -- the actual application functions define
 * a specific form structure instead of void*).
 *
 * context - request context
 * form    - pointer to form-specific structure
 *
 * No return value
 */
typedef void EwaFormServe_f ( EwsContext context, void * form );

/*
 * Application Form Submit Function
 * Processes submitted form.  (We comment out the prototype
 * to avoid compiler warnings -- the actual application functions define
 * a specific form structure instead of void*).
 *
 * context - request context
 * form    - pointer to form-specific structure
 *
 * Returns NULL for default response to server (accepted), or redirection URL.
 */
typedef EW_NSC char * EwaFormSubmit_f ( EwsContext   context
                                      , void       * form );

/*
 * Application Namespace Functions
 *
 *   Get        - get value of object
 *   SetStart   - prepare to set a group of zero or more objects
 *   Set        - prepare set a specific object within the group
 *   SetFinish  - commit group of set objects
 */
typedef EW_NSC char * EwaNamespaceGet_f ( EwsContext  context
                                         ,const char *object_id
                                         ,const char *object_params
                                        );
typedef void * EwaNamespaceSetStart_f ( EwsContext context );
typedef void   EwaNamespaceSet_f ( EwsContext context
                                  ,void      *handle
                                  ,const char *object_id
                                  ,const char *object_params
                                  ,const char *value
                                 );
typedef EW_NSC char *EwaNamespaceSetFinish_f ( EwsContext context
                                              ,void      *handle
                                              ,EwsStatus  status
                                             );

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
/*
 * Application Dynamic Select Option generation function
 * Provides an OPTION for dynamic select
 *
 * context - request context
 * option  - pointer to option-specific structure
 * iterator - pointer to application's structure
 *
 * Returns NULL for no more options, or iterator structure.
 */
typedef void *EwaFormSelectOption_f ( EwsContext context
                                    , EwsFormSelectOptionP option
                                    , void * iterator );
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */


/*
 * Form Field Types
 * Each field element has a type associated with it.  Each type has conversion
 * routines to handle conversions between HTML and internal representations.
 * NOTE: if you modify this enum list, you MUST update 'fieldMap' in
 * file ewc_code.c!!
 * NOTE: new fields must be added to the end of this list for backward
 * compatibility!!
 */
typedef enum EwFieldType_e
  {
    ewFieldTypeDecimalOverride = 0 /* In EMWEB_STRING: No conversion - output
                                    * text directly into HTML stream.  (Thus,
                                    * text can contain markup.) Specified by
                                    * absence of EMWEB_TYPE attribute.
                                    * In forms: unsigned decimal integer, used
                                    * for SIZE and other decimal-valued
                                    * attributes that are serve-only. */
   ,ewFieldTypeRadio            /* radio button - serve setup and submit */
   ,ewFieldTypeRadioField       /* radio fields - used in serving form */
   ,ewFieldTypeSelect           /* select enum - serve setup and submit */
   ,ewFieldTypeSelectField      /* select enum fields - used in serving form */
   ,ewFieldTypeSelectMulti      /* multiple select */
   ,ewFieldTypeCheckbox         /* checkbox */
   ,ewFieldTypeText             /* In forms: used for VALUE of INPUT with
                                 * EMWEB_TYPE=TEXT or missing.
                                 * Value is &-quoted before put into the HTML
                                 * stream so string displays as given on the
                                 * browser.
                                 * In EMWEB_STRING: &-quote string before put
                                 * into the HTML stream, so it is character
                                 * data.  Specified by EMWEB_TYPE=TYPE. */
   ,ewFieldTypeImage            /* image field */
   ,ewFieldTypeDecimalUint      /* unsigned integer */
   ,ewFieldTypeDecimalInt       /* signed integer */
   ,ewFieldTypeHexInt           /* hexadecimal integer */
   ,ewFieldTypeHexString        /* hexadecimal string */
   ,ewFieldTypeDottedIP         /* dotted IP address */
   ,ewFieldTypeIEEEMAC          /* IEEE MAC */
   ,ewFieldTypeFDDIMAC          /* FDDI MAC */
   ,ewFieldTypeStdMAC           /* Standard MAC */
   ,ewFieldTypeDecnetIV         /* Decnet IV Address */
   ,ewFieldTypeFile             /* input type file handle */
   ,ewFieldTypeTextOverride     /* In forms: used for VALUE of INPUT TYPE=FILE
                                 * on serve only.
                                 * In EMWEB_STRING: Not used. */
   ,ewFieldTypeObjectID         /* object identifier */
   ,ewFieldTypeDynamicSingleSelect  /* dynamic single SELECT */
   ,ewFieldTypeDynamicMultiSelect   /* dynamic multi SELECT */
   ,ewFieldTypeRepeat           /* repeat body */
   ,ewFieldTypeRepeatEnd        /* end repeat body */
   ,ewFieldTypeFormRadio        /* repeated radio button within FORM scope
                                   - serve setup and submit */
   ,ewFieldTypeFormRadioField   /* repeated radio fields within FORM scope
                                   - used in serving form */
   ,ewFieldTypeBoolean          /* boolean */
   ,ewFieldTypeBase64           /* Base 64 */
   ,ewFieldTypeUUID             /* UUID */
   ,ewFieldTypeDynamic          /* indirect conversion type */
   ,ewFieldTypeMAX              /* MUST BE LAST */
  } EwFieldType;

/*
 * Form Field Representation
 * The compiler generates the following structure for each element in the
 * form.  In general, a FORM document node is placed in the archive at the
 * offset immediately following a VALUE= in TEXT, TEXTAREA, HIDDEN, or PASSWORD
 * field.  The field component of the form node index refers to a field
 * structure as defined below.  The field type would be ewFieldTypeText, or
 * one of the derivative extended types such as ewFieldToDottedIP.  If a
 * VALUE was specified in the source HTML, the compiler translates it into
 * the appropriate internal representation and creates the default_value of
 * the field.  (For ewFieldTypeText, this is simply a pointer to the null
 * terminated string.  For other fields, this is a pointer to a value whose
 * length is determined by the type in ewFieldTable).  The NAME= part of
 * the field is saved, and the offset into the compiler-generated form for
 * the value and status portions of the field is stored in value_offset and
 * status_offset.
 *
 * Checkboxes and multiple selections are fairly straight forward.
 * The type ewFieldCheckbox or ewFieldSelectMulti is used, and the
 * node appears where the word "CHECKED" or "SELECTED" may appear.  If a
 * VALUE= is present, it is saved in the sel_value field.  Otherwise, these
 * types are identical to the extended text types above.
 *
 * Radio buttons and single-select boxes are more complex.  With a single
 * NAME=, there may be multiple VALUE= where only one applies to the actual
 * value of the form field.  With N radio buttons or single select options,
 * the compiler generates N+1 field structures.  One structure is used to
 * read and write the value in the internal generated form.  This would have
 * type ewFieldTypeRadio or ewFieldTypeSelect, contain the default value,
 * offsets into the form structure, and the field name.  In addition,an array of
 * integers containing all values corresponding to the generated enumerator is
 * saved in enum_list.  The other N field structures would represent each radio
 * button or select option and have the type ewFieldTypeRadioField or
 * ewFieldTypeSelectField.  These are referenced by form nodes in the document
 * archive at the location corresponding to where a "CHECKED" or "SELECTED"
 * keyword would be inserted.  They would not have a name field.  Instead,
 * the enum_value field would be set to the VALUE= enumeration corresponding
 * to the specific field.
 *
 * NOTE:
 * despite its name, default_value field, may hold a pointer to the second
 * level of field descriptors:
 *  - for repeated form
 *  - for dynamic SELECTs.
 *   In this case this field contains a pointer to
 *   EwFormFieldDynamic[Single|Multi]Select structure
 */
typedef struct EwFormField_s
  {
    EwFieldType         field_type;     /* type of field */
    const void          *default_value; /* pointer to default value */
    uintf               value_offset;   /* offset to value in form struct */
    uintf               status_offset;  /* offset to status in form struct */
    const char          *name;          /* name of field from NAME="" */
    const int           *enum_list;     /* list of field values by enum */
    int                 enum_value;     /* SELECT/RADIO value of field */
    const char          *sel_value;     /* SELECT multiple value field */
  } EwFormField, * EwFormFieldP;

/*
 * Dynamic SELECTs
 *
 * Additional structures are required to hold all the informations
 * needed with processing of dynamic SELECTs. We need space to hold
 * iterator function to be called to supply SELECT's OPTIONS
 * and for multi SELECTS we need space to hold offset to the
 * ...._choices field in the form structure and space to save maximum
 * number of options alowed to be submitted (specified by EMWEB_MAX attribute
 */


#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
typedef struct EwFormFieldDynamicSingleSelect_s
  {
    EwFieldType           field_type;   /* type of field suplied by EMWEB_TYPE */
    EwaFormSelectOption_f *iteration_f; /* iterator function */
  } EwFormFieldDynamicSingleSelect, *EwFormFieldDynamicSingleSelecP;

typedef struct EwFormFieldDynamicMultiSelect_s
  {
    EwFieldType         field_type;     /* type of field suplied by EMWEB_TYPE */
    EwaFormSelectOption_f *iteration_f; /* iterator function */
    uintf               choices_offset; /* offset to ..._choices in form struct*/
    uintf               max_options;    /* max allowable number of submitted
                                           options = EMWEB_MAX */
  } EwFormFieldDynamicMultiSelect, *EwFormFieldDynamicMultiSelecP;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

/*
 * <EMWEB_REPEAT>
 *
 * Additional structure is required to hold information describing repeatable
 * body. Additional information contains an index to the emweb_string switch
 * (C code to be executed by server at serve time to obtain number of body
 * repetitions), offset to the repeat storage in the form structure,
 * pointer to the subtable of fields constituting the body, subtable size,
 * and sizes of value and status substructures in the form structure.
 */

#ifdef EW_CONFIG_OPTION_FORM_REPEAT
typedef struct EwFormRepeatField_s
  {
    uint32        index;        /* this index directs server to the 'C'
                                   code fragment  */
    uintf        repeat_offset;
    uint16       subtable_field_cnt;
    uint16       value_substruct_size;
    uint16       status_substruct_size;
    uint16       max;           /* max number of repeatitions allowed */
  } EwFormRepeatField;

typedef struct EwFormRepeatRadioFormField_s
  {
    uintf  instance_offset;     /* offset to radio instance field in form */
    void *default_value;        /* for most fields this is in EwFormField */
  } EwFormRepeatRadioFormField;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */

/*
 * Form Representation
 * The following structure represents an HTML FORM.  This points to the
 * application-provided serve and submit functions, indicates the size
 * of the compiler-generated form structure, and points to a table of
 * field structures as defined above.
 */
typedef struct EwFormEntry_s
  {
    EwaFormServe_f      *serve_f;       /* application serve function */
    EwaFormSubmit_f     *submit_f;      /* application submit function */
    uintf               form_size;      /* sizeof form structure */
    uintf               field_count;    /* number of fields in form */
    const EwFormField   *field_list;     /* list of fields */
  } EwFormEntry, * EwFormEntryP;

/*
 * Document Node Conversion Macros
 * These macros convert between the 32-bit node index and a form and element
 * index.
 */
#define EW_FORM_ELEMENT_START                   (0)
#define EW_FORM_ELEMENT_END                     (0xffff)
#define EW_FORM_NODE_INDEX_TO_FORM_INDEX(i)     ((i) >> 16)
#define EW_FORM_NODE_INDEX_TO_ELEMENT_INDEX(i)  ((i) & 0xffff)
#define EW_FORM_ELEMENT_TO_NODE_INDEX(f, e)     (((uint32)(f) << 16) | (e))

/*
 * If form index is 0xffff, then this is a namespace form.  Use the namespace
 * index to select from the namespace table.
 */
#define EW_FORM_INDEX_NAMESPACE                 (0xffff)
#define EW_FORM_NODE_INDEX_TO_NS_INDEX(i)       ((i) & 0xff)

/*
 * Form conversion functions.
 * The "To" function converts to HTML ASCII format from the internal
 * representation and writes it directly to the output data stream.
 * The "From" function converts from HTML ASCII contained in a parsed network
 * buffer EwsString and converts it to the internal representation.
 */
typedef void EwFieldTypeTo_f
  ( EwsContext context, const EwFormField *field );
typedef EwsStatus EwFieldTypeFrom_f
  ( EwsContext context, const EwFormField *field, EwsStringP stringp );
typedef void EwFieldTypeFree_f ( void *p );

#ifdef EW_CONFIG_OPTION_FIELDTYPE_RADIO
extern EwFieldTypeTo_f          ewFieldToRadioField;
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
extern EwFieldTypeTo_f          ewFieldToFormRadioField;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
#endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_RADIO)\
 || defined(EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE)
extern EwFieldTypeFrom_f        ewFieldFromRadio;
#ifdef EW_CONFIG_OPTION_FORM_REPEAT
extern EwFieldTypeFrom_f        ewFieldFromFormRadio;
#endif /* EW_CONFIG_OPTION_FORM_REPEAT */
#endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO | SELECT_SINGLE */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE
extern EwFieldTypeTo_f          ewFieldToSelectField;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_MULTIPLE
extern EwFieldTypeTo_f          ewFieldToSelectMulti;
extern EwFieldTypeFrom_f        ewFieldFromSelectMulti;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_MULTIPLE */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_CHECKBOX
extern EwFieldTypeTo_f          ewFieldToCheckbox;
extern EwFieldTypeFrom_f        ewFieldFromCheckbox;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_CHECKBOX */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_TEXT) || \
    defined(EW_CONFIG_OPTION_FIELDTYPE_FILE)
extern EwFieldTypeTo_f          ewFieldToText;
extern EwFieldTypeFrom_f        ewFieldFromText;
extern EwFieldTypeFree_f        ewFieldFreeText;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_TEXT */

extern EwFieldTypeTo_f          ewFieldToDecimalUint;

#ifdef EW_CONFIG_OPTION_FIELDTYPE_IMAGE
extern EwFieldTypeFrom_f        ewFieldFromImage;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_IMAGE */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_UINT)\
 || defined(EW_CONFIG_OPTION_FIELDTYPE_IMAGE)
extern EwFieldTypeFrom_f        ewFieldFromUDecimal;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_UINT|IMAGE */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT)
extern EwFieldTypeFrom_f        ewFieldFromDecimal;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT)
extern EwFieldTypeTo_f          ewFieldToDecimalInt;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_HEX_INT)
extern EwFieldTypeTo_f          ewFieldToHexInt;
extern EwFieldTypeFrom_f        ewFieldFromHexInt;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_INT */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING)
extern EwFieldTypeTo_f          ewFieldToHexString;
extern EwFieldTypeFrom_f        ewFieldFromHexString;
extern EwFieldTypeFree_f        ewFieldFreeHexString;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP)
extern EwFieldTypeTo_f          ewFieldToDottedIP;
extern EwFieldTypeFrom_f        ewFieldFromDottedIP;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV)
extern EwFieldTypeTo_f          ewFieldToDecnetIV;
extern EwFieldTypeFrom_f        ewFieldFromDecnetIV;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC)\
 || defined(EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC)\
 || defined(EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
extern EwFieldTypeFrom_f        ewFieldFromMAC;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_IEEE|FDDI|STD_MAC */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC)\
 || defined(EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
extern EwFieldTypeTo_f          ewFieldToIEEEMAC;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_IEEE|STD_MAC */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC)
extern EwFieldTypeTo_f          ewFieldToFDDIMAC;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
extern EwFieldTypeFrom_f        ewFieldFromStdMAC;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_STD_MAC */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
extern EwFieldTypeFrom_f        ewFieldFromFile;
extern EwFieldTypeFree_f        ewFieldFreeFile;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

#if defined(EW_CONFIG_OPTION_FIELDTYPE_OID)
extern EwFieldTypeTo_f          ewFieldToObjectID;
extern EwFieldTypeFrom_f        ewFieldFromObjectID;
extern EwFieldTypeFree_f        ewFieldFreeObjectID;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
extern EwFieldTypeFrom_f        ewFieldFromDynamicSingleSel;
extern EwFieldTypeFrom_f        ewFieldFromDynamicMultiSel;
extern EwFieldTypeFree_f        ewFieldFreeDynamicMultiSel;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

#ifdef EW_CONFIG_OPTION_SOAP
extern EwFieldTypeTo_f          ewuFieldToBase64;
extern EwFieldTypeFrom_f        ewuFieldFromBase64;
extern EwFieldTypeFree_f        ewuFieldFreeBase64;
extern EwFieldTypeTo_f          ewuFieldToBoolean;
extern EwFieldTypeFrom_f        ewuFieldFromBoolean;
extern EwFieldTypeTo_f          ewuFieldToUUID;
extern EwFieldTypeFrom_f        ewuFieldFromUUID;
#endif /* EW_CONFIG_OPTION_SOAP */

extern EwFieldTypeTo_f          ewFieldToDynamic;

typedef struct EwFormFieldTable_s
  {
    uintf                       field_size;     /* size of field */
    EwFieldTypeTo_f             *to_f;          /* convert to HTML */
    EwFieldTypeFrom_f           *from_f;        /* convert from HTML */
    EwFieldTypeFree_f           *free_f;        /* free dynamic pointer */
  } EwFormFieldTable;

#ifdef EW_DEFINE_FIELD_TYPE_TABLE
#ifndef __cplusplus
const
#endif

/* This array is indexed by EwFieldType.
 * Note that if a group of elements is #ifdef'ed out because an option is
 * not configured in, then there is a #else of an equal number of null lines.
 */
EwFormFieldTable ewFormFieldTable[] =
  {

    { sizeof(uint32),  ewFieldToDecimalUint,    NULL,                NULL }

#  ifdef EW_CONFIG_OPTION_FIELDTYPE_RADIO
   ,{ sizeof(int),     NULL,                    ewFieldFromRadio,    NULL }
   ,{ 0,               ewFieldToRadioField,     NULL,                NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_RADIO */
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_RADIO */

#  ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE
   ,{ sizeof(int),     NULL,                    ewFieldFromRadio,    NULL }
   ,{ 0,               ewFieldToSelectField,    NULL,                NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE */
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_SINGLE */

#  ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_MULTIPLE
   ,{ sizeof(boolean),  ewFieldToSelectMulti, ewFieldFromSelectMulti,NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_MULTIPLE */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_MULTIPLE */

#  ifdef EW_CONFIG_OPTION_FIELDTYPE_CHECKBOX
   ,{ sizeof(boolean), ewFieldToCheckbox,    ewFieldFromCheckbox,    NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_CHECKBOX */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_CHECKBOX */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_TEXT)
   ,{ sizeof(char *),   ewFieldToText,         ewFieldFromText,ewFieldFreeText }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_TEXT */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_TEXT */

#  ifdef EW_CONFIG_OPTION_FIELDTYPE_IMAGE
   ,{ sizeof(uint32),   NULL,          ewFieldFromUDecimal,  NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_IMAGE */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_IMAGE */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_UINT)
   ,{ sizeof(uint32),   ewFieldToDecimalUint,   ewFieldFromUDecimal,  NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_UINT */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_UINT */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT)
   ,{ sizeof(uint32),   ewFieldToDecimalInt,    ewFieldFromDecimal,  NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_DECIMAL_INT */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_HEX_INT)
   ,{ sizeof(uint32),   ewFieldToHexInt,        ewFieldFromHexInt,   NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_HEX_INT */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_INT */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING)
   ,{ sizeof(EwsFormFieldHexString), ewFieldToHexString, ewFieldFromHexString,
      ewFieldFreeHexString }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_HEX_STRING */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP)
   ,{ sizeof(uint32),   ewFieldToDottedIP,      ewFieldFromDottedIP, NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_DOTTEDIP */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC)
   ,{ 6,                ewFieldToIEEEMAC,       ewFieldFromMAC,      NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_IEEE_MAC */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC)
   ,{ 6,                ewFieldToFDDIMAC,       ewFieldFromMAC,      NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_FDDI_MAC */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_STD_MAC)
   ,{ 6,                ewFieldToIEEEMAC,       ewFieldFromStdMAC,   NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_STD_MAC */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_STD_MAC */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV)
   ,{ sizeof(uint16),   ewFieldToDecnetIV,      ewFieldFromDecnetIV, NULL }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_DECNET_IV */

#  ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
   ,{ sizeof(EwaFileHandle), NULL,        ewFieldFromFile,ewFieldFreeFile }
   ,{ sizeof(char *),   ewFieldToText,    ewFieldFromText,ewFieldFreeText }
#  else  /* !EW_CONFIG_OPTION_FIELDTYPE_FILE */
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */

#  if defined(EW_CONFIG_OPTION_FIELDTYPE_OID)
   ,{ sizeof(EwsFormFieldObjectID), ewFieldToObjectID, ewFieldFromObjectID,
      ewFieldFreeObjectID }
#  else /* EW_CONFIG_OPTION_FIELDTYPE_OID */
   ,{ 0,                NULL,                   NULL,                NULL }
#  endif /* EW_CONFIG_OPTION_FIELDTYPE_OID */

#ifdef EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC
   ,{ 0,                NULL, ewFieldFromDynamicSingleSel, NULL }
   ,{ 0,                NULL , ewFieldFromDynamicMultiSel,
      ewFieldFreeDynamicMultiSel }
#else /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }
#endif /* EW_CONFIG_OPTION_FIELDTYPE_SELECT_DYNAMIC */

   /* ewFieldTypeRepeat, ewFieldTypeRepeatEnd */
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }

#if defined(EW_CONFIG_OPTION_FORM_REPEAT) && \
    defined(EW_CONFIG_OPTION_FIELDTYPE_RADIO)
   ,{ sizeof(int),      NULL,                   ewFieldFromFormRadio, NULL }
   ,{ 0,                ewFieldToFormRadioField, NULL,               NULL }
#else /* EW_CONFIG_OPTION_FORM_REPEAT && RADIO */
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }
#endif /* EW_CONFIG_OPTION_FORM_REPEAT && RADIO */

#if 0
   ,{ sizeof(char *),   ewUFieldToBinBase64,    ewUFieldFromBinBase64, ewUFieldFreeBinBase64}
   ,{ sizeof(boolean),  ewUFieldToBoolean,      ewUFieldFromBoolean, NULL}
   ,{ 36,               ewUFieldToUUID,         ewUFieldFromUUID,    NULL}
else
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }
#endif

#ifdef EW_CONFIG_OPTION_SOAP
   ,{ sizeof(boolean),  ewuFieldToBoolean,      ewuFieldFromBoolean, NULL}
   ,{ sizeof(EwsFormFieldHexString), ewuFieldToBase64, ewuFieldFromBase64,
                                                          ewFieldFreeHexString}
   ,{ 16,               ewuFieldToUUID,         ewuFieldFromUUID,    NULL}
#else /* EW_CONFIG_OPTION_SOAP */
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }
   ,{ 0,                NULL,                   NULL,                NULL }
#endif /* EW_CONFIG_OPTION_SOAP */
   ,{ sizeof(EwsFormFieldDynamic), ewFieldToDynamic, NULL,          NULL}
  };
#else /* EW_DEFINE_FORM_FIELD_TYPE_TABLE */
extern
#ifndef __cplusplus
const
#endif
EwFormFieldTable ewFormFieldTable[];
#endif /* EW_DEFINE_FORM_FIELD_TYPE_TABLE */

#endif /* _EW_FORM_H */
