
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  registry.c
*
* @purpose   Provide system-wide Registry support routines
*
* @component sysapi
*
* @create 07/27/2000
*
* @author paulq 
*
* @end
*
*********************************************************************/

#include <stdlib.h>                /* for malloc() etc... */
#include <string.h>                /* for memcpy() etc... */

#include "l7_common.h"
#include "log.h"
#include "registry.h"
#include "osapi.h"

typedef struct registry_entity_t
{
  L7_uint32 type;       /* see reg_entry_type */
  L7_uint32 ptr_val;
} REGISTRY_ENTITY;

static REGISTRY_ENTITY *pRegistry;

/**************************************************************************
* @purpose  Initialize the system Registry
*
* @param    none.
*
* @returns  L7_SUCCESS 
*
* @notes    Does minimal initialization
*
* @end
*************************************************************************/

L7_RC_t sysapiRegistryInit(void)
{
  L7_uint32 temp32 = (L7_uint32)0xFFFFFFFF;
  pRegistry = ( REGISTRY_ENTITY * )osapiMalloc(L7_SIM_COMPONENT_ID, (L7_uint32)(LAST_REGISTRY_INDEX*sizeof( REGISTRY_ENTITY )));
  if ( pRegistry == L7_NULLPTR )
  {
    return L7_ERROR;
  }

  /*
  * clear the whole thing.....
  */
  memset(( void * )pRegistry, 0, (size_t)(sizeof( REGISTRY_ENTITY ) * LAST_REGISTRY_INDEX) ); 

  /* Set the heap address to an invalid state in case this is not used by some OS's */
  if (sysapiRegistryPut (OS_HEAPADDR, U32_ENTRY, (void *)&temp32) != L7_SUCCESS )
  {
    LOG_MSG("sysapiRegistryPut failed");
    return L7_FAILURE;
  }
  return L7_SUCCESS;

}   /* end : sysapiRegistryInit */


/**************************************************************************
* @purpose  Pull a value out of the registry
*
* @param   reg_key    @b{(input)} registry key from which to get information
* @param   entry_type @b{(input)} registry key type
* @param   buffer     @b{(output)} user's mem - where to place key information
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE key is out of range or entry type does not match current type of that key
*
* @notes
*
* @end
*************************************************************************/
L7_RC_t sysapiRegistryGet(registry_t reg_key, reg_entry_type entry_type, void *buffer)
{
  REGISTRY_ENTITY *entp;
  if ( reg_key < FIRST_REGISTRY_INDEX || reg_key >= LAST_REGISTRY_INDEX )
  {
    return L7_FAILURE;
  }
  entp = &pRegistry[reg_key - FIRST_REGISTRY_INDEX - 1];
  if ( entp->type != entry_type )
  {
    return L7_FAILURE;
  }
  switch ( entry_type )
  {
  case U8_ENTRY:
    *( L7_uchar8 * )buffer =  ( L7_uchar8 )entp->ptr_val;
    break;

  case U16_ENTRY:
    *( L7_ushort16 * )buffer =  ( L7_ushort16 )entp->ptr_val;
    break;

  case U32_ENTRY:
    *( L7_uint32 * )buffer =  ( L7_uint32 )entp->ptr_val;
    break;

  case MAC_ENTRY:
    memcpy (buffer, (void *)entp->ptr_val, 6);
    break;

  case STR_ENTRY:
    strcpy ((char *)buffer, (char *)entp->ptr_val );
    break;

  case SLOT_ENTRY:
    *( L7_uint32 * )buffer =  ( L7_uint32 )entp->ptr_val;
    break;

  case PORT_ENTRY:
    *( L7_uint32 * )buffer =  ( L7_uint32 )entp->ptr_val;
    break;
  case IGNORE_SVC_PORT:
  case LAST_REG_ENTRY_TYPE:
    return L7_FAILURE;
    break;

  }
  return L7_SUCCESS;

}  /* end: sysapiRegistryGet */


/**************************************************************************
* @purpose  Put a value into the registry
*
* @param reg_key     @b{(input)} registry key to put information
* @param entry_type  @b{(input)} registry key type to set entry to 
* @param *buffer     @b{(output)} ptr to key information to place in registry
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE key is out of range
*
* @notes
*
* @end
*************************************************************************/
L7_RC_t sysapiRegistryPut(registry_t reg_key, reg_entry_type entry_type, void *buffer)
{
  REGISTRY_ENTITY *entp;

  if ( reg_key < FIRST_REGISTRY_INDEX || reg_key >= LAST_REGISTRY_INDEX )
  {
    return L7_FAILURE;
  }
  entp = &pRegistry[reg_key - FIRST_REGISTRY_INDEX - 1];

  entp->type = entry_type;
  switch ( entry_type )
  {
  case U8_ENTRY:
    entp->ptr_val = ( L7_uchar8 ) *( L7_uchar8 * )buffer;
    break;

  case U16_ENTRY:
    entp->ptr_val = ( L7_ushort16 ) *( L7_ushort16 * )buffer;
    break;

  case U32_ENTRY:
    entp->ptr_val = *( L7_uint32 * )buffer;
    break;

  case MAC_ENTRY:
    entp->ptr_val = (L7_uint32)osapiMalloc( L7_SIM_COMPONENT_ID, 6 );
    if ( entp->ptr_val != L7_NULL )
      memcpy ((void *)entp->ptr_val, buffer, 6);
    break;

  case STR_ENTRY:    
    entp->ptr_val = (L7_uint32)osapiMalloc (L7_SIM_COMPONENT_ID, strlen ( (char *)buffer ) + 1 );
    if ( entp->ptr_val != L7_NULL )
      strcpy ( (char *)entp->ptr_val, (char *)buffer );
    break;

  case SLOT_ENTRY:    
    entp->ptr_val = *( L7_uint32 * )buffer;
    break;

  case PORT_ENTRY:    
    entp->ptr_val = *( L7_uint32 * )buffer;
    break;
  case IGNORE_SVC_PORT:
  case LAST_REG_ENTRY_TYPE:
    return L7_FAILURE;
    break;
  }

  return L7_SUCCESS;
}  /* end: sysapiRegistryPut */ 

