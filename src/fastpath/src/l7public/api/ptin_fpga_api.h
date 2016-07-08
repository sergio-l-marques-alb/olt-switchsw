/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2014-2014
*
**********************************************************************
*
* @filename ptin_fpga_api.h
*
* @purpose API to get the status of the CPLD  register on the FPGA
*
* @component fpga
*
* @comments none
* 
* @create    06/06/2014
*
* @author    Marcio Daniel Melo
* 
* @end
**********************************************************************/
#ifndef _PTIN_FPGA_API_H
#define _PTIN_FPGA_API_H

#include "ptin_globaldefs.h"

typedef enum
{
  PTIN_FPGA_STANDBY_MATRIX=0,
  PTIN_FPGA_ACTIVE_MATRIX
} ptin_fpga_matrix_type_t;

/* OLT10 */
#if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
#include "ptin_fpga_olt1t0.h"

/* OLT10F */
#elif (PTIN_BOARD == PTIN_BOARD_OLT1T0F)
#include "ptin_fpga_olt1t0f.h"

/* OLT7-8CH */
#elif (PTIN_BOARD == PTIN_BOARD_OLT7_8CH_B)
#include "ptin_fpga_olt7-8ch-b.h"

/* TOLT8G */
#elif (PTIN_BOARD == PTIN_BOARD_TOLT8G)
#include "ptin_fpga_tolt8g.h"

/* TG16G */
#elif (PTIN_BOARD == PTIN_BOARD_TG16G)
#include "ptin_fpga_tg16g.h"

/* TG16GF */
#elif (PTIN_BOARD == PTIN_BOARD_TG16GF)
#include "ptin_fpga_tg16gf.h"

/* TG4G */
#elif (PTIN_BOARD == PTIN_BOARD_TG4G)
#include "ptin_fpga_tg4g.h"

/* TA48GE */
#elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
#include "ptin_fpga_ta48ge.h"

/* CXO160G */
#elif (PTIN_BOARD == PTIN_BOARD_CXO160G)
#include "ptin_fpga_cxo160g.h"

/* OLT360 Matrix card */
#elif (PTIN_BOARD == PTIN_BOARD_CXP360G)
#include "ptin_fpga_cxo360g.h"

/* OLT1T3 Matrix card */
#elif (PTIN_BOARD == PTIN_BOARD_CXO640G)
#include "ptin_fpga_cxo640g.h"

#endif


#if PTIN_BOARD_IS_MATRIX
#define is_matrix_protection() (CPLD_SLOT_ID_GET() != 0)   /* To know if we are in protection matrix */
#define matrix_board_version() ((CPLD_ID_GET() == CPLD_ID_CXO640G_V1) ? 1 : 2)
#endif  // PTIN_BOARD_IS_MATRIX


/**
 * Maps FPGA and PLD registers map
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_fpga_init(void);


#ifdef MAP_CPLD

#if (PTIN_BOARD_IS_MATRIX)

/**************************************************************************
*
* @purpose  Verify if this Matrix is Currently in Active State. 
*           (Only for Matrix board)
* 
* @param    void
*
* @returns  TRUE or FALSE
*
* @comments If this Card is not a Matrix it is returned 0.
*
* @end
*
*************************************************************************/
L7_uint8 ptin_fpga_mx_is_matrixactive(void);

/**************************************************************************
*
* @purpose  Verify if this Matrix is Currently in Active State.
*           (Only for Matrix board)
*
* @param    void
*
* @returns  TRUE or FALSE
*
* @comments 
*
* @end
*
*************************************************************************/
L7_uint8 ptin_fpga_mx_is_matrixactive_rt(void);

/**
 * Check if current Matrix is the Working one (slot 1) 
 * (Only for Matrix board)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_mx_is_matrix_in_workingslot(void);
#endif

#if (PTIN_BOARD_IS_LINECARD)
/**
 * Check if Active Matrix is in Working slot 
 * (Only for linecards)
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_lc_is_matrixactive_in_workingslot(void);
#endif

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

/**
 * Get slot position of Active Matrix 
 * (For all cards) 
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_matrixActive_slot(void);

/**
 * Get slot position of Inactive Matrix 
 * (For all cards) 
 * 
 * @return L7_uint8 : L7_TRUE / L7_FALSE
 */
L7_uint8 ptin_fpga_matrixInactive_slot(void);

#endif//(PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

#endif//MAP_CPLD

/**
 * Get slot id 
 * (For all cards)
 * 
 * @return L7_uint8 : slot id
 */
L7_uint8 ptin_fpga_board_slot_get(void);

#if (PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

/**************************************************************************
*
* @purpose  Verify if this Matrix is Currently in Active State.
*           (Only for Matrix board)
*
* @param    void
*
* @returns  TRUE or FALSE
*
* @comments 
*
* @end
*
*************************************************************************/
L7_uint8 ptin_fpga_mx_get_matrixactive(void);

/**
 * Get active/backup matrix slot id.
 * 
 * @param matrixType : Matrix type (1-active; 0-backup) 
 * 
 * @return slotId  : Slot Id of the given  matrix type
 *  
 * @note When this method is used in a linecard, the matrixType parameter is ignored and the slotId returned always belongs to the active matrix 
 */
L7_uint8 ptin_fpga_matrix_slotid_get(ptin_fpga_matrix_type_t matrixType);

/**
 * Get active/backup matrix IP address.
 * 
 * @param matrixType : Matrix type (1-active; 0-backup)
 * @param 
 * 
 * @return ipAddr    : IP address of the given matrix type
 *  
 * @note When this method is used in a linecard, the matrixType parameter is ignored and the IP address returned always belongs to the active matrix 
 */
L7_uint32 ptin_fpga_matrix_ipaddr_get(ptin_fpga_matrix_type_t matrixType);

/**
 * Get slot IP address.
 * 
 * @param slotId [in] : Slot Id 
 * @param ipAddr [out]: IP Address  
 * 
 * @return rc    : L7_SUCESS 
 *                 L7_FAILURE 
 *  
 */
L7_RC_t ptin_fpga_slot_ip_addr_get(L7_uint8 slotId, L7_uint32 *ipAddr);

/**
 * Get Board Type
 * 
 * 
 * @return boardType    : PTIN_BOARD_CXO160G 
 *                        PTIN_BOARD_CXO640G 
 *  
 */
L7_uint32 ptin_fpga_board_get(void);
#endif//(PTIN_BOARD_IS_MATRIX || PTIN_BOARD_IS_LINECARD)

#endif //_PTIN_FPGA_API_H
