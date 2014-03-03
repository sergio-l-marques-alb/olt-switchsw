/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    24/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/

#ifdef _COMPILE_AS_BINARY_ //All methods in this file should not be compiled if we are compiling as a lib

#include "ptin_mgmd_cfg_api.h"
#include "logger.h"

/**
* @purpose Enable/Disable IGMP packets to be captures by the HW
*  
* @param  admin[in] : Admin [0-disable / 1-enable]
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_cfg_igmp_admin_set(uint8 admin)
{
  _UNUSED_(admin);
  return SUCCESS;
}

/**
* @purpose Enable/Disable MLD packets to be captures by the HW
*  
* @param  admin[in] : Admin [0-disable / 1-enable]
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_cfg_mld_admin_set(uint8 admin)
{
  _UNUSED_(admin);
  return SUCCESS;
}

#endif //_COMPILE_AS_BINARY_


