/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename edb_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __EDB_EXPORTS_H_
#define __EDB_EXPORTS_H_


/******************************************************************/
/*************       Start Entity DB types and defines *********/
/******************************************************************/
typedef enum
{
  L7_EDB_MESSAGE_TYPE_UNIT_JOIN = 0,
  L7_EDB_MESSAGE_TYPE_UNIT_LEAVE,
  L7_EDB_MESSAGE_TYPE_SLOT_CREATE,
  L7_EDB_MESSAGE_TYPE_SLOT_DELETE,
  L7_EDB_MESSAGE_TYPE_CARD_PLUGIN,
  L7_EDB_MESSAGE_TYPE_CARD_UNPLUG,
  L7_EDB_MESSAGE_TYPE_PORT_CREATE,
  L7_EDB_MESSAGE_TYPE_PORT_DELETE
} L7_EDB_MESSAGE_TYPE_t;

/* Source: Textual-Convention PhysicalClass in RFC 2737 */

/*An enumerated value which provides an indication of the
general hardware type of a particular physical entity.
There are no restrictions as to the number of
entPhysicalEntries of each entPhysicalClass, which must be
instantiated by an agent.*/
typedef enum
{
  /* The enumeration 'other' is applicable if the physical entity
  class is known, but does not match any of the supported
  values. */
  L7_EDB_PHYSICAL_CLASS_OTHER = 1,

  /* The enumeration 'unknown' is applicable if the physical
  entity class is unknown to the agent. */
  L7_EDB_PHYSICAL_CLASS_UNKNOWN = 2,

  /* The enumeration 'chassis' is applicable if the physical
  entity class is an overall container for networking
  equipment.  Any class of physical entity except a stack may
  be contained within a chassis, and a chassis may only be
  contained within a stack. */
  L7_EDB_PHYSICAL_CLASS_CHASSIS = 3,

  /* The enumeration 'backplane' is applicable if the physical
  entity class is some sort of device for aggregating and
  forwarding networking traffic, such as a shared backplane in
  a modular ethernet switch.  Note that an agent may model a
  backplane as a single physical entity, which is actually
  implemented as multiple discrete physical components (within
  a chassis or stack). */
  L7_EDB_PHYSICAL_CLASS_BACKPLANE = 4,

  /* The enumeration 'container' is applicable if the physical
  entity class is capable of containing one or more removable
  physical entities, possibly of different types. For example,
  each (empty or full) slot in a chassis will be modeled as a
  container. Note that all removable physical entities should
  be modeled within a container entity, such as field-
  replaceable modules, fans, or power supplies.  Note that all
  known containers should be modeled by the agent, including
  empty containers. */
  L7_EDB_PHYSICAL_CLASS_CONTAINER = 5,

  /* The enumeration 'powerSupply' is applicable if the physical
  entity class is a power-supplying component. */
  L7_EDB_PHYSICAL_CLASS_POWERSUPPLY = 6,

  /* The enumeration 'fan' is applicable if the physical entity
  class is a fan or other heat-reduction component. */
  L7_EDB_PHYSICAL_CLASS_FAN = 7,

  /* The enumeration 'sensor' is applicable if the physical
  entity class is some sort of sensor, such as a temperature
  sensor within a router chassis. */
  L7_EDB_PHYSICAL_CLASS_SENSOR = 8,

  /* The enumeration 'module' is applicable if the physical
  entity class is some sort of self-contained sub-system.  If
  it is removable, then it should be modeled within a
  container entity, otherwise it should be modeled directly
  within another physical entity (e.g., a chassis or another
  module). */
  L7_EDB_PHYSICAL_CLASS_MODULE = 9,

  /* The enumeration 'port' is applicable if the physical entity
  class is some sort of networking port, capable of receiving
  and/or transmitting networking traffic. */
  L7_EDB_PHYSICAL_CLASS_PORT = 10,

  /* The enumeration 'stack' is applicable if the physical entity
  class is some sort of super-container (possibly virtual),
  intended to group together multiple chassis entities.  A
  stack may be realized by a 'virtual' cable, a real
  interconnect cable, attached to multiple chassis, or may in
  fact be comprised of multiple interconnect cables. A stack
  should not be modeled within any other physical entities,
  but a stack may be contained within another stack.  Only
  chassis entities should be contained within a stack. */
  L7_EDB_PHYSICAL_CLASS_STACK = 11
} L7_EDB_PHYSICAL_CLASS_t;

typedef enum
{
  L7_EDB_OBJECT_TYPE_UNKNOWN = 0,
  L7_EDB_OBJECT_TYPE_STACK,
  L7_EDB_OBJECT_TYPE_UNIT,
  L7_EDB_OBJECT_TYPE_SLOT,
  L7_EDB_OBJECT_TYPE_CARD,
  L7_EDB_OBJECT_TYPE_PORT,
  L7_EDB_OBJECT_TYPE_POWERSUPPLY,
  L7_EDB_OBJECT_TYPE_FAN
} L7_EDB_OBJECT_TYPE_t;


/******************************************************************/
/*************       Start Entity DB types and defines *********/
/******************************************************************/



/******************** conditional Override *****************************/

#ifdef INCLUDE_EDB_EXPORTS_OVERRIDES
#include "edb_exports_overrides.h"
#endif


#endif /* __EDB_EXPORTS_H_*/
