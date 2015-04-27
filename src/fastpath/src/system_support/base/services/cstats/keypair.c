/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    keypair.c
* @purpose     Implements the API's for the management(store/retrieve)
*              of the counter pointers
* @component   stats
* @comments    
* @create      18/06/2007
* @author      YSaritha 
* @end
*             
**********************************************************************/
#include "commdefs.h"           /* lvl7 common definitions   */
#include "datatypes.h"          /* lvl7 data types definition */
#include "osapi.h"              /* lvl7 operating system apis */
#include "statsapi.h"           /* stats public methods */
#include "counter64.h"
#include "collector.h"
#include "presenter.h"
#include "statsconfig.h"
#include "log.h"

L7_RC_t keyPair_init (keyPair ** kp)
{
  L7_ushort16 i;

  for (i = 0; i < COUNTER_FLY_WEIGHT_MAX; i++)
  {
    kp[i] = L7_NULL;
  }
  return L7_SUCCESS;
}

L7_RC_t keyPair_get (keyPair * kp, keyPair ** base)
{
  L7_ushort16 i;

  for (i = 0; i < COUNTER_FLY_WEIGHT_MAX; i++)
  {
    if (base[i] != L7_NULL)
    {
      if ((base[i]->id == kp->id) && (base[i]->key == kp->key))
      {
        if (base[i]->ptr)
        {
          kp->ptr = base[i]->ptr;
          kp->ctype = base[i]->ctype;
          return L7_SUCCESS;
        }
        return L7_FAILURE;
      }
    }
  }
  return L7_FAILURE;
}

L7_RC_t keyPair_set (keyPair * kp, keyPair ** base)
{
  L7_ushort16 i;

  for (i = 0; i < COUNTER_FLY_WEIGHT_MAX; i++)
  {
    if (base[i] == L7_NULL)
    {
      break;
    }
    if (base[i]->id == kp->id && base[i]->key == kp->key)
    {
      base[i]->ptr = kp->ptr;
      base[i]->ctype = kp->ctype;
      return L7_SUCCESS;
    }
  }
  if (i < COUNTER_FLY_WEIGHT_MAX)
  {
    base[i] = (keyPair *) osapiMalloc (L7_STATSMGR_COMPONENT_ID, sizeof (keyPair));
    base[i]->id = kp->id;
    base[i]->key = kp->key;
    base[i]->ptr = kp->ptr;
    base[i]->ctype = kp->ctype;
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}
