/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename    bt.c
 *
 * @purpose     Binary tree object implementation
 *
 * @component   Routing Utils Component
 *
 * @comments  
 *      ExternalRoutines:
 * BT_Initinitbinarytreeobject
 * BT_Destroydestroybinarytreeobject
 * BT_Insertinsertanewentry
 * BT_Findfindentry
 * BT_Deletedeleteentry
 * BT_GetEntriesNmbgetnumberofentries
 * BT_Cleanupdeallocateetriesmemory
 * BT_GetFirstgetfirst(root)entryofbinarytree
 * BT_GetNextgetnextentry
 * BT_Browsebrowsebinarytree
 * BT_ChangechangeBTentry'svalue
 * BT_InterruptinterruptBT_Browserunning
 * 
 *
 * @create     02/01/1999
 *
 * @author     Dan Dovolsky 
 *     
 * @end
 *
 * ********************************************************************/
#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: /home/cvs/PR1003/PR1003/FASTPATH/RELEASE_610_FP_6.1.0.5-FastPath-Ent-esw-xgs3-bmw-LR-CSxw-IQH/src/system_support/routing/utils/bt.c,v 1.1 2011/04/18 17:10:53 mruas Exp $";
#endif


/* --- standard include files --- */

#include <string.h>
#include <stdlib.h>
#include "xx.ext"
#include "bt.ext"
#include "common.h"

/* Value structure definition */
typedef struct tagt_Value 
{
    struct tagt_Value *left;       /* left child */
    struct tagt_Value *right;      /* right child */
    struct tagt_Value *parent;     /* parent */
    void    *value;             /* user data */
} t_Value;

/* binary tree instance structure */
typedef struct tagt_BT
{
#ifdef ERRCHK
   struct tagt_BT *next;
   struct tagt_BT *prev;
#endif
   word       status;              /* current status BT_STAT_Valid */
   t_Value    *root;               /* root of binary tree   */
   Bool       dataOffset;          /* handle pointer offset in the entry */
   Bool       keyIsValue;          /* key and value are equal */
   word       valueNmb;            /* current values number in the table */
   byte       keyLen;              /* key length in bytes */
   word       keyOffset;           /* key offset in the value */
   int        (*f_KeyCompare)(void *, void *); /* user key compare function */
   byte       *(*f_getKey)(void *);/* user get key function */
   int        cost;                /* cost of find function comparision */
   Bool       browseInterrupt;     /* browse function interrupt flag  */
   t_Handle   rootsave;            /* saved root entry for browse interrupt */
   t_Value    *getValue;           /* current value for the find and get first/next requests */
} t_BT;

#ifdef ERRCHK
/* global list of objects for the debuging only */
t_BT *BTObjList = NULLP;
#endif

#define GET_VALUE(bt,v)((void *)((bt)->dataOffset? ((t_Value *)(v))->value: v)) 

#define GET_KEYPOINTER(bt,v) ((byte *)(((byte *)(GET_VALUE(bt,v))) + (bt)->keyOffset))

#define GET_KEY(bt,v) \
 ( ((bt)->f_getKey) ? (*(bt)->f_getKey)( (bt)->dataOffset ? ((t_Value*)(v))->value : v ) : \
             (bt)->keyIsValue ? ((byte*)&((t_Value*)(v))->value): GET_KEYPOINTER(bt,v))

#define BT_OFFSET ((word)(sizeof(void *) * 3))

#define BT_STAT_Valid 31901

#define KEY_COMPARE(bt,k1,k2) (((bt)->f_KeyCompare) ? \
         ((bt)->f_KeyCompare)(k1,k2) : memcmp(k1, k2,(bt)->keyLen))

#define KEY_COMPARE_EXACT(bt,k1,k2) (memcmp(k1, k2,(bt)->keyLen))



/*********************************************************************
*@purpose                Initialize BT object.
*
*
* @param  flags          @b{(input)}  control flags: BT_OWN_LINK,
*                                     BT_KEY_IS_VALUE
* @param  keyLen         @b{(input)}  key length in bytes
* @param  keyOffset      @b{(input)}  key offset in the value structure
* @param  f_KeyCompare   @b{(input)}  user key compare function or NULLP
* @param  f_getKey       @b{(input)}  user get key function or NULLP
* @param  p_objId        @b{(output)}  pointer to put binary tree objectId
*
* @returns               E_OK        success
* @returns               E_NOMEMORY  can't allocate memory
* @returns               E_BADPARAM  wrong parameters
*
* @notes
*
* @end
********************************************************************/
e_Err BT_Init(byte flags, byte keyLen, word keyOffset,
              int (*f_KeyCompare)(void *, void *), byte *(*f_getKey)(void *),
              t_Handle *p_objId)
{
   t_BT *p_BT;

   ASSERT(p_objId);

    /* allocate object header memory */
    if((p_BT = (t_BT *)XX_Malloc(sizeof(t_BT))) == NULLP)
        return E_NOMEMORY;

    memset (p_BT, 0, sizeof(t_BT));

    p_BT->keyLen = keyLen;
    p_BT->keyOffset = keyOffset;
    p_BT->f_KeyCompare = f_KeyCompare;
    p_BT->f_getKey = f_getKey;

    if(flags & BT_OWN_LINK)
        p_BT->dataOffset = BT_OFFSET;
    if(flags & BT_KEY_IS_VALUE)
        p_BT->keyIsValue = TRUE;

    p_BT->status = BT_STAT_Valid;
    *p_objId = p_BT;

    return E_OK;
}


/*********************************************************************
 * @purpose            destroy BT object
 *
 *
 * @param p_objId      @b{(output)}  pointer to get and clear binary tree
 *                                  object Id
 *
 * @returns            E_OK        success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_Destroy ( t_Handle *p_objId)
{
    t_BT *p_BT = (t_BT *)*p_objId;

    ASSERT(p_BT);
    ASSERT(p_BT->status == BT_STAT_Valid);

    if(p_BT->dataOffset)
       BT_Cleanup(p_BT, 0);

    p_BT->status = 0;
    XX_Free(p_BT);
    *p_objId = NULLP;

    return E_OK;
}



/*********************************************************************
 * @purpose          Insert new entry to the binary tree.
 *
 *
 * @param  objId     @b{(input)}  object Id
 * @param  key       @b{(input)}  key pointer
 * @param  value     @b{(input)}  value pointer
 *
 * @returns          E_OK        success
 * @returns          E_NOMEMORY  failed in memory allocation
 * @returns          E_IN_MATCH  duplicate key
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_InsertExact( t_Handle objId, byte *key, void *value)
{
   t_BT    *p_BT = (t_BT *)objId;
   t_Value *p_V = (t_Value *)value;
   t_Value *current, *parent;
   int comp;
   byte *k1,*k2;

   ASSERT(p_BT);
   ASSERT(p_BT->status == BT_STAT_Valid);

   /* find future parent */
   current = p_BT->root;
   parent = 0;
   while(current) 
   {
      /* compare keys */
      comp = KEY_COMPARE_EXACT(p_BT, key, GET_KEY(p_BT, current));
      
      /* check may be this same key is included already */
      if(comp == 0) 
      {
         ASSERT(comp);   
         return E_IN_MATCH;
      }

      parent = current;
      current = (comp < 0) ? current->left : current->right;
   }

   /* allocate entry memory if necessary */
   if(p_BT->dataOffset)
   {
     if((p_V = (t_Value *)XX_Malloc(sizeof(t_Value))) == NULLP)
         return E_NOMEMORY;
     p_V->value = value;
   }

   /* setup new entry */
   p_V->parent = parent;
   p_V->left = NULL;
   p_V->right = NULL;

   /* insert value in tree */
   if(parent)
   {
     k1 = GET_KEY(p_BT, p_V);
     k2 = GET_KEY(p_BT, parent);
     if(KEY_COMPARE_EXACT(p_BT, k1, k2) < 0)
         parent->left = p_V;
     else
         parent->right = p_V;
   }         
   else
     p_BT->root = p_V;

   p_BT->valueNmb++;

   return E_OK;
}


/*********************************************************************
 * @purpose          Insert new entry to the binary tree.
 *
 *
 * @param  objId     @b{(input)}  object Id
 * @param  key       @b{(input)}  key pointer
 * @param  value     @b{(input)}  value pointer
 *
 * @returns          E_OK        success
 * @returns          E_NOMEMORY  failed in memory allocation
 * @returns          E_IN_MATCH  duplicate key
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_Insert( t_Handle objId, byte *key, void *value)
{
   t_BT    *p_BT = (t_BT *)objId;
   t_Value *p_V = (t_Value *)value;
   t_Value *current, *parent;
   int comp;
   byte *k1,*k2;

   ASSERT(p_BT);
   ASSERT(p_BT->status == BT_STAT_Valid);

   /* find future parent */
   current = p_BT->root;
   parent = 0;
   while(current)
   {
      /* compare keys */
      comp = KEY_COMPARE(p_BT, key, GET_KEY(p_BT, current));
      
      /* check may be this same key is included already */
      if(comp == 0) 
      {
         ASSERT(comp);   
         return E_IN_MATCH;
      }

      parent = current;
      current = (comp < 0) ? current->left : current->right;
   }

   /* allocate entry memory if necessary */
   if(p_BT->dataOffset)
   {
     if((p_V = (t_Value *)XX_Malloc(sizeof(t_Value))) == NULLP)
         return E_NOMEMORY;
     p_V->value = value;
   }

   /* setup new entry */
   p_V->parent = parent;
   p_V->left = NULL;
   p_V->right = NULL;

   /* insert value in tree */
   if(parent)
   {
     k1 = GET_KEY(p_BT, p_V);
     k2 = GET_KEY(p_BT, parent);
     if(KEY_COMPARE(p_BT, k1, k2) < 0)
         parent->left = p_V;
     else
         parent->right = p_V;
   }         
   else
     p_BT->root = p_V;

   p_BT->valueNmb++;

   return E_OK;
}


/*********************************************************************
 * @purpose          Delete the entry from the binary tree.
 *
 *
 * @param objId      @b{(input)}  object Id
 * @param key        @b{(input)}  key pointer
 * @param value      @b{(output)}  pointer to put found value or NULL
 *
 * @returns          E_OK        success
 * @returns          E_NOT_FOUND entry wasn't found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_DeleteExact( t_Handle objId, byte *key, void **value)
{
   t_BT    *p_BT = (t_BT *)objId;
   t_Value *x, *y, *z;
   int     comp;

   ASSERT(p_BT);
   ASSERT(p_BT->status == BT_STAT_Valid);

   /* find node in tree */
   z = p_BT->root;
   while(z)
   {
      /* compare keys */
      comp = KEY_COMPARE_EXACT(p_BT, key, GET_KEY(p_BT, z));

     if(comp == 0) 
         break;
     else
         z = (comp < 0) ? z->left : z->right;
   }
   
   if(!z)
      return E_NOT_FOUND;

   /* return found value */
   if(value)
      *value = GET_VALUE(p_BT, z);
      
   /* find tree successor */
   if((z->left == NULL) || (z->right == NULL))
     y = z;
   else 
   {
     y = z->right;
     
     while(y->left) 
        y = y->left;
   }

   /* x is y's only child */
   if(y->left != NULL)
     x = y->left;
   else
     x = y->right;

   /* remove y from the parent chain */
   if(x)
      x->parent = y->parent;

   if(y->parent)
   {
     if(y == y->parent->left)
       y->parent->left = x;
     else
       y->parent->right = x;
   }
   else
     p_BT->root = x;

   /* if z and y are not the same, replace z with y. */
   if(y != z)
   {
     y->left = z->left;
     if(y->left)
        y->left->parent = y;

     y->right = z->right;
     if(y->right)
        y->right->parent = y;

     y->parent = z->parent;
     if(z->parent)
     {
        if(z == z->parent->left)
          z->parent->left = y;
        else
          z->parent->right = y;
     }
     else
        p_BT->root = y;
   }

   /* delete value */
   if(p_BT->dataOffset)
     XX_Free(z);

   p_BT->valueNmb--;

   return E_OK;
}



/*********************************************************************
 * @purpose          Delete the entry from the binary tree.
 *
 *
 * @param  objId     @b{(input)}  object Id
 * @param  key       @b{(input)}  key pointer
 * @param  value     @b{(output)}  pointer to put found value or NULL
 *
 * @returns          E_OK        success
 * @returns          E_NOT_FOUND entry wasn't found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_Delete( t_Handle objId, byte *key, void **value)
{
   t_BT    *p_BT = (t_BT *)objId;
   t_Value *x, *y, *z;
   int     comp;

   ASSERT(p_BT);
   ASSERT(p_BT->status == BT_STAT_Valid);

   /* find node in tree */
   z = p_BT->root;
   while(z)
   {
      /* compare keys */
      comp = KEY_COMPARE(p_BT, key, GET_KEY(p_BT, z));

     if(comp == 0) 
         break;
     else
         z = (comp < 0) ? z->left : z->right;
   }
   
   if(!z) 
      return E_NOT_FOUND;

   /* return found value */
   if(value)
      *value = GET_VALUE(p_BT, z);
      
   /* find tree successor */
   if((z->left == NULL) || (z->right == NULL))
     y = z;
   else 
   {
     y = z->right;
     
     while(y->left)
        y = y->left;
   }

   /* x is y's only child */
   if(y->left != NULL)
     x = y->left;
   else
     x = y->right;

   /* remove y from the parent chain */
   if(x)
      x->parent = y->parent;

   if(y->parent)
   {
     if(y == y->parent->left)
       y->parent->left = x;
     else
       y->parent->right = x;
   }
   else
     p_BT->root = x;

   /* if z and y are not the same, replace z with y. */
   if(y != z)
   {
     y->left = z->left;
     if(y->left)
        y->left->parent = y;

     y->right = z->right;
     if(y->right)
        y->right->parent = y;

     y->parent = z->parent;
     if(z->parent)
     {
        if(z == z->parent->left)
          z->parent->left = y;
        else
          z->parent->right = y;
     }
     else
        p_BT->root = y;
   }

   /* delete value */
   if(p_BT->dataOffset)
     XX_Free(z);

   p_BT->valueNmb--;

   return E_OK;
}


/*********************************************************************
 * @purpose          Find the entry with the given key
 *
 *
 * @param objId      @b{(input)}  object Id
 * @param key        @b{(input)}  key pointer
 * @param p_value    @b{(output)}  pointer to put value or NULLP
 *
 * @returns          E_OK        exactly matched entry found
 * @returns          E_NOT_FOUND no exactly matched entry found, however,
 *                   the best matched entry returned in p_value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_Find( t_Handle objId, byte *key, void **p_value)
{
   t_BT    *p_BT = (t_BT *)objId;
   t_Value *current;
   int     comp;

   ASSERT(p_BT);
   ASSERT(p_BT->status == BT_STAT_Valid);

   p_BT->cost = MAX_BT_COST;
   p_BT->getValue = NULL;

   if(!(current = p_BT->root))
      return E_NOT_FOUND;
   
   while(current) 
   {
      /* compare keys */
      comp = KEY_COMPARE(p_BT, key, GET_KEY(p_BT, current));

      if(comp == 0) 
      {
         /* Exactly match entry found */
         *p_value = GET_VALUE(p_BT, current);
         
         return E_OK; 
      }
      else 
      {
         /* check this entry cost */
         if(abs(comp) < p_BT->cost)
         {
            /* if better save this entry as a best match */
            p_BT->cost = abs(comp);
            p_BT->getValue = current;    
         }  

         current = (comp < 0) ? current->left : current->right;
      }       
   }

    /* for best match return the entry with the best cost */
   if(p_BT->getValue)
      *p_value = GET_VALUE(p_BT, p_BT->getValue);
   else
      *p_value = NULL;

   return E_NOT_FOUND;
}



/*********************************************************************
 * @purpose          Find the entry with the given key
 *
 *
 * @param objId      @b{(input)}  object Id
 * @param key        @b{(input)}  key pointer
 * @param p_value    @b{(output)}  pointer to put value or NULLP
 *
 * @returns          E_OK        exactly matched entry found
 * @returns          E_NOT_FOUND no exactly matched entry found, however,
 *                   the best matched entry returned in p_value
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_FindExact( t_Handle objId, byte *key, void **p_value)
{
   t_BT    *p_BT = (t_BT *)objId;
   t_Value *current;
   int     comp;

   ASSERT(p_BT);
   ASSERT(p_BT->status == BT_STAT_Valid);

   p_BT->cost = MAX_BT_COST;
   p_BT->getValue = NULL;

   if(!(current = p_BT->root))
      return E_NOT_FOUND;
   
   while(current) 
   {
      /* compare keys */
      comp = KEY_COMPARE_EXACT(p_BT, key, GET_KEY(p_BT, current));

      if(comp == 0) 
      {
         /* Exactly match entry found */
         *p_value = GET_VALUE(p_BT, current);
         
         return E_OK; 
      }
      else 
      {
         /* check this entry cost */
         if(abs(comp) < p_BT->cost)
         {
            /* if better save this entry as a best match */
            p_BT->cost = abs(comp);
            p_BT->getValue = current;    
         }  

         current = (comp < 0) ? current->left : current->right;
      }       
   }

    /* for best match return the entry with the best cost */
   if(p_BT->getValue)
      *p_value = GET_VALUE(p_BT, p_BT->getValue);
   else
      *p_value = NULL;

   return E_NOT_FOUND;
}


/*********************************************************************
 * @purpose              Returns number of active entries in a binary
 *                       tree
 *
 *
 * @param  objId         @b{(input)}  binary tree object Id
 * @param  p_entriesNmb  @b{(output)}  pointer to put return value
 *
 * @returns              E_OK        success
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_GetEntriesNmb(t_Handle objId, word *p_entriesNmb)
{
    t_BT    *p_BT = (t_BT *)objId;

    ASSERT(p_BT);
    ASSERT(p_BT->status == BT_STAT_Valid);
    *p_entriesNmb = p_BT->valueNmb;
    return E_OK;
}




/*********************************************************************
 * @purpose           deallocate memory for all entries
 *
 *
 * @param  p_BT       @b{(input)}  binary tree object pointer
 * @param  freeEntry  @b{(input)}  free entry's memory flag
 *
 * @returns           n/a
 *
 * @notes
 *
 * @end
 * ********************************************************************/
void BT_Cleanup(t_Handle hnd, Bool freeEntry)
{
    t_BT    *p_BT = (t_BT *)hnd;
    t_Value *current, *parent;

   parent = p_BT->root;
   
   while(parent)
   {
      /* find the smallest left child */
      current = parent;
      while(current->left || current->right) 
      {
         if(current->left) 
           current = current->left;
         else if(current->right)
           current = current->right;
      }

      /* go up one step */
      parent = current->parent;

      if(parent)
      {
         /* delete the current entry from the BT and free memory */
         if(parent->left == current)
            parent->left = NULL;
         else if(parent->right == current)
            parent->right = NULL;
      }

      if(p_BT->dataOffset && freeEntry)
         XX_Free(GET_VALUE(p_BT, current));

      XX_Free(current);
   }      
}



/*********************************************************************
 * @purpose            Get first entry from the binary tree
 *
 *
 * @param  objId       @b{(input)}  binary tree object Id
 * @param  p_value     @b{(output)}  pointer to put the value
 *
 * @returns            E_OK            success
 * @returns            E_NOT_FOUND     hash list is empty
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_GetFirst(t_Handle objId, void **p_value)
{
    t_BT    *p_BT = (t_BT *)objId;

    ASSERT(p_BT);
    ASSERT(p_BT->status == BT_STAT_Valid);

    if(p_BT->root == NULL)
      return E_NOT_FOUND;
    
    p_BT->getValue = p_BT->root;    
        
    *p_value = GET_VALUE(p_BT, p_BT->root);

    return E_OK;
}



/*********************************************************************
 * @purpose            Get next entry in binary tree from last accessed.
 *
 *
 * @param objId        @b{(input)}  binary tree object Id
 * @param p_value      @b{(output)}  pointer to put the value
 *
 * @returns            E_OK           success
 * @returns            E_FAILED       the call BT_GetFirst was not successed
 * @returns            E_NOT_FOUND    no more entries in the binary tree
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_GetNext(t_Handle objId, void **p_value)
{
    t_BT    *p_BT = (t_BT *)objId;
    t_Value *entry, *parent;
    
    ASSERT(p_BT);
    ASSERT(p_BT->status == BT_STAT_Valid);

    if(p_BT->getValue == NULL)
      return E_FAILED;
        
    if(p_BT->getValue->left)
      entry = p_BT->getValue->left;
    else if(p_BT->getValue->right)
      entry = p_BT->getValue->right;
    else if(p_BT->getValue->parent) 
    {
      entry =  p_BT->getValue;
      parent = entry->parent;
      
      while(parent)
      {
         if((parent->left == entry) && parent->right)
         {
            entry = parent->right;
            break;
         }        

         entry = parent;
         parent = entry->parent;
      } 
      if(!parent)
         return E_NOT_FOUND;
    }
    else
      return E_NOT_FOUND;

    p_BT->getValue = entry;          
    *p_value = GET_VALUE(p_BT, entry);

    return E_OK;
}


/*********************************************************************
 * @purpose           Go through all the entry and call user browse
 *                    function; if user browse function returned FALSE
 *                    delete current entry.
 *
 *
 * @param  objId      @b{(input)}  binary tree object Id
 * @param  f_browse   @b{(input)}  user browse function
 * @param  flag       @b{(input)}  flag to put as a parameter to the browse
 *                                 function
 *
 * @returns           E_OK        success
 * @returns           E_BADPARAM  wrong parameters
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_Browse( t_Handle objId, BROWSEFUNC f_browse, ulng flag)
{
   t_BT    *p_BT = (t_BT *)objId;
   t_Value *parent = 0;
   t_Value *root = 0;
   void    *value;
   e_Err   e;

   ASSERT(p_BT);
   ASSERT(p_BT->status == BT_STAT_Valid);
   ASSERT(f_browse);

   if(!p_BT->valueNmb)
       return E_OK; /* binary tree is empty */

   if(!p_BT->browseInterrupt)
   {
      root = p_BT->root;
      if((e = BT_GetFirst(p_BT, &value)) != E_OK)
         return e;
   }
   else
   {
      value = GET_VALUE(p_BT, p_BT->getValue);
      root = p_BT->rootsave;
      p_BT->browseInterrupt = FALSE;
      e = E_OK;
   }

   while(e == E_OK)
   {
      if(!f_browse((byte *)value, flag))
      {
         /* delete entry */
         
         if(p_BT->getValue->parent)
            parent = p_BT->getValue->parent;
         else if(p_BT->getValue->left)
            parent = p_BT->getValue->left;
         else if(p_BT->getValue->right)
            parent = p_BT->getValue->right;
         else 
            parent = 0;

         BT_Delete(p_BT, GET_KEY(p_BT, p_BT->getValue), 0);
         
         if(!parent)
            p_BT->getValue = p_BT->root;
         else
            p_BT->getValue = parent;
      }
      else
         parent = 0;
      
      if(p_BT->browseInterrupt)
      {
         p_BT->rootsave = root;
         return E_OK;
      }

      e = BT_GetNext(p_BT, &value);
   }

   /* delete the last entry */
   if(parent)
   {
      if(!f_browse(GET_VALUE(p_BT, parent), flag))
         BT_Delete(p_BT, GET_KEY(p_BT, parent), 0);

      if(p_BT->root && (p_BT->root != root))
      {
         if(!f_browse(GET_VALUE(p_BT, p_BT->root), flag))
            BT_Delete(p_BT, GET_KEY(p_BT, p_BT->root), 0);
      }
   }
   return E_OK;
}



/*********************************************************************
 * @purpose          Insert new entry to the binary tree.
 *
 *
 * @param  objId     @b{(input)}  object Id
 * @param  key       @b{(input)}  key pointer
 * @param  value     @b{(input)}  new value
 *
 * @returns          E_OK        success
 * @returns          E_NOMEMORY  failed in memory allocation
 * @returns          E_NOT_FOUND  no match entry found
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_Change( t_Handle objId, byte *key, void *value)
{
   t_BT    *p_BT = (t_BT *)objId;
   t_Value *current;
   int     comp;
   e_Err   e = E_NOT_FOUND;

   ASSERT(p_BT);
   ASSERT(p_BT->status == BT_STAT_Valid);
   ASSERT(p_BT->dataOffset);

   if(!(current = p_BT->root))
      return e;
   
   while(current)
   {
      /* compare keys */
      comp = KEY_COMPARE(p_BT, key, GET_KEY(p_BT, current));

      if(comp == 0) 
      {
         /* change Exactly match entry found */
         current->value = value;
         return E_OK; 
      }
      else 
         current = (comp < 0) ? current->left : current->right;
   }

   return E_NOT_FOUND;
}



/*********************************************************************
 * @purpose           Interrupt BT_Browse running.
 *
 *
 * @param  objId      @b{(input)}  object Id
 *
 * @returns           E_OK        always
 *
 * @notes
 *
 * @end
 * ********************************************************************/
e_Err BT_Interrupt( t_Handle objId)
{
   t_BT    *p_BT = (t_BT *)objId;
   p_BT->browseInterrupt = TRUE;
   return E_OK;
}
/******************end of file**************************************/
