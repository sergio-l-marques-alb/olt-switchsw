/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename dot1x_auth_serv_cfg.h
*
* @purpose Contains prototypes and Data Structures
*          to support the IDAS application
*
* @component IDAS
*
* @comments
*
* @create 11/05/2010
*
* @author msaleem
* @end
*
**********************************************************************/
#ifndef INCLUDE_IDAS_CFG_H
#define INCLUDE_IDAS_CFG_H

#define DOT1X_AUTH_SERV_CFG_FILENAME     "dot1x_auth_serv.cfg"
#define DOT1X_AUTH_SERV_CFG_VER_1        0x1
#define DOT1X_AUTH_SERV_CFG_VER_CURRENT  DOT1X_AUTH_SERV_CFG_VER_1

/*********************************************************************
* @purpose  Build default IDAS config data
*
* @param    ver   @{{input}} Software version of Config Data
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void  dot1xAuthServBuildDefaultConfigData(L7_uint32 ver);

/*********************************************************************
* @purpose  Checks if IDAS user config data has changed
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dot1xAuthServHasDataChanged(void);

/*********************************************************************
* @purpose  Restores  IDAS user config data change Flag
*
* @param    void
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL dot1xAuthServResetDataChanged(void);

/*********************************************************************
* @purpose  Saves IDAS user config file to NVStore
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dot1xAuthServSave(void);
 
/*********************************************************************
* @purpose  Restores IDAS user config file to factore defaults
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FALIURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 dot1xAuthServRestore(void);

#endif  /* end of INCLUDE_IDAS_CFG_H */

