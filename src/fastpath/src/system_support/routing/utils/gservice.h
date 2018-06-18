/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename          gservice.h
 *
 * @purpose           To make a call from application to a service
 *                    (LC-trie, AVL tree, Patricia tree, Binary tree)
 *                    transparently.
 *
 * @component         Routing Utils Component
 *
 * @comments
 *                     * This file contains
 *  general definitions;
 *  for initialization;
 *  prototypes for user defined functions;
 *  definition of aspect that provides the vector of target routines
 *  for every available service. Every such target is to be call from
 *  the related gate procedure in order to make a call from application
 *  to a service (LC-trie, AVL tree, Patricia tree, Binary tree) transparently.
 *  Application should be aware of a service only when it calls an object
 *  constructor gate.
 *
 * @create            
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#ifndef gservice_h
#define gservice_h

#include "gservice.ext"

/* IP Version 4 / Versoin 6 definitions  */

#define Verson_4

#define IP_Address_Length_Version_4   32
#define IP_Address_Length_Version_6  128

#ifdef Verson_4
#define IP_Address_Length    IP_Address_Length_Version_4
#endif

#ifdef Verson_6
#define IP_Address_Length    IP_Address_Length_Version_6
#define Subnetwork_ID_Length 64
#define IF_ID_Length         IP_Address_Length_Version_6 - Subnetwork_ID_Length
#endif

#define IP_String_Length     IP_Address_Length/IP_Address_Length_Version_4

#define DEFAULT_VALUE        NULLP /* Default value to be returned by lookup
                                      target in case no match is found          */

#define ADRSIZE       32           /* Size of address                           */

typedef ulng KEYVAL;               /* Key bitpattern                            */
typedef byte KEYLEN;               /* Key length in bits                        */
typedef ulng SP_IPADR;             /* Address of a data destination             */

/* Pointer to a null aspect. */

static t_GServiceAspect *n_Aspect;

/* Structure contains the data of the service entry in the service table.          */
typedef struct t_Service
{
   Bool            registered;     /* The flag indicating if the particular
                                      service has been registered or not.          */
   byte            ObjCounter;     /* Counter for keeping the number of object
                                      instances  currently bound to the service.   */
   t_Target        target;         /* Service type and service and a pointer to
                                      a target vector.                             */
} t_Service;

/* Service registry table. It contains the entries for the target routines vectors
   per every defined service.
   All the table entries is initialized to null (*n_Aspect) upon global initialization
   (when routine InitAllServices is invoked).
   The table is datafilled for the related entry when ServiceRegistry
   routine is invoked the first time for this entry (service).
   The table is cleared for the related entry when ServiceDeRegistry
   routine is invoked for this entry (service) and there is no more objects
   that make use of this particular service.
*/

static t_Service  Services[MAX_SERVICES];

/* ---------------------- Null targets -------------------------------- */
/* The related gate procedures are defined in the module gservice.ext.  */

static void n_Destruct            (t_Handle       objId);

static e_Err n_Delete             (t_Handle       objId,
                                   byte           *p_key,
                                   void           **pp_value);

static e_Err n_FillObject         (t_Handle       objId,
                                   t_Handle       entry,
                                   ulng           EntriesN,
                                   GS_FILLOBJFUNC f_Fill,
                                   ulng           Ctrl);

static e_Err n_Insert             (t_Handle       objId,
                                   byte           *p_key,
                                   void           *p_value);

static e_Err n_Change             (t_Handle       objId,
                                   byte           *p_key,
                                   void           *p_value);

static e_Err n_Find               (t_Handle       objId,
                                   Bool           flag,
                                   byte           *p_key,
                                   void           **pp_value);

static e_Err n_Browse             (t_Handle       objId,
                                   GS_BROWSEFUNC  f_Browse,
                                   ulng           Ctrl);

static e_Err n_WriteLock          (t_Handle       objId);

static e_Err n_WriteUnLock        (t_Handle       objId);

static e_Err n_GetEntriesNmb      (t_Handle       objId,
                                   ulng           *p_entries);

static void n_Cleanup             (t_Handle       objId,
                                   Bool           flag);

static e_Err n_GetNext            (t_Handle       objId,
                                   void           **pp_value,
                                   void           *p_value);

static e_Err n_GetFirst           (t_Handle       objId,
                                   void           **pp_value);

static e_Err n_SortedInsert     (IN  t_Handle         objId,
                                 IN  byte             *p_key,
                                 IN  void             *p_value);

static e_Err n_FindNext         (IN  t_Handle   objId,
                                 IN  byte       *p_key,
                                 IN  void       *p_valueLast,
                                 OUT void       **pp_value);

static e_Err n_FindBrowse       (IN  t_Handle      objId,
                                 IN  byte          *p_key,
                                 IN  GS_BROWSEFUNC f_Browse,
                                 IN  ulng          param);

static e_Err n_DeleteFixed      (IN  t_Handle objId,
                                 IN  byte     *p_key,
                                 IN  void     *p_value);

static e_Err n_Move             (IN  t_Handle objId,
                                 IN  byte     *p_oldKey,
                                 IN  void     *p_value,
                                 IN  byte     *p_newKey);

static e_Err n_Interrupt        (IN  t_Handle  objId);

static void  n_PrintEntry       (IN   t_Handle       objId,
                                 IN   byte           *p_key,
                                 IN   void           *p_value,
                                 IN   F_PrintStruct  f_PrintEntry,
                                 IN   word           prtFlg);

#endif
