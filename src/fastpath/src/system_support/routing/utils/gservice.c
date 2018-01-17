/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename       gservice.c
 *
 * @purpose        This file contains API gate procedures for initialization
 *                 and access to different tree types.
 *
 * @component      Routing Utils Component
 *
 * @comments
 *
 * @create         
 *
 * @author
 *
 * @end
 *
 * ********************************************************************/
#include <string.h>
#include "std.h"
#include "xx.ext"
#include "gservice.h"
#include "gservice.ext"

/*   List of the module routines:
 *
 *   Global service gate routines that are used to generalized a call
 *   to any services like level-compressed trie, Patricia tree, Binary
 *   tree, AVL tree, Hash list, Match table.
 *
 * - e_Err g_Construct
 * - void  g_Destruct
 * - e_Err g_FillObject
 * - e_Err g_Insert
 * - e_Err g_Change
 * - e_Err g_Delete
 * - e_Err g_Find
 * - e_Err g_FindNext
 * - e_Err g_Browse
 * - e_Err g_WriteLock
 * - e_Err g_WriteUnLock
 * - e_Err g_GetEntriesNmb
 * - void  g_Cleanup
 * - e_Err g_GetFirst
 * - e_Err g_GetNext
 * - e_Err g_SortedInsert
 * - e_Err g_FindBrowse
 * - e_Err g_DeleteFixed
 * - e_Err g_Move
 * - e_Err g_Interrupt
 * - void  g_PrintEntry
 *
 *   Null targets that are used for global service aspect initialization.
 *
 * - static e_Err n_Construct
 * - static void  n_Destruct
 * - static e_Err n_Delete
 * - static e_Err n_DeleteFixed
 * - static e_Err n_FillObject
 * - static e_Err n_Insert
 * - static e_Err n_Change
 * - static e_Err n_Find
 * - static e_Err n_FindNext
 * - static e_Err n_FindBrowse
 * - static e_Err n_Browse
 * - static e_Err n_WriteLock
 * - static e_Err n_WriteUnLock
 * - static e_Err n_GetEntriesNmb
 * - static void  n_Cleanup
 * - static e_Err n_GetNext
 * - static e_Err n_GetFirst
 * - static e_Err g_GetFirst
 * - static e_Err g_GetNext
 * - static e_Err n_SortedInsert
 * - static e_Err n_Move
 * - static e_Err n_Interrupt
 * - static void  n_PrintEntry
 *
 *   Service registration, de-registration, binding and initialization routines.
 *
 * - static e_Err ServiceRegistry
 * - static e_Err ServiceDeRegistry
 * - static e_Err ServiceBind
 * - e_Err InitAllServices
*/

static e_Err ServiceRegistry      (IN  G_Service       service,
                                   OUT t_Target        **pp_Target);
static e_Err ServiceDeRegistry    (IN  G_Service       service);
static e_Err ServiceBind          (IN  t_Handle        objId,
                                   IN  t_Target        *p_Target);

/*----------------------------------------------------------------------------*/
/* Global service gate routines.                                              */
/*----------------------------------------------------------------------------*/


 /*********************************************************************
 * @purpose     Gate routine constructs an empty object instance through
 *              the call to the object constructor target.
 *
 *
 * @param service     @b{(input)}  type of general service (object type);
 * @param *dparams    @b{(input)}  inter to constructor input data;
 * @param fConstruct  @b{(input)}  object constructor;
 * @param *ftarg      @b{(input)}  is handle of the set of object methods;
 *
 * @param *p_objId    @b{(output)}  pointer to returned object instance
 *                                     handle;
 * @param  *ftarg     @b{(output)}   is handle of the set of object methods;
 *
 * @returns           E_OK         - Success
 * @returns           Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_Construct                 (IN     G_Service           service,
                                   IN     t_ConstrDataParams  *dparams,
                                   IN     GS_Constructor      fConstruct,
                                   IN OUT t_GServiceAspect    *ftarg,
                                   OUT    t_Handle            *p_objId)
{
    t_Target        *p_Target = NULL;

    ServiceRegistry (service, &p_Target);

    fConstruct (dparams, ftarg, p_objId);

    ServiceBind (*p_objId, p_Target);

    return E_OK;
}



 /*********************************************************************
 * @purpose         Gate routine performs object destruction through the
 *                  call to the object destructor target.
 *
 *
 * @param objId     @b{(input)}  object instance handle returned by
 *                                g_Construct.
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void g_Destruct                   (IN  t_Handle       objId)
{
   t_Target   *p_Target = (t_Target *)objId;
   G_Service  service   = p_Target->service;

   p_Target->target->f_Destruct (objId);

   ServiceDeRegistry (service);
}


 /*********************************************************************
 * @purpose          Gate routine datafills empty object.
 *
 *
 * @param objId      @b{(input)}  object instance handle returned by
 *                                g_Construct;
 * @param entry      @b{(input)}  pointer to pre-allocated user input
 *                                data;
 * @param EntriesN   @b{(input)}  number of items allocated for datafill;
 * @param f_Fill     @b{(input)}  user defined datafill function (in case
 *                                of some special user requirements for
 *                                datafill or NULLP otherwise);
 * @param Ctrl       @b{(input)}  control parameters for f_Fill routine.
 *
 * @returns          E_OK         - Success
 * @returns          Not E_OK     - Failure, look at std.h for details
 *
 * @notes           Gate routine datafills empty object. instance or extends
 *                  the non empty object ith an input data through the call
 *                  to the object related target.nput data is to be allocated
 *                  by user (entry).
 *
 * @end
 * ********************************************************************/
e_Err g_FillObject                (IN  t_Handle       objId,
                                   IN  t_Handle       entry,
                                   IN  ulng           EntriesN,
                                   IN  GS_FILLOBJFUNC f_Fill,
                                   IN  ulng           Ctrl)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_FillObject (objId, entry, EntriesN, f_Fill, Ctrl);
}


 /*********************************************************************
 * @purpose     Gate routine inserts an element into an object instance.
 *
 * @param objId       @b{(input)} object instance handle returned by
 *                                g_Construct;
 * @param *p_key      @b{(input)} pointer to a key;
 * @param *p_value    @b{(input)} pointer to a user data
 *
 * @returns           E_OK         - Success
 * @returns           Not E_OK     - Failure, look at std.h for details
 *
 * @notes         Gate routine inserts an element into an object instance.
 *                (by key *p_key with the reference to user specified data
 *                *p_value) through the call to the object related target
 *
 * @end
 * ********************************************************************/
e_Err g_Insert                    (IN  t_Handle       objId,
                                   IN  byte*          key,
                                   IN  void*          p_value)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_Insert (objId, key, p_value);
}




 /*********************************************************************
 * @purpose           Gate routine inserts an element.
 *
 *
 * @param  objId      @b{(input)}  object instance handle returned by
 *                                 g_Construct;
 * @param *p_key      @b{(input)}  pointer to a key;
 * @param *p_value    @b{(input)}  pointer to a user data that has to
 *                                 inserted into an object instead of
 *                                 old data.
 *
 * @returns           E_OK         - Success
 * @returns           Not E_OK     - Failure, look at std.h for details
 *
 * @notes       Gate routine inserts an element (pointed by p_value into
 *              a routing object by key *p_key instead of an element
 *              currently stored within this object by this key) through
 *              the call to the object related target.
 *
 * @end
 * ********************************************************************/
e_Err g_Change                    (IN  t_Handle       objId,
                                   IN  byte*          key,
                                   IN  void*          p_value)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_Change (objId, key, p_value);
}


 /*********************************************************************
 * @purpose    Gate routine deletes an element from an object instance
 *             through the call to the object to the object related target
 *
 *
 * @param objId       @b{(input)}  object instance handle returned by
 *                                 g_Construct;
 * @param *p_key      @b{(input)}  pointer to a key;
 * @param **pp_value  @b{(input)}  double pointer to a user data.
 *
 * @returns           E_OK         - Success
 * @returns           Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_Delete                    (IN  t_Handle       objId,
                                   IN  byte           *p_key,
                                   OUT void           **pp_value)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_Delete (objId, p_key, pp_value);
}




 /*********************************************************************
 * @purpose            Gate routine performs a lookup for the exact match
 *
 * @param objId         @b{(input)}  object instance handle returned by
 *                                   g_Construct;
 * @param flag          @b{(input)}  find flag;
 * @param *p_key        @b{(input)}  pointer to a key;
 *
 * @param **pp_value    @b{(output)} double pointer to the found user data
 *                                      stored by *p_key.
 *
 * @returns             E_OK         - Success
 * @returns             Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *               Gate routine performs a lookup for the exact match
 *             for a user data stored within an object instance through
 *             the call to the object related target.
 *
 *
 * @end
 * ********************************************************************/
e_Err g_Find                     (IN  t_Handle        objId,
                                  IN  Bool            flag,
                                  IN  byte            *p_key,
                                  OUT void            **pp_value)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_Find (objId, flag, p_key, pp_value);
}



/*********************************************************************
 * @purpose    Gate routine browses an object instance through the
 *             call to the object related target.
 *
 *
 * @param objId      @b{(input)}  object instance handle returned by
 *                                g_Construct;
 * @param f_Browse   @b{(input)}  user defined browser routine;
 * @param Ctrl       @b{(input)}  control parameters for f_Browser.
 *
 * @returns          E_OK         - Success
 * @returns          Not E_OK     - Failure, look at std.h for details
 *
 * @notes            This target applies user defined browse routine
 *                   f_Browser to every valid object entry.
 *
 * @end
 * ********************************************************************/
e_Err g_Browse                   (IN  t_Handle        objId,
                                  IN  GS_BROWSEFUNC   f_Browse,
                                  IN  ulng            Ctrl)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_Browse (objId, f_Browse, Ctrl);
}


/*********************************************************************
 * @purpose         Gate routine is used to prevent an object instance
 *                  from write access through the call to the object
 *
 *
 * @param  objId    @b{(input)}  routing object handle returned by
 *                              g_Construct.
 *
 * @returns         E_OK         - Success
 * @returns         Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_WriteLock                (IN  t_Handle        objId)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_WriteLock (objId);
}


 /*********************************************************************
 * @purpose         Gate routine allows write access to an object through
 *                  the call to the object related target
 *
 *
 * @param  objId     @b{(input)}  object instance handle returned by
 *                           g_Construct.
 *
 * @returns         E_OK         - Success
 * @returns         Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_WriteUnLock              (IN  t_Handle        objId)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_WriteUnLock (objId);
}


 /*********************************************************************
 * @purpose    Gate routine returns number of the object instance entries
 *             through the call to the object related target.
 *
 *
 * @param objId         @b{(input)}  object instance handle returned by
 *                                   g_Construct;
 * @param *p_entries    @b{(input)}  pointer to the number of user data
 *                                   items.
 *
 * @returns             E_OK         - Success
 * @returns             Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_GetEntriesNmb            (IN  t_Handle        objId,
                                  OUT  ulng           *p_entries)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_GetEntriesNmb (objId, p_entries);
}


 /*********************************************************************
 * @purpose    Gate routine performs object instance cleanup through the
 *             call to the object related target.
 *
 *
 * @param  objId     @b{(input)} object instance handle returned by
 *                               g_Construct.
 * @param flag      @b{(input)} flag indicating whether entry is to be
 *                              deleted or not
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void g_Cleanup                  (IN  t_Handle         objId,
                                 IN  Bool             flag)
{
   t_Target   *p_Target = (t_Target *)objId;

   p_Target->target->f_Cleanup (objId, flag);
}



 /*********************************************************************
 * @purpose    Gate routine returns the first object instance entry
 *             through the call to the object related target.
 *
 *
 * @param objId        @b{(input)}  object instance handle returned by
 *                                g_Construct;
 * @param **pp_value   @b{(input)}  double pointer to put the value.
 *
 * @returns            E_OK         - Success
 * @returns            Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_GetFirst                (IN  t_Handle         objId,
                                 OUT void             **pp_value)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_GetFirst (objId, pp_value);
}


 /*********************************************************************
 * @purpose        Gate routine returns the next from last accessed entry
 *                 in an object instance through the call to the object
 *                 related target.
 *
 *
 * @param objId       @b{(input)}  object instance handle returned by
 *                                  g_Construct;
 * @param *p_value    @b{(input)}  last accessed value.
 *
 * @param **pp_value  @b{(output)}  double pointer to put the value;
 *
 * @returns           E_OK         - Success
 * @returns           Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_GetNext                 (IN  t_Handle         objId,
                                 OUT void             **pp_value,
                                 IN  void             *p_value)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_GetNext (objId, pp_value, p_value);
}



 /*********************************************************************
 * @purpose          Gate routine to initialize new entry in sorted way
 *
 *
 * @param objId      @b{(input)}  object handle
 * @param *p_key     @b{(input)}  pointer to key
 * @param *p_value   @b{(input)}  pointer to value
 *
 * @returns          E_OK         - Success
 * @returns          Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_SortedInsert            (IN  t_Handle         objId,
                                 IN  byte             *p_key,
                                 IN  void             *p_value)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_SortedInsert (objId, p_key, p_value);
}


/*********************************************************************
 * @purpose    Find entry which hits specified key next after previously
 *             found in f_Find / f_FindNext loop
 *
 *
 * @param   objId         @b{(input)}  object handle
 * @param  *p_key         @b{(input)}  pointer to key
 * @param  *p_valueLast   @b{(input)}  pointer to value from which get
 *                                        next operation is to be started
 *
 * @param  **pp_value     @b{(output)} pointer to return value or NULLP
 *
 * @returns               E_OK         - Success
 * @returns               Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_FindNext                (IN  t_Handle   objId,
                                 IN  byte       *p_key,
                                 IN  void       *p_valueLast,
                                 OUT void       **pp_value)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_FindNext (objId, p_key, p_valueLast, pp_value);
}



 /*********************************************************************
 * @purpose     Browse entries of object which hit specified key, calling
 *              browse funcion for every entry,if browse funcion for some
 *              entry returns FALSE, delete the entry
 *
 *
 * @param  objId     @b{(input)}  object handle
 * @param *p_key     @b{(input)}  pointer to key
 * @param f_Browse   @b{(input)}  user defined browse function
 * @param param      @b{(input)}  parameter to be passed to browse function
 *
 * @returns          E_OK        success
 * @returns          E_BADPARM   wrong parameters
 *
 * @notes           calling browse funcion for every entry,if browse funcion
 *                  for some entry returns FALSE, delete the entry
 *
 * @end
 * ********************************************************************/
e_Err g_FindBrowse              (IN  t_Handle      objId,
                                 IN  byte          *p_key,
                                 IN  GS_BROWSEFUNC f_Browse,
                                 IN  ulng          param)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_FindBrowse (objId, p_key, f_Browse, param);
}



 /*********************************************************************
 * @purpose          Delete the first entry by key from object
 *
 *
 * @param objId      @b{(input)}   object handle
 * @param *p_key     @b{(input)}   pointer to key
 *
 * @param *p_value   @b{(output)}  pointer to value to be deleted
 *
 * @returns         E_OK        success
 * @returns         E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_DeleteFixed             (IN  t_Handle objId,
                                 IN  byte     *p_key,
                                 OUT void     *p_value)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_DeleteFixed (objId, p_key, p_value);
}


 /*********************************************************************
 * @purpose            Move the value to other entry according to the new
 *                     key
 *
 *
 * @param  objId       @b{(input)}  object Id
 * @param  *p_oldKey   @b{(input)}  key to find entry
 * @param  *p_value    @b{(input)}  entry value
 * @param  **p_newKey  @b{(input)}  new key to be changed and moved
 *
 * @returns            E_OK        success
 * @returns            E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_Move                    (IN  t_Handle objId,
                                 IN  byte     *p_oldKey,
                                 IN  void     *p_value,
                                 IN  byte     *p_newKey)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_Move (objId, p_oldKey, p_value, p_newKey);
}


 /*********************************************************************
 * @purpose          Interrupt f_Browse
 *
 *
 * @param objId      @b{(input)}  object Id
 *
 * @returns          E_OK        success
 * @returns          E_BADPARM   wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err g_Interrupt               (IN  t_Handle  objId)
{
   t_Target   *p_Target = (t_Target *)objId;

   return p_Target->target->f_Interrupt (objId);
}


 /*********************************************************************
 * @purpose              Print all object fileds
 *
 *
 * @param objId          @b{(input)}  object Id
 * @param *p_key         @b{(input)}  pointer to a key
 * @param *p_value       @b{(input)}  pointer to a value to printed
 *                                    by f_PrintEntry
 * @param f_PrintEntry   @b{(input)}  entry specific print routine
 * @param prtFlg         @b{(input)}  print flags
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void  g_Print                     (IN  t_Handle      objId,
                                   IN  byte          *p_key,
                                   IN  void          *p_value,
                                   IN  F_PrintStruct f_PrintEntry,
                                   IN  word          prtFlg)
{
   t_Target   *p_Target = (t_Target *)objId;

   p_Target->target->f_PrintEntry (objId, p_key, p_value, f_PrintEntry, prtFlg);

   return;
}

/*----------------------------------------------------------------------------*/
/* Null targets that are used for global service aspect initialization.       */
/*----------------------------------------------------------------------------*/

static void n_Destruct          (t_Handle         objId)
{
   return;
}

static e_Err n_Delete           (t_Handle         objId,
                                 byte             *p_key,
                                 void             **pp_value)
{
   return E_OK;
}

static e_Err n_FillObject       (t_Handle         objId,
                                 t_Handle         entry,
                                 ulng             EntriesN,
                                 GS_FILLOBJFUNC   f_Fill,
                                 ulng             Ctrl)
{
   return E_OK;
}

static e_Err n_Insert           (t_Handle         objId,
                                 byte             *p_key,
                                 void             *p_value)
{
   return E_OK;
}

static e_Err n_Change           (t_Handle         objId,
                                 byte             *p_key,
                                 void             *p_value)
{
   return E_OK;
}

static e_Err n_Find             (t_Handle         objId,
                                 Bool             flag,
                                 byte             *p_key,
                                 void             **pp_value)
{
   return E_OK;
}

static e_Err n_Browse           (t_Handle         objId,
                                 GS_BROWSEFUNC    f_Browse,
                                 ulng             Ctrl)
{
   return E_OK;
}

static e_Err n_WriteLock        (t_Handle         objId)
{
   return E_OK;
}


static e_Err n_WriteUnLock      (t_Handle         objId)
{
   return E_OK;
}

static e_Err n_GetEntriesNmb    (t_Handle         objId,
                                 ulng             *p_entries)
{
   return E_OK;
}

static void n_Cleanup           (t_Handle         objId,
                                 Bool             flag)
{
   return;
}

static e_Err n_GetNext          (t_Handle         objId,
                                 void             **pp_value,
                                 void             *p_value)
{
   return E_OK;
}

static e_Err n_GetFirst         (t_Handle         objId,
                                 void             **pp_value)

{
   return E_OK;
}

static e_Err n_SortedInsert     (IN  t_Handle         objId,
                                 IN  byte             *p_key,
                                 IN  void             *p_value)
{
   return E_OK;
}

e_Err n_FindNext                (IN  t_Handle   objId,
                                 IN  byte       *p_key,
                                 IN  void       *p_valueLast,
                                 OUT void       **pp_value)
{
   return E_OK;
}

e_Err n_FindBrowse              (IN  t_Handle      objId,
                                 IN  byte          *p_key,
                                 IN  GS_BROWSEFUNC f_Browse,
                                 IN  ulng          param)
{
   return E_OK;
}

e_Err n_DeleteFixed             (IN  t_Handle objId,
                                 IN  byte     *p_key,
                                 IN  void     *p_value)
{
   return E_OK;
}

e_Err n_Move                    (IN  t_Handle objId,
                                 IN  byte     *p_oldKey,
                                 IN  void     *p_value,
                                 IN  byte     *p_newKey)
{
   return E_OK;
}

e_Err n_Interrupt               (IN  t_Handle  objId)
{
   return E_OK;
}

void  n_PrintEntry                (IN   t_Handle       objId,
                                   IN   byte           *p_key,
                                   IN   void           *p_value,
                                   IN   F_PrintStruct  f_PrintEntry,
                                   IN   word           prtFlg)
{
   return;
}

/*----------------------------------------------------------------------------*/
/* Service registration, de-registration, binding and initialization routines.*/
/*----------------------------------------------------------------------------*/


 /*********************************************************************
 * @purpose             Routine performs registartion of a new service
 *
 *
 * @param service       @b{(input)}  service to be registered;
 * @param **pp_Target   @b{(input)}  double pointer to a target vector.
 *
 * @returns             E_OK         - Success
 * @returns             Not E_OK     - Failure, look at std.h for details
 *
 * @notes         for example, LC-trie, Binary tree,AVL, Patricia tree etc
 *                Actaully routine initialized an appropriate entry in the
 *                target table (t_Target vector) according to an input service.
 *
 * @end
 * ********************************************************************/
static e_Err ServiceRegistry    (IN  G_Service        service,
                                 OUT t_Target         **pp_Target)
{
   if ( (service < 0) || (service >= MAX_SERVICES) )
   {
      return E_FAILED;
   }

   if ( Services[service].registered )
   {
      Services[service].ObjCounter++;
      *pp_Target = &Services[service].target;
   }
   else
   {
      *pp_Target = XX_Malloc(sizeof(t_Target));

      if ( *pp_Target == NULLP )
      {
         return E_NOMEMORY;
      }

      memset (*pp_Target, 0, sizeof(t_Target));

      (*pp_Target)->target = XX_Malloc(sizeof(t_GServiceAspect));

      if ( (*pp_Target)->target == NULLP )
      {
         return E_NOMEMORY;
      }

      memset ((*pp_Target)->target, 0, sizeof(t_GServiceAspect));

      (*pp_Target)->target = n_Aspect;

      switch ( service )
      {
      case LCT_S:

        break;

      case PT_S:

        break;

      case BT_S :

        break;

      case MT_S :

        break;

      case HL_S :

        break;

      case AVL_S :

        break;

      case AVLH_S :

        break;

      default:
         ASSERT(FALSE);
        break;
      }

      Services[service].registered = TRUE;
      Services[service].ObjCounter++;
      Services[service].target.target  = (**pp_Target).target;
      Services[service].target.service = service;
      (*pp_Target)->service = service;
   }

   return E_OK;
}



/*********************************************************************
 * @purpose    Routine performs de-registartion of a service that has
 *             been in use.
 *
 *
 * @param  service       @b{(input)}  service to be registered;
 *
 * @returns              E_OK         - Success
 * @returns              Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err ServiceDeRegistry    (IN  G_Service      service)
{
   if ( (service < 0) || (service >= MAX_SERVICES) )
   {
      return E_FAILED;
   }

   if ( --Services[service].ObjCounter == 0 )
   {
       if ( Services[service].target.target != NULLP )
       {
          XX_Free(Services[service].target.target);
          Services[service].registered = FALSE;
       }
   }
   return E_OK;
}



 /*********************************************************************
 * @purpose    Routine binds a vector of target routines to an appropriate
 *             object instance.
 *
 *
 * @param objId      @b{(input)}  object instance handle as returned by
 *                                the object constructor;
 * @param *p_Target  @b{(input)}  pointer to a target vector.
 *
 * @returns          E_OK         - Success
 * @returns          Not E_OK     - Failure, look at std.h for details
 *
 * @notes
 *
 * @end
 * ********************************************************************/
static e_Err ServiceBind           (IN  t_Handle        objId,
                                    IN  t_Target        *p_Target)
{

   ((t_Target*)objId)->target  = p_Target->target;
   ((t_Target*)objId)->service = p_Target->service;

   return E_OK;

}


/*********************************************************************
 * @purpose     Routine is to be invoked during global system initializ
 *              ation(system startup).
 *
 * @returns     E_OK         - Success
 * @returns     Not E_OK     - Failure, look at std.h for details
 *
 * @notes       The purpose of this routine is allocate null target for
 *              the generalized service aspect and to initialize all the
 *              entries in a service table to null target value.
 *
 * @end
 * ********************************************************************/
e_Err InitAllServices             (void)
{

    G_Service      service;

    n_Aspect = XX_Malloc(sizeof(t_GServiceAspect));

    if ( n_Aspect == NULLP )
    {
       return E_NOMEMORY;
    }

    memset (n_Aspect, 0, sizeof(t_GServiceAspect));

    n_Aspect->f_Destruct        = n_Destruct;
    n_Aspect->f_Insert          = n_Insert;
    n_Aspect->f_SortedInsert    = n_SortedInsert;
    n_Aspect->f_Delete          = n_Delete;
    n_Aspect->f_DeleteFixed     = n_DeleteFixed;
    n_Aspect->f_Change          = n_Change;
    n_Aspect->f_Move            = n_Move;
    n_Aspect->f_Find            = n_Find;
    n_Aspect->f_FindNext        = n_FindNext;
    n_Aspect->f_FindBrowse      = n_FindBrowse;
    n_Aspect->f_Browse          = n_Browse;
    n_Aspect->f_FillObject      = n_FillObject;
    n_Aspect->f_WriteLock       = n_WriteLock;
    n_Aspect->f_WriteUnLock     = n_WriteUnLock;
    n_Aspect->f_GetEntriesNmb   = n_GetEntriesNmb;
    n_Aspect->f_Cleanup         = n_Cleanup;
    n_Aspect->f_GetFirst        = n_GetFirst;
    n_Aspect->f_GetNext         = n_GetNext;
    n_Aspect->f_Interrupt       = n_Interrupt;
    n_Aspect->f_PrintEntry      = n_PrintEntry;

    for ( service = 0; service < MAX_SERVICES; service++ )
    {
       Services[service].registered     = FALSE;
       Services[service].ObjCounter     = 0;
       Services[service].target.target  = n_Aspect;
       Services[service].target.service = service;
    }

    return E_OK;
}
