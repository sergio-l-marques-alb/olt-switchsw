/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2010
*
**********************************************************************
*
* @filename sdm_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by 
*           management and the switch database manager (SDM).  
*
* @component 
*
* @comments 
*
* @created March 1, 2010
*
* @author rrice    
* @end
*
**********************************************************************/

#ifndef __SDM_EXPORTS_H_
#define __SDM_EXPORTS_H_


/* Maximum number of characters in a template name, including the 
 * NULL terminator. This is the display
 * string the UI should use when referring to a template. */
#define SDM_TEMPLATE_NAME_LEN  32


/* These values are stored persistently and identify the template in use
 * when a switch boots. Existing values must stay the same in future 
 * releases. If you add a value, add it to the end. If you add a value
 * for a services customer, you may want to assign the new template ID 
 * something other than the next available value to avoid a future 
 * clash with a template ID for another project. */
typedef enum
{
  SDM_TEMPLATE_NONE = 0,
  SDM_TEMPLATE_DUAL_DEFAULT = 1,
  SDM_TEMPLATE_V4_DEFAULT = 2,
  SDM_TEMPLATE_V4_DATA_CENTER = 3
} sdmTemplateId_t;

#endif


